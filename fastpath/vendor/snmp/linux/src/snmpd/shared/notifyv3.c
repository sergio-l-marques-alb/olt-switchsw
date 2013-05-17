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

#include "frmt_lib.h"
#include "diag.h"
#include "pclo.h"

#include <errno.h>

#include <malloc.h>

#include <stdlib.h>

#include "sr_snmp.h"

#if defined(SR_SNMPv3_PACKET)
#include "sr_boots.h"
#include "oid_lib.h"
#endif	/* defined(SR_SNMPv3_PACKET) */

#include "v3_vta.h"

#ifdef SR_EPIC

#include "sri/vbtable.h"

#include "epic.h"
#include "epiccore.h"

extern SnmpLcd epic_snmp_lcd;
#endif /* SR_EPIC */

extern SnmpV2Table agt_snmpNotifyTable;
extern SnmpV2Table agt_snmpTargetAddrTable;
extern SnmpV2Table agt_snmpTargetParamsTable;
extern SnmpV2Table agt_vacmSecurityToGroupTable;
extern SnmpV2Table agt_vacmAccessTable;
#ifdef SR_SNMPv3_PACKET
extern SnmpV2Table agt_usmUserTable;
#endif /* SR_SNMPv3_PACKET */
#ifdef SR_NOTIFY_FILTERING
extern SnmpV2Table agt_snmpNotifyFilterProfileTable;
#include "chk_filt.h"
#endif /* SR_NOTIFY_FILTERING */

#ifdef SR_NOTIFY_FULL_COMPLIANCE
#include "sr_fdset.h"
#include "tmq.h"


#include "notify.h"
#include "cacheid.h"


SR_INT32 next_inform_timeout;
static TimeOutDescriptor inform_timeout_tod = {
    (SR_INT32)0,
    (SR_INT32)-1,
    SrInformTimeout,
    (SR_INT32)0,
    (void *)NULL
};

#ifdef SR_SNMPv3_PACKET
static SR_UINT32 notInTimeWindow_sids[] = { 1, 3, 6, 1, 6, 3, 15, 1, 1, 2, 0 };
static SR_UINT32 unknownUserNames_sids[] = { 1, 3, 6, 1, 6, 3, 15, 1, 1, 3, 0 };
static SR_UINT32 unknownEngineIDs_sids[] = { 1, 3, 6, 1, 6, 3, 15, 1, 1, 4, 0 };
static OID notInTimeWindow = { 11, notInTimeWindow_sids };
static OID unknownUserNames = { 11, unknownUserNames_sids };
static OID unknownEngineIDs = { 11, unknownEngineIDs_sids };
#endif /* SR_SNMPv3_PACKET */

typedef struct SR_NotificationRecord NotificationRecord;
struct SR_NotificationRecord {
    SR_INT32 serial_num;
    SR_INT32 cur_timeout;
    SR_INT32 timeout;
    SR_INT32 retries;
    union {
        SR_INT32 spi;
        SR_INT32 securityLevel;
    } u;
    OctetString *pdu_packlet;
    OctetString *dummy_pdu_packlet;
    int use_dummy;
    SnmpMessage *snmp_msg;
    TransportInfo ti;
    NotificationRecord *next;
};

static NotificationRecord *notification_record_queue = NULL;

static void Sr_send_inform(
    NotificationRecord *nr);

static void Sr_init_inform(
    SR_INT32 serial_num,
    SR_INT32 timeout,
    SR_INT32 retries,
    SnmpMessage *snmp_msg,
    Pdu *pdu,
    TransportInfo *dstTI);


extern snmpCommunityEntry_t *FindSnmpCommunityEntry(
    const OctetString *contextName,
    snmpTargetParamsEntry_t *stpe,
    TransportInfo *ti);

static void
Sr_send_inform(
    NotificationRecord *nr)
{
    FNAME("Sr_send_inform")
    TransportInfo *ti = NULL;
#ifdef SR_DEBUG
    char tstr[64];
#endif /* SR_DEBUG */
    Pdu pdu;
#ifdef SR_EPIC
    int i, status;
#endif /* EPIC */

    memset(&pdu, 0, sizeof(Pdu));

    switch (nr->ti.type) {
        case SR_IP_TRANSPORT:
            ipTrapTI.t_ipAddr = nr->ti.t_ipAddr;
            if (nr->ti.t_ipPort == 0) {
            ipTrapTI.t_ipPort = osapiHtons((short)GetSNMPTrapPort()); /* LVL7 */
            } else {
                ipTrapTI.t_ipPort = nr->ti.t_ipPort;
            }
            ti = &ipTrapTI;
    }


    if (ti == NULL) {
        DPRINTF((APERROR, "%s: bad transport\n", Fname));
        return;
    }

    if (nr->use_dummy) {
#ifndef SR_EPIC
        pdu.packlet = nr->dummy_pdu_packlet;
#else /* SR_EPIC */
#ifndef SR_SNMPv3_PACKET
        pdu.packlet = nr->dummy_pdu_packlet;
#else /* SR_SNMPv3_PACKET */
        if ( (nr->snmp_msg->version < SR_SNMPv3_VERSION)
             || ((nr->snmp_msg->version == SR_SNMPv3_VERSION) &&
                 (nr->snmp_msg->u.v3.securityModel) == SR_SECURITY_MODEL_USM)) {
            pdu.packlet = nr->dummy_pdu_packlet;
        }
        else {
            pdu.packlet = nr->pdu_packlet;
        }
#endif /* SR_SNMPv3_PACKET */
#endif /* SR_EPIC */
    } else {
        pdu.packlet = nr->pdu_packlet;
    }

    pdu.type = INFORM_REQUEST_TYPE;
    if (SrBuildSnmpMessage(nr->snmp_msg, &pdu, &agt_snmp_lcd)) {
        return;
    }

#ifdef SR_DEBUG
    packdump("Outgoing Packet",
             (unsigned char *)nr->snmp_msg->packlet->octet_ptr,
             nr->snmp_msg->packlet->length,
             &(nr->ti),
             pdu.var_bind_list,
             0);
#endif /* SR_DEBUG */

    /*
     * Send SNMPv2c and SNMPv3/USM informs directly to the dest.
     */
#ifdef SR_EPIC
#ifdef SR_SNMPv3_PACKET
    if ( (nr->snmp_msg->version < SR_SNMPv3_VERSION)
         || ((nr->snmp_msg->version == SR_SNMPv3_VERSION) &&
             (nr->snmp_msg->u.v3.securityModel) == SR_SECURITY_MODEL_USM)
        ) {
#endif /* SR_SNMPv3_PACKET */
#endif /* SR_EPIC */

        if (SendToTransport((char *) nr->snmp_msg->packlet->octet_ptr,
                            (int) nr->snmp_msg->packlet->length, ti, NULL) < 0) { /* LVL7 */
            DPRINTF((APWARN,
                "%s: Sendto failed, unable to send inform to manager %s.\n",
                Fname, FormatTransportString(tstr, sizeof(tstr), &nr->ti)));
        }
        else {
            DPRINTF((APTRAP, "%s: inform sent to %s\n", Fname,
                FormatTransportString(tstr, sizeof(tstr), &nr->ti)));
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
    if ( (ti->type == SR_IP_TRANSPORT) &&
         (ti->t_ipAddr == inet_addr("127.0.0.1")) ) {
#else /* SR_SNMPv3_PACKET */
    else {
#endif /* SR_SNMPv3_PACKET */
        for (i = 0; i < MAX_EPIC_STREAM; i++) {
#ifndef SR_SNMPv3_PACKET
            if (epicClients[i].fd != 0) {
#else /* SR_SNMPv3_PACKET */
            /*
             * We already know that this an SNMPV3 version inform, so
             * make sure security model matches.
             */
            if ( (epicClients[i].fd != 0) &&
                 (nr->snmp_msg->u.v3.securityModel ==
                  epicClients[i].chosenSecurityModel) ) {
#endif /* SR_SNMPv3_PACKET */
                if (SrBuildSnmpMessage(nr->snmp_msg, &pdu,
                                       &epic_snmp_lcd) != -1) {
                    status = send(epicClients[i].fd,
                                  (char *) nr->snmp_msg->packlet->octet_ptr,
                                  (int) nr->snmp_msg->packlet->length,
                                  0);
                    if (status == -1) {
                        DPRINTF((APTRACE, "%s: send of inform to EPIC client failed: %s\n",
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

static void
Sr_init_inform(
    SR_INT32 serial_num,
    SR_INT32 timeout,
    SR_INT32 retries,
    SnmpMessage *snmp_msg,
    Pdu *pdu,
    TransportInfo *dstTI)
{
    NotificationRecord *nr = NULL;
    SnmpMessage *new_snmp_msg = NULL;
#if defined(SR_SNMPv3_PACKET)
    OctetString unknown_snmp_id = { NULL, 0 };
    OctetString *snmp_id;
#endif	/* defined(SR_SNMPv3_PACKET) */
#ifdef SR_SNMPv3_PACKET
    SR_INT32 securityLevel;
#endif /* SR_SNMPv3_PACKET */
    OctetString *pdu_packlet = NULL, *dummy_pdu_packlet = NULL;
    VarBind *vbl, *vble;
    int use_dummy = 0;
    TransportInfo tmpTI;

    DPRINTF((APTRACE, "Initializing to send InformRequest.\n"));

    pdu->u.normpdu.request_id = serial_num;

    if (BuildPdu(pdu)) {
        goto fail;
    }
    pdu_packlet = pdu->packlet;
    pdu->packlet = NULL;

    vbl = pdu->var_bind_list;
    vble = pdu->var_bind_end_ptr;
    pdu->var_bind_list = NULL;
    pdu->var_bind_end_ptr = NULL;
    if (BuildPdu(pdu)) {
        pdu->var_bind_list = vbl;
        pdu->var_bind_end_ptr = vble;
        goto fail;
    }
    pdu->var_bind_list = vbl;
    pdu->var_bind_end_ptr = vble;
    dummy_pdu_packlet = pdu->packlet;
    pdu->packlet = NULL;

    switch (snmp_msg->version) {
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
            new_snmp_msg =
                SrCreateV2cSnmpMessage(snmp_msg->u.v1.community, TRUE);
            break;
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
        case SR_SNMPv3_VERSION:
            securityLevel = snmp_msg->u.v3.securityLevel;
            memset(&tmpTI, '0', sizeof(TransportInfo));
            CopyIpTransportInfo(&tmpTI, dstTI);

            switch (tmpTI.type) {
                case SR_IP_TRANSPORT:
                    if (tmpTI.t_ipPort == 0) {
                        tmpTI.t_ipPort = osapiHtons((short)GetSNMPTrapPort()); /* LVL7 */
                    } 
                    break;
                default:
                    break;
            }
            snmp_id = LookupSnmpID(&tmpTI);
            if (snmp_id == NULL) {
                securityLevel = SR_SECURITY_LEVEL_NOAUTH;
                snmp_id = &unknown_snmp_id;
                use_dummy = 1;
            }
            new_snmp_msg =
                SrCreateV3SnmpMessage(securityLevel,
                                      snmp_msg->u.v3.securityModel,
                                      snmp_id,
                                      snmp_msg->u.v3.userName,
                                      snmp_msg->u.v3.contextSnmpEngineID,
                                      snmp_msg->u.v3.contextName,
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
            if (use_dummy != 1) {
                FreeOctetString(snmp_id);
                snmp_id = NULL;
            }
            break;
#endif /* SR_SNMPv3_PACKET */
    }
    if (new_snmp_msg == NULL) {
        goto fail;
    }

    nr = (NotificationRecord *)malloc(sizeof(NotificationRecord));
    if (nr == NULL) {
        goto fail;
    }
    SR_CLEAR(nr);
    nr->serial_num = serial_num;
    nr->cur_timeout = timeout;
    nr->timeout = timeout;
    nr->retries = retries;
    nr->pdu_packlet = pdu_packlet;
    nr->dummy_pdu_packlet = dummy_pdu_packlet;
    nr->use_dummy = use_dummy;
#ifdef SR_SNMPv3_PACKET
    if (snmp_msg->version == SR_SNMPv3_VERSION) {
        nr->u.securityLevel = snmp_msg->u.v3.securityLevel;
    }
#endif /* SR_SNMPv3_PACKET */
    nr->snmp_msg = new_snmp_msg;
    nr->ti.type = dstTI->type;
    CopyTransportAddr(&nr->ti, dstTI);
    nr->next = notification_record_queue;
    notification_record_queue = nr;

    pdu_packlet = NULL;
    dummy_pdu_packlet = NULL;
    new_snmp_msg = NULL;

    Sr_send_inform(nr);

    next_inform_timeout = nr->cur_timeout;
    while (nr != NULL) {
        if (nr->cur_timeout < next_inform_timeout) {
            next_inform_timeout = nr->cur_timeout;
        }
        nr = nr->next;
    }
    if (inform_timeout_tod.TimerId != -1) {
        CancelTimeout(inform_timeout_tod.TimerId);
    }
    inform_timeout_tod.TimerId =
        SetOneshotTimeout(GetTimeNow() + (SR_UINT32)next_inform_timeout,
                          (void *)&inform_timeout_tod);
    return;

  fail:
    if (new_snmp_msg != NULL) {
        SrFreeSnmpMessage(new_snmp_msg);
    }
    if (pdu_packlet != NULL) {
        FreeOctetString(pdu_packlet);
    }
    if (dummy_pdu_packlet != NULL) {
        FreeOctetString(dummy_pdu_packlet);
    }
    if (nr != NULL) {
        if (nr->pdu_packlet != NULL) {
            FreeOctetString(nr->pdu_packlet);
        }
        if (nr->dummy_pdu_packlet != NULL) {
            FreeOctetString(nr->dummy_pdu_packlet);
        }
        if (nr->snmp_msg != NULL) {
            SrFreeSnmpMessage(nr->snmp_msg);
        }
        free(nr);
    }
    return;
}

int
SrIsInformTimeout(
    TimeOutDescriptor *tod)
{
    if (tod == &inform_timeout_tod) {
        return 1;
    } 
    return 0;
}

void
SrInformTimeout(
    TimeOutDescriptor *tod)
{
    NotificationRecord *nr, **pnr;

    DPRINTF((APTRACE, "InformRequest timed out\n"));

    inform_timeout_tod.TimerId = -1;

    pnr = &notification_record_queue;
    while ((*pnr) != NULL) {
        nr = *pnr;
        if (next_inform_timeout >= nr->cur_timeout) {
            if (nr->retries == 0) {
                *pnr = nr->next;
                if (nr->pdu_packlet != NULL) {
                    FreeOctetString(nr->pdu_packlet);
                }
                if (nr->dummy_pdu_packlet != NULL) {
                    FreeOctetString(nr->dummy_pdu_packlet);
                }
                if (nr->snmp_msg != NULL) {
                    SrFreeSnmpMessage(nr->snmp_msg);
                }
                free(nr);
                continue;
            } else {
                nr->retries--;
                nr->cur_timeout = nr->timeout;
                Sr_send_inform(nr);
            }
        } else {
            nr->cur_timeout -= next_inform_timeout;
        }
        pnr = &((*pnr)->next);
    }

    next_inform_timeout = 0;
    if (notification_record_queue != NULL) {
        nr = notification_record_queue;
        next_inform_timeout = nr->cur_timeout;
        while (nr != NULL) {
            if (nr->cur_timeout < next_inform_timeout) {
                next_inform_timeout = nr->cur_timeout;
            }
            nr = nr->next;
        }
        inform_timeout_tod.TimerId = SetOneshotTimeout(GetTimeNow() + (SR_UINT32)next_inform_timeout,
                          (void *)&inform_timeout_tod);
    }
}

void
SrInformResponse(
    TransportInfo *ti)
{
    static unsigned char *trap_pkt = NULL;
#ifdef SR_DEBUG
    unsigned char *engine_id_text = NULL;
#endif /* SR_DEBUG */
    int length;
    SnmpMessage *snmp_msg = NULL;
    Pdu *pdu;
    NotificationRecord **pnr, *nr;
    SR_INT32 sn;
#if defined(SR_SNMPv3_PACKET)
    int resend = 0;
    SR_INT32 curtime;
    OctetString *tmp_engine_id = NULL;
#endif	/* defined(SR_SNMPv3_PACKET) */

    DPRINTF((APTRACE, "Got response/report to InformRequest\n"));
    if (trap_pkt == NULL) {
        trap_pkt = (unsigned char *)malloc(sr_pkt_size);
        if (trap_pkt == NULL) {
            return;
        }
    }

    length = ReceiveFromTransport((char *)trap_pkt, sr_pkt_size, ti, NULL); /* LVL7 */
    if (length <= 0) {
        return;
    }
    else {
        snmpData.snmpInPkts++;
    }

    snmp_msg = SrParseSnmpMessage(&agt_snmp_lcd, ti, NULL, trap_pkt, length);
    if (snmp_msg == NULL) {
        DPRINTF((APWARN,
            "Could not parse response/report to InformRequest\n"));
        return;
    }

    if (snmp_msg->error_code) {
        DPRINTF((APWARN,
            "Error parsing response/report to InformRequest, error code %d\n",
            snmp_msg->error_code)); 
        switch (snmp_msg->error_code) {
            case ASN_PARSE_ERROR:
                snmpData.snmpInASNParseErrs++;
                break;
            case SNMP_BAD_VERSION_ERROR:
                snmpData.snmpInBadVersions++;
                break;
#ifdef SR_SNMPv3_PACKET
            case SR_SNMP_UNKNOWN_SECURITY_MODELS:
                agt_snmp_v3_lcd.snmpMPDStatsData->snmpUnknownSecurityModels++;
                break;
            case SR_SNMP_INVALID_MSGS:
                agt_snmp_v3_lcd.snmpMPDStatsData->snmpInvalidMsgs++;
                break;
            case SR_SNMP_UNKNOWN_PDU_HANDLERS:
                agt_snmp_v3_lcd.snmpMPDStatsData->snmpUnknownPDUHandlers++;
                break;
            case SR_USM_UNSUPPORTED_SEC_LEVELS:
                agt_snmp_v3_lcd.usmStatsData->usmStatsUnsupportedSecLevels++;
                break;
            case SR_USM_NOT_IN_TIME_WINDOWS:
                agt_snmp_v3_lcd.usmStatsData->usmStatsNotInTimeWindows++;
                break;
            case SR_USM_UNKNOWN_USER_NAMES:
                agt_snmp_v3_lcd.usmStatsData->usmStatsUnknownUserNames++;
                break;
            case SR_USM_UNKNOWN_ENGINE_IDS:
                agt_snmp_v3_lcd.usmStatsData->usmStatsUnknownEngineIDs++;
                break;
            case SR_USM_WRONG_DIGESTS:
                agt_snmp_v3_lcd.usmStatsData->usmStatsWrongDigests++;
                break;
            case SR_USM_DECRYPTION_ERRORS:
                agt_snmp_v3_lcd.usmStatsData->usmStatsDecryptionErrors++;
                break;
#endif /* SR_SNMPv3_PACKET */
        }
        SrFreeSnmpMessage(snmp_msg);
        return;
    }

    pdu = SrParsePdu(snmp_msg->packlet->octet_ptr, snmp_msg->packlet->length);
    if (pdu == NULL) {
        SrFreeSnmpMessage(snmp_msg);
        return;
    }

#ifdef SR_DEBUG
        packdump("Incoming Response/Report Packet",
                 (unsigned char *)snmp_msg->packlet->octet_ptr,
                 snmp_msg->packlet->length,
                 ti,
                 pdu->var_bind_list,
                 0);
#endif /* SR_DEBUG */

    sn = pdu->u.normpdu.request_id;
    DPRINTF((APVERBOSE, "Incoming inform has request id of %d\n", sn));
    for (nr = notification_record_queue; nr != NULL; nr = nr->next) {
        if (sn == nr->serial_num) {
            goto found_pending_inform;
        }
    }
#ifdef SR_SNMPv3_PACKET
    /* 
     * could not find the matched pending inform.
     * if incoming message type is REPORT with unknown EngineID,
     * flush the old snmpEngineID cache.
     * possiblly  manager's snmpEngineID has been changed.
     */
    if ((pdu->type == REPORT_TYPE) && 
	       (snmp_msg->version == SR_SNMPv3_VERSION)) {
        /* Check for unknownEngineIDs report */
        if (!CmpOID(&unknownEngineIDs, pdu->var_bind_list->name)) {
            /* flush the original BootsAndTime cache. */ 
            tmp_engine_id = LookupSnmpID(ti);
            if (tmp_engine_id != NULL) {
                DeleteBootsAndTimeEntry(tmp_engine_id);
                FreeOctetString(tmp_engine_id);
            }
            InsertSnmpID(snmp_msg->u.v3.authSnmpEngineID, ti);
        }
    }
#endif /* SR_SNMPv3_PACKET */
    SrFreeSnmpMessage(snmp_msg);
    FreePdu(pdu);
    return;

  found_pending_inform:
    if (pdu->type == REPORT_TYPE) {
#ifdef SR_SNMPv3_PACKET
        if (snmp_msg->version != SR_SNMPv3_VERSION) {
            DPRINTF((APWARN,
                 "Inform response/report not SNMPv3, dropping response\n"));
            SrFreeSnmpMessage(snmp_msg);
            return;
        }
        /* Check for notInTimeWindow report */
        if (!CmpOID(&notInTimeWindow, pdu->var_bind_list->name)) {
            curtime = GetTimeNow() / 100;
            SetBootsAndTimeEntry(snmp_msg->u.v3.authSnmpEngineID,
                                 snmp_msg->u.v3.authSnmpEngineBoots,
                                 snmp_msg->u.v3.authSnmpEngineTime - curtime,
                                 snmp_msg->u.v3.authSnmpEngineTime - curtime);
            DPRINTF((APTRACE, "Inform response not in time window\n")); 
            resend = 1;
        }

        /* Check for unknownEngineIDs report */
        if (!CmpOID(&unknownEngineIDs, pdu->var_bind_list->name)) {
#ifdef SR_DEBUG
            engine_id_text = (unsigned char *)
                malloc(snmp_msg->u.v3.authSnmpEngineID->length * 5);
            if (engine_id_text != NULL) {
                SPrintOctetString(snmp_msg->u.v3.authSnmpEngineID,
                                  77, (char *)engine_id_text);
                DPRINTF((APTRACE,
                    "Inform response has unknown EngineID, the correct one is: %s\n",
                    engine_id_text)); 
                free(engine_id_text);
            }
#endif /* SR_DEBUG */
            /* flush the original BootsAndTime cache. */ 
            tmp_engine_id = LookupSnmpID(ti);
            if (tmp_engine_id != NULL) {
                DeleteBootsAndTimeEntry(tmp_engine_id);
                FreeOctetString(tmp_engine_id);
            }
            InsertSnmpID(snmp_msg->u.v3.authSnmpEngineID, ti);
            FreeOctetString(nr->snmp_msg->u.v3.authSnmpEngineID);
            nr->snmp_msg->u.v3.authSnmpEngineID =
                snmp_msg->u.v3.authSnmpEngineID;
            snmp_msg->u.v3.authSnmpEngineID = NULL;
            nr->use_dummy = 0;
            nr->snmp_msg->u.v3.securityLevel = nr->u.securityLevel;
            resend = 1;
        }
        /* Check for unknownUserNames report */
        if (!CmpOID(&unknownUserNames, pdu->var_bind_list->name)) {
            if (CmpOctetStrings(snmp_msg->u.v3.authSnmpEngineID,
                                nr->snmp_msg->u.v3.authSnmpEngineID)) {
                InsertSnmpID(snmp_msg->u.v3.authSnmpEngineID, ti);
                FreeOctetString(nr->snmp_msg->u.v3.authSnmpEngineID);
                nr->snmp_msg->u.v3.authSnmpEngineID =
                    snmp_msg->u.v3.authSnmpEngineID;
                snmp_msg->u.v3.authSnmpEngineID = NULL;
                nr->use_dummy = 0;
                nr->snmp_msg->u.v3.securityLevel = nr->u.securityLevel;
                DPRINTF((APTRACE, "Inform response has unknown user name\n")); 
                resend = 1;
            }
        }
#endif /* SR_SNMPv3_PACKET */
    }

    SrFreeSnmpMessage(snmp_msg);
    FreePdu(pdu);

    if (inform_timeout_tod.TimerId != -1) {
        CancelTimeout(inform_timeout_tod.TimerId);
        inform_timeout_tod.TimerId = -1;
    }

    if (resend) {
#ifdef SR_SNMPv3_PACKET
        if (nr->snmp_msg->version == SR_SNMPv3_VERSION) {
            nr->snmp_msg->u.v3.securityLevel = nr->u.securityLevel;
        }
#endif /* SR_SNMPv3_PACKET */
        Sr_send_inform(nr);
    } else {
        pnr = &notification_record_queue;
        while ((*pnr) != NULL) {
            nr = *pnr;
            if (sn == nr->serial_num) {
                *pnr = nr->next;
                if (nr->pdu_packlet != NULL) {
                    FreeOctetString(nr->pdu_packlet);
                }
                if (nr->dummy_pdu_packlet != NULL) {
                    FreeOctetString(nr->dummy_pdu_packlet);
                }
                if (nr->snmp_msg != NULL) {
                    SrFreeSnmpMessage(nr->snmp_msg);
                }
               free(nr);
               DPRINTF((APTRACE, "Inform response received without error\n"));
               break;
            }
            pnr = &((*pnr)->next);
        }
    }

    next_inform_timeout = 0;
    if (notification_record_queue != NULL) {
        nr = notification_record_queue;
        next_inform_timeout = nr->cur_timeout;
        while (nr != NULL) {
            if (nr->cur_timeout < next_inform_timeout) {
                next_inform_timeout = nr->cur_timeout;
            }
            nr = nr->next;
        }
        inform_timeout_tod.TimerId = SetOneshotTimeout(GetTimeNow() + (SR_UINT32)next_inform_timeout,
                          (void *)&inform_timeout_tod);
    }
}


/*
 *   Non-packaged agent functions for handling inform file descriptors.
 */
int
SrSetInformFds(
    fd_set *fds,
    int maxfd)
{
    if (SrInitializeTraps()) {
        return maxfd;
    }

    switch (bind_ip_proto) {
        default:
            SR_FD_SET(ipTrapTI.fd, fds);
            if (ipTrapTI.fd > maxfd) {
                maxfd = ipTrapTI.fd;
            }
            break;
    }


    return maxfd;
}

int
SrCheckInformFds(
    fd_set *fds,
    int numfds)
{
    switch (bind_ip_proto) {
        default:
            if (FD_ISSET(ipTrapTI.fd, fds)) {
                FD_CLR(ipTrapTI.fd, fds);
                numfds--;
                SrInformResponse(&ipTrapTI);
            }
            break;
    }


    return numfds;
}

#endif /* SR_NOTIFY_FULL_COMPLIANCE */


static SR_INT32 notify_serial_num = 1;

static void SrGenerateNotification(
    const OctetString *contextName,
    Pdu *v1_pdu,
    Pdu *other_pdu,
    snmpNotifyEntry_t *sne,
    snmpTargetAddrEntry_t *stae,
    snmpTargetParamsEntry_t *stpe,
    TransportInfo *ti);

static void
SrGenerateNotification(
    const OctetString *contextName,
    Pdu *v1_pdu,
    Pdu *other_pdu,
    snmpNotifyEntry_t *sne,
    snmpTargetAddrEntry_t *stae,
    snmpTargetParamsEntry_t *stpe,
    TransportInfo *ti)
{
    int idx1, first_index;
    vacmSecurityToGroupEntry_t *vstge = NULL;
    vacmAccessEntry_t *vae = NULL;
    VarBind *vb = NULL;
    OID *ssto = NULL;
    OID *esto = NULL;
    SnmpMessage *snmp_msg = NULL;

#ifdef SR_SNMPv1_WRAPPER
    snmpCommunityEntry_t *sce;
#endif /* SR_SNMPv1_WRAPPER */
#ifdef SR_SNMPv3_PACKET
    usmUserEntry_t *uue = NULL;
#endif /* SR_SNMPv3_PACKET */
    Pdu *pdu = NULL;
#ifdef SR_NOTIFY_FILTERING
    VarBind *fvb = NULL;
    snmpNotifyFilterProfileEntry_t *snfpe = NULL;
#endif /* SR_NOTIFY_FILTERING */
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

    /* Bump serial number */
        if (notify_serial_num == 0x7fffffff) {
            notify_serial_num = 1;
        } else {
            notify_serial_num++;
        }

    /* Find group name */
    agt_vacmSecurityToGroupTable.tip[0].value.uint_val =
        stpe->snmpTargetParamsSecurityModel;
    agt_vacmSecurityToGroupTable.tip[1].value.octet_val =
        stpe->snmpTargetParamsSecurityName;

    idx1 = SearchTable(&agt_vacmSecurityToGroupTable, EXACT);
    if (idx1 == -1) {
        goto done;
    }
    vstge = (vacmSecurityToGroupEntry_t *)agt_vacmSecurityToGroupTable.tp[idx1];

    vae = LookupVacmAccessEntryWithMask(vstge->vacmGroupName,
                                        (OctetString *)contextName,
                                        stpe->snmpTargetParamsSecurityModel,
                                        stpe->snmpTargetParamsSecurityLevel);
    if (vae == NULL) {
        goto done;
    }

    switch (stpe->snmpTargetParamsMPModel) {
#ifdef SR_SNMPv1_PACKET
        case SR_SNMPv1_VERSION:
            vb = v1_pdu->var_bind_list;
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
            break;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2_PDU
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
        case SR_SNMPv3_VERSION:
#endif /* SR_SNMPv3_PACKET */
            vb = other_pdu->var_bind_list;
            if (vb != NULL) {
                if (vb->next_var != NULL) {
                    if (vb->next_var->value.type == OBJECT_ID_TYPE) {
                        ssto = vb->next_var->value.oid_value;
                    }
                }
            }
            break;
#endif /* SR_SNMPv2_PDU */
    }

#ifdef SR_NOTIFY_FILTERING
    fvb = vb;
#endif /* SR_NOTIFY_FILTERING */

    first_index = -1;
    if (ssto != NULL) {
        if (!SrCheckMIBView(ssto,
                            vae->vacmAccessNotifyViewName,
                            &first_index)) {
            goto done;
        }
    }
    if (esto != NULL) {
        if (!SrCheckMIBView(esto,
                            vae->vacmAccessNotifyViewName,
                            &first_index)) {
            goto done;
        }
    }
    while (vb != NULL) {
        if (!SrCheckMIBView(vb->name,
                            vae->vacmAccessNotifyViewName,
                            &first_index)) {
            goto done;
        }
        vb = vb->next_var;
    }
#ifdef SR_NOTIFY_FILTERING
    /* Filtering should be performed here */
    agt_snmpNotifyFilterProfileTable.tip[0].value.octet_val =
                                          stpe->snmpTargetParamsName;
    idx1 = SearchTable(&agt_snmpNotifyFilterProfileTable, EXACT);
    if (idx1 != -1) {
        snfpe = (snmpNotifyFilterProfileEntry_t *)
                    agt_snmpNotifyFilterProfileTable.tp[idx1];
        if (snfpe->snmpNotifyFilterProfileName != NULL) {
            first_index = -1;
            if (ssto != NULL) {
                if (SrCheckNotificationFilter(
                        ssto,
                        snfpe->snmpNotifyFilterProfileName,
                        &first_index)) {
                    goto done;
                }
            }
            if (esto != NULL) {
                if (SrCheckNotificationFilter(
                        esto,
                        snfpe->snmpNotifyFilterProfileName,
                        &first_index)) {
                    goto done;
                }
            }
            while (fvb != NULL) {
                if (SrCheckNotificationFilter(
                        fvb->name,
                        snfpe->snmpNotifyFilterProfileName,
                        &first_index) == 1) {
                    goto done;
                }
                fvb = fvb->next_var;
            }
        }
    }
#endif /* SR_NOTIFY_FILTERING */

    switch (stpe->snmpTargetParamsMPModel) {
#ifdef SR_SNMPv1_PACKET
        case SR_SNMPv1_VERSION:
	    sce = FindSnmpCommunityEntry(contextName, stpe, ti);
	    if (sce != NULL) {
               snmp_msg = SrCreateV1SnmpMessage(sce->snmpCommunityName, TRUE);
            }
            pdu = v1_pdu;
            break;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
            sce = FindSnmpCommunityEntry(contextName, stpe, ti);
	    if (sce != NULL) {
              snmp_msg = SrCreateV2cSnmpMessage(sce->snmpCommunityName, TRUE);
            }
            pdu = other_pdu;
            pdu->type = SNMPv2_TRAP_TYPE;
            pdu->u.normpdu.request_id = notify_serial_num;
            break;
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
        case SR_SNMPv3_VERSION:
#ifndef SR_EPIC
            if (sne->snmpNotifyType == SR_NTFY_TYPE_TRAP) {
#else /* SR_EPIC */
            if ((sne->snmpNotifyType == SR_NTFY_TYPE_TRAP) &&
                (stpe->snmpTargetParamsSecurityModel ==
                 SR_SECURITY_MODEL_USM)) {
#endif /* SR_EPIC */
                agt_usmUserTable.tip[0].value.octet_val = agt_local_snmpID;
                agt_usmUserTable.tip[1].value.octet_val =
                    stpe->snmpTargetParamsSecurityName;
                idx1 = SearchTable(&agt_usmUserTable, EXACT);
                if (idx1 == -1) {
                    goto done;
                }
                uue = (usmUserEntry_t *)agt_usmUserTable.tp[idx1];
#ifndef SR_UNSECURABLE
                auth_secret = uue->auth_secret;
                auth_protocol = SrV3ProtocolOIDToInt(uue->usmUserAuthProtocol);
                if ((auth_protocol == 0) &&
                    (stpe->snmpTargetParamsSecurityLevel != 
                     SR_SECURITY_LEVEL_NOAUTH)) {
                   /* user not configured for authentication */
                   goto done;
                }
                auth_localized = 1;
#ifndef SR_NO_PRIVACY
                priv_secret = uue->priv_secret;
                priv_protocol = SrV3ProtocolOIDToInt(uue->usmUserPrivProtocol);
                if ((priv_protocol == 0) &&
                    (stpe->snmpTargetParamsSecurityLevel == 
                     SR_SECURITY_LEVEL_PRIV)) {
                   /* user not configured for privacy */
                   goto done;
                }
                priv_localized = 1;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
            } else {
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

            snmp_msg =
                SrCreateV3SnmpMessage(
                              stpe->snmpTargetParamsSecurityLevel,
                              stpe->snmpTargetParamsSecurityModel,
                              agt_local_snmpID,
                              stpe->snmpTargetParamsSecurityName,
                              agt_local_snmpID,
                              (OctetString *)contextName,
#ifndef SR_UNSECURABLE
                              auth_secret, auth_protocol, auth_localized,
#ifndef SR_NO_PRIVACY
                              priv_secret, priv_protocol, priv_localized,
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
                              TRUE);
            pdu = other_pdu;
                pdu->type = SNMPv2_TRAP_TYPE;
            pdu->u.normpdu.request_id = notify_serial_num;
            break;
#endif /* SR_SNMPv3_PACKET */
    }

    if (snmp_msg == NULL) {
        goto done;
    }

    if ( (sne->snmpNotifyType == SR_NTFY_TYPE_TRAP) ||
         (stpe->snmpTargetParamsMPModel == SR_SNMPv1_VERSION) )   {
        Sr_send_trap_ctx(snmp_msg, pdu, ti, contextName);
#ifdef SR_DEBUG
        if (snmp_msg->packlet != NULL) {
            packdump("Outgoing Packet",
                     (unsigned char *)snmp_msg->packlet->octet_ptr,
                     snmp_msg->packlet->length,
                     ti,
                     pdu->var_bind_list,
                     0);
        }
#endif /* SR_DEBUG */
    } else {
#ifdef SR_NOTIFY_FULL_COMPLIANCE
            pdu->type = INFORM_REQUEST_TYPE;
        Sr_init_inform(notify_serial_num,
                       stae->snmpTargetAddrTimeout,
                       stae->snmpTargetAddrRetryCount,
                       snmp_msg,
                       pdu,
                       ti);
#endif /* SR_NOTIFY_FULL_COMPLIANCE */
    }

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
 * The basic algorithm is as follows (all entries must be active):
 *
 *    if only_snmp_msg != NULL and only_ti != NULL and cfg_chk = 0,
 *            then simply send the notification using those parameters,
 *            and return
 *    foreach snmpNotifyEntry SNE in the snmpNotifyTable
 *      foreach snmpTargetAddrEntry STAE in snmpTargetAddrTable
 *        if tag in SNE not present in taglist in STAE, continue
 *        if only_ti != NULL and SNE does not match only_ti, continue
 *        if snmpTargetParamsEntry STPE in snmpTargetParamsTable referenced by
 *                snmpTargetAddrParams in STAE does not exist, continue
 *        if only_snmp_msg != NULL and STAE does not match only_snmp_msg, cont
 *        call SrGenerateNotification
 */
static void
SrGenerateNotifications(
    SR_INT32 type,
    const OctetString *contextName,
    Pdu *v1_pdu,
    Pdu *other_pdu,
    SnmpMessage *only_snmp_msg,
    TransportInfo *only_ti,
    SR_INT32 cfg_chk)
{
    snmpNotifyEntry_t *sne;
    snmpTargetAddrEntry_t *stae;
    snmpTargetParamsEntry_t *stpe;
    int idx1, idx2, idx3;
    TransportInfo ti;
    int pdu_version = 0;

    if ((only_snmp_msg != NULL) && (only_ti != NULL) && (cfg_chk == 0)) {
#if (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU))
        if (only_snmp_msg->version == SR_SNMPv1_VERSION) {
            pdu_version = SR_SNMPv1_PDU_PAYLOAD;
        } else if (only_snmp_msg->version == SR_SNMPv2_VERSION) {
            pdu_version = SR_SNMPv2_PDU_PAYLOAD;
        } else if (only_snmp_msg->version == SR_SNMPv3_VERSION) {
            pdu_version = SR_SNMPv2_PDU_PAYLOAD;
        }
 
        /* Perform pdu conversion if necessary */
        switch (only_ti->type) {
            default:    
                if (convert_pdu(only_snmp_msg->version, 
                             &v1_pdu, &other_pdu, SR_IP_TRANSPORT, pdu_version)) {
                    goto done;
                }
                break;
        }
#endif	/* (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU)) */
        switch (only_snmp_msg->version) {
#ifdef SR_SNMPv1_PACKET
            case SR_SNMPv1_VERSION:
                if (type != SR_NTFY_TYPE_INFORM) {
                    Sr_send_trap_ctx(only_snmp_msg, v1_pdu, only_ti,
                                     contextName);
                }
                break;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2_PDU
#ifdef SR_SNMPv2c_PACKET
            case SR_SNMPv2c_VERSION:
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
            case SR_SNMPv3_VERSION:
#endif /* SR_SNMPv3_PACKET */
                if (type == SR_NTFY_TYPE_INFORM) {
#ifdef SR_NOTIFY_FULL_COMPLIANCE
                    other_pdu->type = INFORM_REQUEST_TYPE;
                    Sr_init_inform(0,
                                   0,
                                   0,
                                   only_snmp_msg,
                                   other_pdu,
                                   only_ti);
#endif /* SR_NOTIFY_FULL_COMPLIANCE */
                } else {
                    other_pdu->type = SNMPv2_TRAP_TYPE;
                    Sr_send_trap_ctx(only_snmp_msg, other_pdu, only_ti,
                                     contextName);
                }
                break;
#endif /* SR_SNMPv2_PDU */
        }
        goto done;
    }


    for (idx1 = 0; idx1 < agt_snmpNotifyTable.nitems; idx1++) {
        sne = (snmpNotifyEntry_t *)agt_snmpNotifyTable.tp[idx1];
        if (sne->snmpNotifyRowStatus != RS_ACTIVE) {
            continue;
        }
        if (type != SR_NTFY_TYPE_EITHER) {
            if (type != sne->snmpNotifyType) {
                continue;
            }
        }

        for (idx2 = 0; idx2 < agt_snmpTargetAddrTable.nitems; idx2++) {
            stae = (snmpTargetAddrEntry_t *)agt_snmpTargetAddrTable.tp[idx2];
            if (stae->snmpTargetAddrRowStatus != RS_ACTIVE) {
                continue;
            }

            /* Check if the tag selects this entry */
            if (!SrTagInTagList(sne->snmpNotifyTag,
                                stae->snmpTargetAddrTagList)) {
                continue;
            }

            /* Convert transport info into TransportInfo structure */
            if (OIDAndOctetStringToTransportInfo(&ti,
                   stae->snmpTargetAddrTDomain,
                   stae->snmpTargetAddrTAddress)) {
                continue;
            }

            /* Check if it matches only_ti if necessary */
            if (only_ti != NULL) {
                if (CmpTransportInfo(&ti, only_ti)) {
                    continue;
                }
            }

            /* Find corresponding snmpTargetParamsEntry */
            agt_snmpTargetParamsTable.tip[0].value.octet_val =
                stae->snmpTargetAddrParams;
            idx3 = SearchTable(&agt_snmpTargetParamsTable, EXACT);
            if (idx3 == -1) {
                continue;
            }
            stpe =
                (snmpTargetParamsEntry_t *)agt_snmpTargetParamsTable.tp[idx3];
            if (stpe->snmpTargetParamsRowStatus != RS_ACTIVE) {
                continue;
            }
#ifdef SR_SNMPv1_PACKET
            if ((sne->snmpNotifyType == SR_NTFY_TYPE_INFORM) &&
                (stpe->snmpTargetParamsMPModel == SR_SNMPv1_VERSION)) {
            }
#endif /* SR_SNMPv1_PACKET */

            /* Check against only_snmp_msg here */
            if (only_snmp_msg != NULL) {
                if (only_snmp_msg->version != stpe->snmpTargetParamsMPModel) {
                    continue;
                }
                switch (only_snmp_msg->version) {
#ifdef SR_SNMPv1_PACKET
                    case SR_SNMPv1_VERSION:
                        if (stpe->snmpTargetParamsSecurityModel !=
                                SR_SECURITY_MODEL_V1) {
                            continue;
                        }
                        if (stpe->snmpTargetParamsSecurityLevel !=
                                SR_SECURITY_LEVEL_NOAUTH) {
                            continue;
                        }
                        if (only_snmp_msg->u.v1.community != NULL) {
                            if (CmpOctetStrings(only_snmp_msg->u.v1.community,
                                    stpe->snmpTargetParamsSecurityName)) {
                                continue;
                            }
                        }
                        break;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
                    case SR_SNMPv2c_VERSION:
                        if (stpe->snmpTargetParamsSecurityModel !=
                                SR_SECURITY_MODEL_V2C) {
                            continue;
                        }
                        if (stpe->snmpTargetParamsSecurityLevel !=
                                SR_SECURITY_LEVEL_NOAUTH) {
                            continue;
                        }
                        if (only_snmp_msg->u.v1.community != NULL) {
                            if (CmpOctetStrings(only_snmp_msg->u.v1.community,
                                    stpe->snmpTargetParamsSecurityName)) {
                                continue;
                            }
                        }
                        break;
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
                    case SR_SNMPv3_VERSION:
                        if (only_snmp_msg->u.v3.securityModel != 0) {
                            if (only_snmp_msg->u.v3.securityModel !=
                                    stpe->snmpTargetParamsSecurityModel) {
                                continue;
                            }
                        }
                        if (only_snmp_msg->u.v3.securityLevel != 0) {
                            if (only_snmp_msg->u.v3.securityLevel !=
                                    stpe->snmpTargetParamsSecurityLevel) {
                                continue;
                            }
                        }
                        if (only_snmp_msg->u.v3.userName != NULL) {
                            if (!CmpOctetStrings(
                                    only_snmp_msg->u.v3.userName,
                                    stpe->snmpTargetParamsSecurityName)) {
                                continue;
                            }
                        }
                        break;
#endif /* SR_SNMPv3_PACKET */
                }
            }

#if (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU))
            /* Perform pdu conversion if necessary */
            if (stpe->snmpTargetParamsMPModel == SR_SNMPv1_VERSION) {
                pdu_version = SR_SNMPv1_PDU_PAYLOAD;
            } else if (stpe->snmpTargetParamsMPModel == SR_SNMPv2c_VERSION) {
                pdu_version = SR_SNMPv2_PDU_PAYLOAD;
            } else {
                    pdu_version = SR_SNMPv2_PDU_PAYLOAD;
            }
            if (convert_pdu(stpe->snmpTargetParamsMPModel, 
                            &v1_pdu, &other_pdu, SR_IP_TRANSPORT, pdu_version)) {
                continue;
            }
#endif	/* (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU)) */

            /* Call SrGenerateNotification */
            SrGenerateNotification(contextName,
                                   v1_pdu,
                                   other_pdu,
                                   sne,
                                   stae,
                                   stpe,
                                   &ti);
        }
    }

  done:
    if (v1_pdu) {
        FreePdu(v1_pdu);
    }
    if (other_pdu) {
        FreePdu(other_pdu);
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
 * The basic algorithm is as follows (all entries must be active):
 *
 *    if snmp_msg != NULL and only_ti != NULL and cfg_chk = 0,
 *            then simply send the notification using those parameters,
 *            and return
 *    foreach snmpNotifyEntry SNE in the snmpNotifyTable
 *      foreach snmpTargetAddrEntry STAE in snmpTargetAddrTable
 *        if tag in SNE not present in taglist in STAE, continue
 *        if only_ti != NULL and SNE does not match only_ti, continue
 *        if snmpTargetParamsEntry STPE in snmpTargetParamsTable referenced by
 *                snmpTargetAddrParams in STAE does not exist, continue
 *        if snmp_msg != NULL and STAE does not match snmp_msg, cont
 *        call SrGenerateNotification
 */
static void
SrSendNotifications(
    SR_INT32 notifyType,
    const OctetString *contextName,
    Pdu *v1_pdu,
    Pdu *other_pdu,
    SnmpMessage *snmp_msg,
    TransportInfo *only_ti,
    SR_INT32 cfg_chk,
    const OctetString *agent_addr,
    SR_INT32 retryCount,
    SR_INT32 timeout)
{
    snmpNotifyEntry_t *sne;
    snmpTargetAddrEntry_t *stae;
    snmpTargetParamsEntry_t *stpe;
    int idx1, idx2, idx3;
    TransportInfo ti;
    int pdu_version;

    if ((snmp_msg != NULL) && (only_ti != NULL) && (cfg_chk == 0)) {
#if (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU))
        /* Perform pdu conversion if necessary */

        if (snmp_msg->version == SR_SNMPv1_VERSION) { 
            pdu_version = SR_SNMPv1_PDU_PAYLOAD;
        } else if (snmp_msg->version == SR_SNMPv2_VERSION) {
            pdu_version = SR_SNMPv2_PDU_PAYLOAD;
        } else if (snmp_msg->version == SR_SNMPv3_VERSION) { 
            pdu_version = SR_SNMPv2_PDU_PAYLOAD;
        }

        switch (only_ti->type) {
	default:
            if (translate_pdu(snmp_msg->version, &v1_pdu, &other_pdu, 
                     agent_addr, SR_IP_TRANSPORT, pdu_version)) {
                goto done;
            }
            break;
        }
#endif	/* (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU)) */
        if (notify_serial_num == 0x7fffffff) {
            notify_serial_num = 1;
        } else {
            notify_serial_num++;
        }

        switch (snmp_msg->version) {
#ifdef SR_SNMPv1_PACKET
            case SR_SNMPv1_VERSION:
                if (notifyType != SR_NTFY_TYPE_INFORM) {
                    Sr_send_trap_ctx(snmp_msg, v1_pdu, only_ti, contextName);
                }
                break;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2_PDU
#ifdef SR_SNMPv2c_PACKET
            case SR_SNMPv2c_VERSION:
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
            case SR_SNMPv3_VERSION:
#endif /* SR_SNMPv3_PACKET */
                other_pdu->u.normpdu.request_id = notify_serial_num;
                if (notifyType == SR_NTFY_TYPE_INFORM) {
#ifdef SR_NOTIFY_FULL_COMPLIANCE
                    other_pdu->type = INFORM_REQUEST_TYPE;
                    Sr_init_inform(notify_serial_num,
                                   timeout,
                                   retryCount,
                                   snmp_msg,
                                   other_pdu,
                                   only_ti);
#endif /* SR_NOTIFY_FULL_COMPLIANCE */
                } else {
                    other_pdu->type = SNMPv2_TRAP_TYPE;
                    Sr_send_trap_ctx(snmp_msg, other_pdu, only_ti, contextName);
                }
                break;
#endif /* SR_SNMPv2_PDU */
        }
        goto done;
    }


    for (idx1 = 0; idx1 < agt_snmpNotifyTable.nitems; idx1++) {
        sne = (snmpNotifyEntry_t *)agt_snmpNotifyTable.tp[idx1];
        if (sne->snmpNotifyRowStatus != RS_ACTIVE) {
            continue;
        }
        if (notifyType != SR_NTFY_TYPE_EITHER) {
            if (notifyType != sne->snmpNotifyType) {
                continue;
            }
        }
        for (idx2 = 0; idx2 < agt_snmpTargetAddrTable.nitems; idx2++) {
            stae = (snmpTargetAddrEntry_t *)agt_snmpTargetAddrTable.tp[idx2];
            if (stae->snmpTargetAddrRowStatus != RS_ACTIVE) {
                continue;
            }

            /* Check if the tag selects this entry */
            if (!SrTagInTagList(sne->snmpNotifyTag,
                                stae->snmpTargetAddrTagList)) {
                continue;
            }

            /* Convert transport info into TransportInfo structure */
            if (OIDAndOctetStringToTransportInfo(&ti,
                   stae->snmpTargetAddrTDomain,
                   stae->snmpTargetAddrTAddress)) {
                continue;
            }

            /* Check if it matches only_ti if necessary */
            if (only_ti != NULL) {
                if (CmpTransportInfo(&ti, only_ti)) {
                    continue;
                }
            }

            /* Find corresponding snmpTargetParamsEntry */
            agt_snmpTargetParamsTable.tip[0].value.octet_val =
                stae->snmpTargetAddrParams;
            idx3 = SearchTable(&agt_snmpTargetParamsTable, EXACT);
            if (idx3 == -1) {
                continue;
            }
            stpe =
                (snmpTargetParamsEntry_t *)agt_snmpTargetParamsTable.tp[idx3];
            if (stpe->snmpTargetParamsRowStatus != RS_ACTIVE) {
                continue;
            }

            /* Check against snmp_msg here */
            if (snmp_msg != NULL) {
                if (snmp_msg->version != stpe->snmpTargetParamsMPModel) {
                    continue;
                }
                switch (snmp_msg->version) {
#ifdef SR_SNMPv1_PACKET
                    case SR_SNMPv1_VERSION:
                        if (stpe->snmpTargetParamsSecurityModel !=
                                SR_SECURITY_MODEL_V1) {
                            continue;
                        }
                        if (stpe->snmpTargetParamsSecurityLevel !=
                                SR_SECURITY_LEVEL_NOAUTH) {
                            continue;
                        }
                        if (snmp_msg->u.v1.community != NULL) {
                            if (CmpOctetStrings(snmp_msg->u.v1.community,
                                    stpe->snmpTargetParamsSecurityName)) {
                                continue;
                            }
                        }
                        break;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
                    case SR_SNMPv2c_VERSION:
                        if (stpe->snmpTargetParamsSecurityModel !=
                                SR_SECURITY_MODEL_V2C) {
                            continue;
                        }
                        if (stpe->snmpTargetParamsSecurityLevel !=
                                SR_SECURITY_LEVEL_NOAUTH) {
                            continue;
                        }
                        if (snmp_msg->u.v1.community != NULL) {
                            if (CmpOctetStrings(snmp_msg->u.v1.community,
                                    stpe->snmpTargetParamsSecurityName)) {
                                continue;
                            }
                        }
                        break;
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
                    case SR_SNMPv3_VERSION:
                        if (snmp_msg->u.v3.securityModel != 0) {
                            if (snmp_msg->u.v3.securityModel !=
                                    stpe->snmpTargetParamsSecurityModel) {
                                continue;
                            }
                        }
                        if (snmp_msg->u.v3.securityLevel != 0) {
                            if (snmp_msg->u.v3.securityLevel !=
                                    stpe->snmpTargetParamsSecurityLevel) {
                                continue;
                            }
                        }
                        if (snmp_msg->u.v3.userName != NULL) {
                            if (!CmpOctetStrings(
                                    snmp_msg->u.v3.userName,
                                    stpe->snmpTargetParamsSecurityName)) {
                                continue;
                            }
                        }
                        break;
#endif /* SR_SNMPv3_PACKET */
                }
            }

#if (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU))
            /* Perform pdu conversion if necessary */
            if (stpe->snmpTargetParamsMPModel == SR_SNMPv1_VERSION) {
                pdu_version = SR_SNMPv1_PDU_PAYLOAD;
            } else if (stpe->snmpTargetParamsMPModel == SR_SNMPv2c_VERSION) {
                pdu_version = SR_SNMPv2_PDU_PAYLOAD;
            } else {
                    pdu_version = SR_SNMPv2_PDU_PAYLOAD;
            }


            if (translate_pdu(stpe->snmpTargetParamsMPModel, &v1_pdu, &other_pdu,
                              agent_addr, ti.type, pdu_version)) {
                continue;
            }
#endif	/* (defined(SR_SNMPv1_PACKET) && defined(SR_SNMPv2_PDU)) */

            /* Call SrGenerateNotification */
            SrGenerateNotification(contextName,
                                   v1_pdu,
                                   other_pdu,
                                   sne,
                                   stae,
                                   stpe,
                                   &ti);
        }
    }

  done:
    if (v1_pdu) {
        FreePdu(v1_pdu);
    }
    if (other_pdu) {
        FreePdu(other_pdu);
    }
}
