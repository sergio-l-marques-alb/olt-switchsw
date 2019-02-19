/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_intf_assoc_cfg.c
*
* @purpose    Code in support of the cp_intf_assoc_cfg.html page
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
#include "usmdb_cpim_api.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "captive_portal_commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "util_pstring.h"


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
void *ewaFormSelect_cpIntf(EwsContext context, EwsFormSelectOptionP option, void *iterator)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 *bufChoice;
  static L7_uint32 ifId;
  static cpId_t cpId;
  L7_uint32 next_ifId = 0;
  cpId_t next_cpId = 0;
  L7_uchar8 ifDescription[CP_INTF_DESCRIPTION_MAX_LEN+1];
  L7_char8 buf[USMWEB_APP_BUF_SIZE];
  L7_RC_t rc = L7_FAILURE;

  if (iterator == L7_NULL)
  {
    ifId = 0;
    cpId = CP_ID_MIN;

    net = ewsContextNetHandle(context);
    if (net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      cpId = appInfo.data[1];
    }
  }

  rc = usmDbCpdmCPConfigIntIfNumNextGet(cpId, ifId, &next_cpId, &next_ifId);
  if ((rc == L7_SUCCESS) && (next_cpId == cpId))
  {
    ifId = next_ifId;
    option->valuep = (void *)&ifId;
    bufChoice = ewsContextNetHandle(context)->buffer;
    if (usmDbCpimIntfCapabilityDescriptionGet(ifId, ifDescription) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf), "%s", ifDescription);
    }
    osapiSnprintf(bufChoice, APP_BUFFER_SIZE, "%s", buf);
    option->choice = bufChoice;
    option->selected = FALSE;
    return (void *) option;
  }

  return L7_NULL;
}

/*********************************************************************
* @purpose  Get the list of all non-associated interfaces
*
* @param    context   @b{(input)} EmWeb/Server request context handle
* @param    option    @b{(input)} dynamic Select Support
* @param    iterator  @b{(input)} pointer to the current iteration
*
* @returns
*
* @end
*********************************************************************/
void *ewaFormSelect_cpInterface(EwsContext context, EwsFormSelectOptionP option, void *iterator)
{
  L7_char8 *bufChoice;
  static L7_uint32 ifId;
  L7_uchar8 ifDescription[CP_INTF_DESCRIPTION_MAX_LEN+1];
  L7_char8 buf[USMWEB_APP_BUF_SIZE];
  L7_RC_t rc = L7_FAILURE;

  if (iterator == L7_NULL)
  {
    ifId = 0;
  }
  rc = usmDbCpdmCPConfigIntfNotAssocNextGet(ifId, &ifId);
  if (rc != L7_SUCCESS)
  {
    return L7_NULL;
  }

  option->valuep = (void *)&ifId;
  bufChoice = ewsContextNetHandle(context)->buffer;
  if (usmDbCpimIntfCapabilityDescriptionGet(ifId, ifDescription) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%s", ifDescription);
  }
  else
  {
    return L7_NULL;
  }
  osapiSnprintf(bufChoice, APP_BUFFER_SIZE, "%s", buf);
  option->choice = bufChoice;
  option->selected = FALSE;
  return (void *) option;
}

/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext              context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpIntfAssocCfgP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpIntfAssocCfg(EwsContext context, EwaForm_cpIntfAssocCfgP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_ushort16 selected_cpId;
  L7_uint32 user_access;

  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->value.err_flag = L7_FALSE;
  form->status.err_flag |= (EW_FORM_INITIALIZED);
  user_access = usmWebUserAccessGet(context);
  selected_cpId = CP_ID_MIN;

  if (user_access == USMWEB_READWRITE)
  {
    form->status.cpIntf |= (EW_FORM_INITIALIZED);
    form->status.cpInterface |= (EW_FORM_INITIALIZED);
  }

  net = ewsContextNetHandle(context);
  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    selected_cpId = (L7_ushort16)appInfo.data[1];
    form->value.err_flag = appInfo.err.err_flag;
    form->value.err_msg = osapiStrDup(appInfo.err.msg);
    form->status.err_msg |= (EW_FORM_DYNAMIC);
  }

  form->value.Add = (char *) osapiStrDup(pStrInfo_common_Add);
  form->status.Add |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Delete = (char *) osapiStrDup(pStrInfo_common_Del);
  form->status.Delete |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
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
* @param    EwsContext              context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpIntfAssocCfgP form    @b{(input)} pointer to the form
*
* @returns  cp_intf_assoc_cfg.html
*
* @end
*********************************************************************/
L7_char8 *ewaFormSubmit_cpIntfAssocCfg(EwsContext context, EwaForm_cpIntfAssocCfgP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_ushort16 curr_cpId=0;
  L7_uint32 selected_cpId=0, i, numIntf;
  L7_uint32 intfList[CP_INTERFACE_MAX];
  L7_uchar8 ifDescription[CP_INTF_DESCRIPTION_MAX_LEN+1];
  L7_char8 buf[USMWEB_APP_BUF_SIZE];
  L7_char8 errMsg[USMWEB_ERROR_MSG_SIZE];
  L7_char8 tmpBuf[USMWEB_APP_BUF_SIZE];
  L7_uint32 count;

  selected_cpId = form->value.cp_id;
  memset(errMsg, 0x00, sizeof(errMsg));

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0x00, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;
  appInfo.data[1] = selected_cpId;

  if (form->status.Add & EW_FORM_RETURNED)
  {
    numIntf = form->value.cpInterface_choices;
    if (numIntf > 0)
    {
      memcpy(intfList, form->value.cpInterface, sizeof(L7_uint32)*form->value.cpInterface_choices);
      count = 0;
      for (i=0; i<numIntf; i++)
      {
        if (usmDbCpdmCPConfigIntIfNumFind(intfList[i], &curr_cpId) == L7_SUCCESS)
        {
          if (selected_cpId != (L7_uint32)curr_cpId)
          {
            appInfo.err.err_flag = L7_TRUE;
            if (usmDbCpimIntfCapabilityDescriptionGet(intfList[i], ifDescription) == L7_SUCCESS)
            {
              osapiSnprintf(buf, sizeof(buf), "%s", ifDescription);
            }
            if (count == 0)
            {
              osapiSnprintfAddBlanks(1,0,0,0, L7_NULLPTR, errMsg, sizeof(errMsg), pStrInfo_security_UnableToAddInterfacesToCP);
            }
            osapiSnprintf(tmpBuf, sizeof(tmpBuf), "\r\n%s%s%u", buf, pStrInfo_security_IntfCPAlreadyAssociated, curr_cpId);
            osapiStrncat(errMsg, tmpBuf, (sizeof(errMsg)-strlen(errMsg)-1));
            count++;
          }
        }
        else
        {
          if (usmDbCpdmCPConfigIntIfNumAdd((L7_ushort16)selected_cpId, intfList[i]) != L7_SUCCESS)
          {
            appInfo.err.err_flag = L7_TRUE;
            usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrErr_common_FailedToAdd, pStrInfo_security_CPConfig);
          }
        }
      }
      if (count > 0)
      {
        osapiStrncat(appInfo.err.msg, errMsg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1));
      }
    }
  }

  else if (form->status.Delete & EW_FORM_RETURNED)
  {
    if (form->value.refresh_key == 1)
    {
      numIntf = form->value.cpIntf_choices;
      if (numIntf > 0)
      {
        memcpy(intfList, form->value.cpIntf, sizeof(L7_uint32)*form->value.cpIntf_choices);
        count = 0;
        for (i=0; i<numIntf; i++)
        {
          if (usmDbCpimIntfCapabilityDescriptionGet(intfList[i], ifDescription) == L7_SUCCESS)
          {
            osapiSnprintf(buf, sizeof(buf), "%s", ifDescription);
          }

          if (usmDbCpdmCPConfigIntIfNumFind(intfList[i], &curr_cpId) == L7_SUCCESS)
          {
            if (selected_cpId == (L7_uint32)curr_cpId)
            {
              if (usmDbCpdmCPConfigIntIfNumDelete(curr_cpId, intfList[i]) != L7_SUCCESS)
              {
                appInfo.err.err_flag = L7_TRUE;
                usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrErr_common_FailedToDel, pStrInfo_security_CPConfig);
              }
            }
            else
            {
              appInfo.err.err_flag = L7_TRUE;
              if (count == 0)
              {
                osapiSnprintfAddBlanks(1,0,0,0, L7_NULLPTR, errMsg, sizeof(errMsg), 
                                       pStrInfo_security_UnableToDelInterfacesFromCP, selected_cpId);
              }
              osapiSnprintf(tmpBuf, sizeof(tmpBuf), "\r\n%s", buf);
              osapiStrncat(errMsg, tmpBuf, (sizeof(errMsg)-strlen(errMsg)-1));
              count++;
            }
          }
          else
          {
            appInfo.err.err_flag = L7_TRUE;
            if (count == 0)
            {
              osapiSnprintfAddBlanks(1,0,0,0, L7_NULLPTR, errMsg, sizeof(errMsg), 
                                     pStrInfo_security_UnableToDelInterfacesFromCP, selected_cpId);
            }
            osapiSnprintf(tmpBuf, sizeof(tmpBuf), "\r\n%s", buf);
            osapiStrncat(errMsg, tmpBuf, (sizeof(errMsg)-strlen(errMsg)-1));
            count++;
          }
        }
        if (count > 0)
        {
          osapiStrncat(appInfo.err.msg, errMsg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1));
        }
      }
    }
  }

  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPIntfAssocCfg);
}

