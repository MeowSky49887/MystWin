#pragma once

#include <napi.h>
#include <windows.h>

namespace mystwin {
	static int WM_SPAWN_WORKER = 0x052C;

	static HWND workerw = NULL;
	static HWND shelldll = NULL;

	BOOL CALLBACK FindWorkerW(HWND, LPARAM);

	Napi::Value AttachAsWallpaperExport(const Napi::CallbackInfo& info);
	Napi::Value AttachAsDesktopExport(const Napi::CallbackInfo& info);
	Napi::Value DetachWindowExport(const Napi::CallbackInfo& info);
	Napi::Value SendToBackExport(const Napi::CallbackInfo& info);
	Napi::Value BringToFrontExport(const Napi::CallbackInfo& info);
	Napi::Value MoveWindowExport(const Napi::CallbackInfo& info);
	Napi::Value ResizeWindowExport(const Napi::CallbackInfo& info);
	Napi::Value SetOpacityExport(const Napi::CallbackInfo& info);
	Napi::Value ToggleOverlayExport(const Napi::CallbackInfo& info);
	Napi::Value ToggleFrameExport(const Napi::CallbackInfo& info);
}
