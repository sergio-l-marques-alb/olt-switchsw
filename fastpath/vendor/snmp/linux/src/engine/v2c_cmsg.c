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

SnmpMessage *
SrCreateV2cSnmpMessage(
    OctetString *community,
    int cache)
{
    FNAME("SrCreateV2cSnmpMessage")
    SnmpMessage *snmp_msg;

    snmp_msg = (SnmpMessage *)malloc(sizeof(SnmpMessage));
    if (snmp_msg == NULL) {
        DPRINTF((APWARN, "%s:  snmp_msg malloc\n", Fname));
        goto fail;
    }
    SR_CLEAR(snmp_msg);

    if (cache) {
        snmp_msg->u.v1.community = CloneOctetString(community);
        if (snmp_msg->u.v1.community == NULL) {
            DPRINTF((APWARN, "%s:  snmp_msg->community malloc\n", Fname));
            goto fail;
        }
    } else {
        snmp_msg->u.v1.community = community;
    }
    snmp_msg->cached = cache;
    snmp_msg->version = SR_SNMPv2c_VERSION;
    return snmp_msg;

  fail:
    if (snmp_msg != NULL) {
        free(snmp_msg);
    }
    return NULL;
}
