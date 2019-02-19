
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpCaptivePortalClientStatusTable.c
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
#include "_xe_captiveportalcpCaptivePortalClientStatusTable_obj.h"

#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_connstatus_api.h"

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress
*
* @purpose Get 'cpCaptivePortalClientMacAddress'
 *@description  [cpCaptivePortalClientMacAddress] The MAC address of the
* workstation from which the client is authenticated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress (void *wap,
                                                                                        void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_enetMacAddr_t objcpCaptivePortalClientMacAddressValue;
  L7_enetMacAddr_t nextObjcpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (objcpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) objcpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_MAC(objcpCaptivePortalClientMacAddressValue.addr);
    owa.l7rc = usmDbCpdmClientConnStatusNextGet (&objcpCaptivePortalClientMacAddressValue, 
                    &nextObjcpCaptivePortalClientMacAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objcpCaptivePortalClientMacAddressValue.addr, owa.len);
    owa.l7rc = usmDbCpdmClientConnStatusNextGet (&objcpCaptivePortalClientMacAddressValue, 
                    &nextObjcpCaptivePortalClientMacAddressValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjcpCaptivePortalClientMacAddressValue.addr, owa.len);

  /* return the object value: cpCaptivePortalClientMacAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjcpCaptivePortalClientMacAddressValue.addr,
                           sizeof (nextObjcpCaptivePortalClientMacAddressValue.addr));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientIpAddress
*
* @purpose Get 'cpCaptivePortalClientIpAddress'
 *@description  [cpCaptivePortalClientIpAddress] The IP address of the
* workstation from which the client is authenticated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientIpAddress (void
                                                                                                *wap,
                                                                                                void
                                                                                                *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibIpV4_t objcpCaptivePortalClientIpAddressValue;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmClientConnStatusIpGet (&keycpCaptivePortalClientMacAddressValue,
                              &objcpCaptivePortalClientIpAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientIpAddressValue,
                     sizeof (objcpCaptivePortalClientIpAddressValue));

  /* return the object value: cpCaptivePortalClientIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalClientIpAddressValue,
                           sizeof (objcpCaptivePortalClientIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalConfigName
*
* @purpose Get 'cpCaptivePortalConfigName'
 *@description  [cpCaptivePortalConfigName] The name, in alpha-numeric
* characters, assigned to this captive portal configuration.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalConfigName (void *wap,
                                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpCaptivePortalConfigNameValue;

  xLibU32_t keycpCaptivePortalClientCPIDValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientCPID */
  owa.len = sizeof (keycpCaptivePortalClientCPIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientCPID,
                          (xLibU8_t *) & keycpCaptivePortalClientCPIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalClientCPIDValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigNameGet (keycpCaptivePortalClientCPIDValue,
                              objcpCaptivePortalConfigNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigNameValue,
                     strlen (objcpCaptivePortalConfigNameValue));

  /* return the object value: cpCaptivePortalConfigName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigNameValue,
                           strlen (objcpCaptivePortalConfigNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientUserName
*
* @purpose Get 'cpCaptivePortalClientUserName'
 *@description  [cpCaptivePortalClientUserName] User name (or Guest ID) of the
* connected client.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientUserName (void
                                                                                               *wap,
                                                                                               void
                                                                                               *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpCaptivePortalClientUserNameValue;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmClientConnStatusUserNameGet (&keycpCaptivePortalClientMacAddressValue,
                              objcpCaptivePortalClientUserNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalClientUserNameValue,
                     strlen (objcpCaptivePortalClientUserNameValue));

  /* return the object value: cpCaptivePortalClientUserName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalClientUserNameValue,
                           strlen (objcpCaptivePortalClientUserNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientProtocolMode
*
* @purpose Get 'cpCaptivePortalClientProtocolMode'
 *@description  [cpCaptivePortalClientProtocolMode] Protocol mode used for the
* client authentication.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientProtocolMode (void *wap,
                                                                                          void
                                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalClientProtocolModeValue;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmClientConnStatusProtocolModeGet (&keycpCaptivePortalClientMacAddressValue,
                              &objcpCaptivePortalClientProtocolModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientProtocolModeValue,
                     sizeof (objcpCaptivePortalClientProtocolModeValue));

  /* return the object value: cpCaptivePortalClientProtocolMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalClientProtocolModeValue,
                           sizeof (objcpCaptivePortalClientProtocolModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientVerificationMode
*
* @purpose Get 'cpCaptivePortalClientVerificationMode'
 *@description  [cpCaptivePortalClientVerificationMode] Type of user
* verification performed for the client authentication.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientVerificationMode (void
                                                                                              *wap,
                                                                                              void
                                                                                              *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalClientVerificationModeValue;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmClientConnStatusVerifyModeGet (&keycpCaptivePortalClientMacAddressValue,
                              &objcpCaptivePortalClientVerificationModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientVerificationModeValue,
                     sizeof (objcpCaptivePortalClientVerificationModeValue));

  /* return the object value: cpCaptivePortalClientVerificationMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalClientVerificationModeValue,
                           sizeof (objcpCaptivePortalClientVerificationModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientAssocIfIndex
*
* @purpose Get 'cpCaptivePortalClientAssocIfIndex'
 *@description  [cpCaptivePortalClientAssocIfIndex] The interface on which the
* client was authenticated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientAssocIfIndex (void *wap,
                                                                                          void
                                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalClientAssocIfIndexValue;
  xLibU32_t objcpCaptivePortalClientIfNum;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  /* get the value from application */
  if (usmDbCpdmClientConnStatusIntfIdGet (&keycpCaptivePortalClientMacAddressValue,
                          &objcpCaptivePortalClientIfNum) == L7_SUCCESS)
  {
    owa.l7rc = nimGetIntfIfIndex(objcpCaptivePortalClientIfNum, 
                    &objcpCaptivePortalClientAssocIfIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientAssocIfIndexValue,
                     sizeof (objcpCaptivePortalClientAssocIfIndexValue));

  /* return the object value: cpCaptivePortalClientAssocIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalClientAssocIfIndexValue,
                           sizeof (objcpCaptivePortalClientAssocIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientCPID
*
* @purpose Get 'cpCaptivePortalClientCPID'
 *@description  [cpCaptivePortalClientCPID] The captive portal instance on which
* the client was authenticated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientCPID (void
                                                                                           *wap,
                                                                                           void
                                                                                           *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalClientCPIDValue;
  xLibU16_t cpClientCPID;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmClientConnStatusCpIdGet (&keycpCaptivePortalClientMacAddressValue,
                              &cpClientCPID);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  objcpCaptivePortalClientCPIDValue = (xLibU32_t)cpClientCPID;

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientCPIDValue,
                     sizeof (objcpCaptivePortalClientCPIDValue));

  /* return the object value: cpCaptivePortalClientCPID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalClientCPIDValue,
                           sizeof (objcpCaptivePortalClientCPIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientSessionTime
*
* @purpose Get 'cpCaptivePortalClientSessionTime'
 *@description  [cpCaptivePortalClientSessionTime] The time (in seconds) since
* the client was authenticated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientSessionTime (void *wap,
                                                                                         void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalClientSessionTimeValue;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmClientConnStatusSessionTimeGet (&keycpCaptivePortalClientMacAddressValue,
                              &objcpCaptivePortalClientSessionTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientSessionTimeValue,
                     sizeof (objcpCaptivePortalClientSessionTimeValue));

  /* return the object value: cpCaptivePortalClientSessionTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalClientSessionTimeValue,
                           sizeof (objcpCaptivePortalClientSessionTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientSwitchMacAddress
*
* @purpose Get 'cpCaptivePortalClientSwitchMacAddress'
 *@description  [cpCaptivePortalClientSwitchMacAddress] The MAC address of the
* switch to which the client is authenticated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientSwitchMacAddress (void
                                                                                              *wap,
                                                                                              void
                                                                                              *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_enetMacAddr_t objcpCaptivePortalClientSwitchMacAddressValue;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);
      
  if (usmDbCpdmClusterSupportGet() == L7_SUCCESS)
  {
    /* get the value from application */
    owa.l7rc = usmDbCpdmClientConnStatusSwitchMacAddrGet (&keycpCaptivePortalClientMacAddressValue,
                              &objcpCaptivePortalClientSwitchMacAddressValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalClientSwitchMacAddressValue.addr,
                     sizeof (objcpCaptivePortalClientSwitchMacAddressValue.addr));

  /* return the object value: cpCaptivePortalClientSwitchMacAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalClientSwitchMacAddressValue.addr,
                           sizeof (objcpCaptivePortalClientSwitchMacAddressValue.addr));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientSwitchIpAddress
*
* @purpose Get 'cpCaptivePortalClientSwitchIpAddress'
 *@description  [cpCaptivePortalClientSwitchIpAddress] The IP address of the
* switch to which the client is authenticated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientSwitchIpAddress (void
                                                                                             *wap,
                                                                                             void
                                                                                             *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibIpV4_t objcpCaptivePortalClientSwitchIpAddressValue;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  if (usmDbCpdmClusterSupportGet() == L7_SUCCESS)
  {
    /* get the value from application */
    owa.l7rc = usmDbCpdmClientConnStatusSwitchIpGet (&keycpCaptivePortalClientMacAddressValue,
                              &objcpCaptivePortalClientSwitchIpAddressValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientSwitchIpAddressValue,
                     sizeof (objcpCaptivePortalClientSwitchIpAddressValue));

  /* return the object value: cpCaptivePortalClientSwitchIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalClientSwitchIpAddressValue,
                           sizeof (objcpCaptivePortalClientSwitchIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientSwitchType
*
* @purpose Get 'cpCaptivePortalClientSwitchType'
 *@description  [cpCaptivePortalClientSwitchType] The switch (peer or local) to
* which the client is authenticated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientSwitchType (void *wap,
                                                                                        void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalClientSwitchTypeValue;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  if (usmDbCpdmClusterSupportGet() == L7_SUCCESS)
  {
    /* get the value from application */
    owa.l7rc = usmDbCpdmClientConnStatusWhichSwitchGet (&keycpCaptivePortalClientMacAddressValue,
                              &objcpCaptivePortalClientSwitchTypeValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientSwitchTypeValue,
                     sizeof (objcpCaptivePortalClientSwitchTypeValue));

  /* return the object value: cpCaptivePortalClientSwitchType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalClientSwitchTypeValue,
                           sizeof (objcpCaptivePortalClientSwitchTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientDeauthAction
*
* @purpose Get 'cpCaptivePortalClientDeauthAction'
 *@description  [cpCaptivePortalClientDeauthAction] This is an action object.
* Setting this object to start will initiate the deauthentication of
* authenticated client. Read on this object will always return none.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientDeauthAction (void *wap,
                                                                                          void
                                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalClientDeauthActionValue;

  objcpCaptivePortalClientDeauthActionValue = L7_XUI_MANUAL_ACTION_NONE;

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientDeauthActionValue,
                     sizeof (objcpCaptivePortalClientDeauthActionValue));

  /* return the object value: cpCaptivePortalClientDeauthAction */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalClientDeauthActionValue,
                           sizeof (objcpCaptivePortalClientDeauthActionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientDeauthAction
*
* @purpose Set 'cpCaptivePortalClientDeauthAction'
 *@description  [cpCaptivePortalClientDeauthAction] This is an action object.
* Setting this object to start will initiate the deauthentication of
* authenticated client. Read on this object will always return none.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientDeauthAction (void *wap,
                                                                                          void
                                                                                          *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalClientDeauthActionValue;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalClientDeauthAction */
  owa.len = sizeof (objcpCaptivePortalClientDeauthActionValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objcpCaptivePortalClientDeauthActionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientDeauthActionValue, owa.len);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  if ((objcpCaptivePortalClientDeauthActionValue == L7_XUI_MANUAL_ACTION_START) && (usmDbCpdmClientConnStatusGet(&keycpCaptivePortalClientMacAddressValue) == L7_SUCCESS))
  {
    /* set the value in application */
    owa.l7rc = usmDbCpdmClientConnStatusDelete (&keycpCaptivePortalClientMacAddressValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

#if 0
/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientAuthFailureConnectionAttempts
*
* @purpose Get 'cpCaptivePortalClientAuthFailureConnectionAttempts'
 *@description  [cpCaptivePortalClientAuthFailureConnectionAttempts] The captive
* portal authentication failure count.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientAuthFailureConnectionAttempts
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalClientAuthFailureConnectionAttemptsValue;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keycpCaptivePortalClientMacAddressValue,
                              &objcpCaptivePortalClientAuthFailureConnectionAttemptsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientAuthFailureConnectionAttemptsValue,
                     sizeof (objcpCaptivePortalClientAuthFailureConnectionAttemptsValue));

  /* return the object value: cpCaptivePortalClientAuthFailureConnectionAttempts */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalClientAuthFailureConnectionAttemptsValue,
                    sizeof (objcpCaptivePortalClientAuthFailureConnectionAttemptsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
#endif

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientConnStatusDeleteAll
*
* @purpose Set 'cpCaptivePortalClientConnStatusDeleteAll'
 *@description  [cpCaptivePortalClientConnStatusDeleteAll] Delete all current
* connections.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_captiveportalcpCaptivePortalClientStatusTable_cpCaptivePortalClientConnStatusDeleteAll
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalClientConnStatusDeleteAllValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalClientConnStatusDeleteAll */
  owa.len = sizeof (objcpCaptivePortalClientConnStatusDeleteAllValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objcpCaptivePortalClientConnStatusDeleteAllValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientConnStatusDeleteAllValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbCpdmClientConnStatusDeleteAll ();
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_DEAUTH_CLIENTS;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
