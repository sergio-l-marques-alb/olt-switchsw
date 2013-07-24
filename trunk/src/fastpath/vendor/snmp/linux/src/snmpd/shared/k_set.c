/*
 *
 * Copyright (C) 1992-2002 by SNMP Research, Incorporated.
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
 */

#include "sr_conf.h"

#include <stdio.h>

#include <string.h>


#include "sr_snmp.h"
#include "sr_proto.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"

snmpSet_t snmpSetData;

int
k_set_initialize(void)
{
   snmpSetData.snmpSetSerialNo = 0;
   SET_ALL_VALID(snmpSetData.valid);
   return 1;
}

int
k_set_terminate(void)
{
   return 1;
}

snmpSet_t *
k_snmpSet_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   return(&snmpSetData);
}

#ifdef SETS
int
k_snmpSet_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    if (value->sl_value != snmpSetData.snmpSetSerialNo) {
        return INCONSISTENT_VALUE_ERROR;
    }
    return NO_ERROR;
}

int
k_snmpSet_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_snmpSet_set(data, contextInfo, function)
   snmpSet_t *data;
   ContextInfo *contextInfo;
   int function;
{
   if (data->snmpSetSerialNo == 2147483647) {
       snmpSetData.snmpSetSerialNo = 0;
   } else {
       snmpSetData.snmpSetSerialNo = data->snmpSetSerialNo + 1;
   }
   return NO_ERROR;
}

#ifdef SR_snmpSet_UNDO
/* add #define SR_snmpSet_UNDO in sitedefs.h to
 * include the undo routine for the snmpSet family.
 */
int
snmpSet_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_snmpSet_UNDO */

#endif /* SETS */

