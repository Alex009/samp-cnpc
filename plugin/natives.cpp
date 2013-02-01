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
#include "SAMP/CSAMP.h"
#include "CNPC.h"
#include "CNode.h"
#include "CNodeManager.h"
#include "CNodePath.h"
#include "CZMap.h"
#include "hooks.h"
#include "defines.h"
#include "utils/utils.h"
#include "utils/time.h"
#include "path_calc.h"

extern	CNPC*			pNPC[MAX_NPCS];
extern	CNode*			pNodes[MAX_NODES];
extern	CNodeManager*	pNodeManager;
extern	CNodePath*		pNodePaths[MAX_NODE_PATHS];
extern	CZMap*			pZMap;
extern	CSAMP*			pSaMp;
extern	CHooks*			pHooks;
extern	char			VisualDeath;
extern	unsigned short	MaxPlayers;
extern	int				WeaponReload[MAX_PLAYERS];
extern	float			WeaponsDamage[MAX_WEAPONS + 2];
extern	int				WeaponsReloadTime[MAX_WEAPONS + 1];

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
		pNPC[npcid] = NULL;
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
		pNPC[npcid]->GoTo(x,y,z,VELOCITY_LEN_WALK,z_map,false);
		pNPC[npcid]->SetKeys(KEY_UP,0,KEY_WALK);
		pNPC[npcid]->SetState(NPC_STATE_ONFOOT);
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
		pNPC[npcid]->GoTo(x,y,z,VELOCITY_LEN_RUN,z_map,false);
		pNPC[npcid]->SetKeys(KEY_UP,0,0);
		pNPC[npcid]->SetState(NPC_STATE_ONFOOT);
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
		pNPC[npcid]->GoTo(x,y,z,VELOCITY_LEN_SPRINT,z_map,false);
		pNPC[npcid]->SetKeys(KEY_UP,0,KEY_SPRINT);
		pNPC[npcid]->SetState(NPC_STATE_ONFOOT);
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
		float x,y,z,tx,ty,tz,len;
		pNPC[npcid]->GetPosition(&x,&y,&z);
		z += 0.7f;
		tx = amx_ctof(params[2]);
		ty = amx_ctof(params[3]);
		tz = amx_ctof(params[4]);
		// create vector
		tx -= x;
		ty -= y;
		tz -= z;
		len = sqrt((tx*tx) + (ty*ty) + (tz*tz));
		tx /= len;
		ty /= len;
		tz /= len;
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
		float tx,ty,tz;

		tx = amx_ctof(params[2]);
		ty = amx_ctof(params[3]);
		tz = amx_ctof(params[4]);

		pNPC[npcid]->SetAimByWeapon(pNPC[npcid]->GetWeapon(),tx,ty,tz);

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
				break;
			}
		}
		return 1;
	}
	return 1;
}
// NPC_ShootAt(NPC:npcid,Float:X,Float:Y,Float:Z);
static cell AMX_NATIVE_CALL n_NPC_ShootAt( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		float tx,ty,tz;

		tx = amx_ctof(params[2]);
		ty = amx_ctof(params[3]);
		tz = amx_ctof(params[4]);

		pNPC[npcid]->SetAimByWeapon(pNPC[npcid]->GetWeapon(),tx,ty,tz);
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
		if(health <= 0.0) pNPC[npcid]->Kill(npcid,0);
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
// SetNPCSpecialAction(NPC:npcid,sactionid);
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
// SetNPCAnimationIndex(NPC:npcid,animationid);
static cell AMX_NATIVE_CALL n_SetNPCAnimationIndex( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		pNPC[npcid]->SetAnimationIndex(params[2]);
		return 1;
	}
	return 0;
}
// GetNPCAnimationIndex(NPC:npcid);
static cell AMX_NATIVE_CALL n_GetNPCAnimationIndex( AMX* amx, cell* params )
{
	int npcid = (int)params[1];
	if(pNPC[npcid])
	{
		return pNPC[npcid]->GetAnimationIndex();
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
		float speed = amx_ctof(params[5]) / 200.0f;
		bool z_map = (params[6]==0?false:true);
		pNPC[npcid]->GoTo(x,y,z,speed,z_map,true);
		pNPC[npcid]->SetKeys(KEY_UP,0,0);
		pNPC[npcid]->SetState(NPC_STATE_DRIVER);
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
	VisualDeath = (params[1] == 0?0:1);
	return 1;
}
// FindLastFreeSlot();
static cell AMX_NATIVE_CALL n_FindLastFreeSlot( AMX* amx, cell* params )
{
	for(int i = (MaxPlayers - 1);i > 0;i--)
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
		if(pNodes[i]) continue;
		char* temp;
		amx_StrParam(amx, params[1], temp);

		pNodes[i] = new CNode(temp);
		return i;
	}
	return 0;	
}
// CloseNode(Node:nodeid);
static cell AMX_NATIVE_CALL n_CloseNode( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		delete pNodes[nodeid];
		pNodes[nodeid] = 0;
		return 1;
	}
	return 0;
}
// GetNodeHeader(Node:nodeid,&nodes,&vehicle_nodes,&ped_nodes,&navi_nodes);
static cell AMX_NATIVE_CALL n_GetNodeHeader( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		unsigned long d1 = 0,
			d2 = 0,
			d3 = 0,
			d4 = 0;
		pNodes[nodeid]->GetInfo(&d1,&d2,&d3,&d4);
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
	if(pNodes[nodeid])
	{
		pNodes[nodeid]->SetPoint((int)params[2]);
		return 1;
	}
	return 0;
}
// GetNodePoint(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodePoint( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->GetPointId();
	}
	return 0;
}
// GetNodePointPos(Node:nodeid,&Float:X,&Float:Y,&Float:Z);
static cell AMX_NATIVE_CALL n_GetNodePointPos( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		float x,y,z;
		pNodes[nodeid]->GetPos(&x,&y,&z);

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
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->GetLinkId();
	}
	return 0;
}
// GetNodePointAreaId(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodePointAreaId( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->GetAreaId();
	}
	return 0;
}
// GetNodePointWidth(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodePointWidth( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->GetPathWidth();
	}
	return 0;
}
// GetNodePointLinkCount(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodePointLinkCount( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->GetLinkCount();
	}
	return 0;
}
// GetNodePointTrafficLevel(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodePointTrafficLevel( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->GetTrafficLevel();
	}
	return 0;
}
// IsNodePointRoadBlock(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointRoadBlock( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->IsRoadBlock();
	}
	return 0;
}
// IsNodePointBoats(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointBoats( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->IsBoats();
	}
	return 0;
}
// IsNodePointEmergency(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointEmergency( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->IsEmergency();
	}
	return 0;
}
// IsNodePointNotHighway(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointNotHighway( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->IsNotHighway();
	}
	return 0;
}
// IsNodePointSpawn(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointSpawn( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->IsSpawn();
	}
	return 0;
}
// IsNodePointRoadBlock1(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointRoadBlock1( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->IsRoadBlock1();
	}
	return 0;
}
// IsNodePointParking(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointParking( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->IsParking();
	}
	return 0;
}
// IsNodePointRoadBlock2(Node:nodeid);
static cell AMX_NATIVE_CALL n_IsNodePointRoadBlock2( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->IsRoadBlock2();
	}
	return 0;
}
// GetNodePointType(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodePointType( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->GetNodeType();
	}
	return 0;
}
// SetNodeLink(Node:nodeid,linkid);
static cell AMX_NATIVE_CALL n_SetNodeLink( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		pNodes[nodeid]->SetLink((int)params[2]);
		return 1;
	}
	return 0;
}
// GetNodeLinkAreaId(Node:nodeid);
static cell AMX_NATIVE_CALL n_GetNodeLinkAreaId( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];
	if(pNodes[nodeid])
	{
		return pNodes[nodeid]->GetLinkAreaId();
	}
	return 0;
}
// GetNodeLinkNodeId(Node:nodeid,&type);
static cell AMX_NATIVE_CALL n_GetNodeLinkNodeId( AMX* amx, cell* params )
{
	int nodeid = (int)params[1];

	if(pNodes[nodeid]) return pNodes[nodeid]->GetLinkNodeId();
	return 0;
}
// nodes_Init();
static cell AMX_NATIVE_CALL n_nodes_Init( AMX* amx, cell* params )
{
	if(!pNodeManager) pNodeManager = new CNodeManager();
	return 1;
}
// nodes_Exit();
static cell AMX_NATIVE_CALL n_nodes_Exit( AMX* amx, cell* params )
{
	if(pNodeManager) 
	{
		delete pNodeManager;
		pNodeManager = NULL;
	}
	return 1;
}
// nodes_GetNodePos(nodetype,nodeid,&Float:x,&Float:y,&Float:z);
static cell AMX_NATIVE_CALL n_nodes_GetNodePos( AMX* amx, cell* params )
{
	cell
			* x,
			* y,
			* z;
	int nodetype = (int)params[1];
	int nodeid = (int)params[2];
	float pos[3];

	amx_GetAddr(amx, params[3], &x);
	amx_GetAddr(amx, params[4], &y);
	amx_GetAddr(amx, params[5], &z);

	if(pNodeManager->GetNodePos(nodetype,nodeid,&pos[0],&pos[1],&pos[2]))
	{
		*x = amx_ftoc(pos[0]);
		*y = amx_ftoc(pos[1]);
		*z = amx_ftoc(pos[2]);

		return 1;
	}
	else return 0;
}
// nodes_GetNodeAreaid(nodetype,nodeid);
static cell AMX_NATIVE_CALL n_nodes_GetNodeAreaid( AMX* amx, cell* params )
{
	int nodetype = (int)params[1];
	int nodeid = (int)params[2];

	return pNodeManager->GetNodeAreaid(nodetype,nodeid);
}
// nodes_GetNodeNodeid(nodetype,nodeid);
static cell AMX_NATIVE_CALL n_nodes_GetNodeNodeid( AMX* amx, cell* params )
{
	int nodetype = (int)params[1];
	int nodeid = (int)params[2];

	return pNodeManager->GetNodeNodeid(nodetype,nodeid);
}
// nodes_GetNodeLink(nodetype,nodeid,linkid);
static cell AMX_NATIVE_CALL n_nodes_GetNodeLink( AMX* amx, cell* params )
{
	int nodetype = (int)params[1];
	int nodeid = (int)params[2];
	int linkid = (int)params[3];

	return pNodeManager->GetNodeLink(nodetype,nodeid,linkid);
}
// Float:nodes_GetNodeLinkDist(nodetype,nodeid,linkid);
static cell AMX_NATIVE_CALL n_nodes_GetNodeLinkDist( AMX* amx, cell* params )
{
	int nodetype = (int)params[1];
	int nodeid = (int)params[2];
	int linkid = (int)params[3];

	float result = pNodeManager->GetNodeLinkDist(nodetype,nodeid,linkid);

	return amx_ftoc(result);
}
// nodes_GetNodeLinkCount(nodetype,nodeid);
static cell AMX_NATIVE_CALL n_nodes_GetNodeLinkCount( AMX* amx, cell* params )
{
	int nodetype = (int)params[1];
	int nodeid = (int)params[2];

	return pNodeManager->GetNodeLinkCount(nodetype,nodeid);
}
// nodes_GetNodeCount(nodetype);
static cell AMX_NATIVE_CALL n_nodes_GetNodeCount( AMX* amx, cell* params )
{
	int nodetype = (int)params[1];

	return pNodeManager->GetNodeCount(nodetype);
}
// nodes_GetNodeid(nodetype,Float:x,Float:y,Float:z);
static cell AMX_NATIVE_CALL n_nodes_GetNodeid( AMX* amx, cell* params )
{
	int nodetype = (int)params[1];
	float x = amx_ctof(params[2]);
	float y = amx_ctof(params[3]);
	float z = amx_ctof(params[4]);
	
	return pNodeManager->GetNodeid(nodetype,x,y,z);
}
// nodes_GetAreaid(Float:x,Float:y,Float:z);
static cell AMX_NATIVE_CALL n_nodes_GetAreaid( AMX* amx, cell* params )
{
	float x = amx_ctof(params[1]);
	float y = amx_ctof(params[2]);
	float z = amx_ctof(params[3]);
	
	return pNodeManager->GetAreaid(x,y,z);
}
// nodes_GetAreasFromPos(Float:startx,Float:starty,Float:endx,Float:endy,areas[64]);
static cell AMX_NATIVE_CALL n_nodes_GetAreasFromPos( AMX* amx, cell* params )
{
	cell* r_areas;
	unsigned char* areas = new unsigned char[64];

	float startx = amx_ctof(params[1]);
	float starty = amx_ctof(params[2]);
	float endx = amx_ctof(params[3]);
	float endy = amx_ctof(params[4]);
	amx_GetAddr(amx,params[5],&r_areas);

	if(pNodeManager->GetAreasFromPos(startx,starty,endx,endy,areas))
	{
		for(unsigned char i = 0;i < 64;i++) r_areas[i] = areas[i];
		return 1;
	}
	else return 0;
}
// nodes_GetAreaNodeid(nodetype,areaid,nodeid);
static cell AMX_NATIVE_CALL n_nodes_GetAreaNodeid( AMX* amx, cell* params )
{
	unsigned char nodetype = (unsigned char)params[1];
	unsigned char areaid = (unsigned char)params[2];
	unsigned long nodeid = (unsigned long)params[3];
	
	return pNodeManager->GetAreaNodeid(nodetype,areaid,nodeid);
}
// nodes_GetAreaNodeCount(nodetype,areaid);
static cell AMX_NATIVE_CALL n_nodes_GetAreaNodeCount( AMX* amx, cell* params )
{
	unsigned char nodetype = (unsigned char)params[1];
	unsigned char areaid = (unsigned char)params[2];
	
	return pNodeManager->GetAreaNodeCount(nodetype,areaid);
}
// dijkstra_CalcPathByNodes(areas[64],nodetype,startnodeid,endnodeid);
static cell AMX_NATIVE_CALL n_dijkstra_CalcPathByNodes( AMX* amx, cell* params )
{
	cell* r_areas;
	unsigned char areas[64];

	amx_GetAddr(amx,params[1],&r_areas);
	int nodetype = (int)params[2];
	int startnodeid = (int)params[3];
	int endnodeid = (int)params[4];

	for(unsigned char i = 0;i < 64;i++) areas[i] = (unsigned char)r_areas[i];

	return StartPathCalculation(areas,nodetype,startnodeid,endnodeid);
}
// path_Destroy(pathid);
static cell AMX_NATIVE_CALL n_path_Destroy( AMX* amx, cell* params )
{
	unsigned short pathid = (unsigned short)params[1];
	
	if((int)pNodePaths[pathid] > 1)
	{
		delete pNodePaths[pathid];
		pNodePaths[pathid] = NULL;
		return 1;
	}
	return 0;
}
// path_GetPathNodeid(pathid,pointid);
static cell AMX_NATIVE_CALL n_path_GetPathNodeid( AMX* amx, cell* params )
{
	unsigned short pathid = (unsigned short)params[1];
	unsigned long pointid = (unsigned long)params[2];
	
	if((int)pNodePaths[pathid] > 1)
	{
		return pNodePaths[pathid]->GetPathNodeid(pointid);
	}
	return INVALID_NODE_ID;
}
// path_GetPathLen(pathid);
static cell AMX_NATIVE_CALL n_path_GetPathLen( AMX* amx, cell* params )
{
	unsigned short pathid = (unsigned short)params[1];
	
	if((int)pNodePaths[pathid] > 1)
	{
		return pNodePaths[pathid]->GetPathLen();
	}
	return 0;
}
// Float:path_GetPathDist(pathid);
static cell AMX_NATIVE_CALL n_path_GetPathDist( AMX* amx, cell* params )
{
	unsigned short pathid = (unsigned short)params[1];
	
	if((int)pNodePaths[pathid] > 1)
	{
		float result = pNodePaths[pathid]->GetPathDist();
		return amx_ftoc(result);
	}
	return 0;
}
// zmap_Init(file[],mode);
static cell AMX_NATIVE_CALL n_zmap_Init( AMX* amx, cell* params )
{
	char* temp;
	int mode;
	amx_StrParam(amx, params[1], temp);
	mode = (int)params[2];

	if(!pZMap) pZMap = new CZMap(temp,mode);

	return 1;
}
// zmap_Exit();
static cell AMX_NATIVE_CALL n_zmap_Exit( AMX* amx, cell* params )
{
	if(pZMap)
	{
		delete pZMap;
		pZMap = NULL;
	}

	return 1;
}
// Float:zmap_GetZCoord(Float:x,Float:y);
static cell AMX_NATIVE_CALL n_zmap_GetZCoord( AMX* amx, cell* params )
{
	float x = amx_ctof(params[1]);
	float y = amx_ctof(params[2]);
	float result = 0.0;

	if(pZMap) result = pZMap->GetZForCoords(x,y);

	return amx_ftoc(result);
}
// zmap_IsPointToPointLOS(Float:sx,Float:sy,Float:sz,Float:ex,Float:ey,Float:ez);
static cell AMX_NATIVE_CALL n_zmap_IsPointToPointLOS( AMX* amx, cell* params )
{
	float sx = amx_ctof(params[1]);
	float sy = amx_ctof(params[2]);
	float sz = amx_ctof(params[3]);
	float ex = amx_ctof(params[4]);
	float ey = amx_ctof(params[5]);
	float ez = amx_ctof(params[6]);
	bool result = false;

	if(pZMap) result = pZMap->IsPointToPointLOS(sx,sy,sz,ex,ey,ez);

	return result;
}
// microtime(&sec,&microsec);
static cell AMX_NATIVE_CALL n_microtime( AMX* amx, cell* params )
{
	cell *p1,*p2;
	long sec,microsec;

	amx_GetAddr(amx, params[1], &p1);
	amx_GetAddr(amx, params[2], &p2);

	microtime(&sec,&microsec);

	*p1 = (cell)sec;
	*p2 = (cell)microsec;

	return 1;
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
	{ "NPC_ShootAt",				n_NPC_ShootAt},
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
	{ "SetNPCAnimationIndex",		n_SetNPCAnimationIndex},
	{ "GetNPCAnimationIndex",		n_GetNPCAnimationIndex},
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
	{ "nodes_Init",					n_nodes_Init},
	{ "nodes_Exit",					n_nodes_Exit},
	{ "nodes_GetNodePos",			n_nodes_GetNodePos},
	{ "nodes_GetNodeAreaid",		n_nodes_GetNodeAreaid},
	{ "nodes_GetNodeNodeid",		n_nodes_GetNodeNodeid},
	{ "nodes_GetNodeLink",			n_nodes_GetNodeLink},
	{ "nodes_GetNodeLinkDist",		n_nodes_GetNodeLinkDist},
	{ "nodes_GetNodeLinkCount",		n_nodes_GetNodeLinkCount},
	{ "nodes_GetNodeCount",			n_nodes_GetNodeCount},
	{ "nodes_GetNodeid",			n_nodes_GetNodeid},
	{ "nodes_GetAreaid",			n_nodes_GetAreaid},
	{ "nodes_GetAreasFromPos",		n_nodes_GetAreasFromPos},
	{ "nodes_GetAreaNodeid",		n_nodes_GetAreaNodeid},
	{ "nodes_GetAreaNodeCount",		n_nodes_GetAreaNodeCount},
	// dijkstra module
	{ "dijkstra_CalcPathByNodes",	n_dijkstra_CalcPathByNodes},
	// node paths
	{ "path_Destroy",				n_path_Destroy},
	{ "path_GetPathNodeid",			n_path_GetPathNodeid},
	{ "path_GetPathLen",			n_path_GetPathLen},
	{ "path_GetPathDist",			n_path_GetPathDist},	
	// zmap
	{ "zmap_Init",					n_zmap_Init},
	{ "zmap_Exit",					n_zmap_Exit},
	{ "zmap_GetZCoord",				n_zmap_GetZCoord},
	{ "zmap_IsPointToPointLOS",		n_zmap_IsPointToPointLOS},
	// addition
	{ "microtime",					n_microtime},
	{ 0,							0 }
};

void NativesOnAMXLoad(AMX* amx)
{
	amx_Register(amx, Natives, -1 );
}
