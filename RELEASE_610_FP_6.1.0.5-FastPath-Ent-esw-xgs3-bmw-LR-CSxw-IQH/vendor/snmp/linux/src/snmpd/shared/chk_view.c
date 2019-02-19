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

#include <string.h>

#include <stdlib.h>

#include "sr_snmp.h"
#include "lookup.h"
#include "v2table.h"
#include "snmptype.h"
#include "frmt_lib.h"
#include "oid_lib.h"
#include "diag.h"
#include "chk_view.h"
SR_FILENAME

#ifdef SR_SNMPv3_ADMIN
#define VIEWTABLE   agt_vacmViewTreeFamilyTable
#define VIEWENTRY_T vacmViewTreeFamilyEntry_t
#define VIEWNAME    vacmViewTreeFamilyViewName
#define VIEWSUBTREE vacmViewTreeFamilySubtree
#define VIEWMASK    vacmViewTreeFamilyMask
#define VIEWTYPE    vacmViewTreeFamilyType
#define VIEWSTATUS  vacmViewTreeFamilyStatus
#endif /* SR_SNMPv3_ADMIN */

extern SnmpV2Table VIEWTABLE;


VIEWENTRY_T *LookupViewFamily(
    OctetString *viewName,
    int         *index);

/*
 * LookupViewFamily:
 *    This routine searches for the first entry in the VIEWTABLE that has
 *    a particular viewName if index is equal to -1. If index is not equal
 *    to -1, index is incremented and this routine checks to see if we have
 *    gone past the last item in the table. In both cases, it makes sure that
 *    entry matches what we are looking for. This routine returns the
 *    actual VIEWTABLE entry on success and NULL on failure.
 */
VIEWENTRY_T *
LookupViewFamily(viewName, index)
    OctetString *viewName;
    int         *index;
{
    OID empty_oid;
    VIEWENTRY_T *ve;

  try_next:
    if (*index == -1) {
        empty_oid.oid_ptr = NULL;
        empty_oid.length = 0;
        VIEWTABLE.tip[0].value.octet_val = viewName;
        VIEWTABLE.tip[1].value.oid_val = &empty_oid;
        *index = SearchTable(&VIEWTABLE, NEXT_SKIP);
        if (*index == -1) {
            return NULL;
        }
    } else {
        (*index)++;
        if (*index >= VIEWTABLE.nitems) {
            *index = -1;
            return NULL;
        }
    }

    ve = (VIEWENTRY_T *)VIEWTABLE.tp[*index];
    if (CmpOctetStrings(viewName, ve->VIEWNAME)) {
        *index = -1;
        return NULL;
    }
    if (ve->VIEWSTATUS != RS_ACTIVE) {
        goto try_next;
    }
    return ve;
}

/*
 * SrCheckMIBView
 *
 * This routine checks to see if the given object,
 * object_instance, is in this MIB view (context).  This goes
 * through the view table and looks at all entries that have a
 * viewName field that matches the viewName field
 * passed in.  The matching row that has the longest
 * VIEWSUBTREE field (or is lexicographically greater or equal
 * VIEWSUBTREE) is the correct entry.
 *
 * Arguments: object_instance - is this object in the MIB view?
 *            viewName - the MIB view (matches any VIEWSUBTREE values in
 *                           the view table).
 *            first_index - if NULL, start the search at the beginning of the
 *                          table. If not NULL, start search at that index.
 *
 * Returns:  1 if the match is an included subtree.
 *           0 if the match is an excluded subtree, there were no matches, or
 *             another error occurred.
 *
 * The former name (pre-snmp12.1.0.0 release) was check_mib_view().
 */
int
SrCheckMIBView(
    OID *object_instance,
    OctetString *viewName,
    int *first_index)
{
    SR_INT32        len_tree;
    SR_INT32        len_bits;
    SR_INT32        pos;
    unsigned char  *view_mask;
    int             index = -1;
    VIEWENTRY_T      *longest_match = NULL;
    VIEWENTRY_T      *ve = NULL;
#ifdef SR_DEBUG
    char             VNbuf[32], OIDSbuf[MAX_OID_SIZE * 4];
#endif /* SR_DEBUG */

    if (first_index != NULL) {
        index = *first_index;
    }

    if (object_instance == NULL) {
        DPRINTF((APACCESS, "CheckMIBView: object_instance is NULL.\n"));
        return 0;
    }

    if (viewName == NULL) {
        DPRINTF((APACCESS, "CheckMIBView: viewName is NULL.\n"));
        return 0;
    }


    /*
     * Need to find the longest VIEWSUBTREE shorter than or equal to the
     * object_instance in length (or the lexically greatest of these if there
     * are more than one).
     */

    while ((ve = LookupViewFamily(viewName, &index)) != NULL) {

        len_tree = ve->VIEWSUBTREE->length;

        if (object_instance->length >= len_tree) {

            view_mask = ve->VIEWMASK->octet_ptr;
            len_bits = ve->VIEWMASK->length * 8;

            /*
             * Check to see if the VIEWSUBTREE matches the object. There
             * is a match if the two are exactly equal or the VIEWMASK
             * has a value of 0 (wildcard) where they differ.
             */
            for (pos = 0; pos < len_tree; pos++) {
                if ((pos >= len_bits ||
                 (view_mask[pos / 8] & (1 << (7-(pos % 8)))) != 0) &&
                    object_instance->oid_ptr[pos] !=
                    ve->VIEWSUBTREE->oid_ptr[pos]) {

                    break;
                }
            }                /* for (pos = 0; ..... ) */

            if (pos == len_tree) {
                /* find the lexically greatest of the longest matched */
                if (longest_match == NULL ||
                    len_tree > longest_match->VIEWSUBTREE->length ||
                    CmpOID(ve->VIEWSUBTREE,
                                   longest_match->VIEWSUBTREE) > 0) {
                    longest_match = ve;
                }
            }                /* if (pos < len_tree) */
        }                        /* if object_instance ... */
    }                                /* while */

    /*
     * If there was no match in the view table or the match says that this
     * tree is EXCLUDED from their view, return 0.
     */

    if ((longest_match == NULL) || (longest_match->VIEWTYPE == EXCLUDED)) {
#ifdef SR_DEBUG
        /* --- Format the view name --- */
        memset(VNbuf, 0x00, sizeof(VNbuf));    
        SPrintAscii(viewName, VNbuf);

        /* --- Format the OID --- */
        memset(OIDSbuf, 0x00, sizeof(OIDSbuf));

        DPRINTF((APACCESS, "CheckMIBView: OID not in MIB view \"%s\".\n", VNbuf));
#endif /* SR_DEBUG */

        return 0;
    }

    DPRINTF((APACCESS, "CheckMIBView: OID is in MIB view.\n"));
    return 1;

}

/*
 * SrCheckClassMIBView:
 *
 * The former name (pre-snmp12.1.0.0 release) was check_class_mib_view().
 */
int
SrCheckClassMIBView(
    OID *object_class,
    OctetString *viewName,
    int *first_index)
{
    FNAME("CheckClassMIBView")
    SR_INT32         len_tree;
    SR_INT32         len_bits;
    SR_INT32         len_common;
    unsigned char   *view_mask;
    SR_INT32         pos;
    int              index = -1;
    VIEWENTRY_T      *longest_match = NULL;
    VIEWENTRY_T      *ve = NULL;
    int              AllIncluded = 1;
    int              AllExcluded = 1;

    if (first_index != NULL) {
        index = *first_index;
    }

    if (object_class == NULL) {
        DPRINTF((APACCESS, "%s: object_class is NULL.\n", Fname));
        return -1;
    }

    if (viewName == NULL) {
        DPRINTF((APACCESS, "CheckMIBView: viewName is NULL.\n"));
        return -1;
    }

    /*
     * Need to find all the VIEWSUBTREE which are *longer* than the
     * object_class which match as well as the longest one shorter than or
     * equal to the object_class in length (or the lexically greatest of
     * these if there are more than one).
     *
     * If all of the (zero or more VIEWSUBTREE in) above collection are
     * EXCLUDED, then we return -1 indicating a method routine call is
     * unnecessary.
     * 
     * Else if some are INCLUDED and some are EXCLUDED then we return 0
     * indicating that the method call must be made, but a post check must
     * also be made with CheckMIBView(object_instance, viewName).
     *
     * Else all such matches are INCLUDED and we return 1 indicating that the
     * method call should be made and no post check is necessary.
     */

    while ((ve = LookupViewFamily(viewName, &index)) != NULL) {

        len_tree = ve->VIEWSUBTREE->length;

        view_mask = ve->VIEWMASK->octet_ptr;
        len_bits = ve->VIEWMASK->length * 8;

        len_common = MIN(len_tree, object_class->length);

        for (pos = 0; pos < len_common; pos++) {
            if ((pos >= len_bits ||
                 (view_mask[pos / 8] & (1 << (7-(pos % 8)))) != 0) &&
                object_class->oid_ptr[pos] !=
                ve->VIEWSUBTREE->oid_ptr[pos]) {
                break;
            }
        }

        if (pos == len_common) {
            if (len_tree <= object_class->length) {
                /*
                 * find the lexically greatest of the longest matched of
                 * this type
                 */
                if (longest_match == NULL ||
                    len_tree > longest_match->VIEWSUBTREE->length ||
                    CmpOID(ve->VIEWSUBTREE,
                                   longest_match->VIEWSUBTREE) > 0) {
                    longest_match = ve;
                }
            }
            else {
                /*
                 * remember whether all the matches of this type are all
                 * EXCLUDED, whether all are INCLUDED
                 */

                if (ve->VIEWTYPE == INCLUDED) {
                    AllExcluded = 0;
                }
                if (ve->VIEWTYPE == EXCLUDED) {
                    AllIncluded = 0;
                }
            }
        }
    }
    if (longest_match == NULL || longest_match->VIEWTYPE == EXCLUDED) {
        AllIncluded = 0;
    }
    if (longest_match != NULL && longest_match->VIEWTYPE == INCLUDED) {
        AllExcluded = 0;
    }
    if (AllExcluded) {
        DPRINTF((APACCESS, "%s: all excluded\n", Fname));
        return -1;
    }
    if (!AllIncluded) {
        DPRINTF((APACCESS, "%s: included and excluded\n", Fname));
        return 0;
    }
    DPRINTF((APACCESS, "%s: all included\n", Fname));
    return 1;
}












