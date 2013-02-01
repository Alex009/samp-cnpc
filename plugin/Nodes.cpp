/*
*	Created:			09.04.10
*	Author:				009
*	Last Modifed:		-
*/

// SDK
#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"
// core
#include <stdio.h>
// plugin
#include "os.h"
#include "Nodes.h"

CNode::CNode(char* nodename)
{
	char* buf = new char[128];
	// path
	sprintf(buf,"scriptfiles/%s",nodename);
	// open
	NodeFile = fopen(buf,"rb");
	if(NodeFile)
	{
		// read main info
		fread(&header,1,sizeof(NodeHeader),NodeFile);
		// read first segment		
		fread(&PathTmp,1,sizeof(NodePath),NodeFile);
	}
}

CNode::~CNode()
{
	if(NodeFile) fclose(NodeFile);
}

void CNode::GetInfo(int* nodes,int* vehnodes,int* pednodes,int* navinodes)
{
	if(NodeFile)
	{
		*nodes = header.NumNodes;
		*vehnodes = header.NumVehicleNodes;
		*pednodes = header.NumPedNodes;
		*navinodes = header.NumNaviNodes;
	}
}

void CNode::SetPoint(int pointid)
{
	if(NodeFile)
	{
		CurrentPoint = pointid;
		if(pointid > header.NumNodes) return;

		pointid *= sizeof(NodePath);

		fseek(NodeFile,(pointid + sizeof(NodeHeader)),0);

		fread(&PathTmp,1,sizeof(NodePath),NodeFile);
	}
}

void CNode::GetPos(float* x,float* y,float* z)
{
	if(NodeFile)
	{
		*x = (float)(PathTmp.PosX * 0.125);
		*y = (float)(PathTmp.PosY * 0.125);
		*z = (float)(PathTmp.PosZ * 0.125);
	}
}

int CNode::GetLinkId()
{
	if(NodeFile)
	{
		return (int)PathTmp.LinkId;
	}
	return 0;
}
int CNode::GetAreaId()
{
	if(NodeFile)
	{
		return (int)PathTmp.AreaId;
	}
	return 0;
}

int CNode::GetPointId()
{
	if(NodeFile)
	{
		return CurrentPoint;
	}
	return 0;
}

int CNode::GetPathWidth()
{
	if(NodeFile)
	{
		return (int)PathTmp.PathWidth;
	}
	return 0;
}

int CNode::GetNodeType()
{
	if(NodeFile)
	{
		return (int)PathTmp.NodeType;
	}
	return 0;
}

int CNode::GetLinkCount()
{
	if(NodeFile)
	{
		return (int)(PathTmp.Flags & 0xF);
	}
	return 0;
}

int CNode::GetTrafficLevel()
{
	if(NodeFile)
	{
		return (int)(PathTmp.Flags & 0x30);
	}
	return 0;
}

int CNode::IsRoadBlock()
{
	if(NodeFile)
	{
		return (int)(PathTmp.Flags & 0x40);
	}
	return 0;
}
int CNode::IsBoats()
{
	if(NodeFile)
	{
		return (int)(PathTmp.Flags & 0x80);
	}
	return 0;
}
int CNode::IsEmergency()
{
	if(NodeFile)
	{
		return (int)(PathTmp.Flags & 0x100);
	}
	return 0;
}
int CNode::IsNotHighway()
{
	if(NodeFile)
	{
		return (int)(PathTmp.Flags & 0x1000);
	}
	return 0;
}
int CNode::IsSpawn()
{
	if(NodeFile)
	{
		return (int)(PathTmp.Flags & 0xF0000);
	}
	return 0;
}
int CNode::IsRoadBlock1()
{
	if(NodeFile)
	{
		return (int)(PathTmp.Flags & 0x100000);
	}
	return 0;
}
int CNode::IsParking()
{
	if(NodeFile)
	{
		return (int)(PathTmp.Flags & 0x200000);
	}
	return 0;
}
int CNode::IsRoadBlock2()
{
	if(NodeFile)
	{
		return (int)(PathTmp.Flags & 0x800000);
	}
	return 0;
}


void CNode::SetLink(int linkid)
{
	if(NodeFile)
	{
		fseek(NodeFile,(sizeof(NodeHeader) + ((header.NumVehicleNodes + header.NumPedNodes) * sizeof(NodePath)) + (header.NumNaviNodes * sizeof(NodeNavi)) + (linkid  * sizeof(NodeLink))),0);
		fread(&LinkTmp,1,sizeof(NodeLink),NodeFile);
	}
}

int CNode::GetLinkAreaId()
{
	if(NodeFile)
	{
		return (int)LinkTmp.AreaId;
	}
	return 0;
}

int CNode::GetLinkNodeId()
{
	if(NodeFile)
	{
		return (int)(LinkTmp.NodeId);
	}
	return 0;
}

