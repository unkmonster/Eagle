#pragma once

struct Vec3 {
	float x, y, z;

	Vec3() = default;
	Vec3(float x_, float y_, float z_): x(x_), y(y_), z(z_) {}
};

struct Vec4 : public Vec3 {
	float w;
	Vec4() = default;
	Vec4(float x_, float y_, float z_, float w_): Vec3(x_, y_, z_), w(w_) {}
};

struct Matrix16 {
	float data[4][4];
};