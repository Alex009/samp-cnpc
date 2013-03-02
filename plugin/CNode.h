/*
 *	Copyright (C) 2012 Alex009 (Алексей Михайлов)
 *	License read in license.txt
 *	Description: GTA SA Node reader
 *	Last edit at 30.09.12
 */

#pragma once

#include "NodesStructs.h"

#define MAX_NODES	500

class CNode
{
public:
	CNode(char* nodename);
	~CNode();

	void GetInfo(unsigned long* nodes,unsigned long* vehnodes,unsigned long* pednodes,unsigned long* navinodes);
	void SetPoint(unsigned long pointid);
	void GetPos(float* x,float* y,float* z);
	unsigned long GetLinkId();
	unsigned short GetAreaId();
	unsigned long GetPointId();
	unsigned char GetPathWidth();
	unsigned char GetNodeType();
	unsigned char GetLinkCount();
	unsigned char GetTrafficLevel();
	bool IsRoadBlock();
	bool IsBoats();
	bool IsEmergency();
	bool IsNotHighway();
	bool IsSpawn();
	bool IsRoadBlock1();
	bool IsParking();
	bool IsRoadBlock2();
	void SetLink(unsigned long linkid);
	unsigned short GetLinkAreaId();
	unsigned long GetLinkNodeId();

private:
	NodeHeader	head;
	NodePath*	path_data;
	NodeLink*	link_data;
	unsigned long	cur_path;
	unsigned long	cur_link;
};

