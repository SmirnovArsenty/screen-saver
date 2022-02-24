#pragma once

#include <Windows.h>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <cstdint>

class OpenGL {
private:
	HGLRC m_hGLRC{ nullptr };

	GLuint m_program{};
	GLuint m_vaoID{};
	GLuint m_vboID[2]{};
	GLuint m_uboID{};

	bool m_error_flag{ false };

	GLfloat m_width{}, m_height{};
public:
	OpenGL(HWND);
	~OpenGL();

	void draw();
	void resize(GLint w, GLint h);

	[[nodiscard]] bool is_error();
};
