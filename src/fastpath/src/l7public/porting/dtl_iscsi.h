/*********************************************************************
*                                                                 
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_iscsi.h
*
* Purpose: This file contains the functions to setup iSCSI
*          connection calls to the driver.
*
* Component: Device Transformation Layer (DTL)
*
* Comments: 
*
*********************************************************************/
#ifndef DTL_ISCSI_H
#define DTL_ISCSI_H

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
                              L7_BOOL taggingEnabled, L7_QOS_COS_MAP_INTF_MODE_t tagMode, L7_uint32 tagData);

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
L7_RC_t dtlIscsiTargetPortDelete(L7_uint32 tcpPortNumber, L7_uint32 ipAddress);

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
                              L7_BOOL taggingEnabled, L7_QOS_COS_MAP_INTF_MODE_t tagMode, L7_uint32 tagData);

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
L7_RC_t dtlIscsiConnectionRemove(L7_uint32 identifier);

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
L7_RC_t dtlIscsiConnectionCounterGet(L7_uint32 counterIdentifier, L7_ulong64 *countData);
#endif

