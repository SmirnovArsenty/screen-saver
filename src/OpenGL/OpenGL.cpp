#include "OpenGL.h"
#include <cassert>
#include <vector>
#include <fstream>

OpenGL::OpenGL(HWND hWnd) {
	HDC hDC = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	if (nPixelFormat == 0) {
		m_error_flag = true;
		return;
	}

	BOOL bResult = SetPixelFormat(hDC, nPixelFormat, &pfd);

	if (!bResult) {
		m_error_flag = true;
		return;
	}

	HGLRC tempContext = wglCreateContext(hDC);
	wglMakeCurrent(hDC, tempContext);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		m_error_flag = true;
		return;
	}

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 2,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		m_hGLRC = wglCreateContextAttribsARB(hDC, 0, attribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(hDC, m_hGLRC);
	}
	else
	{	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		m_hGLRC = tempContext;
	}

	//Checking GL version
	const GLubyte* GLVersionString = glGetString(GL_VERSION);

	//Or better yet, use the GL3 way to get the version number
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

	if (!m_hGLRC) {
		m_error_flag = true;
	}

	// initial contents
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

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glProgramUniform2f(m_program, 0, m_width, m_height);
}

OpenGL::~OpenGL() {
	glDeleteVertexArrays(1, &m_vaoID);
	glDeleteBuffers(2, m_vboID);
	glDeleteBuffers(1, &m_uboID);
	glDeleteProgram(m_program);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hGLRC);
}

void OpenGL::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(m_vaoID);		// select first VAO
	glDrawArrays(GL_TRIANGLES, 0, 3);	// draw first object

	glBindVertexArray(0);
}

void OpenGL::resize(GLint w, GLint h) {
	glViewport(0, 0, w, h);
	m_width = (float)w;
	m_height = (float)h;
}

bool OpenGL::is_error() {
	return m_error_flag;
}
