/*
*	Created:			05.10.12
*	Author:				009
*	Last Modifed:		07.10.12
*	Description:		NPC paths manager (Controllable NPC)
*/


#if defined _cnpc_paths_included
	#endinput
#endif

#define _cnpc_paths_included
#pragma library cnpc_paths

// --------------------------------------------------
// includes
// --------------------------------------------------
#include <cnpc>

// --------------------------------------------------
// defines
// --------------------------------------------------


// --------------------------------------------------
// enums
// --------------------------------------------------
enum
{
	PATH_STATE_NOT_EXIST,
	PATH_STATE_CALCULATING,
	PATH_STATE_CALCULATED,
	PATH_STATE_NEED_DESTROY
};

// --------------------------------------------------
// statics
// --------------------------------------------------
static
	path_for[MAX_NODE_PATHS],		// ссылки на NPC для которых пострен путь
	path_type[MAX_NODE_PATHS char],	// тип созданного пути (пешеходный либо транспортный)
	path_state[MAX_NODE_PATHS char],// состояние пути
	npc_use[MAX_NPCS];				// ссылки на пути которые построены для NPC
	
// --------------------------------------------------
// forwards
// --------------------------------------------------
forward OnStartCalcPathForNPC(npcid);
forward OnCompleteCalcPathForNPC(npcid);

// --------------------------------------------------
// stocks
// --------------------------------------------------
/*
	Name: 
		paths_ResetData(npcid)
	Description:
		обнуляет данные модуля для NPC
	Params:
		npcid - id NPC
	Return:
		-
*/
stock paths_ResetData(npcid)
{
	if(npc_use[npcid] != INVALID_NODE_PATH)
	{
		if(path_state{ npc_use[npcid] } == PATH_STATE_CALCULATED)
		{
			path_Destroy(npc_use[npcid]);
			path_state{ npc_use[npcid] } = PATH_STATE_NOT_EXIST;
		}
		else path_state{ npc_use[npcid] } = PATH_STATE_NEED_DESTROY;
		
		path_for[ npc_use[npcid] ] = INVALID_PLAYER_ID;
		path_type{ npc_use[npcid] } = 0;
		npc_use[npcid] = INVALID_NODE_PATH;
	}
}

/*
	Name: 
		paths_CalcPathForNPC(npcid,nodetype,startnodeid,endnodeid)
	Description:
		расчёт пути для NPC (используя весь навигационный граф)
	Params:
		npcid - id NPC
		nodetype - тип используемого графа (пешеходный либо транспортный)
		startnodeid - id вершины графа начала пути
		endnodeid - id вершины графа конца пути
	Return:
		-
*/
stock paths_CalcPathForNPC(npcid,nodetype,startnodeid,endnodeid)
{
	// проверяем закреплен ли какой либо путь за данным NPC
	if(npc_use[npcid] != INVALID_NODE_PATH)
	{
		// чистим данные пути
		paths_ResetData(npcid);
	}
	// помечаем все области для поиска
	new areas[64];
	for(new i = 0;i < 64;i++) areas[i] = 1;
	// начинаем расчёт пути
	new pathid = dijkstra_CalcPathByNodes(areas,nodetype,startnodeid,endnodeid);
	// запоминаем данные
	npc_use[npcid] = pathid;
	path_for[pathid] = npcid;
	path_type{pathid} = nodetype;
	path_state{pathid} = PATH_STATE_CALCULATING;
	// вызываем калбек
	OnStartCalcPathForNPC(npcid);
}

/*
	Name: 
		paths_GetPathLen(npcid)
	Description:
		получает длину пути (количество точек) созданного для NPC
	Params:
		npcid - id NPC
	Return:
		int
*/
stock paths_GetPathLen(npcid)
{
	// проверяем закреплен ли какой либо путь за данным NPC
	if(npc_use[npcid] == INVALID_NODE_PATH) return 0;
	
	return path_GetPathLen(npc_use[npcid]);
}

/*
	Name: 
		paths_GetPathDist(npcid)
	Description:
		получает расстояние пути созданного для NPC
	Params:
		npcid - id NPC
	Return:
		float
*/
stock Float:paths_GetPathDist(npcid)
{
	// проверяем закреплен ли какой либо путь за данным NPC
	if(npc_use[npcid] == INVALID_NODE_PATH) return 0.0;
	
	return path_GetPathDist(npc_use[npcid]);
}

/*
	Name: 
		paths_GetPathPointNodeid(npcid,pointid)
	Description:
		получает nodeid точки пути
	Params:
		npcid - id NPC
		pointid - id точки на пути
	Return:
		int
*/
stock paths_GetPathPointNodeid(npcid,pointid)
{
	// проверяем закреплен ли какой либо путь за данным NPC
	if(npc_use[npcid] == INVALID_NODE_PATH) return 0;
	// получаем количество точек пути
	new len = path_GetPathLen(npc_use[npcid]);
	// проверяем принадлежность точки пути
	if((pointid < 0) || (pointid >= len)) return 0;
	// получаем номер точки пути
	return path_GetPathNodeid(npc_use[npcid],pointid);
}

/*
	Name: 
		paths_GetPathPointPos(npcid,pointid,&Float:x,&Float:y,&Float:z)
	Description:
		получает координаты точки пути
	Params:
		npcid - id NPC
		pointid - id точки на пути
		x,y,z - возвращаемые координаты точки
	Return:
		0 - ошибка (нет пути либо точка не принадлежит пути)
		1 - координаты получены
*/
stock paths_GetPathPointPos(npcid,pointid,&Float:x,&Float:y,&Float:z)
{
	// проверяем закреплен ли какой либо путь за данным NPC
	if(npc_use[npcid] == INVALID_NODE_PATH) return 0;
	// получаем количество точек пути
	new len = path_GetPathLen(npc_use[npcid]);
	// проверяем принадлежность точки пути
	if((pointid < 0) || (pointid >= len)) return 0;
	// получаем номер точки пути
	new nodeid = path_GetPathNodeid(npc_use[npcid],pointid);
	// получаем координаты точки пути
	nodes_GetNodePos(path_type{ npc_use[npcid] },nodeid,x,y,z);
	return 1;
}

// --------------------------------------------------
// obligatory functions
// --------------------------------------------------
paths_Init()
{
	// чистим ссылки
	for(new i = 0;i < MAX_NODE_PATHS;i++)
	{
		path_for[i] = INVALID_PLAYER_ID;
		path_type{i} = 0;
		path_state{i} = PATH_STATE_NOT_EXIST;
	}
	for(new i = 0;i < MAX_NPCS;i++) npc_use[i] = INVALID_NODE_PATH;
}

paths_Exit()
{
	// обнуляем данные всех npc
	for(new i = 0;i < MAX_NPCS;i++)
	{
		if(!IsValidNPC(i)) continue;
		
		paths_ResetData(i);
	}
}

paths_OnPathCalculated(pathid)
{
	// проверяем состояние пути
	if(path_state{pathid} == PATH_STATE_NEED_DESTROY)
	{
		path_Destroy(pathid);
		path_state{pathid} = PATH_STATE_NOT_EXIST;
	}
	else if(path_state{pathid} == PATH_STATE_CALCULATING)
	{
		// получаем NPC для которого создан путь
		new npcid = path_for[pathid];
		// проверяем принадлежность пути к данному модулю
		if(npcid == INVALID_PLAYER_ID) return;
		// изменияем данные
		path_state{pathid} = PATH_STATE_CALCULATED;
		// вызываем калбек о завершении создания пути
		OnCompleteCalcPathForNPC(npcid);
	}
}