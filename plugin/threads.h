/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: threads of npcs
 */

#include "os.h"

THREAD_START(NPCSyncThread);
THREAD_START(NPCDamageThread);
THREAD_START(ReloadThread);