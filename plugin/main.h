/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 */

#define PLUGIN_VERSION			"0.3.3 R1"

// plugin
void **ppPluginData;
extern void *pAMXFunctions;
// logprintf
typedef void (*logprintf_t)(char* format, ...);
logprintf_t logprintf;
// callpublicfs
typedef void (*AmxCallPublicFilterScript_t)(char* szFunctionName);
AmxCallPublicFilterScript_t AmxCallPublicFilterScript;
// callpublicgm
typedef void (*AmxCallPublicGameMode_t)(char* szFunctionName);
AmxCallPublicGameMode_t AmxCallPublicGameMode;
// csamphandle
typedef int (*CSampHandle_t) ();
CSampHandle_t CSampHandle;
// cconfighandle
typedef int (*CConfigHandle_t) ();
CConfigHandle_t CConfigHandle;
// raknethandle
typedef int (*RaknetHandle_t) ();
RaknetHandle_t RaknetHandle;
// loadfs
typedef void (*AmxLoadFilterScript_t)(char* szFileName,int unknown);
AmxLoadFilterScript_t AmxLoadFilterScript;
// unloadfs
typedef void (*AmxUnloadFilterScript_t)(char* szFileName);
AmxUnloadFilterScript_t AmxUnloadFilterScript;
