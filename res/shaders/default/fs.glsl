#version 430

out vec4 o_Color;

uniform vec2 screen_size;

void main(void) {
	vec2 coord;
	coord.x = gl_FragCoord.x / screen_size.x;
	coord.y = gl_FragCoord.y / screen_size.y;
	o_Color = vec4(coord.x, coord.y, coord.x * sin(coord.y) * sin(coord.y),1.0);
}
