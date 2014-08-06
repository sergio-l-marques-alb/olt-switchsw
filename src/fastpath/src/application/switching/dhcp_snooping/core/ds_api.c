/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  ds_api.c
*
* @purpose   DHCP snooping APIs
*
* @component DHCP snooping
*
* @comments Replaces the DHCP filtering component. 
*           See functional spec "DHCP Snooping and IP Source Guard FS"
*
*           DHCP Snooping is a security feature that monitors traffic between a 
*           DHCP client and DHCP server to build a bindings database of 
*           {MAC address, IP address, port} tuples that are considered authorized. 
*           The network administrator enables DHCP snooping globally and on specific 
*           VLANs, and configures ports within the VLAN to be trusted or untrusted.  
*           DHCP servers must be reached through trusted ports. DHCP snooping 
*           enforces the following security rules:
*
*           - DHCP packets from a DHCP server (DHCPOFFER, DHCPACK, DHCPNAK, 
*             DHCPRELEASEQUERY) are dropped if received on an untrusted port.
*
*           - DHCPRELEASE and DHCPDECLINE messages are dropped if for a MAC address 
*             in the snooping database, but the binding's interface is other than 
*             the interface where the message was received.
*
*           - On untrusted interfaces, the switch drops DHCP packets whose source MAC 
*             address is not the same as the DHCP client hardware address in the 
*             DHCP header.
*
*           IP Source Guard and Dynamic ARP Inspection use the DHCP snooping 
*           bindings database. When IP Source Guard is enabled, the switch drops 
*           incoming packets whose source IP address does not match an IP address 
*           in the bindings database. IPSG can optionally filter on source MAC
*           address. Dynamic ARP Inspection uses the bindings database to validate 
*           ARP packets.
*
*
* @create 3/15/2007
*
* @author Rob Rice (rrice)
*
* @end
*             
**********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "log.h"

#include "ds_cfg.h"
#include "ds_util.h"
#include "dhcp_snooping_api.h"
#include "dhcp_snooping_exports.h"

extern dsCfgData_t *dsCfgData;
extern osapiRWLock_t dsCfgRWLock;
extern dsIntfInfo_t *dsIntfInfo;

static L7_uchar8 nullMacAddr[L7_ENET_MAC_ADDR_LEN] = {0,0,0,0,0,0};



/*********************************************************************
* @purpose  Gets the DHCP Snooping Admin mode
*
* @param    adminMode   @b((output)) DHCP snooping admin mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsAdminModeGet(L7_uint32 *adminMode)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *adminMode = dsCfgData->dsGlobalAdminMode;

  osapiReadLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the DHCP Snooping admin mode
*
* @param    adminMode   @b((input)) DHCP snooping admin mode
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsAdminModeSet(L7_uint32 adminMode)
{
  if ((adminMode != L7_ENABLE) && (adminMode != L7_DISABLE))
  {
    return L7_FAILURE;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (adminMode != dsCfgData->dsGlobalAdminMode)
  {
    dsCfgData->dsGlobalAdminMode = adminMode;
    dsAdminModeApply(adminMode);
    dsConfigDataChange();
  }
  osapiWriteLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}

#ifdef L7_DHCP_L2_RELAY_PACKAGE
/*********************************************************************
* @purpose  Gets the DHCP L2 Relay Admin mode
*
* @param    adminMode   @b((output)) DHCP snooping admin mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayAdminModeGet(L7_uint32 *adminMode)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *adminMode = dsCfgData->dsL2RelayAdminMode;

  osapiReadLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay admin mode
*
* @param    adminMode   @b((input)) DHCP snooping admin mode
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayAdminModeSet(L7_uint32 adminMode)
{
  if ((adminMode != L7_ENABLE) && (adminMode != L7_DISABLE))
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to set L2relay admin mode as of "
                    "invalid input mode %d", adminMode);
      dsTraceWrite(traceMsg);
    }
    return L7_FAILURE;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (adminMode != dsCfgData->dsL2RelayAdminMode)
  {
    dsCfgData->dsL2RelayAdminMode = adminMode;
    dsL2RelayAdminModeApply(adminMode);
    dsConfigDataChange();
  }
  osapiWriteLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Gets the DHCP L2 Relay interface mode
*
* @param    intIfNum    @b((input)) internal interface number
* @param    intfMode    @b((output)) DHCP L2 Relay interface mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayIntfModeGet(L7_uint32 intIfNum, L7_uint32 *intfMode)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *intfMode = _dsIntfL2RelayGet(intIfNum);
  
  osapiReadLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay interface mode
*
* @param    intIfNum    @b((input)) internal interface number
* @param    intfMode    @b((input)) DHCP L2 Relay interface mode
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayIntfModeSet(L7_uint32 intIfNum, L7_uint32 intfMode)
{
  if ((intfMode != L7_ENABLE) && (intfMode != L7_DISABLE))
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to set L2relay interface mode as of "
                    "invalid input mode %d", intfMode);
      dsTraceWrite(traceMsg);
    }
    return L7_FAILURE;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (_dsIntfL2RelayGet(intIfNum) != intfMode)
  {
    if (intfMode)
      _dsIntfL2RelayEnable(intIfNum);
    else
      _dsIntfL2RelayDisable(intIfNum);
    dsL2RelayIntfModeApply(intIfNum, intfMode);

    dsConfigDataChange();
  }

  osapiWriteLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get API for whether DHCP L@ Relay considers a port trusted.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((output)) L7_TRUE if port is trusted
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayIntfTrustGet(L7_uint32 intIfNum, L7_BOOL *trust)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *trust = _dsIntfL2RelayTrustGet(intIfNum);

  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set API for whether DHCP L2 Relay considers a port trusted.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((input))  L7_TRUE if port is trusted
*
* @returns  L7_SUCCESS, if mode was set
*           L7_NOT_SUPPORTED, if DHCP L2 Relay is not supported on this 
*                             type of interface
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayIntfTrustSet(L7_uint32 intIfNum, L7_BOOL trust)
{
  if (!dsIntfIsValid(intIfNum))
  {
    return L7_NOT_SUPPORTED;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  if (_dsIntfL2RelayTrustGet(intIfNum) != trust)
  {
    if (trust)
      _dsIntfL2RelayTrust(intIfNum);
    else
      _dsIntfL2RelayDistrust(intIfNum);
    dsConfigDataChange();
  }

  osapiWriteLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the DHCP L2 Relay for a VLAN
*
* @param    vlanNum       @b((input)) VLAN number
* @param    l2RelayMode   @b((output)) DHCP L2 Relay  mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayVlanModeGet(L7_uint32 vlanNum, L7_uint32 *l2RelayMode)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *l2RelayMode = _dsVlanL2RelayGet(vlanNum);
  
  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay mode for a vlan range
*
* @param    vlanStart       @b((input)) Starting Vlan number
* @param    vlanEnd         @b((input)) Ending Vlan number
* @param    l2RelayMode        @b((input)) L2 DHCP Relay Agent mode

*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayVlanModeSet(L7_uint32 vlanStart, L7_uint32 vlanEnd,
                             L7_uint32 l2RelayMode)
{
  L7_uint32 index;

  if ((l2RelayMode != L7_ENABLE) && (l2RelayMode != L7_DISABLE))
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to set L2relay vlan mode as of "
                    "invalid input mode %d", l2RelayMode);
      dsTraceWrite(traceMsg);
    }
    return L7_FAILURE;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  for (index = vlanStart; index <= vlanEnd; index++ )
  {
    if (_dsVlanL2RelayGet(index) != l2RelayMode)
    {
      if (l2RelayMode)
        _dsVlanL2RelayEnable(index);
      else
        _dsVlanL2RelayDisable(index);
      dsConfigDataChange();
    }
  }

  osapiWriteLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the DHCP L2 Relay circuit-id mode for a vlan
*
* @param    vlanNum        @b((input))  VLAN number
* @param    circuitIdMode  @b((output)) DHCP L2 Relay Circuit-Id mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayCircuitIdGet(L7_uint32 vlanNum, L7_uint32 *circuitIdMode)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *circuitIdMode = _dsVlanL2RelayCircuitIdGet(vlanNum);
  
  osapiReadLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay  circuit-Id mode for a vlan range
*
* @param    vlanStart       @b((input)) Starting Vlan number
* @param    vlanEnd         @b((input)) Ending Vlan number
* @param    circuitIdMode   @b((input)) L2 DHCP Relay Agent mode

*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayCircuitIdSet(L7_uint32 vlanStart, L7_uint32 vlanEnd,
                              L7_uint32 circuitId)
{
  L7_uint32 index;

  if ((circuitId != L7_ENABLE) && (circuitId != L7_DISABLE))
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to set circuit-id vlan mode as of "
                    "invalid input mode %d", circuitId);
      dsTraceWrite(traceMsg);
    }
    return L7_FAILURE;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  for (index = vlanStart; index <= vlanEnd; index++ )
  {
    if (_dsVlanL2RelayCircuitIdGet(index) != circuitId)
    {
      if (circuitId)
        _dsVlanL2RelayCircuitIdEnable(index);
      else
        _dsVlanL2RelayCircuitIdDisable(index);
      dsConfigDataChange();
    }
  }

  osapiWriteLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the DHCP L2 Relay remote-id string for a vlan
*
* @param    vlanNum   @b((input))  VLAN number
* @param    remoteId  @b((output)) DHCP L2 Relay remote Id string
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayRemoteIdGet(L7_uint32 vlanNum, L7_uchar8 *remoteId)
{
  if (remoteId == L7_NULLPTR)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to get L2relay remote-id as of "
                    "null input string");
      dsTraceWrite(traceMsg);
    }
    return L7_FAILURE;
  }
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  osapiStrncpy(remoteId, dsCfgData->dsL2RelayRemoteId[vlanNum],
               sizeof(dsCfgData->dsL2RelayRemoteId[vlanNum]));
    
  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay remote-Id string for a vlan range
*
* @param    vlanStart    @b((input)) Starting Vlan number
* @param    vlanEnd      @b((input)) Ending Vlan number
* @param    remoteId     @b((input)) L2 DHCP Relay Agent remote Id string

*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    The remote-id is expected to be string with null character
*           when disabling, but it should not Null pointer.
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayRemoteIdSet(L7_uint32 vlanStart, L7_uint32 vlanEnd,
                             L7_uchar8 *remoteId)
{
  L7_uint32 index;

  if (remoteId == L7_NULLPTR)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to set L2relay remote-id as of "
                    "null input string");
      dsTraceWrite(traceMsg);
    }
    return L7_FAILURE;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  for (index = vlanStart; index <= vlanEnd; index++ )
  {
    /* The length that needs to be compared has to be revised.
       Its the max length of two strings. */
    if (osapiStrncmp(remoteId, dsCfgData->dsL2RelayRemoteId[index],
               sizeof(dsCfgData->dsL2RelayRemoteId[index])) != L7_NULL) 
    {
      /* disabling of remote-id is represented by NULL string.*/
      osapiStrncpy(dsCfgData->dsL2RelayRemoteId[index], remoteId,
               sizeof(dsCfgData->dsL2RelayRemoteId[index]));
      dsConfigDataChange();
    }
  }

  osapiWriteLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
/*********************************************************************
* @purpose  Gets the DHCP L2 Relay for a given subscription on an interface.
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subsripion name
* @param    l2RelayMode      @b((output)) DHCP L2 Relay  mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelaySubscripionModeGet(L7_uint32 intIfNum,
                                        L7_uchar8 *subscriptionName,
                                        L7_uint32 *l2RelayMode)
{
  dsL2RelaySubscriptionCfg_t *subsCfg = L7_NULLPTR;
  if (intIfNum == L7_NULL || subscriptionName == L7_NULLPTR ||
      l2RelayMode == L7_NULLPTR)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to get L2relay mode as of "
                    "invalid input parameters");
      dsTraceWrite(traceMsg);
    }
    return L7_FAILURE;
  }
  if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                  "%s-%d :Entered with intIfNum = %d , subscName = %s",
                  __FUNCTION__, __LINE__, intIfNum, subscriptionName);
    dsTraceWrite(traceMsg);
  }

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  /* Get the subscription entry on the given interface. */
  if (dsL2RelaySubscriptionEntryGet(intIfNum, subscriptionName, L7_TRUE,
                                    &subsCfg) != L7_SUCCESS)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to get L2relay subscription entry for (intIfNum, suscription) ="
                    "(%d, %s)", intIfNum, subscriptionName);
      dsTraceWrite(traceMsg);
    }
    osapiReadLockGive(dsCfgRWLock);
    return L7_NOT_EXIST;
  }

  *l2RelayMode = subsCfg->l2relay;
  
  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay for a given subscription on an interface.
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subsripion name
* @param    l2RelayMode      @b((input)) DHCP L2 Relay  mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelaySubscripionModeSet(L7_uint32 intIfNum,
                                    L7_uchar8 *subscriptionName,
                                    L7_uint32 l2RelayMode)
{
  dsL2RelaySubscriptionCfg_t *subsCfg = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (intIfNum == L7_NULL || subscriptionName == L7_NULLPTR)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to set L2relay mode as of "
                    "invalid input parameters");
      dsTraceWrite(traceMsg);
    }
    return L7_FAILURE;
  }

  if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                  "Entered with intIfNum = %d , subscName = %s",
                  intIfNum, subscriptionName);
    dsTraceWrite(traceMsg);
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  /* Get the  slot subscription entry on the given interface. */
  rc = dsL2RelaySubscriptionEntryGet(intIfNum, subscriptionName, L7_FALSE,
                                    &subsCfg);
  if (rc == L7_SUCCESS)
  {
    osapiStrncpy(subsCfg->subscriptionName, subscriptionName, 
                 L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
    subsCfg->l2relay = l2RelayMode;
    dsL2RelaySubscriptionEntryFree(subsCfg);
  }
  else
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to get L2relay subscription entry for (intIfNum, suscription) ="
                    "(%d, %s)", intIfNum, subscriptionName);
      dsTraceWrite(traceMsg);
    }
  }

  osapiWriteLockGive(dsCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  Gets the DHCP L2 Relay Circuit-id mode for a
*           given subscription on an interface.
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subsripion name
* @param    cIdMode          @b((output)) DHCP L2 Relay circuit-id mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelaySubscripionCircuitIdModeGet(L7_uint32 intIfNum,
                                        L7_uchar8 *subscriptionName,
                                        L7_uint32 *cIdMode)
{
  dsL2RelaySubscriptionCfg_t *subsCfg = L7_NULLPTR;
  if (intIfNum == L7_NULL || subscriptionName == L7_NULLPTR ||
      cIdMode == L7_NULLPTR)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to get L2relay circuit-id mode as of "
                    "invalid input parameters");
      dsTraceWrite(traceMsg);
    }
    return L7_FAILURE;
  }
  if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                  "%s-%d :Entered with intIfNum = %d , subscName = %s",
                  __FUNCTION__, __LINE__,intIfNum, subscriptionName);
    dsTraceWrite(traceMsg);
  }

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  /* Get the subscription entry on the given interface. */
  if (dsL2RelaySubscriptionEntryGet(intIfNum, subscriptionName, L7_TRUE,
                                    &subsCfg) != L7_SUCCESS)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to get L2relay subscription entry for (intIfNum, suscription) ="
                    "(%d, %s)", intIfNum, subscriptionName);
      dsTraceWrite(traceMsg);
    }
    osapiReadLockGive(dsCfgRWLock);
    return L7_NOT_EXIST;
  }

  *cIdMode = subsCfg->circuitId;
  
  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay Circuit-id mode for a
*           given subscription on an interface.
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subsripion name
* @param    cIdMode          @b((input)) DHCP L2 Relay circuit-id mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelaySubscripionCircuitIdModeSet(L7_uint32 intIfNum,
                                    L7_uchar8 *subscriptionName,
                                    L7_uint32 cIdMode)
{
  dsL2RelaySubscriptionCfg_t *subsCfg = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (intIfNum == L7_NULL || subscriptionName == L7_NULLPTR)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to set L2relay circuit-id mode as of "
                    "invalid input parameters");
      dsTraceWrite(traceMsg);
    }
    return L7_FAILURE;
  }

  if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                  "Entered with intIfNum = %d , subscName = %s",
                  intIfNum, subscriptionName);
    dsTraceWrite(traceMsg);
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;


  /* Get the  slot subscription entry on the given interface. */
  rc = dsL2RelaySubscriptionEntryGet(intIfNum, subscriptionName, L7_FALSE,
                                    &subsCfg);
  if (rc == L7_SUCCESS)
  {
    osapiStrncpy(subsCfg->subscriptionName, subscriptionName, 
                 L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
    subsCfg->circuitId = cIdMode;
    dsL2RelaySubscriptionEntryFree(subsCfg);
  }
  else  
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to get L2relay subscription entry for (intIfNum, suscription) ="
                    "(%d, %s)", intIfNum, subscriptionName);
      dsTraceWrite(traceMsg);
    }
  }

  osapiWriteLockGive(dsCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  Gets the DHCP L2 Relay Remote-id string for a
*           given subscription on an interface.
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subsripion name
* @param    rIdStr           @b((output)) DHCP L2 Relay remote-id string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelaySubscripionRemoteIdModeGet(L7_uint32 intIfNum,
                                        L7_uchar8 *subscriptionName,
                                        L7_uchar8 *rIdStr)
{
  dsL2RelaySubscriptionCfg_t *subsCfg = L7_NULLPTR;
  if (intIfNum == L7_NULL || subscriptionName == L7_NULLPTR ||
      rIdStr == L7_NULLPTR)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to get L2relay remote-id mode as of "
                    "invalid input parameters");
      dsTraceWrite(traceMsg);
    }
    return L7_FAILURE;
  }
  if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                  "%s-%d :Entered with intIfNum = %d , subscName = %s",
                  __FUNCTION__, __LINE__, intIfNum, subscriptionName);
    dsTraceWrite(traceMsg);
  }

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  /* Get the subscription entry on the given interface. */
  if (dsL2RelaySubscriptionEntryGet(intIfNum, subscriptionName, L7_TRUE,
                                    &subsCfg) != L7_SUCCESS)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to get L2relay subscription entry for (intIfNum, suscription) ="
                    "(%d, %s)", intIfNum, subscriptionName);
      dsTraceWrite(traceMsg);
    }
    osapiReadLockGive(dsCfgRWLock);
    return L7_NOT_EXIST;
  }

  osapiStrncpy(rIdStr, subsCfg->remoteId, DS_MAX_REMOTE_ID_STRING);
  
  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay Remote-id string for a
*           given subscription on an interface.
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subsripion name
* @param    rIdstr           @b((input)) DHCP L2 Relay remote-id string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelaySubscripionRemoteIdModeSet(L7_uint32 intIfNum,
                                    L7_uchar8 *subscriptionName,
                                    L7_uchar8 *rIdStr)
{
  dsL2RelaySubscriptionCfg_t *subsCfg = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (intIfNum == L7_NULL || subscriptionName == L7_NULLPTR ||
      rIdStr == L7_NULLPTR)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to set L2relay remote-id mode as of "
                    "invalid input parameters");
      dsTraceWrite(traceMsg);
    }
    return L7_FAILURE;
  }

  if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                  "Entered with intIfNum = %d , subscName = %s",
                  intIfNum, subscriptionName);
    dsTraceWrite(traceMsg);
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  /* Get the  slot subscription entry on the given interface. */
  rc = dsL2RelaySubscriptionEntryGet(intIfNum, subscriptionName, L7_FALSE,
                                    &subsCfg);
  if (rc == L7_SUCCESS)
  {
    osapiStrncpy(subsCfg->subscriptionName, subscriptionName, 
                 L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
    osapiStrncpy(subsCfg->remoteId, rIdStr, DS_MAX_REMOTE_ID_STRING);
    dsL2RelaySubscriptionEntryFree(subsCfg);
  }
  else
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to get L2relay subscription entry for (intIfNum, suscription) ="
                    "(%d, %s)", intIfNum, subscriptionName);
      dsTraceWrite(traceMsg);
    }
  }

  osapiWriteLockGive(dsCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  Get the next DHCP L2 Relay Subscription entry on an interface
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subscription name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayIntfSubscriptionGet(L7_uint32 intIfNum, L7_uchar8 *subscriptionName)
{
  dsL2RelaySubscriptionCfg_t *subsCfgTemp = L7_NULLPTR;

  if (intIfNum == L7_NULL || subscriptionName == L7_NULLPTR )
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to get next L2relay subscription entry as of "
                    "invalid input parameters");
      dsTraceWrite(traceMsg);
    }
    return L7_FAILURE;
  }

  if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                  "Given params to get L2relay subscription entry : intIfNum (%d) ,  "
                  "subscriptionName (%s)", intIfNum, subscriptionName);
    dsTraceWrite(traceMsg);
  }
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  /* Get the  slot subscription entry on the given interface. */
  if (dsL2RelaySubscriptionEntryGet(intIfNum, subscriptionName, L7_TRUE,
                                    &subsCfgTemp) != L7_SUCCESS)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to get L2relay subscription entry for (intIfNum, suscription) ="
                    "(%d, %s)", intIfNum, subscriptionName);
      dsTraceWrite(traceMsg);
    }
    osapiReadLockGive(dsCfgRWLock);
    return L7_FAILURE;
  }

  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the next DHCP L2 Relay Subscription entry on an interface
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subscription name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayIntfSubscriptionNextGet(L7_uint32 *intIfNum,
                                           L7_uchar8 *subscriptionName)
{
  dsL2RelaySubscriptionCfg_t *subsCfgTemp = L7_NULLPTR, subsCfgFound;
  L7_uint32 subsIndex = 0, intIfNumTemp = L7_NULL;
  L7_uchar8 eosOfStr = L7_EOS, subscTemp[L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX+1];
  L7_BOOL foundEntry = L7_FALSE;
  L7_uint32 intIfNumFound = L7_NULL;

  if (intIfNum == L7_NULL || subscriptionName == L7_NULLPTR)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                    "Failed to get next L2relay subscription entry as of "
                    "invalid input parameters");
      dsTraceWrite(traceMsg);
    }
    return L7_FAILURE;
  }

  intIfNumTemp = *intIfNum;
  intIfNumFound = intIfNumTemp;
  if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, 
                  "Given params get next L2relay subscription entry : intIfNum (%d) ,  "
                  "subscriptionName (%s)", *intIfNum, subscriptionName);
    dsTraceWrite(traceMsg);
  }

  memset(&subsCfgFound,0, sizeof(dsL2RelaySubscriptionCfg_t));
  osapiStrncpy(subscTemp, subscriptionName, L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  if (*intIfNum == L7_NULL)
  {
    intIfNumTemp = 1; /* If given interface number is 0, start from first interface.*/
  }

  for (;intIfNumTemp <= DS_MAX_INTF_COUNT; intIfNumTemp++)
  { 
    for (subsIndex = 0; subsIndex < L7_DOT1AD_SERVICES_MAX_PER_INTF; subsIndex++)
    {
      subsCfgTemp = &(dsCfgData->dsIntfCfg[intIfNumTemp].dsL2RelaySubscrptionCfg[subsIndex]);
      /* find a finite entry in the config list. */
      if (osapiStrncmp(subsCfgTemp->subscriptionName, &eosOfStr, sizeof(L7_uchar8)) != L7_NULL)
      {
        if (foundEntry == L7_TRUE && intIfNumTemp > intIfNumFound)
        {
          break;
        }
        if (osapiStrncmp(subsCfgTemp->subscriptionName, subscTemp, L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX) != L7_NULL)
        {
          if ((foundEntry == L7_TRUE) && 
              (osapiStrncmp(subsCfgTemp->subscriptionName, subsCfgFound.subscriptionName, L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX) < L7_NULL)
              && (osapiStrncmp(subsCfgTemp->subscriptionName, subscTemp, L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX) >= L7_NULL))
          {
            intIfNumFound = intIfNumTemp;
            memcpy(&subsCfgFound, subsCfgTemp , sizeof(dsL2RelaySubscriptionCfg_t));
          }
          if ((foundEntry == L7_FALSE) && 
              (osapiStrncmp(subsCfgTemp->subscriptionName, subscTemp, L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX) > L7_NULL))
          {
            intIfNumFound = intIfNumTemp;
            memcpy(&subsCfgFound, subsCfgTemp , sizeof(dsL2RelaySubscriptionCfg_t));
            foundEntry = L7_TRUE;
          }
        }
      }
    }
    memset(&subscTemp, 0, L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
  }
  if (foundEntry == L7_TRUE)
  {
    *intIfNum = intIfNumFound;
    osapiStrncpy(subscriptionName, subsCfgFound.subscriptionName,L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
    osapiReadLockGive(dsCfgRWLock);
    return L7_SUCCESS;
  }
  osapiReadLockGive(dsCfgRWLock);
  return L7_FAILURE;
}
#endif

/*********************************************************************
* @purpose  Gets the DHCP L2 Relay interface specific statistics.
*
* @param    intIfNum     @b((input)) internal interface number
* @param    intfStats    @b((ouput)) L2 Relay statstics.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayIntfStatsGet(L7_uint32 intIfNum, 
                              dsL2RelayIntfStats_t *intfStats)
{
  intfStats->untrustedSrvMsgsWithOpt82 = 
    dsIntfInfo[intIfNum].dsIntfStats.untrustedServerFramesWithOption82;
  intfStats->untrustedCliMsgsWithOpt82 = 
    dsIntfInfo[intIfNum].dsIntfStats.untrustedClientFramesWithOption82;
  intfStats->trustedSrvMsgsWithoutOpt82 = 
    dsIntfInfo[intIfNum].dsIntfStats.trustedServerFramesWithoutOption82;
  intfStats->trustedCliMsgsWithoutOpt82 =
    dsIntfInfo[intIfNum].dsIntfStats.trustedClientFramesWithoutOption82;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Clears the DHCP L2 Relay interface specific statistics.
*
* @param    intIfNum     @b((input)) internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayIntfStatsClear(L7_uint32 intIfNum)
{
  dsIntfInfo[intIfNum].dsIntfStats.untrustedServerFramesWithOption82 = 0;
  dsIntfInfo[intIfNum].dsIntfStats.untrustedClientFramesWithOption82 = 0;
  dsIntfInfo[intIfNum].dsIntfStats.trustedServerFramesWithoutOption82 = 0;
  dsIntfInfo[intIfNum].dsIntfStats.trustedClientFramesWithoutOption82 = 0;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Gives the next interface on which DHCP L2 Relay is enabled.
*
* @param    intIfNum     @b((input)) internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayIntfEnabledNextGet(L7_uint32 *intIfNum)
{
  L7_uint32 i;

  if (*intIfNum > L7_MAX_INTERFACE_COUNT)
    return L7_ERROR;

 if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = *intIfNum+1; i <= L7_MAX_INTERFACE_COUNT; i++)
  {
    if (_dsIntfL2RelayGet(i))
      break;
  }

  if (i > L7_MAX_INTERFACE_COUNT)
  {
    *intIfNum = 0;
    osapiReadLockGive(dsCfgRWLock);
    return L7_SUCCESS;

  }
  *intIfNum = i;
  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Gives the next vlan range on which DHCP L2 Relay is enabled.
*
* @param    vlanStart     @b((input)) starting VLAN Id
* @param    vlanEnd       @b((input)) Ending VLAN Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayVlanRangeNextGet(L7_uint32 *vlanStart, L7_uint32 *vlanEnd)
{
  L7_uint32 i;

  if (*vlanEnd > L7_PLATFORM_MAX_VLAN_ID)
    return L7_ERROR;

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  for (i = *vlanEnd + 1; i <= L7_PLATFORM_MAX_VLAN_ID; i++)
  {
    if (_dsVlanL2RelayGet(i))
      break;
  }

  if (i > L7_PLATFORM_MAX_VLAN_ID)
  {
    /* No more VLANs enabled */
    *vlanStart = 0;
    osapiReadLockGive(dsCfgRWLock);
    return L7_SUCCESS;
  }

  *vlanStart = i;
  for (i = i + 1; i <= L7_PLATFORM_MAX_VLAN_ID; i++)
  {
    if (!_dsVlanL2RelayGet(i))
    {
      *vlanEnd = i - 1; 
      osapiReadLockGive(dsCfgRWLock);
      return L7_SUCCESS;
    }
  }
  *vlanEnd = L7_PLATFORM_MAX_VLAN_ID;
  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Gives the next vlan range on which DHCP CircuitId is enabled.
*
* @param    vlanStart     @b((input)) starting VLAN Id
* @param    vlanEnd       @b((input)) Ending VLAN Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayCircuitIdVlanRangeNextGet(L7_uint32 *vlanStart, L7_uint32 *vlanEnd)
{
  L7_uint32 i;

  if (*vlanEnd > L7_PLATFORM_MAX_VLAN_ID)
    return L7_ERROR;

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  for (i = *vlanEnd + 1; i <= L7_PLATFORM_MAX_VLAN_ID; i++)
  {
    if (_dsVlanL2RelayCircuitIdGet(i))
      break;
  }

  if (i > L7_PLATFORM_MAX_VLAN_ID)
  {
    /* No more VLANs enabled */
    *vlanStart = 0;
    osapiReadLockGive(dsCfgRWLock);
    return L7_SUCCESS;
  }

  *vlanStart = i;
  for (i = i + 1; i <= L7_PLATFORM_MAX_VLAN_ID; i++)
  {
    if (!_dsVlanL2RelayCircuitIdGet(i))
    {
      *vlanEnd = i - 1; 
      osapiReadLockGive(dsCfgRWLock);
      return L7_SUCCESS;
    }
  }
  *vlanEnd = L7_PLATFORM_MAX_VLAN_ID;
  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Gives the next vlan on which DHCP RemoteId is enabled.
*
* @param    vlanNext      @b((input)) VLAN Id
* @param    remoteIdStr   @b((input)) Remote Id string confiured on that VLAN.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayRemoteIdVlanNextGet(L7_uint32 *vlanNext, L7_uchar8 *remoteIdStr)
{
  L7_uint32 i;

  if ((*vlanNext > L7_PLATFORM_MAX_VLAN_ID) || (remoteIdStr == L7_NULLPTR))
    return L7_ERROR;

 if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = *vlanNext+1; i <= L7_PLATFORM_MAX_VLAN_ID; i++)
  {
    if (_dsVlanL2RelayRemoteIdGet(i))
      break;
  }

  if (i > L7_PLATFORM_MAX_VLAN_ID)
  {
    osapiReadLockGive(dsCfgRWLock);
    return L7_FAILURE;

  }
  *vlanNext = i;
  osapiStrncpy(remoteIdStr, dsCfgData->dsL2RelayRemoteId[i],
               sizeof(dsCfgData->dsL2RelayRemoteId[i]));
  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}
#endif
/*********************************************************************
* @purpose  Get API for whether DHCP snooping verifies source MAC addresses.
*
* @param    verifyMac   @b((output)) Verify MAC option
*
* @returns  L7_SUCCESS, if mode was set
*
* @notes    When MAC verify is enabled (and port security is enabled 
*           on the interface), DHCP snooping tells the hardware to 
*           drop any incoming frames on the interface if the source MAC
*           is not the MAC address for a DHCP client who has been leased
*           an IP address.
*
* @end
*********************************************************************/
L7_RC_t dsVerifyMacGet(L7_BOOL *verifyMac)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *verifyMac = dsCfgData->dsVerifyMac;

  osapiReadLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set API for whether DHCP snooping verifies source MAC addresses.
*
* @param    verifyMac   @b((input)) L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    see dsVerifyMacGet().
*
* @end
*********************************************************************/
L7_RC_t dsVerifyMacSet(L7_BOOL verifyMac)
{
  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (verifyMac != dsCfgData->dsVerifyMac)
  {
    dsCfgData->dsVerifyMac = verifyMac;
    /* There is no apply function */
    dsConfigDataChange();
  }
  osapiWriteLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get API for whether DHCP snooping is enabled or disabled on a VLAN.
*
* @param    vlanId @b((input)) Low end of VLAN range
* @param    val    @b((output)) L7_ENABLE if DHCP snooping is enabled on the VLAN
*
* @returns  L7_SUCCESS, if value successfully retrieved
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dsVlanConfigGet(L7_uint32 vlanId, L7_uint32 *val)
{
  if ((vlanId < 1) ||
      (vlanId > L7_PLATFORM_MAX_VLAN_ID))
    return L7_ERROR;
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  if (_dsVlanEnableGet(vlanId))
    *val = L7_ENABLE;
  else
    *val = L7_DISABLE;

  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable or disable DHCP snooping on a VLAN.
*
* @param    vlanStart @b((input)) Low end of VLAN range
* @param    vlanEnd   @b((input)) High end of VLAN range
* @param    val       @b((input)) L7_ENABLE if DHCP snooping is enabled 
*                                 on all VLANs in the range
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dsVlanConfigSet(L7_uint32 vlanStart, L7_uint32 vlanEnd, L7_uint32 val)
{
  L7_uint32 i;
  if ((vlanStart > vlanEnd) || 
      ((val != L7_ENABLE) && (val != L7_DISABLE)))
  {
    return L7_ERROR;
  }

  if ((vlanStart == 0) || (vlanEnd > L7_PLATFORM_MAX_VLAN_ID))
    return L7_ERROR;

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = vlanStart; i <= vlanEnd; i++)
  {
    if ((val == L7_ENABLE) && !_dsVlanEnableGet(i))
    {
      _dsVlanEnable(i);       /* set config */
      dsVlanEnableApply(i, val);   /* react to change */
      dsConfigDataChange();
    }
    else if ((val == L7_DISABLE) && (_dsVlanEnableGet(i)))
    {
      _dsVlanDisable(i);
      dsVlanEnableApply(i, val);
      dsConfigDataChange();
    }
  }

  osapiWriteLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Given a VLAN ID, verify whether the DHCP snooping is 
*           enabled or not.
*
* @param    vlanId    @b((input)) Low end of VLAN range
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t dsVlanEnableModeGet(L7_uint32 vlanId )
{

  L7_RC_t rc =L7_FAILURE;
 
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  rc = _dsVlanEnableGet(vlanId);
  osapiReadLockGive(dsCfgRWLock);

  return rc;
   
}

/*********************************************************************
* @purpose  Given a VLAN ID, find the next vlanId where  DHCP snooping
*           is enabled.
*
* @param    vlanId    @b((input/ouput)) 
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t dsNextEnabledVlanGet(L7_uint32 *vlanId)
{
  L7_uint32 i;

  if (*vlanId > L7_PLATFORM_MAX_VLAN_ID)
    return L7_ERROR;

 if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = *vlanId+1; i <= L7_PLATFORM_MAX_VLAN_ID; i++)
  {
    if (_dsVlanEnableGet(i))
      break;
  }

  if (i > L7_PLATFORM_MAX_VLAN_ID)
  {
    *vlanId = 0;
    osapiReadLockGive(dsCfgRWLock);
    return L7_SUCCESS;

  }
  *vlanId = i;
  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Given a VLAN ID, return the endpoints of the next range of 
*           VLANs enabled for DHCP snooping.
*
* @param    vlanStart @b((input/output)) Low end of VLAN range
* @param    vlanEnd   @b((input/output)) High end of VLAN range
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes    This API intended to be used by show run, so that show run
*           doesn't have to list each enabled VLAN individually. Search
*           for next range starts at vlanEnd + 1. 
*
*           If no VLANs are enabled for DHCP snooping beyond vlanIn, 
*           returns L7_SUCCESS with vlanStart = 0.
*
* @end
*********************************************************************/
L7_RC_t dsNextEnabledVlanRangeGet(L7_uint32 *vlanStart,
                                  L7_uint32 *vlanEnd)
{
  L7_uint32 i;

  if (*vlanEnd > L7_PLATFORM_MAX_VLAN_ID)
    return L7_ERROR;

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  for (i = *vlanEnd + 1; i <= L7_PLATFORM_MAX_VLAN_ID; i++)
  {
    if (_dsVlanEnableGet(i))
      break;
  }

  if (i > L7_PLATFORM_MAX_VLAN_ID)
  {
    /* No more VLANs enabled */
    *vlanStart = 0;
    osapiReadLockGive(dsCfgRWLock);
    return L7_SUCCESS;
  }

  *vlanStart = i;
  for (i = i + 1; i <= L7_PLATFORM_MAX_VLAN_ID; i++)
  {
    if (!_dsVlanEnableGet(i))
    {
      *vlanEnd = i - 1; 
      osapiReadLockGive(dsCfgRWLock);
      return L7_SUCCESS;
    }
  }
  *vlanEnd = L7_PLATFORM_MAX_VLAN_ID;
  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
} 

/*********************************************************************
* @purpose  Determines whether DHCP snooping is enabled on a port.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((output)) L7_TRUE if DHCP snooping is enabled on port
*
* @returns  L7_SUCCESS, if value successfully retrieved
*           L7_FAILURE
*
* @notes    DHCP snooping is considered enabled on a port if DHCP snooping
*           is enabled on one or more VLANs the port participates in.
*
* @end
*********************************************************************/
L7_RC_t dsPortEnabledGet(L7_uint32 intIfNum, L7_BOOL *enabled)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *enabled = dsIntfIsSnooping(intIfNum);

  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get API for whether DHCP snooping considers a port trusted.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((output)) L7_TRUE if port is trusted
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dsIntfTrustGet(L7_uint32 intIfNum, L7_BOOL *trust)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *trust = _dsIntfTrustGet(intIfNum);

  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set API for whether DHCP snooping considers a port trusted.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((input))  L7_TRUE if port is trusted
*
* @returns  L7_SUCCESS, if mode was set
*           L7_NOT_SUPPORTED, if DHCP snooping is not supported on this 
*                             type of interface
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dsIntfTrustSet(L7_uint32 intIfNum, L7_BOOL trust)
{
  if (!dsIntfIsValid(intIfNum))
  {
    return L7_NOT_SUPPORTED;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  if (_dsIntfTrustGet(intIfNum) != trust)
  {
    if (trust)
      _dsIntfTrust(intIfNum);
    else
      _dsIntfDistrust(intIfNum);
    dsIntfTrustApply(intIfNum, trust);

    dsConfigDataChange();
  }

  osapiWriteLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get API for whether DHCP snooping logs invalid packets.
*
* @param    intIfNum @b((input))  internal interface number
* @param    log      @b((output)) L7_TRUE logging enabled on this interface
*
* @returns  L7_SUCCESS if value successfully retrieved
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dsIntfLogInvalidGet(L7_uint32 intIfNum, L7_BOOL *log)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *log = _dsIntfLogInvalidGet(intIfNum);

  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set API for whether DHCP snooping logs invalid packets.
*
* @param    intIfNum @b((input))  internal interface number
* @param    log      @b((input))  L7_TRUE if invalid packets should be logged
*
* @returns  L7_SUCCESS, if mode was set
*           L7_NOT_SUPPORTED, if DHCP snooping is not supported on this 
*                             type of interface
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dsIntfLogInvalidSet(L7_uint32 intIfNum, L7_BOOL log)
{
  if (!dsIntfIsValid(intIfNum))
  {
    return L7_NOT_SUPPORTED;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  if (_dsIntfLogInvalidGet(intIfNum) != log)
  {
    if (log)
      _dsIntfLogInvalid(intIfNum);
    else
      _dsIntfDontLogInvalid(intIfNum);

    dsConfigDataChange();
  }

  osapiWriteLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  API to get the rate limit value of incoming DHCP packets on
*           untrusted interfaces
*
* @param    intIfNum        @b((input)) internal interface number
* @param    rate            @b((output)) upper limit for number of
*                           incoming packets processed per second
*
* @returns  L7_SUCCESS, if value successfully updated
*           L7_ERROR, if inputs are invalid
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsIntfRateLimitGet(L7_uint32 intIfNum, L7_int32 *rate)
{
  if((intIfNum <= 0) || (intIfNum >= platIntfMaxCountGet()))
  {
    return L7_ERROR;
  }

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  *rate           = dsCfgData->dsIntfCfg[intIfNum].rateLimitCfg.rate_limit;

  osapiReadLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  API to get the burst interval value of incoming DHCP packets on
*           untrusted interfaces
*
* @param    intIfNum        @b((input)) internal interface number
* @param    burst_interval  @b((output)) the consecutive interval in
*                           seconds, over which the interface is
*                           monitored for a high rate of DHCP packets
*
* @returns  L7_SUCCESS, if value successfully updated
*           L7_ERROR, if inputs are invalid
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsIntfBurstIntervalGet(L7_uint32 intIfNum, L7_uint32 *burst_interval)
{
  if((intIfNum <= 0) || (intIfNum >= platIntfMaxCountGet()))
  {
    return L7_ERROR;
  }

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  *burst_interval = dsCfgData->dsIntfCfg[intIfNum].rateLimitCfg.burst_interval;

  osapiReadLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  API to set the rate limit value of incoming DHCP packets on
*           untrusted interfaces
*
* @param    intIfNum        @b((input)) internal interface number
* @param    rate            @b((input)) upper limit for number of
*                           incoming packets processed per second
*                           If passed as -1, it means no upper limit
*                           is set for the rate.
*
* @returns  L7_SUCCESS, if value successfully updated
*           L7_ERROR, if inputs are invalid
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsIntfRateLimitSet(L7_uint32 intIfNum, L7_int32 rate)
{
  if((intIfNum <= 0) || (intIfNum >= platIntfMaxCountGet()))
  {
    return L7_ERROR;
  }

  if ((rate != -1) && ((rate < L7_DS_RATE_LIMIT_MIN) ||
                       (rate > L7_DS_RATE_LIMIT_MAX)))
  {
    return L7_ERROR;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if(dsCfgData->dsIntfCfg[intIfNum].rateLimitCfg.rate_limit != rate)
  {
    dsCfgData->dsIntfCfg[intIfNum].rateLimitCfg.rate_limit = rate;

    dsIntfRateLimitApply(intIfNum);
  }

  osapiWriteLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  API to set the burst interval value of incoming DHCP packets on
*           untrusted interfaces
*
* @param    intIfNum        @b((input)) internal interface number
* @param    burst_interval  @b((input)) the consecutive interval in
*                           seconds, over which the interface is
*                           monitored for a high rate of DHCP packets
*
* @returns  L7_SUCCESS, if value successfully updated
*           L7_ERROR, if inputs are invalid
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsIntfBurstIntervalSet(L7_uint32 intIfNum, L7_uint32 burst_interval)
{
  if((intIfNum <= 0) || (intIfNum >= platIntfMaxCountGet()))
  {
    return L7_ERROR;
  }

  if ((burst_interval != -1) &&
      ((burst_interval < L7_DS_BURST_INTERVAL_MIN) ||
       (burst_interval > L7_DS_BURST_INTERVAL_MAX)))
  {
    return L7_ERROR;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if(dsCfgData->dsIntfCfg[intIfNum].rateLimitCfg.burst_interval != burst_interval)
  {
    dsCfgData->dsIntfCfg[intIfNum].rateLimitCfg.burst_interval    = burst_interval;

    dsIntfRateLimitApply(intIfNum);
  }

  osapiWriteLockGive(dsCfgRWLock);

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Sets the remote file name to be used to store/restore
*           the DHCP SNOOPING database 
*
* @param    fileName  @b((input)) DHCP snooping admin mode
*
* @returns  L7_SUCCESS, if fileName  was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsDbFileNameSet(L7_uchar8* fileName)
{
  if ((fileName == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  osapiStrncpy(dsCfgData->dsDbFileName,fileName, L7_CLI_MAX_STRING_LENGTH);
  dsConfigDataChange();

  osapiWriteLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the remote IP address to be used to store/restore
*           the DHCP SNOOPING database
*
* @param    ipAddress  @b((input)) ip address 
*
* @returns  L7_SUCCESS, if ipAddress  was set
* @returns  L7_FAILURE, if invalid ipAddress was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsDbIpAddrSet(L7_uint32 ipAddr)
{
  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
   
   if ( dsCfgData->dsDbIpAddr == 0)
   {   
     /* Validate IP address. Don't allow 0, mcast or above, loopback. */
     if ((ipAddr == 0) ||
        (ipAddr >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK) ||
        (((ipAddr & 0xff000000) >> 24) == 127))
     {
       osapiWriteLockGive(dsCfgRWLock);
       if (ipAddr == 0)
         return L7_SUCCESS;
       return L7_ERROR;
     }
   }

   if (ipAddr == 0)
   {
     memset (dsCfgData->dsDbFileName,
             '\0', sizeof(dsCfgData->dsDbFileName));
    dsCfgData->dsDbIpAddr = ipAddr; 
   }
   else if (strlen(dsCfgData->dsDbFileName) > 0)
   {
     dsCfgData->dsDbIpAddr = ipAddr;
   }
   else
   {
     osapiWriteLockGive(dsCfgRWLock);
     return L7_FAILURE;
   }
   dsConfigDataChange();
   

  osapiWriteLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the wtite delay interval to store
*           the DHCP SNOOPING database
*
* @param    storeInterval  @b((input)) ip address
*
* @returns  L7_SUCCESS, if store val  was set
* @returns  L7_FAILURE, if invalid store val was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsDbStoreIntervalSet(L7_uint32 storeInterval)
{
  if ( (storeInterval < L7_DS_DB_MIN_STORE_INTERVAL) || 
       (storeInterval > L7_DS_DB_MAX_STORE_INTERVAL) )
  {
    return L7_FAILURE;
  }
  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  dsCfgData->dsDbStoreInterval = storeInterval;
  dsConfigDataChange();

  osapiWriteLockGive(dsCfgRWLock);

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Get file name that will be used for storing/restoring the
*           DHCP SNOOPING.
*
* @param    fileName  @b((output)) fileName 
*
* @returns  L7_SUCCESS if value successfully retrieved
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsDbFileNameGet(L7_uchar8* fileName)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
    
  memset(fileName, 0, strlen(dsCfgData->dsDbFileName)+1);
  osapiStrncpy (fileName, dsCfgData->dsDbFileName, strlen(dsCfgData->dsDbFileName));

  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the remote IP address to be used to store/restore
*           the DHCP SNOOPING database
*
* @param    ipAddress  @b((input)) ip address
*
* @returns  L7_SUCCESS, if ipAddress  was set
* @returns  L7_FAILURE, if invalid ipAddress was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsDbIpAddrGet(L7_uint32* ipAddr)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  *ipAddr = dsCfgData->dsDbIpAddr;

  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the wait delay interval to be used to store
*           the DHCP SNOOPING database
*
* @param    storeInterval  @b((input)) ip address
*
* @returns  L7_SUCCESS, if store interval  was Get
* @returns  L7_FAILURE, if invalid store interval  was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsDbStoreIntervalGet(L7_uint32 *storeInterval)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  *storeInterval = dsCfgData->dsDbStoreInterval;

  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}





/*********************************************************************
* @purpose  Add a static entry to the DHCP snooping bindings database.
*
* @param    macAddr  @b((input))  client MAC address. 
* @param    ipAddr   @b((input))  client IP address. 
* @param    vlanId   @b((input))  client VLAN ID. 
* @param    intIfNum @b((input))  internal interface number. 
*
* @returns  L7_SUCCESS if entry added.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dsStaticBindingAdd(L7_enetMacAddr_t *macAddr, L7_uint32 ipAddr,
                           L7_ushort16 vlanId, L7_ushort16 innerVlanId /*PTin modified: DHCP */, L7_uint32 intIfNum)
{
  L7_RC_t rc;

  /* Validate IP address. Don't allow 0, mcast or above, loopback. */
  if ((ipAddr == 0) || 
      (ipAddr >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK) ||
      (((ipAddr & 0xff000000) >> 24) == 127))
  {
    return L7_ERROR;
  }

  /* Validate MAC address */
  if ((memcmp(macAddr, nullMacAddr, L7_ENET_MAC_ADDR_LEN) == 0) ||
      ((macAddr->addr[0] & 1) == 1))
  {
    /* Don't allow the following invalid MAC addresses:
     * (a) Address 00:00:00:00:00:00
     * (b) Any multicast address (the first bit in the first byte being 1)
     *     It includes the broadcast address FF:FF:FF:FF:FF:FF
     */
    return L7_ERROR;
  }

  /* Validate VLAN ID */
  if ((vlanId == 0) || (vlanId > L7_PLATFORM_MAX_VLAN_ID))
  {
    return L7_ERROR;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = dsBindingAdd(DS_BINDING_STATIC, macAddr, ipAddr, vlanId, innerVlanId /*PTin modified: DHCP */, intIfNum);

  osapiWriteLockGive(dsCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  Remove a static entry from the DHCP snooping bindings database.
*
* @param    macAddr  @b((input))  client MAC address. 
*
* @returns  L7_SUCCESS if entry removed.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dsStaticBindingRemove(L7_enetMacAddr_t *macAddr)
{
  L7_RC_t rc;

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

   rc = dsBindingRemove(macAddr);

   osapiWriteLockGive(dsCfgRWLock);
   return rc;
}

/*********************************************************************
* @purpose  Clear DHCP snooping bindings.
*
* @param    intIfNum  @b((input))  If non-zero, only bindings associated
*                                  with a specific interface are cleared. 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dsBindingClear(L7_uint32 intIfNum)
{
  L7_RC_t retval = L7_SUCCESS;
  
  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  retval = _dsBindingClear(intIfNum,L7_TRUE);

  osapiWriteLockGive(dsCfgRWLock);
  return retval;
}

/*********************************************************************
* @purpose  Clear DHCP snooping bindings that belong to the requested EVC.
*
* @param    ext_evc_id  @b((input))  EVC id 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dsEvcBindingsClear(L7_uint32 ext_evc_id)
{
  L7_RC_t            retval = L7_SUCCESS;
  
  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  dsBindingEvcRemoveAll(ext_evc_id);

  osapiWriteLockGive(dsCfgRWLock);
  return retval;
}

/*********************************************************************
* @purpose  Get the number of DHCP snooping bindings.
*
* @param    void
*
* @returns  number of bindings
*
* @notes    
*
*
* @end
*********************************************************************/
L7_uint32 dsBindingsCount(void)
{
  L7_uint32 count = 0;
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return 0;
  count = _dsBindingsCount();
  osapiReadLockGive(dsCfgRWLock);
  return count;
}

/*********************************************************************
* @purpose  Get the number of DHCP snooping static bindings.
*
* @param    void
*
* @returns  number of bindings
*
* @notes
*
*
* @end
*********************************************************************/
L7_uint32 dsStaticBindingsCount(void)
{
  L7_uint32 count = 0;
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return 0;
  count = _dsStaticBindingsCount();
  osapiReadLockGive(dsCfgRWLock);
  return count;
}


/*********************************************************************
* @purpose  Get the DHCP snooping binding for a given client.
*
* @param    dsBinding  @b((input/output))  DHCP snooping binding table entry
*
* @returns  L7_SUCCESS if client binding found
*
* @notes    The MAC address uniquely identifies the client.
*
* @end
*********************************************************************/
L7_RC_t dsBindingGet(dhcpSnoopBinding_t *dsBinding)
{
  L7_RC_t rc;

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;
  rc = dsBindingFind(dsBinding, L7_MATCH_EXACT);
  osapiReadLockGive(dsCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  Given a DHCP snooping binding, get the next one.
*
* @param    dsBinding  @b((input/output))  DHCP snooping binding table entry
*
* @returns  L7_SUCCESS if another binding is found
*
* @notes    The MAC address uniquely identifies the client.
*
* @end
*********************************************************************/
L7_RC_t dsBindingGetNext(dhcpSnoopBinding_t *dsBinding)
{
  L7_RC_t rc;

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;
  rc = dsBindingFind(dsBinding, L7_MATCH_GETNEXT);
  osapiReadLockGive(dsCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  Find out if a binding exists for a given MAC address,
*           IP address, and VLAN.
*
* @param    macAddr  @b((input))  client MAC address. 
* @param    ipAddr   @b((input))  client IP address. 
* @param    vlanId   @b((input))  client VLAN ID. 
*
* @returns  L7_TRUE if binding exists
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL dsClientKnown(L7_enetMacAddr_t *macAddr, L7_uint32 ipAddr,
                      L7_ushort16 vlanId)
{
  L7_BOOL found = L7_FALSE;

   if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FALSE;

   if (dsBindingExists(macAddr, ipAddr, vlanId) == L7_TRUE)
     found = L7_TRUE;

   osapiReadLockGive(dsCfgRWLock);
   return found;
}

/*********************************************************************
* @purpose  Get DHCP snooping statistics.
*
* @param    intIfNum  @b((input))  internal interface number
* @param    intfStats @b((output)) stats for given interface 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dsIntfStatsGet(L7_uint32 intIfNum, dhcpSnoopIntfStats_t *intfStats)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
   return L7_FAILURE;

  intfStats->macVerify = dsIntfInfo[intIfNum].dsIntfStats.macVerify;
  intfStats->intfMismatch = dsIntfInfo[intIfNum].dsIntfStats.intfMismatch;
  intfStats->untrustedSvrMsg = dsIntfInfo[intIfNum].dsIntfStats.untrustedSvrMsg;

  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear DHCP snooping interface statistics for all interfaces.
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dsIntfStatsClear(void)
{
  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
   return L7_FAILURE;

  _dsIntfStatsClear();

  osapiWriteLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/* temporary to allow testing */

/* first arg in string format */
void dsAddStaticBinding(L7_uchar8 *macStr, L7_uint32 ipAddr,
                        L7_ushort16 vlanId, L7_ushort16 innerVlanId /*PTin modified: DHCP */, L7_uint32 intIfNum)
{
  L7_enetMacAddr_t macAddr;

  if (dsStringToMac(macStr, &macAddr) == L7_SUCCESS)
  {
    if (dsStaticBindingAdd(&macAddr, ipAddr, vlanId, innerVlanId /*PTin modified: DHCP */, intIfNum) != L7_SUCCESS)
        printf("\nFailed to add static binding for %s.", macStr);
  }
  else
    printf("\nfailed to convert MAC string");
}

/* first arg in string format */
void dsRemoveStaticBinding(L7_uchar8 *macStr)
{
  L7_enetMacAddr_t macAddr;

  if (dsStringToMac(macStr, &macAddr) == L7_SUCCESS)
  {
    if (dsStaticBindingRemove(&macAddr) != L7_SUCCESS)
      printf("\nFailed to remove static binding for %s.", macStr);
  }
  else
    printf("\nfailed to convert MAC string");
}

