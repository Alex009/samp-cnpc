/*
 *	Copyright (C) 2012 Alex009
 *	License read in license.txt
 *	Description: Node path
 *	Last edit at 30.09.12
 */

#pragma once

#define MAX_NODE_PATHS	500

class CNodePath
{
public:
	CNodePath(unsigned long* _path,unsigned long _pathlen,float _pathdist);
	~CNodePath();

	unsigned long GetPathNodeid(unsigned long pointid);
	unsigned long GetPathLen() {return pathlen;}
	float GetPathDist() {return pathdist;}

private:
	unsigned long*	path;		// массив точек пути
	unsigned long	pathlen;	// количество точек пути
	float			pathdist;	// дистанция пути
};