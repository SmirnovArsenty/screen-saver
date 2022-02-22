#include "OpenGL.h"

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
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
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

	const char* vertex_shader_source = 
R"(#version 140

in vec3 in_Position;
in vec3 in_Color;
out vec3 ex_Color;

void main(void)
{
	gl_Position = vec4(in_Position, 1.0);
	ex_Color = in_Color;
}
)";
	glShaderSource(vertex_shader, 1, &vertex_shader_source, 0);

	const char* fragment_shader_source =
R"(#version 140

precision highp float; // needed only for version 1.30

in  vec3 ex_Color;
out vec4 out_Color;

void main(void)
{
	out_Color = vec4(ex_Color,1.0);
}
)";
	glShaderSource(fragment_shader, 1, &fragment_shader_source, 0);

	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);
	glAttachShader(m_program, vertex_shader);
	glAttachShader(m_program, fragment_shader);
	glLinkProgram(m_program);
	glDetachShader(m_program, vertex_shader);
	glDetachShader(m_program, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	glUseProgram(m_program);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

OpenGL::~OpenGL() {
	glDeleteProgram(m_program);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hGLRC);
}

void OpenGL::update_data() {
	// First simple object
	float* vert = new float[9];	// vertex array
	float* col = new float[9];	// color array

	vert[0] = -0.3; vert[1] = 0.5; vert[2] = -1.0;
	vert[3] = -0.8; vert[4] = -0.5; vert[5] = -1.0;
	vert[6] = 0.2; vert[7] = -0.5; vert[8] = -1.0;

	col[0] = 1.0; col[1] = 0.0; col[2] = 0.0;
	col[3] = 0.0; col[4] = 1.0; col[5] = 0.0;
	col[6] = 0.0; col[7] = 0.0; col[8] = 1.0;

	// Second simple object
	float* vert2 = new float[9];	// vertex array

	vert2[0] = -0.2; vert2[1] = 0.5; vert2[2] = -1.0;
	vert2[3] = 0.3; vert2[4] = -0.5; vert2[5] = -1.0;
	vert2[6] = 0.8; vert2[7] = 0.5; vert2[8] = -1.0;

	// Two VAOs allocation
	glGenVertexArrays(2, &m_vaoID[0]);

	// First VAO setup
	glBindVertexArray(m_vaoID[0]);

	glGenBuffers(2, m_vboID);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vert, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), col, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// Second VAO setup	
	glBindVertexArray(m_vaoID[1]);

	glGenBuffers(1, &m_vboID[2]);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[2]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vert2, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	delete[] vert;
	delete[] vert2;
	delete[] col;
}

void OpenGL::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(m_vaoID[0]);		// select first VAO
	glDrawArrays(GL_TRIANGLES, 0, 3);	// draw first object

	glBindVertexArray(m_vaoID[1]);		// select second VAO
	glVertexAttrib3f((GLuint)1, 1.0, 0.0, 0.0); // set constant color attribute
	glDrawArrays(GL_TRIANGLES, 0, 3);	// draw second object

	glBindVertexArray(0);
}

void OpenGL::resize(GLint w, GLint h) {
	glViewport(0, 0, w, h);
}

bool OpenGL::is_error() {
	return m_error_flag;
}
