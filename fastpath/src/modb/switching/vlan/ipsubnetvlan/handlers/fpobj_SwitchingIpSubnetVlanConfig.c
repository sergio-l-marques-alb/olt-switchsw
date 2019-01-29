
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingIpSubnetVlanConfig.c
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
#include "_xe_SwitchingIpSubnetVlanConfig_obj.h"
#include "usmdb_vlan_ipsubnet_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingIpSubnetVlanConfig_IPAddress
*
* @purpose Get 'IPAddress'
*
* @description [IPAddress] The IP address.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIpSubnetVlanConfig_IPAddress (void *wap, void *bufp)
{

  xLibIpV4_t objIPAddressValue;
  xLibIpV4_t nextObjIPAddressValue;
  xLibIpV4_t objSubnetMaskValue;
  xLibIpV4_t nextObjSubnetMaskValue;
  xLibIpV4_t tempIPAddressValue = 0;
  xLibU32_t objVlanIdValue;
  xLibU32_t nextObjVlanIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IPAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIpSubnetVlanConfig_IPAddress,
                          (xLibU8_t *) &objIPAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objIPAddressValue = objSubnetMaskValue = objVlanIdValue = 0;
      owa.l7rc = usmDbVlanIpSubnetSubnetGetNext(L7_UNIT_CURRENT,
                                                objIPAddressValue,
                                                objSubnetMaskValue,
                                                &nextObjIPAddressValue, 
                                                &nextObjSubnetMaskValue,
                                                &nextObjVlanIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIPAddressValue, owa.len);
    tempIPAddressValue = objIPAddressValue;
    objIPAddressValue = 0;
    objSubnetMaskValue = 0;  
    do
    {
      owa.l7rc = usmDbVlanIpSubnetSubnetGetNext(L7_UNIT_CURRENT,
                                                objIPAddressValue,
                                                objSubnetMaskValue,
                                                &nextObjIPAddressValue, 
                                                &nextObjSubnetMaskValue,
                                                &nextObjVlanIdValue);
     objIPAddressValue = nextObjIPAddressValue;
     objSubnetMaskValue = nextObjSubnetMaskValue;

    }while (( tempIPAddressValue != nextObjIPAddressValue) && (owa.l7rc == L7_SUCCESS));

    if( owa.l7rc == L7_SUCCESS)
    {
       owa.l7rc = usmDbVlanIpSubnetSubnetGetNext(L7_UNIT_CURRENT,
                                                objIPAddressValue,
                                                objSubnetMaskValue,
                                                &nextObjIPAddressValue,
                                                &nextObjSubnetMaskValue,
                                                &nextObjVlanIdValue);
       
    }
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIPAddressValue, owa.len);

  /* return the object value: IPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjIPAddressValue, sizeof (objIPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingIpSubnetVlanConfig_SubnetMask
*
* @purpose Get 'SubnetMask'
*
* @description [SubnetMask] The subnet mask.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIpSubnetVlanConfig_SubnetMask (void *wap, void *bufp)
{

  xLibIpV4_t objIPAddressValue;
  xLibIpV4_t nextObjIPAddressValue;
  xLibIpV4_t objSubnetMaskValue;
  xLibIpV4_t nextObjSubnetMaskValue;
  xLibIpV4_t tempIPAddressValue = 0;
  xLibU32_t nextObjVlanIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IPAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIpSubnetVlanConfig_IPAddress,
                          (xLibU8_t *) &objIPAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIPAddressValue, owa.len);

  /* retrieve key: SubnetMask */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIpSubnetVlanConfig_SubnetMask,
                          (xLibU8_t *) &objSubnetMaskValue, &owa.len);

  

  if (owa.rc != XLIBRC_SUCCESS)
  {
    tempIPAddressValue = objIPAddressValue;
    objIPAddressValue = 0;
    objSubnetMaskValue = 0;
    do
    {
      owa.l7rc = usmDbVlanIpSubnetSubnetGetNext(L7_UNIT_CURRENT,
                                                objIPAddressValue,
                                                objSubnetMaskValue,
                                                &nextObjIPAddressValue,
                                                &nextObjSubnetMaskValue,
                                                &nextObjVlanIdValue);
      objIPAddressValue = nextObjIPAddressValue;
      objSubnetMaskValue = nextObjSubnetMaskValue;
 
     }while (( tempIPAddressValue != nextObjIPAddressValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS) || (owa.rc == XLIBRC_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjSubnetMaskValue, owa.len);

  /* return the object value: SubnetMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjSubnetMaskValue, sizeof (nextObjSubnetMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingIpSubnetVlanConfig_VlanId
*
* @purpose Get 'VlanId'
*
* @description [VlanId] The VLAN that is associated to the IP address and subnet mask.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIpSubnetVlanConfig_VlanId (void *wap, void *bufp)
{

  xLibIpV4_t objIPAddressValue;
  xLibIpV4_t objSubnetMaskValue;
  xLibU32_t objVlanIdValue;
  xLibU32_t nextObjVlanIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IPAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIpSubnetVlanConfig_IPAddress,
                          (xLibU8_t *) &objIPAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIPAddressValue, owa.len);

  /* retrieve key: SubnetMask */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIpSubnetVlanConfig_SubnetMask,
                          (xLibU8_t *) &objSubnetMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objSubnetMaskValue, owa.len);

  /* retrieve key: VlanId */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingIpSubnetVlanConfig_VlanId,
                          (xLibU8_t *) & objVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    (void)usmDbVlanIpSubnetSubnetGet(L7_UNIT_CURRENT, objIPAddressValue,
                                   objSubnetMaskValue, &nextObjVlanIdValue);

  }
  else
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
* @function fpObjGet_SwitchingIpSubnetVlanConfig_RowStatus
*
* @purpose Get 'RowStatus'
*
* @description [RowStatus] The Subnet to VLAN association row status. Supported values: Add - used to create a new entry delete- removes the entry
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingIpSubnetVlanConfig_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaIPAddress = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t keyIPAddressValue;
  fpObjWa_t kwaSubnetMask = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t keySubnetMaskValue;
  fpObjWa_t kwaVlanId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IPAddress */
  kwaIPAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingIpSubnetVlanConfig_IPAddress,
                                   (xLibU8_t *) &keyIPAddressValue, &kwaIPAddress.len);
  if (kwaIPAddress.rc != XLIBRC_SUCCESS)
  {
    kwaIPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIPAddress);
    return kwaIPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIPAddressValue, kwaIPAddress.len);

  /* retrieve key: SubnetMask */
  kwaSubnetMask.rc = xLibFilterGet (wap, XOBJ_SwitchingIpSubnetVlanConfig_SubnetMask,
                                    (xLibU8_t *) &keySubnetMaskValue, &kwaSubnetMask.len);
  if (kwaSubnetMask.rc != XLIBRC_SUCCESS)
  {
    kwaSubnetMask.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSubnetMask);
    return kwaSubnetMask.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySubnetMaskValue, kwaSubnetMask.len);

  /* retrieve key: VlanId */
  kwaVlanId.rc = xLibFilterGet (wap, XOBJ_SwitchingIpSubnetVlanConfig_VlanId,
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
* @function fpObjSet_SwitchingIpSubnetVlanConfig_RowStatus
*
* @purpose Set 'RowStatus'
*
* @description [RowStatus] The Subnet to VLAN association row status. Supported values: Add - used to create a new entry delete- removes the entry
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingIpSubnetVlanConfig_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;

  fpObjWa_t kwaIPAddress = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t keyIPAddressValue;
  fpObjWa_t kwaSubnetMask = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t keySubnetMaskValue;
  fpObjWa_t kwaVlanId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: IPAddress */
  kwaIPAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingIpSubnetVlanConfig_IPAddress,
                                   (xLibU8_t *) &keyIPAddressValue, &kwaIPAddress.len);
  if (kwaIPAddress.rc != XLIBRC_SUCCESS)
  {
    kwaIPAddress.rc = XLIBRC_IP_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaIPAddress);
    return kwaIPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIPAddressValue, kwaIPAddress.len);

  /* retrieve key: SubnetMask */
  kwaSubnetMask.rc = xLibFilterGet (wap, XOBJ_SwitchingIpSubnetVlanConfig_SubnetMask,
                                    (xLibU8_t *) &keySubnetMaskValue, &kwaSubnetMask.len);
  if (kwaSubnetMask.rc != XLIBRC_SUCCESS)
  {
    kwaSubnetMask.rc = XLIBRC_SUBNET_MASK_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaSubnetMask);
    return kwaSubnetMask.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySubnetMaskValue, kwaSubnetMask.len);

  /* retrieve key: VlanId */
  kwaVlanId.rc = xLibFilterGet (wap, XOBJ_SwitchingIpSubnetVlanConfig_VlanId,
                                (xLibU8_t *) &keyVlanIdValue, &kwaVlanId.len);
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
    owa.l7rc = usmDbVlanIpSubnetSubnetAdd(L7_UNIT_CURRENT, keyIPAddressValue,
                                         keySubnetMaskValue, keyVlanIdValue);
      if (owa.l7rc != L7_SUCCESS)
      {
         owa.rc = XLIBRC_IP_SUBNET_ADD_FAILURE;    /* TODO: Change if required */
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;     
      }

  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbVlanIpSubnetSubnetDelete(L7_UNIT_CURRENT, keyIPAddressValue,
                                             keySubnetMaskValue, keyVlanIdValue);
      if (owa.l7rc != L7_SUCCESS)
      {
         owa.rc = XLIBRC_IP_SUBNET_DEL_FAILURE;    /* TODO: Change if required */
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
