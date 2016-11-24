
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_IPMCASTagentMulticastPIMSMConfigGroup.c
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
#include "_xe_IPMCASTagentMulticastPIMSMConfigGroup_obj.h"
#include "usmdb_pimsm_api.h"
#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_pim_api.h"
#endif


/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMFamily
*
* @purpose Get 'agentMulticastPIMSMFamily'
*
* @description [agentMulticastPIMSMFamily] Address Family
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMFamily (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMSMFamilyValue;
  xLibU32_t nextObjagentMulticastPIMSMFamilyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPIMSMFamily */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMFamily,
                          (xLibU8_t *) & objagentMulticastPIMSMFamilyValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjagentMulticastPIMSMFamilyValue = AF_INET;
/*  owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, &nextObjagentMulticastPIMSMFamilyValue);*/
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    if( objagentMulticastPIMSMFamilyValue == AF_INET )
    {
       FPOBJ_TRACE_CURRENT_KEY (bufp, &objagentMulticastPIMSMFamilyValue, owa.len);
       nextObjagentMulticastPIMSMFamilyValue = AF6_INET;
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjagentMulticastPIMSMFamilyValue, owa.len);

  /* return the object value: agentMulticastPIMSMFamily */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjagentMulticastPIMSMFamilyValue,
                           sizeof (objagentMulticastPIMSMFamilyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMAdminMode
*
* @purpose Get 'agentMulticastPIMSMAdminMode'
*
* @description [agentMulticastPIMSMAdminMode] enables or disables PIM Sparse Mode on the system
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMAdminMode (void *wap,
                                                                                      void *bufp)
{

  fpObjWa_t kwaagentMulticastPIMSMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMSMFamilyValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMSMAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPIMSMFamily */
  kwaagentMulticastPIMSMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMSMFamilyValue,
                   &kwaagentMulticastPIMSMFamily.len);
  if (kwaagentMulticastPIMSMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMSMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMSMFamily);
    return kwaagentMulticastPIMSMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMSMFamilyValue,
                           kwaagentMulticastPIMSMFamily.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmAdminModeGet (L7_UNIT_CURRENT, keyagentMulticastPIMSMFamilyValue,
                              &objagentMulticastPIMSMAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: agentMulticastPIMSMAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objagentMulticastPIMSMAdminModeValue,
                           sizeof (objagentMulticastPIMSMAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMAdminMode
*
* @purpose Set 'agentMulticastPIMSMAdminMode'
*
* @description [agentMulticastPIMSMAdminMode] enables or disables PIM Sparse Mode on the system
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMAdminMode (void *wap,
                                                                                      void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMSMAdminModeValue;

  fpObjWa_t kwaagentMulticastPIMSMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMSMFamilyValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: agentMulticastPIMSMAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objagentMulticastPIMSMAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objagentMulticastPIMSMAdminModeValue, owa.len);

  /* retrieve key: agentMulticastPIMSMFamily */
  kwaagentMulticastPIMSMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMSMFamilyValue,
                   &kwaagentMulticastPIMSMFamily.len);
  if (kwaagentMulticastPIMSMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMSMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMSMFamily);
    return kwaagentMulticastPIMSMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMSMFamilyValue,
                           kwaagentMulticastPIMSMFamily.len);

  /* set the value in application */
  owa.l7rc = usmDbPimsmAdminModeSet(L7_UNIT_CURRENT, keyagentMulticastPIMSMFamilyValue,
                              objagentMulticastPIMSMAdminModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ADMIN_MODE_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMDataThresholdRate
*
* @purpose Get 'agentMulticastPIMSMDataThresholdRate'
*
* @description [agentMulticastPIMSMDataThresholdRate] Specifies the minimum rate in Kbits/sec for the last hop router to initiate switching to the shortest path.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMDataThresholdRate (void
                                                                                              *wap,
                                                                                              void
                                                                                              *bufp)
{

  fpObjWa_t kwaagentMulticastPIMSMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMSMFamilyValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMSMDataThresholdRateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPIMSMFamily */
  kwaagentMulticastPIMSMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMSMFamilyValue,
                   &kwaagentMulticastPIMSMFamily.len);
  if (kwaagentMulticastPIMSMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMSMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMSMFamily);
    return kwaagentMulticastPIMSMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMSMFamilyValue,
                           kwaagentMulticastPIMSMFamily.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmDataThresholdRateGet(L7_UNIT_CURRENT, keyagentMulticastPIMSMFamilyValue,
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
* @function fpObjSet_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMDataThresholdRate
*
* @purpose Set 'agentMulticastPIMSMDataThresholdRate'
*
* @description [agentMulticastPIMSMDataThresholdRate] Specifies the minimum rate in Kbits/sec for the last hop router to initiate switching to the shortest path.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMDataThresholdRate (void
                                                                                              *wap,
                                                                                              void
                                                                                              *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMSMDataThresholdRateValue;

  fpObjWa_t kwaagentMulticastPIMSMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMSMFamilyValue;

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

  /* retrieve key: agentMulticastPIMSMFamily */
  kwaagentMulticastPIMSMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMSMFamilyValue,
                   &kwaagentMulticastPIMSMFamily.len);
  if (kwaagentMulticastPIMSMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMSMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMSMFamily);
    return kwaagentMulticastPIMSMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMSMFamilyValue,
                           kwaagentMulticastPIMSMFamily.len);

  /* set the value in application */
  owa.l7rc = usmDbPimsmDataThresholdRateSet(L7_UNIT_CURRENT, keyagentMulticastPIMSMFamilyValue,
                              objagentMulticastPIMSMDataThresholdRateValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMRegThresholdRate
*
* @purpose Get 'agentMulticastPIMSMRegThresholdRate'
*
* @description [agentMulticastPIMSMRegThresholdRate] Specifies the minimum rate in Kbits/sec for the RP to switch to the shortest path.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMRegThresholdRate (void
                                                                                             *wap,
                                                                                             void
                                                                                             *bufp)
{

  fpObjWa_t kwaagentMulticastPIMSMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMSMFamilyValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMSMRegThresholdRateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPIMSMFamily */
  kwaagentMulticastPIMSMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMSMFamilyValue,
                   &kwaagentMulticastPIMSMFamily.len);
  if (kwaagentMulticastPIMSMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMSMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMSMFamily);
    return kwaagentMulticastPIMSMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMSMFamilyValue,
                           kwaagentMulticastPIMSMFamily.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmRegisterThresholdRateGet(L7_UNIT_CURRENT, keyagentMulticastPIMSMFamilyValue,
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
* @function fpObjSet_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMRegThresholdRate
*
* @purpose Set 'agentMulticastPIMSMRegThresholdRate'
*
* @description [agentMulticastPIMSMRegThresholdRate] Specifies the minimum rate in Kbits/sec for the RP to switch to the shortest path.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMRegThresholdRate (void
                                                                                             *wap,
                                                                                             void
                                                                                             *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMSMRegThresholdRateValue;

  fpObjWa_t kwaagentMulticastPIMSMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMSMFamilyValue;

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

  /* retrieve key: agentMulticastPIMSMFamily */
  kwaagentMulticastPIMSMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMSMFamilyValue,
                   &kwaagentMulticastPIMSMFamily.len);
  if (kwaagentMulticastPIMSMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMSMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMSMFamily);
    return kwaagentMulticastPIMSMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMSMFamilyValue,
                           kwaagentMulticastPIMSMFamily.len);

  /* set the value in application */
  owa.l7rc = usmDbPimsmRegisterThresholdRateSet(L7_UNIT_CURRENT, keyagentMulticastPIMSMFamilyValue,
                              objagentMulticastPIMSMRegThresholdRateValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMSMConfigGroup_MulticastPIMSMDebugTraceTxFlag
*
* @purpose Get 'MulticastPIMSMDebugTraceTxFlag'
 *@description  [MulticastPIMSMDebugTraceTxFlag] enables or disables PIM SM
* Debug Tx Flag on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMSMConfigGroup_MulticastPIMSMDebugTraceTxFlag (void *wap,
                                                                                        void *bufp)
{
  fpObjWa_t kwaagentMulticastPIMSMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMSMFamilyValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_BOOL));
  L7_BOOL objMulticastPIMSMDebugTraceTxFlagValue;

  L7_BOOL objMulticastPIMSMDebugTraceRxFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPIMSMFamily */
  kwaagentMulticastPIMSMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMSMFamilyValue,
                   &kwaagentMulticastPIMSMFamily.len);
  if (kwaagentMulticastPIMSMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMSMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMSMFamily);
    return kwaagentMulticastPIMSMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMSMFamilyValue,
                           kwaagentMulticastPIMSMFamily.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmPacketDebugTraceFlagGet (keyagentMulticastPIMSMFamilyValue,
                                                                              &objMulticastPIMSMDebugTraceRxFlagValue, 
                                                                              &objMulticastPIMSMDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MulticastPIMSMDebugTraceTxFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastPIMSMDebugTraceTxFlagValue,
                           sizeof (objMulticastPIMSMDebugTraceTxFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentMulticastPIMSMConfigGroup_MulticastPIMSMDebugTraceTxFlag
*
* @purpose Set 'MulticastPIMSMDebugTraceTxFlag'
 *@description  [MulticastPIMSMDebugTraceTxFlag] enables or disables PIM SM
* Debug Tx Flag on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastPIMSMConfigGroup_MulticastPIMSMDebugTraceTxFlag (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_BOOL));
  L7_BOOL objMulticastPIMSMDebugTraceTxFlagValue;

  fpObjWa_t kwaagentMulticastPIMSMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMSMFamilyValue;

  L7_BOOL tempMulticastPIMSMDebugTraceTxFlagValue;
  L7_BOOL objMulticastPIMSMDebugTraceRxFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastPIMSMDebugTraceTxFlag */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMulticastPIMSMDebugTraceTxFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastPIMSMDebugTraceTxFlagValue, owa.len);

  /* retrieve key: agentMulticastPIMSMFamily */
  kwaagentMulticastPIMSMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMSMFamilyValue,
                   &kwaagentMulticastPIMSMFamily.len);
  if (kwaagentMulticastPIMSMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMSMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMSMFamily);
    return kwaagentMulticastPIMSMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMSMFamilyValue,
                           kwaagentMulticastPIMSMFamily.len);

  /* set the value in application */
  owa.l7rc = usmDbPimsmPacketDebugTraceFlagGet (keyagentMulticastPIMSMFamilyValue,
                                                                              &objMulticastPIMSMDebugTraceRxFlagValue,
                                                                              &tempMulticastPIMSMDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* set the value in application */
  owa.l7rc = usmDbPimsmPacketDebugTraceFlagSet (keyagentMulticastPIMSMFamilyValue,
                                                                             objMulticastPIMSMDebugTraceRxFlagValue,
                                                                             objMulticastPIMSMDebugTraceTxFlagValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMSMConfigGroup_MulticastPIMSMDebugTraceRxFlag
*
* @purpose Get 'MulticastPIMSMDebugTraceRxFlag'
 *@description  [MulticastPIMSMDebugTraceRxFlag] enables or disables PIM SM
* Debug Rx Flag on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMSMConfigGroup_MulticastPIMSMDebugTraceRxFlag (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t kwaagentMulticastPIMSMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMSMFamilyValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_BOOL));
  L7_BOOL objMulticastPIMSMDebugTraceRxFlagValue;

  L7_BOOL objMulticastPIMSMDebugTraceTxFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPIMSMFamily */
  kwaagentMulticastPIMSMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMSMFamilyValue,
                   &kwaagentMulticastPIMSMFamily.len);
  if (kwaagentMulticastPIMSMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMSMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMSMFamily);
    return kwaagentMulticastPIMSMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMSMFamilyValue,
                           kwaagentMulticastPIMSMFamily.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmPacketDebugTraceFlagGet (keyagentMulticastPIMSMFamilyValue,
                                                                              &objMulticastPIMSMDebugTraceRxFlagValue,
                                                                              &objMulticastPIMSMDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MulticastPIMSMDebugTraceRxFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastPIMSMDebugTraceRxFlagValue,
                           sizeof (objMulticastPIMSMDebugTraceRxFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentMulticastPIMSMConfigGroup_MulticastPIMSMDebugTraceRxFlag
*
* @purpose Set 'MulticastPIMSMDebugTraceRxFlag'
 *@description  [MulticastPIMSMDebugTraceRxFlag] enables or disables PIM SM
* Debug Rx Flag on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastPIMSMConfigGroup_MulticastPIMSMDebugTraceRxFlag (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_BOOL));
  L7_BOOL objMulticastPIMSMDebugTraceRxFlagValue;

  fpObjWa_t kwaagentMulticastPIMSMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMSMFamilyValue;

  L7_BOOL objMulticastPIMSMDebugTraceTxFlagValue;
  L7_BOOL tempMulticastPIMSMDebugTraceRxFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastPIMSMDebugTraceRxFlag */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMulticastPIMSMDebugTraceRxFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastPIMSMDebugTraceRxFlagValue, owa.len);

  /* retrieve key: agentMulticastPIMSMFamily */
  kwaagentMulticastPIMSMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMSMConfigGroup_agentMulticastPIMSMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMSMFamilyValue,
                   &kwaagentMulticastPIMSMFamily.len);
  if (kwaagentMulticastPIMSMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMSMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMSMFamily);
    return kwaagentMulticastPIMSMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMSMFamilyValue,
                           kwaagentMulticastPIMSMFamily.len);

  /* set the value in application */
  owa.l7rc = usmDbPimdmPacketDebugTraceFlagGet (keyagentMulticastPIMSMFamilyValue,
                                                                              &tempMulticastPIMSMDebugTraceRxFlagValue,
                                                                              &objMulticastPIMSMDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* set the value in application */
  owa.l7rc = usmDbPimdmPacketDebugTraceFlagSet (keyagentMulticastPIMSMFamilyValue,
                                                                             objMulticastPIMSMDebugTraceRxFlagValue,
                                                                             objMulticastPIMSMDebugTraceTxFlagValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

