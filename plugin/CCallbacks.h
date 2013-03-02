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
	CCallbacks();
	int GetDamage(unsigned short npcid,unsigned short attacker,float damage,unsigned char bpart);
	int MovingComplete(unsigned short npcid);
	int PlaybackEnd(unsigned short npcid,unsigned char reason);
	int Spawn(unsigned short npcid);
	int Death(unsigned short npcid,unsigned short killer,unsigned char reason);
	int PathCalculated(unsigned short pathid);
	int Custom(char* callback,void* params,int params_count);
	void CallbacksOnAMXLoad(AMX* amx);
	void CallbacksOnAMXUnLoad(AMX* amx);

private:
	AMX* SampObjects[17];
};
