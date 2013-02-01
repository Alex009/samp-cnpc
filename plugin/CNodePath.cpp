/*
 *	Copyright (C) 2012 Alex009
 *	License read in license.txt
 *	Description: Node path
 *	Last edit at 30.09.12
 */

#include <stdlib.h>
#include "CNodePath.h"
#include "CNodeManager.h"

CNodePath::CNodePath(unsigned long* _path,unsigned long _pathlen,float _pathdist)
{
	if(_pathlen)
	{
		path = new unsigned long[_pathlen];
		for(unsigned long i = 0;i < _pathlen;i++) path[i] = _path[i];
	}
	else path = NULL;
	pathlen = _pathlen;
	pathdist = _pathdist;
}

CNodePath::~CNodePath()
{
	if(pathlen) delete [] path;
}

unsigned long CNodePath::GetPathNodeid(unsigned long pointid)
{
	if(pointid < 0) return INVALID_NODE_ID;
	if(pointid >= pathlen) return INVALID_NODE_ID;

	return path[pointid];
}