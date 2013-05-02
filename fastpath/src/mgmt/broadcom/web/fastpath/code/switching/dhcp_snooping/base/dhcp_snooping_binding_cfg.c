/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/emweb/web/switching/dhcp_snooping/dhcp_snooping_binding_cfg.c
*
* @purpose Code in support of the dhcp_snooping_binding_cfg.html page
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
#include "usmdb_util_api.h"
#include "util_pstring.h"
#define SNOOP_MAX 15


/*********************************************************************
 *
 * @purpose Generate the list of all DHCP Snooping  Capable Interfaces.
 *
 * @param
 *
 * @returns
 *
 * @end
 *
 *********************************************************************/

void *ewaFormSelect_dhcp_snoop_port_intf(EwsContext context,
                              EwsFormSelectOptionP optionp,
                              void * iterator)
{

static L7_uint32 sel_intifnum;
  static L7_uint32 intIfNum;
  L7_uint32 unit;
  L7_char8 buf[APP_BUFFER_SIZE];  /* slot.port */
  L7_char8 * bufChoice;
  L7_uint32 u,s,p;
  L7_RC_t rc;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  unit = usmDbThisUnitGet();
  memset (&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  if (iterator == L7_NULL)
  {
    sel_intifnum = 0;
    intIfNum = 0;
    net = ewsContextNetHandle(context);
    while (1)
    {
      if ( usmDbValidIntIfNumNext(intIfNum, &intIfNum)!= L7_SUCCESS)
      {
        return L7_NULL;
      }
      if (usmDbDsIntfIsValid(intIfNum)== L7_TRUE)
      {
        break;
      }
    }
    if (net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      sel_intifnum = appInfo.data[0];
    }
    else
    {
      sel_intifnum = intIfNum;
    }
  }
  else
  {
    while (1)
    {
      if ( usmDbValidIntIfNumNext(intIfNum, &intIfNum)!= L7_SUCCESS)
      {
        return L7_NULL;
      }
      if (usmDbDsIntfIsValid(intIfNum)== L7_TRUE)
      {
        break;
      }
    }
  }

  rc = usmDbUnitSlotPortGet(intIfNum, &u, &s, &p);
  if (usmDbComponentPresentCheck(0, L7_FLEX_STACKING_COMPONENT_ID))
  {
    osapiSnprintf(buf, sizeof(buf),  "%d/%d/%d", u, s, p);
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf),  "%d/%d", s, p);
  }

  bufChoice = ewsContextNetHandle(context)->buffer;
  osapiStrncpySafe(bufChoice, buf, APP_BUFFER_SIZE-1);
  optionp->choice = bufChoice;
  optionp->valuep = (void *) &intIfNum;
  if (intIfNum == sel_intifnum)
  {
    optionp->selected = TRUE;
  }
  else
  {
    optionp->selected = FALSE;
  }

  return (void *) optionp;
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the ds_binding_cfg form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_ds_binding_cfg( EwsContext context, EwaForm_ds_binding_cfgP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 intIfNum=0, unit, vlanid, spage =1, dpage=1;
  L7_uint32 u, s, p;
  L7_uint32 user_access, elementN, cnt;
  L7_uchar8 slotPort[10];
  dhcpSnoopBinding_t dsBinding;

  unit = usmDbThisUnitGet();
  user_access = usmWebUserAccessGet(context);
  form->value.err_flag = L7_FALSE;
  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->status.err_flag |= (EW_FORM_INITIALIZED);

  net = ewsContextNetHandle(context);
  memset (&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    intIfNum = appInfo.data[0];
    vlanid = appInfo.data[1];
    spage = appInfo.data[2];
    dpage = appInfo.data[3];
    form->value.err_flag = appInfo.err.err_flag;
    form->value.err_msg = osapiStrDup(appInfo.err.msg);
    form->status.err_msg |= (EW_FORM_DYNAMIC);
  }
  else
  {
    while (1)
    {
      if ( usmDbValidIntIfNumNext(intIfNum, &intIfNum)!= L7_SUCCESS)
      {
        break;
      }
      if (usmDbDsIntfIsValid(intIfNum)== L7_TRUE)
      {
        break;
      }
    }
    vlanid = 1;
  }
  if (user_access == USMWEB_READWRITE) 
  {
  form->value.dhcp_snoop_port_intf = intIfNum;
  form->status.dhcp_snoop_port_intf |= (EW_FORM_INITIALIZED);

  form->value.dhcp_snoop_vlan = vlanid;
  form->status.dhcp_snoop_vlan |= (EW_FORM_INITIALIZED);

  form->value.senderIP = 0;
  form->status.senderIP |= (EW_FORM_INITIALIZED);
  
  memset(form->value.senderMac, 0, L7_ENET_MAC_ADDR_LEN);
  form->status.senderMac |= (EW_FORM_INITIALIZED); 
  }
  form->value.spage = spage;
  form->status.spage  |= (EW_FORM_INITIALIZED);

  form->value.dpage = dpage;
  form->status.dpage |= (EW_FORM_INITIALIZED);

  /* Getting The Static Binding Entries */ 
  if (spage != 0)
  {
  for (cnt = 0; cnt < form->value.dhcpSnoopStaticList_repeats; cnt++)
  {
    elementN = ((spage-1)*(SNOOP_MAX))+cnt+1;

    if (usmDbDsBindingNthEntryGet(&dsBinding, elementN, DS_BINDING_STATIC)!= L7_SUCCESS)
    {
       break;
    }
    /* Interface */
    intIfNum = dsBinding.intIfNum;

    usmDbUnitSlotPortGet(intIfNum, &u, &s, &p);
    osapiSnprintf (slotPort, sizeof(slotPort)-1, strUtilUspGet (unit, u, s, p, L7_NULLPTR, 0));
    
    form->value.dhcpSnoopStaticList[cnt].interface = osapiStrDup(slotPort);
    form->status.dhcpSnoopStaticList[cnt].interface |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

    /* Mac Address */
    memcpy (form->value.dhcpSnoopStaticList[cnt].macaddr, dsBinding.macAddr, L7_ENET_MAC_ADDR_LEN);
    form->status.dhcpSnoopStaticList[cnt].macaddr |= (EW_FORM_INITIALIZED |  EW_FORM_DYNAMIC);
    
    /* Vlan */ 
    form->value.dhcpSnoopStaticList[cnt].vlan  = dsBinding.vlanId;
    form->status.dhcpSnoopStaticList[cnt].vlan |= (EW_FORM_INITIALIZED );

    /* IP ADDRESS */
    form->value.dhcpSnoopStaticList[cnt].ipaddress  = dsBinding.ipAddr;
    form->status.dhcpSnoopStaticList[cnt].ipaddress |= (EW_FORM_INITIALIZED );

  }
 }
  /* Getting The Dynamic Binding Entries */ 
 if (dpage != 0)
 {
  for (cnt = 0; cnt < form->value.dhcpSnoopDynamicList_repeats; cnt++)
  {
    elementN = (dpage-1)*(SNOOP_MAX)+cnt+1;

    if (usmDbDsBindingNthEntryGet(&dsBinding, elementN, DS_BINDING_DYNAMIC)!= L7_SUCCESS)
    {
      break;
    }
    /* Interface */
    intIfNum = dsBinding.intIfNum;
    usmDbUnitSlotPortGet(intIfNum, &u, &s, &p);
    osapiSnprintf (slotPort, sizeof(slotPort)-1, strUtilUspGet (unit, u, s, p, L7_NULLPTR, 0));
    form->value.dhcpSnoopDynamicList[cnt].dinterface = osapiStrDup(slotPort);
    form->status.dhcpSnoopDynamicList[cnt].dinterface |= (EW_FORM_INITIALIZED |EW_FORM_DYNAMIC);
    /* Mac Address */
    memcpy (form->value.dhcpSnoopDynamicList[cnt].dmacaddr, dsBinding.macAddr, L7_ENET_MAC_ADDR_LEN);
    form->status.dhcpSnoopDynamicList[cnt].dmacaddr |= (EW_FORM_INITIALIZED |  EW_FORM_DYNAMIC);

    /* Vlan */ 
    form->value.dhcpSnoopDynamicList[cnt].dvlan  =  (L7_uint32)dsBinding.vlanId;
    form->status.dhcpSnoopDynamicList[cnt].dvlan |= (EW_FORM_INITIALIZED );

    /* IP ADDRESS */
    form->value.dhcpSnoopDynamicList[cnt].dipaddress  = dsBinding.ipAddr;
    form->status.dhcpSnoopDynamicList[cnt].dipaddress |= (EW_FORM_INITIALIZED );

    /* Lease Time */
    form->value.dhcpSnoopDynamicList[cnt].dleasetime = dsBinding.remLease;
    form->status.dhcpSnoopDynamicList[cnt].dleasetime  |= (EW_FORM_INITIALIZED );
  }
 }
  form->value.Add = (char *) osapiStrDup(pStrInfo_common_Add);
  form->status.Add |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC); 
  form->value.Clear = (char *) osapiStrDup(pStrInfo_common_ClrAll);
  form->status.Clear |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Submit = (char *) osapiStrDup(pStrInfo_common_Submit);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
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
* @param form pointer to the ds_binding_cfg form
*
* @notes  appInfo.data[0]-- interface
          appInfo.data[1]-- vlanid
          appInfo.data[2]--  spage
          appInfo.data[3] -- dpage  
*
* @returns dhcp_snooping_binding_cfg.html
* 
* @end
*
*********************************************************************/
char *ewaFormSubmit_ds_binding_cfg(EwsContext context, EwaForm_ds_binding_cfgP form)
{
  usmWeb_AppInfo_t appInfo;
  EwaNetHandle net;
  L7_uint32 intIfNum, vlanid, index;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 spage, dpage, delCount = 0;
  L7_enetMacAddr_t macAddress;

  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  memset (&macAddress, 0 , sizeof (L7_enetMacAddr_t)); 
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
 
  intIfNum = form->value.dhcp_snoop_port_intf; 
  vlanid = form->value.dhcp_snoop_vlan;
  spage = form->value.spage;
  dpage = form->value.dpage; 
  
  appInfo.data[0] = intIfNum;
  appInfo.data[1] =  vlanid;
  appInfo.data[2] = spage;
  appInfo.data[3] = dpage;

  /* Adding a New Static Entry */
  if ((form->status.Add & EW_FORM_RETURNED))
  {
     if (form->status.senderMac & EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error, pStrErr_common_FieldInvalidFmt,
                                pStrInfo_common_MacAddr_2);
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_switching_HtmlFileDhcpSnoopingBindingCfg);
    }
    if (form->status.senderIP & EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error, pStrErr_common_FieldInvalidFmt,
                                pStrInfo_common_IpAddr_1);
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_switching_HtmlFileDhcpSnoopingBindingCfg);
    }
    memcpy (macAddress.addr, form->value.senderMac, sizeof (macAddress.addr));

    if (usmDbDsStaticBindingAdd(&macAddress, form->value.senderIP, vlanid, 0 /* PTin modified: DHCP */, intIfNum) != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrErr_common_FailedToSet_1, pStrErr_switching_CouldNotAddBinding);
    }

  }
  /* Removing the selected Static Snooping Entries */
  if ((form->status.Submit & EW_FORM_RETURNED))
  {
     for (index = 0; index < form->value.dhcpSnoopStaticList_repeats;
             index++)
     {
        if (EW_FORM_BOOLEAN_TRUE(form, dhcpSnoopStaticList[index].Remove))
        {
           memcpy (macAddress.addr, form->value.dhcpSnoopStaticList[index].macaddr, sizeof (macAddress.addr));
           rc =  usmDbDsStaticBindingRemove(&macAddress);
           if (rc != L7_SUCCESS)
           {
             usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_FailedToSet, pStrErr_common_FailedToSet_1, pStrInfo_common_MacAddr_2);
           }
           else
           {
             delCount++;
           }
        }
     }
     if (delCount == form->value.dhcpSnoopStaticList_repeats)
     {
       if (spage != 1)
       appInfo.data[2] = (spage -1);
     }
  }
  /* To Clear All Snooping Entries */
  if (form->status.Clear & EW_FORM_RETURNED)
  {
    (void)usmDbDsBindingClear(0);     
     appInfo.data[2] = 1;
     appInfo.data[3] = 1;
  }

    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_switching_HtmlFileDhcpSnoopingBindingCfg);
 }

/****************************************************
* To Get the DS Database Static Entries
*
****************************************************/

L7_RC_t usmWebStaticEntriesCountGet(L7_uint32 *count, L7_uint32 spage)
{
  L7_uint32 staticCnt =0, rem = 0;

  staticCnt = usmDbDsStaticBindingsCount() ;

  rem = staticCnt -((spage-1) * SNOOP_MAX);

  if (rem > SNOOP_MAX)
  rem = SNOOP_MAX;

  *count = rem; 
  return L7_SUCCESS;
}

/****************************************************
* To Get the number of DS Database Dynamic Entries
*
****************************************************/

L7_RC_t usmWebDynamicEntriesCountGet(L7_uint32 *count, L7_uint32 dpage)
{
  L7_uint32 dynamic = 0, staticCnt = 0, total =0, rem =0;

  staticCnt = usmDbDsStaticBindingsCount();
  total = usmDbDsBindingsCount();
  dynamic = total - staticCnt;

  rem = dynamic  -((dpage-1) * SNOOP_MAX);
  if (rem > SNOOP_MAX)
  rem = SNOOP_MAX;

  *count = rem;
  return L7_SUCCESS;

}

/****************************************************
* Get the Nuber of DS Database Static Entries Pages 
*
****************************************************/
L7_uint32 usmWebStaticEntryPagesGet(void)
{
  L7_uint32 staticCnt = 0;
  
  staticCnt = usmDbDsStaticBindingsCount() ;
  if (staticCnt%SNOOP_MAX == 0) 
   return (staticCnt/SNOOP_MAX);
  else
  return((staticCnt/SNOOP_MAX)+1);
}

/****************************************************
* Get the Nuber of DS Database Dynamic Entries Pages 
*
****************************************************/
L7_uint32 usmWebDynamicEntryPagesGet(void)
{
  L7_uint32 dynamic = 0, staticCnt = 0, total =0;

  staticCnt = usmDbDsStaticBindingsCount() ;
  total = usmDbDsBindingsCount();
  dynamic = total - staticCnt;

  if (dynamic%SNOOP_MAX == 0)
   return (dynamic/SNOOP_MAX);
  else
  return((dynamic/SNOOP_MAX)+1);
}

/*********************************************************************
 *
 * @purpose Generate the list  
 *
 * @param
 *
 * @returns
 *
 * @end
 *
 *********************************************************************/
void *ewaFormSelect_spage(EwsContext context, EwsFormSelectOptionP optionp, void * iterator )
{
  dhcpSnoopBinding_t dsBinding;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 buf[APP_BUFFER_SIZE] ;
  static L7_uint32 spage, selected_spage, total;
  L7_char8 *bufChoice ;
  
  memset(buf, 0, sizeof(buf));
  memset (&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  net = ewsContextNetHandle(context);

  if(iterator == L7_NULL)
  {
    spage = 1;
    selected_spage = 1;
    total = 1;
    total = usmWebStaticEntryPagesGet();
    memset (&dsBinding, 0, sizeof(dsBinding));
    if(net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      selected_spage = appInfo.data[2];
    }
  }
  else
  {
    spage++;     
    if (spage > total)
    {
      return NULL;
    }
  }
  optionp->valuep = (void *) &spage ;
  bufChoice = ewsContextNetHandle(context)->buffer ;
  osapiSnprintf (buf, sizeof(buf),"%d",spage );
  memcpy(bufChoice, buf, APP_BUFFER_SIZE-1) ;
  optionp->choice = bufChoice ;
  if (spage == selected_spage)
  {
    optionp->selected = TRUE ;
  }
  else
  {
    optionp->selected = FALSE ;
  }

  return (void *) optionp ;

}

/*********************************************************************
 *
 * @purpose Generate the list  
 *
 * @param
 *
 * @returns
 *
 * @end
 *
 *********************************************************************/
void *ewaFormSelect_dpage(EwsContext context, EwsFormSelectOptionP optionp,
                          void * iterator )
{
  dhcpSnoopBinding_t dsBinding;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 buf[APP_BUFFER_SIZE] ;
 static L7_uint32 dpage, selected_dpage, total;
  L7_uchar8 *bufChoice ;
  memset(buf, 0, sizeof(buf));
  net = ewsContextNetHandle(context);
  memset (&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  if(iterator == L7_NULL)
  {
    dpage = 1;
    selected_dpage = 1;
    total = 1;
    total = usmWebDynamicEntryPagesGet();
    memset (&dsBinding, 0, sizeof(dsBinding));
    if(net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      selected_dpage = appInfo.data[3];
    }
  }
  else
  {
    dpage++;     
    if (dpage > total)
    {
      return NULL;
    }
  }
  optionp->valuep = (void *) &dpage ;
  bufChoice = ewsContextNetHandle(context)->buffer ;
  osapiSnprintf (buf, sizeof(buf),"%d",dpage );
  memcpy(bufChoice, buf, APP_BUFFER_SIZE-1) ;
  optionp->choice = bufChoice ;

  if (dpage == selected_dpage)
  {
    optionp->selected = TRUE ;
  }
  else
  {
    optionp->selected = FALSE ;
  }

  return (void *) optionp ;

}

