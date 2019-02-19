/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_user_local.c
*
* @purpose    Code in support of the cp_user_local.html page
*
* @component  Captive Portal
*
* @comments
*
* @create     8/17/2007
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
#include "web_buffer.h"
#include "web_oem.h"
#include "strlib_security_web.h"
#include "usmdb_cpdm_user_api.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "util_pstring.h"


/*********************************************************************
* @purpose  Get the user entry as HTML data
*
* @param    user  @b{(input)} User name
* @param    idx   @b{(input)} Entry index
*
* @returns  Display buffer
*
* @end
*********************************************************************/
L7_char8 *usmWebCPUserEntryGet(L7_char8 *user, L7_uint32 idx)
{
  L7_char8 buf[USMWEB_APP_BUF_SIZE];
  L7_uint32 val;
  uId_t uId;

  usmDbCpdmUserEntryByNameGet(user, &uId);
  memset(buf,0,sizeof(buf));

  /* user */
  osapiSnprintfAddBlanks(1,0,0,1, L7_NULLPTR, webStaticContentBuffer, sizeof(webStaticContentBuffer),
                         pStrInfo_security_JsdataUser, idx, user);

  osapiStrncat(webStaticContentBuffer, buf, (sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer)-1));

  /* session timeout */
  if (usmDbCpdmUserEntrySessionTimeoutGet(uId, &val) == L7_SUCCESS)
  {
    osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_Session, val);
  }
  else
  {
    osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_SessionError);
  }
  osapiStrncat(webStaticContentBuffer, buf, (sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer)-1));

  /* idle timeout */
  if (usmDbCpdmUserEntryIdleTimeoutGet(uId, &val) == L7_SUCCESS)
  {
    osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_Idle, val);
  }
  else
  {
    osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_IdleError);
  }
  osapiStrncat(webStaticContentBuffer, buf, (sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer)-1));

  return webStaticContentBuffer;
}

/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext           context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpUserLocalP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpUserLocal(EwsContext context, EwaForm_cpUserLocalP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  form->value.err_msg = L7_NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->value.err_flag = L7_FALSE;
  form->status.err_flag |= (EW_FORM_INITIALIZED);

  net = ewsContextNetHandle(context);
  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    if (form->value.err_flag == L7_TRUE)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_DYNAMIC);
    }
  }

  form->value.Add = (char *)osapiStrDup(pStrInfo_common_Add);
  form->status.Add |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Delete = (char *)osapiStrDup(pStrInfo_common_Del);
  form->status.Delete |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Delete_all = (char *)osapiStrDup(pStrInfo_common_DelAll);
  form->status.Delete_all |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Refresh = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->value.refresh_in_progress = 0;
  form->status.refresh_in_progress |= (EW_FORM_INITIALIZED);
  form->value.refresh_key = 0;
  form->status.refresh_key |= (EW_FORM_INITIALIZED);
  form->status.selectedUsers |= (EW_FORM_INITIALIZED);
  form->value.selectedUsers = NULL;
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext           context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpUserLocalP form    @b{(input)} pointer to the form
*
* @returns  cp_user_local.html
*
* @end
*********************************************************************/
char *ewaFormSubmit_cpUserLocal(EwsContext context, EwaForm_cpUserLocalP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 *ptok;
  L7_char8 user[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_char8 errMsg[USMWEB_ERROR_MSG_SIZE];
  L7_char8 errBuf[USMWEB_BUFFER_SIZE_512];
  L7_char8 tmpBuf[USMWEB_APP_DATA_SIZE+1];
  L7_uint32 ptok_len, count=0, failCount=0;
  uId_t uId;

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  if (net->app_pointer == NULL)
  {
    ewsContextSendReply(context, pStrInfo_security_HtmlFileCPUserLocal);
    return L7_NULL;
  }
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;
  memset(errBuf, 0x00, sizeof(errBuf));
  memset(errMsg, 0x00, sizeof(errMsg));

  if (form->status.Add & EW_FORM_RETURNED)
  {
    memset(appInfo.buf, 0, CP_USER_LOCAL_USERNAME_MAX);
    appInfo.data[0] = FD_CP_LOCAL_USERS_MAX+1;
    return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPUserLocalCfg);
  }

  else if (form->status.Delete & EW_FORM_RETURNED)
  {
    if (form->value.refresh_key == 1)
    {
      ptok = strtok(form->value.selectedUsers, ";");
      while (ptok != L7_NULL)
      {
        count++;
        ptok_len = (L7_uint32)strlen(ptok);
        memset(user, 0, sizeof(user));
        memcpy(user, ptok, ptok_len);
        usmDbCpdmUserEntryByNameGet(user, &uId);
        if (usmDbCpdmUserEntryGet(uId) == L7_SUCCESS)
        {
          appInfo.err.err_flag = L7_TRUE;
          if (usmDbCpdmUserEntryDelete(uId) != L7_SUCCESS)
          {
            osapiSnprintf(errMsg, sizeof(errMsg), pStrInfo_security_DeleteUserFailed);
            osapiStrncat(appInfo.err.msg, errMsg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1));
          }
          else
          {
            if (failCount == 0)
            {
              osapiSnprintfAddBlanks(1,0,0,0, L7_NULLPTR, errMsg, sizeof(errMsg), pStrInfo_security_DeletedUsers);
            }
            osapiSnprintf(tmpBuf, sizeof(tmpBuf), "\r\n%s", ptok);
            osapiStrncat(errMsg, tmpBuf, (sizeof(errMsg)-strlen(errMsg)-1));
            failCount++;
          }
          usmDbCpdmUserGroupAssocEntryByUIdDeleteAll(uId);
        }
        ptok = strtok(L7_NULL, ";");
      }
      osapiStrncat(appInfo.err.msg, errMsg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1));
      if (count == 0)
      {
        usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_SelectCheckBox);
      }
    }
  }

  else if (form->status.Delete_all & EW_FORM_RETURNED)
  {
    if (form->value.refresh_key == 2)
    {
      appInfo.err.err_flag = L7_TRUE;
      if (usmDbCpdmUserEntryPurge() != L7_SUCCESS)
      {
        osapiSnprintfAddBlanks(0,1,0,0, pStrErr_common_Error, errMsg, sizeof(errMsg), 
                               pStrErr_common_FailedToDel, pStrInfo_security_UserEntries);
      }
      else
      {
        osapiSnprintfAddBlanks(1,0,0,0, L7_NULLPTR, errMsg, sizeof(errMsg), 
                               pStrInfo_security_UserEntriesAllDeleted);
      }
      osapiStrncat(appInfo.err.msg, errMsg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1));
    }
  }

  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPUserLocal);
}

