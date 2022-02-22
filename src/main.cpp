#include <Windows.h>
#include "win/win.h"

win win::g_win{};

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_GETMINMAXINFO:
		break;
	case WM_CREATE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
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
		L"tesseract", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL,
		hInstance, NULL);

	if (hWnd == nullptr) {
		return 0;
	}

	win::g_win.init(hWnd);

	MSG message;
	while (GetMessage(&message, hWnd, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	win::g_win.deinit();

	return 0;
}
