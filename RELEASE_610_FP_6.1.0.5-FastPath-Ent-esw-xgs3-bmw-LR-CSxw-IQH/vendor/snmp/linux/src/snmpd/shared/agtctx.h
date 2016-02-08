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

#ifndef SR_AGTCTX_H
#define SR_AGTCTX_H

#ifdef  __cplusplus
extern "C" {
#endif

#define MATCH_IP_ADDR(addr,src_addr) \
    (((addr)==(src_addr)) || ((addr)==((SR_INT32)0)))

#ifdef SR_SNMPv1_ADMIN
typedef struct {
    SR_INT32 privs;
} AdminInfo;
#endif /* SR_SNMPv1_ADMIN */

#ifdef SR_SNMPv3_ADMIN
typedef struct {
    SR_INT32 securityModel;
    SR_INT32 securityLevel;
    OctetString *authSnmpEngineID;
    OctetString *securityName;
    OctetString *contextSnmpEngineID;
    OctetString *contextName;
    OctetString *groupName;
    TransportInfo srcTI;
} AdminInfo;

vacmAccessEntry_t *LookupVacmAccessEntryWithMask(
    OctetString *groupName,
    OctetString *contextName,
    SR_INT32 securityModel,
    SR_INT32 securityLevel);
#endif /* SR_SNMPv3_ADMIN */

ContextInfo *findContextInfo(
    SnmpLcd *snmp_lcd,
    TransportInfo *srcTI,
    TransportInfo *dstTI,
    SnmpMessage *snmp_msg,
    AdminInfo *admin_info,
    SR_UINT32 *limit,
    unsigned int outLen);

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_AGTCTX_H */
