/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_global_status.c
*
* @purpose    Code in support of the cp_global_status.html page
*
* @component  Captive Portal
*
* @comments
*
* @create     7/9/2007
*
* @author     rjindal
*
* @end
*
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include "l7_common.h"
#include "usmdb_util_api.h"
#include "ew_proto.h"
#include "ewnet.h"
#include "web.h"
#include "web_oem.h"
#include "strlib_security_web.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_user_api.h"
#include "captive_portal_commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"


/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext              context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpGlobalStatusP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpGlobalStatus(EwsContext context, EwaForm_cpGlobalStatusP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_CP_MODE_STATUS_t status = L7_CP_MODE_DISABLED;
  L7_CP_MODE_REASON_t reason;
  L7_IP_ADDR_t ipAddr = 0;
  L7_char8 ipBuf[USMWEB_IPADDR_SIZE];
  L7_uint32 val;

  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->value.err_flag = L7_FALSE;
  form->status.err_flag |= (EW_FORM_INITIALIZED);

  net = ewsContextNetHandle(context);
  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    form->value.err_msg = osapiStrDup(appInfo.err.msg);
    form->status.err_msg |= (EW_FORM_DYNAMIC);
  }

  /* cp global status */
  if (usmDbCpdmGlobalStatusModeGet(&status) == L7_SUCCESS)
  {
    switch (status)
    {
    case L7_CP_MODE_ENABLED:
      form->value.cp_mode_status = (char *)osapiStrDup(pStrInfo_common_Enbld);
      break;
    case L7_CP_MODE_ENABLE_PENDING:
      form->value.cp_mode_status = (char *)osapiStrDup(pStrInfo_security_CPModeEnablePending);
      break;
    case L7_CP_MODE_DISABLED:
      form->value.cp_mode_status = (char *)osapiStrDup(pStrInfo_common_Dsbld);
      break;
    case L7_CP_MODE_DISABLE_PENDING:
      form->value.cp_mode_status = (char *)osapiStrDup(pStrInfo_security_CPModeDisablePending);
      break;
    default:
      form->value.cp_mode_status = (char *)osapiStrDup(pStrInfo_common_Dsbld);
      break;
    }
    form->status.cp_mode_status |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* cp global disable reason */
  if ((status == L7_CP_MODE_DISABLED) || (status == L7_CP_MODE_DISABLE_PENDING))
  {
    if (usmDbCpdmGlobalCPDisableReasonGet(&reason) == L7_SUCCESS)
    {
      switch (reason)
      {
      case L7_CP_MODE_REASON_ADMIN:
        form->value.cp_mode_disable_reason = (char *)osapiStrDup(pStrInfo_security_CPModeReasonAdmin);
        break;
      case L7_CP_MODE_REASON_NO_IP_ADDR:
        form->value.cp_mode_disable_reason = (char *)osapiStrDup(pStrInfo_security_CPModeReasonNoIpAddr);
        break;
      case L7_CP_MODE_REASON_ROUTING_ENABLED_NO_IP_RT_INTF:
        form->value.cp_mode_disable_reason = (char *)osapiStrDup(pStrInfo_security_CPModeReasonRoutingNoIpRoutingIntf);
        break;
      case L7_CP_MODE_REASON_ROUTING_DISABLED:
        form->value.cp_mode_disable_reason = (char *)osapiStrDup(pStrInfo_security_CPModeReasonRoutingDisabled);
        break;
      default:
        form->value.cp_mode_disable_reason = (char *)osapiStrDup("");
        break;
      }
      form->status.cp_mode_disable_reason |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
  }

  /* cp ip address */
  memset(ipBuf, 0, sizeof(ipBuf));
  osapiStrncpy(ipBuf, pStrInfo_common_0000, sizeof(ipBuf)-1);
  if ((usmDbCpdmGlobalModeGet(&val) == L7_SUCCESS) && (val == L7_ENABLE))
  {
    if (usmDbCpdmGlobalCPIPAddressGet(&ipAddr) == L7_SUCCESS)
    {
      if (usmDbInetNtoa(ipAddr, ipBuf) == L7_SUCCESS)
      {
        form->value.cp_ip_addr = osapiStrDup(ipBuf);
        form->status.cp_ip_addr |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
      }
    }
  }

  /* supported cps */
  if (usmDbCpdmGlobalSupportedCPInstancesGet(&val) == L7_SUCCESS)
  {
    form->value.supported_cp = val;
    form->status.supported_cp |= (EW_FORM_INITIALIZED);
  }

  /* configured cps */
  if (usmDbCpdmGlobalConfiguredCPInstancesGet(&val) == L7_SUCCESS)
  {
    form->value.configured_cp = val;
    form->status.configured_cp |= (EW_FORM_INITIALIZED);
  }

  /* active cps */
  if (usmDbCpdmGlobalActiveCPInstancesGet(&val) == L7_SUCCESS)
  {
    form->value.active_cp = val;
    form->status.active_cp |= (EW_FORM_INITIALIZED);
  }

  /* supported users in system */
  if (usmDbCpdmGlobalSupportedUsersGet(&val) == L7_SUCCESS)
  {
    form->value.supported_users1 = val;
    form->status.supported_users1 |= (EW_FORM_INITIALIZED);

    form->value.supported_users2 = val;
    form->status.supported_users2 |= (EW_FORM_INITIALIZED);
  }

  /* supported local users in system */
  if (usmDbCpdmGlobalLocalUsersGet(&val) == L7_SUCCESS)
  {
    form->value.local_users1 = val;
    form->status.local_users1 |= (EW_FORM_INITIALIZED);

    form->value.local_users2 = val;
    form->status.local_users2 |= (EW_FORM_INITIALIZED);
  }

  /* configured local users in system */
  if (usmDbCpdmUserEntryCountGet(&val) == L7_SUCCESS)
  {
    form->value.local_configured_users1 = val;
    form->status.local_configured_users1 |= (EW_FORM_INITIALIZED);

    form->value.local_configured_users2 = val;
    form->status.local_configured_users2 |= (EW_FORM_INITIALIZED);
  }

  /* authenticated users */
  if (usmDbCpdmGlobalAuthenticatedUsersGet(&val) == L7_SUCCESS)
  {
    form->value.auth_users1 = val;
    form->status.auth_users1 |= (EW_FORM_INITIALIZED);

    form->value.auth_users2 = val;
    form->status.auth_users2 |= (EW_FORM_INITIALIZED);
  }

  form->value.Refresh = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext              context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpGlobalStatusP form    @b{(input)} pointer to the form
*
* @returns  cp_global_status.html
*
* @end
*********************************************************************/
L7_char8 *ewaFormSubmit_cpGlobalStatus(EwsContext context, EwaForm_cpGlobalStatusP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));

  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPGlobalStatus);
}

