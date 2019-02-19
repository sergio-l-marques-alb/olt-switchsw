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
heapBuffPool_t pimdmV6HeapPoolList[] = {
  {0,0,"LAST POOL INDICATOR",0}
};

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

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
