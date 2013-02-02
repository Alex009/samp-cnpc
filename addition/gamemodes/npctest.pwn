/*
*	Created:			12.04.10
*	Author:				009
*	Last Modifed:		06.09.12
*	Description:		Controllable NPC's test mode
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
#define NPC_COUNT 450

#if !defined MAX_STRING
	#define MAX_STRING 128
#endif

#define MESSAGES_COLOR 0xFFFFFFFF

#define DIALOG_NPC_ACTIONS_STRING "Core\nFoot controls\nAim controls\nVehicle controls\nPassanger controls\nPlayback controls\nOther controls"
#define DIALOG_NPC_CORE_STRING "Destroy\nKill\nSpawn\nPut in vehicle\nStop\nImpregnable"
#define DIALOG_NPC_FOOT_STRING "Set position\nGet position\nSet velocity\nGet velocity\nSet keys\nGet keys\nSet skin\nGet skin\nSet interior\nGet interior\nSet weapon\nGet weapon\nSet skill level\nGet skill level\nSet health\nGet health\nSet armour\nGet armour\nSet surfing\nGet surfing\nSet surfing vehicle\nGet surfing vehicle\nSet special action\nGet special action\nSet animation index\nGet animation index"
#define DIALOG_NPC_AIM_STRING "Set camera pos\nGet camera pos\nSet camera front vector\nGet camera front vector\nSet camera mode\nGet camera mode\nSet weapon state\nGet weapon state"
#define DIALOG_NPC_VEHICLE_STRING "Set siren\nGet siren\nSet trailer\nGet trailer"
#define DIALOG_NPC_PASSANGER_STRING "Set driveby\nGet driveby"
#define DIALOG_NPC_PLAYBACK_STRING "Start playback\nPause playback\nContinue playback\nStop playback"
#define DIALOG_NPC_OTHER_STRING "Walk to\nRun to\nSprint to\nDrive to\nLook to\nAim to\nShot to\nAnimation"

// --------------------------------------------------
// enums
// --------------------------------------------------
enum
{
    DIALOG_NPC_ACTIONS,
    DIALOG_NPC_CORE,
    DIALOG_NPC_FOOT,
    DIALOG_NPC_AIM,
    DIALOG_NPC_VEHICLE,
    DIALOG_NPC_PASSANGER,
    DIALOG_NPC_PLAYBACK,
    DIALOG_NPC_OTHER,
    DIALOG_NPC_PUTINVEH,
	DIALOG_NPC_FOOT_SETPOS,
	DIALOG_NPC_FOOT_SETVEL,
	DIALOG_NPC_FOOT_SETKEYS,
	DIALOG_NPC_FOOT_SETSKIN,
	DIALOG_NPC_FOOT_SETINTERIOR,
	DIALOG_NPC_FOOT_SETWEAPON,
	DIALOG_NPC_FOOT_SETSKILL,
	DIALOG_NPC_FOOT_GETSKILL,
	DIALOG_NPC_FOOT_SETHEALTH,
	DIALOG_NPC_FOOT_SETARMOUR,
	DIALOG_NPC_FOOT_SETSURF,
	DIALOG_NPC_FOOT_SETSURFV,
	DIALOG_NPC_FOOT_SETSA,
	DIALOG_NPC_FOOT_SETAI,
	DIALOG_NPC_AIM_SETPOS,
	DIALOG_NPC_AIM_SETFV,
	DIALOG_NPC_AIM_SETCM,
	DIALOG_NPC_AIM_SETWS,
	DIALOG_NPC_VEHICLE_SETTRAILER,
    DIALOG_NPC_PLAYBACK_START,
	DIALOG_NPC_OTHER_WALK,
	DIALOG_NPC_OTHER_RUN,
	DIALOG_NPC_OTHER_SPRINT,
	DIALOG_NPC_OTHER_DRIVE,
	DIALOG_NPC_OTHER_LOOK,
	DIALOG_NPC_OTHER_AIM,
	DIALOG_NPC_OTHER_SHOT,
	DIALOG_NPC_OTHER_ANIM
}

// --------------------------------------------------
// news
// --------------------------------------------------
new badSkins[] = {3, 4, 5, 6, 8, 42, 65, 74, 86, 119, 149, 208, 273, 289},
	strtmp[MAX_STRING],
	Float:floattmp[4],
	inttmp[3],
	ActionTo[MAX_PLAYERS];

// --------------------------------------------------
// publics
// --------------------------------------------------
public OnGameModeInit()
{
	// add class
	AddPlayerClass(1,-1657.8768,-165.0978,14.1484,308.8885,0,0,0,0,0,0);
	// create npcs
	new skin;
	for(new i = 0;i < NPC_COUNT;i++)
	{
	    new id = FindLastFreeSlot();
	    if(id == INVALID_PLAYER_ID) break;
	    
	    CreateNPC(id,"TestNPC");
	    // find valid skin
		do skin = random(300);
		while(!IsValidSkin(skin));
		// move coords
		floattmp[0] += 1.5;
		floattmp[1] += 1.5;
		// set spawn info
		SetSpawnInfo(id,0,skin,(-1657.8768 + floattmp[0]),(-165.0978 + floattmp[1]),14.1484,308.8885,0,0,0,0,0,0);
		SpawnNPC(id);
	}
	printf("Last free slot: %d",FindLastFreeSlot());
	print("NPCs loaded.");
}

public OnGameModeExit()
{
	for(new i = 0;i < MAX_PLAYERS;i++)
	{
	    if(IsValidNPC(i)) DestroyNPC(i);
	}
}

public OnNPCGetDamage(npcid,playerid,Float:health_loss,bodypart)
{
	printf("OnNPCGetDamage(%d,%d,%f,%d)",npcid,playerid,health_loss,bodypart);
	
	format(strtmp,sizeof(strtmp),"OnNPCGetDamage(%d,%d,%f,%d)",npcid,playerid,health_loss,bodypart);
	SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	return 1;
}

public OnRecordingPlaybackEnd(npcid,reason)
{
	printf("OnRecordingPlaybackEnd(%d,%d)",npcid,reason);
}

public OnNPCMovingComplete(npcid)
{
	new Float:tx,Float:ty,Float:tz;
	new Float:x,Float:y,Float:z;
	
	GetNPCPos(npcid,x,y,z);
	tx = GetPVarFloat(npcid,"tx");
	ty = GetPVarFloat(npcid,"ty");
	tz = GetPVarFloat(npcid,"tz");
	
	printf("OnNPCMovingComplete(%d) - [%f,%f,%f] [%f,%f,%f]",npcid,x,y,z,tx,ty,tz);
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
		GetPlayerPos(playerid,floattmp[0],floattmp[1],floattmp[2]);
		GetPlayerFacingAngle(playerid,floattmp[3]);
	    PutPlayerInVehicle(playerid,AddStaticVehicle(strval(strtok(cmdtext,idx)),floattmp[0],floattmp[1],floattmp[2],floattmp[3],0,0),0);
	    return 1;
	}
	if(!strcmp(cmd,"/tp",true))
	{
		new Float:pos[3];
		GetPlayerPos(strval(strtok(cmdtext,idx)),pos[0],pos[1],pos[2]);
	    SetPlayerPos(playerid,pos[0],pos[1],pos[2] + 2.0);
	    return 1;
	}
	if(!strcmp(cmd,"/npc",true))
	{
		ActionTo[playerid] = strval(strtok(cmdtext,idx));
		ShowPlayerDialog(playerid,DIALOG_NPC_ACTIONS,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_ACTIONS_STRING,"Continue","Cancel");
	    return 1;
	}
	if(!strcmp(cmd,"/zmap",true))
	{
	    new filename[20];
	    new mode;
	    filename = strtok(cmdtext,idx);
	    mode = strval(strtok(cmdtext,idx));
	    zmap_Init(filename,mode);
	    SendClientMessage(playerid,MESSAGES_COLOR,"zmap loaded");
	    return 1;
	}
	return 0;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	new idx;
	switch(dialogid)
	{
	    case DIALOG_NPC_ACTIONS:
	    {
	        if(!response) return 0;
	        
	        switch(listitem)
	        {
	            // Core
	            case 0: return ShowPlayerDialog(playerid,DIALOG_NPC_CORE,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_CORE_STRING,"Continue","Cancel");
	            // Foot
	            case 1: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	            // Aim
	            case 2: return ShowPlayerDialog(playerid,DIALOG_NPC_AIM,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_AIM_STRING,"Continue","Cancel");
	            // Vehicle
	            case 3: return ShowPlayerDialog(playerid,DIALOG_NPC_VEHICLE,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_VEHICLE_STRING,"Continue","Cancel");
	            // Passanger
	            case 4: return ShowPlayerDialog(playerid,DIALOG_NPC_PASSANGER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_PASSANGER_STRING,"Continue","Cancel");
 	            // Playback
	            case 5: return ShowPlayerDialog(playerid,DIALOG_NPC_PLAYBACK,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_PLAYBACK_STRING,"Continue","Cancel");
	            // Other
	            case 6: return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");
	        }
	    }
	    case DIALOG_NPC_CORE:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_ACTIONS,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_ACTIONS_STRING,"Continue","Cancel");

	        switch(listitem)
	        {
	            // Destroy
	            case 0: return DestroyNPC(ActionTo[playerid]);
	            // Kill
	            case 1: return KillNPC(ActionTo[playerid]);
	            // Spawn
	            case 2: return SpawnNPC(ActionTo[playerid]);
	            // Put in vehicle
	            case 3: return ShowPlayerDialog(playerid,DIALOG_NPC_PUTINVEH,DIALOG_STYLE_INPUT,"Enter data","Enter vehicleid and seat (example: 1 0)","Continue","Cancel");
	            // Stop
	            case 4: return StopNPC(ActionTo[playerid]);
	            // Impregnable
	            case 5:
				{
					if(IsNPCImpregnable(ActionTo[playerid])) SetNPCImpregnable(ActionTo[playerid],0);
					else SetNPCImpregnable(ActionTo[playerid],1);
					return 1;
				}
	        }
	    }
	    case DIALOG_NPC_FOOT:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_ACTIONS,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_ACTIONS_STRING,"Continue","Cancel");

	        switch(listitem)
	        {
	            // Set position
	            case 0: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT_SETPOS,DIALOG_STYLE_INPUT,"Enter data","Enter position (example: 0.0 0.0 0.0) or null for get your coords","Continue","Cancel");
	            // Get position
	            case 1:
	            {
	                GetNPCPos(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2]);
	                format(strtmp,sizeof(strtmp),"Result: %f,%f,%f",floattmp[0],floattmp[1],floattmp[2]);
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	            }
	            // Set velocity
	            case 2: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT_SETVEL,DIALOG_STYLE_INPUT,"Enter data","Enter vector (example: 0.0 0.0 0.0)","Continue","Cancel");
	            // Get velocity
	            case 3:
	            {
	                GetNPCVelocity(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2]);
	                format(strtmp,sizeof(strtmp),"Result: %f,%f,%f",floattmp[0],floattmp[1],floattmp[2]);
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	            }
	            // Set keys
	            case 4: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT_SETKEYS,DIALOG_STYLE_INPUT,"Enter data","Enter keys (example: 0 0 0)","Continue","Cancel");
	            // Get keys
	            case 5:
	            {
	                GetNPCKeys(ActionTo[playerid],inttmp[0],inttmp[1],inttmp[2]);
	                format(strtmp,sizeof(strtmp),"Result: %d,%d,%d",inttmp[0],inttmp[1],inttmp[2]);
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	            }
	            // Set skin
	            case 6: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT_SETSKIN,DIALOG_STYLE_INPUT,"Enter data","Enter skin id (example: 0)","Continue","Cancel");
	            // Get skin
	            case 7:
	            {
	                format(strtmp,sizeof(strtmp),"Result: %d",GetNPCSkin(ActionTo[playerid]));
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	            }
	            // Set interior
	            case 8: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT_SETINTERIOR,DIALOG_STYLE_INPUT,"Enter data","Enter interior id (example: 0)","Continue","Cancel");
	            // Get interior
	            case 9:
	            {
	                format(strtmp,sizeof(strtmp),"Result: %d",GetNPCInterior(ActionTo[playerid]));
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	            }
	            // Set weapon
	            case 10: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT_SETWEAPON,DIALOG_STYLE_INPUT,"Enter data","Enter weapon id (example: 0)","Continue","Cancel");
	            // Get weapon
	            case 11:
	            {
	                format(strtmp,sizeof(strtmp),"Result: %d",GetNPCWeapon(ActionTo[playerid]));
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	            }
	            // Set skill level
	            case 12: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT_SETSKILL,DIALOG_STYLE_INPUT,"Enter data","Enter weapon type and skill (example: 0 999)","Continue","Cancel");
	            // Get skill level
	            case 13: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT_GETSKILL,DIALOG_STYLE_INPUT,"Enter data","Enter weapon type (example: 0)","Continue","Cancel");
	            // Set health
	            case 14: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT_SETHEALTH,DIALOG_STYLE_INPUT,"Enter data","Enter health (example: 100.0)","Continue","Cancel");
	            // Get health
	            case 15:
	            {
	                format(strtmp,sizeof(strtmp),"Result: %f",GetNPCHealth(ActionTo[playerid]));
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	            }
	            // Set armour
	            case 16: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT_SETARMOUR,DIALOG_STYLE_INPUT,"Enter data","Enter armour (example: 100.0)","Continue","Cancel");
	            // Get armour
	            case 17:
	            {
	                format(strtmp,sizeof(strtmp),"Result: %f",GetNPCArmour(ActionTo[playerid]));
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	            }
	            // Set surfing
	            case 18: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT_SETSURF,DIALOG_STYLE_INPUT,"Enter data","Enter surfing (example: 0.0 0.0 0.0)","Continue","Cancel");
	            // Get surfing
	            case 19:
	            {
	                GetNPCSurfing(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2]);
	                format(strtmp,sizeof(strtmp),"Result: %f,%f,%f",floattmp[0],floattmp[1],floattmp[2]);
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	            }
	            // Set surfing vehicle
	            case 20: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT_SETSURFV,DIALOG_STYLE_INPUT,"Enter data","Enter surfing vehicle id (example: 1)","Continue","Cancel");
	            // Get surfing vehicle
	            case 21:
	            {
	                format(strtmp,sizeof(strtmp),"Result: %d",GetNPCSurfingVehicle(ActionTo[playerid]));
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	            }
	            // Set special action
	            case 22: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT_SETSA,DIALOG_STYLE_INPUT,"Enter data","Enter action id (example: 0)","Continue","Cancel");
	            // Get special action
	            case 23:
	            {
	                format(strtmp,sizeof(strtmp),"Result: %d",GetNPCSpecialAction(ActionTo[playerid]));
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	            }
	            // Set animation index
	            case 24: return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT_SETAI,DIALOG_STYLE_INPUT,"Enter data","Enter animation index (example: 0)","Continue","Cancel");
	            // Get animation index
	            case 25:
	            {
	                format(strtmp,sizeof(strtmp),"Result: %d",GetNPCAnimationIndex(ActionTo[playerid]));
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	            }
	        }
	    }
	    case DIALOG_NPC_AIM:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_ACTIONS,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_ACTIONS_STRING,"Continue","Cancel");

	        switch(listitem)
	        {
	            // Set camera pos
	            case 0: return ShowPlayerDialog(playerid,DIALOG_NPC_AIM_SETPOS,DIALOG_STYLE_INPUT,"Enter data","Enter position (example: 0.0 0.0 0.0)","Continue","Cancel");
	            // Get camera pos
	            case 1:
	            {
	                GetNPCCameraPos(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2]);
	                format(strtmp,sizeof(strtmp),"Result: %f,%f,%f",floattmp[0],floattmp[1],floattmp[2]);
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_AIM,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_AIM_STRING,"Continue","Cancel");
	            }
	            // Set camera front vector
	            case 2: return ShowPlayerDialog(playerid,DIALOG_NPC_AIM_SETFV,DIALOG_STYLE_INPUT,"Enter data","Enter vector (example: 0.0 0.0 0.0)","Continue","Cancel");
	            // Get camera front vector
	            case 3:
	            {
	                GetNPCCameraFrontVector(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2]);
	                format(strtmp,sizeof(strtmp),"Result: %f,%f,%f",floattmp[0],floattmp[1],floattmp[2]);
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_AIM,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_AIM_STRING,"Continue","Cancel");
	            }
	            // Set camera mode
	            case 4: return ShowPlayerDialog(playerid,DIALOG_NPC_AIM_SETCM,DIALOG_STYLE_INPUT,"Enter data","Enter camera mode (example: 7)","Continue","Cancel");
	            // Get camera mode
	            case 5:
	            {
	                format(strtmp,sizeof(strtmp),"Result: %d",GetNPCCameraMode(ActionTo[playerid]));
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_AIM,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_AIM_STRING,"Continue","Cancel");
	            }
	            // Set weapon state
	            case 6: return ShowPlayerDialog(playerid,DIALOG_NPC_AIM_SETWS,DIALOG_STYLE_INPUT,"Enter data","Enter weapon state (example: 53)","Continue","Cancel");
	            // Get weapon state
	            case 7:
	            {
	                format(strtmp,sizeof(strtmp),"Result: %d",GetNPCWeaponState(ActionTo[playerid]));
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_AIM,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_AIM_STRING,"Continue","Cancel");
	            }
	        }
	    }
	    case DIALOG_NPC_VEHICLE:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_ACTIONS,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_ACTIONS_STRING,"Continue","Cancel");

	        switch(listitem)
	        {
	            // Set siren
	            case 0:
				{
					if(GetNPCVehicleSiren(ActionTo[playerid])) SetNPCVehicleSiren(ActionTo[playerid],0);
					else SetNPCVehicleSiren(ActionTo[playerid],1);
					return ShowPlayerDialog(playerid,DIALOG_NPC_VEHICLE,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_VEHICLE_STRING,"Continue","Cancel");
				}
	            // Get siren
	            case 1:
	            {
	                format(strtmp,sizeof(strtmp),"Result: %d",GetNPCVehicleSiren(ActionTo[playerid]));
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_VEHICLE,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_VEHICLE_STRING,"Continue","Cancel");
	            }
	            // Set trailer
	            case 2: return ShowPlayerDialog(playerid,DIALOG_NPC_VEHICLE_SETTRAILER,DIALOG_STYLE_INPUT,"Enter data","Enter weapon id (example: 0)","Continue","Cancel");
	            // Get trailer
	            case 3:
	            {
	                format(strtmp,sizeof(strtmp),"Result: %d",GetNPCVehicleTrailer(ActionTo[playerid]));
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_VEHICLE,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_VEHICLE_STRING,"Continue","Cancel");
	            }
	        }
	    }
	    case DIALOG_NPC_PASSANGER:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_ACTIONS,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_ACTIONS_STRING,"Continue","Cancel");

	        switch(listitem)
	        {
	            // Set siren
	            case 0:
				{
					if(GetNPCPassangerDriveBy(ActionTo[playerid])) SetNPCPassangerDriveBy(ActionTo[playerid],0);
					else SetNPCPassangerDriveBy(ActionTo[playerid],1);
					return ShowPlayerDialog(playerid,DIALOG_NPC_PASSANGER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_PASSANGER_STRING,"Continue","Cancel");
				}
	            // Get siren
	            case 1:
	            {
	                format(strtmp,sizeof(strtmp),"Result: %d",GetNPCPassangerDriveBy(ActionTo[playerid]));
	                SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_PASSANGER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_PASSANGER_STRING,"Continue","Cancel");
	            }
	        }
	    }
	    case DIALOG_NPC_PLAYBACK:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_ACTIONS,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_ACTIONS_STRING,"Continue","Cancel");

	        switch(listitem)
	        {
	            // start
	            case 0: return ShowPlayerDialog(playerid,DIALOG_NPC_PLAYBACK_START,DIALOG_STYLE_INPUT,"Enter data","Enter name of rec (example: testpb)","Continue","Cancel");
	            // pause
	            case 1:
	            {
	                PauseRecordingPlayback(ActionTo[playerid]);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_PLAYBACK,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_PLAYBACK_STRING,"Continue","Cancel");
	            }
	            // continue
	            case 2:
	            {
	                ContinueRecordingPlayback(ActionTo[playerid]);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_PLAYBACK,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_PLAYBACK_STRING,"Continue","Cancel");
	            }
	            // stop
	            case 3:
	            {
	                StopRecordingPlayback(ActionTo[playerid]);
	                return ShowPlayerDialog(playerid,DIALOG_NPC_PLAYBACK,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_PLAYBACK_STRING,"Continue","Cancel");
	            }
	        }
	    }
	    case DIALOG_NPC_OTHER:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_ACTIONS,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_ACTIONS_STRING,"Continue","Cancel");

	        switch(listitem)
	        {
	            // walk
	            case 0: return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER_WALK,DIALOG_STYLE_INPUT,"Enter data","Enter \"is z map\" and coords (example: 0.0 0.0 0.0) or only \"is z map\" for get your coords","Continue","Cancel");
	            // run
	            case 1: return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER_RUN,DIALOG_STYLE_INPUT,"Enter data","Enter \"is z map\" and coords (example: 0.0 0.0 0.0) or only \"is z map\" for get your coords","Continue","Cancel");
	            // sprint
	            case 2: return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER_SPRINT,DIALOG_STYLE_INPUT,"Enter data","Enter \"is z map\" and coords (example: 0.0 0.0 0.0) or only \"is z map\" for get your coords","Continue","Cancel");
	            // drive
	            case 3: return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER_DRIVE,DIALOG_STYLE_INPUT,"Enter data","Enter speed , \"is z map\" and coords (example: 0.0 0.0 0.0) or only speed , \"is z map\" for get your coords","Continue","Cancel");
	            // look
	            case 4: return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER_LOOK,DIALOG_STYLE_INPUT,"Enter data","Enter coords (example: 0.0 0.0 0.0) or null for get your coords","Continue","Cancel");
	            // aim
	            case 5: return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER_AIM,DIALOG_STYLE_INPUT,"Enter data","Enter coords (example: 0.0 0.0 0.0) or null for get your coords","Continue","Cancel");
	            // shot
	            case 6: return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER_SHOT,DIALOG_STYLE_INPUT,"Enter data","Enter coords (example: 0.0 0.0 0.0) or null for get your coords","Continue","Cancel");
	            // anim
	            case 7: return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER_ANIM,DIALOG_STYLE_INPUT,"Enter data","Enter lib and name","Continue","Cancel");
	        }
	    }
	    case DIALOG_NPC_PUTINVEH:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_CORE,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_CORE_STRING,"Continue","Cancel");
	        
	        for(new i = 0;i < 2;i++) inttmp[i] = strval(strtok(inputtext,idx));
            PutNPCInVehicle(ActionTo[playerid],inttmp[0],inttmp[1]);
            return ShowPlayerDialog(playerid,DIALOG_NPC_CORE,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_CORE_STRING,"Continue","Cancel");
	    }
	    // foot addition
	    case DIALOG_NPC_FOOT_SETPOS:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");

			if(!inputtext[0]) GetPlayerPos(playerid,floattmp[0],floattmp[1],floattmp[2]);
	        else
			{
				for(new i = 0;i < 3;i++) floattmp[i] = floatstr(strtok(inputtext,idx));
			}
	        SetNPCPos(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_FOOT_SETVEL:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");

			for(new i = 0;i < 3;i++) floattmp[i] = floatstr(strtok(inputtext,idx));
	        SetNPCVelocity(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_FOOT_SETKEYS:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");

			for(new i = 0;i < 3;i++) inttmp[i] = strval(strtok(inputtext,idx));
	        SetNPCKeys(ActionTo[playerid],inttmp[0],inttmp[1],inttmp[2]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_FOOT_SETSKIN:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");

			inttmp[0] = strval(strtok(inputtext,idx));
	        SetNPCSkin(ActionTo[playerid],inttmp[0]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_FOOT_SETINTERIOR:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");

			inttmp[0] = strval(strtok(inputtext,idx));
	        SetNPCInterior(ActionTo[playerid],inttmp[0]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_FOOT_SETWEAPON:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");

			inttmp[0] = strval(strtok(inputtext,idx));
	        SetNPCWeapon(ActionTo[playerid],inttmp[0]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_FOOT_SETSKILL:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");

			for(new i = 0;i < 2;i++) inttmp[i] = strval(strtok(inputtext,idx));
	        SetNPCWeaponSkillLevel(ActionTo[playerid],inttmp[0],inttmp[1]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_FOOT_GETSKILL:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");

			inttmp[0] = strval(strtok(inputtext,idx));
			format(strtmp,sizeof(strtmp),"Result: %d",GetNPCWeaponSkillLevel(ActionTo[playerid],inttmp[0]));
	        SendClientMessage(playerid,MESSAGES_COLOR,strtmp);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_FOOT_SETHEALTH:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");

			floattmp[0] = floatstr(strtok(inputtext,idx));
	        SetNPCHealth(ActionTo[playerid],floattmp[0]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_FOOT_SETARMOUR:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");

			floattmp[0] = floatstr(strtok(inputtext,idx));
	        SetNPCArmour(ActionTo[playerid],floattmp[0]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_FOOT_SETSURF:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");

			for(new i = 0;i < 3;i++) floattmp[i] = floatstr(strtok(inputtext,idx));
	        SetNPCSurfing(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_FOOT_SETSURFV:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");

			inttmp[0] = strval(strtok(inputtext,idx));
	        SetNPCSurfingVehicle(ActionTo[playerid],inttmp[0]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_FOOT_SETSA:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");

			inttmp[0] = strval(strtok(inputtext,idx));
	        SetNPCSpecialAction(ActionTo[playerid],inttmp[0]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_FOOT_SETAI:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");

			inttmp[0] = strval(strtok(inputtext,idx));
	        SetNPCAnimationIndex(ActionTo[playerid],inttmp[0]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_FOOT,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_FOOT_STRING,"Continue","Cancel");
	    }
	    // aim addition
	    case DIALOG_NPC_AIM_SETPOS:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_AIM,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_AIM_STRING,"Continue","Cancel");

			for(new i = 0;i < 3;i++) floattmp[i] = floatstr(strtok(inputtext,idx));
	        SetNPCCameraPos(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_AIM,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_AIM_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_AIM_SETFV:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_AIM,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_AIM_STRING,"Continue","Cancel");

			for(new i = 0;i < 3;i++) floattmp[i] = floatstr(strtok(inputtext,idx));
	        SetNPCCameraFrontVector(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_AIM,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_AIM_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_AIM_SETCM:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_AIM,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_AIM_STRING,"Continue","Cancel");

			inttmp[0] = strval(strtok(inputtext,idx));
	        SetNPCCameraMode(ActionTo[playerid],inttmp[0]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_AIM,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_AIM_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_AIM_SETWS:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_AIM,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_AIM_STRING,"Continue","Cancel");

			inttmp[0] = strval(strtok(inputtext,idx));
	        SetNPCWeaponState(ActionTo[playerid],inttmp[0]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_AIM,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_AIM_STRING,"Continue","Cancel");
	    }
	    // vehicle addition
	    case DIALOG_NPC_VEHICLE_SETTRAILER:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_VEHICLE,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_VEHICLE_STRING,"Continue","Cancel");

			inttmp[0] = strval(strtok(inputtext,idx));
	        SetNPCVehicleTrailer(ActionTo[playerid],inttmp[0]);
	        return ShowPlayerDialog(playerid,DIALOG_NPC_VEHICLE,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_VEHICLE_STRING,"Continue","Cancel");
	    }
	    // playback addition
	    case DIALOG_NPC_PLAYBACK_START:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_PLAYBACK,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_PLAYBACK_STRING,"Continue","Cancel");

	        StartRecordingPlayback(ActionTo[playerid],inputtext);
            return ShowPlayerDialog(playerid,DIALOG_NPC_PLAYBACK,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_PLAYBACK_STRING,"Continue","Cancel");
	    }
	    // other addition
	    case DIALOG_NPC_OTHER_WALK:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");

            inttmp[0] = strval(strtok(inputtext,idx));
			if(!inputtext[idx]) GetPlayerPos(playerid,floattmp[0],floattmp[1],floattmp[2]);
	        else
			{
				for(new i = 0;i < 3;i++) floattmp[i] = floatstr(strtok(inputtext,idx));
			}
			SetPVarFloat(ActionTo[playerid],"tx",floattmp[0]);
			SetPVarFloat(ActionTo[playerid],"ty",floattmp[1]);
			SetPVarFloat(ActionTo[playerid],"tz",floattmp[2]);
			NPC_WalkTo(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2],inttmp[0]);
            return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_OTHER_RUN:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");

            inttmp[0] = strval(strtok(inputtext,idx));
			if(!inputtext[idx]) GetPlayerPos(playerid,floattmp[0],floattmp[1],floattmp[2]);
	        else
			{
				for(new i = 0;i < 3;i++) floattmp[i] = floatstr(strtok(inputtext,idx));
			}
			SetPVarFloat(ActionTo[playerid],"tx",floattmp[0]);
			SetPVarFloat(ActionTo[playerid],"ty",floattmp[1]);
			SetPVarFloat(ActionTo[playerid],"tz",floattmp[2]);
			NPC_RunTo(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2],inttmp[0]);
            return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_OTHER_SPRINT:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");

            inttmp[0] = strval(strtok(inputtext,idx));
			if(!inputtext[idx]) GetPlayerPos(playerid,floattmp[0],floattmp[1],floattmp[2]);
	        else
			{
				for(new i = 0;i < 3;i++) floattmp[i] = floatstr(strtok(inputtext,idx));
			}
			SetPVarFloat(ActionTo[playerid],"tx",floattmp[0]);
			SetPVarFloat(ActionTo[playerid],"ty",floattmp[1]);
			SetPVarFloat(ActionTo[playerid],"tz",floattmp[2]);
			NPC_SprintTo(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2],inttmp[0]);
            return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_OTHER_DRIVE:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");

            floattmp[0] = floatstr(strtok(inputtext,idx));
            inttmp[0] = strval(strtok(inputtext,idx));
			if(!inputtext[idx])
			{
				GetPlayerPos(playerid,floattmp[1],floattmp[2],floattmp[3]);
			}
	        else
			{
				for(new i = 1;i < 4;i++) floattmp[i] = floatstr(strtok(inputtext,idx));
			}
			SetPVarFloat(ActionTo[playerid],"tx",floattmp[0]);
			SetPVarFloat(ActionTo[playerid],"ty",floattmp[1]);
			SetPVarFloat(ActionTo[playerid],"tz",floattmp[2]);
			NPC_DriveTo(ActionTo[playerid],floattmp[1],floattmp[2],floattmp[3],floattmp[0],inttmp[0]);
            return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_OTHER_LOOK:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");

			if(!inputtext[0]) GetPlayerPos(playerid,floattmp[0],floattmp[1],floattmp[2]);
	        else
			{
				for(new i = 0;i < 3;i++) floattmp[i] = floatstr(strtok(inputtext,idx));
			}
			NPC_LookAt(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2]);
            return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_OTHER_AIM:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");

			if(!inputtext[0]) GetPlayerPos(playerid,floattmp[0],floattmp[1],floattmp[2]);
	        else
			{
				for(new i = 0;i < 3;i++) floattmp[i] = floatstr(strtok(inputtext,idx));
			}
			NPC_AimAt(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2]);
            return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_OTHER_SHOT:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");

			if(!inputtext[0]) GetPlayerPos(playerid,floattmp[0],floattmp[1],floattmp[2]);
	        else
			{
				for(new i = 0;i < 3;i++) floattmp[i] = floatstr(strtok(inputtext,idx));
			}
			NPC_ShootAt(ActionTo[playerid],floattmp[0],floattmp[1],floattmp[2]);
            return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");
	    }
	    case DIALOG_NPC_OTHER_ANIM:
	    {
	        if(!response) return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");

            strmid(strtmp,strtok(inputtext,idx),0,20);
			ApplyAnimation(ActionTo[playerid],strtmp,strtok(inputtext,idx),4.1,0,1,1,1,1);
            return ShowPlayerDialog(playerid,DIALOG_NPC_OTHER,DIALOG_STYLE_LIST,"Select action",DIALOG_NPC_OTHER_STRING,"Continue","Cancel");
	    }
	}
	return 0;
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
