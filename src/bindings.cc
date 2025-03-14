#include <assert.h>
#include "mystwin.h"
#include "napi.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
	exports.Set(Napi::String::New(env, "attachAsWallpaper"), Napi::Function::New(env, mystwin::AttachAsWallpaperExport));
	exports.Set(Napi::String::New(env, "attachAsDesktop"), Napi::Function::New(env, mystwin::AttachAsDesktopExport));
	exports.Set(Napi::String::New(env, "detachWindow"), Napi::Function::New(env, mystwin::DetachWindowExport));
	exports.Set(Napi::String::New(env, "sendToBack"), Napi::Function::New(env, mystwin::SendToBackExport));
	exports.Set(Napi::String::New(env, "bringToFront"), Napi::Function::New(env, mystwin::BringToFrontExport));
	exports.Set(Napi::String::New(env, "moveWindow"), Napi::Function::New(env, mystwin::BringToFrontExport));
	exports.Set(Napi::String::New(env, "resizeWindow"), Napi::Function::New(env, mystwin::BringToFrontExport));
	exports.Set(Napi::String::New(env, "setOpacity"), Napi::Function::New(env, mystwin::BringToFrontExport));
	exports.Set(Napi::String::New(env, "toggleOverlay"), Napi::Function::New(env, mystwin::BringToFrontExport));
	exports.Set(Napi::String::New(env, "toggleFrame"), Napi::Function::New(env, mystwin::BringToFrontExport));
	return exports;
}

NODE_API_MODULE(addon, Init)
