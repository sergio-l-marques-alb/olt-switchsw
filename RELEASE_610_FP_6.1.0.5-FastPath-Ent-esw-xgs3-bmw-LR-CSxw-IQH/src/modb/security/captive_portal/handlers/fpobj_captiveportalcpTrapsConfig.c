
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpTrapsConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to captiveportal-object.xml
*
* @create  06 July 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_captiveportalcpTrapsConfig_obj.h"

#include "usmdb_trapmgr_api.h"
#include "usmdb_cpdm_api.h"

/*******************************************************************************
* @function fpObjGet_captiveportalcpTrapsConfig_cpTrapMode
*
* @purpose Get 'cpTrapMode'
 *@description  [cpTrapMode] Flag to enable/disable following Captive Portal
* SNMP traps: Client Authentication Failures, Client Connections,
* Client Database Full, Client Disconnections.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpTrapsConfig_cpTrapMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpTrapModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTrapCaptivePortalGet (&objcpTrapModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpTrapModeValue, sizeof (objcpTrapModeValue));

  /* return the object value: cpTrapMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpTrapModeValue, sizeof (objcpTrapModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpTrapsConfig_cpTrapMode
*
* @purpose Set 'cpTrapMode'
 *@description  [cpTrapMode] Flag to enable/disable following Captive Portal
* SNMP traps: Client Authentication Failures, Client Connections,
* Client Database Full, Client Disconnections.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpTrapsConfig_cpTrapMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpTrapModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpTrapMode */
  owa.len = sizeof (objcpTrapModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpTrapModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpTrapModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbTrapCaptivePortalSet (objcpTrapModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_TRAPS;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_captiveportalcpTrapsConfig_cpClientAuthenticationFailureTrapMode
*
* @purpose Get 'cpClientAuthenticationFailureTrapMode'
 *@description  [cpClientAuthenticationFailureTrapMode] Flag to enable/disable
* client authentication failure SNMP traps.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpTrapsConfig_cpClientAuthenticationFailureTrapMode (void *wap,
                                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpClientAuthenticationFailureTrapModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalTrapModeGet (CP_TRAP_AUTH_FAILURE, &objcpClientAuthenticationFailureTrapModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpClientAuthenticationFailureTrapModeValue,
                     sizeof (objcpClientAuthenticationFailureTrapModeValue));

  /* return the object value: cpClientAuthenticationFailureTrapMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpClientAuthenticationFailureTrapModeValue,
                           sizeof (objcpClientAuthenticationFailureTrapModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpTrapsConfig_cpClientAuthenticationFailureTrapMode
*
* @purpose Set 'cpClientAuthenticationFailureTrapMode'
 *@description  [cpClientAuthenticationFailureTrapMode] Flag to enable/disable
* client authentication failure SNMP traps.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpTrapsConfig_cpClientAuthenticationFailureTrapMode (void *wap,
                                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpClientAuthenticationFailureTrapModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpClientAuthenticationFailureTrapMode */
  owa.len = sizeof (objcpClientAuthenticationFailureTrapModeValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objcpClientAuthenticationFailureTrapModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpClientAuthenticationFailureTrapModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbCpdmGlobalTrapModeSet (CP_TRAP_AUTH_FAILURE, objcpClientAuthenticationFailureTrapModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_captiveportalcpTrapsConfig_cpClientConnectTrapMode
*
* @purpose Get 'cpClientConnectTrapMode'
 *@description  [cpClientConnectTrapMode] Flag to enable/disable client
* connection SNMP traps.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpTrapsConfig_cpClientConnectTrapMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpClientConnectTrapModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalTrapModeGet (CP_TRAP_CLIENT_CONNECTED, &objcpClientConnectTrapModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpClientConnectTrapModeValue,
                     sizeof (objcpClientConnectTrapModeValue));

  /* return the object value: cpClientConnectTrapMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpClientConnectTrapModeValue,
                           sizeof (objcpClientConnectTrapModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpTrapsConfig_cpClientConnectTrapMode
*
* @purpose Set 'cpClientConnectTrapMode'
 *@description  [cpClientConnectTrapMode] Flag to enable/disable client
* connection SNMP traps.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpTrapsConfig_cpClientConnectTrapMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpClientConnectTrapModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpClientConnectTrapMode */
  owa.len = sizeof (objcpClientConnectTrapModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpClientConnectTrapModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpClientConnectTrapModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbCpdmGlobalTrapModeSet (CP_TRAP_CLIENT_CONNECTED, objcpClientConnectTrapModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_captiveportalcpTrapsConfig_cpClientDatabaseFullTrapMode
*
* @purpose Get 'cpClientDatabaseFullTrapMode'
 *@description  [cpClientDatabaseFullTrapMode] Flag to enable/disable client
* database full SNMP traps.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpTrapsConfig_cpClientDatabaseFullTrapMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpClientDatabaseFullTrapModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalTrapModeGet (CP_TRAP_CONNECTION_DB_FULL, &objcpClientDatabaseFullTrapModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpClientDatabaseFullTrapModeValue,
                     sizeof (objcpClientDatabaseFullTrapModeValue));

  /* return the object value: cpClientDatabaseFullTrapMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpClientDatabaseFullTrapModeValue,
                           sizeof (objcpClientDatabaseFullTrapModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpTrapsConfig_cpClientDatabaseFullTrapMode
*
* @purpose Set 'cpClientDatabaseFullTrapMode'
 *@description  [cpClientDatabaseFullTrapMode] Flag to enable/disable client
* database full SNMP traps.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpTrapsConfig_cpClientDatabaseFullTrapMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpClientDatabaseFullTrapModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpClientDatabaseFullTrapMode */
  owa.len = sizeof (objcpClientDatabaseFullTrapModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpClientDatabaseFullTrapModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpClientDatabaseFullTrapModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbCpdmGlobalTrapModeSet (CP_TRAP_CONNECTION_DB_FULL, objcpClientDatabaseFullTrapModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_captiveportalcpTrapsConfig_cpClientDisconnectTrapMode
*
* @purpose Get 'cpClientDisconnectTrapMode'
 *@description  [cpClientDisconnectTrapMode] Flag to enable/disable client
* disconnection SNMP traps.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpTrapsConfig_cpClientDisconnectTrapMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpClientDisconnectTrapModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalTrapModeGet (CP_TRAP_CLIENT_DISCONNECTED, &objcpClientDisconnectTrapModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpClientDisconnectTrapModeValue,
                     sizeof (objcpClientDisconnectTrapModeValue));

  /* return the object value: cpClientDisconnectTrapMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpClientDisconnectTrapModeValue,
                           sizeof (objcpClientDisconnectTrapModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpTrapsConfig_cpClientDisconnectTrapMode
*
* @purpose Set 'cpClientDisconnectTrapMode'
 *@description  [cpClientDisconnectTrapMode] Flag to enable/disable client
* disconnection SNMP traps.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpTrapsConfig_cpClientDisconnectTrapMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpClientDisconnectTrapModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpClientDisconnectTrapMode */
  owa.len = sizeof (objcpClientDisconnectTrapModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpClientDisconnectTrapModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpClientDisconnectTrapModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbCpdmGlobalTrapModeSet (CP_TRAP_CLIENT_DISCONNECTED, objcpClientDisconnectTrapModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
