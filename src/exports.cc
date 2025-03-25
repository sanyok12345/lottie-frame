#include "converter.h"
#include "utils.h"

#define li_MAX_DIMENSION 4096

void execute_work(napi_env env, void* data) {
    ConverterData* converter_data = static_cast<ConverterData*>(data);
    converter_data->success = convert_to_png_sync(converter_data);
}

void complete_work(napi_env env, napi_status status, void* data) {
    ConverterData* converter_data = static_cast<ConverterData*>(data);
    napi_value result;

    if (converter_data->success) {
        status = napi_create_buffer_copy(env, converter_data->result_size, converter_data->result_buffer, nullptr, &result);
        if (status != napi_ok) {
            THROW_ERROR_NO_RETURN(env, "Failed to create result buffer");
        }
        status = napi_resolve_deferred(env, converter_data->deferred, result);
        if (status != napi_ok) {
            THROW_ERROR_NO_RETURN(env, "Failed to resolve promise");
        }
    } else {
        napi_value error;
        status = napi_create_string_utf8(env, converter_data->error_msg.c_str(), NAPI_AUTO_LENGTH, &error);
        if (status != napi_ok) {
            THROW_ERROR_NO_RETURN(env, "Failed to create error string");
        }
        status = napi_reject_deferred(env, converter_data->deferred, error);
        if (status != napi_ok) {
            THROW_ERROR_NO_RETURN(env, "Failed to reject promise");
        }
    }

    destroy_converter_data(converter_data);
}

napi_value exportFrame(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2];
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    CHECK_STATUS(env, status, "Failed to get callback info");

    if (argc < 1) {
        THROW_ERROR(env, "Expected at least 1 argument: Lottie JSON buffer");
    }

    byte* in_data;
    size_t in_size;
    status = napi_get_buffer_info(env, args[0], (void**)&in_data, &in_size);
    CHECK_STATUS(env, status, "Failed to get buffer info from Lottie JSON argument");

    ConverterData* data = create_converter_data(env, in_data, in_size);
    if (!data) {
        THROW_ERROR(env, "Failed to allocate converter data");
    }

    if (argc >= 2 && args[1] != nullptr) {
        napi_valuetype arg_type;
        status = napi_typeof(env, args[1], &arg_type);
        CHECK_STATUS(env, status, "Failed to get type of options argument");

        if (arg_type == napi_object) {
            napi_value prop;

            if (has_own_property(env, args[1], "frame")) {
                status = napi_get_named_property(env, args[1], "frame", &prop);
                if (status == napi_ok) {
                    uint32_t frame;
                    status = napi_get_value_uint32(env, prop, &frame);
                    if (status != napi_ok) {
                        destroy_converter_data(data);
                        THROW_ERROR(env, "Frame must be a valid integer");
                    }
                    data->frame = frame;
                }
            }

            if (has_own_property(env, args[1], "width")) {
                status = napi_get_named_property(env, args[1], "width", &prop);
                if (status == napi_ok) {
                    uint32_t width;
                    status = napi_get_value_uint32(env, prop, &width);
                    if (status != napi_ok) {
                        destroy_converter_data(data);
                        THROW_ERROR(env, "Width must be a valid integer");
                    }
                    if (width == 0 || width > li_MAX_DIMENSION) {
                        destroy_converter_data(data);
                        THROW_ERROR(env, "Width must be between 1 and 4096");
                    }
                    data->width = width;
                }
            }

            if (has_own_property(env, args[1], "height")) {
                status = napi_get_named_property(env, args[1], "height", &prop);
                if (status == napi_ok) {
                    uint32_t height;
                    status = napi_get_value_uint32(env, prop, &height);
                    if (status != napi_ok) {
                        destroy_converter_data(data);
                        THROW_ERROR(env, "Height must be a valid integer");
                    }
                    if (height == 0 || height > li_MAX_DIMENSION) {
                        destroy_converter_data(data);
                        THROW_ERROR(env, "Height must be between 1 and 4096");
                    }
                    data->height = height;
                }
            }

            if (has_own_property(env, args[1], "quality")) {
                status = napi_get_named_property(env, args[1], "quality", &prop);
                if (status == napi_ok) {
                    uint32_t quality;
                    status = napi_get_value_uint32(env, prop, &quality);
                    if (status != napi_ok) {
                        destroy_converter_data(data);
                        THROW_ERROR(env, "Quality must be a valid integer");
                    }
                    if (quality < 1 || quality > 100) {
                        destroy_converter_data(data);
                        THROW_ERROR(env, "Quality must be between 1 and 100");
                    }
                    data->quality = quality / 100.0f;
                }
            }
        } else if (arg_type != napi_undefined && arg_type != napi_null) {
            destroy_converter_data(data);
            THROW_ERROR(env, "Options argument must be an object");
        }
    }

    napi_deferred deferred;
    napi_value promise;
    status = napi_create_promise(env, &deferred, &promise);
    CHECK_STATUS(env, status, "Failed to create promise");

    data->deferred = deferred;

    napi_async_work work;
    napi_value resource_name;
    status = napi_create_string_utf8(env, "LottieExportFrame", NAPI_AUTO_LENGTH, &resource_name);
    CHECK_STATUS(env, status, "Failed to create resource name");
    status = napi_create_async_work(env, nullptr, resource_name, execute_work, complete_work, data, &work);
    CHECK_STATUS(env, status, "Failed to create async work");
    status = napi_queue_async_work(env, work);
    CHECK_STATUS(env, status, "Failed to queue async work");

    return promise;
}

napi_value exportFrameSync(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2];
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    CHECK_STATUS(env, status, "Failed to get callback info");

    if (argc < 1) {
        THROW_ERROR(env, "Expected at least 1 argument: Lottie JSON buffer");
    }

    byte* in_data;
    size_t in_size;
    status = napi_get_buffer_info(env, args[0], (void**)&in_data, &in_size);
    CHECK_STATUS(env, status, "Failed to get buffer info from Lottie JSON argument");

    ConverterData* data = create_converter_data(env, in_data, in_size);
    if (!data) {
        THROW_ERROR(env, "Failed to allocate converter data");
    }

    if (argc >= 2 && args[1] != nullptr) {
        napi_valuetype arg_type;
        status = napi_typeof(env, args[1], &arg_type);
        CHECK_STATUS(env, status, "Failed to get type of options argument");

        if (arg_type == napi_object) {
            napi_value prop;

            if (has_own_property(env, args[1], "frame")) {
                status = napi_get_named_property(env, args[1], "frame", &prop);
                if (status == napi_ok) {
                    uint32_t frame;
                    status = napi_get_value_uint32(env, prop, &frame);
                    if (status != napi_ok) {
                        destroy_converter_data(data);
                        THROW_ERROR(env, "Frame must be a valid integer");
                    }
                    data->frame = frame;
                }
            }

            if (has_own_property(env, args[1], "width")) {
                status = napi_get_named_property(env, args[1], "width", &prop);
                if (status == napi_ok) {
                    uint32_t width;
                    status = napi_get_value_uint32(env, prop, &width);
                    if (status != napi_ok) {
                        destroy_converter_data(data);
                        THROW_ERROR(env, "Width must be a valid integer");
                    }
                    if (width == 0 || width > li_MAX_DIMENSION) {
                        destroy_converter_data(data);
                        THROW_ERROR(env, "Width must be between 1 and 4096");
                    }
                    data->width = width;
                }
            }

            if (has_own_property(env, args[1], "height")) {
                status = napi_get_named_property(env, args[1], "height", &prop);
                if (status == napi_ok) {
                    uint32_t height;
                    status = napi_get_value_uint32(env, prop, &height);
                    if (status != napi_ok) {
                        destroy_converter_data(data);
                        THROW_ERROR(env, "Height must be a valid integer");
                    }
                    if (height == 0 || height > li_MAX_DIMENSION) {
                        destroy_converter_data(data);
                        THROW_ERROR(env, "Height must be between 1 and 4096");
                    }
                    data->height = height;
                }
            }

            if (has_own_property(env, args[1], "quality")) {
                status = napi_get_named_property(env, args[1], "quality", &prop);
                if (status == napi_ok) {
                    uint32_t quality;
                    status = napi_get_value_uint32(env, prop, &quality);
                    if (status != napi_ok) {
                        destroy_converter_data(data);
                        THROW_ERROR(env, "Quality must be a valid integer");
                    }
                    if (quality < 1 || quality > 100) {
                        destroy_converter_data(data);
                        THROW_ERROR(env, "Quality must be between 1 and 100");
                    }
                    data->quality = quality / 100.0f;
                }
            }
        } else if (arg_type != napi_undefined && arg_type != napi_null) {
            destroy_converter_data(data);
            THROW_ERROR(env, "Options argument must be an object");
        }
    }

    if (!convert_to_png_sync(data)) {
        std::string msg = data->error_msg;
        destroy_converter_data(data);
        THROW_ERROR(env, msg.c_str());
    }

    napi_value result;
    status = napi_create_buffer_copy(env, data->result_size, data->result_buffer, nullptr, &result);
    CHECK_STATUS(env, status, "Failed to create result buffer");

    destroy_converter_data(data);
    return result;
}