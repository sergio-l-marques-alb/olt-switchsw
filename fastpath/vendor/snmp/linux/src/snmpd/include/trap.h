/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
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

#ifndef SR_TRAP_H
#define SR_TRAP_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "sr_trans.h"

SR_UINT32 GetDefaultV1TrapAgentAddr(void);

SR_UINT32 SetDefaultV1TrapAgentAddr(SR_UINT32 ipAddr);

void do_trap(
    SR_INT32 genTrap,
    SR_INT32 specTrap,
    struct _VarBind *vb,
    const OID *enterprise,
    char *entV2Trap);

SR_INT32 InitializeTraps(void);

#ifdef SR_SNMPv1_PACKET
SR_INT32 GenericDoV1Trap(
    SR_INT32 genTrap,
    SR_INT32 specTrap,
    const OID *enterprise,
    VarBind *vb_list);

SR_INT32 DoV1TrapToDest(
    SR_INT32 genTrap,
    SR_INT32 specTrap,
    const OID *enterprise,
    VarBind *vb_list,
    OctetString *community,
    TransportInfo *dstTI,
    SR_INT32 cfg_chk);
#endif /* SR_SNMPv1_PACKET */

/* DoTrapToDest currently a stub function */
void DoTrapToDest(
    SR_INT32 genTrap,
    SR_INT32 specTrap,
    VarBind *vb_list,
    const OID *enterprise,
    char *entV2Trap,
    TransportInfo *dstTI);

#ifdef SR_SNMPv2_PDU

#ifdef SR_SNMPv2c_PACKET
SR_INT32 DoV2cTrapToDest(
    OID *snmpTrapOID,
    OctetString *community,
    VarBind *vb_list,
    TransportInfo *dstTI,
    SR_INT32 cfg_chk);
#endif	/* SR_SNMPv2c_PACKET */

SR_INT32 GenericDoV2Trap(
    const OID *snmpTrapOID,
    OctetString *contextLocalEntity,
    VarBind *vb_list);

SR_INT32 DoV2TrapToDest(
    OID *snmpTrapOID,
    SR_INT32 sPI,
    OctetString *identityName,
    OctetString *contextName,
    VarBind *vb_list,
    TransportInfo *dstTI,
    SR_INT32 cfg_chk);

#ifdef SR_SNMPv3_PACKET
SR_INT32 DoV3TrapToDest(
    OID *snmpTrapOID,
    SR_INT32 securityModel,
    SR_INT32 securityLevel,
    OctetString *securityName,
    OctetString *contextName,
    VarBind *vb_list,
    TransportInfo *dstTI,
    SR_INT32 cfg_chk);
#endif /* SR_SNMPv3_PACKET */
#endif /* SR_SNMPv2_PDU */

#include "ntfy_oid.h"

#ifdef  __cplusplus
}
#endif

#endif				/* SR_TRAP_H */
