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

#include "sr_proto.h"
#include "sr_type.h"
#include "sr_snmp.h"
#include "sr_trans.h"
#include "sr_msg.h"
#include "mib_tree.h"
#include "mibinfo.h"

#include "sri/vbtable.h"

static int get_index_entries_from_ote
    SR_PROTOTYPE((OID_TREE_ELE *ote,
                  char ***name,
                  SR_INT32 **implied));

static int get_enumers_from_ote
    SR_PROTOTYPE((OID_TREE_ELE *ote,
                  char ***name,
                  SR_INT32 **number));

/*
 * get_enumers_from_ote
 *
 * Returns a list of enumerations.  Return value is -1 if passed NULL,
 * otherwise is number of enumerations.  If the object is not an
 * enumerated type, return value is 0.  The name and number arguments
 * will each be assigned an allocated array.  These arrays contain the
 * values.  For the name argument, each entry of the array is a pointer
 * to an allocated string.  The application is responsible for free-ing
 * all allocated memory.
 */
static int
get_enumers_from_ote(ote, name, number)
    OID_TREE_ELE *ote;
    char ***name;
    SR_INT32 **number;
{
    int count;
    struct enumer *e;

    if (ote == NULL) {
        goto fail;
    }
    if (ote->enumer == NULL) {
        *name = NULL;
        *number = NULL;
        return(0);
    }

    /* count number of enumerations */
    count = 0;
    e = ote->enumer;
    while (e != NULL) {
        count++;
        e = e->next;
    }

    /* Allocate arrays */
    *name = (char **)malloc(sizeof(char *) * count);
    if (*name == NULL) {
        goto fail;
    }
    *number = (SR_INT32 *)malloc(sizeof(SR_INT32) * count);
    if (*number == NULL) {
        free(*name);
        goto fail;
    }

    /* Insert values into arrays */
    count = 0;
    e = ote->enumer;
    while (e != NULL) {
        (*name)[count] = (char *)malloc(strlen(e->name) + 1);
        if ((*name)[count] == NULL) {
            return(count);
        }
        strcpy((*name)[count], e->name);
        (*number)[count] = e->number;
        count++;
        e = e->next;
    }
    return(count);

  fail:
    *name = NULL;
    *number = NULL;
    return -1;
}

/*
 * GetEnumerationsFromName
 *
 * Returns list of enumerations for a MIB object given a textual name.
 */
SR_INT32
GetEnumerationsFromName(object, name, number)
    char *object;
    char ***name;
    SR_INT32 **number;
{
    OID_TREE_ELE *ote;
    ote = LP_GetMIBNodeFromDot(object);
    return(get_enumers_from_ote(ote, name, number));
}

/*
 * GetEnumerationsFromOID
 *
 * Returns list of enumerations for a MIB object given an OID.
 */
SR_INT32
GetEnumerationsFromOID(object, name, number)
    OID *object;
    char ***name;
    SR_INT32 **number;
{
    OID_TREE_ELE *ote;
    ote = LP_GetMIBNodeFromOID(object);
    return(get_enumers_from_ote(ote, name, number));
}

/*
 * get_index_entries_from_ote
 *
 * Returns a list of index entries.  Return value is -1 if passed NULL,
 * otherwise is number of index entries.  If the object has no index
 * entries, return value is 0.  The name and implied arguments
 * will each be assigned an allocated array.  These arrays contain the
 * values.  For the name argument, each entry of the array is a pointer
 * to an allocated string.  The entries in the implied array indicate
 * whether each entry in names is an implied index.  A value of 0 means
 * it is not implied, a value of 1 means it is implied.  The application
 * is responsible for free-ing all allocated memory.
 */
static int
get_index_entries_from_ote(ote, name, implied)
    OID_TREE_ELE *ote;
    char ***name;
    SR_INT32 **implied;
{
    int count;
    struct mib_index *mi;

    if (ote == NULL) {
        goto fail;
    }

    while ((ote != NULL) && (ote->mib_index == NULL)) {
        if (ote->parent == NULL) {
            *name = NULL;
            *implied = NULL;
            return 0;
        }
        ote = ote->parent;
    }

    /* count number of index entries */
    count = 0;
    mi = ote->mib_index;
    while (mi != NULL) {
        count++;
        mi = mi->next;
    }

    /* Allocate arrays */
    *name = (char **)malloc(sizeof(char *) * count);
    if (*name == NULL) {
        goto fail;
    }
    *implied = (SR_INT32 *)malloc(sizeof(SR_INT32) * count);
    if (*implied == NULL) {
        free(*name);
        goto fail;
    }

    /* Insert values into arrays */
    count = 0;
    mi = ote->mib_index;
    while (mi != NULL) {
        (*name)[count] = (char *)malloc(strlen(mi->name) + 1);
        if ((*name)[count] == NULL) {
            return(count);
        }
        strcpy((*name)[count], mi->name);
        (*implied)[count] = mi->implied;
        count++;
        mi = mi->next;
    }
    return(count);

  fail:
    *name = NULL;
    *implied = NULL;
    return(-1);
}

/*
 * GetIndexEntriesFromName
 *
 * Returns list of enumerations for a MIB object given a textual name.
 */
SR_INT32
GetIndexEntriesFromName(object, name, implied)
    char *object;
    char ***name;
    SR_INT32 **implied;
{
    OID_TREE_ELE *ote;
    ote = LP_GetMIBNodeFromDot(object);
    return(get_index_entries_from_ote(ote, name, implied));
}

/*
 * GetIndexEntriesFromOID
 *
 * Returns list of enumerations for a MIB object given an OID.
 */
SR_INT32
GetIndexEntriesFromOID(object, name, implied)
    OID *object;
    char ***name;
    SR_INT32 **implied;
{
    OID_TREE_ELE *ote;
    ote = LP_GetMIBNodeFromOID(object);
    return(get_index_entries_from_ote(ote, name, implied));
}

/*
 * GetSizeSpecFromName
 *
 * Returns size specification for a MIB object given a textual name.
 * This is the range of legal lengths for a MIB object with a length.
 * Return value : -1 is failure, 0 is success.
 */
SR_INT32
GetSizeSpecFromName(object, size_lower, size_upper)
    char *object;
    SR_UINT32 *size_lower, *size_upper;
{
    OID_TREE_ELE *ote;
    ote = LP_GetMIBNodeFromDot(object);
    if (ote == NULL) {
        return(-1);
    }
    if (ote->type == OCTET_PRIM_TYPE || ote->type == DISPLAY_STRING_TYPE) {
        *size_lower = ote->size_lower;
        *size_upper = ote->size_upper;
        return(0);
    }
    return(-1);
}

/*
 * GetSizeSpecFromOID
 *
 * Returns list of enumerations for a MIB object given an OID.
 * This is the range of legal lengths for a MIB object with a length.
 * Return values as for GetSizeSpecFromName.
 */
SR_INT32
GetSizeSpecFromOID(object, size_lower, size_upper)
    OID *object;
    SR_UINT32 *size_lower, *size_upper;
{
    OID_TREE_ELE *ote;
    ote = LP_GetMIBNodeFromOID(object);
    if (ote == NULL) {
        return(-1);
    }
    if (ote->type == OCTET_PRIM_TYPE || ote->type == DISPLAY_STRING_TYPE) {
        *size_lower = ote->size_lower;
        *size_upper = ote->size_upper;
        return(0);
    }
    return(-1);
}

/*
 * GetRangeSpecFromName
 *
 * Returns range specification for a MIB object given a textual name.
 * This is the range of legal values for an integral MIB variable.
 * Return value : -1 is failure, 0 is success.
 */
SR_INT32
GetRangeSpecFromName(object, range_lower, range_upper)
    char *object;
    SR_INT32 *range_lower, *range_upper;
{
    OID_TREE_ELE *ote;
    ote = LP_GetMIBNodeFromDot(object);
    if (ote == NULL) {
        return(-1);
    }
    if (ote->type == INTEGER_TYPE) {
        *range_lower = ote->range_lower;
        *range_upper = ote->range_upper;
        return(0);
    }
    return(-1);
}

/*
 * GetRangeSpecFromOID
 *
 * Returns list of enumerations for a MIB object given an OID.
 * This is the range of legal values for an integral MIB variable.
 * Return values as for GetRangeSpecFromName.
 */
SR_INT32
GetRangeSpecFromOID(object, range_lower, range_upper)
    OID *object;
    SR_INT32 *range_lower, *range_upper;
{
    OID_TREE_ELE *ote;
    ote = LP_GetMIBNodeFromOID(object);
    if (ote == NULL) {
        return(-1);
    }
    if (ote->type == INTEGER_TYPE) {
        *range_lower = ote->range_lower;
        *range_upper = ote->range_upper;
        return(0);
    }
    return(-1);
}

VarBind *
GetTableColumns(table)
    OID *table;
{
    OID_TREE_ELE   *ote;
    VarBind        *vb_list = NULL, *vb_ptr = NULL;
    OID            *oid_ptr;
    long            columns;

    if (table == NULL) {
        goto done;
    }

    /*
     * Translate table/instance to a varbind list to be used for the requests.
     * If the OID specified in 'table' is not in a loaded MIB, or is not an
     * Aggregate type, return failure.
     */
    columns = 0;
    ote = LP_GetMIBNodeFromOID(table);
    if (ote == NULL) {
        goto done;
    }
    if (ote->type != AGGREGATE_TYPE) {
        goto done;
    }
    ote = ote->first_descendent;
    if (ote->type == AGGREGATE_TYPE) {
        ote = ote->first_descendent;
    }
    vb_list = NULL;
    while (ote != NULL) {
        if ((ote->access == READ_ONLY_ACCESS) ||
            (ote->access == READ_WRITE_ACCESS) ||
            (ote->access == READ_CREATE_ACCESS)) {
            oid_ptr = MakeOIDFromDot(ote->oid_number_str);
            if (oid_ptr == NULL) {
                if (vb_list != NULL) {
                    FreeVarBindList(vb_list);
                }
                vb_list = NULL;
                goto done;
            }
            if (vb_list == NULL) {
                vb_list = vb_ptr = MakeVarBindWithNull(oid_ptr, NULL);
            } else {
                vb_ptr->next_var = MakeVarBindWithNull(oid_ptr, NULL);
                if (vb_ptr->next_var != NULL) {
                    vb_ptr = vb_ptr->next_var;
                    vb_ptr->next_var = NULL;
                }
            }
            FreeOID(oid_ptr);
            if (vb_ptr == NULL) {
                if (vb_list != NULL) {
                    FreeVarBindList(vb_list);
                }
                vb_list = NULL;
                goto done;
            }
        }
        ote = ote->next;
    }

  done:
    return vb_list;
}
