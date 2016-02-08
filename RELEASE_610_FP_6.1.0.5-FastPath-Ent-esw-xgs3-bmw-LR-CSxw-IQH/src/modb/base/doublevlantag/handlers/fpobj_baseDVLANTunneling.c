
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseDVLANTunneling.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  17 April 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Radha K
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseDVLANTunneling_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_dvlantag_api.h"

#define BASE_DECIMAL 10
/*******************************************************************************
* @function fpObjGet_baseDVLANTunneling_Interface
*
* @purpose Get 'Interface'
*
* @description [Interface] The Interface to be used.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDVLANTunneling_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_baseDVLANTunneling_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet(&nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbValidIntIfNumNext (objInterfaceValue,
                                     &nextObjInterfaceValue); 
  }
  if (owa.l7rc == L7_SUCCESS)
  {
    do
    {
      if(usmDbDvlantagIntfValidCheck(nextObjInterfaceValue) == L7_SUCCESS)
      {
        FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);
        /* return the object value: Interface */
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue, sizeof (objInterfaceValue));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
      owa.l7rc = usmDbValidIntIfNumNext (nextObjInterfaceValue,
                                     &nextObjInterfaceValue);
    }while(owa.l7rc == L7_SUCCESS);
  }
  owa.rc = XLIBRC_ENDOF_TABLE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseDVLANTunneling_Mode
*
* @purpose Get 'Mode'
*
* @description [Mode] This specifies the administrative mode via which Double VLAN Tagging can be enabled or disabled.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDVLANTunneling_Mode (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseDVLANTunneling_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDvlantagIntfModeGet(L7_UNIT_CURRENT, keyInterfaceValue, &objModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Mode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objModeValue, sizeof (objModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseDVLANTunneling_Mode
*
* @purpose Set 'Mode'
*
* @description [Mode] This specifies the administrative mode via which Double VLAN Tagging can be enabled or disabled.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDVLANTunneling_Mode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objModeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Mode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objModeValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseDVLANTunneling_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbDvlantagIntfModeSet(L7_UNIT_CURRENT, keyInterfaceValue, objModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseDVLANTunneling_CustID
*
* @purpose Get 'CustID'
*
* @description [CustID] The value.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDVLANTunneling_CustID (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCustIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseDVLANTunneling_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDvlantagIntfCustIdGet(L7_UNIT_CURRENT, keyInterfaceValue, &objCustIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CustID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCustIDValue, sizeof (objCustIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseDVLANTunneling_CustID
*
* @purpose Set 'CustID'
*
* @description [CustID] The value.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDVLANTunneling_CustID (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCustIDValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CustID */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCustIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCustIDValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseDVLANTunneling_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbDvlantagIntfCustIdSet(L7_UNIT_CURRENT, keyInterfaceValue, objCustIDValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseDVLANTunneling_EtherType
*
* @purpose Get 'EtherType'
*
* @description [EtherType] ToDO: Add Help
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDVLANTunneling_EtherType (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

    xLibStr256_t obj_ether_type_value = {0};
    xLibStr256_t tmpbuf = {0};  
    L7_uint32  tpidvalue, i;

    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (obj_ether_type_value));

    FPOBJ_TRACE_ENTER (bufp);
 
    /* retrieve object: Ether Type */
    owa.len = sizeof(obj_ether_type_value);

    owa.rc = xLibFilterGet (wap, XOBJ_baseDVLANTunneling_EtherType,
                          (xLibU8_t *) obj_ether_type_value, &owa.len);
    if (owa.rc == XLIBRC_SUCCESS) {
        owa.rc = XLIBRC_ENDOF_TABLE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseDVLANTunneling_Interface,
                                  (xLibU8_t *) & keyInterfaceValue, 
                                  &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  for(i=0; i<L7_DVLANTAG_MAX_TPIDS; i++)
  {
    usmDbDvlantagIntfEthertypeGet(L7_UNIT_CURRENT, keyInterfaceValue, 
                                  &tpidvalue, i);
    if(tpidvalue == 0)
      continue;
    osapiSnprintf(tmpbuf, sizeof(tmpbuf), "0x%x ", tpidvalue);
    strncat(obj_ether_type_value , tmpbuf, sizeof(tmpbuf));
  }

  /* return the object value: EtherType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &obj_ether_type_value, 
                           sizeof(obj_ether_type_value));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseDVLANTunneling_EtherType
*
* @purpose Set 'EtherType'
*
* @description [EtherType] ToDO: Add Help
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDVLANTunneling_EtherType (void *wap, void *bufp)
{

  fpObjWa_t     owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t  objEtherTypeValue_str = {0};
  xLibU32_t     objEtherTypeValue_uint;
  fpObjWa_t     kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t     keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve object: EtherType */
  owa.len = sizeof(objEtherTypeValue_str);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objEtherTypeValue_str, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEtherTypeValue_str, owa.len);


  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseDVLANTunneling_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  /* set the value in application */
  osapiStrtoul(objEtherTypeValue_str, BASE_DECIMAL, &objEtherTypeValue_uint);
  owa.l7rc = usmDbDvlantagIntfEthertypeSet (L7_UNIT_CURRENT, keyInterfaceValue, objEtherTypeValue_uint, L7_TRUE);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseDVLANTunneling_PortDVlanTagRowStatus
*
* @purpose Set 'PortDVlanTagRowStatus'
 *@description  [PortDVlanTagRowStatus] Get the RowStaus for the given TPID
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDVLANTunneling_PortDVlanTagRowStatus(void *wap, void *bufp)
{
     fpObjWa_t  owa = FPOBJ_INIT_WA2();
     FPOBJ_TRACE_ENTER(bufp);
     owa.rc = XLIBRC_FAILURE;   
     return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseDVLANTunneling_PortDVlanTagRowStatus
*
* @purpose Set 'PortDVlanTagRowStatus'
*@description  [PortDVlanTagRowStatus] Sets or Deletes the ethertype on the given interface.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDVLANTunneling_PortDVlanTagRowStatus(void *wap, void *bufp)
{
     fpObjWa_t     owa = FPOBJ_INIT_WA2 ();
     xLibStr256_t  obj_ether_type_value_str = {0};
     xLibU32_t     obj_ether_type_value_uint;
     xLibU32_t     interface, dvlan_mode;
     xLibU32_t     obj_dvlan_tag_row_status_value;

     FPOBJ_TRACE_ENTER (bufp);

     /* retrieve object: EtherType */
     owa.len = sizeof(obj_ether_type_value_str);

     owa.rc = xLibFilterGet(wap, XOBJ_baseDVLANTunneling_EtherType,
                            (xLibU8_t *) obj_ether_type_value_str, &owa.len);
     if (owa.rc != XLIBRC_SUCCESS) {
         owa.rc = XLIBRC_FILTER_MISSING;
         FPOBJ_TRACE_EXIT(bufp, owa);
         return owa.rc;
     }
     FPOBJ_TRACE_VALUE(bufp, &obj_ether_type_value_str, owa.len);

     /* retrieve key: Interface */
     owa.len = sizeof(interface);
     owa.rc = xLibFilterGet(wap, XOBJ_baseDVLANTunneling_Interface,
                            (xLibU8_t *)&interface, &owa.len);
     if (owa.rc != XLIBRC_SUCCESS)
     {
          owa.rc = XLIBRC_FILTER_MISSING;
          FPOBJ_TRACE_EXIT(bufp, owa);
          return owa.rc;
     }
     owa.len = sizeof(obj_dvlan_tag_row_status_value);

     /* retrieve object: DVlanTagRowStatus */
     owa.rc = xLibBufDataGet(bufp, (xLibU8_t *) &obj_dvlan_tag_row_status_value, 
                             &owa.len);
     if (owa.rc != XLIBRC_SUCCESS) {
         owa.rc = XLIBRC_FILTER_MISSING;
         FPOBJ_TRACE_EXIT(bufp, owa);
         return owa.rc;
     }
     owa.len = sizeof(dvlan_mode);

     /* retrieve object: DVlan Mode */
     owa.rc = xLibFilterGet(wap, XOBJ_baseDVLANTunneling_Mode,
                            (xLibU8_t *) &dvlan_mode, &owa.len);
     if ((XLIBRC_SUCCESS == owa.rc )&& 
         (obj_dvlan_tag_row_status_value != L7_ROW_STATUS_DESTROY)) {
           /* set the value in application */
         (void)usmDbDvlantagIntfModeSet(L7_UNIT_CURRENT, interface, dvlan_mode);
     } else {
         owa.rc = XLIBRC_SUCCESS;
     }
    
     /* set the value in application */
     if (osapiStrtoul(obj_ether_type_value_str, BASE_DECIMAL, &obj_ether_type_value_uint)
                     != L7_SUCCESS) {
         owa.rc = XLIBRC_FAILURE;
         FPOBJ_TRACE_EXIT(bufp, owa);
         return owa.rc;
     }
     if ((L7_ROW_STATUS_CREATE_AND_GO == obj_dvlan_tag_row_status_value)
         || (L7_ROW_STATUS_CREATE_AND_WAIT == obj_dvlan_tag_row_status_value)) {

         owa.l7rc = usmDbDvlantagIntfEthertypeSet(L7_UNIT_CURRENT,interface, obj_ether_type_value_uint,
                                                  L7_TRUE);
     } else if (L7_ROW_STATUS_DESTROY == obj_dvlan_tag_row_status_value) {

         owa.l7rc = usmDbDvlantagIntfEthertypeSet(L7_UNIT_CURRENT, interface, obj_ether_type_value_uint,
                                                  L7_FALSE);
     } else {
         owa.l7rc = L7_FAILURE;
     }

     if (L7_SUCCESS != owa.l7rc) {
         switch (owa.l7rc) {
             case L7_NOT_EXIST:
                 owa.rc = XLIBRC_TPID_NOT_EXIST;
                 break;
             case L7_NOT_SUPPORTED:
                 owa.rc = XLIBRC_DVLAN_MODE_NOT_ENABLED;
                 break;
             default:
                 owa.rc = XLIBRC_FAILURE;
                 break;
         }
     }
     FPOBJ_TRACE_EXIT(bufp, owa);
     return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseDVLANTunneling_EtherTypeEnum
*
* @purpose Get 'EtherType'
*
* @description [EtherType] ToDO: Add Help
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDVLANTunneling_EtherTypeEnum (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEtherTypeValue;
  xLibU32_t EtherTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseDVLANTunneling_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDvlantagIntfEthertypeGet (L7_UNIT_CURRENT, keyInterfaceValue, &EtherTypeValue,0);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* This is a kind of hack but forcebly done at the UI level because 
   * the underlying USMDB or application doesn't have a support of setting 
   * or getting the enum. Hence the ether type value is mapped according to the enum
   * and given to the UI Interface
   */
  /*    Commonly used are the Ethertypes for vman tags (34984, or 0x88A8) and dot1q
   *    tags (33024, or 0x8100)
   */

   switch(EtherTypeValue)
   {
     case 33024:
               objEtherTypeValue = L7_XUI_DVLANTAG802Q_ETHERTYPE;
           break;
     case 34984:
               objEtherTypeValue = L7_XUI_DVLANTAGVMAN_ETHERTYPE;
           break;
     default:
               objEtherTypeValue = L7_XUI_DVLANTAGCUSTOM_ETHERTYPE;
           break;
    }
 
  /* return the object value: EtherType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEtherTypeValue, sizeof (objEtherTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseDVLANTunneling_EtherTypeEnum
*
* @purpose Set 'EtherType'
*
* @description [EtherType] ToDO: Add Help
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDVLANTunneling_EtherTypeEnum (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEtherTypeValue;
  xLibU32_t EtherTypeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EtherType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEtherTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEtherTypeValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseDVLANTunneling_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);
  /* This is a kind of hack but forcebly done at the UI level because 
   * the underlying USMDB or application doesn't have a support of setting 
   * or getting the enum. Hence the ether type value is mapped according to the enum
   * and given to the UI Interface
   */
  /*    Commonly used are the Ethertypes for vman tags (34984, or 0x88A8) and dot1q
   *    tags (33024, or 0x8100)
   */

  switch(objEtherTypeValue)
  {
    case L7_XUI_DVLANTAG802Q_ETHERTYPE:
         EtherTypeValue = 33024;
         break;
    case L7_XUI_DVLANTAGVMAN_ETHERTYPE:
         EtherTypeValue = 34984;
         break;
    default:
         EtherTypeValue = objEtherTypeValue;
         break;
   } 
  /* set the value in application 
  owa.l7rc = usmDbDvlantagIntfEthertypeSet (L7_UNIT_CURRENT, keyInterfaceValue, EtherTypeValue,0);
   */
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_baseDVLANTunneling_GlobalMode
*
* @purpose Set 'GlobalMode'
 *@description  [GlobalMode] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDVLANTunneling_GlobalMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalModeValue;
  xLibU32_t interface, nextInterface;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalModeValue, owa.len);

  /* set the value in application */

  if (usmDbValidIntIfNumFirstGet(&interface) == L7_SUCCESS)
  {
    while (interface != 0)
    {
      owa.l7rc = usmDbDvlantagIntfModeSet(L7_UNIT_CURRENT, interface, objGlobalModeValue);
      if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
      {
        interface = nextInterface;
      }
      else
      {
        interface = 0;
      }
    }
  }
  owa.l7rc = L7_SUCCESS;

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseDVLANTunneling_AllIntfEtherType
*
* @purpose Set 'AllIntfEtherType'
 *@description  [AllIntfEtherType] To Set the given ether type on all the interfaces.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDVLANTunneling_AllIntfEtherType (void *wap, void *bufp)
{
     fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
     xLibU32_t ether_type;
     xLibU32_t interface, next_interface;

     FPOBJ_TRACE_ENTER (bufp);

     /* retrieve object: Ether Type */
     owa.rc = xLibBufDataGet(bufp, (xLibU8_t *) & ether_type, &owa.len);
     if (owa.rc != XLIBRC_SUCCESS)
     {    
          owa.rc = XLIBRC_FILTER_MISSING;
          FPOBJ_TRACE_EXIT(bufp, owa);
          return owa.rc;
     }
     FPOBJ_TRACE_VALUE (bufp, &ether_type, owa.len);

     /* set the value in application */

     if (usmDbValidIntIfNumFirstGet(&interface) == L7_SUCCESS) {
         while (interface != 0) {
             owa.l7rc = usmDbDvlantagIntfEthertypeSet(L7_UNIT_CURRENT, 
                                                      interface, ether_type, 
                                                      L7_TRUE);
             if (usmDbValidIntIfNumNext(interface, &next_interface) 
                                        == L7_SUCCESS) {
                 interface = next_interface;
             } else {
                 interface = 0;
             }
         }
     }
     owa.l7rc = L7_SUCCESS;
     FPOBJ_TRACE_EXIT(bufp, owa);
     return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDVLANTunneling_GlobalCustID
*
* @purpose Set 'GlobalCustID'
 *@description  [GlobalCustID] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDVLANTunneling_GlobalCustID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalCustIDValue;
  xLibU32_t interface, nextInterface;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalCustID */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalCustIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalCustIDValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  if (usmDbValidIntIfNumFirstGet(&interface) == L7_SUCCESS)
  {
    while (interface != 0)
    {
      owa.l7rc = usmDbDvlantagIntfCustIdSet(L7_UNIT_CURRENT, interface, objGlobalCustIDValue);
      if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
      {
        interface = nextInterface;
      }
      else
      {
        interface = 0;
      }
    }
  }
  owa.l7rc = L7_SUCCESS;
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseDVLANTunneling_GlobalEtherType
*
* @purpose Set 'GlobalEtherType'
 *@description  [GlobalEtherType] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDVLANTunneling_GlobalEtherType(void *wap, void *bufp)
{
  fpObjWa_t  owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objEtherTypeValue = {0};
  xLibStr256_t tmpbuf = {0};  
  L7_uint32  tpidValue, i;
  FPOBJ_TRACE_ENTER (bufp);

 /* retrieve object: Global Ether Type */
  owa.len = sizeof(objEtherTypeValue);

  owa.rc = xLibFilterGet(wap, XOBJ_baseDVLANTunneling_GlobalEtherType,
                         (xLibU8_t *) objEtherTypeValue, &owa.len);
  if (owa.rc == XLIBRC_SUCCESS) {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  /* get the value from application */
  for(i=0; i<L7_DVLANTAG_MAX_TPIDS; i++)
  {
    usmDbDvlantagEthertypeGet(L7_UNIT_CURRENT, &tpidValue, i);
    if(tpidValue == 0)
      continue;
    osapiSnprintf(tmpbuf, sizeof(tmpbuf), "0x%x ", tpidValue);
    strncat(objEtherTypeValue , tmpbuf, sizeof(tmpbuf));
  }

  /* return the object value: EtherType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEtherTypeValue, sizeof (objEtherTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_baseDVLANTunneling_GlobalEtherType
*
* @purpose Set 'GlobalEtherType'
 *@description  [GlobalEtherType] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDVLANTunneling_GlobalEtherType (void *wap, void *bufp)
{
  fpObjWa_t     owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t  objGlobalEtherTypeValue_str = {0};
  xLibU32_t     objGlobalEtherTypeValue_uint; 

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalEtherType */
  owa.len = sizeof(objGlobalEtherTypeValue_str);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objGlobalEtherTypeValue_str, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalEtherTypeValue_str, owa.len);

  /* set the value in application */
  osapiStrtoul(objGlobalEtherTypeValue_str, BASE_DECIMAL, &objGlobalEtherTypeValue_uint);
  owa.l7rc = usmDbDvlantagEthertypeSet(L7_UNIT_CURRENT, objGlobalEtherTypeValue_uint,
                                       L7_FALSE, L7_TRUE);

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDVLANTunneling_GlobalEtherType
*
* @purpose Set 'GlobalEtherType'
 *@description  [GlobalEtherType] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDVLANTunneling_GlobalDefaultTpid (void *wap, void *bufp)
{
  fpObjWa_t  owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objEtherTypeValue = {0};
  L7_uint32  tpidValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  usmDbDvlantagEthertypeGet(L7_UNIT_CURRENT, &tpidValue, 0);
  osapiSnprintf(objEtherTypeValue, sizeof(objEtherTypeValue), "0x%x", tpidValue);

  /* return the object value: EtherType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEtherTypeValue, sizeof (objEtherTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseDVLANTunneling_GlobalEtherType
*
* @purpose Set 'GlobalEtherType'
 *@description  [GlobalEtherType] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDVLANTunneling_GlobalDefaultTpid (void *wap, void *bufp)
{
  fpObjWa_t     owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t  objGlobalEtherTypeValue_str = {0};
  xLibU32_t     objGlobalEtherTypeValue_uint; 

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalEtherType */
  owa.len = sizeof(objGlobalEtherTypeValue_str);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objGlobalEtherTypeValue_str, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalEtherTypeValue_str, owa.len);

  /* set the value in application */
  osapiStrtoul(objGlobalEtherTypeValue_str, BASE_DECIMAL, &objGlobalEtherTypeValue_uint);
  owa.l7rc = usmDbDvlantagEthertypeSet(L7_UNIT_CURRENT, objGlobalEtherTypeValue_uint,
                                       L7_TRUE, L7_TRUE);

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;


}

/*******************************************************************************
* @function fpObjGet_baseDVLANTunneling_GlobalDVlanTagRowStatus
*
* @purpose Set 'GlobalDVlanTagRowStatus'
 *@description  [GlobalDVlanTagRowStatus] Get the RowStaus for the given TPID
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDVLANTunneling_GlobalDVlanTagRowStatus(void *wap, void *bufp)
{
     fpObjWa_t  owa = FPOBJ_INIT_WA2 ();
     xLibStr256_t obj_ether_type_value_str = {0};
     L7_uint32  tpid_value, obj_ether_type_value_uint, i, status;
     FPOBJ_TRACE_ENTER (bufp);

     owa.len = sizeof(obj_ether_type_value_str);
     owa.rc = xLibFilterGet (wap, XOBJ_baseDVLANTunneling_GlobalEtherType,
                          (xLibU8_t *) obj_ether_type_value_str, &owa.len);
     if (owa.rc != XLIBRC_SUCCESS) {
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
     }
     if (osapiStrtoul(obj_ether_type_value_str, BASE_DECIMAL, &obj_ether_type_value_uint)
                      != L7_SUCCESS) {
         owa.rc = XLIBRC_FAILURE;
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
     }
 
     /* get the value from application */
     owa.l7rc = L7_FAILURE;
     owa.rc = XLIBRC_FAILURE;
     for(i=0; i<L7_DVLANTAG_MAX_TPIDS; i++)
     {
          usmDbDvlantagEthertypeGet(L7_UNIT_CURRENT, &tpid_value, i);
          if (tpid_value == obj_ether_type_value_uint) {
              owa.l7rc = L7_SUCCESS;
              status = L7_ROW_STATUS_ACTIVE;
              break;
          }
     }
     if (L7_SUCCESS == owa.l7rc) {
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & status, sizeof (status));
     }
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_baseDVLANTunneling_GlobalDVlanTagRowStatus
*
* @purpose Set 'GlobalDVlanTagRowStatus'
 *@description  [GlobalDVlanTagRowStatus] Create or Deletes the ethertype.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDVLANTunneling_GlobalDVlanTagRowStatus(void *wap, void *bufp)
{
     fpObjWa_t     owa = FPOBJ_INIT_WA2 ();
     xLibStr256_t  obj_global_ether_type_value_str = {0};
     xLibU32_t     obj_global_ether_type_value_uint;
     xLibU32_t     obj_global_dvlan_tag_row_status_value;
     xLibU32_t     defaultTpid = L7_FALSE;


     FPOBJ_TRACE_ENTER (bufp);

     /* retrieve object: Global Ether Type */
     owa.len = sizeof(obj_global_ether_type_value_str);

     owa.rc = xLibFilterGet (wap, XOBJ_baseDVLANTunneling_GlobalEtherType,
                             (xLibU8_t *) obj_global_ether_type_value_str, 
                             &owa.len);
     if (owa.rc != XLIBRC_SUCCESS) {
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
     }
     FPOBJ_TRACE_VALUE (bufp, &obj_global_ether_type_value_str, owa.len);

     owa.len = sizeof(obj_global_dvlan_tag_row_status_value);
     /* retrieve object: Global DVlan Tag RowStatus */
     owa.rc = xLibBufDataGet (bufp, 
                              (xLibU8_t *) &obj_global_dvlan_tag_row_status_value, 
                              &owa.len);
     if (owa.rc != XLIBRC_SUCCESS) {
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
     }
  
     if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_DVLANTAG_COMPONENT_ID, 
         L7_DVLANTAG_MULTI_TPIDS_FEATURE_ID) != L7_TRUE) {
         defaultTpid = L7_TRUE;
     }
     /* set the value in application */
     if (osapiStrtoul(obj_global_ether_type_value_str, BASE_DECIMAL, 
         &obj_global_ether_type_value_uint) != L7_SUCCESS) {
         owa.rc = XLIBRC_FAILURE;
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
     } 
     if ((L7_ROW_STATUS_CREATE_AND_GO == obj_global_dvlan_tag_row_status_value) 
         || (L7_ROW_STATUS_CREATE_AND_WAIT == obj_global_dvlan_tag_row_status_value)) {
    
         owa.l7rc = usmDbDvlantagEthertypeSet(L7_UNIT_CURRENT, 
                                              obj_global_ether_type_value_uint,
                                              defaultTpid, L7_TRUE);
     }
     else if (L7_ROW_STATUS_DESTROY == obj_global_dvlan_tag_row_status_value) {
         owa.l7rc = usmDbDvlantagEthertypeSet(L7_UNIT_CURRENT, 
                                              obj_global_ether_type_value_uint,
                                              defaultTpid, L7_FALSE);
     }
     else {
         owa.l7rc = L7_FAILURE;
     }
     if (L7_SUCCESS != owa.l7rc) {
         switch (owa.l7rc) {
             case L7_NOT_EXIST:
                 owa.rc = XLIBRC_TPID_INDEX_NOT_FREE;
                 break;
             case L7_ALREADY_CONFIGURED:
                 owa.rc = XLIBRC_TPID_ALREADY_CONFIGURED;
                 break;
             default:
                 owa.rc = XLIBRC_FAILURE;
                 break;
         }
     }

     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;

}

