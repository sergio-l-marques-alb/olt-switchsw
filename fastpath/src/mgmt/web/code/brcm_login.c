
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename brcm_login.c
*
* @purpose Login Form Handling
*
* @component WEB
*
* @comments
*
* @create 25-APR-2007
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include "flex.h"

#ifdef L7_XWEB_PACKAGE

#include "ewnet.h"
#include "ew_proto.h"

int emweb_session_login(EwsContext context, char *user, char *passwd);
void emweb_session_clear(EwsContext context);

void ewaFormServe_login (EwsContext context, EwaForm_loginP form)
{
}

char *ewaFormSubmit_login (EwsContext context, EwaForm_loginP form)
{

#ifndef L7_PRODUCT_SMARTPATH
  if (!emweb_session_login (context, form->value.uname, form->value.pwd))
  {
    ewsContextSendReply (context, form->value.passpage);
  }
  else
  {
    ewsContextSendReply (context, "fastpath_login_fail.html");
  }
#else
  if(strcmp(form->value.uname,"admin") != 0)
  	ewsContextSendReply (context, "/web_main.html");
  
  if (!emweb_session_login (context, form->value.uname, form->value.pwd))
  {
    ewsContextSendReply (context, "/web_main.html");
  }
  else
  {
    ewsContextSendReply (context, "/brcm_login.html");
  }
#endif

  return NULL;
}

#ifndef L7_PRODUCT_SMARTPATH
void ewaFormServe_fail_login (EwsContext context, EwaForm_fail_loginP form)
{
}

char *ewaFormSubmit_fail_login (EwsContext context, EwaForm_fail_loginP form)
{
  if (!emweb_session_login (context, form->value.uname, form->value.pwd))
  {
    ewsContextSendReply (context, form->value.passpage);
  }
  else
  {
    ewsContextSendReply (context, "fastpath_login_fail.html");
  }
  return NULL;
}

void ewaFormServe_logout ( EwsContext context, EwaForm_logoutP form )
{
  return;
}

/*********************************************************************
 *
 * @purpose Save the current values in the form
 *
 * @param context EmWeb/Server request context handle
 * @param form    pointer to the logout form
 *
 * @returns global_navigator.html
 *
 * @end
 *
 *********************************************************************/
char *ewaFormSubmit_logout ( EwsContext context, EwaForm_logoutP form )
{
  emweb_session_clear(context);
  ewsContextSendReply (context, form->value.loginpage); 
  return NULL;
}
#endif
#endif
