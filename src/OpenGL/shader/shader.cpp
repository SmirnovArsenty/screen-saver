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
		if (vsID == IDS_SHADER_BLIT_VS) {
			vs_source = R"(#version 430

in vec2 in_Position;

out vec2 o_TexCoord;

void main(void) {
	gl_Position = vec4(in_Position.x, in_Position.y, 0.0, 1.0);
	o_TexCoord.x = (in_Position.x + 1.0) * 0.5;
	o_TexCoord.y = (in_Position.y + 1.0) * 0.5;
}
)";
		} else if (vsID == IDS_SHADER_DEFAULT_VS) {
			vs_source = R"(#version 430

in vec3 in_Position;

uniform mat4 view;
uniform mat4 projection;

void main(void) {
	gl_Position = projection * view * vec4(in_Position, 1.0);
}
)";
		}
		GL_CHECK(glShaderSource(vertex_shader, 1, &vs_source, 0));
	}

	{
		const char* fs_source = nullptr;
		if (fsID == IDS_SHADER_BLIT_FS) {
			fs_source = R"(#version 430

in vec2 in_TexCoord;
out vec4 o_Color;

uniform sampler2DMS screenTex;
uniform vec2 screenSize;

void main(void) {
	o_Color = texelFetch(screenTex, ivec2(gl_FragCoord), 0);
	o_Color += texelFetch(screenTex, ivec2(gl_FragCoord), 1);
	o_Color += texelFetch(screenTex, ivec2(gl_FragCoord), 2);
	o_Color += texelFetch(screenTex, ivec2(gl_FragCoord), 3);
	o_Color.r = o_Color.r / 4;
	o_Color.g = o_Color.g / 4;
	o_Color.b = o_Color.b / 4;
	o_Color.a = o_Color.a / 4;
}
)";
		} else if (fsID == IDS_SHADER_DEFAULT_FS) {
			fs_source = R"(#version 430

out vec4 o_Color;

uniform vec2 screen_size;

void main(void) {
	vec2 coord;
	coord.x = gl_FragCoord.x / screen_size.x;
	coord.y = gl_FragCoord.y / screen_size.y;
	o_Color = vec4(coord.x, coord.y, 1 - (coord.x + coord.y) / 2,1.0);
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
