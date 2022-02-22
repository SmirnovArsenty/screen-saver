#pragma once

#include "Windows.h"
#include "OpenGL/OpenGL.h"

class win {
public:
	~win() = default;

	void init(HWND hWnd);
	void deinit();
private:
	win();

	HWND m_hWnd{ nullptr };
	OpenGL* m_openGL{ nullptr };
public:
	static win g_win;
};
