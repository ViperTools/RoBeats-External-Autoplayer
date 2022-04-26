#pragma once
#include <math.h>

struct Vector3 {
	double x, y, z;
	double magnitude;
	Vector3(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
		magnitude = sqrt(x * x + y * y + z * z);
	}

	Vector3 operator-(const Vector3& b) {
		return { x - b.x, y - b.y, z - b.z };
	}

	bool operator!=(const Vector3& b) {
		return x != b.x || y != b.y || z != b.z;
	}
};