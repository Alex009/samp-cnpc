/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 */

// plugin
#include "SAMP/CSAMP.h"
#include "CNPC.h"
#include "defines.h"
#include "path_calc.h"
#include "utils/time.h"
#include "os.h"

extern	CNPC*			pNPC[MAX_NPCS];
extern	CSAMP*			pSaMp;
extern	unsigned short	MaxPlayers;

void SAMP_ThreadComplete()
{
	double time = microtime();
	// ��������� ������ � ������� ��� ����������� �����
	CNPCDamage::UpdatePlayersData(pSaMp->players,time);
	// ��������� ��������� NPC
	for(unsigned short npcid = 0;npcid < MaxPlayers;npcid++)
	{
		if(pNPC[npcid]) pNPC[npcid]->Update(time);
	}
	// �������� ������� �����
	CallPathCalculationCallback();
	// ���
	SLEEP(5);
}