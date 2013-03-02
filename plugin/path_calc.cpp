/*
 *	Copyright (C) 2012 Alex009 (������� ��������)
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

struct CallPathCalcData // �������� �������� ������
{
	unsigned char areas[64];
	unsigned char nodetype;
	unsigned long startnodeid;
	unsigned long endnodeid;
	unsigned short pathid;
};

std::stack<unsigned short> stack_path; // ��������� ���� ��� ������ �������
bool stack_used; // ���� ������������� � ������ ������ ����� (����� ������ ������ �����������)

// ����� ���������� ����
THREAD_START(PathCalculation){
	CallPathCalcData* data = (CallPathCalcData*)arg;

	unsigned long* path;
	unsigned long pathlen;
	float pathdist;
	unsigned short pathid = data->pathid;

	if(pNodeManager)
	{
		// ��������� ����
		pNodeManager->CalcPathByNodes(data->areas,data->nodetype,data->startnodeid,data->endnodeid,&path,&pathlen,&pathdist);

		// ��������� ����� ����
		pNodePaths[pathid] = new CNodePath(path,pathlen,pathdist);
		// ������ ������
		if(path) delete [] path;
		if(data) delete data;
		// ������� ���������� �����
		while(stack_used) SLEEP(5);
		// �������� ����
		stack_used = true;
		// ��������� � ���� ��� ����
		stack_path.push(pathid);
		// ����������� ����
		stack_used = false;
	}
	
THREAD_END}

unsigned short StartPathCalculation(unsigned char* areas,unsigned char nodetype,unsigned long startnodeid,unsigned long endnodeid)
{
	unsigned short pathid = 0;
	// ���� ��������� ����� ��� �������� ����
	for(;pathid < MAX_NODE_PATHS;pathid++)
	{
		if(!pNodePaths[pathid]) break;
	}
	// ��������� ������� �� ��������� ����� ��� ����
	if(pathid == MAX_NODE_PATHS) return 0xFFFF;
	// �������� ��� ����� ��� ������
	pNodePaths[pathid] = (CNodePath*)1;
	// ��������� �������� �������� ������
	CallPathCalcData* arg = new CallPathCalcData;
	for(int i = 0;i < 64;i++) arg->areas[i] = areas[i];
	arg->nodetype = nodetype;
	arg->startnodeid = startnodeid;
	arg->endnodeid = endnodeid;
	arg->pathid = pathid;
	// ������ �����
	THREAD_IDENTIFY(thread);
	THREAD_RESULT(res);
	CREATE_THREAD(thread,PathCalculation,res,arg);
	// ���������� id ����
	return pathid;
}

void CallPathCalculationCallback()
{
	// ������� ���������� �����
	while(stack_used) SLEEP(5);
	// �������� ����
	stack_used = true;
	// �������� ������� ��� ���� ����� �� �����
	while(!stack_path.empty())
	{
		// ����� id ����
		unsigned short pathid = stack_path.top();
		// ������� ��� �� �����
		stack_path.pop();
		// �������� ������
		pCallbacks->PathCalculated(pathid);
	}
	// ����������� ����
	stack_used = false;
}