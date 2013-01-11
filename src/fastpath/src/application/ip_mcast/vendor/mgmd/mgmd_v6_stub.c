/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  mgmd_v6_stub.c
*
* @purpose   To support MLD (v1 + v2) stub routines for v4-only builds
*
* @component MGMD
*
* @comments  none
*
* @create    18/10/2007
*
* @author    ddevi
* @end
*
**********************************************************************/

/**********************************************************************
                  Includes
***********************************************************************/
#include "mgmd.h"


/* Number of elements in MGMD IPV6 heap pool list, including dummy end-of-list element */
#define MGMD_IPV6_POOL_LIST_SIZE  1
heapBuffPool_t mgmdV6HeapPoolList[MGMD_IPV6_POOL_LIST_SIZE]; 

/* Number of elements in MGMD proxy IPv6 heap pool list, including dummy end-of-list element */
#define MGMD_PROXY_IPV6_POOL_LIST_SIZE  1
heapBuffPool_t mgmdProxyV6HeapPoolList[MGMD_PROXY_IPV6_POOL_LIST_SIZE];


/*********************************************************************
*
* @purpose  Build the MGMD IPv6 buffer pool list
*
* @param    void
*
* @returns  pointer to first array element
*
* @comments Build list at run time since buffer pool sizes depend on 
*           SDM template scaling parameters.
*
*           When a pool is added here, increment MGMD_IPV6_POOL_LIST_SIZE.
*
* @end
*********************************************************************/
heapBuffPool_t *mgmdV6HeapPoolListGet(void)
{
  L7_uint32 i = 0;

  memset(mgmdV6HeapPoolList, 0, sizeof(heapBuffPool_t) * MGMD_IPV6_POOL_LIST_SIZE);

  /* Last item in the Heap Pool ... DO NOT REMOVE/MODIFY IT */
  mgmdV6HeapPoolList[i].buffSize = 0;
  mgmdV6HeapPoolList[i].buffCount = 0;
  osapiStrncpySafe(mgmdV6HeapPoolList[i].name, "LAST POOL INDICATOR", HEAP_POOLNAME_SIZE);
  mgmdV6HeapPoolList[i].flags = 0;
  i++;

  return mgmdV6HeapPoolList;
}

/*********************************************************************
*
* @purpose  Build the MGMD Proxy IPv6 buffer pool list
*
* @param    void
*
* @returns  pointer to first array element
*
* @comments Build list at run time since buffer pool sizes depend on 
*           SDM template scaling parameters.
*
*           When a pool is added here, increment MGMD_PROXY_IPV6_POOL_LIST_SIZE.
*
* @end
*********************************************************************/
heapBuffPool_t *mgmdProxyV6HeapPoolListGet(void)
{
  L7_uint32 i = 0;

  memset(mgmdProxyV6HeapPoolList, 0, sizeof(heapBuffPool_t) * MGMD_PROXY_IPV6_POOL_LIST_SIZE);

  /* Last item in the Heap Pool ... DO NOT REMOVE/MODIFY IT */
  mgmdProxyV6HeapPoolList[i].buffSize = 0;
  mgmdProxyV6HeapPoolList[i].buffCount = 0;
  osapiStrncpySafe(mgmdProxyV6HeapPoolList[i].name, "LAST POOL INDICATOR", HEAP_POOLNAME_SIZE);
  mgmdProxyV6HeapPoolList[i].flags = 0;
  i++;

  return mgmdProxyV6HeapPoolList;
}

/*********************************************************************
*
* @purpose  Initialize the socket FD based on the protocol
*
* @param    mgmdCB    @b{ (input) } MGMD Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmd_init6_sockFd (mgmd_cb_t *mgmdCB)
{
  MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD : Initializing v6 socket on V4-only builds ");
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  De inialize the MLD socket Fd ( ICMPV6)
*
* @param    mgmdCB    @b{ (input) } MGMD Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmd_deinit6_sockFD (mgmd_cb_t *mgmdCB)
{
  MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD : Deinitializing v6 socket on V4-only builds ");
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Enables/Disables the MLD-V2 IPv6 Group address and interface ID with 
*           interpeak stack.
*  
* @param    rtrIfNum   @b{(input)}interface number
* @param    addFlag    @b{(input)}Indicates whether (group, Ifidx) needs to enable or 
*                      disable.
* @param    sockFd     @b{(input)}Socket Desc.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  on failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t mgmdV6SockChangeMembership(L7_uint32 rtrIfNum, L7_uint32 intfIfNum,
                              L7_uint32 addFlag, L7_uint32 sockfd)
{
  MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD : Setting Socket option to receive MLDv2 control packets on V4-only builds ");
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Receive IPV6 MLD packet receive
*
* @param    none
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  on failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmd_ipv6_pkt_receive(mcastControlPkt_t *message)
{
  MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD : Receiving MLD control packets on V4-only builds ");
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Process a MLD (v6) packet
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mgmd_process_v6_packet(mgmd_cb_t *mgmdCB, mcastControlPkt_t * message,
                                   L7_BOOL proxyInterface)
{
  MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD : Processing MLD control packets on V4-only builds ");
  return;
}
/*********************************************************************
* @purpose  Enables/Disables the interface Mcast mode in stack
*  
* @param    mode     @b{(input)} L7_ENABLE/ L7_DISABLE
*
* @returns  none
*
* @notes    none
*
* @end
*
*********************************************************************/
void mgmdProxyV6InterfacesMcastModeSet(L7_uint32 mode)
{
  MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD : Setting Mcast mode for V6 interfaces on V4-only builds ");
  return;
}
/*********************************************************************
* @purpose  Process a MLD (v6) packet
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL mgmd_ipv6_is_scope_id_valid(L7_inet_addr_t *pGroup)
{
  MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD : validatiing MLD control packets scope-id on V4-only builds  ");
  return L7_FALSE;
}
