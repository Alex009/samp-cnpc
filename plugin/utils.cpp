/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 */

#define _USE_MATH_DEFINES

// SDK
#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include "utils.h"
#include "utils/quaternion.h"
#include "utils/3DVector.h"

float p_atan2(float x,float y);

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

void UpVectorFromFrontVector(float * input,float * output) // just brainfuck
{
	/*
	v3 = atan2(*(float *)(a1 + 4), *(float *)a1) - 1.570796370506287;
	v4 = sin(v3);
	v5 = cos(v3);
	*(float *)a2 = *(float *)(a1 + 4) * 0.0 - v5 * *(float *)(a1 + 8);
	*(float *)(a2 + 4) = v4 * *(float *)(a1 + 8) - *(float *)a1 * 0.0;
	*(float *)(a2 + 8) = v5 * *(float *)a1 - v4 * *(float *)(a1 + 4);
	*/
	float tmp[3];
	tmp[0] = atan2f(input[1],input[0]) - (float)M_PI_2;
	tmp[1] = sinf(tmp[0]);
	tmp[2] = cosf(tmp[0]);

	output[0] = input[1] * 0.f - tmp[2] * input[2];
	output[1] = tmp[1] * input[2] - input[0] * 0.f;
	output[2] = tmp[2] * input[0] - tmp[1] * input[1];
}

void AdjustVector(float * input,float Ox,float Oy,float Oz) // Credits Nero_3D
{	
	float Angle = -p_atan2(input[0],input[1]);

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