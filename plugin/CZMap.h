/*
*	Created:			12.05.10
*	Author:				009
*	Last Modifed:		-
*/

// plugin
#include "os.h"
// core
#include <stdio.h>

#define ZMAP_MODE_NOCHACHE 0
#define ZMAP_MODE_CHACHE 1

class CZMap
{
public:
	CZMap(char* _mapname,int _mode);
	~CZMap();

	float	GetZForCoords(float X,float Y);
	bool	IsPointToPointLOS(float sx,float sy,float sz,float ex,float ey,float ez);
	
private:
	int				mode;
	FILE*			MapFile;
	unsigned long	CurrentPoint;
	unsigned short	PointData;
	unsigned char*	binarydata;
};