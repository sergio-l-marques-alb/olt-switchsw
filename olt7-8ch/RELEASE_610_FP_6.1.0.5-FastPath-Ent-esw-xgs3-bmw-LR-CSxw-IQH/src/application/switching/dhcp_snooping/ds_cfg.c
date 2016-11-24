/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  ds_cfg.c
*
* @purpose   DHCP snooping 
*
* @component DHCP snooping
*
* @comments none
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
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "comm_mask.h"
#include "dhcp_snooping_exports.h"

#include "ds_cfg.h"
#include "ds_util.h"

#ifdef L7_IPSG_PACKAGE
#include "ds_ipsg.h"
#endif

extern dsCfgData_t *dsCfgData;
extern osapiRWLock_t  dsCfgRWLock;
extern dsInfo_t *dsInfo;
extern dsIntfInfo_t *dsIntfInfo;
extern void *Ds_Event_Queue;
extern void *dsMsgQSema;
extern dsDbCfgData_t dsDbCfgData;
extern L7_BOOL downloadFlag;
extern L7_BOOL txtBasedConfigComplete;

/*********************************************************************
* @purpose  Set the enable bit for a given VLAN in the config
*
* @param    vlanId   VLAN ID
*
* @returns  none
*
* @end
*********************************************************************/
void _dsVlanEnable(L7_uint32 vlanId)
{
  L7_VLAN_SETMASKBIT(dsCfgData->dsVlans, vlanId);
}

/*********************************************************************
* @purpose  Clear the enable bit for a given VLAN in the config
*
* @param    vlanId   VLAN ID
*
* @returns  none
*
* @end
*********************************************************************/
void _dsVlanDisable(L7_uint32 vlanId)
{
  L7_VLAN_CLRMASKBIT(dsCfgData->dsVlans, vlanId);
}

/*********************************************************************
* @purpose  Determine if DHCP snooping is enabled on a give VLAN.
*
* @param    vlanId   VLAN ID
*
* @returns  none
*
* @end
*********************************************************************/
L7_BOOL _dsVlanEnableGet(L7_uint32 vlanId)
{
  if (L7_VLAN_ISMASKBITSET(dsCfgData->dsVlans, vlanId))
    return L7_TRUE;
  else
    return L7_FALSE;
}

#ifdef L7_DHCP_L2_RELAY_PACKAGE
/*********************************************************************
* @purpose  Sets the L2Relay for a given interface in the config
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  none
*
* @end
*********************************************************************/
void _dsIntfL2RelayEnable(L7_uint32 intIfNum)
{
  L7_INTF_SETMASKBIT(dsCfgData->dsL2RelayIntfMode, intIfNum);
}

/*********************************************************************
* @purpose  Resets the L2Relay for a given interface in the config
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  none
*
* @end
*********************************************************************/
void _dsIntfL2RelayDisable(L7_uint32 intIfNum)
{
  L7_INTF_CLRMASKBIT(dsCfgData->dsL2RelayIntfMode, intIfNum);
}

/*********************************************************************
* @purpose  Determines if an interface is enabled for DHCP L2Relay
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_TRUE if interface is L2 Realy enabled.
*
* @end
*********************************************************************/
L7_BOOL _dsIntfL2RelayGet(L7_uint32 intIfNum)
{
  if (L7_INTF_ISMASKBITSET(dsCfgData->dsL2RelayIntfMode, intIfNum))
    return L7_TRUE;
  else
    return L7_FALSE;
}
/*********************************************************************
* @purpose  Set the L2 Relay trust bit for a given interface in the config
*
* @param    intIfNum   internal interface number
*
* @returns  none
*
* @end
*********************************************************************/
void _dsIntfL2RelayTrust(L7_uint32 intIfNum)
{
  L7_INTF_SETMASKBIT(dsCfgData->dsL2RelayTrust, intIfNum);
}

/*********************************************************************
* @purpose  Reset the L2 Relay trust bit for a given interface in the config
*
* @param    intIfNum   internal interface number
*
* @returns  none
*
* @end
*********************************************************************/
void _dsIntfL2RelayDistrust(L7_uint32 intIfNum)
{
  L7_INTF_CLRMASKBIT(dsCfgData->dsL2RelayTrust, intIfNum);
}

/*********************************************************************
* @purpose  Determine whether a given interface is trusted for DHCP L2 Relay.
*
* @param    intIfNum   internal interface number
*
* @returns  L7_TRUE if interface is trusted.
*
* @end
*********************************************************************/
L7_BOOL _dsIntfL2RelayTrustGet(L7_uint32 intIfNum)
{
  if (L7_VLAN_ISMASKBITSET(dsCfgData->dsL2RelayTrust, intIfNum))
    return L7_TRUE;
  else
    return L7_FALSE;
}
/*********************************************************************
* @purpose  Sets the L2Relay for a given vlan in the config
*
* @param    vlanNum   @b((input)) VLAN Id
*
* @returns  none
*
* @end
*********************************************************************/
void _dsVlanL2RelayEnable(L7_uint32 vlanNum)
{
  L7_VLAN_SETMASKBIT(dsCfgData->dsL2RelayVlanMode, vlanNum);
}

/*********************************************************************
* @purpose  Resets the L2Relay for a given vlan in the config
*
* @param    vlanNum   @b((input)) VLAN Id
*
* @returns  none
*
* @end
*********************************************************************/
void _dsVlanL2RelayDisable(L7_uint32 vlanNum)
{
  L7_VLAN_CLRMASKBIT(dsCfgData->dsL2RelayVlanMode, vlanNum);
}

/*********************************************************************
* @purpose  Determines if a vlan is enabled for DHCP L2Relay
*
* @param    intIfNum   @b((input)) VLAN Id
*
* @returns  L7_TRUE if interface is L2 Realy enabled.
*
* @end
*********************************************************************/
L7_BOOL _dsVlanL2RelayGet(L7_uint32 vlanNum)
{
  if (L7_VLAN_ISMASKBITSET(dsCfgData->dsL2RelayVlanMode, vlanNum))
    return L7_TRUE;
  else
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Sets the L2Relay Circuit-Id for a given vlan in the config
*
* @param    vlanNum   @b((input)) VLAN Id
*
* @returns  none
*
* @end
*********************************************************************/
void _dsVlanL2RelayCircuitIdEnable(L7_uint32 vlanNum)
{
  L7_VLAN_SETMASKBIT(dsCfgData->dsL2RelayCircuitIdMode, vlanNum);
}

/*********************************************************************
* @purpose  Resets the L2Relay Circuit-Id for a given vlan in the config
*
* @param    vlanNum   @b((input)) VLAN Id
*
* @returns  none
*
* @end
*********************************************************************/
void _dsVlanL2RelayCircuitIdDisable(L7_uint32 vlanNum)
{
  L7_VLAN_CLRMASKBIT(dsCfgData->dsL2RelayCircuitIdMode, vlanNum);
}

/*********************************************************************
* @purpose  Determines if a vlan is enabled for DHCP L2Relay Circuit-Id
*
* @param    vlanNum   @b((input)) VLAN Id
*
* @returns  L7_TRUE if vlan is L2 Realy enabled.
*
* @end
*********************************************************************/
L7_BOOL _dsVlanL2RelayCircuitIdGet(L7_uint32 vlanNum)
{
  if (L7_VLAN_ISMASKBITSET(dsCfgData->dsL2RelayCircuitIdMode, vlanNum))
    return L7_TRUE;
  else
    return L7_FALSE;
}
/*********************************************************************
* @purpose  Determines if a vlan is enabled for DHCP L2Relay Remote-Id
*
* @param    vlanNum   @b((input)) VLAN Id
*
* @returns  L7_TRUE if vlan is L2 Realy enabled.
*
* @end
*********************************************************************/
L7_BOOL _dsVlanL2RelayRemoteIdGet(L7_uint32 vlanNum)
{
  L7_uchar8 endOfStr = FD_DHCP_L2RELAY_REMOTE_ID_VLAN_MODE;
  if (osapiStrncmp(dsCfgData->dsL2RelayRemoteId[vlanNum],
                   &(endOfStr), sizeof(endOfStr)) != L7_NULL)
    return L7_TRUE;
  else
    return L7_FALSE;
}
#endif
/*********************************************************************
* @purpose  Set the trust bit for a given interface in the config
*
* @param    intIfNum   internal interface number
*
* @returns  none
*
* @end
*********************************************************************/
void _dsIntfTrust(L7_uint32 intIfNum)
{
  L7_INTF_SETMASKBIT(dsCfgData->dsTrust, intIfNum);
}

/*********************************************************************
* @purpose  Set the trust bit for a given interface in the config
*
* @param    intIfNum   internal interface number
*
* @returns  none
*
* @end
*********************************************************************/
void _dsIntfDistrust(L7_uint32 intIfNum)
{
  L7_INTF_CLRMASKBIT(dsCfgData->dsTrust, intIfNum);
}

/*********************************************************************
* @purpose  Determine whether a given interface is trusted for DHCP snooping.
*
* @param    intIfNum   internal interface number
*
* @returns  L7_TRUE if interface is trusted.
*
* @end
*********************************************************************/
L7_BOOL _dsIntfTrustGet(L7_uint32 intIfNum)
{
  if (L7_VLAN_ISMASKBITSET(dsCfgData->dsTrust, intIfNum))
    return L7_TRUE;
  else
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Configure DHCP snooping to log invalid packets received on
*           a specific interface 
*
* @param    intIfNum   internal interface number
*
* @returns  none
*
* @end
*********************************************************************/
void _dsIntfLogInvalid(L7_uint32 intIfNum)
{
  L7_INTF_SETMASKBIT(dsCfgData->dsLogInvalid, intIfNum);
}

/*********************************************************************
* @purpose  Configure an interface not to log invalid packets
*
* @param    intIfNum   internal interface number
*
* @returns  none
*
* @end
*********************************************************************/
void _dsIntfDontLogInvalid(L7_uint32 intIfNum)
{
  L7_INTF_CLRMASKBIT(dsCfgData->dsLogInvalid, intIfNum);
}

/*********************************************************************
* @purpose  Determine whether a given interface is configured to 
*           log invalid packets.
*
* @param    intIfNum   internal interface number
*
* @returns  L7_TRUE if interface is trusted.
*
* @end
*********************************************************************/
L7_BOOL _dsIntfLogInvalidGet(L7_uint32 intIfNum)
{
  if (L7_VLAN_ISMASKBITSET(dsCfgData->dsLogInvalid, intIfNum))
    return L7_TRUE;
  else
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Build default DHCP snooping config data
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @end
*********************************************************************/
void dsBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 i;
  #ifdef L7_DHCP_L2_RELAY_PACKAGE
  L7_uchar8 endOfStr = FD_DHCP_L2RELAY_REMOTE_ID_VLAN_MODE;
  #endif
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
#ifdef L7_DHCP_L2_RELAY_PACKAGE
  L7_uint32 j = 0;
#endif
#endif

  memset((void *)dsCfgData, 0, sizeof(dsCfgData_t));
  memset((void *)&dsDbCfgData, 0, sizeof(dsDbCfgData_t));

  strcpy( dsDbCfgData.cfgHdr.filename, L7_DHCP_SNOOPING_CFG_FILENAME);
  dsDbCfgData.cfgHdr.version     = L7_DHCP_SNOOPING_VER_CURRENT;
  dsDbCfgData.cfgHdr.componentID = L7_DHCP_SNOOPING_COMPONENT_ID;
  dsDbCfgData.cfgHdr.type        = L7_CFG_DATA;
  dsDbCfgData.cfgHdr.length      = sizeof( dsDbCfgData_t );
  dsDbCfgData.cfgHdr.dataChanged = L7_FALSE;

  

  dsCfgData->dsGlobalAdminMode = FD_DHCP_SNOOP_ADMIN_MODE;
  #ifdef L7_DHCP_L2_RELAY_PACKAGE
  dsCfgData->dsL2RelayAdminMode = FD_DHCP_L2RELAY_ADMIN_MODE;
  #endif
  dsCfgData->dsVerifyMac = FD_DHCP_SNOOP_VERIFY_MAC;
  dsCfgData->dsDbStoreInterval = L7_DS_DB_STORE_INTERVAL;
  for (i = 1; i <= L7_PLATFORM_MAX_VLAN_ID; i++)
  {
    if (FD_DHCP_SNOOP_VLAN == L7_ENABLE)
    {
      _dsVlanEnable(i);
    }

  #ifdef L7_DHCP_L2_RELAY_PACKAGE
    if (FD_DHCP_L2RELAY_VLAN_MODE == L7_ENABLE)
    {
      _dsVlanL2RelayEnable(i);
    }
    if (FD_DHCP_L2RELAY_CIRCUIT_ID_VLAN_MODE == L7_ENABLE)
    {
      _dsVlanL2RelayCircuitIdEnable(i);
    }
    /* Set the remoteId to default string.*/
    osapiStrncpy(dsCfgData->dsL2RelayRemoteId[i],
                 &(endOfStr),
                 sizeof(endOfStr));
   #endif
  }

  for (i = 1; i < DS_MAX_INTF_COUNT; i++)
  {
  #ifdef L7_DHCP_L2_RELAY_PACKAGE
    if (FD_DHCP_L2RELAY_INTERFACE_MODE == L7_ENABLE)
    {
      _dsIntfL2RelayEnable(i);
    }
    if (FD_DHCP_L2RELAY_INTERFACE_TRUST_MODE == L7_TRUE)
    {
      _dsIntfL2RelayTrust(i);
    }
  #endif
    if (FD_DHCP_SNOOP_TRUST == L7_TRUE)
    {
      _dsIntfTrust(i);
    }
    if (FD_DHCP_SNOOP_LOG_INVALID_MSG == L7_TRUE)
    {
      _dsIntfLogInvalid(i);
    }


#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
#ifdef L7_DHCP_L2_RELAY_PACKAGE
    /* For subscription entries, build default config.*/
    for (j = 0; j < L7_DOT1AD_SERVICES_MAX_PER_INTF; j++)
    {
      dsL2RelaySubscriptionCfg_t *subsCfgTemp = L7_NULLPTR;
      subsCfgTemp = &(dsCfgData->dsIntfCfg[i].dsL2RelaySubscrptionCfg[j]);

      osapiStrncpy(subsCfgTemp->subscriptionName,
                   &(endOfStr),
                   sizeof(endOfStr));
      subsCfgTemp->l2relay = FD_DHCP_L2RELAY_SUBSCRIPTION_MODE;
      subsCfgTemp->circuitId = FD_DHCP_L2RELAY_CIRCUIT_ID_SUBSCRIPTION_MODE;

      osapiStrncpy(subsCfgTemp->remoteId,
                   &(endOfStr),
                   sizeof(endOfStr));
    }
#endif
#endif

#ifdef L7_IPSG_PACKAGE
    dsCfgData->ipsgIntfCfg[i].verifyIp = FD_IPSG_VERIFY_IP;
    dsCfgData->ipsgIntfCfg[i].verifyMac = FD_IPSG_VERIFY_MAC;
#endif
    dsCfgData->dsIntfCfg[i].rateLimitCfg.rate_limit     = FD_DHCP_SNOOPING_RATE_LIMIT;
    dsCfgData->dsIntfCfg[i].rateLimitCfg.burst_interval = FD_DHCP_SNOOPING_BURST_INTERVAL;
  }
  dsCfgData->dsTraceFlags = 0;
}

/*********************************************************************
* @purpose  No-op
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsSave(void)
{
  /* Try to restore from remote db or from local db only when the 
   * persistency feature is present */
  if(cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                           L7_DHCP_SNOOPING_DB_PERSISTENCY) == L7_TRUE)
  {
    /* This is one of the few files that are stored in binary format
     * with running config the data changed flag for this component may have been reset
     * hence do not check for data changed flag and save irrespective
     */
    if (dsCfgData->dsDbIpAddr) /* Save the bindings in a remote machine */
    {
      /* Must take care that this does not happen when dsSave is called from 
       * runCfg task
       */
      dsTftpUploadInit();
    }
    else /* Save the bindings in local machine */
    {
      dsDbLocalSave();
    }
    dsCfgDataNotChanged();
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Restores DHCP snooping user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsRestore(void)
{
  L7_RC_t rc = L7_FAILURE;

#ifdef L7_IPSG_PACKAGE
  osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER);
  rc = dsRestoreProcess();
  osapiWriteLockGive(dsCfgRWLock);
#endif

  return rc;
}

/*********************************************************************
* @purpose  Restores DHCP snooping user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    assuming semaphore already taken
*
* @end
*********************************************************************/
L7_RC_t dsRestoreProcess(void)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i;

  dsBuildDefaultConfigData(0);    /* was dsCfgData->cfgHdr.version */

  /* Clear status data */
  for (i = 1; i < DS_MAX_INTF_COUNT; i++)
  {
    memset(&dsIntfInfo[i].dsVlans, '\0',sizeof(L7_VLAN_MASK_t));
    dsIntfInfo[i].dsNumVlansEnabled = 0;
  }

  /* Disable DHCP snooping */
  rc = dsApplyConfigData();
  _dsBindingClear(0,L7_TRUE);

  dsConfigDataChange();

#ifdef L7_IPSG_PACKAGE
  _ipsgEntryClear(0); 
#endif

  return rc;
}

/*********************************************************************
* @purpose  Checks if DHCP snooping user config data is out of sync
*           with persistent storage.
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dsHasDataChanged( void )
{
  return (dsInfo->cfgDataChanged || dsInfo->dsDbDataChanged);
}
void dsResetDataChanged(void)
{
  dsInfo->cfgDataChanged = L7_FALSE;
  dsInfo->dsDbDataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Apply DHCP snooping configuration
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsApplyConfigData(void)
{
  return dsAdminModeApply(dsCfgData->dsGlobalAdminMode);
}


/*********************************************************************
* @purpose  Places the Apply config completion notification
*           in the Q.
*
* @param    event  : event from txt config
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Right now we will handle only one event
*
* @end
*********************************************************************/
L7_RC_t dsApplyConfigCompleteCb(L7_uint32 event)
{
  L7_RC_t rc = L7_SUCCESS;
  dsEventMsg_t msg;

  if (Ds_Event_Queue == L7_NULLPTR)
  {
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
             "Apply config completion came in a wrong phase");
    return L7_FAILURE;
  }
  /* Ignore events types we don't care about.  */
  if ( (event != TXT_CFG_APPLY_FAILURE) &&
       (event != TXT_CFG_APPLY_SUCCESS) )
  {
    return L7_SUCCESS;
  }

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* process event on our thread */
  memset((void *)&msg, 0, sizeof(dsEventMsg_t) );
  msg.msgType = DS_CFG_APPLY_EVENT;
  msg.dsMsgData.cfgApplyEvent.txtCfgEventType = event;

  rc = osapiMessageSend(Ds_Event_Queue, &msg, sizeof(dsEventMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc == L7_SUCCESS)
  {
    osapiSemaGive(dsMsgQSema);
  }
  else
  {
    static L7_uint32 lastMsg = 0;
    dsInfo->debugStats.eventMsgTxError++;
    if (osapiUpTimeRaw() > lastMsg)
    {
      lastMsg = osapiUpTimeRaw();
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
              "Failed to queue apply config event to DHCP snooping thread.");
    }
  }
  osapiReadLockGive(dsCfgRWLock);
  return rc;
}


/*********************************************************************
* @purpose  Process the Apply config completion notification
*
* @param    applyCfgevent  : event from txt config
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Right now we will handle only one event
*
* @end
*********************************************************************/
L7_RC_t dstxtCfgEventProcess(L7_uint32 applyCfgEvent)
{
  L7_RC_t rc =L7_SUCCESS;
  switch (applyCfgEvent)
  {
    case TXT_CFG_APPLY_SUCCESS:

      /* Set the flag that the text based configuration apply is complete.
       * This flag is used in dsDbLocalSave() */
      txtBasedConfigComplete = L7_TRUE;

      /* Try to restore from remote db or from local db only when the
       * persistency feature is present */
      if(cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                               L7_DHCP_SNOOPING_DB_PERSISTENCY) == L7_TRUE)
      {
        if (dsCfgData->dsDbIpAddr)
        {
          downloadFlag = L7_TRUE;
        }
        else
        {
          dsDbLocalRestore();
        }
      }

    break;
   default:
      rc = L7_SUCCESS;
    break;
  }
  return rc;
}





#ifdef L7_DHCPS_PACKAGE 
/*********************************************************************
* @purpose  Places the local DHCP server notification
*
* @param    event  : event on a mac address
* @param    chAddr : binding  mac address to be modified
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Right now we will handle only one event
*
* @end
*********************************************************************/
L7_RC_t  dsLocalServerCb(L7_uint32 sendMsgType,
                         L7_enetMacAddr_t chAddr,
                         L7_uint32 ipAddr,
                         L7_uint32 leaseTime)
{
  L7_RC_t rc = L7_SUCCESS;
  dsEventMsg_t msg;


  if (Ds_Event_Queue == L7_NULLPTR)
  {
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
             "Local server call back came in a wrong phase");
    return L7_FAILURE;
  }
  /* Ignore events types we don't care about.  */
  if ( (sendMsgType != DHCPS_MESSAGE_TYPE_ACK) &&
       (sendMsgType != DHCPS_MESSAGE_TYPE_NACK ) )
  {
    return L7_SUCCESS;
  }

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

 /* process event on our thread */
  memset((void *)&msg, 0, sizeof(dsEventMsg_t) );
  if ( sendMsgType == DHCPS_MESSAGE_TYPE_ACK)
    msg.msgType = DS_DHCPS_ACK_EVENT;
  else if (sendMsgType == DHCPS_MESSAGE_TYPE_NACK)
    msg.msgType = DS_DHCPS_NACK_EVENT;
  memcpy ((void *)&msg.dsMsgData.dhcpsEvent.chAddr.addr,
          (void *)&chAddr.addr,
           L7_ENET_MAC_ADDR_LEN);
  msg.dsMsgData.dhcpsEvent.ipAddr = ipAddr;
  msg.dsMsgData.dhcpsEvent.leaseTime = leaseTime;
   

  rc = osapiMessageSend(Ds_Event_Queue, &msg, sizeof(dsEventMsg_t),                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc == L7_SUCCESS)
  {
    osapiSemaGive(dsMsgQSema);
  }
  else
  {
    static L7_uint32 lastMsg = 0;
    dsInfo->debugStats.eventMsgTxError++;
    if (osapiUpTimeRaw() > lastMsg)
    {
      lastMsg = osapiUpTimeRaw();
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
              "Failed to queue local server event to DHCP snooping thread.");

    }
  }
  osapiReadLockGive(dsCfgRWLock);
  return rc;

}
#endif


/*********************************************************************
* @purpose  Places the  tftp download status of Bindings
*
* @param    direction        : tftp direction upload/download 
* @param    directionStatus  : tftp operation status of the bindings
*
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Right now as the direction can be up or down, a true
*           value in direction  will represent up direction and
*           a false value will represent the down direction.
*
* @end
*********************************************************************/
L7_RC_t  dsTftpStatusCb(L7_uint32 direction, L7_BOOL downloadStatus)

{
  L7_RC_t rc = L7_SUCCESS;
  dsEventMsg_t msg;


  if (Ds_Event_Queue == L7_NULLPTR)
  {
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
             "Tftp download status call back came in a wrong phase");
    return L7_FAILURE;
  }
  

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;


 /* process event on our thread */
  memset((void *)&msg, 0, sizeof(dsEventMsg_t) );
  if ( direction == L7_TRUE)
    msg.msgType = DS_DB_UPLOAD_EVENT;
  else
    msg.msgType = DS_DB_DOWNLOAD_EVENT;
  msg.dsMsgData.tftpEvent.dsDbDownloadStatus = downloadStatus;

  rc = osapiMessageSend(Ds_Event_Queue, &msg, sizeof(dsEventMsg_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc == L7_SUCCESS)
  {
    osapiSemaGive(dsMsgQSema);
  }
  else
  {
    static L7_uint32 lastMsg = 0;
    dsInfo->debugStats.eventMsgTxError++;
    if (osapiUpTimeRaw() > lastMsg)
    {
      lastMsg = osapiUpTimeRaw();
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
              "Failed to queue TFTP download statusinto DHCP snooping thread.");

    }
  }
  osapiReadLockGive(dsCfgRWLock);
  return rc;

}






