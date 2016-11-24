/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_session_statistics.c
*
* @purpose    Code in support of the cp_session_statistics.html page
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
#include "usmdb_cpdm_connstatus_api.h"
#include "captive_portal_commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"


/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext              context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpSessionStatsP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpSessionStats(EwsContext context, EwaForm_cpSessionStatsP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_enetMacAddr_t selected_clientMac;
  L7_uint64 bTx, bRx, pTx, pRx;
  L7_char8 buf[USMWEB_BUFFER_SIZE_256];
  static L7_uchar8 null_mac[] = {0,0,0,0,0,0};

  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->value.err_flag = L7_FALSE;
  form->status.err_flag |= (EW_FORM_INITIALIZED);
  memset(&selected_clientMac, 0, sizeof(L7_enetMacAddr_t));

  net = ewsContextNetHandle(context);
  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    memcpy(&selected_clientMac, appInfo.buf, sizeof(L7_enetMacAddr_t));
    memcpy(form->value.mac, appInfo.buf, sizeof(L7_enetMacAddr_t));
    form->value.err_flag = appInfo.err.err_flag;
    form->value.err_msg = osapiStrDup(appInfo.err.msg);
    form->status.err_msg |= (EW_FORM_DYNAMIC);
  }
  else
  {
    if (usmDbCpdmClientConnStatusNextGet(&selected_clientMac, &selected_clientMac) == L7_SUCCESS)
    {
      memcpy(form->value.mac, selected_clientMac.addr, sizeof(L7_enetMacAddr_t));
    }
  }

  if (memcmp(&selected_clientMac, null_mac, sizeof(L7_enetMacAddr_t)) != 0)
  {
    if (usmDbCpdmClientConnStatusStatisticsGet(&selected_clientMac, &bTx, &bRx, &pTx, &pRx) == L7_SUCCESS)
    {
      /* bytes transmitted */
      osapiSnprintf(buf, sizeof(buf), "%-20llu", bTx);
      form->value.bytes_tx = osapiStrDup((char *)buf);

      /* bytes received */
      osapiSnprintf(buf, sizeof(buf), "%-20llu", bRx);
      form->value.bytes_rx = osapiStrDup((char *)buf);

      /* packets transmitted */
      osapiSnprintf(buf, sizeof(buf), "%-20llu", pTx);
      form->value.pkts_tx = osapiStrDup((char *)buf);

      /* packets received */
      osapiSnprintf(buf, sizeof(buf), "%-20llu", pRx);
      form->value.pkts_rx = osapiStrDup((char *)buf);
    }
    else
    {
      /* bytes transmitted */
      form->value.bytes_tx = osapiStrDup("");

      /* bytes received */
      form->value.bytes_rx = osapiStrDup("");

      /* packets transmitted */
      form->value.pkts_tx = osapiStrDup("");

      /* packets received */
      form->value.pkts_rx = osapiStrDup("");
    }

    form->status.bytes_tx |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    form->status.bytes_rx |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    form->status.pkts_tx |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    form->status.pkts_rx |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  form->value.Refresh = osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->status.mac |= (EW_FORM_INITIALIZED);
  form->value.refresh_in_progress = 0;
  form->status.refresh_in_progress |= (EW_FORM_INITIALIZED);
  form->value.refresh_key = 0;
  form->status.refresh_key |= (EW_FORM_INITIALIZED);
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext              context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpSessionStatsP form    @b{(input)} pointer to the form
*
* @returns  cp_session_statistics.html
*
* @end
*********************************************************************/
L7_char8 *ewaFormSubmit_cpSessionStats(EwsContext context, EwaForm_cpSessionStatsP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0x00, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  if (form->value.refresh_key == 1)
  {
    memcpy(appInfo.buf, form->value.cp_clientMac, L7_ENET_MAC_ADDR_LEN);
  }
  else
  {
    memcpy(appInfo.buf, form->value.mac, L7_ENET_MAC_ADDR_LEN);
  }

  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPSessionStats);
}

