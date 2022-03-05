#pragma once

#include "vec.h"
#include "matrix.h"
#include <cmath>

matrix4x4 matrix4x4::perspective(float l, float r, float b, float t, float n, float f) {
	matrix4x4 res;

	res[0][0] = 2*n/(r-l);	res[1][0] = 0;			res[2][0] = (r+l)/(r-l);	res[3][0] = 0;
	res[0][1] = 0;			res[1][1] = 2*n/(t-b);	res[2][1] = (t+b)/(t-b);	res[3][1] = 0;
	res[0][2] = 0;			res[1][2] = 0;			res[2][2] = -(f+n)/(f-n);	res[3][2] = -(2*f*n)/(f-n);
	res[0][3] = 0;			res[1][3] = 0;			res[2][3] = -1;				res[3][3] = 0;

	return res;
}

matrix4x4 matrix4x4::view(vec3 pos, vec3 at) {
	matrix4x4 rot;

	vec3 up(0, 1, 0);
	vec3 forward = (at - pos).normalize();
	vec3 right = (forward * up).normalize();
	up = (right * forward).normalize();

	rot[0][0] = right.x();	rot[1][0] = up.x();	rot[2][0] = forward.x();	rot[3][0] = 0;
	rot[0][1] = right.y();	rot[1][1] = up.y();	rot[2][1] = forward.y();	rot[3][1] = 0;
	rot[0][2] = right.z();	rot[1][2] = up.z();	rot[2][2] = forward.z();	rot[3][2] = 0;
	rot[0][3] = 0;			rot[1][3] = 0;		rot[2][3] = 0;				rot[3][3] = 1;

	matrix4x4 translate;

	translate[0][0] = 1;
	translate[1][1] = 1;
	translate[2][2] = 1;
	translate[3][3] = 1;
	translate[3][0] = pos.x();
	translate[3][1] = pos.y();
	translate[3][2] = pos.z();

	return rot * translate;
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
