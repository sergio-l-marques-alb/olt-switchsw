/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_global_cfg.c
*
* @purpose    Code in support of the cp_global_cfg.html page
*
* @component  Captive Portal
*
* @comments
*
* @create     7/2/2007
*
* @author     rjindal
*
* @end
*
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include "l7_common.h"
#include "ew_proto.h"
#include "ewnet.h"
#include "web.h"
#include "web_oem.h"
#include "strlib_security_web.h"
#include "usmdb_cpdm_api.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"
#include "sslt_exports.h"

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "util_pstring.h"


#ifdef L7_MGMT_SECURITY_PACKAGE
#include "usmdb_sslt_api.h"
#include "usmdb_sim_api.h"
#endif

/*********************************************************************
* @purpose  Get the input range help string
*
* @param    L7_int32 min @b{(input)} minimum value in range
* @param    L7_int32 max @b{(input)} maximum value in range
*
* @returns  L7_char8 *buf
*
* @end
*********************************************************************/
L7_char8 *usmWebIntegerRangeDisableHelp(L7_int32 min, L7_int32 max)
{
  static L7_char8 buf[40];
  osapiSnprintf(buf, sizeof(buf), " (%d %s %d, 0 - %s)", min, pStrInfo_common_To_5, max, pStrInfo_common_Dsbl_1);
  return buf;
}

/*********************************************************************
* @purpose  Set the form integer value after testing its validity
*
* @param    L7_uchar8        formStatus @b{(input)} form status
* @param    L7_uint32        formValue  @b{(input)} form value
* @param    L7_char8         *pStrInfo  @b{(input)} info string 
* @param    L7_uint32        minValue   @b{(input)} min value
* @param    L7_uint32        maxValue   @b{(input)} max value
* @param    usmWeb_AppInfo_t *appInfo   @b{(input)} pointer to applet
* @param    L7_RC_t          *funcPtr   @b{(input)} pointer to function
*
* @returns  none
*
* @end
*********************************************************************/
static void cpIntegerFieldSetHelp(L7_uchar8 formStatus, L7_uint32 formValue, 
                                  L7_char8 *pStrInfo, L7_uint32 minValue, L7_uint32 maxValue, 
                                  usmWeb_AppInfo_t *appInfo, L7_RC_t (* funcPtr)(L7_uint32))
{
  L7_char8 errMsg[USMWEB_ERROR_MSG_SIZE];

  if (formStatus & EW_FORM_PARSE_ERROR)
  {
    osapiSnprintfAddBlanks(0, 1, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg),
                           pStrErr_common_FieldInvalidFmt, pStrInfo);
  }
  else if ((formValue < minValue) || (formValue > maxValue))
  {
    osapiSnprintfAddBlanks(0, 1, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg),
                           pStrErr_common_FieldOutOfRange, (L7_int32)formValue, pStrInfo, minValue, maxValue);
  }
  else
  {
    if ((*funcPtr)(formValue) == L7_SUCCESS)
    {
      return;
    }
    osapiSnprintfAddBlanks(0, 1, 0, 0, pStrErr_common_FailedToSet, errMsg, sizeof(errMsg),
                           pStrErr_common_FailedToSet_1, pStrInfo);
  }

  appInfo->err.err_flag = L7_TRUE;
  osapiStrncat(appInfo->err.msg, errMsg, (sizeof(appInfo->err.msg)-strlen(appInfo->err.msg)-1));
  return;
}

/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext           context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpGlobalCfgP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpGlobalCfg(EwsContext context, EwaForm_cpGlobalCfgP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 mode, val;
  L7_CP_MODE_STATUS_t status;
  L7_CP_MODE_REASON_t reason;

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

  /* cp mode */
  if (usmDbCpdmGlobalModeGet(&mode) == L7_SUCCESS)
  {
    if (mode == L7_ENABLE)
    {
      form->value.cp_mode = L7_TRUE;
    }
    else
    {
      form->value.cp_mode = L7_FALSE;
    }
  }

  /* cp global status */
  if (usmDbCpdmGlobalStatusModeGet(&status) == L7_SUCCESS)
  {
    switch (status)
    {
    case L7_CP_MODE_ENABLED:
      form->value.cp_status = (char *)osapiStrDup(pStrInfo_common_Enbld);
      break;
    case L7_CP_MODE_ENABLE_PENDING:
      form->value.cp_status = (char *)osapiStrDup(pStrInfo_security_CPModeEnablePending);
      break;
    case L7_CP_MODE_DISABLED:
      form->value.cp_status = (char *)osapiStrDup(pStrInfo_common_Dsbld);
      break;
    case L7_CP_MODE_DISABLE_PENDING:
      form->value.cp_status = (char *)osapiStrDup(pStrInfo_security_CPModeDisablePending);
      break;
    default:
      form->value.cp_status = (char *)osapiStrDup(pStrInfo_common_Dsbld);
      break;
    }
    form->status.cp_status |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* cp global disable reason */
  if ((status == L7_CP_MODE_DISABLED) || (status == L7_CP_MODE_DISABLE_PENDING))
  {
    if (usmDbCpdmGlobalCPDisableReasonGet(&reason) == L7_SUCCESS)
    {
      switch (reason)
      {
      case L7_CP_MODE_REASON_ADMIN:
        form->value.cp_disable_reason = (char *)osapiStrDup(pStrInfo_security_CPModeReasonAdmin);
        break;
      case L7_CP_MODE_REASON_NO_IP_ADDR:
        form->value.cp_disable_reason = (char *)osapiStrDup(pStrInfo_security_CPModeReasonNoIpAddr);
        break;
      case L7_CP_MODE_REASON_ROUTING_ENABLED_NO_IP_RT_INTF:
        form->value.cp_disable_reason = (char *)osapiStrDup(pStrInfo_security_CPModeReasonRoutingNoIpRoutingIntf);
        break;
      case L7_CP_MODE_REASON_ROUTING_DISABLED:
        form->value.cp_disable_reason = (char *)osapiStrDup(pStrInfo_security_CPModeReasonRoutingDisabled);
        break;
      default:
        form->value.cp_disable_reason = (char *)osapiStrDup("");
        break;
      }
      form->status.cp_disable_reason |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
  }

  /* additional http port */
  if (usmDbCpdmGlobalHTTPPortGet(&val) == L7_SUCCESS)
  {
    form->value.http_port = val;
    form->status.http_port |= (EW_FORM_INITIALIZED);
  }

#ifdef L7_MGMT_SECURITY_PACKAGE
  /* optional http secure port */
  if (usmDbCpdmGlobalHTTPSecurePort2Get(&val) == L7_SUCCESS)
  {
    form->value.http_secure_port = val;
    form->status.http_secure_port |= (EW_FORM_INITIALIZED);
  }
#endif

  /* peer switch statistics reporting interval */
  if (usmDbCpdmGlobalStatusPeerSwStatsReportIntervalGet(&val) == L7_SUCCESS)
  {
    form->value.ps_stat_int = val;
    form->status.ps_stat_int |= (EW_FORM_INITIALIZED);
  }

  /* auth session timeout */
  if (usmDbCpdmGlobalStatusSessionTimeoutGet(&val) == L7_SUCCESS)
  {
    form->value.auth_timeout = val;
    form->status.auth_timeout |= (EW_FORM_INITIALIZED);
  }

  form->value.Submit = (char *) osapiStrDup(pStrInfo_common_Submit);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Refresh = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext           context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpGlobalCfgP form    @b{(input)} pointer to the form
*
* @returns  cp_global_cfg.html
*
* @end
*********************************************************************/
L7_char8 *ewaFormSubmit_cpGlobalCfg(EwsContext context, EwaForm_cpGlobalCfgP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 errMsg[USMWEB_ERROR_MSG_SIZE];
  L7_uint32 val = 0;
  L7_uint32 port;
#ifdef L7_MGMT_SECURITY_PACKAGE
  L7_uint32 defaultSecurePort;
  L7_uint32 additionalSecurePort;
  L7_uint32 adminSecurePort;
  L7_uint32 unit = usmDbThisUnitGet();
#endif

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));

  if (form->status.Submit & EW_FORM_RETURNED)
  {
    appInfo.err.err_flag = L7_FALSE;

    /* cp mode */
    if (form->value.cp_mode == TRUE)
    {
      val = L7_ENABLE; 
    } 
    else
    {
      val = L7_DISABLE;
    }

    if (usmDbCpdmGlobalModeSet(val) != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg(&appInfo, L7_NULLPTR, 0, 0, pStrErr_common_ErrCouldNot, errMsg, sizeof(errMsg), 
                             pStrInfo_security_EnableCP);
    }

    if (L7_SUCCESS != usmDbCpdmGlobalHTTPPortGet(&port))
    {
      port = CP_HTTP_PORT_MIN;
    }

#ifdef L7_MGMT_SECURITY_PACKAGE
    if (L7_SUCCESS != usmDbCpdmGlobalHTTPSecurePort1Get(&defaultSecurePort))
    {
      defaultSecurePort = L7_SSLT_SECURE_PORT;
    }
    if (L7_SUCCESS != usmDbCpdmGlobalHTTPSecurePort2Get(&additionalSecurePort))
    {
      additionalSecurePort = FD_CP_HTTP_AUX_SECURE_PORT2;
    }
    if (L7_SUCCESS != usmDbssltSecurePortGet(unit,&adminSecurePort))
    {
      adminSecurePort = FD_SSLT_SECURE_PORT;
    }
#endif

    /* additional http port */
    if ((form->value.http_port != 0) &&
        (form->value.http_port != port) &&
        ((form->value.http_port == CP_STANDARD_HTTP_PORT)
#ifdef L7_MGMT_SECURITY_PACKAGE
         || (form->value.http_port == defaultSecurePort)
         || (form->value.http_port == additionalSecurePort)
         || (form->value.http_port == adminSecurePort)
#endif
         ))
    {
      usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_FailedToSet, 
                             pStrErr_common_PortAlreadyInUse, 
                             pStrInfo_security_HTTPPort,
                             (L7_int32)form->value.http_port);
    }
    else
    {
      cpIntegerFieldSetHelp(form->status.http_port, form->value.http_port, 
                            pStrInfo_security_HTTPPort, 
                            CP_HTTP_PORT_MIN, CP_HTTP_PORT_MAX, 
                            &appInfo, usmDbCpdmGlobalHTTPPortSet);
      if (appInfo.err.err_flag != L7_TRUE)
      {
        port = form->value.http_port;
      }
    }

#ifdef L7_MGMT_SECURITY_PACKAGE
    /* additional http secure port */
    {
      if (appInfo.err.err_flag != L7_TRUE)
      {
        if ((form->value.http_secure_port != 0) &&
            (form->value.http_secure_port != additionalSecurePort) &&
            ((form->value.http_secure_port == CP_STANDARD_HTTP_PORT) ||
             (form->value.http_secure_port == port) ||
             (form->value.http_secure_port == defaultSecurePort) ||
             (form->value.http_secure_port == adminSecurePort)
             ))
        {
          usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_FailedToSet, 
                                 pStrErr_common_PortAlreadyInUse, 
                                 pStrInfo_security_HTTPSecurePort,
                                 (L7_int32)form->value.http_secure_port);
        }
        else
        {
          cpIntegerFieldSetHelp(form->status.http_secure_port, form->value.http_secure_port,
                                pStrInfo_security_HTTPSecurePort,
                                CP_HTTP_SECURE_PORT_MIN, CP_HTTP_SECURE_PORT_MAX,
                                &appInfo, usmDbCpdmGlobalHTTPSecurePort2Set);
          if (appInfo.err.err_flag != L7_TRUE)
          {
            additionalSecurePort = form->value.http_secure_port;
          }
        }
      }
    }
#endif

    /* peer switch statistics reporting interval */
    if (form->value.ps_stat_int == 0)
    {
      if (usmDbCpdmGlobalStatusPeerSwStatsReportIntervalSet(form->value.ps_stat_int) != L7_SUCCESS)
      {
        usmWebAppInfoCatErrMsg(&appInfo, L7_NULLPTR, 0, 0, pStrErr_common_FailedToSet, errMsg, sizeof(errMsg),
                               pStrErr_common_FailedToSet_1, pStrInfo_security_PeerSwStatReportInterval);
      }
    }
    else
    {
      cpIntegerFieldSetHelp(form->status.ps_stat_int, form->value.ps_stat_int, 
                            pStrInfo_security_PeerSwStatReportInterval, 
                            CP_PS_STATS_REPORT_INTERVAL_MIN, CP_PS_STATS_REPORT_INTERVAL_MAX, 
                            &appInfo, usmDbCpdmGlobalStatusPeerSwStatsReportIntervalSet);
    }
    /* auth session timeout */
    cpIntegerFieldSetHelp(form->status.auth_timeout, form->value.auth_timeout, 
                          pStrInfo_security_AuthSessionTimeout, 
                          CP_AUTH_SESSION_TIMEOUT_MIN, CP_AUTH_SESSION_TIMEOUT_MAX, 
                          &appInfo, usmDbCpdmGlobalStatusSessionTimeoutSet);
  }

  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPGlobalCfg);
}

