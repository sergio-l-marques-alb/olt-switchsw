
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingPortSecurityDynamicEntries.c
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
#include "_xe_SwitchingPortSecurityDynamicEntries_obj.h"
#include "usmdb_pml_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingPortSecurityDynamicEntries_Interface
*
* @purpose Get 'Interface'
*
* @description [Interface] Interface to be configured for PortSecurity
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPortSecurityDynamicEntries_Interface (void *wap, void *bufp)
{

  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityDynamicEntries_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objInterfaceValue =  0;
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
* @function fpObjGet_SwitchingPortSecurityDynamicEntries_DynamicVLANId
*
* @purpose Get 'DynamicVLANId'
*
* @description [DynamicVLANId] Source VLAN id of the packet that is received on the dynamically locked port.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPortSecurityDynamicEntries_DynamicVLANId (void *wap, void *bufp)
{

  xLibU32_t objInterfaceValue,nextObjInterfaceValue;
  xLibStr6_t objDynamicMACAddressValue={0};
  xLibU32_t objDynamicVLANIdValue=0;
  xLibU32_t nextObjDynamicVLANIdValue=0,nextFlg=0;
  xLibU16_t nextObjDynamicVLANIdValueShort=0;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityDynamicEntries_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);

  /* retrieve key: DynamicVLANId */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityDynamicEntries_DynamicVLANId,
                          (xLibU8_t *) & objDynamicVLANIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    objDynamicVLANIdValue = 0;
    nextObjDynamicVLANIdValueShort = 0;
    memset(objDynamicMACAddressValue, 0x0, sizeof(objDynamicMACAddressValue));

    owa.l7rc = usmDbPmlIntfDynamicEntryGetFirst(L7_UNIT_CURRENT,
                                     objInterfaceValue,
                                     (L7_enetMacAddr_t *)&objDynamicMACAddressValue,
                                     &nextObjDynamicVLANIdValueShort);

    nextObjInterfaceValue = objInterfaceValue;	
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objDynamicVLANIdValue, owa.len);
    nextObjDynamicVLANIdValueShort = 0;
    memset(objDynamicMACAddressValue, 0x0, sizeof(objDynamicMACAddressValue));

    owa.l7rc = usmDbPmlIntfDynamicEntryGetFirst(L7_UNIT_CURRENT,
                                     objInterfaceValue,
                                     (L7_enetMacAddr_t *)&objDynamicMACAddressValue,
                                     &nextObjDynamicVLANIdValueShort);

    do
    {
      if(nextObjDynamicVLANIdValueShort == objDynamicVLANIdValue)
      {
        nextFlg = 1;
      }

      owa.l7rc = usmDbPmlIntfDynamicEntryGetNext(L7_UNIT_CURRENT,
                                    objInterfaceValue,
                                    (L7_enetMacAddr_t *)&objDynamicMACAddressValue,
                                    &nextObjDynamicVLANIdValueShort);

      if(nextFlg == 1)
      {
        if(nextObjDynamicVLANIdValueShort != objDynamicVLANIdValue)
        {
          break;
        }     
      }
    }while(owa.l7rc == L7_SUCCESS);

    nextObjInterfaceValue = objInterfaceValue;	
  }

  if ((objDynamicVLANIdValue == nextObjDynamicVLANIdValueShort) || (owa.l7rc != L7_SUCCESS))
  {    
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  nextObjDynamicVLANIdValue = nextObjDynamicVLANIdValueShort;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjDynamicVLANIdValue, owa.len);

  /* return the object value: DynamicVLANId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDynamicVLANIdValue,
                           sizeof (objDynamicVLANIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingPortSecurityDynamicEntries_DynamicMACAddress
*
* @purpose Get 'DynamicMACAddress'
*
* @description [DynamicMACAddress] Source MAC address of the packet that is received on the dynamically locked port.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPortSecurityDynamicEntries_DynamicMACAddress (void *wap, void *bufp)
{

  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue = 0;
  xLibStr6_t objDynamicMACAddressValue;
  xLibStr6_t nextObjDynamicMACAddressValue;
  xLibU32_t objDynamicVLANIdValue=0;
  xLibU16_t nextObjDynamicVLANIdValueShort=0;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityDynamicEntries_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);


  /* retrieve key: DynamicVLANId */
  owa.len = sizeof(objDynamicVLANIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityDynamicEntries_DynamicVLANId,
                          (xLibU8_t *) & objDynamicVLANIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objDynamicVLANIdValue, owa.len);


  /* retrieve key: DynamicMACAddress */
  owa.len = sizeof(objDynamicMACAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityDynamicEntries_DynamicMACAddress,
                          (xLibU8_t *) objDynamicMACAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(nextObjDynamicMACAddressValue, 0x0, sizeof(nextObjDynamicMACAddressValue));
    nextObjDynamicVLANIdValueShort = 0;
    nextObjInterfaceValue  = objInterfaceValue;
    owa.l7rc = usmDbPmlIntfDynamicEntryGetFirst(L7_UNIT_CURRENT,
                                     objInterfaceValue,
                                     (L7_enetMacAddr_t *)&nextObjDynamicMACAddressValue,
                                     &nextObjDynamicVLANIdValueShort);
    
	if(nextObjDynamicVLANIdValueShort != objDynamicVLANIdValue)
    {
      do
      {
        owa.l7rc = usmDbPmlIntfDynamicEntryGetNext(L7_UNIT_CURRENT,
                                      objInterfaceValue,
                                      (L7_enetMacAddr_t *)&nextObjDynamicMACAddressValue,
                                      &nextObjDynamicVLANIdValueShort);
      }while((nextObjDynamicVLANIdValueShort != objDynamicVLANIdValue) && (owa.l7rc == L7_SUCCESS));	  
    }    
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objDynamicMACAddressValue, owa.len);
    memcpy(nextObjDynamicMACAddressValue,objDynamicMACAddressValue,sizeof(objDynamicMACAddressValue));
    nextObjDynamicVLANIdValueShort = objDynamicVLANIdValue;
    nextObjInterfaceValue  = objInterfaceValue;
      
    owa.l7rc = usmDbPmlIntfDynamicEntryGetNext(L7_UNIT_CURRENT,
                                      objInterfaceValue,
                                      (L7_enetMacAddr_t *)&nextObjDynamicMACAddressValue,
                                      &nextObjDynamicVLANIdValueShort);    
  }

  if((objDynamicVLANIdValue != nextObjDynamicVLANIdValueShort) || (owa.l7rc != L7_SUCCESS))
  {    
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjDynamicMACAddressValue, owa.len);

  /* return the object value: DynamicMACAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjDynamicMACAddressValue,
                           sizeof (nextObjDynamicMACAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
