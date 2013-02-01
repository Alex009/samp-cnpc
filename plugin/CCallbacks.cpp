/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: callbacks
 */

// SDK
#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"
// plugin
#include "CCallbacks.h"

CCallbacks::CCallbacks() 
{
	for(int i = 0;i < 17;i++) SampObjects[i] = 0;
}

CCallbacks::~CCallbacks() {}

int CCallbacks::GetDamage(int npcid,int attacker,float damage,int bpart)
{
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

	return result;
}

int CCallbacks::MovingComplete(int npcid)
{
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

	return result;
}

int CCallbacks::PlaybackEnd(int npcid,int reason)
{
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

	return result;
}

int CCallbacks::Spawn(int npcid)
{
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

	return result;
}

int CCallbacks::Death(int npcid,int killer,int reason)
{
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

	return result;
}

int CCallbacks::Custom(char* callback,void* params,int params_count)
{
	int index;
	cell ret;
	int result = 1;

	for(int i = 0;i < 17;i++)
	{
		if(SampObjects[i] != 0)
		{
			if(!amx_FindPublic(SampObjects[i],callback,&index))
			{
				for(int j = params_count - 1;j >= 0;j--) amx_Push(SampObjects[i],((cell*)params)[j]);
				amx_Exec(SampObjects[i], &ret, index);
				if(ret == 0) result = 0;
			}
		}
	}

	return result;
}

int CCallbacks::PathCalculated(unsigned short pathid)
{
	int index;
	cell ret;
	int result = 1;

	for(int i = 0;i < 17;i++)
	{
		if(SampObjects[i] != 0)
		{
			if(!amx_FindPublic(SampObjects[i],"OnPathCalculated",&index))
			{
				amx_Push(SampObjects[i],pathid);
				amx_Exec(SampObjects[i], &ret, index);
				if(ret == 0) result = 0;
			}
		}
	}

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