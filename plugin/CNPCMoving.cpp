/*
 *	Copyright (C) 2013 Alex009
 *	License read in license.txt
 *	Description: Controllable NPCs moving class
 */

#include "CNPCMoving.h"
#include "utils\time.h"
#include "utils\quaternion.h"
#include <math.h>

#define VELOCITY_PER_SEC	0.02f

float GetZCoord(float x,float y);

CNPCMoving::CNPCMoving(CNPCCore* _pNPC)
{
	// link
	pNPC = _pNPC;
	// null data
	distance = 0.0;
	for(int i = 0;i < 3;i++) vector[i] = 0.0;
	zmap = false;
	velocity = 0.0;
	last_update = 0.0;
}

bool CNPCMoving::Move(float _x,float _y,float _z,float _velocity,bool _zmap,bool _allaxisrotate)
{
	float x,y,z;
	// get pos
	pNPC->GetPosition(&x,&y,&z);
	// move to
	vector[0] = _x - x;
	vector[1] = _y - y;
	vector[2] = _z - z;
	// velocity
	velocity = _velocity;
	// zmap
	zmap = _zmap;
	// distance
	distance = sqrt(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);
	// check distance (can we go by own velocity?)
	if(distance < velocity)
	{
		// we cant go, less distance
		return false;
	}
	// get moving vector (1.0 len)
	vector[0] = (vector[0] / distance);
	vector[1] = (vector[1] / distance);
	vector[2] = (vector[2] / distance);
	// get quaternion
	Quaternion quat;
	if(_allaxisrotate) quat.FromVector(vector[0],vector[1],vector[2]);
	else quat.FromVector(vector[0],vector[1],0.0);
	// apply all data to npc
	pNPC->SetQuaternion(quat.X,quat.Y,quat.Z,quat.W);
	pNPC->SetVelocity(vector[0] * velocity,vector[1] * velocity,vector[2] * velocity);
	// set time of start moving
	last_update = microtime();
	// complete
	return true;
}

bool CNPCMoving::Update(double update_time)
{
	float x,y,z;
	// get time of moving
	double moving_time = update_time - last_update;
	// calc moving distance
	float mdist = (velocity/VELOCITY_PER_SEC) * (float)moving_time;
	// check distance to destination
	if(distance < mdist) mdist = distance;
	// get pos
	pNPC->GetPosition(&x,&y,&z);
	// calc new pos
	x += vector[0] * mdist;
	y += vector[1] * mdist;
	if(zmap) z = GetZCoord(x,y) + 0.5f;
	else z += vector[2] * mdist;
	// set pos
	pNPC->SetPosition(x,y,z);
	// change distance
	distance -= mdist;
	// save last update time
	last_update = update_time;
	// check complete
	if(distance <= 0.0f) 
	{
		pNPC->SetVelocity(0.0,0.0,0.0);
		pNPC->SetKeys(0,0,0);
		return true;
	}
	return false; // moving not end
}