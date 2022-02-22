#include "win.h"
#include <cassert>

win::win() {}

void win::init(HWND hWnd) {
	assert(m_hWnd == nullptr);
	m_hWnd = m_hWnd;

	ShowWindow(m_hWnd, true);

	m_openGL = new OpenGL(m_hWnd);
}

void win::deinit() {
	delete m_openGL;
}
