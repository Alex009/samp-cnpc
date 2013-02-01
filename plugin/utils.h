/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: additional functions
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
void UpVectorFromFrontVector(float * input,float * output);
void AdjustVector(float * input,float Ox,float Oy,float Oz); // Credits Nero_3D
int GetVehicleModelType(unsigned short model);
int GetWeaponType(unsigned int weapon);