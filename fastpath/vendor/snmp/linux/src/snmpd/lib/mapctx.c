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

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <sys/types.h>




#include <malloc.h>

#include "sr_snmp.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "sr_user.h"
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"

#include "mapctx.h"


int
AddContextMapping(
    contextMappingList_t *cml,
    OctetString *contextName,
    void *data_ptr)
{
    int i;
    OctetString *new_contextName;
    contextMapping_t *new_mappings;

    if (contextName == NULL) {
        cml->default_context_data_ptr = data_ptr;
        return 0;
    }

    if (contextName->length == 0) {
        cml->default_context_data_ptr = data_ptr;
        return 0;
    }

    if (cml->num_mappings == 0) {
        cml->mappings = (contextMapping_t *)malloc(sizeof(contextMapping_t));
        if (cml->mappings == NULL) {
            return -1;
        }
        cml->mappings->contextName = CloneOctetString(contextName);
        if (cml->mappings->contextName == NULL) {
            free(cml->mappings);
            cml->mappings = NULL;
            return -1;
        }
        cml->num_mappings = 1;
        cml->mappings->data_ptr = data_ptr;
        return 0;
    }

    for (i = 0; i < cml->num_mappings; i++) {
        if (CmpOctetStrings(contextName, cml->mappings[i].contextName) == 0) {
            cml->mappings[i].data_ptr = data_ptr;
            return 0;
        }
    }

    new_contextName = CloneOctetString(contextName);
    if (new_contextName == NULL) {
        return -1;
    }

    new_mappings = (contextMapping_t *)
        realloc(cml->mappings,
                (cml->num_mappings + 1) * sizeof(contextMapping_t));
    if (new_mappings == NULL) {
        FreeOctetString(new_contextName);
        return -1;
    }
    cml->mappings = new_mappings;
    cml->mappings[cml->num_mappings].contextName = new_contextName;
    cml->mappings[cml->num_mappings].data_ptr = data_ptr;
    cml->num_mappings++;
    return 0;
}

int
AddContextMappingText(
    contextMappingList_t *cml,
    char *contextName_text,
    void *data_ptr)
{
    OctetString os;

    if (contextName_text == NULL) {
        return AddContextMapping(cml, NULL, data_ptr);
    }

    os.octet_ptr = (unsigned char *)contextName_text;
    os.length = strlen(contextName_text);
    return AddContextMapping(cml, &os, data_ptr);
}

int
RemoveContextMapping(
    contextMappingList_t *cml,
    OctetString *contextName)
{
    int i;

    if (contextName == NULL) {
        return -1;
    }

    if (contextName->length == 0) {
        return -1;
    }

    if (cml->num_mappings == 0) {
        return -1;
    }

    for (i = 0; i < cml->num_mappings; i++) {
        if (CmpOctetStrings(contextName, cml->mappings[i].contextName) == 0) {
            FreeOctetString(cml->mappings[i].contextName);
            for (i++; i < cml->num_mappings; i++) {
                cml->mappings[i-1].contextName = cml->mappings[i].contextName;
                cml->mappings[i-1].data_ptr = cml->mappings[i].data_ptr;
            }
            cml->num_mappings--;
            return 0;
        }
    }

    return -1;
}

int
RemoveContextMappingText(
    contextMappingList_t *cml,
    char *contextName_text)
{
    OctetString os;

    if (contextName_text == NULL) {
        return RemoveContextMapping(cml, NULL);
    }

    os.octet_ptr = (unsigned char *)contextName_text;
    os.length = strlen(contextName_text);
    return RemoveContextMapping(cml, &os);
}

void
MapContext(
    contextMappingList_t *cml,
    ContextInfo *cip,
    void **data_access_ptr)
{
    int i;
    ObjectSyntax *os;

    os = SearchContextInfo(cip, NCI_CONTEXT_NAME);
    if (os == NULL) {
        *data_access_ptr = cml->default_context_data_ptr;
        return;
    }
    if (os->type != OCTET_PRIM_TYPE) {
        *data_access_ptr = cml->default_context_data_ptr;
        return;
    }
    if (os->os_value == NULL) {
        *data_access_ptr = cml->default_context_data_ptr;
        return;
    }
    if (os->os_value->length == 0) {
        *data_access_ptr = cml->default_context_data_ptr;
        return;
    }

    for (i = 0; i < cml->num_mappings; i++) {
        if (CmpOctetStrings(os->os_value, cml->mappings[i].contextName) == 0) {
            *data_access_ptr = cml->mappings[i].data_ptr;
            return;
        }
    }

    *data_access_ptr = cml->default_context_data_ptr;
    return;
}
