/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 */

#include "os.h"

// class
class CHooks
{
public:
	// functions
	CHooks(unsigned long s_version);

	void ClientConnect(void* players,int id,char* name);
	void ClientDisconnect(void* players,int id,int reason);
	void ClientSpawn(void* player);
	void ClientDeath(void* player,int reason,int killerid);
	void PlayerStreamOut(void* player,int forplayerid);
	unsigned short GetMaxPlayers();
};