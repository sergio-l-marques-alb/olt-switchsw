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


#include <ctype.h>

#include <stdlib.h>

#include <malloc.h>



#include <string.h>


#ifdef WANT_SSCANF_PROTO
extern int sscanf();
#endif	/* WANT_SSCANF_PROTO */

#include "sr_snmp.h"
#include "mib_tree.h"


extern MIB_TREE   *default_mib_tree;

/*
 * select_behaviour
 *
 * Returns appropriate value based on behaviour constant.
 */
OID_TREE_ELE *
select_behaviour(node, behaviour)
    OID_TREE_ELE *node;
    int behaviour;
{
    if (node == NULL) {
        return NULL;
    }
    switch (behaviour) {
        case BEHAVIOUR_REJECT_PARENT:
            return(NULL);
        case BEHAVIOUR_ACCEPT_LEAF_PARENT:
            if ((node->type == NON_LEAF_TYPE) ||
                (node->type == AGGREGATE_TYPE)) {
                return(NULL);
            }
            return(node);
        case BEHAVIOUR_ACCEPT_ANY_PARENT:
            return(node);
    }
    return(NULL);
}

/*
 * GetMIBNodeFromOID:
 *
 * This function returns the node in the MIB tree for a particular OID.
 */
OID_TREE_ELE *
SR_GetMIBNodeFromOID(oid, behaviour)
    const OID *oid;
    int behaviour;
{
    MIB_TREE     *mt;
    OID_TREE_ELE *node, *parent;
    int           i;

    if (oid == NULL) {
        return(NULL);
    }
    if (oid->length == 0) {
        return(NULL);
    }
    if (oid->oid_ptr == NULL) {
        return(NULL);
    }

    mt = default_mib_tree;

/*
 *  Do some checking to catch cases where either the
 *  mib tree has not been read, or snmpinfo.dat is empty.
 *  Otherwise, sure as possums cross the road, we'll dereference
 *  a NULL pointer, with pretty much the same results.
 */
    if (mt == NULL) {
       return NULL;
    }
    node = mt->root;
    if (node == NULL) {
       return NULL;
    }

    parent = NULL;
    for (i=mt->prefix->length-1; i<oid->length; i++) {
        while (node->sid_value != oid->oid_ptr[i]) {
            node = node->next;
            if (node == NULL) {
                return(select_behaviour(parent, behaviour));
            }
        }
        if (i < oid->length-1) {
            if (node->first_descendent == NULL) {
                return(select_behaviour(node, behaviour));
            }
            parent = node;
            node = node->first_descendent;
            if (node == NULL) {
                return(select_behaviour(parent, behaviour));
            }
        }
    }
    return(node);
}

/*
 * GetMIBNodeFromOID:
 * LP_GetMIBNodeFromOID:
 * AP_GetMIBNodeFromOID:
 *
 * This function returns the node in the MIB tree for a particular OID.
 * LP means return a leaf parent if the node does not exist.
 * AP means return any parent if the node does not exist.
 */
OID_TREE_ELE *
GetMIBNodeFromOID(oid_ptr)
    const OID *oid_ptr;
{
    return(SR_GetMIBNodeFromOID(oid_ptr, BEHAVIOUR_REJECT_PARENT));
}
OID_TREE_ELE *
LP_GetMIBNodeFromOID(oid_ptr)
    const OID *oid_ptr;
{
    return(SR_GetMIBNodeFromOID(oid_ptr, BEHAVIOUR_ACCEPT_LEAF_PARENT));
}
OID_TREE_ELE *
AP_GetMIBNodeFromOID(oid_ptr)
    const OID *oid_ptr;
{
    return(SR_GetMIBNodeFromOID(oid_ptr, BEHAVIOUR_ACCEPT_ANY_PARENT));
}

/*
 * GetMIBNodeFromDot:
 * LP_GetMIBNodeFromDot:
 * AP_GetMIBNodeFromDot:
 *
 * This function returns the node in the MIB tree for a particular OID.
 * The OID is passed as a character string, which is converted using
 * MakeOIDFromDot.  If the conversion fails, NULL is returned.
 */
OID_TREE_ELE *
GetMIBNodeFromDot(name)
    const char *name;
{
    OID_TREE_ELE *node;
    OID          *oid_ptr;

    if (name == NULL) {
        return(NULL);
    }

    oid_ptr = MakeOIDFromDot(name);
    if (oid_ptr == NULL) {
        return(NULL);
    }
    node = SR_GetMIBNodeFromOID(oid_ptr, BEHAVIOUR_REJECT_PARENT);
    FreeOID(oid_ptr);
    return(node);
}
OID_TREE_ELE *
LP_GetMIBNodeFromDot(name)
    const char *name;
{
    OID_TREE_ELE *node;
    OID          *oid_ptr;

    if (name == NULL) {
        return(NULL);
    }

    oid_ptr = MakeOIDFromDot(name);
    if (oid_ptr == NULL) {
        return(NULL);
    }
    node = SR_GetMIBNodeFromOID(oid_ptr, BEHAVIOUR_ACCEPT_LEAF_PARENT);
    FreeOID(oid_ptr);
    return(node);
}
OID_TREE_ELE *
AP_GetMIBNodeFromDot(name)
    const char *name;
{
    OID_TREE_ELE *node;
    OID          *oid_ptr;

    if (name == NULL) {
        return(NULL);
    }

    oid_ptr = MakeOIDFromDot(name);
    if (oid_ptr == NULL) {
        return(NULL);
    }
    node = SR_GetMIBNodeFromOID(oid_ptr, BEHAVIOUR_ACCEPT_ANY_PARENT);
    FreeOID(oid_ptr);
    return(node);
}

/*
 * Stub functions.  Since the seclib version of the above functions do not
 * allocate storage, the stub function do not free anything.
 */
void
FreeOTE(ote)
    OID_TREE_ELE *ote;
{
}
