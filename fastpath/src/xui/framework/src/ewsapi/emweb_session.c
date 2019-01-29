/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename session.c
*
* @purpose Code to support cookie based sessions
*
* @component
*
* @comments tba
*
* @create 01/13/2005
*
* @author cpverne
* @end
*
**********************************************************************/

#include <string.h>
#include <stdlib.h>
#include "ew_types.h"
#include "ews.h"
#include "ewnet.h"
#include "ews_ctxt.h"
#include "session.h"
#include "user_manager_exports.h"
#include "usmdb_sim_api.h"
#include "usmdb_user_mgmt_api.h"

void ewaCopyInetAddrFromContext(EwsContext context, L7_inet_addr_t *ipAddrPtr);

char *emweb_session_clear(EwsContext context)
{
  EwaSession *session = (EwaSession *) context->session;

  if (session != NULL)
  {
    EwaSessionRemove (session->sid);
  }

  return "";
}

char *emweb_session_getsid(EwsContext context, L7_uint32 *last)
{
  EwaSession *session = (EwaSession *) context->session;

  if (session != NULL)
  {
    *last = session->last_access;
    return session->sid;
  }

  return "";
}

int emweb_session_login(EwsContext context, char *user, char *passwd)
{
  L7_uint32 accessLevel= FD_USER_MGR_GUEST_ACCESS_LEVEL;
  char * session_id = NULL;
  L7_ushort16 sessionType;
  L7_inet_addr_t ipAddr;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 unit;
  L7_ACCESS_LINE_t accessLine = ACCESS_LINE_HTTP;
  L7_LOGIN_TYPE_t loginType;
	
  unit = usmDbThisUnitGet();


  EwaNetHandle net = ewsContextNetHandle (context);
  EwaSession *session = (EwaSession *) context->session;
	
  if (net->connection_type == EW_CONNECTION_HTTPS)
  {
    accessLine = ACCESS_LINE_HTTPS;
    sessionType = L7_LOGIN_TYPE_HTTPS;
  }
  else
  {
    sessionType = L7_LOGIN_TYPE_HTTP;
  }


  rc = usmDbAuthenticateUser(unit, user,passwd, &accessLevel, accessLine, 
                                   ACCESS_LEVEL_LOGIN, L7_NULLPTR, L7_NULLPTR);
  if (rc != L7_SUCCESS)
  {
    if (session != NULL)
    {
      EwaSessionRemove (session->sid);
    }
    return 1;
  }

  if (net->connection_type == EW_CONNECTION_HTTPS)
  {
    sessionType = L7_LOGIN_TYPE_HTTPS;
    session_id = (char *)ewsContextCookieValue (context, "SIDSSL");
    loginType=L7_LOGIN_TYPE_HTTPS;
  }
  else
  {
    sessionType = L7_LOGIN_TYPE_HTTP;
    session_id = (char *)ewsContextCookieValue (context, "SID");
    loginType=L7_LOGIN_TYPE_HTTP;
  }

  ewaCopyInetAddrFromContext(context, &ipAddr);

  context->session = session = EwaSessionLookup (session_id, sessionType, ipAddr, L7_TRUE);
  if (session != NULL)
  {
    osapiStrncpySafe (session->uname, user, sizeof(session->uname));
    osapiStrncpySafe (session->pwd, passwd, sizeof(session->pwd));
    session->access_level = accessLevel;
  }
  else
  {
    return 1;
  }
  rc=usmdbUserMgrAddToLoginHistory(unit,user,loginType,&ipAddr);

  return 0;
}

#if 0
void EwaSessionSendCookie (EwsContext context, EwsStringP estring)
{
  EwaSession *session = (EwaSession *) context->session;

  if ((session != NULL) && (session->sendCookie == L7_TRUE))
  {
    L7_BOOL validCookie = L7_FALSE;
    if (session->type == L7_LOGIN_TYPE_HTTPS)
    {
      (void) ewsStringCopyIn (estring, "\r\nSet-Cookie: SIDSSL=");
      validCookie = L7_TRUE;
    }
    else if (session->type == L7_LOGIN_TYPE_HTTP)
    {
      (void) ewsStringCopyIn (estring, "\r\nSet-Cookie: SID=");
      validCookie = L7_TRUE;
    }
    else
    {
      validCookie = L7_FALSE;
    }
    if (validCookie == L7_TRUE)
    {
      (void) ewsStringCopyIn (estring, session->sid);
      (void) ewsStringCopyIn (estring, ";PATH=/");
    }
    session->sendCookie = L7_FALSE;
  }
}

boolean EwaSessionCheckAccess (EwsContext context, EwsDocument document)
{
  L7_uint32 realmID;
  boolean bAllowAccess;
  EwaSession *session;

  if (document->realmp == NULL)
    return TRUE;

  if (strcmp (document->realmp->realm, "READWRITE") == 0)
    realmID = L7_LOGIN_ACCESS_READ_WRITE;
  else if (strcmp (document->realmp->realm, "READONLY") == 0)
    realmID = L7_LOGIN_ACCESS_READ_ONLY;
  else
    realmID = L7_LOGIN_ACCESS_NONE;

  session = (EwaSession *) context->session;
  if (session == NULL)
  {
    bAllowAccess = L7_FALSE;
  }
  else
  {
    switch (session->access_level)
    {
    case L7_LOGIN_ACCESS_READ_WRITE:   /* READWRITE users get access to it all */
      if ((realmID == L7_LOGIN_ACCESS_READ_WRITE) || (realmID == L7_LOGIN_ACCESS_READ_ONLY))
        bAllowAccess = L7_TRUE;
      else
        bAllowAccess = L7_FALSE;
      break;
    case L7_LOGIN_ACCESS_READ_ONLY:
      if (realmID == L7_LOGIN_ACCESS_READ_ONLY)
        bAllowAccess = L7_TRUE;
      else
        bAllowAccess = L7_FALSE;
      break;
    case L7_LOGIN_ACCESS_NONE:
      bAllowAccess = L7_FALSE;
      break;
    default:
      bAllowAccess = L7_FALSE;
      break;
    }
  }

  if (bAllowAccess == L7_FALSE)
  {
    return FALSE;
  }

  context->auth_state = ewsAuthStateUnauthenticated;
  context->substate.setup = ewsContextServeSetupAuthDone;

  return TRUE;
}

boolean EwsCheckHttpEnable (EwsContext context)
{
  L7_uint32 unit;
  L7_uint32 web_mode;
  EwaNetHandle net;

  if (usmDbUnitMgrMgrNumberGet (&unit) == L7_SUCCESS)
  {
    if (usmDbSwDevCtrlWebMgmtModeGet (unit, &web_mode) == L7_SUCCESS)
    {
      if (web_mode != L7_ENABLE)
      {
        net = ewsContextNetHandle (context);
        if ((net == NULL) || (net->connection_type != EW_CONNECTION_HTTPS))
        {
          return FALSE;
        }
      }
    }
  }
  return TRUE;
}

L7_BOOL EwsCheckUnitMgr(void)
{
  L7_uint32 curr_unit = 0;
  L7_uint32 mgmt_unit = 0;

  if (usmDbUnitMgrMgrNumberGet(&mgmt_unit) == L7_SUCCESS &&
      usmDbUnitMgrNumberGet(&curr_unit) == L7_SUCCESS)
  {
    if (mgmt_unit == curr_unit)
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}
#endif

static int emwebDebugAccessLevel = 0;
int emwebUserAccessGet(EwsContext context)
{
  L7_uint32      userAccess = 64;
  EwaSession *session = (EwaSession *) context->session;

  if(emwebDebugAccessLevel != 0)
    return emwebDebugAccessLevel;

  if (session != NULL && session->active == L7_TRUE)
  {
    switch (session->access_level)
    {
      case L7_LOGIN_ACCESS_READ_WRITE:  /* READWRITE users get access to it all */
        userAccess = 128;
        break;
      case L7_LOGIN_ACCESS_READ_ONLY:
        userAccess = 64;
        break;
      case L7_LOGIN_ACCESS_NONE:
        userAccess = 64;
        break;
      default:
        userAccess = 64;
        break;
    }
  }

  return userAccess;

}

static int ewsServeAuthBypass = 0;
int ewsServeAuthBypassSet(int val)
{
  int old = ewsServeAuthBypass;
  ewsServeAuthBypass = val;
  return old;
}
int ewsServeAuthBypassGet(void)
{
  return (ewsServeAuthBypass);
}

int emwebDebugAccessSet(int val)
{
  int old = emwebDebugAccessLevel;
  emwebDebugAccessLevel = val;
  ewsServeAuthBypassSet(val);
  return old;
}

