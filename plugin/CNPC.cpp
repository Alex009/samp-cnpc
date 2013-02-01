/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 */

// global defines
#define _USE_MATH_DEFINES

// includes
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
// plugin
#include "SAMP/CSAMP.h"
#include "hooks.h"
#include "CCallbacks.h"
#include "CNPC.h"
#include "defines.h"
#include "os.h"
#include "utils/utils.h"
#include "utils/quaternion.h"
#include "utils/3DVector.h"
#include "utils/time.h"

// externs
extern	CSAMP*		pSaMp;
extern	CHooks*		pHooks;
extern	CCallbacks*	pCallbacks;
extern	char		VisualDeath;
extern	int			WeaponReload[MAX_PLAYERS];
extern	float		WeaponsDamage[MAX_WEAPONS + 2];
extern	int			WeaponsReloadTime[MAX_WEAPONS + 1];

float GetZCoord(float x,float y);

CNPC::CNPC(int id,char* name)
{
	local_id = id;

	pHooks->ClientConnect(pSaMp->players,id,name);

	local_class = pSaMp->players->player[local_id];

	SetState(NPC_STATE_REQUEST_CLASS);
	SetPosition(0.0,0.0,0.0);
	SetQuaternion(0.0,0.0,0.0,0.0);
	SetAngle(0.0);
	SetVelocity(0.0,0.0,0.0);
	SetKeys(0,0,0);
	SetState(NPC_STATE_REQUEST_CLASS);
	SetSpecialAction(0);
	SetWeapon(0);
	SetSkin(0);
	SetSurfingVehicle(INVALID_VEHICLE_ID);
	SetVehicleSiren(0);
	SetVehicleUnique(0);
	SetVehicleTrailer(INVALID_VEHICLE_ID);
	SetHealth(100.0);
	SetArmour(0.0);

	//moving_speed = 0.0;
	moving_distance = 0.0;
	moving_vector[0] = 0.0;
	moving_vector[1] = 0.0;
	moving_vector[2] = 0.0;
	moving_state = false;
	moving_zmap = false;

	local_impregnable = false;

	playback_state = false;
	playback_type = 0;
	playback_file = NULL;
	playback_StartTime = 0;
	playback_CurrTime = 0;

	knock_vector[0] = 0.0;
	knock_vector[1] = 0.0;
	knock_vector[2] = 0.0;

	local_class->AimSync.Unknown1 = 0x55;
}

CNPC::~CNPC()
{
	pHooks->ClientDisconnect(pSaMp->players,local_id,0);
}

void CNPC::Spawn()
{
	if(local_state == NPC_STATE_PLAYBACK) StopRecordingPlayback(NPC_RECORD_END_REASON_STOP);
	SetHealth(100.0);
	pHooks->ClientSpawn(local_class);
	// set nulled data
	local_class->FootSync.AnimationIndex = 0;
	local_class->FootSync.KeysLR = 0;
	local_class->FootSync.KeysOther = 0;
	local_class->FootSync.KeysUD = 0;
	local_class->FootSync.SpecialAction = 0;
	local_class->FootSync.SurfingInfo = 0xFFFF;
	SetState(NPC_STATE_ONFOOT);
	pCallbacks->Spawn(local_id);
}

void CNPC::Kill(int killerid,int reason)
{
	if(local_state == NPC_STATE_PLAYBACK) StopRecordingPlayback(NPC_RECORD_END_REASON_DEATH);
	Stop();
	SetHealth(0.0);
	if(VisualDeath == 1) pHooks->ClientDeath(local_class,reason,killerid);
	SetState(NPC_STATE_DEATH);
	pCallbacks->Death(local_id,killerid,reason);
}

void CNPC::SetState(int state)
{
	local_state = state;
	switch(local_state)
	{
	case NPC_STATE_REQUEST_CLASS: 
		{
			local_class->State = PLAYER_STATE_SPECTATING;
			break;
		}
	case NPC_STATE_ONFOOT: 
		{
			local_class->State = PLAYER_STATE_ONFOOT;
			break;
		}
	case NPC_STATE_DRIVER: 
		{
			local_class->State = PLAYER_STATE_DRIVER;
			break;
		}
	case NPC_STATE_PASSENGER: 
		{
			local_class->State = PLAYER_STATE_PASSENGER;
			break;
		}
	case NPC_STATE_DEATH: 
		{
			local_class->State = PLAYER_STATE_WASTED;
			break;
		}
	case NPC_STATE_PLAYBACK: 
		{
			switch(local_playback_type)
			{
			case PLAYER_RECORDING_TYPE_DRIVER:
				{
					local_class->State = PLAYER_STATE_DRIVER;
					break;
				}
			case PLAYER_RECORDING_TYPE_ONFOOT:
				{
					local_class->State = PLAYER_STATE_ONFOOT;
					break;
				}
			}
			break;
		}
	}
}

int CNPC::GetState()
{
	return local_state;
}

void CNPC::SetSkin(int skin)
{
	float x,y,z,angle,health,armour;
	local_class->Skin = static_cast<unsigned long>(skin);
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

int CNPC::GetSkin()
{
	return static_cast<int>(local_class->Skin);
}

void CNPC::SetInterior(int interior)
{
	local_class->Interior = static_cast<unsigned long>(interior);
}

int CNPC::GetInterior()
{
	return static_cast<int>(local_class->Interior);
}

void CNPC::SetImpregnable(bool state)
{
	local_impregnable = state;
}

bool CNPC::IsImpregnable()
{
	return local_impregnable;
}

void CNPC::PutInVehicle(int vehicleid,int seat)
{
	if(vehicleid == 0)
	{
		local_class->FootSync.AnimationIndex = 0;
		local_class->FootSync.KeysLR = 0;
		local_class->FootSync.KeysOther = 0;
		local_class->FootSync.KeysUD = 0;
		local_class->FootSync.SpecialAction = 0;
		local_class->FootSync.SurfingInfo = 0xFFFF;
		SetState(NPC_STATE_ONFOOT);
		return;
	}
	if(pSaMp->vehicles->vehicle[vehicleid])
	{
		if(seat == 0)
		{
			pSaMp->vehicles->vehicle[vehicleid]->Driver = static_cast<unsigned short>(local_id);
			
			local_class->VehicleId = static_cast<unsigned short>(vehicleid);
			local_class->VehicleSeat = static_cast<unsigned char>(seat);

			local_class->InCarSync.VehicleId = static_cast<unsigned short>(vehicleid);

			SetState(NPC_STATE_DRIVER);

			SetPosition(pSaMp->vehicles->vehicle[vehicleid]->Position[0],
				pSaMp->vehicles->vehicle[vehicleid]->Position[1],
				pSaMp->vehicles->vehicle[vehicleid]->Position[2]);
			SetKeys(0,0,0);
			SetHealth(local_class->Health);
			SetArmour(local_class->Armour);
			SetWeapon(0);

			local_class->InCarSync.SirenState = 0;
			local_class->InCarSync.TrailerId_or_ThrustAngle = 0xFFFF;
			local_class->InCarSync.TrainSpeed = 0.0;

			local_class->InCarSync.Health = pSaMp->vehicles->vehicle[vehicleid]->Health;
		}
		else
		{
			local_class->VehicleId = static_cast<unsigned short>(vehicleid);
			local_class->VehicleSeat = static_cast<unsigned char>(seat);

			local_class->PassangerSync.VehicleId = static_cast<unsigned short>(vehicleid);
			local_class->PassangerSync.Seat = static_cast<unsigned char>(seat);

			SetState(NPC_STATE_PASSENGER);

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

void CNPC::SetPosition(float x,float y,float z)
{
	local_class->Position[0] = x;
	local_class->Position[1] = y;
	local_class->Position[2] = z;

	switch(local_class->State)
	{
	case PLAYER_STATE_ONFOOT:
		{
			local_class->FootSync.Position[0] = x;
			local_class->FootSync.Position[1] = y;
			local_class->FootSync.Position[2] = z;
		}
	case PLAYER_STATE_DRIVER:
		{
			local_class->InCarSync.Position[0] = x;
			local_class->InCarSync.Position[1] = y;
			local_class->InCarSync.Position[2] = z;
			// vehicle data
			int vehicleid = local_class->VehicleId;
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
	case PLAYER_STATE_PASSENGER:
		{
			local_class->PassangerSync.Position[0] = x;
			local_class->PassangerSync.Position[1] = y;
			local_class->PassangerSync.Position[2] = z;
		}
	}
}

void CNPC::GetPosition(float* x,float* y,float* z)
{
	*x = local_class->Position[0];
	*y = local_class->Position[1];
	*z = local_class->Position[2];
}

void CNPC::SetQuaternion(float x,float y,float z,float s)
{
	switch(local_class->State)
	{
	case PLAYER_STATE_ONFOOT:
		{
			local_class->FootSync.Quaternion[0] = s;
			local_class->FootSync.Quaternion[1] = x;
			local_class->FootSync.Quaternion[2] = y;
			local_class->FootSync.Quaternion[3] = z;
		}
	case PLAYER_STATE_DRIVER:
		{
			local_class->InCarSync.Quaternion[0] = s;
			local_class->InCarSync.Quaternion[1] = x;
			local_class->InCarSync.Quaternion[2] = y;
			local_class->InCarSync.Quaternion[3] = z;
		}
	}
}

void CNPC::GetQuaternion(float* x,float* y,float* z,float* s)
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

void CNPC::SetAngle(float angle)
{
	float fix = (360.f - angle) * (float)(M_PI / 180.f);
	Quaternion quat;
	quat.FromAxisAngle(0.0,0.0,1.0,fix);
	SetQuaternion(quat.X,quat.Y,quat.Z,quat.W);
	local_class->Angle = angle;
}

float CNPC::GetAngle()
{
	return local_class->Angle;
}

void CNPC::SetVelocity(float x,float y,float z)
{
	switch(local_class->State)
	{
	case PLAYER_STATE_ONFOOT:
		{
			local_class->FootSync.Velocity[0] = x;
			local_class->FootSync.Velocity[1] = y;
			local_class->FootSync.Velocity[2] = z;
		}
	case PLAYER_STATE_DRIVER:
		{
			local_class->InCarSync.Velocity[0] = x;
			local_class->InCarSync.Velocity[1] = y;
			local_class->InCarSync.Velocity[2] = z;
		}
	}
}

void CNPC::GetVelocity(float* x,float* y,float* z)
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

void CNPC::SetKeys(int ud,int lr,int other)
{
	switch(local_class->State)
	{
	case PLAYER_STATE_ONFOOT:
		{
			local_class->FootSync.KeysUD = static_cast<unsigned short>(ud);
			local_class->FootSync.KeysLR = static_cast<unsigned short>(lr);
			local_class->FootSync.KeysOther = static_cast<unsigned short>(other);
		}
	case PLAYER_STATE_DRIVER:
		{
			local_class->InCarSync.KeysUD = static_cast<unsigned short>(ud);
			local_class->InCarSync.KeysLR = static_cast<unsigned short>(lr);
			local_class->InCarSync.KeysOther = static_cast<unsigned short>(other);
		}
	case PLAYER_STATE_PASSENGER:
		{
			local_class->PassangerSync.KeysUD = static_cast<unsigned short>(ud);
			local_class->PassangerSync.KeysLR = static_cast<unsigned short>(lr);
			local_class->PassangerSync.KeysOther = static_cast<unsigned short>(other);
		}
	}
}

void CNPC::GetKeys(int* ud,int* lr,int* other)
{
	switch(local_class->State)
	{
	case PLAYER_STATE_ONFOOT:
		{
			*ud = static_cast<int>(local_class->FootSync.KeysUD);
			*lr = static_cast<int>(local_class->FootSync.KeysLR);
			*other = static_cast<int>(local_class->FootSync.KeysOther);
		}
	case PLAYER_STATE_DRIVER:
		{
			*ud = static_cast<int>(local_class->InCarSync.KeysUD);
			*lr = static_cast<int>(local_class->InCarSync.KeysLR);
			*other = static_cast<int>(local_class->InCarSync.KeysOther);
		}
	case PLAYER_STATE_PASSENGER:
		{
			*ud = static_cast<int>(local_class->PassangerSync.KeysUD);
			*lr = static_cast<int>(local_class->PassangerSync.KeysLR);
			*other = static_cast<int>(local_class->PassangerSync.KeysOther);
		}
	}
}

void CNPC::SetSpecialAction(int actionid)
{
	local_class->FootSync.SpecialAction = static_cast<unsigned char>(actionid);
}

int CNPC::GetSpecialAction()
{
	return static_cast<int>(local_class->FootSync.SpecialAction);
}

void CNPC::SetAnimationIndex(unsigned long animationid)
{
	local_class->FootSync.AnimationIndex = animationid;
}

unsigned long CNPC::GetAnimationIndex()
{
	return local_class->FootSync.AnimationIndex;
}

void CNPC::SetWeapon(int weaponid)
{
	switch(local_class->State)
	{
	case PLAYER_STATE_ONFOOT:
		{
			local_class->FootSync.PlayerWeapon = static_cast<unsigned char>(weaponid);
		}
	case PLAYER_STATE_DRIVER:
		{
			local_class->InCarSync.PlayerWeapon = static_cast<unsigned char>(weaponid);
		}
	case PLAYER_STATE_PASSENGER:
		{
			local_class->PassangerSync.Weapon = static_cast<unsigned char>(weaponid);
		}
	}
}

int CNPC::GetWeapon()
{
	switch(local_class->State)
	{
	case PLAYER_STATE_ONFOOT:
		{
			return static_cast<int>(local_class->FootSync.PlayerWeapon);
		}
	case PLAYER_STATE_DRIVER:
		{
			return static_cast<int>(local_class->InCarSync.PlayerWeapon);
		}
	case PLAYER_STATE_PASSENGER:
		{
			return static_cast<int>(local_class->PassangerSync.Weapon);
		}
	default: return 0;
	}
}

void CNPC::SetWeaponSkillLevel(int weapontype,int level)
{
	float x,y,z,angle,health,armour;
	local_class->WeaponSkill[weapontype] = static_cast<unsigned short>(level);
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

int CNPC::GetWeaponSkillLevel(int weapontype)
{
	return static_cast<int>(local_class->WeaponSkill[weapontype]);
}

void CNPC::SetHealth(float health)
{
	switch(local_class->State)
	{
	case PLAYER_STATE_ONFOOT:
		{
			local_class->FootSync.PlayerHealth = static_cast<unsigned char>(health);
		}
	case PLAYER_STATE_DRIVER:
		{
			local_class->InCarSync.PlayerHealth = static_cast<unsigned char>(health);
		}
	case PLAYER_STATE_PASSENGER:
		{
			local_class->PassangerSync.Health = static_cast<unsigned char>(health);
		}
	}
	local_class->Health = health;
}

float CNPC::GetHealth()
{
	return local_class->Health;
}

void CNPC::SetArmour(float armour)
{
	switch(local_class->State)
	{
	case PLAYER_STATE_ONFOOT:
		{
			local_class->FootSync.PlayerArmour = static_cast<unsigned char>(armour);
		}
	case PLAYER_STATE_DRIVER:
		{
			local_class->InCarSync.PlayerArmour = static_cast<unsigned char>(armour);
		}
	case PLAYER_STATE_PASSENGER:
		{
			local_class->PassangerSync.Armour = static_cast<unsigned char>(armour);
		}
	}
	local_class->Armour = armour;
}

float CNPC::GetArmour()
{
	return local_class->Armour;
}

void CNPC::SetSurfing(float x,float y,float z)
{
	local_class->FootSync.Surfing[0] = x;
	local_class->FootSync.Surfing[1] = y;
	local_class->FootSync.Surfing[2] = z;
}

void CNPC::GetSurfing(float* x,float* y,float* z)
{
	*x = local_class->FootSync.Surfing[0];
	*y = local_class->FootSync.Surfing[1];
	*z = local_class->FootSync.Surfing[2];
}

void CNPC::SetSurfingVehicle(int vehicleid)
{
	local_class->FootSync.SurfingInfo = static_cast<unsigned short>(vehicleid);
}

int CNPC::GetSurfingVehicle()
{
	return static_cast<int>(local_class->FootSync.SurfingInfo);
}

void CNPC::SetCameraPos(float x,float y,float z)
{
	local_class->AimSync.Position[0] = x;
	local_class->AimSync.Position[1] = y;
	local_class->AimSync.Position[2] = z;

	local_class->AimSyncSendState = 1;
}

void CNPC::GetCameraPos(float* x,float* y,float* z)
{
	*x = local_class->AimSync.Position[0];
	*y = local_class->AimSync.Position[1];
	*z = local_class->AimSync.Position[2];
}

void CNPC::SetCameraFrontVector(float x,float y,float z)
{
	local_class->AimSync.FrontVector[0] = x;
	local_class->AimSync.FrontVector[1] = y;
	local_class->AimSync.FrontVector[2] = z;

	local_class->AimSyncSendState = 1;
}

void CNPC::GetCameraFrontVector(float* x,float* y,float* z)
{
	*x = local_class->AimSync.FrontVector[0];
	*y = local_class->AimSync.FrontVector[1];
	*z = local_class->AimSync.FrontVector[2];
}

void CNPC::SetCameraZAim(float angle)
{
	local_class->AimSync.ZAim = angle;

	local_class->AimSyncSendState = 1;
}

float CNPC::GetCameraZAim()
{
	return local_class->AimSync.ZAim;
}

void CNPC::SetCameraMode(int mode)
{
	local_class->AimSync.CameraMode = static_cast<unsigned char>(mode);

	local_class->AimSyncSendState = 1;
}

int CNPC::GetCameraMode()
{
	return static_cast<int>(local_class->AimSync.CameraMode);
}

void CNPC::SetWeaponState(int wstate)
{
	local_class->AimSync.WeaponState = static_cast<unsigned char>(wstate);

	local_class->AimSyncSendState = 1;
}

int CNPC::GetWeaponState()
{
	return static_cast<int>(local_class->AimSync.WeaponState);
}

void CNPC::SetVehicleSiren(int state)
{
	local_class->InCarSync.SirenState = static_cast<unsigned char>(state);
}

int CNPC::GetVehicleSiren()
{
	return static_cast<int>(local_class->InCarSync.SirenState);
}

void CNPC::SetVehicleHealth(float health)
{
	local_class->InCarSync.Health = health;
	int vehicleid = static_cast<int>(local_class->VehicleId);
	if(pSaMp->vehicles->vehicle[vehicleid]) pSaMp->vehicles->vehicle[vehicleid]->Health = health;
}

float CNPC::GetVehicleHealth()
{
	return local_class->InCarSync.Health;
}

void CNPC::SetVehicleUnique(float unique)
{
	local_class->InCarSync.TrainSpeed = unique;
}

float CNPC::GetVehicleUnique()
{
	return local_class->InCarSync.TrainSpeed;
}

void CNPC::SetVehicleTrailer(int trailer)
{
	local_class->InCarSync.TrailerId_or_ThrustAngle = static_cast<unsigned char>(trailer);
}

int CNPC::GetVehicleTrailer()
{
	int tr = static_cast<int>(local_class->InCarSync.TrailerId_or_ThrustAngle);
	return tr;
}

void CNPC::SetPassangerDriveBy(int state)
{
	if(state == 1) local_class->PassangerSync.DriveBy = 1;
	else local_class->PassangerSync.DriveBy = 0;
}

int CNPC::GetPassangerDriveBy()
{
	if(local_class->PassangerSync.DriveBy == 1) return 1;
	return 0;
}

/*
 *		MOVING
 */

void CNPC::GoTo(float x,float y,float z,float velocity,bool use_z_map,bool xyz_rotate)
{
	float pX,pY,pZ,vX,vY,vZ;
	// coords
	GetPosition(&pX,&pY,&pZ);
	// move to
	vX = x - pX;
	vY = y - pY;
	vZ = z - pZ;
	// velocity
	moving_velocity = velocity;
	// distance
	moving_distance = sqrt((vX*vX) + (vY*vY) + (vZ*vZ));
	// fix NULL distance
	if(abs(moving_distance) < 0.00001f) moving_distance = 0.000001f;
	// fix vector by velocity len
	vX = (vX / moving_distance) * moving_velocity;
	vY = (vY / moving_distance) * moving_velocity;
	vZ = (vZ / moving_distance) * moving_velocity;
	// moving vector
	moving_vector[0] = vX * VELOCITY_DISTANCE;
	moving_vector[1] = vY * VELOCITY_DISTANCE;
	moving_vector[2] = vZ * VELOCITY_DISTANCE;
	// new velocity len
	moving_velocity = sqrt((moving_vector[0]*moving_vector[0]) + (moving_vector[1]*moving_vector[1]) + (moving_vector[2]*moving_vector[2]));
	// get quaternion of rotation
	Quaternion quat;
	if(xyz_rotate) quat.FromVector(vX,vY,vZ);
	else quat.FromVector(vX,vY,0.0);
	// set data
	SetQuaternion(quat.X,quat.Y,quat.Z,quat.W);
	SetVelocity(vX,vY,vZ);
	moving_state = true;
	moving_zmap = use_z_map;
}
/*
void CNPC::DriveTo(float x,float y,float z,float step,bool use_z_map)
{
	// get pos
	float pos_x,pos_y,pos_z;
	float euler[3];
	Quaternion q1,q2;
	GetPosition(&pos_x,&pos_y,&pos_z);
	// get len
	moving_vector[0] = x - pos_x;
	moving_vector[1] = y - pos_y;
	moving_vector[2] = z - pos_z;
	moving_distance = sqrt((moving_vector[0] * moving_vector[0]) + (moving_vector[1] * moving_vector[1]) + (moving_vector[2] * moving_vector[2]));
	// check distance
	if(abs(moving_distance) < 0.00001) moving_distance = 0.000001;
	// get vector
	moving_vector[0] /= moving_distance;
	moving_vector[1] /= moving_distance;
	moving_vector[2] /= moving_distance;
	// set quaternion
	C3DVector vec(moving_vector[0],moving_vector[1],moving_vector[2]);
	vec.ToEuler(euler);
	euler[2] -= (90.f * (float)(M_PI / 180.f)); // gta fix
	for(int i = 0;i < 3;i++) euler[i] = (360.f * (float)(M_PI / 180.f)) - euler[i]; // gta fix
	q1.FromAxisAngle(0.0,0.0,1.0,euler[2]);
	q2.FromAxisAngle(1.0,0.0,0.0,euler[0]);
	q1.Multiply(q2);
	SetQuaternion(q1.X,q1.Y,q1.Z,q1.W);
	// get real velocity
	moving_speed = step / 500;
	// get current velocity vector
	pos_x = moving_vector[0] * moving_speed;
	pos_y = moving_vector[1] * moving_speed;
	pos_z = moving_vector[2] * moving_speed;
	// set velocity
	SetVelocity(pos_x,pos_y,pos_z);
	// get real speed
	moving_speed *= 4.0;
	// get current speed vector
	moving_vector[0] *= moving_speed;
	moving_vector[1] *= moving_speed;
	moving_vector[2] *= moving_speed;
	// state
	SetState(NPC_STATE_DRIVER);
	// set info
	moving_state = true;
	moving_zmap = use_z_map;
}
*/
void CNPC::Stop()
{
	moving_state = false;
	//moving_speed = 0.0;
	SetVelocity(0.0,0.0,0.0);
	SetKeys(0,0,0);
}

/*
 *		AIM
 */
void CNPC::SetAimByWeapon(int weaponid,float tx,float ty,float tz)
{
	float x,y,z;
	float fv[3];
	float len;
	GetPosition(&x,&y,&z);
	z += 0.7f;
	fv[0] = tx - x;
	fv[1] = ty - y;
	fv[2] = tz - z;
	// z angle
	float a = (atan2f(fv[1],fv[0]) * (180.0f / 3.14159265358979323846f)) + 270.0f;
	if(a > 360.0f) a -= 360.0f;
	// create vector
	len = sqrt((fv[0] * fv[0]) + (fv[1] * fv[1]) + (fv[2] * fv[2]));
	fv[0] /= len;
	fv[1] /= len;
	fv[2] /= len;
	// z camera angle
	float z_aim = -acos((fv[0] * fv[0]) + (fv[1] * fv[1]));
	SetCameraZAim(z_aim);
	// aim fix
	switch(weaponid) 
	{
		case WEAPON_SNIPER:
		case WEAPON_ROCKETLAUNCHER:
		case WEAPON_HEATSEEKER: 
			break;
		case WEAPON_RIFLE: 
			AdjustVector(fv, -0.016204f, -0.009899f, -0.047177f);
			break;
		case WEAPON_AK47:
		case WEAPON_M4:
			AdjustVector(fv, -0.026461f, -0.013070f, -0.069079f);
			break;
		default:
			AdjustVector(fv, -0.043949f, -0.015922f, -0.103412f);
			break;
	}
	// set data
	SetCameraPos(x,y,z);
	SetCameraFrontVector(fv[0],fv[1],fv[2]);
	// z aim
	SetAngle(a);
}

/*
 *		PLAYBACKS
 */

int CNPC::StartRecordingPlayback(char* name)
{
	if(!playback_state)
	{
		char* buf = new char[128];
		// path
		sprintf(buf,"scriptfiles/%s.rec",name);
		// open
		playback_file = fopen(buf,"rb");
		if(playback_file)
		{
			playback_StartTime = GetTickCount();
			// read main info
			unsigned long tmp;

			fread(&tmp,1,4,playback_file);
			fread(&playback_type,1,4,playback_file);
			// read one block
			switch(playback_type)
			{
			case PLAYER_RECORDING_TYPE_DRIVER:
				{
					fread(&playback_CurrTime,1,4,playback_file);
					fread(&playback_CurrInCarData,1,sizeof(Packet_InCar),playback_file);
					break;
				}
			case PLAYER_RECORDING_TYPE_ONFOOT:
				{
					fread(&playback_CurrTime,1,4,playback_file);
					fread(&playback_CurrOnFootData,1,sizeof(Packet_Foot),playback_file);
					break;
				}
			}
			// state
			SetState(NPC_STATE_PLAYBACK);
			playback_state = true;
			return 1;
		}
	}
	return 0;
}

void CNPC::ReadNextPlaybackBlock()
{
	if(playback_file)
	{
		if(feof(playback_file))
		{
			StopRecordingPlayback(NPC_RECORD_END_REASON_END);	
		}
		else
		{
			switch(playback_type)
			{
			case PLAYER_RECORDING_TYPE_DRIVER:
				{
					fread(&playback_CurrTime,1,4,playback_file);
					fread(&playback_CurrInCarData,1,sizeof(Packet_InCar),playback_file);
					break;
				}
			case PLAYER_RECORDING_TYPE_ONFOOT:
				{
					fread(&playback_CurrTime,1,4,playback_file);
					fread(&playback_CurrOnFootData,1,sizeof(Packet_Foot),playback_file);
					break;
				}
			}
		}
	}
}

void CNPC::PauseRecordingPlayback()
{
	if(playback_state)
	{
		switch(playback_type)
		{
		case PLAYER_RECORDING_TYPE_DRIVER:
			{
				SetVelocity(0.0,0.0,0.0);
				// state
				SetState(NPC_STATE_DRIVER);
				break;
			}
		case PLAYER_RECORDING_TYPE_ONFOOT:
			{
				SetVelocity(0.0,0.0,0.0);
				// state
				SetState(NPC_STATE_ONFOOT);
				break;
			}
		}
		playback_StartTime -= GetTickCount();
	}
}

void CNPC::ContinueRecordingPlayback()
{
	if(playback_state)
	{
		// state
		SetState(NPC_STATE_PLAYBACK);
		playback_StartTime += GetTickCount();
	}
}

void CNPC::StopRecordingPlayback(int reason)
{
	if(playback_state)
	{
		switch(playback_type)
		{
		case PLAYER_RECORDING_TYPE_DRIVER:
			{
				SetKeys(0,0,0);
				SetVelocity(0.0,0.0,0.0);
				// state
				SetState(NPC_STATE_DRIVER);
				break;
			}
		case PLAYER_RECORDING_TYPE_ONFOOT:
			{
				SetKeys(0,0,0);
				SetVelocity(0.0,0.0,0.0);
				// state
				SetState(NPC_STATE_ONFOOT);
				break;
			}
		}
		fclose(playback_file);

		playback_StartTime = 0;
		playback_file = 0;
		playback_state = false;

		pCallbacks->PlaybackEnd(local_id,reason);
	}
}

void CNPC::ApplySync(unsigned long type,unsigned char state)
{
	double time = microtime();

	if((time - LastUpdate) > 0.1) 
	{
		local_class->SyncType = type;
		LastUpdate = time;
	}
	local_class->State = state;
}

/*
 *		THREAD
 */

void CNPC::Sync()
{
	switch(GetState())
	{
	case NPC_STATE_ONFOOT:
		{
			int id = GetSurfingVehicle();
			if(id != INVALID_VEHICLE_ID)
			{
				if(pSaMp->vehicles->vehicle[id])
				{
					CVehicle * pl_veh = pSaMp->vehicles->vehicle[id];
					SetPosition(pl_veh->Position[0] + local_class->FootSync.Surfing[0],pl_veh->Position[1] + local_class->FootSync.Surfing[1],pl_veh->Position[2] + local_class->FootSync.Surfing[2]);
				}
			}

			if(moving_state)
			{
				// get current pos
				float x,y,z;
				GetPosition(&x,&y,&z);

				// move coords
				if(moving_distance > moving_velocity)
				{
					x += moving_vector[0];
					y += moving_vector[1];
					if(moving_zmap) z = GetZCoord(x,y) + 0.5f;
					else z += moving_vector[2];
				}
				else
				{
					x += moving_vector[0] * (moving_distance / moving_velocity);
					y += moving_vector[1] * (moving_distance / moving_velocity);
					if(moving_zmap) z = GetZCoord(x,y) + 0.5f;
					else z += moving_vector[2] * (moving_distance / moving_velocity);
				}
				// set coords
				SetPosition(x,y,z);
				// change move steps
				moving_distance -= moving_velocity;
				// check path complete
				if(moving_distance <= 0.0f)
				{
					Stop();
					// callback
					pCallbacks->MovingComplete(local_id);
				}
			}
			/*
			if((knock_vector[0] != 0.0f) || (knock_vector[1] != 0.0f) || (knock_vector[2] != 0.0f))
			{
				if(sqrt((knock_vector[0] * knock_vector[0]) + (knock_vector[1] * knock_vector[1]) + (knock_vector[2] * knock_vector[2])) < 0.1f)
				{
					knock_vector[0] = 0.0f;
					knock_vector[1] = 0.0f;
					knock_vector[2] = 0.0f;
				}
				float x,y,z;
				GetPosition(&x,&y,&z);
				// moving
				x += knock_vector[0];
				y += knock_vector[1];
				z += knock_vector[2];
				// set coords
				SetPosition(x,y,z);
				// less velocity
				knock_vector[0] /= 1.05f;
				knock_vector[1] /= 1.05f;
				knock_vector[2] /= 1.05f;
			}
			*/
			ApplySync(UPDATE_TYPE_ONFOOT,PLAYER_STATE_ONFOOT);
			break;
		}
	case NPC_STATE_DRIVER:
		{
			if(moving_state)
			{
				// get current pos
				float x,y,z;
				GetPosition(&x,&y,&z);
				// move coords
				if(moving_distance > moving_velocity)
				{
					x += moving_vector[0];
					y += moving_vector[1];
					if(moving_zmap) z = GetZCoord(x,y) + 0.5f;
					else z += moving_vector[2];
				}
				else
				{
					x += moving_vector[0] * (moving_distance / moving_velocity);
					y += moving_vector[1] * (moving_distance / moving_velocity);
					if(moving_zmap) z = GetZCoord(x,y) + 0.5f;
					else z += moving_vector[2] * (moving_distance / moving_velocity);
				}
				// set coords
				SetPosition(x,y,z);
				// change move steps
				moving_distance -= moving_velocity;
				// check path complete
				if(moving_distance <= 0.0f)
				{
					Stop();
					// callback
					pCallbacks->MovingComplete(local_id);
				}
			}
			ApplySync(UPDATE_TYPE_INCAR,PLAYER_STATE_DRIVER);
			break;
		}
	case NPC_STATE_PASSENGER:
		{
			ApplySync(UPDATE_TYPE_PASSENGER,PLAYER_STATE_PASSENGER);
			break;
		}
	case NPC_STATE_DEATH:
		{
			ApplySync(UPDATE_TYPE_ONFOOT,PLAYER_STATE_ONFOOT);
			break;
		}
	case NPC_STATE_PLAYBACK:
		{
			if((GetTickCount() - playback_StartTime) >= playback_CurrTime)
			{
				switch(playback_type)
				{
				case PLAYER_RECORDING_TYPE_DRIVER:
					{
						// save vehicle id
						unsigned short vid = local_class->InCarSync.VehicleId;
						// set data
						local_class->InCarSync = playback_CurrInCarData;
						SetPosition(playback_CurrInCarData.Position[0],playback_CurrInCarData.Position[1],playback_CurrInCarData.Position[2]);
						// load vehicle id
						local_class->InCarSync.VehicleId = vid;
						// apply to sync
						ApplySync(UPDATE_TYPE_INCAR,PLAYER_STATE_DRIVER);
						break;
					}
				case PLAYER_RECORDING_TYPE_ONFOOT:
					{
						// set data
						local_class->FootSync = playback_CurrOnFootData;
						SetPosition(playback_CurrOnFootData.Position[0],playback_CurrOnFootData.Position[1],playback_CurrOnFootData.Position[2]);
						// apply to sync
						ApplySync(UPDATE_TYPE_ONFOOT,PLAYER_STATE_ONFOOT);
						break;
					}
				}
				// read next block
				ReadNextPlaybackBlock();
			}	
			break;
		}
	}
}

int CNPC::WeaponDamage(int a_id,float cp_x,float cp_y,float cp_z,float f_x,float f_y,float f_z,float health,int weapon,int type)
{
	// vars
	float	tmp_x;
	float	tmp_y;
	float	tmp_z;
	float	np_x;
	float	np_y;
	float	np_z;
	float	d_len;
	int		part;
	// npc pos
	GetPosition(&np_x,&np_y,&np_z);
	tmp_x = np_x - cp_x;
	tmp_y = np_y - cp_y;
	tmp_z = np_z - cp_z;
	// get distance of vector for check
	d_len = ((tmp_x / f_x) + (tmp_y / f_y)) / 2.f;
	// switch type
	switch(type)
	{
	case DAMAGE_TYPE_RAKET:
		{
			// get coords for range
			tmp_x = np_x - (cp_x + (d_len * f_x)); 
			tmp_y = np_y - (cp_y + (d_len * f_y)); 
			tmp_z = np_z - (cp_z + (d_len * f_z)); 
			// part
			if(IsInRange(tmp_x,tmp_y,tmp_z,5.0)) part = BODYPART_ALL;
			else part = -1;
			break;
		}
	case DAMAGE_TYPE_GRENADE:
		{
			if(IsInRange((np_x - cp_x),(np_y - cp_y),(np_z - cp_z),5.0f)) part = BODYPART_ALL;
			else part = -1;
			break;
		}
	case DAMAGE_TYPE_MEELE:
		{
			if(IsInRange(tmp_x,tmp_y,tmp_z,0.5f)) part = BODYPART_ALL;
			else part = -1;
			break;
		}
	case DAMAGE_TYPE_BULLET:
		{
			tmp_x = np_x - (cp_x + (d_len * f_x)); 
			tmp_y = np_y - (cp_y + (d_len * f_y)); 
			tmp_z = np_z - (cp_z + (d_len * f_z)); 
						
			// part
			if(IsInRange(tmp_x,tmp_y,(tmp_z + 0.7f),0.15f)) part = BODYPART_HEAD;
			else if(IsInRange(tmp_x,tmp_y,(tmp_z + 0.4f),0.35f)) part = BODYPART_TORSO;
			else if(IsInRange(tmp_x,tmp_y,(tmp_z - 0.4f),0.6f)) part = BODYPART_FOOT;
			else part = -1;
			break;
		}
	default:
		{
			part = -1;
			break;
		}
	}
	
	if(part != -1)
	{
		health -= WeaponsDamage[weapon];
		if(pCallbacks->GetDamage(local_id,a_id,WeaponsDamage[weapon],part))
		{
			SetHealth(health);
			if(health <= 0.0) Kill(a_id,weapon);
		}
		if(part != BODYPART_ALL) return 1;
	}
	return 0;
}

int CNPC::VehicleDamage(int a_id,float cp_x,float cp_y,float cp_z,float f_x,float f_y,float f_z,float health,int weapon,int type)
{
	// vars
	float	tmp_x;
	float	tmp_y;
	float	tmp_z;
	float	np_x;
	float	np_y;
	float	np_z;
	float	d_len;
	int		part;
	// npc pos
	GetPosition(&np_x,&np_y,&np_z);
	tmp_x = np_x - cp_x;
	tmp_y = np_y - cp_y;
	tmp_z = np_z - cp_z;
	// get distance of vector for check
	d_len = ((tmp_x / f_x) + (tmp_y / f_y)) / 2.f;
	// switch type
	switch(type)
	{
	case 1: // boom
		{
			// get coords for range
			tmp_x = np_x - (cp_x + (d_len * f_x)); 
			tmp_y = np_y - (cp_y + (d_len * f_y)); 
			tmp_z = np_z - (cp_z + (d_len * f_z)); 
			// part
			if(IsInRange(tmp_x,tmp_y,tmp_z,5.0f)) part = BODYPART_ALL;
			else part = -1;
			break;
		}
	case 2: // bullet by driver
		{
			// get coords for range
			tmp_x = np_x - (cp_x + (d_len * f_x)); 
			tmp_y = np_y - (cp_y + (d_len * f_y)); 
			tmp_z = np_z - (cp_z + (d_len * f_z)); 
			// part
			if(IsInRange(tmp_x,tmp_y,tmp_z,1.0f)) part = BODYPART_ALL;
			else part = -1;
			break;
		}
	case 3: // bullet by passanger
		{
			// get coords for range
			tmp_x = np_x - (cp_x + (d_len * f_x)); 
			tmp_y = np_y - (cp_y + (d_len * f_y)); 
			tmp_z = cp_z + (d_len * f_z); 
			// part
			if(IsInRange(tmp_x,tmp_y,((np_z + 0.8f) - tmp_z),0.15f)) part = BODYPART_HEAD;
			else if(IsInRange(tmp_x,tmp_y,((np_z + 0.4f) - tmp_z),0.3f)) part = BODYPART_TORSO;
			else if(IsInRange(tmp_x,tmp_y,((np_z - 0.4f) - tmp_z),0.5f)) part = BODYPART_FOOT;
			else part = -1;
			break;
		}
	default:
		{
			part = -1;
			break;
		}
	}

	if(part != -1)
	{
		health -= WeaponsDamage[weapon];
		if(pCallbacks->GetDamage(local_id,a_id,WeaponsDamage[weapon],part))
		{
			SetHealth(health);
			if(health <= 0.0) Kill(a_id,weapon);
		}
		if(part != BODYPART_ALL) return 1;
	}
	return 0;
}

void CNPC::VehicleKnock(int a_id,float pos_x,float pos_y,float pos_z,float v_x,float v_y,float v_z)
{
	if((knock_vector[0] != 0.0f) || (knock_vector[1] != 0.0f) || (knock_vector[2] != 0.0f)) return;
	// vars
	float	np_x;
	float	np_y;
	float	np_z;
	// npc pos
	GetPosition(&np_x,&np_y,&np_z);
	pos_x += (v_x * 5) - np_x;
	pos_y += (v_y * 5) - np_y;
	pos_z += (v_z * 5) - np_z;
	// if range
	if(IsInRange(pos_x,pos_y,pos_z,1.0))
	{
		if(sqrt((v_x * v_x) + (v_y * v_y) + (v_z * v_z)) < 0.1) return;
		knock_vector[0] = v_x * 2;
		knock_vector[1] = v_y * 2;
		knock_vector[2] = v_z * 2;
		// less health
		float health = GetHealth();
		float dmg = sqrt((knock_vector[0] * knock_vector[0]) + (knock_vector[1] * knock_vector[1]) + (knock_vector[2] * knock_vector[2])) * 20;
		health -= dmg;
		if(pCallbacks->GetDamage(local_id,a_id,dmg,4))
		{
			SetHealth(health);
			if(health <= 0.0) Kill(a_id,47);
		}
	}
}
