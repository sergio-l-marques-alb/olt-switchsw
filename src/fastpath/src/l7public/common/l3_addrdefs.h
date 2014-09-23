
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   l3_addrdefs.h
*
* @purpose    layer 3 address defs
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


#ifndef _L3_ADDRDEFS_H
#define _L3_ADDRDEFS_H

#include "l7_common.h"
#include "l3_addrdefs.h"

/*NOTE WELL: these values are NOT those of underlying stack.
  Do not confuse L7_AF_INET and AF_INET,eg. Values differ
  across platforms and we intentionally dont want to include
  a way to resolve them here. (would hose many things).
*/

#define INET_IPV4_ALL_MCAST_GROUPS_ADDR   (L7_uint32)0xe0000000
                                                        /* 224.0.0.0 */
#define INET_IPV6_ALL_MCAST_GROUPS_ADDR   "ff00::"

#define L7_AF_INET           1
#define L7_AF_INET6          2


typedef struct L7_in_addr_s
{
    L7_uint32   s_addr;    /* 32 bit IPv4 address in network byte order */
}L7_in_addr_t;

/***************************************
 *
 * 128-bit IP6 address.
 ***************************************/
typedef struct L7_in6_addr_s
{
    union
    {
        L7_uchar8     addr8[16];
        L7_ushort16   addr16[8];
        L7_uint32     addr32[4];
    } in6;
}L7_in6_addr_t;

#define L7_IP6_LINK_LOCAL_MASKLEN 128

#define L7_MAX_POSSIBLE_IPV6_PREFIXES 128

/* buf len for printing ipv6 address */
#define IPV6_DISP_ADDR_LEN   46

#define IPV6_ADDRESS_LEN     sizeof(L7_in6_addr_t)

typedef struct L7_in6_prefix_s
{
  L7_in6_addr_t in6Addr;
  L7_int32      in6PrefixLen;
} L7_in6_prefix_t;




/*****************************************************************************
 *                    sockaddr
 ****************************************************************************
 */
typedef struct L7_sockaddr_s
{
    L7_uchar8             sa_len;
    L7_uchar8             sa_family;
    L7_uchar8             sa_data[14];
}L7_sockaddr_t;


/*
 ****************************************************************************
 *                    sockaddr_in
 ****************************************************************************
 */
typedef struct L7_sockaddr_in_s
{
    L7_uchar8             sin_len;
    L7_uchar8             sin_family;
    L7_ushort16           sin_port;

    L7_in_addr_t          sin_addr;

    char                  sin_zero[8];  /* unused */
}L7_sockaddr_in_t;




/*
 ****************************************************************************
 *                    sockaddr_in6
 ****************************************************************************
 */
typedef struct L7_sockaddr_in6_s
{
    L7_uchar8	            sin6_len;
    L7_uchar8               sin6_family;
    L7_ushort16             sin6_port;
    L7_uint32	            sin6_flowinfo;

    L7_in6_addr_t           sin6_addr;

    L7_uint32	            sin6_scope_id;
}L7_sockaddr_in6_t;


/*
 * union needed to hold sockaddr of unknown type since struct sockaddr is
 * not big enough to hold in6
 */
typedef struct L7_sockaddr_union_s{
    union{
         L7_sockaddr_t      sa;
         L7_sockaddr_in_t   sa4;
         L7_sockaddr_in6_t  sa6;
    }u;
}L7_sockaddr_union_t;

/*
 ****************************************************************************
 *                    Generic Ip Address Structure
 ****************************************************************************
 */

typedef struct L7_inet_addr_s
{
  L7_uchar8     family;  /* L7_AF_INET, L7_AF_INET6, ... */
  union
  {
    struct L7_in_addr_s    ipv4;
    struct L7_in6_addr_s   ipv6;
  } addr;
} L7_inet_addr_t;

#define L7_INET_GET_FAMILY(xaddr) \
  ((L7_inet_addr_t *)(xaddr))->family

/* some macros for inet_address */
#define	L7_IP4_IN_MULTICAST(a)		((((a)) & 0xf0000000) == 0xe0000000)
#define L7_INET_IS_ADDR_EQUAL(xaddr, yaddr) \
  ( (L7_AF_INET6 == ((L7_inet_addr_t *)(xaddr))->family) ? \
    ( (((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr32[0] == \
       ((L7_inet_addr_t *)(yaddr))->addr.ipv6.in6.addr32[0]) && \
      (((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr32[1] == \
       ((L7_inet_addr_t *)(yaddr))->addr.ipv6.in6.addr32[1]) && \
      (((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr32[2] == \
       ((L7_inet_addr_t *)(yaddr))->addr.ipv6.in6.addr32[2]) && \
      (((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr32[3] == \
       ((L7_inet_addr_t *)(yaddr))->addr.ipv6.in6.addr32[3]) ) : \
    ( ((L7_inet_addr_t *)(xaddr))->addr.ipv4.s_addr == \
      ((L7_inet_addr_t *)(yaddr))->addr.ipv4.s_addr ) )

#define L7_INET_ADDR_COMPARE(xaddr, yaddr) \
    ( (L7_AF_INET6 == ((L7_inet_addr_t *)(xaddr))->family) ? \
      (memcmp(&(((L7_inet_addr_t *)(xaddr))->addr.ipv6), \
              &(((L7_inet_addr_t *)(yaddr))->addr.ipv6), sizeof(L7_in6_addr_t))) : \
      (memcmp(&(((L7_inet_addr_t *)(xaddr))->addr.ipv4), \
              &(((L7_inet_addr_t *)(yaddr))->addr.ipv4), sizeof(L7_in_addr_t))) )

#define L7_INET_IS_ADDR_BROADCAST(xaddr) \
  ( (L7_AF_INET6 == ((L7_inet_addr_t *)(xaddr))->family) ? \
    ( (((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr32[0] == \
       0xff) && \
      (((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr32[1] == \
       0xff) && \
      (((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr32[2] == \
       0xff) && \
      (((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr32[3] == \
       0xff) ) : \
    ( ((L7_inet_addr_t *)(xaddr))->addr.ipv4.s_addr == \
      0xffffffff ) )

#define L7_INET_IS_ADDR_EXPERIMENTAL(xaddr) \
  ( (L7_AF_INET6 == ((L7_inet_addr_t *)(xaddr))->family) ? 0\
    : ( ((L7_inet_addr_t *)(xaddr))->addr.ipv4.s_addr & 0xe0000000) == 0xe0000000)

/* some macros for inet_address */


#define L7_IP6_IS_ADDR_UNSPECIFIED(xaddr) \
  ( (((L7_in6_addr_t *)(xaddr))->in6.addr32[0] == 0) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[1] == 0) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[2] == 0) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[3] == 0) )

#define L7_IP6_IS_ADDR_LOOPBACK(xaddr) \
  ( (((L7_in6_addr_t *)(xaddr))->in6.addr32[0] == 0) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[1] == 0) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[2] == 0) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[3] == osapiNtohl(1)) )

#define L7_IP6_IS_ADDR_EQUAL(xaddr,yaddr) \
  ( (((L7_in6_addr_t *)(xaddr))->in6.addr32[0] == ((L7_in6_addr_t *)(yaddr))->in6.addr32[0]) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[1] == ((L7_in6_addr_t *)(yaddr))->in6.addr32[1]) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[2] == ((L7_in6_addr_t *)(yaddr))->in6.addr32[2]) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[3] == ((L7_in6_addr_t *)(yaddr))->in6.addr32[3]) )

#define L7_IP6_IS_ADDR_LINK_LOCAL(xaddr) \
  ((osapiNtohs( ((L7_in6_addr_t *)(xaddr))->in6.addr16[0] ) & 0xffc0) == 0xfe80)

#define L7_IP6_IS_ADDR_MULTICAST(xaddr) \
  ( ((L7_in6_addr_t *)(xaddr))->in6.addr8[0] ==0xff )

#define L7_IP6_IS_ADDR_MULTICAST_LOC_SCOPE(xaddr) \
  (osapiNtohs( ((L7_in6_addr_t *)(xaddr))->in6.addr16[0] ) == 0xff02)

#define L7_IP6_IS_ADDR_6TO4(xaddr) \
  (osapiNtohs( ((L7_in6_addr_t *)(xaddr))->in6.addr16[0] ) == 0x2002)

#define L7_IP6_IS_ADDR_V4COMPAT(xaddr) \
  ( (((L7_in6_addr_t *)(xaddr))->in6.addr32[0] == 0) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[1] == 0) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[2] == 0) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[3] != osapiNtohl(0)) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[3] != osapiNtohl(1)) )

#define L7_IP6_IS_ADDR_V4MAPPED(xaddr) \
  ( (((L7_in6_addr_t *)(xaddr))->in6.addr32[0] == 0) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[1] == 0) && \
    (((L7_in6_addr_t *)(xaddr))->in6.addr32[2] == osapiNtohl(0x0000ffff)) )

#define L7_IP6_ADDR_V4MAPPED(xaddr) (((L7_in6_addr_t *)(xaddr))->in6.addr32[3])

#define L7_IP6_ADDR_LEN 16
#define L7_IP6_ADDR_COMPARE(xaddr1,xaddr2)  memcmp((xaddr1),(xaddr2),L7_IP6_ADDR_LEN)

/* a handy macro for snmp walks */
#define L7_IP6_ADDR_INCREMENT(xaddr, xcarry)  {\
   int xx; \
   xcarry = L7_FALSE; \
   for(xx = 15; xx >= 0 ;xx--) \
   { \
      (xaddr)->in6.addr8[xx]++; \
      if((xaddr)->in6.addr8[xx] != 0) \
      { \
         break; \
      } \
   } \
   if(xx < 0) xcarry = L7_TRUE; \
}

#define L7_INET_ADDR_GET_ALL_ONES_MASKLEN(addr)  ((addr)->family == L7_AF_INET)?32:128

/*
 Following things are added for mcast support
 */

#define L7_INET_APPLY_MASK(xaddr, yaddr) \
  ( (L7_AF_INET6 == ((L7_inet_addr_t *)(xaddr))->family) ? \
    ( (((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr32[0] & \
       ((L7_inet_addr_t *)(yaddr))->addr.ipv6.in6.addr32[0]) && \
      (((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr32[1] & \
       ((L7_inet_addr_t *)(yaddr))->addr.ipv6.in6.addr32[1]) && \
      (((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr32[2] & \
       ((L7_inet_addr_t *)(yaddr))->addr.ipv6.in6.addr32[2]) && \
      (((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr32[3] & \
       ((L7_inet_addr_t *)(yaddr))->addr.ipv6.in6.addr32[3]) ) : \
    ( ((L7_inet_addr_t *)(xaddr))->addr.ipv4.s_addr & \
      ((L7_inet_addr_t *)(yaddr))->addr.ipv4.s_addr ) )

#define L7_INET_IN_SSM_RANGE(xaddr) \
	((L7_AF_INET6 == ((L7_inet_addr_t *)(xaddr))->family) ? \
	 (((osapiNtohs( ((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr16[0] ) & 0xfff0) == 0xff30)&&  \
      ((osapiNtohs( ((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr16[1] ) & 0xffff) == 0x0000)&&  \
      ((osapiNtohs( ((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr16[2] ) & 0xffff) == 0x0000)&&  \
      ((osapiNtohs( ((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr16[3] ) & 0xffff) == 0x0000)&&  \
      ((osapiNtohs( ((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr16[4] ) & 0xffff) == 0x0000)&&  \
      ((osapiNtohs( ((L7_inet_addr_t *)(xaddr))->addr.ipv6.in6.addr16[5] ) & 0xffff) == 0x0000)) :\
	 (((((L7_inet_addr_t *)(xaddr))->addr.ipv4.s_addr) & 0xFF000000) == \
	 L7_IP_SSM_BASE_ADDR))


#define L7_INET_GET_MAX_MASK_LEN(family) (family == L7_AF_INET)?32:128

#define L7_IP6_GET_VER(ip6)      (osapiNtohl((ip6)->ver_class_flow) >> 28)
#define L7_IP6_SET_VER(ip6,val)  (ip6)->ver_class_flow = osapiHtonl((val & 0x0f) << 28)
#define L7_IP6_GET_CLASS(ip6)   ((L7_uchar8) ((osapiNtohl((ip6)->ver_class_flow) >> 20) & 0xff))
#endif /* _L3_ADDRDEFS_H */

