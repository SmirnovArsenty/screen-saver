#include "win.h"
#include "common.h"
#include <cassert>

win::win() {}
HINSTANCE win::instance() { return m_hInstance;  }
void win::set_instance(HINSTANCE hInstance) { m_hInstance = hInstance; }

win::~win() {
	deinit();
}

void win::init(HWND hWnd) {
	assert(m_hWnd == nullptr);
	m_hWnd = hWnd;

	m_d3d11 = new D3D11Driver(m_hWnd);

	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	if (g_scrmode == ScrMode::smSaver) {
		// MONITORINFO monitor_info;
		// monitor_info.cbSize = sizeof(monitor_info);
		// GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST),
		// 	&monitor_info);
		// RECT window_rect(monitor_info.rcMonitor);
		// SetWindowPos(m_hWnd, NULL, window_rect.left, window_rect.top,
		// 	window_rect.right, window_rect.bottom,
		// 	SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		// SetCursorPos((window_rect.right - window_rect.left) / 2, (window_rect.bottom - window_rect.top) / 2);
		ShowCursor(false);
	}

	RECT rc;
	GetWindowRect(hWnd, &rc);
	m_is_closed = false;
	m_d3d11->resize(rc.right - rc.left, rc.bottom - rc.top);
}

void win::deinit() {
	if (m_is_closed) {
		return;
	}
	delete m_d3d11;
	m_d3d11 = nullptr;
	m_is_closed = true;
	DestroyWindow(m_hWnd);
	m_hWnd = nullptr;
}

void win::draw(HDC hDC) {
	if (m_d3d11 == nullptr) {
		return;
	}
	m_d3d11->draw();
}

void win::resize(uint32_t w, uint32_t h) {
	if (m_d3d11 != nullptr) {
		m_d3d11->resize(w, h);
	}
}

bool win::is_closed() {
	return m_is_closed;
}
