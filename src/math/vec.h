#pragma once

#include <cstdint>

class matrix4x4;

class vec3 {
private:
	float m_x, m_y, m_z;
public:
	vec3(float x, float y, float z);
	vec3(float x);
	vec3();

	float& x();
	float& y();
	float& z();

	vec3 normalize();

	vec3 operator+(vec3 v);
	vec3 operator-(vec3 v);
	vec3& operator+=(vec3 v);
	vec3& operator-=(vec3 v);
	vec3 operator*(vec3 v);
	vec3 operator-();
};

class vec4 {
private:
	float m_x, m_y, m_z, m_w;
public:
	vec4(float x, float y, float z, float w);
	vec4(float x);
	vec4();

	float& x();
	float& y();
	float& z();
	float& w();
};
