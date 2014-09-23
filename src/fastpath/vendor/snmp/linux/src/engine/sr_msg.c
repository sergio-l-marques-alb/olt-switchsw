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

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include "sr_snmp.h"
#include "sr_trans.h"
#include "sr_msg.h"

#include "diag.h"
SR_FILENAME

/*
 * SrParseSnmpMessage
 *
 * This function parses an SNMP message, and dispatches the results to
 * an appropriate message handler.  The result is a return code that
 * indicates whether the message was successfully dispatched.
 */
SnmpMessage *
SrParseSnmpMessage(
    SnmpLcd *sl_ptr,
    TransportInfo *src_ti,
    TransportInfo *dst_ti,
    const unsigned char *msg,
    const int msg_len)
{
    FNAME("SrParseSnmpMessage")
    SnmpMessage *snmp_msg = NULL;
    short type;
    SR_INT32 seq_length;
    SR_INT32 i;

    snmp_msg = (SnmpMessage *)malloc(sizeof(SnmpMessage));
    if (snmp_msg == NULL) {
        DPRINTF((APWARN, "%s:  snmp_msg malloc\n", Fname));
        return NULL;
    }
    SR_CLEAR(snmp_msg);
    snmp_msg->version = -1;
    snmp_msg->wptr = msg;
    snmp_msg->end_ptr = msg + msg_len;

    /* Extract sequence tag and length */
    seq_length = ParseSequence(&snmp_msg->wptr, snmp_msg->end_ptr, &type);
    if (seq_length == -1) {
        DPRINTF((APPACKET, "%s: ParseSequence:\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        return snmp_msg;
    }

    /* Re-calculate end_ptr, based on ASN.1 structure's internal length. If
     * they do not match up, use the minimum. */
    snmp_msg->new_end_ptr = snmp_msg->wptr + seq_length;
    if (snmp_msg->new_end_ptr < snmp_msg->end_ptr) {
        snmp_msg->end_ptr = snmp_msg->new_end_ptr;
    } else if (snmp_msg->new_end_ptr > snmp_msg->end_ptr) {
        DPRINTF((APPACKET, "%s: bad BER length:\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        return snmp_msg;
    }

    /* Parse SNMP version */
    snmp_msg->version = ParseInt(&snmp_msg->wptr, snmp_msg->end_ptr, &type);
    if (type == -1) {
        DPRINTF((APPACKET, "%s: version:\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        return snmp_msg;
    }
    for (i = 0; i < sl_ptr->num_vers; i++) {
        if (snmp_msg->version == sl_ptr->versions_supported[i]) {
            (*sl_ptr->parse_functions[i])(snmp_msg,
                                          sl_ptr,
                                          sl_ptr->lcds[i],
                                          src_ti,
                                          dst_ti,
                                          msg,
                                          msg_len);
            return snmp_msg;
        }
    }
    snmp_msg->error_code = SNMP_BAD_VERSION_ERROR;
    return snmp_msg;
}

/*
 * SrBuildSnmpMessage
 *
 * This function builds an SNMP message.
 */
int
SrBuildSnmpMessage(
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr,
    SnmpLcd *sl_ptr)
{
    FNAME("SrBuildSnmpMessage")
    SR_INT32 i;

    FreeOctetString(snmp_msg->packlet);
    snmp_msg->packlet = NULL;

    /* Call appropriate build function */
    for (i = 0; i < sl_ptr->num_vers; i++) {
        if (snmp_msg->version == sl_ptr->versions_supported[i]) {
            return (*sl_ptr->build_functions[i])(snmp_msg,
                                                 pdu_ptr,
                                                 sl_ptr->lcds[i],
                                                 sl_ptr);
        }
    }

    DPRINTF((APWARN, "%s, bad SNMP version: %d\n", Fname, snmp_msg->version));
    return -1;
}

void
SrFreeSnmpMessage(
    SnmpMessage *snmp_msg)
{
    if (snmp_msg == NULL) {
        return;
    }

    FreeOctetString(snmp_msg->packlet);
    snmp_msg->packlet = NULL;

    switch (snmp_msg->version) {
        case SR_SNMPv1_VERSION:
        case SR_SNMPv2c_VERSION:
            FreeOctetString(snmp_msg->u.v1.community);
            break;
        case SR_SNMPv2_VERSION:
#ifndef SR_UNSECURABLE
            FreeOctetString(snmp_msg->u.v2.authKey);
#ifndef SR_NO_PRIVACY
            FreeOctetString(snmp_msg->u.v2.privKey);
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
            FreeOctetString(snmp_msg->u.v2.contextSnmpID);
            FreeOctetString(snmp_msg->u.v2.contextName);
            FreeOctetString(snmp_msg->u.v2.authSnmpID);
            FreeOctetString(snmp_msg->u.v2.identityName);
            FreeOctetString(snmp_msg->u.v2.groupName);
            FreeOctetString(snmp_msg->u.v2.transportLabel);
            break;
        case SR_SNMPv3_VERSION:
#ifndef SR_UNSECURABLE
            FreeOctetString(snmp_msg->u.v3.authKey);
#ifndef SR_NO_PRIVACY
            FreeOctetString(snmp_msg->u.v3.privKey);
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
            FreeOctetString(snmp_msg->u.v3.authSnmpEngineID);
            FreeOctetString(snmp_msg->u.v3.userName);
            FreeOctetString(snmp_msg->u.v3.contextSnmpEngineID);
            FreeOctetString(snmp_msg->u.v3.contextName);
            FreeOctetString(snmp_msg->u.v3.userSecurityName);
            break;
    }

    free(snmp_msg);
}

void *
SrGetVersionLcd(
    SnmpLcd *sl_ptr,
    SR_INT32 version)
{
    SR_INT32 i;

    for (i = 0; i < sl_ptr->num_vers; i++) {
        if (version == sl_ptr->versions_supported[i]) {
            return sl_ptr->lcds[i];
        }
    }
    return NULL;
}
