#pragma once

#include <ostream>
#include <cmath>

#include <imgui.h>

struct Vec2 {
	float x{}, y{};

	Vec2() = default;
	Vec2(float a, float b): x(a), y(b) {}
	Vec2(const ImVec2& v): Vec2(v.x, v.y) {}
	
	operator ImVec2() const {
		return ImVec2(x, y);
	}

	Vec2 operator+ (const Vec2& rhs) const {
		return {x + rhs.x, y + rhs.y};
	}
};
struct Vec3: public Vec2 {
	float z{};

	Vec3() = default;
	Vec3(float x_, float y_, float z_): Vec2(x_, y_), z(z_) {}
	Vec3(const Vec2& v, float z_): Vec2(v), z(z_) {}
	Vec3(const float* data): Vec3(data[0], data[1], data[2]) {}

	Vec3 operator+ (const Vec3& rhs) const {
		return Vec3(this->Vec2::operator+(rhs), z + rhs.z);
	}
};

struct Vec4 : public Vec3 {
	float w{};
	Vec4() = default;
	Vec4(float x_, float y_, float z_, float w_): Vec3(x_, y_, z_), w(w_) {}

	operator ImVec4() const {
		return ImVec4(x, y, z, w);
	}
};

struct Matrix16 {
	float data[4][4];
};

inline std::ostream& operator<<(std::ostream& os, const Vec3& vec) {
	return os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
}

inline std::ostream& operator<<(std::ostream& os, const Vec4& vec) {
	return os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
}

inline double distance(const Vec2& p1, const Vec2& p2) {
	return sqrt(pow(fabs(p1.x - p2.x), 2) + pow(fabs(p1.y - p2.y), 2));
}

inline Vec3 operator*(const Vec3& v, const Matrix16& matrix) {
	return {
		v.x * matrix.data[0][0] + v.y * matrix.data[1][0] + v.z * matrix.data[2][0],
		v.x * matrix.data[0][1] + v.y * matrix.data[1][1] + v.z * matrix.data[2][1],
		v.x * matrix.data[0][2] + v.y * matrix.data[1][2] + v.z * matrix.data[2][2],
	};
}

inline bool InBound(const Vec2& point, const Vec2& min, const Vec2& max) {
	return point.x > min.x && point.x < max.x && point.y > min.y && point.y < max.y;
}