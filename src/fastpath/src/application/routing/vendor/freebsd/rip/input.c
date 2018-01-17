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
 * $FreeBSD: src/sbin/routed/input.c,v 1.7 1999/09/05 17:49:11 peter Exp $
 */

#include "defs.h"
#include "osapi_support.h"
#include "simapi.h"
#include "rip_map_debug.h"

#if !defined(sgi) && !defined(__NetBSD__)
static char sccsid[] __attribute__((unused)) = "@(#)input.c 8.1 (Berkeley) 6/5/93";
#elif defined(__NetBSD__)
#include <sys/cdefs.h>
__RCSID("$NetBSD$");
#endif

static void input(struct sockaddr_in *, struct interface *, struct interface *,
          struct rip *, int);
static void input_route(naddr, naddr, struct rt_spare *, struct netinfo *);
static int ck_passwd(struct interface *, struct rip *, void *,
             naddr, struct msg_limit *);
static int ck_auth(struct interface *, struct rip *, void *,
             naddr, struct msg_limit *, struct auth *);

/* LVL7_MOD: RFC 1724 RIPv2 MIB start */
/* MIB counter working variables */
static u_int ripQueryResponse;
static u_int ripRespBadPkts;
static u_int ripRespBadRoutes;
/* LVL7_MOD: RFC 1724 RIPv2 MIB end */


/* process RIP input
 */
void
read_rip(int sock,
     struct interface *sifp)
{
    L7_sockaddr_in_t from,to;
    struct interface *aifp;
    int fromlen, tolen,cc;
    int flags;
    L7_uint32 ifIndex, ripIfIndex, dum_hops;
    L7_RC_t rc;
#ifdef USE_PASSIFNAME
    static struct msg_limit  bad_name;
    struct {
        char    ifname[IFNAMSIZ];
        union pkt_buf pbuf;
    } inbuf;
#else
    struct {
        union pkt_buf pbuf;
    } inbuf;
#endif


    for (;;) {
        fromlen = sizeof(from);
        flags = 0;
        cc = sizeof(inbuf);
	tolen = sizeof(to);
        rc = osapiPktInfoRecv(sock, (void *)&inbuf, &cc, &flags,
                  (L7_sockaddr_t *)&from, &fromlen, 
                  (L7_sockaddr_t *)&to, &tolen, &ifIndex,&dum_hops, NULL);

        if (rc != L7_SUCCESS) {
            if (cc < 0 && errno != EWOULDBLOCK)
                LOGERR("RIP: Could not receive from socket.\n");
            break;
        }
        if(flags & MSG_TRUNC)
        {
            LOGERR("RIP: Could not receive from socket.\n");
            break;
        }
        if(flags & MSG_CTRUNC)
        {
            LOGERR("RIP: Could not receive from socket.\n");
            break;
        }
        if (fromlen != sizeof(struct sockaddr_in))
            logbad(1,"RIP: Bad num of bytes received from socket. fromlen=%d\n",
                   fromlen);
        if(inbuf.pbuf.rip.rip_vers == 1)
        {
            if((osapiNtohl(to.sin_addr.s_addr) & 0xff000000) == 0xe0000000)
            {
                LOGERR("Bad RIP Packet.\n");
                break;
            }
        }
        /* aifp is the "authenticated" interface via which the packet
         *  arrived.  In fact, it is only the interface on which
         *  the packet should have arrived based on is source
         *  address.
         * sifp is interface associated with the socket through which
         *  the packet was received.
         */
#ifdef USE_PASSIFNAME
        if ((cc -= sizeof(inbuf.ifname)) < 0)
            logbad(0,"RIP: missing USE_PASSIFNAME; only %d bytes\n",
                   cc+sizeof(inbuf.ifname));

        /* check the remote interfaces first */
        for (aifp = remote_if; aifp; aifp = aifp->int_rlink) {
            if (aifp->int_addr == osapiNtohl(from.sin_addr.s_addr))
                break;
        }
        if (aifp == 0) {
            aifp = ifwithname(inbuf.ifname, 0);
            if (aifp == 0) {
                msglim(&bad_name, osapiNtohl(from.sin_addr.s_addr),
                       "RIP: impossible interface name %.*s\n",
                       IFNAMSIZ, inbuf.ifname);
            } else if (((aifp->int_if_flags & IFF_POINTOPOINT)
                    && aifp->int_dstaddr!=osapiNtohl(from.sin_addr.s_addr))
                   || (!(aifp->int_if_flags & IFF_POINTOPOINT)
                       && !on_net(osapiNtohl(from.sin_addr.s_addr),
                          aifp->int_net,
                          aifp->int_mask))) {
                /* If it came via the wrong interface, do not
                 * trust it.
                 */
                aifp = 0;
            }
        }
#else
        aifp = 0;
        if(get_rip_ifindex(ifIndex, &ripIfIndex) == 0)
        {
           aifp = ifwith_index_and_addr(ripIfIndex,
                                        osapiNtohl(from.sin_addr.s_addr), 0);
        }
#endif
        if (sifp == 0)
            sifp = aifp;

    /* LVL7_MOD: RFC 1724 RIPv2 MIB start */
    ripQueryResponse = 0;
    ripRespBadPkts = 0;
    ripRespBadRoutes = 0;
    /* LVL7_MOD: RFC 1724 RIPv2 MIB end */

    input((struct sockaddr_in *)&from, sifp, aifp, &inbuf.pbuf.rip, cc);

    /* LVL7_MOD: RFC 1724 RIPv2 MIB start */
    rip_glob_mibstats.queries += ripQueryResponse;
    if (aifp) {
      if (inbuf.pbuf.rip.rip_cmd == RIPCMD_RESPONSE)
      {
        aifp->int_mibstats.ibadpkts += ripRespBadPkts;
        aifp->int_mibstats.ibadroutes += ripRespBadRoutes;
      }
      aifp->int_rip_pkts_rcvd++;   /* NOTE: not an official MIB stat */
    }
    /* LVL7_MOD: RFC 1724 RIPv2 MIB end */
    }
}


/* Process a RIP packet
 */
static void
input(struct sockaddr_in *from,     /* received from this IP address */
      struct interface *sifp,       /* interface of incoming socket */
      struct interface *aifp,       /* "authenticated" interface */
      struct rip *rip,
      int cc)
{
#   define FROM_NADDR osapiNtohl(from->sin_addr.s_addr)
    static struct msg_limit use_auth, bad_len, bad_mask;
    static struct msg_limit unk_router, bad_router, bad_nhop;

    struct rt_entry *rt;
    struct rt_spare new;
    struct netinfo *n, *lim;
    struct interface *ifp1;
    naddr gate, mask, dst;
    struct auth *ap;
    struct tgate *tg = 0;
    struct tgate_net *tn;
    int i;
    int send_ver_mismatch = 0;            /* LVL7_MOD */
    L7_uchar8 strIfAddr[OSAPI_INET_NTOA_BUF_SIZE];


    memset(&use_auth, 0, sizeof(struct msg_limit));
    memset(&bad_len, 0, sizeof(struct msg_limit));
    memset(&bad_mask, 0, sizeof(struct msg_limit));
    memset(&unk_router, 0, sizeof(struct msg_limit));
    memset(&bad_router, 0, sizeof(struct msg_limit));
    memset(&bad_nhop, 0, sizeof(struct msg_limit));

    /* Notice when we hear from a remote gateway
     */
    if (aifp != 0
        && (aifp->int_state & IS_REMOTE))
        aifp->int_act_time = now.tv_sec;

    trace_rip("Recv", "from", from, sifp, rip, cc);

    if(sifp)
      osapiInetNtoa((L7_uint32)sifp->int_addr, strIfAddr);
    else
      strcpy(strIfAddr, "Unknown");

    RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Received rip pkt on intf %s from %s.", 
                    strIfAddr, naddr_ntoa(FROM_NADDR));

    ripRespBadPkts = 1;                 /* LVL7_MOD: RFC 1724 RIPv2 MIB */

    if ((rip->rip_vers == 0) || (rip->rip_vers > RIPv2)) {
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Received rip pkt with bad ver from %s.", naddr_ntoa(FROM_NADDR));
      return;
    }

    if (cc > (int)OVER_MAXPACKETSIZE) {
        msglim(&bad_router, FROM_NADDR,
               "RIP packet at least %d bytes too long received from %s\n",
               cc-MAXPACKETSIZE, naddr_ntoa(FROM_NADDR));
        return;
    }

    n = rip->rip_nets;
    lim = (struct netinfo *)((char*)rip + cc);

  /* LVL7_MOD start */
  /* Check that the number of route entries does not exceed the
   * maximum allowed, otherwise discard entire packet.
   */
  if ((n + RIP_MAXNETENTRIES) < lim) {
    msglim(&bad_router, FROM_NADDR,
           "RIP packet contains too many route entries, cmd %d, received from %s\n",
           rip->rip_cmd, naddr_ntoa(FROM_NADDR));
    return;
  }

  /* Check the unused field after the version field. It must be zero. The
   * rfc's 1058, 1723, 2453 alternate between saying, it must be zero and unused.
   * We will force that it must be zero only for RIPv1 
   * For  RIPv2 we will take the value and not discard the packet.
   */
  if ((rip->rip_res1 != 0) && (rip->rip_vers == RIPv1))
  {
    msglim(&bad_router, FROM_NADDR,
           "The must be zero field after version is nonzero, cmd %d, received from %s\n",
           rip->rip_cmd, naddr_ntoa(FROM_NADDR));
    return;
  }
  /* Check RIPv1 "must be zero" (MBZ) fields.
   * Per RFC 1028, the RIPv1 MBZ fields are to be checked prior to
   * processing the RIP command.  Any MBZ field that is nonzero causes
   * the entire packet to be rejected.
   */
  if (rip->rip_vers == RIPv1) {
    int bad = 0;
    struct netinfo *n2 = n;

    if (rip->rip_res1 != 0)
      bad = 1;

    while (n2 < lim) {
      if (n2->n_tag != 0
          || n2->n_mask != 0
          || n2->n_nhop != 0) {
        bad = 1;
        break;
      }
      n2++;
    }

    if (bad != 0) {
      msglim(&bad_router, FROM_NADDR,
             "RIPv1 must be zero field is nonzero, cmd %d, received from %s\n",
             rip->rip_cmd, naddr_ntoa(FROM_NADDR));
      return;
    }
  }
  /* LVL7_MOD end */
  
  if (sifp)
  {
    ripMapDebugPacketRxTrace(sifp->int_index,(char *)rip ,cc, strIfAddr, naddr_ntoa(FROM_NADDR));
  }

  switch (rip->rip_cmd) {

  case RIPCMD_REQUEST:

        /* For mere requests, be a little sloppy about the source
         */
        if (aifp == 0)
            aifp = sifp;

        /* Are we talking to ourself or a remote gateway?
         */
        ifp1 = ifwithaddr(FROM_NADDR, 0, 1);
        if (ifp1) {
            if (ifp1->int_state & IS_REMOTE) {
                /* remote gateway */
                aifp = ifp1;
                if (check_remote(aifp)) {
                    aifp->int_act_time = now.tv_sec;
                    (void)if_ok(aifp, "remote ");
                }
            } else if (from->sin_port == osapiHtons(RIP_PORT)) {
                trace_pkt1("    discard our own RIP request");  /* LVL7_MOD */
                RIP_DEBUG_TRACE(RIP_TRACE_REQUEST,"discard our own RIP request received from %s",naddr_ntoa(FROM_NADDR));
                return;
            }
        }

    RIP_DEBUG_TRACE(RIP_TRACE_REQUEST,"Received rip REQUEST, received from %s",naddr_ntoa(FROM_NADDR));
    /* General mode checking
     */
    if (aifp != 0) {
      /* don't respond if send mode turned off */
      if (IS_RIP_OUT_OFF(aifp->int_state)) {
        trace_pkt1("    discard request while RIP not sending");
        RIP_DEBUG_TRACE(RIP_TRACE_REQUEST,"Discard request because RIP out is off on intf %s",naddr_ntoa(aifp->int_addr));
        return;
      }
      /* check request against current receive mode */
      if ((rip->rip_vers == RIPv2 && (aifp->int_state & IS_NO_RIPV2_IN))
          || (rip->rip_vers == RIPv1 && (aifp->int_state & IS_NO_RIPV1_IN))) {
        trace_pkt1("    discard request not matching receive version");
        RIP_DEBUG_TRACE(RIP_TRACE_REQUEST,"Request pkt ver does not match recv ver on intf %s",naddr_ntoa(aifp->int_addr));
        return;
      }

    }
        /* Did the request come from a router?
         */
        if (from->sin_port == osapiHtons(RIP_PORT)) {
            /* yes, ignore the request if RIP is off so that
             * the router does not depend on us;
       * also ignore if we are a silent process.
             */
            if (rip_sock < 0 || !supplier) {
                trace_pkt1("    discard request while RIP off or silent");
                if(aifp!=0)
                {
                  RIP_DEBUG_TRACE(RIP_TRACE_REQUEST,"Discard request, RIP is off on intf %s",naddr_ntoa(aifp->int_addr));
                }
                return;
      }
        }

        /* According to RFC 1723, we should ignore unauthenticated
         * queries.  That is too silly to bother with.  Sheesh!
         * Are forwarding tables supposed to be secret, when
         * a bad guy can infer them with test traffic?  When RIP
         * is still the most common router-discovery protocol
         * and so hosts need to send queries that will be answered?
         * What about `rtquery`?
         * Maybe on firewalls you'd care, but not enough to
         * give up the diagnostic facilities of remote probing.
         */

        if (n >= lim) {
            msglim(&bad_len, FROM_NADDR, "RIP: empty request from %s\n",
                   naddr_ntoa(FROM_NADDR));
            if(aifp!=0)
            {
              RIP_DEBUG_TRACE(RIP_TRACE_REQUEST,"Empty request from %s on intf %s",naddr_ntoa(FROM_NADDR),naddr_ntoa(aifp->int_addr));
            }
            return;
        }
        if ((cc%sizeof(*n) != sizeof(struct rip)%sizeof(*n)) ) {
            msglim(&bad_len, FROM_NADDR,
                   "RIP: request of bad length (%d) from %s\n",
                   cc, naddr_ntoa(FROM_NADDR));
            if(aifp!=0)
            {
              RIP_DEBUG_TRACE(RIP_TRACE_REQUEST,"Request pkt of bad length from %s on intf %s",naddr_ntoa(FROM_NADDR),naddr_ntoa(aifp->int_addr));
            }
        }

    /* Authorization checking is more involved than that of
     * the original code.
     */
    ap = find_auth(aifp);
    if (!ck_auth(aifp,rip,lim,FROM_NADDR,&use_auth,ap)) {
            trace_pkt1("    discard request due to auth failure");  /* LVL7_MOD */
            if(aifp!=0)
            {
              RIP_DEBUG_TRACE(RIP_TRACE_REQUEST,"Request disacrded due to auth failure from %s on intf %s",naddr_ntoa(FROM_NADDR),naddr_ntoa(aifp->int_addr));
            }
            return;
        }
        if (rip->rip_vers == RIPv2
            && (aifp == 0 || (aifp->int_state & IS_NO_RIPV1_OUT))) {
            v12buf.buf->rip_vers = RIPv2;
        } else {
            v12buf.buf->rip_vers = RIPv1;
            ap = 0;
        }

    /* Do not answer a RIP request if its version
     * does not match the current send mode (just
     * flag mismatch here and check it below).
     *
     * NOTE: Allow answering RIPv1 when send mode is
     *       RIPv1-compatibility.
     */
    if (rip->rip_vers == RIPv1) {
      if (aifp == 0
          || ((aifp->int_state & IS_NO_RIPV1_OUT)
              && !(aifp->int_state & IS_NO_RIP_MCAST))) {
        send_ver_mismatch = 1;
      }
    } else {
      if (aifp != 0
          && (aifp->int_state & IS_NO_RIPV2_OUT)) {
        send_ver_mismatch = 1;
      }
    }

        clr_ws_buf(&v12buf, ap);

        do {
            NTOHL(n->n_metric);

            /* A single entry with family RIP_AF_UNSPEC and
             * metric HOPCNT_INFINITY means "all routes".
             * We respond to routers only if we are acting
             * as a supplier, or to anyone other than a router
             * (i.e. a query).
             */

      RIP_DEBUG_TRACE(RIP_TRACE_REQUEST,"checking unicast Req metric %u family %u", n->n_metric, n->n_family);
      if (n->n_family == RIP_AF_UNSPEC
          && n->n_metric == HOPCNT_INFINITY) {

        /* make sure "all routes" request contains precisely one route entry */
        if ((n + 1) != lim) {
          msglim(&bad_len, FROM_NADDR,
                 "RIP all routes request packet from %s has invalid format\n",
                 naddr_ntoa(FROM_NADDR));
          if(aifp!=0)
          {
            RIP_DEBUG_TRACE(RIP_TRACE_REQUEST_DETAIL,"Bad all routes request packet from %s on intf %s",naddr_ntoa(FROM_NADDR),naddr_ntoa(aifp->int_addr));
          }
          return;
        }

        /* Note: not ready to call supply() function yet, even for
         *       source port other than 520 case.
         */

                /* A router trying to prime its tables.
                 * Filter the answer in the about same way
                 * broadcasts are filtered.
                 *
                 * Only answer a router if we are a supplier
                 * to keep an unwary host that is just starting
                 * from picking us as a router.
                 */
                if (aifp == 0) {
                    trace_pkt1("ignore distant router");  /* LVL7_MOD */
                    RIP_DEBUG_TRACE(RIP_TRACE_REQUEST_DETAIL,"Ignore distant router. Pkt from %s.",naddr_ntoa(FROM_NADDR));
                    return;
                }
                if (from->sin_port == htons(RIP_PORT)   /* LVL7_MOD */
                    && (!supplier
                        || IS_RIP_OFF(aifp->int_state))) {
                    trace_pkt1("ignore; not supplying");  /* LVL7_MOD */
                    RIP_DEBUG_TRACE(RIP_TRACE_REQUEST_DETAIL,"Ignore req. Not supplying on intf %s",naddr_ntoa(aifp->int_addr));
                    return;
                }

                /* Do not answer a RIPv1 router if
                 * we are sending only RIPv2.  But do offer
                 * poor man's router discovery.
         *
         * LVL7 NOTE: Allow answering RIPv1 when send mode is
         *            RIPv1-compatibility.
                 */
                if ((aifp->int_state & IS_NO_RIPV1_OUT)
            && !(aifp->int_state & IS_NO_RIP_MCAST)     /* LVL7_MOD */
                    && rip->rip_vers == RIPv1) {
                    if (!(aifp->int_state & IS_PM_RDISC)) {
                        trace_pkt1("ignore; sending RIPv2 only"); /* LVL7_MOD */
                        RIP_DEBUG_TRACE(RIP_TRACE_REQUEST_DETAIL,"Ignore req. Sending RIPv2 only on intf %s",naddr_ntoa(aifp->int_addr));
                        return;
                    }

                    v12buf.n->n_family = RIP_AF_INET;
                    v12buf.n->n_dst = RIP_DEFAULT;
                    i = aifp->int_d_metric;
                    if (0 != (rt = rtget(RIP_DEFAULT, 0)))
                      /* We should not add anything to the metric of the route
                         in the outgoing response messages.
                      */
                      i = MIN(i, rt->rt_metric);
                    v12buf.n->n_metric = osapiHtonl(i);
                    v12buf.n++;
                    break;
                }

        /* Don't respond if a version mismatch was detected.
         */
        if (send_ver_mismatch) {
          trace_pkt1("ignore; send version mismatch");
          RIP_DEBUG_TRACE(RIP_TRACE_REQUEST_DETAIL,"Ignore req. Send ver mismatch on intf %s",naddr_ntoa(aifp->int_addr));
          return;
        }

                /* Respond with version as determined above.
                 */
                supply(from, aifp, OUT_QUERY, 0,
                       v12buf.buf->rip_vers,
                       ap != 0);

                ripQueryResponse++;     /* LVL7_MOD: RFC 1724 RIPv2 MIB */

                return;
            }

            /* Ignore authentication */
            if (n->n_family == RIP_AF_AUTH)
                continue;

            if (n->n_family != RIP_AF_INET) {
                msglim(&bad_router, FROM_NADDR,
                       "RIP: request from %s for unsupported"
                       " (af %d) %s\n",
                       naddr_ntoa(FROM_NADDR),
                       osapiNtohs(n->n_family),
                       naddr_ntoa(osapiNtohl(n->n_dst)));
                RIP_DEBUG_TRACE(RIP_TRACE_REQUEST_DETAIL,"Bad req from %s for unsupported family %d.",naddr_ntoa(FROM_NADDR),osapiNtohs(n->n_family));
                return;
            }

            /* We are being asked about a specific destination.
             */
            dst = osapiNtohl(n->n_dst);
            if (!check_dst(dst)) {
                msglim(&bad_router, FROM_NADDR,
                       "RIP; bad queried destination %s from %s\n",
                       naddr_ntoa(dst),
                       naddr_ntoa(FROM_NADDR));
                RIP_DEBUG_TRACE(RIP_TRACE_REQUEST_DETAIL,"Bad queried dest %s from %s",naddr_ntoa(dst),naddr_ntoa(FROM_NADDR));
                return;
            }

      /* Don't respond if a version mismatch was detected.
       */
      if (send_ver_mismatch) {
        trace_pkt1("ignore; send version mismatch");
        if(aifp!=0)
        {
          RIP_DEBUG_TRACE(RIP_TRACE_REQUEST_DETAIL,"Ignore: Send ver mismatch on intf %s",naddr_ntoa(aifp->int_addr));
        }
        return;
      }

            /* decide what mask was intended */
            if (rip->rip_vers == RIPv1
                || 0 == (mask = osapiNtohl(n->n_mask))
                || 0 != (dst & ~mask))
                mask = ripv1_mask_host(dst, aifp);

            /* try to find the answer */
            rt = rtget(dst, mask);
            if (!rt && dst != RIP_DEFAULT)
                rt = rtfind(osapiNtohl(n->n_dst));

            if (v12buf.buf->rip_vers != RIPv1)
                v12buf.n->n_mask = osapiHtonl(mask);
            if (rt == 0) {
                /* we do not have the answer */
        /* supposed to provide same info as request, but with infinite metric */
        /* NOTE:  Originally planned just to copy the original RTE to the
         *        output buffer, but a response to a RIPv1 request packet
         *        with nonzero must-be-zero fields would cause the receiver
         *        to discard the entire packet.
         */
        v12buf.n->n_family = RIP_AF_INET;
        v12buf.n->n_dst = osapiHtonl(dst);
                if (v12buf.buf->rip_vers != RIPv1) {
          v12buf.n->n_tag = n->n_tag;
          v12buf.n->n_nhop = n->n_nhop;
        }
                v12buf.n->n_metric = HOPCNT_INFINITY;
            } else {
                /* we have the answer, so compute the
                 * right metric and next hop.
                 */
                v12buf.n->n_family = RIP_AF_INET;
                v12buf.n->n_dst = osapiHtonl(dst);
                v12buf.n->n_metric = rt->rt_metric;
                if (v12buf.n->n_metric > HOPCNT_INFINITY)
                    v12buf.n->n_metric = HOPCNT_INFINITY;
                if (v12buf.buf->rip_vers != RIPv1) {
                    v12buf.n->n_tag = osapiHtonl(rt->rt_tag);
                    v12buf.n->n_mask = osapiHtonl(mask);
                    if (aifp != 0
                        && on_net(rt->rt_gate,
                              aifp->int_net,
                              aifp->int_mask)
                        && rt->rt_gate != aifp->int_addr)
                        v12buf.n->n_nhop = osapiHtonl(rt->rt_gate);
                }
            }
            HTONL(v12buf.n->n_metric);

            /* Stop paying attention if we fill the output buffer.
             */
            if (++v12buf.n >= v12buf.lim)
                break;
        } while (++n < lim);

        /* Send the answer about specific routes.
         */
        if (ap != 0 && ap->type == RIP_AUTH_MD5)
            end_md5_auth(&v12buf, ap);

    /* LVL7_MOD start */
    {
    int res = 0;
        if (from->sin_port != osapiHtons(RIP_PORT)) {
            /* query */
            if (aifp != 0)
              res = output(OUT_QUERY, from, aifp,     /* LVL7_MOD: RFC 1724 RIPv2 MIB */
                       v12buf.buf,
                       ((char *)v12buf.n - (char*)v12buf.buf));
        } else if (supplier) {
            if (aifp != 0)
              res = output(OUT_UNICAST, from, aifp,   /* LVL7_MOD: RFC 1724 RIPv2 MIB */
                       v12buf.buf,
                       ((char *)v12buf.n - (char*)v12buf.buf));
        } else {
            /* Only answer a router if we are a supplier
             * to keep an unwary host that is just starting
             * from picking us an a router.
             */
            ;
        }
    if (res > 0)                      /* LVL7_MOD: RFC 1724 RIPv2 MIB */
      ripQueryResponse++;
    }
    /* LVL7_MOD end */
        return;


    case RIPCMD_TRACEON:
  case RIPCMD_TRACEOFF:

        /* Notice that trace messages are turned off for all possible
         * abuse if _PATH_TRACE is undefined in pathnames.h.
         * Notice also that because of the way the trace file is
         * handled in trace.c, no abuse is plausible even if
         * _PATH_TRACE_ is defined.
         *
         * First verify message came from a privileged port. */
        if (osapiNtohs(from->sin_port) > IPPORT_RESERVED) {
            msglog("RIP trace command from untrusted port on %s\n",
                   naddr_ntoa(FROM_NADDR));
            return;
        }
        if (aifp == 0) {
            msglog("RIP trace command from unknown router %s\n",
                   naddr_ntoa(FROM_NADDR));
            return;
        }
        if (rip->rip_cmd == RIPCMD_TRACEON) {
            rip->rip_tracefile[0] = '\0';
            set_tracefile((char*)rip->rip_tracefile,
                      "trace command: %s\n", 0);
        } else {
            trace_off("tracing turned off by %s",
                  naddr_ntoa(FROM_NADDR));
        }
        return;


  case RIPCMD_RESPONSE:

        if (cc%sizeof(*n) != sizeof(struct rip)%sizeof(*n)) {
            msglim(&bad_len, FROM_NADDR,
                   "RIP response of bad length (%d) from %s\n",
                   cc, naddr_ntoa(FROM_NADDR));
            return;                           /* LVL7_MOD (fixed possible bug) */
        }

        /* verify message came from a router */
        if (from->sin_port != osapiHtons(RIP_PORT)) {
            msglim(&bad_router, FROM_NADDR,
                   "    discard RIP response from unknown port"
                   " %d\n", osapiNtohs(from->sin_port));
            return;
        }

        if (rip_sock < 0) {
            trace_pkt1("    discard response while RIP off");  /* LVL7_MOD */
            RIP_DEBUG_TRACE(RIP_TRACE_RESPONSE,"Response disacrded. RIP is off. From %s",naddr_ntoa(FROM_NADDR));
            return;
        }

        /* Are we talking to ourself or a remote gateway?
         */
        ifp1 = ifwithaddr(FROM_NADDR, 0, 1);
        if (ifp1) {
            if (ifp1->int_state & IS_REMOTE) {
                /* remote gateway */
                aifp = ifp1;
                if (check_remote(aifp)) {
                    aifp->int_act_time = now.tv_sec;
                    (void)if_ok(aifp, "remote ");
                }
            } else {
                trace_pkt1("    discard our own RIP response");  /* LVL7_MOD */
                RIP_DEBUG_TRACE(RIP_TRACE_RESPONSE,"Discard our own RIP response. From %s",naddr_ntoa(FROM_NADDR));
                /* don't treat reflections of our own response as a bad pkt */
                ripRespBadPkts = 0;             /* LVL7_MOD: RFC 1724 RIPv2 MIB */
                return;
            }
        }

        /* Accept routing packets from routers directly connected
         * via broadcast or point-to-point networks, and from
         * those listed in /etc/gateways.
         */
        if (aifp == 0) {
          /* supress logging for messages received on the service port */
          if(simGetSystemIPNetMask() && simGetSystemIPAddr() != FROM_NADDR && simGetSystemIPNetMask())
          {            
            msglim(&unk_router, FROM_NADDR,
                   "   RIP : discard response from %s"
                   " via unexpected interface\n",
                   naddr_ntoa(FROM_NADDR));
            RIP_DEBUG_TRACE(RIP_TRACE_RESPONSE,"RIP : discard response from %s via system interface",naddr_ntoa(FROM_NADDR));
          }
          else
          {
            RIP_DEBUG_TRACE(RIP_TRACE_RESPONSE,"RIP : discard response from %s via unexpected interface",naddr_ntoa(FROM_NADDR));
          }

          return;
        }
        if (IS_RIP_IN_OFF(aifp->int_state)) {
            trace_pkt("    discard RIPv%d response"
                  " via disabled interface %s",
                  rip->rip_vers, aifp->int_name);
            RIP_DEBUG_TRACE(RIP_TRACE_RESPONSE,"Response disacrded via disabled interface. From %s",naddr_ntoa(FROM_NADDR));
            return;
        }

        if (n >= lim) {
            msglim(&bad_len, FROM_NADDR, "RIP: empty response from %s\n",
                   naddr_ntoa(FROM_NADDR));
            return;
        }

        if (((aifp->int_state & IS_NO_RIPV1_IN)
             && rip->rip_vers == RIPv1)
            || ((aifp->int_state & IS_NO_RIPV2_IN)
            && rip->rip_vers != RIPv1)) {
            trace_pkt("    discard RIPv%d response",
                  rip->rip_vers);
            RIP_DEBUG_TRACE(RIP_TRACE_RESPONSE,"Discard RIPv%d response. From %s",rip->rip_vers, naddr_ntoa(FROM_NADDR));
            return;
        }

        /* Ignore routes via dead interface.
         */
        if (aifp->int_state & IS_BROKE) {
            trace_pkt("discard response via broken interface %s",
                  aifp->int_name);
            RIP_DEBUG_TRACE(RIP_TRACE_RESPONSE,"Discard response via broken interface. From %s",naddr_ntoa(FROM_NADDR));
            return;
        }

        /* If the interface cares, ignore bad routers.
         * Trace but do not log this problem, because where it
         * happens, it happens frequently.
         */
        if (aifp->int_state & IS_DISTRUST) {
            tg = tgates;
            while (tg->tgate_addr != FROM_NADDR) {
                tg = tg->tgate_next;
                if (tg == 0) {
                    trace_pkt("    discard RIP response"
                          " from untrusted router %s",
                          naddr_ntoa(FROM_NADDR));
                    RIP_DEBUG_TRACE(RIP_TRACE_RESPONSE,"Discard RIP response from untrusted router. From %s",naddr_ntoa(FROM_NADDR));
                    return;
                }
            }
        }

    ripRespBadPkts = 0;
    ripRespBadRoutes = ((char *)lim - (char *)n) / sizeof(*n);
    if (n->n_family == RIP_AF_AUTH)
      ripRespBadRoutes--;

    /* Authorization checking is more involved than that of
     * the original code.
     */
    ap = find_auth(aifp);
    if (!ck_auth(aifp,rip,lim,FROM_NADDR,&use_auth,ap))
    {
      RIP_DEBUG_TRACE(RIP_TRACE_RESPONSE,"Auth failure. From %s",naddr_ntoa(FROM_NADDR));
      return;
    }
    
        do {
            if (n->n_family == RIP_AF_AUTH)
                continue;

            dst = osapiNtohl(n->n_dst);
            if (n->n_family != RIP_AF_INET
                && (n->n_family != RIP_AF_UNSPEC
                || dst != RIP_DEFAULT)) {
                msglim(&bad_router, FROM_NADDR,
                       "RIP: route from %s to unsupported"
                       " address family=%ld destination=%s\n",
                       naddr_ntoa(FROM_NADDR),
                       osapiNtohl(n->n_family),
                       naddr_ntoa(dst));
                continue;
            }
            if (!check_dst(dst)) {
                msglim(&bad_router, FROM_NADDR,
                       "RIP: bad destination %s from %s\n",
                       naddr_ntoa(dst),
                       naddr_ntoa(FROM_NADDR));
                continue;
            }
            if (n->n_metric == 0
                || n->n_metric > osapiHtonl(HOPCNT_INFINITY)) {
                msglim(&bad_router, FROM_NADDR,
                       "RIP: bad metric %ld from %s"
                       " for destination %s\n",
                       osapiNtohl(n->n_metric),
                       naddr_ntoa(FROM_NADDR),
                       naddr_ntoa(dst));
                continue;
            }
            /* Check the validity of the mask of the advertised route */
            if (n->n_mask != 0)
            {
              u_int temp_mask = osapiNtohl(n->n_mask);

              while ((temp_mask & 0x80000000) != 0)
                temp_mask = temp_mask << 1;      /* Left shift and fill LSB w/0 */
              if (temp_mask != 0){
                RIP_DEBUG_TRACE(RIP_TRACE_RESPONSE_DETAIL,"Bad netmask for dst %s, mask %lX. From %s",naddr_ntoa(dst), osapiNtohl(n->n_mask), naddr_ntoa(FROM_NADDR));
                continue;
              }
            }
            /* Notice the next-hop.
             */
            gate = FROM_NADDR;
            if (n->n_nhop != 0) {
                if (rip->rip_vers == RIPv1) {
                    n->n_nhop = 0;
                } else {
                    /* Use it only if it is valid. */
                    if (on_net(osapiNtohl(n->n_nhop),
                           aifp->int_net, aifp->int_mask)
                    && check_dst(osapiNtohl(n->n_nhop))) {
                        gate = osapiNtohl(n->n_nhop);
                    } else {
                        msglim(&bad_nhop, FROM_NADDR,
                           "RIP: router %s to %s"
                           " has bad next hop %s\n",
                           naddr_ntoa(FROM_NADDR),
                           naddr_ntoa(dst),
                           naddr_ntoa(osapiNtohl(n->n_nhop)));
                        n->n_nhop = 0;
                    }
                }
            }

            if (rip->rip_vers == RIPv1
                || 0 == (mask = ntohl(n->n_mask))) {
                mask = ripv1_mask_host(dst,aifp);
            } else if ((dst & ~mask) != 0) {
                msglim(&bad_mask, FROM_NADDR,
                       "RIP: router %s sent bad netmask"
                       " %#lx with %s\n",
                       naddr_ntoa(FROM_NADDR),
                       (u_long)mask,
                       naddr_ntoa(dst));
                continue;
            }
            if (rip->rip_vers == RIPv1)
                n->n_tag = 0;


      /* Reject the host routes if we are configured to not to accept them */
      if ((mask == HOST_MASK) &&
          (ripHostRoutesAccept == HOSTROUTESACCEPT_OFF))
      {
        continue;
      }
      /* Reject an all-ones subnet mask presented in a RIP packet
       * Note: This is primarily for RIPv2, since RIPv1 would have already
       *       failed the "must be zero" test above
       */
      /* Reject an all-ones subnet mask only if the address is not host address */
      if ((osapiNtohl(n->n_mask) == HOST_MASK) &&
         (((dst & 0x000000ffL) == 0) ||
         ((dst & 0x000000ffL) == 0xff))) {
        msglim(&bad_router, FROM_NADDR,
               "RIP: router %s sent all-ones net mask for"
               " route %s\n",
               naddr_ntoa(FROM_NADDR),
               naddr_ntoa(dst));
        continue;
      }

            /* Don't accept an all-ones host portion as a route
       * Note: The all-ones limited bcast addr is rejected by check_dst() above
       */
      {
        naddr masknet;
        if (rip->rip_vers == RIPv1
            || 0 == (masknet = osapiNtohl(n->n_mask)))
          masknet = ripv1_mask_net(dst,aifp);
        if (masknet != HOST_MASK) {
          if ((dst & ~masknet) == ~masknet) {
            msglim(&bad_router, FROM_NADDR,
                   "RIP: router %s sent net-directed bcast"
                   " route %s\n",
                   naddr_ntoa(FROM_NADDR),
                   naddr_ntoa(dst));
            continue;
          }
        }
      }

            /* Adjust metric according to incoming interface..
             */
            NTOHL(n->n_metric);
            n->n_metric += aifp->int_metric;
            if (n->n_metric > HOPCNT_INFINITY)
                n->n_metric = HOPCNT_INFINITY;

            /* Should we trust this route from this router? */
            if (tg && (tn = tg->tgate_nets)->mask != 0) {
                for (i = 0; i < MAX_TGATE_NETS; i++, tn++) {
                    if (on_net(dst, tn->net, tn->mask)
                        && tn->mask <= mask)
                        break;
                }
                if (i >= MAX_TGATE_NETS || tn->mask == 0) {
                    trace_pkt("   ignored unauthorized %s",
                          addrname(dst,mask,0));
                    RIP_DEBUG_TRACE(RIP_TRACE_RESPONSE_DETAIL,"ignored unauthorized %s",addrname(dst,mask,0));
                    continue;
                }
            }

            /* Recognize and ignore a default route we faked
             * which is being sent back to us by a machine with
             * broken split-horizon.
             * Be a little more paranoid than that, and reject
             * default routes with the same metric we advertised.
             */
            if (aifp->int_d_metric != 0
                && dst == RIP_DEFAULT
                && (int)n->n_metric >= aifp->int_d_metric)
                continue;

            /* If this is a default route, set the netmask to
             * all zeroes, regardless of what came in the packet.
             */
            if (dst == RIP_DEFAULT) {
                mask = 0;
            }

            i = 0; /* do not de-aggregate (defect 27823) */
            new.rts_gate = gate;
            new.rts_router = FROM_NADDR;
            new.rts_metric = n->n_metric;
            new.rts_tag = osapiNtohl(n->n_tag);
            new.rts_time = now.tv_sec;
            new.rts_ifp = aifp;
            new.rts_de_ag = i;
            new.rts_redist_proto = RIP_RT_REDIST_FIRST; /* Redistributed route protocol */

            input_route(dst, mask, &new, n);

      if (ripRespBadRoutes > 0)         /* LVL7_MOD: RFC 1724 RIPv2 MIB */
        ripRespBadRoutes--;
        } while (++n < lim);
        break;
    }
#undef FROM_NADDR
}

/* Process a single input route.
 */
static void
input_route(naddr dst,          /* network order */
        naddr mask,
        struct rt_spare *new,
        struct netinfo *n)
{
    int i;
    struct rt_entry *rt;
    struct rt_spare *rts, *rts0;
    struct interface *ifp1;
    L7_BOOL isRedistRoute = L7_FALSE;

    /* See if the other guy is telling us to send our packets to him.
     * Sometimes network routes arrive over a point-to-point link for
     * the network containing the address(es) of the link.
     *
     * If our interface is broken, switch to using the other guy.
     */
    ifp1 = ifwithaddr(dst, 1, 1);
    if (ifp1 != 0
        && (!(ifp1->int_state & IS_BROKE)
        || (ifp1->int_state & IS_PASSIVE)))
        return;

    /* Look for the route in our table.
     */
    rt = rtget(dst, mask);

    /* Consider adding the route if we do not already have it.
     */
    if (rt == 0) {
        /* Ignore unknown routes being poisoned.
         */
        if (new->rts_metric == HOPCNT_INFINITY)
            return;

        /* Ignore the route if it points to us */
        if (n->n_nhop != 0
            && 0 != ifwithaddr(osapiNtohl(n->n_nhop), 1, 0))
            return;

        /* If something has not gone crazy and tried to fill
         * our memory, accept the new route.
         */
        rtadd(dst, mask, 0, new);

        return;
    }

    /* We already know about the route.  Consider this update.
     *
     * If (rt->rt_state & RS_NET_SYN), then this route
     * is the same as a network route we have inferred
     * for subnets we know, in order to tell RIPv1 routers
     * about the subnets.
     *
     * It is impossible to tell if the route is coming
     * from a distant RIPv2 router with the standard
     * netmask because that router knows about the entire
     * network, or if it is a round-about echo of a
     * synthetic, RIPv1 network route of our own.
     * The worst is that both kinds of routes might be
     * received, and the bad one might have the smaller
     * metric.  Partly solve this problem by never
     * aggregating into such a route.  Also keep it
     * around as long as the interface exists.
     */

    rts0 = rt->rt_spares;
    for (rts = rts0, i = NUM_SPARES; i != 0; i--, rts++) {
        /* Looking for a route from same RIP neighbor. Skip
         * redistributed routes that happen to be from the same neighbor. */
        isRedistRoute = ((rts->rts_redist_proto > RIP_RT_REDIST_FIRST) &&
                         (rts->rts_redist_proto < RIP_RT_REDIST_LAST));
        if ((rts->rts_router == new->rts_router) && !isRedistRoute)
            break;
        /* Note the worst slot to reuse, other than the current slot. */
        if (rts0 == rt->rt_spares
            || BETTER_LINK(rt, rts0, rts))
            rts0 = rts;
    }
    if (i != 0) {
        /* Found a route from the router already in the table.
         */

        /* If the new route is a route broken down from an
         * aggregated route, and if the previous route is either
         * not a broken down route or was broken down from a finer
         * netmask, and if the previous route is current,
         * then forget this one.
         */
        if (new->rts_de_ag > rts->rts_de_ag
            && now_stale <= rts->rts_time)
            return;

        /* Keep poisoned routes around only long enough to pass
         * the poison on.  Use a new timestamp for good routes.
         */
        if (rts->rts_metric == HOPCNT_INFINITY
            && new->rts_metric == HOPCNT_INFINITY)
            new->rts_time = rts->rts_time;

        /* If this is an update for the router we currently prefer,
         * then note it.
         */
        if (i == NUM_SPARES) {
            rtchange(rt, rt->rt_state, new, 0);
            /* If the route got worse, check for something better.
             */
            if (new->rts_metric > rts->rts_metric)
                rtswitch(rt, 0);
            return;
        }
#if 0
        /* We need to check if the existing preferred route 
         * has become stale in which case the advertised
         * and already existing spare route with the same metric
         * might take the priority
         * Hence commenting the checks below, and avoiding the return 
         * from here when the advertised spare route with the same
         * metric is received.
         */

        /* This is an update for a spare route.
         * Finished if the route is unchanged.
         */
        if (rts->rts_gate == new->rts_gate
            && rts->rts_metric == new->rts_metric
            && rts->rts_tag == new->rts_tag) {
            trace_upslot(rt, rts, new);
            *rts = *new;
            return;
        }
#endif
        /* Forget it if it has gone bad.
         */
        if (new->rts_metric == HOPCNT_INFINITY) {
            rts_delete(rt, rts);
            return;
        }

    } else {
        /* The update is for a route we know about,
         * but not from a familiar router.
         *
         * Ignore the route if it points to us.
         */
        if (n->n_nhop != 0
            && 0 != ifwithaddr(osapiNtohl(n->n_nhop), 1, 0))
            return;

        /* the loop above set rts0=worst spare */
        rts = rts0;

        /* Save the route as a spare only if it has
         * a better metric than our worst spare.
         * This also ignores poisoned routes (those
         * received with metric HOPCNT_INFINITY).
         */
        if (new->rts_metric >= rts->rts_metric)
            return;
    }

    trace_upslot(rt, rts, new);
    /* if replacing a redistributed route, clear the redist flag */
    if ((rts->rts_redist_proto > RIP_RT_REDIST_FIRST) &&
        (rts->rts_redist_proto < RIP_RT_REDIST_LAST))
    {
      rt->rt_state &= ~RS_REDIST;
    }
    *rts = *new;

    /* try to switch to a better route */
    rtswitch(rt, rts);
}


static int              /* 0 if bad */
ck_passwd(struct interface *aifp,
      struct rip *rip,
      void *lim,
      naddr from,
      struct msg_limit *use_authp)
{
#   define NA (rip->rip_auths)
    struct netauth *na2;
    struct auth *ap;
    MD5_CTX md5_ctx;
    u_char hash[RIP_AUTH_PW_LEN];
    int i, len;


    if ((void *)NA >= lim || NA->a_family != RIP_AF_AUTH) {
        msglim(use_authp, from, "missing password from %s",
               naddr_ntoa(from));
        return 0;
    }

    /* accept any current (+/- 24 hours) password
     */
    for (ap = aifp->int_auth, i = 0; i < MAX_AUTH_KEYS; i++, ap++) {
        if (ap->type != osapiNtohs(NA->a_type)
            || (u_long)ap->start > (u_long)clk.tv_sec+DAY
            || (u_long)ap->end < (u_long)clk.tv_sec - MIN((u_long)clk.tv_sec, DAY)
             )
            continue;

        if (NA->a_type == RIP_AUTH_PW) {
            if (!memcmp(NA->au.au_pw, ap->key, RIP_AUTH_PW_LEN))
                return 1;

        } else {
            /* accept MD5 secret with the right key ID
             */
            if (NA->au.a_md5.md5_keyid != ap->keyid)
                continue;

            len = ntohs(NA->au.a_md5.md5_pkt_len);
            if ((len-sizeof(*rip)) % sizeof(*NA) != 0
                || len != (char *)lim-(char*)rip-(int)sizeof(*NA)) {
                msglim(use_authp, from,
                       "wrong MD5 RIPv2 packet length of %d"
                       " instead of %d from %s\n",
                       len, (int)((char *)lim-(char *)rip
                          -sizeof(*NA)),
                       naddr_ntoa(from));
                return 0;
            }
            na2 = (struct netauth *)((char *)rip+len);

            /* Given a good hash value, these are not security
             * problems so be generous and accept the routes,
             * after complaining.
             */
            if (TRACEPACKETS) {
                  /* common practice uses an auth length of 20 instead of 16 */
                  if ((NA->au.a_md5.md5_auth_len != RIP_AUTH_MD5_LEN) &&
                      (NA->au.a_md5.md5_auth_len != RIP_AUTH_ENTRY_LEN))
                    msglim(use_authp, from,
                           "unknown MD5 RIPv2 auth len %#x"
                           " instead of %#x from %s\n",
                           NA->au.a_md5.md5_auth_len,
                           RIP_AUTH_MD5_LEN,
                           naddr_ntoa(from));
                if (na2->a_family != RIP_AF_AUTH)
                    msglim(use_authp, from,
                           "unknown MD5 RIPv2 family %#x"
                           " instead of %#x from %s\n",
                           osapiHtons(na2->a_family), RIP_AF_AUTH,
                           naddr_ntoa(from));
                if (na2->a_type != osapiHtons(1))
                    msglim(use_authp, from,
                           "MD5 RIPv2 hash has %#x"
                           " instead of %#x from %s\n",
                           osapiNtohs(na2->a_type), 1,
                           naddr_ntoa(from));
            }

            MD5Init(&md5_ctx);
            /* include 4-byte header of trailing auth data entry in digest */
            MD5Update(&md5_ctx, (u_char *)rip, len+RIP_AUTH_ENTRY_HDR_LEN);
            MD5Update(&md5_ctx, ap->key, RIP_AUTH_MD5_LEN);
            MD5Final(hash, &md5_ctx);
            if (!memcmp(hash, na2->au.au_pw, sizeof(hash)))
                return 1;
        }
    }

    msglim(use_authp, from, "RIP: bad password from %s\n",
           naddr_ntoa(from));
    return 0;
#undef NA
}

/* LVL7_MOD start */
static int seqnoFromNbrIsValid(L7_uchar8 keyid, L7_uint32 seqno,
                               L7_uint32 nbr)
{
  int i=0, j=0, nbr_lost=0;

  /* delete the neighbors with whom connectivity is lost */
  for(i=0; i<NUM_RIP_NBRS; i++)
  {
    if((nbr_data[i].nbr_router != 0) &&
       ((now.tv_sec - nbr_data[i].lastMsgRcvd) > 50))
       /* ((now.tv_sec - nbr_data[i].lastMsgRcvd) > EXPIRE_TIME)) */
    {
      memset(&nbr_data[i], 0, sizeof(struct nbr_specific_data));
      nbr_lost = 1;
    }
  }

  if(nbr_lost == 1)
  {
    /* remove the holes in the array */
    memset(tmp_data, 0, sizeof(tmp_data));
    for(i=0; i<NUM_RIP_NBRS; i++)
    {
      if(nbr_data[i].nbr_router != 0)
      {
        memcpy(&tmp_data[j], &nbr_data[i], sizeof(struct nbr_specific_data));
        j++;
      }
    }
    memcpy(nbr_data, tmp_data, sizeof(tmp_data));
  } 

  /* check the md5 seqNo in the received message for validity */
  for(i=0; i<NUM_RIP_NBRS; i++)
  {
    if(nbr_data[i].nbr_router == 0)
    {
      /* entry with nbr_router field as 0 is the first available entry in the list */
      nbr_data[i].nbr_router = nbr;
      nbr_data[i].md5_keyid = keyid;
      nbr_data[i].md5_seqno = seqno;
      nbr_data[i].lastMsgRcvd = now.tv_sec;
      return 1;
    }

    if(nbr_data[i].nbr_router == nbr)
    {
      if(nbr_data[i].md5_keyid == keyid)
      {
        if(seqno < nbr_data[i].md5_seqno)
          return 0;
      }
      nbr_data[i].md5_keyid = keyid;
      nbr_data[i].md5_seqno = seqno;
      nbr_data[i].lastMsgRcvd = now.tv_sec;
      return 1;
    }
  }

  /* if it comes here, it means the neighbor list is full, return failure */
  return 0;
}
/* LVL7_MOD end */

/* RIPv2 authentication checking:
 *  - if RIPv1 router, accept all RIP msgs
 *  - if RIPv2 router, only accept matching authentications:
 *      1) router config NONE: all RIPv1 msgs and RIPv2 with no auth entry
 *      2) router config PW:   ONLY RIPv2 msgs with auth type 2 (password)
 *      3) router config MD5:  ONLY RIPv2 msgs with auth type 3 (keyed MD5)
 *
 * NOTE:  The auth_ok flag is used to relax the checking such that case 1
 *        accepts authenticated RIPv2 msgs unconditionally, while cases 2
 *        and 3 accept unauthenticated RIPv2 msgs.
 */
static int              /* 0 if bad */
ck_auth(struct interface *aifp,
      struct rip *rip,
      void *lim,
      naddr from,
      struct msg_limit *use_authp,
        struct auth *ap)
{
    int rtr_ver, msg_ver, if_auth, msg_auth;
    struct netauth *na, *na2;
/* LVL7_MOD start */
    L7_uchar8 msg_md5_keyid;
    L7_uint32 msg_md5_seqno;
/* LVL7_MOD end */

    /* use the interface send version as the router RIP version of interest
     */
    rtr_ver = (aifp != 0 && (aifp->int_state & IS_NO_RIPV2_OUT)) ? RIPv1 : RIPv2;

    /* accept all msgs if router receive mode is version 1 only */
    if (rtr_ver == RIPv1)
        return 1;

    msg_ver = (rip->rip_vers == RIPv1) ? RIPv1 : RIPv2;
    if_auth = (ap == 0) ? RIP_AUTH_NONE : ap->type;

    /* determine RIP message authentication type */
    na = rip->rip_auths;
    if (msg_ver == RIPv2 && na->a_family == RIP_AF_AUTH) {
        msg_auth = osapiNtohs(na->a_type);

/* LVL7_MOD start */
        msg_md5_keyid = na->au.a_md5.md5_keyid;
        msg_md5_seqno = osapiNtohl(na->au.a_md5.md5_seqno);

        if(! seqnoFromNbrIsValid(msg_md5_keyid, msg_md5_seqno, from))
        {
           msglim(use_authp, from, "RIP: packet from %s"
                                   " failed md5 seqno checking\n",
                                   naddr_ntoa(from));
           return 0;
        }
/* LVL7_MOD end */

        /* should only be one RIP_AF_AUTH entry in RIP msg EXCEPT for
         * keyed MD5 trailer per RFC 2082
         */
        na2 = na;
        while (++na2 < (struct netauth *)lim) {
            if (na2->a_family == RIP_AF_AUTH) {
                /* only allowed as last entry for keyed MD5 message */
                if (msg_auth == RIP_AUTH_MD5
                        && na2->a_type == osapiHtons(0x0001)   /* per RFC 2082 */
                        && (na2+1) >= (struct netauth *)lim)
                {
                  ripRespBadRoutes--;
                  continue;
                }
                msglim(use_authp, from, "RIP: packet from %s"
                             " has multiple authentication entries\n",
                             naddr_ntoa(from));
                return 0;
            }
        }
    } else {
        /* message auth type none must be inferred (not explicitly included in msg) */
        msg_auth = RIP_AUTH_NONE;
    }

    /* case 1: configured for NONE */
    if (if_auth == RIP_AUTH_NONE) {
      if (msg_auth == RIP_AUTH_NONE)
            return 1;
    /* cases 2-3: configured for PW or MD5 */
    } else {
      if (msg_auth != RIP_AUTH_NONE)
            return ck_passwd(aifp,rip,lim,from,use_authp);
    }

    /* accept packet for fall-thru cases if relaxed auth checking desired
     * (i.e., 'routed' -A option NOT specified)
     *
     * Note: this option does not comply with RFC 1723, but facilitates router
     *       discovery
     */
    if (auth_ok)
        return 1;

    msglim(use_authp, from, "RIP: packet from %s"
                 " failed authentication checking\n",
           naddr_ntoa(from));
    return 0;
}

