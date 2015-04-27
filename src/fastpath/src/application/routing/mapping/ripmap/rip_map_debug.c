/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    rip_debug.c
* @purpose     RIP Debug functions
* @component   rip
* @comments    none
* @create      08/02/2006
* @author      ssuvarna
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/
#include "l7_common.h"
#include "usmdb_util_api.h"
#include "osapi.h"

#include "log.h"

#include "l7_ripinclude.h"
#include "l7_rip_debug_api.h"
#include "rip_util.h"
#include "support_api.h"

#define RIP_REQUEST 1
#define RIP_RESPONSE 2

#define RIPMAP_DEBUG_PACKET_RX_FORMAT "\r\nPkt RX on Intf: %d/%d/%d(%d,%s), Src_IP:%s Dest_IP:%s\r\nRip_Version:%s Packet_Type:%s\r\n%s"
#define RIPMAP_DEBUG_PACKET_TX_FORMAT "\r\nPkt TX on Intf: %d/%d/%d(%d,%s), Src_IP:%s Dest_IP:%s\r\nRip_Version:%s Packet_Type:%s\r\n%s"

#define DISPLAY_ROUTE_NO 5

#define RIPMAP_USER_TRACE(__fmt__, __args__... )                              \
          if (ripMapDebugPacketTraceFlag == L7_TRUE)                          \
          {                                                                 \
            LOG_USER_TRACE(L7_RIP_MAP_COMPONENT_ID, __fmt__,##__args__);        \
          }

static L7_BOOL ripMapDebugPacketTraceFlag = L7_FALSE;

extern ripMapDebugCfg_t ripMapDebugCfg;

/*********************************************************************
*
* @purpose Trace rip packets received
*
* @param   rtrIfNum @b{(input)} Internal Interface Number
* @param   *buff    @b{(input)} pointer to packet
* @param   cc   @b{(input)} packet size
* @param   *srcIp   @b{(input)} Source Ip Address
* @param   *destIp  @b{(input)} Dest IP Address
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/

void ripMapDebugPacketRxTrace(L7_uint32 rtrIfNum,L7_uchar8 *buff, L7_int32 cc, L7_uchar8 *srcIp, L7_uchar8 *destIp)
{
  struct rip *rip_pkt = (struct rip*) buff;
  struct netinfo *n;
  struct netinfo *lim;
  L7_uchar8 route[100];
  L7_uchar8 all_routes[600]= "";
  int no_routes = 0;
  L7_char8 dest[20];
  L7_char8 mask[20];
  L7_char8 nexthop[25];
  L7_char8 vers[10] = " ";
  L7_char8 msg_type[20] = " ";
  L7_uint32 unit, slot, port;
  L7_uint32 intIfNum;

  if(ripMapDebugPacketTraceFlag != L7_TRUE)
      return;

  n = rip_pkt->rip_nets;
  lim = (struct netinfo *)((char*)rip_pkt + cc);

  if(rip_pkt->rip_vers == RIPv1)
      osapiSnprintf(vers,sizeof(vers),"RIPv1");
  else if(rip_pkt->rip_vers == RIPv2)
      osapiSnprintf(vers,sizeof(vers),"RIPv2");

  if(rip_pkt->rip_cmd == RIPCMD_REQUEST)
      osapiSnprintf(msg_type,sizeof(msg_type),"RIP_REQUEST");
  else if(rip_pkt->rip_cmd == RIPCMD_RESPONSE)
      osapiSnprintf(msg_type,sizeof(msg_type),"RIP_RESPONSE");
  else return;

  do{
      osapiInetNtoa(n->n_dst,dest);
      osapiInetNtoa(n->n_mask,mask);
      osapiInetNtoa(n->n_nhop,nexthop);

      if(no_routes < DISPLAY_ROUTE_NO)
      {
          if(n->n_nhop == 0)
              osapiSnprintf(route,sizeof(route),"ROUTE %d): Network:%s Mask:%s Metric:%d\n",no_routes,
                            dest,mask,n->n_metric);
          else
              osapiSnprintf(route,sizeof(route),"ROUTE %d): Network:%s Mask:%s Next_Hop:%s Metric:%d\n",no_routes
                            ,dest,mask,nexthop,n->n_metric);

          osapiStrncat(all_routes,route,sizeof(route));
      }
      ++no_routes;
  }while((++n < lim));

  if(no_routes > DISPLAY_ROUTE_NO)
     {
       osapiSnprintf(route,sizeof(route),
                    "No of additional Routes present but not printed: %d\n",(no_routes - DISPLAY_ROUTE_NO));
       osapiStrncat(all_routes,route,sizeof(route));
     }

  if (ipMapRtrIntfToIntIfNum(rtrIfNum, &intIfNum) == L7_SUCCESS)
      if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
      {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

          RIPMAP_USER_TRACE(RIPMAP_DEBUG_PACKET_RX_FORMAT,unit,slot,port,intIfNum,ifName,srcIp,destIp,
                            vers,msg_type,all_routes);
      }
  return;
}
/*********************************************************************
*
* @purpose Trace rip packets transmitted
*
* @param   rtrIfNum @b{(input)} Internal Interface Number
* @param   *buff    @b{(input)} pointer to packet
* @param   cc   @b{(input)} packet size
* @param   *srcIp   @b{(input)} Source Ip Address
* @param   *destIp  @b{(input)} Dest IP Address
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void ripMapDebugPacketTxTrace(L7_uint32 rtrIfNum,L7_uchar8 *buff, L7_int32 cc, L7_uchar8 *srcIp, L7_uchar8 *destIp)
{
  struct rip *rip_pkt = (struct rip*) buff;
  struct netinfo *n;
  struct netinfo *lim;
  L7_uchar8 route[100];
  L7_uchar8 all_routes[600]= "";
  int no_routes = 0;
  L7_char8 dest[20];
  L7_char8 mask[20];
  L7_char8 nexthop[25];
  L7_char8 vers[10] = " ";
  L7_char8 msg_type[20] = " ";
  L7_uint32 unit, slot, port;
  L7_uint32 intIfNum;

  if(ripMapDebugPacketTraceFlag != L7_TRUE)
      return;

  n = rip_pkt->rip_nets;
  lim = (struct netinfo *)((char*)rip_pkt + cc);

  if(rip_pkt->rip_vers == RIPv1)
      osapiSnprintf(vers,sizeof(vers),"RIPv1");
  else if(rip_pkt->rip_vers == RIPv2)
      osapiSnprintf(vers,sizeof(vers),"RIPv2");

  if(rip_pkt->rip_cmd == RIPCMD_REQUEST)
      osapiSnprintf(msg_type,sizeof(msg_type),"RIP_REQUEST");
  else if(rip_pkt->rip_cmd == RIPCMD_RESPONSE)
      osapiSnprintf(msg_type,sizeof(msg_type),"RIP_RESPONSE");
  else return;

  do{
      osapiInetNtoa(n->n_dst,dest);
      osapiInetNtoa(n->n_mask,mask);
      osapiInetNtoa(n->n_nhop,nexthop);

      if(no_routes < DISPLAY_ROUTE_NO)
      {
          if(n->n_nhop == 0)
              osapiSnprintf(route,sizeof(route),"ROUTE %d): Network:%s Mask:%s Metric:%d\n",no_routes,
                            dest,mask,n->n_metric);
          else
              osapiSnprintf(route,sizeof(route),"ROUTE %d): Network:%s Mask:%s Next_Hop:%s Metric:%d\n",no_routes
                            ,dest,mask,nexthop,n->n_metric);

          osapiStrncat(all_routes,route,sizeof(route));
      }
      ++no_routes;
  }while((++n < lim));

  if(no_routes > DISPLAY_ROUTE_NO)
     {
       osapiSnprintf(route,sizeof(route),
                    "No of additional Routes present but not printed: %d\n",(no_routes - DISPLAY_ROUTE_NO));
       osapiStrncat(all_routes,route,sizeof(route));
     }

  if (ipMapRtrIntfToIntIfNum(rtrIfNum, &intIfNum) == L7_SUCCESS)
      if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
      {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

          RIPMAP_USER_TRACE(RIPMAP_DEBUG_PACKET_TX_FORMAT,unit,slot,port,intIfNum,ifName,srcIp,destIp,
                            vers,msg_type,all_routes);
      }

  return;
}


/*********************************************************************
* @purpose  Get the current status of displaying rip packet debug info
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL ripMapDebugPacketTraceFlagGet()
{
  return ripMapDebugPacketTraceFlag;
}


/*********************************************************************
* @purpose  Turns on/off the displaying of rip packet debug info
*
* @param    flag         new value of the Packet Debug flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ripMapDebugPacketTraceFlagSet(L7_BOOL flag)
{
  ripMapDebugPacketTraceFlag = flag;

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a
     debug config save.
    */

  ripMapDebugCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Save configuration settings for rip trace data
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ripMapDebugPacketTraceFlagSave()
{
  ripMapDebugCfg.cfg.ripMapDebugPacketTraceFlag = ripMapDebugPacketTraceFlag;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Copy the trace settings to the debug config file
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ripMapDebugCfgUpdate(void)
{
  ripMapDebugPacketTraceFlagSave();
}

/*********************************************************************
* @purpose  Read and apply the debug config
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ripMapDebugCfgRead(void)
{
      /* reset the debug flags*/
    memset((void*)&ripMapDebugCfg, 0 ,sizeof(ripMapDebugCfg_t));

    (void)sysapiSupportCfgFileGet(L7_RIP_MAP_COMPONENT_ID, RIPMAP_DEBUG_CFG_FILENAME,
                         (L7_char8 *)&ripMapDebugCfg, (L7_uint32)sizeof(ripMapDebugCfg_t),
                         &ripMapDebugCfg.checkSum, RIPMAP_DEBUG_CFG_VER_CURRENT,
                         ripMapDebugBuildDefaultConfigData, L7_NULL);

    ripMapDebugCfg.hdr.dataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Register to general debug infrastructure
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ripMapDebugRegister(void)
{
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_RIP_MAP_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = ripMapDebugSave;
    supportDebugDescr.userControl.hasDataChanged = ripMapDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = ripMapDebugRestore;

    /* Register Internal Parms */

    /* Submit registration */

    (void) supportDebugRegister(supportDebugDescr);
}

