/*
 *	Copyright (C) 2013 Alex009
 *	License read in license.txt
 *	Description: Controllable NPCs class
 */

#pragma once

#include "CNPCCore.h"
#include "CNPCMoving.h"
#include "CNPCPlayback.h"
#include "CNPCDamage.h"

class CNPC: public CNPCCore
{
public:
	CNPC(unsigned short id,char* name);
	~CNPC();
	
	void	Spawn();
	void	Kill(unsigned short killerid,unsigned char reason);
	void	Stop();
	bool	Move(float x,float y,float z,float velocity,bool zmap,bool allaxisrotate);
	void	Look(float x,float y,float z);
	void	Aim(float x,float y,float z);
	void	Fire(float x,float y,float z);
	bool	StartRecordingPlayback(char* name);
	bool	PauseRecordingPlayback();
	bool	ContinueRecordingPlayback();
	bool	StopRecordingPlayback();
	bool	IsImpregnable() { return (pDamage != NULL); }
	void	SetImpregnable(bool state);
	void	Update(double update_time);

private:
	CNPCMoving*		pMoving;
	CNPCPlayback*	pPlayback;
	CNPCDamage*		pDamage;
};