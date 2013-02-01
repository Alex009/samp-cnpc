/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: callbacks
 */

// SDK
#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"
// plugin
#include "callbacks.h"

CCallbacks::CCallbacks(MUTEX_IDENTIFY(mutex)) 
{
	CallbackMutex = mutex;
	for(int i = 0;i < 17;i++) SampObjects[i] = 0;
}

CCallbacks::~CCallbacks() {}

int CCallbacks::GetDamage(int npcid,int attacker,float damage,int bpart)
{
	MUTEX_ENTER(CallbackMutex);

	int index;
	cell ret;
	int result = 1;

	for(int i = 0;i < 17;i++)
	{
		if(SampObjects[i] != 0)
		{
			if(!amx_FindPublic(SampObjects[i],"OnNPCGetDamage",&index))
			{
				amx_Push(SampObjects[i],(cell)bpart);
				amx_Push(SampObjects[i],amx_ftoc(damage));
				amx_Push(SampObjects[i],(cell)attacker);
				amx_Push(SampObjects[i],(cell)npcid);
				amx_Exec(SampObjects[i], &ret, index);
				if(ret == 0) result = 0;
			}
		}
	}

	MUTEX_EXIT(CallbackMutex);

	return result;
}

int CCallbacks::MovingComplete(int npcid)
{
	MUTEX_ENTER(CallbackMutex);

	int index;
	cell ret;
	int result = 1;

	for(int i = 0;i < 17;i++)
	{
		if(SampObjects[i] != 0)
		{
			if(!amx_FindPublic(SampObjects[i],"OnNPCMovingComplete",&index))
			{
				amx_Push(SampObjects[i],(cell)npcid);
				amx_Exec(SampObjects[i], &ret, index);
				if(ret == 0) result = 0;
			}
		}
	}

	MUTEX_EXIT(CallbackMutex);

	return result;
}

int CCallbacks::PlaybackEnd(int npcid,int reason)
{
	MUTEX_ENTER(CallbackMutex);

	int index;
	cell ret;
	int result = 1;

	for(int i = 0;i < 17;i++)
	{
		if(SampObjects[i] != 0)
		{
			if(!amx_FindPublic(SampObjects[i],"OnRecordingPlaybackEnd",&index))
			{
				amx_Push(SampObjects[i],(cell)reason);
				amx_Push(SampObjects[i],(cell)npcid);
				amx_Exec(SampObjects[i], &ret, index);
				if(ret == 0) result = 0;
			}
		}
	}

	MUTEX_EXIT(CallbackMutex);

	return result;
}

int CCallbacks::Spawn(int npcid)
{
	MUTEX_ENTER(CallbackMutex);

	int index;
	cell ret;
	int result = 1;

	for(int i = 0;i < 17;i++)
	{
		if(SampObjects[i] != 0)
		{
			if(!amx_FindPublic(SampObjects[i],"OnNPCSpawn",&index))
			{
				amx_Push(SampObjects[i],(cell)npcid);
				amx_Exec(SampObjects[i], &ret, index);
				if(ret == 0) result = 0;
			}
		}
	}

	MUTEX_EXIT(CallbackMutex);

	return result;
}

int CCallbacks::Death(int npcid,int killer,int reason)
{
	MUTEX_ENTER(CallbackMutex);

	int index;
	cell ret;
	int result = 1;

	for(int i = 0;i < 17;i++)
	{
		if(SampObjects[i] != 0)
		{
			if(!amx_FindPublic(SampObjects[i],"OnNPCDeath",&index))
			{
				amx_Push(SampObjects[i],(cell)reason);
				amx_Push(SampObjects[i],(cell)killer);
				amx_Push(SampObjects[i],(cell)npcid);
				amx_Exec(SampObjects[i], &ret, index);
				if(ret == 0) result = 0;
			}
		}
	}

	MUTEX_EXIT(CallbackMutex);

	return result;
}

void CCallbacks::CallbacksOnAMXLoad(AMX* amx)
{
	for(int i = 0;i < 17;i++)
	{
		if(SampObjects[i] == 0)
		{
			SampObjects[i] = amx;
			break;
		}
	}
}

void CCallbacks::CallbacksOnAMXUnLoad(AMX* amx)
{
	for(int i = 0;i < 17;i++)
	{
		if(SampObjects[i] == amx)
		{
			SampObjects[i] = 0;
			break;
		}
	}
}