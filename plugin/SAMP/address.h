/*
 *	Copyright (C) 2012 Alex009
 *	License read in license.txt
 *	Description: SA-MP functions addresses
 */

#pragma once

#define SAMP_VERSION	"0.3x R1-2"

#ifdef WIN32

#define CLIENT_CONNECT						0x465AA0
#define CLIENT_DISCONNECT					0x465E20
#define SPAWN_FOR_WORLD						0x481EB0
#define KILL_FOR_WORLD						0x47FAA0
#define STREAM_OUT							0x47EB30
#define ADDR_THREAD_START					0x488B17
#define ADDR_THREAD_END						0x488B33
#define GET_CONFIG_VAR						0x486DB0
#define CONFIG_CLASS						0x4F23C8

#else

#define CLIENT_CONNECT						0x80C4840
#define CLIENT_DISCONNECT					0x80C4C70
#define SPAWN_FOR_WORLD						0x80C16A0
#define KILL_FOR_WORLD						0x80BFB80
#define STREAM_OUT							0x80BF560
#define ADDR_THREAD_START					0x80B145C
#define ADDR_THREAD_END						0x80B146E
#define GET_CONFIG_VAR						0x80A84D0
#define CONFIG_CLASS						0x8192E84

#endif
