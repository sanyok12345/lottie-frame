#include "png_writer.h"
#include <cstring>

#define lp_COLOR_BYTES 4

bool write_png(ConverterData* data, uint32_t* buffer) {
    struct PngBuffer {
        byte* buffer;
        size_t size;
        size_t capacity;
    } png_data = {nullptr, 0, 0};

    auto write_func = [](png_structp png_ptr, png_bytep data, png_size_t length) {
        PngBuffer* buf = (PngBuffer*)png_get_io_ptr(png_ptr);
        if (buf->size + length > buf->capacity) {
            size_t new_capacity = buf->capacity == 0 ? 1024 : buf->capacity * 2;
            while (new_capacity < buf->size + length) new_capacity *= 2;
            byte* new_buffer = (byte*)realloc(buf->buffer, new_capacity);
            if (!new_buffer) {
                png_error(png_ptr, "Failed to allocate memory for PNG");
                return;
            }
            buf->buffer = new_buffer;
            buf->capacity = new_capacity;
        }
        memcpy(buf->buffer + buf->size, data, length);
        buf->size += length;
    };

    auto flush_func = [](png_structp png_ptr) {};

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        data->error_msg = "Failed to create PNG write struct";
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        data->error_msg = "Failed to create PNG info struct";
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        free(png_data.buffer);
        data->error_msg = "Error during PNG creation";
        return false;
    }

    png_set_write_fn(png_ptr, &png_data, write_func, flush_func);
    png_set_IHDR(png_ptr, info_ptr, data->width, data->height, 8, PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    int compression_level = (int)((1.0f - data->quality) * 9);
    png_set_compression_level(png_ptr, compression_level);

    byte** row_pointers = (byte**)malloc(data->height * sizeof(byte*));
    if (!row_pointers) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        free(png_data.buffer);
        data->error_msg = "Failed to allocate row pointers";
        return false;
    }

    byte* byte_buffer_raw = reinterpret_cast<byte*>(buffer);
    for (int y = 0; y < data->height; y++) {
        row_pointers[y] = (byte*)malloc(data->width * lp_COLOR_BYTES);
        if (!row_pointers[y]) {
            for (int i = 0; i < y; i++) free(row_pointers[i]);
            free(row_pointers);
            png_destroy_write_struct(&png_ptr, &info_ptr);
            free(png_data.buffer);
            data->error_msg = "Failed to allocate row buffer";
            return false;
        }
        for (int x = 0; x < data->width; x++) {
            byte b = *byte_buffer_raw++, g = *byte_buffer_raw++, r = *byte_buffer_raw++, a = *byte_buffer_raw++;
            row_pointers[y][x * 4 + 0] = r;
            row_pointers[y][x * 4 + 1] = g;
            row_pointers[y][x * 4 + 2] = b;
            row_pointers[y][x * 4 + 3] = a;
        }
    }

    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

    for (int y = 0; y < data->height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    data->result_buffer = png_data.buffer;
    data->result_size = png_data.size;
    return true;
}