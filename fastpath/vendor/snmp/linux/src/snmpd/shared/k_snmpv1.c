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

#include <stdlib.h>

#include <malloc.h>


#include <string.h>

#include "sr_snmp.h"
#include "sr_proto.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "mibout.h"
#include "lookup.h"
#include "v2table.h"
#include "sr_cfg.h"
#include "scan.h"
#include "diag.h"
SR_FILENAME

#define MAXBUFSIZE 32

snmp_t  snmpData;

const SnmpType enableAuthenTrapsType[] = {
    { INTEGER_TYPE, SR_READ_WRITE, 0, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER enableAuthenTrapsConverters[] = {
    { ConvToken_integer, NULL },
    { NULL, NULL }
};
const PARSER_RECORD_TYPE enableAuthenTrapsRecord = {
    PARSER_SCALAR, "snmpEnableAuthenTraps ", enableAuthenTrapsConverters,
    enableAuthenTrapsType, NULL, &snmpData.snmpEnableAuthenTraps,
    -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
#endif /* SR_CONFIG_FP */

snmp_t *k_snmp_get
    (int serialNum, ContextInfo *contextInfo, int nominator);

int
k_snmp_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_snmp_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_snmp_set(data, contextInfo, function)
   snmp_t *data;
   ContextInfo *contextInfo;
   int function;
{
   snmpData.snmpEnableAuthenTraps = data->snmpEnableAuthenTraps;
   writeConfigFileFlag = TRUE;
   return NO_ERROR;
}

snmp_t *
k_snmp_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   SET_ALL_VALID(snmpData.valid);
   return (&snmpData);
}

/* LVL7 */
#include "l7_common.h"

L7_uint32
k_snmp_snmpEnableAuthenTrapsGet()
{
  if (snmpData.snmpEnableAuthenTraps == D_snmpEnableAuthenTraps_enabled)
  {
    return L7_ENABLE;
  }
  return L7_DISABLE;
}

void
k_snmp_snmpEnableAuthenTrapsSet(L7_uint32 val)
{
  
  if (val == L7_ENABLE)
  {
    snmpData.snmpEnableAuthenTraps = D_snmpEnableAuthenTraps_enabled;
  }
  else
  {
    snmpData.snmpEnableAuthenTraps = D_snmpEnableAuthenTraps_disabled;
  }
}
/* LVL7 end */
