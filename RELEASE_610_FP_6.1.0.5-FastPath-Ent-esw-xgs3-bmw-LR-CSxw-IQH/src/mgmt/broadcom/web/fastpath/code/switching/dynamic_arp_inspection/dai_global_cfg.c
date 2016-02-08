/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/emweb/web/switching/dai/dai_global_cfg.c
*
* @purpose Code in support of the dai_global_cfg.html page
*
* @component unitmgr
*
* @comments tba
*
* @create 04/10/2007
*
* @author kkiran
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
#include "usmdb_dai_api.h"
#include "usmdb_sim_api.h"

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the dai_global_cfg form
*
* @returns none
*
* @end
*
*********************************************************************/
/*void ewaFormServe_dai_global_cfg( EwsContext context, EwaForm_dai_global_cfgP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 val, unit;
  L7_uint32 user_access;

  unit = usmDbThisUnitGet();
  user_access = usmWebUserAccessGet(context);
  form->value.err_flag = L7_FALSE;
  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->status.err_flag |= (EW_FORM_INITIALIZED);

  net = ewsContextNetHandle(context);
  if (net->app_pointer != NULL)
  {

    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    form->value.err_msg = osapiStrDup(appInfo.err.msg);
    form->status.err_msg |= (EW_FORM_DYNAMIC);
  }

  if (usmDbDaiVerifySMacGet(&val) == L7_SUCCESS)
  {
    if (user_access == USMWEB_READWRITE)
    {
    form->value.valid_source_mac_rw = usmWebL7TrueFalseToEnDisGet(val, Disable);
    form->status.valid_source_mac_rw |= (EW_FORM_INITIALIZED);
    }
    else
    {
      form->value.valid_source_mac_ro = osapiStrDup(strUtilEnabledDisabledGet(val, pStrInfo_common_Dsbld));
      form->status.valid_source_mac_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
  }
  val =0;
  if (usmDbDaiVerifyDMacGet(&val) == L7_SUCCESS)
  {
    if (user_access == USMWEB_READWRITE)
    {
      form->value.valid_dest_mac_rw =  usmWebL7TrueFalseToEnDisGet(val, Disable);
      form->status.valid_dest_mac_rw |= (EW_FORM_INITIALIZED);
    }
    else
    {
      form->value.valid_dest_mac_ro =  osapiStrDup(strUtilEnabledDisabledGet(val, pStrInfo_common_Dsbld));
      form->status.valid_dest_mac_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
  }
  val =0;
   if (usmDbDaiVerifyIPGet (&val) == L7_SUCCESS)
  {
    if (user_access == USMWEB_READWRITE)
    {
      form->value.valid_ip_addr_rw =  usmWebL7TrueFalseToEnDisGet(val, Disable);
      form->status.valid_ip_addr_rw |= (EW_FORM_INITIALIZED);
    }
    else
    {
      form->value.valid_ip_addr_ro =  osapiStrDup(strUtilEnabledDisabledGet(val, pStrInfo_common_Dsbld));
      form->status.valid_ip_addr_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
  }

  
  form->value.Submit = (char *) osapiStrDup(pStrInfo_common_Submit);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}
*/

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the dai_global_cfg form

*
* @returns dai_global_cfg.html
*
* @end
*
*********************************************************************/
/*char *ewaFormSubmit_dai_global_cfg(EwsContext context, EwaForm_dai_global_cfgP form)
{
  L7_uint32 unit;
  usmWeb_AppInfo_t appInfo;
  L7_RC_t rc = L7_FAILURE;
  
  unit = usmDbThisUnitGet();

  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));

  if ((form->status.Submit & EW_FORM_RETURNED))
  {
    switch (form->value.valid_source_mac_rw)
    {
      case Disable:
        rc = usmDbDaiVerifySMacSet(L7_FALSE);
        break;
      case Enable:
        rc = usmDbDaiVerifySMacSet( L7_TRUE);
        break;
      default:
        break;
  
    }
    if (rc != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FailedToSet, pStrInfo_switching_SrcMacValidate);
    }
   rc = L7_FAILURE;
    switch (form->value.valid_dest_mac_rw)
    {
      case Disable:
        rc = usmDbDaiVerifyDMacSet(L7_FALSE);
        break;
      case Enable:
        rc = usmDbDaiVerifyDMacSet(L7_TRUE);
        break;
      default:
        break;

    }
    if (rc != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FailedToSet, pStrInfo_switching_DestMacValidate);
    }
   rc = L7_FAILURE;
    switch (form->value.valid_ip_addr_rw)
    {
      case Disable:
        rc = usmDbDaiVerifyIPSet(L7_FALSE);
        break;
      case Enable:
        rc = usmDbDaiVerifyIPSet(L7_TRUE);
        break;
      default:
        break;

    }
    if (rc != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FailedToSet, pStrInfo_switching_IpAddrValidate);
    }

  }
  return usmWebEwsContextSendReply (context, L7_TRUE, &appInfo,  pStrInfo_switching_HtmlFileDaiGlobCfg);
}
*/
/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the dai_stats form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_dai_stats( EwsContext context, EwaForm_dai_statsP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 unit, vlanid, val =0;
  L7_uint32 user_access;
  daiVlanStats_t stats;
  L7_RC_t rc = L7_FAILURE;

  unit = usmDbThisUnitGet();
  user_access = usmWebUserAccessGet(context);

  form->value.err_flag = L7_FALSE;
  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->status.err_flag |= (EW_FORM_INITIALIZED);

  net = ewsContextNetHandle(context);
  vlanid = 1;
  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    vlanid = appInfo.data[0];
    form->value.err_flag = appInfo.err.err_flag;
    form->value.err_msg = osapiStrDup(appInfo.err.msg);
    form->status.err_msg |= (EW_FORM_DYNAMIC);
    rc = L7_SUCCESS;
    val = L7_ENABLE;
  }
  else
  {
    if ( (rc =usmDbDaiVlanEnableGet(vlanid, &val)) != L7_SUCCESS || val!= L7_ENABLE)
    {
    while ((rc=usmDbDaiVlanNextGet(vlanid, &vlanid) )== L7_SUCCESS )
    {
      rc = usmDbDaiVlanEnableGet (vlanid, &val) ;
      if (rc == L7_SUCCESS && val== L7_ENABLE)
      {
        break;
      }
    }
    }
  }

  form->status.dai_vlan_id |= (EW_FORM_INITIALIZED);
   
  if (rc == L7_SUCCESS && val== L7_ENABLE ) 
  {
    form->value.dai_vlan_id = vlanid;
    (void) usmDbDaiVlanStatsGet(vlanid, &stats);
    if (rc == L7_SUCCESS)  
    {
      form->value.dhcp_drops = stats.dhcpDrops;
      form->value.acl_drops = stats.aclDrops;
      form->value.dhcp_permits= stats.dhcpPermits;
      form->value.acl_permits= stats.aclPermits;
      form->value.bad_src_mac = stats.sMacFailures;
      form->value.bad_dest_mac = stats.dMacFailures;
      form->value.invalid_ip = stats.ipValidFailures;
      form->value.forwarded = stats.forwarded;
      form->value.dropped = stats.dropped ;
    }
    else
    {
     form->value.dhcp_drops = 0;
     form->value.acl_drops = 0;
     form->value.dhcp_permits = 0;
     form->value.acl_permits= 0;
     form->value.bad_src_mac = 0;
     form->value.bad_dest_mac = 0;
     form->value.invalid_ip = 0;
     form->value.forwarded = 0;
     form->value.dropped = 0;

   }
  }

  form->status.dhcp_drops |= (EW_FORM_INITIALIZED);
  form->status.acl_drops |= (EW_FORM_INITIALIZED);
  form->status.dhcp_permits |= (EW_FORM_INITIALIZED);
  form->status.acl_permits  |= (EW_FORM_INITIALIZED);
  form->status.bad_src_mac |= (EW_FORM_INITIALIZED);
  form->status.bad_dest_mac  |= (EW_FORM_INITIALIZED);
  form->status.invalid_ip |= (EW_FORM_INITIALIZED);
  form->status.forwarded |= (EW_FORM_INITIALIZED);
  form->status.dropped  |= (EW_FORM_INITIALIZED); 

  form->value.Refresh = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the dai_stats form

*
* @returns dai_stats.html
*
* @end
*
*********************************************************************/
char *ewaFormSubmit_dai_stats(EwsContext context, EwaForm_dai_statsP form)
{
  L7_uint32  vlanid;
  usmWeb_AppInfo_t appInfo;

  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  vlanid = form->value.dai_vlan_id;
  appInfo.data[0] = vlanid; 

  return usmWebEwsContextSendReply (context, L7_TRUE, &appInfo,  pStrInfo_switching_HtmlFileDaiStats);
  
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
void *ewaFormSelect_dai_vlan_id( EwsContext context,
                                 EwsFormSelectOptionP optionp,
                                 void * iterator)
{
 static L7_uint32 sel_vlanId, vlanId;
  L7_char8 buf[APP_BUFFER_SIZE];
  L7_char8 * bufChoice;
  L7_uint32 unit, val = L7_DISABLE;
  L7_RC_t rc = L7_FAILURE;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  unit = usmDbThisUnitGet();

  memset (buf, 0, APP_BUFFER_SIZE);
  if (iterator == L7_NULL)
  {
    vlanId = 1;
    net = ewsContextNetHandle(context);
    if ( (rc =usmDbDaiVlanEnableGet(vlanId, &val)) != L7_SUCCESS || val!= L7_ENABLE)
    {
    while ((rc=usmDbDaiVlanNextGet(vlanId, &vlanId) )== L7_SUCCESS )
    {
      rc = usmDbDaiVlanEnableGet (vlanId, &val) ;
      if (rc == L7_SUCCESS && val== L7_ENABLE)
      {
        sel_vlanId = vlanId;
        break;
      }
    }
    }
    if (rc !=L7_SUCCESS || val!= L7_ENABLE)
    {
     return L7_NULL;
    }
    if (net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      sel_vlanId = appInfo.data[0];
    }

  }
  else
  {
    while ((rc =usmDbDaiVlanNextGet(vlanId,  &vlanId)) == L7_SUCCESS)
    {
      rc = usmDbDaiVlanEnableGet (vlanId, &val) ;
      if (rc == L7_SUCCESS && val== L7_ENABLE)
      {
        break;
      }
    }
    if(rc != L7_SUCCESS || val != L7_ENABLE)
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

