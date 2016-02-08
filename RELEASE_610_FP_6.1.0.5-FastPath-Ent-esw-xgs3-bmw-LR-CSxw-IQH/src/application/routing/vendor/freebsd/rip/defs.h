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
 *  @(#)defs.h  8.1 (Berkeley) 6/5/93
 *
 * $FreeBSD: src/sbin/routed/defs.h,v 1.13 1999/09/05 17:49:11 peter Exp $
 */

#ifdef  sgi
#ident "$FreeBSD: src/sbin/routed/defs.h,v 1.13 1999/09/05 17:49:11 peter Exp $"
#endif

/* Definitions for RIPv2 routing process.
 *
 * This code is based on the 4.4BSD `routed` daemon, with extensions to
 * support:
 *  RIPv2, including variable length subnet masks.
 *  Router Discovery
 *  aggregate routes in the kernel tables.
 *  aggregate advertised routes.
 *  maintain spare routes for faster selection of another gateway
 *      when the current gateway dies.
 *  timers on routes with second granularity so that selection
 *      of a new route does not wait 30-60 seconds.
 *  tolerance of static routes.
 *  tell the kernel hop counts
 *  do not advertise if ipforwarding=0
 *
 * The vestigial support for other protocols has been removed.  There
 * is no likelihood that IETF RIPv1 or RIPv2 will ever be used with
 * other protocols.  The result is far smaller, faster, cleaner, and
 * perhaps understandable.
 *
 * The accumulation of special flags and kludges added over the many
 * years have been simplified and integrated.
 */
#ifdef _L7_OS_VXWORKS_
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "taskLib.h"
#include "msgQLib.h"
#include "rebootLib.h"
#include "logLib.h"
#include "tickLib.h"
#include "sysLib.h"
#include "signal.h"
#include "iv.h"
#include "intLib.h"
#include "types.h"
#include "socket.h"
#if VX_VERSION != 65
#include "bsdSockLib.h"
#endif
#include "ioLib.h"
#include "ioctl.h"
#include "routed.h"
#include "sockLib.h"
#include "inetLib.h"
#endif

#if VX_VERSION == 65
#define RTM_VERSION 3   /* Up the ante and ignore older versions */
#define NET_RT_IFLIST 3
#define RTV_HOPCOUNT    0x2 /* init or lock _hopcount */


struct rt_addrinfo{
    int rti_addrs;
    struct sockaddr *rti_info[RTAX_MAX];
};
#endif


#ifdef _L7_OS_LINUX_
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <net/if.h>
#define NTOHL(x) (x) = ntohl(x)
#define HTONL(x) (x) = htonl(x)
#define NET_RT_DUMP 1
#define NET_RT_FLAGS 2
#define NET_RT_IFLIST 3
#define NET_RT_MAXID 4
#define RTM_VERSION 3   /* Up the ante and ignore older versions */

#define RTM_ADD     0x1 /* Add Route */
#define RTM_DELETE  0x2 /* Delete Route */
#define RTM_CHANGE  0x3 /* Change Metrics or flags */
#define RTM_GET     0x4 /* Report Metrics */
#define RTM_LOSING  0x5 /* Kernel Suspects Partitioning */
#define RTM_REDIRECT    0x6 /* Told to use different route */
#define RTM_MISS    0x7 /* Lookup failed on this address */
#define RTM_LOCK    0x8 /* fix specified metrics */
#define RTM_OLDADD  0x9 /* caused by SIOCADDRT */
#define RTM_OLDDEL  0xa /* caused by SIOCDELRT */
#define RTM_RESOLVE 0xb /* req to resolve dst to LL addr */
#define RTM_NEWADDR 0xc /* address being added to iface */
#define RTM_DELADDR 0xd /* address being removed from iface */
#define RTM_IFINFO  0xe /* iface going up/down etc. */
/*
 * Index offsets for sockaddr array for alternate internal encoding.
 */
#define RTAX_DST    0   /* destination sockaddr present */
#define RTAX_GATEWAY    1   /* gateway sockaddr present */
#define RTAX_NETMASK    2   /* netmask sockaddr present */
#define RTAX_GENMASK    3   /* cloning mask sockaddr present */
#define RTAX_IFP    4   /* interface name sockaddr present */
#define RTAX_IFA    5   /* interface addr sockaddr present */
#define RTAX_AUTHOR 6   /* sockaddr for author of redirect */
#define RTAX_BRD    7   /* for NEWADDR, broadcast or p-p dest addr */
#define RTAX_MAX    8   /* size of array to allocate */

struct sockaddr_in_new {
    u_char sin_len;
    u_char sin_family;
    u_short sin_port;
    struct in_addr sin_addr;
    char sin_zero[8];
 };

struct rt_addrinfo{
    int rti_addrs;
    struct sockaddr *rti_info[RTAX_MAX];
};

/*
 * Structure defining statistics and other data kept regarding a network
 * interface.
 */
struct  if_data {
    /* generic interface information */
    u_char  ifi_type;       /* ethernet, tokenring, etc. */
    u_char  ifi_addrlen;        /* media address length */
    u_char  ifi_hdrlen;     /* media header length */
    u_long  ifi_mtu;        /* maximum transmission unit */
    u_long  ifi_metric;     /* routing metric (external only) */
    u_long  ifi_baudrate;       /* linespeed */
    /* volatile statistics */
    u_long  ifi_ipackets;       /* packets received on interface */
    u_long  ifi_ierrors;        /* input errors on interface */
    u_long  ifi_opackets;       /* packets sent on interface */
    u_long  ifi_oerrors;        /* output errors on interface */
    u_long  ifi_collisions;     /* collisions on csma interfaces */
    u_long  ifi_ibytes;     /* total number of octets received */
    u_long  ifi_obytes;     /* total number of octets sent */
    u_long  ifi_imcasts;        /* packets received via multicast */
    u_long  ifi_omcasts;        /* packets sent via multicast */
    u_long  ifi_iqdrops;        /* dropped on input, this interface */
    u_long  ifi_noproto;        /* destined for unsupported protocol */
    u_long  ifi_lastchange;     /* last updated */
};
/*
 * Message format for use in obtaining information about interfaces
 * from sysctl and the routing socket.
 */
struct if_msghdr {
    u_short ifm_msglen; /* to skip over non-understood messages */
    u_char  ifm_version;    /* future binary compatability */
    u_char  ifm_type;   /* message type */
    int ifm_addrs;  /* like rtm_addrs */
    int ifm_flags;  /* value of if_flags */
    u_short ifm_index;  /* index for associated ifp */
    struct  if_data ifm_data;/* statistics and other data about if */
};

/*
 * Message format for use in obtaining information about interface addresses
 * from sysctl and the routing socket.
 */
struct ifa_msghdr {
    u_short ifam_msglen;    /* to skip over non-understood messages */
    u_char  ifam_version;   /* future binary compatability */
    u_char  ifam_type;  /* message type */
    int ifam_addrs; /* like rtm_addrs */
    int ifam_flags; /* value of ifa_flags */
    u_short ifam_index; /* index for associated ifp */
    int ifam_metric;    /* value of ifa_metric */
};

struct rt_metrics {
    u_long  rmx_locks;  /* Kernel must leave these values alone */
    u_long  rmx_mtu;    /* MTU for this path */
    u_long  rmx_expire; /* lifetime for route, e.g. redirect */
    u_long  rmx_recvpipe;   /* inbound delay-bandwith product */
    u_long  rmx_sendpipe;   /* outbound delay-bandwith product */
    u_long  rmx_ssthresh;   /* outbound gateway buffer limit */
    u_long  rmx_rtt;    /* estimated round trip time */
    u_long  rmx_rttvar; /* estimated rtt variance */
    u_short rmx_hopcount;   /* max hops expected */
};
/*
 * Structures for routing messages.
 */
struct rt_msghdr {
    u_short rtm_msglen; /* to skip over non-understood messages */
    u_char  rtm_version;    /* future binary compatibility */
    u_char  rtm_type;   /* message type */
    u_short rtm_index;  /* index for associated ifp */
    int rtm_flags;  /* flags, incl. kern & message, e.g. DONE */
    int rtm_addrs;  /* bitmask identifying sockaddrs in msg */
    pid_t   rtm_pid;    /* identify sender */
    int rtm_seq;    /* for sender to identify action */
    int rtm_errno;  /* why failed */
    int rtm_use;    /* from rtentry */
    u_long  rtm_inits;  /* which metrics we are initializing */
    struct  rt_metrics rtm_rmx; /* metrics themselves */
};

/*
 * Bitmask values for rtm_addr.
 */
#define RTA_DST     0x1 /* destination sockaddr present */
#define RTA_GATEWAY 0x2 /* gateway sockaddr present */
#define RTA_NETMASK 0x4 /* netmask sockaddr present */
#define RTA_GENMASK 0x8 /* cloning mask sockaddr present */
#define RTA_IFP     0x10    /* interface name sockaddr present */
#define RTA_IFA     0x20    /* interface addr sockaddr present */
#define RTA_AUTHOR  0x40    /* sockaddr for author of redirect */
#define RTA_BRD     0x80    /* for NEWADDR, broadcast or p-p dest addr */

#define RTV_MTU     0x1 /* init or lock _mtu */
#define RTV_HOPCOUNT    0x2 /* init or lock _hopcount */
#define RTV_EXPIRE  0x4 /* init or lock _hopcount */
#define RTV_RPIPE   0x8 /* init or lock _recvpipe */
#define RTV_SPIPE   0x10    /* init or lock _sendpipe */
#define RTV_SSTHRESH    0x20    /* init or lock _ssthresh */
#define RTV_RTT     0x40    /* init or lock _rtt */
#define RTV_RTTVAR  0x80    /* init or lock _rttvar */

#define RTF_UP      0x1     /* route usable */
#define RTF_GATEWAY 0x2     /* destination is a gateway */
#define RTF_HOST    0x4     /* host entry (net otherwise) */
#define RTF_REJECT  0x8     /* host or net unreachable */
#define RTF_DYNAMIC 0x10        /* created dynamically (by redirect) */
#define RTF_MODIFIED    0x20        /* modified dynamically (by redirect) */
#define RTF_DONE    0x40        /* message confirmed */
#define RTF_MASK    0x80        /* subnet mask present */
#define RTF_CLONING 0x100       /* generate new routes on use */
#define RTF_XRESOLVE    0x200       /* external daemon resolves name */
#define RTF_LLINFO  0x400       /* generated by ARP or ESIS */
#define RTF_STATIC  0x800       /* manually added */
#define RTF_BLACKHOLE   0x1000      /* just discard pkts (during updates) */
#define RTF_PROTO2  0x4000      /* protocol specific routing flag */
#define RTF_PROTO1  0x8000      /* protocol specific routing flag */

#define RTF_MGMT    0x2000      /* modfied by managment proto */
#endif
#include "routed.h"
#include "if_dl.h"
#include "radix.h"
/*#include "time.h"*/                   /* LVL7_MOD */
#include "rip_time.h"
#include "rip_porting.h"

#ifdef LVL7_ORIGINAL_VENDOR_CODE        /* LVL7_MOD start */
#else                                   /* LVL7_MOD */
#include "rip_debug.h"
#endif /* LVL7_ORIGINAL_VENDOR_CODE */        /* LVL7_MOD end */

#ifdef LVL7_ORIGINAL_VENDOR_CODE        /* LVL7_MOD start */
#define UNUSED __attribute__((unused))
#else                                   /* LVL7_MOD */
#define UNUSED
#endif /* LVL7_ORIGINAL_VENDOR_CODE */        /* LVL7_MOD end */


/* Type of an IP address.
 *  Some systems do not like to pass structures, so do not use in_addr.
 *  Some systems think a long has 64 bits, which would be a gross waste.
 * So define it here so it can be changed for the target system.
 * It should be defined somewhere netinet/in.h, but it is not.
 */
#ifdef sgi
#define naddr u_int32_t
#elif defined (__NetBSD__)
#define naddr u_int32_t
#define _HAVE_SA_LEN
#define _HAVE_SIN_LEN
#else
#define naddr u_long
#define _HAVE_SA_LEN
#define _HAVE_SIN_LEN
#endif
#ifdef _L7_OS_LINUX_
#undef _HAVE_SIN_LEN
#undef _HAVE_SA_LEN
#endif
/* Turn on if IP_DROP_MEMBERSHIP and IP_ADD_MEMBERSHIP do not look at
 * the dstaddr of point-to-point interfaces.
 */
#ifdef __NetBSD__
#define MCAST_PPP_BUG
#endif

#define DAY (24*60*60)
#define NEVER DAY           /* a long time */
#define EPOCH NEVER         /* bias time by this to avoid <0 */

/* Scan the kernel regularly to see if any interfaces have appeared or been
 * turned off.  These must be less than STALE_TIME.
 */
#define CHECK_BAD_INTERVAL  5   /* when an interface is known bad */
#define CHECK_ACT_INTERVAL  30  /* when advertising */
#define CHECK_QUIET_INTERVAL    300 /* when not */

#define LIM_SEC(s,l) ((s).tv_sec = MIN((s).tv_sec, (l)))

/* Metric used for fake default routes.  It ought to be 15, but when
 * processing advertised routes, previous versions of `routed` added
 * to the received metric and discarded the route if the total was 16
 * or larger.
 */
#define FAKE_METRIC (HOPCNT_INFINITY-2)


/* Router Discovery parameters */
#ifndef sgi
#define INADDR_ALLROUTERS_GROUP     0xe0000002  /* 224.0.0.2 */
#endif
#define MaxMaxAdvertiseInterval     1800
#define MinMaxAdvertiseInterval     4
#define DefMaxAdvertiseInterval     600
#define DEF_PreferenceLevel     0
#define MIN_PreferenceLevel     0x80000000

#define MAX_INITIAL_ADVERT_INTERVAL 16
#define MAX_INITIAL_ADVERTS     3
#define MAX_RESPONSE_DELAY      2

#define MAX_SOLICITATION_DELAY      1
#define SOLICITATION_INTERVAL       3
#define MAX_SOLICITATIONS       3


/* Bloated packet size for systems that simply add authentication to
 * full-sized packets
 */
#define OVER_MAXPACKETSIZE (MAXPACKETSIZE+sizeof(struct netinfo)*2)
/* typical packet buffers */
union pkt_buf {
    char    packet[OVER_MAXPACKETSIZE*2];
    struct  rip rip;
};

#define GNAME_LEN   64          /* assumed=64 in parms.c */
/* bigger than IFNAMSIZ, with room for "external()" or "remote()" */
#define IF_NAME_LEN (GNAME_LEN+15)
#define MAXHOSTNAMELEN  64    /* LVL7_MOD */

/* No more routes than this, to protect ourself in case something goes
 * whacko and starts broadcasting zillions of bogus routes.
 */
#ifdef LVL7_ORIGINAL_VENDOR_CODE        /* LVL7_MOD start */
#define MAX_ROUTES  (128*1024)
#else                                   /* LVL7_MOD */
#define MAX_ROUTES  ripMapVendRipRoutesMaxGet()
#endif /* LVL7_ORIGINAL_VENDOR_CODE */        /* LVL7_MOD end */


#ifdef LVL7_ORIGINAL_VENDOR_CODE        /* LVL7_MOD start */
#else                                   /* LVL7_MOD */
typedef enum
{
  RIP_RT_REDIST_FIRST = 0,
  RIP_RT_REDIST_LOCAL,
  RIP_RT_REDIST_STATIC,
  RIP_RT_REDIST_OSPF,
  RIP_RT_REDIST_BGP,
  RIP_RT_REDIST_LAST
} rip_redist_proto;
#endif /* LVL7_ORIGINAL_VENDOR_CODE */        /* LVL7_MOD end */

/* Main, daemon routing table structure
 */
struct rt_entry {
    struct  radix_node rt_nodes[2]; /* radix tree glue */
    u_int   rt_state;
#       define RS_IF    0x001   /* for network interface */
#       define RS_NET_INT   0x002   /* authority route */
#       define RS_NET_SYN   0x004   /* fake net route for subnet */
#       define RS_NO_NET_SYN (RS_LOCAL | RS_LOCAL | RS_IF)
#       define RS_SUBNET    0x008   /* subnet route from any source */
#       define RS_LOCAL 0x010   /* loopback for pt-to-pt */
#       define RS_MHOME 0x020   /* from -m */
#       define RS_STATIC    0x040   /* from the kernel */
#       define RS_RDISC     0x080   /* from router discovery */
#ifdef LVL7_ORIGINAL_VENDOR_CODE        /* LVL7_MOD start */
#else                                   /* LVL7_MOD */
        /* Either the preferred route or a spare is a redistributed route */
#       define RS_REDIST    0x100   
#endif /*LVL7_ORIGINAL_VENDOR_CODE*/
#ifdef _L7_OS_VXWORKS_
    struct sockaddr_in rt_dst_sock;
#endif
#ifdef _L7_OS_LINUX_
    struct sockaddr_in_new rt_dst_sock;
#endif
    naddr   rt_mask;
    struct rt_spare {
        struct interface *rts_ifp;
        naddr   rts_gate;       /* forward packets here */
        naddr   rts_router;     /* on the authority of this router */
        char    rts_metric;
        u_short rts_tag;
        time_t  rts_time;       /* timer to junk stale routes */
        u_int   rts_de_ag;      /* de-aggregation level */
#ifdef LVL7_ORIGINAL_VENDOR_CODE        /* LVL7_MOD start */
#else                                   /* LVL7_MOD */
        u_int   rts_redist_proto; /* Redistributed route protocol */
#endif /*LVL7_ORIGINAL_VENDOR_CODE*/
#define NUM_SPARES 4
    } rt_spares[NUM_SPARES];
    u_int   rt_seqno;       /* when last changed */
    char    rt_poison_metric;   /* to notice maximum recently */
    time_t  rt_poison_time;     /*  advertised metric */
};

#define rt_dst      rt_dst_sock.sin_addr.s_addr
#define rt_msk      rt_mask
#define rt_ifp      rt_spares[0].rts_ifp
#define rt_gate     rt_spares[0].rts_gate
#define rt_router   rt_spares[0].rts_router
#define rt_metric   rt_spares[0].rts_metric
#define rt_tag      rt_spares[0].rts_tag
#define rt_time     rt_spares[0].rts_time
#define rt_de_ag    rt_spares[0].rts_de_ag
#ifdef LVL7_ORIGINAL_VENDOR_CODE        /* LVL7_MOD start */
#else                                   /* LVL7_MOD */
#define rt_redist_proto   rt_spares[0].rts_redist_proto
#endif /*LVL7_ORIGINAL_VENDOR_CODE*/

#define HOST_MASK   0xffffffff
#define RT_ISHOST(rt)   ((rt)->rt_mask == HOST_MASK)

/* age all routes that
 *  are not from -g, -m, or static routes from the kernel
 *  not unbroken interface routes
 *      but not broken interfaces
 *  nor non-passive, remote interfaces that are not aliases
 *      (i.e. remote & metric=0)
 */
#define AGE_RT(rt_state,ifp) (0 == ((rt_state) & (RS_MHOME | RS_STATIC      \
                          | RS_NET_SYN | RS_RDISC)) \
                  && (!((rt_state) & RS_IF)             \
                  || (ifp) == 0                 \
                  || (((ifp)->int_state & IS_REMOTE)        \
                      && !((ifp)->int_state & IS_PASSIVE))))

/* true if A is better than B
 * Better if
 *  - A is not a poisoned route
 *  - and A is not stale
 *  - and A has a shorter path
 *      - or is the router speaking for itself
 *      - or the current route is equal but stale
 *      - or it is a host route advertised by a system for itself
 */
#define BETTER_LINK(rt,A,B) ((A)->rts_metric < HOPCNT_INFINITY      \
                 && now_stale <= (A)->rts_time      \
                 && ((A)->rts_metric < (B)->rts_metric  \
                 || ((A)->rts_gate == (A)->rts_router   \
                     && (B)->rts_gate != (B)->rts_router) \
                 || ((A)->rts_metric == (B)->rts_metric \
                     && now_stale > (B)->rts_time)  \
                 || (RT_ISHOST(rt)          \
                     && (rt)->rt_dst == (A)->rts_router \
                     && (A)->rts_metric == (B)->rts_metric)))


/* LVL7_MOD: RFC 1724 RIPv2 MIB start */
/* defined RIP global MIB stats */
struct rip_global_mibstats {
  u_int route_changes;        /* number of route changes in RIP database */
  u_int queries;              /* number of RIP query responses sent out */
};
/* LVL7_MOD: RFC 1724 RIPv2 MIB end */

/* LVL7_MOD: RFC 2082 start */

/* For now, we assume the number of neighboring routers for the array
 * size declaration to be 50.
 *
 * NOTE: This limiting value is not yet used in the whole RIP application
 *       as a limit on the number of neighbors.
 *       This limit is right now enforced only on the neighbors to this router
 *       that are using md5 authentication */
#define NUM_RIP_NBRS   50

struct nbr_specific_data
{
    naddr  nbr_router;

    time_t    lastMsgRcvd;  /* time when last message RQST/RESP was received
                             * If this time is > EXPIRE_TIME seconds, we assume the
                             * connectivity to this nbr is lost */

    /* The following 2 values are valid if the auth-type is keyed md5 */
    /* For a given keyid, the seqno is supposed to be
     * received non-decreasing from the same neighboring router */
    u_int32_t md5_seqno;
    int8_t    md5_keyid;
};
/* LVL7_MOD: RFC 2082 end */

/* An "interface" is similar to a kernel ifnet structure, except it also
 * handles "logical" or "IS_REMOTE" interfaces (remote gateways).
 */
struct interface {
    struct interface *int_next, **int_prev;
    struct interface *int_ahash, **int_ahash_prev;
    struct interface *int_bhash, **int_bhash_prev;
    struct interface *int_rlink, **int_rlink_prev;
    struct interface *int_nhash, **int_nhash_prev;
    char    int_name[IF_NAME_LEN+1];
    u_short int_index;
    naddr   int_addr;       /* address on this host (net order) */
    naddr   int_brdaddr;        /* broadcast address (n) */
    naddr   int_dstaddr;        /* other end of pt-to-pt link (n) */
    naddr   int_net;        /* working network # (host order)*/
    naddr   int_mask;       /* working net mask (host order) */
    naddr   int_ripv1_mask;     /* for inferring a mask (n) */
    naddr   int_std_addr;       /* class A/B/C address (n) */
    naddr   int_std_net;        /* class A/B/C network (h) */
    naddr   int_std_mask;       /* class A/B/C netmask (h) */
    int int_rip_sock;       /* for queries */
    int int_if_flags;       /* some bits copied from kernel */
    u_int   int_state;
    time_t  int_act_time;       /* last thought healthy */
    time_t  int_query_time;
    u_short int_transitions;    /* times gone up-down */
    char    int_metric;
    char    int_d_metric;       /* for faked default route */
    struct int_data {
        u_int   ipackets;   /* previous network stats */
        u_int   ierrors;
        u_int   opackets;
        u_int   oerrors;
#ifdef sgi
        u_int   odrops;
#endif
        time_t  ts;     /* timestamp on network stats */
    } int_data;
#   define MAX_AUTH_KEYS 5
    struct auth {           /* authentication info */
        u_int16_t type;
        u_char  key[RIP_AUTH_PW_LEN];
        u_char  keyid;
        time_t  start, end;
    } int_auth[MAX_AUTH_KEYS];
    /* router discovery parameters */
    int int_rdisc_pref;     /* signed preference to advertise */
    int int_rdisc_int;      /* MaxAdvertiseInterval */
    int int_rdisc_cnt;
    struct timeval int_rdisc_timer;
  /* LVL7_MOD: RFC 1724 RIPv2 MIB start */
  struct int_mibstats {
    u_int ibadpkts;     /* bad RIP packets received */
    u_int ibadroutes;   /* bad routes detected in received RIP packets */
    u_int otrupdates;   /* triggered RIP updates sent (not full updates) */
  } int_mibstats;
  u_int int_rip_pkts_rcvd;  /* total RIP packets received (NOT A MIB STAT) */
  /* LVL7_MOD: RFC 1724 RIPv2 MIB end */
};

/* bits in int_state */
#define IS_ALIAS        0x0000001   /* interface alias */
#define IS_SUBNET       0x0000002   /* interface on subnetted network */
#define IS_REMOTE       0x0000004   /* interface is not on this machine */
#define IS_PASSIVE      0x0000008   /* remote and does not do RIP */
#define IS_EXTERNAL     0x0000010   /* handled by EGP or something */
#define IS_CHECKED      0x0000020   /* still exists */
#define IS_ALL_HOSTS        0x0000040   /* in INADDR_ALLHOSTS_GROUP */
#define IS_ALL_ROUTERS      0x0000080   /* in INADDR_ALLROUTERS_GROUP */
#define IS_DISTRUST     0x0000100   /* ignore untrusted routers */
#define IS_REDIRECT_OK      0x0000200   /* accept ICMP redirects */
#define IS_BROKE        0x0000400   /* seems to be broken */
#define IS_SICK         0x0000800   /* seems to be broken */
#define IS_DUP          0x0001000   /* has a duplicate address */
#define IS_NEED_NET_SYN     0x0002000   /* need RS_NET_SYN route */
#define IS_NO_AG        0x0004000   /* do not aggregate subnets */
#define IS_NO_SUPER_AG      0x0008000   /* do not aggregate networks */
#define IS_NO_RIPV1_IN      0x0010000   /* no RIPv1 input at all */
#define IS_NO_RIPV2_IN      0x0020000   /* no RIPv2 input at all */
#define IS_NO_RIP_IN    (IS_NO_RIPV1_IN | IS_NO_RIPV2_IN)
#define IS_RIP_IN_OFF(s) (((s) & IS_NO_RIP_IN) == IS_NO_RIP_IN)
#define IS_NO_RIPV1_OUT     0x0040000   /* no RIPv1 output at all */
#define IS_NO_RIPV2_OUT     0x0080000   /* no RIPv2 output at all */
#define IS_NO_RIP_OUT   (IS_NO_RIPV1_OUT | IS_NO_RIPV2_OUT)
#define IS_NO_RIP   (IS_NO_RIP_OUT | IS_NO_RIP_IN)
#define IS_RIP_OUT_OFF(s) (((s) & IS_NO_RIP_OUT) == IS_NO_RIP_OUT)
#define IS_RIP_OFF(s)   (((s) & IS_NO_RIP) == IS_NO_RIP)
#define IS_NO_RIP_MCAST     0x0100000   /* broadcast RIPv2 */
#define IS_NO_ADV_IN        0x0200000   /* do not listen to advertisements */
#define IS_NO_SOL_OUT       0x0400000   /* send no solicitations */
#define IS_SOL_OUT      0x0800000   /* send solicitations */
#define GROUP_IS_SOL_OUT (IS_SOL_OUT | IS_NO_SOL_OUT)
#define IS_NO_ADV_OUT       0x1000000   /* do not advertise rdisc */
#define IS_ADV_OUT      0x2000000   /* advertise rdisc */
#define GROUP_IS_ADV_OUT (IS_NO_ADV_OUT | IS_ADV_OUT)
#define IS_BCAST_RDISC      0x4000000   /* broadcast instead of multicast */
#define IS_NO_RDISC (IS_NO_ADV_IN | IS_NO_SOL_OUT | IS_NO_ADV_OUT)
#define IS_PM_RDISC     0x8000000   /* poor-man's router discovery */

#define iff_up(f) ((f) & IFF_UP)


/* Information for aggregating routes */
#define NUM_AG_SLOTS    32
struct ag_info {
    struct ag_info *ag_fine;    /* slot with finer netmask */
    struct ag_info *ag_cors;    /* more coarse netmask */
    naddr   ag_dst_h;       /* destination in host byte order */
    naddr   ag_mask;
    naddr   ag_gate;
    naddr   ag_nhop;
    char    ag_metric;      /* metric to be advertised */
    char    ag_pref;        /* aggregate based on this */
    u_int   ag_seqno;
    u_short ag_tag;
    u_short ag_state;
#define     AGS_SUPPRESS    0x001   /* combine with coarser mask */
#define     AGS_AGGREGATE   0x002   /* synthesize combined routes */
#define     AGS_REDUN0      0x004   /* redundant, finer routes output */
#define     AGS_REDUN1      0x008
#define     AG_IS_REDUN(state) (((state) & (AGS_REDUN0 | AGS_REDUN1)) \
                == (AGS_REDUN0 | AGS_REDUN1))
#define     AGS_GATEWAY     0x010   /* tell kernel RTF_GATEWAY */
#define     AGS_IF      0x020   /* for an interface */
#define     AGS_RIPV2       0x040   /* send only as RIPv2 */
#define     AGS_FINE_GATE   0x080   /* ignore differing ag_gate when this
                     * has the finer netmask */
#define     AGS_CORS_GATE   0x100   /* ignore differing gate when this
                     * has the coarser netmasks */
#define     AGS_SPLIT_HZ    0x200   /* suppress for split horizon */

    /* some bits are set if they are set on either route */
#define     AGS_AGGREGATE_EITHER (AGS_RIPV2 | AGS_GATEWAY |   \
                  AGS_SUPPRESS | AGS_CORS_GATE)
};


/* parameters for interfaces */
extern struct parm {
    struct parm *parm_next;
    char    parm_name[IF_NAME_LEN+1];
    naddr   parm_net;
    naddr   parm_mask;

    char    parm_d_metric;
    u_int   parm_int_state;
    int parm_rdisc_pref;    /* signed IRDP preference */
    int parm_rdisc_int;     /* IRDP advertising interval */
    struct auth parm_auth[MAX_AUTH_KEYS];
} *parms;

/* authority for internal networks */
extern struct intnet {
    struct intnet *intnet_next;
    naddr   intnet_addr;        /* network byte order */
    naddr   intnet_mask;
    char    intnet_metric;
} *intnets;

/* defined RIPv1 netmasks */
extern struct r1net {
    struct r1net *r1net_next;
    naddr   r1net_net;      /* host order */
    naddr   r1net_match;
    naddr   r1net_mask;
} *r1nets;

/* trusted routers */
extern struct tgate {
    struct tgate *tgate_next;
    naddr   tgate_addr;
#define     MAX_TGATE_NETS 32
    struct tgate_net {
        naddr   net;        /* host order */
        naddr   mask;
    } tgate_nets[MAX_TGATE_NETS];
} *tgates;

enum output_type {OUT_QUERY, OUT_UNICAST, OUT_BROADCAST, OUT_MULTICAST,
    NO_OUT_MULTICAST, NO_OUT_RIPV2};

/* common output buffers */
extern struct ws_buf {
    struct rip  *buf;
    struct netinfo  *n;
    struct netinfo  *base;
    struct netinfo  *lim;
    enum output_type type;
} v12buf, v2buf, req_buf;

extern pid_t    mypid;
extern naddr    myaddr;         /* main address of this system */

extern int  stopint;        /* !=0 to stop */

extern int  sock_max;
extern int  rip_sock;       /* RIP socket */
extern int  rt_sock;        /* routing socket */
extern int  rt_sock_seqno;
extern int  rdisc_sock;     /* router-discovery raw socket */

extern int  seqno;          /* sequence number for messages */
extern int  supplier;       /* process should supply updates */
extern int  supplier_set;       /* -s or -q requested */
extern int  lookforinterfaces;  /* 1=probe for new up interfaces */
extern int  ridhosts;       /* 1=reduce host routes */
extern int  mhome;          /* 1=want multi-homed host route */
extern int  advertise_mhome;    /* 1=must continue advertising it */
extern int  auth_ok;        /* 1=ignore auth if we do not care */

extern struct timeval clk;      /* system clock's idea of time */
extern struct timeval epoch;        /* system clock when started */
extern struct timeval now;      /* current idea of time */
extern time_t   now_stale;
extern time_t   now_expire;
extern time_t   now_garbage;

extern struct timeval next_bcast;   /* next general broadcast */
extern struct timeval age_timer;    /* next check of old routes */
extern long orig_age_usec;              /* initial value of randomized usec time */  /* LVL7_MOD */
extern struct timeval no_flash;     /* inhibit flash update until then */
extern struct timeval rdisc_timer;  /* next advert. or solicitation */
extern int rdisc_ok;            /* using solicited route */

extern struct timeval ifinit_timer; /* time to check interfaces */

extern naddr    loopaddr;       /* our address on loopback */
extern int  tot_interfaces;     /* # of remote and local interfaces */
extern int  rip_interfaces;     /* # of interfaces doing RIP */
extern struct interface *ifnet;     /* all interfaces */
extern struct interface *remote_if; /* remote interfaces */
extern int  have_ripv1_out;     /* have a RIPv1 interface */
extern int  have_ripv1_in;
extern int  need_flash;     /* flash update needed */
extern struct timeval need_kern;    /* need to update kernel table */
extern u_int    update_seqno;       /* a route has changed */
/* LVL7_MOD start */
extern struct rip_global_mibstats  rip_glob_mibstats;  /* RIPv2 MIB counters */
extern struct nbr_specific_data    nbr_data[NUM_RIP_NBRS];
extern struct nbr_specific_data    tmp_data[NUM_RIP_NBRS];
extern int ripSplitHorizon;
extern int ripAutoSummary;
extern int ripHostRoutesAccept;
/* LVL7_MOD end */

extern int  tracelevel, new_tracelevel;
#define MAX_TRACELEVEL 4
#define TRACEKERNEL (tracelevel >= 4)   /* log kernel changes */
#define TRACECONTENTS (tracelevel >= 3) /* display packet contents */
#define TRACEPACKETS (tracelevel >= 2)  /* note packets */
#define TRACEACTIONS (tracelevel != 0)
extern FILE *ftrace;        /* output trace file */
extern char inittracename[MAXPATHLEN+1];

extern struct radix_node_head *rhead;

#ifdef LVL7_ORIGINAL_VENDOR_CODE        /* LVL7_MOD start */
#else
/* rip pipe defines, structures, enums, etc */
/* We currently use only one argument to pass between mapping and vendor code.
   Since variable arguments doesn't work properly, I have declared the argument
   here.
*/
typedef struct ripPipeMsg_s
{
  unsigned int msgId;
  unsigned int arg1;
  unsigned int arg2;
  unsigned int arg3;
  unsigned int arg4;
  unsigned int arg5;
  unsigned int arg6;
  rip_redist_proto rip_proto;
}ripPipeMsg_t;

/* RIP task communication pipe defines */
#define RIP_INTF_PIPE_NAME      "/tmp/ripIntfPipe"    /* Should not be greater than
                                                    RIP_INTF_PIPE_NAME_LEN */
#define RIP_INTF_PIPE_NAME_LEN  20  /* Arbitrary Number*/
#define RIP_INTF_PIPE_MAX_MSGS  25  /* Arbitrary Number*/
#define RIP_INTF_PIPE_MSG_LEN   (sizeof(ripPipeMsg_t))

/* Message ids */
typedef enum l7_rip_pipe_msgId_e
{
  L7_RIP_INTF_STATE_CHANGE = 1,
  L7_RIP_SPLITHORIZON_MODE_CHANGE,
  L7_RIP_AUTOSUMMARY_MODE_CHANGE,
  L7_RIP_HOSTROUTESACCEPT_MODE_CHANGE,
  L7_RIP_DEFAULT_ROUTE_ADV_CHANGE,
  L7_RIP_DEFAULT_METRIC_CHANGE,
  L7_RIP_ROUTE_REDISTRIBUTE_METRIC_CHANGE,
  L7_RIP_ROUTE_REDISTRIBUTE_ROUTE_ADD,
  L7_RIP_ROUTE_REDISTRIBUTE_ROUTE_DELETE,
  L7_RIP_LAST_MODE_CHANGE
} l7_rip_pipe_msgId_t;

/* States for split horizon */
typedef enum
{
  SPHZ_NONE = 1,
  SPHZ_SIMPLE,
  SPHZ_POISONREVERSE
} rip_sphz_states;

/* States for routes auto summarization */
typedef enum
{
  AUTOSUMMARY_OFF = 1,
  AUTOSUMMARY_ON
} rip_autosummary_states;

/* States for host routes accept */
typedef enum
{
  HOSTROUTESACCEPT_OFF = 1,
  HOSTROUTESACCEPT_ON
} rip_hostroutesaccept_states;

typedef enum
{
  DEF_ROUTE_ADV_OFF = 1,
  DEF_ROUTE_ADV_ON
} rip_def_rt_adv_states;

/* RIP task communication channel */
typedef struct ripIntfCom_s
{
  unsigned char intfPipeName[RIP_INTF_PIPE_NAME_LEN];
  int intfPipe;
  unsigned int intfPipeMaxMsgs;
  unsigned int intfPipeMsgLen;
}ripIntfCom_t;

extern ripIntfCom_t      intfCom;            /* Used for communication from mapping to vendor*/
#endif /* LVL7_ORIGINAL_VENDOR_CODE */        /* LVL7_MOD end */


#ifdef sgi
/* Fix conflicts */
#define dup2(x,y)       BSDdup2(x,y)
#endif /* sgi */

extern void fix_sock(int, const char *);
extern void fix_select(void);
extern void rip_off(void);
extern void rip_on(struct interface *);

extern void bufinit(void);
extern int  output(enum output_type, struct sockaddr_in *,
           struct interface *, struct rip *, int);
extern void clr_ws_buf(struct ws_buf *, struct auth *);
extern void rip_query(void);
extern void rip_bcast(int);
extern void supply(struct sockaddr_in *, struct interface *,
           enum output_type, int, int, int);

/*extern void   msglog(const char *, ...) PATTRIB(1,2);*/
struct msg_limit {
    time_t  reuse;
    struct msg_sub {
    naddr   addr;
    time_t  until;
#   define MSG_SUBJECT_N 8
    } subs[MSG_SUBJECT_N];
};
#ifdef LVL7_ORIGINAL_VENDOR_CODE        /* LVL7_MOD start */
extern void msglim(struct msg_limit *, naddr,
               const char *, ...) PATTRIB(3,4);
#define LOGERR(msg) msglog(msg ": %s", strerror(errno))
extern void logbad(int, const char *, ...) PATTRIB(2,3);
#define BADERR(dump,msg) logbad(dump,msg ": %s", strerror(errno))
#ifdef DEBUG
#define DBGERR(dump,msg) BADERR(dump,msg)
#else
#define DBGERR(dump,msg) LOGERR(msg)
#endif

#else                                   /* LVL7_MOD */
#define BADERR(dump,msg) LOG_COMPONENT_MSG(L7_RIP_MAP_COMPONENT_ID,msg)
#define DBGERR(dump,msg) LOG_COMPONENT_MSG(L7_RIP_MAP_COMPONENT_ID,msg)

#endif /* LVL7_ORIGINAL_VENDOR_CODE */        /* LVL7_MOD end */

extern  char    *naddr_ntoa(naddr);
extern const char *saddr_ntoa(struct sockaddr *);

extern void *rtmalloc(size_t, const char *);
extern void ripTimevaladd(struct timeval *, struct timeval *);
extern void intvl_random(struct timeval *, u_long, u_long);
extern int  getnet(char *, naddr *, naddr *);
extern int  gethost(char *, naddr *);
extern void gwkludge(void);
extern const char *parse_parms(char *, int);
extern const char *check_parms(struct parm *);
extern void get_parms(struct interface *);

#ifdef LVL7_ORIGINAL_VENDOR_CODE        /* LVL7_MOD start */
extern void trace_close(int);
extern void set_tracefile(const char *, const char *, int);
extern void tracelevel_msg(const char *, int);
extern void trace_off(const char*, ...) PATTRIB(1,2);
extern void set_tracelevel(void);
extern void trace_flush(void);
extern void trace_misc(const char *, ...) PATTRIB(1,2);
extern void trace_act(const char *, ...) PATTRIB(1,2);
extern void trace_pkt(const char *, ...) PATTRIB(1,2);
extern void trace_add_del(const char *, struct rt_entry *);
extern void trace_change(struct rt_entry *, u_int, struct rt_spare *,
                 const char *);
extern void trace_if(const char *, struct interface *);
extern void trace_upslot(struct rt_entry *, struct rt_spare *,
                 struct rt_spare *);
extern void trace_rip(const char*, const char*, struct sockaddr_in *,
              struct interface *, struct rip *, int);
#endif /* LVL7_ORIGINAL_VENDOR_CODE */        /* LVL7_MOD end */

extern char *addrname(naddr, naddr, int);
extern char *rtname(naddr, naddr, naddr);

extern void rdisc_age(naddr);
extern void set_rdisc_mg(struct interface *, int);
extern void set_supplier(void);
extern void if_bad_rdisc(struct interface *);
extern void if_ok_rdisc(struct interface *);
extern void read_rip(int, struct interface *);
extern void read_rt(void);
extern void read_d(void);
extern void rdisc_adv(void);
extern void rdisc_sol(void);

extern void sigalrm(int);
extern void sigterm(int);

extern void sigtrace_on(int);
extern void sigtrace_off(int);

extern void flush_kern(void);
extern void age(naddr);

extern void ag_flush(naddr, naddr, void (*)(struct ag_info *));
extern void ag_check(naddr, naddr, naddr, naddr, char, char, u_int,
             u_short, u_short, void (*)(struct ag_info *));
extern void del_static(naddr, naddr, naddr, int);
extern void del_redirects(naddr, time_t);
extern struct rt_entry *rtget(naddr, naddr);
extern struct rt_entry *ripRtfind(naddr);
extern void rtinit(void);
extern void set_need_flash(void);
extern void rtadd(naddr, naddr, u_int, struct rt_spare *);
extern void rtchange(struct rt_entry *, u_int, struct rt_spare *, char *);
extern void rtdelete(struct rt_entry *);
extern struct rt_entry *rtfind(naddr);
extern void rts_delete(struct rt_entry *, struct rt_spare *);
extern void rtbad_sub(struct rt_entry *);
extern void rtswitch(struct rt_entry *, struct rt_spare *);
extern void rtbad(struct rt_entry *);

#define S_ADDR(x)   (((struct sockaddr_in *)(x))->sin_addr.s_addr)
#define INFO_DST(I) ((I)->rti_info[RTAX_DST])
#define INFO_GATE(I)    ((I)->rti_info[RTAX_GATEWAY])
#define INFO_MASK(I)    ((I)->rti_info[RTAX_NETMASK])
#define INFO_IFA(I) ((I)->rti_info[RTAX_IFA])
#define INFO_IFP(I) ((I)->rti_info[RTAX_IFP])
#define INFO_AUTHOR(I)  ((I)->rti_info[RTAX_AUTHOR])
#define INFO_BRD(I) ((I)->rti_info[RTAX_BRD])
void rt_xaddrs(struct rt_addrinfo *, struct sockaddr *, struct sockaddr *,
           int);

extern naddr    std_mask(naddr);
extern naddr    ripv1_mask_net(naddr, struct interface *);
extern naddr    ripv1_mask_host(naddr,struct interface *);
#define     on_net(a,net,mask) ((((a) ^ (net)) & (mask)) == 0)
extern int  check_dst(naddr);
extern struct interface *check_dup(naddr, naddr, naddr, int);
extern int  check_remote(struct interface *);
extern int  addrouteforif(struct interface *);
extern void rip_ifinit(void);
extern int  walk_bad(struct radix_node *, struct walkarg *);
extern int  walk_display(struct radix_node *, struct walkarg *);
extern int  if_ok(struct interface *, const char *);
extern void if_sick(struct interface *);
extern void if_bad(struct interface *);
extern void if_link(struct interface *);
extern struct interface *ifwithaddr(naddr, int, int);
extern struct interface *ifwithname(char *, naddr);
extern struct interface *ifwithindex(u_short, int);
extern struct interface *ifwith_index_and_addr(u_short index,
                                               naddr addr,
                                               int rescan_ok);
extern struct interface *iflookup(naddr);

extern struct auth *find_auth(struct interface *);
extern void end_md5_auth(struct ws_buf *, struct auth *);

#define MD5_DIGEST_LEN 16
typedef struct {
    u_int32_t state[4];     /* state (ABCD) */
    u_int32_t count[2];     /* # of bits, modulo 2^64 (LSB 1st) */
    unsigned char buffer[64];   /* input buffer */
} MD5_CTX;
extern void MD5Init(MD5_CTX*);
extern void MD5Update(MD5_CTX*, u_char*, u_int);
extern void MD5Final(u_char[MD5_DIGEST_LEN], MD5_CTX*);

/* LVL7_MOD start */
extern void srandom(u_long);
extern u_long random(void);

extern int ripMapVend_clock_gettime(int clockId, struct timespec *tp);
extern int ripMapVend_ioctl(int fd, int cmd, int data);
extern void ripMapVendUserParmsGet(struct interface *ifp);
extern int ripMapVendRipRoutesMaxGet(void);
extern void ripMapVendTaskControl(void);
extern int ripMapVendRnTreeCritEnter(void);
extern int ripMapVendRnTreeCritExit(void);
extern L7_RC_t ripMapVendRouteChangeCallback(int action, naddr ipAddr,
                                          naddr ipMask, naddr gateway,
                                          int metric);
extern void ripMapVendLogMsg(char *message);
extern uint ripMapVendInetNtoa (naddr ipAddr, u_char *buf, uint buflen);
extern void ripMapVendRipIntfPipeGet(int *intfPipe, uint *intfPipeMsgLen);
extern void ripMapVendGlobalsGet(int *ripSplitHorizon, int *ripAutoSummary,
                          int *ripHostRoutesAccept);
extern void ripMapVendRipRouteRedistMetricSet(rip_redist_proto rip_proto, uint metric);
extern void ripMapVendRipDefaultMetricSet(uint metric);
extern int  ripMapVendIfNumToRtrIfNum(L7_uint32 intIfNum, L7_uint32 *rtrIfNum); 
/* LVL7_MOD end */

