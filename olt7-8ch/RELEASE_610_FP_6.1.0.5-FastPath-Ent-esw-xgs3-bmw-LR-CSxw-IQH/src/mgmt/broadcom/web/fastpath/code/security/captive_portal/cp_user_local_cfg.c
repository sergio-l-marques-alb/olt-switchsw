/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_user_local_cfg.c
*
* @purpose    Code in support of the cp_user_local_cfg.html page
*
* @component  Captive Portal
*
* @comments
*
* @create     7/6/2007
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
#include "usmdb_cpdm_user_api.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"


static L7_uint32 new_user_value = FD_CP_LOCAL_USERS_MAX+1;

extern void cpIntegerSetHelp(L7_uchar8 formStatus, L7_uint32 formValue, 
                                 L7_char8 *pStrInfo, L7_uint32 minValue, L7_uint32 maxValue, 
                                 usmWeb_AppInfo_t *appInfo, L7_RC_t (* funcPtr)(L7_ushort16, L7_uint32),
                                 L7_ushort16 id);

/*********************************************************************
* @purpose  Get the list of configured groups
*
* @param    context   @b{(input)} EmWeb/Server request context handle
* @param    option    @b{(input)} dynamic Select Support
* @param    iterator  @b{(input)} pointer to the current iteration
*
* @returns
*
* @end
*********************************************************************/
void *ewaFormSelect_user_gp_id(EwsContext context, EwsFormSelectOptionP option, void *iterator)
{
  static uId_t uId, nextUId;
  static gpId_t gpId, nextGpId;
  static L7_uint32 gpId32;
  static L7_BOOL fSelectForNewUser;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 *bufChoice;
  L7_char8 user[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_char8 groupName[CP_USER_LOCAL_USERGROUP_MAX + 1];
  L7_char8 data[CP_USER_LOCAL_USERNAME_MAX+1+5];
  L7_char8 dataUser[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_uint32 i, numBytes;

  if (iterator == L7_NULL)
  {
    uId = 0;
    gpId = 0;
    nextUId = 0;
    nextGpId = 0;
    net = ewsContextNetHandle(context);
    if (net->app_pointer != L7_NULL)
    {
      numBytes = ewsCGIQueryString(context, data, sizeof(data));
      memset(user,0,CP_USER_LOCAL_USERNAME_MAX+1);
      if (numBytes > 0)
      {
        for (i=0; i<CP_USER_LOCAL_USERNAME_MAX; i++)
        {
          dataUser[i] = data[i+5];
        }
        memcpy(user, dataUser, CP_USER_LOCAL_USERNAME_MAX);
        usmDbCpdmUserEntryByNameGet(user,&uId);
      }
      else
      {
        memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
        memcpy(user, appInfo.buf, CP_USER_LOCAL_USERNAME_MAX);
        usmDbCpdmUserEntryByNameGet(user,&uId);
      }

      /* Set flag to indicate a new user */
      fSelectForNewUser = (0==strlen(user))?L7_TRUE:L7_FALSE;
    }
  }
  if (L7_SUCCESS==usmDbCpdmUserGroupEntryNextGet(gpId, &gpId))
  {
    bufChoice = ewsContextNetHandle(context)->buffer;
    memset(groupName,0,sizeof(groupName));
    if (L7_SUCCESS==usmDbCpdmUserGroupEntryNameGet(gpId,groupName))
    {
      osapiSnprintf(bufChoice,APP_BUFFER_SIZE-1,"%d-%s",gpId,groupName);
      bufChoice[APP_BUFFER_SIZE-1] = '\0';
      option->choice = bufChoice;
      gpId32 = (L7_uint32) gpId;
      option->valuep = (void *)&gpId32;
      option->selected = (L7_SUCCESS==usmDbCpdmUserGroupAssocEntryGet(uId,gpId))?TRUE:FALSE;
      if (L7_TRUE==fSelectForNewUser)
      {
        option->selected = TRUE;
        fSelectForNewUser = L7_FALSE;
      }
      return (void *) option;
    }
  }
  return L7_NULL;
}


/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext              context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpUserLocalCfgP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpUserLocalCfg(EwsContext context, EwaForm_cpUserLocalCfgP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 selected_user[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_char8 buf[CP_USER_LOCAL_PASSWORD_MAX+1];
  L7_char8 data[CP_USER_LOCAL_USERNAME_MAX+1+5];
  L7_char8 dataUser[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_uint32 user_access, userValue=0, val, i, numBytes;
  uId_t uId;

  user_access = usmWebUserAccessGet(context);
  form->value.err_msg = L7_NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->value.err_flag = L7_FALSE;
  form->status.err_flag |= (EW_FORM_INITIALIZED);

  memset(selected_user, 0, sizeof(selected_user));
  memset(data, 0, sizeof(data));
  memset(dataUser, 0, sizeof(dataUser));

  numBytes = ewsCGIQueryString(context, data, sizeof(data));
  if (numBytes > 0)
  {
    for (i=0; i<CP_USER_LOCAL_USERNAME_MAX; i++)
    {
      dataUser[i] = data[i+5];
    }
    memcpy(selected_user, dataUser, CP_USER_LOCAL_USERNAME_MAX);
  }
  else
  {
    net = ewsContextNetHandle(context);
    if (net->app_pointer != NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      memcpy(selected_user, appInfo.buf, CP_USER_LOCAL_USERNAME_MAX);
      form->value.err_flag = appInfo.err.err_flag;
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_DYNAMIC);
    }
    else
    {
      if (L7_SUCCESS!=usmDbCpdmUserEntryByNameGet(selected_user, &uId))
      {
        userValue = new_user_value;
      }
    }
  }

  /* user name */
  if (userValue != new_user_value)
  {
    form->value.user_exists_maxlength = strlen(selected_user)+1;
	form->value.user_exists_size = form->value.user_exists_maxlength+3; /* A little extra pad */
    form->value.user_exists = (char *)osapiStrDup(selected_user);
    form->status.user_exists |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }
  else
  {
    if (user_access == USMWEB_READWRITE)
    {
      form->status.user_name |= (EW_FORM_INITIALIZED);
    }
  }

  form->value.userId = osapiStrDup((char *)selected_user);

  /* Uid maybe invalid, this is ok when adding a new user */
  usmDbCpdmUserEntryByNameGet(selected_user,&uId);

  /* password */
  memset(buf, 0, sizeof(buf));
  if (usmDbCpdmUserEntryPasswordGet(uId, buf) == L7_SUCCESS)
  {
    if (osapiStrncmp(buf, pStrInfo_common_EmptyString, sizeof(buf)) == 0)
    {
      form->value.user_pwd = osapiStrDup((char *)pStrInfo_common_EmptyString);
    }
    else
    {
      form->value.user_pwd = osapiStrDup((char *)pStrInfo_security_PasswordEncrypted);
    }
    form->status.user_pwd |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* session timeout */
  if (usmDbCpdmUserEntrySessionTimeoutGet(uId, &val) == L7_SUCCESS)
  {
    form->value.session_timeout = val;
    form->status.session_timeout |= (EW_FORM_INITIALIZED);
  }

  /* idle timeout */
  if (usmDbCpdmUserEntryIdleTimeoutGet(uId, &val) == L7_SUCCESS)
  {
    form->value.idle_timeout = val;
    form->status.idle_timeout |= (EW_FORM_INITIALIZED);
  }

  /* Max Bandwidth Up */
  if (usmDbCpdmUserEntryMaxBandwidthUpGet(uId, &val) == L7_SUCCESS)
  {
    /* Convert bits to bytes */
    form->value.user_max_bandwidth_up = val/8;
    form->status.user_max_bandwidth_up |= (EW_FORM_INITIALIZED);
  }

  /* Max Bandwidth Down */
  if (usmDbCpdmUserEntryMaxBandwidthDownGet(uId, &val) == L7_SUCCESS)
  {
    /* Convert bits to bytes */
    form->value.user_max_bandwidth_down = val/8;
    form->status.user_max_bandwidth_down |= (EW_FORM_INITIALIZED);
  }

  /* Max Input */
  if (usmDbCpdmUserEntryMaxInputOctetsGet(uId, &val) == L7_SUCCESS)
  {
    form->value.user_max_input = val;
    form->status.user_max_input |= (EW_FORM_INITIALIZED);
  }

  /* Max Output */
  if (usmDbCpdmUserEntryMaxOutputOctetsGet(uId, &val) == L7_SUCCESS)
  {
    form->value.user_max_output = val;
    form->status.user_max_output |= (EW_FORM_INITIALIZED);
  }

  /* Max Total */
  if (usmDbCpdmUserEntryMaxTotalOctetsGet(uId, &val) == L7_SUCCESS)
  {
    form->value.user_max_total = val;
    form->status.user_max_total |= (EW_FORM_INITIALIZED);
  }

  form->value.Add = (char *)osapiStrDup(pStrInfo_common_Add);
  form->status.Add |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Delete = (char *)osapiStrDup(pStrInfo_common_Del);
  form->status.Delete |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Submit = (char *) osapiStrDup(pStrInfo_common_Submit);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Refresh = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Refresh_ro = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->value.refresh_in_progress = 0;
  form->status.refresh_in_progress |= (EW_FORM_INITIALIZED);
  form->value.refresh_key = 0;
  form->status.refresh_key |= (EW_FORM_INITIALIZED);
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext              context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpUserLocalCfgP form    @b{(input)} pointer to the form
*
* @returns  cp_user_local_cfg.html
*
* @end
*********************************************************************/
char *ewaFormSubmit_cpUserLocalCfg(EwsContext context, EwaForm_cpUserLocalCfgP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 buf[USMWEB_BUFFER_SIZE_256];
  L7_char8 user[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_char8 pwd[CP_USER_LOCAL_PASSWORD_MAX+1];
  L7_uint32 gpIds, gpIdList[GP_ID_MAX];
  L7_uint32 len, idx, bytes;
  uId_t uId;

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0x00, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0x00, sizeof(usmWeb_AppInfo_t));
  memset(buf,0,sizeof(buf));
  appInfo.err.err_flag = L7_FALSE;

  if (form->status.Add & EW_FORM_RETURNED)
  {
    memset(user, 0, sizeof(user));
    memcpy(user, form->value.user_name, CP_USER_LOCAL_USERNAME_MAX);
    if (L7_SUCCESS==usmDbCpdmUserEntryByNameGet(user,&uId))
    {
      appInfo.err.err_flag = L7_TRUE;
      usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_UserExists, user);
      return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPUserLocal);
    }
    /* Is user name valid? */
    len = strlen(user);
    if ((len <  CP_USER_LOCAL_USERNAME_MIN) || 
        (len >  CP_USER_LOCAL_USERNAME_MAX))
    {
      appInfo.err.err_flag = L7_TRUE;
      usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_UserNameLengthInvalid,
                             CP_USER_LOCAL_USERNAME_MIN, CP_USER_LOCAL_USERNAME_MAX);
      return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPUserLocal);
    }
    if ((usmDbStringAlphaNumericCheck(user) != L7_SUCCESS) ||
        (' ' == user[0]) /* don't allow the obvious; leading space */)
    {
      appInfo.err.err_flag = L7_TRUE;
      usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_FailedToSet, pStrErr_common_FailedToSetReason, 
                             user, pStrInfo_security_UserNameMustContainAlnumCharsOnly);
      return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPUserLocal);
    }
    /* Is password valid? */
    memset(pwd, 0, sizeof(pwd));
    memcpy(pwd, form->value.user_pwd, strlen(form->value.user_pwd));
    len = strlen(pwd);
    if ((len <  CP_USER_LOCAL_PASSWORD_MIN) || 
        (len >  CP_USER_LOCAL_PASSWORD_MAX))
    {
      appInfo.err.err_flag = L7_TRUE;
      usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_PasswordLengthInvalid,
                             CP_USER_LOCAL_PASSWORD_MIN, CP_USER_LOCAL_PASSWORD_MAX);
      return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPUserLocal);
    }

    uId = 1;
    while ((uId < FD_CP_LOCAL_USERS_MAX) && (L7_SUCCESS==usmDbCpdmUserEntryGet(uId)))
    {
      uId++;
    }
    if (uId > FD_CP_LOCAL_USERS_MAX)
    {
      appInfo.err.err_flag = L7_TRUE;
      usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_FailedToAddUser, user);
    }
    if (L7_TRUE!=appInfo.err.err_flag)
    {
      if (L7_SUCCESS!=usmDbCpdmUserEntryAdd(uId))
      {
        appInfo.err.err_flag = L7_TRUE;
        usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_FailedToAddUser, user);
      }
    }
    /* set login name */
    if (L7_TRUE!=appInfo.err.err_flag)
    {
      if (L7_SUCCESS!=usmDbCpdmUserEntryLoginNameSet(uId,user))
      {
        appInfo.err.err_flag = L7_TRUE;
        usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_FailedToAddUser, user);
      }
    }
    /* set pwd */
    if (L7_TRUE!=appInfo.err.err_flag)
    {
      if (L7_SUCCESS!=usmDbCpdmUserEntryPasswordSet(uId,pwd))
      {
        appInfo.err.err_flag = L7_TRUE;
        usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_FailedToAddUser, user);
      }
    }
    /* user group */
    if (L7_TRUE!=appInfo.err.err_flag)
    {
      gpIds = form->value.user_gp_id_choices;
      if (0==gpIds) /* If no selection, set default */
      {
        if (L7_SUCCESS!=usmDbCpdmUserGroupAssocEntryAdd(uId,GP_ID_MIN))
        {
          appInfo.err.err_flag = L7_TRUE;
          usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_FailedToAddUser, user);
        }
      }
      else
      {
        memcpy(gpIdList, form->value.user_gp_id, sizeof(L7_uint32)*form->value.user_gp_id_choices);
        for (idx=0; idx<gpIds; idx++)
        {
          if (L7_SUCCESS!=usmDbCpdmUserGroupAssocEntryAdd(uId,(gpId_t)gpIdList[idx]))
          {
            appInfo.err.err_flag = L7_TRUE;
            usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_FailedToAddUser, user);
            break;
          }
        }
      }
    }
    return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPUserLocal);
  }

  else if (form->status.Delete & EW_FORM_RETURNED)
  {
    if (form->value.refresh_key == 1) /* User indicates OK to delete */
    {
      memset(user, 0, sizeof(user));
      memcpy(user, form->value.user_exists, CP_USER_LOCAL_USERNAME_MAX);

      if (L7_SUCCESS!=usmDbCpdmUserEntryByNameGet(user, &uId))
      {
        appInfo.err.err_flag = L7_TRUE;
        usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_FailedToDelUser, user);
      }
      if (L7_TRUE!=appInfo.err.err_flag)
      {
        if (L7_SUCCESS!=usmDbCpdmUserEntryDelete(uId))
        {
          appInfo.err.err_flag = L7_TRUE;
          usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_FailedToDelUser, user);
        }
      }
      if (L7_TRUE!=appInfo.err.err_flag)
      {
        if (usmDbCpdmUserGroupAssocEntryByUIdDeleteAll(uId) != L7_SUCCESS)
        {
          appInfo.err.err_flag = L7_TRUE;
          usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_FailedToDelUser, user);
        }
      }
      if (L7_TRUE!=appInfo.err.err_flag)
      {
        appInfo.err.err_flag = L7_TRUE;
        usmWebAppInfoCatErrMsg(&appInfo, pStrInfo_security_DeletedUser, user);
      }
      return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPUserLocal);
    }

    if (form->value.refresh_key == 2) /* User indicated not OK to delete */
    {
      memset(user, 0, sizeof(user));
      memcpy(user, form->value.userId, strlen(form->value.userId));
      memcpy(appInfo.buf, user, CP_USER_LOCAL_USERNAME_MAX);
    }
  }

  else if (form->status.Submit & EW_FORM_RETURNED)
  {
    memset(user, 0, sizeof(user));
    memcpy(user, form->value.user_exists, CP_USER_LOCAL_USERNAME_MAX);
    memcpy(appInfo.buf, user, CP_USER_LOCAL_USERNAME_MAX);

    if (L7_SUCCESS!=usmDbCpdmUserEntryByNameGet(user, &uId))
    {
      appInfo.err.err_flag = L7_TRUE;
      usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_FailedToAddUser, user);
    }

    /* password */
    if (L7_TRUE!=appInfo.err.err_flag)
    {
      memset(pwd, 0, sizeof(pwd));
      memcpy(pwd, form->value.user_pwd, strlen(form->value.user_pwd));
      if (osapiStrncmp(pwd, pStrInfo_security_PasswordEncrypted, sizeof(pwd)) != 0)
      {
        if ((strlen(pwd) < CP_USER_LOCAL_PASSWORD_MIN) || (strlen(pwd) > CP_USER_LOCAL_PASSWORD_MAX))
        {
          appInfo.err.err_flag = L7_TRUE;
          usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_PasswordLengthInvalid,
                                 CP_USER_LOCAL_PASSWORD_MIN, CP_USER_LOCAL_PASSWORD_MAX);
        }
        else
        {
          if (usmDbCpdmUserEntryPasswordSet(uId, pwd) != L7_SUCCESS)
          {
            appInfo.err.err_flag = L7_TRUE;
            usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_FailedToSet, 
                                   pStrErr_common_FailedToSet_1, pStrInfo_security_Password);
          }
        }
      }
    }
    if (L7_TRUE==appInfo.err.err_flag)
    {
      return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPUserLocalCfg);
    }

    /* user group */
    if (L7_SUCCESS!=usmDbCpdmUserGroupAssocEntryByUIdDeleteAll(uId))
    {
      appInfo.err.err_flag = L7_TRUE;
      usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_FailedToAddUser, user);
    }
    gpIds = form->value.user_gp_id_choices;

    if (0==gpIds) /* If no selection, set default */
    {
      if (usmDbCpdmUserGroupAssocEntryAdd(uId,GP_ID_MIN)!=L7_SUCCESS)
      {
		appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                         pStrInfo_security_FailedToAddUserGroup,buf);
        return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPUserLocalCfg);
      }
    }
    else
    {
      memcpy(gpIdList, form->value.user_gp_id, sizeof(L7_uint32)*form->value.user_gp_id_choices);
      for (idx=0; idx<gpIds; idx++)
      {
        if (usmDbCpdmUserGroupAssocEntryAdd(uId,(gpId_t)gpIdList[idx]) != L7_SUCCESS)
        {
		  appInfo.err.err_flag = L7_TRUE;
          osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                           pStrInfo_security_FailedToAddUserGroup,buf);
          return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPUserLocalCfg);
        }
      }
    }

    /* session timeout */
    cpIntegerSetHelp(form->status.session_timeout, form->value.session_timeout, 
                         pStrInfo_security_SessionTimeoutSecs, 
                         CP_USER_LOCAL_SESSION_TIMEOUT_MIN, CP_USER_LOCAL_SESSION_TIMEOUT_MAX, 
                         &appInfo, usmDbCpdmUserEntrySessionTimeoutSet, uId);

    /* idle timeout */
    cpIntegerSetHelp(form->status.idle_timeout, form->value.idle_timeout, 
                         pStrInfo_security_IdleTimeout, 
                         CP_USER_LOCAL_IDLE_TIMEOUT_MIN, CP_USER_LOCAL_IDLE_TIMEOUT_MAX, 
                         &appInfo, usmDbCpdmUserEntryIdleTimeoutSet, uId);

    /* max bandwidth up */
    bytes = form->value.user_max_bandwidth_up;
	if (form->status.user_max_bandwidth_up & EW_FORM_PARSE_ERROR)
	{
	  appInfo.err.err_flag = L7_TRUE;
	  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s%s\r\n", 
					   pStrErr_common_Error, pStrInfo_security_MaxBwUp, pStrInfo_security_FieldInvalidFmt);
	}
	else if ((bytes < CP_USER_LOCAL_MAX_BW_UP_MIN) || 
			 (bytes > CP_USER_LOCAL_MAX_BW_UP_MAX))
	{
	  appInfo.err.err_flag = L7_TRUE;
	  osapiSnprintf(buf, sizeof(buf), pStrErr_common_FieldOutOfRange, 
					bytes, pStrInfo_security_MaxBwUp, 
					CP_USER_LOCAL_MAX_BW_UP_MIN, CP_USER_LOCAL_MAX_BW_UP_MAX);
	  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s\r\n", 
					   pStrErr_common_Error, buf);
	}
	else
	{
	  if (usmDbCpdmUserEntryMaxBandwidthUpSet(uId, bytes*8) != L7_SUCCESS)
	  {
		appInfo.err.err_flag = L7_TRUE;
		osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
						 pStrErr_common_FailedToSet, pStrInfo_security_MaxBwUp);
	  }
	}

    /* max bandwidth down */
    bytes = form->value.user_max_bandwidth_down;
	if (form->status.user_max_bandwidth_down & EW_FORM_PARSE_ERROR)
	{
	  appInfo.err.err_flag = L7_TRUE;
	  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s%s\r\n", 
					   pStrErr_common_Error, pStrInfo_security_MaxBwDown, pStrInfo_security_FieldInvalidFmt);
	}
	else if ((bytes < CP_USER_LOCAL_MAX_BW_DOWN_MIN) || 
			 (bytes > CP_USER_LOCAL_MAX_BW_DOWN_MAX))
	{
	  appInfo.err.err_flag = L7_TRUE;
	  osapiSnprintf(buf, sizeof(buf), pStrErr_common_FieldOutOfRange, 
					bytes, pStrInfo_security_MaxBwDown, 
					CP_USER_LOCAL_MAX_BW_DOWN_MIN, CP_USER_LOCAL_MAX_BW_DOWN_MAX);
	  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s\r\n", 
					   pStrErr_common_Error, buf);
	}
	else
	{
	  if (usmDbCpdmUserEntryMaxBandwidthDownSet(uId, bytes*8) != L7_SUCCESS)
	  {
		appInfo.err.err_flag = L7_TRUE;
		osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
						 pStrErr_common_FailedToSet, pStrInfo_security_MaxBwDown);
	  }
	}

    /* max input */
    cpIntegerSetHelp(form->status.user_max_input, form->value.user_max_input, 
                         pStrInfo_security_MaxInputOctets, 
                         CP_USER_LOCAL_MAX_INPUT_OCTETS_MIN, CP_USER_LOCAL_MAX_INPUT_OCTETS_MAX, 
                         &appInfo, usmDbCpdmUserEntryMaxInputOctetsSet, uId);

    /* max output */
    cpIntegerSetHelp(form->status.user_max_output, form->value.user_max_output, 
                         pStrInfo_security_MaxOutputOctets, 
                         CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MIN, CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MAX, 
                         &appInfo, usmDbCpdmUserEntryMaxOutputOctetsSet, uId);

    /* max total */
    cpIntegerSetHelp(form->status.user_max_total, form->value.user_max_total, 
                         pStrInfo_security_MaxTotalOctets, 
                         CP_USER_LOCAL_MAX_TOTAL_OCTETS_MIN, CP_USER_LOCAL_MAX_TOTAL_OCTETS_MAX, 
                         &appInfo, usmDbCpdmUserEntryMaxTotalOctetsSet, uId);

  }

  else if ((form->status.Refresh & EW_FORM_RETURNED) || 
           (form->status.Refresh_ro & EW_FORM_RETURNED))
  {
    memset(user, 0, sizeof(user));
    memcpy(user, form->value.userId, strlen(form->value.userId));
    memcpy(appInfo.buf, user, CP_USER_LOCAL_USERNAME_MAX);
  }

  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPUserLocalCfg);
}

