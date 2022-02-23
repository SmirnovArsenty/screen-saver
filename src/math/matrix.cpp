#pragma once

#include "matrix.h"
#include <cmath>

matrix4x4 matrix4x4::rotateXY(float angle) {
	matrix4x4 res;
	res.a[0][0] = cos(angle); res.a[0][1] = -sin(angle); res.a[0][2] = 0; res.a[0][3] = 0;
	res.a[1][0] = sin(angle); res.a[1][1] = cos(angle); res.a[1][2] = 0; res.a[1][3] = 0;
	res.a[2][0] = 0; res.a[2][1] = 0; res.a[2][2] = 1; res.a[2][3] = 0;
	res.a[3][0] = 0; res.a[3][1] = 0; res.a[3][2] = 0; res.a[3][3] = 1;
	return res;
}

matrix4x4 matrix4x4::rotateZW(float angle) {
	matrix4x4 res;
	res.a[0][0] = 1; res.a[0][1] = 0; res.a[0][2] = 0; res.a[0][3] = 0;
	res.a[1][0] = 0; res.a[1][1] = 1; res.a[1][2] = 0; res.a[1][3] = 0;
	res.a[2][0] = 0; res.a[2][1] = 0; res.a[2][2] = cos(angle); res.a[2][3] = -sin(angle);
	res.a[3][0] = 0; res.a[3][1] = 0; res.a[3][2] = sin(angle); res.a[3][3] = cos(angle);
	return res;
}
