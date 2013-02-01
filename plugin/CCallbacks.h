/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: callbacks
 */

#pragma once

// SDK
#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"

class CCallbacks
{
public:
	CCallbacks() ;
	~CCallbacks();

	int GetDamage(int npcid,int attacker,float damage,int bpart);
	int MovingComplete(int npcid);
	int PlaybackEnd(int npcid,int reason);
	int Spawn(int npcid);
	int Death(int npcid,int killer,int reason);
	int Custom(char* callback,void* params,int params_count);
	int PathCalculated(unsigned short pathid);

	void CallbacksOnAMXLoad(AMX* amx);
	void CallbacksOnAMXUnLoad(AMX* amx);

	// vars
	AMX* SampObjects[17];
};
