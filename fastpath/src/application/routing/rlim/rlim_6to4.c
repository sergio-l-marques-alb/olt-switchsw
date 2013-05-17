/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006
*
**********************************************************************
*
* @filename rlimtnnl.c
*
* @purpose Primary tunnel-specific operational code.
*
* @component Routing Logical Interface Manager
*
* @comments
*
* @create 06/16/2005
*
* @author eberge
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "log.h"
#include "rlim.h"
#include "l7_ip_api.h"
#include "l7_ip6_api.h"
#include "rto_api.h"
#include "l7_rto6_api.h"
#include "nimapi.h"
#include "osapi.h"

#define RLIM_6TO4_TRACE_LEN    500

static L7_uint32 rlim6to4TunnelDestsMax = L7_MAX_6TO4_NEXTHOPS;
static avlTreeTables_t   *rlim6to4TunnelDestTreeHeap;   /* space for tree nodes */
static rlim6to4Dest_t *rlim6to4TunnelDestDataHeap;   /* space for data */
avlTree_t         rlim6to4TunnelDestTree;

L7_uint32 neighbor_age_time = RLIM_TUNNEL_DEST_AGE_MAX;


/*****************************************************************
* @purpose  Compare tunnel dest keys
*
* @param    a           Pointer to the first key
* @param    b           Pointer to the second key
* @param    len         Length of the key (unused)
*
* @returns   0 if the keys are equal
* @returns   1 if key 'a' is greater than key 'b'
* @returns  -1 if key 'a' is less than key 'b'
*
* @notes
*
* @end
*****************************************************************/
int rlimCompare6to4TunnelDest(const void *a, const void *b, size_t len)
{
  rlim6to4Dest_t *va = (rlim6to4Dest_t *)a;
  rlim6to4Dest_t *vb = (rlim6to4Dest_t *)b;

  if(va->tunnelId > vb->tunnelId) return 1;
  if(va->tunnelId < vb->tunnelId) return -1;
  if(va->remoteAddr > vb->remoteAddr) return 1;
  if(va->remoteAddr < vb->remoteAddr) return -1;
  return 0;
}


/*********************************************************************
*
* @purpose Create the 6to4 next hop list. The list is
*          implemented as an AVL tree.
*
* @param none
*
* @returns L7_SUCCESS if tree successfully created.
* @returns L7_FAILURE otherwise
*
* @comments key is tunnel dest/ tunnel ID
*
* @end
*
*********************************************************************/
L7_RC_t rlim6to4TunnelDestListCreate(void)
{
    L7_uint32 avlType = 0x10;      /* common constant for all our AVL trees */

    rlim6to4TunnelDestTreeHeap =
        osapiMalloc(L7_RLIM_COMPONENT_ID, rlim6to4TunnelDestsMax * sizeof(avlTreeTables_t));
    if (rlim6to4TunnelDestTreeHeap == NULL) {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
                "Error: unable to allocate RLIM 6to4 tree heap.\n");
        return L7_FAILURE;
    }
    memset(rlim6to4TunnelDestTreeHeap, 0,
           rlim6to4TunnelDestsMax * sizeof(avlTreeTables_t));

    rlim6to4TunnelDestDataHeap =
        osapiMalloc(L7_RLIM_COMPONENT_ID, rlim6to4TunnelDestsMax * sizeof(rlim6to4Dest_t));
    if (rlim6to4TunnelDestDataHeap == NULL) {
        osapiFree(L7_RLIM_COMPONENT_ID, rlim6to4TunnelDestTreeHeap);
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
                "Error: unable to allocate RLIM 6to4 data heap.\n");
        return L7_FAILURE;
    }
    memset(rlim6to4TunnelDestDataHeap, 0,
           rlim6to4TunnelDestsMax * sizeof(rlim6to4Dest_t));

    memset(&rlim6to4TunnelDestTree, 0, sizeof(avlTree_t));

    /* key is IP address plus mask. */
    avlCreateAvlTree(&rlim6to4TunnelDestTree, rlim6to4TunnelDestTreeHeap,
                     rlim6to4TunnelDestDataHeap, rlim6to4TunnelDestsMax,
                     (L7_uint32)(sizeof(rlim6to4Dest_t)), avlType,
                     sizeof(L7_uint32) * 2);
    (void)avlSetAvlTreeComparator(&rlim6to4TunnelDestTree, rlimCompare6to4TunnelDest);

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Destroy the 6to4 list for RLIM.
*
* @param none
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t rlim6to4TunnelDestListDestroy(void)
{
    if (rlim6to4TunnelDestTreeHeap != L7_NULLPTR) {
        osapiFree(L7_RLIM_COMPONENT_ID, rlim6to4TunnelDestTreeHeap);
        rlim6to4TunnelDestTreeHeap = L7_NULLPTR;
    }

    if (rlim6to4TunnelDestDataHeap != L7_NULLPTR) {
        osapiFree(L7_RLIM_COMPONENT_ID, rlim6to4TunnelDestDataHeap);
        rlim6to4TunnelDestDataHeap = L7_NULLPTR;
    }

    memset(&rlim6to4TunnelDestTree, 0, sizeof(avlTree_t));

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Add a 6to4 tunnel dest.
*
* @param none
*
* @returns L7_SUCCESS if route successfully added.
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t rlim6to4TunnelDestListAdd(L7_uint32 destAddr, L7_uint32 tunnelId)
{
    L7_RC_t rc = L7_FAILURE;
    rlim6to4Dest_t dummyDest;
    L7_uchar8      destStr[OSAPI_INET_NTOA_BUF_SIZE];

    memset(&dummyDest,0,sizeof(dummyDest));
    dummyDest.remoteAddr = destAddr;
    dummyDest.tunnelId = tunnelId;

    osapiInetNtoa(destAddr, destStr);

    /* make sure we haven't exceeded max number of entries. */
    if (avlTreeCount(&rlim6to4TunnelDestTree) == rlim6to4TunnelDestsMax) {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RLIM_COMPONENT_ID,
              "RLIM: Not adding 6to4 route to destination %s!! Total 6to4 routes at max (%d) already."
              " In Fastpath we allow upto max 16 6to4 tunnel neighbors. After reaching max, if we try "
              "to send data to new 6to4 neighbor we get this log message and the 6to4 route to tunnel "
              "neighbor is not added in h/w.\n",
              destStr, rlim6to4TunnelDestsMax);

    }
    else if (avlInsertEntry(&rlim6to4TunnelDestTree, &dummyDest) != NULL) {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
                "Error inserting route into RLIM 6to4 list\n.");
    }
    else {
        rc = L7_SUCCESS;
        if (avlTreeCount(&rlim6to4TunnelDestTree) == rlim6to4TunnelDestsMax) {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
                  "RLIM: Total number of 6to4 routes reached max = %d in the system!!", rlim6to4TunnelDestsMax);
        }
    }

    /* start aging timer */
    if (rlimOpData->tunnelDestAgeTimer == L7_NULL)
    {
      /* schedule an event to process best route changes */
      osapiTimerAdd((void *)rlimTunnelDestAgeTimerCallback, L7_NULL, L7_NULL,
                  RLIM_TUNNEL_DEST_AGE_TIME, &rlimOpData->tunnelDestAgeTimer);
    }

    /* let rto know we want callback for next hop change */
    rtoNHResCallbackRegister(destAddr, rlimTunnelIp4NhopChangeCallback, (void *)tunnelId);

    return rc;
}

/*********************************************************************
*
* @purpose Delete a 6to4 tunnel dest.
*
* @param @b{(input)} destAddr - destination IP address
* @param @b{(input)} tunnelId
*
* @returns L7_SUCCESS if route successfully deleted.
* @returns L7_FAILURE if the list does not contain the route to be deleted.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t rlim6to4TunnelDestListDelete(rlim6to4Dest_t *tdest)
{
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 destAddr = tdest->remoteAddr;
    L7_uint32 tunnelId = tdest->tunnelId;
    rlim6to4Dest_t dummyDest;
    L7_uint32 intIfNum;
    rlimTunnelOpData_t *top;

    dummyDest.remoteAddr = destAddr;
    dummyDest.tunnelId = tunnelId;

    top = &rlimTunnelOpData[tdest->tunnelId];
    intIfNum = top->tunnelIntIfNum;

    if ((tdest->flags & RLIM_TNNLOP_FLAG_DTL_ROUTE_CREATED) != 0){

        rlim6to4TunnelNetworkSet(intIfNum, tdest, RLIM_RT_6TO4_DEL );
    }
    /* delete the ipv6 neighbor from the stack */
    osapiIpv6NdpFlush(intIfNum, &(tdest->nbr));

    rtoNHResCallbackUnregister(destAddr, rlimTunnelIp4NhopChangeCallback, (void *)tunnelId);

#ifdef L7_NSF_PACKAGE
    /* Checkpoint the deletion event of this 6to4 neighbor */
    ip6MapCheckpointNeighbor(&tdest->nbr, intIfNum, L7_FALSE);
#endif

    if (avlDeleteEntry(&rlim6to4TunnelDestTree, &dummyDest)) {
        rc = L7_SUCCESS;
    }
    return rc;
}


/*********************************************************************
*
* @purpose Find a tunnel dest
*
* @returns L7_SUCCESS if a matching route was found.
* @returns L7_FAILURE otherwise.
*
* @comments
*
* @end
*
*********************************************************************/
rlim6to4Dest_t *rlim6to4TunnelDestListFind(L7_uint32 destAddr, L7_uint32 tunnelId)
{
    rlim6to4Dest_t dummyDest;
    rlim6to4Dest_t *tdest;

    dummyDest.remoteAddr = destAddr;
    dummyDest.tunnelId = tunnelId;


    tdest = avlSearchLVL7(&rlim6to4TunnelDestTree, &dummyDest, AVL_EXACT);
    return tdest;
}

/*********************************************************************
* @purpose  Check on the reachability of the 6to4 tunnel destinations.
*
* @param    tdest     ptr to tunnel dest structure
*
* @returns  void
*
* @notes    Also keep track of the nexthop
*
* @end
*********************************************************************/
void rlim6to4TunnelReachabilityCheck(rlim6to4Dest_t *tdest)
{
  L7_uint32 i;
  rlimTunnelOpData_t *top = &rlimTunnelOpData[tdest->tunnelId];
  L7_routeEntry_t route;
  L7_arpEntry_t *pArpEntry;
  L7_BOOL found = L7_FALSE;
  L7_INTF_TYPES_t itype;


  found = L7_FALSE;
  if (((top->flags & RLIM_TNNLOP_FLAG_DISABLE_IN_PROGRESS) == 0) &&
      ((top->flags & RLIM_TNNLOP_FLAG_PORT_ENABLED) != 0) &&
       (rtoBestRouteLookup(tdest->remoteAddr, &route, L7_FALSE) == L7_SUCCESS))
  {
      for (i = 0; i < route.ecmpRoutes.numOfRoutes; i++)
      {
        pArpEntry = &route.ecmpRoutes.equalCostPath[i].arpEntry;

        if (nimGetIntfType(pArpEntry->intIfNum, &itype) == L7_SUCCESS)
        {
      /* next hop interface must be tunnel source interface */
          if ((itype != L7_TUNNEL_INTF) &&
          (pArpEntry->intIfNum == top->localIntIfNum))
          {
            /*
             * Locally attached subnet have our local address as the
             * nexthop addr, so we need to change this to the address
             * of the destination.
             */
            if (route.protocol == RTO_LOCAL)
              tdest->nextHopIp4Addr = tdest->remoteAddr;
            else
              tdest->nextHopIp4Addr = pArpEntry->ipAddr;

            tdest->metric = route.metric;
            tdest->flags |= RLIM_TNNLOP_FLAG_REACHABLE;
            found = L7_TRUE;
            break;
          }
        }
      }
    }

  if (!found)
  {
      tdest->nextHopIp4Addr = L7_NULL_IP_ADDR;
      tdest->flags &= ~RLIM_TNNLOP_FLAG_REACHABLE;
  }
}



/*********************************************************************
* @purpose Add a 48 bit 6to4 route to rto6. Required because hardware
*          needs mac address of ipv4 next hop. hapi will determine the
*          mac based on v4 next hop. The route needs to go in rto6 because it
*          needs to be counted with all the other routes.
*
*
* @param   intIfNum        @b{(input)} The tunnel internal interface
* @param   tdest           @b{(input)} Tunnel destination
* @param   enable          @b{(input)} if enable, route is added, else deleted
*
*
*
* @returns  L7_SUCCESS
*           L7_FAILURE
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  rlim6to4TunnelNetworkSet(L7_uint32 intIfNum, rlim6to4Dest_t *tdest, RLIM_RT_6TO4_TYPE_t rtype )
{
   L7_uchar8      nhStr[OSAPI_INET_NTOA_BUF_SIZE];
   L7_uchar8      destStr[OSAPI_INET_NTOA_BUF_SIZE];
   L7_uchar8      traceBuf[RLIM_6TO4_TRACE_LEN];
   L7_route6Entry_t routeEntry;
   L7_RC_t rc;


   memset(&routeEntry, 0, sizeof(L7_route6Entry_t));
   routeEntry.ip6Addr.in6.addr16[0] = osapiHtons(0x2002);
   routeEntry.ip6Addr.in6.addr8[2] = (L7_uchar8)(tdest->remoteAddr >> 24);
   routeEntry.ip6Addr.in6.addr8[3] = (L7_uchar8)(tdest->remoteAddr >> 16);
   routeEntry.ip6Addr.in6.addr8[4] = (L7_uchar8)(tdest->remoteAddr >> 8);
   routeEntry.ip6Addr.in6.addr8[5] = (L7_uchar8)(tdest->remoteAddr );
   routeEntry.ip6PrefixLen = 48;
   routeEntry.protocol = RTO6_6TO4;
   routeEntry.metric = tdest->metric;
   routeEntry.pref = ip6MapRouterPreferenceGet(RTO6_STATIC); /* like automatic static route */
   routeEntry.ecmpRoutes.equalCostPath[0].ip6Addr.in6.addr16[0] = osapiHtonl(0xfe80);
   routeEntry.ecmpRoutes.equalCostPath[0].ip6Addr.in6.addr32[3] = osapiHtonl(tdest->nextHopIp4Addr);
   routeEntry.ecmpRoutes.equalCostPath[0].intIfNum = intIfNum;
   routeEntry.ecmpRoutes.numOfRoutes = 1;

   osapiInetNtoa(tdest->remoteAddr, destStr);
   if(rtype == RLIM_RT_6TO4_ADD)
   {
        if (RLIM_TRACE_6TO4_ROUTES & rlimDebugTraceFlags)
        {
           osapiSnprintf(traceBuf, RLIM_6TO4_TRACE_LEN,
                         "RLIM adding 6to4 route to destination %s on tunnel %d",
                         destStr, rlimIntIfNumToTunnelId[intIfNum]);
           rlimDebugTraceWrite(traceBuf);
        }
        if((rc = rto6RouteAdd(&routeEntry)) != L7_SUCCESS)
        {
           L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
                   "rlimTunnel6to4NetworkSet: address to rto failed\n");
        }
   }
   else if(rtype == RLIM_RT_6TO4_MOD)
   {
        if((rc = rto6RouteDelete(&routeEntry)) != L7_SUCCESS)
        {
           L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
                   "rlimTunnel6to4NetworkSet: address to rto failed\n");
        }
        if((rc = rto6RouteAdd(&routeEntry)) != L7_SUCCESS)
        {
           L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
                   "rlimTunnel6to4NetworkSet: address to rto failed\n");
        }

        if (RLIM_TRACE_NEXT_HOP_CHANGES & rlimDebugTraceFlags)
        {
           osapiInetNtoa(tdest->nextHopIp4Addr, nhStr);
           osapiSnprintf(traceBuf, RLIM_6TO4_TRACE_LEN,
                         "Nexthop changed to %s for 6to4 route to destination %s on tunnel %d",
                         nhStr, destStr, rlimIntIfNumToTunnelId[intIfNum]);
           rlimDebugTraceWrite(traceBuf);
        }
   }
   else
   {
        if (RLIM_TRACE_6TO4_ROUTES & rlimDebugTraceFlags)
        {
           osapiSnprintf(traceBuf, RLIM_6TO4_TRACE_LEN,
                         "RLIM deleting 6to4 route to destination %s on tunnel %d",
                         destStr, rlimIntIfNumToTunnelId[intIfNum]);
           rlimDebugTraceWrite(traceBuf);
        }

        if((rc = rto6RouteDelete(&routeEntry)) != L7_SUCCESS)
        {
           L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
                   "rlimTunnel6to4NetworkSet: address to rto failed\n");
        }

        /* Remove 6to4 ipv6 neighbor that earlier resulted in the creation
         * of this 6to4 route, from the stack */
   }

   return rc;
}

/*********************************************************************
* @purpose notification of v6 neighbor on 6to4 network. If possible, create
*          a 48 bit route.
*
*
* @param   intIfNum        @b{(input)} The tunnel internal interface
* @param   remote          @b{(input)} remote tunnel endpoint ipv4 addr
* @param   enable          @b{(input)} if enable, route is added, else deleted
*
*
*
* @returns  L7_SUCCESS
*           L7_FAILURE
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  rlimTunnel6to4NbrNotify(L7_uint32 intIfNum, L7_in6_addr_t *nbr, L7_uint32 enable )
{
  rlim6to4Dest_t *tdest;
  rlimTunnelOpData_t *top;
  L7_uint32 tunnelId;
  L7_uint32 remote;

  if(intIfNum > L7_MAX_INTERFACE_COUNT)
       return L7_FAILURE;

  /* extract ipv4 address */
  remote = (nbr->in6.addr8[2] << 24) |
           (nbr->in6.addr8[3] << 16) |
           (nbr->in6.addr8[4] << 8) |
        nbr->in6.addr8[5];

  RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);

  if((tunnelId = rlimIntIfNumToTunnelId[intIfNum]) <= RLIM_MAX_TUNNEL_ID){

     top = &rlimTunnelOpData[tunnelId];
     if(top)
     {
      /* We shall not create 6to4 neighbor route if the neighbor's prefix is
       * behind the 6to4 tunnel endpoint's site-prefix.
       * E.g., 2002:101:101:1::1 neighbor is in the site behind tunnel end-point
       * 1.1.1.1 (2002:101:101::/48).
       * Such neighbor should be reachable via non-6to4 routes (like connected
       * or dynamic routes) in our routing table  */
      if(remote == top->localIp4Addr)
      {
        /* The neighbor notified to us would have been created in the stack
         * already. Delete this neighbor in the stack that is created wrongly on
         * the tunnel interface */
        osapiIpv6NdpFlush(intIfNum, nbr);

        RLIM_WRITE_LOCK_GIVE(rlimRWLock);
        return L7_SUCCESS;
      }
     }

     tdest = rlim6to4TunnelDestListFind( remote, tunnelId);
     if(enable == L7_ENABLE)
     {
       /* create 48-bit 6to4 route */
       if(tdest == L7_NULL)
       {
         if(rlim6to4TunnelDestListAdd(remote,tunnelId) == L7_SUCCESS){

#ifdef L7_NSF_PACKAGE
             /* Checkpoint the addition of this 6to4 neighbor */
             ip6MapCheckpointNeighbor(nbr, intIfNum, L7_TRUE);
#endif
             tdest = rlim6to4TunnelDestListFind( remote, tunnelId);
             if(tdest != L7_NULL){
                /* store the ipv6 neighbor that resulted in the 6to4 route creation.
                 * We shall delete this neighbor from the stack when the reachability
                 * to the resulted 6to4 route is lost. That will ensure that we shall
                 * create the 6to4 route again once any ipv6 neighbor in that subnet
                 * later gets created in the stack */
                memcpy(&(tdest->nbr), nbr, sizeof(L7_in6_addr_t));
             }
         }
         else
         {
           /* The neighbor notified to us would have been created in the stack
            * already. Delete this neighbor in the stack as we reached max
            * 6to4 border router neighbors */
            osapiIpv6NdpFlush(intIfNum, nbr);
         }
       }
       else
       {
         /* It means a new neighbor entry is created in the stack that falls in
          * the already existing 6to4 route prefix. Please delete it from the stack.
          * We want to ensure we have only 1 ipv6 neighbor in the stack that
          * corresponds to the 6to4 route in RLIM. Else it creates problems */
         osapiIpv6NdpFlush(intIfNum, nbr);
       }

       if(tdest != L7_NULL){
          tdest->timeStamp = osapiUpTimeRaw();

          rlim6to4TunnelReachabilityCheck(tdest);
          if (((tdest->flags & RLIM_TNNLOP_FLAG_REACHABLE) != 0) &&
              ((tdest->flags & RLIM_TNNLOP_FLAG_DTL_ROUTE_CREATED) == 0)){

               if(rlim6to4TunnelNetworkSet(intIfNum, tdest, RLIM_RT_6TO4_ADD ) == L7_SUCCESS){

                    tdest->flags |= RLIM_TNNLOP_FLAG_DTL_ROUTE_CREATED;
               }
          }
          else
          {
            if((tdest->flags & RLIM_TNNLOP_FLAG_REACHABLE) == 0)
            {
               /* no reachability to the 6to4 neighbor exists, delete this destination
                * from the tunnel destinations list */
               rlim6to4TunnelDestListDelete(tdest);
            }
          }
       }
     }
     else if(enable == L7_DISABLE)
     {
        /* delete 48-bit 6to4 route */
       if(tdest != L7_NULL)
       {
         rlim6to4TunnelDestListDelete(tdest);
       }
     }
  }

  RLIM_WRITE_LOCK_GIVE(rlimRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Checkpoint the list of 6to4 tunnel destinations
*
* @param   void
*
* @returns  void
*
* @end
*********************************************************************/
void rlim6to4Ckpt(void)
{
#ifdef L7_NSF_PACKAGE
  rlim6to4Dest_t dest, *tdest;
  rlimTunnelOpData_t *top;

  RLIM_READ_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);
  memset(&dest, 0, sizeof(dest));
  while ((tdest = avlSearchLVL7(&rlim6to4TunnelDestTree, &dest, AVL_NEXT)) != L7_NULLPTR)
  {
    top = &rlimTunnelOpData[tdest->tunnelId];

    dest.tunnelId    = tdest->tunnelId;
    dest.remoteAddr = tdest->remoteAddr;

    (void)ip6MapNdpCkptEntryAdd(&tdest->nbr, top->tunnelIntIfNum, L7_TRUE);
  }
  RLIM_READ_LOCK_GIVE(rlimRWLock);
#endif
}

/*********************************************************************
* @purpose  Check avl tree of tunnel dests for changed next hops
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void rlimTunnel6to4NhopChangeProcess(void)
{
    rlim6to4Dest_t dummyDest, *tdest, *tdestNext;
    rlimTunnelOpData_t *top;
    L7_uint32 intIfNum;

    memset(&dummyDest, 0, sizeof(dummyDest));


    /* Get first entry in the tree. */
    tdest = avlSearchLVL7(&rlim6to4TunnelDestTree, &dummyDest, AVL_EXACT);
    if (tdest == L7_NULLPTR) {
        tdest = avlSearchLVL7(&rlim6to4TunnelDestTree, &dummyDest, AVL_NEXT);
    }
    while (tdest) {


      L7_uint32 oldNextHop = tdest->nextHopIp4Addr;

      rlim6to4TunnelReachabilityCheck(tdest);
      top = &rlimTunnelOpData[tdest->tunnelId];
      intIfNum = top->tunnelIntIfNum;

      tdestNext = avlSearchLVL7(&rlim6to4TunnelDestTree, tdest, AVL_NEXT);

      /* look for change to next hop */
      if((tdest->flags & RLIM_TNNLOP_FLAG_DTL_ROUTE_CREATED) == 0){

          /* route not installed yet */
          if((tdest->flags & RLIM_TNNLOP_FLAG_REACHABLE) != 0){
            if(rlim6to4TunnelNetworkSet(intIfNum, tdest, RLIM_RT_6TO4_ADD ) == L7_SUCCESS){

                 tdest->flags |= RLIM_TNNLOP_FLAG_DTL_ROUTE_CREATED;
        }
          }
      }
      else if((tdest->flags & RLIM_TNNLOP_FLAG_REACHABLE) == 0){
          /* route was installed, now not reachable */
          /* Delete the 6to4 ipv6 neighbor from the stack, delete the 6to4 route from h/w */
          rlim6to4TunnelDestListDelete(tdest);
      }
      else if(tdest->nextHopIp4Addr != oldNextHop){
          rlim6to4TunnelNetworkSet(intIfNum, tdest, RLIM_RT_6TO4_MOD);
      }

      tdest = tdestNext;
    }
}


/*********************************************************************
* @purpose  Purge all tunnel dests associated with tunnel
*
* @param    tunnelId      Id of deleted tunnel.
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void rlim6to4TunnelDestPurge(L7_uint32 tunnelId)
{
    rlim6to4Dest_t dummyDest, *tdest,*tdestNext;

    memset(&dummyDest, 0, sizeof(dummyDest));


    /* Get first entry in the tree. */
    tdest = avlSearchLVL7(&rlim6to4TunnelDestTree, &dummyDest, AVL_EXACT);
    if (tdest == L7_NULLPTR) {
        tdest = avlSearchLVL7(&rlim6to4TunnelDestTree, &dummyDest, AVL_NEXT);
    }
    while (tdest) {


      tdestNext = avlSearchLVL7(&rlim6to4TunnelDestTree, tdest, AVL_NEXT);

      if(tdest->tunnelId == tunnelId){

          if(rlim6to4TunnelDestListDelete(tdest) != L7_SUCCESS){
               L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
                       "rlim6to4TunnelDestPurge: failed to remove from tree\n");
          }

      }

      tdest = tdestNext;
    }
}

/*********************************************************************
* @purpose Timer callback to trigger 6to4 tunnel destination aging
*
* @param   void
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void rlimTunnelDestAgeTimerCallback(void)
{
  L7_RC_t rc;
  rlimMsg_t msg;

  /*
   * Mark timer as "fired"
   */
  rlimOpData->tunnelDestAgeTimer = L7_NULL;

  if (rlimOpData->processQueue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RLIM_COMPONENT_ID,
            "RLIM Process_Queue is NULL");
    return;
  }

  memset((void *)&msg, 0, sizeof(msg));
  msg.msgId = RLIM_TUNNEL_DEST_AGE_EVENT;

  rc = osapiMessageSend(rlimOpData->processQueue, &msg, sizeof(msg),
                          L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RLIM_COMPONENT_ID,
              "osapiMessageSend() failed with rc = %u", rc);
  }
}

/*********************************************************************
* @purpose  Set the age time of the 6to4 neighbor
*
* @param    age_time @b{(input)} Age time
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void rlim6to4SetAgeTime(L7_uint32 age_time)
{
   neighbor_age_time = age_time;
}

/*********************************************************************
* @purpose  Purge tunnel dests if aged out
*
* @param    tunnelId      Id of deleted tunnel.
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void rlim6to4TunnelDestAgeEventProcess(void)
{
    rlim6to4Dest_t dummyDest, *tdest,*tdestNext;
    L7_uint32 curr_time = osapiUpTimeRaw();

    memset(&dummyDest, 0, sizeof(dummyDest));


    /* Get first entry in the tree. */
    tdest = avlSearchLVL7(&rlim6to4TunnelDestTree, &dummyDest, AVL_EXACT);
    if (tdest == L7_NULLPTR) {
        tdest = avlSearchLVL7(&rlim6to4TunnelDestTree, &dummyDest, AVL_NEXT);
    }
    while (tdest) {


      tdestNext = avlSearchLVL7(&rlim6to4TunnelDestTree, tdest, AVL_NEXT);

      if(curr_time > tdest->timeStamp + neighbor_age_time){
         /* age it out */

         if(rlim6to4TunnelDestListDelete(tdest) != L7_SUCCESS){
               L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RLIM_COMPONENT_ID,
                       "rlim6to4TunnelDestAgeEventProcess: failed to remove from tree\n");
         }
      }
      else if(tdest->timeStamp > curr_time){
         tdest->timeStamp = curr_time;
      }

      tdest = tdestNext;
    }

    /* if non-empty, restart timer */
    tdest = avlSearchLVL7(&rlim6to4TunnelDestTree, &dummyDest, AVL_EXACT);
    if (tdest == L7_NULLPTR) {
        tdest = avlSearchLVL7(&rlim6to4TunnelDestTree, &dummyDest, AVL_NEXT);
    }
    if(tdest != L7_NULL){
        if (rlimOpData->tunnelDestAgeTimer == L7_NULL)
        {
          /* schedule an event to process best route changes */
          osapiTimerAdd((void *)rlimTunnelDestAgeTimerCallback, L7_NULL, L7_NULL,
                  RLIM_TUNNEL_DEST_AGE_TIME, &rlimOpData->tunnelDestAgeTimer);
        }
    }
}
