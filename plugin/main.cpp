/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 */

// SDK
#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"
// head
#include "main.h"
#include "SAMP/CSAMP.h"
#include "SAMP/address.h"
#include "CNPC.h"
#include "CNode.h"
#include "CNodeManager.h"
#include "CNodePath.h"
#include "CCallbacks.h"
#include "CZMap.h"
#include "hooks.h"
#include "natives.h"
#include "defines.h"
#include "utils/md5.h"
#include <string.h>
#include "demo_mode.h"
#include "os.h"

CNPC*			pNPC[MAX_NPCS];
CNode*			pNodes[MAX_NODES];
CNodeManager*	pNodeManager = NULL;
CNodePath*		pNodePaths[MAX_NODE_PATHS];
CZMap*			pZMap = NULL;
CHooks*			pHooks = NULL;
CSAMP*			pSaMp = NULL;
CCallbacks*		pCallbacks = NULL;
unsigned short	MaxPlayers;
bool			VisualDeath = true;
AMX*			pAMX;

float GetZCoord(float x,float y)
{
	if(pZMap) return pZMap->GetZForCoords(x,y);
	else return 0.0;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load( void **ppData ) 
{
	unsigned char output[16];
	char str[33];
	memset(str,0,33);

	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
	AmxCallPublicFilterScript = (AmxCallPublicFilterScript_t)ppData[PLUGIN_DATA_CALLPUBLIC_FS];
	AmxCallPublicGameMode = (AmxCallPublicGameMode_t)ppData[PLUGIN_DATA_CALLPUBLIC_GM];
	CSampHandle = (CSampHandle_t)ppData[PLUGIN_DATA_CSAMP_HANDLE];
	CConfigHandle = (CConfigHandle_t)ppData[PLUGIN_DATA_CCONFIG_HANDLE];
	RaknetHandle = (RaknetHandle_t)ppData[PLUGIN_DATA_RAKNET_HANDLE];
	AmxLoadFilterScript = (AmxLoadFilterScript_t)ppData[PLUGIN_DATA_LOAD_FILTERSCRIPT];
	AmxUnloadFilterScript = (AmxUnloadFilterScript_t)ppData[PLUGIN_DATA_UNLOAD_FILTERSCRIPT];
	
	// plugin load
	logprintf("================");
	logprintf("Controllable NPC v " PLUGIN_VERSION);
	logprintf("by 009");
	logprintf("for SA-MP " SAMP_VERSION " server");
	// check samp server version
	if(Curl_md5file((unsigned char*)output,SAMP_SERVER_FILE))
	{
		for(int i = 0;i < 16;i++) sprintf(str,"%s%X",str,output[i]);
		if(strcmp(str,SAMP_SERVER_MD5))
		{
			logprintf("md5 hash of your server is invalid, please check your " SAMP_SERVER_FILE);
			logprintf("md5 should be %s, your server is %s ",SAMP_SERVER_MD5,str);
		}
	}
	else logprintf("can't check md5 hash (maybe you have incorrect SA-MP server vesion), check your " SAMP_SERVER_FILE);
	// null data
	for(unsigned short i = 0;i < MAX_NPCS;i++) pNPC[i] = NULL;
	for(unsigned short i = 0;i < MAX_NODES;i++) pNodes[i] = NULL;
	for(unsigned short i = 0;i < MAX_NODE_PATHS;i++) pNodePaths[i] = NULL;
	// init static data
	CNPCDamage::Init();
	// hooks load
	pHooks = new CHooks(reinterpret_cast<unsigned long>(ppData[PLUGIN_DATA_CSAMP_HANDLE]));
	// callbacks
	pCallbacks = new CCallbacks();
	// max players
	MaxPlayers = pHooks->GetMaxPlayers();
	// data
	logprintf("max players detected: %d",MaxPlayers);
	// demo mode
#ifdef DEMO_MODE
	logprintf("It's demo version (max players 1), please buy full version");
	logprintf("Information: http://alekseymikhailov.blogspot.com/");
	logprintf("Contact:");
	logprintf("\tSkype: Aleksey.Mikhailov.ru");
	logprintf("\tICQ: 5306763");
	logprintf("\tE-mail: Aleksey.Mikhailov.ru@gmail.com");
#endif
	// complete
	logprintf("================");

	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload( )
{
	logprintf("================");
	logprintf("Controllable NPC v " PLUGIN_VERSION);
	logprintf("by 009");
	logprintf("================");
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad( AMX *amx ) 
{
	pAMX = amx;

	// link samp pointer
	pSaMp = (CSAMP*)CSampHandle();
	// natives
	NativesOnAMXLoad(amx);
	pCallbacks->CallbacksOnAMXLoad(amx);
	return AMX_ERR_NONE;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload( AMX *amx ) 
{
	pCallbacks->CallbacksOnAMXUnLoad(amx);
	return AMX_ERR_NONE;
}
