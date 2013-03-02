/*
 *	Copyright (C) 2012 Alex009 (Алексей Михайлов)
 *	License read in license.txt
 *	Description: Node manager with path finder
 *	Last edit at 30.09.12
 */

#include <math.h>
#include <stdio.h>
#include "CNodeManager.h"
#include "CNode.h"
#include "os.h"

CNodeManager::CNodeManager()
{
	char buffer[32];
	float tmp[3];
	unsigned long header[4];
	CNode* areafile[64];
	unsigned long i;
	unsigned long vehicle_count = 0,ped_count = 0;
	// зануляем переменные
	node_vehicle_count = 0;
	node_ped_count = 0;
	// считаем общее количество точек
	for(unsigned char nodeid = 0;nodeid < 64;nodeid++)
	{
		// открываем файл
	    sprintf(buffer,"Nodes/NODES%d.DAT",nodeid);
		areafile[nodeid] = new CNode(buffer);
		// получаем количество точек
		areafile[nodeid]->GetInfo(&header[0],&header[1],&header[2],&header[3]);
		// создаём массивы ссылок
		node_vehicle_link[nodeid].count = header[1];
		node_vehicle_link[nodeid].links = new unsigned long[ header[1] ];
		node_ped_link[nodeid].count = header[2];
		node_ped_link[nodeid].links = new unsigned long[ header[2] ];
		// считаем общее количество точек
		vehicle_count += header[1];
		ped_count += header[2];
	}
	// создаём массивы данных
	node_vehicle = new NodeInfo[vehicle_count];
	node_ped = new NodeInfo[ped_count];
	// считываем точки в массивы
	for(unsigned char nodeid = 0;nodeid < 64;nodeid++)
	{
	    // ïîëó÷àåì êîëè÷åñòâî òî÷åê ïóòåé
		areafile[nodeid]->GetInfo(&header[0],&header[1],&header[2],&header[3]);
		// ñ÷èòûâàåì âñå òðàíñïîðòíûå ïóòè
		for(i = 0;i < header[1];i++)
		{
			areafile[nodeid]->SetPoint(i);
			areafile[nodeid]->GetPos(&tmp[0],&tmp[1],&tmp[2]);
			// äîáàâëÿåì åž â ìàññèâ òî÷åê
			node_vehicle[node_vehicle_count].ni_pos[0] = tmp[0];
			node_vehicle[node_vehicle_count].ni_pos[1] = tmp[1];
			node_vehicle[node_vehicle_count].ni_pos[2] = tmp[2];
			node_vehicle[node_vehicle_count].ni_areaid = nodeid;
			node_vehicle[node_vehicle_count].ni_nodeid = i;
			*(unsigned long*)(&node_vehicle[node_vehicle_count].ni_links) = (unsigned long)areafile[nodeid]->GetLinkId();
			node_vehicle[node_vehicle_count].ni_linkcount = areafile[nodeid]->GetLinkCount();
			// ëèíêóåì òåêóùóþ òî÷êó
			node_vehicle_link[nodeid].links[i] = node_vehicle_count + 1;
			// óâåëè÷èâàåì êîëè÷åñòâî çàãðóæåííûõ òî÷åê
			node_vehicle_count++;
		}
		// ñ÷èòûâàåì âñå ïåøåõîäíûå ïóòè
		for(i = 0;i < header[2];i++)
		{
			areafile[nodeid]->SetPoint(i + header[1]);
			areafile[nodeid]->GetPos(&tmp[0],&tmp[1],&tmp[2]);
			// äîáàâëÿåì åž â ìàññèâ òî÷åê
			node_ped[node_ped_count].ni_pos[0] = tmp[0];
			node_ped[node_ped_count].ni_pos[1] = tmp[1];
			node_ped[node_ped_count].ni_pos[2] = tmp[2];
			node_ped[node_ped_count].ni_areaid = nodeid;
			node_ped[node_ped_count].ni_nodeid = i + header[1];
			*(unsigned long*)(&node_ped[node_ped_count].ni_links) = (unsigned long)areafile[nodeid]->GetLinkId();
			node_ped[node_ped_count].ni_linkcount = areafile[nodeid]->GetLinkCount();
			// ëèíêóåì òåêóùóþ òî÷êó
			node_ped_link[nodeid].links[i] = node_ped_count + 1;
			// óâåëè÷èâàåì êîëè÷åñòâî çàãðóæåííûõ òî÷åê
			node_ped_count++;
		}
	}
	// ÷èòàåì ñâÿçè òðàíñïîðòíûõ òî÷åê
	for(unsigned long nodeid = 0;nodeid < node_vehicle_count;nodeid++)
	{
		unsigned long lid = *(unsigned long*)(&node_vehicle[nodeid].ni_links);
		unsigned char areaid = node_vehicle[nodeid].ni_areaid;
		unsigned char lc = 0;
		// читаем ссылки
		if(node_vehicle[nodeid].ni_linkcount > 0)
		{
			// ñîçäàžì ìàññèâû ññûëîê
			node_vehicle[nodeid].ni_links = new unsigned long[ node_vehicle[nodeid].ni_linkcount ];
			node_vehicle[nodeid].ni_links_dist = new float[ node_vehicle[nodeid].ni_linkcount ];
			for(unsigned char j = 0;j < node_vehicle[nodeid].ni_linkcount;j++)
			{
				areafile[areaid]->SetLink(lid + j);
				unsigned long pointid = node_vehicle_link[ areafile[areaid]->GetLinkAreaId() ].links[ areafile[areaid]->GetLinkNodeId() ];
				if(pointid != 0)
				{
					pointid--;
					node_vehicle[nodeid].ni_links[lc] = pointid;
					// âû÷èñëÿåì ðàññòîÿíèå îò òåêóùåé âåðøèíû äî ñâÿçàííîé
					tmp[0] = node_vehicle[pointid].ni_pos[0] - node_vehicle[nodeid].ni_pos[0];
					tmp[1] = node_vehicle[pointid].ni_pos[1] - node_vehicle[nodeid].ni_pos[1];
					tmp[2] = node_vehicle[pointid].ni_pos[2] - node_vehicle[nodeid].ni_pos[2];
					node_vehicle[nodeid].ni_links_dist[lc] = sqrt((tmp[0]*tmp[0]) + (tmp[1]*tmp[1]) + (tmp[2]*tmp[2]));
					lc++;
				}
			}
		}
		node_vehicle[nodeid].ni_linkcount = lc;
	}
	// ÷èòàåì ñâÿçè ïåøåõîäíûõ òî÷åê
	for(unsigned long nodeid = 0;nodeid < node_ped_count;nodeid++)
	{
		unsigned long lid = *(unsigned long*)(&node_ped[nodeid].ni_links);
		unsigned char areaid = node_ped[nodeid].ni_areaid;
		unsigned char lc = 0;
		// читаем ссылки
		if(node_ped[nodeid].ni_linkcount > 0)
		{
			node_ped[nodeid].ni_links = new unsigned long[ node_ped[nodeid].ni_linkcount ];
			node_ped[nodeid].ni_links_dist = new float[ node_ped[nodeid].ni_linkcount ];
			for(unsigned char j = 0;j < node_ped[nodeid].ni_linkcount;j++)
			{
				areafile[areaid]->SetLink(lid + j);
				unsigned short pareaid = areafile[areaid]->GetLinkAreaId();
				areafile[pareaid]->GetInfo(&header[0],&header[1],&header[2],&header[3]);
				unsigned long pnodeid = areafile[areaid]->GetLinkNodeId() - header[1];
				unsigned long pointid = node_ped_link[pareaid].links[pnodeid];
				if(pointid != 0)
				{
					pointid--;
					node_ped[nodeid].ni_links[lc] = pointid;
					// âû÷èñëÿåì ðàññòîÿíèå îò òåêóùåé âåðøèíû äî ñâÿçàííîé
					tmp[0] = node_ped[pointid].ni_pos[0] - node_ped[nodeid].ni_pos[0];
					tmp[1] = node_ped[pointid].ni_pos[1] - node_ped[nodeid].ni_pos[1];
					tmp[2] = node_ped[pointid].ni_pos[2] - node_ped[nodeid].ni_pos[2];
					node_ped[nodeid].ni_links_dist[lc] = sqrt((tmp[0]*tmp[0]) + (tmp[1]*tmp[1]) + (tmp[2]*tmp[2]));
					lc++;
				}
			}
		}
		node_ped[nodeid].ni_linkcount = lc;
	}
	// закрываем все ноды
	for(unsigned char nodeid = 0;nodeid < 64;nodeid++)
	{
		delete areafile[nodeid];
	}
	// выводим статистику
	//logprintf("Nodes manager load:");
	//logprintf("vehicle nodes: %d",node_vehicle_count);
	//logprintf("ped nodes: %d",node_ped_count);
}

CNodeManager::~CNodeManager()
{
	// nodes
	for(unsigned long i = 0;i < node_vehicle_count;i++) 
	{
		delete [] node_vehicle[i].ni_links;
		delete [] node_vehicle[i].ni_links_dist;
	}
	for(unsigned long i = 0;i < node_ped_count;i++) 
	{
		delete [] node_ped[i].ni_links;
		delete [] node_ped[i].ni_links_dist;
	}
	delete [] node_vehicle;
	delete [] node_ped;
	// links
	for(unsigned char i = 0;i < 64;i++)
	{
		delete [] node_vehicle_link[i].links;
		delete [] node_ped_link[i].links;
	}
}

bool CNodeManager::GetNodePos(unsigned char nodetype,unsigned long nodeid,float* x,float* y,float* z)
{
	NodeInfo* nodes;

	if(nodetype == VEHICLE_NODES) nodes = node_vehicle;
	else if(nodetype == PED_NODES) nodes = node_ped;
	else return false;

	*x = nodes[nodeid].ni_pos[0];
	*y = nodes[nodeid].ni_pos[1];
	*z = nodes[nodeid].ni_pos[2];
	return true;
}

unsigned char CNodeManager::GetNodeAreaid(unsigned char nodetype,unsigned long nodeid)
{
	NodeInfo* nodes;

	if(nodetype == VEHICLE_NODES) nodes = node_vehicle;
	else if(nodetype == PED_NODES) nodes = node_ped;
	else return 0xFF;

	return nodes[nodeid].ni_areaid;
}

unsigned long CNodeManager::GetNodeNodeid(unsigned char nodetype,unsigned long nodeid)
{
	NodeInfo* nodes;

	if(nodetype == VEHICLE_NODES) nodes = node_vehicle;
	else if(nodetype == PED_NODES) nodes = node_ped;
	else return 0xFFFF;

	return nodes[nodeid].ni_nodeid;
}

unsigned long CNodeManager::GetNodeLink(unsigned char nodetype,unsigned long nodeid,unsigned char linkid)
{
	NodeInfo* nodes;

	if(nodetype == VEHICLE_NODES) nodes = node_vehicle;
	else if(nodetype == PED_NODES) nodes = node_ped;
	else return INVALID_NODE_ID;

	return nodes[nodeid].ni_links[linkid];
}

float CNodeManager::GetNodeLinkDist(unsigned char nodetype,unsigned long nodeid,unsigned char linkid)
{
	NodeInfo* nodes;

	if(nodetype == VEHICLE_NODES) nodes = node_vehicle;
	else if(nodetype == PED_NODES) nodes = node_ped;
	else return 0.0;

	return nodes[nodeid].ni_links_dist[linkid];
}

unsigned char CNodeManager::GetNodeLinkCount(unsigned char nodetype,unsigned long nodeid)
{
	NodeInfo* nodes;

	if(nodetype == VEHICLE_NODES) nodes = node_vehicle;
	else if(nodetype == PED_NODES) nodes = node_ped;
	else return 0;

	return nodes[nodeid].ni_linkcount;
}

unsigned long CNodeManager::GetNodeCount(unsigned char nodetype)
{
	switch(nodetype)
	{
		case VEHICLE_NODES: return node_vehicle_count;
		case PED_NODES: return node_ped_count;
	}
	return 0;
}

unsigned long CNodeManager::GetNodeid(unsigned char nodetype,float x,float y,float z)
{
	unsigned char areaid;
	unsigned long result = 0xFFFFFFFF;
	float minlen = 65000.0f;
	float tmp[4];
	NodeInfo* nodes;
	LinkInfo* links;

	if(nodetype == VEHICLE_NODES)
	{
		nodes = node_vehicle;
		links = (LinkInfo*)(&node_vehicle_link);
	}
	else if(nodetype == PED_NODES)
	{
		nodes = node_ped;
		links = (LinkInfo*)(&node_ped_link);
	}
	else return INVALID_NODE_ID;

	// получаем номер квадрата
	areaid = GetAreaid(x,y,0.0);
	// ищем ближайшую точку к координатам
	for(unsigned long i = 0;i < links[areaid].count;i++)
	{
		if(!links[areaid].links[i]) continue; // не линкована
				
		unsigned long pid = links[areaid].links[i] - 1;
		// получаем расстояние
		tmp[0] = nodes[pid].ni_pos[0] - x;
		tmp[1] = nodes[pid].ni_pos[1] - y;
		tmp[2] = nodes[pid].ni_pos[2] - z;
		tmp[3] = sqrt((tmp[0]*tmp[0]) + (tmp[1]*tmp[1]) + (tmp[2]*tmp[2]));
		// сверяем с минимальным
		if(tmp[3] < minlen)
		{
			result = pid;
			minlen = tmp[3];
			// если расстояние меньше 2.0 считаем что точка найдена
			if(minlen < 2.0) break;
		}
	}
	// возвращаем результат
	return result;
}

unsigned char CNodeManager::GetAreaid(float x,float y,float z)
{
	if((x > 3000.0) || (x < -3000.0)) return 0;
	if((y > 3000.0) || (y < -3000.0)) return 0;
	return ((unsigned char)floor(((y + 3000.0f) / 750.0f)) * 8) + (unsigned char)floor(((x + 3000.0f) / 750.0f));
}

bool CNodeManager::GetAreasFromPos(float startx,float starty,float endx,float endy,unsigned char* areas)
{
	unsigned char startdata[2],enddata[2];

	startdata[0] = (unsigned char)floor(((startx + 3000.0f) / 750.0f));
	startdata[1] = (unsigned char)floor(((starty + 3000.0f) / 750.0f));
	enddata[0] = (unsigned char)floor(((endx + 3000.0f) / 750.0f));
	enddata[1] = (unsigned char)floor(((endy + 3000.0f) / 750.0f));
	// ñòàâèì ìåíüøåå çíà÷åíèå â 0 ÿ÷åéêó, áîëüøåå â 1
	for(unsigned char i = 0;i < 2;i++)
	{
		if(startdata[i] > enddata[i])
		{
		    unsigned char temp = startdata[i];
		    startdata[i] = enddata[i];
		    enddata[i] = temp;
		}
	}
	// çàíóëÿåì çíà÷åíèÿ
	for(unsigned char i = 0;i < 64;i++) areas[i] = 0;
	// îòìå÷àåì êâàäðàòû äëÿ çàãðóçêè ïóòåé
	for(unsigned char i = startdata[0];i <= enddata[0];i++)
	{
	    for(unsigned char j = startdata[1];j <= enddata[1];j++)
	    {
	        unsigned char nodeid = (j * 8) + i;
	        areas[nodeid] = 1;
	    }
	}
	return true;
}

unsigned long CNodeManager::GetAreaNodeid(unsigned char nodetype,unsigned char areaid,unsigned long nodeid)
{
	switch(nodetype)
	{
		case VEHICLE_NODES:
		{
			if(nodeid > node_vehicle_link[areaid].count) return INVALID_NODE_ID;
			if(!node_vehicle_link[areaid].links[nodeid]) return INVALID_NODE_ID;

			return (node_vehicle_link[areaid].links[nodeid] - 1);
		}
		case PED_NODES:
		{
			if(nodeid > node_ped_link[areaid].count) return INVALID_NODE_ID;
			if(!node_ped_link[areaid].links[nodeid]) return INVALID_NODE_ID;

			return (node_ped_link[areaid].links[nodeid] - 1);
		}
	}
	return INVALID_NODE_ID;
}

unsigned long CNodeManager::GetAreaNodeCount(unsigned char nodetype,unsigned char areaid)
{
	switch(nodetype)
	{
		case VEHICLE_NODES:
		{
			return node_vehicle_link[areaid].count;
		}
		case PED_NODES:
		{
			return node_ped_link[areaid].count;
		}
	}
	return 0;
}

bool CNodeManager::CalcPathByNodes(unsigned char* areas,unsigned char nodetype,unsigned long startnodeid,unsigned long endnodeid,unsigned long** path,unsigned long* pathlen,float* pathdist)
{
	struct _NodeInfo
	{
		// dijkstra
		float			ni_dist;		// расстояние от начала пути
		unsigned long	ni_parentid;	// номер точки из которой пришли сюда самым коротким путём от начала пути
		unsigned short	ni_step;		// шаг от начала пути
		bool			ni_check;		// флаг проверена ли точка при поиске пути
	};

	unsigned long cur;
	unsigned long last;
	float min_dist;
	unsigned long ncount = GetNodeCount(nodetype);
	unsigned char lcount;
	unsigned long pid;
	unsigned char areaid;
	_NodeInfo* nodes = new _NodeInfo[ncount];

	// чистим данные для поиска пути
	for(unsigned long i = 0;i < ncount;i++)
	{
		nodes[i].ni_dist = 65000.0f;
		nodes[i].ni_parentid = 0xFFFFFFFF;
		nodes[i].ni_step = 1;
		nodes[i].ni_check = false;
	}
	// устанавливаем данные начальной точки
	nodes[startnodeid].ni_dist = 0.0;
	cur = startnodeid;
	// работаем пока есть не проверенные точки
	while(!nodes[cur].ni_check)
	{
		// помечаем точку проверенной
		nodes[cur].ni_check = true;
		// указываем что эта точка была проверена последней
		last = cur;
		// если это целевая точка - прекращаем поиск
		if(cur == endnodeid) break;
		// получаем количество связей
		lcount = GetNodeLinkCount(nodetype,cur);
		// проходим по всем связанным точкам
		for(unsigned char i = 0;i < lcount;i++)
		{
			// получаем номер связанной точки
			pid = GetNodeLink(nodetype,cur,i);
			// берем только не проверенные точки
			if(nodes[pid].ni_check) continue;
			// получаем номер квадрата точки
			areaid = GetNodeAreaid(nodetype,pid);
			// проверяем можем ли мы использовать этот квадрат
			if(!areas[ areaid ]) continue;
			// получаем расстояние от начала пути до точки
			float d = nodes[cur].ni_dist + GetNodeLinkDist(nodetype,cur,i);
			// если это расстояние меньше предыдущих - меняем данные
			if(d < nodes[pid].ni_dist)
			{
				nodes[pid].ni_dist = d;
				nodes[pid].ni_parentid = cur;
				nodes[pid].ni_step = nodes[cur].ni_step + 1;
			}
		}
		// ищем новую точку для обработки
		min_dist = 65000.0f;
		for(unsigned long i = 0;i < ncount;i++)
		{
			// берем только не проверенные точки
			if(nodes[i].ni_check) continue;
			// получаем номер квадрата точки
			areaid = GetNodeAreaid(nodetype,pid);
			// проверяем можем ли мы использовать этот квадрат
			if(!areas[ areaid ]) continue;
			// проверяем расстояние
			if(nodes[i].ni_dist < min_dist) 
			{
				cur = i;
				min_dist = nodes[i].ni_dist;
			}
		}
		// чтобы не грузить процессор при большом расчете
		SLEEP(1);
	}
	// проверяем достигли мы конца пути
	if(last != endnodeid)
	{
		// конец пути не достигнут
		*pathlen = 0;
		*pathdist = 0.0f;
		*path = NULL;
	}
	else
	{
		// конец пути достигнут, разворачиваем путь
		cur = last;

		*pathlen = nodes[cur].ni_step;
		*pathdist = nodes[cur].ni_dist;
		*path = new unsigned long[ nodes[cur].ni_step ];

		for(long pathpos = nodes[cur].ni_step - 1;pathpos >= 0;pathpos--)
		{
			(*path)[pathpos] = cur;
			cur = nodes[cur].ni_parentid;
		}
	}
	// чистим память
	delete [] nodes;
	// готово
	return true;
}