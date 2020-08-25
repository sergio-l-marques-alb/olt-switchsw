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


/* global data describing the vacmAccessTable entries */
Index_t vacmAccessEntryIndex[] = {
    { offsetof(vacmAccessEntry_t, vacmGroupName),  T_var_octet},
    { offsetof(vacmAccessEntry_t, vacmAccessContextPrefix),  T_var_octet},
    { offsetof(vacmAccessEntry_t, vacmAccessSecurityModel),  T_uint},
    { offsetof(vacmAccessEntry_t, vacmAccessSecurityLevel),  T_uint}
};

const SnmpType vacmAccessEntryTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(vacmAccessEntry_t, vacmAccessContextPrefix), 1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(vacmAccessEntry_t, vacmAccessSecurityModel), 2 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(vacmAccessEntry_t, vacmAccessSecurityLevel), 3 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(vacmAccessEntry_t, vacmAccessContextMatch), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(vacmAccessEntry_t, vacmAccessReadViewName), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(vacmAccessEntry_t, vacmAccessWriteViewName), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(vacmAccessEntry_t, vacmAccessNotifyViewName), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(vacmAccessEntry_t, vacmAccessStorageType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(vacmAccessEntry_t, vacmAccessStatus), -1 },
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(vacmAccessEntry_t, vacmGroupName), 0 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(vacmAccessEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER vacmAccessEntryConverters[] = {
    { ConvToken_textOctetString, NULL },    /* vacmAccessContextPrefix */
    { SR_INTEGER_TRANSLATION, (char *)&securityModel_TranslationTable },
    { SR_INTEGER_TRANSLATION, (char *)&securityLevel_TranslationTable },
    { SR_INTEGER_TRANSLATION, (char *)&contextMatch_TranslationTable },
    { ConvToken_textOctetString, NULL },    /* vacmAccessReadViewName */
    { ConvToken_textOctetString, NULL },    /* vacmAccessWriteViewName */
    { ConvToken_textOctetString, NULL },    /* vacmAccessNotifyViewName */
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },    /* vacmAccessStatus */
    { ConvToken_textOctetString, NULL },    /* vacmGroupName */
    { ConvToken_integer, "-1" },
    /* Additional entries for user defines go here */
    { NULL, NULL }
};
#ifndef SR_NO_COMMENTS_IN_CONFIG
const char vacmAccessEntryRecordFormatString[] =
    "#Entry type: vacmAccessEntry\n"
    "#Format:  vacmGroupName  (text)\n"
    "#         vacmAccessContextPrefix  (text)\n"
    "#         vacmAccessSecurityModel  (snmpv1, snmpv2c, usm"
#ifdef SR_EPIC
    ", epic"
    ", xml"
    ", cli"
#endif /* SR_EPIC */
#ifdef SR_EMWEB_EPIC
    ", emweb"
#endif /* SR_EMWEB_EPIC */
    ")\n"
    "#         vacmAccessSecurityLevel  (noAuthNoPriv, authNoPriv, authPriv)\n"
    "#         vacmAccessContextMatch  (exact, prefix)\n"
    "#         vacmAccessReadViewName  (text)\n"
    "#         vacmAccessWriteViewName  (text)\n"
    "#         vacmAccessNotifyViewName  (text)\n"
    "#         vacmAccessStorageType  (nonVolatile, permanent, readOnly)";
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
#endif /* SR_CONFIG_FP */

/* initialize support for vacmAccessEntry objects */
int
i_vacmAccessEntry_initialize(
    SnmpV2Table *vat)
{
    vat->tp = (void **) malloc (sizeof(vacmAccessEntry_t *));
    if (vat->tp == NULL) {
        return 0;
    }
    vat->tip = vacmAccessEntryIndex;
    vat->nitems = 0;
    vat->nindices = sizeof(vacmAccessEntryIndex) / sizeof(Index_t);
    vat->rowsize = sizeof(vacmAccessEntry_t);

    return 1;
}

/* terminate support for vacmAccessEntry objects */
int
i_vacmAccessEntry_terminate(
    SnmpV2Table *vat)
{
    /* free allocated memory */
    while(vat->nitems > 0) {
        FreeEntries(vacmAccessEntryTypeTable, vat->tp[0]);
        free(vat->tp[0]);
        RemoveTableEntry(vat, 0);
    }

    if (vat->tp != NULL){
      free(vat->tp);
      vat->tp = NULL;
    }
    return 1;
}

int
i_ac_initialize(
    SnmpV2Table *vat)
{
    int status = 1;

    if (i_vacmAccessEntry_initialize(vat) != 1) {
        status = -1;
    }

    return status;
}

int
i_ac_terminate(
    SnmpV2Table *vat)
{
    int status = 1;

    if (i_vacmAccessEntry_terminate(vat) != 1) {
        status = -1;
    }

    return status;
}
