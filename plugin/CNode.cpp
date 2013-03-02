/*
 *	Copyright (C) 2012 Alex009 (Алексей Михайлов)
 *	License read in license.txt
 *	Description: GTA SA Node reader
 *	Last edit at 08.09.12
 */

// core
#include <stdio.h>
// plugin
#include "os.h"
#include "CNode.h"

CNode::CNode(char* nodename)
{
	char* buf = new char[128];
	// получаем путь
	sprintf(buf,"scriptfiles/%s",nodename);
	// открываем файл
	FILE* input = fopen(buf,"rb");
	if(input)
	{
		// читаем заголовок
		fread(&head,1,sizeof(NodeHeader),input);
		// создаём массивы под пути и ссылки
		unsigned long nodescount = head.NumVehicleNodes + head.NumPedNodes;
		path_data = new NodePath[ nodescount ];
		link_data = new NodeLink[ head.NumLinks ];
		// читаем точки путей
		for(unsigned short i = 0;i < nodescount;i++) fread(&path_data[i],1,sizeof(NodePath),input);
		// ставим указатель на начало ссылок
		fseek(input,(sizeof(NodeHeader) + ((head.NumVehicleNodes + head.NumPedNodes) * sizeof(NodePath)) + (head.NumNaviNodes * sizeof(NodeNavi))),SEEK_SET);
		// читаем ссылки путей
		for(unsigned short i = 0;i < head.NumLinks;i++) fread(&link_data[i],1,sizeof(NodeLink),input);
		// закрываем файл
		fclose(input);
	}
	else
	{
		// файл не открылся
		path_data = NULL;
		link_data = NULL;
	}
	// устанавливаем стандартные данные
	cur_path = 0;
	cur_link = 0;
}

CNode::~CNode()
{
	if(path_data) delete [] path_data;
	if(link_data) delete [] link_data;
}

void CNode::GetInfo(unsigned long* nodes,unsigned long* vehnodes,unsigned long* pednodes,unsigned long* navinodes)
{
	*nodes = head.NumNodes;
	*vehnodes = head.NumVehicleNodes;
	*pednodes = head.NumPedNodes;
	*navinodes = head.NumNaviNodes;
}

void CNode::SetPoint(unsigned long pointid)
{
	if(pointid > (head.NumVehicleNodes + head.NumPedNodes)) return;

	cur_path = pointid;
}

void CNode::GetPos(float* x,float* y,float* z)
{
	*x = (float)(path_data[cur_path].PosX * 0.125);
	*y = (float)(path_data[cur_path].PosY * 0.125);
	*z = (float)(path_data[cur_path].PosZ * 0.125);
}

unsigned long CNode::GetLinkId()
{
	return path_data[cur_path].LinkId;
}

unsigned short CNode::GetAreaId()
{
	return path_data[cur_path].AreaId;
}

unsigned long CNode::GetPointId()
{
	return cur_path;
}

unsigned char CNode::GetPathWidth()
{
	return path_data[cur_path].PathWidth;
}

unsigned char CNode::GetNodeType()
{
	return path_data[cur_path].NodeType;
}

unsigned char CNode::GetLinkCount()
{
	return (path_data[cur_path].Flags & 0xF);
}

unsigned char CNode::GetTrafficLevel()
{
	return (path_data[cur_path].Flags & 0x30);
}

bool CNode::IsRoadBlock()
{
	return ((path_data[cur_path].Flags & 0x40) == 1);
}

bool CNode::IsBoats()
{
	return ((path_data[cur_path].Flags & 0x80) == 1);
}

bool CNode::IsEmergency()
{
	return ((path_data[cur_path].Flags & 0x100) == 1);
}

bool CNode::IsNotHighway()
{
	return ((path_data[cur_path].Flags & 0x1000) == 1);
}

bool CNode::IsSpawn()
{
	return ((path_data[cur_path].Flags & 0xF0000) == 1);
}

bool CNode::IsRoadBlock1()
{
	return ((path_data[cur_path].Flags & 0x100000) == 1);
}

bool CNode::IsParking()
{
	return ((path_data[cur_path].Flags & 0x200000) == 1);
}

bool CNode::IsRoadBlock2()
{
	return ((path_data[cur_path].Flags & 0x800000) == 1);
}

void CNode::SetLink(unsigned long linkid)
{
	if(linkid > head.NumLinks) return;

	cur_link = linkid;
}

unsigned short CNode::GetLinkAreaId()
{
	return link_data[cur_link].AreaId;
}

unsigned long CNode::GetLinkNodeId()
{
	return link_data[cur_link].NodeId;
}

