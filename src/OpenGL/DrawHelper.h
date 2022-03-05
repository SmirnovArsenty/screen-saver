#pragma once

#include "OpenGL.h"
#include "shader/shader.h"
#include "math/vec.h"

class DrawHelper {
private:
	OpenGL* m_engine;

	shader* m_program{ nullptr };

	GLuint m_vao;
	GLuint m_vbo;

	GLfloat m_line_width{ 1 };
	GLfloat m_point_size{ 1 };

	void DrawPoint(vec3 pos);
	void DrawLine(vec3 v1, vec3 v2);
public:
	DrawHelper(OpenGL* engine);
	~DrawHelper();

	void DrawScene();
};
