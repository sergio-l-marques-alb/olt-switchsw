
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

#include <unistd.h>

#include <malloc.h>

#include <errno.h>

#include <sys/types.h>




#include <netinet/in.h>

#include <string.h>

#include <sys/socket.h>


#include <netdb.h>

#include <arpa/inet.h>


#include "sr_type.h"
#include "sr_time.h"










#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME

#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "sr_trans.h"




#include "sr_ip.h"



/* LVL7 */
#include <l7_common.h>
#include "dtlapi.h"

#include <dtlapi.h>
#include "osapi_support.h"
/* LVL7 end */


#define SA_CAST(x) (struct sockaddr *)x



void
CopyIp6TransportInfo(dst, src)
    TransportInfo *dst;
    const TransportInfo *src;
{
    dst->type = src->type;
    dst->flags = src->flags;
    dst->fd = src->fd;
    memcpy(dst->t_ip6Addr , src->t_ip6Addr, 16);
    dst->t_ip6Port = src->t_ip6Port;
}

void
CopyIp6TransportAddr(dst, src)
    TransportInfo *dst;
    const TransportInfo *src;
{
    memcpy(dst->t_ip6Addr , src->t_ip6Addr, 16);
    dst->t_ip6Port = src->t_ip6Port;
}

int
CmpIp6TransportInfo(ti1, ti2)
    const TransportInfo *ti1;
    const TransportInfo *ti2;
{
    int i;
 
    for(i = 0; i < 16;i++){
      if (ti1->t_ip6Addr[i] != ti2->t_ip6Addr[i]) {
        return -1;
      }
    }
    if (ti1->t_ip6Port != ti2->t_ip6Port) {
        return -1;
    }
    return 0;
}

int
CmpIp6TransportInfoWithMask(ti1, ti2, mask)
    const TransportInfo *ti1;
    const TransportInfo *ti2;
    const TransportInfo *mask;
{
     int i;
 
     for(i = 0; i < 16;i++){
       if ((ti1->t_ip6Addr[i] & mask->t_ip6Addr[i]) != 
	 (ti2->t_ip6Addr[i] & mask->t_ip6Addr[i])) {
         return -1;
       }
     }
     if ((ti1->t_ip6Port & mask->t_ip6Port) !=
	 (ti2->t_ip6Port & mask->t_ip6Port)) {
         return -1;
     }
     return 0;
}



int
SendToIp6Transport(data, len, sti, dti)
    const char *data;
    int len;
    const TransportInfo *sti;
    const TransportInfo *dti;
{
/* LVL7 */
    L7_RC_t rc;
    L7_sockaddr_in6_t from;
    int               fromlen = sizeof(from);
    L7_sockaddr_in6_t to;
    int               tolen = sizeof(to);
    int bytes_sent;

    if (dti != NULL) {
      from.sin6_family = L7_AF_INET6;
      from.sin6_port = dti->t_ip6Port;
      memcpy(&from.sin6_addr , dti->t_ip6Addr, 16);
    }
    else {
      from.sin6_family = L7_AF_INET6;
      from.sin6_port = 0;
      memset(&from.sin6_addr , 0, 16);
    }
    
    to.sin6_family = L7_AF_INET6;
    to.sin6_port = sti->t_ip6Port;
    memcpy(&to.sin6_addr , sti->t_ip6Addr, 16);

    /* Get any info ready */
    rc = osapiPktInfoSend(sti->fd, (void *)data, len, 0,
                          (L7_sockaddr_t *)&from, fromlen, 
                          (L7_sockaddr_t *)&to, tolen, 0, &bytes_sent, 
                          0, 0, 0, L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE);

    if (rc == L7_SUCCESS)
    {
      return bytes_sent;
    }
    return -1;
/* LVL7 */
}

int
ReceiveFromIp6Transport(data, len, sti, dti)
    char *data;
    int len;
    TransportInfo *sti;
    TransportInfo *dti;
{
/* LVL7 */
    L7_RC_t rc;
    int flags=0;
    L7_sockaddr_in6_t from;
    int               fromlen = sizeof(from);
    L7_sockaddr_in6_t to;
    int               tolen = sizeof(to);
    int ifIndex;
    int dum_hops;

    /* Get any info ready */
    rc = osapiPktInfoRecv(sti->fd, (void *)data, &len, &flags,
                          (L7_sockaddr_t *)&from, &fromlen, 
                          (L7_sockaddr_t *)&to, &tolen, &ifIndex, &dum_hops, NULL);

    if (rc == L7_SUCCESS)
    {
      sti->type = SR_IP6_TRANSPORT;
      sti->t_ip6Port = from.sin6_port;
      memcpy(sti->t_ip6Addr , &from.sin6_addr, 16);

      if (dti != NULL) {
        dti->type = SR_IP6_TRANSPORT;
        dti->t_ip6Port = to.sin6_port;
        memcpy(dti->t_ip6Addr , &to.sin6_addr, 16);
      }

      return len;
    }

    return -1;
/* LVL7 */
}

char *
FormatIp6TransportString(buf, len, ti)
    char *buf;
    int len;
    const TransportInfo *ti;
{

    inet_ntop(AF_INET6,ti->t_ip6Addr,buf,len);

    return buf;
}


int
OpenIp6Transport(ti)
    TransportInfo *ti;
{
    int optval = 1;
#ifdef SR_DEBUG
#define OPEN_IP_TRANSPORT_BUFLEN 40
    static char fmt[OPEN_IP_TRANSPORT_BUFLEN];

    FormatIp6TransportString(fmt, OPEN_IP_TRANSPORT_BUFLEN,
                            (const TransportInfo *) ti);
    DPRINTF((APTRACE, "Opening socket for IP6 transport %s\n", fmt));
#endif /* SR_DEBUG */
    ti->fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (ti->fd < 0) {
        DPRINTF((APERROR, "Could not open socket for IP6\n"));
        return -1;
    }
/* LVL7 */
    if(setsockopt(ti->fd, SOL_SOCKET, SO_REUSEADDR, 
		   &optval, sizeof(optval)) != 0) {
        DPRINTF((APWARN,
		 "Failed to set REUSEADDR for socket\n"));
    }
    if (osapiSetsockopt(ti->fd,IPPROTO_IP,L7_IP_PKTINFO,(L7_uchar8 *)&optval,sizeof(optval)) != L7_SUCCESS) {
       DPRINTF((APWARN,
		 "Failed to set IP_PKTINFO option for socket\n"));
    }
    if (osapiSetsockopt(ti->fd,IPPROTO_IPV6,L7_IPV6_PKTINFO,(L7_uchar8 *)&optval,sizeof(optval)) != L7_SUCCESS) {
       DPRINTF((APWARN,
		 "Failed to set L7_IPV6_PKTINFO option for socket\n"));
    }
    if(osapiSetsockopt(ti->fd,IPPROTO_IPV6,L7_IPV6_RECVPKTINFO,(L7_uchar8 *)&optval,sizeof(optval)) != L7_SUCCESS)
    {
       DPRINTF((APALWAYS,
                 "Failed to set L7_IPV6_RECVPKTINFO option for socket\n"));
    }
/* LVL7 */
    return 0;
}

int
BindIp6Transport(ti, tries)
    TransportInfo *ti;
    int tries;
{
    struct sockaddr_in6 addr;
    int status;

    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    memcpy(&addr.sin6_addr , ti->t_ip6Addr, 16);
    addr.sin6_port = ti->t_ip6Port;

    DPRINTF((APTRACE, "Binding to port %u\n", ntohs(ti->t_ip6Port)));
    while (tries != 0) {
        DPRINTF((APVERBOSE, "Attempt #%u to bind to socket\n", tries));
        status = bind(ti->fd, SA_CAST(&addr), sizeof(addr));
        if (status >= 0) {
            return 0;
        }
        if (tries > 0) {
            tries--;
        }
    }
    DPRINTF((APWARN, "Cannot bind to socket: %s\n", sys_errname(errno)));
    return -1;
}

void
CloseIp6Transport(ti)
    TransportInfo *ti;
{
    close(ti->fd);
}



OctetString *
Ip6TransportInfoToOctetString(ti)
    const TransportInfo *ti;
{
    OctetString *os;
 
    os = MakeOctetString(NULL, 18);
    if (os == NULL) {
        return NULL;
    }
    memcpy(os->octet_ptr, ti->t_ip6Addr, 16);
    memcpy(os->octet_ptr + 16, &ti->t_ip6Port, 2);
    return os;
}
 
int
OctetStringToIp6TransportInfo(ti, os)
    TransportInfo *ti;
    const OctetString *os;
{
    if (os == NULL) {
        return -1;
    }
    if (os->length != 18) {
        return -1;
    }
    ti->type = SR_IP6_TRANSPORT;
    memcpy(ti->t_ip6Addr, os->octet_ptr, 16);
    memcpy(&ti->t_ip6Port, os->octet_ptr + 16, 2);
    return 0;
}

int
TMaskOctetStringToIp6TransportInfo(ti, os)
    TransportInfo *ti;
    const OctetString *os;
{
    if (os == NULL) {
        return -1;
    }
    if (os->length == 0) {
        ti->type = SR_IP6_TRANSPORT;
        memset(ti->t_ip6Addr , 0xff,16);
        ti->t_ip6Port = 0;
        return 0;
    }
    if (os->length != 18) {
        return -1;
    }
    ti->type = SR_IP6_TRANSPORT;
    memcpy(ti->t_ip6Addr, os->octet_ptr, 16);
    memcpy(&ti->t_ip6Port, os->octet_ptr + 16, 2);
    return 0;
}



SR_INT32
ConvToken_IP6_TAddr(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OctetString **local = (OctetString **)value;
    SR_INT32 port;
    short unsigned int s_port;
    struct in6_addr a6;
    int cc;

    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            if(inet_pton(AF_INET6, *token, (void *)&a6) < 0)
                return -1;
            /* note cant use non-default port due to ':' in addr syntax */
            port = 0;

            *local = MakeOctetString(NULL, 18);
            if (*local == NULL) {
                return -1;
            }
            s_port = (short unsigned int)port;
            memcpy((*local)->octet_ptr, (unsigned char *)&a6, 16);
            (*local)->octet_ptr[16] = (unsigned char)((int) (s_port & 0xff00) >> 8);
            (*local)->octet_ptr[17] = (unsigned char)(s_port & 0xff);
            return 0;
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(48);
            if (*token == NULL) {
                return -1;
            }
            if ((*local)->length != 18) {
                strcpy(*token, "::0");
                return 0;
            } else {
                inet_ntop(AF_INET6, (*local)->octet_ptr, *token, 48);
                return 0;
            }
        default:
            return -1;
    }
}

