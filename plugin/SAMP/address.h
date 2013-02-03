/*
 *	Copyright (C) 2012 Alex009
 *	License read in license.txt
 *	Description: SA-MP functions addresses
 */

#pragma once

#define SAMP_VERSION	"0.3x R1"

#ifdef WIN32

#define CLIENT_CONNECT						0x4657C0
#define CLIENT_DISCONNECT					0x465B40
#define SPAWN_FOR_WORLD						0x481C30
#define KILL_FOR_WORLD						0x47F820
#define ADDR_THREAD_START					0x488887
#define ADDR_THREAD_END						0x4888A3
#define GET_CONFIG_VAR						0x486B20
#define CONFIG_CLASS						0x4F13C8

#else

#define CLIENT_CONNECT						0x80C4480
#define CLIENT_DISCONNECT					0x80C48B0
#define SPAWN_FOR_WORLD						0x80C12E0
#define KILL_FOR_WORLD						0x80BF7C0
#define ADDR_THREAD_START					0x80B10AC
#define ADDR_THREAD_END						0x80B10BE
#define GET_CONFIG_VAR						0x80A8120
#define CONFIG_CLASS						0x8192A24

#endif
