#include "DrawHelper.h"
#include "math/matrix.h"

#include <chrono>
#include <fstream>
#include <vector>

DrawHelper::DrawHelper(OpenGL* engine) : m_engine{ engine } {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	m_program = glCreateProgram();
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	{
		std::ifstream vs_source("../res/shaders/vs.glsl");
		std::string vs_source_str((std::istreambuf_iterator<char>(vs_source)),
			std::istreambuf_iterator<char>());
		const char* vs_source_ptr = vs_source_str.c_str();
		glShaderSource(vertex_shader, 1, &vs_source_ptr, 0);
	}

	{
		std::ifstream fs_source("../res/shaders/fs.glsl");
		std::string fs_source_str((std::istreambuf_iterator<char>(fs_source)),
			std::istreambuf_iterator<char>());
		const char* fs_source_ptr = fs_source_str.c_str();
		glShaderSource(fragment_shader, 1, &fs_source_ptr, 0);
	}

	GLint is_compiled = false;
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_compiled);
	if (!is_compiled) {
		GLint maxLength = 0;
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> error_log(maxLength);
		glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &error_log[0]);

		OutputDebugStringA(&error_log[0]);
		glDeleteShader(vertex_shader);
	}
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_compiled);
	if (!is_compiled) {
		GLint maxLength = 0;
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> error_log(maxLength);
		glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &error_log[0]);

		OutputDebugStringA(&error_log[0]);
		glDeleteShader(fragment_shader);
	}

	glAttachShader(m_program, vertex_shader);
	glAttachShader(m_program, fragment_shader);
	glLinkProgram(m_program);
	glDetachShader(m_program, vertex_shader);
	glDetachShader(m_program, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	glUseProgram(m_program);
}
DrawHelper::~DrawHelper() {
	glDeleteProgram(m_program);
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}
void DrawHelper::DrawPoint(vec3 pos) {
	float vert[] = { pos.x(), pos.y(), pos.z() };

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, _countof(vert) * sizeof(GLfloat), vert, GL_STREAM_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_POINTS, 0, 1);

	glBindVertexArray(0);
}
void DrawHelper::DrawLine(vec3 v1, vec3 v2) {
	float vert[] = { v1.x(), v1.y(), v1.z(), v2.x(), v2.y(), v2.z() };

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, _countof(vert) * sizeof(GLfloat), vert, GL_STREAM_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_LINES, 0, 2);

	glBindVertexArray(0);
}
void DrawHelper::DrawScene() {
	m_engine->clear();
	glPointSize(5.f);
	glLineWidth(2.f);

	// use program
	glUseProgram(m_program);

	// setup camera
	matrix4x4 projection = matrix4x4::perspective(
		1.f, -1.f,
		1.f, -1.f,
		1.f, 100.f);
	matrix4x4 view = matrix4x4::view({ 10.f, 0.f, 0.f }, { 0.f, 0.f, 0.f });
	matrix4x4 VP = projection * view;
	glUniformMatrix4fv(glGetUniformLocation(m_program, "VP"), 1, GL_FALSE, VP[0]);

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
		float time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		float angle = time * 0.001f;
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
