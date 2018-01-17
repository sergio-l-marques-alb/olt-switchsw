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

#ifndef SR_SR_NTFY_H
#define SR_SR_NTFY_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "sr_trans.h"

#define SR_NTFY_TYPE_EITHER 0
#define SR_NTFY_TYPE_TRAP   1
#define SR_NTFY_TYPE_INFORM 2

SR_INT32 SendNotificationsSMIv2Params(
    const OID *snmpTrapOID,
    VarBind *vb_list,
    const OctetString *contextName);

SR_INT32 SendNotificationsSMIv1Params(
    SR_INT32 genTrap,
    SR_INT32 specTrap,
    const OID *enterprise,
    VarBind *vb_list,
    const OctetString *contextName);

SR_INT32 SendNotificationsSMIv1(
    SR_INT32 genTrap,
    SR_INT32 specTrap,
    const OID *enterprise,
    VarBind *vb_list,
    const OctetString *contextName,
    SnmpMessage *snmp_msg);

/* Translates to a v2 trap, and calls SrV2GenerateNotification */
void SrV1GenerateNotification(
    SR_INT32 type,
    SR_INT32 genTrap,
    SR_INT32 specTrap,
    const OID *enterprise,
    const OctetString *agent_addr,
    VarBind *vb,
    SnmpMessage *only_snmp_msg,
    TransportInfo *only_ti,
    SR_INT32 cfg_chk);

/*
 * Goes through all notification configurations, and sends the notification
 * to each one.  If only_snmp_msg is present, only sends to ones which
 * match the fields that are present in only_snmp_msg.  If only_ti is
 * present, only sends to ones which match only_ti.  If both only_snmp_msg
 * and only_ti are present, then if cfg_chk is non-zero, only sends to
 * ones which match both only_snmp_msg and only_ti, otherwise just
 * directly sends using only_snmp_msg and only_ti.
 */
void SrV2GenerateNotification(
    SR_INT32 type,
    const OctetString *contextName,
    const OID *snmpTrapOID,
    VarBind *vb,
    SnmpMessage *only_snmp_msg,
    TransportInfo *only_ti,
    SR_INT32 cfg_chk);

void SendNotificationToDestSMIv1Params(
    SR_INT32 notifyType,
    SR_INT32 genTrap,
    SR_INT32 specTrap,
    const OID *enterprise,
    const OctetString *agent_addr,
    VarBind *vb,
    const OctetString *contextName,
    SR_INT32 retryCount,
    SR_INT32 timeout,
    const OctetString *securityName,
    SR_INT32 securityLevel,
    SR_INT32 securityModel,
    TransportInfo *only_ti,
    SR_INT32 cfg_chk);

void SendNotificationToDestSMIv2Params(
    SR_INT32 notifyType,
    const OID *snmpTrapOID,
    const OctetString *agent_addr,
    VarBind *vb,
    const OctetString *contextName,
    SR_INT32 retryCount,
    SR_INT32 timeout,
    const OctetString *securityName,
    SR_INT32 securityLevel,
    SR_INT32 securityModel,
    TransportInfo *only_ti,
    SR_INT32 cfg_chk);

#ifdef  __cplusplus
}
#endif

#endif				/* SR_SR_NTFY_H */
