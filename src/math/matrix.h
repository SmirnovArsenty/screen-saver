#pragma once

class matrix4x4 {
private:
	float a[4][4]{};
	matrix4x4();
public:
	static matrix4x4 rotateXY(float angle);
	static matrix4x4 rotateZW(float angle);
};
