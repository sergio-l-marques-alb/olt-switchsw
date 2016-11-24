/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2007
 *
 **********************************************************************
*
* @filename cp_welcome.html
*
* @purpose  User authentication success notify page
*
* @comments 
*
* @create 12-16-2008
*
* @author darsen
* @end
*
 **********************************************************************/

#include "ews_ctxt.h"
#include "ew_proto.h"
#include "ewnet.h"
#include "web.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"


/*****************************************************************************
 ****************************************************************************/
void ewaFormServe_welcome ( EwsContext context, EwaForm_welcomeP form )
{
}

/******************************************************************************
******************************************************************************/
char *ewaFormSubmit_welcome ( EwsContext context, EwaForm_welcomeP form )
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
  ewsContextSendReply(context,CP_URL_WELCOME_PATH);
  return NULL;
}



