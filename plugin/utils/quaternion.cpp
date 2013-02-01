/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 */

#define _USE_MATH_DEFINES

#include "quaternion.h"
#include <math.h>
#include <stdio.h>

float clamp(float X,float min,float max);

Quaternion::Quaternion()
{
	X = 0;
	Y = 0;
	Z = 0;
	W = 0;
}

Quaternion::Quaternion(float x,float y,float z,float w)
{
	X = x;
	Y = y;
	Z = z;
	W = w;
}

Quaternion::~Quaternion()
{
}

Quaternion operator * (const Quaternion &a, const Quaternion &b)
{
	float w,x,y,z;
	
	w = a.W*b.W - (a.X*b.X + a.Y*b.Y + a.Z*b.Z);
	
	x = a.W*b.X + b.W*a.X + a.Y*b.Z - a.Z*b.Y;
	y = a.W*b.Y + b.W*a.Y + a.Z*b.X - a.X*b.Z;
	z = a.W*b.Z + b.W*a.Z + a.X*b.Y - a.Y*b.X;
	
	return Quaternion(x,y,z,w); 
}

Quaternion operator + (const Quaternion &a, const Quaternion &b)
{
	float w,x,y,z;
	
	w = a.W + b.W;
	
	x = a.X + b.X;
	y = a.Y + b.Y;
	z = a.Z + b.Z;
	
	return Quaternion(x,y,z,w); 
}

void Quaternion::Normalize()
{
	float norme = sqrtf(W*W + X*X + Y*Y + Z*Z);
	
	if(norme == 0.0)
	{
		W = 1.0; 
		X = Y = Z = 0.0;
	}
	else
	{
		float recip = 1.f/norme;

		W *= recip;
		X *= recip;
		Y *= recip;
		Z *= recip;
	}
}

void Quaternion::FromAxisAngle(float axis_x,float axis_y,float axis_z,float angle)
{
	float sin_a = sinf(angle / 2.f);
	float cos_a = cosf(angle / 2.f);

	W = cos_a;
	X = axis_x * sin_a;
	Y = axis_y * sin_a;
	Z = axis_z * sin_a;

	Normalize();
}

void Quaternion::ToAxisAngle(float * axis_x,float * axis_y,float * axis_z,float * angle)
{
	*angle = acosf(W);

	// pre-compute to save time
	float sinf_theta_inv = 1.f / sinf(*angle);

	// now the vector
	*axis_x = X * sinf_theta_inv;
	*axis_y = Y * sinf_theta_inv;
	*axis_z = Z * sinf_theta_inv;

	// multiply by 2
	*angle *= 2;
}

void Quaternion::FromEuler(float * euler)
{
	float cosP = cosf(euler[0] / 2.f);
	float sinP = sinf(euler[0] / 2.f);
	float cosR = cosf(euler[1] / 2.f);
	float sinR = sinf(euler[1] / 2.f);
	float cosY = cosf(euler[2] / 2.f);
	float sinY = sinf(euler[2] / 2.f);

	W = cosR * cosP * cosY + sinR * sinP * sinY;
	
	X = cosR * sinP * cosY + sinR * cosP * sinY;
	Y = cosR * cosP * sinY - sinR * sinP * cosY;
	Z = sinR * cosP * cosY - cosR * sinP * sinY;

	Normalize();
}

void Quaternion::ToEuler(float * euler)
{
	float sqw = W*W;
    float sqx = X*X;
    float sqy = Y*Y;
    float sqz = Z*Z;
    float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
    //если normalised, - один, в противном случае - показатель коррекции
    float test = X*Y + Z*W;
    if (test > 0.499*unit)
    { // singularity at north pole - особенность на северном полюсе
        euler[0] = 2.f * atan2f(X,W);
        euler[2] = (float)M_PI_2;
        euler[1] = 0;
        return;
    }
    if (test < -0.499*unit)
    { // singularity at south pole - особенность на южном полюсе
        euler[0] = -2.f * atan2f(X,W);
        euler[2] = -(float)M_PI_2;
        euler[1] = 0;
		return;
    }
    euler[0] = atan2f(2.f * Y*W - 2.f * X*Z, sqx - sqy - sqz + sqw);
    euler[2] = -asinf(2.f * test/unit);
    euler[1] = atan2f(2.f * X*W - 2.f * Y*Z, -sqx + sqy - sqz + sqw);
}

void Quaternion::Multiply(Quaternion a)
{
	float w,x,y,z;
	
	w = a.W*W - (a.X*X + a.Y*Y + a.Z*Z);
	
	x = a.W*X + W*a.X + a.Y*Z - a.Z*Y;
	y = a.W*Y + W*a.Y + a.Z*X - a.X*Z;
	z = a.W*Z + W*a.Z + a.X*Y - a.Y*X;
	
	W = w;
	X = x;
	Y = y;
	Z = z;

	Normalize();
}

// utils

float clamp(float X,float min,float max)
{
	if(X < min) return min;
	if(X > max) return max;
	return X;
}