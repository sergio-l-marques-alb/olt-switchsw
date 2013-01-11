/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseSysInboundTelnet.c
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
#include "_xe_baseSysInboundTelnet_obj.h"
#include "usmdb_sim_api.h"
#include "cli_web_mgr_api.h"


/*******************************************************************************
* @function fpObjGet_baseSysInboundTelnet_Timeout
*
* @purpose Get 'Timeout'
*
* @description Telnet timeout period in minutes 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysInboundTelnet_Timeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbAgentTelnetTimeoutGet (L7_UNIT_CURRENT, &objTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTimeoutValue, sizeof (objTimeoutValue));

  /* return the object value: Timeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTimeoutValue,
                           sizeof (objTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysInboundTelnet_Timeout
*
* @purpose Set 'Timeout'
*
* @description Telnet timeout period in minutes 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysInboundTelnet_Timeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Timeout */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTimeoutValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAgentTelnetTimeoutSet (L7_UNIT_CURRENT, objTimeoutValue);
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
* @function fpObjGet_baseSysInboundTelnet_NumSessions
*
* @purpose Get 'NumSessions'
*
* @description umber of telnet sessions 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysInboundTelnet_NumSessions (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNumSessionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbAgentTelnetNumSessionsGet (L7_UNIT_CURRENT, &objNumSessionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNumSessionsValue, sizeof (objNumSessionsValue));

  /* return the object value: NumSessions */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumSessionsValue,
                           sizeof (objNumSessionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysInboundTelnet_NumSessions
*
* @purpose Set 'NumSessions'
*
* @description umber of telnet sessions 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysInboundTelnet_NumSessions (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNumSessionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NumSessions */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objNumSessionsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNumSessionsValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbAgentTelnetNumSessionsSet (L7_UNIT_CURRENT, objNumSessionsValue);
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
* @function fpObjGet_baseSysInboundTelnet_NewSessions
*
* @purpose Get 'NewSessions'
*
* @description Allow or disallow new telnet sessions 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysInboundTelnet_NewSessions (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNewSessionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbAgentTelnetNewSessionsGet (L7_UNIT_CURRENT, &objNewSessionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNewSessionsValue, sizeof (objNewSessionsValue));

  /* return the object value: NewSessions */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNewSessionsValue,
                           sizeof (objNewSessionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysInboundTelnet_NewSessions
*
* @purpose Set 'NewSessions'
*
* @description Allow or disallow new telnet sessions 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysInboundTelnet_NewSessions (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNewSessionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NewSessions */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objNewSessionsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNewSessionsValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbAgentTelnetNewSessionsSet (L7_UNIT_CURRENT, objNewSessionsValue);
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
* @function fpObjGet_baseSysInboundTelnet_AdminMode
*
* @purpose Get 'AdminMode'
*
* @description Admin Mode of the telnet connection 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysInboundTelnet_AdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;
  objAdminModeValue = L7_ENABLE;
  usmDbAgentTelnetAdminModeGet (L7_UNIT_CURRENT, &objAdminModeValue);
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
* @function fpObjSet_baseSysInboundTelnet_AdminMode
*
* @purpose Set 'AdminMode'
*
* @description Admin Mode of the telnet connection 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysInboundTelnet_AdminMode (void *wap, void *bufp)
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
  owa.l7rc = L7_SUCCESS;
  usmDbAgentTelnetAdminModeSet (L7_UNIT_CURRENT, objAdminModeValue);
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
* @function fpObjGet_baseSysInboundTelnet_TelnetPortNumber
*
* @purpose Get 'TelnetPortNumber'
 *@description  [TelnetPortNumber]    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysInboundTelnet_TelnetPortNumber (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTelnetPortNumberValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlTelnetMgmtPortNumGet(L7_UNIT_CURRENT, &objTelnetPortNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTelnetPortNumberValue, sizeof (objTelnetPortNumberValue));

  /* return the object value: sysWebHTTPPortNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTelnetPortNumberValue,
                           sizeof (objTelnetPortNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSysInboundTelnet_sysTelnetPortNumber
*
* @purpose Set 'sysTelnetPortNumber'
 *@description  [sysTelnetPortNumber]    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysInboundTelnet_TelnetPortNumber (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTelnetPortNumberValue;
  xLibU32_t objsysTelnetPortFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysTelnetPortNumber */
  owa.len = sizeof (objTelnetPortNumberValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTelnetPortNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTelnetPortNumberValue, owa.len);

  /* retrieve key: sysTelnetPortNumber */
  owa.len = sizeof (objsysTelnetPortFlagValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSysInboundTelnet_sysTelnetPortFlag,
                          (xLibU8_t *) & objsysTelnetPortFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlTelnetMgmtPortNumSet (L7_UNIT_CURRENT, objTelnetPortNumberValue,
                                         objsysTelnetPortFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseSysWebConnection_sysTelnetPortFlag
*
* @purpose Set 'sysTelnetPortFlag'
 *@description  [sysTelnetPortFlag] 
* Access   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysInboundTelnet_sysTelnetPortFlag (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysTelnetPortFlagValue;

  xLibU32_t keyTelnetPortNumberValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysWebHTTPPortFlag */
  owa.len = sizeof (objsysTelnetPortFlagValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysTelnetPortFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTelnetPortFlagValue, owa.len);

  /* retrieve key: sysWebHTTPPortNumber */
  owa.len = sizeof (keyTelnetPortNumberValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSysInboundTelnet_TelnetPortNumber,
                          (xLibU8_t *) & keyTelnetPortNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTelnetPortNumberValue, owa.len);

  return XLIBRC_SUCCESS;
}

