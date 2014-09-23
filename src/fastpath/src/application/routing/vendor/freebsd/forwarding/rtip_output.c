/*
 * Copyright (c) 1982, 1986, 1988, 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ip_output.c	8.3 (Berkeley) 1/21/94
 * $FreeBSD: src/sys/netinet/ip_output.c,v 1.99.2.14 2001/04/25 12:47:12 ru Exp $
 */
#define	_IP_VHL
#include "rtip.h"
#include "rtip_var.h"
#include "rtip_icmp.h"
#include "rtmbuf.h"
#include "rtiprecv.h"
#include "sysapi.h"
#include <l7_ip_api.h>
#include "l3_defaultconfig.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "nimapi.h"
#include "ip_util.h"

unsigned short ip_id;

int	ip_optcopy (struct ip *, struct ip *);
extern void ipMapTraceWrite(L7_uchar8 *traceMsg);

extern L7_uint32 ipMapTraceFlags;


/*
 * IP output.  The packet in mbuf chain m contains a skeletal IP
 * header (with len, off, ttl, proto, tos, src, dst).
 * The mbuf chain containing the packet will be freed.
 * The mbuf opt, if present, will not be freed.
 */
int
rtip_output(m0, opt, routerIP, netMask, intIfNum, flags, imo, srcrt)
	struct rtmbuf *m0;
	struct rtmbuf *opt;
	int routerIP;
	unsigned int netMask;
    int intIfNum;
	int flags;
	struct ip_moptions *imo;
    int srcrt;
{
	struct ip *ip;
	struct rtm_ifnet *ifp = NULL, tempIfp;
	struct rtmbuf *m = m0;
	int hlen = sizeof (struct ip);
	int error = 0;
    int if_flags = 0;
    int tosEnable;
    int tosvalue = 0;
	int matchRoute = IP_NONLOCAL_ROUTE;
  sysnet_pdu_info_t pduInfo;
  SYSNET_PDU_RC_t hookVerdict;
  L7_uint32 mtu_size;
  L7_RC_t rc;

  /* Call interceptors who are interested in IP frames to be transmitted that were
  ** caused by incoming frames.  If L7_TRUE is returned, the frame was either
  ** discarded or consumed, which means that the network buffer has been freed by the
  ** intercept call, or will be freed by the consumer.
  */
  memset(&pduInfo, 0, sizeof(sysnet_pdu_info_t));
  if (m->rtm_pkthdr.rcvif != (struct rtm_ifnet *)0)
    pduInfo.intIfNum = m->rtm_pkthdr.rcvif->if_index;
  pduInfo.destIntIfNum = intIfNum;
  if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_RECV_OUT,
                           (L7_netBufHandle)m->rtm_bufhandle,
                           &pduInfo, L7_NULLPTR, &hookVerdict) == L7_TRUE)
  {
    if (hookVerdict == SYSNET_PDU_RC_DISCARD)
      SYSAPI_IP_STATS_INCREMENT(FD_CNFGR_NIM_MIN_CPU_INTF_NUM, 
                                L7_PLATFORM_CTR_TX_IP_OUT_DISCARDS);
    rtm_freem(m);
    goto done;
  }


#ifdef	DIAGNOSTIC
	if ((m->rtm_flags & M_PKTHDR) == 0)
		panic("ip_output no HDR");
	if (!routerIP)
		panic("ip_output no route, proto = %d",
		      rtmtod(m, struct ip *)->ip_p);
#endif
    
	ip = rtmtod(m, struct ip *);

  if (ipMapTraceFlags & IPMAP_TRACE_FORWARDING)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    L7_uchar8 srcStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(ntohl(ip->ip_src.s_addr), srcStr);
    osapiInetNtoa(ntohl(ip->ip_dst.s_addr), destStr);
    sprintf(traceBuf, "rtip_output():  src %s, dst %s, proto %d, ttl %d.\n", 
            srcStr, destStr, ip->ip_p, ip->ip_ttl);
    ipMapTraceWrite(traceBuf);
  }

	/*
	 * Fill in IP header.
	 */
	if ((flags & (IP_FORWARDING|IP_RAWOUTPUT)) == 0) {
        ip->ip_vhl = IP_MAKE_VHL(IPVERSION, hlen >> 2);
		NTOHS(ip->ip_off);
		ip->ip_off &= IP_DF;
		HTONS(ip->ip_off);
		ip->ip_id = osapiHtons(ip_id++);
		L7_ipstat.ips_localout++;
	} else {
		hlen = IP_VHL_HL(ip->ip_vhl) << 2;
	}

    if(!routerIP){
        tosEnable = ipMapRtrTosForwardingModeGet();
        if(tosEnable == L7_ENABLE)
        {       
            if(ip->ip_p==IPPROTO_ICMP)
                matchRoute = rtFindRoute(&intIfNum, &routerIP, 
                                           osapiNtohl(ip->ip_dst.s_addr),
                                           osapiNtohl(ip->ip_src.s_addr));      
            else{
                tosvalue = ip->ip_tos;
                matchRoute = rtFindRouteWithTOS(&intIfNum, &routerIP, 
                                                  osapiNtohl(ip->ip_dst.s_addr), 
                                                  osapiNtohl(ip->ip_src.s_addr),
                                                  tosvalue);  
            }
        }
        else
            matchRoute = rtFindRoute(&intIfNum, &routerIP, 
                                       osapiNtohl(ip->ip_dst.s_addr),
                                       osapiNtohl(ip->ip_src.s_addr));                
        
        if(!routerIP){
            error = EHOSTUNREACH;
            L7_ipstat.ips_noroute++;
            goto bad;
        }
		if (matchRoute == IP_LOCAL_ROUTE)
		{
		  netMask = rtGetSubnetMaskOfIntf(intIfNum);
		}
    }
    memset(&tempIfp, 0, sizeof(tempIfp));
    ifp = &tempIfp;
    ifp->if_index = intIfNum;                  
    if_flags |= IFF_BROADCAST | IFF_MULTICAST;

    if(rtGetUpStatusOfIntf(intIfNum) == L7_SUCCESS)
        if_flags |= IFF_UP;

    ifp->if_flags |= if_flags;
    if((ifp->if_flags & IFF_UP) == 0){
		L7_ipstat.ips_noroute++;
		error = EHOSTUNREACH;
		goto bad;
	}
    
	if (IN_MULTICAST(osapiNtohl(ip->ip_dst.s_addr))) {
		m->rtm_flags |= M_MCAST;
		/*
		 * Confirm that the outgoing interface supports multicast.
		 */
		if ((imo == NULL) || (imo->imo_multicast_vif == -1)) {
			if ((ifp->if_flags & IFF_MULTICAST) == 0) {
				L7_ipstat.ips_noroute++;
				error = ENETUNREACH;
				goto bad;
			}
		} 

		/*
		 * Multicasts with a time-to-live of zero may be looped-
		 * back, above, but must not be transmitted on a network.
		 * Also, multicasts addressed to the loopback interface
		 * are not sent -- the above call to ip_mloopback() will
		 * loop back a copy if this host actually belongs to the
		 * destination group on the loopback interface.
		 */
		if (ip->ip_ttl == 0 || ifp->if_flags & IFF_LOOPBACK) {
            if (m->rtm_bufhandle != NULL)
                SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
			rtm_freem(m);
            goto done;
		}

		goto sendit;
	}

	/*
	 * If source address not specified yet, use address
	 * of outgoing interface.
	 */
	if (ip->ip_src.s_addr == osapiHtonl(INADDR_ANY)) {
        ip->ip_src.s_addr = osapiHtonl(rtGetIPAddrOfIntf(intIfNum));
	}

	/* Cannot forward a broadcast packet. So clear broadcast flag. */ 
  m->rtm_flags &= ~M_BCAST; 

sendit:
	
    rc = ipMapIntfEffectiveIpMtuGet(intIfNum, &mtu_size);
    if (rc != L7_SUCCESS)
    {
      mtu_size = L7_PORT_ENET_ENCAP_DEF_MTU;  /* 1500 */
    }

    if (((unsigned short)osapiNtohs(ip->ip_len)) <= mtu_size){ 
		ip->ip_sum = 0;
		ip->ip_sum = rtin_cksum(m, hlen);

    error = rtInIPForwardArpTable(routerIP, netMask, m, intIfNum, srcrt);
		goto done;
	}
	/*
	 * Too large for interface; fragment if possible.
	 * Must be able to put at least 8 bytes per fragment.
	 */
    /* Next 2 lines are added because we r not handling fragmentation currently */
     error = EMSGSIZE;

     SYSAPI_IP_STATS_INCREMENT(FD_CNFGR_NIM_MIN_CPU_INTF_NUM, 
                                L7_PLATFORM_CTR_TX_IP_OUT_DISCARDS);
	 goto bad;
done:
	return (error);
bad:
    if (m->rtm_bufhandle != NULL)
                SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
	rtm_freem(m0);
    goto done;
}



