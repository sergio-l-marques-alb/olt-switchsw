/*********************************************************************
*                                                                 
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_iscsi.c
*
* Purpose: This file contains the functions to setup iSCSI
*          management calls to the driver.
*
* Component: Device Transformation Layer (DTL)
*
* Comments: 
*
*********************************************************************/

#include "dtlinclude.h"

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
L7_RC_t dtlIscsiTargetPortAdd(L7_uint32 tcpPortNumber, L7_uint32 ipAddress, L7_uint32 cosQueue, 
                              L7_BOOL taggingEnabled, L7_QOS_COS_MAP_INTF_MODE_t tagMode, L7_uint32 tagData)
{
  DAPI_QOS_CMD_t dapiCmd;
  L7_RC_t  rc;
  DAPI_USP_t     ddUsp;
 
  ddUsp.unit =0;
  ddUsp.slot =0; 
  ddUsp.port =0; 

  dapiCmd.cmdData.iscsiTargetPortAdd.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.iscsiTargetPortAdd.tcpPortNumber = (L7_ushort16)tcpPortNumber;
  dapiCmd.cmdData.iscsiTargetPortAdd.ipAddress = ipAddress;
  dapiCmd.cmdData.iscsiTargetPortAdd.traffic_class = cosQueue;
  dapiCmd.cmdData.iscsiTargetPortAdd.taggingEnabled = taggingEnabled;
  dapiCmd.cmdData.iscsiTargetPortAdd.tagMode = tagMode;
  dapiCmd.cmdData.iscsiTargetPortAdd.tagData = tagData;
  
  rc = dapiCtl(&ddUsp,DAPI_CMD_QOS_ISCSI_TARGET_PORT_ADD,&dapiCmd);
  
  if (rc == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Remove TCP port and address to monitor for iSCSI protocol packets
*
* @param    tcpPortNumber   @b{(input)} target's TCP port 
* @param    ipAddress       @b{(input)} target's IP address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t dtlIscsiTargetPortDelete(L7_uint32 tcpPortNumber, L7_uint32 ipAddress)
{
  DAPI_QOS_CMD_t dapiCmd;
  L7_RC_t  rc;
  DAPI_USP_t     ddUsp;
 
  ddUsp.unit =0;
  ddUsp.slot =0; 
  ddUsp.port =0; 
 
  dapiCmd.cmdData.iscsiTargetPortDelete.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.iscsiTargetPortDelete.tcpPortNumber = tcpPortNumber;
  dapiCmd.cmdData.iscsiTargetPortDelete.ipAddress = ipAddress;
  
  rc = dapiCtl(&ddUsp,DAPI_CMD_QOS_ISCSI_TARGET_PORT_DELETE,&dapiCmd);
  
  if (rc == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Add iSCSI connection
*
* @param    identifier       @b{(input)} unique id for this connection
* @param    targetIpAddress  @b{(input)} target's IP address
* @param    targetTcpPort    @b{(input)} target's TCP port 
* @param    initiatorIpAddress @b{(input)} target's IP address
* @param    initiatorTcpPort   @b{(input)} target's TCP port 
* @param    counterIdentifier  @b{(input)} counter ID for reporting activity on connection
* @param    cosQueue        @b{(input)} CoS queue to assign protocol traffic 
* @param    taggingEnabled  @b{(input)} indicates if packets should be retagged
* @param    tagMode         @b{(input)} indicates if iSCSI data for this TCP port/address
*                                       should be retagged and if so, how
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
L7_RC_t dtlIscsiConnectionAdd(L7_uint32 identifier, L7_uint32 targetIpAddress, L7_uint32 targetTcpPort,
                              L7_uint32 initiatorIpAddress, L7_uint32 initiatorTcpPort,
                              L7_uint32 counterIdentifier, L7_uint32 cosQueue,
                              L7_BOOL taggingEnabled, L7_QOS_COS_MAP_INTF_MODE_t tagMode, L7_uint32 tagData)
{
  DAPI_QOS_CMD_t dapiCmd;
  L7_RC_t  rc;
  DAPI_USP_t     ddUsp;
 
  ddUsp.unit =0;
  ddUsp.slot =0; 
  ddUsp.port =0; 
 
  dapiCmd.cmdData.iscsiConnectionAdd.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.iscsiConnectionAdd.identifier = identifier;
  dapiCmd.cmdData.iscsiConnectionAdd.targetIpAddress = targetIpAddress;
  dapiCmd.cmdData.iscsiConnectionAdd.targetTcpPort = targetTcpPort;
  dapiCmd.cmdData.iscsiConnectionAdd.initiatorIpAddress = initiatorIpAddress;
  dapiCmd.cmdData.iscsiConnectionAdd.initiatorTcpPort = initiatorTcpPort;
  dapiCmd.cmdData.iscsiConnectionAdd.counterIdentifier = counterIdentifier;
  dapiCmd.cmdData.iscsiConnectionAdd.traffic_class = cosQueue;
  dapiCmd.cmdData.iscsiConnectionAdd.taggingEnabled = taggingEnabled;
  dapiCmd.cmdData.iscsiConnectionAdd.tagMode = tagMode;
  dapiCmd.cmdData.iscsiConnectionAdd.tagData = tagData;
  
  rc = dapiCtl(&ddUsp, DAPI_CMD_QOS_ISCSI_CONNECTION_ADD, &dapiCmd);
  
  if (rc == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Remove iSCSI connection
*
* @param    identifier       @b{(input)} unique id for this connection
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t dtlIscsiConnectionRemove(L7_uint32 identifier)
{
  DAPI_QOS_CMD_t dapiCmd;
  L7_RC_t  rc;
  DAPI_USP_t     ddUsp;
 
  ddUsp.unit =0;
  ddUsp.slot =0; 
  ddUsp.port =0; 
 
  dapiCmd.cmdData.iscsiConnectionRemove.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.iscsiConnectionRemove.identifier = identifier;
  
  rc = dapiCtl(&ddUsp, DAPI_CMD_QOS_ISCSI_CONNECTION_REMOVE, &dapiCmd);
  
  if (rc == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Retrieve counter data for iSCSI connection
*
* @param    counterIdentifier  @b{(input)} counter ID for reporting activity on connection
* @param    *countData     @b{(input)} pointer to location where count data is to be stored
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t dtlIscsiConnectionCounterGet(L7_uint32 counterIdentifier, L7_ulong64 *countData)
{
  DAPI_QOS_CMD_t dapiCmd;
  L7_RC_t  rc;
  DAPI_USP_t     ddUsp;
  L7_ulong64     count_data;
 
  ddUsp.unit =0;
  ddUsp.slot =0; 
  ddUsp.port =0; 
 
  dapiCmd.cmdData.iscsiConnectionCounterGet.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.iscsiConnectionCounterGet.identifier = counterIdentifier;
  dapiCmd.cmdData.iscsiConnectionCounterGet.counter_value = &count_data;
  
  rc = dapiCtl(&ddUsp, DAPI_CMD_QOS_ISCSI_COUNTER_GET, &dapiCmd);
  
  if (rc == L7_SUCCESS)
  {
    countData->low = count_data.low;
    countData->high = count_data.high;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

