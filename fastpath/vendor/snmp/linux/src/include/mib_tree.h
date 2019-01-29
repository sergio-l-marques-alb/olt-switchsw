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

#ifndef SR_MIB_TREE_H
#define SR_MIB_TREE_H

#ifdef  __cplusplus
extern "C" {
#endif

#define MIB_HASH_TABLE_SIZE 512

typedef struct enumer {
    char           *name;
    SR_INT32             number;
    struct enumer  *next;
} ENUMER;

typedef struct mib_index {
    char *name;
    int implied; /* 0=no, 1=yes */
    struct mib_index *next;
} MIB_INDEX;

typedef struct _OID_TREE_ELE {
    SR_UINT32             sid_value;       /* branch from the parent */
    char                 *oid_name;        /* english descriptor */
    char                 *oid_number_str;  /* dotted form */
    short                 type;            /* variable type */
    short                 access;          /* access */
    struct enumer        *enumer;          /* english<->enumer translations */
    struct mib_index     *mib_index;       /* indexes */
    SR_UINT32             size_lower;      /* Valid sizes */
    SR_UINT32             size_upper;
    SR_INT32              range_lower;     /* Valid lengths */
    SR_INT32              range_upper;
    struct _OID_TREE_ELE *hash_next;       /* next member of hash collisions */
    struct _OID_TREE_ELE *hash_prev;       /* prev member of hash collisions */
    struct _OID_TREE_ELE *parent;          /* parent MIB variable */
    struct _OID_TREE_ELE *next, *prev;     /* next and previous siblings */
    struct _OID_TREE_ELE *first_descendent;/* first child node */
} OID_TREE_ELE;

typedef struct _MIB_TREE {
    char *name;             /* textual name of this MIB tree */
    OID *prefix;            /* OID prefix to the root of the tree */
    struct _MIB_TREE *next; /* pointer to the next MIB tree */
    struct _MIB_TREE *prev; /* pointer to the previous MIB tree */
    OID_TREE_ELE *root;     /* root of the tree */
    OID_TREE_ELE *hash_table[MIB_HASH_TABLE_SIZE];
} MIB_TREE;

/* Symbols for selecting collision behavior when loading/merging MIBs */
#define SR_COL_REPLACE 0
#define SR_COL_DISCARD 1

/* Symbols for controlling MIB tree traversal behaviour */
#define BEHAVIOUR_REJECT_PARENT      0
#define BEHAVIOUR_ACCEPT_LEAF_PARENT 1
#define BEHAVIOUR_ACCEPT_ANY_PARENT  2

/*
 * These TYPE definitions are only used in this structure and are used to
 * distinguish types that normally are treated as other types.  The
 * normal types for these are as follows:
 *      DISPLAY_STRING_TYPE  OCTET_PRIM_TYPE
 *      PHYS_ADDRESS_TYPE    OCTET_PRIM_TYPE
 *      NETWORK_ADDRESS_TYPE OCTET_PRIM_TYPE
 *      SERVICES_TYPE        INTEGER_TYPE
 *      BIT_STRING_TYPE     OCTET_PRIM_TYPE
 */
#define DISPLAY_STRING_TYPE  0x80
#define PHYS_ADDRESS_TYPE    0x81
#define NETWORK_ADDRESS_TYPE 0x82
#define SERVICES_TYPE        0x83
#define BIT_STRING_TYPE     0x86
/*
 * In addition, the type field may contain one of these.
 */
#define NON_LEAF_TYPE        0x84
#define AGGREGATE_TYPE       0x85
/*
 * These definitions are for the access of variables.
 */
#define READ_ONLY_ACCESS             0x01
#define READ_WRITE_ACCESS            0x02
#define READ_CREATE_ACCESS           0x03
#define NOT_ACCESSIBLE_ACCESS        0x04
#define ACCESSIBLE_FOR_NOTIFY_ACCESS 0x04 /* will change to 0x05 */

int CalculateMIBHash(const char *str, int len);

OID_TREE_ELE *NewOID_TREE_ELE(void);

void ReleaseOID_TREE_ELE(OID_TREE_ELE *ote);

MIB_TREE *CreateMIBTree(const char *name, const OID *prefix);

void AttachMIBTree(MIB_TREE *mib_tree);

void DetachMIBTree(MIB_TREE *mib_tree);

int ReadMIBFile(const char *filename, MIB_TREE *mib_tree, int collision);

int MergeMIBTrees(
    MIB_TREE *mib_tree_dst,
    MIB_TREE *mib_tree_src,
    int collision);

void FreeMIBTree(MIB_TREE *mib_tree);

void FreeOidTreeNodeStorage(void);

OID_TREE_ELE *AddOTEToMibTree(
    MIB_TREE *mib_tree,
    OID_TREE_ELE *ote,
    int collision);

void RemoveOTEFromMibTree(MIB_TREE *mib_tree, OID_TREE_ELE *ote);

OID_TREE_ELE *GetNextOTE(OID_TREE_ELE *node, SR_INT32 key);

OID *MakeOIDFragFromHFDot(const char *text_str);

OID *MakeOIDFromHFDot(const char *text_str);

int MakeHFDotFromOID(
    const OID *oid_ptr,
    char *buffer,
    int bufsize,
    int behavior);

int MakeHFIndexFromOID(
    const OID *oid_ptr,
    const OID *prev_oid,
    char *buffer,
    SR_INT32 bufsize,
    SR_INT32 behavior);

int NewMIBFromFile(const char *filename);

int
MergeMIBFromFile(const char *filename);

/* Prototypes for mib_lib.c */
OID_TREE_ELE *SR_GetMIBNodeFromOID(const OID *oid_ptr, int behaviour);

OID_TREE_ELE *SR_GetMIBNodeFromDot(const char *name, int behaviour);

OID_TREE_ELE *GetMIBNodeFromOID(const OID *oid_ptr);

OID_TREE_ELE *GetMIBNodeFromDot(const char *name);

OID_TREE_ELE *LP_GetMIBNodeFromOID(const OID *oid_ptr);

OID_TREE_ELE *LP_GetMIBNodeFromDot(const char *name);

OID_TREE_ELE *AP_GetMIBNodeFromOID(const OID *oid_ptr);

OID_TREE_ELE *AP_GetMIBNodeFromDot(const char *name);

void FreeOTE(OID_TREE_ELE *ote);

OID_TREE_ELE *select_behaviour(OID_TREE_ELE *, int);

OID_TREE_ELE *OIDTreeDive(
    OID_TREE_ELE *oid_tree_ptr, 
    SR_UINT32 *sid,
    int *sid_counter_ptr);

short TypeStringToShort(const char *str);

const char *TypeShortToString(int shrt);

short AccessStringToShort(const char *str);

const char *AccessShortToString(int shrt);


#ifdef  __cplusplus
}
#endif

#endif                                /* !defined SR_MIB_TREE_H */
