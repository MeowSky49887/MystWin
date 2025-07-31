#include "mystwin.h"

BOOL CALLBACK mystwin::FindWorkerW(HWND hwnd, LPARAM param) {
	shelldll = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);

	if (shelldll) {
		workerw = FindWindowEx(NULL, hwnd, L"WorkerW", NULL);
		if (workerw) {
			return FALSE;
		}
	}

	return TRUE;
}

void mystwin::WrapFindWorkerW(const Napi::Env env) {
    auto progman = FindWindow(L"Progman", NULL);
    SendMessageTimeout(progman, WM_SPAWN_WORKER, 0, 0, SMTO_NORMAL, 1000, NULL);

    EnumWindows(&FindWorkerW, reinterpret_cast<LPARAM>(&workerw));

    if (!workerw) {
        EnumChildWindows(progman, [](HWND hwnd, LPARAM param) -> BOOL {
            wchar_t className[256];
            GetClassName(hwnd, className, sizeof(className) / sizeof(wchar_t));
            if (wcscmp(className, L"WorkerW") == 0) {
                *((HWND*)param) = hwnd;
                return FALSE;
            }
            return TRUE;
        }, reinterpret_cast<LPARAM>(&workerw));
    }
}

Napi::Value mystwin::AttachAsWallpaperExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 1 || !info[0].IsBuffer()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	WrapFindWorkerW(env);

	if (!workerw) {
	        Napi::TypeError::New(env, "WorkerW not found.").ThrowAsJavaScriptException();
	        return env.Null();
	}

	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetParent(hwnd, workerw);

	return env.Null();
}

Napi::Value mystwin::AttachAsDesktopExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 1 || !info[0].IsBuffer()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	WrapFindWorkerW(env);

	if (!shelldll) {
	        Napi::TypeError::New(env, "ShellDll not found.").ThrowAsJavaScriptException();
	        return env.Null();
	}

	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetParent(hwnd, shelldll);

	return env.Null();
}

Napi::Value mystwin::AttachToTopExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 1 || !info[0].IsBuffer()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	RECT rect;
	GetWindowRect(hwnd, &rect);

	SetWindowPos(hwnd, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL);

	return env.Null();
}

Napi::Value mystwin::DetachWindowExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 1 || !info[0].IsBuffer()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	SetParent(hwnd, NULL);

	RECT rect;
	GetWindowRect(hwnd, &rect);

	SetWindowPos(hwnd, HWND_NOTOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL);
	
	return env.Null();
}

Napi::Value mystwin::SendToBackExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 1 || !info[0].IsBuffer()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	RECT rect;
	GetWindowRect(hwnd, &rect);

	SetWindowPos(hwnd, HWND_BOTTOM, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL);

	return env.Null();
}

Napi::Value mystwin::BringToFrontExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 1 || !info[0].IsBuffer()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	RECT rect;
	GetWindowRect(hwnd, &rect);

	SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL);
		
	return env.Null();
}

Napi::Value mystwin::MoveWindowExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 3 || !info[0].IsBuffer() || !info[1].IsNumber() || !info[2].IsNumber()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	int x = info[1].As<Napi::Number>().Int32Value();
	int y = info[2].As<Napi::Number>().Int32Value();

	RECT rect;
	GetWindowRect(hwnd, &rect);

	SetWindowPos(hwnd, NULL, x, y, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
		
	return env.Null();
}

Napi::Value mystwin::ResizeWindowExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 3 || !info[0].IsBuffer() || !info[1].IsNumber() || !info[2].IsNumber()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	int width = info[1].As<Napi::Number>().Int32Value();
	int height = info[2].As<Napi::Number>().Int32Value();

	RECT rect;
	GetWindowRect(hwnd, &rect);

	SetWindowPos(hwnd, NULL, rect.left, rect.top, width, height, SWP_NOZORDER);
		
	return env.Null();
}

Napi::Value mystwin::SetOpacityExport(const Napi::CallbackInfo& info) {
    auto env = info.Env();

    if (info.Length() < 2 || !info[0].IsBuffer() || !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Invalid arguments.").ThrowAsJavaScriptException();
        return env.Null();
    }

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
    HWND hwnd = (HWND)(LONG_PTR)handle;

    uint32_t opacityValue = info[1].As<Napi::Number>().Uint32Value();
    if (opacityValue > 255) {
        Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    BYTE opacity = static_cast<BYTE>(opacityValue);

    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, opacity, LWA_ALPHA);

    return env.Null();
}

Napi::Value mystwin::ToggleTaskBarExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 2 || !info[0].IsBuffer() || !info[1].IsBoolean()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	bool enable = info[1].As<Napi::Boolean>().Value();

	LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

	if (enable) {
		SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TOOLWINDOW);
	} else {
		SetWindowLong(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TOOLWINDOW);
	}

	RECT rect;
	GetWindowRect(hwnd, &rect);
		
	SetWindowPos(hwnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_FRAMECHANGED);

	return env.Null();
}

Napi::Value mystwin::ToggleFrameExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 2 || !info[0].IsBuffer() || !info[1].IsBoolean()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	bool enable = info[1].As<Napi::Boolean>().Value();

	LONG style = GetWindowLong(hwnd, GWL_STYLE);

	if (enable) {
		SetWindowLong(hwnd, GWL_STYLE, style | WS_CAPTION | WS_THICKFRAME);
	} else {
		SetWindowLong(hwnd, GWL_STYLE, style & ~(WS_CAPTION | WS_THICKFRAME));
	}

	RECT rect;
	GetWindowRect(hwnd, &rect);

	SetWindowPos(hwnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_FRAMECHANGED);
		
	return env.Null();
}

Napi::Value mystwin::ToggleOverlayExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 2 || !info[0].IsBuffer() || !info[1].IsBoolean()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	bool enable = info[1].As<Napi::Boolean>().Value();

	LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

	if (enable) {
		SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
	} else {
		SetWindowLong(hwnd, GWL_EXSTYLE, (exStyle | WS_EX_LAYERED) & ~WS_EX_TRANSPARENT);
	}

	RECT rect;
	GetWindowRect(hwnd, &rect);
		
	SetWindowPos(hwnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_FRAMECHANGED);

	return env.Null();
}

Napi::Value mystwin::ToggleFullScreenExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 2 || !info[0].IsBuffer() || !info[1].IsBoolean()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	bool enable = info[1].As<Napi::Boolean>().Value();

	LONG style = GetWindowLong(hwnd, GWL_STYLE);

	RECT rect;
	GetWindowRect(hwnd, &rect);

	if (enable) {
 		SetWindowLong(hwnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
 		MONITORINFO mi = { sizeof(mi) };
	        if (GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
	             SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
	 	}
	 	SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
	} else {
		SetWindowLong(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
		SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	return env.Null();
}

Napi::Value mystwin::MaximizeExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 1 || !info[0].IsBuffer()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	ShowWindow(hwnd, SW_MAXIMIZE);	

	return env.Null();
}

Napi::Value mystwin::MinimizeExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 1 || !info[0].IsBuffer()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	ShowWindow(hwnd, SW_MINIMIZE);	

	return env.Null();
}

Napi::Value mystwin::CloseExport(const Napi::CallbackInfo& info) {
	auto env = info.Env();

	if (info.Length() < 1 || !info[0].IsBuffer()) {
		Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	auto windowHandleBuffer = info[0].As<Napi::Buffer<uint8_t>>().Data();

	LONG_PTR handle = *reinterpret_cast<LONG_PTR*>(windowHandleBuffer);
	HWND hwnd = (HWND)(LONG_PTR)handle;

	DestroyWindow(hwnd);	

	return env.Null();
}
