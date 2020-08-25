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
#include "diag.h"
SR_FILENAME
#include "v3type.h"


/* global data describing the snmpNotifyTable entries */
Index_t snmpNotifyEntryIndex[] = {
    { offsetof(snmpNotifyEntry_t, snmpNotifyName),  T_octet}
};

static const ParserIntegerTranslation notifyType_Translations[] = {
    { "trap", 1 },
    { "inform", 2 },
    { "1", 1 },
    { "2", 2 },
    { NULL, 0 }
};
static const ParserIntegerTranslationTable notifyType_TranslationTable = {
    INTEGER_TYPE,
    "snmpNotifyType",
    8,
    notifyType_Translations
};

const SnmpType snmpNotifyEntryTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(snmpNotifyEntry_t, snmpNotifyName), 0 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(snmpNotifyEntry_t, snmpNotifyTag), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpNotifyEntry_t, snmpNotifyType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpNotifyEntry_t, snmpNotifyStorageType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpNotifyEntry_t, snmpNotifyRowStatus), -1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(snmpNotifyEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER snmpNotifyEntryConverters[] = {
    { ConvToken_textOctetString, NULL },    /* snmpNotifyName */
    { ConvToken_textOctetString, NULL },    /* snmpNotifyTag */
    { SR_INTEGER_TRANSLATION, (char *)&notifyType_TranslationTable },
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },    /* snmpNotifyRowStatus */
    { ConvToken_integer, "-1" },
    /* Additional entries for user defines go here */
    { NULL, NULL }
};
#ifndef SR_NO_COMMENTS_IN_CONFIG
const char snmpNotifyEntryRecordFormatString[] =
    "#Entry type: snmpNotifyEntry\n"
    "#Format:  snmpNotifyName  (text)\n"
    "#         snmpNotifyTag  (text)\n"
    "#         snmpNotifyType  (trap(1), inform(2))\n"
    "#         snmpNotifyStorageType  (nonVolatile, permanent, readOnly)";
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
#endif /* SR_CONFIG_FP */

/* global data describing the snmpNotifyFilterProfileTable entries */
Index_t snmpNotifyFilterProfileEntryIndex[] = {
    { offsetof(snmpNotifyFilterProfileEntry_t, snmpTargetParamsName),  T_octet}
};

const SnmpType snmpNotifyFilterProfileEntryTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(snmpNotifyFilterProfileEntry_t,
               snmpNotifyFilterProfileName), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpNotifyFilterProfileEntry_t,
               snmpNotifyFilterProfileStorType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpNotifyFilterProfileEntry_t,
               snmpNotifyFilterProfileRowStatus), -1 },
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(snmpNotifyFilterProfileEntry_t,
               snmpTargetParamsName), 0 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(snmpNotifyFilterProfileEntry_t,
               RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER snmpNotifyFilterProfileEntryConverters[] = {
    { ConvToken_textOctetString, NULL },    /* snmpNotifyFilterProfileName */
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },    /* snmpNotifyFilterProfileRowStatus */
    { ConvToken_textOctetString, NULL },    /* snmpTargetParamsName */
    { ConvToken_integer, "-1" },
    /* Additional entries for user defines go here */
    { NULL, NULL }
};
#ifndef SR_NO_COMMENTS_IN_CONFIG
const char snmpNotifyFilterProfileEntryRecordFormatString[] =
    "#Entry type: snmpNotifyFilterProfileEntry\n"
    "#Format:  snmpTargetParamsName  (text)\n"
    "#         snmpNotifyFilterProfileName  (text)\n"
    "#         snmpNotifyFilterProfileStorageType  (nonVolatile,permanent,readOnly)";
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
#endif /* SR_CONFIG_FP */

/* global data describing the snmpNotifyFilterTable entries */
Index_t snmpNotifyFilterEntryIndex[] = {
    { offsetof(snmpNotifyFilterEntry_t, snmpNotifyFilterProfileName),  T_var_octet},
    { offsetof(snmpNotifyFilterEntry_t, snmpNotifyFilterSubtree),  T_oid}
};

const SnmpType snmpNotifyFilterEntryTypeTable[] = {
    { OBJECT_ID_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(snmpNotifyFilterEntry_t, snmpNotifyFilterSubtree), 1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(snmpNotifyFilterEntry_t, snmpNotifyFilterMask), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpNotifyFilterEntry_t, snmpNotifyFilterType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpNotifyFilterEntry_t, snmpNotifyFilterStorageType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(snmpNotifyFilterEntry_t, snmpNotifyFilterRowStatus), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(snmpNotifyFilterEntry_t, snmpNotifyFilterProfileName), 0 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(snmpNotifyFilterEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER snmpNotifyFilterEntryConverters[] = {
    { ConvToken_OID, NULL },    /* snmpNotifyFilterSubtree */
    { ConvToken_octetString, NULL },    /* snmpNotifyFilterMask */
    { SR_INTEGER_TRANSLATION, (char *)&viewType_TranslationTable },
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },    /* snmpNotifyFilterRowStatus */
    { ConvToken_textOctetString, NULL },    /* snmpNotifyFilterProfileName */
    { ConvToken_integer, "-1" },
    /* Additional entries for user defines go here */
    { NULL, NULL }
};
#ifndef SR_NO_COMMENTS_IN_CONFIG
const char snmpNotifyFilterEntryRecordFormatString[] =
    "#Entry type: snmpNotifyFilterEntry\n"
    "#Format:  snmpNotifyFilterProfileName  (text)\n"
    "#         snmpNotifyFilterSubtree  (OID)\n"
    "#         snmpNotifyFilterMask  (octetString)\n"
    "#         snmpNotifyFilterType  (included, excluded)\n"
    "#         snmpNotifyFilterStorageType  (nonVolatile, permanent, readOnly)";
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
#endif /* SR_CONFIG_FP */

/* initialize support for snmpNotifyEntry objects */
int
i_snmpNotifyEntry_initialize(
    SnmpV2Table *snt)
{
    snt->tp = (void **) malloc (sizeof(snmpNotifyEntry_t *));
    if (snt->tp == NULL) {
        return 0;
    }
    snt->tip = snmpNotifyEntryIndex;
    snt->nitems = 0;
    snt->nindices = sizeof(snmpNotifyEntryIndex) / sizeof(Index_t);
    snt->rowsize = sizeof(snmpNotifyEntry_t);

    return 1;
}

/* terminate support for snmpNotifyEntry objects */
int
i_snmpNotifyEntry_terminate(
    SnmpV2Table *snt)
{
    /* free allocated memory */
    while(snt->nitems > 0) {
        FreeEntries(snmpNotifyEntryTypeTable, snt->tp[0]);
        free(snt->tp[0]);
        RemoveTableEntry(snt, 0);
    }

    if (snt->tp != NULL){
      free(snt->tp);
      snt->tp = NULL;
    }
    return 1;
}

/* initialize support for snmpNotifyFilterProfileEntry objects */
int
i_snmpNotifyFilterProfileEntry_initialize(
    SnmpV2Table *snfpt)
{
    snfpt->tp = (void **) malloc (sizeof(snmpNotifyFilterProfileEntry_t *));
    if (snfpt->tp == NULL) {
        return 0;
    }
    snfpt->tip = snmpNotifyFilterProfileEntryIndex;
    snfpt->nitems = 0;
    snfpt->nindices = sizeof(snmpNotifyFilterProfileEntryIndex) / sizeof(Index_t);
    snfpt->rowsize = sizeof(snmpNotifyFilterProfileEntry_t);

    return 1;
}

/* terminate support for snmpNotifyFilterProfileEntry objects */
int
i_snmpNotifyFilterProfileEntry_terminate(
    SnmpV2Table *snfpt)
{
    /* free allocated memory */
    while(snfpt->nitems > 0) {
        FreeEntries(snmpNotifyFilterProfileEntryTypeTable, snfpt->tp[0]);
        free(snfpt->tp[0]);
        RemoveTableEntry(snfpt, 0);
    }

    if (snfpt->tp != NULL){
      free(snfpt->tp);
      snfpt->tp = NULL;
    }
    return 1;
}

/* initialize support for snmpNotifyFilterEntry objects */
int
i_snmpNotifyFilterEntry_initialize(
    SnmpV2Table *snft)
{
    snft->tp = (void **) malloc (sizeof(snmpNotifyFilterEntry_t *));
    if (snft->tp == NULL) {
        return 0;
    }
    snft->tip = snmpNotifyFilterEntryIndex;
    snft->nitems = 0;
    snft->nindices = sizeof(snmpNotifyFilterEntryIndex) / sizeof(Index_t);
    snft->rowsize = sizeof(snmpNotifyFilterEntry_t);

    return 1;
}

/* terminate support for snmpNotifyFilterEntry objects */
int
i_snmpNotifyFilterEntry_terminate(
    SnmpV2Table *snft)
{
    /* free allocated memory */
    while(snft->nitems > 0) {
        FreeEntries(snmpNotifyFilterEntryTypeTable, snft->tp[0]);
        free(snft->tp[0]);
        RemoveTableEntry(snft, 0);
    }

    if (snft->tp != NULL){
      free(snft->tp);
      snft->tp = NULL;
    }
    return 1;
}

int
i_notif_initialize(
    SnmpV2Table *snt,
    SnmpV2Table *snfpt,
    SnmpV2Table *snft)
{
    int status = 1;

    if (i_snmpNotifyEntry_initialize(snt) != 1) {
        status = -1;
    }
    if (i_snmpNotifyFilterProfileEntry_initialize(snfpt) != 1) {
        status = -1;
    }
    if (i_snmpNotifyFilterEntry_initialize(snft) != 1) {
        status = -1;
    }

    return status;
}

int
i_notif_terminate(
    SnmpV2Table *snt,
    SnmpV2Table *snfpt,
    SnmpV2Table *snft)
{
    int status = 1;

    if (i_snmpNotifyEntry_terminate(snt) != 1) {
        status = -1;
    }
    if (i_snmpNotifyFilterProfileEntry_terminate(snfpt) != 1) {
        status = -1;
    }
    if (i_snmpNotifyFilterEntry_terminate(snft) != 1) {
        status = -1;
    }

    return status;
}
