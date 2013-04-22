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
 */


/*
 * Arguments used to create this file:
 * -f mib.cnf -o v3 -per_file_init -stubs_only -search_table (implies -row_status) 
 * -test_and_incr -parser -row_status -userpart -storage_type 
 */


/*
 * File Description:
 */

#include "sr_conf.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stddef.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "snmpv2.h"
#include "tdomain.h"
#include "diag.h"
SR_FILENAME
#include "v3type.h"


/* global data describing the snmpTargetObjects family */
snmpTargetObjects_t snmpTargetObjectsData;

/* global data describing the snmpTargetAddrTable entries */
Index_t snmpTargetAddrEntryIndex[] = {
    { offsetof(snmpTargetAddrEntry_t, snmpTargetAddrName),  T_octet}
};

const SnmpType snmpTargetAddrEntryTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(snmpTargetAddrEntry_t, snmpTargetAddrName), 0 },
    { OBJECT_ID_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetAddrEntry_t, snmpTargetAddrTDomain), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetAddrEntry_t, snmpTargetAddrTAddress), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetAddrEntry_t, snmpTargetAddrTimeout), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetAddrEntry_t, snmpTargetAddrRetryCount), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetAddrEntry_t, snmpTargetAddrTagList), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetAddrEntry_t, snmpTargetAddrParams), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetAddrEntry_t, snmpTargetAddrStorageType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetAddrEntry_t, snmpTargetAddrRowStatus), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetAddrExtEntry_t, snmpTargetAddrTMask), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetAddrExtEntry_t, snmpTargetAddrMMS), -1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(snmpTargetAddrEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER snmpTargetAddrEntryConverters[] = {
    { ConvToken_textOctetString, NULL },    /* snmpTargetAddrName */
    { ConvToken_TDomain, NULL },    /* snmpTargetAddrTDomain */
    { ConvToken_TAddr, NULL },    /* snmpTargetAddrTAddress */
    { ConvToken_integer, NULL },    /* snmpTargetAddrTimeout */
    { ConvToken_integer, NULL },    /* snmpTargetAddrRetryCount */
    { ConvToken_textOctetString, NULL },    /* snmpTargetAddrTagList */
    { ConvToken_textOctetString, NULL },    /* snmpTargetAddrParams */
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },    /* snmpTargetAddrRowStatus */
    { ConvToken_TAddrMask, NULL },    /* snmpTargetAddrTMask */
    { ConvToken_integer, "default:2048" },    /* snmpTargetAddrMMS */
    { ConvToken_integer, "-1" },
    /* Additional entries for user defines go here */
    { NULL, NULL }
};
#ifndef SR_NO_COMMENTS_IN_CONFIG
const char snmpTargetAddrEntryRecordFormatString[] =
    "#Entry type: snmpTargetAddrEntry\n"
    "#Format:  snmpTargetAddrName  (text)\n"
    "#         snmpTargetAddrTDomain  (snmpUDPDomain, snmpIPXDomain, etc.)\n"
    "#         snmpTargetAddrTAddress  (transport address,i.e. 192.147.142.254:0)\n"
    "#         snmpTargetAddrTimeout  (integer)\n"
    "#         snmpTargetAddrRetryCount  (integer)\n"
    "#         snmpTargetAddrTagList  (text)\n"
    "#         snmpTargetAddrParams  (text)\n"
    "#         snmpTargetAddrStorageType  (nonVolatile, permanent, readOnly)\n"
    "#         snmpTargetAddrTMask  (transport mask, i.e. 255.255.255.255:0)\n"
    "#         snmpTargetAddrMMS  (integer)";
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
#endif /* SR_CONFIG_FP */

/* global data describing the snmpTargetParamsTable entries */
Index_t snmpTargetParamsEntryIndex[] = {
    { offsetof(snmpTargetParamsEntry_t, snmpTargetParamsName),  T_octet}
};

const SnmpType snmpTargetParamsEntryTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(snmpTargetParamsEntry_t, snmpTargetParamsName), 0 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetParamsEntry_t, snmpTargetParamsMPModel), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetParamsEntry_t, snmpTargetParamsSecurityModel), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetParamsEntry_t, snmpTargetParamsSecurityName), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetParamsEntry_t, snmpTargetParamsSecurityLevel), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetParamsEntry_t, snmpTargetParamsStorageType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpTargetParamsEntry_t, snmpTargetParamsRowStatus), -1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(snmpTargetParamsEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER snmpTargetParamsEntryConverters[] = {
    { ConvToken_textOctetString, NULL },    /* snmpTargetParamsName */
    { ConvToken_integer, NULL },    /* snmpTargetParamsMPModel */
    { SR_INTEGER_TRANSLATION, (char *)&securityModel_TranslationTable },
    { ConvToken_textOctetString, NULL },    /* snmpTargetParamsSecurityName */
    { SR_INTEGER_TRANSLATION, (char *)&securityLevel_TranslationTable },
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },    /* snmpTargetParamsRowStatus */
    { ConvToken_integer, "-1" },
    /* Additional entries for user defines go here */
    { NULL, NULL }
};
#ifndef SR_NO_COMMENTS_IN_CONFIG
const char snmpTargetParamsEntryRecordFormatString[] =
    "#Entry type: snmpTargetParamsEntry\n"
    "#Format:  snmpTargetParamsName  (text)\n"
    "#         snmpTargetParamsMPModel  (integer)\n"
    "#         snmpTargetParamsSecurityModel  (snmpv1, snmpv2c, snmpv2s, usm"
#ifdef SR_EPIC
    ", epic"
    ", cli"
    ", xml"
#endif /* SR_EPIC */
#ifdef SR_EMWEB_EPIC
    ", emweb"
#endif /* SR_EMWEB_EPIC */
    ")\n"
    "#         snmpTargetParamsSecurityName  (text)\n"
    "#         snmpTargetParamsSecurityLevel  (noAuthNoPriv,authNoPriv,authPriv)\n"
    "#         snmpTargetParamsStorageType  (nonVolatile, permanent, readOnly)\n";
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
#endif /* SR_CONFIG_FP */

/* initialize support for snmpTargetObjects objects */
int
i_snmpTargetObjects_initialize(
    snmpTargetObjects_t *sto)
{
    sto->snmpTargetSpinLock = GetTimeNow() * 246;
    sto->snmpUnknownContexts = 0;
    return 1;
}

/* terminate support for snmpTargetObjects objects */
int
i_snmpTargetObjects_terminate(void)
{
    return 1;
}

/* initialize support for snmpTargetAddrEntry objects */
int
i_snmpTargetAddrEntry_initialize(
    SnmpV2Table *stat)
{
    stat->tp = (void **) malloc (sizeof(snmpTargetAddrEntry_t *));
    if (stat->tp == NULL) {
        return 0;
    }
    stat->tip = snmpTargetAddrEntryIndex;
    stat->nitems = 0;
    stat->nindices = sizeof(snmpTargetAddrEntryIndex) / sizeof(Index_t);
    stat->rowsize = sizeof(snmpTargetAddrEntry_t);

    return 1;
}

/* terminate support for snmpTargetAddrEntry objects */
int
i_snmpTargetAddrEntry_terminate(
    SnmpV2Table *stat)
{
    /* free allocated memory */
    while(stat->nitems > 0) {
        FreeEntries(snmpTargetAddrEntryTypeTable, stat->tp[0]);
        free(stat->tp[0]);
        RemoveTableEntry(stat, 0);
    }

    if (stat->tp != NULL){
      free(stat->tp);
      stat->tp = NULL;
    }
    return 1;
}

/* initialize support for snmpTargetParamsEntry objects */
int
i_snmpTargetParamsEntry_initialize(
    SnmpV2Table *stpt)
{
    stpt->tp = (void **) malloc (sizeof(snmpTargetParamsEntry_t *));
    if (stpt->tp == NULL) {
        return 0;
    }
    stpt->tip = snmpTargetParamsEntryIndex;
    stpt->nitems = 0;
    stpt->nindices = sizeof(snmpTargetParamsEntryIndex) / sizeof(Index_t);
    stpt->rowsize = sizeof(snmpTargetParamsEntry_t);

    return 1;
}

/* terminate support for snmpTargetParamsEntry objects */
int
i_snmpTargetParamsEntry_terminate(
    SnmpV2Table *stpt)
{
    /* free allocated memory */
    while(stpt->nitems > 0) {
        FreeEntries(snmpTargetParamsEntryTypeTable, stpt->tp[0]);
        free(stpt->tp[0]);
        RemoveTableEntry(stpt, 0);
    }

    if (stpt->tp != NULL){
      free(stpt->tp);
      stpt->tp = NULL;
    }
    return 1;
}

int
i_tgt_initialize(
    snmpTargetObjects_t *sto,
    SnmpV2Table *stat,
    SnmpV2Table *stpt)
{
    int status = 1;

    if (i_snmpTargetObjects_initialize(sto) != 1) {
        status = -1;
    }
    if (i_snmpTargetAddrEntry_initialize(stat) != 1) {
        status = -1;
    }
    if (i_snmpTargetParamsEntry_initialize(stpt) != 1) {
        status = -1;
    }

    return status;
}

int
i_tgt_terminate(
    SnmpV2Table *stat,
    SnmpV2Table *stpt)
{
    int status = 1;

    if (i_snmpTargetObjects_terminate() != 1) {
        status = -1;
    }
    if (i_snmpTargetAddrEntry_terminate(stat) != 1) {
        status = -1;
    }
    if (i_snmpTargetParamsEntry_terminate(stpt) != 1) {
        status = -1;
    }

    return status;
}
