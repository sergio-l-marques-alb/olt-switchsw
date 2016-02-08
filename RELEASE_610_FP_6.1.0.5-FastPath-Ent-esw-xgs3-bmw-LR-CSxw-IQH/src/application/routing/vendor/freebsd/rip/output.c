/*
 * Copyright (c) 1983, 1988, 1993
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
 *    must display the following acknowledgment:
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
 * $FreeBSD: src/sbin/routed/output.c,v 1.5 1999/09/05 17:49:11 peter Exp $
 */

#include "defs.h"
#include "osapi_support.h"
#include "rto_api.h"
#include "rip_map_debug.h"

#if VX_VERSION == 65
#include <net/if.h>
#endif

#if !defined(sgi) && !defined(__NetBSD__)
static char sccsid[] __attribute__((unused)) = "@(#)output.c    8.1 (Berkeley) 6/5/93";
#elif defined(__NetBSD__)
__RCSID("$NetBSD$");
#endif
#ident "$FreeBSD: src/sbin/routed/output.c,v 1.5 1999/09/05 17:49:11 peter Exp $"


u_int update_seqno;


/* walk the tree of routes with this for output
 */
struct {
    struct sockaddr_in to;
    naddr   to_mask;
    naddr   to_net;
    naddr   to_std_mask;
    naddr   to_std_net;
    struct interface *ifp;      /* usually output interface */
    struct auth *a;
    char    metric;         /* adjust metrics by interface */
    int npackets;
    int gen_limit;
    u_int   state;
#define     WS_ST_FLASH     0x001   /* send only changed routes */
#define     WS_ST_RIP2_ALL  0x002   /* send full featured RIPv2 */
#define     WS_ST_AG        0x004   /* ok to aggregate subnets */
#define     WS_ST_SUPER_AG  0x008   /* ok to aggregate networks */
#define     WS_ST_QUERY     0x010   /* responding to a query */
#define     WS_ST_TO_ON_NET 0x020   /* sending onto one of our nets */
#define     WS_ST_DEFAULT   0x040   /* faking a default */
#define     WS_ST_QUERY_ANS 0x080   /* ok to answer a query */  /* LVL7_MOD */
/* LVL7 NOTE: We deprecated WS_ST_QUERY, since it was not being used in a
 *            manner consistent with RFC 1058.  For LVL7 modifications to
 *            handling RIP requests, WD_ST_QUERY will always be off, and
 *            WS_ST_QUERY_ANS denotes whether a response is allowed.
 */
} ws;

/* A buffer for what can be heard by both RIPv1 and RIPv2 listeners */
struct ws_buf v12buf;
union pkt_buf ripv12_buf;

/* Another for only RIPv2 listeners */
struct ws_buf v2buf;
union pkt_buf rip_v2_buf;

/* Another buffer for RIP Request */
struct ws_buf req_buf;
union pkt_buf rip_req_buf;


void
bufinit(void)
{
    ripv12_buf.rip.rip_cmd = RIPCMD_RESPONSE;
    v12buf.buf = &ripv12_buf.rip;
    v12buf.base = &v12buf.buf->rip_nets[0];

    rip_v2_buf.rip.rip_cmd = RIPCMD_RESPONSE;
    rip_v2_buf.rip.rip_vers = RIPv2;
    v2buf.buf = &rip_v2_buf.rip;
    v2buf.base = &v2buf.buf->rip_nets[0];

    rip_req_buf.rip.rip_cmd = RIPCMD_REQUEST;
    req_buf.buf = &rip_req_buf.rip;
    req_buf.base = &req_buf.buf->rip_nets[0];
}


/* Send the contents of the global buffer via the non-multicast socket
 */
int                 /* <0 on failure */
output(enum output_type type,
       struct sockaddr_in *dst,     /* send to here */
       struct interface *ifp,
       struct rip *buf,
       int size)            /* this many bytes */
{
    struct sockaddr_in sin;
    L7_sockaddr_in_t l7sin;
    int flags;
    const char *msg;
    int res;
    int soc;
    int serrno;
  char addrStr[INET_NTOA_BUFSIZE];  /* LVL7_MOD */
    L7_int32 rc;
    fd_set writefds; /* fds ready to write */

    sin = *dst;
    if (sin.sin_port == 0)
        sin.sin_port = osapiHtons(RIP_PORT);
#ifdef _HAVE_SIN_LEN
    if (sin.sin_len == 0)
        sin.sin_len = sizeof(sin);
#endif

    if(ifp == 0)
    {
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"ifp received NULL in the output() function");
      return 0;
    }

    soc = ifp->int_rip_sock;
    flags = 0;

    switch (type) {
    case OUT_QUERY:
        msg = "Answer Query";
        if (soc < 0)
            soc = ifp->int_rip_sock;
        break;
    case OUT_UNICAST:
        msg = "Send";
        if (soc < 0)
            soc = ifp->int_rip_sock;
        flags = MSG_DONTROUTE;
        break;
    case OUT_BROADCAST:
        if (ifp->int_if_flags & IFF_POINTOPOINT) {
            msg = "Send";
        } else {
            msg = "Send bcast";
        }
        flags = MSG_DONTROUTE;
        break;
    case OUT_MULTICAST:
        if (ifp->int_if_flags & IFF_POINTOPOINT) {
            msg = "Send pt-to-pt";
        } else if (ifp->int_state & IS_DUP) {
            trace_act("abort multicast output via %s"
                  " with duplicate address",
                  ifp->int_name);
            RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Abort multicast output via %s with duplicate address", ifp->int_name);
            return 0;
        } else {
            msg = "Send mcast";
            sin.sin_addr.s_addr = osapiHtonl(INADDR_RIP_GROUP);
        }
        break;

    case NO_OUT_MULTICAST:
    case NO_OUT_RIPV2:
    default:
#ifdef DEBUG
        abort();
#endif
        return -1;
    }

    trace_rip(msg, "to", &sin, ifp, buf, size);
    RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"%s to %s", msg,
                    naddr_ntoa(osapiNtohl(sin.sin_addr.s_addr)));

    ripMapDebugPacketTxTrace(ifp->int_index,(char *)buf,size,naddr_ntoa(ifp->int_addr),naddr_ntoa(osapiNtohl(sin.sin_addr.s_addr)));

    /* easiest to convert sockaddr to L7_sockaddr here */
    l7sin = *(L7_sockaddr_in_t *)&sin;
    l7sin.sin_family = L7_AF_INET;
    rc = osapiSocketSendto(soc, (char *)buf, size, flags,
                           ( L7_sockaddr_t *)&l7sin, sizeof(l7sin),&res);
    if ((rc != L7_SUCCESS)
        && (ifp == 0 || !(ifp->int_state & IS_BROKE)))
    {
      if ((errno == EAGAIN) || (errno == EWOULDBLOCK))   
      {
        /* retry once */
        FD_ZERO(&writefds); 
        FD_SET(soc, &writefds);
        rc = osapiSelect(soc+1, NULL, &writefds, NULL, RIP_RETRY_TIMEOUT, 0);
        if (rc > 0)
        {
          rc = osapiSocketSendto(soc, (char *)buf, size, flags,
                                 ( L7_sockaddr_t *)&l7sin, sizeof(l7sin),&res);
        }
        else if (rc == 0)
        {
          /* select timed out */
          ripMapVendInetNtoa(osapiNtohl(sin.sin_addr.s_addr),
                             addrStr, INET_NTOA_BUFSIZE);     /* LVL7_MOD */
          serrno = errno;
          msglog("RIP: %s sendto(%s%s%s.%d) failed and write select timed out: %s\n", 
                 msg,
                 ifp != 0 ? ifp->int_name : "",
                 ifp != 0 ? ", " : "",
                 addrStr,                                   /* LVL7_MOD */
                 osapiNtohs(sin.sin_port),
                 strerror(errno));
    
          RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, "RIP: %s sendto(%s%s%s.%d) failed "
                          "and write select timed out: %s\n", msg,
                          ifp != 0 ? ifp->int_name : "",
                          ifp != 0 ? ", " : "",
                          addrStr,                                   /* LVL7_MOD */
                          osapiNtohs(sin.sin_port),
                          strerror(errno));
          errno = serrno;
        }
      }
      else  /* failure other than EAGAIN or EBUSY */
      {
        ripMapVendInetNtoa(osapiNtohl(sin.sin_addr.s_addr),
                           addrStr, INET_NTOA_BUFSIZE);     /* LVL7_MOD */
        serrno = errno;
        msglog("RIP: %s sendto(%s%s%s.%d) failed : %s\n", msg,
               ifp != 0 ? ifp->int_name : "",
               ifp != 0 ? ", " : "",
               addrStr,                                   /* LVL7_MOD */
               osapiNtohs(sin.sin_port),
               strerror(errno));
  
        RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, "RIP: %s sendto(%s%s%s.%d) failed : %s\n", msg,
                        ifp != 0 ? ifp->int_name : "",
                        ifp != 0 ? ", " : "",
                        addrStr,                                   /* LVL7_MOD */
                        osapiNtohs(sin.sin_port),
                        strerror(errno));
        errno = serrno;
      }
    }

    return res;
}


/* Find the first key for a packet to send.
 * Try for a key that is eligible and has not expired, but settle for
 * the last key if they have all expired.
 * If no key is ready yet, give up.
 */
struct auth *
find_auth(struct interface *ifp)
{
    struct auth *ap, *res;
    int i;


    if (ifp == 0)
        return 0;

    res = 0;
    ap = ifp->int_auth;
    for (i = 0; i < MAX_AUTH_KEYS; i++, ap++) {
        /* stop looking after the last key */
        if (ap->type == RIP_AUTH_NONE)
            break;

        /* ignore keys that are not ready yet */
        if ((u_long)ap->start > (u_long)clk.tv_sec)
            continue;

        if ((u_long)ap->end < (u_long)clk.tv_sec) {
            /* note best expired password as a fall-back */
            if (res == 0 || (u_long)ap->end > (u_long)res->end)
                res = ap;
            continue;
        }

        /* note key with the best future */
        if (res == 0 || (u_long)res->end < (u_long)ap->end)
            res = ap;
    }
    return res;
}


void
clr_ws_buf(struct ws_buf *wb,
       struct auth *ap)
{
    struct netauth *na;

    wb->lim = wb->base + NETS_LEN;
    wb->n = wb->base;
    memset(wb->n, 0, NETS_LEN*sizeof(*wb->n));

    /* (start to) install authentication if appropriate
     */
    if (ap == 0)
        return;

    na = (struct netauth*)wb->n;
    if (ap->type == RIP_AUTH_PW) {
        na->a_family = RIP_AF_AUTH;
        na->a_type = RIP_AUTH_PW;
        memcpy(na->au.au_pw, ap->key, sizeof(na->au.au_pw));
        wb->n++;

    } else if (ap->type ==  RIP_AUTH_MD5) {
        na->a_family = RIP_AF_AUTH;
        na->a_type = RIP_AUTH_MD5;
        na->au.a_md5.md5_keyid = ap->keyid;
#ifdef LVL7_ORIGINAL_VENDOR_CODE        /* LVL7_MOD start */
        na->au.a_md5.md5_auth_len = RIP_AUTH_MD5_LEN;
        na->au.a_md5.md5_seqno = htonl(clk.tv_sec);
#else                                   /* LVL7_MOD */
    /* common practice uses an auth length of 20 instead of 16
     * (i.e., includes 4-byte entry header plus the message digest length)
     */
        na->au.a_md5.md5_auth_len = (int8_t)RIP_AUTH_ENTRY_LEN;
        na->au.a_md5.md5_seqno = osapiHtonl(clk.tv_sec);
#endif /* LVL7_ORIGINAL_VENDOR_CODE */        /* LVL7_MOD end */
        wb->n++;
        wb->lim--;      /* make room for trailer */
    }
}


void
end_md5_auth(struct ws_buf *wb,
         struct auth *ap)
{
    struct netauth *na, *na2;
    MD5_CTX md5_ctx;
    int len;


    na = (struct netauth*)wb->base;
    na2 = (struct netauth*)wb->n;
    len = (char *)na2-(char *)wb->buf;
    na2->a_family = RIP_AF_AUTH;
#ifndef LVL7_ORIGINAL_VENDOR_CODE        /* LVL7_MOD start */
    na2->a_type = osapiHtons(1);
    na->au.a_md5.md5_pkt_len = osapiHtons(len);
    MD5Init(&md5_ctx);
  /* include 4-byte header of trailing auth data entry in digest */
    MD5Update(&md5_ctx, (u_char *)wb->buf, len+RIP_AUTH_ENTRY_HDR_LEN);
#else                                   /* LVL7_MOD */
    na2->a_type = htons(1);
    na->au.a_md5.md5_pkt_len = htons(len);
    MD5Init(&md5_ctx);
    MD5Update(&md5_ctx, (u_char *)wb->buf, len);
#endif /* LVL7_ORIGINAL_VENDOR_CODE */        /* LVL7_MOD end */
    MD5Update(&md5_ctx, ap->key, RIP_AUTH_MD5_LEN);
    MD5Final(na2->au.au_pw, &md5_ctx);
    wb->n++;
}


/* Send the buffer
 */
static void
supply_write(struct ws_buf *wb)
{
    /* Output multicast only if legal.
     * If we would multicast and it would be illegal, then discard the
     * packet.
     */
    switch (wb->type) {
    case NO_OUT_MULTICAST:
        trace_pkt("skip multicast to %s because impossible",
                  naddr_ntoa(osapiNtohl(ws.to.sin_addr.s_addr)));
        RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,
                        "Skip multicast to %s because impossible.",
                        naddr_ntoa(osapiNtohl(ws.to.sin_addr.s_addr)));
        break;
    case NO_OUT_RIPV2:
        break;
    default:
        if (ws.a != 0 && ws.a->type == RIP_AUTH_MD5)
            end_md5_auth(wb,ws.a);
        if (output(wb->type, &ws.to, ws.ifp, wb->buf,
               ((char *)wb->n - (char*)wb->buf)) < 0
            && ws.ifp != 0)
        {
          if_sick(ws.ifp);
        }
        else
        {
          /* RIP_MSG_LOG("\nsupply_write: sent buffer!"); */
        }
        ws.npackets++;
        break;
    }

    clr_ws_buf(wb,ws.a);
}


/* put an entry into the packet
 */
static void
supply_out(struct ag_info *ag)
{
    int i;
    naddr mask, v1_mask, dst_h, ddst_h = 0;
    struct ws_buf *wb;


    /* Skip this route if doing a flash update and it and the routes
     * it aggregates have not changed recently.
     */
    if (ag->ag_seqno < update_seqno
        && (ws.state & WS_ST_FLASH))
        return;

    dst_h = ag->ag_dst_h;
    mask = ag->ag_mask;
#ifndef L7_ORIGINAL_VENDOR_CODE
    v1_mask = ripv1_mask_host(dst_h,
                  (ws.state & WS_ST_TO_ON_NET) ? ws.ifp : 0);
#else /* L7_ORIGINAL_VENDOR_CODE */
    v1_mask = ripv1_mask_host(htonl(dst_h),
                  (ws.state & WS_ST_TO_ON_NET) ? ws.ifp : 0);
#endif /* L7_ORIGINAL_VENDOR_CODE */
    i = 0;

    /* If we are sending RIPv2 packets that cannot (or must not) be
     * heard by RIPv1 listeners, do not worry about sub- or supernets.
     * Subnets (from other networks) can only be sent via multicast.
     * A pair of subnet routes might have been promoted so that they
     * are legal to send by RIPv1.
     * If RIPv1 is off, use the multicast buffer.
     */
    if ((ws.state & WS_ST_RIP2_ALL)
        || ((ag->ag_state & AGS_RIPV2) && v1_mask != mask)) {
        /* use the RIPv2-only buffer */
        wb = &v2buf;

    } else {
        /* use the RIPv1-or-RIPv2 buffer */
        wb = &v12buf;

        /* Convert supernet route into corresponding set of network
         * routes for RIPv1, but leave non-contiguous netmasks
         * to ag_check().
         */
        if (v1_mask > mask
            && mask + (mask & -mask) == 0) {
            ddst_h = v1_mask & -v1_mask;
            i = (v1_mask & ~mask)/ddst_h;

            if (i > ws.gen_limit) {
                /* Punt if we would have to generate an
                 * unreasonable number of routes.
                 */
                if (TRACECONTENTS)
#ifndef L7_ORIGINAL_VENDOR_CODE 
                    trace_misc("sending %s-->%s as 1"
                           " instead of %d routes",
                           addrname(dst_h, mask, 0),
                           naddr_ntoa(osapiNtohl(ws.to.sin_addr.s_addr)),
                           i+1);

#else /* L7_ORIGINAL_VENDOR_CODE */
                   trace_misc("sending %s-->%s as 1"
                           " instead of %d routes",
                           addrname(htonl(dst_h), mask,
                            1),
                           naddr_ntoa(ws.to.sin_addr
                            .s_addr),
                           i+1);
#endif /* L7_ORIGINAL_VENDOR_CODE*/ 
                i = 0;

            } else {
                mask = v1_mask;
                ws.gen_limit -= i;
            }
        }
    }

    do {
        wb->n->n_family = RIP_AF_INET;
#ifndef L7_ORIGINAL_VENDOR_CODE
        wb->n->n_dst = osapiHtonl(dst_h);
#else /* L7_ORIGINAL_VENDOR_CODE */
        wb->n->n_dst = htonl(dst_h);
#endif/* L7_ORIGINAL_VENDOR_CODE */
        /* If the route is from router-discovery or we are
         * shutting down, admit only a bad metric.
         */
        wb->n->n_metric = ((stopint || ag->ag_metric < 1)
                   ? HOPCNT_INFINITY
                   : ag->ag_metric);
        HTONL(wb->n->n_metric);
        /* Any non-zero bits in the supposedly unused RIPv1 fields
         * cause the old `routed` to ignore the route.
         * That means the mask and so forth cannot be sent
         * in the hybrid RIPv1/RIPv2 mode.
         */
        if (ws.state & WS_ST_RIP2_ALL) {
            if (ag->ag_nhop != 0
                && ((ws.state & WS_ST_QUERY)
                || (ag->ag_nhop != ws.ifp->int_addr
                    && on_net(ag->ag_nhop,
                          ws.ifp->int_net,
                        ws.ifp->int_mask))))
#ifndef L7_ORIGINAL_VENDOR_CODE
            wb->n->n_nhop = osapiHtonl(ag->ag_nhop);
        
    wb->n->n_mask = osapiHtonl(mask);
            wb->n->n_tag = osapiHtons(ag->ag_tag);
          
#else  /*L7_ORIGINAL_VENDOR_CODE */
            wb->n->n_nhop = ag->ag_nhop;
            wb->n->n_mask=htonl(mask);
            wb->n->n_tag = ag->ag_tag;
           
#endif /* L7_ORIGINAL_VENDOR_CODE */
        }
        dst_h += ddst_h;

        if (++wb->n >= wb->lim)
            supply_write(wb);
    } while (i-- != 0);
}

/* check if addr is available on any alias (including self - ifp!) 
 */
L7_BOOL on_alias(struct interface * ifp, naddr addr)
{
  struct rt_entry *rt;
  struct interface * rtifp;
  /*
  char strTraceBuf[RIP_TRACE_ENTRY_LEN];
  char strTempBuf[RIP_TRACE_ENTRY_LEN];
  */

  /*
  sprintf(strTraceBuf, "\non_alias addr %x being sent via if %x is reachable via ", 
    (L7_uint32) addr, (L7_uint32) ifp->int_addr);
  */


  rt = rtfind(addr);

  /* route via next-hop gateway? */
  if(rt)
  {
    if(rt->rt_ifp == 0)
    {
      if(rt->rt_spares[0].rts_gate != 0)
      {
        /* sprintf(strTempBuf, "GW %x ", (L7_uint32) rt->rt_spares[0].rts_gate); */
        rt = rtfind(rt->rt_spares[0].rts_gate);  /* find if to gateway! */
      }
      else
      {
        /* sprintf(strTempBuf, "NO IF, NO GW! "); */
      }
    }
  }

  /* got a valid route? */
  if(rt)
  {
    if(rt->rt_ifp != 0) 
    {
      /* on alias? */
      rtifp = rt->rt_ifp;
      if(rt->rt_ifp->int_index == ifp->int_index) /* same ifindex */        
      {
        /*
        sprintf(strTempBuf, "ALIAS if %x", (L7_uint32) (rtifp)->int_addr); 
        RIP_DEBUG_TRACE(RIP_TRACE_OUTPUT_DETAIL, "%s %s", strTraceBuf, strTempBuf);
        */
        return L7_TRUE;
      }
      else
      {
        /* sprintf(strTempBuf, "OTHER if %x", (L7_uint32) (rtifp)->int_addr); */
      }
    }
    else
    {
      /* sprintf(strTempBuf, "NO IF! "); */
    }
  }
  else
  {
    /* sprintf(strTempBuf, "!NO_ROUTE!"); */
  }
 
  /* RIP_DEBUG_TRACE(RIP_TRACE_OUTPUT_DETAIL, "%s %s", strTraceBuf, strTempBuf); */

  return L7_FALSE;
}


/* supply one route from the table
 */
/* ARGSUSED */
static int
walk_supply(struct radix_node *rn,
        struct walkarg *argp UNUSED)
{
/* #define RT ((struct rt_entry *)rn) */        /* LVL7_MOD */
  struct rt_entry *RT = (struct rt_entry *)rn;  /* LVL7_MOD (was #define) */
    u_short ags;
    char metric, pref;
    naddr dst, nhop;
    struct rt_spare *rts;
    int i;
    int rtoGwAddr; /* to retrieve gw addr in RTO */
    char strTraceBuf[RIP_TRACE_ENTRY_LEN];
    L7_uchar8 strIfAddr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 strDstAddr[OSAPI_INET_NTOA_BUF_SIZE];


    /* Do not advertise external remote interfaces or passive interfaces.
     */
    if ((RT->rt_state & RS_IF)
        && RT->rt_ifp != 0
        && (RT->rt_ifp->int_if_flags & IS_PASSIVE)
        && !(RT->rt_state & RS_MHOME))
        return 0;

    /* If being quiet about our ability to forward, then
     * do not say anything unless responding to a query,
     * except about our main interface.
     */
    if (!supplier && !(ws.state & WS_ST_QUERY_ANS)  /* LVL7_MOD (was WS_ST_QUERY) */
        && !(RT->rt_state & RS_MHOME))
        return 0;

    dst = (RT->rt_dst);

    osapiInetNtoa((L7_uint32)dst, strDstAddr);
    osapiInetNtoa((L7_uint32)ws.ifp->int_addr, strIfAddr);              
    sprintf(strTraceBuf, "walk_supply ifaddr %s dst %s", 
         strIfAddr, strDstAddr);

    /* do not collide with the fake default route */
    if (dst == RIP_DEFAULT
        && (ws.state & WS_ST_DEFAULT))
        return 0;

    /* Detect subnet routes that are eclipsed by the interface!
     * - delete those that have a state=RS_SUBNET, metric=HOPCNT_INFINITY
     *   and no ifp.
     * 
     * This handles the case where the subnet mask of an interface is made more
     * general. E.g. 10.10.10.1/24 to 10.10.10.1/8 will leave behind a poisoned
     * route to 10.10.10.1/24. This makes no sense since this subnet is reachable 
     * via the new interface config (10.10.10.1/8).
     *
     */
    if((ws.ifp->int_addr & ws.ifp->int_mask) == ((dst & ws.ifp->int_mask) & RT->rt_mask) &&
       RT->rt_metric == HOPCNT_INFINITY && 
       RT->rt_state == RS_SUBNET && 
       RT->rt_ifp == 0)
    {
      RIP_DEBUG_TRACE(RIP_TRACE_OUTPUT_DETAIL, "%s deleting eclipsed subnet route mask %s", 
                      strTraceBuf, naddr_ntoa(RT->rt_mask)) ;
      rtdelete(RT);
      return 0;
    }

    /* fix for defect 18943
     * Verifies that the route being processed for advt is installed 
     * in the FIB by doing a best route lookup with RTO. This is required
     * because rto route addition failures are not currently handled by RIP.
     * Further since RTO provisions only "best" routes, if a RIP route is
     * no longer the "best" route (i.e it has (somehow) been superseded by 
     * a different route) then its not installed in the FIB, so donot advt!
     * The solution is to have RIP listen to best route announcements and 
     * handle rtoAdd failures (a rel J fix?).
     *
     * The current patch skips valid rip routes (metric<16) that are not
     * best routes. Routes with metric=16 are being advertised as unreachable
     * so the best route check is not applied on them (obviously they'd make
     * very sad best routes!).
     *
     * Also currently route addition is attempted only when a route becomes active,
     * RIP response messages do not trigger route addition. Events such as 
     * toggling link state, toggling rip admin state etc. cause RIPMAP to try
     * to re-add routes to RTO.
     */
    if(RT->rt_metric != HOPCNT_INFINITY)
    {
      if(rtoBestRouteLookupExact(RT->rt_dst, RT->rt_msk, &rtoGwAddr) == L7_SUCCESS)
      {
        if(rtoGwAddr != RT->rt_gate)
          rtoGwAddr = 0; /* signal that the RIP route is not the best route as per RTO */
        else
          rtoGwAddr = 1; /* RIP route is the best route as per RTO */
      }
      else
      {
        rtoGwAddr = 0; /* signal that the RIP route is not installed in RTO */
      }
  
      /* route not installed in FIB? (not installed in RTO or not best route) */
      if(rtoGwAddr == 0)
        return 0; /* do not advertise it! */
    }

    if (RT->rt_state & RS_NET_SYN) {
        if (RT->rt_state & RS_NET_INT) {
            /* Do not send manual synthetic network routes
             * into the subnet.
             */
            if (on_net(osapiNtohl(ws.to.sin_addr.s_addr), dst, RT->rt_mask))
                return 0;

        } else {
            /* Do not send automatic synthetic network routes
             * if they are not needed because no RIPv1 listeners
             * can hear them.
             */
            if (ws.state & WS_ST_RIP2_ALL)
                return 0;

            /* Do not send automatic synthetic network routes to
             * the real subnet.
             */
            if (on_net(osapiNtohl(ws.to.sin_addr.s_addr), dst, RT->rt_mask))
                return 0;
        }
        nhop = 0;

    } else {
        /* Advertise the next hop if this is not a route for one
         * of our interfaces and the next hop is on the same
         * network as the target.
         * The final determination is made by supply_out().
         */
        if (!(RT->rt_state & RS_IF)
            && RT->rt_gate != myaddr
            && RT->rt_gate != loopaddr)
            nhop = RT->rt_gate;
        else
            nhop = 0;
    }

    metric = RT->rt_metric;
    ags = 0;

    if (RT->rt_state & RS_MHOME) {
        /* retain host route of multi-homed servers */
        ;

    } else if (RT_ISHOST(RT)) {
        /* We should always suppress (into existing network routes)
         * the host routes for the local end of our point-to-point
         * links.
         * If we are suppressing host routes in general, then do so.
         * Avoid advertising host routes onto their own network,
         * where they should be handled by proxy-ARP.
         */
        if ((RT->rt_state & RS_LOCAL)
            || ridhosts
            || on_net(dst, ws.to_net, ws.to_mask))
            ags |= AGS_SUPPRESS;

        /* Aggregate stray host routes into network routes if allowed.
         * We cannot aggregate host routes into small network routes
         * without confusing RIPv1 listeners into thinking the
         * network routes are host routes.
         */
        if ((ws.state & WS_ST_AG)
            && !(ws.state & WS_ST_RIP2_ALL))
            ags |= AGS_AGGREGATE;

    } else {
        /* Always suppress network routes into other, existing
         * network routes
         */
        ags |= AGS_SUPPRESS;

        /* Generate supernets if allowed.
         * If we can be heard by RIPv1 systems, we will
         * later convert back to ordinary nets.
         * This unifies dealing with received supernets.
         */
        if ((ws.state & WS_ST_AG)
            && ((RT->rt_state & RS_SUBNET)
            || (ws.state & WS_ST_SUPER_AG)))
            ags |= AGS_AGGREGATE;
    }

    /* Do not send RIPv1 advertisements of subnets to other
     * networks. If possible, multicast them by RIPv2.
     */
    if ((RT->rt_state & RS_SUBNET)
        && !(ws.state & WS_ST_RIP2_ALL)
        && !on_net(dst, ws.to_std_net, ws.to_std_mask))
        ags |= AGS_RIPV2 | AGS_AGGREGATE;

    /* Always advertise an infinite metric for the route
     * corresponding to the output interface.
     *
     * Do not allow this local route to be suppressed into a
     * superset route or aggregated with other routes, since
     * the poisoned metric can become the metric of the container
     * route entry that ultimately gets advertised.
     */
      if ( (ws.ifp != 0) && 
          ( ((ws.to_net == dst) || (on_alias(ws.ifp, dst) == L7_TRUE)) &&
           (RT->rt_state & RS_IF)) )
      {
        if (ripSplitHorizon != SPHZ_NONE)         /* no split horizon */
        {
          metric = HOPCNT_INFINITY;
          ags &= ~(AGS_SUPPRESS | AGS_AGGREGATE); /* lvl7_d3228 */
          strcat(strTraceBuf, " [SPLT_HRZ");
        }
    }

    /* Do not send a route back to where it came from, except in
     * response to a query.  This is "split-horizon".  That means not
     * advertising back to the same network and so via the same interface.
     *
     * We want to suppress routes that might have been fragmented
     * from this route by a RIPv1 router and sent back to us, and so we
     * cannot forget this route here.  Let the split-horizon route
     * suppress the fragmented routes and then itself be forgotten.
     *
     * Include the routes for both ends of point-to-point interfaces
     * among those suppressed by split-horizon, since the other side
     * should knows them as well as we do.
     *
     * Notice spare routes with the same metric that we are about to
     * advertise, to split the horizon on redundant, inactive paths.
     */
    if (ws.ifp != 0
        && !(ws.state & WS_ST_QUERY)
        && ((ws.state & WS_ST_TO_ON_NET) || (on_alias(ws.ifp, dst) == L7_TRUE))
        && (!(RT->rt_state & RS_IF)
        || ws.ifp->int_if_flags & IFF_POINTOPOINT)) {
        for (rts = RT->rt_spares, i = NUM_SPARES; i != 0; i--, rts++) {

            if (rts->rts_metric > metric)                
              continue;

            /* Don't apply split horizon to redistributed routes since route
             * did not originate with a RIP neighbor. */
            if ((rts->rts_redist_proto > RIP_RT_REDIST_FIRST) && 
                (rts->rts_redist_proto < RIP_RT_REDIST_LAST))
              continue;

            if(on_alias(ws.ifp, dst) != L7_TRUE)
              continue;

            /* If we do not mark the route with AGS_SPLIT_HZ here,
             * it will be poisoned-reverse, or advertised back
             * toward its source with an infinite metric.
             * If we have recently advertised the route with a
             * better metric than we now have, then we should
             * poison-reverse the route before suppressing it for
             * split-horizon.
             *
             * In almost all cases, if there is no spare for the
             * route then it is either old and dead or a brand
             * new route. If it is brand new, there is no need
             * for poison-reverse. If it is old and dead, it
             * is already poisoned.
             */

            /* Check the configuration of split horizon mode.*/
            /* We should be doing whatever the configuration of split horizon is
               whether we are sending a response to a query or regular update.*/
            if (ripSplitHorizon == SPHZ_NONE)         /* no split horizon */
              break;
            else if (ripSplitHorizon == SPHZ_SIMPLE)  /* simple split horizon */
            {
              if (RT->rt_poison_time < now_expire
                  || RT->rt_poison_metric >= metric
                  || RT->rt_spares[1].rts_gate == 0) {
                ags |= AGS_SPLIT_HZ;
                ags &= ~AGS_SUPPRESS;
                strcat(strTraceBuf, " [SPLT_HRZ - supress SPARE]"); 
              }
              metric = HOPCNT_INFINITY;
              break;
            }
            else                                      /* poison reverse split horizon */
            {
              strcat(strTraceBuf, "[SPLT_HRZ - poison SPARE]"); 
              metric = HOPCNT_INFINITY;
              break;
            }
      
      /*      if (!(ws.state & WS_ST_QUERY_ANS))
            {
              if (RT->rt_poison_time < now_expire
                  || RT->rt_poison_metric >= metric
                  || RT->rt_spares[1].rts_gate == 0) {
                ags |= AGS_SPLIT_HZ;
                ags &= ~AGS_SUPPRESS;
              }
            }
      */
/*            metric = HOPCNT_INFINITY;
            break;
*/
        }
    }

    /* Keep track of the best metric with which the
     * route has been advertised recently.
     */
    if (RT->rt_poison_metric >= metric
        || RT->rt_poison_time < now_expire) {
        RT->rt_poison_time = now.tv_sec;
        RT->rt_poison_metric = metric;
    }

    /* Adjust the outgoing metric by the cost of the link.
     * Avoid aggregation when a route is counting to infinity.
     */
    pref = RT->rt_poison_metric + ws.metric;
    metric += ws.metric;

    /* Do not advertise stable routes that will be ignored,
     * unless we are answering a query.
     * If the route recently was advertised with a metric that
     * would have been less than infinity through this interface,
     * we need to continue to advertise it in order to poison it.
     */
    if (metric >= HOPCNT_INFINITY) {
        if (!(ws.state & WS_ST_QUERY_ANS)   /* LVL7_MOD (was WT_ST_QUERY) */
            && (pref >= HOPCNT_INFINITY
            || RT->rt_poison_time < now_garbage))
        {
          RIP_DEBUG_TRACE(RIP_TRACE_OUTPUT_DETAIL, "%s poisoned]", strTraceBuf);
          return 0;
        }

        metric = HOPCNT_INFINITY;        
    }

    /* Do not advertise route for local interface when sending a update for
       that interface
    */
    if ((ws.ifp != 0) && 
        (on_alias(ws.ifp, dst) == L7_TRUE) && 
        (RT->rt_state & RS_IF))
    {
      if (ripSplitHorizon == SPHZ_SIMPLE)  /* simple split horizon */
      {
        RIP_DEBUG_TRACE(RIP_TRACE_OUTPUT_DETAIL, "%s supress]", strTraceBuf);
        return 0;
      }
      else if (ripSplitHorizon == SPHZ_POISONREVERSE)
      {
        strcat(strTraceBuf, " poison]");
      }
    }


    /* Check for route auto summary mode.*/
    /* If route auto summarization is OFF, do not suppress or aggregate.
       If it is ON, leave it to the protocol to take care of aggregation*/
    if (ripAutoSummary == AUTOSUMMARY_OFF)
    {
      ags &= ~AGS_SUPPRESS;
      ags &= ~AGS_AGGREGATE;
    }

    ag_check(dst, RT->rt_mask, 0, nhop, metric, metric,
         RT->rt_seqno, RT->rt_tag, ags, supply_out);

    /* log trace */
    RIP_DEBUG_TRACE(RIP_TRACE_OUTPUT_DETAIL, "%s metric %u", strTraceBuf, RT->rt_metric);

    return 0;
/* #undef RT */                         /* LVL7_MOD */
}


/* Supply dst with the contents of the routing tables.
 * If this won't fit in one packet, chop it up into several.
 */
void
rip_supply(struct sockaddr_in *dst,
       struct interface *ifp,       /* output interface */
       enum output_type type,
       int flash,           /* 1=flash update */
       int vers,            /* RIP version */
       int passwd_ok)           /* OK to include cleartext password */
{
    struct rt_entry *rt;
    int def_metric;


    ws.state = 0;
    ws.gen_limit = 1024;

    ws.to = *dst;
    ws.to_std_mask = std_mask(osapiNtohl(ws.to.sin_addr.s_addr));
    ws.to_std_net = osapiNtohl(ws.to.sin_addr.s_addr) & ws.to_std_mask;

    if (ifp != 0) {
        ws.to_mask = ifp->int_mask;
        ws.to_net = ifp->int_net;

        if (on_net(osapiNtohl(ws.to.sin_addr.s_addr), ws.to_net, ws.to_mask))

            ws.state |= WS_ST_TO_ON_NET;

    } else {

        ws.to_mask = ripv1_mask_net(osapiNtohl(ws.to.sin_addr.s_addr), 0);
        ws.to_net = osapiNtohl(ws.to.sin_addr.s_addr) & ws.to_mask;
        rt = rtfind(osapiNtohl(dst->sin_addr.s_addr));

        if (rt)
            ifp = rt->rt_ifp;
    }

    ws.npackets = 0;
    if (flash)
        ws.state |= WS_ST_FLASH;

    if ((ws.ifp = ifp) == 0) {
        ws.metric = 1;
    } else {
        /* Do not adjust the metric during generation of response messages.
           The rfc 2453 doesn't say anything in this regard.
        */
        ws.metric = 0;
    }

    ripv12_buf.rip.rip_vers = vers;

    switch (type) {
    case OUT_MULTICAST:
        if ((ifp != 0) && (ifp->int_if_flags & IFF_MULTICAST))
            v2buf.type = OUT_MULTICAST;
        else
            v2buf.type = NO_OUT_MULTICAST;
        v12buf.type = OUT_BROADCAST;
        break;

    case OUT_QUERY:
        ws.state |= WS_ST_QUERY_ANS;        /* LVL7_MOD (added flag) */
        /* fall through */
    case OUT_BROADCAST:
    case OUT_UNICAST:
        v2buf.type = (vers == RIPv2) ? type : NO_OUT_RIPV2;
        v12buf.type = type;
        break;

    case NO_OUT_MULTICAST:
    case NO_OUT_RIPV2:
        break;          /* no output */
    }

    if (vers == RIPv2) {
        /* If we are sending ver 2 packet, we have to do full ver 2 whether
           we are broadcasting (rip1c) or multicasting (rip2)
        */
        ws.state |= WS_ST_RIP2_ALL;

        if ((ws.state & WS_ST_QUERY)
            || !(ws.state & WS_ST_TO_ON_NET)) {
            ws.state |= (WS_ST_AG | WS_ST_SUPER_AG);
        } else if (ifp == 0 || !(ifp->int_state & IS_NO_AG)) {
            ws.state |= WS_ST_AG;

            /* If we are sending ver 2 packet, we have to do full ver 2 whether
               we are broadcasting (rip1c) or multicasting (rip2)
            */
            if ((ifp == 0
                || !(ifp->int_state & IS_NO_SUPER_AG)))
                ws.state |= WS_ST_SUPER_AG;
        }
    }

    ws.a = (vers == RIPv2) ? find_auth(ifp) : 0;
    if (!passwd_ok && ws.a != 0 && ws.a->type == RIP_AUTH_PW)
        ws.a = 0;
    clr_ws_buf(&v12buf,ws.a);
    clr_ws_buf(&v2buf,ws.a);

    /*  Fake a default route if asked and if there is not already
     * a better, real default route.
     */
    if (supplier && ifp && (def_metric = ifp->int_d_metric) != 0) {
        if (0 == (rt = rtget(RIP_DEFAULT, 0))
            || rt->rt_metric+ws.metric >= def_metric) {
            ws.state |= WS_ST_DEFAULT;
            ag_check(0, 0, 0, 0, def_metric, def_metric,
                 0, 0, 0, supply_out);
        } else {
            def_metric = rt->rt_metric+ws.metric;
        }

        /* If both RIPv2 and the poor-man's router discovery
         * kludge are on, arrange to advertise an extra
         * default route via RIPv1.
         */
        if ((ws.state & WS_ST_RIP2_ALL)
            && (ifp->int_state & IS_PM_RDISC)) {
            ripv12_buf.rip.rip_vers = RIPv1;
            v12buf.n->n_family = RIP_AF_INET;
            v12buf.n->n_dst = RIP_DEFAULT;
            v12buf.n->n_metric = osapiHtonl(def_metric);
            v12buf.n++;
        }
    }

    (void)rn_walktree(rhead, walk_supply, 0);
    ag_flush(0,0,supply_out);

    /* Flush the packet buffers, provided they are not empty and
     * do not contain only the password.
     */
    if (v12buf.n != v12buf.base
        && (v12buf.n > v12buf.base+1
        || v12buf.base->n_family != RIP_AF_AUTH))
        supply_write(&v12buf);
    if (v2buf.n != v2buf.base
        && (v2buf.n > v2buf.base+1
        || v2buf.base->n_family != RIP_AF_AUTH))
        supply_write(&v2buf);

    /* If we sent nothing and this is an answer to a query, send
     * an empty buffer.
     */
    if (ws.npackets == 0
        && (ws.state & WS_ST_QUERY))
        supply_write(&v12buf);
}


/* send all of the routing table or just do a flash update
 */
void
rip_bcast(int flash)
{
#ifdef _HAVE_SIN_LEN
    static struct sockaddr_in dst = {sizeof(dst), AF_INET};
#else
    static struct sockaddr_in dst = {AF_INET};
#endif
    struct interface *ifp;
    enum output_type type;
    int vers;
    struct timeval rtime;


    need_flash = 0;
    intvl_random(&rtime, MIN_WAITTIME, MAX_WAITTIME);
    no_flash = rtime;
    ripTimevaladd(&no_flash, &now);

    if (rip_sock < 0)
        return;

    trace_act("send %s and inhibit dynamic updates for %d sec",
          flash ? "dynamic update" : "all routes",
          rtime.tv_sec);

    for (ifp = ifnet; ifp != 0; ifp = ifp->int_next) {
        /* Skip interfaces not doing RIP.
         * Do try broken interfaces to see if they have healed.
         */
        if (IS_RIP_OUT_OFF(ifp->int_state))
            continue;

        /* skip turned off interfaces */
        if (!iff_up(ifp->int_if_flags))
            continue;

        vers = (ifp->int_state & IS_NO_RIPV1_OUT) ? RIPv2 : RIPv1;

        if (ifp->int_if_flags & IFF_BROADCAST) {
            /* ordinary, hardware interface */
            dst.sin_addr.s_addr = osapiHtonl(ifp->int_brdaddr);

            if (vers == RIPv2
                && !(ifp->int_state  & IS_NO_RIP_MCAST)) {
                type = OUT_MULTICAST;
            } else {
                type = OUT_BROADCAST;
            }

        } else if (ifp->int_if_flags & IFF_POINTOPOINT) {
            /* point-to-point hardware interface */
            dst.sin_addr.s_addr = osapiHtonl(ifp->int_dstaddr);
            type = OUT_UNICAST;

        } else if (ifp->int_state & IS_REMOTE) {
            /* remote interface */
#ifndef L7_ORIGINAL_VENDOR_CODE
            dst.sin_addr.s_addr = osapiHtonl(ifp->int_addr);
#else /* L7_ORIGINAL_VENDOR_CODE */
            dst.sin_addr.s_addr = ifp->int_addr;
#endif /* L7_ORIGINAL_VENDOR_CODE */
            type = OUT_UNICAST;

        } else {
            /* ATM, HIPPI, etc. */
            continue;
        }

        supply(&dst, ifp, type, flash, vers, 1);
    ifp->int_mibstats.otrupdates++;     /* LVL7_MOD: RFC 1724 RIPv2 MIB */
    }

    update_seqno++;         /* all routes are up to date */
}


/* Ask for routes
 * Do it only once to an interface, and not even after the interface
 * was broken and recovered.
 */
void
rip_query(void)
{
#ifdef _HAVE_SIN_LEN
    static struct sockaddr_in dst = {sizeof(dst), AF_INET};
#else
    static struct sockaddr_in dst = {AF_INET};
#endif
    struct interface *ifp;
    struct auth      *ifap;
    enum output_type type;
    L7_uchar8 strIfAddr[OSAPI_INET_NTOA_BUF_SIZE];
    struct rip *rip_buf = NULL;


    if (rip_sock < 0)
        return;

    rip_buf = &rip_req_buf.rip;

    for (ifp = ifnet; ifp; ifp = ifp->int_next) {
        /* Skip interfaces those already queried.
         * Do not ask via interfaces through which we don't
         * accept input.  Do not ask via interfaces that cannot
         * send RIP packets.
         * Do try broken interfaces to see if they have healed.
         */
        if (IS_RIP_IN_OFF(ifp->int_state)
            || ifp->int_query_time != NEVER)
            continue;

        /* skip turned off interfaces */
        if (!iff_up(ifp->int_if_flags))
            continue;

        rip_buf->rip_vers = (ifp->int_state&IS_NO_RIPV1_OUT) ? RIPv2:RIPv1;
        rip_buf->rip_cmd = RIPCMD_REQUEST;

        /* Send a RIPv1 query only if allowed and if we will
         * listen to RIPv1 routers.
         */
        if ((ifp->int_state & IS_NO_RIPV1_OUT)
            || (ifp->int_state & IS_NO_RIPV1_IN)) {
            rip_buf->rip_vers = RIPv2;
        } else {
            rip_buf->rip_vers = RIPv1;
        }

        if (ifp->int_if_flags & IFF_BROADCAST) {
            /* ordinary, hardware interface */
#ifndef L7_ORIGINAL_VENDOR_CODE
            dst.sin_addr.s_addr = osapiHtonl(ifp->int_brdaddr);
#else /* L7_ORIGINAL_VENDOR_CODE */
           dst.sin_addr.s_addr = ifp->int_brdaddr;
#endif /* L7_ORIGINAL_VENDOR_CODE */

            /* Broadcast RIPv1 queries and RIPv2 queries
             * when the hardware cannot multicast.
             */
            if (rip_buf->rip_vers == RIPv2
                && (ifp->int_if_flags & IFF_MULTICAST)
                && !(ifp->int_state  & IS_NO_RIP_MCAST)) {
                type = OUT_MULTICAST;
            } else {
                type = OUT_BROADCAST;
            }

        } else if (ifp->int_if_flags & IFF_POINTOPOINT) {
            /* point-to-point hardware interface */
#ifndef L7_ORIGINAL_VENDOR_CODE
            dst.sin_addr.s_addr = osapiHtonl(ifp->int_dstaddr);
#else /* L7_ORIGINAL_VENDOR_CODE */
           dst.sin_addr.s_addr = ifp->int_dstaddr;
#endif/* L7_ORIGINAL_VENDOR_CODE */
            type = OUT_UNICAST;

        } else if (ifp->int_state & IS_REMOTE) {
            /* remote interface */
#ifndef L7_ORIGINAL_VENDOR_CODE
            dst.sin_addr.s_addr = osapiHtonl(ifp->int_addr);
#else /* L7_ORIGINAL_VENDOR_CODE */
            dst.sin_addr.s_addr = ifp->int_addr;
#endif /* L7_ORIGINAL_VENDOR_CODE */
            type = OUT_UNICAST;

        } else {
            /* ATM, HIPPI, etc. */
            continue;
        }
        /* LVL7_MOD Start: defect 68891  */
        ifap = (rip_buf->rip_vers == RIPv2) ? find_auth(ifp) : 0;
        /* Inserting authentication info in rip request */
        clr_ws_buf(&req_buf, ifap);
        /* LVL7_MOD End: defect 68891  */
        osapiInetNtoa((L7_uint32)ifp->int_addr, strIfAddr);
        RIP_DEBUG_TRACE(RIP_TRACE_REQUEST, 
                      "Requesting routes on interface %s", strIfAddr);
        /* LVL7_MOD Start: defect 68891  */
        req_buf.n->n_family = RIP_AF_UNSPEC;
#ifndef L7_ORIGINAL_VENDOR_CODE
        req_buf.n->n_metric = osapiHtonl(HOPCNT_INFINITY);
#else /* L7_ORIGINAL_VENDOR_CODE */
        req_buf.n->n_metric = htonl(HOPCNT_INFINITY);
#endif /* L7_ORIGINAL_VENDOR_CODE */
        req_buf.n++;
        /* LVL7_MOD End: defect 68891  */

        ifp->int_query_time = now.tv_sec+SUPPLY_INTERVAL;
        
        /* LVL7_MOD Start: defect 68891  */
        if (ifap != 0 && ifap->type == RIP_AUTH_MD5)
          end_md5_auth(&req_buf, ifap);
        /* LVL7_MOD End: defect 68891  */
        if (output(type, &dst, ifp, rip_buf, 
                   ((char *)req_buf.n - (char *)req_buf.buf)) < 0)
            if_sick(ifp);
    }
}
