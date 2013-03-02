/*
 *	Copyright (C) 2012 Alex009 (Алексей Михайлов)
 *	License read in license.txt
 *	Description: Path calculation on own thread
 *	Last edit at 30.09.12
 */

#include <math.h>
#include <stdio.h>
#include <stack>
#include "CNodeManager.h"
#include "CNodePath.h"
#include "CCallbacks.h"
#include "os.h"

extern	CNodePath*		pNodePaths[MAX_NODE_PATHS];
extern	CNodeManager*	pNodeManager;
extern	CCallbacks*		pCallbacks;

struct CallPathCalcData // параметр создания потока
{
	unsigned char areas[64];
	unsigned char nodetype;
	unsigned long startnodeid;
	unsigned long endnodeid;
	unsigned short pathid;
};

std::stack<unsigned short> stack_path; // найденные пути для вызова калбека
bool stack_used; // флаг использования в данный момент стека (чтобы небыло плохих пересечений)

// поток вычисления пути
THREAD_START(PathCalculation){
	CallPathCalcData* data = (CallPathCalcData*)arg;

	unsigned long* path;
	unsigned long pathlen;
	float pathdist;
	unsigned short pathid = data->pathid;

	if(pNodeManager)
	{
		// вычисляем путь
		pNodeManager->CalcPathByNodes(data->areas,data->nodetype,data->startnodeid,data->endnodeid,&path,&pathlen,&pathdist);

		// заполняем класс пути
		pNodePaths[pathid] = new CNodePath(path,pathlen,pathdist);
		// чистим память
		if(path) delete [] path;
		if(data) delete data;
		// ожидаем свободного стека
		while(stack_used) SLEEP(5);
		// занимаем стек
		stack_used = true;
		// добавляем в стек наш путь
		stack_path.push(pathid);
		// освобождаем стек
		stack_used = false;
	}
	
THREAD_END}

unsigned short StartPathCalculation(unsigned char* areas,unsigned char nodetype,unsigned long startnodeid,unsigned long endnodeid)
{
	unsigned short pathid = 0;
	// ищем свободное место для создания пути
	for(;pathid < MAX_NODE_PATHS;pathid++)
	{
		if(!pNodePaths[pathid]) break;
	}
	// проверяем найдено ли свободное место для пути
	if(pathid == MAX_NODE_PATHS) return 0xFFFF;
	// помечаем что место уже занято
	pNodePaths[pathid] = (CNodePath*)1;
	// заполняем аргумент создания потока
	CallPathCalcData* arg = new CallPathCalcData;
	for(int i = 0;i < 64;i++) arg->areas[i] = areas[i];
	arg->nodetype = nodetype;
	arg->startnodeid = startnodeid;
	arg->endnodeid = endnodeid;
	arg->pathid = pathid;
	// создаём поток
	THREAD_IDENTIFY(thread);
	THREAD_RESULT(res);
	CREATE_THREAD(thread,PathCalculation,res,arg);
	// возвращаем id пути
	return pathid;
}

void CallPathCalculationCallback()
{
	// ожидаем свободного стека
	while(stack_used) SLEEP(5);
	// занимаем стек
	stack_used = true;
	// вызываем калбеки для всех путей из стека
	while(!stack_path.empty())
	{
		// берем id пути
		unsigned short pathid = stack_path.top();
		// убираем его из стека
		stack_path.pop();
		// вызываем калбек
		pCallbacks->PathCalculated(pathid);
	}
	// освобождаем стек
	stack_used = false;
}