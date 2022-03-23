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
	if (g_scrmode == ScrMode::smPreview) {
		GL_CHECK(glPointSize(1.f));
		GL_CHECK(glLineWidth(1.f));
	} else {
		GL_CHECK(glPointSize(m_point_size));
		GL_CHECK(glLineWidth(m_line_width));
	}

	// use program
	m_program->use();

	// setup camera
	matrix4x4 projection = matrix4x4::perspective(
		-1.f, 1.f,
		-1.f, 1.f,
		1.f, 7.f);
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

	static float angle = 0.f;
	static auto last_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> diff = std::chrono::high_resolution_clock::now() - last_time;
	last_time = std::chrono::high_resolution_clock::now();
	angle += .1f * diff.count();
	matrix4x4 transform = matrix4x4::rotateXY(angle * 0.852f) * matrix4x4::rotateZW(angle * 0.583f);
	for (uint32_t i = 0; i < _countof(points4D); ++i) {
		vec4 rotated = transform * points4D[i];
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
