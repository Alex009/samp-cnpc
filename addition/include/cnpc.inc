/*
*	Created:			09.03.10
*	Author:				009
*	Last Modifed:		18.08.12
*	Description:		controll your NPC
*/


#if defined _cnpc_included
	#endinput
#endif


#define _cnpc_included
#pragma library cnpc


//
//	Defines
//
#define MICRO_IN_SEC 							1000000

#define MAX_NPCS								500
#define MAX_NODES								500
#define MAX_NODE_PATHS							500

#define NPC_STATE_REQUEST_CLASS					PLAYER_STATE_SPECTATING
#define NPC_STATE_ONFOOT						PLAYER_STATE_ONFOOT
#define NPC_STATE_DRIVER						PLAYER_STATE_DRIVER
#define NPC_STATE_PASSANGER						PLAYER_STATE_PASSENGER
#define NPC_STATE_DEATH							PLAYER_STATE_WASTED

#define PLAYER_RECORDING_TYPE_NONE				0
#define PLAYER_RECORDING_TYPE_DRIVER			1
#define PLAYER_RECORDING_TYPE_ONFOOT			2


#define	NPC_RECORD_END_REASON_END				1
#define	NPC_RECORD_END_REASON_DEATH				2
#define	NPC_RECORD_END_REASON_STOP				3
#define	NPC_RECORD_END_REASON_DESTROY			4

#define ZMAP_MODE_NOCHACHE						0
#define ZMAP_MODE_CHACHE						1

#define INVALID_NODE_ID							0xFFFFFFFF
// типы путей
#define VEHICLE_NODES							1
#define PED_NODES								2

#define INVALID_NODE_PATH						0xFFFF


#define SPEED_WALK			0.03
#define SPEED_RUN			0.08
#define SPEED_SPRINT		0.12

//
//	Natives
//


// core
native CreateNPC(npcid,npcname[]);
native DestroyNPC(npcid);
native IsValidNPC(npcid);
native KillNPC(npcid);
native SpawnNPC(npcid);
native StopNPC(npcid);
native GetNPCState(npcid);
native ToogleVisualDeath(tstate);
native FindLastFreeSlot();
// on foot controlls
native SetNPCPos(npcid,Float:X,Float:Y,Float:Z);
native GetNPCPos(npcid,&Float:X,&Float:Y,&Float:Z);
native SetNPCQuaternion(npcid,Float:X,Float:Y,Float:Z,Float:S);
native GetNPCQuaternion(npcid,&Float:X,&Float:Y,&Float:Z,&Float:S);
native SetNPCFacingAngle(npcid,Float:angle);
native Float:GetNPCFacingAngle(npcid);
native SetNPCVelocity(npcid,Float:X,Float:Y,Float:Z);
native GetNPCVelocity(npcid,&Float:X,&Float:Y,&Float:Z);
native NPC_WalkTo(npcid,Float:X,Float:Y,Float:Z,is_z_map,Float:speed=SPEED_WALK);
native NPC_RunTo(npcid,Float:X,Float:Y,Float:Z,is_z_map,Float:speed=SPEED_RUN);
native NPC_SprintTo(npcid,Float:X,Float:Y,Float:Z,is_z_map,Float:speed=SPEED_SPRINT);
native NPC_LookAt(npcid,Float:X,Float:Y,Float:Z);
native NPC_AimAt(npcid,Float:X,Float:Y,Float:Z);
native NPC_ShootAt(npcid,Float:X,Float:Y,Float:Z);
native SetNPCCameraPos(npcid,Float:X,Float:Y,Float:Z);
native GetNPCCameraPos(npcid,&Float:X,&Float:Y,&Float:Z);
native SetNPCCameraFrontVector(npcid,Float:X,Float:Y,Float:Z);
native GetNPCCameraFrontVector(npcid,&Float:X,&Float:Y,&Float:Z);
native SetNPCCameraMode(npcid,mode);
native GetNPCCameraMode(npcid);
native SetNPCWeaponState(npcid,wstate);
native GetNPCWeaponState(npcid);
native SetNPCKeys(npcid,updown,leftright,keys);
native GetNPCKeys(npcid,&updown,&leftright,&keys);
native SetNPCWeapon(npcid,weaponid);
native GetNPCWeapon(npcid);
native SetNPCWeaponSkillLevel(npcid,weapontype,level);
native GetNPCWeaponSkillLevel(npcid,weapontype);
native SetNPCHealth(npcid,Float:amount);
native Float:GetNPCHealth(npcid);
native SetNPCArmour(npcid,Float:amount);
native Float:GetNPCArmour(npcid);
native SetNPCInterior(npcid,interior);
native GetNPCInterior(npcid);
native SetNPCSpecialAction(npcid,action);
native GetNPCSpecialAction(npcid);
native SetNPCAnimationIndex(npcid,animationid);
native GetNPCAnimationIndex(npcid);
native SetNPCSkin(npcid,skin);
native GetNPCSkin(npcid);
native SetNPCSurfing(npcid,Float:X,Float:Y,Float:Z);
native GetNPCSurfing(npcid,&Float:X,&Float:Y,&Float:Z);
native SetNPCSurfingVehicle(npcid,vehicleid);
native GetNPCSurfingVehicle(npcid);
native SetNPCImpregnable(npcid,istate);
native IsNPCImpregnable(npcid);
// driver controls
native PutNPCInVehicle(npcid,vehicleid,seat);
native NPC_DriveTo(npcid,Float:X,Float:Y,Float:Z,Float:speed,is_z_map);
native SetNPCVehicleSiren(npcid,vstate);
native GetNPCVehicleSiren(npcid);
native SetNPCVehicleHealth(npcid,health);
native GetNPCVehicleHealth(npcid);
native SetNPCVehicleTrailer(npcid,trailerid);
native GetNPCVehicleTrailer(npcid);
// passanger controls
native SetNPCPassangerDriveBy(npcid,istate);
native GetNPCPassangerDriveBy(npcid);
// damage config
native SetWeaponDamageForNPC(weaponid,Float:damage);
native SetWeaponReloadTimeForNPC(weaponid,mstime);
// .rec playbacks
native StartRecordingPlayback(npcid,name[]);
native PauseRecordingPlayback(npcid);
native ContinueRecordingPlayback(npcid);
native StopRecordingPlayback(npcid);
// GTA SA paths
native Node:OpenNode(name[]);
native CloseNode(Node:nodeid);
native GetNodeHeader(Node:nodeid,&nodes,&vehicle_nodes,&ped_nodes,&navi_nodes);
native SetNodePoint(Node:nodeid,pointid);
native GetNodePoint(Node:nodeid);
native GetNodePointPos(Node:nodeid,&Float:X,&Float:Y,&Float:Z);
native GetNodePointLinkId(Node:nodeid);
native GetNodePointAreaId(Node:nodeid);
native GetNodePointWidth(Node:nodeid);
native GetNodePointLinkCount(Node:pointid);
native GetNodePointTrafficLevel(Node:nodeid);
native IsNodePointRoadBlock(Node:nodeid);
native IsNodePointBoats(Node:nodeid);
native IsNodePointEmergency(Node:nodeid);
native IsNodePointNotHighway(Node:nodeid);
native IsNodePointSpawn(Node:nodeid);
native IsNodePointRoadBlock1(Node:nodeid);
native IsNodePointParking(Node:nodeid);
native IsNodePointRoadBlock2(Node:nodeid);
native GetNodePointType(Node:nodeid);
native SetNodeLink(Node:nodeid,linkid);
native GetNodeLinkAreaId(Node:nodeid);
native GetNodeLinkNodeId(Node:nodeid);
// Nodes manager
native nodes_Init();
native nodes_Exit();
native nodes_GetNodePos(nodetype,nodeid,&Float:x,&Float:y,&Float:z);
native nodes_GetNodeAreaid(nodetype,nodeid);
native nodes_GetNodeNodeid(nodetype,nodeid);
native nodes_GetNodeLink(nodetype,nodeid,linkid);
native Float:nodes_GetNodeLinkDist(nodetype,nodeid,linkid);
native nodes_GetNodeLinkCount(nodetype,nodeid);
native nodes_GetNodeCount(nodetype);
native nodes_GetNodeid(nodetype,Float:x,Float:y,Float:z);
native nodes_GetAreaid(Float:fx,Float:fy,Float:fz);
native nodes_GetAreasFromPos(Float:startx,Float:starty,Float:endx,Float:endy,areas[64]);
native nodes_GetAreaNodeid(nodetype,areaid,nodeid);
native nodes_GetAreaNodeCount(nodetype,areaid);
// dijkstra
native dijkstra_CalcPathByNodes(areas[64],nodetype,startnodeid,endnodeid);
// node paths
native path_Destroy(pathid);
native path_GetPathNodeid(pathid,pointid);
native path_GetPathLen(pathid);
native Float:path_GetPathDist(pathid);
// zmap
native zmap_Init(filename[],mode);
native zmap_Exit();
native Float:zmap_GetZCoord(Float:x,Float:y);
native zmap_IsPointToPointLOS(Float:sx,Float:sy,Float:sz,Float:ex,Float:ey,Float:ez);
// addition
native microtime(&sec,&microsec);

//
//	Callbacks
//
forward OnNPCGetDamage(npcid,playerid,Float:health_loss,bodypart);
forward OnRecordingPlaybackEnd(npcid,reason);
forward OnNPCMovingComplete(npcid);
forward OnNPCSpawn(npcid);
forward OnNPCDeath(npcid,killerid,reason);
forward OnPathCalculated(pathid);