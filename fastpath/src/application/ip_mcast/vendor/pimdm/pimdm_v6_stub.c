/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   pimdm_v6_stub.c
*
* @purpose    PIM-DM stub functions for IPv6 
*
* @component  PIM-DM
*
* @comments   none
*
* @create     27/10/2006
*
* @author     Ramu
* @end
*
**********************************************************************/

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "heap_api.h"
#include "pimdm_main.h"

/*******************************************************************************
**                    Externs & Global declarations                           **
*******************************************************************************/

/* Number of elements in IPv6 heap pool list, including dummy end-of-list element */
#define PIMDM_IPV6_POOL_LIST_SIZE 1
heapBuffPool_t pimdmV6HeapPoolList[PIMDM_IPV6_POOL_LIST_SIZE];

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  Build the PIM-DM IPv6 buffer pool list
*
* @param    void
*
* @returns  pointer to first array element
*
* @comments Build list at run time since buffer pool sizes depend on 
*           SDM template scaling parameters.
*
*           When a pool is added here, increment PIMDM_IPV6_POOL_LIST_SIZE.
*
* @end
*********************************************************************/
heapBuffPool_t *pimdmV6HeapPoolListGet(void)
{
  L7_uint32 i = 0;

  memset(pimdmV6HeapPoolList, 0, sizeof(heapBuffPool_t) * PIMDM_IPV6_POOL_LIST_SIZE);

  /* End indicator */
  pimdmV6HeapPoolList[i].buffSize = 0;
  pimdmV6HeapPoolList[i].buffCount = 0;
  osapiStrncpySafe(pimdmV6HeapPoolList[i].name, "LAST POOL INDICATOR", HEAP_POOLNAME_SIZE);
  pimdmV6HeapPoolList[i].flags = 0;
  i++;

  return pimdmV6HeapPoolList;
}

/*********************************************************************
*
* @purpose  Set Ipv6 Socket Options
*
* @param    pimdmCB @b{ (input) } pointer to Control Block
* @param    sockFd  @b{ (input) } socket fd
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimdmV6SocketOptionsSet (pimdmCB_t *pimdmCB,L7_int32 sockFd)
{
  return L7_FAILURE;
}

/******************************************************************************
* @purpose RTO best-route change event handler
*
* @param  pimdmCb     @b{ (input) } Pointer PIMDM Control Block.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @comments 
*
* @end
******************************************************************************/
L7_RC_t pimdmV6BestRoutesGet(pimdmCB_t *pimdmCB)
{
  return L7_FAILURE;
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
L7_RC_t pimdmV6RouteChangeBufMemAlloc(pimdmCB_t *pimdmCB)
{
  return L7_FAILURE;
}
