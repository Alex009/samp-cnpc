/*
 *	Copyright (C) 2013 Alex009
 *	License read in license.txt
 */

#include "os.h"

bool Unlock(void *address, int len);

void InitDemoMode()
{
#ifdef WIN32
	Unlock((void*)0x45401E,2);
	*(unsigned char*)(0x45401E) = 0x90;
	*(unsigned char*)(0x45401F) = 0x90;
#else
	Unlock((void*)0x8070141,2);
	*(unsigned char*)(0x8070141) = 0x90;
	*(unsigned char*)(0x8070142) = 0x90;
#endif
}