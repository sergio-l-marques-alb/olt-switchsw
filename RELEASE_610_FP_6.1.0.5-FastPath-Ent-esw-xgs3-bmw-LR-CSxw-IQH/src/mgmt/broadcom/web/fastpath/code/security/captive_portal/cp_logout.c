/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2007
 *
 **********************************************************************
 *
 * @filename     cp_logout.c
 *
 * @purpose      Allow user to de-authenticate
 *               
 *
 * @component captive portal
 *
 * @comments
 *
 * @create    12/16/2008
 *
 * @author    darsen
 *
 * @end
 *
 **********************************************************************/

#include "ews_ctxt.h"
#include "ew_proto.h"
#include "ewnet.h"
#include "web.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_web_api.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"


extern L7_RC_t getLangWebId(cpId_t cpId, L7_char8 *lang, L7_short16 *webId);

/*****************************************************************************
 ****************************************************************************/
void ewaFormServe_userLogout ( EwsContext context, EwaForm_userLogoutP form )
{
  EwaNetHandle net;
  cpAppInfo_t appInfo;
  L7_char8 lang[USMWEB_BUFFER_SIZE_64];     /* from user or browser accept */
  L7_char8 tmp[CP_AUP_TEXT_MAX+1];          /* max UTF-16 format */
  L7_char8 buf[CP_HEX_NCR_MAX+1];           /* max Hexadecimal NCR */
  CP_AUTH_STATUS_FLAG_t flag;
  cpId_t cpId = 0;
  webId_t webId = 0;
  L7_IP_ADDR_t ip;
  L7_uint32 intf;

  net = ewsContextNetHandle(context);
  if (net->app_pointer != NULL)
  {
    memset(lang,0,sizeof(lang));
    memset(&appInfo,0,sizeof(cpAppInfo_t));
    memcpy(&appInfo,net->app_pointer,sizeof(cpAppInfo_t));
    flag = (CP_AUTH_STATUS_FLAG_t) appInfo.data[CP_APPINFO_FLAG_IDX];
    intf = (L7_uint32) appInfo.data[CP_APPINFO_INTF_IDX];
    ip   = (L7_IP_ADDR_t) appInfo.data[CP_APPINFO_IP_IDX];
    osapiStrncpy(lang,appInfo.lang,USMWEB_BUFFER_SIZE_64);
  }
  else
  {
    return; /* will always have a context */
  }

  if (CP_PREVIEW == flag)
  {
    cpId  = (cpId_t) appInfo.data[CP_APPINFO_INTF_IDX];
    webId = (webId_t) appInfo.data[CP_APPINFO_IP_IDX];
  }
  else
  {
    /* Get associated configuration and web id */
    if (L7_SUCCESS != usmDbCpdmCPConfigIntIfNumFind(intf,&cpId))
    {
      return; /* Can't go on.. */
    }
    getLangWebId(cpId,lang,&webId);
  }

  /* button label */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebLogoutButtonLabelGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);

    form->value.Submit = osapiStrDup(buf);
    form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }
  /* Save hidden parms */
  form->value.p1 = flag;
  form->value.p2 = intf;
  form->value.p3 = ip;
  form->value.p4 = (char *) osapiStrDup(lang);
  form->status.p1 |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->status.p2 |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->status.p3 |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->status.p4 |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
}

/******************************************************************************
******************************************************************************/
char *ewaFormSubmit_userLogout ( EwsContext context, EwaForm_userLogoutP form )
{
  EwaNetHandle net;
  cpAppInfo_t appInfo;

  memset(&appInfo,0,sizeof(cpAppInfo_t));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(cpAppInfo_t));

  /* Save params from the form */
  appInfo.data[CP_APPINFO_FLAG_IDX] = (L7_uint32) form->value.p1;       /* flag */
  appInfo.data[CP_APPINFO_INTF_IDX] = (L7_uint32) form->value.p2;       /* intf or cpid */
  appInfo.data[CP_APPINFO_IP_IDX]   = (L7_uint32) form->value.p3;       /* ip or webid */
  if (NULL!=form->value.p4)
  {
    osapiStrncpy(appInfo.lang,form->value.p4,USMWEB_BUFFER_SIZE_64);    /* lang */
  }
  memcpy(net->app_pointer, &appInfo, sizeof(cpAppInfo_t));

  if (CP_LOGOUT==form->value.p1)
  {
    /* De-authenticate the user */
    L7_enetMacAddr_t macAddr;

    memset(&macAddr,0,sizeof(L7_enetMacAddr_t));
    if (L7_SUCCESS==usmDbCpdmClientConnStatusFirstIPMatch(form->value.p3,&macAddr))
    {
      if (L7_SUCCESS==usmDbCpdmClientConnStatusUserLogoutFlagSet(&macAddr,L7_CP_USER_LOGOUT))
      {
        ewsContextSendReply(context,CP_URL_SUCCESS_PATH);
      }
    }
  }
  else
  {
    ewsContextSendReply(context,CP_URL_LOGOUT_PATH);
  }
  return NULL;
}
