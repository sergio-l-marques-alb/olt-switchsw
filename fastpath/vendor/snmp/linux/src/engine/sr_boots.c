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


#include <ctype.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include <stddef.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "global.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "sr_boots.h"

typedef struct SR_bootsAndTime {
    OctetString *id;
    SR_INT32 boots;
    SR_INT32 time;
    SR_INT32 lastTime;
} bootsAndTime;

static Index_t bootsAndTimeIndex[] = {
    { offsetof(bootsAndTime, id), T_octet }
};

static SnmpV2Table bootsAndTimeTable = {
    NULL,
    bootsAndTimeIndex,
    0,
    1,
    sizeof(bootsAndTime)
};


static int Init_bootsAndTimeTable(void);

static int
Init_bootsAndTimeTable(void)
{
    static int initialized = 0;

    if (initialized) {
        return 0;
    }

    bootsAndTimeTable.tp = (void **)malloc(sizeof(bootsAndTime *));
    if (bootsAndTimeTable.tp == NULL) {
        return -1;
    }
    initialized = 1;
    return 0;
}

void
LookupBootsAndTimeEntry(
    const OctetString *id,
    SR_INT32 *boots,
    SR_INT32 *time,
    SR_INT32 *lastTime)
{
    bootsAndTime *bat;
    int index;
    SR_INT32 curtime;

    if (Init_bootsAndTimeTable()) {
        return;
    }
    bootsAndTimeTable.tip[0].value.octet_val = (OctetString *) id;
    index = SearchTable(&bootsAndTimeTable, EXACT);
    if (index == -1) {
        *boots = 0;
        *time = 0;
        *lastTime = 0;
    } else {
        GetTimeNowInSeconds(&curtime, NULL);
        bat = (bootsAndTime *)bootsAndTimeTable.tp[index];
        *boots = bat->boots;
        *time = bat->time + curtime;
        *lastTime = bat->lastTime;
    }
}

void
SetBootsAndTimeEntry(
    const OctetString *id,
    SR_INT32 boots,
    SR_INT32 time,
    SR_INT32 lastTime)
{
    bootsAndTime *bat;
    int index;

    if (Init_bootsAndTimeTable()) {
        return;
    }
    bootsAndTimeTable.tip[0].value.octet_val = (OctetString *)id;
    index = SearchTable(&bootsAndTimeTable, EXACT);
    if (index == -1) {
        index = NewTableEntry(&bootsAndTimeTable);
    }
    if (index != -1) {
        bat = (bootsAndTime *)bootsAndTimeTable.tp[index];
        bat->boots = boots;
        bat->time = time;
        bat->lastTime = lastTime;
    }
}

void
DeleteBootsAndTimeEntry(const OctetString *id)
{
    bootsAndTime *bat;
    int index;

    if (Init_bootsAndTimeTable()) {
        return;
    }
    bootsAndTimeTable.tip[0].value.octet_val = (OctetString *)id;
    index = SearchTable(&bootsAndTimeTable, EXACT);
    if (index != -1) {
        bat = (bootsAndTime *)bootsAndTimeTable.tp[index];
        if (bat != NULL) {
            if (bat->id != NULL) {
                FreeOctetString(bat->id);
            }
            free((char *)bat);
            bat = NULL;
        }
        RemoveTableEntry(&bootsAndTimeTable, index);
    }
    return;
}

    
