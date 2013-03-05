/*
*	Created:			25.08.12
*	Author:				009
*	Last Modifed:		11.10.12
*	Description:		NPC controller (Controllable NPC)
*/


#if defined _cnpc_controller_included
	#endinput
#endif

#define _cnpc_controller_included
#pragma library cnpc_controller

// --------------------------------------------------
// includes
// --------------------------------------------------
#include <cnpc>
#include <cnpc_paths>

// --------------------------------------------------
// defines
// --------------------------------------------------
// подтипы передвижения
#define NPC_MOVE_PED_WALK			1
#define NPC_MOVE_PED_RUN			2
#define NPC_MOVE_PED_SPRINT			3

#define MOVING_BY_ZMAP				0

// --------------------------------------------------
// enums
// --------------------------------------------------
enum
{
	NPC_STATE_NOT_CONTROLLED,
	NPC_STATE_DRIVE_BY_NODES,
	NPC_STATE_DRIVE_BY_COORDS,
	NPC_STATE_MOVE_BY_NODES,
	NPC_STATE_MOVE_BY_COORDS,
	NPC_STATE_FOLLOW_BY_NODES
};

// --------------------------------------------------
// statics
// --------------------------------------------------
static
	npc_state[MAX_NPCS char],		// состояние NPC
	npc_type[MAX_NPCS char],		// тип состояния
	npc_target[MAX_NPCS],			// цель движения
	npc_target_node[MAX_NPCS],
	// moving
	npc_step[MAX_NPCS],				// точка на пути, к которой будет двигаться NPC после завершения текущего движения
	Float:npc_speed[MAX_NPCS],		// скорость передвижения NPC по пути (для транспорта)
	npc_boost[MAX_NPCS char];		// процент добавляемый к скорости передвижения NPC по пути, расчитывается как "оставшееся расстояние до конечной точки / 100 * процент" (для транспорта)
	
// --------------------------------------------------
// forwards
// --------------------------------------------------

// --------------------------------------------------
// stocks
// --------------------------------------------------
/*
	Name: 
		controller_ResetData(npcid)
	Description:
		обнуление данных контроллера указанного NPC
	Params:
		npcid - id NPC
	Return:
		-
*/
stock controller_ResetData(npcid)
{
	new str[32];
	// данные
	npc_state{npcid} = NPC_STATE_NOT_CONTROLLED;
	npc_type{npcid} = 0;
	npc_step[npcid] = 0;
	npc_speed[npcid] = 0.0;
	npc_boost{npcid} = 0;
	npc_target[npcid] = -1;
	npc_target_node[npcid] = -1;
	// пути навигационного графа
	paths_ResetData(npcid);
	// координатные пути
	new count = GetPVarInt(npcid,"controller_path_coords_count");
	for(new i = 0;i < count;i++)
	{
		format(str,sizeof(str),"controller_path_coord%d_X",i);
		DeletePVar(npcid,str);
		format(str,sizeof(str),"controller_path_coord%d_Y",i);
		DeletePVar(npcid,str);
		format(str,sizeof(str),"controller_path_coord%d_Z",i);
		DeletePVar(npcid,str);
		format(str,sizeof(str),"controller_path_coord%d_dist",i);
		DeletePVar(npcid,str);
	}
	DeletePVar(npcid,"controller_path_coords_count");
	DeletePVar(npcid,"controller_path_coords_dist");
}

/*
	Name: 
		controller_AddPathCoords(npcid,Float:x,Float:y,Float:z)
	Description:
		Добавляет новые координаты для движения
	Params:
		npcid - id NPC
		x,y,z - координаты
	Return:
		-
*/
stock controller_AddPathCoords(npcid,Float:x,Float:y,Float:z)
{
	new Float:lx,Float:ly,Float:lz;
	new count = GetPVarInt(npcid,"controller_path_coords_count");
	new str[32];
	new Float:dist = 0.0;
	// заполняем данные новой точки
	format(str,sizeof(str),"controller_path_coord%d_X",count);
	SetPVarFloat(npcid,str,x);
	format(str,sizeof(str),"controller_path_coord%d_Y",count);
	SetPVarFloat(npcid,str,y);
	format(str,sizeof(str),"controller_path_coord%d_Z",count);
	SetPVarFloat(npcid,str,z);
	// получаем дистанцию от прошлой позиции
	if(count == 0)
	{
		format(str,sizeof(str),"controller_path_coord%d_dist",count);
		SetPVarFloat(npcid,str,dist);
	}
	else
	{
		// получаем координаты предыдущей точки
		format(str,sizeof(str),"controller_path_coord%d_X",count - 1);
		lx = GetPVarFloat(npcid,str);
		format(str,sizeof(str),"controller_path_coord%d_Y",count - 1);
		ly = GetPVarFloat(npcid,str);
		format(str,sizeof(str),"controller_path_coord%d_Z",count - 1);
		lz = GetPVarFloat(npcid,str);
		// считаем расстояние
		dist = floatsqroot((x-lx)*(x-lx) + (y-ly)*(y-ly) + (z-lz)*(z-lz));
		// запоминаем
		format(str,sizeof(str),"controller_path_coord%d_dist",count);
		SetPVarFloat(npcid,str,dist);
	}
	// меняем количество точек
	SetPVarInt(npcid,"controller_path_coords_count",count + 1);
	// вычисляем новую дистанцию
	if(count == 0) SetPVarFloat(npcid,"controller_path_coords_dist",0.0);
	else
	{
		new Float:newdist = GetPVarFloat(npcid,"controller_path_coords_dist") + dist;
		SetPVarFloat(npcid,"controller_path_coords_dist",newdist);
	}
}

/*
	Name: 
		controller_GetPathCoordsLen(npcid)
	Description:
		Получает количество координат для движения
	Params:
		npcid - id NPC
	Return:
		int
*/
stock controller_GetPathCoordsLen(npcid)
{
	return GetPVarInt(npcid,"controller_path_coords_count");
}

/*
	Name: 
		controller_GetPathCoordsData(npcid,pointid,&Float:x,&Float:y,&Float:z)
	Description:
		Получает координаты определенной точки из координат для движения
	Params:
		npcid - id NPC
		pointid - id точки
		x,y,z - координаты (возвращаемое)
	Return:
		1 - координаты точки есть и возвращены
		0 - такой точки нет
*/
stock controller_GetPathCoordsData(npcid,pointid,&Float:x,&Float:y,&Float:z)
{
	new count = GetPVarInt(npcid,"controller_path_coords_count");
	new str[32];
	
	if(pointid < 0) return 0;
	if(pointid >= count) return 0;
	
	// получаем данные точки
	format(str,sizeof(str),"controller_path_coord%d_X",pointid);
	x = GetPVarFloat(npcid,str);
	format(str,sizeof(str),"controller_path_coord%d_Y",pointid);
	y = GetPVarFloat(npcid,str);
	format(str,sizeof(str),"controller_path_coord%d_Z",pointid);
	z = GetPVarFloat(npcid,str);
	
	return 1;
}

/*
	Name: 
		controller_GetPathCoordsDist(npcid)
	Description:
		Получает дистанцию всего пути
	Params:
		npcid - id NPC
	Return:
		Float
*/
stock Float:controller_GetPathCoordsDist(npcid)
{
	return GetPVarFloat(npcid,"controller_path_coords_dist");
}

/*
	Name: 
		controller_DriveToByNodes(npcid,Float:x,Float:y,Float:z,Float:speed,boost)
	Description:
		получение кратчайшего пути от текущей позиции NPC до указанных координат по навигационному графу, и движение в транспорте по пути.
	Params:
		npcid - id NPC
		x,y,z - координаты цели
		speed - базовая скорость движения
		boost - процент ускорения при удалённости от цели
	Return:
		1 - путь построен и NPC движется по нему
		0 - ошибка построения пути
*/
stock controller_DriveToByNodes(npcid,Float:x,Float:y,Float:z,Float:speed,boost)
{
	new Float:tmp[7],
		startnodeid,
		endnodeid;
	
	switch(npc_state{npcid})
	{
		case NPC_STATE_NOT_CONTROLLED:
		{
			// NPC не движется по пути, вычисляем путь от текущих координат до цели
			GetNPCPos(npcid,tmp[0],tmp[1],tmp[2]);
			// получаем точку начала
			startnodeid = nodes_GetNodeid(VEHICLE_NODES,tmp[0],tmp[1],tmp[2]);
			// получаем точку назначения
			endnodeid = nodes_GetNodeid(VEHICLE_NODES,x,y,z);
		}
		case NPC_STATE_DRIVE_BY_NODES:
		{
			// NPC движется по путям, берем точку к которой на данный момент едет NPC как начало пути
			startnodeid = paths_GetPathPointNodeid(npcid,npc_step[npcid] - 1);
			// получаем точку назначения
			endnodeid = nodes_GetNodeid(VEHICLE_NODES,x,y,z);
			// проверим точку назначения, возможно путь не нужно создавать заного
			if(endnodeid == paths_GetPathPointNodeid(npcid,paths_GetPathLen(npcid) - 1)) return 1; // NPC и так движется к данной точке
			// чистим данные пути
			paths_ResetData(npcid);
		}
		default: return 0; // не подходящее состояние
	}
	// проверяем есть ли необходимость в построении пути
	if(startnodeid == endnodeid) 
	{
		// NPC уже на точке назначения
		controller_ResetData(npcid);
		return 1;
	}
	// вычисляем путь
	paths_CalcPathForNPC(npcid,VEHICLE_NODES,startnodeid,endnodeid);
	// устанавилваем данные для движения
	npc_state{npcid} = NPC_STATE_DRIVE_BY_NODES;
	npc_speed[npcid] = speed;
	npc_boost{npcid} = boost;
	npc_step[npcid] = 0;
	// готово
	return 1;
}

/*
	Name: 
		controller_DriveToByExistPath(npcid,Float:speed,boost)
	Description:
		NPC будет двигаться по закрепленному за ним пути
	Params:
		npcid - id NPC
		speed - базовая скорость движения
		boost - процент ускорения при удалённости от цели
	Return:
		1 - путь существует и NPC движется по нему
		0 - путь не существует
*/
stock controller_DriveToByExistPath(npcid,Float:speed,boost)
{
	new Float:tmp[7];
	
	if(!paths_GetPathLen(npcid)) return 0;
	// устанавилваем данные для движения
	npc_state{npcid} = NPC_STATE_DRIVE_BY_NODES;
	npc_speed[npcid] = speed;
	npc_boost{npcid} = boost;
	npc_step[npcid] = 0;
	// получаем текущую точку
	GetNPCPos(npcid,tmp[0],tmp[1],tmp[2]);
	// ищем точку отличную от текущей
	do
	{
		// получаем координаты движения
		paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
		// получаем расстояние
		tmp[3] -= tmp[0];
		tmp[4] -= tmp[1];
		tmp[5] -= tmp[2];
		tmp[6] = floatsqroot((tmp[3]*tmp[3]) + (tmp[4]*tmp[4]) + (tmp[5]*tmp[5]));
		// меняем следующую точку
		npc_step[npcid]++;
	}
	while(tmp[6] < 1.0);
	// считаем скорость движения к точке
	new Float:drivespeed = npc_speed[npcid] + ((paths_GetPathDist(npcid) / 100.0) * npc_boost{npcid});
	// начинаем движение
	paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
	NPC_DriveTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,drivespeed,MOVING_BY_ZMAP);
	// указываем следующий шаг
	npc_step[npcid]++;
	return 1;
}

/*
	Name: 
		controller_DriveToByCoords(npcid,Float:speed,boost)
	Description:
		движение в транспорте по координатам записанным в памяти
	Params:
		npcid - id NPC
		speed - базовая скорость движения
		boost - процент ускорения при удалённости от цели
	Return:
		1 - NPC движется
		0 - ошибка
*/
stock controller_DriveToByCoords(npcid,Float:speed,boost)
{
	new Float:tmp[3];
	
	// получаем количество точек в памяти
	new count = controller_GetPathCoordsLen(npcid);
	// проверяем данные
	if(count == 0) return 0;
	// устанавливаем данные для движения
	npc_state{npcid} = NPC_STATE_DRIVE_BY_COORDS;
	npc_speed[npcid] = speed;
	npc_boost{npcid} = boost;
	npc_step[npcid] = 0;
	SetPVarFloat(npcid,"controller_complete_dist",0.0);
	// получаем координаты первого движения
	controller_GetPathCoordsData(npcid,npc_step[npcid],tmp[0],tmp[1],tmp[2]);
	// считаем скорость движения к точке
	new Float:drivespeed = npc_speed[npcid] + ((controller_GetPathCoordsDist(npcid) / 100.0) * npc_boost{npcid});
	// начинаем движение
	NPC_DriveTo(npcid,tmp[0],tmp[1],tmp[2] + 1.0,drivespeed,MOVING_BY_ZMAP);
	// указываем следующий шаг
	npc_step[npcid]++;
	// готово
	return 1;
}

/*
	Name: 
		controller_MoveToByNodes(npcid,Float:x,Float:y,Float:z,move_type)
	Description:
		получение кратчайшего пути от текущей позиции NPC до указанных координат по навигационному графу, и движение пешком по пути.
	Params:
		npcid - id NPC
		x,y,z - координаты цели
		move_type - тип движения (шаг, бег, спринт)
	Return:
		1 - путь построен и NPC движется по нему
		0 - ошибка построения пути
*/
stock controller_MoveToByNodes(npcid,Float:x,Float:y,Float:z,move_type)
{
	new Float:tmp[7],
		startnodeid,
		endnodeid;
	
	switch(npc_state{npcid})
	{
		case NPC_STATE_NOT_CONTROLLED:
		{
			// NPC не движется по пути, вычисляем путь от текущих координат до цели
			GetNPCPos(npcid,tmp[0],tmp[1],tmp[2]);
			// получаем точку начала
			startnodeid = nodes_GetNodeid(PED_NODES,tmp[0],tmp[1],tmp[2]);
			// получаем точку назначения
			endnodeid = nodes_GetNodeid(PED_NODES,x,y,z);
		}
		case NPC_STATE_MOVE_BY_NODES:
		{
			// NPC движется по путям, берем точку к которой на данный момент едет NPC как начало пути
			startnodeid = paths_GetPathPointNodeid(npcid,npc_step[npcid] - 1);
			// получаем точку назначения
			endnodeid = nodes_GetNodeid(PED_NODES,x,y,z);
			// проверим точку назначения, возможно путь не нужно создавать заного
			if(endnodeid == paths_GetPathPointNodeid(npcid,paths_GetPathLen(npcid) - 1)) return 1; // NPC и так движется к данной точке
			// чистим данные пути
			paths_ResetData(npcid);
		}
		default: return 0; // не подходящее состояние
	}
	// проверяем есть ли необходимость в построении пути
	if(startnodeid == endnodeid) 
	{
		// NPC уже на точке назначения
		controller_ResetData(npcid);
		return 1;
	}
	// вычисляем путь
	paths_CalcPathForNPC(npcid,PED_NODES,startnodeid,endnodeid);
	// устанавилваем данные для движения
	npc_state{npcid} = NPC_STATE_MOVE_BY_NODES;
	npc_boost{npcid} = move_type;
	npc_step[npcid] = 0;
	// готово
	return 1;
}

/*
	Name: 
		controller_MoveToByExistPath(npcid,move_type)
	Description:
		NPC будет двигаться по закрепленному за ним пути
	Params:
		npcid - id NPC
		move_type - тип движения (шаг, бег, спринт)
	Return:
		1 - путь существует и NPC движется по нему
		0 - путь не существует
*/
stock controller_MoveToByExistPath(npcid,move_type)
{
	new Float:tmp[7];
	
	if(!paths_GetPathLen(npcid)) return 0;
	// устанавилваем данные для движения
	npc_state{npcid} = NPC_STATE_MOVE_BY_NODES;
	npc_boost{npcid} = move_type;
	npc_step[npcid] = 0;
	// получаем текущую точку
	GetNPCPos(npcid,tmp[0],tmp[1],tmp[2]);
	// ищем точку отличную от текущей
	do
	{
		// получаем координаты движения
		paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
		// получаем расстояние
		tmp[3] -= tmp[0];
		tmp[4] -= tmp[1];
		tmp[5] -= tmp[2];
		tmp[6] = floatsqroot((tmp[3]*tmp[3]) + (tmp[4]*tmp[4]) + (tmp[5]*tmp[5]));
		// меняем следующую точку
		npc_step[npcid]++;
	}
	while(tmp[6] < 1.0);
	// начинаем движение
	paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
	switch(npc_boost{npcid})
	{
		case NPC_MOVE_PED_WALK: NPC_WalkTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
		case NPC_MOVE_PED_RUN: NPC_RunTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
		case NPC_MOVE_PED_SPRINT: NPC_SprintTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
	}
	// указываем следующий шаг
	npc_step[npcid]++;
	return 1;
}

/*
	Name: 
		controller_MoveToByCoords(npcid,move_type)
	Description:
		движение пешком по координатам записанным в памяти
	Params:
		npcid - id NPC
		move_type - тип движения (шаг, бег, спринт)
	Return:
		1 - NPC движется
		0 - ошибка
*/
stock controller_MoveToByCoords(npcid,move_type)
{
	new Float:tmp[3];
	
	// получаем количество точек в памяти
	new count = controller_GetPathCoordsLen(npcid);
	// проверяем данные
	if(count == 0) return 0;
	// устанавливаем данные для движения
	npc_state{npcid} = NPC_STATE_MOVE_BY_COORDS;
	npc_boost{npcid} = move_type;
	npc_step[npcid] = 0;
	SetPVarFloat(npcid,"controller_complete_dist",0.0);
	// получаем координаты первого движения
	controller_GetPathCoordsData(npcid,npc_step[npcid],tmp[0],tmp[1],tmp[2]);
	// начинаем движение
	switch(npc_boost{npcid})
	{
		case NPC_MOVE_PED_WALK: NPC_WalkTo(npcid,tmp[0],tmp[1],tmp[2],MOVING_BY_ZMAP);
		case NPC_MOVE_PED_RUN: NPC_RunTo(npcid,tmp[0],tmp[1],tmp[2],MOVING_BY_ZMAP);
		case NPC_MOVE_PED_SPRINT: NPC_SprintTo(npcid,tmp[0],tmp[1],tmp[2],MOVING_BY_ZMAP);
	}
	// указываем следующий шаг
	npc_step[npcid]++;
	// готово
	return 1;
}

/*
	Name: 
		controller_FollowPlayerByNodes(npcid,playerid,move_type)
	Description:
		следовать пешком за игроком по навигационному графу
	Params:
		npcid - id NPC
		playerid - id игрока за кем следовать
		move_type - тип движения (шаг, бег, спринт)
	Return:
		1 - NPC движется
		0 - ошибка
*/
stock controller_FollowPlayerByNodes(npcid,playerid,move_type)
{
	new Float:tmp[7],
		startnodeid,
		endnodeid;
	
	switch(npc_state{npcid})
	{
		case NPC_STATE_NOT_CONTROLLED:
		{
			// NPC не движется по пути, вычисляем путь от текущих координат до цели
			// получаем точку начала
			GetNPCPos(npcid,tmp[0],tmp[1],tmp[2]);
			startnodeid = nodes_GetNodeid(PED_NODES,tmp[0],tmp[1],tmp[2]);
			// получаем точку назначения
			GetPlayerPos(playerid,tmp[0],tmp[1],tmp[2]);
			endnodeid = nodes_GetNodeid(PED_NODES,tmp[0],tmp[1],tmp[2]);
		}
		default: return 0; // не подходящее состояние
	}
	
	if(startnodeid != endnodeid)
	{
		nodes_GetNodePos(PED_NODES,endnodeid,tmp[0],tmp[1],tmp[2]);
		// ищем связи
		new lcount = nodes_GetNodeLinkCount(PED_NODES,startnodeid);
		new clid;
		new Float:cld = 65000.0;
		for(new lid = 0;lid < lcount;lid++)
		{
			new nodeid = nodes_GetNodeLink(PED_NODES,startnodeid,lid);
			nodes_GetNodePos(PED_NODES,nodeid,tmp[3],tmp[4],tmp[5]);
			
			tmp[3] -= tmp[0];
			tmp[4] -= tmp[1];
			tmp[5] -= tmp[2];
			
			new Float:dist = floatsqroot(tmp[3]*tmp[3] + tmp[4]*tmp[4] + tmp[5]*tmp[5]);
			
			if(dist < cld)
			{
				clid = nodeid;
				cld = dist;
			}
		}
		
		// отправляемся к точке пути которая ближе всех к назначению
		nodes_GetNodePos(PED_NODES,clid,tmp[3],tmp[4],tmp[5]);
		switch(move_type)
		{
			case NPC_MOVE_PED_WALK: NPC_WalkTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
			case NPC_MOVE_PED_RUN: NPC_RunTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
			case NPC_MOVE_PED_SPRINT: NPC_SprintTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
		}
		npc_target_node[npcid] = clid;
	}
	else npc_target_node[npcid] = -1;
	
	// устанавилваем данные для движения
	npc_state{npcid} = NPC_STATE_FOLLOW_BY_NODES;
	npc_boost{npcid} = move_type;
	npc_target[npcid] = playerid;
	npc_step[npcid] = 0;
	// готово
	return 1;
}

/*
	Name: 
		controller_IsMoving(npcid)
	Description:
		проверяет движется ли NPC
	Params:
		npcid - id NPC
	Return:
		1 - NPC движется
		0 - NPC не движется
*/
stock controller_IsMoving(npcid)
{	
	return (npc_state{npcid} != NPC_STATE_NOT_CONTROLLED);
}

// --------------------------------------------------
// obligatory functions
// --------------------------------------------------
controller_OnCompCalcPathForNPC(npcid)
{
	new Float:tmp[7];
	
	// проверяем действительность создания пути
	if(paths_GetPathLen(npcid) == 0)
	{
		// путь не создан
		controller_ResetData(npcid);
		return;
	}	
	// получаем координаты назначения
	switch(npc_state{npcid})
	{
		case NPC_STATE_DRIVE_BY_NODES:
		{
			// получаем текущую точку
			GetNPCPos(npcid,tmp[0],tmp[1],tmp[2]);
			// ищем точку отличную от текущей
			do
			{
				// получаем координаты движения
				paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
				// получаем расстояние
				tmp[3] -= tmp[0];
				tmp[4] -= tmp[1];
				tmp[5] -= tmp[2];
				tmp[6] = floatsqroot((tmp[3]*tmp[3]) + (tmp[4]*tmp[4]) + (tmp[5]*tmp[5]));
				// меняем следующую точку
				npc_step[npcid]++;
			}
			while(tmp[6] < 1.0);
			// считаем скорость движения к точке
			new Float:drivespeed = npc_speed[npcid] + ((paths_GetPathDist(npcid) / 100.0) * npc_boost{npcid});
			// начинаем движение
			paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
			NPC_DriveTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,drivespeed,MOVING_BY_ZMAP);
			// указываем следующий шаг
			npc_step[npcid]++;
		}
		case NPC_STATE_MOVE_BY_NODES:
		{
			// получаем текущую точку
			GetNPCPos(npcid,tmp[0],tmp[1],tmp[2]);
			// ищем точку отличную от текущей
			do
			{
				// получаем координаты движения
				paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
				// получаем расстояние
				tmp[3] -= tmp[0];
				tmp[4] -= tmp[1];
				tmp[5] -= tmp[2];
				tmp[6] = floatsqroot((tmp[3]*tmp[3]) + (tmp[4]*tmp[4]) + (tmp[5]*tmp[5]));
				// меняем следующую точку
				npc_step[npcid]++;
			}
			while(tmp[6] < 1.0);
			// начинаем движение
			paths_GetPathPointPos(npcid,npc_step[npcid],tmp[3],tmp[4],tmp[5]);
			switch(npc_boost{npcid})
			{
				case NPC_MOVE_PED_WALK: NPC_WalkTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
				case NPC_MOVE_PED_RUN: NPC_RunTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
				case NPC_MOVE_PED_SPRINT: NPC_SprintTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
			}
			// указываем следующий шаг
			npc_step[npcid]++;
		}
	}
}

controller_OnNPCMovingComplete(npcid)
{
	new startnodeid;
	new endnodeid;
	new Float:tmp[6];
	// проверяем контролируется ли NPC
	switch(npc_state{npcid})
	{
		case NPC_STATE_DRIVE_BY_NODES:
		{
			new pathlen = paths_GetPathLen(npcid);
			// проверяем достигли ли конца пути
			if(npc_step[npcid] == pathlen)
			{
				// конец пути достигнут
				// чистим данные
				controller_ResetData(npcid);
			}
			else
			{
				// конец пути не достигнут
				new Float:dist = (paths_GetPathDist(npcid) / pathlen) * (pathlen - npc_step[npcid]);
				// считаем скорость движения к точке
				new Float:drivespeed = npc_speed[npcid] + ((dist / 100.0) * npc_boost{npcid});
				// получаем координаты назначения
				paths_GetPathPointPos(npcid,npc_step[npcid],tmp[0],tmp[1],tmp[2]);
				// начинаем движение
				NPC_DriveTo(npcid,tmp[0],tmp[1],tmp[2] + 1.0,drivespeed,MOVING_BY_ZMAP);
				// указываем следующий шаг
				npc_step[npcid]++;
			}
		}
		case NPC_STATE_DRIVE_BY_COORDS:
		{
			new pathlen = controller_GetPathCoordsLen(npcid);
			// проверяем достигли ли конца пути
			if(npc_step[npcid] == pathlen)
			{
				// конец пути достигнут
				// чистим данные
				controller_ResetData(npcid);
			}
			else
			{
				// конец пути не достигнут
				new Float:dist = (controller_GetPathCoordsDist(npcid) / pathlen) * (pathlen - npc_step[npcid]);
				// считаем скорость движения к точке
				new Float:drivespeed = npc_speed[npcid] + ((dist / 100.0) * npc_boost{npcid});
				// получаем координаты назначения
				controller_GetPathCoordsData(npcid,npc_step[npcid],tmp[0],tmp[1],tmp[2]);
				// начинаем движение
				NPC_DriveTo(npcid,tmp[0],tmp[1],tmp[2] + 1.0,drivespeed,MOVING_BY_ZMAP);
				// указываем следующий шаг
				npc_step[npcid]++;
			}
		}
		case NPC_STATE_MOVE_BY_NODES:
		{
			new pathlen = paths_GetPathLen(npcid);
			// проверяем достигли ли конца пути
			if(npc_step[npcid] == pathlen)
			{
				// конец пути достигнут
				// чистим данные
				controller_ResetData(npcid);
			}
			else
			{
				// получаем координаты назначения
				paths_GetPathPointPos(npcid,npc_step[npcid],tmp[0],tmp[1],tmp[2]);
				// начинаем движение
				switch(npc_boost{npcid})
				{
					case NPC_MOVE_PED_WALK: NPC_WalkTo(npcid,tmp[0],tmp[1],tmp[2] + 1.0,MOVING_BY_ZMAP);
					case NPC_MOVE_PED_RUN: NPC_RunTo(npcid,tmp[0],tmp[1],tmp[2] + 1.0,MOVING_BY_ZMAP);
					case NPC_MOVE_PED_SPRINT: NPC_SprintTo(npcid,tmp[0],tmp[1],tmp[2] + 1.0,MOVING_BY_ZMAP);
				}
				// указываем следующий шаг
				npc_step[npcid]++;
			}
		}
		case NPC_STATE_MOVE_BY_COORDS:
		{
			new pathlen = controller_GetPathCoordsLen(npcid);
			// проверяем достигли ли конца пути
			if(npc_step[npcid] == pathlen)
			{
				// конец пути достигнут
				// чистим данные
				controller_ResetData(npcid);
			}
			else
			{
				// получаем координаты назначения
				controller_GetPathCoordsData(npcid,npc_step[npcid],tmp[0],tmp[1],tmp[2]);
				// начинаем движение
				switch(npc_boost{npcid})
				{
					case NPC_MOVE_PED_WALK: NPC_WalkTo(npcid,tmp[0],tmp[1],tmp[2],MOVING_BY_ZMAP);
					case NPC_MOVE_PED_RUN: NPC_RunTo(npcid,tmp[0],tmp[1],tmp[2],MOVING_BY_ZMAP);
					case NPC_MOVE_PED_SPRINT: NPC_SprintTo(npcid,tmp[0],tmp[1],tmp[2],MOVING_BY_ZMAP);
				}
				// указываем следующий шаг
				npc_step[npcid]++;
			}
		}
		case NPC_STATE_FOLLOW_BY_NODES:
		{
			startnodeid = npc_target_node[npcid];
			// получаем точку назначения
			GetPlayerPos(npc_target[npcid],tmp[0],tmp[1],tmp[2]);
			endnodeid = nodes_GetNodeid(PED_NODES,tmp[0],tmp[1],tmp[2]);
			nodes_GetNodePos(PED_NODES,endnodeid,tmp[0],tmp[1],tmp[2]);
			
			if(startnodeid != endnodeid)
			{
				// ищем связи
				new lcount = nodes_GetNodeLinkCount(PED_NODES,startnodeid);
				new clid;
				new Float:cld = 65000.0;
				for(new lid = 0;lid < lcount;lid++)
				{
					new nodeid = nodes_GetNodeLink(PED_NODES,startnodeid,lid);
					nodes_GetNodePos(PED_NODES,nodeid,tmp[3],tmp[4],tmp[5]);
					
					tmp[3] -= tmp[0];
					tmp[4] -= tmp[1];
					tmp[5] -= tmp[2];
					
					new Float:dist = floatsqroot(tmp[3]*tmp[3] + tmp[4]*tmp[4] + tmp[5]*tmp[5]);
					
					if(dist < cld)
					{
						clid = nodeid;
						cld = dist;
					}
				}
			
				// отправляемся к точке пути которая ближе всех к назначению
				nodes_GetNodePos(PED_NODES,clid,tmp[3],tmp[4],tmp[5]);
				switch(npc_boost{npcid})
				{
					case NPC_MOVE_PED_WALK: NPC_WalkTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
					case NPC_MOVE_PED_RUN: NPC_RunTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
					case NPC_MOVE_PED_SPRINT: NPC_SprintTo(npcid,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
				}
				
				npc_target_node[npcid] = clid;
			}
			else npc_target_node[npcid] = -1;
		}
	}
}

controller_OnNPCSpawn(npcid)
{
	controller_ResetData(npcid);
}

controller_OnNPCDeath(npcid,killerid,reason)
{
	#pragma unused killerid
	#pragma unused reason
	controller_ResetData(npcid);
}

controller_OnPlayerUpdate(playerid)
{
	new bool:dexist = false; // флаг взяли ли нужные данные
	new Float:tmp[6];
	new startnodeid;
	new endnodeid;
	
	for(new i = 0;i < MAX_NPCS;i++)
	{
		if(npc_state{i} != NPC_STATE_FOLLOW_BY_NODES) continue;
		if(npc_target[i] != playerid) continue;
		
		// проверяем получали ли уже данные
		if(!dexist)
		{
			// данные не получали, получаем нод у которого в данный момент игрок
			GetPlayerPos(playerid,tmp[0],tmp[1],tmp[2]);
			endnodeid = nodes_GetNodeid(PED_NODES,tmp[0],tmp[1],tmp[2]);
			dexist = true;
		}
		
		// двигаемся ли
		if(npc_target_node[i] == -1)
		{
			// получаем точку начала
			GetNPCPos(i,tmp[0],tmp[1],tmp[2]);
			startnodeid = nodes_GetNodeid(PED_NODES,tmp[0],tmp[1],tmp[2]);
			
			if(startnodeid != endnodeid)
			{
				nodes_GetNodePos(PED_NODES,endnodeid,tmp[0],tmp[1],tmp[2]);
				// ищем связи
				new lcount = nodes_GetNodeLinkCount(PED_NODES,startnodeid);
				new clid;
				new Float:cld = 65000.0;
				for(new lid = 0;lid < lcount;lid++)
				{
					new nodeid = nodes_GetNodeLink(PED_NODES,startnodeid,lid);
					nodes_GetNodePos(PED_NODES,nodeid,tmp[3],tmp[4],tmp[5]);
					
					tmp[3] -= tmp[0];
					tmp[4] -= tmp[1];
					tmp[5] -= tmp[2];
					
					new Float:dist = floatsqroot(tmp[3]*tmp[3] + tmp[4]*tmp[4] + tmp[5]*tmp[5]);
					
					if(dist < cld)
					{
						clid = nodeid;
						cld = dist;
					}
				}
				
				// отправляемся к точке пути которая ближе всех к назначению
				nodes_GetNodePos(PED_NODES,clid,tmp[3],tmp[4],tmp[5]);
				switch(npc_boost{i})
				{
					case NPC_MOVE_PED_WALK: NPC_WalkTo(i,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
					case NPC_MOVE_PED_RUN: NPC_RunTo(i,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
					case NPC_MOVE_PED_SPRINT: NPC_SprintTo(i,tmp[3],tmp[4],tmp[5] + 1.0,MOVING_BY_ZMAP);
				}
				npc_target_node[i] = clid;
			}
		}
	}
}