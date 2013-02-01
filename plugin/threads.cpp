/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 */

#define _USE_MATH_DEFINES

#include <math.h>
// plugin
#include "os.h"
#include "defines.h"
#include "CNPC.h"
#include "SAMP/defines.h"
#include "SAMP/CSAMP.h"
#include "callbacks.h"
#include "utils.h"
#include "utils/quaternion.h"
#include "utils/3DVector.h"

extern	CNPC*		pNPC[MAX_NPCS];
extern	CSAMP*		pSaMp;
extern	CCallbacks*	pCallbacks;
extern	char		VisualDeath;
extern	int			WeaponReload[MAX_PLAYERS];
extern	float		WeaponsDamage[MAX_WEAPONS + 2];
extern	int			WeaponsReloadTime[MAX_WEAPONS + 1];

// csamphandle
typedef int (*CSampHandle_t) ();
extern CSampHandle_t CSampHandle;
typedef void (*logprintf_t)(char* format, ...);
extern logprintf_t logprintf;

bool	FireComplete[MAX_PLAYERS];

int		EndAgility[MAX_PLAYERS];
int		StartAgility[MAX_PLAYERS];
int		Attacked[MAX_PLAYERS];

THREAD_START(NPCSyncThread){
	int npcid;
	while(true)
	{
		for(npcid = 0;npcid < MAX_NPCS;npcid++)
		{
			if(!pNPC[npcid]) continue; 

			pNPC[npcid]->Sync();
		}
		SLEEP(50);
	}
THREAD_END

THREAD_START(NPCDamageThread){
	// vars
	unsigned int	local_player_id;
	CPlayer*		local_player;
	unsigned int	local_player_keys;
	unsigned int	local_player_type;
	unsigned int	local_player_knock;
	float			local_player_position[3];
	float			local_player_velocity[3];
	unsigned int	local_player_weapon_id;
	float			local_player_angle;
	unsigned short	local_vehicle_id;
	CVehicle*		local_vehicle;
	unsigned short	local_vehicle_model;
	float			local_float_temp[9];
	unsigned int	local_npc_id;
	float			local_npc_health;
	// reset
	for(local_player_id = 0;local_player_id < MAX_PLAYERS;local_player_id++)
	{
		Attacked[local_player_id] = 0;
		StartAgility[local_player_id] = 0;
		EndAgility[local_player_id] = 0;
	}
	// thread
	while(true)
	{
		// wait
		pSaMp = (CSAMP*)CSampHandle();
		if(!pSaMp) continue;
		if((pSaMp->players == NULL) || (pSaMp->vehicles == NULL)) continue;

		for(local_player_id = 0;local_player_id < MAX_PLAYERS;local_player_id++)
		{
			local_player = pSaMp->players->player[local_player_id];
			if(!local_player) continue;
			
			local_player_keys = 0;
			local_player_type = 0;
			local_player_knock = 0;

			// switch driver and passanger
			switch(local_player->State)
			{
			case PLAYER_STATE_DRIVER:
				{
					local_vehicle_id = local_player->InCarSync.VehicleId;
					local_vehicle = pSaMp->vehicles->vehicle[local_vehicle_id];
					// check vehicle
					if(!local_vehicle) break;
					// get knock data
					local_player_position[0] = local_player->InCarSync.Position[0];
					local_player_position[1] = local_player->InCarSync.Position[1];
					local_player_position[2] = local_player->InCarSync.Position[2];
					local_player_velocity[0] = local_player->InCarSync.Velocity[0];
					local_player_velocity[1] = local_player->InCarSync.Velocity[1];
					local_player_velocity[2] = local_player->InCarSync.Velocity[2];
					// set knock
					local_player_knock = 1;
					// get vehicle model
					local_vehicle_model = local_vehicle->Model;
					// switch model type
					switch(GetVehicleModelType(local_vehicle_model))
					{
					case VEHICLE_MODEL_RHINO:
						{
							if((local_player->InCarSync.KeysOther & KEY_FIRE) == 0)
							{
								if(Attacked[local_player_id] != 0) Attacked[local_player_id] = 0;
								break;
							}
							// if fire completed
							if(Attacked[local_player_id] != 0) break;
							// get coords
							local_float_temp[0] = local_player->InCarSync.Position[0];
							local_float_temp[1] = local_player->InCarSync.Position[1];
							local_float_temp[2] = local_player->InCarSync.Position[2] + 1.6f; // z correct in rhino 1.6
							local_float_temp[3] = local_player->AimSync.FrontVector[0];
							local_float_temp[4] = local_player->AimSync.FrontVector[1];
							local_float_temp[5] = local_player->AimSync.FrontVector[2] + 0.23f; // z correct in rhino 0.23
							// get weapon
							local_player_weapon_id = 47;
							// set type
							local_player_type = DAMAGE_TYPE_RAKET_FROM_VEHICLE;
							// complete
							Attacked[local_player_id] = 1;
							break;
						}
					case VEHICLE_MODEL_HYDRA:
						{
							if((local_player->InCarSync.KeysOther & KEY_ACTION) == 0)
							{
								if(Attacked[local_player_id] != 0) Attacked[local_player_id] = 0;
								break;
							}
							// if fire completed
							if(Attacked[local_player_id] != 0) break;
							// get coords
							local_float_temp[0] = local_player->InCarSync.Position[0];
							local_float_temp[1] = local_player->InCarSync.Position[1];
							local_float_temp[2] = local_player->InCarSync.Position[2];
							QuaternionConvertToVector(local_player->InCarSync.Quaternion,&local_float_temp[3]);
							// get weapon
							local_player_weapon_id = 47;
							// set type
							local_player_type = DAMAGE_TYPE_RAKET_FROM_VEHICLE;
							// complete
							Attacked[local_player_id] = 1;
							break;
						}
					case VEHICLE_MODEL_HUNTER:
						{
							// shoot raket
							if(local_player->InCarSync.KeysOther & KEY_FIRE)
							{
								// if fire completed
								if(Attacked[local_player_id] != 0) break;
								// get coords
								local_float_temp[0] = local_player->InCarSync.Position[0];
								local_float_temp[1] = local_player->InCarSync.Position[1];
								local_float_temp[2] = local_player->InCarSync.Position[2];
								QuaternionConvertToVector(local_player->InCarSync.Quaternion,&local_float_temp[3]);
								// get weapon
								local_player_weapon_id = 47;
								// set type
								local_player_type = DAMAGE_TYPE_RAKET_FROM_VEHICLE;
								// complete
								Attacked[local_player_id] = 1;
							}
							else if(Attacked[local_player_id] != 0) Attacked[local_player_id] = 0;
							// shoot bullet
							if(local_player->InCarSync.KeysOther & KEY_ACTION)
							{
								// reload
								if(WeaponReload[local_player_id] > 0) break;
								// get coords
								local_float_temp[0] = local_player->InCarSync.Position[0];
								local_float_temp[1] = local_player->InCarSync.Position[1];
								local_float_temp[2] = local_player->InCarSync.Position[2];
								QuaternionConvertToVector(local_player->InCarSync.Quaternion,&local_float_temp[3]);
								// get weapon
								local_player_weapon_id = 31;
								// set type
								local_player_type = DAMAGE_TYPE_BULLET_FROM_VEHICLE;
								// complete
								WeaponReload[local_player_id] = WeaponsReloadTime[local_player_weapon_id];
							}
							break;
						}
					case VEHICLE_MODEL_CAR:
						{
							if(WeaponReload[local_player_id] > 0) break;
							if((local_player->InCarSync.KeysOther & KEY_FIRE) == 0) break;
							if((local_player->AimSync.WeaponState >> 6) == WEAPONSTATE_RELOADING) break;
							if((local_player->AimSync.WeaponState >> 6) == WEAPONSTATE_NO_BULLETS) break;
							// direction
							if(local_player->InCarSync.KeysOther & KEY_LOOK_LEFT) local_player_angle = 90.f * (float)(M_PI / 180.f);
							else if(local_player->InCarSync.KeysOther & KEY_LOOK_RIGHT) local_player_angle = -90.f * (float)(M_PI / 180.f);
							else break;
							// get weapon
							local_player_weapon_id = static_cast<unsigned int>(local_player->InCarSync.PlayerWeapon);
							if(local_player_weapon_id == 0) break;
							// get coords
							local_float_temp[0] = local_player->InCarSync.Position[0];
							local_float_temp[1] = local_player->InCarSync.Position[1];
							local_float_temp[2] = local_player->InCarSync.Position[2];
							QuaternionConvertToEuler(local_player->InCarSync.Quaternion,&local_float_temp[6]);
							local_float_temp[8] += local_player_angle;
							EulerConvertToVector(&local_float_temp[6],&local_float_temp[3]);
							// set type
							local_player_type = DAMAGE_TYPE_BULLET_FROM_VEHICLE;
							// complete
							WeaponReload[local_player_id] = WeaponsReloadTime[local_player_weapon_id];
							break;
						}
					case VEHICLE_MODEL_BIKE:
						{
							if(WeaponReload[local_player_id] > 0) break;
							if((local_player->InCarSync.KeysOther & KEY_FIRE) == 0) break;
							if((local_player->AimSync.WeaponState >> 6) == WEAPONSTATE_RELOADING) break;
							if((local_player->AimSync.WeaponState >> 6) == WEAPONSTATE_NO_BULLETS) break;
							// direction
							if(local_player->InCarSync.KeysOther & KEY_LOOK_LEFT) local_player_angle = 90.f * (float)(M_PI / 180.f);
							else if(local_player->InCarSync.KeysOther & KEY_LOOK_RIGHT) local_player_angle = -90.f * (float)(M_PI / 180.f);
							else local_player_angle = 0.0;
							// get weapon
							local_player_weapon_id = static_cast<unsigned int>(local_player->InCarSync.PlayerWeapon);
							if(local_player_weapon_id == 0) break;
							// get coords
							local_float_temp[0] = local_player->InCarSync.Position[0];
							local_float_temp[1] = local_player->InCarSync.Position[1];
							local_float_temp[2] = local_player->InCarSync.Position[2];
							QuaternionConvertToEuler(local_player->InCarSync.Quaternion,&local_float_temp[6]);
							local_float_temp[8] += local_player_angle;
							EulerConvertToVector(&local_float_temp[6],&local_float_temp[3]);
							// set type
							local_player_type = DAMAGE_TYPE_BULLET_FROM_VEHICLE;
							// complete
							WeaponReload[local_player_id] = WeaponsReloadTime[local_player_weapon_id];
							break;
						}
					}
					break;
				}
			case PLAYER_STATE_PASSENGER:
				{
					switch(GetVehicleModelType(local_vehicle_model))
					{
					case VEHICLE_MODEL_CAR:
					case VEHICLE_MODEL_BIKE:
						{
							if(WeaponReload[local_player_id] > 0) break;
							if((local_player->PassangerSync.KeysOther & KEY_FIRE) == 0) break;
							if((local_player->AimSync.WeaponState >> 6) == WEAPONSTATE_RELOADING) break;
							if((local_player->AimSync.WeaponState >> 6) == WEAPONSTATE_NO_BULLETS) break;
							// get weapon
							local_player_weapon_id = static_cast<unsigned int>(local_player->PassangerSync.Weapon);
							if(local_player_weapon_id == 0) break;
							// get coords
							local_float_temp[0] = local_player->AimSync.Position[0];
							local_float_temp[1] = local_player->AimSync.Position[1];
							local_float_temp[2] = local_player->AimSync.Position[2];
							local_float_temp[3] = local_player->AimSync.FrontVector[0];
							local_float_temp[4] = local_player->AimSync.FrontVector[1];
							local_float_temp[5] = local_player->AimSync.FrontVector[2];
							if(local_player_weapon_id != WEAPON_SNIPER)
							{
								UpVectorFromFrontVector(&local_float_temp[3],&local_float_temp[6]);
								C3DVector vec(local_float_temp[3],local_float_temp[4],local_float_temp[5]);
								vec.MoveAroundAxis(&local_float_temp[6],(-1.7f * (float)(M_PI / 180.f)));
								local_float_temp[3] = vec.X;
								local_float_temp[4] = vec.Y;
								local_float_temp[5] = vec.Z + 0.053f;
							}
							// set type
							local_player_type = DAMAGE_TYPE_BULLET_FROM_PASSENGER;
							// complete
							WeaponReload[local_player_id] = WeaponsReloadTime[local_player_weapon_id];
							break;
						}
					}
					break;
				}
			case PLAYER_STATE_ONFOOT:
				{
					if(WeaponReload[local_player_id] > 0) break;
					if(local_player->FootSync.KeysOther & KEY_SPRINT) break;
					if(local_player->AimSync.WeaponState == WEAPONSTATE_RELOADING) break;
					if(local_player->AimSync.WeaponState == WEAPONSTATE_NO_BULLETS) break;
					// get weapon
					local_player_weapon_id = static_cast<unsigned int>(local_player->FootSync.PlayerWeapon);
					// get key state
					local_player_keys = ((local_player->FootSync.KeysOther & KEY_FIRE) != 0);
					if(local_player_keys)
					{
						switch(GetWeaponType(local_player_weapon_id))
						{
						case WEAPON_TYPE_PROJECTILE:
							{
								if(Attacked[local_player_id] == 0)
								{
									if(StartAgility[local_player_id] == 0)
									{
										StartAgility[local_player_id] = GetTickCount();
										EndAgility[local_player_id] = GetTickCount();
									}
									else EndAgility[local_player_id] = GetTickCount();
								}
								break;
							}
						case WEAPON_TYPE_MEELE:
							{
								if(Attacked[local_player_id] == 0)
								{
									// get current pos
									local_float_temp[0] = local_player->FootSync.Position[0];
									local_float_temp[1] = local_player->FootSync.Position[1];
									local_float_temp[2] = local_player->FootSync.Position[2];
									local_float_temp[3] = local_player->Angle * 0.0174532925199433f;
									// move coords
									local_float_temp[0] += (1.0f * sin(-local_float_temp[3]));
									local_float_temp[1] += (1.0f * cos(-local_float_temp[3]));
									// type
									local_player_type = DAMAGE_TYPE_MEELE;
									// reload
									WeaponReload[local_player_id] = WeaponsReloadTime[local_player_weapon_id];
								}
								break;
							}
						default:
							{
								local_float_temp[0] = local_player->AimSync.Position[0];
								local_float_temp[1] = local_player->AimSync.Position[1];
								local_float_temp[2] = local_player->AimSync.Position[2];
								local_float_temp[3] = local_player->AimSync.FrontVector[0];
								local_float_temp[4] = local_player->AimSync.FrontVector[1];
								local_float_temp[5] = local_player->AimSync.FrontVector[2];
								switch(local_player_weapon_id) 
								{
									case WEAPON_SNIPER:
									case WEAPON_ROCKETLAUNCHER:
									case WEAPON_HEATSEEKER: 
										break;
									case WEAPON_RIFLE: 
										AdjustVector(&local_float_temp[3], 0.016204f, 0.009899f, 0.047177f);
										break;
									case WEAPON_AK47:
									case WEAPON_M4:
										AdjustVector(&local_float_temp[3], 0.026461f, 0.013070f, 0.069079f);
										break;
									default:
										AdjustVector(&local_float_temp[3], 0.043949f, 0.015922f, 0.103412f);
										break;
								}
								// type
								if(GetWeaponType(local_player_weapon_id) == WEAPON_TYPE_SHOOT) local_player_type = DAMAGE_TYPE_BULLET;
								else local_player_type = DAMAGE_TYPE_RAKET;
								// reload
								WeaponReload[local_player_id] = WeaponsReloadTime[local_player_weapon_id];
								break;
							}
						}
					}
					else if(Attacked[local_player_id] == 1) Attacked[local_player_id] = 0;

					if(((StartAgility[local_player_id] != 0) && (local_player_keys == 0)) || ((EndAgility[local_player_id] - StartAgility[local_player_id]) >= PROJECTILE_MAX_AGILITY))
					{
						EndAgility[local_player_id] -= StartAgility[local_player_id];
						if(EndAgility[local_player_id] > PROJECTILE_MAX_AGILITY) EndAgility[local_player_id] = PROJECTILE_MAX_AGILITY;
						// dist
						local_float_temp[3] = PROJECTILE_START_DIST + (PROJECTILE_DIST_BY_AGILITY * EndAgility[local_player_id]);
						// angle
						local_float_temp[4] = local_player->Angle * 0.0174532925199433f;
						// pos
						local_float_temp[0] = local_player->FootSync.Position[0] + (local_float_temp[3] * sin(-local_float_temp[4])); 
						local_float_temp[1] = local_player->FootSync.Position[1] + (local_float_temp[3] * cos(-local_float_temp[4]));
						local_float_temp[2] = local_player->FootSync.Position[2];
						// attacked
						Attacked[local_player_id] = 1;
						// type
						local_player_type = DAMAGE_TYPE_GRENADE;
					}
					break;
				}
			}

			if((local_player_type == 0) && (local_player_knock == 0)) continue;

			// npcs
			for(local_npc_id = 0;local_npc_id < MAX_NPCS;local_npc_id++)
			{
				// if valid
				if(!pNPC[local_npc_id]) continue;
				if(local_npc_id == local_player_id) continue;
				if(pNPC[local_npc_id]->IsImpregnable()) continue;
				if(!local_player->IsStreamed[local_npc_id]) continue;

				local_npc_health = pNPC[local_npc_id]->GetHealth();
				
				if(local_npc_health <= 0.0) continue;

				if(local_player_knock) pNPC[local_npc_id]->VehicleKnock(local_player_id,
					local_player_position[0],local_player_position[1],local_player_position[2],
					local_player_velocity[0],local_player_velocity[1],local_player_velocity[2]);
				
				if(local_player_type == 0) continue;

				// switch type
				switch(local_player_type)
				{
				case DAMAGE_TYPE_RAKET_FROM_VEHICLE:
				case DAMAGE_TYPE_BULLET_FROM_VEHICLE:
				case DAMAGE_TYPE_BULLET_FROM_PASSENGER:
					{
						if(pNPC[local_npc_id]->VehicleDamage(local_player_id,
							local_float_temp[0],local_float_temp[1],local_float_temp[2],
							local_float_temp[3],local_float_temp[4],local_float_temp[5],
							local_npc_health,local_player_weapon_id,local_player_type)) goto end_check;
						break;
					}
				case DAMAGE_TYPE_RAKET:
				case DAMAGE_TYPE_GRENADE:
				case DAMAGE_TYPE_MEELE:
				case DAMAGE_TYPE_BULLET:
					{
						if(pNPC[local_npc_id]->WeaponDamage(local_player_id,
							local_float_temp[0],local_float_temp[1],local_float_temp[2],
							local_float_temp[3],local_float_temp[4],local_float_temp[5],
							local_npc_health,local_player_weapon_id,local_player_type)) goto end_check;
						break;
					}
				}
			}

			// goto point
			end_check:
			if((StartAgility[local_player_id] != 0) && (local_player_keys == 0))
			{
				StartAgility[local_player_id] = 0;
				EndAgility[local_player_id] = 0;
			}
		}
		SLEEP(10);
	}
THREAD_END

THREAD_START(ReloadThread){
	int playerid;
	while(true)
	{
		for(playerid = 0;playerid < MAX_PLAYERS;playerid++)
		{
			if(WeaponReload[playerid] > 0) WeaponReload[playerid]--;
		}
		SLEEP(10);
	}
THREAD_END