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
		if (vsID == IDS_SHADER_BLIT_VS) {
			vs_source = R"(#version 420
in vec2 in_Position;
out vec2 g_pos;
void main(void) {
	gl_Position = vec4(in_Position.x, in_Position.y, 0.0, 1.0);
	g_pos = gl_Position.xy;
}
)";
		} else if (vsID == IDS_SHADER_DEFAULT_VS) {
			vs_source = R"(#version 420

layout(location = 0) in vec3 in_Position;

out vec2 g_pos;
// out float g_time;

void main(void)
{
	gl_Position = vec4(in_Position.x, in_Position.y, 0.0, 1.0);
	g_pos = gl_Position.xy;
	// g_time = in_Position.z;
}
)";
		}
		GL_CHECK(glShaderSource(vertex_shader, 1, &vs_source, 0));
	}

	{
		const char* fs_source = nullptr;
		if (fsID == IDS_SHADER_BLIT_FS) {
			fs_source = R"(#version 420
in vec2 g_pos;
out vec4 o_Color;
uniform sampler2D screenTex;
uniform vec2 screenSize;
void main(void) {
	o_Color = texture(screenTex, (g_pos + vec2(1)) / 2);
	o_Color += texture(screenTex, (g_pos + vec2(1)) / 2 + vec2(1, 1) / screenSize);
	o_Color += texture(screenTex, (g_pos + vec2(1)) / 2 + vec2(1, 0) / screenSize);
	o_Color += texture(screenTex, (g_pos + vec2(1)) / 2 + vec2(0, 1) / screenSize);
	o_Color = o_Color / 4;
}
)";
		} else if (fsID == IDS_SHADER_DEFAULT_FS) {
			fs_source = R"(#version 420
uniform float time;
in vec2 g_pos;
// in float g_time;
out vec4 o_Color;

void main(void)
{
	float time_scale = time;
	vec2 pos_x = g_pos + vec2(1, 1) * sin(time_scale * 1.32 + 13) * cos(time_scale * 0.83 + 16);
	vec2 pos_y = g_pos + vec2(1, 1) * sin(time_scale * 0.36 + 14) * cos(time_scale * 1.56 + 17);
	vec2 pos_z = g_pos + vec2(1, 1) * sin(time_scale * 0.57 + 15) * cos(time_scale * 1.33 + 18);

	vec3 color;
	color.r = abs(pos_x.x * 1.3 * sin(time_scale * 0.89 + 1) * cos(time_scale * 1.23 + 4)) + abs(pos_x.y * 0.7 * sin(time_scale * 1.13 + 7) * cos(time_scale * 0.86 + 10));
	color.g = abs(pos_y.x * 0.5 * sin(time_scale * 0.47 + 2) * cos(time_scale * 0.97 + 5)) + abs(pos_y.y * 0.4 * sin(time_scale * 1.43 + 8) * cos(time_scale * 0.75 + 11));
	color.b = abs(pos_z.x * 0.8 * sin(time_scale * 1.32 + 3) * cos(time_scale * 0.73 + 6)) + abs(pos_z.y * 1.5 * sin(time_scale * 0.93 + 9) * cos(time_scale * 1.27 + 12));

	color = pow(color, vec3(1/1.22));
	o_Color = vec4(color, 1.0);
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
