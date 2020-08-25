/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name:       sysnet_api_ipv4.h
*
* Purpose:    Defines, enums and data structures for sysnet IPv4
*
* Component:  sysnet
*
* Comments:   none
*
* Date:       11/01/2002
*
* Created by: mfiorito
*
*********************************************************************/

#ifndef SYSNET_API_IPV4_H
#define SYSNET_API_IPV4_H

/* Sysnet intercept hooks for address family AF_INET */
typedef enum
{
  SYSNET_INET_RECV_IN = 0,   /* All IP packets passing thru sysnet, before VLAN routing intIfNum conversion */
  SYSNET_INET_RECV_ARP_IN,   /* ARP packets on DTL receive thread */
  SYSNET_INET_IN,            /* All IP packets received, prior to any validity checks */
  SYSNET_INET_VALID_IN,      /* IP packets received, post any validity checks */
  SYSNET_INET_FORWARD_IN,    /* IP packets received to be forwarded by the application */
  SYSNET_INET_MCAST_IN,      /* All IP Multicast packets received */
  SYSNET_INET_MFORWARD_IN,   /* IP multicast packets received by the multicast forwarding engine */
  SYSNET_INET_LOCAL_IN,      /* IP packets being sent to the local IP stack */
  SYSNET_INET_RECV_OUT,      /* IP packets to be transmitted that were caused by incoming frames */
  SYSNET_INET_MFORWARD_OUT,  /* IP multicast packets to be transmitted by multicast forwarding engine */
  SYSNET_INET_PRECAP_OUT,    /* IP packets to be transmitted, prior to L2 encapsulation */
  SYSNET_INET_POSTCAP_OUT,   /* IP packets to be transmitted, post L2 encapsulation */
  SYSNET_INET_ARP_IN         /* ARP frames received */
} SYSNET_INET_HOOKS_t;

#endif /* SYSNET_API_IPV4_H */
