#ifndef EXPORTS_H
#define EXPORTS_H

#include <napi.h>
#include "converter.h"

void execute_work(napi_env env, void* data);
void complete_work(napi_env env, napi_status status, void* data);
napi_value exportFrame(napi_env env, napi_callback_info info);
napi_value exportFrameSync(napi_env env, napi_callback_info info);

#endif // EXPORTS_H