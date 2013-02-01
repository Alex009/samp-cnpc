/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 */

#define _USE_MATH_DEFINES

#include "3DVector.h"
#include <math.h>
#include <stdio.h>

C3DVector::C3DVector()
{
	X = 0;
	Y = 0;
	Z = 0;
}

C3DVector::C3DVector(float x,float y,float z)
{
	X = x;
	Y = y;
	Z = z;
}

C3DVector::~C3DVector()
{
}

C3DVector operator * (const C3DVector &a, const C3DVector &b)
{
	float x,y,z;
	
	// do...
	
	return C3DVector(x,y,z); 
}

C3DVector operator + (const C3DVector &a, const C3DVector &b)
{
	float x,y,z;
	
	x = a.X + b.X;
	y = a.Y + b.Y;
	z = a.Z + b.Z;
	
	return C3DVector(x,y,z); 
}

void C3DVector::Normalize()
{
	float norme = sqrtf(X*X + Y*Y + Z*Z);
	
	if(norme == 0.0)
	{
		X = Y = Z = 0.0;
	}
	else
	{
		float recip = 1.f/norme;

		X *= recip;
		Y *= recip;
		Z *= recip;
	}
}

void C3DVector::FromEuler(float * euler)
{
	float axis[3];
	X = 0.f;
	Y = 1.f;
	Z = 0.f;

	axis[0] = 1.f;
	axis[1] = 0.f;
	axis[2] = 0.f;
	MoveAroundAxis(axis,euler[0]);
	
	axis[0] = 0.f;
	axis[1] = 1.f;
	axis[2] = 0.f;
	MoveAroundAxis(axis,euler[1]);
	
	axis[0] = 0.f;
	axis[1] = 0.f;
	axis[2] = 1.f;
	MoveAroundAxis(axis,euler[2]);
	
	Normalize();
}

void C3DVector::ToEuler(float * euler) // special for quaternion convert
{
	euler[0] = atan2f(Z,sqrtf(X*X + Y*Y));
	euler[1] = 0.f;
	euler[2] = atan2f(Y,X);
	
	// fuck this!
	// euler[0] = atan2f(Z,X);
	// euler[1] = atan2f(Z,Y);
	// euler[2] = atan2f(Y,X);
}

void C3DVector::MoveAroundAxis(float * axis,float angle)
{
	float cosa,sina,tmp_x,tmp_y,tmp_z;
	// angle
	cosa = cosf(angle);
	sina = sinf(angle);
	// Найдем новую позицию X для вращаемой точки
	tmp_x = (cosa + (1 - cosa) * axis[0] * axis[0]) * X;
	tmp_x += ((1 - cosa) * axis[0] * axis[1] - axis[2] * sina)	* Y;
	tmp_x += ((1 - cosa) * axis[0] * axis[2] + axis[1] * sina)	* Z;
	// Найдем позицию Y
	tmp_y  = ((1 - cosa) * axis[0] * axis[1] + axis[2] * sina)	* X;
	tmp_y += (cosa + (1 - cosa) * axis[1] * axis[1])	* Y;
	tmp_y += ((1 - cosa) * axis[1] * axis[2] - axis[0] * sina)	* Z;
	// И позицию Z
	tmp_z = ((1 - cosa) * axis[0] * axis[2] - axis[1] * sina)	* X;
	tmp_z += ((1 - cosa) * axis[1] * axis[2] + axis[0] * sina)	* Y;
	tmp_z += (cosa + (1 - cosa) * axis[2] * axis[2]) * Z;
	// set vector
	X = tmp_x;
	Y = tmp_y;
	Z = tmp_z;
}