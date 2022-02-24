#include "DrawHelper.h"

DrawHelper::DrawHelper(OpenGL* engine) : m_engine{ engine } {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ubo);
}
void DrawHelper::DrawPoint(vec3 pos) {
	float vert[24] = {
		1.f, 1.f, 1.f,
		-1.f, 1.f, 1.f,
		-1.f, -1.f, 1.f,
		1.f, -1.f, 1.f,
		1.f, 1.f, -1.f,
		-1.f, 1.f, -1.f,
		-1.f, -1.f, -1.f,
		1.f, -1.f, -1.f
	};
	for (uint32_t i = 0; i < 24; ++i) {
		vert[i] *= m_point_size;
	}
	for (uint32_t i = 0; i < 8; ++i) {
		vert[i * 3] += pos.x();
		vert[i * 3 + 1] += pos.y();
		vert[i * 3 + 2] += pos.z();
	}

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, _countof(vert) * sizeof(GLfloat), vert, GL_STREAM_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}
