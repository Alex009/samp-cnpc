/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 */

#define _USE_MATH_DEFINES

#include <math.h>
#include <stdio.h>

#include "quaternion.h"
#include "3DVector.h"

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

void Quaternion::FromVector(float vX,float vY,float vZ)
{
	float euler[3];
	C3DVector vec(vX,vY,vZ);
	vec.ToEuler(euler);
	euler[2] -= (90.f * (float)(M_PI / 180.f)); // gta fix
	for(int i = 0;i < 3;i++) euler[i] = (360.f * (float)(M_PI / 180.f)) - euler[i]; // gta fix
	FromAxisAngle(0.0,0.0,1.0,euler[2]);
	Quaternion quat;
	quat.FromAxisAngle(1.0,0.0,0.0,euler[0]);
	Multiply(quat);


	/*
Vector3 v0(0, 0, 1); // направление модели. 
Vector3 v1(0.7, 0.7, 0); // текущее направление

Real d = v0.dotProduct(v1); 
Real s = Math::Sqrt( (1+d)*2 ); 
Real invs = 1 / s; 
Vector3 c = v0.crossProduct(v1);

Quaternion q; 
q.x = c.x * invs; 
q.y = c.y * invs; 
q.z = c.z * invs; 
q.w = s * 0.5f; 
q.normalise(); 
return q; // результат.

Я опустил проверки на крайние случаи (0 и 180 градусов). 
Real = float.
	// get quaternion
	C3DVector vec(moving_vector[0],moving_vector[1],moving_vector[2]);
	vec.ToEuler(euler);
	euler[2] -= (90.f * (float)(M_PI / 180.f)); // gta fix
	for(int i = 0;i < 3;i++) euler[i] = (360.f * (float)(M_PI / 180.f)) - euler[i]; // gta fix
	q1.FromAxisAngle(0.0,0.0,1.0,euler[2]);
	q2.FromAxisAngle(1.0,0.0,0.0,euler[0]);
	q1.Multiply(q2);
	SetQuaternion(q1.X,q1.Y,q1.Z,q1.W);
	// get real speed
	moving_speed = step / 1200;
	// now speed = 1.0 per 50 ms
	// we need speed per 1 min
	// 1 min = 60 sec = 60_000 ms
	// now speed 1.0 per 1/1200 min
	// find new speed: (step / 1200) * v* ? try...
	// get current speed vector
	moving_vector[0] *= moving_speed;
	moving_vector[1] *= moving_speed;
	moving_vector[2] *= moving_speed;
	// velocity
	*/
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