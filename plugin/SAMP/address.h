/*
 *	Copyright (C) 2012 Alex009
 *	License read in license.txt
 *	Description: SA-MP functions addresses
 */

#pragma once

#ifdef WIN32

#define CLIENT_CONNECT						0x4668B0
#define CLIENT_DISCONNECT					0x466C30
#define SPAWN_FOR_WORLD						0x4829A0
#define KILL_FOR_WORLD						0x4804C0
#define ADDR_THREAD_START					0x487087
#define ADDR_THREAD_END						0x4870A3

#else

#define CLIENT_CONNECT						0x80CE9D0
#define CLIENT_DISCONNECT					0x80CEE00
#define SPAWN_FOR_WORLD						0x80CB970
#define KILL_FOR_WORLD						0x80C9E60
#define ADDR_THREAD_START					0x80A8FBC
#define ADDR_THREAD_END						0x80A8FCE

#endif