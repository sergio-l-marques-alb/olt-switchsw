
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
********************************************************************************
*
* @filename fpobj_SwitchingStpMstVlanMapGroup.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  28 January 2009, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Vijayanand Kamasamudram
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingStpMstVlanMapGroup_obj.h"
#include "usmdb_dot1s_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingStpMstVlanMapGroup_MstIdEdit
*
* @purpose Get 'MstIdEdit'
 *@description  [MstIdEdit] The MSTP instance ID   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstVlanMapGroup_MstIdEdit (void *wap, void *bufp)
{
  return  XLIBRC_SUCCESS; 
}

/*******************************************************************************
* @function fpObjSet_SwitchingStpMstVlanMapGroup_MstIdEdit
*
* @purpose Set 'MstIdEdit'
 *@description  [MstIdEdit] The MSTP instance ID   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStpMstVlanMapGroup_MstIdEdit (void *wap, void *bufp)
{
   return  XLIBRC_SUCCESS; 
}

/*******************************************************************************
* @function fpObjGet_SwitchingStpMstVlanMapGroup_VlanIndex
*
* @purpose Get 'VlanIndex'
 *@description  [VlanIndex] The VLAN-ID or other identifier refering to this VLAN   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstVlanMapGroup_VlanIndex (void *wap, void *bufp)
{
   return  XLIBRC_SUCCESS; 

}

/*******************************************************************************
* @function fpObjGet_SwitchingStpMstVlanMapGroup_MstVlanRowStatusEdit
*
* @purpose Get 'MstVlanRowStatusEdit'
 *@description  [MstVlanRowStatusEdit] The association status of an MSTP
* instance and a VLAN. Supported values: Add - used to create a new
* association between an MSTP instance and a VLAN. Delete- removes the
* association between an MSTP instance and a V   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstVlanMapGroup_MstVlanRowStatusEdit (void *wap, void *bufp)
{
   return  XLIBRC_SUCCESS; 
}

/*******************************************************************************
* @function fpObjSet_SwitchingStpMstVlanMapGroup_MstVlanRowStatusEdit
*
* @purpose Set 'MstVlanRowStatusEdit'
 *@description  [MstVlanRowStatusEdit] The association status of an MSTP
* instance and a VLAN. Supported values: Add - used to create a new
* association between an MSTP instance and a VLAN. Delete- removes the
* association between an MSTP instance and a V   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStpMstVlanMapGroup_MstVlanRowStatusEdit (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objMstVlanRowStatusEditValue;

  xLibU32_t keyVlanIndexValue;
  xLibU32_t keyMstIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MstVlanRowStatusEdit */
  owa.len = sizeof (objMstVlanRowStatusEditValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMstVlanRowStatusEditValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMstVlanRowStatusEditValue, owa.len);

  /* retrieve key: VlanIndex */
  owa.len = sizeof (keyVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstVlanMapGroup_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: MSTIndex */
  owa.len = sizeof (keyMstIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstVlanMapGroup_MstIdEdit,
                          (xLibU8_t *) & keyMstIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (objMstVlanRowStatusEditValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    if (usmDbDot1sInstanceGet(L7_UNIT_CURRENT, keyMstIdValue) != L7_SUCCESS)
    {
      owa.l7rc = usmDbDot1sMstiCreate (L7_UNIT_CURRENT,keyMstIdValue);
      /* As the MST instance creation is a Event
       * delay is added to this to wait for 2 seconds
       * so that the dependent objects will get the appropriate key
       * for configuration. This is useful when both MST instance
       * and its dependent paramenters are configured in a single shot
       */
      if(usmDbdot1sInstCheckInUse(L7_UNIT_CURRENT,keyMstIdValue) != L7_SUCCESS)
      {
        osapiSleep(2);
      }
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_MST_INSTANCE_CREATE_FAILURE;  /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    owa.l7rc = usmDbDot1sMstiVlanAdd(L7_UNIT_CURRENT, keyMstIdValue,
                               keyVlanIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
       owa.rc = XLIBRC_MST_VLAN_ADD_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;     
    }
  }
  else if (objMstVlanRowStatusEditValue == L7_ROW_STATUS_DESTROY)
  {
     owa.l7rc = usmDbDot1sMstiVlanRemove(L7_UNIT_CURRENT, keyMstIdValue,
                                   keyVlanIndexValue);
      if (owa.l7rc != L7_SUCCESS)
      {
         owa.rc = XLIBRC_MST_VLAN_DEL_FAILURE ;    /* TODO: Change if required */
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
