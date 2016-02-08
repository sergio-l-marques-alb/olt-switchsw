/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_trap_cfg.c
*
* @purpose    Code in support of the cp_trap_cfg.html page
*
* @component  Captive Portal
*
* @comments
*
* @create     12/13/2007
*
* @author     rjindal
*
* @end
*
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include "l7_common.h"
#include "usmdb_trapmgr_api.h"
#include "ew_proto.h"
#include "ewnet.h"
#include "web.h"
#include "web_oem.h"
#include "strlib_security_web.h"
#include "captive_portal_commdefs.h"
#include "usmdb_cpdm_api.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"


/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext         context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpTrapCfgP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpTrapCfg(EwsContext context, EwaForm_cpTrapCfgP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 mode;

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

  /* cp trap mode */
  if (usmDbTrapCaptivePortalGet(&mode) == L7_SUCCESS)
  {
    if (mode == L7_ENABLE)
    {
      form->value.cp_trapMode = (char *)osapiStrDup(pStrInfo_common_Enbld);
    }
    else
    {
      form->value.cp_trapMode = (char *)osapiStrDup(pStrInfo_common_Dsbld);
    }
  }
  else
  {
    form->value.cp_trapMode = (char *)osapiStrDup(pStrInfo_common_Dsbld);
  }
  form->status.cp_trapMode |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  /* client authentication failure traps */
  if (usmDbCpdmGlobalTrapModeGet(CP_TRAP_AUTH_FAILURE, &mode) == L7_SUCCESS)
  {
    if (mode == L7_ENABLE)
    {
      form->value.cp_cltAuthFailureTrap = Enable;
    }
    else
    {
      form->value.cp_cltAuthFailureTrap = Disable;
    }
  }
  else
  {
    form->value.cp_cltAuthFailureTrap = Disable;
  }
  form->status.cp_cltAuthFailureTrap |= (EW_FORM_INITIALIZED);

  /* client connection traps */
  if (usmDbCpdmGlobalTrapModeGet(CP_TRAP_CLIENT_CONNECTED, &mode) == L7_SUCCESS)
  {
    if (mode == L7_ENABLE)
    {
      form->value.cp_cltConnectTrap = Enable;
    }
    else
    {
      form->value.cp_cltConnectTrap = Disable;
    }
  }
  else
  {
    form->value.cp_cltConnectTrap = Disable;
  }
  form->status.cp_cltConnectTrap |= (EW_FORM_INITIALIZED);

  /* client database full traps */
  if (usmDbCpdmGlobalTrapModeGet(CP_TRAP_CONNECTION_DB_FULL, &mode) == L7_SUCCESS)
  {
    if (mode == L7_ENABLE)
    {
      form->value.cp_cltDBFullTrap = Enable;
    }
    else
    {
      form->value.cp_cltDBFullTrap = Disable;
    }
  }
  else
  {
    form->value.cp_cltDBFullTrap = Disable;
  }
  form->status.cp_cltDBFullTrap |= (EW_FORM_INITIALIZED);

  /* client disconnection traps */
  if (usmDbCpdmGlobalTrapModeGet(CP_TRAP_CLIENT_DISCONNECTED, &mode) == L7_SUCCESS)
  {
    if (mode == L7_ENABLE)
    {
      form->value.cp_cltDisconnectTrap = Enable;
    }
    else
    {
      form->value.cp_cltDisconnectTrap = Disable;
    }
  }
  else
  {
    form->value.cp_cltDisconnectTrap = Disable;
  }
  form->status.cp_cltDisconnectTrap |= (EW_FORM_INITIALIZED);

  form->value.Submit = (char *) osapiStrDup(pStrInfo_common_Submit);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Refresh = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext         context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpTrapCfgP form    @b{(input)} pointer to the form
*
* @returns  cp_trap_cfg.html
*
* @end
*********************************************************************/
L7_char8 *ewaFormSubmit_cpTrapCfg(EwsContext context, EwaForm_cpTrapCfgP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 errMsg[USMWEB_ERROR_MSG_SIZE];
  L7_uint32 mode = 0;

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));

  if (form->status.Submit & EW_FORM_RETURNED)
  {
    appInfo.err.err_flag = L7_FALSE;

    /* client authentication failure traps */
    if (form->value.cp_cltAuthFailureTrap == Enable)
    {
      mode = L7_ENABLE; 
    } 
    else
    {
      mode = L7_DISABLE;
    }
    if (usmDbCpdmGlobalTrapModeSet(CP_TRAP_AUTH_FAILURE, mode) != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg(&appInfo, L7_NULLPTR, 0, 0, pStrErr_common_ErrCouldNot, errMsg, sizeof(errMsg), 
                             pStrInfo_security_EnableCPClientAuthFailureTrap);
    }

    /* client connection traps */
    if (form->value.cp_cltConnectTrap == Enable)
    {
      mode = L7_ENABLE; 
    } 
    else
    {
      mode = L7_DISABLE;
    }
    if (usmDbCpdmGlobalTrapModeSet(CP_TRAP_CLIENT_CONNECTED, mode) != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg(&appInfo, L7_NULLPTR, 0, 0, pStrErr_common_ErrCouldNot, errMsg, sizeof(errMsg), 
                             pStrInfo_security_EnableCPClientConnectionTrap);
    }

    /* client database full traps */
    if (form->value.cp_cltDBFullTrap == Enable)
    {
      mode = L7_ENABLE; 
    } 
    else
    {
      mode = L7_DISABLE;
    }
    if (usmDbCpdmGlobalTrapModeSet(CP_TRAP_CONNECTION_DB_FULL, mode) != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg(&appInfo, L7_NULLPTR, 0, 0, pStrErr_common_ErrCouldNot, errMsg, sizeof(errMsg), 
                             pStrInfo_security_EnableCPClientDBFullTrap);
    }

    /* client disconnection traps */
    if (form->value.cp_cltDisconnectTrap == Enable)
    {
      mode = L7_ENABLE; 
    } 
    else
    {
      mode = L7_DISABLE;
    }
    if (usmDbCpdmGlobalTrapModeSet(CP_TRAP_CLIENT_DISCONNECTED, mode) != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg(&appInfo, L7_NULLPTR, 0, 0, pStrErr_common_ErrCouldNot, errMsg, sizeof(errMsg), 
                             pStrInfo_security_EnableCPClientDisconnectionTrap);
    }
  }

  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPTrapCfg);
}

