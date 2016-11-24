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


#include <sys/utsname.h>
#include <linux/version.h>

#include "sr_type.h"
#include "sr_time.h"








#include "sr_snmp.h"
#include "compat.h"
#include "diag.h"
SR_FILENAME

#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "sr_trans.h"




#include "sr_ip.h"
#include "sr_tinfo.h"
#include "sri/sr_kwmatch.h"



/* LVL7 */
#include <l7_common.h>

#include <dtlapi.h>
#include "osapi_support.h"
/* LVL7 end */

static SR_UINT32       LocalIPAddress = (SR_UINT32)0;
char *send_port_range = NULL;

#define SA_CAST(x) (struct sockaddr *)x







/*
 * GetLocalIPAddress:
 *
 *   This routine will attempt to determine the local IP address of a device,
 *   and return this value as an unsigned 32-bit integer.  Returned in host
 *   byte order.
 */
SR_UINT32
GetLocalIPAddress()
{
    FNAME("GetLocalIPAddress")
#ifdef OLD_CODE
    char hostname[64];
    struct hostent *hp;
#endif /* OLD_CODE */

    /* LVL7 */
    /* assume local IP address of 0, doesn't block traps. */
    LocalIPAddress = 0;

    #ifdef OLD_CODE
    /* LVL7 end */

    if (LocalIPAddress != (SR_UINT32)0) {
        return LocalIPAddress;
    }






    if (!gethostname(hostname, 64)) {
        DPRINTF((APTRACE, "%s: hostname is %s\n", Fname, hostname));
        LocalIPAddress = inet_addr(hostname);
        if (LocalIPAddress == (SR_UINT32) -1) {


            hp = gethostbyname(hostname);
            
            if (hp != NULL) {
                    memcpy((char *)&LocalIPAddress,
                          (char *)hp->h_addr,
                          (int)sizeof(SR_INT32));
            }
        }
        LocalIPAddress = osapiNtohl(LocalIPAddress);
    }








    if (LocalIPAddress == (SR_UINT32) -1) {
        DPRINTF((APERROR, "%s: %s\n%s\n",
		Fname,
	        "Could not determine entity IP address.  This can cause",
                "SNMP communication failure.  Is host system misconfigured?"));
    }

    #endif /* OLD_CODE */
    /* LVL7 end */

    return LocalIPAddress;
}

/*
 * TranslateAddress
 *
 * Translate a string representing an IP address to an unsigned long
 * representation in HOST byte order.  The string can be the dotted
 * form, or some alias for the host.
 */
SR_UINT32
TranslateAddress(dest)
    char           *dest;
{
    struct hostent *host;
    SR_UINT32   sin_addr;

    sin_addr = inet_addr(dest);

    if (sin_addr == -1) {
	host = gethostbyname(dest);
	
	if (host) {
	    memcpy(&sin_addr, host->h_addr, sizeof(sin_addr));
	}
    }

    return(osapiNtohl(sin_addr));
}


/*
 * Convert an IP Address (in the form of an unsigned long) to
 * an OctetString.
 */
OctetString *
IPToOctetString(ipAddr)
   SR_UINT32     ipAddr;
{
   unsigned char buff[4];
 
   buff[0] = (ipAddr >> 24) & 0xff;
   buff[1] = (ipAddr >> 16) & 0xff;
   buff[2] = (ipAddr >>  8) & 0xff;
   buff[3] = (ipAddr >>  0) & 0xff;

   return (MakeOctetString(buff, ((SR_INT32)4)));
}

/*
 * Convert an OctetString to an IP6 Address (in the form of an L7_in6_addr_t.)
 */
void
OctetStringToIP6(os, buff)
   const OctetString *os;
   unsigned char *buff;
{
   int i;

   for (i = 15; i >= 0; i--) {
       buff[i] = (unsigned char) os->octet_ptr[i];
   }
}


/*
 * Convert an OctetString to an IP Address (in the form of an unsigned long).
 */
SR_UINT32
OctetStringToIP(os)
   const OctetString *os;
{
   int i;
   SR_UINT32     ipAddr = ((SR_INT32)0);

   for (i = 3; i >= 0; i--) {
       ipAddr |= (SR_UINT32) os->octet_ptr[i] << ((3 - i) * 8);
   }
   return ipAddr;
}

/*
 * Convert a Network Address to an IP Address (in the form of an unsigned long).
 */
SR_UINT32
NetworkAddressToIP(os)
   const OctetString *os;
{
   int i;
   SR_UINT32     ipAddr = ((SR_INT32)0);

   for (i = 4; i >= 1; i--) {
       ipAddr |= (SR_UINT32) os->octet_ptr[i] << ((4 - i) * 8);
   }
   return ipAddr;
}

/*
 * Convert an OctetString to an unsigned long.
 * This function will take an OctetString of length <= 4 and
 * convert it to an unsigned long. This function is similiar
 * to OctetStringToIP(), but it also checks the length.
 */
SR_UINT32
OctetStringToUlong (os)
    const OctetString *os;
{
    int i, len;
    SR_UINT32 ul = ((SR_INT32)0);

    /*
     * limit the conversion to 4 octets [array elements 0-3]
     */
    len = os->length;
    if (len > 4) {
        len = 4;
    }
 
    for (i = 0; i < len; i++) {
        ul = (ul << 8) | os->octet_ptr[i];
    }
    return ul;
}



/****************************************************************
 * 
 * Function: BindIpTransportGivenPortRange
 * Input:    TransportInfo structure and string specifying ports
 *           to try. String format: a,b,c-f,g
 * Behavior: This function will step through the ports provided
 *           in the string an attempt binds.  Exit will occur with
 *           first successful bind.
 * Return value:  Sucessful bind return 0
 *                Unable to bind return -1
 * Note:  Instead of trying to bind multiple times in this function
 *        retry should be implemented by calling recalling 
 *        the function with the string.
 *        added IPv6 support.
 *****************************************************************/

int
BindIpTransportGivenPortRange(ti)
    TransportInfo *ti;
{
    struct sockaddr_in addr;
    int status = 0;
    int i=0;
    int isrange=FALSE;
    char *tok = NULL;
    char range[10];
    char *rangeptr = NULL;
    char *buf = NULL;

    FNAME("BindIpTransportGivenPortRange")

    switch (ti->type) {
        default:
#ifdef SR_CLEAR_AUTOS
            memset(&addr, 0, sizeof(addr));
#endif  /* SR_CLEAR_AUTOS */
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = ti->t_ipAddr;
            break;
    }

    if (send_port_range != NULL){
        buf = strdup(send_port_range);
    } 
    else {
        DPRINTF((APTRACE, "%s: Range not valid\n", Fname));
        goto fail;
    }

    if ((tok = (char *)strtok(buf, ",\n\t\r"))==NULL){
        goto fail;
    }

    for (i=0; i<strlen(tok); i++){
        if (tok[i] == '-'){
            isrange = TRUE;
        }
    }
 
    if (isrange){
        strcpy(range, tok);
        rangeptr = range;
        status = HandlePortRange(ti, rangeptr);
        if(status >= 0){
           goto done;
        }
        status=0;
        isrange = FALSE;
    }
    else {
        DPRINTF((APTRACE, 
            "%s: Attempting bind to port %u\n", Fname, atoi(tok)));
        switch (ti->type) {
            default:
                addr.sin_port = htons((unsigned short)atoi(tok));
                status = bind(ti->fd, SA_CAST(&addr), sizeof(addr));
                break;
        }
 
        if (status >= 0) {
            goto done;
        } else {
            DPRINTF((APTRACE, 
                   "%s: Unable to bind to port %u\n", Fname, addr.sin_port));
        }
        status=0;
    }
 
    /* step through the remainder of the parameters */
 
    while((tok = (char *)strtok(NULL, ",\n\r\t"))!=NULL){
        for(i=0; i<strlen(tok); i++){
            if(tok[i] == '-'){
                isrange = TRUE;
            }
        }

        if(isrange){
            strcpy(range, tok);
            rangeptr = range;
 
            status  = HandlePortRange(ti, rangeptr);
            if(status >=0){
                goto done;
            }
            isrange = FALSE;
            status=0;
        }
        else {
            DPRINTF((APTRACE, "%s: Attempting bind to port %u\n", 
                                                      Fname, atoi(tok)));
            switch (ti->type) {
                default:
                    addr.sin_port = htons((unsigned short)atoi(tok));
                    status = bind(ti->fd, SA_CAST(&addr), sizeof(addr));
                    break;
            }

            if (status >= 0) {
                DPRINTF((APTRACE, "Bind successful\n"));
                goto done; 
            } else {
                 DPRINTF((APTRACE, 
                      "%s: Unable to bind to port %u\n", Fname, addr.sin_port));
                 status=0;
            }
        }
    }

done:
    if (buf){
        free(buf);
    }
    return(0);
fail:
    if(buf){
        free(buf);
    }
    return(-1);
}

int
HandlePortRange(TransportInfo *ti, char *range)
{
    struct sockaddr_in addr;
    char *rtok = NULL;
    char tempbuf[30];
    int firstval=0, i=0;
    int secondval=0;
    int status=0;

    FNAME("HandlePortRange")

    switch (ti->type) {
        default:
#ifdef SR_CLEAR_AUTOS
            memset(&addr, 0, sizeof(addr));
#endif  /* SR_CLEAR_AUTOS */
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = ti->t_ipAddr;
            break;
    }

    strcpy(tempbuf, range);
    rtok = (char *)&tempbuf;
 
    while (tempbuf[i] != '\0'){
        if (tempbuf[i] == '-'){
            tempbuf[i]='\0';
            firstval = atoi(rtok);
            if (firstval==0){
                DPRINTF((APWARN, "%s: Incorrect range format", Fname));
                return(-1);
            }
            i++;
            rtok=&tempbuf[i];
            continue;
        }
        i++;
    }
    secondval = atoi(rtok);
    if ( secondval == 0 ){
        DPRINTF((APWARN, "%s: Incorrect range format", Fname));
        return(-1);
    }
    DPRINTF((APTRACE, "%s: %d to %d\n", Fname, firstval, secondval));

    if (firstval > secondval ) {
        DPRINTF((APWARN, "%s: Incorrect range format", Fname));
        return(-1);
    }

    for (i=firstval; i<=secondval; i++){
        status=0;
        DPRINTF((APTRACE, "%s: Attempting bind to port %u\n", Fname, i));
        switch (ti->type) {
            default:
                addr.sin_port = htons((unsigned short)i);
                status = bind(ti->fd, SA_CAST(&addr), sizeof(addr));
                break;
        }
        if (status >= 0) {
            DPRINTF((APTRACE, "%s: Bind successful\n", Fname));
            return 0;
        }
    }
 
    memset(tempbuf, 0, sizeof(tempbuf));
 
    return(-1);
}

SR_INT32
ConvToken_IP_TAddr(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OctetString **local = (OctetString **)value;
    SR_INT32 s1, s2, s3, s4, port;
    short unsigned int s_port;
    int cc;

    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            cc = sscanf(*token, INT32_FORMAT"."INT32_FORMAT"."INT32_FORMAT"."INT32_FORMAT":"INT32_FORMAT,
                        &s1, &s2, &s3, &s4, &port);
            if (cc < 4) {
                return -1;
            }
            if (cc == 4) {
                port = (SR_INT32)0;
            }
            if ((s1 > 255) ||
                (s1 < 0)   ||
                (s2 > 255) ||
                (s2 < 0)   ||
                (s3 > 255) ||
                (s3 < 0)   ||
                (s4 > 255) ||
                (s4 < 0)   ||
                (port < 0) ||
                (port > 65535)) {
                return -1;
            }
            *local = MakeOctetString(NULL, 6);
            if (*local == NULL) {
                return -1;
            }
            s_port = (short unsigned int)port;
            (*local)->octet_ptr[0] = (unsigned char)s1;
            (*local)->octet_ptr[1] = (unsigned char)s2;
            (*local)->octet_ptr[2] = (unsigned char)s3;
            (*local)->octet_ptr[3] = (unsigned char)s4;
            (*local)->octet_ptr[4] = (unsigned char)((int) (s_port & 0xff00) >> 8);
            (*local)->octet_ptr[5] = (unsigned char)(s_port & 0xff);
            return 0;
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(32);
            if (*token == NULL) {
                return -1;
            }
            if ((*local)->length != 6) {
                strcpy(*token, "0.0.0.0:0");
                return 0;
            } else {
                s1 = (SR_INT32)(*local)->octet_ptr[0];
                s2 = (SR_INT32)(*local)->octet_ptr[1];
                s3 = (SR_INT32)(*local)->octet_ptr[2];
                s4 = (SR_INT32)(*local)->octet_ptr[3];
                s_port = (((short unsigned int)(*local)->octet_ptr[4]) << 8) +
                         ((short unsigned int)(*local)->octet_ptr[5]);
                port = (SR_UINT32)s_port;
                sprintf(*token, "%d.%d.%d.%d:%d", (int)s1, (int)s2, (int)s3, (int)s4, (int)port);
                return 0;
            }
        default:
            return -1;
    }
}

SR_INT32
ConvToken_IP_TAddrMask(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OctetString **local = (OctetString **)value;
    SR_INT32 s1, s2, s3, s4, port;
    short unsigned int s_port;
    int cc;

    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            if (strcmp(*token, "-") != 0) {
                cc = sscanf(*token, INT32_FORMAT"."INT32_FORMAT"."INT32_FORMAT"."INT32_FORMAT":"INT32_FORMAT,
                        &s1, &s2, &s3, &s4, &port);
                if (cc < 4) {
                    return -1;
                }
                if (cc == 4) {
                    port = (SR_INT32)0;
                }
                if ((s1 > 255) ||
                    (s1 < 0)   ||
                    (s2 > 255) ||
                    (s2 < 0)   ||
                    (s3 > 255) ||
                    (s3 < 0)   ||
                    (s4 > 255) ||
                    (s4 < 0)   ||
                    (port < 0) ||
                    (port > 65535)) {
                    return -1;
            }
                *local = MakeOctetString(NULL, 6);
                if (*local == NULL) {
                    return -1;
                }
                s_port = (short unsigned int)port;
                (*local)->octet_ptr[0] = (unsigned char)s1;
                (*local)->octet_ptr[1] = (unsigned char)s2;
                (*local)->octet_ptr[2] = (unsigned char)s3;
                (*local)->octet_ptr[3] = (unsigned char)s4;
                (*local)->octet_ptr[4] = (unsigned char)((int) (s_port & 0xff00) >> 8);
                (*local)->octet_ptr[5] = (unsigned char)(s_port & 0xff);
                return 0;
            }
            else {
                *local = MakeOctetString(NULL, 0);
                if (*local == NULL) {
                    return -1;
                }
                return 0;
            }
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(32);
            if (*token == NULL) {
                return -1;
            }
            if ((*local)->length == 0) {
                strcpy(*token, "-");
                return 0;
            } else if ((*local)->length != 6) {
                strcpy(*token, "0.0.0.0:0");
                return 0;
            } else {
                s1 = (SR_INT32)(*local)->octet_ptr[0];
                s2 = (SR_INT32)(*local)->octet_ptr[1];
                s3 = (SR_INT32)(*local)->octet_ptr[2];
                s4 = (SR_INT32)(*local)->octet_ptr[3];
                s_port = (((short unsigned int)(*local)->octet_ptr[4]) << 8) +
                         ((short unsigned int)(*local)->octet_ptr[5]);
                port = (SR_UINT32)s_port;
                sprintf(*token, "%d.%d.%d.%d:%d", (int)s1, (int)s2, (int)s3, (int)s4, (int)port);
                return 0;
            }
        default:
            return -1;
    }
}

void
CopyIpTransportInfo(dst, src)
    TransportInfo *dst;
    const TransportInfo *src;
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
}

void
CopyIpTransportAddr(dst, src)
    TransportInfo *dst;
    const TransportInfo *src;
{
    switch (src->type) {
        default:
            dst->t_ipAddr = src->t_ipAddr;
            dst->t_ipPort = src->t_ipPort;
            break;
    }
}

int
CmpIpTransportInfo(ti1, ti2)
    const TransportInfo *ti1;
    const TransportInfo *ti2;
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
}

int
CmpIpTransportInfoWithMask(ti1, ti2, mask)
    const TransportInfo *ti1;
    const TransportInfo *ti2;
    const TransportInfo *mask;
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
}


OctetString *
IpTransportInfoToOctetString(ti)
    const TransportInfo *ti;
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
}
 
int
OctetStringToIpTransportInfo(ti, os)
    TransportInfo *ti;
    const OctetString *os;
{
    if (os == NULL) {
        return -1;
    }
    if (os->length != 6) {
        return -1;
    }
    ti->type = SR_IP_TRANSPORT;
    memcpy(&ti->t_ipAddr, os->octet_ptr, 4);
    memcpy(&ti->t_ipPort, os->octet_ptr + 4, 2);
    return 0;
}

int
TMaskOctetStringToIpTransportInfo(ti, os)
    TransportInfo *ti;
    const OctetString *os;
{
    if (os == NULL) {
        return -1;
    }
    if (os->length == 0) {
        ti->type = SR_IP_TRANSPORT;
        ti->t_ipAddr = inet_addr("255.255.255.255");
        ti->t_ipPort = 0;
        return 0;
    }
    if (os->length != 6) {
        return -1;
    }
    ti->type = SR_IP_TRANSPORT;
    memcpy(&ti->t_ipAddr, os->octet_ptr, 4);
    memcpy(&ti->t_ipPort, os->octet_ptr + 4, 2);
    return 0;
}



int
SendToIpTransport(data, len, sti, dti)
    const char *data;
    int len;
    const TransportInfo *sti;
    const TransportInfo *dti;
{
/* LVL7 */
    L7_RC_t rc;
    L7_sockaddr_in_t from;
    int              fromlen = sizeof(from);
    L7_sockaddr_in_t to;
    int              tolen = sizeof(to);
    int bytes_sent;

    if (dti != NULL) {
      from.sin_family = L7_AF_INET;
      from.sin_port = dti->t_ipPort;
      from.sin_addr.s_addr = dti->t_ipAddr;
    }
    else {
      from.sin_family = L7_AF_INET;
      from.sin_port = 0;
      from.sin_addr.s_addr = 0;
    }

    to.sin_family = L7_AF_INET;
    to.sin_port = sti->t_ipPort;
    to.sin_addr.s_addr = sti->t_ipAddr;

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
ReceiveFromIpTransport(data, len, sti, dti)
    char *data;
    int len;
    TransportInfo *sti;
    TransportInfo *dti;
{
/* LVL7 */
    L7_RC_t rc;
    int flags=0;
    L7_sockaddr_in_t from;
    int              fromlen = sizeof(from);
    L7_sockaddr_in_t to;
    int              tolen = sizeof(to);
    int ifIndex;
    int dum_hops;

    /* Get any info ready */
    rc = osapiPktInfoRecv(sti->fd, (void *)data, &len, &flags,
                          (L7_sockaddr_t *)&from, &fromlen, 
                          (L7_sockaddr_t *)&to, &tolen, &ifIndex, &dum_hops, NULL);

    if (rc == L7_SUCCESS)
    {
      sti->type = SR_IP_TRANSPORT;
      sti->t_ipPort = from.sin_port;
      sti->t_ipAddr = from.sin_addr.s_addr;

      if (dti != NULL) {
        dti->type = SR_IP_TRANSPORT;
        dti->t_ipPort = to.sin_port;
        dti->t_ipAddr = to.sin_addr.s_addr;
      }

      return len;
    }
    return -1;
/* LVL7 */
}

char *
FormatIpTransportString(buf, len, ti)
    char *buf;
    int len;
    const TransportInfo *ti;
{
    SR_UINT32 addr = osapiNtohl(ti->t_ipAddr);
    unsigned long port = (unsigned long)osapiNtohs(ti->t_ipPort);

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
}

#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif /* KERNEL_VERSION */

/*
 * OpenIpTransport
 *
 * open a socket based on the transport type
 */
int
OpenIpTransport(ti)
    TransportInfo *ti;
{
    const int optval = 1;
    unsigned int version;
    int a, b, c, ret;
    struct utsname buf;
#ifdef SR_DEBUG
#define OPEN_IP_TRANSPORT_BUFLEN 22
    static char fmt[OPEN_IP_TRANSPORT_BUFLEN];

    switch (ti->type) {
        default:
            FormatIpTransportString(fmt, OPEN_IP_TRANSPORT_BUFLEN,
                                                  (const TransportInfo *) ti);
            DPRINTF((APTRACE, "Opening socket for IP transport %s\n", fmt));
            break;  
    }
#endif /* SR_DEBUG */
    switch (ti->type) {
        default:
            ti->fd = socket(AF_INET, SOCK_DGRAM, 0);
            break;
    }
    if (ti->fd < 0) {
        DPRINTF((APERROR, "Could not open socket for IP\n"));
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

/* LVL7 */
    return 0;
}

int
BindIpTransport(ti, tries)
    TransportInfo *ti;
    int tries;
{
    int tryCount = tries;
    struct sockaddr_in addr;
    int status;

    if (ti == NULL) {
	return -1;
    }
    switch(ti->type) {    
        default:
#ifdef SR_CLEAR_AUTOS
            memset(&addr, 0, sizeof(addr));
#endif  /* SR_CLEAR_AUTOS */
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = ti->t_ipAddr;
            addr.sin_port = ti->t_ipPort;
            break;
    }

    DPRINTF((APTRACE, "Binding to port %u\n", osapiNtohs(ti->t_ipPort)));
    while (tryCount != 0) {
        DPRINTF((APVERBOSE, "Attempt #%u to bind to socket\n", 
                                           tries + 1 - tryCount));
        switch(ti->type) {
            default:
                status = bind(ti->fd, SA_CAST(&addr), sizeof(addr));
                break;
        }
        if (status >= 0) {
            return 0;
        }
        if (tryCount > 0) {
            tryCount--;
        }
    }
    DPRINTF((APWARN, "Cannot bind to socket: %s\n", sys_errname(errno)));
    return -1;
}

void
CloseIpTransport(ti)
    TransportInfo *ti;
{
    close(ti->fd);
}

