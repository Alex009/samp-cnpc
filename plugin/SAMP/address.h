/*
 *	Copyright (C) 2012 Alex009
 *	License read in license.txt
 *	Description: SA-MP functions addresses
 */

#pragma once

#define SAMP_VERSION	"0.3z R1"

#ifdef WIN32

#define CLIENT_CONNECT			0x465C10
#define CLIENT_DISCONNECT		0x465F90
#define SPAWN_FOR_WORLD			0x482CC0
#define KILL_FOR_WORLD			0x4806D0
#define STREAM_OUT				0x47F730
#define ADDR_THREAD_START		0x4877E5
#define ADDR_THREAD_END			0x487801
#define GET_CONFIG_VAR			0x4859F0
#define CONFIG_CLASS			0x4F2350

#else

#define CLIENT_CONNECT			0x80CD740
#define CLIENT_DISCONNECT		0x80CDB70
#define SPAWN_FOR_WORLD			0x80CA460
#define KILL_FOR_WORLD			0x80C8940
#define STREAM_OUT				0x80C86E0
#define ADDR_THREAD_START		0x80A81BB
#define ADDR_THREAD_END			0x80A81CD
#define GET_CONFIG_VAR			0x809ED10
#define CONFIG_CLASS			0x819A564

#endif
