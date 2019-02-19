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

#include <string.h>



#include "sr_snmp.h"
#include "sr_trans.h"
#include "sr_msg.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#include "snmptype.h"
#include "snmpdefs.h"
#include "agtctx.h"
#include "snmpd.h"
#include "snmpv2d.h"
#include "trap.h"
#include "report.h"
#include "lookup.h"
#include "v2table.h"
#include "v3_msg.h"
#include "diag.h"
SR_FILENAME



extern snmpTargetObjects_t agt_snmpTargetObjectsData;

Pdu *
SrDoSnmp(
    SnmpLcd *snmp_lcd,
    TransportInfo  *srcTI,
    TransportInfo  *dstTI,
    SnmpMessage    *snmp_msg,
    unsigned int    outLen)
{
    FNAME("SrDoSnmp")
    Pdu            *in_pdu_ptr = NULL;
    Pdu            *out_pdu_ptr = NULL;
    ContextInfo    *contextInfo = NULL;
    AdminInfo       admin_info;
    SR_INT32        req;
    SR_UINT32       limit = 0;
    vacmAccessEntry_t *vae;
    int             payload = 0;
    SnmpV3Lcd      *snmp_v3_lcd =
        (SnmpV3Lcd *)SrGetVersionLcd(snmp_lcd, SR_SNMPv3_VERSION);

    switch (snmp_msg->error_code) {
        case ASN_PARSE_ERROR:
        case RESOURCE_UNAVAILABLE_ERROR:
            snmpData.snmpInASNParseErrs++;
            goto cleanup;
        case SNMP_BAD_VERSION_ERROR:
            snmpData.snmpInBadVersions++;
            goto cleanup;
#ifdef SR_SNMPv3_PACKET
        case SR_SNMP_UNKNOWN_SECURITY_MODELS:
            snmp_v3_lcd->snmpMPDStatsData->snmpUnknownSecurityModels++;
            DPRINTF((APPACKET, "%s: Unknown security model\n", Fname));
            goto cleanup;
        case SR_SNMP_INVALID_MSGS:
            snmp_v3_lcd->snmpMPDStatsData->snmpInvalidMsgs++;
            DPRINTF((APPACKET, "%s: Invalid message\n", Fname));
            goto cleanup;
        case SR_SNMP_UNKNOWN_PDU_HANDLERS:
            snmp_v3_lcd->snmpMPDStatsData->snmpUnknownPDUHandlers++;
            DPRINTF((APPACKET, "%s: Unknown PDU handler\n", Fname));
            goto cleanup;
        case SR_USM_UNSUPPORTED_SEC_LEVELS:
            snmp_v3_lcd->usmStatsData->usmStatsUnsupportedSecLevels++;
            DPRINTF((APPACKET, "%s: Unsupported Security Level\n", Fname));
            goto cleanup;
        case SR_USM_NOT_IN_TIME_WINDOWS:
            snmp_v3_lcd->usmStatsData->usmStatsNotInTimeWindows++;
            DPRINTF((APPACKET, "%s: Not in time window error\n", Fname));
            goto cleanup;
        case SR_USM_UNKNOWN_USER_NAMES:
            snmp_v3_lcd->usmStatsData->usmStatsUnknownUserNames++;
            DPRINTF((APPACKET, "%s: Unknown user\n", Fname));
            goto cleanup;
        case SR_USM_UNKNOWN_ENGINE_IDS:
            snmp_v3_lcd->usmStatsData->usmStatsUnknownEngineIDs++;
            DPRINTF((APPACKET, "%s: Unknown EngineIDs\n", Fname));
            goto cleanup;
        case SR_USM_WRONG_DIGESTS:
            snmp_v3_lcd->usmStatsData->usmStatsWrongDigests++;
            DPRINTF((APPACKET, "%s: usmStatsWrongDigests error\n", Fname));
            goto cleanup;
        case SR_USM_DECRYPTION_ERRORS:
            snmp_v3_lcd->usmStatsData->usmStatsDecryptionErrors++;
            DPRINTF((APPACKET, "%s: Decryption error\n", Fname));
            goto cleanup;
#endif /* SR_SNMPv3_PACKET */
    }

    /* Determine payload version */
    payload = SR_SNMPv2_PDU_PAYLOAD;
#ifdef SR_SNMPv1_PACKET
    if (snmp_msg->version == SR_SNMPv1_VERSION) {
        payload = SR_SNMPv1_PDU_PAYLOAD;
    }
#endif /* SR_SNMPv1_PACKET */

    contextInfo = findContextInfo(snmp_lcd, srcTI, dstTI, snmp_msg,
                                  &admin_info, &limit, outLen);
    switch (snmp_msg->error_code) {
        case SR_SNMP_BAD_COMMUNITY_NAMES:
            snmpData.snmpInBadCommunityNames++;
            if (snmpData.snmpEnableAuthenTraps ==
                D_snmpEnableAuthenTraps_enabled) {
                do_trap(AUTHEN_FAILURE_TRAP, ((SR_INT32)0), (VarBind *) NULL,
                        systemData.sysObjectID, (char *) NULL);
            }
            goto cleanup;
        case UNKNOWN_CONTEXT_ERROR:
#ifdef SR_SNMPv3_PACKET
            if (snmp_msg->version == SR_SNMPv3_VERSION) {
                agt_snmpTargetObjectsData.snmpUnknownContexts++;
                DPRINTF((APPACKET, "%s: Unknown Context\n", Fname));
            }
#endif /* SR_SNMPv3_PACKET */
            goto cleanup;
#ifdef SR_SNMPv3_PACKET
        case SR_SNMP_UNKNOWN_PDU_HANDLERS:
            snmp_v3_lcd->snmpMPDStatsData->snmpUnknownPDUHandlers++;
            goto cleanup;
#endif /* SR_SNMPv3_PACKET */
        case SR_NO_GROUP_ERROR:
#ifdef SR_SNMPv1_PACKET
            if ( snmp_msg->version == SR_SNMPv1_VERSION ) {
                 snmpData.snmpInBadCommunityUses++;
            }
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
            if ( snmp_msg->version == SR_SNMPv2c_VERSION ) {
                 snmpData.snmpInBadCommunityUses++;
            }
#endif /* SR_SNMPv2c_PACKET */

            DPRINTF((APPACKET, "%s: No group error \n", Fname));
       /* cleanup is not called as error handled after call to
          LookupVacmAccessEntryWithMask */
            goto skip_null_check; /* skip over the contextInfo NULL check */
    }
    if (contextInfo == NULL) {
        goto cleanup;
    }

skip_null_check:
    in_pdu_ptr = SrParsePdu(snmp_msg->packlet->octet_ptr,
                            snmp_msg->packlet->length);
    if (in_pdu_ptr == NULL) {
        snmpData.snmpInASNParseErrs++;
        DPRINTF((APPACKET, "%s: Error parsing pdu packlet\n", Fname));
        goto cleanup;
    }


    if (in_pdu_ptr->u.normpdu.error_status == 99)  {     /* type error */
         DPRINTF((APPACKET, "%s: Error parsing pdu packlet\n", Fname));
         if ( snmp_msg->version == SR_SNMPv1_VERSION)  {               /* version 1, wants wrong value */
         out_pdu_ptr = make_error_pdu(GET_RESPONSE_TYPE, in_pdu_ptr->u.normpdu.request_id,
                                      WRONG_VALUE_ERROR, in_pdu_ptr->u.normpdu.error_index,
                                      in_pdu_ptr, SR_SNMPv1_PDU_PAYLOAD);
         }
         else   {                                             /* version 2 or 3 , wants wrong type  */
         out_pdu_ptr = make_error_pdu(GET_RESPONSE_TYPE, in_pdu_ptr->u.normpdu.request_id,
                                      WRONG_TYPE_ERROR, in_pdu_ptr->u.normpdu.error_index,
                                      in_pdu_ptr, SR_SNMPv2_PDU_PAYLOAD);
         }
         goto cleanup;
    }

    req = in_pdu_ptr->u.normpdu.request_id;

    SrAssignRequestIDToContextInfo(contextInfo, req);

    /* append request_id to contextInfo */

    switch (in_pdu_ptr->type) {
        case GET_RESPONSE_TYPE:
            DPRINTF((APPACKET, "%s: received response pdu\n", Fname));
            snmpData.snmpInGetResponses++;
#ifdef SR_SNMPv3_PACKET
            snmp_v3_lcd->snmpMPDStatsData->snmpUnknownPDUHandlers++;
#endif /* SR_SNMPv3_PACKET */
            break;
        case GET_REQUEST_TYPE:
            DPRINTF((APPACKET, "%s: received get pdu\n", Fname));
            snmpData.snmpInGetRequests++;
            break;
        case GET_NEXT_REQUEST_TYPE:
            DPRINTF((APPACKET, "%s: received get-next pdu\n", Fname));
            snmpData.snmpInGetNexts++;
            break;
        case GET_BULK_REQUEST_TYPE:
#ifdef SR_SNMPv1_PACKET
            if (payload == SR_SNMPv1_PDU_PAYLOAD) {
                DPRINTF((APALWAYS, "%s: SNMPv2 request in v1 packet ?\n",
                         Fname));
                goto cleanup;
            }
#endif /* SR_SNMPv1_PACKET */
            DPRINTF((APPACKET, "%s: received get-bulk pdu\n", Fname));
            break;
        case SET_REQUEST_TYPE:
            DPRINTF((APPACKET, "%s: received set pdu\n", Fname));
            snmpData.snmpInSetRequests++;
            break;
#ifdef SR_SNMPv1_PACKET
        case TRAP_TYPE:
            if (payload == SR_SNMPv2_PDU_PAYLOAD || 
                payload == SR_AGGREGATE_PDU_PAYLOAD) {

                DPRINTF((APPACKET, "%s: SNMPv1 pdu in non-v1 packet ?\n",
                         Fname));
                goto cleanup;
            }
            DPRINTF((APPACKET, "%s: received v1 trap pdu\n", Fname));
            snmpData.snmpInTraps++;
            break;
#endif /* SR_SNMPv1_PACKET */
        case SNMPv2_TRAP_TYPE:
        case INFORM_REQUEST_TYPE:
#ifdef SR_SNMPv1_PACKET
            if (payload == SR_SNMPv1_PDU_PAYLOAD) {
                DPRINTF((APALWAYS, "%s: SNMPv2 request in v1 packet ?\n",
                         Fname));
                goto cleanup;
            }
#endif /* SR_SNMPv1_PACKET */
            DPRINTF((APPACKET, "%s: received v2 trap or inform pdu\n", Fname));
            break;
        default:
            snmp_msg->error_code = SR_SNMP_UNKNOWN_PDU_HANDLERS;
            snmp_v3_lcd->snmpMPDStatsData->snmpUnknownPDUHandlers++;
            DPRINTF((APPACKET, 
                     "%s: Unknown PDU handler for unexpected pdu type %d\n",
                     Fname, in_pdu_ptr->type));
            goto cleanup;
    }

    switch (in_pdu_ptr->type) {
        case GET_RESPONSE_TYPE:
        case GET_REQUEST_TYPE:
        case GET_NEXT_REQUEST_TYPE:
        case GET_BULK_REQUEST_TYPE:
        case SET_REQUEST_TYPE:
        case INFORM_REQUEST_TYPE:
            if (snmp_msg->version == SR_SNMPv3_VERSION) {
                if (snmp_msg->u.v3.reportableFlag == 0) {
                    if (CmpOctetStrings(snmp_msg->u.v3.authSnmpEngineID,
                            snmp_v3_lcd->snmpEngineData->snmpEngineID)) {
                        snmp_msg->error_code = SR_USM_UNKNOWN_ENGINE_IDS;
                        snmp_v3_lcd->usmStatsData->usmStatsUnknownEngineIDs++;
                        FreeOctetString(snmp_msg->u.v3.authSnmpEngineID);
                        snmp_msg->u.v3.authSnmpEngineID = CloneOctetString(
                            snmp_v3_lcd->snmpEngineData->snmpEngineID);
                        goto cleanup;
                    }
                }
            }
    }


    /* Step (8) d) */
    vae = LookupVacmAccessEntryWithMask(admin_info.groupName,
                                        admin_info.contextName,
                                        admin_info.securityModel,
                                        admin_info.securityLevel);
    if (vae == NULL) {
        switch (snmp_msg->version) {
#ifdef SR_SNMPv1_WRAPPER
#ifdef SR_SNMPv1_PACKET
            case SR_SNMPv1_VERSION:
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
            case SR_SNMPv2c_VERSION:
#endif /* SR_SNMPv2c_PACKET */
                snmpData.snmpInBadCommunityUses++;
                if (snmpData.snmpEnableAuthenTraps ==
                        D_snmpEnableAuthenTraps_enabled) {
                    do_trap(AUTHEN_FAILURE_TRAP,
                            ((SR_INT32)0), (VarBind *) NULL,
                            (OID *)NULL, (char *) NULL);
                }
                goto cleanup;
#endif /* SR_SNMPv1_WRAPPER */
#ifdef SR_SNMPv3_PACKET
            case SR_SNMPv3_VERSION:
        out_pdu_ptr = make_error_pdu(GET_RESPONSE_TYPE, req,
                                     AUTHORIZATION_ERROR, 0,
                                     in_pdu_ptr, SR_SNMPv2_PDU_PAYLOAD);
        goto cleanup;
#endif /* SR_SNMPv3_PACKET */
        }
    } 

    switch (in_pdu_ptr->type) {

        case GET_REQUEST_TYPE:
        case GET_NEXT_REQUEST_TYPE:
        case GET_BULK_REQUEST_TYPE:
            out_pdu_ptr = do_response(in_pdu_ptr, contextInfo, limit,
                                      vae->vacmAccessReadViewName, payload);
            if (out_pdu_ptr == NULL) {
                DPRINTF((APTRACE, "%s: Request failed\n", Fname));
            }
            break;

#ifdef SETS
    case SET_REQUEST_TYPE:
#ifdef SR_SNMPv3_PACKET
            if (snmp_msg->version == SR_SNMPv3_VERSION) {
                if (limit <= 0) {
                    snmpData.snmpSilentDrops++;
                    goto cleanup;        /* silently drop it */
                }
            }
#endif /* SR_SNMPv3_PACKET */
            out_pdu_ptr = do_sets(in_pdu_ptr, contextInfo,
                                  vae->vacmAccessWriteViewName, payload);

#ifdef SR_SNMPv2c_PACKET

        switch (snmp_msg->version) {
            case SR_SNMPv2c_VERSION:
              if( out_pdu_ptr != NULL &&
                  out_pdu_ptr->u.normpdu.error_status == NO_ACCESS_ERROR ){
                  snmpData.snmpInBadCommunityUses++;
              }
           }
#endif  /* SR_SNMPv2c_PACKET */

            if (out_pdu_ptr == NULL) {
#ifdef SR_DEBUG
                /* Log information about this successful SET request. */
                PrintSetAuditMessage(snmp_msg, in_pdu_ptr, 1, srcTI);
#endif /* SR_DEBUG */
                out_pdu_ptr = in_pdu_ptr;
                    out_pdu_ptr->type = GET_RESPONSE_TYPE;
                out_pdu_ptr->u.normpdu.error_status = 0;
                out_pdu_ptr->u.normpdu.error_index = 0;
                NormalizeVarBindListLengths(out_pdu_ptr->var_bind_list);
                in_pdu_ptr = NULL;
            }
#ifdef SR_DEBUG
            else {
                PrintSetAuditMessage(snmp_msg, in_pdu_ptr, 0, srcTI);
            }
#endif /* SR_DEBUG */
            break;

#endif /* SETS */

        default:
            DPRINTF((APPACKET,
                     "%s: Internal error. (Invalid packet type)\n", Fname));
            goto cleanup;

    }

  cleanup:
    if (in_pdu_ptr != NULL) {
        FreePdu(in_pdu_ptr);
    }
    if (contextInfo != NULL) {
        FreeContextInfo(contextInfo);
    }
/* do not generate report if out_pdu_ptr not NULL, or if error is
    ASN_PARSE_ERROR or SR_SNMP_INVALID_MSGS          */
    if (out_pdu_ptr == NULL && (snmp_msg->error_code != ASN_PARSE_ERROR &&
                                snmp_msg->error_code != SR_SNMP_INVALID_MSGS)) {
        switch (snmp_msg->version) {
#ifdef SR_SNMPv3_PACKET
            case SR_SNMPv3_VERSION:
                out_pdu_ptr = SrDoSnmpV3Report(snmp_lcd, snmp_msg);
                break;
#endif /* SR_SNMPv3_PACKET */
        }
    }
    return (out_pdu_ptr);
}
