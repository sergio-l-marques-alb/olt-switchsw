/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_status.c
*
* @purpose    Code in support of the cp_status.html page
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
#include "ew_proto.h"
#include "ewnet.h"
#include "web.h"
#include "web_oem.h"
#include "strlib_security_web.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpim_api.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "captive_portal_commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"


/*********************************************************************
* @purpose  Get the list of all configured CP configurations
*
* @param    context   @b{(input)} EmWeb/Server request context handle
* @param    option    @b{(input)} dynamic Select Support
* @param    iterator  @b{(input)} pointer to the current iteration
*
* @returns
*
* @end
*********************************************************************/
void *ewaFormSelect_cp_id(EwsContext context, EwsFormSelectOptionP option, void *iterator)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 *bufChoice;
  static L7_ushort16 selected_cpId;
  static L7_ushort16 cpId;
  static L7_uint32 cpId32;
  static L7_BOOL flag;
  L7_char8 cpName[CP_NAME_MAX+1];
  L7_char8 buf[USMWEB_APP_BUF_SIZE];
  L7_RC_t rc = L7_FAILURE;

  if (iterator == L7_NULL)
  {
    flag = L7_FALSE;
    cpId = 0;
    selected_cpId = 0;

    rc = usmDbCpdmCPConfigNextGet(cpId, &cpId);

    net = ewsContextNetHandle(context);
    if (net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      selected_cpId = appInfo.data[1];
    }
    else
    {
      if (rc == L7_SUCCESS)
      {
        selected_cpId = cpId;
      }
    }
  }

  if (flag == L7_TRUE)
  {
    rc = usmDbCpdmCPConfigNextGet(cpId, &cpId);
  }
  else
  {
    flag = L7_TRUE;
  }

  if (rc == L7_SUCCESS)
  {
    cpId32 = (L7_uint32)cpId;
    option->valuep = (void *)&cpId32;
    bufChoice = ewsContextNetHandle(context)->buffer;
    memset(buf, 0, sizeof(buf));
    memset(cpName, 0, sizeof(cpName));
    if (usmDbCpdmCPConfigNameGet(cpId, cpName) == L7_SUCCESS)
    {
      osapiSnprintf(buf, APP_BUFFER_SIZE, "%u - %s", cpId, cpName);
    }
    osapiStrncpy(bufChoice, buf, APP_BUFFER_SIZE-1);
    bufChoice[APP_BUFFER_SIZE-1] = '\0';
    option->choice = bufChoice;

    if (cpId == selected_cpId)
    {
      option->selected = TRUE;
    }
    else
    {
      option->selected = FALSE;
    }
    return (void *) option;
  }

  return L7_NULL;
}

/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext        context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpStatusP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpStatus(EwsContext context, EwaForm_cpStatusP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_ushort16 selected_cpId;
  L7_CP_INST_OPER_STATUS_t status;
  L7_CP_INST_DISABLE_REASON_t reason;
  L7_CP_INST_BLOCK_STATUS_t block_status;
  L7_char8 data[USMWEB_APP_SMALL_DATA_SIZE];
  L7_char8 dataCpid[USMWEB_APP_SMALL_DATA_SIZE];
  L7_uint32 val, i, numBytes;

  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->value.err_flag = L7_FALSE;
  form->status.err_flag |= (EW_FORM_INITIALIZED);
  selected_cpId = 0;
  memset(data, 0, sizeof(data));
  memset(dataCpid, 0, sizeof(dataCpid));

  numBytes = ewsCGIQueryString(context, data, sizeof(data));
  if (numBytes > 0)
  {
    for (i=0; i<strlen(data); i++)
    {
      dataCpid[i] = data[i+3];
    }
    selected_cpId = atoi(dataCpid);
  }
  else
  {
    net = ewsContextNetHandle(context);
    if (net->app_pointer != NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      selected_cpId = appInfo.data[1];
      form->value.err_flag = appInfo.err.err_flag;
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_DYNAMIC);
    }
    else
    {
      if (usmDbCpdmCPConfigNextGet(selected_cpId, &selected_cpId) != L7_SUCCESS)
      {
        selected_cpId = 0;
      }
    }
  }

  if (selected_cpId != 0)
  {
    /* operational status */
    if (usmDbCpdmCPConfigOperStatusGet(selected_cpId, &status) == L7_SUCCESS)
    {
      switch (status)
      {
      case L7_CP_INST_OPER_STATUS_ENABLED:
        form->value.oper_status = (char *)osapiStrDup(pStrInfo_common_Enbld);
        break;
      case L7_CP_INST_OPER_STATUS_DISABLED:
        form->value.oper_status = (char *)osapiStrDup(pStrInfo_common_Dsbld);
        break;
      default:
        form->value.oper_status = (char *)osapiStrDup("");
        break;
      }
      form->status.oper_status |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    /* disable reason */
    if (status == L7_CP_INST_OPER_STATUS_DISABLED)
    {
      if (usmDbCpdmCPConfigDisableReasonGet(selected_cpId, &reason) == L7_SUCCESS)
      {
        switch (reason)
        {
        case L7_CP_INST_DISABLE_REASON_ADMIN:
          form->value.disable_reason = (char *)osapiStrDup(pStrInfo_security_CPModeReasonAdmin);
          break;
        case L7_CP_INST_DISABLE_REASON_NO_RADIUS_SERVER:
          form->value.disable_reason = (char *)osapiStrDup(pStrInfo_security_NoRadiusServer);
          break;
        case L7_CP_INST_DISABLE_REASON_NO_ACCT_SERVER:
          form->value.disable_reason = (char *)osapiStrDup(pStrInfo_security_NoAcctServer);
          break;
        case L7_CP_INST_DISABLE_REASON_NOT_ASSOC_INTF:
          form->value.disable_reason = (char *)osapiStrDup(pStrInfo_security_NoAssocWithIntf);
          break;
        case L7_CP_INST_DISABLE_REASON_NO_ACTIVE_INTF:
          form->value.disable_reason = (char *)osapiStrDup(pStrInfo_security_NoValidActiveIntf);
          break;
      case L7_CP_INST_DISABLE_REASON_NO_VALID_CERT:
          form->value.disable_reason = (char *)osapiStrDup(pStrInfo_security_NoValidCert);
          break;
        default:
          form->value.disable_reason = (char *)osapiStrDup("");
          break;
        }
        form->status.disable_reason |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
      }
    }

    /* block status */
    if (usmDbCpdmCPConfigBlockedStatusGet(selected_cpId, &block_status) == L7_SUCCESS)
    {
      switch(block_status)
      {
        case L7_CP_INST_BLOCK_STATUS_BLOCKED:
          form->value.block_status = (char *)osapiStrDup(pStrInfo_security_Blocked);
          break;
        case L7_CP_INST_BLOCK_STATUS_BLOCKED_PENDING:
          form->value.block_status = 
            (char *)osapiStrDup(pStrInfo_security_BlockedPending);
          break;
        case L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED:
          form->value.block_status = (char *)osapiStrDup(pStrInfo_security_NotBlocked);
          break;
        case L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED_PENDING:
          form->value.block_status = 
            (char *)osapiStrDup(pStrInfo_security_NotBlockedPending);
          break;
        default:
          form->value.block_status = (char *)osapiStrDup("???"); /* should NEVER happen */
          break;
      }
      form->status.block_status |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    /* authenticated users */
    if (usmDbCpdmCPConfigAuthenticatedUsersGet(selected_cpId, &val) == L7_SUCCESS)
    {
      form->value.cp_auth_users = val;
      form->status.cp_auth_users |= (EW_FORM_INITIALIZED);
    }
  }

  form->value.refresh_in_progress = 0;
  form->status.refresh_in_progress |= (EW_FORM_INITIALIZED);
  form->value.refresh_key = 0;
  form->status.refresh_key |= (EW_FORM_INITIALIZED);
  form->value.Block = (char *) osapiStrDup(pStrInfo_common_Block);
  form->status.Block |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Unblock = (char *) osapiStrDup(pStrInfo_common_Unblock);
  form->status.Unblock |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Refresh = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext        context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpStatusP form    @b{(input)} pointer to the form
*
* @returns  cp_status.html
*
* @end
*********************************************************************/
L7_char8 *ewaFormSubmit_cpStatus(EwsContext context, EwaForm_cpStatusP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  cpId_t cpId;
  L7_CP_INST_BLOCK_STATUS_t blockStatus;

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  /*
  * Note: the following Block/Unblock functionality is NOT optimal (for a couple of reasons);
  * 1)  It is inefficient to sent Block/Unblock commands for each interface.
  *     It would be better if we told the AP in a single command.
  * 2)  We really don't know for sure if the Block/Unblock has succeeded for not because this is
  *     determined in the callback. We don't check each callback to determine success or not.
  */
  if (form->status.Block & EW_FORM_RETURNED)
  {
    if (form->value.refresh_key == 1)
    {
      /* Block all associated interfaces */
      appInfo.err.err_flag = L7_FALSE;
      cpId = form->value.cp_id;

      /* Set CP Block status */
      blockStatus = (L7_TRUE==appInfo.err.err_flag)?L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED_PENDING:L7_CP_INST_BLOCK_STATUS_BLOCKED_PENDING;
      usmDbCpdmCPConfigBlockedStatusSet(cpId,blockStatus);
    }
  }
  else if (form->status.Unblock & EW_FORM_RETURNED)
  {
    /* Unblock all associated interfaces */
    appInfo.err.err_flag = L7_FALSE;
    cpId = form->value.cp_id;

    /* Set CP Block status */
    blockStatus = (L7_TRUE==appInfo.err.err_flag)?L7_CP_INST_BLOCK_STATUS_BLOCKED_PENDING:L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED_PENDING;
    usmDbCpdmCPConfigBlockedStatusSet(cpId,blockStatus);
  }

  appInfo.data[1] = form->value.cp_id;
  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPStatus);
}

