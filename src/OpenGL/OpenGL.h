#pragma once

#include <Windows.h>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <cstdint>

#include "shader/shader.h"

void printGLerror(GLenum error);

#define GL_CHECK(x) \
x;

#if 0
do { \
	x; \
	GLenum error = glGetError(); \
} while ((void)0, 0)
#endif

class OpenGL {
private:
	HGLRC m_hGLRC{ nullptr };

	GLuint m_vao{};
	GLuint m_vbo{};

	bool m_error_flag{ false };

	GLfloat m_width{}, m_height{};
public:
	OpenGL(HWND);
	~OpenGL();

	void clear();
	void resize(GLint w, GLint h);
	void SwapBuffers(HDC hDC);

	GLfloat width();
	GLfloat height();

	[[nodiscard]] bool is_error();
};
