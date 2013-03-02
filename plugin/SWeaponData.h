/*
 *	Copyright (C) 2013 Alex009
 *	License read in license.txt
 *	Description: weapons information
 */

#pragma once

#include "SAMP/defines.h"

#define WEAPON_TYPE_MELEE			1
#define WEAPON_TYPE_SHOOT			2
#define WEAPON_TYPE_PROJECTILE		3
#define WEAPON_TYPE_RAKET			4
#define WEAPON_TYPE_SPECIAL			5

struct SWeaponData
{
	unsigned char type;
	float cam_pos_fix[3];
	float vector_angles[2];
	float damage;
	float shoot_time;
};

SWeaponData* GetWeaponData(unsigned char weaponid);