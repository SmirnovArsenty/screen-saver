#version 430

in vec3 in_Position;

uniform mat4 view;
uniform mat4 projection;

void main(void) {
	gl_Position = projection * view * vec4(in_Position, 1.0);
}
