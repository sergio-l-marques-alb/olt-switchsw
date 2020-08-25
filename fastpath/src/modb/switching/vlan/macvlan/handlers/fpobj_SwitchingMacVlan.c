
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingMacVlan.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  25 April 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Radha K
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingMacVlan_obj.h"
#include "usmdb_vlan_mac_api.h"
#include "usmdb_common.h"

L7_RC_t
fpObjUtilSwitchVlanMacAssociationEntryGetNext(L7_char8* macAddress,
             L7_uint32 *agentSwitchVlanMacAssociationVlanId)
{
  L7_enetMacAddr_t    macAddr;
  L7_uint32 temp;
  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&macAddr.addr, macAddress, L7_ENET_MAC_ADDR_LEN);

  if((usmDbVlanMacGet(USMDB_UNIT_CURRENT, macAddr, &temp) == L7_SUCCESS) &&
     (temp > *agentSwitchVlanMacAssociationVlanId))
  {
    *agentSwitchVlanMacAssociationVlanId = temp;
  }
  else
  {
    if(usmDbVlanMacGetNext(USMDB_UNIT_CURRENT, macAddr, &macAddr,
                           agentSwitchVlanMacAssociationVlanId) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    memset(macAddress, 0x00, L7_ENET_MAC_ADDR_LEN);
    memcpy(macAddress, macAddr.addr, sizeof(L7_enetMacAddr_t));
  }
  return L7_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_SwitchingMacVlan_MacAddress
*
* @purpose Get 'MacAddress'
*
* @description [MacAddress] The Mac address
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMacVlan_MacAddress (void *wap, void *bufp)
{

  xLibStr6_t objMacAddressValue;
  xLibStr6_t nextObjMacAddressValue;
  xLibU32_t nextObjVlanIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MacAddress */
  memset(objMacAddressValue, 0x0, sizeof(L7_enetMacAddr_t));

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingMacVlan_MacAddress,
                          (xLibU8_t *) objMacAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjVlanIdValue = 0;
    memset(nextObjMacAddressValue, 0x0, sizeof(nextObjMacAddressValue));
    owa.l7rc = fpObjUtilSwitchVlanMacAssociationEntryGetNext(nextObjMacAddressValue, &nextObjVlanIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objMacAddressValue, owa.len);
    nextObjVlanIdValue = 0;
    memcpy(nextObjMacAddressValue, objMacAddressValue, L7_ENET_MAC_ADDR_LEN);
    
    do
    {
      owa.l7rc = fpObjUtilSwitchVlanMacAssociationEntryGetNext( nextObjMacAddressValue, &nextObjVlanIdValue);
    }
    while ( (memcmp(objMacAddressValue,nextObjMacAddressValue, sizeof(L7_enetMacAddr_t)) == 0) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjMacAddressValue, owa.len);

  /* return the object value: MacAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjMacAddressValue, sizeof (nextObjMacAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingMacVlan_VlanId
*
* @purpose Get 'VlanId'
*
* @description [VlanId] The VLAN the Mac address assigned to
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMacVlan_VlanId (void *wap, void *bufp)
{

  xLibStr6_t objMacAddressValue;
  xLibU32_t objVlanIdValue;
  xLibU32_t nextObjVlanIdValue;
  L7_enetMacAddr_t nextmacAddr;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  FPOBJ_TRACE_ENTER (bufp);

  memset(objMacAddressValue, 0x0, sizeof(L7_enetMacAddr_t));
  memset(&objVlanIdValue, 0x0, sizeof(objVlanIdValue));

  /* retrieve key: MacAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingMacVlan_MacAddress,
                          (xLibU8_t *) objMacAddressValue, &kwa.len);

  memset(&nextmacAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&nextmacAddr.addr, objMacAddressValue, L7_ENET_MAC_ADDR_LEN);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objMacAddressValue, owa.len);

  /* retrieve key: VlanId */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingMacVlan_VlanId,
                          (xLibU8_t *) & objVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjVlanIdValue = 0;
    owa.l7rc = fpObjUtilSwitchVlanMacAssociationEntryGetNext(
                                   nextmacAddr.addr,
                                   &nextObjVlanIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIdValue, owa.len);

    nextObjVlanIdValue = objVlanIdValue;

    owa.l7rc = fpObjUtilSwitchVlanMacAssociationEntryGetNext(
                                   nextmacAddr.addr, 
                                   &nextObjVlanIdValue);
  }

  if ( (memcmp(objMacAddressValue, nextmacAddr.addr, sizeof(nextmacAddr)) != 0) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanIdValue, owa.len);

  /* return the object value: VlanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanIdValue, sizeof (objVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingMacVlan_RowStatus
*
* @purpose Get 'RowStatus'
*
* @description [RowStatus] The Mac to VLAN association row status. Supported values:Add - used to create a new entry delete- removes the entry
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMacVlan_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMacAddressValue;
  fpObjWa_t kwaVlanId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MacAddress */
  kwaMacAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingMacVlan_MacAddress,
                                    (xLibU8_t *) keyMacAddressValue, &kwaMacAddress.len);
  if (kwaMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaMacAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMacAddress);
    return kwaMacAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMacAddressValue, kwaMacAddress.len);

  /* retrieve key: VlanId */
  kwaVlanId.rc = xLibFilterGet (wap, XOBJ_SwitchingMacVlan_VlanId,
                                (xLibU8_t *) & keyVlanIdValue, &kwaVlanId.len);
  if (kwaVlanId.rc != XLIBRC_SUCCESS)
  {
    kwaVlanId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanId);
    return kwaVlanId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIdValue, kwaVlanId.len);

  /* get the value from application */
  objRowStatusValue = L7_ROW_STATUS_ACTIVE;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue, sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingMacVlan_RowStatus
*
* @purpose Set 'RowStatus'
*
* @description [RowStatus] The Mac to VLAN association row status. Supported values:Add - used to create a new entry delete- removes the entry
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingMacVlan_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;

  fpObjWa_t kwaMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMacAddressValue;
  fpObjWa_t kwaVlanId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIdValue;
  L7_enetMacAddr_t    macAddr;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: MacAddress */
  kwaMacAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingMacVlan_MacAddress,
                                    (xLibU8_t *) keyMacAddressValue, &kwaMacAddress.len);
  if (kwaMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaMacAddress.rc = XLIBRC_MAC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaMacAddress);
    return kwaMacAddress.rc;
  }
  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&macAddr.addr, keyMacAddressValue, L7_ENET_MAC_ADDR_LEN);

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMacAddressValue, kwaMacAddress.len);

  /* Check for proper MAC Address */
  if(!(macAddr.addr[0]|macAddr.addr[1]|macAddr.addr[2]|
       macAddr.addr[3]|macAddr.addr[4]|macAddr.addr[5]))
  {
    owa.rc = XLIBRC_INVALID_NON_ZERO_MAC;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: VlanId */
  kwaVlanId.rc = xLibFilterGet (wap, XOBJ_SwitchingMacVlan_VlanId,
                                (xLibU8_t *) & keyVlanIdValue, &kwaVlanId.len);
  if (kwaVlanId.rc != XLIBRC_SUCCESS)
  {
    kwaVlanId.rc = XLIBRC_VLANID_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanId);
    return kwaVlanId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIdValue, kwaVlanId.len);

  owa.l7rc = L7_SUCCESS;
  if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    memcpy(macAddr.addr,keyMacAddressValue, sizeof(keyMacAddressValue)); 
    owa.l7rc = usmDbVlanMacAdd (L7_UNIT_CURRENT, 
                                macAddr,
                                keyVlanIdValue);
      if (owa.l7rc != L7_SUCCESS)
      {
         owa.rc = XLIBRC_VLAN_MAC_ADD_FAILURE;    /* TODO: Change if required */
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
      }

  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    memcpy(macAddr.addr,keyMacAddressValue, sizeof(keyMacAddressValue)); 
    owa.l7rc = usmDbVlanMacDelete (L7_UNIT_CURRENT, 
                                   macAddr,
                                   keyVlanIdValue);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_VLAN_MAC_DEL_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
 
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingMacVlan_VlanIdEdit
*
* @purpose Get 'VlanIdEdit'
*
* @description [VlanIdEdit] The VLAN the Mac address assigned to
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMacVlan_VlanIdEdit (void *wap, void *bufp)
{
  /* just return SUCCESS as this logic is handled in java script */
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjSet_SwitchingMacVlan_VlanIdEdit
*
* @purpose Get 'VlanIdEdit'
*
* @description [VlanIdEdit] The VLAN the Mac address assigned to
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingMacVlan_VlanIdEdit (void *wap, void *bufp)
{
  /* just return SUCCESS as this logic is handled in java script */
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_SwitchingMacVlan_RowStatusEdit
*
* @purpose Get 'RowStatusEdit'
*
* @description [RowStatusEdit] The Mac to VLAN association row status. Supported values:Add - used to create a new entry delete- removes the entry
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMacVlan_RowStatusEdit (void *wap, void *bufp)
{

  fpObjWa_t kwaMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMacAddressValue;
  fpObjWa_t kwaVlanId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MacAddress */
  kwaMacAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingMacVlan_MacAddress,
                                    (xLibU8_t *) keyMacAddressValue, &kwaMacAddress.len);
  if (kwaMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaMacAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMacAddress);
    return kwaMacAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMacAddressValue, kwaMacAddress.len);

  /* retrieve key: VlanId */
  kwaVlanId.rc = xLibFilterGet (wap, XOBJ_SwitchingMacVlan_VlanIdEdit,
                                (xLibU8_t *) & keyVlanIdValue, &kwaVlanId.len);
  if (kwaVlanId.rc != XLIBRC_SUCCESS)
  {
    kwaVlanId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanId);
    return kwaVlanId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIdValue, kwaVlanId.len);

  /* get the value from application */
  objRowStatusValue = L7_ROW_STATUS_ACTIVE;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue, sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingMacVlan_RowStatusEdit
*
* @purpose Set 'RowStatusEdit'
*
* @description [RowStatusEdit] The Mac to VLAN association row status. Supported values:Add - used to create a new entry delete- removes the entry
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingMacVlan_RowStatusEdit (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;

  fpObjWa_t kwaMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMacAddressValue;
  fpObjWa_t kwaVlanId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIdValue;
  L7_enetMacAddr_t    macAddr;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: MacAddress */
  kwaMacAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingMacVlan_MacAddress,
                                    (xLibU8_t *) keyMacAddressValue, &kwaMacAddress.len);
  if (kwaMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaMacAddress.rc = XLIBRC_MAC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaMacAddress);
    return kwaMacAddress.rc;
  }
  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&macAddr.addr, keyMacAddressValue, L7_ENET_MAC_ADDR_LEN);

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMacAddressValue, kwaMacAddress.len);

  /* Check for proper MAC Address */
  if(!(macAddr.addr[0]|macAddr.addr[1]|macAddr.addr[2]|
       macAddr.addr[3]|macAddr.addr[4]|macAddr.addr[5]))
  {
    owa.rc = XLIBRC_INVALID_NON_ZERO_MAC;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: VlanId */
  kwaVlanId.rc = xLibFilterGet (wap, XOBJ_SwitchingMacVlan_VlanIdEdit,
                                (xLibU8_t *) & keyVlanIdValue, &kwaVlanId.len);
  if (kwaVlanId.rc != XLIBRC_SUCCESS)
  {
    kwaVlanId.rc = XLIBRC_VLANID_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanId);
    return kwaVlanId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIdValue, kwaVlanId.len);

  owa.l7rc = L7_SUCCESS;
  if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    memcpy(macAddr.addr,keyMacAddressValue, sizeof(keyMacAddressValue)); 
    owa.l7rc = usmDbVlanMacAdd (L7_UNIT_CURRENT, 
                                macAddr,
                                keyVlanIdValue);
      if (owa.l7rc != L7_SUCCESS)
      {
         owa.rc = XLIBRC_VLAN_MAC_ADD_FAILURE;    /* TODO: Change if required */
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
      }

  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    memcpy(macAddr.addr,keyMacAddressValue, sizeof(keyMacAddressValue)); 
    owa.l7rc = usmDbVlanMacDelete (L7_UNIT_CURRENT, 
                                   macAddr,
                                   keyVlanIdValue);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_VLAN_MAC_DEL_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
 
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


