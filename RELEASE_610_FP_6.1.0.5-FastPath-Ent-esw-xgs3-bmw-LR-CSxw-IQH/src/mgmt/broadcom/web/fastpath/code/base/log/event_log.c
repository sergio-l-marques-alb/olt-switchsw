/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/base/system/event_log.c
 *
 * @purpose Code in support of the fwd_db_search.html page
 *
 * @component unitmgr
 *
 * @comments none
 *
 * @create 01/29/2001
 *
 * @author jlshaw
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_base_common.h"
#include "strlib_base_web.h"
#include <l7_common.h>
#include <stdio.h>
#include <ew_proto.h>
#include <ewnet.h>
#include <ews_ctxt.h>
#include <web.h>
#include <log.h>
#include "web_oem.h"

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_event_log ( EwsContext context, EwaForm_event_logP form )
{
  /* Sets initial form data */

  form->value.Refresh = (char *) osapiStrDup( pStrInfo_common_Refresh );
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  return;
}

/*********************************************************************
*
* @purpose Refreshes the data in the fdb table
*
* @param context EmWeb/Server request context handle
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_event_log ( EwsContext context, EwaForm_event_logP form )
{

  if (form->status.Refresh & EW_FORM_RETURNED)
  {
    ewsContextSendReply(context, pStrInfo_base_HtmlFileEventLog);
    return NULL;
  }

  L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID, "#  Form Submission Failed. "
				"No Action Taken. The form submission failed and no action is taken. # "
				"indicates the file under consideration.\n");
  return NULL;

}
