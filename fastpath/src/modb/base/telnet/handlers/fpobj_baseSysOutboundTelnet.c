/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseSysOutboundTelnet.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  6 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseSysOutboundTelnet_obj.h"
#include "usmdb_telnet_api.h"

/*******************************************************************************
* @function fpObjGet_baseSysOutboundTelnet_AdminMode
*
* @purpose Get 'AdminMode'
*
* @description Admin mode of outbound telnet 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysOutboundTelnet_AdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTelnetAdminModeGet (L7_UNIT_CURRENT, &objAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, sizeof (objAdminModeValue));

  /* return the object value: AdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminModeValue,
                           sizeof (objAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysOutboundTelnet_AdminMode
*
* @purpose Set 'AdminMode'
*
* @description Admin mode of outbound telnet 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysOutboundTelnet_AdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbTelnetAdminModeSet (L7_UNIT_CURRENT, objAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSysOutboundTelnet_MaxSessions
*
* @purpose Get 'MaxSessions'
*
* @description Maximum allowed outbound tlenet sessions 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysOutboundTelnet_MaxSessions (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxSessionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTelnetMaxSessionsGet (L7_UNIT_CURRENT, &objMaxSessionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxSessionsValue, sizeof (objMaxSessionsValue));

  /* return the object value: MaxSessions */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxSessionsValue,
                           sizeof (objMaxSessionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysOutboundTelnet_MaxSessions
*
* @purpose Set 'MaxSessions'
*
* @description Maximum allowed outbound tlenet sessions 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysOutboundTelnet_MaxSessions (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxSessionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MaxSessions */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMaxSessionsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxSessionsValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbTelnetMaxSessionsSet (L7_UNIT_CURRENT, objMaxSessionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSysOutboundTelnet_TelnetTimeout
*
* @purpose Get 'TelnetTimeout'
*
* @description Timeout for outbound tlenet sessions 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysOutboundTelnet_TelnetTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTelnetTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTelnetTimeoutGet (L7_UNIT_CURRENT, &objTelnetTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTelnetTimeoutValue,
                     sizeof (objTelnetTimeoutValue));

  /* return the object value: TelnetTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTelnetTimeoutValue,
                           sizeof (objTelnetTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysOutboundTelnet_TelnetTimeout
*
* @purpose Set 'TelnetTimeout'
*
* @description Timeout for outbound tlenet sessions 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysOutboundTelnet_TelnetTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTelnetTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TelnetTimeout */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTelnetTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTelnetTimeoutValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbTelnetTimeoutSet (L7_UNIT_CURRENT, objTelnetTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSysOutboundTelnet_NoOfActiveSessions
*
* @purpose Get 'NoOfActiveSessions'
*
* @description No of currently active outbound telnet sessions 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysOutboundTelnet_NoOfActiveSessions (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNoOfActiveSessionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTelnetNoOfActiveSessionsGet (L7_UNIT_CURRENT,
                                      &objNoOfActiveSessionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNoOfActiveSessionsValue,
                     sizeof (objNoOfActiveSessionsValue));

  /* return the object value: NoOfActiveSessions */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNoOfActiveSessionsValue,
                           sizeof (objNoOfActiveSessionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

