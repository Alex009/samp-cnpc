/*
 *	Copyright (C) 2013 Alex009
 *	License read in license.txt
 *	Description: Controllable NPCs damage class
 */

#pragma once

#include "CNPCCore.h"
#include "SAMP/CPlayers.h"

class CNPCDamage
{
public:
	CNPCDamage(CNPCCore* _pNPC);
	~CNPCDamage();

	void ProcessRangeWeaponDamage(double update_time);
	void ProcessMeleeWeaponDamage(double update_time) {}
	void ProcessVehicleWeaponDamage(double update_time) {}
	void ProcessVehicleCrashDamage(double update_time) {}
	bool Update(double update_time);

	static void Init();
	static void UpdatePlayersData(CPlayers* _pPlayers,double update_time);
	
	struct s_player_data
	{
		float cam_pos[3];
		float vec_ed[3];
		bool isfire;
		unsigned char weaponid;
		unsigned char state;
	};
	static s_player_data* pPlayersData[MAX_PLAYERS];
	static unsigned short ObjectsCount;

protected:
	CNPCCore*	pNPC;
};