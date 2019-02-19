/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
********************************************************************************
*
* @filename  broad_l3_debug.c
*
* @purpose   This file contains debug functions and performance tests for
*            Layer3 module
*
* @component HAPI
*
* @comments  none
*
* @create    10/1/2007
*
* @author    sdoke
*
* @end
*
*******************************************************************************/

#include "broad_l3_int.h"
#include "bcm/error.h"
#include "hpc_hw_api.h"
#include "osapi.h"

/* Debug flags */
L7_BOOL broadL3Debug = L7_FALSE;       /* trace DAPI L3 commands. sync */
L7_BOOL broadL3AsyncDebug = L7_FALSE;  /* trace Async operations */
L7_BOOL broadL3BcmxDebug = L7_FALSE;   /* trace all BCMX calls */
L7_BOOL broadL3WaitDebug = L7_FALSE;   /* trace wait times */
L7_BOOL broadL3SemaDebug = L7_FALSE;
L7_BOOL broadL3ArpDebug = 0;

/* Async and caller task ids */
static L7_uint32 broadL3DebugTaskIds[16] = {0};

/* Debug sema to write to console/buffer */
void *broadL3DebugSema;


/* Work list stats */
L7_uint32 broadL3NhopWlistCurrDepth = 0;
L7_uint32 broadL3HostWlistCurrDepth = 0;
L7_uint32 broadL3RouteWlistCurrDepth = 0;

L7_uint32 broadL3NhopWlistMaxDepth = 0;
L7_uint32 broadL3HostWlistMaxDepth = 0;
L7_uint32 broadL3RouteWlistMaxDepth = 0;


/* Trace options */
void hapiBroadL3DebugEnable(L7_BOOL dbg1,
                            L7_BOOL dbg2,
                            L7_BOOL dbg3,
                            L7_BOOL dbg4)
{
  broadL3Debug = dbg1;
  broadL3AsyncDebug = dbg2;
  broadL3BcmxDebug = dbg3;
  broadL3ArpDebug = dbg4;
}

/* Trace the Broad L3 DAPI commands - application paramaters */
void hapiBroadL3DebugDapiCmdPrint(const L7_char8 *func, DAPI_USP_t *usp,
                                  DAPI_CMD_t cmd, void *data)
{
  L7_ulong32       count = 0;
  L7_ulong32       *pIpv4 = L7_NULL, *pV4Mask = L7_NULL;
  L7_uchar8        *pMac = L7_NULL;
  L7_in6_addr_t    *pIpv6 = L7_NULL, *pV6Mask = L7_NULL;
  DAPI_ROUTING_ARP_FLAG_t arpFlags = 0;
  DAPI_ROUTING_ARP_CMD_t *arpCmd = (DAPI_ROUTING_ARP_CMD_t *)data;
  DAPI_ROUTING_MGMT_CMD_t *routeCmd = (DAPI_ROUTING_MGMT_CMD_t *)data;
  DAPI_ROUTING_INTF_MGMT_CMD_t *intfCmd = (DAPI_ROUTING_INTF_MGMT_CMD_t *)data;
  DAPI_ROUTING_ROUTE_ENTRY_t   *pNhops = L7_NULL;
  DAPI_TUNNEL_MGMT_CMD_t       *tunCmd = (DAPI_TUNNEL_MGMT_CMD_t *)data;

  HAPI_BROAD_L3_DEBUG_DECL(L7_uchar8 addr[BROAD_L3_ADDR_STR_LEN]);
  HAPI_BROAD_L3_DEBUG_DECL(L7_uchar8 mask[BROAD_L3_ADDR_STR_LEN]);

  /* Dump DAPI ARP commands */
  if (cmd == DAPI_CMD_ROUTING_ARP_ENTRY_ADD)
  {
    pIpv4 = &(arpCmd->cmdData.arpAdd.ipAddr);
    pMac = arpCmd->cmdData.arpAdd.macAddr.addr;
    arpFlags = arpCmd->cmdData.arpAdd.flags;
  }
  else if (cmd == DAPI_CMD_ROUTING_ARP_ENTRY_MODIFY)
  {
    pIpv4 = &(arpCmd->cmdData.arpModify.ipAddr);
    pMac = arpCmd->cmdData.arpModify.macAddr.addr;
    arpFlags = arpCmd->cmdData.arpModify.flags;
  }
  else if (cmd == DAPI_CMD_ROUTING_ARP_ENTRY_DELETE)
  {
    pIpv4 = &(arpCmd->cmdData.arpDelete.ipAddr);
    pMac = arpCmd->cmdData.arpDelete.macAddr.addr;
    arpFlags = arpCmd->cmdData.arpDelete.flags;
  }
  else if (cmd == DAPI_CMD_IPV6_NEIGH_ENTRY_ADD)
  {
    pIpv6 = &(arpCmd->cmdData.arpAdd.ip6Addr);
    pMac = arpCmd->cmdData.arpAdd.macAddr.addr;
    arpFlags = arpCmd->cmdData.arpAdd.flags;
  }
  else if (cmd == DAPI_CMD_IPV6_NEIGH_ENTRY_MODIFY)
  {
    pIpv6 = &(arpCmd->cmdData.arpModify.ip6Addr);
    pMac = arpCmd->cmdData.arpModify.macAddr.addr;
    arpFlags = arpCmd->cmdData.arpModify.flags;
  }
  else if (cmd == DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE)
  {
    pIpv6 = &(arpCmd->cmdData.arpDelete.ip6Addr);
    pMac = arpCmd->cmdData.arpDelete.macAddr.addr;
    arpFlags = arpCmd->cmdData.arpDelete.flags;
  }

  if (pIpv4 != L7_NULL)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "%s: USP %d/%d/%d, %s"
                  "(%02X:%02X:%02X:%02X:%02X:%02X) "
                  "flags %d", func, usp->unit, usp->slot, usp->port,
                   NTOP(L7_AF_INET, pIpv4, addr, sizeof(addr)),
                  pMac[0], pMac[1], pMac[2], pMac[3], pMac[4], pMac[5],
                  arpFlags);
  }
  else if (pIpv6 != L7_NULL)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "%s: USP %d/%d/%d, %s"
                  "(%02X:%02X:%02X:%02X:%02X:%02X) "
                  "flags %d", func, usp->unit, usp->slot, usp->port,
                   NTOP(L7_AF_INET6, pIpv6, addr, sizeof(addr)),
                  pMac[0], pMac[1], pMac[2], pMac[3], pMac[4], pMac[5],
                  arpFlags);
  }

  pIpv4 = L7_NULL;
  pIpv6 = L7_NULL;

  /* Dump DAPI route commands */
  if (cmd == DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD)
  {
    pIpv4 = &(routeCmd->cmdData.routeAdd.ipAddr);
    pV4Mask = &(routeCmd->cmdData.routeAdd.ipMask);
    count = routeCmd->cmdData.routeAdd.count;
    pNhops = &(routeCmd->cmdData.routeAdd.route);
  }
  else if (cmd == DAPI_CMD_ROUTING_ROUTE_ENTRY_MODIFY)
  {
    pIpv4 = &(routeCmd->cmdData.routeModify.ipAddr);
    pV4Mask = &(routeCmd->cmdData.routeModify.ipMask);
    count = routeCmd->cmdData.routeModify.count;
    pNhops = &(routeCmd->cmdData.routeModify.route);
  }
  else if (cmd == DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE)
  {
    pIpv4 = &(routeCmd->cmdData.routeDelete.ipAddr);
    pV4Mask = &(routeCmd->cmdData.routeDelete.ipMask);
    count = routeCmd->cmdData.routeDelete.count;
  }
  else if (cmd == DAPI_CMD_IPV6_ROUTE_ENTRY_ADD)
  {
    pIpv6 = &(routeCmd->cmdData.routeAdd.ip6Addr);
    pV6Mask = &(routeCmd->cmdData.routeAdd.ip6Mask);
    count = routeCmd->cmdData.routeAdd.count;
    pNhops = &(routeCmd->cmdData.routeAdd.route);
  }
  else if (cmd == DAPI_CMD_IPV6_ROUTE_ENTRY_MODIFY)
  {
    pIpv6 = &(routeCmd->cmdData.routeModify.ip6Addr);
    pV6Mask = &(routeCmd->cmdData.routeModify.ip6Mask);
    count = routeCmd->cmdData.routeModify.count;
    pNhops = &(routeCmd->cmdData.routeModify.route);
  }
  else if (cmd == DAPI_CMD_IPV6_ROUTE_ENTRY_DELETE)
  {
    pIpv6 = &(routeCmd->cmdData.routeDelete.ip6Addr);
    pV6Mask = &(routeCmd->cmdData.routeDelete.ip6Mask);
    count = routeCmd->cmdData.routeDelete.count;
  }
  else if ((cmd == DAPI_CMD_IPV6_INTF_ADDR_DELETE) ||
           (cmd == DAPI_CMD_IPV6_INTF_ADDR_ADD))
  {
    pIpv6 =  &(intfCmd->cmdData.rtrIntfIpAddrAddDelete.ip6Addr);
    pV6Mask =  &(intfCmd->cmdData.rtrIntfIpAddrAddDelete.ip6Mask);
  }
  else if (cmd == DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET)
  {
    pIpv4 = &(intfCmd->cmdData.rtrIntfModify.ipAddr);
    pV4Mask = &(intfCmd->cmdData.rtrIntfModify.ipMask);
  }

  if (pIpv4 != L7_NULL)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "%s: USP %d/%d/%d, %s %s count %d",
                        func, usp->unit, usp->slot, usp->port,
                        NTOP(L7_AF_INET, pIpv4, addr, sizeof(addr)),
                        NTOP(L7_AF_INET, pV4Mask, mask, sizeof(mask)),
                        count);
  }
  else if (pIpv6 != L7_NULL)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "%s: USP %d/%d/%d, %s %s count %d", 
                        func, usp->unit, usp->slot, usp->port,
                        NTOP(L7_AF_INET6, pIpv6, addr, sizeof(addr)),
                        NTOP(L7_AF_INET6, pV6Mask, mask, sizeof(mask)),
                        count);
  } 
  if (pNhops != L7_NULL)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "Next Hops:");
    for (count=0; count < L7_RT_MAX_EQUAL_COST_ROUTES; count++)
    {
      if (pNhops->equalCostRoute[count].valid == L7_TRUE)
      {
        if (pIpv4 != L7_NULL)
        {
          HAPI_BROAD_L3_DEBUG(broadL3Debug, "\t%s  USP %d:%d:%d",
                      NTOP(L7_AF_INET, &(pNhops->equalCostRoute[count].ipAddr),
                           addr, sizeof(addr)),
                       pNhops->equalCostRoute[count].usp.unit,
                       pNhops->equalCostRoute[count].usp.slot,
                       pNhops->equalCostRoute[count].usp.port);
        }
        else
        {
          HAPI_BROAD_L3_DEBUG(broadL3Debug, "\t%s  USP %d:%d:%d",
                      NTOP(L7_AF_INET6, &(pNhops->equalCostRoute[count].ip6Addr),
                            addr, sizeof(addr)),
                       pNhops->equalCostRoute[count].usp.unit,
                       pNhops->equalCostRoute[count].usp.slot,
                       pNhops->equalCostRoute[count].usp.port);
        }
      }
    }
  }

  /* Dump Tunnel commands */
  if ((cmd == DAPI_CMD_L3_TUNNEL_ADD) || (cmd == DAPI_CMD_L3_TUNNEL_DELETE))
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "%s: Mode %d, Local IP 0x%x, Remote IP %x, "
             "Nexthop IP %x, Nhop USP %d:%d:%d", func,
             tunCmd->cmdData.l3TunnelAddDelete.tunnelMode, 
             tunCmd->cmdData.l3TunnelAddDelete.localAddr.u.sa4.sin_addr.s_addr,
             tunCmd->cmdData.l3TunnelAddDelete.remoteAddr.u.sa4.sin_addr.s_addr,
             tunCmd->cmdData.l3TunnelAddDelete.nextHopAddr.u.sa4.sin_addr.s_addr,
             tunCmd->cmdData.l3TunnelAddDelete.nextHopUsp.unit,
             tunCmd->cmdData.l3TunnelAddDelete.nextHopUsp.slot,
             tunCmd->cmdData.l3TunnelAddDelete.nextHopUsp.port);
  }
  else if (cmd == DAPI_CMD_TUNNEL_CREATE)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "%s: USP %d:%d:%d, Mode %d, Local IP 0x%x, "
             "Remote IP %x, Nexthop IP %x, Nhop USP %d:%d:%d", func,
             usp->unit, usp->slot, usp->port,
             tunCmd->cmdData.tunnelCreate.tunnelMode, 
             tunCmd->cmdData.tunnelCreate.localAddr.u.sa4.sin_addr.s_addr,
             tunCmd->cmdData.tunnelCreate.remoteAddr.u.sa4.sin_addr.s_addr,
             tunCmd->cmdData.tunnelCreate.nextHopAddr.u.sa4.sin_addr.s_addr,
             tunCmd->cmdData.tunnelCreate.nextHopUsp.unit,
             tunCmd->cmdData.tunnelCreate.nextHopUsp.slot,
             tunCmd->cmdData.tunnelCreate.nextHopUsp.port);
  }
  else if (cmd == DAPI_CMD_TUNNEL_DELETE)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "%s: USP %d:%d:%d\n", func,
                        usp->unit, usp->slot, usp->port);
  }

  if (cmd == DAPI_CMD_TUNNEL_NEXT_HOP_SET)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "%s: USP %d:%d:%d, Nexthop %x, USP %d:%d:%d", 
            func, usp->unit, usp->slot, usp->port,
            tunCmd->cmdData.tunnelNextHopSet.nextHopAddr.u.sa4.sin_addr.s_addr,
            tunCmd->cmdData.tunnelNextHopSet.nextHopUsp.unit,
            tunCmd->cmdData.tunnelNextHopSet.nextHopUsp.slot,
            tunCmd->cmdData.tunnelNextHopSet.nextHopUsp.port);
  }
  else if ((cmd == DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_ADD) ||
           (cmd ==  DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_DELETE))
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "%s: USP %d:%d:%d, Mode %d, Local IP %x,"
       " Remote %x, MAC %02X:%02X:%02X:%02X:%02X:%02X", func,
       usp->unit, usp->slot, usp->port,
       tunCmd->cmdData.l3TunnelMacAddressAddDelete.tunnelMode,
       tunCmd->cmdData.l3TunnelMacAddressAddDelete.localAddr.u.sa4.sin_addr.s_addr,
       tunCmd->cmdData.l3TunnelMacAddressAddDelete.remoteAddr.u.sa4.sin_addr.s_addr,
       tunCmd->cmdData.l3TunnelMacAddressAddDelete.macAddr.addr[0],
       tunCmd->cmdData.l3TunnelMacAddressAddDelete.macAddr.addr[1],
       tunCmd->cmdData.l3TunnelMacAddressAddDelete.macAddr.addr[2],
       tunCmd->cmdData.l3TunnelMacAddressAddDelete.macAddr.addr[3],
       tunCmd->cmdData.l3TunnelMacAddressAddDelete.macAddr.addr[4],
       tunCmd->cmdData.l3TunnelMacAddressAddDelete.macAddr.addr[5]);

  }
}

void hapiBroadL3DebugArpQueryPrint(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data)
{
  L7_ulong32 entry;
  L7_uchar8 addr_str[BROAD_L3_ADDR_STR_LEN];
  DAPI_ROUTING_ARP_CMD_t *dapiCmd = (DAPI_ROUTING_ARP_CMD_t*)data;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3HostEntryQuery: USP %d:%d:%d, "
                      "NumEntries %d\n", usp->unit, usp->slot, usp->port, 
                      dapiCmd->cmdData.arpQuery.numOfEntries);

  for (entry = 0; entry < dapiCmd->cmdData.arpQuery.numOfEntries; entry++)
  {
    if (cmd == DAPI_CMD_IPV6_NEIGH_ENTRY_QUERY)
    {
      HAPI_BROAD_L3_DEBUG(broadL3Debug,
                            "*** Host %s AQF=%08X LHS=%d LHD=%d",
                            NTOP(L7_AF_INET6,
                                 &(dapiCmd->cmdData.arpQuery.ip6Addr),
                                 addr_str,
                                 sizeof(addr_str)),
                            dapiCmd->cmdData.arpQuery.arpQueryFlags[entry],
                            dapiCmd->cmdData.arpQuery.lastHitSrc[entry],
                            dapiCmd->cmdData.arpQuery.lastHitDst[entry]);
    }
    else if (cmd == DAPI_CMD_ROUTING_ARP_ENTRY_QUERY)
    {
      HAPI_BROAD_L3_DEBUG(broadL3Debug,
                            "*** Host %s AQF=%08X LHS=%d LHD=%d",
                            NTOP(L7_AF_INET,
                                 &(dapiCmd->cmdData.arpQuery.ipAddr),
                                 addr_str,
                                 sizeof(addr_str)),
                            dapiCmd->cmdData.arpQuery.arpQueryFlags[entry],
                            dapiCmd->cmdData.arpQuery.lastHitSrc[entry],
                            dapiCmd->cmdData.arpQuery.lastHitDst[entry]);
    }
  }
}


/* Stores the caller's task id */
void hapiBroadL3DebugTaskIdSet(void)
{
  L7_uint8 i, index = 255;
  L7_uint32 taskId = 0;

  osapiTaskIDSelfGet(&taskId);
  for (i=0; i<16; i++)
  {
    if (broadL3DebugTaskIds[i] == taskId)
    {
      return;
    }
    if ((index == 255) && (broadL3DebugTaskIds[i] == 0))
    {
      index = i;
    }
  }

  if (index < 16)
  {
    broadL3DebugTaskIds[index] = taskId;
  }
}

void hapiBroadL3DebugTaskIdShow(void)
{
  /* List of tasks that called in HAPI L3 cmds */
  L7_uint8 i;

  for (i=0; i<16; i++)
  {
    if (broadL3DebugTaskIds[i] != 0)
      sysapiPrintf("Task Id: 0x%x\n", broadL3DebugTaskIds[i]);
  }
}

/* Stack trace for all tasks calling into broad L3 code */
extern void osapiDebugStackTrace(L7_uint32 task_id, FILE *filePtr);
void hapiBroadL3DebugStackTrace(void)
{
  L7_uint8 i=0;

  for (i=0; i<16; i++)
  {
    if (broadL3DebugTaskIds[i] != 0)
    {
      osapiDebugStackTrace(broadL3DebugTaskIds[i], L7_NULL);
    }
  }
} 

static L7_BOOL hapiBroadL3DebugUserPrompt()
{
  L7_char8 string[4];
  sysapiPrintf("\n\n--More-- or (q)uit\n");

  if (fgets(string, 2, stdin) == (void *)0)
  {
    return L7_FALSE;
  }

  if (string[0] == 'q')
  {
    return L7_TRUE; /* Quit */
  }

  return L7_FALSE;
}

/* Show routine for Next hop table */
void hapiBroadL3DebugNhopShow(L7_uint32 arg) /* 1=failed 2=all or EgrId */
{
  L7_uchar8 count=0;
  BROAD_L3_NH_KEY_t zero;
  BROAD_L3_NH_ENTRY_t *pNhopEntry;
  BROAD_L3_MAC_ENTRY_t *pMacEntry;
  L7_uchar8 addr_str[BROAD_L3_ADDR_STR_LEN];
 
  if (arg == 0)
  {
    sysapiPrintf("Usage: hapiBroadL3DebugNhopShow(arg). Where arg is \n"
                 "\t 1 - show failed NHOP entries only\n"
                 "\t 2 - show all NHOP entries\n"
                 "\t EgressId - show the entry matching egress Id\n");
    return;
  }

  memset(&zero, 0, sizeof(zero));
  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
  sysapiPrintf ("\nHAPI Broad L3 Nexthop table: count %d\n", 
                hapiBroadL3NhopTree.count);

  pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, (void *)&zero,  AVL_EXACT);
  if (pNhopEntry == L7_NULL) 
  { 
    pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, (void *)&zero, AVL_NEXT);
  } 
  
  while(pNhopEntry != L7_NULL)
  {
    if (((arg == 1) && (pNhopEntry->rv == 0)) ||
         ((arg > 2) && (pNhopEntry->egressId != arg)))
    {
      pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, pNhopEntry, AVL_NEXT);
      continue;
    }
    

    memset(addr_str, 0, BROAD_L3_ADDR_STR_LEN);

    sysapiPrintf("\n%15s : %d/%d/%d ", NTOP(pNhopEntry->key.family,
                 &(pNhopEntry->key.addrUsp.addr), addr_str, sizeof(addr_str)),
                 pNhopEntry->key.addrUsp.usp.unit,
                 pNhopEntry->key.addrUsp.usp.slot,
                 pNhopEntry->key.addrUsp.usp.port);
     
    if (pNhopEntry->pMac != L7_NULL)
    {
      pMacEntry = pNhopEntry->pMac;
      sysapiPrintf("%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x  vid %4d",
                   pMacEntry->key.macAddr[0], pMacEntry->key.macAddr[1],
                   pMacEntry->key.macAddr[2], pMacEntry->key.macAddr[3],
                   pMacEntry->key.macAddr[4], pMacEntry->key.macAddr[5],
                   pMacEntry->key.vlanId);
    }

    sysapiPrintf(" Flags 0x%x", pNhopEntry->flags);
   
    sysapiPrintf(" Ref %d",  pNhopEntry->ref_count);
    sysapiPrintf(" Id %d",  pNhopEntry->egressId);
    sysapiPrintf(" cmd %d",  pNhopEntry->wl.cmd);
    sysapiPrintf(" rv %d",  pNhopEntry->rv);
    count++;
    
    if (count == 15)
    {
      count = 0;
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

      if (hapiBroadL3DebugUserPrompt() == L7_TRUE)
      {
        return;
      }
      HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
    }
       
    pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, pNhopEntry, AVL_NEXT);
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  sysapiPrintf("\n\n");
}


/* Show routine for ECMP table */
void hapiBroadL3DebugEcmpShow(void)
{
  BROAD_L3_ECMP_KEY_t zero;
  L7_uint8 i;
  BROAD_L3_ECMP_ENTRY_t *pEcmpEntry;
  L7_uchar8 addr_str[BROAD_L3_ADDR_STR_LEN];

  memset(&zero, 0, sizeof(zero));
  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
  pEcmpEntry = avlSearchLVL7(&hapiBroadL3EcmpTree, (void *)&zero,  AVL_EXACT);
  if (pEcmpEntry == L7_NULL)
  {
    pEcmpEntry = avlSearchLVL7(&hapiBroadL3EcmpTree, (void *)&zero, AVL_NEXT);
  }

  sysapiPrintf ("\nHAPI Broad L3 ECMP table: %d\n", hapiBroadL3EcmpTree.count);

  while(pEcmpEntry != L7_NULL)
  {
    sysapiPrintf ("\nMulitpath Egr Id: %d, ", pEcmpEntry->egressId);
    sysapiPrintf ("Ref Count: %d, ", pEcmpEntry->ref_count);
    sysapiPrintf ("Flags: 0x%x", pEcmpEntry->flags);
    sysapiPrintf(" : rv %d",  pEcmpEntry->rv);

    for (i=0; i < L7_RT_MAX_EQUAL_COST_ROUTES; i++)
    {
      if(pEcmpEntry->pNhop[i] != L7_NULL)
      {
        sysapiPrintf("\n%15s\t", NTOP(pEcmpEntry->key.family,
             &(pEcmpEntry->key.addrUsp[i].addr), addr_str, sizeof(addr_str)));

        sysapiPrintf("%d:%d:%d\t", pEcmpEntry->key.addrUsp[i].usp.unit,
                     pEcmpEntry->key.addrUsp[i].usp.slot,
                     pEcmpEntry->key.addrUsp[i].usp.port);

        sysapiPrintf("%d", pEcmpEntry->pNhop[i]->egressId); 
      }
    }

    pEcmpEntry = avlSearchLVL7(&hapiBroadL3EcmpTree, pEcmpEntry, AVL_NEXT);
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  sysapiPrintf("\n\n");
}

/* Show routine for Host table */
void hapiBroadL3DebugHostShow(L7_uint32 arg) 
{
  BROAD_L3_HOST_KEY_t zero;
  BROAD_L3_HOST_ENTRY_t *pHostEntry;
  L7_uchar8 addr_str[BROAD_L3_ADDR_STR_LEN];
  L7_uchar8 count=0;

  if (arg == 0)
  {
    sysapiPrintf("Usage: hapiBroadL3DebugHostShow(arg). Where arg is \n"
                 "\t 1 - show failed Host entries only\n"
                 "\t 2 - show all Host entries\n"
                 "\t IP - show the entry matching IPv4\n");
    return;
  }

  memset(&zero, 0, sizeof(zero));
  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
  pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, (void *)&zero,  AVL_EXACT);
  if (pHostEntry == L7_NULL)
  {
    pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, (void *)&zero, AVL_NEXT);
  }

  sysapiPrintf ("\nHAPI Broad L3 Host table: count %d\n", 
                hapiBroadL3HostTree.count);

  while(pHostEntry != L7_NULL)
  {
    if (((arg == 1) && (pHostEntry->rv == 0)) ||
        ((arg > 2)  && (pHostEntry->key.addrUsp.addr.ipv4 != arg)))
    {
      pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, pHostEntry, AVL_NEXT);
      continue;
    }
    memset(addr_str, 0, BROAD_L3_ADDR_STR_LEN);

    sysapiPrintf("\n%15s ",
                 NTOP(pHostEntry->key.family, &(pHostEntry->key.addrUsp.addr),
                      addr_str, sizeof(addr_str)));

    sysapiPrintf(" : %d/%d/%d", pHostEntry->key.addrUsp.usp.unit,
                   pHostEntry->key.addrUsp.usp.slot,
                   pHostEntry->key.addrUsp.usp.port);

    if (pHostEntry->pNhop != NULL)
    {
      sysapiPrintf(" EgrId %d",  pHostEntry->pNhop->egressId);
    }

    if (pHostEntry->pTunNhop != NULL)
    {
      sysapiPrintf(" TnlEgrId %d",  pHostEntry->pTunNhop->egressId);
    }
    sysapiPrintf(" rv %d",  pHostEntry->rv);
    sysapiPrintf(" flags %d",  pHostEntry->flags);
    sysapiPrintf(" cmd %d",  pHostEntry->wl.cmd);
    count++;

    if (count == 15)
    {
      count = 0;
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

      if (hapiBroadL3DebugUserPrompt() == L7_TRUE)
      {
        return;
      }
      HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
    }

    pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, pHostEntry, AVL_NEXT);
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  sysapiPrintf("\n\n");
}

/* Show routine for MAC table */
void hapiBroadL3DebugMacShow(L7_uint32 arg) /* match last 4 bytes of MAC */
{
  L7_uchar8 count=0;
  BROAD_L3_MAC_KEY_t zero;
  BROAD_L3_MAC_ENTRY_t *pMacEntry;

  memset(&zero, 0, sizeof(zero));
  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  sysapiPrintf("\nHAPI Broad L3 MAC table: count %d", hapiBroadL3MacTree.count);

  pMacEntry = avlSearchLVL7(&hapiBroadL3MacTree, (void *)&zero,  AVL_EXACT);
  if (pMacEntry == L7_NULL)
  {
    pMacEntry = avlSearchLVL7(&hapiBroadL3MacTree, (void *)&zero, AVL_NEXT);
  }


  while(pMacEntry != L7_NULL)
  {
    if ((arg !=0) && (arg != *(L7_uint32 *)&pMacEntry->key.macAddr[2]))
    {
      pMacEntry = avlSearchLVL7(&hapiBroadL3MacTree, pMacEntry, AVL_NEXT);
      continue;
    }

    sysapiPrintf("\n%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x ",
           pMacEntry->key.macAddr[0], pMacEntry->key.macAddr[1],
           pMacEntry->key.macAddr[2], pMacEntry->key.macAddr[3],
           pMacEntry->key.macAddr[4], pMacEntry->key.macAddr[5]);

    sysapiPrintf(" vid %4d,", pMacEntry->key.vlanId);

    printf(" %d/%d/%d,", pMacEntry->usp.unit,
            pMacEntry->usp.slot, pMacEntry->usp.port);

    if (pMacEntry->resolved == L7_TRUE)
      sysapiPrintf(", Resolved");
    else
      sysapiPrintf(", Not resolved");

    sysapiPrintf(", nhops %p", pMacEntry->nHopList);

    if (pMacEntry->pTunnelEntry != L7_NULL)
    {
      sysapiPrintf(", tunnel %p", pMacEntry->pTunnelEntry);
      if (pMacEntry->hostList != L7_NULL)
      {
        sysapiPrintf (", hosts %p\n", pMacEntry->hostList);
      }
    }
    count++;

    if (count == 15)
    {
      count = 0;
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

      if (hapiBroadL3DebugUserPrompt() == L7_TRUE)
      {
        return;
      }
      HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
    }

    pMacEntry = avlSearchLVL7(&hapiBroadL3MacTree, pMacEntry, AVL_NEXT);
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  sysapiPrintf("\n\n");
}

void hapiBroadL3DebugListNhops(BROAD_L3_NH_ENTRY_t *nHopList)
{
  BROAD_L3_NH_ENTRY_t *pNhop;

  pNhop = nHopList;

  if (pNhop == L7_NULL)
    return;

  sysapiPrintf("List of Next hops:\n");
  while (pNhop != L7_NULL)
  {
    sysapiPrintf("Id %d, Resolved %s\n", pNhop->egressId,
                 (pNhop->flags & BROAD_L3_NH_RESOLVED)? "YES":"NO");
    pNhop = pNhop->pMacNhopNext;
  }
}

/* Show routine for route table */
void hapiBroadL3DebugRouteShow(L7_uint32 arg)
{
  L7_uchar8 count=0;
  BROAD_L3_ROUTE_KEY_t zero;
  BROAD_L3_ROUTE_ENTRY_t *pRouteEntry;
  L7_uchar8 addr_str[BROAD_L3_ADDR_STR_LEN];
  L7_uchar8 mask_str[BROAD_L3_ADDR_STR_LEN];

  if (arg == 0)
  {
    sysapiPrintf("Usage: hapiBroadL3DebugRouteShow(arg). Where arg is \n"
                 "\t 1 - show failed route entries only\n"
                 "\t 2 - show all route entries\n"
                 "\t IP - show the entry matching IPv4\n");
    return;
  }

  memset(&zero, 0, sizeof(zero));

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
  pRouteEntry = avlSearchLVL7(&hapiBroadL3RouteTree, (void *)&zero,  AVL_EXACT);
  if (pRouteEntry == L7_NULL)
  {
    pRouteEntry = avlSearchLVL7(&hapiBroadL3RouteTree, (void *)&zero, AVL_NEXT);
  }

  sysapiPrintf ("\nHAPI Broad L3 Route table: count %d\n",
                hapiBroadL3RouteTree.count); 

  while(pRouteEntry != L7_NULL)
  {
    if (((arg == 1) && (pRouteEntry->rv == 0)) ||
        ((arg > 2)  && (pRouteEntry->key.ipAddr.ipv4 != arg)))
    {
      pRouteEntry = avlSearchLVL7(&hapiBroadL3RouteTree, pRouteEntry, AVL_NEXT);
      continue;
    }

    memset(addr_str, 0, BROAD_L3_ADDR_STR_LEN);
    memset(mask_str, 0, BROAD_L3_ADDR_STR_LEN);
   
    sysapiPrintf("\n%15s : %15s",
                 NTOP(pRouteEntry->key.family, &(pRouteEntry->key.ipAddr),
                      addr_str, sizeof(addr_str)),
                 NTOP(pRouteEntry->key.family, &(pRouteEntry->key.ipMask),
                      mask_str, sizeof(mask_str)));

    sysapiPrintf(" nHops %d", pRouteEntry->numNextHops);

    if (pRouteEntry->pNhop != L7_NULL)
    {
      sysapiPrintf(" EgrId %d",  pRouteEntry->pNhop->egressId);
    }
    if (pRouteEntry->pEcmp != L7_NULL)
    {
      sysapiPrintf(" MpathId %d",  pRouteEntry->pEcmp->egressId);
    }

    sysapiPrintf(" rv %d",  pRouteEntry->rv);
    sysapiPrintf(" cmd %d",  pRouteEntry->wl.cmd);
    count++;

    if (count == 15)
    {
      count = 0;
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

      if (hapiBroadL3DebugUserPrompt() == L7_TRUE)
      {
        return;
      }
      HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
    }

    pRouteEntry = avlSearchLVL7(&hapiBroadL3RouteTree, pRouteEntry, AVL_NEXT);
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

  sysapiPrintf("\n\n");
}

/* Show routine for tunnels */
void hapiBroadL3DebugTunnelShow(void)
{
  L7_uint8 i=0;
  BROAD_L3_TUNNEL_KEY_t zero;
  BROAD_L3_TUNNEL_ENTRY_t *pTunEntry;

  memset(&zero, 0, sizeof(zero));
  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
  pTunEntry = avlSearchLVL7(&hapiBroadL3TunnelTree, (void *)&zero,  AVL_EXACT);
  if (pTunEntry == L7_NULL)
  {
    pTunEntry = avlSearchLVL7(&hapiBroadL3TunnelTree, (void *)&zero, AVL_NEXT);
  }

  sysapiPrintf ("\nHAPI Broad L3 Tunnel table:\n\n");

  while(pTunEntry != L7_NULL)
  {
    sysapiPrintf ("Tunnel mode %d,", pTunEntry->key.tunnelMode);
    sysapiPrintf ("Local IP 0x%x,", 
                  osapiNtohl(pTunEntry->key.localAddr.u.sa4.sin_addr.s_addr));
    sysapiPrintf ("Remote IP 0x%x\n", 
                  osapiNtohl(pTunEntry->key.remoteAddr.u.sa4.sin_addr.s_addr));

    sysapiPrintf ("Tunnel L3Id %d,", pTunEntry->bcmL3IntfId);
    sysapiPrintf ("Has Initiator %d,", pTunEntry->hasInitiator);
    sysapiPrintf ("Has Terminator %d,", pTunEntry->hasTerminator);
    sysapiPrintf ("Error code %d,", pTunEntry->rv);

    if (pTunEntry->pXportNhop != L7_NULL)
    {
      sysapiPrintf ("Xport EgrId %d ", pTunEntry->pXportNhop->egressId);
    }
    if (pTunEntry->pTunnelNhop != L7_NULL)
    {
      sysapiPrintf ("Tunnel EgrId %d", pTunEntry->pTunnelNhop->egressId);
    }

 
    if (pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_6TO4)
    {
      sysapiPrintf ("\n6to4 Nexthops: \n");
      for (i=0; i < HAPI_BROAD_L3_MAX_6TO4_NHOP; i++)
      {
        if (pTunEntry->tun6to4Nhops[i].pV6Nhop != L7_NULL)
        {
          sysapiPrintf("%d: V6 Egress ID %d ", i, 
                       pTunEntry->tun6to4Nhops[i].pV6Nhop->egressId);

          if (pTunEntry->tun6to4Nhops[i].pV4Nhop != L7_NULL)
          {
            sysapiPrintf(": V4 Egress ID %d ",  
                         pTunEntry->tun6to4Nhops[i].pV4Nhop->egressId);
          }
          else
            sysapiPrintf(": V4 Egress ID (null) ");

          sysapiPrintf(": Tunnel id %d\n", pTunEntry->tun6to4Nhops[i].relayTunnelId);
        }
      }
    }
 
    sysapiPrintf("\n\n");
    pTunEntry = avlSearchLVL7(&hapiBroadL3TunnelTree, pTunEntry, AVL_NEXT);
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  sysapiPrintf("\n\n");
}

void hapiBroadL3DebugListHosts(BROAD_L3_HOST_ENTRY_t *pHostList)
{
  BROAD_L3_HOST_ENTRY_t *pHost;

  pHost = pHostList;

  if (pHost == L7_NULL)
    return;

  sysapiPrintf("List of Hosts:\n");
  while (pHost != L7_NULL)
  {
    sysapiPrintf("IP 0x%x\n", pHost->key.addrUsp.addr.ipv4);
    pHost = pHost->pMacHostNext;
  }
}


void hapiBroadL3DebugNhopWListShow(void)
{
  L7_uint8 i=0;
  BROAD_L3_NH_ENTRY_t *pNhopEntry;

  if (hapiBroadL3NhopListHead != L7_NULL)
  {
    pNhopEntry = hapiBroadL3NhopListHead;
    do
    {
      sysapiPrintf("\n %d : EgrId %d, Cmd %d", i, pNhopEntry->egressId, pNhopEntry->wl.cmd);
      i++;
      pNhopEntry = pNhopEntry->wl.next;
    } while(pNhopEntry != L7_NULL);
  }
  else
    sysapiPrintf("\nNext hop WL is empty");

  sysapiPrintf("\n");
}

void hapiBroadL3DebugWListShow(void)
{
  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  hapiBroadL3DebugNhopWListShow();

  if (hapiBroadL3EcmpListHead != L7_NULL)
     sysapiPrintf("\nEcmp Wlist has work\n");
  else
     sysapiPrintf("\nEcmp Wlist is empty\n");

  if (hapiBroadL3HostListHead != L7_NULL)
     sysapiPrintf("\nHost Wlist has work\n");
  else
     sysapiPrintf("\nHost Wlist is empty\n");

  if (hapiBroadL3RouteListHead != L7_NULL)
     sysapiPrintf("\nRoute Wlist has work\n");
  else
     sysapiPrintf("\nRoute Wlist is empty\n");

  if (hapiBroadL3TunnelListHead != L7_NULL)
     sysapiPrintf("\nTunnel Wlist has work\n");
  else
     sysapiPrintf("\nTunnel Wlist is empty\n");

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
}

/* Show internal broad l3 stats - Wlist depths and AVL tree counts */
void hapiBroadL3StatsShow(void)
{
  /* Current depth and Max depth for each work list */
  sysapiPrintf("\nWork list depths:\n");

  sysapiPrintf("Nhop Wlist Curr Depth: %d\n", broadL3NhopWlistCurrDepth);
  sysapiPrintf("Nhop Wlist Max Depth: %d\n", broadL3NhopWlistMaxDepth);

  sysapiPrintf("Host Wlist Curr Depth: %d\n", broadL3HostWlistCurrDepth);
  sysapiPrintf("Host Wlist Max Depth: %d\n", broadL3HostWlistMaxDepth);

  sysapiPrintf("Route Wlist Curr Depth: %d\n", broadL3RouteWlistCurrDepth);
  sysapiPrintf("Route Wlist Max Depth: %d\n", broadL3RouteWlistMaxDepth);

  sysapiPrintf("Zero count nexthops pending : %s\n",
               hapiBroadL3ProcessZeroCountNhops ? "Yes": "No");

  /* Number of nodes in each AVL tree */
  sysapiPrintf("\nAVL tree node counts:\n");

  sysapiPrintf("Nhop AVL tree count: %d\n", hapiBroadL3NhopTree.count);
  sysapiPrintf("Ecmp AVL tree count: %d\n", hapiBroadL3EcmpTree.count);
  sysapiPrintf("Host AVL tree count: %d\n", hapiBroadL3HostTree.count);
  sysapiPrintf("MAC AVL tree count: %d\n",  hapiBroadL3MacTree.count);
  sysapiPrintf("Route AVL tree count: %d\n", hapiBroadL3RouteTree.count);
  sysapiPrintf("Tunnel AVL tree count: %d\n", hapiBroadL3TunnelTree.count);
}

/* Reset internal broad l3 stats - work list depths */
void hapiBroadL3StatsReset(void)
{
  broadL3NhopWlistMaxDepth = 0;
  broadL3HostWlistMaxDepth = 0;
  broadL3RouteWlistMaxDepth = 0;
}

extern void _bcm_l3_sw_dump(int unit);
void hapiBroadL3DebugBcmSwDump(int unit)
{
  /* _bcm_l3_sw_dump(unit); */
}

void hapiBroadL3Help(void)
{
  sysapiPrintf("\nhapiBroadL3StatsShow: stats for worklists and AVL trees\n");
  sysapiPrintf("hapiBroadL3HwStatsShow: stats for hardware L3 operations\n");
  sysapiPrintf("hapiBroadL3HwStatsReset: reset stats for hardware L3 operations\n");
  sysapiPrintf("hapiBroadL3DebugBulkShow: show bulk operation limits\n");
  sysapiPrintf("hapiBroadL3DebugNhopShow: show next hop entries\n");
  sysapiPrintf("hapiBroadL3DebugHostShow: show host entries\n");
  sysapiPrintf("hapiBroadL3DebugRouteShow: show route entries\n");
  sysapiPrintf("hapiBroadL3DebugMacShow: show MAC entries\n");
  sysapiPrintf("hapiBroadL3DebugEcmpShow: show ECMP entries\n");
  sysapiPrintf("hapiBroadL3DebugTunnelShow: show tunnel entries\n");
  sysapiPrintf("hapiBroadL3CacheStats: show L3 cache stats\n");
  sysapiPrintf("hapiBroadL3DebugStackTrace: stack trace for all L3 tasks\n");
  sysapiPrintf("hapiBroadL3DebugRoutePerfTest: Measure L3 route performance\n");
  sysapiPrintf("hapiBroadL3DebugHostPerfTest: Measure L3 host performance\n"); 
}

void myDump(void)
{

  hapiBroadL3DebugNhopShow(2);
  hapiBroadL3DebugHostShow(2);
  hapiBroadL3DebugRouteShow(2);
  hapiBroadL3DebugMacShow(0);
  hapiBroadL3DebugEcmpShow();
  hapiBroadL3DebugTunnelShow();
}

void hapiBroadL3DebugNhopCheck(void)
{
  BROAD_L3_NH_KEY_t zero;
  BROAD_L3_NH_ENTRY_t     *pNhopEntry;
  bcmx_l3_egress_t         egrObj;
  int rv;

  memset(&zero, 0, sizeof(zero));
  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
  pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, (void *)&zero,  AVL_EXACT);
  if (pNhopEntry == L7_NULL)
  {
    pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, (void *)&zero, AVL_NEXT);
  }

  /* Check whether the nhops are in hardware or not */
  while(pNhopEntry != L7_NULL)
  {
    if (pNhopEntry->egressId  != HAPI_BROAD_INVALID_L3_INTF_ID)
    {
      rv = bcmx_l3_egress_get(pNhopEntry->egressId, &egrObj);
      if (rv != BCM_E_NONE)
      {
        sysapiPrintf("hapiBroadL3DebugNhopCheck: Failed for %d, rv %d (%s)\n",
                     pNhopEntry->egressId, rv, bcm_errmsg(rv));
      }
    }
  
    pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, pNhopEntry, AVL_NEXT);
  }

  /* Check other way too. Traverse nhops in hardware and check in AVL tree. TBD */
  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
}

/* Bulk operation limits */
void hapiBroadL3DebugBulkShow(void)
{
#ifdef L7_STACKING_PACKAGE
  L7_uint32 maxRpcLen = hpcHardwareRpcMaxMessageLengthGet();

  sysapiPrintf("\nSize of bcmx_l3_route_t %d, max routes/rpc %d\n",
               sizeof(bcmx_l3_route_t), maxRpcLen/sizeof(bcmx_l3_route_t));

  sysapiPrintf("Size of bcmx_l3_host_t %d, max hosts/rpc %d\n",
               sizeof(bcmx_l3_host_t), maxRpcLen/sizeof(bcmx_l3_host_t));

  sysapiPrintf("Size of bcmx_l3_egress_t %d, max egress objs/rpc %d\n",
               sizeof(bcmx_l3_egress_t), maxRpcLen/sizeof(bcmx_l3_egress_t));
#endif
}

/* Reset hardware stats */
void hapiBroadL3HwStatsReset(void)
{
  memset(&broadL3HwRouteStats, 0, sizeof(broadL3HwRouteStats));
  memset(&broadL3HwHostStats, 0, sizeof(broadL3HwHostStats));
  memset(&broadL3HwNhopStats, 0, sizeof(broadL3HwNhopStats));
  memset(&broadL3HwEcmpStats, 0, sizeof(broadL3HwEcmpStats));
}

/* Stats for hardware operations */
void hapiBroadL3HwStatsShow(void)
{
  sysapiPrintf("\nRoute Hardware stats:\n");
  sysapiPrintf("  Successul adds    = %d\n", broadL3HwRouteStats.total_adds);
  sysapiPrintf("  Failed adds       = %d\n", broadL3HwRouteStats.total_add_failures);
  sysapiPrintf("  Successul mods    = %d\n", broadL3HwRouteStats.total_mods);
  sysapiPrintf("  Failed mods       = %d\n", broadL3HwRouteStats.total_mod_failures);
  sysapiPrintf("  Successul Deletes = %d\n", broadL3HwRouteStats.total_dels);
  sysapiPrintf("  Failed dels       = %d\n", broadL3HwRouteStats.total_del_failures);
  sysapiPrintf("  Current failures  = %d\n", broadL3HwRouteStats.current_add_failures);

  sysapiPrintf("\nHost Hardware stats:\n");
  sysapiPrintf("  Successul adds    = %d\n", broadL3HwHostStats.total_adds);
  sysapiPrintf("  Failed adds       = %d\n", broadL3HwHostStats.total_add_failures);
  sysapiPrintf("  Successul mods    = %d\n", broadL3HwHostStats.total_mods);
  sysapiPrintf("  Failed mods       = %d\n", broadL3HwHostStats.total_mod_failures);
  sysapiPrintf("  Successul Deletes = %d\n", broadL3HwHostStats.total_dels);
  sysapiPrintf("  Failed dels       = %d\n", broadL3HwHostStats.total_del_failures);
  sysapiPrintf("  Current failures  = %d\n", broadL3HwHostStats.current_add_failures);

  sysapiPrintf("\nNhop Hardware stats:\n");
  sysapiPrintf("  Successul adds    = %d\n", broadL3HwNhopStats.total_adds);
  sysapiPrintf("  Failed adds       = %d\n", broadL3HwNhopStats.total_add_failures);
  sysapiPrintf("  Successul mods    = %d\n", broadL3HwNhopStats.total_mods);
  sysapiPrintf("  Failed mods       = %d\n", broadL3HwNhopStats.total_mod_failures);
  sysapiPrintf("  Successul Deletes = %d\n", broadL3HwNhopStats.total_dels);
  sysapiPrintf("  Failed dels       = %d\n", broadL3HwNhopStats.total_del_failures);
  sysapiPrintf("  Current failures  = %d\n", broadL3HwNhopStats.current_add_failures);

  sysapiPrintf("\nEcmp Hardware stats:\n");
  sysapiPrintf("  Successul adds    = %d\n", broadL3HwEcmpStats.total_adds);
  sysapiPrintf("  Failed adds       = %d\n", broadL3HwEcmpStats.total_add_failures);
  sysapiPrintf("  Successul mods    = %d\n", broadL3HwEcmpStats.total_mods);
  sysapiPrintf("  Failed mods       = %d\n", broadL3HwEcmpStats.total_mod_failures);
  sysapiPrintf("  Successul Deletes = %d\n", broadL3HwEcmpStats.total_dels);
  sysapiPrintf("  Failed dels       = %d\n", broadL3HwEcmpStats.total_del_failures);
  sysapiPrintf("  Current failures  = %d\n", broadL3HwEcmpStats.current_add_failures);
}


/******************************************************************************
*                      DEBUG PERFORMANCE TESTS                                *
******************************************************************************/

/* Route performance test */
/* Sample execution:
 *
 * To add and delete 1000 routes with nexthop 1.0.0.2 on 1/0/10 routing intf
 * "devs hapiBroadL3DebugRoutePerfTest(1000,0x1000002,1,0,9,2)"
 */
L7_uint32
hapiBroadL3DebugRoutePerfTest(L7_uint32 nroutes,     /* Num of routes to add/del */
                                 L7_uint32 nhop,     /* v4 next hop IP address */
                                 L7_uint32 unit,     /* USP of routing interface */
                                 L7_uint32 slot,
                                 L7_uint32 port,
                                 L7_uint32 addOrDel) /* 0=add, 1=del, 2=add+del */
{
  extern DAPI_t *dapi_g;
  L7_long32 i;
  L7_clocktime tStart, tCurr;
  DAPI_USP_t usp;
  DAPI_ROUTING_MGMT_CMD_t data;

  if ((nroutes == 0) || (nhop == 0))
  {
    sysapiPrintf("usage: hapiBroadL3DebugRouteAddPerfTest(nroutes, nhop, unit, "
                 "slot, port, addOrDel(0=add only, 1=del only, 2=add + del)\n");
    return 1;
  }

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;
  if (isValidUsp(&usp, dapi_g) != L7_TRUE)
  {
    sysapiPrintf("Invalid USP %d:%d:%d\n", unit, slot, port);
    return 1;
  }

  if (addOrDel == 1) /* Delete only */
     goto route_del;
  
  memset(&data, 0, sizeof(data));
  data.cmdData.routeAdd.getOrSet = DAPI_CMD_SET;
  data.cmdData.routeAdd.ipAddr = 0x0C000100;
  data.cmdData.routeAdd.ipMask = 0xFFFFFF00;
  data.cmdData.routeAdd.route.equalCostRoute[0].valid = L7_TRUE;
  data.cmdData.routeAdd.route.equalCostRoute[0].ipAddr = nhop;
  data.cmdData.routeAdd.route.equalCostRoute[0].usp = usp;
  data.cmdData.routeAdd.count = 1;
  
  /*
   * Run through adding a bunch of routes, and give an indication for
   * each 100 routes added how much time was spent.  Finally, give the
   * total elapsed time.
   */
  sysapiPrintf("Installing %d routes\n", nroutes);
  osapiClockTimeRaw(&tStart);
  for (i = 0; i < nroutes; ++i)
  {
    hapiBroadL3RouteEntryAdd(&usp, DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD,
                             &data, dapi_g);
    data.cmdData.routeAdd.ipAddr += 0x00000100;

  }
  
  /* Wait for async task to finish processing all the work lists */
  hapiBroadL3AsyncWait(&usp, DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD);

  osapiClockTimeRaw(&tCurr);
  if (tStart.nanoseconds > tCurr.nanoseconds) {
    tCurr.seconds -= 1;
    tCurr.nanoseconds += 1000000000;
  }
  sysapiPrintf("Time expired after adding %d routes = %d.%9.9d seconds\n",
         nroutes,
         tCurr.seconds - tStart.seconds, tCurr.nanoseconds - tStart.nanoseconds);


  if (addOrDel == 0)  /* Add only */
    return 0;

route_del:

  /*
   * Run through removing the same routes, and give an indication for
   * each 100 routes removed how much time was spent.  Finally, give
   * the total elapsed time.
   */
  memset(&data, 0, sizeof(data));
  data.cmdData.routeDelete.getOrSet = DAPI_CMD_SET;
  data.cmdData.routeDelete.ipAddr = 0x0C000100;
  data.cmdData.routeDelete.ipMask = 0xFFFFFF00;
  data.cmdData.routeDelete.count = 1;
  sysapiPrintf("Removing %d routes\n", nroutes);
  osapiClockTimeRaw(&tStart);
  for (i = 0; i < nroutes; ++i)
  {
    hapiBroadL3RouteEntryDelete(&usp,
                                     DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE,
                                     &data, dapi_g);
    data.cmdData.routeDelete.ipAddr += 0x00000100;
  }

  /* Wait for async task to finish processing all the work lists */
  hapiBroadL3AsyncWait(&usp, DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE);

  osapiClockTimeRaw(&tCurr);
  if (tStart.nanoseconds > tCurr.nanoseconds) {
    tCurr.seconds -= 1;
    tCurr.nanoseconds += 1000000000;
  }
  sysapiPrintf("Time expired after removing %d routes = %d.%9.9d seconds\n",
         nroutes,
         tCurr.seconds - tStart.seconds, tCurr.nanoseconds - tStart.nanoseconds);
  sysapiPrintf("Done\n");
  return 0;
}


/* Host performace test */
/* Sample execution:
 *
 * To add and delete 1000 hosts starting from 1.0.0.2 on 1/0/10 routing intf
 * "devs hapiBroadL3DebugHostPerfTest(1000,0x1000002,1,0,9,2)"
 */

L7_uint32
hapiBroadL3DebugHostPerfTest(L7_uint32 nhosts,
                             L7_uint32 ip,
                             L7_uint32 unit,
                             L7_uint32 slot,
                             L7_uint32 port,
                             L7_uint32 addOrDel) /* 0=add, 1=del, 2=add+del */
{
  extern DAPI_t *dapi_g;
  L7_long32 i;
  L7_clocktime tStart, tCurr;
  DAPI_USP_t usp;
  DAPI_ROUTING_ARP_CMD_t data;
  L7_uchar8 mac[6] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5};

  if ((nhosts == 0) || (ip == 0))
  {
    sysapiPrintf("usage: hapiBroadL3DebugHostPerfTest(nhosts, ip, unit, "
           "slot, port, addOrDel(0 = add only, 1 = del only, 2 = add+del)\n");
    return 1;
  }

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;

  if (isValidUsp(&usp, dapi_g) != L7_TRUE)
  {
    sysapiPrintf("Invalid USP %d:%d:%d\n", unit, slot, port);
    return 1;
  }

  if (addOrDel == 1) /* Delete only */
     goto host_del;
  
  memset(&data, 0, sizeof(data));
  data.cmdData.arpAdd.getOrSet = DAPI_CMD_SET;
  data.cmdData.arpAdd.flags = DAPI_ROUTING_ARP_DEST_KNOWN;
  data.cmdData.arpAdd.ipAddr = ip;
  memcpy(data.cmdData.arpAdd.macAddr.addr, mac, sizeof(mac));
  
  /*
   * Run through adding a bunch of hosts, and give an indication for
   * each 200 hosts added how much time was spent.  Finally, give the
   * total elapsed time.
   */
  sysapiPrintf("Installing %d hosts\n", nhosts);
  osapiClockTimeRaw(&tStart);
  for (i = 0; i < nhosts; ++i)
  {
    hapiBroadL3HostEntryAdd(&usp, DAPI_CMD_ROUTING_ARP_ENTRY_ADD,
                             &data, dapi_g);
    data.cmdData.arpAdd.ipAddr += 0x00000001;
    (*((L7_uint32 *)&data.cmdData.arpAdd.macAddr.addr[2]))++; 
  }

  /* Wait for async task to finish processing all the work lists */
  hapiBroadL3AsyncWait(&usp, DAPI_CMD_ROUTING_ARP_ENTRY_ADD);

  osapiClockTimeRaw(&tCurr);
  if (tStart.nanoseconds > tCurr.nanoseconds) {
    tCurr.seconds -= 1;
    tCurr.nanoseconds += 1000000000;
  }
  sysapiPrintf("Time expired after adding %d hosts = %d.%9.9d seconds\n",
         nhosts,
         tCurr.seconds - tStart.seconds, tCurr.nanoseconds - tStart.nanoseconds);


  if (addOrDel == 0)  /* Add only */
    return 0;

host_del:

  /*
   * Run through removing the same hosts, and give an indication for
   * each 200 hosts removed how much time was spent.  Finally, give
   * the total elapsed time.
   */
  memset(&data, 0, sizeof(data));
  data.cmdData.arpDelete.getOrSet = DAPI_CMD_SET;
  data.cmdData.arpDelete.ipAddr = ip;
  memcpy(data.cmdData.arpDelete.macAddr.addr, mac, sizeof(mac));

  sysapiPrintf("Removing %d hosts\n", nhosts);
  osapiClockTimeRaw(&tStart);
  for (i = 0; i < nhosts; ++i)
  {
    hapiBroadL3HostEntryDelete(&usp,
                               DAPI_CMD_ROUTING_ARP_ENTRY_DELETE,
                               &data, dapi_g);
    data.cmdData.arpDelete.ipAddr += 0x00000001;
    (*((L7_uint32 *)&data.cmdData.arpDelete.macAddr.addr[2]))++; 
  }

  /* Wait for async task to finish processing all the work lists */
  hapiBroadL3AsyncWait(&usp, DAPI_CMD_ROUTING_ARP_ENTRY_DELETE);

  osapiClockTimeRaw(&tCurr);
  if (tStart.nanoseconds > tCurr.nanoseconds) {
    tCurr.seconds -= 1;
    tCurr.nanoseconds += 1000000000;
  }
  sysapiPrintf("Time expired after removing %d hosts = %d.%9.9d seconds\n",
         nhosts,
         tCurr.seconds - tStart.seconds, tCurr.nanoseconds - tStart.nanoseconds);
  sysapiPrintf("Done\n");
  return 0;
}

