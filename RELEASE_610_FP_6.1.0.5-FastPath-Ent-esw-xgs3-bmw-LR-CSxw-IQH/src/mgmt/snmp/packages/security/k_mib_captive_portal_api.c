/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename k_mib_captive_portal_api.c
*
* @purpose Captive Portal Data Manager (CPDM) USMDB API functions
*
* @component CPDM
*
* @comments none
*
* @create 06/26/2007
*
* @authors  wjacobs, rjindal
*
* @end
*             
**********************************************************************/

#include "k_private_base.h"
#include "k_mib_captive_portal_api.h"
#include "usmdb_common.h"
#include "cli_web_exports.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "usmdb_cpim_api.h"
#include "usmdb_trapmgr_api.h"

/*********************************************************************
* @purpose  Get the CP mode
*
* @param    L7_uint32  *mode  @b{(output)} CP global mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmGlobalModeGet(L7_uint32 *mode)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmGlobalModeGet(&temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *mode = D_cpAdminMode_enable;
      break;

    case L7_DISABLE:
      *mode = D_cpAdminMode_disable;
      break;

    default:
      *mode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the CP mode
*
* @param    L7_uint32 mode @b{(input)} CP global mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmGlobalModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (mode)
  {
  case D_cpAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_cpAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbCpdmGlobalModeSet(temp_val);
  }

  return rc;
}

/********************************************************************* 
* @purpose  Set CP config mode
*
* @param    cpId @b{(input)} CP config id
* @param    mode @b{(input)} value for config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments only L7_CP_MODE_ENABLED, .._DISABLED are acceptable as inputs.
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigModeGet(cpId_t cpId, L7_uint32 *mode)
{
  L7_CP_MODE_STATUS_t temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmCPConfigModeGet(cpId, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_CP_MODE_ENABLED:
      *mode = D_cpCaptivePortalAdminMode_enable;
      break;

    case L7_CP_MODE_DISABLED:
      *mode = D_cpCaptivePortalAdminMode_disable;
      break;

    default:
      *mode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Get CP config protocol mode
*
* @param    cpId @b{(input)} CP config id
* @param    *protocolMode @b{(output)} protocol mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigProtocolModeGet(cpId_t cpId, L7_uint32 *protocolMode)
{
  L7_LOGIN_TYPE_t temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmCPConfigProtocolModeGet(cpId, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_LOGIN_TYPE_HTTPS:
      *protocolMode = D_cpCaptivePortalProtocolMode_https;
      break;

    case L7_LOGIN_TYPE_HTTP:
      *protocolMode = D_cpCaptivePortalProtocolMode_http;
      break;

    default:
      *protocolMode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Get CP config verify mode
*
* @param    cpId @b{(input)} CP config id
* @param    *verifyMode @b{(output)} verify mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigVerifyModeGet(cpId_t cpId, L7_uint32 *verifyMode)
{
  CP_VERIFY_MODE_t temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmCPConfigVerifyModeGet(cpId, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case CP_VERIFY_MODE_GUEST:
      *verifyMode = D_cpCaptivePortalVerificationMode_guest;
      break;

    case CP_VERIFY_MODE_LOCAL:
      *verifyMode = D_cpCaptivePortalVerificationMode_local;
      break;

    case CP_VERIFY_MODE_RADIUS:
      *verifyMode = D_cpCaptivePortalVerificationMode_radius;
      break;

    default:
      *verifyMode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Get CP config user group
*
* @param    cpId @b{(input)} CP config id
* @param    *group @b{(output)} group id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigGroupIDGet(cpId_t cpId, L7_uint32 *group)
{
  CP_VERIFY_MODE_t verifyMode;
  gpId_t temp_val;

  if ((usmDbCpdmCPConfigVerifyModeGet(cpId, &verifyMode) == L7_SUCCESS) && 
      (verifyMode == CP_VERIFY_MODE_LOCAL))
  {
    if (usmDbCpdmCPConfigGpIdGet(cpId, &temp_val) == L7_SUCCESS)
    {
      *group = (L7_uint32)temp_val;  
    }
  }
  else
  {
    *group = 0;
  }

  return L7_SUCCESS;
}

#ifdef I_cpCaptivePortalUserLogoutMode
/*********************************************************************
* @purpose  Get CP config user logout Mode
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_BOOL *userLogoutMode @b{(output)} user logout mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigUserLogoutModeGet(cpId_t cpId, L7_uint32 *userLogoutMode)
{
  L7_uchar8 temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmCPConfigUserLogoutModeGet(cpId, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *userLogoutMode = D_cpCaptivePortalUserLogoutMode_enable;
      break;

    case L7_DISABLE:
      *userLogoutMode = D_cpCaptivePortalUserLogoutMode_disable;
      break;

    default:
      *userLogoutMode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}
#endif /* I_cpCaptivePortalUserLogoutMode */

/*********************************************************************
* @purpose  Get CP config redirectMode
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_BOOL *redirectMode @b{(output)} verify mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigRedirectModeGet(cpId_t cpId, L7_uint32 *redirectMode)
{
  L7_uchar8 temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmCPConfigRedirectModeGet(cpId, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *redirectMode = D_cpCaptivePortalURLRedirectMode_enable;
      break;

    case L7_DISABLE:
      *redirectMode = D_cpCaptivePortalURLRedirectMode_disable;
      break;

    default:
      *redirectMode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Get CP config redirect url
*
* @param    cpId @b{(input)} CP config id
* @param    *url @b{(output)} redirect url
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigRedirectURLGet(cpId_t cpId, L7_char8 *url)
{
  L7_uchar8 redirectMode;
  L7_char8 temp_val[CP_WELCOME_URL_MAX+1];

  memset(temp_val, 0, sizeof(temp_val));

  if ((usmDbCpdmCPConfigRedirectModeGet(cpId, &redirectMode) == L7_SUCCESS) && 
      (redirectMode == L7_ENABLE))
  {
    (void)usmDbCpdmCPConfigRedirectURLGet(cpId, temp_val);
  }

  strncpy(url, temp_val, CP_WELCOME_URL_MAX);
  url[CP_WELCOME_URL_MAX] = '\0';
  return L7_SUCCESS;
}

/********************************************************************* 
* @purpose  Set CP config mode
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_CP_MODE_STATUS_t mode @b{(input)} value for
*           onfig item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments only L7_CP_MODE_ENABLED, .._DISABLED are acceptable as inputs.
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigModeSet(cpId_t cpId, L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_CP_MODE_STATUS_t temp_val = 0;

  switch (mode)
  {
  case D_cpCaptivePortalAdminMode_enable:
    temp_val = L7_CP_MODE_ENABLED;
    break;

  case D_cpCaptivePortalAdminMode_disable:
    temp_val = L7_CP_MODE_DISABLED;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbCpdmCPConfigModeSet(cpId, temp_val);
  }

  return rc;
}

/*********************************************************************
* @purpose  Set CP config protocol mode
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_LOGIN_TYPE_t *protocolMode @b{(input)} protocol mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigProtocolModeSet(cpId_t cpId, L7_uint32 protocolMode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_LOGIN_TYPE_t temp_val = 0;

  switch (protocolMode)
  {
  case D_cpCaptivePortalProtocolMode_https:
    temp_val = L7_LOGIN_TYPE_HTTPS;
    break;

  case D_cpCaptivePortalProtocolMode_http:
    temp_val = L7_LOGIN_TYPE_HTTP;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbCpdmCPConfigProtocolModeSet(cpId, temp_val);
  }

  return rc;
}

/*********************************************************************
* @purpose  Set CP config verify mode
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    CP_VERIFY_MODE_t *verifyMode @b{(input)} verify mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigVerifyModeSet(cpId_t cpId, L7_uint32 verifyMode)
{
  L7_RC_t rc = L7_SUCCESS;
  CP_VERIFY_MODE_t temp_val = 0;

  switch (verifyMode)
  {
  case D_cpCaptivePortalVerificationMode_guest:
    temp_val = CP_VERIFY_MODE_GUEST;
    break;

  case D_cpCaptivePortalVerificationMode_local:
    temp_val = CP_VERIFY_MODE_LOCAL;
    break;

  case D_cpCaptivePortalVerificationMode_radius:
    temp_val = CP_VERIFY_MODE_RADIUS;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbCpdmCPConfigVerifyModeSet(cpId, temp_val);
  }

  return rc;
}

#ifdef I_cpCaptivePortalUserLogoutMode
/*********************************************************************
* @purpose  Set CP config user logout mode
*
* @param    cpId_t    cpId          @b{(input)} CP config id
* @param    L7_uint32 userLogoutMode  @b{(input)} logout mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigUserLogoutModeSet(cpId_t cpId, L7_uint32 userLogoutMode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 temp_val = 0;

  switch (userLogoutMode)
  {
  case D_cpCaptivePortalUserLogoutMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_cpCaptivePortalUserLogoutMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbCpdmCPConfigUserLogoutModeSet(cpId, temp_val);
  }

  return rc;
}
#endif /* I_cpCaptivePortalUserLogoutMode */

/*********************************************************************
* @purpose  Set CP config redirect mode
*
* @param    cpId_t    cpId          @b{(input)} CP config id
* @param    L7_uint32 redirectMode  @b{(input)} redirect mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigRedirectModeSet(cpId_t cpId, L7_uint32 redirectMode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 temp_val = 0;

  switch (redirectMode)
  {
  case D_cpCaptivePortalURLRedirectMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_cpCaptivePortalURLRedirectMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbCpdmCPConfigRedirectModeSet(cpId, temp_val);
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the global admin mode for captive portal
*
* @param    L7_CP_MODE_STATUS_t @b{(output)} admin mode status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmGlobalStatusModeGet(L7_uint32 *mode)
{
  L7_CP_MODE_STATUS_t temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmGlobalStatusModeGet(&temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_CP_MODE_ENABLED:
      *mode = D_cpCaptivePortalOperStatus_enabled;
      break;

    case L7_CP_MODE_ENABLE_PENDING:
      *mode = D_cpCaptivePortalOperStatus_enablePending;
      break;

    case L7_CP_MODE_DISABLE_PENDING:
      *mode = D_cpCaptivePortalOperStatus_disablePending;
      break;

    case L7_CP_MODE_DISABLED:
      *mode = D_cpCaptivePortalOperStatus_disabled;
      break;

    default:
      *mode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the reason for the CP to be disabled
*
* @param    L7_CP_MODE_REASON_t *reason @b{(output)} disable reason
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmGlobalCPDisableReasonGet(L7_uint32 *reason)
{
  L7_CP_MODE_STATUS_t status;
  L7_CP_MODE_REASON_t temp_val;
  L7_RC_t rc;

  if ((usmDbCpdmGlobalStatusModeGet(&status) == L7_SUCCESS) && 
      (status == L7_CP_MODE_DISABLED || status == L7_CP_MODE_DISABLE_PENDING))
  {
    rc = usmDbCpdmGlobalCPDisableReasonGet(&temp_val);
    if (rc == L7_SUCCESS)
    {
      switch (temp_val)
      {
      case L7_CP_MODE_REASON_NONE:
        *reason = D_cpCaptivePortalOperDisabledReason_none;
        break;

      case L7_CP_MODE_REASON_ADMIN:
        *reason = D_cpCaptivePortalOperDisabledReason_adminDisabled;
        break;

      case L7_CP_MODE_REASON_NO_IP_ADDR:
        *reason = D_cpCaptivePortalOperDisabledReason_noIPAddress;
        break;

      case L7_CP_MODE_REASON_ROUTING_ENABLED_NO_IP_RT_INTF:
        *reason = D_cpCaptivePortalOperDisabledReason_noIPRoutingIntf;
        break;

      case L7_CP_MODE_REASON_ROUTING_DISABLED:
        *reason = D_cpCaptivePortalOperDisabledReason_routingDisabled;
        break;

      default:
        *reason = 0;
        rc = L7_FAILURE;
      }
    }
  }
  else
  {
    *reason = D_cpCaptivePortalOperDisabledReason_none;
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the CP IP address
*
* @param    L7_IP_ADDR_t *ipAddr @b{(output)} ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmGlobalCPIPAddressGet(L7_IP_ADDR_t *ipAddr)
{
  L7_uint32 temp_val;

  if ((usmDbCpdmGlobalModeGet(&temp_val) == L7_SUCCESS) && (temp_val == L7_ENABLE))
  {
    return usmDbCpdmGlobalCPIPAddressGet(ipAddr);
  }

  *ipAddr = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the operational status of a CP instance.
*
* @param    cpid    @b{(input)} CP config id
* @param    *status @b{(output)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigOperStatusGet(cpId_t cpId, L7_uint32 *status)
{
  L7_CP_INST_OPER_STATUS_t temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmCPConfigOperStatusGet(cpId, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_CP_INST_OPER_STATUS_ENABLED:
      *status = D_cpCaptivePortalInstanceOperStatus_enable;
      break;

    case L7_CP_INST_OPER_STATUS_DISABLED:
      *status = D_cpCaptivePortalInstanceOperStatus_disable;
      break;

    default:
      *status = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the disable reason of a disabled CP instance.
*
* @param    cpid    @b{(input)} CP config id
* @param    *reason @b{(output)} disable reason
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
********************************************************************/
L7_RC_t snmpCpdmCPConfigDisableReasonGet(cpId_t cpId, L7_uint32 *reason)
{
  L7_CP_INST_DISABLE_REASON_t temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmCPConfigDisableReasonGet(cpId, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_CP_INST_DISABLE_REASON_NONE:
      *reason = D_cpCaptivePortalInstanceOperDisabledReason_none;
      break;

    case L7_CP_INST_DISABLE_REASON_ADMIN:
      *reason = D_cpCaptivePortalInstanceOperDisabledReason_adminDisabled;
      break;

    case L7_CP_INST_DISABLE_REASON_NO_RADIUS_SERVER:
      *reason = D_cpCaptivePortalInstanceOperDisabledReason_noRadiusServer;
      break;

    case L7_CP_INST_DISABLE_REASON_NO_ACCT_SERVER:
      *reason = D_cpCaptivePortalInstanceOperDisabledReason_noAccountingServer;
      break;

    case L7_CP_INST_DISABLE_REASON_NOT_ASSOC_INTF:
      *reason = D_cpCaptivePortalInstanceOperDisabledReason_noIntfAssociation;
      break;

    case L7_CP_INST_DISABLE_REASON_NO_ACTIVE_INTF:
      *reason = D_cpCaptivePortalInstanceOperDisabledReason_noActiveIntf;
      break;

    case L7_CP_INST_DISABLE_REASON_NO_VALID_CERT:
      *reason = D_cpCaptivePortalInstanceOperDisabledReason_noValidCert;
      break;

    default:
      *reason = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the blocked status of a CP instance
*
* @param    cpId_t     cpId          @b{(input)} CP config id
* @param    L7_uint32  *blockStatus  @b{(output)} block status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t snmpCpdmCPConfigBlockStatusGet(cpId_t cpId, L7_uint32 *blockStatus)
{
  L7_CP_INST_BLOCK_STATUS_t temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmCPConfigBlockedStatusGet(cpId, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_CP_INST_BLOCK_STATUS_BLOCKED_PENDING:
      *blockStatus = D_cpCaptivePortalInstanceBlockStatus_blockPending;
      break;

    case L7_CP_INST_BLOCK_STATUS_BLOCKED:
      *blockStatus = D_cpCaptivePortalInstanceBlockStatus_blocked;
      break;

    case L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED_PENDING:
      *blockStatus = D_cpCaptivePortalInstanceBlockStatus_notBlockPending;
      break;

    case L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED:
      *blockStatus = D_cpCaptivePortalInstanceBlockStatus_notBlocked;
      break;

    default:
      *blockStatus = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the block status of a CP instance
*
* @param    cpId_t    cpId         @b{(input)} CP config id
* @param    L7_uint32 blockStatus  @b{(input)} block status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigBlockStatusSet(cpId_t cpId, L7_uint32 blockStatus)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_CP_INST_BLOCK_STATUS_t temp_val = 0;

  switch (blockStatus)
  {
  case D_cpCaptivePortalInstanceBlockStatus_blocked:
    temp_val = L7_CP_INST_BLOCK_STATUS_BLOCKED;
    break;

  case D_cpCaptivePortalInstanceBlockStatus_notBlocked:
    temp_val = L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbCpdmCPConfigBlockedStatusSet(cpId, temp_val);
  }

  return rc;
}

/*********************************************************************
* @purpose  Retrieve enabled status for interface
*
* @param    L7_uint32  intf     @b{(input)} interface number
* @param    L7_uint32  *status  @b{(output)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPIntfStatusGet(L7_uint32 intf, L7_uint32 *status)
{
  L7_BOOL temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmIntfStatusGetIsEnabled(intf, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *status = D_cpCaptivePortalIntfOperStatus_enable;
      break;

    case L7_FALSE:
      *status = D_cpCaptivePortalIntfOperStatus_disable;
      break;

    default:
      *status = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Retrieve reason for disabling of interface
*
* @param    L7_uint32               intf  @b{(input)} interface number
* @param    L7_INTF_WHY_DISABLED_t  *why  @b{(output)} intf disable reason
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPIntfStatusGetWhy(L7_uint32 intf, L7_uint32 *why)
{
  L7_BOOL status;
  L7_INTF_WHY_DISABLED_t temp_val;
  L7_RC_t rc;

  if ((usmDbCpdmIntfStatusGetIsEnabled(intf, &status) == L7_SUCCESS) && 
      (status == L7_DISABLE))
  {
    rc = usmDbCpdmIntfStatusGetWhy(intf, &temp_val);
    if (rc == L7_SUCCESS)
    {
      switch (temp_val)
      {
      case L7_INTF_WHY_DISABLED_NONE:
        *why = D_cpCaptivePortalIntfOperDisabledReason_none;
        break;
  
      case L7_INTF_WHY_DISABLED_UNATTACHED:
        *why = D_cpCaptivePortalIntfOperDisabledReason_intfNotAttached;
        break;
  
      case L7_INTF_WHY_DISABLED_ADMIN:
        *why = D_cpCaptivePortalIntfOperDisabledReason_adminDisabled;
        break;
  
      default:
        *why = 0;
        rc = L7_FAILURE;
        break;
      }
    }
  }
  else
  {
    *why = D_cpCaptivePortalIntfOperDisabledReason_none;
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the blocked status of a CP interface
*
* @param    L7_uint32  intf          @b{(input)} CP interface
* @param    L7_uint32  *blockStatus  @b{(output)} block status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t snmpCpdmCPIntfBlockStatusGet(L7_uint32 intf, L7_uint32 *blockStatus)
{
  L7_CP_INST_BLOCK_STATUS_t temp_val;
  L7_RC_t rc;

  rc = usmDbCpimIntfCapabilityBlockStatusGet(intf, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_CP_INST_BLOCK_STATUS_BLOCKED:
      *blockStatus = D_cpCaptivePortalIntfBlockStatus_blocked;
      break;

    case L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED:
      *blockStatus = D_cpCaptivePortalIntfBlockStatus_notBlocked;
      break;

    default:
      *blockStatus = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

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

#ifdef D_cpCaptivePortalIntfCapabilities_idleTimeout
  if (usmDbCpimIntfCapabilityIdleTimeoutGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *buf |= (1 << D_cpCaptivePortalIntfCapabilities_idleTimeout);
    }
  }
#endif /* D_cpCaptivePortalIntfCapabilities_idleTimeout */

#ifdef D_cpCaptivePortalIntfCapabilities_maxBytesReceivedCounter
  if (usmDbCpimIntfCapabilityBytesReceivedCounterGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *buf |= (1 << D_cpCaptivePortalIntfCapabilities_maxBytesReceivedCounter);
    }
  }
#endif /* D_cpCaptivePortalIntfCapabilities_maxBytesReceivedCounter */

#ifdef D_cpCaptivePortalIntfCapabilities_maxBytesTransmittedCounter
  if (usmDbCpimIntfCapabilityBytesTransmittedCounterGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *(buf) |= (1 << (D_cpCaptivePortalIntfCapabilities_maxBytesTransmittedCounter));
    }
  }
#endif /* D_cpCaptivePortalIntfCapabilities_maxBytesTransmittedCounter */

#ifdef D_cpCaptivePortalIntfCapabilities_maxPacketsReceivedCounter
  if (usmDbCpimIntfCapabilityPacketsReceivedCounterGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *(buf) |= (1 << (D_cpCaptivePortalIntfCapabilities_maxPacketsReceivedCounter));
    }
  }
#endif /* D_cpCaptivePortalIntfCapabilities_maxPacketsReceivedCounter */

#ifdef D_cpCaptivePortalIntfCapabilities_maxPacketsTransmittedCounter
  if (usmDbCpimIntfCapabilityPacketsTransmittedCounterGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *(buf) |= (1 << (D_cpCaptivePortalIntfCapabilities_maxPacketsTransmittedCounter));
    }
  }
#endif /* D_cpCaptivePortalIntfCapabilities_maxPacketsTransmittedCounter */

#ifdef D_cpCaptivePortalIntfCapabilities_clientRoaming
  if (usmDbCpimIntfCapabilityRoamingSupportGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *(buf) |= (1 << (D_cpCaptivePortalIntfCapabilities_clientRoaming));
    }
  }
#endif /* D_cpCaptivePortalIntfCapabilities_clientRoaming */

#ifdef D_cpCaptivePortalIntfCapabilities_maxBandwidthUp
  if (usmDbCpimIntfCapabilityBandwidthUpRateControlGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *buf |= (1 << D_cpCaptivePortalIntfCapabilities_maxBandwidthUp);
    }
  }
#endif /* D_cpCaptivePortalIntfCapabilities_maxBandwidthUp */

#ifdef D_cpCaptivePortalIntfCapabilities_maxBandwidthDown
  if (usmDbCpimIntfCapabilityBandwidthDownRateControlGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *buf |= (1 << D_cpCaptivePortalIntfCapabilities_maxBandwidthDown);
    }
  }
#endif /* D_cpCaptivePortalIntfCapabilities_maxBandwidthDown */

#ifdef D_cpCaptivePortalIntfCapabilities_maxInputOctets
  if (usmDbCpimIntfCapabilityMaxInputOctetMonitorGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *buf |= (1 << D_cpCaptivePortalIntfCapabilities_maxInputOctets);
    }
  }
#endif /* D_cpCaptivePortalIntfCapabilities_maxInputOctets */

#ifdef D_cpCaptivePortalIntfCapabilities_maxOutputOctets
  if (usmDbCpimIntfCapabilityMaxOutputOctetMonitorGet(intf, &temp_val) == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *buf |= (1 << D_cpCaptivePortalIntfCapabilities_maxOutputOctets);
    }
  }
#endif /* D_cpCaptivePortalIntfCapabilities_maxOutputOctets */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get protocolMode with existing MAC for connection status
*
* @param    *macAddr      @b{(input)} MAC addr to try
* @param    protocolMode  @b{(output)} value of "protocolMode"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientConnStatusProtocolModeGet(L7_enetMacAddr_t *macAddr, L7_uint32 *protocolMode)
{
  L7_RC_t rc;
  L7_LOGIN_TYPE_t temp_val;

  rc = usmDbCpdmClientConnStatusProtocolModeGet(macAddr, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_LOGIN_TYPE_HTTPS:
      *protocolMode = D_cpCaptivePortalClientProtocolMode_https;
      break;

    case L7_LOGIN_TYPE_HTTP:
      *protocolMode = D_cpCaptivePortalClientProtocolMode_http;
      break;

    default:
      *protocolMode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Get verifyMode with existing MAC for connection status
*
* @param    *macAddr     @b{(input)} MAC addr to try
* @param    *verifyMode  @b{(output)} value of "verifyMode"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientConnStatusVerifyModeGet(L7_enetMacAddr_t *macAddr, L7_uint32 *verifyMode)
{
  L7_RC_t rc;
  CP_VERIFY_MODE_t temp_val;

  rc = usmDbCpdmClientConnStatusVerifyModeGet(macAddr, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case CP_VERIFY_MODE_GUEST:
      *verifyMode = D_cpCaptivePortalClientVerificationMode_guest;
      break;

    case CP_VERIFY_MODE_LOCAL:
      *verifyMode = D_cpCaptivePortalClientVerificationMode_local;
      break;

    case CP_VERIFY_MODE_RADIUS:
      *verifyMode = D_cpCaptivePortalClientVerificationMode_radius;
      break;

    default:
      *verifyMode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Get intfId with existing MAC for connection status
*
* @param    *macAddr  @b{(input)} MAC addr to try
* @param    *ifIndex  @b{(output)} value of "intfId"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientConnStatusIntfIdGet(L7_enetMacAddr_t *macAddr, L7_uint32 *ifIndex)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfNum; 

  if (usmDbCpdmClientConnStatusIntfIdGet(macAddr, &intIfNum) == L7_SUCCESS)
  {
    rc = nimGetIntfIfIndex(intIfNum, ifIndex);
  }

  return rc;
}

/*********************************************************************
* @purpose  Get sessionTime with existing MAC for connection status
*
* @param    L7_enetMacAddr_t  *macAddr      @b{(input)} MAC addr to try
* @param    L7_uint32         *sessionTime  @b{(output)} value of "sessionTime"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientConnStatusSessionTimeGet(L7_enetMacAddr_t *macAddr, L7_uint32 *sessionTime)
{
  L7_uint32 clientTime;
  usmDbTimeSpec_t ts;
  L7_RC_t rc;

  rc = usmDbCpdmClientConnStatusSessionTimeGet(macAddr, &clientTime);
  osapiConvertRawUpTime(clientTime, (L7_timespec *)&ts);

  if (rc == L7_SUCCESS)
  {
    *sessionTime = ts.seconds;
    *sessionTime += ts.minutes * SNMP_MINUTES;      /* 60 */
    *sessionTime += ts.hours * SNMP_HOURS;          /* 60 * 60 */
    *sessionTime += ts.days * SNMP_DAYS;            /* 60 * 60 * 24 */
    *sessionTime *= SNMP_HUNDRED;
  }

  return rc;
}

#ifdef I_cpCaptivePortalClientSwitchType
/*********************************************************************
* @purpose  Get the switch type with existing MAC for connection status
*
* @param    *macAddr  @b{(input)} MAC addr to try
* @param    type      @b{(output)} switch type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientConnStatusWhichSwitchGet(L7_enetMacAddr_t *macAddr, L7_uint32 *type)
{
  L7_RC_t rc;
  cpdmAuthWhichSwitch_t temp_val;

  rc = usmDbCpdmClientConnStatusWhichSwitchGet(macAddr, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case CP_AUTH_ON_PEER_SWITCH:
      *type = D_cpCaptivePortalClientSwitchType_peer;
      break;

    case CP_AUTH_ON_LOCAL_SWITCH:
      *type = D_cpCaptivePortalClientSwitchType_local;
      break;

    default:
      *type = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}
#endif /* I_cpCaptivePortalClientSwitchType */

/*********************************************************************
* @purpose  Convert a 64-bit interger
*
* @param    L7_uint64 value  @b{(input)} 64-bit integer
* @param    L7_uint32 *high  @b{(input)} high end
* @param    L7_uint32 *low   @b{(input)} low end
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCPConvert64BitUnsignedInteger(L7_uint64 value, L7_uint32 *high, L7_uint32 *low)
{
  if ((high == L7_NULLPTR) || (low == L7_NULLPTR))
    return L7_FAILURE;

  *high = (L7_uint32)(value >> 32);
  *low = (L7_uint32)value;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deauthenticate connected client
*
* @param    L7_uchar8 *mac  @b{(input)} MAC addr to try
* @param    L7_uint32 val   @b{(input)} action
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientDeauthenticateAction(L7_uchar8 *mac, L7_int32 val)
{
  L7_enetMacAddr_t macAddr;
  L7_RC_t rc = L7_FAILURE;

  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&macAddr.addr, mac, L7_MAC_ADDR_LEN);

  switch(val)
  {
  case D_cpCaptivePortalClientDeauthAction_none:
    rc = L7_SUCCESS;
    break;

  case D_cpCaptivePortalClientDeauthAction_start:
    rc = usmDbCpdmClientConnStatusDelete(&macAddr);
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

/*********************************************************************
* @purpose  Check for existing CP instance/client MAC association
*
* @param  cpId_t           cpId @b{(input)} CP instance to find
* @param  L7_enetMacAddr_t *mac @b{(input)} MAC address to find
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConnStatusGet(L7_uint32 cpId, L7_enetMacAddr_t *mac)
{
  cpId_t appCpId;

  appCpId = (cpId_t) cpId;
  if (usmDbCpdmCPConnStatusGet(appCpId, mac)== L7_SUCCESS) 
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Find NEXT association of CP instance and client MAC
*
* @param  cpId_t           cpId @b{(input)} CP instance to find
* @param  L7_enetMacAddr_t *mac @b{(input)} MAC address to find
* @param  cpId_t           *pCpId @b{(output)} next CP instance
* @param  L7_enetMacAddr_t *pMmac @b{(output)} next MAC address 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConnStatusNextGet(L7_uint32 cpId, L7_enetMacAddr_t *mac,
                                    L7_uint32 *pCpId, L7_enetMacAddr_t *pMac)
{
  cpId_t appCpId;

  appCpId = (cpId_t) cpId;
  if (usmDbCpdmCPConnStatusNextGet(cpId, mac, &appCpId, pMac)== L7_SUCCESS) 
  {
    *pCpId =  (L7_uint32) appCpId;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get Trap Manager's Captive Portal trap flag
*
* @param    L7_uint32  *mode  @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t snmpTrapCaptivePortalGet(L7_uint32 *mode)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbTrapCaptivePortalGet(&temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *mode = D_cpTrapMode_enable;
      break;

    case L7_DISABLE:
      *mode = D_cpTrapMode_disable;
      break;

    default:
      *mode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set Trap Manager's Captive Portal trap flag
*
* @param    L7_uint32  mode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t snmpTrapCaptivePortalSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (mode)
  {
  case D_cpTrapMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_cpTrapMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbTrapCaptivePortalSet(temp_val);
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the CP client authentication failure trap mode
*
* @param    L7_uint32  *mode  @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientAuthFailureTrapModeGet(L7_uint32 *mode)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmGlobalTrapModeGet(CP_TRAP_AUTH_FAILURE, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *mode = D_cpClientAuthenticationFailureTrapMode_enable;
      break;

    case L7_DISABLE:
      *mode = D_cpClientAuthenticationFailureTrapMode_disable;
      break;

    default:
      *mode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the CP client authentication failure trap mode
*
* @param    L7_uint32  mode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientAuthFailureTrapModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (mode)
  {
  case D_cpClientAuthenticationFailureTrapMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_cpClientAuthenticationFailureTrapMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbCpdmGlobalTrapModeSet(CP_TRAP_AUTH_FAILURE, temp_val);
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the CP client connection trap mode
*
* @param    L7_uint32  *mode  @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientConnectTrapModeGet(L7_uint32 *mode)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmGlobalTrapModeGet(CP_TRAP_CLIENT_CONNECTED, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *mode = D_cpClientConnectTrapMode_enable;
      break;

    case L7_DISABLE:
      *mode = D_cpClientConnectTrapMode_disable;
      break;

    default:
      *mode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the CP client connection trap mode
*
* @param    L7_uint32  mode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientConnectTrapModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (mode)
  {
  case D_cpClientConnectTrapMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_cpClientConnectTrapMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbCpdmGlobalTrapModeSet(CP_TRAP_CLIENT_CONNECTED, temp_val);
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the CP client database full trap mode
*
* @param    L7_uint32  *mode  @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientDBFullTrapModeGet(L7_uint32 *mode)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmGlobalTrapModeGet(CP_TRAP_CONNECTION_DB_FULL, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *mode = D_cpClientDatabaseFullTrapMode_enable;
      break;

    case L7_DISABLE:
      *mode = D_cpClientDatabaseFullTrapMode_disable;
      break;

    default:
      *mode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the CP client database full trap mode
*
* @param    L7_uint32  mode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientDBFullTrapModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (mode)
  {
  case D_cpClientDatabaseFullTrapMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_cpClientDatabaseFullTrapMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbCpdmGlobalTrapModeSet(CP_TRAP_CONNECTION_DB_FULL, temp_val);
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the CP client disconnection trap mode
*
* @param    L7_uint32  *mode  @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientDisconnectTrapModeGet(L7_uint32 *mode)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmGlobalTrapModeGet(CP_TRAP_CLIENT_DISCONNECTED, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *mode = D_cpClientDisconnectTrapMode_enable;
      break;

    case L7_DISABLE:
      *mode = D_cpClientDisconnectTrapMode_disable;
      break;

    default:
      *mode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the CP client disconnection trap mode
*
* @param    L7_uint32  mode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientDisconnectTrapModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (mode)
  {
  case D_cpClientDisconnectTrapMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_cpClientDisconnectTrapMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbCpdmGlobalTrapModeSet(CP_TRAP_CLIENT_DISCONNECTED, temp_val);
  }

  return rc;
}

#if 0
/* Currently not implemented */

/*********************************************************************
* @purpose  Get CP config RADIUS accounting
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uint32 *radiusAccounting @b{(output)} RADIUS accounting
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigRadiusAccountingGet(cpId_t cpId, L7_uint32 *radiusAccounting)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbCpdmCPConfigRadiusAccountingGet(cpId, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *radiusAccounting = D_cpCaptivePortalRadiusAccountingMode_enable;
      break;

    case L7_DISABLE:
      *radiusAccounting = D_cpCaptivePortalRadiusAccountingMode_disable;
      break;

    default:
      *radiusAccounting = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set CP config RADIUS accounting
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uint32 radiusAccounting @b{(input)} RADIUS accounting
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigRadiusAccountingSet(cpId_t cpId, L7_uint32 radiusAccounting)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (radiusAccounting)
  {
  case D_cpCaptivePortalRadiusAccountingMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_cpCaptivePortalRadiusAccountingMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbCpdmCPConfigRadiusAccountingSet(cpId, temp_val);
  }

  return rc;
}
#endif

