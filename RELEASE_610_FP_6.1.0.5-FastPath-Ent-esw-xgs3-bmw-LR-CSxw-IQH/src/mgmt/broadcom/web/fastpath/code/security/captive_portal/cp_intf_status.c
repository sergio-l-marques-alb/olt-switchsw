/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_intf_status.c
*
* @purpose    Code in support of the cp_intf_status.html page
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



/* Using a quick wrapper function to accomodate a new API */
static
L7_RC_t cpdmIntIfNumNextGet(cpId_t cpId, L7_uint32 intIfNum, L7_uint32 * pIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 nextIf = 0;
  cpId_t  next_cpId = 0;

  rc = usmDbCpdmCPConfigIntIfNumNextGet(cpId, intIfNum, &next_cpId, &nextIf);
  if (L7_SUCCESS == rc)
  {
    if (next_cpId != cpId)
      rc = L7_FAILURE;
    else
      *pIfNum = nextIf;
  }
  return rc;
}


static
void *get_if_id(EwsContext context, EwsFormSelectOptionP option, 
                void *iterator, L7_BOOL bOnlyAssoced)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 *bufChoice;
  static L7_uint32 ifId;
  static L7_uint32 selected_ifId;
  static L7_BOOL flag;
  L7_uchar8 ifDescription[CP_INTF_DESCRIPTION_MAX_LEN+1];
  L7_char8 buf[USMWEB_APP_BUF_SIZE];
  L7_RC_t rc = L7_FAILURE;

  if (iterator == L7_NULL)
  {
    flag = L7_FALSE;
    ifId = 0;
    selected_ifId = 0;

    rc = usmDbCpimIntfNextGet(ifId, &ifId);
    if ((L7_SUCCESS == rc) && (L7_TRUE == bOnlyAssoced))
    {
      cpId_t ignored = 0;
      while (L7_SUCCESS != usmDbCpdmCPConfigIntIfNumFind(ifId, &ignored))
      {
        rc = usmDbCpimIntfNextGet(ifId, &ifId);  
        if (L7_SUCCESS != rc)
          break;
      }
    }

    net = ewsContextNetHandle(context);
    if (net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      selected_ifId = appInfo.data[0];
    }
    else
    {
      if (rc == L7_SUCCESS)
      {
        selected_ifId = ifId;
      }
    }
  }

  if (flag == L7_TRUE)
  {
    rc = usmDbCpimIntfNextGet(ifId, &ifId);
    if ((L7_SUCCESS == rc) && (L7_TRUE == bOnlyAssoced))
    {
      cpId_t ignored = 0;
      while (L7_SUCCESS != usmDbCpdmCPConfigIntIfNumFind(ifId, &ignored))
      {
        rc = usmDbCpimIntfNextGet(ifId, &ifId);  
        if (L7_SUCCESS != rc)
          break;
      }
    }
  }
  else
  {
    flag = L7_TRUE;
  }

  if (rc == L7_SUCCESS)
  {
    option->valuep = (void *)&ifId;
    bufChoice = ewsContextNetHandle(context)->buffer;
    memset(buf, 0, sizeof(buf));
    if (usmDbCpimIntfCapabilityDescriptionGet(ifId, ifDescription) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf), "%s", ifDescription);
    }
    osapiSnprintf(bufChoice, APP_BUFFER_SIZE, "%s", buf);
    option->choice = bufChoice;

    if (selected_ifId == ifId)
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
* @purpose  Get the list of all interfaces
*
* @param    context   @b{(input)} EmWeb/Server request context handle
* @param    option    @b{(input)} dynamic Select Support
* @param    iterator  @b{(input)} pointer to the current iteration
*
* @returns
*
* @end
*********************************************************************/
void *ewaFormSelect_if_id(EwsContext context, EwsFormSelectOptionP option, void *iterator)
{
  return get_if_id(context, option, iterator, L7_FALSE);
}

/*********************************************************************
* @purpose  Get the list of all interfaces
*
* @param    context   @b{(input)} EmWeb/Server request context handle
* @param    option    @b{(input)} dynamic Select Support
* @param    iterator  @b{(input)} pointer to the current iteration
*
* @returns
*
* @end
*********************************************************************/
void *ewaFormSelect_assoced_if_id(EwsContext context, EwsFormSelectOptionP option, 
                                  void *iterator)
{
  return get_if_id(context, option, iterator, L7_TRUE);
}





/*********************************************************************
* @purpose  Get the list of all associated interfaces for a given CP
*
* @param    context   @b{(input)} EmWeb/Server request context handle
* @param    option    @b{(input)} dynamic Select Support
* @param    iterator  @b{(input)} pointer to the current iteration
*
* @returns
*
* @end
*********************************************************************/

void *ewaFormSelect_cpAssocIntf(EwsContext context, EwsFormSelectOptionP option, void *iterator)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 *bufChoice;
  static L7_uint32 selected_ifId;
  static L7_uint32 ifId;
  static cpId_t cpId;
  L7_char8 qStr[256];
  L7_char8 *pStr, *pStr2=NULL;
  L7_uchar8 ifDescription[CP_INTF_DESCRIPTION_MAX_LEN+1];
  L7_char8 buf[USMWEB_APP_BUF_SIZE];
  L7_RC_t rc = L7_FAILURE;

  if (iterator == L7_NULL)
  {
    selected_ifId = 0;
    ifId = 0;
    cpId = CP_ID_MIN;

    rc = cpdmIntIfNumNextGet(cpId, ifId, &ifId);

    if (ewsCGIQueryString(context, qStr, 256) > 0)
    {
      pStr = strstr(qStr, "cp=");
      if (pStr != L7_NULLPTR)
      {
        pStr += strlen("cp=");
        pStr2 = strtok(pStr, "&");
        if (pStr2==NULL)
        {
          rc = L7_FAILURE;
        }
        else
        {
          cpId = atoi(pStr2);
          ifId = 0;
          rc = cpdmIntIfNumNextGet(cpId, ifId, &ifId);
          if (rc == L7_SUCCESS)
          {
            selected_ifId = ifId;
          }
        }
      }
    }
    else
    {
      net = ewsContextNetHandle(context);
      if (net->app_pointer != L7_NULL)
      {
        memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
        cpId = appInfo.data[1];
        selected_ifId = appInfo.data[0];
        ifId = 0;
        rc = cpdmIntIfNumNextGet(cpId, ifId, &ifId);
      }
    }
  }
  else
  {
    rc = cpdmIntIfNumNextGet(cpId, ifId, &ifId);
  }

  if (rc == L7_SUCCESS)
  {
    option->valuep = (void *)&ifId;
    bufChoice = ewsContextNetHandle(context)->buffer;
    if (usmDbCpimIntfCapabilityDescriptionGet(ifId, ifDescription) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf), "%s", ifDescription);
    }
    osapiSnprintf(bufChoice, APP_BUFFER_SIZE, "%s", buf);
    option->choice = bufChoice;

    if (selected_ifId == ifId)
    {
      option->selected = TRUE;
    }
    else
    {
      option->selected = FALSE;
    }

    return(void *) option;
  }

  return L7_NULL;
}

/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext            context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpIntfStatusP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpIntfStatus(EwsContext context, EwaForm_cpIntfStatusP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  cpId_t selected_cpId;
  L7_uint32 selected_ifId;
  L7_BOOL status, intf = L7_FALSE;
  L7_INTF_WHY_DISABLED_t reason;
  L7_CP_INST_BLOCK_STATUS_t blockStatus;
  L7_char8 data[USMWEB_APP_SMALL_DATA_SIZE];
  L7_char8 dataCpid[USMWEB_APP_SMALL_DATA_SIZE];
  L7_char8 dataIntf[USMWEB_APP_SMALL_DATA_SIZE];
  L7_uint32 val, i, j = 0, numBytes, user_access;

  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->value.err_flag = L7_FALSE;
  form->status.err_flag |= (EW_FORM_INITIALIZED);
  selected_cpId = 0;
  selected_ifId = 0;
  memset(data, 0, sizeof(data));
  memset(dataCpid, 0, sizeof(dataCpid));
  memset(dataIntf, 0, sizeof(dataIntf));
  user_access = usmWebUserAccessGet(context);
  if (user_access == USMWEB_READWRITE)
  {
    form->status.cpAssocIntf |= (EW_FORM_INITIALIZED);
  }

  numBytes = ewsCGIQueryString(context, data, sizeof(data));
  if (numBytes > 0)
  {
    for (i=0; i<strlen(data); i++)
    {
      if ((data[i+3] != '&') && (intf != L7_TRUE))
      {
        dataCpid[i] = data[i+3];
      }
      else
      {
        dataIntf[j] = data[i+7];
        j++;
        intf = L7_TRUE;
      }
    }
    selected_cpId = atoi(dataCpid);
    selected_ifId = atoi(dataIntf);
  }
  else
  {
    net = ewsContextNetHandle(context);
    if (net->app_pointer != NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      selected_ifId = appInfo.data[0];
      selected_cpId = appInfo.data[1];
      form->value.err_flag = appInfo.err.err_flag;
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_DYNAMIC);
    }
    else
    {
      selected_cpId = CP_ID_MIN;
      if (cpdmIntIfNumNextGet(selected_cpId, selected_ifId, &selected_ifId) != L7_SUCCESS)
      {
        selected_ifId = 0;
      }
    }
  }

  if (selected_cpId != 0)
  {
    /* operational status */
    if (usmDbCpdmIntfStatusGetIsEnabled(selected_ifId, &status) == L7_SUCCESS)
    {
      if (status == L7_ENABLE)
      {
        form->value.cpIntf_status = (char *)osapiStrDup(pStrInfo_common_Enbld);
      }
      else
      {
        form->value.cpIntf_status = (char *)osapiStrDup(pStrInfo_common_Dsbld);
      }
      form->status.cpIntf_status |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    /* disable reason */
    if (status == L7_DISABLE)
    {
      if (usmDbCpdmIntfStatusGetWhy(selected_ifId, &reason) == L7_SUCCESS)
      {
        switch (reason)
        {
        case L7_INTF_WHY_DISABLED_ADMIN:
          form->value.cpIntf_disableReason = (char *)osapiStrDup(pStrInfo_security_CPModeReasonAdmin);
          break;
        case L7_INTF_WHY_DISABLED_UNATTACHED:
          form->value.cpIntf_disableReason = (char *)osapiStrDup(pStrInfo_security_IntfNotAttached);
          break;
        default:
          form->value.cpIntf_disableReason = (char *)osapiStrDup("");
          break;
        }
        form->status.cpIntf_disableReason |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
      }
    }

    /* blocked status */
    if (usmDbCpimIntfCapabilityBlockStatusGet(selected_ifId, &blockStatus) == L7_SUCCESS)
    {
      if (blockStatus == L7_CP_INST_BLOCK_STATUS_BLOCKED)
      {
        form->value.cpIntf_blocked_status = (char *)osapiStrDup(pStrInfo_security_Blocked);
      }
      else
      {
        form->value.cpIntf_blocked_status = (char *)osapiStrDup(pStrInfo_security_NotBlocked);
      }
      form->status.cpIntf_blocked_status |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    /* authenticated users */
    if (usmDbCpdmIntfStatusGetUsers(selected_ifId, &val) == L7_SUCCESS)
    {
      form->value.cpIntf_authUsers = val;
      form->status.cpIntf_authUsers |= (EW_FORM_INITIALIZED);
    }
  }

  form->value.Refresh = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->value.refresh_in_progress = 0;
  form->status.refresh_in_progress |= (EW_FORM_INITIALIZED);
  form->value.refresh_key = 0;
  form->status.refresh_key |= (EW_FORM_INITIALIZED);
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext            context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpIntfStatusP form    @b{(input)} pointer to the form
*
* @returns  cp_intf_status.html
*
* @end
*********************************************************************/
L7_char8 *ewaFormSubmit_cpIntfStatus(EwsContext context, EwaForm_cpIntfStatusP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 ifId;
  cpId_t cpId;

  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));

  appInfo.data[1] = form->value.cp_id;

  if (form->value.refresh_key == 1)
  {
    cpId = (L7_ushort16)appInfo.data[1];
    ifId = 0;
    if (cpdmIntIfNumNextGet(cpId, ifId, &ifId) == L7_SUCCESS)
    {
      appInfo.data[0] = ifId;
    }
  }

  else if ((form->value.refresh_key == 2) || (form->status.Refresh & EW_FORM_RETURNED))
  {
    appInfo.data[0] = form->value.cpAssocIntf;
  }

  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPIntfStatus);
}

