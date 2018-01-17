
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingPortSecurityStaticEntries.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  09 May 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingPortSecurityStaticEntries_obj.h"
#include "usmdb_pml_api.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_vlan_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingPortSecurityStaticEntries_Interface
*
* @purpose Get 'Interface'
*
* @description [Interface] Interface to be configured for PortSecurity
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPortSecurityStaticEntries_Interface (void *wap, void *bufp)
{

  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof(objInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objInterfaceValue = 0;
	owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, &nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    do
    {	  
      owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 0 , objInterfaceValue,&nextObjInterfaceValue);
    }
    while ((objInterfaceValue == nextObjInterfaceValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue, sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingPortSecurityStaticEntries_StaticVLANId
*
* @purpose Get 'StaticVLANId'
*
* @description [StaticVLANId] Source VLAN id of the packet that is received on the statically locked port.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPortSecurityStaticEntries_StaticVLANId (void *wap, void *bufp)
{

  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  xLibStr6_t objStaticMACAddressValue={0};
  xLibU32_t objStaticVLANIdValue=0;
  xLibU16_t nextObjStaticVLANIdValueShort=0;
  xLibU32_t nextObjStaticVLANIdValue=0,nextFlg=0;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
  
  /* retrieve key: StaticVLANId */
  owa.len = sizeof(objStaticVLANIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_StaticVLANId,
                          (xLibU8_t *) & objStaticVLANIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objStaticVLANIdValue = 0;
    nextObjStaticVLANIdValueShort= 0;
    owa.l7rc = usmDbPmlIntfStaticEntryGetFirst(L7_UNIT_CURRENT,
                                     objInterfaceValue,
                                     (L7_enetMacAddr_t *)&objStaticMACAddressValue,
                                     &nextObjStaticVLANIdValueShort);

    nextObjInterfaceValue = objInterfaceValue;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objStaticVLANIdValue, owa.len);
	
	  nextObjStaticVLANIdValueShort= 0;
    owa.l7rc = usmDbPmlIntfStaticEntryGetFirst(L7_UNIT_CURRENT,
                                     objInterfaceValue,
                                     (L7_enetMacAddr_t *)&objStaticMACAddressValue,
                                     &nextObjStaticVLANIdValueShort);

	 do
    {
      if(nextObjStaticVLANIdValueShort == objStaticVLANIdValue)
      {
        nextFlg = 1;
      }

      owa.l7rc = usmDbPmlIntfStaticEntryGetNext(L7_UNIT_CURRENT,
                                      objInterfaceValue,
                                      (L7_enetMacAddr_t *)&objStaticMACAddressValue,
                                      &nextObjStaticVLANIdValueShort);

      if(nextFlg == 1)
      {
        if(nextObjStaticVLANIdValueShort != objStaticVLANIdValue)
        {
          break;
        }     
      }
    }while(owa.l7rc == L7_SUCCESS);					 
  }

  if ((objStaticVLANIdValue == nextObjStaticVLANIdValueShort) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjStaticVLANIdValue = nextObjStaticVLANIdValueShort;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjStaticVLANIdValue, owa.len);

  /* return the object value: StaticVLANId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjStaticVLANIdValue,
                           sizeof (nextObjStaticVLANIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingPortSecurityStaticEntries_StaticMACAddress
*
* @purpose Get 'StaticMACAddress'
*
* @description [StaticMACAddress] Source MAC address of the packet that is received on the statically locked port.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPortSecurityStaticEntries_StaticMACAddress (void *wap, void *bufp)
{

  xLibU32_t objInterfaceValue, nextObjInterfaceValue;
  xLibStr6_t objStaticMACAddressValue={0};
  xLibStr6_t nextObjStaticMACAddressValue={0};
  xLibU32_t objStaticVLANIdValue=0;
  xLibU16_t nextObjStaticVLANIdValueShort=0;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  FPOBJ_TRACE_ENTER (bufp);
  


  /* retrieve key: Interface */
  owa.len = sizeof(objInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);

  /* retrieve key: StaticVLANId */
  owa.len = sizeof(objStaticVLANIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_StaticVLANId,
                          (xLibU8_t *) & objStaticVLANIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objStaticVLANIdValue, owa.len);

  /* retrieve key: StaticMACAddress */
  owa.len = sizeof(objStaticMACAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_StaticMACAddress,
                          (xLibU8_t *) objStaticMACAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(nextObjStaticMACAddressValue, 0x0, sizeof(nextObjStaticMACAddressValue));
    nextObjStaticVLANIdValueShort = 0;
    nextObjInterfaceValue  = objInterfaceValue;

    owa.l7rc = usmDbPmlIntfStaticEntryGetFirst(L7_UNIT_CURRENT,
                                     objInterfaceValue,
                                     (L7_enetMacAddr_t *)&nextObjStaticMACAddressValue,
                                     &nextObjStaticVLANIdValueShort);

    if(nextObjStaticVLANIdValueShort != objStaticVLANIdValue)
    {
      do
      {
        owa.l7rc = usmDbPmlIntfStaticEntryGetNext(L7_UNIT_CURRENT,
                                      objInterfaceValue,
                                      (L7_enetMacAddr_t *)&nextObjStaticMACAddressValue,
                                      &nextObjStaticVLANIdValueShort);
      }while((nextObjStaticVLANIdValueShort != objStaticVLANIdValue) && (owa.l7rc == L7_SUCCESS));
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objStaticMACAddressValue, owa.len);
    memcpy(nextObjStaticMACAddressValue,objStaticMACAddressValue,sizeof(objStaticMACAddressValue));
    nextObjStaticVLANIdValueShort = objStaticVLANIdValue;
    nextObjInterfaceValue  = objInterfaceValue;
    
    owa.l7rc = usmDbPmlIntfStaticEntryGetNext(L7_UNIT_CURRENT,
                                      objInterfaceValue,
                                      (L7_enetMacAddr_t *)&nextObjStaticMACAddressValue,
                                      &nextObjStaticVLANIdValueShort);
  }


  if ((objStaticVLANIdValue != nextObjStaticVLANIdValueShort) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjStaticMACAddressValue, owa.len);

  /* return the object value: StaticMACAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjStaticMACAddressValue,
                           sizeof (objStaticMACAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingPortSecurityStaticEntries_StaticEntryDelete
*
* @purpose Set 'StaticEntryDelete'
*
* @description [StaticEntryDelete] Deletes a statically locked entry from an interface.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingPortSecurityStaticEntries_StaticEntryDelete (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr6_t objStaticEntryDeleteValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_enetMacAddr_t macAddr;
  xLibU32_t keyInterfaceValue;
  xLibU32_t keyVlanIdValue;
  xLibU16_t vlanID;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StaticEntryDelete */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objStaticEntryDeleteValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objStaticEntryDeleteValue, owa.len);

  memset(&macAddr, 0x0, sizeof(macAddr));
  memcpy(&macAddr.addr, objStaticEntryDeleteValue, L7_ENET_MAC_ADDR_LEN);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: VlanId */
  owa.len = sizeof (keyVlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_VlanId,
                                   (xLibU8_t *) & keyVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIdValue, owa.len);

  vlanID = keyVlanIdValue;

  /* set the value in application */
  owa.l7rc = usmDbPmlIntfStaticEntryDelete (L7_UNIT_CURRENT, keyInterfaceValue, macAddr, vlanID);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingPortSecurityStaticEntries_VlanId
*
* @purpose Set 'VlanId'
*
* @description [VlanId] 
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingPortSecurityStaticEntries_VlanId (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t objVlanIdValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VlanId */
  owa.len = sizeof (objVlanIdValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *)&objVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVlanIdValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/**********************************************************


************************************************************/
xLibRC_t fpObjGet_SwitchingPortSecurityStaticEntries_StaticVidMac (void *wap, void *bufp)
{
  xLibU8_t vid_mac[L7_VID_MAC_ADDR_LEN];
  xLibU32_t key;
  xLibU16_t keyLen = sizeof (key), len = sizeof(vid_mac), vlanId=0;
  xLibU8_t mac[L7_ENET_MAC_ADDR_LEN];
  xLibRC_t rc;
  L7_uint32 tempVlanid=0;

  memset(&vid_mac, 0, sizeof(vid_mac));
  memset(&mac, 0, sizeof(mac));

  /* retrieve the interface */
  if(xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_Interface, (xLibU8_t *)&key, &keyLen)!= XLIBRC_SUCCESS)
  {
    return XLIBRC_FILTER_MISSING;
  }

  len = sizeof (vid_mac);
  rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_StaticVidMac, (xLibU8_t *)vid_mac, &len);
  if (rc == XLIBRC_NO_FILTER)
  {
    if(usmDbPmlIntfStaticEntryGetFirst(1, key, (L7_enetMacAddr_t *)mac, &vlanId) == L7_SUCCESS)
    {
      usmDbEntryVidMacCombine(vlanId, mac, vid_mac);
      xLibBufDataSet (bufp, (xLibU8_t *) vid_mac, sizeof (vid_mac));
      return XLIBRC_SUCCESS;
    }
    else
    {
      return XLIBRC_ENDOF_TABLE;
    }
  }
  else
  {
    usmDbEntryVidMacSeparate(vid_mac, &tempVlanid, mac);
    vlanId = (xLibU16_t)tempVlanid;
    if (usmDbPmlIntfStaticEntryGetNext(1, key, (L7_enetMacAddr_t *)mac, &vlanId) == L7_SUCCESS)
    {
      tempVlanid = vlanId;
      usmDbEntryVidMacCombine(tempVlanid, mac, vid_mac);
      xLibBufDataSet (bufp, (xLibU8_t *) vid_mac, sizeof (vid_mac));
      return XLIBRC_SUCCESS;
    }
    else
    {
      return XLIBRC_ENDOF_TABLE;
    }
  }

  return XLIBRC_SUCCESS;
}
/********************************************************************


********************************************************************/
xLibRC_t fpObjGet_SwitchingPortSecurityStaticEntries_DynamicVidMac (void *wap, void *bufp)
{
  xLibU8_t vid_mac[L7_VID_MAC_ADDR_LEN];
  xLibU32_t key;
  xLibU16_t keyLen = sizeof (key), len = sizeof(vid_mac), vlanId=0;
  xLibU8_t mac[L7_ENET_MAC_ADDR_LEN];
  xLibRC_t rc;
  L7_uint32 tempVlanid=0;

  memset(&vid_mac, 0, sizeof(vid_mac));
  memset(&mac, 0, sizeof(mac));

  /* retrieve the interface */
  if(xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_Interface, (xLibU8_t *)&key, &keyLen)!= XLIBRC_SUCCESS)
  {
    return XLIBRC_FILTER_MISSING;
  }

  len = sizeof (vid_mac);
  rc = xLibFilterGet (wap,  XOBJ_SwitchingPortSecurityStaticEntries_DynamicVidMac, (xLibU8_t *)vid_mac, &len);
  if (rc == XLIBRC_NO_FILTER)
  {
    if(usmDbPmlIntfDynamicEntryGetFirst(1, key, (L7_enetMacAddr_t *)mac, &vlanId) == L7_SUCCESS)
    {
      usmDbEntryVidMacCombine(vlanId, mac, vid_mac);
      xLibBufDataSet (bufp, (xLibU8_t *) vid_mac, sizeof (vid_mac));
      return XLIBRC_SUCCESS;
    }
    else
    {
       return XLIBRC_ENDOF_TABLE;
    }
  }
  else
  {
    usmDbEntryVidMacSeparate(vid_mac, &tempVlanid, mac);
    vlanId = (xLibU16_t)tempVlanid;
    if (usmDbPmlIntfDynamicEntryGetNext(1, key, (L7_enetMacAddr_t *)mac, &vlanId) == L7_SUCCESS)
    {
      tempVlanid = vlanId;
      usmDbEntryVidMacCombine(tempVlanid, mac, vid_mac);
      xLibBufDataSet (bufp, (xLibU8_t *) vid_mac, sizeof (vid_mac));

      return XLIBRC_SUCCESS;
    }
    else
    {
      return XLIBRC_ENDOF_TABLE;
    }
  }

  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjSet_SwitchingPortSecurityStaticEntries_StaticMACEntryDelete
*
* @purpose Set 'StaticMACEntryDelete'
 *@description  [StaticMACEntryDelete] Deletes a statically locked entry from an
* interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingPortSecurityStaticEntries_StaticMACEntryDelete (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStaticMACEntryDeleteValue;

  xLibU32_t keyInterfaceValue;
  xLibU32_t keyStaticVLANIdValue;
  xLibStr6_t keyStaticMACAddressValue;

  L7_enetMacAddr_t macAddr;
  xLibU16_t vlanID;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StaticMACEntryDelete */
  owa.len = sizeof (objStaticMACEntryDeleteValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objStaticMACEntryDeleteValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStaticMACEntryDeleteValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* retrieve key: StaticVLANId */
  owa.len = sizeof (keyStaticVLANIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_StaticVLANId,
                          (xLibU8_t *) & keyStaticVLANIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticVLANIdValue, owa.len);
  vlanID = keyStaticVLANIdValue;

  /* retrieve key: StaticMACAddress */
  owa.len = sizeof (keyStaticMACAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticEntries_StaticMACAddress,
                          (xLibU8_t *) keyStaticMACAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyStaticMACAddressValue, owa.len);

  memset(&macAddr, 0x0, sizeof(macAddr));
  memcpy(&macAddr.addr, keyStaticMACAddressValue, L7_ENET_MAC_ADDR_LEN);
  
  /* set the value in application */
  owa.l7rc = usmDbPmlIntfStaticEntryDelete (L7_UNIT_CURRENT, keyInterfaceValue,
                              macAddr,keyStaticVLANIdValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
