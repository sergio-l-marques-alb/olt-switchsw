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

#include "sr_snmp.h"
#include "mib_tree.h"
#include "otewalk.h"

extern char *unassigned_text;

static  OID_TREE_ELE *_GetNextOTE(OID_TREE_ELE *node);

static  OID_TREE_ELE *_GetPrevOTE(OID_TREE_ELE *node);

static  OID_TREE_ELE  *_FindLastDescendent(OID_TREE_ELE *inode);


/*
 * _GetNextOTE
 *
 * Finds the next OID_TREE_ELE in the current MIB tree.
 */
static OID_TREE_ELE *
_GetNextOTE(node)
    OID_TREE_ELE *node;
{
keep_searching:
    if (node->first_descendent != NULL) {
        node = node->first_descendent;
        if (node->oid_name == unassigned_text) {
            goto keep_searching;
        }
        return(node);
    } else if (node->next != NULL) {
        node = node->next;
        if (node->oid_name == unassigned_text) {
            goto keep_searching;
        }
        return(node);
    } else {
        node = node->parent;
        while (node != NULL) {
            if (node->next == NULL) {
                node = node->parent;
            } else {
                node = node->next;
                if (node->oid_name == unassigned_text) {
                    goto keep_searching;
                }
                return(node);
            }
        }
        return(NULL);
    }
}


/*
 *  _FindLastDescendent()
 *
 *  Function:  Finds the last descendent of the input node.
 *
 *  Input:    inode - the OID_TREE_ELE for which to find the last descendent.
 *
 *  Outputs:  NULL  - if inode is NULL.
 *            inode - if inode has no descendents.
 *            Otherwise, this function finds the last descendent of the
 *            input node.
 */
static OID_TREE_ELE *
_FindLastDescendent(OID_TREE_ELE *inode)
{
    OID_TREE_ELE  *current = inode;

    /* --- Return if no input --- */
    if (current == NULL) {
        return NULL;
    }

walk_descendents:

    /* --- Return the current node if no descendents --- */
    if (current->first_descendent == NULL) {
        return current;
    }

    /* --- Point to the first descendent of the current node --- */
    current = current->first_descendent;

    /* --- Walk to the end of the descendent list --- */
    while (current->next != NULL) {
        current = current->next;
    }

    /*
     *  The end of the descendent list has been reached.  See if this
     *  current node also has decendents.  If so, walk its descendents.
     */

    if (current->first_descendent != NULL) {
        goto walk_descendents;
    }

    /*
     *  If this node does not have an English descriptor, then
     *  check either the previous sibling _or_ the parent node.
     */
    if (current->oid_name == unassigned_text) {

        /* --- Check for a previous sibling --- */
        if (current->prev != NULL) {

            /* --- Set the previous sibling to be the current node --- */
            current = current->prev;
        }
        else {
            /* --- Set the parent to be the current node --- */
            current = current->parent;
        }

        /* --- Walk to end of the descendent list for current node --- */
        goto walk_descendents;
    }

    /*
     *  Current node has an English descriptor, so return the current node.
     */
    return(current);
}


/*
 *  _GetPrevOTE()
 *
 *  Function:  Finds the previous OID_TREE_ELE in the current MIB tree.
 *
 */
static OID_TREE_ELE *
_GetPrevOTE(OID_TREE_ELE *inode)
{
    OID_TREE_ELE  *current = inode;

keep_searching:

    /* --- Does the current node have a previous sibling --- */
    if (current->prev != NULL) {

        /* --- Point to the previous sibling --- */
        current = current->prev;

        /* --- Walk to end of the sibling's descendent list --- */
        current = _FindLastDescendent(current);

        /*
         *  Keep searching if the last descendent does not 
         *  have an English descriptor.
         */
        if (current->oid_name == unassigned_text) {
            goto keep_searching;
        }

        /*
         *  The found last descendent does have an English
         *  descriptor, so return the found last descendent.
         */
        return(current);
    }

    /*
     *  The current node does not have a previous sibling.  This means
     *  the current node is the first decendent of it's parent.  Thus,
     *  to get the previous node, we must move higher up the MIB tree
     *  and examine the parent of the current node.
     *
     *  Return NULL if no previous parent exists, because we're at the
     *  root of the MIB tree.
     */
    if (current->parent == NULL) {
        return(NULL);
    }

    /* --- Point to the parent --- */
    current = current->parent;

    /*
     *  Keep searching if the last descendent does not 
     *  have an English descriptor.
     */
    if (current->oid_name == unassigned_text) {
        goto keep_searching;
    }

    /* --- Parent has an English descriptor, return the parent --- */
    return(current);
}


/*
 * GetNextOTE
 *
 * Finds the next OTE node in the MIB tree of a particular type:
 *
 * NEXT_ANY ---------- Matches any OTE node.
 * NEXT_ACCESSIBLE --- Matches read-only or read-write nodes.
 * NEXT_GROUP -------- Matches next nonLeaf with at least one child which
 *                     is a read-only leaf, read-write leaf, or Aggregate.
 * NEXT_SCALAR_GROUP - Matches next nonLeaf with at least one leaf child
 *                     which is read-only or read-write.
 * NEXT_TABLE -------- Matches next aggregate whose parent is a nonLeaf.
 * NEXT_SCALAR ------- Matches next read-only or read-write whose
 *                     parent is a nonLeaf.
 * NEXT_VECTOR ------- Matches next read-only or read-write whose
 *                     parent is an Aggregate.
 * PREV_xxx ---------- Same as the NEXT_xxx values, but searches backwards
 *                     through the tree.
 * PARENT_OTE -------- Matches parent of this node.
 * CHILD_OTE --------- Matches the first child of this node.
 * NEXT_SIBLING ------ Matches the next sibling of this node.
 * PREV_SIBLING ------ Matches the previous sibling of this node.
 *
 * Note that this function will NOT traverse nodes between MIB trees.
 */
OID_TREE_ELE *
GetNextOTE(node, key)
    OID_TREE_ELE *node;
    SR_INT32 key;
{
    OID_TREE_ELE *tmp;
    int found;

    if (node == NULL) {
        return NULL;
    }

    switch (key) {
    case NEXT_ANY:
        node = _GetNextOTE(node);
        break;
    case NEXT_ACCESSIBLE:
        while (1) {
            node = _GetNextOTE(node);
            if (node == NULL) {
                return(NULL);
            }
            if ((node->type == NON_LEAF_TYPE) ||
                (node->type == AGGREGATE_TYPE) ||
                (node->access == NOT_ACCESSIBLE_ACCESS)) {
                continue;
            }
            break;
        }
        break;
    case NEXT_NOT_ACCESSIBLE:
        while (1) {
            node = _GetNextOTE(node);
            if (node == NULL) {
                return(NULL);
            }
            if ((node->type == NON_LEAF_TYPE) ||
                (node->type == AGGREGATE_TYPE) ||
                (node->access != NOT_ACCESSIBLE_ACCESS)) {
                continue;
            }
            break;
        }
        break;
    case NEXT_GROUP:
        while (1) {
            node = _GetNextOTE(node);
            if (node == NULL) {
                return(NULL);
            }
            if (node->type != NON_LEAF_TYPE) {
                continue;
            }
            if (node->first_descendent == NULL) {
                continue;
            }
            found = 0;
            for (tmp=node->first_descendent; tmp!=NULL; tmp=tmp->next) {
                if ((tmp->type != NON_LEAF_TYPE) &&
                    (tmp->type != AGGREGATE_TYPE)) {
                    if ((tmp->access != READ_ONLY_ACCESS) &&
                        (tmp->access != READ_WRITE_ACCESS) &&
                        (tmp->access != READ_CREATE_ACCESS)) {
                        continue;
                    }
                    found = 1;
                    break;
                }
                if (node->type == AGGREGATE_TYPE) {
                    found = 1;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
        break;
    case NEXT_SCALAR_GROUP:
        while (1) {
            node = _GetNextOTE(node);
            if (node == NULL) {
                return(NULL);
            }
            if (node->type != NON_LEAF_TYPE) {
                continue;
            }
            if (node->first_descendent == NULL) {
                continue;
            }
            found = 0;
            for (tmp=node->first_descendent; tmp!=NULL; tmp=tmp->next) {
                if ((tmp->type != NON_LEAF_TYPE) &&
                    (tmp->type != AGGREGATE_TYPE)) {
                    if ((tmp->access != READ_ONLY_ACCESS) &&
                        (tmp->access != READ_WRITE_ACCESS) &&
                        (tmp->access != READ_CREATE_ACCESS)) {
                        continue;
                    }
                    found = 1;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
        break;
    case NEXT_TABLE:
        while (1) {
            node = _GetNextOTE(node);
            if (node == NULL) {
                return(NULL);
            }
            if (node->type != AGGREGATE_TYPE) {
                continue;
            }
            if (node->parent == NULL) {
                continue;
            }
            if (node->parent->type != NON_LEAF_TYPE) {
                continue;
            }
            break;
        }
        break;
    case NEXT_SCALAR:
        while (1) {
            node = _GetNextOTE(node);
            if (node == NULL) {
                return(NULL);
            }
            if ((node->type == NON_LEAF_TYPE) ||
                (node->type == AGGREGATE_TYPE) ||
                (node->access == NOT_ACCESSIBLE_ACCESS)) {
                continue;
            }
            if (node->parent == NULL) {
                continue;
            }
            if (node->parent->type != NON_LEAF_TYPE) {
                continue;
            }
            break;
        }
        break;
    case NEXT_VECTOR:
        while (1) {
            node = _GetNextOTE(node);
            if (node == NULL) {
                return(NULL);
            }
            if ((node->type == NON_LEAF_TYPE) ||
                (node->type == AGGREGATE_TYPE) ||
                (node->access == NOT_ACCESSIBLE_ACCESS)) {
                continue;
            }
            if (node->parent == NULL) {
                continue;
            }
            if (node->parent->type != AGGREGATE_TYPE) {
                continue;
            }
            break;
        }
        break;
    case PREV_ANY:
        node = _GetPrevOTE(node);
        break;
    case PREV_ACCESSIBLE:
        while (1) {
            node = _GetPrevOTE(node);
            if (node == NULL) {
                return(NULL);
            }
            if ((node->type == NON_LEAF_TYPE) ||
                (node->type == AGGREGATE_TYPE) ||
                (node->access == NOT_ACCESSIBLE_ACCESS)) {
                continue;
            }
            break;
        }
        break;
    case PREV_NOT_ACCESSIBLE:
        while (1) {
            node = _GetPrevOTE(node);
            if (node == NULL) {
                return(NULL);
            }
            if ((node->type == NON_LEAF_TYPE) ||
                (node->type == AGGREGATE_TYPE) ||
                (node->access != NOT_ACCESSIBLE_ACCESS)) {
                continue;
            }
            break;
        }
        break;
    case PREV_GROUP:
        while (1) {
            node = _GetPrevOTE(node);
            if (node == NULL) {
                return(NULL);
            }
            if (node->type != NON_LEAF_TYPE) {
                continue;
            }
            if (node->first_descendent == NULL) {
                continue;
            }
            found = 0;
            for (tmp=node->first_descendent; tmp!=NULL; tmp=tmp->next) {
                if ((tmp->type != NON_LEAF_TYPE) &&
                    (tmp->type != AGGREGATE_TYPE)) {
                    if ((tmp->access != READ_ONLY_ACCESS) &&
                        (tmp->access != READ_WRITE_ACCESS) &&
                        (tmp->access != READ_CREATE_ACCESS)) {
                        continue;
                    }
                    found = 1;
                    break;
                }
                if (node->type == AGGREGATE_TYPE) {
                    found = 1;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
        break;
    case PREV_SCALAR_GROUP:
        while (1) {
            node = _GetPrevOTE(node);
            if (node == NULL) {
                return(NULL);
            }
            if (node->type != NON_LEAF_TYPE) {
                continue;
            }
            if (node->first_descendent == NULL) {
                continue;
            }
            found = 0;
            for (tmp=node->first_descendent; tmp!=NULL; tmp=tmp->next) {
                if ((tmp->type != NON_LEAF_TYPE) &&
                    (tmp->type != AGGREGATE_TYPE)) {
                    if ((tmp->access != READ_ONLY_ACCESS) &&
                        (tmp->access != READ_WRITE_ACCESS) &&
                        (tmp->access != READ_CREATE_ACCESS)) {
                        continue;
                    }
                    found = 1;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
        break;
    case PREV_TABLE:
        while (1) {
            node = _GetPrevOTE(node);
            if (node == NULL) {
                return(NULL);
            }
            if (node->type != AGGREGATE_TYPE) {
                continue;
            }
            if (node->parent == NULL) {
                continue;
            }
            if (node->parent->type != NON_LEAF_TYPE) {
                continue;
            }
            break;
        }
        break;
    case PREV_SCALAR:
        while (1) {
            node = _GetPrevOTE(node);
            if (node == NULL) {
                return(NULL);
            }
            if ((node->type == NON_LEAF_TYPE) ||
                (node->type == AGGREGATE_TYPE) ||
                (node->access == NOT_ACCESSIBLE_ACCESS)) {
                continue;
            }
            if (node->parent == NULL) {
                continue;
            }
            if (node->parent->type != NON_LEAF_TYPE) {
                continue;
            }
            break;
        }
        break;
    case PREV_VECTOR:
        while (1) {
            node = _GetPrevOTE(node);
            if (node == NULL) {
                return(NULL);
            }
            if ((node->type == NON_LEAF_TYPE) ||
                (node->type == AGGREGATE_TYPE) ||
                (node->access == NOT_ACCESSIBLE_ACCESS)) {
                continue;
            }
            if (node->parent == NULL) {
                continue;
            }
            if (node->parent->type != AGGREGATE_TYPE) {
                continue;
            }
            break;
        }
        break;
    case PARENT_OTE:
        node = node->parent;
        break;
    case CHILD_OTE:
        node = node->first_descendent;
        break;
    case NEXT_SIBLING:
        node = node->next;
        break;
    case PREV_SIBLING:
        node = node->prev;
        break;
    default:
        node = NULL;
        break;
    }
    return(node);
}

SR_INT32
SameParentOTE(node1, node2)
    OID_TREE_ELE *node1;
    OID_TREE_ELE *node2;
{
    if ((node1 == NULL) || (node2 == NULL)) {
        return 0;
    }
    if (node1->parent == node2->parent) {
        return 1;
    }
    return 0;
}
