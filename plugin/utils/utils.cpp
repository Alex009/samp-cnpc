/*
 *	Copyright (C) 2012 Alex009 (Алексей Михайлов)
 *	License read in license.txt
 *	Description: Utils functions
 *	Last edit at 26.09.12
 */

#define _USE_MATH_DEFINES
// sys
#include <float.h>
#include <math.h>
#include <stdio.h>
// SDK
#include "../SDK/amx/amx.h"
#include "../SDK/plugincommon.h"
// plugin
#include "quaternion.h"
#include "3DVector.h"
#include "utils.h"

#ifndef WIN32
	#include "sys/time.h"
	timeval start_time;
	timeval now_time;

	void TickCountLoad()
	{
		// get tick count ex
		gettimeofday(&start_time, 0);
	}

	int GetTickCount()
	{
		gettimeofday(&now_time,0);
		return (now_time.tv_usec - start_time.tv_usec) / 1000 + 1000 * (now_time.tv_sec - start_time.tv_sec);
	}
#endif

bool IsInRange(float x,float y,float z,float range)
{
	return ((x * x) + (y * y) + (z * z) < (range * range));
}

void EulerConvertToVector(float* euler,float* vector)
{
	C3DVector vec;
	vec.FromEuler(euler);

	vector[0] = vec.X;
	vector[1] = vec.Y;
	vector[2] = vec.Z;
}

void QuaternionConvertToVector(float * quaternion,float * vector)
{
	float euler[3];

	Quaternion q(quaternion[1],quaternion[2],quaternion[3],quaternion[0]);
	q.ToEuler(euler);
	C3DVector vec;
	vec.FromEuler(euler);

	vector[0] = vec.X;
	vector[1] = vec.Y;
	vector[2] = vec.Z;
}

void QuaternionConvertToEuler(float * quaternion,float * euler)
{
	Quaternion q(quaternion[1],quaternion[2],quaternion[3],quaternion[0]);
	q.ToEuler(euler);
}

void AdjustVector(float * input,float Ox,float Oy,float Oz) // Credits Nero_3D
{
	double Angle = -(atan2((double)input[0],(double)input[1]) * (180.0 / M_PI));

	if(45.0 < Angle) 
	{
		*(unsigned long*)(&Ox) ^= *(unsigned long*)(&Oy);
		*(unsigned long*)(&Oy) ^= *(unsigned long*)(&Ox);
		*(unsigned long*)(&Ox) ^= *(unsigned long*)(&Oy);

		if(90.0 < Angle) 
		{
			Ox *= -1;

			if(135.0 < Angle) 
			{
				Ox *= -1;
				*(unsigned long*)(&Ox) ^= *(unsigned long*)(&Oy);
				*(unsigned long*)(&Oy) ^= *(unsigned long*)(&Ox);
				*(unsigned long*)(&Ox) ^= *(unsigned long*)(&Oy);
				Ox *= -1;
			}
		}
	} 
	else if(Angle < 0.0) 
	{
		Oy *= -1;

		if(Angle < -45.0) 
		{
			Ox *= -1;
			*(unsigned long*)(&Ox) ^= *(unsigned long*)(&Oy);
			*(unsigned long*)(&Oy) ^= *(unsigned long*)(&Ox);
			*(unsigned long*)(&Ox) ^= *(unsigned long*)(&Oy);
			Ox *= -1;
				
			if(Angle < -90.0) 
			{
				Ox *= -1;
					
				if(Angle < -135.0) 
				{
					*(unsigned long*)(&Ox) ^= *(unsigned long*)(&Oy);
					*(unsigned long*)(&Oy) ^= *(unsigned long*)(&Ox);
					*(unsigned long*)(&Ox) ^= *(unsigned long*)(&Oy);
				}
			}
		}
	}
	
	input[0] += Ox;
	input[1] += Oy;
	input[2] += Oz;
	return;
}

int GetVehicleModelType(unsigned short model)
{
	switch(model)
	{
	case 432: return VEHICLE_MODEL_RHINO;
	case 520: return VEHICLE_MODEL_HYDRA;
	case 425: return VEHICLE_MODEL_HUNTER;
	case 448:
	case 461:
	case 462:
	case 463:
	case 468:
	case 521:
	case 522:
	case 523:
	case 581:
	case 586: return VEHICLE_MODEL_BIKE;
	case 481:
	case 509:
	case 510:
	case 430:
	case 446:
	case 452:
	case 453:
	case 454:
	case 472:
	case 473:
	case 484:
	case 493:
	case 595:
	case 417:
	case 447:
	case 465:
	case 469:
	case 487:
	case 488:
	case 497:
	case 501:
	case 548:
	case 563:
	case 406:
	case 444:
	case 556:
	case 557:
	case 573:
	case 460:
	case 464:
	case 476:
	case 511:
	case 512:
	case 513:
	case 519:
	case 539:
	case 553:
	case 577:
	case 592:
	case 593:
	case 471: return VEHICLE_MODEL_OTHER;
	default: return VEHICLE_MODEL_CAR;
	}
}

int GetWeaponType(unsigned int weapon)
{
	switch(weapon)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15: return WEAPON_TYPE_MEELE;
	case 16:
	case 17:
	case 18: return WEAPON_TYPE_PROJECTILE;
	case 35:
	case 36:
	case 37: return WEAPON_TYPE_MASS;
	default: return WEAPON_TYPE_SHOOT;
	}
}

void rotate_vector(float& x,float& y,float& z,float angle_xy,float angle_z)
{
	float _x = x*cosf(angle_xy) - y*sinf(angle_xy);
	float _y = x*sinf(angle_xy) + y*cosf(angle_xy);
	
	float d = sqrt(_x*_x + _y*_y);
	
	float _d = d*cosf(angle_z) - z*sinf(angle_z);
	float _z = d*sinf(angle_z) + z*cosf(angle_z);
	
	float fix = _d / d;
	
 	x = _x * fix;
	y = _y * fix;
	z = _z;
}

float scal_prod(float x1,float y1,float z1,float x2,float y2,float z2)
{
	return (x1*x2 + y1*y2 + z1*z2);
}

void vect_prod(float x1,float y1,float z1,float x2,float y2,float z2,float* x3,float* y3,float* z3)
{
	*x3 = y1*z2 - y2*z1;
	*y3 = z1*x2 - z2*x1;
	*z3 = x1*y2 - x2*y1;
}

float vector_len(float x,float y,float z)
{
	return sqrt(x*x + y*y + z*z);
}

bool GetDistanceFromRayToPoint(float rsx,float rsy,float rsz,float rvx,float rvy,float rvz,float px,float py,float pz,float* result)
{
	float wx,wy,wz;
	float rx,ry,rz;
	float scal;

	wx = px - rsx;
	wy = py - rsy;
	wz = pz - rsz;

	scal = scal_prod(wx,wy,wz,rvx,rvy,rvz);

	if(scal < 0) return false;
	else
	{
		vect_prod(rvx,rvy,rvz,wx,wy,wz,&rx,&ry,&rz);
		*result = sqrt(rx*rx + ry*ry + rz*rz);
		return true;
	}
}

float AngleBetweenVectors(float x1,float y1,float z1,float x2,float y2,float z2)
{
	return acos(scal_prod(x1,y1,z1,x2,y2,z2)/(vector_len(x1,y1,z1)*(vector_len(x2,y2,z2))));
}