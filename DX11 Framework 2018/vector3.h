#pragma once
#include <iostream>
#include <math.h>
#include <assert.h>

class vector3 {
public:
	float x, y, z;

	//Constructors
	vector3();
	vector3(float _x, float _y, float _z);
	vector3(const vector3& vec);

	//Arithmetic operators
	vector3 operator+(const vector3& vec);
	vector3 &operator+=(const vector3& vec);
	vector3 operator-(const vector3& vec);
	vector3& operator-=(const vector3& vec);
	vector3 operator*(const vector3& vec);
	vector3& operator*=(const vector3& vec);
	vector3 operator/(const vector3& vec);
	vector3& operator/=(const vector3& vec);
	vector3 operator=(const vector3& vec);

	//Vector operations
	float dot_product(const vector3& vec);	//Scalar dot product
	vector3 cross_product(const vector3& vec);
	vector3 normalisation();	//Normalised vector
	vector3 normalization();	//American normalised vector

	//Scalar operations
	float square();
	float distance(const vector3& vec);
	float magnitude();

	//Display operations
	float GetX();	//return x
	float GetY();	//return y
	float GetZ();	//return z
	void disp();	//Print vector
};