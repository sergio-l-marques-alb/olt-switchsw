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


/* functions shared by java.c and cli.c */ 

#include "mibtools.h"
#include "agent.h"

/*
 * next_augmenting_table: find the next table after aug_table_ptr that augments
 * base_table_ptr
 */
struct OID_INFO *
next_augmenting_table(struct OID_INFO *base_table_ptr,
                      struct OID_INFO *aug_table_ptr)
{
    if (augments_separate) {
        return NULL;
    }
 
    /* if the table is not augmented, return NULL; */
    if (!base_table_ptr->augmented) {
        return NULL;
    }
 
    while (aug_table_ptr != NULL) {
        /* if aug_table_ptr augments base_table_ptr */
        if ((aug_table_ptr->augments) &&
            (strcmp(aug_table_ptr->augments, base_table_ptr->name) == 0)) {
 
            return aug_table_ptr;
        }
        aug_table_ptr = aug_table_ptr->next_sorted;
    }
    return NULL;
}

/*
 * next_augmenting_object: find the next object after g1 that augments ptr
 */
struct OID_INFO *
next_augmenting_object(struct OID_INFO *augments_list)
{
    if (augments_separate) {
        return NULL;
    }
 
    while (augments_list) {
       if (augments_list->valid) {
           return augments_list;
       }
       augments_list = augments_list->next_family_entry;
    }
    return NULL;
}
 

int
IsGoodFamily(struct OID_INFO *ptr)
{
    if ((ptr->family_root == 0) || (!ptr->valid)) {
        return 0;
    }
 
    if (ptr->augments && !augments_separate) {
        return 0;
    }
 
    if (!IsValidFamily(ptr)) {
        return 0;
    }
 
    if (!IsReadableFamily(ptr)) {
        return 0;
    }
 
    return 1;
}

/*
 * next_unused_index: return the next index which is not already in the table
 * (i.e., an index that is defined in another table)
 */
struct _index_array *
next_unused_index(struct _index_array *index_array, 
                  struct _index_array *temp_index,
                  struct OID_INFO *ptr)
{
    int found;
    struct OID_INFO *temp_ptr;

    if (temp_index == NULL || index_array == NULL) {
        return NULL;
    }

    for (; temp_index && temp_index->name; temp_index++) {
        /*
         * first, see if this entry is already in the
         * file (i.e. if is in this table).
         */
        found = 0;
        temp_ptr = ptr->next_family_entry;
        while (temp_ptr) {
            if (strcmp(temp_ptr->name, temp_index->name) == 0) {
                 found = 1;
                 break;
            }
            temp_ptr = temp_ptr->next_family_entry;
        }
 
        if (!found) {
            return temp_index;
        }
    }
    return NULL;
}
