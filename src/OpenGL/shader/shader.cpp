#include "common.h"

#include <Windows.h>

#include "OpenGL/OpenGL.h"
#include "shader.h"

#include <fstream>
#include <vector>

#include "win/win.h"
#include "resources.h"

shader::shader(int32_t vsID, int32_t fsID) {
	m_program = glCreateProgram();
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	{
		const char* vs_source = nullptr;
		if (vsID == IDS_SHADER_DEFAULT_VS) {
			vs_source = R"(#version 430

layout(location = 0) in vec3 in_Position;

out vec2 o_pos;

void main(void) {
	gl_Position = vec4(in_Position.xyz, 1.0);
	o_pos = gl_Position.xy;
}
)";
		}
		GL_CHECK(glShaderSource(vertex_shader, 1, &vs_source, 0));
	}

	{
		const char* fs_source = nullptr;
		if (fsID == IDS_SHADER_DEFAULT_FS) {
			fs_source = R"(#version 430

in vec2 o_pos;

uniform float time;
out vec4 o_Color;

void main(void)
{
	float time_scale = time * 100;
	vec2 pos_x = o_pos + vec2(1, 1) * sin(time_scale * 1.72);
	vec2 pos_y = o_pos + vec2(1, 1) * cos(time_scale * 0.36);
	vec2 pos_z = o_pos + vec2(1, 1) * cos(time_scale * 0.57) * sin(time_scale * 1.23);
	o_Color = vec4(abs(pos_x.x * 1.3 * sin(time_scale * 0.89) - pos_x.y * 0.2 * sin(time_scale * 1.13)) + 0.2,
					abs(pos_y.x * 0.5 * sin(time_scale * 0.47) - pos_y.y * 0.4 * sin(time_scale * 1.43)) + 0.2,
					abs(pos_z.x * 0.8 * sin(time_scale * 1.32) - pos_z.y * 1.5 * sin(time_scale * 0.93)) + 0.2,
					1.0);
}
)";
		}
		GL_CHECK(glShaderSource(fragment_shader, 1, &fs_source, 0));
	}

	GLint is_compiled = false;
	GL_CHECK(glCompileShader(vertex_shader));
	GL_CHECK(glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_compiled));
	if (!is_compiled) {
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

	GL_CHECK(glBindAttribLocation(m_program, 0, "in_Position"));
	GL_CHECK(glBindAttribLocation(m_program, 1, "in_Time"));
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
