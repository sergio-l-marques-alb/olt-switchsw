/*
 *
 * Copyright (C) 2005-2006 by SNMP Research, Incorporated.
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

/*
 *  This file contains functions related to SNMP Research's TCP transport.
 *  The functions in this file do such things are converting/formatting 
 *  of TransportInfos, and, as such, do not really program to the platform's
 *  network stack.
 */


#include "sr_conf.h"


/* --- Needed for snprintf() prototype --- */
#include <stdio.h>

#include <stdlib.h>

/* --- Needed for memcpy() prototype --- */
#include <string.h>

/* --- Prerequisite for ntohl() prototype --- */
#include <sys/types.h>




/* --- Prerequisite for ntohl() prototype --- */
#include <netinet/in.h>


/* --- Prerequisite for inet_addr() prototype --- */
#include <sys/socket.h>

#include <arpa/inet.h>


/* --- Neeed for isdigit() macro --- */
#include <ctype.h>


#include "sr_type.h"
#include "sr_time.h"



#include "sr_snmp.h"
#include "compat.h"
#include "diag.h"
#include "sr_trans.h"
SR_FILENAME


/*
 *  This is assigned as srExperimentalDomains.2.  RFC3417 defines 
 *  snmpUDPDomain; no snmpTCPDomain was found.
 */
static const SR_UINT32 snmpTCPDomain_sids[] = {1, 3, 6, 1, 4, 1, 99, 15, 2, 2};
static const OID snmpTCPDomain_OID =
    { OIDSIZE(snmpTCPDomain_sids), (SR_UINT32 *) snmpTCPDomain_sids };
const OID      *snmpTCPDomainOID = &snmpTCPDomain_OID;



void
CopyTcpTransportInfo(TransportInfo *dst, const TransportInfo *src)
{
    dst->type = src->type;
    dst->flags = src->flags;
    dst->fd = src->fd;
    switch (src->type) {
        default:
            dst->t_ipAddr = src->t_ipAddr;
            dst->t_ipPort = src->t_ipPort;
            break;
    }
}   /* CopyTcpTransportInfo() */


void
CopyTcpTransportAddr(TransportInfo *dst, const TransportInfo *src)
{
    switch (src->type) {
        default:
            dst->t_ipAddr = src->t_ipAddr;
            dst->t_ipPort = src->t_ipPort;
            break;
    }
}   /* CopyTcpTransportAddr() */


int
CmpTcpTransportInfo(const TransportInfo *ti1, const TransportInfo *ti2)
{
    if ((ti1 == NULL) || (ti2 == NULL)) {
	return -1;
    }
    if (ti1->type != ti2->type) {
	return -1;
    }
    switch (ti1->type) {
        default:
            if (ti1->t_ipAddr != ti2->t_ipAddr) {
                return -1;
            }
            if (ti1->t_ipPort != ti2->t_ipPort) {
                return -1;
            }
            break;
    }
    return 0;
}   /* CmpTcpTransportInfo() */


int
CmpTcpTransportInfoWithMask(
    const TransportInfo *ti1,
    const TransportInfo *ti2,
    const TransportInfo *mask)
{

    if ((ti1 == NULL) || (ti2 == NULL) || (mask == NULL)) {
	return -1;
    }
    if (ti1->type != ti2->type) {
	return -1;
    }

    switch (ti1->type) {
	default:
            if ((ti1->t_ipAddr & mask->t_ipAddr) != 
                       (ti2->t_ipAddr & mask->t_ipAddr)) {
                return -1;
            }
            if ((ti1->t_ipPort & mask->t_ipPort) !=
                   (ti2->t_ipPort & mask->t_ipPort)) {
                 return -1;
            }
            break;
    }

    return 0;
}   /* CmpTcpTransportInfoWithMask() */




OctetString *
TcpTransportInfoToOctetString(const TransportInfo *ti)
{
    OctetString *os = NULL;
 
    if (ti == NULL) {
        return NULL;
    }
    switch (ti->type) {
        default:
            os = MakeOctetString(NULL, 6);
            if (os == NULL) {
                return NULL;
            }
            memcpy(os->octet_ptr, &ti->t_ipAddr, 4);
            memcpy(os->octet_ptr + 4, &ti->t_ipPort, 2);
            break;
    }
    return os;
}   /* TcpTransportInfoToOctetString() */

 
int
OctetStringToTcpTransportInfo(TransportInfo *ti, const OctetString *os)
{
    if (os == NULL) {
        return -1;
    }
    if (os->length != 6) {
        return -1;
    }
    ti->type = SR_TCP_TRANSPORT;
    memcpy(&ti->t_ipAddr, os->octet_ptr, 4);
    memcpy(&ti->t_ipPort, os->octet_ptr + 4, 2);
    return 0;
}   /* OctetStringToTcpTransportInfo() */


int
TMaskOctetStringToTcpTransportInfo(TransportInfo *ti, const OctetString *os)
{
    if (os == NULL) {
        return -1;
    }
    if (os->length == 0) {
        ti->type = SR_TCP_TRANSPORT;
        ti->t_ipAddr = inet_addr("255.255.255.255");
        ti->t_ipPort = 0;
        return 0;
    }
    if (os->length != 6) {
        return -1;
    }
    ti->type = SR_TCP_TRANSPORT;
    memcpy(&ti->t_ipAddr, os->octet_ptr, 4);
    memcpy(&ti->t_ipPort, os->octet_ptr + 4, 2);
    return 0;
}   /* TMaskOctetStringToTcpTransportInfo() */


char *
FormatTcpTransportString(char *buf, int len, const TransportInfo *ti)
{
    SR_UINT32 addr = ntohl(ti->t_ipAddr);
    unsigned long port = (unsigned long)ntohs(ti->t_ipPort);

    if (len < 22) {
        strcpy(buf, "");
    } else {
        snprintf(buf, len, "%lu.%lu.%lu.%lu:%lu",
                (unsigned long)((addr & 0xff000000) >> 24),
                (unsigned long)((addr & 0xff0000) >> 16),
                (unsigned long)((addr & 0xff00) >> 8),
                (unsigned long)(addr & 0xff),
                port);
    }
    return buf;
}   /* FormatTcpTransportString() */


