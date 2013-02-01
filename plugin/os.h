/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 *	Description: operating system specials
 */

#define POINTER_TO_MEMBER(m,n,o) temp = n, m = *(o *)&temp
#define _ ,

#ifdef WIN32
	#include "windows.h"
	#define THREAD_IDENTIFY(a) HANDLE a
	#define THREAD_RESULT(a) DWORD a
	#define THREAD_START(a) DWORD WINAPI a(LPVOID arg)
	#define THREAD_END return 1;
	#define CREATE_THREAD(a,b,c,d) a = CreateThread(NULL,0,b,d,0,&c)
	#define CALL_NOARGS(a,b) (((FakeClass*)a)->*b)()
	#define CALL_ARGS(a,b,c) (((FakeClass*)a)->*b)(c)
	#define UTILS();
	#define SLEEP(a) Sleep(a)

	#define MUTEX_IDENTIFY(a) HANDLE a
	#define CREATE_MUTEX(a) a = CreateMutex(NULL, false, NULL)
	#define MUTEX_ENTER(a) WaitForSingleObject(a, INFINITE)
	#define MUTEX_EXIT(a) ReleaseMutex(a)
#else
	#include "pthread.h"
	#include "unistd.h"
	#include <sys/mman.h>
	#define THREAD_IDENTIFY(a) pthread_t a
	#define THREAD_RESULT(a) int a
	#define THREAD_START(a) void* a(void* arg)
	#define THREAD_END 
	#define CREATE_THREAD(a,b,c,d) c = pthread_create(&a,0,b,d);
	#define CALL_NOARGS(a,b) b((FakeClass*)a)
	#define CALL_ARGS(a,b,c) b((FakeClass*)a,c)
	#define UTILS(); TickCountLoad();
	#define SLEEP(a) usleep((a) * 1000)

	#define MUTEX_IDENTIFY(a) pthread_mutex_t a
	#define CREATE_MUTEX(a) pthread_mutex_init(&a, NULL)
	#define MUTEX_ENTER(a) pthread_mutex_lock(&a)
	#define MUTEX_EXIT(a) pthread_mutex_unlock(&a)
	// types
	#define DWORD	unsigned long
	#define BOOL	int
	#define BYTE	unsigned char
	#define WORD	unsigned short
	#define FLOAT	float
	int GetTickCount();
	void TickCountLoad();
#endif
