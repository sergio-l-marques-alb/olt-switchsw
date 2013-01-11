/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  rtip_input.c
*
* @purpose   IP Forwarding
*
* @component
*
* @comments  the mbuf structure originally used in FreeBSD has been
*            replaced by a similar kind of structure called rtmbuf
*            so that new data members can be added to it.Also a prefix
*            of 'rt' has been added to all the mbuf related functions,
*            structures and data members, so that they won't clash with
*            the VxWorks names.
*
* @create    03/26/2001
*
* @author    dipanjan
*
* @end
*
**********************************************************************/
/*
 * Copyright (c) 1982, 1986, 1988, 1993
 *  The Regents of the University of California.  All rights reserved.
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
 *  This product includes software developed by the University of
 *  California, Berkeley and its contributors.
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
 *  @(#)ip_input.c  8.2 (Berkeley) 1/4/94
 * $FreeBSD: src/sys/netinet/ip_input.c,v 1.130.2.21 2001/03/08 23:14:54 iedowse Exp $
 */
/*********************************************************************
 *
 ********************************************************************/

#define _IP_VHL

/* lvl7_@ip_forward start*/
#include "rtip.h"
#include "rtip_icmp.h"
#include "rtmbuf.h"
#include "rtip_var.h"
#include "rtip_fw.h"
#include "rtiprecv.h"
#include "osapi.h"                    /* LVL7_MOD */
#include "sysapi.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "buff_api.h"
#include "l7_ip_api.h"
#include "l3_defaultconfig.h"
#include "ip_util.h"
#include "log.h"
#include "l7_vrrp_api.h"


/* lvl7_@ip_forward end*/

#define MAX_IPOPTLEN 40
#define PRC_NCMDS 22

int rsvp_on = 0;
struct socket *ip_rsvpd;

int rtipforwarding = FD_RTR_DEFAULT_FORWARDING;


static int  ip_dosourceroute = 1;
static int  ip_acceptsourceroute = 1;

/*
 * XXX - Setting ip_checkinterface mostly implements the receive side of
 * the Strong ES model described in RFC 1122, but since the routing table
 * and transmit implementation do not implement the Strong ES model,
 * setting this to 1 results in an odd hybrid.
 *
 * XXX - ip_checkinterface currently must be disabled if you use ipnat
 * to translate the destination address to another local interface.
 *
 * XXX - ip_checkinterface must be disabled if you add IP aliases
 * to the loopback interface instead of the interface where the
 * packets for those addresses are received.
 */
/*static int  ip_checkinterface = 0;*/

static int  ipprintfs = 0;

extern  struct domain inetdomain;

struct L7_ipstat L7_ipstat;

#ifdef IPCTL_DEFMTU
SYSCTL_INT(_net_inet_ip, IPCTL_DEFMTU, mtu, CTLFLAG_RW,
           &ip_mtu, 0, "Default MTU");
#endif

/*
 * We need to save the IP options in case a protocol wants to respond
 * to an incoming packet over the same route if the packet got here
 * using IP source routing.  This allows connection establishment and
 * maintenance when the remote end is on a network that is not known
 * to us.
 */
static int  ip_nhops = 0;
static  struct ip_srcrt
{
  struct  in_addr dst;      /* final destination */
  char  nop;        /* one NOP to align */
  char  srcopt[IPOPT_OFFSET + 1]; /* OPTVAL, OLEN and OFFSET */
  struct  in_addr route[MAX_IPOPTLEN/sizeof(struct in_addr)];
} ip_srcrt;

static void save_rte (unsigned char *, struct in_addr);
static int  ip_dooptions (struct rtmbuf *);
static void ip_forward (struct rtmbuf *, int);

static struct sockaddr_in ipaddr = { sizeof(ipaddr), AF_INET};

extern L7_uint32 ipMapTraceFlags;
extern L7_uint32 ipMapTraceIntf;
#ifndef L7_ORIGINAL_VENDOR_CODE
extern ipMapDebugIcmpCtlStats_t icmpCtlStats;
#endif

void rtipStatsReset(void)
{
  memset(&L7_ipstat, 0, sizeof(L7_ipstat));
}

void rtipStatsShow(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\ntotal packets received: %lu", L7_ipstat.ips_total); 
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nchecksum bad: %lu", L7_ipstat.ips_badsum); 
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\npacket too short: %lu", L7_ipstat.ips_tooshort);   
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nnot enough data: %lu", L7_ipstat.ips_toosmall);   
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nip header length < data size: %lu", L7_ipstat.ips_badhlen);    
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nip length < ip header length: %lu", L7_ipstat.ips_badlen);   
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nInvalid source IP address: %lu", L7_ipstat.ips_badsa);   
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nfragments received: %lu", L7_ipstat.ips_fragments);  
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nfrags dropped (dups, out of space): %lu", L7_ipstat.ips_fragdropped);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nfragments timed out: %lu", L7_ipstat.ips_fragtimeout);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\npackets forwarded: %lu", L7_ipstat.ips_forward);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\npackets fast forwarded: %lu", L7_ipstat.ips_fastforward);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\npackets rcvd for unreachable dest: %lu", L7_ipstat.ips_cantforward);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\npackets forwarded on same net: %lu", L7_ipstat.ips_redirectsent);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nunknown or unsupported protocol: %lu", L7_ipstat.ips_noproto);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\ndatagrams delivered to upper level: %lu", L7_ipstat.ips_delivered);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\ntotal ip packets generated here: %lu", L7_ipstat.ips_localout);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nlost packets due to nobufs, etc.: %lu", L7_ipstat.ips_odropped);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\ntotal packets reassembled ok: %lu", L7_ipstat.ips_reassembled);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\ndatagrams successfully fragmented: %lu", L7_ipstat.ips_fragmented);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\noutput fragments created: %lu", L7_ipstat.ips_ofragments);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\ndon't fragment flag was set, etc.: %lu", L7_ipstat.ips_cantfrag);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nerror in option processing: %lu", L7_ipstat.ips_badoptions);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\npackets discarded due to no route: %lu", L7_ipstat.ips_noroute);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nip version != 4: %lu", L7_ipstat.ips_badvers);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\ntotal raw ip packets generated: %lu", L7_ipstat.ips_rawout);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nip length > max ip packet size: %lu", L7_ipstat.ips_toolong);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nmulticasts for unregistered grps: %lu", L7_ipstat.ips_notmember);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nno match gif found: %lu", L7_ipstat.ips_nogif);
}

/*static struct route ipforward_rt; */

/*
 * Ip input routine.  Checksum and byte swap header.  If fragmented
 * try to reassemble.  Process options.  Pass to next level.
 */
void
ip_input(struct rtmbuf *m)
{
  struct ip *ip;
  int    hlen;
  u_short sum;
  int    intIfNum;
  int    tmp_ip_len;
  sysnet_pdu_info_t pduInfo;
  SYSNET_PDU_RC_t hookVerdict;
  L7_uint32 netDirBcastIntfAddr;
  L7_uint32 destIntIfNum; /* intIfNum of interface where destAddr configured */
  L7_uint32 destAddr;     /* destination IP addr in host byte order */
  L7_uchar8 pMacAddr[L7_MAC_ADDR_LEN];
  L7_BOOL vrrpEnabled;

#ifdef  DIAGNOSTIC
  if (m == NULL || (m->rtm_flags & M_PKTHDR) == 0)
    panic("ip_input no HDR");
#endif
  L7_ipstat.ips_total++;

  if (m->rtm_pkthdr.len < sizeof(struct ip))
    goto tooshort;

  if (m->rtm_len < sizeof (struct ip))
  {
    L7_ipstat.ips_toosmall++;
    SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
    rtm_freem(m);
    return;
  }
  ip = rtmtod(m, struct ip *);

  if (IP_VHL_V(ip->ip_vhl) != IPVERSION)
  {
    L7_ipstat.ips_badvers++;
    goto bad;
  }

  hlen = IP_VHL_HL(ip->ip_vhl) << 2;
  if (hlen < sizeof(struct ip))
  { /* minimum header length */
    L7_ipstat.ips_badhlen++;
    goto bad;
  }

  /* lvl7_@ip_forward start*/
  if (hlen > m->rtm_len)
  {
    L7_ipstat.ips_badhlen++;
    goto bad;
  }
  if (m->rtm_pkthdr.csum_flags & CSUM_IP_CHECKED)
  {
    sum = !(m->rtm_pkthdr.csum_flags & CSUM_IP_VALID);
  }
  else
  {
    sum = rtin_cksum(m, hlen);
  }
  /* lvl7_@ip_forward end*/

  if (sum)
  {
    L7_ipstat.ips_badsum++;
    goto bad;
  }

  /*
   * Convert fields to host representation.
   */
#ifndef L7_ORIGINAL_VENDOR_CODE
  /* Since we are going to pass this packet to the IP stack in the kernel,
   * we should not modify the byte-order in the packet.  Use a
   * temporary variable.  Instead of tmp_ip_len, original code was ip->ip_len.
   */
  tmp_ip_len = osapiNtohs(ip->ip_len);
#else
  NTOHS(ip->ip_len);
#endif
  if (tmp_ip_len < hlen)
  {
    L7_ipstat.ips_badlen++;
    goto bad;
  }
#ifdef L7_ORIGINAL_VENDOR_CODE
  NTOHS(ip->ip_off);
#endif

  /*
   * Check that the amount of data in the buffers
   * is as at least much as the IP header would have us expect.
   * Trim mbufs if longer than we expect.
   * Drop packet if shorter than we expect.
   */
  if (m->rtm_pkthdr.len < tmp_ip_len)
  {
    tooshort:
    L7_ipstat.ips_tooshort++;
    goto bad;
  }
  if (m->rtm_pkthdr.len > tmp_ip_len)
  {
    if (m->rtm_len == m->rtm_pkthdr.len)
    {
      m->rtm_len = tmp_ip_len;
      m->rtm_pkthdr.len = tmp_ip_len;
    }
    else
      rtm_adj(m, tmp_ip_len - m->rtm_pkthdr.len);
  }

  if ((ipMapTraceFlags & IPMAP_TRACE_FORWARDING) &&
      (!ipMapTraceIntf || (ipMapTraceIntf == m->rtm_pkthdr.rcvif->if_index)))
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    L7_uchar8 srcStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(ntohl(ip->ip_src.s_addr), srcStr);
    osapiInetNtoa(ntohl(ip->ip_dst.s_addr), destStr);
    sprintf(traceBuf, "ip_input():  ver/hlen %#4.2x, tos %d, len %d, id %d, flags/offset "
            "%#4.2x, ttl %d, proto %d, src %s, dst %s\n", 
            ip->ip_vhl, ip->ip_tos, ntohs(ip->ip_len), ntohs(ip->ip_id), ip->ip_off, ip->ip_ttl,
            ip->ip_p, srcStr, destStr);
    ipMapTraceWrite(traceBuf);
  }

  /* Source Address Checking */
  /* lvl7_@ip_forward start*/

  if (ipMapIpSourceCheckingGet() == L7_ENABLE)
  {
    if (ip->ip_src.s_addr == INADDR_ANY)
    {
      L7_ipstat.ips_badsa++;
      goto bad;
    }
  }
#ifndef L7_ORIGINAL_VENDOR_CODE
  if (osapiNtohl(ip->ip_src.s_addr) >= INADDR_UNSPEC_GROUP)  /* i.e., all class D, E, and beyond */
#else /* L7_ORIGINAL_VENDOR_CODE */
  if (ntohl(ip->ip_src.s_addr) >= INADDR_UNSPEC_GROUP)  /* i.e., all class D, E, and beyond */
#endif /* L7_ORIGINAL_VENDOR_CODE */
  {
    L7_ipstat.ips_badsa++;
    goto bad;
  }
#ifndef L7_ORIGINAL_VENDOR_CODE
  if ((osapiNtohl(ip->ip_src.s_addr) & 0xff000000) == (IN_LOOPBACKNET << IN_CLASSA_NSHIFT))
#else /* L7_ORIGINAL_VENDOR_CODE */
  if ((ntohl(ip->ip_src.s_addr) & 0xff000000) == (IN_LOOPBACKNET << IN_CLASSA_NSHIFT))
#endif /* L7_ORIGINAL_VENDOR_CODE */
  {
    L7_ipstat.ips_badsa++;
    goto bad;
  }
  /* Compare net-directed broadcast address if netmask != 0xFFFFFFFF */
  if ((~rtGetSubnetMaskOfIntf(m->rtm_pkthdr.rcvif->if_index)) && rtGetNetDirBcastAddrOfIntf(m->rtm_pkthdr.rcvif->if_index, &netDirBcastIntfAddr) == L7_SUCCESS )
  {
#ifndef L7_ORIGINAL_VENDOR_CODE
    if (osapiNtohl(ip->ip_src.s_addr) == netDirBcastIntfAddr)
#else /* L7_ORIGINAL_VENDOR_CODE */
    if (ntohl(ip->ip_src.s_addr) == netDirBcastIntfAddr)
#endif /* L7_ORIGINAL_VENDOR_CODE */
    {
      L7_ipstat.ips_badsa++;
      goto bad;
    }
  }
  /* lvl7_@ip_forward end*/

  /*
   * Don't accept packets with a loopback destination address
   * unless they arrived via the loopback interface.
   */
#ifndef L7_ORIGINAL_VENDOR_CODE
  if ((osapiNtohl(ip->ip_dst.s_addr) & IN_CLASSA_NET) ==
#else /* L7_ORIGINAL_VENDOR_CODE */
  if ((ntohl(ip->ip_dst.s_addr) & IN_CLASSA_NET) ==
#endif /* L7_ORIGINAL_VENDOR_CODE */
      (IN_LOOPBACKNET << IN_CLASSA_NSHIFT) &&
      (m->rtm_pkthdr.rcvif->if_flags & IFF_LOOPBACK) == 0)
  {
    goto bad;
  }

  /* Call interceptors who are interested in valid IP frames.  If L7_TRUE is returned,
  ** the frame was either discarded or consumed, which means that the network buffer
  ** has been freed by the intercept call, or has been copied by the consumer.
  **
  ** Note that for this intercept point, the buffer is referenced through the rtmbuf which
  ** is freed here if the caller consumes it.  So, the interceptor should make a copy of the
  ** frame if processing is done on another task.
  */
  memset(&pduInfo, 0, sizeof(sysnet_pdu_info_t));
  pduInfo.intIfNum = m->rtm_pkthdr.rcvif->if_index;

  if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_VALID_IN, (L7_netBufHandle)m->rtm_bufhandle,
                           &pduInfo, L7_NULLPTR, &hookVerdict) == L7_TRUE)
  {
    /* If freed by intercept call, don't free it again */
    if (hookVerdict != SYSNET_PDU_RC_DISCARD)
    {
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
    }
      rtm_freem(m);
      if (hookVerdict == SYSNET_PDU_RC_DISCARD)
        SYSAPI_IP_STATS_INCREMENT(FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
                                  L7_PLATFORM_CTR_RX_IP_IN_DISCARDS);
      return;
    }

  /*
   * Process options and, if not destined for us,
   * ship it on.  ip_dooptions returns 1 when an
   * error was detected (causing an icmp message
   * to be sent and the original packet to be freed).
   */
  ip_nhops = 0;   /* for source routed packets */

  if (hlen > sizeof (struct ip) && ip_dooptions(m))
  {
    return;
  }

  /* greedy RSVP, snatches any PATH packet of the RSVP protocol and no
   * matter if it is destined to another node, or whether it is
   * a multicast one, RSVP wants it! and prevents it from being forwarded
   * anywhere else. Also checks if the rsvp daemon is running before
* grabbing the packet.
   */
  if (rsvp_on && ip->ip_p==IPPROTO_RSVP)
    goto ours;

  destAddr = osapiNtohl(ip->ip_dst.s_addr);

  if (!IN_MULTICAST(destAddr))
  {
    /* See if destination IP address is ours. */
    if (ipMapIpAddressToIntf(destAddr, &destIntIfNum) == L7_SUCCESS)
    {
      /* don't require packet to have arrived on interface where destination
       * address is configured.  */
      goto ours;
    }
  
    /* See if destination IP address is the net directed broadcast address
     * of the receive interface. If so, consume locally. */
    if (ipMapRouterIfResolve(destAddr, &destIntIfNum) == L7_SUCCESS)
    {
      if ((destIntIfNum == pduInfo.intIfNum) &&
          (ipMapNetDirBcastMatchCheck(destIntIfNum, destAddr, pMacAddr) == L7_SUCCESS))
      {
        goto ours;
      }
    }
  }
  else
  {
    /* multicast destination address */
    /* Don't forward a packet with time-to-live of zero or one,
       or a packet destined to a local-only group. */

#ifdef L7_MCAST_PACKAGE
  if (destAddr > INADDR_MAX_LOCAL_GROUP) {
    /*
     * All incoming multicast packets are passed to the
     * kernel-level multicast forwarding function.
     * The packet is returned (relatively) intact; if
     * ip_mforward() returns a non-zero value, the packet
     * must be discarded, else it may be accepted below.
     */

      if (mfcIpv4PktRecv(ip, m->rtm_pkthdr.rcvif, m, 0) != L7_FALSE) {
          L7_ipstat.ips_cantforward++;
          SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
          rtm_freem(m);
          return;
      }
  }
#endif /* L7_MCAST_PACKAGE */

      /*
       * The process-level routing demon needs to receive
       * all multicast IGMP packets, whether or not this
       * host belongs to their destination groups.
       */
      if (ip->ip_p == IPPROTO_IGMP)
        goto ours;
      L7_ipstat.ips_forward++;

    goto ours;
  }
#ifndef L7_ORIGINAL_VENDOR_CODE
  if (ip->ip_dst.s_addr == osapiHtonl((u_long)INADDR_BROADCAST))
#else /* L7_ORIGINAL_VENDOR_CODE */
  if (ip->ip_dst.s_addr == (u_long)INADDR_BROADCAST)
#endif /* L7_ORIGINAL_VENDOR_CODE */
    goto ours;
#ifndef L7_ORIGINAL_VENDOR_CODE
  if (ip->ip_dst.s_addr == osapiHtonl(INADDR_ANY))
#else /* L7_ORIGINAL_VENDOR_CODE */
  if (ip->ip_dst.s_addr == INADDR_ANY)
#endif /* L7_ORIGINAL_VENDOR_CODE */
    goto ours;

  /* If packet is an Echo Request to one of our VRRP addresses, and 
   * this router is the master, accept the packet as ours. Note that if this
   * router is the IP address owner, we would have already found a matching
   * local interface address above. */
  if ((vrrpFuncTable.L7_vrrpAdminModeGet &&
       (vrrpFuncTable.L7_vrrpAdminModeGet(&vrrpEnabled) == L7_SUCCESS)) &&
      (vrrpEnabled == L7_ENABLE))
  {
    L7_uchar8 *data;
    L7_enetHeader_t *ethHeader;
    L7_uchar8 vrid;
    L7_vrrpState_t vrrpStatus;
    
    /* Use dest MAC to determine if sent to a VRRP IP address */
    SYSAPI_NET_MBUF_GET_DATASTART(m->rtm_bufhandle, data);
    ethHeader = (L7_enetHeader_t *) data;
    if ((ethHeader->dest.addr[0] == 0x00) && 
        (ethHeader->dest.addr[1] == 0x00) && 
        (ethHeader->dest.addr[2] == 0x5e) &&
        (ethHeader->dest.addr[3] == 0x00) && 
        (ethHeader->dest.addr[4] == 0x01))
    {
      /* Packet's destination is a VRRP address */
      vrid = ethHeader->dest.addr[5];
      
      /*  we simply let through all packets with VRRP dest MAC, assuming
       * vrrpIpFrameFilter() weeded out the ones we don't want. */
      if ((vrrpFuncTable.L7_vrrpStateGet &&
           (vrrpFuncTable.L7_vrrpStateGet(vrid, destIntIfNum, &vrrpStatus) == L7_SUCCESS)) &&
          (vrrpStatus == L7_VRRP_STATE_MASTER))            
      {
        /* Packet's destination IP address matches VRRP IP address of destination
         * virtual router, and this router is the Master. */
        goto ours;
      }
    }
  }

  /*
   * Not for us; forward if possible and desirable.
   */
  if (rtipforwarding == 0)
  {
    L7_ipstat.ips_cantforward++;
    SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
    rtm_freem(m);

  }
  else
    ip_forward(m, 0);
  return;

  ours:

  intIfNum = m->rtm_pkthdr.rcvif->if_index;
  if (ipmRecvLocal((L7_netBufHandle)m->rtm_bufhandle,intIfNum) != L7_SUCCESS)
  {
    SYSAPI_IP_STATS_INCREMENT(FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
                              L7_PLATFORM_CTR_RX_IP_IN_DISCARDS);
  }

  /* The corresponding sysapi network buffer (m->rtm_bufhandle) does not need to be
  ** freed here since it has has already been freed by a hook or the local IP stack's
  ** wrapper function.
   */
  rtm_freem(m);
    return;

  bad:
  SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
  rtm_freem(m);

}

/*
 * Do option processing on a datagram,
 * possibly discarding it if bad options are encountered,
 * or forwarding it if source-routed.
 * Returns 1 if packet has been forwarded/freed,
 * 0 if the packet should be processed further.
 */
static int
ip_dooptions(m)
struct rtmbuf *m;
{
  register struct ip *ip = rtmtod(m, struct ip *);
  register u_char *cp;
  register struct ip_timestamp *ipt;
  int opt, optlen, cnt, off, code, type = ICMP_PARAMPROB, forward = 0;
  struct in_addr *sin, dst;
  struct in_addr routerIP;
  n_time ntime;
  L7_uint32 matchRoute, MyLocalIfIndex, nexthopIP = 0;
  struct rtmbuf *mcopy = NULL ;
  L7_uchar8  *buffer_addr;
  L7_uint32 buffer_pool_id = rtMemPoolIdGet();
  L7_RC_t rc;
  L7_uint32 pktOurs;
  L7_uint32 icmpUnreachablesMode = L7_DISABLE;

  /* get unreachables mode on recv if */
  ipMapRtrICMPUnreachablesModeGet(m->rtm_pkthdr.rcvif->if_index, &icmpUnreachablesMode);

  dst = ip->ip_dst;
  cp = (u_char *)(ip + 1);
  cnt = (IP_VHL_HL(ip->ip_vhl) << 2) - sizeof (struct ip);
  for (; cnt > 0; cnt -= optlen, cp += optlen)
  {
    opt = cp[IPOPT_OPTVAL];
    if (opt == IPOPT_EOL)
      break;
    if (opt == IPOPT_NOP)
      optlen = 1;
    else
    {
      if (cnt < IPOPT_OLEN + sizeof(*cp))
      {
        code = &cp[IPOPT_OLEN] - (u_char *)ip;
        goto bad;
      }
      optlen = cp[IPOPT_OLEN];
      if (optlen < IPOPT_OLEN + sizeof(*cp) || optlen > cnt)
      {
        code = &cp[IPOPT_OLEN] - (u_char *)ip;
        goto bad;
      }
    }
    switch (opt)
    {

    default:
      break;

      /*
       * Source routing with record.
       * Find interface with current destination address.
       * If none on this machine then drop if strictly routed,
       * or do nothing if loosely routed.
       * Record interface address and bring up next address
       * component.  If strictly routed make sure next
       * address is on directly accessible net.
       */
    case IPOPT_LSRR:
    case IPOPT_SSRR:
      if ((off = cp[IPOPT_OFFSET]) < IPOPT_MINOFF)
      {
        code = &cp[IPOPT_OFFSET] - (u_char *)ip;
        goto bad;
      }
      ipaddr.sin_addr = ip->ip_dst;

      /* lvl7_@ip_forward start*/
      MyLocalIfIndex = 0;
#ifndef L7_ORIGINAL_VENDOR_CODE
      rtCheckOurIntf(&MyLocalIfIndex, osapiNtohl(ipaddr.sin_addr.s_addr));
#else /* L7_ORIGINAL_VENDOR_CODE */
      rtCheckOurIntf(&MyLocalIfIndex, ntohl(ipaddr.sin_addr.s_addr));
#endif /* L7_ORIGINAL_VENDOR_CODE */
      if (!MyLocalIfIndex)
      {
        /* lvl7_@ip_forward end*/
        if (opt == IPOPT_SSRR)
        {
          type = ICMP_UNREACH;
          code = ICMP_UNREACH_SRCFAIL;
          goto bad;
        }
        if (!ip_dosourceroute)
          goto nosourcerouting;
        /*
         * Loose routing, and not at next destination
         * yet; nothing to do except forward.
         */
        break;
      }

      off--;      /* 0 origin */
#ifdef LVL7_ORIGINAL_VENDOR_CODE
#else
      pktOurs = 0;
      do
      {
        if (off > optlen - (int)sizeof(struct in_addr))
        {
           /* End of source route.  Should be for us.*/
          if (!ip_acceptsourceroute)
            goto nosourcerouting;
          save_rte(cp, ip->ip_src);
          pktOurs = 1;
          break;
        }
        /*
         * locate outgoing interface
         */
        (void)memcpy(&ipaddr.sin_addr, cp + off,
                     sizeof(ipaddr.sin_addr));
        MyLocalIfIndex = 0;
        rtCheckOurIntf(&MyLocalIfIndex, osapiNtohl(ipaddr.sin_addr.s_addr));
        if (!MyLocalIfIndex)
        {
          if (!ip_dosourceroute)
            goto nosourcerouting;
          /* forward the packet*/
          break;
        }
        /* Get the next ip address and check if it is one of our intf addr */
        off += 4;
      }while (1);

      if (pktOurs == 1)
        break;
#endif   /* LVL7_ORIGINAL_VENDOR_CODE */

      if (!ip_dosourceroute)
      {
        if (rtipforwarding)
        {
          char buf[OSAPI_INET_NTOA_BUF_SIZE]; /* aaa.bbb.ccc.ddd\0 */
          /*
           * Acting as a router, so generate ICMP
           */
          nosourcerouting:

          memset(buf, 0, OSAPI_INET_NTOA_BUF_SIZE);
#ifndef L7_ORIGINAL_VENDOR_CODE
          osapiInetNtoa(osapiNtohl(ip->ip_dst.s_addr), buf);
#else /* L7_ORIGINAL_VENDOR_CODE */
          osapiInetNtoa(ip->ip_dst.s_addr, buf);
#endif /* L7_ORIGINAL_VENDOR_CODE */

          type = ICMP_UNREACH;
          code = ICMP_UNREACH_SRCFAIL;
          goto bad;
        }
        else
        {
          /*
           * Not acting as a router, so silently drop.
           */
          L7_ipstat.ips_cantforward++;
          SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
          rtm_freem(m);
          return(1);
        }
      }

      /*
       * locate outgoing interface
       */
      (void)memcpy(&ipaddr.sin_addr, cp + off,
                   sizeof(ipaddr.sin_addr));

      /* lvl7_@ip_forward start*/
      matchRoute = IP_NONLOCAL_ROUTE;
      MyLocalIfIndex = 0;    /* This will be an internal interface number */
#ifndef L7_ORIGINAL_VENDOR_CODE
      matchRoute = rtFindRoute(&MyLocalIfIndex, &nexthopIP,
                                 osapiNtohl(ipaddr.sin_addr.s_addr),
                                 osapiNtohl(ip->ip_src.s_addr));
#else /* L7_ORIGINAL_VENDOR_CODE */
      matchRoute = rtFindRoute(&MyLocalIfIndex, &nexthopIP,
                                 ntohl(ipaddr.sin_addr.s_addr),
                                 ntohl(ip->ip_src.s_addr));
#endif /* L7_ORIGINAL_VENDOR_CODE */
      memset(&routerIP,0,sizeof(routerIP));
      /* lvl7_@ip_forward end*/

      if (opt == IPOPT_SSRR)
      {
        /* lvl7_@ip_forward start*/
#ifndef L7_ORIGINAL_VENDOR_CODE
        if (matchRoute == IP_LOCAL_ROUTE)
          routerIP.s_addr = osapiHtonl(rtGetIPAddrOfIntf(MyLocalIfIndex));
#else /* L7_ORIGINAL_VENDOR_CODE */
        if (matchRoute == IP_LOCAL_ROUTE)
          routerIP.s_addr = htonl(rtGetIPAddrOfIntf(MyLocalIfIndex));
#endif /* L7_ORIGINAL_VENDOR_CODE */
      }
      else
      {
#ifndef L7_ORIGINAL_VENDOR_CODE
        if (MyLocalIfIndex)
          routerIP.s_addr = osapiHtonl(rtGetIPAddrOfIntf(MyLocalIfIndex));
#else /* L7_ORIGINAL_VENDOR_CODE */
        if (MyLocalIfIndex)
          routerIP.s_addr = htonl(rtGetIPAddrOfIntf(MyLocalIfIndex));
#endif /* L7_ORIGINAL_VENDOR_CODE */
        /* lvl7_@ip_forward end*/
      }

      /* lvl7_@ip_forward start*/
      if (routerIP.s_addr == 0)
      {
        /* lvl7_@ip_forward end*/

        type = ICMP_UNREACH;
        code = ICMP_UNREACH_HOST;
        goto bad;
      }

      ip->ip_dst = ipaddr.sin_addr;

      /* lvl7_@ip_forward start*/
      (void)memcpy(cp + off, &routerIP,
                   sizeof(struct in_addr));
      /* lvl7_@ip_forward end*/

#ifdef LVL7_ORIGINAL_VENDOR_CODE
      cp[IPOPT_OFFSET] += sizeof(struct in_addr);
#else
      cp[IPOPT_OFFSET] = off + 1 + sizeof(struct in_addr);
#endif /*LVL7_ORIGINAL_VENDOR_CODE */

      /*
       * Let ip_intr's mcast routing check handle mcast pkts
       */
#ifndef L7_ORIGINAL_VENDOR_CODE
      forward = !IN_MULTICAST(osapiNtohl(ip->ip_dst.s_addr));
#else /* L7_ORIGINAL_VENDOR_CODE */
      forward = !IN_MULTICAST(ntohl(ip->ip_dst.s_addr));
#endif /* L7_ORIGINAL_VENDOR_CODE */
      break;

    case IPOPT_RR:
      if (optlen < IPOPT_OFFSET + sizeof(*cp))
      {
        code = &cp[IPOPT_OFFSET] - (u_char *)ip;
        goto bad;
      }
      if ((off = cp[IPOPT_OFFSET]) < IPOPT_MINOFF)
      {
        code = &cp[IPOPT_OFFSET] - (u_char *)ip;
        goto bad;
      }
      /*
       * If no space remains, ignore.
       */
      off--;      /* 0 origin */
      if (off > optlen - (int)sizeof(struct in_addr))
        break;
      (void)memcpy(&ipaddr.sin_addr, &ip->ip_dst,
                   sizeof(ipaddr.sin_addr));
      /*
       * locate outgoing interface; if we're the destination,
       * use the incoming interface (should be same).
       */
      /* lvl7_@ip_forward start*/
      MyLocalIfIndex = 0;
#ifndef L7_ORIGINAL_VENDOR_CODE
      rtCheckOurIntf(&MyLocalIfIndex, osapiNtohl(ipaddr.sin_addr.s_addr));
#else /* L7_ORIGINAL_VENDOR_CODE */
      rtCheckOurIntf(&MyLocalIfIndex, ntohl(ipaddr.sin_addr.s_addr));
#endif /* L7_ORIGINAL_VENDOR_CODE */
      memset(&routerIP,0,sizeof(routerIP));
      if (MyLocalIfIndex)
      {
        break;
      }
      else
      {
        matchRoute = IP_NONLOCAL_ROUTE;
        MyLocalIfIndex = 0;
#ifndef L7_ORIGINAL_VENDOR_CODE
        rtFindRoute(&MyLocalIfIndex, &nexthopIP, osapiNtohl(ipaddr.sin_addr.s_addr),
                    osapiNtohl(ip->ip_src.s_addr));
        if (MyLocalIfIndex)
          routerIP.s_addr = osapiHtonl(rtGetIPAddrOfIntf(MyLocalIfIndex));
#else /* L7_ORIGINAL_VENDOR_CODE */
        rtFindRoute(&MyLocalIfIndex, &nexthopIP, ntohl(ipaddr.sin_addr.s_addr),
                    ntohl(ip->ip_src.s_addr));
        if (MyLocalIfIndex)
          routerIP.s_addr = htonl(rtGetIPAddrOfIntf(MyLocalIfIndex));
#endif /* L7_ORIGINAL_VENDOR_CODE */
      }
      if (routerIP.s_addr == 0)
      {
        /* lvl7_@ip_forward end*/

        type = ICMP_UNREACH;
        code = ICMP_UNREACH_HOST;
        goto bad;
      }
      /* lvl7_@ip_forward start*/
      (void)memcpy(cp + off, &routerIP,
                   sizeof(struct in_addr));
      /* lvl7_@ip_forward end*/

      cp[IPOPT_OFFSET] += sizeof(struct in_addr);
      break;

    case IPOPT_TS:
      MyLocalIfIndex = 0;
#ifndef L7_ORIGINAL_VENDOR_CODE
      rtCheckOurIntf(&MyLocalIfIndex, osapiNtohl(ip->ip_dst.s_addr));
#else /* L7_ORIGINAL_VENDOR_CODE */
      rtCheckOurIntf(&MyLocalIfIndex, ntohl(ip->ip_dst.s_addr));
#endif /* L7_ORIGINAL_VENDOR_CODE */
      if (MyLocalIfIndex != 0)
      {

        break;

      }
      code = cp - (u_char *)ip;
      ipt = (struct ip_timestamp *)cp;
      if (ipt->ipt_len < 5)
        goto bad;
      if (ipt->ipt_ptr >
          ipt->ipt_len - (int)sizeof(int32_t))
      {
        if (++ipt->ipt_oflw == 0)
          goto bad;
        break;
      }
      sin = (struct in_addr *)(cp + ipt->ipt_ptr - 1);
      switch (ipt->ipt_flg)
      {

      case IPOPT_TS_TSONLY:
        break;

      case IPOPT_TS_TSANDADDR:
        if (ipt->ipt_ptr - 1 + sizeof(n_time) +
            sizeof(struct in_addr) > ipt->ipt_len)
          goto bad;
        ipaddr.sin_addr = dst;

        /* lvl7_@ip_forward start*/
        memset(&routerIP,0,sizeof(routerIP));
#ifndef L7_ORIGINAL_VENDOR_CODE
        routerIP.s_addr = osapiHtonl(rtGetIPAddrOfIntf(m->rtm_pkthdr.rcvif->if_index));
#else /* L7_ORIGINAL_VENDOR_CODE */
        routerIP.s_addr = htonl(rtGetIPAddrOfIntf(m->rtm_pkthdr.rcvif->if_index));
#endif /* L7_ORIGINAL_VENDOR_CODE */

        if (routerIP.s_addr == 0)
          /* lvl7_@ip_forward end*/

          continue;

        /* lvl7_@ip_forward start*/
        (void)memcpy(sin, &routerIP,
                     sizeof(struct in_addr));
        /* lvl7_@ip_forward end*/

        ipt->ipt_ptr += sizeof(struct in_addr);
        break;

      case IPOPT_TS_PRESPEC:
        if (ipt->ipt_ptr - 1 + sizeof(n_time) +
            sizeof(struct in_addr) > ipt->ipt_len)
          goto bad;
        (void)memcpy(&ipaddr.sin_addr, sin,
                     sizeof(struct in_addr));

        /* lvl7_@ip_forward start*/
        MyLocalIfIndex = 0;
#ifndef L7_ORIGINAL_VENDOR_CODE
        rtCheckOurIntf(&MyLocalIfIndex, osapiNtohl(ipaddr.sin_addr.s_addr));
#else /* L7_ORIGINAL_VENDOR_CODE */
        rtCheckOurIntf(&MyLocalIfIndex, ntohl(ipaddr.sin_addr.s_addr));
#endif /* L7_ORIGINAL_VENDOR_CODE */
        if (!MyLocalIfIndex)
          /* lvl7_@ip_forward end*/

          continue;
        ipt->ipt_ptr += sizeof(struct in_addr);
        break;

      default:
        goto bad;
      }
      ntime = rtiptime();
      (void)memcpy(cp + ipt->ipt_ptr - 1, &ntime,
                   sizeof(n_time));
      ipt->ipt_ptr += sizeof(n_time);
    }
  }

  if (forward && rtipforwarding)
  {
    ip_forward(m, 1);
    return(1);
  }
  return(0);

  bad:

  L7_ipstat.ips_badoptions++;

  /* check unreachables mode */
  if(L7_ENABLE == icmpUnreachablesMode && ICMP_UNREACH != type)
  {
    mcopy = 0;
    rc = bufferPoolAllocate (buffer_pool_id, &buffer_addr);
    if (rc == L7_SUCCESS)
    {
      mcopy = (struct rtmbuf *) buffer_addr;
      memset(mcopy, 0, sizeof( *mcopy));
      mcopy->rtm_pkthdr.rcvif = m->rtm_pkthdr.rcvif;
      rtcreateIcmpErrPckt(m, mcopy);
      icmp_error(mcopy, type, code, 0, 0);
    }
  }

  SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
  rtm_freem(m);
  return(1);
}

/*
 * Save incoming source route for use in replies,
 * to be picked up later by ip_srcroute if the receiver is interested.
 */
void
save_rte(option, dst)
u_char *option;
struct in_addr dst;
{
  unsigned olen;

  olen = option[IPOPT_OLEN];
#ifdef DIAGNOSTIC
  if (ipprintfs)
    printf("save_rte: olen %d\n", olen);
#endif
  if (olen > sizeof(ip_srcrt) - (1 + sizeof(dst)))
    return;
  bcopy(option, ip_srcrt.srcopt, olen);
  ip_nhops = (olen - IPOPT_OFFSET - 1) / sizeof(struct in_addr);
  ip_srcrt.dst = dst;
}

/*
 * Retrieve incoming source route for use in replies,
 * in the same form used by setsockopt.
 * The first hop is placed before the options, will be removed later.
 */
struct rtmbuf *
rtip_srcroute()
{
  register struct in_addr *p, *q;
  register struct rtmbuf *m;

  if (ip_nhops == 0)
    return((struct rtmbuf *)0);
  m = rtm_getm(M_DONTWAIT, MT_HEADER);
  if (m == 0)
    return((struct rtmbuf *)0);

#define OPTSIZ  (sizeof(ip_srcrt.nop) + sizeof(ip_srcrt.srcopt))

  m->rtm_len = ip_nhops * sizeof(struct in_addr) + sizeof(struct in_addr) +
               OPTSIZ;
#ifdef DIAGNOSTIC
  if (ipprintfs)
    printf("rtip_srcroute: nhops %d mlen %d", ip_nhops, m->rtm_len);
#endif

  /*
   * First save first hop for return route
   */
  p = &ip_srcrt.route[ip_nhops - 1];
  *(rtmtod(m, struct in_addr *)) = *p--;
#ifdef DIAGNOSTIC
  if (ipprintfs)
#ifndef L7_ORIGINAL_VENDOR_CODE
    printf(" hops %lx", (u_long)osapiNtohl(rtmtod(m, struct in_addr *)->s_addr));
#else /* L7_ORIGINAL_VENDOR_CODE */
    printf(" hops %lx", (u_long)ntohl(rtmtod(m, struct in_addr *)->s_addr));
#endif /* L7_ORIGINAL_VENDOR_CODE */
#endif

  /*
   * Copy option fields and padding (nop) to mbuf.
   */
  ip_srcrt.nop = IPOPT_NOP;
  ip_srcrt.srcopt[IPOPT_OFFSET] = IPOPT_MINOFF;
  (void)memcpy(rtmtod(m, caddr_t) + sizeof(struct in_addr),
               &ip_srcrt.nop, OPTSIZ);
  q = (struct in_addr *)(rtmtod(m, caddr_t) +
                         sizeof(struct in_addr) + OPTSIZ);
#undef OPTSIZ

  /*
   * Record return path as an IP source route,
   * reversing the path (pointers are now aligned).
   */
  while (p >= ip_srcrt.route)
  {
#ifdef DIAGNOSTIC
    if (ipprintfs)
#ifndef L7_ORIGINAL_VENDOR_CODE
      printf(" %lx", (u_long)osapiNtohl(q->s_addr));
#else /* L7_ORIGINAL_VENDOR_CODE */
      printf(" %lx", (u_long)ntohl(q->s_addr));
#endif /* L7_ORIGINAL_VENDOR_CODE */
#endif
    *q++ = *p--;
  }
  /*
   * Last hop goes to final destination.
   */
  *q = ip_srcrt.dst;
#ifdef DIAGNOSTIC
  if (ipprintfs)
#ifndef L7_ORIGINAL_VENDOR_CODE
    printf(" %lx\n", (u_long)osapiNtohl(q->s_addr));
#else /* L7_ORIGINAL_VENDOR_CODE */
    printf(" %lx\n", (u_long)ntohl(q->s_addr));
#endif /* L7_ORIGINAL_VENDOR_CODE */
#endif
  return(m);
}

/*
 * Strip out IP options, at higher
 * level protocol in the kernel.
 * Second argument is buffer to which options
 * will be moved, and return value is their length.
 * XXX should be deleted; last arg currently ignored.
 */
void
rtip_stripoptions(m, mopt)
register struct rtmbuf *m;
struct rtmbuf *mopt;
{
  register int i;
  struct ip *ip = rtmtod(m, struct ip *);
  register caddr_t opts;
  int olen;

  olen = (IP_VHL_HL(ip->ip_vhl) << 2) - sizeof (struct ip);
  opts = (caddr_t)(ip + 1);
  i = m->rtm_len - (sizeof (struct ip) + olen);
  bcopy(opts + olen, opts, (unsigned)i);
  m->rtm_len -= olen;
  if (m->rtm_flags & M_PKTHDR)
    m->rtm_pkthdr.len -= olen;
  ip->ip_vhl = IP_MAKE_VHL(IPVERSION, sizeof(struct ip) >> 2);
}

u_char rtinetctlerrmap[PRC_NCMDS] = {
  0,    0,    0,    0,
  0,    EMSGSIZE, EHOSTDOWN,  EHOSTUNREACH,
  EHOSTUNREACH, EHOSTUNREACH, ECONNREFUSED, ECONNREFUSED,
  EMSGSIZE, EHOSTUNREACH, 0,    0,
  0,    0,    0,    0,
  ENOPROTOOPT,  ENETRESET
};

/*
 * Forward a packet.  If some error occurs return the sender
 * an icmp packet.  Note we can't always generate a meaningful
 * icmp message because icmp doesn't have a large enough repertoire
 * of codes and types.
 *
 * If not forwarding, just drop the packet.  This could be confusing
 * if ipforwarding was zero but some routing protocol was advancing
 * us as a gateway to somewhere.  However, we must let the routing
 * protocol deal with that.
 *
 * The srcrt parameter indicates whether the packet is being forwarded
 * via a source route.
 */
static void
ip_forward(m, srcrt)
struct rtmbuf *m;
int srcrt;
{
  register struct ip *ip = rtmtod(m, struct ip *);
  /*register struct sockaddr_in *sin;*/
  /*register struct rtentry *rt;*/
  int error, type = 0, code = 0;
  struct rtmbuf *mcopy = NULL ;
  unsigned long dest;
  struct rtm_ifnet destifp;

  /* lvl7_@ip_forward start*/
  L7_uint32 MyLocalIfIndex = 0;  /* intIfNum associated with next hop */
  L7_uint32 routerIP = 0;
  L7_uint32 icmpSendRedirectMode;
#ifndef L7_ORIGINAL_VENDOR_CODE
  L7_uint32 icmpIfSendRedirectMode = L7_ENABLE;
#endif
  L7_uint32 tosEnable;
  L7_uint32 tosvalue = 0;
  L7_uchar8  *buffer_addr;
  L7_uint32 buffer_pool_id = rtMemPoolIdGet();
  L7_RC_t rc;
  int netDirectBCastsEnable;
  L7_netBufHandle netMbufHandle = L7_NULL;
  L7_char8 *pdataStart;
  L7_char8 *olddataStart;
  L7_uint32 datalen;
  L7_uint32 netMask = 0;
  char rtetherbroadcastaddr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  L7_uint32 matchRoute;
  sysnet_pdu_info_t pduInfo;
  L7_uint32 icmpUnreachablesMode = L7_DISABLE;
  SYSNET_PDU_RC_t hookVerdict;
  L7_uint32 mtu_size = 0;

  /* lvl7_@ip_forward end*/

  /* get unreachables mode on recv if */
  ipMapRtrICMPUnreachablesModeGet(m->rtm_pkthdr.rcvif->if_index, &icmpUnreachablesMode);

  /* Call interceptors who are interested in received IP packets that are to be forwarded.
  ** If L7_TRUE is returned, the frame was either discarded or consumed, which means that
  ** the network buffer has been freed by the intercept call, or has been copied by the consumer.
  **
  ** Note that for this intercept point, the buffer is referenced through the rtmbuf which
  ** is freed here if the caller consumes it.  So, the interceptor should make a copy of the
  ** frame if processing is done on another task.
  */
  memset(&pduInfo, 0, sizeof(sysnet_pdu_info_t));
  pduInfo.intIfNum = m->rtm_pkthdr.rcvif->if_index;

  if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_FORWARD_IN, (L7_netBufHandle)m->rtm_bufhandle,
                           &pduInfo, L7_NULLPTR, &hookVerdict) == L7_TRUE)
  {
    /* If freed by intercept call, don't free it again */
    if (hookVerdict != SYSNET_PDU_RC_DISCARD)
    {
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
    }
    rtm_freem(m);
    return;
  }

  dest = 0;

  if (ipprintfs)
#ifndef L7_ORIGINAL_VENDOR_CODE
    IPMAP_TRACE("forward: src %lx dst %lx ttl %x\n",
           osapiNtohl((u_long)ip->ip_src.s_addr),
           osapiNtohl((u_long)ip->ip_dst.s_addr),
           ip->ip_ttl);
#else /* L7_ORIGINAL_VENDOR_CODE */
    IPMAP_TRACE("forward: src %lx dst %lx ttl %x\n",
           (u_long)ip->ip_src.s_addr, (u_long)ip->ip_dst.s_addr,
           ip->ip_ttl);
#endif /* L7_ORIGINAL_VENDOR_CODE */

  if ((ipMapTraceFlags & IPMAP_TRACE_FORWARDING) &&
      (!ipMapTraceIntf || (ipMapTraceIntf == pduInfo.intIfNum)))
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    L7_uchar8 srcStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(ntohl(ip->ip_src.s_addr), srcStr);
    osapiInetNtoa(ntohl(ip->ip_dst.s_addr), destStr);
    sprintf(traceBuf, "ip_forward():  src %s, dst %s, proto %d, ttl %d.\n", 
            srcStr, destStr, ip->ip_p, ip->ip_ttl);
    ipMapTraceWrite(traceBuf);
  }

  /* Type of pkts that cannot be forwarded :
     1. Multicast packets
     2. Pure broadcast pkts i.e. dst ip is 255.255.255.255 and mac is ff:ff:...
     3. rtin_canforward returns 0

     Note: For any pkt which has a mac ff:ff:ff:ff:ff:ff, the M_BCAST and M_MCAST
           bits are set in the flags. So we have to differentiate between
           multicast and broadcast packets. Broadcast packets can be
           1. pure broadcast pkts (see above)
           2. net directed broadcast packets
           3. broadcast mac with unicast ip pkt.
           Type 1 broadcast pkts are dropped in the condition below. Type 2 depends
           on the user configuration. Type 3 should also be dropped as per
           rfc 1122. Type 2 and type 3 differentiation is done in this function
           at a later stage.
  */
#ifndef L7_ORIGINAL_VENDOR_CODE
  if (((m->rtm_flags & M_MCAST) && ((m->rtm_flags & M_BCAST) != M_BCAST)) ||
      ((m->rtm_flags & M_BCAST) && osapiNtohl(ip->ip_dst.s_addr) == 0xffffffff) ||
      rtin_canforward(ip->ip_dst) == 0)
#else /* L7_ORIGINAL_VENDOR_CODE */
  if (((m->rtm_flags & M_MCAST) && ((m->rtm_flags & M_BCAST) != M_BCAST)) ||
      ((m->rtm_flags & M_BCAST) && ntohl(ip->ip_dst.s_addr) == 0xffffffff) ||
      rtin_canforward(ip->ip_dst) == 0)
#endif /* L7_ORIGINAL_VENDOR_CODE */
  {
    L7_ipstat.ips_cantforward++;
    SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
    rtm_freem(m);
    return;
  }

    if (ip->ip_ttl <= IPTTLDEC)
    {
      /* lvl7_@ip_forward start*/
      mcopy = 0;
      rc = bufferPoolAllocate (buffer_pool_id, &buffer_addr);
      if (rc == L7_SUCCESS)
      {
        mcopy = (struct rtmbuf *) buffer_addr;
        memset(mcopy, 0, sizeof( *mcopy));
        mcopy->rtm_pkthdr.rcvif = m->rtm_pkthdr.rcvif;
        rtcreateIcmpErrPckt(m, mcopy);
        icmp_error(mcopy, ICMP_TIMXCEED, ICMP_TIMXCEED_INTRANS, dest, 0);
      }

      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
      rtm_freem(m);
      /* lvl7_@ip_forward end*/
      return;
    }

  /* lvl7_@ip_forward start*/
  tosEnable = ipMapRtrTosForwardingModeGet();
  if (tosEnable == L7_ENABLE)
  {
    tosvalue = ip->ip_tos;
#ifndef L7_ORIGINAL_VENDOR_CODE
    matchRoute = rtFindRouteWithTOS(&MyLocalIfIndex, &routerIP,
                                      osapiNtohl(ip->ip_dst.s_addr),
                                      osapiNtohl(ip->ip_src.s_addr), tosvalue);
#else /* L7_ORIGINAL_VENDOR_CODE */
    matchRoute = rtFindRouteWithTOS(&MyLocalIfIndex, &routerIP,
                                      ntohl(ip->ip_dst.s_addr),
                                      ntohl(ip->ip_src.s_addr), tosvalue);
#endif /* L7_ORIGINAL_VENDOR_CODE */
  }
  else
#ifndef L7_ORIGINAL_VENDOR_CODE
    matchRoute = rtFindRoute(&MyLocalIfIndex, &routerIP,
                               osapiNtohl(ip->ip_dst.s_addr),
                               osapiNtohl(ip->ip_src.s_addr));
#else /* L7_ORIGINAL_VENDOR_CODE */
    matchRoute = rtFindRoute(&MyLocalIfIndex, &routerIP,
                               ntohl(ip->ip_dst.s_addr),
                               ntohl(ip->ip_src.s_addr));
#endif /* L7_ORIGINAL_VENDOR_CODE */

  if (routerIP == 0)
  {
    if(L7_ENABLE == icmpUnreachablesMode)
    {
      mcopy = 0;
      rc = bufferPoolAllocate (buffer_pool_id, &buffer_addr);
      if (rc == L7_SUCCESS)
      {
        mcopy = (struct rtmbuf *) buffer_addr;
        memset(mcopy, 0, sizeof( *mcopy));
        mcopy->rtm_pkthdr.rcvif = m->rtm_pkthdr.rcvif;
        rtcreateIcmpErrPckt(m, mcopy);

        if (tosEnable == L7_ENABLE)
          icmp_error(mcopy, ICMP_UNREACH, ICMP_UNREACH_TOSNET, dest, 0);
        else
          icmp_error(mcopy, ICMP_UNREACH, ICMP_UNREACH_NET, dest, 0);
      }
    }

    SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
    rtm_freem(m);
    return;
  }

  /* Type 2 and type 3 differentiation is done here. Check the note at the
     begining of this function. Type 3 should be dropped here.
  */
  if (m->rtm_flags & M_BCAST)
  {
    if (matchRoute == IP_LOCAL_ROUTE)
    {
      netMask = rtGetSubnetMaskOfIntf(MyLocalIfIndex);
    }
#ifndef L7_ORIGINAL_VENDOR_CODE
    if ((osapiNtohl(ip->ip_dst.s_addr) & (~netMask)) != (~netMask))
#else /* L7_ORIGINAL_VENDOR_CODE */
    if ((ntohl(ip->ip_dst.s_addr) & (~netMask)) != (~netMask))
#endif /* L7_ORIGINAL_VENDOR_CODE */
    {
      L7_ipstat.ips_cantforward++;
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
      rtm_freem(m);
      return;
    }
  }

#ifndef L7_ORIGINAL_VENDOR_CODE
  if (routerIP == osapiNtohl(ip->ip_dst.s_addr))
#else /* L7_ORIGINAL_VENDOR_CODE */
  if (routerIP == ntohl(ip->ip_dst.s_addr))
#endif /* L7_ORIGINAL_VENDOR_CODE */
  {
    L7_uint32 ifMask = rtGetSubnetMaskOfIntf(MyLocalIfIndex);
    /* Compare net-directed broadcast adress if netmask != 0xFFFFFFFF */
    if ((~ifMask) && ((routerIP & (~ifMask)) == ~ifMask))
    {
      netDirectBCastsEnable = ipMapRtrIntfNetDirectBcastsGet(MyLocalIfIndex);
      if (netDirectBCastsEnable == L7_ENABLE)
      {
        /* Get an mbuf to make a copy of the packet to be forwarded. Receive
         * mbuf will be consumed locally. */
        SYSAPI_NET_MBUF_GET(netMbufHandle);
        if (netMbufHandle == L7_NULL)
        {
          /* Cant forward. Can still consume locally */
          SYSAPI_NET_MBUF_GET_DATASTART((L7_netBufHandle)m->rtm_bufhandle, olddataStart);
          memcpy(&olddataStart[0], (L7_char8 *)rtetherbroadcastaddr, 6);
          ipmRecvLocal((L7_netBufHandle)m->rtm_bufhandle,MyLocalIfIndex);

          SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
          rtm_freem(m);
          return ;
        }
        SYSAPI_NET_MBUF_GET_DATASTART(netMbufHandle, pdataStart);
        SYSAPI_NET_MBUF_GET_DATASTART((L7_netBufHandle)m->rtm_bufhandle, olddataStart);
        SYSAPI_NET_MBUF_GET_DATALENGTH((L7_netBufHandle)m->rtm_bufhandle, datalen);
        memcpy(&pdataStart[0], &olddataStart[0], datalen);

        SYSAPI_NET_MBUF_SET_DATALENGTH(netMbufHandle, datalen);

      }

      SYSAPI_NET_MBUF_GET_DATASTART((L7_netBufHandle)m->rtm_bufhandle, olddataStart);
      memcpy(&olddataStart[0], (L7_char8 *)rtetherbroadcastaddr, 6);
      ipmRecvLocal((L7_netBufHandle)m->rtm_bufhandle,MyLocalIfIndex);

      if (netDirectBCastsEnable != L7_ENABLE)
      {
        L7_ipstat.ips_noroute++;
        rtm_freem(m);
        return;
      }
      else
        m->rtm_bufhandle = (void *)netMbufHandle;
    }

  }
  else
  {
    if (~netMask)
    {
#ifndef L7_ORIGINAL_VENDOR_CODE
      if ((osapiNtohl(ip->ip_dst.s_addr) & ~netMask) == ~netMask)
#else /* L7_ORIGINAL_VENDOR_CODE */
      if ((ntohl(ip->ip_dst.s_addr) & ~netMask) == ~netMask)
#endif /* L7_ORIGINAL_VENDOR_CODE */
      {
        netDirectBCastsEnable = ipMapRtrIntfNetDirectBcastsGet(MyLocalIfIndex);
        if (netDirectBCastsEnable != L7_ENABLE)
        {
          L7_ipstat.ips_noroute++;
          SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
          rtm_freem(m);
          return;
        }

      }
    }
  }

  /* Discard the packet if the dest ip address is a network address */
  if (matchRoute == IP_LOCAL_ROUTE)
  {
    netMask = rtGetSubnetMaskOfIntf(MyLocalIfIndex);
  }
#ifndef L7_ORIGINAL_VENDOR_CODE
  if ((netMask != L7_NULL_IP_MASK) && (netMask != 0xffffffff) &&
      ((osapiNtohl(ip->ip_dst.s_addr) & (~netMask)) == L7_NULL_IP_ADDR))
#else /* L7_ORIGINAL_VENDOR_CODE */
  if ((netMask != L7_NULL_IP_MASK) && (netMask != 0xffffffff) &&
      ((ntohl(ip->ip_dst.s_addr) & (~netMask)) == L7_NULL_IP_ADDR))
#endif /* L7_ORIGINAL_VENDOR_CODE */
  {
    /* Discard the packet. */
    L7_ipstat.ips_cantforward++;
    SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
    rtm_freem(m);
    return;
  }

  /* lvl7_@ip_forward end*/

  /*
   * Save the IP header and at most 8 bytes of the payload,
   * in case we need to generate an ICMP message to the src.
   *
   * We don't use m_copy() because it might return a reference
   * to a shared cluster. Both this function and ip_output()
   * assume exclusive access to the IP header in `m', so any
   * data in a cluster may change before we reach icmp_error().
   */

  /* lvl7_@ip_forward start*/
  mcopy = 0;
  rc = bufferPoolAllocate (buffer_pool_id, &buffer_addr);
  if (rc == L7_SUCCESS)
  {
    mcopy = (struct rtmbuf *) buffer_addr;
    memset(mcopy, 0, sizeof( *mcopy));
    mcopy->rtm_pkthdr.rcvif = m->rtm_pkthdr.rcvif;
    rtcreateIcmpErrPckt(m, mcopy);
  }
  /* lvl7_@ip_forward end*/

#ifdef IPSTEALTH
  if (!ipstealth)
  {
#endif
    ip->ip_ttl -= IPTTLDEC;
#ifdef IPSTEALTH
  }
#endif

  /*
   * If forwarding packet using same interface that it came in on,
   * perhaps should send a redirect to sender to shortcut a hop.
   * Only send redirect if source is sending directly to us,
   * and if packet was not source routed (or has any options).
   * Also, don't send redirect if forwarding using a default route
   * or a route modified by a redirect.
   */
#define satosin(sa) ((struct sockaddr_in *)(sa))

  /* lvl7_@ip_forward start*/
  icmpSendRedirectMode = ipMapRtrICMPRedirectModeGet();
#ifndef L7_ORIGINAL_VENDOR_CODE
  (void)ipMapIfICMPRedirectsModeGet (m->rtm_pkthdr.rcvif->if_index, &icmpIfSendRedirectMode); 
#endif
  if (MyLocalIfIndex == m->rtm_pkthdr.rcvif->if_index
      && icmpSendRedirectMode == L7_ENABLE && !srcrt 
#ifndef L7_ORIGINAL_VENDOR_CODE
&& icmpIfSendRedirectMode == L7_ENABLE
#endif
     )
  {
#ifndef L7_ORIGINAL_VENDOR_CODE
    unsigned long src = osapiNtohl(ip->ip_src.s_addr);
#else /* L7_ORIGINAL_VENDOR_CODE */
    unsigned long src = ntohl(ip->ip_src.s_addr);
#endif /* L7_ORIGINAL_VENDOR_CODE */
    if ((src & rtGetSubnetMaskOfIntf(MyLocalIfIndex))
        == (routerIP & rtGetSubnetMaskOfIntf(MyLocalIfIndex)))
    {
      dest = routerIP;
      type = ICMP_REDIRECT;
      code = ICMP_REDIRECT_HOST;
      /* lvl7_@ip_forward end*/

      if ((ipMapTraceFlags & IPMAP_TRACE_FORWARDING) &&
          (!ipMapTraceIntf || (ipMapTraceIntf == pduInfo.intIfNum)))
      {
        L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
        L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(dest, destStr);
        sprintf(traceBuf, "redirect (%d) to %s\n", code, destStr);
        ipMapTraceWrite(traceBuf);
      }
    }
  }
  /* lvl7_@ip_forward start*/
#ifndef L7_ORIGINAL_VENDOR_CODE
if ((MyLocalIfIndex == m->rtm_pkthdr.rcvif->if_index) && !srcrt)
{
  if (icmpSendRedirectMode == L7_DISABLE || icmpIfSendRedirectMode == L7_DISABLE)
  {
    icmpCtlStats.totalSuppressedRedirects++;
  }
}
error = 0;
  if ((sysapiNetMbufGetRxReasonCode((L7_netBufHandle)m->rtm_bufhandle)&
              L7_MBUF_RX_REASON_ICMP_REDIR) == 0 )
  {
#endif /* L7_ORIGINAL_VENDOR_CODE */
    error = rtip_output(m, (struct rtmbuf *)0, routerIP, netMask, MyLocalIfIndex,
                      IP_FORWARDING, 0, srcrt);
#ifndef L7_ORIGINAL_VENDOR_CODE
  }
  else
  {
     if (m->rtm_bufhandle != NULL)
                SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
        rtm_freem(m);
  }
#endif /* L7_ORIGINAL_VENDOR_CODE */

  if (error && error != EARPINPROGRESS)
    /* lvl7_@ip_forward end*/

    L7_ipstat.ips_cantforward++;
  else
  {
    L7_ipstat.ips_forward++;
    if (type)
      L7_ipstat.ips_redirectsent++;
    else
    {
      if (mcopy)
      {
        rtm_freem(mcopy);
      }
      return;
    }
  }
  if (mcopy == NULL)
    return;
  memset(&destifp, 0, sizeof(destifp));

  switch (error)
  {

  case 0:       /* forwarded, but need redirect */
    /* type, code set above */
    break;

    /* lvl7_@ip_forward start*/
  case EARPINPROGRESS:    /* Arp Resolve in Progress.Call back will take care  */
    rtm_freem(mcopy);
    return;
    /* lvl7_@ip_forward end*/

  case ENETUNREACH:   /* shouldn't happen, checked above */
  case EHOSTUNREACH:
  case ENETDOWN:
  case EHOSTDOWN:
  default:
    /* check unreachables mode */
    if(L7_ENABLE == icmpUnreachablesMode)
    {
      type = ICMP_UNREACH;
      if (tosEnable == L7_ENABLE)
        code = ICMP_UNREACH_TOSHOST;
      else
        code = ICMP_UNREACH_HOST;
    }
    else
    {
      rtm_freem(mcopy);
      return;
    }

    break;

  case EMSGSIZE:
    type = ICMP_UNREACH;
    code = ICMP_UNREACH_NEEDFRAG;
    L7_ipstat.ips_cantfrag++;
    rc = ipMapIntfEffectiveIpMtuGet(MyLocalIfIndex, &mtu_size);
    if (rc != L7_SUCCESS)
    {
      mtu_size = L7_PORT_ENET_ENCAP_DEF_MTU;  /* 1500 */
    }
    destifp.rtm_if_mtu = mtu_size;
    break;

  case ENOBUFS:
#ifndef L7_ORIGINAL_VENDOR_CODE
    type = ICMP_UNREACH;
    code = ICMP_UNREACH_NET;
#else
    type = ICMP_SOURCEQUENCH;
    code = 0;
#endif /* L7_ORIGINAL_VENDOR_CODE */
    break;

  case EACCES:      /* ipfw denied packet */
    rtm_freem(mcopy);
    return;
  }

  icmp_error(mcopy, type, code, dest, &destifp);
}

/*
 * Determine whether an IP address is in a reserved set of addresses
 * that may not be forwarded, or whether datagrams to that destination
 * may be forwarded.
 */
int
rtin_canforward(in)
    struct in_addr in;
{
#ifndef L7_ORIGINAL_VENDOR_CODE
    register u_long i = osapiNtohl(in.s_addr);
#else /* L7_ORIGINAL_VENDOR_CODE */
    register u_long i = ntohl(in.s_addr);
#endif /* L7_ORIGINAL_VENDOR_CODE */
    register u_long net;

    if (IN_EXPERIMENTAL(i) || IN_MULTICAST(i))
        return (0);
    if (IN_CLASSA(i)) {
        net = i & IN_CLASSA_NET;
        if (net == 0 || net == (IN_LOOPBACKNET << IN_CLASSA_NSHIFT))
            return (0);
    }
    return (1);
}

