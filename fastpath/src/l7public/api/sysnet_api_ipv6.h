/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name:       sysnet_api_ipv6.h
*
* Purpose:    Defines, enums and data structures for sysnet IPv6
*
* Component:  sysnet
*
* Comments:   none
*
* Date:       05/22/2006
*
* Created by: kkiran
*
*********************************************************************/

#ifndef SYSNET_API_IPV6_H
#define SYSNET_API_IPV6_H

/* Sysnet intercept hooks for address family AF_INET6 */
typedef enum
{
  SYSNET_INET6_RECV_IN = 0,   /* All IPv6 packets passing thru sysnet, before VLAN routing intIfNum conversion */
  SYSNET_INET6_IN,            /* All IPv6 packets received, prior to any validity checks */
  SYSNET_INET6_VALID_IN,      /* IPv6 packets received, post any validity checks */
  SYSNET_INET6_FORWARD_IN,    /* IPv6 packets received to be forwarded by the application */
  SYSNET_INET6_MCAST_IN,      /* All IPv6 Multicast packets received */
  SYSNET_INET6_MFORWARD_IN,   /* IPv6 Multicast packets received by the multicast forwarding engine */
  SYSNET_INET6_LOCAL_IN,      /* IPv6 packets being sent to the local IPv6 stack */
  SYSNET_INET6_RECV_OUT,      /* IPv6 packets to be transmitted that were caused by incoming frames */
  SYSNET_INET6_MFORWARD_OUT,  /* IPv6 Multicast packets to be transmitted by multicast forwarding engine */
  SYSNET_INET6_PRECAP_OUT,    /* IPv6 packets to be transmitted, prior to L2 encapsulation */
  SYSNET_INET6_POSTCAP_OUT,   /* IPv6 packets to be transmitted, post L2 encapsulation */
} SYSNET_INET6_HOOKS_t;

#endif /* SYSNET_API_IPV6_H */
