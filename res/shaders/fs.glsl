#version 430

layout(location = 0) uniform vec2 screen_size;

out vec4 out_Color;

void main(void) {
	float x = gl_FragCoord.x / screen_size.x;
	float y = gl_FragCoord.y / screen_size.y;
	out_Color = vec4(x, y, 0.0 ,1.0);
}
