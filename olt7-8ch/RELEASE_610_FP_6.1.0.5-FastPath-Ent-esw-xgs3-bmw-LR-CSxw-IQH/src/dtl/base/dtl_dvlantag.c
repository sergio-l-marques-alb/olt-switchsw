/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dtl_l2_dvlantag.c
*
* @purpose   DTL interface
*
* @component DTL
*
* @comments  none
*
* @create    08/20/2003
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/

#include "dtlinclude.h"
#include "dvlantag_api.h"

L7_RC_t dtlDvlantagApply(DVLANTAG_DTL_GLOBAL_t *dTag)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Applies the Double vlan tagging configuration for this interface
*          
* @param    intIfNum @b{(input)} Internal Interface Number 
* @param    *dTag    @b{(input)} Pointer to the Double Vlan Tag stucture
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments 
*
*
* @end
*********************************************************************/
L7_RC_t dtlDvlantagIntfApply(L7_uint32 intIfNum, DVLANTAG_DTL_t *dTag)
{
  L7_RC_t rc = L7_SUCCESS;
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  L7_RC_t dr;
  
  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    dapiCmd.cmdData.doubleVlanTagConfig.getOrSet = DAPI_CMD_SET;
    /* We don't have SDK functions on setting based on Mask */
    dapiCmd.cmdData.doubleVlanTagConfig.etherType = dTag->etherType;
    dapiCmd.cmdData.doubleVlanTagConfig.customerId = dTag->custId;

    if (dTag->mode == L7_ENABLE)
    {
      dapiCmd.cmdData.doubleVlanTagConfig.enable = L7_TRUE;
    }
    else
    {
      dapiCmd.cmdData.doubleVlanTagConfig.enable = L7_FALSE;
    }
    dr = dapiCtl(&ddUsp, DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG, &dapiCmd);
    if (dr == L7_SUCCESS)
      rc = L7_SUCCESS;
    else
      rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Applies the Double vlan tagging configuration for this 
*           interface. 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *dTag    @b{(input)} Pointer to the Double Vlan Tag stucture
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function is useful when the system supports multiple
*           Outer TPIDs per port. The above function is used for only
*           setting the default TPID of system when setting the mode 
*           (UNI/NNI) of the port.
*
*
* @end
*********************************************************************/
L7_RC_t dtlDvlantagIntfMultiTpidApply(L7_uint32 intIfNum, DVLANTAG_DTL_t *dTag)
{
  L7_RC_t rc = L7_SUCCESS;
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    dapiCmd.cmdData.doubleVlanTagConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.doubleVlanTagConfig.etherType = dTag->etherType;
    dapiCmd.cmdData.doubleVlanTagConfig.customerId = dTag->custId;
    dapiCmd.cmdData.doubleVlanTagConfig.enable = dTag->enable;

    dr = dapiCtl(&ddUsp, DAPI_CMD_INTF_MULTI_TPID_DOUBLEVLAN_TAG_CONFIG, &dapiCmd);

    if (dr == L7_SUCCESS)
      rc = L7_SUCCESS;
    else
      rc = L7_FAILURE;
  }
  return rc;
}

/*********************************************************************
* @purpose  Applies the Default TPID configuration for the
*           system.
*
* @param    *dTag    @b{(input)} Pointer to the Double Vlan Tag stucture
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function is useful when the system supports multiple
*           Outer TPID. This is the TPID, system uses as OUTER TPID
*           while  egressing out the DT/SOT frames.
*
*
* @end
*********************************************************************/
L7_RC_t dtlDvlantagDefaultTpidApply(DVLANTAG_DTL_t *dTag)
{
  L7_RC_t rc = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  L7_RC_t dr;

  {
    ddUsp.unit = -1;
    ddUsp.slot = -1;
    ddUsp.port = -1;

    dapiCmd.cmdData.doubleVlanTagConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.doubleVlanTagConfig.etherType = dTag->etherType;
    dapiCmd.cmdData.doubleVlanTagConfig.customerId = 0;
    dapiCmd.cmdData.doubleVlanTagConfig.enable = 0;

    dr = dapiCtl(&ddUsp,DAPI_CMD_DEFAULT_TPID_DOUBLEVLAN_TAG_CONFIG, &dapiCmd);

    if (dr == L7_SUCCESS)
      rc = L7_SUCCESS;
    else
      rc = L7_FAILURE;
  }
  return rc;
}




