/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/emweb/web/switching/dhcp_snooping/dhcp_snooping_vlan_cfg.c
*
* @purpose Code in support of the dhcp_snooping_vlan_cfg.html page
*
* @component unitmgr
*
* @comments tba
*
* @create 17/10/2007
*
* @author msudhir
* @end
*
**********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_switching_common.h"
#include "strlib_switching_web.h"
#include <l7_common.h>
#include <stdio.h>
#include <string.h>
#include <ew_proto.h>
#include <ewnet.h>

#include <log.h>
#include "web.h"
#include "web_oem.h"
#include "usmdb_dhcp_snooping.h"
#include "usmdb_sim_api.h"
#include "util_enumstr.h"
#include "dot1q_exports.h"

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the ds_cfg form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_ds_vlan_cfg( EwsContext context, EwaForm_ds_vlan_cfgP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 val, unit, vlanid;
  L7_uint32 user_access;

  unit = usmDbThisUnitGet();
  user_access = usmWebUserAccessGet(context);
  form->value.err_flag = L7_FALSE;
  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->status.err_flag |= (EW_FORM_INITIALIZED);
  vlanid = 1;
  net = ewsContextNetHandle(context);
  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    vlanid = appInfo.data[0];
    form->value.err_flag = appInfo.err.err_flag;
    form->value.err_msg = osapiStrDup(appInfo.err.msg);
    form->status.err_msg |= (EW_FORM_DYNAMIC);
  }

  form->value.dhcp_snoop_vlan  = vlanid;
  form->status.dhcp_snoop_vlan  |= (EW_FORM_INITIALIZED);
  val = 0;
  if (usmDbDsVlanConfigGet(vlanid, &val) == L7_SUCCESS)
  {
    if (user_access == USMWEB_READWRITE)
    {
    form->value.dhcp_snoop_mode_rw = usmWebL7AdminModeToEnDisGet(val, Disable);
    form->status.dhcp_snoop_mode_rw |= (EW_FORM_INITIALIZED);
    }
    else
    {
      form->value.dhcp_snoop_mode_ro = osapiStrDup(strUtilEnabledDisabledGet(val, pStrInfo_common_Dsbld));
      form->status.dhcp_snoop_mode_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
  }

  form->value.Submit = (char *) osapiStrDup(pStrInfo_common_Submit);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the ds_vlan_cfg form

*
* @returns dhcp_snooping_vlan_cfg.html
*
* @end
*
*********************************************************************/
char *ewaFormSubmit_ds_vlan_cfg(EwsContext context, EwaForm_ds_vlan_cfgP form)
{
  L7_uint32 unit, vlanid;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_RC_t rc = L7_FAILURE;

  unit = usmDbThisUnitGet();
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));

  vlanid = form->value.dhcp_snoop_vlan;
  appInfo.data[0] = vlanid;

  if ((form->status.Submit & EW_FORM_RETURNED))
  {
    switch (form->value.dhcp_snoop_mode_rw)
    {
      case Disable:
        rc = usmDbDsVlanConfigSet(vlanid, vlanid, L7_FALSE);
        break;
      case Enable:
        rc = usmDbDsVlanConfigSet(vlanid, vlanid, L7_TRUE);
        break;
      default:
        break;

    }
    if (rc != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FailedToSet, pStrErr_common_FailedToSet_1, pStrInfo_switching_DhcpSnoopingMode);
    }
  }

    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_switching_HtmlFileDhcpSnoopingVlanCfg);
 }

/*********************************************************************
*
* @purpose Generate the list of configured vlan id options
*          for the DAI VLAN Configuration screen select box
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
void *ewaFormSelect_dhcp_snoop_vlan( EwsContext context,
                                 EwsFormSelectOptionP optionp,
                                 void * iterator)
{
  static L7_uint32 sel_vlanId, vlanId;
  L7_char8 buf[APP_BUFFER_SIZE];
  L7_char8 * bufChoice;
  L7_uint32 unit, user_access;
  #define MAX L7_DOT1Q_MAX_VLAN_ID
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  unit = usmDbThisUnitGet();
  user_access = usmWebUserAccessGet(context);
  memset (buf, 0, APP_BUFFER_SIZE);
  if (iterator == L7_NULL)
  {
    vlanId = 1;
    sel_vlanId = vlanId;

    net = ewsContextNetHandle(context);
    if (net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      sel_vlanId = appInfo.data[0];
    }
  }
  else
  {
    vlanId++;
    if (vlanId > MAX)
    {
      return L7_NULL;
    }
  }

  optionp->valuep = (void *) &vlanId;

  bufChoice = ewsContextNetHandle(context)->buffer;
  osapiSnprintf(buf, sizeof(buf),  "%d", vlanId);
  osapiStrncpySafe(bufChoice, buf, APP_BUFFER_SIZE-1);
  optionp->choice = bufChoice;
  if (vlanId == sel_vlanId)
  {
    optionp->selected = TRUE;
  }
  else
  {
    optionp->selected = FALSE;
  }

  return (void *) optionp;
}

