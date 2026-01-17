#include "mystwin.h"

struct WindowRectCache {
	RECT rect;
	HWND parent;
};
std::unordered_map<HWND, WindowRectCache> cacheMap;

bool mystwin::Init(const Napi::Env env) {
    auto progman = FindWindow(L"Progman", NULL);
    auto result = SendMessageTimeout(progman, WM_SPAWN_WORKER, 0, 0, SMTO_NORMAL, 1000, NULL);

	if (!result) {
		Napi::TypeError::New(env, "Cannot send message timeout.").ThrowAsJavaScriptException();
		return false;
	}

	EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        shelldll = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);
        if (shelldll) {
            *reinterpret_cast<HWND*>(lParam) = FindWindowEx(NULL, hwnd, L"WorkerW", NULL);
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&workerw));

	if (shelldll == NULL) {
		shelldll = FindWindowEx(progman, NULL, L"SHELLDLL_DefView", NULL);
	}
	if (workerw == NULL) {
		workerw = FindWindowEx(progman, NULL, L"WorkerW", NULL);
	}

	return shelldll != NULL && workerw != NULL;
}

Napi::Value mystwin::Refresh(const Napi::CallbackInfo& info, std::string action) {
	auto env = info.Env();

	if (!info[0].IsBuffer()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();
	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	if (!Init(env)) {
		Napi::TypeError::New(env, "System Windows not found.").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto& cache = cacheMap[hwnd];

	RECT rect;
	GetWindowRect(hwnd, &rect);

	HWND insert = NULL;

	UINT flags = SWP_NOZORDER;

	int xPos = rect.left;
	int yPos = rect.top;
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	uint32_t opacityValue = 255;

	LONG style = GetWindowLong(hwnd, GWL_STYLE);
	LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

	style |= WS_TILEDWINDOW;
	exStyle |= WS_EX_LAYERED;

	if (info.Length() == 1) {
		if (action == "Close") {
			DestroyWindow(hwnd);\
			return env.Null();
		} else if (action == "Minimize") {
			ShowWindow(hwnd, SW_MINIMIZE);
			return env.Null();
		} else if (action == "Maximize") {
			ShowWindow(hwnd, SW_MAXIMIZE);
			return env.Null();
		} else if (action == "AttachAsWallpaper") {
			cache.parent = workerw;
		} else if (action == "AttachAsDesktop") {
			cache.parent = shelldll;
		} else if (action == "AttachToTop") {
			insert = HWND_TOPMOST;
			flags = NULL;
		} else if (action == "DetachWindow") {
			insert = HWND_NOTOPMOST;
			flags = NULL;
			cache.parent = NULL;
		} else if (action == "SendToBack") {
			insert = HWND_BOTTOM;
			flags = NULL;
		} else if (action == "BringToFront") {
			insert = HWND_TOP;
			flags = NULL;
		}
	} else if (info.Length() == 2) {
		if (action == "SetOpacity") {
			opacityValue = std::clamp<uint32_t>(info[1].As<Napi::Number>().Uint32Value(), 0u, 255u);
		} else {
			bool enable = info[1].As<Napi::Boolean>().Value();
			if (action == "ToggleFrame") {
				if (enable) {
					style |= (WS_CAPTION | WS_THICKFRAME | WS_SYSMENU);
				} else {
					style &= ~(WS_CAPTION | WS_THICKFRAME | WS_SYSMENU);
				}
			} else if (action == "ToggleTaskBar") {
				if (enable) {
					exStyle |= WS_EX_TOOLWINDOW;
				} else {
					exStyle &= ~WS_EX_TOOLWINDOW;
				}
			} else if (action == "ToggleOverlay") {
				if (enable) {
					exStyle |= WS_EX_TRANSPARENT;
				} else {
					exStyle &= ~WS_EX_TRANSPARENT;
				}
			} else if (action == "ToggleFullScreen") {
				if (enable) {
					GetWindowRect(hwnd, &cache.rect);
					MONITORINFO mi = { sizeof(mi) };
					if (GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
						xPos = mi.rcMonitor.left;
						yPos = mi.rcMonitor.top;
						width = mi.rcMonitor.right - mi.rcMonitor.left;
						height = mi.rcMonitor.bottom - mi.rcMonitor.top;
					}
				} else {
					xPos = cache.rect.left;
					yPos = cache.rect.top;
					width = cache.rect.right - cache.rect.left;
					height = cache.rect.bottom - cache.rect.top;
				}
			}
		}
	} else if (info.Length() == 3) {
		if (action == "MoveWindow") {
			xPos = info[1].As<Napi::Number>().Int32Value();
			yPos = info[2].As<Napi::Number>().Int32Value();
		} else if (action == "ResizeWindow") {
			width = info[1].As<Napi::Number>().Int32Value();
			height = info[2].As<Napi::Number>().Int32Value();
		}
	} else {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	BYTE opacity = static_cast<BYTE>(opacityValue);

	SetParent(hwnd, cache.parent);
	SetWindowLong(hwnd, GWL_STYLE, style);
	SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
	SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED);
	SetWindowPos(hwnd, insert, xPos, yPos, width, height, flags);
	SetLayeredWindowAttributes(hwnd, 0, opacity, LWA_ALPHA);

	return env.Null();
}

Napi::Value mystwin::AttachAsWallpaperExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "AttachAsWallpaper");
}

Napi::Value mystwin::AttachAsDesktopExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "AttachAsDesktop");
}

Napi::Value mystwin::AttachToTopExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "AttachToTop");
}

Napi::Value mystwin::DetachWindowExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "DetachWindow");
}

Napi::Value mystwin::SendToBackExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "SendToBack");
}

Napi::Value mystwin::BringToFrontExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "BringToFront");
}

Napi::Value mystwin::MoveWindowExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "MoveWindow");
}

Napi::Value mystwin::ResizeWindowExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "ResizeWindow");
}

Napi::Value mystwin::SetOpacityExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "SetOpacity");
}

Napi::Value mystwin::ToggleFrameExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "ToggleFrame");
}

Napi::Value mystwin::ToggleTaskBarExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "ToggleTaskBar");
}

Napi::Value mystwin::ToggleOverlayExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "ToggleOverlay");
}

Napi::Value mystwin::ToggleFullScreenExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "ToggleFullScreen");
}

Napi::Value mystwin::MaximizeExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "Maximize");
}

Napi::Value mystwin::MinimizeExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "Minimize");
}

Napi::Value mystwin::CloseExport(const Napi::CallbackInfo& info) {
	return Refresh(info, "Close");
}