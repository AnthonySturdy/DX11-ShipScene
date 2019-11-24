#include "vector3.h"

vector3::vector3() {
	x = y = z = 0.0f;
}

vector3::vector3(float _x, float _y, float _z) {
	this->x = _x;
	this->y = _y;
	this->z = _z;
}

vector3::vector3(const vector3& vec) {
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

vector3 vector3::operator+(const vector3& vec) {
	return vector3(x + vec.x, y + vec.y, z + vec.z);
}

vector3& vector3::operator+=(const vector3& vec) {
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}

vector3 vector3::operator-(const vector3& vec) {
	return vector3(x - vec.x, y - vec.y, z - vec.z);
}

vector3& vector3::operator-=(const vector3& vec) {
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}

vector3 vector3::operator*(const vector3& vec) {
	return vector3(x * vec.x, y * vec.y, z * vec.z);
}

vector3& vector3::operator*=(const vector3& vec) {
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;
	return *this;
}

vector3 vector3::operator/(const vector3& vec) {
	//If any are 0, program will terminate
	assert(vec.x != 0);
	assert(vec.y != 0);
	assert(vec.z != 0);
	return vector3(x / vec.x, y / vec.y, z / vec.z);
}

vector3& vector3::operator/=(const vector3& vec) {
	//If any are 0, program will terminate
	assert(vec.x != 0);
	assert(vec.y != 0);
	assert(vec.z != 0);
	x /= vec.x;
	y /= vec.y;
	z /= vec.z;
	return *this;
}

vector3 vector3::operator=(const vector3& vec) {
	return vector3(x = vec.x, y = vec.y, z = vec.z);
}

float vector3::dot_product(const vector3& vec) {
	return (x * vec.x + y * vec.y + z * vec.z);
}

vector3 vector3::cross_product(const vector3& vec) {
	float nA = y * vec.z - z * vec.y;
	float nB = z * vec.x - x * vec.z;
	float nC = x * vec.y - y * vec.x;
	return vector3(nA, nB, nC);
}

vector3 vector3::normalisation() {
	return vector3(x / magnitude(), y / magnitude(), z / magnitude());
}

vector3 vector3::normalization() {
	return normalisation();
}

float vector3::square() {
	return x*x+y*y+z*z;
}

float vector3::distance(const vector3& vec) {
	return 0.0f;
}

float vector3::magnitude() {
	return sqrt(square());
}

float vector3::GetX() {
	return x;
}

float vector3::GetY() {
	return y;
}

float vector3::GetZ() {
	return z;
}

void vector3::disp() {
	std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
}