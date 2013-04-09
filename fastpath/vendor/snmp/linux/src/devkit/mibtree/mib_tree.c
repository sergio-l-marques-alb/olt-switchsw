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



#include <ctype.h>

#include <malloc.h>


#include <stdlib.h>

#ifdef WANT_SSCANF_PROTO
extern int sscanf();
#endif	/* WANT_SSCANF_PROTO */

#include "sr_snmp.h"
#include "sr_cfg.h"
#include "mib_tree.h"
#include "oid_lib.h"
#include "diag.h"
SR_FILENAME


MIB_TREE       *default_mib_tree = NULL, *default_mib_tree_tail = NULL;
OID_TREE_ELE   *oid_tree_node_storage = NULL;
char           *unassigned_text = "UNASSIGNED";

/*
 * CalculateMIBHash:
 *
 * Generate a hash value from a string containing a MIB variable name.
 * If len is -1, strlen is used for the length of the string.  Otherwise,
 * len should be the length over which to calculate the hash.
 */
int
CalculateMIBHash(str, len)
    const char *str;
    int len;
{
    int i, hash;

    if (len == -1) {
        len = strlen(str);
    }
    hash = 0;
    for (i=0; i<len; i++) {
        hash = (hash + str[i]) % MIB_HASH_TABLE_SIZE;
    }
    return hash;
}

/*
 * NewOID_TREE_ELE:
 *
 * Removes an OID_TREE_ELE structure from oid_tree_node_storage if any
 * exist, and returns it.  Otherwise, allocates a new one and returns it.
 */
OID_TREE_ELE *
NewOID_TREE_ELE()
{
    OID_TREE_ELE *newote;
    if (oid_tree_node_storage != NULL) {
        newote = oid_tree_node_storage;
        oid_tree_node_storage = oid_tree_node_storage->next;
    } else {
        newote = (OID_TREE_ELE *)malloc(sizeof(OID_TREE_ELE));
        if (newote == NULL) {
            DPRINTF((APERROR, "NewOID_TREE_ELE: malloc failed\n"));
            return NULL;
        }
#ifdef SR_CLEAR_MALLOC
        else {
            memset(newote, 0, sizeof(*newote));
        }
#endif	/* SR_CLEAR_MALLOC */
    }
    newote->next = NULL;
    return newote;
}

/*
 * ReleaseOID_TREE_ELE:
 *
 * Places an OID_TREE_ELE instance into oid_tree_node_storage, and free's
 * its contents.
 */
void
ReleaseOID_TREE_ELE(ote)
    OID_TREE_ELE *ote;
{
    ENUMER *ke;
    struct mib_index *ki;

    if (ote->oid_name) {
        if (ote->oid_name != unassigned_text) {
            free(ote->oid_name);
        }
    }
    if (ote->oid_number_str) {
        free(ote->oid_number_str);
    }
    while (ote->enumer) {
        ke = ote->enumer;
        ote->enumer = ote->enumer->next;
        free(ke->name);
        free(ke);
    }
    while (ote->mib_index) {
        ki = ote->mib_index;
        ote->mib_index = ote->mib_index->next;
        free(ki->name);
        free(ki);
    }
    memset(ote, 0, sizeof(*ote));
    ote->next = oid_tree_node_storage;
    oid_tree_node_storage = ote;
}

/*
 * FreeOidTreeNodeStorage
 *
 * This function frees all OID_TREE_ELE structures stored in the
 * oid_tree_node_storage list.
 */
void
FreeOidTreeNodeStorage()
{
    OID_TREE_ELE *ote;

    while (oid_tree_node_storage != NULL) {
        ote = oid_tree_node_storage;
        oid_tree_node_storage = oid_tree_node_storage->next;
        free(ote);
    }
}

/*
 * CreateMIBTree:
 *
 * Creates a new MIB tree, with no nodes.  The name and prefix are cloned.
 *
 * Note that the pointer returned should be treated as a token, and the
 * underlying data structure should be accessed only via API calls.
 */
MIB_TREE *
CreateMIBTree(name, prefix)
    const char *name;
    const OID *prefix;
{
    MIB_TREE *mib_tree = NULL;

    mib_tree = (MIB_TREE *)malloc(sizeof(MIB_TREE));
    if (mib_tree == NULL) {
        goto fail;
    }
#ifdef SR_CLEAR_MALLOC
    memset(mib_tree, 0, sizeof(*mib_tree));
#endif	/* SR_CLEAR_MALLOC */

    mib_tree->name = (char *)malloc(strlen(name) + 1);
    if (mib_tree->name == NULL) {
        goto fail;
    }
    strcpy(mib_tree->name, name);

    mib_tree->prefix = CloneOID(prefix);
    if (mib_tree->prefix == NULL) {
        goto fail;
    }

    return mib_tree;

  fail:
    if (mib_tree != NULL) {
        if (mib_tree->name != NULL) {
            free(mib_tree->name);
        }
        if (mib_tree->prefix != NULL) {
            FreeOID(mib_tree->prefix);
        }
        free(mib_tree);
    }
    return NULL;
}

/*
 * AttachMIBTree:
 *
 * Attaches a MIB tree to the default MIB tree.  Attaching a MIB tree in
 * this manner will make its contents available to various other API
 * calls (such as MakeOIDFromDot).
 *
 * Note, that when looking up
 * a MIB variable by OID, the attached trees are searched in order of
 * descending prefix length, but only the first matching tree is searched.
 * So, if a MIB tree with a long prefix overlaps part of a MIB tree with a
 * short prefix, the second tree will never be searched for MIB variables
 * whose OID matches the first tree's prefix.  This means that applications
 * should be careful not to load and attach overlapping trees.
 *
 * By overlapping trees, we mean trees with nodes for the same variable,
 * not trees where one tree is rooted within another tree.
 */
void
AttachMIBTree(mib_tree)
    MIB_TREE *mib_tree;
{
    MIB_TREE *tmp;

    if (default_mib_tree == NULL) {
        default_mib_tree = mib_tree;
        default_mib_tree_tail = mib_tree;
        mib_tree->next = NULL;
        mib_tree->prev = NULL;
        return;
    }

    tmp = default_mib_tree;
    while (tmp != NULL) {
        if (mib_tree->prefix->length > tmp->prefix->length) {
            break;
        }
        tmp = tmp->next;
    }

    if (tmp == NULL) {
        default_mib_tree_tail->next = mib_tree;
        mib_tree->next = NULL;
        mib_tree->prev = default_mib_tree_tail;
        default_mib_tree_tail = mib_tree;
        return;
    }

    mib_tree->next = tmp;
    mib_tree->prev = tmp->prev;
    if (tmp == default_mib_tree) {
        default_mib_tree = mib_tree;
        default_mib_tree = mib_tree;
    } else {
        tmp->prev->next = mib_tree;
        tmp->prev = mib_tree;
    }
}

/*
 * DetachMIBTree:
 *
 * Detaches a MIB tree from the default MIB tree.
 */
void
DetachMIBTree(mib_tree)
    MIB_TREE *mib_tree;
{
    if (mib_tree->prev == NULL) {
        default_mib_tree = mib_tree->next;
    } else {
        mib_tree->prev->next = mib_tree->next;
    }
    if (mib_tree->next == NULL) {
        default_mib_tree_tail = mib_tree->prev;
    } else {
        mib_tree->next->prev = mib_tree->prev;
    }
    mib_tree->next = NULL;
    mib_tree->prev = NULL;
}

/*
 * MergeMIBTrees:
 *
 * Merges the contents of two MIB trees (which must be separate trees).  The
 * mib_tree_dst argument may be NULL, which means to merge mib_tree_src into
 * the default MIB tree.  The collision argument is used as in ReadMIBFile.
 *
 * The return value is -1 on failure, 0 on success.
 */
int
MergeMIBTrees(mib_tree_dst, mib_tree_src, collision)
    MIB_TREE *mib_tree_dst;
    MIB_TREE *mib_tree_src;
    int collision;
{
    OID_TREE_ELE *cur, *next;

    if ((mib_tree_src->next != NULL) || (mib_tree_src->prev != NULL)) {
        DPRINTF((APWARN, "Must detach source tree before merging\n"));
        return -1;
    }

    cur = mib_tree_src->root;
    while (cur != NULL) {
        if (cur->first_descendent != NULL) {
            cur = cur->first_descendent;
            continue;
        }
        if (cur->next != NULL) {
            cur = cur->next;
            continue;
        }
        if (cur->prev != NULL) {
            next = cur->prev;
            next->next = NULL;
        } else if (cur->parent != NULL) {
            next = cur->parent;
            next->first_descendent = NULL;
        } else {
            next = NULL;
        }
        cur->hash_next = NULL;
        cur->parent = NULL;
        cur->prev = NULL;
        if (cur->oid_name == unassigned_text) {
            ReleaseOID_TREE_ELE(cur);
        } else {
            AddOTEToMibTree(mib_tree_dst, cur, collision);
        }
        cur = next;
    }
    FreeMIBTree(mib_tree_src);
    return 0;
}

/*
 * FreeMIBTree:
 *
 * Free all allocated storage used by a MIB tree.
 */
void
FreeMIBTree(mib_tree)
    MIB_TREE *mib_tree;
{
    OID_TREE_ELE *cur, *next;

    cur = mib_tree->root;
    while (cur != NULL) {
        if (cur->first_descendent != NULL) {
            cur = cur->first_descendent;
            continue;
        }
        if (cur->next != NULL) {
            cur = cur->next;
            continue;
        }
        if (cur->prev != NULL) {
            next = cur->prev;
            next->next = NULL;
        } else if (cur->parent != NULL) {
            next = cur->parent;
            next->first_descendent = NULL;
        } else {
            next = NULL;
        }
        cur->hash_next = NULL;
        cur->parent = NULL;
        cur->prev = NULL;
        ReleaseOID_TREE_ELE(cur);
        cur = next;
    }
    free(mib_tree->name);
    FreeOID(mib_tree->prefix);
    free(mib_tree);
}

/*
 * AddOTEToMibTree:
 *
 * Adds an OID_TREE_ELE node to a MIB tree.  If collision is set to
 * SR_COL_DISCARD, the return value is the value of ote if the node was
 * successfully inserted, and NULL if there was a collision.  If collision
 * is set to SR_COL_REPLACE, the return value is the value of ote if
 * the node was inserted without any collision, and if there is a collision,
 * the return value is the node with which the collision occured.  In this
 * case, the contents of the two nodes will have been swapped, and the
 * caller should call ReleaseOID_TREE_ELE.
 */
OID_TREE_ELE *
AddOTEToMibTree(mib_tree, ote, collision)
    MIB_TREE *mib_tree;
    OID_TREE_ELE *ote;
    int collision;
{
    OID *oid;
    int pos, ins;
    OID_TREE_ELE *cur, *par, *ele;
    char *swap_oid_name, *swap_oid_number_str;
    ENUMER *swap_enumer;
    struct mib_index *swap_mib_index;
    int hash_value;
    short swap_type, swap_access;
    SR_UINT32 swap_size_lower, swap_size_upper;
    SR_INT32 swap_range_lower, swap_range_upper;
#ifdef REPLACE_DUPLICATE_OBJECT_NAMES
    SR_UINT32 swap_sid_value;
    OID *tmp_oid = NULL;
#endif /* REPLACE_DUPLICATE_OBJECT_NAMES */

    if (mib_tree == NULL) {
        mib_tree = default_mib_tree;
    }

    oid = MakeOIDFromDot(ote->oid_number_str);
    if (oid == NULL) {
        return NULL;
    }

    par = NULL;
    cur = mib_tree->root;
    ins = 0;
    for (pos=MAX(0,mib_tree->prefix->length-1); !ins;) {
        ins = (pos == (oid->length - 1));
        if (cur == NULL) {
            if (ins) {
                ele = ote;
                ele->sid_value = oid->oid_ptr[pos];
            } else {
                ele = NewOID_TREE_ELE();
                ele->oid_name = unassigned_text;
                ele->sid_value = oid->oid_ptr[pos];
            }
            if (par == NULL) {
                mib_tree->root = ele;
            } else {
                par->first_descendent = ele;
                ele->parent = par;
            }
            cur = ele;
            continue;
        }
        if (cur->sid_value == oid->oid_ptr[pos]) {
            if (ins) {
                /* COLLISION */
                if (collision == SR_COL_REPLACE) {
                    if (cur->oid_name != unassigned_text) {
                        if (cur->hash_prev == NULL) {
                            hash_value = CalculateMIBHash(cur->oid_name, -1);
                            mib_tree->hash_table[hash_value] = cur->hash_next;
                        } else {
                            cur->hash_prev->hash_next = cur->hash_next;
                        }
                        if (cur->hash_next != NULL) {
                            cur->hash_next->hash_prev = cur->hash_prev;
                        }
                        cur->hash_prev = NULL;
                        cur->hash_next = NULL;
                    }

                    swap_oid_name = ote->oid_name;
                    swap_oid_number_str = ote->oid_number_str;
                    swap_enumer = ote->enumer;
                    swap_mib_index = ote->mib_index;
                    swap_type = ote->type;
                    swap_access = ote->access;
                    swap_size_lower = ote->size_lower;
                    swap_size_upper = ote->size_upper;
                    swap_range_lower = ote->range_lower;
                    swap_range_upper = ote->range_upper;

                    ote->oid_name = cur->oid_name;
                    ote->oid_number_str = cur->oid_number_str;
                    ote->enumer = cur->enumer;
                    ote->mib_index = cur->mib_index;
                    ote->type = cur->type;
                    ote->access = cur->access;
                    ote->size_lower = cur->size_lower;
                    ote->size_upper = cur->size_upper;
                    ote->range_lower = cur->range_lower;
                    ote->range_upper = cur->range_upper;

                    cur->oid_name = swap_oid_name;
                    cur->oid_number_str = swap_oid_number_str;
                    cur->enumer = swap_enumer;
                    cur->mib_index = swap_mib_index;
                    cur->type = swap_type;
                    cur->access = swap_access;
                    cur->size_lower = swap_size_lower;
                    cur->size_upper = swap_size_upper;
                    cur->range_lower = swap_range_lower;
                    cur->range_upper = swap_range_upper;

                    ote = cur;
                    goto add_to_hash_table;
                } else {
                    FreeOID(oid);
                    return NULL;
                }
            }
            par = cur;
            cur = cur->first_descendent;
            pos++;
            continue;
        }

#ifdef REPLACE_DUPLICATE_OBJECT_NAMES
        /*
         * Check for duplicate MIB object names before inserting it into
         * the tree. Only do so if OID number strings don't match.
         */
        if ( (ote->oid_name != NULL) &&
             (cur->oid_name != NULL) &&
             (ote->oid_number_str != NULL) && (cur->oid_number_str != NULL) &&
             ((strcmp(ote->oid_number_str, cur->oid_number_str)) != 0) &&
             ((strcmp(ote->oid_name, cur->oid_name)) == 0) &&
             ((tmp_oid = MakeOIDFromDot(ote->oid_name)) != NULL) ) {

            /* COLLISION */
            if (collision == SR_COL_REPLACE) {
                DPRINTF((APWARN, "Warning, duplicate MIB object name %s detected in tree %s (Replacing %s with %s)\n", cur->oid_name, mib_tree->name, cur->oid_number_str, ote->oid_number_str));
 
                /* Reassign hash ptrs */
                if (cur->oid_name != unassigned_text) {
                    if (cur->hash_prev == NULL) {
                        hash_value = CalculateMIBHash(cur->oid_name, -1);
                        mib_tree->hash_table[hash_value] = cur->hash_next;
                    } else {
                        cur->hash_prev->hash_next = cur->hash_next;
                    }
                    if (cur->hash_next != NULL) {
                        cur->hash_next->hash_prev = cur->hash_prev;
                    }
                    cur->hash_prev = NULL;
                    cur->hash_next = NULL;
                }

                /* Assign sid_value for new object before doing swap */
                ote->sid_value = oid->oid_ptr[pos];


                /* Swap values between ote and cur */
                swap_oid_name = ote->oid_name;
                swap_oid_number_str = ote->oid_number_str;
                swap_enumer = ote->enumer;
                swap_mib_index = ote->mib_index;
                swap_type = ote->type;
                swap_access = ote->access;
                swap_size_lower = ote->size_lower;
                swap_size_upper = ote->size_upper;
                swap_range_lower = ote->range_lower;
                swap_range_upper = ote->range_upper;
                swap_sid_value = ote->sid_value;

                ote->oid_name = cur->oid_name;
                ote->oid_number_str = cur->oid_number_str;
                ote->enumer = cur->enumer;
                ote->mib_index = cur->mib_index;
                ote->type = cur->type;
                ote->access = cur->access;
                ote->size_lower = cur->size_lower;
                ote->size_upper = cur->size_upper;
                ote->range_lower = cur->range_lower;
                ote->range_upper = cur->range_upper;
                ote->sid_value = cur->sid_value;

                cur->oid_name = swap_oid_name;
                cur->oid_number_str = swap_oid_number_str;
                cur->enumer = swap_enumer;
                cur->mib_index = swap_mib_index;
                cur->type = swap_type;
                cur->access = swap_access;
                cur->size_lower = swap_size_lower;
                cur->size_upper = swap_size_upper;
                cur->range_lower = swap_range_lower;
                cur->range_upper = swap_range_upper;
                cur->sid_value = swap_sid_value;

                ote = cur;
                FreeOID(tmp_oid);
                tmp_oid = NULL;
                goto add_to_hash_table;
            }
            else {
                FreeOID(tmp_oid);
                FreeOID(oid);
                return NULL;
            }
        }
#endif /* REPLACE_DUPLICATE_OBJECT_NAMES */

        if (cur->sid_value < oid->oid_ptr[pos]) {
            if (cur->next == NULL) {
                if (ins) {
                    cur->next = ote;
                    ote->prev = cur;
                    ote->parent = cur->parent;
                    ote->sid_value = oid->oid_ptr[pos];
                    continue;
                } else {
                    cur->next = NewOID_TREE_ELE();
                    cur->next->prev = cur;
                    cur->next->parent = par;
                    cur = cur->next;
                    cur->oid_name = unassigned_text;
                    cur->sid_value = oid->oid_ptr[pos];
                }
            } else {
                cur = cur->next;
                ins = 0;
            }
            continue;
        }
        if (cur->sid_value > oid->oid_ptr[pos]) {
            if (ins) {
                ele = ote;
                ele->sid_value = oid->oid_ptr[pos];
            } else {
                ele = NewOID_TREE_ELE();
                ele->oid_name = unassigned_text;
                ele->sid_value = oid->oid_ptr[pos];
            }
            ele->parent = par;
            ele->next = cur;
            ele->prev = cur->prev;
            if (cur->prev == NULL) {
                if (par == NULL) {
                    mib_tree->root = ele;
                } else {
                    par->first_descendent = ele;
                }
            } else {
                cur->prev->next = ele;
            }
            cur->prev = ele;
            cur = ele;
            continue;
        }
    }

  add_to_hash_table:

    /* add to hash table */
    hash_value = CalculateMIBHash(ote->oid_name, -1);
    if (mib_tree->hash_table[hash_value] == NULL) {
        mib_tree->hash_table[hash_value] = ote;
    } else {
        cur = mib_tree->hash_table[hash_value];
        while (cur->hash_next != NULL) {
            if (!strcmp(cur->oid_name, ote->oid_name)) {
                DPRINTF((APWARN, "Warning, inserting duplicate MIB object name %s into tree %s (%s %s)\n", cur->oid_name, mib_tree->name, cur->oid_number_str, ote->oid_number_str));
            }
            cur = cur->hash_next;
        }
        if (!strcmp(cur->oid_name, ote->oid_name)) {
            DPRINTF((APWARN, "Warning, inserting duplicate MIB object name %s into tree %s (%s %s)\n", cur->oid_name, mib_tree->name, cur->oid_number_str, ote->oid_number_str));
        }
        cur->hash_next = ote;
        ote->hash_prev = cur;
    }

    FreeOID(oid);
    return ote;
}

/*
 * RemoveOTEFromMibTree:
 *
 * This function will remove an OID_TREE_ELE structure from a MIB tree.
 * The node is not actually free'd (no call to ReleaseOID_TREE_ELE is made).
 */
/* if the ote is not in this mib_tree, there can be problems */
void
RemoveOTEFromMibTree(mib_tree, ote)
    MIB_TREE *mib_tree;
    OID_TREE_ELE *ote;
{
    OID_TREE_ELE *ph;
    int hash, release = 0;

    if (mib_tree == NULL) {
        mib_tree = default_mib_tree;
    }

    while (ote != NULL) {
        /* First remove the node from the hash list */
        if (ote->hash_prev == NULL) {
            hash = CalculateMIBHash(ote->oid_name, -1);
            mib_tree->hash_table[hash] = ote->hash_next;
        } else {
            ote->hash_prev->hash_next = ote->hash_next;
        }
        if (ote->hash_next != NULL) {
            ote->hash_next->hash_prev = ote->hash_prev;
        }
        ote->hash_prev = NULL;
        ote->hash_next = NULL;

        if (ote->first_descendent != NULL) {
            /* Must create a place holder */
            ph = NewOID_TREE_ELE();
            ph->sid_value = ote->sid_value;
            ph->oid_name = unassigned_text;
            ph->parent = ote->parent;
            ph->next = ote->next;
            ph->prev = ote->prev;
            ph->first_descendent = ote->first_descendent;
            if (release) {
                ReleaseOID_TREE_ELE(ote);
            }
            ote = NULL;
        } else {
            ph = ote->parent;
            if (ote->prev == NULL) {
                if (ph != NULL) {
                    ph->first_descendent = ote->next;
                } else {
                    mib_tree->root = ote->next;
                }
            } else {
                ote->prev->next = ote->next;
            }
            if (ote->next != NULL) {
                ote->next->prev = ote->prev;
            }
            if (release) {
                ReleaseOID_TREE_ELE(ote);
            }
            ote = NULL;
            if (ph != NULL) {
                if ((ph->oid_name == unassigned_text) &&
                    (ph->first_descendent == NULL)) {
                    ote = ph;
                    release = 1;
                }
            }
        }
    }
}

