/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
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

#ifndef SR_SRI_GETTIME_H
#define SR_SRI_GETTIME_H


#ifdef  __cplusplus
extern "C" {
#endif

/*
 *  Data structure returned by GetTOD and PrintTOD
 */

typedef struct srTOD {
    SR_UINT32 secs;		/* Full seconds */
    SR_UINT32 nsecs;		/* Fractional (nano) seconds */
} SrTOD;


void InitTimeNow(void);

SR_UINT32 GetTimeNow(void);

SR_UINT32 GetSubagentTimeNow(void);

SR_UINT32 GetSubagentTimeNowWrap(int *wrap);
SR_UINT32 GetTimeNowWrap(int *wrap);
SR_UINT32 TvpToTimeOfDayWrap(const struct timeval *tp, int *wrap);

void GetTimeNowInSeconds(SR_INT32 *seconds, SR_UINT32 *nseconds);

void AdjustTimeNow(SR_UINT32 target);

int leapyear(int y);

int GetTOD(SrTOD *tod);

char *PrintTOD(const SrTOD *tod, char *buf, const int buflen);

void sysUpToTOD(const SR_UINT32 uptime, SrTOD *tod);

OctetString *MakeDateAndTime(const struct tm *tm);

OctetString *GetSystemDateAndTime(OctetString *os, int size);

SR_INT32 GetSecondsOffsetFromUTC(void);

#define MAX_WRAP 4294967295UL

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_SRI_GETTIME_H */
