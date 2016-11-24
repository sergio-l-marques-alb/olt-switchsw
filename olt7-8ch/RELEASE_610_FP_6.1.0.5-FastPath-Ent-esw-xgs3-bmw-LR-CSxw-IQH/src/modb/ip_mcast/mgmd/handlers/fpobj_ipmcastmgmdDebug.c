
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastmgmdDebug.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  03 June 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastmgmdDebug_obj.h"
#include "l3_mcast_commdefs.h"
#include "usmdb_igmp_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdDebug_mgmdRouterInterfaceType
*
* @purpose Get 'mgmdRouterInterfaceType'
 *@description  [mgmdRouterInterfaceType] Router Interface Type - V4 or V6   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdDebug_mgmdRouterInterfaceType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceTypeValue;
  xLibU32_t nextObjmgmdRouterInterfaceTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdDebug_mgmdRouterInterfaceType,
                          (xLibU8_t *) & objmgmdRouterInterfaceTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdRouterInterfaceTypeValue = L7_INET_ADDR_TYPE_IPV4;
  }
  else if (objmgmdRouterInterfaceTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterInterfaceTypeValue, owa.len);
    nextObjmgmdRouterInterfaceTypeValue = L7_INET_ADDR_TYPE_IPV6;
  }
  else
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdRouterInterfaceTypeValue, owa.len);

  /* return the object value: mgmdRouterInterfaceType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdRouterInterfaceTypeValue,
                           sizeof (objmgmdRouterInterfaceTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdDebug_mgmdDebugTraceTxFlag
*
* @purpose Get 'mgmdDebugTraceTxFlag'
 *@description  [mgmdDebugTraceTxFlag] enables or disables MGMD Debug Tx Flag on
* the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdDebug_mgmdDebugTraceTxFlag (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdDebugTraceTxFlagValue;

  xLibU32_t objmgmdDebugTraceRxFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdDebug_mgmdRouterInterfaceType,
                                                 (xLibU8_t *) & keymgmdRouterInterfaceTypeValue,
                                                 &kwamgmdRouterInterfaceType.len);
  if (kwamgmdRouterInterfaceType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceType);
    return kwamgmdRouterInterfaceType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceTypeValue, kwamgmdRouterInterfaceType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdPacketDebugTraceFlagGet (keymgmdRouterInterfaceTypeValue,
                                                                             &objmgmdDebugTraceRxFlagValue, 
                                                                             &objmgmdDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdDebugTraceTxFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdDebugTraceTxFlagValue,
                           sizeof (objmgmdDebugTraceTxFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdDebug_mgmdDebugTraceTxFlag
*
* @purpose Set 'mgmdDebugTraceTxFlag'
 *@description  [mgmdDebugTraceTxFlag] enables or disables MGMD Debug Tx Flag on
* the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdDebug_mgmdDebugTraceTxFlag (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdDebugTraceTxFlagValue;

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  xLibU32_t tempmgmdDebugTraceTxFlagValue;
  xLibU32_t objmgmdDebugTraceRxFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdDebugTraceTxFlag */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmgmdDebugTraceTxFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdDebugTraceTxFlagValue, owa.len);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdDebug_mgmdRouterInterfaceType,
                                                 (xLibU8_t *) & keymgmdRouterInterfaceTypeValue,
                                                 &kwamgmdRouterInterfaceType.len);
  if (kwamgmdRouterInterfaceType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceType);
    return kwamgmdRouterInterfaceType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceTypeValue, kwamgmdRouterInterfaceType.len);

  /* set the value in application */
  owa.l7rc = usmDbMgmdPacketDebugTraceFlagGet (keymgmdRouterInterfaceTypeValue,
                                                                              &objmgmdDebugTraceRxFlagValue,
                                                                              &tempmgmdDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* set the value in application */
  owa.l7rc = usmDbMgmdPacketDebugTraceFlagSet (keymgmdRouterInterfaceTypeValue,
                                                                             objmgmdDebugTraceRxFlagValue,
                                                                             objmgmdDebugTraceTxFlagValue);


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdDebug_mgmdDebugTraceRxFlag
*
* @purpose Get 'mgmdDebugTraceRxFlag'
 *@description  [mgmdDebugTraceRxFlag] enables or disables MGMD Debug Rx Flag on
* the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdDebug_mgmdDebugTraceRxFlag (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdDebugTraceRxFlagValue;

  xLibU32_t objmgmdDebugTraceTxFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdDebug_mgmdRouterInterfaceType,
                                                 (xLibU8_t *) & keymgmdRouterInterfaceTypeValue,
                                                 &kwamgmdRouterInterfaceType.len);
  if (kwamgmdRouterInterfaceType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceType);
    return kwamgmdRouterInterfaceType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceTypeValue, kwamgmdRouterInterfaceType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdPacketDebugTraceFlagGet (keymgmdRouterInterfaceTypeValue,
                                                                             &objmgmdDebugTraceRxFlagValue, 
                                                                             &objmgmdDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdDebugTraceRxFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdDebugTraceRxFlagValue,
                           sizeof (objmgmdDebugTraceRxFlagValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdDebug_mgmdDebugTraceRxFlag
*
* @purpose Set 'mgmdDebugTraceRxFlag'
 *@description  [mgmdDebugTraceRxFlag] enables or disables MGMD Debug Rx Flag on
* the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdDebug_mgmdDebugTraceRxFlag (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdDebugTraceRxFlagValue;

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  xLibU32_t tempmgmdDebugTraceRxFlagValue;
  xLibU32_t objmgmdDebugTraceTxFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdDebugTraceRxFlag */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmgmdDebugTraceRxFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdDebugTraceRxFlagValue, owa.len);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdDebug_mgmdRouterInterfaceType,
                                                 (xLibU8_t *) & keymgmdRouterInterfaceTypeValue,
                                                 &kwamgmdRouterInterfaceType.len);
  if (kwamgmdRouterInterfaceType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceType);
    return kwamgmdRouterInterfaceType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceTypeValue, kwamgmdRouterInterfaceType.len);

  /* set the value in application */
  owa.l7rc = usmDbMgmdPacketDebugTraceFlagGet (keymgmdRouterInterfaceTypeValue,
                                                                              &tempmgmdDebugTraceRxFlagValue,
                                                                              &objmgmdDebugTraceTxFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* set the value in application */
  owa.l7rc = usmDbMgmdPacketDebugTraceFlagSet (keymgmdRouterInterfaceTypeValue,
                                                                             objmgmdDebugTraceRxFlagValue,
                                                                             objmgmdDebugTraceTxFlagValue);


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
