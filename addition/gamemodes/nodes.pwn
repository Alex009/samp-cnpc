/*
*	Created:			03.04.10
*	Author:				009
*	Last Modifed:		14.07.10
*	Description:        Nodes test
*/

main() {}

// --------------------------------------------------
// includes
// --------------------------------------------------
#include <a_samp>
#include <cnpc>

// --------------------------------------------------
// defines
// --------------------------------------------------
#if !defined MAX_STRING
	#define MAX_STRING 128
#endif

#define TEST_NODE_NAME "Nodes/NODES0.DAT"

// --------------------------------------------------
// news
// --------------------------------------------------
new strtmp[MAX_STRING],
	Float:floattmp[3],
	Node:path;

// --------------------------------------------------
// publics
// --------------------------------------------------
public OnGameModeInit()
{
	// add class
	AddPlayerClass(1,-1657.8768,-165.0978,14.1484,308.8885,0,0,0,0,0,0);
	// read node
	path = OpenNode(TEST_NODE_NAME);
	new header[4];
	GetNodeHeader(path,header[0],header[1],header[2],header[3]);
	printf("[HEADER] all:%d , vehicles: %d , peds: %d , navi: %d",header[0],header[1],header[2],header[3]);
	for(new i = 0;i < header[1];i++)
	{
	    SetNodePoint(path,i);
		GetNodePointPos(path,floattmp[0],floattmp[1],floattmp[2]);
		printf("[VEHICLE] id %d , pos %f,%f,%f",i,floattmp[0],floattmp[1],floattmp[2]);
		format(strtmp,sizeof(strtmp),"Vehicle path\nPoint %d",i);
		Create3DTextLabel(strtmp,0xFFFFFFFF,floattmp[0],floattmp[1],floattmp[2] + 1.0,100.0,0,1);
	}
	for(new i = header[1];i < (header[1] + header[2]);i++)
	{
	    SetNodePoint(path,i);
		GetNodePointPos(path,floattmp[0],floattmp[1],floattmp[2]);
		printf("[PED] id %d , pos %f,%f,%f",i,floattmp[0],floattmp[1],floattmp[2]);
		format(strtmp,sizeof(strtmp),"Ped path\nPoint %d",i);
		Create3DTextLabel(strtmp,0x000000FF,floattmp[0],floattmp[1],floattmp[2] + 1.0,100.0,0,1);
	}
	print("Node loaded.");
}

public OnGameModeExit()
{
    CloseNode(path);
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	new cmd[20],
		idx;

	cmd = strtok(cmdtext,idx);

	if(!strcmp(cmd,"/money",true))
	{
		GivePlayerMoney(playerid,strval(strtok(cmdtext,idx)));
	    return 1;
	}
	if(!strcmp(cmd,"/w",true))
	{
		GivePlayerWeapon(playerid,strval(strtok(cmdtext,idx)),999);
	    return 1;
	}
	if(!strcmp(cmd,"/car",true))
	{
		new Float:pos[4];
		GetPlayerPos(playerid,pos[0],pos[1],pos[2]);
		GetPlayerFacingAngle(playerid,pos[3]);
	    PutPlayerInVehicle(playerid,AddStaticVehicle(strval(strtok(cmdtext,idx)),pos[0],pos[1],pos[2],pos[3],0,0),0);
	    return 1;
	}
	if(!strcmp(cmd,"/tp",true))
	{
		SetNodePoint(path,strval(strtok(cmdtext,idx)));
		GetNodePointPos(path,floattmp[0],floattmp[1],floattmp[2]);
	    SetPlayerPos(playerid,floattmp[0],floattmp[1],floattmp[2] + 2.0);
	    return 1;
	}
	if(!strcmp(cmd,"/cp",true))
	{
		SetNodePoint(path,strval(strtok(cmdtext,idx)));
		GetNodePointPos(path,floattmp[0],floattmp[1],floattmp[2]);
		SetPlayerCheckpoint(playerid,floattmp[0],floattmp[1],floattmp[2],1.0);
	    return 1;
	}
	return 0;
}

// --------------------------------------------------
// stocks
// --------------------------------------------------
stock strtok(const string[], &index)
{
	new length = strlen(string);
	while ((index < length) && (string[index] <= ' '))
	{
		index++;
	}

	new offset = index;
	new result[20];
	while ((index < length) && (string[index] > ' ') && ((index - offset) < (sizeof(result) - 1)))
	{
		result[index - offset] = string[index];
		index++;
	}
	result[index - offset] = EOS;
	return result;
}
