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
void DrawHelper::DrawScene(int32_t w, int32_t h) {
	GL_CHECK(glViewport(0, 0, w * 2, h * 2));
	m_engine->clear();

	// use program
	m_program->use();

	float time = clock() / 1e4f;
	// OutputDebugString(("Time: " + std::to_string(time) + "\n").c_str());
	GL_CHECK(glUniform1f(m_program->getUniformLocation("time"), time));

	GL_CHECK(glBindVertexArray(m_vao));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));

	GLfloat triangle_points[] = {
		 3.f, -1.f,
		 -1.f, 3.f,
		 -1.f, -1.f,
	};
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, _countof(triangle_points) * sizeof(GLfloat), triangle_points, GL_DYNAMIC_COPY));
	GL_CHECK(glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, nullptr));
	GL_CHECK(glEnableVertexAttribArray(0));
	
	GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CHECK(glBindVertexArray(0));

	return;
}