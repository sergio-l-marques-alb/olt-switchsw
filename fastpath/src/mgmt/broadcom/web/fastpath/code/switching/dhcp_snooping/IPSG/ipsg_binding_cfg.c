/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/emweb/web/switching/dhcp_snooping/ipsg_binding_cfg.c
*
* @purpose Code in support of the ipsg_binding_cfg.html page
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


#include <l7_common.h>
#ifdef L7_IPSG_PACKAGE
#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_switching_common.h"
#include "strlib_switching_web.h"
#include <stdio.h>
#include <string.h>
#include <ew_proto.h>
#include <ewnet.h>

#include <log.h>
#include <web.h>
#include "web_oem.h"
#include "usmdb_dhcp_snooping.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"
#include "util_pstring.h"
#define IPSG_MAX 15


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

void *ewaFormSelect_ipsg_port_intf(EwsContext context,
                              EwsFormSelectOptionP optionp,
                              void * iterator)
{

static L7_uint32 sel_intifnum, intIfNum;
  L7_uint32 unit;
  L7_char8 buf[APP_BUFFER_SIZE];  /* slot.port */
  L7_char8 *bufChoice;
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
* @param form pointer to the ipsg_binding_cfg form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_ipsg_binding_cfg( EwsContext context, EwaForm_ipsg_binding_cfgP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 unit, intIfNum=0, vlanid, spage, dpage;
  L7_uint32 u, s, p;
  L7_uchar8 slotPort[10];
  L7_uint32 user_access, elementN, cnt;
  ipsgBinding_t ipsgBinding;
  L7_BOOL verifyMac, verifyIp;
  
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
    spage =1;
    dpage = 1;
  }
  if (user_access == USMWEB_READWRITE)
  {

    form->value.ipsg_port_intf = intIfNum;
    form->status.ipsg_port_intf |= (EW_FORM_INITIALIZED);
    form->value.dhcp_snoop_vlan = vlanid;
    form->status.dhcp_snoop_vlan |= (EW_FORM_INITIALIZED); 
    memset (form->value.senderMac, 0, L7_ENET_MAC_ADDR_LEN); 
    form->status.senderMac |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    form->value.senderIP = 0;
    form->status.senderIP |= (EW_FORM_INITIALIZED);
  }

  form->value.ipsg_spage = spage;
  form->status.ipsg_spage  |= (EW_FORM_INITIALIZED);

  form->value.ipsg_dpage = dpage;
  form->status.ipsg_dpage |= (EW_FORM_INITIALIZED);

  /* Getting The Static Binding Entries */ 
  for (cnt = 0; cnt < form->value.ipsgStaticList_repeats; cnt++)
  {
    elementN = ((spage-1)*(IPSG_MAX))+cnt+1;

    if (usmDbIpsgBindingNthEntryGet(&ipsgBinding, elementN, IPSG_ENTRY_STATIC) != L7_SUCCESS)
    {
       break;
    }
    /* Interface */
    intIfNum = ipsgBinding.intIfNum;

    usmDbUnitSlotPortGet(intIfNum, &u, &s, &p);
    osapiSnprintf (slotPort, sizeof(slotPort)-1, strUtilUspGet (unit, u, s, p, L7_NULLPTR, 0));

    form->value.ipsgStaticList[cnt].sinterface = osapiStrDup(slotPort);
    form->status.ipsgStaticList[cnt].sinterface |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
     /* This is hidden value for easy refernce of intIfNum when Removing the Binding*/
    form->value.ipsgStaticList[cnt].sinterface_hid = intIfNum;
    form->status.ipsgStaticList[cnt].sinterface_hid |= (EW_FORM_INITIALIZED );
    
    /* Mac Address */
    memcpy (form->value.ipsgStaticList[cnt].smacaddr, ipsgBinding.macAddr, L7_ENET_MAC_ADDR_LEN);
    form->status.ipsgStaticList[cnt].smacaddr |= (EW_FORM_INITIALIZED |  EW_FORM_DYNAMIC);
    
    /* VLAN */
    form->value.ipsgStaticList[cnt].svlan  = ipsgBinding.vlanId;
    form->status.ipsgStaticList[cnt].svlan |= (EW_FORM_INITIALIZED );

    /* IP ADDRESS */
    form->value.ipsgStaticList[cnt].sipaddress  = ipsgBinding.ipAddr;
    form->status.ipsgStaticList[cnt].sipaddress |= (EW_FORM_INITIALIZED );

    /* Filter Type */
    if (usmDbIpsgVerifySourceGet(intIfNum, &verifyIp, &verifyMac)== L7_SUCCESS)
    {
      if(verifyMac)
      {
        form->value.ipsgStaticList[cnt].sfilterType = osapiStrDup("ip-mac");
      }
      else
      {
        form->value.ipsgStaticList[cnt].sfilterType = osapiStrDup("ip");
      }
   }
   else
   {
     form->value.ipsgStaticList[cnt].sfilterType = osapiStrDup(pStrErr_common_Err);
   }
    form->status.ipsgStaticList[cnt].sfilterType  |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }
  /* Getting The Dynamic Binding Entries */
  for (cnt = 0; cnt < form->value.ipsgDynamicList_repeats; cnt++)
  {
    elementN = ((dpage-1)*(IPSG_MAX))+cnt+1;

    if (usmDbIpsgBindingNthEntryGet(&ipsgBinding, elementN, IPSG_ENTRY_DYNAMIC) != L7_SUCCESS)
    {
       break;
    }
    /* Interface */
    intIfNum = ipsgBinding.intIfNum; 
    usmDbUnitSlotPortGet(intIfNum, &u, &s, &p);
    osapiSnprintf (slotPort, sizeof(slotPort)-1, strUtilUspGet (unit, u, s, p, L7_NULLPTR, 0));

    form->value.ipsgDynamicList[cnt].dinterface = osapiStrDup(slotPort);
    form->status.ipsgDynamicList[cnt].dinterface |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

    /* Mac Address */ 
    memcpy (form->value.ipsgDynamicList[cnt].dmacaddr, ipsgBinding.macAddr, L7_ENET_MAC_ADDR_LEN);
    form->status.ipsgDynamicList[cnt].dmacaddr |= (EW_FORM_INITIALIZED |  EW_FORM_DYNAMIC);

    /* Vlan */
    form->value.ipsgDynamicList[cnt].dvlan  = ipsgBinding.vlanId;
    form->status.ipsgDynamicList[cnt].dvlan |= (EW_FORM_INITIALIZED );

    /* IP ADDRESS */
    form->value.ipsgDynamicList[cnt].dipaddress  = ipsgBinding.ipAddr;
    form->status.ipsgDynamicList[cnt].dipaddress |= (EW_FORM_INITIALIZED );

    /* Filter Type */  
    if (usmDbIpsgVerifySourceGet(intIfNum, &verifyIp, &verifyMac)== L7_SUCCESS)
    {
      if(verifyIp)
      {
        form->value.ipsgDynamicList[cnt].dfilterType = osapiStrDup("ip-mac");
      }
      else
      {
        form->value.ipsgDynamicList[cnt].dfilterType = osapiStrDup("ip");
      }
    }
    else
    {
      form->value.ipsgDynamicList[cnt].dfilterType = osapiStrDup(pStrErr_common_Err);
    }
    form->status.ipsgDynamicList[cnt].dfilterType  |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);


  }
 
  form->value.Add = (char *) osapiStrDup(pStrInfo_common_Add);
  form->status.Add |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
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
* @param form pointer to the ipsg_binding_cfg form

*
* @returns ipsg_binding_cfg.html
*
* @end
*
*********************************************************************/
char *ewaFormSubmit_ipsg_binding_cfg(EwsContext context, EwaForm_ipsg_binding_cfgP form)
{
  usmWeb_AppInfo_t appInfo;
  EwaNetHandle net;
  L7_uint32 intIfNum, vlanid, index;
  L7_uint32 spage, dpage, delCount = 0;
  L7_enetMacAddr_t macAddr;
  L7_RC_t rc = L7_FAILURE;

  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));

  intIfNum = form->value.ipsg_port_intf; 
  vlanid = form->value.dhcp_snoop_vlan;
  spage  = form->value.ipsg_spage;
  dpage  = form->value.ipsg_dpage;
  appInfo.data[0] = intIfNum;
  appInfo.data[1] = vlanid;
  appInfo.data[2] = spage;
  appInfo.data[3] = dpage;

  /* Adding a New Static Entry */
  if ((form->status.Add & EW_FORM_RETURNED))
  {
     if (form->status.senderMac & EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error, pStrErr_common_FieldInvalidFmt,
                                pStrInfo_common_MacAddr_2);
    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_switching_HtmlFileIpsgBindingCfg);
    }
    if (form->status.senderIP & EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error, pStrErr_common_FieldInvalidFmt,
                                pStrInfo_common_IpAddr_1);
    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_switching_HtmlFileIpsgBindingCfg);
    }
    memcpy (macAddr.addr, form->value.senderMac, sizeof(macAddr.addr));
    if (usmDbIpsgStaticEntryAdd(intIfNum, vlanid, &macAddr, form->value.senderIP) != L7_SUCCESS)
    {
      usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrErr_common_FailedToSet_1, pStrErr_switching_CouldNotAddBinding);
    }
  }
   /* Removing the selected Static IPSG Binding Entries */
  if ((form->status.Submit & EW_FORM_RETURNED))
  {
     for (index = 0; index < form->value.ipsgStaticList_repeats;
             index++)
     {
        if (EW_FORM_BOOLEAN_TRUE(form, ipsgStaticList[index].Remove))
        {
               memcpy (macAddr.addr, form->value.ipsgStaticList[index].smacaddr, sizeof(macAddr.addr)); 
               rc =  usmDbIpsgStaticEntryRemove(form->value.ipsgStaticList[index].sinterface_hid,
                                             form->value.ipsgStaticList[index].svlan, &macAddr,
                                             form->value.ipsgStaticList[index].sipaddress);
               if (rc != L7_SUCCESS)
               {
                 usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_FailedToSet,
                 pStrErr_common_FailedToSet_1, pStrInfo_common_MacAddr_2);
               }
               else
              {
                delCount++;
              }

         }
      }
      /* If all the elements on the selected page are deleted the Make sure previous page to be displayed  */
      if (delCount == form->value.ipsgStaticList_repeats)
      {
        if (spage != 1)
        appInfo.data[2] = (spage -1);
      }

  }
    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_switching_HtmlFileIpsgBindingCfg);
 }

/* *********************************************** */
/*  To Get The Number IPSG static Entries          */
/*                                                 */
/* *********************************************** */
L7_RC_t usmWebIpsgStaticEntriesCountGet(L7_uint32 *count, L7_uint32 spage)
{
  L7_uint32 staticCnt =0, rem;

  staticCnt = usmDbIpsgStaticBindingsCount();
  rem = staticCnt -((spage-1) * IPSG_MAX);
  if (rem > IPSG_MAX)
  rem = IPSG_MAX;

  *count = rem;
  return L7_SUCCESS;
}

/* *********************************************** */
/*  To Get The Number IPSG Dynamic Entries         */
/*                                                 */
/* *********************************************** */
L7_RC_t usmWebIpsgDynamicEntriesCountGet(L7_uint32 *count, L7_uint32 dpage)
{
  L7_uint32 total = 0, staticCnt = 0, dynamic =0, rem;
 
  total = usmDbIpsgBindingsCount();
  staticCnt = usmDbIpsgStaticBindingsCount();
  dynamic = total - staticCnt;
  rem = dynamic  -((dpage-1) * IPSG_MAX);
  if (rem> IPSG_MAX)
  {
    *count = IPSG_MAX;
  }
  else
  {
     *count = rem;
  }

  return L7_SUCCESS;

}
/* *********************************************** */
/*  To Get The Number IPSG database Static Pages   */
/*                                                 */
/* *********************************************** */
L7_uint32 usmWebIpsgStaticEntryPagesGet(void)
{
  L7_uint32 staticCnt =0;

  staticCnt = usmDbIpsgStaticBindingsCount();
  if (staticCnt%IPSG_MAX == 0)
   return (staticCnt/IPSG_MAX);
  else
  return((staticCnt/IPSG_MAX)+1);
}

/* *********************************************** */
/*  To Get The Number IPSG database DYNAMIC Pages  */
/*                                                 */
/* *********************************************** */
L7_uint32 usmWebIpsgDynamicEntryPagesGet(void)
{
  L7_uint32 total =0, staticCnt = 0, dynamicCnt = 0;
  
  total = usmDbIpsgBindingsCount();
  staticCnt = usmDbIpsgStaticBindingsCount();
  dynamicCnt = total - staticCnt;

  if (dynamicCnt%IPSG_MAX == 0)
   return (dynamicCnt/IPSG_MAX);
  else
  return((dynamicCnt/IPSG_MAX)+1);
}
/*********************************************************************
 *
 * @purpose Generate the IPSG Database static pages list
 *
 * @param
 *
 * @returns
 *
 * @end
 *
 *********************************************************************/
void *ewaFormSelect_ipsg_spage(EwsContext context, EwsFormSelectOptionP optionp, void * iterator )
{
  dhcpSnoopBinding_t dsBinding;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 buf[APP_BUFFER_SIZE] ;
  static L7_uint32 spage, selected_spage, total;
  L7_char8 *bufChoice ;

  memset(buf, 0, sizeof(buf));
  net = ewsContextNetHandle(context);
  memset (&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  if(iterator == L7_NULL)
  {
    spage = 1;
    selected_spage = 1;
    total = 1;
    total = usmWebIpsgStaticEntryPagesGet();
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
 * @purpose Generate the IPSG Database Dynamic Pages List
 *
 * @param
 *
 * @returns
 *
 * @end
 *
 *********************************************************************/
void *ewaFormSelect_ipsg_dpage(EwsContext context, EwsFormSelectOptionP optionp,
                          void * iterator )
{
  dhcpSnoopBinding_t dsBinding;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 buf[APP_BUFFER_SIZE] ;
 static L7_uint32 dpage, selected_dpage, total=1;
  L7_uchar8 *bufChoice ;

  memset (&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  memset(buf, 0, sizeof(buf));
  net = ewsContextNetHandle(context);
  if(iterator == L7_NULL)
  {
    dpage = 1;
    selected_dpage = 1;
    total = 1;
    total = usmWebIpsgDynamicEntryPagesGet();
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

#endif
