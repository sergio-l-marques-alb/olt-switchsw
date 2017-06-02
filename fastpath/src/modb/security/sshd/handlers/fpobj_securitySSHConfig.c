/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_securitySSHConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ssh-object.xml
*
* @create  14 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_securitySSHConfig_obj.h"
#include "usmdb_sshd_api.h"
#include "sshd_exports.h"


/*******************************************************************************
* @function fpObjGet_securitySSHConfig_AdminMode
*
* @purpose Get 'AdminMode'
*
* @description [AdminMode]: Configures whether the SSH service is enabled
*              on this switch. The default value is disable(2) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_AdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbsshdAdminModeGet (L7_UNIT_CURRENT, &objAdminModeValue);
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
* @function fpObjSet_securitySSHConfig_AdminMode
*
* @purpose Set 'AdminMode'
*
* @description [AdminMode]: Configures whether the SSH service is enabled
*              on this switch. The default value is disable(2) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHConfig_AdminMode (void *wap, void *bufp)
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
  owa.l7rc = usmDbsshdAdminModeSet (L7_UNIT_CURRENT, objAdminModeValue);
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
* @function fpObjGet_securitySSHConfig_ProtocolLevel
*
* @purpose Get 'ProtocolLevel'
*
* @description [ProtocolLevel]: Configures which protocol versions of SSH
*              are enabled on this switch. The default value is both(3). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_ProtocolLevel (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProtocolLevelValue;
  xLibU32_t tempProtocolLevelValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbsshdProtoLevelGet (L7_UNIT_CURRENT, &tempProtocolLevelValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

    switch (tempProtocolLevelValue)
    {
    case L7_SSHD_PROTO_LEVEL_V1_ONLY:
      objProtocolLevelValue = L7_SSH_PROTO_SSH10;
      break;

    case L7_SSHD_PROTO_LEVEL_V2_ONLY:
      objProtocolLevelValue = L7_SSH_PROTO_SSH20;
      break;

    case L7_SSHD_PROTO_LEVEL:
      objProtocolLevelValue = L7_SSH_PROTO_BOTH;
      break;

    default:
      /* unknown value */
      owa.rc = L7_FAILURE;
      break;
    }
     if (owa.l7rc != L7_SUCCESS)
     {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
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
* @function fpObjSet_securitySSHConfig_ProtocolLevel
*
* @purpose Set 'ProtocolLevel'
*
* @description [ProtocolLevel]: Configures which protocol versions of SSH
*              are enabled on this switch. The default value is both(3). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHConfig_ProtocolLevel (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProtocolLevelValue;
  xLibU32_t tempProtocolLevelValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ProtocolLevel */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objProtocolLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objProtocolLevelValue, owa.len);

  switch (objProtocolLevelValue)
  {
    case L7_SSH_PROTO_SSH10:
      tempProtocolLevelValue = L7_SSHD_PROTO_LEVEL_V1_ONLY ;
      break;

    case L7_SSH_PROTO_SSH20:
      tempProtocolLevelValue = L7_SSHD_PROTO_LEVEL_V2_ONLY;
      break;

    case L7_SSH_PROTO_BOTH:
      tempProtocolLevelValue = L7_SSHD_PROTO_LEVEL;
      break;

    default:
      /* unknown value */
      tempProtocolLevelValue = 0;
      owa.l7rc= L7_FAILURE;
      break;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbsshdProtoLevelSet (L7_UNIT_CURRENT, tempProtocolLevelValue);
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
* @function fpObjGet_securitySSHConfig_SessionsCount
*
* @purpose Get 'SessionsCount'
*
* @description [SessionsCount]: Current number of active SSH sessions on this
*              switch. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_SessionsCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSessionsCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbsshdNumSessionsGet (L7_UNIT_CURRENT, &objSessionsCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSessionsCountValue,
                     sizeof (objSessionsCountValue));

  /* return the object value: SessionsCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSessionsCountValue,
                           sizeof (objSessionsCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitySSHConfig_MaxSessionsCount
*
* @purpose Get 'MaxSessionsCount'
*
* @description [MaxSessionsCount]: Max number of SSH sessions permitted on
*              this switch. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_MaxSessionsCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxSessionsCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSshdMaxNumSessionsGet (L7_UNIT_CURRENT, &objMaxSessionsCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxSessionsCountValue,
                     sizeof (objMaxSessionsCountValue));

  /* return the object value: MaxSessionsCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxSessionsCountValue,
                           sizeof (objMaxSessionsCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitySSHConfig_MaxSessionsCount
*
* @purpose Set 'MaxSessionsCount'
*
* @description [MaxSessionsCount]: Max number of SSH sessions permitted on
*              this switch. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHConfig_MaxSessionsCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxSessionsCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MaxSessionsCount */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objMaxSessionsCountValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxSessionsCountValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSshdMaxNumSessionsSet (L7_UNIT_CURRENT, &objMaxSessionsCountValue);
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
* @function fpObjGet_securitySSHConfig_SessionTimeout
*
* @purpose Get 'SessionTimeout'
*
* @description [SessionTimeout]: ssh idle timeout value for this switch im
*              minutes. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_SessionTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSessionTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbsshdTimeoutGet (L7_UNIT_CURRENT, &objSessionTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSessionTimeoutValue,
                     sizeof (objSessionTimeoutValue));

  /* return the object value: SessionTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSessionTimeoutValue,
                           sizeof (objSessionTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitySSHConfig_SessionTimeout
*
* @purpose Set 'SessionTimeout'
*
* @description [SessionTimeout]: ssh idle timeout value for this switch im
*              minutes. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHConfig_SessionTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSessionTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SessionTimeout */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objSessionTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSessionTimeoutValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbsshdTimeoutSet (L7_UNIT_CURRENT, &objSessionTimeoutValue);
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
* @function fpObjGet_securitySSHConfig_KeysPresent
*
* @purpose Get 'KeysPresent'
*
* @description [KeysPresent]: Indicates what key files are present on the
*              device, if any. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_KeysPresent (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objKeysPresentValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_BOOL rsa_present = L7_FALSE;
  L7_BOOL dsa_present = L7_FALSE;

  if (usmDbsshdKeyExists(SSHD_KEY_TYPE_RSA) == L7_SUCCESS)
  {
    rsa_present = L7_TRUE;
  }
  if (usmDbsshdKeyExists(SSHD_KEY_TYPE_DSA) == L7_SUCCESS)
  {
    dsa_present = L7_TRUE;
  }

  if (rsa_present == L7_TRUE && dsa_present == L7_TRUE)
  {
    objKeysPresentValue = L7_SSH_KEY_BOTH;
  }
  else if (rsa_present == L7_TRUE)
  {
    objKeysPresentValue = L7_SSH_KEY_RSA;
  }
  else if (dsa_present == L7_TRUE)
  {
    objKeysPresentValue = L7_SSH_KEY_DSA;
  }
  else
  {
    objKeysPresentValue = L7_SSH_KEY_NONE;
  }
  

  /* get the value from application */

  FPOBJ_TRACE_VALUE (bufp, &objKeysPresentValue, sizeof (objKeysPresentValue));

  /* return the object value: KeysPresent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objKeysPresentValue,
                           sizeof (objKeysPresentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitySSHConfig_GenerationStatus
*
* @purpose Get 'GenerationStatus'
*
* @description [GenerationStatus]: Indicates what key files are currently
*              being generated, if any. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_GenerationStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGenerationStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  L7_BOOL rsa = L7_FALSE;
  L7_BOOL dsa = L7_FALSE;

  if (usmDbsshdKeyExists(SSHD_KEY_TYPE_RSA) == L7_FAILURE)
  {
    rsa = L7_TRUE;
  }
  if (usmDbsshdKeyExists(SSHD_KEY_TYPE_DSA) == L7_FAILURE)
  {
    dsa = L7_TRUE;
  }

  if (rsa == L7_TRUE && dsa == L7_TRUE)
  {
    objGenerationStatusValue = L7_SSH_KEY_BOTH;
  }
  else if (rsa == L7_TRUE)
  {
    objGenerationStatusValue = L7_SSH_KEY_RSA;
  }
  else if (dsa == L7_TRUE)
  {
    objGenerationStatusValue = L7_SSH_KEY_DSA;
  }
  else
  {
    objGenerationStatusValue = L7_SSH_KEY_NONE;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objGenerationStatusValue,
                     sizeof (objGenerationStatusValue));

  /* return the object value: GenerationStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGenerationStatusValue,
                           sizeof (objGenerationStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitySSHConfig_RSAKeyControl
*
* @purpose Get 'RSAKeyControl'
*
* @description [RSAKeyControl]: Controls RSA key generation and deletion.
*              Always returns noop(1). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_RSAKeyControl (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitySSHConfig_RSAKeyControl
*
* @purpose Set 'RSAKeyControl'
*
* @description [RSAKeyControl]: Controls RSA key generation and deletion.
*              Always returns noop(1). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHConfig_RSAKeyControl (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRSAKeyControlValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RSAKeyControl */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRSAKeyControlValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRSAKeyControlValue, owa.len);
	
  switch (objRSAKeyControlValue)
  {
  case L7_SSH_GENERATE:
    owa.l7rc = usmDbsshdKeyGenerate(SSHD_KEY_TYPE_RSA);
    break;

  case L7_SSH_DELETE:
    osapiFsDeleteFile(L7_SSHD_PATH_SERVER_PRIVKEY);
    owa.l7rc = osapiFsDeleteFile(L7_SSHD_PATH_SERVER_PRIVKEY_RSA);
    break;

  case L7_SSH_NOOP:
    break;

  default:
    /* unknown value */
    owa.l7rc = L7_FAILURE;
    break;
  }
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitySSHConfig_DSAKeyControl
*
* @purpose Get 'DSAKeyControl'
*
* @description [DSAKeyControl]: Controls DSA key generation and deletion.
*              Always returns noop(1). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_DSAKeyControl (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitySSHConfig_DSAKeyControl
*
* @purpose Set 'DSAKeyControl'
*
* @description [DSAKeyControl]: Controls DSA key generation and deletion.
*              Always returns noop(1). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHConfig_DSAKeyControl (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDSAKeyControlValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DSAKeyControl */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDSAKeyControlValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDSAKeyControlValue, owa.len);

  /* set the value in application */
  switch (objDSAKeyControlValue)
  {
  case L7_SSH_GENERATE:
    owa.l7rc = usmDbsshdKeyGenerate(SSHD_KEY_TYPE_DSA);
    break;

  case L7_SSH_DELETE:
    owa.l7rc = osapiFsDeleteFile(L7_SSHD_PATH_SERVER_PRIVKEY_DSA);
    break;

  case L7_SSH_NOOP:
    break;

  default:
    /* unknown value */
    owa.l7rc = L7_FAILURE;
    break;
  }
  
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
* @function fpObjGet_securitySSHConfig_SSHv1Mode
*
* @purpose Get 'SSHv1Mode'
*
* @description [SSHv1Mode]: Configures whether the SSH service is enabled
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_SSHv1Mode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbsshdProtoLevelGet(L7_UNIT_CURRENT, &objAdminModeValue);
 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, sizeof (objAdminModeValue));

  if(objAdminModeValue & L7_SSHD_PROTO_LEVEL_V1_ONLY)
    objAdminModeValue = L7_ENABLE;
  else
     objAdminModeValue = L7_DISABLE;

  /* return the object value: AdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminModeValue,
                           sizeof (objAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitySSHConfig_SSHv1Mode
*
* @purpose Set 'SSHv1Mode'
*
* @description [SSHv1Mode]: SSHv1Mode
*            
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHConfig_SSHv1Mode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  xLibU32_t tempObjAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, owa.len);

  owa.l7rc = usmDbsshdProtoLevelGet(L7_UNIT_CURRENT, &tempObjAdminModeValue);
   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  if(objAdminModeValue==L7_ENABLE)
  {
    objAdminModeValue= L7_SSHD_PROTO_LEVEL_V1_ONLY | tempObjAdminModeValue;
  }
  else
  {
   objAdminModeValue = ~L7_SSHD_PROTO_LEVEL_V1_ONLY & tempObjAdminModeValue; 
  }
  /* set the value in application */
  owa.l7rc = usmDbsshdProtoLevelSet (L7_UNIT_CURRENT, objAdminModeValue);
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
* @function fpObjGet_securitySSHConfig_SSHv2Mode
*
* @purpose Get 'SSHv2Mode'
*
* @description [SSHv2Mode]: 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_SSHv2Mode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =  usmDbsshdProtoLevelGet(L7_UNIT_CURRENT, &objAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, sizeof (objAdminModeValue));

  if(objAdminModeValue & L7_SSHD_PROTO_LEVEL_V2_ONLY)
    objAdminModeValue = L7_ENABLE;
  else
    objAdminModeValue = L7_DISABLE;
  /* return the object value: AdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminModeValue,
                           sizeof (objAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_securitySSHConfig_SSHv2Mode
*
* @purpose Set 'SSHv2Mode'
*
* @description [SSHv2Mode]
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHConfig_SSHv2Mode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  xLibU32_t tempObjAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, owa.len);
  owa.l7rc = usmDbsshdProtoLevelGet(L7_UNIT_CURRENT, &tempObjAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objAdminModeValue==L7_ENABLE)
  {
    objAdminModeValue= L7_SSHD_PROTO_LEVEL_V2_ONLY | tempObjAdminModeValue;
  }
  else
  {
   objAdminModeValue = ~L7_SSHD_PROTO_LEVEL_V2_ONLY & tempObjAdminModeValue;
  }

  /* set the value in application */
  owa.l7rc = usmDbsshdProtoLevelSet (L7_UNIT_CURRENT, objAdminModeValue);
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
* @function fpObjGet_securitySSHConfig_DSAKeyPresent
*
* @purpose Get 'DSAKeyPresent'
 *@description  [DSAKeyPresent] <HTML>To check the availability of DSA Key
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_DSAKeyPresent (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDSAKeyPresentValue = L7_SSH_KEY_NONE;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbsshdKeyExists(SSHD_KEY_TYPE_DSA);
  if (owa.l7rc == L7_FAILURE)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (owa.l7rc == L7_SUCCESS)
  {
    objDSAKeyPresentValue = L7_SSH_KEY_DSA;
  }
  else if (owa.l7rc == L7_NOT_EXIST)
  {
    objDSAKeyPresentValue = L7_SSH_KEY_NONE;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDSAKeyPresentValue, sizeof (objDSAKeyPresentValue));

  /* return the object value: DSAKeyPresent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objDSAKeyPresentValue,
                           sizeof (objDSAKeyPresentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitySSHConfig_RSAKeyPresent
*
* @purpose Get 'RSAKeyPresent'
 *@description  [RSAKeyPresent] <HTML>To verify availability of RSA Key
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_RSAKeyPresent (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t)); 
  xLibU32_t objRSAKeyPresentValue = L7_SSH_KEY_NONE;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbsshdKeyExists(SSHD_KEY_TYPE_RSA);
  if (owa.l7rc == L7_FAILURE)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (owa.l7rc == L7_SUCCESS)
  {
    objRSAKeyPresentValue = L7_SSH_KEY_RSA; 
  }
  else if (owa.l7rc == L7_NOT_EXIST)
  {
    objRSAKeyPresentValue = L7_SSH_KEY_NONE;
  }

  FPOBJ_TRACE_VALUE (bufp, &objRSAKeyPresentValue, sizeof (objRSAKeyPresentValue));

  /* return the object value: RSAKeyPresent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objRSAKeyPresentValue,
                           sizeof (objRSAKeyPresentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitySSHConfig_sshSecurePort
*
* @purpose Get 'sshSecurePort'
 *@description  [sshSecurePort] <HTML>The Port Number for the ssh server   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_sshSecurePort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsshSecurePortValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbsshdPortNumGet(&objsshSecurePortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objsshSecurePortValue, sizeof (objsshSecurePortValue));

  /* return the object value: sshSecurePort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsshSecurePortValue,
                           sizeof (objsshSecurePortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitySSHConfig_sshSecurePort
*
* @purpose Set 'sshSecurePort'
 *@description  [sshSecurePort] <HTML>The Port Number for the ssh server   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHConfig_sshSecurePort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsshSecurePortValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sshSecurePort */
  owa.len = sizeof (objsshSecurePortValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsshSecurePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsshSecurePortValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
    owa.l7rc = usmDbsshdPortNumSet(objsshSecurePortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_securitySSHConfig_sshdPubKeyAuthMode
*
* @purpose Get 'sshdPubKeyAuthMode'
 *@description  [sshdPubKeyAuthMode] <HTML>Public-Key Authentication Mode of the
* SSHD server   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_sshdPubKeyAuthMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsshdPubKeyAuthModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbsshdPubKeyAuthModeGet (&objsshdPubKeyAuthModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objsshdPubKeyAuthModeValue, sizeof (objsshdPubKeyAuthModeValue));

  /* return the object value: sshdPubKeyAuthMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsshdPubKeyAuthModeValue,
                           sizeof (objsshdPubKeyAuthModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitySSHConfig_sshdPubKeyAuthMode
*
* @purpose Set 'sshdPubKeyAuthMode'
 *@description  [sshdPubKeyAuthMode] <HTML>Public-Key Authentication Mode of the
* SSHD server   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHConfig_sshdPubKeyAuthMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsshdPubKeyAuthModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sshdPubKeyAuthMode */
  owa.len = sizeof (objsshdPubKeyAuthModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsshdPubKeyAuthModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsshdPubKeyAuthModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbsshdPubKeyAuthModeSet (objsshdPubKeyAuthModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitySSHConfig_DSAsshdFingerPrintHexGet
*
* @purpose Get 'DSAsshdFingerPrintHexGet'
 *@description  [DSAsshdFingerPrintHexGet] <HTML>To get key fingerprint in hex
* format for a specific key type.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_DSAsshdFingerPrintHexGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDSAsshdFingerPrintHexGetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbsshdKeyFingerprintHexGet (SSHD_KEY_TYPE_DSA, objDSAsshdFingerPrintHexGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objDSAsshdFingerPrintHexGetValue,
                     strlen (objDSAsshdFingerPrintHexGetValue));

  /* return the object value: DSAsshdFingerPrintHexGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDSAsshdFingerPrintHexGetValue,
                           strlen (objDSAsshdFingerPrintHexGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitySSHConfig_RSASshdFingerPrintHexGet
*
* @purpose Get 'RSASshdFingerPrintHexGet'
 *@description  [RSASshdFingerPrintHexGet] <HTML>To get key fingerprint in hex
* format for a specific key type for RSA   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_RSASshdFingerPrintHexGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objRSASshdFingerPrintHexGetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbsshdKeyFingerprintHexGet (SSHD_KEY_TYPE_RSA,objRSASshdFingerPrintHexGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objRSASshdFingerPrintHexGetValue,
                     strlen (objRSASshdFingerPrintHexGetValue));

  /* return the object value: RSASshdFingerPrintHexGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objRSASshdFingerPrintHexGetValue,
                           strlen (objRSASshdFingerPrintHexGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitySSHConfig_DSASshdFingerPrintBBGet
*
* @purpose Get 'DSASshdFingerPrintBBGet'
 *@description  [DSASshdFingerPrintBBGet] <HTML>get key fingerprint in bubble
* babble format for a specific key type   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_DSASshdFingerPrintBBGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDSASshdFingerPrintBBGetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbsshdKeyFingerprintBBGet (SSHD_KEY_TYPE_DSA,objDSASshdFingerPrintBBGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objDSASshdFingerPrintBBGetValue,
                     strlen (objDSASshdFingerPrintBBGetValue));

  /* return the object value: DSASshdFingerPrintBBGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDSASshdFingerPrintBBGetValue,
                           strlen (objDSASshdFingerPrintBBGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitySSHConfig_RSASshdFingerPrintBBGet
*
* @purpose Get 'RSASshdFingerPrintBBGet'
 *@description  [RSASshdFingerPrintBBGet] <HTML>get key fingerprint in bubble
* babble format for a specific key type   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_RSASshdFingerPrintBBGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objRSASshdFingerPrintBBGetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbsshdKeyFingerprintBBGet (SSHD_KEY_TYPE_RSA,objRSASshdFingerPrintBBGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objRSASshdFingerPrintBBGetValue,
                     strlen (objRSASshdFingerPrintBBGetValue));

  /* return the object value: RSASshdFingerPrintBBGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objRSASshdFingerPrintBBGetValue,
                           strlen (objRSASshdFingerPrintBBGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_securitySSHConfig_DSASshdPubKeyChainUserKeyAdd
*
* @purpose Set 'DSASshdPubKeyChainUserKeyAdd'
 *@description  [DSASshdPubKeyChainUserKeyAdd] <HTML>Add public key
* configuration for remote user/device   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHConfig_DSASshdPubKeyChainUserKeyAdd (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDSASshdPubKeyChainUserKeyAddValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DSASshdPubKeyChainUserKeyAdd */
  owa.len = sizeof (objDSASshdPubKeyChainUserKeyAddValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDSASshdPubKeyChainUserKeyAddValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDSASshdPubKeyChainUserKeyAddValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc =  usmDbsshdPubKeyChainUserKeyAdd (objDSASshdPubKeyChainUserKeyAddValue,SSHD_KEY_TYPE_DSA);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_securitySSHConfig_DSASshdPubKeyChainUserKeyRemove
*
* @purpose Set 'DSASshdPubKeyChainUserKeyRemove'
 *@description  [DSASshdPubKeyChainUserKeyRemove] <HTML>Add public key
* configuration for remote user/device   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHConfig_DSASshdPubKeyChainUserKeyRemove (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDSASshdPubKeyChainUserKeyRemoveValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DSASshdPubKeyChainUserKeyRemove */
  owa.len = sizeof (objDSASshdPubKeyChainUserKeyRemoveValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDSASshdPubKeyChainUserKeyRemoveValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDSASshdPubKeyChainUserKeyRemoveValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbsshdPubKeyChainUserKeyRemove (objDSASshdPubKeyChainUserKeyRemoveValue,SSHD_KEY_TYPE_DSA);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_securitySSHConfig_RSASshdPubKeyChainUserKeyAdd
*
* @purpose Set 'RSASshdPubKeyChainUserKeyAdd'
 *@description  [RSASshdPubKeyChainUserKeyAdd] <HTML>Add public key
* configuration for remote user/device   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHConfig_RSASshdPubKeyChainUserKeyAdd (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objRSASshdPubKeyChainUserKeyAddValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RSASshdPubKeyChainUserKeyAdd */
  owa.len = sizeof (objRSASshdPubKeyChainUserKeyAddValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objRSASshdPubKeyChainUserKeyAddValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objRSASshdPubKeyChainUserKeyAddValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbsshdPubKeyChainUserKeyAdd (objRSASshdPubKeyChainUserKeyAddValue,SSHD_KEY_TYPE_RSA);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_securitySSHConfig_RSASshdPubKeyChainUserKeyRemove
*
* @purpose Set 'RSASshdPubKeyChainUserKeyRemove'
 *@description  [RSASshdPubKeyChainUserKeyRemove] <HTML>Add public key
* configuration for remote user/device   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHConfig_RSASshdPubKeyChainUserKeyRemove (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objRSASshdPubKeyChainUserKeyRemoveValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RSASshdPubKeyChainUserKeyRemove */
  owa.len = sizeof (objRSASshdPubKeyChainUserKeyRemoveValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objRSASshdPubKeyChainUserKeyRemoveValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objRSASshdPubKeyChainUserKeyRemoveValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbsshdPubKeyChainUserKeyRemove (objRSASshdPubKeyChainUserKeyRemoveValue,SSHD_KEY_TYPE_RSA);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitySSHConfig_SshdChainUserGet
*
* @purpose Get 'SshdChainUserGet'
 *@description  [SshdChainUserGet] <HTML>Get next public key User name
* configured for remote user/device   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_SshdChainUserGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objSshdChainUserGetValue;
  xLibStr256_t nextObjSshdChainUserGetValue;
  
  sshdKeyType_t dummyType;

  FPOBJ_TRACE_ENTER (bufp);
  
  memset(objSshdChainUserGetValue,0x00,sizeof(objSshdChainUserGetValue));
  memset(nextObjSshdChainUserGetValue,0x00,sizeof(nextObjSshdChainUserGetValue));

  /* retrieve key: SshdChainUserGet */
  /* All this confusing logic is made just becuase the applcation doesnt have a usmdb 
   * pattern of get and get next
   */
  owa.len = sizeof (objSshdChainUserGetValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitySSHConfig_SshdChainUserGet,
                          (xLibU8_t *) objSshdChainUserGetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbsshdPubKeyChainUserKeyNextGet (nextObjSshdChainUserGetValue,&dummyType);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objSshdChainUserGetValue, owa.len);
    strcpy(nextObjSshdChainUserGetValue,objSshdChainUserGetValue);
    owa.l7rc = usmDbsshdPubKeyChainUserKeyNextGet (nextObjSshdChainUserGetValue,&dummyType);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjSshdChainUserGetValue, owa.len);

  /* return the object value: SshdChainUserGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjSshdChainUserGetValue,
                           strlen (nextObjSshdChainUserGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitySSHConfig_sshdChainUserKeyTypeGet
*
* @purpose Get 'sshdChainUserKeyTypeGet'
 *@description  [sshdChainUserKeyTypeGet] <HTML>Get next public key configured
* for remote user/device   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_sshdChainUserKeyTypeGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsshdChainUserKeyTypeGetValue;
  xLibU32_t nextObjsshdChainUserKeyTypeGetValue;

  xLibStr256_t keySshdChainUserGetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SshdChainUserGet */
  owa.len = sizeof (keySshdChainUserGetValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitySSHConfig_SshdChainUserGet,
                          (xLibU8_t *) keySshdChainUserGetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keySshdChainUserGetValue, owa.len);

  /* retrieve key: sshdChainUserKeyTypeGet */
  owa.len = sizeof (objsshdChainUserKeyTypeGetValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitySSHConfig_sshdChainUserKeyTypeGet,
                          (xLibU8_t *) & objsshdChainUserKeyTypeGetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbsshdPubKeyChainUserKeyNextGet (keySshdChainUserGetValue,
                            &nextObjsshdChainUserKeyTypeGetValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objsshdChainUserKeyTypeGetValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjsshdChainUserKeyTypeGetValue, owa.len);

  /* return the object value: sshdChainUserKeyTypeGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjsshdChainUserKeyTypeGetValue,
                           sizeof (nextObjsshdChainUserKeyTypeGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitySSHConfig_sshdChainUserKeyDataGet
*
* @purpose Get 'sshdChainUserKeyDataGet'
 *@description  [sshdChainUserKeyDataGet] <HTML>Get public key string for remote
* user/device   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_sshdChainUserKeyDataGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsshdChainUserKeyDataGetValue;

  xLibStr256_t keySshdChainUserGetValue;
  xLibU32_t keysshdChainUserKeyTypeGetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SshdChainUserGet */
  owa.len = sizeof (keySshdChainUserGetValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitySSHConfig_SshdChainUserGet,
                          (xLibU8_t *) keySshdChainUserGetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keySshdChainUserGetValue, owa.len);

  /* retrieve key: sshdChainUserKeyTypeGet */
  owa.len = sizeof (keysshdChainUserKeyTypeGetValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitySSHConfig_sshdChainUserKeyTypeGet,
                          (xLibU8_t *) & keysshdChainUserKeyTypeGetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysshdChainUserKeyTypeGetValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbsshdPubKeyChainUserKeyDataGet (keySshdChainUserGetValue,
                              keysshdChainUserKeyTypeGetValue, objsshdChainUserKeyDataGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objsshdChainUserKeyDataGetValue,
                     strlen (objsshdChainUserKeyDataGetValue));

  /* return the object value: sshdChainUserKeyDataGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsshdChainUserKeyDataGetValue,
                           strlen (objsshdChainUserKeyDataGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitySSHConfig_sshdPubFingerPrintBBGet
*
* @purpose Get 'sshdPubFingerPrintBBGet'
 *@description  [sshdPubFingerPrintBBGet] <HTML>Get public key string for remote
* user/device   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_sshdPubFingerPrintBBGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsshdPubFingerPrintBBGetValue;

  xLibStr256_t keySshdChainUserGetValue;
  xLibU32_t keysshdChainUserKeyTypeGetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SshdChainUserGet */
  owa.len = sizeof (keySshdChainUserGetValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitySSHConfig_SshdChainUserGet,
                          (xLibU8_t *) keySshdChainUserGetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keySshdChainUserGetValue, owa.len);

  /* retrieve key: sshdChainUserKeyTypeGet */
  owa.len = sizeof (keysshdChainUserKeyTypeGetValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitySSHConfig_sshdChainUserKeyTypeGet,
                          (xLibU8_t *) & keysshdChainUserKeyTypeGetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysshdChainUserKeyTypeGetValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbsshdPubKeyChainUserKeyFingerprintBBGet (keySshdChainUserGetValue,
                              keysshdChainUserKeyTypeGetValue, objsshdPubFingerPrintBBGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objsshdPubFingerPrintBBGetValue,
                     strlen (objsshdPubFingerPrintBBGetValue));

  /* return the object value: sshdPubFingerPrintBBGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsshdPubFingerPrintBBGetValue,
                           strlen (objsshdPubFingerPrintBBGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitySSHConfig_sshdPubFingerPrintHexGet
*
* @purpose Get 'sshdPubFingerPrintHexGet'
 *@description  [sshdPubFingerPrintHexGet] <HTML>Get public key string for
* remote user/device   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHConfig_sshdPubFingerPrintHexGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsshdPubFingerPrintHexGetValue;

  xLibStr256_t keySshdChainUserGetValue;
  xLibU32_t keysshdChainUserKeyTypeGetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SshdChainUserGet */
  owa.len = sizeof (keySshdChainUserGetValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitySSHConfig_SshdChainUserGet,
                          (xLibU8_t *) keySshdChainUserGetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keySshdChainUserGetValue, owa.len);

  /* retrieve key: sshdChainUserKeyTypeGet */
  owa.len = sizeof (keysshdChainUserKeyTypeGetValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitySSHConfig_sshdChainUserKeyTypeGet,
                          (xLibU8_t *) & keysshdChainUserKeyTypeGetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysshdChainUserKeyTypeGetValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbsshdPubKeyChainUserKeyFingerprintHexGet (keySshdChainUserGetValue,
                              keysshdChainUserKeyTypeGetValue, objsshdPubFingerPrintHexGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objsshdPubFingerPrintHexGetValue,
                     strlen (objsshdPubFingerPrintHexGetValue));

  /* return the object value: sshdPubFingerPrintHexGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsshdPubFingerPrintHexGetValue,
                           strlen (objsshdPubFingerPrintHexGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
