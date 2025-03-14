#include <assert.h>
#include "mystwin.h"
#include "napi.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
	exports.Set(Napi::String::New(env, "attachAsWallpaper"), Napi::Function::New(env, mystwin::AttachAsWallpaperExport));
	exports.Set(Napi::String::New(env, "attachAsDesktop"), Napi::Function::New(env, mystwin::AttachAsDesktopExport));
	exports.Set(Napi::String::New(env, "detachWindow"), Napi::Function::New(env, mystwin::DetachWindowExport));
	return exports;
}

NODE_API_MODULE(addon, Init)