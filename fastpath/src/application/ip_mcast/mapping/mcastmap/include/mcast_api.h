/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  mcast_api.h
*
* @purpose   Common defines, enums and data structures for multicast modules
*
* @component
*
* @comments  none
*
* @create    11/21/2006
*
* @author    Ramu
*
* @end
*
**********************************************************************/

#ifndef MCAST_API_H
#define MCAST_API_H

#include <l7_common.h>
#include <l3_addrdefs.h>
#include <async_event_api.h>
#include <l3_default_cnfgr.h>

#define MAX_INTERFACES (L7_RTR_MAX_RTR_INTERFACES + 1)

typedef struct mcastAdminMsgInfo_s
{
        L7_uint32           intIfNum;         /* Interface on which the boundary is applied */
        L7_inet_addr_t      groupAddress;     /* Group address */
        L7_inet_addr_t      groupMask;        /* Group Mask */
        L7_uint32           mode;             /* Admin scope active or not */
        L7_uchar8          family;
}mcastAdminMsgInfo_t;

/* This structure is filled by the mcast map and is passed on to the MRP/IGMP
   module for further packet processing */
typedef struct mcastControlPkt_s
{
  L7_uchar8       family;
  L7_uint32       rtrIfNum;         /* interface that received the packet  */
  L7_inet_addr_t  srcAddr;
  L7_inet_addr_t  destAddr;
  L7_uchar8       *payLoad;
  L7_uint32       length;           /* payload length */
  L7_char8        ipTypeOfService;  /* DSCP bit in IP header */
  L7_char8        ipRtrAlert;  /* Rtr alert option in IP header */  
  L7_uint32       numHops;     /* TTL in IPv4 and HopLimit field in IPv6 */
} mcastControlPkt_t;

/* This structure is filled by the MRP b4 calling the send function to
   send the packet out */
typedef struct mcastSendPkt_s
{
  L7_inet_addr_t  srcAddr;
  L7_inet_addr_t  destAddr;
  L7_uint32       protoType; /* protocol number IPPROTO_XXX*/
  L7_uint32       ttl;
  L7_uint32       rtrIfNum;  /* outgoing interface */
  L7_uchar8       *payLoad;  /* payload, pim/igmp/dvmrp header start */
  L7_uint32       length;    /* data length */
  L7_uint32       flags;
  L7_uint32       sockFd;
  L7_ushort16     port;      /* port numbr */
  L7_uchar8       family;
  L7_BOOL         rtrAlert;  /* Send with router-alert header option? */
} mcastSendPkt_t;
/********************************************************
* @purpose   This function is called to send the multicast control packet.
*
* @param     pSendPkt      @b{(input)}  structure containing the buffer
                                         to sendpacket out
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     none
*
* @end
*********************************************************/

L7_RC_t mcastMapPacketSend(mcastSendPkt_t *pSendPkt);

/*********************************************************************
* @purpose  Signals the mapping layer to handle the end of the Warm
*           restart processing.  This should only be invoked on the 
*           MCAST mapping thread
*
* @param    none
*
* @comments none
*
* @end
*********************************************************************/
extern void mcastMapWarmRestartPeriodComplete(void);

#endif /*MCAST_API_H*/
