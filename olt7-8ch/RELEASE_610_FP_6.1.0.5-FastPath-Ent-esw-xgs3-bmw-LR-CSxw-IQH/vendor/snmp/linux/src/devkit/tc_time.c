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

/*
 *  Function:  This file contains functions for rendering DateAndTime
 *             textual conventions and TimeTicks types.
 */



#include "sr_conf.h"



#include <stdio.h>


#include <sys/types.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <unistd.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>



#include "sr_type.h"

#include "sr_snmp.h"

#include "tc_time.h"


/*
 *  For the DateAndTime textual convention, map the month integer
 *  to human-friendly 3 character strings.
 */
const char *MonthNames[] = {
    "???",
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec",
};


/*
 *  Create an array of unsigned characters and an OctetString that represents
 *  0000-Jan-1,00:00:00.0.  Used for rendering in SPrintDateAndTime();
 */
static 
unsigned char JanOctets[] = { 0x00, 0x00, 0x01, 0x01, 
                              0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00 };

static const
OctetString Jan1short = { JanOctets, 8 };

static const
OctetString Jan1long = { JanOctets, 11 };


/*
 *  SPrintDateAndTime()
 *
 *  Function:  Renders an 8 or 11 byte DateAndTime OctetString 
 *             MIB object into a human-readable format.
 *
 *  Inputs:    os      - Points to the OctetString to render.
 *             buf     - Points to a character buffer in which to render.
 *             bufsize - specifies the size of the character buffer.
 *
 *  Outputs:   On failure, returns -1, and does not render into the buffer.
 *             On success, returns 0, and renders into the buffer.
 *
 *  Notes:  The minimum-size buffer that may be passed in is 32 bytes.
 *
 *          When processing an OctetString, this function runs every
 *          possible range check to verify the OctetString can be
 *          rendered as a DateAndTime.  If any of these range checks
 *          fails, this function returns a failure return code and
 *          does not modify the character string output buffer.
 *
 *          By doing all of the possible range-checks, the function
 *          may be used to "guess" if an OctetString is a DateAndTime
 */
int
SPrintDateAndTime(OctetString *os, char *buf, int bufsize)
{
    int year;

    /* --- Verify arguments and a reasonable bufsize were passed--- */
    if ((os == NULL) || (buf == NULL) || (bufsize < 32)) {
        return -1;
    }

    /* - Based upon length, verify the OctetString is/not a DateAndTime - */
    if ((os->length != 8) && (os->length != 11)) {
        return -1;
    }

    /* --- Special case rendering of "00 00 01 01 00 00 00 00" --- */
    if (CmpOctetStringsWithLen(os, &Jan1short) == 0) {
        snprintf(buf, bufsize, "0000-Jan-1,00:00:00.0");
        return 0;
    }

    /* --- Special case rendering of "00 00 01 01 00 00 00 00 00 00 00" --- */
    if (CmpOctetStringsWithLen(os, &Jan1long) == 0) {
        snprintf(buf, bufsize, "0000-Jan-1,00:00:00.0");
        /* sprintf(buf, "0000-Jan-1,00:00:00.0+0:0"); */
        return 0;
    }

    /* --- Return failure if year octets are out-of-range --- */
    year = ((int) os->octet_ptr[0] << 8) + ((int) os->octet_ptr[1]);
    if ((year < 1900) || (year > 2100)) {
        return -1;
    }

    /* --- Return failure if month octet is out-of-range --- */
    if ((os->octet_ptr[2] < (unsigned char) 1) || 
        (os->octet_ptr[2] > (unsigned char) 12)) {
        return -1;
    }

    /* --- Return failure if day octet is out-of-range --- */
    if ((os->octet_ptr[3] < (unsigned char) 1) || 
        (os->octet_ptr[3] > (unsigned char) 31)) {
        return -1;
    }

    /*
     *  Unsigned char type can't contain a value less than zero, so no
     *  lower-bound range check is necessary for some of these octets.
     */

    /* --- Return failure if hour octet is out-of-range --- */
    if (os->octet_ptr[4] > (unsigned char) 23) {
        return -1;
    }

    /* --- Return failure if minute octet is out-of-range --- */
    if (os->octet_ptr[5] > (unsigned char) 59) {
        return -1;
    }

    /* --- Return failure if second octet is out-of-range --- */
    if (os->octet_ptr[6] > (unsigned char) 60) {
        return -1;
    }

    /* --- Return failure if deci-second octet is out-of-range --- */
    if (os->octet_ptr[7] > (unsigned char) 9) {
        return -1;
    }

    /* --- Return the 8-octet date-and-time format --- */
    if (os->length == 8) {
        snprintf(buf, bufsize, "%d-%s-%d,%02d:%02d:%02d.%1d", 
                year, 
                MonthNames[os->octet_ptr[2]], 
                os->octet_ptr[3], 
                os->octet_ptr[4], 
                os->octet_ptr[5], 
                os->octet_ptr[6], 
                os->octet_ptr[7]);
        return 0;
    }

    /* --- Return failure if direction octet is out-of-range --- */
    if ((os->octet_ptr[8] != '+') && (os->octet_ptr[8] != '-')) {
        return -1;
    }

    /* --- Return failure if hours from UTC octet is out-of-range --- */
    if (os->octet_ptr[9] > (unsigned char) 11) {
        return -1;
    }

    /* --- Return failure if minutes from UTC octet is out-of-range --- */
    if (os->octet_ptr[10] > (unsigned char) 59) {
        return -1;
    }

    /* --- Return the 11-octet date-and-time format --- */
    snprintf(buf, bufsize, "%d-%s-%d,%02d:%02d:%02d.%1d,%c%d:%d", 
            year, 
            MonthNames[os->octet_ptr[2]], 
            os->octet_ptr[3], 
            os->octet_ptr[4], 
            os->octet_ptr[5], 
            os->octet_ptr[6], 
            os->octet_ptr[7], 
            os->octet_ptr[8], 
            os->octet_ptr[9], 
            os->octet_ptr[10]);

    /* --- Normal return --- */
    return 0;
}


/*
 *  Constants for TimeTicks calculations.  Each constant is 
 *  the number of centi-seconds per ___.
 */
#define CSECS_PER_SECOND       100
#define CSECS_PER_MINUTE      6000
#define CSECS_PER_HOUR      360000
#define CSECS_PER_DAY      8640000


/*
 *  SPrintTimeTicks()
 *
 *  Function:  Renders a time-ticks MIB object into a human-readable format.
 *
 *  Inputs:    tt      - Specifies the TimeTicks value.
 *             buf     - Points to a character buffer in which to render.
 *             bufsize - specifies the size of the character buffer.
 *
 *  Outputs:   On failure, returns -1, and does not render into the buffer.
 *             On success, returns 0, and renders into the buffer.
 *
 *  Notes:  The minimum-size buffer that may be passed in is 22 bytes.
 *
 *          The output format is:  dd days, hh:mm:ss.cc
 */
int
SPrintTimeTicks(SR_UINT32 tt, char *buf, int bufsize)
{
    char       ibuf[16];
    int        days = 0, hours = 0, minutes = 0, seconds = 0;
    SR_UINT32  csecs = tt;

    /* --- Verify arguments and a reasonable bufsize were passed--- */
    if ((buf == NULL) || (bufsize < 22)) {
        return -1;
    }

    /* --- Calculate the number of days --- */
    days   = (int) (csecs / CSECS_PER_DAY);
    csecs  = (csecs % CSECS_PER_DAY);

    /* --- Calculate the number of hours --- */
    hours  = (int) (csecs / CSECS_PER_HOUR);
    csecs  = (csecs % CSECS_PER_HOUR);

    /* --- Calculate the number of minutes --- */
    minutes  = (int) (csecs / CSECS_PER_MINUTE);
    csecs    = (csecs % CSECS_PER_MINUTE);

    /* --- Calculate the number of seconds --- */
    seconds  = (int) (csecs / CSECS_PER_SECOND);
    csecs    = (csecs % CSECS_PER_SECOND);

    /* --- Clear the output buffer --- */
    memset(buf, 0x00, bufsize);

    /* --- Sprint the number of days --- */
    if (days > 0) {
        /* --- Singular --- */
        if (days == 1) {
            strcpy(buf, "1 day, ");
        }
        /* --- Plural --- */
        else {
            snprintf(ibuf, bufsize, "%d days, ", days);
            strcat(buf, ibuf);
        }
    } 

    /* --- Sprint the time --- */
    snprintf(ibuf, bufsize, "%02d:%02d:%02d.%02d", hours, minutes, seconds, csecs);
    strcat(buf, ibuf);

    /* --- Normal return --- */
    return 0;    
}

/*
 * TextToDateAndTime():
 *
 *    Function: Convert a string in the format produced by
 *              SPrintDateAndTime() to DateAndTime textual 
 *              convention format as defined in RFC 2579. 
 *
 *    Input: A character string with the date and time encoded 
 *           in the format: 2004-Jan-26,13:30:15.0 or 
 *           2004-Jan-26,13:30:15.0,-4:0.
 *
 *    Outputs: Return an OctetString with the date and time encoded according
 *             to the DateAndTime textual convention defined in RFC 2579.
 *             On failure, return NULL.
 */
OctetString *
TextToDateAndTime(char *dat)
{
    char *ptr;
    int year, month, day, hour, minute, second, deci_second;
    int dir_from_utc, hours_from_utc, minutes_from_utc;
    int i, size = 8;
    OctetString *os_ptr;

    if (dat == NULL) {
        return NULL;
    }
    if (strlen(dat) < 14) {
        return NULL;
    }
    ptr = dat;

    /* year */
    if (!isdigit(dat[0]) || !isdigit(dat[1]) || !isdigit(dat[2]) || 
        !isdigit(dat[3]) || dat[4] != '-') {
        return NULL;
    }
    year = atoi(ptr); 
    ptr += 5;

    /* month */
    month = 0;
    if (ptr[1] == '-') {
        if (!isdigit(ptr[0])) {
            return NULL;
        }
        month = atoi(ptr);
        if ((month < 1) || (month > 9)) {
            month = 0;
            return NULL;
        }
        ptr += 2;
    }
    else if (ptr[2] == '-'){
        if (!isdigit(ptr[0]) || !isdigit(ptr[1])) {
            return NULL;
        }
        month = atoi(ptr);
        if ((month < 1) || (month > 12)) {
            month = 0;
            return NULL;
        }
        ptr += 3;
    }
    else if (ptr[3] == '-') {
        for (i = 1; i <= 12; i++) {
           if (strncmp(ptr, MonthNames[i], 3) == 0) {
               month = i;
               break;
           }
        }
        if (month == 0) {
            return NULL;
        }
        ptr += 4;
    } 
    else {
        return NULL;
    } 

    /* day */
    if (!isdigit(ptr[0])) {
        return NULL;
    }
    day = atoi(ptr);
    ptr = strchr(ptr, ',');
    if (ptr == NULL) {
        return NULL;
    }
    ptr++;

    /* hour */
    if (!isdigit(ptr[0])) {
        return NULL;
    }
    hour = atoi(ptr);
    ptr = strchr(ptr, ':');
    if (ptr == NULL) {
        return NULL;
    }
    ptr++;

    /* minute */
    if (!isdigit(ptr[0])) {
        return NULL;
    }
    minute = atoi(ptr);
    ptr = strchr(ptr, ':');
    if (ptr == NULL) {
        return NULL;
    }
    ptr++;

    /* second */
    if (!isdigit(ptr[0])) {
        return NULL;
    }
    second = atoi(ptr);
    ptr = strchr(ptr, '.');
    if (ptr == NULL) {
        return NULL;
    }
    ptr++;

    /* deci-second */
    if (!isdigit(ptr[0])) {
        return NULL;
    }
    deci_second = atoi(ptr);
    ptr++;

    /* done ? */
    if (strlen(ptr) == 0) {
        size = 8;
    } else {
        size = 11;
        /* direction from UTC */
        if (strlen(ptr) < 5) {
            return NULL;
        }
        ptr++;
        dir_from_utc = *ptr;
        if (dir_from_utc != '+' && dir_from_utc != '-') {
            return NULL;
        }
        ptr++;
        /* hours from UTC */
        hours_from_utc = atoi(ptr);
        ptr++;
        ptr++; /* skip : */
        /* minutes from UTC */
        minutes_from_utc = atoi(ptr);
        if (minutes_from_utc > 9) {
            ptr += 2;
        }
        else {
            ptr++;
        }
        if (strlen(ptr) != 0) {
            return NULL;
        }
    }

    os_ptr = MakeOctetString(NULL, size);
    if (os_ptr == NULL) {
        return NULL;
    }
    os_ptr->octet_ptr[0] = (unsigned char) (year >> 8); 
    os_ptr->octet_ptr[1] = (unsigned char) (year & 0xff);
    os_ptr->octet_ptr[2] = (unsigned char) month;
    os_ptr->octet_ptr[3] = (unsigned char) day;
    os_ptr->octet_ptr[4] = (unsigned char) hour;
    os_ptr->octet_ptr[5] = (unsigned char) minute;
    os_ptr->octet_ptr[6] = (unsigned char) second;
    os_ptr->octet_ptr[7] = (unsigned char) deci_second;
    if (os_ptr->length == 11) {
        os_ptr->octet_ptr[8] = dir_from_utc;
        os_ptr->octet_ptr[9] = hours_from_utc;
        os_ptr->octet_ptr[10] = minutes_from_utc;
    }

    return os_ptr;
}

/*
 *  SrGetMonth()
 *
 *  Function:  Returns a textual representation of the month.
 *             If a DateAndTime value is passed in as a parameter,
 *             the month returned is that of the date value.  If the
 *             parameter is NULL, return the current month.
 *
 *  Inputs:    dt_os   - Points to a DateAndTime OctetString (may be NULL).
 *
 *  Outputs:   On success, returns an element of the MonthNames array.
 *             On failure, returns NULL.
 *
 *  Notes:  This is an initial, quick-and-dirty implementation using
 *          SPrintDateAndTime() to verify that the OctetString is valid.
 *
 */
const char *
SrGetMonth(OctetString *dt_os)
{
    char dt[32];  /* minimum size */

    if (dt_os != NULL) {
	/* return the month of the passed-in DateAndTime value */
        if (SPrintDateAndTime(dt_os, dt, sizeof(dt)) == -1) {
	    /* the parameter is not a valid DateAndTime value */
	    return NULL;
        }
    } else {
	/* no parameter was passed in, so return the current month */
	dt_os = GetSystemDateAndTime((OctetString *) NULL, 8);
	if (dt_os == NULL) {
	    return NULL;
	}
        if (SPrintDateAndTime(dt_os, dt, sizeof(dt)) != -1) {
	    return NULL;
        }
        FreeOctetString(dt_os);
        dt_os = NULL;
    }
    return MonthNames[dt_os->octet_ptr[2]];
}


