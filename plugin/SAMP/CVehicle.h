/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: Vehicle SA-MP server class
 */

#pragma once

#pragma pack(push)
#pragma pack(1)

class CVehicle
{
public:
	CVehicle();
	~CVehicle();
	
	// vars
	float				Position[3];		// 0x0
	float				Quaternion[4];		// 0xC
	unsigned char		unk_1[0x4E];		// 0x1C
	unsigned short		Driver;				// 0x6A	
	unsigned char		unk_2[0x16];		// 0x6C		
	unsigned short		Model;				// 0x82
	unsigned char		unk_3[0x22];		// 0x84	
	float				Health;				// 0xA6
};

#pragma pack(pop)