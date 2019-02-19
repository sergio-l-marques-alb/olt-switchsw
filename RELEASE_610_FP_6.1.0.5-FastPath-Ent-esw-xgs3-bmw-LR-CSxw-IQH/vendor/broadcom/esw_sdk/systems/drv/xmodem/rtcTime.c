/* $Id: rtcTime.c,v 1.1 2011/04/18 17:11:09 mruas Exp $
 * Port to IDT RC32364 BSP 06/04/02, James Dougherty (jfd@broadcom.com)
 *
 * rtcTime.c - Unix type time() and ctime() routines
 */

/* This stub created by Matt Michaelis, NCAR, but you have to make it work */

/*
modification history
--------------------
01a,28jul93,mcm  Whacked this together from rtc stuff I generated a few months
                 ago.
*/

/* 
DESCRIPTION
This file contains two Unix type time routines that need to be interfaced to
a real time clock.  I included a prototype rtcUpdateTime routine which will 
need to be filled in or replaced.

FIXME WORK NEEDED
The rtcUpdateTime routine.
*/

#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef VXWORKS
#include <sysLib.h>
#endif
#ifdef MOUSSE
#include <vxLib.h>
#include <mousse.h>
#endif

#ifdef MBZ
#include <mbz.h>
#endif

#include "vxWorks.h"
#include "types.h"

#define BASE_YEAR 1980	/* This is for clocks that have two digit years */

#if CPU == RC32364
IMPORT STATUS ds1553_tod_get(int *year,			/* 1980-2079 */
			     int *month,		/* 01-12 */
			     int *day,		        /* 01-31 */
			     int *hour,			/* 00-23 */
			     int *minute,		/* 00-59 */
			     int *second);		/* 00-59 */
#elif (defined(BMW) || defined(MBZ))

/* standard TOD get in sysLib */

#elif (CPU == PPC603)
/* ST M48T59Y */
STATUS m48_tod_get(int *year,		/* 1980-2079 */
		   int *month,		/* 01-12 */
		   int *day,		/* 01-31 */
		   int *hour,		/* 00-23 */
		   int *minute,		/* 00-59 */
		   int *second);	/* 00-59 */

#else
#undef BASE_YEAR 
#define BASE_YEAR 1990	/* This is for clocks that have two digit years */
#endif
LOCAL int rtcYear;
LOCAL int rtcMonth;
LOCAL int rtcDayOfYear;
LOCAL int rtcDayOfMonth;
LOCAL int rtcDayOfWeek;
LOCAL int rtcHour;
LOCAL int rtcMinute;
LOCAL int rtcSecond;
LOCAL int rtc100sSec;
LOCAL int rtcPm;


/*
 * sysSyncFs()
 * Synch local file system on write.
 *
 */
IMPORT void flashFsSync(void);
void sysSynchFs(void)
{
    /* Synch flash filesystem */
    flashFsSync();
}



/*******************************************************************************
*
* rtcUpdateTime - updates time and date from rtc.
*
*/
void
rtcUpdateTime(void)
{
    struct timespec	ltv;

    clock_gettime(CLOCK_REALTIME, &ltv);

    rtc100sSec = (ltv.tv_sec * 1000000 + ltv.tv_nsec / 1000);
#if CPU == RC32364
    ds1553_tod_get(&rtcYear,		 /* 1980-2079 */
		   &rtcMonth,		/* 01-12 */
		   &rtcDayOfMonth,	        /* 01-31 */
		   &rtcHour,		/* 00-23 */
		   &rtcMinute,		/* 00-59 */
		   &rtcSecond);		/* 00-59 */
    rtcPm = rtcHour > 12 ? 1 : 0;
#elif (defined(BMW) || defined(MBZ))
    
    sysTodGet (&rtcYear,	        /* 1980-2079 */
	       &rtcMonth,		/* 01-12 */
	       &rtcDayOfMonth,          /* 01-31 */
	       &rtcHour,		/* 00-23 */
	       &rtcMinute,		/* 00-59 */
	       &rtcSecond);		/* 00-59 */
    rtcPm = rtcHour > 12 ? 1 : 0;
#elif (CPU == PPC603)
    m48_tod_get(&rtcYear,		/* 1980-2079 */
		&rtcMonth,		/* 01-12 */
		&rtcDayOfMonth,	        /* 01-31 */
		&rtcHour,		/* 00-23 */
		&rtcMinute,		/* 00-59 */
		&rtcSecond);		/* 00-59 */
    rtcPm = rtcHour > 12 ? 1 : 0;
#else		
    rtcYear = fixme;		
    rtcMonth = fixme;
    rtcDayOfYear = fixme;
    rtcDayOfMonth = fixme;
    rtcDayOfWeek = fixme;
    rtcHour = fixme;
    rtcMinute = fixme;
    rtcSecond = fixme;
    rtc100sSec = fixme;
    rtcPm = fixme;
#endif
    /* Common */
    rtcDayOfWeek = 0;

}

/*******************************************************************************
*
* rtcUnixTime - get the time and date. 
*
* PARAMTERS
* a pointer to time_t location, may be null
*
* RETURNS
* The number of seconds since 1/1/1970 00:00:00 GMT
*/

/* Use this if your clock does not return dayOfYear */
/* short monthToDay[] =
	{31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365}; */

time_t rtcUnixTime (tloc)
time_t *tloc;
    {
    time_t tsecs;

    tsecs = 0;
    rtcUpdateTime();		/* Get the current date and time */

    tsecs += (rtcYear + BASE_YEAR -1970) * (365 * 24 * 60 * 60);
    tsecs += ((rtcYear + BASE_YEAR - 1)/4 - 1970/4) * (24 * 60 * 60);
    tsecs += (rtcDayOfYear -1) * 24 * 60 * 60;
    tsecs += rtcHour * 60 * 60;
    tsecs += rtcMinute * 60;
    tsecs += rtcSecond;
    if ((rtcPm) && (rtcHour != 12))
        tsecs += 12 * 60 * 60;
    if (tloc != NULL) *tloc = tsecs;
    return tsecs;
    }


/*******************************************************************************
*
* rtcConvUnixTime - convert the time and date from Unixtime to year, month, etc.
*
* PARAMTERS
* a time_t value
*
* RETURNS
* A pointer to a string containing the Date/Time in ASCII
*/

char *monthTable [12] =
    {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

char *dayOfWeek [7] =
    {"Thu", "Fri", "Sat", "Sun", "Mon", "Tue", "Wed"};

short monthToDay[] =
	{31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
short monthToDayLeap[] =
	{31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

/* short dayToMonth[] =
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30 31}; */

char *rtcConvUnixTime (timInSec)
time_t timInSec;
    {
    int year, month, day, date, num_days, hour, minute, sec;
    static char asciiTime[26];

    num_days = timInSec / (24 * 60 * 60);
    /* 1/1/1970 was a Thursday */
    day = num_days % 7;
    year = 1970 + timInSec / (365* 24 * 60 * 60);
    timInSec -= (year - 1970) * (365 * 24 * 60 * 60);
    /* Remove leap year seconds from all but current year */
    timInSec -= ((year - 1)/4 - 1970/4) * (24 * 60 * 60);

    date = timInSec / (24 * 60 * 60);
    timInSec -= date * (24 * 60 * 60);
    date++;			/* Change date origin from 0 to 1 */
    month = 0;
    /* Check for not leap year */
    if (year % 4)
	{
	while(date - monthToDay[month] > 0)
 	    month++;
	if (month) date -= monthToDay[month - 1];
	}
    else
    /* Is a leap year */
	{
	while(date - monthToDayLeap[month] > 0)
	    month++;
	if (month) date -= monthToDayLeap[month - 1];
	}

    /* Now we are down to time of day */
    hour = timInSec / (60 * 60);
    timInSec -= hour * (60 * 60);
    minute = timInSec / 60;
    timInSec -= minute * 60;
    sec = timInSec;

    /* String format example: 
     *                       Sun Sep 16 01:03:52 1973\n\0
     */
    sprintf(asciiTime, "%s %s %02d %02d:%02d:%02d %04d\n",
	    dayOfWeek[day], monthTable[month], date, hour, minute, sec, year);

    return &asciiTime[0];
    }
