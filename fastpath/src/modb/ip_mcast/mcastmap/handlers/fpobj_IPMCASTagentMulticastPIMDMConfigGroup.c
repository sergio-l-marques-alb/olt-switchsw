
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_IPMCASTagentMulticastPIMDMConfigGroup.c
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
#include "_xe_IPMCASTagentMulticastPIMDMConfigGroup_obj.h"
#include "usmdb_mib_pim_api.h"
#include "usmdb_mib_mcast_api.h"

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMDMConfigGroup_agentMulticastPIMDMFamily
*
* @purpose Get 'agentMulticastPIMDMFamily'
*
* @description [agentMulticastPIMDMFamily] Address Family
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMDMConfigGroup_agentMulticastPIMDMFamily (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMDMFamilyValue;
  xLibU32_t nextObjagentMulticastPIMDMFamilyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPIMDMFamily */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMDMConfigGroup_agentMulticastPIMDMFamily,
                          (xLibU8_t *) & objagentMulticastPIMDMFamilyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjagentMulticastPIMDMFamilyValue = AF_INET;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    if(objagentMulticastPIMDMFamilyValue == AF_INET)
    {
       FPOBJ_TRACE_CURRENT_KEY (bufp, &objagentMulticastPIMDMFamilyValue, owa.len);
       nextObjagentMulticastPIMDMFamilyValue = AF6_INET;
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjagentMulticastPIMDMFamilyValue, owa.len);

  /* return the object value: agentMulticastPIMDMFamily */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjagentMulticastPIMDMFamilyValue,
                           sizeof (objagentMulticastPIMDMFamilyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMDMConfigGroup_agentMulticastPIMDMAdminMode
*
* @purpose Get 'agentMulticastPIMDMAdminMode'
*
* @description [agentMulticastPIMDMAdminMode] enables or disables PIM Dense Mode on the system
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMDMConfigGroup_agentMulticastPIMDMAdminMode (void *wap,
                                                                                      void *bufp)
{

  fpObjWa_t kwaagentMulticastPIMDMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMDMFamilyValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMDMAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPIMDMFamily */
  kwaagentMulticastPIMDMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMDMConfigGroup_agentMulticastPIMDMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMDMFamilyValue,
                   &kwaagentMulticastPIMDMFamily.len);
  if (kwaagentMulticastPIMDMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMDMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMDMFamily);
    return kwaagentMulticastPIMDMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMDMFamilyValue,
                           kwaagentMulticastPIMDMFamily.len);

  /* get the value from application */
  owa.l7rc = usmDbPimRouterAdminModeGet(L7_UNIT_CURRENT, keyagentMulticastPIMDMFamilyValue,
                              &objagentMulticastPIMDMAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: agentMulticastPIMDMAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objagentMulticastPIMDMAdminModeValue,
                           sizeof (objagentMulticastPIMDMAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentMulticastPIMDMConfigGroup_agentMulticastPIMDMAdminMode
*
* @purpose Set 'agentMulticastPIMDMAdminMode'
*
* @description [agentMulticastPIMDMAdminMode] enables or disables PIM Dense Mode on the system
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastPIMDMConfigGroup_agentMulticastPIMDMAdminMode (void *wap,
                                                                                      void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastPIMDMAdminModeValue;

  fpObjWa_t kwaagentMulticastPIMDMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMDMFamilyValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: agentMulticastPIMDMAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objagentMulticastPIMDMAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objagentMulticastPIMDMAdminModeValue, owa.len);

  /* retrieve key: agentMulticastPIMDMFamily */
  kwaagentMulticastPIMDMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMDMConfigGroup_agentMulticastPIMDMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMDMFamilyValue,
                   &kwaagentMulticastPIMDMFamily.len);
  if (kwaagentMulticastPIMDMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMDMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMDMFamily);
    return kwaagentMulticastPIMDMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMDMFamilyValue,
                           kwaagentMulticastPIMDMFamily.len);

  /* set the value in application */
  owa.l7rc = usmDbPimRouterAdminModeSet (L7_UNIT_CURRENT, keyagentMulticastPIMDMFamilyValue,
                              objagentMulticastPIMDMAdminModeValue);

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
* @function fpObjGet_IPMCASTagentMulticastPIMDMConfigGroup_MulticastPIMDMDebugTraceTxFlag
*
* @purpose Get 'MulticastPIMDMDebugTraceTxFlag'
 *@description  [MulticastPIMDMDebugTraceTxFlag] enables or disables PIM DM
* Debug Tx Flag on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMDMConfigGroup_MulticastPIMDMDebugTraceTxFlag (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t kwaagentMulticastPIMDMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMDMFamilyValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_BOOL));
  L7_BOOL objMulticastPIMDMDebugTraceTxFlagValue;

  L7_BOOL objMulticastPIMDMDebugTraceRxFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPIMDMFamily */
  kwaagentMulticastPIMDMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMDMConfigGroup_agentMulticastPIMDMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMDMFamilyValue,
                   &kwaagentMulticastPIMDMFamily.len);
  if (kwaagentMulticastPIMDMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMDMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMDMFamily);
    return kwaagentMulticastPIMDMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMDMFamilyValue,
                           kwaagentMulticastPIMDMFamily.len);

  /* get the value from application */
  owa.l7rc = usmDbPimdmPacketDebugTraceFlagGet (keyagentMulticastPIMDMFamilyValue,
                                                                              &objMulticastPIMDMDebugTraceRxFlagValue,
                                                                              &objMulticastPIMDMDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MulticastPIMDMDebugTraceTxFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastPIMDMDebugTraceTxFlagValue,
                           sizeof (objMulticastPIMDMDebugTraceTxFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentMulticastPIMDMConfigGroup_MulticastPIMDMDebugTraceTxFlag
*
* @purpose Set 'MulticastPIMDMDebugTraceTxFlag'
 *@description  [MulticastPIMDMDebugTraceTxFlag] enables or disables PIM DM
* Debug Tx Flag on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastPIMDMConfigGroup_MulticastPIMDMDebugTraceTxFlag (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_BOOL));
  L7_BOOL objMulticastPIMDMDebugTraceTxFlagValue;

  fpObjWa_t kwaagentMulticastPIMDMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMDMFamilyValue;

  L7_BOOL objMulticastPIMDMDebugTraceRxFlagValue;
  L7_BOOL tempMulticastPIMDMDebugTraceTxFlagValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastPIMDMDebugTraceTxFlag */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMulticastPIMDMDebugTraceTxFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastPIMDMDebugTraceTxFlagValue, owa.len);

  /* retrieve key: agentMulticastPIMDMFamily */
  kwaagentMulticastPIMDMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMDMConfigGroup_agentMulticastPIMDMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMDMFamilyValue,
                   &kwaagentMulticastPIMDMFamily.len);
  if (kwaagentMulticastPIMDMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMDMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMDMFamily);
    return kwaagentMulticastPIMDMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMDMFamilyValue,
                           kwaagentMulticastPIMDMFamily.len);

  owa.l7rc = usmDbPimdmPacketDebugTraceFlagGet (keyagentMulticastPIMDMFamilyValue,
                                                                              &objMulticastPIMDMDebugTraceRxFlagValue,
                                                                              &tempMulticastPIMDMDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* set the value in application */
  owa.l7rc = usmDbPimdmPacketDebugTraceFlagSet (keyagentMulticastPIMDMFamilyValue,
                                                                             objMulticastPIMDMDebugTraceRxFlagValue,
                                                                             objMulticastPIMDMDebugTraceTxFlagValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMDMConfigGroup_MulticastPIMDMDebugTraceRxFlag
*
* @purpose Get 'MulticastPIMDMDebugTraceRxFlag'
 *@description  [MulticastPIMDMDebugTraceRxFlag] enables or disables PIM DM
* Debug Rx Flag on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMDMConfigGroup_MulticastPIMDMDebugTraceRxFlag (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t kwaagentMulticastPIMDMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMDMFamilyValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_BOOL));
  L7_BOOL objMulticastPIMDMDebugTraceRxFlagValue;

  L7_BOOL objMulticastPIMDMDebugTraceTxFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastPIMDMFamily */
  kwaagentMulticastPIMDMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMDMConfigGroup_agentMulticastPIMDMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMDMFamilyValue,
                   &kwaagentMulticastPIMDMFamily.len);
  if (kwaagentMulticastPIMDMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMDMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMDMFamily);
    return kwaagentMulticastPIMDMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMDMFamilyValue,
                           kwaagentMulticastPIMDMFamily.len);

  /* get the value from application */
  owa.l7rc = usmDbPimdmPacketDebugTraceFlagGet (keyagentMulticastPIMDMFamilyValue,
                                                                              &objMulticastPIMDMDebugTraceRxFlagValue,
                                                                              &objMulticastPIMDMDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MulticastPIMDMDebugTraceRxFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastPIMDMDebugTraceRxFlagValue,
                           sizeof (objMulticastPIMDMDebugTraceRxFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentMulticastPIMDMConfigGroup_MulticastPIMDMDebugTraceRxFlag
*
* @purpose Set 'MulticastPIMDMDebugTraceRxFlag'
 *@description  [MulticastPIMDMDebugTraceRxFlag] enables or disables PIM DM
* Debug Rx Flag on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastPIMDMConfigGroup_MulticastPIMDMDebugTraceRxFlag (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_BOOL));
  L7_BOOL objMulticastPIMDMDebugTraceRxFlagValue;

  fpObjWa_t kwaagentMulticastPIMDMFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastPIMDMFamilyValue;

  L7_BOOL objMulticastPIMDMDebugTraceTxFlagValue;
  L7_BOOL tempMulticastPIMDMDebugTraceRxFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastPIMDMDebugTraceRxFlag */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMulticastPIMDMDebugTraceRxFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastPIMDMDebugTraceRxFlagValue, owa.len);

  /* retrieve key: agentMulticastPIMDMFamily */
  kwaagentMulticastPIMDMFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastPIMDMConfigGroup_agentMulticastPIMDMFamily,
                   (xLibU8_t *) & keyagentMulticastPIMDMFamilyValue,
                   &kwaagentMulticastPIMDMFamily.len);
  if (kwaagentMulticastPIMDMFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastPIMDMFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastPIMDMFamily);
    return kwaagentMulticastPIMDMFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastPIMDMFamilyValue,
                           kwaagentMulticastPIMDMFamily.len);

  /* set the value in application */
  owa.l7rc = usmDbPimdmPacketDebugTraceFlagGet (keyagentMulticastPIMDMFamilyValue,
                                                                              &tempMulticastPIMDMDebugTraceRxFlagValue,
                                                                              &objMulticastPIMDMDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* set the value in application */
  owa.l7rc = usmDbPimdmPacketDebugTraceFlagSet (keyagentMulticastPIMDMFamilyValue,
                                                                             objMulticastPIMDMDebugTraceRxFlagValue,
                                                                             objMulticastPIMDMDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastPIMDMConfigGroup_globalMcastMode
*
* @purpose Get 'globalMcastMode'
*
* @description [globalMcastMode] enables or disables multicast on router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastPIMDMConfigGroup_globalMcastMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objglobalMcastModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbMcastAdminModeGet(L7_UNIT_CURRENT, &objglobalMcastModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objglobalMcastModeValue, sizeof (objglobalMcastModeValue));

  /* return the object value: globalMcastMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objglobalMcastModeValue,
                           sizeof (objglobalMcastModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentMulticastPIMDMConfigGroup_globalMcastMode
*
* @purpose Set 'globalMcastMode'
*
* @description [globalMcastMode] enables or disables multicast on router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastPIMDMConfigGroup_globalMcastMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objglobalMcastModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: globalMcastMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objglobalMcastModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objglobalMcastModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbMcastAdminModeSet(L7_UNIT_CURRENT, objglobalMcastModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
