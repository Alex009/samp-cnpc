/*
*	Created:			21.08.12
*	Author:				009
*	Last Modifed:		-
*	Description:		Controllable NPC's taxi filterscript
*/

main() {}

// --------------------------------------------------
// includes
// --------------------------------------------------
#include <a_samp>
#include <cnpc>
#include <cnpc_controller>

// --------------------------------------------------
// defines
// --------------------------------------------------
#define VERSION "0.1 beta"

#if !defined MAX_STRING
	#define MAX_STRING 128
#endif

#define MAX_TAXI    1

#define MESSAGES_COLOR 0xFFFFFFFF

// --------------------------------------------------
// enums
// --------------------------------------------------
enum taxi_info
{
	name[MAX_PLAYER_NAME],
	skin,
	Float:pos[4],
	cnpcid,
	vehicleid
};

// --------------------------------------------------
// news
// --------------------------------------------------
new taxi_spawn[MAX_TAXI][taxi_info] =
	{
	    {"First_taxi",61,{-2011.4387,-41.8610,35.1650,353.0915},-1,-1}
	};

// --------------------------------------------------
// forwards
// --------------------------------------------------


// --------------------------------------------------
// publics
// --------------------------------------------------
public OnFilterScriptInit()
{
	// подключаем ZMap
	zmap_Init("SAfull.hmap",ZMAP_MODE_CHACHE);
	// подключаем менеджер путей
	nodes_Init();
	// подключаем модуль обработки путей
	paths_Init();
	// создаём таксистов
	for(new i = 0;i < MAX_TAXI;i++)
	{
	    taxi_spawn[i][vehicleid] = CreateVehicle(420,taxi_spawn[i][pos][0],taxi_spawn[i][pos][1],taxi_spawn[i][pos][2],taxi_spawn[i][pos][3],-1,-1,0);
	    taxi_spawn[i][cnpcid] = FindLastFreeSlot();
	    CreateNPC(taxi_spawn[i][cnpcid],taxi_spawn[i][name]);
	    SetSpawnInfo(taxi_spawn[i][cnpcid],0,taxi_spawn[i][skin],taxi_spawn[i][pos][0],taxi_spawn[i][pos][1],taxi_spawn[i][pos][2],taxi_spawn[i][pos][3],0,0,0,0,0,0);
	    SpawnNPC(taxi_spawn[i][cnpcid]);
	    SetNPCPos(taxi_spawn[i][cnpcid],taxi_spawn[i][pos][0],taxi_spawn[i][pos][1],taxi_spawn[i][pos][2]);
	    SetNPCFacingAngle(taxi_spawn[i][cnpcid],taxi_spawn[i][pos][3]);
	    SetNPCVelocity(taxi_spawn[i][cnpcid],0.0,0.0,0.0);
	    PutNPCInVehicle(taxi_spawn[i][cnpcid],taxi_spawn[i][vehicleid],0);
	}
	
	print("Controllable NPC's taxi " VERSION " loaded.");
}

public OnFilterScriptExit()
{
	// отключаем модуль обработки путей
	paths_Exit();
	// отключаем менеджер путей
	nodes_Exit();
    // отключаем ZMap
	zmap_Exit();
	// удаляем таксистов
	for(new i = 0;i < MAX_TAXI;i++)
	{
		DestroyNPC(taxi_spawn[i][cnpcid]);
		DestroyVehicle(taxi_spawn[i][vehicleid]);
	}
	
	print("Controllable NPC's taxi " VERSION " unloaded.");
}

public OnPlayerConnect(playerid)
{

}

public OnPlayerDisconnect(playerid, reason)
{
    
}

public OnNPCGetDamage(npcid,playerid,Float:health_loss)
{
	return 1;
}

public OnNPCSpawn(npcid)
{
    controller_OnNPCSpawn(npcid);
}

public OnNPCMovingComplete(npcid)
{
    controller_OnNPCMovingComplete(npcid);
    
    if(!controller_IsMoving(npcid)) SetPlayerChatBubble(npcid,"Приехали",0xFFFF00FF,20.0,5000);
}

public OnNPCDeath(npcid,killerid,reason)
{
    controller_OnNPCDeath(npcid,killerid,reason);
}

public OnPathCalculated(pathid)
{
    paths_OnPathCalculated(pathid);
}

public OnCompleteCalcPathForNPC(npcid)
{
    controller_OnCompCalcPathForNPC(npcid);
}

public OnStartCalcPathForNPC(npcid)
{
    
}

public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
	if(!IsPlayerInAnyVehicle(playerid)) return;
	
	for(new i = 0;i < MAX_TAXI;i++)
	{
	    if(!IsPlayerInVehicle(playerid,taxi_spawn[i][vehicleid])) continue;
	    
	    controller_DriveToByNodes(taxi_spawn[i][cnpcid],fX,fY,fZ,20.0,0);
	}
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
