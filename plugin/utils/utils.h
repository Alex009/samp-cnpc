/*
 *	Copyright (C) 2012 Alex009 (Алексей Михайлов)
 *	License read in license.txt
 *	Description: Utils functions
 *	Last edit at 26.09.12
 */

enum
{
	VEHICLE_MODEL_RHINO,
	VEHICLE_MODEL_HYDRA,
	VEHICLE_MODEL_HUNTER,
	VEHICLE_MODEL_CAR,
	VEHICLE_MODEL_BIKE,
	VEHICLE_MODEL_OTHER
};

enum
{
	WEAPON_TYPE_PROJECTILE,
	WEAPON_TYPE_MEELE,
	WEAPON_TYPE_SHOOT,
	WEAPON_TYPE_MASS
};

bool IsInRange(float x,float y,float z,float range);
void EulerConvertToVector(float* euler,float* vector);
void QuaternionConvertToVector(float * quaternion,float * vector);
void QuaternionConvertToEuler(float * quaternion,float * euler);
void AdjustVector(float * input,float Ox,float Oy,float Oz); // Credits Nero_3D
int GetVehicleModelType(unsigned short model);
int GetWeaponType(unsigned int weapon);