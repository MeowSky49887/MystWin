#include "mystwin.h"

BOOL CALLBACK mystwin::FindWorkerW(HWND hwnd, LPARAM param) {
	HWND shelldll = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);

	if (shelldll) {
		workerw = FindWindowEx(NULL, hwnd, L"WorkerW", NULL);
		return FALSE;
	}

	return TRUE;
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

	auto progman = FindWindow(L"Progman", NULL);
	auto result = SendMessageTimeout(
		progman,
		WM_SPAWN_WORKER,
		NULL,
		NULL,
		SMTO_NORMAL,
		1000,
		NULL);

	if (!result) {
	        Napi::TypeError::New(env, "Failed for unknown reason.").ThrowAsJavaScriptException();
	        return env.Null();
	}

	EnumWindows(&FindWorkerW, reinterpret_cast<LPARAM>(&workerw));

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

	auto progman = FindWindow(L"Progman", NULL);
	auto result = SendMessageTimeout(
		progman,
		WM_SPAWN_WORKER,
		NULL,
		NULL,
		SMTO_NORMAL,
		1000,
		NULL);

	if (!result) {
	        Napi::TypeError::New(env, "Failed for unknown reason.").ThrowAsJavaScriptException();
	        return env.Null();
	}

	EnumWindows(&FindWorkerW, reinterpret_cast<LPARAM>(&workerw));

	RECT rect;
	GetWindowRect(hwnd, &rect);

	SetWindowPos(hwnd, workerw, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE);

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

	SetWindowPos(hwnd, HWND_BOTTOM, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE);

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

	SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE);
		
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

	SetWindowPos(hwnd, NULL, x, y, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE);
		
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

	SetWindowPos(hwnd, NULL, rect.left, rect.top, width, height, SWP_NOACTIVATE);
		
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
		
	SetWindowPos(hwnd, workerw, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_FRAMECHANGED);

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

	SetWindowPos(hwnd, workerw, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_FRAMECHANGED);
		
	return env.Null();
}

