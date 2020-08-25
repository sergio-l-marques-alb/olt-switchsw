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

#define        WANT_ALL_ERRORS

#include "sr_conf.h"

#include <stdio.h>

#include <string.h>


#include <sys/types.h>




#include "sr_snmp.h"
#include "sr_trans.h"
#include "sr_msg.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#include "snmptype.h"
#include "snmpd.h"
#include "mgmt.h"
#include "dosnmp.h"
#include "sitedefs.h"
#include "trap.h"

#include "diag.h"
#include "pclo.h"
SR_FILENAME

#include <malloc.h>


SR_UINT32 sourceIPAddr;
unsigned short int sourceUDPPort;

extern SnmpLcd agt_snmp_lcd;

/*------------------------------------------------------------
 * Process one packet.
 *------------------------------------------------------------*/
unsigned short
do_mgmt(
    unsigned char  *inPkt,
    unsigned int    inLen,
    TransportInfo  *srcTI,
    TransportInfo  *dstTI,
    unsigned char  *outPkt,
    unsigned int    outLen)
{
    return SrDoMgmt(&agt_snmp_lcd, inPkt, inLen, srcTI, dstTI, outPkt, outLen);
}

unsigned short
SrDoMgmt(
    SnmpLcd *snmp_lcd,
    unsigned char  *inPkt,
    unsigned int    inLen,
    TransportInfo  *srcTI,
    TransportInfo  *dstTI,
    unsigned char  *outPkt,
    unsigned int    outLen)
{
    FNAME("SrDoMgmt")
    SnmpMessage    *snmp_msg;
    Pdu            *result_pdu;
    unsigned short  result_len = 0;

    sourceIPAddr = srcTI->t_ipAddr;
    sourceUDPPort = srcTI->t_ipPort;

    snmp_msg = NULL;
    result_pdu = NULL;
    snmpData.snmpInPkts++;

    snmp_msg = SrParseSnmpMessage(snmp_lcd, srcTI, dstTI, inPkt, (int)inLen);
    if (snmp_msg == NULL) {
        return 0;
    }

#ifdef SR_DEBUG
    packdump("Incoming Packet", 
	     inPkt,
	     inLen,
             srcTI,
             snmp_msg,
             0);
#endif /* SR_DEBUG */

    result_pdu = SrDoSnmp(snmp_lcd, srcTI, dstTI, snmp_msg, outLen);
    if (result_pdu == NULL) {
        goto cleanup;
    }

    if (BuildPdu(result_pdu) == -1) {
        DPRINTF((APPACKET, "%s: Failed to build PDU.\n", Fname));
        goto cleanup;
    }

    if (SrBuildSnmpMessage(snmp_msg, result_pdu, snmp_lcd) == -1) {
        DPRINTF((APALWAYS, "%s: Failed to build SnmpMessage.\n", Fname));
        goto cleanup;
    }

    memcpy((char *) outPkt,
           (char *) snmp_msg->packlet->octet_ptr,
           (int) snmp_msg->packlet->length);
    result_len = (int)snmp_msg->packlet->length;

#ifdef SR_SNMPv1_PACKET
    if (snmp_msg->version == SR_SNMPv1_VERSION) {
        snmpData.snmpOutGetResponses++;
    }
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
    if (snmp_msg->version == SR_SNMPv2c_VERSION) {
        snmpData.snmpOutGetResponses++;
    }
#endif /* SR_SNMPv2c_PACKET */

#ifdef SR_DEBUG
    packdump("Outgoing Packet", 
    	     outPkt,
	     result_len,
             srcTI,
             result_pdu->var_bind_list,
             0);
#endif /* SR_DEBUG */

cleanup:
    if (snmp_msg != NULL) {
        SrFreeSnmpMessage(snmp_msg);
        snmp_msg = NULL;
    }
    if (result_pdu != NULL) {
        FreePdu(result_pdu);
        result_pdu = NULL;
    }

    return (result_len);        /* signal results to layer above */
}                                /* do_mgmt() */

/*------------------------------------------------------------
 * Copy the input pdu to an error pdu format.
 *------------------------------------------------------------*/
Pdu            *
make_error_pdu(type, req_id, status, counter, in_pdu_ptr, payload)
    int             type;
    SR_INT32        req_id;
    int             status;
    int             counter;
    Pdu            *in_pdu_ptr;
    int             payload;
{
    Pdu            *out_pdu_ptr;

#ifdef        SR_SNMPv2_PDU
    /* 
     * first, do any remappings necessary
     * and ensure any counters are zero as appropriate.
     * note that there is no situation specified where an error index
     * of 0 may be returned on an error in SNMPv1 except for on too 
     * big errors * (RFC1157, 4.1.2, 4.1,3)
     */
    switch (status) {
    case TOO_BIG_ERROR:
        counter = 0;
        break;
    case GEN_ERROR:
        /* do nothing */
        break;
    case AUTHORIZATION_ERROR:
        counter = 0;
        /* no break intended here -- fall into next case */
    case NOT_WRITABLE_ERROR:
    case NO_ACCESS_ERROR:
    case NO_CREATION_ERROR:
    case NO_SUCH_NAME_ERROR:
    case INCONSISTENT_NAME_ERROR:
#ifdef SR_SNMPv1_PACKET
        if (payload == SR_SNMPv1_PDU_PAYLOAD) {
            status = NO_SUCH_NAME_ERROR;
            if (counter == 0) {
                counter = 1;
            }
        }
#endif /* SR_SNMPv1_PACKET */
        break;
    case WRONG_TYPE_ERROR:
    case WRONG_LENGTH_ERROR:
    case WRONG_ENCODING_ERROR:
    case WRONG_VALUE_ERROR:
    case INCONSISTENT_VALUE_ERROR:
    case BAD_VALUE_ERROR:
#ifdef SR_SNMPv1_PACKET
        if (payload == SR_SNMPv1_PDU_PAYLOAD) {
            status = BAD_VALUE_ERROR;
            if (counter == 0) {
                counter = 1;
            }
        }
#endif /* SR_SNMPv1_PACKET */
        break;
    case RESOURCE_UNAVAILABLE_ERROR:
    case COMMIT_FAILED_ERROR:
    case UNDO_FAILED_ERROR:
#ifdef SR_SNMPv1_PACKET
        if (payload == SR_SNMPv1_PDU_PAYLOAD) {
            status = GEN_ERROR;
            if (counter == 0) {
                counter = 1;
            }
        }
#endif /* SR_SNMPv1_PACKET */
        break;
    default:
        DPRINTF((APWARN, "Internal error: unknown remapping case (%d) in make_error_pdu\n", status));
        break;
    }                                /* switch */
#endif                                /* SR_SNMPv2_PDU */

    /* now make the pdu */
    out_pdu_ptr = MakePdu(type,
                          req_id,
                          (SR_INT32) status,
                          (SR_INT32) counter,
                          (OID *) NULL,
                          (OctetString *) NULL,
                          ((SR_INT32)0),
                          ((SR_INT32)0),
                          ((SR_INT32)0));
    if (out_pdu_ptr == (Pdu *) NULL) {
        DPRINTF((APWARN, "make_error_pdu: Malloc of out_pdu_ptr failed.\n"));
        return ((Pdu *) NULL);
    }

    /* now do the right thing with the varbind lists */

#ifndef        SR_SNMPv2_PDU
    out_pdu_ptr->var_bind_list = in_pdu_ptr->var_bind_list;
    out_pdu_ptr->var_bind_end_ptr = in_pdu_ptr->var_bind_end_ptr;
    in_pdu_ptr->var_bind_list = NULL;
    in_pdu_ptr->var_bind_end_ptr = NULL;
    NormalizeVarBindListLengths(out_pdu_ptr->var_bind_list);
#else                                /* SR_SNMPv2_PDU */
    if ((payload == SR_SNMPv2_PDU_PAYLOAD) && (status == TOO_BIG_ERROR)) {
        /* empty varbindlist on reply */
        /* nothing to be done */
    } else {
        /* echo the input list to the output list */
        out_pdu_ptr->var_bind_list = in_pdu_ptr->var_bind_list;
        out_pdu_ptr->var_bind_end_ptr = in_pdu_ptr->var_bind_end_ptr;
        in_pdu_ptr->var_bind_list = NULL;
        in_pdu_ptr->var_bind_end_ptr = NULL;
        NormalizeVarBindListLengths(out_pdu_ptr->var_bind_list);
    }
#endif                                /* SR_SNMPv2_PDU */



    /* now increment the counters (only get here if successful in making pdu */
    switch (status) {
    case TOO_BIG_ERROR:
        snmpData.snmpOutTooBigs++;
        break;

#ifdef        SR_SNMPv1_PACKET
    case NO_SUCH_NAME_ERROR:
        snmpData.snmpOutNoSuchNames++;
        break;
    case BAD_VALUE_ERROR:
        snmpData.snmpOutBadValues++;
        break;
#endif                                /* SR_SNMPv1_PACKET */

    case GEN_ERROR:
        snmpData.snmpOutGenErrs++;
        break;

#ifdef SR_SNMPv2_PDU
    case NO_ACCESS_ERROR:
    case WRONG_TYPE_ERROR:
    case WRONG_LENGTH_ERROR:
    case WRONG_ENCODING_ERROR:
    case WRONG_VALUE_ERROR:
    case NO_CREATION_ERROR:
    case INCONSISTENT_VALUE_ERROR:
    case RESOURCE_UNAVAILABLE_ERROR:
    case COMMIT_FAILED_ERROR:
    case UNDO_FAILED_ERROR:
    case AUTHORIZATION_ERROR:
    case NOT_WRITABLE_ERROR:
    case INCONSISTENT_NAME_ERROR:
        break;
#endif                                /* SR_SNMPv2_PDU */


    default:
        DPRINTF((APWARN, "Internal error: unknown counter case (%d) in make_error_pdu\n", status));
        break;
    }                                /* switch (status) */

    return (out_pdu_ptr);
}                                /* make_error_pdu() */
