
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   osapi_sockdefs.h
*
* @purpose    socket definitions for osapi users
*
* @component  osapi
*
* @comments   none
*
* @create     
*
* @author     jpp
* @end
*
**********************************************************************/


#ifndef _OSAPI_SOCKDEFS_H
#define _OSAPI_SOCKDEFS_H

#include "l3_addrdefs.h"

/*NOTE WELL: these values are NOT those of underlying stack.
  Do not confuse L7_AF_INET and AF_INET,eg. Values differ
  across platforms and we intentionally dont want to include
  a way to resolve them here. (would hose many things). 
*/


#define L7_SOCK_RAW          1
#define L7_SOCK_DGRAM        2
#define L7_SOCK_STREAM       3




/*
 ****************************************************************************
 *                         setsockopt/getsockopt
 ****************************************************************************
 */

#define L7_SOL_SOCKET       0xffff  

#define L7_SO_BROADCAST     0x0020  
#define L7_SO_REUSEADDR     0x0100  
#define L7_SO_REUSEPORT     0x0200  
#define L7_SO_DONTROUTE     0x0500 

#define L7_SO_SNDBUF        0x1001  
#define L7_SO_RCVBUF        0x1002 
#define L7_SO_RCVTIMEO      0x1006
#define L7_SO_ERROR         0x1007
#define L7_SO_TYPE          0x1008
#define L7_SO_KEEPALIVE     0x1009 

#define L7_SO_BINDTODEVICE  0x100B  /* char[IP_IFNAMSIZ]; bind to specified device */


/***** L7_IPPROTO_IP level socket options: *****/
#define L7_IP_HDRINCL           2   
#define L7_IP_TOS               3  
#define L7_IP_TTL               4  
#define L7_IP_PKTINFO           8  
#define L7_IP_MULTICAST_IF      9  
#define L7_IP_MULTICAST_TTL     10 
#define L7_IP_MULTICAST_LOOP    11
#define L7_IP_ADD_MEMBERSHIP    12  
#define L7_IP_DROP_MEMBERSHIP   13 
#define L7_IP_RECVTTL           14
#define L7_IP_RECVIF            20 
#define L7_IP_ROUTER_ALERT      21 
#define L7_IP_DONTFRAG          22 
#define L7_UDP_X_CHKSUM         40 
#define L7_IP_OPTIONS           41

/***** L7_IPPROTO_TCP level socket options: *****/
#define L7_TCP_NODELAY      0x0001  
#define L7_TCP_X_MD5_SIGNATURE_KEY  0x0010 



#define L7_IPV6_V6ONLY            1   
#define L7_IPV6_UNICAST_HOPS      4   
#define L7_IPV6_MULTICAST_IF      9  
#define L7_IPV6_MULTICAST_HOPS   10  
#define L7_IPV6_MULTICAST_LOOP   11 
#define	L7_IPV6_JOIN_GROUP       12
#define L7_IPV6_ADD_MEMBERSHIP   L7_IPV6_JOIN_GROUP
#define	L7_IPV6_LEAVE_GROUP      13 
#define L7_IPV6_DROP_MEMBERSHIP  L7_IPV6_LEAVE_GROUP

#define L7_IPV6_PKTINFO          20   
#define L7_IPV6_TCLASS           22  
#define L7_IPV6_NEXTHOP          23  
#define L7_IPV6_RTHDR            24  
#define L7_IPV6_HOPOPTS          25 
#define L7_IPV6_DSTOPTS          26 
#define L7_IPV6_RTHDRDSTOPTS     27

#define L7_IPV6_RECVPKTINFO      30
#define L7_IPV6_RECVHOPLIMIT     31
#define L7_IPV6_RECVTCLASS       32
#define L7_IPV6_RECVRTHDR        34
#define L7_IPV6_RECVHOPOPTS      35
#define L7_IPV6_RECVDSTOPTS      36
#define L7_IPV6_ROUTER_ALERT     37

#define L7_IPV6_CHECKSUM         46
#define L7_IPV6_FW               51
#define L7_IPV6_RECVIF          100

#define L7_IPV6_HOPLIMIT         21   

/***** L7_IPPROTO_ICMP6 level socket options: *****/

#define L7_ICMP6_FILTER           1   


/*
 * Temporarily add this #undef to avoid a conflict with
 * a build environment that #defines imr_interface.
 */
#undef imr_interface
typedef struct L7_ip_mreq_s
{
    struct  L7_in_addr_s   imr_multiaddr;  
    struct  L7_in_addr_s   imr_interface;  
}L7_ip_mreq_t;

typedef struct L7_ip_mreq6_s
{
    struct  L7_in6_addr_s   imr6_multiaddr;  
    L7_uint32               imr6_intIfNum;  
    L7_uchar8               imr6_ifName[L7_INTF_NAME_MAX_LEN];
}L7_ip_mreq6_t;


/* shutdown values */
#define L7_SHUT_RD       0
#define L7_SHUT_WR       1
#define L7_SHUT_RDWR     2


/* IOCTL: note that socket ioctls are all handles through osapi 
   using calls specific to that ioctl. for that reason, ioctl codes
   are not defined here
*/

/* flags for interface type */
#define L7_IFF_BROADCAST           1
#define L7_IFF_LOOPBACK            2
#define L7_IFF_POINTOPOINT         4
#define L7_IFF_MULTICAST           8




/* for L7_ICMP6_FILTER sockopt */
typedef struct L7_icmp6_filter_s
{
    L7_uint32  icmp6_filt[8];  
}L7_icmp6_filter_t;

#define L7_ICMP6_FILTER_SETPASS(type, filterp) \
    ((((filterp)->icmp6_filt[(type) >> 5]) |= \
        (1 << ((type) & 31))))


#define L7_ICMP6_FILTER_SETBLOCK(type, filterp) \
    ((((filterp)->icmp6_filt[(type) >> 5]) &= \
        ~(1 << ((type) & 31))))


#define L7_ICMP6_FILTER_SETPASSALL(filterp) \
    memset((filterp), 0xFF, sizeof( L7_icmp6_filter_t))

#define L7_ICMP6_FILTER_SETBLOCKALL(filterp) \
    memset((filterp), 0, sizeof(L7_icmp6_filter_t))



#endif /* _OSAPI_SOCKDEFS_H */

