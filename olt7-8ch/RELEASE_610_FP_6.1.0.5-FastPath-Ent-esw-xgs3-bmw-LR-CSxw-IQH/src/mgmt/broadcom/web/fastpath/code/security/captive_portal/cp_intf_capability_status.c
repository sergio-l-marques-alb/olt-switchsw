/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_intf_capability_status.c
*
* @purpose    Code in support of the cp_intf_capability_status.html page
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
#include "usmdb_cpdm_api.h"
#include "usmdb_cpim_api.h"
#include "captive_portal_commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"


/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext               context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpIntfCapStatusP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpIntfCapStatus(EwsContext context, EwaForm_cpIntfCapStatusP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 selected_ifId;
  L7_uchar8 val;
  L7_char8 data[USMWEB_APP_SMALL_DATA_SIZE];
  L7_char8 dataIntf[USMWEB_APP_SMALL_DATA_SIZE];
  L7_uint32 i, numBytes;

  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->value.err_flag = L7_FALSE;
  form->status.err_flag |= (EW_FORM_INITIALIZED);
  selected_ifId = 0;
  memset(data, 0, sizeof(data));
  memset(dataIntf, 0, sizeof(dataIntf));

  numBytes = ewsCGIQueryString(context, data, sizeof(data));
  if (numBytes > 0)
  {
    for (i=0; i<strlen(data); i++)
    {
      dataIntf[i] = data[i+3];
    }
    selected_ifId = atoi(dataIntf);
  }
  else
  {
    net = ewsContextNetHandle(context);
    if (net->app_pointer != NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      selected_ifId = appInfo.data[0];
      form->value.err_flag = appInfo.err.err_flag;
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_DYNAMIC);
    }
    else
    {
      if (usmDbCpimIntfNextGet(selected_ifId, &selected_ifId) != L7_SUCCESS)
      {
        selected_ifId = 0;
      }
    }
  }

  if (selected_ifId != 0)
  {
    /* intf session timeout */
    if (usmDbCpimIntfCapabilitySessionTimeoutGet(selected_ifId, &val) == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        form->value.intf_session_timeout = (char *)osapiStrDup(pStrInfo_common_Supported);
      }
      else
      {
        form->value.intf_session_timeout = (char *)osapiStrDup(pStrErr_common_NotSupported);
      }
      form->status.intf_session_timeout |= (EW_FORM_INITIALIZED);
    }

    /* intf idle timeout */
    if (usmDbCpimIntfCapabilityIdleTimeoutGet(selected_ifId, &val) == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        form->value.intf_idle_timeout = (char *)osapiStrDup(pStrInfo_common_Supported);
      }
      else
      {
        form->value.intf_idle_timeout = (char *)osapiStrDup(pStrErr_common_NotSupported);
      }
      form->status.intf_idle_timeout |= (EW_FORM_INITIALIZED);
    }

    /* bytes received counter */
    if (usmDbCpimIntfCapabilityBytesReceivedCounterGet(selected_ifId, &val) == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        form->value.bytes_rx_counter = (char *)osapiStrDup(pStrInfo_common_Supported);
      }
      else
      {
        form->value.bytes_rx_counter = (char *)osapiStrDup(pStrErr_common_NotSupported);
      }
      form->status.bytes_rx_counter |= (EW_FORM_INITIALIZED);
    }

    /* bytes transmitted counter */
    if (usmDbCpimIntfCapabilityBytesTransmittedCounterGet(selected_ifId, &val) == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        form->value.bytes_tx_counter = (char *)osapiStrDup(pStrInfo_common_Supported);
      }
      else
      {
        form->value.bytes_tx_counter = (char *)osapiStrDup(pStrErr_common_NotSupported);
      }
      form->status.bytes_tx_counter |= (EW_FORM_INITIALIZED);
    }

    /* packets received counter */
    if (usmDbCpimIntfCapabilityPacketsReceivedCounterGet(selected_ifId, &val) == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        form->value.pkts_rx_counter = (char *)osapiStrDup(pStrInfo_common_Supported);
      }
      else
      {
        form->value.pkts_rx_counter = (char *)osapiStrDup(pStrErr_common_NotSupported);
      }
      form->status.pkts_rx_counter |= (EW_FORM_INITIALIZED);
    }

    /* packets transmitted counter */
    if (usmDbCpimIntfCapabilityPacketsTransmittedCounterGet(selected_ifId, &val) == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        form->value.pkts_tx_counter = (char *)osapiStrDup(pStrInfo_common_Supported);
      }
      else
      {
        form->value.pkts_tx_counter = (char *)osapiStrDup(pStrErr_common_NotSupported);
      }
      form->status.pkts_tx_counter |= (EW_FORM_INITIALIZED);
    }

    /* intf roaming support */
    if (usmDbCpimIntfCapabilityRoamingSupportGet(selected_ifId, &val) == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        form->value.intf_roaming_support = (char *)osapiStrDup(pStrInfo_common_Supported);
      }
      else
      {
        form->value.intf_roaming_support = (char *)osapiStrDup(pStrErr_common_NotSupported);
      }
      form->status.intf_roaming_support |= (EW_FORM_INITIALIZED);
    }
  }

  form->value.Refresh = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext               context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpIntfCapStatusP form    @b{(input)} pointer to the form
*
* @returns  cp_intf_capability_status.html
*
* @end
*********************************************************************/
L7_char8 *ewaFormSubmit_cpIntfCapStatus(EwsContext context, EwaForm_cpIntfCapStatusP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  appInfo.data[0] = form->value.if_id;
  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPIntfCapStatus);
}

