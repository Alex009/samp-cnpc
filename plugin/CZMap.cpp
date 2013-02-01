/*
*	Created:			12.05.10
*	Author:				009
*	Last Modifed:		-
*/

// core
#include <stdio.h>
#include <math.h>
// plugin
#include "os.h"
#include "CZMap.h"

CZMap::CZMap(char* _mapname,int _mode)
{
	char* buf = new char[128];
	// data
	mode = _mode;
	CurrentPoint = -1;
	// path
	sprintf(buf,"scriptfiles/%s",_mapname);
	// open
	MapFile = fopen(buf,"rb");
	// mode
	if(mode == ZMAP_MODE_CHACHE)
	{
		binarydata = new unsigned char[6000 * 6000 * 2];
		fread(binarydata,6000 * 6000 * 2,1,MapFile);
		fclose(MapFile);
		MapFile = NULL;
	}
}

CZMap::~CZMap()
{
	if(MapFile) fclose(MapFile);
}

float CZMap::GetZForCoords(float X,float Y)
{
	if(X < -3000.0f || X > 3000.0f || Y > 3000.0f || Y < -3000.0f) return 0.0f;

	// get row/col on 6000x6000 grid
	unsigned short iGridX = ((int)X) + 3000;
	unsigned short iGridY = (((int)Y) - 3000) * -1;
	unsigned long Point = ((iGridY * 6000) + iGridX) * 2; // for every Y, increment by the number of cols, add the col index.

	if(mode == ZMAP_MODE_CHACHE)
	{
		if(CurrentPoint != Point)
		{
			CurrentPoint = Point;
			PointData = *(unsigned short*)(binarydata + CurrentPoint);
		}
		return (float)PointData / 100.0f;
	}
	else if(MapFile) // sanks for DarkByte's MapAndreas NOCHACHE
	{
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

bool CZMap::IsPointToPointLOS(float sx,float sy,float sz,float ex,float ey,float ez)
{
	float vx,vy,vz,len,x,y,z;
	unsigned long step,size;
	// получаем длину отрезка
	len = sqrt((ex-sx)*(ex-sx) + (ey-sy)*(ey-sy) + (ez-sz)*(ez-sz));
	size = (unsigned long)len;
	// получаем направляющий вектор прямой
	vx = (ex - sx) / len;
	vy = (ey - sy) / len;
	vz = (ez - sz) / len;
	// двигаясь по прямой проверяем Z координаты
	for(step = 0;step < size;step++)
	{
		// получаем координаты точки на прямой
		x = sx + vx * step;
		y = sy + vy * step;
		z = sz + vz * step;
		// уменьшаем длину
		len -= 1.0f;
		// проверяем высоту согласно карте высот
		if(z < GetZForCoords(x,y)) return false;
	}
	return true;
}