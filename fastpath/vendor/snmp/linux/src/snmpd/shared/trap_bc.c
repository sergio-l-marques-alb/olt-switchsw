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

#include <sys/types.h>


#include <string.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>


#include <malloc.h>

#include "sr_snmp.h"
#include "sr_type.h"
#include "sr_proto.h"
#include "sr_trans.h"
#include "sr_msg.h"
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"

#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"

#include "snmptype.h"
#include "sr_ntfy.h"
#include "diag.h"
#if defined(SR_SNMPv2_PDU)
SR_FILENAME
#endif	/* defined(SR_SNMPv2_PDU) */

#if (defined(SR_SNMPv1_PACKET) || defined(SR_SNMPv2c_PACKET))
#include "v1_msg.h"
#endif	/* (defined(SR_SNMPv1_PACKET) || defined(SR_SNMPv2c_PACKET)) */

#ifdef SR_SNMPv3_PACKET
#include "v3_msg.h"
#endif /* SR_SNMPv3_PACKET */

#if defined(SR_SNMPv3_PACKET)
extern OctetString *agt_local_snmpID;
#endif	/* defined(SR_SNMPv3_PACKET) */

#include "trap.h"

extern SnmpLcd agt_snmp_lcd;

extern system_t systemData;


static void SrGenerateNotificationWrapper(
    SR_INT32 genTrap,
    SR_INT32 specTrap,
    VarBind *vb_list,
    const OID *enterprise,
    char *entV2Trap,
    SnmpMessage *snmp_msg,
    TransportInfo *dstTI,
    SR_INT32 cfg_chk);

static void
SrGenerateNotificationWrapper(
    SR_INT32 genTrap,
    SR_INT32 specTrap,
    VarBind *vb_list,
    const OID *enterprise,
    char *entV2Trap,
    SnmpMessage *snmp_msg,
    TransportInfo *dstTI,
    SR_INT32 cfg_chk)
{
#ifdef SR_SNMPv2_PDU
    FNAME("SrGenerateNotificationWrapper")
    OID *snmpTrapOID = NULL;
#endif /* SR_SNMPv2_PDU */

    if (enterprise == NULL) {
        enterprise = systemData.sysObjectID;
    }

#ifndef SR_SNMPv2_PDU
    SrV1GenerateNotification(SR_NTFY_TYPE_EITHER,
                             genTrap,
                             specTrap,
                             enterprise,
                             NULL,
                             vb_list,
                             snmp_msg,
                             dstTI,
                             cfg_chk);
#else /* SR_SNMPv2_PDU */
    switch (genTrap) {
        case COLD_START_TRAP:
            SrV2GenerateNotification(SR_NTFY_TYPE_EITHER,
                                     NULL,
                                     coldStartTrap,
                                     vb_list,
                                     snmp_msg,
                                     dstTI,
                                     cfg_chk);
            break;
        case WARM_START_TRAP:
            SrV2GenerateNotification(SR_NTFY_TYPE_EITHER,
                                     NULL,
                                     warmStartTrap,
                                     vb_list,
                                     snmp_msg,
                                     dstTI,
                                     cfg_chk);
            break;
        case LINK_DOWN_TRAP:
            SrV2GenerateNotification(SR_NTFY_TYPE_EITHER,
                                     NULL,
                                     linkDownTrap,
                                     vb_list,
                                     snmp_msg,
                                     dstTI,
                                     cfg_chk);
            break;
        case LINK_UP_TRAP:
            SrV2GenerateNotification(SR_NTFY_TYPE_EITHER,
                                     NULL,
                                     linkUpTrap,
                                     vb_list,
                                     snmp_msg,
                                     dstTI,
                                     cfg_chk);
            break;
        case AUTHEN_FAILURE_TRAP:
            SrV2GenerateNotification(SR_NTFY_TYPE_EITHER,
                                     NULL,
                                     authenFailureTrap,
                                     vb_list,
                                     snmp_msg,
                                     dstTI,
                                     cfg_chk);
            break;
        case EGP_NEIGHBOR_LOSS_TRAP:
            SrV2GenerateNotification(SR_NTFY_TYPE_EITHER,
                                     NULL,
                                     egpNeighborLossTrap,
                                     vb_list,
                                     snmp_msg,
                                     dstTI,
                                     cfg_chk);
            break;
        case ENTERPRISE_TRAP:
            if (entV2Trap != NULL) {
                snmpTrapOID = MakeOIDFromDot(entV2Trap);
            } else {
                snmpTrapOID = MakeOID(NULL, enterprise->length + 2);
                if (snmpTrapOID != NULL) {
                    memcpy(snmpTrapOID->oid_ptr,
                           enterprise->oid_ptr,
                           (size_t)(enterprise->length * sizeof(SR_UINT32)));
                    snmpTrapOID->oid_ptr[enterprise->length + 1] =
                        (SR_UINT32)specTrap;
                }
            }
            if (snmpTrapOID != NULL) {
                SrV2GenerateNotification(SR_NTFY_TYPE_EITHER,
                                         NULL,
                                         snmpTrapOID,
                                         vb_list,
                                         snmp_msg,
                                         dstTI,
                                         cfg_chk);
                FreeOID(snmpTrapOID);
            }
            break;
        default:
            DPRINTF((APWARN, "%s: bad trap type\n", Fname));
            break;
    }
#endif /* SR_SNMPv2_PDU */
}

/*
 * do_trap:
 *
 *   This routine sends a trap message to every management station to which
 *   an agent is configured to send traps.  The particular method in which
 *   the agent is configured should be transparent to the caller of this
 *   function.
 *
 * Parameters:
 *        genTrap    - The generic trap type.
 *        specTrap   - The specific trap type. This parameter is only used if
 *                   this is a SNMPv1 enterprise specific trap (genTrap = 
 *                   ENTERPRISE_TRAP).
 *      vb         - The varbind list to append to the trap.
 *      enterprise - Will be used as the value for the enterprise field in a
 *                   SNMPv1 trap.
 *      entV2Trap  - Only used when sending a SNMPv2 enterprise specific
 *                   trap. This parameter will be used as the value for the
 *                   snmpTrapOID.0 varbind in the SNMPv2 trap. If this
 *                   parameter is not NULL, then the specTrap and enterprise
 *                   parameters are ignored from a SNMPv1
 *                   trap. This value should be the concatenation of the
 *                   enterprise parameter, "0", and the specTrap field 
 *                   (Ex: "enterprise.0.specTrap" - snmp.0.4). 
 *
 * This function is available as an API function.
 */
void
do_trap(genTrap, specTrap, vb_list, enterprise, entV2Trap)
    SR_INT32        genTrap;
    SR_INT32        specTrap;
    VarBind        *vb_list;
    const OID      *enterprise;
    char           *entV2Trap;
{


    SrGenerateNotificationWrapper(genTrap,
                                  specTrap,
                                  vb_list,
                                  enterprise,
                                  entV2Trap,
                                  NULL,
                                  NULL,
                                  0);

    if (vb_list != NULL) {
        FreeVarBindList(vb_list);
    }
}

/*
 * DoTrapToDest
 *
 * This function will attempt to send traps to a particular destination.
 * There MUST be configuration information available to the agent in order
 * to determine the authentication information with which to send the traps.
 */
void
DoTrapToDest(genTrap, specTrap, vb_list, enterprise, entV2Trap, dstTI)
    SR_INT32        genTrap;
    SR_INT32        specTrap;
    VarBind        *vb_list;
    const OID      *enterprise;
    char           *entV2Trap;
    TransportInfo  *dstTI;
{
    SrGenerateNotificationWrapper(genTrap,
                                  specTrap,
                                  vb_list,
                                  enterprise,
                                  entV2Trap,
                                  NULL,
                                  dstTI,
                                  0);

    if (vb_list != NULL) {
        FreeVarBindList(vb_list);
    }
}

#ifdef SR_SNMPv1_PACKET
/*
 * GenericDoV1Trap
 *
 * This function will send an SNMPv1 trap to all management stations to which
 * an agent is configured to send SNMPv1 traps.
 *
 * The return value is 0 on success, -1 on failure.
 *
 * This function is available as an API function.
 */
SR_INT32
GenericDoV1Trap(genTrap, specTrap, enterprise, vb_list)
    SR_INT32 genTrap;
    SR_INT32 specTrap;
    const OID *enterprise;
    VarBind *vb_list;
{
    SnmpMessage snmp_msg;
    memset(&snmp_msg, 0, sizeof(snmp_msg));
    snmp_msg.version = SR_SNMPv1_VERSION;
    SrV1GenerateNotification(SR_NTFY_TYPE_EITHER,
                             genTrap,
                             specTrap,
                             enterprise,
                             NULL,
                             vb_list,
                             &snmp_msg,
                             NULL,
                             0);
    return 0;
}
#endif /* SR_SNMPv1_PACKET */

#ifdef SR_SNMPv2_PDU
/*
 * GenericDoV2Trap
 *
 * This function will send an SNMPv2 trap to all management stations to which
 * an agent is configured to send SNMPv2 traps.  The contextLocalEntity is
 * used as the contextName from which the notification originated.
 *
 * The return value is 0 on success, -1 on failure.
 *
 * This function is available as an API function.
 */
SR_INT32
GenericDoV2Trap(snmpTrapOID, contextLocalEntity, vb_list)
    const OID *snmpTrapOID;
    OctetString *contextLocalEntity;
    VarBind *vb_list;
{
    SnmpMessage snmp_msg;

#ifdef SR_SNMPv2c_PACKET
    memset(&snmp_msg, 0, sizeof(snmp_msg));
    snmp_msg.version = SR_SNMPv2c_VERSION;
    SrV2GenerateNotification(SR_NTFY_TYPE_EITHER,
                             contextLocalEntity,
                             snmpTrapOID,
                             vb_list,
                             &snmp_msg,
                             NULL,
                             0);
#endif /* SR_SNMPv2c_PACKET */

#ifdef SR_SNMPv3_PACKET
    memset(&snmp_msg, 0, sizeof(snmp_msg));
    snmp_msg.version = SR_SNMPv3_VERSION;
    SrV2GenerateNotification(SR_NTFY_TYPE_EITHER,
                             contextLocalEntity,
                             snmpTrapOID,
                             vb_list,
                             &snmp_msg,
                             NULL,
                             0);
#endif /* SR_SNMPv3_PACKET */

    return 0;
}
#endif /* SR_SNMPv2_PDU */

#ifdef SR_SNMPv1_PACKET
/*
 * DoV1TrapToDest
 *
 * This function will send an SNMPv1 trap to a particular destination.
 * It will also optionally check to make sure the agent is configured to
 * send to this destination and fail if it is not.
 *
 * The return value is 0 on success, -1 on failure.
 *
 * This function is available as an API function.
 */
SR_INT32
DoV1TrapToDest(
    SR_INT32 genTrap,
    SR_INT32 specTrap,
    const OID *enterprise,
    VarBind *vb_list,
    OctetString *community,
    TransportInfo *dstTI,
    SR_INT32 cfg_chk)
{
    SnmpMessage *snmp_msg;

    snmp_msg = SrCreateV1SnmpMessage(community, 1);
    if (snmp_msg != NULL) {
        SrV1GenerateNotification(SR_NTFY_TYPE_EITHER,
                                 genTrap,
                                 specTrap,
                                 enterprise,
                                 NULL,
                                 vb_list,
                                 snmp_msg,
                                 dstTI,
                                 cfg_chk);
        SrFreeSnmpMessage(snmp_msg);
        return 0;
    }
    return -1;
}
#endif /* SR_SNMPv1_PACKET */

#ifdef SR_SNMPv2c_PACKET
/*
 * DoV2cTrapToDest
 *
 * This function will send an SNMPv2 trap to a particular destination,
 * using the SNMPv2c authentication mechanisms (i.e. community strings).
 * It will also optionally check to make sure the agent is configured to
 * send to this destination, and if everything in the trap is within a
 * configured view.  If these checks fail, the trap will not be sent.
 *
 * The return value is 0 on success, -1 on failure.
 *
 * This function is available as an API function.
 */
SR_INT32
DoV2cTrapToDest(snmpTrapOID, community, vb_list, dstTI, cfg_chk)
    OID *snmpTrapOID;
    OctetString *community;
    VarBind *vb_list;
    TransportInfo *dstTI;
    SR_INT32 cfg_chk;
{
    SnmpMessage *snmp_msg;

    snmp_msg = SrCreateV2cSnmpMessage(community, 1);
    if (snmp_msg != NULL) {
        SrV2GenerateNotification(SR_NTFY_TYPE_EITHER,
                                 NULL,
                                 snmpTrapOID,
                                 vb_list,
                                 snmp_msg,
                                 dstTI,
                                 cfg_chk);
        SrFreeSnmpMessage(snmp_msg);
        return 0;
    }
    return -1;
}
#endif /* SR_SNMPv2c_PACKET */

#ifdef SR_SNMPv3_PACKET
/*
 * DoV3TrapToDest
 *
 * This function will send an SNMPv3 trap to a particular destination.
 * It will also optionally check to make sure the agent is configured to
 * send to this destination, and if everything in the trap is within a
 * configured view.  If these checks fail, the trap will not be sent.
 *
 * The return value is 0 on success, -1 on failure.
 *
 * This function is available as an API function.
 */
SR_INT32
DoV3TrapToDest(
    OID *snmpTrapOID,
    SR_INT32 securityModel,
    SR_INT32 securityLevel,
    OctetString *securityName,
    OctetString *contextName,
    VarBind *vb_list,
    TransportInfo *dstTI,
    SR_INT32 cfg_chk)
{
    SnmpMessage *snmp_msg;
    snmp_msg = SrCreateV3SnmpMessage(securityLevel,
                                     securityModel,
                                     agt_local_snmpID,
                                     securityName,
                                     agt_local_snmpID,
                                     contextName,
#ifndef SR_UNSECURABLE
                                     NULL,
                                     0,
                                     0,
#ifndef SR_NO_PRIVACY
                                     NULL,
                                     0,
                                     0,
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
                                     TRUE);
    if (snmp_msg != NULL) {
        SrV2GenerateNotification(SR_NTFY_TYPE_EITHER,
                                 NULL,
                                 snmpTrapOID,
                                 vb_list,
                                 snmp_msg,
                                 dstTI,
                                 cfg_chk);
        SrFreeSnmpMessage(snmp_msg);
        return 0;
    }
    return -1;
}
#endif /* SR_SNMPv3_PACKET */
