/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_acl.c
*
* Purpose: This file contains the functions to setup access list
*          management calls to the driver.
*
* Component: Device Transformation Layer (DTL)
*
* Comments: 
*
* Created by: djohnson 6/24/2002 
*
*********************************************************************/


#include "dtlinclude.h"
#include "acl_api.h"
#include "unitmgr_api.h"

/*********************************************************************
* @purpose  Create a new access list
*
* @param    intIfNum       @b{(input)} Interface number to apply to
* @param    direction      @b{(input)} Direction to filter traffic
* @param    pTLV           @b{(input)} ptr to first type-length pair
* @param    tlvTotalSize   @b{(input)} number of TLVs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Adding a (aclnum, interface, direction) instance
*
* @end
*********************************************************************/
L7_RC_t dtlQosAclCreate(L7_uint32 intIfNum, L7_uint32 direction, 
                        L7_uchar8 *pTLV, L7_uint32 tlvTotalSize)
{
  DAPI_QOS_CMD_t dapiCmd;
  DAPI_USP_t     ddUsp;
  L7_RC_t  rc;
  nimUSP_t       nimUsp;
  
  if (nimGetUnitSlotPort(intIfNum, &nimUsp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = (L7_ushort16)nimUsp.unit;
  ddUsp.slot = (L7_ushort16)nimUsp.slot;  
  ddUsp.port = (L7_ushort16)nimUsp.port-1;
  
  dapiCmd.cmdData.aclInstAdd.getOrSet = DAPI_CMD_SET;
  
  if (direction == L7_INBOUND_ACL)
    dapiCmd.cmdData.aclInstAdd.ifDirection = DAPI_QOS_INTF_DIR_IN;
  else
    dapiCmd.cmdData.aclInstAdd.ifDirection = DAPI_QOS_INTF_DIR_OUT;

  dapiCmd.cmdData.aclInstAdd.pTLV = (L7_tlv_t *)pTLV;
  
  dapiCmd.cmdData.aclInstAdd.tlvTotalSize = tlvTotalSize;
  
  rc = dapiCtl(&ddUsp,DAPI_CMD_QOS_ACL_ADD,&dapiCmd);
  
  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
    return L7_SUCCESS;
  else
    return L7_FAILURE;
  
}

/*********************************************************************
* @purpose  Delete an access list
*
* @param    intIfNum       @b{(input)} Interface number
* @param    direction      @b{(input)} Direction
* @param    pTLV           @b{(input)} ptr to first type-length pair
* @param    tlvTotalSize   @b{(input)} number of TLVs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Removes an (aclnum, interface, direction) instance
*
* @end
*********************************************************************/
L7_RC_t dtlQosAclDelete(L7_uint32 intIfNum, L7_uint32 direction, L7_uchar8 *pTLV, L7_uint32 tlvTotalSize)
{
  DAPI_QOS_CMD_t dapiCmd;
  DAPI_USP_t     ddUsp;
  L7_RC_t  rc;
  nimUSP_t       nimUsp;
  
  if (nimGetUnitSlotPort(intIfNum, &nimUsp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = (L7_ushort16)nimUsp.unit;
  ddUsp.slot = (L7_ushort16)nimUsp.slot;  
  ddUsp.port = (L7_ushort16)nimUsp.port-1;
  
  dapiCmd.cmdData.aclInstDelete.getOrSet     = DAPI_CMD_SET;
  
  if (direction == L7_INBOUND_ACL)
    dapiCmd.cmdData.aclInstDelete.ifDirection = DAPI_QOS_INTF_DIR_IN;
  else
    dapiCmd.cmdData.aclInstDelete.ifDirection = DAPI_QOS_INTF_DIR_OUT;
  
  dapiCmd.cmdData.aclInstDelete.pTLV         = (L7_tlv_t *)pTLV;
  
  dapiCmd.cmdData.aclInstDelete.tlvTotalSize = tlvTotalSize;
  
  rc = dapiCtl(&ddUsp,DAPI_CMD_QOS_ACL_DELETE,&dapiCmd);
  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Create a new access list
*
* @param    direction      @b{(input)} Direction to filter traffic
* @param    pTLV           @b{(input)} ptr to first type-length pair
* @param    tlvTotalSize   @b{(input)} number of TLVs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Adding a (aclnum, direction) instance
*
* @end
*********************************************************************/
L7_RC_t dtlQosSysAclCreate(L7_uint32 direction, L7_uchar8 *pTLV, L7_uint32 tlvTotalSize )
{
  DAPI_QOS_CMD_t dapiCmd;
  DAPI_USP_t     ddUsp;
  L7_RC_t        rc;

  ddUsp.unit = 0;
  ddUsp.slot = 0;
  ddUsp.port = 0;

  dapiCmd.cmdData.aclInstAdd.getOrSet = DAPI_CMD_SET;

  if (L7_INBOUND_ACL == direction)
  {
    dapiCmd.cmdData.aclInstAdd.ifDirection = DAPI_QOS_INTF_DIR_IN;
  }
  else
  {
    dapiCmd.cmdData.aclInstAdd.ifDirection = DAPI_QOS_INTF_DIR_OUT;
  }

  dapiCmd.cmdData.aclInstAdd.pTLV = (L7_tlv_t *)pTLV;

  dapiCmd.cmdData.aclInstAdd.tlvTotalSize = tlvTotalSize;

  rc = dapiCtl(&ddUsp, DAPI_CMD_QOS_SYS_ACL_ADD, &dapiCmd);

  if ((L7_SUCCESS         == rc) ||
      (L7_ASYNCH_RESPONSE == rc ))
  {
    return(L7_SUCCESS);
  }
  else
  {
    return(L7_FAILURE);
  }
}

/*********************************************************************
* @purpose  Delete an access list
*
* @param    direction      @b{(input)} Direction
* @param    pTLV           @b{(input)} ptr to first type-length pair
* @param    tlvTotalSize   @b{(input)} number of TLVs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Removes an (aclnum, direction) instance
*
* @end
*********************************************************************/
L7_RC_t dtlQosSysAclDelete(L7_uint32 direction, L7_uchar8 *pTLV, L7_uint32 tlvTotalSize)
{
  DAPI_QOS_CMD_t dapiCmd;
  DAPI_USP_t     ddUsp;
  L7_RC_t        rc;

  ddUsp.unit = 0;
  ddUsp.slot = 0;
  ddUsp.port = 0;

  dapiCmd.cmdData.aclInstDelete.getOrSet = DAPI_CMD_SET;

  if (L7_INBOUND_ACL == direction)
  {
    dapiCmd.cmdData.aclInstDelete.ifDirection = DAPI_QOS_INTF_DIR_IN;
  }
  else
  {
    dapiCmd.cmdData.aclInstDelete.ifDirection = DAPI_QOS_INTF_DIR_OUT;
  }

  dapiCmd.cmdData.aclInstDelete.pTLV = (L7_tlv_t *)pTLV;

  dapiCmd.cmdData.aclInstDelete.tlvTotalSize = tlvTotalSize;

  rc = dapiCtl(&ddUsp, DAPI_CMD_QOS_SYS_ACL_DELETE, &dapiCmd);
  if ((L7_SUCCESS         == rc) ||
      (L7_ASYNCH_RESPONSE == rc))
  {
    return(L7_SUCCESS);
  }
  else
  {
    return(L7_FAILURE);
  }
}

/*********************************************************************
* @purpose  Request an ACL logging rule hit counter from the device
*
* @param    correlator     @b{(input)} ACL rule log correlator
* @param    *pHitCount     @b{(input)} Hit counter output ptr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST   Correlator not found in hardware
*
* @notes    Don't panic if this call fails, since it probably means
*           a correlator known by the application did not get applied
*           in the device (perhaps it ran out of counter resources).
*
* @notes    This call is not for a specific interface.  If the 
*           hardware maintains separate counters per interface for
*           a given correlator, then it is assumed that HAPI will
*           output the sum total value.
*
* @end
*********************************************************************/
L7_RC_t dtlQosAclRuleCountGet(L7_uint32 correlator, L7_ulong64 *pHitCount) 
{
  DAPI_QOS_CMD_t dapiCmd;
  DAPI_USP_t     ddUsp;
  L7_RC_t        rc;
  L7_uint32      unit;

  if (unitMgrNumberGet(&unit) != L7_SUCCESS)
    return L7_FAILURE;

  ddUsp.unit = (L7_int8)unit; 
  ddUsp.slot = 0;
  ddUsp.port = 0;
  
  dapiCmd.cmdData.aclRuleCountGet.getOrSet = DAPI_CMD_GET;
  dapiCmd.cmdData.aclRuleCountGet.correlator = correlator;
  dapiCmd.cmdData.aclRuleCountGet.hitCount = pHitCount;
  
  rc = dapiCtl(&ddUsp, DAPI_CMD_QOS_ACL_RULE_COUNT_GET, &dapiCmd);
  
  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
    return L7_SUCCESS;
  else if (rc == L7_NOT_EXIST)
    return L7_NOT_EXIST;                /* correlator not found in HW */
  else
    return L7_FAILURE;
}
/*********************************************************************
* @purpose  Set the ACL rule status
*
* @param    correlator @b{(input)} Time based ACL rule correlator
* @param    status     @b{(input)} status of the rule
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST   Correlator not found in hardware, rule not installed
*
* @end
*********************************************************************/
L7_RC_t dtlQosAclRuleStatusSet(L7_uint32 correlator, L7_ushort16 status)
{
  DAPI_QOS_CMD_t dapiCmd;
  DAPI_USP_t     ddUsp;
  L7_RC_t        rc;
  L7_uint32      unit;

  if (unitMgrNumberGet(&unit) != L7_SUCCESS)
    return L7_FAILURE;

  ddUsp.unit = (L7_int8)unit;
  ddUsp.slot = 0;
  ddUsp.port = 0;

  dapiCmd.cmdData.aclRuleStatusSet.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.aclRuleStatusSet.correlator = correlator;
  dapiCmd.cmdData.aclRuleStatusSet.status = status;

  rc = dapiCtl(&ddUsp, DAPI_CMD_QOS_ACL_RULE_STATUS_SET, &dapiCmd);

  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
    return L7_SUCCESS;
  else if (rc == L7_NOT_EXIST)
    return L7_NOT_EXIST;                /* correlator not found in HW */
  else
    return L7_FAILURE;
}
