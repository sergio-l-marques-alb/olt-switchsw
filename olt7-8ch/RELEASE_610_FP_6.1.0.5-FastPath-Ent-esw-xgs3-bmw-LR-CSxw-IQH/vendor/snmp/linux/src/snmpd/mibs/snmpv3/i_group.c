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


/* global data describing the vacmSecurityToGroupTable entries */
Index_t vacmSecurityToGroupEntryIndex[] = {
    { offsetof(vacmSecurityToGroupEntry_t, vacmSecurityModel),  T_uint},
    { offsetof(vacmSecurityToGroupEntry_t, vacmSecurityName),  T_var_octet}
};

const SnmpType vacmSecurityToGroupEntryTypeTable[] = {
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(vacmSecurityToGroupEntry_t, vacmSecurityModel), 0 },
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(vacmSecurityToGroupEntry_t, vacmSecurityName), 1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(vacmSecurityToGroupEntry_t, vacmGroupName), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(vacmSecurityToGroupEntry_t, vacmSecurityToGroupStorageType), -1},
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(vacmSecurityToGroupEntry_t, vacmSecurityToGroupStatus), -1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(vacmSecurityToGroupEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER vacmSecurityToGroupEntryConverters[] = {
    { SR_INTEGER_TRANSLATION, (char *)&securityModel_TranslationTable },
    { ConvToken_textOctetString, NULL },    /* vacmSecurityName */
    { ConvToken_textOctetString, NULL },    /* vacmGroupName */
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },    /* vacmSecurityToGroupStatus */
    { ConvToken_integer, "-1" },
    /* Additional entries for user defines go here */
    { NULL, NULL }
};
#ifndef SR_NO_COMMENTS_IN_CONFIG
const char vacmSecurityToGroupEntryRecordFormatString[] =
    "#Entry type: vacmSecurityToGroupEntry\n"
    "#Format:  vacmSecurityModel  (snmpv1, snmpv2c, usm"
#ifdef SR_EPIC
    ", epic"
    ", cli"
    ", xml"
#endif /* SR_EPIC */
#ifdef SR_EMWEB_EPIC
    ", emweb"
#endif /* SR_EMWEB_EPIC */
    ")\n"
    "#         vacmSecurityName  (text)\n"
    "#         vacmGroupName  (text)\n"
    "#         vacmSecurityToGroupStorageType  (nonVolatile, permanent, readOnly)";
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
#endif /* SR_CONFIG_FP */

/* initialize support for vacmSecurityToGroupEntry objects */
int
i_vacmSecurityToGroupEntry_initialize(
    SnmpV2Table *vstgt)
{
    vstgt->tp = (void **) malloc (sizeof(vacmSecurityToGroupEntry_t *));
    if (vstgt->tp == NULL) {
        return 0;
    }
    vstgt->tip = vacmSecurityToGroupEntryIndex;
    vstgt->nitems = 0;
    vstgt->nindices = sizeof(vacmSecurityToGroupEntryIndex) / sizeof(Index_t);
    vstgt->rowsize = sizeof(vacmSecurityToGroupEntry_t);

    return 1;
}

/* terminate support for vacmSecurityToGroupEntry objects */
int
i_vacmSecurityToGroupEntry_terminate(
    SnmpV2Table *vstgt)
{
    /* free allocated memory */
    while(vstgt->nitems > 0) {
        FreeEntries(vacmSecurityToGroupEntryTypeTable, vstgt->tp[0]);
        free(vstgt->tp[0]);
        RemoveTableEntry(vstgt, 0);
    }
    
    free(vstgt->tp);
    return 1;
}

int
i_group_initialize(
    SnmpV2Table *vstgt)
{
    int status = 1;

    if (i_vacmSecurityToGroupEntry_initialize(vstgt) != 1) {
        status = -1;
    }

    return status;
}

int
i_group_terminate(
    SnmpV2Table *vstgt)
{
    int status = 1;

    if (i_vacmSecurityToGroupEntry_terminate(vstgt) != 1) {
        status = -1;
    }

    return status;
}
