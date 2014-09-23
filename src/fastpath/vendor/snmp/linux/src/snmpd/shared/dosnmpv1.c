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
#include "trap.h"
#include "diag.h"
SR_FILENAME


/*------------------------------------------------------------
 * Process a trivial (rfc1157) snmp packet.
 *------------------------------------------------------------*/
Pdu            *
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
    SR_INT32        req;
    ContextInfo    *contextInfo = NULL;
    AdminInfo       admin_info;
    SR_UINT32       limit;
    int             payload = 0;

    switch (snmp_msg->error_code) {
        case ASN_PARSE_ERROR:
        case RESOURCE_UNAVAILABLE_ERROR:
            snmpData.snmpInASNParseErrs++;
            goto cleanup;
        case SNMP_BAD_VERSION_ERROR:
            snmpData.snmpInBadVersions++;
            goto cleanup;
    }

#ifdef SR_SNMPv1_PACKET
    if (snmp_msg->version == SR_SNMPv1_VERSION) {
        payload = SR_SNMPv1_PDU_PAYLOAD;
    }
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
    if (snmp_msg->version == SR_SNMPv2c_VERSION) {
        payload = SR_SNMPv2_PDU_PAYLOAD;
    }
#endif /* SR_SNMPv2c_PACKET */



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
            goto cleanup;
    }
    if (contextInfo == NULL) {
        goto cleanup;
    }

    in_pdu_ptr = SrParsePdu(snmp_msg->packlet->octet_ptr,
                            snmp_msg->packlet->length);
    if (in_pdu_ptr == NULL) {
        snmpData.snmpInASNParseErrs++;
        DPRINTF((APPACKET, "%s: Error parsing pdu packlet\n", Fname));
        goto cleanup;
    }

    req = in_pdu_ptr->u.normpdu.request_id;

    SrAssignRequestIDToContextInfo(contextInfo, req);

    switch (in_pdu_ptr->type) {
        case GET_RESPONSE_TYPE:
	    DPRINTF((APPACKET, "%s: received response pdu\n", Fname));
            snmpData.snmpInGetResponses++;
            break;
        case GET_REQUEST_TYPE:
	    DPRINTF((APPACKET, "%s: received get pdu\n", Fname));
            snmpData.snmpInGetRequests++;
            break;
        case GET_NEXT_REQUEST_TYPE:
	    DPRINTF((APPACKET, "%s: received get-next pdu\n", Fname));
            snmpData.snmpInGetNexts++;
            break;
        case SET_REQUEST_TYPE:
	    DPRINTF((APPACKET, "%s: received set pdu\n", Fname));
            snmpData.snmpInSetRequests++;
            break;
        case TRAP_TYPE:
	    DPRINTF((APPACKET, "%s: received trap pdu\n", Fname));
            snmpData.snmpInTraps++;
            break;
#ifdef SR_SNMPv2_PDU
        case GET_BULK_REQUEST_TYPE:
        case SNMPv2_TRAP_TYPE:
        case INFORM_REQUEST_TYPE:
            break;
#endif /* SR_SNMPv2_PDU */
        default:
            snmpData.snmpInASNParseErrs++;
	    DPRINTF((APPACKET, "%s: unknown pdu type\n", Fname));
            goto cleanup;
    }

    switch (in_pdu_ptr->type) {

#ifdef SR_SNMPv2_PDU
        case GET_BULK_REQUEST_TYPE:
#ifdef SR_SNMPv1_PACKET
            if (payload == SR_SNMPv1_PDU_PAYLOAD) {
                DPRINTF((APALWAYS, "%s: get_bulk in v1 packet ?\n", Fname));
                goto cleanup;
            }
#endif /* SR_SNMPv1_PACKET */
#endif /* SR_SNMPv2_PDU */
        case GET_REQUEST_TYPE:
        case GET_NEXT_REQUEST_TYPE:

            out_pdu_ptr = do_response(in_pdu_ptr, contextInfo, limit,
                                      NULL, payload);
            if (out_pdu_ptr == NULL) {
                DPRINTF((APTRACE, "%s: Request failed\n", Fname));
                goto cleanup;
            }

        break;

    case SET_REQUEST_TYPE:

        if (admin_info.privs != SR_READ_WRITE) {
            snmpData.snmpInBadCommunityUses++;
            out_pdu_ptr = make_error_pdu(GET_RESPONSE_TYPE, req,
                                         NO_SUCH_NAME_ERROR, 0,
                                         in_pdu_ptr, payload);
        } else {
            out_pdu_ptr = do_sets(in_pdu_ptr, contextInfo, NULL, payload);
        }
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

    default:
        DPRINTF((APPACKET, "%s: Internal error. (Invalid packet type)\n", Fname));
        goto cleanup;
    }                                /* switch */


cleanup:
    if (in_pdu_ptr != NULL) {
        FreePdu(in_pdu_ptr);
        in_pdu_ptr = NULL;
    }
    if (contextInfo != NULL) {
        FreeContextInfo(contextInfo);
    }
    return (out_pdu_ptr);
}
