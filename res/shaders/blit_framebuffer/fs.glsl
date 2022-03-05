#version 430

in vec2 in_TexCoord;
out vec4 o_Color;

uniform sampler2DMS screenTex;
uniform vec2 screenSize;

void main(void) {
	o_Color = texelFetch(screenTex, ivec2(gl_FragCoord), 0);
	o_Color += texelFetch(screenTex, ivec2(gl_FragCoord), 1);
	o_Color += texelFetch(screenTex, ivec2(gl_FragCoord), 2);
	o_Color += texelFetch(screenTex, ivec2(gl_FragCoord), 3);
	o_Color.r = o_Color.r / 4;
	o_Color.g = o_Color.g / 4;
	o_Color.b = o_Color.b / 4;
	o_Color.a = o_Color.a / 4;
}
