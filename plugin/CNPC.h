/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: Controllable NPCs class
 */

#include "SAMP/CPlayer.h"
#include "SAMP/packets.h"
#include <stdio.h>

class CNPC
{
public:
	CNPC(int id,char* name);
	~CNPC();

	// core
	void	Spawn();
	void	Kill(int killerid,int reason);
	void	SetState(int state);
	int		GetState();
	void	SetSkin(int skin);
	int		GetSkin();
	void	SetInterior(int interior);
	int		GetInterior();
	void	SetImpregnable(bool istate);
	bool	IsImpregnable();
	void	PutInVehicle(int vehicleid,int seat);
	void	SetPosition(float x,float y,float z);
	void	GetPosition(float* x,float* y,float* z);
	void	SetQuaternion(float x,float y,float z,float s);
	void	GetQuaternion(float* x,float* y,float* z,float* s);
	void	SetAngle(float angle);
	float	GetAngle(); 
	void	SetVelocity(float x,float y,float z);
	void	GetVelocity(float* x,float* y,float* z);
	void	SetKeys(int ud,int lr,int other);
	void	GetKeys(int* ud,int* lr,int* other);
	void	SetSpecialAction(int actionid);
	int		GetSpecialAction();
	void	SetWeapon(int weaponid);
	int		GetWeapon();
	void	SetWeaponSkillLevel(int weapontype,int level);
	int		GetWeaponSkillLevel(int weapontype);
	void	SetHealth(float health);
	float	GetHealth();
	void	SetArmour(float armour);
	float	GetArmour();
	void	SetSurfing(float x,float y,float z);
	void	GetSurfing(float* x,float* y,float* z);
	void	SetSurfingVehicle(int vehicleid);
	int		GetSurfingVehicle();
	void	SetCameraPos(float x,float y,float z);
	void	GetCameraPos(float* x,float* y,float* z);
	void	SetCameraFrontVector(float x,float y,float z);
	void	GetCameraFrontVector(float* x,float* y,float* z);
	void	SetCameraZAim(float angle);
	float	GetCameraZAim();
	void	SetCameraMode(int mode);
	int		GetCameraMode();
	void	SetWeaponState(int wstate);
	int		GetWeaponState();
	void	SetVehicleSiren(int state);
	int		GetVehicleSiren();
	void	SetVehicleHealth(float health);
	float	GetVehicleHealth();
	void	SetVehicleUnique(float unique);
	float	GetVehicleUnique();
	void	SetVehicleTrailer(int trailer);
	int		GetVehicleTrailer();
	void	SetPassangerDriveBy(int dstate);
	int		GetPassangerDriveBy();
	// moving
	void	GoTo(float x,float y,float z,float step,bool use_z_map);
	void	DriveTo(float x,float y,float z,float step,bool use_z_map);
	void	Stop();
	// playbacks
	int		StartRecordingPlayback(char* name);
	void	ReadNextPlaybackBlock();
	void	PauseRecordingPlayback();
	void	ContinueRecordingPlayback();
	void	StopRecordingPlayback(int reason);
	// thread
	void	Sync();
	int		WeaponDamage(int a_id,float cp_x,float cp_y,float cp_z,float f_x,float f_y,float f_z,float health,int weapon,int type);
	int		VehicleDamage(int a_id,float cp_x,float cp_y,float cp_z,float f_x,float f_y,float f_z,float health,int weapon,int type);
	void	VehicleKnock(int a_id,float pos_x,float pos_y,float pos_z,float v_x,float v_y,float v_z);

	// vars
	int				local_id;
	CPlayer*		local_class;
	int				local_state;
	int				local_playback_type;
	bool			local_impregnable;

	float			moving_speed;
	float			moving_distance;
	float			moving_vector[3];
	bool			moving_state;
	bool			moving_zmap;
	
	bool			playback_state;
	int				playback_type;
	FILE*			playback_file;
	unsigned long	playback_StartTime;
	Packet_InCar	playback_CurrInCarData;
	Packet_Foot		playback_CurrOnFootData;
	unsigned long	playback_CurrTime;

	float			knock_vector[3];
};