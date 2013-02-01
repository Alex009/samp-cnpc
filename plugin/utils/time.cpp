/*
 *	Copyright (C) 2012 Alex009 (Алексей Михайлов)
 *	License read in license.txt
 *	Description: Time functions (microtime() from PHP 5.0)
 *	Last edit at 26.09.12
 */

#ifdef WIN32
	#include <windows.h>
	#include <WinBase.h>
	#include <time.h>

struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};


struct itimerval {
	struct timeval it_interval;	/* next value */
	struct timeval it_value;	/* current value */
};

#ifndef timespec
struct timespec
{
	time_t   tv_sec;   /* seconds */
	long     tv_nsec;  /* nanoseconds */
};
#endif

#else
	#include <sys/time.h>
#endif

#define MICRO_IN_SEC 1000000.00
#define SEC_IN_MIN 60

/*
	value microtime_now()
{       
        #if _WINDOWS
                FILETIME ft; GetSystemTimeAsFileTime(&ft);
                unsigned long long tt = ft.dwHighDateTime;
                tt <<=32;
                tt |= ft.dwLowDateTime;
                tt /=10;
                tt -= 11644473600000000ULL;
                double r = tt / 1000.0;
        #else
                struct timeval tv; gettimeofday(&tv, NULL);
                double r = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
        #endif
        
        return alloc_float(r);
}
*/

#ifdef WIN32

int getfilesystemtime(struct timeval *time_Info) 
{
	FILETIME ft;
	__int64 ff;

    GetSystemTimeAsFileTime(&ft);   /* 100 ns blocks since 01-Jan-1641 */
                                    /* resolution seems to be 0.01 sec */ 
    ff = *(__int64*)(&ft);
    time_Info->tv_sec = (int)(ff/(__int64)10000000 - (__int64)11644473600);
    time_Info->tv_usec = (int)(ff % 10000000)/10;
    return 0;
}

int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info)
{
	static struct timeval starttime = {0, 0};
	static __int64 lasttime = 0;
	static __int64 freq = 0;
	__int64 timer;
	LARGE_INTEGER li;
	BOOL b;
	double dt;

	/* Get the time, if they want it */
	if (time_Info != NULL) {
		if (starttime.tv_sec == 0) {
            b = QueryPerformanceFrequency(&li);
            if (!b) {
                starttime.tv_sec = -1;
            }
            else {
                freq = li.QuadPart;
                b = QueryPerformanceCounter(&li);
                if (!b) {
                    starttime.tv_sec = -1;
                }
                else {
                    getfilesystemtime(&starttime);
                    timer = li.QuadPart;
                    dt = (double)timer/freq;
                    starttime.tv_usec -= (int)((dt-(int)dt)*1000000);
                    if (starttime.tv_usec < 0) {
                        starttime.tv_usec += 1000000;
                        --starttime.tv_sec;
                    }
                    starttime.tv_sec -= (int)dt;
                }
            }
        }
        if (starttime.tv_sec > 0) {
            b = QueryPerformanceCounter(&li);
            if (!b) {
                starttime.tv_sec = -1;
            }
            else {
                timer = li.QuadPart;
                if (timer < lasttime) {
                    getfilesystemtime(time_Info);
                    dt = (double)timer/freq;
                    starttime = *time_Info;
                    starttime.tv_usec -= (int)((dt-(int)dt)*1000000);
                    if (starttime.tv_usec < 0) {
                        starttime.tv_usec += 1000000;
                        --starttime.tv_sec;
                    }
                    starttime.tv_sec -= (int)dt;
                }
                else {
                    lasttime = timer;
                    dt = (double)timer/freq;
                    time_Info->tv_sec = starttime.tv_sec + (int)dt;
                    time_Info->tv_usec = starttime.tv_usec + (int)((dt-(int)dt)*1000000);
                    if (time_Info->tv_usec > 1000000) {
                        time_Info->tv_usec -= 1000000;
                        ++time_Info->tv_sec;
                    }
                }
            }
        }
        if (starttime.tv_sec < 0) {
            getfilesystemtime(time_Info);
        }

	}
	/* Get the timezone, if they want it */
	if (timezone_Info != NULL) {
		_tzset();
		timezone_Info->tz_minuteswest = _timezone;
		timezone_Info->tz_dsttime = _daylight;
	}
	/* And return */
	return 0;
}

#endif

double microtime()
{
	struct timeval tp = {0};
	struct timezone tz = {0};

	if (gettimeofday(&tp, &tz)) return 0.0;

	return (double)(tp.tv_sec + tp.tv_usec / MICRO_IN_SEC);
}

void microtime(long* sec,long* microsec)
{
	struct timeval tp = {0};
	struct timezone tz = {0};

	if (gettimeofday(&tp, &tz)) return;

	*sec = tp.tv_sec;
	*microsec = tp.tv_usec;
	return;
}
