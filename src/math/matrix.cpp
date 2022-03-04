#pragma once

#include "vec.h"
#include "matrix.h"
#include <cmath>

matrix4x4 matrix4x4::perspective(float width, float height, float n, float f) {
	matrix4x4 res;

	res[0][0] = n / width;	res[1][0] = 0;			res[2][0] = 0;					res[3][0] = 0;
	res[0][1] = 0;			res[1][1] = n / height;	res[2][1] = 0;					res[3][1] = 0;
	res[0][2] = 0;			res[1][2] = 0;			res[2][2] = -(f + n) / (f - n);	res[3][2] = -(2 * f * n) / (f - n);
	res[0][3] = 0;			res[1][3] = 0;			res[2][3] = -1;					res[3][3] = 0;

	return res;
}

matrix4x4 matrix4x4::view(vec3 pos, vec3 at) {
	matrix4x4 res;

	vec3 up(0, 0, 1);
	vec3 forward = (at - pos).normalize();
	vec3 right = (forward * up).normalize();
	up = (right * forward).normalize();

	res[0][0] = right.x();	res[1][0] = up.x();	res[2][0] = forward.x();	res[3][0] = pos.x();
	res[0][1] = right.y();	res[1][1] = up.y();	res[2][1] = forward.y();	res[3][1] = pos.y();
	res[0][2] = right.z();	res[1][2] = up.z();	res[2][2] = forward.z();	res[3][2] = pos.z();
	res[0][3] = 0;			res[1][3] = 0;		res[2][3] = 0;				res[3][3] = 1;

	return res;
}

vec3 matrix4x4::projectVec4to3D(vec4 v, float distance) {
	matrix4x4 mat;

	float w = 1 / (distance - v.w());
	mat[0][0] = w; mat[1][1] = w; mat[2][2] = w;

	vec4 projected = mat * v;
	return { projected.x(), projected.y(), projected.z() };
}

matrix4x4 matrix4x4::translate(vec3 v) {
	matrix4x4 res;
	res[0][0] = 1; res[1][0] = 0; res[2][0] = 0; res[3][0] = v.x();
	res[0][1] = 0; res[1][1] = 1; res[2][1] = 0; res[3][1] = v.y();
	res[0][2] = 0; res[1][2] = 0; res[2][2] = 1; res[3][2] = v.z();
	res[0][3] = 0; res[1][3] = 0; res[2][3] = 0; res[3][3] = 1;
	return res;
}

matrix4x4 matrix4x4::scale(float scale) {
	matrix4x4 res;
	res[0][0] = scale;	res[1][0] = 0;		res[2][0] = 0;		res[0][3] = 0;
	res[0][1] = 0;		res[1][1] = scale;	res[2][1] = 0;		res[1][3] = 0;
	res[0][2] = 0;		res[1][2] = 0;		res[2][2] = scale;	res[2][3] = 0;
	res[0][3] = 0;		res[1][3] = 0;		res[2][3] = 0;		res[3][3] = 1;
	return res;
}

matrix4x4 matrix4x4::rotateX(float angle) {
	matrix4x4 res;
	res[0][0] = 1;	res[1][0] = 0;			res[2][0] = 0;			res[3][0] = 0;
	res[0][1] = 0;	res[1][1] = cos(angle);	res[2][1] = -sin(angle);	res[3][1] = 0;
	res[0][2] = 0;	res[1][2] = sin(angle);	res[2][2] = cos(angle);	res[3][2] = 0;
	res[0][3] = 0;	res[1][3] = 0;			res[2][3] = 0;			res[3][3] = 1;
	return res;
}

matrix4x4 matrix4x4::rotateY(float angle) {
	matrix4x4 res;
	res[0][0] = cos(angle);	res[1][0] = 0;	res[2][0] = sin(angle);	res[3][0] = 0;
	res[0][1] = 0;			res[1][1] = 1;	res[2][1] = 0;			res[3][1] = 0;
	res[0][2] = -sin(angle);	res[1][2] = 0;	res[2][2] = cos(angle);	res[3][2] = 0;
	res[0][3] = 0;			res[1][3] = 0;	res[2][3] = 0;			res[3][3] = 1;
	return res;
}

matrix4x4 matrix4x4::rotateZ(float angle) {
	matrix4x4 res;
	res[0][0] = cos(angle);	res[1][0] = -sin(angle);	res[2][0] = 0;	res[3][0] = 0;
	res[0][1] = sin(angle);	res[1][1] = cos(angle);	res[2][1] = 0;	res[3][1] = 0;
	res[0][2] = 0;			res[1][2] = 0;			res[2][2] = 1;	res[3][2] = 0;
	res[0][3] = 0;			res[1][3] = 0;			res[2][3] = 0;	res[3][3] = 1;
	return res;
}

matrix4x4 matrix4x4::rotateXY(float angle) {
	matrix4x4 res;
	res[0][0] = cos(angle);	res[1][0] = -sin(angle);	res[2][0] = 0; res[3][0] = 0;
	res[0][1] = sin(angle);	res[1][1] = cos(angle);	res[2][1] = 0; res[3][1] = 0;
	res[0][2] = 0;			res[1][2] = 0;			res[2][2] = 1; res[3][2] = 0;
	res[0][3] = 0;			res[1][3] = 0;			res[2][3] = 0; res[3][3] = 1;
	return res;
}

matrix4x4 matrix4x4::rotateZW(float angle) {
	matrix4x4 res;
	res[0][0] = 1; res[1][0] = 0; res[2][0] = 0; res[3][0] = 0;
	res[0][1] = 0; res[1][1] = 1; res[2][1] = 0; res[3][1] = 0;
	res[0][2] = 0; res[1][2] = 0; res[2][2] = cos(angle); res[3][2] = -sin(angle);
	res[0][3] = 0; res[1][3] = 0; res[2][3] = sin(angle); res[3][3] = cos(angle);
	return res;
}

float** matrix4x4::data() {
	return reinterpret_cast<float**>(a);
}

float* matrix4x4::operator[](uint32_t index) {
	return a[index];
}

matrix4x4 matrix4x4::operator*(matrix4x4 mat) {
	matrix4x4 res;

	for (uint32_t i = 0; i < 4; ++i) {
		for (uint32_t j = 0; j < 4; ++j) {
			res[i][j] = 0;
			for (uint32_t k = 0; k < 4; ++k) {
				res[i][j] += a[k][i] * mat[j][k];
			}
		}
	}
	return res;
}

vec4 matrix4x4::operator*(vec4 v) {
	vec4 res = {
		v.x() * a[0][0] + v.y() * a[1][0] + v.z() * a[2][0] + v.w() * a[3][0],
		v.x() * a[0][1] + v.y() * a[1][1] + v.z() * a[2][1] + v.w() * a[3][1],
		v.x() * a[0][2] + v.y() * a[1][2] + v.z() * a[2][2] + v.w() * a[3][2],
		v.x() * a[0][3] + v.y() * a[1][3] + v.z() * a[2][3] + v.w() * a[3][3]
	};
	return res;
}
