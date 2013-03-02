/*
 *	Copyright (C) 2013 Alex009
 *	License read in license.txt
 *	Description: Controllable NPCs playback class
 */

#include "CNPCPlayback.h"
#include "utils/time.h"
#include "defines.h"
#include <string.h>

CNPCPlayback::CNPCPlayback(CNPCCore* _pNPC)
{
	// link
	pNPC = _pNPC;
	// null data
	pFile = NULL;
	type = 0;
	start_time = 0;
	current_block = NULL;
	current_time = 0;
	paused = false;
}

CNPCPlayback::~CNPCPlayback()
{
	if(pFile) fclose(pFile);
	if(current_block) delete current_block;
}

bool CNPCPlayback::Start(char* name)
{
	unsigned long tmp;
	char* buf = new char[128];
	// path
	sprintf(buf,"scriptfiles/%s.rec",name);
	// try open
	pFile = fopen(buf,"rb");
	if(pFile)
	{
		paused = false;
		start_time = GetTimeFromStart();
		// read main info
		fread(&tmp,1,4,pFile);
		fread(&type,1,4,pFile);
		// create buffer and read first block
		switch(type)
		{
		case PLAYER_RECORDING_TYPE_DRIVER:
			{
				current_block = new Packet_InCar;
				fread(&current_time,1,4,pFile);
				fread(current_block,1,sizeof(Packet_InCar),pFile);
				break;
			}
		case PLAYER_RECORDING_TYPE_ONFOOT:
			{
				current_block = new Packet_Foot;
				fread(&current_time,1,4,pFile);
				fread(current_block,1,sizeof(Packet_Foot),pFile);
				break;
			}
		}
		return true;
	}
	return false;
}

bool CNPCPlayback::Next()
{
	if(feof(pFile)) return true;
	
	switch(type)
	{
	case PLAYER_RECORDING_TYPE_DRIVER:
		{
			fread(&current_time,1,4,pFile);
			fread(current_block,1,sizeof(Packet_InCar),pFile);
			break;
		}
	case PLAYER_RECORDING_TYPE_ONFOOT:
		{
			fread(&current_time,1,4,pFile);
			fread(current_block,1,sizeof(Packet_Foot),pFile);
			break;
		}
	}
	return false;
}

bool CNPCPlayback::Pause()
{
	if(paused) return false;

	pNPC->SetVelocity(0.0,0.0,0.0);
	start_time -= GetTimeFromStart();
	return true;
}

bool CNPCPlayback::Continue()
{
	if(!paused) return false;

	start_time += GetTimeFromStart();
	return true;
}

bool CNPCPlayback::Update(double update_time)
{
	if((GetTimeFromStart() - start_time) >= current_time)
	{
		CPlayer* pClass = pNPC->GetCPlayerPointer();
		switch(type)
		{
		case PLAYER_RECORDING_TYPE_DRIVER:
			{
				// save vehicle id
				unsigned short vid = pClass->InCarSync.VehicleId;
				// set data
				memcpy(&pClass->InCarSync,current_block,sizeof(Packet_InCar));
				// get pos array
				float x,y,z;
				x = ((Packet_InCar*)current_block)->Position[0];
				y = ((Packet_InCar*)current_block)->Position[1];
				z = ((Packet_InCar*)current_block)->Position[2];
				// set pos
				pNPC->SetPosition(x,y,z);
				// load vehicle id
				pClass->InCarSync.VehicleId = vid;
				break;
			}
		case PLAYER_RECORDING_TYPE_ONFOOT:
			{
				// set data
				memcpy(&pClass->FootSync,current_block,sizeof(Packet_Foot));
				// get pos array
				float x,y,z;
				x = ((Packet_Foot*)current_block)->Position[0];
				y = ((Packet_Foot*)current_block)->Position[1];
				z = ((Packet_Foot*)current_block)->Position[2];
				// set pos
				pNPC->SetPosition(x,y,z);
				break;
			}
		}
		// read next block
		if(Next()) 
		{
			// end of record
			pNPC->SetVelocity(0.0,0.0,0.0);
			pNPC->SetKeys(0,0,0);
			return true;
		}
	}
	return false;
}