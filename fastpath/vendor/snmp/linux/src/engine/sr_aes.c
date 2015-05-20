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
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY

#include "sr_conf.h"

#include <stdlib.h>     

#include "sr_type.h"
#include "sr_snmp.h"
#include "aes.h"
#include "sr_aes.h"

static UInt64 RandVal;
static int RandVal_set = 0;

int
AesSetup()
{
    SR_UINT32 stime;

    if (RandVal_set) {
       return 1;
    }
    stime = GetTimeNow();
    srand(stime);
    RandVal.little_end = rand();
    stime++;
    srand(stime);
    RandVal.big_end = rand();
    RandVal.little_end = rand();
    RandVal.big_end = rand();
    RandVal_set = 1;
    return 1;
}

int
Get_AES_RandVal(UInt64 *val)
{
   val->little_end = RandVal.little_end;
   val->big_end = RandVal.big_end;
   return 1;
}

void
Increment_AES_RandVal()
{
   RandVal.little_end++;
   if (RandVal.little_end == 0) {        /* if wrap occured */
       RandVal.big_end++;                /* increment big end */
   }
   return;
}
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
