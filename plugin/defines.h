/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: defines
 */

#define MAX_NPCS								500
#define MAX_NODES								100

#define NPC_WALKING_DISTANCE					0.1552086f * 600 // 0.1552086 100 ms
#define NPC_RUNING_DISTANCE						0.56444f * 600 // 0.56444 100 ms
#define NPC_SPRINTING_DISTANCE					0.926784f * 600 // 0.926784 100 ms

#define TARGET_CHECK_ROUND						1.0f
#define Z_CORRECT								0.5f
#define PROJECTILE_START_DIST					6.227f
#define PROJECTILE_MAX_AGILITY					485
#define PROJECTILE_DIST_BY_AGILITY				0.053999f

#define MAX_NPC_NAME							24

#define NPC_STATE_REQUEST_CLASS					0
#define NPC_STATE_ONFOOT						1
#define NPC_STATE_DRIVER						2
#define NPC_STATE_PASSENGER						3
#define NPC_STATE_DEATH							4
#define	NPC_STATE_PLAYBACK						5

#define BODYPART_HEAD							1
#define	BODYPART_TORSO							2
#define BODYPART_FOOT							3
#define BODYPART_ALL							4

#define	NPC_RECORD_END_REASON_END				1
#define	NPC_RECORD_END_REASON_DEATH				2
#define	NPC_RECORD_END_REASON_STOP				3
#define NPC_RECORD_END_REASON_DESTROY			4

#define UPDATE_TYPE_NONE						0
#define UPDATE_TYPE_ONFOOT						1
#define UPDATE_TYPE_INCAR						2
#define UPDATE_TYPE_PASSENGER					3

#define DAMAGE_TYPE_RAKET_FROM_VEHICLE			1
#define DAMAGE_TYPE_BULLET_FROM_VEHICLE			2
#define DAMAGE_TYPE_BULLET_FROM_PASSENGER		3
#define DAMAGE_TYPE_RAKET						4
#define DAMAGE_TYPE_GRENADE						5
#define DAMAGE_TYPE_MEELE						6
#define DAMAGE_TYPE_BULLET						7


#define INVALID_NODE_ID				-1
// типы путей
#define VEHICLE_NODES				1
#define PED_NODES					2