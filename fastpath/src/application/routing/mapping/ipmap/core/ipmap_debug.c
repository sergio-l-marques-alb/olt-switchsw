/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  ipmap_debug.c
*
* @purpose   IP Mapping Debug functions
*
* @component IP Mapping Layer
*
* @comments  none
*
* @create    06/21/2001
*
* @author    wjacobs
*
* @end
*
**********************************************************************/

#define L7_MAP_ROUTE_PREFERENCES 1

#include "l7_common.h"
#include "l7_ipinclude.h"
#include "osapi_support.h"
#include "nimapi.h"
#include "l7_ip_api.h"
#include "l7_ospf_api.h"
#include "l7_rip_api.h"
#include "osapi.h"
#include "ipmap_sid.h"
#include "bspapi.h"
#include "portevent_mask.h"
#include "ipmap_arp_exten.h"

#ifdef L7_QOS_FLEX_PACKAGE_ACL
#include "acl_api.h"
#endif

#include "l7_icmp.h"
#include "support_api.h"


/* For easier debugging, name the router events. */
L7_uchar8 *ipMapRouterEventNames[L7_LAST_RTR_STATE_CHANGE] =
{
    "Reserved", "Router Enable", "Router Disable Pending", "Router Disable", 
    "Interface Create", "Interface Enable", "Interface Disable Pending", "Interface Disable",
    "Interface Delete", "Address Add", "Address Delete Pending", "Address Delete",
    "Secondary Address Add", "Secondary Address Delete", 
    "IP MTU Change", "Bandwidth Change", "Speed Change", "Startup Events Done",
    "Host Interface"
};

/* And names of message types received on the IP MAP processing task */
L7_uchar8 *ipMapProcMessageNames[IPMAP_LAST_MSG] =
{
    "Undefined", "PDU", "ROUTER ID", "RESTORE", "IP ADDR", "CNFGR", "INTF CHANGE",
    "NIM STARTUP", "STALE_ROUTE_TIMER", "CFG COMPLETE", "ASYNC EVENT COMPLETE", 
    "RTO CHANGE", "ARP GW"
};


extern L7_rtrIntfMap_t          rtrIntfMap[L7_RTR_MAX_RTR_INTERFACES+1];
extern L7_uint32                intIfNumToRtrIntf[L7_MAX_INTERFACE_COUNT+1];
extern ipMapInfo_t             *pIpMapInfo;
extern ipMapLockInfo_t *ipMapLockInfo;
extern L7_int32                L7_ip_forwarding_task_id;
extern ipMapIntfInfo_t     *ipMapIntfInfo;
extern ipMapCnfgrState_t    ipMapCnfgrState;

/* bit mask indicating the items of interest for debug tracing. Bits are
 * indexed using ipMapTraceFlag_t.*/
L7_uint32 ipMapTraceFlags = 0x0;    

/* If non-zero, limits packet tracing to the specified interface. */
L7_uint32 ipMapTraceIntf = 0;

/* ICMP Rate limiting Stats */
ipMapDebugIcmpCtlStats_t icmpCtlStats;
L7_BOOL ratelimitKicks = L7_FALSE;

extern L7_int32 ipMapForwardingTaskId;
extern L7_int32 ipMapProcessingTaskId;

/*---------------------------------------*/
/* Global Configuration Data Structures  */
/*---------------------------------------*/
extern L7_ipMapCfg_t            *ipMapCfg;
extern L7_rtrStaticRouteCfg_t   *route;
extern L7_rtrStaticArpCfg_t     *arp;


extern void ipMapForwardQueueInfoShow(void);
extern void ipForwardStatsShow(void);
extern void rtipStatsShow(void);
extern void rtipIcmpStatsShow(void);
extern void IpRxStatsShowIcmp(void);
extern void IpRxStatsShowUdp(void);
extern void IpRxStatsShowTcp(void);
extern void IpRxStatsShowVrrp(void);

void layer3DebugMenu(void);
void ipMapDebugHelp(void);
void ipMapDebugRoutingInfoDump(void);
void ipMapCfgDataShow(void);
void ipMapCfgSwitchShow(void);
void ipMapCfgIntfShow(L7_uint32 unit, L7_uint32 slot, L7_uint32 port);
void rtrIntfShow(L7_uint32 unit, L7_uint32 slot, L7_uint32 port);
void ipMapPendingArpListShow(void);
void ipMapStaticArpShow(void);
void ipMapStaticRouteShow(void);
void ipIntfBuildTestConfigData(nimConfigID_t *configId, L7_rtrCfgCkt_t *pCfg);
void ipMapTraceModeSet(L7_uint32 mode);


#ifdef L7_QOS_FLEX_PACKAGE_ACL

#define IPMAP_DEBUG_PACKET_RX_FORMAT "Pkt RX - Intf: %d/%d/%d(%d),vlan: %d, ver/hlen %#4.2x, tos: %d, len: %d, id: %d, flags/offset: %#4.2x, ttl: %d, proto: %d, src: %s, dst: %s"
#define IPMAP_DEBUG_PACKET_TCPUDP_RX_FORMAT "Pkt RX - Intf: %d/%d/%d(%d),vlan: %d, ver/hlen %#4.2x, tos: %d, len: %d, id: %d, flags/offset: %#4.2x, ttl: %d, proto: %d, src: %s, dst: %s, srcPort: %d, dstPort: %d"
#define IPMAP_DEBUG_PACKET_ICMP_RX_FORMAT "Pkt RX - Intf: %d/%d/%d(%d),vlan: %d, ver/hlen %#4.2x, tos: %d, len: %d, id: %d, flags/offset: %#4.2x, ttl: %d, proto: %d, src: %s, dst: %s, icmp-type: %d, icmp-code: %d" 
#define IPMAP_DEBUG_PACKET_IGMP_RX_FORMAT "Pkt RX - Intf: %d/%d/%d(%d),vlan: %d, ver/hlen %#4.2x, tos: %d, len: %d, id: %d, flags/offset: %#4.2x, ttl: %d, proto: %d, src: %s, dst: %s, igmpType: %d, igmpGroupAddrs: %s"
#define IPMAP_DEBUG_PACKET_TX_FORMAT "Pkt TX - Intf: %d/%d/%d(%d), vlan: %d, ver/hlen %#4.2x, tos: %d, len: %d id: %d, flags/offset: %#4.2x, ttl: %d, proto: %d, src: %s, dst: %s"

static L7_uint32 aclNumTrace[L7_MAX_ACL_ID];
extern ipMapDebugCfg_t ipMapDebugCfg;

#endif

/*********************************************************************
* @purpose  Enable linking of IP Mapping layer debug functions
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ipMapTraceLinkerAssist(void)
{
  L7_uint32 traceMode = ipMapTraceModeGet();
  ipMapTraceModeSet(traceMode);
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
void ipMapDebugRegister(void)
{ 
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_IP_MAP_COMPONENT_ID;

    /* Register User Control Parms */
    #ifdef L7_QOS_FLEX_PACKAGE_ACL
    supportDebugDescr.userControl.notifySave = ipMapDebugSave;
    supportDebugDescr.userControl.hasDataChanged = ipMapDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = ipMapDebugRestore;
    #endif

    /* Register Internal Parms */
    supportDebugDescr.internal.debugHelp = ipMapDebugHelp;
    strncpy(supportDebugDescr.internal.debugHelpRoutineName, 
            "ipMapDebugHelp",SUPPORT_DEBUG_HELP_NAME_SIZE);
    strncpy(supportDebugDescr.internal.debugHelpDescr, 
            "Help Menu for Routing",SUPPORT_DEBUG_HELP_DESCR_SIZE);

    (void) supportDebugRegister(supportDebugDescr);
}

/*********************************************************************
* @purpose  debug function to dump display the layer3 debug menu
*           on the screen
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void layer3DebugMenu(void)
{
  L7_uint32 unit, slot, port, areaId;
  L7_uchar8 choice;
  L7_BOOL   virtFlag;

  while (1)
  {
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n+-----------------------------------------------------+");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n|                Layer3 Debug Menu                    |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n+-----------------------------------------------------+");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n| 1. Display IP config data                           |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n| 2. Display OSPF config data                         |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n| 3. Display RIP config data                          |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n| 4. Display OSPF RTO structure                       |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n| 5. Display OSPF IFO structure                       |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n| 6. Display OSPF Virtual IFO structure               |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n| 7. Display OSPF ARO structure                       |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n| 8. Display OSPF NBO structure                       |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n| Q. Quit                                             |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n+-----------------------------------------------------+");

     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nMake a selection: ");
     fflush (stdout);
     osapiFileRead(bspapiConsoleFdGet(), &choice, sizeof(choice));
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n");
     switch (choice)
     {
     case '1':
       ipMapCfgDataShow();
       break;
     case '2':
       if(ospfFuncTable.ospfMapCfgDataShow)
       {
         ospfFuncTable.ospfMapCfgDataShow();
       }
       else
       {
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "OSPF Component not present!\n");
       }
       break;
     case '3':
       if(ripFuncTable.ripMapCfgDataShow)
       {
         ripFuncTable.ripMapCfgDataShow();
       }
       else
       {
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "RIP Component not present!\n");
       }
       break;
     case '4':
       if(ospfFuncTable.ospfMapExtenRTOShow)
       {
         ospfFuncTable.ospfMapExtenRTOShow();
       }
       else
       {
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "OSPF Component not present!\n");
       }
       break;
     case '5':
     case '6':
       if(ospfFuncTable.ospfMapExtenIFOShow)
       {
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nunit: ");
         unit = osapiGetChoice();
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nslot: ");
         slot = osapiGetChoice();
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "%u \nport: ", slot);
         port = osapiGetChoice();
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "%u \n", port);
         virtFlag = (choice == '6') ? L7_TRUE : L7_FALSE;
         ospfFuncTable.ospfMapExtenIFOShow(unit, slot, port, virtFlag);
       }
       else
       {
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "OSPF Component not present!\n");
       }
       break;
     case '7':  
       if(ospfFuncTable.ospfMapExtenAROShow)
       { 
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\narea ID: ");
         areaId = osapiGetChoice();
         ospfFuncTable.ospfMapExtenAROShow(areaId);
       }
       else
       {
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "OSPF Component not present!\n");
       }
       break;
     case '8':
       if(ospfFuncTable.ospfMapExtenNBOShow)
       {
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nunit: ");
         unit = osapiGetChoice();
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nslot: ");
         slot = osapiGetChoice();
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nport: ");
         port = osapiGetChoice();
         ospfFuncTable.ospfMapExtenNBOShow(unit,slot,port);
       }
       else
       {
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "OSPF Component not present!\n");
       }
       break;
     case 'q':
     case 'Q':
       return;
       break;
     default:
       SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nPlease enter a valid menu choice.");
       break;
     }
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n\n");
  }
}

/*********************************************************************
*
* @purpose Display help menu for component ip map
*
* @param   void
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
void ipMapDebugHelp(void)
{
  /* Routine provides conversion to standard naming convention */
  layer3DebugMenu();
}

/*********************************************************************
* @purpose  Registered support debug dump routine for general routing issues
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes 
*                                 
* @end
*********************************************************************/
void ipMapDebugRoutingInfoDumpRegister(void)
{
    supportDebugCategory_t supportDebugCategory;

    memset(&supportDebugCategory, 0x00, sizeof(supportDebugCategory));
    supportDebugCategory.componentId = L7_IP_MAP_COMPONENT_ID;


    /*---------------------------------*/
    /* Register routing pkg debug dump */
    /*---------------------------------*/
    supportDebugCategory.category = SUPPORT_CATEGORY_ROUTING;
    supportDebugCategory.info.supportRoutine = ipMapDebugRoutingInfoDump;
    (void) strncpy(supportDebugCategory.info.supportRoutineName, "ipMapDebugRoutingInfoDump",
            SUPPORT_DEBUG_HELP_NAME_SIZE);
    (void) supportDebugCategoryRegister(supportDebugCategory);
}

/*********************************************************************
* @purpose  Registered support debug dump routine for general routing issues
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes 
*                                 
* @end
*********************************************************************/
void ipMapDebugRoutingInfoDump(void)
{
    sysapiPrintf("\r\n");
    sysapiPrintf("/*=====================================================================*/\n");
    sysapiPrintf("/*                  ROUTING  INFORMATION                               */\n");
    sysapiPrintf("/*=====================================================================*/\n");
    
    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*-----------------------------------------------*/\n");
    sysapiPrintf("/* Output of ipMapForwardQueueInfoShow():        */\n");
    sysapiPrintf("/*-----------------------------------------------*/\n");
    sysapiPrintf("\r\n");

    ipMapForwardQueueInfoShow();

    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*---------------------------------*/\n");
    sysapiPrintf("/* Output of ipForwardStatsShow(): */\n");
    sysapiPrintf("/*---------------------------------*/\n");
    sysapiPrintf("\r\n");

    ipForwardStatsShow();

    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*------------------------------*/\n");
    sysapiPrintf("/* Output of rtipStatsShow():             \n");
    sysapiPrintf("/*------------------------------*/\n");
    sysapiPrintf("\r\n");

    rtipStatsShow();


    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*----------------------------------*/\n");
    sysapiPrintf("/* Output of rtipIcmpStatsShow():   */\n");
    sysapiPrintf("/*----------------------------------*/\n");
    sysapiPrintf("\r\n");

    rtipIcmpStatsShow();

    sysapiPrintf("\r\n");
    sysapiPrintf("/*-----------------------------------*/\n");
    sysapiPrintf("/* Output of IpRxStatsShowIcmp():    */\n");
    sysapiPrintf("/*-----------------------------------*/\n");
    sysapiPrintf("\r\n");

    IpRxStatsShowIcmp();

    sysapiPrintf("\r\n");
    sysapiPrintf("/*---------------------------------*/\n");
    sysapiPrintf("/* Output of IpRxStatsShowUdp():   */\n");
    sysapiPrintf("/*---------------------------------*/\n");
    sysapiPrintf("\r\n");

    IpRxStatsShowUdp();


    sysapiPrintf("\r\n");
    sysapiPrintf("/*---------------------------------*/\n");
    sysapiPrintf("/* Output of IpRxStatsShowTcp():   */\n");
    sysapiPrintf("/*---------------------------------*/\n");
    sysapiPrintf("\r\n");

    IpRxStatsShowTcp();


    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*----------------------------------*/\n");
    sysapiPrintf("/* Output of IpRxStatsShowVrrp():   */\n");
    sysapiPrintf("/*----------------------------------*/\n");
    sysapiPrintf("\r\n");

    IpRxStatsShowVrrp();

    sysapiPrintf("\r\n");
    sysapiPrintf("\n");

}
/*********************************************************************
* @purpose  Set IP Mapping Layer tracing mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void ipMapTraceModeSet(L7_uint32 mode)
{
  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return;

  ipMapCfg->rtr.rtrTraceMode    = mode;
  ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
}

/*********************************************************************
* @purpose  Set the debug trace flag.
*
* @param    traceFlag  @b{(input)}  The new value for the IP MAP trace flag
*
* @notes
*
* @end
*********************************************************************/
void ipMapTraceFlagsSet(L7_uint32 traceFlag)
{
    ipMapTraceFlags = traceFlag;
}

/*********************************************************************
* @purpose  Specify an interface for packet tracing
*
* @param    intIfNum  @b{(input)}  internal interface number
*
* @notes
*
* @end
*********************************************************************/
void ipMapTraceIntfSet(L7_uint32 intIfNum)
{
  ipMapTraceIntf = intIfNum;
}

/*********************************************************************
* @purpose  Write a debug trace message.
*
* @param    traceMsg @b{(input)} A string to be displayed as a trace message.
*
* @notes    The input string must be no longer than IPMAP_TRACE_LEN_MAX
*           characters. This function will prepend the message with the
*           current time and will insert a new line character at the
*           beginning of the message.
*
* @end
*********************************************************************/
void ipMapTraceWrite(L7_uchar8 *traceMsg)
{
    L7_uint32 stackUptime;
    L7_uint32 secs;
    L7_uint32 msecs;
    L7_uchar8 debugMsg[512];

    if (traceMsg == NULL)
        return;

    stackUptime = simSystemUpTimeMsecGet();
    secs = stackUptime / 1000;
    msecs = stackUptime % 1000;

    /* For now, just print the message with a timestamp. */
    sprintf(debugMsg, "\n%d.%03d:  ", secs, msecs);
    strncat(debugMsg, traceMsg, IPMAP_TRACE_LEN_MAX);
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,debugMsg);
}

/*********************************************************************
* @purpose  Dumps the ip configuration information onto the
*           display screen.
*
* @param    void
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapCfgDataShow(void)
{
  L7_uchar8 choice;
  L7_uint32 unit,slot,port = 0;

  while (1)
  {
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n+-----------------------------------------------------+");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n|                IpMap Debug Menu                     |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n+-----------------------------------------------------+");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n| 1. Display ipMapCfg switch specific data            |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n| 2. Display ipMapCfg interface specific data         |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n| 3. Display rtrIntf config data                      |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n| Q. Quit                                             |");
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n+-----------------------------------------------------+");

     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nMake a selection: ");
     fflush (stdout);
     osapiFileRead(bspapiConsoleFdGet(), &choice, sizeof(choice));
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n");
     switch (choice)
     {
     case '1':
       ipMapCfgSwitchShow();
       break;
     case '2':
       SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nUnit: ");
       unit = osapiGetChoice();
       SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nSlot (%d for all slots): ",L7_MAX_SLOTS_PER_UNIT+1);
       slot = osapiGetChoice();
       SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nPort (%d for all ports): ",L7_MAX_PORTS_PER_SLOT+1);
       port = osapiGetChoice();
       ipMapCfgIntfShow(unit,slot,port);
       break;
     case '3':
       SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nUnit: ");
       unit = osapiGetChoice();
       SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nSlot (%d for all router interfaces): ",L7_MAX_SLOTS_PER_UNIT+1);
       slot = osapiGetChoice();
       if (slot < L7_MAX_SLOTS_PER_UNIT+1)
       {
         SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nPort (%d for all ports): ",L7_MAX_PORTS_PER_SLOT+1);
         port = osapiGetChoice();
       }
       rtrIntfShow(unit,slot,port);
       break;
     case 'q':
     case 'Q':
       return;
     default:
       SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nPlease enter a valid menu choice.");
       break;
     }
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n\n");
  }
}


/*********************************************************************
* @purpose  Dumps the ip configuration information onto the
*           display screen.
*
* @param    void
*
* @notes    none
*
* @end
*********************************************************************/
void rtrIntfShow(L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  L7_uint32 i, j;
  L7_uchar8 debug_buf[30];
  L7_uint32 intIfNum=0, rtrIfNum;
  nimUSP_t usp;
  L7_uchar8 ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_rtrCfgCkt_t *pCfg = NULL;     
  ipMapOperRtrIntf_t *operIntf;

  if (slot >= L7_MAX_SLOTS_PER_UNIT+1)
    return;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return;
  
  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;
  if (nimGetIntIfNumFromUSP(&usp, &intIfNum) != L7_SUCCESS)
  {
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return;
  }
  if (_ipMapIntIfNumToRtrIntf(intIfNum,&rtrIfNum) != L7_SUCCESS)
  {
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return;
  }
  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return;
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n\nrtrIntf structure:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n------------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nip            ip             admin    mcast   net");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nAddress       mask            mode    fwd     bcast");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n                                      mode    mode");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n------------------------------------------------------");
  for (i = 1;  i < (L7_RTR_MAX_RTR_INTERFACES+1); i++)
  {
      if (rtrIfNum != i)
      {
        continue;
      }

    osapiInetNtoa(pCfg->addrs[0].ipAddr, debug_buf);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n%-15.15s", debug_buf);
    osapiInetNtoa(pCfg->addrs[0].ipMask, debug_buf);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\t%-15.15s", debug_buf);
    if (pCfg->flags & L7_RTR_INTF_ADMIN_MODE_ENABLE)
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tE");
    }
    else
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tD");
    }

    if (pCfg->flags & L7_RTR_INTF_MCAST_FWD_MODE_ENABLE)
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tE");
    }
    else
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tD");
    }

    if (pCfg->flags & L7_RTR_INTF_NETDIR_BCAST_MODE_ENABLE)
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tE");
    }
    else
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tD");
    }
  }

  /* Print secondary addresses */
  if(pCfg->addrs[1].ipAddr != L7_NULL_IP_ADDR)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nSecondary IP Addresses:");
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,
                   "\nIndex     IP            IP");
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,
                   "\n          address       mask");
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,
                   "\n-----------------------------------");

    for(j=1; j < L7_L3_NUM_IP_ADDRS; j++)
    {
      if ((pCfg->addrs[j].ipAddr == 0) || (pCfg->addrs[j].ipMask == 0))
        continue;

      memset((L7_char8 *)ipStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
      memset((L7_char8 *)maskStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
      osapiInetNtoa((L7_uint32)pCfg->addrs[j].ipAddr, ipStr);
      osapiInetNtoa((L7_uint32)pCfg->addrs[j].ipMask, maskStr);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n%d", j);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\t%-15.15s", ipStr);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\t%-15.15s", maskStr);
    }
  }

  operIntf = &pIpMapInfo->operIntf[intIfNum];
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP, "\n\nlastNotify:  %s", 
                 ipMapRouterEventNames[operIntf->lastNotify]);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP, "\nasyncPending:  %s", 
                 operIntf->asyncPending ? "yes" : "no");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP, "\nstackEnabled:  %s", 
                 operIntf->stackEnabled ? "yes" : "no");
  if (operIntf->nimParms.correlator)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP, "\nNIM event pending:  %u", 
                   operIntf->nimParms.event);
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n\nrtrIntfMap structure:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n--------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nrtr     present   intIfNum");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nintf                      ");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n--------------------------");
  for (i = 0; i < (L7_RTR_MAX_RTR_INTERFACES+1); i++)
  {
    if (rtrIfNum != L7_RTR_MAX_RTR_INTERFACES+1)
    {
      if (rtrIfNum != i)
      {
        continue;
      }
    }

    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n%u", i);
    if (rtrIntfMap[i].present == L7_TRUE)
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\ttrue");
    }
    else
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tfalse");
    }
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\t\t%u", rtrIntfMap[i].intIfNum);
  }
  if (rtrIfNum != L7_RTR_MAX_RTR_INTERFACES+1)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n--------------------------\n");

    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n\nintIfNumToRtrIntf structure:");
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n------------------");
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nrtrIfNum  intIfNum");
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n                  ");
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n------------------");
    for (i = 1; i < (L7_RTR_MAX_RTR_INTERFACES+1); i++)
    {
      if (rtrIntfMap[i].intIfNum)
      {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n%u", i);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\t\t%u", rtrIntfMap[i].intIfNum);
      }
    }
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n------------------\n");
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return;
}

/*********************************************************************
* @purpose  Dumps the ip configuration information onto the
*           display screen.
*
* @param    void
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapCfgIntfShow(L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  nimUSP_t  usp;
  L7_uint32 intIfNum, j;
  L7_uchar8 debug_buf[30];
  L7_rtrCfgCkt_t *pCfg;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_uchar8 ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n\nRouting Interfaces:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,
                 "\n--------------------------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "---------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,
                 "\ninterface   IP            IP             admin    mcast   net    proxy   local   addr");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,
                 "\n           address        mask            mode    fwd     bcast   arp    proxy   method");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,
                 "\n                                                  mode    mode    mode   arp");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,
                 "\n                                                                         mode");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,
                 "\n--------------------------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "---------------");

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;

  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return;

  if (nimGetIntIfNumFromUSP(&usp, &intIfNum) != L7_SUCCESS)
  {
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return;
  }

  if (ipMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return;
  }

  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n%s", ifName);

  if (ipMapIntfIsUnnumbered(intIfNum))
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "  %-15.15s", "unnumbered");
  }
  else
  {
    osapiInetNtoa(pCfg->addrs[0].ipAddr, debug_buf);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "   %-15.15s", debug_buf);
    osapiInetNtoa(pCfg->addrs[0].ipMask, debug_buf);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "  %-15.15s", debug_buf);
  }
  if (pCfg->flags & L7_RTR_INTF_ADMIN_MODE_ENABLE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tE");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tD");
  }

  if (pCfg->flags & L7_RTR_INTF_MCAST_FWD_MODE_ENABLE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tE");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tD");
  }

  if (pCfg->flags & L7_RTR_INTF_NETDIR_BCAST_MODE_ENABLE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tE");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tD");
  }

  if ((pCfg->flags & L7_RTR_INTF_PROXY_ARP) != 0)
  {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tE");
  }
  else
  {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tD");
  }

if ((pCfg->flags & L7_RTR_INTF_LOCAL_PROXY_ARP) != 0)
  {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tE");
  }
  else
  {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tD");
  }

  if ((pCfg->flags & L7_RTR_INTF_ADDR_METHOD_DHCP) != 0)
  {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tDHCP");
  }
  else if (pCfg->addrs[0].ipAddr != 0)
  {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tManual");
  }
  else
  {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\tNone");
  }

  /* Print secondary addresses */

  /* If there are no configured ip addresses then return here */
  if(pCfg->addrs[1].ipAddr == L7_NULL_IP_ADDR)
  {
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
    return;
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nSecondary IP Addresses:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,
                 "\nIndex     IP            IP");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,
                 "\n          address       mask");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,
                 "\n-----------------------------------");

  for(j=1; j < L7_L3_NUM_IP_ADDRS; j++)
  {
    if (pCfg->addrs[j].ipAddr == L7_NULL_IP_ADDR)
      continue;

    memset((L7_char8 *)ipStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
    memset((L7_char8 *)maskStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
    osapiInetNtoa((L7_uint32)pCfg->addrs[j].ipAddr, ipStr);
    osapiInetNtoa((L7_uint32)pCfg->addrs[j].ipMask, maskStr);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n%d", j);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\t%-15.15s", ipStr);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\t%-15.15s", maskStr);
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n");
  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return;
}

/*********************************************************************
* @purpose  Dumps the ip configuration information onto the
*           display screen.
*
* @param    void
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapCfgSwitchShow(void)
{
  if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    return;

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n|              IP CONFIGURATION DATA                     |");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n\nHeader Information:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nfilename...............................%s", ipMapCfg->cfgHdr.filename);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nversion................................%d", ipMapCfg->cfgHdr.version);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\ncomponentID............................%d", ipMapCfg->cfgHdr.componentID);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\ntype...................................%d", ipMapCfg->cfgHdr.type);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nlength.................................%d", ipMapCfg->cfgHdr.length);

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n\nIP specific Information:");
  if (ipMapCfg->ip.ipSourceAddrCheck == L7_ENABLE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nipSourceAddrCheck......................enable");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nipSourceAddrCheck......................disable");
  }

  if (ipMapCfg->ip.ipSpoofingCheck == L7_ENABLE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nipSpoofingCheck........................enable");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nipSpoofingCheck........................disable");
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nipDefaultTTL...........................%d", ipMapCfg->ip.ipDefaultTTL);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nipArpAgeTime...........................%d", ipMapCfg->ip.ipArpAgeTime);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nipArpRespTime..........................%d", ipMapCfg->ip.ipArpRespTime);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nipArpRetries...........................%d", ipMapCfg->ip.ipArpRetries);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nipArpCacheSize.........................%d", ipMapCfg->ip.ipArpCacheSize);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nipArpDynamicRenew......................%d", ipMapCfg->ip.ipArpDynamicRenew);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n\nRouter Information:");

  if (ipMapCfg->rtr.rtrAdminMode == L7_ENABLE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nrtrAdminMode...........................enable");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nrtrAdminMode...........................disable");
  }
  
  if (ipMapCfg->rtr.rtrTOSForwarding == L7_ENABLE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nrtrTOSForwarding.......................enable");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nrtrTOSForwarding.......................disable");
  }

  if (ipMapCfg->rtr.rtrICMPRedirectMode == L7_ENABLE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nrtrICMPRedirectMode....................enable");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nrtrICMPRedirectMode....................disable");
  }

  if (ipMapCfg->rtr.rtrTraceMode == L7_ENABLE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nrtrTraceMode...........................enable");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nrtrTraceMode...........................disable");
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n|              IP INFORMATION STRUCTURES                 |");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n+--------------------------------------------------------+");

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP, "\n\nlastNotify:  %s", 
                 ipMapRouterEventNames[pIpMapInfo->operRtr.lastNotify]);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP, "\nasyncPending:  %s", 
                 pIpMapInfo->operRtr.asyncPending ? "yes" : "no");

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nipMapForwardingTaskId...............%u", 
                 (L7_uint32) ipMapForwardingTaskId);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nipMapProcessingTaskId...............%u", 
                 (L7_uint32) ipMapProcessingTaskId);

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n\tstackSize..............%u", ipMapProcessingSidDefaultStackSize());
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n\ttaskPriority...........%u", ipMapProcessingSidDefaultTaskPriority());
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n\ttaskSlice..............%u", ipMapProcessingSidDefaultTaskSlice());

  ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
  return;
}

/*********************************************************************
* @purpose  Display all static ARP entries.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void ipMapStaticArpShow(void)
{
    L7_arpEntry_t staticEntries[L7_IP_ARP_CACHE_STATIC_MAX];
    L7_uint32 i = 0;
    L7_uint32 j;
    L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];

    memset(staticEntries, 0, L7_IP_ARP_CACHE_STATIC_MAX * sizeof(L7_arpEntry_t));
    if (ipMapStaticArpGetAll(staticEntries) == L7_SUCCESS)
    {
        for (i = 0;
             (i < L7_IP_ARP_CACHE_STATIC_MAX) && (staticEntries[i].ipAddr != 0);
             i++)
        {
            if (i == 0)
            {
                SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, "\n");
                SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "IP Address     MAC Address          Intf\n");
            }
            osapiInetNtoa(staticEntries[i].ipAddr, ipAddrStr);
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                          "\n%-15.15s", ipAddrStr);

            for (j = 0; j < L7_MAC_ADDR_LEN-1; j++)
            {
                SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                              "%2.2x:", staticEntries[i].macAddr.addr.enetAddr.addr[j]);
            }
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                          "%2.2x    ", staticEntries[i].macAddr.addr.enetAddr.addr[j]);
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                          "%-4.4x\n", staticEntries[i].intIfNum);
        }
    }
}

/*********************************************************************
* @purpose  Display all static Route entries.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void ipMapStaticRouteShow(void)
{
    L7_uint32 i, nh;  /* array indices */
    L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 ipMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 nhStr[OSAPI_INET_NTOA_BUF_SIZE];

    if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
      return;

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                  "\nDest Address   Dest Mask     Preference     Next Hop       Intf     Flags\n");

    for (i = 0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
    {
        if (route->rtrStaticRouteCfgData[i].inUse == L7_TRUE)
        {
            osapiInetNtoa(route->rtrStaticRouteCfgData[i].ipAddr, ipAddrStr);
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                          "\n%-15.15s", ipAddrStr);

            osapiInetNtoa(route->rtrStaticRouteCfgData[i].ipMask, ipMaskStr);
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                          "%-15.15s", ipMaskStr);

            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, "%10u   ",
                          route->rtrStaticRouteCfgData[i].preference);

            for (nh = 0; (nh < platRtrRouteMaxEqualCostEntriesGet()) &&
                  (route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr != 0); nh++)
            {
                if (nh != 0)
                    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, "\n%43s", "");
                osapiInetNtoa(route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr, nhStr);
                SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, "%-15.15s", nhStr);
                SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,"%02d/%02d/%02d",
                    route->rtrStaticRouteCfgData[i].nextHops[nh].intfConfigId.configSpecifier.usp.unit,
                    route->rtrStaticRouteCfgData[i].nextHops[nh].intfConfigId.configSpecifier.usp.slot,
                    route->rtrStaticRouteCfgData[i].nextHops[nh].intfConfigId.configSpecifier.usp.port);
            }

            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP, "%10u   ",
                          route->rtrStaticRouteCfgData[i].flags);
        }
    }
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
}

/*********************************************************************
* @purpose  Give the IP MAP read/write lock, ipRwLock.
*
* @param    lockType  ({input})  whether to give read lock or write lock
* @param    lockHolder({output}) function name of caller. Use for debugging. 
*
* @returns  L7_SUCCESS  if lock successfully acquired
* @returns  L7_FAILURE  otherwise
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapLockInfoShow(void)
{
  L7_uint32 i;

  if (strlen(ipMapLockInfo->writeLockHolder.fName) != 0)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,  
                  "\nIP MAP write lock last taken in %s.",
                  ipMapLockInfo->writeLockHolder.fName);
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,  
                  "\nIP MAP write lock available.");
  }

  for (i = 0; i < IPMAP_READLOCK_REC_MAX; i++)
  {
    if (strlen(ipMapLockInfo->readLockHolder[i].fName) != 0) 
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,  
                    "\nIP MAP read lock taken in %s.",
                    ipMapLockInfo->readLockHolder[i].fName);
    }
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,  
                "\nIP MAP read/write lock stats:  "
                "\n    write take attempts: %u"
                "\n    write take failures: %u"
                "\n    write give attempts: %u"
                "\n    write give failures: %u"
                "\n    read take attempts: %u"
                "\n    read take failures: %u"
                "\n    read give attempts: %u"
                "\n    read give failures: %u",
                ipMapLockInfo->writeTakeAttempts, ipMapLockInfo->writeTakeFailures,
                ipMapLockInfo->writeGiveAttempts, ipMapLockInfo->writeGiveFailures,
                ipMapLockInfo->readTakeAttempts, ipMapLockInfo->readTakeFailures,
                ipMapLockInfo->readGiveAttempts, ipMapLockInfo->readGiveFailures);
}


/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void ipMapStaticRouteBuildTestConfigData(void)
{


  L7_uint32 i, j;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/

  for (i = 0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
  {

      route->rtrStaticRouteCfgData[i].inUse     = L7_TRUE;
      route->rtrStaticRouteCfgData[i].ipAddr    = 0x0A0A0A01 + i;
      route->rtrStaticRouteCfgData[i].ipMask    = 0xFFFF0000;

      for (j = 0; j < platRtrRouteMaxEqualCostEntriesGet(); j++)
      {
          route->rtrStaticRouteCfgData[i].nextHops[j].nextHopRtr     = 0x0A0A0B01 + j;
      }
      route->rtrStaticRouteCfgData[i].preference = i+2;
      route->rtrStaticRouteCfgData[i].flags     = 0x155;
  }

 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   route->cfgHdr.dataChanged = L7_TRUE;
   sysapiPrintf("Built test config data\n");


}



/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void ipMapStaticRouteConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(route->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    ipMapStaticRouteShow();


    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "L7_RTR_MAX_STATIC_ROUTES - %d\n", L7_RTR_MAX_STATIC_ROUTES);
    sysapiPrintf( "L7_RT_MAX_EQUAL_COST_ROUTES - %d\n", platRtrRouteMaxEqualCostEntriesGet());



    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("route->checkSum : %u\n", route->checkSum);


}



/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/



/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void ipStaticArpBuildTestConfigData(void)
{


  L7_uint32 i, j;
  L7_IP_ADDR_t ipAddr;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/


  ipAddr    = 0x0A0A0B01;
  for (i=0;  i < L7_RTR_MAX_STATIC_ARP_ENTRIES; i++)
  {

      arp->rtrStaticArpCfgData[i].macAddr.type =  L7_LL_ETHERNET;
      arp->rtrStaticArpCfgData[i].macAddr.len =   L7_MAC_ADDR_LEN;
      for (j = 0; j < (L7_MAC_ADDR_LEN-1); j++)
      {
          arp->rtrStaticArpCfgData[i].macAddr.addr.enetAddr.addr[j] =   0xaa;
      }

      arp->rtrStaticArpCfgData[i].ipAddr = ipAddr + i;
  }

 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   arp->cfgHdr.dataChanged = L7_TRUE;
   sysapiPrintf("Built test config data\n");


}



/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void ipStaticArpConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(arp->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    ipMapStaticArpShow();


    /*-------------------------------*/
    /* Scaling Constants             */
    /*-------------------------------*/


    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "FD_RTR_MAX_STATIC_ARP_ENTRIES - %d\n", FD_RTR_MAX_STATIC_ARP_ENTRIES);


    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("arp->checkSum : %u\n", arp->checkSum);


}



/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/




/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void ipMapBuildTestConfigData(void)
{


  L7_uint32 cfgIndex;
  L7_rtrCfgCkt_t *pCfg;


  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/


  /* Generic IP Stack Cfg */
  ipMapCfg->ip.ipSourceAddrCheck     =  L7_ENABLE;
  ipMapCfg->ip.ipSpoofingCheck       =  L7_ENABLE;
  ipMapCfg->ip.ipDefaultTTL          =  44;

  /* ARP Configuration parameters */
  ipMapCfg->ip.ipArpAgeTime          =  55;
  ipMapCfg->ip.ipArpRespTime         =  66;
  ipMapCfg->ip.ipArpRetries          =  77;
  ipMapCfg->ip.ipArpCacheSize        =  88;
  ipMapCfg->ip.ipArpDynamicRenew     =  L7_ENABLE;

  memcpy((L7_uint32 *)&ipMapCfg->ip.route_preferences, (L7_uint32 *)&FD_rtr_rto_preferences,
         (sizeof (L7_uint32) * L7_ROUTE_MAX_PREFERENCES));

  /* Router Configuration parameters */
  ipMapCfg->rtr.rtrAdminMode             = L7_ENABLE;
  ipMapCfg->rtr.rtrTOSForwarding         = L7_DISABLE;
  ipMapCfg->rtr.rtrICMPRedirectMode      = L7_DISABLE;
  ipMapCfg->rtr.rtrTraceMode             = L7_ENABLE;

  /* Router Configuration parameters */
  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
  {
    pCfg = &ipMapCfg->ckt[cfgIndex];
    ipIntfBuildTestConfigData(&pCfg->configId, pCfg);
  }
 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   ipMapCfg->cfgHdr.dataChanged = L7_TRUE;
   sysapiPrintf("Built test config data\n");


}

/*********************************************************************
* @purpose  Build non-default ip config data for the intf
*
* @param    slot                slot number
* @param    port                port number
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipIntfBuildTestConfigData(nimConfigID_t *configId, L7_rtrCfgCkt_t *pCfg)
{
  L7_uint32 intIfNum;

  intIfNum = 1;

  (void) nimIntIfFromConfigIDGet(&(pCfg->configId), &intIfNum);

  pCfg->icmpUnreachablesMode = L7_ENABLE;

  pCfg->ipMtu = FD_IP_DEFAULT_IP_MTU + intIfNum;

  if (FD_RTR_DEFAULT_INTF_ADMIN_MODE == L7_ENABLE)
  {
    pCfg->flags  |=  L7_RTR_INTF_ADMIN_MODE_ENABLE;
  }

  if (FD_RTR_DEFAULT_FWD_MULTICASTS == L7_ENABLE)
  {
    pCfg->flags  |=  L7_RTR_INTF_MCAST_FWD_MODE_ENABLE;
  }

  if (FD_RTR_DEFAULT_FWD_NET_DIRECT_BCAST == L7_ENABLE)
  {
    pCfg->flags  |=  L7_RTR_INTF_NETDIR_BCAST_MODE_ENABLE;
  }

  if (FD_PROXY_ARP_ADMIN_MODE == L7_ENABLE)
  {
    pCfg->flags  |=  L7_RTR_INTF_PROXY_ARP;
  }

  if (FD_RTR_DEFAULT_GRAT_ARP == L7_ENABLE)
  {
    pCfg->flags  |=  L7_RTR_INTF_GRAT_ARP;
  }

  return;
}

/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void ipMapConfigDataTestShow(void)
{

    nimUSP_t    usp;
    L7_uint32 cfgIndex;
    L7_fileHdr_t  *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(ipMapCfg->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    ipMapCfgSwitchShow();

    /* Router Configuration parameters */
    for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
    {

        memset((void *)&usp, 0, sizeof(nimUSP_t));
        if (nimUspFromConfigIDGet(&(ipMapCfg->ckt[cfgIndex].configId), &usp) == L7_SUCCESS)
        {
            ipMapCfgIntfShow(usp.unit, usp.slot, usp.port);
        }
    }



    /*-------------------------------*/
    /* Scaling Constants             */
    /*-------------------------------*/


    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "L7_IPMAP_INTF_MAX_COUNT - %d\n", L7_IPMAP_INTF_MAX_COUNT);


    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("ipMapCfg->checkSum : %u\n", ipMapCfg->checkSum);


}



/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/

typedef struct _dbg_arp
{
    L7_uchar8 addrType[2];
    L7_uchar8 protoType[2];
    L7_uchar8 macSize;
    L7_uchar8 protoSize;
    L7_uchar8 op[2];
    L7_uchar8 sndMac[6];
    L7_uchar8 sndIp[4];
    L7_uchar8 rcvMac[6];
    L7_uchar8 rcvIp[4];
} dbg_arp;


void dbg_form_arp(L7_uchar8 *buf, L7_uint32 sa, L7_uint32 da, 
        L7_uchar8 *smac, L7_uchar8 *dmac, L7_uchar8 op )
{
    dbg_arp *pkt = (dbg_arp *)buf;

    pkt->addrType[0] = 0;
    pkt->addrType[1] = 1;

    pkt->protoType[0] = 0x08;
    pkt->protoType[1] = 0x00;

    pkt->macSize = 6;
    pkt->protoSize = 4;

    pkt->op[0] = 0;
    pkt->op[1] = op;

    memcpy(pkt->sndMac, smac, 6);
    memcpy(pkt->sndIp, &sa, 4);
    memcpy(pkt->rcvMac, dmac, 6);
    memcpy(pkt->rcvIp, &da, 4);
}

void dbg_arp_req(L7_uint32 intIfNum, L7_uint32 ipSa, L7_uint32 ipDa)
{
    L7_uchar8 *buff = osapiMalloc(L7_IP_MAP_COMPONENT_ID, 100);
    L7_uchar8 sMac[6] = {0, 0, 0, 1, 2, 0};
    L7_uchar8 dMac[6] = {0, 0, 0, 4, 5, 0};
    sysnet_pdu_info_t pduInfo;
    L7_uint32 bufHandle;
    L7_ushort16 pFlag = L7_LL_ENCAP_ENET | L7_LL_USE_L3_SRC_MAC;

    sMac[4] = (ipSa & 0xFF00)>>8;
    sMac[5] = (ipSa & 0xFF);

    dMac[4] = (ipDa & 0xFF00)>>8;
    dMac[5] = (ipDa & 0xFF);

    dbg_form_arp(buff, osapiHtonl(ipSa), osapiHtonl(ipDa), sMac, dMac, 1);
    memset( &pduInfo, 0, sizeof(pduInfo));
    pduInfo.intIfNum = intIfNum;

    sysNetPduHdrEncapsulate(dMac, intIfNum, &pFlag, sizeof(dbg_arp), 0, 
            L7_ETYPE_ARP, &bufHandle, buff );
    ipMapArpRecvIP(bufHandle, &pduInfo);
}

void dbg_arp_rep(L7_uint32 intIfNum, L7_uint32 ipSa, L7_uint32 ipDa)
{
    L7_uchar8 *buff = osapiMalloc(L7_IP_MAP_COMPONENT_ID, 100);
    L7_uchar8 sMac[6] = {0, 0, 0, 1, 2, 0};
    L7_uchar8 dMac[6] = {0, 0, 0, 4, 5, 0};
    sysnet_pdu_info_t pduInfo;
    L7_uint32 bufHandle;
    L7_ushort16 pFlag = L7_LL_ENCAP_ENET | L7_LL_USE_L3_SRC_MAC;

    sMac[4] = (ipSa & 0xFF00)>>8;
    sMac[5] = (ipSa & 0xFF);

    dMac[4] = (ipDa & 0xFF00)>>8;
    dMac[5] = (ipDa & 0xFF);

    dbg_form_arp(buff, osapiHtonl(ipDa), osapiHtonl(ipSa), dMac, sMac, 2);
    memset( &pduInfo, 0, sizeof(pduInfo));
    pduInfo.intIfNum = intIfNum;

    sysNetPduHdrEncapsulate(dMac, intIfNum, &pFlag, sizeof(dbg_arp), 0, 
            L7_ETYPE_ARP, &bufHandle, buff );
    ipMapArpRecvIP(bufHandle, &pduInfo);
}

void dbg_arp_get(L7_uint32 intIfNum, L7_uint32 ipAddr)
{
  L7_uchar8 mac[16];

  if ( ipMapArpAddrResolve(intIfNum, ipAddr, mac, NULL, 0, 0) == L7_SUCCESS)
  {
    sysapiPrintf("Intf = %d IP = %x MAC = %02x:%02x:%02x:%02x:%02x:%02x\n",
       intIfNum, ipAddr, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
  }
  else
  {
    sysapiPrintf("MAC Resolution for IP %x on Intf %d failed\n", ipAddr, 
        intIfNum );
  }
}

typedef struct _dbg_ip_hdr{
  L7_uint32 ihl; /* Ver + IHL + tos + length */
  L7_uint32 id;  /* ID + flags + Frag offset */
  L7_uint32 ttl; /* ttl + Protocol + checksum */
  L7_uint32 sa;
  L7_uint32 da;
  L7_uint32 op;
} dbg_ip_hdr_t;

void dbg_form_ip(L7_uchar8 *buff, L7_uint32 sa, L7_uint32 da, L7_uint32 op)
{
  dbg_ip_hdr_t hdr;

  hdr.ihl = osapiHtonl(0x45000040);
  hdr.id = osapiHtonl(0x00010000);
  hdr.ttl = osapiHtonl(0xFF000000);
  hdr.sa = osapiHtonl(sa);
  hdr.da = osapiHtonl(da);
  if ( op != 0 )
  {
    hdr.ihl += osapiHtonl(0x01000000);
    hdr.op = osapiHtonl(op);
  }
  memcpy(buff, &hdr, sizeof(hdr) );
}

void dbg_ip_rcv(L7_uint32 intIfNum, L7_uint32 ipSa, L7_uint32 ipDa, L7_uint32 op)
{
    L7_uchar8 *buff = osapiMalloc(L7_IP_MAP_COMPONENT_ID, 100);
    L7_uchar8 sMac[6] = {0, 0, 0, 1, 2, 0};
    L7_uchar8 dMac[6] = {0, 0, 0, 4, 5, 0};
    sysnet_pdu_info_t pduInfo;
    L7_uint32 bufHandle;
    L7_ushort16 pFlag = L7_LL_ENCAP_ENET | L7_LL_USE_L3_SRC_MAC;

    sMac[4] = (ipSa & 0xFF00)>>8;
    sMac[5] = (ipSa & 0xFF);

    dMac[4] = (ipDa & 0xFF00)>>8;
    dMac[5] = (ipDa & 0xFF);

    dbg_form_ip(buff, ipSa, ipDa, op);
    memset( &pduInfo, 0, sizeof(pduInfo));
    pduInfo.intIfNum = intIfNum;

    sysNetPduHdrEncapsulate(dMac, intIfNum, &pFlag, sizeof(dbg_arp), 0, 
            L7_ETYPE_IP, &bufHandle, buff );
    rtoIPv4Handle(bufHandle, &pduInfo);
    return;
}

/***************************** Start IPMAP Debug Packet Traces ****************/
#ifdef L7_QOS_FLEX_PACKAGE_ACL

/*********************************************************************
*
* @purpose Trace Ip packets received
*
* @param   intIfNum     @b{(input)} Internal Interface Number
* @param   *buff        @b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void ipMapDebugPacketRxTrace(L7_uint32 intIfNum,L7_uint32 vlanId,L7_ipHeader_t *ipHdr, void *layer4_data )
{
  L7_uint32 unit, slot, port,aclSrcIp,aclDestIp,aclmask;
  L7_uchar8 srcStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 igmpGrpAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_ACL_ASSIGNED_INTF_LIST_t intfList;
  L7_uint32 rulenum,i,index;
  L7_udp_header_t *udp_header = L7_NULL;
  struct icmp *icmp_header = L7_NULL;
  L7_igmpMsg_t *igmp_header = L7_NULL;
  L7_BOOL every=L7_FALSE;  
  L7_RC_t rc;
  L7_BOOL flag = L7_FALSE;
  L7_uint32 protocol;
  L7_uint32 val,val2;
  L7_BOOL matchAcl = L7_FALSE; 
  nimUSP_t usp;

  aclSrcIp = L7_NULL;
  aclDestIp = L7_NULL;

  for ( index=0; (index < L7_MAX_ACL_ID) && (aclNumTrace[index] != 0); index ++)  
  {
    if ( matchAcl == L7_TRUE)
    {
      break;
    }
    if (aclNumCheckValid(aclNumTrace[index]) != L7_SUCCESS )
    {
      continue;
    }
    if( aclAssignedIntfDirListGet(aclNumTrace[index], L7_INBOUND_ACL, &intfList) != L7_SUCCESS)
    {
      continue;
    }
  /* make sure ACL exists for an interface in order to trace it */
    for (i=0; i<intfList.count; i++)
    {
      if (intfList.intIfNum[i] == intIfNum)
      {
        break;
      }
    }
    if (i >= intfList.count)
    {
      continue;
    }
    rc =  aclRuleGetFirst(aclNumTrace[index], &rulenum);
    if( rc != L7_SUCCESS)
    {
      continue; 
    }
    do
    {
      flag = L7_FALSE;
      every = L7_FALSE;
      if (aclIsFieldConfigured(aclNumTrace[index],rulenum,ACL_ACTION) == L7_TRUE)
      {
        if(aclRuleActionGet(aclNumTrace[index],rulenum,&val)== L7_SUCCESS)
        {
          if (val !=  L7_ACL_PERMIT)
          {
            continue;
          }
        }
        else
        {
          continue;
        }
      }
      if (aclIsFieldConfigured(aclNumTrace[index],rulenum,ACL_EVERY)== L7_TRUE)
      {
        if (aclRuleEveryGet(aclNumTrace[index],rulenum,&every)!= L7_SUCCESS)
        {
          continue;
        }
      }

      if((rc == L7_SUCCESS) && (every == L7_FALSE))
      {
        if ( aclIsFieldConfigured(aclNumTrace[index],rulenum,ACL_SRCIP) == L7_TRUE) 
        {
          if(aclRuleSrcIpMaskGet(aclNumTrace[index],rulenum,&aclSrcIp,&aclmask)!= L7_SUCCESS)
          {
            continue;
          }
          if ((aclSrcIp &aclmask) == (osapiNtohl(ipHdr->iph_src)& aclmask))
          {
            flag = L7_TRUE;
          }
          else
          {
            continue;
          }
        }
        if ( aclIsFieldConfigured(aclNumTrace[index],rulenum,ACL_DSTIP) == L7_TRUE)
        {
          if (aclRuleDstIpMaskGet(aclNumTrace[index], rulenum, &aclDestIp, &aclmask)!= L7_SUCCESS)
          {
            continue;
          }
          if ( (aclmask &aclDestIp) == (osapiNtohl(ipHdr->iph_dst)& aclmask))
          {
            flag = L7_TRUE;
          }
          else
          {
            continue;
          }
        }
        if (aclIsFieldConfigured(aclNumTrace[index],rulenum,ACL_PROTOCOL) == L7_TRUE)
        {
          if(aclRuleProtocolGet(aclNumTrace[index],rulenum,&protocol)!= L7_SUCCESS)
          {
            continue;
          }
          if (ipHdr->iph_prot == protocol)
          {
            flag = L7_TRUE;
            switch ( ipHdr->iph_prot)
            {
              case L7_ACL_PROTOCOL_UDP:
              case L7_ACL_PROTOCOL_TCP:

                udp_header = (L7_udp_header_t *)layer4_data;
                if (aclIsFieldConfigured(aclNumTrace[index],rulenum,ACL_DSTPORT) == L7_TRUE)
                {
                  if(aclRuleDstL4PortGet(aclNumTrace[index],rulenum,&port)!= L7_SUCCESS)
                  {
                    flag = L7_FALSE;
                    break; 
                  }
                  if (osapiHtons(port) == (udp_header->destPort))
                  {
                    flag = L7_TRUE;
                  }
                  else
                  {
                    flag = L7_FALSE;
                    break;
                  }
                }        
                if ( (aclIsFieldConfigured(aclNumTrace[index],rulenum,ACL_DSTSTARTPORT) == L7_TRUE) &&
                    (aclIsFieldConfigured(aclNumTrace[index],rulenum,ACL_DSTENDPORT) == L7_TRUE) )
                {
                  if (aclRuleDstL4PortRangeGet( aclNumTrace[index], rulenum, &val, &val2)!= L7_SUCCESS)
                  {
                    flag = L7_FALSE;
                    break; 
                  }
                  if ((udp_header->destPort >= osapiHtons(val)
                        && udp_header->destPort <= osapiHtons(val2)))
                  {
                    flag = L7_TRUE;
                  }
                  else
                  {
                    flag = L7_FALSE;
                    break;
                  }
                }
                if (aclIsFieldConfigured(aclNumTrace[index],rulenum,ACL_SRCPORT) == L7_TRUE)
                {
                  if (aclRuleSrcL4PortGet(aclNumTrace[index],rulenum,&val)!= L7_SUCCESS)
                  {
                    flag = L7_FALSE;
                    break;
                  }
                  if (osapiHtons(val) == (udp_header->sourcePort))
                  {
                    flag = L7_TRUE;
                  }
                  else
                  { 
                    flag = L7_FALSE;
                    break;
                  }
                }

                if ( (aclIsFieldConfigured(aclNumTrace[index],rulenum,ACL_SRCSTARTPORT) == L7_TRUE) &&
                    (aclIsFieldConfigured(aclNumTrace[index],rulenum,ACL_SRCENDPORT) == L7_TRUE) )
                {
                  if(aclRuleSrcL4PortRangeGet( aclNumTrace[index], rulenum, &val, &val2) != L7_SUCCESS)
                  {
                    flag = L7_FALSE;
                    break;
                  }
                  if ((udp_header->sourcePort >= osapiHtons(val))
                      &&(udp_header->destPort <=osapiHtons(val2)))
                  {
                    flag = L7_TRUE;
                  }
                  else
                  {
                    flag = L7_FALSE;
                    break;
                  }
                }
                break; 

              case L7_ACL_PROTOCOL_ICMP:
                icmp_header = (struct icmp *)layer4_data;
                break;

              case L7_ACL_PROTOCOL_IGMP:   
                igmp_header = (L7_igmpMsg_t *)layer4_data;
                break;

              default:
                break;    
            }
            if(flag == L7_FALSE)
            {
              continue;/* go to next rule if the protocol does not match with acl configured protocol*/
            }
          }/* end if protocol match*/
          else if ( protocol == L7_ACL_PROTOCOL_IP)
          {
            flag = L7_TRUE;
          }
          else
          {
            continue;
          }
        }/* end if acl protocol field configuration */
        if ( aclIsFieldConfigured(aclNumTrace[index],rulenum,ACL_IPTOS) == L7_TRUE)
        {
          if (aclRuleIPTosGet( aclNumTrace[index], rulenum, &val, &val2) != L7_SUCCESS)
          {
           continue;
          }
          if ( ipHdr->iph_tos ==  (val&val2) )
          {
            flag = L7_TRUE;
          }
          else
          {
            continue;
          }
        }
      }

      /* printing on to the console if this packet matches any one of the Acl rule */ 
      if ((flag == L7_TRUE) || (every == L7_TRUE))
      {
        if (nimGetIntfName(intIfNum, L7_SYSNAME, ifName) != L7_SUCCESS)
          strcpy(ifName, "invalid interface");
        osapiInetNtoa(osapiNtohl(ipHdr->iph_src), srcStr);
        osapiInetNtoa(osapiNtohl(ipHdr->iph_dst), destStr);
        matchAcl = L7_TRUE;
        /* Convert from intIfNum to USP */
        if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
        { 
          unit = usp.unit;
          slot = usp.slot;
          port = usp.port;
          if (aclIsFieldConfigured(aclNumTrace[index],rulenum,ACL_PROTOCOL) == L7_TRUE)
          {
            switch ( protocol)
            {
              case L7_ACL_PROTOCOL_UDP:
              case L7_ACL_PROTOCOL_TCP:
                if (udp_header != L7_NULL)
                  IPMAP_USER_TRACE(IPMAP_DEBUG_PACKET_TCPUDP_RX_FORMAT,unit, slot, port, intIfNum, vlanId, ipHdr->iph_versLen, ipHdr->iph_tos, osapiNtohs(ipHdr->iph_len),  osapiNtohs(ipHdr->iph_ident), ipHdr->iph_flags_frag, ipHdr->iph_ttl, ipHdr->iph_prot, srcStr, destStr ,osapiNtohs(udp_header->sourcePort), osapiNtohs(udp_header->destPort));
                break;

              case L7_ACL_PROTOCOL_ICMP:
                if (icmp_header != L7_NULL)
                  IPMAP_USER_TRACE(IPMAP_DEBUG_PACKET_ICMP_RX_FORMAT,unit, slot, port, intIfNum, vlanId, ipHdr->iph_versLen, ipHdr->iph_tos, osapiNtohs(ipHdr->iph_len),  osapiNtohs(ipHdr->iph_ident), ipHdr->iph_flags_frag, ipHdr->iph_ttl, ipHdr->iph_prot, srcStr, destStr ,icmp_header->icmp_type, icmp_header->icmp_code);

                break;   

              case L7_ACL_PROTOCOL_IGMP:
                if (igmp_header != L7_NULL)
                {
                  osapiInetNtoa(osapiNtohl(igmp_header->igmpGroupAddr), igmpGrpAddrStr);
                  IPMAP_USER_TRACE(IPMAP_DEBUG_PACKET_IGMP_RX_FORMAT,unit, slot, port, intIfNum, vlanId, ipHdr->iph_versLen, ipHdr->iph_tos, osapiNtohs(ipHdr->iph_len),  osapiNtohs(ipHdr->iph_ident), ipHdr->iph_flags_frag, ipHdr->iph_ttl, ipHdr->iph_prot, srcStr, destStr ,igmp_header->igmpType, igmpGrpAddrStr);
                }
                break; 

              default:
                IPMAP_USER_TRACE(IPMAP_DEBUG_PACKET_RX_FORMAT,unit, slot, port, intIfNum, vlanId, ipHdr->iph_versLen, ipHdr->iph_tos, osapiNtohs(ipHdr->iph_len),  osapiNtohs(ipHdr->iph_ident), ipHdr->iph_flags_frag, ipHdr->iph_ttl, ipHdr->iph_prot, srcStr, destStr);
                break;
            }
          }
          else
          {
            IPMAP_USER_TRACE(IPMAP_DEBUG_PACKET_RX_FORMAT,unit, slot, port, intIfNum, vlanId, ipHdr->iph_versLen, ipHdr->iph_tos, osapiNtohs(ipHdr->iph_len),  osapiNtohs(ipHdr->iph_ident), ipHdr->iph_flags_frag, ipHdr->iph_ttl, ipHdr->iph_prot, srcStr, destStr);
          }
        }
        break;
      }/* end if flag = TRUE*/

    }while(aclRuleGetNext(aclNumTrace[index],rulenum,&rulenum)== L7_SUCCESS );/*end of do- while */
  }

    return;
}

/*********************************************************************
*
* @purpose Trace Ip packets transmitted
*
* @param   intIfNum     @b{(input)} Internal Interface Number
* @param   *buff        @b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void ipMapDebugPacketTxTrace(L7_uint32 intIfNum,L7_uint32 vlanId, L7_char8  *ipHeader)
{
  /* Future use when we support acl's for eggressive ports*/
  return;
}

/*********************************************************************
 * @purpose  Get the current status of displaying ip packet debug info
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
L7_RC_t ipMapDebugPacketTraceFlagGet(L7_uint32 aclNum,L7_BOOL *flag)
{
  L7_uint32 aclIndex =0;
  if ( aclNumRangeCheck(aclNum) != L7_SUCCESS )
    return L7_FAILURE;

  for (aclIndex = 0; aclIndex<L7_MAX_ACL_ID; aclIndex++)
  {
    if ( aclNumTrace[aclIndex] == aclNum)
    {
      *flag = L7_TRUE; 
       return L7_SUCCESS; 
    }
  }
  
  *flag = L7_FALSE;
  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  Turns on/off the displaying of ip packet debug info
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
L7_RC_t ipMapDebugPacketTraceFlagSet(L7_uint32 aclNum, L7_BOOL flag)
{
  L7_uint32 aclIndex =0;
  L7_uint32 tempAcls[L7_MAX_ACL_ID];
  L7_uint32 tempIndex =0;
  
  memset(tempAcls, 0x00, sizeof(tempAcls)); 
  if ( aclNumRangeCheck(aclNum) != L7_SUCCESS )
  {
    return L7_FAILURE;
  }

  if (flag == L7_TRUE)
  {
    for (aclIndex = 0; aclIndex<L7_MAX_ACL_ID; aclIndex++)
    {
      if ( aclNumTrace[aclIndex] == aclNum)
      {
        return L7_SUCCESS;
      }
      else if ( aclNumTrace[aclIndex] == 0)
      {
        aclNumTrace[aclIndex]= aclNum;
        break;
      }
    }
  }
  else if (flag == L7_FALSE)
  {
    for (aclIndex = 0; aclIndex<L7_MAX_ACL_ID ; aclIndex++)
    {
      if ( aclNumTrace[aclIndex] == aclNum)
      {
        aclNumTrace[aclIndex] = 0;
        break;
      }
    }
    for (aclIndex = 0; aclIndex<L7_MAX_ACL_ID ; aclIndex++)
    {
      if (aclNumTrace[aclIndex] != 0)
      {
        tempAcls[tempIndex]= aclNumTrace[aclIndex];
        tempIndex++;
      }
    }
    memcpy(aclNumTrace, tempAcls, sizeof(aclNumTrace));

  }
  else
  {
    return L7_FAILURE;
  }
  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a
     debug config save.
   */
  ipMapDebugCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Save configuration settings for ipMap trace data
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
L7_RC_t ipMapDebugPacketTraceFlagSave()
{
  memcpy(ipMapDebugCfg.cfg.aclNumTrace, aclNumTrace, sizeof(ipMapDebugCfg.cfg.aclNumTrace));
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
void ipMapDebugCfgUpdate(void)
{
 ipMapDebugPacketTraceFlagSave();

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
void ipMapDebugCfgRead(void)
{
  /* reset the debug flags*/
  memset((void*)&ipMapDebugCfg, 0 ,sizeof(ipMapDebugCfg_t));
  (void)sysapiSupportCfgFileGet(L7_IP_MAP_COMPONENT_ID, IPMAP_DEBUG_CFG_FILENAME,
                                (L7_char8 *)&ipMapDebugCfg, (L7_uint32)sizeof(ipMapDebugCfg_t),
                                &ipMapDebugCfg.checkSum, IPMAP_DEBUG_CFG_VER_CURRENT,
                                ipMapDebugBuildDefaultConfigData, L7_NULL);

 ipMapDebugCfg.hdr.dataChanged = L7_FALSE;
}

#endif


/* ICMP Rate limiting/Suppressing stats */

void ipMapIcmpCtlStatsShow(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nNumber of packets dropped due to ICMP Rate limiting: %u", icmpCtlStats.totalRateLimitedPkts);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nNumber of Suppressed ICMP Redirects: %u", icmpCtlStats.totalSuppressedRedirects);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nNumber of times Rate limit kicks in: %u", icmpCtlStats.rateLimitKicks);
}

/* To Clear ICMP Ratelimiting/Suppressing Stats */
void ipMapIcmpCtlStatsClear(void)
{

  icmpCtlStats.totalRateLimitedPkts =0;
  icmpCtlStats.totalSuppressedRedirects =0;
  icmpCtlStats.rateLimitKicks =0;
  ratelimitKicks = L7_FALSE;
}


#ifdef L7_NSF_PACKAGE

void ipMapNsfShow(void)
{
  printf("\nConfigurator state:  %s", ipMapCnfgrStateNames[ipMapCnfgrState]);
  if (pIpMapInfo->warmRestart)
    printf("\nLast restart was warm");
  else
    printf("\nLast restart was cold");

  if (pIpMapInfo->backupElected)
    printf("\nBackup unit exists");
  else
    printf("\nNo backup unit");

  if (pIpMapInfo->l3HwUpdateComplete)
    printf("\nL3 HW updates complete");
  else
    printf("\nL3 HW updates pending");

  if (pIpMapInfo->createStartupDone)
    printf("\nNIM create startup done");
  else
    printf("\nNIM create startup not yet finished");

  printf("\nNSF protocol pending mask:  %#x", pIpMapInfo->nsfRoutesPending);

  if (pIpMapInfo->warmRestartCompleted)
    printf("\nWarm Restarted completed set to true");
  else
      printf("\nWarm Restarted completed set to false");
}


#endif

void ipMapPortEventMaskShow(void)
{
  L7_PORT_EVENTS_t event;

  printf("\nIP MAP registered for the following NIM events:");
  for (event = L7_PORT_DISABLE; event < L7_LAST_PORT_EVENT; event++)
  {
    if (PORTEVENT_ISMASKBITSET(pIpMapInfo->nimEventMask, event))
    {
      printf("\n  %s", nimGetIntfEvent(event)); 
    }
  }
}

/* Return the name of a given NSF protocol */
L7_uchar8 *ipMapNsfProtocolName(L7_NSF_PROTOCOL_t protocol)
{
  switch (protocol)
  {
    case NSF_PROTOCOL_OSPF:   return "OSPF";
    case NSF_PROTOCOL_RIP:    return "RIP";
    case NSF_PROTOCOL_BGP:    return "BGP";
    default:                  return "Unknown";
  }
}

/* Return the names of all NSF protocols whose routes are still pending */
L7_uchar8 *ipMapNsfRoutesPendingString(void)
{
  static L7_uchar8 protoStr[64];
  L7_BOOL needComma = L7_FALSE;

  protoStr[0] = '\0';
  if (pIpMapInfo->nsfRoutesPending & NSF_PROTOCOL_OSPF)
  {
    strncat(protoStr, ipMapNsfProtocolName(NSF_PROTOCOL_OSPF), 16);
    needComma = L7_TRUE;
  }
  if (pIpMapInfo->nsfRoutesPending & NSF_PROTOCOL_RIP)
  {
    if (needComma)
      strncat(protoStr, ", ", 3);
    strncat(protoStr, ipMapNsfProtocolName(NSF_PROTOCOL_RIP), 16);
    needComma = L7_TRUE;
  }
  if (pIpMapInfo->nsfRoutesPending & NSF_PROTOCOL_BGP)
  {
    if (needComma)
      strncat(protoStr, ", ", 3);
    strncat(protoStr, ipMapNsfProtocolName(NSF_PROTOCOL_BGP), 16);
  }
  return protoStr;
}

/*********************************************************************
* @purpose  Dump the statistics of the address conflicts detected
*
* @param    none
*
* @returns  none
*
* @notes    none
*       
* @end
*********************************************************************/
void ipMapAddrConflictStatsShow()
{
  ipMapArpExtenAddrConflictStatsShow();
}

/******************************End *******************************************/

