/*
*	Created:			12.05.10
*	Author:				009
*	Last Modifed:		-
*/

// plugin
#include "os.h"
#include "ZMap.h"
// core
#include <stdio.h>

CZMap::CZMap(char* mapname)
{
	char* buf = new char[128];
	// path
	sprintf(buf,"scriptfiles/%s",mapname);
	// open
	MapFile = fopen(buf,"rb");
	// stat
	CurrentPoint = -1;
}

CZMap::~CZMap()
{
	if(MapFile) fclose(MapFile);
}

float CZMap::GetZForCoords(float X,float Y) // sanks for DarkByte's MapAndreas NOCHACHE
{
	if(MapFile)
	{
		if(X < -3000.0f || X > 3000.0f || Y > 3000.0f || Y < -3000.0f) return 0.0f;

		// get row/col on 6000x6000 grid
		int iGridX = ((int)X) + 3000;
		int iGridY = (((int)Y) - 3000) * -1;
		int Point = ((iGridY * 6000) + iGridX) * 2; // for every Y, increment by the number of cols, add the col index.

		if(CurrentPoint != Point)
		{
			CurrentPoint = Point;
			fseek(MapFile,CurrentPoint,SEEK_SET);
			fread(&PointData,1,sizeof(unsigned short),MapFile);
		}
		return (float)PointData / 100.0f;
	}
	return 0.0;
}