
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   pimdm_util.c
*
* @purpose    To maintain the PIM-DM V4 specific functionality
*
* @component  PIM-DM
*
* @comments   none
*
* @create     
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/
/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l7_socket.h"
#include "rto_api.h"
#include "buff_api.h"
#include "l3_mcast_commdefs.h"
#include "l7_mcast_api.h"
#include "pimdm_debug.h"
#include "l7_ip_api.h"
#include "pimdm_mrt.h"
#include "pimdm_util.h"
#include "pimdm_intf.h"
#include "pimdm_map.h"
#include "pimdm_mgmd.h"
#include "pimdm_admin_scope.h"
#include "pim_defs.h"
/*******************************************************************************
**                    Externs & Global declarations                           **
*******************************************************************************/
extern long random (void);
extern void srandom(unsigned int seed);

heapBuffPool_t pimdmV4HeapPoolList[] = {

  /* PIMDM Protocol Control Block Buffer */
  {sizeof(pimdmCB_t),1,"PIMDM CONTROL BLOCK",HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Join/Prune Message Buffers */
  {PIMDM_MAX_JP_MSG_SIZE,PIMDM_MAX_JP_PACKETS,"PIMDM_JPG_MSG_BUFFER", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Join/Prune Address List Buffers for IPv4*/
  {PIM_IPV4_MAX_JOIN_OR_PRUNE_ADDRS_IN_LIST * sizeof(pim_encod_src_addr_t), PIMDM_MAX_PROTOCOL_PACKETS,"PIMDM_JP_ADDR_LIST_BUFFER", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* AVL Tree Heap Buffers for MRT (S,G) Database */
  {sizeof(avlTreeTables_t) * PIMDM_MRT_MAX_IPV4_ROUTE_ENTRIES,1,"PIMDM MRT TREE HEAP",HEAP_BUFFPOOL_NO_OVERFLOW},

  /* AVL Data Heap Buffers for MRT (S,G) Database */
  {sizeof(pimdmMrtEntry_t) * PIMDM_MRT_MAX_IPV4_ROUTE_ENTRIES,1,"PIMDM MRT DATA HEAP",HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Downstream Interface Buffers */
  {sizeof(pimdmDownstrmIntfInfo_t),(PIMDM_NUM_OPTIMAL_OUTGOING_INTERFACES * PIMDM_MRT_MAX_IPV4_ROUTE_ENTRIES),"PIMDM MRT OPT OIFs",HEAP_BUFFPOOL_NO_OVERFLOW},

  /* AVL Tree Heap Buffers for MGMD Database */
  {sizeof(avlTreeTables_t) * PIMDM_MAX_LOCAL_MEMB_ENTRIES,1,"PIMDM MGMD TREE HEAP",HEAP_BUFFPOOL_NO_OVERFLOW},

  /* AVL Data Heap Buffers for MGMD Database */
  {sizeof(pimdmLocalMemb_t) * PIMDM_MAX_LOCAL_MEMB_ENTRIES,1,"PIMDM MGMD DATA HEAP",HEAP_BUFFPOOL_NO_OVERFLOW},

  /* PIMDM Interface Control Block Buffer */
  {sizeof(pimdmIntfInfo_t),1,"PIMDM INTERFACES",HEAP_BUFFPOOL_NO_OVERFLOW},

  /* RTO-IPv4 Route Change Buffers */
  {sizeof(rtoRouteChange_t) * PIMDM_MAX_BEST_ROUTE_CHANGES,1,"PIMDM RTO CHANGES", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Admin Scope Boundary Database Buffers */
  {sizeof(pimdmASBNode_t),PIMDM_MAX_ADMIN_SCOPE_ENTRIES,"PIMDM ADMIN SCOPE DATABASE",HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Last item in the Heap Pool ... DO NOT REMOVE/MODIFY IT */
  {0,0,"LAST POOL INDICATOR",0}
};

/*******************************************************************************
**                      Defines & local Typedefs                              **
*******************************************************************************/

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  Set Ipv4 Socket Options
*
* @param    sockFD  @b{ (input) } socket fd
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t pimdmV4SocketOptionsSet (L7_int32 sockFd)
{
  L7_uchar8 sockOptMLoop = 0;
  
  /* Set the Multicast Loop Option */
  if (osapiSetsockopt (sockFd, IPPROTO_IP, L7_IP_MULTICAST_LOOP,
                       (L7_uchar8*) &sockOptMLoop, sizeof (sockOptMLoop))
                        != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Mcast Loop v4 Sock Opt Set Failure");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Wrapper to set socket options
*
* @param    pimdmCB @b{ (input) } Pointer to Control Block
* @param    sockFD  @b{ (input) } socket fd
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t pimdmSocketOptionsSet (pimdmCB_t *pimdmCB,L7_int32 sockFd)
{
  
  if(pimdmCB->addrFamily == L7_AF_INET)
  {
    if(pimdmV4SocketOptionsSet(sockFd) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else if(pimdmCB->addrFamily == L7_AF_INET6)
  {
    if(pimdmV6SocketOptionsSet(pimdmCB,sockFd) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Initialize the PIM-DM Tx Socket
*
* @param    pimdmCB  @b{ (input) } Pointer to the PIM-DM Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimdmSocketInit (pimdmCB_t *pimdmCB)
{
  L7_int32 sockFD = -1;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  pimdmCB->sockFD = -1;

  if ((osapiSocketCreate (pimdmCB->addrFamily, L7_SOCK_RAW, 
                          PIM_IP_PROTO_NUM, &sockFD))
                          != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Socket Creation Failure for Address family %d",
              pimdmCB->addrFamily);
    return L7_FAILURE;
  }

  if(pimdmSocketOptionsSet(pimdmCB,sockFD) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Socket Option set failed for address family %d",
             pimdmCB->addrFamily);
    osapiSocketClose(sockFD);
    return L7_FAILURE;
  }
  /* Assign the Socket FD */
  pimdmCB->sockFD = sockFD;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To De-initialize the PIM-DM Tx Socket
*
* @param    sockFD   @b{ (input) } Pointer to the Socket FD
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmSocketDeInit (pimdmCB_t *pimdmCB)
{
  L7_int32 tempSockFD = pimdmCB->sockFD;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* De-Register the SockFD with PktRcvr for IPv6 Packet Reception */
  if (pimdmCB->addrFamily == L7_AF_INET6)
  {
    if(mcastMapPktRcvrSocketFdDeRegister (L7_IPV6_PKT_RCVR_ID_PIMDM) != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Socket FD De-Regis'tion Failure");
      return L7_FAILURE;
    }
  }

  /* Close the Socket */
  osapiSocketClose (tempSockFD);
  tempSockFD = -1;

  pimdmCB->sockFD = tempSockFD;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Function to get the best route changes
*           
*
* @param    pimdmCb          @b{(input)} pimdm control block
* @param    routeEntry       @b{(input)} Route information 
* @param    route_status     @b{(input)} Route Status
      Information regarding the reason the callback was
*        issued.  Route_status events are enumerated in 
*        RTO_ROUTE_EVENT_t.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @comments   
*
* @end
*********************************************************************/
static L7_RC_t pimdmV4BestRouteChanges(pimdmCB_t *pimdmCB,
                                       L7_routeEntry_t *routeEntry, 
                                       L7_uint32 route_status)
{
  L7_uint32      intIfNum =0;
  mcastRPFInfo_t routeInfo;
  L7_uchar8      maskLen=0;
  L7_inet_addr_t subnetMask;

  PIMDM_TRACE(PIMDM_DEBUG_API,"Entry");
  
  if(routeEntry == L7_NULLPTR || pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid input parameters");
    return L7_FAILURE;
  }
  if (route_status == RTO_ADD_ROUTE  ||
      route_status == RTO_DELETE_ROUTE)
  {  
    if ((routeEntry->flags & L7_RTF_REJECT) != L7_NULL)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Recvd Reject Route - Ignore");
      return L7_SUCCESS;
    }
  } 
  else if (route_status == RTO_CHANGE_ROUTE )
  {
    if ((routeEntry->flags & L7_RTF_REJECT) != L7_NULL)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Recvd Reject Route - Delete");
      route_status = RTO_DELETE_ROUTE;
    }
  }
  memset(&routeInfo,0,sizeof(mcastRPFInfo_t));
  inetAddressZeroSet(pimdmCB->addrFamily,&(routeInfo.rpfNextHop));
  inetAddressZeroSet(pimdmCB->addrFamily,&(routeInfo.rpfRouteAddress));

  /* select only one path, let it be the first one index by zero*/
  inetAddressSet(pimdmCB->addrFamily, 
                 &(routeEntry->ecmpRoutes.equalCostPath[0].arpEntry.ipAddr),
                 &routeInfo.rpfNextHop);

  /* RTO doesn't provide the intIfNum for the Deleted Routes.
   */
  if (route_status != RTO_DELETE_ROUTE)
  {
    intIfNum = routeEntry->ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
    if(ipMapIntIfNumToRtrIntf(intIfNum, &routeInfo.rpfIfIndex) != L7_SUCCESS)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Failed to convert intfIfNum to rtrIfNum "
                  "for intIfNum - %d", intIfNum);
      return L7_FAILURE;
    }
  }

  routeInfo.rpfRouteMetricPref = routeEntry->pref;
  routeInfo.rpfRouteMetric = routeEntry->metric;
  routeInfo.rpfRouteProtocol =routeEntry->protocol;

  routeInfo.status = route_status;
  inetAddressSet(pimdmCB->addrFamily,
                 (void *)&routeEntry->subnetMask,&subnetMask);
  inetMaskToMaskLen(&subnetMask,&maskLen);
  routeInfo.prefixLength = (L7_uint32)maskLen; 
  inetAddressSet(pimdmCB->addrFamily, &routeEntry->ipAddr, 
                 &(routeInfo.rpfRouteAddress));

  if (pimdmMrtRTOBestRouteChangeProcess (pimdmCB,&routeInfo) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Failed to process Best Route Changes");
    return L7_FAILURE;
  }

  PIMDM_TRACE(PIMDM_DEBUG_API,"Exit");
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose RTO best-route change event handler
*
* @param    pimdmCb   @b{ (input) } Pointer PIMDM Control Block.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @comments 
*
* @end
******************************************************************************/
L7_RC_t pimdmV4BestRoutesGet(pimdmCB_t *pimdmCB)
{
  L7_uint32          idx=0,numChanges =0;
  rtoRouteChange_t   *routeChangeBuf;
  L7_BOOL            moreChanges=L7_FALSE;
  
  PIMDM_TRACE(PIMDM_DEBUG_API,"Entry");

  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"PIMDM Control Block is NULL");
    return L7_FAILURE;
  }

  pimdmCB->rtoTimer = L7_NULLPTR;  
routeChangeBuf = (rtoRouteChange_t*)pimdmCB->routeChangeBuffer;
  if(rtoRouteChangeRequest(pimdmCB->rtoCallback,
                            PIMDM_MAX_BEST_ROUTE_CHANGES,
                            &numChanges,&moreChanges,
                            routeChangeBuf) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Failed to process Route Change Request");
    return L7_FAILURE;
  }

  for(idx=0;idx < numChanges;idx++)
  {
    pimdmV4BestRouteChanges(pimdmCB,
                            &routeChangeBuf->routeEntry,
                            routeChangeBuf->changeType);
    routeChangeBuf++;
  }

  if(moreChanges == L7_TRUE)
  {
    pimdmCB->rtoCallback();
  }
  PIMDM_TRACE(PIMDM_DEBUG_API,"Exit");
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose Allocate memory for the RTO buffer 
*
* @param  pimdmCB     @b{ (input) } Pointer PIMDM Control Block.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @comments 
*
* @end
******************************************************************************/
L7_RC_t pimdmV4RouteChangeBufMemAlloc(pimdmCB_t *pimdmCB)
{
  rtoRouteChange_t  *routeChangeBuf = L7_NULLPTR;

  PIMDM_TRACE(PIMDM_DEBUG_API,"Entry");
  if(L7_NULLPTR == pimdmCB)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"PIMDM Control Block is NULL");
    return L7_FAILURE;
  }

  if ((routeChangeBuf = PIMDM_ALLOC (pimdmCB->addrFamily,
                   (sizeof (rtoRouteChange_t) * PIMDM_MAX_BEST_ROUTE_CHANGES)))
                                  == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIMDM v4 RTO Route Change Info Heap Allocation Failed");
    return L7_FAILURE;
  }

  pimdmCB->routeChangeBuffer =(void *)routeChangeBuf;
  
  PIMDM_TRACE(PIMDM_DEBUG_API,"Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Function to Start the Timer (Both Set and Reset Timer)
*
* @param   pimdmCb   @b{(input)} Control Block.
*          pFunc     @b{(input)} Function Pointer.
*          pParam    @b{(input)} Function Parameter
*          timeout   @b{(input)} Timeout in seconds
*          errMessage@b{(input)}  Error Message
*          successMessage@b{(input)}  Error Message
*          tmrHandle     @b{(output)} Timer Handle
*           
*                            
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t
pimdmUtilAppTimerSet (pimdmCB_t *pimdmCB,
                      L7_app_tmr_fn pFunc,
                      void *pParam,
                      L7_uint32 timeOut,
                      L7_uchar8 *errMessage,
                      L7_uchar8 *successMessage,
                      L7_APP_TMR_HNDL_t *tmrHandle,
                      L7_uchar8 *timerName)

{
  if(*tmrHandle != NULL)
  {
    if (appTimerUpdate (pimdmCB->appTimer,tmrHandle,pFunc,pParam,timeOut,timerName)
                     != L7_SUCCESS)
    {
      *tmrHandle = 0;
      if (errMessage != L7_NULLPTR)
      {
        PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "%s", errMessage);
      }
      L7_LOGF(L7_LOG_SEVERITY_NOTICE,L7_FLEX_PIMDM_MAP_COMPONENT_ID, "PIMDM : appTimerUpdate() failed timeout = %d."
											" PIMDM FSM/General Timer's timeout value updation Failed.", timeOut);
      return L7_FAILURE;
    }
  }
  else
  {
    if ((*tmrHandle = appTimerAdd (pimdmCB->appTimer, pFunc, (void*) pParam,
                                   timeOut, timerName))
                                == L7_NULLPTR)
    {
      if (errMessage != L7_NULLPTR)
      {
        PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "%s for %d Sec.", errMessage, timeOut);
      }
      L7_LOGF(L7_LOG_SEVERITY_WARNING,L7_FLEX_PIMDM_MAP_COMPONENT_ID, "PIMDM : appTimerAdd() failed timeout = "
											  "%d. PIMDM FSM/General Timer's Start Failed.", timeOut);
      return L7_FAILURE;
    }
  }

  if (successMessage != L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_TIMER, "%s for %d Sec.", successMessage, timeOut);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Function to Cancel the Timer
*
* @param   pimdmCb   @b{(input)} Control Block.
*          handle    @b{(output)} Timer Handle
*           
*                            
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmUtilAppTimerCancel (pimdmCB_t *pimdmCB,
                              L7_APP_TMR_HNDL_t *tmrHandle)

{
  L7_APP_TMR_HNDL_t timerHandle = *tmrHandle;
  
  if(*tmrHandle != L7_NULLPTR)
  {
    if (appTimerDelete (pimdmCB->appTimer, timerHandle) == L7_SUCCESS)
    {
        /* Nothing to do*/
    }
    else
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Failed to Delete the Timer");
    }
    *tmrHandle = L7_NULLPTR;    
  }

  return;
}

/*********************************************************************
*
* @purpose Generate a random number between a given set of min and max values
*
* @param   minVal   @b{(input)} Minimum output value
* @param   maxVal   @b{(input)} Maximum output value
*                            
* @returns nnnn     Random value between the given min and max values (both inclusive)
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 pimdmUtilRandomGet(L7_uint32 minVal,
                             L7_uint32 maxVal)
{
  static L7_uint32 seed = 0;
  L7_uint32        val;

  /* Initialize the seed for the random generator if needed */
  if(seed == 0)
  {
    L7_uint32 maxIntfNum;
    L7_uchar8 mac[L7_MAC_ADDR_LEN];

    if(nimGetHighestIntfNumber(&maxIntfNum) == L7_SUCCESS)
    {
      if(nimGetIntfAddress(maxIntfNum, L7_SYSMAC_BIA, mac) == L7_SUCCESS)
      {
        seed = mac[L7_MAC_ADDR_LEN - 1];
        seed |= mac[L7_MAC_ADDR_LEN - 2] << 8;
        seed |= mac[L7_MAC_ADDR_LEN - 3] << 16;
        seed |= mac[L7_MAC_ADDR_LEN - 4] << 24;
        seed += osapiTimeMillisecondsGet();

        PIMDM_TRACE(PIMDM_DEBUG_API,
                   "Initializing PIM-DM random generator with a seed of %d", 
                    seed);
#if defined(_L7_OS_VXWORKS_) && VX_VERSION==65
        srand(seed);
#else
        srandom(seed);
#endif
      }
    }
  }

  /* Basic sanity check */
  if(minVal >= maxVal)
  {
    return minVal;
  }

  /* Generate/Compute a random number */
#if defined(_L7_OS_VXWORKS_) && VX_VERSION==65
  val = (L7_uint32)rand();
#else
  val = (L7_uint32)random();
#endif
  if((val >= minVal) && (val <= maxVal))
  {
    return val;
  }
  val = (val % (maxVal - minVal + 1)) + minVal;
  return val;
}

/*********************************************************************
*
* @purpose Get the TTL Threshold for a Router Interface Index
*
* @param   pimdmCB      @b{(input)} Pointer to the PIM-DM Control Block
* @param   rtrIfNum     @b{(input)} Index of the Router Interface
* @param   ttlThreshold @b{(output)} Pointer to the TTL Threshold
*                            
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmUtilIntfTTLThresholdGet (pimdmCB_t *pimdmCB,
                              L7_uint32 rtrIfNum,
                              L7_uint32 *ttlThreshold)
{
#if PIMDM_TO_REMOVE
  L7_uint32 intIfNum = 0;

  if (pimdmIntfIntIfNumGet (pimdmCB, rtrIfNum, &intIfNum) == L7_SUCCESS)
  {
    return (mcastMapIntfTtlThresholdGet (intIfNum, ttlThreshold));
  }
#endif

  if(pimdmIntfStateRefreshTTLThresholdGet(pimdmCB,rtrIfNum,ttlThreshold) 
                                      != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Initialise the RTO Buffer and the RTO CallBack
*
* @param   pimdmCB    @b{(input)} Pointer to the PIM-DM Control Block
*                            
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmRTORouteChangeBufferAllocate (pimdmCB_t *pimdmCB)
{ 
  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"control block is NULL");
    return L7_FAILURE;
  }

  if (pimdmCB->addrFamily == L7_AF_INET)
  {
    if (pimdmV4RouteChangeBufMemAlloc (pimdmCB) != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Route Change v4 Buf Alloc Failure");
      return L7_FAILURE;
    }
  }
  else if (pimdmCB->addrFamily == L7_AF_INET6)
  {
    if (pimdmV6RouteChangeBufMemAlloc (pimdmCB) != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Route Change v6 Buf Alloc Failure");
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Initialize the PIM-DM Heap Memory
*
* @param    addrFamily @b{ (input) } Address Family Identifier
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments Enable the HEAP_LOCK_GUARD so that the Heap Buffer Pools
*           are protected when multiple tasks try to access the same 
*           heap buffer pools.
*           This has been enabled to resolve the race condition in the
*           IPv6 control packet reception path, where-in PktRcvr Task
*           tries to allocate and PIM-DM Task tries to free the buffers.
*
* @end
*********************************************************************/
L7_RC_t
pimdmUtilMemoryInit (L7_uint32 addrFamily)
{
  heapBuffPool_t *pimdmHeapPoolList = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (addrFamily == L7_AF_INET)
  {
    pimdmHeapPoolList = pimdmV4HeapPoolList;
  }
  else if (addrFamily == L7_AF_INET6)
  {
    extern heapBuffPool_t pimdmV6HeapPoolList[];

    pimdmHeapPoolList = pimdmV6HeapPoolList;
  }
  else
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Bad Address Family - %d", addrFamily);
    return L7_FAILURE;
  }

  if (heapInit (mcastMapHeapIdGet(addrFamily), pimdmHeapPoolList,
                HEAP_LOCK_GUARD)
             != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_INIT, "PIM-DM Heap Initialization Failed");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

