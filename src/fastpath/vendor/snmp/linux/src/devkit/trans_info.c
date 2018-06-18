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

#ifdef HAVE_STROPTS_H
#include <stropts.h>
#endif /* HAVE_STROPTS_H */

#include <malloc.h>

#include <errno.h>

#include <sys/types.h>




#include <netinet/in.h>

#include <string.h>

#include <sys/socket.h>


#include <netdb.h>

#include <arpa/inet.h>


#include <net/if.h>


#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif /* HAVE_SYS_SOCKIO_H */

#include <sys/ioctl.h>


#include "sr_snmp.h"
#include "sr_type.h"
#include "sr_trans.h"
#include "sr_ip.h"
#include "sr_tinfo.h"
#include "prnt_lib.h"
#include "diag.h"
SR_FILENAME


int SrGetInterfaceInfo(void **tInfo);



int SrGetInterfaceInfo(void **tInfo)
{
    FNAME("SrGetInterfaceInfo")
    char *buffer = NULL;
    TRANS_INFO *newTI = NULL, *tmpTI = NULL;
    TRANS_INFO *TI = NULL;
    struct sockaddr_in *addr_in = NULL;
    char *tptr = NULL;
    struct  ifconf  *ifc = NULL;
    struct ifreq *ifr = NULL, tmp_ifr;
    int sockfd = -1, buf_len;
    char *ifr_ptr = NULL;
    int ret = 0;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    buf_len = IFREQ_MAX*sizeof(struct ifreq);
    buffer = (char *)malloc(buf_len);
    if (buffer == NULL) {
        return -1;
    }
    memset(buffer, '\0', buf_len);
    if (sockfd < 0) {
        ret = -1;
        goto done;
    }
    ifc = (struct ifconf *)malloc(sizeof(struct ifconf));
    ifc->ifc_len = buf_len;
    ifc->ifc_buf = buffer;
    if (ioctl(sockfd, SIOCGIFCONF, ifc) < 0) {
        DPRINTF((APERROR, "%s: Cannot get interface info\n", Fname));
        ret = -1;
        goto done;
    }
    for (ifr_ptr = buffer; ifr_ptr < buffer + ifc->ifc_len; ) {
        ifr = (struct ifreq *)ifr_ptr;
        memset(&tmp_ifr, '\0', sizeof(struct ifreq));
        strcpy(tmp_ifr.ifr_name, ifr->ifr_name);
        tptr = strchr(tmp_ifr.ifr_name, ':');
        if (tptr != NULL) {
            tptr[0] = '\0';
        }
        /*
         * check whether an interface is up or down.
         */
        if (ioctl(sockfd, SIOCGIFFLAGS, &tmp_ifr) < 0) {
            DPRINTF((APWARN, "%s: Failed on ioctl SIOCGIFFLAGS\n",
                                                             Fname));
            continue;
        }
        if ((tmp_ifr.ifr_flags & IFF_UP) == 0) {
            /*
             *  ignore an interface which is down.
             */
            continue;
        }

        newTI = (TRANS_INFO *)malloc(sizeof(TRANS_INFO));
        if (newTI == NULL) {
            goto done;
        }
        ifr_ptr += sizeof(ifr->ifr_name) + sizeof(struct sockaddr);
        memset(newTI, '\0', sizeof(TRANS_INFO));
        newTI->tInfo.t_ipPort = htons((short) GetSNMPPort());
        newTI->tInfo.type = SR_IP_TRANSPORT;
        addr_in = (struct sockaddr_in *) &ifr->ifr_addr;
        newTI->tInfo.t_ipAddr = addr_in->sin_addr.s_addr;
        newTI->next = NULL;
        if (TI == NULL) {
            TI = newTI;
        }
        else {
            for (tmpTI = TI;
                     tmpTI->next != NULL; tmpTI = tmpTI->next) {}
            tmpTI->next = newTI;
        }
        ret++;
    }

  done:
    if (sockfd != -1) {
        close(sockfd);
    }
    if (ifc != NULL) {
	free(ifc);
    }
    if (buffer != NULL) {
        free(buffer);
    }
    *tInfo = (void *)TI;
    return ret;
}




/*
 * GetIpTransportInfo
 *
 * get all existing interface info(both ipv4 and ipv6), then save those 
 * info into the transport info array.
 *
 * return value
 *    success  total number of interface
 *    failure  -1
 */
int GetIpTransportInfo(int bind_type, void **tInfo, 
		       char *ipv4_bind_addr, char *ipv6_bind_addr)
{
    TRANS_INFO *newTI = NULL;
    TRANS_INFO *TI = NULL;
    SR_UINT32 bind_addr;
    int ret = 0;

    switch (bind_type) {
        case BIND_DEFAULT:
            newTI = (TRANS_INFO *)malloc(sizeof(TRANS_INFO));
            if (newTI == NULL) {
                ret = -1;
                goto done;
            }
            memset(newTI, '\0', sizeof(TRANS_INFO));
            newTI->tInfo.t_ipPort = htons((short) GetSNMPPort());
            newTI->tInfo.type = SR_IP_TRANSPORT;
            if (ipv4_bind_addr != NULL) {
                bind_addr = inet_addr(ipv4_bind_addr);
                if (bind_addr == -1) {
                    DPRINTF((APERROR,
                        "Cannot resolve snmp_bindaddr %s\n",
                                                        ipv4_bind_addr));
                }
                else {
                    newTI->tInfo.t_ipAddr = bind_addr;
                    DPRINTF((APTRACE,
                        "SNMP port bind restricted to address %s\n",
                                                        ipv4_bind_addr));
                }
            }
            newTI->next = NULL;
            TI = newTI;
            ret++;
            *tInfo = (void *)TI;
            break;
        case BIND_IP_ALL:
            ret = SrGetInterfaceInfo(tInfo); 
            break;
        default:
            break;
    }

  done:
    return ret;
}

