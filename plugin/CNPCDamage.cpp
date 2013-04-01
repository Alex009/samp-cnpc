/*
 *	Copyright (C) 2013 Alex009
 *	License read in license.txt
 *	Description: Controllable NPCs damage class
 */

#include "CNPCDamage.h"
#include "CCallbacks.h"
#include "utils/utils.h"
#include "defines.h"
#include "SWeaponData.h"
#include "os.h"
#include <stdlib.h>
#include <math.h>

extern  CCallbacks*     pCallbacks;
extern	unsigned short	MaxPlayers;

double ShootTime[MAX_PLAYERS];

unsigned short CNPCDamage::ObjectsCount = 0;
CNPCDamage::s_player_data* CNPCDamage::pPlayersData[MAX_PLAYERS];

CNPCDamage::CNPCDamage(CNPCCore* _pNPC)
{
	// link
	pNPC = _pNPC;
	// counter
	ObjectsCount++;
}

CNPCDamage::~CNPCDamage()
{
	// counter
	ObjectsCount--;
}

void CNPCDamage::ProcessRangeWeaponDamage(double update_time)
{
	// check attack from others players
	for(unsigned short id = 0;id < MaxPlayers;id++)
	{
		if(id == pNPC->GetMyPlayerid()) continue;
		if(!pPlayersData[id]) continue;

		switch(pPlayersData[id]->state)
		{
		case PLAYER_STATE_ONFOOT:
			{
				if(!pPlayersData[id]->isfire) continue;

				float mx,my,mz,vx,vy,vz,dist;
				signed char part = -1;
				pNPC->GetPosition(&mx,&my,&mz);
				pNPC->GetVelocity(&vx,&vy,&vz);

				// moving fix
				mx += vx;
				my += vy;
				mz += vz;

				if((part == -1) && GetDistanceFromRayToPoint(pPlayersData[id]->cam_pos[0],pPlayersData[id]->cam_pos[1],pPlayersData[id]->cam_pos[2],
					pPlayersData[id]->vec_ed[0],pPlayersData[id]->vec_ed[1],pPlayersData[id]->vec_ed[2],
					mx,my,mz - 0.5f,
					&dist))
				{
					if(dist < 0.4f) part = BODYPART_FOOT;
				}
				if((part == -1) && GetDistanceFromRayToPoint(pPlayersData[id]->cam_pos[0],pPlayersData[id]->cam_pos[1],pPlayersData[id]->cam_pos[2],
					pPlayersData[id]->vec_ed[0],pPlayersData[id]->vec_ed[1],pPlayersData[id]->vec_ed[2],
					mx,my,mz + 0.25f,
					&dist))
				{
					if(dist < 0.35f) part = BODYPART_TORSO;
				}
				if((part == -1) && GetDistanceFromRayToPoint(pPlayersData[id]->cam_pos[0],pPlayersData[id]->cam_pos[1],pPlayersData[id]->cam_pos[2],
					pPlayersData[id]->vec_ed[0],pPlayersData[id]->vec_ed[1],pPlayersData[id]->vec_ed[2],
					mx,my,mz + 0.75f,
					&dist))
				{
					if(dist < 0.15f) part = BODYPART_HEAD;
				}
				
				if(part != -1)
				{
					unsigned char weapon =  pPlayersData[id]->weaponid;
					SWeaponData* wd = GetWeaponData(weapon);
					float health = pNPC->GetHealth() - wd->damage;
					if(pCallbacks->GetDamage(pNPC->GetMyPlayerid(),id,wd->damage,part))
					{
						pNPC->SetHealth(health);
						if(health <= 0.0) pNPC->Kill(id,weapon);
					}
				}
			}
		}
	}
}

bool CNPCDamage::Update(double update_time)
{
	// check current state
	if(pNPC->GetState() == PLAYER_STATE_WASTED) return true;
	if(pNPC->GetState() == PLAYER_STATE_SPECTATING) return true;

	// process damage
	ProcessRangeWeaponDamage(update_time);

	return true;
}

void CNPCDamage::Init()
{
	ObjectsCount = 0;
	for(unsigned short id = 0;id < MAX_PLAYERS;id++) 
	{
		pPlayersData[id] = NULL;
		ShootTime[id] = 0.0;
	}
}

void CNPCDamage::UpdatePlayersData(CPlayers* _pPlayers,double update_time)
{
	// if not have killable npc - not update data
	if(ObjectsCount == 0) return;

	for(unsigned short id = 0;id < MaxPlayers;id++)
	{
		CPlayer* pPlayer = _pPlayers->player[id];
		
		if(pPlayer)
		{
			// player exist, create struct if it not exist
			if(!pPlayersData[id]) pPlayersData[id] = new s_player_data;
			// data
			pPlayersData[id]->state = pPlayer->State;
			// check by state
			switch(pPlayersData[id]->state)
			{
			case PLAYER_STATE_ONFOOT:
				{
					// update data
					for(int i = 0;i < 3;i++)
					{
						pPlayersData[id]->cam_pos[i] = pPlayer->AimSync.Position[i];
						pPlayersData[id]->vec_ed[i] = pPlayer->AimSync.FrontVector[i];
					}
					pPlayersData[id]->isfire = 
						(
							(pPlayer->FootSync.KeysOther & KEY_FIRE) && 
							!(pPlayer->FootSync.KeysOther & KEY_SPRINT) &&
							(pPlayer->AimSync.WeaponState != WEAPONSTATE_RELOADING) &&
							(pPlayer->AimSync.WeaponState != WEAPONSTATE_NO_BULLETS)
						);
					pPlayersData[id]->weaponid = pPlayer->FootSync.PlayerWeapon;
					// get weapon data
					SWeaponData* wd = GetWeaponData(pPlayersData[id]->weaponid);
					// check shoot time
					if((pPlayersData[id]->isfire) && ((update_time - ShootTime[id]) < wd->shoot_time)) pPlayersData[id]->isfire = false;
					// fire?
					if(pPlayersData[id]->isfire)
					{
						// vector by weapon
						rotate_vector(pPlayersData[id]->vec_ed[0],pPlayersData[id]->vec_ed[1],pPlayersData[id]->vec_ed[2],wd->vector_angles[0],wd->vector_angles[1]);
						// shoot time
						ShootTime[id] = update_time;
					}
					// set len of vector = 1.0
					float len = sqrt(pPlayersData[id]->vec_ed[0]*pPlayersData[id]->vec_ed[0] + pPlayersData[id]->vec_ed[1]*pPlayersData[id]->vec_ed[1] + pPlayersData[id]->vec_ed[2]*pPlayersData[id]->vec_ed[2]);
					for(int i = 0;i < 3;i++) pPlayersData[id]->vec_ed[i] /= len;
					break;
				}
			}
			
		}
		else
		{
			// player not exist
			if(pPlayersData[id]) 
			{
				delete pPlayersData[id];
				pPlayersData[id] = NULL;
			}
		}
	}
}