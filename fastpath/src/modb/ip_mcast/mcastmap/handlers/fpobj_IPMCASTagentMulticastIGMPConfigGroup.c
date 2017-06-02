
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_IPMCASTagentMulticastIGMPConfigGroup.c
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
#include "_xe_IPMCASTagentMulticastIGMPConfigGroup_obj.h"
#include "l3_mcast_commdefs.h"
#include "usmdb_igmp_api.h"
#include "usmdb_mib_mcast_api.h"

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastIGMPConfigGroup_agentMulticastIGMPFamily
*
* @purpose Get 'agentMulticastIGMPFamily'
*
* @description [agentMulticastIGMPFamily] Address Family
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastIGMPConfigGroup_agentMulticastIGMPFamily (void *wap,
                                                                                 void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastIGMPFamilyValue;
  xLibU32_t nextObjagentMulticastIGMPFamilyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastIGMPFamily */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastIGMPConfigGroup_agentMulticastIGMPFamily,
                          (xLibU8_t *) & objagentMulticastIGMPFamilyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjagentMulticastIGMPFamilyValue = L7_IPv4;  
    owa.l7rc = L7_SUCCESS;  
/*    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, &nextObjagentMulticastIGMPFamilyValue);*/
  }
  else
  {
    if (objagentMulticastIGMPFamilyValue == L7_IPv4) 
    {
       nextObjagentMulticastIGMPFamilyValue = L7_IPv6;
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjagentMulticastIGMPFamilyValue, owa.len);

  /* return the object value: agentMulticastIGMPFamily */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjagentMulticastIGMPFamilyValue,
                           sizeof (objagentMulticastIGMPFamilyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastIGMPConfigGroup_agentMulticastIGMPAdminMode
*
* @purpose Get 'agentMulticastIGMPAdminMode'
*
* @description [agentMulticastIGMPAdminMode] enables or disables IGMP on the system
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastIGMPConfigGroup_agentMulticastIGMPAdminMode (void *wap,
                                                                                    void *bufp)
{

  fpObjWa_t kwaagentMulticastIGMPFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastIGMPFamilyValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastIGMPAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastIGMPFamily */
  kwaagentMulticastIGMPFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastIGMPConfigGroup_agentMulticastIGMPFamily,
                   (xLibU8_t *) & keyagentMulticastIGMPFamilyValue,
                   &kwaagentMulticastIGMPFamily.len);
  if (kwaagentMulticastIGMPFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastIGMPFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastIGMPFamily);
    return kwaagentMulticastIGMPFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastIGMPFamilyValue,
                           kwaagentMulticastIGMPFamily.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdAdminModeGet(L7_UNIT_CURRENT, keyagentMulticastIGMPFamilyValue,
                              &objagentMulticastIGMPAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: agentMulticastIGMPAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objagentMulticastIGMPAdminModeValue,
                           sizeof (objagentMulticastIGMPAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentMulticastIGMPConfigGroup_agentMulticastIGMPAdminMode
*
* @purpose Set 'agentMulticastIGMPAdminMode'
*
* @description [agentMulticastIGMPAdminMode] enables or disables IGMP on the system
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastIGMPConfigGroup_agentMulticastIGMPAdminMode (void *wap,
                                                                                    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentMulticastIGMPAdminModeValue;

  fpObjWa_t kwaagentMulticastIGMPFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastIGMPFamilyValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: agentMulticastIGMPAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objagentMulticastIGMPAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objagentMulticastIGMPAdminModeValue, owa.len);

  /* retrieve key: agentMulticastIGMPFamily */
  kwaagentMulticastIGMPFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastIGMPConfigGroup_agentMulticastIGMPFamily,
                   (xLibU8_t *) & keyagentMulticastIGMPFamilyValue,
                   &kwaagentMulticastIGMPFamily.len);
  if (kwaagentMulticastIGMPFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastIGMPFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastIGMPFamily);
    return kwaagentMulticastIGMPFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastIGMPFamilyValue,
                           kwaagentMulticastIGMPFamily.len);

  /* set the value in application */
  owa.l7rc = usmDbMgmdAdminModeSet (L7_UNIT_CURRENT, keyagentMulticastIGMPFamilyValue,
                              objagentMulticastIGMPAdminModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastIGMPConfigGroup_MulticastMCacheDebugTraceTxFlag
*
* @purpose Get 'MulticastMCacheDebugTraceTxFlag'
 *@description  [MulticastMCacheDebugTraceTxFlag] enables or disables MCache
* data Debug Tx Flag on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastIGMPConfigGroup_MulticastMCacheDebugTraceTxFlag (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t kwaagentMulticastIGMPFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastIGMPFamilyValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_BOOL));
  L7_BOOL objMulticastMCacheDebugTraceTxFlagValue;

  L7_BOOL objMulticastMCacheDebugTraceRxFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastIGMPFamily */
  kwaagentMulticastIGMPFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastIGMPConfigGroup_agentMulticastIGMPFamily,
                   (xLibU8_t *) & keyagentMulticastIGMPFamilyValue,
                   &kwaagentMulticastIGMPFamily.len);
  if (kwaagentMulticastIGMPFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastIGMPFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastIGMPFamily);
    return kwaagentMulticastIGMPFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastIGMPFamilyValue,
                           kwaagentMulticastIGMPFamily.len);

  /* get the value from application */
  owa.l7rc = usmDbMcachePacketDebugTraceFlagGet (keyagentMulticastIGMPFamilyValue,
                                                                              &objMulticastMCacheDebugTraceRxFlagValue, 
                                                                              &objMulticastMCacheDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MulticastMCacheDebugTraceTxFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastMCacheDebugTraceTxFlagValue,
                           sizeof (objMulticastMCacheDebugTraceTxFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentMulticastIGMPConfigGroup_MulticastMCacheDebugTraceTxFlag
*
* @purpose Set 'MulticastMCacheDebugTraceTxFlag'
 *@description  [MulticastMCacheDebugTraceTxFlag] enables or disables MCache
* data Debug Tx Flag on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastIGMPConfigGroup_MulticastMCacheDebugTraceTxFlag (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_BOOL));
  L7_BOOL objMulticastMCacheDebugTraceTxFlagValue;

  fpObjWa_t kwaagentMulticastIGMPFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastIGMPFamilyValue;

  L7_BOOL tempMulticastMCacheDebugTraceTxFlagValue;
  L7_BOOL objMulticastMCacheDebugTraceRxFlagValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastMCacheDebugTraceTxFlag */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMulticastMCacheDebugTraceTxFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastMCacheDebugTraceTxFlagValue, owa.len);

  /* retrieve key: agentMulticastIGMPFamily */
  kwaagentMulticastIGMPFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastIGMPConfigGroup_agentMulticastIGMPFamily,
                   (xLibU8_t *) & keyagentMulticastIGMPFamilyValue,
                   &kwaagentMulticastIGMPFamily.len);
  if (kwaagentMulticastIGMPFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastIGMPFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastIGMPFamily);
    return kwaagentMulticastIGMPFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastIGMPFamilyValue,
                           kwaagentMulticastIGMPFamily.len);

  /* set the value in application */
  owa.l7rc = usmDbMcachePacketDebugTraceFlagGet (keyagentMulticastIGMPFamilyValue,
                                                                              &objMulticastMCacheDebugTraceRxFlagValue,
                                                                              &tempMulticastMCacheDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* set the value in application */
  owa.l7rc = usmDbMcachePacketDebugTraceFlagSet (keyagentMulticastIGMPFamilyValue,
                                                                             objMulticastMCacheDebugTraceRxFlagValue,
                                                                             objMulticastMCacheDebugTraceTxFlagValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTagentMulticastIGMPConfigGroup_MulticastMCacheDebugTraceRxFlag
*
* @purpose Get 'MulticastMCacheDebugTraceRxFlag'
 *@description  [MulticastMCacheDebugTraceRxFlag] enables or disables MCache
* data Debug Rx Flag on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentMulticastIGMPConfigGroup_MulticastMCacheDebugTraceRxFlag (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t kwaagentMulticastIGMPFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastIGMPFamilyValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_BOOL));
  L7_BOOL objMulticastMCacheDebugTraceRxFlagValue;

  L7_BOOL objMulticastMCacheDebugTraceTxFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: agentMulticastIGMPFamily */
  kwaagentMulticastIGMPFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastIGMPConfigGroup_agentMulticastIGMPFamily,
                   (xLibU8_t *) & keyagentMulticastIGMPFamilyValue,
                   &kwaagentMulticastIGMPFamily.len);
  if (kwaagentMulticastIGMPFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastIGMPFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastIGMPFamily);
    return kwaagentMulticastIGMPFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastIGMPFamilyValue,
                           kwaagentMulticastIGMPFamily.len);

  /* get the value from application */
  owa.l7rc = usmDbMcachePacketDebugTraceFlagGet (keyagentMulticastIGMPFamilyValue,
                                                                              &objMulticastMCacheDebugTraceRxFlagValue, 
                                                                              &objMulticastMCacheDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MulticastMCacheDebugTraceRxFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastMCacheDebugTraceRxFlagValue,
                           sizeof (objMulticastMCacheDebugTraceRxFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentMulticastIGMPConfigGroup_MulticastMCacheDebugTraceRxFlag
*
* @purpose Set 'MulticastMCacheDebugTraceRxFlag'
 *@description  [MulticastMCacheDebugTraceRxFlag] enables or disables MCache
* data Debug Rx Flag on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentMulticastIGMPConfigGroup_MulticastMCacheDebugTraceRxFlag (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_BOOL));
  L7_BOOL objMulticastMCacheDebugTraceRxFlagValue;

  fpObjWa_t kwaagentMulticastIGMPFamily = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyagentMulticastIGMPFamilyValue;

  L7_BOOL tempMulticastMCacheDebugTraceRxFlagValue;
  L7_BOOL objMulticastMCacheDebugTraceTxFlagValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastMCacheDebugTraceRxFlag */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMulticastMCacheDebugTraceRxFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastMCacheDebugTraceRxFlagValue, owa.len);

  /* retrieve key: agentMulticastIGMPFamily */
  kwaagentMulticastIGMPFamily.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTagentMulticastIGMPConfigGroup_agentMulticastIGMPFamily,
                   (xLibU8_t *) & keyagentMulticastIGMPFamilyValue,
                   &kwaagentMulticastIGMPFamily.len);
  if (kwaagentMulticastIGMPFamily.rc != XLIBRC_SUCCESS)
  {
    kwaagentMulticastIGMPFamily.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaagentMulticastIGMPFamily);
    return kwaagentMulticastIGMPFamily.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyagentMulticastIGMPFamilyValue,
                           kwaagentMulticastIGMPFamily.len);

  /* set the value in application */
  owa.l7rc = usmDbMcachePacketDebugTraceFlagGet (keyagentMulticastIGMPFamilyValue,
                                                                              &tempMulticastMCacheDebugTraceRxFlagValue,
                                                                              &objMulticastMCacheDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* set the value in application */
  owa.l7rc = usmDbMcachePacketDebugTraceFlagSet (keyagentMulticastIGMPFamilyValue,
                                                                             objMulticastMCacheDebugTraceRxFlagValue,
                                                                             objMulticastMCacheDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

