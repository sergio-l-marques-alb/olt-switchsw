/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_cfg_mgmt.c
*
* @purpose    Code in support of the cp_cfg_mgmt.html page
*
* @component  Captive Portal
*
* @comments
*
* @create     7/9/2007
*
* @author     rjindal/darsen
*
* @end
*
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include "l7_common.h"
#include "radius_exports.h"
#include "usmdb_radius_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"
#include "ew_proto.h"
#include "ewnet.h"
#include "web.h"
#include "web_buffer.h"
#include "web_oem.h"
#include "strlib_security_web.h"
#include "strlib_security_cli.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_web_api.h"
#include "usmdb_cpdm_user_api.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"

#define SUBMIT      1
#define CLEAR       2
#define DELETE      3
#define GROUP_ADD   4
#define GROUP_DEL   5
#define GROUP_MOD   6

extern L7_RC_t cliWebConvertTo32BitUnsignedInteger(const L7_char8 *buf, L7_uint32 *pVal);

extern void cpIntegerSetHelp(L7_uchar8 formStatus, L7_uint32 formValue, 
                                 L7_char8 *pStrInfo, L7_uint32 minValue, L7_uint32 maxValue, 
                                 usmWeb_AppInfo_t *appInfo, L7_RC_t (* funcPtr)(L7_ushort16, L7_uint32),
                                 L7_ushort16 id);

/*********************************************************************
* @purpose  If the webId doesn't exist, create it if valid.  If it
*           does exist, update if valid. Delete existing webIds if it
*           exists but the new code or link is empty.
*
* @param    cpId_t cpId @b{(input)} current CP config
* @param    webId_t webId @b{(input)} current web locale
* @param    L7_char8 *code @b{(input)} browser code
* @param    L7_char8 *link @b{(input)} language id
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t addOrUpdateWebLocale(cpId_t cpId, webId_t webId, L7_char8 *code, L7_char8 *link)
{
  L7_char8 langCode[CP_LANG_CODE_MAX+1];

  if ((0!=strlen(code)) && (0!=strlen(link)))
  {
    if (CP_LOCALE_LINK_MAX < strlen(link))
    {
      return L7_FAILURE;
    }
    
    if (L7_SUCCESS == usmDbCpdmCPConfigWebIdGet(cpId,webId))
    {
      memset(langCode,0,sizeof(langCode));
      usmDbCpdmCPConfigWebLangCodeGet(cpId,webId,langCode);
      if ((osapiStrncmp(langCode,code,CP_LANG_CODE_MAX) != 0) &&
          (usmDbCpdmCPConfigWebIsDefault(code,CP_PARAM_NAME_CODE,langCode) == L7_TRUE))
      {
        usmDbCpdmCPConfigWebAddDefaults(cpId,webId,code,CP_ALL);
      }
      usmDbCpdmCPConfigWebLangCodeSet(cpId,webId,code);
      usmDbCpdmCPConfigWebLocaleLinkSet(cpId,webId,link);
    }
    else
    {
      if (usmDbCpdmCPConfigWebIdAdd(cpId,webId) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      usmDbCpdmCPConfigWebAddDefaults(cpId,webId,code,CP_ALL);
      usmDbCpdmCPConfigWebLocaleLinkSet(cpId,webId,link);
    }
  }
  else
  {
    if (usmDbCpdmCPConfigWebIdDelete(cpId,webId) == L7_REQUEST_DENIED)
    {
      /* Must be the last locale */
      usmDbCpdmCPConfigWebLangCodeSet(cpId,webId,WEB_DEF1_LANG_CODE);
      usmDbCpdmCPConfigWebLocaleLinkSet(cpId,webId,WEB_DEF1_LOCALE_LINK);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Construct and return HTML code that consists of
*           CP_WEB_ID_MAX table rows that is used to contain language
*           code and link data. Each existing locale is also assigned
*           a webId
*
* @param    context   @b{(input)} EmWeb/Server request context handle
*
* @returns  L7_char8 * HTML
*
* @end
*********************************************************************/
L7_char8 *usmWebCPLangCodeGet(EwsContext context)
{
  EwaNetHandle net = ewsContextNetHandle(context);
  cpAppInfo_t appInfo;
  L7_char8 langCode[CP_LANG_CODE_MAX+1];
  L7_char8 langLink[CP_LOCALE_LINK_MAX+1];
  L7_char8 tmpLangLink[CP_LOCALE_LINK_MAX+1];
  cpId_t cpId = 0;
  webId_t webId;
  L7_char8 TRH[]   = "<TR>";
  L7_char8 TRF[]   = "</TR>";
  L7_char8 TDH[]   = "<TD COLSPAN=\"%d\" NOWRAP CLASS=\"rowdataattributes\">";
  L7_char8 CODE[]  = "<INPUT CLASS=\"inputfield\" TYPE=\"text\" ID=\"code%d\" NAME=\"code%d\" SIZE=\"1\" MAXLENGTH=\"32\" VALUE=\"%s\"></TD>";
  L7_char8 LINK[]  = "<INPUT CLASS=\"inputfield\" TYPE=\"text\" ID=\"link%d\" NAME=\"link%d\" SIZE=\"26\" MAXLENGTH=\"512\" VALUE=\"%s\">";
  L7_char8 SEL[]   = "<INPUT CLASS=\"buttonattributes\" TYPE=\"button\" onClick=\"popupLocaleActivate(document.forms[0].code%d,document.forms[0].link%d,'cp_sel%d');return false;\" NAME=\"cp_sel%d\" ID=\"cp_sel%d\" VALUE=\"...\">";
  L7_char8 CLR[]   = "<INPUT CLASS=\"buttonattributes\" TYPE=\"button\" onClick=\"clr_lang(document.forms[0].code%d,document.forms[0].link%d);return false;\" NAME=\"cp_clr%d\" ID=\"cp_clr%d\" VALUE=\"Clear\"></TD>";


  memset(webStaticContentBuffer,0,sizeof(webStaticContentBuffer));

  if (net->app_pointer != L7_NULL)      
  {
    memset(&appInfo,0,sizeof(cpAppInfo_t));
    memcpy(&appInfo,net->app_pointer,sizeof(cpAppInfo_t));
    cpId = (cpId_t) appInfo.data[0];
  }

  /* Display existing web locales */
  webId = 0;
  while (L7_SUCCESS == usmDbCpdmCPConfigWebIdNextGet(cpId,webId,&webId))
  {
    memset(langCode,0,sizeof(langCode));
    memset(langLink,0,sizeof(langLink));
    memset(tmpLangLink,0,sizeof(tmpLangLink));
    usmDbCpdmCPConfigWebLangCodeGet(cpId,webId,langCode);
    usmDbCpdmCPConfigWebLocaleLinkGet(cpId,webId,tmpLangLink);
    usmWebConvertHexToUnicode(tmpLangLink,langLink);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",TRH);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),TDH,1);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),CODE,webId,webId,langCode);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),TDH,2);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),LINK,webId,webId,langLink);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),SEL,webId,webId,webId,webId,webId);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),CLR,webId,webId,webId,webId,webId);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",TRF);
  }

  /* Display blank components up to the allowed max */
  for (webId=1; webId<=CP_WEB_ID_MAX; webId++)
  {
    if (L7_SUCCESS == usmDbCpdmCPConfigWebIdGet(cpId,webId))
    {
      continue;
    }
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",TRH);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),TDH,1);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),CODE,webId,webId,"");
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),TDH,2);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),LINK,webId,webId,"");
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),SEL,webId,webId,webId,webId,webId);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),CLR,webId,webId,webId,webId,webId);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",TRF);
  }

  return webStaticContentBuffer;
}

/*********************************************************************
* @purpose  Get the list of configured/preferred language codes
*
* @param    context   @b{(input)} EmWeb/Server request context handle
* @param    option    @b{(input)} dynamic Select Support
* @param    iterator  @b{(input)} pointer to the current iteration
*
* @returns
*
* @end
*********************************************************************/
L7_char8 *usmWebCPRadiusServerSelect(EwsContext context)
{
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_uchar8 hostName[L7_DNS_HOST_NAME_LEN_MAX]; 
  L7_uchar8 hostAddr[L7_DNS_HOST_NAME_LEN_MAX]; 
  L7_uchar8 nextHostAddr[L7_DNS_HOST_NAME_LEN_MAX];

  L7_uint32 val;
  L7_uint32 unit;
  L7_char8 type;

  L7_char8 BEGIN[]  = "<CENTER><FORM><SELECT NAME='sel_radius' onChange='radiusPick(this.options[this.selectedIndex].value);' SIZE='8'>";
  L7_char8 OPTION[] = "<OPTION VALUE='%s'>%c - %s (%s)";
  L7_char8 NOOPT[]  = "<OPTION VALUE='%s'>%s";
  L7_char8 END[]    = "</SELECT></FORM></CENTER>";

  memset(webStaticContentBuffer,0,sizeof(webStaticContentBuffer));
  unit = usmDbThisUnitGet();

  osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer),"%s",BEGIN);

  /* Get the First entry */
  memset(hostAddr,0,sizeof(hostAddr));
  memset(hostName,0,sizeof(hostName));

  if (L7_SUCCESS != usmDbRadiusServerFirstIPHostNameGet(unit, hostAddr, &addrType))
  {
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                     NOOPT,"",pStrErr_security_NoRadiusSrvrsCfgured);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",END);
    return webStaticContentBuffer;
  }

  usmDbRadiusServerHostNameGet(unit,hostAddr,addrType,hostName);

  type = 'N';
  if (L7_SUCCESS == usmDbRadiusHostNameServerEntryTypeGet(unit,hostAddr,addrType,&val))
  {
    type = (val == L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY)?'P':'S';
  }

  osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                   OPTION,hostName,type,hostName,hostAddr);

  while (L7_SUCCESS == usmDbRadiusServerNextIPHostNameGet(unit,hostAddr,nextHostAddr,&addrType))
  {
    usmDbRadiusServerHostNameGet(unit,nextHostAddr,addrType,hostName);

    type = 'N';
    if (L7_SUCCESS == usmDbRadiusHostNameServerEntryTypeGet(unit,nextHostAddr,addrType,&val))
    {
      type = (val == L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY)?'P':'S';
    }
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                     OPTION,hostName,type,hostName,nextHostAddr);

    strcpy(hostAddr, nextHostAddr);
  }

  osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",END);
  return webStaticContentBuffer;
}

/*********************************************************************
* @purpose  Get the list of configured/preferred language codes
*
* @param    context   @b{(input)} EmWeb/Server request context handle
* @param    option    @b{(input)} dynamic Select Support
* @param    iterator  @b{(input)} pointer to the current iteration
*
* @returns
*
* @end
*********************************************************************/
L7_char8 *usmWebCPLangCodeSelect(EwsContext context)
{
  static L7_uint32 idx;
  L7_char8 langCode[CP_LANG_CODE_MAX+1];
  L7_char8 langLink[CP_LOCALE_LINK_MAX+1];
  L7_char8 tmpLangLink[CP_LOCALE_LINK_MAX+1];

  L7_char8 BEGIN[]  = "<CENTER><FORM><SELECT NAME='sel' onChange='langPick(this.options[this.selectedIndex].value,this.options[this.selectedIndex].text);' SIZE='8'>";
  L7_char8 OPTION[] = "<OPTION VALUE='%s'>%s";
  L7_char8 END[]    = "</SELECT></FORM></CENTER>";

  memset(webStaticContentBuffer,0,sizeof(webStaticContentBuffer));
  osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer),"%s",BEGIN);

  for (idx=1; idx<=CP_DEFAULT_LOCALE_MAX; idx++)
  {
    memset(langCode,0,sizeof(langCode));
    memset(tmpLangLink,0,sizeof(tmpLangLink));
    switch (idx)
    {
      case 1:
      {
        strcpy(langCode,WEB_DEF1_LANG_CODE);
        strcpy(tmpLangLink,WEB_DEF1_LOCALE_LINK);
        break;
      }
      case 2:
      {
        strcpy(langCode,WEB_DEF2_LANG_CODE);
        strcpy(tmpLangLink,WEB_DEF2_LOCALE_LINK);
        break;
      }
      case 3:
      {
        strcpy(langCode,WEB_DEF3_LANG_CODE);
        strcpy(tmpLangLink,WEB_DEF3_LOCALE_LINK);
        break;
      }
      case 4:
      {
        strcpy(langCode,WEB_DEF4_LANG_CODE);
        strcpy(tmpLangLink,WEB_DEF4_LOCALE_LINK);
        break;
      }
      case 5:
      {
        strcpy(langCode,WEB_DEF5_LANG_CODE);
        strcpy(tmpLangLink,WEB_DEF5_LOCALE_LINK);
        break;
      }
      case 6:
      {
        strcpy(langCode,WEB_DEF6_LANG_CODE);
        strcpy(tmpLangLink,WEB_DEF6_LOCALE_LINK);
        break;
      }
      case 7:
      {
        strcpy(langCode,WEB_DEF7_LANG_CODE);
        strcpy(tmpLangLink,WEB_DEF7_LOCALE_LINK);
        break;
      }
    }
    memset(langLink,0,sizeof(langLink));
    usmWebConvertHexToUnicode(tmpLangLink,langLink);
    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                     OPTION,langCode,langLink);
  }

  osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",END);

  return webStaticContentBuffer;
}

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
void *ewaFormSelect_gp_id(EwsContext context, EwsFormSelectOptionP option, void *iterator)
{
  static cpId_t cpId;
  static gpId_t gpId, sGpId;
  static L7_uint32 gpId32;
  static CP_VERIFY_MODE_t verifyMode;

  EwaNetHandle net;
  L7_char8 *bufChoice;
  L7_char8 groupName[CP_USER_LOCAL_USERGROUP_MAX + 1];
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i, numBytes;
  L7_char8 data[USMWEB_APP_SMALL_DATA_SIZE];
  L7_char8 dataCP[USMWEB_APP_SMALL_DATA_SIZE];

  if (iterator == L7_NULL)
  {
    gpId = 0;
    net = ewsContextNetHandle(context);
    if (net->app_pointer != L7_NULL)
    {
      numBytes = ewsCGIQueryString(context, data, sizeof(data));
      if (numBytes > 0)
      {
        for (i=0; i<strlen(data); i++)
        {
          dataCP[i] = data[i+3];
        }
        cpId = atoi(dataCP);
      }
      else
      {
        usmWeb_AppInfo_t appInfo;
        memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
        memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
        cpId = appInfo.data[0];
      }
    }
    sGpId = 0;
    usmDbCpdmCPConfigVerifyModeGet(cpId,&verifyMode);
    rc = usmDbCpdmCPConfigGpIdGet(cpId,&sGpId);
  }
  else
  {
    rc = L7_SUCCESS;
  }

  if (L7_SUCCESS == rc)
  {
    rc = ((CP_ID_MIN<=cpId) && (CP_ID_MAX>=cpId))?L7_SUCCESS:L7_FAILURE;
  }
  if (L7_SUCCESS == rc)
  {
    rc = usmDbCpdmUserGroupEntryNextGet(gpId, &gpId);
  }
  if (L7_SUCCESS == rc)
  {
    bufChoice = ewsContextNetHandle(context)->buffer;
    memset(groupName, 0, sizeof(groupName));
    if (L7_SUCCESS == usmDbCpdmUserGroupEntryNameGet(gpId, groupName))
    {
      osapiSnprintf(bufChoice,APP_BUFFER_SIZE-1,"%d-%s",gpId,groupName);
    }
    bufChoice[APP_BUFFER_SIZE-1] = '\0';
    option->choice = bufChoice;
    gpId32 = (L7_uint32) gpId;
    option->valuep = (void *)&gpId32;
    option->selected = (gpId==sGpId)?TRUE:FALSE;

    /* Select first choice for guests, just because it looks better */
    if ((CP_VERIFY_MODE_GUEST==verifyMode) && (GP_ID_MIN==gpId))
    {
      option->selected = TRUE;
    }
    return (void *) option;
  }
  return L7_NULL;
}

/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext         context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpCfgMgmtP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpCfgMgmt(EwsContext context, EwaForm_cpCfgMgmtP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 value, cpId32;
  L7_uint32 bytes;
  cpId_t cpId;
  L7_char8 buf[USMWEB_BUFFER_SIZE_256];
  L7_char8 urlBuf[USMWEB_BUFFER_SIZE_512];
  L7_CP_MODE_STATUS_t mode;
  L7_LOGIN_TYPE_t protocol;
  CP_VERIFY_MODE_t verifyMode;
  L7_uchar8 val;
  L7_char8 data[USMWEB_APP_SMALL_DATA_SIZE];
  L7_char8 dataCP[USMWEB_APP_SMALL_DATA_SIZE];
  L7_char8  *pStr;

  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->value.err_flag = L7_FALSE;
  form->status.err_flag |= (EW_FORM_INITIALIZED);
  cpId32 = CP_ID_MIN;
  memset(data, 0, sizeof(data));
  memset(dataCP, 0, sizeof(dataCP));

  if (ewsCGIQueryString(context, data, sizeof(data)) > 0)
  {
    pStr = strstr(data, "cp=");
    if (pStr != L7_NULLPTR)
    {
      pStr += strlen("cp=");
      cpId32 = atoi(pStr);
    }
  }
  else
  {
    net = ewsContextNetHandle(context);
    if (net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      cpId32 = appInfo.data[0];
      form->value.err_flag = appInfo.err.err_flag;
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_DYNAMIC);
    }
  }

  form->value.cpId = cpId32;
  cpId = (cpId_t) cpId32;

  /* cp instance mode */
  if (usmDbCpdmCPConfigModeGet(cpId, &mode) == L7_SUCCESS)
  {
    if (mode == L7_CP_MODE_ENABLED)
    {
      form->value.cfg_mode = L7_TRUE;
    }
    else
    {
      form->value.cfg_mode = L7_FALSE;
    }
  }

  /* configuration name */
  memset(buf, 0, sizeof(buf));
  if (usmDbCpdmCPConfigNameGet(cpId, buf) == L7_SUCCESS)
  {
    form->value.cpConfig_name = osapiStrDup(buf);
    form->status.cpConfig_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* protocol mode */
  if (usmDbCpdmCPConfigProtocolModeGet(cpId, &protocol) == L7_SUCCESS)
  {
    if (protocol == L7_LOGIN_TYPE_HTTP)
    {
      form->value.cfg_protocol = http;
    }
    else
    {
      form->value.cfg_protocol = https;
    }
  }

  /* verification */
  if (usmDbCpdmCPConfigVerifyModeGet(cpId, &verifyMode) == L7_SUCCESS)
  {
    if (verifyMode == CP_VERIFY_MODE_GUEST)
    {
      form->value.cfg_verify = guest;
    }
    else if (verifyMode == CP_VERIFY_MODE_LOCAL)
    {
      form->value.cfg_verify = local;
    }
    else
    {
      form->value.cfg_verify = radius;
    }
  }

  /* user logout mode */
  if (usmDbCpdmCPConfigUserLogoutModeGet(cpId, &val) == L7_SUCCESS)
  {
    if (val == L7_ENABLE)
    {
      form->value.cfg_user_logout_mode = L7_TRUE;
    }
    else
    {
      form->value.cfg_user_logout_mode = L7_FALSE;
    }
  }

  /* redirect mode */
  if (usmDbCpdmCPConfigRedirectModeGet(cpId, &val) == L7_SUCCESS)
  {
    if (val == L7_ENABLE)
    {
      form->value.cfg_redirect_mode = L7_TRUE;
    }
    else
    {
      form->value.cfg_redirect_mode = L7_FALSE;
    }
  }

  /* configured url */
  memset(urlBuf, 0, sizeof(urlBuf));
  if (usmDbCpdmCPConfigRedirectURLGet(cpId, urlBuf) == L7_SUCCESS)
  {
    form->value.cfg_redirect_url = osapiStrDup(urlBuf);
    form->status.cfg_redirect_url |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* radius authentication server */
  memset(buf, 0, sizeof(buf));
  if (usmDbCpdmCPConfigRadiusAuthServerGet(cpId, buf) == L7_SUCCESS)
  {
    form->value.cfg_radius_server = osapiStrDup(buf);
    form->status.cfg_radius_server |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* session timeout */
  if (usmDbCpdmCPConfigSessionTimeoutGet(cpId, &value) == L7_SUCCESS)
  {
    form->value.cfg_session_timeout = value;
    form->status.cfg_session_timeout |= (EW_FORM_INITIALIZED);
  }

  /* idle timeout */
  if (usmDbCpdmCPConfigIdleTimeoutGet(cpId, &value) == L7_SUCCESS)
  {
    form->value.cfg_idle_timeout = value;
    form->status.cfg_idle_timeout |= (EW_FORM_INITIALIZED);
  }

  /* user group */
  memset(buf, 0, sizeof(buf));
  form->value.cfg_user_group = osapiStrDup(buf);
  form->status.cfg_user_group |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  /* Max Up Rate */
  if (usmDbCpdmCPConfigUserUpRateGet(cpId, &value) == L7_SUCCESS)
  {
    form->value.cfg_max_bandwidth_up = value/8; /* convert from bits to bytes */
    form->status.cfg_max_bandwidth_up |= (EW_FORM_INITIALIZED);
  }

  /* Max Down Rate */
  if (usmDbCpdmCPConfigUserDownRateGet(cpId, &value) == L7_SUCCESS)
  {
    form->value.cfg_max_bandwidth_down = value/8; /* convert from bits to bytes */
    form->status.cfg_max_bandwidth_down |= (EW_FORM_INITIALIZED);
  }

  /* Max Output Bytes */
  if (usmDbCpdmCPConfigMaxOutputOctetsGet(cpId, &bytes) == L7_SUCCESS)
  { 
    form->value.cfg_max_output = bytes;
    form->status.cfg_max_output |= (EW_FORM_INITIALIZED);
  }

  /* Max Input Bytes */
  if (usmDbCpdmCPConfigMaxInputOctetsGet(cpId, &bytes) == L7_SUCCESS)
  {
    form->value.cfg_max_input = bytes;
    form->status.cfg_max_input |= (EW_FORM_INITIALIZED);
  }

  /* Max Total Bytes */
  if (usmDbCpdmCPConfigMaxTotalOctetsGet(cpId, &bytes) == L7_SUCCESS)
  {
    form->value.cfg_max_total = bytes;
    form->status.cfg_max_total |= (EW_FORM_INITIALIZED);
  }

  form->value.Clear = (char *)osapiStrDup(pStrInfo_common_Clr);
  form->status.Clear |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Delete = (char *)osapiStrDup(pStrInfo_common_Del);
  form->status.Delete |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Refresh = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->status.cpId |= (EW_FORM_INITIALIZED);
  form->value.refresh_in_progress = 0;
  form->status.refresh_in_progress |= (EW_FORM_INITIALIZED);
  form->value.refresh_key = 0;
  form->status.refresh_key |= (EW_FORM_INITIALIZED);
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext         context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpCfgMgmtP form    @b{(input)} pointer to the form
*
* @returns  cp_cfg_mgmt.html
*
* @comments If the new CP configuration reflects modifications to either the
*           protocol, verification mode or group name, then de-authenticate
*           the clients that are currently connected via this CP instance.
*
* @end
*********************************************************************/
L7_char8 *ewaFormSubmit_cpCfgMgmt(EwsContext context, EwaForm_cpCfgMgmtP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 buf[USMWEB_BUFFER_SIZE_256];
  L7_uint32 val;
  L7_uint32 cpId32, webId32;
  cpId_t cpId;
  webId_t webId;
  gpId_t gpId, oldGpId;
  L7_CP_MODE_STATUS_t mode, oldMode;
  L7_LOGIN_TYPE_t protocol, oldProtocol;
  CP_VERIFY_MODE_t verifyMode, oldVerifyMode;
  L7_char8 urlBuf[USMWEB_BUFFER_SIZE_512];
  L7_char8 errMsg[USMWEB_ERROR_MSG_SIZE];
  L7_char8 tmpBuf[USMWEB_APP_BUF_SIZE*2];
  L7_uint32 intfId = 0;
  L7_uint32 nextIntfId;
  L7_BOOL fCleanup = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;
  L7_char8 *pStr;
  L7_uint32 bytes;

  cpId32 = form->value.cpId;
  if (0==cpId32)
  {
    memset(tmpBuf, 0, sizeof(tmpBuf));
    if (ewsCGIQueryString(context, tmpBuf, sizeof(tmpBuf)) > 0)
    {
      pStr = strstr(tmpBuf, "cp=");
      if (pStr != L7_NULLPTR)
      {
        pStr += strlen("cp=");
        cpId32 = atoi(pStr);
      }
    }
  }

  webId32 = form->value.webId;
  if (0==webId32)
  {
    memset(tmpBuf, 0, sizeof(tmpBuf));
    if (ewsCGIQueryString(context, tmpBuf, sizeof(tmpBuf)) > 0)
    {
      pStr = strstr(tmpBuf, "web=");
      if (pStr != L7_NULLPTR)
      {
        pStr += strlen("web=");
        webId32 = atoi(pStr);
      }
    }
  }

  memset(errMsg, 0x00, sizeof(errMsg));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;
  appInfo.data[0] = cpId32;
  appInfo.data[1] = webId32;
  memcpy(net->app_pointer, &appInfo, sizeof(usmWeb_AppInfo_t));
  cpId = (cpId_t)cpId32;
  webId = (webId_t)webId32;

  /* SUBMIT - note we also submit for group actions */
  if ((form->value.refresh_key == SUBMIT) ||
      (form->value.refresh_key == GROUP_ADD) ||
      (form->value.refresh_key == GROUP_DEL) ||
      (form->value.refresh_key == GROUP_MOD))
  {
    /* cp instance mode */
    mode = (TRUE==form->value.cfg_mode)?L7_CP_MODE_ENABLED:L7_CP_MODE_DISABLED;
    if (usmDbCpdmCPConfigModeGet(cpId,&oldMode) == L7_SUCCESS)
    {
      if (mode!=oldMode)
      {
        if (usmDbCpdmCPConfigModeSet(cpId, mode) != L7_SUCCESS)
        {
          appInfo.err.err_flag = L7_TRUE;
          osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                           pStrErr_common_FailedToSet, pStrInfo_security_CPMode);
        }
      }
    }

    /* configuration name */
    memset(buf, 0, sizeof(buf));
    memcpy(buf, form->value.cpConfig_name, strlen(form->value.cpConfig_name));
    if (L7_SUCCESS != usmDbStringAlphaNumericCheck(buf))
    {
	  appInfo.err.err_flag = L7_TRUE;
	  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s. %s\r\n", 
					   pStrErr_common_FailedToSet, pStrInfo_security_ConfigurationName,
                       pStrInfo_security_CPNameMustContainAlnumCharsOnly);
    }
    else
    {
      if (L7_SUCCESS != usmDbCpdmCPConfigNameSet(cpId, buf))
      {
		appInfo.err.err_flag = L7_TRUE;
		osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
						 pStrErr_common_FailedToSet, pStrInfo_security_ConfigurationName);
      }
    }

    /* protocol mode */
    if (form->value.cfg_protocol == http)
    {
      protocol = L7_LOGIN_TYPE_HTTP;
    }
    else
    {
      protocol = L7_LOGIN_TYPE_HTTPS;
    }

    if ((usmDbCpdmCPConfigProtocolModeGet(cpId, &oldProtocol) == L7_SUCCESS) && (protocol!=oldProtocol))
    {
	  if (L7_SUCCESS==usmDbCpdmCPConfigProtocolModeSet(cpId, protocol))
      {
        fCleanup = L7_TRUE;
      }
      else
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_ProtocolMode);
      }
    }

    /* verification */
	if (form->value.cfg_verify == guest)
	{
	  verifyMode = CP_VERIFY_MODE_GUEST;
	}
	else if (form->value.cfg_verify == local)
	{
	  verifyMode = CP_VERIFY_MODE_LOCAL;
	}
	else
	{
	  verifyMode = CP_VERIFY_MODE_RADIUS;
	}
    usmDbCpdmCPConfigVerifyModeGet(cpId,&oldVerifyMode);
    if (verifyMode!=oldVerifyMode)
    {
      if (L7_SUCCESS==usmDbCpdmCPConfigVerifyModeSet(cpId, verifyMode))
      {
        fCleanup = L7_TRUE;
      }
      else
	  {
	    appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_VerificationMode);
      }
	}

    /* user logout mode */
    if (form->value.cfg_user_logout_mode == TRUE)
    {
      val = L7_ENABLE; 
    } 
    else
    {
      val = L7_DISABLE;
    }
    if (usmDbCpdmCPConfigUserLogoutModeSet(cpId, val) != L7_SUCCESS)
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_UserLogoutMode);
    }

    /* radius authentication server */
    usmDbCpdmCPConfigVerifyModeGet(cpId,&verifyMode);
    if (verifyMode == CP_VERIFY_MODE_RADIUS)
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cfg_radius_server, strlen(form->value.cfg_radius_server));

      if (L7_SUCCESS != usmDbStringAlphaNumericPlusCheck(buf, "_- ", L7_NULLPTR))
      {
  	    appInfo.err.err_flag = L7_TRUE;
		  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s %s.",
                           pStrErr_common_FailedToSet, pStrErr_security_RadiusSrvrMustBeAlphaNumericPlus);
      }
      else
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigRadiusAuthServerSet(cpId, buf))
        {
	  	  appInfo.err.err_flag = L7_TRUE;
		  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s %s.",
                           pStrErr_common_FailedToSet, pStrInfo_security_RadiusAuthServer);
        }
      }
    }

    /* user group */
    usmDbCpdmCPConfigVerifyModeGet(cpId,&verifyMode);
    if (CP_VERIFY_MODE_GUEST!=verifyMode)
    {
      if (L7_SUCCESS == usmDbCpdmCPConfigGpIdGet(cpId,&oldGpId))
      {
        gpId = form->value.gp_id;
        if (gpId!=oldGpId)
        {
          if (L7_SUCCESS == usmDbCpdmCPConfigGpIdSet(cpId,gpId))
          {
            fCleanup = L7_TRUE;
          }
          else
          {
    	    appInfo.err.err_flag = L7_TRUE;
            osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                             pStrErr_common_FailedToSet, pStrInfo_security_UserGroup);
          }
        }
      }
    }

    /* redirect mode */
	if (form->value.cfg_redirect_mode == TRUE)
	{
	  val = L7_ENABLE; 
	} 
	else
	{
	  val = L7_DISABLE;
	}
	if (usmDbCpdmCPConfigRedirectModeSet(cpId, val) != L7_SUCCESS)
	{
	  appInfo.err.err_flag = L7_TRUE;
	  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
					   pStrErr_common_FailedToSet, pStrInfo_security_CfgURLRedirectMode);
	}

    /* configured url */
    if (form->value.cfg_redirect_mode == TRUE)
    {
      memset(urlBuf, 0, sizeof(urlBuf));
      memcpy(urlBuf, form->value.cfg_redirect_url, strlen(form->value.cfg_redirect_url));
      if (usmDbCpdmCPConfigRedirectURLSet(cpId, urlBuf) != L7_SUCCESS)
      {
		appInfo.err.err_flag = L7_TRUE;
		osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
						 pStrErr_common_FailedToSet, pStrInfo_security_CfgURL);
      }
    }

    /* session timeout */
    if (form->status.cfg_session_timeout & EW_FORM_PARSE_ERROR)
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s%s\r\n", 
                       pStrErr_common_Error, pStrInfo_security_Session_Timeout, pStrInfo_security_FieldInvalidFmt);
    }
    else if ((form->value.cfg_session_timeout < CP_USER_LOCAL_SESSION_TIMEOUT_MIN) || 
             (form->value.cfg_session_timeout > CP_USER_LOCAL_SESSION_TIMEOUT_MAX))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintf(tmpBuf, sizeof(tmpBuf), pStrErr_common_FieldOutOfRange, 
                    form->value.cfg_session_timeout, pStrInfo_security_Session_Timeout, 
                    CP_USER_LOCAL_SESSION_TIMEOUT_MIN, CP_USER_LOCAL_SESSION_TIMEOUT_MAX);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s\r\n", 
                       pStrErr_common_Error, tmpBuf);
    }
    else
    {
      if (usmDbCpdmCPConfigSessionTimeoutSet(cpId, form->value.cfg_session_timeout) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_Session_Timeout);
      }
    }

    /* idle timeout */
	if (form->status.cfg_idle_timeout & EW_FORM_PARSE_ERROR)
	{
	  appInfo.err.err_flag = L7_TRUE;
	  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s%s\r\n", 
					   pStrErr_common_Error, pStrInfo_security_Idle_Timeout, pStrInfo_security_FieldInvalidFmt);
	}
	else if ((form->value.cfg_idle_timeout < CP_USER_LOCAL_IDLE_TIMEOUT_MIN) || 
			 (form->value.cfg_idle_timeout > CP_USER_LOCAL_IDLE_TIMEOUT_MAX))
	{
	  appInfo.err.err_flag = L7_TRUE;
	  osapiSnprintf(tmpBuf, sizeof(tmpBuf), pStrErr_common_FieldOutOfRange, 
					form->value.cfg_idle_timeout, pStrInfo_security_Idle_Timeout, 
					CP_USER_LOCAL_IDLE_TIMEOUT_MIN, CP_USER_LOCAL_IDLE_TIMEOUT_MAX);
	  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s\r\n", 
					   pStrErr_common_Error, tmpBuf);
	}
	else
	{
	  if (usmDbCpdmCPConfigIdleTimeoutSet(cpId, form->value.cfg_idle_timeout) != L7_SUCCESS)
	  {
		appInfo.err.err_flag = L7_TRUE;
		osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
						 pStrErr_common_FailedToSet, pStrInfo_security_Idle_Timeout);
	  }
	}

    /* Max Up Rate */
    bytes = form->value.cfg_max_bandwidth_up;
	if (form->status.cfg_max_bandwidth_up & EW_FORM_PARSE_ERROR)
	{
	  appInfo.err.err_flag = L7_TRUE;
	  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s%s\r\n", 
					   pStrErr_common_Error, pStrInfo_security_MaxBwUp, pStrInfo_security_FieldInvalidFmt);
	}
	else if ((bytes < CP_USER_LOCAL_MAX_BW_UP_MIN) || 
			 (bytes > CP_USER_LOCAL_MAX_BW_UP_MAX))
	{
	  appInfo.err.err_flag = L7_TRUE;
	  osapiSnprintf(tmpBuf, sizeof(tmpBuf), pStrErr_common_FieldOutOfRange, 
					bytes, pStrInfo_security_MaxBwUp, 
					CP_USER_LOCAL_MAX_BW_UP_MIN, CP_USER_LOCAL_MAX_BW_UP_MAX);
	  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s\r\n", 
					   pStrErr_common_Error, tmpBuf);
	}
	else
	{
	  if (usmDbCpdmCPConfigUserUpRateSet(cpId, bytes*8) != L7_SUCCESS)
	  {
		appInfo.err.err_flag = L7_TRUE;
		osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
						 pStrErr_common_FailedToSet, pStrInfo_security_MaxBwUp);
	  }
	}
  
    /* Max Down Rate */
    bytes = form->value.cfg_max_bandwidth_down;
	if (form->status.cfg_max_bandwidth_down & EW_FORM_PARSE_ERROR)
	{
	  appInfo.err.err_flag = L7_TRUE;
	  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s%s\r\n", 
					   pStrErr_common_Error, pStrInfo_security_MaxBwDown, pStrInfo_security_FieldInvalidFmt);
	}
	else if ((bytes < CP_USER_LOCAL_MAX_BW_DOWN_MIN) || 
			 (bytes > CP_USER_LOCAL_MAX_BW_DOWN_MAX))
	{
	  appInfo.err.err_flag = L7_TRUE;
	  osapiSnprintf(tmpBuf, sizeof(tmpBuf), pStrErr_common_FieldOutOfRange, 
					bytes, pStrInfo_security_MaxBwDown, 
					CP_USER_LOCAL_MAX_BW_DOWN_MIN, CP_USER_LOCAL_MAX_BW_DOWN_MAX);
	  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s\r\n", 
					   pStrErr_common_Error, tmpBuf);
	}
	else
	{
	  if (usmDbCpdmCPConfigUserDownRateSet(cpId, bytes*8) != L7_SUCCESS)
	  {
		appInfo.err.err_flag = L7_TRUE;
		osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
						 pStrErr_common_FailedToSet, pStrInfo_security_MaxBwDown);
	  }
	}
  
    /* Max Output Bytes */
    cpIntegerSetHelp(form->status.cfg_max_output, form->value.cfg_max_output, 
                         pStrInfo_security_MaxOutputOctets, 
                         CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MIN, CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MAX, 
                         &appInfo, usmDbCpdmCPConfigMaxOutputOctetsSet, cpId);

    /* Max Input Bytes */
    cpIntegerSetHelp(form->status.cfg_max_input, form->value.cfg_max_input, 
                         pStrInfo_security_MaxInputOctets, 
                         CP_USER_LOCAL_MAX_INPUT_OCTETS_MIN, CP_USER_LOCAL_MAX_INPUT_OCTETS_MAX, 
                         &appInfo, usmDbCpdmCPConfigMaxInputOctetsSet, cpId);

    /* Max Total Bytes */
    cpIntegerSetHelp(form->status.cfg_max_total, form->value.cfg_max_total, 
                         pStrInfo_security_MaxTotalOctets, 
                         CP_USER_LOCAL_MAX_TOTAL_OCTETS_MIN, CP_USER_LOCAL_MAX_TOTAL_OCTETS_MAX, 
                         &appInfo, usmDbCpdmCPConfigMaxTotalOctetsSet, cpId);

    /* Add or Update web locales */
    rc = addOrUpdateWebLocale(cpId,1,form->value.cp_code1,form->value.cp_link1);
    if (L7_SUCCESS == rc)
    {
      rc = addOrUpdateWebLocale(cpId,2,form->value.cp_code2,form->value.cp_link2);
    }
    if (L7_SUCCESS == rc)
    {
      rc = addOrUpdateWebLocale(cpId,3,form->value.cp_code3,form->value.cp_link3);
    }
    if (L7_SUCCESS == rc)
    {
      rc = addOrUpdateWebLocale(cpId,4,form->value.cp_code4,form->value.cp_link4);
    }
    if (L7_SUCCESS == rc)
    {
      rc = addOrUpdateWebLocale(cpId,5,form->value.cp_code5,form->value.cp_link5);
    }
    if (L7_SUCCESS != rc)
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_Language);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_LOCALE_LINK_MAX);
    }

    /* 
     * Remove all clients currently connected via this CP instance if the modifications
     * just made require a client cleanup.
     */
    if (L7_TRUE==fCleanup)
    {
      cpId_t next_cpId = 0;
      nextIntfId = 0;
      while ((L7_SUCCESS == 
              usmDbCpdmCPConfigIntIfNumNextGet(cpId, intfId, &next_cpId, &nextIntfId))
             && (cpId == next_cpId))
      {
        usmDbCpdmClientConnStatusIntfDeleteAll(nextIntfId);
        intfId = nextIntfId;
      }
    }
  }

  /* DELETE CP INSTANCE */
  if (form->status.Delete & EW_FORM_RETURNED)
  {
    if (form->value.refresh_key == DELETE)
    {
      if (usmDbCpdmCPConfigDelete(cpId) != L7_SUCCESS)
      {
        usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrErr_common_FailedToDel, 
                               pStrInfo_security_CPConfig);
      }

      memcpy(net->app_pointer, &appInfo, sizeof(usmWeb_AppInfo_t));
      if (appInfo.err.err_flag == L7_FALSE)
      {
        ewsContextSendReply(context, pStrInfo_security_HtmlFileCPCfg);
      }
      else
      {
        ewsContextSendReply(context, pStrInfo_security_HtmlFileCPCfgMgmt);
      }
      return L7_NULL;
    }
  }
  /* CLEAR/RESET CP INSTANCE */
  if (form->status.Clear & EW_FORM_RETURNED)
  {
    if (form->value.refresh_key == CLEAR)
    {
      if (usmDbCpdmCPConfigReset(cpId) != L7_SUCCESS)
      {
        usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrErr_common_FailedToClr, 
                               pStrInfo_security_CPConfig);
      }
    }
  }
  /* ADD USER GROUP */
  if (form->value.refresh_key == GROUP_ADD)
  {
	memset(buf, 0, sizeof(buf));
	memcpy(buf, form->value.cfg_user_group, strlen(form->value.cfg_user_group));
    /* Check format */
    if ((0<strlen(buf)) && (L7_SUCCESS != usmDbStringAlphaNumericCheck(buf)))
	{
	  appInfo.err.err_flag = L7_TRUE;
	  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrErr_common_FailedToAdd, pStrInfo_security_UserGroup);
      osapiStrncat(appInfo.err.msg,".\r\n",(sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1));
      osapiStrncat(appInfo.err.msg,pStrInfo_security_UserGroupMustContainAlnumCharsOnly,
                   (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1));
	}
    /* check if group name already exists */
    if (L7_TRUE != appInfo.err.err_flag)
    {
      if (L7_SUCCESS==usmDbCpdmUserGroupEntryByNameGet(buf,&gpId))
      {
	    appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                         pStrErr_common_FailedToAdd, pStrInfo_security_UserGroup);
        osapiStrncat(appInfo.err.msg,".\r\n",(sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1));
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                         pStrInfo_security_UserGroupExists,buf);
      }
    }
    if (L7_TRUE != appInfo.err.err_flag)
    {
      /* Get next available group id */
      gpId = GP_ID_MIN;
      rc = L7_SUCCESS;
      while ((L7_SUCCESS==rc) && (gpId <= GP_ID_MAX))
      {
        rc = usmDbCpdmUserGroupEntryGet(gpId);
        if (L7_SUCCESS==rc)
        {
          gpId++;
        }
      }
      /* Add new group id */
      if (L7_SUCCESS!=usmDbCpdmUserGroupEntryAdd(gpId))
      {
	    appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), 
                         pStrInfo_security_FailedToAddUserGroup,buf);
      }
      /* Set new group name */
      if (L7_TRUE!=appInfo.err.err_flag)
      {
        if (L7_SUCCESS==usmDbCpdmUserGroupEntryNameSet(gpId,buf))
        {
	      appInfo.err.err_flag = L7_TRUE;
          osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                           pStrInfo_security_AddedUserGroup,buf);
        }
        else
        {
	      appInfo.err.err_flag = L7_TRUE;
          osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                           pStrInfo_security_FailedToAddUserGroup,buf); 
        }
      }
    }
  }
  /* DELETE USER GROUP */
  if (form->value.refresh_key == GROUP_DEL)
  {
    gpId = form->value.gp_id;
    /* First, replace the 'to be' deleted group associations with the default group */
    usmDbCpdmUserGroupAssocEntryGroupRename(gpId,GP_ID_MIN);

    /* Now delete the group */
    if (L7_SUCCESS!=usmDbCpdmUserGroupEntryDelete(gpId))
    {
      appInfo.err.err_flag = L7_TRUE;
      memset(buf,0,sizeof(buf));
      if (usmDbCpdmUserGroupEntryNameGet(gpId,buf) != L7_SUCCESS)
      {
        memset(buf,0,sizeof(buf)); /* shutup coverity */
      }
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_FailedToDelUserGroup,buf); 
    }
  }
  /* MODIFY USER GROUP */
  if (form->value.refresh_key == GROUP_MOD)
  {
	memset(buf, 0, sizeof(buf));
	memcpy(buf, form->value.cfg_user_group, strlen(form->value.cfg_user_group));
    /* Check format */
    if ((0<strlen(buf)) && (L7_SUCCESS != usmDbStringAlphaNumericCheck(buf)))
	{
	  appInfo.err.err_flag = L7_TRUE;
	  osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
					   pStrErr_common_FailedToSet, pStrInfo_security_UserGroup,
                       pStrInfo_security_UserGroupMustContainAlnumCharsOnly);
	}
    /* check if group name already exists */
    if (L7_TRUE != appInfo.err.err_flag)
    {
      gpId = form->value.gp_id;
      if (L7_SUCCESS==usmDbCpdmUserGroupEntryByNameGet(buf,&gpId))
      {
	    appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                         pStrErr_common_FailedToSet, pStrInfo_security_UserGroup);
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                         pStrInfo_security_UserGroupExists,buf);
      }
      if (L7_TRUE != appInfo.err.err_flag)
      {
        if (L7_SUCCESS==usmDbCpdmUserGroupEntryNameSet(gpId,buf))
        {
	      appInfo.err.err_flag = L7_TRUE;
          osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                           pStrInfo_security_AddedUserGroup,buf);
        }
        else
        {
          appInfo.err.err_flag = L7_TRUE;
          osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                           pStrErr_common_FailedToSet, pStrInfo_security_UserGroup);
        }
      }
    }
  }

  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPCfgMgmt);
}

