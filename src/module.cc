#include <napi.h>
#include "exports.h"

napi_value init(napi_env env, napi_value exports) {
    napi_status status;

    napi_value exportFrame_fn, exportFrameSync_fn;
    status = napi_create_function(env, "exportFrame", NAPI_AUTO_LENGTH, exportFrame, nullptr, &exportFrame_fn);
    if (status != napi_ok) return nullptr;

    status = napi_create_function(env, "exportFrameSync", NAPI_AUTO_LENGTH, exportFrameSync, nullptr, &exportFrameSync_fn);
    if (status != napi_ok) return nullptr;

    status = napi_set_named_property(env, exports, "exportFrame", exportFrame_fn);
    if (status != napi_ok) return nullptr;

    status = napi_set_named_property(env, exports, "exportFrameSync", exportFrameSync_fn);
    if (status != napi_ok) return nullptr;

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init)