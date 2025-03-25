#ifndef CONVERTER_H
#define CONVERTER_H

#include <napi.h>
#include <string>

typedef uint8_t byte;

struct ConverterData {
    napi_env env;
    napi_deferred deferred;
    byte* in_data;
    size_t in_size;
    int width;
    int height;
    float quality;
    uint32_t frame;
    byte* result_buffer;
    size_t result_size;
    std::string error_msg;
    bool success;
};

ConverterData* create_converter_data(napi_env env, const byte* in_data, size_t size);
void destroy_converter_data(ConverterData* data);
bool convert_to_png_sync(ConverterData* data);

#endif // CONVERTER_H