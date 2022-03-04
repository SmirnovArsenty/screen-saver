#version 430

in vec3 in_Position;

uniform mat4 VP;

void main(void) {
	gl_Position = VP * vec4(in_Position, 1.0);
}
