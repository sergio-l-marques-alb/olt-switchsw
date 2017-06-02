/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/emweb/web/switching/dai/dai_vlan_cfg.c
*
* @purpose Code in support of the dai_vlan_cfg.html page
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
#include "util_enumstr.h"



/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the dai_vlan_cfg form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_dai_vlan_cfg( EwsContext context, EwaForm_dai_vlan_cfgP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 unit, vlanID, val;
  L7_uint32 user_access;
  L7_uchar8 arpAclName[32];
  
  vlanID = 1;  
  memset(arpAclName, 0, sizeof(arpAclName));
  unit = usmDbThisUnitGet();
  user_access = usmWebUserAccessGet(context);
  net = ewsContextNetHandle(context);

  form->value.err_flag = L7_FALSE;
  form->status.err_flag |= (EW_FORM_INITIALIZED);
  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);

  if (net->app_pointer != NULL)
  {

     memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    if (appInfo.err.err_flag == L7_TRUE)
    {
      form->value.err_flag = 1;
      form->status.err_flag |= (EW_FORM_INITIALIZED);
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_DYNAMIC);
    }
    vlanID = appInfo.data[0];
   
  }
  
  form->status.dai_vlan_list |= (EW_FORM_INITIALIZED);
  form->value.dai_vlan_list= vlanID;

/* Dyanamic ARP Inspection Mode */

  if(usmDbDaiVlanEnableGet(vlanID, &val) == L7_SUCCESS)
  {
   if (user_access == USMWEB_READWRITE)
   { 
   form->value.dai_rw = usmWebL7EnDisToEnDisGet(val,Disable); 
   form->status.dai_rw |= (EW_FORM_INITIALIZED);   
   }
   else
   {
     form->value.dai_ro = osapiStrDup(strUtilEnabledDisabledGet(val, pStrInfo_common_Dsbld));
     form->status.dai_ro |= (EW_FORM_INITIALIZED);
   }
  }
/* Logging Invalid Packets */
  if (usmDbDaiVlanLoggingEnableGet(vlanID, &val) == L7_SUCCESS)
  {
    if (user_access == USMWEB_READWRITE)
    {
      form->value.logg_invalid_pack_rw = usmWebL7EnDisToEnDisGet(val,Disable);
      form->status.logg_invalid_pack_rw |= (EW_FORM_INITIALIZED);
    }
    else
    {
      form->value.logg_invalid_pack_ro = osapiStrDup(strUtilEnabledDisabledGet(val, pStrInfo_common_Dsbld));
      form->status.logg_invalid_pack_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
  }
/* ARP ACL Name */
  if (usmDbDaiVlanArpAclGet(vlanID, arpAclName) == L7_SUCCESS)
  {
     if (user_access == USMWEB_READWRITE)
     {
       form->value.arp_acl_name_rw = osapiStrDup(arpAclName);
       form->status.arp_acl_name_rw |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
     }
     else
     {
       form->value.arp_acl_name_ro = osapiStrDup(arpAclName);
       form->status.arp_acl_name_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
     }
  }
/* ARP ACL Stactic Flag */
  if (usmDbDaiVlanArpAclStaticFlagGet(vlanID, &val) == L7_SUCCESS) 
  {
    if (user_access == USMWEB_READWRITE)
     {
       form->value.staticFlag_rw = usmWebL7EnDisToEnDisGet(val,Disable);
       form->status.staticFlag_rw |= (EW_FORM_INITIALIZED);
     }
    else
     {
       form->value.staticFlag_ro = osapiStrDup(strUtilEnabledDisabledGet(val, pStrInfo_common_Dsbld));
       form->status.staticFlag_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
     }
  }


  if (user_access == USMWEB_READWRITE)
  {
    form->value.Submit = (char *) osapiStrDup(pStrInfo_common_Submit);
    form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }
  form->value.Refresh = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
}


/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the dai_vlan_cfg form
*
* @returns dai_vlan_cfg.html
*
* @end
*
*********************************************************************/
char *ewaFormSubmit_dai_vlan_cfg(EwsContext context, EwaForm_dai_vlan_cfgP form)
{
  L7_uint32 vlanId;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 daimode = L7_DISABLE, logging = L7_DISABLE, staticFlag = L7_DISABLE; 
  L7_uchar8 arpaclname[32], prevAcl[32];
  vlanId = form->value.dai_vlan_list;
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  memset (arpaclname, 0, sizeof(arpaclname));
  appInfo.data[0] = vlanId; 

  if ((form->status.Submit & EW_FORM_RETURNED))
  {
   switch (form->value.dai_rw)
   {
    case Disable:
      daimode = L7_DISABLE;
      break;
    case Enable:
      daimode = L7_ENABLE;
      break;
    default:
      break;
  }
  switch (form->value.logg_invalid_pack_rw)
  {
    case Disable:
      logging = L7_DISABLE;
      break;
    case Enable:
      logging = L7_ENABLE;
      break;
    default:
      break;
  }
  switch(form->value.staticFlag_rw)
  {
    case Disable:
      staticFlag = L7_DISABLE;
       break;
      case Enable:
        staticFlag = L7_ENABLE;
        break;
      default:
        break;
  }
    if(usmDbDaiVlanEnableSet( vlanId, daimode) != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_FailedToSet, pStrErr_common_FailedToSet_1,
                                 pStrInfo_switching_DynamicArpInspection);
    } 
    if (usmDbDaiVlanLoggingEnableSet(vlanId, logging)!= L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_FailedToSet, pStrErr_common_FailedToSet_1, 
                             pStrInfo_switching_LoggingInvalidPackets);
    }
    if (form->status.arp_acl_name_rw & EW_FORM_RETURNED)
    {
      usmDbDaiVlanArpAclGet(vlanId, prevAcl);
      if (osapiStrncmp(prevAcl, form->value.arp_acl_name_rw, 32) != 0)
      {
        if (form->value.arp_acl_name_rw[0] == 0)
        {
          if (usmDbDaiVlanARPAclRangeSet(prevAcl, vlanId, vlanId, L7_DISABLE, L7_DISABLE) != L7_SUCCESS)
          {
            usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error, pStrErr_common_FieldInvalidFmt,
                                    pStrInfo_switching_ArpACLName);
          }
        }
        else
        {
          if (usmDbDaiVlanArpAclSet(vlanId, form->value.arp_acl_name_rw) != L7_SUCCESS)
          {
            usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error, pStrErr_common_FieldInvalidFmt,
                                    pStrInfo_switching_ArpACLName);
          }
        }
      }
    }
    if(usmDbDaiVlanArpAclStaticFlagSet( vlanId, staticFlag) != L7_SUCCESS)
    {

      usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_FailedToSet, pStrErr_common_FailedToSet_1,
                             pStrInfo_switching_StaticFalg);
    }
 } 
  return usmWebEwsContextSendReply (context, L7_TRUE, &appInfo,  pStrInfo_switching_HtmlFileDaiVlanCfg);
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
void *ewaFormSelect_dai_vlan_list( EwsContext context,
                                 EwsFormSelectOptionP optionp,
                                 void * iterator)
{
 static L7_uint32 sel_vlanId, vlanId;
  L7_char8 buf[APP_BUFFER_SIZE];
  L7_char8 * bufChoice;
  L7_uint32 unit, user_access, val;
  L7_RC_t rc = L7_FAILURE;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  unit = usmDbThisUnitGet();
  user_access = usmWebUserAccessGet(context);
  memset (buf, 0, APP_BUFFER_SIZE);
  if (iterator == L7_NULL)
  {
    vlanId = 1;
      rc = usmDbDaiVlanEnableGet (vlanId, &val) ;
      if (rc == L7_SUCCESS)
      {
        sel_vlanId = vlanId;
      }
    if (rc != L7_SUCCESS)
    {
      return L7_NULL;
    }
  
    net = ewsContextNetHandle(context);
    if (net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      sel_vlanId = appInfo.data[0];
    }
  }
  else
  {
    if (vlanId == 0)
       return L7_NULL;
    rc = usmDbDaiVlanNextGet (vlanId, &vlanId) ;

     if (rc != L7_SUCCESS)
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



