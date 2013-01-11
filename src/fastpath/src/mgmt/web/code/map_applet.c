
#include "flex.h"
#include "usmdb_util_api.h"

#ifdef L7_XWEB_PACKAGE

#include "ewnet.h"
#include "ew_proto.h"

/*
int emweb_session_login(EwsContext context, char *user, char *passwd);
void emweb_session_clear(EwsContext context);
*/
void ewaFormServe_map_applet_port_show (EwsContext context, EwaForm_map_applet_port_showP form)
{
}

char *ewaFormSubmit_map_applet_port_show (EwsContext context, EwaForm_map_applet_port_showP form)
{
  if ((form->value.intfValid != 0) || (form->value.stackIntfValid != 0))
  {
    return NULL;
  }
  ewsContextSendReply(context, form->value.selPageToLoad);
  return NULL;
}
void ewaFormServe_full_stack_port_show( EwsContext context, EwaForm_full_stack_port_showP form  )
{
  return;
}

L7_char8 *ewaFormSubmit_full_stack_port_show( EwsContext context, EwaForm_full_stack_port_showP form )
{
  if ((form->value.intfValid != 0) || (form->value.stackIntfValid != 0))
  {
    return NULL;
  }
  ewsContextSendReply(context, form->value.selPageToLoad);
  return NULL;
}
#endif

