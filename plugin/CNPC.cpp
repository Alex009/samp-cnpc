/*
 *	Copyright (C) 2013 Alex009
 *	License read in license.txt
 */

#define _USE_MATH_DEFINES

#include "CNPC.h"
#include "CCallbacks.h"
#include "defines.h"
#include "utils/utils.h"
#include "SWeaponData.h"
#include <math.h>

extern	CCallbacks*	pCallbacks;

CNPC::CNPC(unsigned short id,char* name): CNPCCore(id,name)
{
	pMoving = NULL;
	pPlayback = NULL;
	pDamage = new CNPCDamage(this);
}

CNPC::~CNPC()
{
	if(pMoving) delete pMoving;
	if(pPlayback) 
	{
		delete pPlayback;
		pCallbacks->PlaybackEnd(local_id,NPC_RECORD_END_REASON_DESTROY);
	}
	if(pDamage) delete pDamage;
}

void CNPC::Spawn()
{
	if(pMoving)
	{
		delete pMoving;
		pMoving = NULL;
	}
	if(pPlayback)
	{
		delete pPlayback;
		pPlayback = NULL;
		pCallbacks->PlaybackEnd(local_id,NPC_RECORD_END_REASON_STOP);
	}

	CNPCCore::Spawn();
}

void CNPC::Kill(unsigned short killerid,unsigned char reason)
{
	if(pMoving)
	{
		delete pMoving;
		pMoving = NULL;
	}
	if(pPlayback)
	{
		delete pPlayback;
		pPlayback = NULL;
		pCallbacks->PlaybackEnd(local_id,NPC_RECORD_END_REASON_DEATH);
	}

	CNPCCore::Kill(killerid,reason);
}

void CNPC::Stop()
{
	// check moving
	if(pMoving) 
	{
		delete pMoving;
		pMoving = NULL;
	}
	if(pPlayback)
	{
		delete pPlayback;
		pPlayback = NULL;
		pCallbacks->PlaybackEnd(local_id,NPC_RECORD_END_REASON_STOP);
	}
	// stop all actions
	this->SetAnimationIndex(0);
	this->SetPassangerDriveBy(false);
	this->SetSpecialAction(0);
	this->SetVelocity(0.0,0.0,0.0);
	this->SetKeys(0,0,0);
}

bool CNPC::Move(float x,float y,float z,float velocity,bool zmap,bool allaxisrotate)
{
	// check already exist destination
	if(pMoving) delete pMoving;
	// create moving
	pMoving = new CNPCMoving(this);
	// try start moving
	if(!pMoving->Move(x,y,z,velocity,zmap,allaxisrotate))
	{
		delete pMoving;
		pMoving = NULL;
		return false;
	}
	return true;
}

void CNPC::Look(float x,float y,float z)
{
	unsigned short ud,lr,other;
	float mx,my,mz;
	float cpx,cpy,cpz;
	float tx,ty,tz;
	float vx,vy,vz;
	float len;
	unsigned char weaponid;
	// get my data
	GetKeys(&ud,&lr,&other);
	GetPosition(&mx,&my,&mz);
	// get weapon data
	weaponid = GetWeapon();
	SWeaponData* wd = GetWeaponData(weaponid);
	// find vector mypos - target
	vx = x - mx;
	vy = y - my;
	vz = z - mz;
	// find z rotate
	float a = (atan2f(vy,vx) * (180.0f / (float)M_PI)) + 270.0f;
	if(a > 360.0f) a -= 360.0f;
	// apply rotate
	SetAngle(a);
	// calc x,y fix
	cpx = wd->cam_pos_fix[0];
	cpy = wd->cam_pos_fix[1];
	// fix
	a *= ((float)M_PI / 180.0f);
	tx = cpx*cosf(a) - cpy*sinf(a);
	ty = cpx*sinf(a) + cpy*cosf(a);
	// save
	cpx = tx;
	cpy = ty;
	// calc side a
	tx = mx+cpx - x;
	ty = my+cpy - y;
	tz = mz - z;
	// calc z fix
	cpz = wd->cam_pos_fix[3] * sqrt(tx*tx + ty*ty + tz*tz);
	// calc cam pos
	cpx += mx;
	cpy += my;
	cpz += mz;
	// recalc aim vector
	vx = x - cpx;
	vy = y - cpy;
	vz = z - cpz;
	// set len = 1
	len = sqrt(vx*vx + vy*vy + vz*vz);
	vx /= len;
	vy /= len;
	vz /= len;
	// fix gta aim
	rotate_vector(vx,vy,vz,wd->vector_angles[0],wd->vector_angles[1]);
	// apply data
	SetCameraPos(cpx,cpy,cpz);
	SetCameraFrontVector(vx,vy,vz);
}

void CNPC::Aim(float x,float y,float z)
{
	unsigned short ud,lr,other;
	float mx,my,mz;
	float cpx,cpy,cpz;
	float tx,ty,tz;
	float vx,vy,vz;
	float len;
	unsigned char weaponid;
	// get my data
	GetKeys(&ud,&lr,&other);
	GetPosition(&mx,&my,&mz);
	// get weapon data
	weaponid = GetWeapon();
	SWeaponData* wd = GetWeaponData(weaponid);
	// select way
	switch(wd->type)
	{
	case WEAPON_TYPE_SHOOT:
		{
			// find vector mypos - target
			vx = x - mx;
			vy = y - my;
			vz = z - mz;
			// find z rotate
			float a = (atan2f(vy,vx) * (180.0f / (float)M_PI)) + 270.0f;
			if(a > 360.0f) a -= 360.0f;
			// apply rotate
			SetAngle(a);
			// calc x,y fix
			cpx = wd->cam_pos_fix[0];
			cpy = wd->cam_pos_fix[1];
			// fix
			a *= ((float)M_PI / 180.0f);
			tx = cpx*cosf(a) - cpy*sinf(a);
			ty = cpx*sinf(a) + cpy*cosf(a);
			// save
			cpx = tx;
			cpy = ty;
			// calc side a
			tx = mx+cpx - x;
			ty = my+cpy - y;
			tz = mz - z;
			// calc z fix
			cpz = wd->cam_pos_fix[3] * sqrt(tx*tx + ty*ty + tz*tz);
			// calc cam pos
			cpx += mx;
			cpy += my;
			cpz += mz;
			// recalc aim vector
			vx = x - cpx;
			vy = y - cpy;
			vz = z - cpz;
			// set len = 1
			len = sqrt(vx*vx + vy*vy + vz*vz);
			vx /= len;
			vy /= len;
			vz /= len;
			// fix gta aim
			rotate_vector(vx,vy,vz,wd->vector_angles[0],wd->vector_angles[1]);
			// apply data
			SetCameraPos(cpx,cpy,cpz);
			SetCameraFrontVector(vx,vy,vz);
			SetKeys(ud,lr,KEY_HANDBRAKE);
			SetCameraMode(7);
			SetWeaponState(WEAPONSTATE_MORE_BULLETS);
			break;
		}
	}
}

void CNPC::Fire(float x,float y,float z)
{
	unsigned short ud,lr,other;
	float mx,my,mz;
	float cpx,cpy,cpz;
	float tx,ty;//,tz;
	float vx,vy,vz;
	float len;
	unsigned char weaponid;
	// get my data
	GetKeys(&ud,&lr,&other);
	GetPosition(&mx,&my,&mz);
	// get weapon data
	weaponid = GetWeapon();
	SWeaponData* wd = GetWeaponData(weaponid);
	// select way
	switch(wd->type)
	{
	case WEAPON_TYPE_SHOOT:
		{
			// find vector mypos - target
			vx = x - mx;
			vy = y - my;
			vz = z - mz;
			// find z rotate
			float a = (atan2f(vy,vx) * (180.0f / (float)M_PI)) + 270.0f;
			if(a > 360.0f) a -= 360.0f;
			// apply rotate
			SetAngle(a);
			// calc x,y fix
			cpx = wd->cam_pos_fix[0];
			cpy = wd->cam_pos_fix[1];
			// fix
			a *= ((float)M_PI / 180.0f);
			tx = cpx*cosf(a) - cpy*sinf(a);
			ty = cpx*sinf(a) + cpy*cosf(a);
			// save
			cpx = tx;
			cpy = ty;
			// calc z fix
			cpz = wd->cam_pos_fix[3] * (mz - z) + 1.0f;
			// calc cam pos
			cpx += mx;
			cpy += my;
			cpz += mz;
			// recalc aim vector
			vx = x - cpx;
			vy = y - cpy;
			vz = z - cpz;
			// set len = 1
			len = sqrt(vx*vx + vy*vy + vz*vz);
			vx /= len;
			vy /= len;
			vz /= len;
			// fix gta aim
			rotate_vector(vx,vy,vz,-wd->vector_angles[0],-wd->vector_angles[1]);
			// apply data
			SetCameraPos(cpx,cpy,cpz);
			SetCameraFrontVector(vx,vy,vz);
			SetKeys(ud,lr,KEY_HANDBRAKE + KEY_FIRE);
			SetCameraMode(7);
			SetWeaponState(WEAPONSTATE_MORE_BULLETS);
			break;
		}
	}
}

bool CNPC::StartRecordingPlayback(char* name)
{
	if(pPlayback) return false;
	
	pPlayback = new CNPCPlayback(this);
	if(!pPlayback->Start(name))
	{
		delete pPlayback;
		pPlayback = NULL;
		pCallbacks->PlaybackEnd(local_id,NPC_RECORD_END_REASON_NOT_FOUND);
		return false;
	}
	return true;
}

bool CNPC::PauseRecordingPlayback()
{
	if(!pPlayback) return false;
	
	return pPlayback->Pause();
}

bool CNPC::ContinueRecordingPlayback()
{
	if(!pPlayback) return false;
	
	return pPlayback->Continue();
}

bool CNPC::StopRecordingPlayback()
{
	if(!pPlayback) return false;
	
	delete pPlayback;
	pPlayback = NULL;
	pCallbacks->PlaybackEnd(local_id,NPC_RECORD_END_REASON_STOP);
	return true;
}

void CNPC::SetImpregnable(bool state)
{
	if((state) && (!pDamage))
	{
		pDamage = new CNPCDamage(this);
	}
	else if(pDamage)
	{
		delete pDamage;
		pDamage = NULL;
	}
}

void CNPC::Update(double update_time)
{
	if(pMoving)
	{
		if(pMoving->Update(update_time))
		{
			delete pMoving;
			pMoving = NULL;
			pCallbacks->MovingComplete(local_id);
		}
	}
	if(pPlayback)
	{
		if(pPlayback->Update(update_time))
		{
			delete pPlayback;
			pPlayback = NULL;
			pCallbacks->PlaybackEnd(local_id,NPC_RECORD_END_REASON_END);
		}
	}
	if(pDamage)
	{
		pDamage->Update(update_time);
	}

	CNPCCore::Update(update_time);
}