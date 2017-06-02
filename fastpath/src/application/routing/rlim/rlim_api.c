/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename rlim_api.c
*
* @purpose Contains externally callable RLIM entry points.
*
* @component Routing Logical Interface Manager
*
* @comments
*
* @create 02/16/2005
*
* @author eberge
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_rlim_api.h"
#include "l7_cnfgr_api.h"
#include "rlim.h"
#ifdef L7_IPV6_PACKAGE
#include "l7_ospfv3_api.h"
#endif

/* How long RLIM waits between calls to NIM to confirm interface creation.
 * milliseconds. */
#define RLIM_CREATE_SLEEP   10

/* Max time RLIM waits for confirmation from NIM that interface has
 * been created. milliseconds. */
#define RLIM_CREATE_WAIT_MAX  10000


/*********************************************************************
* @purpose  Create a loopback interface
*
* @param    loopbackId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Does not return until RLIM confirms that the interface has
*           reached the L7_INTF_CREATED state.
*
* @end
*********************************************************************/
L7_RC_t rlimLoopbackCreate(L7_uint32 loopbackId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_NIM_QUERY_DATA_t nimQueryData;
  L7_uint32 wait = 0;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_uint32 intIfNum;

  if (loopbackId > RLIM_MAX_LOOPBACK_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "RLIM: invalid loopback interface ID (%u)", loopbackId);
    return L7_FAILURE;
  }

  RLIM_SEMA_TAKE(rlimCreateDeleteMutex, L7_WAIT_FOREVER);

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimLoopbackOpCreate(loopbackId);
  if (rc == L7_SUCCESS)
  {
    RLIM_MASK_SETBIT(rlimCfgData->loopbackMask, loopbackId);
    rlimCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  else if (rc == L7_ALREADY_CONFIGURED)
  {
    /*
     * Allow this, but do not clear existing configuration
     */
    rc = L7_SUCCESS;
  }

  intIfNum = rlimLoopbackOpData[loopbackId].loopbackIntIfNum;

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);

  RLIM_SEMA_GIVE(rlimCreateDeleteMutex);

  /* nimEventIntfNotify() just queues a message to NIM. It may be a while
   * before NIM actually creates the interface and even longer before
   * other components process the interface creation event. Wait here
   * until we confirm the interface is created. If UI proceeds to configure
   * this interface, we need to know that components are already aware of
   * the interface. Have to do this outside the RLIM semaphores, so that
   * components can query RLIM as they process interface events. */
  memset((void *) &nimQueryData, 0, sizeof(nimQueryData));
  nimQueryData.intIfNum = intIfNum;
  nimQueryData.request = L7_NIM_QRY_RQST_STATE;
  do
  {
    if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      /* should never get here */
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RLIM_COMPONENT_ID,
              "nimIntfQuery() failed for intf %s", ifName);
      return L7_FAILURE;
    }

    if (nimQueryData.data.state >= L7_INTF_CREATED)
    {
      return L7_SUCCESS;
    }
    osapiSleepMSec(RLIM_CREATE_SLEEP);
    wait += RLIM_CREATE_SLEEP;
  } while (wait <= RLIM_CREATE_WAIT_MAX);

  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
  L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
          "Interface %s state has not reached L7_INTF_CREATED in %u milliseconds. "
          "State is %d",
          ifName, wait, nimQueryData.data.state);

  return rc;
}

/*********************************************************************
* @purpose  Delete a loopback interface
*
* @param    intIfNum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimLoopbackDelete(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 loopbackId;

  RLIM_SEMA_TAKE(rlimCreateDeleteMutex, L7_WAIT_FOREVER);

  if (rlimLoopbackIdGet(intIfNum, &loopbackId) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid loopback interface ID (%u)", loopbackId);
    rc = L7_FAILURE;
  }
  else
  {
    rlimLoopbackOpDelete(intIfNum, loopbackId);
  }

  if (rc == L7_SUCCESS)
  {
    /*
     * Wait for the operational deletion to complete,
     * then clear out config.
     */
    rlimLoopbackOpDeleteWait(loopbackId);
    RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);
    RLIM_MASK_CLRBIT(rlimCfgData->loopbackMask, loopbackId);
    rlimCfgData->cfgHdr.dataChanged = L7_TRUE;
    RLIM_WRITE_LOCK_GIVE(rlimRWLock);
  }

  RLIM_SEMA_GIVE(rlimCreateDeleteMutex);

  return rc;
}

/*********************************************************************
* @purpose  Convert an internal interace number to a loopbackId
*
* @param    intIfNum     internal interface number
* @param    pLoopbackId  pointer to Loopback ID return value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t rlimLoopbackIdGet(L7_uint32 intIfNum, L7_uint32 *pLoopbackId)
{
  L7_RC_t rc = L7_SUCCESS;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);
  rc = rlimLoopbackIdGetNoLock(intIfNum, pLoopbackId);
  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
* @purpose  Convert a loopbackId to an internal interace number
*
* @param    loopbackId   Loopback ID
* @param    pIntIfNum    return value for internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t rlimLoopbackIntIfNumGet(L7_uint32 loopbackId, L7_uint32 *pIntIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 intIfNum;

  if (loopbackId > RLIM_MAX_LOOPBACK_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid loopback interface ID (%u)", loopbackId);
    return L7_FAILURE;
  }

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  intIfNum = rlimLoopbackOpData[loopbackId].loopbackIntIfNum;
  if (intIfNum == 0)
  {
    rc = L7_FAILURE;
  }
  else
  {
    *pIntIfNum = intIfNum;
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Get the first valid loopback Id.
*
* @param pLoopbackId   return value for the loopbackId
*
* @returns L7_SUCCESS  If able to find a valid loopback
* @returns L7_FAILURE  If no valid loopbacks
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimLoopbackIdFirstGet(L7_uint32 *pLoopbackId)
{
  L7_uint32 loopbackId;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  for (loopbackId = 0; loopbackId <= RLIM_MAX_LOOPBACK_ID; loopbackId++)
  {
    if (rlimLoopbackOpData[loopbackId].loopbackIntIfNum != 0)
    {
      /* found one */
      break;
    }
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  if (loopbackId > RLIM_MAX_LOOPBACK_ID)
  {
    return L7_FAILURE;
  }

  *pLoopbackId = loopbackId;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the next valid loopback Id.
*
* @param loopbackId       loopbackId to start after
* @param pNextLoopbackId  return value for the next loopbackId
*
* @returns L7_SUCCESS  If able to find a succeeding valid loopback
* @returns L7_FAILURE  If no succeeding valid loopbacks
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimLoopbackIdNextGet(L7_uint32 loopbackId, L7_uint32 *pNextLoopbackId)
{
  L7_uint32 loopId;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  for (loopId = loopbackId+1; loopId <= RLIM_MAX_LOOPBACK_ID; loopId++)
  {
    if (rlimLoopbackOpData[loopId].loopbackIntIfNum != 0)
    {
      /* found one */
      break;
    }
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  if (loopId > RLIM_MAX_LOOPBACK_ID)
  {
    return L7_FAILURE;
  }

  *pNextLoopbackId = loopId;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a tunnel interface
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
L7_RC_t rlimTunnelCreate(L7_uint32 tunnelId)
{
  L7_RC_t rc = L7_SUCCESS;

  L7_NIM_QUERY_DATA_t nimQueryData;
  L7_uint32 wait = 0;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_uint32 intIfNum;

  if (tunnelId > RLIM_MAX_TUNNEL_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid tunnel interface ID (%u)", tunnelId);
    return L7_FAILURE;
  }

  RLIM_SEMA_TAKE(rlimCreateDeleteMutex, L7_WAIT_FOREVER);

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelOpCreate(tunnelId);
  if (rc == L7_SUCCESS)
  {
    RLIM_MASK_SETBIT(rlimCfgData->tunnelMask, tunnelId);
    rlimTunnelDefaultConfigSet(&rlimCfgData->tunnelCfgData[tunnelId]);
    rlimCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  else if (rc == L7_ALREADY_CONFIGURED)
  {
    /*
     * Allow this, but do not clear existing configuration
     */
    rc = L7_SUCCESS;
  }

  intIfNum = rlimTunnelOpData[tunnelId].tunnelIntIfNum;

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);

  RLIM_SEMA_GIVE(rlimCreateDeleteMutex);

  /* nimEventIntfNotify() just queues a message to NIM. It may be a while
   * before NIM actually creates the interface and even longer before
   * other components process the interface creation event. Wait here
   * until we confirm the interface is created. If UI proceeds to configure
   * this interface, we need to know that components are already aware of
   * the interface. Have to do this outside the RLIM semaphores, so that
   * components can query RLIM as they process interface events. */
  memset((void *) &nimQueryData, 0, sizeof(nimQueryData));
  nimQueryData.intIfNum = intIfNum;
  nimQueryData.request = L7_NIM_QRY_RQST_STATE;
  do
  {
    if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
    {
      /* should never get here */
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RLIM_COMPONENT_ID,
              "nimIntfQuery() failed for interface %s with intIfNum",
              ifName, intIfNum);
      return L7_FAILURE;
    }

    if (nimQueryData.data.state >= L7_INTF_CREATED)
    {
      return L7_SUCCESS;
    }
    osapiSleepMSec(RLIM_CREATE_SLEEP);
    wait += RLIM_CREATE_SLEEP;
  } while (wait <= RLIM_CREATE_WAIT_MAX);

  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
  L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
          "Interface %s state has not reached L7_INTF_CREATED in %u milliseconds. "
          "State is %d",
          ifName, wait, nimQueryData.data.state);

  return rc;
}

/*********************************************************************
* @purpose  Delete a tunnel interface
*
* @param    intIfNum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimTunnelDelete(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;

  RLIM_SEMA_TAKE(rlimCreateDeleteMutex, L7_WAIT_FOREVER);

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  if (rlimTunnelIdGetNoLock(intIfNum, &tunnelId) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid tunnel interface ID (%u)", tunnelId);
    rc = L7_FAILURE;
  }
  else
  {
    rlimTunnelOpDelete(intIfNum, tunnelId);
  }

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);

  if (rc == L7_SUCCESS)
  {
    /*
     * Wait for the operational deletion to complete,
     * then clear out config.
     */
    rlimTunnelOpDeleteWait(tunnelId);
    RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);
    RLIM_MASK_CLRBIT(rlimCfgData->tunnelMask, tunnelId);
    rlimTunnelDefaultConfigSet(&rlimCfgData->tunnelCfgData[tunnelId]);
    rlimCfgData->cfgHdr.dataChanged = L7_TRUE;
    RLIM_WRITE_LOCK_GIVE(rlimRWLock);
  }

  RLIM_SEMA_GIVE(rlimCreateDeleteMutex);

  return rc;
}

/*********************************************************************
* @purpose  Convert an internal interace number to a tunnelId
*
* @param    intIfNum     internal interface number
* @param    pTunnelId    pointer to Tunnel ID return value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t rlimTunnelIdGet(L7_uint32 intIfNum, L7_uint32 *pTunnelId)
{
  L7_RC_t rc = L7_SUCCESS;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);
  rc = rlimTunnelIdGetNoLock(intIfNum, pTunnelId);
  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
* @purpose  Convert a tunnelId to an internal interace number
*
* @param    tunnelId     Tunnel ID
* @param    pIntIfNum    return value for internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t rlimTunnelIntIfNumGet(L7_uint32 tunnelId, L7_uint32 *pIntIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 intIfNum;

  if (tunnelId > RLIM_MAX_TUNNEL_ID)
  {
    return L7_FAILURE;
  }

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  intIfNum = rlimTunnelOpData[tunnelId].tunnelIntIfNum;
  if (intIfNum == 0)
  {
    rc = L7_FAILURE;
  }
  else
  {
    *pIntIfNum = intIfNum;
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
* @purpose Set the tunnel mode of a tunnel.
*
* @param intIfNum       internal interface identifier
* @param tunnelMode     Tunnel ID of the desired tunnel interface
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelModeSet(L7_uint32 intIfNum, L7_TUNNEL_MODE_t tunnelMode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId, tnnlId;
  rlimTunnelCfgData_t temptcp;
#ifdef L7_IPV6_PACKAGE
  L7_uint32 ospfv3Mode = L7_DISABLE;
#endif

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);
  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc == L7_SUCCESS)
  {
    switch (tunnelMode)
    {
      case L7_TUNNEL_MODE_6TO4:
         /* Multiple 6to4 tunnels in the system not allowed */
        for (tnnlId = 0; tnnlId <= RLIM_MAX_TUNNEL_ID; tnnlId++)
        {
          if (tnnlId == tunnelId)
            continue;

          if(! RLIM_MASK_ISBITSET(rlimCfgData->tunnelMask, tnnlId))
            continue;

          if (rlimCfgData->tunnelCfgData[tnnlId].tunnelMode == L7_TUNNEL_MODE_6TO4)
          {
            rc = L7_ALREADY_CONFIGURED;
            break;
          }
        }
        /* pass through */
      case L7_TUNNEL_MODE_UNDEFINED:
      case L7_TUNNEL_MODE_6OVER4:
         if(tunnelMode == L7_TUNNEL_MODE_6OVER4)
         {
            memcpy(&temptcp,&rlimCfgData->tunnelCfgData[tunnelId], sizeof(temptcp));
            temptcp.tunnelMode = tunnelMode;

            /* don't allow two tunnel configurations to be the same */
            for (tnnlId = 0; tnnlId <= RLIM_MAX_TUNNEL_ID; tnnlId++)
            {
               if (tnnlId == tunnelId)
                  continue;
               if(! RLIM_MASK_ISBITSET(rlimCfgData->tunnelMask, tnnlId))
                  continue;
               if(memcmp(&temptcp, &rlimCfgData->tunnelCfgData[tnnlId],
                         sizeof(rlimTunnelCfgData_t)) == 0)
               {
                  rc = L7_FAILURE;
                  break;
               }
            }
         }
         /* pass through */
      case L7_TUNNEL_MODE_ISATAP:
        if((rc == L7_SUCCESS) &&
           (tunnelMode != rlimCfgData->tunnelCfgData[tunnelId].tunnelMode))
        {
           rlimCfgData->tunnelCfgData[tunnelId].tunnelMode = tunnelMode;
           rlimCfgData->cfgHdr.dataChanged = L7_TRUE;
           rlimTunnelOpUpdate(tunnelId);
        }
        break;

      default:
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
                "invalid tunnel mode (%u)", tunnelMode);
        rc = L7_FAILURE;
    }
  }

#ifdef L7_IPV6_PACKAGE
  if ((rc == L7_SUCCESS) &&
      (ospfv3MapIntfAdminModeGet(intIfNum, &ospfv3Mode) == L7_SUCCESS) &&
      (ospfv3Mode == L7_ENABLE))
  {
    rc = L7_NOT_SUPPORTED;
  }
#endif

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Set the local/source Ip4 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         address
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelLocalIp4AddrSet(L7_uint32 intIfNum, L7_uint32 addr)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId, tnnlId;
  rlimTunnelCfgData_t *tcp;
  rlimTunnelCfgData_t temptcp;

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid tunnel interface (%s)", ifName);
  }
  else if (!RLIM_VALID_IP4_ADDR(addr))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid tunnel source address");
    rc = L7_FAILURE;
  }
  else
  {
     memset(&temptcp, 0, sizeof(temptcp));
     memcpy(&temptcp,&rlimCfgData->tunnelCfgData[tunnelId], sizeof(temptcp));
     temptcp.localAddr.addrType = RLIM_ADDRTYPE_IP4;
     temptcp.localAddr.un.ip4addr = addr;

     /* don't allow two tunnel configurations to be the same */
     for (tnnlId = 0; tnnlId <= RLIM_MAX_TUNNEL_ID; tnnlId++)
     {
        if (tnnlId == tunnelId)
           continue;
        if(! RLIM_MASK_ISBITSET(rlimCfgData->tunnelMask, tnnlId))
           continue;
        if(memcmp(&temptcp, &rlimCfgData->tunnelCfgData[tnnlId],
                  sizeof(rlimTunnelCfgData_t)) == 0)
        {
           rc = L7_FAILURE;
           break;
        }
    }

    tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    if((rc == L7_SUCCESS) &&
       (memcmp(&temptcp, tcp, sizeof(rlimTunnelCfgData_t)) != 0))
    {
       memcpy(tcp, &temptcp, sizeof(rlimTunnelCfgData_t));
       rlimCfgData->cfgHdr.dataChanged = L7_TRUE;
       rlimTunnelOpLocalAddrUpdate(tunnelId);
       rlimTunnelOpUpdate(tunnelId);
    }
  }

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Set the local/source Ip6 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         address
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelLocalIp6AddrSet(L7_uint32 intIfNum, L7_in6_addr_t *addr)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;
  rlimTunnelCfgData_t *tcp;

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid tunnel interface (%s)", ifName);
  }
  else if (!RLIM_VALID_IP6_ADDR(addr))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid tunnel source address");
    rc = L7_FAILURE;
  }
  else
  {
    tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    memset(&tcp->localAddr, 0, sizeof(tcp->localAddr));
    tcp->localAddr.addrType = RLIM_ADDRTYPE_IP6;
    tcp->localAddr.un.ip6addr = *addr;
    rlimCfgData->cfgHdr.dataChanged = L7_TRUE;

    rlimTunnelOpLocalAddrUpdate(tunnelId);
    rlimTunnelOpUpdate(tunnelId);
  }

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Set the interface to get the local address from
*
* @param intIfNum       internal interface identifier of the tunnel
* @param localIntIfNum  internal interface identifier of the interface
*                       to get the local address from
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelLocalIntfSet(L7_uint32 intIfNum, L7_uint32 localIntIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;
  rlimTunnelCfgData_t *tcp;
  nimConfigID_t configId;

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  if (intIfNum == localIntIfNum)
  {
    /* self-reference is not allowed */
    rc = L7_FAILURE;
  }
  else if (rlimTunnelIdGetNoLock(intIfNum, &tunnelId) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid tunnel interface (%s)", ifName);
  }
  else if (nimConfigIdGet(localIntIfNum, &configId) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid tunnel source interface %s", ifName);
    rc = L7_FAILURE;
  }
  else
  {
    tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    memset(&tcp->localAddr, 0, sizeof(tcp->localAddr));
    tcp->localAddr.addrType = RLIM_ADDRTYPE_INTERFACE;
    tcp->localAddr.un.intfConfigId = configId;
    rlimCfgData->cfgHdr.dataChanged = L7_TRUE;

    rlimTunnelOpLocalAddrUpdate(tunnelId);
    rlimTunnelOpUpdate(tunnelId);
  }

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);
  return rc;
}

/*********************************************************************
*
* @purpose Clear the local/source address of a tunnel.
*
* @param intIfNum     internal interface identifier
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelLocalAddrClear(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;
  rlimTunnelCfgData_t *tcp;

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid tunnel interface (%s)", ifName);
  }
  else
  {
    tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    memset(&tcp->localAddr, 0, sizeof(tcp->localAddr));
    tcp->localAddr.addrType = RLIM_ADDRTYPE_UNDEFINED;
    rlimCfgData->cfgHdr.dataChanged = L7_TRUE;

    rlimTunnelOpLocalAddrUpdate(tunnelId);
    rlimTunnelOpUpdate(tunnelId);
  }

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Set the remote/destination Ip4 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         address
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelRemoteIp4AddrSet(L7_uint32 intIfNum, L7_uint32 addr)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId, tnnlId;
  rlimAddr_t newAddr;
  rlimTunnelCfgData_t *tcp;
  rlimTunnelCfgData_t temptcp;

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid tunnel interface (%s)", ifName);
  }
  else if (!RLIM_VALID_IP4_ADDR(addr))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid tunnel source address");
    rc = L7_FAILURE;
  }
  else
  {
     memcpy(&temptcp,&rlimCfgData->tunnelCfgData[tunnelId], sizeof(temptcp));
     temptcp.remoteAddr.addrType = RLIM_ADDRTYPE_IP4;
     temptcp.remoteAddr.un.ip4addr = addr;

     /* don't allow two tunnel configurations to be the same */
     for (tnnlId = 0; tnnlId <= RLIM_MAX_TUNNEL_ID; tnnlId++)
     {
        if (tnnlId == tunnelId)
           continue;
        if(! RLIM_MASK_ISBITSET(rlimCfgData->tunnelMask, tnnlId))
           continue;
        if(memcmp(&temptcp, &rlimCfgData->tunnelCfgData[tnnlId],
                  sizeof(rlimTunnelCfgData_t)) == 0)
        {
           rc = L7_FAILURE;
           break;
        }
    }

    tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    if((rc == L7_SUCCESS) &&
       (memcmp(&temptcp, tcp, sizeof(rlimTunnelCfgData_t)) != 0))
    {
       memset(&newAddr, 0, sizeof(newAddr));
       newAddr.addrType = RLIM_ADDRTYPE_IP4;
       newAddr.un.ip4addr = addr;
       rlimCfgData->cfgHdr.dataChanged = L7_TRUE;
       rlimTunnelOpRemoteAddrUpdate(tunnelId, &newAddr);
       rlimTunnelOpUpdate(tunnelId);
    }
  }

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Set the remote/destination Ip6 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         address
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelRemoteIp6AddrSet(L7_uint32 intIfNum, L7_in6_addr_t *addr)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;
  rlimAddr_t newAddr;

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid tunnel interface (%s)", ifName);
  }
  else if (!RLIM_VALID_IP6_ADDR(addr))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid tunnel source address");
    rc = L7_FAILURE;
  }
  else
  {
    memset(&newAddr, 0, sizeof(newAddr));
    newAddr.addrType = RLIM_ADDRTYPE_IP6;
    newAddr.un.ip6addr = *addr;
    rlimCfgData->cfgHdr.dataChanged = L7_TRUE;

    rlimTunnelOpRemoteAddrUpdate(tunnelId, &newAddr);

    rlimTunnelOpUpdate(tunnelId);
  }

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Clear the remote/destination address of a tunnel.
*
* @param intIfNum     internal interface identifier
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelRemoteAddrClear(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;
  rlimAddr_t newAddr;

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
            "Invalid tunnel interface (%s)", ifName);
  }
  else
  {
    memset(&newAddr, 0, sizeof(rlimAddr_t));
    newAddr.addrType = RLIM_ADDRTYPE_UNDEFINED;
    rlimCfgData->cfgHdr.dataChanged = L7_TRUE;

    rlimTunnelOpRemoteAddrUpdate(tunnelId, &newAddr);

    rlimTunnelOpUpdate(tunnelId);
  }

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Get the tunnel mode of a tunnel.
*
* @param intIfNum       internal interface identifier
* @param pTunnelMode    return value for tunnel mode
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelModeGet(L7_uint32 intIfNum, L7_TUNNEL_MODE_t *pTunnelMode)
{
  L7_RC_t rc;
  L7_uint32 tunnelId;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc == L7_SUCCESS)
  {
    *pTunnelMode = rlimCfgData->tunnelCfgData[tunnelId].tunnelMode;
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Get the active tunnel mode of a tunnel.
*
* @param intIfNum       internal interface identifier
* @param pTunnelMode    return value for tunnel mode
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes this must remain constant during life of tunnel created in stack
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelActiveModeGet(L7_uint32 intIfNum, L7_TUNNEL_MODE_t *pTunnelMode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;
  rlimTunnelOpData_t *top;

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc == L7_SUCCESS)
  {
    top = &rlimTunnelOpData[tunnelId];
    *pTunnelMode = top->tunnelActiveMode;
  }

  return rc;
}

/*********************************************************************
*
* @purpose Get the configured local address type of a tunnel.
*
* @param intIfNum       internal interface identifier
* @param pAddrType      return value for address type
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes Use this interface to see how the tunnel address type is
*        configured.  If, instead, you wish to we the operationally
*        active address type (IP4 or IP6), use rlimTunnelLocalAddrTypeGet.
*        This is relevant in the case where the configured address type
*        is RLIM_ADDRTYPE_INTERFACE in which case this interfaces will
*        return RLIM_ADDRTYPE_INTERFACE, whereas rlimTunnelLocalAddrTypeGet
*        returns RLIM_ADDRTYPE_IP4 or RLIM_ADDRTYPE_IP6 depending on
*        the tunnel mode and valid addresses on the source interface.
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelCfgLocalAddrTypeGet(L7_uint32 intIfNum,
                                      rlimAddrType_t *pAddrType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;
  rlimTunnelCfgData_t *tcp;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc == L7_SUCCESS)
  {
    tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    *pAddrType = tcp->localAddr.addrType;
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Get the type of the local address of a tunnel.
*
* @param intIfNum       internal interface identifier
* @param pAddrType      return value for address type
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes Returns the type of interface used operationally on the
*        tunnel.  This is the same as the configured interface
*        unless the type in RLIM_ADDRTYPE_INTERFACE, in which case
*        the actually protocol (IP4 or IP6) is returned if the
*        address is operationally available.
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelLocalAddrTypeGet(L7_uint32 intIfNum,
                                   rlimAddrType_t *pAddrType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;
  rlimTunnelCfgData_t *tcp;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc == L7_SUCCESS)
  {
    /*
     * If the local address type is RLIM_ADDRTYPE_INTERFACE
     * we need to figure out the address type from the
     * tunnel mode and whether we currently have a valid address
     * of the appropriate type.
     */
    tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    if (tcp->localAddr.addrType == RLIM_ADDRTYPE_INTERFACE)
    {
      rlimTunnelOpLocalAddrTypeGet(tunnelId, pAddrType);
    }
    else
    {
      *pAddrType = tcp->localAddr.addrType;
    }
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Get the type of the remote address of a tunnel.
*
* @param intIfNum       internal interface identifier
* @param pAddrType      return value for address type
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelRemoteAddrTypeGet(L7_uint32 intIfNum,
                                    rlimAddrType_t *pAddrType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc == L7_SUCCESS)
  {
    *pAddrType = rlimCfgData->tunnelCfgData[tunnelId].remoteAddr.addrType;
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Get the local/source Ip4 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         return value for address
*
* @returns L7_SUCCESS  If able to find tunnel and the source address
*                      is an Ip4 address
* @returns L7_FAILURE  Otherwise
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelLocalIp4AddrGet(L7_uint32 intIfNum, L7_uint32 *addr)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;
  rlimTunnelCfgData_t *tcp;
  rlimAddrType_t addrType;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc == L7_SUCCESS)
  {
    /*
     * If the local address type is RLIM_ADDRTYPE_INTERFACE, we
     * need to get the address from the operational data.
     */
    tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    addrType = tcp->localAddr.addrType;
    if (addrType == RLIM_ADDRTYPE_INTERFACE)
    {
      rlimTunnelOpLocalAddrTypeGet(tunnelId, &addrType);
    }

    if (addrType != RLIM_ADDRTYPE_IP4)
    {
      rc = L7_FAILURE;
    }
    else
    {
      rc = rlimTunnelOpLocalIp4AddrGet(tunnelId, addr);
    }
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Get the local/source Ip6 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         return value for address
*
* @returns L7_SUCCESS  If able to find tunnel and the source address
*                      is an Ip6 address
* @returns L7_FAILURE  Otherwise
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelLocalIp6AddrGet(L7_uint32 intIfNum, L7_in6_addr_t *addr)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;
  rlimTunnelCfgData_t *tcp;
  rlimAddrType_t addrType;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc == L7_SUCCESS)
  {
    /*
     * If the local address type is RLIM_ADDRTYPE_INTERFACE, we
     * need to get the address from the operational data.
     */
    tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    addrType = tcp->localAddr.addrType;
    if (addrType == RLIM_ADDRTYPE_INTERFACE)
    {
      rlimTunnelOpLocalAddrTypeGet(tunnelId, &addrType);
    }

    if (tcp->localAddr.addrType != RLIM_ADDRTYPE_IP6)
    {
      rc = L7_FAILURE;
    }
    else
    {
      rc = rlimTunnelOpLocalIp6AddrGet(tunnelId, addr);
    }
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Get the local/source interface of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param pIntIfNum    return value for local interface of the tunnel
*
* @returns L7_SUCCESS  If able to find tunnel and the source address
*                      is specified by a valid interface
* @returns L7_FAILURE  Otherwise
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelLocalIntfGet(L7_uint32 intIfNum, L7_uint32 *pIntIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;
  rlimTunnelCfgData_t *tcp;
  rlimAddrType_t addrType;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc == L7_SUCCESS)
  {
    tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    addrType = tcp->localAddr.addrType;
    if (tcp->localAddr.addrType != RLIM_ADDRTYPE_INTERFACE)
    {
      rc = L7_FAILURE;
    }
    else
    {
      rc = rlimTunnelOpLocalIntfGet(tunnelId, pIntIfNum);
    }
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Get the remote/destination Ip4 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         return value for address
*
* @returns L7_SUCCESS  If able to find tunnel and the remote address
*                      is an Ip4 address
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelRemoteIp4AddrGet(L7_uint32 intIfNum, L7_uint32 *addr)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;
  rlimTunnelCfgData_t *tcp;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc == L7_SUCCESS)
  {
    tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    if (tcp->remoteAddr.addrType != RLIM_ADDRTYPE_IP4)
    {
      rc = L7_FAILURE;
    }
    else
    {
      *addr = tcp->remoteAddr.un.ip4addr;
    }
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Get the remote/destination Ip6 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         return value for address
*
* @returns L7_SUCCESS  If able to find tunnel and the remote address
*                      is an Ip6 address
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelRemoteIp6AddrGet(L7_uint32 intIfNum, L7_in6_addr_t *addr)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tunnelId;
  rlimTunnelCfgData_t *tcp;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  rc = rlimTunnelIdGetNoLock(intIfNum, &tunnelId);
  if (rc == L7_SUCCESS)
  {
    tcp = &rlimCfgData->tunnelCfgData[tunnelId];
    if (tcp->remoteAddr.addrType != RLIM_ADDRTYPE_IP6)
    {
      rc = L7_FAILURE;
    }
    else
    {
      *addr = tcp->remoteAddr.un.ip6addr;
    }
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  return rc;
}

/*********************************************************************
*
* @purpose Get the hop limit for the given tunnel
*
* @param intIfNum       internal interface identifier
* @param pHopLimit      return value for the hopLimit
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelHopLimitGet(L7_uint32 intIfNum, L7_uint32 *pHopLimit)
{
  *pHopLimit = 0;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the security value for the given tunnel
*
* @param intIfNum       internal interface identifier
* @param pSecurity      return value for the security value
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelSecurityGet(L7_uint32 intIfNum, L7_uint32 *pSecurity)
{
  *pSecurity = RLIM_SECURITY_NONE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the TOS value for the given tunnel
*
* @param intIfNum       internal interface identifier
* @param pTOS           return value for the TOS value
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelTOSGet(L7_uint32 intIfNum, L7_int32 *pTOS)
{
  *pTOS = 0;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the flow label for the given tunnel
*
* @param intIfNum       internal interface identifier
* @param pFlowLabel     return value for the flow label
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelFlowLabelGet(L7_uint32 intIfNum, L7_int32 *pFlowLabel)
{
  *pFlowLabel = 0;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the encapsulation limit for the given tunnel
*
* @param intIfNum       internal interface identifier
* @param pEncapsLimit   return value for the encapsulation limit
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelEncapsLimitGet(L7_uint32 intIfNum, L7_int32 *pEncapsLimit)
{
  *pEncapsLimit = -1;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the first valid tunnel Id.
*
* @param pTunnelId      return value for the tunnelId
*
* @returns L7_SUCCESS  If able to find a valid tunnel
* @returns L7_FAILURE  If no valid tunnels
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelIdFirstGet(L7_uint32 *pTunnelId)
{
  L7_uint32 tunnelId;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  for (tunnelId = 0; tunnelId <= RLIM_MAX_TUNNEL_ID; tunnelId++)
  {
    if (rlimTunnelOpData[tunnelId].tunnelIntIfNum != 0)
    {
      /* found one */
      break;
    }
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  if (tunnelId > RLIM_MAX_TUNNEL_ID)
  {
    return L7_FAILURE;
  }

  *pTunnelId = tunnelId;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the next valid tunnel Id.
*
* @param tunnelId       tunnelId to start after
* @param pNextTunnelId  return value for the next tunnelId
*
* @returns L7_SUCCESS  If able to find a succeeding valid tunnel
* @returns L7_FAILURE  If no succeeding valid tunnels
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t rlimTunnelIdNextGet(L7_uint32 tunnelId, L7_uint32 *pNextTunnelId)
{
  L7_uint32 tnnlId;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  for (tnnlId = tunnelId+1; tnnlId <= RLIM_MAX_TUNNEL_ID; tnnlId++)
  {
    if (rlimTunnelOpData[tnnlId].tunnelIntIfNum != 0)
    {
      /* found one */
      break;
    }
  }

  RLIM_READ_LOCK_GIVE(rlimRWLock);

  if (tnnlId > RLIM_MAX_TUNNEL_ID)
  {
    return L7_FAILURE;
  }

  *pNextTunnelId = tnnlId;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  check the given address is a loopback interface address.
*
*
* @param  rpAddr    @b{(input)}rp address
*
* @returns   L7_SUCCESS, if rpAddr is not loopback address
* @returns   L7_FAILURE, else case.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t rlimIsAddressLoopback(L7_uint32 rpAddr)
{
  L7_uint32 loopbackId;
  L7_uint32 nextLoopbackId;
  L7_uint32 intIfNum, nextInterface;
  L7_IP_ADDR_t ipAddr;
  L7_IP_MASK_t mask;

  if (rlimLoopbackIdFirstGet(&loopbackId) != L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  if (rlimLoopbackIntIfNumGet(loopbackId, &intIfNum) != L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  while(intIfNum)
  {
    if (ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &mask) == L7_SUCCESS)
    {
      if (rpAddr == ipAddr)
      {
        return L7_FAILURE;
      }
    }
    
    if (rlimLoopbackIdNextGet(loopbackId, &nextLoopbackId) == L7_SUCCESS)
    {
      if (rlimLoopbackIntIfNumGet(nextLoopbackId, &nextInterface) == L7_SUCCESS)
      {
        intIfNum = nextInterface;
        loopbackId = nextLoopbackId;
      }
      else
      {
        intIfNum = 0;
      }
    }
    else
    {
      break;
    }
  }
  return L7_SUCCESS;
}

