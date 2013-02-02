/*
*	Created:			20.08.12
*	Author:				009 & OKStyle
*	Last Modifed:		06.09.12
*	Description:		Controllable NPC's missions filterscript
*/

main() {}

// --------------------------------------------------
// includes
// --------------------------------------------------
#include <a_samp>
#include <cnpc>
#include <cnpc_controller>

// --------------------------------------------------
// defines
// --------------------------------------------------
#define VERSION             "0.1 beta"

#if !defined MAX_STRING
	#define MAX_STRING 128
#endif

#define MISSIONS_COUNT 3
#define MESSAGES_COLOR 0xFFFFFFFF

// --------------------------------------------------
// enums
// --------------------------------------------------
enum MissionInfo
{
	name[64],			// название миссии
	pickupmodel,        // id модели пикапа
	Float:start[3],		// координаты начала миссии
	request,			// id миссии которая должна быть пройдена для запуска этой
	pickup				// id созданного на старте пикапа
};
enum PlayerInfo
{
	progress[MISSIONS_COUNT],	// пройденные миссии (флаги - 1 или 0)
	current_mission,			// текущая миссия
	current_step,				// текущий этап миссии
	iscutscene,					// флаг - текущий этап это катсцена?
	timer						// таймер, сохраняем для обрывания
};


// --------------------------------------------------
// news
// --------------------------------------------------
new strtmp[MAX_STRING],
	missions[MISSIONS_COUNT][MissionInfo] = {
		{"Let's Rock! Part 1",1559,{1723.5537,-1721.2167,13.5466},-1,-1},
		{"Let's Rock! Part 2",1559,{1720.5525,-1721.3824,13.5462},-1,-1},
		{"Case delivery",1559,{2069.9470,-1766.6082,13.5621},-1,-1}
	},
	players[MAX_PLAYERS][PlayerInfo],
	Text:Letterbox[2],
	player_mission_data[MAX_PLAYERS][MISSIONS_COUNT][20];
new Float:ftmp[6];

// --------------------------------------------------
// forwards
// --------------------------------------------------
forward Mission_CutsceneStep(playerid);
forward Mission_InProgressStep(playerid);

// --------------------------------------------------
// publics
// --------------------------------------------------
public OnFilterScriptInit()
{
	// подключаем ZMap
	zmap_Init("SAfull.hmap",ZMAP_MODE_CHACHE);
	// подключаем менеджер путей
	nodes_Init();
	// подключаем модуль обработки путей
	paths_Init();
	// добавляем пикапы начала миссий
	for(new i = 0;i < MISSIONS_COUNT;i++)
	{
		missions[i][pickup] = CreatePickup(missions[i][pickupmodel],1,missions[i][start][0],missions[i][start][1],missions[i][start][2]);
	}
	// создаем TextDraw
	Letterbox[0] = TextDrawCreate(0.0,0.0,"_");
	TextDrawUseBox(Letterbox[0],1);
	TextDrawBoxColor(Letterbox[0],0x000000ff);
	TextDrawLetterSize(Letterbox[0],0.0,12.2);
	Letterbox[1] = TextDrawCreate(0.0,480.0,"_");
	TextDrawUseBox(Letterbox[1],1);
	TextDrawBoxColor(Letterbox[1],0x000000ff);
	TextDrawLetterSize(Letterbox[1],0.0,-17.0);
	// для онлайн игроков задаем значения (если скрипт догружен во время работы)
	for(new playerid = 0;playerid < MAX_PLAYERS;playerid++)
	{
	    if(!IsPlayerConnected(playerid)) continue;
	    
	    OnPlayerConnect(playerid);
	}
	
	print("Controllable NPC's missions " VERSION " loaded.");
}

public OnFilterScriptExit()
{
    // отключаем модуль обработки путей
	paths_Exit();
	// отключаем менеджер путей
	nodes_Exit();
    // отключаем ZMap
	zmap_Exit();
	// удаляем пикапы начала миссий
	for(new i = 0;i < MISSIONS_COUNT;i++) DestroyPickup(missions[i][pickup]);
	// удаляем TextDraw
	TextDrawDestroy(Letterbox[0]);
	TextDrawDestroy(Letterbox[1]);
	// для онлайн игроков задаем значения (если скрипт выгружен во время работы)
	for(new playerid = 0;playerid < MAX_PLAYERS;playerid++)
	{
	    if(!IsPlayerConnected(playerid)) continue;

	    OnPlayerDisconnect(playerid,0);
	}
	
	print("Controllable NPC's missions " VERSION " unloaded.");
}

public OnPlayerConnect(playerid)
{
	for(new i = 0;i < MISSIONS_COUNT;i++) players[playerid][progress][i] = 0;
	players[playerid][current_mission] = -1;
	players[playerid][current_step] = -1;
	players[playerid][timer] = -1;
}

public OnPlayerDisconnect(playerid, reason)
{
    for(new i = 0;i < MISSIONS_COUNT;i++) players[playerid][progress][i] = 0;
	Mission_Clear(playerid);
}

public OnPlayerUpdate(playerid)
{
    controller_OnPlayerUpdate(playerid);
    return 1;
}

public OnPlayerPickUpPickup(playerid, pickupid)
{
	// ищем миссию этого пикапа
	for(new i = 0;i < sizeof(missions);i++)
	{
		if(missions[i][pickup] == pickupid)
		{
			// миссия найдена, запускаем
			Mission_Start(i,playerid);
			// готово, обрываем цикл
			break;
		}
	}
	return 1;
}

public OnNPCGetDamage(npcid,playerid,Float:health_loss)
{

	return 1;
}

public OnPathCalculated(pathid)
{
    paths_OnPathCalculated(pathid);
}

public OnCompleteCalcPathForNPC(npcid)
{
    controller_OnCompCalcPathForNPC(npcid);
}

public OnNPCMovingComplete(npcid)
{
	controller_OnNPCMovingComplete(npcid);
}

public OnNPCSpawn(npcid)
{
    controller_OnNPCSpawn(npcid);
}

public OnNPCDeath(npcid,killerid,reason)
{
    controller_OnNPCDeath(npcid,killerid,reason);
}

public OnStartCalcPathForNPC(npcid)
{

}

public OnPlayerCommandText(playerid, cmdtext[])
{
	new cmd[20],
		idx;

	cmd = strtok(cmdtext,idx);

	if(!strcmp(cmd,"/mission",true))
	{
	    new i = strval(strtok(cmdtext,idx));
	    SetPlayerPos(playerid,missions[i][start][0],missions[i][start][1],missions[i][start][2]);
	    return 1;
	}
	return 0;
}

public Mission_CutsceneStep(playerid)
{
	// проверяем в катсцене ли игрок, если нет (произошла ошибка ><)
	if(!players[playerid][iscutscene]) return printf("[ERROR - missions.amx] Mission_CutsceneStep called for player not in cutscene");
	// определяем текущую миссию
	new missionid = players[playerid][current_mission];
	switch(missionid)
	{
		case 0: // Let's Rock! Part 1
		{
			// определяем текущий этап катсцены
			switch(players[playerid][current_step])
			{
				case 0: // начало показа
				{
					SetPlayerPos(playerid,1723.5537,-1721.2167,13.5466);
					SetPlayerFacingAngle(playerid,89.5532);
				    TogglePlayerControllable(playerid,false);
					TextDrawShowForPlayer(playerid,Letterbox[0]);
					TextDrawShowForPlayer(playerid,Letterbox[1]);
					SetPlayerSpecialAction(playerid,SPECIAL_ACTION_USECELLPHONE);

					SendClientMessage(playerid,0x00FF00FF,"Проследи за одним человеком, где его найти указано в бумажке воткнутой за аппаратом");
					SendClientMessage(playerid,0x00FF00FF,"Он должен вывести тебя на предателей из нашей конторы, как только увидишь всех - убей");
					
					players[playerid][current_step]++;
					players[playerid][timer] = SetTimerEx("Mission_CutsceneStep",10000,0,"d",playerid);
				}
				case 1: // конец показа
				{
				    SetPlayerSpecialAction(playerid,SPECIAL_ACTION_STOPUSECELLPHONE);
                    SetCameraBehindPlayer(playerid);
					TextDrawHideForPlayer(playerid,Letterbox[0]);
					TextDrawHideForPlayer(playerid,Letterbox[1]);
					TogglePlayerControllable(playerid,true);
				
					// устанавливаем данные для начала миссии
					players[playerid][current_step] = 0;
					players[playerid][iscutscene] = 0;
					// запускаем миссию
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",0,0,"d",playerid);
				}
			}
		}
		case 1: // Let's Rock! Part 2
		{
		    // определяем текущий этап катсцены
			switch(players[playerid][current_step])
			{
				case 0: // начало показа
				{
					SetPlayerPos(playerid,1720.5525,-1721.3824,13.5462);
					SetPlayerFacingAngle(playerid,272.0335);
				    TogglePlayerControllable(playerid,false);
					TextDrawShowForPlayer(playerid,Letterbox[0]);
					TextDrawShowForPlayer(playerid,Letterbox[1]);
					SetPlayerSpecialAction(playerid,SPECIAL_ACTION_USECELLPHONE);

					SendClientMessage(playerid,0x00FF00FF,"В прошлый раз ты убил не всех предателей, точнее один из них не пришёл на встречу");
					SendClientMessage(playerid,0x00FF00FF,"Теперь он выдал позицию нашей фабрики агентам ФБР, они захватили её");
					SendClientMessage(playerid,0x00FF00FF,"Наших повязали, но как мне известно, они ещё не добрались до данных компьютера");
					SendClientMessage(playerid,0x00FF00FF,"Нужно забрать эти данные до того как они доберутся до них");

					players[playerid][current_step]++;
					players[playerid][timer] = SetTimerEx("Mission_CutsceneStep",10000,0,"d",playerid);
				}
				case 1: // конец показа
				{
				    SetPlayerSpecialAction(playerid,SPECIAL_ACTION_STOPUSECELLPHONE);
                    SetCameraBehindPlayer(playerid);
					TextDrawHideForPlayer(playerid,Letterbox[0]);
					TextDrawHideForPlayer(playerid,Letterbox[1]);
					TogglePlayerControllable(playerid,true);

					// устанавливаем данные для начала миссии
					players[playerid][current_step] = 0;
					players[playerid][iscutscene] = 0;
					// запускаем миссию
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",0,0,"d",playerid);
				}
			}
		}
		case 2: // Case delivery
		{
		    // определяем текущий этап катсцены
			switch(players[playerid][current_step])
			{
				case 0: // начало показа
				{
				    SetPlayerPos(playerid,2069.9470,-1766.6082,13.5621);
                    SetPlayerFacingAngle(playerid,89.4755);
                    SetCameraBehindPlayer(playerid);
				    TogglePlayerControllable(playerid,false);
					TextDrawShowForPlayer(playerid,Letterbox[0]);
					TextDrawShowForPlayer(playerid,Letterbox[1]);
					SetPlayerSpecialAction(playerid,SPECIAL_ACTION_USECELLPHONE);
					
					SendClientMessage(playerid,0x00FF00FF,"Слушай, наши люди потеряли кейс с деньгами, ты должен вернуть его");
					SendClientMessage(playerid,0x00FF00FF,"Но будь аккуратен, мафия знает о этих деньгах");
					
					players[playerid][current_step]++;
					players[playerid][timer] = SetTimerEx("Mission_CutsceneStep",10000,0,"d",playerid);
				}
				case 1: // конец показа
				{
				    SetPlayerSpecialAction(playerid,SPECIAL_ACTION_STOPUSECELLPHONE);
					TextDrawHideForPlayer(playerid,Letterbox[0]);
					TextDrawHideForPlayer(playerid,Letterbox[1]);
					TogglePlayerControllable(playerid,true);
					
					// устанавливаем данные для начала миссии
					players[playerid][current_step] = 0;
					players[playerid][iscutscene] = 0;
					// запускаем миссию
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",0,0,"d",playerid);
				}
			}
		    
		}
	}
	return 1;
}

public Mission_InProgressStep(playerid)
{
	// проверяем в катсцене ли игрок, если да (произошла ошибка ><)
	if(players[playerid][iscutscene]) return printf("[ERROR - missions.amx] Mission_InProgressStep called for player in cutscene");
	// определяем текущую миссию
	new missionid = players[playerid][current_mission];
	switch(missionid)
	{
		case 0: // Let's Rock! Part 1
		{
			// определяем текущий этап выполнения
			switch(players[playerid][current_step])
			{
				case 0: // начало миссии
				{
					// отмечаем зону куда нужно приехать
					SetPlayerCheckpoint(playerid,1189.6824,-1275.7974,13.2871,0.0);
					
					// устанавливаем данные
					players[playerid][current_step]++;
					// ожидаем следующий шаг
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
				}
				case 1:
				{
				    // ожидаем пока игрок будет близко к цели
					if(IsPlayerInRangeOfPoint(playerid,75.0,1189.6824,-1275.7974,13.2871))
				    {
				        KillTimer(players[playerid][timer]);
				        DisablePlayerCheckpoint(playerid);
				        // начинаем видео вставку
						TogglePlayerControllable(playerid,false);
						TextDrawShowForPlayer(playerid,Letterbox[0]);
						TextDrawShowForPlayer(playerid,Letterbox[1]);
						// создаём машину NPC
						player_mission_data[playerid][missionid][0] = CreateVehicle(421,1189.6824,-1275.7974,13.2871,90.6647,0,0,-1);
						// создаём самого NPC
						player_mission_data[playerid][missionid][1] = FindLastFreeSlot();
					    format(strtmp,sizeof(strtmp),"MissionNPC_%d",player_mission_data[playerid][missionid][1]);
					    CreateNPC(player_mission_data[playerid][missionid][1],strtmp);
					    SetSpawnInfo(player_mission_data[playerid][missionid][1],0,166,1180.1959,-1260.9313,15.1797,267.2988,0,0,0,0,0,0);
					    SpawnNPC(player_mission_data[playerid][missionid][1]);
					    // составляем путь движения к машине
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],1184.0657,-1267.7537,15.1797);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],1185.4275,-1271.0238,13.5469);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],1188.9404,-1274.0692,13.5469);
						// отправляем NPC по пути
						controller_MoveToByCoords(player_mission_data[playerid][missionid][1],NPC_MOVE_PED_WALK);
						// показываем его
						InterpolateCameraLookAt(playerid,1180.1959,-1260.9313,15.1797,1189.6824,-1275.7974,13.2871,10000,CAMERA_MOVE);
						
						SendClientMessage(playerid,0x00FF00FF,"А вот и цель, нужно проследить куда он отправится...");

						// устанавливаем данные
						players[playerid][current_step]++;
						// ожидаем следующий шаг
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
				    }
				}
				case 2:
				{
				    // ожидаем пока NPC подойдёт к машине
					if(!controller_IsMoving(player_mission_data[playerid][missionid][1]))
				    {
				        KillTimer(players[playerid][timer]);
				        // заканчиваем видео вставку
					    SetCameraBehindPlayer(playerid);
						TextDrawHideForPlayer(playerid,Letterbox[0]);
						TextDrawHideForPlayer(playerid,Letterbox[1]);
						TogglePlayerControllable(playerid,true);
						// садим NPC в машину
						PutNPCInVehicle(player_mission_data[playerid][missionid][1],player_mission_data[playerid][missionid][0],0);
						// отправляем по пути
						controller_DriveToByNodes(player_mission_data[playerid][missionid][1],1022.8997,-1135.9625,23.2232,20.0,0);
						// устанавливаем данные
						player_mission_data[playerid][missionid][2] = 0;

						SendClientMessage(playerid,0x00FF00FF,"Нельзя приближаться слишком близко...");

						// устанавливаем данные
						players[playerid][current_step]++;
						// ожидаем следующий шаг
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
				    }
				}
				case 3:
				{
				    // ожидаем пока NPC доедет до первой цели
					if(!controller_IsMoving(player_mission_data[playerid][missionid][1]))
				    {
				        KillTimer(players[playerid][timer]);

						SendClientMessage(playerid,0x00FF00FF,"Что он там остановился?");

						// устанавливаем данные
						players[playerid][current_step]++;
						// ожидаем следующий шаг
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",5000,0,"d",playerid);
				    }
				    else
				    {
				        GetPlayerPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
				        // проверяем не отстал ли игрок
				        if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][1],250.0,ftmp[0],ftmp[1],ftmp[2]))
				        {
				            // проверяем не подошёл ли игрок слишком близко
					        if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][1],50.0,ftmp[0],ftmp[1],ftmp[2]))
					    	{
					    	    // проверяем на критическое расстояние
					    	    if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][1],25.0,ftmp[0],ftmp[1],ftmp[2]))
					    		{
					    		    // слишком близко, миссия провалена
					    		    SendClientMessage(playerid,0x00FF00FF,"Черт, он меня заметил...теперь он не поедет на встречу.");
					    		    GameTextForPlayer(playerid, "~g~~h~Mission Failed!", 3000, 3);
		    						Mission_Clear(playerid);
					    		}
					    		else
					    		{
					    		    // расстояние предупреждения
					    		    SendClientMessage(playerid,0x00FF00FF,"Я слишком близко, он может заметить");
					    		}
					    	}
					    	// обнуляем счётчик отставания
					    	player_mission_data[playerid][missionid][2] = 0;
						}
						else
						{
						    SendClientMessage(playerid,0x00FF00FF,"Я слишком далеко, нужно подобраться ближе");
						    player_mission_data[playerid][missionid][2]++;
						    
						    if(player_mission_data[playerid][missionid][2] == 20)
						    {
						        SendClientMessage(playerid,0x00FF00FF,"Черт, я упустил его...");
				    		    GameTextForPlayer(playerid, "~g~~h~Mission Failed!", 3000, 3);
	    						Mission_Clear(playerid);
						    }
						}
				    }
				}
				case 4:
				{
				    // отправляем NPC к следующей точке
				    controller_DriveToByNodes(player_mission_data[playerid][missionid][1],817.9597,-1331.3726,12.9682,20.0,0);

					SendClientMessage(playerid,0x00FF00FF,"Поехал дальше...может быть ему позвонили?");
				    
				    // устанавливаем данные
					players[playerid][current_step]++;
					// ожидаем следующий шаг
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
				}
				case 5:
				{
				    // ожидаем пока NPC доедет до второй цели
					if(!controller_IsMoving(player_mission_data[playerid][missionid][1]))
				    {
				        KillTimer(players[playerid][timer]);
						// выпускаем его из машины
						PutNPCInVehicle(player_mission_data[playerid][missionid][1],0,0);
						SetNPCPos(player_mission_data[playerid][missionid][1],817.1392,-1334.9526,13.5469);
						SetNPCFacingAngle(player_mission_data[playerid][missionid][1],182.0857);
						// записываем путь движения
						controller_ResetData(player_mission_data[playerid][missionid][1]);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],824.0291,-1341.0946,13.5157);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],829.2626,-1341.5818,11.0234);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],835.9733,-1341.7489,7.1719);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],837.5139,-1345.6136,7.1787);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],832.6624,-1352.0516,3.3359);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],827.7214,-1357.4257,-0.5078);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],823.3764,-1361.9156,-0.5078);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][1],804.6523,-1345.9818,-0.5078);
					    // отправляем его по пути
					    controller_MoveToByCoords(player_mission_data[playerid][missionid][1],NPC_MOVE_PED_WALK);
					    // создаём пару помошников
					    for(new i = 2;i < 4;i++)
					    {
					        player_mission_data[playerid][missionid][i] = FindLastFreeSlot();
						    format(strtmp,sizeof(strtmp),"MissionNPC_%d",player_mission_data[playerid][missionid][i]);
						    CreateNPC(player_mission_data[playerid][missionid][i],strtmp);
						    SetSpawnInfo(player_mission_data[playerid][missionid][i],0,165,0.0,0.0,0.0,0.0,0,0,0,0,0,0);
						    SpawnNPC(player_mission_data[playerid][missionid][i]);
					    }
					    // ставим их на места
					    SetNPCPos(player_mission_data[playerid][missionid][2],802.8537,-1341.7098,-0.5078);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][2],221.1942);
					    SetNPCPos(player_mission_data[playerid][missionid][3],800.3175,-1344.5209,-0.5078);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][3],238.0099);

						SendClientMessage(playerid,0x00FF00FF,"Он спускается, нужно идти за ним...");

						// устанавливаем данные
						players[playerid][current_step]++;
						// ожидаем следующий шаг
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
				    }
				    else
				    {
				        GetPlayerPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
				        // проверяем не отстал ли игрок
				        if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][1],250.0,ftmp[0],ftmp[1],ftmp[2]))
				        {
				            // проверяем не подошёл ли игрок слишком близко
					        if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][1],50.0,ftmp[0],ftmp[1],ftmp[2]))
					    	{
					    	    // проверяем на критическое расстояние
					    	    if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][1],25.0,ftmp[0],ftmp[1],ftmp[2]))
					    		{
					    		    // слишком близко, миссия провалена
					    		    SendClientMessage(playerid,0x00FF00FF,"Черт, он меня заметил...теперь он не поедет на встречу.");
					    		    GameTextForPlayer(playerid, "~g~~h~Mission Failed!", 3000, 3);
		    						Mission_Clear(playerid);
					    		}
					    		else
					    		{
					    		    // расстояние предупреждения
					    		    SendClientMessage(playerid,0x00FF00FF,"Я слишком близко, он может заметить");
					    		}
					    	}
						}
						else
						{
						    SendClientMessage(playerid,0x00FF00FF,"Я слишком далеко, нужно подобраться ближе");
						    player_mission_data[playerid][missionid][2]++;

						    if(player_mission_data[playerid][missionid][2] == 20)
						    {
						        SendClientMessage(playerid,0x00FF00FF,"Черт, я упустил его...");
				    		    GameTextForPlayer(playerid, "~g~~h~Mission Failed!", 3000, 3);
	    						Mission_Clear(playerid);
						    }
						}
				    }
				}
				case 6:
				{
				    // ожидаем пока NPC дойдёт до конца
					if(!controller_IsMoving(player_mission_data[playerid][missionid][1]))
				    {
				        KillTimer(players[playerid][timer]);
						// пусть NPC атакуют игрока, пока игрок их не убьёт
				        GetPlayerPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
						for(new i = 1;i < 4;i++)
						{
							SetNPCWeapon(player_mission_data[playerid][missionid][i],29);
							NPC_ShootAt(player_mission_data[playerid][missionid][i],ftmp[0],ftmp[1],ftmp[2]);
						}
						// а цель слежки крикнет что либо
						SetPlayerChatBubble(player_mission_data[playerid][missionid][1],"За мной хвост, он один, как я и говорил, убейте его!",0xFF0000FF,50.0,10000);
						// сообщим цель
						SendClientMessage(playerid,0xFF0000FF,"Вот и они, пора убить их всех!");

						// устанавливаем данные
						players[playerid][current_step]++;
						// ожидаем следующий шаг
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",1000,1,"d",playerid);
				    }
				}
				case 7:
				{
				    // ждём пока NPC не будут убиты
				    new i;
				    for(i = 1;i < 4;i++)
					{
						if(GetNPCState(player_mission_data[playerid][missionid][i]) != NPC_STATE_DEATH) break;
					}
					if(i == 4)
					{
					    // NPC мертвы
					    KillTimer(players[playerid][timer]);
				        DestroyVehicle(player_mission_data[playerid][missionid][0]);
				        for(i = 1;i < 4;i++) DestroyNPC(player_mission_data[playerid][missionid][i]);
				        GameTextForPlayer(playerid, "~g~~h~Mission Passed!", 3000, 3);
						PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
						GivePlayerMoney(playerid, 5000);

						// устанавливаем данные
						players[playerid][current_mission] = -1;
						players[playerid][current_step] = 0;
						players[playerid][iscutscene] = 0;
						players[playerid][progress][missionid] = 1;
						players[playerid][timer] = -1;
					}
				}
			}
		}
		case 1: // Let's Rock! Part 2
		{
		    // определяем текущий этап выполнения
			switch(players[playerid][current_step])
			{
				case 0: // начало миссии
				{
					// отмечаем зону куда нужно приехать
					SetPlayerCheckpoint(playerid,2148.4314,-2260.8513,13.3001,0.0);

					// устанавливаем данные
					players[playerid][current_step]++;
					// ожидаем следующий шаг
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
				}
				case 1:
				{
				    // ожидаем пока игрок будет близко к цели
					if(IsPlayerInRangeOfPoint(playerid,100.0,2148.4314,-2260.8513,13.3001))
				    {
				        KillTimer(players[playerid][timer]);
				        DisablePlayerCheckpoint(playerid);
						// начинаем считать время
						player_mission_data[playerid][missionid][0] = 0;
						// создаём NPC
						for(new i = 1;i < 9;i++)
						{
							player_mission_data[playerid][missionid][i] = FindLastFreeSlot();
						    format(strtmp,sizeof(strtmp),"MissionNPC_%d",player_mission_data[playerid][missionid][i]);
						    CreateNPC(player_mission_data[playerid][missionid][i],strtmp);
						    SetSpawnInfo(player_mission_data[playerid][missionid][i],0,166,0.0,0.0,0.0,0.0,0,0,0,0,0,0);
						    SpawnNPC(player_mission_data[playerid][missionid][i]);
						    SetPlayerColor(player_mission_data[playerid][missionid][i],0x00000000);
					    }
					    // расставляем по позициям
					    // 1
					    SetNPCPos(player_mission_data[playerid][missionid][1],2127.0657,-2287.4819,13.4600);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][1],220.1741);
					    SetNPCWeapon(player_mission_data[playerid][missionid][1],31);
					    // 2
					    SetNPCPos(player_mission_data[playerid][missionid][2],2128.6174,-2286.2454,13.4465);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][2],225.4591);
					    SetNPCWeapon(player_mission_data[playerid][missionid][2],31);
					    // 3
					    SetNPCPos(player_mission_data[playerid][missionid][3],2133.0415,-2278.0474,20.6719);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][3],313.6685);
					    SetNPCWeapon(player_mission_data[playerid][missionid][3],31);
					    // 4
					    SetNPCPos(player_mission_data[playerid][missionid][4],2139.4290,-2267.6992,13.3044);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][4],262.7409);
					    SetNPCWeapon(player_mission_data[playerid][missionid][4],24);
					    // 5
					    SetNPCPos(player_mission_data[playerid][missionid][5],2143.0178,-2267.9617,13.2936);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][5],78.7289);
					    SetNPCWeapon(player_mission_data[playerid][missionid][5],24);
					    // 6
					    SetNPCPos(player_mission_data[playerid][missionid][6],2179.9490,-2253.9900,14.7734);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][6],226.3703);
					    SetNPCWeapon(player_mission_data[playerid][missionid][6],31);
					    // 7
					    SetNPCPos(player_mission_data[playerid][missionid][7],2177.6272,-2255.6787,14.7700);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][7],226.3703);
					    SetNPCWeapon(player_mission_data[playerid][missionid][7],31);
					    // 8
					    SetNPCPos(player_mission_data[playerid][missionid][8],2167.3552,-2254.1885,13.3019);
					    SetNPCFacingAngle(player_mission_data[playerid][missionid][8],39.8125);
					    SetNPCWeapon(player_mission_data[playerid][missionid][8],31);
					    // для последнего сделаем путь патрулирования
					    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2160.9036,-2247.5107,13.2927);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2165.6145,-2241.9050,13.2875);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.6187,-2237.7000,13.2937);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2151.6653,-2247.4675,13.2987);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.2144,-2257.1599,13.3008);
					    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2152.7708,-2265.6494,13.3009);
					    // и отправим его по этому пути
					    controller_MoveToByCoords(player_mission_data[playerid][missionid][8],NPC_MOVE_PED_WALK);
					    // укажем игроку куда ему нужно попасть
					    SetPlayerCheckpoint(playerid,2123.2700,-2275.2988,20.6719,2.0);
					    // указываем что тревоги нет
					    player_mission_data[playerid][missionid][9] = 0;
					    
						SendClientMessage(playerid,0x00FF00FF,"ФБР охраняют входы...");
						SendClientMessage(playerid,0x00FF00FF,"Либо я их убью...");
						SendClientMessage(playerid,0x00FF00FF,"Либо попаду в кабинет запрыгнув на лестницу позади агентов...");

						// устанавливаем данные
						players[playerid][current_step]++;
						// ожидаем следующий шаг
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
				    }
				}
				case 2:
				{
				    player_mission_data[playerid][missionid][0]++;
				    // проверяем не кончилось ли время
				    if(player_mission_data[playerid][missionid][0] == 300)
				    {
				        SendClientMessage(playerid,0x00FF00FF,"Они добрались до данных...");
		    		    GameTextForPlayer(playerid, "~g~~h~Mission Failed!", 3000, 3);
						Mission_Clear(playerid);
				    }
				    else
				    {
					    // проверяем на позиции ли игрок
					    if(IsPlayerInRangeOfPoint(playerid,2.0,2123.2700,-2275.2988,20.6719))
					    {
					        KillTimer(players[playerid][timer]);
					        DisablePlayerCheckpoint(playerid);

					        SendClientMessage(playerid,0x00FF00FF,"Готово, нужно уходить отсюда как можно дальше");

							// устанавливаем данные
							players[playerid][current_step]++;
							// ожидаем следующий шаг
							players[playerid][timer] = SetTimerEx("Mission_InProgressStep",500,1,"d",playerid);
						}
						else
						{
						    GetPlayerPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
						    // проверяем действия NPC
						    // проверяем поднята ли тревога
						    if(player_mission_data[playerid][missionid][9] == 1)
						    {
						        // все NPC стреляют в игрока на поражение
								for(new i = 1;i < 9;i++)
								{
								    if(GetNPCState(player_mission_data[playerid][missionid][i]) == NPC_STATE_DEATH) continue; // пропускаем трупов
								    
								    if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][i],20.0,ftmp[0],ftmp[1],ftmp[2]))
										NPC_ShootAt(player_mission_data[playerid][missionid][i],ftmp[0],ftmp[1],ftmp[2]);
								}
						    }
						    else
						    {
						        // болтуны
							    ApplyAnimation(player_mission_data[playerid][missionid][4],"PED","IDLE_chat",4.1,1,0,0,0,0,1);
							    ApplyAnimation(player_mission_data[playerid][missionid][5],"PED","IDLE_chat",4.1,1,0,0,0,0,1);
							    // патрульный
							    if(!controller_IsMoving(player_mission_data[playerid][missionid][8]))
							    {
							        if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][8],1.0,2152.7708,-2265.6494,13.3009))
							        {
									    // сделаем обратный путь
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.2144,-2257.1599,13.3008);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2151.6653,-2247.4675,13.2987);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.6187,-2237.7000,13.2937);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2165.6145,-2241.9050,13.2875);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2160.9036,-2247.5107,13.2927);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2167.3552,-2254.1885,13.3019);
									    // и отправим его по этому пути
									    controller_MoveToByCoords(player_mission_data[playerid][missionid][8],NPC_MOVE_PED_WALK);
								    }
								    else
								    {
								        // повторяем опять
								        controller_AddPathCoords(player_mission_data[playerid][missionid][8],2160.9036,-2247.5107,13.2927);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2165.6145,-2241.9050,13.2875);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.6187,-2237.7000,13.2937);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2151.6653,-2247.4675,13.2987);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.2144,-2257.1599,13.3008);
									    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2152.7708,-2265.6494,13.3009);
									    // и отправим его по этому пути
									    controller_MoveToByCoords(player_mission_data[playerid][missionid][8],NPC_MOVE_PED_WALK);
								    }
							    }
							    // проверяем нет ли игрока перед NPC
							    for(new i = 1;i < 9;i++)
								{
								    if(GetNPCState(player_mission_data[playerid][missionid][i]) == NPC_STATE_DEATH) continue; // пропускаем трупов
								    
								    if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][i],50.0,ftmp[0],ftmp[1],ftmp[2]))
								    {
								        GetNPCPos(player_mission_data[playerid][missionid][i],ftmp[3],ftmp[4],ftmp[5]);
								        // игрок около NPC, проверяем с какой стороны
								        ftmp[2] = atan2((ftmp[3] - ftmp[0]),(ftmp[4] - ftmp[1]));
								        ftmp[5] = GetNPCFacingAngle(player_mission_data[playerid][missionid][i]);
										if(ftmp[2] < 0.0) ftmp[2] += 360.0;
								        // если в области перед NPC
								        if((ftmp[2] > (ftmp[5] - 45.0)) &&(ftmp[2] < (ftmp[5] + 45.0)))
								        {
								            // объявляем тревогу
								            player_mission_data[playerid][missionid][9] = 1;
								            // останавливаем болтунов
								            ClearAnimations(player_mission_data[playerid][missionid][4]);
								            ClearAnimations(player_mission_data[playerid][missionid][5]);
								            // пусть NPC крикнет
											SetPlayerChatBubble(player_mission_data[playerid][missionid][i],"Здесь посторонний!",0xFF0000FF,50.0,10000);
								            
								            SendClientMessage(playerid,0x00FF00FF,"Черт, меня заметили!");
								            break;
								        }
								    }
								}
						    }
						}
					}
				}
				case 3:
				{
				    // ожидаем пока игрок уедет
				    if(!IsPlayerInRangeOfPoint(playerid,200.0,2148.4314,-2260.8513,13.3001))
				    {
					    KillTimer(players[playerid][timer]);
				        for(new i = 1;i < 9;i++) DestroyNPC(player_mission_data[playerid][missionid][i]);
				        GameTextForPlayer(playerid, "~g~~h~Mission Passed!", 3000, 3);
						PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
						GivePlayerMoney(playerid, 5000);

						// устанавливаем данные
						players[playerid][current_mission] = -1;
						players[playerid][current_step] = 0;
						players[playerid][iscutscene] = 0;
						players[playerid][progress][missionid] = 1;
						players[playerid][timer] = -1;
				    }
					else
					{
					    GetPlayerPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
					    // проверяем действия NPC
					    // проверяем поднята ли тревога
					    if(player_mission_data[playerid][missionid][9] == 1)
					    {
					        // все NPC стреляют в игрока на поражение
							for(new i = 1;i < 9;i++)
							{
								if(GetNPCState(player_mission_data[playerid][missionid][i]) == NPC_STATE_DEATH) continue; // пропускаем трупов
								    
							    if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][i],20.0,ftmp[0],ftmp[1],ftmp[2]))
									NPC_ShootAt(player_mission_data[playerid][missionid][i],ftmp[0],ftmp[1],ftmp[2]);
							}
					    }
					    else
					    {
					        // болтуны
						    ApplyAnimation(player_mission_data[playerid][missionid][4],"PED","IDLE_chat",4.1,1,0,0,0,0,1);
						    ApplyAnimation(player_mission_data[playerid][missionid][5],"PED","IDLE_chat",4.1,1,0,0,0,0,1);
						    // патрульный
						    if(!controller_IsMoving(player_mission_data[playerid][missionid][8]))
						    {
						        if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][8],1.0,2152.7708,-2265.6494,13.3009))
						        {
								    // сделаем обратный путь
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.2144,-2257.1599,13.3008);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2151.6653,-2247.4675,13.2987);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.6187,-2237.7000,13.2937);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2165.6145,-2241.9050,13.2875);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2160.9036,-2247.5107,13.2927);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2167.3552,-2254.1885,13.3019);
								    // и отправим его по этому пути
								    controller_MoveToByCoords(player_mission_data[playerid][missionid][8],NPC_MOVE_PED_WALK);
							    }
							    else
							    {
							        // повторяем опять
							        controller_AddPathCoords(player_mission_data[playerid][missionid][8],2160.9036,-2247.5107,13.2927);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2165.6145,-2241.9050,13.2875);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.6187,-2237.7000,13.2937);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2151.6653,-2247.4675,13.2987);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2161.2144,-2257.1599,13.3008);
								    controller_AddPathCoords(player_mission_data[playerid][missionid][8],2152.7708,-2265.6494,13.3009);
								    // и отправим его по этому пути
								    controller_MoveToByCoords(player_mission_data[playerid][missionid][8],NPC_MOVE_PED_WALK);
							    }
						    }
						    // проверяем нет ли игрока перед NPC
						    for(new i = 1;i < 9;i++)
							{
								if(GetNPCState(player_mission_data[playerid][missionid][i]) == NPC_STATE_DEATH) continue; // пропускаем трупов
								    
							    if(IsPlayerInRangeOfPoint(player_mission_data[playerid][missionid][i],20.0,ftmp[0],ftmp[1],ftmp[2]))
							    {
							        GetNPCPos(player_mission_data[playerid][missionid][i],ftmp[3],ftmp[4],ftmp[5]);
							        // игрок около NPC, проверяем с какой стороны
							        ftmp[2] = atan2((ftmp[3] - ftmp[0]),(ftmp[4] - ftmp[1]));
							        ftmp[5] = GetNPCFacingAngle(player_mission_data[playerid][missionid][i]);
									if(ftmp[2] < 0.0) ftmp[2] += 360.0;
							        // если в обасти перед NPC
							        if((ftmp[2] > (ftmp[5] - 45.0)) &&(ftmp[2] < (ftmp[5] + 45.0)))
							        {
							            // объявляем тревогу
							            player_mission_data[playerid][missionid][9] = 1;
							            // останавливаем болтунов
							            ClearAnimations(player_mission_data[playerid][missionid][4]);
							            ClearAnimations(player_mission_data[playerid][missionid][5]);
							            // пусть NPC крикнет
										SetPlayerChatBubble(player_mission_data[playerid][missionid][i],"Здесь посторонний!",0xFF0000FF,50.0,10000);

							            SendClientMessage(playerid,0x00FF00FF,"Черт, меня заметили!");
							            break;
							        }
							    }
							}
					    }
					}
				}
			}
		}
		case 2: // Case delivery
		{
		    // определяем текущий этап катсцены
			switch(players[playerid][current_step])
			{
				case 0: // начало миссии
				{
		    		player_mission_data[playerid][missionid][0] = CreatePickup(1210, 23, 2788.7070, -1417.5576, 16.2500);
					player_mission_data[playerid][missionid][1] = CreateVehicle(468, 2792.5432, -1415.6924, 15.9153, 270.0, 0, 0, -1); // sanchez
					SetPlayerCheckpoint(playerid,2788.7070, -1417.5576, 26.2500,0.0);

					// устанавливаем данные
					players[playerid][current_step]++;
					// ожидаем следующий шаг
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",1000,1,"d",playerid);
				}
				case 1: // ожидаем пока игрок поднимет кейс
				{
				    if(IsPlayerInRangeOfPoint(playerid,5.0,2788.7070, -1417.5576, 16.2500))
				    {
				        KillTimer(players[playerid][timer]);
				        // кейс поднят, начинаем видео вставку
				        DestroyPickup(player_mission_data[playerid][missionid][0]);
				        DisablePlayerCheckpoint(playerid);
						TogglePlayerControllable(playerid,false);
						TextDrawShowForPlayer(playerid,Letterbox[0]);
						TextDrawShowForPlayer(playerid,Letterbox[1]);
						PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
						SetPlayerAttachedObject(playerid, 1, 1210, 5, 0.2989, 0.0939, 0.0270, -15.3, -108.0);
						InterpolateCameraPos(playerid, 2788.7070, -1417.5576, 16.2500, 2805.2500, -1419.2341, 19.2500, 2000, CAMERA_MOVE);
						InterpolateCameraLookAt(playerid, 2823.1003, -1418.5031, 16.2562, 2823.1003, -1418.5031, 16.2562, 2000, CAMERA_MOVE);
						SendClientMessage(playerid,0x00FF00FF,"Слышу гостей...");
						
						// устанавливаем данные
						players[playerid][current_step]++;
						// ожидаем следующий шаг
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",5000,0,"d",playerid);
				    }
				}
				case 2: // конец видео вставки, запускаем погоню
				{
				    for(new i = 2;i < 6;i++)
				    {
					    player_mission_data[playerid][missionid][i] = FindLastFreeSlot();
					    format(strtmp,sizeof(strtmp),"MissionNPC_%d",player_mission_data[playerid][missionid][i]);
					    CreateNPC(player_mission_data[playerid][missionid][i],strtmp);
					    SetSpawnInfo(player_mission_data[playerid][missionid][i],0,126,0.0,0.0,0.0,0.0,0,0,0,0,0,0);
					    SpawnNPC(player_mission_data[playerid][missionid][i]);
				    }
				    player_mission_data[playerid][missionid][6] = CreateVehicle(507, 2834.1208,-1483.2860,11.9153,88.9492, 0, 0, -1);
				    PutNPCInVehicle(player_mission_data[playerid][missionid][2],player_mission_data[playerid][missionid][6],0);
				    SetNPCPos(player_mission_data[playerid][missionid][2],2834.1208,-1483.2860,11.9153);
				    SetNPCFacingAngle(player_mission_data[playerid][missionid][2],88.9492);
				    for(new i = 3;i < 6;i++)
				    {
					    PutNPCInVehicle(player_mission_data[playerid][missionid][i],player_mission_data[playerid][missionid][6],(i - 2));
					    SetNPCWeapon(player_mission_data[playerid][missionid][i],29);
					}

					SetPlayerCheckpoint(playerid,1459.1420,-1264.5731,13.3906,4.0);
				    SetCameraBehindPlayer(playerid);
					TextDrawHideForPlayer(playerid,Letterbox[0]);
					TextDrawHideForPlayer(playerid,Letterbox[1]);
					TogglePlayerControllable(playerid,true);
					
					// устанавливаем данные
					players[playerid][current_step]++;
					// ожидаем следующий шаг
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",1000,1,"d",playerid);
				}
				case 3: // погоня, боты движутся за игроком, если игрок доходит до точки назначения - миссия пройдена
				{
				    if(IsPlayerInRangeOfPoint(playerid,5.0,1459.1420,-1264.5731,13.3906))
				    {
				        KillTimer(players[playerid][timer]);
				        // игрок достиг точки назначения, миссия пройдена
				        DestroyVehicle(player_mission_data[playerid][missionid][1]);
					    for(new i = 2;i < 6;i++)
					    {
						    DestroyNPC(player_mission_data[playerid][missionid][i]);
					    }
					    DestroyVehicle(player_mission_data[playerid][missionid][6]);
				        GameTextForPlayer(playerid, "~g~~h~Mission Passed!", 3000, 3);
						PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
						GivePlayerMoney(playerid, 5000);
						DisablePlayerCheckpoint(playerid);
						RemovePlayerAttachedObject(playerid, 1);
				        
						// устанавливаем данные
						players[playerid][current_mission] = -1;
						players[playerid][current_step] = 0;
						players[playerid][iscutscene] = 0;
						players[playerid][progress][missionid] = 1;
						players[playerid][timer] = -1;
				    }
				    else
				    {
				        // игрок не на точке назничения, преследуем его
				        new npcid = player_mission_data[playerid][missionid][2];
				        GetPlayerPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
				        // даём задачу ехать по путям к игроку
				        controller_DriveToByNodes(npcid,ftmp[0],ftmp[1],ftmp[2],40.0,20);
						// определяем расстояние от ботов до игрока (при достаточном приближении боты будут стрелять)
						GetNPCPos(npcid,ftmp[3],ftmp[4],ftmp[5]);
						ftmp[0] -= ftmp[3];
						ftmp[1] -= ftmp[4];
						ftmp[2] -= ftmp[5];
						new Float:d = floatsqroot((ftmp[0]*ftmp[0]) + (ftmp[1]*ftmp[1]) + (ftmp[2]*ftmp[2]));
						if(d < 50.0)
						{
						    // боты ближе 50 метров к игроку, стреляем
						    for(new i = 3;i < 6;i++)
							{
							    SetNPCCameraFrontVector(player_mission_data[playerid][missionid][i],ftmp[0],ftmp[1],ftmp[2]);
								SetNPCCameraPos(player_mission_data[playerid][missionid][i],ftmp[3],ftmp[4],ftmp[5]);
								SetNPCCameraMode(player_mission_data[playerid][missionid][i],55);
							    SetNPCPassangerDriveBy(player_mission_data[playerid][missionid][i],1);
							    SetNPCKeys(player_mission_data[playerid][missionid][i],0,0,KEY_FIRE);
							}
						}
						else
						{
						    // боты дальше 50 метров от игрока,прекращаем стрельбу
						    for(new i = 3;i < 6;i++) SetNPCKeys(player_mission_data[playerid][missionid][i],0,0,0);
						}
				    }
				}
			}
		}
	}
	return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
	if(players[playerid][current_mission] != -1)
	{
	    GameTextForPlayer(playerid, "~g~~h~Mission Failed!", 3000, 3);
	    Mission_Clear(playerid);
	}
	return 1;
}

// --------------------------------------------------
// stocks
// --------------------------------------------------

stock Mission_Start(missionid,playerid)
{
	// проверяем уже запущенную миссию
	if(players[playerid][current_mission] != -1)
	{
		// игрок не может начать миссию
		SendClientMessage(playerid,MESSAGES_COLOR,"У тебя и так есть дела.");
		return 0;
	}
	// проверяем требования миссии
	if(missions[missionid][request] != -1)
	{
		new requestid = missions[missionid][request];
		if(!players[playerid][progress][requestid])
		{
			// игрок не может начать миссию
			SendClientMessage(playerid,MESSAGES_COLOR,"На данный момент для тебя заданий нет.");
			return 0;
		}
	}
	// устанавливаем данные миссии
	players[playerid][current_mission] = missionid;
	players[playerid][current_step] = 0;
	players[playerid][iscutscene] = 1;
	// показываем название
	GameTextForPlayer(playerid,missions[missionid][name],3000,1);
	// начинаем показывать катсцену
	players[playerid][timer] = SetTimerEx("Mission_CutsceneStep",0,0,"d",playerid);
	return 1;
}

stock Mission_Clear(playerid)
{
    // удаляем данные миссии
	new missionid = players[playerid][current_mission];
	switch(missionid)
	{
		case 0:
		{
		    switch(players[playerid][current_step])
			{
			    case 1:
			    {
					DisablePlayerCheckpoint(playerid);
			    }
			    case 2,3,4,5:
			    {
			        DestroyVehicle(player_mission_data[playerid][missionid][0]);
			        DestroyNPC(player_mission_data[playerid][missionid][1]);
			    }
			    case 6,7:
			    {
			        DestroyVehicle(player_mission_data[playerid][missionid][0]);
			        DestroyNPC(player_mission_data[playerid][missionid][1]);
			        DestroyNPC(player_mission_data[playerid][missionid][2]);
			        DestroyNPC(player_mission_data[playerid][missionid][3]);
			    }
			}
		}
		case 1:
		{
		    switch(players[playerid][current_step])
			{
			    case 1:
			    {
					DisablePlayerCheckpoint(playerid);
			    }
			    case 2:
			    {
			        DisablePlayerCheckpoint(playerid);
			        for(new i = 1;i < 9;i++) DestroyNPC(player_mission_data[playerid][missionid][i]);
			    }
			    case 3:
			    {
			        for(new i = 1;i < 9;i++) DestroyNPC(player_mission_data[playerid][missionid][i]);
			    }
			}
		}
	    case 2: // Case delivery
		{
			switch(players[playerid][current_step])
			{
				case 1:
				{
		    		DestroyPickup(player_mission_data[playerid][missionid][0]);
					DestroyVehicle(player_mission_data[playerid][missionid][1]);
					RemovePlayerMapIcon(playerid, 98);
				}
				case 2:
				{
				    DestroyVehicle(player_mission_data[playerid][missionid][1]);
				    RemovePlayerAttachedObject(playerid, 1);
				}
				case 3:
				{
				    DestroyVehicle(player_mission_data[playerid][missionid][1]);
				    RemovePlayerAttachedObject(playerid, 1);
				    for(new i = 2;i < 6;i++)
				    {
					    DestroyNPC(player_mission_data[playerid][missionid][i]);
				    }
				    DestroyVehicle(player_mission_data[playerid][missionid][6]);
				    DisablePlayerCheckpoint(playerid);
				}
			}
		}
	}
	if(players[playerid][timer] != 0) KillTimer(players[playerid][timer]);
	players[playerid][current_mission] = -1;
	players[playerid][current_step] = -1;
}

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
