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

#include <stdlib.h>

#include <errno.h>

#include <unistd.h>

#include <sys/types.h>


#include <string.h>

#include <arpa/inet.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>


#include <malloc.h>




#include "sr_snmp.h"
#include "sr_type.h"
#include "sr_proto.h"
#include "sr_trans.h"
#include "oid_lib.h"
#include "sr_msg.h"
#include "ntfy_cvt.h"
#include "ntfy_oid.h"
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "v3_tag.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
#include "agtctx.h"
#include "sr_ntfy.h"
#include "diag.h"
#include "sr_ip.h"
SR_FILENAME

#if (defined(SR_SNMPv1_PACKET) || defined(SR_SNMPv2c_PACKET))
#include "v1_msg.h"
#endif	/* (defined(SR_SNMPv1_PACKET) || defined(SR_SNMPv2c_PACKET)) */

#ifdef SR_SNMPv3_PACKET
#include "snmpv3/v3type.h"
#include "v3_msg.h"
extern SnmpV3Lcd agt_snmp_v3_lcd;
extern SnmpV2Table agt_usmUserTable;
#endif /* SR_SNMPv3_PACKET */

#include "trap.h"


#include "sr_user.h"
#include "snmpv2d.h"

extern SnmpLcd agt_snmp_lcd;
extern OctetString *agt_local_snmpID;

/* LVL7 */

#include "snmp_util_api.h"
#include "osapi_support.h"
#include "usmdb_sim_api.h"
/* LVL7 end */

#ifdef SR_EPIC
#include "sri/vbtable.h"
#include "epic.h"
#include "epiccore.h"

extern SnmpLcd epic_snmp_lcd;
#endif /* SR_EPIC */

extern system_t systemData;
extern snmp_t  snmpData;


extern SR_UINT32 GetLocalIPAddress(void);

extern int bind_ip_proto;

extern char *send_port_range;
extern char *snmpPortBindAddr;


SR_INT32 SrInitializeTraps(void);

#if (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU))
static SR_INT32 convert_pdu(
    SR_INT32 required_version,
    Pdu **v1_pdu,
    Pdu **other_pdu,
    int trans_type,
    int pdu_version);

static SR_INT32 translate_pdu(
    SR_INT32 required_version,
    Pdu **v1_pdu,
    Pdu **other_pdu,
    const OctetString *agent_addr,
    int trans_type,
    int pdu_version);
#endif	/* (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU)) */

void Sr_send_trap(
    SnmpMessage *snmp_msg,
    Pdu *pdu,
    TransportInfo *dstTI);

static void Sr_send_trap_ctx(
    SnmpMessage *snmp_msg,
    Pdu *pdu,
    TransportInfo *dstTI,
    const OctetString *contextName);

static void SrGenerateNotifications(
    SR_INT32 type,
    const OctetString *contextName,
    Pdu *v1_pdu,
    Pdu *other_pdu,
    SnmpMessage *only_snmp_msg,
    TransportInfo *only_ti,
    SR_INT32 cfg_chk);

static void SrSendNotifications(
    SR_INT32 notifyType,
    const OctetString *contextName,
    Pdu *v1_pdu,
    Pdu *other_pdu,
    SnmpMessage *snmp_msg,
    TransportInfo *only_ti,
    SR_INT32 cfg_chk,
    const OctetString *agent_addr,
    SR_INT32 retryCount,
    SR_INT32 timeout);

static SnmpMessage *SrFillSnmpMessage(
    SR_INT32 notifyType,
    const OctetString *securityName,
    SR_INT32 securityLevel,
    SR_INT32 securityModel,
    const OctetString *contextName);

static int             ipTransportOpened = 0;
TransportInfo          ipTrapTI;
#ifdef SR_SNMPv1_PACKET
static SR_UINT32       LocalIP = (SR_UINT32)0;
static unsigned char   buffer[4];
static OctetString     localAddr = { buffer, 4 };
#endif /* SR_SNMPv1_PACKET */

/* LVL7 */
void
SrLocalAddrUpdate()
{
  L7_uint32 tmpLocalIP = 0;

  /* Get the first valid local IP address */
  usmDbIPFirstLocalAddress(&tmpLocalIP);

  *((SR_UINT32 *)buffer) = osapiHtonl(tmpLocalIP);
}

/* LVL7 end */


#ifdef SR_SNMPv1_PACKET
/*
 * The value of agentaddr_init_flag and agentipv6addr_init_flag are:
 *
 *   0 - if the default SNMPv1 Trap agent-addr field has NOT been
 *       set manually.
 *
 *   1 - if the default SNMPv1 Trap agent-addr field has been set
 *       manually with SetDefaultV1TrapAgentAddr() or 
 *       SetDefaultV1TrapAgentIpv6Addr().
 */
int agentaddr_init_flag = 0;

SR_UINT32
GetDefaultV1TrapAgentAddr(void)
{
    return OctetStringToIP(&localAddr);
}

SR_UINT32
SetDefaultV1TrapAgentAddr(SR_UINT32 ipAddr)
{
    SR_UINT32 old_default;

    old_default = GetDefaultV1TrapAgentAddr();
    localAddr.octet_ptr[0] = (ipAddr >> 24) & 0xff;
    localAddr.octet_ptr[1] = (ipAddr >> 16) & 0xff;
    localAddr.octet_ptr[2] = (ipAddr >>  8) & 0xff;
    localAddr.octet_ptr[3] = (ipAddr >>  0) & 0xff;
    agentaddr_init_flag = 1;
    return old_default;
}
#endif /* SR_SNMPv1_PACKET */

SR_INT32
SrInitializeTraps()
{
    FNAME("SrInitializeTraps")
    static SR_INT32 initialized = 1;
    int trap_send_port = 0;
    char *check1=NULL, *check2=NULL;
    SR_UINT32 bind_addr, bound_port = 0;
#ifdef SR_DEBUG
    char tstr[64];
#endif /* SR_DEBUG */

    if (initialized != 1) {
        return initialized;
    }
    DPRINTF((APTRACE, "%s: Opening transport(s) to send SNMP notifications\n",
             Fname));

    if (!ipTransportOpened) {
        switch (bind_ip_proto) {
            default:
                memset(&ipTrapTI, 0, sizeof(ipTrapTI));
                ipTrapTI.type = SR_IP_TRANSPORT;
                if (OpenTransport(&ipTrapTI)) {
                    DPRINTF((APWARN,
                          "%s: cannot open IP transport traps.\n", Fname));
                    initialized = -1;
                    return -1;
                }
                break;
        }

        ipTransportOpened = 1;

        /* check for user-specified trap source address */
        if (snmpPortBindAddr != NULL) {
            bind_addr = inet_addr(snmpPortBindAddr);
            if (bind_addr == -1) {
               DPRINTF((APERROR, "Cannot resolve snmp_bindaddr %s\n",
                        snmpPortBindAddr));
               initialized = -1;
               return -1;
            } else {
               ipTrapTI.t_ipAddr = bind_addr;
               DPRINTF((APTRACE, "SNMP trap bind restricted to address %s\n",
                       snmpPortBindAddr));
            }
        }

        /* check for user-specified trap source port */
        if(send_port_range != NULL){
            /* Bind to a user-specified port for sending notifications */
            check1 = strchr(send_port_range, ',');
            check2 = strchr(send_port_range, '-');
            if((check1 == NULL) && (check2 == NULL)){
                /* single port specified */
                trap_send_port = atoi(send_port_range);
                if(trap_send_port <= 0 || trap_send_port >= 65536){
                    DPRINTF((APWARN,
                               "The trap send port can not be set to %d\n",
                               trap_send_port));
                    initialized = -1;
                    return -1;
               } else if (trap_send_port == GetSNMPPort()) {
                    DPRINTF((APWARN, "Cannot send traps from the same port "
                        "as requests are received\n"));
                    initialized = -1;
                    return -1;
               }
               ipTrapTI.t_ipPort = osapiHtons((short) trap_send_port);
               if (BindTransport(&ipTrapTI, 5)) {
                   DPRINTF((APERROR, "Cannot bind TRAP transport %s\n%s",
                          FormatTransportString(tstr, sizeof(tstr), &ipTrapTI),
                          (trap_send_port == 162) ?
                          "Is an SNMP manager running?\n" : ""));
                   initialized = -1;
                   return -1;
               }
               DPRINTF((APTRACE|APTRAP, "Traps will be sent from %s\n",
                        FormatTransportString(tstr, sizeof(tstr), &ipTrapTI)));
            } else {
               /* port range specified */
               if(BindTransportGivenPortRange(&ipTrapTI)) {
                   DPRINTF((APERROR,
                             "Cannot bind TRAP transport given range %s\n",
                             send_port_range));
                   initialized = -1;
                   return -1;
               }
            }
 
            bound_port = 1;
        }

        if ((snmpPortBindAddr != NULL) && (bound_port == 0)) {
            /* no port specified, bind to trap source addr on port 0 */
            if (BindTransport(&ipTrapTI, 5)) {
                DPRINTF((APERROR, "Cannot bind TRAP transport %s\n%s",
                          FormatTransportString(tstr, sizeof(tstr), &ipTrapTI),
                          (trap_send_port == 162) ?
                          "Is an SNMP manager running?\n" : ""));
                initialized = -1;
                return -1;
            }
            DPRINTF((APTRACE|APTRAP, "Traps will be sent from %s\n",
                        FormatTransportString(tstr, sizeof(tstr), &ipTrapTI)));
        }

        /* Update localAddr LVL7 */
        SrLocalAddrUpdate();

#ifdef OLD_CODE /* LVL7 */
#ifdef SR_SNMPv1_PACKET
        
        if (agentaddr_init_flag == 0) {
                DPRINTF((APTRAP, "%s: setting default value for agent-addr\n",
                     Fname));
                if (snmpPortBindAddr != NULL) {
                    LocalIP = htonl(bind_addr);
                } else {
                    LocalIP = GetLocalIPAddress();
                }
                if ((LocalIP == -1) || (LocalIP == 0)) {
                    DPRINTF((APWARN, 
                        "%s: host db doesn't know self.\n", Fname));
                    initialized = -1;
                    return -1;
                } else {
                    DPRINTF((APTRAP, "%s: default value for agent-addr "
                        "has already been initialized\n", Fname));
		}
            *((SR_UINT32 *)buffer) = htonl(LocalIP);
        }
#endif /* SR_SNMPv1_PACKET */
#endif /* OLD_CODE */  /* LVL7 */
    }


    initialized = 0;
    return 0;
}

#if (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU))
static SR_INT32
convert_pdu(
    SR_INT32 required_version,
    Pdu **v1_pdu,
    Pdu **other_pdu,
    int trans_type,
    int pdu_version)
{

    if (((*v1_pdu) == NULL) && ((*other_pdu) == NULL)) {
        return -1;
    }

    if (((*v1_pdu) != NULL) && ((*other_pdu) != NULL)) {
        return 0;
    }

    /* Perform pdu conversion if necessary */
    switch (required_version) {
        case SR_SNMPv1_VERSION:
            if (pdu_version != SR_SNMPv1_PDU_PAYLOAD) {
                return -1;
            } 
            if ((*v1_pdu) == NULL) {
                switch (trans_type) {
                    default:
                        *v1_pdu = 
                              SrConvertV2NotificationToV1Notification(*other_pdu, 
                                           systemData.sysObjectID, &localAddr);
                        break;
                }
                if ((*v1_pdu) == NULL) {
                    return -1;
                }
            }
            break;
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
            if (pdu_version != SR_SNMPv2_PDU_PAYLOAD) {
                return -1;
            } 
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
        case SR_SNMPv3_VERSION:
#endif /* SR_SNMPv3_PACKET */
            if ((*other_pdu) == NULL) {
                *other_pdu =
                    SrConvertV1NotificationToV2Notification(*v1_pdu);
                if ((*other_pdu) == NULL) {
                    return -1;
                }
            }
            break;
    }

    return 0;
}

static SR_INT32
translate_pdu(
    SR_INT32 required_version,
    Pdu **v1_pdu,
    Pdu **other_pdu,
    const OctetString *agent_addr,
    int trans_type, 
    int pdu_version)
{

    if (((*v1_pdu) == NULL) && ((*other_pdu) == NULL)) {
        return -1;
    }

    if (((*v1_pdu) != NULL) && ((*other_pdu) != NULL)) {
        return 0;
    }

    /* Perform pdu conversion if necessary */
    switch (required_version) {
        case SR_SNMPv1_VERSION:
            if (pdu_version != SR_SNMPv1_PDU_PAYLOAD) {
                return -1;
            } 
            if ((*v1_pdu) == NULL) {
                if (agent_addr == NULL) {
                    switch (trans_type) {
                        default:
                            *v1_pdu = SrConvertV2NotificationToV1Notification(
                                   *other_pdu, systemData.sysObjectID, &localAddr);
                            break;
                    }
                }
                else {
                    *v1_pdu = SrConvertV2NotificationToV1Notification(
                        *other_pdu, systemData.sysObjectID, agent_addr);
                }
                if ((*v1_pdu) == NULL) {
                    return -1;
                }
            }
            break;
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
            if (pdu_version != SR_SNMPv2_PDU_PAYLOAD) {
                return -1;
            } 
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
        case SR_SNMPv3_VERSION:
#endif /* SR_SNMPv3_PACKET */
            if ((*other_pdu) == NULL) {
                *other_pdu =
                    SrConvertV1NotificationToV2Notification(*v1_pdu);
                if ((*other_pdu) == NULL) {
                    return -1;
                }
            }
            break;
    }

    return 0;
}
#endif	/* (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU)) */

void
Sr_send_trap(
    SnmpMessage *snmp_msg,
    Pdu *pdu,
    TransportInfo *dstTI)
{
    Sr_send_trap_ctx(snmp_msg, pdu, dstTI, NULL);
}

static void
Sr_send_trap_ctx(
    SnmpMessage *snmp_msg,
    Pdu *pdu,
    TransportInfo *dstTI,
    const OctetString *contextName)
{
    TransportInfo *ti = NULL;
#ifdef SR_DEBUG
    char tstr[64];
#endif /* SR_DEBUG */
#ifdef SR_EPIC
    int i, status;
#endif /* EPIC */
    FNAME("Sr_send_trap_ctx")

    switch (dstTI->type) {   
/* LVL7 */
        case SR_IP6_TRANSPORT:
                memcpy(ipTrapTI.t_ip6Addr , dstTI->t_ip6Addr,16);
                if (dstTI->t_ip6Port == 0) {
                    ipTrapTI.t_ip6Port = osapiHtons((short)GetSNMPTrapPort());
                } else {
                    ipTrapTI.t_ip6Port = dstTI->t_ip6Port;
                }
                ti = &ipTrapTI;
                break;
/* LVL7 end */
        default:
                ipTrapTI.t_ipAddr = dstTI->t_ipAddr;
                if (dstTI->t_ipPort == 0) {
                    ipTrapTI.t_ipPort = osapiHtons((short)GetSNMPTrapPort());
                } else {
                    ipTrapTI.t_ipPort = dstTI->t_ipPort;
                }
                ti = &ipTrapTI;
            break;
    }


    if (ti == NULL) {
        DPRINTF((APERROR, "%s: bad transport\n", Fname));
        return;
    }


    if (BuildPdu(pdu)) {
        return;
    }

    if (SrBuildSnmpMessage(snmp_msg, pdu, &agt_snmp_lcd)) {
        return;
    }

    /*
     * Send SNMPv1, SNMPv2c, and SNMPv3/USM traps directly to the dest.
     */
#ifdef SR_EPIC
#ifdef SR_SNMPv3_PACKET
    if ( (snmp_msg->version < SR_SNMPv3_VERSION)
         || ((snmp_msg->version == SR_SNMPv3_VERSION) &&
             (snmp_msg->u.v3.securityModel) == SR_SECURITY_MODEL_USM)) {
#endif /* SR_SNMPv3_PACKET */
#endif /* SR_EPIC */

        /* Send the packet */
        if (SendToTransport((char *) snmp_msg->packlet->octet_ptr,
                            (int) snmp_msg->packlet->length, ti, NULL) < 0) {  /* LVL7 */
            DPRINTF((APWARN,
                "%s: Sendto failed, unable to send trap to manager %s.\n",
                Fname, FormatTransportString(tstr, sizeof(tstr), dstTI)));
        }
        else {
            DPRINTF((APTRAP, "%s: trap sent to %s\n", Fname,
                FormatTransportString(tstr, sizeof(tstr), dstTI)));
            snmpData.snmpOutPkts++;
            snmpData.snmpOutTraps++;
        }
#ifdef SR_EPIC
#ifdef SR_SNMPv3_PACKET
    }
#endif /* SR_SNMPv3_PACKET */
#endif /* SR_EPIC */

#ifdef SR_EPIC
#ifndef SR_SNMPv3_PACKET
    /*
     * Just send all traps destined for the loopback interface to the
     * EPIC clients.
     */
    if ( (dstTI->type == SR_IP_TRANSPORT) && 
         (dstTI->t_ipAddr == inet_addr("127.0.0.1")) ) {
#else /* SR_SNMPv3_PACKET */
    else {
#endif /* SR_SNMPv3_PACKET */
        for (i = 0; i < MAX_EPIC_STREAM; i++) {
#ifndef SR_SNMPv3_PACKET
            if (epicClients[i].fd != 0) {
#else /* SR_SNMPv3_PACKET */
            /*
             * We already know that this an SNMPV3 version trap, so
             * make sure security model matches.
             */
             if ( (epicClients[i].fd != 0) &&
                  (snmp_msg->u.v3.securityModel ==
                   epicClients[i].chosenSecurityModel) ) {
#endif /* SR_SNMPv3_PACKET */
                if (SrBuildSnmpMessage(snmp_msg, pdu, &epic_snmp_lcd) != -1) {
                    status = send(epicClients[i].fd, 
                                  (char *) snmp_msg->packlet->octet_ptr,
                                  (int) snmp_msg->packlet->length,
                                  0);
                    if (status == -1) {
                        DPRINTF((APTRACE, "%s: send of trap to EPIC client failed: %s\n",
                                 Fname, sys_errname(errno)));
                        close(epicClients[i].fd);
                        epicClients[i].fd = 0;
                        if (epicClients[i].partialPkt != NULL) {
                            free(epicClients[i].partialPkt);
                            epicClients[i].partialPkt = NULL;
                        }
                        epicClients[i].totalPktLen = 0;
                        epicClients[i].curPktLen = 0;
                    }
                }
            }
        }
    }
#endif /* SR_EPIC */
}

#ifdef SR_SNMPv1_PACKET
void
SrV1GenerateNotification(
    SR_INT32 type,
    SR_INT32 genTrap,
    SR_INT32 specTrap,
    const OID *enterprise,
    const OctetString *agent_addr,
    VarBind *vb,
    SnmpMessage *only_snmp_msg,
    TransportInfo *only_ti,
    SR_INT32 cfg_chk)
{
    Pdu *pdu = NULL;
    OID *tmp_enterprise = NULL;
    OctetString *tmp_agent_addr = NULL;
    VarBind *tvb = NULL;

    if (SrInitializeTraps()) {
        goto done;
    }

    if (enterprise == NULL) {
        tmp_enterprise = CloneOID(systemData.sysObjectID);
    } else {
        tmp_enterprise = CloneOID(enterprise);
    }
    if (tmp_enterprise == NULL) {
        goto done;
    }

    if (agent_addr == NULL) {
        /* Force update of localAddr LVL7*/
        SrLocalAddrUpdate();

        tmp_agent_addr = CloneOctetString(&localAddr);
    } else {
        tmp_agent_addr = CloneOctetString(agent_addr);
    }
    if (tmp_agent_addr == NULL) {
        goto done;
    }

    pdu = MakePdu(TRAP_TYPE,
                  0L, 0L, 0L,
                  tmp_enterprise,
                  tmp_agent_addr,
                  genTrap,
                  specTrap,
                  GetTimeNow());
    if (pdu == NULL) {
        goto done;
    }
    tmp_enterprise = NULL;
    tmp_agent_addr = NULL;

    if (vb != NULL) {
        tvb = CloneVarBindList(vb);
        if (tvb == NULL) {
            goto done;
        }
    }
    pdu->var_bind_list = tvb;
    if (tvb != NULL) {
        while (tvb->next_var != NULL) {
            tvb = tvb->next_var;
        }
    }
    pdu->var_bind_end_ptr = tvb;
    tvb = NULL;

    SrGenerateNotifications(type,
                            NULL,
                            pdu,
                            NULL,
                            only_snmp_msg,
                            only_ti,
                            cfg_chk);

  done:
    if (tmp_enterprise) {
        FreeOID(tmp_enterprise);
    }
    if (tmp_agent_addr) {
        FreeOctetString(tmp_agent_addr);
    }
    if (tvb) {
        FreeVarBindList(tvb);
    }
}
#endif /* SR_SNMPv1_PACKET */

#ifdef SR_SNMPv2_PDU
void
SrV2GenerateNotification(
    SR_INT32 type,
    const OctetString *contextName,
    const OID *snmpTrapOID,
    VarBind *vb,
    SnmpMessage *only_snmp_msg,
    TransportInfo *only_ti,
    SR_INT32 cfg_chk)
{
    OID *oid = NULL;
    VarBind *vb1 = NULL;
    VarBind *vb2 = NULL;
    VarBind *tvb = NULL;
    Pdu *pdu = NULL;
    SR_INT32 curtime;

    if (SrInitializeTraps()) {
        goto done;
    }

    pdu = MakePdu(SNMPv2_TRAP_TYPE, 0L, 0L, 0L, NULL, NULL, 0L, 0L, 0L);
    if (pdu == NULL) {
        goto done;
    }

    if (vb != NULL) {
        tvb = CloneVarBindList(vb);
        if (tvb == NULL) {
            goto done;
        }
    }

    curtime = GetTimeNow();
    vb1 = MakeVarBindWithValue(&sysUpTime_OID, NULL, TIME_TICKS_TYPE, &curtime);
    if (vb1 == NULL) {
        goto done;
    }

    oid = CloneOID(snmpTrapOID);
    if (oid == NULL) {
        goto done;
    }
    vb2 = MakeVarBindWithValue(&snmpTrapOID_OID, NULL, OBJECT_ID_TYPE, oid);
    if (vb2 == NULL) {
        goto done;
    }
    oid = NULL;

    vb1->next_var = vb2;
    vb2->next_var = tvb;
    tvb = vb1;
    vb1 = NULL;
    vb2 = NULL;

    pdu->var_bind_list = tvb;
    while (tvb->next_var != NULL) {
        tvb = tvb->next_var;
    }
    pdu->var_bind_end_ptr = tvb;
    tvb = NULL;

    SrGenerateNotifications(type,
                            contextName,
                            NULL,
                            pdu,
                            only_snmp_msg,
                            only_ti,
                            cfg_chk);

  done:
    if (oid) {
        FreeOID(oid);
    }
    if (vb1) {
        FreeVarBindList(vb1);
    }
    if (vb2) {
        FreeVarBindList(vb2);
    }
    if (tvb) {
        FreeVarBindList(tvb);
    }
}
#endif /* SR_SNMPv2_PDU */

/*
 * SendNotificationsSMIv2Params
 *
 * This function will send a notification (V2-SMI style) to all
 * management stations to which an agent is configured. The contextName
 * is used as the context from which the notification originated.
 * Underlying routines will convert this SMIv2 style
 * notification into an SMIv1 style notification for all destinations that
 * receive SMIv1 style notifications.
 *
 * If any of the incoming parameters are malloc'ed and need to be freed,
 * the calling routine is responsible for freeing the parameters.
 *
 * The return value is 0 on success, -1 on failure.
 *
 * This function is available as an API function.
 *
 * Example:
 *
 * Calling this routine to send out cold start traps to all destinations.
 *
 * SendNotificationsSMIv2Params(coldStartTrap, 0, NULL);
 *
 * You would have to supply the values for vb_list. The
 * vb_list for a cold start trap is usually NULL. It's possible you may need to
 * supply the OID for coldStartTrap as well (look in mibs/common/v2-mib.my).
 *
 * Sending an enterprise specific trap (V2-SMI).
 *
 *    surgeSpikeAlarm NOTIFICATION-TYPE
 *        STATUS  current
 *        DESCRIPTION
 *                "An event that is generated when the circuit breaker
 *                detects a power spike."
 *        ::= { surgeProtector 4 }
 *
 *    The snmpTrapOID field would be the OID for surgeProtector.4.
 *    The contextName field will normally be NULL.
 *    The vb_list would be NULL.
 *
 * Example:
 *    OID *snmpTrapOID;
 *
 *    snmpTrapOID = MakeOIDFromDot("surgeProtector.4");
 *
 *    SendNotificationsSMIv2Params(snmpTrapOID, NULL, NULL);
 *    FreeOID(snmpTrapOID);
 *
 */
SR_INT32 
SendNotificationsSMIv2Params(snmpTrapOID, vb_list, contextName)
    const OID *snmpTrapOID;
    VarBind *vb_list;
    const OctetString *contextName;
{
    SR_INT32 status = -1;
#ifndef SR_SNMPv2_PDU
    SR_INT32 genTrap;
    SR_INT32 specTrap;
    OID *enterprise = NULL;

    specTrap = 0;

    if (snmpTrapOID == NULL) {
        goto done; 
    }
    if (CmpOID(snmpTrapOID, coldStartTrap) == 0) {
        genTrap = COLD_START_TRAP;
    } else
    if (CmpOID(snmpTrapOID, warmStartTrap) == 0) {
        genTrap = WARM_START_TRAP;
    } else
    if (CmpOID(snmpTrapOID, linkDownTrap) == 0) {
        genTrap = LINK_DOWN_TRAP;
    } else
    if (CmpOID(snmpTrapOID, linkUpTrap) == 0) {
        genTrap = LINK_UP_TRAP;
    } else
    if (CmpOID(snmpTrapOID, authenFailureTrap) == 0) {
        genTrap = AUTHEN_FAILURE_TRAP;
    } else
    if (CmpOID(snmpTrapOID, egpNeighborLossTrap) == 0) {
        genTrap = EGP_NEIGHBOR_LOSS_TRAP;
    } else {
        genTrap = ENTERPRISE_TRAP;
        enterprise = CloneOID(snmpTrapOID);
        if (enterprise == NULL) {
            goto done;
        }
        enterprise->length--;
        specTrap = enterprise->oid_ptr[enterprise->length];
        /* If next subid is 0, remove it as well */
        if (enterprise->oid_ptr[enterprise->length - 1] == 0) {
            enterprise->length--;
        }
    }

    if (enterprise == NULL) {
        enterprise = systemData.sysObjectID;
    }
  
    SendNotificationsSMIv1Params(genTrap, specTrap, enterprise, vb_list, 
                                 contextName);
    status = 0;

  done:
    if ( (genTrap == ENTERPRISE_TRAP) && (enterprise != NULL) ) {
        FreeOID(enterprise);
    }
    return status;
#else /* SR_SNMPv2_PDU */
    SnmpMessage snmp_msg;

#ifdef SR_SNMPv1_PACKET
    memset(&snmp_msg, 0, sizeof(snmp_msg));
    snmp_msg.version = SR_SNMPv1_VERSION;
    SrV2GenerateNotification(SR_NTFY_TYPE_EITHER,
                             contextName,
                             snmpTrapOID,
                             vb_list,
                             &snmp_msg,
                             NULL,
                             0);
#endif /* SR_SNMPv1_PACKET */

#ifdef SR_SNMPv2c_PACKET
    memset(&snmp_msg, 0, sizeof(snmp_msg));
    snmp_msg.version = SR_SNMPv2c_VERSION;
    SrV2GenerateNotification(SR_NTFY_TYPE_EITHER,
                             contextName,
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
                             contextName,
                             snmpTrapOID,
                             vb_list,
                             &snmp_msg,
                             NULL,
                             0);
#endif /* SR_SNMPv3_PACKET */

    status = 0;

    return status;
#endif /* SR_SNMPv2_PDU */
}

/*
 * SendNotificationsSMIv1Params
 *
 * This function will send a notification (SMIv1 style) to all
 * management stations to which an agent is configured to send
 * notifications. Underlying routines will convert this SMIv1 style 
 * notification into an SMIv2 style notification for all destinations that
 * receive SMIv2 style notifications. The contextName
 * is used as the context from which the notification originated.
 *
 * If any of the incoming parameters are malloc'ed and need to be freed,
 * the calling routine is responsible for freeing the parameters.
 *
 * The return value is 0 on success, -1 on failure.
 *
 * This function is available as an API function.
 *
 * Example:
 *
 * Calling this routine to send out cold start traps to all destinations.
 *
 * SendNotificationsSMIv1Params(0, 0, enterpriseOID, vb_list).
 *
 * You would need to supply the values for enterpriseOID and vb_list. The
 * vb_list for a cold start trap is usually NULL.
 * 
 * The genTrap field can have the following valid values:
 *
 * 0 - cold start trap
 * 1 - warm start trap
 * 2 - link down trap
 * 3 - link up trap
 * 4 - authentication failure trap
 * 5 - egp neighbor loss trap
 * 6 - enterprise specific trap
 * 
 * The specTrap field is ignored unless the genTrap field is 6.
 *
 * Sending an enterprise specific trap (SMIv1). (Example from RFC1516)
 *
 *    rptrGroupChange TRAP-TYPE
 *        ENTERPRISE  snmpDot3RptrMgt
 *        VARIABLES   { rptrGroupIndex }
 *        DESCRIPTION
 *                "This trap is sent when a change occurs in the
 *                ...............
 *                recipients.)"
 *        REFERENCE
 *                "Reference IEEE 802.3 Rptr Mgt, 19.2.3.4,
 *                groupMapChange notification."
 *        ::= 2
 *
 *    The genTrap field would be 6 (for enterprise specific).
 *    The specTrap field would be 2 (matching value after ::= ).
 *    The enterprise field would be the OID for snmpDot3RptrMgt.
 *    The vb_list would contain a varbind for rptrGroupIndex.
 *
 * Example:
 *    OID *enterpriseOID;
 *
 *    enterpriseOID = MakeOIDFromDot("snmpDot3RptrMgt");
 *    SendNotificationsSMIv1Params(6, 2, enterpriseOID, vb_list);
 *
 */
SR_INT32 
SendNotificationsSMIv1Params(genTrap, specTrap, enterprise, vb_list, contextName) 
    SR_INT32 genTrap;
    SR_INT32 specTrap;
    const OID *enterprise;
    VarBind *vb_list;
    const OctetString *contextName;
{
    SnmpMessage snmp_msg;
    SR_INT32 status = -1;

#ifdef SR_SNMPv1_PACKET
    memset(&snmp_msg, 0, sizeof(snmp_msg));
    snmp_msg.version = SR_SNMPv1_VERSION;
    status = SendNotificationsSMIv1(genTrap,
                                    specTrap,
                                    enterprise,
                                    vb_list,
                                    contextName,
                                    &snmp_msg);
    if (status == -1) {
        return status;
    } 
#endif /* SR_SNMPv1_PACKET */

#ifdef SR_SNMPv2c_PACKET
    memset(&snmp_msg, 0, sizeof(snmp_msg));
    snmp_msg.version = SR_SNMPv2c_VERSION;
    status = SendNotificationsSMIv1(genTrap,
                                    specTrap,
                                    enterprise,
                                    vb_list,
                                    contextName,
                                    &snmp_msg);
    if (status == -1) {
        return status;
    } 
#endif /* SR_SNMPv2c_PACKET */

#ifdef SR_SNMPv3_PACKET
    memset(&snmp_msg, 0, sizeof(snmp_msg));
    snmp_msg.version = SR_SNMPv3_VERSION;
    status = SendNotificationsSMIv1(genTrap,
                                    specTrap,
                                    enterprise,
                                    vb_list,
                                    contextName,
                                    &snmp_msg);
    if (status == -1) {
        return status;
    } 
#endif /* SR_SNMPv3_PACKET */

    return status;
}

SR_INT32 
SendNotificationsSMIv1(genTrap, specTrap, enterprise, vb_list, contextName, snmp_msg) 
    SR_INT32 genTrap;
    SR_INT32 specTrap;
    const OID *enterprise;
    VarBind *vb_list;
    const OctetString *contextName;
    SnmpMessage *snmp_msg;
{
    Pdu *pdu = NULL;
    OID *tmp_enterprise = NULL;
    OctetString *tmp_agent_addr = NULL;
    VarBind *tvb = NULL;
    SR_INT32 status = -1;

    if (SrInitializeTraps()) {
        goto done;
    }
    if (enterprise == NULL) {
        tmp_enterprise = CloneOID(systemData.sysObjectID);
    } else {
        tmp_enterprise = CloneOID(enterprise);
    }
    if (tmp_enterprise == NULL) {
        goto done;
    }

    /* Force update of localAddr LVL7*/
    SrLocalAddrUpdate();

    tmp_agent_addr = CloneOctetString(&localAddr);

    if (tmp_agent_addr == NULL) {
        goto done;
    }

    pdu = MakePdu(TRAP_TYPE,
                  0L, 0L, 0L,
                  tmp_enterprise,
                  tmp_agent_addr,
                  genTrap,
                  specTrap,
                  GetTimeNow());
    if (pdu == NULL) {
        goto done;
    }
    tmp_enterprise = NULL;
    tmp_agent_addr = NULL;

    if (vb_list != NULL) {
        tvb = CloneVarBindList(vb_list);
        if (tvb == NULL) {
            goto done;
        }
    }
    pdu->var_bind_list = tvb;
    tvb = NULL;

    SrGenerateNotifications(SR_NTFY_TYPE_EITHER,
                            contextName,
                            pdu,
                            NULL,
                            snmp_msg,
                            NULL,
                            1);

    status = 0;

  done:
    if (tmp_enterprise) {
        FreeOID(tmp_enterprise);
    }
    if (tmp_agent_addr) {
        FreeOctetString(tmp_agent_addr);
    }
    if (tvb) {
        FreeVarBindList(tvb);
    }

    return status;
}

#ifdef SR_SNMPv1_PACKET
void
SendNotificationToDestSMIv1Params(
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
    SR_INT32 cfg_chk)
{
    Pdu *pdu = NULL;
    OID *tmp_enterprise = NULL;
    OctetString *tmp_agent_addr = NULL;
    VarBind *tvb = NULL;
    SnmpMessage *snmp_msg = NULL;

    if (SrInitializeTraps()) {
        goto done;
    }

    if ( (notifyType != SR_NTFY_TYPE_TRAP) && 
         (notifyType != SR_NTFY_TYPE_INFORM) ) {
        goto done;
    }

    if (securityName != (OctetString *)NULL) {
        /* fill in snmp_msg structure */
        snmp_msg = SrFillSnmpMessage(notifyType, securityName, securityLevel,
                                     securityModel, contextName);
        if (snmp_msg == NULL) {
            DPRINTF((APTRACE, "SendNotificationToDestSMIv1Params: SrFillSnmpMessage returned NULL.\n"));
            goto done;
        }
    }

    if (enterprise == NULL) {
        tmp_enterprise = CloneOID(systemData.sysObjectID);
    } else {
        tmp_enterprise = CloneOID(enterprise);
    }
    if (tmp_enterprise == NULL) {
        goto done;
    }

    if (agent_addr == NULL) {
        /* Force update of localAddr LVL7*/
        SrLocalAddrUpdate();

        tmp_agent_addr = CloneOctetString(&localAddr);
    } else {
        tmp_agent_addr = CloneOctetString(agent_addr);
    }
    if (tmp_agent_addr == NULL) {
        goto done;
    }

    pdu = MakePdu(TRAP_TYPE,
                  0L, 0L, 0L,
                  tmp_enterprise,
                  tmp_agent_addr,
                  genTrap,
                  specTrap,
                  GetTimeNow());
    if (pdu == NULL) {
        goto done;
    }
    tmp_enterprise = NULL;
    tmp_agent_addr = NULL;

    if (vb != NULL) {
        tvb = CloneVarBindList(vb);
        if (tvb == NULL) {
            goto done;
        }
    }
    pdu->var_bind_list = tvb;
    if (tvb != NULL) {
        while (tvb->next_var != NULL) {
            tvb = tvb->next_var;
        }
    }
    pdu->var_bind_end_ptr = tvb;
    tvb = NULL;

    SrSendNotifications(notifyType,
                        contextName,
                        pdu,
                        NULL,
                        snmp_msg,
                        only_ti,
                        cfg_chk,
                        agent_addr,
                        retryCount,
                        timeout);

  done:
    if (snmp_msg) {
        SrFreeSnmpMessage(snmp_msg);
        snmp_msg = NULL;
    }
    if (tmp_enterprise) {
        FreeOID(tmp_enterprise);
    }
    if (tmp_agent_addr) {
        FreeOctetString(tmp_agent_addr);
    }
    if (tvb) {
        FreeVarBindList(tvb);
    }
}
#endif /* SR_SNMPv1_PACKET */

void
SendNotificationToDestSMIv2Params(
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
    SR_INT32 cfg_chk)
{
#ifndef SR_SNMPv2_PDU
    SR_INT32 genTrap;
    SR_INT32 specTrap;
    OID *enterprise = NULL;
#else /* SR_SNMPv2_PDU */
    OID *oid = NULL;
    VarBind *vb1 = NULL;
    VarBind *vb2 = NULL;
    VarBind *tvb = NULL;
    Pdu *pdu = NULL;
    SR_INT32 curtime;
    SnmpMessage *snmp_msg = NULL;
#endif /* SR_SNMPv2_PDU */

#ifndef SR_SNMPv2_PDU
    specTrap = 0;

    if (snmpTrapOID == NULL) {
        goto done;
    }
    if (CmpOID(snmpTrapOID, coldStartTrap) == 0) {
        genTrap = COLD_START_TRAP;
    } else
    if (CmpOID(snmpTrapOID, warmStartTrap) == 0) {
        genTrap = WARM_START_TRAP;
    } else
    if (CmpOID(snmpTrapOID, linkDownTrap) == 0) {
        genTrap = LINK_DOWN_TRAP;
    } else
    if (CmpOID(snmpTrapOID, linkUpTrap) == 0) {
        genTrap = LINK_UP_TRAP;
    } else
    if (CmpOID(snmpTrapOID, authenFailureTrap) == 0) {
        genTrap = AUTHEN_FAILURE_TRAP;
    } else
    if (CmpOID(snmpTrapOID, egpNeighborLossTrap) == 0) {
        genTrap = EGP_NEIGHBOR_LOSS_TRAP;
    } else {
        genTrap = ENTERPRISE_TRAP;
        enterprise = CloneOID(snmpTrapOID);
        if (enterprise == NULL) {
            goto done;
        }
        enterprise->length--;
        specTrap = enterprise->oid_ptr[enterprise->length];
        /* If next subid is 0, remove it as well */
        if (enterprise->oid_ptr[enterprise->length - 1] == 0) {
            enterprise->length--;
        }
    }

    if (enterprise == NULL) {
        enterprise = systemData.sysObjectID;
    }

    SendNotificationToDestSMIv1Params(notifyType,
                                      genTrap,
                                      specTrap,
                                      enterprise,
                                      agent_addr,
                                      vb,
                                      contextName,
                                      retryCount,
                                      timeout,
                                      securityName,
                                      securityLevel,
                                      securityModel,
                                      only_ti,
                                      cfg_chk);

  done:
    if ( (genTrap == ENTERPRISE_TRAP) && (enterprise != NULL) ) {
        FreeOID(enterprise);
    }
#else /* SR_SNMPv2_PDU */
    if (SrInitializeTraps()) {
        goto done;
    }

    if (securityName != (OctetString *) NULL) {
        /* fill in snmp_msg structure */
        snmp_msg = SrFillSnmpMessage(notifyType, securityName, securityLevel,
                                     securityModel, contextName);
        if (snmp_msg == NULL) {
            DPRINTF((APTRACE, "SendNotificationToDestSMIv2Params: SrFillSnmpMessage returned NULL.\n"));
            goto done;
        }   
    }

    pdu = MakePdu(SNMPv2_TRAP_TYPE, 0L, 0L, 0L, NULL, NULL, 0L, 0L, 0L);
    if (pdu == NULL) {
        goto done;
    }

    if (vb != NULL) {
        tvb = CloneVarBindList(vb);
        if (tvb == NULL) {
            goto done;
        }
    }

    curtime = GetTimeNow();
    vb1 = MakeVarBindWithValue(&sysUpTime_OID, NULL, TIME_TICKS_TYPE, &curtime);
    if (vb1 == NULL) {
        goto done;
    }

    oid = CloneOID(snmpTrapOID);
    if (oid == NULL) {
        goto done;
    }
    vb2 = MakeVarBindWithValue(&snmpTrapOID_OID, NULL, OBJECT_ID_TYPE, oid);
    if (vb2 == NULL) {
        goto done;
    }
    oid = NULL;

    vb1->next_var = vb2;
    vb2->next_var = tvb;
    tvb = vb1;
    vb1 = NULL;
    vb2 = NULL;

    pdu->var_bind_list = tvb;
    while (tvb->next_var != NULL) {
        tvb = tvb->next_var;
    }
    pdu->var_bind_end_ptr = tvb;
    tvb = NULL;

    SrSendNotifications(notifyType,
                        contextName,
                        NULL,
                        pdu,
                        snmp_msg,
                        only_ti,
                        cfg_chk,
                        agent_addr,
                        retryCount,
                        timeout);

  done:
    if (snmp_msg) {
        SrFreeSnmpMessage(snmp_msg);
        snmp_msg = NULL;
    }
    if (oid) {
        FreeOID(oid);
    }
    if (vb1) {
        FreeVarBindList(vb1);
    }
    if (vb2) {
        FreeVarBindList(vb2);
    }
    if (tvb) {
        FreeVarBindList(tvb);
    }
#endif /* SR_SNMPv2_PDU */
}

/*
 * SrFillSnmpMessage
 *
 * This function will malloc and fill in the header of an SNMP message based 
 * on the information passed in to this function.  The securityModel parameter 
 * is used to determine whether to create an SNMPv1, SNMPv2c, or SNMPv3 message
 * header. The "securityName" corresponds to either a community string for
 * SNMPv1 or SNMPv2c, and to a user name for SNMPv3. The contextName parameter
 * may be used when SrCreateV3SnmpMessage is called.
 *
 * The return value is a filled in SnmpMessage structure on success, 
 * NULL on failure.
 *
 */
static SnmpMessage *
SrFillSnmpMessage(
    SR_INT32 notifyType,
    const OctetString *securityName,
    SR_INT32 securityLevel,
    SR_INT32 securityModel,
    const OctetString *contextName)
{
    SnmpMessage *new_snmp_msg = NULL;
#ifdef SR_SNMPv3_PACKET
    int idx1;
    usmUserEntry_t *uue = NULL;
#ifndef SR_UNSECURABLE
    OctetString *auth_secret;
    SR_INT32 auth_protocol;
    SR_INT32 auth_localized;
#ifndef SR_NO_PRIVACY
    OctetString *priv_secret;
    SR_INT32 priv_protocol;
    SR_INT32 priv_localized;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
#endif /* SR_SNMPv3_PACKET */


    if (securityName == NULL) {
        goto done;
    }
  
    switch (securityModel) {
#ifdef SR_SNMPv1_PACKET
        case SR_SECURITY_MODEL_V1:
            new_snmp_msg =
                SrCreateV1SnmpMessage((OctetString *)securityName, TRUE);
            break;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
        case SR_SECURITY_MODEL_V2C:
           new_snmp_msg =
                SrCreateV2cSnmpMessage((OctetString *)securityName, TRUE);
            break;
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
        case SR_SECURITY_MODEL_USM:
            if (notifyType == SR_NTFY_TYPE_TRAP) {
                agt_usmUserTable.tip[0].value.octet_val = agt_local_snmpID;
                agt_usmUserTable.tip[1].value.octet_val = 
                      (OctetString *)securityName; 
                idx1 = SearchTable(&agt_usmUserTable, EXACT);
                if (idx1 == -1) {
                    goto done;
                }
                uue = (usmUserEntry_t *)agt_usmUserTable.tp[idx1];
#ifndef SR_UNSECURABLE
                auth_secret = uue->auth_secret;
                auth_protocol = SrV3ProtocolOIDToInt(uue->usmUserAuthProtocol);
                auth_localized = 1;
#ifndef SR_NO_PRIVACY
                priv_secret = uue->priv_secret;
                priv_protocol = SrV3ProtocolOIDToInt(uue->usmUserPrivProtocol);
                priv_localized = 1;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
            } else {
/* Handling informs right? */
#ifndef SR_UNSECURABLE
                auth_secret = NULL;
                auth_protocol = 0;
                auth_localized = 0;
#ifndef SR_NO_PRIVACY
                priv_secret = NULL;
                priv_protocol = 0;
                priv_localized = 0;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
            }

            new_snmp_msg =
                SrCreateV3SnmpMessage(
                              securityLevel, 
                              securityModel,
                              agt_local_snmpID,
                              (OctetString *)securityName,
                              agt_local_snmpID,
                              (OctetString *)contextName,
#ifndef SR_UNSECURABLE
                              auth_secret, auth_protocol, auth_localized,
#ifndef SR_NO_PRIVACY
                              priv_secret, priv_protocol, priv_localized,
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
                              TRUE);

            break;
#endif /* SR_SNMPv3_PACKET */
        default:
            goto done;
            break;
    }

  done:
      return (new_snmp_msg);
}

#ifdef SR_RCSID
#undef SR_RCSID
#endif /* SR_RCSID */

#ifdef SR_SNMPv1_ADMIN
#include "notifyv1.c"
#endif /* SR_SNMPv1_ADMIN */

#ifdef SR_SNMPv3_ADMIN
#include "notifyv3.c"
#endif /* SR_SNMPv3_ADMIN */
