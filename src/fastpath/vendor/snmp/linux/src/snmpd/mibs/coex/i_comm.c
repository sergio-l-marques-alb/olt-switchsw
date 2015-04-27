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
 */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>


#include <string.h>

#include <malloc.h>

#include <stddef.h>

#include "sr_type.h"
#include "sr_time.h"

#include "sr_snmp.h"
#include "sr_trans.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "snmpv2.h"
#include "coextype.h"

/* global data describing the snmpCommunityTable entries */
Index_t snmpCommunityEntryIndex[] = {
    { offsetof(snmpCommunityEntry_t, snmpCommunityIndex),  T_octet}
};

const SnmpType snmpCommunityEntryTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(snmpCommunityEntry_t, snmpCommunityIndex), 0 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(snmpCommunityEntry_t, snmpCommunityName), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(snmpCommunityEntry_t, snmpCommunitySecurityName), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(snmpCommunityEntry_t, snmpCommunityContextEngineID), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(snmpCommunityEntry_t, snmpCommunityContextName), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(snmpCommunityEntry_t, snmpCommunityTransportTag), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpCommunityEntry_t, snmpCommunityStorageType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpCommunityEntry_t, snmpCommunityStatus), -1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(snmpCommunityEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER snmpCommunityEntryConverters[] = {
    { ConvToken_textOctetString, NULL },    /* snmpCommunityIndex */
    { ConvToken_textOctetString, NULL },    /* snmpCommunityName */
    { ConvToken_textOctetString, NULL },    /* snmpCommunitySecurityName */
    { ConvToken_snmpID, NULL },    /* snmpCommunityContextEngineID */
    { ConvToken_textOctetString, NULL },    /* snmpCommunityContextName */
    { ConvToken_textOctetString, NULL },    /* snmpCommunityTransportTag */
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },    /* snmpCommunityStatus */
    { ConvToken_integer, "-1" },
    /* Additional entries for user defines go here */
    { NULL, NULL }
};
#ifndef SR_NO_COMMENTS_IN_CONFIG
const char snmpCommunityEntryRecordFormatString[] =
    "#Entry type: snmpCommunityEntry\n"
    "#Format:  snmpCommunityIndex  (text)\n"
    "#         snmpCommunityName  (text)\n"
    "#         snmpCommunitySecurityName  (text)\n"
    "#         snmpCommunityContextEngineID  (octetString)\n"
    "#         snmpCommunityContextName  (text)\n"
    "#         snmpCommunityTransportTag  (text)\n"
    "#         snmpCommunityStorageType  (nonVolatile, permanent, readOnly)";
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
#endif /* SR_CONFIG_FP */

/* initialize support for snmpCommunityEntry objects */
int
i_snmpCommunityEntry_initialize(
    SnmpV2Table *snmpCommunityTable)
{
    /* initialize index descriptor */
    snmpCommunityTable->tp = (void **)malloc (sizeof(snmpCommunityEntry_t *));
    if (snmpCommunityTable->tp == NULL) {
        return 0;
    }
    snmpCommunityTable->tip = snmpCommunityEntryIndex;
    snmpCommunityTable->nitems = 0;
    snmpCommunityTable->nindices = sizeof(snmpCommunityEntryIndex) / sizeof(Index_t);
    snmpCommunityTable->rowsize = sizeof(snmpCommunityEntry_t);

    return 1;
}

/* terminate support for snmpCommunityEntry objects */
int
i_snmpCommunityEntry_terminate(
    SnmpV2Table *snmpCommunityTable)
{
    /* free allocated memory */
    while(snmpCommunityTable->nitems > 0) {
        FreeEntries(snmpCommunityEntryTypeTable, snmpCommunityTable->tp[0]);
        free(snmpCommunityTable->tp[0]);
        RemoveTableEntry(snmpCommunityTable, 0);
    }
    
    if(snmpCommunityTable->tp != NULL){
       free(snmpCommunityTable->tp);
       snmpCommunityTable->tp = NULL;
    }
    return 1;
}
