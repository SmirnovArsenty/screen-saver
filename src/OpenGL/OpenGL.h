#pragma once

#include <Windows.h>
#include <GL/GL.h>

class OpenGL {
private:
	HGLRC m_hGLRC;
public:
	OpenGL(HWND);
	~OpenGL();
};
