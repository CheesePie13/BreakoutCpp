#pragma once

#include <math.h> 
#include "types.hpp"

struct Vec2 {
	float32 x, y;
};

struct Vec2Int {
	int32 x, y;
};

//
// Constants
//

const Vec2 Vec2_ONE = {1.0f, 1.0f};

//
// Vec2 Operators
//

inline Vec2 operator+(const Vec2& a, const Vec2& b) {
	Vec2 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	return result;
}

inline Vec2 operator-(const Vec2& a, const Vec2& b) {
	Vec2 result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	return result;
}

inline Vec2 operator*(const Vec2& v, const float32& s) {
	Vec2 result;
	result.x = v.x * s;
	result.y = v.y * s;
	return result;
}

inline Vec2 operator*(const float32& s, const Vec2& v) {
	Vec2 result;
	result.x = v.x * s;
	result.y = v.y * s;
	return result;
}

inline Vec2 operator/(const float32& s, const Vec2& v) {
	Vec2 result;
	result.x = v.x / s;
	result.y = v.y / s;
	return result;
}

inline Vec2 operator/(const Vec2& v, const float32& s) {
	Vec2 result;
	result.x = v.x / s;
	result.y = v.y / s;
	return result;
}

//
// Vec2 Functions
//

inline float32 dot(Vec2 a, Vec2 b) {
	float32 result;
	result = (a.x * b.x) + (a.y * b.y);
	return result;
}

inline float32 magnitude(Vec2 v) {
	float32 result;
	result = sqrt(v.x * v.x + v.y * v.y);
	return result;
}

inline Vec2 normalize(Vec2 v) {
	Vec2 result;
	result = v / magnitude(v);
	return result;
}

// Normal must be normalized
inline Vec2 reflect(Vec2 v, Vec2 normal) {
	Vec2 result;
	result = v - 2.0f * dot(v, normal) * normal;
	return result;
}

inline Vec2 rotate(Vec2 v, float32 radians) {
	Vec2 result;
	result.x = v.x * cos(radians) - v.y * sin(radians);
	result.y = v.x * sin(radians) + v.y * cos(radians);
	return result;
} 
