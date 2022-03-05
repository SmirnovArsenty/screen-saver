#version 430

in vec2 in_Position;

out vec2 o_TexCoord;

void main(void) {
	gl_Position = vec4(in_Position.x, in_Position.y, 0.0, 1.0);
	o_TexCoord.x = (in_Position.x + 1.0) * 0.5;
	o_TexCoord.y = (in_Position.y + 1.0) * 0.5;
}
