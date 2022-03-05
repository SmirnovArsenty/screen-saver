#pragma once

#include <cstdint>

class vec3;
class vec4;

class matrix4x4 {
private:
	float a[4][4]{};
	matrix4x4() = default;
public:
	static matrix4x4 perspective(float l, float r, float b, float t, float n, float f);
	static matrix4x4 view(vec3 pos, vec3 at);

	static vec3 projectVec4to3D(vec4 v, float distance = 2.f);

	static matrix4x4 translate(vec3 v);
	static matrix4x4 scale(float scale);
	static matrix4x4 rotateX(float angle);
	static matrix4x4 rotateY(float angle);
	static matrix4x4 rotateZ(float angle);
	static matrix4x4 rotateXY(float angle);
	static matrix4x4 rotateZW(float angle);

	float** data();
	float* operator[](uint32_t index);

	matrix4x4 operator*(matrix4x4 mat);
	vec4 operator*(vec4 v);
};
