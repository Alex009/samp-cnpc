/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: packets SA-MP server structs
 */

#pragma once

#pragma pack(push)
#pragma pack(1)

struct Packet_Foot // 0x224A - 0x228E
{
	unsigned short	KeysLR;	// 0x224A - 0x0
	unsigned short	KeysUD; // 0x224C - 0x2
	unsigned short	KeysOther; // 0x224E - 0x4
	float			Position[3]; // 0x48 - 0x6
	float			Quaternion[4]; // 0x54 - 0x12
	unsigned char	PlayerHealth; // 0x64 - 0x22
	unsigned char	PlayerArmour; // 0x65 - 0x23
	unsigned char	PlayerWeapon; // 0x66 - 0x24
	unsigned char	SpecialAction; // 0x67 - 0x25
	float			Velocity[3]; // 0x68 - 0x26
	float			Surfing[3]; // 0x74 - 0x32
	unsigned short	SurfingInfo; // 0x80 - 0x3E
	unsigned long	AnimationIndex; // 0x82 - 0x40
};

struct Packet_Aim // 0x21D4 - 0x21F3
{
	unsigned char	CameraMode;			// 0x21D4 - 0x0
	float			FrontVector[3];		// 0x21D5 - 0x1
	float			Position[3];		// 0x21E1 - 0xD
	float			ZAim;				// 0x21ED - 0x19
	unsigned char	CamExtZoom : 6;		// 0x21F1 - 0x1D
	unsigned char	WeaponState : 2;
	unsigned char	Unknown1;			// 0x21F2 - 0x1E
};

struct Packet_InCar // 0x86 - 0xC5
{
	unsigned short	VehicleId; // 0x86 - 0x0
	unsigned short	KeysLR; // 0x88 - 0x2
	unsigned short	KeysUD; // 0x8A - 0x4
	unsigned short	KeysOther; // 0x8C - 0x6
	float			Quaternion[4]; // 0x8E - 0x8
	float			Position[3]; // 0x9E - 0x18
	float			Velocity[3]; // 0xAA - 0x24
	float			Health; // 0xB6 - 0x30
	unsigned char	PlayerHealth; // 0xBA - 0x34
	unsigned char	PlayerArmour; // 0xBB - 0x35
	unsigned char	PlayerWeapon; // 0xBC - 0x36
	unsigned char	SirenState; // 0xBD - 0x37
	unsigned char	LandingGearState; // 0xBE - 0x38
	unsigned short	TrailerId_or_ThrustAngle; // 0xBF - 0x39
	float			TrainSpeed; // 0xC1 - 0x3B
}; // 0x3F

struct Packet_Passanger // 0xC5 - 0xDD
{
	unsigned short	VehicleId; // 0xC5
	unsigned char	Seat : 6; // 0xC7
	unsigned char	DriveBy : 2; 
	unsigned char	Weapon; // 0xC8
	unsigned char	Health; // 0xC9
	unsigned char	Armour; // 0xCA
	unsigned short	KeysLR; // 0xCB
	unsigned short	KeysUD; // 0xCD
	unsigned short	KeysOther; // 0xCF
	float			Position[3]; // 0xD1
};	// 0xDD

#pragma pack(pop)