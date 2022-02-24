#pragma once

#include "OpenGL.h"
#include "math/vec.h"

class DrawHelper {
private:
	OpenGL* m_engine;

	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ubo;

	GLfloat m_line_width{ 1 };
	GLfloat m_point_size{ 1 };
public:
	DrawHelper(OpenGL* engine);
	~DrawHelper();

	void DrawPoint(vec3 pos);
	void DrawLine(vec3 v1, vec3 v2);
};
