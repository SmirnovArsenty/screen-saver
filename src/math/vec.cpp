#include <cmath>
#include "vec.h"

vec3::vec3(float x, float y, float z) : m_x{ x }, m_y{ y }, m_z{ z } {}
vec3::vec3(float x) : m_x{ x }, m_y{ x }, m_z{ x } {}
vec3::vec3() : m_x{ 0.f }, m_y{ 0.f }, m_z{ 0.f } {}
float& vec3::x() { return m_x; }
float& vec3::y() { return m_y; }
float& vec3::z() { return m_z; }
vec3 vec3::normalize() {
	float len = sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
	return { m_x / len, m_y / len, m_z / len };
}
vec3 vec3::operator+(vec3 v) {
	return { m_x + v.m_x, m_y + v.m_y, m_z + v.m_z };
}
vec3 vec3::operator-(vec3 v) {
	return { m_x - v.m_x, m_y - v.m_y, m_z - v.m_z };
}
vec3& vec3::operator+=(vec3 v) {
	*this = *this + v;
	return *this;
}
vec3& vec3::operator-=(vec3 v) {
	*this = *this - v;
	return *this;
}
vec3 vec3::operator*(vec3 v) {
	return {
		m_y * v.m_z - m_z * v.m_y,
		m_z * v.m_x - m_x * v.m_z,
		m_x * v.m_y - m_y * v.m_x
	};
}
vec3 vec3::operator-() {
	return { -m_x, -m_y, -m_z };
}

vec4::vec4(float x, float y, float z, float w) : m_x{ x }, m_y{ y }, m_z{ z }, m_w{ w } {}
vec4::vec4(float x) : m_x{ x }, m_y{ x }, m_z{ x }, m_w{ x } {}
vec4::vec4() : m_x{ 0.f }, m_y{ 0.f }, m_z{ 0.f }, m_w{ 0.f } {}
float& vec4::x() { return m_x; }
float& vec4::y() { return m_y; }
float& vec4::z() { return m_z; }
float& vec4::w() { return m_w; }
