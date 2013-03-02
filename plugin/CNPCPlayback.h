/*
 *	Copyright (C) 2013 Alex009
 *	License read in license.txt
 *	Description: Controllable NPCs playback class
 */

#pragma once

#include "CNPCCore.h"
#include <stdio.h>

class CNPCPlayback
{
public:
	CNPCPlayback(CNPCCore* _pNPC);
	~CNPCPlayback();

	bool Start(char* name);
	bool Next();
	bool Pause();
	bool Continue();
	bool Update(double update_time);

protected:
	CNPCCore*		pNPC;
	FILE*			pFile;
	unsigned long	type;
	unsigned long	start_time;
	void*			current_block;
	unsigned long	current_time;
	bool			paused;
};