
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastmgmdStats.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  05 May 2008, Monday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastmgmdStats_obj.h"
#include "l3_mcast_commdefs.h"
#include "usmdb_igmp_api.h"
#include "usmdb_mib_igmp_api.h"


/*******************************************************************************
* @function fpObjGet_ipmcastmgmdStats_mgmdRouterInterfaceType
*
* @purpose Get 'mgmdRouterInterfaceType'
*
* @description [mgmdRouterInterfaceType] Router Interface Type - V4 or V6
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdStats_mgmdRouterInterfaceType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceTypeValue;
  xLibU32_t nextObjmgmdRouterInterfaceTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdStats_mgmdRouterInterfaceType,
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
* @function fpObjGet_ipmcastmgmdStats_mgmdRouterAlertCheck
*
* @purpose Get 'mgmdRouterAlertCheck'
*
* @description [mgmdRouterAlertCheck] Indicates whether the optional Router Alert field is required / present
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdStats_mgmdRouterAlertCheck (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterAlertCheckValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdStats_mgmdRouterInterfaceType,
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
  owa.l7rc = usmDbMgmdRouterAlertMandatoryGet (L7_UNIT_CURRENT, keymgmdRouterInterfaceTypeValue,
                              &objmgmdRouterAlertCheckValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterAlertCheck */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterAlertCheckValue,
                           sizeof (objmgmdRouterAlertCheckValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdStats_mgmdRouterAlertCheck
*
* @purpose Set 'mgmdRouterAlertCheck'
*
* @description [mgmdRouterAlertCheck] Indicates whether the optional Router Alert field is required / present
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdStats_mgmdRouterAlertCheck (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterAlertCheckValue;

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdRouterAlertCheck */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmgmdRouterAlertCheckValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdRouterAlertCheckValue, owa.len);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdStats_mgmdRouterInterfaceType,
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
  owa.l7rc = usmDbMgmdRouterAlertMandatorySet (L7_UNIT_CURRENT, keymgmdRouterInterfaceTypeValue,
                              objmgmdRouterAlertCheckValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdStats_mgmdQueriesRecvd
*
* @purpose Get 'mgmdQueriesRecvd'
*
* @description [mgmdQueriesRecvd] Queries Received for the router
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdStats_mgmdQueriesRecvd (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdQueriesRecvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdStats_mgmdRouterInterfaceType,
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
  owa.l7rc = usmDbQueriesReceivedGet (L7_UNIT_CURRENT, keymgmdRouterInterfaceTypeValue,
                              &objmgmdQueriesRecvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdQueriesRecvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdQueriesRecvdValue,
                           sizeof (objmgmdQueriesRecvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdStats_mgmdQueriesSent
*
* @purpose Get 'mgmdQueriesSent'
*
* @description [mgmdQueriesSent] Queries Sent for the router
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdStats_mgmdQueriesSent (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdQueriesSentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdStats_mgmdRouterInterfaceType,
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
  owa.l7rc = usmDbQueriesSentGet (L7_UNIT_CURRENT, keymgmdRouterInterfaceTypeValue,
                              &objmgmdQueriesSentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdQueriesSent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdQueriesSentValue,
                           sizeof (objmgmdQueriesSentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdStats_mgmdReportsRecvd
*
* @purpose Get 'mgmdReportsRecvd'
*
* @description [mgmdReportsRecvd] Reports Received for the router
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdStats_mgmdReportsRecvd (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdReportsRecvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdStats_mgmdRouterInterfaceType,
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
  owa.l7rc = usmDbReportsReceivedGet (L7_UNIT_CURRENT, keymgmdRouterInterfaceTypeValue,
                              &objmgmdReportsRecvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdReportsRecvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdReportsRecvdValue,
                           sizeof (objmgmdReportsRecvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdStats_mgmdReportsSent
*
* @purpose Get 'mgmdReportsSent'
*
* @description [mgmdReportsSent] Reports Sent for the router
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdStats_mgmdReportsSent (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdReportsSentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdStats_mgmdRouterInterfaceType,
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
  owa.l7rc = usmDbReportsSentGet (L7_UNIT_CURRENT, keymgmdRouterInterfaceTypeValue,
                              &objmgmdReportsSentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdReportsSent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdReportsSentValue,
                           sizeof (objmgmdReportsSentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdStats_mgmdLeavesRecvd
*
* @purpose Get 'mgmdLeavesRecvd'
*
* @description [mgmdLeavesRecvd] Leaves Received for the router
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdStats_mgmdLeavesRecvd (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdLeavesRecvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdStats_mgmdRouterInterfaceType,
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
  owa.l7rc = usmDbLeavesReceivedGet (L7_UNIT_CURRENT, keymgmdRouterInterfaceTypeValue,
                              &objmgmdLeavesRecvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdLeavesRecvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdLeavesRecvdValue,
                           sizeof (objmgmdLeavesRecvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdStats_mgmdLeavesSent
*
* @purpose Get 'mgmdLeavesSent'
*
* @description [mgmdLeavesSent] Leaves Sent for the router
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdStats_mgmdLeavesSent (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdLeavesSentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdStats_mgmdRouterInterfaceType,
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
  owa.l7rc = usmDbLeavesSentGet (L7_UNIT_CURRENT, keymgmdRouterInterfaceTypeValue,
                              &objmgmdLeavesSentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdLeavesSent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdLeavesSentValue,
                           sizeof (objmgmdLeavesSentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdStats_mgmdValidPktsRecvd
*
* @purpose Get 'mgmdValidPktsRecvd'
*
* @description [mgmdValidPktsRecvd] Valid Packets Received for the router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdStats_mgmdValidPktsRecvd (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdValidPktsRecvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdStats_mgmdRouterInterfaceType,
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
  owa.l7rc = usmDbValidPacketsRcvdGet (L7_UNIT_CURRENT, keymgmdRouterInterfaceTypeValue,
                              &objmgmdValidPktsRecvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdValidPktsRecvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdValidPktsRecvdValue,
                           sizeof (objmgmdValidPktsRecvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdStats_mgmdValidPktsSent
*
* @purpose Get 'mgmdValidPktsSent'
*
* @description [mgmdValidPktsSent] Valid Packets Sent for the router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdStats_mgmdValidPktsSent (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdValidPktsSentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdStats_mgmdRouterInterfaceType,
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
  owa.l7rc = usmDbValidPacketsSentGet (L7_UNIT_CURRENT, keymgmdRouterInterfaceTypeValue,
                              &objmgmdValidPktsSentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdValidPktsSent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdValidPktsSentValue,
                           sizeof (objmgmdValidPktsSentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdStats_mgmdBadChkSumPktsRecvd
*
* @purpose Get 'mgmdBadChkSumPktsRecvd'
*
* @description [mgmdBadChkSumPktsRecvd] Bad Checksum packets for the router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdStats_mgmdBadChkSumPktsRecvd (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdBadChkSumPktsRecvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdStats_mgmdRouterInterfaceType,
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
  owa.l7rc = usmDbBadCheckSumPktsGet (L7_UNIT_CURRENT, keymgmdRouterInterfaceTypeValue,
                              &objmgmdBadChkSumPktsRecvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdBadChkSumPktsRecvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdBadChkSumPktsRecvdValue,
                           sizeof (objmgmdBadChkSumPktsRecvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdStats_mgmdMalformedPktsRecvd
*
* @purpose Get 'mgmdMalformedPktsRecvd'
*
* @description [mgmdMalformedPktsRecvd] Malformed packets for the router
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdStats_mgmdMalformedPktsRecvd (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdMalformedPktsRecvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdStats_mgmdRouterInterfaceType,
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
  owa.l7rc = usmDbMalformedPktsGet (L7_UNIT_CURRENT, keymgmdRouterInterfaceTypeValue,
                              &objmgmdMalformedPktsRecvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdMalformedPktsRecvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdMalformedPktsRecvdValue,
                           sizeof (objmgmdMalformedPktsRecvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdStats_mgmdRouterInterfaceClearTrafficCounters
*
* @purpose Set 'mgmdRouterInterfaceClearTrafficCounters'
*
* @description [mgmdRouterInterfaceClearTrafficCounters] Clear Traffic Counters
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdStats_mgmdRouterInterfaceClearTrafficCounters (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  fpObjWa_t kwamgmdRouterInterfaceType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceType */
  kwamgmdRouterInterfaceType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdStats_mgmdRouterInterfaceType,
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
  owa.l7rc = usmDbMldTrafficCountersClear (L7_UNIT_CURRENT, keymgmdRouterInterfaceTypeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
