#pragma once

#include <ostream>

#include <imgui.h>

struct Vec2 {
	float x, y;

	Vec2() = default;
	Vec2(float a, float b): x(a), y(b) {}
	
	explicit operator ImVec2() const {
		return ImVec2(x, y);
	}
};
struct Vec3: public Vec2 {
	float z;

	Vec3() = default;
	Vec3(float x_, float y_, float z_): Vec2(x_, y_), z(z_) {}
};

struct Vec4 : public Vec3 {
	float w;
	Vec4() = default;
	Vec4(float x_, float y_, float z_, float w_): Vec3(x_, y_, z_), w(w_) {}
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