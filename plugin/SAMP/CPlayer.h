/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: Player SA-MP server class
 */

#pragma once

#include "packets.h"
#include "defines.h"

#pragma pack(push)
#pragma pack(1)

class CPlayer
{
public:
	CPlayer();
	~CPlayer();
	
	// vars
	Packet_Aim			AimSync;				// 0x0
	Packet_InCar		InCarSync;				// 0x1F
	Packet_Passanger	PassangerSync;			// 0x5E
	Packet_Foot			FootSync;				// 0x76
	unsigned char		IsStreamed[MAX_PLAYERS];
	unsigned char		unk_1[0x1FE0];			
	unsigned char		unk_2[0x93];			// 0x228E
	float				Position[3];			// 0x2321
	float				Health;					// 0x232D
	float				Armour;					// 0x2331
	unsigned char		unk_3[0x10];			// 0x2335
	float				Angle;					// 0x2345
	unsigned char		unk_4[0x2C];			// 0x2349
/*
	unsigned short		k_leftright;			// 0x2355
	unsigned short		k_updown;				// 0x2357
	unsigned short		k_others;				// 0x2359
*/
	unsigned long		SyncType;				// 0x2375
	unsigned char		unk_5[0x234];			// 0x2379
	unsigned long		AimSyncSendState;		// 0x25AD
	unsigned char		unk_6[0x8];				// 0x25B1
	unsigned char		State;					// 0x25B9
	unsigned char		unk_7[0x39];			// 0x25BA
	unsigned short		WeaponSkill[11];		// 0x25F3
	unsigned char		unk_8[0x5];				// 0x2609
	unsigned long		Skin;					// 0x260E
	unsigned char		unk_9[0x2F];			// 0x2612
	unsigned char		VehicleSeat;			// 0x2641
	unsigned short		VehicleId;				// 0x2642
	unsigned char		unk_10[0x3E];			// 0x2612
	unsigned long		Interior;				// 0x2650
	unsigned short		TargetId;				// 0x2696
};



#pragma pack(pop)