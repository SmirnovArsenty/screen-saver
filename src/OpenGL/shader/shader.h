#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <string>

class shader {
	GLuint m_program;
public:
	shader(const std::string vs, const std::string fs);
	~shader();

	void use() const;
	GLint getUniformLocation(const char* name) const;
};
