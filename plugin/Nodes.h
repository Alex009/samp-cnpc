/*
*	Created:			09.04.10
*	Author:				009
*	Last Modifed:		-
*/

// plugin
#include "os.h"
#include "NodesStructs.h"

class CNode
{
public:
	CNode(char* nodename);
	~CNode();

	void	GetInfo(int* nodes,int* vehnodes,int* pednodes,int* navinodes);
	// paths
	void	SetPoint(int pointid);
	void	GetPos(float* x,float* y,float* z);
	int		GetLinkId();
	int		GetAreaId();
	int		GetPointId();
	int		GetPathWidth();
	int		GetNodeType();
	int		GetLinkCount();
	int		GetTrafficLevel();
	int		IsRoadBlock();
	int		IsBoats();
	int		IsEmergency();
	int		IsNotHighway();
	int		IsSpawn();
	int		IsRoadBlock1();
	int		IsParking();
	int		IsRoadBlock2();
	// links
	void	SetLink(int linkid);
	int		GetLinkAreaId();
	int		GetLinkNodeId();

private:
	FILE*		NodeFile;
	NodeHeader	header;
	NodePath	PathTmp;
	NodeLink	LinkTmp;
	int			CurrentPoint;
};

