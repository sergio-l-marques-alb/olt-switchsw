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

#ifndef SR_METHOD_H
#define SR_METHOD_H

#ifdef  __cplusplus
extern "C" {
#endif


/* search type (i.e., get or get-next) */
#define NEXT       		GET_NEXT_REQUEST_TYPE
#define EXACT      		GET_REQUEST_TYPE

/* The following #define will cause the core agent to treat
 * write-only variables as read-write. The MIB compiler will
 * automatically generate code to prevent the write-only variable
 * from being read. This will correctly implement write-only.
 * Note that this in only for backward compatibility with SNMPv1
 * and write-only should not be used in newly defined MIBs since
 * it is not allowed in SNMPv2
 */
#define SR_WRITE_ONLY             SR_READ_WRITE

/*
 * Definitions for SET actions.
 *     1) Set function values.
 *     2) Do list definition.
 *     3) Valid bit-field macros.
 * We've done some name changes here but have preserved
 * backward compatibility by keeping the same names as
 * well.  All new code should use the SR_ variants.
 */
#define SR_UNUSED                    0

#define SR_UNKNOWN                   1

#define SR_DELETE                    2

#define SR_ADD_MODIFY                3
#define ADD_MODIFY                   3

#define SR_CREATE_AND_GO_STATE       4
#define CREATE_AND_GO_STATE          4

#define SR_NOT_READY_STATE           5
#define NOT_READY_STATE              5

#define SR_NOT_IN_SERVICE_STATE      6
#define NOT_IN_SERVICE_STATE         6

#define SR_CLOCK_SET_BACKWARDS_STATE 7
#define CLOCK_SET_BACKWARDS_STATE    7


#define SR_PENDING_STATE             9

#define SR_REPLACE                  10

#define SR_NO_OP                    11

#define INCONSISTENT_VALUE_STATE    12   /* same as INCONSISTENT_VALUE_ERROR */
#define INCONSISTENT_NAME_STATE     18   /* same as INCONSISTENT_NAME_ERROR */


/*
 * These macros are used for manipulating the valid bits of a MIB structure.
 * 
 * NOTE: These macros require that the actual valid bits array be passed into
 *       the function call. These macros, particularly those that call sizeof()
 *       will not work if you pass in a pointer to the valid bits instead.
 */
#define ZERO_VALID(vf)		memset(vf, 0, sizeof(vf))
#define SET_VALID(n, vf)	vf[n/8] |= (1 << (n % 8))
#define SET_ALL_VALID(vf)       memset(vf, 0xff, sizeof(vf))
#define CLR_VALID(n, vf)	vf[n/8] &= ~( 1 << (n % 8))
#define VALID(n,vf)		((vf)[(n)/8] & (1 << ((n) % 8)))

/*
 * These macros are used for caching in the v_ routines to indicate if there
 * might be a hole in a table causing the cache to be invalid. The last bit
 * in the valid bitfield defined in the snmptype.h file is reserved for this
 * purpose.
 *
 * NOTE: These macros require that the actual valid bits array be passed into
 *       the function call. These macros, particularly those that call sizeof()
 *       will not work if you pass in a pointer to the valid bits instead.
 */
#define NEXT_ROW(vf)           (vf[sizeof(vf) - 1] & (1 << 7))
#define CLEAR_NEXT_ROW_BIT(vf) \
        vf[(sizeof(vf) * 8 - 1) / 8] &= ~(1 << ((sizeof(vf) * 8 - 1) % 8))

/* VXWORKS uses SR_OFFSET as well, but not in any code we'd touch so I'm 
    replacing it with ours */

#define SR_OFFSET(x,off) (void *)((char *)x + (off * sizeof(SR_INT32)))

#define U_OFFSET(x,off)	(void *)((char *)x + (off * sizeof(SR_UINT32)))

typedef struct _do_list doList_t;
typedef struct _ObjectInfo ObjectInfo;

typedef VarBind  *(*GetMethodType)(
    OID *incoming,
    ObjectInfo *object,
    int searchType,
    ContextInfo *contextInfo,
    int serialNum);

typedef int (*TestMethodType)(
    OID *incoming,
    ObjectInfo *object,
    ObjectSyntax *value,
    doList_t *doHead,
    doList_t *doCur,
    ContextInfo *contextInfo);

typedef int (*SetMethodType)(
    doList_t *doHead,
    doList_t *doCur,
    ContextInfo *contextInfo);

typedef int (*UndoMethodType)(
    doList_t *doHead,
    doList_t *doCur,
    ContextInfo *contextInfo);

typedef int (*CleanupMethodType)(
    doList_t *trash);

typedef int (*ReadyMethodType)(
    doList_t *doHead,
    doList_t *doCur,
    ContextInfo *contextInfo);

typedef GetMethodType GetMethod;
typedef TestMethodType TestMethod;

struct _do_list {
    SetMethodType setMethod;         /* The set/abort method */
    CleanupMethodType cleanupMethod; /* The cleanup method */
    UndoMethodType undoMethod;       /* The undo method */
    SR_INT32  vbPos;    /* First varBind position */
    SR_INT32  state;    /* SR_UNUSED/SR_UNKNOWN/SR_ADD_MODIFY/SR_DELETE */
    void     *data;     /* opaque data, used by setMethod() */
    void     *undodata; /* opaque data, used by undoMethod() */
    doList_t *next;     /* next doList item */
    ReadyMethodType readyMethod;    /* The ready method */
    VarBind  *hintvbs;     /* optional hints objects */     
    SR_UINT16 vbSubPos;  /* position within aggregate, 0 for non-aggregate */
    SR_UINT16 set_kind;  /* which kind of set (create/delete/modify/replace */
    void     *userdata;  /* reserved for future use */
};

struct _ObjectInfo {
    OID             oid;
#ifndef LIGHT
    char           *oidname;
#else /* LIGHT */
#endif                          /* LIGHT */
    unsigned char   oidtype, oidstatus, nominator, priority;
    GetMethod       get;
    TestMethod      test;
};

typedef struct _AbbreviatedObjectInfo {
    SR_UINT32 last_sid;
    unsigned char   oidtype, oidstatus, nominator, priority;
}               AbbreviatedObjectInfo;

typedef struct _FamilyInfo {
    OID             oid;
    GetMethod       get;
    TestMethod      test;
    AbbreviatedObjectInfo *abbrOI;
}               FamilyInfo;

int k_initialize(void);
int k_terminate(void);

int readOnly_test(
    OID *incoming,
    ObjectInfo *object,
    ObjectSyntax *value,
    doList_t *doHead,
    doList_t *doCur,
    ContextInfo *auth);

SR_INT32 CheckAuthName(OctetString *authname);

int GetEmanateVersion(void);

extern const OID      ZeroOid;

extern int writeConfigFileFlag;

#ifdef  __cplusplus
}
#endif

#endif				/* SR_METHOD_H */
