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
 *
 */

#include "sr_conf.h"

#include <stdio.h>

#include "sr_snmp.h"
#include "lookup.h"
#include "v2table.h"
#include "snmptype.h"
#include "oid_lib.h"
#include "diag.h"
SR_FILENAME

extern SnmpV2Table agt_snmpNotifyFilterTable;

static snmpNotifyFilterEntry_t *lookup_filter(
    OctetString *filterName,
    int         *index);

/*
 * lookup_filter
 *
 * This routine searches for the first entry in the snmpNotifyFilterTable
 * that has a particular filterName if index is equal to -1. If index is
 * not equal to -1, index is incremented and this routine checks to see
 * if we have gone past the last item in the table. In both cases, it
 * makes sure that entry matches what we are looking for. This routine
 * returns the actual snmpNotifyFilterTable entry on success and NULL
 * on failure.
 */
static snmpNotifyFilterEntry_t *
lookup_filter(
    OctetString *filterName,
    int *index)
{
    OID empty_oid;
    snmpNotifyFilterEntry_t *snfe;

  try_next:
    if (*index == -1) {
        empty_oid.oid_ptr = NULL;
        empty_oid.length = 0;
        agt_snmpNotifyFilterTable.tip[0].value.octet_val = filterName;
        agt_snmpNotifyFilterTable.tip[1].value.oid_val = &empty_oid;
        *index = SearchTable(&agt_snmpNotifyFilterTable, NEXT_SKIP);
        if (*index == -1) {
            return NULL;
        }
    } else {
        (*index)++;
        if (*index >= agt_snmpNotifyFilterTable.nitems) {
            *index = -1;
            return NULL;
        }
    }

    snfe = (snmpNotifyFilterEntry_t *)agt_snmpNotifyFilterTable.tp[*index];
    if (CmpOctetStrings(filterName, snfe->snmpNotifyFilterProfileName)) {
        *index = -1;
        return NULL;
    }
    if (snfe->snmpNotifyFilterRowStatus != RS_ACTIVE) {
        goto try_next;
    }
    return snfe;
}

/*
 * SrCheckNotificationFilter
 *
 * This routine checks to see if the given object,
 * object_instance, matches a particular notificatin filter.
 *
 * Returns:  1 oid is specifically excluded
 *           0 oid is specifically included
 *          -1 no entries matched the oid
 */
int
SrCheckNotificationFilter(
    OID *object_instance,
    OctetString *filterName,
    int *first_index)
{
    SR_INT32        len_tree;
    SR_INT32        len_bits;
    SR_INT32        pos;
    unsigned char  *view_mask;
    int             index = -1;
    snmpNotifyFilterEntry_t *longest = NULL;
    snmpNotifyFilterEntry_t *snfe = NULL;
    int status = -2;

    if (first_index != NULL) {
        index = *first_index;
    }

    if (object_instance == NULL) {
        DPRINTF((APACCESS, "SrCheckNotificationFilter: object_instance is NULL.\n"));
        return 0;
    }

    if (filterName == NULL) {
        DPRINTF((APACCESS, "SrCheckNotificationFilter: filterName is NULL.\n"));
        return 0;
    }

    /*
     * Need to find the longest snmpNotifyFilterSubtree shorter than or
     * equal to the object_instance in length (or the lexically greatest
     * of these if there are more than one).
     */

    while ((snfe = lookup_filter(filterName, &index)) != NULL) {

        if (status == -2) {
            *first_index = index - 1;
            status = -1;
        }

        len_tree = snfe->snmpNotifyFilterSubtree->length;

        if (object_instance->length >= len_tree) {

            view_mask = snfe->snmpNotifyFilterMask->octet_ptr;
            len_bits = snfe->snmpNotifyFilterMask->length * 8;

            /*
             * Check to see if the snmpNotifyFilterSubtree matches the
             * object.  There is a match if the two are exactly equal
             * or the snmpNotifyFilterMask has a value of 0 (wildcard)
             * where they differ.
             */
            for (pos = 0; pos < len_tree; pos++) {
                if ((pos >= len_bits ||
                 (view_mask[pos / 8] & (1 << (7-(pos % 8)))) != 0) &&
                    object_instance->oid_ptr[pos] !=
                    snfe->snmpNotifyFilterSubtree->oid_ptr[pos]) {

                    break;
                }
            }

            if (pos == len_tree) {
                status = 0;
                /* find the lexically greatest of the longest matched */
                if ((longest == NULL) ||
                    (len_tree > longest->snmpNotifyFilterSubtree->length) ||
                    (CmpOID(snfe->snmpNotifyFilterSubtree,
                            longest->snmpNotifyFilterSubtree) > 0)) {
                    longest = snfe;
                }
            }
        }
    }

    if (status == -2) {
        /* There were no entries in the snmpNotifyFilterTable */
        return 0;
    }

    if (longest == NULL) {
        /* There were no entries matching the oid */
        return -1;
    }

    if (longest->snmpNotifyFilterType == EXCLUDED) {
        /* The longest match specifically excludes the oid */
        DPRINTF((APACCESS, "SrCheckNotificationFilter: OID is excluded.\n"));
        return 1;
    }

    /* The longest match specifically includes the oid */
    DPRINTF((APACCESS, "SrCheckNotificationFilter: OID is included.\n"));
    return 0;

}
