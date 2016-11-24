/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename rlimtnnl.c
*
* @purpose Primary tunnel-specific operational code.
*
* @component Routing Logical Interface Manager
*
* @comments
*
* @create 06/16/2005
*
* @author eberge
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_rlim_api.h"
#include "log.h"
#include "rlim.h"
#include "l7_ip_api.h"
#include "l7_ip6_api.h"
#include "rto_api.h"
#include "nimapi.h"
#include "osapi.h"

L7_uint32 intIfNumToTunnelId[L7_MAX_INTERFACE_COUNT + 1];

extern L7_BOOL ipMapIntfIsUp(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Initialize tunnel operational data
*
* @param    top - tunnel operational data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelOpDataInit(rlimTunnelOpData_t *top)
{
  void *saveSema;

  saveSema = top->deleteSema;
  memset(top, 0, sizeof(*top));
  top->deleteSema = saveSema;
}

/*********************************************************************
* @purpose  Create operational state for a tunnel interface
*
* @param    tunnelId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimTunnelOpCreate(L7_uint32 tunnelId)
{
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];
  L7_RC_t rc;
  L7_uint32 intIfNum;

  if (top->tunnelIntIfNum != 0)
  {
    return L7_ALREADY_CONFIGURED;
  }

  rlimTunnelOpDataInit(top);

  rc = rlimIntfCreateInNim(L7_TUNNEL_INTF, tunnelId, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("failed to create tunnel%u in NIM (rc=%u)\n", tunnelId, rc);
    return rc;
  }

  top->tunnelIntIfNum = intIfNum;
  if(intIfNum <= L7_MAX_INTERFACE_COUNT) rlimIntIfNumToTunnelId[intIfNum] = tunnelId;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the operational state of a tunnel interface
*
* @param    intIfNum
* @param    tunnelId
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelOpDelete(L7_uint32 intIfNum, L7_uint32 tunnelId)
{
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

  if (top->tunnelIntIfNum != intIfNum)
  {
    LOG_MSG("invalid tunnelId/intIfNum combo\n");
    return;
  }

  top->flags |= RLIM_TNNLOP_FLAG_DELETE_IN_PROGRESS;
  rlimTunnelOpUpdate(tunnelId);
  if(intIfNum <= L7_MAX_INTERFACE_COUNT) rlimIntIfNumToTunnelId[intIfNum] = RLIM_MAX_TUNNEL_ID +1;
}

/*********************************************************************
* @purpose  Wait for the completion of a tunnel deletion
*
* @param    tunnelId
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelOpDeleteWait(L7_uint32 tunnelId)
{
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];
  RLIM_SEMA_TAKE(top->deleteSema, L7_WAIT_FOREVER);
}

/*********************************************************************
* @purpose  Signal the completion of the deletion of a tunnel from NIM
*
* @param    intIfNum
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelNimDeleteDone(NIM_NOTIFY_CB_INFO_t retVal)
{
  L7_uint32 tunnelId;

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  for (tunnelId = 0; tunnelId <= RLIM_MAX_TUNNEL_ID; tunnelId++)
  {
    rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];
    if (top->tunnelIntIfNum == retVal.intIfNum)
    {
      rlimTunnelOpDataInit(top);
      RLIM_SEMA_GIVE(top->deleteSema);
      break;
    }
  }

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);
}

/*********************************************************************
* @purpose  Update the operational state of the tunnel
*
* @param    tunnelId
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelOpUpdate(L7_uint32 tunnelId)
{
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];
  L7_RC_t rc;

  if ((top->flags & (RLIM_TNNLOP_FLAG_ENABLED |
                     RLIM_TNNLOP_FLAG_DISABLE_IN_PROGRESS)) == 0)
  {
    if (rlimTunnelCanEnable(tunnelId) &&
        rlimTunnelEnable(tunnelId) == L7_SUCCESS)
    {
      top->flags |= RLIM_TNNLOP_FLAG_ENABLED;
    }
  }
  else
  {
    rlimTunnelDisable(tunnelId);
    top->flags &= ~RLIM_TNNLOP_FLAG_ENABLED;
    
    if (rlimTunnelCanEnable(tunnelId) == L7_TRUE &&
        rlimTunnelEnable(tunnelId) == L7_SUCCESS)
    {
      top->flags |= RLIM_TNNLOP_FLAG_ENABLED;
    }
  }

  /*
   * If the disabled tunnel had a duplicate, check if we should
   * enable the duplicate now.
   */
  if ((top->flags & RLIM_TNNLOP_FLAG_ENABLED) == 0)
  {
    if ((top->flags & RLIM_TNNLOP_FLAG_HAS_DUPLICATE) != 0)
    {
      top->flags &= ~RLIM_TNNLOP_FLAG_HAS_DUPLICATE;
      rlimTunnelDuplicateCheck();
    }
  }

  /*
   * If a tunnel delete is in progress, remove the tunnel
   * if it is fully disabled.
   */
  if ((top->flags & RLIM_TNNLOP_FLAG_DELETE_IN_PROGRESS) != 0)
  {
    if ((top->flags & (RLIM_TNNLOP_FLAG_ENABLED |
                       RLIM_TNNLOP_FLAG_DISABLE_IN_PROGRESS)) == 0)
    {
#ifdef L7_IPV6_PACKAGE
      rlim6to4TunnelDestPurge(tunnelId);
#endif
      rc = rlimIntfDeleteInNim(top->tunnelIntIfNum);
      if (rc != L7_SUCCESS)
      {
        LOG_MSG("failed to delete tunnel%u in NIM (rc=%u)\n", tunnelId, rc);
      }
    }
  }
}

/*********************************************************************
* @purpose  Enable the tunnel in the system stack and hardware
*
* @param    tunnelId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimTunnelEnable(L7_uint32 tunnelId)
{
  L7_sockaddr_union_t srcAddr, dstAddr, nhAddr;
  L7_uint32 nhIntIfNum;
  rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 ttl; /* default ttl for tunnel should be populated by app*/

  if (rlimTunnelAddrsGet(tunnelId, &srcAddr, &dstAddr) != L7_SUCCESS)
  {
    LOG_MSG("failed to get src/dst addrs for tunnel%u\n", tunnelId);
    return L7_FAILURE;
  }

  if (rlimTunnelNextHopGet(tunnelId, &nhAddr, &nhIntIfNum) != L7_SUCCESS)
  {
    LOG_MSG("failed to get nexthop info for tunnel%u\n", tunnelId);
    return L7_FAILURE;
  }

  /* remains constant over life of tunnel in stack */
  top->tunnelActiveMode = tcp->tunnelMode;

  /* Populate the TTL to be used in the outer IP */
  ttl = ipMapIpDefaultTTLGet();
  /*
   * Create the tunnel interface in the OS IP Stack
   */
  rc = osapiTunnelIfCreate(top->tunnelIntIfNum, tcp->tunnelMode, &srcAddr,
                           &dstAddr, ttl);
  if (rc == L7_SUCCESS)
  {
    top->flags |= RLIM_TNNLOP_FLAG_OSAPI_INTF_CREATED;
  }
  else
  {
    LOG_MSG("failed to create tunnel%u in stack\n", tunnelId);
  }

  /*
   * Create the tunnel in hardware
   */
  if (rc == L7_SUCCESS)
  {
    rc = dtlTunnelCreate(top->tunnelIntIfNum, tcp->tunnelMode, &srcAddr,
                         &dstAddr, &nhAddr, nhIntIfNum);
    if (rc == L7_SUCCESS)
    {
      top->flags |= RLIM_TNNLOP_FLAG_DTL_INTF_CREATED;
    }
    else
    {
      LOG_MSG("failed to create tunnel%u vi DTL\n", tunnelId);
    }
  }

  /*
   * Enable IPv4 routing if the tunnel can be an IPv4 interface
   */
  if (rc == L7_SUCCESS && RLIM_TUNNEL_CANBE_IP4_INTF(tcp->tunnelMode))
  {
    rc = ipMapRtrIntfModeSet(top->tunnelIntIfNum, L7_ENABLE);
    if (rc == L7_SUCCESS)
    {
      top->flags |= RLIM_TNNLOP_FLAG_IP4_ROUTING;
    }
    else
    {
      LOG_MSG("failed to enable routing on tunnel%u\n", tunnelId);
    }
  }

#ifdef L7_IPV6_PACKAGE
  /*
   * Enable IPv6 routing if the tunnel can be an IPv6 interface
   */
  if (rc == L7_SUCCESS && RLIM_TUNNEL_CANBE_IP6_INTF(tcp->tunnelMode))
  {
    rc = ip6MapRtrIntfModeSet(top->tunnelIntIfNum, L7_ENABLE);
    if (rc == L7_SUCCESS)
    {
      top->flags |= RLIM_TNNLOP_FLAG_IP6_ROUTING;
    }
    else
    {
      LOG_MSG("failed to enable routing on tunnel%u\n", tunnelId);
    }
  }
#endif

  if (rc != L7_SUCCESS)
  {
    rlimTunnelDisable(tunnelId);
    return L7_FAILURE;
  }

  rlimNimEventSend(top->tunnelIntIfNum, L7_UP);
#ifdef L7_IPV6_PACKAGE
  rlimTunnel6to4NhopChangeProcess();
#endif

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Disable a tunnel in the system stack and hardware
*
* @param    tunnelId
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelDisable(L7_uint32 tunnelId)
{
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

  /*
   * First-half of disable processing
   *
   * Do all the things we can do unilaterially, basically
   * inform ipMap/ip6Map to cleanup their state.  When
   * we get the indications back that routing is fully
   * disabled, do the 2nd half processing below (i.e.,
   * call this function again.
   *
   * The flag RLIM_TNNLOP_FLAG_DISABLE_IN_PROGRESS indicates
   * that we are in the intermediate state between the 1st
   * and 2nd halves of the "disable" process.
   */

  if ((top->flags & RLIM_TNNLOP_FLAG_DISABLE_IN_PROGRESS) == 0)
  {
    top->flags |= RLIM_TNNLOP_FLAG_DISABLE_IN_PROGRESS;

    if ((top->flags & RLIM_TNNLOP_FLAG_IP4_ROUTING) != 0)
    {
      ipMapRtrIntfModeSet(top->tunnelIntIfNum, L7_DISABLE);
    }
#ifdef L7_IPV6_PACKAGE
    if ((top->flags & RLIM_TNNLOP_FLAG_IP6_ROUTING) != 0) 
    {
      ip6MapRtrIntfModeSet(top->tunnelIntIfNum, L7_DISABLE);
    }
    rlimTunnel6to4NhopChangeProcess();
#endif

    rlimNimEventSend(top->tunnelIntIfNum, L7_DOWN);
  }

  /*
   * Second-half of disable processing.
   *
   * If routing is fully off, delete the interface from the
   * stack and hardware.
   */

  if ((top->flags & (RLIM_TNNLOP_FLAG_IP4_ROUTING |
                     RLIM_TNNLOP_FLAG_IP6_ROUTING)) == 0)
  {
    if ((top->flags & RLIM_TNNLOP_FLAG_DTL_INTF_CREATED) != 0)
    {
      top->flags &= ~RLIM_TNNLOP_FLAG_DTL_INTF_CREATED;
      if (dtlTunnelDelete(top->tunnelIntIfNum) != L7_SUCCESS)
      {
        LOG_MSG("dtlTunnelDelete failed for tunnel%u\n", tunnelId);
      }
    }

    if ((top->flags & RLIM_TNNLOP_FLAG_OSAPI_INTF_CREATED) != 0)
    {
      top->flags &= ~RLIM_TNNLOP_FLAG_OSAPI_INTF_CREATED;
      if (osapiTunnelIfDelete(top->tunnelIntIfNum) != L7_SUCCESS)
      {
        LOG_MSG("osapiTunnelIfDelete failed for tunnel%u\n", tunnelId);
      }
    }

    /*
     * Now we're fully down, so we can now allow a re-enable
     */
    top->flags &= ~RLIM_TNNLOP_FLAG_DISABLE_IN_PROGRESS;
  }
}

/*********************************************************************
* @purpose  Determine if the given tunnel should be operationally up
*
* @param    tunnelId  tunnel ID
*
* @returns  L7_TRUE   if the tunnel should be operationally up
* @returns  L7_FALSE  otherwise
*
* @notes    The local address type must be checked on in the operational
*           data as the config data might just indicate that it should
*           be obtained from the interface
*
* @end
*********************************************************************/
L7_BOOL rlimTunnelCanEnable(L7_uint32 tunnelId)
{
  rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

  if ((top->flags & RLIM_TNNLOP_FLAG_DELETE_IN_PROGRESS) != 0)
  {
    return L7_FALSE;
  }

  if ((top->flags & RLIM_TNNLOP_FLAG_DISABLE_IN_PROGRESS) != 0)
  {
    return L7_FALSE;
  }

  switch (tcp->tunnelMode)
  {
    case L7_TUNNEL_MODE_6OVER4:
      if (tcp->remoteAddr.addrType != RLIM_ADDRTYPE_IP4 ||
          tcp->localAddr.addrType == RLIM_ADDRTYPE_UNDEFINED ||
          top->localIp4Addr == L7_NULL_IP_ADDR ||
          (top->flags & RLIM_TNNLOP_FLAG_LOCALIP4UP) == 0)
      {
        return L7_FALSE;
      }
      if ((top->flags & RLIM_TNNLOP_FLAG_REACHABLE) == 0)
      {
        return L7_FALSE;
      }
      break;

    case L7_TUNNEL_MODE_6TO4:
    case L7_TUNNEL_MODE_ISATAP:
      if (tcp->remoteAddr.addrType != RLIM_ADDRTYPE_UNDEFINED ||
          tcp->localAddr.addrType == RLIM_ADDRTYPE_UNDEFINED ||
          top->localIp4Addr == L7_NULL_IP_ADDR ||
          (top->flags & RLIM_TNNLOP_FLAG_LOCALIP4UP) == 0)
      {
        return L7_FALSE;
      }
      break;

    case L7_TUNNEL_MODE_IP6:
      if (tcp->remoteAddr.addrType != RLIM_ADDRTYPE_IP6 ||
          tcp->localAddr.addrType != RLIM_ADDRTYPE_IP6 ||
          L7_IP6_IS_ADDR_UNSPECIFIED(&top->localIp6Addr) ||
          (top->flags & RLIM_TNNLOP_FLAG_LOCALIP6UP) == 0)
      {
        return L7_FALSE;
      }
      if ((top->flags & RLIM_TNNLOP_FLAG_REACHABLE) == 0)
      {
        return L7_FALSE;
      }
      break;

    default:
      return L7_FALSE;
  }

  if (rlimTunnelIsDuplicate(tunnelId) == L7_TRUE)
  {
    return L7_FALSE;
  }

  if ((top->flags & RLIM_TNNLOP_FLAG_PORT_ENABLED) == 0)
  {
    return L7_FALSE;
  }

  return L7_TRUE;
}

/*********************************************************************
 * @purpose  Update the operational state on local address change
 *
* @param    tunnelId  tunnel identifier
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelOpLocalAddrUpdate(L7_uint32 tunnelId)
{
  rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];
  L7_uint32 ip4addr, ip4mask;
  L7_uint32 localIntIfNum = 0;
#if 0 /* not supported yet */ 
  L7_uint32 i;
  L7_ipv6IntfAddr_t ip6AddrList[L7_RTR6_MAX_INTF_ADDRS];
  L7_uint32 numAddrs;
#endif /* not supported yet */

  /*
   * Reset the tunnels attributes to, effectively, "down and unknown".
   */
  top->flags &= ~(RLIM_TNNLOP_FLAG_LOCALIP4UP | RLIM_TNNLOP_FLAG_LOCALIP6UP);
  top->localIp4Addr = L7_NULL_IP_ADDR;
  memset(&top->localIp6Addr, 0, sizeof(top->localIp6Addr));

  /*
   * Initialize the operation addresses of the tunnel and determine
   * the interface associated with the local address.
   */
  switch (tcp->localAddr.addrType)
  {
    case RLIM_ADDRTYPE_UNDEFINED:
      break;

    case RLIM_ADDRTYPE_IP4:
      top->localIp4Addr = tcp->localAddr.un.ip4addr;
      ipMapIpAddressToIntf(top->localIp4Addr, &localIntIfNum);
      break;

#if 0 /* not supported yet */
    case RLIM_ADDRTYPE_IP6:
      top->localIp6Addr = tcp->localAddr.un.ip6addr;
      if (ip6MapIpAddressToRtrIntf(&top->localIp6Addr, &rtrIfNum) == L7_SUCCESS)
      {
        (void)ip6MapRtrIntfToIntIfNum(rtrIfNum, &localIntIfNum);
      }
      break;
#endif /* not supported yet */

    case RLIM_ADDRTYPE_INTERFACE:
      if (nimIntIfFromConfigIDGet(&tcp->localAddr.un.intfConfigId,
                                  &localIntIfNum) != L7_SUCCESS)
      {
        LOG_MSG("invalid config ID\n");
        break;
      }
      if (ipMapRtrIntfIpAddressGet(localIntIfNum, &ip4addr,
                                   &ip4mask) == L7_SUCCESS)
      {
        top->localIp4Addr = ip4addr;
      }
#if 0 /* not supported yet */
      numAddrs = L7_RTR6_MAX_INTF_ADDRS;
      if (ip6MapRtrIntfAddressesGet(localIntIfNum, &numAddrs, ip6AddrList) == L7_SUCCESS)
      {
        for (i = 0; i < numAddrs; i++)
        {
          if (!L7_IP6_IS_ADDR_LINK_LOCAL(&ip6AddrList[i].ip6Addr))
          {
            top->localIp6Addr = ip6AddrList[i].ip6Addr;
            break;
          }
        }
      }
#endif /* not supported yet */
      break;

    default:
      LOG_MSG("invalid addr type (%u)\n", tcp->localAddr.addrType);
      break;
  }

  /*
   * Disallow self-reference for the local/source interface
   */
  if (localIntIfNum == top->tunnelIntIfNum)
  {
    localIntIfNum = 0;
  }

  /*
   * Save the interface associated with the local address.
   */
  top->localIntIfNum = localIntIfNum;

  /*
   * If there is a local address, determine if it is up and save
   * this information in the tunnel operational data.
   */
  if (localIntIfNum != 0)
  {
    L7_uint32 linkState;

    if (nimGetIntfActiveState(localIntIfNum, &linkState) == L7_SUCCESS &&
        linkState == L7_ACTIVE)
    {
      if (top->localIp4Addr != L7_NULL_IP_ADDR)
      {
        if (ipMapIntfIsUp(localIntIfNum))
        {
          top->flags |= RLIM_TNNLOP_FLAG_LOCALIP4UP;
        }
      }
#if 0 /* not supported yet */
      if (!L7_IP6_IS_ADDR_UNSPECIFIED(&top->localIp6Addr))
      {
        if (ip6MapRtrIntfModeGet(localIntIfNum, &mode) == L7_SUCCESS &&
            mode == L7_ENABLE)
        {
          top->flags |= RLIM_TNNLOP_FLAG_LOCALIP6UP;
        }
      }
#endif /* not supported yet */

      /*
       * Preserve the attribute relationships:
       * 1. If an "interface" source address, clear addresses if
       *    the interface is down.
       * 2. Otherwise, clear the associated interface ID, if the
       *    source protocol is not enabled on the interface.
       * Note that these relationships are also maintined in the
       * event handling code below, this is here to set up the
       * initial conditions.
       */
      switch (tcp->localAddr.addrType)
      {
        case RLIM_ADDRTYPE_INTERFACE:
          if ((top->flags & RLIM_TNNLOP_FLAG_LOCALIP4UP) == 0)
          {
            top->localIp4Addr = L7_NULL_IP_ADDR;
          }
          if ((top->flags & RLIM_TNNLOP_FLAG_LOCALIP6UP) == 0)
          {
            memset(&top->localIp6Addr, 0, sizeof(top->localIp6Addr));
          }
          break;

        case RLIM_ADDRTYPE_IP4:
          if ((top->flags & RLIM_TNNLOP_FLAG_LOCALIP4UP) == 0)
          {
            top->localIntIfNum = 0;
          }
          break;

        case RLIM_ADDRTYPE_IP6:
          if ((top->flags & RLIM_TNNLOP_FLAG_LOCALIP6UP) == 0)
          {
            top->localIntIfNum = 0;
          }
          break;

        default:
          LOG_MSG("unexpected address type (%u)\n", tcp->localAddr.addrType);
          break;
      }
    }
  }
}

/*********************************************************************
* @purpose  Update the operational state on local address change
*
* @param    tunnelId  tunnel identifier
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelOpRemoteAddrUpdate(L7_uint32 tunnelId, rlimAddr_t *newAddr)
{
  rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

  /* remove next hop registration */
  if(newAddr){
   switch (tcp->remoteAddr.addrType)
   {
     case RLIM_ADDRTYPE_UNDEFINED:
       break;

     case RLIM_ADDRTYPE_IP4:
       rtoNHResCallbackUnregister(tcp->remoteAddr.un.ip4addr, rlimTunnelIp4NhopChangeCallback, (void *)tunnelId);
       break;

#if 0 /* not yet */
     case RLIM_ADDRTYPE_IP6:
       rto6NHResCallbackUnregister(&tcp->remoteAddr.un.ip6addr, rlimTunnelIp6NhopChangeCallback, (void *)tunnelId);
       break;
#endif

     default:
       LOG_MSG("invalid addr type (%u)\n", tcp->remoteAddr.addrType);
       break;
   }
  }

  top->flags &= ~RLIM_TNNLOP_FLAG_REACHABLE;
  top->nextHopIp4Addr = L7_NULL_IP_ADDR;
  top->nextHopIp4IntIfNum = 0;
  memset(&top->nextHopIp6Addr, 0, sizeof(top->nextHopIp6Addr));
  top->nextHopIp6IntIfNum = 0;

  if(newAddr)
    memcpy(&tcp->remoteAddr, newAddr, sizeof(rlimAddr_t));

  switch (tcp->remoteAddr.addrType)
  {
    case RLIM_ADDRTYPE_UNDEFINED:
      break;

    case RLIM_ADDRTYPE_IP4:
      rlimTunnelIp4ReachabilityCheck(tunnelId);
      rtoNHResCallbackRegister(tcp->remoteAddr.un.ip4addr, rlimTunnelIp4NhopChangeCallback, (void *)tunnelId);
      break;

#if 0 /* not yet */
    case RLIM_ADDRTYPE_IP6:
      rlimTunnelIp6ReachabilityCheck(tunnelId);
      rto6NHResCallbackRegister(&tcp->remoteAddr.un.ip6addr, rlimTunnelIp6NhopChangeCallback, (void *)tunnelId);
      break;
#endif

    default:
      LOG_MSG("invalid addr type (%u)\n", tcp->remoteAddr.addrType);
      break;
  }
}

/*********************************************************************
* @purpose  Get the operational local address type
*
* @param    tunnelId  tunnel identifier
* @param    pAddrType return value for address type
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelOpLocalAddrTypeGet(L7_uint32 tunnelId, L7_uint32 *pAddrType)
{
  rlimAddrType_t type = RLIM_ADDRTYPE_UNDEFINED;
  rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

  switch (tcp->tunnelMode)
  {
    case L7_TUNNEL_MODE_6OVER4:
    case L7_TUNNEL_MODE_6TO4:
    case L7_TUNNEL_MODE_ISATAP:
      if (top->localIp4Addr != L7_NULL_IP_ADDR)
      {
        type = RLIM_ADDRTYPE_IP4;
      }
      break;

    case L7_TUNNEL_MODE_IP6:
      if (!L7_IP6_IS_ADDR_UNSPECIFIED(&top->localIp6Addr))
      {
        type = RLIM_ADDRTYPE_IP6;
      }
      break;

    default:
      break;
  }

  *pAddrType = type;
}

/*********************************************************************
* @purpose  Get the operational local IPv4 address
*
* @param    tunnelId  tunnel identifier
* @param    addr      return value for local address
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimTunnelOpLocalIp4AddrGet(L7_uint32 tunnelId, L7_uint32 *addr)
{
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

  if (top->localIp4Addr == L7_NULL_IP_ADDR)
  {
    return L7_FAILURE;
  }

  *addr = top->localIp4Addr;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the operational local IPv6 address
*
* @param    tunnelId  tunnel identifier
* @param    addr      return value for local address
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimTunnelOpLocalIp6AddrGet(L7_uint32 tunnelId, L7_in6_addr_t *addr)
{
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

  if (L7_IP6_IS_ADDR_UNSPECIFIED(&top->localIp6Addr))
  {
    return L7_FAILURE;
  }

  *addr = top->localIp6Addr;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the operational local interface for a tunnel
*
* @param    tunnelId   tunnel identifier
* @param    pIntIfNum  returns the local interface of a tunnel
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimTunnelOpLocalIntfGet(L7_uint32 tunnelId, L7_uint32 *pIntIfNum)
{
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

  if (top->localIntIfNum == 0)
  {
    return L7_FAILURE;
  }

  *pIntIfNum = top->localIntIfNum;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process an IP4 interface coming operationally up.
*
* @param    eventIntIfNum   internal interface number
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelIp4IntfUpProcess(L7_uint32 eventIntIfNum)
{
  L7_uint32 tunnelId;
  L7_uint32 ip4addr, ip4mask;
  L7_RC_t rc;

  for (tunnelId = 0; tunnelId <= RLIM_MAX_TUNNEL_ID; tunnelId++)
  {
    rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

    switch (tcp->localAddr.addrType)
    {
      case RLIM_ADDRTYPE_UNDEFINED:
        break;

      case RLIM_ADDRTYPE_INTERFACE:
        if (eventIntIfNum == top->localIntIfNum)
        {
          rc = ipMapRtrIntfIpAddressGet(eventIntIfNum, &ip4addr, &ip4mask);
          if (rc == L7_SUCCESS && top->localIp4Addr != ip4addr)
          {
            rlimTunnelForceIp4Up(tunnelId, ip4addr, eventIntIfNum);
          }
        }
        break;

      case RLIM_ADDRTYPE_IP4:
        rc = ipMapRtrIntfCfgIpAddressCheck(eventIntIfNum, top->localIp4Addr);
        if (rc == L7_SUCCESS)
        {
          rlimTunnelForceIp4Up(tunnelId, top->localIp4Addr, eventIntIfNum);
        }
        break;

      case RLIM_ADDRTYPE_IP6:
        /* not interested since this is an IPv4 event... */
        break;

      default:
        LOG_MSG("unexpected address type (%u)\n", tcp->localAddr.addrType);
        break;
    }
  }
}

/*********************************************************************
* @purpose  Process an IP4 interface going operationally down.
*
* @param    eventIntIfNum   internal interface number
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelIp4IntfDownProcess(L7_uint32 eventIntIfNum)
{
  L7_uint32 tunnelId;

  for (tunnelId = 0; tunnelId <= RLIM_MAX_TUNNEL_ID; tunnelId++)
  {
    rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

    switch (tcp->localAddr.addrType)
    {
      case RLIM_ADDRTYPE_UNDEFINED:
        break;

      case RLIM_ADDRTYPE_INTERFACE:
        if (eventIntIfNum == top->localIntIfNum)
        {
          rlimTunnelForceIp4Down(tunnelId);
        }
        break;

      case RLIM_ADDRTYPE_IP4:
        if (eventIntIfNum == top->localIntIfNum)
        {
          rlimTunnelForceIp4Down(tunnelId);
        }
        break;

      case RLIM_ADDRTYPE_IP6:
        /* not interested since this is an IPv4 event... */
        break;

      default:
        LOG_MSG("unexpected address type (%u)\n", tcp->localAddr.addrType);
        break;
    }
  }
}

/*********************************************************************
* @purpose  Process the addition of a secondary IP4 address to an interface.
*
* @param    eventIntIfNum   internal interface number
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelIp4SecondaryAddrAddProcess(L7_uint32 eventIntIfNum)
{
  L7_uint32 tunnelId;
  L7_RC_t rc;

  for (tunnelId = 0; tunnelId <= RLIM_MAX_TUNNEL_ID; tunnelId++)
  {
    rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

    switch (tcp->localAddr.addrType)
    {
      case RLIM_ADDRTYPE_UNDEFINED:
        break;

      case RLIM_ADDRTYPE_INTERFACE:
        /*
         * Only primary addresses are used for interface references,
         * ignore...
         */
        break;

      case RLIM_ADDRTYPE_IP4:
        /*
         * If the local address of a down tunnel matches any of
         * the interfaces address, bring the tunnel up.
         */
        if ((top->flags & RLIM_TNNLOP_FLAG_LOCALIP4UP) == 0)
        {
          rc = ipMapRtrIntfCfgIpAddressCheck(eventIntIfNum, top->localIp4Addr);
          if (rc == L7_SUCCESS)
          {
            rlimTunnelForceIp4Up(tunnelId, top->localIp4Addr, eventIntIfNum);
          }
        }
        break;

      case RLIM_ADDRTYPE_IP6:
        /* not interested since this is an IPv4 event... */
        break;

      default:
        LOG_MSG("unexpected address type (%u)\n", tcp->localAddr.addrType);
        break;
    }
  }
}

/*********************************************************************
* @purpose  Process the deletion of a secondary IP4 address from an interface.
*
* @param    eventIntIfNum   internal interface number
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelIp4SecondaryAddrDeleteProcess(L7_uint32 eventIntIfNum)
{
  L7_uint32 tunnelId;
  L7_RC_t rc;

  for (tunnelId = 0; tunnelId <= RLIM_MAX_TUNNEL_ID; tunnelId++)
  {
    rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

    switch (tcp->localAddr.addrType)
    {
      case RLIM_ADDRTYPE_UNDEFINED:
        break;

      case RLIM_ADDRTYPE_INTERFACE:
        /*
         * Only primary addresses are used for interface references,
         * ignore...
         */
        break;

      case RLIM_ADDRTYPE_IP4:
        /*
         * If the local interface of an active tunnel matches the
         * affected interface, force the tunnel down if the local
         * address is no longer assigned to the interface.
         */
        if ((top->flags & RLIM_TNNLOP_FLAG_LOCALIP4UP) != 0 &&
            top->localIntIfNum == eventIntIfNum)
        {
          rc = ipMapRtrIntfCfgIpAddressCheck(eventIntIfNum, top->localIp4Addr);
          if (rc != L7_SUCCESS)
          {
            rlimTunnelForceIp4Down(tunnelId);
          }
        }
        break;

      case RLIM_ADDRTYPE_IP6:
        /* not interested since this is an IPv4 event... */
        break;

      default:
        LOG_MSG("unexpected address type (%u)\n", tcp->localAddr.addrType);
        break;
    }
  }
}

/*********************************************************************
* @purpose  Process the deletion of an interface.
*
* @param    eventIntIfNum   internal interface number
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelIntfDeleteProcess(L7_uint32 eventIntIfNum)
{
  L7_uint32 tunnelId;
  nimConfigID_t configId;

  if (nimConfigIdGet(eventIntIfNum, &configId) != L7_SUCCESS)
  {
    LOG_MSG("cannot get configId for deleted interface");
    return;
  }

  for (tunnelId = 0; tunnelId <= RLIM_MAX_TUNNEL_ID; tunnelId++)
  {
    rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];

    if (tcp->localAddr.addrType == RLIM_ADDRTYPE_INTERFACE &&
        NIM_CONFIG_ID_IS_EQUAL(&configId, &tcp->localAddr.un.intfConfigId))
    {
      memset(&tcp->localAddr, 0, sizeof(tcp->localAddr));
      tcp->localAddr.addrType = RLIM_ADDRTYPE_UNDEFINED;
      rlimCfgData->cfgHdr.dataChanged = L7_TRUE;

      rlimTunnelOpLocalAddrUpdate(tunnelId);
      rlimTunnelOpUpdate(tunnelId);
    }
  }
}

/*********************************************************************
* @purpose  Bring an IP4 transport tunnel operationally up.
*
* @param    tunnelId
* @param    localIp4Addr   local/source transport address
* @param    localIntIfNum  local/source transport interface
*
* @returns  void
*
* @notes    Maintains the operational state of the tunnel relative
*           to up/down events.  This varies based on tunnel type.
*           For RLIM_ADDRTYPE_INTERFACE tunnels, the associated
*           local/source address should be set, whereas for
*           RLIM_ADDRTYPE_IP4 tunnels the local/source interface
*           should be set.
*
* @end
*********************************************************************/
void rlimTunnelForceIp4Up(L7_uint32 tunnelId, L7_uint32 localIp4Addr,
                          L7_uint32 localIntIfNum)
{
  rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

  switch (tcp->localAddr.addrType)
  {
    case RLIM_ADDRTYPE_INTERFACE:
      top->localIp4Addr = localIp4Addr;
      break;

    case RLIM_ADDRTYPE_IP4:
      top->localIntIfNum = localIntIfNum;
      break;

    default:
      LOG_MSG("invalid addrtype for enabling ip4 on tunnel (%u)",
              tcp->localAddr.addrType);
      break;
  }

  top->flags |= RLIM_TNNLOP_FLAG_LOCALIP4UP;
  rlimTunnelOpUpdate(tunnelId);
}

/*********************************************************************
* @purpose  Bring an IP4 transport tunnel operationally up.
*
* @param    tunnelId
*
* @returns  void
*
* @notes    Maintains the operational state of the tunnel relative
*           to up/down events.  This varies based on tunnel type.
*           For RLIM_ADDRTYPE_INTERFACE tunnels, the associated
*           local/source address should be clearer, whereas for
*           RLIM_ADDRTYPE_IP4 tunnels the local/source interface
*           should be clearer.
*
* @end
*********************************************************************/
void rlimTunnelForceIp4Down(L7_uint32 tunnelId)
{
  rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

  switch (tcp->localAddr.addrType)
  {
    case RLIM_ADDRTYPE_INTERFACE:
      top->localIp4Addr = L7_NULL_IP_ADDR;
      break;

    case RLIM_ADDRTYPE_IP4:
      top->localIntIfNum = 0;
      break;

    default:
      LOG_MSG("invalid addrtype for disabling ip4 on tunnel (%u)",
              tcp->localAddr.addrType);
      break;
  }

  top->flags &= ~RLIM_TNNLOP_FLAG_LOCALIP4UP;
  rlimTunnelOpUpdate(tunnelId);
}

/*********************************************************************
* @purpose  Get the source/destination addresses of a tunnel.
*
* @param    tunnelId   Tunnel ID
* @param    pSrcAddr   Return value for the source address
* @param    pDstAddr   Return value for the destination address
*
* @returns  L7_SUCCESS  If the tunnel has valid src/dst addrs
* @returns  L7_FAILURE  Otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimTunnelAddrsGet(L7_uint32 tunnelId, L7_sockaddr_union_t *pSrcAddr,
                           L7_sockaddr_union_t *pDstAddr)
{
  rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

  switch (tcp->tunnelMode)
  {
    case L7_TUNNEL_MODE_6OVER4:
      if (top->localIp4Addr == L7_NULL_IP_ADDR ||
          tcp->remoteAddr.addrType != RLIM_ADDRTYPE_IP4)
      {
        return L7_FAILURE;
      }

      RLIM_IP4_ADDR_PUT(top->localIp4Addr, pSrcAddr);
      RLIM_IP4_ADDR_PUT(tcp->remoteAddr.un.ip4addr, pDstAddr);
      break;

    case L7_TUNNEL_MODE_6TO4:
    case L7_TUNNEL_MODE_ISATAP:
      if (top->localIp4Addr == L7_NULL_IP_ADDR )
      {
        return L7_FAILURE;
      }

      tcp->remoteAddr.un.ip4addr = 0;
      RLIM_IP4_ADDR_PUT(top->localIp4Addr, pSrcAddr);
      RLIM_IP4_ADDR_PUT(tcp->remoteAddr.un.ip4addr, pDstAddr);
      break;

    case L7_TUNNEL_MODE_IP6:
      if (L7_IP6_IS_ADDR_UNSPECIFIED(&top->localIp6Addr) ||
          tcp->remoteAddr.addrType != RLIM_ADDRTYPE_IP6)
      {
        return L7_FAILURE;
      }

      RLIM_IP6_ADDR_PUT(&top->localIp6Addr, pSrcAddr);
      RLIM_IP6_ADDR_PUT(&tcp->remoteAddr.un.ip6addr, pDstAddr);
      break;

    default:
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the nexthop info associated with a tunnel
*
* @param    tunnelId           Tunnel ID
* @param    pNextHopAddr       Return value for nexthop address
* @param    pNextHopIntIfNum   Return value for nexthop address
*
* @returns  L7_SUCCESS  If the tunnel has valid dst addr
* @returns  L7_FAILURE  Otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimTunnelNextHopGet(L7_uint32 tunnelId,
                             L7_sockaddr_union_t *pNextHopAddr,
                             L7_uint32 *pNextHopIntIfNum)
{
  rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

  switch (tcp->tunnelMode)
  {
    case L7_TUNNEL_MODE_6OVER4:
      if (top->localIp4Addr == L7_NULL_IP_ADDR ||
          tcp->remoteAddr.addrType != RLIM_ADDRTYPE_IP4)
      {
        return L7_FAILURE;
      }

      RLIM_IP4_ADDR_PUT(top->nextHopIp4Addr, pNextHopAddr);
      *pNextHopIntIfNum = top->nextHopIp4IntIfNum;
      break;

    case L7_TUNNEL_MODE_6TO4:
    case L7_TUNNEL_MODE_ISATAP:
      if (top->localIp4Addr == L7_NULL_IP_ADDR ) 
      {
        return L7_FAILURE;
      }
      /* can be multiple next hops */
      RLIM_IP4_ADDR_PUT(L7_NULL_IP_ADDR, pNextHopAddr);
      *pNextHopIntIfNum = top->localIntIfNum;
      break;

    case L7_TUNNEL_MODE_IP6:
      if (L7_IP6_IS_ADDR_UNSPECIFIED(&top->localIp6Addr) ||
          tcp->remoteAddr.addrType != RLIM_ADDRTYPE_IP6)
      {
        return L7_FAILURE;
      }

      RLIM_IP6_ADDR_PUT(&top->nextHopIp6Addr, pNextHopAddr);
      *pNextHopIntIfNum = top->nextHopIp6IntIfNum;
      break;

    default:
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process changes in the IPv4 route table
*
* @param    void
*
* @returns  void
*
* @notes    For each tunnel, check for changes in reachability.
*           Also, look for changes of the transport nexthop so
*           that we send this change to the hardware.
*
* @end
*********************************************************************/
void rlimTunnelIp4NhopChangeProcess(void)
{
  L7_uint32 tunnelId;

  for (tunnelId = 0; tunnelId <= RLIM_MAX_TUNNEL_ID; tunnelId++)
  {
    rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

    if (tcp->remoteAddr.addrType == RLIM_ADDRTYPE_IP4)
    {
      L7_uint32 oldNextHop = top->nextHopIp4Addr;
      L7_uint32 oldIntIfNum = top->nextHopIp4IntIfNum;

      rlimTunnelIp4ReachabilityCheck(tunnelId);

      /*
       * If the tunnel enabled and reachable, do nothing unless
       * the nexthop has changed (in which case inform the hardware).
       *
       * Otherwise, run through the usual check for tunnel updates.
       */
      if ((top->flags & RLIM_TNNLOP_FLAG_ENABLED) != 0 &&
          (top->flags & RLIM_TNNLOP_FLAG_REACHABLE) != 0)
      {
        if (top->nextHopIp4Addr != oldNextHop ||
            top->nextHopIp4IntIfNum != oldIntIfNum)
        {
          L7_sockaddr_union_t nhAddr;
          L7_uint32 nhIntIfNum;

          if (rlimTunnelNextHopGet(tunnelId, &nhAddr, &nhIntIfNum)
              != L7_SUCCESS)
          {
            LOG_MSG("failed to get nexthop info for tunnel%u\n", tunnelId);
            return;
          }

          if (dtlTunnelNextHopSet(top->tunnelIntIfNum, &nhAddr, nhIntIfNum)
              != L7_SUCCESS)
          {
            LOG_MSG("dtl tunnel nexthop set failed\n");
          }
        }
      }
      else
      {
        rlimTunnelOpUpdate(tunnelId);
      }
    }
  }
#ifdef L7_IPV6_PACKAGE
  /* also check 6to4 destinations */
  rlimTunnel6to4NhopChangeProcess();
#endif
}

/*********************************************************************
* @purpose  Check on the reachability of the IPv4 tunnel destination
*
* @param    void
*
* @returns  void
*
* @notes    Also keep track of the nexthop
*
* @end
*********************************************************************/
void rlimTunnelIp4ReachabilityCheck(L7_uint32 tunnelId)
{
  L7_uint32 i;
  rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];
  L7_routeEntry_t route;
  L7_arpEntry_t *pArpEntry;
  L7_BOOL found = L7_FALSE;
  L7_INTF_TYPES_t itype;
  L7_RC_t rc;

  if (rtoBestRouteLookup(tcp->remoteAddr.un.ip4addr, &route, L7_FALSE) == L7_SUCCESS)
  {
    for (i = 0; i < route.ecmpRoutes.numOfRoutes; i++)
    {
      pArpEntry = &route.ecmpRoutes.equalCostPath[i].arpEntry;

      if (nimGetIntfType(pArpEntry->intIfNum, &itype) == L7_SUCCESS)
      {
        if (itype == L7_PHYSICAL_INTF || itype == L7_LOGICAL_VLAN_INTF)
        {
          /*
           * Check if the destination is a local address and
           * do not bring the tunnel up if it is.
           */
          rc = ipMapRtrIntfCfgIpAddressCheck(pArpEntry->intIfNum,
                                             tcp->remoteAddr.un.ip4addr);
          if (rc != L7_SUCCESS)
          {
            /*
             * Locally attached subnet have our local address as the
             * nexthop addr, so we need to change this to the address
             * of the destination.
             */
            if (route.protocol == RTO_LOCAL)
              top->nextHopIp4Addr = tcp->remoteAddr.un.ip4addr;
            else
              top->nextHopIp4Addr = pArpEntry->ipAddr;

            top->nextHopIp4IntIfNum = pArpEntry->intIfNum;
            top->flags |= RLIM_TNNLOP_FLAG_REACHABLE;
            found = L7_TRUE;
            break;
          }
        }
      }
    }
  }

  if (!found)
  {
    top->nextHopIp4Addr = L7_NULL_IP_ADDR;
    top->nextHopIp4IntIfNum = 0;
    top->flags &= ~RLIM_TNNLOP_FLAG_REACHABLE;
  }
}

/*********************************************************************
* @purpose  See the the tunnel is a duplicate
*
* @param    void
*
* @returns  L7_TRUE   if the tunnel is a duplicate
* @returns  L7_FALSE  otherwise
*
* @notes    The compared tunnel should be full configured and, if
*           it has an interface source address, the address should
*           be known.
*
* @end
*********************************************************************/
L7_BOOL rlimTunnelIsDuplicate(L7_uint32 tunnelId1)
{
  rlimTunnelCfgData_t *tcp1 = &rlimCfgData->tunnelCfgData[tunnelId1];
  rlimTunnelOpData_t *top1 = &rlimTunnelOpData[tunnelId1];
  L7_uint32 tunnelId2;
  rlimAddr_t laddr1, laddr2;
  L7_uint32 addrType;
  L7_RC_t rc;

  /*
   * For each tunnel check if the mode, local addr, and remote addr
   * match.  If so we have a duplicate.  This check is complicated
   * by the case where the local address can be a reference to an
   * interface.
   */
  for (tunnelId2 = 0; tunnelId2 <= RLIM_MAX_TUNNEL_ID; tunnelId2++)
  {
    rlimTunnelCfgData_t *tcp2 = &rlimCfgData->tunnelCfgData[tunnelId2];
    rlimTunnelOpData_t *top2 = &rlimTunnelOpData[tunnelId2];

    /*
     * Don't compare against ourself
     */
    if (tunnelId1 == tunnelId2)
    {
      continue;
    }

    /*
     * Duplicates are only relevant if the duplicate is enabled,
     * and the first one enabled wins.
     */
    if ((top2->flags & RLIM_TNNLOP_FLAG_ENABLED) == 0)
    {
      continue;
    }

    if (tcp1->tunnelMode != tcp2->tunnelMode)
    {
      continue;
    }

    if (rlimAddrIsEqual(&tcp1->remoteAddr, &tcp2->remoteAddr) != L7_TRUE)
    {
      continue;
    }

    rlimTunnelOpLocalAddrTypeGet(tunnelId1, &addrType);
    laddr1.addrType = addrType;
    switch (laddr1.addrType)
    {
      case RLIM_ADDRTYPE_IP4:
        rc = rlimTunnelOpLocalIp4AddrGet(tunnelId1, &laddr1.un.ip4addr);
        if (rc != L7_SUCCESS)
        {
          continue;
        }
        break;

      case RLIM_ADDRTYPE_IP6:
        rc = rlimTunnelOpLocalIp6AddrGet(tunnelId1, &laddr1.un.ip6addr);
        if (rc != L7_SUCCESS)
        {
          continue;
        }
        break;

      default:
        LOG_MSG("invalid addr type: %u\n", laddr1.addrType);
        continue;
    }

    rlimTunnelOpLocalAddrTypeGet(tunnelId2, &addrType);
    laddr2.addrType = addrType;
    switch (laddr2.addrType)
    {
      case RLIM_ADDRTYPE_IP4:
        rc = rlimTunnelOpLocalIp4AddrGet(tunnelId2, &laddr2.un.ip4addr);
        if (rc != L7_SUCCESS)
        {
          continue;
        }
        break;

      case RLIM_ADDRTYPE_IP6:
        rc = rlimTunnelOpLocalIp6AddrGet(tunnelId2, &laddr2.un.ip6addr);
        if (rc != L7_SUCCESS)
        {
          continue;
        }
        break;

      default:
        LOG_MSG("invalid addr type: %u\n", laddr2.addrType);
        continue;
    }

    if (rlimAddrIsEqual(&laddr1, &laddr2) != L7_TRUE)
    {
      continue;
    }

    /*
     * Found an enabled tunnel that we're a duplicate off,
     * mark us and them accordingly.
     */
    top1->flags |= RLIM_TNNLOP_FLAG_IS_DUPLICATE;
    top2->flags |= RLIM_TNNLOP_FLAG_HAS_DUPLICATE;
    return L7_TRUE;
  }

  top1->flags &= ~RLIM_TNNLOP_FLAG_IS_DUPLICATE;
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check for any duplicates that can be enabled
*
* @param    voidfNum      internal interface ID of tunnel
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelDuplicateCheck()
{
  L7_uint32 tunnelId;

  for (tunnelId = 0; tunnelId <= RLIM_MAX_TUNNEL_ID; tunnelId++)
  {
    rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

    if ((top->flags & RLIM_TNNLOP_FLAG_IS_DUPLICATE) != 0)
    {
      top->flags &= ~RLIM_TNNLOP_FLAG_IS_DUPLICATE;
      rlimTunnelOpUpdate(tunnelId);
    }
  }
}

/*********************************************************************
* @purpose  Process the disabling of routing on a tunnel
*
* @param    intIfNum      internal interface ID of tunnel
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelPortRoutingDisableProcess(L7_uint32 intIfNum)
{
  L7_uint32 tunnelId;

  if (rlimTunnelIdGetNoLock(intIfNum, &tunnelId) == L7_SUCCESS)
  {
    rlimTunnelOpData_t *top = &rlimTunnelOpData[tunnelId];

    /*
     * Turn off all routing-related flags and then see if we can
     * complete the disabling of the tunnel (and perhaps re-enable
     * it with it's intended new state).
     */
    top->flags &= ~(RLIM_TNNLOP_FLAG_IP4_ROUTING|RLIM_TNNLOP_FLAG_IP6_ROUTING);
    rlimTunnelOpUpdate(tunnelId);
  }
}
