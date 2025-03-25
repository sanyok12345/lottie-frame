#ifndef UTILS_H
#define UTILS_H

#include <napi.h>

#define THROW_ERROR(env, msg) napi_throw_error(env, nullptr, msg); return nullptr
#define THROW_ERROR_NO_RETURN(env, msg) napi_throw_error(env, nullptr, msg); return
#define CHECK_STATUS(env, status, msg) if (status != napi_ok) { THROW_ERROR(env, msg); }

bool has_own_property(napi_env env, napi_value obj, const char* key);

#endif // UTILS_H