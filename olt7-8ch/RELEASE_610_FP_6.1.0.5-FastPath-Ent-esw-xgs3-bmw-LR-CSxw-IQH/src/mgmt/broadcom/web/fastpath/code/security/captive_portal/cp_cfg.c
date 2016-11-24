/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_cfg.c
*
* @purpose    Code in support of the cp_cfg.html page
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
#include "usmdb_cpdm_web_api.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"

static L7_char8 usmWebBigBuf[USMWEB_BUFFER_SIZE_1024];

/*********************************************************************
* @purpose  Get the total number of CP configurations
*
* @param    void
*
* @returns  Number of CP configurations
*
* @end
*********************************************************************/
L7_uint32 *usmCPConfigListCount()
{
  static L7_uint32 cpCount;
  cpId_t cpId = 0;

  cpCount = 0;
  while (usmDbCpdmCPConfigNextGet(cpId, &cpId) == L7_SUCCESS)
  {
    cpCount++;
  }

  return &cpCount;
}

/*********************************************************************
* @purpose  Display summary of existing CP configurations
*
* @param    cpId_t  cpId  @b{(input)} CP ID
*
* @returns  L7_char8 *buf
*
* @note     A typical row contains a checkbox for which its name is the CP ID. 
*           This is used to parse the row to determine which CP is checked for deletion.
*
* @end
*********************************************************************/
L7_char8 *usmWebCPConfigurationGet(cpId_t cpId)
{
  L7_char8 buf[USMWEB_BUFFER_SIZE_256];
  L7_char8 name[USMWEB_APP_DATA_SIZE+1];
  L7_CP_MODE_STATUS_t mode;
  L7_LOGIN_TYPE_t protocol;
  CP_VERIFY_MODE_t verifyMode;
  webId_t webId;
  L7_uint32 idx;

  memset(usmWebBigBuf, 0, sizeof(usmWebBigBuf));
  memset(name, 0, sizeof(name));

  /* configuration */
  if (L7_SUCCESS != usmDbCpdmCPConfigNameGet(cpId, name))
  {
    return usmWebBigBuf;
  }

  osapiSnprintf(usmWebBigBuf, sizeof(usmWebBigBuf), 
                "<TD CLASS=\"rowdataattributes\" emweb:/USMWEB_BUTTON_ALIGN; nowrap name=\"cpConfig_id\"> \
				 <a href=\"cp_cfg_mgmt.html?cp=%d\">%d-%s</a></TD>\n", cpId, cpId, name);

  /* mode */
  if (usmDbCpdmCPConfigModeGet(cpId, &mode) == L7_SUCCESS)
  {
    if (mode == L7_CP_MODE_ENABLED)
    {
      osapiSnprintf(buf, sizeof(buf), "<TD CLASS=\"rowdataattributes\" name=\"cp_mode\">%s</TD>", 
                    pStrInfo_common_Enbl_1);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), "<TD CLASS=\"rowdataattributes\" name=\"cp_mode\">%s</TD>", 
                    pStrInfo_common_Dsbl_1);
    }
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "<TD CLASS=\"rowdataattributes\"></TD>");
  }
  osapiStrncat(usmWebBigBuf, buf, (sizeof(usmWebBigBuf)-strlen(usmWebBigBuf)-1));

  /* protocol */
  if (usmDbCpdmCPConfigProtocolModeGet(cpId, &protocol) == L7_SUCCESS)
  {
    if (protocol == L7_LOGIN_TYPE_HTTP)
    {
      osapiSnprintf(buf, sizeof(buf), "<TD CLASS=\"rowdataattributes\" name=\"cp_protocol\">%s</TD>", 
                    pStrInfo_security_ProtocolHTTP);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), "<TD CLASS=\"rowdataattributes\" name=\"cp_protocol\">%s</TD>", 
                    pStrInfo_security_ProtocolHTTPS);
    }
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "<TD CLASS=\"rowdataattributes\"></TD>");
  }
  osapiStrncat(usmWebBigBuf, buf, (sizeof(usmWebBigBuf)-strlen(usmWebBigBuf)-1));

  /* verification */
  if (usmDbCpdmCPConfigVerifyModeGet(cpId, &verifyMode) == L7_SUCCESS)
  {
    if (verifyMode == CP_VERIFY_MODE_GUEST)
    {
      osapiSnprintf(buf, sizeof(buf), "<TD CLASS=\"rowdataattributes\" name=\"cp_verify\">%s</TD>", 
                    pStrInfo_security_VerificationModeGuest);
    }
    else if (verifyMode == CP_VERIFY_MODE_LOCAL)
    {
      osapiSnprintf(buf, sizeof(buf), "<TD CLASS=\"rowdataattributes\" name=\"cp_verify\">%s</TD>", 
                    pStrInfo_security_VerificationModeLocal);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), "<TD CLASS=\"rowdataattributes\" name=\"cp_verify\">%s</TD>", 
                    pStrInfo_security_VerificationModeRadius);
    }
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "<TD CLASS=\"rowdataattributes\"></TD>");
  }
  osapiStrncat(usmWebBigBuf, buf, (sizeof(usmWebBigBuf)-strlen(usmWebBigBuf)-1));

  /* locales */
  idx = 0;
  webId = 0;
  while (L7_SUCCESS == usmDbCpdmCPConfigWebIdNextGet(cpId,webId,&webId))
  {
    idx++;
  }
  osapiSnprintf(buf, sizeof(buf), "<TD CLASS=\"rowdataattributes\" name=\"cp_locales\">%d</TD>",idx);
  osapiStrncat(usmWebBigBuf, buf, (sizeof(usmWebBigBuf)-strlen(usmWebBigBuf)-1));

  osapiStrncat(usmWebBigBuf, "</TR>", (sizeof(usmWebBigBuf)-strlen(usmWebBigBuf)-1));
  return usmWebBigBuf;
}

/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext     context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpCfgP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpCfg(EwsContext context, EwaForm_cpCfgP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 i;
  cpId_t cpId;

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

  i = 0;
  cpId = 0;
  while (usmDbCpdmCPConfigNextGet(cpId, &cpId) == L7_SUCCESS)
  {
    form->value.cpConfigs[i].cpConfigChk = L7_FALSE;
    form->status.cpConfigs[i].cpConfigChk |= EW_FORM_INITIALIZED;
    form->value.cpConfigs[i].cpConfig_id = (L7_uint32)cpId;
    form->status.cpConfigs[i].cpConfig_id |= EW_FORM_INITIALIZED;
    i++;
  }

  form->value.Add = (char *) osapiStrDup(pStrInfo_common_Add);
  form->status.Add |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Delete = (char *) osapiStrDup(pStrInfo_common_Del);
  form->status.Delete |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Refresh = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->status.cpConfig_name |= (EW_FORM_INITIALIZED);
  form->value.refresh_in_progress = 0;
  form->status.refresh_in_progress |= (EW_FORM_INITIALIZED);
  form->value.refresh_key = 0;
  form->status.refresh_key |= (EW_FORM_INITIALIZED);
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext     context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpCfgP form    @b{(input)} pointer to the form
*
* @returns  cp_cfg.html
*
* @end
*********************************************************************/
L7_char8 *ewaFormSubmit_cpCfg(EwsContext context, EwaForm_cpCfgP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 buf[CP_NAME_MAX+1];
  L7_uint32 i;
  cpId_t cpId = 0;
  L7_RC_t rc;

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  if (net->app_pointer == NULL)
  {
    ewsContextSendReply(context, pStrInfo_security_HtmlFileCPCfg);
    return L7_NULL;
  }

  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  if (form->status.Add & EW_FORM_RETURNED)
  {
    memset(buf, 0, sizeof(buf));
    memcpy(buf, form->value.cpConfig_name, strlen(form->value.cpConfig_name));
    if (usmDbStringAlphaNumericCheck(buf) != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_CPNameMustContainAlnumCharsOnly);
    }
    if (appInfo.err.err_flag == L7_FALSE)
    {
      rc = L7_SUCCESS;
      cpId = CP_ID_MIN;
      while ((rc == L7_SUCCESS) && (cpId <= CP_ID_MAX))
      {
        rc = usmDbCpdmCPConfigGet(cpId);
        if (rc == L7_SUCCESS)
        {
          cpId++;
        }
      }
      if (cpId > CP_ID_MAX)
      {
        usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrErr_common_FailedToAdd, 
                               pStrInfo_security_CPConfig);
      }
    }

    if (appInfo.err.err_flag == L7_FALSE)
    {
      if (usmDbCpdmCPConfigAdd(cpId) != L7_SUCCESS)
      {
        usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrErr_common_FailedToAdd, 
                               pStrInfo_security_CPConfig);
      }
    }

    if (appInfo.err.err_flag == L7_FALSE)
    {
      /* set CP name if specified, as the application supports CP creation without a name */
      if (osapiStrncmp(buf, pStrInfo_common_EmptyString, sizeof(buf)) != 0)
      {
        if (usmDbCpdmCPConfigNameSet(cpId, buf) != L7_SUCCESS)
        {
          usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_FailedToSet, pStrErr_common_FailedToSet_1, 
                                 pStrInfo_security_CPName);
        }
      }
    }

    if (appInfo.err.err_flag == L7_FALSE)
    {
      appInfo.data[0] = (L7_uint32)cpId;
      return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPCfgMgmt);
    }
  }

  else if (form->status.Delete & EW_FORM_RETURNED)
  {
    if (form->value.refresh_key == 1)
    {
      for (i=1; i<form->value.cpConfigs_repeats; i++)
      {
        if (form->value.cpConfigs[i].cpConfigChk == L7_TRUE)
        {
          if (appInfo.err.err_flag == L7_FALSE)
          {
            if (usmDbCpdmCPConfigDelete(form->value.cpConfigs[i].cpConfig_id) != L7_SUCCESS)
            {
              usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrErr_common_FailedToDel, 
                                     pStrInfo_security_CPConfig);
              break;
            }
          }
        }
      }
    }
  }

  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPCfg);
}

