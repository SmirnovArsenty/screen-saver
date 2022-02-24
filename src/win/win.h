#pragma once

#include "Windows.h"
#include "OpenGL/DrawHelper.h"

class win {
private:
	win();

	HWND m_hWnd{ nullptr };
	OpenGL* m_openGL{ nullptr };
	DrawHelper* m_draw_helper{ nullptr };

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
