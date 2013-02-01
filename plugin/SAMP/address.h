/*
 *	Copyright (C) 2012 Alex009
 *	License read in license.txt
 *	Description: SA-MP functions addresses
 */

#pragma once

#define SAMP_VERSION	"0.3e R2"

#ifdef WIN32

#define CLIENT_CONNECT						0x4668B0
#define CLIENT_DISCONNECT					0x466C30
#define SPAWN_FOR_WORLD						0x4829A0
#define KILL_FOR_WORLD						0x4804C0
#define ADDR_THREAD_START					0x487087
#define ADDR_THREAD_END						0x4870A3
#define GET_CONFIG_VAR						0x485320
#define CONFIG_CLASS						0x4F2388

#else

#define CLIENT_CONNECT						0x80CE9D0
#define CLIENT_DISCONNECT					0x80CEE00
#define SPAWN_FOR_WORLD						0x80CB970
#define KILL_FOR_WORLD						0x80C9E60
#define ADDR_THREAD_START					0x80A8FBC
#define ADDR_THREAD_END						0x80A8FCE
#define GET_CONFIG_VAR						0x80A0030
#define CONFIG_CLASS						0x819B5A4

#endif
