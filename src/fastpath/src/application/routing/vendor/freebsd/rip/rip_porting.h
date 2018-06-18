/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   rip_porting.h
*
* @purpose    RIP application porting definitions
*
* @component  RIP
*
* @comments   none
*
* @create     05/21/2001
*
* @author     gpaussa
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/



#ifndef _RIP_PORTING_H_
#define _RIP_PORTING_H_

#include "log.h"
#include "osapi.h"
#ifdef _L7_OS_LINUX_
#define _STRUCT_TIMESPEC
struct timezone {
    int tz_minuteswest; /* minutes west of Greenwich */
    int tz_dsttime; /* type of dst correction */
};
#endif

#ifdef _L7_OS_VXWORKS_
/* base data types */
typedef unsigned int    uint;
#endif

/* non-API mapping function prototypes */
extern int ripMapSysctl(int *name, u_int namelen, void *old, size_t *oldlenp,
                        void *new, size_t newlen);
extern void ripMapVendTrace(int, const char *, ...);

/* external references */
extern struct timeval   last_ifinit;
extern void ripgettimeofday( struct timeval *tv , struct timezone *not_used );

/* definitions */
#define CTL_NET           4               /* network, from sys/sysctl.h */
#define MAXPACKETSIZE   512
#define MAXPATHLEN      128             /* just made up this value */
#define NO_INSTALL      1               /* don't install RIP routes in kernel */
#define MIN(A,B)        ((A) < (B) ? (A) : (B))
#ifndef min
#define min(A,B)        ((A) < (B) ? (A) : (B))
#endif
#define INET_NTOA_BUFSIZE 16            /* buffer size should match OSAPI_INET_NTOA_BUF_SIZE */

/* global data name remapping */
#define ifnet           rip_ifnet
#define radix_node      rip_radix_node
#define radix_mask      rip_radix_mask
#define radix_node_head rip_radix_node_head
#define rn_mkfreelist   rip_rn_mkfreelist

/* global function name remapping */
#define ifinit          rip_ifinit
#define rt_xaddrs       rip_rt_xaddrs
#define addrouteforif   rip_addrouteforif
#define supply          rip_supply
#define srandom         rip_srandom
#define random          rip_random

#define rtinit          rip_rtinit
#define rtget           rip_rtget
#define rtfind          rip_rtfind
#define rtadd           rip_rtadd
#define rtchange        rip_rtchange
#define rtswitch        rip_rtswitch
#define rtdelete        rip_rtdelete
#define rtbad           rip_rtbad

#define rn_search       rip_rn_search
#define rn_search_m     rip_rn_search_m
#define rn_refines      rip_rn_refines
#define rn_lookup       rip_rn_lookup
#define rn_satisfies_leaf  rip_rn_satisfies_leaf
#define rn_match        rip_rn_match
#define rn_newpair      rip_rn_newpair
#define rn_insert       rip_rn_insert
#define rn_addmask      rip_rn_addmask
#define rn_lexobetter   rip_rn_lexobetter
#define rn_new_radix_mask  rip_rn_new_radix_mask
#define rn_addroute     rip_rn_addroute
#define rn_delete       rip_rn_delete
#define rn_walktree     rip_rn_walktree
#define rn_inithead     rip_rn_inithead
#define rn_init         rip_rn_init

#define MD5Init         md5MapInit
#define MD5Update       md5MapUpdate
#define MD5Final        md5MapFinal

#define sysctl          ripMapSysctl
#define free(_x)        osapiFree(L7_RIP_MAP_COMPONENT_ID, (_x))
#define clock_gettime   ripMapVend_clock_gettime
#define ioctl           ripMapVend_ioctl
#define get_rip_ifindex ripMapVendIfNumToRtrIfNum
#define malloc(nbytes)  osapiMalloc(L7_RIP_MAP_COMPONENT_ID, nbytes)

#define RIP_MSG_LOG(format, args...)  {\
          char trace_buf[200]; \
          osapiSnprintf(trace_buf,sizeof(trace_buf),format, ##args); \
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,"%s\n", trace_buf); \
        }

#define LOGERR(format)      RIP_MSG_LOG(format)
#define msglog(format, args...)      RIP_MSG_LOG(format, ##args)
#define printf(format, args...)      RIP_MSG_LOG(format, ##args)

#define logbad(_x, _str, args...)    RIP_MSG_LOG(_str, ##args)
#define logbad2(_x, _str)            RIP_MSG_LOG((_str))
#define syslog(_x, _str, args...)    RIP_MSG_LOG((_str), ##args)
#define msglim(_x, _naddr, _str, args...)  RIP_MSG_LOG((_str), ##args)

/* trace function mapping */
typedef enum
{
  RIP_TRACE_ACT = 1,
  RIP_TRACE_ADD_DEL,
  RIP_TRACE_CHANGE,
  RIP_TRACE_UPSLOT,
  RIP_TRACE_MISC,
  RIP_TRACE_IF,
  RIP_TRACE_RIP,
  RIP_TRACE_PKT,
  RIP_TRACE_RDISC,
  RIP_TRACE_OFF,
} ripTraceId_t;

#define trace_act1(_str) \
  ripMapVendTrace(RIP_TRACE_ACT, (_str))

#define trace_act(_str, args...) \
  ripMapVendTrace(RIP_TRACE_ACT, (_str), ##args)

#define trace_add_del(_str, _rtp) \
  ripMapVendTrace(RIP_TRACE_ADD_DEL, "Route %s: rtp=0x%8.8x", (_str), (_rtp))

#define trace_change(_rtp, _state, _newrtp, _label) \
  ripMapVendTrace(RIP_TRACE_CHANGE, \
                  "Route %s: rtp=0x%8.8x  state=0x%8.8x  newrtp=0x%8.8x", \
                  (_label), (_rtp), (_state), (_newrtp))

#define trace_upslot(_rtp, _rts, _rte) \
  ripMapVendTrace(RIP_TRACE_UPSLOT, \
                  "Up slot: rtp=0x%8.8x spare=0x%8.8x  empty=0x%8.8x", \
                  (_rtp), (_rts), (_rte))

#define trace_misc(_str, args...) \
  ripMapVendTrace(RIP_TRACE_MISC, (_str), ##args)

#define trace_if(_str, _ifp) \
  ripMapVendTrace(RIP_TRACE_IF, "IF %s: ifp=0x%8.8x", (_str), (_ifp))

#define trace_rip(_a, _b, _c, _d, _e, _f) \
  ripMapVendTrace(RIP_TRACE_RIP, "%s, %s", (_a), (_b))

#define trace_pkt1(_str) \
  ripMapVendTrace(RIP_TRACE_PKT, (_str))

#define trace_pkt(_str, args...) \
  ripMapVendTrace(RIP_TRACE_PKT, (_str), ##args)

#define trace_off(_str, args...) \
  ripMapVendTrace(RIP_TRACE_OFF, (_str), ##args)

#define fprintf(_f, _str, args...) \
  ripMapVendTrace(RIP_TRACE_RDISC, (_str), ##args)

/* PTin added: ARM processor */
#ifndef fputc
#define fputc(_c, _x) \
  ripMapVendTrace(RIP_TRACE_RDISC, "%c", (_c))
#endif

/* functions with no replacement */
#define trace_flush()
#define set_tracefile(_fn, _str, _x)
#define set_tracelevel()
#define lastlog()

#endif /* !_RIP_PORTING_H_ */
