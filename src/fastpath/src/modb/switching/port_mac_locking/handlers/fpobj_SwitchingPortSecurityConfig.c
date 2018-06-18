
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingPortSecurityStaticConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  19 November 2008, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "usmdb_util_api.h"
#include "usmdb_pml_api.h"
#include "fpobj_util.h"
#include "_xe_SwitchingPortSecurityStaticConfig_obj.h"

/*******************************************************************************
* @function fpObjGet_SwitchingPortSecurityStaticConfig_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] Interface to be configured for PortSecurity   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPortSecurityStaticConfig_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (objInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticConfig_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet( &nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    do
    {
      owa.l7rc = usmDbValidIntIfNumNext(objInterfaceValue, &nextObjInterfaceValue);
    }
    while ((objInterfaceValue == nextObjInterfaceValue) && (owa.l7rc == L7_SUCCESS));

  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue,
                           sizeof (nextObjInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingPortSecurityStaticConfig_MACAddressAction
*
* @purpose Get 'MACAddressAction'
 *@description  [MACAddressAction] This object specifies the action of either
* add/remove/none    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPortSecurityStaticConfig_MACAddressAction (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objMACAddressActionValue;
  xLibU32_t nextObjMACAddressActionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MACAddressAction */
  owa.len = sizeof (objMACAddressActionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticConfig_MACAddressAction,
                          (xLibU8_t *) & objMACAddressActionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = L7_SUCCESS;
    nextObjMACAddressActionValue=L7_GENERIC_OPTION_NONE;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objMACAddressActionValue, owa.len);
    switch(objMACAddressActionValue)
    {
      case L7_GENERIC_OPTION_NONE:
           nextObjMACAddressActionValue= L7_GENERIC_OPTION_ADD;
           owa.l7rc = L7_SUCCESS;
           break;
      case L7_GENERIC_OPTION_ADD:
           nextObjMACAddressActionValue= L7_GENERIC_OPTION_REMOVE;
           owa.l7rc = L7_SUCCESS;
           break;
      default:
             owa.l7rc = L7_FAILURE;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjMACAddressActionValue, owa.len);

  /* return the object value: MACAddressAction */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjMACAddressActionValue,
                           sizeof (nextObjMACAddressActionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingPortSecurityStaticConfig_MACAddressAction
*
* @purpose 	TODO 
* @description  
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingPortSecurityStaticConfig_MACAddressAction(void *wap, void *bufp)
{
 return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjSet_SwitchingPortSecurityStaticConfig_MACAddress
*
* @purpose Set 'MACAddress'
 *@description  [MACAddress] This MIB variable accepts a VLAN id and MAC address
* to be added to the list of statically locked MAC addresses on a
* port. The VLAN id and MAC address combination would be entered in
* a particular fashion like :- 2 a0:b0:c0:d1:e2:a1(the vlan-id and
* MAC addr   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingPortSecurityStaticConfig_MACAddress (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_enetMacAddr_t objMACAddressValue;

  xLibU32_t keyInterfaceValue;
  xLibU32_t keyMACAddressActionValue;
  xLibU32_t keyVlanIdValue;
  xLibU16_t vlanID = 0 ;

  FPOBJ_TRACE_ENTER (bufp);
  
  memset(&objMACAddressValue,0x0,sizeof(objMACAddressValue));
  /* retrieve object: MACAddress */
  owa.len = sizeof (objMACAddressValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objMACAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objMACAddressValue.addr, owa.len);
  
  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);
  
  /* retrieve key: MACAddressAction */
  owa.len = sizeof (keyMACAddressActionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticConfig_MACAddressAction,
                          (xLibU8_t *) & keyMACAddressActionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMACAddressActionValue, owa.len);
  
  
  /* retrieve key: VlanId */
  owa.len = sizeof (keyVlanIdValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_SwitchingPortSecurityStaticConfig_VlanId,
                                   (xLibU8_t *) & keyVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIdValue, owa.len);

  vlanID = keyVlanIdValue;
  
  if(!(objMACAddressValue.addr[0] | objMACAddressValue.addr[1] | objMACAddressValue.addr[2] | 
      objMACAddressValue.addr[3] | objMACAddressValue.addr[4] | objMACAddressValue.addr[5])) 
  {
    owa.rc = XLIBRC_INVALID_NON_ZERO_MAC;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  switch(keyMACAddressActionValue)
  {
    case L7_GENERIC_OPTION_ADD:
         owa.l7rc = usmDbPmlIntfStaticEntryAdd (L7_UNIT_CURRENT, keyInterfaceValue, (L7_enetMacAddr_t)objMACAddressValue, (L7_ushort16)vlanID); 
         break;
    case L7_GENERIC_OPTION_REMOVE:
          owa.l7rc = usmDbPmlIntfStaticEntryDelete (L7_UNIT_CURRENT, keyInterfaceValue, (L7_enetMacAddr_t)objMACAddressValue, (L7_ushort16)vlanID); 
         break;
    default:
            /*dont do anything*/
            owa.l7rc = L7_SUCCESS;  
  }

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
xLibRC_t fpObjSet_SwitchingPortSecurityStaticConfig_VlanId (void *wap, void *bufp)
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
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityStaticConfig_Interface,
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

