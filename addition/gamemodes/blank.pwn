/*
*	Created:			09.03.10
*	Author:				009
*	Last Modifed:		18.08.12
*	Description:		Blank for Controllable NPC
*/

main() {}

// --------------------------------------------------
// includes
// --------------------------------------------------
#include <a_samp>
#include <cnpc>

// --------------------------------------------------
// defines
// --------------------------------------------------

// --------------------------------------------------
// enums
// --------------------------------------------------


// --------------------------------------------------
// news
// --------------------------------------------------
new str[128];

// --------------------------------------------------
// forwards
// --------------------------------------------------

// --------------------------------------------------
// publics
// --------------------------------------------------
public OnGameModeInit()
{
	AddPlayerClass(1,-1657.8768,-165.0978,14.1484,308.8885,0,0,0,0,0,0);
	return 1;
}

public OnPlayerConnect(playerid)
{
    printf("OnPlayerConnect(%d)",playerid);
}

public OnPlayerDisconnect(playerid, reason)
{
    printf("OnPlayerDisconnect(%d,%d)",playerid, reason);
}

public OnPlayerSpawn(playerid)
{
    printf("OnPlayerSpawn(%d)",playerid);
}

public OnPlayerDeath(playerid, killerid, reason)
{
    printf("OnPlayerDeath(%d,%d,%d)",playerid, killerid, reason);
}

public OnNPCGetDamage(npcid,playerid,Float:health_loss,bodypart)
{
	format(str,sizeof(str),"OnNPCGetDamage(%d,%d,%f,%d)",npcid,playerid,health_loss,bodypart);
	SendClientMessageToAll(0xFFFFFFFF,str);
	return 1;
}

public OnRecordingPlaybackEnd(npcid,reason)
{
	printf("OnRecordingPlaybackEnd(%d,%d)",npcid,reason);
}

public OnNPCMovingComplete(npcid)
{
	printf("OnNPCMovingComplete(%d)",npcid);
}

public OnNPCSpawn(npcid)
{
	printf("OnNPCSpawn(%d)",npcid);
}

public OnNPCDeath(npcid,killerid,reason)
{
	printf("OnNPCDeath(%d,%d,%d)",npcid,killerid,reason);
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	new cmd[20],
		idx;

	cmd = strtok(cmdtext,idx);

	if(!strcmp(cmd,"/money",true))
	{
		GivePlayerMoney(playerid,strval(strtok(cmdtext,idx)));
	    return 1;
	}
	if(!strcmp(cmd,"/w",true))
	{
		GivePlayerWeapon(playerid,strval(strtok(cmdtext,idx)),999);
	    return 1;
	}
	if(!strcmp(cmd,"/car",true))
	{
		new Float:pos[4];
		GetPlayerPos(playerid,pos[0],pos[1],pos[2]);
		GetPlayerFacingAngle(playerid,pos[3]);
	    PutPlayerInVehicle(playerid,AddStaticVehicle(strval(strtok(cmdtext,idx)),pos[0],pos[1],pos[2],pos[3],0,0),0);
	    return 1;
	}
	if(!strcmp(cmd,"/tp",true))
	{
		new Float:pos[3];
		GetPlayerPos(playerid,pos[0],pos[1],pos[2]);
	    SetNPCPos(strval(strtok(cmdtext,idx)),pos[0],pos[1],pos[2]);
	    return 1;
	}
	if(!strcmp(cmd,"/tp2",true))
	{
		new Float:pos[3];
		GetNPCPos(strval(strtok(cmdtext,idx)),pos[0],pos[1],pos[2]);
	    SetPlayerPos(playerid,pos[0],pos[1],pos[2] + 2.0);
	    return 1;
	}
	return 0;
}

// --------------------------------------------------
// stocks
// --------------------------------------------------
stock strtok(const string[], &index)
{
	new length = strlen(string);
	while ((index < length) && (string[index] <= ' '))
	{
		index++;
	}

	new offset = index;
	new result[20];
	while ((index < length) && (string[index] > ' ') && ((index - offset) < (sizeof(result) - 1)))
	{
		result[index - offset] = string[index];
		index++;
	}
	result[index - offset] = EOS;
	return result;
}
