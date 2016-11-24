/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename iscsi.c
*
* @purpose ISCSI component functions
*
* @component ISCSI
*
* @comments none
*
* @create 04/18/2008
*
* @end
*
**********************************************************************/
#include "dtlapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "sysnet_api_ipv4.h"
#include "l7_packet.h"
#include "buff_api.h"
#include "iscsi.h"
#include "iscsi_db.h"
#include "iscsi_cnfgr.h"
#include "iscsi_api.h"
#include "dot1q_api.h"
#include "iscsi_sid.h"
#include "iscsi_packet.h"
#include "dtl_iscsi.h"

extern iscsiCfgData_t  *iscsiCfgData;

void                   *iscsiQueue        = L7_NULLPTR;
L7_int32                iscsiTaskId;
osapiRWLock_t           iscsiRWLock;
L7_uint32               iscsiDebug = 0;

void iscsiImpConnectionDelete(L7_uint32 connectionId);
void iscsiImpSessionDelete(L7_uint32 sessionId);


/*********************************************************************
* @purpose  Applies the global administrative mode iSCSI component
*
* @param  L7_uint32 adminMode      (input) Global enable mode for iSCSI
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiAdminModeApply(L7_uint32 mode)
{
  L7_uint32 i, cosQueue, tagMode, tagData;
  sysnetPduIntercept_t sysnetPduIntercept;

  if (mode == L7_ENABLE)
  {
    if (iscsiImpCosDataGet(&cosQueue, &tagMode, &tagData) != L7_SUCCESS)
    {
      /* failed to retrieve needed CoS data from configuration, indicates some internal error */
      return L7_FAILURE;
    }

    /* install packet trapping rules for each port/target-address in the target table */
    for (i=0; i<L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT; i++)
    {
      if ((iscsiCfgData->iscsiTargetCfgData[i].inUse == L7_TRUE) &&
          (iscsiCfgData->iscsiTargetCfgData[i].deletedDefaultCfgEntry == L7_FALSE))
      {
        if (iscsiImpTargetTcpPortAdd(iscsiCfgData->iscsiTargetCfgData[i].portNumber, 
                                     iscsiCfgData->iscsiTargetCfgData[i].ipAddress,
                                     cosQueue, 
                                     iscsiCfgData->markingEnabled,
                                     tagMode, tagData) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
      }
    }
    /* start the iscsi time-out timer */
    if (iscsiTimerAdd() != L7_SUCCESS)
    {
      L7_LOG(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID, 
             "Error starting iSCSI timer");
      return L7_FAILURE;
    }

    sysnetPduIntercept.addressFamily   = L7_AF_INET;
    sysnetPduIntercept.hookId          = SYSNET_INET_RECV_IN;
    sysnetPduIntercept.hookPrecedence  = FD_SYSNET_HOOK_ISCSI_PRECEDENCE;
    sysnetPduIntercept.interceptFunc   = iscsiPktIntercept;
    osapiStrncpySafe((L7_char8 *)sysnetPduIntercept.interceptFuncName, "iscsiPktIntercept", 
                     sizeof(sysnetPduIntercept.interceptFuncName));

    /* Register with sysnet */
    if (sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS)
    {
      L7_LOG(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID, 
             "Error registering iSCSI with sysNetPduInterceptRegister()");
      return L7_FAILURE;
    }
  }
  else if (mode == L7_DISABLE)
  {
    sysnetPduIntercept.addressFamily   = L7_AF_INET;
    sysnetPduIntercept.hookId          = SYSNET_INET_RECV_IN;
    sysnetPduIntercept.hookPrecedence  = FD_SYSNET_HOOK_ISCSI_PRECEDENCE;
    sysnetPduIntercept.interceptFunc   = iscsiPktIntercept;
    osapiStrncpySafe((L7_char8 *)sysnetPduIntercept.interceptFuncName, "iscsiPktIntercept", 
                     sizeof(sysnetPduIntercept.interceptFuncName));

    (void)sysNetPduInterceptDeregister(&sysnetPduIntercept);

    /* stop the timer */
    iscsiTimerDelete();
    /* clear the session table and remove any installed connection rules */
    iscsiSessionDataClear();
    for (i=0; i<L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT; i++)
    {
      if ((iscsiCfgData->iscsiTargetCfgData[i].inUse == L7_TRUE) &&
          (iscsiCfgData->iscsiTargetCfgData[i].deletedDefaultCfgEntry == L7_FALSE))
      {
        if (iscsiImpTargetTcpPortDelete(i) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verifies the timeout parameter is in valid range
*
* @param  L7_uint32 seconds      (input)  seconds
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiTimeOutIntervalRangeCheck(L7_uint32 seconds)
{
  return(((seconds >= ISCSI_SESSION_TIME_OUT_SECONDS_MIN) && (seconds <= ISCSI_SESSION_TIME_OUT_SECONDS_MAX)) ? L7_SUCCESS : L7_FAILURE);
}

/*********************************************************************
* @purpose  Configures the session time out value used for iSCSI sessions
*
* @param  L7_uint32 seconds   (input) number of seconds to wait before
*                             deleting a dormant iSCSI session  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiTimeOutIntervalApply(L7_uint32 seconds)
{
#ifdef ISCSI_IMMEDIATELY_AGE_OUT_SESSIONS_ON_CONFIG_CHANGE
  L7_int32 sessionId = -1;
  L7_uint32 silentTime;

  (void)osapiWriteLockTake(iscsiRWLock, L7_WAIT_FOREVER);

  while (iscsiDbSessionIterate(sessionId, &sessionId) == L7_SUCCESS)
  {
    if ((iscsiDbSessionSilentTimeGet(sessionId, &silentTime) == L7_SUCCESS) &&
        (silentTime > seconds))
    {
      /* delete session and all its connections */
      iscsiImpSessionDelete(sessionId);
    }
  }

  (void)osapiWriteLockGive(iscsiRWLock);
#endif
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configures whether iSCSI session packets will be marked
*
* @param  L7_uint32 adminMode      (input) tagging enable mode for iSCSI
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiMarkingModeApply(L7_uint32 mode)
{
  /* currently no processing required, placeholder function */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verifies the VLAN priority parameter is in valid range
*
* @param  L7_uint32 priority      (input)  VLAN tag priority 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiVlanPriorityRangeCheck(L7_uint32 priority)
{
  return(((priority >= L7_DOT1P_MIN_PRIORITY) && (priority <= L7_DOT1P_MAX_PRIORITY)) ? L7_SUCCESS : L7_FAILURE);
}

/*********************************************************************
* @purpose  Configures the VLAN priority for treatment of iSCSI packets
*
* @param  L7_uint32 priority      (input)  VLAN tag priority 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiVlanPriorityApply(L7_uint32 priority)
{
  /* currently no processing required, placeholder function */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verifies the IP DSCP parameter is in valid range
*
* @param  L7_uint32 dscp      (input)  DSCP
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiDscpRangeCheck(L7_uint32 dscp)
{
  return(((dscp >= L7_ACL_MIN_DSCP) && (dscp <= L7_ACL_MAX_DSCP)) ? L7_SUCCESS : L7_FAILURE);
}

/*********************************************************************
* @purpose  Configures the IP DSCP for treatment of iSCSI packets
*
* @param  L7_uint32 dscp      (input)  DSCP
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiDscpApply(L7_uint32 dscp)
{
  /* currently no processing required, placeholder function */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verifies the IP precedence parameter is in valid range
*
* @param  L7_uint32 prec      (input)  precedence
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiIpPrecRangeCheck(L7_uint32 prec)
{
  return(((prec >= L7_ACL_MIN_PRECEDENCE) && (prec <= L7_ACL_MAX_PRECEDENCE)) ? L7_SUCCESS : L7_FAILURE);
}

/*********************************************************************
* @purpose  Configures the IP precedence for treatment of iSCSI packets
*
* @param  L7_uint32 prec      (input)  precedence
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiIpPrecApply(L7_uint32 prec)
{
  /* currently no processing required, placeholder function */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verifies the tag field selector parameter is in valid range
*
* @param  L7_uint32 selector      (input)  selector
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiTagFieldRangeCheck(L7_uint32 selector)
{
  return(((selector >= L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P) && (selector <= L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)) ? L7_SUCCESS : L7_FAILURE);
}

/*********************************************************************
* @purpose  Configures the tag field selector for treatment of iSCSI packets
*
* @param  L7_uint32 selector      (input)  selector
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiTagFieldApply(L7_uint32 selector)
{
  /* currently no processing required, placeholder function */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Finds and returns the next free entry in the target tcp port
*           table if available.
*
* @param    entryId     @b{(output)} identifier of available entry 
*
* @returns  L7_SUCCESS  if available entry found
* @returns  L7_FAILURE  if table is full
*
* @comments The entryId value remains free until used in a subsequent tcpPortAdd
*           request.  When two or more requestors get the same entryId value,
*           only the first one to successfully create a new entry
*           gets to keep it.  All other requestors' create requests will
*           fail due to the entryId being in use, so they must call this
*           function again to get a new value.
*
* @end
*********************************************************************/
L7_RC_t iscsiImpTargetTcpPortFreeEntryGet(L7_uint32 *entryId)
{
  L7_uint32 index;
  L7_RC_t   rc = L7_FAILURE;

  if (entryId != L7_NULLPTR)
  {
    /* look for available space in config structure */
    if (iscsiCfgData->numActiveTargetEntries < L7_ISCSI_MAX_TARGET_TCP_PORTS)
    {
      for (index=0; index<L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT; index++)
      {
        if (iscsiCfgData->iscsiTargetCfgData[index].inUse != L7_TRUE)
        {
          break;
        }
      }

      /* if we have space... if so, add to application and update config if successful */
      if (index < L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT)
      {
        *entryId = index;
        rc =  L7_SUCCESS;
      }
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Add TCP port and address to monitor for iSCSI protocol packets
*
* @param    tcpPortNumber   @b{(input)} target's TCP port 
* @param    ipAddress       @b{(input)} target's IP address
* @param    cosQueue        @b{(input)} CoS queue to assign protocol traffic 
* @param    taggingEnabled  @b{(input)} indicates if packets should be retagged
* @param    tagMode         @b{(input)} indicates how iSCSI data for this TCP port/address
*                                       should be retagged
* @param    tagData         @b{(input)} the value used to retag the packets if enabled 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @note     Values for tagMode are taken from the enum L7_QOS_COS_MAP_INTF_MODE_t.  If taggingEnabled is false,
*           retagging of the packets is not done and the tagData and tagMode parameters
*           are ignored.
*
* @end
*********************************************************************/
L7_RC_t iscsiImpTargetTcpPortAdd(L7_uint32 tcpPort, L7_uint32 ipAddress, L7_uint32 cosQueue, 
                                 L7_BOOL taggingEnabled, L7_QOS_COS_MAP_INTF_MODE_t tagMode, L7_uint32 tagData)
{
  if (dtlIscsiTargetPortAdd(tcpPort, ipAddress, cosQueue, taggingEnabled, tagMode, tagData) == L7_SUCCESS)
    return L7_SUCCESS;
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_QOS_ISCSI_COMPONENT_ID, 
            "Error adding iSCSI target TCP port/address: tcpPort = %u, ipAddress=%X, cosQueue=%u, ",
            "taggingEnabled=%u, tagMode=%u, tagData=%u", 
            tcpPort, ipAddress, cosQueue, taggingEnabled, tagMode, tagData);
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Removes the TCP ports and (optionally) IP addresses for
*           traffic to monitor for iSCSI session setup packets.  The
*           target name can be configured for user convenience.
*
* @param  L7_uint32 tcpPort    (input) TCP port to be monitored  
* @param  L7_uint32 ipAddress  (input) IP address to be monitored on this port
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*           L7_ERROR
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpTargetTcpPortDelete(L7_uint32 entryId)
{
  L7_RC_t   rc = L7_ERROR;
  L7_int32  connectionId = -1;
  L7_uint32 connectionTcpPortId;

  (void)osapiWriteLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  if ((entryId < L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT) &&
      (iscsiCfgData->iscsiTargetCfgData[entryId].inUse) && 
      (iscsiCfgData->iscsiTargetCfgData[entryId].deletedDefaultCfgEntry == L7_FALSE))
  {
    /* remove classifier rule trapping iSCSI protocol frames from this TCP port/IP address */
    if ((rc = dtlIscsiTargetPortDelete(iscsiCfgData->iscsiTargetCfgData[entryId].portNumber, 
                                       iscsiCfgData->iscsiTargetCfgData[entryId].ipAddress)) == L7_SUCCESS)
    {
      /* clean out any entries in the connection table that were detected due to monitoring this target port/IP address */
      while (iscsiDbConnectionIterate(connectionId, &connectionId) == L7_SUCCESS)
      {
        if ((connectionTcpPortId = iscsiDbConnectionTargetTcpPortIdGet(connectionId)) < 0)
        {
          L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID, 
                  "Error retrieving connection target table ID. connectionId=%d", connectionId);
        }
        else
        {
          if (entryId == connectionTcpPortId)
          {
            iscsiImpConnectionDelete(connectionId);
          }
        }
      }
    }
  }
  (void)osapiWriteLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Finds the entry in the Target TCP port table the matches
*           the specified port number and IP address.
*
* @param  L7_uint32 port  (input) TCP port number 
* @param  L7_uint32 ip_addr  (input) IP address
* @param  L7_uint32 *entryId  (output) the entry identifier, if found
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpTargetTableLookUp(L7_uint32 port, L7_uint32 ip_addr, L7_uint32 *index)
{
  L7_uint32 i;

  for (i=0; i<L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT; i++)
  {
    if ((iscsiCfgData->iscsiTargetCfgData[i].inUse == L7_TRUE) &&
        (iscsiCfgData->iscsiTargetCfgData[i].ipAddress == ip_addr) &&
        (iscsiCfgData->iscsiTargetCfgData[i].portNumber == port))
    {
        break;
      }
    }

  if ((i < L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT)  &&
      (iscsiCfgData->iscsiTargetCfgData[i].deletedDefaultCfgEntry == L7_FALSE))
  {
    *index = i;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Finds the entry in the connection table the matches
*           the specified data.
*
* @param  L7_uint32 targetIpAddress  
* @param  L7_uint32 targetTcpPort    
* @param  L7_uint32 initiatorIpAddress
* @param  L7_uint32 initiatorTcpPort  
* @param  L7_uint32 cid              
*
* @returns  L7_SUCCESS  entry found with no errors 
* @returns  L7_FAILURE  no matching entry found
*
* @notes    None
*
* @end
*********************************************************************/
L7_int32 iscsiConnectionLookup(L7_uint32 targetIpAddress,
                               L7_uint32 targetTcpPort,
                               L7_uint32 initiatorIpAddress,
                               L7_uint32 initiatorTcpPort)
{
  L7_int32 connectionId;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  connectionId = iscsiDbConnectionRecordLookup(targetIpAddress,targetTcpPort,initiatorIpAddress,initiatorTcpPort);
  (void)osapiReadLockGive(iscsiRWLock);

  return(connectionId);
}

/*********************************************************************
* @purpose  Creates a connection for the data provided.
*
* @param  L7_uchar8 *targetName,        
* @param  L7_uchar8 *initiatorName     
* @param  L7_uint32 isid
* @param  L7_uint32 targetIpAddress
* @param  L7_uint32 targetTcpPort     
* @param  L7_uint32 initiatorIpAddress
* @param  L7_uint32 initiatorTcpPort  
* @param  L7_uint32 cid    
* @param  L7_uint32 targetTcpPortTableId
*
* @returns  L7_SUCCESS  entry created with no errors 
* @returns  L7_FAILURE  entry not created
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionCreate(L7_uchar8 *targetName,
                              L7_uchar8 *initiatorName,
                              L7_uchar8 *isid,
                              L7_uint32 targetIpAddress,
                              L7_uint32 targetTcpPort,
                              L7_uint32 initiatorIpAddress,
                              L7_uint32 initiatorTcpPort,
                              L7_uint32 cid,
                              L7_uint32 targetTcpPortTableId)
{
  L7_RC_t rc;
  L7_int32 connectionId;
  L7_int32 sessionId;
  L7_uint32 cosQueue, tagMode, tagData;

  (void)osapiWriteLockTake(iscsiRWLock, L7_WAIT_FOREVER);

  if ((rc = iscsiDbConnectionCreate(targetName, initiatorName, isid, targetIpAddress, targetTcpPort, 
                                    initiatorIpAddress, initiatorTcpPort, cid, &connectionId, &sessionId, targetTcpPortTableId)) == L7_SUCCESS)
  {
    /* retrieve queue and tagging data */
    if ((rc = iscsiImpCosDataGet(&cosQueue, &tagMode, &tagData)) == L7_SUCCESS)
    {
      /* update hardware */
      /* use sessionIndex as counter identifer to unify counts for all connections in session in driver */
      if ((rc = dtlIscsiConnectionAdd(connectionId, targetIpAddress, targetTcpPort, initiatorIpAddress, initiatorTcpPort, 
                                      sessionId, cosQueue, iscsiCfgData->markingEnabled, tagMode, tagData)) != L7_SUCCESS)
      {
        L7_LOG(L7_LOG_SEVERITY_ERROR, L7_FLEX_QOS_ISCSI_COMPONENT_ID, 
               "Error returned from dtlIscsiConnectionAdd().");
        iscsiDbConnectionDelete(connectionId);
      }
    }
  }
  (void)osapiWriteLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Deletes a connection.
*
* @param   L7_uint32 connectionId  (input) identifier of connection to be
*                                       deleted
*
* @returns  L7_SUCCESS  entry deleted with no errors 
* @returns  L7_FAILURE  entry not deleted
*
* @notes    None
*
* @end
*********************************************************************/
void iscsiImpConnectionDelete(L7_uint32 connectionId)
{
  /* remove connection data from driver */
  if (dtlIscsiConnectionRemove(connectionId) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_FLEX_QOS_ISCSI_COMPONENT_ID, 
           "Error returned from dtlIscsiConnectionRemove().");
  }
  iscsiDbConnectionDelete(connectionId);
}

/*********************************************************************
* @purpose  Deletes a connection.
*
* @param   L7_uint32 connectionId  (input) identifier of connection to be
*                                       deleted
*
* @returns  L7_SUCCESS  entry deleted with no errors 
* @returns  L7_FAILURE  entry not deleted
*
* @notes    None
*
* @end
*********************************************************************/
void iscsiConnectionDelete(L7_uint32 connectionId)
{
  (void)osapiWriteLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  iscsiImpConnectionDelete(connectionId);
  (void)osapiWriteLockGive(iscsiRWLock);
}

/*********************************************************************
* @purpose  Deletes a session.
*
* @param   L7_uint32 sessionId  (input) identifier of session to be
*                                       deleted
*
* @returns  L7_SUCCESS  entry deleted with no errors 
* @returns  L7_FAILURE  entry not deleted
*
* @notes    None
*
* @end
*********************************************************************/
void iscsiImpSessionDelete(L7_uint32 sessionId)
{
  L7_int32                 i = -1;

  if (iscsiDbSessionIdInUse(sessionId) == L7_TRUE)
  {
    while (iscsiDbConnectionIterate(i, &i) == L7_SUCCESS)
    {
      if (iscsiDbConnectionSessionIdGet(i) == sessionId)
      {
        /* delete this connection */
        iscsiImpConnectionDelete(i);
      }
    }
  }
}

/*********************************************************************
* @purpose  Deletes a session.
*
* @param   L7_uint32 sessionId  (input) identifier of session to be
*                                       deleted
*
* @returns  L7_SUCCESS  entry deleted with no errors 
* @returns  L7_FAILURE  entry not deleted
*
* @notes    None
*
* @end
*********************************************************************/
void iscsiSessionDelete(L7_uint32 sessionId)
{
  (void)osapiWriteLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  iscsiImpSessionDelete(sessionId);
  (void)osapiWriteLockGive(iscsiRWLock);
}

/*********************************************************************
* @purpose  Retrieves configuration data specifying which priority tag
*           to use and whether retagging is enabled for handling QoS
*           for iSCSI traffic.
*
* @param   L7_uint32 *cosQueue  (output) queue for assigning iSCSI traffic
* @param   L7_QOS_COS_MAP_INTF_MODE_t *tagMode (output) which field to use for QoS of iSCSI traffic
* @param   L7_uint32  *tagData  (output) what tagging data is to be applied to the traffic for marking
*
* @returns  L7_SUCCESS  dta retrieved with no errors 
* @returns  L7_ERROR    configuration error
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpCosDataGet(L7_uint32 *cosQueue, L7_QOS_COS_MAP_INTF_MODE_t *tagMode, L7_uint32 *tagData)
{
  L7_uint32 value;
  L7_RC_t   rc;

  switch (iscsiCfgData->tagFieldSelector)
  {
  case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
    if ((rc = dot1dPortTrafficClassGet(L7_ALL_INTERFACES, iscsiCfgData->vlanPriorityValue, &value)) == L7_SUCCESS)
    {
      *cosQueue = value;
      *tagData = iscsiCfgData->vlanPriorityValue;
    }
    break;
  case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
    if ((rc = cosMapIpPrecTrafficClassGet(L7_ALL_INTERFACES, iscsiCfgData->precValue, &value)) == L7_SUCCESS)
    {
      *cosQueue = value;
      *tagData = iscsiCfgData->precValue;
    }
    break;
  case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
    if ((rc = cosMapIpDscpTrafficClassGet(L7_ALL_INTERFACES, iscsiCfgData->dscpValue, &value)) == L7_SUCCESS)
    {
      *cosQueue = value;
      *tagData = iscsiCfgData->dscpValue;
    }
    break;
  default:
    {
      rc = L7_ERROR;
    }
  }

  if (rc == L7_SUCCESS)
  {
    *tagMode = iscsiCfgData->tagFieldSelector;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_QOS_ISCSI_COMPONENT_ID, 
            "Error retrieving CoS queue data: iscsiCfgData->tagFieldSelector=%u", iscsiCfgData->tagFieldSelector);
  }
  return(rc);
}

/*********************************************************************
* @purpose  Checks whether an entry associated with the given session
*           identifier is in use.
*
* @param  L7_uint32 sessionId    (input) an identifier of a
*                                table entry.  
*
* @returns  L7_SUCCESS  session ID is in use 
* @returns  L7_FAILURE  session ID is not in use
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpSessionTableIdIsValid(L7_uint32 sessionId)
{
  L7_RC_t rc;
  L7_BOOL status;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  status = iscsiDbSessionIdInUse(sessionId);
  (void)osapiReadLockGive(iscsiRWLock);
  if (status == L7_TRUE)
  {
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the first entry in the iSCSI session table.
*           Entries are returned in a sorted order by target IQN, 
*           iniator IQN, and session ISID.
*
* @param  L7_uint32 *sessionId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpSessionFirstGet(L7_uint32 *sessionId)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbSessionFirstGet(sessionId);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the next entry in the iSCSI session table.
*           Entries are returned in a sorted order by target IQN, 
*           iniator IQN, and session ISID.
*
* @param  L7_uint32 sessionId  (input) an identifier of the current table entry
* @param  L7_uint32 *nextSessionId  (output) an identifier of the retrieved
*                                table entry.  
*
* @returns  L7_SUCCESS  table not empty and next entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpSessionNextGet(L7_uint32 sessionId, L7_uint32 *nextSessionId)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbSessionNextGet(sessionId, nextSessionId);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the next entry in the iSCSI session table.
*           Entries are returned in order of increasing sessionId.
*
* @param  L7_uint32 sessionId  (input) an identifier of the current table entry
* @param  L7_uint32 *nextSessionId  (output) an identifier of the retrieved
*                                table entry.  
*
* @returns  L7_SUCCESS  table not empty and next entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpSessionNextByEntryIdGet(L7_uint32 sessionId, L7_uint32 *nextSessionId)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbSessionIterate((L7_int32) sessionId, (L7_int32 *) nextSessionId);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the target IQN for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uchar8 *name     (output) the IQN 
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpSessionTargetNameGet(L7_uint32 sessionId, L7_uchar8 *name)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbSessionTargetNameGet(sessionId, name);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the initiator IQN for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uchar8 *name     (output) the IQN 
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpSessionInitiatorNameGet(L7_uint32 sessionId, L7_uchar8 *name)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbSessionInitiatorNameGet(sessionId, name);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the start time for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_clocktime *time   (output) the system time when the session 
*                                was detected
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpSessionStartTimeGet(L7_uint32 sessionId, L7_clocktime *time)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbSessionStartTimeGet(sessionId, time);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the time since last data traffic for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uint32 *seconds   (output) seconds since last seen data for session 
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpSessionSilentTimeGet(L7_uint32 sessionId, L7_uint32 *seconds)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbSessionSilentTimeGet(sessionId, seconds);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the ISID for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uchar8 *isid      (output) the ISID associated with the specified
*                                 session
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpSessionIsidGet(L7_uint32 sessionId, L7_uchar8 *isid)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbSessionIsidGet(sessionId, isid);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}


/*********************************************************************
* @purpose  Checks whether an entry associated with the given connection
*           identifier is in use.
*
* @param  L7_uint32 connectionId    (input) an identifier of a
*                                table entry.  
*
* @returns  L7_SUCCESS  connection ID is in use 
* @returns  L7_FAILURE  connection ID is not in use
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpConnectionTableIdIsValid(L7_uint32 connectionId)
{
  L7_RC_t rc;
  L7_BOOL status;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  status = iscsiDbConnectionIdInUse(connectionId);
  (void)osapiReadLockGive(iscsiRWLock);
  if (status == L7_TRUE)
  {
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the first entry in the iSCSI connection table 
*           associated with the specified session.
*           Entries are returned in a sorted order by initiator IP
*           address, initiator TCP port, target IP address, target TCP
*           port, iSCSI CID (connection ID)
*
* @param  L7_uint32 sessionId  (input) an identifier of the session table entry
* @param  L7_uint32 *connectionId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or sessionId not found 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpConnectionFirstGet(L7_uint32 sessionId, L7_uint32 *connectionId)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbConnectionFirstGet(sessionId, connectionId);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the next entry in the iSCSI connection table 
*           associated with the specified session.
*           Entries are returned in a sorted order by initiator IP
*           address, initiator TCP port, target IP address, target TCP
*           port, iSCSI CID (connection ID)
*
* @param  L7_uint32 sessionId  (input) an identifier of the session table entry
* @param  L7_uint32 connectionId  (input) an identifier of the current connection table entry
* @param  L7_uint32 *nextConnectionId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or sessionId not found 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpConnectionNextGet(L7_uint32 sessionId, L7_uint32 connectionId, L7_uint32 *nextConnectionId)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbConnectionNextGet(sessionId, connectionId, nextConnectionId);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the next entry in the iSCSI connection table.
*           Entries are returned in order of increasing connectionId.
*
* @param  L7_uint32 connectionId  (input) an identifier of the current table entry
* @param  L7_uint32 *nextConnectionId  (output) an identifier of the retrieved
*                                table entry.  
*
* @returns  L7_SUCCESS  table not empty and next entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpConnectionNextByEntryIdGet(L7_uint32 connectionId, L7_uint32 *nextConnectionId)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbConnectionIterate((L7_int32) connectionId, (L7_int32 *) nextConnectionId);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the target IP address for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *ipAddr  (output) the IP address associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpConnectionTargetIpAddressGet(L7_uint32 connectionId, L7_uint32 *ipAddr)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbConnectionTargetIpAddressGet(connectionId, ipAddr);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the initiator IP address for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *ipAddr  (output) the IP address associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpConnectionInitiatorIpAddressGet(L7_uint32 connectionId, L7_uint32 *ipAddr)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbConnectionInitiatorIpAddressGet(connectionId, ipAddr);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the target TCP port for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *port         (output) the TCP port associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpConnectionTargetTcpPortGet(L7_uint32 connectionId, L7_uint32 *port)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbConnectionTargetTcpPortGet(connectionId, port);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the initiator TCP port for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *port         (output) the TCP port associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpConnectionInitiatorTcpPortGet(L7_uint32 connectionId, L7_uint32 *port)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbConnectionInitiatorTcpPortGet(connectionId, port);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the iSCSI connection id (CID) for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *cid         (output) the CID associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpConnectionCidGet(L7_uint32 connectionId, L7_uint32 *cid)
{
  L7_RC_t rc;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbConnectionCidGet(connectionId, cid);
  (void)osapiReadLockGive(iscsiRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the iSCSI Session table entry id for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *sessionId    (output) the CID associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiImpConnectionSessionIdGet(L7_uint32 connectionId, L7_uint32 *sessionId)
{
  L7_RC_t rc;
  L7_int32 value;

  (void)osapiReadLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  value = iscsiDbConnectionSessionIdGet(connectionId);
  (void)osapiReadLockGive(iscsiRWLock);

  if (value < 0)
  {
    rc = L7_FAILURE;
  }
  else
  {
    *sessionId = value;
    rc = L7_SUCCESS;
  }

  return(rc);
}

/*********************************************************************
* @purpose  Deletes all iSCSI session and connection data.
*
*
* @notes    None
*
* @end
*********************************************************************/
void iscsiSessionDataClear(void)
{
  L7_int32    i = -1;

  (void)osapiWriteLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  while (iscsiDbConnectionIterate(i, &i) == L7_SUCCESS)
  {
    /* delete this connection */
    iscsiImpConnectionDelete(i);
  }
  (void)osapiWriteLockGive(iscsiRWLock);
}

/*********************************************************************
* @purpose  Allocates and initializes all tables for the iSCSI session
*           database.
* @end
*********************************************************************/
L7_RC_t iscsiSessionDataBaseCreate(void)
{
  L7_RC_t rc;

  (void)osapiWriteLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  rc = iscsiDbSessionDataStructureCreate();
  (void)osapiWriteLockGive(iscsiRWLock);

  return(rc);
}

/*********************************************************************
* @purpose  Frees and initializes all tables for the iSCSI session
*           database.
* @end
*********************************************************************/
void iscsiSessionDataBaseFree(void)
{
  (void)osapiWriteLockTake(iscsiRWLock, L7_WAIT_FOREVER);
  iscsiDbSessionDataStructureDestroy();
  (void)osapiWriteLockGive(iscsiRWLock);

  return;
}

