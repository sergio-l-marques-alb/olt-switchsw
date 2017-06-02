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
#include "v1_msg.h"

#include "diag.h"
SR_FILENAME

void
SrParseV1SnmpMessage(
    SnmpMessage *snmp_msg,
    SnmpLcd *sl_ptr,
    void *v_sl_ptr,
    TransportInfo *src_ti,
    TransportInfo *dst_ti,
    const unsigned char *msg,
    const int msg_len)
{
    FNAME("SrParseV1SnmpMessage")
    short type;
    SR_INT32 pdulen;

    snmp_msg->u.v1.community =
        ParseOctetString(&snmp_msg->wptr, snmp_msg->end_ptr, &type);
    if (snmp_msg->u.v1.community == NULL) {
        DPRINTF((APPACKET, "%s:  community\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        return;
    }

    /* Determine maximum size of a response pdu that can be sent back */
    snmp_msg->wrapper_size = snmp_msg->wptr - msg;
    snmp_msg->max_vbl_size = sr_pkt_size -
                             snmp_msg->wrapper_size -
                             SR_PDU_WRAPPER_SIZE;

    /* Put pdu portion of packet into snmp_msg->packlet */
    pdulen = (SR_INT32)(snmp_msg->end_ptr - snmp_msg->wptr);
    snmp_msg->packlet = MakeOctetString2(snmp_msg->wptr, pdulen);
    if (snmp_msg->packlet == NULL) {
      DPRINTF((APPACKET, "%s: Failed to create snmp_msg PDU packlet\n", Fname));
      snmp_msg->error_code = RESOURCE_UNAVAILABLE_ERROR;
    }
}

int
SrBuildV1SnmpMessage(
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr,
    void *v_sl_ptr,
    SnmpLcd *sl_ptr)
{
    FNAME("SrBuildV1SnmpMessage")

    SR_INT32        lenlen;
    SR_INT32        datalen;
    unsigned char  *wptr;

    datalen = FindLenUInt(snmp_msg->version) +
              FindLenOctetString(snmp_msg->u.v1.community) +
              pdu_ptr->packlet->length;
    lenlen = DoLenLen(datalen);

    snmp_msg->packlet = MakeOctetString(NULL, 1 + lenlen + datalen);
    if (snmp_msg->packlet == NULL) {
        DPRINTF((APWARN, "%s: malloc\n", Fname));
        goto fail;
    }

    wptr = snmp_msg->packlet->octet_ptr;

    *wptr++ = SEQUENCE_TYPE;
    AddLen(&wptr, lenlen, datalen);

    (void) AddUInt(&wptr, INTEGER_TYPE, snmp_msg->version);
    (void) AddOctetString(&wptr, OCTET_PRIM_TYPE, snmp_msg->u.v1.community);

    memcpy(wptr, pdu_ptr->packlet->octet_ptr, (int)pdu_ptr->packlet->length);
    wptr += pdu_ptr->packlet->length;
    return 0;

  fail:
    FreeOctetString(snmp_msg->packlet);
    snmp_msg->packlet = NULL;
    return -1;
}
