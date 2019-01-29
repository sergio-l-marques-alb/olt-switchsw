/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/routing/l3_web.c
 *
 * @purpose Code in support of the various EmWeb html pages
 *
 * @component unitmgr
 *
 * @comments tba
 *
 * @create 24-jan-2002
 *
 * @author tgaunce
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_routing_common.h"
#include "strlib_routing_web.h"
#include "l7_common.h"
#include "usmdb_ip_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_ospf_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
#include "usmdb_ip6_api.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "util_pstring.h"
#include "usmdb_rlim_api.h"
#include "web.h"
#include "l3_web.h"
#include "ews_api.h"
#include "ewnet.h"

extern usmWeb_AppInfo_t appInfo;

/*********************************************************************
*
* @purpose Generate the list of valid ipv4 routing interfaces
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
void *ewaFormSelect_valid_ipv4_intf(EwsContext context,
                                    EwsFormSelectOptionP optionp,
                                    void * iterator )
{
  static L7_uint32 sel_intifnum;
  static L7_uint32 intIfNum;
  L7_char8 buf[APP_BUFFER_SIZE];
  L7_char8 * bufChoice;
  L7_uint32 u,s,p;
  L7_uint32 sysIntfType;
#ifdef L7_RLIM_PACKAGE
  L7_uint32 loopbackId;
  L7_uint32 tunnelId;
#endif
  L7_BOOL useUSP;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  if (iterator == L7_NULL)
  {
    if (usmDbIpMapValidIntfFirstGet(&intIfNum) != L7_SUCCESS)
    {
      return L7_NULL;
    }

    net = ewsContextNetHandle(context);
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
    if (usmDbIpMapValidIntfNextGet(intIfNum, &intIfNum) != L7_SUCCESS)
    {
      return L7_NULL;
    }
  }

  if (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) != L7_SUCCESS)
  {
    OSAPI_STRNCPY_SAFE(buf, pStrInfo_common_Error_2);
  }
  else
  {
    useUSP = L7_TRUE;
    if (usmDbIntfTypeGet(intIfNum, &sysIntfType) == L7_SUCCESS)
    {
      switch (sysIntfType)
      {
#ifdef L7_RLIM_PACKAGE
      case L7_LOOPBACK_INTF:
        if (usmDbRlimLoopbackIdGet(intIfNum, &loopbackId) == L7_SUCCESS)
        {
          osapiSnprintf(buf, sizeof(buf),  "%s %d", pStrInfo_common_LoopBack, loopbackId);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), pStrInfo_common_LoopbackX);
        }
        useUSP = L7_FALSE;
        break;

      case L7_TUNNEL_INTF:
        if (usmDbRlimTunnelIdGet(intIfNum, &tunnelId) == L7_SUCCESS)
        {
          osapiSnprintf(buf, sizeof(buf),  "%s %d", pStrInfo_common_Tunnel_1, tunnelId);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), pStrInfo_common_TunnelX);
        }
        useUSP = L7_FALSE;
        break;
#endif
      case L7_PHYSICAL_INTF:
      case L7_STACK_INTF:
      case L7_CPU_INTF:
      case L7_LAG_INTF:
      case L7_LOGICAL_VLAN_INTF:
      default:
        useUSP = L7_TRUE;
        break;
      }
    }
    else
    {
      OSAPI_STRNCPY_SAFE(buf, pStrInfo_common_Error_2);
    }

    if (useUSP == L7_TRUE)
    {
      if (usmDbComponentPresentCheck(0, L7_FLEX_STACKING_COMPONENT_ID))
      {
        osapiSnprintf(buf, sizeof(buf),  "%d/%d/%d", u, s, p);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf),  "%d/%d", s, p);
      }
    }
  }
  optionp->valuep = (void *) &intIfNum;

  bufChoice = ewsContextNetHandle(context)->buffer;
  strncpy(bufChoice, buf, APP_BUFFER_SIZE-1);
  optionp->choice = bufChoice;
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
* @purpose Generate the list of configured Router Ports.
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
void *l3_select_rtr_port(EwsContext context,
                         EwsFormSelectOptionP optionp,
                         void * iterator,
                         L7_BOOL (* compIsValidIntf)(L7_uint32,
                                                     L7_uint32))
{
  static L7_uint32 itrValid;
  static L7_uint32 sel_intifnum;
  static L7_uint32 rtrIntIfNum;
  L7_uint32 rtr_intf_mode;
  L7_char8 buf[APP_BUFFER_SIZE];
  L7_char8 * bufChoice;
  L7_uint32 u,s,p;
  L7_RC_t rc;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  itrValid = L7_FALSE;

  if (iterator == L7_NULL)
  {
    rtrIntIfNum = 1;
    sel_intifnum = 0;
    net = ewsContextNetHandle(context);
    if (net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      sel_intifnum = appInfo.data[0];
    }
  }
  else
  {
    rtrIntIfNum++;
  }

  rc = usmDbIpRtrIntfModeGet(unit, rtrIntIfNum, &rtr_intf_mode);
  if (rc != L7_SUCCESS || (*compIsValidIntf)(unit, rtrIntIfNum) != L7_TRUE)
  {
    rtr_intf_mode = L7_DISABLE;
  }
  while ((rtr_intf_mode == L7_DISABLE) && (rtrIntIfNum < L7_ALL_INTERFACES))
  {
    rtrIntIfNum++;
    rc = usmDbIpRtrIntfModeGet(unit, rtrIntIfNum, &rtr_intf_mode);
    if (rc != L7_SUCCESS || (*compIsValidIntf)(unit, rtrIntIfNum) != L7_TRUE)
    {
      rtr_intf_mode = L7_DISABLE;
    }
  }
  if (rtr_intf_mode == L7_ENABLE)
  {
    if (sel_intifnum == 0)
    {                        /* If no selection use the first defined router port */
      sel_intifnum =  rtrIntIfNum;
    }

    rc = usmDbUnitSlotPortGet(rtrIntIfNum, &u, &s, &p);
    optionp->valuep = (void *) &rtrIntIfNum;
    osapiSnprintf (buf, sizeof (buf), strUtilUspGet (unit, u, s, p, L7_NULLPTR, 0));

    bufChoice = ewsContextNetHandle(context)->buffer;
    memset(bufChoice, 0, APP_BUFFER_SIZE);
    strncpy(bufChoice, buf, APP_BUFFER_SIZE-1);
    optionp->choice = bufChoice;                                          /* --slot.port-- */
    if (rtrIntIfNum == sel_intifnum)
    {
      optionp->selected = TRUE;
    }
    else
    {
      optionp->selected = FALSE;
    }
    itrValid = L7_TRUE;
  }

  if (itrValid == L7_TRUE)
  {
    return (void *) optionp;
  }
  else
  {
    return L7_NULL;
  }

}

#ifdef L7_IPV6_PACKAGE
/*********************************************************************
*
* @purpose Generate the list of configured V6 Router Ports.
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
void *l3_select_V6rtr_port(EwsContext context,
                           EwsFormSelectOptionP optionp,
                           void * iterator,
                           L7_BOOL (* compIsValidIntf)(L7_uint32,
                                                       L7_uint32))
{
  static L7_uint32 itrValid;
  static L7_uint32 sel_intifnum;
  static L7_uint32 rtrIntIfNum;
  L7_uint32 rtr_intf_mode;
  L7_char8 buf[APP_BUFFER_SIZE];
  L7_char8 * bufChoice;
  L7_uint32 u,s,p;
  L7_RC_t rc;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  itrValid = L7_FALSE;

  if (iterator == L7_NULL)
  {
    rtrIntIfNum = 1;
    sel_intifnum = 0;
    net = ewsContextNetHandle(context);
    if (net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      sel_intifnum = appInfo.data[0];
    }
  }
  else
  {
    rtrIntIfNum++;
  }

/*  rc = usmDbIpRtrIntfModeGet(unit, rtrIntIfNum, &rtr_intf_mode);*/
  rc = usmDbIp6RtrIntfOperModeGet(rtrIntIfNum, &rtr_intf_mode);
  if (rc != L7_SUCCESS || (*compIsValidIntf)(unit, rtrIntIfNum) != L7_TRUE)
  {
    rtr_intf_mode = L7_DISABLE;
  }
  while ((rtr_intf_mode == L7_DISABLE) && (rtrIntIfNum < L7_ALL_INTERFACES))
  {
    rtrIntIfNum++;
    /*rc = usmDbIpRtrIntfModeGet(unit, rtrIntIfNum, &rtr_intf_mode);*/
    rc = usmDbIp6RtrIntfOperModeGet(rtrIntIfNum, &rtr_intf_mode);
    if (rc != L7_SUCCESS || (*compIsValidIntf)(unit, rtrIntIfNum) != L7_TRUE)
    {
      rtr_intf_mode = L7_DISABLE;
    }
  }

  if (rtr_intf_mode == L7_ENABLE)
  {

    if (sel_intifnum == 0)
    {                        /* If no selection use the first defined router port */
      sel_intifnum =  rtrIntIfNum;
    }

    rc = usmDbUnitSlotPortGet(rtrIntIfNum, &u, &s, &p);
    optionp->valuep = (void *) &rtrIntIfNum;
    osapiSnprintf (buf, sizeof (buf), strUtilUspGet (unit, u, s, p, L7_NULLPTR, 0));

    bufChoice = ewsContextNetHandle(context)->buffer;
    memset(bufChoice, 0, APP_BUFFER_SIZE);
    strncpy(bufChoice, buf, APP_BUFFER_SIZE-1);
    optionp->choice = bufChoice;                                          /* --slot.port-- */
    if (rtrIntIfNum == sel_intifnum)
    {
      optionp->selected = TRUE;
    }
    else
    {
      optionp->selected = FALSE;
    }
    itrValid = L7_TRUE;
  }

  if (itrValid == L7_TRUE)
  {
    return (void *) optionp;
  }
  else
  {
    return L7_NULL;
  }

}
#endif

#ifdef L7_OSPF_PACKAGE
/**************************************************************************
 * Function: usmWebOspfAdminModeGet
 *
 * Description:
 *     Returns the ospf admin mode
 *
 * Parameters:
 *
 * Return:
 *
 * Notes:
 *************************************************************************/

L7_uint32 usmWebOspfIsAdminModeEnabled(L7_uint32 unit)
{
  L7_uint32 rc = 0;
  L7_uint32 val;

  if (usmDbOspfAdminModeGet (unit, &val) == L7_SUCCESS)
  {
    if (val != L7_ENABLE)
    {
      rc = L7_FAILURE;
    }
    else
    {rc = L7_SUCCESS;}
  }
  return rc;
}
#endif

/**************************************************************************
 * Description: This function Determines whether a given string of the format Slot.Port
 *
 * Parameters: slotPort - A character string representing slot.port
 *             slot  - If valid Slot.Port, this contains slot part.
 *             port  - If valid Slot.Port, this contains Port part.
 *
 * Return:  L7_SUCCESS if the string is in slot.port format
 *          L7_FAILURE Otherwise
 *
 *************************************************************************/
L7_RC_t usmWebParseSlotPort(L7_uchar8 * slotPort,L7_uint32 * slotP, L7_uint32 * portP)
{
  register L7_uchar8 c;
  register L7_uint32 slot, port;
  L7_BOOL dotFlag;

  if(slotPort == NULL)
  {
    return L7_FAILURE;
  }

  dotFlag=L7_FALSE;
  slot=USMWEB_INVALID_VALUE;
  port=USMWEB_INVALID_VALUE;

  c = *slotPort;
  if (!isdigit(c))
  {
    return L7_FAILURE;
  }

  for (;;)
  {
    if(c=='\0')
    {
      if((slot==USMWEB_INVALID_VALUE)||(port==USMWEB_INVALID_VALUE))
      {
        return L7_FAILURE;
      }
      else
      {
        *slotP = slot;
        *portP = port;
        return L7_SUCCESS;
      }
    }

    if(isdigit(c))
    {
      if(dotFlag!=L7_TRUE)
      {
        if(slot == USMWEB_INVALID_VALUE)
        {
          slot=0;
        }
        slot = (slot * 10 )+(c - '0');
      }
      else
      {
        if(port == USMWEB_INVALID_VALUE)
        {
          port=0;
        }
        port = (port * 10)+(c - '0');
      }
    }
    else
    if((c=='.')&&(dotFlag!=L7_TRUE))
    {
      dotFlag=L7_TRUE;
    }
    else
    {
      return L7_FAILURE;
    }
    c=*++slotPort;
  }
}

/**************************************************************************
 * Function: usmWebIsRouterPortAvaialable
 *
 * Description:
 *     Returns L7_TRUE if any router port is available else
 *     Returns L7_FALSE
 *
 * Parameters:
 *
 * Return:
 *
 * Notes:
 *************************************************************************/
L7_BOOL usmWebIsRouterPortAvaialable(L7_uint32 unit)
{
  L7_uint32 index, intIfNum = 0, mode;

  unit = usmDbThisUnitGet();

  for (index = 1; index <= L7_RTR_MAX_RTR_INTERFACES; index++)
  {
    if (ipMapRtrIntfToIntIfNum(index, &intIfNum) == L7_SUCCESS)
    {
      if(usmDbIpRtrIntfModeGet(unit, intIfNum,&mode)== L7_SUCCESS && mode == L7_ENABLE)
      {
        intIfNum = index;
        break;
      }
      else
      {
        intIfNum = 0;
      }
    }
  }

  if(intIfNum != 0)
  {
    return L7_TRUE;
  }
  else
  {
    return L7_FALSE;
  }
}

/*********************************************************************
*
* @purpose Generate the list of valid ipv6 routing interfaces
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
void *webFormSelect_ipv4_intf_secaddr(EwsContext context,
                                      EwsFormSelectOptionP optionp,
                                      void * iterator,
                                      L7_uint32 intIfNum)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 i;
  L7_uint32 addrCount;
  static L7_RC_t rc;
  static L7_uint32 selected_ipaddr;
  static L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  static L7_uint32 ndx;
  static L7_uint32 ipaddr;
  static L7_char8 ipshowbuf[USMWEB_IPADDR_SIZE+1];
  L7_uint32 user_access;

  if (iterator == L7_NULL)
  {
    rc = usmDbIpRtrIntfCfgIpAddrListGet(0, intIfNum, ipAddrList);
    addrCount = 0;
    selected_ipaddr = 0;
    for (i=1; i < L7_L3_NUM_IP_ADDRS; i++)
    {
      if (ipAddrList[i].ipAddr != 0)
      {
        selected_ipaddr = ipAddrList[i].ipAddr;
        addrCount++;
      }
    }

    user_access = usmWebUserAccessGet(context);
    net = ewsContextNetHandle(context);
    if (net->app_pointer != NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      selected_ipaddr = appInfo.data[2];
    }

    /* Don't offer create or submit if the L7_L3_NUM_IP_ADDRS reached */
    if ((addrCount < (L7_L3_NUM_IP_ADDRS-1)) && (user_access == USMWEB_READWRITE))
    {
      OSAPI_STRNCPY_SAFE(ipshowbuf, pStrInfo_routing_AddSecondary);
      ndx = 0;
      optionp->valuep = (void *)&ndx;
      optionp->choice = ipshowbuf;
      if (selected_ipaddr == 0)
      {
        optionp->selected = TRUE;
      }
      else
      {
        optionp->selected = FALSE;
      }
      return (void *) optionp;
    }
  }

  ndx++;

  if (ndx >= L7_L3_NUM_IP_ADDRS)
  {
    return L7_NULL;
  }

  while ((ipAddrList[ndx].ipAddr == 0) && (ndx < (L7_L3_NUM_IP_ADDRS-1)))
  {
    ndx++;
  }          /* find the next secondary address */

  if (ipAddrList[ndx].ipAddr != 0)
  {
    if (usmDbInetNtoa((L7_ulong32)(ipAddrList[ndx].ipAddr),ipshowbuf) != L7_SUCCESS)
    {
      OSAPI_STRNCPY_SAFE(ipshowbuf,pStrInfo_common_EmptyString);
    }
    ipaddr = ipAddrList[ndx].ipAddr;
    optionp->valuep = (void *) &ipaddr;
    optionp->choice = ipshowbuf;
    if (selected_ipaddr == ipaddr)
    {
      optionp->selected = TRUE;
    }
    else
    {
      optionp->selected = FALSE;
    }
    return (void *) optionp;
  }

  return L7_NULL;
}
