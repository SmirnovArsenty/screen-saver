#include <Windows.h>
#include "win/win.h"

win win::g_win{};

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_GETMINMAXINFO: {
		break;
	}
	case WM_CREATE: {
		win::g_win.init(hWnd);
		break;
	}
	case WM_PAINT: {
		win::g_win.draw();
		break;
	}
	case WM_SIZE: {
		win::g_win.resize(LOWORD(lParam), HIWORD(lParam));
		break;
	}
	case WM_DESTROY: {
		win::g_win.deinit();
		break;
	}
	case WM_MOUSEMOVE: {
		int32_t x = LOWORD(lParam);
		int32_t y = HIWORD(lParam);
		RECT rc;
		GetWindowRect(hWnd, &rc);
		if (x * 2 != (rc.right - rc.left) || y * 2 != (rc.bottom - rc.top)) {
			win::g_win.deinit();
		}
		break;
	}
	case WM_KEYDOWN: {
		win::g_win.deinit();
		break;
	}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int) {
	const wchar_t class_name[] = L"tesseract window class";
	WNDCLASS wc{};
	wc.lpfnWndProc = WinProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = class_name;
	wc.style = CS_DBLCLKS | CS_OWNDC;
	RegisterClass(&wc);

	HWND hWnd = CreateWindow(class_name,
		L"tesseract", WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL,
		hInstance, NULL);

	if (hWnd == nullptr) {
		return 0;
	}

	MSG message;
	while (true) {
		PeekMessage(&message, hWnd, 0, 0, PM_REMOVE);
		TranslateMessage(&message);
		DispatchMessage(&message);
		if (win::g_win.is_closed()) {
			break;
		}
		win::g_win.draw();
	}

	return 0;
}
