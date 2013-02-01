/*
 *	Copyright (C) 2012 Alex009
 *	License read in license.txt
 *	Description: Node manager with path finder
 *	Last edit at 30.09.12
 */

#pragma once

#define INVALID_NODE_ID	0xFFFFFFFF
#define VEHICLE_NODES	1
#define PED_NODES		2

class CNodeManager
{
public:
	CNodeManager();
	~CNodeManager();

	bool GetNodePos(unsigned char nodetype,unsigned long nodeid,float* x,float* y,float* z);
	unsigned char GetNodeAreaid(unsigned char nodetype,unsigned long nodeid);
	unsigned long GetNodeNodeid(unsigned char nodetype,unsigned long nodeid);
	unsigned long GetNodeLink(unsigned char nodetype,unsigned long nodeid,unsigned char linkid);
	float GetNodeLinkDist(unsigned char nodetype,unsigned long nodeid,unsigned char linkid);
	unsigned char GetNodeLinkCount(unsigned char nodetype,unsigned long nodeid);
	unsigned long GetNodeCount(unsigned char nodetype);
	unsigned long GetNodeid(unsigned char nodetype,float x,float y,float z);
	unsigned char GetAreaid(float x,float y,float z);
	bool GetAreasFromPos(float startx,float starty,float endx,float endy,unsigned char* areas);
	unsigned long GetAreaNodeid(unsigned char nodetype,unsigned char areaid,unsigned long nodeid);
	unsigned long GetAreaNodeCount(unsigned char nodetype,unsigned char areaid);
	bool CalcPathByNodes(unsigned char* areas,unsigned char nodetype,unsigned long startnodeid,unsigned long endnodeid,unsigned long** path,unsigned long* pathlen,float* pathdist);

private:
	struct NodeInfo						// ������ ������ ����� �������������� �����
	{
		float			ni_pos[3];		// �������	
		unsigned char	ni_areaid;		// ����� �������� �����
		unsigned long	ni_nodeid;		// ����� ����� � ��������
		unsigned long*	ni_links;		// �����
		float*			ni_links_dist;	// ���������� �� ����� �� � ������
		unsigned char	ni_linkcount;	// ���������� ������
	};
	struct LinkInfo
	{
		unsigned long*	links;
		unsigned long	count;
	};

	NodeInfo*		node_vehicle;			// ������ ����� ������������� �������������� �����
	LinkInfo		node_vehicle_link[64];	// ������ ������ ����� � ������ ����� (64), �� ����� � ����� ������� ������������� �������������� �����
	unsigned long	node_vehicle_count;		// ���������� ����������� ����� ������������� �������������� �����
	NodeInfo*		node_ped;				// ������ ����� ����������� �������������� �����
	LinkInfo		node_ped_link[64];		// ������ ������ ����� � ������ ����� (64), �� ����� � ����� ������� ����������� �������������� �����
	unsigned long	node_ped_count;			// ���������� ����������� ����� ����������� �������������� �����
};