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

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

OpenGL::~OpenGL() {
	glDeleteVertexArrays(1, &m_vaoID);
	glDeleteBuffers(2, m_vboID);
	glDeleteBuffers(1, &m_uboID);
	glDeleteProgram(m_program);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hGLRC);
}

void OpenGL::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGL::resize(GLint w, GLint h) {
	glViewport(0, 0, w, h);
	m_width = (float)w;
	m_height = (float)h;
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
