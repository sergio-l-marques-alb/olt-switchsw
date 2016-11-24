
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpCaptivePortalClientStatisticsTable.c
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
#include "_xe_captiveportalcpCaptivePortalClientStatisticsTable_obj.h"

#include "usmdb_cpdm_connstatus_api.h"

typedef unsigned long long xLibUL64_t;

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientMacAddress
*
* @purpose Get 'cpCaptivePortalClientMacAddress'
 *@description  [cpCaptivePortalClientMacAddress] The MAC address of the
* workstation from which the client is authenticated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientMacAddress (void
                                                                                            *wap,
                                                                                            void
                                                                                            *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_enetMacAddr_t objcpCaptivePortalClientMacAddressValue;
  L7_enetMacAddr_t nextObjcpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (objcpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientMacAddress,
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
* @function fpObjGet_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientBytesReceived
*
* @purpose Get 'cpCaptivePortalClientBytesReceived'
 *@description  [cpCaptivePortalClientBytesReceived] Number of bytes received by
* this client.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientBytesReceived (void
                                                                                               *wap,
                                                                                               void
                                                                                               *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibUL64_t objcpCaptivePortalClientBytesReceivedValue;
  xLibUL64_t objcpCaptivePortalClientBytesTransmittedValue;
  xLibUL64_t objcpCaptivePortalClientPacketsReceivedValue;
  xLibUL64_t objcpCaptivePortalClientPacketsTransmittedValue;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmClientConnStatusStatisticsGet (&keycpCaptivePortalClientMacAddressValue,
                              &objcpCaptivePortalClientBytesTransmittedValue,
                              &objcpCaptivePortalClientBytesReceivedValue,
                              &objcpCaptivePortalClientPacketsTransmittedValue,
                              &objcpCaptivePortalClientPacketsReceivedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientBytesReceivedValue,
                     sizeof (objcpCaptivePortalClientBytesReceivedValue));

  /* return the object value: cpCaptivePortalClientBytesReceived */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalClientBytesReceivedValue,
                           sizeof (objcpCaptivePortalClientBytesReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientBytesTransmitted
*
* @purpose Get 'cpCaptivePortalClientBytesTransmitted'
 *@description  [cpCaptivePortalClientBytesTransmitted] Number of bytes
* transmitted by this client.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientBytesTransmitted
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibUL64_t objcpCaptivePortalClientBytesTransmittedValue;
  xLibUL64_t objcpCaptivePortalClientBytesReceivedValue;
  xLibUL64_t objcpCaptivePortalClientPacketsReceivedValue;
  xLibUL64_t objcpCaptivePortalClientPacketsTransmittedValue;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmClientConnStatusStatisticsGet (&keycpCaptivePortalClientMacAddressValue,
                              &objcpCaptivePortalClientBytesTransmittedValue,
                              &objcpCaptivePortalClientBytesReceivedValue,
                              &objcpCaptivePortalClientPacketsTransmittedValue,
                              &objcpCaptivePortalClientPacketsReceivedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientBytesTransmittedValue,
                     sizeof (objcpCaptivePortalClientBytesTransmittedValue));

  /* return the object value: cpCaptivePortalClientBytesTransmitted */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalClientBytesTransmittedValue,
                           sizeof (objcpCaptivePortalClientBytesTransmittedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientPacketsReceived
*
* @purpose Get 'cpCaptivePortalClientPacketsReceived'
 *@description  [cpCaptivePortalClientPacketsReceived] Number of packets
* received by this client.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientPacketsReceived
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibUL64_t objcpCaptivePortalClientPacketsReceivedValue;
  xLibUL64_t objcpCaptivePortalClientBytesReceivedValue;
  xLibUL64_t objcpCaptivePortalClientBytesTransmittedValue;
  xLibUL64_t objcpCaptivePortalClientPacketsTransmittedValue;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmClientConnStatusStatisticsGet (&keycpCaptivePortalClientMacAddressValue,
                              &objcpCaptivePortalClientBytesTransmittedValue,
                              &objcpCaptivePortalClientBytesReceivedValue,
                              &objcpCaptivePortalClientPacketsTransmittedValue,
                              &objcpCaptivePortalClientPacketsReceivedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientPacketsReceivedValue,
                     sizeof (objcpCaptivePortalClientPacketsReceivedValue));

  /* return the object value: cpCaptivePortalClientPacketsReceived */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalClientPacketsReceivedValue,
                           sizeof (objcpCaptivePortalClientPacketsReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientPacketsTransmitted
*
* @purpose Get 'cpCaptivePortalClientPacketsTransmitted'
 *@description  [cpCaptivePortalClientPacketsTransmitted] Number of packets
* transmitted by this client.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientPacketsTransmitted
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibUL64_t objcpCaptivePortalClientPacketsTransmittedValue;
  xLibUL64_t objcpCaptivePortalClientBytesReceivedValue;
  xLibUL64_t objcpCaptivePortalClientBytesTransmittedValue;
  xLibUL64_t objcpCaptivePortalClientPacketsReceivedValue;

  L7_enetMacAddr_t keycpCaptivePortalClientMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalClientMacAddress */
  owa.len = sizeof (keycpCaptivePortalClientMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalClientStatisticsTable_cpCaptivePortalClientMacAddress,
                   (xLibU8_t *) keycpCaptivePortalClientMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalClientMacAddressValue.addr, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmClientConnStatusStatisticsGet (&keycpCaptivePortalClientMacAddressValue,
                              &objcpCaptivePortalClientBytesTransmittedValue,
                              &objcpCaptivePortalClientBytesReceivedValue,
                              &objcpCaptivePortalClientPacketsTransmittedValue,
                              &objcpCaptivePortalClientPacketsReceivedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalClientPacketsTransmittedValue,
                     sizeof (objcpCaptivePortalClientPacketsTransmittedValue));

  /* return the object value: cpCaptivePortalClientPacketsTransmitted */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalClientPacketsTransmittedValue,
                           sizeof (objcpCaptivePortalClientPacketsTransmittedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
