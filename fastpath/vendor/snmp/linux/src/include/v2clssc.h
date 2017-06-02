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

#ifndef SR_SECURE_H
#define SR_SECURE_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct _PARTY {
    int             privs;
    int             domain;	/* snmp/udp/ip or snmpv2/udp/ip or etc */
    OID            *partyIdentity;
    int             partyTDomain;
    unsigned short  partyTPort;	/* In Network-Byte order */
    OctetString    *partyTAddress;
    SR_INT32        partyMaxMessageSize;
    int             partyAuthProtocol;
    SR_UINT32       partyAuthClock;
    OctetString    *partyAuthPublic;
    OctetString    *partyAuthPrivate;
    SR_INT32        partyAuthLifetime;
    int             partyPrivProtocol;
    OctetString    *partyPrivPublic;
    OctetString    *partyPrivPrivate;
    int             partyDiscriminator;

    SR_INT32        partyLocal;
    SR_INT32        partyStatus;
    SR_UINT32       snmpTrapNumbers;
    SR_INT32        partyIndex;
    SR_INT32        partyStorageType;
    int             partyCloneFrom;
    struct _PARTY  *next_ptr;	/* MGH 10-June-93 */
}               PARTY;

/* MGH 14-Dec-92 */
typedef struct _SR_CONTEXT {
    OID             *contextIdentity;
    SR_INT32         contextIndex;
    SR_INT32         contextLocal;
    SR_INT32         contextViewIndex;
    OctetString     *contextLocalEntity;
    int              contextLocalTime;
    OID             *contextProxyDstParty;
    OID             *contextProxySrcParty;
    OID             *contextProxyContext;
    SR_INT32         contextStorageType;
    SR_INT32         contextStatus;
    struct _SR_CONTEXT *next_ptr;
}               SR_CONTEXT;

/* JDC July 93 */
typedef struct _TRIOS {
    int             srcPartyIndex;
    int             dstPartyIndex;
    int             contextIndex;
}               TRIOS;
typedef struct _REQ_TRIOS {
    PARTY          *srcParty;
    PARTY          *dstParty;
    SR_CONTEXT        *context;
} REQ_TRIOS;

typedef struct _CLUSTER {
    OctetString    *cluster_name;
    int             readOperationAuthType;
    int             writeOperationAuthType;
    int             adminOperationAuthType;
    TRIOS           readOperation;
    TRIOS           writeOperation;
    TRIOS           noAuthClkSync;
    TRIOS           md5AuthClkSync;
    TRIOS           adminOperation;
    struct _CLUSTER *next;
}               CLUSTER;

#define CLSTR_R_S(cp) (cp)->readOperation.srcPartyIndex
#define CLSTR_R_D(cp) (cp)->readOperation.dstPartyIndex
#define CLSTR_R_C(cp) (cp)->readOperation.contextIndex
#define CLSTR_W_S(cp) (cp)->writeOperation.srcPartyIndex
#define CLSTR_W_D(cp) (cp)->writeOperation.dstPartyIndex
#define CLSTR_W_C(cp) (cp)->writeOperation.contextIndex
#define CLSTR_N_S(cp) (cp)->noAuthClkSync.srcPartyIndex
#define CLSTR_N_D(cp) (cp)->noAuthClkSync.dstPartyIndex
#define CLSTR_N_C(cp) (cp)->noAuthClkSync.contextIndex
#define CLSTR_M_S(cp) (cp)->md5AuthClkSync.srcPartyIndex
#define CLSTR_M_D(cp) (cp)->md5AuthClkSync.dstPartyIndex
#define CLSTR_M_C(cp) (cp)->md5AuthClkSync.contextIndex
#define CLSTR_A_S(cp) (cp)->adminOperation.srcPartyIndex
#define CLSTR_A_D(cp) (cp)->adminOperation.dstPartyIndex
#define CLSTR_A_C(cp) (cp)->adminOperation.contextIndex

/* MGH 12-Jun-92 */
typedef struct _ACCESS {
    SR_INT32        aclTarget;	        /* dstParty's partyIndex */
    SR_INT32        aclSubject;	        /* srcParty's partyIndex */
    SR_INT32        aclResources;	/* context's contextIndex */
    SR_INT32        aclPrivileges;
    SR_INT32        aclStorageType;	/* storage type */
    SR_INT32        aclStatus;	        /* now of type RowStatus */
    struct _ACCESS *next_ptr;
}               ACCESS;

typedef struct _V2_ACL {
    ACCESS         *acl;
    SR_UINT32       last_active;
}               V2_ACL;

/* MGH 9-Dec-92 */
typedef struct _MIB_VIEW {
    SR_INT32          viewIndex;
    OID              *viewSubtree;
    OctetString      *viewMask;
    SR_INT32          viewType;
    SR_INT32          viewStorageType;
    SR_INT32          viewStatus;
    struct _MIB_VIEW *next_ptr;
}               MIB_VIEW;

typedef struct V2_VIEW {
    MIB_VIEW       *mib_view;
    SR_UINT32       last_active;
}               V2_VIEW;

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_SECURE_H */
