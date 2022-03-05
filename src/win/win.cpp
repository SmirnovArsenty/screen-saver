#include "win.h"
#include <cassert>

win::win() {}

void win::init(HWND hWnd) {
	assert(m_hWnd == nullptr);
	m_hWnd = hWnd;

	ShowWindow(m_hWnd, true);

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
	m_is_closed = true;
}

void win::draw() {
	if (m_openGL == nullptr) {
		return;
	}
	m_draw_helper->DrawScene();
	m_openGL->SwapBuffers(GetDC(m_hWnd));
}

void win::resize(GLint w, GLint h) {
	if (m_openGL != nullptr) {
		m_openGL->resize(w, h);
	}
}

bool win::is_closed() {
	return m_is_closed;
}
