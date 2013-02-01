/*
*	Created:			12.05.10
*	Author:				009
*	Last Modifed:		-
*/

// plugin
#include "os.h"
// core
#include <stdio.h>

class CZMap
{
public:
	CZMap(char* mapname);
	~CZMap();

	float	GetZForCoords(float X,float Y);
	
private:
	FILE*			MapFile;
	int				CurrentPoint;
	unsigned short	PointData;
};