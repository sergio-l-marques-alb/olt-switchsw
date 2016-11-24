/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
*
* @filename src/mgmt/emweb/web/switching/dhcp_snooping/dhcp_l2relay_vlan_cfg.c
*
* @purpose Code in support of the dhcp_l2relay_vlan_cfg.html page
*
* @component unitmgr
*
* @comments 
*
* @create 04/30/2008
*
* @author ddevi
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
#include <web.h>
#include "web_oem.h"
#include "usmdb_dhcp_snooping.h"
#include "usmdb_sim_api.h"
#include "dhcp_snooping_api.h"
#include "util_enumstr.h"

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the ds_l2relay_vlan_cfg form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_ds_l2relay_vlan_cfg( EwsContext context, EwaForm_ds_l2relay_vlan_cfgP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 val, unit, vlanid;
  L7_uint32 user_access;
  L7_char8 remoteId[DS_MAX_REMOTE_ID_STRING], eosStr = L7_EOS;
  L7_RC_t rc = L7_FAILURE;

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

  form->value.dhcp_l2relay_vlan  = vlanid;
  form->status.dhcp_l2relay_vlan  |= (EW_FORM_INITIALIZED);
  val = 0;
  if (usmDbDsL2RelayVlanModeGet(vlanid, &val) == L7_SUCCESS)
  {
    if (user_access == USMWEB_READWRITE)
    {
    form->value.dhcp_l2relay_mode_rw = usmWebL7EnDisToEnDisGet(val, Disable);
    form->status.dhcp_l2relay_mode_rw |= (EW_FORM_INITIALIZED);
    }
    else
    {
      form->value.dhcp_l2relay_mode_ro = osapiStrDup(strUtilEnableDisableGet(val, L7_NULLPTR));
      form->status.dhcp_l2relay_mode_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
  }

  val = 0;
  if (usmDbDsL2RelayCircuitIdGet(vlanid, &val) == L7_SUCCESS)
  {
    if (user_access == USMWEB_READWRITE)
    {
    form->value.dhcp_cid_mode_rw = usmWebL7EnDisToEnDisGet(val, Disable);
    form->status.dhcp_cid_mode_rw |= (EW_FORM_INITIALIZED);
    }
    else
    {
      form->value.dhcp_cid_mode_ro = osapiStrDup(strUtilEnableDisableGet(val, L7_NULLPTR));
      form->status.dhcp_cid_mode_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
  }
  memset(remoteId, 0, sizeof(remoteId));
  rc = usmDbDsL2RelayRemoteIdGet( vlanid, remoteId);
  if( rc == L7_SUCCESS)
  {
    if (user_access == USMWEB_READWRITE)
    {
      form->value.dhcp_rid_mode_rw = (L7_uchar8 *) osapiStrDup( remoteId);
      form->status.dhcp_rid_mode_rw |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
    else
    {
      form->value.dhcp_rid_mode_ro = osapiStrDup(&eosStr);
      form->status.dhcp_rid_mode_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
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
* @param form pointer to the ds_l2relay_vlan_cfg form

*
* @returns dhcp_l2relay_vlan_cfg.html
*
* @end
*
*********************************************************************/
char *ewaFormSubmit_ds_l2relay_vlan_cfg(EwsContext context, EwaForm_ds_l2relay_vlan_cfgP form)
{
  L7_uint32 unit, vlanid;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_RC_t rc = L7_FAILURE;
  L7_char8 remoteId[DS_MAX_REMOTE_ID_STRING], eosStr = L7_EOS;

  unit = usmDbThisUnitGet();
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));

  vlanid = form->value.dhcp_l2relay_vlan;
  appInfo.data[0] = vlanid;

  if ((form->status.Submit & EW_FORM_RETURNED))
  {
    /* L2 Relay mode */
    switch (form->value.dhcp_l2relay_mode_rw)
    {
      case Disable:
        rc = usmDbDsL2RelayVlanModeSet(vlanid, vlanid, L7_DISABLE);
        break;
      case Enable:
        rc = usmDbDsL2RelayVlanModeSet(vlanid, vlanid, L7_ENABLE);
        break;
      default:
        break;

    }
    if (rc != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FailedToSet, 
                              pStrErr_common_FailedToSet_1, pStrInfo_switching_DhcpL2RelayMode);
    }
    /* Circuit-id */
    switch (form->value.dhcp_cid_mode_rw)
    {
      case Disable:
        rc = usmDbDsL2RelayCircuitIdSet(vlanid, vlanid, L7_DISABLE);
        break;
      case Enable:
        rc = usmDbDsL2RelayCircuitIdSet(vlanid, vlanid, L7_ENABLE);
        break;
      default:
        break;

    }
    if (rc != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FailedToSet, 
                              pStrErr_common_FailedToSet_1, pStrInfo_switching_DhcpL2RelayCircuitIdMode);
    }
    /* Remote-id */
    rc = L7_SUCCESS;
    if (form->value.dhcp_rid_mode_rw != L7_NULL)
    {
      osapiStrncpy(remoteId, form->value.dhcp_rid_mode_rw, sizeof(remoteId));
      if (osapiStrncmp(remoteId, &eosStr, sizeof(L7_uchar8)) != L7_NULL)
      {
        rc = usmDbDsL2RelayRemoteIdSet(vlanid, vlanid, remoteId);
      }
    }
    if (rc != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FailedToSet, 
                              pStrErr_common_FailedToSet_1, pStrInfo_switching_DhcpL2RelayRemoteIdMode);
    }
  }

    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_switching_HtmlFileDhcpL2RelayVlanCfg);
 }

/*********************************************************************
*
* @purpose Generate the list of configured vlan id options
*          for the VLAN Configuration screen select box
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
void *ewaFormSelect_dhcp_l2relay_vlan( EwsContext context,
                                 EwsFormSelectOptionP optionp,
                                 void * iterator)
{
  static L7_uint32 sel_vlanId, vlanId;
  L7_char8 buf[APP_BUFFER_SIZE];
  L7_char8 * bufChoice;
  L7_uint32 unit, user_access;
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
    if (vlanId > L7_PLATFORM_MAX_VLAN_ID)
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

