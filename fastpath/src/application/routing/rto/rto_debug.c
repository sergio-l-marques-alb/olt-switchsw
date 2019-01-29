/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rto.c
*
* @purpose The rto component maintains a database of all routes in the
*          system.
*
* @component Routing Table Object for IP Mapping Layer
*
* @comments none
*
* @create 03/22/2001
*
* @author alt
* @end
*
**********************************************************************/


#include "l7_common.h"
#include "usmdb_util_api.h"
#include "l7_common_l3.h"
#include "osapi.h"
#include "l3end_api.h"
#include "rto_api.h"
#include "rto.h"
#include "avl_api.h"
#include "rtiprecv.h"
#include "l7_ip_api.h"
#include "rng_api.h"
#include "osapi_support.h"

extern radixTree_t       rtoRouteTreeData;
extern rto_stats_t       rtoStats;

extern L7_uchar8 *rtoProtoNames[RTO_LAST_PROTOCOL];
extern L7_uint32 rtoTraceFlags;
extern L7_uint32 rtoTracePfx;
extern osapiRWLock_t rtoRwLock;

void rtoRouteShow(L7_uint32 detail, L7_uint32 proto, L7_BOOL bestOnly, L7_BOOL tentOnly);
void rtoTraceFlagsSet(L7_uint32 flags);
void rtoTracePfxSet(L7_uint32 pfx);
rtoRouteData_t *rtoFirstRouteGet(L7_uint32 lookupType);


static L7_uchar8 * rtoLongToIP (L7_uint32 addr, L7_uchar8 * buf)
{
  sprintf (buf, "%d.%d.%d.%d",
           (addr >> 24),
           (addr >> 16) & 0x000000ff,
           (addr >> 8) & 0x000000ff,
           addr & 0x000000ff);

  return buf;
}

/* Used only to get functions in this file to be linked in */
void rtoDebugInit(void)
{
    return;
}

/*********************************************************************
* @purpose  Set the RTO debug trace flag.
*
* @param    traceFlag  @b{(input)}  The new value for the RTO trace flag
*
* @notes
*
* @end
*********************************************************************/
void rtoTraceFlagsSet(L7_uint32 flags)
{
    rtoTraceFlags = flags;
}


void rtoCkptDump(L7_uint32 detail, L7_uint32 proto)
{
  rtoRouteShow(detail, proto, L7_TRUE, L7_TRUE);
}

L7_uchar8 *rtoRouteFlagsToString(rtoRouteInfo_t *routeInfo)
{
  /* Return value points to this buffer. Not thread safe. Just for debug. */
  static L7_uchar8 flagString[32];

  flagString[0] = '\0';

  if (routeInfo->flags & RTO_BEST_ROUTE_NODE)
    strcat(flagString, "B");
  if (routeInfo->flags & RTO_ECMP_TRUNC)
    strcat(flagString, "E");
  if (routeInfo->flags & RTO_REJECT_ROUTE)
    strcat(flagString, "R");
  if (routeInfo->flags & RTO_TENTATIVE_ROUTE)
    strcat(flagString, "T");

  return flagString;
}

/*********************************************************************
* @purpose  Specify a prefix to trace.
*
* @param    pfx  @b{(input)}  Network prefix to trace (host bits 0)
*
* @notes
*
* @end
*********************************************************************/
void rtoTracePfxSet(L7_uint32 pfx)
{
    rtoTracePfx = pfx;
}

void rtoTracePfxNone(void)
{
  rtoTracePfx = RTO_TRACE_PFX_NONE;
}

/*********************************************************************
*
* @purpose  Show Route Table.
*
*              detail - 0 - show stats only
*                       1 - show all routes
*
*              proto - limit display to routes for this protocol (L7_RTO_PROTOCOL_INDICES_t)
*                      0 to display all
*              bestOnly - only display best routes
*              tentOnly - only display tentative (checkpointed) routes
*
* @notes    This is a debug function to print routing table.
*
* @end
*********************************************************************/
void rtoRouteShow(L7_uint32 detail, L7_uint32 proto, L7_BOOL bestOnly, L7_BOOL tentOnly)
{
    L7_uint32 i;
    rtoRouteData_t *pData;
    rtoRouteInfo_t *routeInfo;
    rtoNextHop_t *nextHop;
    L7_uchar8 buf[30];
    rtoRouteData_t tempData;

    if (osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
    {
      sysapiPrintf("rtoRouteShow could not take semaphore\n");
      return;
    }

    sysapiPrintf("\n");
    sysapiPrintf("route_entries = %u,  radix_entries = %u,  best_entries = %u\n",
                 (unsigned int)rtoStats.route_entries,
                 (unsigned int)rtoStats.radix_entries,
                 (unsigned int)rtoStats.best_entries);

    sysapiPrintf("tot_adds = %u, tot_dels = %u, tot_mods = %u, "
                 "\ndup_adds = %u, bad_adds = %u, "
                 "bad_dels = %u, bad_mods = %u, bad_lookups = %u\n",
                 (unsigned int)rtoStats.tot_adds,
                 (unsigned int)rtoStats.tot_dels,
                 (unsigned int)rtoStats.tot_mods,
                 (unsigned int)rtoStats.dup_adds,
                 (unsigned int)rtoStats.bad_adds,
                 (unsigned int)rtoStats.bad_dels,
                 (unsigned int)rtoStats.bad_mods,
                 (unsigned int)rtoStats.bad_lookups);

    sysapiPrintf("reserved_locals: %u, active_locals: %u\n",
                 rtoStats.reserved_locals, rtoStats.active_locals);
    sysapiPrintf("pending additions: %u\n",
                 rtoStats.reserved_locals - rtoStats.active_locals);

    sysapiPrintf("\ncheckpoint routes confirmed by post failover route:  %u", rtoStats.ckpt_conf);
    sysapiPrintf("\nnumber of tentative routes:  %u", rtoStats.tentative_routes);

    sysapiPrintf("\nNumber of next hops in use:  %u", rtoStats.nextHopCount);
    sysapiPrintf("\nNext hop high water:  %u", rtoStats.nextHopHighWater);

    if (detail)
    {
      sysapiPrintf("\n\nNetwork          NetMask          Proto             Flags   Pref   Gateway        IfNum\n");
  
      memset(&tempData, 0, sizeof(rtoRouteData_t));
      i = 0;
      pData = radixLookupNode(&rtoRouteTreeData, &tempData.network, &tempData.netmask, L7_RN_EXACT);
      if (pData == L7_NULL)
      {
          pData = radixLookupNode (&rtoRouteTreeData, &tempData.network, &tempData.netmask, L7_RN_GETNEXT);
      }
      while (pData != L7_NULL)
      {
          routeInfo = pData->nextRouteInfo;
          tempData.network = pData->network;
          tempData.netmask = pData->netmask;
          i++;
  
          sysapiPrintf("--------------------------------------------------------------------------------------------\n");
          while (routeInfo)
          {
            if ((!proto || (proto == routeInfo->protocol)) &&
                (!bestOnly || (routeInfo->flags & RTO_BEST_ROUTE_NODE)) &&
                (!tentOnly || (routeInfo->flags & RTO_TENTATIVE_ROUTE)))
            {
                sysapiPrintf("%-15.15s  ", rtoLongToIP(osapiNtohl(pData->network.addr), buf));
                sysapiPrintf("%-15.15s  ", rtoLongToIP(osapiNtohl(pData->netmask.addr), buf));
                sysapiPrintf("%-15.15s  ", rtoProtoNames[routeInfo->protocol]);
                sysapiPrintf("%5s", rtoRouteFlagsToString(routeInfo));
  
                sysapiPrintf("%5u  ", routeInfo->preference1);
  
                /* For each route info print out all the next hops */
                nextHop = routeInfo->nextHops;
                while (nextHop)
                {
                    if (nextHop != routeInfo->nextHops)
                        sysapiPrintf("%-61s  ",  "");
                    sysapiPrintf("%-15.15s  ",
                                     rtoLongToIP(nextHop->nextHopIP, buf));
  
                    sysapiPrintf(" %3.1u   \n", (unsigned int)nextHop->intIfNum);
                    nextHop = nextHop->nextNh;
                }
            }
            routeInfo = routeInfo->next;
          }
          tempData.network = pData->network;
          tempData.netmask = pData->netmask;
  
  
          /* Give others a chance every 15 routes */
          if(!(i % 15))
          {
            if (osapiReadLockGive(rtoRwLock) == L7_SUCCESS)
            {
              if (osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
              {
                sysapiPrintf("rtoRouteShow could not take semaphore\n");
                return;
              }
            }
  
            /* we need to lookup as we released the lock, so the current node may have
             * changed in unexpected ways, or even disappeared!
             */
            pData = radixLookupNode (&rtoRouteTreeData, &tempData.network, &tempData.netmask, L7_RN_GETNEXT_ASCEND);
          }
          else
          {
            /* we can use the current node to move to the next node without a lookup as
             * our lock guarantees the tree has not changed since we got pData last.
             * This is more efficient than doing a lookup, but can only be used with
             * GET_NEXT style lookups as its natural for the radix tree to return routes
             * in LPM order (longest prefix first)
             */
            pData = radixGetNextEntry(&rtoRouteTreeData, pData);
          }
  
  
      } /* End while pData != NULL */
    }

    if (osapiReadLockGive(rtoRwLock
                          ) != L7_SUCCESS)
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
                "Error: rto.c osapiSemaGive\n");
}


void rtoDebugDumpTree()
{
  osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER);
  sysapiPrintf("Printing RTO radix tree nodes count %d\n", rtoRouteTreeData.count);
  radixDumpTree(&rtoRouteTreeData);
  osapiReadLockGive(rtoRwLock);
}

void dumptree(struct l7_radix_node *x);

/*********************************************************************
*
* @purpose  Prints all tree nodes to the console comma delimeted
*
*
* @end
*********************************************************************/
void rtoDebugTree()
{
  osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER);
  dumptree(rtoRouteTreeData.mask_rnhead.rnh_treetop);
  osapiReadLockGive(rtoRwLock);
}

/*********************************************************************
*
* @purpose  Walk the free mask list pool
*
*
* @end
*********************************************************************/
L7_uint32 rtoDebugWalkMkList()
{
  L7_uint32 i = 0;
  struct    l7_radix_mask * ptr;
  osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER);
  ptr = rtoRouteTreeData.mask_rnhead.rn_mkfreelist;
  while(ptr)
  {
    i++;
    ptr = ptr->rm_mklist;
  }
  osapiReadLockGive(rtoRwLock);

  return i;
}

/*********************************************************************
*
* @purpose  Walk the free mask nodes pool
*
*
* @end
*********************************************************************/
L7_uint32 rtoDebugWalkMkNodes()
{
  L7_uint32 i = 0;
  void * ptr;
  L7_uint32 offset_next;

  osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER);
  ptr = rtoRouteTreeData.mask_rnhead.rn_masknodefreelist;
  offset_next = RADIX_MASK_NODE_ITEM_OFFSET_NEXT(rtoRouteTreeData.mask_rnhead.max_keylen);
  while (ptr)
  {
    ptr = (void*)(*((L7_uint32*)((char*)ptr+offset_next)));
    i++;
  }
  osapiReadLockGive(rtoRwLock);

  return i;
}

/*********************************************************************
*
* @purpose  Walk the free data node pool
*
*
* @end
*********************************************************************/
L7_uint32 rtoDebugWalkDataNodes()
{
  L7_uint32 i = 0;
  void * ptr;
  L7_uint32 offset_next;

  osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER);
  offset_next = rtoRouteTreeData.offset_next;
  ptr = rtoRouteTreeData.rn_datanodefreelist;

  while (ptr)
  {
    ptr=(void*)(*((L7_uint32*)((char*)ptr+offset_next)));
    i++;
  }
  osapiReadLockGive(rtoRwLock);

  return i;
}

/*********************************************************************
*
* @purpose  Tests the radix LPM lookup
*
*
* @end
*********************************************************************/
void rtoDebugMatch(L7_uint32 addr, L7_uint32 count)
{
  L7_routeEntry_t route;
  L7_uint32 i, msecSinceBoot, msecElapsed;
  L7_RC_t rc = L7_FAILURE;

  if(!count)
   count = 1;

  msecSinceBoot = osapiTimeMillisecondsGet();

  for(i = 0; i < count; i++)
    rc = rtoBestRouteLookup (addr, &route, L7_TRUE);

  msecElapsed = osapiTimeMillisecondsGetOffset(msecSinceBoot);

  sysapiPrintf("%ums for %d runs: %s: ", msecElapsed, i, osapiInet_ntoa(addr));

  if(rc == L7_SUCCESS)
  {
    sysapiPrintf("next hop %s (%x)\n", osapiInet_ntoa(route.ipAddr), route.subnetMask);
  }
  else
  {
    sysapiPrintf("no match\n");
  }

}

/*********************************************************************
*
* @purpose  Tests the RTO get, get_next and get_next_asc lookups
*
*
* @end
*********************************************************************/
void rtoDebugLookup(L7_uint32 addr, L7_uint32 mask, L7_uint32 next)
{
  rtoRouteData_t *pData = 0;
  rtoTreeKey_t network;
  rtoTreeKey_t netmask;
  L7_uint32 msecSinceBoot, msecElapsed;

  if(next != L7_RN_GETNEXT_ASCEND &&
     next != L7_RN_GETNEXT &&
     next != L7_RN_EXACT)
  {
    sysapiPrintf("usage: rtoDebugLookup(L7_uint32 addr, L7_uint32 mask, L7_uint32 next)\n");
    sysapiPrintf("next must be one of L7_RN_EXACT(%d), L7_RN_GETNEXT(%d) and L7_RN_GETNEXT_ASCEND(%d)\n",
                 L7_RN_EXACT, L7_RN_GETNEXT, L7_RN_GETNEXT_ASCEND);
    return;
  }

  rtoSetKey(&network, addr);
  rtoSetKey(&netmask, mask);


  sysapiPrintf("lookup %s for %s mask %x ",
               (next ==  L7_RN_GETNEXT) ? "L7_RN_GETNEXT" : ((next ==  L7_RN_EXACT) ? "L7_RN_EXACT" : "L7_RN_GETNEXT_ASCEND"),
               osapiInet_ntoa(addr),
               mask);

  /* Find the next network given the input network.
  */
  msecSinceBoot = osapiTimeMillisecondsGet();

  osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER);
  pData = radixLookupNode (&rtoRouteTreeData, &network, &netmask, next);

  msecElapsed = osapiTimeMillisecondsGetOffset(msecSinceBoot);

  sysapiPrintf("\n%ums: %s: ", msecElapsed, osapiInet_ntoa(addr));

  if (pData == L7_NULLPTR)
  {
    osapiReadLockGive(rtoRwLock);
    sysapiPrintf("entry does not exist\n");
    return;
  }

  sysapiPrintf("entry found:\n");

  sysapiPrintf(" ip %s (%x)",
               osapiInet_ntoa(osapiNtohl(pData->network.addr)),
               pData->network);
  sysapiPrintf(" mask %s (%x)",
               osapiInet_ntoa(osapiNtohl(pData->netmask.addr)),
               pData->netmask);
  sysapiPrintf(" next hop %s (%x)\n",
               osapiInet_ntoa(pData->nextRouteInfo->nextHops[0].nextHopIP),
               pData->nextRouteInfo->nextHops[0].nextHopIP);
  osapiReadLockGive(rtoRwLock);

  return;
}

extern L7_RC_t usmDbInetAton(L7_uchar8 *ipAddr, L7_uint32 *val);
extern L7_BOOL usmDbNetmaskIsContiguous(L7_uint32 netMask);
extern L7_RC_t rtoRouteMetricChange(L7_uint32 destAddr, L7_uint32 destMask,
                             L7_uint32 metric);
void rtoRouteMetricChangeStr(L7_char8 *argIPaddr,
                             L7_char8 *argSubnetMask,
                             L7_uint32 metric)
{
  L7_char8 strSubnetMask[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 longSubnetMask, longIPaddr;
  L7_RC_t rc = L7_FAILURE;

   memset (strIPaddr, 0, sizeof(strIPaddr));
   memset (strSubnetMask, 0, sizeof(strSubnetMask));
   OSAPI_STRNCPY_SAFE(strIPaddr, argIPaddr);
   OSAPI_STRNCPY_SAFE(strSubnetMask, argSubnetMask);

   if ((usmDbInetAton(strIPaddr, &longIPaddr) == L7_SUCCESS) &&
       (usmDbInetAton(strSubnetMask, &longSubnetMask) == L7_SUCCESS))
   {
     if ((longSubnetMask != 0) && usmDbNetmaskIsContiguous(longSubnetMask) != L7_TRUE)
     {
        printf("Mask must have contiguous ones.");
        return;
     }
     if (ipMapStaticRouteAddrIsValid(longIPaddr, longSubnetMask) != L7_TRUE)
     {
       printf("The specified Static Route Address is invalid.");
       return;
     }
     rc = rtoRouteMetricChange(longIPaddr, longSubnetMask, metric);
     if (rc != L7_SUCCESS)
     {
       printf("Metric could not be set.");
       return;
     }
   }
   else
   {
     printf("usmDbInetAton failed.");
     return;
   }
}


#define RTO_UNIT_TEST 0
#if RTO_UNIT_TEST
/*********************************************************************
*
* @purpose  Create a random route
*
*
* @notes
*
* @end
*********************************************************************/
static
void rtoRandomRouteGet(L7_uint32 *ipAddr, L7_uint32 *ipMask)
{
  L7_uint32 prefixLen, i;
  L7_double64 rVal;
  L7_uchar8 * ptr = (L7_uchar8 *)ipAddr;

  /* make sure first byte is non-zero */
  for (i = 0; i < 4; i++)
  {
    rVal = L7_Random();
    ptr[i] = (L7_uchar8)(rVal * 255.0);
  }

  /* skip first byte 0, mcast e0 or first byte f */
  while(ptr[0] == 0 || (ptr[0] & 0xe0) || (ptr[0] & 0xf0) == 0xf0 )
  {
    /* allow all zeros - for default route */
    if(*ipAddr == 0)
      break;

    rVal = L7_Random();
    ptr[0] = (L7_uchar8)(rVal * 255.0);
  }


  rVal = L7_Random();
  prefixLen = (L7_uint32)(rVal * 65535.0) %32;

  /* get and apply mask */
  *ipMask = -1;
  *ipMask <<= (32 - prefixLen);
  (*ipAddr) &= (*ipMask);
}

static int skip_all_default = 0;
static int skip_default_nzmask = 1;
static int skip_dup = 1;
/* static int do_rn_walk = 0; */
/* static int force_def = 1; */
static int do_forever = 0;

L7_routeEntry_t rtoTestRoutes[10000];

L7_RC_t rtoTestBestMatch(L7_uint32 max_routes)
{
  L7_int32  i, ipaddr, failed = 0;
  L7_RC_t rc;
  L7_routeEntry_t rtMatch;
  rtoRouteData_t * pData = 0, * pLPM = 0;

  for(i = 0; i < max_routes; i++)
  {
    if(rtoTestRoutes[i].pref == 255)
       continue;

    ipaddr = rtoTestRoutes[i].ipAddr + 1;

    rc = rtoBestRouteLookup(ipaddr, &rtMatch, L7_TRUE);

    /* search manually! */
    pData  = rtoFirstRouteGet(L7_RN_GETNEXT);
    pLPM = 0;
    while(pData)
    {
        /* dont look at route awaiting delete notify */
      if((ipaddr & osapiNtohl(pData->netmask.addr)) ==
         (osapiNtohl(pData->network.addr) & osapiNtohl(pData->netmask.addr)) &&
         pData->nextRouteInfo)
      {
        if(!pLPM || osapiNtohl(pData->netmask.addr) > osapiNtohl(pLPM->netmask.addr))
        {
          pLPM = pData;
        }
      }

      pData = radixGetNextEntry(&rtoRouteTreeData, pData);
    }

    if(rc == L7_FAILURE)
    {

      if(pLPM)
      {
        printf("\naddr 0x%x match NONE ", ipaddr);
        printf("walk FOUND net 0x%x mask 0x%x - FAILED",
               osapiNtohl(pLPM->network.addr), osapiNtohl(pLPM->netmask.addr));
        failed++;
      }
    }
    else
    {
      if(pLPM)
      {
        if(osapiNtohl(pLPM->network.addr) != rtMatch.ipAddr || osapiNtohl(pLPM->netmask.addr) != rtMatch.subnetMask)
        {
          printf("\naddr 0x%x match FOUND net 0x%x mask 0x%x ",
                 ipaddr, rtMatch.ipAddr, rtMatch.subnetMask);
          printf("walk FOUND net 0x%x mask 0x%x",
                 osapiNtohl(pLPM->network.addr), osapiNtohl(pLPM->netmask.addr));
          printf(" - FAILED");
          failed++;
        }
      }
      else
      {
        printf("\naddr 0x%x match FOUND net 0x%x mask 0x%x ",
               ipaddr, rtMatch.ipAddr, rtMatch.subnetMask);
        printf("walk NONE - FAILED");
        failed++;
      }
    }
  }

  if(failed)
    printf("\n%u best match FAILURES!", failed);
  else
    printf("\nbest match test PASS for %u routes", max_routes);

  return 1;
}

/*********************************************************************
*
* @purpose  Test the radix tree
*
* @notes    Some platforms require the use of setSkipNotify(1) to
*           skip RTO notifications. This is to avoid crashes in the
*           BCM driver (draco_l3.c)
*
*           This function inserts max_routes random routes into the
*           tree, tests deletion, lookup and LPM matches for
*           these routes.
*
* @end
*********************************************************************/
L7_RC_t rtoRtTest(L7_uint32 nextHop, L7_uint32 max_routes, L7_uint32 forever)
{
    L7_int32  i, found, failed;
    L7_uint32 link_loc = nextHop;
    L7_routeEntry_t tempRoute;

    /* need next hop active */
    randSelectStream(0);   /* bad SIM !! */
    randPlantSeeds(-1);

again:

    for(i = 0; i < max_routes; i++)
    {
        rtoTestRoutes[i].protocol = RTO_STATIC;
        rtoTestRoutes[i].pref= 1;
        rtoTestRoutes[i].ecmpRoutes.numOfRoutes = 1;
        rtoTestRoutes[i].ecmpRoutes.equalCostPath[0].arpEntry.ipAddr = link_loc;
        rtoTestRoutes[i].ecmpRoutes.equalCostPath[0].arpEntry.intIfNum = 1;

        rtoRandomRouteGet(&rtoTestRoutes[i].ipAddr,&rtoTestRoutes[i].subnetMask);

        if (rtoValidDestAddr(rtoTestRoutes[i].ipAddr, rtoTestRoutes[i].subnetMask) != L7_TRUE){
           i--;
           continue;
        }

        if(skip_all_default)
          if(!rtoTestRoutes[i].ipAddr)
          {
           i--;
           continue;
          }
        if(skip_default_nzmask)
          if(rtoTestRoutes[i].subnetMask && !rtoTestRoutes[i].ipAddr)
          {
            i--;
            continue;
          }

        if(skip_dup)
          if(rtoPrefixFind(rtoTestRoutes[i].ipAddr,
                           rtoTestRoutes[i].subnetMask,
                           &tempRoute) == L7_SUCCESS)
          {
           i--;
           continue;
          }

        printf("rtoRtTest(%u): adding ip %x mask %x ", rtoRouteCount(L7_FALSE),
               rtoTestRoutes[i].ipAddr, rtoTestRoutes[i].subnetMask);

        if(rtoRouteAdd(&rtoTestRoutes[i]) != L7_SUCCESS)
        {
          printf("failed\n");
           rtoTestRoutes[i].pref = 255;
        }
        else
        {
          printf("ok [dnodes %u, mnodes %u, mkl %u]\n",
                 rtoDebugWalkDataNodes(), rtoDebugWalkMkNodes(), rtoDebugWalkMkList());
        }

    }

    /* test best matches */
    rtoTestBestMatch(max_routes);

    /* also serves to flush ouput */
    for(i = 0; i < 10; i++)
      printf("rto6RtTest: about to delete!\n");

    /* delete half the routes */
    for(i = max_routes/2; i < max_routes; i++)
    {
        if(rtoTestRoutes[i].pref == 255)
           continue;
        printf("rto6RtTest(%u): deleting ip %x mask %x ", rtoRouteCount(L7_FALSE),
               rtoTestRoutes[i].ipAddr, rtoTestRoutes[i].subnetMask);

        if(rtoRouteDelete(&rtoTestRoutes[i]) != L7_SUCCESS)
        {
           printf("failed\n");
           return L7_FAILURE;
           rtoTestRoutes[i].pref = 255;
        }
        else
        {
          printf("ok [dnodes %u, mnodes %u, mkl %u]\n",
                 rtoDebugWalkDataNodes(), rtoDebugWalkMkNodes(), rtoDebugWalkMkList());
        }

    }

    /* test best matches */
    rtoTestBestMatch(max_routes/2);

    /* add half */
    for(i = max_routes/2; i < max_routes; i++)
    {
        rtoTestRoutes[i].protocol = RTO_STATIC;
        rtoTestRoutes[i].pref= 1;
        rtoTestRoutes[i].ecmpRoutes.numOfRoutes = 1;
        rtoTestRoutes[i].ecmpRoutes.equalCostPath[0].arpEntry.ipAddr = link_loc;
        rtoTestRoutes[i].ecmpRoutes.equalCostPath[0].arpEntry.intIfNum = 1;

        rtoRandomRouteGet(&rtoTestRoutes[i].ipAddr,&rtoTestRoutes[i].subnetMask);

        if (rtoValidDestAddr(rtoTestRoutes[i].ipAddr, rtoTestRoutes[i].subnetMask) != L7_TRUE){
           i--;
           continue;
        }

        if(skip_all_default)
          if(!rtoTestRoutes[i].ipAddr)
          {
           i--;
           continue;
          }
        if(skip_default_nzmask)
          if(rtoTestRoutes[i].subnetMask && !rtoTestRoutes[i].ipAddr)
          {
            i--;
            continue;
          }

        if(skip_dup)
          if(rtoPrefixFind(rtoTestRoutes[i].ipAddr,
                           rtoTestRoutes[i].subnetMask,
                           &tempRoute) == L7_SUCCESS)
          {
           i--;
           continue;
          }

          printf("rtoRtTest(%u): adding ip %x mask %x ", rtoRouteCount(L7_FALSE),
                 rtoTestRoutes[i].ipAddr, rtoTestRoutes[i].subnetMask);

        if(rtoRouteAdd(&rtoTestRoutes[i]) != L7_SUCCESS)
        {
          printf("failed\n");
           rtoTestRoutes[i].pref = 255;
        }
        else
          printf("ok [dnodes %u, mnodes %u, mkl %u]\n",
                 rtoDebugWalkDataNodes(), rtoDebugWalkMkNodes(), rtoDebugWalkMkList());

    }

    /* test best matches */
    rtoTestBestMatch(max_routes);

    /* test lookups */
    failed = 0;
    for(i = 0; i < max_routes; i++)
    {
      if(rtoTestRoutes[i].pref == 255)
         continue;

      memcpy(&tempRoute, &rtoTestRoutes[i], sizeof(tempRoute));
      tempRoute.subnetMask = 0;
      found = 0;

      while(rtoNextRouteGet(&tempRoute, 0) == L7_SUCCESS)
      {
        if(tempRoute.ipAddr == rtoTestRoutes[i].ipAddr)
        {
          if(tempRoute.subnetMask == rtoTestRoutes[i].subnetMask)
          {
            found = 1;
            break;
          }
        }
        else
        {
          /* if addr doesnt match we've missed what we wanted */
          break;
        }
      }

      if(!found)
      {
        printf("\nlookup failed for 0x%x mask 0x%x",
               rtoTestRoutes[i].ipAddr, rtoTestRoutes[i].subnetMask);
        failed++;
      }
    }

    if(!failed)
      printf("\nlookup test PASSED");

    /* delete all routes */
    for(i = 0; i < max_routes; i++)
    {
        if(rtoTestRoutes[i].pref == 255)
           continue;
        if(rtoRouteDelete(&rtoTestRoutes[i]) != L7_SUCCESS)
        {
           printf("rtoRtTest:  del failure\n");
           return L7_FAILURE;
           rtoTestRoutes[i].pref = 255;
        }
        else
        {
          printf("deleted [dnodes %u, mnodes %u, mkl %u]\n",
                 rtoDebugWalkDataNodes(), rtoDebugWalkMkNodes(), rtoDebugWalkMkList());
        }
    }

    if(do_forever || forever) goto again;
    return L7_SUCCESS;
}

void rtoDebugAdd(L7_uint32 ipaddr, L7_uint32 mask, L7_uint32 link_loc)
{
  L7_routeEntry_t rtoTestRt;

  memset(&rtoTestRt, 0, sizeof(rtoTestRt));

  rtoTestRt.protocol = RTO_STATIC;
  rtoTestRt.pref= 1;
  rtoTestRt.ecmpRoutes.numOfRoutes = 1;
  rtoTestRt.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr = link_loc;
  rtoTestRt.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum = 1;

  rtoTestRt.ipAddr = ipaddr;
  rtoTestRt.subnetMask = mask;


  if(rtoRouteAdd(&rtoTestRt) != L7_SUCCESS)
  {
    printf("(%u): FAILED adding ip %x mask %x ", rtoRouteCount(L7_FALSE),
           rtoTestRt.ipAddr, rtoTestRt.subnetMask);
  }
}

void rtoDebugDel(L7_uint32 ipaddr, L7_uint32 mask, L7_uint32 link_loc)
{
  L7_routeEntry_t rtoTestRt;

  memset(&rtoTestRt, 0, sizeof(rtoTestRt));

  rtoTestRt.protocol = RTO_STATIC;
  rtoTestRt.pref= 1;
  rtoTestRt.ecmpRoutes.numOfRoutes = 1;
  rtoTestRt.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr = link_loc;
  rtoTestRt.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum = 1;

  rtoTestRt.ipAddr = ipaddr;
  rtoTestRt.subnetMask = mask;

  if(rtoRouteDelete(&rtoTestRt) != L7_SUCCESS)
  {
    printf("(%u): FAILED deleting ip %x mask %x ", rtoRouteCount(L7_FALSE),
           rtoTestRt.ipAddr, rtoTestRt.subnetMask);
  }
}



#endif

