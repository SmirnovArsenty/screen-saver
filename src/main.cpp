#include <Windows.h>
#include "win/win.h"

win win::g_win{};

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_GETMINMAXINFO: {
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y =
			GetSystemMetrics(SM_CYMAXTRACK) +
			GetSystemMetrics(SM_CYCAPTION) +
			GetSystemMetrics(SM_CYMENU) +
			GetSystemMetrics(SM_CYBORDER) * 2;
		return 0;
	}
	case WM_CREATE: {
		win::g_win.init(hWnd);
		return 0;
	}
	case WM_ERASEBKGND: {
		return 0;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(hWnd, &ps);
		win::g_win.draw(hDC);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_SIZE: {
		win::g_win.resize(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	case WM_DESTROY: {
		win::g_win.deinit();
		return 0;
	}
	case WM_MOUSEMOVE: {
		return 0;
		int32_t x = LOWORD(lParam);
		int32_t y = HIWORD(lParam);
		RECT rc;
		GetWindowRect(hWnd, &rc);
		if (x * 2 != (rc.right - rc.left) || y * 2 != (rc.bottom - rc.top)) {
			win::g_win.deinit();
		}
		return 0;
	}
	case WM_KEYDOWN: {
		return 0;
		win::g_win.deinit();
		return 0;
	}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int) {
	win::g_win.set_instance(hInstance);
	const wchar_t class_name[] = L"tesseract window class";
	WNDCLASS wc{};
	wc.lpfnWndProc = WinProc;
	wc.hInstance = hInstance;
	wc.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = class_name;
	RegisterClass(&wc);

	HWND hWnd = CreateWindow(class_name,
		L"tesseract", WS_POPUP | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL,
		hInstance, NULL);

	if (hWnd == nullptr) {
		return 0;
	}

	MSG message;
	while (true) {
		if (PeekMessage(&message, hWnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
			if (win::g_win.is_closed()) {
				break;
			}
		} else {
			HDC hDC = GetDC(hWnd);
			win::g_win.draw(hDC);
			ReleaseDC(hWnd, hDC);
		}
	}

	return 0;
}
