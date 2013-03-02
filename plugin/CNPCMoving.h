/*
 *	Copyright (C) 2013 Alex009
 *	License read in license.txt
 *	Description: Controllable NPCs moving class
 */

#pragma once

#include "CNPCCore.h"

class CNPCMoving
{
public:
	CNPCMoving(CNPCCore* _pNPC);

	bool Move(float _x,float _y,float _z,float _velocity,bool _zmap,bool _allaxisrotate);
	bool Update(double update_time);

protected:
	CNPCCore*	pNPC;
	float		distance;
	float		vector[3];
	bool		zmap;
	float		velocity;
	double		last_update;
};