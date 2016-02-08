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

#include <string.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "sr_msg.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#include "snmptype.h"
#include "lookup.h"
#include "v2table.h"
#include "diag.h"
SR_FILENAME

extern snmp_t  snmpData;
static SR_UINT32 _snmp_sids[] = { 1, 3, 6, 1, 2, 1, 11, 0, 0 };
static OID snmp_OID = { 9, _snmp_sids };

#ifdef SR_SNMPv3_PACKET

#include "v3_msg.h"

extern snmpTargetObjects_t agt_snmpTargetObjectsData;

static SR_UINT32 _snmpMPDStats_sids[] = { 1, 3, 6, 1, 6, 3, 11, 2, 1, 0, 0 };
static SR_UINT32 _usmStats_sids[] = { 1, 3, 6, 1, 6, 3, 15, 1, 1, 0, 0 };
static SR_UINT32 _sto_sids[] = { 1, 3, 6, 1, 6, 3, 12, 1, 0, 0 };
static OID snmpMPDStats_OID = { 11, _snmpMPDStats_sids };
static OID usmStats_OID = { 11, _usmStats_sids };
static OID sto_OID = { 10, _sto_sids };

Pdu *
SrDoSnmpV3Report(
    SnmpLcd *snmp_lcd,
    SnmpMessage *snmp_msg)
{
    SR_INT32 req_id = 0x7fffffff;
    OID *oid;
    SR_UINT32 value;
    VarBind *vb;
    Pdu *pdu;
    const unsigned char *wptr, *end_ptr;
    short type;
    SnmpV3Lcd *snmp_v3_lcd =
        (SnmpV3Lcd *)SrGetVersionLcd(snmp_lcd, SR_SNMPv3_VERSION);

    oid = NULL;
    vb = NULL;
    pdu = NULL;

    if (snmp_msg == NULL) {
        goto cleanup;
    }

    snmp_msg->u.v3.securityLevel = SR_SECURITY_LEVEL_NOAUTH;
    switch (snmp_msg->error_code) {
        case ASN_PARSE_ERROR:
            snmp_OID.oid_ptr[7] = 6;
            oid = &snmp_OID;
            value = snmpData.snmpInASNParseErrs;
            break;
        case SR_SNMP_UNKNOWN_SECURITY_MODELS:
            goto cleanup;
        case SR_SNMP_INVALID_MSGS:
            snmpMPDStats_OID.oid_ptr[9] = 2;
            oid = &snmpMPDStats_OID;
            value = snmp_v3_lcd->snmpMPDStatsData->snmpInvalidMsgs;
            break;
        case SR_SNMP_UNKNOWN_PDU_HANDLERS:
            snmpMPDStats_OID.oid_ptr[9] = 3;
            oid = &snmpMPDStats_OID;
            value = snmp_v3_lcd->snmpMPDStatsData->snmpUnknownPDUHandlers;
            break;
        case SR_USM_UNSUPPORTED_SEC_LEVELS:
            usmStats_OID.oid_ptr[9] = 1;
            oid = &usmStats_OID;
            value = snmp_v3_lcd->usmStatsData->usmStatsUnsupportedSecLevels;
            break;
#ifndef SR_UNSECURABLE
        case SR_USM_NOT_IN_TIME_WINDOWS:
            usmStats_OID.oid_ptr[9] = 2;
            oid = &usmStats_OID;
            value = snmp_v3_lcd->usmStatsData->usmStatsNotInTimeWindows;
            snmp_msg->u.v3.securityLevel = SR_SECURITY_LEVEL_AUTH;
            break;
#endif /* SR_UNSECURABLE */
        case SR_USM_UNKNOWN_USER_NAMES:
            usmStats_OID.oid_ptr[9] = 3;
            oid = &usmStats_OID;
            value = snmp_v3_lcd->usmStatsData->usmStatsUnknownUserNames;
            break;
        case SR_USM_UNKNOWN_ENGINE_IDS:
            usmStats_OID.oid_ptr[9] = 4;
            oid = &usmStats_OID;
            value = snmp_v3_lcd->usmStatsData->usmStatsUnknownEngineIDs;
            break;
#ifndef SR_UNSECURABLE
        case SR_USM_WRONG_DIGESTS:
            usmStats_OID.oid_ptr[9] = 5;
            oid = &usmStats_OID;
            value = snmp_v3_lcd->usmStatsData->usmStatsWrongDigests;
            break;
#ifndef SR_NO_PRIVACY
        case SR_USM_DECRYPTION_ERRORS:
            usmStats_OID.oid_ptr[9] = 6;
            oid = &usmStats_OID;
            value = snmp_v3_lcd->usmStatsData->usmStatsDecryptionErrors;
            break;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
        case UNKNOWN_CONTEXT_ERROR:
            sto_OID.oid_ptr[8] = 5;
            oid = &sto_OID;
            value = agt_snmpTargetObjectsData.snmpUnknownContexts;
            break;
        default:
            goto cleanup;
    }

    if (snmp_msg->u.v3.contextSnmpEngineID != NULL) {
        FreeOctetString(snmp_msg->u.v3.contextSnmpEngineID);
    }
    if (snmp_msg->u.v3.contextName != NULL) {
        FreeOctetString(snmp_msg->u.v3.contextName);
    }
    snmp_msg->u.v3.contextSnmpEngineID =
        CloneOctetString(snmp_v3_lcd->snmpEngineData->snmpEngineID);
    if (snmp_msg->u.v3.contextSnmpEngineID == NULL) {
        DPRINTF((APPACKET, "cannot make contextSnmpEngineID\n"));
        goto cleanup;
    }
    snmp_msg->u.v3.contextName = MakeOctetString(NULL, 0);
    if (snmp_msg->u.v3.contextName == NULL) {
        DPRINTF((APPACKET, "cannot make contextName\n"));
        goto cleanup;
    }

    vb = MakeVarBindWithValue(oid, NULL, COUNTER_TYPE, (void *)&value);
    NULL_CHECK(vb, (APPACKET, "cannot make varbind for report PDU"), cleanup);

    if (snmp_msg->packlet) {
        wptr = snmp_msg->packlet->octet_ptr;
        end_ptr = wptr + snmp_msg->packlet->length;
        wptr++;    /* pduType */
        if (ParseLength(&wptr, end_ptr) != -1) {
            req_id = ParseInt(&wptr, end_ptr, &type);
            if (type == -1) {
                req_id = 0x7fffffff;
                DPRINTF((APWARN, "Could not parse reqid in encrypted message.\nYou may have just got an error in ParseType, but this is normal behavior.\n"));
            }
        }
    }
    pdu = MakePdu(REPORT_TYPE,
                  req_id,
                  (SR_INT32)0,
                  (SR_INT32)0,
                  (OID *)NULL,
                  (OctetString *)NULL,
                  (SR_INT32)0,
                  (SR_INT32)0,
                  (SR_INT32)0);
    NULL_CHECK(pdu, (APPACKET, "cannot make report PDU"), cleanup);

    LinkVarBind(pdu, vb);
    vb = NULL;
    return pdu;

  cleanup:
    if (vb) {
        FreeVarBind(vb);
    }
    if (pdu) {
        FreePdu(pdu);
    }
    return NULL;
}

#endif /* SR_SNMPv3_PACKET */
