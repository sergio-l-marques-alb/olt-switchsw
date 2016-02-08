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

#include <errno.h>

#include <sys/types.h>





#include <string.h>

#include <malloc.h>


#include <netinet/in.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "cacheid.h"

#include "sr_ip.h"    /* for GetLocalIPAddress() */

#if defined(SR_SNMPv3_PACKET)
typedef struct _SNMPID_MAPPING {
    TransportInfo ti;
    unsigned char snmpid[32];
    SR_INT32 snmpid_len;
    int authenticated;
    struct _SNMPID_MAPPING *next;
} SNMPID_MAPPING;
SNMPID_MAPPING *snmpid_mappings = NULL;

OctetString *
LookupSnmpID(ti)
    TransportInfo *ti;
{
    SNMPID_MAPPING *sm;

    for (sm = snmpid_mappings; sm != NULL; sm = sm->next) {
        if (!CmpTransportInfo(&sm->ti, ti)) {
            return MakeOctetString(sm->snmpid, sm->snmpid_len);
        }
    }
    return NULL;
}

void
InsertSnmpID(snmpID, ti)
    OctetString *snmpID;
    TransportInfo *ti;
{
    SNMPID_MAPPING *sm;

    for (sm = snmpid_mappings; sm != NULL; sm = sm->next) {
        if (!CmpTransportInfo(&sm->ti, ti)) {
            if (sm->authenticated) {
                return;
            }
            memcpy(sm->snmpid, snmpID->octet_ptr, (size_t)snmpID->length);
            sm->snmpid_len = snmpID->length;
            return;
        }
    }

    sm = (SNMPID_MAPPING *)malloc(sizeof(SNMPID_MAPPING));
    if (sm != NULL) {
        memcpy(sm->snmpid, snmpID->octet_ptr, (size_t)snmpID->length);
        sm->snmpid_len = snmpID->length;
        CopyTransportInfo(&sm->ti, ti);
        sm->authenticated = 0;
        sm->next = snmpid_mappings;
        snmpid_mappings = sm;
    }
}

void
InsertAuthenticatedSnmpID(snmpID, ti)
    OctetString *snmpID;
    TransportInfo *ti;
{
    SNMPID_MAPPING *sm;

    for (sm = snmpid_mappings; sm != NULL; sm = sm->next) {
        if (!CmpTransportInfo(&sm->ti, ti)) {
            memcpy(sm->snmpid, snmpID->octet_ptr, (size_t)snmpID->length);
            sm->snmpid_len = snmpID->length;
            sm->authenticated = 1;
            return;
        }
    }

    sm = (SNMPID_MAPPING *)malloc(sizeof(SNMPID_MAPPING));
    if (sm != NULL) {
        memcpy(sm->snmpid, snmpID->octet_ptr, (size_t)snmpID->length);
        sm->snmpid_len = snmpID->length;
        CopyTransportInfo(&sm->ti, ti);
        sm->authenticated = 1;
        sm->next = snmpid_mappings;
        snmpid_mappings = sm;
    }
}
#endif	/* defined(SR_SNMPv3_PACKET) */
