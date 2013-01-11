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


#include "pclo.h"

#ifdef SR_SNMPv2c_PACKET
static SR_INT32 trap_req_id = 0;
#endif /* SR_SNMPv2c_PACKET */

extern SnmpV2Table trapCommunities;

static
void SrGenerateNotification(
    SR_INT32 type,
    SR_INT32 version,
    Pdu *v1_pdu,
    Pdu *v2_pdu,
    TRAP_COMMUNITY *tc,
    TransportInfo *ti);

static
void
SrGenerateNotification(
    SR_INT32 type,
    SR_INT32 version,
    Pdu *v1_pdu,
    Pdu *v2_pdu,
    TRAP_COMMUNITY *tc,
    TransportInfo *ti)
{
    OID *ssto = NULL;
    OID *esto = NULL;
    SnmpMessage *snmp_msg = NULL;
    Pdu *pdu = NULL;

#ifdef SR_SNMPv1_PACKET
    if (version == SR_SNMPv1_VERSION) {
        switch (v1_pdu->u.trappdu.generic_trap) {
            case COLD_START_TRAP:
                ssto = (OID *)coldStartTrap;
                break;
            case WARM_START_TRAP:
                ssto = (OID *)warmStartTrap;
                break;
            case LINK_DOWN_TRAP:
                ssto = (OID *)linkDownTrap;
                break;
            case LINK_UP_TRAP:
                ssto = (OID *)linkUpTrap;
                break;
            case AUTHEN_FAILURE_TRAP:
                ssto = (OID *)authenFailureTrap;
                break;
            case EGP_NEIGHBOR_LOSS_TRAP:
                ssto = (OID *)egpNeighborLossTrap;
                break;
            case ENTERPRISE_TRAP:
                esto = MakeOID(NULL,
                               v1_pdu->u.trappdu.enterprise->length + 2);
                if (esto == NULL) {
                    goto done;
                }
                memcpy(esto->oid_ptr,
                       v1_pdu->u.trappdu.enterprise->oid_ptr,
                       (size_t)(v1_pdu->u.trappdu.enterprise->length *
                           sizeof(SR_UINT32)));
                esto->oid_ptr[esto->length-1] =
                    v1_pdu->u.trappdu.specific_trap;
                break;
        }
    }
#endif /* SR_SNMPv1_PACKET */

    switch (version) {
#ifdef SR_SNMPv1_PACKET
        case SR_SNMPv1_VERSION:
            snmp_msg =
                SrCreateV1SnmpMessage(tc->community_name,
                                      TRUE);
            pdu = v1_pdu;
            break;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
            snmp_msg =
                SrCreateV2cSnmpMessage(tc->community_name,
                                       TRUE);
            pdu = v2_pdu;
            pdu->type = SNMPv2_TRAP_TYPE;
            pdu->u.normpdu.request_id = trap_req_id++;
            break;
#endif /* SR_SNMPv2c_PACKET */
    }

    if (snmp_msg == NULL) {
        goto done;
    }

    if (type != SR_NTFY_TYPE_INFORM) {
        Sr_send_trap(snmp_msg, pdu, ti);
    } else {
        /*pdu->type = INFORM_REQUEST_TYPE;
        Sr_send_inform(snmp_msg, pdu, ti);*/
    }

#ifdef SR_DEBUG
    if (snmp_msg->packlet != NULL) {
        packdump("Outgoing Packet",
	         (unsigned char *)snmp_msg,
	         snmp_msg->packlet->length,
	         ti,
	         pdu->var_bind_list,
	         0);
    }
#endif /* SR_DEBUG */

    SrFreeSnmpMessage(snmp_msg);

  done:
    if (esto != NULL) {
        FreeOID(esto);
    }
}

/*
 * SrGenerateNotifications:
 *
 * Goes through all notification configurations, and sends the notification
 * to each one.
 *
 * If both only_snmp_msg and only_ti are non-NULL, then cfg_chk is used to
 * determine whether configuration info should be checked before sending
 * the notification.  If cfg_chk is 0, then the notification is sent
 * without checking any configuration information.  If cfg_chk is 1,
 * then the notification is only sent if it would normally be sent
 * given the current configuration.
 *
 * If either only_snmp_msg or only_ti is NULL, then cfg_chk is ignored.
 *
 * If only_snmp_msg is non-NULL, notifications are only sent if their
 * parameters match only_snmp_msg.  Likewise, if only_ti is non-NULL,
 * notifications are only sent if their destination matches only_ti.
 *
 */
static void
SrGenerateNotifications(
    SR_INT32 type,
    const OctetString *contextName,
    Pdu *v1_pdu,
    Pdu *v2_pdu,
    SnmpMessage *only_snmp_msg,
    TransportInfo *only_ti,
    SR_INT32 cfg_chk)
{
    TRAP_COMMUNITY *tc;
    int idx1;
    TransportInfo ti;
    SR_INT32 version;
    int pdu_version;

    if ((only_snmp_msg != NULL) && (only_ti != NULL) && (cfg_chk == 0)) {
#if (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU))
        /* Perform pdu conversion if necessary */

        if (only_snmp_msg->version == SR_SNMPv1_VERSION) {
            pdu_version = SR_SNMPv1_PDU_PAYLOAD;
        } else if (only_snmp_msg->version == SR_SNMPv2_VERSION) {
            pdu_version = SR_SNMPv2_PDU_PAYLOAD;
        }

        switch (only_ti->type) {
            default:
                if (convert_pdu(only_snmp_msg->version, 
                             &v1_pdu, &v2_pdu, SR_IP_TRANSPORT, pdu_version)) {
                    goto done;
                }
                break;
        }
#endif	/* (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU)) */
        switch (only_snmp_msg->version) {
#ifdef SR_SNMPv1_PACKET
            case SR_SNMPv1_VERSION:
                if (type != SR_NTFY_TYPE_INFORM) {
                    Sr_send_trap(only_snmp_msg, v1_pdu, only_ti);
                }
                break;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2_PDU
#ifdef SR_SNMPv2c_PACKET
            case SR_SNMPv2c_VERSION:
#endif /* SR_SNMPv2c_PACKET */
                if (type == SR_NTFY_TYPE_INFORM) {
                    /*v2_pdu->type = INFORM_REQUEST_TYPE;
                    Sr_send_inform(only_snmp_msg, v2_pdu, only_ti);*/
                } else {
                    v2_pdu->type = SNMPv2_TRAP_TYPE;
                    Sr_send_trap(only_snmp_msg, v2_pdu, only_ti);
                }
                break;
#endif /* SR_SNMPv2_PDU */
        }
        goto done;
    }

    for (idx1 = 0; idx1 < trapCommunities.nitems; idx1++) {
        tc = (TRAP_COMMUNITY *)trapCommunities.tp[idx1];

        ti.type = SR_IP_TRANSPORT;
        ti.t_ipAddr = tc->ip_addr;
        ti.t_ipPort = htons((short)GetSNMPTrapPort());

        /* Check if it matches only_ti if necessary */
        if (only_ti != NULL) {
            if (CmpTransportInfo(&ti, only_ti)) {
                continue;
            }
        }

#ifndef SR_SNMPv1_PACKET
        version = SR_SNMPv2c_VERSION;
        pdu_version = SR_SNMPv2_PDU_PAYLOAD;
#else /* SR_SNMPv1_PACKET */
        version = SR_SNMPv1_VERSION;
        pdu_version = SR_SNMPv1_PDU_PAYLOAD;
#endif /* SR_SNMPv1_PACKET */
        if (only_snmp_msg) {
            version = only_snmp_msg->version;
            if (only_snmp_msg->version == SR_SNMPv1_VERSION) {
                pdu_version = SR_SNMPv1_PDU_PAYLOAD;
            } else if (only_snmp_msg->version == SR_SNMPv2_VERSION) {
                pdu_version = SR_SNMPv2_PDU_PAYLOAD;
            }
        }

#if (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU))
        /* Perform pdu conversion if necessary */
        if (convert_pdu(version, &v1_pdu, &v2_pdu, SR_IP_TRANSPORT, pdu_version)) {
            continue;
        }
#endif	/* (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU)) */

        /* Call SrGenerateNotification */
        SrGenerateNotification(type,
                               version,
                               v1_pdu,
                               v2_pdu,
                               tc,
                               &ti);
    }


  done:
    if (v1_pdu) {
        FreePdu(v1_pdu);
    }
    if (v2_pdu) {
        FreePdu(v2_pdu);
    }
}


/*
 * SrSendNotifications:
 *
 * Goes through all notification configurations, and sends the notification
 * to each one.
 *
 * If both snmp_msg and only_ti are non-NULL, then cfg_chk is used to
 * determine whether configuration info should be checked before sending
 * the notification.  If cfg_chk is 0, then the notification is sent
 * without checking any configuration information.  If cfg_chk is 1,
 * then the notification is only sent if it would normally be sent
 * given the current configuration.
 *
 * If either snmp_msg or only_ti is NULL, then cfg_chk is ignored.
 *
 * If snmp_msg is non-NULL, notifications are only sent if their
 * parameters match snmp_msg.  Likewise, if only_ti is non-NULL,
 * notifications are only sent if their destination matches only_ti.
 *
 */
static void
SrSendNotifications(
    SR_INT32 notifyType,
    const OctetString *contextName,
    Pdu *v1_pdu,
    Pdu *v2_pdu,
    SnmpMessage *snmp_msg,
    TransportInfo *only_ti,
    SR_INT32 cfg_chk,
    const OctetString *agent_addr, 
    SR_INT32 retryCount,
    SR_INT32 timeout)
{
    TRAP_COMMUNITY *tc;
    int idx1;
    TransportInfo ti;
    SR_INT32 version;
#if (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU))
    int pdu_version;
#endif	/* (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU)) */

    if ((snmp_msg != NULL) && (only_ti != NULL) && (cfg_chk == 0)) {
#if (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU))
        /* Perform pdu conversion if necessary */

        if (snmp_msg->version == SR_SNMPv1_VERSION) { 
            pdu_version = SR_SNMPv1_PDU_PAYLOAD; 
        } else if (snmp_msg->version == SR_SNMPv2_VERSION) {
            pdu_version = SR_SNMPv2_PDU_PAYLOAD;
        } 

        switch (only_ti->type) {
	    default:
                if (translate_pdu(snmp_msg->version, &v1_pdu, &v2_pdu,
                    agent_addr, SR_IP_TRANSPORT, pdu_version)) {
                    goto done;
                } 
                break;
        }
#endif	/* (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU)) */
        switch (snmp_msg->version) {
#ifdef SR_SNMPv1_PACKET
            case SR_SNMPv1_VERSION:
                if (notifyType != SR_NTFY_TYPE_INFORM) {
                    Sr_send_trap(snmp_msg, v1_pdu, only_ti);
                }
                break;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2_PDU
#ifdef SR_SNMPv2c_PACKET
            case SR_SNMPv2c_VERSION:
#endif /* SR_SNMPv2c_PACKET */
                if (notifyType == SR_NTFY_TYPE_INFORM) {
                    /*v2_pdu->type = INFORM_REQUEST_TYPE;
                    Sr_send_inform(snmp_msg, v2_pdu, only_ti);*/
                } else {
                    v2_pdu->type = SNMPv2_TRAP_TYPE;
                    Sr_send_trap(snmp_msg, v2_pdu, only_ti);
                }
                break;
#endif /* SR_SNMPv2_PDU */
        }
        goto done;
    }

    for (idx1 = 0; idx1 < trapCommunities.nitems; idx1++) {
        tc = (TRAP_COMMUNITY *)trapCommunities.tp[idx1];

        ti.type = SR_IP_TRANSPORT;
        ti.t_ipAddr = tc->ip_addr;
        ti.t_ipPort = htons((short)GetSNMPTrapPort());

        /* Check to see if community matches (if present) */
        if ( (snmp_msg != NULL) && (CmpOctetStrings(tc->community_name, 
                                                    snmp_msg->u.v1.community)) ) {
            continue;
        }

        /* Check if it matches only_ti if necessary */
        if (only_ti != NULL) {
            if (CmpTransportInfo(&ti, only_ti)) {
                continue;
            }
        }

#ifndef SR_SNMPv1_PACKET
        version = SR_SNMPv2c_VERSION;
#else /* SR_SNMPv1_PACKET */
        version = SR_SNMPv1_VERSION;
#endif /* SR_SNMPv1_PACKET */
        if (snmp_msg) {
            version = snmp_msg->version;
        }

#if (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU))
        /* Perform pdu conversion if necessary */
        if (translate_pdu(version, &v1_pdu, &v2_pdu, 
                           agent_addr, SR_IP_TRANSPORT, pdu_version)) {
            continue;
        }
#endif	/* (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU)) */

        /* Call SrGenerateNotification */
        SrGenerateNotification(notifyType,
                               version,
                               v1_pdu,
                               v2_pdu,
                               tc,
                               &ti);
    }


  done:
    if (v1_pdu) {
        FreePdu(v1_pdu);
    }
    if (v2_pdu) {
        FreePdu(v2_pdu);
    }
}


