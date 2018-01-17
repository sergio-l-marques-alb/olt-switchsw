/*
 * Copyright (c) 1983, 1993
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
 * $FreeBSD: src/sbin/routed/if.c,v 1.6.2.1 2000/08/14 17:00:03 sheldonh Exp $
 */

#include "defs.h"
#include "pathnames.h"
#include "osapi_support.h"
#include <net/if.h>

#if !defined(sgi) && !defined(__NetBSD__)
static char sccsid[] __attribute__((unused)) = "@(#)if.c    8.1 (Berkeley) 6/5/93";
#elif defined(__NetBSD__)
#include <sys/cdefs.h>
__RCSID("$NetBSD$");
#endif

struct interface *ifnet;        /* all interfaces */

/* hash table for all interfaces, big enough to tolerate ridiculous
 * numbers of IP aliases.  Crazy numbers of aliases such as 7000
 * still will not do well, but not just in looking up interfaces
 * by name or address.
 */
#define AHASH_LEN 211           /* must be prime */
#define AHASH(a) &ahash_tbl[(a)%AHASH_LEN]
struct interface *ahash_tbl[AHASH_LEN];

#define BHASH_LEN 211           /* must be prime */
#define BHASH(a) &bhash_tbl[(a)%BHASH_LEN]
struct interface *bhash_tbl[BHASH_LEN];

struct interface *remote_if;        /* remote interfaces */

/* hash for physical interface names.
 * Assume there are never more 100 or 200 real interfaces, and that
 * aliases are put on the end of the hash chains.
 */
#define NHASH_LEN 97
struct interface *nhash_tbl[NHASH_LEN];

int tot_interfaces;         /* # of remote and local interfaces */
int rip_interfaces;         /* # of interfaces doing RIP */
int foundloopback;          /* valid flag for loopaddr */
naddr   loopaddr;           /* our address on loopback */
struct  rt_spare loop_rts;

struct timeval ifinit_timer;
struct timeval last_ifinit;             /* LVL7_MOD (was static) */
#define IF_RESCAN_DELAY() (last_ifinit.tv_sec == now.tv_sec     \
               && last_ifinit.tv_usec == now.tv_usec    \
               && timercmp(&ifinit_timer, &now, >))

int have_ripv1_out;         /* have a RIPv1 interface */
int have_ripv1_in;

#ifdef LVL7_ORIGINAL_VENDOR_CODE
#else
static int ripIntfCost = 1;
void rip_intf_cost_set(int cost);
void rip_intf_cost_get(int * cost);
#endif /* LVL7_ORIGINAL_VENDOR_CODE */

static struct interface**
nhash(char *p)
{
    u_int i;

    for (i = 0; *p != '\0'; p++) {
        i = ((i<<1) & 0x7fffffff) | ((i>>31) & 1);
        i ^= *p;
    }
    return &nhash_tbl[i % NHASH_LEN];
}


/* Link a new interface into the lists and hash tables.
 */
void
if_link(struct interface *ifp)
{
    struct interface **hifp;

    ifp->int_prev = &ifnet;
    ifp->int_next = ifnet;
    if (ifnet != 0)
        ifnet->int_prev = &ifp->int_next;
    ifnet = ifp;

    hifp = AHASH(ifp->int_addr);
    ifp->int_ahash_prev = hifp;
    if ((ifp->int_ahash = *hifp) != 0)
        (*hifp)->int_ahash_prev = &ifp->int_ahash;
    *hifp = ifp;

    if (ifp->int_if_flags & IFF_BROADCAST) {
        hifp = BHASH(ifp->int_brdaddr);
        ifp->int_bhash_prev = hifp;
        if ((ifp->int_bhash = *hifp) != 0)
            (*hifp)->int_bhash_prev = &ifp->int_bhash;
        *hifp = ifp;
    }

    if (ifp->int_state & IS_REMOTE) {
        ifp->int_rlink_prev = &remote_if;
        ifp->int_rlink = remote_if;
        if (remote_if != 0)
            remote_if->int_rlink_prev = &ifp->int_rlink;
        remote_if = ifp;
    }

    hifp = nhash(ifp->int_name);
    if (ifp->int_state & IS_ALIAS) {
        /* put aliases on the end of the hash chain */
        while (*hifp != 0)
            hifp = &(*hifp)->int_nhash;
    }
    ifp->int_nhash_prev = hifp;
    if ((ifp->int_nhash = *hifp) != 0)
        (*hifp)->int_nhash_prev = &ifp->int_nhash;
    *hifp = ifp;
}


/* Find the interface with an address
 */
struct interface *
ifwithaddr(naddr addr,
       int  bcast,          /* notice IFF_BROADCAST address */
       int  remote)         /* include IS_REMOTE interfaces */
{
    struct interface *ifp, *possible = 0;

    remote = (remote == 0) ? IS_REMOTE : 0;

    for (ifp = *AHASH(addr); ifp; ifp = ifp->int_ahash) {
        if (ifp->int_addr != addr)
            continue;
        if ((ifp->int_state & remote) != 0)
            continue;
        if ((ifp->int_state & (IS_BROKE | IS_PASSIVE)) == 0)
            return ifp;
        possible = ifp;
    }

    if (possible || !bcast)
        return possible;

    for (ifp = *BHASH(addr); ifp; ifp = ifp->int_bhash) {
        if (ifp->int_brdaddr != addr)
            continue;
        if ((ifp->int_state & remote) != 0)
            continue;
        if ((ifp->int_state & (IS_BROKE | IS_PASSIVE)) == 0)
            return ifp;
        possible = ifp;
    }

    return possible;
}


/* find the interface with a name
 */
struct interface *
ifwithname(char *name,          /* "ec0" or whatever */
       naddr addr)          /* 0 or network address */
{
    struct interface *ifp;

    for (;;) {
        for (ifp = *nhash(name); ifp != 0; ifp = ifp->int_nhash) {
            /* If the network address is not specified,
             * ignore any alias interfaces.  Otherwise, look
             * for the interface with the target name and address.
             */
            if (!strcmp(ifp->int_name, name)
                && ((addr == 0 && !(ifp->int_state & IS_ALIAS))
                || (ifp->int_addr == addr)))
                return ifp;
        }

        /* If there is no known interface, maybe there is a
         * new interface.  So just once look for new interfaces.
         */
        if (IF_RESCAN_DELAY())
            return 0;
        ifinit();
    }
}


struct interface *
ifwithindex(u_short index,
        int rescan_ok)
{
    struct interface *ifp;

    for (;;) {
        for (ifp = ifnet; 0 != ifp; ifp = ifp->int_next) {
            if (ifp->int_index == index)
                return ifp;
        }

        /* If there is no known interface, maybe there is a
         * new interface.  So just once look for new interfaces.
         */
        if (!rescan_ok
            || IF_RESCAN_DELAY())
            return 0;
        ifinit();
    }
}

/* find the matching interface for an index and address
 */
struct interface *
ifwith_index_and_addr(u_short index,
                      naddr addr,
                      int rescan_ok)
{
    struct interface *ifp, *maybe;

    maybe = 0;
    for (;;) {
        for (ifp = ifnet; 0 != ifp; ifp = ifp->int_next) {
           /* all the interface structures corresponding
            * to a single interface with primary and secondary addresses
            * have the same "ifp->int_index" value
            */

            /* For those matching the index, match for address now */
            if (ifp->int_index == index) {
               if (ifp->int_if_flags & IFF_POINTOPOINT) {
                  /* finished with a match */
                  if (ifp->int_dstaddr == addr)
                     return ifp;
               }
               else {
                  /* finished with an exact match */
                  if (ifp->int_addr == addr)
                     return ifp;
                  /* Look for the longest approximate match.
                   */
                  if (on_net(addr, ifp->int_net, ifp->int_mask)
                      && (maybe == 0
                          || ifp->int_mask > maybe->int_mask))
                     maybe = ifp;
               }
            }
        }

        /* If there is no known interface, maybe there is a
         * new interface.  So just once look for new interfaces.
         */
        if (!rescan_ok
            || IF_RESCAN_DELAY())
            return maybe;
        ifinit();
    }
}

/* Find an interface from which the specified address
 * should have come from.  Used for figuring out which
 * interface a packet came in on.
 */
struct interface *
iflookup(naddr addr)
{
    struct interface *ifp, *maybe;

    maybe = 0;
    for (;;) {
        for (ifp = ifnet; ifp; ifp = ifp->int_next) {
            if (ifp->int_if_flags & IFF_POINTOPOINT) {
                /* finished with a match */
                if (ifp->int_dstaddr == addr)
                    return ifp;

            } else {
                /* finished with an exact match */
                if (ifp->int_addr == addr)
                    return ifp;

                /* Look for the longest approximate match.
                 */
                if (on_net(addr, ifp->int_net, ifp->int_mask)
                    && (maybe == 0
                    || ifp->int_mask > maybe->int_mask))
                    maybe = ifp;
            }
        }

        if (maybe != 0
            || IF_RESCAN_DELAY())
            return maybe;

        /* If there is no known interface, maybe there is a
         * new interface.  So just once look for new interfaces.
         */
        ifinit();
    }
}


/* Return the classical netmask for an IP address.
 */
#ifndef L7_ORIGINAL_VENDOR_CODE
naddr
std_mask(naddr addr)
#else /* L7_ORIGINAL_VENDOR_CODE */
naddr                   /* host byte order */
std_mask(naddr addr)            /* network byte order */
#endif /* L7_ORIGINAL_VENDOR_CODE */
{
#ifdef L7_ORIGINAL_VENDOR_CODE
    NTOHL(addr);            /* was a host, not a network */

#endif /* L7_ORIGINAL_VENDOR_CODE */
    if (addr == 0)          /* default route has mask 0 */
        return 0;
    if (IN_CLASSA(addr))
        return IN_CLASSA_NET;
    if (IN_CLASSB(addr))
        return IN_CLASSB_NET;
    return IN_CLASSC_NET;
}


/* Find the netmask that would be inferred by RIPv1 listeners
 *  on the given interface for a given network.
 *  If no interface is specified, look for the best fitting interface.
 */
naddr
#ifndef L7_ORIGINAL_VENDOR_CODE
ripv1_mask_net(naddr addr,
#else /* L7_ORIGINAL_VENDOR_CODE */
ripv1_mask_net(naddr addr,      /* in network byte order */
#endif /* L7_ORIGINAL_VENDOR_CODE */
           struct interface *ifp)   /* as seen on this interface */
{
    struct r1net *r1p;
    naddr mask = 0;

    if (addr == 0)          /* default always has 0 mask */
        return mask;

    if (ifp != 0 && ifp->int_ripv1_mask != HOST_MASK) {
        /* If the target network is that of the associated interface
         * on which it arrived, then use the netmask of the interface.
         */
        if (on_net(addr, ifp->int_net, ifp->int_std_mask))
            mask = ifp->int_ripv1_mask;

    } else {
        /* Examine all interfaces, and if it the target seems
         * to have the same network number of an interface, use the
         * netmask of that interface.  If there is more than one
         * such interface, prefer the interface with the longest
         * match.
         */
        for (ifp = ifnet; ifp != 0; ifp = ifp->int_next) {
            if (on_net(addr, ifp->int_std_net, ifp->int_std_mask)
                && ifp->int_ripv1_mask > mask
                && ifp->int_ripv1_mask != HOST_MASK)
                mask = ifp->int_ripv1_mask;
        }

    }

    /* check special definitions */
    if (mask == 0) {
        for (r1p = r1nets; r1p != 0; r1p = r1p->r1net_next) {
            if (on_net(addr, r1p->r1net_net, r1p->r1net_match)
                && r1p->r1net_mask > mask)
                mask = r1p->r1net_mask;
        }

        /* Otherwise, make the classic A/B/C guess.
         */
        if (mask == 0)
            mask = std_mask(addr);
    }

    return mask;
}


naddr
#ifndef L7_ORIGINAL_VENDOR_CODE
ripv1_mask_host(naddr addr,
#else /* L7_ORIGINAL_VENDOR_CODE */
ripv1_mask_host(naddr addr,     /* in network byte order */
#endif /* L7_ORIGINAL_VENDOR_CODE */
        struct interface *ifp)  /* as seen on this interface */
{
    naddr mask = ripv1_mask_net(addr, ifp);


    /* If the computed netmask does not mask the address,
     * then assume it is a host address
     */
#ifndef L7_ORIGINAL_VENDOR_CODE
    if ((addr & ~mask) != 0)
#else /* L7_ORIGINAL_VENDOR_CODE */
    if ((ntohl(addr) & ~mask) != 0)
#endif /* L7_ORIGINAL_VENDOR_CODE */
        mask = HOST_MASK;
    return mask;
}


/* See if a IP address looks reasonable as a destination
 */
int                 /* 0=bad */
check_dst(naddr addr)
{
#ifdef L7_ORIGINAL_VENDOR_CODE
    NTOHL(addr);

#endif /* L7_ORIGINAL_VENDOR_CODE */
    if (IN_CLASSA(addr)) {
        if (addr == 0)
            return 1;   /* default */

        addr >>= IN_CLASSA_NSHIFT;
        return (addr != 0 && addr != IN_LOOPBACKNET);
    }

    return (IN_CLASSB(addr) || IN_CLASSC(addr));
}


/* See a new interface duplicates an existing interface.
 */
struct interface *
#ifndef L7_ORIGINAL_VENDOR_CODE
check_dup(naddr addr,
      naddr dstaddr,
      naddr mask,
#else /* L7_ORIGINAL_VENDOR_CODE */
check_dup(naddr addr,           /* IP address, so network byte order */
      naddr dstaddr,        /* ditto */
      naddr mask,           /* mask, so host byte order */
#endif /* L7_ORIGINAL_VENDOR_CODE */
      int if_flags)
{
    struct interface *ifp;

    for (ifp = ifnet; 0 != ifp; ifp = ifp->int_next) {
        if (ifp->int_mask != mask)
            continue;

        if (!iff_up(ifp->int_if_flags))
            continue;

        /* The local address can only be shared with a point-to-point
         * link.
         */
        if (ifp->int_addr == addr
            && (((if_flags|ifp->int_if_flags) & IFF_POINTOPOINT) == 0))
            return ifp;

#ifndef L7_ORIGINAL_VENDOR_CODE
        if (on_net(ifp->int_dstaddr, dstaddr, mask))
#else /* L7_ORIGINAL_VENDOR_CODE */
        if (on_net(ifp->int_dstaddr, ntohl(dstaddr),mask))
#endif /* L7_ORIGINAL_VENDOR_CODE */
            return ifp;
    }
    return 0;
}


/* See that a remote gateway is reachable.
 *  Note that the answer can change as real interfaces come and go.
 */
int                 /* 0=bad */
check_remote(struct interface *ifp)
{
    struct rt_entry *rt;

    /* do not worry about other kinds */
    if (!(ifp->int_state & IS_REMOTE))
        return 1;

    rt = rtfind(ifp->int_addr);
    if (rt != 0
        && rt->rt_ifp != 0
        &&on_net(ifp->int_addr,
             rt->rt_ifp->int_net, rt->rt_ifp->int_mask))
        return 1;

    /* the gateway cannot be reached directly from one of our
     * interfaces
     */
    if (!(ifp->int_state & IS_BROKE)) {
        msglog("unreachable gateway %s\n",     /* LVL7_mod */
               naddr_ntoa(ifp->int_addr));
        if_bad(ifp);
    }
    return 0;
}


/* Delete an interface.
 */
static void
ifdel(struct interface *ifp)
{
    struct L7_ip_mreq_s m;
    struct interface *ifp1;


    trace_if("Del", ifp);
    RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Deleting intf %s from intf structures.", naddr_ntoa(ifp->int_addr));

    ifp->int_state |= IS_BROKE;

    /* unlink the interface
     */
    *ifp->int_prev = ifp->int_next;
    if (ifp->int_next != 0)
        ifp->int_next->int_prev = ifp->int_prev;
    *ifp->int_ahash_prev = ifp->int_ahash;
    if (ifp->int_ahash != 0)
        ifp->int_ahash->int_ahash_prev = ifp->int_ahash_prev;
    *ifp->int_nhash_prev = ifp->int_nhash;
    if (ifp->int_nhash != 0)
        ifp->int_nhash->int_nhash_prev = ifp->int_nhash_prev;
    if (ifp->int_if_flags & IFF_BROADCAST) {
        *ifp->int_bhash_prev = ifp->int_bhash;
        if (ifp->int_bhash != 0)
            ifp->int_bhash->int_bhash_prev = ifp->int_bhash_prev;
    }
    if (ifp->int_state & IS_REMOTE) {
        *ifp->int_rlink_prev = ifp->int_rlink;
        if (ifp->int_rlink != 0)
            ifp->int_rlink->int_rlink_prev = ifp->int_rlink_prev;
    }

    if (!(ifp->int_state & IS_ALIAS)) {
        /* delete aliases when the main interface dies
         */
        for (ifp1 = ifnet; 0 != ifp1; ifp1 = ifp1->int_next) {
            if (ifp1 != ifp
                && !strcmp(ifp->int_name, ifp1->int_name))
                ifdel(ifp1);
        }

        if ((ifp->int_if_flags & IFF_MULTICAST)
#ifdef MCAST_PPP_BUG
            && !(ifp->int_if_flags & IFF_POINTOPOINT)
#endif
            && rip_sock >= 0) {
            m.imr_multiaddr.s_addr = osapiHtonl(INADDR_RIP_GROUP);
            m.imr_interface.s_addr = osapiHtonl((ifp->int_if_flags
                           & IFF_POINTOPOINT)
                          ? ifp->int_dstaddr
                          : ifp->int_addr);
            if ((osapiSetsockopt(rip_sock,IPPROTO_IP,L7_IP_DROP_MEMBERSHIP,
                       (caddr_t)&m, sizeof(m)) != L7_SUCCESS)
                && errno != EADDRNOTAVAIL
                && !TRACEACTIONS)
                LOGERR("RIP: Couldn't set socket option IP_DROP_MEMBERSHIP for intf\n");
        }
        if (ifp->int_rip_sock >= 0) {
            (void)close(ifp->int_rip_sock);
            ifp->int_rip_sock = -1;
            fix_select();
        }

        tot_interfaces--;
        if (!IS_RIP_OFF(ifp->int_state))
            rip_interfaces--;

        set_rdisc_mg(ifp, 0);
        if_bad_rdisc(ifp);
    }

	/* close interface socket */
	if (ifp->int_rip_sock >= 0) {
		(void)close(ifp->int_rip_sock);
		ifp->int_rip_sock = -1;
		fix_select();
	}

    /* Zap all routes associated with this interface (even if its an alias).
     * Assume routes just using gateways beyond this interface
     * will timeout naturally, and have probably already died.
     */
    (void)rn_walktree(rhead, walk_bad, 0);

    free(ifp);
}


/* Mark an interface ill.
 */
void
if_sick(struct interface *ifp)
{
    if (0 == (ifp->int_state & (IS_SICK | IS_BROKE))) {
        ifp->int_state |= IS_SICK;
        ifp->int_act_time = NEVER;
        trace_if("Chg", ifp);
        RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, "Intf is sick : %s %s", ifp->int_name, naddr_ntoa(ifp->int_addr));
        LIM_SEC(ifinit_timer, now.tv_sec+CHECK_BAD_INTERVAL);
    }
}


/* Mark an interface dead.
 */
void
if_bad(struct interface *ifp)
{
    struct interface *ifp1;


    if (ifp->int_state & IS_BROKE)
        return;

    LIM_SEC(ifinit_timer, now.tv_sec+CHECK_BAD_INTERVAL);

    ifp->int_state |= (IS_BROKE | IS_SICK);
    ifp->int_act_time = NEVER;
    ifp->int_query_time = NEVER;
    ifp->int_data.ts = now.tv_sec;

    RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Intf %s %s is going bad.", ifp->int_name, naddr_ntoa(ifp->int_addr));
    trace_if("Chg", ifp);

    /* Bring down all aliases of this interface! */
    if (!(ifp->int_state & IS_ALIAS)) 
    {
        for (ifp1 = ifnet; 0 != ifp1; ifp1 = ifp1->int_next) 
        {
            if (ifp1 != ifp                                 /* ptr doesnt match */
                && !strcmp(ifp->int_name, ifp1->int_name))  /* but name matches! */
            {
              if_bad(ifp1); /* its an alias, mark it as bad */
            }
        }

        /* Only a primary interface can go "bad". Secondary/Alias interfaces
         * never go bad - they just disappear and are deleted! 
         */
        (void)rn_walktree(rhead, walk_bad, 0);
        if_bad_rdisc(ifp);
    }
}


/* Mark an interface alive
 */
int                 /* 1=it was dead */
if_ok(struct interface *ifp,
      const char *type)
{
    struct interface *ifp1;


    if (!(ifp->int_state & IS_BROKE)) {
        if (ifp->int_state & IS_SICK) {
            trace_act("%sinterface %s to %s working better",
                  type,
                  ifp->int_name, naddr_ntoa(ifp->int_dstaddr));
            RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Intf %s %s is working better.", ifp->int_name, naddr_ntoa(ifp->int_addr));
            ifp->int_state &= ~IS_SICK;
        }
        return 0;
    }

    RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"RIP %s interface %s to %s restored",
           type, ifp->int_name, naddr_ntoa(ifp->int_dstaddr));

    msglog("RIP %sinterface %s to %s restored",
           type, ifp->int_name, naddr_ntoa(ifp->int_dstaddr));
    ifp->int_state &= ~(IS_BROKE | IS_SICK);
    ifp->int_data.ts = 0;

    if (!(ifp->int_state & IS_ALIAS)) {
        for (ifp1 = ifnet; 0 != ifp1; ifp1 = ifp1->int_next) {
            if (ifp1 != ifp
                && !strcmp(ifp->int_name, ifp1->int_name))
                if_ok(ifp1, type);
        }
        if_ok_rdisc(ifp);
    }

    if (ifp->int_state & IS_REMOTE) {
        if (!addrouteforif(ifp))
            return 0;
    }
    return 1;
}


/* disassemble routing message
 */
void
rip_rt_xaddrs(struct rt_addrinfo *info,   /* LVL7_MOD */
      struct sockaddr *sa,
      struct sockaddr *lim,
      int addrs)
{
    int i;
#ifdef _HAVE_SA_LEN
    static struct sockaddr sa_zero;
#endif
#ifdef sgi
#define ROUNDUP(a) ((a) > 0 ? (1 + (((a) - 1) | (sizeof(__uint64_t) - 1))) \
            : sizeof(__uint64_t))
#else
#define ROUNDUP(a) ((a) > 0 ? (1 + (((a) - 1) | (sizeof(long) - 1))) \
            : sizeof(long))
#endif


    memset(info, 0, sizeof(*info));
    info->rti_addrs = addrs;
    for (i = 0; i < RTAX_MAX && sa < lim; i++) {
        if ((addrs & (1 << i)) == 0)
            continue;
#ifdef _HAVE_SA_LEN
        info->rti_info[i] = (sa->sa_len != 0) ? sa : &sa_zero;
        sa = (struct sockaddr *)((char*)(sa)
                     + ROUNDUP(sa->sa_len));
#else
#ifdef _L7_OS_VXWORKS_
        info->rti_info[i] = sa;
        sa = (struct sockaddr *)((char*)(sa)
                     + ROUNDUP(_FAKE_SA_LEN_DST(sa)));
#endif
#ifdef _L7_OS_LINUX_
        info->rti_info[i] = sa;
        sa = (struct sockaddr *)((char*)(sa)
                     + ROUNDUP(sizeof(struct sockaddr)));
#endif
#endif
    }
}

 /***********************************************************************
 * Lowdown on what this rather long function does:
 *
 * The functions begins by fetching a list of RIP interfaces from the 
 * mapping layer. This list is an array of ifa_msghdr. For each interface 
 * there is a ifam_type=RTM_IFINFO entry followed by one or more 
 * ifam_type=RTM_NEWADDR entries. The first RTM_NEWADDR entry is for the
 * primary address, subsequent entries are for aliases. 
 *
 * This list is then iterated multiple times:
 *
 *  - (iteration 1) process changed (if_ok, if_sick, if_bad) and 
 *                  new interfaces (if_up)
 *
 *  - do some multi-homed jazz (not sure if this is used for FASTPATH)
 *
 *  - (iteration 2) get rid of interfaces that have disappeared (if_bad, ifdel)
 *
 *  - (iteration 3) add or reinstate interface routes (via addrouteforif)
 *
 *  - (iteration 4) add authority routes (doesnt look like this is used in FP as
 *                  it seems to depend on parse_parms)
 *
 ************************************************************************/

/* Find the network interfaces which have configured themselves.
 * This must be done regularly, if only for extra addresses
 * that come and go on interfaces.
 */
void
rip_ifinit(void)
{
    static char *sysctl_buf;
    static size_t sysctl_buf_size = 0;
    uint complaints = 0;
    static u_int prev_complaints = 0;
#   define COMP_NOT_INET    0x001
#   define COMP_NOADDR  0x002
#   define COMP_BADADDR 0x004
#   define COMP_NODST   0x008
#   define COMP_NOBADR  0x010
#   define COMP_NOMASK  0x020
#   define COMP_DUP     0x040
#   define COMP_BAD_METRIC  0x080
#   define COMP_NETMASK 0x100

    struct interface ifs, ifs0, *ifp, *ifp1;
    struct rt_entry *rt;
    size_t needed;
    int mib[6];
    struct if_msghdr *ifm;
    struct ifa_msghdr *ifam, *ifam_lim, *ifam2;
    int in, ierr, out, oerr;
    struct intnet *intnetp;
    struct rt_addrinfo info;


    memset(&ifs0, 0, sizeof(ifs0));
    last_ifinit = now;
    ifinit_timer.tv_sec = now.tv_sec + (supplier
                        ? CHECK_ACT_INTERVAL
                        : CHECK_QUIET_INTERVAL);

    /* mark all interfaces so we can get rid of those that disappear */
    for (ifp = ifnet; 0 != ifp; ifp = ifp->int_next)
        ifp->int_state &= ~(IS_CHECKED | IS_DUP);

    /***********************************************
     * Fetch interface list from the mapping layer
     * 
     * sysctl is defined as "ripMapSysctl" that goes
     * up to the mapping layer and gets a list of interfaces
     * in the "kernel" format that this code expects!
     *
     ***********************************************/
    mib[0] = CTL_NET;
    mib[1] = PF_ROUTE;
    mib[2] = 0;
    mib[3] = AF_INET;
    mib[4] = NET_RT_IFLIST;
    mib[5] = 0;
    for (;;) {
        if ((needed = sysctl_buf_size) != 0) {
            if (sysctl(mib, 6, sysctl_buf,&needed, 0, 0) >= 0)
                break;
            /* retry if the table grew */
            if (errno != ENOMEM && errno != EFAULT)
                BADERR(1, "Could not get RIP interface list.\n");
            free(sysctl_buf);
            needed = 0;
        }
        if (sysctl(mib, 6, 0, &needed, 0, 0) < 0)
            BADERR(1,"Could not get memory estimate for RIP interfaces.\n");
    /* LVL7_MOD start */
    /* if no storage is needed, probably means there are no RIP interfaces */
    if (needed == 0)
      return;
    /* LVL7_MOD end */
        sysctl_buf = rtmalloc(sysctl_buf_size = needed,
                      "ifinit sysctl");
    }

    /***********************************************
     * Iterate all interfaces 1
     * 
     * Process changed and new interfaces
     * - get interface info from message RTM_IFINFO (ifs0)
     * - process each interface address RTM_NEWADDR (ifs)
     *   note: there is nothing new about this addr at this point!
     * - setup current ifs based on iftype (broadcast is the only one
     *   currently used.
     *
     * Detect new interfaces/find existing by searching 
     * based on ifname (ifwithname)
     *
     * Existing:
     * - compare ifs with existing RIP ifinfo (ifp) to handle if_ok,
     *   if_sick or if_bad events.
     *
     * New:
     * - create an ifp, link it, turn on rip (rip_on) and 
     *   rtr disc (if_ok_rdisc), for the primary address of the 
     *   interface.
     *
     ***********************************************/
    ifam_lim = (struct ifa_msghdr *)(sysctl_buf + needed);
    for (ifam = (struct ifa_msghdr *)sysctl_buf;
         ifam < ifam_lim;
         ifam = ifam2) {

        ifam2 = (struct ifa_msghdr*)((char*)ifam + ifam->ifam_msglen);

        if (ifam->ifam_type == RTM_IFINFO) {
            struct sockaddr_dl *sdl;

            ifm = (struct if_msghdr *)ifam;
            /* make prototype structure for the IP aliases
             */
            memset(&ifs0, 0, sizeof(ifs0));
            ifs0.int_rip_sock = -1;
            ifs0.int_index = ifm->ifm_index;
            ifs0.int_if_flags = ifm->ifm_flags;
            ifs0.int_state = IS_CHECKED;
            ifs0.int_query_time = NEVER;
            ifs0.int_act_time = now.tv_sec;
            ifs0.int_data.ts = now.tv_sec;
            ifs0.int_data.ipackets = ifm->ifm_data.ifi_ipackets;
            ifs0.int_data.ierrors = ifm->ifm_data.ifi_ierrors;
            ifs0.int_data.opackets = ifm->ifm_data.ifi_opackets;
            ifs0.int_data.oerrors = ifm->ifm_data.ifi_oerrors;
            sdl = (struct sockaddr_dl *)(ifm + 1);
            sdl->sdl_data[sdl->sdl_nlen] = 0;
            strncpy(ifs0.int_name, sdl->sdl_data,
                MIN(sizeof(ifs0.int_name), sdl->sdl_nlen));
            continue;
        }

        /* Every RTM_IFINFO must be followed by one or more RTM_NEWADDR
         * entries for the primary (and secondary if any) address(es)
         */
        if (ifam->ifam_type != RTM_NEWADDR) {
            logbad2(1,"RIP interface list is out of sync.\n");  /* LVL7_MOD */
            continue;
        }
        rt_xaddrs(&info, (struct sockaddr *)(ifam+1),
              (struct sockaddr *)ifam2,
              ifam->ifam_addrs);

        /* copy interface data ifs0 to ifs to process the current address */
        memcpy(&ifs, &ifs0, sizeof(ifs));

        /* Prepare ifs0 for the next address of this interface, which 
         * will be an alias. (on the next iteration, unless it gets overwritten 
         * by the next interface's RTM_IFINFO entry)
         *
         * Do not output RIP or Router-Discovery packets via aliases.
         */

        /* TBD: why not output RIP packets via aliases? check what this affects :-o */
        ifs0.int_state |= (IS_ALIAS | IS_NO_RIP_OUT | IS_NO_RDISC);


        /* Validate current interface information
         * - does it have an address?
         * - is it AF_INET
         * - is the first byte non 0 (or FF) e.g 0.10.1.1 is invalid!
         */
        if (INFO_IFA(&info) == 0) {
            if (iff_up(ifs.int_if_flags)) {
                if (!(prev_complaints & COMP_NOADDR))
                    msglog("RIP intf %s has no address.\n",
                           ifs.int_name);
                complaints |= COMP_NOADDR;
            }
            continue;
        }
        if (INFO_IFA(&info)->sa_family != AF_INET) {
            if (iff_up(ifs.int_if_flags)) {
                if (!(prev_complaints & COMP_NOT_INET))
                {
                  trace_act("%s: not AF_INET", ifs.int_name);
                  RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Intf %s is not AF_INET.", ifs.int_name);
                }
                complaints |= COMP_NOT_INET;
            }
            continue;
        }

        ifs.int_addr = osapiNtohl(S_ADDR(INFO_IFA(&info)));

        if (ifs.int_addr >> 24 == 0
            || ifs.int_addr >>24 == 0xff) 
        {
            if (iff_up(ifs.int_if_flags)) 
            {
                if (!(prev_complaints & COMP_BADADDR))
                    msglog("RIP intf %s has a bad address.\n",
                           ifs.int_name);
                complaints |= COMP_BADADDR;
            }
            continue;
        }

        /***********************************************
         * setup ifs structure with interface info
         * - for loopback interfaces (skip, dont thnk its used)
         * - for PtP interfaces (skip, dont thnk its used)
         *
         * - for Broadcast interfaces
         ***********************************************/
        if (ifs.int_if_flags & IFF_LOOPBACK) {
            ifs.int_state |= IS_PASSIVE | IS_NO_RIP | IS_NO_RDISC;
            ifs.int_dstaddr = ifs.int_addr;
            ifs.int_mask = HOST_MASK;
            ifs.int_ripv1_mask = HOST_MASK;
            ifs.int_std_mask = std_mask(ifs.int_dstaddr);
            ifs.int_net = ifs.int_dstaddr;
            if (!foundloopback) 
            {
                foundloopback = 1;
                loopaddr = ifs.int_addr;
                loop_rts.rts_gate = loopaddr;
                loop_rts.rts_router = loopaddr;
            }

        } else if (ifs.int_if_flags & IFF_POINTOPOINT) {
            if (INFO_BRD(&info) == 0
                || INFO_BRD(&info)->sa_family != AF_INET) {
                if (iff_up(ifs.int_if_flags)) {
                    if (!(prev_complaints & COMP_NODST))
                        msglog("RIP intf %s has a bad"
                               " destination address.\n",
                               ifs.int_name);
                    complaints |= COMP_NODST;
                }
                continue;
            }
            ifs.int_dstaddr = osapiNtohl(S_ADDR(INFO_BRD(&info)));
            if (ifs.int_dstaddr >> 24 == 0
                || ifs.int_dstaddr >> 24 == 0xff) 
            {
                if (iff_up(ifs.int_if_flags))
                {
                    if (!(prev_complaints & COMP_NODST))
                    {
                      msglog("RIP intf %s has a bad"
                             " destination address.\n",
                             ifs.int_name);
                    }
                    complaints |= COMP_NODST;
                }
                continue;
            }
            ifs.int_mask = HOST_MASK;
            ifs.int_ripv1_mask = osapiNtohl(S_ADDR(INFO_MASK(&info)));
            ifs.int_std_mask = std_mask(ifs.int_dstaddr);
            ifs.int_net = ifs.int_dstaddr;
        }  
        else 
        {
            /* ensure we have a netmask */
            if (INFO_MASK(&info) == 0) 
            {
                if (iff_up(ifs.int_if_flags)) 
                {
                    if (!(prev_complaints & COMP_NOMASK))
                    {
                      msglog("RIP intf %s has no netmask.\n",
                             ifs.int_name);
                    }
                    complaints |= COMP_NOMASK;
                }
                continue;
            }

            /******************************************************
             * init for broadcast networks
             * (the only one thats really used in FASTPATH as yet)
             ******************************************************/
            ifs.int_dstaddr = ifs.int_addr;
            ifs.int_mask = osapiNtohl(S_ADDR(INFO_MASK(&info)));
            ifs.int_ripv1_mask = ifs.int_mask;
            ifs.int_std_mask = std_mask(ifs.int_addr);
            ifs.int_net = ifs.int_addr & ifs.int_mask;

            if (ifs.int_mask != ifs.int_std_mask)
                ifs.int_state |= IS_SUBNET;

            if (ifs.int_if_flags & IFF_BROADCAST)
            {
                if (INFO_BRD(&info) == 0)
                {
                    if (iff_up(ifs.int_if_flags))
                    {
                        if (!(prev_complaints & COMP_NOBADR))
                        {
                          msglog("RIP intf %s has"
                                 "no broadcast address.\n",
                                 ifs.int_name);
                        }

                        complaints |= COMP_NOBADR;
                    }
                    continue;
                }

                ifs.int_brdaddr = osapiNtohl(S_ADDR(INFO_BRD(&info)));
            }
        }
        ifs.int_std_net = ifs.int_net & ifs.int_std_mask;
        ifs.int_std_addr = ifs.int_std_net;

        /* The metric obtained from the RIP interface cost
         */
        ifs.int_metric = ripIntfCost;

        if (ifs.int_metric > HOPCNT_INFINITY) {
            ifs.int_metric = 0;
            if (!(prev_complaints & COMP_BAD_METRIC)
                && iff_up(ifs.int_if_flags)) {
                complaints |= COMP_BAD_METRIC;
                msglog("RIP intf %s has a metric of %d\n",
                       ifs.int_name, ifs.int_metric);
            }
        }

        /***********************************************************
         * Is this a new interface?
         *
         * See if this is a familiar interface.
         * If so, stop worrying about it if it is the same.
         * Start it over if it now is to somewhere else, as happens
         * frequently with PPP and SLIP.
         *
         ***********************************************************/
        ifp = ifwithname(ifs.int_name, ((ifs.int_state & IS_ALIAS)
                        ? ifs.int_addr
                        : 0));
        if (ifp != 0) {
            ifp->int_state |= IS_CHECKED;

            if (0 != ((ifp->int_if_flags ^ ifs.int_if_flags)
                  & (IFF_BROADCAST
                     | IFF_LOOPBACK
                     | IFF_POINTOPOINT
                     | IFF_MULTICAST))
                || 0 != ((ifp->int_state ^ ifs.int_state)
                     & IS_ALIAS)
                || ifp->int_addr != ifs.int_addr
                || ifp->int_brdaddr != ifs.int_brdaddr
                || ifp->int_dstaddr != ifs.int_dstaddr
                || ifp->int_mask != ifs.int_mask
                || ifp->int_metric != ifs.int_metric) {
                /* Forget old information about
                 * a changed interface.
                 */
                trace_act("interface %s has changed",
                      ifp->int_name);
                RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Intf %s %s has changed.", ifp->int_name, naddr_ntoa(ifp->int_addr));
                ifdel(ifp);
                ifp = 0;
            }
        }

        if (ifp != 0) {
            /* The primary representative of an alias worries
             * about how things are working.
             *
             * aliases dont worry about this becuase when you do a "no ip rip" it
             * affects the primary and all aliases. Never just an alias!
             */
            if (ifp->int_state & IS_ALIAS)
              continue;

            /* note interfaces that have been turned off
             */
            if (!iff_up(ifs.int_if_flags)) {
                if (iff_up(ifp->int_if_flags)) {
                    msglog("RIP interface %s to %s turned off\n",
                           ifp->int_name,
                           naddr_ntoa(ifp->int_dstaddr));
                    RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Intf %s %s turned off.", ifp->int_name, naddr_ntoa(ifp->int_addr));
                    if_bad(ifp);
                    ifp->int_if_flags &= ~IFF_UP;
                } else if (now.tv_sec>(ifp->int_data.ts
                               + CHECK_BAD_INTERVAL)) {
                    trace_act("interface %s has been off"
                          " %ld seconds; forget it",
                          ifp->int_name,
                          now.tv_sec-ifp->int_data.ts);
                    RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Intf %s has been off for %ld seconds. Forget it.", naddr_ntoa(ifp->int_addr), now.tv_sec-ifp->int_data.ts);
                    ifdel(ifp);
                }
                continue;
            }
            /* or that were off and are now ok */
            if (!iff_up(ifp->int_if_flags)) {
                ifp->int_if_flags |= IFF_UP;
                (void)if_ok(ifp, "");
            }

            /* If it has been long enough,
             * see if the interface is broken.
             */
            if (now.tv_sec < ifp->int_data.ts+CHECK_BAD_INTERVAL)
                continue;

            in = ifs.int_data.ipackets - ifp->int_data.ipackets;
            ierr = ifs.int_data.ierrors - ifp->int_data.ierrors;
            out = ifs.int_data.opackets - ifp->int_data.opackets;
            oerr = ifs.int_data.oerrors - ifp->int_data.oerrors;

            /* If the interface just awoke, restart the counters.
             */
            if (ifp->int_data.ts == 0) {
                ifp->int_data = ifs.int_data;
                continue;
            }
            ifp->int_data = ifs.int_data;

            /* Withhold judgment when the short error
             * counters wrap or the interface is reset.
             */
            if (ierr < 0 || in < 0 || oerr < 0 || out < 0) {
                LIM_SEC(ifinit_timer,
                    now.tv_sec+CHECK_BAD_INTERVAL);
                continue;
            }

            /* Withhold judgement when there is no traffic
             */
            if (in == 0 && out == 0 && ierr == 0 && oerr == 0)
                continue;

            /* It is bad if input or output is not working.
             * Require presistent problems before marking it dead.
             */
            if ((in <= ierr && ierr > 0)
                || (out <= oerr && oerr > 0)) {
                if (!(ifp->int_state & IS_SICK)) {
                    trace_act("interface %s to %s"
                          " sick: in=%d ierr=%d"
                          " out=%d oerr=%d",
                          ifp->int_name,
                          naddr_ntoa(ifp->int_dstaddr),
                          in, ierr, out, oerr);
                    RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Intf %s is sick: in=%d ierr=%d out=%d oerr=%d.", naddr_ntoa(ifp->int_addr),in, ierr, out, oerr);
                    if_sick(ifp);
                    continue;
                }
                if (!(ifp->int_state & IS_BROKE)) {
                    msglog("RIP interface %s to %s broken:"
                           " in=%d ierr=%d out=%d oerr=%d\n",
                           ifp->int_name,
                           naddr_ntoa(ifp->int_dstaddr),
                           in, ierr, out, oerr);
                    RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Intf %s is broken: in=%d ierr=%d out=%d oerr=%d.", naddr_ntoa(ifp->int_addr),in, ierr, out, oerr);
                    if_bad(ifp);
                }
                continue;
            }

            /* otherwise, it is active and healthy
             */
            ifp->int_act_time = now.tv_sec;
            (void)if_ok(ifp, "");
            continue;
        }

      /************************************************
       * Found a new interface!
       *
       * ifp == 0 (if we get here!)
       * => RIP does not have an interface by this name
       * => Its a new interface!
       *
       * Add it to the interface list and turn on RIP!
       ************************************************/

        /* This is a new interface.
         * If it is dead, forget it.
         */
        if (!iff_up(ifs.int_if_flags))
            continue;

        /* If it duplicates an existing interface,
         * complain about it, mark the other one
         * duplicated, and forget this one.
         */
        ifp = check_dup(ifs.int_addr,ifs.int_dstaddr,ifs.int_mask,
                ifs.int_if_flags);
        if (ifp != 0) {
            /* Ignore duplicates of itself, caused by having
             * IP aliases on the same network.
             */
            if (!strcmp(ifp->int_name, ifs.int_name))
                continue;

            if (!(prev_complaints & COMP_DUP)) {
                complaints |= COMP_DUP;
                msglog("RIP intf %s (%s%s%s) is duplicated by"
                       " %s (%s%s%s)\n",
                       ifs.int_name,
                       addrname(ifs.int_addr,ifs.int_mask,1),
                       ((ifs.int_if_flags & IFF_POINTOPOINT)
                    ? "-->" : ""),
                       ((ifs.int_if_flags & IFF_POINTOPOINT)
                    ? naddr_ntoa(ifs.int_dstaddr) : ""),
                       ifp->int_name,
                       addrname(ifp->int_addr,ifp->int_mask,1),
                       ((ifp->int_if_flags & IFF_POINTOPOINT)
                    ? "-->" : ""),
                       ((ifp->int_if_flags & IFF_POINTOPOINT)
                    ? naddr_ntoa(ifp->int_dstaddr) : ""));
            }
            ifp->int_state |= IS_DUP;
            continue;
        }

        if (0 == (ifs.int_if_flags & (IFF_POINTOPOINT | IFF_BROADCAST))
            && !(ifs.int_state & IS_PASSIVE)) {
            trace_act("%s is neither broadcast, point-to-point,"
                  " nor loopback",
                  ifs.int_name);
            RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Intf %s is neither broadcast, pt-to-pt nor loopback.", ifs.int_name);
            if (!(ifs.int_state & IFF_MULTICAST))
                ifs.int_state |= IS_NO_RDISC;
        }


        /* It is new and ok.   Add it to the list of interfaces
         */
        ifp = (struct interface *)rtmalloc(sizeof(*ifp), "ifinit ifp");
        memcpy(ifp, &ifs, sizeof(*ifp));
        get_parms(ifp);
        if_link(ifp);
        trace_if("Add", ifp);
        RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Intf %s %s added.", ifp->int_name, naddr_ntoa(ifp->int_addr));

        /* Notice likely bad netmask.
         */
        if (!(prev_complaints & COMP_NETMASK)
            && !(ifp->int_if_flags & IFF_POINTOPOINT)
            && ifp->int_addr != RIP_DEFAULT) {
            for (ifp1 = ifnet; 0 != ifp1; ifp1 = ifp1->int_next) {
                if (ifp1->int_mask == ifp->int_mask)
                    continue;
                if (ifp1->int_if_flags & IFF_POINTOPOINT)
                    continue;
                if (ifp1->int_dstaddr == RIP_DEFAULT)
                    continue;
                /* ignore aliases on the right network */
                if (!strcmp(ifp->int_name, ifp1->int_name))
                    continue;
                if (on_net(ifp->int_dstaddr,
                       ifp1->int_net, ifp1->int_mask)
                    || on_net(ifp1->int_dstaddr,
                          ifp->int_net, ifp->int_mask)) 
                  {
                    msglog("RIP: possible netmask problem"
                           " between %s:%s and %s:%s\n",
                           ifp->int_name,
                           addrname(osapiHtonl(ifp->int_net),
                            ifp->int_mask, 1),
                           ifp1->int_name,
                           addrname(osapiHtonl(ifp1->int_net),
                            ifp1->int_mask, 1));

                    complaints |= COMP_NETMASK;
                }
            }
        }

        /***********************************************
         * turn on router discovery and RIP If needed 
         ***********************************************/
        if (!(ifp->int_state & IS_ALIAS)) 
        {
            /* Count the # of directly connected networks.
             */
            if (!(ifp->int_if_flags & IFF_LOOPBACK))
                tot_interfaces++;
            if (!IS_RIP_OFF(ifp->int_state))
                rip_interfaces++;

            if_ok_rdisc(ifp);
        }
        rip_on(ifp);
    }

    /***********************************************
     * Do some multi-home and router discovery jazz
     * 
     * Not sure if FASTPATH really cares!
     *
     ***********************************************/

    /* If we are multi-homed and have at least two interfaces
     * listening to RIP, then output by default.
     */
    if (!supplier_set && rip_interfaces > 1)
        set_supplier();

    /* If we are multi-homed, optionally advertise a route to
     * our main address.
     */
    if (advertise_mhome
        || (tot_interfaces > 1
        && mhome
        && (ifp = ifwithaddr(myaddr, 0, 0)) != 0
        && foundloopback)) 
    {
        advertise_mhome = 1;
        rt = rtget(myaddr, HOST_MASK);
        if (rt != 0) {
            if (rt->rt_ifp != ifp
                || rt->rt_router != loopaddr) {
                rtdelete(rt);
                rt = 0;
            } else {
                loop_rts.rts_ifp = ifp;
                loop_rts.rts_metric = 0;
                loop_rts.rts_time = rt->rt_time;
                rtchange(rt, rt->rt_state | RS_MHOME,
                     &loop_rts, 0);
            }
        }
        if (rt == 0) {
            loop_rts.rts_ifp = ifp;
            loop_rts.rts_metric = 0;
            rtadd(myaddr, HOST_MASK, RS_MHOME, &loop_rts);
        }
    }

    have_ripv1_out = 0;
    have_ripv1_in  = 0;

    /***********************************************
     * Iterate all interfaces 2
     * 
     * Clean out interfaces that have disappeared
     *
     ***********************************************/
    for (ifp = ifnet; ifp != 0; ifp = ifp1) {
        ifp1 = ifp->int_next;   /* because we may delete it */

        /* Forget any interfaces that have disappeared.
         */
        if (!(ifp->int_state & (IS_CHECKED | IS_REMOTE))) {
            trace_act("interface %s has disappeared",
                  ifp->int_name);
            RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Intf %s %s has disappeared.", ifp->int_name, naddr_ntoa(ifp->int_addr));
            ifdel(ifp);
            continue;
        }

        if ((ifp->int_state & IS_BROKE)
            && !(ifp->int_state & IS_PASSIVE))
            LIM_SEC(ifinit_timer, now.tv_sec+CHECK_BAD_INTERVAL);

        /* If we ever have a RIPv1 interface, assume we always will.
         * It might come back if it ever goes away.
         */
        if (!(ifp->int_state & IS_NO_RIPV1_OUT) && supplier)
            have_ripv1_out = 1;
        if (!(ifp->int_state & IS_NO_RIPV1_IN))
            have_ripv1_in = 1;
    }

    /***********************************************
     * Iterate all interfaces 3
     * 
     * Add (or reinstate) interface local routes
     *
     ***********************************************/
    for (ifp = ifnet; ifp != 0; ifp = ifp->int_next) {
        /* Ensure there is always a network route for interfaces,
         * after any dead interfaces have been deleted, which
         * might affect routes for point-to-point links.
         */
        if (!addrouteforif(ifp))
            continue;

        /* Add routes to the local end of point-to-point interfaces
         * using loopback.
         */
        if ((ifp->int_if_flags & IFF_POINTOPOINT)
            && !(ifp->int_state & IS_REMOTE)
            && foundloopback) {
            /* Delete any routes to the network address through
             * foreign routers. Remove even static routes.
             */
            del_static(ifp->int_addr, HOST_MASK, 0, 0);
            rt = rtget(ifp->int_addr, HOST_MASK);
            if (rt != 0 && rt->rt_router != loopaddr) {
                rtdelete(rt);
                rt = 0;
            }
            if (rt != 0) {
                if (!(rt->rt_state & RS_LOCAL)
                    || rt->rt_metric > ifp->int_metric) {
                    ifp1 = ifp;
                } else {
                    ifp1 = rt->rt_ifp;
                }
                loop_rts.rts_ifp = ifp1;
                loop_rts.rts_metric = 0;
                loop_rts.rts_time = rt->rt_time;
                rtchange(rt, ((rt->rt_state & ~RS_NET_SYN)
                          | (RS_IF|RS_LOCAL)),
                     &loop_rts, 0);
            } else {
                loop_rts.rts_ifp = ifp;
                loop_rts.rts_metric = 0;
                rtadd(ifp->int_addr, HOST_MASK,
                      (RS_IF | RS_LOCAL), &loop_rts);
            }
        }
    }

    /***********************************************
     * Iterate all interfaces 4
     * 
     * Add authority routes 
     * (doesnt look like this is used in FP as it 
     * seems to depend on parse_parms)
     *
     ***********************************************/
    for (intnetp = intnets; intnetp!=0; intnetp = intnetp->intnet_next) {
        rt = rtget(intnetp->intnet_addr, intnetp->intnet_mask);
        if (rt != 0
            && !(rt->rt_state & RS_NO_NET_SYN)
            && !(rt->rt_state & RS_NET_INT)) {
            rtdelete(rt);
            rt = 0;
        }
        if (rt == 0) {
            loop_rts.rts_ifp = 0;
            loop_rts.rts_metric = intnetp->intnet_metric-1;
            rtadd(intnetp->intnet_addr, intnetp->intnet_mask,
                  RS_NET_SYN | RS_NET_INT, &loop_rts);
        }
    }

    prev_complaints = complaints;
}


static void
check_net_syn(struct interface *ifp)
{
    struct rt_entry *rt;
    static struct rt_spare new;
    struct rt_spare *rts, new1;
    int i;
    L7_uchar8 strIfAddr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 strIfMask[OSAPI_INET_NTOA_BUF_SIZE];

#ifndef LVL7_ORIGINAL_VENDOR_CODE
    memset(&new1, 0, sizeof(new1));
#endif /* LVL7_ORIGINAL_VENDOR_CODE */
    /* Initialize the spare used for storing the redist route. */
    new1.rts_metric = HOPCNT_INFINITY;


    /* Turn on the need to automatically synthesize a network route
     * for this interface only if we are running RIPv1 on some other
     * interface that is on a different class-A,B,or C network.
     */
    if (have_ripv1_out || have_ripv1_in) {
        ifp->int_state |= IS_NEED_NET_SYN;
        rt = rtget(ifp->int_std_addr, ifp->int_std_mask);
        if (rt != 0 && 
            0 == (rt->rt_state & RS_NO_NET_SYN) && 
            (!(rt->rt_state & RS_NET_SYN) || rt->rt_metric > ifp->int_metric)) 
        {

            /* If a redistributed route exists, we have to be careful to not to delete
             * it. Move it to be as spare.
             */
            /* If a redist route is the best spare, then remove all other spares and then
             * change. Else delete all the spares, except the best spare and redist spare
             * and poison the best spare, then switch and then change.
             */
            /* We can do all that is said above or just copy the redistributed route
             * info, if it exists, delete this whole route, just like normally it would
             * do, and then re-add the redistributed route info as spare.
             */
            if ((rt->rt_state & RS_REDIST) != 0)
            {
              rts = rt->rt_spares;
              for (i = NUM_SPARES; i != 0; i--, rts++) 
              {
                if ((rts->rts_redist_proto > RIP_RT_REDIST_FIRST) && 
                    (rts->rts_redist_proto < RIP_RT_REDIST_LAST) &&
                    (rts->rts_metric < HOPCNT_INFINITY))
                      break;
              }
              if (i != 0)
                new1 = *rts;
            }

            rtdelete(rt);
            rt = 0;
        }

        /* did not find a route on with the classful mask */
        if (rt == 0) 
        {
            osapiInetNtoa((L7_uint32)ifp->int_std_addr, strIfAddr);
            osapiInetNtoa((L7_uint32)ifp->int_std_mask, strIfMask);
            RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, 
                            "Adding a RS_NET_SYN route for ripv1 net %s mask %s", 
                            strIfAddr, strIfMask);

            new.rts_ifp = ifp;
            new.rts_gate = ifp->int_addr;
            new.rts_router = ifp->int_addr;
            new.rts_metric = ifp->int_metric;
            new.rts_redist_proto = RIP_RT_REDIST_FIRST; /* not redistributed! */
            rtadd(ifp->int_std_addr, ifp->int_std_mask,
                  RS_NET_SYN, &new);

            if (new1.rts_metric != HOPCNT_INFINITY)
            {
              /* There was a redist route. So add it as spare. */
              rt = rtget(ifp->int_std_addr, ifp->int_std_mask);
              if(rt != 0)
              {
                rts = rt->rt_spares;
                rts++;
                for (i = NUM_SPARES-1; i != 0; i--, rts++) 
                {
                  if (rts->rts_gate == 0)
                    break;
                }
                if (i == 0)
                  rts--;
                *rts = new1;
                rt->rt_state |= RS_REDIST;
              }
            }
        }

    } else {
        ifp->int_state &= ~IS_NEED_NET_SYN;

        rt = rtget(ifp->int_std_addr,
               ifp->int_std_mask);
        if (rt != 0
            && (rt->rt_state & RS_NET_SYN)
            && rt->rt_ifp == ifp)
            rtbad_sub(rt);
    }
}


/* Add route for interface if not currently installed.
 * Create route to other end if a point-to-point link,
 * otherwise a route to this (sub)network.
 */
int                 /* 0=bad interface */
rip_addrouteforif(struct interface *ifp)    /* LVL7_MOD */
{
    struct rt_entry *rt;
    static struct rt_spare new;
    naddr dst;
    struct rt_spare *rts, new1;
    int i;

#ifndef LVL7_ORIGINAL_VENDOR_CODE
    memset(&new1, 0, sizeof(new1));
#endif /* LVL7_ORIGINAL_VENDOR_CODE */

    /* skip sick interfaces
     */
    if (ifp->int_state & IS_BROKE)
        return 0;

    /* If the interface on a subnet, then install a RIPv1 route to
     * the network as well (unless it is sick).
     */
    if (ifp->int_state & IS_SUBNET)
        check_net_syn(ifp);

    dst = (0 != (ifp->int_if_flags & (IFF_POINTOPOINT | IFF_LOOPBACK))
           ? ifp->int_dstaddr
           : ifp->int_net);

    new.rts_ifp = ifp;
    new.rts_router = ifp->int_addr;
    new.rts_gate = ifp->int_addr;
    new.rts_metric = ifp->int_metric;
    new.rts_time = now.tv_sec;
    new.rts_redist_proto = RIP_RT_REDIST_FIRST; /* Redistributed route protocol */

    /* Initialize the spare used for storing the redist route. */
    new1.rts_metric = HOPCNT_INFINITY;

    /* If we are going to send packets to the gateway,
     * it must be reachable using our physical interfaces
     */
    if ((ifp->int_state & IS_REMOTE)
        && !(ifp->int_state & IS_EXTERNAL)
        && !check_remote(ifp))
        return 0;

    /* We are finished if the correct main interface route exists.
     * The right route must be for the right interface, not synthesized
     * from a subnet, be a "gateway" or not as appropriate, and so forth.
     */
    del_static(dst, ifp->int_mask, 0, 0);
    rt = rtget(dst, ifp->int_mask);
    if (rt != 0) 
    {
        if ((rt->rt_ifp != ifp
             || rt->rt_router != ifp->int_addr)
            && (!(ifp->int_state & IS_DUP)
            || rt->rt_ifp == 0
            || (rt->rt_ifp->int_state & IS_BROKE))) 
          {
            /* If a redistributed route exists, we have to be careful to not to delete
             * it. Move it to be as spare.
             */
            /* If a redist route is the best spare, then remove all other spares and then
             * change. Else delete all the spares, except the best spare and redist spare
             * and poison the best spare, then switch and then change.
             */
            /* We can do all that is said above or just copy the redistributed route
             * info, if it exists, delete this whole route, just like normally it would
             * do, and then re-add the redistributed route info as spare.
             */
            if ((rt->rt_state & RS_REDIST) != 0)
            {
              rts = rt->rt_spares;
              for (i = NUM_SPARES; i != 0; i--, rts++) 
              {
                if ((rts->rts_redist_proto > RIP_RT_REDIST_FIRST) && 
                    (rts->rts_redist_proto < RIP_RT_REDIST_LAST) &&
                    (rts->rts_metric < HOPCNT_INFINITY))
                      break;
              }
              if (i != 0)
                new1 = *rts;
            }

            rtdelete(rt);
            rt = 0;
        } else {
            rtchange(rt, ((rt->rt_state | RS_IF)
                      & ~(RS_NET_SYN | RS_LOCAL)),
                 &new, 0);
        }
    }
    if (rt == 0) {
        if (ifp->int_transitions++ > 0)
        {
          trace_act("re-install interface %s",
                ifp->int_name);
          RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, "re-install interface %s", ifp->int_name);
        }

        rtadd(dst, ifp->int_mask, RS_IF, &new);

        if (new1.rts_metric != HOPCNT_INFINITY)
        {
          /* There was a redist route. So add it as spare. */
          rt = rtget(dst, ifp->int_mask);
          if(rt != 0)
          {
            rts = rt->rt_spares;
            rts++;
            for (i = NUM_SPARES-1; i != 0; i--, rts++) 
            {
              if (rts->rts_gate == 0)
                break;
            }
            if (i == 0)
              rts--;
            *rts = new1;
            rt->rt_state |= RS_REDIST;
          }
        }
    }

    return 1;
}

void rip_intf_cost_set(int cost)
{
  ripIntfCost = cost;
  return;
}

void rip_intf_cost_get(int * cost)
{
  *cost = ripIntfCost;
  return;
}

