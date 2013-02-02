/*
*	Created:			26.08.12
*	Author:				009
*	Last Modifed:		02.02.13
*	Description:		Controllable NPC's maniacs filterscript
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
#define VERSION "0.1"

#if !defined MAX_STRING
	#define MAX_STRING 128
#endif

#define MESSAGES_COLOR 0xFFFFFFFF
#define MAX_MANIACS 490

// --------------------------------------------------
// enums
// --------------------------------------------------

// --------------------------------------------------
// news
// --------------------------------------------------
new strtmp[MAX_STRING],
	Float:ftmp[3],
    badSkins[] = {3, 4, 5, 6, 8, 42, 65, 74, 86, 119, 149, 208, 273, 289},
	maniac[MAX_MANIACS],
	maniac_nodeid[MAX_MANIACS],
	maniac_lastnodeid[MAX_MANIACS],
	maniac_target[MAX_MANIACS],
	maniac_talk[][128] = {
	    "������� � ���� �����, ��������� ����� ����...",
	    "�������, �������, � ���� ��������� �����",
	    "� ����� ����. ������ ����� � �����. ����� � ������ ���� � ������� ����� ������.",
	    "��� �� ���..."
	},
	maniac_lost[][64] = {
	    "������ ��� � ����...",
	    "�������, �� ���� �� ��!"
	},
	timer;

// --------------------------------------------------
// forwards
// --------------------------------------------------
forward ManiacsCheck();
forward RespawnNPC(npcid);

// --------------------------------------------------
// publics
// --------------------------------------------------
public OnFilterScriptInit()
{
	new skinid,
	    weaponid;
    // ���������� ����
	nodes_Init();
	// ������ � ��������� ������ ��������� ���������� ��������, �� ��������� ������� � ������
	for(new i = 0;i < MAX_MANIACS;i++)
	{
	    // �������� ��������� ����� ��������� NPC
		maniac_nodeid[i] = random(nodes_GetNodeCount(PED_NODES));
		while(nodes_GetNodeLinkCount(PED_NODES,maniac_nodeid[i]) == 0) maniac_nodeid[i] = random(nodes_GetNodeCount(PED_NODES));
		// �������� ��������� ����
		do skinid = random(300);
		while(!IsValidSkin(skinid));
		// �������� ��������� ������
		weaponid = random(38);
		// �������� ���������� ���������
		nodes_GetNodePos(PED_NODES,maniac_nodeid[i],ftmp[0],ftmp[1],ftmp[2]);
		// ������ NPC
		maniac[i] = FindLastFreeSlot();
		format(strtmp,sizeof(strtmp),"Maniac_%d",maniac[i]);
		CreateNPC(maniac[i],strtmp);
		SetSpawnInfo(maniac[i],0,skinid,ftmp[0],ftmp[1],ftmp[2],0.0,0,0,0,0,0,0);
		SpawnNPC(maniac[i]);
		SetNPCWeapon(maniac[i],weaponid);
		// ���������� � ����� �� ��������� �����
		maniac_lastnodeid[i] = maniac_nodeid[i];
		maniac_nodeid[i] = nodes_GetNodeLink(PED_NODES,maniac_lastnodeid[i],random(nodes_GetNodeLinkCount(PED_NODES,maniac_lastnodeid[i])));
		nodes_GetNodePos(PED_NODES,maniac_nodeid[i],ftmp[0],ftmp[1],ftmp[2]);
		NPC_WalkTo(maniac[i],ftmp[0],ftmp[1],ftmp[2] + 0.5,0);
		// ������������� ������
		maniac_target[i] = INVALID_PLAYER_ID;
	}
	// ������ ������ ��������� ��������
	timer = SetTimer("ManiacsCheck",500,1);

	print("Controllable NPC's maniacs " VERSION " loaded.");
}

public OnFilterScriptExit()
{
	// ���������� ������
	KillTimer(timer);
	// ���������� NPC
    for(new i = 0;i < MAX_MANIACS;i++) DestroyNPC(maniac[i]);
}

public OnNPCSpawn(npcid)
{
    for(new i = 0;i < MAX_MANIACS;i++)
	{
	    if(maniac[i] != npcid) continue;
	    
		GetNPCPos(npcid,ftmp[0],ftmp[1],ftmp[2]);
    	maniac_nodeid[i] = nodes_GetNodeid(PED_NODES,ftmp[0],ftmp[1],ftmp[2]);
    	nodes_GetNodePos(PED_NODES,maniac_nodeid[i],ftmp[0],ftmp[1],ftmp[2]);
		NPC_WalkTo(npcid,ftmp[0],ftmp[1],ftmp[2] + 0.5,0);
	}
}

public OnNPCDeath(npcid,killerid,reason)
{
	SetTimerEx("RespawnNPC",1000,0,"d",npcid);
}

public OnNPCMovingComplete(npcid)
{
    for(new i = 0;i < MAX_MANIACS;i++)
	{
	    if(maniac[i] != npcid) continue;

	    // ���������� � ����� �� ��������� �����
		maniac_lastnodeid[i] = maniac_nodeid[i];
		if(nodes_GetNodeLinkCount(PED_NODES,maniac_lastnodeid[i]) > 1)
		{
			do maniac_nodeid[i] = nodes_GetNodeLink(PED_NODES,maniac_lastnodeid[i],random(nodes_GetNodeLinkCount(PED_NODES,maniac_lastnodeid[i])));
			while(maniac_lastnodeid[i] == maniac_nodeid[i]);
		}
		else maniac_nodeid[i] = nodes_GetNodeLink(PED_NODES,maniac_lastnodeid[i],random(nodes_GetNodeLinkCount(PED_NODES,maniac_lastnodeid[i])));
	    
		nodes_GetNodePos(PED_NODES,maniac_nodeid[i],ftmp[0],ftmp[1],ftmp[2]);
		NPC_WalkTo(maniac[i],ftmp[0],ftmp[1],ftmp[2] + 0.5,0);
		// ����� ��� ���� ������
		SetPlayerChatBubble(maniac[i],maniac_talk[random(sizeof(maniac_talk))],0xFF0000FF,50.0,10000);
	    return;
	}
}

public ManiacsCheck()
{
    for(new i = 0;i < MAX_MANIACS;i++)
	{
	    if(maniac_target[i] == INVALID_PLAYER_ID)
	    {
		    // ���� ���, �������� �� ���� ������� ��� ������� ����� ���� NPC
		    for(new j = 0;j < MAX_PLAYERS;j++)
		    {
		        if(!IsPlayerConnected(j)) continue;
		        if(!IsPlayerStreamedIn(maniac[i],j)) continue;

		        // �������� ���������� ������
		        GetPlayerPos(j,ftmp[0],ftmp[1],ftmp[2]);
		        // ��������� � ������� ����� �� �����
		        if(!IsPlayerInRangeOfPoint(maniac[i],25.0,ftmp[0],ftmp[1],ftmp[2])) continue;
				// ������� ������
				StopNPC(maniac[i]);
				NPC_ShootAt(maniac[i],ftmp[0],ftmp[1],ftmp[2]);
				maniac_target[i] = j;
				break;
		    }
	    }
	    else
	    {
	        // ���� ����
	        // �������� ���������� ������
			GetPlayerPos(maniac_target[i],ftmp[0],ftmp[1],ftmp[2]);
			// ��������� � ������� ����� �� ����
			if(!IsPlayerInRangeOfPoint(maniac[i],25.0,ftmp[0],ftmp[1],ftmp[2]))
			{
			    // ��������� ������
				nodes_GetNodePos(PED_NODES,maniac_nodeid[i],ftmp[0],ftmp[1],ftmp[2]);
				NPC_WalkTo(maniac[i],ftmp[0],ftmp[1],ftmp[2] + 0.5,0);
				// ����� ��� ���� ������
				SetPlayerChatBubble(maniac[i],maniac_lost[random(sizeof(maniac_lost))],0xFF0000FF,50.0,10000);
				// ������� ������
			    maniac_target[i] = INVALID_PLAYER_ID;
			}
			else
			{
			    // ������� ������
				NPC_ShootAt(maniac[i],ftmp[0],ftmp[1],ftmp[2]);
			}
	    }
	}
}

public RespawnNPC(npcid)
{
	SpawnNPC(npcid);
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
