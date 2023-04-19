#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <string>

class shader {
	GLuint m_program;
public:
	shader(int32_t vsID, int32_t fsID);
	~shader();

	void use() const;
	GLint getUniformLocation(const char* name) const;
};
