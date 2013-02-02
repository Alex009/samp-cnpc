/*
*	Created:			20.08.12
*	Author:				009 & OKStyle
*	Last Modifed:		06.09.12
*	Description:		Controllable NPC's missions filterscript
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
#define VERSION             "0.1 beta"

#if !defined MAX_STRING
	#define MAX_STRING 128
#endif

#define MISSIONS_COUNT 3
#define MESSAGES_COLOR 0xFFFFFFFF

// --------------------------------------------------
// enums
// --------------------------------------------------
enum MissionInfo
{
	name[64],			// �������� ������
	pickupmodel,        // id ������ ������
	Float:start[3],		// ���������� ������ ������
	request,			// id ������ ������� ������ ���� �������� ��� ������� ����
	pickup				// id ���������� �� ������ ������
};
enum PlayerInfo
{
	progress[MISSIONS_COUNT],	// ���������� ������ (����� - 1 ��� 0)
	current_mission,			// ������� ������
	current_step,				// ������� ���� ������
	iscutscene,					// ���� - ������� ���� ��� ��������?
	timer						// ������, ��������� ��� ���������
};


// --------------------------------------------------
// news
// --------------------------------------------------
new strtmp[MAX_STRING],
	missions[MISSIONS_COUNT][MissionInfo] = {
		{"Let's Rock! Part 1",1559,{1723.5537,-1721.2167,13.5466},-1,-1},
		{"Let's Rock! Part 2",1559,{1720.5525,-1721.3824,13.5462},-1,-1},
		{"Case delivery",1559,{2069.9470,-1766.6082,13.5621},-1,-1}
	},
	players[MAX_PLAYERS][PlayerInfo],
	Text:Letterbox[2],
	player_mission_data[MAX_PLAYERS][MISSIONS_COUNT][20];
new Float:ftmp[6];

// --------------------------------------------------
// forwards
// --------------------------------------------------
forward Mission_CutsceneStep(playerid);
forward Mission_InProgressStep(playerid);

// --------------------------------------------------
// publics
// --------------------------------------------------
public OnFilterScriptInit()
{
	// ���������� ZMap
	zmap_Init("SAfull.hmap",ZMAP_MODE_CHACHE);
	// ���������� �������� �����
	nodes_Init();
	// ���������� ������ ��������� �����
	paths_Init();
	// ��������� ������ ������ ������
	for(new i = 0;i < MISSIONS_COUNT;i++)
	{
		missions[i][pickup] = CreatePickup(missions[i][pickupmodel],1,missions[i][start][0],missions[i][start][1],missions[i][start][2]);
	}
	// ������� TextDraw
	Letterbox[0] = TextDrawCreate(0.0,0.0,"_");
	TextDrawUseBox(Letterbox[0],1);
	TextDrawBoxColor(Letterbox[0],0x000000ff);
	TextDrawLetterSize(Letterbox[0],0.0,12.2);
	Letterbox[1] = TextDrawCreate(0.0,480.0,"_");
	TextDrawUseBox(Letterbox[1],1);
	TextDrawBoxColor(Letterbox[1],0x000000ff);
	TextDrawLetterSize(Letterbox[1],0.0,-17.0);
	// ��� ������ ������� ������ �������� (���� ������ �������� �� ����� ������)
	for(new playerid = 0;playerid < MAX_PLAYERS;playerid++)
	{
	    if(!IsPlayerConnected(playerid)) continue;
	    
	    OnPlayerConnect(playerid);
	}
	
	print("Controllable NPC's missions " VERSION " loaded.");
}

public OnFilterScriptExit()
{
    // ��������� ������ ��������� �����
	paths_Exit();
	// ��������� �������� �����
	nodes_Exit();
    // ��������� ZMap
	zmap_Exit();
	// ������� ������ ������ ������
	for(new i = 0;i < MISSIONS_COUNT;i++) DestroyPickup(missions[i][pickup]);
	// ������� TextDraw
	TextDrawDestroy(Letterbox[0]);
	TextDrawDestroy(Letterbox[1]);
	// ��� ������ ������� ������ �������� (���� ������ �������� �� ����� ������)
	for(new playerid = 0;playerid < MAX_PLAYERS;playerid++)
	{
	    if(!IsPlayerConnected(playerid)) continue;

	    OnPlayerDisconnect(playerid,0);
	}
	
	print("Controllable NPC's missions " VERSION " unloaded.");
}

public OnPlayerConnect(playerid)
{
	for(new i = 0;i < MISSIONS_COUNT;i++) players[playerid][progress][i] = 0;
	players[playerid][current_mission] = -1;
	players[playerid][current_step] = -1;
	players[playerid][timer] = -1;
}

public OnPlayerDisconnect(playerid, reason)
{
    for(new i = 0;i < MISSIONS_COUNT;i++) players[playerid][progress][i] = 0;
	Mission_Clear(playerid);
}

public OnPlayerUpdate(playerid)
{
    controller_OnPlayerUpdate(playerid);
    return 1;
}

public OnPlayerPickUpPickup(playerid, pickupid)
{
	// ���� ������ ����� ������
	for(new i = 0;i < sizeof(missions);i++)
	{
		if(missions[i][pickup] == pickupid)
		{
			// ������ �������, ���������
			Mission_Start(i,playerid);
			// ������, �������� ����
			break;
		}
	}
	return 1;
}

public OnNPCGetDamage(npcid,playerid,Float:health_loss)
{

	return 1;
}

public OnPathCalculated(pathid)
{
    paths_OnPathCalculated(pathid);
}

public OnCompleteCalcPathForNPC(npcid)
{
    controller_OnCompCalcPathForNPC(npcid);
}

public OnNPCMovingComplete(npcid)
{
	controller_OnNPCMovingComplete(npcid);
}

public OnNPCSpawn(npcid)
{
    controller_OnNPCSpawn(npcid);
}

public OnNPCDeath(npcid,killerid,reason)
{
    controller_OnNPCDeath(npcid,killerid,reason);
}

public OnStartCalcPathForNPC(npcid)
{

}

public OnPlayerCommandText(playerid, cmdtext[])
{
	new cmd[20],
		idx;

	cmd = strtok(cmdtext,idx);

	if(!strcmp(cmd,"/mission",true))
	{
	    new i = strval(strtok(cmdtext,idx));
	    SetPlayerPos(playerid,missions[i][start][0],missions[i][start][1],missions[i][start][2]);
	    return 1;
	}
	return 0;
}

public Mission_CutsceneStep(playerid)
{
	// ��������� � �������� �� �����, ���� ��� (��������� ������ ><)
	if(!players[playerid][iscutscene]) return printf("[ERROR - missions.amx] Mission_CutsceneStep called for player not in cutscene");
	// ���������� ������� ������
	new missionid = players[playerid][current_mission];
	switch(missionid)
	{
		case 0: // Let's Rock! Part 1
		{
			// ���������� ������� ���� ��������
			switch(players[playerid][current_step])
			{
				case 0: // ������ ������
				{
					SetPlayerPos(playerid,1723.5537,-1721.2167,13.5466);
					SetPlayerFacingAngle(playerid,89.5532);
				    TogglePlayerControllable(playerid,false);
					TextDrawShowForPlayer(playerid,Letterbox[0]);
					TextDrawShowForPlayer(playerid,Letterbox[1]);
					SetPlayerSpecialAction(playerid,SPECIAL_ACTION_USECELLPHONE);

					SendClientMessage(playerid,0x00FF00FF,"�������� �� ����� ���������, ��� ��� ����� ������� � ������� ��������� �� ���������");
					SendClientMessage(playerid,0x00FF00FF,"�� ������ ������� ���� �� ���������� �� ����� �������, ��� ������ ������� ���� - ����");
					
					players[playerid][current_step]++;
					players[playerid][timer] = SetTimerEx("Mission_CutsceneStep",10000,0,"d",playerid);
				}
				case 1: // ����� ������
				{
				    SetPlayerSpecialAction(playerid,SPECIAL_ACTION_STOPUSECELLPHONE);
                    SetCameraBehindPlayer(playerid);
					TextDrawHideForPlayer(playerid,Letterbox[0]);
					TextDrawHideForPlayer(playerid,Letterbox[1]);
					TogglePlayerControllable(playerid,true);
				
					// ������������� ������ ��� ������ ������
					players[playerid][current_step] = 0;
					players[playerid][iscutscene] = 0;
					// ��������� ������
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",0,0,"d",playerid);
				}
			}
		}
		case 1: // Let's Rock! Part 2
		{
		    // ���������� ������� ���� ��������
			switch(players[playerid][current_step])
			{
				case 0: // ������ ������
				{
					SetPlayerPos(playerid,1720.5525,-1721.3824,13.5462);
					SetPlayerFacingAngle(playerid,272.0335);
				    TogglePlayerControllable(playerid,false);
					TextDrawShowForPlayer(playerid,Letterbox[0]);
					TextDrawShowForPlayer(playerid,Letterbox[1]);
					SetPlayerSpecialAction(playerid,SPECIAL_ACTION_USECELLPHONE);

					SendClientMessage(playerid,0x00FF00FF,"� ������� ��� �� ���� �� ���� ����������, ������ ���� �� ��� �� ������ �� �������");
					SendClientMessage(playerid,0x00FF00FF,"������ �� ����� ������� ����� ������� ������� ���, ��� ��������� �");
					SendClientMessage(playerid,0x00FF00FF,"����� ��������, �� ��� ��� ��������, ��� ��� �� ��������� �� ������ ����������");
					SendClientMessage(playerid,0x00FF00FF,"����� ������� ��� ������ �� ���� ��� ��� ��������� �� ���");

					players[playerid][current_step]++;
					players[playerid][timer] = SetTimerEx("Mission_CutsceneStep",10000,0,"d",playerid);
				}
				case 1: // ����� ������
				{
				    SetPlayerSpecialAction(playerid,SPECIAL_ACTION_STOPUSECELLPHONE);
                    SetCameraBehindPlayer(playerid);
					TextDrawHideForPlayer(playerid,Letterbox[0]);
					TextDrawHideForPlayer(playerid,Letterbox[1]);
					TogglePlayerControllable(playerid,true);

					// ������������� ������ ��� ������ ������
					players[playerid][current_step] = 0;
					players[playerid][iscutscene] = 0;
					// ��������� ������
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",0,0,"d",playerid);
				}
			}
		}
		case 2: // Case delivery
		{
		    // ���������� ������� ���� ��������
			switch(players[playerid][current_step])
			{
				case 0: // ������ ������
				{
				    SetPlayerPos(playerid,2069.9470,-1766.6082,13.5621);
                    SetPlayerFacingAngle(playerid,89.4755);
                    SetCameraBehindPlayer(playerid);
				    TogglePlayerControllable(playerid,false);
					TextDrawShowForPlayer(playerid,Letterbox[0]);
					TextDrawShowForPlayer(playerid,Letterbox[1]);
					SetPlayerSpecialAction(playerid,SPECIAL_ACTION_USECELLPHONE);
					
					SendClientMessage(playerid,0x00FF00FF,"������, ���� ���� �������� ���� � ��������, �� ������ ������� ���");
					SendClientMessage(playerid,0x00FF00FF,"�� ���� ���������, ����� ����� � ���� �������");
					
					players[playerid][current_step]++;
					players[playerid][timer] = SetTimerEx("Mission_CutsceneStep",10000,0,"d",playerid);
				}
				case 1: // ����� ������
				{
				    SetPlayerSpecialAction(playerid,SPECIAL_ACTION_STOPUSECELLPHONE);
					TextDrawHideForPlayer(playerid,Letterbox[0]);
					TextDrawHideForPlayer(playerid,Letterbox[1]);
					TogglePlayerControllable(playerid,true);
					
					// ������������� ������ ��� ������ ������
					players[playerid][current_step] = 0;
					players[playerid][iscutscene] = 0;
					// ��������� ������
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",0,0,"d",playerid);
				}
			}
		    
		}
	}
	return 1;
}

public Mission_InProgressStep(playerid)
{
	// ��������� � �������� �� �����, ���� �� (��������� ������ ><)
	if(players[playerid][iscutscene]) return printf("[ERROR - missions.amx] Mission_InProgressStep called for player in cutscene");
	// ���������� ������� ������
	new missionid = players[playerid][current_mission];
	switch(missionid)
	{
		case 0: // Let's Rock! Part 1
		{
			// ���������� ������� ���� ����������
			switch(players[playerid][current_step])
			{
				case 0: // ������ ������
				{
					// �������� ���� ���� ����� ��������
					SetPlayerCheckpoint(playerid,1189.6824,-1275.7974,13.2871,0.0);
					
					// ������������� ������
					players[playerid][current_step]++;
					// ������� ��������� ���
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
				}
				case 1:
				{
				    // ������� ���� ����� ����� ������ � ����
					if(IsPlayerInRangeOfPoint(playerid,75.0,1189.6824,-1275.7974,13.2871))
				    {
				        KillTimer(players[playerid][timer]);
				        DisablePlayerCheckpoint(playerid);
				        // �������� ����� �������
						TogglePlayerControllable(playerid,false);
						TextDrawShowForPlayer(playerid,Letterbox[0]);
						TextDrawShowForPlayer(playerid,Letterbox[1]);
						// ������ ������ NPC
						player_mission_data[playerid][missionid][0] = CreateVehicle(421,1189.6824,-1275.7974,13.2871,90.6647,0,0,-1);
						// ������ ������ NPC
						player_mission_data[playerid][missionid][1] = FindLastFreeSlot();
					    format(strtmp,sizeof(strtmp),"MissionNPC_%d",player_mission_data[playerid][missionid][1]);
					    CreateNPC(player_mission_data[playerid][missionid][1],strtmp);
					    SetSpawnInfo(player_mission_data[playerid][missionid][1],0,166,1180.1959,-1260.9313,15.1797,267.2988,0,0,0,0,0,0);
					    SpawnNPC(player_mission_data[playerid][missionid][1]);
					    // ���������� ���� �������� � ������
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],1184.0657,-1267.7537,15.1797);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],1185.4275,-1271.0238,13.5469);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],1188.9404,-1274.0692,13.5469);
						// ���������� NPC �� ����
						controller_MoveToByCoords(player_mission_data[playerid][missionid][1],NPC_MOVE_PED_WALK);
						// ���������� ���
						InterpolateCameraLookAt(playerid,1180.1959,-1260.9313,15.1797,1189.6824,-1275.7974,13.2871,10000,CAMERA_MOVE);
						
						SendClientMessage(playerid,0x00FF00FF,"� ��� � ����, ����� ���������� ���� �� ����������...");

						// ������������� ������
						players[playerid][current_step]++;
						// ������� ��������� ���
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
				    }
				}
				case 2:
				{
				    // ������� ���� NPC ������� � ������
					if(!controller_IsMoving(player_mission_data[playerid][missionid][1]))
				    {
				        KillTimer(players[playerid][timer]);
				        // ����������� ����� �������
					    SetCameraBehindPlayer(playerid);
						TextDrawHideForPlayer(playerid,Letterbox[0]);
						TextDrawHideForPlayer(playerid,Letterbox[1]);
						TogglePlayerControllable(playerid,true);
						// ����� NPC � ������
						PutNPCInVehicle(player_mission_data[playerid][missionid][1],player_mission_data[playerid][missionid][0],0);
						// ���������� �� ����
						controller_DriveToByNodes(player_mission_data[playerid][missionid][1],1022.8997,-1135.9625,23.2232,20.0,0);
						// ������������� ������
						player_mission_data[playerid][missionid][2] = 0;

						SendClientMessage(playerid,0x00FF00FF,"������ ������������ ������� ������...");

						// ������������� ������
						players[playerid][current_step]++;
						// ������� ��������� ���
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
				    }
				}
				case 3:
				{
				    // ������� ���� NPC ������ �� ������ ����
					if(!controller_IsMoving(player_mission_data[playerid][missionid][1]))
				    {
				        KillTimer(players[playerid][timer]);

						SendClientMessage(playerid,0x00FF00FF,"��� �� ��� �����������?");

						// ������������� ������
						players[playerid][current_step]++;
						// ������� ��������� ���
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",5000,0,"d",playerid);
				    }
				    else
				    {
				        GetPlayerPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
				        // ��������� �� ������ �� �����
				        if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][1],250.0,ftmp[0],ftmp[1],ftmp[2]))
				        {
				            // ��������� �� ������� �� ����� ������� ������
					        if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][1],50.0,ftmp[0],ftmp[1],ftmp[2]))
					    	{
					    	    // ��������� �� ����������� ����������
					    	    if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][1],25.0,ftmp[0],ftmp[1],ftmp[2]))
					    		{
					    		    // ������� ������, ������ ���������
					    		    SendClientMessage(playerid,0x00FF00FF,"����, �� ���� �������...������ �� �� ������ �� �������.");
					    		    GameTextForPlayer(playerid, "~g~~h~Mission Failed!", 3000, 3);
		    						Mission_Clear(playerid);
					    		}
					    		else
					    		{
					    		    // ���������� ��������������
					    		    SendClientMessage(playerid,0x00FF00FF,"� ������� ������, �� ����� ��������");
					    		}
					    	}
					    	// �������� ������� ����������
					    	player_mission_data[playerid][missionid][2] = 0;
						}
						else
						{
						    SendClientMessage(playerid,0x00FF00FF,"� ������� ������, ����� ����������� �����");
						    player_mission_data[playerid][missionid][2]++;
						    
						    if(player_mission_data[playerid][missionid][2] == 20)
						    {
						        SendClientMessage(playerid,0x00FF00FF,"����, � ������� ���...");
				    		    GameTextForPlayer(playerid, "~g~~h~Mission Failed!", 3000, 3);
	    						Mission_Clear(playerid);
						    }
						}
				    }
				}
				case 4:
				{
				    // ���������� NPC � ��������� �����
				    controller_DriveToByNodes(player_mission_data[playerid][missionid][1],817.9597,-1331.3726,12.9682,20.0,0);

					SendClientMessage(playerid,0x00FF00FF,"������ ������...����� ���� ��� ���������?");
				    
				    // ������������� ������
					players[playerid][current_step]++;
					// ������� ��������� ���
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
				}
				case 5:
				{
				    // ������� ���� NPC ������ �� ������ ����
					if(!controller_IsMoving(player_mission_data[playerid][missionid][1]))
				    {
				        KillTimer(players[playerid][timer]);
						// ��������� ��� �� ������
						PutNPCInVehicle(player_mission_data[playerid][missionid][1],0,0);
						SetNPCPos(player_mission_data[playerid][missionid][1],817.1392,-1334.9526,13.5469);
						SetNPCFacingAngle(player_mission_data[playerid][missionid][1],182.0857);
						// ���������� ���� ��������
						controller_ResetData(player_mission_data[playerid][missionid][1]);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],824.0291,-1341.0946,13.5157);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],829.2626,-1341.5818,11.0234);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],835.9733,-1341.7489,7.1719);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],837.5139,-1345.6136,7.1787);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],832.6624,-1352.0516,3.3359);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],827.7214,-1357.4257,-0.5078);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],823.3764,-1361.9156,-0.5078);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],804.6523,-1345.9818,-0.5078);
					    // ���������� ��� �� ����
					    controller_MoveToByCoords(player_mission_data[playerid][missionid][1],NPC_MOVE_PED_WALK);
					    // ������ ���� ����������
					    for(new i = 2;i < 4;i++)
					    {
					        player_mission_data[playerid][missionid][i] = FindLastFreeSlot();
						    format(strtmp,sizeof(strtmp),"MissionNPC_%d",player_mission_data[playerid][missionid][i]);
						    CreateNPC(player_mission_data[playerid][missionid][i],strtmp);
						    SetSpawnInfo(player_mission_data[playerid][missionid][i],0,165,0.0,0.0,0.0,0.0,0,0,0,0,0,0);
						    SpawnNPC(player_mission_data[playerid][missionid][i]);
					    }
					    // ������ �� �� �����
					    SetNPCPos(player_mission_data[playerid][missionid][2],802.8537,-1341.7098,-0.5078);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][2],221.1942);
					    SetNPCPos(player_mission_data[playerid][missionid][3],800.3175,-1344.5209,-0.5078);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][3],238.0099);

						SendClientMessage(playerid,0x00FF00FF,"�� ����������, ����� ���� �� ���...");

						// ������������� ������
						players[playerid][current_step]++;
						// ������� ��������� ���
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
				    }
				    else
				    {
				        GetPlayerPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
				        // ��������� �� ������ �� �����
				        if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][1],250.0,ftmp[0],ftmp[1],ftmp[2]))
				        {
				            // ��������� �� ������� �� ����� ������� ������
					        if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][1],50.0,ftmp[0],ftmp[1],ftmp[2]))
					    	{
					    	    // ��������� �� ����������� ����������
					    	    if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][1],25.0,ftmp[0],ftmp[1],ftmp[2]))
					    		{
					    		    // ������� ������, ������ ���������
					    		    SendClientMessage(playerid,0x00FF00FF,"����, �� ���� �������...������ �� �� ������ �� �������.");
					    		    GameTextForPlayer(playerid, "~g~~h~Mission Failed!", 3000, 3);
		    						Mission_Clear(playerid);
					    		}
					    		else
					    		{
					    		    // ���������� ��������������
					    		    SendClientMessage(playerid,0x00FF00FF,"� ������� ������, �� ����� ��������");
					    		}
					    	}
						}
						else
						{
						    SendClientMessage(playerid,0x00FF00FF,"� ������� ������, ����� ����������� �����");
						    player_mission_data[playerid][missionid][2]++;

						    if(player_mission_data[playerid][missionid][2] == 20)
						    {
						        SendClientMessage(playerid,0x00FF00FF,"����, � ������� ���...");
				    		    GameTextForPlayer(playerid, "~g~~h~Mission Failed!", 3000, 3);
	    						Mission_Clear(playerid);
						    }
						}
				    }
				}
				case 6:
				{
				    // ������� ���� NPC ����� �� �����
					if(!controller_IsMoving(player_mission_data[playerid][missionid][1]))
				    {
				        KillTimer(players[playerid][timer]);
						// ����� NPC ������� ������, ���� ����� �� �� �����
				        GetPlayerPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
						for(new i = 1;i < 4;i++)
						{
							SetNPCWeapon(player_mission_data[playerid][missionid][i],29);
							NPC_ShootAt(player_mission_data[playerid][missionid][i],ftmp[0],ftmp[1],ftmp[2]);
						}
						// � ���� ������ ������� ��� ����
						SetPlayerChatBubble(player_mission_data[playerid][missionid][1],"�� ���� �����, �� ����, ��� � � �������, ������ ���!",0xFF0000FF,50.0,10000);
						// ������� ����
						SendClientMessage(playerid,0xFF0000FF,"��� � ���, ���� ����� �� ����!");

						// ������������� ������
						players[playerid][current_step]++;
						// ������� ��������� ���
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",1000,1,"d",playerid);
				    }
				}
				case 7:
				{
				    // ��� ���� NPC �� ����� �����
				    new i;
				    for(i = 1;i < 4;i++)
					{
						if(GetNPCState(player_mission_data[playerid][missionid][i]) != NPC_STATE_DEATH) break;
					}
					if(i == 4)
					{
					    // NPC ������
					    KillTimer(players[playerid][timer]);
				        DestroyVehicle(player_mission_data[playerid][missionid][0]);
				        for(i = 1;i < 4;i++) DestroyNPC(player_mission_data[playerid][missionid][i]);
				        GameTextForPlayer(playerid, "~g~~h~Mission Passed!", 3000, 3);
						PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
						GivePlayerMoney(playerid, 5000);

						// ������������� ������
						players[playerid][current_mission] = -1;
						players[playerid][current_step] = 0;
						players[playerid][iscutscene] = 0;
						players[playerid][progress][missionid] = 1;
						players[playerid][timer] = -1;
					}
				}
			}
		}
		case 1: // Let's Rock! Part 2
		{
		    // ���������� ������� ���� ����������
			switch(players[playerid][current_step])
			{
				case 0: // ������ ������
				{
					// �������� ���� ���� ����� ��������
					SetPlayerCheckpoint(playerid,2148.4314,-2260.8513,13.3001,0.0);

					// ������������� ������
					players[playerid][current_step]++;
					// ������� ��������� ���
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
				}
				case 1:
				{
				    // ������� ���� ����� ����� ������ � ����
					if(IsPlayerInRangeOfPoint(playerid,100.0,2148.4314,-2260.8513,13.3001))
				    {
				        KillTimer(players[playerid][timer]);
				        DisablePlayerCheckpoint(playerid);
						// �������� ������� �����
						player_mission_data[playerid][missionid][0] = 0;
						// ������ NPC
						for(new i = 1;i < 9;i++)
						{
							player_mission_data[playerid][missionid][i] = FindLastFreeSlot();
						    format(strtmp,sizeof(strtmp),"MissionNPC_%d",player_mission_data[playerid][missionid][i]);
						    CreateNPC(player_mission_data[playerid][missionid][i],strtmp);
						    SetSpawnInfo(player_mission_data[playerid][missionid][i],0,166,0.0,0.0,0.0,0.0,0,0,0,0,0,0);
						    SpawnNPC(player_mission_data[playerid][missionid][i]);
						    SetPlayerColor(player_mission_data[playerid][missionid][i],0x00000000);
					    }
					    // ����������� �� ��������
					    // 1
					    SetNPCPos(player_mission_data[playerid][missionid][1],2127.0657,-2287.4819,13.4600);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][1],220.1741);
					    SetNPCWeapon(player_mission_data[playerid][missionid][1],31);
					    // 2
					    SetNPCPos(player_mission_data[playerid][missionid][2],2128.6174,-2286.2454,13.4465);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][2],225.4591);
					    SetNPCWeapon(player_mission_data[playerid][missionid][2],31);
					    // 3
					    SetNPCPos(player_mission_data[playerid][missionid][3],2133.0415,-2278.0474,20.6719);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][3],313.6685);
					    SetNPCWeapon(player_mission_data[playerid][missionid][3],31);
					    // 4
					    SetNPCPos(player_mission_data[playerid][missionid][4],2139.4290,-2267.6992,13.3044);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][4],262.7409);
					    SetNPCWeapon(player_mission_data[playerid][missionid][4],24);
					    // 5
					    SetNPCPos(player_mission_data[playerid][missionid][5],2143.0178,-2267.9617,13.2936);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][5],78.7289);
					    SetNPCWeapon(player_mission_data[playerid][missionid][5],24);
					    // 6
					    SetNPCPos(player_mission_data[playerid][missionid][6],2179.9490,-2253.9900,14.7734);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][6],226.3703);
					    SetNPCWeapon(player_mission_data[playerid][missionid][6],31);
					    // 7
					    SetNPCPos(player_mission_data[playerid][missionid][7],2177.6272,-2255.6787,14.7700);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][7],226.3703);
					    SetNPCWeapon(player_mission_data[playerid][missionid][7],31);
					    // 8
					    SetNPCPos(player_mission_data[playerid][missionid][8],2167.3552,-2254.1885,13.3019);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][8],39.8125);
					    SetNPCWeapon(player_mission_data[playerid][missionid][8],31);
					    // ��� ���������� ������� ���� ��������������
					    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2160.9036,-2247.5107,13.2927);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2165.6145,-2241.9050,13.2875);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.6187,-2237.7000,13.2937);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2151.6653,-2247.4675,13.2987);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.2144,-2257.1599,13.3008);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2152.7708,-2265.6494,13.3009);
					    // � �������� ��� �� ����� ����
					    controller_MoveToByCoords(player_mission_data[playerid][missionid][8],NPC_MOVE_PED_WALK);
					    // ������ ������ ���� ��� ����� �������
					    SetPlayerCheckpoint(playerid,2123.2700,-2275.2988,20.6719,2.0);
					    // ��������� ��� ������� ���
					    player_mission_data[playerid][missionid][9] = 0;
					    
						SendClientMessage(playerid,0x00FF00FF,"��� �������� �����...");
						SendClientMessage(playerid,0x00FF00FF,"���� � �� ����...");
						SendClientMessage(playerid,0x00FF00FF,"���� ������ � ������� ��������� �� �������� ������ �������...");

						// ������������� ������
						players[playerid][current_step]++;
						// ������� ��������� ���
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
				    }
				}
				case 2:
				{
				    player_mission_data[playerid][missionid][0]++;
				    // ��������� �� ��������� �� �����
				    if(player_mission_data[playerid][missionid][0] == 300)
				    {
				        SendClientMessage(playerid,0x00FF00FF,"��� ��������� �� ������...");
		    		    GameTextForPlayer(playerid, "~g~~h~Mission Failed!", 3000, 3);
						Mission_Clear(playerid);
				    }
				    else
				    {
					    // ��������� �� ������� �� �����
					    if(IsPlayerInRangeOfPoint(playerid,2.0,2123.2700,-2275.2988,20.6719))
					    {
					        KillTimer(players[playerid][timer]);
					        DisablePlayerCheckpoint(playerid);

					        SendClientMessage(playerid,0x00FF00FF,"������, ����� ������� ������ ��� ����� ������");

							// ������������� ������
							players[playerid][current_step]++;
							// ������� ��������� ���
							players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
						}
						else
						{
						    GetPlayerPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
						    // ��������� �������� NPC
						    // ��������� ������� �� �������
						    if(player_mission_data[playerid][missionid][9] == 1)
						    {
						        // ��� NPC �������� � ������ �� ���������
								for(new i = 1;i < 9;i++)
								{
								    if(GetNPCState(player_mission_data[playerid][missionid][i]) == NPC_STATE_DEATH) continue; // ���������� ������
								    
								    if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][i],20.0,ftmp[0],ftmp[1],ftmp[2]))
										NPC_ShootAt(player_mission_data[playerid][missionid][i],ftmp[0],ftmp[1],ftmp[2]);
								}
						    }
						    else
						    {
						        // �������
							    ApplyAnimation(player_mission_data[playerid][missionid][4],"PED","IDLE_chat",4.1,1,0,0,0,0,1);
							    ApplyAnimation(player_mission_data[playerid][missionid][5],"PED","IDLE_chat",4.1,1,0,0,0,0,1);
							    // ����������
							    if(!controller_IsMoving(player_mission_data[playerid][missionid][8]))
							    {
							        if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][8],1.0,2152.7708,-2265.6494,13.3009))
							        {
									    // ������� �������� ����
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.2144,-2257.1599,13.3008);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2151.6653,-2247.4675,13.2987);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.6187,-2237.7000,13.2937);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2165.6145,-2241.9050,13.2875);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2160.9036,-2247.5107,13.2927);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2167.3552,-2254.1885,13.3019);
									    // � �������� ��� �� ����� ����
									    controller_MoveToByCoords(player_mission_data[playerid][missionid][8],NPC_MOVE_PED_WALK);
								    }
								    else
								    {
								        // ��������� �����
								        controller_AddPathCoords(player_mission_data[playerid][missionid][8],2160.9036,-2247.5107,13.2927);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2165.6145,-2241.9050,13.2875);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.6187,-2237.7000,13.2937);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2151.6653,-2247.4675,13.2987);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.2144,-2257.1599,13.3008);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2152.7708,-2265.6494,13.3009);
									    // � �������� ��� �� ����� ����
									    controller_MoveToByCoords(player_mission_data[playerid][missionid][8],NPC_MOVE_PED_WALK);
								    }
							    }
							    // ��������� ��� �� ������ ����� NPC
							    for(new i = 1;i < 9;i++)
								{
								    if(GetNPCState(player_mission_data[playerid][missionid][i]) == NPC_STATE_DEATH) continue; // ���������� ������
								    
								    if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][i],50.0,ftmp[0],ftmp[1],ftmp[2]))
								    {
								        GetNPCPos(player_mission_data[playerid][missionid][i],ftmp[3],ftmp[4],ftmp[5]);
								        // ����� ����� NPC, ��������� � ����� �������
								        ftmp[2] = atan2((ftmp[3] - ftmp[0]),(ftmp[4] - ftmp[1]));
								        ftmp[5] = GetNPCFacingAngle(player_mission_data[playerid][missionid][i]);
										if(ftmp[2] < 0.0) ftmp[2] += 360.0;
								        // ���� � ������� ����� NPC
								        if((ftmp[2] > (ftmp[5] - 45.0)) &&(ftmp[2] < (ftmp[5] + 45.0)))
								        {
								            // ��������� �������
								            player_mission_data[playerid][missionid][9] = 1;
								            // ������������� ��������
								            ClearAnimations(player_mission_data[playerid][missionid][4]);
								            ClearAnimations(player_mission_data[playerid][missionid][5]);
								            // ����� NPC �������
											SetPlayerChatBubble(player_mission_data[playerid][missionid][i],"����� �����������!",0xFF0000FF,50.0,10000);
								            
								            SendClientMessage(playerid,0x00FF00FF,"����, ���� ��������!");
								            break;
								        }
								    }
								}
						    }
						}
					}
				}
				case 3:
				{
				    // ������� ���� ����� �����
				    if(!IsPlayerInRangeOfPoint(playerid,200.0,2148.4314,-2260.8513,13.3001))
				    {
					    KillTimer(players[playerid][timer]);
				        for(new i = 1;i < 9;i++) DestroyNPC(player_mission_data[playerid][missionid][i]);
				        GameTextForPlayer(playerid, "~g~~h~Mission Passed!", 3000, 3);
						PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
						GivePlayerMoney(playerid, 5000);

						// ������������� ������
						players[playerid][current_mission] = -1;
						players[playerid][current_step] = 0;
						players[playerid][iscutscene] = 0;
						players[playerid][progress][missionid] = 1;
						players[playerid][timer] = -1;
				    }
					else
					{
					    GetPlayerPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
					    // ��������� �������� NPC
					    // ��������� ������� �� �������
					    if(player_mission_data[playerid][missionid][9] == 1)
					    {
					        // ��� NPC �������� � ������ �� ���������
							for(new i = 1;i < 9;i++)
							{
								if(GetNPCState(player_mission_data[playerid][missionid][i]) == NPC_STATE_DEATH) continue; // ���������� ������
								    
							    if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][i],20.0,ftmp[0],ftmp[1],ftmp[2]))
									NPC_ShootAt(player_mission_data[playerid][missionid][i],ftmp[0],ftmp[1],ftmp[2]);
							}
					    }
					    else
					    {
					        // �������
						    ApplyAnimation(player_mission_data[playerid][missionid][4],"PED","IDLE_chat",4.1,1,0,0,0,0,1);
						    ApplyAnimation(player_mission_data[playerid][missionid][5],"PED","IDLE_chat",4.1,1,0,0,0,0,1);
						    // ����������
						    if(!controller_IsMoving(player_mission_data[playerid][missionid][8]))
						    {
						        if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][8],1.0,2152.7708,-2265.6494,13.3009))
						        {
								    // ������� �������� ����
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.2144,-2257.1599,13.3008);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2151.6653,-2247.4675,13.2987);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.6187,-2237.7000,13.2937);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2165.6145,-2241.9050,13.2875);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2160.9036,-2247.5107,13.2927);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2167.3552,-2254.1885,13.3019);
								    // � �������� ��� �� ����� ����
								    controller_MoveToByCoords(player_mission_data[playerid][missionid][8],NPC_MOVE_PED_WALK);
							    }
							    else
							    {
							        // ��������� �����
							        controller_AddPathCoords(player_mission_data[playerid][missionid][8],2160.9036,-2247.5107,13.2927);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2165.6145,-2241.9050,13.2875);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.6187,-2237.7000,13.2937);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2151.6653,-2247.4675,13.2987);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.2144,-2257.1599,13.3008);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2152.7708,-2265.6494,13.3009);
								    // � �������� ��� �� ����� ����
								    controller_MoveToByCoords(player_mission_data[playerid][missionid][8],NPC_MOVE_PED_WALK);
							    }
						    }
						    // ��������� ��� �� ������ ����� NPC
						    for(new i = 1;i < 9;i++)
							{
								if(GetNPCState(player_mission_data[playerid][missionid][i]) == NPC_STATE_DEATH) continue; // ���������� ������
								    
							    if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][i],20.0,ftmp[0],ftmp[1],ftmp[2]))
							    {
							        GetNPCPos(player_mission_data[playerid][missionid][i],ftmp[3],ftmp[4],ftmp[5]);
							        // ����� ����� NPC, ��������� � ����� �������
							        ftmp[2] = atan2((ftmp[3] - ftmp[0]),(ftmp[4] - ftmp[1]));
							        ftmp[5] = GetNPCFacingAngle(player_mission_data[playerid][missionid][i]);
									if(ftmp[2] < 0.0) ftmp[2] += 360.0;
							        // ���� � ������ ����� NPC
							        if((ftmp[2] > (ftmp[5] - 45.0)) &&(ftmp[2] < (ftmp[5] + 45.0)))
							        {
							            // ��������� �������
							            player_mission_data[playerid][missionid][9] = 1;
							            // ������������� ��������
							            ClearAnimations(player_mission_data[playerid][missionid][4]);
							            ClearAnimations(player_mission_data[playerid][missionid][5]);
							            // ����� NPC �������
										SetPlayerChatBubble(player_mission_data[playerid][missionid][i],"����� �����������!",0xFF0000FF,50.0,10000);

							            SendClientMessage(playerid,0x00FF00FF,"����, ���� ��������!");
							            break;
							        }
							    }
							}
					    }
					}
				}
			}
		}
		case 2: // Case delivery
		{
		    // ���������� ������� ���� ��������
			switch(players[playerid][current_step])
			{
				case 0: // ������ ������
				{
		    		player_mission_data[playerid][missionid][0] = CreatePickup(1210, 23, 2788.7070, -1417.5576, 16.2500);
					player_mission_data[playerid][missionid][1] = CreateVehicle(468, 2792.5432, -1415.6924, 15.9153, 270.0, 0, 0, -1); // sanchez
					SetPlayerCheckpoint(playerid,2788.7070, -1417.5576, 26.2500,0.0);

					// ������������� ������
					players[playerid][current_step]++;
					// ������� ��������� ���
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",1000,1,"d",playerid);
				}
				case 1: // ������� ���� ����� �������� ����
				{
				    if(IsPlayerInRangeOfPoint(playerid,5.0,2788.7070, -1417.5576, 16.2500))
				    {
				        KillTimer(players[playerid][timer]);
				        // ���� ������, �������� ����� �������
				        DestroyPickup(player_mission_data[playerid][missionid][0]);
				        DisablePlayerCheckpoint(playerid);
						TogglePlayerControllable(playerid,false);
						TextDrawShowForPlayer(playerid,Letterbox[0]);
						TextDrawShowForPlayer(playerid,Letterbox[1]);
						PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
						SetPlayerAttachedObject(playerid, 1, 1210, 5, 0.2989, 0.0939, 0.0270, -15.3, -108.0);
						InterpolateCameraPos(playerid, 2788.7070, -1417.5576, 16.2500, 2805.2500, -1419.2341, 19.2500, 2000, CAMERA_MOVE);
						InterpolateCameraLookAt(playerid, 2823.1003, -1418.5031, 16.2562, 2823.1003, -1418.5031, 16.2562, 2000, CAMERA_MOVE);
						SendClientMessage(playerid,0x00FF00FF,"����� ������...");
						
						// ������������� ������
						players[playerid][current_step]++;
						// ������� ��������� ���
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",5000,0,"d",playerid);
				    }
				}
				case 2: // ����� ����� �������, ��������� ������
				{
				    for(new i = 2;i < 6;i++)
				    {
					    player_mission_data[playerid][missionid][i] = FindLastFreeSlot();
					    format(strtmp,sizeof(strtmp),"MissionNPC_%d",player_mission_data[playerid][missionid][i]);
					    CreateNPC(player_mission_data[playerid][missionid][i],strtmp);
					    SetSpawnInfo(player_mission_data[playerid][missionid][i],0,126,0.0,0.0,0.0,0.0,0,0,0,0,0,0);
					    SpawnNPC(player_mission_data[playerid][missionid][i]);
				    }
				    player_mission_data[playerid][missionid][6] = CreateVehicle(507, 2834.1208,-1483.2860,11.9153,88.9492, 0, 0, -1);
				    PutNPCInVehicle(player_mission_data[playerid][missionid][2],player_mission_data[playerid][missionid][6],0);
				    SetNPCPos(player_mission_data[playerid][missionid][2],2834.1208,-1483.2860,11.9153);
				    SetNPCFacingAngle(player_mission_data[playerid][missionid][2],88.9492);
				    for(new i = 3;i < 6;i++)
				    {
					    PutNPCInVehicle(player_mission_data[playerid][missionid][i],player_mission_data[playerid][missionid][6],(i - 2));
					    SetNPCWeapon(player_mission_data[playerid][missionid][i],29);
					}

					SetPlayerCheckpoint(playerid,1459.1420,-1264.5731,13.3906,4.0);
				    SetCameraBehindPlayer(playerid);
					TextDrawHideForPlayer(playerid,Letterbox[0]);
					TextDrawHideForPlayer(playerid,Letterbox[1]);
					TogglePlayerControllable(playerid,true);
					
					// ������������� ������
					players[playerid][current_step]++;
					// ������� ��������� ���
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",1000,1,"d",playerid);
				}
				case 3: // ������, ���� �������� �� �������, ���� ����� ������� �� ����� ���������� - ������ ��������
				{
				    if(IsPlayerInRangeOfPoint(playerid,5.0,1459.1420,-1264.5731,13.3906))
				    {
				        KillTimer(players[playerid][timer]);
				        // ����� ������ ����� ����������, ������ ��������
				        DestroyVehicle(player_mission_data[playerid][missionid][1]);
					    for(new i = 2;i < 6;i++)
					    {
						    DestroyNPC(player_mission_data[playerid][missionid][i]);
					    }
					    DestroyVehicle(player_mission_data[playerid][missionid][6]);
				        GameTextForPlayer(playerid, "~g~~h~Mission Passed!", 3000, 3);
						PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
						GivePlayerMoney(playerid, 5000);
						DisablePlayerCheckpoint(playerid);
						RemovePlayerAttachedObject(playerid, 1);
				        
						// ������������� ������
						players[playerid][current_mission] = -1;
						players[playerid][current_step] = 0;
						players[playerid][iscutscene] = 0;
						players[playerid][progress][missionid] = 1;
						players[playerid][timer] = -1;
				    }
				    else
				    {
				        // ����� �� �� ����� ����������, ���������� ���
				        new npcid = player_mission_data[playerid][missionid][2];
				        GetPlayerPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
				        // ��� ������ ����� �� ����� � ������
				        controller_DriveToByNodes(npcid,ftmp[0],ftmp[1],ftmp[2],40.0,20);
						// ���������� ���������� �� ����� �� ������ (��� ����������� ����������� ���� ����� ��������)
						GetNPCPos(npcid,ftmp[3],ftmp[4],ftmp[5]);
						ftmp[0] -= ftmp[3];
						ftmp[1] -= ftmp[4];
						ftmp[2] -= ftmp[5];
						new Float:d = floatsqroot((ftmp[0]*ftmp[0]) + (ftmp[1]*ftmp[1]) + (ftmp[2]*ftmp[2]));
						if(d < 50.0)
						{
						    // ���� ����� 50 ������ � ������, ��������
						    for(new i = 3;i < 6;i++)
							{
							    SetNPCCameraFrontVector(player_mission_data[playerid][missionid][i],ftmp[0],ftmp[1],ftmp[2]);
								SetNPCCameraPos(player_mission_data[playerid][missionid][i],ftmp[3],ftmp[4],ftmp[5]);
								SetNPCCameraMode(player_mission_data[playerid][missionid][i],55);
							    SetNPCPassangerDriveBy(player_mission_data[playerid][missionid][i],1);
							    SetNPCKeys(player_mission_data[playerid][missionid][i],0,0,KEY_FIRE);
							}
						}
						else
						{
						    // ���� ������ 50 ������ �� ������,���������� ��������
						    for(new i = 3;i < 6;i++) SetNPCKeys(player_mission_data[playerid][missionid][i],0,0,0);
						}
				    }
				}
			}
		}
	}
	return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
	if(players[playerid][current_mission] != -1)
	{
	    GameTextForPlayer(playerid, "~g~~h~Mission Failed!", 3000, 3);
	    Mission_Clear(playerid);
	}
	return 1;
}

// --------------------------------------------------
// stocks
// --------------------------------------------------

stock Mission_Start(missionid,playerid)
{
	// ��������� ��� ���������� ������
	if(players[playerid][current_mission] != -1)
	{
		// ����� �� ����� ������ ������
		SendClientMessage(playerid,MESSAGES_COLOR,"� ���� � ��� ���� ����.");
		return 0;
	}
	// ��������� ���������� ������
	if(missions[missionid][request] != -1)
	{
		new requestid = missions[missionid][request];
		if(!players[playerid][progress][requestid])
		{
			// ����� �� ����� ������ ������
			SendClientMessage(playerid,MESSAGES_COLOR,"�� ������ ������ ��� ���� ������� ���.");
			return 0;
		}
	}
	// ������������� ������ ������
	players[playerid][current_mission] = missionid;
	players[playerid][current_step] = 0;
	players[playerid][iscutscene] = 1;
	// ���������� ��������
	GameTextForPlayer(playerid,missions[missionid][name],3000,1);
	// �������� ���������� ��������
	players[playerid][timer] = SetTimerEx("Mission_CutsceneStep",0,0,"d",playerid);
	return 1;
}

stock Mission_Clear(playerid)
{
    // ������� ������ ������
	new missionid = players[playerid][current_mission];
	switch(missionid)
	{
		case 0:
		{
		    switch(players[playerid][current_step])
			{
			    case 1:
			    {
					DisablePlayerCheckpoint(playerid);
			    }
			    case 2,3,4,5:
			    {
			        DestroyVehicle(player_mission_data[playerid][missionid][0]);
			        DestroyNPC(player_mission_data[playerid][missionid][1]);
			    }
			    case 6,7:
			    {
			        DestroyVehicle(player_mission_data[playerid][missionid][0]);
			        DestroyNPC(player_mission_data[playerid][missionid][1]);
			        DestroyNPC(player_mission_data[playerid][missionid][2]);
			        DestroyNPC(player_mission_data[playerid][missionid][3]);
			    }
			}
		}
		case 1:
		{
		    switch(players[playerid][current_step])
			{
			    case 1:
			    {
					DisablePlayerCheckpoint(playerid);
			    }
			    case 2:
			    {
			        DisablePlayerCheckpoint(playerid);
			        for(new i = 1;i < 9;i++) DestroyNPC(player_mission_data[playerid][missionid][i]);
			    }
			    case 3:
			    {
			        for(new i = 1;i < 9;i++) DestroyNPC(player_mission_data[playerid][missionid][i]);
			    }
			}
		}
	    case 2: // Case delivery
		{
			switch(players[playerid][current_step])
			{
				case 1:
				{
		    		DestroyPickup(player_mission_data[playerid][missionid][0]);
					DestroyVehicle(player_mission_data[playerid][missionid][1]);
					RemovePlayerMapIcon(playerid, 98);
				}
				case 2:
				{
				    DestroyVehicle(player_mission_data[playerid][missionid][1]);
				    RemovePlayerAttachedObject(playerid, 1);
				}
				case 3:
				{
				    DestroyVehicle(player_mission_data[playerid][missionid][1]);
				    RemovePlayerAttachedObject(playerid, 1);
				    for(new i = 2;i < 6;i++)
				    {
					    DestroyNPC(player_mission_data[playerid][missionid][i]);
				    }
				    DestroyVehicle(player_mission_data[playerid][missionid][6]);
				    DisablePlayerCheckpoint(playerid);
				}
			}
		}
	}
	if(players[playerid][timer] != 0) KillTimer(players[playerid][timer]);
	players[playerid][current_mission] = -1;
	players[playerid][current_step] = -1;
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
