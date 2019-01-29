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
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "diag.h"
SR_FILENAME
#include "v3type.h"



/* global data describing the vacmContextTable entries */
Index_t vacmContextEntryIndex[] = {
    { offsetof(vacmContextEntry_t, vacmContextName),  T_var_octet}
};

#ifdef SR_CONFIG_FP
const PARSER_CONVERTER vacmContextEntryConverters[] = {
    { ConvToken_textOctetString, NULL },    /* vacmContextName */
    { ConvToken_integer, "0" },             /* use_count */
    /* Additional entries for user defines go here */
    { NULL, NULL }
};

#ifndef SR_NO_COMMENTS_IN_CONFIG
const char vacmContextEntryRecordFormatString[] =
    "#Entry type: vacmContextEntry\n"
    "#Format:  vacmContextName  (textOctetString)";
#endif  /* SR_NO_COMMENTS_IN_CONFIG */
#endif /* SR_CONFIG_FP */


const SnmpType vacmContextEntryTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(vacmContextEntry_t, vacmContextName), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(vacmContextEntry_t, use_count), -1 },
    /* Additional entries for user defines go here */
    { -1, -1, (unsigned short) -1, -1 }
};

/* initialize support for vacmContextEntry objects */
int
i_vacmContextEntry_initialize(
    SnmpV2Table *vct)
{
    vct->tp = (void **) malloc (sizeof(vacmContextEntry_t *));
    if (vct->tp == NULL) {
        return 0;
    }
    vct->tip = vacmContextEntryIndex;
    vct->nitems = 0;
    vct->nindices = sizeof(vacmContextEntryIndex) / sizeof(Index_t);
    vct->rowsize = sizeof(vacmContextEntry_t);

    /* 
     * The add default context code has been moved to 
     *  i_vacmContextEntry_addDefaultContext() to avoid
     *  bogus messages about overwriting existing vacmContext entry.
     *  This gets called in snmpd/shared/agtinit:Configure().
     */

    return 1;
}

/* add default vacmContextEntry row */
int
i_vacmContextEntry_addDefaultContext (
    SnmpV2Table *vct)
{
    int index;
    vacmContextEntry_t *vce;

    /* 
     * The add default context code has been moved to 
     *  i_vacmContextEntry_addDefaultContext() to avoid
     *  bogus messages about overwriting existing vacmContext entry.
     */
    /* Add the default context */

    vct->tip[0].value.octet_val = MakeOctetString(NULL, 0);
    if (vct->tip[0].value.octet_val == NULL) {
        return 0;
    }
    
    if ((index = SearchTable(vct, EXACT)) == -1) {
       index = NewTableEntry(vct);
       FreeOctetString(vct->tip[0].value.octet_val);
       if (index == -1) {
           return 0;
       }
       vce = (vacmContextEntry_t *)vct->tp[index];
       vce->use_count = 1;
    } else {
       FreeOctetString(vct->tip[0].value.octet_val);	/* already in table */
    }
    return 1;
}

/* terminate support for vacmContextEntry objects */
int
i_vacmContextEntry_terminate(
    SnmpV2Table *vct)
{
    /* free allocated memory */
    while(vct->nitems > 0) {
        FreeEntries(vacmContextEntryTypeTable, vct->tp[0]);
        free(vct->tp[0]);
        RemoveTableEntry(vct, 0);
    }

    if (vct->tp != NULL){
      free(vct->tp);
      vct->tp = NULL;
    }

    return 1;
}

int
i_ctx_initialize(
    SnmpV2Table *vct)
{
    int status = 1;

    if (i_vacmContextEntry_initialize(vct) != 1) {
        status = -1;
    }

    return status;
}

int
i_ctx_terminate(
    SnmpV2Table *vct)
{
    int status = 1;

    if (i_vacmContextEntry_terminate(vct) != 1) {
        status = -1;
    }

    return status;
}
