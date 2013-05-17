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


/* global data describing the vacmMIBViews family */
vacmMIBViews_t vacmMIBViewsData;

/* global data describing the vacmViewTreeFamilyTable entries */
Index_t vacmViewTreeFamilyEntryIndex[] = {
    { offsetof(vacmViewTreeFamilyEntry_t, vacmViewTreeFamilyViewName),  T_var_octet},
    { offsetof(vacmViewTreeFamilyEntry_t, vacmViewTreeFamilySubtree),  T_var_oid}
};

const SnmpType vacmViewTreeFamilyEntryTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(vacmViewTreeFamilyEntry_t, vacmViewTreeFamilyViewName), 0 },
    { OBJECT_ID_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(vacmViewTreeFamilyEntry_t, vacmViewTreeFamilySubtree), 1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(vacmViewTreeFamilyEntry_t, vacmViewTreeFamilyMask), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(vacmViewTreeFamilyEntry_t, vacmViewTreeFamilyType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(vacmViewTreeFamilyEntry_t, vacmViewTreeFamilyStorageType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(vacmViewTreeFamilyEntry_t, vacmViewTreeFamilyStatus), -1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(vacmViewTreeFamilyEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER vacmViewTreeFamilyEntryConverters[] = {
    { ConvToken_textOctetString, NULL },    /* vacmViewTreeFamilyViewName */
    { ConvToken_OID, NULL },    /* vacmViewTreeFamilySubtree */
    { ConvToken_octetString, NULL },    /* vacmViewTreeFamilyMask */
    { SR_INTEGER_TRANSLATION, (char *)&viewType_TranslationTable },
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },    /* vacmViewTreeFamilyStatus */
    { ConvToken_integer, "-1" },
    /* Additional entries for user defines go here */
    { NULL, NULL }
};
#ifndef SR_NO_COMMENTS_IN_CONFIG
const char vacmViewTreeFamilyEntryRecordFormatString[] =
    "#Entry type: vacmViewTreeFamilyEntry\n"
    "#Format:  vacmViewTreeFamilyViewName  (text)\n"
    "#         vacmViewTreeFamilySubtree  (OID)\n"
    "#         vacmViewTreeFamilyMask  (octetString)\n"
    "#         vacmViewTreeFamilyType  (included, excluded)\n"
    "#         vacmViewTreeFamilyStorageType  (nonVolatile, permanent, readOnly)";
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
#endif /* SR_CONFIG_FP */

/* initialize support for vacmMIBViews objects */
int
i_vacmMIBViews_initialize(
    vacmMIBViews_t *vmv)
{
    vmv->vacmViewSpinLock = GetTimeNow() * 735;
    return 1;
}

/* terminate support for vacmMIBViews objects */
int
i_vacmMIBViews_terminate(void)
{
    return 1;
}

/* initialize support for vacmViewTreeFamilyEntry objects */
int
i_vacmViewTreeFamilyEntry_initialize(
    SnmpV2Table *vvtft)
{
    vvtft->tp = (void **) malloc (sizeof(vacmViewTreeFamilyEntry_t *));
    if (vvtft->tp == NULL) {
        return 0;
    }
    vvtft->tip = vacmViewTreeFamilyEntryIndex;
    vvtft->nitems = 0;
    vvtft->nindices = sizeof(vacmViewTreeFamilyEntryIndex) / sizeof(Index_t);
    vvtft->rowsize = sizeof(vacmViewTreeFamilyEntry_t);

    return 1;
}

/* terminate support for vacmViewTreeFamilyEntry objects */
int
i_vacmViewTreeFamilyEntry_terminate(
    SnmpV2Table *vvtft)
{
    /* free allocated memory */
    while(vvtft->nitems > 0) {
        FreeEntries(vacmViewTreeFamilyEntryTypeTable, vvtft->tp[0]);
        free(vvtft->tp[0]);
        RemoveTableEntry(vvtft, 0);
    }

    if (vvtft->tp != NULL){
      free(vvtft->tp);
      vvtft->tp = NULL;
    }
    return 1;
}

int
i_view_initialize(
    vacmMIBViews_t *vmv,
    SnmpV2Table *vvtft)
{
    int status = 1;

    if (i_vacmMIBViews_initialize(vmv) != 1) {
        status = -1;
    }
    if (i_vacmViewTreeFamilyEntry_initialize(vvtft) != 1) {
        status = -1;
    }

    return status;
}

int
i_view_terminate(
    SnmpV2Table *vvtft)
{
    int status = 1;

    if (i_vacmMIBViews_terminate() != 1) {
        status = -1;
    }
    if (i_vacmViewTreeFamilyEntry_terminate(vvtft) != 1) {
        status = -1;
    }

    return status;
}
