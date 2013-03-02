/*
 *	Copyright (C) 2013 Alex009
 *	License read in license.txt
 *	Description: Controllable NPCs core class
 */

#pragma once

#include "SAMP/CPlayer.h"

class CNPCCore
{
public:
	CNPCCore(unsigned short id,char* name);
	~CNPCCore();

	virtual void	Spawn();
	virtual void	Kill(unsigned short killerid,unsigned char reason);
	void			SetState(unsigned char state);
	unsigned char	GetState();
	void			SetSkin(unsigned long skin);
	unsigned long	GetSkin();
	void			SetInterior(unsigned long interior);
	unsigned long	GetInterior();
	void			PutInVehicle(unsigned short vehicleid,unsigned char seat);
	void			SetPosition(float x,float y,float z);
	void			GetPosition(float* x,float* y,float* z);
	void			SetQuaternion(float x,float y,float z,float s);
	void			GetQuaternion(float* x,float* y,float* z,float* s);
	void			SetAngle(float angle);
	float			GetAngle(); 
	void			SetVelocity(float x,float y,float z);
	void			GetVelocity(float* x,float* y,float* z);
	void			SetKeys(unsigned short ud,unsigned short lr,unsigned short other);
	void			GetKeys(unsigned short* ud,unsigned short* lr,unsigned short* other);
	void			SetSpecialAction(unsigned char actionid);
	unsigned char	GetSpecialAction();
	void			SetAnimationIndex(unsigned long animationid);
	unsigned long	GetAnimationIndex();
	void			SetWeapon(unsigned char weaponid);
	unsigned char	GetWeapon();
	void			SetWeaponSkillLevel(unsigned char weapontype,unsigned short level);
	unsigned short	GetWeaponSkillLevel(unsigned char weapontype);
	void			SetHealth(float health);
	float			GetHealth();
	void			SetArmour(float armour);
	float			GetArmour();
	void			SetSurfing(float x,float y,float z);
	void			GetSurfing(float* x,float* y,float* z);
	void			SetSurfingVehicle(unsigned short vehicleid);
	unsigned short	GetSurfingVehicle();
	void			SetCameraPos(float x,float y,float z);
	void			GetCameraPos(float* x,float* y,float* z);
	void			SetCameraFrontVector(float x,float y,float z);
	void			GetCameraFrontVector(float* x,float* y,float* z);
	void			SetCameraZAim(float angle);
	float			GetCameraZAim();
	void			SetCameraMode(unsigned char mode);
	unsigned char	GetCameraMode();
	void			SetWeaponState(unsigned char state);
	unsigned char	GetWeaponState();
	void			SetVehicleSiren(bool state);
	bool			GetVehicleSiren();
	void			SetVehicleHealth(float health);
	float			GetVehicleHealth();
	void			SetVehicleUnique(float unique);
	float			GetVehicleUnique();
	void			SetVehicleTrailer(unsigned short trailer);
	unsigned short	GetVehicleTrailer();
	void			SetPassangerDriveBy(bool dstate);
	bool			GetPassangerDriveBy();
	CPlayer*		GetCPlayerPointer() {return local_class;}
	unsigned short	GetMyPlayerid() {return local_id;}
	void			Update(double update_time);
	
protected:
	unsigned short	local_id;
	CPlayer*		local_class;
	double			last_update;
};