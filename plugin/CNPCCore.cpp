/*
 *	Copyright (C) 2013 Alex009
 *	License read in license.txt
 */

#define _USE_MATH_DEFINES

#include "SAMP/CSAMP.h"
#include "utils/quaternion.h"
#include "CCallbacks.h"
#include "CNPCCore.h"
#include "hooks.h"
#include "defines.h"
#include <math.h>
#include <stdio.h>
#include <stddef.h>

extern	CSAMP*		pSaMp;
extern	CHooks*		pHooks;
extern	CCallbacks*	pCallbacks;
extern	bool		VisualDeath;
extern	unsigned short MaxPlayers;

CNPCCore::CNPCCore(unsigned short id,char* name)
{
	local_id = id;

	pHooks->ClientConnect(pSaMp->players,id,name);

	local_class = pSaMp->players->player[local_id];

	SetState(PLAYER_STATE_SPECTATING);
	SetPosition(0.0,0.0,0.0);
	SetQuaternion(0.0,0.0,0.0,0.0);
	SetAngle(0.0);
	SetVelocity(0.0,0.0,0.0);
	SetKeys(0,0,0);
	SetSpecialAction(0);
	SetWeapon(0);
	SetSkin(0);
	SetSurfingVehicle(INVALID_VEHICLE_ID);
	SetVehicleSiren(0);
	SetVehicleUnique(0);
	SetVehicleTrailer(INVALID_VEHICLE_ID);
	SetHealth(100.0);
	SetArmour(0.0);

	last_update = 0;

	local_class->AimSync.Unknown1 = 0x55;
}

CNPCCore::~CNPCCore()
{
	pHooks->ClientDisconnect(pSaMp->players,local_id,0);
}

void CNPCCore::Spawn()
{
	for(int id = 0;id < MaxPlayers;id++)
	{
		if(pSaMp->players->player[id]) pHooks->PlayerStreamOut(pSaMp->players->player[id],local_id);
	}
	pHooks->ClientSpawn(local_class);
	// set nulled data
	SetVelocity(0.0,0.0,0.0);
	SetKeys(0,0,0);
	SetSpecialAction(0);
	SetWeapon(0);
	SetSurfingVehicle(INVALID_VEHICLE_ID);
	SetVehicleSiren(0);
	SetVehicleUnique(0);
	SetVehicleTrailer(INVALID_VEHICLE_ID);
	SetHealth(100.0);
	SetArmour(0.0);
	// complete
	SetState(PLAYER_STATE_ONFOOT);
	pCallbacks->Spawn(local_id);
}

void CNPCCore::Kill(unsigned short killerid,unsigned char reason)
{
	SetHealth(0.0);
	SetState(PLAYER_STATE_WASTED);
	if(VisualDeath) pHooks->ClientDeath(local_class,reason,killerid);
	pCallbacks->Death(local_id,killerid,reason);
}

void CNPCCore::SetState(unsigned char state)
{
	local_class->State = state;
}

unsigned char CNPCCore::GetState()
{
	return local_class->State;
}

void CNPCCore::SetSkin(unsigned long skin)
{
	float x,y,z,angle,health,armour;
	local_class->Skin = skin;
	if(GetState() != PLAYER_STATE_SPECTATING)
	{
		// respawn
		GetPosition(&x,&y,&z);
		angle = GetAngle();
		health = GetHealth();
		armour = GetArmour();
		pHooks->ClientSpawn(local_class);
		SetPosition(x,y,z);
		SetAngle(angle);
		SetHealth(health);
		SetArmour(armour);
	}
}

unsigned long CNPCCore::GetSkin()
{
	return local_class->Skin;
}

void CNPCCore::SetInterior(unsigned long interior)
{
	local_class->Interior = interior;
}

unsigned long CNPCCore::GetInterior()
{
	return local_class->Interior;
}

void CNPCCore::PutInVehicle(unsigned short vehicleid,unsigned char seat)
{
	if(vehicleid == 0)
	{
		local_class->FootSync.AnimationIndex = 0;
		local_class->FootSync.KeysLR = 0;
		local_class->FootSync.KeysOther = 0;
		local_class->FootSync.KeysUD = 0;
		local_class->FootSync.SpecialAction = 0;
		local_class->FootSync.SurfingInfo = 0xFFFF;
		SetState(PLAYER_STATE_ONFOOT);
		return;
	}
	if(pSaMp->vehicles->vehicle[vehicleid])
	{
		if(seat == 0)
		{
			pSaMp->vehicles->vehicle[vehicleid]->Driver = local_id;
			
			local_class->VehicleId = vehicleid;
			local_class->VehicleSeat = seat;

			local_class->InCarSync.VehicleId = vehicleid;

			SetState(PLAYER_STATE_DRIVER);

			SetPosition(pSaMp->vehicles->vehicle[vehicleid]->Position[0],
				pSaMp->vehicles->vehicle[vehicleid]->Position[1],
				pSaMp->vehicles->vehicle[vehicleid]->Position[2]);
			SetKeys(0,0,0);
			SetHealth(local_class->Health);
			SetArmour(local_class->Armour);
			SetWeapon(0);

			local_class->InCarSync.SirenState = 0;
			local_class->InCarSync.TrailerId_or_ThrustAngle = INVALID_VEHICLE_ID;
			local_class->InCarSync.TrainSpeed = 0.0;

			local_class->InCarSync.Health = pSaMp->vehicles->vehicle[vehicleid]->Health;
		}
		else
		{
			local_class->VehicleId = vehicleid;
			local_class->VehicleSeat = seat;

			local_class->PassangerSync.VehicleId = vehicleid;
			local_class->PassangerSync.Seat = seat;

			SetState(PLAYER_STATE_PASSENGER);

			SetPosition(pSaMp->vehicles->vehicle[vehicleid]->Position[0],
				pSaMp->vehicles->vehicle[vehicleid]->Position[1],
				pSaMp->vehicles->vehicle[vehicleid]->Position[2]);
			SetKeys(0,0,0);
			SetHealth(local_class->Health);
			SetArmour(local_class->Armour);
			SetWeapon(0);
		}
	}
}

void CNPCCore::SetPosition(float x,float y,float z)
{
	local_class->Position[0] = x;
	local_class->Position[1] = y;
	local_class->Position[2] = z;
	local_class->FootSync.Position[0] = x;
	local_class->FootSync.Position[1] = y;
	local_class->FootSync.Position[2] = z;
	local_class->InCarSync.Position[0] = x;
	local_class->InCarSync.Position[1] = y;
	local_class->InCarSync.Position[2] = z;
	local_class->PassangerSync.Position[0] = x;
	local_class->PassangerSync.Position[1] = y;
	local_class->PassangerSync.Position[2] = z;
	// vehicle data
	unsigned short vehicleid = local_class->VehicleId;
	CVehicle * veh = pSaMp->vehicles->vehicle[vehicleid];
	if(veh)
	{
		veh->Position[0] = x;
		veh->Position[1] = y;
		veh->Position[2] = z;
		// train fix
		int model = veh->Model;
		if((model == 537) || (model == 538))
		{
			for(int i = 0;i < 3;i++)
			{
				vehicleid++;
				CVehicle * veh = pSaMp->vehicles->vehicle[vehicleid];
				if(veh)
				{
					veh->Position[0] = x;
					veh->Position[1] = y;
					veh->Position[2] = z;
				}
			}
		}
	}
}

void CNPCCore::GetPosition(float* x,float* y,float* z)
{
	*x = local_class->Position[0];
	*y = local_class->Position[1];
	*z = local_class->Position[2];
}

void CNPCCore::SetQuaternion(float x,float y,float z,float s)
{
	local_class->FootSync.Quaternion[0] = s;
	local_class->FootSync.Quaternion[1] = x;
	local_class->FootSync.Quaternion[2] = y;
	local_class->FootSync.Quaternion[3] = z;
	local_class->InCarSync.Quaternion[0] = s;
	local_class->InCarSync.Quaternion[1] = x;
	local_class->InCarSync.Quaternion[2] = y;
	local_class->InCarSync.Quaternion[3] = z;
}

void CNPCCore::GetQuaternion(float* x,float* y,float* z,float* s)
{
	switch(local_class->State)
	{
	case PLAYER_STATE_ONFOOT:
		{
			*s = local_class->FootSync.Quaternion[0];
			*x = local_class->FootSync.Quaternion[1];
			*y = local_class->FootSync.Quaternion[2];
			*z = local_class->FootSync.Quaternion[3];
		}
	case PLAYER_STATE_DRIVER:
		{
			*s = local_class->InCarSync.Quaternion[0];
			*x = local_class->InCarSync.Quaternion[1];
			*y = local_class->InCarSync.Quaternion[2];
			*z = local_class->InCarSync.Quaternion[3];
		}
	}
}

void CNPCCore::SetAngle(float angle)
{
	float fix = (360.f - angle) * (float)(M_PI / 180.f);
	Quaternion quat;
	quat.FromAxisAngle(0.0,0.0,1.0,fix);
	SetQuaternion(quat.X,quat.Y,quat.Z,quat.W);
	local_class->Angle = angle;
}

float CNPCCore::GetAngle()
{
	return local_class->Angle;
}

void CNPCCore::SetVelocity(float x,float y,float z)
{
	local_class->FootSync.Velocity[0] = x;
	local_class->FootSync.Velocity[1] = y;
	local_class->FootSync.Velocity[2] = z;
	local_class->InCarSync.Velocity[0] = x;
	local_class->InCarSync.Velocity[1] = y;
	local_class->InCarSync.Velocity[2] = z;
}

void CNPCCore::GetVelocity(float* x,float* y,float* z)
{
	switch(local_class->State)
	{
	case PLAYER_STATE_ONFOOT:
		{
			*x = local_class->FootSync.Velocity[0];
			*y = local_class->FootSync.Velocity[1];
			*z = local_class->FootSync.Velocity[2];
		}
	case PLAYER_STATE_DRIVER:
		{
			*x = local_class->InCarSync.Velocity[0];
			*y = local_class->InCarSync.Velocity[1];
			*z = local_class->InCarSync.Velocity[2];
		}
	}
}

void CNPCCore::SetKeys(unsigned short ud,unsigned short lr,unsigned short other)
{
	local_class->FootSync.KeysUD = ud;
	local_class->FootSync.KeysLR = lr;
	local_class->FootSync.KeysOther = other;
	local_class->InCarSync.KeysUD = ud;
	local_class->InCarSync.KeysLR = lr;
	local_class->InCarSync.KeysOther = other;
	local_class->PassangerSync.KeysUD = ud;
	local_class->PassangerSync.KeysLR = lr;
	local_class->PassangerSync.KeysOther = other;
}

void CNPCCore::GetKeys(unsigned short* ud,unsigned short* lr,unsigned short* other)
{
	switch(local_class->State)
	{
	case PLAYER_STATE_ONFOOT:
		{
			*ud = local_class->FootSync.KeysUD;
			*lr = local_class->FootSync.KeysLR;
			*other = local_class->FootSync.KeysOther;
		}
	case PLAYER_STATE_DRIVER:
		{
			*ud = local_class->InCarSync.KeysUD;
			*lr = local_class->InCarSync.KeysLR;
			*other = local_class->InCarSync.KeysOther;
		}
	case PLAYER_STATE_PASSENGER:
		{
			*ud = local_class->PassangerSync.KeysUD;
			*lr = local_class->PassangerSync.KeysLR;
			*other = local_class->PassangerSync.KeysOther;
		}
	}
}

void CNPCCore::SetSpecialAction(unsigned char actionid)
{
	local_class->FootSync.SpecialAction = actionid;
}

unsigned char CNPCCore::GetSpecialAction()
{
	return local_class->FootSync.SpecialAction;
}

void CNPCCore::SetAnimationIndex(unsigned long animationid)
{
	local_class->FootSync.AnimationIndex = animationid;
}

unsigned long CNPCCore::GetAnimationIndex()
{
	return local_class->FootSync.AnimationIndex;
}

void CNPCCore::SetWeapon(unsigned char weaponid)
{
	local_class->FootSync.PlayerWeapon = weaponid;
	local_class->InCarSync.PlayerWeapon = weaponid;
	local_class->PassangerSync.Weapon = weaponid;
}

unsigned char CNPCCore::GetWeapon()
{
	switch(local_class->State)
	{
	case PLAYER_STATE_ONFOOT:
		{
			return local_class->FootSync.PlayerWeapon;
		}
	case PLAYER_STATE_DRIVER:
		{
			return local_class->InCarSync.PlayerWeapon;
		}
	case PLAYER_STATE_PASSENGER:
		{
			return local_class->PassangerSync.Weapon;
		}
	default: return 0;
	}
}

void CNPCCore::SetWeaponSkillLevel(unsigned char weapontype,unsigned short level)
{
	float x,y,z,angle,health,armour;
	local_class->WeaponSkill[weapontype] = level;
	// respawn
	GetPosition(&x,&y,&z);
	angle = GetAngle();
	health = GetHealth();
	armour = GetArmour();
	pHooks->ClientSpawn(local_class);
	SetPosition(x,y,z);
	SetAngle(angle);
	SetHealth(health);
	SetArmour(armour);
}

unsigned short CNPCCore::GetWeaponSkillLevel(unsigned char weapontype)
{
	return local_class->WeaponSkill[weapontype];
}

void CNPCCore::SetHealth(float health)
{
	local_class->FootSync.PlayerHealth = static_cast<unsigned char>(health);
	local_class->InCarSync.PlayerHealth = static_cast<unsigned char>(health);
	local_class->PassangerSync.Health = static_cast<unsigned char>(health);
	local_class->Health = health;
}

float CNPCCore::GetHealth()
{
	return local_class->Health;
}

void CNPCCore::SetArmour(float armour)
{
	local_class->FootSync.PlayerArmour = static_cast<unsigned char>(armour);
	local_class->InCarSync.PlayerArmour = static_cast<unsigned char>(armour);
	local_class->PassangerSync.Armour = static_cast<unsigned char>(armour);
	local_class->Armour = armour;
}

float CNPCCore::GetArmour()
{
	return local_class->Armour;
}

void CNPCCore::SetSurfing(float x,float y,float z)
{
	local_class->FootSync.Surfing[0] = x;
	local_class->FootSync.Surfing[1] = y;
	local_class->FootSync.Surfing[2] = z;
}

void CNPCCore::GetSurfing(float* x,float* y,float* z)
{
	*x = local_class->FootSync.Surfing[0];
	*y = local_class->FootSync.Surfing[1];
	*z = local_class->FootSync.Surfing[2];
}

void CNPCCore::SetSurfingVehicle(unsigned short vehicleid)
{
	local_class->FootSync.SurfingInfo = vehicleid;
}

unsigned short CNPCCore::GetSurfingVehicle()
{
	return local_class->FootSync.SurfingInfo;
}

void CNPCCore::SetCameraPos(float x,float y,float z)
{
	local_class->AimSync.Position[0] = x;
	local_class->AimSync.Position[1] = y;
	local_class->AimSync.Position[2] = z;

	local_class->AimSyncSendState = 1;
}

void CNPCCore::GetCameraPos(float* x,float* y,float* z)
{
	*x = local_class->AimSync.Position[0];
	*y = local_class->AimSync.Position[1];
	*z = local_class->AimSync.Position[2];
}

void CNPCCore::SetCameraFrontVector(float x,float y,float z)
{
	local_class->AimSync.FrontVector[0] = x;
	local_class->AimSync.FrontVector[1] = y;
	local_class->AimSync.FrontVector[2] = z;

	local_class->AimSyncSendState = 1;
}

void CNPCCore::GetCameraFrontVector(float* x,float* y,float* z)
{
	*x = local_class->AimSync.FrontVector[0];
	*y = local_class->AimSync.FrontVector[1];
	*z = local_class->AimSync.FrontVector[2];
}

void CNPCCore::SetCameraZAim(float angle)
{
	local_class->AimSync.ZAim = angle;

	local_class->AimSyncSendState = 1;
}

float CNPCCore::GetCameraZAim()
{
	return local_class->AimSync.ZAim;
}

void CNPCCore::SetCameraMode(unsigned char mode)
{
	local_class->AimSync.CameraMode = mode;

	local_class->AimSyncSendState = 1;
}

unsigned char CNPCCore::GetCameraMode()
{
	return local_class->AimSync.CameraMode;
}

void CNPCCore::SetWeaponState(unsigned char state)
{
	local_class->AimSync.WeaponState = state;

	local_class->AimSyncSendState = 1;
}

unsigned char CNPCCore::GetWeaponState()
{
	return local_class->AimSync.WeaponState;
}

void CNPCCore::SetVehicleSiren(bool state)
{
	if(state) local_class->InCarSync.SirenState = 1;
	else local_class->InCarSync.SirenState = 0;
}

bool CNPCCore::GetVehicleSiren()
{
	return (local_class->InCarSync.SirenState==0?false:true);
}

void CNPCCore::SetVehicleHealth(float health)
{
	local_class->InCarSync.Health = health;
	unsigned short vehicleid = local_class->VehicleId;
	if(pSaMp->vehicles->vehicle[vehicleid]) pSaMp->vehicles->vehicle[vehicleid]->Health = health;
}

float CNPCCore::GetVehicleHealth()
{
	return local_class->InCarSync.Health;
}

void CNPCCore::SetVehicleUnique(float unique)
{
	local_class->InCarSync.TrainSpeed = unique;
}

float CNPCCore::GetVehicleUnique()
{
	return local_class->InCarSync.TrainSpeed;
}

void CNPCCore::SetVehicleTrailer(unsigned short trailer)
{
	local_class->InCarSync.TrailerId_or_ThrustAngle = trailer;
}

unsigned short CNPCCore::GetVehicleTrailer()
{
	return local_class->InCarSync.TrailerId_or_ThrustAngle;
}

void CNPCCore::SetPassangerDriveBy(bool state)
{
	if(state) local_class->PassangerSync.DriveBy = 1;
	else local_class->PassangerSync.DriveBy = 0;
}

bool CNPCCore::GetPassangerDriveBy()
{
	if(local_class->PassangerSync.DriveBy == 1) return true;
	return false;
}

void CNPCCore::Update(double update_time)
{
	// check needed send package
	if((update_time - last_update) < 0.1) return; // not need update
	// need update
	last_update = update_time;
	// update
	switch(GetState())
	{
	case PLAYER_STATE_ONFOOT:
		{
			// surfing sync
			unsigned short id = GetSurfingVehicle();
			if(id != INVALID_VEHICLE_ID)
			{
				if(pSaMp->vehicles->vehicle[id])
				{
					CVehicle * pl_veh = pSaMp->vehicles->vehicle[id];
					SetPosition(pl_veh->Position[0] + local_class->FootSync.Surfing[0],pl_veh->Position[1] + local_class->FootSync.Surfing[1],pl_veh->Position[2] + local_class->FootSync.Surfing[2]);
				}
			}
			local_class->SyncType = UPDATE_TYPE_ONFOOT;
			break;
		}
	case PLAYER_STATE_DRIVER:
		{
			local_class->SyncType = UPDATE_TYPE_INCAR;
			break;
		}
	case PLAYER_STATE_PASSENGER:
		{
			local_class->SyncType = UPDATE_TYPE_PASSENGER;
			break;
		}
	case PLAYER_STATE_WASTED:
		{
			local_class->SyncType = UPDATE_TYPE_ONFOOT;
			break;
		}
	}
}