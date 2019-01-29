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

#ifndef SR_SNMP_H
#define SR_SNMP_H

#ifdef  __cplusplus
extern "C" {
#endif	/* __cplusplus */

/* LVL7 */
#include "../snmpd/unix/sitedefs.h"
/* LVL7 end */

#include "sr_type.h"
#include "sr_proto.h"
#include "sr_time.h"

#ifndef lint
#include "sr_copyright.h"

#ifdef SR_WHAT
#include "what.h"
#endif	/* SR_WHAT */

#endif	/* lint */

/* map old library names to new library names? */
#ifdef SR_OLDNAMES
#include "oldnames.h"
#endif /* SR_OLDNAMES */

/* This is a default value for maximum message size. */
#define SR_DEFAULT_MAX_MSG_SIZE 2048

/*
 * This is the size of the wrapper fields in a pdu, and is used to
 * determine the maximum size of a varbind list that can be included
 * in an snmp message.
 *
 * This is calculated for the worst case scenario for the
 * size of extra fields in the PDU:
 *
 *        4 (worst case pdu type/length)
 *      + 6 (worst case request id)
 *      + 3 (worst case error status)
 *      + 6 (worst case error index)
 *      + 5 (worst case varbindlist wrapper)
 *      ---
 *       24
 *
 * Note this is an overestimate because we know that it normally should be:
 *
 *        4 (worst case pdu type/length)
 *      + 6 (worst case request id)
 *      + 3 (worst case error status)
 *      + 3 (worst case error index)
 *      + 4 (worst case varbindlist wrapper)
 *      ---
 *       20
 *
 * We will have a few more silent drops or tooBig errors -- but these cases are
 * so pathological that we will not invest any more resources to make it perfect
 */
#define SR_PDU_WRAPPER_SIZE 24


/*
 * Define SR_SNMPv._PDU_PAYLOAD constants
 */
#define SR_SNMPv1_PDU_PAYLOAD 1
#define SR_SNMPv2_PDU_PAYLOAD 2
#define SR_AGGREGATE_PDU_PAYLOAD 3

#define SR_SNMPv1_VERSION  0
#define SR_SNMPv2c_VERSION 1
#define SR_SNMPv2_VERSION  2
#define SR_SNMPv3_VERSION  3

/* Message processing models */
#define SR_SNMPv1_MESSAGE_PROCESSING_MODEL  0
#define SR_SNMPv2c_MESSAGE_PROCESSING_MODEL 1
#define SR_SNMPv2_MESSAGE_PROCESSING_MODEL  2
#define SR_SNMPv3_MESSAGE_PROCESSING_MODEL  3
#define SR_HTTP_MESSAGE_PROCESSING_MODEL    25344

/* SNMPv3 PDU classes */


/* Lifetime for v2* usec messages */
#define SR_USEC_LIFETIME_WINDOW 150

/* SPI values for SNMPv2* */
#define SR_SPI_SNMPv1        1
#define SR_SPI_SNMPv2c       2
#define SR_SPI_MAINT         3
#define SR_SPI_USEC_NOAUTH   4
#define SR_SPI_USM_NOAUTH    7
#ifndef SR_UNSECURABLE
#define SR_SPI_USEC_AUTH     5
#define SR_SPI_USM_AUTH      8
#ifndef SR_NO_PRIVACY
#define SR_SPI_USEC_PRIV     6
#define SR_SPI_USM_PRIV      9
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

/* Security Models */
#define SR_SECURITY_MODEL_V1     1
#define SR_SECURITY_MODEL_V2C    2
#define SR_SECURITY_MODEL_V2STAR 25345
#define SR_SECURITY_MODEL_USM    3
#ifdef SR_EPIC
#define SR_SECURITY_MODEL_EPIC 25346
#endif /* SR_EPIC */
#if defined(SR_EPIC)
#define SR_SECURITY_MODEL_XML  25347
#endif	/* defined(SR_EPIC) */
#ifdef SR_EPIC
#define SR_SECURITY_MODEL_CLI 25348
#endif /* SR_EPIC */
#ifdef SR_EMWEB_EPIC
/* below for custom work done for Agranat */
#define SR_SECURITY_MODEL_EMWEB_EPIC 973057
#endif /* SR_EMWEB_EPIC */

/* Security Levels */
#define SR_SECURITY_LEVEL_NOAUTH 1
#ifndef SR_UNSECURABLE
#define SR_SECURITY_LEVEL_AUTH   2
#ifndef SR_NO_PRIVACY
#define SR_SECURITY_LEVEL_PRIV   3
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

/* Authentication protocols */
#define SR_USM_NOAUTH_PROTOCOL        0
#ifndef SR_UNSECURABLE
#define SR_USM_HMAC_MD5_AUTH_PROTOCOL 1
#ifdef SR_SHA_HASH_ALGORITHM
#define SR_USM_HMAC_SHA_AUTH_PROTOCOL 2
#endif /* SR_SHA_HASH_ALGORITHM */
#endif /* SR_UNSECURABLE */

/* Privacy protocols */
#define SR_USM_NOPRIV_PROTOCOL   0
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
#define SR_USM_DES_PRIV_PROTOCOL 1
#ifdef SR_3DES
#define SR_USM_3DES_PRIV_PROTOCOL 2
#endif /* SR_3DES */
#ifdef SR_AES
#define SR_USM_AES_CFB_128_PRIV_PROTOCOL 3
#define SR_USM_AES_CFB_192_PRIV_PROTOCOL 4
#define SR_USM_AES_CFB_256_PRIV_PROTOCOL 5
#endif /* SR_AES */
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

/* choke on ANYTHING over this size (cause it will only get bigger later on). */
#define MAX_OCTET_STRING_SIZE 2048
#define MAX_OID_SIZE 128

#ifndef TRUE
#define TRUE 1
#endif				/* TRUE */

#ifndef FALSE
#define FALSE 0
#endif				/* FALSE */

/* Added 8/8/95 for subtree registration. -DLB */
#define ANY_TYPE   0xFF
#define ANY_STATUS SR_READ_WRITE
#define ANY_NOMINATOR 0
#define FULLY_QUALIFIED ((unsigned char) 128)
#define SR_ACCESS_MASK ((unsigned char) ~FULLY_QUALIFIED)

/*
 * Standard trap types (SNMPv1 definitions)
 */
#define COLD_START_TRAP         ((SR_INT32)0)
#define WARM_START_TRAP         ((SR_INT32)1)
#define LINK_DOWN_TRAP          ((SR_INT32)2)
#define LINK_UP_TRAP            ((SR_INT32)3)
#define AUTHEN_FAILURE_TRAP     ((SR_INT32)4)
#define EGP_NEIGHBOR_LOSS_TRAP  ((SR_INT32)5)
#define ENTERPRISE_TRAP         ((SR_INT32)6)

/* Universals */
#define INTEGER_TYPE            0x02
#define OCTET_PRIM_TYPE         0x04
#define NULL_TYPE               0x05
#define OBJECT_ID_TYPE          0x06
#define SEQUENCE_TYPE           0x30
#define INTEGER_32_TYPE         INTEGER_TYPE

/* Applications */
#define IP_ADDR_PRIM_TYPE      0x40
#define COUNTER_TYPE           0x41
#define GAUGE_TYPE             0x42
#define TIME_TICKS_TYPE        0x43
#define OPAQUE_PRIM_TYPE       0x44
#define COUNTER_32_TYPE        COUNTER_TYPE
#define	GAUGE_32_TYPE          GAUGE_TYPE
#define	UNSIGNED32_TYPE        GAUGE_TYPE
#define	COUNTER_64_TYPE        0x46
#define	INTEGER_64_TYPE        0x4A
#define	UINTEGER_64_TYPE       0x4B

#define BITS_TYPE              OCTET_PRIM_TYPE

/* Contexts */
#define GET_REQUEST_TYPE       0xA0
#define GET_NEXT_REQUEST_TYPE  0xA1
#define GET_RESPONSE_TYPE      0xA2
#define SET_REQUEST_TYPE       0xA3
#define TRAP_TYPE              0xA4
#define	RESPONSE_TYPE GET_RESPONSE_TYPE
#define GET_BULK_REQUEST_TYPE  0xA5
#define INFORM_REQUEST_TYPE    0xA6
#define SNMPv2_TRAP_TYPE       0xA7
#define REPORT_TYPE            0xA8

/* Type used in SNMPv2* */
#define V2_AUTH_MSG_TYPE       0xA9
#define USEC_AUTH_INFO_TYPE    0xA9

/* Exceptions in SNMPv2 PDUs (also contexts) */
#define NO_SUCH_OBJECT_EXCEPTION     0x80
#define NO_SUCH_INSTANCE_EXCEPTION   0x81
#define END_OF_MIB_VIEW_EXCEPTION    0x82



/* Private */



/* Private Constructs */

/* Error codes */
#define NO_ERROR            0
#define TOO_BIG_ERROR       1
#define NO_SUCH_NAME_ERROR  2
#define BAD_VALUE_ERROR     3
#define READ_ONLY_ERROR     4
#define GEN_ERROR           5

#ifndef	SR_SNMPv2_PDU
#define NO_ACCESS_ERROR             NO_SUCH_NAME_ERROR
#define WRONG_TYPE_ERROR            BAD_VALUE_ERROR
#define WRONG_LENGTH_ERROR          BAD_VALUE_ERROR
#define WRONG_ENCODING_ERROR        BAD_VALUE_ERROR
#define WRONG_VALUE_ERROR           BAD_VALUE_ERROR
#define NO_CREATION_ERROR           NO_SUCH_NAME_ERROR
#define INCONSISTENT_VALUE_ERROR    BAD_VALUE_ERROR
#define RESOURCE_UNAVAILABLE_ERROR  GEN_ERROR
#define COMMIT_FAILED_ERROR         GEN_ERROR
#define UNDO_FAILED_ERROR           GEN_ERROR
#define AUTHORIZATION_ERROR         NO_SUCH_NAME_ERROR
#define NOT_WRITABLE_ERROR          NO_SUCH_NAME_ERROR
#define INCONSISTENT_NAME_ERROR     NO_SUCH_NAME_ERROR
#else				/* SR_SNMPv2_PDU */
#define NO_ACCESS_ERROR              6
#define WRONG_TYPE_ERROR             7
#define WRONG_LENGTH_ERROR           8
#define WRONG_ENCODING_ERROR         9
#define WRONG_VALUE_ERROR           10
#define NO_CREATION_ERROR           11
#define INCONSISTENT_VALUE_ERROR    12
#define RESOURCE_UNAVAILABLE_ERROR  13
#define COMMIT_FAILED_ERROR         14
#define UNDO_FAILED_ERROR           15
#define AUTHORIZATION_ERROR         16
#define NOT_WRITABLE_ERROR          17
#define INCONSISTENT_NAME_ERROR     18
#endif				/* SR_SNMPv2_PDU */

/* These can be returned from ParseAuthentication */
#define ASN_PARSE_ERROR          9
#define SNMP_BAD_VERSION_ERROR   10
#define UNKNOWN_SPI_ERROR         50
#define WRONG_DIGEST_VALUE_ERROR  51
#define UNKNOWN_USER_NAME_ERROR   52
#define NOT_IN_TIME_WINDOW_ERROR  53
#define BAD_PRIVACY_ERROR         54
#define UNKNOWN_CONTEXT_ERROR     55
#define UNAVAILABLE_CONTEXT_ERROR 56
#define SILENT_DROP_ERROR         58
#define SR_NO_GROUP_ERROR         59

#define SR_SNMP_BAD_COMMUNITY_NAMES 100

/* Error codes generated by MP */
#define SR_SNMP_UNKNOWN_SECURITY_MODELS    500
#define SR_SNMP_INVALID_MSGS               501
#define SR_SNMP_UNKNOWN_PDU_HANDLERS       504

/* Error codes generated by USM */
#define SR_USM_UNSUPPORTED_SEC_LEVELS      600
#define SR_USM_NOT_IN_TIME_WINDOWS         601
#define SR_USM_UNKNOWN_USER_NAMES          602
#define SR_USM_UNKNOWN_ENGINE_IDS          603
#define SR_USM_WRONG_DIGESTS               604
#define SR_USM_DECRYPTION_ERRORS           605

#ifdef SR_EPIC
/* Error codes generated by EPIC */
#define SR_EPIC_DROPPED_REQUEST_ERROR      700
#endif /* SR_EPIC */

/* Security Levels (as stored in msgFlags) */
#define SR_SECURITY_LEVEL_BITS_NOAUTH_NOPRIV 0
#ifndef SR_UNSECURABLE
#define SR_SECURITY_LEVEL_BITS_AUTH_NOPRIV   1
#ifndef SR_NO_PRIVACY
#define SR_SECURITY_LEVEL_BITS_NOAUTH_PRIV   2
#define SR_SECURITY_LEVEL_BITS_AUTH_PRIV     3
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

/* ContextMatch values */
#define SR_CONTEXT_MATCH_EXACT  1
#define SR_CONTEXT_MATCH_PREFIX 2

/* Lifetime for USM messages */
#define SR_USM_LIFETIME_WINDOW 150

/* domains */
#define UNKNOWN_DOMAIN		      0
#define SNMPv1_DOMAIN                 1
#define SNMPv2_DOMAIN                 2

/* View types */
#define INCLUDED   1
#define EXCLUDED   2

/* StorageType values  */
#define SR_OTHER 1
#define SR_VOLATILE 2
#define SR_NONVOLATILE 3
#define SR_PERMANENT 4
#define SR_READONLY 5

/* RowStatus types */
#define RS_UNINITIALIZED   0
#define RS_ACTIVE          1
#define RS_NOT_IN_SERVICE  2
#define RS_NOT_READY       3
#define RS_CREATE_AND_GO   4
#define RS_CREATE_AND_WAIT 5
#define RS_DESTROY         6

/* TruthValue types */
#define SR_LOCAL 1
#define SR_REMOTE 2

/* read flags (access and variable) */
#define SR_DISCARD        -1
#define SR_NONE            0
#define SR_READ_ONLY       0x20
#define SR_READ_WRITE      0x21
#define SR_OLD_READ_CREATE 0x22
#define SR_READ_CREATE     0x23
#define SR_NOT_ACCESSIBLE  0x24
#define SR_ACCESSIBLE_FOR_NOTIFY SR_NOT_ACCESSIBLE

#ifndef SR_NO_PRIVACY
#ifdef SR_3DES
/* key length for 3DES */
#define SR_3DES_KEY_LENGTH 32
#define SR_3DES_EACH_KEY_LENGTH 8
#endif /* SR_3DES */

#ifdef SR_AES
/* key length for AES */
/* current max is 256 bits */
#define SR_AES_KEY_LENGTH 32
#define SR_AES_128_KEY_LENGTH 16
#define SR_AES_192_KEY_LENGTH 24
#endif /* SR_AES */
#endif /* SR_NO_PRIVACY */

/* ip protocol */
#define SR_IPV4_PROTO      0

#ifndef NONULL
#define NULLIT(pointer) pointer = NULL;
#else                           /* NONULL */
#define NULLIT(pointer)
#endif                          /* NONULL */

#define OIDSIZE(x) (sizeof(x)/sizeof(SR_UINT32))

#ifdef MAX
#undef MAX
#endif

#ifdef MIN
#undef MIN
#endif

#define  MAX(x, y)       (((x) > (y)) ? (x) : (y))
#define  MIN(x, y)       (((x) < (y)) ? (x) : (y))



typedef struct _OctetString {
    unsigned char  *octet_ptr;
    SR_INT32        length;
} OctetString;

typedef struct _OID {
    SR_INT32        length;
    SR_UINT32      *oid_ptr;
} OID;

typedef struct _UInt64 {
    SR_UINT32  big_end;
    SR_UINT32  little_end;
}               UInt64;

typedef struct _Int64 {
    SR_INT32  big_end;
    SR_UINT32  little_end;
}               Int64;





typedef struct _ObjectSyntax {
    short           type;
    union {
	SR_UINT32   ulval;	/* Counter, Gauge, time_ticks */
	SR_INT32    slval;	/* simple num. */
	OctetString    *osval;	/* for OS, IpAddress, Opaque */
	OID            *oidval;	/* Object Identifier */
	UInt64         *uint64val;	/* Big Unsigned Integer, e.g.,
					 * Counter64 */
        struct _VarBind        *vbval;  /* Aggregate object value */
	Int64          *int64val;

    }               u;
    SR_UINT16       flags;	     /* bit flags defined in comments above */
    SR_UINT16       child_supp_type; /* SR_*_SUPPRESSION type applied to 
                                      * VarBind value                       */
} ObjectSyntax;
#define ul_value        u.ulval
#define sl_value        u.slval
#define os_value        u.osval
#define oid_value       u.oidval
#define uint64_value    u.uint64val
#define vb_value        u.vbval
#define int64_value     u.int64val


typedef struct _VarBind {
    SR_INT32        data_length;
    OID             *name;
    short           name_size;	/* number of bytes to encode the oid's tlv */
    ObjectSyntax    value;
    short           value_size;	/* number of bytes to encode the value's tlv */
    struct _VarBind *next_var;
    short           name_type;  /* name format for this vb (listing below)  */
    SR_UINT32       subid;      /* column identifier for suppressed column name
                                 * or position of last subid in the object 
                                 * portion of aggregate table or row name */
} VarBind;


/*
 * VarBind name_type values:
 * 
 *   OBJECT_ID_TYPE          -->  non-suppressed OID
 *   INTEGER_TYPE            -->  format for explicit suppressed column objs
 *   OBJECT_ID_FRAG_TYPE     -->  format for explicit or implicit suppressed 
 *                                rows, partial OID identifies row instance
 *   IMPLICIT_NAME_TYPE      -->  Implicit naming determined by position in list
 */
#define OBJECT_ID_FRAG_TYPE        0xC7
#define IMPLICIT_NAME_TYPE         0xC9



typedef struct _Old_ObjectSyntax {
    short           type;
    union {
        SR_UINT32   ulval;      /* Counter, Gauge, time_ticks */
        SR_INT32    slval;      /* simple num. */
        OctetString    *osval;  /* for OS, IpAddress, Opaque */
        OID            *oidval; /* Object Identifier */
        UInt64         *uint64val;      /* Big Unsigned Integer, e.g.,
                                         * Counter64 */
    }               u;
} Old_ObjectSyntax;

typedef struct _Old_VarBind {
    SR_INT32        data_length;
    OID             *name;
    short           name_size;  /* number of bytes to encode the oid's tlv */
    Old_ObjectSyntax    value;
    short           value_size; /* number of bytes to encode the value's tlv */
    struct _Old_VarBind *next_var;
} Old_VarBind;

typedef struct _NormPdu {
    SR_INT32    request_id;
    SR_INT32    error_status;
    SR_INT32    error_index;
} NormPdu;

typedef struct _BulkPdu {
    SR_INT32    request_id;
    SR_INT32    non_repeaters;
    SR_INT32    max_repetitions;
}               BulkPdu;

typedef struct _TrapPdu {
    OID            *enterprise;
    OctetString    *agent_addr;
    SR_INT32        generic_trap;
    SR_INT32        specific_trap;
    SR_UINT32       time_ticks;
} TrapPdu;

typedef struct _Pdu {
    OctetString    *packlet;
    short           type;
    SR_INT32        num_varbinds;
    union {
	NormPdu         normpdu;
	TrapPdu         trappdu;
	BulkPdu         bulkpdu;
    }               u;
    VarBind        *var_bind_list;
    VarBind        *var_bind_end_ptr;
} Pdu;

typedef struct {
    OctetString *community;
} SnmpV1Message;

/* v2c uses SnmpV1Message */

typedef struct {
    /* Packet information */
    SR_UINT32       mms;
    SR_UINT32       reportableFlag;
    SR_UINT32       spi;
    OctetString    *authSnmpID;
    OctetString    *identityName;
    SR_INT32        authSnmpBoots;
    SR_INT32        authSnmpTime;
    OctetString    *transportLabel;
    OctetString    *contextSnmpID;
    OctetString    *contextName;

    /* Information from LCD */
#ifndef SR_UNSECURABLE
    OctetString    *authKey;
#ifndef SR_NO_PRIVACY
    OctetString    *privKey;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    OctetString    *groupName;
} SnmpV2Message;

typedef struct {
    /* Packet information */
    SR_UINT32       msgID;
    SR_UINT32       mms;
    SR_UINT32       reportableFlag;
    SR_UINT32       securityLevel;
    SR_UINT32       securityModel;
    OctetString    *authSnmpEngineID;
    SR_INT32        authSnmpEngineBoots;
    SR_INT32        authSnmpEngineTime;
    OctetString    *userName;
    OctetString    *userSecurityName;
    OctetString    *contextSnmpEngineID;
    OctetString    *contextName;

    /* Information from LCD */
#ifndef SR_UNSECURABLE
    OctetString    *authKey;
    SR_INT32        authProtocol;
    SR_INT32        authLocalized;
#ifndef SR_NO_PRIVACY
    OctetString    *privKey;
    SR_INT32        privProtocol;
    SR_INT32        privLocalized;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
} SnmpV3Message;

typedef struct {
    OctetString *packlet;
    int error_code;
    int cached;

    SR_INT32 version;
    SR_INT32 wrapper_size;
    SR_INT32 max_vbl_size;

    union {
        SnmpV1Message v1; /* also v2c */
        SnmpV2Message v2;
        SnmpV3Message v3;
    } u;

    /* These are used for parsing through a packet */
    const unsigned char *wptr;
    const unsigned char *end_ptr;
    const unsigned char *new_end_ptr;

} SnmpMessage;

int strcmpi(char *s1, char *s2);

#include "sri/getport.h"

#include "sri/make_lib.h"

#include "sri/free_lib.h"

#include "sri/bld_pkt.h"

#include "sri/prse_pkt.h"

#include "sri/auth_lib.h"

#include "sri/nvt_chk.h"

#include "sri/sigfunc.h"

#include "sri/gettime.h"



extern SR_UINT32 sr_pkt_size;


#ifdef  __cplusplus
}
#endif	/* __cplusplus */

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_SNMP_H */
