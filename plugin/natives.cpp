/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: natives for pawn
 */

#define _USE_MATH_DEFINES

#include <stdio.h>
#include <malloc.h>
#include <math.h>
// SDK
#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"
// plugin
#include "CNPC.h"
#include "Nodes.h"
#include "hooks.h"
#include "callbacks.h"
#include "SAMP/CSAMP.h"
#include "defines.h"
// debug
#include "utils/quaternion.h"

extern	CNPC*		pNPC[MAX_NPCS];
extern	CNode*		Nodes[MAX_NODES];
extern	CSAMP*		pSaMp;
extern	CHooks*		pHooks;
extern	CCallbacks*	pCallbacks;
extern	char		VisualDeath;
extern	int			WeaponReload[MAX_PLAYERS];
extern	float		WeaponsDamage[MAX_WEAPONS + 2];
extern	int			WeaponsReloadTime[MAX_WEAPONS + 1];
// for nodes module
#define MAX_VEHICLE_NODES_TO_LOAD	31000
#define MAX_VEHICLE_NODES_IN_FILE	1000
#define MAX_PED_NODES_TO_LOAD		38000
#define MAX_PED_NODES_IN_FILE		2500
#define INVALID_NODE_ID				-1
// типы путей
#define VEHICLE_NODES				1
#define PED_NODES					2

struct NodeInfo						// данные каждой точки навигационного графа
{
	float			ni_pos[3];		// позиция	
	unsigned char	ni_areaid;		// номер квадрата точки
	unsigned short	ni_nodeid;		// номер точки в квадрате
	unsigned short*	ni_links;		// связи
	float*			ni_links_dist;	// расстояние от точки до её связей
	unsigned char	ni_linkcount;	// количество связей
};
NodeInfo		node_vehicle[MAX_VEHICLE_NODES_TO_LOAD];			// массив точек транспортного навигационного графа
unsigned short	node_vehicle_link[64][MAX_VEHICLE_NODES_IN_FILE];	// массив ссылок точек в файлах путей (64), на точки в общем массиве транспортного навигационного графа
unsigned short	node_vehicle_count;									// количество загруженных точек транспортного навигационного графа
NodeInfo		node_ped[MAX_PED_NODES_TO_LOAD];					// массив точек пешеходного навигационного графа
unsigned short	node_ped_link[64][MAX_PED_NODES_IN_FILE];			// массив ссылок точек в файлах путей (64), на точки в общем массиве пешеходного навигационного графа
unsigned short	node_ped_count;										// количество загруженных точек пешеходного навигационного графа
// for dijkstra module
struct dijkstra_NodeInfo
{
	float			ni_dist;		// расстояние от начала пути
	unsigned short	ni_parentid;	// номер точки из которой пришли сюда самым коротким путём от начала пути
	unsigned short	ni_step;		// шаг от начала пути
};
dijkstra_NodeInfo	node_vehicle[MAX_VEHICLE_NODES_TO_LOAD];		// массив данных точек для поиска пути
bool				node_vehicle_check[MAX_VEHICLE_NODES_TO_LOAD];	// массив флагов проверена ли точка (для поиска пути)
dijkstra_NodeInfo	node_ped[MAX_PED_NODES_TO_LOAD];				// массив данных точек для поиска пути	
bool				node_ped_check[MAX_PED_NODES_TO_LOAD];			// массив флагов проверена ли точка (для поиска пути)

// CreateNPC(NPC:npcid,npcname[]);
static cell AMX_NATIVE_CALL n_CreateNPC( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	char* temp;
	amx_StrParam(amx, params[2], temp);
	if(!pNPC[npcid])
	{
		pNPC[npcid] = new CNPC(npcid,temp);
		return 1;
	}
	return 0;
}
// DestroyNPC(NPC:npcid);
static cell AMX_NATIVE_CALL n_DestroyNPC( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		delete pNPC[npcid];
		pNPC[npcid] = 0;
		return 1;
	}
	return 0;
}
// IsValidNPC(NPC:npcid);
static cell AMX_NATIVE_CALL n_IsValidNPC( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid]) return 1;
	return 0;
}
// GetNPCState(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCState( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid]) return pNPC[npcid]->GetState();
	return 0;
}
// SetNPCPos(NPC:npcid,Float:X,Float:Y,Float:Z);
static cell AMX_NATIVE_CALL n_SetNPCPos( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x = amx_ctof(params[2]);
		float y = amx_ctof(params[3]);
		float z = amx_ctof(params[4]);
		pNPC[npcid]->SetPosition(x,y,z);
		return 1;
	}
	return 0;
}
// GetNPCPos(NPC:npcid,&Float:X,&Float:Y,&Float:Z);
static cell AMX_NATIVE_CALL n_GetNPCPos( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x,y,z;
		pNPC[npcid]->GetPosition(&x,&y,&z);
		cell
			* p1,
			* p2,
			* p3;
		amx_GetAddr(amx, params[2], &p1);
		amx_GetAddr(amx, params[3], &p2);
		amx_GetAddr(amx, params[4], &p3);
		*p1 = amx_ftoc(x);
		*p2 = amx_ftoc(y);
		*p3 = amx_ftoc(z);
		return 1;
	}
	return 0;
}
// SetNPCQuaternion(NPC:npcid,Float:X,Float:Y,Float:Z,Float:Scal);
static cell AMX_NATIVE_CALL n_SetNPCQuaternion( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x = amx_ctof(params[2]);
		float y = amx_ctof(params[3]);
		float z = amx_ctof(params[4]);
		float s = amx_ctof(params[4]);
		pNPC[npcid]->SetQuaternion(x,y,z,s);
		return 1;
	}
	return 0;
}
// GetNPCQuaternion(NPC:npcid,&Float:X,&Float:Y,&Float:Z,&Float:Scal);
static cell AMX_NATIVE_CALL n_GetNPCQuaternion( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x,y,z,s;
		pNPC[npcid]->GetQuaternion(&x,&y,&z,&s);
		cell
			* p1,
			* p2,
			* p3,
			* p4;
		amx_GetAddr(amx, params[2], &p1);
		amx_GetAddr(amx, params[3], &p2);
		amx_GetAddr(amx, params[4], &p3);
		amx_GetAddr(amx, params[5], &p4);
		*p1 = amx_ftoc(x);
		*p2 = amx_ftoc(y);
		*p3 = amx_ftoc(z);
		*p4 = amx_ftoc(s);
		return 1;
	}
	return 0;
}
// SetNPCFacingAngle(NPC:npcid,Float:angle);
static cell AMX_NATIVE_CALL n_SetNPCFacingAngle( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->SetAngle(amx_ctof(params[2]));
		return 1;
	}
	return 0;
}
// Float:GetNPCFacingAngle(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCFacingAngle( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float a = pNPC[npcid]->GetAngle();
		return amx_ftoc(a);
	}
	return 0;
}
// SetNPCVelocity(NPC:npcid,Float:X,Float:Y,Float:Z);
static cell AMX_NATIVE_CALL n_SetNPCVelocity( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x = amx_ctof(params[2]);
		float y = amx_ctof(params[3]);
		float z = amx_ctof(params[4]);
		pNPC[npcid]->SetVelocity(x,y,z);
		return 1;
	}
	return 0;
}
// GetNPCVelocity(NPC:npcid,&Float:X,&Float:Y,&Float:Z);
static cell AMX_NATIVE_CALL n_GetNPCVelocity( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x,y,z;
		pNPC[npcid]->GetVelocity(&x,&y,&z);
		cell
			* p1,
			* p2,
			* p3;
		amx_GetAddr(amx, params[2], &p1);
		amx_GetAddr(amx, params[3], &p2);
		amx_GetAddr(amx, params[4], &p3);
		*p1 = amx_ftoc(x);
		*p2 = amx_ftoc(y);
		*p3 = amx_ftoc(z);
		return 1;
	}
	return 0;
}
// NPC_WalkTo(NPC:npcid,Float:X,Float:Y,Float:Z,is_z_map);
static cell AMX_NATIVE_CALL n_NPC_WalkTo( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x = amx_ctof(params[2]);
		float y = amx_ctof(params[3]);
		float z = amx_ctof(params[4]);
		bool z_map = (params[5]==0?false:true);
		pNPC[npcid]->GoTo(x,y,z,NPC_WALKING_DISTANCE,z_map);
		pNPC[npcid]->SetKeys(KEY_UP,0,KEY_WALK);
		return 1;
	}
	return 0;
}
// NPC_RunTo(NPC:npcid,Float:X,Float:Y,Float:Z,is_z_map);
static cell AMX_NATIVE_CALL n_NPC_RunTo( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x = amx_ctof(params[2]);
		float y = amx_ctof(params[3]);
		float z = amx_ctof(params[4]);
		bool z_map = (params[5]==0?false:true);
		pNPC[npcid]->GoTo(x,y,z,NPC_RUNING_DISTANCE,z_map);
		pNPC[npcid]->SetKeys(KEY_UP,0,0);
		return 1;
	}
	return 0;
}
// NPC_SprintTo(NPC:npcid,Float:X,Float:Y,Float:Z,is_z_map);
static cell AMX_NATIVE_CALL n_NPC_SprintTo( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x = amx_ctof(params[2]);
		float y = amx_ctof(params[3]);
		float z = amx_ctof(params[4]);
		bool z_map = (params[5]==0?false:true);
		pNPC[npcid]->GoTo(x,y,z,NPC_SPRINTING_DISTANCE,z_map);
		pNPC[npcid]->SetKeys(KEY_UP,0,KEY_SPRINT);
		return 1;
	}
	return 0;
}
// NPC_LookAt(NPC:npcid,Float:X,Float:Y,Float:Z);
static cell AMX_NATIVE_CALL n_NPC_LookAt( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x,y,z,tx,ty,tz;
		pNPC[npcid]->GetPosition(&x,&y,&z);
		z += 0.7f;
		tx = amx_ctof(params[2]);
		ty = amx_ctof(params[3]);
		tz = amx_ctof(params[4]);
		// create vector
		tx -= x;
		ty -= y;
		tz -= z;
		// set data
		pNPC[npcid]->SetCameraPos(x,y,z);
		pNPC[npcid]->SetCameraFrontVector(tx,ty,tz);
		pNPC[npcid]->SetCameraMode(53);
		int ud,lr,other;
		pNPC[npcid]->GetKeys(&ud,&lr,&other);
		pNPC[npcid]->SetKeys(ud,lr,0);
		return 1;
	}
	return 0;
}
// NPC_AimAt(NPC:npcid,Float:X,Float:Y,Float:Z);
static cell AMX_NATIVE_CALL n_NPC_AimAt( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x,y,z,tx,ty,tz;
		pNPC[npcid]->GetPosition(&x,&y,&z);
		z += 0.7f;
		tx = amx_ctof(params[2]);
		ty = amx_ctof(params[3]);
		tz = amx_ctof(params[4]);
		// z angle
		float a = (atan2((ty - y),(tx - x)) * 57.2957763671875f) + 270.0f;
		if(a > 360.0) a -= 360.0;
		// create vector
		tx -= x;
		ty -= y;
		tz -= z;
		// z camera angle
		float z_aim = -acos(((tx *tx) + (ty * ty)) / (sqrt((tx * tx) + (ty * ty) + (tz * tz)) * sqrt((tx * tx) + (ty * ty)))) + 0.1f;
		pNPC[npcid]->SetCameraZAim(z_aim);
		// set data
		pNPC[npcid]->SetCameraPos(x,y,z);
		pNPC[npcid]->SetCameraFrontVector(tx,ty,tz);
		int ud,lr,other;
		pNPC[npcid]->GetKeys(&ud,&lr,&other);
		pNPC[npcid]->SetKeys(ud,lr,KEY_HANDBRAKE);
		// camera mode
		switch(pNPC[npcid]->GetWeapon())
		{
		case 0: break;
		// 2 hands
		case WEAPON_COLT45:
		case WEAPON_SAWEDOFF:
		case WEAPON_UZI:
		case WEAPON_TEC9:
			{
				pNPC[npcid]->SetCameraMode(53);
				pNPC[npcid]->SetWeaponState(2);
				break;
			}
		// 1 hand
		default:
			{
				pNPC[npcid]->SetCameraMode(7);
				pNPC[npcid]->SetWeaponState(2);
				// z aim
				pNPC[npcid]->SetAngle(a);
				break;
			}
		}
		return 1;
	}
	return 1;
}
// NPC_ShotAt(NPC:npcid,Float:X,Float:Y,Float:Z);
static cell AMX_NATIVE_CALL n_NPC_ShotAt( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x,y,z,tx,ty,tz;
		pNPC[npcid]->GetPosition(&x,&y,&z);
		z += 0.7f;
		tx = amx_ctof(params[2]);
		ty = amx_ctof(params[3]);
		tz = amx_ctof(params[4]);
		// z angle
		float a = (atan2((ty - y),(tx - x)) * 57.2957763671875f) + 270.0f;
		if(a > 360.0f) a -= 360.0f;
		// create vector
		tx -= x;
		ty -= y;
		tz -= z;
		// z camera angle
		float z_aim = -acos(((tx *tx) + (ty * ty)) / (sqrt((tx * tx) + (ty * ty) + (tz * tz)) * sqrt((tx * tx) + (ty * ty)))) + 0.1f;
		pNPC[npcid]->SetCameraZAim(z_aim);
		// set data
		pNPC[npcid]->SetCameraPos(x,y,z);
		pNPC[npcid]->SetCameraFrontVector(tx,ty,tz);
		// keys
		int ud,lr,other;
		pNPC[npcid]->GetKeys(&ud,&lr,&other);
		pNPC[npcid]->SetKeys(ud,lr,KEY_HANDBRAKE + KEY_FIRE);
		// camera mode
		switch(pNPC[npcid]->GetWeapon())
		{
		case 0: break;
		// 2 hands
		case WEAPON_COLT45:
		case WEAPON_SAWEDOFF:
		case WEAPON_UZI:
		case WEAPON_TEC9:
			{
				pNPC[npcid]->SetCameraMode(53);
				pNPC[npcid]->SetWeaponState(WEAPONSTATE_MORE_BULLETS);
				break;
			}
		// 1 hand
		default:
			{
				pNPC[npcid]->SetCameraMode(7);
				pNPC[npcid]->SetWeaponState(WEAPONSTATE_MORE_BULLETS);
				// z aim
				pNPC[npcid]->SetAngle(a);
				break;
			}
		}
		return 1;
	}
	return 1;
}
// SetNPCCameraPos(NPC:npcid,Float:X,Float:Y,Float:Z);
static cell AMX_NATIVE_CALL n_SetNPCCameraPos( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x = amx_ctof(params[2]);
		float y = amx_ctof(params[3]);
		float z = amx_ctof(params[4]);
		pNPC[npcid]->SetCameraPos(x,y,z);
		return 1;
	}
	return 0;
}
// GetNPCCameraPos(NPC:npcid,&Float:X,&Float:Y,&Float:Z);
static cell AMX_NATIVE_CALL n_GetNPCCameraPos( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x,y,z;
		pNPC[npcid]->GetCameraPos(&x,&y,&z);
		cell
			* p1,
			* p2,
			* p3;
		amx_GetAddr(amx, params[2], &p1);
		amx_GetAddr(amx, params[3], &p2);
		amx_GetAddr(amx, params[4], &p3);
		*p1 = amx_ftoc(x);
		*p2 = amx_ftoc(y);
		*p3 = amx_ftoc(z);
		return 1;
	}
	return 0;
}
// SetNPCCameraFrontVector(NPC:npcid,Float:X,Float:Y,Float:Z);
static cell AMX_NATIVE_CALL n_SetNPCCameraFrontVector( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x = amx_ctof(params[2]);
		float y = amx_ctof(params[3]);
		float z = amx_ctof(params[4]);
		pNPC[npcid]->SetCameraFrontVector(x,y,z);
		return 1;
	}
	return 0;
}
// GetNPCCameraFrontVector(NPC:npcid,&Float:X,&Float:Y,&Float:Z);
static cell AMX_NATIVE_CALL n_GetNPCCameraFrontVector( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x,y,z;
		pNPC[npcid]->GetCameraFrontVector(&x,&y,&z);
		cell
			* p1,
			* p2,
			* p3;
		amx_GetAddr(amx, params[2], &p1);
		amx_GetAddr(amx, params[3], &p2);
		amx_GetAddr(amx, params[4], &p3);
		*p1 = amx_ftoc(x);
		*p2 = amx_ftoc(y);
		*p3 = amx_ftoc(z);
		return 1;
	}
	return 0;
}
// SetNPCCameraMode(NPC:npcid,mode);
static cell AMX_NATIVE_CALL n_SetNPCCameraMode( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->SetCameraMode((int)params[2]);
		return 1;
	}
	return 0;
}
// GetNPCCameraMode(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCCameraMode( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		return pNPC[npcid]->GetCameraMode();
	}
	return 0;
}
// SetNPCWeaponState(NPC:npcid,state);
static cell AMX_NATIVE_CALL n_SetNPCWeaponState( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->SetWeaponState(((int)params[2] << 6));
		return 1;
	}
	return 0;
}
// GetNPCWeaponState(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCWeaponState( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		return pNPC[npcid]->GetWeaponState() >> 6;
	}
	return 0;
}
// SetNPCKeys(NPC:npcid,updown,leftright,keys);
static cell AMX_NATIVE_CALL n_SetNPCKeys( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		int ud = (int)params[2];
		int lr = (int)params[3];
		int other = (int)params[4];
		pNPC[npcid]->SetKeys(ud,lr,other);
		return 1;
	}
	return 0;
}
// GetNPCKeys(NPC:npcid,&updown,&leftright,&keys);
static cell AMX_NATIVE_CALL n_GetNPCKeys( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		int ud,lr,other;
		pNPC[npcid]->GetKeys(&ud,&lr,&other);
		cell
			* p1,
			* p2,
			* p3;
		amx_GetAddr(amx, params[2], &p1);
		amx_GetAddr(amx, params[3], &p2);
		amx_GetAddr(amx, params[4], &p3);
		*p1 = ud;
		*p2 = lr;
		*p3 = other;
		return 1;
	}
	return 0;
}
// SetNPCWeapon(NPC:npcid,weaponid);
static cell AMX_NATIVE_CALL n_SetNPCWeapon( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->SetWeapon((int)params[2]);
		return 1;
	}
	return 0;
}
// GetNPCWeapon(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCWeapon( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		return pNPC[npcid]->GetWeapon();
	}
	return 0;
}
// SetNPCWeaponSkillLevel(NPC:npcid,weapontype,level);
static cell AMX_NATIVE_CALL n_SetNPCWeaponSkillLevel( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->SetWeaponSkillLevel((int)params[2],(int)params[3]);
		return 1;
	}
	return 0;
}
// GetNPCWeaponSkillLevel(NPC:npcid,weapontype);
static cell AMX_NATIVE_CALL n_GetNPCWeaponSkillLevel( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		return pNPC[npcid]->GetWeaponSkillLevel((int)params[2]);
	}
	return 0;
}
// SetNPCHealth(NPC:npcid,Float:health);
static cell AMX_NATIVE_CALL n_SetNPCHealth( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float health = amx_ctof(params[2]);
		pNPC[npcid]->SetHealth(health);
		if(health <= 0.0) pNPC[npcid]->Kill(-1,0);
		return 1;
	}
	return 0;
}
// Float:GetNPCHealth(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCHealth( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float health = pNPC[npcid]->GetHealth();
		return amx_ftoc(health);
	}
	return 0;
}
// SetNPCArmour(NPC:npcid,Float:armour);
static cell AMX_NATIVE_CALL n_SetNPCArmour( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float armour = amx_ctof(params[2]);
		pNPC[npcid]->SetArmour(armour);
		return 1;
	}
	return 0;
}
// Float:GetNPCArmour(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCArmour( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float armour = pNPC[npcid]->GetArmour();
		return amx_ftoc(armour);
	}
	return 0;
}
// SetNPCInterior(NPC:npcid,interior);
static cell AMX_NATIVE_CALL n_SetNPCInterior( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->SetInterior(params[2]);
		return 1;
	}
	return 0;
}
// GetNPCInterior(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCInterior( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		return pNPC[npcid]->GetInterior();
	}
	return 0;
}
// SetNPCSpecialAction(NPC:npcid,interior);
static cell AMX_NATIVE_CALL n_SetNPCSpecialAction( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->SetSpecialAction(params[2]);
		return 1;
	}
	return 0;
}
// GetNPCSpecialAction(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCSpecialAction( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		return pNPC[npcid]->GetSpecialAction();
	}
	return 0;
}
// SetNPCSkin(NPC:npcid,skin);
static cell AMX_NATIVE_CALL n_SetNPCSkin( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->SetSkin(params[2]);
		return 1;
	}
	return 0;
}
// GetNPCSkin(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCSkin( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		return pNPC[npcid]->GetSkin();
	}
	return 0;
}
// SetNPCSurfing(NPC:npcid,Float:X,Float:Y,Float:Z);
static cell AMX_NATIVE_CALL n_SetNPCSurfing( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x = amx_ctof(params[2]);
		float y = amx_ctof(params[3]);
		float z = amx_ctof(params[4]);
		pNPC[npcid]->SetSurfing(x,y,z);
		return 1;
	}
	return 0;
}
// GetNPCSurfing(NPC:npcid,&Float:X,&Float:Y,&Float:Z);
static cell AMX_NATIVE_CALL n_GetNPCSurfing( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x,y,z;
		pNPC[npcid]->GetSurfing(&x,&y,&z);
		cell
			* p1,
			* p2,
			* p3;
		amx_GetAddr(amx, params[2], &p1);
		amx_GetAddr(amx, params[3], &p2);
		amx_GetAddr(amx, params[4], &p3);
		*p1 = amx_ftoc(x);
		*p2 = amx_ftoc(y);
		*p3 = amx_ftoc(z);
		return 1;
	}
	return 0;
}
// SetNPCSurfingVehicle(NPC:npcid,vehicleid);
static cell AMX_NATIVE_CALL n_SetNPCSurfingVehicle( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->SetSurfingVehicle(params[2]);
		return 1;
	}
	return 0;
}
// GetNPCSurfingVehicle(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCSurfingVehicle( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		return pNPC[npcid]->GetSurfingVehicle();
	}
	return 0;
}
// SetNPCImpregnable(NPC:npcid,bool:istate);
static cell AMX_NATIVE_CALL n_SetNPCImpregnable( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		bool state = (params[2]==0?false:true);
		pNPC[npcid]->SetImpregnable(state);
		return 1;
	}
	return 0;
}
// IsNPCImpregnable(NPC:npcid);
static cell AMX_NATIVE_CALL n_IsNPCImpregnable( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		return pNPC[npcid]->IsImpregnable();
	}
	return 0;
}
// KillNPC(NPC:npcid);
static cell AMX_NATIVE_CALL n_KillNPC( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->Kill(-1,0);
		return 1;
	}
	return 0;
}
// SpawnNPC(NPC:npcid);
static cell AMX_NATIVE_CALL n_SpawnNPC( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->Spawn();
		return 1;
	}
	return 0;
}
// StopNPC(NPC:npcid);
static cell AMX_NATIVE_CALL n_StopNPC( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->Stop();
		pNPC[npcid]->SetKeys(0,0,0);
		return 1;
	}
	return 0;
}
// PutNPCInVehicle(NPC:npcid,vehicleid,seat);
static cell AMX_NATIVE_CALL n_PutNPCInVehicle( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->PutInVehicle((int)params[2],(int)params[3]);
		return 1;
	}
	return 0;
}
// NPC_DriveTo(NPC:npcid,Float:X,Float:Y,Float:Z,Float:speed,is_z_map);
static cell AMX_NATIVE_CALL n_NPC_DriveTo( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float x = amx_ctof(params[2]);
		float y = amx_ctof(params[3]);
		float z = amx_ctof(params[4]);
		float speed = amx_ctof(params[5]);
		bool z_map = (params[6]==0?false:true);
		pNPC[npcid]->DriveTo(x,y,z,speed,z_map);
		pNPC[npcid]->SetKeys(0,0,0);
		return 1;
	}
	return 0;
}
// SetNPCVehicleSiren(NPC:npcid,state);
static cell AMX_NATIVE_CALL n_SetNPCVehicleSiren( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->SetVehicleSiren(params[2]);
		return 1;
	}
	return 0;
}
// GetNPCVehicleSiren(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCVehicleSiren( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		return pNPC[npcid]->GetVehicleSiren();
	}
	return 0;
}
// SetNPCVehicleHealth(NPC:npcid,Float:health);
static cell AMX_NATIVE_CALL n_SetNPCVehicleHealth( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float health = amx_ctof(params[2]);
		pNPC[npcid]->SetVehicleHealth(health);
		return 1;
	}
	return 0;
}
// Float:GetNPCVehicleHealth(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCVehicleHealth( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float health = pNPC[npcid]->GetVehicleHealth();
		return amx_ftoc(health);
	}
	return 0;
}
// SetNPCVehicleTrailer(NPC:npcid,trailerid);
static cell AMX_NATIVE_CALL n_SetNPCVehicleTrailer( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->SetVehicleTrailer(params[2]);
		return 1;
	}
	return 0;
}
// GetNPCVehicleTrailer(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCVehicleTrailer( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		return pNPC[npcid]->GetVehicleTrailer();
	}
	return 0;
}
// SetNPCPassangerDriveBy(NPC:npcid,weaponid);
static cell AMX_NATIVE_CALL n_SetNPCPassangerDriveBy( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->SetPassangerDriveBy(params[2]);
		return 1;
	}
	return 0;
}
// GetNPCPassangerDriveBy(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCPassangerDriveBy( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		return pNPC[npcid]->GetPassangerDriveBy();
	}
	return 0;
}
// SetWeaponDamageForNPC(weaponid,Float:damage);
static cell AMX_NATIVE_CALL n_SetWeaponDamageForNPC( AMX* amx, cell* params )
{
	int weaponid = (int)params[1];
	WeaponsDamage[weaponid] = amx_ctof(params[2]);
	return 1;	
}
// SetWeaponReloadTimeForNPC(weaponid,mstime);
static cell AMX_NATIVE_CALL n_SetWeaponReloadTimeForNPC( AMX* amx, cell* params )
{
	int weaponid = (int)params[1];
	WeaponsReloadTime[weaponid] = (int)params[2];
	return 1;	
}
// StartRecordingPlayback(NPC:npcid,name[]);
static cell AMX_NATIVE_CALL n_StartRecordingPlayback( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		char* temp;
		amx_StrParam(amx, params[2], temp);
		return pNPC[npcid]->StartRecordingPlayback(temp);
	}
	return 0;	
}
// PauseRecordingPlayback(NPC:npcid);
static cell AMX_NATIVE_CALL n_PauseRecordingPlayback( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->PauseRecordingPlayback();
		return 1;
	}
	return 0;	
}
// ContinueRecordingPlayback(NPC:npcid);
static cell AMX_NATIVE_CALL n_ContinueRecordingPlayback( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->ContinueRecordingPlayback();
		return 1;
	}
	return 0;	
}
// StopRecordingPlayback(NPC:npcid);
static cell AMX_NATIVE_CALL n_StopRecordingPlayback( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->StopRecordingPlayback(NPC_RECORD_END_REASON_STOP);
		return 1;
	}
	return 0;	
}
// ToogleVisualDeath(tstate);
static cell AMX_NATIVE_CALL n_ToogleVisualDeath( AMX* amx, cell* params )
{
	VisualDeath = static_cast<char>(params[1]);
	return 1;
}
// FindLastFreeSlot();
static cell AMX_NATIVE_CALL n_FindLastFreeSlot( AMX* amx, cell* params )
{
	for(int i = (MAX_PLAYERS - 1);i > 0;i--)
	{
		if(!pSaMp->players->player[i]) return i;
	}
	return INVALID_PLAYER_ID;	
}

// Node:OpenNode(name[]);
static cell AMX_NATIVE_CALL n_OpenNode( AMX* amx, cell* params )
{
	for(int i = 0;i < MAX_NODES;i++)
	{
		if(Nodes[i]) continue;
		char* temp;
		amx_StrParam(amx, params[1], temp);

		Nodes[i] = new CNode(temp);
		return i;
	}
	return 0;	
}
// CloseNode(Node:nodeid);
static cell AMX_NATIVE_CALL n_CloseNode( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		delete Nodes[nodeid];
		Nodes[nodeid] = 0;
		return 1;
	}
	return 0;
}
// GetNodeHeader(Node:nodeid,&nodes,&vehicle_nodes,&ped_nodes,&navi_nodes);
static cell AMX_NATIVE_CALL n_GetNodeHeader( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		int d1 = 0,
			d2 = 0,
			d3 = 0,
			d4 = 0;
		Nodes[nodeid]->GetInfo(&d1,&d2,&d3,&d4);
		//
		cell
			* p1,
			* p2,
			* p3,
			* p4;
		amx_GetAddr(amx, params[2], &p1);
		amx_GetAddr(amx, params[3], &p2);
		amx_GetAddr(amx, params[4], &p3);
		amx_GetAddr(amx, params[5], &p4);
		//
		*p1 = d1;
		*p2 = d2;
		*p3 = d3;
		*p4 = d4;
		return 1;
	}
	return 0;
}
// SetNodePoint(Node:nodeid,node_type,pointid);
static cell AMX_NATIVE_CALL n_SetNodePoint( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		Nodes[nodeid]->SetPoint((int)params[2]);
		return 1;
	}
	return 0;
}
// GetNodePoint(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodePoint( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->GetPointId();
	}
	return 0;
}
// GetNodePointPos(Node:nodeid,&Float:X,&Float:Y,&Float:Z);
static cell AMX_NATIVE_CALL n_GetNodePointPos( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		float x,y,z;
		Nodes[nodeid]->GetPos(&x,&y,&z);

		cell
			* p1,
			* p2,
			* p3;
		amx_GetAddr(amx, params[2], &p1);
		amx_GetAddr(amx, params[3], &p2);
		amx_GetAddr(amx, params[4], &p3);

		*p1 = amx_ftoc(x);
		*p2 = amx_ftoc(y);
		*p3 = amx_ftoc(z);
		return 1;
	}
	return 0;
}
// GetNodePointLinkId(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodePointLinkId( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->GetLinkId();
	}
	return 0;
}
// GetNodePointAreaId(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodePointAreaId( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->GetAreaId();
	}
	return 0;
}
// GetNodePointWidth(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodePointWidth( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->GetPathWidth();
	}
	return 0;
}
// GetNodePointLinkCount(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodePointLinkCount( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->GetLinkCount();
	}
	return 0;
}
// GetNodePointTrafficLevel(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodePointTrafficLevel( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->GetTrafficLevel();
	}
	return 0;
}
// IsNodePointRoadBlock(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointRoadBlock( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->IsRoadBlock();
	}
	return 0;
}
// IsNodePointBoats(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointBoats( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->IsBoats();
	}
	return 0;
}
// IsNodePointEmergency(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointEmergency( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->IsEmergency();
	}
	return 0;
}
// IsNodePointNotHighway(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointNotHighway( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->IsNotHighway();
	}
	return 0;
}
// IsNodePointSpawn(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointSpawn( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->IsSpawn();
	}
	return 0;
}
// IsNodePointRoadBlock1(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointRoadBlock1( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->IsRoadBlock1();
	}
	return 0;
}
// IsNodePointParking(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointParking( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->IsParking();
	}
	return 0;
}
// IsNodePointRoadBlock2(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointRoadBlock2( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->IsRoadBlock2();
	}
	return 0;
}
// GetNodePointType(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodePointType( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->GetNodeType();
	}
	return 0;
}
// SetNodeLink(Node:nodeid,linkid);
static cell AMX_NATIVE_CALL n_SetNodeLink( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		Nodes[nodeid]->SetLink((int)params[2]);
		return 1;
	}
	return 0;
}
// GetNodeLinkAreaId(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodeLinkAreaId( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->GetLinkAreaId();
	}
	return 0;
}
// GetNodeLinkNodeId(Node:nodeid,&type);
static cell AMX_NATIVE_CALL n_GetNodeLinkNodeId( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(Nodes[nodeid])
	{
		return Nodes[nodeid]->GetLinkNodeId();
	}
	return 0;
}
// synccpy(playerid,npcid);
static cell AMX_NATIVE_CALL n_synccpy( AMX* amx, cell* params )
{
	int playerid = (int)params[1];
	int npcid = (int)params[2];

	CPlayer* pl = pSaMp->players->player[playerid];
	CPlayer* np = pSaMp->players->player[npcid];
	
	typedef void (*logprintf_t)(char* format, ...);
	extern logprintf_t logprintf;

	if(pl)
	{
		if(np)
		{
			unsigned short vid = np->PassangerSync.VehicleId;

			memcpy(&np->PassangerSync,&pl->PassangerSync,16);
			memcpy(&np->AimSync,&pl->AimSync,29);

			np->PassangerSync.VehicleId = vid;
			
			logprintf("passanger");
			logprintf("seat %d",np->PassangerSync.Seat);
			logprintf("driveby %d",np->PassangerSync.DriveBy);
			logprintf("weapon %d",np->PassangerSync.Weapon);
			logprintf("health %d",np->PassangerSync.Health);
			logprintf("armour %d",np->PassangerSync.Armour);
			logprintf("keyslr %d",np->PassangerSync.KeysLR);
			logprintf("keysud %d",np->PassangerSync.KeysUD);
			logprintf("other %d",np->PassangerSync.KeysOther);
			logprintf("position %f,%f,%f",np->PassangerSync.Position[0],np->PassangerSync.Position[1],np->PassangerSync.Position[2]);
			
			logprintf("aim");
			logprintf("cameramode %d",np->AimSync.CameraMode);
			logprintf("fv %f,%f,%f",np->AimSync.FrontVector[0],np->AimSync.FrontVector[1],np->AimSync.FrontVector[2]);
			logprintf("pos %f,%f,%f",np->AimSync.Position[0],np->AimSync.Position[1],np->AimSync.Position[2]);
			logprintf("zaim %f",np->AimSync.ZAim);
			logprintf("CamExtZoom %d",np->AimSync.CamExtZoom);
			logprintf("WeaponState %d",np->AimSync.WeaponState);
			logprintf("Unknown1 %d",np->AimSync.Unknown1);
		}
	}

	return 0;
}

AMX_NATIVE_INFO Natives[ ] =
{
	// core
	{ "CreateNPC",					n_CreateNPC},
	{ "DestroyNPC",					n_DestroyNPC},
	{ "IsValidNPC",					n_IsValidNPC},
	{ "KillNPC",					n_KillNPC},
	{ "SpawnNPC",					n_SpawnNPC},
	{ "StopNPC",					n_StopNPC},
	{ "GetNPCState",				n_GetNPCState},
	{ "ToogleVisualDeath",			n_ToogleVisualDeath},
	{ "FindLastFreeSlot",			n_FindLastFreeSlot},
	// controls foot 
	{ "SetNPCPos",					n_SetNPCPos},
	{ "GetNPCPos",					n_GetNPCPos},
	{ "SetNPCQuaternion",			n_SetNPCQuaternion},
	{ "GetNPCQuaternion",			n_GetNPCQuaternion},
	{ "SetNPCFacingAngle",			n_SetNPCFacingAngle},
	{ "GetNPCFacingAngle",			n_GetNPCFacingAngle},
	{ "SetNPCVelocity",				n_SetNPCVelocity},
	{ "GetNPCVelocity",				n_GetNPCVelocity},
	{ "NPC_WalkTo",					n_NPC_WalkTo},
	{ "NPC_RunTo",					n_NPC_RunTo},
	{ "NPC_SprintTo",				n_NPC_SprintTo},
	{ "NPC_LookAt",					n_NPC_LookAt},
	{ "NPC_AimAt",					n_NPC_AimAt},
	{ "NPC_ShotAt",					n_NPC_ShotAt},
	{ "SetNPCCameraPos",			n_SetNPCCameraPos},
	{ "GetNPCCameraPos",			n_GetNPCCameraPos},
	{ "SetNPCCameraFrontVector",	n_SetNPCCameraFrontVector},
	{ "GetNPCCameraFrontVector",	n_GetNPCCameraFrontVector},
	{ "SetNPCCameraMode",			n_SetNPCCameraMode},
	{ "GetNPCCameraMode",			n_GetNPCCameraMode},
	{ "SetNPCWeaponState",			n_SetNPCWeaponState},
	{ "GetNPCWeaponState",			n_GetNPCWeaponState},
	{ "SetNPCKeys",					n_SetNPCKeys},
	{ "GetNPCKeys",					n_GetNPCKeys},
	{ "SetNPCWeapon",				n_SetNPCWeapon},
	{ "GetNPCWeapon",				n_GetNPCWeapon},
	{ "SetNPCWeaponSkillLevel",		n_SetNPCWeaponSkillLevel},
	{ "GetNPCWeaponSkillLevel",		n_GetNPCWeaponSkillLevel},
	{ "SetNPCHealth",				n_SetNPCHealth},
	{ "GetNPCHealth",				n_GetNPCHealth},
	{ "SetNPCArmour",				n_SetNPCArmour},
	{ "GetNPCArmour",				n_GetNPCArmour},
	{ "SetNPCInterior",				n_SetNPCInterior},
	{ "GetNPCInterior",				n_GetNPCInterior},
	{ "SetNPCSpecialAction",		n_SetNPCSpecialAction},
	{ "GetNPCSpecialAction",		n_GetNPCSpecialAction},
	{ "SetNPCSkin",					n_SetNPCSkin},
	{ "GetNPCSkin",					n_GetNPCSkin},
	{ "SetNPCSurfing",				n_SetNPCSurfing},
	{ "GetNPCSurfing",				n_GetNPCSurfing},
	{ "SetNPCSurfingVehicle",		n_SetNPCSurfingVehicle},
	{ "GetNPCSurfingVehicle",		n_GetNPCSurfingVehicle},
	{ "SetNPCImpregnable",			n_SetNPCImpregnable},
	{ "IsNPCImpregnable",			n_IsNPCImpregnable},
	// controls vehicle
	{ "PutNPCInVehicle",			n_PutNPCInVehicle},
	{ "NPC_DriveTo",				n_NPC_DriveTo},	
	{ "SetNPCVehicleSiren",			n_SetNPCVehicleSiren},
	{ "GetNPCVehicleSiren",			n_GetNPCVehicleSiren},
	{ "SetNPCVehicleHealth",		n_SetNPCVehicleHealth},
	{ "GetNPCVehicleHealth",		n_GetNPCVehicleHealth},
	{ "SetNPCVehicleTrailer",		n_SetNPCVehicleTrailer},
	{ "GetNPCVehicleTrailer",		n_GetNPCVehicleTrailer},
	// controls passanger
	{ "SetNPCPassangerDriveBy",		n_SetNPCPassangerDriveBy},
	{ "GetNPCPassangerDriveBy",		n_GetNPCPassangerDriveBy},
	// weapons damage
	{ "SetWeaponDamageForNPC",		n_SetWeaponDamageForNPC},
	{ "SetWeaponReloadTimeForNPC",	n_SetWeaponReloadTimeForNPC},
	// playbacks
	{ "StartRecordingPlayback",		n_StartRecordingPlayback},
	{ "PauseRecordingPlayback",		n_PauseRecordingPlayback},
	{ "ContinueRecordingPlayback",	n_ContinueRecordingPlayback},
	{ "StopRecordingPlayback",		n_StopRecordingPlayback},
	// nodes
	{ "OpenNode",					n_OpenNode},
	{ "CloseNode",					n_CloseNode},
	{ "GetNodeHeader",				n_GetNodeHeader},
	{ "SetNodePoint",				n_SetNodePoint},
	{ "GetNodePoint",				n_GetNodePoint},
	{ "GetNodePointPos",			n_GetNodePointPos},
	{ "GetNodePointLinkId",			n_GetNodePointLinkId},
	{ "GetNodePointLinkCount",		n_GetNodePointLinkCount},
	{ "GetNodePointAreaId",			n_GetNodePointAreaId},
	{ "GetNodePointWidth",			n_GetNodePointWidth},
	{ "GetNodePointType",			n_GetNodePointType},
	{ "SetNodeLink",				n_SetNodeLink},
	{ "GetNodeLinkAreaId",			n_GetNodeLinkAreaId},
	{ "GetNodeLinkNodeId",			n_GetNodeLinkNodeId},
	{ "GetNodePointTrafficLevel",	n_GetNodePointTrafficLevel},
	{ "IsNodePointRoadBlock",		n_IsNodePointRoadBlock},
	{ "IsNodePointBoats",			n_IsNodePointBoats},
	{ "IsNodePointEmergency",		n_IsNodePointEmergency},
	{ "IsNodePointNotHighway",		n_IsNodePointNotHighway},
	{ "IsNodePointSpawn",			n_IsNodePointSpawn},
	{ "IsNodePointRoadBlock1",		n_IsNodePointRoadBlock1},
	{ "IsNodePointParking",			n_IsNodePointParking},
	{ "IsNodePointRoadBlock2",		n_IsNodePointRoadBlock2},
	// nodes module
	{ "nodes_OnInit",				n_nodes_OnInit},
	{ 0,							0 }
};

void NativesOnAMXLoad(AMX* amx)
{
	amx_Register(amx, Natives, -1 );
}
