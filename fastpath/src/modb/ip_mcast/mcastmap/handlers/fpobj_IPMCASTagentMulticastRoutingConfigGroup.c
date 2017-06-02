
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_IPMCASTagentMulticastRoutingConfigGroup.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to IPMCAST-object.xml
*
* @create  02 May 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_IPMCASTagentMulticastRoutingConfigGroup_obj.h"
#include "_xe_IPMCASTagentMulticastPIMConfigGroup_obj.h"
#include "usmdb_mib_mcast_api.h"
#include "usmdb_mib_pim_api.h"
#include "usmdb_pimsm_api.h"

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastRoutingConfigGroup_agentMulticastRoutingAdminMode
*
* @purpose Get 'agentMulticastRoutingAdminMode'
*
* @description [agentMulticastRoutingAdminMode] This enables or disables Multicast Routing Mode on the system.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastRoutingConfigGroup_agentMulticastRoutingAdminMode (void *wap,
                                                                                          void
                                                                                          *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastRoutingAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbMcastAdminModeGet(L7_UNIT_CURRENT, &objagentMulticastRoutingAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objagentMulticastRoutingAdminModeValue,
                     sizeof (objagentMulticastRoutingAdminModeValue));

  /* return the object value: agentMulticastRoutingAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objagentMulticastRoutingAdminModeValue,
                           sizeof (objagentMulticastRoutingAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentMulticastRoutingConfigGroup_agentMulticastRoutingAdminMode
*
* @purpose Set 'agentMulticastRoutingAdminMode'
*
* @description [agentMulticastRoutingAdminMode] This enables or disables Multicast Routing Mode on the system.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastRoutingConfigGroup_agentMulticastRoutingAdminMode (void *wap,
                                                                                          void
                                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastRoutingAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: agentMulticastRoutingAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objagentMulticastRoutingAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objagentMulticastRoutingAdminModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbMcastAdminModeSet(L7_UNIT_CURRENT, objagentMulticastRoutingAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMFamily
*
* @purpose Get 'agentMulticastPIMFamily'
*
* @description [agentMulticastPIMFamily] Address Family
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMFamily (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMFamilyValue;
  xLibU32_t nextObjagentMulticastPIMFamilyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPIMFamily */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMFamily,
                          (xLibU8_t *) & objagentMulticastPIMFamilyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjagentMulticastPIMFamilyValue = AF_INET;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    if(objagentMulticastPIMFamilyValue == AF_INET)
    {
       FPOBJ_TRACE_CURRENT_KEY (bufp, &objagentMulticastPIMFamilyValue, owa.len);
       nextObjagentMulticastPIMFamilyValue = AF6_INET;
       owa.l7rc = L7_SUCCESS;
    }
	else
	{
	  owa.l7rc = L7_FAILURE;
	}
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjagentMulticastPIMFamilyValue, owa.len);

  /* return the object value: agentMulticastPIMFamily */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjagentMulticastPIMFamilyValue,
                           sizeof (objagentMulticastPIMFamilyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPimProtocolType
*
* @purpose Get 'agentMulticastPimProtocolType'
*
* @description [agentMulticastPimProtocolType] PIM Protocol Type (SM or DM)
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPimProtocolType (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPimProtocolTypeValue;
  xLibU32_t nextObjagentMulticastPimProtocolTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPimProtocolType */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPimProtocolType,
                          (xLibU8_t *) & objagentMulticastPimProtocolTypeValue, &owa.len);


  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjagentMulticastPimProtocolTypeValue = L7_XUI_CURRENT_PIM_MODE_DM;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    if(objagentMulticastPimProtocolTypeValue == L7_XUI_CURRENT_PIM_MODE_DM)
    {
       FPOBJ_TRACE_CURRENT_KEY (bufp, &objagentMulticastPimProtocolTypeValue, owa.len);
       nextObjagentMulticastPimProtocolTypeValue = L7_XUI_CURRENT_PIM_MODE_SM;
       owa.l7rc = L7_SUCCESS;
    }
	else
	{
	  owa.l7rc = L7_FAILURE;
	}
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjagentMulticastPimProtocolTypeValue, owa.len);

  /* return the object value: agentMulticastPimProtocolType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjagentMulticastPimProtocolTypeValue,
                           sizeof (objagentMulticastPimProtocolTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastEnabledPimProtocolType
*
* @purpose Get 'agentMulticastEnabledPimProtocolType'
*
* @description [agentMulticastEnabledPimProtocolType] PIM Protocol Type (SM or DM)
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastEnabledPimProtocolType (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastEnabledPimProtocolType, pimdmMode = L7_DISABLE;
  fpObjWa_t kwaagentMulticastPIMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMFamilyValue;

  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve key: agentMulticastPIMFamily */
  kwaagentMulticastPIMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMFamilyValue,
                   &kwaagentMulticastPIMFamily.len);
  if (kwaagentMulticastPIMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMFamily);
    return kwaagentMulticastPIMFamily.rc;
  }


  owa.l7rc = usmDbPimRouterAdminModeGet(L7_UNIT_CURRENT, keyagentMulticastPIMFamilyValue, &pimdmMode);
  if (owa.l7rc  == L7_SUCCESS && pimdmMode == L7_ENABLE)
  {
    objagentMulticastEnabledPimProtocolType = L7_XUI_CURRENT_PIM_MODE_DM;
  }
  else
  {
    objagentMulticastEnabledPimProtocolType = L7_XUI_CURRENT_PIM_MODE_SM;
  }


  owa.l7rc = L7_SUCCESS;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &objagentMulticastEnabledPimProtocolType, owa.len);

  /* return the object value: objagentMulticastEnabledPimProtocolType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objagentMulticastEnabledPimProtocolType,
                           sizeof (objagentMulticastEnabledPimProtocolType));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMAdminMode
*
* @purpose Get 'agentMulticastPIMAdminMode'
*
* @description [agentMulticastPIMAdminMode] enables or disables PIM Mode on the system
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMAdminMode (void *wap,
                                                                                      void *bufp)
{

  fpObjWa_t kwaagentMulticastPIMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMFamilyValue;

  fpObjWa_t kwaagentMulticastPimProtocolType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPimProtocolTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPIMFamily */
  kwaagentMulticastPIMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMFamilyValue,
                   &kwaagentMulticastPIMFamily.len);
  if (kwaagentMulticastPIMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMFamily);
    return kwaagentMulticastPIMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMFamilyValue,
                           kwaagentMulticastPIMFamily.len);

  /* retrieve key: agentMulticastPimProtocolType */
  kwaagentMulticastPimProtocolType.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPimProtocolType,
                   (xLibU8_t *) & keyagentMulticastPimProtocolTypeValue,
                   &kwaagentMulticastPimProtocolType.len);
  if (kwaagentMulticastPimProtocolType.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPimProtocolType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPimProtocolType);
    return kwaagentMulticastPimProtocolType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPimProtocolTypeValue,
                           kwaagentMulticastPimProtocolType.len);

  if (keyagentMulticastPimProtocolTypeValue == L7_XUI_CURRENT_PIM_MODE_DM)
  {
    /* get the value from application */
    owa.l7rc = usmDbPimRouterAdminModeGet(L7_UNIT_CURRENT, keyagentMulticastPIMFamilyValue,
                                &objagentMulticastPIMAdminModeValue);
  }
  else if (keyagentMulticastPimProtocolTypeValue == L7_XUI_CURRENT_PIM_MODE_SM)
  {
    /* get the value from application */
    owa.l7rc = usmDbPimsmAdminModeGet(L7_UNIT_CURRENT, keyagentMulticastPIMFamilyValue,
                                &objagentMulticastPIMAdminModeValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: agentMulticastPIMAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objagentMulticastPIMAdminModeValue,
                           sizeof (objagentMulticastPIMAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMAdminMode
*
* @purpose Set 'agentMulticastPIMAdminMode'
*
* @description [agentMulticastPIMAdminMode] enables or disables PIM Mode on the system
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMAdminMode (void *wap,
                                                                                      void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMAdminModeValue;

  fpObjWa_t kwaagentMulticastPimProtocolType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPimProtocolTypeValue;

  fpObjWa_t kwaagentMulticastPIMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMFamilyValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: agentMulticastPIMAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objagentMulticastPIMAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objagentMulticastPIMAdminModeValue, owa.len);

  /* retrieve key: agentMulticastPIMFamily */
  kwaagentMulticastPIMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMFamilyValue,
                   &kwaagentMulticastPIMFamily.len);
  if (kwaagentMulticastPIMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMFamily);
    return kwaagentMulticastPIMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMFamilyValue,
                           kwaagentMulticastPIMFamily.len);

  /* retrieve key: agentMulticastPimProtocolType */
  kwaagentMulticastPimProtocolType.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPimProtocolType,
                   (xLibU8_t *) & keyagentMulticastPimProtocolTypeValue,
                   &kwaagentMulticastPimProtocolType.len);
  if (kwaagentMulticastPimProtocolType.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPimProtocolType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPimProtocolType);
    return kwaagentMulticastPimProtocolType.rc;
  }

  if (keyagentMulticastPimProtocolTypeValue == L7_XUI_CURRENT_PIM_MODE_DM)
  {
    /* set the value in application */
    owa.l7rc = usmDbPimRouterAdminModeSet (L7_UNIT_CURRENT, keyagentMulticastPIMFamilyValue,
                              objagentMulticastPIMAdminModeValue);
  }
  else if(keyagentMulticastPimProtocolTypeValue == L7_XUI_CURRENT_PIM_MODE_SM)
  {
    /* set the value in application */
    owa.l7rc = usmDbPimsmAdminModeSet (L7_UNIT_CURRENT, keyagentMulticastPIMFamilyValue,
                              objagentMulticastPIMAdminModeValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    if(owa.l7rc == L7_ERROR) 
      owa.rc = XLIBRC_IPMCAST_ALREADY_SET; 
    else
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMSMDataThresholdRate
*
* @purpose Get 'agentMulticastPIMSMDataThresholdRate'
*
* @description [agentMulticastPIMSMDataThresholdRate] Specifies the minimum rate in Kbits/sec
*              for the last hop router to initiate switching to the shortest path.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMSMDataThresholdRate (void
                                                                                              *wap,
                                                                                              void
                                                                                              *bufp)
{

  fpObjWa_t kwaagentMulticastPIMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMFamilyValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMSMDataThresholdRateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPIMFamily */
  kwaagentMulticastPIMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMFamilyValue,
                   &kwaagentMulticastPIMFamily.len);
  if (kwaagentMulticastPIMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMFamily);
    return kwaagentMulticastPIMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMFamilyValue,
                           kwaagentMulticastPIMFamily.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmDataThresholdRateGet(L7_UNIT_CURRENT, keyagentMulticastPIMFamilyValue,
                              &objagentMulticastPIMSMDataThresholdRateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: agentMulticastPIMSMDataThresholdRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objagentMulticastPIMSMDataThresholdRateValue,
                           sizeof (objagentMulticastPIMSMDataThresholdRateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMSMDataThresholdRate
*
* @purpose Set 'agentMulticastPIMSMDataThresholdRate'
*
* @description [agentMulticastPIMSMDataThresholdRate] Specifies the minimum rate in Kbits/sec
*              for the last hop router to initiate switching to the shortest path.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMSMDataThresholdRate (void
                                                                                              *wap,
                                                                                              void
                                                                                              *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMSMDataThresholdRateValue;

  fpObjWa_t kwaagentMulticastPIMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMFamilyValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: agentMulticastPIMSMDataThresholdRate */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objagentMulticastPIMSMDataThresholdRateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objagentMulticastPIMSMDataThresholdRateValue, owa.len);

  /* retrieve key: agentMulticastPIMFamily */
  kwaagentMulticastPIMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMFamilyValue,
                   &kwaagentMulticastPIMFamily.len);
  if (kwaagentMulticastPIMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMFamily);
    return kwaagentMulticastPIMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMFamilyValue,
                           kwaagentMulticastPIMFamily.len);

  /* set the value in application */
  owa.l7rc = usmDbPimsmDataThresholdRateSet(L7_UNIT_CURRENT, keyagentMulticastPIMFamilyValue,
                              objagentMulticastPIMSMDataThresholdRateValue);

  if (owa.l7rc == L7_NOT_SUPPORTED)
  {
   owa.rc = XLIBRC_PIM_NON_ZERO_SPT_THRESHOLD;
  }
  else if (owa.l7rc == L7_SUCCESS)
  {
    owa.rc = XLIBRC_SUCCESS;
  }
  else
  {
    owa.rc = XLIBRC_FAILURE;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMSMRegThresholdRate
*
* @purpose Get 'agentMulticastPIMSMRegThresholdRate'
*
* @description [agentMulticastPIMSMRegThresholdRate] Specifies the minimum rate in Kbits/sec 
*              for the RP to switch to the shortest path.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMSMRegThresholdRate (void
                                                                                             *wap,
                                                                                             void
                                                                                             *bufp)
{

  fpObjWa_t kwaagentMulticastPIMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMFamilyValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMSMRegThresholdRateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPIMFamily */
  kwaagentMulticastPIMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMFamilyValue,
                   &kwaagentMulticastPIMFamily.len);
  if (kwaagentMulticastPIMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMFamily);
    return kwaagentMulticastPIMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMFamilyValue,
                           kwaagentMulticastPIMFamily.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmRegisterThresholdRateGet(L7_UNIT_CURRENT, keyagentMulticastPIMFamilyValue,
                              &objagentMulticastPIMSMRegThresholdRateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: agentMulticastPIMSMRegThresholdRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objagentMulticastPIMSMRegThresholdRateValue,
                           sizeof (objagentMulticastPIMSMRegThresholdRateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMSMRegThresholdRate
*
* @purpose Set 'agentMulticastPIMSMRegThresholdRate'
*
* @description [agentMulticastPIMSMRegThresholdRate] Specifies the minimum rate in Kbits/sec 
*              for the RP to switch to the shortest path.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMSMRegThresholdRate (void
                                                                                             *wap,
                                                                                             void
                                                                                             *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMSMRegThresholdRateValue;

  fpObjWa_t kwaagentMulticastPIMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMFamilyValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: agentMulticastPIMSMRegThresholdRate */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objagentMulticastPIMSMRegThresholdRateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objagentMulticastPIMSMRegThresholdRateValue, owa.len);

  /* retrieve key: agentMulticastPIMFamily */
  kwaagentMulticastPIMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMConfigGroup_agentMulticastPIMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMFamilyValue,
                   &kwaagentMulticastPIMFamily.len);
  if (kwaagentMulticastPIMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMFamily);
    return kwaagentMulticastPIMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMFamilyValue,
                           kwaagentMulticastPIMFamily.len);

  /* set the value in application */
  owa.l7rc = usmDbPimsmRegisterThresholdRateSet(L7_UNIT_CURRENT, keyagentMulticastPIMFamilyValue,
                              objagentMulticastPIMSMRegThresholdRateValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

