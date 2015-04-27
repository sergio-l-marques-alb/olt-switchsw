/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  mgmd.c
*
* @purpose   To support MGMD initialization
*
* @component MGMD (IGMP+MLD)
*
* @comments  none
*
* @create    22/10/2007
*
* @author    ddevi
* @end
*
**********************************************************************/

/**********************************************************************
                  Includes
***********************************************************************/

#include <l7_socket.h>
#include "l7_common.h"
#include "l3_addrdefs.h"
#include "l3_mcast_defaultconfig.h"
#include "heap_api.h"
#include "l7_mgmd_api.h"
#include "l7_mgmdmap_include.h"
#include "mgmd_proxy_api.h"
#include "mgmd.h"
#include "mgmd_proxy.h"
#include "mgmd_proxy_debug.h"
#include "mgmd_v6.h"
#include "sdm_api.h"

/**********************************************************************
                  Globals
**********************************************************************/

/* Number of elements in MGMD IPV4 heap pool list, including dummy end-of-list element */
#define MGMD_IPV4_POOL_LIST_SIZE  9
heapBuffPool_t mgmdV4HeapPoolList[MGMD_IPV4_POOL_LIST_SIZE]; 

/* Number of elements in MGMD proxy heap pool list, including dummy end-of-list element */
#define MGMD_PROXY_IPV4_POOL_LIST_SIZE  8
heapBuffPool_t mgmdProxyV4HeapPoolList[MGMD_PROXY_IPV4_POOL_LIST_SIZE];


/*********************************************************************
*
* @purpose  Build the MGMD IPv4 buffer pool list
*
* @param    void
*
* @returns  pointer to first array element
*
* @comments Build list at run time since buffer pool sizes depend on 
*           SDM template scaling parameters.
*
*           When a pool is added here, increment MGMD_IPV4_POOL_LIST_SIZE.
*
* @end
*********************************************************************/
static heapBuffPool_t *mgmdV4HeapPoolListGet(void)
{
  L7_uint32 i = 0;

  memset(mgmdV4HeapPoolList, 0, sizeof(heapBuffPool_t) * MGMD_IPV4_POOL_LIST_SIZE);

  /* MGMD Protocol Control Block Buffer */
  mgmdV4HeapPoolList[i].buffSize = sizeof (mgmd_cb_t);
  mgmdV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdV4HeapPoolList[i].name, "MGMD_CB", HEAP_POOLNAME_SIZE);
  mgmdV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Tree Heap Buffers for MGMD Group Membership Database */
  mgmdV4HeapPoolList[i].buffSize = MGMD_MAX_GROUPS * sizeof (avlTreeTables_t);
  mgmdV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdV4HeapPoolList[i].name, "MGMD_GROUP_TREE_HEAP", HEAP_POOLNAME_SIZE);
  mgmdV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Data Heap Buffers for MGMD Group Membership Database */
  mgmdV4HeapPoolList[i].buffSize = MGMD_MAX_GROUPS * sizeof (mgmd_group_t);
  mgmdV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdV4HeapPoolList[i].name, "MGMD_GROUP_DATA_HEAP", HEAP_POOLNAME_SIZE);
  mgmdV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Group Membership Source Record Buffers */
  mgmdV4HeapPoolList[i].buffSize = sizeof (mgmd_source_record_t);
  mgmdV4HeapPoolList[i].buffCount = MGMD_MAX_SRC_RECORDS;
  osapiStrncpySafe(mgmdV4HeapPoolList[i].name, "MGMD_SOURCE_RECORD_BUFFERS", HEAP_POOLNAME_SIZE);
  mgmdV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Query Request Node Buffers */
  mgmdV4HeapPoolList[i].buffSize = sizeof(mgmd_query_req_t);
  mgmdV4HeapPoolList[i].buffCount = MGMD_MAX_QUERY_REQ_NODES;
  osapiStrncpySafe(mgmdV4HeapPoolList[i].name, "MGMD_QUERY_REQ_NODES_BUFFER", HEAP_POOLNAME_SIZE);
  mgmdV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Timer Buffers */
  mgmdV4HeapPoolList[i].buffSize = sizeof (mgmd_timer_event_info_t);
  mgmdV4HeapPoolList[i].buffCount = MGMD_MAX_TIMER_NODES;
  osapiStrncpySafe(mgmdV4HeapPoolList[i].name, "MGMD_TIMER_NODES_BUFFER", HEAP_POOLNAME_SIZE);
  mgmdV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Handle List Buffers */
  mgmdV4HeapPoolList[i].buffSize = (sizeof(handle_member_t) * MGMD_MAX_TIMERS);
  mgmdV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdV4HeapPoolList[i].name, "MGMD_HANDLE_NODES_BUFFER", HEAP_POOLNAME_SIZE);
  mgmdV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Group Membership Source Address Buffers */
  mgmdV4HeapPoolList[i].buffSize = sizeof (L7_inet_addr_t);
  mgmdV4HeapPoolList[i].buffCount = MGMD_MAX_SRC_RECORDS;
  osapiStrncpySafe(mgmdV4HeapPoolList[i].name, "MGMD_SOURCE_ADDR_BUFFERS", HEAP_POOLNAME_SIZE);
  mgmdV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Last item in the Heap Pool ... DO NOT REMOVE/MODIFY IT */
  mgmdV4HeapPoolList[i].buffSize = 0;
  mgmdV4HeapPoolList[i].buffCount = 0;
  osapiStrncpySafe(mgmdV4HeapPoolList[i].name, "LAST POOL INDICATOR", HEAP_POOLNAME_SIZE);
  mgmdV4HeapPoolList[i].flags = 0;
  i++;

  if (i != MGMD_IPV4_POOL_LIST_SIZE)
  {
    LOG_ERROR(i);
  }
  return mgmdV4HeapPoolList;
}

/*********************************************************************
*
* @purpose  Build the MGMD Proxy IPv4 buffer pool list
*
* @param    void
*
* @returns  pointer to first array element
*
* @comments Build list at run time since buffer pool sizes depend on 
*           SDM template scaling parameters.
*
*           When a pool is added here, increment MGMD_PROXY_IPV4_POOL_LIST_SIZE.
*
* @end
*********************************************************************/
static heapBuffPool_t *mgmdProxyV4HeapPoolListGet(void)
{
  L7_uint32 i = 0;

  memset(mgmdProxyV4HeapPoolList, 0, sizeof(heapBuffPool_t) * MGMD_PROXY_IPV4_POOL_LIST_SIZE);

  /* MGMD-Proxy Host Information Control Block */
  mgmdProxyV4HeapPoolList[i].buffSize = sizeof (mgmd_host_info_t);
  mgmdProxyV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdProxyV4HeapPoolList[i].name, "MGMD_PROXY_CB", HEAP_POOLNAME_SIZE);
  mgmdProxyV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Tree Heap Buffers for MGMD-Proxy Group Membership Database Database*/
  mgmdProxyV4HeapPoolList[i].buffSize = sizeof (avlTreeTables_t) * MGMD_HOST_MAX_GROUPS;
  mgmdProxyV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdProxyV4HeapPoolList[i].name, "MGMD_PROXY_GRP_TREE_HEAP", HEAP_POOLNAME_SIZE);
  mgmdProxyV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Data Heap Buffers for MGMD-Proxy Group Membership Database */
  mgmdProxyV4HeapPoolList[i].buffSize = MGMD_HOST_MAX_GROUPS * sizeof (mgmd_host_group_t);
  mgmdProxyV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdProxyV4HeapPoolList[i].name, "MGMD_PROXY_GRP_DATA_HEAP", HEAP_POOLNAME_SIZE);
  mgmdProxyV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* MGMD-Proxy Host Source Record + AdminScope Database Buffers */
  mgmdProxyV4HeapPoolList[i].buffSize = sizeof (mgmd_host_source_record_t);
  mgmdProxyV4HeapPoolList[i].buffCount = (MGMD_HOST_MAX_SRC_RECORDS + L7_MCAST_MAX_ADMINSCOPE_ENTRIES );
  osapiStrncpySafe(mgmdProxyV4HeapPoolList[i].name, "MGMD_PROXY_SOURCE_N_ADMINSCOPE_RECORD_BUFFERS", HEAP_POOLNAME_SIZE);
  mgmdProxyV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* MGMD-Proxy Unsolicited Reports Buffer */
  mgmdProxyV4HeapPoolList[i].buffSize = sizeof (mgmd_proxy_unsolicited_rpt_t);
  mgmdProxyV4HeapPoolList[i].buffCount = MGMD_MAX_UNSOLICITED_REPORTS;
  osapiStrncpySafe(mgmdProxyV4HeapPoolList[i].name, "MGMD_PROXY_REPORTS_BUFFER", HEAP_POOLNAME_SIZE);
  mgmdProxyV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Tree Heap Buffers for MGMD-Proxy MRT (S,G) Database*/
  mgmdProxyV4HeapPoolList[i].buffSize = sizeof (avlTreeTables_t) * MGMD_PROXY_MRT_MAX_IPV4_ROUTE_ENTRIES;
  mgmdProxyV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdProxyV4HeapPoolList[i].name, "MGMD_PROXY_MRT_TREE_HEAP", HEAP_POOLNAME_SIZE);
  mgmdProxyV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Data Heap Buffers for MGMD-Proxy MRT (S,G) Database */
  mgmdProxyV4HeapPoolList[i].buffSize = MGMD_PROXY_MRT_MAX_IPV4_ROUTE_ENTRIES * sizeof (mgmdProxyCacheEntry_t);
  mgmdProxyV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdProxyV4HeapPoolList[i].name, "MGMD_PROXY_MRT_DATA_HEAP", HEAP_POOLNAME_SIZE);
  mgmdProxyV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Last item in the Heap Pool ... DO NOT REMOVE/MODIFY IT */
  mgmdProxyV4HeapPoolList[i].buffSize = 0;
  mgmdProxyV4HeapPoolList[i].buffCount = 0;
  osapiStrncpySafe(mgmdProxyV4HeapPoolList[i].name, "LAST POOL INDICATOR", HEAP_POOLNAME_SIZE);
  mgmdProxyV4HeapPoolList[i].flags = 0;
  i++;

  if (i != MGMD_PROXY_IPV4_POOL_LIST_SIZE)
  {
    LOG_ERROR(i);
  }
  return mgmdProxyV4HeapPoolList;
}

/*********************************************************************
*
* @purpose  To Initialize the Memory for MGMD Protocol
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
L7_RC_t mgmdMemoryInit (L7_uchar8 addrFamily)
{
  heapBuffPool_t *mgmdHeapPoolList = L7_NULLPTR;

  MGMD_DEBUG(MGMD_DEBUG_APIS, "Entry, family = %d", addrFamily);

  if (addrFamily == L7_AF_INET)
  {
    mgmdHeapPoolList = mgmdV4HeapPoolListGet();
  }
  else if (addrFamily == L7_AF_INET6)
  {
    /* If active SDM template does not support IPv6, IPv6 mgmd heap not allocated. */
    if (!sdmTemplateSupportsIpv6())
    {
      return L7_SUCCESS;
    }
    mgmdHeapPoolList = mgmdV6HeapPoolListGet();
  }
  else
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS,"Bad Address Family - %d", addrFamily);
    return L7_FAILURE;
  }

  if (heapInit (mgmdMapHeapIdGet(addrFamily), mgmdHeapPoolList,
                HEAP_LOCK_GUARD)
             != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS," MGMD Heap Initialization Failed");
    return L7_FAILURE;
  }
  MGMD_DEBUG (MGMD_DEBUG_APIS,"MGMD Memory Initialization Successful for addrFamily - %d",
               addrFamily);
  MGMD_DEBUG (MGMD_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To De-Initialize the Memory for MGMD Protocol
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
L7_RC_t mgmdMemoryDeInit (L7_uchar8 addrFamily)
{
  MGMD_DEBUG (MGMD_DEBUG_APIS, "Entry , family = %d ", addrFamily);

  /* Validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Bad Address Family");
    return L7_FAILURE;
  }

  /* If active SDM template doesn't support IPv6, IPv6 mgmd 
   * template not allocated. */
  if ((addrFamily == L7_AF_INET6) && !sdmTemplateSupportsIpv6())
  {
    return L7_SUCCESS;
  }

  /* De-Initialize the Heap memory */
  if (heapDeInit (mgmdMapHeapIdGet (addrFamily), L7_FALSE) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_APIS,
                 "MGMD Heap De-Init Failed for Family - %d", addrFamily);
    return L7_FAILURE;
  }

  MGMD_DEBUG (MGMD_DEBUG_APIS,
               "MGMD Memory De-Initialization Successful");

  MGMD_DEBUG (MGMD_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Compare two mgmd membership Entries.
*
* @param    pData1   @b{ (input) }  pointer to the first entry
* @param    pData2   @b{ (input) }  pointer to the second entry
* @param    size     @b{ (input) }  size of the key in each entry (ignored as of now).
*
* @returns  >0, if pData1 is greater than pData2.
* @returns  =0, if pData1 is equal to pData2.
* @returns  <0, if pData1 is less than pData2.
*
* @notes
*
* @end
*********************************************************************/
static L7_int32 mgmdMembershipEntryCompare(const void *pData1, const void *pData2, size_t size)
{
  mgmd_group_t *pKey1 = (mgmd_group_t *)pData1;
  mgmd_group_t *pKey2 = (mgmd_group_t *)pData2;
  register L7_int32 retVal = 0;

  if ((pKey1 == L7_NULLPTR) || (pKey2 == L7_NULLPTR))
  {
    return 1;
  }

  if ((retVal = L7_INET_ADDR_COMPARE(&(pKey1->group), &(pKey2->group))) == 0)
  {
    retVal =  memcmp(&(pKey1->rtrIfNum), &(pKey2->rtrIfNum), sizeof(L7_uint32));
  }
  return retVal;
}

/*********************************************************************
*
* @purpose  Create the AVL tree for group membership
*
* @param    mgmdCB  @b{ (input) } MGMD Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t mgmd_create_avltree (mgmd_cb_t* mgmdCB)
{
  /* Initialize memory for the membership group */
  mgmdCB->mgmdMembershipTreeHeap = (avlTreeTables_t *)MGMD_ALLOC(mgmdCB->proto,
                                                                 (sizeof(avlTreeTables_t) * MGMD_MAX_GROUPS));
  mgmdCB->mgmdMembershipDataHeap = (void *)MGMD_ALLOC(mgmdCB->proto,
                                                      sizeof(mgmd_group_t) * MGMD_MAX_GROUPS);

  if ((mgmdCB->mgmdMembershipTreeHeap == L7_NULLPTR) || 
      (mgmdCB->mgmdMembershipDataHeap == L7_NULLPTR))
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error allocating memory for membership group\n");
    if (mgmdCB->mgmdMembershipTreeHeap != L7_NULLPTR)
      osapiFree(L7_FLEX_MGMD_MAP_COMPONENT_ID, mgmdCB->mgmdMembershipTreeHeap);
    mgmdCB->mgmdMembershipTreeHeap = L7_NULLPTR;
    mgmdCB->mgmdMembershipDataHeap = L7_NULLPTR;
    return L7_FAILURE;
  }

  /* Initialize the Membership Group on an AVL tree */
  memset(&(mgmdCB->membership), 0, sizeof(avlTree_t));

  avlCreateAvlTree(&(mgmdCB->membership),         /* Pointer to the AVL Tree object */
                   mgmdCB->mgmdMembershipTreeHeap,/* Pointer to a AVL Tree Node/Entry heap */
                   mgmdCB->mgmdMembershipDataHeap,/* Pointer to a data heap */
                   MGMD_MAX_GROUPS,               /* Max. number of entries supported */
                   sizeof(mgmd_group_t),          /* Size of the data portion of each entry */
                   AVL_TREE_TYPE,                 /* Generic value for AVL Tree type */
                   sizeof(L7_uint32) + 
                   (sizeof(L7_inet_addr_t) * 2)); /* Length of the key - <G, R> */

  /* check AVL semaphore ID to estimate if create succeeded */
  if (mgmdCB->membership.semId == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Error creating the membership object\n");
    osapiFree(L7_FLEX_MGMD_MAP_COMPONENT_ID, mgmdCB->mgmdMembershipTreeHeap);
    osapiFree(L7_FLEX_MGMD_MAP_COMPONENT_ID, mgmdCB->mgmdMembershipDataHeap);
    mgmdCB->mgmdMembershipTreeHeap = L7_NULLPTR;
    mgmdCB->mgmdMembershipDataHeap = L7_NULLPTR;
    return L7_FAILURE;
  }
  avlSetAvlTreeComparator(&(mgmdCB->membership), mgmdMembershipEntryCompare);

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Delete the AVL tree and free the memory associated with it
*
* @param    mgmdCB  @b{ (input) } MGMD Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t mgmd_delete_avltree (mgmd_cb_t* mgmdCB)
{
  avlDeleteAvlTree(&(mgmdCB->membership));

  if (mgmdCB->mgmdMembershipDataHeap)
    MGMD_FREE(mgmdCB->proto,mgmdCB->mgmdMembershipDataHeap);

  if (mgmdCB->mgmdMembershipTreeHeap)
    MGMD_FREE(mgmdCB->proto,mgmdCB->mgmdMembershipTreeHeap);

  return L7_SUCCESS;
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

static L7_RC_t mgmd_init_sockFD (mgmd_cb_t *mgmdCB)
{
  L7_uint32 sockFD = MGMD_FAILURE;

  if (mgmdCB->proto == L7_AF_INET)
  {
    int sockOpt;
    /* Open the socket for communication */
    if (osapiSocketCreate(mgmdCB->proto, L7_SOCK_RAW, IPPROTO_IGMP, &sockFD) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Socket Open Failed \n");
      return(L7_FAILURE);
    }

    sockOpt = 0xC0;
    if (osapiSetsockopt(sockFD, IPPROTO_IP,L7_IP_TOS, 
                        (L7_uchar8 *)&sockOpt, sizeof(sockOpt)) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD: TOS socket option Failed \n");
    }

    /* Tell the OS to copy all forwarded packets that have the Router Alert 
       header option set to this socket */
    sockOpt= L7_TRUE;
    if (osapiSetsockopt(sockFD, IPPROTO_IP,L7_IP_ROUTER_ALERT, 
                        (L7_uchar8 *)&sockOpt, sizeof(sockOpt)) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD: Router-alert socket option Failed \n");
    }
    /* When receiving with osapiPktInfoRecv(), tell OS to have the IP options 
       delivered in the control message, so we can verify the packet has 
       router-alert set. */
    sockOpt= L7_TRUE;
    if (osapiSetsockopt(sockFD, IPPROTO_IP,L7_IP_OPTIONS, 
                        (L7_uchar8 *)&sockOpt, sizeof(sockOpt)) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD: IP_OPTIONS socket option Failed \n");
    }
    /* This can be enabled only for systems supporting this option
     * Disable loopback of the multicast packets.
     * Ignore failure as its not supported on all platforms
     */
    sockOpt= L7_FALSE;
    if(osapiSetsockopt(sockFD, IPPROTO_IP, L7_IP_MULTICAST_LOOP,
                       (L7_uchar8 *)&sockOpt, sizeof(sockOpt)) < 0)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD: IP_MULTICAST_LOOP socket option Failed \n");
    }    
    /* Memory needed to send a flood of reports in the worst-case */
    /* /2 because kernel gives us twice the space we ask for */
    sockOpt = (28+16*(1+MGMD_MAX_QUERY_SOURCES))*MGMD_MAX_GROUPS/2;
    sockOpt += 32768; /* For breathing room */
    if (osapiSetsockopt(sockFD, L7_SOL_SOCKET, L7_SO_SNDBUF,
                      (L7_uchar8 *) & sockOpt, sizeof(sockOpt)) == L7_FAILURE)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:L7_SO_SNDBUF socket option failed\n");
      osapiSocketClose(sockFD);
      return L7_FAILURE;
    }

    mgmdCB->sockfd = sockFD;     
  }
  else if (mgmdCB->proto == L7_AF_INET6)
  {
    mgmd_init6_sockFd(mgmdCB);
  }
  return(L7_SUCCESS);
}
/*********************************************************************
*
* @purpose  Initialize the MGMD protocol control block and
*           elements within MGMD CB
*
* @param    familyType    @b{ (input) }   Family Type
* @param    ebHandle      @b{ (input) }   EB Handle
*
* @returns  MCAST_CB_HNDL_t
* @returns  L7_NULL
*
* @notes
*
* @end
*********************************************************************/
MCAST_CB_HNDL_t mgmdCtrlBlockInit (L7_uchar8 familyType)
{
  mgmd_cb_t *mgmdCB = L7_NULLPTR;
  MCAST_CB_HNDL_t     mgmdHostCB = L7_NULLPTR; 
  L7_uint32  appTimerbufPoolId;

  if ((familyType == L7_AF_INET6) && !sdmTemplateSupportsIpv6())
  {
    return L7_NULLPTR;
  }

  /* Allocate memory for the MGMD Control Block */
  if ((mgmdCB = (mgmd_cb_t*)MGMD_ALLOC(familyType,
                                       sizeof(mgmd_cb_t))) == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Allocation of MGMD Control Block Failed.\n");
    return L7_NULLPTR;
  }

  memset(mgmdCB, 0 , sizeof(mgmd_cb_t));
  mgmdCB->proto   = familyType;

    /* Create Socket FD */
  if (mgmd_init_sockFD (mgmdCB) != L7_SUCCESS)
  {
    L7_LOGF (L7_LOG_SEVERITY_WARNING, L7_FLEX_MGMD_MAP_COMPONENT_ID,
             "MGMD Socket Creation/Initialization Failed for addrFamily - %d"
             " MGMD Socket Creation/options Set Failed. As a result of this,"
             " the MGMD Control packets cannot be sent out on an interface.",
             familyType);
    mgmdCtrlBlockDeInit ((void *) mgmdCB);
    return L7_NULLPTR;
  }

  /* Create the AVL tree for group membership */
  if (mgmd_create_avltree (mgmdCB) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:MGMD AVL Tree Creation Failed.\n");
    mgmdCtrlBlockDeInit ((void *) mgmdCB);
    return L7_NULLPTR;
  }

  /* Set the ALL IGMP Host Address */
  if (inetAllHostsAddressInit (mgmdCB->proto, &(mgmdCB->all_hosts)) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:MGMD ALL Hosts Init Failed.\n");
    mgmdCtrlBlockDeInit ((void *) mgmdCB);
    return L7_NULLPTR;
  }

  /* Set the ALL IGMP Router Address */
  if (inetAllRoutersAddressInit (mgmdCB->proto, &(mgmdCB->all_routers)) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:MGMD ALL Routers Init Failed.\n");
    mgmdCtrlBlockDeInit ((void *) mgmdCB);
    return L7_NULLPTR;
  }

  memset (mgmdCB->mgmd_info, 0, ((sizeof(mgmd_info_t))*MAX_INTERFACES));
  memset (&mgmdCB->counters,0,sizeof(mgmd_counters_t));

  if (mcastMapGetAppTimerBufPoolId(&appTimerbufPoolId) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Unable to get Buffer Pool ID for App Timer\n");
    mgmdCtrlBlockDeInit ((void *) mgmdCB);
    return L7_NULLPTR;
  }

  /* Initialize the APP timer for MGMD */
  if (((mgmdCB->timerHandle) = appTimerInit(L7_FLEX_MGMD_MAP_COMPONENT_ID, 
                                            mgmdTimerExpiryHdlr, 
                                            (void *) mgmdCB, L7_APP_TMR_1SEC, 
                                            appTimerbufPoolId)) == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:MGMD App Timer Initialization Failed.\n");
    mgmdCtrlBlockDeInit ((void *) mgmdCB);
    return L7_NULLPTR;
  }

  mgmdCB->handleListMem = MGMD_ALLOC(mgmdCB->proto,(sizeof(handle_member_t) * MGMD_MAX_TIMERS));
  if (mgmdCB->handleListMem == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:MGMD App Timer Initialization Failed.\n");
    mgmdCtrlBlockDeInit ((void *) mgmdCB);
    return L7_NULLPTR;
  }

  if(handleListInit(L7_FLEX_MGMD_MAP_COMPONENT_ID,
                   MGMD_MAX_TIMERS,
                   &mgmdCB->handle_list,
                   (handle_member_t *)(mgmdCB->handleListMem)) != L7_SUCCESS)
  {
    mgmdCtrlBlockDeInit ((void *) mgmdCB);
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Unable to create handle list ");
    return L7_NULLPTR;
  }

  if (mgmdMapProtocolProxyCtrlBlockGet(familyType , &mgmdHostCB) == L7_SUCCESS)
  {
    mgmdCB->mgmd_host_info = mgmdHostCB;
  }
  return mgmdCB;
}


/*********************************************************************
*
* @purpose  De-Initialize the MGMD protocol control block
*
* @param    cbHandle  @b{ (input) } MGMD Control Block

*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This module assumes that mgmdCB was initialized and memory
*           allocated for mgmdCB
*
* @end
*********************************************************************/
L7_RC_t mgmdCtrlBlockDeInit (MCAST_CB_HNDL_t cbHandle)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)cbHandle;

  if (mgmdCB == L7_NULLPTR)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"Invalid control block ");
    return L7_FAILURE;
  }

  /* Close the socket FD */
  if (mgmdCB->sockfd >= MGMD_ZERO)
    osapiSocketClose(mgmdCB->sockfd);

  /* if the socket is V6 => Deregister with eventType scheduler */
  /* eventSchDeRegisterSocketFd */
  if (mgmdCB->proto == L7_AF_INET6)
  {
    mgmd_deinit6_sockFD(mgmdCB);
  }

  /* Delete the AVL Tree */
  mgmd_delete_avltree(mgmdCB);

  /* Delete the Timer Handle */
  if ((mgmdCB)->timerHandle != L7_NULLPTR)
  {
    appTimerDeInit(mgmdCB->timerHandle);
  }
  /* Free the memory allocated for the Execution Block */
  MGMD_FREE(mgmdCB->proto,mgmdCB->handleListMem);

  /* Free the memory allocated for MGMD Control Block */
  MGMD_FREE(mgmdCB->proto,mgmdCB);

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Compare two mgmd membership Entries.
*
* @param    pData1   @b { (input) }   pointer to the first entry
*           pData2   @b { (input) }   pointer to the second entry
*           size     @b { (input) }   size of the key in each entry (ignored as of now).
*
* @returns  >0, if pData1 is greater than pData2.
* @returns  =0, if pData1 is equal to pData2.
* @returns  <0, if pData1 is less than pData2.
*
* @notes
*
* @end
*********************************************************************/
static L7_int32 mgmd_host_membership_entry_compare(const void *pData1, const void *pData2, size_t size)
{
  L7_inet_addr_t *pKey1 = (L7_inet_addr_t *)pData1;
  L7_inet_addr_t *pKey2 = (L7_inet_addr_t *)pData2;

  if ((pKey1 == L7_NULLPTR) || (pKey2 == L7_NULLPTR))
  {
    return 1;
  }

  return L7_INET_ADDR_COMPARE(pKey1, pKey2);

}
/*****************************************************************
* @purpose Destroys the mgmd Proxy reports
*
* @param    rreq  @b{(input)}   Report to be deleted
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes 
* @end
********************************************************************/
static L7_RC_t mgmd_proxy_report_destroy (L7_sll_member_t *pData)
{
  mgmd_proxy_unsolicited_rpt_t *rreq = (mgmd_proxy_unsolicited_rpt_t *)pData;
  mgmd_cb_t *mgmdCB;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n   Entered ");

  if (rreq == L7_NULLPTR)
    return L7_FAILURE;

  mgmdCB = rreq->mgmdCB;
  mgmdProxyUtilAppTimerHandleDelete(mgmdCB, &(rreq->timer) , &(rreq->timer_handle));
  
  MGMD_PROXY_FREE(mgmdCB->proto, (rreq));  
  return L7_SUCCESS;
}/*End-of-Function.*/

/*****************************************************************
* @purpose Compares the mgmd Proxy reports
*
* @param    pData1   @b{ (input) }  pointer to the first entry
* @param    pData2   @b{ (input) }  pointer to the second entry
* @param    size     @b{ (input) }  size of the key in each entry (ignored as of now).
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes 
* @end
********************************************************************/
static L7_int32 mgmd_proxy_report_compare (void *pData1, void *pData2, size_t size)
{
  mgmd_proxy_unsolicited_rpt_t *pKey1 = (mgmd_proxy_unsolicited_rpt_t *)pData1;
  mgmd_proxy_unsolicited_rpt_t *pKey2 = (mgmd_proxy_unsolicited_rpt_t *)pData2;

  if ((pKey1 == L7_NULLPTR) || (pKey2 == L7_NULLPTR))
  {
    return 1;
  }

  return memcmp(&(pKey1->reportId), &(pKey2->reportId), sizeof(L7_uint32));
}
/*********************************************************************
*
* @purpose  Compares the two Adminscope Boundary Nodes for a Group
*
* @param     asbNode1        @b{ (input) } Pointer to the Admin Scope node
* @param     asbNode2        @b{ (input) } Pointer to the Admin Scope node
*
* @returns  len, difference between two nodes
*
* @comments
*
* @end
*********************************************************************/

static L7_int32 mgmdAdminScopeBoundaryNodeCompare(void       *asbNode1,
                                                 void       *asbNode2,
                                                 L7_uint32  len)
{
  mgmdProxyASBNode_t *tempCurrNode1 = (mgmdProxyASBNode_t*)asbNode1;
  mgmdProxyASBNode_t *tempCurrNode2 = (mgmdProxyASBNode_t*)asbNode2;

  if (L7_INET_ADDR_COMPARE(&tempCurrNode1->grpAddr, &tempCurrNode2->grpAddr) != 0)
  {
    return L7_INET_ADDR_COMPARE(&tempCurrNode1->grpAddr, &tempCurrNode2->grpAddr);
  }
  return L7_INET_ADDR_COMPARE(&tempCurrNode1->grpMask, &tempCurrNode2->grpMask);
}
/*********************************************************************
*
* @purpose  Delete the Adminscope Boundary Node for a Group
*
* @param     asbNode        @b{ (input) } Pointer to the Admin Scope node
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t mgmdAdminScopeBoundaryNodeDelete(L7_sll_member_t *asbNode)
{
  mgmdProxyASBNode_t *tempCurrNode = (mgmdProxyASBNode_t*)asbNode;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  if (tempCurrNode != L7_NULLPTR)
  {
    MGMD_PROXY_FREE(tempCurrNode->family, asbNode);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose   Compare function for the MRT SC tree
*
* @param     pData1 @b{ (input) } Pointer to MRT Entry
*            pData2 @b{ (input) } Pointer to MRT Entry
*            size   @b{ (input) } Size for the comparision 
*                            
* @returns   > 0  if pData1 > pData2
*            = 0 if pData1 == pData2
*            < 0 if pData1 < pData2
*
* @comments  None
*
* @end
*********************************************************************/
static L7_int32
mgmdProxyCacheEntryCompare (const void* pData1,
                            const void* pData2,
                            size_t size)
{
  mgmdProxyCacheEntry_t *pKey1 = (mgmdProxyCacheEntry_t *) pData1;
  mgmdProxyCacheEntry_t *pKey2 = (mgmdProxyCacheEntry_t *) pData2;
  register L7_int32 retVal = 0;

  if ((pKey1 == L7_NULLPTR) || (pKey2 == L7_NULLPTR))
  {
    return 1;
  }

  if ((retVal = L7_INET_ADDR_COMPARE (&(pKey1->grpAddr),
                                      &(pKey2->grpAddr))) != 0)
  {
    return retVal;
  }

  retVal = L7_INET_ADDR_COMPARE (&(pKey1->srcAddr), &(pKey2->srcAddr));

  return retVal;
}
/*********************************************************************
*
* @purpose  To Initialize the Memory for MGMD Proxy Protocol
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
L7_RC_t mgmdProxyMemoryInit (L7_uchar8 addrFamily)
{
  heapBuffPool_t *mgmdProxyHeapPoolList = L7_NULLPTR;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Entry, family = %d", addrFamily);

  if (addrFamily == L7_AF_INET)
  {
    mgmdProxyHeapPoolList = mgmdProxyV4HeapPoolListGet();
  }
  else if (addrFamily == L7_AF_INET6)
  {
    if (!sdmTemplateSupportsIpv6())
    {
      /* Active SDM template does not support IPv6. IPv6 heap not allocated. */
      return L7_SUCCESS;
    }
    mgmdProxyHeapPoolList = mgmdProxyV6HeapPoolListGet();
  }
  else
  {
    MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS,"Bad Address Family - %d", addrFamily);
    return L7_FAILURE;
  }

  if (heapInit (mcastMapHeapIdGet(addrFamily), mgmdProxyHeapPoolList,
                HEAP_LOCK_GUARD)
             != L7_SUCCESS)
  {
    MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS," MGMD Proxy Heap Initialization Failed");
    return L7_FAILURE;
  }
  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS,"MGMD Proxy Memory Initialization Successful for addrFamily - %d",
               addrFamily);
  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To De-Initialize the Memory for MGMD Proxy Protocol
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
L7_RC_t mgmdProxyMemoryDeInit (L7_uchar8 addrFamily)
{
  MGMD_DEBUG (MGMD_PROXY_DEBUG_APIS, "Entry , family = %d ", addrFamily);

  /* Validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Bad Address Family");
    return L7_FAILURE;
  }

  /* If active SDM template doesn't support IPv6, v6 heap not allocated. */
  if ((addrFamily == L7_AF_INET6) && (!sdmTemplateSupportsIpv6()))
  {
    return L7_SUCCESS;
  }

  /* De-Initialize the Heap memory */
  if (heapDeInit (mcastMapHeapIdGet (addrFamily), L7_FALSE) != L7_SUCCESS)
  {
    MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS,
                 " MGMD Proxy Heap De-Init Failed for Family - %d", addrFamily);
    return L7_FAILURE;
  }

  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS,
               "MGMD Proxy Memory De-Initialization Successful");

  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Exit");
  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}
/*****************************************************************
* @purpose  Proxy Init functionality
*
* @param    mgmdCB    @b{ (input) }   MGMD Control Block
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
MCAST_CB_HNDL_t mgmdProxyCtrlBlockInit (L7_uchar8 familyType )           
{
  mgmd_host_info_t    *mgmdHostCB;
  MCAST_CB_HNDL_t      mgmdCB = L7_NULLPTR; 
  L7_uint32 mrtTableSize = 0;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n Entered ");


  mgmdHostCB = MGMD_PROXY_ALLOC(familyType, sizeof(mgmd_host_info_t));

  memset(mgmdHostCB, 0, sizeof(mgmd_host_info_t));

  mgmdHostCB->family = familyType;

  mgmdHostCB->router_report_event = mgmd_proxy_routerReportEvent;
  mgmdHostCB->router_timer_event = mgmd_proxy_routerTimerEvents;

  /* Initialize memory for the membership group */
  mgmdHostCB->mgmdHostMembershipTreeHeap = (avlTreeTables_t *)MGMD_PROXY_ALLOC(mgmdHostCB->family,
                                                                              (sizeof(avlTreeTables_t) * MGMD_HOST_MAX_GROUPS));
  mgmdHostCB->mgmdHostMembershipDataHeap = (void *)MGMD_PROXY_ALLOC(mgmdHostCB->family,
                                                                   (sizeof(mgmd_host_group_t) * MGMD_HOST_MAX_GROUPS));
  if ((mgmdHostCB->mgmdHostMembershipTreeHeap == L7_NULLPTR) || 
      (mgmdHostCB->mgmdHostMembershipDataHeap == L7_NULLPTR))
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Error allocating memory for Host membership group\n");
    mgmdProxyCtrlBlockDeInit((void *)mgmdHostCB);
    return L7_NULLPTR;
  }

  /* Initialize the Membership Group on an AVL tree */
  memset(&(mgmdHostCB->hostMembership), 0, sizeof(avlTree_t));

  avlCreateAvlTree(&(mgmdHostCB->hostMembership), /* Pointer to the AVL Tree object */
                   mgmdHostCB->mgmdHostMembershipTreeHeap,        /* Pointer to a AVL Tree Node/Entry heap */
                   mgmdHostCB->mgmdHostMembershipDataHeap,        /* Pointer to a data heap */
                   MGMD_HOST_MAX_GROUPS,                      /* Max. number of entries supported */
                   sizeof(mgmd_host_group_t),         /* Size of the data portion of each entry */
                   AVL_TREE_TYPE,                     /* Generic value for AVL Tree type */
                   sizeof(L7_inet_addr_t));           /* Length of the key - <Group> */
  /* check AVL semaphore ID to estimate if create succeeded */
  if (mgmdHostCB->hostMembership.semId == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Error creating the membership object\n");
    mgmdProxyCtrlBlockDeInit((void *)mgmdHostCB);
    return L7_NULLPTR;
  }
  avlSetAvlTreeComparator(&(mgmdHostCB->hostMembership), mgmd_host_membership_entry_compare);


  if (SLLCreate (L7_FLEX_MGMD_MAP_COMPONENT_ID, L7_SLL_NO_ORDER, L7_NULL, mgmd_proxy_report_compare,
                 mgmd_proxy_report_destroy, &(mgmdHostCB->ll_unsolicited_reports)) != L7_SUCCESS)
  {
    mgmdProxyCtrlBlockDeInit((void *)mgmdHostCB);
    return L7_NULLPTR;
  }

  /* Initialize the Proxy Cache Table */

  /* Allocate the MRT AVL Tree Heap */
  if (familyType == L7_AF_INET)
  {
    mrtTableSize = MGMD_PROXY_MRT_MAX_IPV4_ROUTE_ENTRIES;
  }
  else if (familyType == L7_AF_INET6)
  {
    mrtTableSize = MGMD_PROXY_MRT_MAX_IPV6_ROUTE_ENTRIES;
  }
  else
  {
    mgmdProxyCtrlBlockDeInit((void *)mgmdHostCB);
    return L7_NULLPTR;
  }

  if ((mgmdHostCB->mgmdProxyCacheTreeHeap = 
       MGMD_PROXY_ALLOC(mgmdHostCB->family, (mrtTableSize * sizeof (avlTreeTables_t)))) 
       == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MGMD Proxy MRT Tree Heap Allocation Failed\n");
    mgmdProxyCtrlBlockDeInit((void *)mgmdHostCB);
    return L7_NULLPTR;
  }

  /* Allocate the MRT AVL Data Heap */
  if ((mgmdHostCB->mgmdProxyCacheDataHeap = 
       MGMD_PROXY_ALLOC (mgmdHostCB->family,(mrtTableSize * sizeof (mgmdProxyCacheEntry_t))))
       == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MGMD Proxy Tree Data Heap Allocation Failed\n");
    mgmdProxyCtrlBlockDeInit((void *)mgmdHostCB);
    return L7_NULLPTR;
  }

  /* Create the MRT (S,G) AVL Tree */
  avlCreateAvlTree (&mgmdHostCB->mgmdProxyCache, mgmdHostCB->mgmdProxyCacheTreeHeap,
                    mgmdHostCB->mgmdProxyCacheDataHeap, mrtTableSize,
                    (L7_uint32) (sizeof (mgmdProxyCacheEntry_t)),
                    0x10, sizeof (L7_inet_addr_t) * 2);
  if (mgmdHostCB->mgmdProxyCache.semId == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Error creating the membership object\n");
    mgmdProxyCtrlBlockDeInit((void *)mgmdHostCB);
    return L7_NULLPTR;
  }
  avlSetAvlTreeComparator (&mgmdHostCB->mgmdProxyCache, mgmdProxyCacheEntryCompare);


  /* Initialize the Admin Scope SLL */
  if (SLLCreate (L7_FLEX_MGMD_MAP_COMPONENT_ID, L7_SLL_NO_ORDER,
                 L7_NULL, mgmdAdminScopeBoundaryNodeCompare,
                 mgmdAdminScopeBoundaryNodeDelete, &(mgmdHostCB->asbList))
      != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Admin Scope List Creation Failure");
    mgmdProxyCtrlBlockDeInit((void *)mgmdHostCB);
    return L7_NULLPTR;
  }

  /* Register with MFC */
  if (mfcCacheEventRegister (L7_MRP_MGMD_PROXY, mgmdMapComponentCallback, L7_FALSE, 0)
                          != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MFC Registration Failed");
    return L7_NULLPTR;
  }

  mgmdHostCB->hostCompatibilityMode = L7_MGMD_VERSION_3;  

  if (mgmdMapProtocolCtrlBlockGet(familyType , &mgmdCB) == L7_SUCCESS)
  {
    ((mgmd_cb_t *)mgmdCB)->mgmd_host_info = mgmdHostCB;
  }
  return mgmdHostCB;
}

/*****************************************************************
* @purpose  Proxy Deinit functionality
*
* @param    mgmdCB    @b{ (input) }   MGMD Control Block
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyCtrlBlockDeInit (void *mgmdHostCb )           
{
  mgmd_host_info_t    *mgmdHostCB = mgmdHostCb;
  MCAST_CB_HNDL_t      mgmdCB = L7_NULLPTR; 
  
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  if (mgmdHostCB == L7_NULLPTR)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"Invalid control block ");
    return L7_FAILURE;
  }

  /* Delete the AVL tree for groups */
  if (avlDeleteAvlTree(&(mgmdHostCB->hostMembership)) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Could not Delete the AVL tree.\n");
  }

  if (mgmdHostCB->mgmdHostMembershipDataHeap != L7_NULLPTR)
  {
    MGMD_PROXY_FREE(mgmdHostCB->family,mgmdHostCB->mgmdHostMembershipDataHeap);
    mgmdHostCB->mgmdHostMembershipDataHeap = L7_NULLPTR;
  }

  if (mgmdHostCB->mgmdHostMembershipTreeHeap != L7_NULLPTR)
  {
    MGMD_PROXY_FREE(mgmdHostCB->family,mgmdHostCB->mgmdHostMembershipTreeHeap);
    mgmdHostCB->mgmdHostMembershipTreeHeap = L7_NULLPTR;
  }

  /* Delete the SLL for source records */
  if (SLLDestroy(L7_FLEX_MGMD_MAP_COMPONENT_ID, &(mgmdHostCB->ll_unsolicited_reports)) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "SLL not Destroyed.\n");
  }

  /* Destroy the MRT (S,G) Tree */
  if (avlDeleteAvlTree (&mgmdHostCB->mgmdProxyCache) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MRT Tree Destroy Failed\n");
  }

  /* Free the MRT Tree Heap */
  if (mgmdHostCB->mgmdProxyCacheTreeHeap != L7_NULLPTR)
  {
    MGMD_PROXY_FREE(mgmdHostCB->family, mgmdHostCB->mgmdProxyCacheTreeHeap);
    mgmdHostCB->mgmdProxyCacheTreeHeap = L7_NULLPTR;
  }
  
  /* Free the MRT Data Heap */
  if (mgmdHostCB->mgmdProxyCacheDataHeap != L7_NULLPTR)
  {
    MGMD_PROXY_FREE(mgmdHostCB->family, mgmdHostCB->mgmdProxyCacheDataHeap);
    mgmdHostCB->mgmdProxyCacheDataHeap = L7_NULLPTR;
  }

  /* Destory the Admin Scope SLL */
  if (SLLDestroy (L7_FLEX_MGMD_MAP_COMPONENT_ID, &(mgmdHostCB->asbList))
      != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Admin Scope List Destroy Failure");
  }

    /* De-Register with MFC  */
  if (mfcCacheEventDeregister (L7_MRP_MGMD_PROXY) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MFC De-Registration Failed");
  }

  if (mgmdMapProtocolCtrlBlockGet(mgmdHostCB->family , &mgmdCB) == L7_SUCCESS)
  {
    ((mgmd_cb_t *)mgmdCB)->mgmd_host_info = L7_NULLPTR;
  }

  MGMD_PROXY_FREE(mgmdHostCB->family, mgmdHostCB);
  mgmdHostCB = L7_NULL;

  return L7_SUCCESS;  
}

