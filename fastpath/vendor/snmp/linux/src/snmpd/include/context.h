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

#ifndef SR_CONTEXT_H
#define SR_CONTEXT_H

#ifdef  __cplusplus
extern "C" {
#endif

/* Tag values for values listed in a NewContextInfo structure */
#define NCI_SENDER_TRANSPORT          1
#define NCI_RECEIVER_TRANSPORT        2
#define NCI_REQUEST_ID                3
#define NCI_SNMP_VERSION              4
#define NCI_MESSAGE_PROCESSING_MODEL  4
#define NCI_COMMUNITY                 5
#define NCI_SPI                       6
#define NCI_AUTH_SNMP_ID              7
#define NCI_AUTH_SNMP_ENGINE_ID       7
#define NCI_IDENTITY_NAME             8
#define NCI_SECURITY_NAME             8
#define NCI_CONTEXT_SNMP_ID           9
#define NCI_CONTEXT_SNMP_ENGINE_ID    9
#define NCI_CONTEXT_NAME              10
#define NCI_CONTEXT_LOCAL_ENTITY      11
#define NCI_CONTEXT_LOCAL_TIME        12
#define NCI_COMMUNITY_PRIVS           13
#define NCI_SECURITY_MODEL            14
#define NCI_SECURITY_LEVEL            15
#define NCI_MMS_REMAINING             16
#define NCI_AGGREGATE_NAME_TYPE       17
#define NCI_SSEID_FILTER              18
#define NCI_STARTING_POINT            19
#define NCI_AGG_VIEW_LIMIT            20 
#define NCI_SET_OPERATION_TYPE        21 
#define NCI_SET_APPLY_TYPE            22 

/* Tag values for passing other data through the contextInfo structure */
#define NCI_OPAQUE_POINTER         100

typedef struct _PointerOrObjectSyntax {
    SR_UINT32 tag;
    union {
        ObjectSyntax os;
        void *ptr;
    } value;
} PointerOrObjectSyntax;

/*
 *  Length is the number of PointerOrObjectSyntax structures
 *  pointed to by tv.  An arbitrary number of PointerOrObjecSyntax
 *  structures may be concatenated there.
 */
typedef struct _NewContextInfo {
    SR_UINT32 length;
    PointerOrObjectSyntax *tv;
} NewContextInfo;

/* New structure which replaces asod. */
#define CI_LOCAL            1
#define CI_PROXY            2
#define CI_COMMUNITY        3
#define CI_PARTY_COMMUNITY  4
#define CI_BY_DOT_ID        5
#define CI_NEW_CONTEXT_INFO 6


#define SR_SET_DEFAULT   0 


#ifndef SR_COMUNITY_H
typedef struct _COMMUNITY COMMUNITY;
#endif /* SR_COMUNITY_H */

#ifndef SR_SECURE_H
typedef struct _PARTY PARTY;
#endif /* SR_SECURE_H */

typedef struct _ContextInfo {
    SR_INT32          CIType;	/* one of the CI_ values */
    union {
	/* when CIType == CI_LOCAL */
	struct {
	    SR_INT32        Time;
	    OctetString    *Entity;
	    OID *dstParty, *srcParty, *context;
	}               local;

	/* when CIType == CI_COMMUNITY */
	COMMUNITY      *Community;

	/* when CIType == CI_PARTY_COMMUNITY */
	PARTY          *PartyCommunity;

	/* when CIType == CI_BY_DOT_ID */ 
	char *contextDotIdentity;

	/* when CIType == CI_NEW_CONTEXT_INFO */
        NewContextInfo new_context_info;
    }               u;
}               ContextInfo;

typedef struct _Old_PointerOrObjectSyntax {
    SR_UINT32 tag;
    union {
        Old_ObjectSyntax os;
        void *ptr;
    } value;
} Old_PointerOrObjectSyntax;

/*
 *  Length is the number of Old_PointerOrObjectSyntax structures
 *  pointed to by tv.  An arbitrary number of Old_PointerOrObjecSyntax
 *  structures may be concatenated there.
 */
typedef struct _Old_NewContextInfo {
    SR_UINT32 length;
    Old_PointerOrObjectSyntax *tv;
} Old_NewContextInfo;

typedef struct _Old_ContextInfo {
    SR_INT32          CIType;   /* one of the CI_ values */
    union {
        /* when CIType == CI_LOCAL */
        struct {
            SR_INT32        Time;
            OctetString    *Entity;
            OID *dstParty, *srcParty, *context;
        }               local;

        /* when CIType == CI_COMMUNITY */
        COMMUNITY      *Community;

        /* when CIType == CI_PARTY_COMMUNITY */
        PARTY          *PartyCommunity;

        /* when CIType == CI_BY_DOT_ID */
        char *contextDotIdentity;

        /* when CIType == CI_NEW_CONTEXT_INFO */
        Old_NewContextInfo new_context_info;
    }               u;
}               Old_ContextInfo;

ContextInfo *AllocContextInfo(
    SR_UINT32 length);

ContextInfo *CloneContextInfo(
    ContextInfo *cip);

void FreeContextInfo(
    struct _ContextInfo *cip);

OctetString *LookupViewFromContextInfo(
    struct _ContextInfo *contextInfo,
    SR_INT32 req_type);

ObjectSyntax *SearchContextInfo(
    ContextInfo *cip,
    SR_UINT32 tag);

void *SearchAPOContextInfo(
    ContextInfo *cip,
    SR_UINT32 tag);

int SrAssignRequestIDToContextInfo(
    ContextInfo *cip,
    SR_INT32 val);

int SrUpdateContextInfoRecord(
    ContextInfo *cip,
    SR_INT32 tag,
    void *val);

#ifdef  __cplusplus
}
#endif

#endif	/* SR_CONTEXT_H */

