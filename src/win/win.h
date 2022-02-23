#pragma once

#include "Windows.h"
#include "OpenGL/OpenGL.h"

class win {
private:
	win();

	HWND m_hWnd{ nullptr };
	OpenGL* m_openGL{ nullptr };

	bool m_is_closed{ false };
public:
	~win() = default;

	void init(HWND hWnd);
	void deinit();

	void draw();
	void resize(GLint w, GLint h);

	bool is_closed();

	static win g_win;
};
