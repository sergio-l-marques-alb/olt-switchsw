/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/routing/routing/route_table_good.c
 *
 * @purpose Code in support of the route_table_good.html page
 *
 * @component unitmgr
 *
 * @comments tba
 *
 * @create 06/15/2001
 *
 * @author betsyt
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_routing_common.h"
#include "strlib_routing_web.h"
#include "l7_common.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"
#include <stdio.h>
#include <string.h>
#include "ew_proto.h"
#include "ewnet.h"
#include "l3end_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_mib_ospf_api.h"
#include "l7_common_l3.h"

#include "log.h"
#include "web.h"
#include "web_oem.h"

static L7_char8 usmWebBigBuf[1024];
static L7_char8 usmWebBuffer[256];
L7_uint32 best_routes = 0;

/*********************************************************************
*
* @purpose Get the Route Table Info
*
* @param unit  Unit Number
*
* @returns usmWebBigBuf with the Route Table Info
*
* @end
*
*********************************************************************/
L7_char8 *usmWebGoodRouteTableInfoGet(L7_uint32 unit, L7_routeEntry_t *routeEntry)
{
  L7_RC_t rc;
  L7_char8 stat[80];
  L7_char8 net[USMWEB_IPADDR_SIZE];
  L7_char8 mask[USMWEB_IPADDR_SIZE];
  L7_char8 protocol[80];
  L7_char8 route[USMWEB_IPADDR_SIZE];
  L7_uint32 u, s, p;
  L7_uint32 nextHopCounter;
  L7_char8 nextHopIp[USMWEB_IPADDR_SIZE];

  memset(usmWebBigBuf, 0, sizeof(usmWebBigBuf));
  memset(stat, 0, sizeof(stat));
  memset(net, 0, USMWEB_IPADDR_SIZE);
  memset(mask, 0, USMWEB_IPADDR_SIZE);
  memset(protocol, 0, sizeof(protocol));
  memset(route, 0, USMWEB_IPADDR_SIZE);
  memset(nextHopIp, 0, USMWEB_IPADDR_SIZE);
  best_routes = best_routes + 1;
  rc = usmDbInetNtoa(routeEntry->ipAddr, net);
  rc = usmDbInetNtoa(routeEntry->subnetMask, mask);
  switch (routeEntry->protocol)
  {
  case RTO_LOCAL:
    osapiSnprintf(protocol, sizeof(protocol),  "%s ", pStrInfo_common_WsNwLocal);
    break;
  case RTO_STATIC:
    osapiSnprintf(protocol, sizeof(protocol),  "%s ", pStrInfo_common_PimSmGrpRpMapStatic);
    break;
  case RTO_MPLS:
    osapiSnprintf(protocol, sizeof(protocol),  "%s ", pStrInfo_common_Mpls);
    break;
  case RTO_OSPF_INTRA_AREA:
    osapiSnprintf(protocol, sizeof(protocol),  "%s ", pStrInfo_common_OspfIntra);
    break;
  case RTO_OSPF_INTER_AREA:
    osapiSnprintf(protocol, sizeof(protocol),  "%s ", pStrInfo_common_OspfInter);
    break;
  case RTO_OSPF_TYPE1_EXT:
    osapiSnprintf(protocol, sizeof(protocol),  "%s ", pStrInfo_common_OspfExternal);
    break;
  case RTO_OSPF_TYPE2_EXT:
    osapiSnprintf(protocol, sizeof(protocol),  "%s ", pStrInfo_common_OspfExternal);
    break;
  case RTO_OSPF_NSSA_TYPE1_EXT:
    osapiSnprintf(protocol, sizeof(protocol),  "%s ", pStrInfo_routing_OspfNssaType1);
    break;
  case RTO_OSPF_NSSA_TYPE2_EXT:
    osapiSnprintf(protocol, sizeof(protocol),  "%s ", pStrInfo_routing_OspfNssaType2);
    break;
  case RTO_RIP:
    osapiSnprintf(protocol, sizeof(protocol),  "%s ", pStrInfo_common_Rip);
    break;
  case RTO_DEFAULT:
    osapiSnprintf(protocol, sizeof(protocol),  "%s ", pStrInfo_common_Defl);
    break;
  case RTO_IBGP:
  case RTO_EBGP:
    osapiSnprintf(protocol, sizeof(protocol),  "%s ", pStrInfo_common_Bgp4);
    break;
  default:
    osapiSnprintf(protocol, sizeof(protocol),  "%s ", pStrInfo_common_NotApplicable);
    break;
  }

  /* ROBRICE - need to display all next hops */
#if 0
  if (usmDbUnitSlotPortGet(routeTable.intIfNum, &u, &s, &p) == L7_SUCCESS)
  {
    if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID))
    {
      osapiSnprintf(stat, sizeof(stat), "%d/%d/%d", u, s, p);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%d/%d", s, p);
    }
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Invalid_1);                                            /* WPJ_REVIEW:  Use NLS string */
  }

  rc = usmDbInetNtoa(routeTable.router, route);
#endif

for( nextHopCounter = 0; nextHopCounter < routeEntry->ecmpRoutes.numOfRoutes; nextHopCounter++ )
  {
     if (usmDbUnitSlotPortGet(routeEntry->ecmpRoutes.equalCostPath[nextHopCounter].arpEntry.intIfNum, &u, &s, &p) == L7_SUCCESS)
     {
         if(usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID))
         {
             sprintf(stat,"%d/%d/%d", u, s, p);
         }
         else
         {
             sprintf(stat,"%d/%d", s, p);
         }

         rc = usmDbInetNtoa(routeEntry->ecmpRoutes.equalCostPath[nextHopCounter].arpEntry.ipAddr, nextHopIp);  /* ECMP*/

         if(nextHopCounter != 0)
         {   /* If a Next Hop is present, then pad the table entries for net, mask and protocol entries*/
             sprintf(usmWebBigBuf, "%s\n %s\n %s%s%s\n  %s%s %s\n %s%s %s\n%s %s %s\n%s%s %s\n %s\n",
                      "<TR>",
                      usmWebBigBuf,
                      "<td CLASS=\"tabledata\">"," ", "</td>",
                      "<td CLASS=\"tabledata\">"," ", "</TD>",
                      "<td CLASS=\"tabledata\">"," ","</td>",
                      "<td CLASS=\"tabledata\">",stat,"</td>",
                      "<td CLASS=\"tabledata\">",nextHopIp,"</td>",
                      "</TR>");
         }
         else
         {
             sprintf(usmWebBigBuf, "%s\n %s\n %s%s%s\n  %s%s %s\n %s%s %s\n%s %s %s\n%s%s %s\n %s\n",
                     "<TR>",
                     usmWebBigBuf,
                     "<td CLASS=\"tabledata\">",net, "</td>",
                     "<td CLASS=\"tabledata\">",mask, "</TD>",
                     "<td CLASS=\"tabledata\">",protocol,"</td>",
                     "<td CLASS=\"tabledata\">",stat,"</td>",
                     "<td CLASS=\"tabledata\">",nextHopIp,"</td>",
                     "</TR>");
         }
     }
     else
     {
         continue;
     }
  }
  return usmWebBigBuf;
}

/*********************************************************************
*
* @purpose Get the Number of Routes
*
* @param unit  Unit Number
*
* @returns usmWebBigBuf with the number of routes
*
* @end
*
*********************************************************************/
L7_char8 *usmWebNumGoodRoutesGet()
{
  L7_uint32 unit = usmDbThisUnitGet();
  L7_uint32 route_count = usmDbRouteCount(unit, L7_TRUE);  /* best only */;
  osapiSnprintf(usmWebBuffer, sizeof(usmWebBuffer),  "%u", route_count);
  return usmWebBuffer;
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the route_table_good form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_route_table_good(EwsContext context, EwaForm_route_table_goodP form)
{
  best_routes = 0;
  form->value.Refresh = (char *) osapiStrDup( pStrInfo_common_Refresh );
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the route_table_good form
*
* @returns route_table_good.html
*
* @end
*
*********************************************************************/
char *ewaFormSubmit_route_table_good ( EwsContext context, EwaForm_route_table_goodP form )
{

  if (form->status.Refresh & EW_FORM_RETURNED)
  {
    best_routes = 0;
    ewsContextSendReply(context, pStrInfo_routing_HtmlFileRouteTblGood);
    return NULL;
  }
  else
  {
	L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID, "# (route_table_good.html) Form Submission Failed. "
				"No Action Taken. The form submission failed and no action is taken. # (route_table_good.html) "
				"indicates the file under consideration.\n");
    return NULL;
  }

}
