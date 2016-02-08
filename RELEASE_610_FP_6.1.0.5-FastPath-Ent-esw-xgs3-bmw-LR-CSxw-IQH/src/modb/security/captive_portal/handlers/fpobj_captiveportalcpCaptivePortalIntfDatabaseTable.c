
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpCaptivePortalIntfDatabaseTable.c
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
#include "_xe_captiveportalcpCaptivePortalIntfDatabaseTable_obj.h"

#include "usmdb_cpim_api.h"

extern L7_char8 *pStrInfo_common_Supported;
extern L7_char8 *pStrErr_common_NotSupported;

#if 0
/*********************************************************************
* @purpose  Get interface capability mask
*
* @param    L7_uint32  intf   @b{(input)} interface
* @param    L7_uchar8  *mask  @b{(output)} pointer to capability mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpCaptivePortalIntfCapabilitiesGet(L7_uint32 intf, L7_uchar8 *buf, L7_uint32 *buf_len)
{
  L7_uchar8 temp_val;

  *buf_len = 2;
  if (usmDbCpimIntfCapabilitySessionTimeoutGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *buf |= (1 << D_cpCaptivePortalIntfCapabilities_sessionTimeout);
    }
  }
  if (usmDbCpimIntfCapabilityIdleTimeoutGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *buf |= (1 << D_cpCaptivePortalIntfCapabilities_idleTimeout);
    }
  }
  if (usmDbCpimIntfCapabilityBytesReceivedCounterGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *buf |= (1 << D_cpCaptivePortalIntfCapabilities_maxBytesReceivedCounter);
    }
  }
  if (usmDbCpimIntfCapabilityBytesTransmittedCounterGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *(buf) |= (1 << (D_cpCaptivePortalIntfCapabilities_maxBytesTransmittedCounter));
    }
  }
  if (usmDbCpimIntfCapabilityPacketsReceivedCounterGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *(buf) |= (1 << (D_cpCaptivePortalIntfCapabilities_maxPacketsReceivedCounter));
    }
  }
  if (usmDbCpimIntfCapabilityPacketsTransmittedCounterGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *(buf) |= (1 << (D_cpCaptivePortalIntfCapabilities_maxPacketsTransmittedCounter));
    }
  }
  if (usmDbCpimIntfCapabilityRoamingSupportGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *(buf) |= (1 << (D_cpCaptivePortalIntfCapabilities_clientRoaming));
    }
  }
#if  FASTPATH_BUT_NOT_DLINK
  if (usmDbCpimIntfCapabilityBandwidthUpRateControlGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *buf |= (1 << D_cpCaptivePortalIntfCapabilities_maxBandwidthUp);
    }
  }
  if (usmDbCpimIntfCapabilityBandwidthDownRateControlGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *buf |= (1 << D_cpCaptivePortalIntfCapabilities_maxBandwidthDown);
    }
  }
  if (usmDbCpimIntfCapabilityMaxInputOctetMonitorGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *buf |= (1 << D_cpCaptivePortalIntfCapabilities_maxInputOctets);
    }
  }
  if (usmDbCpimIntfCapabilityMaxOutputOctetMonitorGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *buf |= (1 << D_cpCaptivePortalIntfCapabilities_maxOutputOctets);
    }
  }
#endif

  return L7_SUCCESS;
}
#endif


/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfIfIndex
*
* @purpose Get 'cpCaptivePortalIntfIfIndex'
 *@description  [cpCaptivePortalIntfIfIndex] The ifIndex of the interface
* associated with this entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfIfIndex (void
                                                                                            *wap,
                                                                                            void
                                                                                            *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalIntfIfIndexValue;
  xLibU32_t nextObjcpCaptivePortalIntfIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (objcpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & objcpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objcpCaptivePortalIntfIfIndexValue = 0;
    owa.l7rc = usmDbCpimIntfNextGet (objcpCaptivePortalIntfIfIndexValue, &nextObjcpCaptivePortalIntfIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpCaptivePortalIntfIfIndexValue, owa.len);
    owa.l7rc = usmDbCpimIntfNextGet (objcpCaptivePortalIntfIfIndexValue, &nextObjcpCaptivePortalIntfIfIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpCaptivePortalIntfIfIndexValue, owa.len);

  /* return the object value: cpCaptivePortalIntfIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpCaptivePortalIntfIfIndexValue,
                           sizeof (nextObjcpCaptivePortalIntfIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

#if 0
/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilities
*
* @purpose Get 'cpCaptivePortalIntfCapabilities'
 *@description  [cpCaptivePortalIntfCapabilities] The bitmap value used to
* identify which capabilities are available on the interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilities (void *wap,
                                                                                        void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpCaptivePortalIntfCapabilitiesValue;
  xLibU32_t    objcpCaptivePortalIntfCapabilitiesValueLen;

  xLibU32_t keycpCaptivePortalIntfIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (keycpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & keycpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalIntfIfIndexValue, owa.len);

  /* get the value from application */
  FPOBJ_CLR_STR256(objcpCaptivePortalIntfCapabilitiesValue);
  owa.l7rc = snmpCpCaptivePortalIntfCapabilitiesGet (keycpCaptivePortalIntfIfIndexValue,
                              objcpCaptivePortalIntfCapabilitiesValue, 
                              &objcpCaptivePortalIntfCapabilitiesValueLen);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalIntfCapabilitiesValue,
                     strlen (objcpCaptivePortalIntfCapabilitiesValue));

  /* return the object value: cpCaptivePortalIntfCapabilities */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalIntfCapabilitiesValue,
                           strlen (objcpCaptivePortalIntfCapabilitiesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
#endif

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilitySessionTimeout
*
* @purpose Get 'cpCaptivePortalIntfCapabilitySessionTimeout'
 *@description  [cpCaptivePortalIntfCapabilitySessionTimeout] Interface
* capability field: sessionTimeout.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilitySessionTimeout
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpCaptivePortalIntfCapabilitySessionTimeoutValue;

  xLibU32_t keycpCaptivePortalIntfIfIndexValue;
  xLibU8_t  val;

  FPOBJ_TRACE_ENTER (bufp);
  val = L7_DISABLE;
  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (keycpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & keycpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalIntfIfIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpimIntfCapabilitySessionTimeoutGet (keycpCaptivePortalIntfIfIndexValue,
                              &val);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (val == L7_ENABLE)
  {
    osapiStrncpy(objcpCaptivePortalIntfCapabilitySessionTimeoutValue,pStrInfo_common_Supported,
                    sizeof(objcpCaptivePortalIntfCapabilitySessionTimeoutValue));
  }
  else
  {
    osapiStrncpy(objcpCaptivePortalIntfCapabilitySessionTimeoutValue,pStrErr_common_NotSupported,
                    sizeof(objcpCaptivePortalIntfCapabilitySessionTimeoutValue));
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalIntfCapabilitySessionTimeoutValue,
                     strlen (objcpCaptivePortalIntfCapabilitySessionTimeoutValue));

  /* return the object value: cpCaptivePortalIntfCapabilitySessionTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalIntfCapabilitySessionTimeoutValue,
                           strlen (objcpCaptivePortalIntfCapabilitySessionTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilityIdleTimeout
*
* @purpose Get 'cpCaptivePortalIntfCapabilityIdleTimeout'
 *@description  [cpCaptivePortalIntfCapabilityIdleTimeout] Interface capability
* field: idleTimeout.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilityIdleTimeout
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpCaptivePortalIntfCapabilityIdleTimeoutValue;

  xLibU32_t keycpCaptivePortalIntfIfIndexValue;
  xLibU8_t  val;

  FPOBJ_TRACE_ENTER (bufp);
  val = L7_DISABLE;
  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (keycpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & keycpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalIntfIfIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpimIntfCapabilityIdleTimeoutGet (keycpCaptivePortalIntfIfIndexValue,
                              &val);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (val == L7_ENABLE)
  {
    osapiStrncpy(objcpCaptivePortalIntfCapabilityIdleTimeoutValue,pStrInfo_common_Supported,
                    sizeof(objcpCaptivePortalIntfCapabilityIdleTimeoutValue));
  }
  else
  {
    osapiStrncpy(objcpCaptivePortalIntfCapabilityIdleTimeoutValue,pStrErr_common_NotSupported,
                    sizeof(objcpCaptivePortalIntfCapabilityIdleTimeoutValue));
  }


  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalIntfCapabilityIdleTimeoutValue,
                     strlen (objcpCaptivePortalIntfCapabilityIdleTimeoutValue));

  /* return the object value: cpCaptivePortalIntfCapabilityIdleTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalIntfCapabilityIdleTimeoutValue,
                           strlen (objcpCaptivePortalIntfCapabilityIdleTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilityBytesReceivedCounter
*
* @purpose Get 'cpCaptivePortalIntfCapabilityBytesReceivedCounter'
 *@description  [cpCaptivePortalIntfCapabilityBytesReceivedCounter] Interface
* capability field: bytesReceivedCounter.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilityBytesReceivedCounter
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpCaptivePortalIntfCapabilityBytesReceivedCounterValue;

  xLibU32_t keycpCaptivePortalIntfIfIndexValue;
  xLibU8_t  val;

  FPOBJ_TRACE_ENTER (bufp);
  val = L7_DISABLE;
  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (keycpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & keycpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalIntfIfIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpimIntfCapabilityBytesReceivedCounterGet (keycpCaptivePortalIntfIfIndexValue,
                              &val);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (val == L7_ENABLE)
  {
    osapiStrncpy(objcpCaptivePortalIntfCapabilityBytesReceivedCounterValue,pStrInfo_common_Supported,
                    sizeof(objcpCaptivePortalIntfCapabilityBytesReceivedCounterValue));
  }
  else
  {
    osapiStrncpy(objcpCaptivePortalIntfCapabilityBytesReceivedCounterValue,pStrErr_common_NotSupported,
                    sizeof(objcpCaptivePortalIntfCapabilityBytesReceivedCounterValue));
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalIntfCapabilityBytesReceivedCounterValue,
                     strlen (objcpCaptivePortalIntfCapabilityBytesReceivedCounterValue));

  /* return the object value: cpCaptivePortalIntfCapabilityBytesReceivedCounter */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalIntfCapabilityBytesReceivedCounterValue,
                    strlen (objcpCaptivePortalIntfCapabilityBytesReceivedCounterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilityBytesTransmittedCounter
*
* @purpose Get 'cpCaptivePortalIntfCapabilityBytesTransmittedCounter'
 *@description  [cpCaptivePortalIntfCapabilityBytesTransmittedCounter] Interface
* capability field: bytesTransmittedCounter.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilityBytesTransmittedCounter
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpCaptivePortalIntfCapabilityBytesTransmittedCounterValue;

  xLibU32_t keycpCaptivePortalIntfIfIndexValue;
  xLibU8_t  val;

  FPOBJ_TRACE_ENTER (bufp);
  val  = L7_DISABLE;
  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (keycpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & keycpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalIntfIfIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpimIntfCapabilityBytesTransmittedCounterGet (keycpCaptivePortalIntfIfIndexValue,
                              &val);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (val == L7_ENABLE)
  {
    osapiStrncpy(objcpCaptivePortalIntfCapabilityBytesTransmittedCounterValue,pStrInfo_common_Supported,
                    sizeof(objcpCaptivePortalIntfCapabilityBytesTransmittedCounterValue));
  }
  else
  {
    osapiStrncpy(objcpCaptivePortalIntfCapabilityBytesTransmittedCounterValue,pStrErr_common_NotSupported,
                    sizeof(objcpCaptivePortalIntfCapabilityBytesTransmittedCounterValue));
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalIntfCapabilityBytesTransmittedCounterValue,
                     strlen (objcpCaptivePortalIntfCapabilityBytesTransmittedCounterValue));

  /* return the object value: cpCaptivePortalIntfCapabilityBytesTransmittedCounter */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalIntfCapabilityBytesTransmittedCounterValue,
                    strlen (objcpCaptivePortalIntfCapabilityBytesTransmittedCounterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilityPacketsReceivedCounter
*
* @purpose Get 'cpCaptivePortalIntfCapabilityPacketsReceivedCounter'
 *@description  [cpCaptivePortalIntfCapabilityPacketsReceivedCounter] Interface
* capability field: packetsReceivedCounter.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilityPacketsReceivedCounter
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpCaptivePortalIntfCapabilityPacketsReceivedCounterValue;

  xLibU32_t keycpCaptivePortalIntfIfIndexValue;
  xLibU8_t  val;

  FPOBJ_TRACE_ENTER (bufp);
  val  =  L7_DISABLE;
  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (keycpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & keycpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalIntfIfIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpimIntfCapabilityPacketsReceivedCounterGet (keycpCaptivePortalIntfIfIndexValue,
                              &val);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (val == L7_ENABLE)
  {
    osapiStrncpy(objcpCaptivePortalIntfCapabilityPacketsReceivedCounterValue,pStrInfo_common_Supported,
                    sizeof(objcpCaptivePortalIntfCapabilityPacketsReceivedCounterValue));
  }
  else
  {
    osapiStrncpy(objcpCaptivePortalIntfCapabilityPacketsReceivedCounterValue,pStrErr_common_NotSupported,
                    sizeof(objcpCaptivePortalIntfCapabilityPacketsReceivedCounterValue));
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalIntfCapabilityPacketsReceivedCounterValue,
                     strlen (objcpCaptivePortalIntfCapabilityPacketsReceivedCounterValue));

  /* return the object value: cpCaptivePortalIntfCapabilityPacketsReceivedCounter */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalIntfCapabilityPacketsReceivedCounterValue,
                    strlen (objcpCaptivePortalIntfCapabilityPacketsReceivedCounterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilityPacketsTransmittedCounter
*
* @purpose Get 'cpCaptivePortalIntfCapabilityPacketsTransmittedCounter'
 *@description  [cpCaptivePortalIntfCapabilityPacketsTransmittedCounter]
* Interface capability field: packetsTransmittedCounter.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilityPacketsTransmittedCounter
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpCaptivePortalIntfCapabilityPacketsTransmittedCounterValue;

  xLibU32_t keycpCaptivePortalIntfIfIndexValue;
  xLibU8_t  val;

  FPOBJ_TRACE_ENTER (bufp);
  val =  L7_DISABLE;
  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (keycpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & keycpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalIntfIfIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpimIntfCapabilityPacketsTransmittedCounterGet (keycpCaptivePortalIntfIfIndexValue,
                              &val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (val == L7_ENABLE)
  {
    osapiStrncpy(objcpCaptivePortalIntfCapabilityPacketsTransmittedCounterValue,pStrInfo_common_Supported,
                    sizeof(objcpCaptivePortalIntfCapabilityPacketsTransmittedCounterValue));
  }
  else
  {
    osapiStrncpy(objcpCaptivePortalIntfCapabilityPacketsTransmittedCounterValue,pStrErr_common_NotSupported,
                    sizeof(objcpCaptivePortalIntfCapabilityPacketsTransmittedCounterValue));
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalIntfCapabilityPacketsTransmittedCounterValue,
                     strlen (objcpCaptivePortalIntfCapabilityPacketsTransmittedCounterValue));

  /* return the object value: cpCaptivePortalIntfCapabilityPacketsTransmittedCounter */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) objcpCaptivePortalIntfCapabilityPacketsTransmittedCounterValue,
                    strlen (objcpCaptivePortalIntfCapabilityPacketsTransmittedCounterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilityRoamingSupport
*
* @purpose Get 'cpCaptivePortalIntfCapabilityRoamingSupport'
 *@description  [cpCaptivePortalIntfCapabilityRoamingSupport] Interface
* capability field: roamingSupport.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilityRoamingSupport
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpCaptivePortalIntfCapabilityRoamingSupportValue;

  xLibU32_t keycpCaptivePortalIntfIfIndexValue;
  xLibU8_t  val;

  FPOBJ_TRACE_ENTER (bufp);
  
  val =  L7_DISABLE;
  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (keycpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & keycpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalIntfIfIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpimIntfCapabilityRoamingSupportGet (keycpCaptivePortalIntfIfIndexValue,
                              &val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (val == L7_ENABLE)
  {
    osapiStrncpy(objcpCaptivePortalIntfCapabilityRoamingSupportValue,pStrInfo_common_Supported,
                    sizeof(objcpCaptivePortalIntfCapabilityRoamingSupportValue));
  }
  else
  {
    osapiStrncpy(objcpCaptivePortalIntfCapabilityRoamingSupportValue,pStrErr_common_NotSupported,
                    sizeof(objcpCaptivePortalIntfCapabilityRoamingSupportValue));
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalIntfCapabilityRoamingSupportValue,
                     strlen (objcpCaptivePortalIntfCapabilityRoamingSupportValue));

  /* return the object value: cpCaptivePortalIntfCapabilityRoamingSupport */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalIntfCapabilityRoamingSupportValue,
                           strlen (objcpCaptivePortalIntfCapabilityRoamingSupportValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilityDescription
*
* @purpose Get 'cpCaptivePortalIntfCapabilityDescription'
 *@description  [cpCaptivePortalIntfCapabilityDescription] Interface Capabilty
* field Description.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfCapabilityDescription
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalIntfCapabilityDescriptionValue[CP_INTF_DESCRIPTION_MAX_LEN+1];

  xLibU32_t keycpCaptivePortalIntfIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (keycpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfDatabaseTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & keycpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalIntfIfIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpimIntfCapabilityDescriptionGet (keycpCaptivePortalIntfIfIndexValue,
                              objcpCaptivePortalIntfCapabilityDescriptionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalIntfCapabilityDescriptionValue,
                     strlen (objcpCaptivePortalIntfCapabilityDescriptionValue));

  /* return the object value: cpCaptivePortalIntfCapabilityDescription */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalIntfCapabilityDescriptionValue,
                           strlen (objcpCaptivePortalIntfCapabilityDescriptionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
