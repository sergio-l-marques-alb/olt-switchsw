/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
 * @filename src/mgmt/emweb/web/routing/arp/arp_table_cfg.c
*
* @purpose Code in support of the arp_table_cfg.html page
*
* @component unitmgr
*
* @comments tba
*
* @create 05/15/2001
*
* @author betsyt
* @end
*
**********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_routing_common.h"
#include "strlib_routing_web.h"
#include <l7_common.h>
#include <l3_comm_structs.h>
#include <l3_commdefs.h>
#include <stdio.h>
#include <string.h>
#include <ew_proto.h>
#include <ewnet.h>
#include "util_enumstr.h"
#include <log.h>
#include <web.h>
#include <usmdb_ip_api.h>
#include "web_oem.h"
#include "usmdb_ip_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"

static L7_char8  usmWebBuffer[256];
static  L7_char8 usmWebBigBuf[1024];

/*********************************************************************
*
* @purpose Get the Arp Agetime
*
* @param unit  Unit Number
*
* @returns usmWebBuffer with the Arp Agetime
*
* @end
*
*********************************************************************/

L7_char8 *usmWebIpArpAgeTimeGet(L7_uint32 unit)
{
  L7_RC_t    rc;
  L7_uint32  age;

  memset(usmWebBuffer, 0, sizeof(usmWebBuffer));
  rc = usmDbIpArpAgeTimeGet(unit, &age);
  osapiSnprintf(usmWebBuffer, sizeof(usmWebBuffer),  "%u", age);

  return usmWebBuffer;
}

/*********************************************************************
*
* @purpose Get the Arp Resptime
*
* @param unit  Unit Number
*
* @returns usmWebBuffer with the Arp Resptime
*
* @end
*
*********************************************************************/

L7_char8 *usmWebIpArpRespTimeGet(L7_uint32 unit)
{
  L7_RC_t    rc;
  L7_uint32  resp;

  memset(usmWebBuffer, 0, sizeof(usmWebBuffer));
  rc = usmDbIpArpRespTimeGet(unit, &resp);
  osapiSnprintf(usmWebBuffer, sizeof(usmWebBuffer),  "%u", resp);

  return usmWebBuffer;
}

/*********************************************************************
*
* @purpose Get the Arp Retries
*
* @param unit  Unit Number
*
* @returns usmWebBuffer with the Arp Retries
*
* @end
*
*********************************************************************/

L7_char8 *usmWebIpArpRetriesGet(L7_uint32 unit)
{
  L7_RC_t    rc;
  L7_uint32  retries;

  memset(usmWebBuffer, 0, sizeof(usmWebBuffer));
  rc = usmDbIpArpRetriesGet(unit, &retries);
  osapiSnprintf(usmWebBuffer, sizeof(usmWebBuffer),  "%u", retries);

  return usmWebBuffer;
}

/*********************************************************************
*
* @purpose Get the Arp Cachesize
*
* @param unit  Unit Number
*
* @returns usmWebBuffer with the Arp Cachesize
*
* @end
*
*********************************************************************/

L7_char8 *usmWebIpArpCacheSizeGet(L7_uint32 unit)
{
  L7_RC_t    rc;
  L7_uint32  num;

  memset(usmWebBuffer, 0, sizeof(usmWebBuffer));
  rc = usmDbIpArpCacheSizeGet(unit, &num);
  osapiSnprintf(usmWebBuffer, sizeof(usmWebBuffer),  "%u", num);

  return usmWebBuffer;
}

/*********************************************************************
*
* @purpose Get the Dynamic Renew Mode
*
* @param unit  Unit Number
*
* @returns usmWebBuffer with the Dynamic Renew Mode
*
* @end
*
*********************************************************************/

L7_char8 *usmWebDynamicRenewModeGet(L7_uint32 unit)
{
  L7_RC_t    rc;
  L7_uint32  num;

  memset(usmWebBuffer, 0, sizeof(usmWebBuffer));
  rc = usmDbIpArpDynamicRenewGet(unit, &num);
  osapiSnprintf(usmWebBuffer,sizeof(usmWebBuffer),strUtilEnableDisableGet(num,pStrInfo_common_Enbl_1));

  return usmWebBuffer;
}

/*********************************************************************
*
* @purpose Get the Total Entry Count
*
* @param unit  Unit Number
*
* @returns usmWebBuffer with the Total Entry Count
*
* @end
*
*********************************************************************/

L7_char8 *usmWebTotalEntryCountGet(L7_uint32 unit)
{
  L7_RC_t    rc;
  L7_arpCacheStats_t Stats;

  memset(usmWebBuffer, 0, sizeof(usmWebBuffer));
  rc = usmDbIpArpCacheStatsGet(unit, &Stats);
  osapiSnprintf(usmWebBuffer, sizeof(usmWebBuffer),  "%u", Stats.cacheCurrent);

  return usmWebBuffer;
}

/*********************************************************************
*
* @purpose Get the Peak Total Entries
*
* @param unit  Unit Number
*
* @returns usmWebBuffer with the Peak Total Entries
*
* @end
*
*********************************************************************/

L7_char8 *usmWebPeakTotalEntryGet(L7_uint32 unit)
{
  L7_RC_t    rc;
  L7_arpCacheStats_t Stats;

  memset(usmWebBuffer, 0, sizeof(usmWebBuffer));
  rc = usmDbIpArpCacheStatsGet(unit, &Stats);
  osapiSnprintf(usmWebBuffer, sizeof(usmWebBuffer),  "%u", Stats.cachePeak);

  return usmWebBuffer;
}

/*********************************************************************
*
* @purpose Get the Active Static Entry Count
*
* @param unit  Unit Number
*
* @returns usmWebBuffer with the Static Entry Count
*
* @end
*
*********************************************************************/

L7_char8 *usmWebStaticEntryCountGet(L7_uint32 unit)
{
  L7_RC_t    rc;
  L7_arpCacheStats_t Stats;

  memset(usmWebBuffer, 0, sizeof(usmWebBuffer));
  rc = usmDbIpArpCacheStatsGet(unit, &Stats);
  osapiSnprintf(usmWebBuffer, sizeof(usmWebBuffer),  "%u", Stats.staticCurrent);

  return usmWebBuffer;
}

/*********************************************************************
*
* @purpose Get the Configured Static Entry Count
*
* @param unit  Unit Number
*
* @returns usmWebBuffer with the Static Entry Count
*
* @end
*
*********************************************************************/

L7_char8 *usmWebCfgStaticEntryCountGet(L7_uint32 unit)
{
  osapiSnprintf(usmWebBuffer, sizeof(usmWebBuffer),  "%u", usmDbIpMapStaticArpCount(unit));

  return usmWebBuffer;
}

/*********************************************************************
*
* @purpose Get the Maximum Static Entries
*
* @param unit  Unit Number
*
* @returns usmWebBuffer with the Maximum Static Entries
*
* @end
*
*********************************************************************/

L7_char8 *usmWebMaxStaticEntryGet(L7_uint32 unit)
{
  L7_RC_t    rc;
  L7_arpCacheStats_t Stats;

  memset(usmWebBuffer, 0, sizeof(usmWebBuffer));
  rc = usmDbIpArpCacheStatsGet(unit, &Stats);
  osapiSnprintf(usmWebBuffer, sizeof(usmWebBuffer),  "%u", Stats.staticMax);

  return usmWebBuffer;
}

/*********************************************************************
*
* @purpose Get the Arp Table
*
* @param unit  Unit Number
*
* @param lvl7_flag  Flag for emweb iteration
*
* @param usmWebBigBuf  Used to buffer data for mac address entry
*
* @returns L7_SUCCESS or L7_FAILURE with the Arp Table Entry
*
* @end
*
*********************************************************************/

L7_char8 *usmWebArpInfoGet(L7_uint32 lvl7_flag, L7_uint32 unit, L7_arpEntry_t arpEntry)
{
  L7_RC_t rc;
  L7_uchar8 ipbuf[USMWEB_IPADDR_SIZE];
  L7_uint32 slot, port;
  L7_uint32 ageSecs, ageMins, ageHrs;
  L7_char8 status[36];
  L7_char8 ageFmt[32];

  memset(ipbuf, 0, sizeof(ipbuf));
  memset(usmWebBigBuf, 0, sizeof(usmWebBigBuf));
  memset(status, 0, sizeof(status));
  rc = usmDbInetNtoa(arpEntry.ipAddr, ipbuf);
  if (lvl7_flag == L7_TRUE)
  {                                                                            /* First time through the iterate */
    rc = usmDbUnitSlotPortGet(arpEntry.intIfNum, &unit, &slot, &port);

    if ((arpEntry.flags & L7_ARP_LOCAL) != 0)
    {
      osapiSnprintf(status, sizeof(status), pStrInfo_common_WsNwLocal);
    }                                                                                       /* Local */
    else if ((arpEntry.flags & L7_ARP_GATEWAY) != 0)
    {
      osapiSnprintf(status, sizeof(status), pStrInfo_routing_Gateway_1);
    }                                                                                       /* Gateway */
    else if ((arpEntry.flags & L7_ARP_STATIC) != 0)
    {
      osapiSnprintf(status, sizeof(status), pStrInfo_common_PimSmGrpRpMapStatic);
    }                                                                                       /* Static */
    else
    {
      osapiSnprintf(status, sizeof(status), pStrInfo_common_Dyn_1);
    }

    ageSecs = arpEntry.ageSecs;
    ageHrs = ageSecs / SECONDS_PER_HOUR;
    ageSecs %= SECONDS_PER_HOUR;
    ageMins = ageSecs / SECONDS_PER_MINUTE;
    ageSecs %= SECONDS_PER_MINUTE;
    if ((arpEntry.flags & (L7_ARP_LOCAL | L7_ARP_STATIC)) != 0)
    {
      osapiSnprintf(ageFmt, sizeof(ageFmt), pStrInfo_routing_NotApplicable_1);
    }
    else
    {
      osapiSnprintf(ageFmt, sizeof(ageFmt), "%2.2u:%2.2u:%2.2u", ageHrs, ageMins, ageSecs);
    }

    if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID))
    {
      osapiSnprintf(usmWebBigBuf, sizeof(usmWebBigBuf),  "%s%s %s%s%s\n %02X:%02X:%02X:%02X:%02X:%02X\n  %s%s%d/%d/%d\n %s%s%s\n %s%s%s\n %s%s",
                    "<tr>", "<td class=\"tabledata\">",
                    ipbuf, "</td>", "<td class=\"tabledata\">",
                    arpEntry.macAddr.addr.enetAddr.addr[0],
                    arpEntry.macAddr.addr.enetAddr.addr[1],
                    arpEntry.macAddr.addr.enetAddr.addr[2],
                    arpEntry.macAddr.addr.enetAddr.addr[3],
                    arpEntry.macAddr.addr.enetAddr.addr[4],
                    arpEntry.macAddr.addr.enetAddr.addr[5],
                    "</td>", "<td class=\"tabledata\">", unit, slot, port,
                    "</td>", "<td class=\"tabledata\">", status,
                    "</td>", "<td class=\"tabledata\">", ageFmt,
                    "</td>", "</tr>");
    }
    else
    {
      osapiSnprintf(usmWebBigBuf, sizeof(usmWebBigBuf),  "%s%s %s%s%s\n %02X:%02X:%02X:%02X:%02X:%02X\n  %s%s%d/%d\n %s%s%s\n %s%s%s\n %s%s",
                    "<tr>", "<td class=\"tabledata\">",
                    ipbuf, "</td>", "<td class=\"tabledata\">",
                    arpEntry.macAddr.addr.enetAddr.addr[0],
                    arpEntry.macAddr.addr.enetAddr.addr[1],
                    arpEntry.macAddr.addr.enetAddr.addr[2],
                    arpEntry.macAddr.addr.enetAddr.addr[3],
                    arpEntry.macAddr.addr.enetAddr.addr[4],
                    arpEntry.macAddr.addr.enetAddr.addr[5],
                    "</td>", "<td class=\"tabledata\">", slot, port,
                    "</td>", "<td class=\"tabledata\">", status,
                    "</td>", "<td class=\"tabledata\">", ageFmt,
                    "</td>", "</tr>");
    }
  }
  else
  {                                                                            /* After first time through */
    rc = usmDbUnitSlotPortGet(arpEntry.intIfNum, &unit, &slot, &port);
    if ((arpEntry.flags & L7_ARP_LOCAL) != 0)
    {
      osapiSnprintf(status, sizeof(status), pStrInfo_common_WsNwLocal);
    }                                                                                       /* Local */
    else if ((arpEntry.flags & L7_ARP_GATEWAY) != 0)
    {
      osapiSnprintf(status, sizeof(status), pStrInfo_routing_Gateway_1);
    }                                                                                       /* Gateway */
    else if ((arpEntry.flags & L7_ARP_STATIC) != 0)
    {
      osapiSnprintf(status, sizeof(status), pStrInfo_common_PimSmGrpRpMapStatic);
    }                                                                                       /* Static */
    else
    {
      osapiSnprintf(status, sizeof(status), pStrInfo_common_Dyn_1);
    }

    ageSecs = arpEntry.ageSecs;
    ageHrs = ageSecs / SECONDS_PER_HOUR;
    ageSecs %= SECONDS_PER_HOUR;
    ageMins = ageSecs / SECONDS_PER_MINUTE;
    ageSecs %= SECONDS_PER_MINUTE;
    if ((arpEntry.flags & (L7_ARP_LOCAL | L7_ARP_STATIC)) != 0)
    {
      osapiSnprintf(ageFmt, sizeof(ageFmt), pStrInfo_routing_NotApplicable_1);
    }
    else
    {
      osapiSnprintf(ageFmt, sizeof(ageFmt), "%2.2u:%2.2u:%2.2u", ageHrs, ageMins, ageSecs);
    }

    /* Copies all of the HTML data necessary to provide arp table statistics into usmWebBigBuf */
    if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID))
    {
        osapiSnprintf(usmWebBigBuf, sizeof(usmWebBigBuf),  "%s%s %s%s%s\n %02X:%02X:%02X:%02X:%02X:%02X\n  %s%s%d/%d/%d\n %s%s%s\n %s%s%s\n %s%s",
            "<tr>", "<td class=\"tabledata\">",
            ipbuf, "</td>", "<td class=\"tabledata\">",
            arpEntry.macAddr.addr.enetAddr.addr[0],
            arpEntry.macAddr.addr.enetAddr.addr[1],
            arpEntry.macAddr.addr.enetAddr.addr[2],
            arpEntry.macAddr.addr.enetAddr.addr[3],
            arpEntry.macAddr.addr.enetAddr.addr[4],
            arpEntry.macAddr.addr.enetAddr.addr[5],
            "</td>", "<td class=\"tabledata\">", unit, slot, port,
            "</td>", "<td class=\"tabledata\">", status,
            "</td>", "<td class=\"tabledata\">", ageFmt,
            "</td>", "</tr>");
    }
    else
    {
        osapiSnprintf(usmWebBigBuf, sizeof(usmWebBigBuf),  "%s%s %s%s%s\n %02X:%02X:%02X:%02X:%02X:%02X\n  %s%s%d/%d\n %s%s%s\n %s%s%s\n %s%s",
            "<tr>", "<td class=\"tabledata\">",
            ipbuf, "</td>", "<td class=\"tabledata\">",
            arpEntry.macAddr.addr.enetAddr.addr[0],
            arpEntry.macAddr.addr.enetAddr.addr[1],
            arpEntry.macAddr.addr.enetAddr.addr[2],
            arpEntry.macAddr.addr.enetAddr.addr[3],
            arpEntry.macAddr.addr.enetAddr.addr[4],
            arpEntry.macAddr.addr.enetAddr.addr[5],
            "</td>", "<td class=\"tabledata\">", slot, port,
            "</td>", "<td class=\"tabledata\">", status,
            "</td>", "<td class=\"tabledata\">", ageFmt,
            "</td>", "</tr>");
    }
  }
  return usmWebBigBuf;
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the arp_table_cfg form
            appInfo.data[2] - Dynamic Renew Mode
                              1. All Dynamic Entries -  appInfo.data[2] = 1
                              2. All Dynamic and Gateway Entries -  appInfo.data[2] = 2
                              3. Specific Dynamic/Gateway Entry -  appInfo.data[2] = 3
                              4. Specific Static Entry -  appInfo.data[2] = 4
                              5. None -  appInfo.data[2] = 5
            appInfo.data[3] - Selected item in Remove from Table "clear"
            appInfo.data[4] - Age Time
            appInfo.data[5] - Response Time
            appInfo.data[6] - Retries
            appInfo.data[7] - Cache Size
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_arp_table_cfg ( EwsContext context, EwaForm_arp_table_cfgP form)
{
  L7_uint32 val;
  L7_uint32 rc;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_arpCacheStats_t Stats;
  L7_uint32  unit, flag = 0;

  unit = usmDbThisUnitGet();

  net = ewsContextNetHandle(context);

  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    form->status.err_flag |= (EW_FORM_INITIALIZED);
    form->value.err_msg = osapiStrDup(appInfo.err.msg);
    form->status.err_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

    if (appInfo.data[1] == L7_TRUE)
    {

        /* Set all configurable data to reflect the values set by the user before an OnChange submit from item "clear" */

        form->value.dynmc_rnw = appInfo.data[2];
        form->status.dynmc_rnw |= (EW_FORM_INITIALIZED);

         form->value.agetime = appInfo.data[4];
         form->status.agetime |= (EW_FORM_INITIALIZED);

         form->value.resptime = appInfo.data[5];
         form->status.resptime |= (EW_FORM_INITIALIZED);

         form->value.retries = appInfo.data[6];
         form->status.retries |= (EW_FORM_INITIALIZED);

         form->value.cachesize = appInfo.data[7];
         form->status.cachesize |= (EW_FORM_INITIALIZED);

        if (appInfo.data[3] == 1)
      {
           form->value.clear = All_Dynmc;
      }
        else if (appInfo.data[3] == 2)
      {
           form->value.clear = All_Dynmc_Gtwy;
      }
        else if (appInfo.data[3] == 3)
      {
           form->value.clear = Specific_Dynmc_Gtwy;
      }
        else if (appInfo.data[3] == 4)
      {
           form->value.clear = Specific_Static;
      }
        else if (appInfo.data[3] == 5)
      {
        form->value.clear = None;
      }

      form->status.clear |= (EW_FORM_INITIALIZED);
    }
    else
    {flag = 1;}
  }

  if ((flag == 1) || (net->app_pointer == NULL))
    {

    if (net->app_pointer == NULL)
    {
         form->value.err_flag = L7_FALSE;
         form->status.err_flag |= (EW_FORM_INITIALIZED);
         form->value.err_msg = NULL;
         form->status.err_msg |= (EW_FORM_INITIALIZED);
    }

    form->value.clear = None;
    form->status.clear |= (EW_FORM_INITIALIZED);

    rc = usmDbIpArpDynamicRenewGet(unit, &val);
    form->value.dynmc_rnw = usmWebL7EnDisToEnDisGet(val, Enable);
    form->status.dynmc_rnw |= (EW_FORM_INITIALIZED);

     rc = usmDbIpArpAgeTimeGet(unit, &val);
     form->value.agetime = val;
     form->status.agetime |= (EW_FORM_INITIALIZED);

     rc = usmDbIpArpRespTimeGet(unit, &val);
     form->value.resptime = val;
     form->status.resptime |= (EW_FORM_INITIALIZED);

     rc = usmDbIpArpRetriesGet(unit, &val);
     form->value.retries = val;
     form->status.retries |= (EW_FORM_INITIALIZED);

     rc = usmDbIpArpCacheSizeGet(unit, &val);
     form->value.cachesize = val;
     form->status.cachesize |= (EW_FORM_INITIALIZED);

  }

  rc = usmDbIpArpCacheStatsGet(unit, &Stats);

  form->value.tot_entry_cnt = Stats.cacheCurrent;
  form->status.tot_entry_cnt |= (EW_FORM_INITIALIZED);

  form->value.peak_tot_entry = Stats.cachePeak;
  form->status.peak_tot_entry |= (EW_FORM_INITIALIZED);

  form->value.static_entry_cnt_active = Stats.staticCurrent;
  form->status.static_entry_cnt_active |= (EW_FORM_INITIALIZED);

  form->value.static_entry_cnt_cfg = usmDbIpMapStaticArpCount(unit);
  form->status.static_entry_cnt_cfg |= (EW_FORM_INITIALIZED);

  form->value.max_static_entry = Stats.staticMax;
  form->status.max_static_entry |= (EW_FORM_INITIALIZED);

  form->value.max_static_entry = Stats.staticMax;
  form->status.max_static_entry |= (EW_FORM_INITIALIZED);

  if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID))
  form->value.isStacking = 1;
  else
  form->value.isStacking = 0;

  form->status.isStacking |= (EW_FORM_INITIALIZED);

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
* @param form pointer to the arp_table_cfg form
            appInfo.data[2] - Dynamic Renew Mode
                              1. All Dynamic Entries -  appInfo.data[2] = 1
                              2. All Dynamic and Gateway Entries -  appInfo.data[2] = 2
                              3. Specific Dynamic/Gateway Entry -  appInfo.data[2] = 3
                              4. Specific Static Entry -  appInfo.data[2] = 4
                              5. None -  appInfo.data[2] = 5
            appInfo.data[3] - Selected item in Remove from Table "clear"
            appInfo.data[4] - Age Time
            appInfo.data[5] - Response Time
            appInfo.data[6] - Retries
            appInfo.data[7] - Cache Size

*
* @returns arp_table_cfg.html
*
* @end
*
*********************************************************************/
char *ewaFormSubmit_arp_table_cfg(EwsContext context, EwaForm_arp_table_cfgP form)
{
  L7_RC_t rc = L7_SUCCESS;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32  unit;
  L7_arpEntry_t pArp;
  unit = usmDbThisUnitGet();

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  appInfo.data[1] = L7_FALSE;

  if(form->status.Refresh & EW_FORM_RETURNED)
  {
     usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_routing_HtmlFileArpTblCfg);
  }

  if (form->status.Submit & EW_FORM_RETURNED)
  {

    /*********************** range checking **********************/
    if  (form->status.agetime & EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FieldInvalidFmt,
                              pStrInfo_routing_AgeTimeSecs);

    }
    else if (form->value.agetime > L7_IP_ARP_AGE_TIME_MAX ||
             form->value.agetime < L7_IP_ARP_AGE_TIME_MIN)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FieldOutOfRange,
                          (L7_int32) form->value.agetime,
                              pStrInfo_routing_AgeTimeSecs, /* Age Time (secs) */
                          L7_IP_ARP_AGE_TIME_MIN, L7_IP_ARP_AGE_TIME_MAX);
    }
    else
    {
      rc = usmDbIpArpAgeTimeSet(unit, form->value.agetime);
      if (rc != L7_SUCCESS)
        {
        usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_FailedToSet,  pStrErr_common_FailedToSet_1,
                                pStrInfo_routing_AgeTimeSecs);
        }
    }

    if (form->status.resptime & EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FieldInvalidFmt,
                              pStrInfo_common_RespTimeSecs);
    }
    else if (form->value.resptime < L7_IP_ARP_RESP_TIME_MIN ||
        form->value.resptime > L7_IP_ARP_RESP_TIME_MAX)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FieldOutOfRange,
                    (L7_int32) form->value.resptime,
                              pStrInfo_common_RespTimeSecs, /* Response Time (secs) */
                    L7_IP_ARP_RESP_TIME_MIN, L7_IP_ARP_RESP_TIME_MAX);
    }
    else
    {
      rc = usmDbIpArpRespTimeSet(unit, form->value.resptime);
      if (rc != L7_SUCCESS)
      {
        usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_FailedToSet,  pStrErr_common_FailedToSet_1,
                                pStrInfo_common_RespTimeSecs);
      }
    }

    if (form->status.retries & EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FieldInvalidFmt,
                              pStrInfo_routing_Retries_1);
    }
    else if (form->value.retries < L7_IP_ARP_RETRIES_MIN ||
        form->value.retries > L7_IP_ARP_RETRIES_MAX)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FieldOutOfRange,
                    (L7_int32) form->value.retries,
                              pStrInfo_routing_Retries_1, /* Retries */
                    L7_IP_ARP_RETRIES_MIN, L7_IP_ARP_RETRIES_MAX);
    }
    else
    {
      rc = usmDbIpArpRetriesSet(unit, form->value.retries);
      if (rc != L7_SUCCESS)
      {
        usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_FailedToSet,  pStrErr_common_FailedToSet_1,
                                pStrInfo_routing_Retries_1);
      }
    }

    if (form->status.cachesize  & EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FieldInvalidFmt,
                              pStrInfo_routing_CacheSize);
    }
    else if (form->value.cachesize  < L7_IP_ARP_CACHE_SIZE_MIN ||
        form->value.cachesize > platRtrArpMaxEntriesGet())
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FieldOutOfRange,
                              (L7_int32) form->value.cachesize, pStrInfo_routing_CacheSize,
                    L7_IP_ARP_CACHE_SIZE_MIN, platRtrArpMaxEntriesGet());
    }
    else
    {
      rc = usmDbIpArpCacheSizeSet(unit, form->value.cachesize);

      /* ARP cache size could not be set.\n
        Note that the ARP cache size must be at least twice the
        number of static ARP entries in the cache. */
      if (rc != L7_SUCCESS)
      {
        usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_FailedToSet,  pStrErr_common_FailedToSet_1,
                                pStrInfo_routing_CacheSize);
      }
    }

    if  (form->status.dynmc_rnw & EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FieldInvalidFmt,
                              pStrInfo_routing_DynRenew);
    }
    else
    {
      switch (form->value.dynmc_rnw)
      {
      case Enable:
        rc = usmDbIpArpDynamicRenewSet(unit,L7_ENABLE);
        break;
      case Disable:
        rc = usmDbIpArpDynamicRenewSet(unit,L7_DISABLE);
        break;
      default:
        usmWebAppInfoCatErrMsg (&appInfo, L7_NULLPTR, pStrInfo_routing_DynRenewModeInvalid);
        break;
      }

      if (rc != L7_SUCCESS)
        {
        usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_FailedToSet,  pStrErr_common_FailedToSet_1,
                                pStrInfo_routing_DynRenew);
        }
    }

    if  (form->status.clear & EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FieldInvalidFmt,
                              pStrInfo_routing_RemoveFromTbl);

    }
    else
    {
      if (form->value.clear != None)
      {

          if (form->value.clear == All_Dynmc)
          {
            rc = usmDbIpArpCacheClear(unit, L7_FALSE);
            if (rc != L7_SUCCESS)
             {
            usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_routing_FailedTo,
                                    pStrInfo_routing_RemoveFromTbl);
          }
        }

          if (form->value.clear == All_Dynmc_Gtwy)
          {
            rc = usmDbIpArpCacheClear(unit, L7_TRUE);
            if (rc != L7_SUCCESS)
             {
            usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_routing_FailedTo,
                                    pStrInfo_routing_RemoveFromTbl);
          }
        }

          if (form->value.clear == Specific_Dynmc_Gtwy)
          {
              if    (form->status.clr_ipaddr & EW_FORM_PARSE_ERROR)
              {
            usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FieldInvalidFmt,
                                    pStrInfo_routing_RemoveIpAddr);

              }
              else
              {

                 /* Check whether the specified address coresponds to a Dynamic or Gateway ARP Entry*/
                 if (usmDbIpArpEntryGet(unit, form->value.clr_ipaddr, L7_INVALID_INTF, &pArp) != L7_SUCCESS)
                  {
              usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrInfo_routing_InSpecifyingIpAddrToBeRemovedFromArpCache);

                   }

                  if (((pArp.flags & L7_ARP_LOCAL)==0x01) || ((pArp.flags & L7_ARP_STATIC)==0x20))
                   {
              usmWebAppInfoCatErrMsg (&appInfo, L7_NULLPTR,  pStrErr_routing_IpAddrAddrOfDynGatewayEntryDesired);

                   }
                rc = usmDbIpArpEntryPurge(unit, form->value.clr_ipaddr, L7_INVALID_INTF);
                if (rc != L7_SUCCESS)
                 {
              usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_routing_FailedTo,
                                      pStrInfo_routing_RemoveIpAddr);
            }

          }
        }

          if (form->value.clear == Specific_Static)
          {
              /* Verify that the specified IP Address is valid */
            if (form->status.clr_ipaddr & EW_FORM_PARSE_ERROR)
            {
            usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FieldDottedIp,
                                    pStrInfo_common_ApIpAddr);
            }
            if (appInfo.err.err_flag == L7_FALSE)
            {
               rc = usmDbIpMapStaticArpDelete(unit, form->value.clr_ipaddr, L7_NULL);
            }
            if (rc != L7_SUCCESS)
            {
            usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error,  pStrErr_common_FailedToDel,
                                    pStrInfo_common_Arp);
          }
        }

      }
    }

    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_routing_HtmlFileArpTblCfg);
  }
  else if(form->value.clear != None)
  {

      /* Submit from an OnChange Request fired on item "clear */

      if (form->value.clear == All_Dynmc)
    {
          appInfo.data[3] = 1;
    }
      else if (form->value.clear == All_Dynmc_Gtwy)
    {
          appInfo.data[3] = 2;
    }
      else if (form->value.clear == Specific_Dynmc_Gtwy)
    {
          appInfo.data[3] = 3;
    }
      else if (form->value.clear == Specific_Static)
    {
          appInfo.data[3] = 4;
    }
      else if (form->value.clear == None)
    {
          appInfo.data[3] = 5;
    }

      appInfo.data[1] = L7_TRUE;

      appInfo.data[2] = form->value.dynmc_rnw;

      appInfo.data[4] = form->value.agetime;

      appInfo.data[5] = form->value.resptime;

      appInfo.data[6] = form->value.retries;

      appInfo.data[7] = form->value.cachesize;

    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_routing_HtmlFileArpTblCfg);

  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID, "# (arp_table_cfg.html) Form Submission Failed. "
                "No Action Taken. The form submission failed and no action is taken. # (arp_table_cfg.html) "
                "indicates the file under consideration.\n");
    return L7_NULL;
  }
}


/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @returns none
*
* @end
*
*********************************************************************/

void ewaFormServe_arp_table_cfgro ( EwsContext context, EwaForm_arp_table_cfgroP form)
{

  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32  unit;

  unit = usmDbThisUnitGet();

  net = ewsContextNetHandle(context);

  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    form->status.err_flag |= (EW_FORM_INITIALIZED);
    form->value.err_msg = osapiStrDup(appInfo.err.msg);
    form->status.err_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  if (net->app_pointer == NULL)
  {
         form->value.err_flag = L7_FALSE;
         form->status.err_flag |= (EW_FORM_INITIALIZED);
         form->value.err_msg = NULL;
         form->status.err_msg |= (EW_FORM_INITIALIZED);
  }

  if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID))
  form->value.isStacking = 1;
  else
  form->value.isStacking = 0;

  form->status.isStacking |= (EW_FORM_INITIALIZED);

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
* @returns arp_table_cfg.html
*
* @end
*
*********************************************************************/
char *ewaFormSubmit_arp_table_cfgro(EwsContext context, EwaForm_arp_table_cfgroP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32  unit;

  unit = usmDbThisUnitGet();

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

     usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_routing_HtmlFileArpTblCfg);
     return L7_NULL;
}
