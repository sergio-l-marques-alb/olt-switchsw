/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/base/web_utils.c
 *
 * @purpose string utilites
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/01/2007
 *
 * @author  Rama Sasthri kristipati
 * @end
 *
 **********************************************************************/

#include "l7_common.h"
#include "web.h"

#include "ew_proto.h"
#include "ew_config.h"
#include "ewnet.h"

L7_uint32 usmWebL7EnDisToEnDisGet (L7_int32 val, L7_uint32 def)
{
  switch (val)
  {
  case L7_ENABLE:
    return Enable;
    /* pass-through */
  case L7_DISABLE:
    return Disable;
    /* pass-through */
  default:
    break;
  }
  return def;
}

L7_uint32 usmWebL7AdminModeToEnDisGet (L7_int32 val, L7_uint32 def)
{
  switch (val)
  {
  case L7_ENABLE:
    return Enable;
    /* pass-through */
  case L7_DISABLE:
    return Disable;
    /* pass-through */
  default:
    break;
  }
  return def;
}

void usmWebAppInfoCatErrMsg (usmWeb_AppInfo_t * appInfop, L7_char8 * prefix, L7_char8 * fmt, ...)
{
  L7_int32    rc;
  L7_uint32   len = sizeof(appInfop->err.msg) - 1;;
  va_list     ap;
  L7_char8    errMsg[USMWEB_ERROR_MSG_SIZE];

  memset (&ap, 0, sizeof (ap));
  va_start (ap, fmt);
  rc = osapiVsnprintf (errMsg, sizeof (errMsg), fmt, ap);
  va_end (ap);

  appInfop->err.err_flag = L7_TRUE;
  if (L7_NULLPTR != prefix)  
  {
    osapiSnprintfcat (appInfop->err.msg, len, "%s %s\r\n", prefix, errMsg);
  }
  else
  {
    osapiSnprintfcat (appInfop->err.msg, len, "%s\r\n", errMsg);
  }
}

L7_char8 *usmWebEwsContextSendReply (EwsContext context, L7_BOOL create,
                                     usmWeb_AppInfo_t * appInfop, L7_char8 * file)
{
  EwaNetHandle net = ewsContextNetHandle (context);

  if(create == L7_TRUE)
  {
    net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
    memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  }

  if(appInfop != L7_NULLPTR)
  {
    memcpy (net->app_pointer, appInfop, sizeof (usmWeb_AppInfo_t));
  }
  ewsContextSendReply (context, file);
  return L7_NULL;
}


