/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_fastpath_mgmt_security_api.c
*
* Purpose: APIs used for FASTPATH Security MIB
*
* Created by: Colin Verne 09/17/2003
*
* Component: SNMP
*
*********************************************************************/

#include "k_private_base.h"
#include "usmdb_sshd_api.h"
#include "usmdb_sslt_api.h"
#include "k_mib_fastpath_mgmt_security_api.h"
#include "sshd_exports.h"
#include "sslt_exports.h"

/* Begin Function Declarations: k_mib_fastpath_mgmt_security_api.h */

L7_RC_t
snmpAgentSSLAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbssltAdminModeGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSSLAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSSLAdminMode_disable;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSSLAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentSSLAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSSLAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbssltAdminModeSet(UnitIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentSSLProtocolLevelGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_ssl_val;
  L7_uint32 temp_tls_val;

  rc = usmDbssltProtocolLevelGet(UnitIndex, L7_SSLT_PROTOCOL_SSL30, &temp_ssl_val);

  if (rc == L7_SUCCESS)
  {
    if (rc == L7_SUCCESS)
    {
      rc = usmDbssltProtocolLevelGet(UnitIndex, L7_SSLT_PROTOCOL_TLS10, &temp_tls_val);

      if (temp_ssl_val == L7_ENABLE && temp_tls_val == L7_ENABLE)
      {
        *val = D_agentSSLProtocolLevel_both;
      }
      else if (temp_ssl_val == L7_ENABLE && temp_tls_val == L7_DISABLE)
      {
        *val = D_agentSSLProtocolLevel_ssl30;
      }
      else if (temp_ssl_val == L7_DISABLE && temp_tls_val == L7_ENABLE)
      {
        *val = D_agentSSLProtocolLevel_tls10;
      }
      else
      {
        /* unknown values */
        rc = L7_FAILURE;
      }
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSSLProtocolLevelSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_ssl_val = 0;
  L7_uint32 temp_tls_val = 0;

  switch (val)
  {
  case D_agentSSLProtocolLevel_ssl30:
    temp_ssl_val = L7_ENABLE;
    temp_tls_val = L7_DISABLE;
    break;

  case D_agentSSLProtocolLevel_tls10:
    temp_ssl_val = L7_DISABLE;
    temp_tls_val = L7_ENABLE;
    break;

  case D_agentSSLProtocolLevel_both:
    temp_ssl_val = L7_ENABLE;
    temp_tls_val = L7_ENABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbssltProtocolLevelSet(UnitIndex, L7_SSLT_PROTOCOL_SSL30, temp_ssl_val);

    if (rc == L7_SUCCESS)
    {
      rc = usmDbssltProtocolLevelSet(UnitIndex, L7_SSLT_PROTOCOL_TLS10, temp_tls_val);
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSSHAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbsshdAdminModeGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSSHAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSSHAdminMode_disable;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSSHAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentSSHAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSSHAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbsshdAdminModeSet(UnitIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentSSHProtocolLevelGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbsshdProtoLevelGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SSHD_PROTO_LEVEL_V1_ONLY:
      *val = D_agentSSHProtocolLevel_ssh10;
      break;

    case L7_SSHD_PROTO_LEVEL_V2_ONLY:
      *val = D_agentSSHProtocolLevel_ssh20;
      break;

    case L7_SSHD_PROTO_LEVEL:
      *val = D_agentSSHProtocolLevel_both;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSSHProtocolLevelSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentSSHProtocolLevel_ssh10:
    temp_val = L7_SSHD_PROTO_LEVEL_V1_ONLY;
    break;

  case D_agentSSHProtocolLevel_ssh20:
    temp_val = L7_SSHD_PROTO_LEVEL_V2_ONLY;
    break;

  case D_agentSSHProtocolLevel_both:
    temp_val = L7_SSHD_PROTO_LEVEL;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbsshdProtoLevelSet(UnitIndex, temp_val);
  }

  return rc;
}

/* SSH TELNET */


L7_RC_t snmpAgentMaxSSHSessionsGet(L7_uint32 UnitIndex, L7_uint32 val){

  L7_RC_t rc;
  L7_uint32 temp_val;


  rc = usmDbSshdMaxNumSessionsGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
      val = temp_val;
  }
  else
  {
      rc = L7_FAILURE;
  }

  return rc;

}



L7_RC_t snmpAgentMaxSSHSessionsSet(L7_uint32 UnitIndex, L7_uint32 val){

  L7_RC_t rc;

  rc = usmDbSshdMaxNumSessionsSet(UnitIndex, &val);

  return rc;

}
/* -- */

L7_RC_t snmpAgenSSHTimeoutGet(L7_uint32 UnitIndex, L7_uint32 val){

  L7_RC_t rc;
  L7_uint32 temp_val;


  rc = usmDbsshdTimeoutGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
      val = temp_val;
  }
  else
  {
      rc = L7_FAILURE;
  }

  return rc;

}



L7_RC_t snmpAgentSSHTimeoutSet(L7_uint32 UnitIndex, L7_uint32 val){

  L7_RC_t rc;

  rc = usmDbsshdTimeoutSet(UnitIndex, &val);

  return rc;

}

/* SSH TELNET */

L7_RC_t
snmpAgentSSHKeysPresentGet(L7_uint32 *val)
{
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
    *val = D_agentSSHKeysPresent_both;
  }
  else if (rsa_present == L7_TRUE)
  {
    *val = D_agentSSHKeysPresent_rsa;
  }
  else if (dsa_present == L7_TRUE)
  {
    *val = D_agentSSHKeysPresent_dsa;
  }
  else
  {
    *val = D_agentSSHKeysPresent_none;
  }

  return L7_SUCCESS;
}

L7_RC_t snmpAgentSSHKeyGenerationStatusGet(L7_uint32 *val)
{
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
    *val = D_agentSSHKeysPresent_both;
  }
  else if (rsa == L7_TRUE)
  {
    *val = D_agentSSHKeysPresent_rsa;
  }
  else if (dsa == L7_TRUE)
  {
    *val = D_agentSSHKeysPresent_dsa;
  }
  else
  {
    *val = D_agentSSHKeysPresent_none;
  }

  return L7_SUCCESS;
}

L7_RC_t
snmpAgentSSLCertificateControlSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSSLCertificateControl_generate:
    rc = usmDbssltCertificateGenerate(1,1024,0,0,0,0,0,0,0,0);
    break;

  case D_agentSSLCertificateControl_delete:
    osapiFsDeleteFile(L7_SSLT_SERVER_CERT_PEM_ONE);
    rc = osapiFsDeleteFile(L7_SSLT_SERVER_KEY_PEM_ONE);
    break;

  case D_agentSSLCertificateControl_noop:
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t
snmpAgentSSHRSAKeyControlSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSSHRSAKeyControl_generate:

    rc = usmDbsshdKeyGenerate(SSHD_KEY_TYPE_RSA);
    break;

  case D_agentSSHRSAKeyControl_delete:
    osapiFsDeleteFile(L7_SSHD_PATH_SERVER_PRIVKEY);
    rc = osapiFsDeleteFile(L7_SSHD_PATH_SERVER_PRIVKEY_RSA);
    break;

  case D_agentSSHRSAKeyControl_noop:
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t
snmpAgentSSHDSAKeyControlSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSSHDSAKeyControl_generate:
    rc = usmDbsshdKeyGenerate(SSHD_KEY_TYPE_DSA);
    break;

  case D_agentSSHDSAKeyControl_delete:
    rc = osapiFsDeleteFile(L7_SSHD_PATH_SERVER_PRIVKEY_DSA);
    break;

  case D_agentSSHDSAKeyControl_noop:
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

/* End Function Declarations */
