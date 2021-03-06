/*
*	Created:			25.08.12
*	Author:				009
*	Last Modifed:		11.10.12
*	Description:		NPC controller (Controllable NPC)
*/


#if defined _cnpc_controller_included
	#endinput
#endif

#define _cnpc_controller_included
#pragma library cnpc_controller

// --------------------------------------------------
// includes
// --------------------------------------------------
#include <cnpc>
#include <cnpc_paths>

// --------------------------------------------------
// defines
// --------------------------------------------------
// ������� ������������
#define NPC_MOVE_PED_WALK			1
#define NPC_MOVE_PED_RUN			2
#define NPC_MOVE_PED_SPRINT			3

#define MOVING_BY_ZMAP				0

// --------------------------------------------------
// enums
// --------------------------------------------------
enum
{
	NPC_STATE_NOT_CONTROLLED,
	NPC_STATE_DRIVE_BY_NODES,
	NPC_STATE_DRIVE_BY_COORDS,
	NPC_STATE_MOVE_BY_NODES,
	NPC_STATE_MOVE_BY_COORDS,
	NPC_STATE_FOLLOW_BY_NODES
};

// --------------------------------------------------
// statics
// --------------------------------------------------
static
	npc_state[MAX_NPCS char],		// ��������� NPC
	npc_type[MAX_NPCS char],		// ��� ���������
	npc_target[MAX_NPCS],			// ���� ��������
	npc_target_node[MAX_NPCS],
	// moving
	npc_step[MAX_NPCS],				// ����� �� ����, � ������� ����� ��������� NPC ����� ���������� �������� ��������
	Float:npc_speed[MAX_NPCS],		// �������� ������������ NPC �� ���� (��� ����������)
	npc_boost[MAX_NPCS char];		// ������� ����������� � �������� ������������ NPC �� ����, ������������� ��� "���������� ���������� �� �������� ����� / 100 * �������" (��� ����������)
	
// --------------------------------------------------
// forwards
// --------------------------------------------------

// --------------------------------------------------
// stocks
// --------------------------------------------------
/*
	Name: 
		controller_ResetData(npcid)
	Description:
		��������� ������ ����������� ���������� NPC
	Params:
		npcid - id NPC
	Return:
		-
*/
stock controller_ResetData(npcid)
{
	new str[32];
	// ������
	npc_state{npcid} = NPC_STATE_NOT_CONTROLLED;
	npc_type{npcid} = 0;
	npc_step[npcid] = 0;
	npc_speed[npcid] = 0.0;
	npc_boost{npcid} = 0;
	npc_target[npcid] = -1;
	npc_target_node[npcid] = -1;
	// ���� �������������� �����
	paths_ResetData(npcid);
	// ������������ ����
	new count = GetPVarInt(npcid,"controller_path_coords_count");
	for(new i = 0;i < count;i++)
	{
		format(str,sizeof(str),"controller_path_coord%d_X",i);
		DeletePVar(npcid,str);
		format(str,sizeof(str),"controller_path_coord%d_Y",i);
		DeletePVar(npcid,str);
		format(str,sizeof(str),"controller_path_coord%d_Z",i);
		DeletePVar(npcid,str);
		format(str,sizeof(str),"controller_path_coord%d_dist",i);
		DeletePVar(npcid,str);
	}
	DeletePVar(npcid,"controller_path_coords_count");
	DeletePVar(npcid,"controller_path_coords_dist");
}

/*
	Name: 
		controller_AddPathCoords(npcid,Float:x,Float:y,Float:z)
	Description:
		��������� ����� ���������� ��� ��������
	Params:
		npcid - id NPC
		x,y,z - ����������
	Return:
		-
*/
stock controller_AddPathCoords(npcid,Float:x,Float:y,Float:z)
{
	new Float:lx,Float:ly,Float:lz;
	new count = GetPVarInt(npcid,"controller_path_coords_count");
	new str[32];
	new Float:dist = 0.0;
	// ��������� ������ ����� �����
	format(str,sizeof(str),"controller_path_coord%d_X",count);
	SetPVarFloat(npcid,str,x);
	format(str,sizeof(str),"controller_path_coord%d_Y",count);
	SetPVarFloat(npcid,str,y);
	format(str,sizeof(str),"controller_path_coord%d_Z",count);
	SetPVarFloat(npcid,str,z);
	// �������� ��������� �� ������� �������
	if(count == 0)
	{
		format(str,sizeof(str),"controller_path_coord%d_dist",count);
		SetPVarFloat(npcid,str,dist);
	}
	else
	{
		// �������� ���������� ���������� �����
		format(str,sizeof(str),"controller_path_coord%d_X",count - 1);
		lx = GetPVarFloat(npcid,str);
		format(str,sizeof(str),"controller_path_coord%d_Y",count - 1);
		ly = GetPVarFloat(npcid,str);
		format(str,sizeof(str),"controller_path_coord%d_Z",count - 1);
		lz = GetPVarFloat(npcid,str);
		// ������� ����������
		dist = floatsqroot((x-lx)*(x-lx) + (y-ly)*(y-ly) + (z-lz)*(z-lz));
		// ����������
		format(str,sizeof(str),"controller_path_coord%d_dist",count);
		SetPVarFloat(npcid,str,dist);
	}
	// ������ ���������� �����
	SetPVarInt(npcid,"controller_path_coords_count",count + 1);
	// ��������� ����� ���������
	if(count == 0) SetPVarFloat(npcid,"controller_path_coords_dist",0.0);
	else
	{
		new Float:newdist = GetPVarFloat(npcid,"controller_path_coords_dist") + dist;
		SetPVarFloat(npcid,"controller_path_coords_dist",newdist);
	}
}

/*
	Name: 
		controller_GetPathCoordsLen(npcid)
	Description:
		�������� ���������� ��������� ��� ��������
	Params:
		npcid - id NPC
	Return:
		int
*/
stock controller_GetPathCoordsLen(npcid)
{
	return GetPVarInt(npcid,"controller_path_coords_count");
}

/*
	Name: 
		controller_GetPathCoordsData(npcid,pointid,&Float:x,&Float:y,&Float:z)
	Description:
		�������� ���������� ������������ ����� �� ��������� ��� ��������
	Params:
		npcid - id NPC
		pointid - id �����
		x,y,z - ���������� (������������)
	Return:
		1 - ���������� ����� ���� � ����������
		0 - ����� ����� ���
*/
stock controller_GetPathCoordsData(npcid,pointid,&Float:x,&Float:y,&Float:z)
{
	new count = GetPVarInt(npcid,"controller_path_coords_count");
	new str[32];
	
	if(pointid < 0) return 0;
	if(pointid >= count) return 0;
	
	// �������� ������ �����
	format(str,sizeof(str),"controller_path_coord%d_X",pointid);
	x = GetPVarFloat(npcid,str);
	format(str,sizeof(str),"controller_path_coord%d_Y",pointid);
	y = GetPVarFloat(npcid,str);
	format(str,sizeof(str),"controller_path_coord%d_Z",pointid);
	z = GetPVarFloat(npcid,str);
	
	return 1;
}

/*
	Name: 
		controller_GetPathCoordsDist(npcid)
	Description:
		�������� ��������� ����� ����
	Params:
		npcid - id NPC
	Return:
		Float
*/
stock Float:controller_GetPathCoordsDist(npcid)
{
	return GetPVarFloat(npcid,"controller_path_coords_dist");
}

/*
	Name: 
		controller_DriveToByNodes(npcid,Float:x,Float:y,Float:z,Float:speed,boost)
	Description:
		��������� ����������� ���� �� ������� ������� NPC �� ��������� ��������� �� �������������� �����, � �������� � ���������� �� ����.
	Params:
		npcid - id NPC
		x,y,z - ���������� ����
		speed - ������� �������� ��������
		boost - ������� ��������� ��� ���������� �� ����
	Return:
		1 - ���� �������� � NPC �������� �� ����
		0 - ������ ���������� ����
*/
stock controller_DriveToByNodes(npcid,Float:x,Float:y,Float:z,Float:speed,boost)
{
	new Float:tmp[7],
		startnodeid,
		endnodeid;
	
	switch(npc_state{npcid})
	{
		case NPC_STATE_NOT_CONTROLLED:
		{
			// NPC �� �������� �� ����, ��������� ���� �� ������� ��������� �� ����
			GetNPCPos(npcid,tmp[0],tmp[1],tmp[2]);
			// �������� ����� ������
			startnodeid = nodes_GetNodeid(VEHICLE_NODES,tmp[0],tmp[1],tmp[2]);
			// �������� ����� ����������
			endnodeid = nodes_GetNodeid(VEHICLE_NODES,x,y,z);
		}
		case NPC_STATE_DRIVE_BY_NODES:
		{
			// NPC �������� �� �����, ����� ����� � ������� �� ������ ������ ���� NPC ��� ������ ����
			startnodeid = paths_GetPathPointNodeid(npcid,npc_step[npcid] - 1);
			// �������� ����� ����������
			endnodeid = nodes_GetNodeid(VEHICLE_NODES,x,y,z);
			// �������� ����� ����������, �������� ���� �� ����� ��������� ������
			if(endnodeid == paths_GetPathPointNodeid(npcid,paths_GetPathLen(npcid) - 1)) return 1; // NPC � ��� �������� � ������ �����
			// ������ ������ ����
			paths_ResetData(npcid);
		}
		default: return 0; // �� ���������� ���������
	}
	// ��������� ���� �� ������������� � ���������� ����
	if(startnodeid == endnodeid) 
	{
		// NPC ��� �� ����� ����������
		controller_ResetData(npcid);
		return 1;
	}
	// ��������� ����
	paths_CalcPathForNPC(npcid,VEHICLE_NODES,startnodeid,endnodeid);
	// ������������� ������ ��� ��������
	npc_state{npcid} = NPC_STATE_DRIVE_BY_NODES;
	npc_speed[npcid] = speed;
	npc_boost{npcid} = boost;
	npc_step[npcid] = 0;
	// ������
	return 1;
}

/*
	Name: 
		controller_DriveToByExistPath(npcid,Float:speed,boost)
	Description:
		NPC ����� ��������� �� ������������� �� ��� ����
	Params:
		npcid - id NPC
		speed - ������� �������� ��������
		boost - ������� ��������� ��� ���������� �� ����
	Return:
		1 - ���� ���������� � NPC �������� �� ����
		0 - ���� �� ����������
*/
stock controller_DriveToByExistPath(npcid,Float:speed,boost)
{
	new Float:tmp[7];
	
	if(!paths_GetPathLen(npcid)) return 0;
	// ������������� ������ ��� ��������
	npc_state{npcid} = NPC_STATE_DRIVE_BY_NODES;
	npc_speed[npcid] = speed;
	npc_boost{npcid} = boost;
	npc_step[npcid] = 0;
	// �������� ������� �����
	GetNPCPos(npcid,tmp[0],tmp[1],tmp[2]);
	// ���� ����� �������� �� �������
	do
	{
		// �������� ���������� ��������
		paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
		// �������� ����������
		tmp[3] -= tmp[0];
		tmp[4] -= tmp[1];
		tmp[5] -= tmp[2];
		tmp[6] = floatsqroot((tmp[3]*tmp[3]) + (tmp[4]*tmp[4]) + (tmp[5]*tmp[5]));
		// ������ ��������� �����
		npc_step[npcid]++;
	}
	while(tmp[6] < 1.0);
	// ������� �������� �������� � �����
	new Float:drivespeed = npc_speed[npcid] + ((paths_GetPathDist(npcid) / 100.0) * npc_boost{npcid});
	// �������� ��������
	paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
	NPC_DriveTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,drivespeed,MOVING_BY_ZMAP);
	// ��������� ��������� ���
	npc_step[npcid]++;
	return 1;
}

/*
	Name: 
		controller_DriveToByCoords(npcid,Float:speed,boost)
	Description:
		�������� � ���������� �� ����������� ���������� � ������
	Params:
		npcid - id NPC
		speed - ������� �������� ��������
		boost - ������� ��������� ��� ���������� �� ����
	Return:
		1 - NPC ��������
		0 - ������
*/
stock controller_DriveToByCoords(npcid,Float:speed,boost)
{
	new Float:tmp[3];
	
	// �������� ���������� ����� � ������
	new count = controller_GetPathCoordsLen(npcid);
	// ��������� ������
	if(count == 0) return 0;
	// ������������� ������ ��� ��������
	npc_state{npcid} = NPC_STATE_DRIVE_BY_COORDS;
	npc_speed[npcid] = speed;
	npc_boost{npcid} = boost;
	npc_step[npcid] = 0;
	SetPVarFloat(npcid,"controller_complete_dist",0.0);
	// �������� ���������� ������� ��������
	controller_GetPathCoordsData(npcid,npc_step[npcid],tmp[0],tmp[1],tmp[2]);
	// ������� �������� �������� � �����
	new Float:drivespeed = npc_speed[npcid] + ((controller_GetPathCoordsDist(npcid) / 100.0) * npc_boost{npcid});
	// �������� ��������
	NPC_DriveTo(npcid,tmp[0],tmp[1],tmp[2] + 1.0,drivespeed,MOVING_BY_ZMAP);
	// ��������� ��������� ���
	npc_step[npcid]++;
	// ������
	return 1;
}

/*
	Name: 
		controller_MoveToByNodes(npcid,Float:x,Float:y,Float:z,move_type)
	Description:
		��������� ����������� ���� �� ������� ������� NPC �� ��������� ��������� �� �������������� �����, � �������� ������ �� ����.
	Params:
		npcid - id NPC
		x,y,z - ���������� ����
		move_type - ��� �������� (���, ���, ������)
	Return:
		1 - ���� �������� � NPC �������� �� ����
		0 - ������ ���������� ����
*/
stock controller_MoveToByNodes(npcid,Float:x,Float:y,Float:z,move_type)
{
	new Float:tmp[7],
		startnodeid,
		endnodeid;
	
	switch(npc_state{npcid})
	{
		case NPC_STATE_NOT_CONTROLLED:
		{
			// NPC �� �������� �� ����, ��������� ���� �� ������� ��������� �� ����
			GetNPCPos(npcid,tmp[0],tmp[1],tmp[2]);
			// �������� ����� ������
			startnodeid = nodes_GetNodeid(PED_NODES,tmp[0],tmp[1],tmp[2]);
			// �������� ����� ����������
			endnodeid = nodes_GetNodeid(PED_NODES,x,y,z);
		}
		case NPC_STATE_MOVE_BY_NODES:
		{
			// NPC �������� �� �����, ����� ����� � ������� �� ������ ������ ���� NPC ��� ������ ����
			startnodeid = paths_GetPathPointNodeid(npcid,npc_step[npcid] - 1);
			// �������� ����� ����������
			endnodeid = nodes_GetNodeid(PED_NODES,x,y,z);
			// �������� ����� ����������, �������� ���� �� ����� ��������� ������
			if(endnodeid == paths_GetPathPointNodeid(npcid,paths_GetPathLen(npcid) - 1)) return 1; // NPC � ��� �������� � ������ �����
			// ������ ������ ����
			paths_ResetData(npcid);
		}
		default: return 0; // �� ���������� ���������
	}
	// ��������� ���� �� ������������� � ���������� ����
	if(startnodeid == endnodeid) 
	{
		// NPC ��� �� ����� ����������
		controller_ResetData(npcid);
		return 1;
	}
	// ��������� ����
	paths_CalcPathForNPC(npcid,PED_NODES,startnodeid,endnodeid);
	// ������������� ������ ��� ��������
	npc_state{npcid} = NPC_STATE_MOVE_BY_NODES;
	npc_boost{npcid} = move_type;
	npc_step[npcid] = 0;
	// ������
	return 1;
}

/*
	Name: 
		controller_MoveToByExistPath(npcid,move_type)
	Description:
		NPC ����� ��������� �� ������������� �� ��� ����
	Params:
		npcid - id NPC
		move_type - ��� �������� (���, ���, ������)
	Return:
		1 - ���� ���������� � NPC �������� �� ����
		0 - ���� �� ����������
*/
stock controller_MoveToByExistPath(npcid,move_type)
{
	new Float:tmp[7];
	
	if(!paths_GetPathLen(npcid)) return 0;
	// ������������� ������ ��� ��������
	npc_state{npcid} = NPC_STATE_MOVE_BY_NODES;
	npc_boost{npcid} = move_type;
	npc_step[npcid] = 0;
	// �������� ������� �����
	GetNPCPos(npcid,tmp[0],tmp[1],tmp[2]);
	// ���� ����� �������� �� �������
	do
	{
		// �������� ���������� ��������
		paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
		// �������� ����������
		tmp[3] -= tmp[0];
		tmp[4] -= tmp[1];
		tmp[5] -= tmp[2];
		tmp[6] = floatsqroot((tmp[3]*tmp[3]) + (tmp[4]*tmp[4]) + (tmp[5]*tmp[5]));
		// ������ ��������� �����
		npc_step[npcid]++;
	}
	while(tmp[6] < 1.0);
	// �������� ��������
	paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
	switch(npc_boost{npcid})
	{
		case NPC_MOVE_PED_WALK: NPC_WalkTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
		case NPC_MOVE_PED_RUN: NPC_RunTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
		case NPC_MOVE_PED_SPRINT: NPC_SprintTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
	}
	// ��������� ��������� ���
	npc_step[npcid]++;
	return 1;
}

/*
	Name: 
		controller_MoveToByCoords(npcid,move_type)
	Description:
		�������� ������ �� ����������� ���������� � ������
	Params:
		npcid - id NPC
		move_type - ��� �������� (���, ���, ������)
	Return:
		1 - NPC ��������
		0 - ������
*/
stock controller_MoveToByCoords(npcid,move_type)
{
	new Float:tmp[3];
	
	// �������� ���������� ����� � ������
	new count = controller_GetPathCoordsLen(npcid);
	// ��������� ������
	if(count == 0) return 0;
	// ������������� ������ ��� ��������
	npc_state{npcid} = NPC_STATE_MOVE_BY_COORDS;
	npc_boost{npcid} = move_type;
	npc_step[npcid] = 0;
	SetPVarFloat(npcid,"controller_complete_dist",0.0);
	// �������� ���������� ������� ��������
	controller_GetPathCoordsData(npcid,npc_step[npcid],tmp[0],tmp[1],tmp[2]);
	// �������� ��������
	switch(npc_boost{npcid})
	{
		case NPC_MOVE_PED_WALK: NPC_WalkTo(npcid,tmp[0],tmp[1],tmp[2],MOVING_BY_ZMAP);
		case NPC_MOVE_PED_RUN: NPC_RunTo(npcid,tmp[0],tmp[1],tmp[2],MOVING_BY_ZMAP);
		case NPC_MOVE_PED_SPRINT: NPC_SprintTo(npcid,tmp[0],tmp[1],tmp[2],MOVING_BY_ZMAP);
	}
	// ��������� ��������� ���
	npc_step[npcid]++;
	// ������
	return 1;
}

/*
	Name: 
		controller_FollowPlayerByNodes(npcid,playerid,move_type)
	Description:
		��������� ������ �� ������� �� �������������� �����
	Params:
		npcid - id NPC
		playerid - id ������ �� ��� ���������
		move_type - ��� �������� (���, ���, ������)
	Return:
		1 - NPC ��������
		0 - ������
*/
stock controller_FollowPlayerByNodes(npcid,playerid,move_type)
{
	new Float:tmp[7],
		startnodeid,
		endnodeid;
	
	switch(npc_state{npcid})
	{
		case NPC_STATE_NOT_CONTROLLED:
		{
			// NPC �� �������� �� ����, ��������� ���� �� ������� ��������� �� ����
			// �������� ����� ������
			GetNPCPos(npcid,tmp[0],tmp[1],tmp[2]);
			startnodeid = nodes_GetNodeid(PED_NODES,tmp[0],tmp[1],tmp[2]);
			// �������� ����� ����������
			GetPlayerPos(playerid,tmp[0],tmp[1],tmp[2]);
			endnodeid = nodes_GetNodeid(PED_NODES,tmp[0],tmp[1],tmp[2]);
		}
		default: return 0; // �� ���������� ���������
	}
	
	if(startnodeid != endnodeid)
	{
		nodes_GetNodePos(PED_NODES,endnodeid,tmp[0],tmp[1],tmp[2]);
		// ���� �����
		new lcount = nodes_GetNodeLinkCount(PED_NODES,startnodeid);
		new clid;
		new Float:cld = 65000.0;
		for(new lid = 0;lid < lcount;lid++)
		{
			new nodeid = nodes_GetNodeLink(PED_NODES,startnodeid,lid);
			nodes_GetNodePos(PED_NODES,nodeid,tmp[3],tmp[4],tmp[5]);
			
			tmp[3] -= tmp[0];
			tmp[4] -= tmp[1];
			tmp[5] -= tmp[2];
			
			new Float:dist = floatsqroot(tmp[3]*tmp[3] + tmp[4]*tmp[4] + tmp[5]*tmp[5]);
			
			if(dist < cld)
			{
				clid = nodeid;
				cld = dist;
			}
		}
		
		// ������������ � ����� ���� ������� ����� ���� � ����������
		nodes_GetNodePos(PED_NODES,clid,tmp[3],tmp[4],tmp[5]);
		switch(move_type)
		{
			case NPC_MOVE_PED_WALK: NPC_WalkTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
			case NPC_MOVE_PED_RUN: NPC_RunTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
			case NPC_MOVE_PED_SPRINT: NPC_SprintTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
		}
		npc_target_node[npcid] = clid;
	}
	else npc_target_node[npcid] = -1;
	
	// ������������� ������ ��� ��������
	npc_state{npcid} = NPC_STATE_FOLLOW_BY_NODES;
	npc_boost{npcid} = move_type;
	npc_target[npcid] = playerid;
	npc_step[npcid] = 0;
	// ������
	return 1;
}

/*
	Name: 
		controller_IsMoving(npcid)
	Description:
		��������� �������� �� NPC
	Params:
		npcid - id NPC
	Return:
		1 - NPC ��������
		0 - NPC �� ��������
*/
stock controller_IsMoving(npcid)
{	
	return (npc_state{npcid} != NPC_STATE_NOT_CONTROLLED);
}

// --------------------------------------------------
// obligatory functions
// --------------------------------------------------
controller_OnCompCalcPathForNPC(npcid)
{
	new Float:tmp[7];
	
	// ��������� ���������������� �������� ����
	if(paths_GetPathLen(npcid) == 0)
	{
		// ���� �� ������
		controller_ResetData(npcid);
		return;
	}	
	// �������� ���������� ����������
	switch(npc_state{npcid})
	{
		case NPC_STATE_DRIVE_BY_NODES:
		{
			// �������� ������� �����
			GetNPCPos(npcid,tmp[0],tmp[1],tmp[2]);
			// ���� ����� �������� �� �������
			do
			{
				// �������� ���������� ��������
				paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
				// �������� ����������
				tmp[3] -= tmp[0];
				tmp[4] -= tmp[1];
				tmp[5] -= tmp[2];
				tmp[6] = floatsqroot((tmp[3]*tmp[3]) + (tmp[4]*tmp[4]) + (tmp[5]*tmp[5]));
				// ������ ��������� �����
				npc_step[npcid]++;
			}
			while(tmp[6] < 1.0);
			// ������� �������� �������� � �����
			new Float:drivespeed = npc_speed[npcid] + ((paths_GetPathDist(npcid) / 100.0) * npc_boost{npcid});
			// �������� ��������
			paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
			NPC_DriveTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,drivespeed,MOVING_BY_ZMAP);
			// ��������� ��������� ���
			npc_step[npcid]++;
		}
		case NPC_STATE_MOVE_BY_NODES:
		{
			// �������� ������� �����
			GetNPCPos(npcid,tmp[0],tmp[1],tmp[2]);
			// ���� ����� �������� �� �������
			do
			{
				// �������� ���������� ��������
				paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
				// �������� ����������
				tmp[3] -= tmp[0];
				tmp[4] -= tmp[1];
				tmp[5] -= tmp[2];
				tmp[6] = floatsqroot((tmp[3]*tmp[3]) + (tmp[4]*tmp[4]) + (tmp[5]*tmp[5]));
				// ������ ��������� �����
				npc_step[npcid]++;
			}
			while(tmp[6] < 1.0);
			// �������� ��������
			paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
			switch(npc_boost{npcid})
			{
				case NPC_MOVE_PED_WALK: NPC_WalkTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
				case NPC_MOVE_PED_RUN: NPC_RunTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
				case NPC_MOVE_PED_SPRINT: NPC_SprintTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
			}
			// ��������� ��������� ���
			npc_step[npcid]++;
		}
	}
}

controller_OnNPCMovingComplete(npcid)
{
	new startnodeid;
	new endnodeid;
	new Float:tmp[6];
	// ��������� �������������� �� NPC
	switch(npc_state{npcid})
	{
		case NPC_STATE_DRIVE_BY_NODES:
		{
			new pathlen = paths_GetPathLen(npcid);
			// ��������� �������� �� ����� ����
			if(npc_step[npcid] == pathlen)
			{
				// ����� ���� ���������
				// ������ ������
				controller_ResetData(npcid);
			}
			else
			{
				// ����� ���� �� ���������
				new Float:dist = (paths_GetPathDist(npcid) / pathlen) * (pathlen - npc_step[npcid]);
				// ������� �������� �������� � �����
				new Float:drivespeed = npc_speed[npcid] + ((dist / 100.0) * npc_boost{npcid});
				// �������� ���������� ����������
				paths_GetPathPointPos(npcid,npc_step[npcid],tmp[0],tmp[1],tmp[2]);
				// �������� ��������
				NPC_DriveTo(npcid,tmp[0],tmp[1],tmp[2] + 1.0,drivespeed,MOVING_BY_ZMAP);
				// ��������� ��������� ���
				npc_step[npcid]++;
			}
		}
		case NPC_STATE_DRIVE_BY_COORDS:
		{
			new pathlen = controller_GetPathCoordsLen(npcid);
			// ��������� �������� �� ����� ����
			if(npc_step[npcid] == pathlen)
			{
				// ����� ���� ���������
				// ������ ������
				controller_ResetData(npcid);
			}
			else
			{
				// ����� ���� �� ���������
				new Float:dist = (controller_GetPathCoordsDist(npcid) / pathlen) * (pathlen - npc_step[npcid]);
				// ������� �������� �������� � �����
				new Float:drivespeed = npc_speed[npcid] + ((dist / 100.0) * npc_boost{npcid});
				// �������� ���������� ����������
				controller_GetPathCoordsData(npcid,npc_step[npcid],tmp[0],tmp[1],tmp[2]);
				// �������� ��������
				NPC_DriveTo(npcid,tmp[0],tmp[1],tmp[2] + 1.0,drivespeed,MOVING_BY_ZMAP);
				// ��������� ��������� ���
				npc_step[npcid]++;
			}
		}
		case NPC_STATE_MOVE_BY_NODES:
		{
			new pathlen = paths_GetPathLen(npcid);
			// ��������� �������� �� ����� ����
			if(npc_step[npcid] == pathlen)
			{
				// ����� ���� ���������
				// ������ ������
				controller_ResetData(npcid);
			}
			else
			{
				// �������� ���������� ����������
				paths_GetPathPointPos(npcid,npc_step[npcid],tmp[0],tmp[1],tmp[2]);
				// �������� ��������
				switch(npc_boost{npcid})
				{
					case NPC_MOVE_PED_WALK: NPC_WalkTo(npcid,tmp[0],tmp[1],tmp[2] + 1.0,MOVING_BY_ZMAP);
					case NPC_MOVE_PED_RUN: NPC_RunTo(npcid,tmp[0],tmp[1],tmp[2] + 1.0,MOVING_BY_ZMAP);
					case NPC_MOVE_PED_SPRINT: NPC_SprintTo(npcid,tmp[0],tmp[1],tmp[2] + 1.0,MOVING_BY_ZMAP);
				}
				// ��������� ��������� ���
				npc_step[npcid]++;
			}
		}
		case NPC_STATE_MOVE_BY_COORDS:
		{
			new pathlen = controller_GetPathCoordsLen(npcid);
			// ��������� �������� �� ����� ����
			if(npc_step[npcid] == pathlen)
			{
				// ����� ���� ���������
				// ������ ������
				controller_ResetData(npcid);
			}
			else
			{
				// �������� ���������� ����������
				controller_GetPathCoordsData(npcid,npc_step[npcid],tmp[0],tmp[1],tmp[2]);
				// �������� ��������
				switch(npc_boost{npcid})
				{
					case NPC_MOVE_PED_WALK: NPC_WalkTo(npcid,tmp[0],tmp[1],tmp[2],MOVING_BY_ZMAP);
					case NPC_MOVE_PED_RUN: NPC_RunTo(npcid,tmp[0],tmp[1],tmp[2],MOVING_BY_ZMAP);
					case NPC_MOVE_PED_SPRINT: NPC_SprintTo(npcid,tmp[0],tmp[1],tmp[2],MOVING_BY_ZMAP);
				}
				// ��������� ��������� ���
				npc_step[npcid]++;
			}
		}
		case NPC_STATE_FOLLOW_BY_NODES:
		{
			startnodeid = npc_target_node[npcid];
			// �������� ����� ����������
			GetPlayerPos(npc_target[npcid],tmp[0],tmp[1],tmp[2]);
			endnodeid = nodes_GetNodeid(PED_NODES,tmp[0],tmp[1],tmp[2]);
			nodes_GetNodePos(PED_NODES,endnodeid,tmp[0],tmp[1],tmp[2]);
			
			if(startnodeid != endnodeid)
			{
				// ���� �����
				new lcount = nodes_GetNodeLinkCount(PED_NODES,startnodeid);
				new clid;
				new Float:cld = 65000.0;
				for(new lid = 0;lid < lcount;lid++)
				{
					new nodeid = nodes_GetNodeLink(PED_NODES,startnodeid,lid);
					nodes_GetNodePos(PED_NODES,nodeid,tmp[3],tmp[4],tmp[5]);
					
					tmp[3] -= tmp[0];
					tmp[4] -= tmp[1];
					tmp[5] -= tmp[2];
					
					new Float:dist = floatsqroot(tmp[3]*tmp[3] + tmp[4]*tmp[4] + tmp[5]*tmp[5]);
					
					if(dist < cld)
					{
						clid = nodeid;
						cld = dist;
					}
				}
			
				// ������������ � ����� ���� ������� ����� ���� � ����������
				nodes_GetNodePos(PED_NODES,clid,tmp[3],tmp[4],tmp[5]);
				switch(npc_boost{npcid})
				{
					case NPC_MOVE_PED_WALK: NPC_WalkTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
					case NPC_MOVE_PED_RUN: NPC_RunTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
					case NPC_MOVE_PED_SPRINT: NPC_SprintTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
				}
				
				npc_target_node[npcid] = clid;
			}
			else npc_target_node[npcid] = -1;
		}
	}
}

controller_OnNPCSpawn(npcid)
{
	controller_ResetData(npcid);
}

controller_OnNPCDeath(npcid,killerid,reason)
{
	#pragma unused killerid
	#pragma unused reason
	controller_ResetData(npcid);
}

controller_OnPlayerUpdate(playerid)
{
	new bool:dexist = false; // ���� ����� �� ������ ������
	new Float:tmp[6];
	new startnodeid;
	new endnodeid;
	
	for(new i = 0;i < MAX_NPCS;i++)
	{
		if(npc_state{i} != NPC_STATE_FOLLOW_BY_NODES) continue;
		if(npc_target[i] != playerid) continue;
		
		// ��������� �������� �� ��� ������
		if(!dexist)
		{
			// ������ �� ��������, �������� ��� � �������� � ������ ������ �����
			GetPlayerPos(playerid,tmp[0],tmp[1],tmp[2]);
			endnodeid = nodes_GetNodeid(PED_NODES,tmp[0],tmp[1],tmp[2]);
			dexist = true;
		}
		
		// ��������� ��
		if(npc_target_node[i] == -1)
		{
			// �������� ����� ������
			GetNPCPos(i,tmp[0],tmp[1],tmp[2]);
			startnodeid = nodes_GetNodeid(PED_NODES,tmp[0],tmp[1],tmp[2]);
			
			if(startnodeid != endnodeid)
			{
				nodes_GetNodePos(PED_NODES,endnodeid,tmp[0],tmp[1],tmp[2]);
				// ���� �����
				new lcount = nodes_GetNodeLinkCount(PED_NODES,startnodeid);
				new clid;
				new Float:cld = 65000.0;
				for(new lid = 0;lid < lcount;lid++)
				{
					new nodeid = nodes_GetNodeLink(PED_NODES,startnodeid,lid);
					nodes_GetNodePos(PED_NODES,nodeid,tmp[3],tmp[4],tmp[5]);
					
					tmp[3] -= tmp[0];
					tmp[4] -= tmp[1];
					tmp[5] -= tmp[2];
					
					new Float:dist = floatsqroot(tmp[3]*tmp[3] + tmp[4]*tmp[4] + tmp[5]*tmp[5]);
					
					if(dist < cld)
					{
						clid = nodeid;
						cld = dist;
					}
				}
				
				// ������������ � ����� ���� ������� ����� ���� � ����������
				nodes_GetNodePos(PED_NODES,clid,tmp[3],tmp[4],tmp[5]);
				switch(npc_boost{i})
				{
					case NPC_MOVE_PED_WALK: NPC_WalkTo(i,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
					case NPC_MOVE_PED_RUN: NPC_RunTo(i,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
					case NPC_MOVE_PED_SPRINT: NPC_SprintTo(i,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
				}
				npc_target_node[i] = clid;
			}
		}
	}
}