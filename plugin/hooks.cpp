/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 */

#include <stdio.h>
#include "hooks.h"
#include "SAMP/address.h"

class FakeClass {};

#ifdef WIN32
	typedef void (FakeClass::* f_ClientConnect)(int,char*,char*,char*,int);
	typedef void (FakeClass::* f_ClientDisconnect)(int,int);
	typedef void (FakeClass::* f_SpawnForWorld)();
	typedef void (FakeClass::* f_DeathForWorld)(int,int);
	typedef unsigned short (FakeClass::* f_GetConfigVar)(char*);

	typedef void (* f_TeleportFunction)(int,float,float,float);
#else
	typedef void (* f_ClientConnect)(FakeClass*,int,char*,char*,char*,int);
	typedef void (* f_ClientDisconnect)(FakeClass*,int,int);
	typedef void (* f_SpawnForWorld)(FakeClass*);
	typedef void (* f_DeathForWorld)(FakeClass*,int,int);
	typedef unsigned short (* f_GetConfigVar)(FakeClass*,char*);
#endif

#ifndef PAGESIZE
	#define PAGESIZE (4096)
#endif

// vars
f_ClientConnect		pClientConnect;
f_ClientDisconnect	pClientDisconnect;
f_SpawnForWorld		pClientSpawn;
f_DeathForWorld		pClientDeath;
f_GetConfigVar		pGetConfigVar;

// funcs
bool Unlock(void *address, int len);
void CallHook(DWORD from, DWORD to);
void SAMP_ThreadComplete();

// class
CHooks::CHooks(unsigned long s_version)
{
	void* temp;
	POINTER_TO_MEMBER(pClientConnect,(void *)(CLIENT_CONNECT),f_ClientConnect);
	// disconnect
	POINTER_TO_MEMBER(pClientDisconnect,(void *)(CLIENT_DISCONNECT),f_ClientDisconnect);
	// spawn
	POINTER_TO_MEMBER(pClientSpawn,(void *)(SPAWN_FOR_WORLD),f_SpawnForWorld);
	// death
	POINTER_TO_MEMBER(pClientDeath,(void *)(KILL_FOR_WORLD),f_DeathForWorld);
	// config var
	POINTER_TO_MEMBER(pGetConfigVar,(void *)(GET_CONFIG_VAR),f_GetConfigVar);
	// set hook
	Unlock((void*)(ADDR_THREAD_START),(ADDR_THREAD_END - ADDR_THREAD_START));
	CallHook(ADDR_THREAD_START,(unsigned long)SAMP_ThreadComplete);
	for(unsigned int i = ADDR_THREAD_START + 5;i < ADDR_THREAD_END;i++) *(unsigned char*)(i) = 0x90;
}

CHooks::~CHooks()
{

}

void CHooks::ClientConnect(void* players,int id,char* name)
{
	CALL_ARGS(players,pClientConnect,id _ name _ NULL _ NULL _ 1);
}

void CHooks::ClientDisconnect(void* players,int id,int reason)
{
	CALL_ARGS(players,pClientDisconnect,id _ 1);
}

void CHooks::ClientSpawn(void* player)
{
	CALL_NOARGS(player,pClientSpawn);
}

void CHooks::ClientDeath(void* player,int reason,int killerid)
{
	CALL_ARGS(player,pClientDeath,reason _ killerid);
}

unsigned short CHooks::GetMaxPlayers()
{
	return CALL_ARGS(*(DWORD*)(CONFIG_CLASS),pGetConfigVar,"maxplayers");
}

// other
bool Unlock(void *address, int len)
{
	#ifdef WIN32
		DWORD
			oldp;
		// Shut up the warnings :D
		return !!VirtualProtect(address, len, PAGE_EXECUTE_READWRITE, &oldp);
	#else
		return !mprotect((void*)(((int)address / PAGESIZE) * PAGESIZE), PAGESIZE, PROT_WRITE | PROT_READ | PROT_EXEC);
	#endif
}

void CallHook(DWORD from, DWORD to) 
{
	if(Unlock((void*)from,5))
	{
		DWORD disp = to - (from + 5);
		*(BYTE *)(from) = 0xE8;
		*(DWORD *)(from + 1) = (DWORD)disp;
	}
}