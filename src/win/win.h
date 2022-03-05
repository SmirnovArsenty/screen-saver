#pragma once

#include "Windows.h"
#include "OpenGL/DrawHelper.h"

class win {
private:
	win();

	HINSTANCE m_hInstance;
	HWND m_hWnd{ nullptr };
	OpenGL* m_openGL{ nullptr };
	DrawHelper* m_draw_helper{ nullptr };

	bool m_is_closed{ false };
public:
	~win() = default;

	void set_instance(HINSTANCE hInstance);
	HINSTANCE instance();

	void init(HWND hWnd);
	void deinit();

	void draw(HDC hDC);
	void resize(GLint w, GLint h);

	bool is_closed();

	static win g_win;
};
