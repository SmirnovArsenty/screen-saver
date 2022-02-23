#include "vec.h"

vec4::vec4(float x, float y, float z, float w) : x{ x }, y{ y }, z{ z }, w{ w } {}
vec4::vec4(float x) : x{ x }, y{ x }, z{ x }, w{ x } {}
vec4::vec4() : x{ 0 }, y{ 0 }, z{ 0 }, w{ 0 } {}
