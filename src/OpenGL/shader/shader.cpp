#include <Windows.h>

#include "OpenGL/OpenGL.h"
#include "shader.h"

#include <fstream>
#include <vector>

shader::shader(const std::string vs, const std::string fs) {
	m_program = glCreateProgram();
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	{
		std::ifstream vs_source(vs);
		std::string vs_source_str((std::istreambuf_iterator<char>(vs_source)),
			std::istreambuf_iterator<char>());
		const char* vs_source_ptr = vs_source_str.c_str();
		GL_CHECK(glShaderSource(vertex_shader, 1, &vs_source_ptr, 0));
	}

	{
		std::ifstream fs_source(fs);
		std::string fs_source_str((std::istreambuf_iterator<char>(fs_source)),
			std::istreambuf_iterator<char>());
		const char* fs_source_ptr = fs_source_str.c_str();
		GL_CHECK(glShaderSource(fragment_shader, 1, &fs_source_ptr, 0));
	}

	GLint is_compiled = false;
	GL_CHECK(glCompileShader(vertex_shader));
	GL_CHECK(glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_compiled));
	if (!is_compiled) {
		OutputDebugStringA(("\tError in shader: " + vs + "\n").c_str());
		GLint maxLength = 0;
		GL_CHECK(glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength));

		std::vector<GLchar> error_log(maxLength);
		GL_CHECK(glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &error_log[0]));

		OutputDebugStringA(&error_log[0]);
		GL_CHECK(glDeleteShader(vertex_shader));
	}
	GL_CHECK(glCompileShader(fragment_shader));
	GL_CHECK(glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_compiled));
	if (!is_compiled) {
		OutputDebugStringA(("\tError in shader: " + fs + "\n").c_str());
		GLint maxLength = 0;
		GL_CHECK(glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength));

		std::vector<GLchar> error_log(maxLength);
		GL_CHECK(glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &error_log[0]));

		OutputDebugStringA(&error_log[0]);
		GL_CHECK(glDeleteShader(fragment_shader));
	}

	GL_CHECK(glAttachShader(m_program, vertex_shader));
	GL_CHECK(glAttachShader(m_program, fragment_shader));
	GL_CHECK(glLinkProgram(m_program));
	GL_CHECK(glDetachShader(m_program, vertex_shader));
	GL_CHECK(glDetachShader(m_program, fragment_shader));
	GL_CHECK(glDeleteShader(vertex_shader));
	GL_CHECK(glDeleteShader(fragment_shader));
}

shader::~shader() {
	GL_CHECK(glDeleteProgram(m_program));
}

void shader::use() const {
	GL_CHECK(glUseProgram(m_program));
}

GLint shader::getUniformLocation(const char* name) const {
	GLint res = glGetUniformLocation(m_program, name);
	GL_CHECK((void)0);
	return res;
}
