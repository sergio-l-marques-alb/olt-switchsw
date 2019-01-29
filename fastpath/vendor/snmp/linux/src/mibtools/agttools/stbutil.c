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

#include "mibtools.h"
#include "agent.h"

/* this routine returns a pointer to the family's storageType object if
 * one exists, or NULL if it doesn't */
struct OID_INFO *
storage_type_in_family(ptr)
    struct OID_INFO *ptr;
{
    struct OID_INFO *temp_ptr, *retptr;

    retptr = NULL;
    for (temp_ptr = ptr->next_family_entry; temp_ptr;
	 temp_ptr = temp_ptr->next_family_entry) {
	if (!strcmp(temp_ptr->oid_type, storageTypeString) ||
	    !strcmp(temp_ptr->oid_type, memoryTypeString)) {
	    /* this is a storage type entry */
	    retptr = temp_ptr;
	    break;
	}
    }

    /* return null if no storageType objects, nonzero otherwise */
    return (retptr);
}

int
settable_object_in_family(ptr)
    struct OID_INFO *ptr;
{
    struct OID_INFO *temp_ptr = NULL;

    for (temp_ptr = ptr->next_family_entry;
                           temp_ptr != NULL;
                           temp_ptr = temp_ptr->next_family_entry) {
        if (!temp_ptr->valid) continue;
        if ((strcmp(temp_ptr->oid_access, READ_WRITE_STR) == 0) ||
                (strcmp(temp_ptr->oid_access, WRITE_ONLY_STR) == 0) ||
                (strcmp(temp_ptr->oid_access, READ_CREATE_STR) == 0)) {
            return 1;
        }
    }
    return 0;
}

char *
last_object_in_family(ptr)
    struct OID_INFO *ptr;
{
    struct OID_INFO *temp_ptr = NULL, *last_ptr = NULL;
    struct OID_INFO *augments_list = NULL;

    if (ptr->augmented) {
        /*
         * if this group is augmented, generate a converter
         * for the items which augment the group
         */
        for (temp_ptr = sorted_root; temp_ptr;
                                     temp_ptr = temp_ptr->next_sorted) {
            if (temp_ptr->augments
                && strcmp(temp_ptr->augments, ptr->name) == 0) {
                augments_list = temp_ptr->next_family_entry;
                while (augments_list) {
                    if (augments_list->valid) {
                        last_ptr = augments_list;
                    }
                    augments_list = augments_list->next_family_entry;
                }
            }
        }
    } else {
        for (temp_ptr = ptr->next_family_entry;
                               temp_ptr != NULL;
                               temp_ptr = temp_ptr->next_family_entry) {
            if (temp_ptr->valid) {
                last_ptr = temp_ptr;
            }
        }
    }
    if (last_ptr) {
        return last_ptr->name;
    } else {
        return (char *) NULL;
    }
}

/* this routine returns a pointer to the family's rowStatus object if
 * one exists, or NULL if it doesn't */
struct OID_INFO *
row_status_in_family(ptr)
    struct OID_INFO *ptr;
{
    struct OID_INFO *temp_ptr, *retptr;
    struct _index_array *index_array;
    int             idx;

    /* if row status checking is disabled, return false */
    if (row_status == 0)
	return (NULL);

    /* this must be a valid family, look for an index */
    idx = get_index_array(ptr->name, &index_array);
    retptr = NULL;
    if (idx != 0) {
	for (temp_ptr = ptr->next_family_entry; temp_ptr;
	     temp_ptr = temp_ptr->next_family_entry) {
	    if (strcmp(temp_ptr->oid_type, rowStatusString) == 0) {
		/* this is a row status entry */
		retptr = temp_ptr;
		break;
	    }
	}
    }

    /* return zero if no row status, nonzero otherwise */
    return (retptr);
}

/* 
 * return the RowStatus object in this family if one exists. This routine
 * is just like row_status_in_family except that it works even when the
 * -row_status option is not used.
 */
struct OID_INFO *
row_status_in_family2(ptr)
    struct OID_INFO *ptr;
{
    struct OID_INFO *temp_ptr, *retptr;
    struct _index_array *index_array;
    int             idx;

    /* this must be a valid family, look for an index */
    idx = get_index_array(ptr->name, &index_array);
    retptr = NULL;
    if (idx != 0) {
	for (temp_ptr = ptr->next_family_entry; temp_ptr;
	     temp_ptr = temp_ptr->next_family_entry) {
	    if (strcmp(temp_ptr->oid_type, rowStatusString) == 0) {
		/* this is a row status entry */
		retptr = temp_ptr;
		break;
	    }
	}
    }

    /* return zero if no row status, nonzero otherwise */
    return (retptr);
}

/* this routine returns nonzero if the rowstatus textual convention is
 * used in the structures emitted into the current file */
int
row_status_in_file()
{
    struct OID_INFO *ptr;
    int             status;

    /* if row status checking is disabled, return false */
    if (row_status == 0)
	return 0;

    /* look for a family using rowstatus */
    status = 0;
    for (ptr = sorted_root; (ptr = next_valid_family(ptr)) != NULL;
	 ptr = ptr->next_sorted) {
	/* does this family use row status? */
	if (row_status_in_family(ptr) != NULL) {
	    /* yes, get out now */
	    status = 1;
	    break;
	}
    }

    return (status);
}

