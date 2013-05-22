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

#ifndef SR_SNMPD_H
#define SR_SNMPD_H

#ifdef  __cplusplus
extern "C" {
#endif

#define SNMP_TRAPS_ENABLED 1
#define SNMP_TRAPS_DISABLED 2

struct _Pdu *make_error_pdu(
    int type,
    SR_INT32 req_id,
    int status, 
    int counter,
    struct _Pdu *in_pdu_ptr,
    int domain);	/* in mgmt.c */

#include "chk_view.h"

#ifndef SR_SNMPv2_PDU
#define exception(type, name, version)        NULL
#else  /* SR_SNMPv2_PDU */
struct _VarBind *exception(
    int type,
    struct _OID *name,
    int payload);
#endif /* SR_SNMPv2_PDU */

unsigned short do_mgmt(
    unsigned char *inPdu,
    unsigned int inLen,
    TransportInfo *srcTI,
    TransportInfo *dstTI,
    unsigned char *outPkt,
    unsigned int outLen);

struct _Pdu  *do_response(
    struct _Pdu *inpdu, 
    struct _ContextInfo *contextInfo,
    SR_UINT32 limit,
    struct _OctetString *viewName, 
    int version);

/* sets.c */
struct _Pdu *do_sets(
    struct _Pdu *inPdu,
    struct _ContextInfo *contextInfo,
    struct _OctetString *viewName,
    int payload);

struct _Pdu *do_sets_noblock(
    struct _Pdu *inPdu,
    struct _ContextInfo *contextInfo,
    struct _OctetString *viewName,
    int payload);

#ifdef SR_DEBUG
void PrintSetAuditMessage(
    SnmpMessage *snmp_msg,
    struct _Pdu *in_pdu_ptr,
    SR_INT32 success,
    TransportInfo *srcTI);
#endif /* SR_DEBUG */

typedef SR_UINT32 Ounit;
typedef SR_UINT32 bit_32;

struct object_index {
    struct object_info *object;
    bit_32          permbits;
};

/*============================================================
 * Global data initialization section.
 *============================================================*/

#ifndef SUBSYSTEM
/*
 * The system group.
 */
extern system_t systemData;

/*
 * The snmp group.
 */
extern snmp_t  snmpData;
#endif /* SUBSYSTEM */


#ifdef USE_POSTDEFS
#include "postdefs.h"
#endif /* USE_POSTDEFS */


#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_SNMPD_H */
