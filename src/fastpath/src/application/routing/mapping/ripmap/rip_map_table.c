/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  rip_map_table.c
*
* @purpose   This file contains operations on the route table maintained in
*            mapping layer that contains best routes other than RIP routes.
*
* @component RIP Mapping Layer
*
* @comments  Access to elements in the list is protected by a semaphore.
*            Functions that return values from the list always return a copy
*            of the data in the list.
*
* @create    7/8/2003
*
* @author    skanchi
*
* @end
*
**********************************************************************/

#include "l7_ripinclude.h"
#include "avl_api.h"
#include "rip_map_table.h"

static L7_uint32 ripMapRoutesMax = 0;
static avlTreeTables_t   *ripMapRouteTableTreeHeap;   /* space for tree nodes */
static ripMapRouteData_t *ripMapRouteDataHeap;   /* space for data */
static avlTree_t         ripMapRouteTableTree;

/*********************************************************************
* @purpose  Initializes the ripMap route table
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableInit()
{
  L7_uint32 avlType = 0x10;

  ripMapRoutesMax = platRtrRouteMaxEntriesGet();

  ripMapRouteTableTreeHeap = osapiMalloc(L7_RIP_MAP_COMPONENT_ID,
                                         ripMapRoutesMax * sizeof(avlTreeTables_t));
  if (ripMapRouteTableTreeHeap == NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Error: unable to allocate RIP Route Table tree heap.\n");
    return L7_FAILURE;
  }

  ripMapRouteDataHeap = osapiMalloc(L7_RIP_MAP_COMPONENT_ID,
                                    ripMapRoutesMax * sizeof(ripMapRouteData_t));
  if (ripMapRouteDataHeap == NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Error: unable to allocate RIP Route Table data heap.\n");
    return L7_FAILURE;
  }

  /* Cleanup data structures before proceeding */
  bzero ((char *)ripMapRouteTableTreeHeap, ripMapRoutesMax * sizeof (avlTreeTables_t));
  bzero ((char *)ripMapRouteDataHeap, ripMapRoutesMax * sizeof (ripMapRouteData_t));

  /* The destination networks are stored in an AVL tree.
  */
  avlCreateAvlTree(&ripMapRouteTableTree, ripMapRouteTableTreeHeap,
                   ripMapRouteDataHeap, ripMapRoutesMax,
                   (L7_uint32)(sizeof(ripMapRouteData_t)), avlType, sizeof(L7_uint32) * 2);
  (void)avlSetAvlTreeComparator(&ripMapRouteTableTree, avlCompareIPNetAddr);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Acquires the semaphore of the ripMap route table
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableResourceAcquire()
{
  if (osapiSemaTake(ripMapRouteTableTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Releases the semaphore of the ripMap rouet table
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableResourceRelease()
{
  if (osapiSemaGive(ripMapRouteTableTree.semId) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Add a route to the ripMap route table
*
* @param    *routeData         @b{(input)} Route information to be added
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableRouteAdd(ripMapRouteData_t *routeData)
{
  ripMapRouteData_t *pData = L7_NULLPTR;

  if (avlTreeCount(&ripMapRouteTableTree) >= ripMapRoutesMax)
    L7_LOG_ERROR(0);

  pData = avlInsertEntry(&ripMapRouteTableTree, routeData);
  if (pData != L7_NULLPTR)
    return L7_FAILURE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Delete a route from the ripMap route table
*
* @param    network         @b{(input)} Newtork ip address of the route
* @param    subnetMask      @b{(input)} Subnet mask of the route
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableRouteDelete(L7_IP_ADDR_t network, L7_IP_MASK_t subnetMask)
{
  ripMapRouteData_t dummyRoute;

  dummyRoute.network = network;
  dummyRoute.subnetMask = subnetMask;

  if (avlDeleteEntry(&ripMapRouteTableTree, &dummyRoute) == L7_NULLPTR) {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Modify a route in the ripMap route table
*
* @param    *routeData         @b{(input)} Route information to be modified
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableRouteModify(ripMapRouteData_t *routeData)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check whether the route exists in the ripMap route table
*
* @param    *srcRtData      @b{(input)} Route info to look for
* @param    **routeData     @b{(output)} Pointer to the route info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableRouteGet(ripMapRouteData_t *srcRtData, ripMapRouteData_t **routeData)
{
  /* Searches for the network and gateway and returns pointer to the structure
     stored in the tree.
  */
  ripMapRouteData_t  *pData;

  if (srcRtData == L7_NULLPTR)
    return L7_FAILURE;

  pData = L7_NULLPTR;
  pData = avlSearchLVL7(&ripMapRouteTableTree, &srcRtData->network, AVL_EXACT);
  if (pData == L7_NULLPTR)
    return L7_FAILURE;

  if (routeData != L7_NULLPTR)
    *routeData = pData;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the next route after the specified route in the ripMap route table
*
* @param    *srcRtData      @b{(input)} Route info to look for
* @param    **routeData     @b{(output)} Pointer to the next route info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableRouteGetNext(ripMapRouteData_t *srcRtData, ripMapRouteData_t **routeData)
{
  ripMapRouteData_t  *pData = L7_NULLPTR;

  if (srcRtData == L7_NULLPTR)
    return L7_FAILURE;


  if ((srcRtData->network == 0x00000000) && (srcRtData->subnetMask == 0x00000000) &&
      (srcRtData->gateway == 0x00000000))
  {
    /* They are asking for the first route. */
    pData = avlSearchLVL7(&ripMapRouteTableTree, &srcRtData->network, AVL_EXACT);
  }

  if (pData == L7_NULLPTR)
    pData = avlSearchLVL7(&ripMapRouteTableTree, &srcRtData->network, AVL_NEXT);

  if (pData == L7_NULLPTR)
    return L7_FAILURE;

  if (routeData != L7_NULLPTR)
    *routeData = pData;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Search the ripMap route table
*
* @param    network         @b{(input)} Network ip address of the route
* @param    subnetMask      @b{(input)} Subnet mask of the route
* @param    **routeData     @b{(output)} Pointer to the route info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableRouteSearch(L7_IP_ADDR_t network, L7_IP_MASK_t subnetMask, ripMapRouteData_t **routeData)
{
  /* Searches for the network and gateway and returns pointer to the structure
     stored in the tree.
  */
  ripMapRouteData_t  dummyRoute, *pData;

  /* Find the first route.*/
  dummyRoute.network = network;
  dummyRoute.subnetMask = subnetMask;

  pData = avlSearchLVL7(&ripMapRouteTableTree, &dummyRoute, AVL_EXACT);
  if (pData == L7_NULLPTR)
    return L7_FAILURE;

  *routeData = pData;
  return L7_SUCCESS;
}



/*********************************************************************
*
*  DEBUG Routines
*
*********************************************************************/
static char *ripMapRouteTableRedistProtoStr[] = { "----", "local", "static", "rip", "ospf", "bgp", "----" };

L7_RC_t ripMapRouteTableShow()
{
  ripMapRouteData_t routeData, *rtData;
  L7_BOOL printMsg = L7_FALSE;
  L7_uchar8 inetBuf[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 maskBuf[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 gateBuf[OSAPI_INET_NTOA_BUF_SIZE];

  /* Acquire the ripmap internal redist route table resource and get the routes */
  if (ripMapRouteTableResourceAcquire() != L7_SUCCESS)
    L7_LOG_ERROR(1);

  memset(&routeData, 0, sizeof(ripMapRouteData_t));
  rtData = &routeData;
  while (ripMapRouteTableRouteGetNext(rtData, &rtData) == L7_SUCCESS)
  {
    if (printMsg == L7_FALSE)
    {
      printf("    Network           Mask            Gateway      protocol   protoSpecific  routeAdded  \n");
      printf("---------------  ---------------  ---------------  ---------  -------------  ----------  \n");
      printMsg = L7_TRUE;
    }
    osapiInetNtoa(rtData->network, inetBuf);
    osapiInetNtoa(rtData->subnetMask, maskBuf);
    osapiInetNtoa(rtData->gateway, gateBuf);
    printf("%-15.15s  %-15.15s  %-15.15s  %2d %-6.6s       %2d            %-1s       \n",
           inetBuf, maskBuf, gateBuf,
           rtData->protocol, ripMapRouteTableRedistProtoStr[rtData->protocol],
           rtData->protoSpecific,
           (rtData->routeAdded == L7_TRUE) ? "Y" : "N");
  }
  if (printMsg == L7_FALSE)
    printf("There are no routes in the route table.\n");

  /* Release the ripmap internal redist route table resource. */
  if (ripMapRouteTableResourceRelease() != L7_SUCCESS)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Could not release the RIP route table semaphore.\n");

  return L7_SUCCESS;
}




