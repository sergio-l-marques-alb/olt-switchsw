/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  ip_1213_api.c
*
* @purpose   Ip Api functions for rfc 1213
*
* @component Ip Mapping Layer
*
* @comments  Routing package version
*
* @create    06/26/2001
*                       
* @author    anayar
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include "l7_common.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "l3_comm_structs.h"
#include "l3_defaultconfig.h"
#include "rto_api.h"
#include "usmdb_ip_api.h"
#include "default_cnfgr.h"
#include "l7_ip_api.h"
#include "rtip_var.h"
#include "rtip_icmp.h"
#include "rticmp_var.h"
#include "platform_counters.h"
#include "statsapi.h"
#include "dtlapi.h"
#include "sysapi.h"
#include "ip_1213_api.h"

/*********************************************************************
* @purpose Gets the IP Router Admin Mode
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(output)} L7_ENABLE, L7_DISABLE
*
* @returns L7_SUCCESS
* @returns L7_FAIlURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ipmIpRtrAdminModeGet(L7_uint32 *val)
{
  *val = ipMapRtrAdminModeGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Ip Forwarding mode
*
* @param    mode  @b{(input)} enables or disables Ip Forwarding
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ipmIpRtrAdminModeSet(L7_uint32 mode)
{
  L7_RC_t rc;
  rc = ipMapRtrAdminModeSet(mode);
  return rc;
}

/*********************************************************************
* @purpose  Sets Ip default Time To Live
*
* @param    *val  @b{(output)} Default TTL value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*       
* @comments
*
* @end
*********************************************************************/
L7_RC_t ipmIpDefaultTTLGet(L7_uint32 *val)
{
  *val = ipMapIpDefaultTTLGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets Ip default Time To Live
*
* @param    ttl  @b{(input)} Default ttl value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*       
* @comments
*
* @end
*********************************************************************/
L7_RC_t ipmIpDefaultTTLSet(L7_uint32 ttl)
{
  return ipMapIpDefaultTTLSet(ttl);
}

/*********************************************************************
* @purpose  Get the total number of input datagrams received from interfaces
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments (ip packets handled by only the NP + ip packets handled
*           by the Operating System IP stack)   = 
*         
*           ( DAPI_STATS_RECEIVED_L3_IN +
*            vxworks ipInReceives -
*            DAPI_STATS_RECEIVED_L3_IP_TO_CPU -
*            DAPI_STATS_RECEIVED_L3_MAC_MCAST_DISCARDS )
*                                                        
* @end
*********************************************************************/
L7_RC_t ipmIpInReceivesGet(L7_uint32 *val)
{
  L7_ulong64 ctr;

  *val = 0;
  if (osapiIpInReceivesGet(val) == L7_SUCCESS)
  {
    *val += L7_ipstat.ips_total;

    /* Get the driver stat for number of ip packets in */
    if (dtlStatsGet(FD_CNFGR_NIM_MIN_CPU_INTF_NUM, L7_PLATFORM_CTR_RX_IP_IN_RECEIVES, C32_BITS, &ctr) == L7_SUCCESS)
    {
      *val += ctr.low;
      ctr.low = ctr.high = 0;

      if (dtlStatsGet(FD_CNFGR_NIM_MIN_CPU_INTF_NUM, L7_PLATFORM_CTR_RECEIVED_L3_IP_TO_CPU, C32_BITS, &ctr) == L7_SUCCESS)
      {
        *val -= ctr.low;
        ctr.low = ctr.high = 0;

        if (dtlStatsGet(FD_CNFGR_NIM_MIN_CPU_INTF_NUM, L7_PLATFORM_CTR_RECEIVED_L3_MAC_MCAST_DISCARDS, C32_BITS, &ctr) == L7_SUCCESS)
        {
          *val -= ctr.low;
          return L7_SUCCESS;
        }
      }
    }
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the total number of input datagrams discarded due to headers
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments (packets discarded by the NP + packets discarded by the 
*           Operating System IP stack + packets discarded by the
*           IP Mapping layer forwarder )  
*         
*        
* @end
*********************************************************************/
L7_RC_t ipmIpInHdrErrorsGet(L7_uint32 *val)
{
  L7_ulong64 ctr;

  *val = 0;
  if (osapiIpInHdrErrorsGet(val) == L7_SUCCESS)
  {
    *val += (L7_ipstat.ips_badsum + L7_ipstat.ips_tooshort + L7_ipstat.ips_toosmall +
             L7_ipstat.ips_badlen + L7_ipstat.ips_badoptions + L7_ipstat.ips_badvers +
             L7_ipstat.ips_badsa);

    if (dtlStatsGet(FD_CNFGR_NIM_MIN_CPU_INTF_NUM, L7_PLATFORM_CTR_RX_IP_IN_HDR_ERRORS, C32_BITS, &ctr) == L7_SUCCESS)
    {
      *val += ctr.low;
      return L7_SUCCESS;
    }
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of input datagrams discarded because
*           the IP address in their IP header's destination
*           field was not a valid address to be received at
*           this entity.
*          
* @param    *val    @b{(output)} datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments (packets discarded by the NP + packets discarded by the 
*           Operating System IP stack + packets discarded by the
*           IP Mapping layer forwarder )  
*        
* @end
*********************************************************************/
L7_RC_t ipmIpInAddrErrorsGet(L7_uint32 *val)
{
  L7_ulong64 ctr;

  *val = 0;
  if (osapiIpInAddrErrorsGet(val) == L7_SUCCESS)
  {
    *val += L7_ipstat.ips_cantforward;

    if (dtlStatsGet(FD_CNFGR_NIM_MIN_CPU_INTF_NUM, L7_PLATFORM_CTR_RX_IP_IN_ADDR_ERRORS, C32_BITS, &ctr) == L7_SUCCESS)
    {
      *val += ctr.low;
      return L7_SUCCESS;
    }
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of input datagrams for which this
*           entity was not their final IP destination, as a
*           result of which an attempt was made to find a
*           route to forward them to that final destination.
*          
* @param    *val   @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments (packets forwarded by the NP + packets forwarded by the 
*           Operating System IP stack + packets forwarded by the
*           IP Mapping layer forwarder )  
*        
* @end
*********************************************************************/
L7_RC_t ipmIpForwDatagramsGet(L7_uint32 *val)
{
  L7_ulong64 ctr;

  *val = 0;
  if (osapiIpForwDatagramsGet(val) == L7_SUCCESS)
  {
    *val += L7_ipstat.ips_forward;

    if (dtlStatsGet(FD_CNFGR_NIM_MIN_CPU_INTF_NUM, L7_PLATFORM_CTR_RX_IP_FORW_DATAGRAMS, C32_BITS, &ctr) == L7_SUCCESS)
    {
      *val += ctr.low;
      return L7_SUCCESS;
    }
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of locally-addressed datagrams
*           received successfully but discarded because of an
*           unknown or unsupported protocol.
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ( packets discarded by the Operating System IP stack 
*           + packets discarded by the IP Mapping layer forwarder )  
*        
* @end
*********************************************************************/
L7_RC_t ipmIpInUnknownProtosGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIpInUnknownProtosGet(val) == L7_SUCCESS)
  {
    *val += L7_ipstat.ips_noproto;
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of input IP datagrams for which no
*           problems were encountered to prevent their
*           continued processing, but which were discarded
*
*
* @param    *val   @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments (packets discarded by the NP + packets discarded by the 
*           Operating System IP stack + packets discarded by the
*           IP Mapping layer forwarder + packets discarded prior to
*           receipt in the forwarder layer)  
*        
* @end
*********************************************************************/
L7_RC_t ipmIpInDiscardsGet(L7_uint32 *val)
{
  L7_ulong64 ctr;
  L7_uint32  cpuStat;

  *val = 0;
  if (osapiIpInDiscardsGet(val) == L7_SUCCESS)
  {
    if (dtlStatsGet(FD_CNFGR_NIM_MIN_CPU_INTF_NUM, L7_PLATFORM_CTR_RECEIVED_L3_MAC_MCAST_DISCARDS, C32_BITS, &ctr) == L7_SUCCESS)
    {
      *val += ctr.low;

      SYSAPI_IP_STATS_GET(&cpuStat,FD_CNFGR_NIM_MIN_CPU_INTF_NUM, L7_PLATFORM_CTR_RX_IP_IN_DISCARDS);

      *val += cpuStat;

      return L7_SUCCESS;
    }
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the total number of input datagrams successfully
*           delivered to IP user-protocols (including ICMP).
*          
* @param    *val   @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*          
* @comments (packets delivered to the Operating System IP stack's user protocols
*           + packets delivered to the IP Mapping layer forwarder's
*           user protocols (but not delivered to the IP stack))
*        
* @end
*********************************************************************/
L7_RC_t ipmIpInDeliversGet(L7_uint32 *val)
{
  L7_uint32  cpuStat;

  *val = 0;
  if (osapiIpInDeliversGet(val) == L7_SUCCESS)
  {
    SYSAPI_IP_STATS_GET(&cpuStat,FD_CNFGR_NIM_MIN_CPU_INTF_NUM, L7_PLATFORM_CTR_RX_IP_IN_DELIVERS);
    *val += cpuStat;

    *val += L7_ipstat.ips_delivered;

    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the total number of IP datagrams which local IP
*           user-protocols (including ICMP) supplied to IP in
*           requests for transmission.  
*          
* @param    *val   @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Note that this counter
*           does not include any datagrams counted in
*           ipForwDatagrams
*
* @comments (packets submitted by the Operating System IP stack
*           + packets submitted by the IP Mapping layer forwarder's
*           user protocols (but which bypass the OS IP stack))
*        
* @end
*********************************************************************/
L7_RC_t ipmIpOutRequestsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIpOutRequestsGet(val) == L7_SUCCESS)
  {
    *val += L7_ipstat.ips_localout;
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Gets the number of output IP datagrams for which no
*           problem was encountered to prevent their
*           transmission to their destination, but which were
*           discarded (e.g., for lack of buffer space).  
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpOutDiscardsGet(L7_uint32 *val)
{
  L7_uint32 cpuStat;

  *val = 0;
  if (osapiIpOutDiscardsGet(val) == L7_SUCCESS)
  {
    *val += L7_ipstat.ips_odropped;

    SYSAPI_IP_STATS_GET(&cpuStat,FD_CNFGR_NIM_MIN_CPU_INTF_NUM, L7_PLATFORM_CTR_TX_IP_OUT_DISCARDS);
    *val += cpuStat;
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of IP datagrams discarded because no
*           route could be found to transmit them to their
*           destination.  
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments (packets discarded by the NP + packets discarded by the 
*           Operating System IP stack + packets discarded by the
*           IP Mapping layer forwarder + packets discarded prior to
*           receipt in the forwarder layer)  
*        
* @end
*********************************************************************/
L7_RC_t ipmIpOutNoRoutesGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIpOutNoRoutesGet(val) == L7_SUCCESS)
  {
    *val += L7_ipstat.ips_noroute;
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the maximum number of seconds which received
*           fragments are held while they are awaiting
*           reassembly at this entity
*
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpReasmTimeoutGet(L7_uint32 *val)
{
  *val = FD_IP_FRAG_TTL_DFLT;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of IP fragments received which needed
*           to be reassembled at this entity
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpReasmReqdsGet(L7_uint32 *val)
{
  *val = L7_ipstat.ips_fragments;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of IP datagrams successfully re-assembled
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpReasmOKsGet(L7_uint32 *val)
{
  *val = L7_ipstat.ips_reassembled;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of failures detected by the IP re-
*           assembly algorithm (for whatever reason: timed
*           out, errors, etc).  
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpReasmFailsGet(L7_uint32 *val)
{
  *val = (L7_ipstat.ips_fragdropped + L7_ipstat.ips_fragtimeout);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of IP datagrams that have been
*           successfully fragmented at this entity
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpFragOKsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIpFragOKsGet(val) == L7_SUCCESS)
  {
    *val += L7_ipstat.ips_fragmented;
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of IP datagrams that have been
*           discarded because they needed to be fragmented at
*           this entity but could not be, e.g., because their
*           Don't Fragment flag was set.
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpFragFailsGet(L7_uint32 *val)
{
  L7_ulong64 ctr;

  *val = L7_ipstat.ips_cantfrag;

  if (dtlStatsGet(FD_CNFGR_NIM_MIN_CPU_INTF_NUM, L7_PLATFORM_CTR_RX_IP_FRAG_FAILS, C32_BITS, &ctr) == L7_SUCCESS)
  {
    *val += ctr.low;
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of IP datagram fragments that have
*           been generated as a result of fragmentation at
*           this entity.
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpFragCreatesGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIpFragCreatesGet(val) == L7_SUCCESS)
  {
    *val += L7_ipstat.ips_ofragments;
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of routing entries which were chosen
*           to be discarded even though they are valid.  
*
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRoutingDiscardsGet(L7_uint32 *val)
{
  L7_ulong64 ctr;

  *val = 0;
  if (dtlStatsGet(FD_CNFGR_NIM_MIN_CPU_INTF_NUM, L7_PLATFORM_CTR_IP_ROUTING_DISCARDS, C32_BITS, &ctr) == L7_SUCCESS)
  {
    *val += ctr.low;
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the number of routing entries which were chosen
*           to be discarded even though they are valid.  
*
* @param    val    @b{(input)} Address
*
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRoutingDiscardsSet(L7_uint32 val)
{
  return L7_NOT_SUPPORTED;
}


/*:ignore*/
/*
/------------------------------------------------------------------\
*                         IP ADDRESSING INFORMATION                *
\------------------------------------------------------------------/
*/
/*:end ignore*/

/*********************************************************************
* @purpose  Determines if the Ip Address entry corresponding to 
*           this ip address exists
*          
* @param    ipAddr     @b{(input)} The ip address to which this entry's
*                               addressing information pertains.
* @param    intIfNum   @b{(input)} Internal interface number for entry
*
* @returns  L7_SUCCESS  if the specified address entry exists
* @returns  L7_FAILURE  if the specified address entry does not exist
*
* @comments Each entry contains addressing information for one of
*           this entity's IP addresses. 
*
*           Ignores unnumbered interfaces
*        
* @end
*********************************************************************/
L7_RC_t ipmIpAdEntAddrGet(L7_uint32 ipAddr, L7_uint32 intIfNum)
{
  L7_arpEntry_t arpEntry;

  /* If we could get a matching local entry from the arp table
     return success */

  arpEntry.ipAddr = ipAddr;
  arpEntry.intIfNum = 0;

  while ( ipMapIpArpEntryNext(arpEntry.ipAddr, arpEntry.intIfNum, &arpEntry) ==
      L7_SUCCESS)
  {

    if ( arpEntry.ipAddr != ipAddr )
    {
      break;
    }

    /* Skip unnumbered interfaces */
    if (arpEntry.flags & L7_ARP_UNNUMBERED)
    {
      continue;
    }

    if (arpEntry.flags & L7_ARP_LOCAL)
      return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the ip address from the entry's addressing information
*           which is next to the entry to which this ip address pertains
*          
* @param    *ipAddr   @b{(input)} The ip address to which this entry's
*                   addressing information pertains. @line
*                     @b{(output)} The ip address of the next entry
* @param    *intIfNum  @b{(input)} Internal interface number for entry
*                      @b{(output)} Internal interface of next entry
*
* @returns  L7_SUCCESS  if the next route entry exists
* @returns  L7_FAILURE  if the next route entry does not exist
*
* @comments Each entry contains addressing information for one of
*           this entity's IP addresses. 
*
*           Ignores unnumbered interfaces
*        
* @end
*********************************************************************/
L7_RC_t ipmIpAdEntAddrGetNext(L7_uint32* ipAddr, L7_uint32 *intIfNum)
{
  L7_arpEntry_t arpEntry;
  arpEntry.ipAddr = *ipAddr;
  arpEntry.intIfNum = *intIfNum;

  /* Keep looping while there are entries in the arp table */
  while (ipMapIpArpEntryNext(arpEntry.ipAddr, arpEntry.intIfNum,
        &arpEntry) == L7_SUCCESS)
  {
    /* If the next entry is a local arp entry or cpu interface entry
       or service port entry return success.
       Note: The service port interface number is = 0 */
    if ((arpEntry.flags & L7_ARP_LOCAL) || 
        (arpEntry.intIfNum == FD_CNFGR_NIM_MIN_CPU_INTF_NUM) ||
        (arpEntry.intIfNum == 0))
    {
      /* Skip unnumbered interfaces */
      if (arpEntry.flags & L7_ARP_UNNUMBERED)
      {
        continue;
      }
      *ipAddr = arpEntry.ipAddr;
      *intIfNum = arpEntry.intIfNum;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the index value which uniquely identifies the
*           interface to which this entry is applicable
*          
* @param    intIfNum   @b{(input)} The internal interface associated with
*                               this entry
* @param    *ifIndex   @b{(output)} The ifindex of this entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The ifIndex is the index value which uniquely identifies
*           the interface to which this entry is applicable. The interface
*           identified by a particular value of this index is the same
*           interface as identified by the same value of ifIndex.
*        
* @end
*********************************************************************/
L7_RC_t ipmIpEntIfIndexGet(L7_uint32 intIfNum, L7_uint32 *ifIndex)
{
  /* If this happens to be the Service port internal interface num = 0
     we return the ifIndex as 0 since right now we don't have a intIfNum
     or ifIndex associated with the service port */
  if (intIfNum == 0)
  {
    *ifIndex = 0;
    return L7_SUCCESS;
  }

  return usmDbExtIfNumFromIntIfNum(intIfNum, ifIndex);
}

/*********************************************************************
* @purpose  Gets the subnet mask associated with the IP address of
*           this entry.  
*
* @param    intIfNum   @b{(input)} The internal interface associated with
*                               this entry
* @param    *netmask   @b{(output)} The subnet mask associated with the
*                                ip address of this entry. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The value of the mask is an IP
*           address with all the network bits set to 1 and all
*           the hosts bits set to 0."  
*        
* @end
*********************************************************************/
L7_RC_t ipmIpAdEntNetMaskGet (L7_uint32 intIfNum, L7_uint32 *netmask)
{
  L7_uint32 ipAddr;

  return ipMapRtrIntfIpAddressGet(intIfNum, (L7_IP_ADDR_t *)&ipAddr,
                                  (L7_IP_MASK_t *)netmask);
}

/*********************************************************************
* @purpose  Gets the value of the least-significant bit in the IP
*           broadcast address used for sending datagrams on
*           the (logical) interface associated with the IP
*           address of this entry
*
* @param    intIfNum   @b{(input)} The internal interface associated with
*                               this entry
* @param    *bcastAddr @b{(output)} bcast address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The standard specifies that any host id consisting of all
*           1s is reserved for broadcast. 
*           bcast addr = (ipaddr & netmask) | ~netmask 
*        
* @end
*********************************************************************/
L7_RC_t ipmIpAdEntBCastGet (L7_uint32 intIfNum, L7_uint32 *bcastAddr)
{
  L7_uint32 ipAddr;
  L7_uint32 ipMask;

  if (ipMapRtrIntfIpAddressGet(intIfNum, (L7_IP_ADDR_t *)&ipAddr,
                               (L7_IP_MASK_t *)&ipMask) == L7_SUCCESS)
  {
    *bcastAddr = ((ipAddr & ipMask) | (~ipMask)) & 1;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the size of the largest IP datagram which this
*           entity can re-assemble from incoming IP fragmented
*           datagrams received on this interface
*
* @param    ipAddr     @b{(input)} the ip address associated with this
*                               entry.
* @param    *maxSize   @b{(output)} size of largest re-assembled datagram
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE   This return code is not used currently
*
* @comments As the OS performs reassembly this value will be retrieved
*           from the OS.
*           For Linux: The value will be obtained from the file
*           /proc/sys/net/ipv4/ipfrag_high_thresh
*           For VxWorks: The value will be obtained from the mib
*           using the call m2IpAddrTblEntryGet()
*        
* @end
*********************************************************************/
L7_RC_t ipmIpAdEntReasmMaxSizeGet(L7_uint32 ipAddr, L7_uint32 *maxSize)
{
  return osapiIpAdEntReasmMaxSizeGet(ipAddr, maxSize);

}

/*:ignore*/
/*
/------------------------------------------------------------------\
*                         IP ROUTING INFORMATION                *
\------------------------------------------------------------------/
*/
/*:end ignore*/

/* Since the caller only gives us a destination address, we can only
 * walk the best routes. The destination address alone does not provide
 * enough information to find a previous route within a set of routes to
 * the destination. RFC 1213 doesn't really support ECMP. If the routing
 * table contains best routes with more than one next hop, we will only
 * return the first next hop.
 */

/*********************************************************************
* @purpose  Determines if a route entry exists for the specified route
*           destination.
*
* @param    routeDest  @b{(input)} Route destination for this entry.
*
* @returns  L7_SUCCESS If the corresponding entry exists.
* @returns  L7_FAILURE If the corresponding entry does not exist
*
* @comment   
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteEntryGet(L7_uint32 routeDest)
{
  L7_routeEntry_t routeEntry;

  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  if(rtoBestRouteLookup (routeDest, &routeEntry, L7_TRUE) == L7_SUCCESS)
    if(routeDest == routeEntry.ipAddr)
      return L7_SUCCESS;

  return L7_FAILURE;
}   

/*********************************************************************
* @purpose  Determines if the next route entry exists for the specified route
*           destination.
*
* @param    *routeDest  @b{(input)} Route destination for this entry. @line
*                       @b{(output)} Route destination of next entry.
*
* @returns  L7_SUCCESS If the corresponding entry exists
* @returns  L7_FAILURE If the corresponding entry does not exist
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteEntryGetNext(L7_uint32 *routeDest)
{
    /* ROBRICE - I suspect caller needs to give us at least the protocol of
     * the previous route in order for this to work. */
  L7_routeEntry_t routeEntry;
  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));
  routeEntry.ipAddr = *routeDest;

  if (rtoNextRouteGet(&routeEntry, L7_TRUE) == L7_SUCCESS)  /* best routes only */
  {
    *routeDest = routeEntry.ipAddr;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the index value which uniquely identifies the
*           local interface through which the next hop of this
*           route should be reached.  
*
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *intIfNum  @b{(output)} internal interface number for this entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteIfIndexGet(L7_uint32 routeDest, L7_uint32 *intIfNum)
{
  L7_routeEntry_t routeEntry;

  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  /* do exact match lookup */
  if((rtoBestRouteLookup (routeDest, &routeEntry, L7_TRUE) != L7_SUCCESS) ||
     (routeDest != routeEntry.ipAddr))
      return L7_FAILURE;

  /* intIfNum of "first" equal cost path */
  *intIfNum = routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
  if(*intIfNum == 0)
  {
     /* This will be the case of a REJECT ROUTE since reject route
      * doesn't have any next hops.
      *
      * But for the SNMP query to proceed with the other valid routes following
      * this route, we shall give the interface number as CPU interface's
      * internal interface number, as the packets with DA best match as
      * a reject route are directed to CPU and get discarded there.
      */
     if(usmDbMgtSwitchintIfNumGet(0, intIfNum) != L7_SUCCESS)
     {
       /* If usmDbMgtSwitchintIfNumGet() fails, we default to 1.
        * This shouldn't generally happen */
       *intIfNum = 1;
     }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the primary routing metric for this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric1   @b{(output)} route metric 1
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteMetric1Get(L7_uint32 routeDest, L7_uint32 *metric1)
{
  L7_routeEntry_t routeEntry;

  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  /* do exact match lookup */
  if((rtoBestRouteLookup (routeDest, &routeEntry, L7_TRUE) != L7_SUCCESS) ||
     (routeDest != routeEntry.ipAddr))
      return L7_FAILURE;

  *metric1 = routeEntry.metric;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the primary routing metric for this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    metric1    @b{(input)} route metric 1
*
* @returns  L7_NOT_SUPPORTED
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteMetric1Set(L7_uint32 routeDest, L7_uint32 metric1)
{
  return L7_NOT_SUPPORTED;  
}

/*********************************************************************
* @purpose  Gets the alternate routing metric for this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric2   @b{(output)} Route metric 2
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteMetric2Get(L7_uint32 routeDest, L7_uint32 *metric2)
{
    *metric2 = -1;          
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the alternate routing metric for this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    metric2    @b{(input)} Route
*
* @returns  L7_NOT_SUPPORTED
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteMetric2Set(L7_uint32 routeDest, L7_uint32 metric2)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets the alternate routing metric for this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric3   @b{(output)} route metric 3
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteMetric3Get(L7_uint32 routeDest, L7_uint32 *metric3)
{
    *metric3 = -1;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the alternate routing metric for this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    metric3    @b{(input)} Route metric 3
*
* @returns  L7_NOT_SUPPORTED
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteMetric3Set(L7_uint32 routeDest, L7_uint32 metric3)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets the alternate routing metric for this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric4   @b{(output)} route metric 4
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteMetric4Get(L7_uint32 routeDest, L7_uint32 *metric4)
{
    *metric4 = -1;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the alternate routing metric for this route  
*
* @param    intIfNum   @b{(input)} the internal interface number associated
*                               with this entry
* @param    metric4    @b{(input)} route metric 4
*
* @returns  L7_NOT_SUPPORTED
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteMetric4Set(L7_uint32 routeDest, L7_uint32 *metric4)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets the IP address of the next hop of this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    nextHop    @b{(output)} ip address of the next hop of this
*                                route entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteNextHopGet(L7_uint32 routeDest, L7_uint32 *nextHop)
{
  L7_routeEntry_t routeEntry;

  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  /* do exact match lookup */
  if((rtoBestRouteLookup (routeDest, &routeEntry, L7_TRUE) != L7_SUCCESS) ||
     (routeDest != routeEntry.ipAddr))
      return L7_FAILURE;

  *nextHop = routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the IP address of the next hop of this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    nextHop    @b{(input)} ip address of the next hop of this
*                                route entry
*
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteNextHopSet(L7_uint32 routeDest, L7_uint32 nextHop)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets the type of route.   
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    routeType  @b{(output)} The type of route.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Note that the values direct(3) and indirect(4) refer to 
*           the notion of direct and indirect routing in the IP
*           architecture.  
*
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteTypeGet(L7_uint32 routeDest, L7_uint32 *routeType)
{
  L7_routeEntry_t routeEntry;
  L7_uint32 intf_ip_addr;
  L7_uint32 intf_net_mask;

  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  /* do exact match lookup */
  if((rtoBestRouteLookup (routeDest, &routeEntry, L7_TRUE) != L7_SUCCESS) ||
     (routeDest != routeEntry.ipAddr))
      return L7_FAILURE;

  /* get interface address */
  if(ipMapRtrIntfIpAddressGet(routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum, 
                (L7_IP_ADDR_t *)&intf_ip_addr,
                  (L7_IP_MASK_t *)&intf_net_mask) != L7_SUCCESS) 
    return L7_FAILURE;

  /* compare network addresses */
  if ((routeEntry.ipAddr & intf_net_mask) == (intf_ip_addr & intf_net_mask))
    *routeType = L7_RTF_TYPE_DIRECT;
  else
    *routeType = L7_RTF_TYPE_INDIRECT;

  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Set the type of route.    
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    routeType  @b{(input)} The type of route.
*
* @returns  L7_NOT_SUPPORTED
*
* @comments Note that the values
*           direct(3) and indirect(4) refer to the notion of
*           direct and indirect routing in the IP
*           architecture. 
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteTypeSet(L7_uint32 routeDest, L7_uint32 routeType)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets the routing mechanism via which this route was
*           learned.  
*
* @param    routeDest   @b{(input)} route destination for this entry
* @param    *routeProto @b{(output)} protocol via which this route
*                               was learned
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteProtoGet(L7_uint32 routeDest, L7_uint32 *routeProto)
{
  L7_routeEntry_t routeEntry;
 
  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  /* do exact match lookup */
  if((rtoBestRouteLookup (routeDest, &routeEntry, L7_TRUE) != L7_SUCCESS) ||
     (routeDest != routeEntry.ipAddr))
      return L7_FAILURE;


  switch (routeEntry.protocol) 
  {
  case RTO_LOCAL:
    *routeProto = L7_IP_CIDR_ROUTE_PROTO_LOCAL;
    break;

  case RTO_STATIC:
    *routeProto = L7_IP_CIDR_ROUTE_PROTO_NETMGMT;
    break;

    case RTO_MPLS:
    *routeProto = L7_IP_CIDR_ROUTE_PROTO_OTHER;
    break;

    case RTO_OSPF_INTRA_AREA:
    case RTO_OSPF_INTER_AREA:
    case RTO_OSPF_TYPE1_EXT:
    case RTO_OSPF_TYPE2_EXT:
    case RTO_OSPF_NSSA_TYPE1_EXT:
    case RTO_OSPF_NSSA_TYPE2_EXT:
    *routeProto = L7_IP_CIDR_ROUTE_PROTO_OSPF;
    break;

    case RTO_RIP:
    *routeProto = L7_IP_CIDR_ROUTE_PROTO_RIP;
    break;

    case RTO_DEFAULT:
    *routeProto = L7_IP_CIDR_ROUTE_PROTO_NETMGMT;
    break;

    case RTO_IBGP:  
    case RTO_EBGP:
    *routeProto = L7_IP_CIDR_ROUTE_PROTO_BGP;
    break;

    case RTO_OSPFV3_INTRA_AREA:
    case RTO_OSPFV3_INTER_AREA:
    case RTO_OSPFV3_TYPE1_EXT:
    case RTO_OSPFV3_TYPE2_EXT:
    case RTO_OSPFV3_NSSA_TYPE1_EXT:
    case RTO_OSPFV3_NSSA_TYPE2_EXT:
    *routeProto = L7_IP_CIDR_ROUTE_PROTO_OSPF;
    break;

  default:
    *routeProto = L7_IP_CIDR_ROUTE_PROTO_OTHER;
    break;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of seconds since this route was last
*           updated or otherwise determined to be correct
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *routeAge  @b{(output)} seconds since the route was last updated
*
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteAgeGet(L7_uint32 routeDest, L7_uint32 *routeAge)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Sets the number of seconds since this route was last
*           updated or otherwise determined to be correct
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    routeAge   @b{(input)} Seconds since this route was last
*                               updated
*
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteAgeSet(L7_uint32 routeDest, L7_uint32 routeAge)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets the mask to be logical-ANDed with the
*           destination address before being compared to the
*           value in the ipRouteDest field
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *routeMask @b{(output)} route mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteMaskGet(L7_uint32 routeDest, L7_uint32 *routeMask)
{
  L7_routeEntry_t routeEntry;
 
  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  /* do exact match lookup */
  if((rtoBestRouteLookup (routeDest, &routeEntry, L7_TRUE) != L7_SUCCESS) ||
     (routeDest != routeEntry.ipAddr))
      return L7_FAILURE;

  *routeMask = routeEntry.subnetMask;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the mask to be logical-ANDed with the
*           destination address before being compared to the
*           value in the ipRouteDest field
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    routeMask  @b{(input)} route mask
*
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteMaskSet(L7_uint32 routeDest, L7_uint32 routeMask)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets an alternate routing metric for this route
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric5   @b{(output)} route metric 5
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteMetric5Get(L7_uint32 routeDest, L7_uint32 *metric5)
{
    *metric5 = -1;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets an alternate routing metric for this route
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric5   @b{(output)} route metric 5
*
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteMetric5Set(L7_uint32 routeDest, L7_uint32 *metric5)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets reference to MIB definitions specific to the
*           particular routing protocol which is responsible
*           for this route 
*
* @param    intIfNum   @b{(input)} internal interface number associated
*                               with this entry
* @param    *oid1      @b{(output)} object id
* @param    *oid2      @b{(output)} object id
*
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIpRouteInfoGet(L7_uint32 routeDest, L7_uint32 *oid1, L7_uint32* oid2)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  determines if the entry exists in the ipNetToMedia table
*          
* @param    ipNetToMediaIfIndex        @b{(input)} interface number
* @param    ipNetToMediaNetAddress     @b{(input)} network address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Includes errors received
*        
* @end
*********************************************************************/
L7_RC_t ipmIpNetToMediaEntryGet(L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress)
{
  L7_RC_t rc;
  L7_arpEntry_t pArp;

  /* check for the entry in the OS table */
  if (osapiIpNetToMediaEntryGet(ipNetToMediaIfIndex, ipNetToMediaNetAddress) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  /* check for the entry in the ARP table */
  if ((rc = ipMapIpArpEntryGet(ipNetToMediaNetAddress, ipNetToMediaIfIndex,
          &pArp)) == L7_SUCCESS &&
      pArp.intIfNum == ipNetToMediaIfIndex)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Retrieves the next valid ARP entry for the ipNetToMedia table
*          
* @param    *ipNetToMediaIfIndex        @b{(input)} internal interface number of current entry @line
*                                       @b{(output)} internal interface number of next entry
* @param    *ipNetToMediaNetAddress     @b{(input)} network address of current entry @line
*                                       @b{(output)} network address of next entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Utility function to wrap the 
*        
* @end
*********************************************************************/
L7_RC_t ipmIpNetToMediaEntryArpNextGet(L7_uint32 *ipNetToMediaIfIndex, L7_uint32 *ipNetToMediaNetAddress)
{
  L7_RC_t rc;
  L7_arpEntry_t arpEntry;

  L7_uint32 next_ipNetToMediaIfIndex = 0;
  L7_uint32 next_ipNetToMediaNetAddress = 0;

  L7_BOOL next_found = L7_FALSE;

  arpEntry.ipAddr = 0;
  arpEntry.intIfNum = 0;
  while ((rc = ipMapIpArpEntryNext(arpEntry.ipAddr, arpEntry.intIfNum,
          &arpEntry)) == L7_SUCCESS)
  {
    /* check that it's greater than or equal to the the provided interface */
      /* if the ifIndex greater it's ok, if it's the same, only if the NetAddress is greater */
    if (arpEntry.intIfNum > *ipNetToMediaIfIndex ||
        (arpEntry.intIfNum == *ipNetToMediaIfIndex &&
         arpEntry.ipAddr > *ipNetToMediaNetAddress))
    {
      if (next_found != L7_TRUE)
      {
        /* this is the first entry found */
        next_ipNetToMediaIfIndex = arpEntry.intIfNum;
        next_ipNetToMediaNetAddress = arpEntry.ipAddr;
        next_found = L7_TRUE;
      }
      else
      {
        /* check that it's less than or equal the previous found interface */
        /* if the ifIndex is the same, only if the NetAddress is less */
        if (arpEntry.intIfNum < next_ipNetToMediaIfIndex ||
            (arpEntry.intIfNum == next_ipNetToMediaIfIndex && 
             arpEntry.ipAddr < next_ipNetToMediaNetAddress))
        {
          next_ipNetToMediaIfIndex = arpEntry.intIfNum;
          next_ipNetToMediaNetAddress = arpEntry.ipAddr;
        }
        /* otherwise ignore this entry */
      }
    }
  }

  if (next_found == L7_TRUE)
  {
    *ipNetToMediaIfIndex = next_ipNetToMediaIfIndex;
    *ipNetToMediaNetAddress = next_ipNetToMediaNetAddress;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next valid entry in the ipNetToMedia table
*          
* @param    *ipNetToMediaIfIndex        @b{(input)} internal interface number of current entry @line
*                                       @b{(output)} internal interface number of next entry
* @param    *ipNetToMediaNetAddress     @b{(input)} network address of current entry @line
*                                       @b{(output)} network address of next entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Includes errors received
*        
* @end
*********************************************************************/
L7_RC_t ipmIpNetToMediaEntryNextGet(L7_int32 *ipNetToMediaIfIndex, L7_uint32 *ipNetToMediaNetAddress)
{
  L7_RC_t osapi_rc;
  L7_RC_t arp_rc;
  L7_uint32 osapi_ipNetToMediaIfIndex = *ipNetToMediaIfIndex;
  L7_uint32 osapi_ipNetToMediaNetAddress = *ipNetToMediaNetAddress;
  L7_uint32 arp_ipNetToMediaIfIndex = *ipNetToMediaIfIndex;
  L7_uint32 arp_ipNetToMediaNetAddress = *ipNetToMediaNetAddress;

  /* retrieve entry from OS table */
  osapi_rc = osapiIpNetToMediaEntryNextGet(&osapi_ipNetToMediaIfIndex, &osapi_ipNetToMediaNetAddress);

  /* retrieve entry from ARP table */
  arp_rc = ipmIpNetToMediaEntryArpNextGet(&arp_ipNetToMediaIfIndex, &arp_ipNetToMediaNetAddress);

  if (osapi_rc != L7_SUCCESS && arp_rc != L7_SUCCESS)
  {
    /* if neither table contains this entry, return failure */
    return L7_FAILURE;
  }

  if (osapi_rc == L7_SUCCESS)
  {
    if (arp_rc == L7_SUCCESS)
    {
      /* determine which is the next entry */
      if ((osapi_ipNetToMediaIfIndex < arp_ipNetToMediaIfIndex) ||
          ((osapi_ipNetToMediaIfIndex == arp_ipNetToMediaIfIndex) &&
           (osapi_ipNetToMediaNetAddress < arp_ipNetToMediaNetAddress)))
      {
        /* return OS entry */
        *ipNetToMediaIfIndex = osapi_ipNetToMediaIfIndex;
        *ipNetToMediaNetAddress = osapi_ipNetToMediaNetAddress;
      }
      else
      {
        /* return ARP entry */
        *ipNetToMediaIfIndex = arp_ipNetToMediaIfIndex;
        *ipNetToMediaNetAddress = arp_ipNetToMediaNetAddress;
      }
    }
    else
    {
      /* return OS entry */
      *ipNetToMediaIfIndex = osapi_ipNetToMediaIfIndex;
      *ipNetToMediaNetAddress = osapi_ipNetToMediaNetAddress;
    }
  }
  else
  {
    /* return ARP entry */
    *ipNetToMediaIfIndex = arp_ipNetToMediaIfIndex;
    *ipNetToMediaNetAddress = arp_ipNetToMediaNetAddress;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the ipNetToMediaPhysAddress object
*          
* @param    ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param    ipNetToMediaNetAddress     @b{(input)}  network address
* @param    *physAddress               @b{(output)} physAddress buffer
* @param    *addrLength                @b{(output)} physAddress buffer length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Includes errors received
*        
* @end
*********************************************************************/
L7_RC_t ipmIpNetToMediaPhysAddressGet(L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_char8 *physAddress, L7_uint32 *addrLength)
{
  L7_RC_t rc;
  L7_arpEntry_t pArp;

  /* check for the entry in the OS table*/
  if (osapiIpNetToMediaEntryGet(ipNetToMediaIfIndex, ipNetToMediaNetAddress) == L7_SUCCESS)
  {
    /* if it exists, return this entry */
    return osapiIpNetToMediaPhysAddressGet(ipNetToMediaIfIndex, ipNetToMediaNetAddress, physAddress, addrLength);
  }

  /* check for the entry in the ARP table*/
  if ((rc = ipMapIpArpEntryGet(ipNetToMediaNetAddress, ipNetToMediaIfIndex,
          &pArp)) == L7_SUCCESS && pArp.intIfNum == ipNetToMediaIfIndex)
  {
    /* if it exists, return this entry */
    memcpy(physAddress, pArp.macAddr.addr.enetAddr.addr, pArp.macAddr.len);
    *addrLength = pArp.macAddr.len;
    return L7_SUCCESS;
  }

  /* neither table contains this entry */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the ipNetToMediaPhysAddress object
*          
* @param    ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param    ipNetToMediaNetAddress     @b{(input)}  network address
* @param    physAddress                @b{(input)}  physAddress buffer
* @param    addrLength                 @b{(input)}  physAddress buffer length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Includes errors received
*        
* @end
*********************************************************************/
L7_RC_t ipmIpNetToMediaPhysAddressSet(L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_char8 *physAddress, L7_uint32 addrLength)
{
  /* check for the entry in the OS table*/
  if (osapiIpNetToMediaEntryGet(ipNetToMediaIfIndex, ipNetToMediaNetAddress) == L7_SUCCESS)
  {
    /* if it exists, set this entry */
    return osapiIpNetToMediaPhysAddressSet(ipNetToMediaIfIndex, ipNetToMediaNetAddress, physAddress, addrLength);
  }

  /* only the OS table can set this value, so return failure if it doesn't have this entry */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  get the ipNetToMediaType object
*          
* @param    ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param    ipNetToMediaNetAddress     @b{(input)}  network address
* @param    *ipNetToMediaType          @b{(output)} ipNetToMediaType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Includes errors received
*        
* @end
*********************************************************************/
L7_RC_t ipmIpNetToMediaTypeGet(L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_uint32 *ipNetToMediaType)
{
  L7_RC_t rc;
  L7_arpEntry_t pArp;

  /* check for the entry in the OS table*/
  if (osapiIpNetToMediaEntryGet(ipNetToMediaIfIndex, ipNetToMediaNetAddress) == L7_SUCCESS)
  {
    /* if it exists, return this entry */
    return osapiIpNetToMediaTypeGet(ipNetToMediaIfIndex, ipNetToMediaNetAddress, ipNetToMediaType);
  }

  /* check for the entry in the ARP table*/
  if ((rc = ipMapIpArpEntryGet(ipNetToMediaNetAddress, ipNetToMediaIfIndex,
          &pArp)) == L7_SUCCESS && pArp.intIfNum == ipNetToMediaIfIndex)
  {
    /* convert ARP flags to media type */
    if (pArp.flags & L7_ARP_LOCAL ||
        pArp.flags & L7_ARP_GATEWAY)
    {
      *ipNetToMediaType = L7_IP_AT_TYPE_DYNAMIC;
    }
    else if (pArp.flags & L7_ARP_STATIC)
    {
      *ipNetToMediaType = L7_IP_AT_TYPE_STATIC;
    }
    else
    {
      *ipNetToMediaType = L7_IP_AT_TYPE_OTHER;
    }

    /* if it exists, return this entry */
    return L7_SUCCESS;
  }

  /* neither table contains this entry */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  set the ipNetToMediaType object
*          
* @param    ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param    ipNetToMediaNetAddress     @b{(input)}  network address
* @param    ipNetToMediaType           @b{(input)}  ipNetToMediaType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Includes errors received
*        
* @end
*********************************************************************/
L7_RC_t ipmIpNetToMediaTypeSet(L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_uint32 ipNetToMediaType)
{
  /* check for the entry in the OS table*/
  if (osapiIpNetToMediaEntryGet(ipNetToMediaIfIndex, ipNetToMediaNetAddress) == L7_SUCCESS)
  {
    /* if it exists, set this entry */
    return osapiIpNetToMediaTypeSet(ipNetToMediaIfIndex, ipNetToMediaNetAddress, ipNetToMediaType);
  }

  /* only the OS table can set this value, so return failure if it doesn't have this entry */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the total number of ICMP messages which the entity 
*           received.
*
* @param    *val   @b{(output)} Number of received icmp messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The total number of ICMP messages which the entity
*           received.  Note that this counter includes all those 
*           counted by icmpInErrors.
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpInMsgsGet(L7_uint32 *val)
{
  return osapiIcmpInMsgsGet(val);
}

/*********************************************************************
* @purpose  Get the number of ICMP messages which the entity received
*           but determined as having ICMP specific errors (bad ICMP
*           checksums, bad length, etc.)
*
* @param    *val  @b{(output)} Number of received icmp messages with errors
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The number of ICMP messages which the entity received
*           but determined as having ICMP specific errors (bad ICMP
*           checksums, bad length, etc.)
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpInErrorsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpInErrorsGet(val) == L7_SUCCESS)
  {
    *val += (L7_icmpstat.icps_badcode + L7_icmpstat.icps_badlen +
             L7_icmpstat.icps_checksum + L7_icmpstat.icps_tooshort);

    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Destination Unreachable messages
*           received.
*
* @param    *val   @b{(output)}  Number of icmp dest unreach messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The number of ICMP Destination Unreachable messages
*           received.
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpInDestUnreachsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpInDestUnreachsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_inhist[ICMP_UNREACH];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Time Exceeded messages received.
*
* @param    *val   @b{(output)} Number of icmp time exceeded messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The number of ICMP Time Exceeded messages received.
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpInTimeExcdsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpInTimeExcdsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_inhist[ICMP_TIMXCEED];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Parameter Problem messages received
*
* @param    *val   @b{(output)} Number of received icmp parameter
*                       problem messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpInParmProbsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpInParmProbsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_inhist[ICMP_PARAMPROB];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Source Quench messages.
*
* @param    *val   @b{(output)} Number of received icmp sorce quench messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpInSrcQuenchsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpInSrcQuenchsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_inhist[ICMP_SOURCEQUENCH];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Redirect messages received.
*
* @param    *val   @b{(output)} Number of received icmp redirect messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpInRedirectsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpInRedirectsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_inhist[ICMP_REDIRECT];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Echo (request) messages received. 
*
* @param    *val   @b{(output)} Number of received icmp echo request messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpInEchosGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpInEchosGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_inhist[ICMP_ECHO];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Echo Reply messages received
*
* @param    *val   @b{(output)} Number of received icmp echo response messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpInEchoRepsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpInEchoRepsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_inhist[ICMP_ECHOREPLY];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Timestamp (request) messages received
*
* @param    *val   @b{(output)} Number of received icmp timestamp messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpInTimestampsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpInTimestampsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_inhist[ICMP_TSTAMP];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Timestamp Reply messages
*
* @param    *val  @b{(output)} Number of received icmp timestamp response messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpInTimestampRepsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpInTimestampRepsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_inhist[ICMP_TSTAMPREPLY];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Address Mask Request messages received
*
* @param    *val  @b{(output)} Number of received icmp address mask req messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpInAddrMasksGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpInAddrMasksGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_inhist[ICMP_MASKREQ];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Address Mask Reply messages received
*
* @param    *val   @b{(output)} Number of received icmp address mask replies messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpInAddrMaskRepsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpInAddrMaskRepsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_inhist[ICMP_MASKREPLY];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the total number of ICMP messages which this entity
*           attempted to send.
*
* @param    *val  @b{(output)} Number of icmp messages sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Note that this counter includes all those counted by
*           icmpOutErrors
* @end
*********************************************************************/
L7_RC_t ipmIcmpOutMsgsGet(L7_uint32 *val)
{
  return osapiIcmpOutMsgsGet(val);
}

/*********************************************************************
* @purpose  Get the number of ICMP messages which this entity did not
*           send due to problems discovered within ICMP.
*
* @param    *val    @b{(output)} Number of icmp messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This includes problems such as lack of buffers. This
*           value should not include errors discovered outside of
*           the ICMP layer such as the inability of IP to route the
*           resultant datagram. In some implementations there may
*           be no types of error which contribute to this counter's
*           value.
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpOutErrorsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpOutErrorsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_oldicmp + L7_icmpstat.icps_oldshort;
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Destination Unreachable messages
*           sent
*
* @param    *val  @b{(output)} Number of sent icmp destination
*                          unreachable messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpOutDestUnreachsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpOutDestUnreachsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_outhist[ICMP_UNREACH];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Time Exceeded messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp time
*                          exceeded messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpOutTimeExcdsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpOutTimeExcdsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_outhist[ICMP_TIMXCEED];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Parameter Problem messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp parameter
*                          problem messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpOutParmProbsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpOutParmProbsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_outhist[ICMP_PARAMPROB];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Source Quench messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp source quench messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpOutSrcQuenchsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpOutSrcQuenchsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_outhist[ICMP_SOURCEQUENCH];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Redirect messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp redirect messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments For a host, this object will always be zero, since
*           hosts do not send redirects.
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpOutRedirectsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpOutRedirectsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_outhist[ICMP_REDIRECT];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Echo (request) messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp echo request messages
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpOutEchosGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpOutEchosGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_outhist[ICMP_ECHO];
    *val += osapiL7IcmpOutEchosGet();
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Echo Reply messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp echo reply messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpOutEchoRepsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpOutEchoRepsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_outhist[ICMP_ECHOREPLY];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Timestamp (request) messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp timestamp request messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpOutTimestampsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpOutTimestampsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_outhist[ICMP_TSTAMP];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Timestamp Reply messages sent
*
* @param    *val  @b{(output)} Number of sent icmp timestamp reply messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpOutTimestampRepsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpOutTimestampRepsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_outhist[ICMP_TSTAMPREPLY];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Address mask Request messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp address mask request
*                          messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpOutAddrMasksGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpOutAddrMasksGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_outhist[ICMP_MASKREQ];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of ICMP Address Mask Reply messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp address mask reply messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t ipmIcmpOutAddrMaskRepsGet(L7_uint32 *val)
{
  *val = 0;
  if (osapiIcmpOutAddrMaskRepsGet(val) == L7_SUCCESS)
  {
    *val += L7_icmpstat.icps_outhist[ICMP_MASKREPLY];
    return L7_SUCCESS;
  }

  *val = 0;
  return L7_SUCCESS;
}



