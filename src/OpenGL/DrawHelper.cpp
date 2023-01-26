#include "DrawHelper.h"
#include "math/matrix.h"

#include <chrono>
#include <fstream>
#include <vector>
#include "resources.h"
#include "common.h"

DrawHelper::DrawHelper(OpenGL* engine) : m_engine{ engine } {
	GL_CHECK(glGenVertexArrays(1, &m_vao));
	GL_CHECK(glGenBuffers(1, &m_vbo));

	m_program = new shader(IDS_SHADER_DEFAULT_VS, IDS_SHADER_DEFAULT_FS);
}
DrawHelper::~DrawHelper() {
	if (m_program != nullptr) {
		delete m_program;
		m_program = nullptr;
	}
	GL_CHECK(glDeleteBuffers(1, &m_vbo));
	GL_CHECK(glDeleteVertexArrays(1, &m_vao));
}
void DrawHelper::DrawScene() {
	m_engine->clear();

	// use program
	m_program->use();

	GL_CHECK(glUniform1f(m_program->getUniformLocation("time"), float(clock())));

	GL_CHECK(glBindVertexArray(m_vao));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));

	const GLfloat triangle_points[] = {
		 3.f, -1.f, 0.f,
		 -1.f, 3.f, 0.f,
		 -1.f, -1.f, 0.f
	};
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, _countof(triangle_points) * sizeof(GLfloat), triangle_points, GL_STATIC_DRAW));
	GL_CHECK(glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, nullptr));
	GL_CHECK(glEnableVertexAttribArray(0));

	GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CHECK(glBindVertexArray(0));

	return;
}
