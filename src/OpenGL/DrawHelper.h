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
public:
	DrawHelper(OpenGL* engine);
	~DrawHelper();

	void DrawScene();
};
