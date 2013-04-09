/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_api.c
*
* @purpose Contains definitions to support the DVMRP protocol
*
* @component
*
* @notes
*
* @create 03/03/2006
*
* @author Prakash/shashidhar
* @end
*
**********************************************************************/
#include "mfc_api.h"
#include "buff_api.h"
#include "heap_api.h"
#include "l7_mcast_api.h"
#include "dvmrp_api.h"
#include "dvmrp_mfc.h"
#include "dvmrp_main.h"
#include "dvmrp_util.h"
#include "dvmrp_timer.h"
#include "dvmrp_cache_table.h"
#include "dvmrp_interface.h"
#include "dvmrp_igmp_group_table.h"
#include "dvmrp_routing.h"
#include "dvmrp_protocol.h"
#include "mcast_wrap.h"
#include "mcast_map.h"

dvmrp_t *dvmrpCB = L7_NULLPTR;
#define DVMRP_BUFFPOOL_NAME      "dvmrpBufferPool"

/*********************************************************************
*
* @purpose  De-Inits the DVMRP protocol
*
* @param    dvmrpcb    @b{ (input) }Pointer to the DVMRP Control Block
*
* @returns  None
*
* @notes    De-inits all the DVMRP tables.
*
* @end
*********************************************************************/
void dvmrpClearInit(void *dvmrpCntrlBlk)
{
  dvmrp_t  *dvmrpcb;

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:ENTERED  \n",__FUNCTION__);         

  dvmrpcb = dvmrpCntrlBlk;
  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return;
  }

  if (dvmrpcb->timerHandle != L7_NULLPTR)
  {
    appTimerDeInit(dvmrpcb->timerHandle);
    dvmrpcb->timerHandle = L7_NULLPTR;
  }
  if (dvmrpcb->handle_list != L7_NULLPTR)
  {
    handleListDeinit(L7_FLEX_DVMRP_MAP_COMPONENT_ID, dvmrpcb->handle_list);
    dvmrpcb->handle_list = L7_NULLPTR;
  }

  if ((dvmrpcb->dvmrpGroupTableTreeHeap != L7_NULLPTR) && 
      (dvmrpcb->dvmrpGroupDataHeap != L7_NULLPTR))
  {
    dvmrpGroupTableDestroy(dvmrpcb);
    dvmrpcb->dvmrpGroupTableTreeHeap = L7_NULLPTR;
    dvmrpcb->dvmrpGroupDataHeap = L7_NULLPTR;
  }
  if (dvmrpcb->dvmrpRouteDataHeap != L7_NULLPTR)
  {
    DVMRP_FREE (L7_AF_INET, dvmrpcb->dvmrpRouteDataHeap);
    dvmrpcb->dvmrpRouteDataHeap = L7_NULLPTR;
  }
  if (dvmrpcb->dvmrpRouteTreeHeap != L7_NULLPTR)
  {
    DVMRP_FREE (L7_AF_INET, dvmrpcb->dvmrpRouteTreeHeap);
    dvmrpcb->dvmrpRouteTreeHeap = L7_NULLPTR;
  }
  if ((dvmrpcb->dvmrpCacheTableTreeHeap != L7_NULLPTR) && 
      (dvmrpcb->dvmrpCacheDataHeap != L7_NULLPTR))
  {
    dvmrpCacheTableDestroy(dvmrpcb);
    dvmrpcb->dvmrpGroupTableTreeHeap = L7_NULLPTR;
    dvmrpcb->dvmrpGroupDataHeap = L7_NULLPTR;
  }

  /* De-Initialize the Admin Scope Boundary database */
  if (dvmrpAdminScopeBoundaryDeInit (dvmrpcb) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Admin Scope database Destroy Failure");
  }

  if (dvmrpcb->dvmrpRwLock.handle != L7_NULLPTR)
  {
    osapiRWLockDelete(dvmrpcb->dvmrpRwLock);
    dvmrpcb->dvmrpRwLock.handle = L7_NULLPTR;
  }

  /* De-Register with MFC */
  if (mfcCacheEventDeregister (L7_MRP_DVMRP) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "MFC De-Registration Failed");
  }

  if (mgmdMapDeRegisterMembershipChangeEvent((L7_uint32)L7_AF_INET,
                   L7_MRP_DVMRP) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "MGMD De-Registration Failed");
  }

  if (dvmrpcb->report_entries != L7_NULLPTR)
  {
    DVMRP_FREE (L7_AF_INET, dvmrpcb->report_entries);
  }

  if (dvmrpcb != L7_NULLPTR)
  {
    DVMRP_FREE (L7_AF_INET, dvmrpcb);
  }

  dvmrpCB = L7_NULLPTR;      

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:LEAVING  \n",__FUNCTION__);         
}

/*********************************************************************
*
* @purpose  To Initialize the Memory for DVMRP Protocol
*
* @param    addrFamily @b{ (input) } Address Family Identifier
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
dvmrpMemoryInit (L7_uchar8 addrFamily)
{
  DVMRP_DEBUG (DVMRP_DEBUG_APIS, "Entry");

  if (dvmrpUtilMemoryInit (addrFamily) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "DVMRP Memory Initialization Failed for "
                 "addrFamily - %d", addrFamily);
    return L7_FAILURE;
  }

  DVMRP_DEBUG (DVMRP_DEBUG_INFO, "DVMRP Memory Initialization Successful for "
               "addrFamily - %d", addrFamily);

  DVMRP_DEBUG (DVMRP_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To De-Initialize the Memory for DVMRP Protocol
*
* @param    addrFamily @b{ (input) } Address Family Identifier
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
dvmrpMemoryDeInit (L7_uchar8 addrFamily)
{
  DVMRP_DEBUG (DVMRP_DEBUG_APIS, "Entry");

  /* Validations */
  if (addrFamily != L7_AF_INET)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Bad/Unsupported Address Family - %d", addrFamily);
    return L7_SUCCESS;
  }

  /* De-Initialize the Heap memory */
  if (heapDeInit (mcastMapHeapIdGet (addrFamily), L7_FALSE) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Heap De-Init Failed for Family - %d",
                 addrFamily);
    return L7_FAILURE;
  }

  DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "DVMRP Memory De-Initialization Successful");

  DVMRP_DEBUG (DVMRP_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  This function initializes the DVMRP task.
 *
 * @param   dvmrpmapcb - Pointer by reference to where the control block pointer 
 *                                      is stored in mapping layer
 *     
 * @returns  L7_SUCCESS - Protocol has been initialized properly.
 *
 * @returns  L7_ERROR   - Protocol has not been initialized properly.
 *
 * @notes    This function initializes all the control block parameters
 *       
 * @end
 *********************************************************************/
L7_int32 dvmrp_init (void **dvmrpmapcb)
{
  L7_uint32 maxTimers = DVMRP_MAX_TIMERS;
  L7_uint32 Ipv4Addr = L7_NULL;
  L7_uint32 treeHeapSize;
  L7_uint32 dataHeapSize;
  radixTree_t *pTreeNode = L7_NULLPTR;
  dvmrp_t *dvmrpcb = L7_NULLPTR; 
  L7_uint32 nbrCount=0; 
  L7_uint32 intfCount=0; 
  L7_uint32 appTimerbufPoolId;
  void *handleListMemHndl = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: Task Initialization started\n",
              __FUNCTION__);

  dvmrpcb =dvmrpCB; 

  if (dvmrpcb != L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "\n\nINVALID : dvmrp_init() called more than once!\n\n");
    return L7_ERROR;
  }

  dvmrpCB = DVMRP_ALLOC (L7_AF_INET, sizeof(dvmrp_t));

  dvmrpcb =dvmrpCB; 

  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "dvmrp_init: Error allocating DVMRP control block \n");
    return L7_ERROR;
  }
  dvmrpcb->report_entries = (dvmrp_report_t*) DVMRP_ALLOC(L7_AF_INET, sizeof(dvmrp_report_t) * platRtrRouteMaxEntriesGet());
  if (dvmrpcb->report_entries == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "dvmrp_init: Error allocating DVMRP control block \n");
    return L7_ERROR;
  }
  dvmrpcb->family= L7_AF_INET;
  dvmrpcb->proto = L7_DVMRP_PROTO;
  dvmrpcb->versionSupport = DVMRP_CAPABILITY_FLAGS << 16 |
                            DVMRP_MINOR_VERSION << 8 | 
                            DVMRP_MAJOR_VERSION;
  dvmrpcb->dvmrpOperFlag = L7_DISABLE;

  if (dvmrpSocketInfoApply(dvmrpcb) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"dvmrp_init: Error applying"
                "socket info dvmrpSocketInfoApply().\n");
    dvmrpClearInit(dvmrpcb);
    return L7_FAILURE;
  }

  osapiInetPton(dvmrpcb->family,DVMRP_ALL_ROUTERS,
                (L7_uchar8 *)&Ipv4Addr);
  if (L7_SUCCESS != inetAddressSet(dvmrpcb->family, &Ipv4Addr, 
                                   &dvmrpcb->all_routers.addr))
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Failed to set the address\n");
    dvmrpClearInit(dvmrpcb);
    return L7_FAILURE;
  }

  for (intfCount= 0; intfCount< MAX_INTERFACES; intfCount++)
  {
    dvmrpcb->dvmrp_interfaces[intfCount].index = DVMRP_INVALID_INDEX;
  }

  for (nbrCount= 1; nbrCount< DVMRP_MAX_NEIGHBORS; nbrCount++)
  {
    dvmrpcb->index2neighbor[nbrCount].status = DVMRP_ENTRY_NOT_USED;
  }

  /* create the handle list */
  if (mcastMapGetHandleListHandle (L7_AF_INET, &handleListMemHndl) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "Failed to get HandleList Handle");
    dvmrpClearInit(dvmrpcb);
    return L7_FAILURE;
  }
  maxTimers = MCAST_MAX_TIMERS;
  if (handleListInit (L7_FLEX_DVMRP_MAP_COMPONENT_ID, maxTimers,
                      &dvmrpcb->handle_list, (handle_member_t*) handleListMemHndl)
                   != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "Failed to Create Handle List");
    dvmrpClearInit(dvmrpcb);
    return L7_FAILURE;
  }

  if (mcastMapGetAppTimerBufPoolId(&appTimerbufPoolId) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "Unable to get Buffer Pool ID for App Timer");
    dvmrpClearInit(dvmrpcb);
    return L7_FAILURE;
  }

  if (L7_NULLPTR == (dvmrpcb->timerHandle = 
                     appTimerInit(L7_FLEX_DVMRP_MAP_COMPONENT_ID,
                                  dvmrpTimerExpiryHdlr,
                                  (void *)dvmrpcb,
                                  L7_APP_TMR_1SEC, appTimerbufPoolId)))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:%d timer instantiation failed \n", __FUNCTION__,__LINE__);
    dvmrpClearInit(dvmrpcb);
    return L7_FAILURE;
  }

  if (L7_SUCCESS != dvmrpCacheTableInit(dvmrpcb))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:%d Initialization of cache table failed\n",__FUNCTION__,__LINE__);
    dvmrpClearInit(dvmrpcb);
    return L7_FAILURE;
  }

  /* Initialize the Admin Scope Boundary database */
  if (dvmrpAdminScopeBoundaryInit (dvmrpcb) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Admin Scope database Creation Failure");
    dvmrpClearInit(dvmrpcb);
    return L7_FAILURE;
  }


/* calculate buffer sizes */
  treeHeapSize = RADIX_TREE_HEAP_SIZE(DVMRP_L3_ROUTE_TBL_SIZE_TOTAL, 
                                      sizeof(dvmrpTreeKey_t));
  dataHeapSize = RADIX_DATA_HEAP_SIZE(DVMRP_L3_ROUTE_TBL_SIZE_TOTAL, 
                                      sizeof (dvmrpRouteData_t));

  dvmrpcb->dvmrpRouteTreeHeap = DVMRP_ALLOC (L7_AF_INET, treeHeapSize);
  if (dvmrpcb->dvmrpRouteTreeHeap == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d Memory Allocation failed\n",
                __FUNCTION__,__LINE__);
    dvmrpClearInit(dvmrpcb);
    return L7_FAILURE;
  }

/* only need one for each best route */
  dvmrpcb->dvmrpRouteDataHeap = DVMRP_ALLOC (L7_AF_INET, dataHeapSize);
  if (dvmrpcb->dvmrpRouteDataHeap == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d Memory Allocation failed\n",
                __FUNCTION__,__LINE__);
    dvmrpClearInit(dvmrpcb);
    return L7_FAILURE;
  }

/* The destination networks are stored in a radix tree.
 * NOTE: Its important to ensure that the correct max_routes is
 * passed in here! Radix uses the same RADIX_..SIZE macros while
 * initializing the structs. A mismatch will cause a crash!
 */
  if (L7_NULLPTR == (pTreeNode = radixCreateTree(&dvmrpcb->dvmrpRouteTreeData,
                                                 dvmrpcb->dvmrpRouteDataHeap,
                                                 dvmrpcb->dvmrpRouteTreeHeap,
                                                 DVMRP_L3_ROUTE_TBL_SIZE_TOTAL,
                                                 sizeof(dvmrpRouteData_t), 
                                                 sizeof(dvmrpTreeKey_t))))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d radixTree creation failed\n",
                __FUNCTION__,__LINE__);
    dvmrpClearInit(dvmrpcb);
    return L7_FAILURE;
  }

  if (L7_SUCCESS != dvmrpGroupTableInit(dvmrpcb))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:%d Initialization of Group table failed\n",__FUNCTION__,__LINE__);
    dvmrpClearInit(dvmrpcb);
    return L7_FAILURE;
  }

  if (osapiRWLockCreate(&dvmrpcb->dvmrpRwLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Failed to create DVMRP read-write lock.\n");

    dvmrpClearInit(dvmrpcb);
    return L7_FAILURE;
  }

  /* Register with MFC */
  if (mfcCacheEventRegister (L7_MRP_DVMRP, dvmrpMapEventChangeCallback, L7_FALSE, 0)
                          != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"MFC Registration Failed");
    dvmrpClearInit(dvmrpcb);
    return L7_FAILURE;
  }

  /********************************************
   * Register callback with IGMP              *
   ********************************************/

  if (mgmdMapRegisterMembershipChangeEvent(L7_AF_INET, 
                                           L7_MRP_DVMRP,
                                           dvmrpMapEventChangeCallback) != 
      L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Unable to register callback with IGMP.\n");
    return L7_FAILURE;
  }
  

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: Task Initialization is successful\n",
              __FUNCTION__);
  *(dvmrpmapcb) = dvmrpcb;
  return(L7_SUCCESS);
}

/*********************************************************************
 * @purpose  This function creates IGMP raw socket to be used by DVMRP for
 *           it's message RX/TX.
 *
 * @param    cb -Dvmrp control block pointer.
 *
 * @return   L7_SUCCESS -On successful creation of socket
 * @return   L7_FAILURE -when the socket creation fails.
 *
 *
 * @notes   None
 * @end
 *********************************************************************/
L7_RC_t dvmrpSocketInfoApply(void *cb)
{
  L7_int32 sockFD = -1;
  L7_int32 precedence = 0xC0;
  L7_BOOL  sockOpt = L7_FALSE;
  dvmrp_t *dvmrpcb=L7_NULLPTR;  

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  dvmrpcb =cb;  

  if (dvmrpcb== L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

/* Create a socket for dvmrp send/receive */
  if (osapiSocketCreate (dvmrpcb->family, L7_SOCK_RAW, IPPROTO_IGMP,
                         &sockFD)
      != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:%d socket creation failure\n",__FUNCTION__,__LINE__);
    return(L7_FAILURE);
  }
  if (L7_SUCCESS != osapiSetsockopt(sockFD, IPPROTO_IP, L7_IP_TOS, 
                                    (char*)&precedence, sizeof(L7_int32)))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:%d set sock option failed\n",__FUNCTION__,__LINE__);
    return(L7_FAILURE);
  }

  /* This can be enabled only for systems supporting this option
   * Disable loopback of the multicast packets.
   * Ignore failure as its not supported on all platforms
   */
  sockOpt= L7_FALSE;
  if(L7_SUCCESS != osapiSetsockopt(sockFD, IPPROTO_IP, L7_IP_MULTICAST_LOOP,
                                   (L7_uchar8 *)&sockOpt, sizeof(sockOpt)))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"IP_MULTICAST_LOOP socket option Failed \n");
  }

  dvmrpcb->sockFd = sockFD;
  DVMRP_DEBUG(DVMRP_DEBUG_INFO,"%s:%d Value of sockFd=%d\n",
              __FUNCTION__,__LINE__,sockFD);

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of routes in the DVMRP routing table
*
* @param    numRoutes   @b{(output)}  Number of Routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpNumRoutesGet(L7_uint32 *numRoutes)
{
  L7_int32 rts = 0;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  *numRoutes = 0;

  rts = radixTreeCount(&dvmrpCB->dvmrpRouteTreeData);
  if (rts != 0)
  {
    rts--;  /*DVMRP internally maintains 0.0.0.0/default route, eliminate it 
             * by (-1)
             */
  }

  *numRoutes = rts;
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_SUCCESS;
}

/*********************************************************************
  * @purpose  Gets the number of routes in the DVMRP routing table with
  *           non-infinite metric
  *
  * @param    reachable    -  @b{(output)}  Number of routes with non-infinite
  *                                         metrics 
  *
  * @returns  L7_SUCCESS
  * @returns  L7_FAILURE
  *
  * @notes    none
  *
  * @end
  *********************************************************************/
L7_RC_t dvmrpReachableRoutesGet(L7_uint32 *reachable)
{
  L7_int32 rts = 0;
  dvmrpRouteData_t *pData = L7_NULLPTR;
  dvmrpRouteData_t *pNextData = L7_NULLPTR;
  dvmrp_route_t *route = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  *reachable = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  pData = radixGetNextEntry(&dvmrpCB->dvmrpRouteTreeData, pData);
  if (pData == L7_NULLPTR)
  {
    /* If network is not found then we are done.*/
    *reachable = 0;
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_SUCCESS;
  }

/* Iterate through all destinations */
  while (pData != L7_NULLPTR)
  {
    /* Next destination. */
    pNextData = radixGetNextEntry(&dvmrpCB->dvmrpRouteTreeData, pData);

    route = &pData->dvmrpRouteInfo;
    if (route->metric < DVMRP_METRIC_INFINITY)
      rts++;
    pData = pNextData;      
  }

  *reachable = rts;
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the DVMRP Generation ID used by the router
*
* @param    rtrIfNum       -  @b{(input)}  Router Interface Number
* @param    genId          -  @b{(output)} Generation ID.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpIntfGenerationIDGet(L7_int32 rtrIfNum, L7_ulong32 *genId)
{
  dvmrp_interface_t  *interface = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  *genId = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  interface = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
  if (interface->index != rtrIfNum)
  {
    return L7_FAILURE;
  }
  *genId = interface ->genid;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    rtrIfNum   -  @b{(input)}   Router interface number
* @param    status      -  @b{(output)}  status
*
* @returns  L7_SUCCESS    if success
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE    if failure
*
* @notes    needs only rtrIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpIntfStatusGet(L7_uint32 rtrIfNum,L7_int32 *status)
{
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  *status = DVMRP_MAP_INTF_STATE_NONE;
  if (MCAST_BITX_TEST(dvmrpCB->interface_mask, rtrIfNum))
  {
    *status = DVMRP_MAP_INTF_STATE_UP;
  }
  else
  {
    *status = DVMRP_MAP_INTF_STATE_DOWN;
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of invalid packets received on this interface.
*
* @param    rtrIfNum  -  @b{(input)} Router interface number
* @param    badPkts    -  @b{(output)} Bad Packets
*
* @returns  L7_SUCCESS     if success
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if failure
*
* @notes    needs only rtrIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpIntfRcvBadPktsGet(L7_uint32 rtrIfNum,L7_ulong32 *badPkts)
{
  dvmrp_interface_t  *interface = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  *badPkts = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  interface = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
  if (interface->index != rtrIfNum)
  {
    return L7_FAILURE;
  }
  *badPkts = interface ->badPkts;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of invalid routes received on this interface.
*
* @param    rtrIfNum   -  @b{(input)}   Router interface number
* @param    badRts     -  @b{(output)}  Bad Routes
*
* @returns  L7_SUCCESS      if success
* @returns  L7_ERROR        if interface does not exist
* @returns  L7_FAILURE      if failure
*
* @notes    needs only rtrIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpIntfRcvBadRoutesGet(L7_uint32 rtrIfNum,L7_ulong32 *badRts)
{
  dvmrp_interface_t  *interface = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  *badRts = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  interface = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
  if (interface->index != rtrIfNum)
  {
    return L7_FAILURE;
  }
  *badRts = interface ->badRts;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of routes sent on this interface.
*
* @param    rtrIfNum  -   @b{(input)}    Router interface number
* @param    sent       -  @b{(output)}   Sent Routes
*
* @returns  L7_SUCCESS    if success
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE    if failure
*
* @notes    needs only rtrIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpIntfSentRoutesGet(L7_uint32 rtrIfNum, L7_ulong32 *sent)
{
  dvmrp_interface_t  *interface = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  *sent = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  interface = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
  if (interface->index != rtrIfNum)
  {
    return L7_FAILURE;
  }
  *sent = interface ->sentRts;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get if the entry of Interfaces is valid or not.
*
* @param    rtrIfNum   -  @b{(input)} Router Interface Number
*
* @returns  L7_SUCCESS    if success
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE    if failure
*
* @notes    needs only rtrIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpIntfGet(L7_uint32 rtrIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  dvmrp_interface_t  *interface = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  interface = &dvmrpCB->dvmrp_interfaces[rtrIfNum];

  if (interface->index == rtrIfNum)
  {
    rc = L7_SUCCESS;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
* @purpose  Get the state for the specified neighbour of the specified 
*           interface.
*
* @param    rtrIfNum   -  @b{(input)}  Router interface number
* @param    nbrIpAddr  -  @b{(input)}  IP Address of the neighbour
* @param    state      -  @b{(output)}  state
*
* @returns  L7_SUCCESS    if success
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE    if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpNeighborStateGet(L7_uint32 rtrIfNum, L7_inet_addr_t *nbrIpAddr, 
                              L7_ulong32 *state)
{
  dvmrp_interface_t *interface ;
  dvmrp_neighbor_t *nbr = L7_NULLPTR, nbrNode;
  L7_RC_t rc = L7_FAILURE;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  *state = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  interface = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
  if (interface->index != rtrIfNum)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }
  memset(&nbrNode, L7_NULL, sizeof(dvmrp_neighbor_t));
  memcpy(&nbrNode.nbrAddr.addr, nbrIpAddr, sizeof(L7_inet_addr_t));
  nbrNode.interface = interface ;
  nbr = (dvmrp_neighbor_t *)SLLFind(&(interface->ll_neighbors), (void *)&nbrNode);
  if (nbr != L7_NULLPTR)
  {
    *state = nbr->state;
    rc = L7_SUCCESS;
  }

  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
* @purpose  Check whether router interface running DVMRP 'rtrIfNum' is a leaf
*
* @param    rtrIfNum  -  @b{(input)}  Router interface number
*
* @returns  L7_SUCCESS       if leaf
* @returns  L7_FAILURE       if interface is not leaf
*
* @notes    needs only rtrIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpIsInterfaceLeaf(L7_uint32 rtrIfNum)
{
  dvmrp_interface_t *interface  = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_ERROR;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  interface  = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
  if (interface ->index != rtrIfNum)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_ERROR;
  }

  if (BIT_TEST(interface ->flags, DVMRP_VIF_LEAF))
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_SUCCESS; /*leaf TRUE*/
  }

  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_FAILURE; /*FALSE*/
}

/*********************************************************************
* @purpose  Chech whether DVMRP is a leaf router
*
* @param    void
*
* @returns  L7_TRUE  if leaf router
* @returns  L7_FALSE, if not a leaf router
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dvmrpRouterIsLeaf(void)
{
  dvmrp_interface_t *interface = L7_NULLPTR;
  L7_uint32 numOfLeafs = 0;
  L7_uint32 numOfInterfaces = 0;
  L7_uint32 uiIntf = 0;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_ERROR;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  for (uiIntf = 0;uiIntf < MAX_INTERFACES;uiIntf++)
  {
    interface = &dvmrpCB->dvmrp_interfaces[uiIntf];
    if (interface->index != uiIntf)
    {
      break;
    }
    numOfInterfaces++;
    if (BIT_TEST(interface->flags, DVMRP_VIF_LEAF))
    {
      numOfLeafs++; /*leaf TRUE*/
    }
  }
  if (numOfLeafs == (numOfInterfaces - 1))
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_SUCCESS; /*Leaf Router*/
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  return L7_FAILURE; /*Non-Leaf Router*/
}

/*********************************************************************
* @purpose  Get the DVMRP Uptime for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum   -  @b{(input)}  Router Interface Number
* @param    nbrIpAddr  -  @b{(input)}  IP Address of the neighbour
* @param    nbrUpTime  -  @b{(output)} Neighbor Up Time
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpNeighborUpTimeGet(L7_uint32 rtrIfNum, L7_inet_addr_t *nbrIpAddr, 
                               L7_ulong32 *nbrUpTime)
{
  dvmrp_interface_t *interface = L7_NULLPTR;
  dvmrp_neighbor_t *nbr = L7_NULLPTR, nbrNode;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 now;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         
  now = osapiUpTimeRaw();

  *nbrUpTime = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_ERROR;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  interface = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
  if (interface->index != rtrIfNum)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }
  memset(&nbrNode, L7_NULL, sizeof(dvmrp_neighbor_t));
  memcpy(&nbrNode.nbrAddr.addr, nbrIpAddr, sizeof(L7_inet_addr_t));
  nbrNode.interface = interface;
  nbr = (dvmrp_neighbor_t *)SLLFind(&(interface->ll_neighbors), (void *)&nbrNode);
  if (nbr != L7_NULLPTR)
  {
    if (nbr->state == L7_DVMRP_NEIGHBOR_STATE_DOWN)
    {
      *nbrUpTime = 0;
    }
    else
    {
      *nbrUpTime = now -(nbr->ctime);
    }
    rc = L7_SUCCESS;
  }

  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
* @purpose  Get the DVMRP Expiry time for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum   -  @b{(input)}  Router Interface Number
* @param    nbrIpAddr  -  @b{(input)}  IP Address of the neighbour
* @param    nbrExpTime -  @b{(output)}  Neighbor Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR,     if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpNeighborExpiryTimeGet(L7_uint32 rtrIfNum, 
                                   L7_inet_addr_t *nbrIpAddr, 
                                   L7_ulong32 *nbrExpTime)
{
  L7_int32 timeLeft;
  dvmrp_interface_t *interface = L7_NULLPTR;
  dvmrp_neighbor_t *nbr = L7_NULLPTR, nbrNode;
  L7_RC_t rc = L7_FAILURE;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         
  *nbrExpTime = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_ERROR;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  interface = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
  if (interface->index != rtrIfNum)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }
  memset(&nbrNode, L7_NULL, sizeof(dvmrp_neighbor_t));
  memcpy(&nbrNode.nbrAddr.addr, nbrIpAddr, sizeof(L7_inet_addr_t));
  nbrNode.interface = interface;
  nbr = (dvmrp_neighbor_t *)SLLFind(&(interface->ll_neighbors), (void *)&nbrNode);
  if (nbr != L7_NULLPTR)
  {
    if (nbr->timeout != L7_NULLPTR)
    {
      if (L7_SUCCESS != appTimerTimeLeftGet(dvmrpCB->timerHandle, nbr->timeout,
                                            &timeLeft))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                    "%s:Failed to get the time left for expiry:%d",
                    __FUNCTION__,__LINE__);
        *nbrExpTime = 0;
        osapiReadLockGive(dvmrpCB->dvmrpRwLock);
        return L7_FAILURE;
      }
    }
    else
    {
      timeLeft = 0;
    }
    *nbrExpTime = (timeLeft <= 0) ? 0 : timeLeft;
    rc = L7_SUCCESS;
  }

  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
* @purpose  Get the DVMRP Generation ID for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum     -  @b{(input)} router Interface Number
* @param    nbrIpAddr    -  @b{(input)} IP Address of the neighbour
* @param    nbrGenId     -  @b{(output)} Neighbor Generation ID
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR,       if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpNeighborGenIdGet(L7_uint32 rtrIfNum, L7_inet_addr_t *nbrIpAddr, 
                              L7_ulong32 *nbrGenId)
{
  dvmrp_interface_t *interface = L7_NULLPTR;
  dvmrp_neighbor_t *nbr = L7_NULLPTR, nbrNode;
  L7_RC_t rc = L7_FAILURE;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         
  *nbrGenId = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  interface = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
  if (interface->index != rtrIfNum)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }

  memset(&nbrNode, L7_NULL, sizeof(dvmrp_neighbor_t));
  memcpy(&nbrNode.nbrAddr.addr, nbrIpAddr, sizeof(L7_inet_addr_t));
  nbrNode.interface = interface;
  nbr = (dvmrp_neighbor_t *)SLLFind(&(interface->ll_neighbors), (void *)&nbrNode);
  if (nbr != L7_NULLPTR)
  {
    *nbrGenId = nbr->genid;
    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get the DVMRP Major Version for the specified neighbour
 *           of the specified interface
 *
 * @param    rtrIfNum      -  @b{(input)}  Router Interface Number
 * @param    nbrIpAddr     -  @b{(input)}  IP Address of the neighbour
 * @param    majorVersion  -  @b{(output)} Neighbor Major Version
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR,        if interface does not exist
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpNeighborMajorVersionGet(L7_uint32 rtrIfNum, 
                                     L7_inet_addr_t *nbrIpAddr, L7_ushort16 *majorVersion)
{
  L7_RC_t rc = L7_FAILURE;
  dvmrp_interface_t *interface = L7_NULLPTR;
  dvmrp_neighbor_t nbrNode, *nbr = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  *majorVersion = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  interface = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
  if (interface->index != rtrIfNum)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }
  memset(&nbrNode, L7_NULL, sizeof(dvmrp_neighbor_t));
  memcpy(&nbrNode.nbrAddr.addr, nbrIpAddr, sizeof(L7_inet_addr_t));
  nbrNode.interface = interface;
  nbr = (dvmrp_neighbor_t *)SLLFind(&(interface->ll_neighbors), (void *)&nbrNode);
  if (nbr != L7_NULLPTR)
  {
    *majorVersion = nbr->versionSupport & 0xff;
    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get the DVMRP Minor Version for the specified neighbour
 *           of the specified interface
 *
 * @param    rtrIfNum      -  @b{(input)}  Router Interface Number
 * @param    nbrIpAddr     -  @b{(input)}  IP Address of the neighbour
 * @param    minorVersion  -  @b{(output)} Neighbor Minor Version
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR,        if interface does not exist
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpNeighborMinorVersionGet(L7_uint32 rtrIfNum, 
                                     L7_inet_addr_t *nbrIpAddr, L7_ushort16 *minorVersion)
{
  L7_RC_t rc = L7_FAILURE;
  dvmrp_interface_t *interface = L7_NULLPTR;
  dvmrp_neighbor_t *nbr = L7_NULLPTR, nbrNode;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  *minorVersion = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  interface = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
  if (interface->index != rtrIfNum)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }
  memset(&nbrNode, L7_NULL, sizeof(dvmrp_neighbor_t));
  memcpy(&nbrNode.nbrAddr.addr, nbrIpAddr, sizeof(L7_inet_addr_t));
  nbrNode.interface = interface;
  nbr = (dvmrp_neighbor_t *)SLLFind(&(interface->ll_neighbors), (void *)&nbrNode);
  if (nbr != L7_NULLPTR)
  {
    *minorVersion = (nbr->versionSupport >> 8) & 0xff;
    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get the DVMRP capabilities for the specified neighbour
 *           of the specified interface
 *
 * @param    rtrIfNum     -  @b{(input)}  Router Interface Number
 * @param    nbrIpAddr    -  @b{(input)}  IP Address of the neighbour
 * @param    capabilities -  @b{(output)} Neighbor Capabilities
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpNeighborCapabilitiesGet(L7_uint32 rtrIfNum, 
                                     L7_inet_addr_t *nbrIpAddr, L7_ushort16 *capabilities)
{
  dvmrp_interface_t *interface = L7_NULLPTR;
  dvmrp_neighbor_t *nbr = L7_NULLPTR, nbrNode;
  L7_RC_t rc = L7_FAILURE;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  *capabilities = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  interface = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
  if (interface->index != rtrIfNum)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }
  memset(&nbrNode, L7_NULL, sizeof(dvmrp_neighbor_t));
  memcpy(&nbrNode.nbrAddr.addr, nbrIpAddr, sizeof(L7_inet_addr_t));
  nbrNode.interface = interface;
  nbr = (dvmrp_neighbor_t *)SLLFind(&(interface->ll_neighbors), (void *)&nbrNode);
  if (nbr != L7_NULLPTR)
  {
    *capabilities = (nbr->versionSupport >> 16) & 0x00FF;
    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get the number of routes received for the specified neighbour
 *           of the specified interface.
 *
 * @param    rtrIfNum   -  @b{(input)}  Router interface number
 * @param    nbrIpAddr  -  @b{(input)}  IP Address of the neighbour
 * @param    rcvRoutes  -  @b{(output)}  Received Routes
 *
 * @returns  L7_SUCCESS    if success
 * @returns  L7_ERROR,     if interface does not exist
 * @returns  L7_FAILURE    if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpNeighborRcvRoutesGet(L7_uint32 rtrIfNum, 
                                  L7_inet_addr_t *nbrIpAddr, L7_ulong32 *rcvRoutes)
{
  int rts = 0;
  dvmrpRouteData_t *pData = L7_NULLPTR;
  dvmrp_route_t *route = L7_NULLPTR;
  dvmrpRouteData_t *pNextData = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  *rcvRoutes = rts;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

/* Gets the first entry in the route table */
  pData = radixGetNextEntry(&dvmrpCB->dvmrpRouteTreeData, L7_NULLPTR);  

/* Iterate through all destinations */
  while (pData != L7_NULLPTR)
  {
    /* Next destination. */
    pNextData = radixGetNextEntry(&dvmrpCB->dvmrpRouteTreeData, pData);

    route = &pData->dvmrpRouteInfo;

    if (route->neighbor != L7_NULLPTR)
    {
      if (L7_INET_ADDR_COMPARE(&route->neighbor->nbrAddr.addr, nbrIpAddr) == 0)
        rts++;
    }
    pData = pNextData;      
  }

  *rcvRoutes = rts;
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the number of invalid packets received for the specified 
 *           neighbour of the specified interface.
 *
 * @param    rtrIfNum  -  @b{(input)}   Router interface number
 * @param    nbrIpAddr -  @b{(input)}   IP Address of the neighbour
 * @param    badPkts   -  @b{(output)}  Bad Packets
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpNeighborRcvBadPktsGet(L7_uint32 rtrIfNum, 
                                   L7_inet_addr_t *nbrIpAddr, L7_ulong32 *badPkts)
{
  L7_RC_t rc = L7_FAILURE;
  dvmrp_interface_t *interface = L7_NULLPTR;
  dvmrp_neighbor_t *nbr = L7_NULLPTR, nbrNode;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  *badPkts = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  interface = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
  if (interface->index != rtrIfNum)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }
  memset(&nbrNode, L7_NULL, sizeof(dvmrp_neighbor_t));
  memcpy(&nbrNode.nbrAddr.addr, nbrIpAddr, sizeof(L7_inet_addr_t));
  nbrNode.interface = interface;
  nbr = (dvmrp_neighbor_t *)SLLFind(&(interface->ll_neighbors), (void *)&nbrNode);
  if (nbr != L7_NULLPTR)
  {
    *badPkts = nbr->badPkts;
    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get the number of invalid routes received for the specified 
 *           neighbour of the specified interface.
 *
 * @param    rtrIfNum   -  @b{(input)}   internal interface number
 * @param    nbrIpAddr  -  @b{(input)}   IP Address of the neighbour
 * @param    badRts     -  @b{(output)}  Bad Routes
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpNeighborRcvBadRoutesGet(L7_uint32 rtrIfNum, 
                                     L7_inet_addr_t *nbrIpAddr, L7_ulong32 *badRts)
{
  L7_RC_t rc = L7_FAILURE;
  dvmrp_interface_t *interface = L7_NULLPTR;
  dvmrp_neighbor_t *nbr = L7_NULLPTR, nbrNode;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  *badRts = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  interface = &dvmrpCB->dvmrp_interfaces[rtrIfNum];
  if (interface->index != rtrIfNum)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }
  memset(&nbrNode, L7_NULL, sizeof(dvmrp_neighbor_t));
  memcpy(&nbrNode.nbrAddr.addr, nbrIpAddr, sizeof(L7_inet_addr_t));
  nbrNode.interface = interface;
  nbr = (dvmrp_neighbor_t *)SLLFind(&(interface->ll_neighbors), (void *)&nbrNode);
  if (nbr != L7_NULLPTR)
  {
    *badRts = nbr->badRts;
    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get an entry of Neighbors.
 *
 * @param    rtrIfNum     -  @b{(input)}   internal interface number
 * @param    nbrIpAddr    -  @b{(input)}   IP Address of the neighbour
 *
 * @returns  L7_SUCCESS    if success
 * @returns  L7_ERROR      if interface does not exist
 * @returns  L7_FAILURE    if failure
 *
 * @notes    needs only rtrIfNum.
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpNeighborEntryGet(L7_uint32 *rtrIfNum, L7_inet_addr_t *nbrIpAddr)
{
  L7_RC_t rc = L7_FAILURE;
  dvmrp_neighbor_t *nbr = L7_NULLPTR, nbrNode;
  dvmrp_interface_t *interface = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  interface = &dvmrpCB->dvmrp_interfaces[*rtrIfNum];
  if (interface->index != *rtrIfNum)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }
  memset(&nbrNode, L7_NULL, sizeof(dvmrp_neighbor_t));
  memcpy(&nbrNode.nbrAddr.addr, nbrIpAddr, sizeof(L7_inet_addr_t));
  nbrNode.interface = interface;
  nbr = (dvmrp_neighbor_t *)SLLFind(&(interface->ll_neighbors), (L7_sll_member_t *)&nbrNode);
  if (nbr != L7_NULLPTR)
  {
    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get the next entry on this interface.
 *
 * @param    rtrIfNum    -  @b{(input/output)}   internal interface number
 * @param    nbrIpAddr   -  @b{(input)}  IP Address of the neighbour
 *
 * @returns  L7_SUCCESS    if success
 * @returns  L7_ERROR      if interface does not exist
 * @returns  L7_FAILURE    if failure
 *
 * @notes    needs only intIfNum.
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpNeighborEntryNextGet(L7_uint32 *rtrIfNum, 
                                  L7_inet_addr_t *nbrIpAddr)
{
  dvmrp_interface_t *interface = L7_NULLPTR;
  dvmrp_neighbor_t tmpNbr, *nbr = L7_NULLPTR;
  L7_uint32 tempIfNum;
  L7_uint32 mode;
  L7_uint32   intIfNum;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
/* scan all the internal interfaces known to NIM starting with the initial 
 * one from the caller */
  if ((*rtrIfNum == 0) && (inetIsAddressZero(nbrIpAddr) == L7_TRUE))
  {
    tempIfNum = 0;
  }
  else
  {
    tempIfNum = *rtrIfNum;
  }
  for (;tempIfNum < MCAST_MAX_INTERFACES; tempIfNum++)
  {

    if (!MCAST_BITX_TEST(dvmrpCB->interface_mask, tempIfNum))
    {
      continue;
    }

    if (ipMapRtrIntfToIntIfNum(tempIfNum, &intIfNum) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Failed to convert Router Interface Number to \
                 Internal Interface Number\n");
      osapiReadLockGive(dvmrpCB->dvmrpRwLock);                        
      return L7_FAILURE;
    }


    /* if the current internal interface is a router interface */
    if (L7_SUCCESS != dvmrpMapIntfAdminModeGet(intIfNum, &mode))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:Failed to get the admon mode :%d",__FUNCTION__,__LINE__);
    }

    /* if this interface is enabled for DVMRP */
    if (mode == L7_ENABLE)
    {
      /* retrieve the interface data for this interface */
      interface = &dvmrpCB->dvmrp_interfaces[tempIfNum];
      if (interface->index != tempIfNum)
      {
        continue;
      }

      if (SLLNumMembersGet(&(interface->ll_neighbors)) == L7_NULL)
      {
        continue;
      }
      if (inetIsAddressZero(nbrIpAddr) == L7_TRUE)
      {
        nbr = (dvmrp_neighbor_t *)SLLFirstGet(&(interface->ll_neighbors));
        inetCopy(nbrIpAddr,&nbr->nbrAddr.addr);                    
        *rtrIfNum  = tempIfNum;   
        osapiReadLockGive(dvmrpCB->dvmrpRwLock);       
        return L7_SUCCESS;    
      }

      /* scan current interface's list of neighbors for the one that has 
       * the next higher ip address 
       */

      if (*rtrIfNum == tempIfNum)
      {
        inetCopy(&tmpNbr.nbrAddr.addr,nbrIpAddr);
        tmpNbr.interface = interface;
        nbr = (dvmrp_neighbor_t *)SLLFind(&(interface->ll_neighbors), (L7_sll_member_t *)&tmpNbr);
        if (nbr != L7_NULLPTR)
        {
          nbr = (dvmrp_neighbor_t *)SLLNextGet(&(interface->ll_neighbors), (L7_sll_member_t *)nbr);
          if (nbr != L7_NULLPTR)
          {
            inetCopy(nbrIpAddr,&nbr->nbrAddr.addr);            
            *rtrIfNum  = tempIfNum;   
            osapiReadLockGive(dvmrpCB->dvmrpRwLock);
            return L7_SUCCESS;    
          }
        }
      }
      else
      {
        nbr = (dvmrp_neighbor_t *)SLLFirstGet(&(interface->ll_neighbors));
        inetCopy(nbrIpAddr,&nbr->nbrAddr.addr);                    
        *rtrIfNum  = tempIfNum;   
        osapiReadLockGive(dvmrpCB->dvmrpRwLock);       
        return L7_SUCCESS;    
      }
    }
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  return L7_FAILURE;                
}

/*********************************************************************
 * @purpose  Gets the IP address of neighbor which is the source for
 *           the packets for a specified source address.
 *
 * @param    srcIpAddr  -  @b{(input)}   source IP Address
 * @param    srcMask    -  @b{(input)}   source Mask
 * @param    addr       -  @b{(output)}  IP Address
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if source address does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpRouteUpstreamNeighborGet(L7_inet_addr_t *srcIpAddr, 
                                      L7_inet_addr_t *srcMask, L7_inet_addr_t *addr)
{
  dvmrpRouteData_t *pData = L7_NULLPTR;
  L7_uint32 skipentries = 0;
  dvmrpTreeKey_t key;
  dvmrp_route_t *route = L7_NULLPTR;
  L7_inet_addr_t  route_network;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }


  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  memset(&route_network,0,sizeof(L7_inet_addr_t));  
  if (inetAddressAnd(srcIpAddr,srcMask, &route_network) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Inet And operation Failed!!!!!!!!\n"); 
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);    
    return L7_FAILURE;
  }
  dvmrpKeySet(&key, route_network);


/* find the longest match */
  pData = radixMatchNode(&dvmrpCB->dvmrpRouteTreeData, &key, skipentries);

  if (pData == L7_NULLPTR)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;    
  }

  route = &pData->dvmrpRouteInfo;
  if (route->neighbor)
  {
    inetCopy(addr ,&route->neighbor->nbrAddr.addr);                        
  }
  else
  {
    memset(addr,0,sizeof(L7_inet_addr_t));
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Gets the Interface index for a specific route for a specified 
 *           source address.
 *
 * @param    srcIpAddr  -  @b{(input)}  source IP Address
 * @param    srcMask    -  @b{(input)}  source Mask
 * @param    IfIndex    -  @b{(output)}  Interface Index which is passed back 
 *                                      to the calling function
 *
 * @returns  L7_SUCCESS   if success
 * @returns  L7_ERROR,    if source address does not exist
 * @returns  L7_FAILURE   if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpRouteIfIndexGet(L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask, 
                             L7_uint32 *rtrIfIndex)
{
  dvmrpRouteData_t *pData = L7_NULLPTR;
  L7_uint32 skipentries = 0;
  dvmrpTreeKey_t key;
  dvmrp_route_t *route = L7_NULLPTR;
  L7_inet_addr_t route_network;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  memset(&route_network,0,sizeof(L7_inet_addr_t));  
  if (inetAddressAnd(srcIpAddr,srcMask, &route_network) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Inet And operation Failed!!!!!!!!\n"); 
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);    
    return L7_FAILURE;
  }
  dvmrpKeySet(&key, route_network);

/* find the longest match */
  pData = radixMatchNode(&dvmrpCB->dvmrpRouteTreeData, &key, skipentries);

  if (pData == L7_NULLPTR)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }

  route = &pData->dvmrpRouteInfo;

  if (route->interface != L7_NULLPTR)
    *rtrIfIndex = route->interface->index;

  osapiReadLockGive(dvmrpCB->dvmrpRwLock);

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Gets the distance in hops for a specified source address.
 *
 * @param    srcIpAddr  -  @b{(input)}   source IP Address
 * @param    srcMask    -  @b{(input)}   source Mask
 * @param    metric     -  @b{(output)}   metric
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if source address does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpRouteMetricGet(L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask,
                            L7_ushort16 *metric)
{
  dvmrpRouteData_t *pData = L7_NULLPTR;
  L7_uint32 skipentries = 0;
  dvmrpTreeKey_t key;
  L7_RC_t rc = L7_FAILURE;
  dvmrp_route_t *route = L7_NULLPTR;
  L7_inet_addr_t route_network;


  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         
  *metric = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  memset(&route_network,0,sizeof(L7_inet_addr_t));  
  if (inetAddressAnd(srcIpAddr,srcMask, &route_network) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Inet And operation Failed!!!!!!!!\n"); 
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);    
    return L7_FAILURE;
  }
  dvmrpKeySet(&key, route_network);

/* find the longest match */
  pData = radixMatchNode(&dvmrpCB->dvmrpRouteTreeData, &key, skipentries);

  if (pData == L7_NULLPTR)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;    
  }
  route = &pData->dvmrpRouteInfo;

  *metric = route->metric;
  rc = L7_SUCCESS;

  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Gets the route expiry time for a specified source address.
 *
 * @param    srcIpAddr   -  @b{(input)}  source IP Address
 * @param    srcMask     -  @b{(input)}  source Mask
 * @param    rtExpTime   -  @b{(output)} route expiry time
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if source address does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpRouteExpiryTimeGet(L7_inet_addr_t *srcIpAddr, 
                                L7_inet_addr_t *srcMask, L7_ulong32 *rtExpTime)
{
  dvmrpRouteData_t *pData = L7_NULLPTR;
  L7_uint32 skipentries = 0;
  dvmrpTreeKey_t key;
  L7_RC_t rc = L7_FAILURE;
  dvmrp_route_t *route = L7_NULLPTR;
  L7_uint32 now = osapiUpTimeRaw();
  L7_inet_addr_t route_network;


  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         
  *rtExpTime = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  memset(&route_network,0,sizeof(L7_inet_addr_t));  
  if (inetAddressAnd(srcIpAddr,srcMask, &route_network) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Inet And operation Failed!!!!!!!!\n"); 
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);    
    return L7_FAILURE;
  }
  dvmrpKeySet(&key, route_network);

/* find the longest match */
  pData = radixMatchNode(&dvmrpCB->dvmrpRouteTreeData, &key, skipentries);

  if (pData == L7_NULLPTR)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;    
  }

  route = &pData->dvmrpRouteInfo;

  if (route->utime == 0)
  {
    *rtExpTime = 0; /* direct routes does not expire */
  }
  else if (BIT_TEST(route->flags,DVMRP_RT_HOLD))
  {
    *rtExpTime = ((int)(route->dtime - now) > 0 )?(route->dtime - now ):0;
  }
  else if (BIT_TEST(route->flags,DVMRP_RT_DELETE))
  {
    *rtExpTime = ((int)(route->dtime - now )>0)?(route->dtime - now):0;
  }
  else
  {
    /* showing zero for routes not marked for deletion/or hold*/
    *rtExpTime = now - route->utime;
  }
  rc = L7_SUCCESS;
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Gets the time when a route was learnt for a specified source 
 *           address.
 *
 * @param    srcIpAddr -  @b{(input)}   source IP Address
 * @param    srcMask   -  @b{(input)}   source Mask
 * @param    rtUpTime  -  @b{(output)}  route up time
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if source address does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpRouteUptimeGet(L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask,
                            L7_ulong32 *rtUpTime)
{
  dvmrpRouteData_t *pData = L7_NULLPTR;
  L7_uint32 skipentries = 0;
  dvmrpTreeKey_t key;
  dvmrp_route_t *route = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 now;
  L7_inet_addr_t route_network;


  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         
  *rtUpTime = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  now =osapiUpTimeRaw();

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  memset(&route_network,0,sizeof(L7_inet_addr_t));  
  if (inetAddressAnd(srcIpAddr,srcMask, &route_network) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Inet And operation Failed!!!!!!!!\n"); 
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);    
    return L7_FAILURE;
  }
  dvmrpKeySet(&key, route_network);


/* find the longest match */
  pData = radixMatchNode(&dvmrpCB->dvmrpRouteTreeData, &key, skipentries);

  if (pData == L7_NULLPTR)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;    
  }

  route = &pData->dvmrpRouteInfo;

  *rtUpTime = now - route->ctime;
  rc = L7_SUCCESS;

  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get an entry of Routes.
 *
 * @param    srcIpAddr -  @b{(input)}   source IP Address
 * @param    srcMask   -  @b{(input)}   source Mask
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 * 
 * @end
 *********************************************************************/
L7_RC_t dvmrpRouteEntryGet(L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 bitLen;
  dvmrpRouteData_t routeData, *pData = L7_NULLPTR;
  L7_inet_addr_t route_network;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }


  if ((inetIsAddressZero(srcIpAddr) == L7_TRUE) && (inetIsAddressZero(srcMask) == L7_TRUE))
  {
    return rc;
  }

  if (L7_SUCCESS != inetMaskToMaskLen(srcMask, &bitLen))
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nINVALID : Mask to length conversion Failed!\n\n");
    return rc;
  }

/* Check for contiguous mask. */
  if (L7_TRUE != inetIsMaskValid(srcMask))
  {
    return rc;
  }
  memset(&route_network,0,sizeof(L7_inet_addr_t));  
  memset(&routeData, L7_NULL, sizeof(dvmrpRouteData_t));
  if (inetAddressAnd(srcIpAddr,srcMask, &route_network) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Inet And operation Failed!!!!!!!!\n"); 
    return L7_FAILURE;
  }
  dvmrpKeySet(&routeData.network, route_network);
  dvmrpKeySet(&routeData.netmask, *srcMask);

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

/* See if we already have a route to this destination. */
  pData = radixLookupNode(&dvmrpCB->dvmrpRouteTreeData, &routeData.network,
                          &routeData.netmask, L7_RN_EXACT);

  if (pData != L7_NULLPTR)
  {
    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);
  return rc;
}

/*********************************************************************
 * @purpose  Get the next entry of Routes.
 *
 * @param    srcIpAddr -  @b{(input)}   source IP Address
 * @param    srcMask   -  @b{(input)}   source Mask
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 * 
 * @end
 *********************************************************************/
L7_RC_t dvmrpRouteEntryNextGet(L7_inet_addr_t *srcIpAddr,
                               L7_inet_addr_t *srcMask)
{
  dvmrpRouteData_t routeData, *pData = L7_NULLPTR;
  L7_inet_addr_t route_network;


  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  if ((inetIsAddressZero(srcIpAddr) == L7_TRUE) && (inetIsAddressZero(srcMask) == L7_TRUE))
  {
    pData = radixGetNextEntry(&dvmrpCB->dvmrpRouteTreeData, L7_NULLPTR);

    if (pData != L7_NULLPTR)
    {
      inetCopy(srcIpAddr,&pData->dvmrpRouteInfo.networkAddr.addr);      
      if (L7_SUCCESS != inetMaskLenToMask(dvmrpCB->family, pData->dvmrpRouteInfo.networkAddr.maskLength, srcMask))
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Couldn't convert masklen to mask\n");
        osapiReadLockGive(dvmrpCB->dvmrpRwLock);
        return L7_FAILURE;
      }

      osapiReadLockGive(dvmrpCB->dvmrpRwLock);
      return L7_SUCCESS; 
    }
  }
  memset(&route_network,0,sizeof(L7_inet_addr_t));  
  memset(&routeData, L7_NULL, sizeof(dvmrpRouteData_t));

  if (inetAddressAnd(srcIpAddr,srcMask, &route_network) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Inet And operation Failed!!!!!!!!\n"); 
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);    
    return L7_FAILURE;
  }
  dvmrpKeySet(&routeData.network, route_network);
  dvmrpKeySet(&routeData.netmask, *srcMask);  

  pData = radixLookupNode(&dvmrpCB->dvmrpRouteTreeData, &routeData.network,
                          &routeData.netmask, L7_RN_EXACT);

  if (pData != L7_NULLPTR)
  {
    pData = radixGetNextEntry(&dvmrpCB->dvmrpRouteTreeData, pData);

    if (pData != L7_NULLPTR)
    {
      inetCopy(srcIpAddr,&pData->dvmrpRouteInfo.networkAddr.addr);
      if (L7_SUCCESS != inetMaskLenToMask(dvmrpCB->family, pData->dvmrpRouteInfo.networkAddr.maskLength, srcMask))
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Couldn't convert masklen to mask\n");
        osapiReadLockGive(dvmrpCB->dvmrpRwLock);
        return L7_FAILURE;
      }
      osapiReadLockGive(dvmrpCB->dvmrpRwLock);  
      return L7_SUCCESS;
    }
    else
    {
      osapiReadLockGive(dvmrpCB->dvmrpRwLock);
      return L7_FAILURE;
    }
  }
  else
  {
    pData = radixLookupNode(&dvmrpCB->dvmrpRouteTreeData, &routeData.network,
                            &routeData.netmask, L7_RN_GETNEXT_ASCEND);
    if (pData != L7_NULLPTR)
    {
      inetCopy(srcIpAddr,&pData->dvmrpRouteInfo.networkAddr.addr);
      if (L7_SUCCESS != inetMaskLenToMask(dvmrpCB->family, pData->dvmrpRouteInfo.networkAddr.maskLength, srcMask))
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Couldn't convert masklen to mask\n");
        osapiReadLockGive(dvmrpCB->dvmrpRwLock);
        return L7_FAILURE;
      }
      osapiReadLockGive(dvmrpCB->dvmrpRwLock);  
      return L7_SUCCESS;
    }

    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }

}

/*********************************************************************
 * @purpose  Gets the type of next hop router - leaf or branch,
 *           for a specified source address and specified interface.
 *
 * @param    nextHopSrc     -  @b{(input)}  Next Hop source IP Address
 * @param    nextHopSrcMask -  @b{(input)}  Next Hop source Mask
 * @param    nextHopRtrIfIndex -  @b{(input)}  Next Hop Interface Index
 * @param    nextHopType    -  @b{(output)} Next Hop Type
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if source address does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpRouteNextHopTypeGet(L7_inet_addr_t *nextHopSrc, 
                                 L7_inet_addr_t *nextHopSrcMask, L7_uint32 nextHopRtrIfIndex, 
                                 L7_uint32 *nextHopType)
{
  dvmrpRouteData_t *pData = L7_NULLPTR;
  L7_uint32 skipentries = 0;
  dvmrpTreeKey_t key;
  dvmrp_route_t *route = L7_NULLPTR;
  neighbor_bitset_t nbrBitset;
  dvmrp_neighbor_t *nbr = L7_NULLPTR;
  L7_BOOL found = L7_FALSE;
  L7_uint32 index = 0;
  L7_inet_addr_t route_network;  

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  *nextHopType = 0;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  memset(&route_network,0,sizeof(L7_inet_addr_t));  
  if (inetAddressAnd(nextHopSrc,nextHopSrcMask, &route_network) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Inet And operation Failed!!!!!!!!\n"); 
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);    
    return L7_FAILURE;
  }
  dvmrpKeySet(&key, route_network);



/* find the longest match */
  pData = radixMatchNode(&dvmrpCB->dvmrpRouteTreeData, &key, skipentries);

  if (pData == L7_NULLPTR)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_SUCCESS;
  }

  route = &pData->dvmrpRouteInfo;
  nbrBitset = route->dependents;
  for (index = 0; index < DVMRP_MAX_NEIGHBORS; index++)
  {
    if (MCAST_BITX_TEST(nbrBitset.bits, index))
    {
      nbr = &dvmrpCB->index2neighbor[index];
      if ((nbr->status != DVMRP_ENTRY_NOT_USED) && (nbr->interface->index == nextHopRtrIfIndex))
      {
        found = L7_TRUE;
        break;
      }
    }
  }
  if (found != L7_TRUE)
  {
    *nextHopType = L7_DVMRP_NEXTHOP_TYPE_LEAF;
  }
  else
  {
    *nextHopType = L7_DVMRP_NEXTHOP_TYPE_BRANCH;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get an entry of next hops.
 *
 * @param    nextHopSrc     -  @b{(input)}   Next Hop source IP Address
 * @param    nextHopSrcMask -  @b{(input)}   Next Hop source Mask
 * @param    rtrIfNum       -  @b{(input)}   Router Interface Number
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    needs only intIfNum.
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpRouteNextHopEntryGet(L7_inet_addr_t *nextHopSrc, 
                                  L7_inet_addr_t *nextHopSrcMask, L7_uint32 *rtrIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  dvmrpRouteData_t *pData = L7_NULLPTR;

  L7_inet_addr_t route_network;    
  dvmrpRouteData_t routeData;


  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (*rtrIfNum == 0)
  {
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  memset(&route_network,0,sizeof(L7_inet_addr_t));  
  if (inetAddressAnd(nextHopSrc,nextHopSrcMask, &route_network) != L7_SUCCESS)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Inet And operation Failed!!!!!!!!\n"); 
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);    
    return L7_FAILURE;
  }

  dvmrpKeySet(&routeData.network, route_network);
  dvmrpKeySet(&routeData.netmask, *nextHopSrcMask);  

  pData = radixLookupNode(&dvmrpCB->dvmrpRouteTreeData, &routeData.network,
                          &routeData.netmask, L7_RN_EXACT);


  if (pData == L7_NULLPTR)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;    
  }

  if (MCAST_BITX_TEST(dvmrpCB->interface_mask, *rtrIfNum))
  {
    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get the next entry of nexthops.
 *
 * @param    nextHopSrc       -  @b{(input)}          Next Hop source IP Address
 * @param    nextHopSrcMask   -  @b{(input)}          Next Hop source Mask
 * @param    nextHopRtrIfIndex   -  @b{(input/output)}   Next hop Index
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpRouteNextHopEntryNextGet(L7_inet_addr_t *nextHopSrc, 
                                      L7_inet_addr_t *nextHopSrcMask, L7_uint32 *nextHopRtrIfIndex)
{
  dvmrpRouteData_t *pData= L7_NULLPTR;
  dvmrpRouteData_t *pNextData = L7_NULLPTR;
  L7_uint32 tempIfNum;     
  dvmrp_interface_t *interface = L7_NULLPTR;
  dvmrpRouteData_t routeData;
  L7_inet_addr_t route_network;


  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }


  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  if ((inetIsAddressZero(nextHopSrc) == L7_TRUE) && (inetIsAddressZero(nextHopSrcMask) == L7_TRUE))
  {

    pData = radixGetNextEntry(&dvmrpCB->dvmrpRouteTreeData, L7_NULLPTR);
    if (pData != L7_NULLPTR)
    {
      for (tempIfNum = 1;(tempIfNum < MCAST_MAX_INTERFACES); tempIfNum++)
      {

        if (MCAST_BITX_TEST(dvmrpCB->interface_mask, tempIfNum))
        {
          /* retrieve the interface data for this interface */
          interface = &dvmrpCB->dvmrp_interfaces[tempIfNum];
          if (interface->index == tempIfNum)
          {
            inetCopy(nextHopSrc,&pData->network.addr);
            inetCopy(nextHopSrcMask,&pData->netmask.addr);            
            *nextHopRtrIfIndex  = tempIfNum;
            osapiReadLockGive(dvmrpCB->dvmrpRwLock);
            return L7_SUCCESS;       
          }
        }
      }
    }
    else
    {
      osapiReadLockGive(dvmrpCB->dvmrpRwLock);
      return L7_FAILURE;       
    }
  }
  else
  {
    memset(&route_network,0,sizeof(L7_inet_addr_t));  
    memset(&routeData, L7_NULL, sizeof(dvmrpRouteData_t));

    if (inetAddressAnd(nextHopSrc,nextHopSrcMask, &route_network) != L7_SUCCESS)
    {
      DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Inet And operation Failed!!!!!!!!\n"); 
      osapiReadLockGive(dvmrpCB->dvmrpRwLock);    
      return L7_FAILURE;
    }
    dvmrpKeySet(&routeData.network, route_network);
    dvmrpKeySet(&routeData.netmask, *nextHopSrcMask);  

    pData = radixLookupNode(&dvmrpCB->dvmrpRouteTreeData, &routeData.network,
                            &routeData.netmask, L7_RN_EXACT);

    if (pData == L7_NULLPTR)
    {
      pData = radixLookupNode(&dvmrpCB->dvmrpRouteTreeData, &routeData.network,
                              &routeData.netmask, L7_RN_GETNEXT_ASCEND );
      if (pData != L7_NULLPTR)
      {
        for (tempIfNum = *nextHopRtrIfIndex;(tempIfNum < MCAST_MAX_INTERFACES); tempIfNum++)
        {

          if (MCAST_BITX_TEST(dvmrpCB->interface_mask, tempIfNum))
          {
            /* retrieve the interface data for this interface */
            interface = &dvmrpCB->dvmrp_interfaces[tempIfNum];
            if (interface->index == tempIfNum)
            {
              inetCopy(nextHopSrc,&pData->network.addr);
              inetCopy(nextHopSrcMask,&pData->netmask.addr);            
              *nextHopRtrIfIndex  = tempIfNum;
              osapiReadLockGive(dvmrpCB->dvmrpRwLock);
              return L7_SUCCESS;       
            }
          }
        }
      }
      else
      {
        osapiReadLockGive(dvmrpCB->dvmrpRwLock);
        return L7_FAILURE;       
      }

    }
    while (pData != L7_NULLPTR)
    {
      pNextData = radixGetNextEntry(&dvmrpCB->dvmrpRouteTreeData, pData);
      for (tempIfNum = *nextHopRtrIfIndex+1;(tempIfNum < MCAST_MAX_INTERFACES); tempIfNum++)
      {
        /* if this interface is enabled for DVMRP */
        if (MCAST_BITX_TEST(dvmrpCB->interface_mask, tempIfNum))
        {
          /* retrieve the interface data for this interface */
          interface = &dvmrpCB->dvmrp_interfaces[tempIfNum];
          if (interface->index == tempIfNum)
          {
            inetCopy(nextHopSrc,&pData->network.addr);                
            inetCopy(nextHopSrcMask,&pData->netmask.addr);                            
            *nextHopRtrIfIndex  = tempIfNum;
            osapiReadLockGive(dvmrpCB->dvmrpRwLock);       
            return L7_SUCCESS;
          }

        }
      }
      *nextHopRtrIfIndex =L7_NULL;
      pData =pNextData;
    }
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Gets the prune expiry time for a group and specified source 
 *           address.
 *
 * @param    pruneGrp       -  @b{(input)}   prune group
 * @param    pruneSrc       -  @b{(input)}   prune source
 * @param    pruneSrcMask   -  @b{(input)}   prune source mask
 * @param    pruneExpTime   -  @b{(output)}  prune expiry time
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if source address does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpPruneExpiryTimeGet(L7_inet_addr_t *pruneGrp, 
                                L7_inet_addr_t *pruneSrc, 
                                L7_inet_addr_t *pruneSrcMask, 
                                L7_ulong32 *pruneExpTime)
{
  dvmrp_cache_entry_t *entry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 now;	


  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

   now = osapiUpTimeRaw();
  *pruneExpTime = L7_NULL;
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  entry = dvmrp_cache_lookup (dvmrpCB,pruneSrc, pruneGrp);
  if (entry)
  {
    *pruneExpTime = entry->expire - osapiUpTimeRaw();
    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get an entry of prunes.
 *
 * @param    pruneGrp       -  @b{(input)}   prune group
 * @param    pruneSrc       -  @b{(input)}   prune source
 * @param    pruneSrcMask   -  @b{(input)}   prune source mask
 *
 * 
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpPruneEntryGet(L7_inet_addr_t *pruneGrp, L7_inet_addr_t *pruneSrc, 
                           L7_inet_addr_t *pruneSrcMask)
{
  L7_RC_t rc = L7_FAILURE;
  dvmrp_cache_entry_t *entry = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  entry = dvmrp_cache_lookup (dvmrpCB,pruneSrc, pruneGrp);
  if ((entry != L7_NULLPTR) && (entry->count > 0))
  {
    rc = L7_SUCCESS;
  }

  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get the next entry of prunes.
 *
 *
 * @param    pruneGrp       -  @b{(input)}   prune group
 * @param    pruneSrc       -  @b{(input)}   prune source
 * @param    pruneSrcMask   -  @b{(input)}   prune source mask
 * 
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpPruneEntryNextGet(L7_inet_addr_t *pruneGrp, 
                               L7_inet_addr_t *pruneSrc, L7_inet_addr_t *pruneSrcMask)
{
  dvmrp_cache_entry_t tmpEntry, *entry = L7_NULLPTR,*entryNext = L7_NULLPTR;
  L7_uchar8 bitlen;
  L7_int32 compGrp = L7_NULL;
  L7_int32 compSrc = L7_NULL;
  dvmrp_route_t *route = L7_NULLPTR;
  dvmrpRouteData_t *pData = L7_NULLPTR;
  dvmrpTreeKey_t key;
  L7_uint32 skipentries = 0;
  L7_char8 src[IPV6_DISP_ADDR_LEN], grp[IPV6_DISP_ADDR_LEN], msk[IPV6_DISP_ADDR_LEN];

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));

  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpCB,&tmpEntry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:There is no cache entry in the table:%d",__FUNCTION__,__LINE__);
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }


  while (L7_NULLPTR != entry)
  {

    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpCB,entry, &entryNext))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:There is no cache entry in the table:%d",__FUNCTION__,__LINE__);
    }

    if (entry->count == 0)
    {
      entry = entryNext;
      continue;
    }

    if ((inetIsAddressZero(pruneGrp) == L7_TRUE) &&(inetIsAddressZero(pruneSrc) == L7_TRUE)
        &&(inetIsAddressZero(pruneSrcMask) == L7_TRUE))
    {
      inetCopy(pruneGrp,&entry->group);
      inetCopy(pruneSrc,&entry->source);

      dvmrpKeySet(&key, entry->source);
      /* find the longest match */
      pData = radixMatchNode(&dvmrpCB->dvmrpRouteTreeData, &key, skipentries);
      if (pData != L7_NULLPTR)
      {
        route = &pData->dvmrpRouteInfo;
        bitlen = route->networkAddr.maskLength;
      }
      else
      {
        bitlen = dvmrpDefaultNetmask(dvmrpCB->family);
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n[%s:%d] Failed to get the best match for source=%s.\n",
                     __FUNCTION__, __LINE__, inetAddrPrint(pruneSrc,src));
      }
      if (L7_SUCCESS != inetMaskLenToMask(dvmrpCB->family,bitlen, pruneSrcMask))
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Couldn't convert masklen to mask\n");
        osapiReadLockGive(dvmrpCB->dvmrpRwLock);
        return L7_FAILURE;
      }
      osapiReadLockGive(dvmrpCB->dvmrpRwLock);

      DVMRP_DEBUG (DVMRP_DEBUG_INFO, "\n[%s-%d]: Success ... Src-%s, Grp-%s, Msk-%s.\n", __FUNCTION__, __LINE__,
                   inetAddrPrint(pruneSrc,src), inetAddrPrint(pruneGrp,grp),
                   inetAddrPrint(pruneSrcMask,msk));
      return  L7_SUCCESS;
    }

    compGrp = L7_INET_ADDR_COMPARE(pruneGrp, &entry->group);
    compSrc = L7_INET_ADDR_COMPARE(pruneSrc, &entry->source);
    if (compGrp < L7_NULL || (compGrp == L7_NULL && compSrc < L7_NULL))
    {
      inetCopy(pruneGrp,&entry->group);      
      inetCopy(pruneSrc,&entry->source);

      dvmrpKeySet(&key, entry->source);
      /* find the longest match */
      pData = radixMatchNode(&dvmrpCB->dvmrpRouteTreeData, &key, skipentries);
      if (pData != L7_NULLPTR)
      {
        route = &pData->dvmrpRouteInfo;
        bitlen = route->networkAddr.maskLength;
      }
      else
      {
        bitlen = dvmrpDefaultNetmask(dvmrpCB->family);
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n[%s:%d] Failed to get the best match for source=%s.\n",
                     __FUNCTION__, __LINE__, inetAddrPrint(pruneSrc,src));
      }

      if (L7_SUCCESS != inetMaskLenToMask(dvmrpCB->family,bitlen, pruneSrcMask))
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Couldn't convert masklen to mask\n");
        osapiReadLockGive(dvmrpCB->dvmrpRwLock);
        return L7_FAILURE;
      }
      osapiReadLockGive(dvmrpCB->dvmrpRwLock);

      DVMRP_DEBUG (DVMRP_DEBUG_INFO, "\n[%s-%d]: Success ... Src-%s, Grp-%s, Msk-%s.\n", __FUNCTION__, __LINE__,
                   inetAddrPrint(pruneSrc,src), inetAddrPrint(pruneGrp,grp),
                   inetAddrPrint(pruneSrcMask,msk));
      return  L7_SUCCESS;

    }

    entry = entryNext;

  }


  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the Route Flags
 *
 * @param    ipMRouteGroup       -  @b{(input)}  Group Address
 * @param    ipMRouteSource      -  @b{(input)}  Source Address
 * @param    ipMRouteSourceMask  -  @b{(input)}  Source Mask
 * @param    flags               -  @b{(output)} Flags
 *
 * @returns  L7_SUCCESS       if success
 * @returns  L7_ERROR         if interface does not exist
 * @returns  L7_FAILURE       if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpIpMRouteFlagsGet(L7_inet_addr_t *ipMRouteGroup, 
                              L7_inet_addr_t *ipMRouteSource,
                              L7_inet_addr_t  *ipMRouteSourceMask, 
                              L7_uint32*  flags)
{
  dvmrp_cache_entry_t *entry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  entry = dvmrp_cache_lookup (dvmrpCB,ipMRouteSource, ipMRouteGroup);
  if (entry)
  {
    *flags = entry->flags;
    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get the Route source
 *
 * @param    ipMRtGrp            -  @b{(input)}  Group Address
 * @param    ipMRtSrc            -  @b{(input)}  Source Address
 * @param    ipMRtSrcMask        -  @b{(input)}  Source Mask
 * @param    rpfAddr             -  @b{(output)} RPF address of (S,G)
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpIpMRouteRtAddrGet(L7_inet_addr_t *ipMRtGrp, 
                               L7_inet_addr_t *ipMRtSrc, 
                               L7_inet_addr_t *ipMRtSrcMsk,
                               L7_inet_addr_t *rpfAddr)
{
  dvmrp_cache_entry_t *entry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  entry = dvmrp_cache_lookup (dvmrpCB, ipMRtSrc, ipMRtGrp);
  if (entry)
  {
    inetCopy(rpfAddr,&entry->data->networkAddr.addr);
    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get the Route source Mask
 *
 * @param    ipMRtGrp            -  @b{(input)}  Group Address
 * @param    ipMRtSrc            -  @b{(input)}  Source Address
 * @param    ipMRtSrcMask        -  @b{(input)}  Source Mask
 * @param    rtMask              -  @b{(output)} Mask pf route entry.
 *
 * @returns  L7_SUCCESS        if success
 * @returns  L7_ERROR          if interface does not exist
 * @returns  L7_FAILURE        if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpIpMRouteRtMaskGet(L7_inet_addr_t *ipMRtGrp, 
                               L7_inet_addr_t *ipMRtSrc,
                               L7_inet_addr_t *ipMRtSrcMsk, 
                               L7_inet_addr_t *rtMask)
{
  dvmrp_cache_entry_t *entry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  entry = dvmrp_cache_lookup (dvmrpCB, ipMRtSrc, ipMRtGrp);
  if (entry)
  {
    if (L7_SUCCESS != inetMaskLenToMask(dvmrpCB->family,entry->data->networkAddr.maskLength, rtMask))
    {
      DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Couldn't convert masklen to mask\n");
      osapiReadLockGive(dvmrpCB->dvmrpRwLock);
      return L7_FAILURE;
    }

    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get the Route source Type
 *
 * @param    ipMRtGrp            -  @b{(input)}  Group Address
 * @param    ipMRtSrc            -  @b{(input)}  Source Address
 * @param    ipMRtSrcMask        -  @b{(input)}  Source Mask
 * @param    rtType              -  @b{(output)} Route Type.
 *
 * @returns  L7_SUCCESS    if success
 * @returns  L7_ERROR,     if interface does not exist
 * @returns  L7_FAILURE    if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpIpMRouteRtTypeGet (L7_inet_addr_t *ipMRtGrp, 
                                L7_inet_addr_t *ipMRtSrc,
                                L7_inet_addr_t *ipMRtSrcMsk, 
                                L7_uint32 *rtType)
{
  dvmrp_cache_entry_t *entry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  entry = dvmrp_cache_lookup (dvmrpCB,ipMRtSrc, ipMRtGrp);
  if (entry)
  {
    *rtType = L7_MULTICAST_ROUTE;
    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}
/*********************************************************************
 * @purpose  Get the Route expiry time
 *
 * @param    ipMRtGrp            -  @b{(input)}  Group Address
 * @param    ipMRtSrc            -  @b{(input)}  Source Address
 * @param    ipMRtSrcMask        -  @b{(input)}  Source Mask
 * @param    expire              -  @b{(output)} expiry time.
 *
 * @returns  L7_SUCCESS    if success
 * @returns  L7_ERROR,     if interface does not exist
 * @returns  L7_FAILURE    if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpIpMRouteExpiryTimeGet (L7_inet_addr_t *ipMRtGrp, 
                                L7_inet_addr_t *ipMRtSrc,
                                L7_inet_addr_t *ipMRtSrcMsk, 
                                L7_uint32       *expire)
{
  dvmrp_cache_entry_t *entry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  entry = dvmrp_cache_lookup (dvmrpCB,ipMRtSrc, ipMRtGrp);
  if (entry)
  {
    if(entry->firstPrune == 0)
    {
      rc = mfcEntryExpiryTimeGet(ipMRtSrc, ipMRtGrp, expire);
    }
    else
    { 
      *expire = entry->expire -(osapiUpTimeRaw());
      rc = L7_SUCCESS;
   }
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get the Route rpf address
 *
 * @param    ipMRtGrp            -  @b{(input)}  Group Address
 * @param    ipMRtSrc            -  @b{(input)}  Source Address
 * @param    ipMRtSrcMask        -  @b{(input)}  Source Mask
 * @param    rpfAddr             -  @b{(output)} RPF address.
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpIpMRouteRpfAddrGet (L7_inet_addr_t *ipMRtGrp, 
                                 L7_inet_addr_t *ipMRtSrc,
                                 L7_inet_addr_t *ipMRtSrcMsk, 
                                 L7_inet_addr_t *rpfAddr)
{
  dvmrp_cache_entry_t *entry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  entry = dvmrp_cache_lookup (dvmrpCB,ipMRtSrc, ipMRtGrp);
  if (entry)
  {
    if (entry->data->neighbor != L7_NULLPTR)
    {
      inetCopy(rpfAddr,&entry->data->neighbor->nbrAddr.addr);
    }
    else
    {
      memset(rpfAddr, L7_NULL, sizeof(L7_inet_addr_t ));
    }
    rc = L7_SUCCESS;
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return rc;
}

/*********************************************************************
 * @purpose  Get the MRoute Entry
 *
 * @param    ipMRouteGroup      -  @b{(input)}  Group address
 * @param    ipMRouteSource     -  @b{(input)}  source IP Address
 * @param    ipMRouteSourceMask -  @b{(input)}  source IP mask
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpIpMRouteEntryGet(L7_inet_addr_t *ipMRouteGroup, 
                              L7_inet_addr_t *ipMRouteSource, 
                              L7_inet_addr_t  *ipMRouteSourceMask)
{
  dvmrp_cache_entry_t *entry = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         
  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  entry = dvmrp_cache_lookup (dvmrpCB, ipMRouteSource, ipMRouteGroup);
  if (entry != NULL)
  {
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);  
    return L7_SUCCESS;
  }

  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the Next MRoute Entry
 *
 * @param    ipMRtGrp      -  @b{(input)}      Group address
 * @param    ipMRtSrc      -  @b{(input)}      source IP Address
 * @param    ipMRtSrcMask  -  @b{(input)}      source IP mask
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpIpMRouteEntryNextGet(L7_inet_addr_t *ipMRtGrp, 
                                  L7_inet_addr_t *ipMRtSrc, 
                                  L7_inet_addr_t  *ipMRtSrcMask)
{
  dvmrp_cache_entry_t tmpEntry, *entry = L7_NULLPTR;
  L7_uchar8 bitlen;
  L7_int32 compGrp = L7_NULL;
  L7_int32 compSrc = L7_NULL;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }

  memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));

  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpCB,&tmpEntry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:There is no cache entry in the table:%d",__FUNCTION__,__LINE__);
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }


  while (L7_NULLPTR != entry)
  {
    if ((inetIsAddressZero(ipMRtGrp) == L7_TRUE) &&(inetIsAddressZero(ipMRtSrc) == L7_TRUE)
        &&(inetIsAddressZero(ipMRtSrcMask) == L7_TRUE))
    {
      inetCopy(ipMRtGrp,&entry->group);
      inetCopy(ipMRtSrc,&entry->source);
      bitlen = dvmrpDefaultNetmask(dvmrpCB->family);  
      if (L7_SUCCESS != inetMaskLenToMask(dvmrpCB->family,bitlen, ipMRtSrcMask))
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Couldn't convert masklen to mask\n");
        osapiReadLockGive(dvmrpCB->dvmrpRwLock);
        return L7_FAILURE;
      }
      osapiReadLockGive(dvmrpCB->dvmrpRwLock);
      return  L7_SUCCESS;
    }



    compGrp = L7_INET_ADDR_COMPARE(ipMRtGrp, &entry->group);
    compSrc = L7_INET_ADDR_COMPARE(ipMRtSrc, &entry->source);
    if (compGrp < L7_NULL || (compGrp == L7_NULL && compSrc < L7_NULL))
    {
      inetCopy(ipMRtGrp,&entry->group);
      inetCopy(ipMRtSrc,&entry->source);
      bitlen = dvmrpDefaultNetmask(dvmrpCB->family);

      if (L7_SUCCESS != inetMaskLenToMask(dvmrpCB->family,bitlen, ipMRtSrcMask))
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Couldn't convert masklen to mask\n");
        osapiReadLockGive(dvmrpCB->dvmrpRwLock);
        return L7_FAILURE;
      }
      osapiReadLockGive(dvmrpCB->dvmrpRwLock);
      return  L7_SUCCESS;

    }
    memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));        
    entry = L7_NULLPTR;
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpCB,&tmpEntry, &entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:There is no cache entry in the table:%d",__FUNCTION__,__LINE__);
      break;
    }
  }


  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the output i/f id of the given source, group pair
 *
 * @param    ipMRtGrp      -  @b{(input)}      Group address
 * @param    ipMRtSrc      -  @b{(input)}      source IP Address
 * @param    ipMRtSrcMask  -  @b{(input)}      source IP mask
 * @param    intIfNum      -  @b{(output)}     output i/f id
 * 
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpRouteOutIntfEntryNextGet(L7_inet_addr_t *ipMRtGrp, 
                                      L7_inet_addr_t *ipMRtSrc,
                                      L7_inet_addr_t *ipMRtSrcMask, 
                                      L7_uint32 *intIfNum)
{
  L7_uint32           rtrIfNum;
  L7_uint32           tmpIntIfNum;
  L7_uchar8    oif[L7_INTF_INDICES];
  dvmrp_cache_entry_t mfentry, *entry = L7_NULLPTR;
  L7_uint32 maxIntfNum;    

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if ((ipMRtGrp == L7_NULLPTR) && (ipMRtSrc == L7_NULLPTR) && 
      (ipMRtSrcMask == L7_NULLPTR))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : Input parameter pointers are NULL!!!!!!!!!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }
  inetCopy(&mfentry.source,ipMRtSrc);
  inetCopy(&mfentry.group,ipMRtGrp);
  if (L7_SUCCESS != dvmrpCacheTableCacheGet(dvmrpCB,&mfentry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:No cache entry in the table",__FUNCTION__);
    osapiReadLockGive(dvmrpCB->dvmrpRwLock);
    return L7_FAILURE;
  }

  if (entry != L7_NULLPTR)
  {
    dvmrpASBNode_t *asbNodeCurr = L7_NULLPTR;
    L7_uchar8 maskLen = 0;
    
    memcpy(&oif, &entry->children, L7_INTF_INDICES);
    MCAST_BITX_OR(oif, entry->routers,oif);
    MCAST_BITX_RESET(oif,entry->parent->index);

    /* Find whether the group in (S,G) entry falls in any existing 
       admin-scope range at the upstream interface */
    for (asbNodeCurr = (dvmrpASBNode_t *)SLLFirstGet(&(dvmrpCB->dvmrpasbList));
         asbNodeCurr != L7_NULLPTR ;
         asbNodeCurr = (dvmrpASBNode_t *)SLLNextGet(&(dvmrpCB->dvmrpasbList), (L7_sll_member_t*) asbNodeCurr))
    {
      inetMaskToMaskLen(&asbNodeCurr->grpMask,&maskLen);
      if (inetAddrCompareAddrWithMask(&entry->group,maskLen,
                                      &asbNodeCurr->grpAddr,maskLen) == L7_SUCCESS)
      {
        if (MCAST_BITX_TEST(asbNodeCurr->intfMask, entry->parent->index))
        {
          memset (&oif, 0, sizeof(oif));
          break;
        }
      }
    }

    nimGetHighestIntfNumber(&maxIntfNum);    

    for (tmpIntIfNum = *intIfNum+1; tmpIntIfNum <= maxIntfNum;
        tmpIntIfNum++)
    {
      if (mcastIpMapIntIfNumToRtrIntf(dvmrpCB->family,tmpIntIfNum, &rtrIfNum) == L7_SUCCESS)
      {
        if (MCAST_BITX_TEST(oif, rtrIfNum))
        {
          *intIfNum = tmpIntIfNum;
          osapiReadLockGive(dvmrpCB->dvmrpRwLock);
          return L7_SUCCESS;
        }
      }
    }
  }
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_FAILURE;
}
/*********************************************************************
 * @purpose  This function is used to get the number of multicast route entries in the 
 *                  cache table.
 *
 * @param     None
 * @returns   entryCount - no of multicast route entries
 * @notes    None
 *
 * @end
 *********************************************************************/

L7_RC_t dvmrpIpMRouteEntryCountGet (L7_uint32 *entryCount)
{
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:ENTERED  \n",__FUNCTION__);         

  if (dvmrpCB == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");
    return L7_FAILURE;
  }

  if (osapiReadLockTake(dvmrpCB->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nCould Not Access the semaphore.\n\n");
    return L7_FAILURE;
  }


  *entryCount = dvmrpCB->dvmrpCacheTableTree.count;
  osapiReadLockGive(dvmrpCB->dvmrpRwLock);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s:LEAVING  \n",__FUNCTION__);         
  return L7_SUCCESS;
}

