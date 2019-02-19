
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingStpSwitchMstVlanMapGroup.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  28 May 2008, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingStpSwitchMstVlanMapGroup_obj.h"
#include "usmdb_dot1s_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingStpSwitchMstVlanMapGroup_MstId
*
* @purpose Get 'MstId'
 *@description  [MstId] The MSTP instance ID   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpSwitchMstVlanMapGroup_MstId (void *wap, void *bufp)
{

  xLibU32_t objMstIdValue;
  xLibU32_t nextObjMstIdValue;
  xLibU32_t objVlanIndexValue;
  xLibU32_t nextObjVlanIndexValue, flag;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpSwitchMstVlanMapGroup_MstId,
                          (xLibU8_t *) & objMstIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objMstIdValue, 0, sizeof (objMstIdValue));
    memset (&objVlanIndexValue, 0, sizeof (objVlanIndexValue));
    owa.l7rc = usmDbDot1sInstanceVlanNextGet(L7_UNIT_CURRENT,
                                     objMstIdValue,&nextObjMstIdValue,
                                     objVlanIndexValue, &nextObjVlanIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objMstIdValue, owa.len);
    memset (&objVlanIndexValue, 0, sizeof (objVlanIndexValue));
    flag = L7_FALSE;
    do
    {
      if(flag == L7_TRUE)
      {
        objMstIdValue = nextObjMstIdValue;
        objVlanIndexValue = nextObjVlanIndexValue;
      } 
      owa.l7rc = usmDbDot1sInstanceVlanNextGet(L7_UNIT_CURRENT,
                                      objMstIdValue,
                                      &nextObjMstIdValue,objVlanIndexValue, 
                                      &nextObjVlanIndexValue);
      flag = L7_TRUE;
    }
    while ((objMstIdValue == nextObjMstIdValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjMstIdValue, owa.len);

  /* return the object value: MstId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjMstIdValue, sizeof (objMstIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingStpSwitchMstVlanMapGroup_VlanIndex
*
* @purpose Get 'VlanIndex'
 *@description  [VlanIndex] The VLAN-ID or other identifier refering to this VLAN   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpSwitchMstVlanMapGroup_VlanIndex (void *wap, void *bufp)
{

  xLibU32_t objMstIdValue;
  xLibU32_t nextObjMstIdValue;
  xLibU32_t objVlanIndexValue;
  xLibU32_t nextObjVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpSwitchMstVlanMapGroup_MstId,
                          (xLibU8_t *) & objMstIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objMstIdValue, owa.len);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpSwitchMstVlanMapGroup_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objVlanIndexValue, 0, sizeof (objVlanIndexValue));
    owa.l7rc = usmDbDot1sInstanceVlanNextGet(L7_UNIT_CURRENT,
                                     objMstIdValue,
                                     &nextObjMstIdValue, objVlanIndexValue, &nextObjVlanIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);

    owa.l7rc = usmDbDot1sInstanceVlanNextGet(L7_UNIT_CURRENT,
                                    objMstIdValue,
                                    &nextObjMstIdValue, objVlanIndexValue, &nextObjVlanIndexValue);

  }

  if ((objMstIdValue != nextObjMstIdValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanIndexValue, owa.len);

  /* return the object value: VlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanIndexValue, sizeof (objVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingStpSwitchMstVlanMapGroup_MstVlanRowStatus
*
* @purpose Get 'MstVlanRowStatus'
 *@description  [MstVlanRowStatus] The association status of an MSTP instance
* and a VLAN. Supported values: Add - used to create a new
* association between an MSTP instance and a VLAN. Delete- removes the
* association between an MSTP instance and a V   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpSwitchMstVlanMapGroup_MstVlanRowStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaMstId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMstVlanRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwaMstId.rc = xLibFilterGet (wap, XOBJ_SwitchingStpSwitchMstVlanMapGroup_MstId,
                               (xLibU8_t *) & keyMstIdValue, &kwaMstId.len);
  if (kwaMstId.rc != XLIBRC_SUCCESS)
  {
    kwaMstId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMstId);
    return kwaMstId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwaMstId.len);

  /* retrieve key: VlanIndex */
  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingStpSwitchMstVlanMapGroup_VlanIndex,
                                   (xLibU8_t *) & keyVlanIndexValue, &kwaVlanIndex.len);
  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwaVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);
    return kwaVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwaVlanIndex.len);

  /* get the value from application */
  objMstVlanRowStatusValue = L7_ROW_STATUS_ACTIVE;

  owa.l7rc = L7_SUCCESS;
            
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MstVlanRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMstVlanRowStatusValue,
                           sizeof (objMstVlanRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingStpSwitchMstVlanMapGroup_MstVlanRowStatus
*
* @purpose Set 'MstVlanRowStatus'
 *@description  [MstVlanRowStatus] The association status of an MSTP instance
* and a VLAN. Supported values: Add - used to create a new
* association between an MSTP instance and a VLAN. Delete- removes the
* association between an MSTP instance and a V   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStpSwitchMstVlanMapGroup_MstVlanRowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMstVlanRowStatusValue;

  fpObjWa_t kwaMstId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MstVlanRowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMstVlanRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMstVlanRowStatusValue, owa.len);

  /* retrieve key: MstId */
  kwaMstId.rc = xLibFilterGet (wap, XOBJ_SwitchingStpSwitchMstVlanMapGroup_MstId,
                               (xLibU8_t *) & keyMstIdValue, &kwaMstId.len);
  if (kwaMstId.rc != XLIBRC_SUCCESS)
  {
    kwaMstId.rc = XLIBRC_MST_ID_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaMstId);
    return kwaMstId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwaMstId.len);

  /* retrieve key: VlanIndex */
  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingStpSwitchMstVlanMapGroup_VlanIndex,
                                   (xLibU8_t *) & keyVlanIndexValue, &kwaVlanIndex.len);
  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwaVlanIndex.rc = XLIBRC_VLANID_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);
    return kwaVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwaVlanIndex.len);

  owa.l7rc = L7_SUCCESS;
  if (objMstVlanRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc = usmDbDot1sMstiVlanAdd(L7_UNIT_CURRENT, keyMstIdValue,
                                keyVlanIndexValue);
      if (owa.l7rc != L7_SUCCESS)
      {
         owa.rc = XLIBRC_MST_VLAN_ADD_FAILURE;    /* TODO: Change if required */
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;     
      }

  }
  else if (objMstVlanRowStatusValue == L7_ROW_STATUS_DESTROY)
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
/*******************************************************************************
* @function fpObjGet_SwitchingStpSwitchMstVlanMapGroup_MstVlanRowStatusEdit
*
* @purpose Get 'MstVlanRowStatus'
 *@description  [MstVlanRowStatus] The association status of an MSTP instance
* and a VLAN. Supported values: Add - used to create a new
* association between an MSTP instance and a VLAN. Delete- removes the
* association between an MSTP instance and a V   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpSwitchMstVlanMapGroup_MstVlanRowStatusEdit (void *wap, void *bufp)
{

  fpObjWa_t kwaMstId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMstVlanRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwaMstId.rc = xLibFilterGet (wap, XOBJ_SwitchingStpSwitchMstVlanMapGroup_MstId,
                               (xLibU8_t *) & keyMstIdValue, &kwaMstId.len);
  if (kwaMstId.rc != XLIBRC_SUCCESS)
  {
    kwaMstId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMstId);
    return kwaMstId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwaMstId.len);

  /* retrieve key: VlanIndex */
  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingStpSwitchMstVlanMapGroup_VlanIndex,
                                   (xLibU8_t *) & keyVlanIndexValue, &kwaVlanIndex.len);
  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwaVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);
    return kwaVlanIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwaVlanIndex.len);

  /* get the value from application */
  objMstVlanRowStatusValue = L7_ROW_STATUS_ACTIVE;

  owa.l7rc = L7_SUCCESS;
            
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MstVlanRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMstVlanRowStatusValue,
                           sizeof (objMstVlanRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_SwitchingStpSwitchMstVlanMapGroup_MstVlanRowStatusEdit
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
xLibRC_t fpObjSet_SwitchingStpSwitchMstVlanMapGroup_MstVlanRowStatusEdit (void *wap, void *bufp)
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
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpSwitchMstVlanMapGroup_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: MSTIndex */
  owa.len = sizeof (keyMstIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpSwitchMstVlanMapGroup_MstId,
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
     owa.l7rc = L7_SUCCESS;
     if ( keyMstIdValue == 0 )
     {
        owa.l7rc = usmDbDot1sVlanToMstiGet(L7_UNIT_CURRENT, keyVlanIndexValue, &keyMstIdValue);
        if (owa.l7rc != L7_SUCCESS)
        {
           owa.rc = XLIBRC_MST_VLAN_DEL_FAILURE ;    /* TODO: Change if required */
           FPOBJ_TRACE_EXIT (bufp, owa);
           return owa.rc;
        }
     }

     if ( keyMstIdValue != 0 )
     {
     owa.l7rc = usmDbDot1sMstiVlanRemove(L7_UNIT_CURRENT, keyMstIdValue,
                                   keyVlanIndexValue);
     }
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
