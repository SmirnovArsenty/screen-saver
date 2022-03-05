#include "win.h"
#include <cassert>

win::win() {}
HINSTANCE win::instance() { return m_hInstance;  }
void win::set_instance(HINSTANCE hInstance) { m_hInstance = hInstance;  }

void win::init(HWND hWnd) {
	assert(m_hWnd == nullptr);
	m_hWnd = hWnd;

	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	MONITORINFO monitor_info;
	monitor_info.cbSize = sizeof(monitor_info);
	GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST),
		&monitor_info);
	RECT window_rect(monitor_info.rcMonitor);
	SetWindowPos(m_hWnd, NULL, window_rect.left, window_rect.top,
		window_rect.right, window_rect.bottom,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	SetCursorPos((window_rect.right - window_rect.left) / 2, (window_rect.bottom - window_rect.top) / 2);
	ShowCursor(false);

	m_openGL = new OpenGL(m_hWnd);
	RECT rc;
	GetWindowRect(hWnd, &rc);
	m_is_closed = false;
	m_openGL->resize(rc.right - rc.left, rc.bottom - rc.top);
	m_draw_helper = new DrawHelper(m_openGL);
}

void win::deinit() {
	delete m_openGL;
	m_openGL = nullptr;
	delete m_draw_helper;
	m_draw_helper = nullptr;
	m_is_closed = true;
	DestroyWindow(m_hWnd);
	m_hWnd = nullptr;
}

void win::draw(HDC hDC) {
	if (m_openGL == nullptr) {
		return;
	}
	m_draw_helper->DrawScene();
	m_openGL->SwapBuffers(hDC);
}

void win::resize(GLint w, GLint h) {
	if (m_openGL != nullptr) {
		m_openGL->resize(w, h);
	}
}

bool win::is_closed() {
	return m_is_closed;
}
