/*
 *
 * Copyright (C) 1992-2006 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#include "sr_conf.h"

#include <stdio.h>

#include <malloc.h>	/* for malloc */

#include <stdlib.h>	/* for malloc */

#include <string.h>

#include <sys/types.h>





#include "sr_type.h"


#include "sr_time.h"
#include "sr_snmp.h"
#include "sr_bdata.h"




#include "tmq.h" /* For TvpToTimeOfDay() prototype */

#include "diag.h"
SR_FILENAME

/*
 * basetime will be measured in 100ths of seconds.
 * Don't export it, since it has no non-local meaning anyway
 * (it wraps every 1.36 years).
 */
static SR_UINT32 basetime;
/*
 * agtbasetime will be measured in 100ths of seconds.
 * Don't export it, since it has no non-local meaning anyway
 * (it wraps every 1.36 years).  It is used in AgtGetTimeNow()
 * which gets the amount of time the agent has been up.
 * This is useful where an invariant time base is needed.
 */
static SR_UINT32 agtbasetime;
/*
 * bootTOD is used to keep track of when this entity was
 * booted.  This is necessary when one needs to print
 * a date/time based on sysUptime (as in the log book functions);
 */
static SrTOD bootTOD;
/*
 * maBootTimeOffset is used by subagents to compute date/time
 * based on a sysUptime (which is based on the master agent boot time.
 * This will always be zero on the authoritive (master/monolithic) program.
 */
static SR_UINT32 maBootTimeOffset = 0;

/*
 * boot_counter is used to keep track of how many times a sysUpTime is
 * wrapped.
 */
static int boot_counter = 0;

/*
 * previous_time is used in seeting timer related funtions to verify
 * whether timer value is wrapped or not.
 */
SR_UINT32 previous_time = 0;

/* 
 * current_time is used to keep track of current sysUpTIme
 */
static SR_UINT32 current_time = 0;
/*

 * timerq_update is used to check whether a timer queue should be updated
 * or not after sysUpTime is wrapped.
 */
int timerq_update = 0; 

/*
 * wrap_flag is used to keep track of sysUpTime wrap.
 */
static int wrap_flag = FALSE;

/* 
 * bootData structire will be used to call a call_back function to
 * update agent/mgr configuration file with updated snmpEngineBoots
 * value when sysUpTIme is wrapped.
 */
BOOT_DATA *bootData = NULL;

#undef SR_USE_GETTIMEOFDAY
#undef SR_USE_CTICKS
#undef SR_USE_FTIME
#undef SR_USE_TICKGET
#undef SR_USE_TM_GET

#define SR_USE_GETTIMEOFDAY

/*
 *  Local prototypes & statics
 */
static SR_UINT32 GetAbsoluteTime(void);
static int dayspermonth[] = { -1,31,-1,31,30,31,30,31,31,30,31,30,31 };

long GetTimeZone(void);

#if (((((! ( defined(SR_USE_GETTIMEOFDAY) ) && ! ( defined(SR_USE_CTICKS) )) && ! ( defined(SR_USE_FTIME) )) && ! ( defined(SR_USE_TICKGET) )) && ! ( defined(SR_USE_TM_GET) )) && ! ( defined(SR_USE_VMSTIME) ))
/*
 * If none of the other options apply, our substitute gettimeofday()
 * will be in compat.c, so we'll just use gettimeofday()
 */
#define SR_USE_GETTIMEOFDAY
#endif	/* (((((! ( defined(SR_USE_GETTIMEOFDAY) ) && ! ( defined(SR_USE_CTICKS) )) && ! ( defined(SR_USE_FTIME) )) && ! ( defined(SR_USE_TICKGET) )) && ! ( defined(SR_USE_TM_GET) )) && ! ( defined(SR_USE_VMSTIME) )) */

#ifdef SR_USE_FTIME
static SR_UINT32
GetAbsoluteTime()
{
    struct _timeb t;

    _ftime(&t);
    return ((SR_UINT32) ((t.time * 100) + (t.millitm / 10)));
}
#endif /* SR_USE_FTIME */

#ifdef SR_USE_CTICKS
static SR_UINT32
GetAbsoluteTime()
{
    return (cticks * 100 / TPS);
}
#endif /* SR_USE_CTICKS */

#ifdef SR_USE_TICKGET
static SR_UINT32
GetAbsoluteTime()
{
    struct timeval  tv;

    SR_UINT32       ticks;
    int             clk_rate;

    ticks = tickGet();
    clk_rate = sysClkRateGet();
    tv.tv_sec = ticks / clk_rate;
    tv.tv_usec = (ticks % clk_rate) * (((SR_INT32)1000000) / clk_rate);

    return ((tv.tv_sec * 100) + (tv.tv_usec / 10000));
}
#endif /* SR_USE_TICKGET */

#ifdef SR_USE_TM_GET
static SR_UINT32
GetAbsoluteTime()
{
    unsigned long date, time, ticks;
    static unsigned long firstticks;

    tm_get(&date, &time, &ticks);

    ticks = (ticks * 100) / KC_TICKS2SEC;

    ticks += 100 * ConvertToCalendarTime(date >> 16, (date & 0xff00) >> 8,
                           date & 0xff,
                           time >> 16, (time & 0xff00) >> 8, time & 0xff);

    if(firstticks == 0) {
        firstticks = ticks;
    }

    return ticks - firstticks;
}
#endif /* SR_USE_TM_GET */

#ifdef SR_USE_GETTIMEOFDAY
static SR_UINT32
GetAbsoluteTime()
{
    struct timeval  tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);
    return ((tv.tv_sec * 100) + (tv.tv_usec / 10000));
}
#endif /* SR_USE_GETTIMEOFDAY */

#ifdef SR_USE_TIMER_ENVIRON
static SR_UINT32
GetAbsoluteTime()
{
    time_t secs;
    unsigned int ticks;
    int ticks_per_sec;

    _os_getime(&secs, &ticks);
    ticks_per_sec = ticks >> 16;
    ticks &= 0xFFFF;

    return(secs * 100 + ((ticks * 100 ) / ticks_per_sec));
}
#endif /* SR_USE_TIMER_ENVIRON */

#ifdef SR_USE_OSE_TIME
static SR_UINT32
GetAbsoluteTime()
{
    struct TimePair curtime;
    SR_UINT32 s;

    rtc_get_time(&curtime);
    s = (curtime.seconds * 100) + (curtime.micros / 10000);
    return s;
}
#endif /* SR_USE_OSE_TIME */

#ifdef SR_USE_CHORUS_TIME
static SR_UINT32
GetAbsoluteTime()
{
    KnTimeVal curtime;
    SR_UINT32 s;

    univTime(&curtime);
    s = (curtime.tmSec * 100) + curtime.tmNSec / (10 * 1000 * 1000);
    return s;
}
#endif /* SR_USE_CHORUS_TIME */

#ifdef SR_USE_NUCLEUS_CTICKS
static SR_UINT32
GetAbsoluteTime()
{
    SR_UINT32 s;

    s = (NU_Retrieve_Clock() * 100) / TICKS_PER_SECOND; 
    return s;
}
#endif /* SR_USE_NUCLEUS_CTICKS */

#ifdef SR_USE_WINCE_CTICKS
static SR_UINT32
GetAbsoluteTime()
{
    return (SR_UINT32) (GetTickCount() / 10);
}
#endif /* SR_USE_WINCE_CTICKS */

#ifdef SR_USE_VMSTIME
static SR_UINT32
GetAbsoluteTime()
{
    __int64     absTime;

    absTime = 0;
    sys$gettim( &absTime );

    return absTime / 100000;
}
#endif /* SR_USE_VMSTIME */

#ifdef SR_USE_MQX_TIME
static SR_UINT32
GetAbsoluteTime()
{
    TIME_STRUCT t;

    _time_get(&t);
     return ((SR_UINT32) ((t.SECONDS*100) + (t.MILLISECONDS / 10)));
}
#endif /* SR_USE_MQX_TIME */

SR_UINT32
GetTimeNow()
{
    SR_UINT32 time_val;

    time_val = GetAbsoluteTime();
    previous_time = current_time;
    if (time_val < basetime) {
        if (wrap_flag == FALSE) {
            wrap_flag = TRUE;
        }
    }
    else {
        /* 
         * clock has been wrapped
         */
        if (wrap_flag == TRUE) {
            wrap_flag = FALSE;
            boot_counter++;
            /*
             * call_back function to update a configuration file is exist
             */
            if (bootData != NULL) {
                (*bootData->fptr)(bootData);
            }
	    timerq_update = 1;
	}
    }
    current_time = time_val - basetime;
    return (time_val - basetime);
}

/*
 * GetTimeNowWrap
 *
 * return sysUpTime. If sysUpTime has been wrapped set "*wrap" to 1
 */
SR_UINT32
GetTimeNowWrap(int *wrap)
{
    SR_UINT32 time_val;

    *wrap = 0;
    time_val = GetAbsoluteTime();
    previous_time = current_time;
    if (time_val < basetime) {
	if (wrap_flag == FALSE) {
            wrap_flag = TRUE;
        }
    }
    else {
        /* 
         * clock has been wrapped
         */
        if (wrap_flag == TRUE) {
            wrap_flag = FALSE;
            *wrap = 1;
            boot_counter++;
            /*
             * call_back function to update a configuration file is exist
             */
            if (bootData != NULL) {
	        (*bootData->fptr)(bootData);
            }
	    timerq_update = 1;
	}
    }
    current_time = time_val - basetime;
    return (time_val - basetime);
}

/*
 *  Return amount of time agent has been up.  This is distinct
 *  from GetTimeNow (which returns sysUptime), for applications
 *  (such as host resources) that need a invariant timing base.
 */

SR_UINT32
GetSubagentTimeNow(void)
{
    SR_UINT32 time_val;

    time_val = GetAbsoluteTime();
    previous_time = current_time;
    if (time_val < agtbasetime) {
	if (wrap_flag == FALSE) {
            wrap_flag = TRUE;
        }
    }
    else {
        /* 
         * clock has been wrapped
         */
        if (wrap_flag == TRUE) {
            wrap_flag = FALSE;
            boot_counter++;
            /*
             * call_back function to update a configuration file is exist
             */
            if (bootData != NULL) {
	        (*bootData->fptr)(bootData);
            }
	    timerq_update = 1;
	}
    }
    current_time = time_val - agtbasetime;
    return (time_val - agtbasetime);
}

/*
 * GetSubagentTimeNowWrap
 *
 * return agent uptime. If agent uptime has been wrapped set "*wrap" to 1
 */
SR_UINT32
GetSubagentTimeNowWrap(int *wrap)
{
    SR_UINT32 time_val;

    *wrap = 0;
    time_val = GetAbsoluteTime();
    previous_time = current_time;
    if (time_val < agtbasetime) {
	if (wrap_flag == FALSE) {
            wrap_flag = TRUE;
        }
    }
    else {
        /* 
         * clock has been wrapped
         */
        if (wrap_flag == TRUE) {
            wrap_flag = FALSE;
            boot_counter++;
            /*
             * call_back function to update a configuration file is exist
             */
            if (bootData != NULL) {
	        (*bootData->fptr)(bootData);
            }
            *wrap = 1;
	    timerq_update = 1;
	}
    }
    current_time = time_val - agtbasetime;
    return (time_val - agtbasetime);
}

void
InitTimeNow()
{
    agtbasetime = basetime = GetAbsoluteTime();
    (void) GetTOD(&bootTOD);
}

void
AdjustTimeNow(target)
    SR_UINT32 target;
{
    SR_UINT32 current;
    SR_UINT32 oldbasetime = basetime;

    current = GetTimeNow();
    basetime -= target - current;
/*
 *  keep offset for sysUpToTOD routine
 */
    maBootTimeOffset += basetime - oldbasetime;
    return;
}


#ifdef SR_USE_GETTIMEOFDAY
SR_UINT32
TvpToTimeOfDay(tp)
    const struct timeval *tp;
{
    SR_UINT32 time_val;

    time_val = ((SR_UINT32) (tp->tv_sec)) * 100 +
		       ((SR_UINT32) (tp->tv_usec / 10000));

    previous_time = current_time;
    if (time_val < basetime) {
	if (wrap_flag == FALSE) {
            wrap_flag = TRUE;
        }
    }
    else {
        /* 
         * clock has been wrapped
         */
        if (wrap_flag == TRUE) {
            wrap_flag = FALSE;
            boot_counter++;
            /*
             * call_back function to update a configuration file is exist
             */
            if (bootData != NULL) {
	        (*bootData->fptr)(bootData);
            }
	    timerq_update = 1;
	}
    }
    current_time = time_val - basetime;
    return (time_val - basetime);
}

/*
 * TvpToTimeOfDayWrap
 *
 * return TvpToTimeOfDay. If TvpToTimeOfDay has been wrapped set "*wrap" to 1
 */
SR_UINT32
TvpToTimeOfDayWrap(tp, wrap)
    const struct timeval *tp;
    int *wrap;
{
    SR_UINT32 time_val;

    *wrap = 0;
    time_val = ((SR_UINT32) (tp->tv_sec)) * 100 +
		       ((SR_UINT32) (tp->tv_usec / 10000));

    previous_time = current_time;
    if (time_val < basetime) {
	if (wrap_flag == FALSE) {
            wrap_flag = TRUE;
        }
    }
    else {
        if (wrap_flag == TRUE) {
            wrap_flag = FALSE;
            boot_counter++;
            if (bootData != NULL) {
	        (*bootData->fptr)(bootData);
            }
            *wrap = 1;
	    timerq_update = 1;
	}
    }
    current_time = time_val - basetime;
    return (time_val - basetime);
}
#endif /* SR_USE_GETTIMEOFDAY */

/* ----------------------------------------------------------------------
 *  leapyear(y) - return 1 if y is a leapyear, else 0.
 *  moved out of psos ifdefs, as later code now uses it.
 */
int 
leapyear(int y) 
{
    if((((y%4) == 0) && !((y % 100) == 0)) ||
       ((y % 400) == 0)) {
        return 1;
    }
    return 0;
}

#ifdef SR_USE_TM_GET
/* pSOS doesn't seem to have any way of getting time in seconds since 1970, so
   we do it ourselves. */

#define SECSPERDAY (60*60*24)

SR_UINT32
ConvertToCalendarTime(year, month, day, hour, minute, second)
    SR_UINT32 year; 
    SR_UINT32 month; 
    SR_UINT32 day; 
    SR_UINT32 hour;
    SR_UINT32 minute; 
    SR_UINT32 second;
{
    int y, m;
    int secs = 0;

    for(y = 1970; y < year; y++) {
        if(leapyear(y)) {
            secs += 366 * SECSPERDAY;
        }
        else {
            secs += 365 * SECSPERDAY;
        }
    }
    for(m = 1; m < month; m++) {
        if(m == 2) {
            if(leapyear(year)) {
                secs += 29 * SECSPERDAY;
            } 
            else {
                secs += 28 * SECSPERDAY;
            }
        }
        else {
            secs += dayspermonth[m] * SECSPERDAY;
        }
    }
    secs += (day - 1) * SECSPERDAY;
    secs += hour * 60 * 60;
    secs += minute * 60;
    secs += second;
    return secs;
}
#endif	/* SR_USE_TM_GET */

OctetString *
MakeDateAndTime_track(tm, file, line)
    const struct tm *tm;
    char * file;
    unsigned int line;
{
    unsigned char buff[8];

    if (tm == NULL) {
        return NULL;
    } else {
        buff[0] = (char) ((tm->tm_year + 1900) >> 8);
        buff[1] = (char) ((tm->tm_year + 1900));
        buff[2] = (char) tm->tm_mon + 1;
        buff[3] = (char) tm->tm_mday;
        buff[4] = (char) tm->tm_hour;
        buff[5] = (char) tm->tm_min;
        buff[6] = (char) tm->tm_sec;
        buff[7] = 0;
    }
    return MakeOctetString_track(buff, 8, file, line);
}

/* ----------------------------------------------------------------------
 * 
 *  GetTOD - Get current time of day.
 *
 *  Arguments:
 *  i  SrTOD *tod
 *     Ptr to SrTOD structure.  The current time of day is filled in here.
 *     Note that if the current tod is not available, 
 *         tod->secs = 0
 *         tod->nsecs = sysUptime
 *     This is intended for use on systems where there is no time of
 *     day concept.
 *  o  int (function value)
 *     0: all ok.
 *     -1: some system call error.   Check errno.
 *     -2: not implemented or implementable on this OS.
 *
 *  Commentary:
 *     GetAbsoluteTime call kept for use, as it is used in GetTimeNow()
 *     which is --very-- heavily called.  This avoids an additional
 *     scaling operation to preserve data that is lost anyway 
 *     (most of the fractional digits are truncated in GetTimeNow).
 *     
 */
int 
GetTOD(SrTOD *tod) 
{

#ifdef SR_USE_FTIME
    struct _timeb t;

    _ftime(&t);
    tod->secs = t.time;
    tod->nsecs = t.millitm * (1000 * 1000);
#endif /* SR_USE_FTIME */

#ifdef SR_USE_CTICKS
/*
 *  Note careful operation order in nsecs computation
 */
    tod->secs = cticks / TPS;
    tod->nsecs = (cticks % TPS) * ((SR_INT32) 1000000000 / TPS);
#endif /* SR_USE_CTICKS */

#ifdef SR_USE_TICKGET
    SR_UINT32       ticks;
    SR_UINT32       clk_rate;

    ticks = (SR_UINT32) tickGet();
    clk_rate = (SR_UINT32) sysClkRateGet();
    tod->secs = ticks / clk_rate;
    tod->nsecs = (ticks % clk_rate) * ((SR_INT32) 1000000000 / clk_rate);
#endif /* SR_USE_TICKGET */

#ifdef SR_USE_TM_GET
    unsigned long date, time, ticks;

    tm_get(&date, &time, &ticks);

    ticks = (ticks * 100) / KC_TICKS2SEC;

    tod->secs = ConvertToCalendarTime(date >> 16, (date & 0xff00) >> 8,
                           date & 0xff,
                           time >> 16, (time & 0xff00) >> 8, time & 0xff);
    tod->secs += ticks / KC_TICKS2SEC;
    tod->nsecs = ((ticks % KC_TICKS2SEC) * 
        ((SR_INT32) 1000000000) / KC_TICKS2SEC);
#endif /* SR_USE_TM_GET */

#ifdef SR_USE_GETTIMEOFDAY
    struct timeval  tv;
    struct timezone tz;

    if (gettimeofday(&tv, &tz) == -1) {
        return -1;
    }
    tod->secs = tv.tv_sec;
    tod->nsecs = tv.tv_usec * 1000;
#endif /* SR_USE_GETTIMEOFDAY */

#ifdef SR_USE_TIMER_ENVIRON
    time_t secs;
    unsigned int ticks;
    int ticks_per_sec;

    _os_getime(&secs, &ticks);
    ticks_per_sec = ticks >> 16;
    ticks &= 0xFFFF;

    tod->secs = secs + ticks / ticks_per_sec;
    tod->nsecs = (ticks % ticks_per_sec ) * 
        ((SR_INT32) 1000000000) / tics_per_sec);
#endif /* SR_USE_TIMER_ENVIRON */

#ifdef SR_USE_OSE_TIME
    struct TimePair curtime;
    SR_UINT32 s;

    rtc_get_time(&curtime);
    tod->secs = curtime.seconds;
    tod->nsecs = curtime.micros * 1000;
#endif /* SR_USE_OSE_TIME */

#ifdef SR_USE_NUCLEUS_CTICKS
    SR_UINT32 curticks;

    curticks = NU_Retrieve_Clock();
    tod->secs = (curticks) / TICKS_PER_SECOND;
    tod->nsecs = (curticks % TICKS_PER_SECOND) * ((SR_INT32) 1000000000 / TICKS_PER_SECOND);
#endif /* SR_USE_NUCLEUS_CTICKS */

#ifdef SR_USE_WINCE_CTICKS
    SR_UINT32 curticks;

    curticks = (SR_UINT32)GetTickCount();

    tod->secs = curticks / 10000;
    tod->nsecs = (curticks % 10000) * ((SR_INT32) 1000000000 / 10000);
#endif /* SR_USE_WINCE_CTICKS */

#ifdef SR_USE_CHORUS_TIME
    KnTimeVal curtime;

    univTime(&curtime);
    tod->secs = curtime.tmSec;
    tod->nsecs = curtime.tmNSec;
#endif /* SR_USE_CHORUS_TIME */

#ifdef SR_USE_MQX_TIME
    TIME_STRUCT t;

    _time_get(&t);
    tod->secs = t.SECONDS;
    tod->nsecs = t.MILLISECONDS*1000000L;
#endif /* SR_USE_MQX_TIME */

    return 0;
}

/* ----------------------------------------------------------------------
 * 
 *  PrintTOD - Print current time of day.
 *
 *  Arguments:
 *  i  const SrTOD *tod
 *     Ptr to SrTOD structure. (include/sri/gettime.h)
 *     .  if time->secs == 0, it is assumed that time->nsecs has a 
 *        centiseconds, and is printed as ddd hh:mm:ss.ff
 *     .  if time == NULL, the current time is assumed.
 *     .  The time format is yyyy/mm/dd hh:mm:ss.ff
 *  i  char *buf
 *     Buffer in which to store time representation.  Must be at
 *     least 23 bytes long.  If null, a buffer is malloced, which
 *     the caller must then mfree.
 *  i  const int buflen
 *     Length of buf.
 *  o  char * (function value)
 *     Pointer to either buf or buffer malloced.  
 *     If NULL, malloc  failed, or buflen too small.
 */
char *
PrintTOD(const SrTOD *tod, char *buf, const int buflen)
{
    SrTOD curtod;		/* if tod argument NULL */
    long f;			/* fractional seconds */
    unsigned long s;		/* seconds */
    long m;			/* minutes */
    long h;			/* hours */
    long d;			/* day */
    long mo;			/* month */
    long y;			/* year */

    if (tod == NULL) {
        GetTOD(&curtod);
        tod = &curtod;
    }
/*
 *  If tod->secs == 0, then tod->nsecs represents a sysuptime.
 */
    if (tod->secs == 0) {
       s = tod->nsecs / 100;
       f = tod->nsecs % 100;
    }  else {
       s = tod->secs;
       f = tod->nsecs / (1000 * 1000 * 10);		/* get centiseconds */
    }
/*
 *  Verify buffer size
 */
    if (buf != NULL) {
       if ((tod->secs != 0 && buflen < 24) || buflen < 16) {
          return NULL;
       }
    } else if (buf == NULL) {
        buf = (char *) malloc(24);
        if (buf == NULL) {
            return NULL;
        }
    }
/*
 *  Split off fields.
 */
    m = s / 60;
    s %= 60;
    h = m / 60;
    m %= 60;
    d = h / 24;
    h %= 24;
/*
 *  If this is a sysuptime, print ddd hh:mm:ss.ff
 */
    if (tod->secs == 0) {
        snprintf(buf, buflen, "%3ld %2.2ld:%2.2ld:%2.2ld.%2.2ld", d, h, m, s, f);
        return buf;
    }
/*
 *  Otherwise, figure out the rest of the fields.  Note that we
 *  one to the day count, since Midnight Jan 1, 1970, is day zero.
 */
    else {
        int isleap = 0, dpm = 0;
        d++;
        for (y = 1970; d > 0; y++) {
            isleap = leapyear((int)y);
            d -= (365 + isleap);
        }
        d += (365 + isleap);
        y--;

        for (mo = 1; d > 0 && mo < 12; mo++) {
           if (mo == 2) {
               dpm = 28 + isleap;
           } else {
               dpm = dayspermonth[mo];
           }
           d -= dpm;
        }
        if (d <= 0) {
           mo--;
           d += dpm;
        }
        snprintf(buf, buflen, "%4ld/%2.2ld/%2.2ld %2.2ld:%2.2ld:%2.2ld.%2.2ld", 
            y, mo, d, h, m, s, f);
        return buf;
    }
}


/* ----------------------------------------------------------------------
 * 
 *  sysUpToTOD - Get time of day from sysUptime.
 *  This routine is cogniscent of altered basetimes (due to 
 *  sysUptime synchronization with master agent) so it can be
 *  called from master- or sub-agent context.
 *
 *  Arguments:
 *  i const SR_UINT32 uptime;
 *    sysUptime (from GetTimeNow()) or master agent).
 *  io SrTOD *tod
 *    Time of day returned here
 */

void 
sysUpToTOD(const SR_UINT32 uptime, SrTOD *tod)
{
    SR_UINT32 adjbase;

    adjbase = uptime + maBootTimeOffset;
/*
 *  Note that this code is carefully ordered so that the nsecs
 *  field is never greater than 2,000,000,000.
 */
    tod->nsecs = bootTOD.nsecs + (adjbase % 100) * (1000 * 1000 * 10);
    /* 
     * MAX_WRAP = 4294967295
     * probably "tod->secs = 
     *            bootTOD.secs + adjbase / 100 + (boot_counter*(MAX_WRAP/100));
     */
    tod->secs = bootTOD.secs + adjbase / 100 + 
                             (boot_counter*((SR_UINT32)MAX_WRAP/100));
    if (tod->nsecs >= 1000 * 1000 * 1000) {
       tod->secs += tod->nsecs / (1000 * 1000 * 1000);
       tod->nsecs = tod->nsecs % (1000 * 1000 * 1000);
    }
    return;
}

/* ----------------------------------------------------------------------
 *
 *  GetTimeNowInSeconds
 *
 *  This function emulates the functionality of GetTimeNow, except that
 *  it is built on top of GetTOD, so that is possible to get uptime
 *  in terms of seconds and fractions of seconds without the
 *  centisecond rounding problem.
 *
 *  Since this function will be called twice per SNMPv3 packet,
 *  the values are returned in separate arguments so that
 *  if nsecs is not wanted, it is not provided, reducing the
 *  operation count.
 *
 *  Arguments:
 *  io SR_INT32 *seconds
 *     If non-NULL, the number of seconds the system has been up
 *     is return here.  If NULL, this function returns immediatly,
 *     without even looking at the nseconds argument.
 *     This declared as a SR_INT32, as most places that use it will
 *     need an SR_INT32 to conform to rfc2574 et sequalae.
 *  io SR_UINT32 *nseconds
 *     If non-NULL, the number of nanoseconds the system has been up
 *     is returned here.  This is denominated in nanoseconds since
 *     some systems have this level of precision.
 *     
 */
void
GetTimeNowInSeconds(SR_INT32 *seconds, SR_UINT32 *nseconds)
{
    SrTOD now;

    GetTOD(&now);
    if (seconds != NULL) {
        *seconds = (SR_INT32) (now.secs -  bootTOD.secs);
        if (nseconds != NULL) {
            if (now.nsecs < bootTOD.nsecs) {
                *nseconds = 1 + now.nsecs - bootTOD.nsecs;
                (*seconds)--;
            } else {
                *nseconds = now.nsecs - bootTOD.nsecs;
            }
        }
    }
    return; 
}


/*
 *  Define local constants that specify the number of seconds, per minute,
 *  per hour, per day, and per (365 day) year.  GetSecondsOffsetFromUTC()
 *  (below) uses these.
 */
#define SR_GT_SECS_PER_MIN         60
#define SR_GT_SECS_PER_HOUR      3600
#define SR_GT_SECS_PER_DAY      86400
#define SR_GT_SECS_PER_YEAR  31536000


/*
 *   GetSecondsOffsetFromUTC()
 *
 *   Function:  Returns the number of seconds offset between
 *              the current local time and GMT.
 *
 *   Syntax:    Offset = GetSecondsOffsetFromUTC();
 *
 *   Input:     None passed explicitly, but queries the operating system
 *              for local and UTC times.
 *
 *   Output:    Returns the number of seconds between the local time zone
 *              and GMT.  The returned offset may be positive or negative,
 *              depending upon the local time zone's offset from UTC.
 *
 *              If this function is unable to retrieve or determine the off-
 *              set, it will return 0.  If the operating-system dependent
 *              code in this function is unimplemented, this function will 
 *              return 0. 
 * 
 *   Notes:
 *
 *   When interpreting the result from this function, here is the math:
 *
 *     UTC Time + Offset = Local Time
 *
 *   This function queries the operating system for local time and UTC time.
 *   It computes the offset in the following manner:
 *
 *   1.   UTC + Offset = Local  (from above)
 *
 *   2.   UTC - UTC + Offset = Local - UTC
 *
 *   3.   Offset = Local - UTC
 *
 *   This function accounts for the possiblilty that the local time and UTC 
 *   time may be on different days or even in different years.  Handling the
 *   possibility that the local time and UTC time may be on different days,
 *   or even in different years, is the reason for the calculations involving
 *   DiffYears/SR_GT_SECS_PER_YEAR and DiffDays/DiffYears/SR_GT_SECS_PER_YEAR.
 */
SR_INT32
GetSecondsOffsetFromUTC(void)
{
    SR_INT32   SecondsOffsetFromUTC = 0;
    int        Local_yday, UTC_yday;
    int        DiffYears, DiffDays, DiffHours, DiffMins;
    time_t     seconds;
    struct tm  Local, UTC, *tmp = NULL;
    SrTOD      tod;

    /* --- Get the seconds since the beginning of the epoch --- */
    if (GetTOD(&tod) != 0) {
        goto done;
    }

    /* --- Set the time zone --- */
#ifdef SR_USE_FTIME     /* applies to win32 */
    _tzset();           /* this sets _timezone */
#endif  /* SR_USE_FTIME */
    tzset();

    /* --- Convert to seconds since beginning of epoch to local time --- */
    seconds = (time_t)(tod.secs);
    tmp     = localtime(&seconds);
    if (tmp == NULL) {
        goto done;
    }
    memcpy(&Local, tmp, sizeof(struct tm));

    /* --- Convert to seconds since beginning of epoch to UTC time --- */
    tmp = gmtime(&seconds);
    if (tmp == NULL) {
        goto done;
    }
    memcpy(&UTC, tmp, sizeof(struct tm));

    /* --- Compute differences of years, hours, minutes --- */
    DiffYears  = Local.tm_year - UTC.tm_year;
    DiffHours  = Local.tm_hour - UTC.tm_hour;
    DiffMins   = Local.tm_min  - UTC.tm_min;

    /* --- Day of year may need leap-year correction below --- */
    Local_yday = Local.tm_yday;
    UTC_yday   = UTC.tm_yday;

    /*
     *  The SR_GT_SECS_PER_YEAR constant defines the number of seconds per
     *  year with 365 days.  In the case where one of the two time zones is
     *  in a leap year, and the other is not, then the number of days per
     *  year for one time zone could be 366.  The extra day would cause 
     *  erroneous calculations, so compensate for it.
     *
     *  Note the tm_yday field in the tm structures ranges from 0-365, not
     *  1-366.
     */
    if (Local_yday == 365) {
        Local_yday = 364;
    }
    if (UTC_yday == 365) {
        UTC_yday = 364;
    }

    /* --- Compute difference of days --- */
    DiffDays  = Local_yday - UTC_yday;

    /*
     *  Compute the seconds offset from UTC.  The calculations involving
     *  DiffDays and DiffYears are necessary in case one time zone has
     *  past midnight of the next day (and/or past midnight of 31-December.
     */
    SecondsOffsetFromUTC = (DiffYears * SR_GT_SECS_PER_YEAR) +
                           (DiffDays  * SR_GT_SECS_PER_DAY)  +
                           (DiffHours * SR_GT_SECS_PER_HOUR) +
                           (DiffMins  * SR_GT_SECS_PER_MIN);

done:
    /* --- Return the offset from GMT --- */
    return SecondsOffsetFromUTC;
}


/*
 *   GetSystemDateAndTime()
 *
 *   Function:   Queries the underlying operating system for the
 *               (real) date and time, converts it to a DateAndTime
 *               textual convention, and returns a pointer to the
 *               OctetString DateAndTime.
 *
 *   Calling syntax:   os = GetSystemDateAndTime(os, size); 
 *
 *   Inputs:   os   - specifies an OctetString * in which to put the
 *                    DateAndTime result. 
 *
 *                    If NULL is passed in, this function will malloc()
 *                    the OctetString and fill it in.  The caller must
 *                    free the OctetString malloc'ed by this function.
 *
 *                    If a non-null value is passed in, this function
 *                    will put the 8 or 11 byte result in the specified
 *                    OctetString.
 *
 *   Inputs:   size - specifies the number of bytes desired in the
 *                    result DateAndTime textual convention.  Valid
 *                    values are 8 or 11.
 *
 *   Outputs:  Returns an OctetString * to the DateAndTime textual 
 *             convention.  If an error occurs, this function returns 
 *             NULL.  If the 'os' argument is specified (and no errors
 *             occur), this function will return the 'os' value passed
 *             in.
 *
 *   Notes:
 */
OctetString *
GetSystemDateAndTime(OctetString *os, int size)
{
    FNAME("GetSystemDateAndTime")
    unsigned char    *buffer    = NULL;
    SR_INT32         Offset     = 0;
    struct  tm       *LocalTime = NULL;
    OctetString      *result    = NULL;
    SrTOD tod;
    time_t seconds;  /*BAB*/
    int retval;

    /*
     *   Error-check the size specified.
     */
    if ((size != 8) && (size != 11)) {
	DPRINTF((APWARN, "%s: Invalid size parameter %d, must be 8 or 11.\n",
                Fname, size));
        goto done;
    }
    if (os != NULL) {
        /* sanity check incoming octet string */
	if (os->length != size) {
	    DPRINTF((APWARN,
		     "%s: Incoming OctetString not the right size.  Should be %d and is %d\n",
		     Fname, size, os->length));
	    goto done;
	}
    }

    /*
     *   Get the local time from the operating system.
     */
     if ((retval = GetTOD(&tod)) != 0) {
	DPRINTF((APWARN, "%s: GetTOD failed with error %d\n",
                Fname, retval));
        goto done;
     }
#ifdef SR_USE_FTIME	/* applies to win32 */
    _tzset();		/* this sets _timezone */
#endif	/* SR_USE_FTIME */
   /* this sets timezone, probably unnecessary since localtime calls it */
    tzset();

    /*
     *   Convert to local time.
     */
    /*BAB*/
    seconds = (time_t)(tod.secs);
    LocalTime  = localtime(&seconds);
    /*BAB*/
     
    /*BAB - Note that this was causing a 32-bit/64-bit problem,
     *      as tod.secs is 32-bit int, and time_t * is 64-bit
     */         
    /*LocalTime  = localtime((time_t *) &tod.secs);*/

    if (LocalTime == NULL) {
	DPRINTF((APWARN, "%s: localtime failed\n", Fname));
        goto done;
    }

    /*
     *   Make sure we have space to build the DateAndTime.
     */
    if (os == NULL) {
        /*
	 * Create an octet string of the right size.  MakeOctetString
	 * creates a cleared OctetString of length size.
	 */
        if ((result = MakeOctetString(NULL, size)) == NULL) {
	    DPRINTF((APWARN, "%s: MakeOctetString failed\n", Fname));
            goto done;
        }
    } else {
        /* --- We've got the OctetString; clear it --- */
        result = os;
        memset(result->octet_ptr, 0, result->length);
    }

    /*
     *   Point to the character string in the octet string.
     */
    buffer = result->octet_ptr;
 
    /*
     *  Convert local time to the first 8-bytes of the DateAndTime.
     */
     buffer[0] = (unsigned char) ((LocalTime->tm_year + 1900) >> 8);
     buffer[1] = (unsigned char) ((LocalTime->tm_year + 1900));
     buffer[2] = (unsigned char) LocalTime->tm_mon + 1;
     buffer[3] = (unsigned char) LocalTime->tm_mday;
     buffer[4] = (unsigned char) LocalTime->tm_hour;
     buffer[5] = (unsigned char) LocalTime->tm_min;
     buffer[6] = (unsigned char) LocalTime->tm_sec;
     buffer[7] = 0;
 
    /*
     *  Return if the user only wants the 8-byte DateAndTime format.
     */
    if (size == 8) {
        goto done;
    }

    /* --- Get the number of seconds offset from UCT --- */
    Offset = GetSecondsOffsetFromUTC();

    /* --- Set default values for last 3 octets of DateAndTime --- */
    buffer[8]  = (char) '+';
    buffer[9]  = (char) 0;
    buffer[10] = (char) 0;

    /* --- If the Offset is zero, we're finished --- */
    if (Offset == 0) {
        goto done;
    }

    /* --- Change sign if the offset is less than zero --- */
    if (Offset < 0) {
        buffer[8]  = '-';
        Offset *= -1;
    }

    /* --- Compute the hours and minutes offset --- */
    buffer[9]  = (unsigned char)  (Offset / 3600);        /* Hours */
    buffer[10] = (unsigned char) ((Offset % 3600) / 60);  /* Minutes */

    /* --- Normal return --- */
done:
    return result;
}

/*
 *   GetTimeZone()
 *
 *   Function:   Queries the underlying operating system for the
 *               current timezone information, especially time difference
 *               between local time and GMT, then return GMT offset.
 *
 *   Calling syntax:   GmtOff = GetTimeZone(); 
 *
 *   Input:	 void 
 *   Output:	 returns long,	GMT offset time
 */

long 
GetTimeZone()
{
    FNAME("GetTimeZone")
    long             GmtOff     = 0;
    struct  tm       *LocalTime = NULL;
    SrTOD tod;
    time_t seconds;  
    int retval;
    long local_zone, gmt_zone;

    /*
     *   Get the local time from the operating system.
     */
     if ((retval = GetTOD(&tod)) != 0) {
	DPRINTF((APWARN, "%s: GetTOD failed with error %d\n",
                Fname, retval));
	return -1;
     }

#ifdef SR_USE_FTIME	/* applies to win32 */
    _tzset();		/* this sets _timezone */
#endif	/* SR_USE_FTIME */
   /* this sets timezone, probably unnecessary since localtime calls it */
    tzset();

    /*
     *   Convert to local time.
     */
    seconds = (time_t)(tod.secs);
    LocalTime  = localtime(&seconds);
     
    if (LocalTime == NULL) {
	DPRINTF((APWARN, "%s: localtime failed\n", Fname));
	return -1;
    }

    local_zone = (LocalTime->tm_hour*3600) + (LocalTime->tm_min * 60);
    LocalTime  = gmtime(&seconds);
    if (LocalTime == NULL) {
	DPRINTF((APWARN, "%s: gmtime() failed\n", Fname));
	return -1;
    } else {
        gmt_zone = (LocalTime->tm_hour*3600) + (LocalTime->tm_min * 60);
        GmtOff = gmt_zone - local_zone;
    }

    return GmtOff;
}

