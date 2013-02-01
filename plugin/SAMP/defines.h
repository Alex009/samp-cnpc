/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: SA-MP server defines
 */

#pragma once

#define INVALID_PLAYER_ID						(0xFFFF)
#define INVALID_VEHICLE_ID						(0xFFFF)

#define MAX_VEHICLES							2000
#define MAX_PLAYERS								500
#define	MAX_WEAPONS								46

#define PLAYER_RECORDING_TYPE_NONE				0
#define PLAYER_RECORDING_TYPE_DRIVER			1
#define PLAYER_RECORDING_TYPE_ONFOOT			2

#define PLAYER_STATE_NONE						0
#define PLAYER_STATE_ONFOOT						1
#define PLAYER_STATE_DRIVER						2
#define PLAYER_STATE_PASSENGER					3
#define PLAYER_STATE_EXIT_VEHICLE				4 
#define PLAYER_STATE_ENTER_VEHICLE_DRIVER		5 
#define PLAYER_STATE_ENTER_VEHICLE_PASSENGER	6
#define PLAYER_STATE_WASTED						7
#define PLAYER_STATE_SPAWNED					8
#define PLAYER_STATE_SPECTATING					9

#define KEY_UP									-128
#define KEY_DOWN								128
#define KEY_LEFT								-128
#define KEY_RIGHT								128
#define KEY_ACTION								1
#define KEY_CROUCH								2
#define KEY_FIRE								4
#define KEY_SPRINT								8
#define KEY_SECONDARY_ATTACK					16
#define KEY_JUMP								32
#define KEY_LOOK_RIGHT							64
#define KEY_HANDBRAKE							128
#define KEY_LOOK_LEFT							256
#define KEY_SUBMISSION							512
#define KEY_LOOK_BEHIND							512
#define KEY_WALK								1024
#define KEY_ANALOG_UP							2048
#define KEY_ANALOG_DOWN							4096
#define KEY_ANALOG_LEFT							8192
#define KEY_ANALOG_RIGHT						16384

#define WEAPON_BRASSKNUCKLE						1
#define WEAPON_GOLFCLUB							2
#define WEAPON_NITESTICK						3
#define WEAPON_KNIFE							4
#define WEAPON_BAT								5
#define WEAPON_SHOVEL							6
#define WEAPON_POOLSTICK						7
#define WEAPON_KATANA							8
#define WEAPON_CHAINSAW							9
#define WEAPON_DILDO							10
#define WEAPON_DILDO2							11
#define WEAPON_VIBRATOR							12
#define WEAPON_VIBRATOR2						13
#define WEAPON_FLOWER							14
#define WEAPON_CANE								15
#define WEAPON_GRENADE							16
#define WEAPON_TEARGAS							17
#define WEAPON_MOLTOV							18
#define WEAPON_COLT45							22
#define WEAPON_SILENCED							23
#define WEAPON_DEAGLE							24
#define WEAPON_SHOTGUN							25
#define WEAPON_SAWEDOFF							26
#define WEAPON_SHOTGSPA							27
#define WEAPON_UZI								28
#define WEAPON_MP5								29
#define WEAPON_AK47								30
#define WEAPON_M4								31
#define WEAPON_TEC9								32
#define WEAPON_RIFLE							33
#define WEAPON_SNIPER							34
#define WEAPON_ROCKETLAUNCHER					35
#define WEAPON_HEATSEEKER						36
#define WEAPON_FLAMETHROWER						37
#define WEAPON_MINIGUN							38
#define WEAPON_SATCHEL							39
#define WEAPON_BOMB								40
#define WEAPON_SPRAYCAN							41
#define WEAPON_FIREEXTINGUISHER					42
#define WEAPON_CAMERA							43
#define WEAPON_PARACHUTE						46
#define WEAPON_VEHICLE							49
#define WEAPON_DROWN							53
#define WEAPON_COLLISION						54

#define WEAPONSTATE_UNKNOWN				-1
#define WEAPONSTATE_NO_BULLETS			0
#define WEAPONSTATE_LAST_BULLET			1
#define WEAPONSTATE_MORE_BULLETS		2
#define WEAPONSTATE_RELOADING			3