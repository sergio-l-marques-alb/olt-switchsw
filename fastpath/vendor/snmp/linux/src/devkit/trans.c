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

#include <errno.h>

#include <string.h>

#include <stdlib.h>


#include "sr_snmp.h"
#include "sr_trans.h"
#include "diag.h"
SR_FILENAME

#include "oid_lib.h"

#include "sr_ip.h"
#include "sr_tcp.h"
#include "sr_tinfo.h"



static const SR_UINT32 rfc1157Domain_sids[] = {1, 3, 6, 1, 6, 2, 1, 1};
static const OID rfc1157Domain_OID =
    { OIDSIZE(rfc1157Domain_sids), (SR_UINT32 *) rfc1157Domain_sids };
const OID      *rfc1157DomainOID = &rfc1157Domain_OID;

static const SR_UINT32 snmpUDPDomain_sids[] = {1, 3, 6, 1, 6, 1, 1};
static const OID snmpUDPDomain_OID =
    { OIDSIZE(snmpUDPDomain_sids), (SR_UINT32 *) snmpUDPDomain_sids };
const OID      *snmpUDPDomainOID = &snmpUDPDomain_OID;


/* LVL7 */
/* per rfc 3419 */
static const SR_UINT32 transportDomainUdpIpv6_sids[] = {1, 3, 6, 1 ,2,1,100, 1, 2};
static const OID transportDomainUdpIpv6_OID =
    { OIDSIZE(transportDomainUdpIpv6_sids), (SR_UINT32 *) transportDomainUdpIpv6_sids };
const OID      *transportDomainUdpIpv6OID = &transportDomainUdpIpv6_OID;
/* LVL7 */


void
CopyTransportInfo(dst, src)
    TransportInfo *dst;
    const TransportInfo *src;
{
    switch (src->type) {
        case SR_IP_TRANSPORT:
            CopyIpTransportInfo(dst, src);
            break;

        case SR_TCP_TRANSPORT:
            CopyTcpTransportInfo(dst, src);
            break;

/* LVL7 */
        case SR_IP6_TRANSPORT:
            CopyIp6TransportInfo(dst, src);
            break;
/* LVL7 */
        default:
            DPRINTF((APERROR, "attempt to copy unknown transport type info\n"));
            break;
    }
}

void
CopyTransportAddr(dst, src)
    TransportInfo *dst;
    const TransportInfo *src;
{
    switch (src->type) {
        case SR_IP_TRANSPORT:
            CopyIpTransportAddr(dst, src);
            break;

        case SR_TCP_TRANSPORT:
            CopyTcpTransportAddr(dst, src);
            break;

/* LVL7 */
        case SR_IP6_TRANSPORT:
            CopyIp6TransportAddr(dst, src);
            break;
/* LVL7 */
        default:
            DPRINTF((APERROR, "attempt to copy unknown transport type addr\n"));
            break;
    }
}

int
CmpTransportInfo(ti1, ti2)
    const TransportInfo *ti1;
    const TransportInfo *ti2;
{
    if (ti1->type == ti2->type) {
        switch (ti1->type) {
            case SR_IP_TRANSPORT:
                return CmpIpTransportInfo(ti1, ti2);

            case SR_TCP_TRANSPORT:
                return CmpTcpTransportInfo(ti1, ti2);

/* LVL7 */
            case SR_IP6_TRANSPORT:
                return CmpIp6TransportInfo(ti1, ti2);
/* LVL7 */
            default:
                DPRINTF((APERROR, "attempt to compare unknown transport type\n"));
                break;
        }
    }

    return -1;
}

int
CmpTransportInfoWithMask(ti1, ti2, mask)
    const TransportInfo *ti1;
    const TransportInfo *ti2;
    const TransportInfo *mask;
{
    if (ti1->type == ti2->type) {
	if (ti1->type != mask->type) {
	    return CmpTransportInfo(ti1, ti2);
	}
	switch (ti1->type) {
	    case SR_IP_TRANSPORT:
	        return CmpIpTransportInfoWithMask(ti1, ti2, mask);

            case SR_TCP_TRANSPORT:
                return CmpTcpTransportInfoWithMask(ti1, ti2, mask);

/* LVL7 */
	case SR_IP6_TRANSPORT:
	    return CmpIp6TransportInfoWithMask(ti1, ti2, mask);
/* LVL7 */
	    default:
	        DPRINTF((APERROR, 
                    "attempt to compare unknown transport type\n"));
	        break;
	}
    }
    
    return -1;
}

int
TransportInfoToOIDAndOctetString(ti, domain, address)
    const TransportInfo *ti;
    OID **domain;
    OctetString **address;
{
    switch (ti->type) {
        case SR_IP_TRANSPORT:
            *domain = CloneOID(snmpUDPDomainOID);
            *address = IpTransportInfoToOctetString(ti);
            break;

        case SR_TCP_TRANSPORT:
            *domain = CloneOID(snmpTCPDomainOID);
            *address = TcpTransportInfoToOctetString(ti);
            break;

/* LVL7 */
        case SR_IP6_TRANSPORT:
            *domain = CloneOID(transportDomainUdpIpv6OID);
            *address = Ip6TransportInfoToOctetString(ti);
            break;
/* LVL7 */
        default:
            DPRINTF((APERROR, "attempt to convert unknown transport type from oid to octetstring\n"));
            return -1;
    }
    return 0;
}

int
OIDAndOctetStringToTransportInfo(ti, domain, address)
    TransportInfo *ti;
    OID *domain;
    const OctetString *address;
{
    if (address == NULL) {
        return -1;
    }
    if (!CmpOID(domain, snmpUDPDomainOID)) {
         return OctetStringToIpTransportInfo(ti, address);
    }

    if (!CmpOID(domain, snmpTCPDomainOID)) {
         return OctetStringToIpTransportInfo(ti, address);
    }

    if (!CmpOID(domain, transportDomainUdpIpv6OID)) {
        return OctetStringToIp6TransportInfo(ti, address);
    }


    return -1;
}

int
SendToTransport(data, len, sti, dti)
    const char *data;
    int len;
    const TransportInfo *sti;
    const TransportInfo *dti;
{
    switch (sti->type) {
        case SR_IP_TRANSPORT:
            return SendToIpTransport(data, len, sti, dti);

        case SR_TCP_TRANSPORT:
            return -1;

/* LVL7 */
        case SR_IP6_TRANSPORT:
            return SendToIp6Transport(data, len, sti, dti);
/* LVL7 */
        default:
            DPRINTF((APERROR, "attempt to send to unknown transport type\n"));
    }
    return -1;
}

int
TMaskOIDAndOctetStringToTransportInfo(ti, domain, address)
    TransportInfo *ti;
    OID *domain;
    const OctetString *address;
{
    if (address == NULL) {
        return -1;
    }
    if (!CmpOID(domain, snmpUDPDomainOID)) {
         return TMaskOctetStringToIpTransportInfo(ti, address);
    }

    if (!CmpOID(domain, snmpTCPDomainOID)) {
         return TMaskOctetStringToIpTransportInfo(ti, address);
    }

/* LVL7 */
    if (!CmpOID(domain, transportDomainUdpIpv6OID)) {
        return TMaskOctetStringToIp6TransportInfo(ti, address);
    }
/* LVL7 */

    return -1;
}

int
ReceiveFromTransport(data, len, sti, dti)
    char *data;
    int len;
    TransportInfo *sti;
    TransportInfo *dti;
{
    switch (sti->type) {
        case SR_IP_TRANSPORT:
            return ReceiveFromIpTransport(data, len, sti, dti);

        case SR_TCP_TRANSPORT:
            return -1;

/* LVL7 */
        case SR_IP6_TRANSPORT:
            return ReceiveFromIp6Transport(data, len, sti, dti);
/* LVL7 */
        default:
            DPRINTF((APERROR, "attempt to receive from unknown transport type\n"));
    }
    return -1;
}

char *
FormatTransportString(buf, len, ti)
    char *buf;
    int len;
    const TransportInfo *ti;
{
    switch (ti->type) {
        case SR_IP_TRANSPORT:
            return FormatIpTransportString(buf, len, ti);

        case SR_TCP_TRANSPORT:
            return FormatIpTransportString(buf, len, ti);

/* LVL7 */
        case SR_IP6_TRANSPORT:
            return FormatIp6TransportString(buf, len, ti);
/* LVL7 */
        default:
            DPRINTF((APERROR, "attempt to format unknown transport type\n"));
    }
    strcpy(buf, "");
    return buf;
}

int
OpenTransport(ti)
    TransportInfo *ti;
{
    switch (ti->type) {
        case SR_IP_TRANSPORT:
            return OpenIpTransport(ti);

        case SR_TCP_TRANSPORT:
            return -1;

/* LVL7 */
        case SR_IP6_TRANSPORT:
            return OpenIp6Transport(ti);
/* LVL7 */
        default:
            DPRINTF((APERROR, "attempt to open unknown transport type\n"));
    }
    return -1;
}

int
BindTransport(ti, tries)
    TransportInfo *ti;
    int tries;
{
    switch (ti->type) {
        case SR_IP_TRANSPORT:
            return BindIpTransport(ti, tries);

        case SR_TCP_TRANSPORT:
            return -1;

/* LVL7 */
        case SR_IP6_TRANSPORT:
            return BindIp6Transport(ti, tries);
/* LVL7 */
        default:
            DPRINTF((APERROR, "attempt to bind unknown transport type\n"));
    }
    return -1;
}

void
CloseTransport(ti)
    TransportInfo *ti;
{
    switch (ti->type) {
        case SR_IP_TRANSPORT:
            CloseIpTransport(ti);
            break;

        case SR_TCP_TRANSPORT:
            break;

/* LVL7 */
        case SR_IP6_TRANSPORT:
            CloseIp6Transport(ti);
            break;
/* LVL7 */
        default:
            DPRINTF((APERROR, "attempt to close unknown transport type\n"));
            break;
    }
}

int
BindTransportGivenPortRange(ti)
    TransportInfo *ti;
{
    switch (ti->type) {
        case SR_IP_TRANSPORT:
            return BindIpTransportGivenPortRange(ti);

        case SR_TCP_TRANSPORT:
            return -1;

        default:
            DPRINTF((APERROR, "attempt to bind unknown transport type\n"));
    }
    return -1;
}

int 
GetTransportInfo(int bind_type, void **tInfo,
		 char *ipv4_bind_addr, char *ipv6_bind_addr)
{
    switch (bind_type) {
        case BIND_DEFAULT:
        case BIND_IP_ALL:
            return GetIpTransportInfo(bind_type, tInfo,
                                      ipv4_bind_addr, ipv6_bind_addr);
        default:
           DPRINTF((APERROR, "unknown bind_type\n"));
    }
    return -1;
}

/*
 * FreeTransinfoList
 *
 * this routine frees the linked list in the incoming TRANS_INFO structure.
 * return value
 *    void
 */

void FreeTransinfoList(TRANS_INFO *ti_list)
{
    TRANS_INFO *tmp_list = NULL;

    while (ti_list != NULL) {
        tmp_list = ti_list;
        ti_list = tmp_list->next;
        free(tmp_list);
    }
    return;
}

/*
 * CloneTransinfoList
 *
 * this routine clones incoming the linked list saved as TRANS_INFO.
 *
 * return value
 *    success: pointer of cloned TRANS_INFO structure.
 *    fail   : NULL
 */
TRANS_INFO *
CloneTransinfoList(TRANS_INFO *ti_list)
{
    TRANS_INFO *tmp_list = NULL;
    TRANS_INFO *clone_list = NULL;
    TRANS_INFO *tmp_clone = NULL;
    TRANS_INFO *current_clone = NULL;

    tmp_list = ti_list;
    while (tmp_list != NULL) {
        tmp_clone = (TRANS_INFO *)malloc(sizeof(TRANS_INFO));
        if (tmp_clone == NULL) {
            goto fail;
        }
        memcpy(tmp_clone, tmp_list, sizeof(TRANS_INFO));
        tmp_clone->next = NULL;
        if (clone_list == NULL) {
            clone_list = tmp_clone;
            current_clone = tmp_clone;
        }
        else {
            current_clone->next = tmp_clone;
            current_clone = tmp_clone;
        }
        tmp_list = tmp_list->next;
    }

    return clone_list;

  fail:
    FreeTransinfoList(clone_list);
    return NULL;
}
