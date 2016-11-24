/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename sslt_api.c
*
* @purpose SSL Tunnel API functions
*
* @component sslt
*
* @comments none
*
* @create 07/15/2003
*
* @author spetriccione
*
* @end
*             
**********************************************************************/
#include "sslt_include.h"
#include "openssl_api.h"
#include "sslt_exports.h"
#include "transfer_exports.h"
#include "simapi.h"

extern ssltCfg_t *ssltCfg;
extern ssltGlobal_t ssltGlobal;
extern SSL_CTX *ctx;
extern ssltCnfgrState_t ssltCnfgrState;

extern void *ssltCertSema;

/*********************************************************************
*
* @purpose Set the Admin Mode of the SSL Tunnel server.
*
* @param mode @b{(input)} value of new Admin mode setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltAdminModeSet(L7_uint32 mode)
{
  L7_char8 certFile[L7_MAX_FILENAME];

  if ((mode != L7_ENABLE) &&
      (mode != L7_DISABLE))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);
    if (mode != ssltCfg->cfg.ssltAdminMode)
    {
    if ((mode == L7_ENABLE) &&
        (ssltServerCertFileGet(ssltCfg->cfg.ssltCertificateNumber,
                               certFile) != L7_SUCCESS))
    {
      osapiSemaGive(ssltCertSema);
      return L7_FAILURE;
    }
    /* issue the command */
    if ((ssltIssueCmd(SSLT_ADMIN_MODE_SET, &mode)) != L7_SUCCESS)
    {
      osapiSemaGive(ssltCertSema);
        return L7_FAILURE;
      }

      ssltCfg->cfg.ssltAdminMode = mode;
    ssltCfg->hdr.dataChanged = L7_TRUE;
  }

  osapiSemaGive(ssltCertSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Admin Mode of the SSL Tunnel server.
*
* @param mode @b{(input)} location to store the Admin mode setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltAdminModeGet(L7_uint32 *mode)
{
  if (SSLT_IS_READY)
  {
    *mode = ssltCfg->cfg.ssltAdminMode;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Set the Auxiliary Mode of the SSL Tunnel server.
*
* @param mode @b{(input)} value of new Aux mode setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltAuxModeSet(L7_uint32 mode)
{
  L7_char8 certFile[L7_MAX_FILENAME];

  if ((mode != L7_ENABLE) &&
      (mode != L7_DISABLE))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);
  if (mode != ssltGlobal.ssltAuxOperMode)
  {
    if ((mode == L7_ENABLE) &&
        (ssltServerCertFileGet(L7_SSLT_CERT_NUMBER_ACTIVE,
                               certFile) != L7_SUCCESS))
    {
      osapiSemaGive(ssltCertSema);
      return L7_FAILURE;
    }
    /* issue the command */
    if ((ssltIssueCmd(SSLT_AUX_MODE_SET, &mode)) != L7_SUCCESS)
    {
      osapiSemaGive(ssltCertSema);
      return L7_FAILURE;
    }
  }

  osapiSemaGive(ssltCertSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Auxilary Mode of the SSL Tunnel server.
*
* @param mode @b{(input)} location to store the Aux mode setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltAuxModeGet(L7_uint32 *mode)
{
  if (SSLT_IS_READY)
  {
    *mode = ssltGlobal.ssltAuxOperMode;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Set the secure port number to listen on for SSLT connections.
*
* @param number @b{(input)} value of the new port setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltSecurePortSet(L7_uint32 number)
{
  L7_RC_t rc;

  if ((number >= L7_SSLT_SECURE_PORT_MIN) && (number <= L7_SSLT_SECURE_PORT_MAX))
  {
    /* Only allow a change if this new port is different than the one
    ** we are currently operating with. 
    */
    if (number != ssltCfg->cfg.ssltSecurePort)
    {
      /* issue the command */
      if ((rc = ssltIssueCmd(SSLT_SECURE_PORT_SET, &number)) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      ssltCfg->cfg.ssltSecurePort = number;

      ssltCfg->hdr.dataChanged = L7_TRUE;
    }
  }
  else
  {
    /* Port number is not in the valid range */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the secure port number that the SSLT connection is using.
*
* @param number @b{(input)} location to store the value of the port number
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltSecurePortGet(L7_uint32 *number)
{

  *number = ssltCfg->cfg.ssltSecurePort;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the first auxiliary port number to listen on for SSLT connections.
*
* @param number @b{(input)} value of the new port setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltAuxSecurePort1Set(L7_uint32 number)
{
  L7_RC_t rc;

  if ((number >= 0) && (number <= L7_SSLT_SECURE_PORT_MAX))
  {
    /* Only allow a change if this new port is different than the one
    ** we are currently operating with.
    */
    if (number != ssltGlobal.ssltAuxSecurePort1)
    {
      /* issue the command */
      if ((rc = ssltIssueCmd(SSLT_AUX_SECURE_PORT1_SET, &number)) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
  }
  else
  {
    /* Port number is not in the valid range */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the first auxiliary port number that the SSLT connection is using.
*
* @param number @b{(input)} location to store the value of the port number
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltAuxSecurePort1Get(L7_uint32 *number)
{
  *number = ssltGlobal.ssltAuxSecurePort1;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the second auxiliary port number to listen on for SSLT connections.
*
* @param number @b{(input)} value of the new port setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltAuxSecurePort2Set(L7_uint32 number)
{
  L7_RC_t rc;

  if ((number >= 0) && (number <= L7_SSLT_SECURE_PORT_MAX))
  {
    /* Only allow a change if this new port is different than the one
    ** we are currently operating with.
    */
    if (number != ssltGlobal.ssltAuxSecurePort2)
    {
      /* issue the command */
      if ((rc = ssltIssueCmd(SSLT_AUX_SECURE_PORT2_SET, &number)) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
  }
  else
  {
    /* Port number is not in the valid range */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the second auxiliary port number that the SSLT connection is using.
*
* @param number @b{(input)} location to store the value of the port number
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltAuxSecurePort2Get(L7_uint32 *number)
{
  *number = ssltGlobal.ssltAuxSecurePort2;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the unsecure port number to open for SSLT connections.
*
* @param number @b{(input)} value of the new port setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltUnSecurePortSet(L7_uint32 number)
{
  L7_RC_t rc;

  if ((number >= L7_SSLT_UNSECURE_PORT_MIN) && (number <= L7_SSLT_UNSECURE_PORT_MAX))
  {
    /* Only allow a change if this new port is different than the one
    ** that SSLT is currently configured to use. 
    */
    if (number != ssltCfg->cfg.ssltUnSecurePort)
    {
      /* issue the command */
      if ((rc = ssltIssueCmd(SSLT_UNSECURE_PORT_SET, &number)) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      ssltCfg->cfg.ssltUnSecurePort = number;

      ssltCfg->hdr.dataChanged = L7_TRUE;
    }
  }
  else
  {
    /* Port number is not in the valid range */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the unsecure port number that the SSLT connection is using.
*
* @param number @b{(input)} location to store the value of the port number
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltUnSecurePortGet(L7_uint32 *number)
{

  *number = ssltCfg->cfg.ssltUnSecurePort;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the unsecure server IP address to open for SSLT connections.
*
* @param number @b{(input)} value of the new server IP address
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltUnSecureServerAddrSet(L7_uint32 number)
{
  L7_RC_t rc;

  /* Only allow a change if this new server address is different than 
  ** the one that SSLT is currently configured to use. 
  */
  if (number != ssltCfg->cfg.ssltUnSecureServerAddr)
  {
    /* issue the command */
    if ((rc = ssltIssueCmd(SSLT_UNSECURE_SERVER_ADDR_SET, &number)) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    ssltCfg->cfg.ssltUnSecureServerAddr = number;

    ssltCfg->hdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the unsecure server IP address that the SSLT connection 
*          is using.
*
* @param number @b{(input)} location to store the value of the server 
*                           IP address 
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltUnSecureServerAddrGet(L7_uint32 *number)
{

  *number = ssltCfg->cfg.ssltUnSecureServerAddr;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the Pass Phrase string used to access the PEM-encoded 
*          (Privacy Enhanced Mail) certificates and key files required 
*          for the SSLT connections.
*
* @param string @b{(input)} pointer to the new PEM Pass Phrase string
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltPemPassPhraseSet(L7_uchar8 *string)
{
  L7_RC_t rc;

  if (string == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (strlen(string) < L7_SSLT_PEM_PASS_PHRASE_MAX)
  {
    /* Only allow a change if this new PEM Pass Phrase is different 
    ** than the one that SSLT is currently configured to use. 
    */
    if (strcmp(string, ssltCfg->cfg.ssltPemPassPhrase) != L7_NULL)
    {
      /* issue the command */
      if ((rc = ssltIssueCmd(SSLT_PEM_PASS_PHRASE_SET, (void *)string)) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      osapiStrncpySafe(ssltCfg->cfg.ssltPemPassPhrase, string, sizeof(ssltCfg->cfg.ssltPemPassPhrase));

      ssltCfg->hdr.dataChanged = L7_TRUE;
    }
  }
  else
  {
    /* string is longer than allowed */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Find out if the PEM Pass Phrase has been set or not
*
* @param phraseIsSet @b{(input)} location to set with boolean result
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltPemPassPhraseCheck(L7_BOOL *phraseIsSet)
{

  *phraseIsSet = (strcmp(ssltCfg->cfg.ssltPemPassPhrase, "") ? L7_TRUE : L7_FALSE);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the SSL Protocol Level to be used by SSL Tunnel code
*
* @param protocolId @b{(input)} ID of SSLT supported protocol
* @param mode @b{(input)} value of new mode setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltProtocolLevelSet(L7_uint32 protocolId, 
                             L7_uint32 mode)
{
  L7_RC_t rc;

  if ((mode == L7_ENABLE) || (mode == L7_DISABLE))
  {
    switch (protocolId)
    {
    case L7_SSLT_PROTOCOL_SSL30:
      if (mode != ssltCfg->cfg.ssltProtocolSSL30)
      {
        /* issue the command */
        if ((rc = ssltIssueCmd(SSLT_PROTOCOL_SSL30_SET, &mode)) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }

        ssltCfg->cfg.ssltProtocolSSL30 = mode;
      }
      break;
    case L7_SSLT_PROTOCOL_TLS10:
      if (mode != ssltCfg->cfg.ssltProtocolTLS10)
      {
        /* issue the command */
        if ((rc = ssltIssueCmd(SSLT_PROTOCOL_TLS10_SET, &mode)) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }

        ssltCfg->cfg.ssltProtocolTLS10 = mode;
      }
      break;
    default: 
      /* Protocol is not in the valid range */
      return L7_FAILURE;
    }

    ssltCfg->hdr.dataChanged = L7_TRUE;

  }
  else
  {
    /* Mode is not in the valid range */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the SSL Protocol Level in use by the SSL Tunnel code
*
* @param protocolId @b{(input)} ID of SSLT supported protocol
* @param mode @b{(input)} location to store the mode setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltProtocolLevelGet(L7_uint32 protocolId,
                             L7_uint32 *mode)
{
  switch (protocolId)
  {
  case L7_SSLT_PROTOCOL_SSL30:
    *mode = ssltCfg->cfg.ssltProtocolSSL30;

    break;

  case L7_SSLT_PROTOCOL_TLS10:
    *mode = ssltCfg->cfg.ssltProtocolTLS10;

    break;

  default:
    /* Protocol is not in the valid range */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Set the Hard Timeout (in hours) for SSLT sessions.
*
* @param number @b{(input)} value of the new timeout setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltSessionHardTimeOutSet(L7_uint32 val)
{
  if ((val >= FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_MIN) && (val <= FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_MAX))
  {
    /* Not necessary to send to the SSLT message queue as this parameter is only
       used by the emweb session code
    */
    if (val != ssltCfg->cfg.ssltSessionHardTimeOut)
    {
      ssltCfg->cfg.ssltSessionHardTimeOut = val;
      ssltCfg->hdr.dataChanged = L7_TRUE;
    }
  }
  else
  {
    /* Timeout is not in the valid range */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Hard Timeout (in hours) for SSLT sessions.
*
* @param val @b{(input)} location to store the value of the timeout
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltSessionHardTimeOutGet(L7_uint32 *val)
{

  *val = ssltCfg->cfg.ssltSessionHardTimeOut;

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Set the Soft Timeout (in minutes) for SSLT sessions.
*
* @param number @b{(input)} value of the new timeout setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltSessionSoftTimeOutSet(L7_uint32 val)
{
  if ((val >= FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_MIN) && (val <= FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_MAX))
  {
    /* Not necessary to send to the SSLT message queue as this parameter is only
       used by the emweb session code
    */
    if (val != ssltCfg->cfg.ssltSessionSoftTimeOut)
    {
      ssltCfg->cfg.ssltSessionSoftTimeOut = val;
      ssltCfg->hdr.dataChanged = L7_TRUE;
    }
  }
  else
  {
    /* Timeout is not in the valid range */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Soft Timeout (in minutes) for SSLT sessions.
*
* @param val @b{(input)} location to store the value of the timeout
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltSessionSoftTimeOutGet(L7_uint32 *val)
{

  *val = ssltCfg->cfg.ssltSessionSoftTimeOut;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the number of allowable SSLT sessions.
*
* @param number @b{(input)} value of the number of sslt sessions
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltNumSessionsSet(L7_uint32 val)
{
  if ((val >= 0) && (val <= FD_SECURE_HTTP_DEFAULT_MAX_CONNECTIONS))
  {
    /* Not necessary to send to the SSLT message queue as this parameter is only
       used by the emweb session code
    */
    if (val != ssltCfg->cfg.ssltNumSessions)
    {
      ssltCfg->cfg.ssltNumSessions = val;
      ssltCfg->hdr.dataChanged = L7_TRUE;
    }
  }
  else
  {
    /* Timeout is not in the valid range */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the number of allowable SSLT sessions.
*
* @param val @b{(input)} location to store the value of the number
*                        of sessions
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltNumSessionsGet(L7_uint32 *val)
{

  *val = ssltCfg->cfg.ssltNumSessions;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Determine if self-signed server certificate exists
*
* @param L7_uint32  number     @b{(input)} certificate number (1-2)
*
* @returns L7_SUCCESS, certificate and RSA key pair exists
* @returns L7_FAILURE  if generation in progress
* @returns L7_NOT_EXIST  if cert not present
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltCertificateExists(L7_uint32 number)
{
  L7_char8 certFile[L7_MAX_FILENAME];

  if ((number < L7_SSLT_CERT_NUMBER_MIN) ||
      (number > L7_SSLT_CERT_NUMBER_MAX))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);
  if (ssltGlobal.ssltCertGenerateFlag != 0)
  {
    /* certificate generation requested and in progress */
    osapiSemaGive(ssltCertSema);
    return L7_FAILURE;
  }
  if (ssltServerCertFileGet(number, certFile) != L7_SUCCESS)
  {
    osapiSemaGive(ssltCertSema);
    return L7_NOT_EXIST;
  }

  osapiSemaGive(ssltCertSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Determine if Diffie Hellman parameters have been generated
*
* @param void
*
* @returns L7_SUCCESS, params exist and are loaded
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltDHParametersExist(void)
{
  L7_RC_t rc = L7_FAILURE;

  osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);
  rc = ssltDiffieHellmanParamsExist();
  osapiSemaGive(ssltCertSema);
  return rc;
}

/*********************************************************************
*
* @purpose Generate a self-signed server certificate for SSL tunnel
*
* @param L7_uint32  number     @b{(input)} certificate number
* @param L7_uint32  keyLength  @b{(input)} RSA key length, 0 will use existing key.
* @param L7_char8  *commonName @b((input)) subject DN common name field
 * @param L7_char8  *orgName    @b((input)) subject DN organization name field
* @param L7_char8  *orgUnit    @b((input)) subject DN organization unit field
* @param L7_char8  *location   @b((input)) subject DN location field
* @param L7_char8  *state      @b((input)) subject DN state field
* @param L7_char8  *country    @b((input)) subject DN country field
* @param L7_char8  *email      @b((input)) subject DN email field
* @param L7_uint32  days       @b((input)) days certificate will be valid
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments  If certificate does not exist, a keyLength must be specified
*             to generate an RSA key pair on the device.
*            If keyLength == 0, certificate will use existing RSA key pair.
*            If specified, keyLength must be >= L7_SSLT_KEY_LENGTH_MIN and
*                                           <= L7_SSLT_KEY_LENGTH_MAX.
*            If DN fields are not specified, defaults will be applied.
*            DN field lengths must be >= L7_SSLT_SUBJECT_DN_FIELD_MIN and
*                                     <= L7_SSLT_SUBJECT_DN_FIELD_MAX.
*            country length must be L7_SSLT_SUBJECT_COUNTRY_SIZE.
*            If days == 0, default valid days will be applied.
*
* @end
*
*********************************************************************/
L7_RC_t ssltCertificateGenerate(L7_uint32 number,
                                L7_uint32 keyLength,
                                L7_char8 *commonName,
                                 L7_char8 *orgName,
                                L7_char8 *orgUnit,
                                L7_char8 *location,
                                L7_char8 *state,
                                L7_char8 *country,
                                 L7_char8 *email,
                                L7_uint32 days)
{
  L7_uint32 ip = 0;
  L7_char8 str[SSLT_ADDRESS_BUFFER_MAX];

  /* check all the parameters */
  if ((number < L7_SSLT_CERT_NUMBER_MIN) ||
      (number > L7_SSLT_CERT_NUMBER_MAX))
  {
    return L7_FAILURE;
  }
  if ((commonName != L7_NULLPTR) &&
      ((strlen(commonName) < L7_SSLT_SUBJECT_DN_FIELD_MIN) ||
       (strlen(commonName) > L7_SSLT_SUBJECT_DN_FIELD_MAX)))
  {
    return L7_FAILURE;
  }
  if ((orgName != L7_NULLPTR) &&
      ((strlen(orgName) < L7_SSLT_SUBJECT_DN_FIELD_MIN) ||
       (strlen(orgName) > L7_SSLT_SUBJECT_DN_FIELD_MAX)))
  {
    return L7_FAILURE;
  }
  if ((orgUnit != L7_NULLPTR) &&
      ((strlen(orgUnit) < L7_SSLT_SUBJECT_DN_FIELD_MIN) ||
       (strlen(orgUnit) > L7_SSLT_SUBJECT_DN_FIELD_MAX)))
  {
    return L7_FAILURE;
  }
  if ((location != L7_NULLPTR) &&
      ((strlen(location) < L7_SSLT_SUBJECT_DN_FIELD_MIN) ||
       (strlen(location) > L7_SSLT_SUBJECT_DN_FIELD_MAX)))
  {
    return L7_FAILURE;
  }
  if ((state != L7_NULLPTR) &&
      ((strlen(state) < L7_SSLT_SUBJECT_DN_FIELD_MIN) ||
       (strlen(state) > L7_SSLT_SUBJECT_DN_FIELD_MAX)))
  {
    return L7_FAILURE;
  }
  if ((country != L7_NULLPTR) &&
      (strlen(country) != L7_SSLT_SUBJECT_DN_COUNTRY_SIZE))
  {
    return L7_FAILURE;
  }
  if ((email != L7_NULLPTR) &&
      ((strlen(email) < L7_SSLT_SUBJECT_DN_FIELD_MIN) ||
       (strlen(email) > L7_SSLT_SUBJECT_DN_FIELD_MAX)))
  {
    return L7_FAILURE;
  }
  if ((days != 0) &&
      ((days < L7_SSLT_CERT_VALID_DAYS_MIN) ||
       (days > L7_SSLT_CERT_VALID_DAYS_MAX)))
  {
    return L7_FAILURE;
  }
  if ((keyLength != 0) &&
      ((keyLength < L7_SSLT_KEY_LENGTH_MIN) ||
       (keyLength > L7_SSLT_KEY_LENGTH_MAX)))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);
  if (ssltGlobal.ssltCertGenerateFlag != 0)
  {
    /* certificate generation requested and in progress */
    osapiSemaGive(ssltCertSema);
    return L7_FAILURE;
  }
  if (keyLength == 0)
  {
    L7_char8 keyFile[L7_MAX_FILENAME];
    if (ssltServerKeyFileGet(number, keyFile) != L7_SUCCESS)
    {
      osapiSemaGive(ssltCertSema);
      return L7_FAILURE;
    }
  }

  ssltGlobal.ssltCertGenerateFlag = 1;

  /* apply defaults if needed */
  if (days == 0)
  {
    days = FD_SSLT_CERT_VALID_DAYS;
  }
  if (commonName == L7_NULLPTR)
  {
    /* use ip address of the device, when there is more
       than one ip address for the device this should
       default to the lowest ip address */
    ip = simGetSystemIPAddr();
    osapiStrncpySafe(str, osapiInet_ntoa(ip), sizeof(str));
    commonName = str;
  }

  osapiSemaGive(ssltCertSema);

  if (ssltCertGenerate(number,
                       keyLength,
                       commonName,
                       orgName,
                       orgUnit,
                       location,
                       state,
                       country,
                       email,
                       days) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}




/*********************************************************************
*
* @purpose Set the active certificate for the SSL tunnel
*
* @param L7_uint32  number     @b{(input)} certificate number (1-2)
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE, certificate does not exist
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltCertificateActiveSet(L7_uint32 number)
{
  L7_char8 certFile[L7_MAX_FILENAME];


  if ((number < L7_SSLT_CERT_NUMBER_MIN) || 
      (number > L7_SSLT_CERT_NUMBER_MAX))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);  
  if (ssltServerCertFileGet(number, certFile) != L7_SUCCESS)
  {
    osapiSemaGive(ssltCertSema);  
    return L7_FAILURE;
  }
  if (ssltCfg->cfg.ssltCertificateNumber == number)
  {
    /* no change */
    osapiSemaGive(ssltCertSema);  
    return L7_SUCCESS;
  }

  if (ssltIssueCmd(SSLT_CERTIFICATE_ACTIVE_SET, &number) != L7_SUCCESS)
  {
    osapiSemaGive(ssltCertSema);  
    return L7_FAILURE;
  }
  ssltCfg->cfg.ssltCertificateNumber = number;
  ssltCfg->hdr.dataChanged = L7_TRUE;
  osapiSemaGive(ssltCertSema);  
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the active certificate for the SSL tunnel
*
* @param L7_uint32  *number     @b{(output)} certificate number (1-2)
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE, no certificates exist
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltCertificateActiveGet(L7_uint32 *number)
{
  osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);  
  *number = ssltCfg->cfg.ssltCertificateNumber;
  osapiSemaGive(ssltCertSema);  
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Import a signed certificate for SSL tunnel
*
* @param L7_uint32  number      @b{(input)} certificate number
* @param L7_char8  *cert        @b((input)) signed cert X509 PEM format
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE, failed to import certificate
*
* @comments  This will replace self-signed certificate (number),
*             use usmDbssltCertificateGet to read certificate info.
*            If RSA key does not match key for certificate (number),
*             import will fail.
*            cert buffer cannot be larger than L7_SSLT_PEM_FILE_SIZE_MAX
*       
* @end
*
*********************************************************************/
L7_RC_t ssltCertificateImport(L7_uint32  number,
                              L7_char8  *cert)
{
  L7_char8 certFile[L7_MAX_FILENAME];

  if ((number < L7_SSLT_CERT_NUMBER_MIN) || 
      (number > L7_SSLT_CERT_NUMBER_MAX) ||
      (cert == L7_NULLPTR) ||
      (strlen(cert) > L7_SSLT_PEM_FILE_SIZE_MAX))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);
  /* we can only import a certificate, if we have an existing
     self-signed certificate with a key pair */
  if (ssltServerCertFileGet(number, certFile) != L7_SUCCESS)
  {
    osapiSemaGive(ssltCertSema);
    return L7_FAILURE;
  }

  if (ssltServerCredentialsImport(number, cert) != L7_SUCCESS)
  {
    osapiSemaGive(ssltCertSema);
    return L7_FAILURE;
  }

  /* If this is the active certificate need to reload the
     certificate, this is the same as setting to active */
  if ((ssltCfg->cfg.ssltAdminMode == L7_ENABLE) &&
      (ssltCfg->cfg.ssltCertificateNumber == number))
  {
    if (ssltIssueCmd(SSLT_CERTIFICATE_ACTIVE_SET, &number) != L7_SUCCESS)
    {
      osapiSemaGive(ssltCertSema);  
      return L7_FAILURE;
    }
  }

  osapiSemaGive(ssltCertSema);  
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Generate a certificate request for the device.
*
* @param L7_uint32  number     @b{(input)}  certificate number
* @param L7_char8  *commonName @b((input))  DN common name field
* @param L7_char8  *orgName    @b((input))  DN organization name field
* @param L7_char8  *orgUnit    @b((input))  DN organization unit field
* @param L7_char8  *location   @b((input))  DN location field
* @param L7_char8  *state      @b((input))  DN state field
* @param L7_char8  *country    @b((input))  DN country field
* @param L7_char8  *email      @b((input))  DN email field
* @param L7_char8  *request    @b((output)) cert request X509 PEM format
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE, self-signed certificate does not exist.
*
* @comments  The certificate request will use the RSA key pair 
*             generated for self-signed certificate (number).
*            DN field lengths must be >= L7_SSLT_SUBJECT_DN_FIELD_MIN and
*                                     <= L7_SSLT_SUBJECT_DN_FIELD_MAX
*            country length must be L7_SSLT_SUBJECT_COUNTRY_SIZE
*            request buffer MUST be L7_SSLT_PEM_BUFFER_SIZE_MAX 
*       
* @end
*
*********************************************************************/
L7_RC_t ssltCertificateRequestGenerate(L7_uint32 number,
                                       L7_char8 *commonName,
                                       L7_char8 *orgName,
                                       L7_char8 *orgUnit,
                                       L7_char8 *location,
                                       L7_char8 *state,
                                       L7_char8 *country,
                                       L7_char8 *email,
                                       L7_char8 *request)
{
  L7_char8 keyFile[L7_MAX_FILENAME];

 
  /* check all the parameters */
  if ((number < L7_SSLT_CERT_NUMBER_MIN) || 
      (number > L7_SSLT_CERT_NUMBER_MAX))
  {
    return L7_FAILURE;
  }

  /* Both Country and Organizational Name are required */
  if ((country != L7_NULLPTR) &&
      (strlen(country) != L7_SSLT_SUBJECT_DN_COUNTRY_SIZE))  
  {
    return L7_FAILURE;
  }
  if ((orgName != L7_NULLPTR) &&
      ((strlen(orgName) < L7_SSLT_SUBJECT_DN_FIELD_MIN) ||
       (strlen(orgName) > L7_SSLT_SUBJECT_DN_FIELD_MAX)))
  {
    return L7_FAILURE;
  }

  /* apply defaults if needed */
  if (commonName == L7_NULLPTR)
  {
    /* use ip address of the device, when there is more
       than one ip address for the device this should
       default to the lowest ip address */
    L7_uint32 ip = simGetSystemIPAddr();
    L7_char8  str[SSLT_ADDRESS_BUFFER_MAX];
    strncpy(str, osapiInet_ntoa(ip), sizeof(str));
    commonName = str;
  }

  osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);  
  /* make sure the key pair exists for this certificate number */
  if (ssltServerKeyFileGet(number, keyFile) != L7_SUCCESS)    
  {
    osapiSemaGive(ssltCertSema);
    return L7_FAILURE;
  }

  if (ssltServerCertReqCreate(number, commonName, orgName, orgUnit, location, 
                              state, country, email, request) != L7_SUCCESS)
  {
    osapiSemaGive(ssltCertSema);
    return L7_FAILURE;
  }

  osapiSemaGive(ssltCertSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get SSL tunnel server certificate information.
*
* @param L7_uint32  number      @b{(input)}  certificate number
* @param L7_BOOL   *active      @b((output)) this is the active certificate
* @param L7_char8  *issuerCN    @b((output)) certificate signing authority (Issued By)
* @param L7_char8  *validFrom   @b((output)) formatted time string 
* @param L7_char8  *validTo     @b((output)) formatted time string 
* @param L7_char8  *subjectCN   @b((output)) subject DN common name (Issued To)
* @param L7_char8  *subject     @b((output)) subject DN one-line format
* @param L7_char8  *fingerprint @b((output)) fingerprint hex format
* @param L7_char8  *cert        @b((output)) cert in X509 PEM format
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE, certificate number does not exist
*
* @comments  cert buffer must be L7_SSLT_PEM_BUFFER_SIZE_MAX
*            DN buffers must be L7_SSLT_SUBJECT_DN_FIELD_MAX
*            subject line buffer must be L7_SSLT_SUBJECT_LINE_MAX
*            fingerprint buffer must be L7_SSLT_FINGERPRINT_MD5_SIZE_MAX
*            date buffer must be L7_SSLT_CERT_DATE_SIZE_MAX
*       
* @end
*
*********************************************************************/
L7_RC_t ssltCertificateGet(L7_uint32  number,
                           L7_BOOL   *active,
                           L7_char8  *issuerCN,
                           L7_char8  *validFrom,
                           L7_char8  *validTo,
                           L7_char8  *subjectCN,
                           L7_char8  *subject,
                           L7_char8  *fingerprint,
                           L7_char8  *cert)
{
  X509 *certX509 = L7_NULLPTR;

  if ((number < L7_SSLT_CERT_NUMBER_MIN) || 
      (number > L7_SSLT_CERT_NUMBER_MAX))
  {
    return L7_FAILURE;
  }

  if ((active == L7_NULLPTR) ||
      (issuerCN == L7_NULLPTR) ||
      (validFrom == L7_NULLPTR) ||
      (validTo == L7_NULLPTR) ||
      (subjectCN == L7_NULLPTR) ||
      (subject == L7_NULLPTR) ||
      (fingerprint == L7_NULLPTR) ||
      (cert == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  /* read both file PEM format, and internal X509 format */
  osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);  
  if (ssltServerCertRead(number, cert, &certX509) != L7_SUCCESS)
  {
    osapiSemaGive(ssltCertSema);  
    return L7_FAILURE;
  }
  if (ssltCfg->cfg.ssltCertificateNumber == number)
  {
    *active = L7_TRUE;
  } else
  {
    *active = L7_FALSE;
  }
  osapiSemaGive(ssltCertSema);  

  /* read and format attributes */
  if ((ssltASN1TimePrint(X509_get_notBefore(certX509), 
                         validFrom) != L7_SUCCESS) ||
      (ssltASN1TimePrint(X509_get_notAfter(certX509), 
                         validTo) != L7_SUCCESS) ||
      (ssltX509DigestPrint(certX509, 
                           fingerprint) != L7_SUCCESS))
  {
    X509_free(certX509);
    return L7_FAILURE;
  }

  X509_NAME_get_text_by_NID(X509_get_issuer_name(certX509), 
                            NID_commonName, issuerCN, 
                            L7_SSLT_SUBJECT_DN_FIELD_MAX);
  X509_NAME_oneline(X509_get_subject_name(certX509), 
                    subject, L7_SSLT_SUBJECT_LINE_MAX);
  X509_NAME_get_text_by_NID(X509_get_subject_name(certX509), 
                            NID_commonName, subjectCN, 
                            L7_SSLT_SUBJECT_DN_FIELD_MAX);

  X509_free(certX509);
  return L7_SUCCESS;
}

