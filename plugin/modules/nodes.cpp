/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: natives for pawn
 */

#define _USE_MATH_DEFINES

#include <stdio.h>
#include <malloc.h>
#include <math.h>
// SDK
#include "../SDK/amx/amx.h"
#include "../SDK/plugincommon.h"
// plugin
#include "../Nodes.h"
#include "../defines.h"

typedef void (*logprintf_t)(char* format, ...);
extern logprintf_t logprintf;

struct NodeInfo						// данные каждой точки навигационного графа
{
	float			ni_pos[3];		// позиция	
	unsigned char	ni_areaid;		// номер квадрата точки
	unsigned short	ni_nodeid;		// номер точки в квадрате
	unsigned short*	ni_links;		// связи
	float*			ni_links_dist;	// расстояние от точки до её связей
	unsigned char	ni_linkcount;	// количество связей
};
struct LinkInfo
{
	unsigned short* links;
	unsigned short	count;
};

NodeInfo*		node_vehicle;			// массив точек транспортного навигационного графа
LinkInfo		node_vehicle_link[64];	// массив ссылок точек в файлах путей (64), на точки в общем массиве транспортного навигационного графа
unsigned short	node_vehicle_count = 0;	// количество загруженных точек транспортного навигационного графа
NodeInfo*		node_ped;				// массив точек пешеходного навигационного графа
LinkInfo		node_ped_link[64];		// массив ссылок точек в файлах путей (64), на точки в общем массиве пешеходного навигационного графа
unsigned short	node_ped_count = 0;		// количество загруженных точек пешеходного навигационного графа


// nodes_OnInit();
static cell AMX_NATIVE_CALL n_nodes_OnInit( AMX* amx, cell* params )
{
	char buffer[32];
	float tmp[3];
	int header[4];
	CNode* areafile[64];
	unsigned short nodeid;
	unsigned short i;
	unsigned short vehicle_count = 0,ped_count = 0;
	// информация
	logprintf("Controllable NPC nodes:");	
	logprintf("start load...");
	// открываем все файлы путей
	for(nodeid = 0;nodeid < 64;nodeid++)
	{
		// открываем файл путей
	    sprintf(buffer,"Nodes/NODES%d.DAT",nodeid);
		areafile[nodeid] = new CNode(buffer);
		// получаем данные количества точек
		areafile[nodeid]->GetInfo(&header[0],&header[1],&header[2],&header[3]);
		// устанавливаем линковку
		node_vehicle_link[nodeid].count = header[1];
		node_vehicle_link[nodeid].links = new unsigned short[ header[1] ];
		node_ped_link[nodeid].count = header[2];
		node_ped_link[nodeid].links = new unsigned short[ header[2] ];
		// считаем количество
		vehicle_count += header[1];
		ped_count += header[2];
	}
	// информация
	logprintf("read nodes");	
	// читаем точки путей
	for(nodeid = 0;nodeid < 64;nodeid++)
	{
	    // получаем количество точек путей
		areafile[nodeid]->GetInfo(&header[0],&header[1],&header[2],&header[3]);
		// считываем все транспортные пути
		for(i = 0;i < header[1];i++)
		{
			areafile[nodeid]->SetPoint(i);
			areafile[nodeid]->GetPos(&tmp[0],&tmp[1],&tmp[2]);
			// добавляем её в массив точек
			node_vehicle[node_vehicle_count].ni_pos[0] = tmp[0];
			node_vehicle[node_vehicle_count].ni_pos[1] = tmp[1];
			node_vehicle[node_vehicle_count].ni_pos[2] = tmp[2];
			node_vehicle[node_vehicle_count].ni_areaid = nodeid;
			node_vehicle[node_vehicle_count].ni_nodeid = i;
			*(int*)(&node_vehicle[node_vehicle_count].ni_links) = areafile[nodeid]->GetLinkId();
			node_vehicle[node_vehicle_count][ni_linkcount] = GetNodePointLinkCount(areafile[nodeid]);
			if(node_vehicle[node_vehicle_count][ni_linkcount] > LINKS_COUNT) {printf("[nodes_OnInit] ERROR! vehicle linkcount %d",node_vehicle[node_vehicle_count][ni_linkcount]);return 0;}
			// линкуем текущую точку
			node_vehicle_link[nodeid][i] = node_vehicle_count + 1;
			// увеличиваем количество загруженных точек
			node_vehicle_count++;
			// проверяем лимит загруженных точек
			if(node_vehicle_count == MAX_VEHICLE_NODES_TO_LOAD) {printf("[nodes_OnInit] ERROR! nodes vehicle count limit!");return 0;}
		}
		// считываем все пешеходные пути
		for(new i = 0;i < header[2];i++)
		{
			SetNodePoint(areafile[nodeid],i + header[1]);
			GetNodePointPos(areafile[nodeid],tmp[0],tmp[1],tmp[2]);
			// добавляем её в массив точек
			node_ped[node_ped_count][ni_pos][0] = tmp[0];
			node_ped[node_ped_count][ni_pos][1] = tmp[1];
			node_ped[node_ped_count][ni_pos][2] = tmp[2];
			node_ped[node_ped_count][ni_areaid] = nodeid;
			node_ped[node_ped_count][ni_nodeid] = i + header[1];
			node_ped[node_ped_count][ni_links][0] = GetNodePointLinkId(areafile[nodeid]);
			node_ped[node_ped_count][ni_linkcount] = GetNodePointLinkCount(areafile[nodeid]);
			if(node_ped[node_ped_count][ni_linkcount] > LINKS_COUNT) {printf("[nodes_OnInit] ERROR! ped linkcount %d",node_ped[node_ped_count][ni_linkcount]);return 0;}
			// линкуем текущую точку
			node_ped_link[nodeid][i] = node_ped_count + 1;
			// увеличиваем количество загруженных точек
			node_ped_count++;
			// проверяем лимит загруженных точек
			if(node_ped_count == MAX_PED_NODES_TO_LOAD) {printf("[nodes_OnInit] ERROR! nodes ped count limit!");return 0;}
		}
	}
	// информация
	printf("read vehicle links");	
	// читаем связи транспортных точек
	for(new nodeid = 0;nodeid < node_vehicle_count;nodeid++)
	{
		new lid = node_vehicle[nodeid][ni_links][0];
		new areaid = node_vehicle[nodeid][ni_areaid];
		new lc = 0;
		for(new i = 0;i < node_vehicle[nodeid][ni_linkcount];i++)
		{
			SetNodeLink(areafile[areaid],(lid + i));
			new pointid = node_vehicle_link[GetNodeLinkAreaId(areafile[areaid])][GetNodeLinkNodeId(areafile[areaid])];
			if(pointid != 0)
			{
				pointid--;
				node_vehicle[nodeid][ni_links][lc] = pointid;
				// вычисляем расстояние от текущей вершины до связанной
				tmp[0] = node_vehicle[pointid][ni_pos][0] - node_vehicle[nodeid][ni_pos][0];
				tmp[1] = node_vehicle[pointid][ni_pos][1] - node_vehicle[nodeid][ni_pos][1];
				tmp[2] = node_vehicle[pointid][ni_pos][2] - node_vehicle[nodeid][ni_pos][2];
				node_vehicle[nodeid][ni_links_dist][lc] = floatsqroot((tmp[0]*tmp[0]) + (tmp[1]*tmp[1]) + (tmp[2]*tmp[2]));
				lc++;
			}
			else printf("[nodes_OnInit] ERROR! vehicle node point %d [%d - %d] have unexist link!",nodeid,node_vehicle[nodeid][ni_areaid],node_vehicle[nodeid][ni_nodeid]);
		}
		node_vehicle[nodeid][ni_linkcount] = lc;
	}
	// информация
	printf("read ped links");	
	// читаем связи пешеходных точек
	for(new nodeid = 0;nodeid < node_ped_count;nodeid++)
	{
		new lid = node_ped[nodeid][ni_links][0];
		new areaid = node_ped[nodeid][ni_areaid];
		new lc = 0;	
		for(new i = 0;i < node_ped[nodeid][ni_linkcount];i++)
		{
			SetNodeLink(areafile[areaid],(lid + i));
			new pareaid = GetNodeLinkAreaId(areafile[areaid]);
			GetNodeHeader(areafile[pareaid],header[0],header[1],header[2],header[3]);
			new pnodeid = GetNodeLinkNodeId(areafile[areaid]) - header[1];
			new pointid = node_ped_link[pareaid][pnodeid];
			if(pointid != 0)
			{
				pointid--;
				node_ped[nodeid][ni_links][lc] = pointid;
				// вычисляем расстояние от текущей вершины до связанной
				tmp[0] = node_ped[pointid][ni_pos][0] - node_ped[nodeid][ni_pos][0];
				tmp[1] = node_ped[pointid][ni_pos][1] - node_ped[nodeid][ni_pos][1];
				tmp[2] = node_ped[pointid][ni_pos][2] - node_ped[nodeid][ni_pos][2];
				node_ped[nodeid][ni_links_dist][lc] = floatsqroot((tmp[0]*tmp[0]) + (tmp[1]*tmp[1]) + (tmp[2]*tmp[2]));
				lc++;
			}
			else printf("[nodes_OnInit] ERROR! ped node point %d [%d - %d] have unexist link [%d - %d]!",nodeid,node_ped[nodeid][ni_areaid],node_ped[nodeid][ni_nodeid],pareaid,pnodeid);
		}
		node_ped[nodeid][ni_linkcount] = lc;
	}
	// информация
	printf("end load...");	
	// закрываем все файлы путей
	for(new nodeid = 0;nodeid < 64;nodeid++)
	{
	    CloseNode(areafile[nodeid]);
	}
	// информация
	printf("vehicle nodes: %d",node_vehicle_count);	
	printf("ped nodes: %d",node_ped_count);	
	return 1;
}

AMX_NATIVE_INFO Natives[ ] =
{
	{ "nodes_OnInit",				n_nodes_OnInit},
	{ 0,							0 }
};

void NodesModuleOnAMXLoad(AMX* amx)
{
	amx_Register(amx, Natives, -1 );
}
