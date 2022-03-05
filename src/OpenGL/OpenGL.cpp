#include "OpenGL.h"
#include <cassert>
#include <vector>
#include <fstream>
#include <chrono>

#include "resources.h"

void printGLerror(GLenum error, const char* file, int line) {
	if (error == 0) {
		return;
	}
	std::string file_line = file;
	file_line += ":" + std::to_string(line) + " ";
	switch (error) {
		case GL_INVALID_ENUM:
			OutputDebugStringA((file_line + "GL_INVALID_ENUM").c_str());
			break;
		case GL_INVALID_VALUE:
			OutputDebugStringA((file_line + "GL_INVALID_VALUE").c_str());
			break;
		case GL_INVALID_OPERATION:
			OutputDebugStringA((file_line + "GL_INVALID_OPERATION").c_str());
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			OutputDebugStringA((file_line + "GL_INVALID_FRAMEBUFFER_OPERATION").c_str());
			break;
		case GL_OUT_OF_MEMORY:
			OutputDebugStringA((file_line + "GL_OUT_OF_MEMORY").c_str());
			break;
	}
}

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
	GL_CHECK(glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]));
	GL_CHECK(glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]));

	if (!m_hGLRC) {
		m_error_flag = true;
	}

	wglSwapIntervalEXT(0);

	GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

	GL_CHECK(glGenFramebuffers(1, &m_framebuffer));
	GL_CHECK(glGenTextures(1, &m_rendertarget));

	GL_CHECK(glGenVertexArrays(1, &m_vao));
	GL_CHECK(glGenBuffers(1, &m_vbo));

	m_swapbuf_program = new shader(IDS_SHADER_BLIT_VS, IDS_SHADER_BLIT_VS);

	GL_CHECK(glEnable(GL_MULTISAMPLE));

	ReleaseDC(hWnd, hDC);
}

OpenGL::~OpenGL() {
	if (m_swapbuf_program != nullptr) {
		delete m_swapbuf_program;
		m_swapbuf_program = nullptr;
	}

	GL_CHECK(glDeleteBuffers(1, &m_vbo));
	GL_CHECK(glDeleteVertexArrays(1, &m_vao));
	if (m_rendertarget != 0) {
		GL_CHECK(glDeleteTextures(1, &m_rendertarget));
	}
	GL_CHECK(glDeleteFramebuffers(1, &m_framebuffer));

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hGLRC);
}

void OpenGL::clear() {
	GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer));
	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
}

void OpenGL::resize(GLint w, GLint h) {
	GL_CHECK(glViewport(0, 0, w, h));
	m_width = (float)w;
	m_height = (float)h;

	if (m_rendertarget != 0) {
		GL_CHECK(glDeleteTextures(1, &m_rendertarget));
		m_rendertarget = 0;
	}
	GL_CHECK(glGenTextures(1, &m_rendertarget));

	GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_rendertarget));
	GL_CHECK(glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, w, h, GL_TRUE));

	GL_CHECK(glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GL_CHECK(glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_rendertarget, 0));

	GL_CHECK(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
	GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));

	GLenum buf[] = { GL_COLOR_ATTACHMENT0 };
	GL_CHECK(glDrawBuffers(1, buf));
}
void OpenGL::SwapBuffers(HDC hDC) {
	GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
	m_swapbuf_program->use();

	GL_CHECK(glUniform2f(m_swapbuf_program->getUniformLocation("screenSize"), m_width, m_height));

	GL_CHECK(glActiveTexture(GL_TEXTURE0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_rendertarget));
	GL_CHECK(glBindVertexArray(m_vao));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GLfloat vert[] = {
		-1.0f,  1.0f,
		-1.0f, -1.0f,
		 1.0f, -1.0f,

		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, _countof(vert) * sizeof(GLfloat), vert, GL_STATIC_DRAW));
	GL_CHECK(glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, nullptr));
	GL_CHECK(glEnableVertexAttribArray(0));
	GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));

	::SwapBuffers(hDC);
	return;
	// calc fps
	static auto last_time_fps = std::chrono::high_resolution_clock::now();
	static int32_t fps = 0;
	++fps;
	auto now = std::chrono::high_resolution_clock::now();

	if (now - last_time_fps > std::chrono::seconds(1)) {
		last_time_fps = now;
		fps = 0;
	}
}

GLfloat OpenGL::width() {
	return m_width;
}
GLfloat OpenGL::height() {
	return m_height;
}

bool OpenGL::is_error() {
	return m_error_flag;
}
