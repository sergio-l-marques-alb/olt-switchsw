/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_securitySSLConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ssl-object.xml
*
* @create  14 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_securitySSLConfig_obj.h"
#include "usmdb_sslt_api.h"
#include "sslt_exports.h"

/*******************************************************************************
* @function fpObjGet_securitySSLConfig_AdminMode
*
* @purpose Get 'AdminMode'
*
* @description [AdminMode]: SSL admin mode 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSLConfig_AdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbssltAdminModeGet (L7_UNIT_CURRENT, &objAdminModeValue);
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
* @function fpObjSet_securitySSLConfig_AdminMode
*
* @purpose Set 'AdminMode'
*
* @description [AdminMode]: SSL admin mode 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSLConfig_AdminMode (void *wap, void *bufp)
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
  owa.l7rc = usmDbssltAdminModeSet (L7_UNIT_CURRENT, objAdminModeValue);
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
* @function fpObjGet_securitySSLConfig_SecurePort
*
* @purpose Get 'SecurePort'
*
* @description [SecurePort]: SSL secure port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSLConfig_SecurePort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSecurePortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbssltSecurePortGet (L7_UNIT_CURRENT, &objSecurePortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSecurePortValue, sizeof (objSecurePortValue));

  /* return the object value: SecurePort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSecurePortValue,
                           sizeof (objSecurePortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitySSLConfig_SecurePort
*
* @purpose Set 'SecurePort'
*
* @description [SecurePort]: SSL secure port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSLConfig_SecurePort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSecurePortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SecurePort */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSecurePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSecurePortValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbssltSecurePortSet (L7_UNIT_CURRENT, objSecurePortValue);
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
* @function fpObjGet_securitySSLConfig_ProtocolLevel
*
* @purpose Get 'ProtocolLevel'
*
* @description [ProtocolLevel]: SSL Protocol Level 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSLConfig_ProtocolLevel (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProtocolLevelValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 temp_ssl_val;
  L7_uint32 temp_tls_val;

  owa.l7rc = usmDbssltProtocolLevelGet(L7_UNIT_CURRENT, L7_SSLT_PROTOCOL_SSL30, &temp_ssl_val);

  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbssltProtocolLevelGet(L7_UNIT_CURRENT, L7_SSLT_PROTOCOL_TLS10, &temp_tls_val);
    if (owa.l7rc == L7_SUCCESS)
    {
      if (temp_ssl_val == L7_ENABLE && temp_tls_val == L7_ENABLE)
      {
        objProtocolLevelValue = L7_SSL_PROTO_BOTH;
      }
      else if (temp_ssl_val == L7_ENABLE && temp_tls_val == L7_DISABLE)
      {
        objProtocolLevelValue = L7_SSL_PROTO_SSL30;
      }
      else if (temp_ssl_val == L7_DISABLE && temp_tls_val == L7_ENABLE)
      {
        objProtocolLevelValue = L7_SSL_PROTO_TLS10;
      }
      else if (temp_ssl_val == L7_DISABLE && temp_tls_val == L7_DISABLE)
      {
        objProtocolLevelValue = L7_SSL_PROTO_NONE;
      }
      else
      {
        /* unknown values */
        owa.l7rc = L7_FAILURE;
      }
    }
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objProtocolLevelValue,
                     sizeof (objProtocolLevelValue));

  /* return the object value: ProtocolLevel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objProtocolLevelValue,
                           sizeof (objProtocolLevelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitySSLConfig_ProtocolLevel
*
* @purpose Set 'ProtocolLevel'
*
* @description [ProtocolLevel]: SSL Protocol Level 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSLConfig_ProtocolLevel (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProtocolLevelValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 temp_ssl_val = 0;
  L7_uint32 temp_tls_val = 0;

  /* retrieve object: ProtocolLevel */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objProtocolLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objProtocolLevelValue, owa.len);

  ;

  switch (objProtocolLevelValue)
  {
  case L7_SSL_PROTO_SSL30:
    temp_ssl_val = L7_ENABLE;
    temp_tls_val = L7_DISABLE;
    break;

  case L7_SSL_PROTO_TLS10:
    temp_ssl_val = L7_DISABLE;
    temp_tls_val = L7_ENABLE;
    break;

  case L7_SSL_PROTO_BOTH:
    temp_ssl_val = L7_ENABLE;
    temp_tls_val = L7_ENABLE;
    break;

  case L7_SSL_PROTO_NONE:
    temp_ssl_val = L7_DISABLE;
    temp_tls_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    owa.l7rc = L7_FAILURE;
    break;
  }

  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbssltProtocolLevelSet(L7_UNIT_CURRENT, L7_SSLT_PROTOCOL_SSL30, temp_ssl_val);

    if (owa.l7rc == L7_SUCCESS)
    {
      owa.l7rc = usmDbssltProtocolLevelSet(L7_UNIT_CURRENT, L7_SSLT_PROTOCOL_TLS10, temp_tls_val);
    }
  }

  /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitySSLConfig_MaxSessions
*
* @purpose Get 'MaxSessions'
*
* @description [MaxSessions]: Maximum SSL Sessionis allowed. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSLConfig_MaxSessions (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxSessionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbssltNumSessionsGet ( &objMaxSessionsValue);
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
* @function fpObjSet_securitySSLConfig_MaxSessions
*
* @purpose Set 'MaxSessions'
*
* @description [MaxSessions]: Maximum SSL Sessionis allowed. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSLConfig_MaxSessions (void *wap, void *bufp)
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
  owa.l7rc = usmDbssltNumSessionsSet ( objMaxSessionsValue);
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
* @function fpObjGet_securitySSLConfig_HardTimeout
*
* @purpose Get 'HardTimeout'
*
* @description [HardTimeout]: Hard timeout. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSLConfig_HardTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHardTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbssltSessionHardTimeOutGet ( &objHardTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHardTimeoutValue, sizeof (objHardTimeoutValue));

  /* return the object value: HardTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHardTimeoutValue,
                           sizeof (objHardTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitySSLConfig_HardTimeout
*
* @purpose Set 'HardTimeout'
*
* @description [HardTimeout]: Hard timeout. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSLConfig_HardTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHardTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HardTimeout */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objHardTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHardTimeoutValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbssltSessionHardTimeOutSet ( objHardTimeoutValue);
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
* @function fpObjGet_securitySSLConfig_SoftTimeout
*
* @purpose Get 'SoftTimeout'
*
* @description [SoftTimeout]: Soft timeout 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSLConfig_SoftTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSoftTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbssltSessionSoftTimeOutGet ( &objSoftTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSoftTimeoutValue, sizeof (objSoftTimeoutValue));

  /* return the object value: SoftTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSoftTimeoutValue,
                           sizeof (objSoftTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitySSLConfig_SoftTimeout
*
* @purpose Set 'SoftTimeout'
*
* @description [SoftTimeout]: Soft timeout 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSLConfig_SoftTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSoftTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SoftTimeout */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSoftTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSoftTimeoutValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbssltSessionSoftTimeOutSet ( objSoftTimeoutValue);
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
* @function fpObjGet_securitySSLConfig_CertificatePresent
*
* @purpose Get 'CertificatePresent'
*
* @description [CertificatePresent]: Check to find if SSL certificate is available
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSLConfig_CertificatePresent (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCertificatePresentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if (usmDbssltCertificateExists(1) == L7_SUCCESS)
  {
      objCertificatePresentValue = L7_TRUE;
  }
  else
  {
      objCertificatePresentValue = L7_FALSE;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objCertificatePresentValue,
                     sizeof (objCertificatePresentValue));

  /* return the object value: CertificatePresent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCertificatePresentValue,
                           sizeof (objCertificatePresentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitySSLConfig_CertificateControl
*
* @purpose Get 'CertificateControl'
*
* @description [CertificateControl]: SSL Certificate control 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSLConfig_CertificateControl (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCertificateControlValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  objCertificateControlValue = L7_SSL_NOOP;
  FPOBJ_TRACE_VALUE (bufp, &objCertificateControlValue,
                     sizeof (objCertificateControlValue));

  /* return the object value: CertificateControl */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCertificateControlValue,
                           sizeof (objCertificateControlValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitySSLConfig_CertificateControl
*
* @purpose Set 'CertificateControl'
*
* @description [CertificateControl]: SSL Certificate control 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSLConfig_CertificateControl (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCertificateControlValue;
  xLibU32_t objAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);    
  
  /* retrieve object: CertificateControl */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCertificateControlValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCertificateControlValue, owa.len);
  
  owa.l7rc = usmDbssltAdminModeGet(L7_UNIT_CURRENT, &objAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    objAdminModeValue = L7_DISABLE;
  }
  
  switch (objCertificateControlValue)
  {
  case L7_SSL_GENERATE:
       if (objAdminModeValue == L7_ENABLE)
       {
         owa.rc = XLIBRC_HTTPS_SSL_CERTIFICATE_GENERATE_FAILURE;  
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;  
       }  
    owa.l7rc = usmDbssltCertificateGenerate(1,1024,0,0,0,0,0,0,0,0);
    break;

  case L7_SSL_DELETE:
       if (objAdminModeValue == L7_ENABLE)
       {
         owa.rc = XLIBRC_HTTPS_SSL_CERTIFICATE_DELETE_FAILURE;  
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;  
       }   
    osapiFsDeleteFile(L7_SSLT_SERVER_CERT_PEM_ONE);
    owa.l7rc = osapiFsDeleteFile(L7_SSLT_SERVER_KEY_PEM_ONE);
    break;

  case L7_SSL_NOOP:
    break;

  default:
    /* unknown value */
    owa.l7rc = L7_FAILURE;
    break;
  }

  /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitySSLConfig_CertificateGenerationStatus
*
* @purpose Get 'CertificateGenerationStatus'
*
* @description [CertificateGenerationStatus]: SSL Certificate generation status
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSLConfig_CertificateGenerationStatus (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCertificateGenerationStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */

  if (usmDbssltCertificateExists(1) == L7_FAILURE)
  {
      objCertificateGenerationStatusValue = L7_TRUE;
  }
  else
  {
      objCertificateGenerationStatusValue = L7_FALSE;
  }
	
  FPOBJ_TRACE_VALUE (bufp, &objCertificateGenerationStatusValue,
                     sizeof (objCertificateGenerationStatusValue));

  /* return the object value: CertificateGenerationStatus */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCertificateGenerationStatusValue,
                    sizeof (objCertificateGenerationStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitySSLConfig_SSLVersion
*
* @purpose Get 'SSLVersion'
 *@description  [SSLVersion] <HTML>To enable or disable SSL Version 3
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSLConfig_SSLVersion (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSSLVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbssltProtocolLevelGet(L7_UNIT_CURRENT, L7_SSLT_PROTOCOL_SSL30, &objSSLVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objSSLVersionValue, sizeof (objSSLVersionValue));

  /* return the object value: SSLVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSSLVersionValue, sizeof (objSSLVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitySSLConfig_SSLVersion
*
* @purpose Set 'SSLVersion'
 *@description  [SSLVersion] <HTML>To enable or disable SSL Version 3
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSLConfig_SSLVersion (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSSLVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SSLVersion */
  owa.len = sizeof (objSSLVersionValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSSLVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSSLVersionValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbssltProtocolLevelSet(L7_UNIT_CURRENT, L7_SSLT_PROTOCOL_SSL30, objSSLVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitySSLConfig_TLSVersion
*
* @purpose Get 'TLSVersion'
 *@description  [TLSVersion] <HTML>To set protocol level as TLS Version 1
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSLConfig_TLSVersion (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTLSVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbssltProtocolLevelGet(L7_UNIT_CURRENT, L7_SSLT_PROTOCOL_TLS10, &objTLSVersionValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTLSVersionValue, sizeof (objTLSVersionValue));

  /* return the object value: TLSVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTLSVersionValue, sizeof (objTLSVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitySSLConfig_TLSVersion
*
* @purpose Set 'TLSVersion'
 *@description  [TLSVersion] <HTML>To set protocol level as TLS Version 1
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSLConfig_TLSVersion (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTLSVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TLSVersion */
  owa.len = sizeof (objTLSVersionValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTLSVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTLSVersionValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbssltProtocolLevelSet(L7_UNIT_CURRENT, L7_SSLT_PROTOCOL_TLS10, objTLSVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

