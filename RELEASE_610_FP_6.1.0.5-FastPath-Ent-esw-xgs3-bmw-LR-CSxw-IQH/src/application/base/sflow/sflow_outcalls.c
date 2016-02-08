/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007-2006
*
**********************************************************************
* @filename  sFlow_outcalls.c
*
* @purpose    Contains definitions to APIs used to interract with
*             external components
*
* @component sFlow
*
* @comments
*
* @create    10/27/2006
*
* @author    drajendra
*
* @end
*
**********************************************************************/
#include "commdefs.h"
#include "nimapi.h"
#include "dot3ad_api.h"
#include "usmdb_common.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_1643_stats_api.h"
#include "usmdb_2233_stats_api.h"
#include "ipv6_commdefs.h"
#include "usmdb_util_api.h"
#include "sflow_proto.h"
#include "l3_addrdefs.h"
/*********************************************************************
* @purpose  Determine if the interface type is valid for sFlow
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE   Valid interface type
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL sFlowIsValidIntfType(L7_uint32 sysIntfType)
{
  switch (sysIntfType)
  {
    case L7_PHYSICAL_INTF:
      return L7_TRUE;
    default:
      break;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine if the interface is valid for sFlow
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE   Supported interface
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL sFlowIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;
  L7_RC_t         rc = L7_FALSE;

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
  {
    return rc;
  }

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    switch (sysIntfType)
    {
      case L7_PHYSICAL_INTF:
        rc = L7_TRUE;
        break;
      default:
        rc = L7_FALSE;
        break;
    }
  }
  return rc;
}
/*********************************************************************
* @purpose  Checks if the interface/VLAN ID can be enabled for sFlow
*
* @param    dsIndex  @b{(input)} Data source
*
* @returns  L7_TRUE, if datasource can be enabled
* @returns  L7_FALSE, if datasource cannot be enabled
*
* @notes    Acceptability criteria
*              1. If data source is interface, it must be in attached
*                 state
*
*
* @end
*********************************************************************/
L7_BOOL sFlowIsDataSourceReady(L7_uint32 dsIndex)
{
  L7_NIM_QUERY_DATA_t  nimQueryData;
 
  memset((void *)&nimQueryData,0,sizeof(nimQueryData));
  nimQueryData.request  = L7_NIM_QRY_RQST_STATE;
  nimQueryData.intIfNum = SFLOW_DS_INDEX_TO_VAL(dsIndex);

  if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
  {
    /* should never get here */
    LOG_MSG("sFlowIntfCanBeEnabled: nimIntfQuery failed for intIfNum %d\n", 
             nimQueryData.intIfNum);
    return L7_FALSE;
  }

  if (nimQueryData.data.state == L7_INTF_ATTACHED ||
      nimQueryData.data.state == L7_INTF_ATTACHING)
  {
    if (dot3adIsLagActiveMember(nimQueryData.intIfNum) == L7_FALSE)
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}
/*********************************************************************
* @purpose  Get the requested counter from stats collector
*
* @param     intIfNum    @b{(input)}  internal interface number
* @param     counterType @b{(input)}  enum specifying the counter
*                                     SFLOW_ETH_COUNTERS_t
*                                     SFLOW_IF_COUNTERS_t
* @param    *val         @b{(output)} ptr to 32bit counter where the
*                                     value will be saved
* @param    *longval     @b{(output)} ptr to 64bit counter where the
*                                     value will be saved
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void sFlowCounterValGet(L7_uint32 intIfNum, L7_uint32 counterType,
                        L7_uint32 *val, L7_uint64 *longval)
{
  L7_uint32 uintVal = L7_NULL, high, low;

  *val     = L7_NULL;
  *longval = L7_NULL;
  switch(counterType)
  {
/* IF COUNTERS */
   case SFLOW_IF_TYPE:
    /* Get ifType. */
    if (nimGetIfType(intIfNum, &uintVal) == L7_SUCCESS)
    {
      /* For all Ethernet types. */
      if ((uintVal == L7_IANA_FAST_ETHERNET)
      || (uintVal == L7_IANA_FAST_ETHERNET_FX)
      || (uintVal == L7_IANA_GIGABIT_ETHERNET)
      || (uintVal == L7_IANA_2G5BIT_ETHERNET)
      || (uintVal == L7_IANA_10G_ETHERNET))
      {
        /* Map to Ethernet type (6). */
        *val = L7_IANA_ETHERNET;
      }
    }
    break;
  case SFLOW_IF_SPEED:
  if (nimGetIntfSpeedStatus(intIfNum, &uintVal) == L7_SUCCESS)
  {
    switch (uintVal)
    {
    case L7_PORTCTRL_PORTSPEED_FULL_100TX:
      /* 100,000,000 bps */
      *longval =  SFLOW_HUNDRED_MBPS;
      break;
    case L7_PORTCTRL_PORTSPEED_HALF_100TX:
      /* 100,000,000 bps */
      *longval = SFLOW_HUNDRED_MBPS;
      break;
    case L7_PORTCTRL_PORTSPEED_FULL_10T:
      /* 10,000,000 bps */
      *longval = SFLOW_TEN_MBPS;
      break;
    case L7_PORTCTRL_PORTSPEED_HALF_10T:
      /* 10,000,000 bps */
      *longval = SFLOW_TEN_MBPS; 
      break;
    case L7_PORTCTRL_PORTSPEED_FULL_100FX:
      /* 100,000,000 bps */
      *longval = SFLOW_HUNDRED_MBPS;
      break;
    case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
    case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
      /* 1,000,000,000 bps */
      *longval = SFLOW_THOUSAND_MBPS; 
      break;
    case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
      /* 4,294,967,295 bps (see MIB description) */
      *longval = SFLOW_IFSPEEDMAX_MBPS; 
      break;
    case L7_PORTCTRL_PORTSPEED_LAG:
      /* get data rate from LAG component */
      if (usmDbIfLAGSpeedGet(intIfNum, &uintVal) == L7_SUCCESS) 
      {
        if(uintVal > (SFLOW_IFSPEEDMAX_MBPS / SFLOW_ONE_MBPS))
          *longval = SFLOW_IFSPEEDMAX_MBPS; /* 4,294,967,295 bps */
        else
          *longval = uintVal * SFLOW_ONE_MBPS;
      }
      break;
    case L7_PORTCTRL_PORTSPEED_UNKNOWN:
      *val = 0;
      break;
    default:
      *val = 0;
    }
  }
    break;
  case SFLOW_IF_DIRECTION:
    if (nimIsIntfSpeedFullDuplex(intIfNum) == L7_TRUE)
    {
       *val = SFLOW_FULL_DPLX;
    }
    else
    {
       *val = SFLOW_HALF_DPLX;
    }
    break;
  case SFLOW_IF_STATUS:
    if (nimGetIntfAdminState(intIfNum, &uintVal) == L7_SUCCESS)
    {
       if (uintVal == L7_ENABLE)
       {
         *val |= 0x01;
       }
    }
    if (nimGetIntfLinkState(intIfNum, &uintVal) == L7_SUCCESS)
    {
       if (uintVal == L7_UP)
       {
         *val |= 0x02;
       }
    }
    break;
  case SFLOW_IF_IN_OCTETS:
    if (usmDbIfHCInOctetsGet(L7_NULL, intIfNum, &high, &low) == L7_SUCCESS)
    {
      *longval = high;
      *longval = ((*longval) << 32) + low;
    }
    break;
  case SFLOW_IF_IN_UCAST_PKTS:
    if (usmDbIfInUcastPktsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
  case SFLOW_IF_IN_MCAST_PKTS:
    if (usmDbIfInMulticastPktsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
  case SFLOW_IF_IN_BCAST_PKTS:
    if (usmDbIfInBroadcastPktsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
  case SFLOW_IF_IN_DISCARDS:
    if (usmDbIfInDiscardsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
  case SFLOW_IF_IN_ERRORS:
    if (usmDbIfInErrorsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
  case SFLOW_IF_IN_UNKNWN_PROTOS:
    if (usmDbIfInUnknownProtosGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
  case SFLOW_IF_OUT_OCTETS:
    if (usmDbIfHCOutOctetsGet(L7_NULL, intIfNum, &high, &low) == L7_SUCCESS)
    {
      *longval = high;
      *longval = ((*longval) << 32) + low;
    }
    break;
  case SFLOW_IF_OUT_UCAST_PKTS:
    if (usmDbIfOutUcastPktsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
  case SFLOW_IF_OUT_MCAST_PKTS:
    if (usmDbIfOutMulticastPktsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
  case SFLOW_IF_OUT_BCAST_PKTS:
    if (usmDbIfOutBroadcastPktsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
  case SFLOW_IF_OUT_DISCARDS:
    if (usmDbIfOutDiscardsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
  case SFLOW_IF_OUT_ERRORS:
    if (usmDbIfOutErrorsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
  case SFLOW_IF_PROMISCOUS_MODE:
    if (usmDbIfPromiscuousModeGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      if (uintVal == L7_TRUE)
      {
        *val = SFLOW_IF_PROMISCOUS_MODE_TRUE;
      }
      else if (uintVal == L7_FALSE)
      {
        *val = SFLOW_IF_PROMISCOUS_MODE_FALSE;
      }
    }
    break;
/* ETH_COUNTERS */
    case SFLOW_ETH_ALIGNMENT_ERRORS:
    if (usmDbDot3StatsAlignmentErrorsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
    case SFLOW_ETH_FCS_ERRORS:
    if (usmDbDot3StatsFCSErrorsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
    case SFLOW_ETH_SINGLE_COLLISION_FRAMES:
    if (usmDbDot3StatsSingleCollisionFramesGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
    case SFLOW_ETH_MULTIPLE_COLLISION_FRAMES:
    if (usmDbDot3StatsMultipleCollisionFramesGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
    case SFLOW_ETH_SQE_TEST_ERRORS:
    if (usmDbDot3StatsSQETestErrorsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
    case SFLOW_ETH_DEFERRED_TX:
    if (usmDbDot3StatsDeferredTransmissionsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
    case SFLOW_ETH_LATE_COLLISIONS:
    if (usmDbDot3StatsLateCollisionsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
    case SFLOW_ETH_EXCESSIVE_COLLISIONS:
    if (usmDbDot3StatsExcessiveCollisionsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
    case SFLOW_ETH_INTERNAL_MAC_TX_ERRORS:
    if (usmDbDot3StatsInternalMacTransmitErrorsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
    case SFLOW_ETH_CARIER_SENSE_ERRORS:
    if (usmDbDot3StatsCarrierSenseErrorsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
    case SFLOW_ETH_FRAME_TOO_LONGS:
    if (usmDbDot3StatsFrameTooLongsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
    case SFLOW_ETH_INTERNAL_MAC_RX_ERRORS:
    if (usmDbDot3StatsInternalMacReceiveErrorsGet(L7_NULL, intIfNum, &uintVal) == L7_SUCCESS)
    {
      *val = uintVal;
    }
    break;
    case SFLOW_ETH_SYMBOL_ERRORS:
    default:
    break;
  }
}
/********************************************************************* 
* @purpose  This function is used to get the Agent Address . 
* 
* @param    strIpAddr   Agent Address in Printable format 
* 
* 
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
* 
* @notes    none 
* 
* @end 
*********************************************************************/
L7_RC_t sFlowAgentAddrGet(L7_inet_addr_t *ipAddr)
{
  L7_BOOL    flag = L7_FALSE;
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  L7_uchar8 strIpAddr[L7_SYS_SIZE];
  L7_uint32       acount, idx;
  L7_in6_prefix_t ipv6Prefixes[L7_RTR6_MAX_INTF_ADDRS*2];
#endif
/* Try to get IP address from network port, if IP address is NULL,
 * then check IP address for Service Port */

  ipAddr->family = L7_AF_INET;
  usmDbAgentIpIfAddressGet(L7_NULL, &(ipAddr->addr.ipv4.s_addr));
  if( ipAddr->addr.ipv4.s_addr != L7_NULL)
  {
    flag = L7_TRUE;  
  }

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  acount = L7_RTR6_MAX_INTF_ADDRS*2;
  memset(strIpAddr, L7_NULL, L7_SYS_SIZE);
  if ( ipAddr->addr.ipv4.s_addr == L7_NULL &&
      (usmDbAgentIpIfIPV6AddrsGet( ipv6Prefixes, &acount) == L7_SUCCESS))
  {
    for (idx = 0; idx < acount; idx++)
    {
      if (L7_IP6_IS_ADDR_LINK_LOCAL(&ipv6Prefixes[idx].in6Addr) == 0)
        {
           if(L7_IP6_IS_ADDR_UNSPECIFIED(&ipv6Prefixes[idx].in6Addr))
            continue;
          
          memcpy((L7_uchar8 *)&ipAddr->addr.ipv6, 
                 (L7_uchar8 *)&ipv6Prefixes[idx].in6Addr, sizeof(L7_in6_addr_t)); 
          ipAddr->family = L7_AF_INET6;
          flag = L7_TRUE;
          break;
        }
    }
  }
#endif
  if( (flag == L7_FALSE) &&
       (ipAddr->addr.ipv4.s_addr == L7_NULL))
  {
    ipAddr->family = L7_AF_INET;
    usmDbServicePortIPAddrGet(L7_NULL, &(ipAddr->addr.ipv4.s_addr)); 
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
    if ( ipAddr->addr.ipv4.s_addr == L7_NULL &&
       (simGetServPortIPV6Addrs( ipv6Prefixes, &acount) == L7_SUCCESS))
    {
      for (idx = 0; idx < acount; idx++)
      {
        if (L7_IP6_IS_ADDR_LINK_LOCAL(&ipv6Prefixes[idx].in6Addr) == 0)
        {
          if(L7_IP6_IS_ADDR_UNSPECIFIED(&ipv6Prefixes[idx].in6Addr))
            continue;
          
          memcpy((L7_uchar8 *)&ipAddr->addr.ipv6, 
                 (L7_uchar8 *)&ipv6Prefixes[idx].in6Addr, sizeof(L7_in6_addr_t)); 
          ipAddr->family = L7_AF_INET6;
          break;
        }
      }
    }
#endif
  }
  return L7_SUCCESS;

}
