/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  rtip_icmp.c
*
* @purpose   Generating ICMP Errors including ICMP Redirect
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
 *	@(#)ip_icmp.c	8.2 (Berkeley) 1/4/94
 * $FreeBSD: src/sys/netinet/ip_icmp.c,v 1.39.2.8 2001/04/25 12:25:58 ru Exp $
 */
/*********************************************************************
 *                   
 ********************************************************************/
#define _IP_VHL

/* lvl7_@ip_forward start*/
#include "rtip_icmp.h"
#include "rtiprecv.h"
#include <time.h>
#include <sys/times.h>
#include "rticmp_var.h"
#include "osapi.h"                    /* LVL7_MOD */
#include "sysapi.h"
#include "buff_api.h"
#include "ip_util.h"
/* lvl7_@ip_forward end*/

                  
#include "l7_vrrp_api.h"

int  rtip_defttl = IPDEFTTL;
/* LVL7_COMMENT: changed the struct to be a global instead
   of a static */
struct L7_icmpstat L7_icmpstat;

#ifndef L7_ORIGINAL_VENDOR_CODE
static int icmp_send (struct rtmbuf *, struct rtmbuf *);
static int  icmp_reflect (struct rtmbuf *);
#else
static void  icmp_send (struct rtmbuf *, struct rtmbuf *);
static void  icmp_reflect (struct rtmbuf *);
#endif

extern L7_uint32 ipMapTraceFlags;
#ifndef L7_ORIGINAL_VENDOR_CODE
extern ipMapDebugIcmpCtlStats_t icmpCtlStats;
extern L7_BOOL ratelimitKicks;
#endif
void rtipIcmpStatsShow(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nnumber of calls to icmp_error: %lu", L7_icmpstat.icps_error); 

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nno error because old was icmp: %lu", L7_icmpstat.icps_oldicmp); 

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nNum of ICMP Destination Unreachable sent: %lu",  
                L7_icmpstat.icps_outhist[ICMP_UNREACH]); 

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nNum of ICMP Source Quench sent: %lu",  
                L7_icmpstat.icps_outhist[ICMP_SOURCEQUENCH]); 

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nNum of ICMP Redirects sent: %lu",  
                L7_icmpstat.icps_outhist[ICMP_REDIRECT]); 

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nNum of ICMP Time Exceeded sent: %lu",  
                L7_icmpstat.icps_outhist[ICMP_TIMXCEED]); 

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, 
                "\nNum of ICMP Parameter Problem sent: %lu",  
                L7_icmpstat.icps_outhist[ICMP_PARAMPROB]); 
}

/*
 * Generate an error packet of type error
 * in response to bad packet ip.
 */
void
icmp_error(n, type, code, dest, destifp)
struct rtmbuf *n;
int type, code;
unsigned long dest;
struct rtm_ifnet *destifp;
{
  register struct ip *oip = rtmtod(n, struct ip *), *nip;
  register unsigned oiplen = IP_VHL_HL(oip->ip_vhl) << 2;
  register struct icmp *icp;
#ifndef L7_ORIGINAL_VENDOR_CODE
  int error = 0;
#endif

  /* lvl7_@ip_forward start*/
  register struct rtmbuf *m ;
  L7_uchar8  * buffer_addr;
  L7_uint32  buffer_pool_id = rtMemPoolIdGet();
  L7_RC_t rc;
  /* lvl7_@ip_forward end*/

  unsigned icmplen;
#ifndef L7_ORIGINAL_VENDOR_CODE
  /* ICMP Error Msgs Ratelimiting: Don't Rate Limit Redirect Messages. */
  if ((type != ICMP_REDIRECT) && (isRateLimitTimerNull()!= L7_TRUE)&& (ipMapIcmpRatelimitSendCountGet()== 0 ))
  {
    icmpCtlStats.totalRateLimitedPkts++;
    if (ratelimitKicks == L7_FALSE) 
    {
      icmpCtlStats.rateLimitKicks++; 
    }
    ratelimitKicks = L7_TRUE;
    goto freeit; 
  }
  ratelimitKicks = L7_FALSE;
#endif
  /* lvl7_@ip_forward start*/
  SYSAPI_IP_STATS_INCREMENT(FD_CNFGR_NIM_MIN_CPU_INTF_NUM, 
                            L7_PLATFORM_CTR_RX_IP_IN_DELIVERS);

  /* lvl7_@ip_forward end*/

if (ipMapTraceFlags & IPMAP_TRACE_FORWARDING)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    L7_uchar8 srcStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(ntohl(oip->ip_src.s_addr), srcStr);
    osapiInetNtoa(ntohl(oip->ip_dst.s_addr), destStr);
    sprintf(traceBuf, "Sending ICMP error msg for proto %d pkt from %s to %s."
            " icmp type %d, icmp code %d.", 
            oip->ip_p, srcStr, destStr, type, code);
    ipMapTraceWrite(traceBuf);
  }

if (type != ICMP_REDIRECT)
    L7_icmpstat.icps_error++ ;
  /*
   * Don't send error if not the first fragment of message.
   * Don't error if the old packet protocol was ICMP
   * error message, only known informational types.
   */
  if (osapiNtohs(oip->ip_off) &~ (IP_MF|IP_DF))
    goto freeit;
  if (oip->ip_p == IPPROTO_ICMP && type != ICMP_REDIRECT &&
      n->rtm_len >= oiplen + ICMP_MINLEN &&
      !ICMP_INFOTYPE(((struct icmp *)((caddr_t)oip + oiplen))->icmp_type))
  {
    L7_icmpstat.icps_oldicmp++;
    goto freeit;
  }
  /* Don't send error in response to a multicast or broadcast packet */
  if (n->rtm_flags & (M_BCAST|M_MCAST))
    goto freeit;
  /*
   * First, formulate icmp message
     */
  /* lvl7_@ip_forward start*/
  m = 0;
  rc = bufferPoolAllocate (buffer_pool_id, &buffer_addr);
  if (rc == L7_SUCCESS)
  {
    m = (struct rtmbuf *) buffer_addr;
  }
  if (m == NULL)
    goto freeit;
  memset(m, 0, sizeof( *m));
  m->rtm_type = MT_DATA;
  m->rtm_flags = M_PKTHDR;
  m->rtm_data = m->rtm_pktdat;
  /* lvl7_@ip_forward end*/


	/*  Get destination MAC address from mbuf and copy to new MBUF
	 *  The m_dmac is used in icmp_reflect().  This could be a VRRP MAC
	 *  which was preserved earlier.
	 */
	bcopy(n->m_dmac, m->m_dmac, L7_MAC_ADDR_LEN);


  /*m = m_gethdr(M_DONTWAIT, MT_HEADER);
  if (m == NULL)
    goto freeit; */

  icmplen = min(oiplen + 8, osapiNtohs(oip->ip_len));
  if (icmplen < sizeof(struct ip))
  {
    printf("icmp_error: bad length\n");
    rtm_freem(m);
    goto freeit;
  }
  /* lvl7_@ip_forward start*/
  m->rtm_len = m->rtm_pkthdr.len = icmplen + ICMP_MINLEN;
  m->rtm_data += sizeof(struct ip);
  /* lvl7_@ip_forward end*/

  icp = rtmtod(m, struct icmp *);
  if ((unsigned int)type > ICMP_MAXTYPE)
  {
    L7_LOGFNC(L7_LOG_SEVERITY_NOTICE,
              "Received ICMP packet with bad packet type %d", type);
    rtm_freem(m);
    rtm_freem(n);
    return;
  }
#ifdef L7_ORIGINAL_VENDOR_CODE
  L7_icmpstat.icps_outhist[type]++;
#endif
  icp->icmp_type = type;
  if (type == ICMP_REDIRECT)
    icp->icmp_gwaddr = osapiHtonl(dest);
  else
  {
    icp->icmp_void = 0;
    /*
     * The following assignments assume an overlay with the
     * zeroed icmp_void field.
     */
    if (type == ICMP_PARAMPROB)
    {
      icp->icmp_pptr = code;
      code = 0;
    }
    else if (type == ICMP_UNREACH &&
             code == ICMP_UNREACH_NEEDFRAG && destifp)
    {
      icp->icmp_nextmtu = (short) osapiHtonl(destifp->rtm_if_mtu);  
    }
  }

  icp->icmp_code = code;

  /* lvl7_@ip_forward start*/
  rtm_copydata(n, 0, icmplen, (caddr_t)&icp->icmp_ip);
  /* lvl7_@ip_forward end*/

  nip = (struct ip *)&icp->icmp_ip;

  /*
   * Now, copy old ip header (without options)
   * in front of icmp message.
   */
  if (m->rtm_data - sizeof(struct ip) < m->rtm_pktdat)
    LOG_MSG("icmp len");
  m->rtm_data -= sizeof(struct ip);
  m->rtm_len += sizeof(struct ip);
  m->rtm_pkthdr.len = m->rtm_len;
  m->rtm_pkthdr.rcvif = n->rtm_pkthdr.rcvif;
  nip = rtmtod(m, struct ip *);
  bcopy((caddr_t)oip, (caddr_t)nip, sizeof(struct ip));
  nip->ip_len = osapiHtons(m->rtm_len);
  nip->ip_vhl = IP_VHL_BORING;
  nip->ip_p = IPPROTO_ICMP;
  nip->ip_tos = 0;
#ifndef L7_ORIGINAL_VENDOR_CODE
  error = icmp_reflect(m);
 /*  Don't  increment counters when mbufs  alloc failed cases.Don't Ratelimit them.
 */
  if (error != ENOBUFS)
  {
    L7_icmpstat.icps_outhist[type]++;
    /* Decrementing the Send count for ICMP Error Messages. */
    if (type != ICMP_REDIRECT)
    {
      if (isRateLimitTimerNull() == L7_TRUE)
      {
        ipMapIcmpRatelimitTimerCb(); 
        ipMapIcmpRatelimitTimerStart(); 
      }
      ipMapIcmpRatelimitSendCountDec();
    }
  }
#else
  icmp_reflect(m);
#endif 

  freeit:
  rtm_freem(n);
}

/*
 * Reflect the ip packet back to the source
 */
#ifndef L7_ORIGINAL_VENDOR_CODE
static int
#else
static void
#endif
icmp_reflect(m)
struct rtmbuf *m;
{
  register struct ip *ip = rtmtod(m, struct ip *);
  /*register struct in_ifaddr *ia;*/
  struct in_addr t;
  struct rtmbuf *opts = 0;
  int optlen = (IP_VHL_HL(ip->ip_vhl) << 2) - sizeof(struct ip);
  L7_uint32 myLocalIfIndex;
  L7_uint32 ipAddress;
  L7_ushort16 ipLen;
#ifndef L7_ORIGINAL_VENDOR_CODE
  int error = 0;  
#endif


  if (!rtin_canforward(ip->ip_src) &&
      ((osapiNtohl(ip->ip_src.s_addr) & IN_CLASSA_NET) !=
       (IN_LOOPBACKNET << IN_CLASSA_NSHIFT)))
  {
    rtm_freem(m);
    /* Bad return address */
    goto done;  /* Ip_output() will check for broadcast */
  }
  t = ip->ip_dst;
  ip->ip_dst = ip->ip_src;


  /* lvl7_@ip_forward start*/
  myLocalIfIndex = 0;
  rtCheckOurIntf(&myLocalIfIndex, osapiNtohl(t.s_addr));

  if (myLocalIfIndex == 0)
    t.s_addr = osapiHtonl(rtGetIPAddrOfIntf(m->rtm_pkthdr.rcvif->if_index));
  /* lvl7_@ip_forward end*/


  if (vrrpFuncTable.L7_vrrpIpGet &&
      (vrrpFuncTable.L7_vrrpIpGet(m->m_dmac, &ipAddress) == L7_SUCCESS))
  {
    t.s_addr = osapiHtonl(ipAddress);
  }
       
  ip->ip_src = t;
  ip->ip_ttl = rtip_defttl;

  if (optlen > 0)
  {
    register unsigned char *cp;
    int opt, cnt;
    unsigned int len;

    /*
     * Retrieve any source routing from the incoming packet;
     * add on any record-route or timestamp options.
     */
    cp = (u_char *) (ip + 1);

    if (opts)
    {
#ifdef ICMPPRINTFS
      if (icmpprintfs)
        printf("icmp_reflect optlen %d rt %d => ",
               optlen, opts->rtm_len);
#endif
      for (cnt = optlen; cnt > 0; cnt -= len, cp += len)
      {
        opt = cp[IPOPT_OPTVAL];
        if (opt == IPOPT_EOL)
          break;
        if (opt == IPOPT_NOP)
          len = 1;
        else
        {
          if (cnt < IPOPT_OLEN + sizeof(*cp))
            break;
          len = cp[IPOPT_OLEN];
          if (len < IPOPT_OLEN + sizeof(*cp) ||
              len > cnt)
            break;
        }
        /*
         * Should check for overflow, but it "can't happen"
         */
        if (opt == IPOPT_RR || opt == IPOPT_TS ||
            opt == IPOPT_SECURITY)
        {
          bcopy((caddr_t)cp,
                rtmtod(opts, caddr_t) + opts->rtm_len, len);
          opts->rtm_len += len;
        }
      }
      /* Terminate & pad, if necessary */
      cnt = opts->rtm_len % 4;
      if (cnt)
      {
        for (; cnt < 4; cnt++)
        {
          *(rtmtod(opts, caddr_t) + opts->rtm_len) =
          IPOPT_EOL;
          opts->rtm_len++;
        }
      }
#ifdef ICMPPRINTFS
      if (icmpprintfs)
        printf("%d\n", opts->rtm_len);
#endif
    }
    /*
     * Now strip out original options by copying rest of first
     * mbuf's data back, and adjust the IP length.
     */
    ipLen = osapiNtohs(ip->ip_len) - optlen;
    ip->ip_len = osapiHtons(optlen);
    ip->ip_vhl = IP_VHL_BORING;
    m->rtm_len -= optlen;
    if (m->rtm_flags & M_PKTHDR)
      m->rtm_pkthdr.len -= optlen;
    optlen += sizeof(struct ip);
    bcopy((caddr_t)ip + optlen, (caddr_t)(ip + 1),
          (unsigned)(m->rtm_len - sizeof(struct ip)));
  }
  m->rtm_flags &= ~(M_BCAST|M_MCAST);
#ifndef L7_ORIGINAL_VENDOR_CODE
  error = icmp_send(m, opts);
#else
  icmp_send(m, opts);
#endif
  done:
  if (opts)
    (void)rtm_freem(opts) ;
#ifndef L7_ORIGINAL_VENDOR_CODE
    return error;
#endif
    
}

/*
 * Send an icmp packet back to the ip level,
 * after supplying a checksum.
 */
#ifndef L7_ORIGINAL_VENDOR_CODE
static int
#else
static void
#endif
icmp_send(m, opts)
register struct rtmbuf *m;
struct rtmbuf *opts;
{
  register struct ip *ip = rtmtod(m, struct ip *);
  register int hlen;
  register struct icmp *icp;
  

  hlen = IP_VHL_HL(ip->ip_vhl) << 2;
  m->rtm_data += hlen;
  m->rtm_len -= hlen;
  icp = rtmtod(m, struct icmp *);
  icp->icmp_cksum = 0;
  icp->icmp_cksum = rtin_cksum(m, osapiNtohs(ip->ip_len) - hlen);
  m->rtm_data -= hlen;
  m->rtm_len += hlen;
  m->rtm_pkthdr.rcvif = (struct rtm_ifnet *)0;
#ifdef ICMPPRINTFS
  if (icmpprintfs)
  {
    char buf[OSAPI_INET_NTOA_BUF_SIZE];
    memset(buf, 0, OSAPI_INET_NTOA_BUF_SIZE);
    osapiInetNtoa(osapiNtohl(ip->ip_dst.s_addr), buf);
    printf("icmp_send dst %s",
           buf);
    memset(buf, 0, OSAPI_INET_NTOA_BUF_SIZE);
    osapiInetNtoa(osapiNtohl(ip->ip_src.s_addr), buf);
    printf(" src %s\n",
           buf);
  }
#endif

  /* lvl7_@ip_forward start*/
#ifndef L7_ORIGINAL_VENDOR_CODE
  return (rtip_output(m, opts, 0, 0, 0, 0, NULL, 0));
#else
  rtip_output(m, opts, 0, 0, 0, 0, NULL, 0);
#endif
  /* lvl7_@ip_forward end*/
}

void
microtime( struct timeval *tv)
{
  L7_clocktime ct;
  osapiClockTimeRaw ( &ct );
  tv->tv_sec = ct.seconds;
  tv->tv_usec = ct.nanoseconds / 1000;

}

n_time
rtiptime()
{
  struct timeval atv;
  u_long t;

  microtime(&atv);
  t = (atv.tv_sec % (24*60*60)) * 1000 + atv.tv_usec / 1000;
  return(osapiHtonl(t));
}


#if LVL7_FUNCTION_NOT_SUPPORTED
/*
 * Return the next larger or smaller MTU plateau (table from RFC 1191)
 * given current value MTU.  If DIR is less than zero, a larger plateau
 * is returned; otherwise, a smaller value is returned.
 */
static int
ip_next_mtu(mtu, dir)
int mtu;
int dir;
{
  static int mtutab[] = {
    65535, 32000, 17914, 8166, 4352, 2002, 1492, 1006, 508, 296,
    68, 0
  };
  int i;

  for (i = 0; i < (sizeof mtutab) / (sizeof mtutab[0]); i++)
  {
    if (mtu >= mtutab[i])
      break;
  }

  if (dir < 0)
  {
    if (i == 0)
    {
      return 0;
    }
    else
    {
      return mtutab[i - 1];
    }
  }
  else
  {
    if (mtutab[i] == 0)
    {
      return 0;
    }
    else if (mtu > mtutab[i])
    {
      return mtutab[i];
    }
    else
    {
      return mtutab[i + 1];
    }
  }
}
#endif

#ifdef ICMP_BANDLIM

/*
 * badport_bandlim() - check for ICMP bandwidth limit
 *
 *	Return 0 if it is ok to send an ICMP error response, -1 if we have
 *	hit our bandwidth limit and it is not ok.  
 *
 *	If icmplim is <= 0, the feature is disabled and 0 is returned.
 *
 *	For now we separate the TCP and UDP subsystems w/ different 'which'
 *	values.  We may eventually remove this separation (and simplify the
 *	code further).
 *
 *	Note that the printing of the error message is delayed so we can
 *	properly print the icmp error rate that the system was trying to do
 *	(i.e. 22000/100 pps, etc...).  This can cause long delays in printing
 *	the 'final' error, but it doesn't make sense to solve the printing 
 *	delay with more complex code.
 */

int
badport_bandlim(int which)
{
  static int lticks[BANDLIM_MAX + 1];
  static int lpackets[BANDLIM_MAX + 1];
  int dticks;
  const char *bandlimittype[] = {
    "Limiting icmp unreach response",
    "Limiting icmp ping response",
    "Limiting icmp tstamp response",
    "Limiting closed port RST response",
    "Limiting open port RST response"
  };

  /*
   * Return ok status if feature disabled or argument out of
   * ranage.
   */

  if (icmplim <= 0 || which > BANDLIM_MAX || which < 0)
    return(0);
  dticks = ticks - lticks[which];

  /*
   * reset stats when cumulative dt exceeds one second.
   */

  if ((unsigned int)dticks > hz)
  {
    if (lpackets[which] > icmplim)
    {
      printf("%s from %d to %d packets per second\n",
             bandlimittype[which],
             lpackets[which],
             icmplim
            );
    }
    lticks[which] = ticks;
    lpackets[which] = 0;
  }

  /*
   * bump packet count
   */

  if (++lpackets[which] > icmplim)
  {
    return(-1);
  }
  return(0);
}

#endif


