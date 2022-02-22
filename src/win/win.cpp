#include "win.h"
#include <cassert>

win::win() {}

void win::init(HWND hWnd) {
	assert(m_hWnd == nullptr);
	m_hWnd = hWnd;

	ShowWindow(m_hWnd, true);

	m_openGL = new OpenGL(m_hWnd);
}

void win::deinit() {
	delete m_openGL;
	m_openGL = nullptr;
}

void win::draw() {
	m_openGL->update_data();
	m_openGL->draw();
	SwapBuffers(GetDC(m_hWnd));
}

void win::resize(GLint w, GLint h) {
	m_openGL->resize(w, h);
}

bool win::is_animating() {
	return m_openGL != nullptr;
}
