/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: callbacks
 */

// SDK
#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"
// plugin
#include "os.h"

class CCallbacks
{
public:
	CCallbacks(MUTEX_IDENTIFY(mutex)) ;
	~CCallbacks();

	int GetDamage(int npcid,int attacker,float damage,int bpart);
	int MovingComplete(int npcid);
	int PlaybackEnd(int npcid,int reason);
	int Spawn(int npcid);
	int Death(int npcid,int killer,int reason);

	void CallbacksOnAMXLoad(AMX* amx);
	void CallbacksOnAMXUnLoad(AMX* amx);

	// vars
	AMX* SampObjects[17];
	MUTEX_IDENTIFY(CallbackMutex);
};