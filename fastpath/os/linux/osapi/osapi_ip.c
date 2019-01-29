/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  osapi_ip.c
*
* @purpose   Gets the ip tables from vxworks
*
* @component ip
*
* @comments  none
*
* @create    05/07/2001
*
* @author    anayar, cpverne
*
* @end
*
**********************************************************************/

/*************************************************************

************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/in.h>

#include "l7_common.h"
#include "osapi_support.h"
#include "bspapi.h"
#include "dtlapi.h"
#include "l3end_api.h" /* still need a better name for this header file... */
#include "ipstk_api.h"
#include "osapi_priv.h"
#include "simapi.h"
#include "l7_vrrp_api.h"

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
extern void osapiIpv6NdpCacheInit(void);
#endif

#define IP_STATS_LINE   "Ip: %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu"
#define ICMP_STATS_LINE "Icmp: %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu"
#define TCP_STATS_LINE  "Tcp: %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu"
#define UDP_STATS_LINE  "Udp: %lu %lu %lu %lu"

#define IP_STATS_PREFIX_LEN 4
#define ICMP_STATS_PREFIX_LEN   6
#define TCP_STATS_PREFIX_LEN    5
#define UDP_STATS_PREFIX_LEN    5

struct ip_mib
{
    unsigned long   ipForwarding;
    unsigned long   ipDefaultTTL;
    unsigned long   ipInReceives;
    unsigned long   ipInHdrErrors;
    unsigned long   ipInAddrErrors;
    unsigned long   ipForwDatagrams;
    unsigned long   ipInUnknownProtos;
    unsigned long   ipInDiscards;
    unsigned long   ipInDelivers;
    unsigned long   ipOutRequests;
    unsigned long   ipOutDiscards;
    unsigned long   ipOutNoRoutes;
    unsigned long   ipReasmTimeout;
    unsigned long   ipReasmReqds;
    unsigned long   ipReasmOKs;
    unsigned long   ipReasmFails;
    unsigned long   ipFragOKs;
    unsigned long   ipFragFails;
    unsigned long   ipFragCreates;
    unsigned long   ipRoutingDiscards;
};

struct udp_mib
{
    unsigned long   udpInDatagrams;
    unsigned long   udpNoPorts;
    unsigned long   udpInErrors;
    unsigned long   udpOutDatagrams;
};

struct icmp_mib
{
    unsigned long   icmpInMsgs;
    unsigned long   icmpInErrors;
    unsigned long   icmpInDestUnreachs;
    unsigned long   icmpInTimeExcds;
    unsigned long   icmpInParmProbs;
    unsigned long   icmpInSrcQuenchs;
    unsigned long   icmpInRedirects;
    unsigned long   icmpInEchos;
    unsigned long   icmpInEchoReps;
    unsigned long   icmpInTimestamps;
    unsigned long   icmpInTimestampReps;
    unsigned long   icmpInAddrMasks;
    unsigned long   icmpInAddrMaskReps;
    unsigned long   icmpOutMsgs;
    unsigned long   icmpOutErrors;
    unsigned long   icmpOutDestUnreachs;
    unsigned long   icmpOutTimeExcds;
    unsigned long   icmpOutParmProbs;
    unsigned long   icmpOutSrcQuenchs;
    unsigned long   icmpOutRedirects;
    unsigned long   icmpOutEchos;
    unsigned long   icmpOutEchoReps;
    unsigned long   icmpOutTimestamps;
    unsigned long   icmpOutTimestampReps;
    unsigned long   icmpOutAddrMasks;
    unsigned long   icmpOutAddrMaskReps;
};


struct tcp_mib
{
    unsigned long   tcpRtoAlgorithm;
    unsigned long   tcpRtoMin;
    unsigned long   tcpRtoMax;
    unsigned long   tcpMaxConn;
    unsigned long   tcpActiveOpens;
    unsigned long   tcpPassiveOpens;
    unsigned long   tcpAttemptFails;
    unsigned long   tcpEstabResets;
    unsigned long   tcpCurrEstab;
    unsigned long   tcpInSegs;
    unsigned long   tcpOutSegs;
    unsigned long   tcpRetransSegs;
    unsigned long   tcpInErrs;
    unsigned long   tcpOutRsts;
    short       tcpInErrsValid;
    short       tcpOutRstsValid;
};


static struct ip_mib ip_mib_stats;
static struct icmp_mib icmp_mib_stats;
static struct tcp_mib tcp_mib_stats;
static struct udp_mib udp_mib_stats;

/*
 *update the cached copies of the IP group stats
 *by re-reading the values in /proc/net/snmp
 *Note: I really should put this on a SIGALRM
 *so I don't read the file every time I want
 *one stat, but I'll get to fixing that
 */
static int update_mib_group_stats(void)
{
  FILE *in = fopen ("/proc/net/snmp", "r");
  char line [1024];

  if(!in) {
    printf("unable to open /proc/net/snmp!\n");
    return -1;
  }



  while (line == fgets (line, sizeof(line), in)) {
    if (!strncmp( line, IP_STATS_LINE, IP_STATS_PREFIX_LEN )) {
      sscanf  ( line, IP_STATS_LINE,
        &ip_mib_stats.ipForwarding,
        &ip_mib_stats.ipDefaultTTL,
        &ip_mib_stats.ipInReceives,
        &ip_mib_stats.ipInHdrErrors,
        &ip_mib_stats.ipInAddrErrors,
        &ip_mib_stats.ipForwDatagrams,
        &ip_mib_stats.ipInUnknownProtos,
        &ip_mib_stats.ipInDiscards,
        &ip_mib_stats.ipInDelivers,
        &ip_mib_stats.ipOutRequests,
        &ip_mib_stats.ipOutDiscards,
        &ip_mib_stats.ipOutNoRoutes,
        &ip_mib_stats.ipReasmTimeout,
        &ip_mib_stats.ipReasmReqds,
        &ip_mib_stats.ipReasmOKs,
        &ip_mib_stats.ipReasmFails,
        &ip_mib_stats.ipFragOKs,
        &ip_mib_stats.ipFragFails,
        &ip_mib_stats.ipFragCreates);
      ip_mib_stats.ipRoutingDiscards = 0;   /* XXX */
    }
    else if (!strncmp( line, ICMP_STATS_LINE, ICMP_STATS_PREFIX_LEN )) {
      sscanf ( line, ICMP_STATS_LINE,
           &icmp_mib_stats.icmpInMsgs,
           &icmp_mib_stats.icmpInErrors,
           &icmp_mib_stats.icmpInDestUnreachs,
           &icmp_mib_stats.icmpInTimeExcds,
           &icmp_mib_stats.icmpInParmProbs,
           &icmp_mib_stats.icmpInSrcQuenchs,
           &icmp_mib_stats.icmpInRedirects,
           &icmp_mib_stats.icmpInEchos,
           &icmp_mib_stats.icmpInEchoReps,
           &icmp_mib_stats.icmpInTimestamps,
           &icmp_mib_stats.icmpInTimestampReps,
           &icmp_mib_stats.icmpInAddrMasks,
           &icmp_mib_stats.icmpInAddrMaskReps,
           &icmp_mib_stats.icmpOutMsgs,
           &icmp_mib_stats.icmpOutErrors,
           &icmp_mib_stats.icmpOutDestUnreachs,
           &icmp_mib_stats.icmpOutTimeExcds,
           &icmp_mib_stats.icmpOutParmProbs,
           &icmp_mib_stats.icmpOutSrcQuenchs,
           &icmp_mib_stats.icmpOutRedirects,
           &icmp_mib_stats.icmpOutEchos,
           &icmp_mib_stats.icmpOutEchoReps,
           &icmp_mib_stats.icmpOutTimestamps,
           &icmp_mib_stats.icmpOutTimestampReps,
           &icmp_mib_stats.icmpOutAddrMasks,
           &icmp_mib_stats.icmpOutAddrMaskReps);
    }
    else if (!strncmp( line, TCP_STATS_LINE, TCP_STATS_PREFIX_LEN )) {
      int ret = sscanf ( line, TCP_STATS_LINE,
             &tcp_mib_stats.tcpRtoAlgorithm,
             &tcp_mib_stats.tcpRtoMin,
             &tcp_mib_stats.tcpRtoMax,
             &tcp_mib_stats.tcpMaxConn,
             &tcp_mib_stats.tcpActiveOpens,
             &tcp_mib_stats.tcpPassiveOpens,
             &tcp_mib_stats.tcpAttemptFails,
             &tcp_mib_stats.tcpEstabResets,
             &tcp_mib_stats.tcpCurrEstab,
             &tcp_mib_stats.tcpInSegs,
             &tcp_mib_stats.tcpOutSegs,
             &tcp_mib_stats.tcpRetransSegs,
             &tcp_mib_stats.tcpInErrs,
             &tcp_mib_stats.tcpOutRsts);
      tcp_mib_stats.tcpInErrsValid = (ret > 12) ? 1 : 0;
      tcp_mib_stats.tcpOutRstsValid = (ret > 13) ? 1 : 0;
    }
    else if (!strncmp( line, UDP_STATS_LINE, UDP_STATS_PREFIX_LEN )) {
      sscanf ( line, UDP_STATS_LINE,
           &udp_mib_stats.udpInDatagrams,
           &udp_mib_stats.udpNoPorts,
           &udp_mib_stats.udpInErrors,
           &udp_mib_stats.udpOutDatagrams);
    }
  }
  fclose (in);

  /*
   * Tweak illegal values:
   *
   * valid values for ipForwarding are 1 == yup, 2 == nope
   * a 0 is forbidden, so patch:
   */
  if (! ip_mib_stats.ipForwarding)
    ip_mib_stats.ipForwarding = 2;

  /*
   * 0 is illegal for tcpRtoAlgorithm
   * so assume `other' algorithm:
   */
  if (! tcp_mib_stats.tcpRtoAlgorithm)
    tcp_mib_stats.tcpRtoAlgorithm = 1;
  return 0;

}



/*
*********************************************************************
*This is the entry point for the monitor task for SNMP under linux
*It sleeps for 10 seconds and then updates the cached copies
*of the mibII stats from /proc/net/snmp
**********************************************************************
*/
void L7_snmp_proc_monitor()
{
   while(1)
   {
     sleep(10);
     update_mib_group_stats();
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
     /* this isnt necessary under normal circumstances as reading
        a cache entry clears it. Its here to handle the case where
        a cache dump doesnt complete and because there is little
        reason to add an extra task.
     */
     osapiIpv6NdpCacheInit();
#endif
   }
}
    

/*
********************************************************************
*                     LINUX GETTERS
********************************************************************
*/


/*********************************************************************
* @purpose Determines if ip is forwarding or not
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
L7_RC_t osapiIpForwardingGet(L7_int32 *val)
{
  *val = ip_mib_stats.ipForwarding;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose Get the default ttl
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
L7_RC_t osapiIpDefaultTTLGet(L7_int32 *val)
{
  *val = ip_mib_stats.ipDefaultTTL;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Get the total number of input datagrams received from interfaces
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
L7_RC_t osapiIpInReceivesGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipInReceives;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Get the total number of input datagrams discarded due to headers
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpInHdrErrorsGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipInHdrErrors;
        
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the number of input datagrams discarded because
*          the IP address in their IP header's destination
*          field was not a valid address to be received at
*          this entity.
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpInAddrErrorsGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipInAddrErrors;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose          Gets the number of input datagrams for which this
*                   entity was not their final IP destination, as a
*                   result of which an attempt was made to find a
*                   route to forward them to that final destination.
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpForwDatagramsGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipForwDatagrams;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the number of locally-addressed datagrams
*          received successfully but discarded because of an
*          unknown or unsupported protocol.
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpInUnknownProtosGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipInUnknownProtos;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the number of input IP datagrams for which no
*          problems were encountered to prevent their
*          continued processing, but which were discarded
*
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpInDiscardsGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipInDiscards;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the total number of input datagrams successfully
*          delivered to IP user-protocols (including ICMP).
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpInDeliversGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipInDelivers;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose Gets the total number of IP datagrams which local IP
*          user-protocols (including ICMP) supplied to IP in
*          requests for transmission.
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments Note that this counter
*           does not include any datagrams counted in
*           ipForwDatagrams
*
* @end
*********************************************************************/
L7_RC_t osapiIpOutRequestsGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipOutRequests;
        
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the number of output IP datagrams for which no
*          problem was encountered to prevent their
*          transmission to their destination, but which were
*          discarded (e.g., for lack of buffer space).
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpOutDiscardsGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipOutDiscards;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the number of IP datagrams discarded because no
*          route could be found to transmit them to their
*          destination.
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpOutNoRoutesGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipOutNoRoutes;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the maximum number of seconds which received
*          fragments are held while they are awaiting
*          reassembly at this entity
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpReasmTimeoutGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipReasmTimeout;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the number of IP fragments received which needed
*          to be reassembled at this entity
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpReasmReqdsGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipReasmReqds;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the number of IP datagrams successfully re-assembled
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpReasmOKsGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipReasmOKs;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the number of failures detected by the IP re-
*          assembly algorithm (for whatever reason: timed
*          out, errors, etc).
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpReasmFailsGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipReasmFails;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the number of IP datagrams that have been
*          successfully fragmented at this entity
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpFragOKsGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipFragOKs;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the number of IP datagrams that have been
*          discarded because they needed to be fragmented at
*          this entity but could not be, e.g., because their
*          Don't Fragment flag was set.
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpFragFailsGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipFragFails;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the number of IP datagram fragments that have
*          been generated as a result of fragmentation at
*          this entity.
*
* @param val        @b{(output)} datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpFragCreatesGet(L7_uint32 *val)
{
    *val = ip_mib_stats.ipFragCreates;
    
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the number of routing entries which were chosen
*          to be discarded even though they are valid.
*
* @param val        @b{(output)} Address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIpRoutingDiscardsGet(L7_uint32 *val)
{
  *val = ip_mib_stats.ipRoutingDiscards;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The total number of ICMP messages which the entity
*           received.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments  Note that this counter includes all those counted by
*            icmpInErrors
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpInMsgsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpInMsgs;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP messages which the entity received
*           but determined as having ICMP specific errors (bad ICMP
*           checksums, bad length, etc.)
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpInErrorsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpInErrors;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Destination Unreachable messages
*           received.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpInDestUnreachsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpInDestUnreachs;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose The number of ICMP Time Exceeded messages received.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpInTimeExcdsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpInTimeExcds;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose The number of ICMP Parameter Problem messages received
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpInParmProbsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpInParmProbs;
    
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  The number of ICMP Source Quench messages.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpInSrcQuenchsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpInSrcQuenchs;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Redirect messages received.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpInRedirectsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpInRedirects;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose The number of ICMP Echo (request) messages received.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpInEchosGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpInEchos;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Echo Reply messages received
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpInEchoRepsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpInEchoReps;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Timestamp (request) messages received
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpInTimestampsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpInTimestamps;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Timestamp Reply messages
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpInTimestampRepsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpInTimestampReps;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Address Mask Request messages received
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpInAddrMasksGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpInAddrMasks;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Address Mask Reply messages received
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpInAddrMaskRepsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpInAddrMaskReps;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The total number of ICMP messages which this entity
*           attempted to send.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments  Note that this counter includes all those counted by
*            icmpOutErrors
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpOutMsgsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpOutMsgs;
  /* In LINUX ICMP out Echos are handled in the Application */
  *val += osapiL7IcmpOutEchosGet();    
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP messages which this entity did not
*           send due to problems discovered within ICMP.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments  This includes problems such as lack of buffers. This
*            value should not include errors discovered outside of
*            the ICMP layer such as the inability of IP to route the
*            resultant datagram. In some implementations there may
*            be no types of error which contribute to this counter's
*            value.
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpOutErrorsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpOutErrors;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Destination Unreachable messages
*           sent
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpOutDestUnreachsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpOutDestUnreachs;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Time Exceeded messages sent.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpOutTimeExcdsGet(L7_uint32 *val)
{
  *val =  icmp_mib_stats.icmpOutTimeExcds;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Parameter Problem messages sent.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpOutParmProbsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpOutParmProbs;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Source Quench messages sent.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpOutSrcQuenchsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpOutSrcQuenchs;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Redirect messages sent.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments  For a host, this object will always be zero, since
*            hosts do not send redirects.
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpOutRedirectsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpOutRedirects;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Echo (request) messages sent.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpOutEchosGet(L7_uint32 *val)
{
/* Returning Success for LINUX, as we are incrementing Out Echos in the application itself */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Echo Reply messages sent.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpOutEchoRepsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpOutEchoReps;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Timestamp (request) messages sent.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpOutTimestampsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpOutTimestamps;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Timestamp Reply messages sent
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpOutTimestampRepsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpOutTimestampReps;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Address mask Request messages sent.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpOutAddrMasksGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpOutAddrMasks;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The number of ICMP Address Mask Reply messages sent.
*
* @param val        @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpOutAddrMaskRepsGet(L7_uint32 *val)
{
  *val = icmp_mib_stats.icmpOutAddrMaskReps;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the size of the largest IP datagram which this
*           entity can re-assemble from incoming IP fragmented
*           datagrams received on this interface
*
* @param ipAddr        @b{(input)}
* @param *maxSize      @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t osapiIpAdEntReasmMaxSizeGet(L7_uint32 ipAddr, L7_uint32 *maxSize)
{
  #define IPADENTREASMMAXSIZE 65535

  *maxSize = IPADENTREASMMAXSIZE;
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Gets the size of the largest IP datagram which this
*           entity can re-assemble from incoming IP fragmented
*           datagrams received on this interface
*
* @param ipAddr        @b{(input)}
* @param *maxSize      @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This functions gets the value from the file
*           /proc/sys/net/ipv4/ipfrag_high_thresh
*
* @end
*********************************************************************/
L7_RC_t osapiIpAdEntReasmMaxMemorySizeGet(L7_uint32 ipAddr, L7_uint32 *maxSize)
{
  FILE *fp = fopen ("/proc/sys/net/ipv4/ipfrag_high_thresh", "r");

  if( fp != NULL)
  {
    if(0 > fscanf(fp, "%d", maxSize)){}
    fclose(fp);
    return L7_SUCCESS;
  }

  return L7_FAILURE;

}

/*********************************************************************
* @purpose determines if the entry exists in the ipNetToMedia table
*
* @param ipNetToMediaIfIndex        @b{(input)} interface number
* @param ipNetToMediaNetAddress     @b{(input)} network address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
L7_RC_t osapiIpNetToMediaEntryGet(L7_uint32 ipNetToMediaIfIndex,
                                  L7_uint32 ipNetToMediaNetAddress)
{
   struct arpreq arpreq;
   struct sockaddr_in *sin;
   int fd;
   char ifName[IFNAMSIZ];
   nimUSP_t usp;
   L7_uint32 intIfNum;

   /* Determine interface name to use... */
   usp.unit = simGetThisUnit();
   usp.slot = L7_CPU_SLOT_NUM;
   usp.port = 1;

   if (ipNetToMediaIfIndex == 0) {

      sprintf(ifName, "%s%d", bspapiServicePortNameGet(),bspapiServicePortUnitGet());

   } else if ((nimGetIntIfNumFromUSP(&usp, &intIfNum) == L7_SUCCESS)
           && (ipNetToMediaIfIndex == intIfNum)) {

      sprintf(ifName, "%s0", L7_DTL_PORT_IF);

   } else { /* This is the guts of ipmRouterIfNameStringGet()...replicated
               here to enable switching-only builds to link... */

      if(nimGetUnitSlotPort(ipNetToMediaIfIndex, &usp) != L7_SUCCESS) {

         return L7_FAILURE;

      }

      sprintf(ifName, "%s%d_%d", L3INTF_DEVICE_NAME, usp.slot, usp.port-1);

   }

   if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {

      close(fd);
      return (L7_FAILURE);

   }

   memset(&arpreq, 0, sizeof(arpreq));

   sin = (struct sockaddr_in *)&arpreq.arp_pa;
   sin->sin_family = AF_INET;
   sin->sin_addr.s_addr = osapiHtonl(ipNetToMediaNetAddress);

   arpreq.arp_ha.sa_family = AF_UNSPEC;

   strcpy(arpreq.arp_dev, ifName);

   if (ioctl(fd, SIOCGARP, &arpreq) == -1) {

      close(fd);
      return (L7_FAILURE);

   }

   close(fd);

   return (L7_SUCCESS);
}

/*********************************************************************
* @purpose get the next valid entry in the ipNetToMedia table
*
* @param ipNetToMediaIfIndex        @b{(output)} internal interface number
* @param ipNetToMediaNetAddress     @b{(output)} network address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
L7_RC_t osapiIpNetToMediaEntryNextGet(L7_uint32 *ipNetToMediaIfIndex, L7_uint32 *ipNetToMediaNetAddress)
{
#if 0 /* This logic is very slow and takes 10 min on linux hence rolling back */
   struct ifreq ifr;
   struct arpreq arpreq;
   struct sockaddr_in *sin;
   int fd;
   char ifName[IFNAMSIZ];
   nimUSP_t usp;
   L7_uint32 intIfNum;
   L7_uint32 ifAddr;
   L7_uint32 ifMask;
   L7_uint32 ip_min;
   L7_uint32 ip_max;
   L7_uint32 ip;

   for (;;) {

      /* Determine interface name to use... */
      usp.unit = simGetThisUnit();
      usp.slot = L7_CPU_SLOT_NUM;
      usp.port = 1;

      if (*ipNetToMediaIfIndex == 0) {

         sprintf(ifName, "%s0", bspapiServicePortNameGet());

      } else if ((nimGetIntIfNumFromUSP(&usp, &intIfNum) == L7_SUCCESS)
              && (*ipNetToMediaIfIndex == intIfNum)) {

         sprintf(ifName, "%s0", L7_DTL_PORT_IF);

      } else { /* This is the guts of ipmRouterIfNameStringGet()...replicated
                  here to enable switching-only builds to link... */

         if(nimGetUnitSlotPort(*ipNetToMediaIfIndex, &usp) != L7_SUCCESS) {

            return L7_FAILURE;

         }

         sprintf(ifName, "%s%d_%d", L3INTF_DEVICE_NAME, usp.slot, usp.port-1);

      }

      if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {

         close(fd);
         return (L7_FAILURE);

      }

      memset(&ifr, 0, sizeof(ifr));

      strncpy(ifr.ifr_name, ifName, (IFNAMSIZ-1));
      ifr.ifr_name[strlen(ifName)] = '\0';

      if (ioctl(fd, SIOCGIFADDR, (int)&ifr) == -1) {

         close(fd);
         return (L7_FAILURE);

      }

      ifAddr = osapiNtohl(((struct sockaddr_in *)
                      &ifr.ifr_addr)->sin_addr.s_addr);

      if (ioctl(fd, SIOCGIFNETMASK, (int)&ifr) == -1) {

         close(fd);
         return (L7_FAILURE);

      }

      ifMask = osapiNtohl(((struct sockaddr_in *)
                      &ifr.ifr_netmask)->sin_addr.s_addr);

      ip_min = (ifAddr & ifMask);

      ip_max = (ip_min | ~ifMask);

      memset(&arpreq, 0, sizeof(arpreq));

      sin = (struct sockaddr_in *)&arpreq.arp_pa;
      sin->sin_family = AF_INET;

      arpreq.arp_ha.sa_family = AF_UNSPEC;

      strcpy(arpreq.arp_dev, ifName);

      for (ip = ip_min; ip < ip_max; ip++) {

         if (ip == ifAddr) continue;

         sin->sin_addr.s_addr = osapiHtonl(ip);

         if (ioctl(fd, SIOCGARP, &arpreq) == -1) {

            continue;

         } else {

            if ((arpreq.arp_flags & ATF_COM) == 0) continue;

            *ipNetToMediaNetAddress = ip;

            close(fd);

            return (L7_SUCCESS);

         }

      }

      (*ipNetToMediaIfIndex)++;

   }

   close(fd);
#endif /* 0 */

   return (L7_FAILURE);
}

/*********************************************************************
* @purpose get the ipNetToMediaPhysAddress object
*
* @param ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param ipNetToMediaNetAddress     @b{(input)}  network address
* @param physAddress                @b{(output)} physAddress buffer
* @param addrLength                 @b{(output)} physAddress buffer length
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
L7_RC_t osapiIpNetToMediaPhysAddressGet(L7_uint32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_char8 *physAddress, L7_uint32 *addrLength)
{
   struct arpreq arpreq;
   struct sockaddr_in *sin;
   int fd;
   char ifName[IFNAMSIZ];
   nimUSP_t usp;
   L7_uint32 intIfNum;

   /* Determine interface name to use... */
   usp.unit = simGetThisUnit();
   usp.slot = L7_CPU_SLOT_NUM;
   usp.port = 1;

   if (ipNetToMediaIfIndex == 0) {

      sprintf(ifName, "%s%d", bspapiServicePortNameGet(),bspapiServicePortUnitGet());

   } else if ((nimGetIntIfNumFromUSP(&usp, &intIfNum) == L7_SUCCESS)
           && (ipNetToMediaIfIndex == intIfNum)) {

      sprintf(ifName, "%s0", L7_DTL_PORT_IF);

   } else { /* This is the guts of ipmRouterIfNameStringGet()...replicated
               here to enable switching-only builds to link... */

      if(nimGetUnitSlotPort(ipNetToMediaIfIndex, &usp) != L7_SUCCESS) {

         return L7_FAILURE;

      }

      sprintf(ifName, "%s%d_%d", L3INTF_DEVICE_NAME, usp.slot, usp.port-1);

   }

   if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {

      close(fd);
      return (L7_FAILURE);

   }

   memset(&arpreq, 0, sizeof(arpreq));

   sin = (struct sockaddr_in *)&arpreq.arp_pa;
   sin->sin_family = AF_INET;
   sin->sin_addr.s_addr = osapiHtonl(ipNetToMediaNetAddress);

   arpreq.arp_ha.sa_family = AF_UNSPEC;

   strcpy(arpreq.arp_dev, ifName);

   if (ioctl(fd, SIOCGARP, &arpreq) == -1) {

      close(fd);
      return (L7_FAILURE);

   }

   close(fd);

   *addrLength = L7_MAC_ADDR_LEN;
   memcpy(physAddress, &arpreq.arp_ha.sa_data, *addrLength);

   return (L7_SUCCESS);
}


/*********************************************************************
* @purpose set the ipNetToMediaPhysAddress object
*
* @param ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param ipNetToMediaNetAddress     @b{(input)}  network address
* @param physAddress                @b{(input)}  physAddress buffer
* @param addrLength                 @b{(input)}  physAddress buffer length
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
L7_RC_t osapiIpNetToMediaPhysAddressSet(L7_uint32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_char8 *physAddress, L7_uint32 addrLength)
{
   struct arpreq arpreq;
   struct sockaddr_in *sin;
   int fd;
   char ifName[IFNAMSIZ];
   nimUSP_t usp;
   L7_uint32 intIfNum;

   /* Determine interface name to use... */
   usp.unit = simGetThisUnit();
   usp.slot = L7_CPU_SLOT_NUM;
   usp.port = 1;

   if (ipNetToMediaIfIndex == 0) {

      sprintf(ifName, "%s%d", bspapiServicePortNameGet(),bspapiServicePortUnitGet());

   } else if ((nimGetIntIfNumFromUSP(&usp, &intIfNum) == L7_SUCCESS)
           && (ipNetToMediaIfIndex == intIfNum)) {

      sprintf(ifName, "%s0", L7_DTL_PORT_IF);

   } else { /* This is the guts of ipmRouterIfNameStringGet()...replicated
               here to enable switching-only builds to link... */

      if(nimGetUnitSlotPort(ipNetToMediaIfIndex, &usp) != L7_SUCCESS) {

         return L7_FAILURE;

      }

      sprintf(ifName, "%s%d_%d", L3INTF_DEVICE_NAME, usp.slot, usp.port-1);

   }

   if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {

      close(fd);
      return (L7_FAILURE);

   }

   memset(&arpreq, 0, sizeof(arpreq));

   sin = (struct sockaddr_in *)&arpreq.arp_pa;
   sin->sin_family = AF_INET;
   sin->sin_addr.s_addr = osapiHtonl(ipNetToMediaNetAddress);

   arpreq.arp_ha.sa_family = AF_UNSPEC;
   memcpy(physAddress, &arpreq.arp_ha.sa_data, addrLength);

   /* Assuming this should be a "static" entry... */
   arpreq.arp_flags = ATF_COM | ATF_PERM;

   strcpy(arpreq.arp_dev, ifName);

   if (ioctl(fd, SIOCSARP, &arpreq) == -1) {

      close(fd);
      return (L7_FAILURE);

   }

   close(fd);

   return (L7_SUCCESS);
}

/*********************************************************************
* @purpose get the ipNetToMediaType object
*
* @param ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param ipNetToMediaNetAddress     @b{(input)}  network address
* @param ipNetToMediaType           @b{(output)} ipNetToMediaType
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
L7_RC_t osapiIpNetToMediaTypeGet(L7_uint32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_uint32 *ipNetToMediaType)
{
   struct arpreq arpreq;
   struct sockaddr_in *sin;
   int fd;
   char ifName[IFNAMSIZ];
   nimUSP_t usp;
   L7_uint32 intIfNum;

   /* Determine interface name to use... */
   usp.unit = simGetThisUnit();
   usp.slot = L7_CPU_SLOT_NUM;
   usp.port = 1;

   if (ipNetToMediaIfIndex == 0) {

      sprintf(ifName, "%s%d", bspapiServicePortNameGet(),bspapiServicePortUnitGet());

   } else if ((nimGetIntIfNumFromUSP(&usp, &intIfNum) == L7_SUCCESS)
           && (ipNetToMediaIfIndex == intIfNum)) {

      sprintf(ifName, "%s0", L7_DTL_PORT_IF);

   } else { /* This is the guts of ipmRouterIfNameStringGet()...replicated
               here to enable switching-only builds to link... */

      if(nimGetUnitSlotPort(ipNetToMediaIfIndex, &usp) != L7_SUCCESS) {

         return L7_FAILURE;

      }

      sprintf(ifName, "%s%d_%d", L3INTF_DEVICE_NAME, usp.slot, usp.port-1);

   }

   if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {

      close(fd);
      return (L7_FAILURE);

   }

   memset(&arpreq, 0, sizeof(arpreq));

   sin = (struct sockaddr_in *)&arpreq.arp_pa;
   sin->sin_family = AF_INET;
   sin->sin_addr.s_addr = osapiHtonl(ipNetToMediaNetAddress);

   arpreq.arp_ha.sa_family = AF_UNSPEC;

   strcpy(arpreq.arp_dev, ifName);

   if (ioctl(fd, SIOCGARP, &arpreq) == -1) {

      close(fd);
      return (L7_FAILURE);

   }

   close(fd);

   if ((arpreq.arp_flags & ATF_COM) == 0) {

      *ipNetToMediaType = L7_IP_AT_TYPE_INVALID;

   } else if ((arpreq.arp_flags & ATF_PERM) == 0) {

      *ipNetToMediaType = L7_IP_AT_TYPE_DYNAMIC;

   } else {

      *ipNetToMediaType = L7_IP_AT_TYPE_STATIC;

   }

   return (L7_SUCCESS);
}

/*********************************************************************
* @purpose set the ipNetToMediaType object
*
* @param ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param ipNetToMediaNetAddress     @b{(input)}  network address
* @param ipNetToMediaType           @b{(input)} ipNetToMediaType
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
L7_RC_t osapiIpNetToMediaTypeSet(L7_uint32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_uint32 ipNetToMediaType)
{
   struct arpreq arpreq;
   struct sockaddr_in *sin;
   int fd;
   char ifName[IFNAMSIZ];
   nimUSP_t usp;
   L7_uint32 intIfNum;

   /* Determine interface name to use... */
   usp.unit = simGetThisUnit();
   usp.slot = L7_CPU_SLOT_NUM;
   usp.port = 1;

   if (ipNetToMediaIfIndex == 0) {

      sprintf(ifName, "%s%d", bspapiServicePortNameGet(),bspapiServicePortUnitGet());

   } else if ((nimGetIntIfNumFromUSP(&usp, &intIfNum) == L7_SUCCESS)
           && (ipNetToMediaIfIndex == intIfNum)) {

      sprintf(ifName, "%s0", L7_DTL_PORT_IF);

   } else { /* This is the guts of ipmRouterIfNameStringGet()...replicated
               here to enable switching-only builds to link... */

      if(nimGetUnitSlotPort(ipNetToMediaIfIndex, &usp) != L7_SUCCESS) {

         return L7_FAILURE;

      }

      sprintf(ifName, "%s%d_%d", L3INTF_DEVICE_NAME, usp.slot, usp.port-1);

   }

   if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {

      close(fd);
      return (L7_FAILURE);

   }

   memset(&arpreq, 0, sizeof(arpreq));

   sin = (struct sockaddr_in *)&arpreq.arp_pa;
   sin->sin_family = AF_INET;
   sin->sin_addr.s_addr = osapiHtonl(ipNetToMediaNetAddress);

   arpreq.arp_ha.sa_family = AF_UNSPEC;

   strcpy(arpreq.arp_dev, ifName);

   if (ioctl(fd, SIOCGARP, &arpreq) == -1) {

      close(fd);
      return (L7_FAILURE);

   }

   switch (ipNetToMediaType) {

      case L7_IP_AT_TYPE_INVALID:
         arpreq.arp_flags &= (ATF_COM | ATF_PERM);
         break;

      case L7_IP_AT_TYPE_DYNAMIC:
         arpreq.arp_flags &= ATF_PERM;
         break;

      case L7_IP_AT_TYPE_STATIC:
         arpreq.arp_flags |= ATF_PERM;
         break;

      default:
         close(fd);
         return (L7_FAILURE);

   }

   if (ioctl(fd, SIOCSARP, &arpreq) == -1) {

      close(fd);
      return (L7_FAILURE);

   }

   close(fd);

   return (L7_SUCCESS);
}

/*********************************************************************
* @purpose  Add the VRRP interface name,VRID and VRIP in the stack
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    vrid        @b{(input)} VRRP group ID
* @param    addresses   @b{(input)} Pointer to vr ip addresses
* @param    netmask     @b{(input)} Pointer to subnet mask array
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*   
* @end
*********************************************************************/
L7_RC_t osapiVrrpMasterVrrpAddressAdd(L7_uint32 intIfNum, L7_uchar8 vrid,
                                      L7_uint32 *addresses, L7_uint32 *netmask)
{
  L7_uint32    i;	
  nimUSP_t     usp;
  struct ifreq ifr;
  L7_uchar8    buf[OSAPI_INET_NTOA_BUF_SIZE];

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  for (i = 0; i < L7_L3_NUM_IP_ADDRS; i++)
  {
    if (! addresses[i])
    {
      continue;
    }

    memset(&ifr, 0x00, sizeof(struct ifreq));

    osapiSnprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s%d_%d_%d:%d_%d",
                  L3INTF_DEVICE_NAME, usp.unit, usp.slot, usp.port-1, vrid, i);		  

    if (ipstkIfAddrSet(ifr.ifr_name, addresses[i]) != L7_SUCCESS)
    {
      memset(buf, 0, sizeof(buf));
      osapiInetNtoa(addresses[i], buf);

      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
              "Failed to Set VRRP IP Address %s on interface %s in the linux stack",
              buf, ifr.ifr_name);
      continue;
    }

    if (ipstkIfMaskSet(ifr.ifr_name, netmask[i]) != L7_SUCCESS)
    {
      memset(buf, 0, sizeof(buf));
      osapiInetNtoa(netmask[i], buf);

      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
              "Failed to Set VRRP IP Address mask %s on interface %s in the linux stack.",
              buf, ifr.ifr_name);
    }
  }
 
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the VRRP interface name,VRID and VRIP in the stack
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    vrid        @b{(input)} VRRP group ID
* @param    addresses   @b{(input)} Pointer to vr ip addresses
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*   
* @end
*********************************************************************/
L7_RC_t osapiVrrpMasterVrrpAddressDelete(L7_uint32 intIfNum, L7_uchar8 vrid,
                                         L7_uint32 *addresses)
{
  L7_uint32    i;	
  nimUSP_t     usp;
  struct ifreq ifr;
  L7_uchar8    buf[OSAPI_INET_NTOA_BUF_SIZE];


  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  for (i = 0; i < L7_L3_NUM_IP_ADDRS; i++)
  {
    if (! addresses[i])
    {
      continue;
    }
    memset(&ifr, 0x00, sizeof(struct ifreq));

    osapiSnprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s%d_%d_%d:%d_%d",
                  L3INTF_DEVICE_NAME, usp.unit, usp.slot, usp.port-1, vrid, i);

    if (ipstkIfAddrDel(ifr.ifr_name, addresses[i]) != L7_SUCCESS)
    {
      memset(buf, 0, sizeof(buf));
      osapiInetNtoa(addresses[i], buf);

      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_OSAPI_COMPONENT_ID,
              "Failed to delete VRRP IP Address %s on interface %s in the linux stack.",
              buf, ifr.ifr_name);
    }
  }
 
  return L7_SUCCESS;
}

