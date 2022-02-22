#pragma once

#include <Windows.h>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

class OpenGL {
private:
	HGLRC m_hGLRC{ nullptr };

	GLuint m_program;

	GLuint m_vaoID[2];
	GLuint m_vboID[3];

	bool m_error_flag{ false };
public:
	OpenGL(HWND);
	~OpenGL();

	void update_data();
	void draw();
	void resize(GLint w, GLint h);

	[[nodiscard]] bool is_error();
};
