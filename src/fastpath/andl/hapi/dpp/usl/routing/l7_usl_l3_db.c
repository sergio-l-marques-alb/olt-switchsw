/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_l3_db.c
*
* @purpose    L3 Db implementation
*
* @component  USL
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#include "l7_common.h"

#ifdef L7_ROUTING_PACKAGE

#include "l7_socket.h"
#include "broad_common.h"
#include "osapi.h"
#include "osapi_support.h"
#include "l7_usl_sm.h"
#include "bcmx/l3.h"
#include "l7_usl_trunk_db.h"
#include "l7_usl_l3_db.h"
#include "l7_rpc_l3.h"
#include "avl_api.h"
#include "platform_config.h"
#include "sysapi.h"
#include "l7_usl_trace.h"
#include "bcmx/bcmx_int.h"
#include "unitmgr_api.h"
#include "broad_l3.h"
#include "sysbrds.h"
#include "appl/stktask/topo_brd.h"
#include "bcm_int/esw/multicast.h"

#include "bcm_int/esw/firebolt.h"
#include "broad_l3_debug.h"
#include "wireless_exports.h"

/* Egress objects table resources - next hop objs and multipath objects */
void               *uslIpEgrNhopDbSema = L7_NULLPTR;
avlTreeTables_t    *uslOperIpEgrNhopTreeHeap = L7_NULLPTR;
usl_egr_nhop_db_elem_t  *uslOperIpEgrNhopDataHeap = L7_NULLPTR;
avlTree_t           uslOperIpEgrNhopTreeData   = { 0};
avlTreeTables_t    *uslShadowIpEgrNhopTreeHeap = L7_NULLPTR;
usl_egr_nhop_db_elem_t  *uslShadowIpEgrNhopDataHeap = L7_NULLPTR;
avlTree_t           uslShadowIpEgrNhopTreeData   = { 0};
avlTree_t          *uslIpEgrNhopTreeHandle   = L7_NULLPTR;
L7_BOOL             uslIpEgrNhopDbActive   = L7_FALSE;

avlTreeTables_t          *uslOperIpMpathEgrNhopTreeHeap = L7_NULLPTR;
usl_mpath_egr_nhop_db_elem_t  *uslOperIpMpathEgrNhopDataHeap = L7_NULLPTR;
avlTree_t                 uslOperIpMpathEgrNhopTreeData   = { 0};
avlTreeTables_t          *uslShadowIpMpathEgrNhopTreeHeap = L7_NULLPTR;
usl_mpath_egr_nhop_db_elem_t  *uslShadowIpMpathEgrNhopDataHeap = L7_NULLPTR;
avlTree_t                 uslShadowIpMpathEgrNhopTreeData   = { 0};
avlTree_t                *uslIpMpathEgrNhopTreeHandle   = L7_NULLPTR;
L7_BOOL                   uslIpMpathEgrNhopDbActive   = L7_FALSE;

/* Host Table resources (semaphore and avl tree) */
void               *uslIpHostDbSema = L7_NULLPTR;
avlTreeTables_t    *uslOperIpHostTreeHeap = L7_NULLPTR;
usl_l3_host_db_elem_t      *uslOperIpHostDataHeap = L7_NULLPTR;
avlTree_t           uslOperIpHostTreeData   = { 0};
avlTreeTables_t    *uslShadowIpHostTreeHeap = L7_NULLPTR;
usl_l3_host_db_elem_t      *uslShadowIpHostDataHeap = L7_NULLPTR;
avlTree_t           uslShadowIpHostTreeData   = { 0};
avlTree_t          *uslIpHostTreeHandle   = L7_NULLPTR;
L7_BOOL             uslIpHostDbActive   = L7_FALSE;

/* LPM Table resources (semaphore and avl tree) */
void               *pUslIpLpmDbSema = L7_NULLPTR;
avlTreeTables_t    *pUslOperIpLpmTreeHeap = L7_NULLPTR;
usl_lpm_db_elem_t       *pUslOperIpLpmDataHeap = L7_NULLPTR;
avlTree_t           uslOperIpLpmTreeData   = { 0};
avlTreeTables_t    *pUslShadowIpLpmTreeHeap = L7_NULLPTR;
usl_lpm_db_elem_t       *pUslShadowIpLpmDataHeap = L7_NULLPTR;
avlTree_t           uslShadowIpLpmTreeData   = { 0};
avlTree_t          *uslIpLpmTreeHandle   = L7_NULLPTR;
L7_BOOL             uslIpLpmDbActive   = L7_FALSE;

/* Tunnel Initiator Table resources (semaphore and avl tree) */
void                          *pUslIpTunnelInitiatorDbSema = L7_NULLPTR;
avlTreeTables_t               *pUslOperIpTunnelInitiatorTreeHeap = L7_NULLPTR;
usl_l3_tunnel_initiator_db_elem_t  *pUslOperIpTunnelInitiatorDataHeap = L7_NULLPTR;
avlTree_t                      uslOperIpTunnelInitiatorTreeData   = { 0};
avlTreeTables_t               *pUslShadowIpTunnelInitiatorTreeHeap = L7_NULLPTR;
usl_l3_tunnel_initiator_db_elem_t  *pUslShadowIpTunnelInitiatorDataHeap = L7_NULLPTR;
avlTree_t                      uslShadowIpTunnelInitiatorTreeData   = { 0};
avlTree_t                     *uslIpTunnelInitiatorTreeHandle   = L7_NULLPTR;
L7_BOOL                        uslIpTunnelInitiatorDbActive   = L7_FALSE;

/* Tunnel Terminator Table resources (semaphore and avl tree) */
void                          *pUslIpTunnelTerminatorDbSema = L7_NULLPTR;
avlTreeTables_t               *pUslOperIpTunnelTerminatorTreeHeap = L7_NULLPTR;
usl_l3_tunnel_terminator_db_elem_t *pUslOperIpTunnelTerminatorDataHeap = L7_NULLPTR;
avlTree_t                      uslOperIpTunnelTerminatorTreeData   = { 0};
avlTreeTables_t               *pUslShadowIpTunnelTerminatorTreeHeap = L7_NULLPTR;
usl_l3_tunnel_terminator_db_elem_t *pUslShadowIpTunnelTerminatorDataHeap = L7_NULLPTR;
avlTree_t                      uslShadowIpTunnelTerminatorTreeData   = { 0};
avlTree_t                     *uslIpTunnelTerminatorTreeHandle   = L7_NULLPTR;
L7_BOOL                        uslIpTunnelTerminatorDbActive   = L7_FALSE;

/* L3 Interface Table resources (semaphore, db) */
void               *pUslL3IntfDbSema = L7_NULLPTR;
usl_l3_intf_db_elem_t   *pUslOperL3IntfDb   = L7_NULLPTR;
usl_l3_intf_db_elem_t   *pUslShadowL3IntfDb   = L7_NULLPTR;
usl_l3_intf_db_elem_t   *pUslL3IntfDbHandle   = L7_NULLPTR;
L7_BOOL             uslL3IntfDbActive   = L7_FALSE;

usl_l3_intf_hw_id_list_t *pUslL3IntfHwIdList = L7_NULLPTR;
bcm_if_t                  uslL3IntfHwIdMin = 0, uslL3IntfHwIdMax = 0;

usl_l3_egr_nhop_hw_id_list_t *pUslL3EgrNhopHwIdList = L7_NULLPTR;
bcm_if_t                     uslL3EgrNhopHwIdMin = 0, uslL3EgrNhopHwIdMax = 0;

usl_l3_mpath_egr_nhop_hw_id_list_t *pUslL3MpathEgrNhopHwIdList = L7_NULLPTR;
bcm_if_t uslL3MpathEgrNhopHwIdMin = 0, uslL3MpathEgrNhopHwIdMax = 0;
/* Consecutive mpath egr id's differ by this value. */
L7_uint32 uslL3MpathEgrNhopHwIdOffset = 1;


L7_BOOL      uslL3DbInited   = L7_FALSE;

#ifdef L7_STACKING_PACKAGE
L7_uint32 uslL3DataplaneCleanupTime  = 0; /* Total time spent doing dataplane cleanups */
L7_uint32 uslL3DataplaneCleanupCount = 0; /* Total dataplane cleanups */
#endif

/* The time in milliseconds to wait between checking the in use of the host entries */
/* -- skew is how 'early' the first pass runs compared to the normal delay */
#define USL_HOST_INUSE_TIMER_INTERVAL_SEC   (300)
#define USL_HOST_INUSE_TIMER_INTERVAL_MSEC  (USL_HOST_INUSE_TIMER_INTERVAL_SEC*1000)
#define USL_HOST_INUSE_TIMER_INTERVAL_SKEW  (USL_HOST_INUSE_TIMER_INTERVAL_MSEC >> 1)



#define GEN_ADDR_STR_LEN sizeof("01:02:03:04:05:06:07:08:09:10:11:12:13:14:15:16")


#ifdef L7_STACKING_PACKAGE
/* TIMER to be used for inuse check */
static osapiTimerDescr_t *pUslHostInUseTimer = L7_NULL;

static void usl_l3host_active_timer(L7_uint32 arg1, L7_uint32 arg2);
#endif

#define USL_L3_EGR_NHOP_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L3_HOST_DB_ID,"EGR/NHOP",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(uslIpEgrNhopDbSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", uslIpEgrNhopDbSema); \
  } \
}

#define USL_L3_EGR_NHOP_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L3_HOST_DB_ID,"EGR/NHOP",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(uslIpEgrNhopDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", uslIpEgrNhopDbSema); \
  } \
}

#define USL_L3_LPM_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L3_LPM_DB_ID,"LPM",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslIpLpmDbSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", pUslIpLpmDbSema); \
  } \
}

#define USL_L3_LPM_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L3_LPM_DB_ID,"LPM",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslIpLpmDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", pUslIpLpmDbSema); \
  } \
}

#define USL_L3_HOST_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L3_HOST_DB_ID,"ARP/NDP",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(uslIpHostDbSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", uslIpHostDbSema); \
  } \
}

#define USL_L3_HOST_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L3_HOST_DB_ID,"ARP/NDP",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(uslIpHostDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", uslIpHostDbSema); \
  } \
}


#define USL_L3_TUNNEL_INITIATOR_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L3_TUNNEL_INITIATOR_DB_ID,"TUNNEL_INITIATOR",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslIpTunnelInitiatorDbSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", pUslIpTunnelInitiatorDbSema); \
  } \
}

#define USL_L3_TUNNEL_INITIATOR_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L3_TUNNEL_INITIATOR_DB_ID,"TUNNEL_INITIATOR",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslIpTunnelInitiatorDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", pUslIpTunnelInitiatorDbSema); \
  } \
}

#define USL_L3_TUNNEL_TERMINATOR_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L3_TUNNEL_TERMINATOR_DB_ID,"TUNNEL_TERMINATOR",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslIpTunnelTerminatorDbSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", pUslIpTunnelTerminatorDbSema); \
  } \
}

#define USL_L3_TUNNEL_TERMINATOR_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L3_TUNNEL_TERMINATOR_DB_ID,"TUNNEL_TERMINATOR",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslIpTunnelTerminatorDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", pUslIpTunnelTerminatorDbSema); \
  } \
}

#define USL_L3_INTF_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L3_INTF_DB_ID,"L3I",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslL3IntfDbSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", pUslL3IntfDbSema); \
  } \
}

#define USL_L3_INTF_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L3_INTF_DB_ID,"L3I",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslL3IntfDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", pUslL3IntfDbSema); \
  } \
}

/*********************************************************************
* @purpose  Given a dbType, return the db Handle.
*
* @params   dbType {(input)} OPER/SHADOW/CURRENT
*
* @returns  Pointer to db Handle
*
* @notes    none
*
* @end
*********************************************************************/
avlTree_t *usl_l3_egr_nhop_db_handle_get(USL_DB_TYPE_t dbType)
{
  avlTree_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = &uslOperIpEgrNhopTreeData;
      break;

    case USL_SHADOW_DB:
      dbHandle = &uslShadowIpEgrNhopTreeData;
      break;

    case USL_CURRENT_DB:
      dbHandle = uslIpEgrNhopTreeHandle;
      break;

    default:
      break;
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Get the size of a egr nhop Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_l3_egr_nhop_db_elem(void)
{
  return sizeof(usl_egr_nhop_db_elem_t);
}

/*********************************************************************
* @purpose  Print the contents of a l3 egr nhop db element in
*           specified buffer
*
* @params   elem     {(input)}  Element to be printed
*           buffer   {(input)}  buffer pointer
*           size     {(input)}  size of buffer
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
void usl_print_l3_egr_nhop_db_elem(void *item, L7_uchar8 *buffer,
                                   L7_uint32 size)
{
  L7_char8                addr_str[64];
  usl_egr_nhop_db_elem_t *elem = item;

  USL_L3_NTOP((elem->egrInfo.key.flags & BCM_L3_IP6
               ? L7_AF_INET6
               : L7_AF_INET),
               (elem->egrInfo.key.flags & BCM_L3_IP6
               ? (void *)&elem->egrInfo.key.addr.l3a_ip6_addr
               : (void *)&elem->egrInfo.key.addr.l3a_ip_addr),
               addr_str,
               sizeof(addr_str));

  osapiSnprintf(buffer, size,
                "IP=%s usp %d/%d/%d\n",
                addr_str,
                elem->egrInfo.key.usp.unit,
                elem->egrInfo.key.usp.slot,
                elem->egrInfo.key.usp.port);

  osapiSnprintfcat(buffer, size,
                   "Egr Id=%d, NextHopMac=%0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x\n",
                   elem->egrId,
                   elem->egrInfo.bcm_data.mac_addr[0],
                   elem->egrInfo.bcm_data.mac_addr[1],
                   elem->egrInfo.bcm_data.mac_addr[2],
                   elem->egrInfo.bcm_data.mac_addr[3],
                   elem->egrInfo.bcm_data.mac_addr[4],
                   elem->egrInfo.bcm_data.mac_addr[5]);

  osapiSnprintfcat(buffer, size,
                   "L3 intf=%d, Flags=%x, Module=%d Port=%d, trunk=%d, vlan=%d\n",
                   elem->egrInfo.bcm_data.intf, elem->egrInfo.bcm_data.flags,
                   elem->egrInfo.bcm_data.module, elem->egrInfo.bcm_data.port,
                   elem->egrInfo.bcm_data.trunk, elem->egrInfo.bcm_data.vlan);

  return;
}

/*********************************************************************
* @purpose  Search element in L3 Egr Nhop db and return pointer to it
*
* @params   dbType     {(input)} Db type (Oper or Shadow)
*           flags      {(input)} USL_EXACT_ELEM or USL_NEXT_ELEM
*           searchInfo {(input)} Element to be searched
*           elemInfo   {(output)} Found element info
*
* @returns  BCM_E_NONE: Element found
*           BCM_E_NOT_FOUND: Element not found
*
* @notes    Internal API. Db lock must be held. Returns pointer
*           to the element.
*
* @end
*********************************************************************/
static L7_int32 usl_search_l3_egr_nhop_db_elem(USL_DB_TYPE_t dbType,
                                               L7_uint32 flags,
                                               uslDbElemInfo_t searchInfo,
                                               uslDbElemInfo_t *elemInfo)
{
  avlTree_t                 *dbHandle = L7_NULLPTR;
  L7_int32                   rv = BCM_E_NOT_FOUND;
  L7_uchar8                  traceStr[128];


  osapiSnprintf(traceStr, sizeof(traceStr),
                "Get Elem: Flags %d \n",
                flags);
  usl_trace_sync_detail(USL_L3_EGR_NHOP_DB_ID, traceStr);


  elemInfo->dbElem = L7_NULLPTR;
  elemInfo->elemIndex = USL_INVALID_DB_INDEX;

  do
  {

    dbHandle = usl_l3_egr_nhop_db_handle_get(dbType);

    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Get next */
    if (flags == USL_DB_NEXT_ELEM)
    {
      elemInfo->dbElem = avlSearchLVL7(dbHandle, searchInfo.dbElem, AVL_NEXT);
    }
    else if (flags == USL_DB_EXACT_ELEM)
    {
      elemInfo->dbElem = avlSearchLVL7(dbHandle, searchInfo.dbElem, AVL_EXACT);
    }

    if (elemInfo->dbElem == L7_NULLPTR)
    {
      rv = BCM_E_NOT_FOUND;
    }
    else
    {
      rv = BCM_E_NONE;
    }

  } while (0);


  return rv;
}

/*********************************************************************
* @purpose  Get info about an element from the Db
*
* @params   dbType        {(input)}  dbType to be searched
*           flags         {(input)}  EXACT or GET_NEXT
*           searchInfo    {(input)}  Information about element to be searched
*           elemInfo      {(output)} Storage for element & index
*
* @returns  BCM error code
*
* @notes    Caller must allocate memory to copy the db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_l3_egr_nhop_db_elem(USL_DB_TYPE_t     dbType,
                                     L7_uint32         flags,
                                     uslDbElemInfo_t   searchElem,
                                     uslDbElemInfo_t  *elemInfo)
{
  L7_int32         rv = BCM_E_NONE;
  uslDbElemInfo_t  nhopInfo;

  if (!uslIpEgrNhopDbActive)
  {
    return rv;
  }

  nhopInfo.dbElem = L7_NULLPTR;
  nhopInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_L3_EGR_NHOP_DB_LOCK_TAKE();

  rv = usl_search_l3_egr_nhop_db_elem(dbType, flags, searchElem, &nhopInfo);
  if (rv == BCM_E_NONE)
  {
    memcpy(elemInfo->dbElem, nhopInfo.dbElem, sizeof(usl_egr_nhop_db_elem_t));
  }

  USL_L3_EGR_NHOP_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Delete a given element from Egr Nhop Db
*
* @params   dbType   {(input)}
*           elemInfo {(input)}
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_l3_egr_nhop_db_elem(USL_DB_TYPE_t   dbType,
                                        uslDbElemInfo_t elemInfo)
{
  avlTree_t               *dbHandle;
  usl_egr_nhop_db_elem_t  *elem;
  L7_int32                 rv = BCM_E_NONE;

  if (!uslIpEgrNhopDbActive)
  {
    return rv;
  }

  USL_L3_EGR_NHOP_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_l3_egr_nhop_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Delete the elem */
    elem = avlDeleteEntry(dbHandle, elemInfo.dbElem);
    if (elem == L7_NULLPTR)
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);

  USL_L3_EGR_NHOP_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Create an Egress Nhop object in USL Db
*
* @param    dbType   @{(input)}  dbType to be modified
* @param    egrIntf  @{(input)}  Egress Id of the nexthop
* @param    pBcmInfo @{(input)}  BCM data for the nexthop
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_egress_create(USL_DB_TYPE_t dbType, bcm_if_t egrIntf,
                            usl_bcm_l3_egress_t *pBcmInfo)
{
  int                     rv = BCM_E_NONE;
  usl_egr_nhop_db_elem_t  data;
  usl_egr_nhop_db_elem_t *pData;
  avlTree_t              *nhopTree;


  if (uslIpEgrNhopDbActive == L7_FALSE)
  {
    return rv;
  }

  nhopTree = usl_l3_egr_nhop_db_handle_get(dbType);
  if (nhopTree == L7_NULLPTR)
  {
    return BCM_E_FAIL;
  }

  USL_L3_EGR_NHOP_DB_LOCK_TAKE();


  memset(&data, 0, sizeof(data));
  memcpy(&(data.egrInfo), pBcmInfo, sizeof(usl_bcm_l3_egress_t));

  /* No need to save the BCM_L3_WITH_ID/BCM_L3_REPLACE flags as they
  ** are operational commands
  */
  data.egrInfo.bcm_data.flags &= ~(BCM_L3_WITH_ID | BCM_L3_REPLACE);

  data.egrId = egrIntf;

  pData = avlInsertEntry(nhopTree, &data);

  if (pData == &data)
  {
    rv = BCM_E_FAIL;
    USL_LOG_ERROR("USL: Error adding egress object entry \n");
  }
  else if (pData != L7_NULLPTR)
  {
    /* It is OK to update existing egress objects */
    memcpy(&(pData->egrInfo), &(data.egrInfo), sizeof(usl_bcm_l3_egress_t));
    pData->egrId = data.egrId;
  }

  USL_L3_EGR_NHOP_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Destroy an Egress Nhop object in USL Db
*
* @param    dbType   @{(input)}  dbType to be modified
* @param    pBcmInfo  @{(output)} BCM data for the nexthop
* @param    egrIntf  @{(output)} Egress Id of the nexthop
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_egress_destroy(USL_DB_TYPE_t dbType,
                             usl_bcm_l3_egress_t *pBcmInfo,
                             bcm_if_t egrIntf)
{

  int                      rv = BCM_E_NONE;
  usl_egr_nhop_db_elem_t  data;
  usl_egr_nhop_db_elem_t *pData;
  avlTree_t              *nhopTree;

  if (uslIpEgrNhopDbActive == L7_FALSE)
  {
    return rv;
  }

  nhopTree = usl_l3_egr_nhop_db_handle_get(dbType);
  if (nhopTree == L7_NULLPTR)
  {
    return BCM_E_FAIL;
  }

  USL_L3_EGR_NHOP_DB_LOCK_TAKE();

  memset(&data, 0, sizeof(data));
  memcpy(&(data.egrInfo), pBcmInfo, sizeof(usl_bcm_l3_egress_t));
  data.egrId = egrIntf;

  pData = avlDeleteEntry(nhopTree, &data);

  if (pData == L7_NULLPTR)
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_L3_EGR_NHOP_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Create an egress Nhop at Usl Bcm layer from the given
*           db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_create_l3_egr_nhop_db_elem_bcm(void *item)
{
  L7_int32              rv;
  usl_bcm_l3_egress_t  *l3Egress = &(((usl_egr_nhop_db_elem_t *)item)->egrInfo);
  bcm_if_t              egrId;

  do
  {
    egrId = ((usl_egr_nhop_db_elem_t *)item)->egrId;
    rv = usl_bcm_l3_egress_create(BCM_L3_WITH_ID, l3Egress, &egrId);

  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Delete L3 Egr nhop at the Usl Bcm layer from the
*           given db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_l3_egr_nhop_db_elem_bcm(void *item)
{
  L7_int32               rv;
  usl_bcm_l3_egress_t  *l3Egress = &(((usl_egr_nhop_db_elem_t *)item)->egrInfo);
  bcm_if_t              egrId;

  do
  {
    egrId = ((usl_egr_nhop_db_elem_t *)item)->egrId;
    rv = usl_bcm_l3_egress_destroy(l3Egress, egrId);

  } while (0);

  return rv;
}


/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.

* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_l3_egr_nhop_db_elem_bcm(void *shadowDbItem, void *operDbItem)
{
  L7_int32                  rv = BCM_E_NONE, flags = 0;
  usl_egr_nhop_db_elem_t   *shadowElem = shadowDbItem;
  usl_egr_nhop_db_elem_t   *operElem = operDbItem;

  do
  {

    /* If the Oper and Shadow egress-Id's are not same then
    ** the element cannot be reconciled. Return failure
    */
    if (shadowElem->egrId != operElem->egrId)
    {
      rv = BCM_E_FAIL;
    }
    else
    {
      if (memcmp(&(shadowElem->egrInfo.bcm_data),
                 &(operElem->egrInfo.bcm_data),
                 sizeof(shadowElem->egrInfo.bcm_data)) != 0)
      {
        flags |= (BCM_L3_WITH_ID | BCM_L3_REPLACE);
        rv = usl_bcm_l3_egress_create(flags, &(shadowElem->egrInfo),
                                      &(shadowElem->egrId));

      }
    }
  } while (0);

  return rv;

}


static L7_RC_t usl_l3_egr_nhop_db_alloc(avlTree_t *nhopTree,
                                        avlTreeTables_t **nhopTreeHeap,
                                        usl_egr_nhop_db_elem_t **nhopDataHeap)
{

  L7_uint32 max = HAPI_BROAD_L3_NH_TBL_SIZE + 1;/* +1 for link local ipv6 route */
  L7_RC_t   rc = L7_SUCCESS;

  *nhopTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                              sizeof(avlTreeTables_t) * max);
  if (*nhopTreeHeap == L7_NULLPTR)
  {
    rc = L7_FAILURE;
    return rc;
  }


  *nhopDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                              sizeof(usl_egr_nhop_db_elem_t) * max);

  if (*nhopDataHeap == L7_NULLPTR)
  {
    rc = L7_FAILURE;
    return rc;
  }

  /* clear the memory before creating the tree */
  memset((void*)nhopTree , 0, sizeof(avlTree_t));
  memset((void*)*nhopTreeHeap,0,sizeof(avlTreeTables_t)  * max);
  memset((void*)*nhopDataHeap,0,sizeof(usl_egr_nhop_db_elem_t) * max);

  /* key is the egress object id */
  avlCreateAvlTree(nhopTree, *nhopTreeHeap, *nhopDataHeap,
                   max, sizeof(usl_egr_nhop_db_elem_t), 0x10,
                   sizeof(usl_bcm_egr_nhop_key_t));

  return rc;

}

/*********************************************************************
* @purpose  Create the Egress Nhop Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_egr_nhop_db_init()
{
  L7_RC_t            rc = L7_ERROR;
  uslDbSyncFuncs_t   l3EgrNhopDbFuncs;

  memset(&l3EgrNhopDbFuncs, 0, sizeof(l3EgrNhopDbFuncs));

  do
  {
    /* Allocate db only on Stackables */
    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {
      uslIpEgrNhopDbSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
      if (uslIpEgrNhopDbSema == L7_NULLPTR) break;


      if (usl_l3_egr_nhop_db_alloc(&uslOperIpEgrNhopTreeData,
                                   &uslOperIpEgrNhopTreeHeap,
                                   &uslOperIpEgrNhopDataHeap) != L7_SUCCESS)
      {
        break;
      }

      /* Allocate shadow db for NSF feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                                L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {
        if (usl_l3_egr_nhop_db_alloc(&uslShadowIpEgrNhopTreeData,
                                     &uslShadowIpEgrNhopTreeHeap,
                                     &uslShadowIpEgrNhopDataHeap) != L7_SUCCESS)
        {
          break;
        }
      }


      /* Register the sync routines */
      l3EgrNhopDbFuncs.get_size_of_db_elem =  usl_get_size_of_l3_egr_nhop_db_elem;
      l3EgrNhopDbFuncs.get_db_elem = usl_get_l3_egr_nhop_db_elem;
      l3EgrNhopDbFuncs.delete_elem_from_db =  usl_delete_l3_egr_nhop_db_elem;
      l3EgrNhopDbFuncs.print_db_elem = usl_print_l3_egr_nhop_db_elem;
      l3EgrNhopDbFuncs.create_usl_bcm = usl_create_l3_egr_nhop_db_elem_bcm;
      l3EgrNhopDbFuncs.delete_usl_bcm = usl_delete_l3_egr_nhop_db_elem_bcm;
      l3EgrNhopDbFuncs.update_usl_bcm = usl_update_l3_egr_nhop_db_elem_bcm;


      /* Not registering optional routines as the db element is deterministic
      ** i.e no pointer member elements
      */
      l3EgrNhopDbFuncs.alloc_db_elem = L7_NULLPTR;
      l3EgrNhopDbFuncs.copy_db_elem  = L7_NULLPTR;
      l3EgrNhopDbFuncs.free_db_elem  = L7_NULLPTR;
      l3EgrNhopDbFuncs.pack_db_elem  = L7_NULLPTR;
      l3EgrNhopDbFuncs.unpack_db_elem = L7_NULLPTR;

      if (usl_db_sync_func_table_register(USL_L3_EGR_NHOP_DB_ID,
                                          &l3EgrNhopDbFuncs) != L7_SUCCESS)
      {
        L7_LOG_ERROR(0);
      }



      /* Point the Db handle to Operational table */
      uslIpEgrNhopTreeHandle = &uslOperIpEgrNhopTreeData;

      uslIpEgrNhopDbActive = L7_TRUE;

    } /* End if stacking */

    rc = L7_SUCCESS;

  } while (0);

  return rc;
}
/*********************************************************************
* @purpose  Destroy the Egress Nhop Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_egr_nhop_db_fini()
{
  L7_uint32 max = HAPI_BROAD_L3_NH_TBL_SIZE + 1;

  uslIpEgrNhopDbActive = L7_FALSE;
  uslIpEgrNhopTreeHandle = L7_NULLPTR;

  if (uslIpEgrNhopDbSema != L7_NULLPTR)
  {
    osapiSemaDelete(uslIpEgrNhopDbSema);
    uslIpEgrNhopDbSema = L7_NULLPTR;
  }

  avlDeleteAvlTree(&uslOperIpEgrNhopTreeData);
  memset((void*)&uslOperIpEgrNhopTreeData,0,sizeof(avlTree_t));


  if (uslOperIpEgrNhopTreeHeap != L7_NULLPTR)
  {
    memset((void*)uslOperIpEgrNhopTreeHeap,0,sizeof(avlTreeTables_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, uslOperIpEgrNhopTreeHeap);
    uslOperIpEgrNhopTreeHeap = L7_NULLPTR;
  }

  if (uslOperIpEgrNhopDataHeap != L7_NULLPTR)
  {
    memset((void*)uslOperIpEgrNhopDataHeap,0,sizeof(usl_egr_nhop_db_elem_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, uslOperIpEgrNhopDataHeap);
    uslOperIpEgrNhopDataHeap = L7_NULLPTR;
  }

  avlDeleteAvlTree(&uslShadowIpEgrNhopTreeData);
  memset((void*)&uslShadowIpEgrNhopTreeData,0,sizeof(avlTree_t));


  if (uslShadowIpEgrNhopTreeHeap != L7_NULLPTR)
  {
    memset((void*)uslShadowIpEgrNhopTreeHeap,0,sizeof(avlTreeTables_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, uslShadowIpEgrNhopTreeHeap);
    uslShadowIpEgrNhopTreeHeap = L7_NULLPTR;
  }

  if (uslShadowIpEgrNhopDataHeap != L7_NULLPTR)
  {
    memset((void*)uslShadowIpEgrNhopDataHeap,0,sizeof(usl_egr_nhop_db_elem_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, uslShadowIpEgrNhopDataHeap);
    uslShadowIpEgrNhopDataHeap = L7_NULLPTR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Invalidate the Egress Nhop Db
*
* @param    Flags  {(input)} Type of db to be invalidated.
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_egr_nhop_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t rc = L7_ERROR;
  L7_uint32 max = HAPI_BROAD_L3_NH_TBL_SIZE + 1;

  if (uslIpEgrNhopDbActive == L7_FALSE)
  {
    return rc;
  }

  USL_L3_EGR_NHOP_DB_LOCK_TAKE();

  do
  {
    if (flags & USL_OPERATIONAL_DB)
    {
      if ((uslOperIpEgrNhopTreeHeap == L7_NULLPTR) || (uslOperIpEgrNhopDataHeap == L7_NULLPTR))
        break;

      /* Key is the subnet and mask */
      avlPurgeAvlTree(&uslOperIpEgrNhopTreeData, max);

    }

    if (flags & USL_SHADOW_DB)
    {
      if ((uslShadowIpEgrNhopTreeHeap == L7_NULLPTR) || (uslShadowIpEgrNhopDataHeap == L7_NULLPTR))
        break;

      /* Key is the subnet and mask */
      avlPurgeAvlTree(&uslShadowIpEgrNhopTreeData, max);

    }

    rc = L7_SUCCESS;

  } while (0);

  USL_L3_EGR_NHOP_DB_LOCK_GIVE();

  return rc;
}

/*********************************************************************
* @purpose  Set the Egr Nhop Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_l3_egr_nhop_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_L3_EGR_NHOP_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    uslIpEgrNhopTreeHandle = &uslOperIpEgrNhopTreeData;
  }
  else if (dbType == USL_SHADOW_DB)
  {
    uslIpEgrNhopTreeHandle = &uslShadowIpEgrNhopTreeData;
  }

  USL_L3_EGR_NHOP_DB_LOCK_GIVE();

  return;
}

void usl_l3_egr_nhop_debug_search(USL_DB_TYPE_t dbType,
                                  L7_uint32      ipAddr,
                                  L7_uint32      unit,
                                  L7_uint32      slot,
                                  L7_uint32      port)
{

  L7_uchar8                uslStr[128];
  usl_egr_nhop_db_elem_t   elem, nextElem;
  uslDbElemInfo_t          searchElem, elemInfo;
  avlTree_t               *dbHandle;

  dbHandle = usl_l3_egr_nhop_db_handle_get(dbType);
  if (dbHandle == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "%s is not present\n",
                   usl_db_type_name_get(dbType));
    return;
  }

  memset(&elem, 0, sizeof(elem));
  memset(&nextElem, 0, sizeof(nextElem));

  elem.egrInfo.key.usp.unit = unit;
  elem.egrInfo.key.usp.slot = slot;
  elem.egrInfo.key.usp.port = port;
  elem.egrInfo.key.addr.l3a_ip_addr = ipAddr;

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  if (usl_get_l3_egr_nhop_db_elem(dbType, USL_DB_EXACT_ELEM,
                                  searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_l3_egr_nhop_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  uslStr);
  }
  else
  {
    printf("not found\n");
  }

}

void usl_l3_egr_nhop_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8                uslStr[128];
  usl_egr_nhop_db_elem_t   elem, nextElem;
  uslDbElemInfo_t          searchElem, elemInfo;
  avlTree_t               *dbHandle;
  L7_uint32                entryCount = 0;

  if (uslIpEgrNhopDbActive == L7_FALSE)
  {

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L3 Egr Nhop database is not active\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L3 Egr Nhop database is active\n");
  }

  dbHandle = usl_l3_egr_nhop_db_handle_get(dbType);
  if (dbHandle != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Dumping %s \n",
                  usl_db_type_name_get(dbType));
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "%s is not present\n",
                   usl_db_type_name_get(dbType));
  }

  memset(&elem, 0, sizeof(elem));
  memset(&nextElem, 0, sizeof(nextElem));

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_l3_egr_nhop_db_elem(dbType, USL_DB_NEXT_ELEM,
                                    searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_l3_egr_nhop_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  uslStr);
    memcpy(searchElem.dbElem, elemInfo.dbElem, sizeof(usl_egr_nhop_db_elem_t));
    entryCount++;
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Total number of entries in the table: %d\n",
                entryCount);


  return;
}

/*********************************************************************
* @purpose  Given a dbType, return the db Handle.
*
* @params   dbType {(input)} OPER/SHADOW/CURRENT
*
* @returns  Pointer to db Handle
*
* @notes    none
*
* @end
*********************************************************************/
avlTree_t *usl_l3_mpath_egr_nhop_db_handle_get(USL_DB_TYPE_t dbType)
{
  avlTree_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = &uslOperIpMpathEgrNhopTreeData;
      break;

    case USL_SHADOW_DB:
      dbHandle = &uslShadowIpMpathEgrNhopTreeData;
      break;

    case USL_CURRENT_DB:
      dbHandle = uslIpMpathEgrNhopTreeHandle;
      break;

    default:
      break;
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Get the size of a egr nhop Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_l3_mpath_egr_nhop_db_elem(void)
{
  return sizeof(usl_mpath_egr_nhop_db_elem_t);
}

/*********************************************************************
* @purpose  Print the contents of a l3 egr nhop db element in
*           specified buffer
*
* @params   elem     {(input)}  Element to be printed
*           buffer   {(input)}  buffer pointer
*           size     {(input)}  size of buffer
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
void usl_print_l3_mpath_egr_nhop_db_elem(void *item, L7_uchar8 *buffer,
                                         L7_uint32 size)
{
  L7_uint32                     i;
  usl_mpath_egr_nhop_db_elem_t *elem = item;

  osapiSnprintf(buffer, size,
                "Mpath Egr Id = %d, NextHops: ", elem->avlKey);
  for (i=0; i < L7_RT_MAX_EQUAL_COST_ROUTES; i++)
  {
    osapiSnprintfcat(buffer, size,
                     "%d ", elem->intf[i]);
  }
  osapiSnprintfcat(buffer, size,
                   "\n");

  return;
}

/*********************************************************************
* @purpose  Search element in L3 Egr Nhop db and return pointer to it
*
* @params   dbType     {(input)} Db type (Oper or Shadow)
*           flags      {(input)} USL_EXACT_ELEM or USL_NEXT_ELEM
*           searchInfo {(input)} Element to be searched
*           elemInfo   {(output)} Found element info
*
* @returns  BCM_E_NONE: Element found
*           BCM_E_NOT_FOUND: Element not found
*
* @notes    Internal API. Db lock must be held. Returns pointer
*           to the element.
*
* @end
*********************************************************************/
static L7_int32 usl_search_l3_mpath_egr_nhop_db_elem(USL_DB_TYPE_t dbType,
                                                     L7_uint32 flags,
                                                     uslDbElemInfo_t searchInfo,
                                                     uslDbElemInfo_t *elemInfo)
{
  avlTree_t                 *dbHandle = L7_NULLPTR;
  L7_int32                   rv = BCM_E_NOT_FOUND;
  L7_uchar8                  traceStr[128];


  osapiSnprintf(traceStr, sizeof(traceStr),
                "Get Elem: Flags %d \n",
                flags);
  usl_trace_sync_detail(USL_L3_MPATH_EGR_NHOP_DB_ID, traceStr);


  elemInfo->dbElem = L7_NULLPTR;
  elemInfo->elemIndex = USL_INVALID_DB_INDEX;

  do
  {

    dbHandle = usl_l3_mpath_egr_nhop_db_handle_get(dbType);

    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Get next */
    if (flags == USL_DB_NEXT_ELEM)
    {
      elemInfo->dbElem = avlSearchLVL7(dbHandle, searchInfo.dbElem, AVL_NEXT);
    }
    else if (flags == USL_DB_EXACT_ELEM)
    {
      elemInfo->dbElem = avlSearchLVL7(dbHandle, searchInfo.dbElem, AVL_EXACT);
    }

    if (elemInfo->dbElem == L7_NULLPTR)
    {
      rv = BCM_E_NOT_FOUND;
    }
    else
    {
      rv = BCM_E_NONE;
    }

  } while (0);


  return rv;
}

/*********************************************************************
* @purpose  Get info about an element from the Db
*
* @params   dbType        {(input)}  dbType to be searched
*           flags         {(input)}  EXACT or GET_NEXT
*           searchInfo    {(input)}  Information about element to be searched
*           elemInfo      {(output)} Storage for element & index
*
* @returns  BCM error code
*
* @notes    Caller must allocate memory to copy the db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_l3_mpath_egr_nhop_db_elem(USL_DB_TYPE_t     dbType,
                                           L7_uint32         flags,
                                           uslDbElemInfo_t   searchElem,
                                           uslDbElemInfo_t  *elemInfo)
{
  L7_int32         rv = BCM_E_NONE;
  uslDbElemInfo_t  nhopInfo;

  if (!uslIpMpathEgrNhopDbActive)
  {
    return rv;
  }

  nhopInfo.dbElem = L7_NULLPTR;
  nhopInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_L3_EGR_NHOP_DB_LOCK_TAKE();

  rv = usl_search_l3_mpath_egr_nhop_db_elem(dbType, flags, searchElem, &nhopInfo);
  if (rv == BCM_E_NONE)
  {
    memcpy(elemInfo->dbElem, nhopInfo.dbElem,
           sizeof(usl_mpath_egr_nhop_db_elem_t));
  }

  USL_L3_EGR_NHOP_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Delete a given element from Egr Nhop Db
*
* @params   dbType   {(input)}
*           elemInfo {(input)}
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_l3_mpath_egr_nhop_db_elem(USL_DB_TYPE_t   dbType,
                                              uslDbElemInfo_t elemInfo)
{
  avlTree_t                       *dbHandle;
  usl_mpath_egr_nhop_db_elem_t    *elem;
  L7_int32                         rv = BCM_E_NONE;

  if (!uslIpMpathEgrNhopDbActive)
  {
    return rv;
  }

  USL_L3_EGR_NHOP_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_l3_mpath_egr_nhop_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Delete the elem */
    elem = avlDeleteEntry(dbHandle, elemInfo.dbElem);
    if (elem == L7_NULLPTR)
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);

  USL_L3_EGR_NHOP_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Create a Multipath Egress Nhop object in USL Db
*
* @param    dbType       @{(input)} dbType to be modified
* @param    mpintf       @{(input)} Multipath egress obj id
* @param    intf_count   @{(input)} Number of egress nhops
* @param    intf_array   @{(input)} Array of egress nhop Ids
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_egress_multipath_create(USL_DB_TYPE_t dbType, bcm_if_t mpintf,
                                      L7_int32 intf_count,
                                      bcm_if_t * intf_array)
{
  int                           rv = BCM_E_NONE;
  L7_uint32                     i;
  usl_mpath_egr_nhop_db_elem_t  data;
  usl_mpath_egr_nhop_db_elem_t *pData;
  avlTree_t                    *nhopTree;

  if (uslIpMpathEgrNhopDbActive == L7_FALSE)
  {
    return rv;
  }

  nhopTree = usl_l3_mpath_egr_nhop_db_handle_get(dbType);
  if (nhopTree == L7_NULLPTR)
  {
    return BCM_E_FAIL;
  }

  USL_L3_EGR_NHOP_DB_LOCK_TAKE();

  memset(&data, 0, sizeof(data));

  if (intf_count > L7_RT_MAX_EQUAL_COST_ROUTES)
    USL_LOG_ERROR("USL: Invalid number of ECMP next hops\n");

  data.avlKey = mpintf;

  for (i=0; i < intf_count; i++)
  {
    data.intf[i] = intf_array[i];
  }
  data.intfCount = intf_count;

  pData = avlInsertEntry(nhopTree, &data);

  if (pData == &data)
  {
    rv = BCM_E_FAIL;
    USL_LOG_ERROR("USL: Error adding multipath object entry \n");
  }
  else if (pData != L7_NULLPTR)
  {
    /* It is normal to update existing ECMP multipath objects */
    pData->intfCount = intf_count;
    for (i=0; i < L7_RT_MAX_EQUAL_COST_ROUTES; i++)
    {
      if (i < intf_count)
        pData->intf[i] = intf_array[i];
      else
        pData->intf[i] = 0;
    }
  }

  USL_L3_EGR_NHOP_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Destroy a Multipath Egress Nhop object in USL Db
*
* @param    dbType       @{(input)} dbType to be modified
* @param    mpintf       @{(input)} Multipath egress obj id
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_egress_multipath_destroy(USL_DB_TYPE_t dbType, bcm_if_t mpintf)
{
  int                           rv = BCM_E_NONE;
  usl_mpath_egr_nhop_db_elem_t  data;
  uslDbElemInfo_t               elemInfo;

  if (uslIpMpathEgrNhopDbActive == L7_FALSE)
  {
    return rv;
  }

  memset(&data, 0, sizeof(data));
  data.avlKey = mpintf;

  elemInfo.dbElem = &data;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  return usl_delete_l3_mpath_egr_nhop_db_elem(dbType, elemInfo);
}

/*********************************************************************
* @purpose  Create an egress Nhop at Usl Bcm layer from the given
*           db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_create_l3_mpath_egr_nhop_db_elem_bcm(void *item)
{
  L7_int32                       rv;
  usl_mpath_egr_nhop_db_elem_t  *elem = item;
  bcm_if_t                       egrId;

  do
  {
    egrId = elem->avlKey;
    rv = usl_bcm_l3_egress_multipath_create(BCM_L3_WITH_ID, elem->intfCount,
                                            elem->intf, &egrId);

  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Delete L3 Egr nhop at the Usl Bcm layer from the
*           given db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_l3_mpath_egr_nhop_db_elem_bcm(void *item)
{
  L7_int32               rv;
  bcm_if_t               egrId;

  do
  {
    egrId = ((usl_mpath_egr_nhop_db_elem_t *)item)->avlKey;
    rv = usl_bcm_l3_egress_multipath_destroy(egrId);

  } while (0);

  return rv;
}


/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.

* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_l3_mpath_egr_nhop_db_elem_bcm(void *shadowDbItem, void *operDbItem)
{
  L7_int32                        rv = BCM_E_NONE;

  /* There shouldn't be any updates needed for Mpath egr nhop as if member nhops
  ** are different then it would be an entirely new Mpath next hop
  */

  return rv;

}


static L7_RC_t usl_l3_mpath_egr_nhop_db_alloc(avlTree_t *nhopTree,
                                              avlTreeTables_t **nhopTreeHeap,
                                              usl_mpath_egr_nhop_db_elem_t **nhopDataHeap)
{

  L7_uint32   max = HAPI_BROAD_L3_MAX_ECMP_GROUPS;
  L7_RC_t     rc = L7_SUCCESS;

  *nhopTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                              sizeof(avlTreeTables_t) * max);
  if (*nhopTreeHeap == L7_NULLPTR)
  {
    rc = L7_FAILURE;
    return rc;
  }


  *nhopDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                              sizeof(usl_mpath_egr_nhop_db_elem_t) * max);

  if (*nhopDataHeap == L7_NULLPTR)
  {
    rc = L7_FAILURE;
    return rc;
  }

  /* clear the memory before creating the tree */
  memset((void*)nhopTree , 0, sizeof(avlTree_t));
  memset((void*)*nhopTreeHeap,0,sizeof(avlTreeTables_t)  * max);
  memset((void*)*nhopDataHeap,0,sizeof(usl_mpath_egr_nhop_db_elem_t) * max);

  /* key is the egress object id */
  avlCreateAvlTree(nhopTree, *nhopTreeHeap, *nhopDataHeap,
                   max, sizeof(usl_mpath_egr_nhop_db_elem_t), 0x10,
                   sizeof(bcm_if_t));

  return rc;

}

/*********************************************************************
* @purpose  Create the Multipath Egress Nhop Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_mpath_egr_nhop_db_init()
{
  L7_RC_t            rc = L7_ERROR;
  uslDbSyncFuncs_t   l3MpathEgrNhopDbFuncs;

  memset(&l3MpathEgrNhopDbFuncs, 0, sizeof(l3MpathEgrNhopDbFuncs));


  do
  {
    /* Allocate db only on Stackables */
    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {

      if (usl_l3_mpath_egr_nhop_db_alloc(&uslOperIpMpathEgrNhopTreeData,
                                         &uslOperIpMpathEgrNhopTreeHeap,
                                         &uslOperIpMpathEgrNhopDataHeap) != L7_SUCCESS)
      {
        break;
      }

      /* Allocate shadow db for NSF feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {
        if (usl_l3_mpath_egr_nhop_db_alloc(&uslShadowIpMpathEgrNhopTreeData,
                                           &uslShadowIpMpathEgrNhopTreeHeap,
                                           &uslShadowIpMpathEgrNhopDataHeap) != L7_SUCCESS)
        {
          break;
        }

      }

      /* Register the sync routines */
      l3MpathEgrNhopDbFuncs.get_size_of_db_elem =  usl_get_size_of_l3_mpath_egr_nhop_db_elem;
      l3MpathEgrNhopDbFuncs.get_db_elem = usl_get_l3_mpath_egr_nhop_db_elem;
      l3MpathEgrNhopDbFuncs.delete_elem_from_db =  usl_delete_l3_mpath_egr_nhop_db_elem;
      l3MpathEgrNhopDbFuncs.print_db_elem = usl_print_l3_mpath_egr_nhop_db_elem;
      l3MpathEgrNhopDbFuncs.create_usl_bcm = usl_create_l3_mpath_egr_nhop_db_elem_bcm;
      l3MpathEgrNhopDbFuncs.delete_usl_bcm = usl_delete_l3_mpath_egr_nhop_db_elem_bcm;
      l3MpathEgrNhopDbFuncs.update_usl_bcm = usl_update_l3_mpath_egr_nhop_db_elem_bcm;


      /* Not registering optional routines as the db element is deterministic
      ** i.e no pointer member elements
      */
      l3MpathEgrNhopDbFuncs.alloc_db_elem = L7_NULLPTR;
      l3MpathEgrNhopDbFuncs.copy_db_elem  = L7_NULLPTR;
      l3MpathEgrNhopDbFuncs.free_db_elem  = L7_NULLPTR;
      l3MpathEgrNhopDbFuncs.pack_db_elem  = L7_NULLPTR;
      l3MpathEgrNhopDbFuncs.unpack_db_elem = L7_NULLPTR;

      if (usl_db_sync_func_table_register(USL_L3_MPATH_EGR_NHOP_DB_ID,
                                          &l3MpathEgrNhopDbFuncs) != L7_SUCCESS)
      {
        L7_LOG_ERROR(0);
      }


      /* Point the Db handle to Operational table */
      uslIpMpathEgrNhopTreeHandle = &uslOperIpMpathEgrNhopTreeData;

      uslIpMpathEgrNhopDbActive = L7_TRUE;

    } /* End if stacking */

    rc = L7_SUCCESS;

  } while (0);

  return rc;
}


/*********************************************************************
* @purpose  Destroy the Multipath Egress Nhop Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_mpath_egr_nhop_db_fini()
{
  L7_uint32 max = HAPI_BROAD_L3_MAX_ECMP_GROUPS;

  uslIpMpathEgrNhopDbActive = L7_FALSE;
  uslIpMpathEgrNhopTreeHandle = L7_NULLPTR;

  avlDeleteAvlTree(&uslOperIpMpathEgrNhopTreeData);
  memset((void*)&uslOperIpMpathEgrNhopTreeData,0,sizeof(avlTree_t));

  if (uslOperIpMpathEgrNhopTreeHeap != L7_NULLPTR)
  {
    memset((void*)uslOperIpMpathEgrNhopTreeHeap,0,sizeof(avlTreeTables_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, uslOperIpMpathEgrNhopTreeHeap);
    uslOperIpMpathEgrNhopTreeHeap = L7_NULLPTR;
  }

  if (uslOperIpMpathEgrNhopDataHeap != L7_NULLPTR)
  {
    memset((void*)uslOperIpMpathEgrNhopDataHeap,0,sizeof(usl_mpath_egr_nhop_db_elem_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, uslOperIpMpathEgrNhopDataHeap);
    uslOperIpMpathEgrNhopDataHeap = L7_NULLPTR;
  }


  avlDeleteAvlTree(&uslShadowIpMpathEgrNhopTreeData);
  memset((void*)&uslShadowIpMpathEgrNhopTreeData,0,sizeof(avlTree_t));

  if (uslShadowIpMpathEgrNhopTreeHeap != L7_NULLPTR)
  {
    memset((void*)uslShadowIpMpathEgrNhopTreeHeap,0,sizeof(avlTreeTables_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, uslShadowIpMpathEgrNhopTreeHeap);
    uslShadowIpMpathEgrNhopTreeHeap = L7_NULLPTR;
  }

  if (uslShadowIpMpathEgrNhopDataHeap != L7_NULLPTR)
  {
    memset((void*)uslShadowIpMpathEgrNhopDataHeap,0,sizeof(usl_mpath_egr_nhop_db_elem_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, uslShadowIpMpathEgrNhopDataHeap);
    uslShadowIpMpathEgrNhopDataHeap = L7_NULLPTR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Invalidate the Mpath Egress Nhop Db
*
* @param    Flags  {(input)} Type of db to be invalidated.
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_mpath_egr_nhop_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t rc = L7_ERROR;
  L7_uint32 max = HAPI_BROAD_L3_MAX_ECMP_GROUPS;

  if (uslIpMpathEgrNhopDbActive == L7_FALSE)
  {
    return rc;
  }

  USL_L3_EGR_NHOP_DB_LOCK_TAKE();

  do
  {
    if (flags & USL_OPERATIONAL_DB)
    {
      if ((uslOperIpMpathEgrNhopTreeHeap == L7_NULLPTR) || (uslOperIpMpathEgrNhopDataHeap == L7_NULLPTR))
        break;

      /* Key is the subnet and mask */
      avlPurgeAvlTree(&uslOperIpMpathEgrNhopTreeData, max);

    }

    if (flags & USL_SHADOW_DB)
    {
      if ((uslShadowIpMpathEgrNhopTreeHeap == L7_NULLPTR) || (uslShadowIpMpathEgrNhopDataHeap == L7_NULLPTR))
        break;

      /* Key is the subnet and mask */
      avlPurgeAvlTree(&uslShadowIpMpathEgrNhopTreeData, max);

    }

    rc = L7_SUCCESS;

  } while (0);


  USL_L3_EGR_NHOP_DB_LOCK_GIVE();

  return rc;
}

/*********************************************************************
* @purpose  Set the Mapth Egr Nhop Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_l3_mpath_egr_nhop_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_L3_EGR_NHOP_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    uslIpMpathEgrNhopTreeHandle = &uslOperIpMpathEgrNhopTreeData;
  }
  else if (dbType == USL_SHADOW_DB)
  {
    uslIpMpathEgrNhopTreeHandle = &uslShadowIpMpathEgrNhopTreeData;
  }

  USL_L3_EGR_NHOP_DB_LOCK_GIVE();

  return;
}

void usl_l3_mpath_egr_nhop_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8                      uslStr[128];
  usl_mpath_egr_nhop_db_elem_t   elem, nextElem;
  uslDbElemInfo_t                searchElem, elemInfo;
  avlTree_t                     *dbHandle;
  L7_uint32                      entryCount = 0;

  if (uslIpMpathEgrNhopDbActive == L7_FALSE)
  {

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L3 Mpath Egr Nhop database is not active\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L3 Mpath Egr Nhop database is active\n");
  }

  dbHandle = usl_l3_mpath_egr_nhop_db_handle_get(dbType);
  if (dbHandle != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Dumping %s \n",
                  usl_db_type_name_get(dbType));
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "%s is not present\n",
                   usl_db_type_name_get(dbType));
  }

  memset(&elem, 0, sizeof(elem));
  memset(&nextElem, 0, sizeof(nextElem));

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_l3_mpath_egr_nhop_db_elem(dbType, USL_DB_NEXT_ELEM,
                                           searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_l3_mpath_egr_nhop_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  uslStr);
    memcpy(searchElem.dbElem, elemInfo.dbElem, sizeof(usl_mpath_egr_nhop_db_elem_t));
    entryCount++;
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Total number of entries in the table: %d\n",
                entryCount);
  return;
}

/*********************************************************************
* @purpose  Return info for a given Mpath Egress Nhop fron the USL Db
*
* @params   dbType      @{(input)} DbType to be searched
* @params   intf_count  @{(input)} Number of intfs
* @param    Egress info @{(input)} Egress info
* @param    mpathNhop   @{(output)} Mpath Egress Nhop Data
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_egress_multipath_get(USL_DB_TYPE_t dbType,
                                   L7_uint32     intf_count,
                                   bcm_if_t *egressInfo,
                                   usl_mpath_egr_nhop_db_elem_t *mpathNhop)
{
  int                           rv = BCM_E_NOT_FOUND, i, j;
  bcm_if_t                      nhopId;
  L7_BOOL                       mpathNhopFound, nhopIdFound;
  usl_mpath_egr_nhop_db_elem_t  data;
  usl_mpath_egr_nhop_db_elem_t *pData;
  avlTree_t                    *mpathNhopTree;

  if (uslIpMpathEgrNhopDbActive == L7_FALSE)
  {
    return rv;
  }

  mpathNhopTree = usl_l3_mpath_egr_nhop_db_handle_get(dbType);
  if (mpathNhopTree == L7_NULLPTR)
  {
    return BCM_E_FAIL;
  }

  USL_L3_EGR_NHOP_DB_LOCK_TAKE();


  memset(&data, 0, sizeof(data));

  while ((pData = avlSearchLVL7(mpathNhopTree, &data, AVL_NEXT)) != L7_NULLPTR)
  {
    memcpy(&data, pData, sizeof(data));

    if (pData->intfCount != intf_count)
    {
      continue;
    }

    mpathNhopFound = L7_TRUE;

    /* Search for each of the nhop-ids in the found element */
    for (i = 0; i < intf_count; i++)
    {
      nhopIdFound = L7_FALSE;
      nhopId = egressInfo[i];

      for (j = 0; j < intf_count; j++)
      {
        if (pData->intf[j] == nhopId)
        {
          nhopIdFound = L7_TRUE;
          break;
        }
      }

      if (nhopIdFound == L7_FALSE)
      {
        mpathNhopFound = FALSE;
        break;
      }

    }

    if (mpathNhopFound == L7_TRUE)
    {
      /* Found matching mapth egress nhop */
      memcpy(mpathNhop, pData, sizeof(*mpathNhop));
      rv = BCM_E_NONE;
      break;
    }
  }

  USL_L3_EGR_NHOP_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Given a dbType, return the db Handle.
*
* @params   dbType {(input)} OPER/SHADOW/CURRENT
*
* @returns  Pointer to db Handle
*
* @notes    none
*
* @end
*********************************************************************/
usl_l3_intf_db_elem_t *usl_l3_intf_db_handle_get(USL_DB_TYPE_t dbType)
{
  usl_l3_intf_db_elem_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = pUslOperL3IntfDb;
      break;

    case USL_SHADOW_DB:
      dbHandle = pUslShadowL3IntfDb;
      break;

    case USL_CURRENT_DB:
      dbHandle = pUslL3IntfDbHandle;
      break;

    default:
      break;
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Get the size of a L3 Intf Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_l3_intf_db_elem(void)
{
  return sizeof(usl_l3_intf_db_elem_t);
}

/*********************************************************************
* @purpose  Print the contents of a l3 intf db element in
*           specified buffer
*
* @params   elem     {(input)}  Element to be printed
*           buffer   {(input)}  buffer pointer
*           size     {(input)}  size of buffer
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
void usl_print_l3_intf_db_elem(void *item, L7_uchar8 *buffer,
                               L7_uint32 size)
{
  usl_l3_intf_db_elem_t  *elem = item;

  osapiSnprintf(buffer, size,
                "L3 Intf= %d, "
                "MAC= %0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x, VID= %d\n",
                elem->intfInfo.bcm_data.l3a_intf_id,
                elem->intfInfo.bcm_data.l3a_mac_addr[0],
                elem->intfInfo.bcm_data.l3a_mac_addr[1],
                elem->intfInfo.bcm_data.l3a_mac_addr[2],
                elem->intfInfo.bcm_data.l3a_mac_addr[3],
                elem->intfInfo.bcm_data.l3a_mac_addr[4],
                elem->intfInfo.bcm_data.l3a_mac_addr[5],
                elem->intfInfo.bcm_data.l3a_vid);

  osapiSnprintfcat(buffer, size,
                   "Tunnel info: Sip %x Dip %x type %d\n",
                   elem->intfInfo.tunnelInfo.sip,
                   elem->intfInfo.tunnelInfo.dip,
                   elem->intfInfo.tunnelInfo.type);

  return;
}

/*********************************************************************
* @purpose  Search element in l3 intf db and return pointer to it
*
* @params   dbType     {(input)} Db type (Oper or Shadow)
*           flags      {(input)} USL_EXACT_ELEM or USL_NEXT_ELEM
*           searchInfo {(input)} Element to be searched
*           elemInfo   {(output)} Found element info
*
* @returns  BCM_E_NONE: Element found
*           BCM_E_NOT_FOUND: Element not found
*
* @notes    Internal API. Db lock must be held. Returns pointer
*           to the element.
*
* @end
*********************************************************************/
static L7_int32 usl_search_l3_intf_db_elem(USL_DB_TYPE_t dbType,
                                           L7_uint32 flags,
                                           uslDbElemInfo_t searchInfo,
                                           uslDbElemInfo_t *elemInfo)
{
  L7_uchar8                  traceStr[128];
  L7_int32                   rv = BCM_E_NOT_FOUND;
  L7_uint32                  max = USL_L3_INTF_DB_LEN, idx;
  L7_BOOL                    found;
  usl_l3_intf_db_elem_t     *dbHandle = L7_NULLPTR;
  usl_l3_intf_db_elem_t     *searchElem = searchInfo.dbElem;


  osapiSnprintf(traceStr, sizeof(traceStr),
                "Get Elem: Flags %d \n",
                flags);
  usl_trace_sync_detail(USL_L3_INTF_DB_ID, traceStr);


  elemInfo->dbElem = L7_NULLPTR;
  elemInfo->elemIndex = USL_INVALID_DB_INDEX;

  do
  {

    dbHandle = usl_l3_intf_db_handle_get(dbType);

    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    found = L7_FALSE;

    /* Get next */
    if (flags == USL_DB_NEXT_ELEM)
    {
      /* Start from beginning if the last index is not valid */
      if (searchInfo.elemIndex == USL_INVALID_DB_INDEX)
      {
        idx = 0;
      }
      else
      {
        idx = searchInfo.elemIndex + 1;
      }

      while ((idx < max) && (dbHandle[idx].isValid == L7_FALSE))
        idx++;

      if (idx < max)
      {
        elemInfo->dbElem = &(dbHandle[idx]);
        elemInfo->elemIndex = idx;
        found = L7_TRUE;
      }

    }
    else if (flags == USL_DB_EXACT_ELEM)
    {
      for (idx=0; idx < max; idx++)
      {
        if (dbHandle[idx].isValid == L7_FALSE)
          continue;

        /* The L3 intf mac/vid/tunnel-info must match */
        if ((dbHandle[idx].intfInfo.bcm_data.l3a_vid == searchElem->intfInfo.bcm_data.l3a_vid) &&
            (memcmp(dbHandle[idx].intfInfo.bcm_data.l3a_mac_addr,
                    searchElem->intfInfo.bcm_data.l3a_mac_addr,
                    sizeof(bcm_mac_t)) == 0) &&
            (memcmp(&(dbHandle[idx].intfInfo.tunnelInfo),
                    &(searchElem->intfInfo.tunnelInfo),
                    sizeof(dbHandle[idx].intfInfo.tunnelInfo)) == 0))
        {
          /* Found the interface */
          elemInfo->dbElem = &(dbHandle[idx]);
          elemInfo->elemIndex = idx;
          found = L7_TRUE;
          break;
        }
      }

    }

    if (found == L7_FALSE)
    {
      rv = BCM_E_NOT_FOUND;
    }
    else
    {
      rv = BCM_E_NONE;
    }

  } while (0);


  return rv;
}

/*********************************************************************
* @purpose  Get info about an element from the Db
*
* @params   dbType        {(input)}  dbType to be searched
*           flags         {(input)}  EXACT or GET_NEXT
*           searchInfo    {(input)}  Information about element to be searched
*           elemInfo      {(output)} Storage for element & index
*
* @returns  BCM error code
*
* @notes    Caller must allocate memory to copy the db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_l3_intf_db_elem(USL_DB_TYPE_t     dbType,
                                 L7_uint32         flags,
                                 uslDbElemInfo_t   searchElem,
                                 uslDbElemInfo_t  *elemInfo)
{
  L7_int32         rv = BCM_E_NONE;
  uslDbElemInfo_t  l3IntfInfo;

  if (uslL3IntfDbActive == L7_FALSE)
  {
    return rv;
  }

  l3IntfInfo.dbElem = L7_NULLPTR;
  l3IntfInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_L3_INTF_DB_LOCK_TAKE();

  rv = usl_search_l3_intf_db_elem(dbType, flags, searchElem, &l3IntfInfo);
  if (rv == BCM_E_NONE)
  {
    memcpy(elemInfo->dbElem, l3IntfInfo.dbElem, sizeof(usl_l3_intf_db_elem_t));
    memcpy(&(elemInfo->elemIndex) , &(l3IntfInfo.elemIndex),
          sizeof(elemInfo->elemIndex));
  }

  USL_L3_INTF_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Delete a given element from L3 Intf Db
*
* @params   dbType   {(input)}
*           elemInfo {(input)}
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_l3_intf_db_elem(USL_DB_TYPE_t   dbType,
                                    uslDbElemInfo_t elemInfo)
{
  L7_int32                    rv = BCM_E_NONE;
  L7_uint32                   max = USL_L3_INTF_DB_LEN;
  usl_l3_intf_db_elem_t      *dbHandle;
  uslDbElemInfo_t             l3Elem;

  if (uslL3IntfDbActive == L7_FALSE)
  {
    return rv;
  }

  l3Elem.dbElem = L7_NULLPTR;
  l3Elem.elemIndex = USL_INVALID_DB_INDEX;

  USL_L3_INTF_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_l3_intf_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* If the elemIndex is specified, then use that to delete the element */
    if ((elemInfo.elemIndex != USL_INVALID_DB_INDEX) &&
        (elemInfo.elemIndex < max))
    {
        memset(&(dbHandle[elemInfo.elemIndex]), 0, sizeof(usl_l3_intf_db_elem_t));
    }
    else
    {

      if (elemInfo.dbElem == L7_NULLPTR)
      {
        rv = BCM_E_FAIL;
        break;
      }

      /* Search and delete the elem */
      rv = usl_search_l3_intf_db_elem(dbType, USL_DB_EXACT_ELEM,
                                      elemInfo, &l3Elem);
      if (rv == BCM_E_NONE)
      {
        memset(l3Elem.dbElem, 0, sizeof(usl_l3_intf_db_elem_t));
      }
      else
      {
        /* Element not found */
        rv = BCM_E_NOT_FOUND;
      }
    }

  } while (0);

  USL_L3_INTF_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Add an L3 interface in USL Db
*
* @param    dbType {(input)} db type to be modified
* @param    info   {(input)} L3 interface info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_intf_create(USL_DB_TYPE_t dbType,
                          usl_bcm_l3_intf_t *info)

{
  L7_uint32                max = USL_L3_INTF_DB_LEN;
  L7_uint32                index_to_use;
  L7_uint32                id_idx;
  int                      rv = BCM_E_NONE;
  usl_l3_intf_db_elem_t   *dbHandle;

  if (uslL3IntfDbActive == L7_FALSE)
  {
    return rv;
  }

  dbHandle = usl_l3_intf_db_handle_get(dbType);
  if (dbHandle == L7_NULLPTR)
  {
    return BCM_E_FAIL;
  }

  USL_L3_INTF_DB_LOCK_TAKE();

  /* preset the index to an invalid value */
  index_to_use = max;

  /* try to find a valid index and look for error conditions */
  for (id_idx=0;id_idx < max;id_idx++)
  {
    if (dbHandle[id_idx].isValid == L7_FALSE)
      index_to_use = id_idx;

    if ((dbHandle[id_idx].intfInfo.bcm_data.l3a_intf_id == info->bcm_data.l3a_intf_id) &&
        (dbHandle[id_idx].isValid == L7_TRUE))
    {

#if 0
      if ((info->bcm_data.l3a_flags & BCM_L3_REPLACE) == L7_FALSE)
      {
        rv = BCM_E_EXISTS;

        /* Shouldn't get here, it probably means that we are out of sync with BCM */
        USL_LOG_MSG(USL_BCM_E_LOG,
                    "USL: L3 interface Id being created was already assigned (%d)\n",
                    info->bcm_data.l3a_intf_id);
      }
      else
#endif
      {
        index_to_use = id_idx;
      }

      break;
    }
  }

  /* update the table */
  if ((index_to_use >= 0) && (index_to_use < max))
  {
    dbHandle[index_to_use].intfInfo = *info;

    /* No need to save the BCM_L3_WITH_ID/BCM_L3_REPLACE flags as they
    ** are operational commands
    */
    dbHandle[index_to_use].intfInfo.bcm_data.l3a_flags &= ~(BCM_L3_WITH_ID | BCM_L3_REPLACE);

    dbHandle[index_to_use].isValid    = L7_TRUE;
  }
  else
  {
    rv = BCM_E_FULL; /* error condition, just return */
  }

  USL_L3_INTF_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete an L3 interface from USL Db
*
* @param    dbType {(input)} db type to be modified
* @param    info   {(input)} L3 interface info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_intf_delete(USL_DB_TYPE_t dbType,
                          usl_bcm_l3_intf_t *info)
{
  L7_uint32                max = USL_L3_INTF_DB_LEN;
  L7_uint32                id_idx;
  int                      rv = BCM_E_NONE;
  usl_l3_intf_db_elem_t   *dbHandle;

  if (uslL3IntfDbActive == L7_FALSE)
  {
    return rv;
  }

  dbHandle = usl_l3_intf_db_handle_get(dbType);
  if (dbHandle == L7_NULLPTR)
  {
    return BCM_E_FAIL;
  }

  USL_L3_INTF_DB_LOCK_TAKE();

  for (id_idx=0; id_idx < max; id_idx++)
  {
    if ((dbHandle[id_idx].isValid == L7_TRUE) &&
        (dbHandle[id_idx].intfInfo.bcm_data.l3a_intf_id == info->bcm_data.l3a_intf_id))
    {
      /* found the entry to delete */
      break;
    }
  }

  if ((id_idx >= 0) && (id_idx < max))
  {
    memset((void *)&dbHandle[id_idx],0,sizeof(usl_l3_intf_db_elem_t));
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_L3_INTF_DB_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Get info for a L3 intf
*
* @param    info          @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @notes    L3-id is used to search for the matching intf
*
* @end
*********************************************************************/
int usl_db_l3_intf_get(USL_DB_TYPE_t dbType, usl_bcm_l3_intf_t *info)
{
  int                      rv = BCM_E_NONE;
  L7_uint32                max = USL_L3_INTF_DB_LEN;
  L7_uint32                id_idx;
  usl_l3_intf_db_elem_t   *dbHandle;

  if (uslL3IntfDbActive == L7_FALSE)
  {
    return rv;
  }

  dbHandle = usl_l3_intf_db_handle_get(dbType);
  if (dbHandle == L7_NULLPTR)
  {
    return BCM_E_FAIL;
  }

  USL_L3_INTF_DB_LOCK_TAKE();

  for (id_idx=0; id_idx < max; id_idx++)
  {
    if ((dbHandle[id_idx].isValid == L7_TRUE) &&
        (dbHandle[id_idx].intfInfo.bcm_data.l3a_intf_id == info->bcm_data.l3a_intf_id))
    {
      /* found the entry to delete */
      break;
    }
  }

  if ((id_idx >= 0) && (id_idx < max))
  {
    memcpy(info, &(dbHandle[id_idx].intfInfo), sizeof(*info));
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_L3_INTF_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Create a L3 intf at Usl Bcm layer from the given
*           db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_create_l3_intf_db_elem_bcm(void *item)
{
  L7_int32                 rv;
  usl_l3_intf_db_elem_t   *elem = item;
  usl_bcm_l3_intf_t        l3Intf;

  memset(&l3Intf, 0, sizeof(l3Intf));

  do
  {
    memcpy(&l3Intf, &(elem->intfInfo), sizeof(l3Intf));
    l3Intf.bcm_data.l3a_flags |= BCM_L3_WITH_ID;

    rv = usl_bcm_l3_intf_create(&l3Intf);

  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Delete l3 intf at the Usl Bcm layer from the
*           given db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_l3_intf_db_elem_bcm(void *item)
{
  L7_int32                 rv;
  usl_l3_intf_db_elem_t   *elem = item;

  do
  {
    rv = usl_bcm_l3_intf_delete(&(elem->intfInfo));

  } while (0);

  return rv;
}


/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.

* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_l3_intf_db_elem_bcm(void *shadowDbItem, void *operDbItem)
{
  L7_int32                   rv = BCM_E_NONE;
  usl_l3_intf_db_elem_t    *shadowElem = shadowDbItem;
  usl_l3_intf_db_elem_t    *operElem = operDbItem;
  usl_bcm_l3_intf_t         l3Intf;

  memset(&l3Intf, 0, sizeof(l3Intf));

  if (memcmp(&(shadowElem->intfInfo), &(operElem->intfInfo),
             sizeof(shadowElem->intfInfo)) != 0)
  {
    memcpy(&l3Intf, &(shadowElem->intfInfo), sizeof(shadowElem->intfInfo));
    l3Intf.bcm_data.l3a_flags |= (BCM_L3_REPLACE | BCM_L3_WITH_ID);

    rv = usl_bcm_l3_intf_create(&l3Intf);
  }

  return rv;

}


static L7_RC_t usl_l3_intf_db_alloc(usl_l3_intf_db_elem_t **intfDb)
{
  L7_uint32 max;
  L7_RC_t   rc = L7_SUCCESS;

  max = USL_L3_INTF_DB_LEN;

  *intfDb  = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(usl_l3_intf_db_elem_t) * max);
  if (*intfDb == L7_NULLPTR)
  {
    rc = L7_FAILURE;
    return rc;
  }

  memset((void *)*intfDb, 0, sizeof(usl_l3_intf_db_elem_t) * max);

  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  Create the L3 Interface Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_intf_db_init()
{
  L7_RC_t            rc = L7_ERROR;
  uslDbSyncFuncs_t   l3IntfDbFuncs;

  memset(&l3IntfDbFuncs, 0, sizeof(l3IntfDbFuncs));

  do
  {
    /* Allocate db only on Stackables */
    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {

      pUslL3IntfDbSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
      if ( pUslL3IntfDbSema == L7_NULLPTR ) break;

      if (usl_l3_intf_db_alloc(&pUslOperL3IntfDb) != L7_SUCCESS)
      {
        break;
      }

      /* Allocate shadow db for NSF feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {
        if (usl_l3_intf_db_alloc(&pUslShadowL3IntfDb) != L7_SUCCESS)
        {
          break;
        }
      }

      /* Register the sync routines */
      l3IntfDbFuncs.get_size_of_db_elem =  usl_get_size_of_l3_intf_db_elem;
      l3IntfDbFuncs.get_db_elem = usl_get_l3_intf_db_elem;
      l3IntfDbFuncs.delete_elem_from_db =  usl_delete_l3_intf_db_elem;
      l3IntfDbFuncs.print_db_elem = usl_print_l3_intf_db_elem;
      l3IntfDbFuncs.create_usl_bcm = usl_create_l3_intf_db_elem_bcm;
      l3IntfDbFuncs.delete_usl_bcm = usl_delete_l3_intf_db_elem_bcm;
      l3IntfDbFuncs.update_usl_bcm = usl_update_l3_intf_db_elem_bcm;


      /* Not registering optional routines as the db element is deterministic
      ** i.e no pointer member elements
      */
      l3IntfDbFuncs.alloc_db_elem = L7_NULLPTR;
      l3IntfDbFuncs.copy_db_elem  = L7_NULLPTR;
      l3IntfDbFuncs.free_db_elem  = L7_NULLPTR;
      l3IntfDbFuncs.pack_db_elem  = L7_NULLPTR;
      l3IntfDbFuncs.unpack_db_elem = L7_NULLPTR;

      if (usl_db_sync_func_table_register(USL_L3_INTF_DB_ID,
                                          &l3IntfDbFuncs) != L7_SUCCESS)
      {
        L7_LOG_ERROR(0);
      }


      /* Point the Db handle to Operational table */
      pUslL3IntfDbHandle = pUslOperL3IntfDb;

      uslL3IntfDbActive = L7_TRUE;

    } /* End if stacking */

    rc = L7_SUCCESS;

  } while(0);

  return rc;
}

/*********************************************************************
* @purpose  Destroy the L3 Interface Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_intf_db_fini()
{
  L7_uint32 max = USL_L3_INTF_DB_LEN;

  uslL3IntfDbActive = L7_FALSE;
  pUslL3IntfDbHandle = L7_NULLPTR;

  if ( pUslL3IntfDbSema != L7_NULLPTR )
  {
    osapiSemaDelete(pUslL3IntfDbSema);
    pUslL3IntfDbSema = L7_NULLPTR;
  }

  if ( pUslOperL3IntfDb != L7_NULLPTR )
  {
    memset((void *)pUslOperL3IntfDb,0,sizeof(usl_l3_intf_db_elem_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperL3IntfDb);
    pUslOperL3IntfDb = L7_NULLPTR;
  }

  if ( pUslShadowL3IntfDb != L7_NULLPTR )
  {
    memset((void *)pUslShadowL3IntfDb,0,sizeof(usl_l3_intf_db_elem_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowL3IntfDb);
    pUslShadowL3IntfDb = L7_NULLPTR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Invalidate the L3 Intf Db
*
* @param    Flags  {(input)} Type of db to be invalidated.
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_intf_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t   rc = L7_ERROR;
  L7_uint32 max = USL_L3_INTF_DB_LEN;

  if (uslL3IntfDbActive == L7_FALSE)
  {
    return rc;
  }

  USL_L3_INTF_DB_LOCK_TAKE();

  do
  {
    if (flags & USL_OPERATIONAL_DB)
    {
      if (pUslOperL3IntfDb == L7_NULLPTR) break;
      memset((void *)pUslOperL3IntfDb,0,sizeof(usl_l3_intf_db_elem_t) * max);
    }

    if (flags & USL_SHADOW_DB)
    {
      if (pUslShadowL3IntfDb == L7_NULLPTR) break;

      memset((void *)pUslShadowL3IntfDb,0,sizeof(usl_l3_intf_db_elem_t) * max);
    }

    rc = L7_SUCCESS;

  } while(0);

  USL_L3_INTF_DB_LOCK_GIVE();

  return rc;
}

/*********************************************************************
* @purpose  Set the L3 Intf Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_l3_intf_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_L3_INTF_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    pUslL3IntfDbHandle = pUslOperL3IntfDb;
  }
  else if (dbType == USL_SHADOW_DB)
  {
    pUslL3IntfDbHandle = pUslShadowL3IntfDb;
  }

  USL_L3_INTF_DB_LOCK_GIVE();

  return;
}

void usl_l3_intf_print(L7_uint32 l3intf)
{
  L7_uint32 max,idx;
  usl_l3_intf_db_elem_t  *ptr;

    if (uslL3DbInited)
  {
    max = USL_L3_INTF_DB_LEN;

    for (idx = 0; idx < max; idx++)
    {
      ptr = &pUslL3IntfDbHandle[idx];

      if (ptr->isValid && (ptr->intfInfo.bcm_data.l3a_intf_id == l3intf))
      {
        sysapiPrintf("idx = %d, L3 Intf= %d, MAC= %0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x, VID= %d\n",
                     idx,
                     ptr->intfInfo.bcm_data.l3a_intf_id,
                     ptr->intfInfo.bcm_data.l3a_mac_addr[0],
                     ptr->intfInfo.bcm_data.l3a_mac_addr[1],
                     ptr->intfInfo.bcm_data.l3a_mac_addr[2],
                     ptr->intfInfo.bcm_data.l3a_mac_addr[3],
                     ptr->intfInfo.bcm_data.l3a_mac_addr[4],
                     ptr->intfInfo.bcm_data.l3a_mac_addr[5],
                     ptr->intfInfo.bcm_data.l3a_vid);
        return;
      }
    }

    if (idx >= max)
    {
      sysapiPrintf("L3 interface %d(0x%x) is not valid\n",l3intf,l3intf);
    }
  }


}

void usl_l3_intf_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8                 uslStr[128];
  usl_l3_intf_db_elem_t     elem, nextElem;
  usl_l3_intf_db_elem_t    *dbHandle;
  uslDbElemInfo_t           searchElem, elemInfo;
  L7_uint32                 entryCount = 0;

  if (uslL3IntfDbActive == L7_FALSE)
  {

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L3 Intf database is not active\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L3 Intf database is active\n");
  }

  dbHandle = usl_l3_intf_db_handle_get(dbType);
  if (dbHandle != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Dumping %s \n",
                  usl_db_type_name_get(dbType));
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "%s is not present\n",
                   usl_db_type_name_get(dbType));
    return;
  }

  memset(&elem, 0, sizeof(elem));
  memset(&nextElem, 0, sizeof(nextElem));

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_l3_intf_db_elem(dbType, USL_DB_NEXT_ELEM,
                                 searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_l3_intf_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  uslStr);
    memcpy(searchElem.dbElem, elemInfo.dbElem, sizeof(usl_l3_intf_db_elem_t));
    searchElem.elemIndex = elemInfo.elemIndex;
    entryCount++;
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Total number of entries in the table: %d\n",
                entryCount);


  return;
}

/*********************************************************************
* @purpose  Given a dbType, return the db Handle.
*
* @params   dbType {(input)} OPER/SHADOW/CURRENT
*
* @returns  Pointer to db Handle
*
* @notes    none
*
* @end
*********************************************************************/
avlTree_t *usl_l3_tunnel_terminator_db_handle_get(USL_DB_TYPE_t dbType)
{
  avlTree_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = &uslOperIpTunnelTerminatorTreeData;
      break;

    case USL_SHADOW_DB:
      dbHandle = &uslShadowIpTunnelTerminatorTreeData;
      break;

    case USL_CURRENT_DB:
      dbHandle = uslIpTunnelTerminatorTreeHandle;
      break;

    default:
      break;
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Get the size of a Tunnel Terminator Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_l3_tunnel_terminator_db_elem(void)
{
  return sizeof(usl_l3_tunnel_terminator_db_elem_t);
}

/*********************************************************************
* @purpose  Print the contents of a l3 tunnel terminator db element in
*           specified buffer
*
* @params   elem     {(input)}  Element to be printed
*           buffer   {(input)}  buffer pointer
*           size     {(input)}  size of buffer
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
void usl_print_l3_tunnel_terminator_db_elem(void *item, L7_uchar8 *buffer,
                                            L7_uint32 size)
{
  usl_l3_tunnel_terminator_db_elem_t  *elem = item;

  osapiSnprintf(buffer, size,
                "flags=%x, sip=%x:%x, dip=%x:%x\n",
                elem->bcm_data.flags,
                elem->bcm_data.sip,
                elem->bcm_data.sip_mask,
                elem->bcm_data.dip,
                elem->bcm_data.dip_mask);

  osapiSnprintfcat(buffer, size,
                   "    type=%u, udp_src=%u, udp_dst=%u\n",
                   elem->bcm_data.type,
                   elem->bcm_data.udp_src_port,
                   elem->bcm_data.udp_dst_port);

  return;
}

/*********************************************************************
* @purpose  Search element in l3 tunnel terminator db and return
*           pointer to it
*
* @params   dbType     {(input)} Db type (Oper or Shadow)
*           flags      {(input)} USL_EXACT_ELEM or USL_NEXT_ELEM
*           searchInfo {(input)} Element to be searched
*           elemInfo   {(output)} Found element info
*
* @returns  BCM_E_NONE: Element found
*           BCM_E_NOT_FOUND: Element not found
*
* @notes    Internal API. Db lock must be held. Returns pointer
*           to the element.
*
* @end
*********************************************************************/
static L7_int32 usl_search_l3_tunnel_terminator_db_elem(USL_DB_TYPE_t dbType,
                                                        L7_uint32 flags,
                                                        uslDbElemInfo_t searchInfo,
                                                        uslDbElemInfo_t *elemInfo)
{
  avlTree_t                 *dbHandle = L7_NULLPTR;
  L7_int32                   rv = BCM_E_NOT_FOUND;
  L7_uchar8                  traceStr[128];


  osapiSnprintf(traceStr, sizeof(traceStr),
                "Get Elem: Flags %d \n",
                flags);
  usl_trace_sync_detail(USL_L3_TUNNEL_TERMINATOR_DB_ID, traceStr);


  elemInfo->dbElem = L7_NULLPTR;
  elemInfo->elemIndex = USL_INVALID_DB_INDEX;

  do
  {

    dbHandle = usl_l3_tunnel_terminator_db_handle_get(dbType);

    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Get next */
    if (flags == USL_DB_NEXT_ELEM)
    {
      elemInfo->dbElem = avlSearchLVL7(dbHandle, searchInfo.dbElem, AVL_NEXT);
    }
    else if (flags == USL_DB_EXACT_ELEM)
    {
      elemInfo->dbElem = avlSearchLVL7(dbHandle, searchInfo.dbElem, AVL_EXACT);
    }

    if (elemInfo->dbElem == L7_NULLPTR)
    {
      rv = BCM_E_NOT_FOUND;
    }
    else
    {
      rv = BCM_E_NONE;
    }

  } while (0);


  return rv;
}

/*********************************************************************
* @purpose  Get info about an element from the Db
*
* @params   dbType        {(input)}  dbType to be searched
*           flags         {(input)}  EXACT or GET_NEXT
*           searchInfo    {(input)}  Information about element to be searched
*           elemInfo      {(output)} Storage for element & index
*
* @returns  BCM error code
*
* @notes    Caller must allocate memory to copy the db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_l3_tunnel_terminator_db_elem(USL_DB_TYPE_t     dbType,
                                              L7_uint32         flags,
                                              uslDbElemInfo_t   searchElem,
                                              uslDbElemInfo_t  *elemInfo)
{
  L7_int32         rv = BCM_E_NONE;
  uslDbElemInfo_t  terminatorInfo;

  if (!uslIpTunnelTerminatorDbActive)
  {
    return rv;
  }

  terminatorInfo.dbElem = L7_NULLPTR;
  terminatorInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_L3_TUNNEL_TERMINATOR_DB_LOCK_TAKE();

  rv = usl_search_l3_tunnel_terminator_db_elem(dbType, flags,
                                               searchElem, &terminatorInfo);
  if (rv == BCM_E_NONE)
  {
    memcpy(elemInfo->dbElem, terminatorInfo.dbElem,
           sizeof(usl_l3_tunnel_terminator_db_elem_t));
  }

  USL_L3_TUNNEL_TERMINATOR_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Delete a given element from L3 tunnel terminator Db
*
* @params   dbType   {(input)}
*           elemInfo {(input)}
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_l3_tunnel_terminator_db_elem(USL_DB_TYPE_t   dbType,
                                                 uslDbElemInfo_t elemInfo)
{
  avlTree_t                               *dbHandle;
  usl_l3_tunnel_terminator_db_elem_t      *elem;
  L7_int32                                 rv = BCM_E_NONE;

  if (!uslIpTunnelTerminatorDbActive)
  {
    return rv;
  }

  USL_L3_TUNNEL_TERMINATOR_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_l3_tunnel_terminator_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Delete the elem */
    elem = avlDeleteEntry(dbHandle, elemInfo.dbElem);
    if (elem == L7_NULLPTR)
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);

  USL_L3_TUNNEL_TERMINATOR_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Add a tunnel terminator in USL Db
*
* @param    dbType         @{(input)} dbType to be modified
* @param    terminator     @{(input)} The tunnel terminator info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_tunnel_terminator_add(USL_DB_TYPE_t dbType,
                                    bcm_tunnel_terminator_t *terminator)
{

  int                                 rv = BCM_E_NONE;
  usl_l3_tunnel_terminator_db_elem_t  data;
  usl_l3_tunnel_terminator_db_elem_t *pData;
  avlTree_t                          *tunnelTree;

  if (uslIpTunnelTerminatorDbActive == L7_FALSE)
  {
    return rv;
  }

  tunnelTree = usl_l3_tunnel_terminator_db_handle_get(dbType);
  if (tunnelTree == L7_NULLPTR)
  {
    return BCM_E_FAIL;
  }

  USL_L3_TUNNEL_TERMINATOR_DB_LOCK_TAKE();

  data.bcm_data = *terminator;
  data.avlKey.sip = terminator->sip;
  data.avlKey.dip = terminator->dip;

  /* No need to save the BCM_TUNNEL_REPLACE flags as they
  ** are operational commands
  */
  data.bcm_data.flags &= ~(BCM_TUNNEL_REPLACE);

  data.isValid = L7_TRUE;

  pData = avlInsertEntry(tunnelTree, &data);

  if (pData == &data)
  {
    rv = BCM_E_FAIL;
  }
  else if (pData != L7_NULLPTR)
  {
    rv = BCM_E_EXISTS;

    /* remove the entry and re-add the entry */
    pData = avlDeleteEntry(tunnelTree, &data);

    pData = avlInsertEntry(tunnelTree, &data);
  }

  USL_L3_TUNNEL_TERMINATOR_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a tunnel terminator from USL Db
*
* @param    dbType         @{(input)} dbType to be modified
* @param    terminator     @{(input)} The tunnel terminator info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_tunnel_terminator_delete(USL_DB_TYPE_t dbType,
                                       bcm_tunnel_terminator_t *terminator)
{
  int                                 rv = BCM_E_NONE;
  usl_l3_tunnel_terminator_db_elem_t  data;
  uslDbElemInfo_t                     elemInfo;

  if (uslIpTunnelTerminatorDbActive == L7_FALSE)
  {
    return rv;
  }

  data.avlKey.sip = terminator->sip;
  data.avlKey.dip = terminator->dip;
  data.bcm_data = *terminator;
  elemInfo.dbElem = &data;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  return usl_delete_l3_tunnel_terminator_db_elem(dbType, elemInfo);
}

/*********************************************************************
* @purpose  Create an egress Nhop at Usl Bcm layer from the given
*           db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_create_l3_tunnel_terminator_db_elem_bcm(void *item)
{
  L7_int32                              rv;
  usl_l3_tunnel_terminator_db_elem_t    *elem = item;

  do
  {
    rv = usl_bcm_l3_tunnel_terminator_add(&(elem->bcm_data));

  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Delete initiator at the Usl Bcm layer from the
*           given db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_l3_tunnel_terminator_db_elem_bcm(void *item)
{
  L7_int32                               rv;
  usl_l3_tunnel_terminator_db_elem_t    *elem = item;

  do
  {
    rv = usl_bcm_l3_tunnel_terminator_delete(&(elem->bcm_data));

  } while (0);

  return rv;
}


/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.

* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_l3_tunnel_terminator_db_elem_bcm(void *shadowDbItem,
                                                     void *operDbItem)
{
  L7_int32                               rv = BCM_E_NONE;
  usl_l3_tunnel_terminator_db_elem_t    *shadowElem = shadowDbItem;
  usl_l3_tunnel_terminator_db_elem_t    *operElem = operDbItem;
  bcm_tunnel_terminator_t                terminatorData;

  memset(&terminatorData, 0, sizeof(terminatorData));

  if (memcmp(&(shadowElem->bcm_data), &(operElem->bcm_data),
             sizeof(shadowElem->bcm_data)) != 0)
  {
    memcpy(&terminatorData, &(shadowElem->bcm_data),
           sizeof(shadowElem->bcm_data));
    terminatorData.flags |= BCM_TUNNEL_REPLACE;

    rv = usl_bcm_l3_tunnel_terminator_add(&terminatorData);
  }

  return rv;

}


static L7_RC_t usl_l3_tunnel_terminator_db_alloc(avlTree_t *terminatorTree,
                                                 avlTreeTables_t **terminatorTreeHeap,
                                                 usl_l3_tunnel_terminator_db_elem_t  **terminatorDataHeap)
{
  L7_uint32    max;
  L7_RC_t      rc = L7_SUCCESS;


  max = platIntfTunnelIntfMaxCountGet() + platWirelessTunnelsMaxCountGet();

  *terminatorTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                    sizeof(avlTreeTables_t) * max);

  if (*terminatorTreeHeap == L7_NULLPTR)
  {
    rc = L7_FAILURE;
    return rc;
  }

  *terminatorDataHeap =
          osapiMalloc(L7_DRIVER_COMPONENT_ID,
                      sizeof(usl_l3_tunnel_terminator_db_elem_t) * max);

  if (*terminatorDataHeap == L7_NULLPTR)
  {
    rc = L7_FAILURE;
    return rc;
  }

  /* clear the memory before creating the tree */
  memset((void*)terminatorTree, 0, sizeof(avlTree_t));
  memset((void*)*terminatorTreeHeap, 0,
         sizeof(avlTreeTables_t) * max);
  memset((void*)*terminatorDataHeap, 0,
         sizeof(usl_l3_tunnel_terminator_db_elem_t) * max);

  /* Key is the subnet and mask */
  avlCreateAvlTree(terminatorTree,
                   *terminatorTreeHeap,
                   *terminatorDataHeap,
                   max, sizeof(usl_l3_tunnel_terminator_db_elem_t), 0x10,
                   sizeof(usl_l3_tunnel_terminator_db_key_t));

  rc = L7_SUCCESS;
  return rc;

}

/*********************************************************************
* @purpose  Create the Tunnel Terminator Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_tunnel_terminator_db_init()
{
  L7_RC_t            rc = L7_ERROR;
  uslDbSyncFuncs_t   tunnelTerminatorDbFuncs;

  memset(&tunnelTerminatorDbFuncs, 0, sizeof(tunnelTerminatorDbFuncs));


  do
  {

    /* Allocate db only on Stackables */
    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {

      pUslIpTunnelTerminatorDbSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
      if ( pUslIpTunnelTerminatorDbSema == L7_NULLPTR ) break;


      if (usl_l3_tunnel_terminator_db_alloc(&uslOperIpTunnelTerminatorTreeData,
                                            &pUslOperIpTunnelTerminatorTreeHeap,
                                            &pUslOperIpTunnelTerminatorDataHeap) != L7_SUCCESS)
      {
        break;
      }


      /* Allocate shadow db for NSF feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {
        if (usl_l3_tunnel_terminator_db_alloc(&uslShadowIpTunnelTerminatorTreeData,
                                              &pUslShadowIpTunnelTerminatorTreeHeap,
                                              &pUslShadowIpTunnelTerminatorDataHeap) != L7_SUCCESS)
        {
          break;
        }

      }

      /* Register the sync routines */
      tunnelTerminatorDbFuncs.get_size_of_db_elem =
         usl_get_size_of_l3_tunnel_terminator_db_elem;
      tunnelTerminatorDbFuncs.get_db_elem =
         usl_get_l3_tunnel_terminator_db_elem;
      tunnelTerminatorDbFuncs.delete_elem_from_db =
         usl_delete_l3_tunnel_terminator_db_elem;
      tunnelTerminatorDbFuncs.print_db_elem =
         usl_print_l3_tunnel_terminator_db_elem;
      tunnelTerminatorDbFuncs.create_usl_bcm =
         usl_create_l3_tunnel_terminator_db_elem_bcm;
      tunnelTerminatorDbFuncs.delete_usl_bcm =
         usl_delete_l3_tunnel_terminator_db_elem_bcm;
      tunnelTerminatorDbFuncs.update_usl_bcm =
         usl_update_l3_tunnel_terminator_db_elem_bcm;


      /* Not registering optional routines as the db element is deterministic
      ** i.e no pointer member elements
      */
      tunnelTerminatorDbFuncs.alloc_db_elem = L7_NULLPTR;
      tunnelTerminatorDbFuncs.copy_db_elem  = L7_NULLPTR;
      tunnelTerminatorDbFuncs.free_db_elem  = L7_NULLPTR;
      tunnelTerminatorDbFuncs.pack_db_elem  = L7_NULLPTR;
      tunnelTerminatorDbFuncs.unpack_db_elem = L7_NULLPTR;

      if (usl_db_sync_func_table_register(USL_L3_TUNNEL_TERMINATOR_DB_ID,
                                          &tunnelTerminatorDbFuncs) != L7_SUCCESS)
      {
        L7_LOG_ERROR(0);
      }

      /* Point the Db handle to Operational table */
      uslIpTunnelTerminatorTreeHandle = &uslOperIpTunnelTerminatorTreeData;

      uslIpTunnelTerminatorDbActive = L7_TRUE;

    } /* End if stacking */

    rc = L7_SUCCESS;

  } while(0);

  return rc;
}

/*********************************************************************
* @purpose  Destroy the Tunnel Terminator Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_tunnel_terminator_db_fini()
{
  L7_uint32 max = platIntfTunnelIntfMaxCountGet() + platWirelessTunnelsMaxCountGet();

  uslIpTunnelTerminatorDbActive = L7_FALSE;
  uslIpTunnelTerminatorTreeHandle = L7_NULLPTR;

  if ( pUslIpTunnelTerminatorDbSema != L7_NULLPTR )
  {
    osapiSemaDelete(pUslIpTunnelTerminatorDbSema);
    pUslIpTunnelTerminatorDbSema = L7_NULLPTR;
  }

  avlDeleteAvlTree(&uslOperIpTunnelTerminatorTreeData);

  /* clear the memory before creating the tree */
  memset((void*)&uslOperIpTunnelTerminatorTreeData,0,sizeof(avlTree_t));


  if ( pUslOperIpTunnelTerminatorTreeHeap != L7_NULLPTR )
  {
    memset((void*)pUslOperIpTunnelTerminatorTreeHeap, 0,
           sizeof(avlTreeTables_t)  * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperIpTunnelTerminatorTreeHeap);
    pUslOperIpTunnelTerminatorTreeHeap = L7_NULLPTR;
  }

  if ( pUslOperIpTunnelTerminatorDataHeap != L7_NULLPTR )
  {
    memset((void*)pUslOperIpTunnelTerminatorDataHeap, 0,
           sizeof(usl_lpm_db_elem_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperIpTunnelTerminatorDataHeap);
    pUslOperIpTunnelTerminatorDataHeap = L7_NULLPTR;
  }

  avlDeleteAvlTree(&uslShadowIpTunnelTerminatorTreeData);

  /* clear the memory before creating the tree */
  memset((void*)&uslShadowIpTunnelTerminatorTreeData,0,sizeof(avlTree_t));


  if ( pUslShadowIpTunnelTerminatorTreeHeap != L7_NULLPTR )
  {
    memset((void*)pUslShadowIpTunnelTerminatorTreeHeap, 0,
           sizeof(avlTreeTables_t)  * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowIpTunnelTerminatorTreeHeap);
    pUslShadowIpTunnelTerminatorTreeHeap = L7_NULLPTR;
  }

  if ( pUslShadowIpTunnelTerminatorDataHeap != L7_NULLPTR )
  {
    memset((void*)pUslShadowIpTunnelTerminatorDataHeap, 0,
           sizeof(usl_lpm_db_elem_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowIpTunnelTerminatorDataHeap);
    pUslShadowIpTunnelTerminatorDataHeap = L7_NULLPTR;
  }

  return L7_SUCCESS;
}

void usl_l3_terminator_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8                             uslStr[128];
  usl_l3_tunnel_terminator_db_elem_t    elem, nextElem;
  uslDbElemInfo_t                       searchElem, elemInfo;
  avlTree_t                            *dbHandle;
  L7_uint32                             entryCount = 0;

  if (uslIpTunnelTerminatorDbActive == L7_FALSE)
  {

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L3 tunnel terminator database is not active\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L3 tunnel terminator database is active\n");
  }

  dbHandle = usl_l3_tunnel_terminator_db_handle_get(dbType);
  if (dbHandle != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Dumping %s \n",
                  usl_db_type_name_get(dbType));
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "%s is not present\n",
                   usl_db_type_name_get(dbType));
  }

  memset(&elem, 0, sizeof(elem));
  memset(&nextElem, 0, sizeof(nextElem));

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_l3_tunnel_terminator_db_elem(dbType, USL_DB_NEXT_ELEM,
                                              searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_l3_tunnel_terminator_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  uslStr);
    memcpy(searchElem.dbElem, elemInfo.dbElem,
           sizeof(usl_l3_tunnel_terminator_db_elem_t));
    entryCount++;
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Total number of entries in the table: %d\n",
                entryCount);

  return;
}

/*********************************************************************
* @purpose  Invalidate the Tunnel terminator Intf Db
*
* @param    Flags  {(input)} Type of db to be invalidated.
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_invalidate_l3_tunnel_terminator_db(USL_DB_TYPE_t flags)
{
  L7_RC_t   rc = L7_ERROR;
  L7_uint32 max = platIntfTunnelIntfMaxCountGet() + platWirelessTunnelsMaxCountGet();

  if (uslIpTunnelTerminatorDbActive == L7_FALSE)
  {
    return rc;
  }

  USL_L3_TUNNEL_TERMINATOR_DB_LOCK_TAKE();

  do
  {
    if (flags & USL_OPERATIONAL_DB)
    {

      if ((pUslOperIpTunnelTerminatorTreeHeap == L7_NULLPTR) || (pUslOperIpTunnelTerminatorDataHeap == L7_NULLPTR))
        break;

      /* Key is the subnet and mask */
      avlPurgeAvlTree(&uslOperIpTunnelTerminatorTreeData, max);
    }

    if (flags & USL_SHADOW_DB)
    {
      if ((pUslShadowIpTunnelTerminatorTreeHeap == L7_NULLPTR) || (pUslShadowIpTunnelTerminatorDataHeap == L7_NULLPTR))
        break;

      /* Key is the subnet and mask */
      avlPurgeAvlTree(&uslShadowIpTunnelTerminatorTreeData, max);

    }

    rc = L7_SUCCESS;

  } while (0);

  USL_L3_TUNNEL_TERMINATOR_DB_LOCK_GIVE();

  return rc;
}

/*********************************************************************
* @purpose  Set the Tunnel terminator Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_l3_tunnel_terminator_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_L3_TUNNEL_TERMINATOR_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    uslIpTunnelTerminatorTreeHandle = &uslOperIpTunnelTerminatorTreeData;
  }
  else if (dbType == USL_SHADOW_DB)
  {
    uslIpTunnelTerminatorTreeHandle = &uslShadowIpTunnelTerminatorTreeData;
  }

  USL_L3_TUNNEL_TERMINATOR_DB_LOCK_GIVE();

  return;
}



/*********************************************************************
* @purpose  Given a dbType, return the db Handle.
*
* @params   dbType {(input)} OPER/SHADOW/CURRENT
*
* @returns  Pointer to db Handle
*
* @notes    none
*
* @end
*********************************************************************/
avlTree_t *usl_l3_tunnel_initiator_db_handle_get(USL_DB_TYPE_t dbType)
{
  avlTree_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = &uslOperIpTunnelInitiatorTreeData;
      break;

    case USL_SHADOW_DB:
      dbHandle = &uslShadowIpTunnelInitiatorTreeData;
      break;

    case USL_CURRENT_DB:
      dbHandle = uslIpTunnelInitiatorTreeHandle;
      break;

    default:
      break;
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Get the size of a Tunnel Initiator Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_l3_tunnel_initiator_db_elem(void)
{
  return sizeof(usl_l3_tunnel_initiator_db_elem_t);
}

/*********************************************************************
* @purpose  Print the contents of a l3 tunnel initiator db element in
*           specified buffer
*
* @params   elem     {(input)}  Element to be printed
*           buffer   {(input)}  buffer pointer
*           size     {(input)}  size of buffer
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
void usl_print_l3_tunnel_initiator_db_elem(void *item, L7_uchar8 *buffer,
                                           L7_uint32 size)
{
  usl_l3_tunnel_initiator_db_elem_t  *elem = item;


  osapiSnprintf(buffer, size,
                "intfId=%u, flags=%x, ttl=%u, sip=%x, dip=%x\n",
                elem->bcm_data.intf.l3a_intf_id,
                elem->bcm_data.initiator.flags,
                elem->bcm_data.initiator.ttl,
                elem->bcm_data.initiator.sip,
                elem->bcm_data.initiator.dip);

  osapiSnprintfcat(buffer, size,
                   "    mac=%0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x\n",
                   elem->bcm_data.initiator.dmac[0],
                   elem->bcm_data.initiator.dmac[1],
                   elem->bcm_data.initiator.dmac[2],
                   elem->bcm_data.initiator.dmac[3],
                   elem->bcm_data.initiator.dmac[4],
                   elem->bcm_data.initiator.dmac[5]);


  osapiSnprintfcat(buffer, size,
                   "    dscp_sel=%u, dscp=%u, dscp_map=%u\n",
                   elem->bcm_data.initiator.dscp_sel,
                   elem->bcm_data.initiator.dscp,
                   elem->bcm_data.initiator.dscp_map);

  return;
}

/*********************************************************************
* @purpose  Search element in lpm db and return pointer to it
*
* @params   dbType     {(input)} Db type (Oper or Shadow)
*           flags      {(input)} USL_EXACT_ELEM or USL_NEXT_ELEM
*           searchInfo {(input)} Element to be searched
*           elemInfo   {(output)} Found element info
*
* @returns  BCM_E_NONE: Element found
*           BCM_E_NOT_FOUND: Element not found
*
* @notes    Internal API. Db lock must be held. Returns pointer
*           to the element.
*
* @end
*********************************************************************/
static L7_int32 usl_search_l3_tunnel_initiator_db_elem(USL_DB_TYPE_t dbType,
                                                       L7_uint32 flags,
                                                       uslDbElemInfo_t searchInfo,
                                                       uslDbElemInfo_t *elemInfo)
{
  avlTree_t                 *dbHandle = L7_NULLPTR;
  L7_int32                   rv = BCM_E_NOT_FOUND;
  L7_uchar8                  traceStr[128];


  osapiSnprintf(traceStr, sizeof(traceStr),
                "Get Elem: Flags %d \n",
                flags);
  usl_trace_sync_detail(USL_L3_TUNNEL_INITIATOR_DB_ID, traceStr);


  elemInfo->dbElem = L7_NULLPTR;
  elemInfo->elemIndex = USL_INVALID_DB_INDEX;

  do
  {

    dbHandle = usl_l3_tunnel_initiator_db_handle_get(dbType);

    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Get next */
    if (flags == USL_DB_NEXT_ELEM)
    {
      elemInfo->dbElem = avlSearchLVL7(dbHandle, searchInfo.dbElem, AVL_NEXT);
    }
    else if (flags == USL_DB_EXACT_ELEM)
    {
      elemInfo->dbElem = avlSearchLVL7(dbHandle, searchInfo.dbElem, AVL_EXACT);
    }

    if (elemInfo->dbElem == L7_NULLPTR)
    {
      rv = BCM_E_NOT_FOUND;
    }
    else
    {
      rv = BCM_E_NONE;
    }

  } while (0);


  return rv;
}

/*********************************************************************
* @purpose  Get info about an element from the Db
*
* @params   dbType        {(input)}  dbType to be searched
*           flags         {(input)}  EXACT or GET_NEXT
*           searchInfo    {(input)}  Information about element to be searched
*           elemInfo      {(output)} Storage for element & index
*
* @returns  BCM error code
*
* @notes    Caller must allocate memory to copy the db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_l3_tunnel_initiator_db_elem(USL_DB_TYPE_t     dbType,
                                             L7_uint32         flags,
                                             uslDbElemInfo_t   searchElem,
                                             uslDbElemInfo_t  *elemInfo)
{
  L7_int32         rv = BCM_E_NONE;
  uslDbElemInfo_t  initiatorInfo;

  if (!uslIpTunnelInitiatorDbActive)
  {
    return rv;
  }

  initiatorInfo.dbElem = L7_NULLPTR;
  initiatorInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_L3_TUNNEL_INITIATOR_DB_LOCK_TAKE();

  rv = usl_search_l3_tunnel_initiator_db_elem(dbType, flags,
                                              searchElem, &initiatorInfo);
  if (rv == BCM_E_NONE)
  {
    memcpy(elemInfo->dbElem, initiatorInfo.dbElem,
           sizeof(usl_l3_tunnel_initiator_db_elem_t));
  }

  USL_L3_TUNNEL_INITIATOR_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Delete a given element from Egr Nhop Db
*
* @params   dbType   {(input)}
*           elemInfo {(input)}
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_l3_tunnel_initiator_db_elem(USL_DB_TYPE_t   dbType,
                                                uslDbElemInfo_t elemInfo)
{
  avlTree_t                             *dbHandle;
  usl_l3_tunnel_initiator_db_elem_t     *elem;
  L7_int32                               rv = BCM_E_NONE;

  if (!uslIpTunnelInitiatorDbActive)
  {
    return rv;
  }

  USL_L3_TUNNEL_INITIATOR_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_l3_tunnel_initiator_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Delete the elem */
    elem = avlDeleteEntry(dbHandle, elemInfo.dbElem);
    if (elem == L7_NULLPTR)
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);

  USL_L3_TUNNEL_INITIATOR_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Add a tunnel initiator in USL Db
*
* @param    dbTyoe        @{(input)} db type to modify
* @param    intf          @{(input)} The tunnel interface
* @param    initiator     @{(input)} The tunnel initiator info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_tunnel_initiator_set(USL_DB_TYPE_t dbType,
                                   bcm_l3_intf_t *intf,
                                   bcm_tunnel_initiator_t *initiator)
{
  int                                   rv = BCM_E_NONE;
  usl_l3_tunnel_initiator_db_elem_t     data;
  usl_l3_tunnel_initiator_db_elem_t    *pData;
  avlTree_t                            *initiatorTree;

  if (uslIpTunnelInitiatorDbActive == L7_FALSE)
  {
    return rv;
  }

  initiatorTree = usl_l3_tunnel_initiator_db_handle_get(dbType);
  if (initiatorTree == L7_NULLPTR)
  {
    return BCM_E_FAIL;
  }

  USL_L3_TUNNEL_INITIATOR_DB_LOCK_TAKE();

  /* no problems, update usl database */
  data.bcm_data.intf = *intf;
  data.bcm_data.initiator = *initiator;

  /* No need to save the BCM_TUNNEL_REPLACE flags as they
  ** are operational commands
  */
  data.bcm_data.initiator.flags &= ~(BCM_TUNNEL_REPLACE);

  /* No need to save the BCM_L3_WITH_ID/BCM_L3_REPLACE flags as they
  ** are operational commands
  */
  data.bcm_data.intf.l3a_flags &= ~(BCM_L3_WITH_ID | BCM_L3_REPLACE);

  data.avlKey.intf_id = intf->l3a_intf_id;

  data.isValid = L7_TRUE;

  pData = avlInsertEntry(initiatorTree, &data);

  if (pData == &data)
  {
    rv = BCM_E_FAIL;
    USL_LOG_ERROR("USL: Error adding initiator\n");
  }
  else if (pData != L7_NULLPTR)
  {
    rv = BCM_E_EXISTS;

    /* remove the entry and re-add the entry */
    pData = avlDeleteEntry(initiatorTree, &data);

    pData = avlInsertEntry(initiatorTree, &data);
  }

  USL_L3_TUNNEL_INITIATOR_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a tunnel initiator from USL Db
*
* @param    dbType        @{(input)} db type to update
* @param    intf          @{(input)} The tunnel interface
* @param    initiator     @{(input)} The tunnel initiator info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_tunnel_initiator_clear(USL_DB_TYPE_t dbType,
                                     bcm_l3_intf_t *intf)

{

  int                                rv = BCM_E_NONE;
  usl_l3_tunnel_initiator_db_elem_t  data;
  uslDbElemInfo_t                    elemInfo;
  if (uslIpTunnelInitiatorDbActive == L7_FALSE)
  {
    return rv;
  }

  data.avlKey.intf_id = intf->l3a_intf_id;
  data.bcm_data.intf = *intf;
  elemInfo.dbElem = &data;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  return usl_delete_l3_tunnel_initiator_db_elem(dbType, elemInfo);
}

/*********************************************************************
* @purpose  Create an egress Nhop at Usl Bcm layer from the given
*           db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_create_l3_tunnel_initiator_db_elem_bcm(void *item)
{
  L7_int32                              rv;
  usl_l3_tunnel_initiator_db_elem_t    *elem = item;

  do
  {
    rv = usl_bcm_l3_tunnel_initiator_set(&(elem->bcm_data.intf),
                                         &(elem->bcm_data.initiator));

  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Delete initiator at the Usl Bcm layer from the
*           given db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_l3_tunnel_initiator_db_elem_bcm(void *item)
{
  L7_int32                              rv;
  usl_l3_tunnel_initiator_db_elem_t    *elem = item;

  do
  {
    rv = usl_bcm_l3_tunnel_initiator_clear(&(elem->bcm_data.intf));

  } while (0);

  return rv;
}


/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.

* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_l3_tunnel_initiator_db_elem_bcm(void *shadowDbItem,
                                                    void *operDbItem)
{
  L7_int32                              rv = BCM_E_NONE;
  usl_l3_tunnel_initiator_db_elem_t    *shadowElem = shadowDbItem;
  usl_l3_tunnel_initiator_db_elem_t    *operElem = operDbItem;
  bcm_tunnel_initiator_t                initiatorData;

  memset(&initiatorData, 0, sizeof(initiatorData));

  if (memcmp(&(shadowElem->bcm_data), &(operElem->bcm_data),
             sizeof(shadowElem->bcm_data)) != 0)
  {
    memcpy(&initiatorData, &(shadowElem->bcm_data.initiator),
           sizeof(shadowElem->bcm_data.initiator));
    initiatorData.flags |= BCM_TUNNEL_REPLACE;

    rv = usl_bcm_l3_tunnel_initiator_set(&(shadowElem->bcm_data.intf),
                                         &initiatorData);
  }

  return rv;

}

static L7_RC_t usl_l3_tunnel_initiator_db_alloc(avlTree_t *initiatorTree,
                                                avlTreeTables_t **initiatorTreeHeap,
                                                usl_l3_tunnel_initiator_db_elem_t  **initiatorDataHeap)
{
  L7_uint32    max;
  L7_RC_t      rc = L7_SUCCESS;


  max = platIntfTunnelIntfMaxCountGet() + platWirelessTunnelsMaxCountGet();

  *initiatorTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                   sizeof(avlTreeTables_t) * max);

  if (*initiatorTreeHeap == L7_NULLPTR)
  {
    rc = L7_FAILURE;
    return rc;
  }

  *initiatorDataHeap =
          osapiMalloc(L7_DRIVER_COMPONENT_ID,
                      sizeof(usl_l3_tunnel_initiator_db_elem_t) * max);

  if (*initiatorDataHeap == L7_NULLPTR)
  {
    rc = L7_FAILURE;
    return rc;
  }

  /* clear the memory before creating the tree */
  memset((void*)initiatorTree, 0, sizeof(avlTree_t));
  memset((void*)*initiatorTreeHeap, 0,
         sizeof(avlTreeTables_t) * max);
  memset((void*)*initiatorDataHeap, 0,
         sizeof(usl_l3_tunnel_initiator_db_elem_t) * max);

  /* Key is the subnet and mask */
  avlCreateAvlTree(initiatorTree,
                   *initiatorTreeHeap,
                   *initiatorDataHeap,
                   max, sizeof(usl_l3_tunnel_initiator_db_elem_t), 0x10,
                   sizeof(usl_l3_tunnel_initiator_db_key_t));

  rc = L7_SUCCESS;
  return rc;

}

/*********************************************************************
* @purpose  Create the Tunnel Initiator Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_tunnel_initiator_db_init()
{
  L7_RC_t            rc = L7_ERROR;
  uslDbSyncFuncs_t   tunnelInitiatorDbFuncs;

  memset(&tunnelInitiatorDbFuncs, 0, sizeof(tunnelInitiatorDbFuncs));

  do
  {
    /* Allocate db only on Stackables */
    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {

      pUslIpTunnelInitiatorDbSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
      if ( pUslIpTunnelInitiatorDbSema == L7_NULLPTR ) break;

      if (usl_l3_tunnel_initiator_db_alloc(&uslOperIpTunnelInitiatorTreeData,
                                           &pUslOperIpTunnelInitiatorTreeHeap,
                                           &pUslOperIpTunnelInitiatorDataHeap) != L7_SUCCESS)
      {
        break;
      }

      /* Allocate shadow db for NSF feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {

        if (usl_l3_tunnel_initiator_db_alloc(&uslShadowIpTunnelInitiatorTreeData,
                                             &pUslShadowIpTunnelInitiatorTreeHeap,
                                             &pUslShadowIpTunnelInitiatorDataHeap) != L7_SUCCESS)
        {
          break;
        }

      }

      /* Register the sync routines */
      tunnelInitiatorDbFuncs.get_size_of_db_elem =
         usl_get_size_of_l3_tunnel_initiator_db_elem;
      tunnelInitiatorDbFuncs.get_db_elem =
         usl_get_l3_tunnel_initiator_db_elem;
      tunnelInitiatorDbFuncs.delete_elem_from_db =
         usl_delete_l3_tunnel_initiator_db_elem;
      tunnelInitiatorDbFuncs.print_db_elem =
         usl_print_l3_tunnel_initiator_db_elem;
      tunnelInitiatorDbFuncs.create_usl_bcm =
         usl_create_l3_tunnel_initiator_db_elem_bcm;
      tunnelInitiatorDbFuncs.delete_usl_bcm =
         usl_delete_l3_tunnel_initiator_db_elem_bcm;
      tunnelInitiatorDbFuncs.update_usl_bcm =
         usl_update_l3_tunnel_initiator_db_elem_bcm;


      /* Not registering optional routines as the db element is deterministic
      ** i.e no pointer member elements
      */
      tunnelInitiatorDbFuncs.alloc_db_elem = L7_NULLPTR;
      tunnelInitiatorDbFuncs.copy_db_elem  = L7_NULLPTR;
      tunnelInitiatorDbFuncs.free_db_elem  = L7_NULLPTR;
      tunnelInitiatorDbFuncs.pack_db_elem  = L7_NULLPTR;
      tunnelInitiatorDbFuncs.unpack_db_elem = L7_NULLPTR;

      if (usl_db_sync_func_table_register(USL_L3_TUNNEL_INITIATOR_DB_ID,
                                          &tunnelInitiatorDbFuncs) != L7_SUCCESS)
      {
        L7_LOG_ERROR(0);
      }

      /* Point the Db handle to Operational table */
      uslIpTunnelInitiatorTreeHandle = &uslOperIpTunnelInitiatorTreeData;

      uslIpTunnelInitiatorDbActive = L7_TRUE;

    } /* End if stacking */

    rc = L7_SUCCESS;

  } while(0);

  return rc;
}

/*********************************************************************
* @purpose  Destroy the Tunnel Initiator Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_tunnel_initiator_db_fini()
{
   L7_uint32 max = platIntfTunnelIntfMaxCountGet() + platWirelessTunnelsMaxCountGet();

   uslIpTunnelInitiatorDbActive = L7_FALSE;
   uslIpTunnelInitiatorTreeHandle = L7_NULLPTR;

   if ( pUslIpTunnelInitiatorDbSema != L7_NULLPTR )
   {
     osapiSemaDelete(pUslIpTunnelInitiatorDbSema);
     pUslIpTunnelInitiatorDbSema = L7_NULLPTR;
   }

   avlDeleteAvlTree(&uslOperIpTunnelInitiatorTreeData);

   /* clear the memory before creating the tree */
   memset((void*)&uslOperIpTunnelInitiatorTreeData,0,sizeof(avlTree_t));


   if ( pUslOperIpTunnelInitiatorTreeHeap != L7_NULLPTR )
   {
     memset((void*)pUslOperIpTunnelInitiatorTreeHeap, 0,
            sizeof(avlTreeTables_t)  * max);
     osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperIpTunnelInitiatorTreeHeap);
     pUslOperIpTunnelInitiatorTreeHeap = L7_NULLPTR;
   }

   if ( pUslOperIpTunnelInitiatorDataHeap != L7_NULLPTR )
   {
     memset((void*)pUslOperIpTunnelInitiatorDataHeap, 0,
            sizeof(usl_lpm_db_elem_t) * max);
     osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperIpTunnelInitiatorDataHeap);
     pUslOperIpTunnelInitiatorDataHeap = L7_NULLPTR;
   }

   avlDeleteAvlTree(&uslShadowIpTunnelInitiatorTreeData);

   /* clear the memory before creating the tree */
   memset((void*)&uslShadowIpTunnelInitiatorTreeData,0,sizeof(avlTree_t));

   if ( pUslShadowIpTunnelInitiatorTreeHeap != L7_NULLPTR )
   {
     memset((void*)pUslShadowIpTunnelInitiatorTreeHeap, 0,
            sizeof(avlTreeTables_t)  * max);
     osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowIpTunnelInitiatorTreeHeap);
     pUslShadowIpTunnelInitiatorTreeHeap = L7_NULLPTR;
   }

   if ( pUslShadowIpTunnelInitiatorDataHeap != L7_NULLPTR )
   {
     memset((void*)pUslShadowIpTunnelInitiatorDataHeap, 0,
            sizeof(usl_lpm_db_elem_t) * max);
     osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowIpTunnelInitiatorDataHeap);
     pUslShadowIpTunnelInitiatorDataHeap = L7_NULLPTR;
   }

   return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Invalidate the Tunnel initiator Db
*
* @param    Flags  {(input)} Type of db to be invalidated.
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_tunnel_initiator_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t   rc = L7_ERROR;
  L7_uint32 max = platIntfTunnelIntfMaxCountGet() + platWirelessTunnelsMaxCountGet();

  if (uslIpTunnelInitiatorDbActive == L7_FALSE)
  {
    return rc;
  }

  USL_L3_TUNNEL_INITIATOR_DB_LOCK_TAKE();

  do
  {
    if (flags & USL_OPERATIONAL_DB)
    {
      if ((pUslOperIpTunnelInitiatorTreeHeap == L7_NULLPTR) || (pUslOperIpTunnelInitiatorDataHeap == L7_NULLPTR))
        break;

      /* Key is the subnet and mask */
      avlPurgeAvlTree(&uslOperIpTunnelInitiatorTreeData, max);
    }

    if (flags & USL_SHADOW_DB)
    {
      if ((pUslShadowIpTunnelInitiatorTreeHeap == L7_NULLPTR) || (pUslShadowIpTunnelInitiatorDataHeap == L7_NULLPTR))
        break;

      /* Key is the subnet and mask */
      avlPurgeAvlTree(&uslShadowIpTunnelInitiatorTreeData, max);

    }

    rc = L7_SUCCESS;

  } while (0);

  USL_L3_TUNNEL_INITIATOR_DB_LOCK_GIVE();

  return rc;
}

/*********************************************************************
* @purpose  Set the Tunnel Initiator Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_l3_tunnel_initiator_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_L3_TUNNEL_INITIATOR_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    uslIpTunnelInitiatorTreeHandle = &uslOperIpTunnelInitiatorTreeData;
  }
  else if (dbType == USL_SHADOW_DB)
  {
    uslIpTunnelInitiatorTreeHandle = &uslShadowIpTunnelInitiatorTreeData;
  }

  USL_L3_TUNNEL_INITIATOR_DB_LOCK_GIVE();

  return;
}

void usl_l3_initiator_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8                           uslStr[128];
  usl_l3_tunnel_initiator_db_elem_t   elem, nextElem;
  uslDbElemInfo_t                     searchElem, elemInfo;
  avlTree_t                          *dbHandle;
  L7_uint32                           entryCount = 0;

  if (uslIpTunnelInitiatorDbActive == L7_FALSE)
  {

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L3 tunnel initiator database is not active\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L3 tunnel initiator database is active\n");
  }

  dbHandle = usl_l3_tunnel_initiator_db_handle_get(dbType);
  if (dbHandle != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Dumping %s \n",
                  usl_db_type_name_get(dbType));
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "%s is not present\n",
                   usl_db_type_name_get(dbType));
  }

  memset(&elem, 0, sizeof(elem));
  memset(&nextElem, 0, sizeof(nextElem));

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_l3_tunnel_initiator_db_elem(dbType, USL_DB_NEXT_ELEM,
                                             searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_l3_tunnel_initiator_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  uslStr);
    memcpy(searchElem.dbElem, elemInfo.dbElem, sizeof(usl_l3_tunnel_initiator_db_elem_t));
    entryCount++;
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Total number of entries in the table: %d\n",
                entryCount);

  return;
}


/*********************************************************************
* @purpose  Given a dbType, return the db Handle.
*
* @params   dbType {(input)} OPER/SHADOW/CURRENT
*
* @returns  Pointer to db Handle
*
* @notes    none
*
* @end
*********************************************************************/
avlTree_t *usl_lpm_db_handle_get(USL_DB_TYPE_t dbType)
{
  avlTree_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = &uslOperIpLpmTreeData;
      break;

    case USL_SHADOW_DB:
      dbHandle = &uslShadowIpLpmTreeData;
      break;

    case USL_CURRENT_DB:
      dbHandle = uslIpLpmTreeHandle;
      break;

    default:
      break;
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Get the size of a Lpm Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_lpm_db_elem(void)
{
  return sizeof(usl_lpm_db_elem_t);
}

/*********************************************************************
* @purpose  Print the contents of a lpm db element in
*           specified buffer
*
* @params   elem     {(input)}  Element to be printed
*           buffer   {(input)}  buffer pointer
*           size     {(input)}  size of buffer
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
void usl_print_lpm_db_elem(void *item, L7_uchar8 *buffer,
                           L7_uint32 size)
{
  L7_char8            addr_str[64];
  L7_char8            mask_str[64];
  usl_lpm_db_elem_t  *elem = item;

  USL_L3_NTOP((elem->bcm_data.l3a_flags & BCM_L3_IP6
               ? L7_AF_INET6
               : L7_AF_INET),
               (elem->bcm_data.l3a_flags & BCM_L3_IP6
               ? (void *)&elem->bcm_data.routeKey.l3a_subnet.l3a_ip6_net
               : (void *)&elem->bcm_data.routeKey.l3a_subnet.l3a_ip_net),
               addr_str,
               sizeof(addr_str));
  USL_L3_NTOP((elem->bcm_data.l3a_flags & BCM_L3_IP6
               ? L7_AF_INET6
               : L7_AF_INET),
               (elem->bcm_data.l3a_flags & BCM_L3_IP6
               ? (void *)&elem->bcm_data.routeKey.l3a_mask.l3a_ip6_mask
               : (void *)&elem->bcm_data.routeKey.l3a_mask.l3a_ip_mask),
               mask_str,
               sizeof(mask_str));

  osapiSnprintf(buffer, size,
                "IP=%s, Mask=%s\n",
                addr_str, mask_str);

  osapiSnprintfcat(buffer, size,
                "L3 intf=%d, Flags=%x,\n",
                elem->bcm_data.l3a_intf,
                elem->bcm_data.l3a_flags);

  return;
}

/*********************************************************************
* @purpose  Search element in lpm db and return pointer to it
*
* @params   dbType     {(input)} Db type (Oper or Shadow)
*           flags      {(input)} USL_EXACT_ELEM or USL_NEXT_ELEM
*           searchInfo {(input)} Element to be searched
*           elemInfo   {(output)} Found element info
*
* @returns  BCM_E_NONE: Element found
*           BCM_E_NOT_FOUND: Element not found
*
* @notes    Internal API. Db lock must be held. Returns pointer
*           to the element.
*
* @end
*********************************************************************/
static L7_int32 usl_search_lpm_db_elem(USL_DB_TYPE_t dbType,
                                       L7_uint32 flags,
                                       uslDbElemInfo_t searchInfo,
                                       uslDbElemInfo_t *elemInfo)
{
  avlTree_t                 *dbHandle = L7_NULLPTR;
  L7_int32                   rv = BCM_E_NOT_FOUND;
  L7_uchar8                  traceStr[128];


  osapiSnprintf(traceStr, sizeof(traceStr),
                "Get Elem: Flags %d \n",
                flags);
  usl_trace_sync_detail(USL_L3_LPM_DB_ID, traceStr);


  elemInfo->dbElem = L7_NULLPTR;
  elemInfo->elemIndex = USL_INVALID_DB_INDEX;

  do
  {

    dbHandle = usl_lpm_db_handle_get(dbType);

    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Get next */
    if (flags == USL_DB_NEXT_ELEM)
    {
      elemInfo->dbElem = avlSearchLVL7(dbHandle, searchInfo.dbElem, AVL_NEXT);
    }
    else if (flags == USL_DB_EXACT_ELEM)
    {
      elemInfo->dbElem = avlSearchLVL7(dbHandle, searchInfo.dbElem, AVL_EXACT);
    }

    if (elemInfo->dbElem == L7_NULLPTR)
    {
      rv = BCM_E_NOT_FOUND;
    }
    else
    {
      rv = BCM_E_NONE;
    }

  } while (0);


  return rv;
}

/*********************************************************************
* @purpose  Get info about an element from the Db
*
* @params   dbType        {(input)}  dbType to be searched
*           flags         {(input)}  EXACT or GET_NEXT
*           searchInfo    {(input)}  Information about element to be searched
*           elemInfo      {(output)} Storage for element & index
*
* @returns  BCM error code
*
* @notes    Caller must allocate memory to copy the db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_lpm_db_elem(USL_DB_TYPE_t     dbType,
                             L7_uint32         flags,
                             uslDbElemInfo_t   searchElem,
                             uslDbElemInfo_t  *elemInfo)
{
  L7_int32         rv = BCM_E_NONE;
  uslDbElemInfo_t  lpmInfo;

  if (!uslIpLpmDbActive)
  {
    return rv;
  }

  lpmInfo.dbElem = L7_NULLPTR;
  lpmInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_L3_LPM_DB_LOCK_TAKE();

  rv = usl_search_lpm_db_elem(dbType, flags, searchElem, &lpmInfo);
  if (rv == BCM_E_NONE)
  {
    memcpy(elemInfo->dbElem, lpmInfo.dbElem, sizeof(usl_lpm_db_elem_t));
  }

  USL_L3_LPM_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Delete a given element from Egr Nhop Db
*
* @params   dbType   {(input)}
*           elemInfo {(input)}
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_lpm_db_elem(USL_DB_TYPE_t   dbType,
                                uslDbElemInfo_t elemInfo)
{
  avlTree_t               *dbHandle;
  usl_lpm_db_elem_t       *elem;
  L7_int32                 rv = BCM_E_NONE;

  if (!uslIpLpmDbActive)
  {
    return rv;
  }

  USL_L3_LPM_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_lpm_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Delete the elem */
    elem = avlDeleteEntry(dbHandle, elemInfo.dbElem);
    if (elem == L7_NULLPTR)
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);

  USL_L3_LPM_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Add a route entry in USL Db
*
* @param    dbType {(input)} Db type to modify
* @param    info   {(input)} Route info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_route_add(USL_DB_TYPE_t dbType, usl_bcm_l3_route_t *info)
{
  int                 rv = BCM_E_NONE;
  usl_lpm_db_elem_t   data;
  usl_lpm_db_elem_t  *pData;
  avlTree_t          *lpmTree;

  if (uslIpLpmDbActive == L7_FALSE)
  {
    return rv;
  }

  lpmTree = usl_lpm_db_handle_get(dbType);
  if (lpmTree == L7_NULLPTR)
  {
    return BCM_E_FAIL;
  }

  USL_L3_LPM_DB_LOCK_TAKE();

  if (uslDebugL3Enable)
  {
    printf("Db: Adding route %x/%x\n",
           info->routeKey.l3a_subnet.l3a_ip_net,
           info->routeKey.l3a_mask.l3a_ip_mask);
  }

  memset (&data, 0, sizeof(data));

  memcpy(&data.bcm_data, info, sizeof(*info));
  /* No need to save the BCM_L3_WITH_ID/BCM_L3_REPLACE flags as they
  ** are operational commands
  */
  data.bcm_data.l3a_flags &= ~(BCM_L3_WITH_ID | BCM_L3_REPLACE);

  data.isValid = L7_TRUE;

  pData = avlSearchLVL7(lpmTree, &data, AVL_EXACT);
  if (pData == L7_NULLPTR)
  {
    pData = avlInsertEntry(lpmTree, &data);
    if (pData != L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      USL_LOG_ERROR("USL: Error adding Route entry \n");
    }
  }
  else
  {
    memcpy (&(pData->bcm_data), &(data.bcm_data), sizeof (data.bcm_data));
  }

  USL_L3_LPM_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a route entry from USL Db
*
* @param    dbType {(input)} Db type to modify
* @param    info   {(input)} Route info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_route_delete(USL_DB_TYPE_t dbType, usl_bcm_l3_route_t *info)
{
  int                 rv = BCM_E_NONE;
  usl_lpm_db_elem_t   data;
  uslDbElemInfo_t     elemInfo;

  if (uslIpLpmDbActive == L7_FALSE)
  {
    return rv;
  }


  if (uslDebugL3Enable)
  {
    printf("Db: Deleting route %x/%x\n",
           info->routeKey.l3a_subnet.l3a_ip_net,
           info->routeKey.l3a_mask.l3a_ip_mask);
  }

  memset (&data, 0, sizeof(data));
  memcpy(&data.bcm_data,info, sizeof(*info));
  data.isValid = L7_TRUE;

  elemInfo.dbElem = &data;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  return usl_delete_lpm_db_elem(dbType, elemInfo);
}

/*********************************************************************
* @purpose  Create an egress Nhop at Usl Bcm layer from the given
*           db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_create_lpm_db_elem_bcm(void *item)
{
  L7_int32                       rv;
  usl_lpm_db_elem_t             *elem = item;

  do
  {
    rv = usl_bcm_l3_route_add(&(elem->bcm_data));

  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Delete route at the Usl Bcm layer from the
*           given db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_lpm_db_elem_bcm(void *item)
{
  L7_int32               rv;
  usl_lpm_db_elem_t     *elem = item;

  do
  {
    rv = usl_bcm_l3_route_delete(&(elem->bcm_data));

  } while (0);

  return rv;
}


/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.

* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_lpm_db_elem_bcm(void *shadowDbItem, void *operDbItem)
{
  L7_int32              rv = BCM_E_NONE;
  usl_lpm_db_elem_t    *shadowElem = shadowDbItem;
  usl_lpm_db_elem_t    *operElem = operDbItem;
  usl_bcm_l3_route_t    routeData;

  memset(&routeData, 0, sizeof(routeData));

  if (memcmp(&(shadowElem->bcm_data), &(operElem->bcm_data),
             sizeof(shadowElem->bcm_data)) != 0)
  {
    memcpy(&routeData, &(shadowElem->bcm_data), sizeof(shadowElem->bcm_data));
    routeData.l3a_flags |= BCM_L3_REPLACE;

    rv = usl_bcm_l3_route_add(&routeData);
  }

  return rv;

}


static L7_RC_t usl_lpm_db_alloc(avlTree_t *lpmTree,
                                avlTreeTables_t **lpmTreeHeap,
                                usl_lpm_db_elem_t  **lpmDataHeap)
{
  L7_uint32   max;
  L7_RC_t     rc = L7_SUCCESS;

  /* note +1 for link local ipv6 fe80:: route which is manually installed */
  max = HAPI_BROAD_L3_ROUTE_TBL_SIZE +1;

  *lpmTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                             sizeof(avlTreeTables_t) * max);
  if (*lpmTreeHeap == L7_NULLPTR )
  {
    rc = L7_FAILURE;
    return rc;
  }

  *lpmDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                             sizeof(usl_lpm_db_elem_t) * max);
  if (*lpmDataHeap == L7_NULLPTR )
  {
    rc = L7_FAILURE;
    return rc;
  }

  /* clear the memory before creating the tree */
  memset((void*)lpmTree,0,sizeof(avlTree_t));
  memset((void*)*lpmTreeHeap,0,sizeof(avlTreeTables_t)  * max);
  memset((void*)*lpmDataHeap,0,sizeof(usl_lpm_db_elem_t) * max);

  /* Key is the subnet and mask */
  avlCreateAvlTree(lpmTree, *lpmTreeHeap, *lpmDataHeap,
                   max, sizeof(usl_lpm_db_elem_t), 0x10,
                   sizeof(usl_bcm_route_key_t));

  rc = L7_SUCCESS;

  return rc;

}

/*********************************************************************
* @purpose  Create the LPM Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_lpm_db_init()
{
  L7_RC_t            rc = L7_ERROR;
  uslDbSyncFuncs_t   lpmDbFuncs;

  memset(&lpmDbFuncs, 0, sizeof(lpmDbFuncs));



  do
  {
    /* Allocate db only on Stackables */
    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {

      pUslIpLpmDbSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
      if ( pUslIpLpmDbSema == L7_NULLPTR ) break;

      if (usl_lpm_db_alloc(&uslOperIpLpmTreeData,
                           &pUslOperIpLpmTreeHeap,
                           &pUslOperIpLpmDataHeap) != L7_SUCCESS)
      {
        break;
      }

      /* Allocate shadow db for NSF feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {
        if (usl_lpm_db_alloc(&uslShadowIpLpmTreeData,
                             &pUslShadowIpLpmTreeHeap,
                             &pUslShadowIpLpmDataHeap) != L7_SUCCESS)
        {
          break;
        }
       }


      /* Register the sync routines */
      lpmDbFuncs.get_size_of_db_elem =  usl_get_size_of_lpm_db_elem;
      lpmDbFuncs.get_db_elem = usl_get_lpm_db_elem;
      lpmDbFuncs.delete_elem_from_db =  usl_delete_lpm_db_elem;
      lpmDbFuncs.print_db_elem = usl_print_lpm_db_elem;
      lpmDbFuncs.create_usl_bcm = usl_create_lpm_db_elem_bcm;
      lpmDbFuncs.delete_usl_bcm = usl_delete_lpm_db_elem_bcm;
      lpmDbFuncs.update_usl_bcm = usl_update_lpm_db_elem_bcm;


      /* Not registering optional routines as the db element is deterministic
      ** i.e no pointer member elements
      */
      lpmDbFuncs.alloc_db_elem = L7_NULLPTR;
      lpmDbFuncs.copy_db_elem  = L7_NULLPTR;
      lpmDbFuncs.free_db_elem  = L7_NULLPTR;
      lpmDbFuncs.pack_db_elem  = L7_NULLPTR;
      lpmDbFuncs.unpack_db_elem = L7_NULLPTR;

      if (usl_db_sync_func_table_register(USL_L3_LPM_DB_ID,
                                          &lpmDbFuncs) != L7_SUCCESS)
      {
        L7_LOG_ERROR(0);
      }

      /* Point the Db handle to Operational table */
      uslIpLpmTreeHandle = &uslOperIpLpmTreeData;

      uslIpLpmDbActive = L7_TRUE;

    } /* End if stacking */

    rc = L7_SUCCESS;
  } while(0);

  return rc;
}

/*********************************************************************
* @purpose  Destroy the LPM Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_lpm_db_fini()
{
  L7_uint32 max = HAPI_BROAD_L3_ROUTE_TBL_SIZE+1;

  uslIpLpmDbActive = L7_FALSE;
  uslIpLpmTreeHandle = L7_NULLPTR;

  if ( pUslIpLpmDbSema != L7_NULLPTR )
  {
    osapiSemaDelete(pUslIpLpmDbSema);
    pUslIpLpmDbSema = L7_NULLPTR;
  }

  avlDeleteAvlTree(&uslOperIpLpmTreeData);

  /* clear the memory before creating the tree */
  memset((void*)&uslOperIpLpmTreeData,0,sizeof(avlTree_t));

  if ( pUslOperIpLpmTreeHeap != L7_NULLPTR )
  {
    memset((void*)pUslOperIpLpmTreeHeap,0,sizeof(avlTreeTables_t)  * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperIpLpmTreeHeap);
    pUslOperIpLpmTreeHeap = L7_NULLPTR;
  }

  if ( pUslOperIpLpmDataHeap != L7_NULLPTR )
  {
    memset((void*)pUslOperIpLpmDataHeap,0,sizeof(usl_lpm_db_elem_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperIpLpmDataHeap);
    pUslOperIpLpmDataHeap = L7_NULLPTR;
  }

  avlDeleteAvlTree(&uslShadowIpLpmTreeData);

  /* clear the memory before creating the tree */
  memset((void*)&uslShadowIpLpmTreeData,0,sizeof(avlTree_t));

  if ( pUslShadowIpLpmTreeHeap != L7_NULLPTR )
  {
    memset((void*)pUslShadowIpLpmTreeHeap,0,sizeof(avlTreeTables_t)  * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowIpLpmTreeHeap);
    pUslShadowIpLpmTreeHeap = L7_NULLPTR;
  }

  if ( pUslShadowIpLpmDataHeap != L7_NULLPTR )
  {
    memset((void*)pUslShadowIpLpmDataHeap,0,sizeof(usl_lpm_db_elem_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowIpLpmDataHeap);
    pUslShadowIpLpmDataHeap = L7_NULLPTR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Invalidate the Lpm Db
*
* @param    Flags  {(input)} Type of db to be invalidated.
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_lpm_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t    rc = L7_ERROR;
  L7_uint32  max = HAPI_BROAD_L3_ROUTE_TBL_SIZE+1;

  if (uslIpLpmDbActive == L7_FALSE)
  {
    return rc;
  }

  USL_L3_LPM_DB_LOCK_TAKE();

  do
  {

    if (flags & USL_OPERATIONAL_DB)
    {

      if ((pUslOperIpLpmTreeHeap == L7_NULLPTR) || (pUslOperIpLpmDataHeap == L7_NULLPTR))
        break;

      /* Key is the subnet and mask */
      avlPurgeAvlTree(&uslOperIpLpmTreeData, max);
    }


    if (flags & USL_SHADOW_DB)
    {
      if ((pUslShadowIpLpmTreeHeap == L7_NULLPTR) || (pUslShadowIpLpmDataHeap == L7_NULLPTR))
        break;

      /* Key is the subnet and mask */
      avlPurgeAvlTree(&uslShadowIpLpmTreeData, max);

    }

    rc = L7_SUCCESS;

  } while(0);


  USL_L3_LPM_DB_LOCK_GIVE();

  return rc;
}

/*********************************************************************
* @purpose  Set the Lpm Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_l3_lpm_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_L3_LPM_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    uslIpLpmTreeHandle = &uslOperIpLpmTreeData;
  }
  else if (dbType == USL_SHADOW_DB)
  {
    uslIpLpmTreeHandle = &uslShadowIpLpmTreeData;
  }

  USL_L3_LPM_DB_LOCK_GIVE();

  return;
}

void usl_lpm_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8                uslStr[128];
  usl_lpm_db_elem_t        elem, nextElem;
  uslDbElemInfo_t          searchElem, elemInfo;
  avlTree_t               *dbHandle;
  L7_uint32                entryCount = 0;

  if (uslIpLpmDbActive == L7_FALSE)
  {

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L3 Lpm database is not active\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L3 Lpm database is active\n");
  }

  dbHandle = usl_lpm_db_handle_get(dbType);
  if (dbHandle != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Dumping %s \n",
                  usl_db_type_name_get(dbType));
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "%s is not present\n",
                   usl_db_type_name_get(dbType));
  }

  memset(&elem, 0, sizeof(elem));
  memset(&nextElem, 0, sizeof(nextElem));

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_lpm_db_elem(dbType, USL_DB_NEXT_ELEM,
                             searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_lpm_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  uslStr);
    memcpy(searchElem.dbElem, elemInfo.dbElem, sizeof(usl_lpm_db_elem_t));
    entryCount++;
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Total number of entries in the table: %d\n",
                entryCount);

  return;
}

/*********************************************************************
* @purpose  Given a dbType, return the db Handle.
*
* @params   dbType {(input)} OPER/SHADOW/CURRENT
*
* @returns  Pointer to db Handle
*
* @notes    none
*
* @end
*********************************************************************/
avlTree_t *usl_l3_host_db_handle_get(USL_DB_TYPE_t dbType)
{
  avlTree_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = &uslOperIpHostTreeData;
      break;

    case USL_SHADOW_DB:
      dbHandle = &uslShadowIpHostTreeData;
      break;

    case USL_CURRENT_DB:
      dbHandle = uslIpHostTreeHandle;
      break;

    default:
      break;
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Get the size of a Host Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_l3_host_db_elem(void)
{
  return sizeof(usl_l3_host_db_elem_t);
}

/*********************************************************************
* @purpose  Print the contents of a lpm db element in
*           specified buffer
*
* @params   elem     {(input)}  Element to be printed
*           buffer   {(input)}  buffer pointer
*           size     {(input)}  size of buffer
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
void usl_print_l3_host_db_elem(void *item, L7_uchar8 *buffer,
                               L7_uint32 size)
{
  L7_char8                addr_str[64];
  usl_l3_host_db_elem_t  *elem = item;

  USL_L3_NTOP(((elem->bcm_data.l3a_flags & BCM_L3_IP6)
                ? L7_AF_INET6
                : L7_AF_INET),
                (void *)&elem->bcm_data.hostKey.addr.l3a_ip6_addr,
                addr_str,
                sizeof(addr_str));

  osapiSnprintf(buffer, size,
                "IP=%s\n", addr_str);


  osapiSnprintfcat(buffer, size,
                   "L3 intf=%d, Flags=%x\n",
                   elem->bcm_data.l3a_intf,
                   elem->bcm_data.l3a_flags);


  return;
}

/*********************************************************************
* @purpose  Search element in host db and return pointer to it
*
* @params   dbType     {(input)} Db type (Oper or Shadow)
*           flags      {(input)} USL_EXACT_ELEM or USL_NEXT_ELEM
*           searchInfo {(input)} Element to be searched
*           elemInfo   {(output)} Found element info
*
* @returns  BCM_E_NONE: Element found
*           BCM_E_NOT_FOUND: Element not found
*
* @notes    Internal API. Db lock must be held. Returns pointer
*           to the element.
*
* @end
*********************************************************************/
static L7_int32 usl_search_l3_host_db_elem(USL_DB_TYPE_t dbType,
                                           L7_uint32 flags,
                                           uslDbElemInfo_t searchInfo,
                                           uslDbElemInfo_t *elemInfo)
{
  avlTree_t                 *dbHandle = L7_NULLPTR;
  L7_int32                   rv = BCM_E_NOT_FOUND;
  L7_uchar8                  traceStr[128];


  osapiSnprintf(traceStr, sizeof(traceStr),
                "Get Elem: Flags %d \n",
                flags);
  usl_trace_sync_detail(USL_L3_HOST_DB_ID, traceStr);


  elemInfo->dbElem = L7_NULLPTR;
  elemInfo->elemIndex = USL_INVALID_DB_INDEX;

  do
  {

    dbHandle = usl_l3_host_db_handle_get(dbType);

    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Get next */
    if (flags == USL_DB_NEXT_ELEM)
    {
      elemInfo->dbElem = avlSearchLVL7(dbHandle, searchInfo.dbElem, AVL_NEXT);
    }
    else if (flags == USL_DB_EXACT_ELEM)
    {
      elemInfo->dbElem = avlSearchLVL7(dbHandle, searchInfo.dbElem, AVL_EXACT);
    }

    if (elemInfo->dbElem == L7_NULLPTR)
    {
      rv = BCM_E_NOT_FOUND;
    }
    else
    {
      rv = BCM_E_NONE;
    }

  } while (0);


  return rv;
}

/*********************************************************************
* @purpose  Get info about an element from the Db
*
* @params   dbType        {(input)}  dbType to be searched
*           flags         {(input)}  EXACT or GET_NEXT
*           searchInfo    {(input)}  Information about element to be searched
*           elemInfo      {(output)} Storage for element & index
*
* @returns  BCM error code
*
* @notes    Caller must allocate memory to copy the db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_l3_host_db_elem(USL_DB_TYPE_t     dbType,
                                 L7_uint32         flags,
                                 uslDbElemInfo_t   searchElem,
                                 uslDbElemInfo_t  *elemInfo)
{
  L7_int32         rv = BCM_E_NONE;
  uslDbElemInfo_t  hostInfo;

  if (!uslIpHostDbActive)
  {
    return rv;
  }

  hostInfo.dbElem = L7_NULLPTR;
  hostInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_L3_HOST_DB_LOCK_TAKE();

  rv = usl_search_l3_host_db_elem(dbType, flags, searchElem, &hostInfo);
  if (rv == BCM_E_NONE)
  {
    memcpy(elemInfo->dbElem, hostInfo.dbElem, sizeof(usl_l3_host_db_elem_t));
  }

  USL_L3_HOST_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Delete a given element from Egr Nhop Db
*
* @params   dbType   {(input)}
*           elemInfo {(input)}
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_l3_host_db_elem(USL_DB_TYPE_t   dbType,
                                    uslDbElemInfo_t elemInfo)
{
  avlTree_t                   *dbHandle;
  usl_l3_host_db_elem_t       *elem;
  L7_int32                     rv = BCM_E_NONE;

  if (!uslIpHostDbActive)
  {
    return rv;
  }

  USL_L3_HOST_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_l3_host_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Delete the elem */
    elem = avlDeleteEntry(dbHandle, elemInfo.dbElem);
    if (elem == L7_NULLPTR)
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);

  USL_L3_HOST_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Add a host entry in USL Db
*
* @param    dbType {(input)} db type to modify
* @param    info   {(input)} Host info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_host_add(USL_DB_TYPE_t dbType, usl_bcm_l3_host_t *info)
{
  int                        rv = BCM_E_NONE;
  L7_uint32                  now;
  usl_l3_host_db_elem_t     data;
  usl_l3_host_db_elem_t    *pData;
  avlTree_t                *hostTree;

  if (uslIpHostDbActive == L7_FALSE)
  {
    return rv;
  }

  hostTree = usl_l3_host_db_handle_get(dbType);
  if (hostTree == L7_NULLPTR)
  {
    return BCM_E_FAIL;
  }

  USL_L3_HOST_DB_LOCK_TAKE();

  memset(&data, 0, sizeof(data));

  memcpy(&data.bcm_data, info, sizeof(*info));

  /* No need to save the BCM_L3_WITH_ID/BCM_L3_REPLACE flags as they
  ** are operational commands
  */
  data.bcm_data.l3a_flags &= ~(BCM_L3_WITH_ID | BCM_L3_REPLACE);


  now = osapiUpTimeRaw();
  data.lastHitSrc = now;
  data.lastHitDst = now;
  data.isValid = L7_TRUE;

  pData = avlInsertEntry(uslIpHostTreeHandle,&data);
  if (pData == &data)
  {
    rv = BCM_E_FAIL;
    USL_LOG_ERROR("USL: Error adding ARP/NDP entry \n");
  }
  else if (pData != L7_NULLPTR)
  {
    rv = BCM_E_EXISTS;

    /* remove the entry and re-add the entry */
    pData = avlDeleteEntry(uslIpHostTreeHandle,&data);

    pData = avlInsertEntry(uslIpHostTreeHandle,&data);
  }

  USL_L3_HOST_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a host entry from USL Db
*
* @param    dbType {(input)} db type to modify
* @param    info   {(input)} Host info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_host_delete(USL_DB_TYPE_t dbType, usl_bcm_l3_host_t *info)
{
  int                      rv = BCM_E_NONE;
  usl_l3_host_db_elem_t    data;
  uslDbElemInfo_t          elemInfo;

  if (uslIpHostDbActive == L7_FALSE)
  {
    return rv;
  }

  memset(&data, 0, sizeof(data));
  memcpy(&data.bcm_data, info, sizeof(*info));
  elemInfo.dbElem = &data;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  return usl_delete_l3_host_db_elem(dbType, elemInfo);
}

/*********************************************************************
* @purpose  Create a l3 host at Usl Bcm layer from the given
*           db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_create_l3_host_db_elem_bcm(void *item)
{
  L7_int32                       rv;
  usl_l3_host_db_elem_t         *elem = item;

  do
  {
    rv = usl_bcm_l3_host_add(&(elem->bcm_data));

    if (rv == BCM_E_FULL)
    {
      /* If there was a hash collision, treat the operation as a success
         for the immediate code path. On the mgmt unit, given an indication
         to HAPI that the HW insert failed so that the HAPI retry mechanism
         can take effect. */
      rv = BCM_E_NONE;

      usl_db_sync_failure_notify_callback(USL_L3_HOST_DB_ID, &elem->bcm_data);
    }

  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Delete host at the Usl Bcm layer from the
*           given db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_l3_host_db_elem_bcm(void *item)
{
  L7_int32                   rv;
  usl_l3_host_db_elem_t     *elem = item;

  do
  {
    rv = usl_bcm_l3_host_delete(&(elem->bcm_data));

  } while (0);

  return rv;
}


/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.

* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_l3_host_db_elem_bcm(void *shadowDbItem, void *operDbItem)
{
  L7_int32                  rv = BCM_E_NONE;
  usl_l3_host_db_elem_t    *shadowElem = shadowDbItem;
  usl_l3_host_db_elem_t    *operElem = operDbItem;
  usl_bcm_l3_host_t        hostData;

  memset(&hostData, 0, sizeof(hostData));

  if (memcmp(&(shadowElem->bcm_data), &(operElem->bcm_data),
             sizeof(shadowElem->bcm_data)) != 0)
  {
    memcpy(&hostData, &(shadowElem->bcm_data), sizeof(shadowElem->bcm_data));
    hostData.l3a_flags |= BCM_L3_REPLACE;

    rv = usl_bcm_l3_host_add(&hostData);
  }

  return rv;

}


static L7_RC_t usl_l3_host_db_alloc(avlTree_t *hostTree,
                                    avlTreeTables_t **hostTreeHeap,
                                    usl_l3_host_db_elem_t  **hostDataHeap)
{
  L7_uint32    max = HAPI_BROAD_L3_HOST_TBL_SIZE;
  L7_RC_t      rc = L7_SUCCESS;

  *hostTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                              sizeof(avlTreeTables_t) * max);
   if (*hostTreeHeap == L7_NULLPTR )
   {
     rc = L7_FAILURE;
     return rc;
   }

   *hostDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                               sizeof(usl_l3_host_db_elem_t) * max);
   if (*hostDataHeap == L7_NULLPTR )
   {
     rc = L7_FAILURE;
     return rc;
   }

   /* clear the memory before creating the tree */
   memset((void*)hostTree, 0, sizeof(avlTree_t));
   memset((void*)*hostTreeHeap, 0, sizeof(avlTreeTables_t)  * max);
   memset((void*)*hostDataHeap, 0, sizeof(usl_l3_host_db_elem_t) * max);

   /* key is the host address */
   avlCreateAvlTree(hostTree, *hostTreeHeap, *hostDataHeap,
                    max, sizeof(usl_l3_host_db_elem_t), 0x10,
                    sizeof(usl_bcm_host_key_t));

   rc = L7_SUCCESS;

   return rc;

}
/*********************************************************************
* @purpose  Create the Host Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/

L7_RC_t usl_l3_host_db_init()
{
  L7_RC_t            rc = L7_ERROR;
  uslDbSyncFuncs_t   hostDbFuncs;

  memset(&hostDbFuncs, 0, sizeof(hostDbFuncs));


  do
  {

    /* Allocate db only on Stackables */
    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {
      uslIpHostDbSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
      if ( uslIpHostDbSema == L7_NULLPTR ) break;

      if (usl_l3_host_db_alloc(&uslOperIpHostTreeData,
                               &uslOperIpHostTreeHeap,
                               &uslOperIpHostDataHeap) != L7_SUCCESS)
      {
        break;
      }

      /* Allocate shadow db for NSF feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                                L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {
        if (usl_l3_host_db_alloc(&uslShadowIpHostTreeData,
                                 &uslShadowIpHostTreeHeap,
                                 &uslShadowIpHostDataHeap) != L7_SUCCESS)
        {
          break;
        }

      }

      /* Register the sync routines */
      hostDbFuncs.get_size_of_db_elem =  usl_get_size_of_l3_host_db_elem;
      hostDbFuncs.get_db_elem = usl_get_l3_host_db_elem;
      hostDbFuncs.delete_elem_from_db =  usl_delete_l3_host_db_elem;
      hostDbFuncs.print_db_elem = usl_print_l3_host_db_elem;
      hostDbFuncs.create_usl_bcm = usl_create_l3_host_db_elem_bcm;
      hostDbFuncs.delete_usl_bcm = usl_delete_l3_host_db_elem_bcm;
      hostDbFuncs.update_usl_bcm = usl_update_l3_host_db_elem_bcm;


      /* Not registering optional routines as the db element is deterministic
      ** i.e no pointer member elements
      */
      hostDbFuncs.alloc_db_elem = L7_NULLPTR;
      hostDbFuncs.copy_db_elem  = L7_NULLPTR;
      hostDbFuncs.free_db_elem  = L7_NULLPTR;
      hostDbFuncs.pack_db_elem  = L7_NULLPTR;
      hostDbFuncs.unpack_db_elem = L7_NULLPTR;

      if (usl_db_sync_func_table_register(USL_L3_HOST_DB_ID,
                                          &hostDbFuncs) != L7_SUCCESS)
      {
        L7_LOG_ERROR(0);
      }

      /* Point the Db handle to Operational table */
      uslIpHostTreeHandle = &uslOperIpHostTreeData;

      uslIpHostDbActive = L7_TRUE;

    } /* End if stacking */

    rc = L7_SUCCESS;

  } while(0);

  return rc;
}

/*********************************************************************
* @purpose  Destroy the Host Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/

L7_RC_t usl_l3_host_db_fini()
{
  L7_uint32 max = HAPI_BROAD_L3_HOST_TBL_SIZE;

  uslIpHostDbActive = L7_FALSE;
  uslIpHostTreeHandle = L7_NULLPTR;

  if ( uslIpHostDbSema != L7_NULLPTR )
  {
    osapiSemaDelete(uslIpHostDbSema );
    uslIpHostDbSema = L7_NULLPTR;
  }

  avlDeleteAvlTree(&uslOperIpHostTreeData);
  /* clear the memory before creating the tree */
  memset((void*)&uslOperIpHostTreeData,0,sizeof(avlTree_t));


  if ( uslOperIpHostTreeHeap != L7_NULLPTR )
  {
    memset((void*)uslOperIpHostTreeHeap,0,sizeof(avlTreeTables_t)  * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, uslOperIpHostTreeHeap);
    uslOperIpHostTreeHeap = L7_NULLPTR;
  }

  if ( uslOperIpHostDataHeap != L7_NULLPTR )
  {
    memset((void*)uslOperIpHostDataHeap,0,sizeof(usl_l3_host_db_elem_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, uslOperIpHostDataHeap);
    uslOperIpHostDataHeap = L7_NULLPTR;
  }

  avlDeleteAvlTree(&uslShadowIpHostTreeData);
  /* clear the memory before creating the tree */
  memset((void*)&uslShadowIpHostTreeData,0,sizeof(avlTree_t));


  if ( uslShadowIpHostTreeHeap != L7_NULLPTR )
  {
    memset((void*)uslShadowIpHostTreeHeap,0,sizeof(avlTreeTables_t)  * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, uslShadowIpHostTreeHeap);
    uslShadowIpHostTreeHeap = L7_NULLPTR;
  }

  if ( uslShadowIpHostDataHeap != L7_NULLPTR )
  {
    memset((void*)uslShadowIpHostDataHeap,0,sizeof(usl_l3_host_db_elem_t) * max);
    osapiFree(L7_DRIVER_COMPONENT_ID, uslShadowIpHostDataHeap);
    uslShadowIpHostDataHeap = L7_NULLPTR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Invalidate the Host Db
*
* @param    Flags  {(input)} Type of db to be invalidated.
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_host_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t   rc = L7_ERROR;
  L7_uint32 max;

  if (uslIpHostDbActive == L7_FALSE)
  {
    return rc;
  }

  max = HAPI_BROAD_L3_HOST_TBL_SIZE;    /* key is the host address */

  USL_L3_HOST_DB_LOCK_TAKE();

  do
  {
    if (flags & USL_OPERATIONAL_DB)
    {

      if ((uslOperIpHostTreeHeap == L7_NULLPTR) || (uslOperIpHostDataHeap == L7_NULLPTR))
        break;

      avlPurgeAvlTree(&uslOperIpHostTreeData,max);
    }

    if (flags & USL_SHADOW_DB)
    {
      if ((uslShadowIpHostTreeHeap == L7_NULLPTR) || (uslShadowIpHostDataHeap == L7_NULLPTR))
        break;

      avlPurgeAvlTree(&uslShadowIpHostTreeData,max);
    }

    rc = L7_SUCCESS;

  } while (0);

  USL_L3_HOST_DB_LOCK_GIVE();

  return rc;
}

/*********************************************************************
* @purpose  Set the Host Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_l3_host_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_L3_HOST_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    uslIpHostTreeHandle = &uslOperIpHostTreeData;
  }
  else if (dbType == USL_SHADOW_DB)
  {
    uslIpHostTreeHandle = &uslShadowIpHostTreeData;
  }

  USL_L3_HOST_DB_LOCK_GIVE();

  return;
}

void usl_l3_host_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8                    uslStr[128];
  usl_l3_host_db_elem_t        elem, nextElem;
  uslDbElemInfo_t              searchElem, elemInfo;
  avlTree_t                   *dbHandle;
  L7_uint32                    entryCount = 0;

  if (uslIpHostDbActive == L7_FALSE)
  {

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L3 Host database is not active\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L3 Host database is active\n");
  }

  dbHandle = usl_l3_host_db_handle_get(dbType);
  if (dbHandle != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Dumping %s \n",
                  usl_db_type_name_get(dbType));
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "%s is not present\n",
                   usl_db_type_name_get(dbType));
  }

  memset(&elem, 0, sizeof(elem));
  memset(&nextElem, 0, sizeof(nextElem));

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_l3_host_db_elem(dbType, USL_DB_NEXT_ELEM,
                                 searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_l3_host_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  uslStr);
    memcpy(searchElem.dbElem, elemInfo.dbElem, sizeof(usl_l3_host_db_elem_t));
    entryCount++;
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Total number of entries in the table: %d\n",
                entryCount);

  return;
}

/*********************************************************************
* @purpose  Deallocate the IPMC tables, semaphores, ...
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @end
*********************************************************************/
L7_RC_t usl_l3_db_fini()
{
  L7_RC_t rc = L7_ERROR;


  do
  {
    uslL3DbInited = L7_FALSE;

    /*
     * Egress next hop resources
     */
    (void) usl_l3_egr_nhop_db_fini();
    (void) usl_l3_mpath_egr_nhop_db_fini();


    /*
     * Host table resources
     */
    (void)usl_l3_host_db_fini();

    /*
     * LPM table resources
     */
    (void)usl_l3_lpm_db_fini();

    /*
     * Tunnel Initiator table resources
     */
    (void)usl_l3_tunnel_initiator_db_fini();

    /*
     * Tunnel Terminator table resources
     */
    (void)usl_l3_tunnel_terminator_db_fini();

   /*
     * L3 Interface table resources
     */
    (void) usl_l3_intf_db_fini();

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Initialize the L3 Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_db_init()
{
  L7_RC_t rc = L7_ERROR;


  /* L7_ERROR is assumed, therefore, for all error cases, I simply break */

  do
  {
    /*
     * Egress object table resources for individual nhops and multipath nhops
     */
    if (usl_l3_egr_nhop_db_init() != L7_SUCCESS)
    {
      break;
    }

    if (usl_l3_mpath_egr_nhop_db_init() != L7_SUCCESS)
    {
      break;
    }

    /*
     * Host table resources
     */
    if (usl_l3_host_db_init() != L7_SUCCESS)
    {
      break;
    }

    /*
     * LPM table resources
     */
    if (usl_l3_lpm_db_init() != L7_SUCCESS)
    {
      break;
    }

    /*
     * Tunnel Initiator table resources
     */
    if (usl_l3_tunnel_initiator_db_init() != L7_SUCCESS)
    {
      break;
    }

    /*
     * Tunnel Terminator table resources
     */
    if (usl_l3_tunnel_terminator_db_init() != L7_SUCCESS)
    {
      break;
    }

    /*
     * L3 Interface table resources
     */
    if (usl_l3_intf_db_init() != L7_SUCCESS)
    {
      break;
    }


    /* All must be going well if we got here */
    rc = L7_SUCCESS;

  } while ( 0 );

  if (rc != L7_SUCCESS)
  {
    /* failed to initialise; clean it all up */
    usl_l3_db_fini();
  }
  else
  {
    /* initialise successful; mark it so */
    uslL3DbInited = L7_TRUE;

#ifdef L7_STACKING_PACKAGE
    /* create the timer for the host in-use check */
    /* we skew startup time to avoid CPU load stacking with L2 age time */
    osapiTimerAdd((void*)usl_l3host_active_timer,
                  USL_HOST_INUSE_TIMER_POP,
                  0,
                  USL_HOST_INUSE_TIMER_INTERVAL_MSEC-USL_HOST_INUSE_TIMER_INTERVAL_SKEW,
                  &pUslHostInUseTimer );
#endif
  }

  return rc;
}

/*********************************************************************
* @purpose  Clear all entries from the L3 databases
*
* @param    Database types to invalidate
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @notes    Use this function to reset all the tables to the default, no
*           resources will be deallocated
*
* @end
*********************************************************************/
L7_RC_t usl_l3_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t rc = L7_ERROR;

  if (!uslL3DbInited)
    return L7_SUCCESS;

  do
  {

    /*
     * Host table resources
     */
    if (usl_l3_host_db_invalidate(flags) != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "L3 Host Db invalidate failed\n");
      break;
    }

    /*
     * LPM table resources
     */
    if (usl_l3_lpm_db_invalidate(flags) != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "L3 Lpm Db invalidate failed\n");
      break;
    }

    /*
     * Tunnel Initiator table resources
     */
    if (usl_l3_tunnel_initiator_db_invalidate(flags) != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "L3 Tunnel Initiator Db invalidate failed\n");
      break;
    }

    /*
     * Tunnel Terminator table resources
     */
    if (usl_invalidate_l3_tunnel_terminator_db(flags) != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "L3 Tunnel Terminator invalidate failed\n");
      break;
    }

    /*
     * L3 Interface table resources
     */
    if (usl_l3_intf_db_invalidate(flags) != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "L3 Intf Db invalidate failed\n");
      break;
    }

    if (usl_l3_egr_nhop_db_invalidate(flags) != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "L3 Egr Nhop Db invalidate failed\n");
      break;
    }

    if (usl_l3_mpath_egr_nhop_db_invalidate(flags) != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "L3 Mpath Egr Nhop Db invalidate failed\n");
      break;
    }

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Set the L3 current db handle to Operational or Shadow table
*
* @param    dbType  {(input)} Type of db to set
*
* @returns  L7_SUCCESS - if all the ports were deleted
* @returns  L7_FAILURE - An error was encountered during deletion
*
*
* @end
*********************************************************************/
L7_RC_t usl_l3_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  L7_RC_t rc = L7_SUCCESS;

  /* If trying to set the dbHandle to Shadow tables when they are not allocated */
  if ((dbType == USL_SHADOW_DB) &&
      (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_FALSE))
  {
    rc = L7_FAILURE;
    return rc;
  }

  usl_l3_intf_current_db_handle_set(dbType);
  usl_l3_egr_nhop_current_db_handle_set(dbType);
  usl_l3_mpath_egr_nhop_current_db_handle_set(dbType);
  usl_l3_host_current_db_handle_set(dbType);
  usl_l3_lpm_current_db_handle_set(dbType);
  usl_l3_tunnel_initiator_current_db_handle_set(dbType);
  usl_l3_tunnel_terminator_current_db_handle_set(dbType);

  return rc;
}




/*
 * Debug routines for the L3 tables
*/

void usl_l3_table_count_show (void)
{
  sysapiPrintf("\n L3 Nhop count %d\n", avlTreeCount(&uslOperIpEgrNhopTreeData));
  sysapiPrintf("\n L3 Mpath count %d\n", avlTreeCount(&uslOperIpMpathEgrNhopTreeData));
  sysapiPrintf("\n L3 Host count %d\n", avlTreeCount(&uslOperIpHostTreeData));
  sysapiPrintf("\n L3 LPM count %d\n", avlTreeCount(&uslOperIpLpmTreeData));
}





void usl_l3_sem_show()
{
  if (uslL3DbInited)
  {
    sysapiPrintf("Host Sema     = 0x%0.8x\n",uslIpHostDbSema);
    sysapiPrintf("LPM Sema      = 0x%0.8x\n",pUslIpLpmDbSema);
    sysapiPrintf("L3 intf Sema  = 0x%0.8x\n",pUslL3IntfDbSema);
    sysapiPrintf("L3 Egress Sema = 0x%0.8x\n",uslIpEgrNhopDbSema);
    sysapiPrintf("Tunnel Initiator Sema  = 0x%0.8x\n",
                 pUslIpTunnelInitiatorDbSema);
    sysapiPrintf("Tunnel Terminator Sema  = 0x%0.8x\n",
                 pUslIpTunnelTerminatorDbSema);
  }
}

void usl_l3_debug_help()
{
  sysapiPrintf("\n****************** USL L3 DEBUG HELP ***************\n");
  sysapiPrintf("L3 is %s\n",(uslL3DbInited)?"active":"inactive");
  sysapiPrintf("usl_l3_intf_db_dump(dbType) - L3 Intf table\n");
  sysapiPrintf("usl_l3_egr_nhop_db_dump(dbType) - Egress table \n");
  sysapiPrintf("usl_l3_mpath_egr_nhop_db_dump(dbType) - Mpath Egress table\n");
  sysapiPrintf("usl_l3_terminator_db_dump(dbType) - Terminator table\n");
  sysapiPrintf("usl_l3_initiator_db_dump(dbType) - Initiator table\n");
  sysapiPrintf("\nUse dbType %d for Operational table, %d for Shadow table\n",
               USL_OPERATIONAL_DB, USL_SHADOW_DB);
  sysapiPrintf("usl_l3_sem_show() - display all semaphore for l3\n");
}

#ifdef L7_STACKING_PACKAGE
/*********************************************************************
* @purpose  Check for in use hosts; update our table & send messages
*           about such hosts to the manager (if we're not manager).
*           More than one message is automatically sent if the table
*           contains more hosts that have been active than will fit
*           into a single message.
*
* @notes    Currently, this does the whole table in a single burst.
*           It would be possible to spread this out over time by
*           adding a last address handled somewhere (maybe a static
*           or perhaps a pointer to one as a parameter) that could
*           be used for resuming, some logic to prevent table wrap
*           until proper time had passed, and more frequent calls
*           to the timer handler so that the entire table would be
*           processed in the expected time frame.
* @end
*********************************************************************/
void usl_l3host_active_check(void)
{
  usl_l3_host_db_elem_t *pData;          /* pointer to data element from tree */
  usl_l3_host_db_elem_t data;            /* working data element */
  bcm_l3_host_t hostInfo;        /* data workspace for broadcom API */
  L7_uint32 hostFlags;           /* usl_host_inuse_t.entryFlags for host */
  int bcm_unit;                  /* unit index for chips */
  L7_uchar8 *msg;                /* pointer to message buffer */
  unsigned int msgLimit;         /* max number of hosts in message buffer */
  usl_host_inuse_t *msgPtr;   /* pointer to current host in msg buffer */
  L7_uint32 mgmtUnit;            /* management unit ID */
  L7_uint32 thisFpUnit;          /* this stack member's unit ID */
  L7_uint32 now;                 /* the time since boot, in seconds */
  L7_uint32 numElements = 0;     /* number of hosts in msg buffer */
  L7_RC_t rc = L7_SUCCESS;       /* return code workspace */

  /* blank the initial search key */
  memset(&data,0, sizeof(usl_l3_host_db_elem_t));
  /* if we can't get the mgmt unit number, give up */
  if (unitMgrMgrNumberGet(&mgmtUnit) != L7_SUCCESS)
  {
    return;
  }
  /* if we can't get our unit number, give up */
  if (unitMgrNumberGet(&thisFpUnit)  != L7_SUCCESS)
  {
    return;
  }
  /* Claim the host table semaphore */
  USL_L3_HOST_DB_LOCK_TAKE();

  /* we'll need a buffer to send any message; get and prep it */
  msg = usl_tx_buffer_alloc();
  msgLimit = ((usl_tx_buffer_size() - USL_MSG_SIZE_HDR) /
              sizeof(usl_host_inuse_t)) - 1;
  msgLimit /= 4; /* Keep max msg size around 2K */

  *((L7_uint32 *)(&(msg[USL_MSG_TYPE_OFFSET]))) = USL_HOST_INUSE;
  *((L7_uint32 *)(&(msg[USL_MSG_UNIT_OFFSET]))) = thisFpUnit;
  msgPtr = (usl_host_inuse_t*)(&(msg[USL_MSG_DATA_OFFSET]));
  /* get the current uptime */
  now = osapiUpTimeRaw();
  /* traverse the tree for host entries */
  pData = avlSearchLVL7(&uslOperIpHostTreeData, &data ,AVL_NEXT);
  while ( pData )
  {
    /* there's a host in the table we need to examine... */
    /* indicate it's in the tables & set family; other flags clear */
    if (pData->bcm_data.l3a_flags & BCM_L3_IP6)
    {
      hostFlags = L7_USL_HOST_INUSE_FLAG_IPV6 | L7_USL_HOST_INUSE_FLAG_IN;
    }
    else
    {
      hostFlags = L7_USL_HOST_INUSE_FLAG_IPV4 | L7_USL_HOST_INUSE_FLAG_IN;
    }
    for (bcm_unit = 0; bcm_unit < soc_ndev; bcm_unit++)
    {
      /* for all local chips (on this) ... */
      if (!BCM_IS_FABRIC(bcm_unit))
      {
        /* ...that are not fabric, read and clear the entry's flags */
        memset(&hostInfo,0,sizeof(hostInfo));
        hostInfo.l3a_flags = BCM_L3_HIT_CLEAR;
        if (pData->bcm_data.l3a_flags & BCM_L3_IP6)
        {
          hostInfo.l3a_flags |= BCM_L3_IP6;
          memcpy(&(hostInfo.l3a_ip6_addr), &(pData->bcm_data.hostKey.addr.l3a_ip6_addr) ,sizeof(hostInfo.l3a_ip6_addr));
        }
        else
        {
          memcpy(&(hostInfo.l3a_ip_addr), &(pData ->bcm_data.hostKey.addr.l3a_ip_addr), sizeof(bcm_ip_t));
        }
        if (BCM_E_NONE == bcm_l3_host_find(bcm_unit,&hostInfo))
        {
          /* entry was found; mark source and dest hit if appropriate */
          /* update both host table and the bits to be used in an RPC */
          /* some hardware sets both bits (no s/d distinction) */
          if (0 != (hostInfo.l3a_flags & BCM_L3_S_HIT))
          {
            hostFlags |= L7_USL_HOST_INUSE_FLAG_S_HIT;
            pData->bcm_data.l3a_flags |= BCM_L3_S_HIT;
            pData->lastHitSrc = now;
          }
          if (0 != (hostInfo.l3a_flags & BCM_L3_D_HIT))
          {
            hostFlags |= L7_USL_HOST_INUSE_FLAG_D_HIT;
            pData->bcm_data.l3a_flags |= BCM_L3_D_HIT;
            pData->lastHitDst = now;
          }
          /*
          ** could break now if both flags are set, as long as we didn't want
          ** to check for errors on all the entries.  Would use something like
          ** (L7_USL_HOST_INUSE_FLAG_HIT == (hostFlags & L7_USL_HOST_INUSE_FLAG_HIT))
          ** to test for the condition.
          */
        }
        else
        {
          /* no entry found or other problem; mark as hardware error */
          hostFlags |= L7_USL_HOST_INUSE_FLAG_ERR_HW;
        }
      } /* end if (!BCM_IS_FABRIC(bcm_unit)) */
    } /* end for (bcm_unit = 0; bcm_unit < soc_ndev; bcm_unit++) */
    /* if we're not the manager, add to a message */
    if ((thisFpUnit != mgmtUnit) &&
        (0 != (hostFlags & (L7_USL_HOST_INUSE_FLAG_ERR | L7_USL_HOST_INUSE_FLAG_HIT))))
    {
      /* we're not the manager, and this entry has error or hit; add to msg */
      msgPtr->entryFlags = hostFlags;
      if (pData->bcm_data.l3a_flags & BCM_L3_IP6)
      {
        /* copy IPv6 host address */
        memcpy(&(msgPtr->addr.v6),&(pData->bcm_data.hostKey.addr.l3a_ip6_addr),sizeof(bcm_ip6_t));
      }
      else
      {
        /* copy IPv4 host address; pad with zeroes */
        msgPtr->addr.v4 = pData->bcm_data.hostKey.addr.l3a_ip_addr;
        memset(((L7_uchar8*)(&(msgPtr->addr.v6)))+sizeof(bcm_ip_t),0,sizeof(bcm_ip6_t)-sizeof(bcm_ip_t));
      }
      numElements++;
      msgPtr++;
      if (numElements >= msgLimit)
      {
        /* filled the buffer; send the message to the manager */
        *((L7_uint32 *)(&(msg[USL_MSG_ELEM_OFFSET]))) = numElements;
        rc = sysapiHpcMessageSend(L7_DRIVER_COMPONENT_ID, mgmtUnit,
                                  ((void*)msgPtr)-((void*)msg),
                                  msg);
        if (L7_SUCCESS != rc)
        {
          /* the message didn't go for some reason; log it */
          USL_LOG_MSG(USL_BCM_E_LOG,"USL: error %d sending host table hit message to manager",rc);
        }
        /* message was sent or lost; reset to start of buffer for more */
        msgPtr = (usl_host_inuse_t*)(&(msg[USL_MSG_DATA_OFFSET]));
        numElements = 0;
      }
    } /* end if ((not manager) and ((error or hit) flags are set)) */
    pData = avlSearchLVL7(&uslOperIpHostTreeData, pData ,AVL_NEXT);
  } /* end while ( pData ) */
  /*
  ** now, if there are any host entries left in the buffer, this means that we
  ** have not yet sent them, so send them as well.
  */
  if ((numElements > 0) && (thisFpUnit != mgmtUnit))
  {
    /* something in the buffer and not manager; send the msg to the manager */
    *((L7_uint32 *)(&(msg[USL_MSG_ELEM_OFFSET]))) = numElements;
    rc = sysapiHpcMessageSend(L7_DRIVER_COMPONENT_ID, mgmtUnit,
                              ((void*)msgPtr)-((void*)msg), msg);

    if (L7_SUCCESS != rc)
    {
      /* the message didn't go for some reason; log it */
      USL_LOG_MSG(USL_BCM_E_LOG,"USL: error %d sending host table hit message to manager",rc);
    }
  } /* if ((numElements > 0) && (thisFpUnit != mgmtUnit)) */
  /* free the message buffer */
  usl_tx_buffer_free(msg);
  /* no longer in critical section; release host table semaphore & return */
  USL_L3_HOST_DB_LOCK_GIVE();
  return;
}

/*********************************************************************
* @purpose  Process the host in use messages from stack members,
*           on the manager.  If, for some reason, a host is reported
*           as in use, but is not in the manager's table, it is
*           quietly ignored, on the grounds that is was probably
*           recently deleted for some reason.
*
* @param    msg     @{(input)} The message with the hosts
*
* @end
*********************************************************************/
void usl_l3host_active_process(const L7_uchar8 *msg)
{
  L7_uint32 index;                   /* current host number in message */
  L7_uint32 numElem;                 /* number of hosts in message */
  L7_uint32 now;                     /* current time (seconds since boot) */
  usl_l3_host_db_elem_t data;                /* working record for lookup */
  usl_l3_host_db_elem_t *pData;              /* pointer to actual record in tree */
  const usl_host_inuse_t *msgPtr; /* pointer to current host in message */

  /* blank the search key */
  memset(&data,0, sizeof(usl_l3_host_db_elem_t));
  /* figure out how many hosts to process */
  numElem = *(L7_uint32 *) &msg[USL_MSG_ELEM_OFFSET];
  /* get first host to process */
  msgPtr = (usl_host_inuse_t*)(&(msg[USL_MSG_DATA_OFFSET]));
  /* enter host handling critical section */
  USL_L3_HOST_DB_LOCK_TAKE();
  /* get the current uptime */
  now = osapiUpTimeRaw();
  /* update the hosts provided in the buffer */
  for (index = 0; index < numElem; index++)
  {
    /* set up the search key */
    if (L7_USL_HOST_INUSE_FLAG_IPV6 == (L7_USL_HOST_INUSE_FLAG_IPVER & msgPtr->entryFlags))
    {
      memcpy(&(data.bcm_data.hostKey.addr.l3a_ip6_addr),&(msgPtr->addr.v6),sizeof(bcm_ip6_t));
    }
    else
    {
      memset(&(data.bcm_data.hostKey.addr.l3a_ip6_addr),0,sizeof(bcm_ip6_t));
      data.bcm_data.hostKey.addr.l3a_ip_addr = msgPtr->addr.v4;
    }
    /* search for the host entry */
    pData = avlSearchLVL7(&uslOperIpHostTreeData, &data ,AVL_EXACT);
    if (L7_NULL != pData)
    {
      /* found it; update the flags and reference time if appropriate */
      if (0 != (msgPtr->entryFlags & L7_USL_HOST_INUSE_FLAG_HIT))
      {
        /* this host has been hit; update hit time & appropriate flags */
        if (0 != (msgPtr->entryFlags & L7_USL_HOST_INUSE_FLAG_S_HIT))
        {
          /* hit source; set that bit */
          pData->bcm_data.l3a_flags |= BCM_L3_S_HIT;
          pData->lastHitSrc = now;
        }
        if (0 != (msgPtr->entryFlags & L7_USL_HOST_INUSE_FLAG_D_HIT))
        {
          /* hit destination; set that bit */
          pData->bcm_data.l3a_flags |= BCM_L3_D_HIT;
          pData->lastHitDst = now;
        }
      } /* end if (0 != (msgPtr->entryFlags & L7_USL_HOST_INUSE_FLAG_HIT)) */
      /* FIXME -- do something about error report bits for this host? */
    }
    else
    {
      /* not found; log this condition (should be in manager database) */
      L7_uchar8 addr_str[GEN_ADDR_STR_LEN];
      USL_LOG_MSG(USL_INFO_LOG,
                  "*** Host %s F=%08X missing", /* \n */
                  USL_L3_NTOP((L7_USL_HOST_INUSE_FLAG_IPV6==((msgPtr->entryFlags)&L7_USL_HOST_INUSE_FLAG_IPVER))?L7_AF_INET6:L7_AF_INET,
                              &(msgPtr->addr),
                              &addr_str,
                              sizeof(addr_str)),
                  msgPtr->entryFlags);
    }
    /* look at the next host in the message */
    msgPtr++;
  } /* end for (index = 0; index < numElem; index++) */
  /* leave host handling critical section */
  USL_L3_HOST_DB_LOCK_GIVE();
  return;
}

/*********************************************************************
* @purpose  Get hit (and some other) flags about a host from the
*           USL host entry for that host, also get the last hit times
*           for that host.
*
* @param    family        @{(input)}  The address family
* @param    l3_addr       @{(input)}  Pointer to the host L3 address
* @param    flagsPtr      @{(output)} Pointer where to put flags
* @param    dstTimePtr    @{(output)} Pointer where to put dest last hit time
* @param    srcTimePtr    @{(output)} Pointer where to put src last hit time
*
* @returns  L7_TRUE if it found the host & filled in the flags
*           L7_FALSE if it did not find the host
*
* @notes    Does nothing but return L7_FALSE if it can't find the host.
*           Copies l3a_flags from Broadcom host data.
*           Clears the USL copy of the _HIT flags.
*           Only fills in last hit times if appropriate _HIT flag was set.
*
* @end
*********************************************************************/
L7_BOOL usl_l3host_hit_flags_get(const L7_uint32 family,
                                 const L7_uint8  *l3_addr,
                                 L7_uint32 *flagsPtr,
                                 L7_uint32 *dstTimePtr,
                                 L7_uint32 *srcTimePtr)
{
  usl_l3_host_db_elem_t data;          /* search key */
  usl_l3_host_db_elem_t *pData;        /* located table entry */
  L7_BOOL rc = L7_FALSE;       /* return whether entry found */

  /* build the key */
  memset(&data,0,sizeof(usl_l3_host_db_elem_t));
  if (L7_AF_INET6 == family)
  {
    memcpy(&(data.bcm_data.hostKey.addr.l3a_ip6_addr),l3_addr,sizeof(bcm_ip6_t));
  }
  else
  {
    memcpy(&(data.bcm_data.hostKey.addr.l3a_ip_addr),l3_addr,sizeof(bcm_ip_t));
  }
  /* enter host critical section */
  USL_L3_HOST_DB_LOCK_TAKE();
  /* search the tree for this host */
  pData = avlSearchLVL7(&uslOperIpHostTreeData, &data ,AVL_EXACT);
  if (L7_NULL != pData)
  {
    /* okay, found the host; set the flags */
    *flagsPtr = pData->bcm_data.l3a_flags;
    if (0 != (pData->bcm_data.l3a_flags & BCM_L3_S_HIT))
    {
      /* source hit detected; update source last hit time */
      *srcTimePtr = pData->lastHitSrc;
    }
    if (0 != (pData->bcm_data.l3a_flags & BCM_L3_D_HIT))
    {
      /* destination hit detected; update destination last hit time */
      *dstTimePtr = pData->lastHitDst;
    }
    /* clear the hit flags in the host entry */
    pData->bcm_data.l3a_flags &= ~BCM_L3_HIT;
    rc = L7_TRUE;
  } /* end if (pData != L7_NULLPTR) */
  /* leave host critical section */
  USL_L3_HOST_DB_LOCK_GIVE();
  /* return whether we found the host */
  return rc;
}

/*********************************************************************
* @purpose  The timer callback routine for the host in use check
*
* @param    arg1 (unused)
*           arg2 (unused)
*
* @end
*********************************************************************/
static void usl_l3host_active_timer(L7_uint32 arg1, L7_uint32 arg2)
{
  static L7_uchar8 *msg = L7_NULL;    /* pointer to message space */
  L7_RC_t rc;                         /* result workspace */
  L7_uint32 thisFpUnit;               /* the stack member number */

  /* initialise things, but only if not done yet */
  if (L7_NULL == msg)
  {
    /* first time called; allocate a buffer for the message */
    msg = osapiMalloc(L7_DRIVER_COMPONENT_ID,USL_MSG_SIZE_HDR);
  }
  if (L7_NULL != msg)
  {
    /* we have a message buffer; try to get unit number */
    if ((rc = unitMgrNumberGet(&thisFpUnit)) == L7_SUCCESS)
    {
      /* we managed to get out unit number; build the message */
      *(L7_uint32 *) &msg[USL_MSG_TYPE_OFFSET] = USL_HOST_INUSE_TIMER_POP;
      *(L7_uint32 *) &msg[USL_MSG_UNIT_OFFSET] = thisFpUnit;
      *(L7_uint32 *) &msg[USL_MSG_ELEM_OFFSET] = 0;
      /* now send the message to the USL task on this unit */
      rc = usl_worker_task_msg_send(msg, USL_MSG_SIZE_HDR, L7_NO_WAIT);
      if (rc != L7_SUCCESS)
      {
        /* failed to send the message for some reason (queue full?) */
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
                "Failed to send host inuse timer expiry event, rc %d\n",rc);
      }
    } /* end if ((rc = unitMgrNumberGet(&thisFpUnit)) == L7_SUCCESS) */
  } /* end if (L7_NULL != msg) */
  if (uslL3DbInited)
  {
    /* resubmit the timer as long as we're initialised */
    osapiTimerAdd ((void*)usl_l3host_active_timer,
                   arg1,
                   arg2,
                   USL_HOST_INUSE_TIMER_INTERVAL_MSEC,
                   &pUslHostInUseTimer );
  }
  else
  {
    /* not going to request another timer hit because not initialised */
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
            "usl_l3host_active_timer(): not initialised so dropping timer\n");
  }
  return;
}
#endif /* L7_STACKING_PACKAGE*/

/*********************************************************************
* @purpose  Initialize L3 Intf hw id generator
*
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_l3_intf_hw_id_generator_init(void)
{
  L7_uint32 max;

  max = USL_L3_INTF_DB_LEN;

  /* There is currently no API to get the range of instance number that SDK expects.
  ** Until such API is available we use the index from 0 to max number of
  ** L3 Intfs supported by FASTPATH
  */
  uslL3IntfHwIdMin = 0;
  uslL3IntfHwIdMax = max - 1;

  pUslL3IntfHwIdList = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                     sizeof(usl_l3_intf_hw_id_list_t) *
                                     (uslL3IntfHwIdMax + 1));
  if (pUslL3IntfHwIdList == L7_NULLPTR)
    return L7_FAILURE;

  memset(pUslL3IntfHwIdList, 0, sizeof(usl_l3_intf_hw_id_list_t) *
                                                (uslL3IntfHwIdMax + 1));

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Reset L3 Intf hw id generator
*
* @params   none
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_l3_intf_hw_id_generator_reset()
{
  L7_RC_t rc = L7_SUCCESS;

  /* Clear the TrunkHwIdList array */
  memset(pUslL3IntfHwIdList, 0, sizeof(usl_l3_intf_hw_id_list_t) *
                                                  (uslL3IntfHwIdMax + 1));

  return rc;
}

/*********************************************************************
* @purpose  Synchronize the L3 Intf hwId generator with contents of Oper Db
*
* @params   None
*
* @returns  L7_RC_t. Assumes BCMX is suspended.
*
* @end
*********************************************************************/
L7_RC_t usl_l3_intf_hw_id_generator_sync()
{
  usl_l3_intf_db_elem_t searchL3IntfElem, l3IntfElem;
  uslDbElemInfo_t       searchInfo, elemInfo;
  bcm_if_t              intfId;

  memset(&searchL3IntfElem, 0, sizeof(searchL3IntfElem));
  memset(&l3IntfElem, 0, sizeof(l3IntfElem));

  searchInfo.dbElem = (void *) &searchL3IntfElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &l3IntfElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_l3_intf_db_elem(USL_OPERATIONAL_DB, USL_DB_NEXT_ELEM,
                                 searchInfo, &elemInfo) == BCM_E_NONE)
  {
    memcpy(searchInfo.dbElem, elemInfo.dbElem,
           sizeof(usl_l3_intf_db_elem_t));
    memcpy(&(searchInfo.elemIndex), &(elemInfo.elemIndex),
           sizeof(elemInfo.elemIndex));

    intfId = ((usl_l3_intf_db_elem_t *)elemInfo.dbElem)->intfInfo.bcm_data.l3a_intf_id;

    if ((intfId > uslL3IntfHwIdMax) || (intfId < uslL3IntfHwIdMin))
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Found invalid L3 intf-id %d during intf syc\n",
                  intfId);
      continue;
    }


    /* Mark this index as used in HwList */
    pUslL3IntfHwIdList[intfId].used = L7_TRUE;

  }

  return L7_SUCCESS;
}

void usl_l3_intf_hw_id_generator_dump()
{
  L7_uint32 idx, numUsedIdxs = 0;

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "L3 Min Index %d Max Index %d Intf Used Hw Indexes: ",
                uslL3IntfHwIdMin, uslL3IntfHwIdMax);
  for (idx = uslL3IntfHwIdMin; idx <= uslL3IntfHwIdMax; idx++)
  {
    if (pUslL3IntfHwIdList[idx].used == L7_TRUE)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "%d, ", idx);
      numUsedIdxs++;
    }
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\n");

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Number of used Hw Indexes %d \n",
                numUsedIdxs);
  return;
}

/*********************************************************************
* @purpose  Allocate a hw L3 Intf for a given Interface
*
* @param    intfInfo  @{(input)}  L3 Intf info
* @param    index     @{(output)} Hw Index
*
* @returns  BCM_E_NONE: HwId Generated successfully
*           BCM_E_FULL: No valid HwId exists
*
* @end
*********************************************************************/
int usl_l3_intf_hw_id_allocate(usl_bcm_l3_intf_t *intfInfo, bcm_if_t *index)
{
  int                    rv = BCM_E_FULL, dbRv;
  bcm_if_t               idx = uslL3IntfHwIdMin;
  usl_l3_intf_db_elem_t  searchElem, intfElem;
  uslDbElemInfo_t        searchInfo, elemInfo;

  *index = USL_BCM_L3_INTF_INVALID;
  memset(&searchElem, 0, sizeof(searchElem));
  memset(&intfElem, 0, sizeof(intfElem));

  memcpy(&(searchElem.intfInfo), intfInfo, sizeof(searchElem.intfInfo));
  searchInfo.dbElem = &searchElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &intfElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  do
  {
    /* If USL is in warm start state, then search the operational db for this intf.
    ** If group is not found then an unused index is generated.
    */
    if (usl_state_get() == USL_WARM_START_STATE)
    {
      dbRv = usl_get_l3_intf_db_elem(USL_OPERATIONAL_DB, USL_DB_EXACT_ELEM,
                                     searchInfo, &elemInfo);
      if (dbRv == BCM_E_NONE) /* L3 Intf found */
      {
        *index = ((usl_l3_intf_db_elem_t *) elemInfo.dbElem)->intfInfo.bcm_data.l3a_intf_id;

        if ((*index > uslL3IntfHwIdMax) || (*index < uslL3IntfHwIdMin))
        {
          L7_LOG_ERROR(*index);
        }

        /* Mark this index as used in HwIdList */
        pUslL3IntfHwIdList[*index].used = L7_TRUE;
        rv = BCM_E_NONE;
        break;
      }
    }

    for (idx = uslL3IntfHwIdMin; idx <= uslL3IntfHwIdMax; idx++)
    {
      /* Found an unused index */
      if (pUslL3IntfHwIdList[idx].used == L7_FALSE)
      {
        pUslL3IntfHwIdList[idx].used = L7_TRUE;
        rv = BCM_E_NONE;
        *index = idx;
        break;
      }
    }
  } while(0);


  return rv;
}

/*********************************************************************
* @purpose  Free a given L3 Intf index
*
* @param    index   @{(input)} L3 Intf index
*
* @returns  BCM_E_NONE: HwId freed successfully
*           BCM_E_FAIL: HwId could not be freed
*
* @end
*********************************************************************/
int usl_l3_intf_hw_id_free(bcm_if_t index)
{
  int rv = BCM_E_NONE;

  if ((index < uslL3IntfHwIdMin) || (index > uslL3IntfHwIdMax))
  {
    rv = BCM_E_FAIL;
    return rv;
  }
  else
  {
    pUslL3IntfHwIdList[index].used = L7_FALSE;
  }

  return rv;
}

/*********************************************************************
* @purpose  Reserve/Unreserve a given L3 Egr Nhop index
*
* @param    index    @{(input/output)} L3 Intf index
* @param    reserve  @{input)} L7_TRUE: Reserve the id
*                              L7_FALSE: Unreserve the id
*
* @returns  BCM_E_NONE: HwId was reserved/unreserved successfully
*           BCM_E_FAIL: HwId could not be reserved
*
* @end
*********************************************************************/
int usl_l3_egr_nhop_hw_id_reserve(bcm_if_t *index, L7_BOOL reserve)
{
  int rv = BCM_E_NONE;
  int tblIdx;

  if (reserve == L7_FALSE)
  {
    tblIdx = *index/* - BCM_XGS3_DVP_EGRESS_IDX_MIN */;
    if ((tblIdx < uslL3EgrNhopHwIdMin) || (tblIdx > uslL3EgrNhopHwIdMax))
    {
      rv = BCM_E_FAIL;
      return rv;
    }
    else
    {
      pUslL3EgrNhopHwIdList[tblIdx].used = L7_FALSE;
    }
  }

  /* find unused index */
  if (reserve == L7_TRUE)
  {
    for (tblIdx = uslL3EgrNhopHwIdMin; tblIdx <= uslL3EgrNhopHwIdMax; tblIdx++)
    {
      /* Found an unused index */
      if (pUslL3EgrNhopHwIdList[tblIdx].used == L7_FALSE)
      {
        pUslL3EgrNhopHwIdList[tblIdx].used = L7_TRUE;
        *index = tblIdx/* + BCM_XGS3_DVP_EGRESS_IDX_MIN*/;
        rv = BCM_E_NONE;
        break;
      }
    }
  }

  return rv;
}

/*********************************************************************
* @purpose  Initialize egress nhop  hw id generator
*
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_l3_egr_nhop_hw_id_generator_init(void)
{
  L7_uint32 max;

  /* Max number of nhops that can  be created */

  /* +1 for cpu egress nhop, +2 for SDK reserved nhop */
  if (cnfgrIsFeaturePresent(L7_FLEX_WIRELESS_COMPONENT_ID, L7_WIRELESS_L2_CENTTNNL_FEATURE_ID) == L7_TRUE)
  {
    max = HAPI_BROAD_L3_NH_TBL_SIZE + L7_MAX_NUM_CAPWAP_TUNNEL_INTF + 3;
  }
  else
  {
    max = HAPI_BROAD_L3_NH_TBL_SIZE + 3;
  }

  /* There is currently no API to get the range of instance number that SDK expects.
  ** Until such API is available we use the index from 0 to max number of
  ** L3 Intfs supported by FASTPATH
  */
  uslL3EgrNhopHwIdMin = 0;
  uslL3EgrNhopHwIdMax = max - 1;

  pUslL3EgrNhopHwIdList = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                      sizeof(usl_l3_egr_nhop_hw_id_list_t) *
                                      (uslL3EgrNhopHwIdMax + 1));
  if (pUslL3EgrNhopHwIdList == L7_NULLPTR)
    return L7_FAILURE;

  memset(pUslL3EgrNhopHwIdList, 0, sizeof(usl_l3_egr_nhop_hw_id_list_t) *
                                                        (uslL3EgrNhopHwIdMax + 1));

  /* SDK reserves first two nexthops */
  pUslL3EgrNhopHwIdList[0].used = pUslL3EgrNhopHwIdList[1].used = L7_TRUE;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Reset l3 egr nhop hw id generator
*
* @params   none
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_l3_egr_nhop_hw_id_generator_reset()
{
  L7_RC_t rc = L7_SUCCESS;

  /* Clear the HwIdList array */
  memset(pUslL3EgrNhopHwIdList, 0, sizeof(usl_l3_egr_nhop_hw_id_list_t) *
                                                     (uslL3EgrNhopHwIdMax + 1));

  /* SDK reserves first two nexthops */
  pUslL3EgrNhopHwIdList[0].used = pUslL3EgrNhopHwIdList[1].used = L7_TRUE;

  return rc;
}

/*********************************************************************
* @purpose  Synchronize the L3 Egr Nhop hwId generator with contents
*           of Oper Db
*
* @params   None
*
* @returns  L7_RC_t. Assumes BCMX is suspended.
*
* @end
*********************************************************************/
L7_RC_t usl_l3_egr_nhop_hw_id_generator_sync()
{
  usl_egr_nhop_db_elem_t searchElem, nhopElem;
  uslDbElemInfo_t        searchInfo, elemInfo;
  L7_uint32              egrId, tblIdx;

  memset(&nhopElem, 0, sizeof(nhopElem));
  memset(&searchElem, 0, sizeof(searchElem));

  searchInfo.dbElem = (void *) &searchElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nhopElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_l3_egr_nhop_db_elem(USL_OPERATIONAL_DB, USL_DB_NEXT_ELEM,
                                     searchInfo, &elemInfo) == BCM_E_NONE)
  {
    memcpy(searchInfo.dbElem, elemInfo.dbElem,
           sizeof(usl_egr_nhop_db_elem_t));
    memcpy(&(searchInfo.elemIndex), &(elemInfo.elemIndex),
           sizeof(elemInfo.elemIndex));

    egrId = ((usl_egr_nhop_db_elem_t *)elemInfo.dbElem)->egrId;
    tblIdx = egrId - BCM_XGS3_EGRESS_IDX_MIN;
    if ((tblIdx > uslL3EgrNhopHwIdMax) || (tblIdx < uslL3EgrNhopHwIdMin))
    {
      L7_LOG_ERROR(*index);
    }

    /* Mark this index as used in HwIdList */
    pUslL3EgrNhopHwIdList[tblIdx].used = L7_TRUE;

  }

  return L7_SUCCESS;
}

void usl_l3_egr_nhop_hw_id_generator_dump()
{
  L7_uint32 idx, validIdCount = 0;

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "L3 Egr Nhop Min Index %d Max Index %d Used Hw Indexes: ",
                uslL3EgrNhopHwIdMin, uslL3EgrNhopHwIdMax);
  for (idx = uslL3EgrNhopHwIdMin; idx <= uslL3EgrNhopHwIdMax; idx++)
  {
    if (pUslL3EgrNhopHwIdList[idx].used == L7_TRUE)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "%d, ", idx);
      validIdCount++;
    }
  }
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nNumber of used Hw Indexes %d \n",
                validIdCount);

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\n");
  return;
}

/*********************************************************************
* @purpose  Allocate a hw Egr Nhop for a given Interface
*
* @param    intfInfo  @{(input)}  L3 Intf info
* @param    index     @{(output)} Hw Index
*
* @returns  BCM_E_NONE: HwId Generated successfully
*           BCM_E_FULL: No valid HwId exists
*
* @end
*********************************************************************/
int usl_l3_egr_nhop_hw_id_allocate(usl_bcm_l3_egress_t *egressInfo,
                                   bcm_if_t *index)
{
  int                       rv = BCM_E_FAIL, dbRv;
  bcm_if_t                  idx = uslL3EgrNhopHwIdMin, tblIdx;
  usl_egr_nhop_db_elem_t    searchElem, nhopElem;
  uslDbElemInfo_t           searchInfo, nhopInfo;

  memset(&searchElem, 0, sizeof(searchElem));
  memcpy(&(searchElem.egrInfo), egressInfo, sizeof(*egressInfo));
  searchInfo.dbElem = &searchElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  memset(&nhopElem, 0, sizeof(nhopElem));
  nhopInfo.dbElem = &nhopElem;
  nhopInfo.elemIndex = USL_INVALID_DB_INDEX;


  do
  {
    /* If USL is in warm start state, then search the operational db for
    ** this egress nhop.
    ** If it is not found then an unused index is generated.
    */

    if (usl_state_get() == USL_WARM_START_STATE)
    {
      dbRv = usl_get_l3_egr_nhop_db_elem(USL_OPERATIONAL_DB, USL_DB_EXACT_ELEM,
                                         searchInfo, &nhopInfo);

      if (dbRv == BCM_E_NONE) /* L3 Intf found */
      {
        *index = nhopElem.egrId;
        tblIdx = *index - BCM_XGS3_EGRESS_IDX_MIN;
        if ((tblIdx > uslL3EgrNhopHwIdMax) || (tblIdx < uslL3EgrNhopHwIdMin))
        {
          L7_LOG_ERROR(*index);
        }

        /* Mark this index as used in HwIdList */
        pUslL3EgrNhopHwIdList[tblIdx].used = L7_TRUE;
        rv = BCM_E_NONE;
        break;
      }
    }

    for (idx = uslL3EgrNhopHwIdMin; idx <= uslL3EgrNhopHwIdMax; idx++)
    {
      /* Found an unused index */
      if (pUslL3EgrNhopHwIdList[idx].used == L7_FALSE)
      {
        pUslL3EgrNhopHwIdList[idx].used = L7_TRUE;
        *index = idx + BCM_XGS3_EGRESS_IDX_MIN;
        rv = BCM_E_NONE;
        break;
      }
    }


  } while(0);


  return rv;
}

/*********************************************************************
* @purpose  Free a given L3 Egr Nhop index
*
* @param    index   @{(input)} L3 Intf index
*
* @returns  BCM_E_NONE: HwId freed successfully
*           BCM_E_FAIL: HwId could not be freed
*
* @end
*********************************************************************/
int usl_l3_egr_nhop_hw_id_free(bcm_if_t index)
{
  int rv = BCM_E_NONE;
  int tblIdx;

  tblIdx = index - BCM_XGS3_EGRESS_IDX_MIN;

  if ((tblIdx < uslL3EgrNhopHwIdMin) || (tblIdx > uslL3EgrNhopHwIdMax))
  {
    rv = BCM_E_FAIL;
    return rv;
  }
  else
  {
    pUslL3EgrNhopHwIdList[tblIdx].used = L7_FALSE;
  }

  return rv;
}

/*********************************************************************
* @purpose  Initialize mpath egress nhop  hw id generator
*
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_l3_mpath_egr_nhop_hw_id_generator_init(void)
{
  L7_uint32 max, bcm_unit;

  max = HAPI_BROAD_L3_MAX_ECMP_GROUPS;

  /* There is currently no API to get the range of instance
  ** number that SDK expects. Until such API is available
  ** we use the index from 0 to max number of mpath
  ** supported by FASTPATH.
  */

  uslL3MpathEgrNhopHwIdMin = 0;
  uslL3MpathEgrNhopHwIdMax = max - 1;

  pUslL3MpathEgrNhopHwIdList = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                           sizeof(usl_l3_mpath_egr_nhop_hw_id_list_t) *
                                           (uslL3MpathEgrNhopHwIdMax + 1));
  if (pUslL3MpathEgrNhopHwIdList == L7_NULLPTR)
    return L7_FAILURE;

  memset(pUslL3MpathEgrNhopHwIdList, 0,
         sizeof(usl_l3_mpath_egr_nhop_hw_id_list_t) *
         (uslL3MpathEgrNhopHwIdMax + 1));

  /* On platforms that have l3_dynamic_ecmp_group feature, id's should be
   * separated by a value of L7_RT_MAX_EQUAL_COST_ROUTES.
   * On all other platforms, id's are consecutive.
   */
  for (bcm_unit = 0; bcm_unit < soc_ndev; bcm_unit++)
  {
    if (!BCM_IS_FABRIC(bcm_unit))
    {
      if (soc_feature(bcm_unit, soc_feature_l3_dynamic_ecmp_group))
      {
        uslL3MpathEgrNhopHwIdOffset = L7_RT_MAX_EQUAL_COST_ROUTES;
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Reset l3 mpath egr nhop hw id generator
*
* @params   none
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_l3_mpath_egr_nhop_hw_id_generator_reset()
{
  L7_RC_t rc = L7_SUCCESS;

  /* Clear the HwIdList array */
  memset(pUslL3MpathEgrNhopHwIdList, 0,
         sizeof(usl_l3_mpath_egr_nhop_hw_id_list_t) * (uslL3MpathEgrNhopHwIdMax + 1));

  return rc;
}

/*********************************************************************
* @purpose  Synchronize the L3 Mpath Egr Nhop hwId generator with contents
*           of Oper Db
*
* @params   None
*
* @returns  L7_RC_t. Assumes BCMX is suspended.
*
* @end
*********************************************************************/
L7_RC_t usl_l3_mpath_egr_nhop_hw_id_generator_sync()
{
  usl_mpath_egr_nhop_db_elem_t searchElem, nhopElem;
  uslDbElemInfo_t              searchInfo, elemInfo;
  L7_uint32                    egrId, tblIdx;

  memset(&nhopElem, 0, sizeof(nhopElem));
  memset(&searchElem, 0, sizeof(searchElem));

  searchInfo.dbElem = (void *) &searchElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nhopElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_l3_mpath_egr_nhop_db_elem(USL_OPERATIONAL_DB, USL_DB_NEXT_ELEM,
                                           searchInfo, &elemInfo) == BCM_E_NONE)
  {
    memcpy(searchInfo.dbElem, elemInfo.dbElem,
           sizeof(usl_mpath_egr_nhop_db_elem_t));
    memcpy(&(searchInfo.elemIndex), &(elemInfo.elemIndex),
           sizeof(elemInfo.elemIndex));

    egrId = ((usl_mpath_egr_nhop_db_elem_t *)elemInfo.dbElem)->avlKey;
    tblIdx = (egrId - BCM_XGS3_MPATH_EGRESS_IDX_MIN)/(uslL3MpathEgrNhopHwIdOffset);
    if ((tblIdx > uslL3MpathEgrNhopHwIdMax) ||
        (tblIdx < uslL3MpathEgrNhopHwIdMin))
    {
      L7_LOG_ERROR(*index);
    }


   HAPI_BROAD_L3_BCMX_DBG(BCM_E_NONE, "Populating id %d from the Oper table in Hw Id list\n",
                          tblIdx);
    /* Mark this index as used in HwIdList */
    pUslL3MpathEgrNhopHwIdList[tblIdx].used = L7_TRUE;

  }

  return L7_SUCCESS;
}


void usl_l3_mpath_egr_nhop_hw_id_generator_dump()
{
  L7_uint32 idx, numUsedIdxs = 0;

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "L3 Mpath Egr Nhop Min Index %d Max Index %d Used Hw Indexes: ",
                uslL3MpathEgrNhopHwIdMin, uslL3MpathEgrNhopHwIdMax);
  for (idx = uslL3MpathEgrNhopHwIdMin; idx <= uslL3MpathEgrNhopHwIdMax; idx++)
  {
    if (pUslL3MpathEgrNhopHwIdList[idx].used == L7_TRUE)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "%d, ", idx);
      numUsedIdxs++;
    }
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\n");

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Number of used Hw Indexes %d \n",
                numUsedIdxs);

  return;
}

/*********************************************************************
* @purpose  Allocate a hw Mpath Egr Nhop
*
* @param    intf_count   @{(input)} Number of egress nhops
* @param    intf_array  @{(input)} List of egress nhops
* @param    index      @{(output)} Hw Index
*
* @returns  BCM_E_NONE: HwId Generated successfully
*           BCM_E_FULL: No valid HwId exists
*
* @end
*********************************************************************/
int usl_l3_mpath_egr_nhop_hw_id_allocate(L7_uint32 intf_count,
                                         bcm_if_t *intf_array,
                                         bcm_if_t *index)
{
  int                          rv = BCM_E_FULL, dbRv;
  usl_mpath_egr_nhop_db_elem_t mpathNhop;
  bcm_if_t                     idx = uslL3MpathEgrNhopHwIdMin, tblIdx;

  memset(&mpathNhop, 0, sizeof(mpathNhop));

  do
  {
    /* If USL is in warm start state, then search the operational db for
    ** this mpath nhop.
    ** If it is not found then an unused index is generated.
    */
    if (usl_state_get() == USL_WARM_START_STATE)
    {
      dbRv = usl_db_l3_egress_multipath_get(USL_OPERATIONAL_DB, intf_count,
                                            intf_array, &mpathNhop);
      if (dbRv == BCM_E_NONE) /* Found */
      {
        *index = mpathNhop.avlKey;
        tblIdx = (*index - BCM_XGS3_MPATH_EGRESS_IDX_MIN)/(uslL3MpathEgrNhopHwIdOffset);
        if ((tblIdx > uslL3MpathEgrNhopHwIdMax) ||
            (tblIdx < uslL3MpathEgrNhopHwIdMin))
        {
          L7_LOG_ERROR(*index);
        }

        /* Mark this index as used in HwIdList */
        pUslL3MpathEgrNhopHwIdList[tblIdx].used = L7_TRUE;
        rv = BCM_E_NONE;
        break;
      }
    }

    for (idx = uslL3MpathEgrNhopHwIdMin; idx <= uslL3MpathEgrNhopHwIdMax; idx++)
    {
      /* Found an unused index */
      if (pUslL3MpathEgrNhopHwIdList[idx].used == L7_FALSE)
      {
        pUslL3MpathEgrNhopHwIdList[idx].used = L7_TRUE;
        *index = (idx * uslL3MpathEgrNhopHwIdOffset) + BCM_XGS3_MPATH_EGRESS_IDX_MIN;
        rv = BCM_E_NONE;
        break;
      }
    }
  } while(0);


  return rv;
}

/*********************************************************************
* @purpose  Free a given L3 Mpath Egr Nhop index
*
* @param    index   @{(input)} Mpath Intf index
*
* @returns  BCM_E_NONE: HwId freed successfully
*           BCM_E_FAIL: HwId could not be freed
*
* @end
*********************************************************************/
int usl_l3_mpath_egr_nhop_hw_id_free(bcm_if_t index)
{
  int rv = BCM_E_NONE;
  int tblIdx;

  tblIdx = (index - BCM_XGS3_MPATH_EGRESS_IDX_MIN)/(uslL3MpathEgrNhopHwIdOffset);

  if ((tblIdx < uslL3MpathEgrNhopHwIdMin) || (tblIdx > uslL3MpathEgrNhopHwIdMax))
  {
    rv = BCM_E_FAIL;
    return rv;
  }
  else
  {
    pUslL3MpathEgrNhopHwIdList[tblIdx].used = L7_FALSE;
  }

  return rv;
}

/*********************************************************************
* @purpose  Initialize the Layer 3 hw id generators
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_hw_id_generator_init()
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    if (usl_l3_intf_hw_id_generator_init() != L7_SUCCESS)
    {
      break;
    }

    if (usl_l3_egr_nhop_hw_id_generator_init() != L7_SUCCESS)
    {
      break;
    }

    if (usl_l3_mpath_egr_nhop_hw_id_generator_init() != L7_SUCCESS)
    {
      break;
    }


    rc = L7_SUCCESS;
  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Reset the Layer 3 hw id generators
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_l3_hw_id_generator_reset()
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    if (usl_l3_intf_hw_id_generator_reset() != L7_SUCCESS)
    {
      break;
    }

    if (usl_l3_egr_nhop_hw_id_generator_reset() != L7_SUCCESS)
    {
      break;
    }

    if (usl_l3_mpath_egr_nhop_hw_id_generator_reset() != L7_SUCCESS)
    {
      break;
    }

    rc = L7_SUCCESS;
  } while(0);

  return rc;
}


/*********************************************************************
* @purpose  Reset and populate the Layer3 Hw id generator from USL
*           Operational Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    Called during warm start to mark the hw indexes in Operational
*           Db as used.
*
* @end
*********************************************************************/
L7_RC_t usl_l3_hw_id_generator_sync()
{
  /* Reset the id generators first and then sync */
  usl_l3_hw_id_generator_reset();

  usl_l3_egr_nhop_hw_id_generator_sync();

  usl_l3_mpath_egr_nhop_hw_id_generator_sync();

  usl_l3_intf_hw_id_generator_sync();

  return L7_SUCCESS;
}

#ifdef L7_STACKING_PACKAGE
/*********************************************************************
* @purpose  Used to clean up the HW L3 tables of any references to
*           modids of the old manager unit. This function is called
*           by each stack unit after a failover occurs.
*           This function also updates nexthop entries that point
*           to the CPU so that they point to the CPU port of the
*           new manager.
*
* @param    *missing_mod_ids      @{(input)} array of missing modIds
*            missing_mod_id_count @{(input)} count of missing modIds
*            manager_left_stack   @{(input)} indicates if this function
*                                            is called as a result of
*                                            the manager leaving the stack.
*            old_cpu_modid        @{(input)} modid of old CPU port.
*            old_cpu_modport      @{(input)} modport of old CPU.
*            new_cpu_modid        @{(input)} modid of new CPU port.
*            new_cpu_modport      @{(input)} modport of new CPU.
*
* @returns  BCM_E_NONE
*           BCM_E_FAIL
*
* @end
*********************************************************************/
int usl_l3_db_dataplane_cleanup(L7_int32 *missing_mod_ids,
                                L7_uint32 missing_mod_id_count,
                                L7_BOOL   manager_left_stack,
                                L7_uint32 old_cpu_modid,
                                L7_uint32 old_cpu_modport,
                                L7_uint32 new_cpu_modid,
                                L7_uint32 new_cpu_modport)
{
  int                           rv = BCM_E_NONE;
  avlTree_t                    *mpathEgrNhopDbHandle = L7_NULLPTR;
  avlTree_t                    *egrNhopDbHandle = L7_NULLPTR;
  usl_mpath_egr_nhop_db_elem_t  mpathEgrNhop;
  usl_egr_nhop_db_elem_t        egrNhop;
  usl_mpath_egr_nhop_db_elem_t *mpathEgrNhopPtr;
  usl_egr_nhop_db_elem_t       *egrNhopPtr;
  L7_uchar8                    *invalidNhops;
  L7_uint32                     maxNhops = HAPI_BROAD_L3_NH_TBL_SIZE + 1;/* +1 for link local ipv6 route */
  L7_uint32                     mpathEgrNhopIdx, egrNhopIdx, modIdIdx;
  bcm_if_t                      mpathEgrNhopIntf[L7_RT_MAX_EQUAL_COST_ROUTES];
  L7_uint32                     mpathEgrNhopCount;
  L7_BOOL                       mpathEgrNhopChanged;
  L7_BOOL                       nhopIsInvalid;
  usl_l3_intf_db_elem_t        *l3IntfDbHandle;
  L7_uint32                     maxL3Intfs = USL_L3_INTF_DB_LEN;
  L7_enetMacAddr_t              L7_ENET_VRRP_MAC_ADDR = {{0x00, 0x00, 0x5E, 0x00, 0x01, 0x00}};
  L7_uint32                     i;
  L7_uint32                     startTime, endTime;

  USL_L3_EGR_NHOP_DB_LOCK_TAKE();

  egrNhopDbHandle      = usl_l3_egr_nhop_db_handle_get(USL_OPERATIONAL_DB);
  mpathEgrNhopDbHandle = usl_l3_mpath_egr_nhop_db_handle_get(USL_OPERATIONAL_DB);
  if ((egrNhopDbHandle == L7_NULLPTR) || (mpathEgrNhopDbHandle == L7_NULLPTR))
  {
    USL_L3_EGR_NHOP_DB_LOCK_GIVE();
    return BCM_E_FAIL;
  }

  /* Iterate through all egress nexthop objects. Update entries that refer to CPU port of old manager. */
  /* Additionally, keep track of which entries use modids that are no longer valid. */
  invalidNhops = osapiMalloc(L7_DRIVER_COMPONENT_ID, maxNhops);
  if (invalidNhops == L7_NULLPTR)
  {
    USL_L3_EGR_NHOP_DB_LOCK_GIVE();
    return BCM_E_MEMORY;
  }

  startTime = osapiTimeMillisecondsGet();

  memset(invalidNhops, 0, maxNhops);

  memset(&egrNhop, 0, sizeof(egrNhop));
  egrNhopPtr = avlSearchLVL7(egrNhopDbHandle, &egrNhop, AVL_NEXT);
  while (egrNhopPtr != L7_NULLPTR)
  {
    memcpy(&egrNhop, egrNhopPtr, sizeof(egrNhop));

    nhopIsInvalid = L7_FALSE;

    /* Track entries w/ invalid modids. */
    if ((egrNhopPtr->egrInfo.bcm_data.flags & BCM_L3_TGID) == 0)
    {
      for (modIdIdx = 0; modIdIdx < missing_mod_id_count; modIdIdx++)
      {
        if (egrNhopPtr->egrInfo.bcm_data.module == missing_mod_ids[modIdIdx])
        {
            /* Mark this next hop as invalid. */
            nhopIsInvalid = L7_TRUE;
          }
        }
      }
    else if ((egrNhopPtr->egrInfo.bcm_data.flags & BCM_L3_TGID) == BCM_L3_TGID)
    {
      /* If the nexthop is a LAG, check to see if the LAG still exists. It may have
         been cleaned (destroyed) as part of the cleanup process if all of the
         member ports are gone. */
      if (usl_trunk_db_tgid_validate(USL_OPERATIONAL_DB, egrNhopPtr->egrInfo.bcm_data.trunk) != BCM_E_NONE)
      {
        /* Mark this next hop as invalid. */
        nhopIsInvalid = L7_TRUE;
      }
    }

    if (nhopIsInvalid)
    {
      egrNhopIdx = egrNhopPtr->egrId - BCM_XGS3_EGRESS_IDX_MIN;
      if (egrNhopIdx < maxNhops)
      {
        invalidNhops[egrNhopIdx] = 1;
      }
      else
      {
        L7_LOG_ERROR(egrNhopIdx);
      }
    }

    egrNhopPtr = avlSearchLVL7(egrNhopDbHandle, &egrNhop, AVL_NEXT);
  }

  /* Iterate through all multipath egress objects. Remove references to missing modids. */
  memset(&mpathEgrNhop, 0, sizeof(mpathEgrNhop));
  mpathEgrNhopPtr = avlSearchLVL7(mpathEgrNhopDbHandle, &mpathEgrNhop, AVL_NEXT);
  while (mpathEgrNhopPtr != L7_NULLPTR)
  {
    memcpy(&mpathEgrNhop, mpathEgrNhopPtr, sizeof(mpathEgrNhop));

    /* Start w/ an empty intf array and build it back up w/ valid intfs. */
    memset(mpathEgrNhopIntf, 0, sizeof(mpathEgrNhopIntf));
    mpathEgrNhopCount   = 0;
    mpathEgrNhopChanged = L7_FALSE;

    for (mpathEgrNhopIdx = 0; mpathEgrNhopIdx < mpathEgrNhopPtr->intfCount; mpathEgrNhopIdx++)
    {
      egrNhopIdx = mpathEgrNhopPtr->intf[mpathEgrNhopIdx] - BCM_XGS3_EGRESS_IDX_MIN;
      if (egrNhopIdx < maxNhops)
      {
        if (invalidNhops[egrNhopIdx] == 1)
        {
          mpathEgrNhopChanged = L7_TRUE;
        }
        else
        {
          mpathEgrNhopIntf[mpathEgrNhopCount] = mpathEgrNhopPtr->intf[mpathEgrNhopIdx];
          mpathEgrNhopCount++;
        }
      }
      else
      {
        L7_LOG_ERROR(egrNhopIdx);
      }
    }

    if (mpathEgrNhopChanged == L7_TRUE)
    {
      if (mpathEgrNhopCount > 0)
      {
        rv = usl_bcm_l3_egress_multipath_create(BCM_L3_WITH_ID | BCM_L3_REPLACE,
                                                mpathEgrNhopCount,
                                                mpathEgrNhopIntf,
                                                &mpathEgrNhopPtr->avlKey);
        if (rv != BCM_E_NONE)
        {
          USL_LOG_MSG(USL_BCM_E_LOG,
                      "USL: Failed to create l3 egress mpath object id 0x%x, rv = %d\n",
                      mpathEgrNhopPtr->avlKey, rv);
        }
      }
      else
      {
        /* It is possible that we cannot destroy the multipath nexthop
           if there are routes pointing to it. In that case, we need to ensure
           the entry is not removed from the USL DB if the entry cannot be
           removed from the HW. This will allow us to clean it up during
           reconciliation. */
        rv = usl_bcm_l3_egress_multipath_destroy(mpathEgrNhopPtr->avlKey);
        if (rv == BCM_E_BUSY)
        {
          /* The entry is used by one or more routes. Leave as-is for now so that
             it can be revisited during reconciliation. */
          rv = BCM_E_NONE;
        }
        if (rv != BCM_E_NONE)
        {
          USL_LOG_MSG(USL_BCM_E_LOG,
                      "USL: Failed to delete l3 egress multipath object 0x%x, rv = %d\n",
                      mpathEgrNhopPtr->avlKey, rv);
        }
      }
    }

    mpathEgrNhopPtr = avlSearchLVL7(mpathEgrNhopDbHandle, &mpathEgrNhop, AVL_NEXT);
  }

  osapiFree(L7_DRIVER_COMPONENT_ID, invalidNhops);

  /* Update all the next-hops pointing to the failed master cpu */
  if (manager_left_stack == L7_TRUE)
  {
    memset(&egrNhop, 0, sizeof(egrNhop));
    egrNhopPtr = avlSearchLVL7(egrNhopDbHandle, &egrNhop, AVL_NEXT);
    while (egrNhopPtr != L7_NULLPTR)
    {
      memcpy(&egrNhop, egrNhopPtr, sizeof(egrNhop));

      /* Track entries w/ invalid modids. */
      if ((egrNhopPtr->egrInfo.bcm_data.flags & BCM_L3_TGID) == 0)
      {
        for (modIdIdx = 0; modIdIdx < missing_mod_id_count; modIdIdx++)
        {
          if (egrNhopPtr->egrInfo.bcm_data.module == missing_mod_ids[modIdIdx])
          {
            if ((egrNhopPtr->egrInfo.bcm_data.module == old_cpu_modid) &&
                (egrNhopPtr->egrInfo.bcm_data.port   == old_cpu_modport))
            {
              /* If it is the mgr that left the stack, we need to update next hop
                 entries that point to the CPU port. */
              egrNhopPtr->egrInfo.bcm_data.module = new_cpu_modid;
              egrNhopPtr->egrInfo.bcm_data.port   = new_cpu_modport;
              rv = usl_bcm_l3_egress_create(BCM_L3_WITH_ID | BCM_L3_REPLACE,
                                            &(egrNhopPtr->egrInfo),
                                            &(egrNhopPtr->egrId));
              if (rv != BCM_E_NONE)
              {
                USL_LOG_MSG(USL_BCM_E_LOG,
                            "USL: Failed to create l3 egress object 0x%x, rv = %d\n",
                            egrNhopPtr->egrId, rv);
              }
            }
          }
        }
      }

      egrNhopPtr = avlSearchLVL7(egrNhopDbHandle, &egrNhop, AVL_NEXT);
    }
  } /* if (manager_left_stack == L7_TRUE) */

  USL_L3_EGR_NHOP_DB_LOCK_GIVE();

  if (manager_left_stack)
  {
    USL_L3_INTF_DB_LOCK_TAKE();
    /* If the manager left the stack, then we need to destroy the VRRP L3 interfaces so
       that our stack does not forward any traffic sent to the virtual router MAC addresses.
       The VRRP slave should notice that the VRRP master is gone and will start forwarding
       this traffic after a short delay. */
    l3IntfDbHandle = usl_l3_intf_db_handle_get(USL_OPERATIONAL_DB);
    if (l3IntfDbHandle == L7_NULLPTR)
    {
      USL_L3_INTF_DB_LOCK_GIVE();
      return BCM_E_FAIL;
    }
    for (i = 0; i < maxL3Intfs; i++)
    {
      if (l3IntfDbHandle[i].isValid)
      {
        if (memcmp(l3IntfDbHandle[i].intfInfo.bcm_data.l3a_mac_addr, &(L7_ENET_VRRP_MAC_ADDR), 5) == 0)
        {
          rv = usl_bcm_l3_intf_delete(&l3IntfDbHandle[i].intfInfo);
          if (rv != BCM_E_NONE)
          {
            USL_LOG_MSG(USL_BCM_E_LOG,
                        "USL: Failed to delete l3 intf %d rv = %d\n",
                        l3IntfDbHandle[i].intfInfo.bcm_data.l3a_intf_id, rv);
          }
        }
      }
    }
    USL_L3_INTF_DB_LOCK_GIVE();
  }

  endTime = osapiTimeMillisecondsGet();
  uslL3DataplaneCleanupTime += osapiTimeMillisecondsGetDiff(endTime,startTime);
  uslL3DataplaneCleanupCount++;

  return rv;
}
void usl_l3_db_dataplane_cleanup_debug_show()
{
  sysapiPrintf("uslL3DataplaneCleanupTime (ms) == %d\n", uslL3DataplaneCleanupTime);
  sysapiPrintf("uslL3DataplaneCleanupCount     == %d\n", uslL3DataplaneCleanupCount);
  sysapiPrintf("average                   (ms) == %d\n", uslL3DataplaneCleanupTime / uslL3DataplaneCleanupCount);
}
void usl_l3_db_dataplane_cleanup_debug_clear()
{
  uslL3DataplaneCleanupTime  = 0;
  uslL3DataplaneCleanupCount = 0;
}
#endif

#endif /* L7_ROUTING_PACKAGE */
