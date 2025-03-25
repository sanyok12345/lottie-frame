#include "converter.h"
#include "animation.h"
#include "png_writer.h"
#include <cstring>

#define lp_COLOR_BYTES 4
#define li_MAX_DIMENSION 4096
#define DEFAULT_QUALITY 100
#define DEFAULT_WIDTH 100
#define DEFAULT_HEIGHT 100
#define DEFAULT_FRAME 0

ConverterData* create_converter_data(napi_env env, const byte* in_data, size_t size) {
    ConverterData* data = new ConverterData();

    data->env = env;
    data->deferred = nullptr;
    data->in_data = (byte*)malloc(size);
    if (!data->in_data) {
        delete data;
        return nullptr;
    }
    memcpy(data->in_data, in_data, size);
    data->in_size = size;
    data->width = DEFAULT_WIDTH;
    data->height = DEFAULT_HEIGHT;
    data->quality = DEFAULT_QUALITY / 100.0f;
    data->frame = DEFAULT_FRAME;
    data->result_buffer = nullptr;
    data->result_size = 0;
    data->success = false;
    data->error_msg = "";
    return data;
}

void destroy_converter_data(ConverterData* data) {
    if (data) {
        if (data->in_data) {
            free(data->in_data);
        }
        if (data->result_buffer) {
            free(data->result_buffer);
        }
        delete data;
    }
}

bool convert_to_png_sync(ConverterData* data) {
    std::string json_data(reinterpret_cast<char*>(data->in_data), data->in_size);
    std::unique_ptr<rlottie::Animation> animation = load_animation_from_data(json_data);
    if (!animation) {
        data->error_msg = "Failed to load Lottie animation from provided JSON data";
        return false;
    }

    size_t frame_count = get_total_frames(animation);
    if (data->frame >= frame_count) {
        data->error_msg = "Frame index out of range (total frames: " + std::to_string(frame_count) + ")";
        return false;
    }

    std::unique_ptr<uint32_t[]> buffer = std::unique_ptr<uint32_t[]>(new (std::nothrow) uint32_t[data->width * data->height]);
    if (!buffer) {
        data->error_msg = "Failed to allocate frame buffer (width: " + std::to_string(data->width) + ", height: " + std::to_string(data->height) + ")";
        return false;
    }

    rlottie::Surface surface(buffer.get(), data->width, data->height, data->width * lp_COLOR_BYTES);
    render_frame_sync(animation, data->frame, surface);

    bool success = write_png(data, buffer.get());
    return success;
}