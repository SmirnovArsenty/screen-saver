#include "DrawHelper.h"
#include "math/matrix.h"

#include <chrono>
#include <fstream>
#include <vector>

DrawHelper::DrawHelper(OpenGL* engine) : m_engine{ engine } {
	GL_CHECK(glGenVertexArrays(1, &m_vao));
	GL_CHECK(glGenBuffers(1, &m_vbo));

	m_program = new shader(
		"../res/shaders/default/vs.glsl",
		"../res/shaders/default/fs.glsl");
}
DrawHelper::~DrawHelper() {
	if (m_program != nullptr) {
		delete m_program;
		m_program = nullptr;
	}
	GL_CHECK(glDeleteBuffers(1, &m_vbo));
	GL_CHECK(glDeleteVertexArrays(1, &m_vao));
}
void DrawHelper::DrawPoint(vec3 pos) {
	float vert[] = { pos.x(), pos.y(), pos.z() };

	GL_CHECK(glBindVertexArray(m_vao));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, _countof(vert) * sizeof(GLfloat), vert, GL_STREAM_DRAW));
	GL_CHECK(glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0));
	GL_CHECK(glEnableVertexAttribArray(0));

	GL_CHECK(glDrawArrays(GL_POINTS, 0, 1));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CHECK(glBindVertexArray(0));
}
void DrawHelper::DrawLine(vec3 v1, vec3 v2) {
	float vert[] = { v1.x(), v1.y(), v1.z(), v2.x(), v2.y(), v2.z() };

	GL_CHECK(glBindVertexArray(m_vao));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, _countof(vert) * sizeof(GLfloat), vert, GL_STREAM_DRAW));
	GL_CHECK(glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0));
	GL_CHECK(glEnableVertexAttribArray(0));

	GL_CHECK(glDrawArrays(GL_LINES, 0, 2));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CHECK(glBindVertexArray(0));
}
void DrawHelper::DrawScene() {
	m_engine->clear();
	GL_CHECK(glPointSize(5.f));
	GL_CHECK(glLineWidth(2.f));

	// use program
	m_program->use();

	// setup camera
	matrix4x4 projection = matrix4x4::perspective(
		-1.f, 1.f,
		-1.f, 1.f,
		1.f, 10.f);
	matrix4x4 view = matrix4x4::view({ 4.f, 0.f, 0.f }, { 0.f, 0.f, 0.f });
	GL_CHECK(glUniformMatrix4fv(m_program->getUniformLocation("projection"), 1, GL_FALSE, projection[0]));
	GL_CHECK(glUniformMatrix4fv(m_program->getUniformLocation("view"), 1, GL_TRUE, view[0]));
	GL_CHECK(glUniform2f(m_program->getUniformLocation("screen_size"), m_engine->width(), m_engine->height()));

	const vec4 points4D[] = {
		{ 1.f, 1.f, 1.f, 1.f },
		{ 1.f, 1.f, -1.f, 1.f },
		{ 1.f, -1.f, 1.f, 1.f },
		{ 1.f, -1.f, -1.f, 1.f },
		{ -1.f, 1.f, 1.f, 1.f },
		{ -1.f, 1.f, -1.f, 1.f },
		{ -1.f, -1.f, 1.f, 1.f },
		{ -1.f, -1.f, -1.f, 1.f },

		{ 1.f, 1.f, 1.f, -1.f },
		{ 1.f, 1.f, -1.f, -1.f },
		{ 1.f, -1.f, 1.f, -1.f },
		{ 1.f, -1.f, -1.f, -1.f },
		{ -1.f, 1.f, 1.f, -1.f },
		{ -1.f, 1.f, -1.f, -1.f },
		{ -1.f, -1.f, 1.f, -1.f },
		{ -1.f, -1.f, -1.f, -1.f }
	};

	vec3 pointsFrom4Dto3D[16];

	for (uint32_t i = 0; i < _countof(points4D); ++i) {
		int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		float angle = now * .0001f;
		vec4 rotated = matrix4x4::rotateXY(angle) * matrix4x4::rotateZW(angle) * points4D[i];

		pointsFrom4Dto3D[i] = matrix4x4::projectVec4to3D(rotated, 2.f);
	}

	// draw points
	for (uint32_t i = 0; i < _countof(points4D); ++i) {
		DrawPoint(pointsFrom4Dto3D[i]);
	}
	// draw lines
	for (uint32_t i = 0; i < 2; ++i) {
		DrawLine(pointsFrom4Dto3D[0 + 8 * i], pointsFrom4Dto3D[1 + 8 * i]);
		DrawLine(pointsFrom4Dto3D[0 + 8 * i], pointsFrom4Dto3D[2 + 8 * i]);
		DrawLine(pointsFrom4Dto3D[1 + 8 * i], pointsFrom4Dto3D[3 + 8 * i]);
		DrawLine(pointsFrom4Dto3D[2 + 8 * i], pointsFrom4Dto3D[3 + 8 * i]);

		DrawLine(pointsFrom4Dto3D[4 + 8 * i], pointsFrom4Dto3D[5 + 8 * i]);
		DrawLine(pointsFrom4Dto3D[4 + 8 * i], pointsFrom4Dto3D[6 + 8 * i]);
		DrawLine(pointsFrom4Dto3D[5 + 8 * i], pointsFrom4Dto3D[7 + 8 * i]);
		DrawLine(pointsFrom4Dto3D[6 + 8 * i], pointsFrom4Dto3D[7 + 8 * i]);

		DrawLine(pointsFrom4Dto3D[0 + 8 * i], pointsFrom4Dto3D[4 + 8 * i]);
		DrawLine(pointsFrom4Dto3D[1 + 8 * i], pointsFrom4Dto3D[5 + 8 * i]);
		DrawLine(pointsFrom4Dto3D[2 + 8 * i], pointsFrom4Dto3D[6 + 8 * i]);
		DrawLine(pointsFrom4Dto3D[3 + 8 * i], pointsFrom4Dto3D[7 + 8 * i]);
	}
	for (uint32_t i = 0; i < 8; ++i) {
		DrawLine(pointsFrom4Dto3D[i], pointsFrom4Dto3D[i + 8]);
	}
}
