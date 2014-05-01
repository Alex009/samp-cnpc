/*
 *	Copyright (C) 2012 Alex009
 *	License read in license.txt
 *	Description: SA-MP functions addresses
 */

#pragma once

#define SAMP_VERSION	"0.3z R2"

#ifdef WIN32

#define CLIENT_CONNECT			0x465D90
#define CLIENT_DISCONNECT		0x466110
#define SPAWN_FOR_WORLD			0x482E10
#define KILL_FOR_WORLD			0x480820
#define STREAM_OUT				0x47F880
#define ADDR_THREAD_START		0x4879D9
#define ADDR_THREAD_END			0x4879F5
#define GET_CONFIG_VAR			0x485B70
#define CONFIG_CLASS			0x4F2370

#else

#define CLIENT_CONNECT			0x80CD970
#define CLIENT_DISCONNECT		0x80CDDA0
#define SPAWN_FOR_WORLD			0x80CA690
#define KILL_FOR_WORLD			0x80C8B70
#define STREAM_OUT				0x80C8910
#define ADDR_THREAD_START		0x80A83EB
#define ADDR_THREAD_END			0x80A83FD
#define GET_CONFIG_VAR			0x809EE80
#define CONFIG_CLASS			0x819B8E4

#endif
