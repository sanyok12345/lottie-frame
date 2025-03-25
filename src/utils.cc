#include "utils.h"

bool has_own_property(napi_env env, napi_value obj, const char* key) {
    napi_value key_value;
    napi_status status = napi_create_string_utf8(env, key, NAPI_AUTO_LENGTH, &key_value);
    if (status != napi_ok) return false;

    bool has_prop = false;
    status = napi_has_own_property(env, obj, key_value, &has_prop);
    if (status != napi_ok) return false;
    return has_prop;
}