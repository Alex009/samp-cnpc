/*
*	Created:			09.03.10
*	Author:				009
*	Last Modifed:		18.08.12
*	Description:		Sample for Controllable NPC
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
#define for_npcs(%1) for(new %1 = 499;%1 >= 10;%1--)

#define GetDistanceBetweenCoords(%1,%2,%3,%4,%5,%6) floatsqroot((%4 - %1)*(%4 - %1) + (%5 - %2)*(%5 - %2) + (%6 - %3)*(%6 - %3))

// --------------------------------------------------
// enums
// --------------------------------------------------
enum {
	NPC_STATUS_WAIT,
	NPC_STATUS_WALK_TO_CLOSEST,
	NPC_STATUS_RUN_TO_CLOSEST,
	NPC_STATUS_SPRINT_TO_CLOSEST,
	NPC_STATUS_LOOK_AT_CLOSEST,
	NPC_STATUS_AIM_AT_CLOSEST,
	NPC_STATUS_SHOT_AT_CLOSEST
};

// --------------------------------------------------
// news
// --------------------------------------------------
new NPC_STATUS,
	badSkins[] = {3, 4, 5, 6, 8, 42, 65, 74, 86, 119, 149, 208, 273, 289},
	Float:floattmp[9];
new str[128];

// --------------------------------------------------
// forwards
// --------------------------------------------------
forward JokeNPC();

// --------------------------------------------------
// publics
// --------------------------------------------------
public OnGameModeInit()
{
	ToogleVisualDeath(0);
	
	new skin;
	AddPlayerClass(1,-1657.8768,-165.0978,14.1484,308.8885,0,0,0,0,0,0);
	for_npcs(i)
	{
		CreateNPC(i,"Just NPC");
		do skin = random(300);
		while(!IsValidSkin(skin));
		// move coords
		floattmp[0] += 1.5;
		floattmp[1] += 1.5;
		// set spawn info
		SetSpawnInfo(i,0,skin,(-1657.8768 + floattmp[0]),(-165.0978 + floattmp[1]),14.1484,308.8885,0,0,0,0,0,0);
		SpawnNPC(i);
	}
	NPC_STATUS = NPC_STATUS_WAIT;
	SetTimer("JokeNPC",1000,1);
	return 1;
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

	ApplyAnimation(npcid,"PED","KO_skid_front",4.1,0,1,1,1,1);
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
	if(!strcmp(cmd,"/walk",true))
	{
		new Float:pos[3];
		GetPlayerPos(playerid,pos[0],pos[1],pos[2]);
		NPC_WalkTo(strval(strtok(cmdtext,idx)),pos[0],pos[1],pos[2],0);
	    return 1;
	}
	if(!strcmp(cmd,"/run",true))
	{
		new Float:pos[3];
		GetPlayerPos(playerid,pos[0],pos[1],pos[2]);
		NPC_RunTo(strval(strtok(cmdtext,idx)),pos[0],pos[1],pos[2],0);
	    return 1;
	}
	if(!strcmp(cmd,"/sprint",true))
	{
		new Float:pos[3];
		GetPlayerPos(playerid,pos[0],pos[1],pos[2]);
		NPC_SprintTo(strval(strtok(cmdtext,idx)),pos[0],pos[1],pos[2],0);
	    return 1;
	}
	if(!strcmp(cmd,"/weapon",true))
	{
	    new id = strval(strtok(cmdtext,idx)),
	        wi = strval(strtok(cmdtext,idx));
		SetNPCWeapon(id,wi);
	    return 1;
	}
	if(!strcmp(cmd,"/weapons",true))
	{
	    new wi = strval(strtok(cmdtext,idx));
     	for_npcs(i) SetNPCWeapon(i,wi);
	    return 1;
	}
	if(!strcmp(cmd,"/k",true))
	{
	    KillNPC(strval(strtok(cmdtext,idx)));
	    return 1;
	}
	if(!strcmp(cmd,"/s",true))
	{
	    for_npcs(i) SpawnNPC(i);
	    return 1;
	}
	if(!strcmp(cmd,"/d",true))
	{
	    new start = strval(strtok(cmdtext,idx)),
			end = strval(strtok(cmdtext,idx));
		for(new i = start;i <= end;i++) DestroyNPC(i);
	    return 1;
	}
	if(!strcmp(cmd,"/state",true))
	{
	    NPC_STATUS = strval(strtok(cmdtext,idx));
	    if(NPC_STATUS == 0)
	    {
	        for_npcs(i) StopNPC(i);
	    }
	    return 1;
	}
	return 0;
}

public JokeNPC()
{
    new Float:pos[3];
	switch(NPC_STATUS)
	{
	    case NPC_STATUS_WAIT: return;
	    case NPC_STATUS_WALK_TO_CLOSEST:
	    {
	        for_npcs(i)
			{
	        	GetPlayerPos(GetPlayerClosestPlayer(i),pos[0],pos[1],pos[2]);
			    NPC_WalkTo(i,(random(5) - random(5) + pos[0]),(random(5) - random(5) + pos[1]),pos[2],0);
			}
			return;
	    }
	    case NPC_STATUS_RUN_TO_CLOSEST:
	    {
	        for_npcs(i)
			{
	        	GetPlayerPos(GetPlayerClosestPlayer(i),pos[0],pos[1],pos[2]);
			    NPC_RunTo(i,(random(5) - random(5) + pos[0]),(random(5) - random(5) + pos[1]),pos[2],0);
			}
			return;
	    }
	    case NPC_STATUS_SPRINT_TO_CLOSEST:
	    {
	        for_npcs(i)
			{
	        	GetPlayerPos(GetPlayerClosestPlayer(i),pos[0],pos[1],pos[2]);
			    NPC_SprintTo(i,(random(5) - random(5) + pos[0]),(random(5) - random(5) + pos[1]),pos[2],0);
			}
			return;
	    }
	    case NPC_STATUS_LOOK_AT_CLOSEST:
	    {
	        for_npcs(i)
			{
	        	GetPlayerPos(GetPlayerClosestPlayer(i),pos[0],pos[1],pos[2]);
			    NPC_LookAt(i,pos[0],pos[1],pos[2]);
			}
			return;
	    }
	    case NPC_STATUS_AIM_AT_CLOSEST:
	    {
	        for_npcs(i)
			{
	        	GetPlayerPos(GetPlayerClosestPlayer(i),pos[0],pos[1],pos[2]);
			    NPC_AimAt(i,pos[0],pos[1],pos[2]);
			}
			return;
	    }
	    case NPC_STATUS_SHOT_AT_CLOSEST:
	    {
	        for_npcs(i)
			{
	        	GetPlayerPos(GetPlayerClosestPlayer(i),pos[0],pos[1],pos[2]);
			    NPC_ShootAt(i,pos[0],pos[1],pos[2]);
			}
			return;
	    }
	}
}

// --------------------------------------------------
// stocks
// --------------------------------------------------
stock IsValidSkin(skinid)
{
  if  (skinid < 0 || skinid > 299) return false;
  for (new i = 0; i < sizeof(badSkins); i++) {
      if (skinid == badSkins[i]) return false;
  }
  return true;
}

stock GetPlayerClosestPlayer(playerid)
{
	new pid = INVALID_PLAYER_ID,
		Float:distance = 69000.0,
		Float:tmp[7];
	GetPlayerPos(playerid,tmp[0],tmp[1],tmp[2]);
	for(new i = 0;i < MAX_PLAYERS;i++)
	{
		if(i == playerid) continue;
		if(!IsPlayerConnected(i)) continue;
		if(IsPlayerNPC(i)) continue;
		GetPlayerPos(i,tmp[3],tmp[4],tmp[5]);
		tmp[6] = GetDistanceBetweenCoords(tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
		if(distance < tmp[6]) continue;
		distance = tmp[6];
		pid = i;
	}
	return pid;
}

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
