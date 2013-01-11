/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  ipstkShared.c
*
* @purpose   ipstack shared utilities
*
* @component ipstack
*
* @comments  none
*
* @create    10/13/2004
*
* @author    jpickering
*
* @end
*
**********************************************************************/


#include <string.h>
#include <unistd.h>


#include <string.h>
#include "l7_common.h"
#include "sysapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "l3_commdefs.h"
#include "ipstk_api.h"
#include "dtl_l3_api.h"
#include "l3end_api.h"
#include "dot1q_api.h"

#include "l7_socket.h"
#include "l7_ip_api.h"
#include "l3_compdefs.h"
#ifdef L7_NSF_PACKAGE
#include "ckpt_api.h"
#include "ipstk_ckpt_api.h"
#endif

#define IPSTK_CKPT_REQ_QUEUE   "ipstkCkptQueue"
#define IPSTK_CKPT_MSG_COUNT   L7_RTR_MAX_RTR_INTERFACES

#ifdef L7_NSF_PACKAGE
#ifdef L7_ROUTING_PACKAGE
static void  *ipstkCkptQueue;
#endif /* L7_ROUTING_PACKAGE */
#endif

typedef struct L7_ipstkRtrInterface_s
{
  L7_BOOL             present;
  L7_uint32           intIfNum;
  L7_uint32           allocFlags;
  L7_BOOL             is_attached;
  L7_BOOL             is_up;
  L7_uint32           upFlags;

  /* Internal VLAN ID assigned by dot1q. */
  L7_uint32          internalVlan;

} L7_ipstkRtrIntfMap_t;

/* bit flags */
#define L7_L3SHARED_COMP_IP    1
#define L7_L3SHARED_COMP_IP6   2

/* map of stack index to intIfNum uses hash bins because linux
   increments ifIndex each intf create
*/
#define STIFI_HASH_BITS 5
#define STIFI_HASH_BINS (1<< STIFI_HASH_BITS)
#define STIFI_HASH_MASK (STIFI_HASH_BINS -1)
typedef struct stifi_entry_s{
   struct stifi_entry_s     *s_fwd;
   L7_uint32                 s_intIfNum;
   L7_uint32                 s_ifIndex;
}stifi_entry_t;
static stifi_entry_t ifIndexMap[L7_MAX_INTERFACE_COUNT+1];

typedef struct stifi_hash_head_s{
   stifi_entry_t        *b_top;
}stifi_hash_head_t;
static stifi_hash_head_t stifi_hash[STIFI_HASH_BINS];


#ifdef L7_ROUTING_PACKAGE

static L7_BOOL ipstkRoutingSharedLibInitialized = L7_FALSE;
static L7_ipstkRtrIntfMap_t  rtrIntfMap[L7_RTR_MAX_RTR_INTERFACES+1];
static L7_uint32 rtrPortRoutingFlags[L7_MAX_INTERFACE_COUNT+1];
static L7_uint32 rtrDtlRoutingFlags[L7_MAX_INTERFACE_COUNT+1];
static L7_uint32 rtrIntIfNumToRtrIfMap[L7_MAX_INTERFACE_COUNT+1];
static void *ipstkSharedLibSema;

static L7_RC_t ipstkInternalVlanAssign(L7_uint32 intIfNum, L7_uint32 *vlanId);
static void ipstkInternalVlanRelease(L7_uint32 intIfNum);

#ifdef L7_NSF_PACKAGE

/* synchronizes access to ipstkCkptInternalVlanTlv[]. */
static void * ipstkCkptSem;

/* message header len in bytes. msg type + msg len */
#define IPSTK_CKPT_MSG_HDR_LEN 4

typedef struct ipstkCkptInternalVlanData_s
{
  /* Internal interface number of port based routing interface */
  L7_uint32  intIfNum;

  /* Internal VLAN ID assigned to this port based routing interface */
  L7_uint32  internalVlan;

} ipstkCkptInternalVlanData_t;


typedef struct ipstkCkptInternalVlanTlv_s
{
  L7_ushort16  type;
  L7_ushort16  length;
  ipstkCkptInternalVlanData_t value;

} ipstkCkptInternalVlanTlv_t;

static ipstkCkptInternalVlanTlv_t ipstkCkptInternalVlanTlv[L7_RTR_MAX_RTR_INTERFACES + 1];

static L7_RC_t ipstkCheckpointRtrIntfDataCallback(L7_uint32 intIfNum);
static L7_RC_t ipstkCheckpointRtrIntfDataApply(L7_uint32 intIfNum );

/* debug stats */
static L7_uint32 ckptBytesSent = 0;     /* only includes intl vlan TLVs */
static L7_uint32 ckptBytesRecd = 0;     /* ditto */
static L7_uint32 ckptRtrIntfRecd = 0;   /* number of intl vlan mappings rec'd from master */
static L7_uint32 ckptRtrIntfSent = 0;   /* number of intl vlan mappings sent to backup */

#endif /* L7_NSF_PACKAGE */

/*********************************************************************
* @purpose  initialize  shared l3 library
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkRoutingSharedLibInit(void)
{
  if(ipstkRoutingSharedLibInitialized == L7_FALSE)
  {
    memset(rtrIntfMap,0,sizeof(L7_ipstkRtrIntfMap_t) * L7_RTR_MAX_RTR_INTERFACES+1);
    memset(rtrPortRoutingFlags ,0,sizeof(L7_uint32) *(L7_MAX_INTERFACE_COUNT+1));
    memset(rtrDtlRoutingFlags ,0,sizeof(L7_uint32) *(L7_MAX_INTERFACE_COUNT+1));
    memset(rtrIntIfNumToRtrIfMap ,0,sizeof(L7_uint32) *(L7_MAX_INTERFACE_COUNT+1));
    memset(stifi_hash ,0,sizeof(stifi_hash_head_t) *(STIFI_HASH_BINS));
    memset(ifIndexMap ,0,sizeof(stifi_entry_t) *(L7_MAX_INTERFACE_COUNT+1));
    if((ipstkSharedLibSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO,OSAPI_SEM_FULL)) == L7_NULLPTR)
        return(L7_FAILURE);
    ipstkRoutingSharedLibInitialized = L7_TRUE;
  }
  return(L7_SUCCESS);
}



/*********************************************************************
* @purpose  allocate a shared router interface num from an intIfNum
*
* @param    intIfNum        Internal Interface Number
* @param    compId          componentId of caller
* @param    rtrIfNum        returned router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkRtrIfNumGet(L7_uint32 intIfNum,L7_uint32 compId, L7_uint32 *rtrIfNum)
{
  L7_uint32       i;
  L7_uint32       compFl;
  L7_uchar8       ifName[IFNAMSIZ];
  L7_uint32       stackIfIndex;
  stifi_entry_t    *ifmap;
  stifi_hash_head_t *head;
  L7_INTF_TYPES_t intfType;


  if(intIfNum > L7_MAX_INTERFACE_COUNT)
       return L7_FAILURE;

  /* validate component */
  switch(compId){
  case L7_IP_MAP_COMPONENT_ID:
       compFl = L7_L3SHARED_COMP_IP;
       break;
  case L7_FLEX_IPV6_MAP_COMPONENT_ID:
       compFl = L7_L3SHARED_COMP_IP6;
       break;
  default:
       return(L7_FAILURE);
  }
  if (nimGetIntfType(intIfNum, &intfType) != L7_SUCCESS)
  {
       return(L7_FAILURE);
  }

  if (osapiSemaTake(ipstkSharedLibSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* check for existing map entry */
  for (i = 1; i <= L7_RTR_MAX_RTR_INTERFACES; i++)
  {
    if((rtrIntfMap[i].present == L7_TRUE) &&
       (rtrIntfMap[i].intIfNum == intIfNum)){
       rtrIntfMap[i].allocFlags |= compFl;
       *rtrIfNum = i;
       osapiSemaGive(ipstkSharedLibSema);
       return(L7_SUCCESS);
    }
  }


  /* next avaliable router interface number */
  for (i = 1; i <= L7_RTR_MAX_RTR_INTERFACES; i++)
  {
    if (rtrIntfMap[i].present == L7_FALSE)
    {
       rtrIntfMap[i].intIfNum = intIfNum;
       rtrIntfMap[i].present = L7_TRUE;
       rtrIntfMap[i].allocFlags = compFl;
       rtrIntIfNumToRtrIfMap[intIfNum] = i;

       /* tell driver */
       if(intfType != L7_TUNNEL_INTF){
           ipmRouterIfMapCreate(i,intIfNum);
       }


       osapiIfNameStringGet (intIfNum, ifName, IFNAMSIZ);

       /* get stack IfIndex for use with RECVPKTINFO, etc */
       if(ipstkStackIfIndexGet(ifName,&stackIfIndex) != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                 "Failed to get interface index from IP stack for interface %s.",
                 ifName);
       }
       else
       {
             ifmap = &ifIndexMap[intIfNum];
             ifmap->s_intIfNum = intIfNum;
             if (ifmap->s_ifIndex != 0)
             {
               L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                       "Interface index map has been corrupted.");
             }
             ifmap->s_ifIndex = stackIfIndex;
             /* link into hash bin */
             head = &stifi_hash[stackIfIndex & STIFI_HASH_MASK];
             ifmap->s_fwd = head->b_top;
             head->b_top = ifmap;
       }

       *rtrIfNum = i;

#ifdef L7_NSF_PACKAGE
       /*  If in warm restart, see if an intIfNum to internal VLAN mapping
        * has been checkpointed for this interface. If so, use it. Otherwise,
        * ask dot1q for an internal VLAN. */
       if (ipMapWarmRestartCompleted() == L7_FALSE)
       {
         ipstkCheckpointRtrIntfDataApply(intIfNum);
       }
#endif
       osapiSemaGive(ipstkSharedLibSema);
       return(L7_SUCCESS);
    }
  }
  osapiSemaGive(ipstkSharedLibSema);
  return(L7_FAILURE);
}


/*********************************************************************
* @purpose  deallocate a shared router interface num from an intIfNum
*
* @param    rtrIfNum        router Interface Number
* @param    compId          componentId of caller
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkRtrIfNumRelease(L7_uint32 rtrIfNum, L7_uint32 compId)
{
  L7_uint32       compFl;
  L7_ipstkRtrIntfMap_t *mapEnt;
  stifi_hash_head_t *head;
  stifi_entry_t *ifmap,*tmp;
  L7_INTF_TYPES_t intfType;

  if(rtrIfNum > L7_RTR_MAX_RTR_INTERFACES)
       return L7_FAILURE;

  /* validate component */
  switch(compId){
  case L7_IP_MAP_COMPONENT_ID:
       compFl = L7_L3SHARED_COMP_IP;
       break;
  case L7_FLEX_IPV6_MAP_COMPONENT_ID:
       compFl = L7_L3SHARED_COMP_IP6;
       break;
  default:
       return(L7_FAILURE);
  }

  mapEnt = &rtrIntfMap[rtrIfNum];

  /* validate component has alloc this */
  if(mapEnt->present != L7_TRUE)
  {
    return(L7_FAILURE);
  }
  if((mapEnt->allocFlags & compFl) == 0){
    return(L7_FAILURE);
  }
  if (nimGetIntfType(mapEnt->intIfNum, &intfType) != L7_SUCCESS)
  {
       return(L7_FAILURE);
  }

  if (osapiSemaTake(ipstkSharedLibSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  mapEnt->allocFlags &= ~compFl;

  /* if no longer used by any component, mark unused */
  if(mapEnt->allocFlags == 0)
  {
    mapEnt->present = L7_FALSE;
    rtrIntIfNumToRtrIfMap[mapEnt->intIfNum] = 0;
    /* if not a tunnel, tell driver */
    if(intfType != L7_TUNNEL_INTF) {
       ipmRouterIfMapDelete(rtrIfNum,mapEnt->intIfNum);
    }

    /* unmap stackIfIndex */
    ifmap = &ifIndexMap[mapEnt->intIfNum];
    head = &stifi_hash[ifmap->s_ifIndex & STIFI_HASH_MASK];
    if(head->b_top == ifmap){
        head->b_top = ifmap->s_fwd;
    }
    else{
        tmp = head->b_top;
        while(tmp){
            if(tmp->s_fwd == ifmap)
            {
                tmp->s_fwd = ifmap->s_fwd;
                break;
            }
            tmp = tmp->s_fwd;
        }
        if(!tmp)
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(mapEnt->intIfNum, L7_SYSNAME, ifName);
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                    "Error releasing interface index from IP stack for interface %s.",
                    ifName);
        }
    }
    ifmap->s_ifIndex = 0;
  }

  osapiSemaGive(ipstkSharedLibSema);
  return(L7_SUCCESS);
}
/*********************************************************************
* @purpose  tell stack router interface is up.
*
* @param    rtrIfNum        router Interface Number
* @param    compId          componentId of caller
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkRtrIfUp(L7_uint32 rtrIfNum,L7_uint32 compId)
{
  L7_uint32       compFl;
  L7_ipstkRtrIntfMap_t *mapEnt;
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 ifname[IFNAMSIZ];
  L7_INTF_TYPES_t intfType;

  /* validate component */
  switch(compId){
  case L7_IP_MAP_COMPONENT_ID:
       compFl = L7_L3SHARED_COMP_IP;
       break;
  case L7_FLEX_IPV6_MAP_COMPONENT_ID:
       compFl = L7_L3SHARED_COMP_IP6;
       break;
  default:
       return(L7_FAILURE);
  }

  mapEnt = &rtrIntfMap[rtrIfNum];

  /* validate component has alloc this */
  if(mapEnt->present != L7_TRUE)
  {
    return(L7_FAILURE);
  }
  if((mapEnt->allocFlags & compFl) == 0){
    return(L7_FAILURE);
  }
  if (nimGetIntfType(mapEnt->intIfNum, &intfType) != L7_SUCCESS)
  {
       return(L7_FAILURE);
  }
  if (osapiSemaTake(ipstkSharedLibSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* set up flags for component */
  mapEnt->upFlags |= compFl;

  /* separate enable for v6 */
  osapiIfNameStringGet(mapEnt->intIfNum,ifname, IFNAMSIZ);

#ifdef L7_IPV6_PACKAGE
  /* needs both IPV6_ENA && IFF_UP */
  if(compId == L7_FLEX_IPV6_MAP_COMPONENT_ID)
    rc = osapiIfIpv6EnableSet(ifname, L7_TRUE);
#endif

  if(mapEnt->is_up != L7_TRUE)
  {
    /* tell driver */
    if(intfType != L7_TUNNEL_INTF){
        ipmRouterIfDrvUp(mapEnt->intIfNum);
    }
    mapEnt->is_up = L7_TRUE;
    ipstkIfFlagChange(ifname,IFF_UP,L7_TRUE);

  }
  osapiSemaGive(ipstkSharedLibSema);

  return rc;
}

/*********************************************************************
* @purpose  tell stack router interface is down.
*
* @param    rtrIfNum        router Interface Number
* @param    compId          componentId of caller
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkRtrIfDown(L7_uint32 rtrIfNum,L7_uint32 compId)
{
  L7_uint32       compFl;
  L7_ipstkRtrIntfMap_t *mapEnt;
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 ifname[IFNAMSIZ];
  L7_INTF_TYPES_t intfType;

  /* validate component */
  switch(compId){
  case L7_IP_MAP_COMPONENT_ID:
       compFl = L7_L3SHARED_COMP_IP;
       break;
  case L7_FLEX_IPV6_MAP_COMPONENT_ID:
       compFl = L7_L3SHARED_COMP_IP6;
       break;
  default:
       return(L7_FAILURE);
  }

  mapEnt = &rtrIntfMap[rtrIfNum];

  /* validate component has alloc this */
  if(mapEnt->present != L7_TRUE)
  {
    return(L7_FAILURE);
  }
  if((mapEnt->allocFlags & compFl) == 0){
    return(L7_FAILURE);
  }
  if (nimGetIntfType(mapEnt->intIfNum, &intfType) != L7_SUCCESS)
  {
       return(L7_FAILURE);
  }

  if (osapiSemaTake(ipstkSharedLibSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* set up flags for component */
  mapEnt->upFlags &= ~compFl;

  osapiIfNameStringGet(mapEnt->intIfNum,ifname, IFNAMSIZ);

#ifdef L7_IPV6_PACKAGE
  if(compId == L7_FLEX_IPV6_MAP_COMPONENT_ID)
    rc = osapiIfIpv6EnableSet(ifname, L7_FALSE);
#endif

  if((mapEnt->upFlags == 0) && (mapEnt->is_up == L7_TRUE))
  {
    /* tell driver */
    if(intfType != L7_TUNNEL_INTF){
        ipmRouterIfDrvDown(mapEnt->intIfNum);
    }
    ipstkIfFlagChange(ifname,IFF_UP,L7_FALSE);

    mapEnt->is_up = L7_FALSE;
  }
  osapiSemaGive(ipstkSharedLibSema);
  return rc;
}

/*********************************************************************
* @purpose  set un-numbered flag for an interface.
*
* @param    ifname          ascii interface name
* @param    flag            enable/disable unnumbered flag (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkRtrIfUnnumbSet(L7_uchar8 *ifname, L7_BOOL flag)
{
#ifdef _L7_OS_VXWORKS_
  L7_RC_t rc;
  if (osapiSemaTake(ipstkSharedLibSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  rc = ipstkIfFlagChange(ifname, IFF_UNNUMB, flag);
  osapiSemaGive(ipstkSharedLibSema);
  return rc;
#else
  return L7_FAILURE;
#endif
}

/*********************************************************************
* @purpose  Get un-numbered flag for an interface
*
* @param    ifname          ascii interface name
* @param    flag            unnumbered interface flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkRtrIfUnnumbGet(L7_uchar8 *ifname, L7_BOOL *flag)
{
#ifdef _L7_OS_VXWORKS_
  L7_RC_t rc;
  if (osapiSemaTake(ipstkSharedLibSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  rc = ipstkIfFlagGet(ifname, IFF_UNNUMB, flag);
  osapiSemaGive(ipstkSharedLibSema);
  return rc;
#else
  return L7_FAILURE;
#endif
}

/*********************************************************************
* @purpose  Convert Internal Interface Number to Router Interface Number
*
* @param    intIfNum   internal interface number
* @param    *rtrIntf   pointer to router interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
*
* @end
*********************************************************************/
L7_RC_t ipstkIntIfNumToRtrIntf(L7_uint32 intIfNum, L7_uint32* rtrIfNum)
{
    if(intIfNum > L7_MAX_INTERFACE_COUNT)
       return L7_FAILURE;

    *rtrIfNum = rtrIntIfNumToRtrIfMap[intIfNum];
    return (*rtrIfNum)?L7_SUCCESS:L7_FAILURE;
}
/*********************************************************************
* @purpose  check status of interface
*
* @param    intIfNum    internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    only works for router interfaces
*
*
* @end
*********************************************************************/
L7_RC_t ipstkRtrIfIsUp(L7_uint32 intIfNum)
{
  L7_ipstkRtrIntfMap_t *mapEnt;
  L7_uint32 rtrIfNum;

  if(intIfNum > L7_MAX_INTERFACE_COUNT)
       return L7_FAILURE;

  rtrIfNum = rtrIntIfNumToRtrIfMap[intIfNum];
  mapEnt = &rtrIntfMap[rtrIfNum];

  /* validate component has alloc this */
  if(mapEnt->present != L7_TRUE)
  {
    return(L7_FAILURE);
  }
  return mapEnt->is_up? L7_SUCCESS:L7_FAILURE;
}
/*********************************************************************
*
* @purpose  Notify NIM of an interface change. IP MAP tells NIM when
*           routing is enabled or disabled on an interface.
*
* @param    intIfNum  @b{(input)}  internal interface number
* @param    event     @b{(input)}  event from L7_PORT_EVENTS_t
* @parma    callback  @b{(input)}  callback function pointer. If non-NULL,
*                                  NIM will call this function when the
*                                  event is complete.
* @param    handle    @b{(output)} identifies the event in case the caller
*                                  requires notification of event completion
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipstkIntfChangeNimNotify(L7_uint32 intIfNum, L7_uint32 event,
                                 NIM_NOTIFY_CB_FUNC_t callback,
                                 NIM_HANDLE_t *handle,L7_uint32 compId)
{
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  L7_uint32       compFl;
  L7_uint32 org_enabled_flags;
  L7_RC_t rc;

  /* validate component */
  if(intIfNum > L7_MAX_INTERFACE_COUNT)
       return(L7_FAILURE);
  switch(compId){
  case L7_IP_MAP_COMPONENT_ID:
       compFl = L7_L3SHARED_COMP_IP;
       break;
  case L7_FLEX_IPV6_MAP_COMPONENT_ID:
       compFl = L7_L3SHARED_COMP_IP6;
       break;
  default:
       return(L7_FAILURE);
  }

  /* set or clear flag */
  if (osapiSemaTake(ipstkSharedLibSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  org_enabled_flags = rtrPortRoutingFlags[intIfNum];
  if(event == L7_PORT_ROUTING_ENABLED)
  {
       rtrPortRoutingFlags[intIfNum] |= compFl;
       if(org_enabled_flags != 0)
       {
          rc = L7_SUCCESS;
          goto exit;
       }
  }
  else if(event == L7_PORT_ROUTING_DISABLED)
  {
       rtrPortRoutingFlags[intIfNum] &= ~compFl;
       if(rtrPortRoutingFlags[intIfNum] != 0)
       {
          rc = L7_SUCCESS;
          goto exit;
       }
  }
  else
  {
          rc = L7_FAILURE;
          goto exit;
  }


  eventInfo.component     = compId;
  eventInfo.event         = event;
  eventInfo.intIfNum      = intIfNum;
  eventInfo.pCbFunc       = callback;

  rc = nimEventIntfNotify(eventInfo, handle);
exit:
  osapiSemaGive(ipstkSharedLibSema);
  return rc;
}

/*********************************************************************
*
* @purpose  Notify DTL of an interface change. IP MAP tells DTL when
*           routing is enabled or disabled on an interface.
*
* @param    intIfNum  @b{(input)}  internal interface number
* @param    event     @b{(input)}  event from L7_ENABLE or DISABLE
*
* @returns  L7_RC_t
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipstkIntfChangeDtlNotify(L7_uint32 intIfNum, L7_uint32 event,
                                 L7_uint32 compId)
{
  L7_uint32       compFl;
  L7_uint32 org_enabled_flags;
  L7_RC_t rc;
  L7_INTF_TYPES_t sysIntfType;
  L7_uint32 vlanId = 0;

  /* validate component */
  if(intIfNum > L7_MAX_INTERFACE_COUNT)
       return(L7_FAILURE);
  switch(compId){
  case L7_IP_MAP_COMPONENT_ID:
       compFl = L7_L3SHARED_COMP_IP;
       break;
  case L7_FLEX_IPV6_MAP_COMPONENT_ID:
       compFl = L7_L3SHARED_COMP_IP6;
       break;
  default:
       return(L7_FAILURE);
  }

  /* set or clear flag */
  if (osapiSemaTake(ipstkSharedLibSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  org_enabled_flags = rtrDtlRoutingFlags[intIfNum];
  if(event == L7_ENABLE)
  {
       rtrDtlRoutingFlags[intIfNum] |= compFl;
       if(org_enabled_flags != 0)
       {
          rc =  L7_SUCCESS;
          goto exit;

       }
  }
  else if(event == L7_DISABLE)
  {
       rtrDtlRoutingFlags[intIfNum] &= ~compFl;
       if(rtrDtlRoutingFlags[intIfNum] != 0)
       {
          rc = L7_SUCCESS;
          goto exit;
       }
  }
  else{
       rc = L7_FAILURE;
       goto exit;
  }
  /* If port based routing interface, get or release internal VLAN ID. */
  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    osapiSemaGive(ipstkSharedLibSema);
    return L7_FAILURE;
  }
  if (sysIntfType == L7_PHYSICAL_INTF)
  {
    if (event == L7_ENABLE)
    {
       if (ipstkInternalVlanAssign(intIfNum, &vlanId) != L7_SUCCESS)
       {
         /* clear flag so we can try again next time. */
         rtrDtlRoutingFlags[intIfNum] &= ~compFl;
         osapiSemaGive(ipstkSharedLibSema);
         return L7_FAILURE;
       }

    }
    else if (event == L7_DISABLE)
    {
      ipstkInternalVlanRelease(intIfNum);
    }
  }

  rc =  dtlRtrIntfModeSet(intIfNum, vlanId, event);
exit:
  osapiSemaGive(ipstkSharedLibSema);
  return rc;
}

/*********************************************************************
* @purpose  Clear the RtrPortRoutingFlags
*
* @param
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void ipStrClearRtrPortRoutingFlags(void)
{
  memset(rtrPortRoutingFlags, 0, sizeof(L7_uint32) * (L7_MAX_INTERFACE_COUNT + 1));
}


#endif /* L7_ROUTING_PACKAGE */



/*********************************************************************
* @purpose  convert statck ifIndex to intIfNum
*
* @parma    ifIndex   @b{(input)}  interface index
* @parma    intIfNum  @b{(output)}  internal interface
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    applies only to router interfaces
*
* @end
*********************************************************************/
L7_RC_t ipstkStackIfIndexToIntIfNum(L7_uint32 ifIndex,L7_uint32 *intIfNum)
{
   stifi_entry_t *ifmap = stifi_hash[ifIndex & STIFI_HASH_MASK].b_top;

   while(ifmap)
   {
       if(ifmap->s_ifIndex == ifIndex){
          *intIfNum = ifmap->s_intIfNum;
          return L7_SUCCESS;
       }
       ifmap = ifmap->s_fwd;
   }
   return L7_FAILURE;
}


/*********************************************************************
* @purpose  convert intIfNum to stack ifIndex
*
* @parma    ifIndex   @b{(input)}  interface index
* @parma    intIfNum  @b{(output)}  internal interface
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    applies only to router interfaces
*
* @end
*********************************************************************/
L7_RC_t ipstkIntIfNumToStackIfIndex(L7_uint32 intIfNum,L7_uint32 *ifIndex)
{

   if(intIfNum > L7_MAX_INTERFACE_COUNT)
   {
        *ifIndex = 0;
        return L7_FAILURE;
   }
   *ifIndex = ifIndexMap[intIfNum].s_ifIndex;
   return L7_SUCCESS;
}
/*********************************************************************
* @purpose  provide task params to stack without including whole l7 universe.
*
* @parma    pStackSize @b{(output)}  ptr to stack size
* @parma    pPriority  @b{(output)}  ptr to task priority
* @parma    pSlice     @b{(output)}  ptr to task slice
*
* @returns  -1 err, 0 success.
*
* @notes    intentionally NOT L7_RC_t or other L7 types.
*
* @end
*********************************************************************/
int ipstkTaskParmsGet(unsigned int *pStackSize, unsigned int *pPriority, unsigned int *pSlice)
{
    *pStackSize = L7_DEFAULT_STACK_SIZE;
    *pPriority  = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY);
    *pSlice     = L7_DEFAULT_TASK_SLICE;

    return 0;
}


#ifdef L7_ROUTING_PACKAGE

/*********************************************************************
* @purpose  Assign a VLAN ID for internal use on a port based routing interface.
*
* @param    intIfNum        internal interface number of port based routing interface
* @param    vlanId          vlanId assigned
*
* @returns  L7_SUCCESS if VLAN ID assigned
* @returns  L7_FAILURE if no VLAN IDs available
*
* @notes    invoked under protection of ipstkSharedLibSema
*
* @end
*********************************************************************/
L7_RC_t ipstkInternalVlanAssign(L7_uint32 intIfNum, L7_uint32 *vlanId)
{
  L7_uint32 vlan;
  L7_uint32 rtrIfNum;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_char8 descr[L7_DOT1Q_MAX_INTERNAL_VLANS_DESCR];


  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
  if (ipstkIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
            "Cannot find rtrIntf for port based routing interface %s.",
            ifName);
    return L7_FAILURE;
  }

  vlan = rtrIntfMap[rtrIfNum].internalVlan;
  osapiSnprintf(descr, L7_DOT1Q_MAX_INTERNAL_VLANS_DESCR - 1,
                "Port Based Routing Interface %s", ifName);

  if (dot1qInternalVlanRequest(DOT1Q_IP_INTERNAL_VLAN, descr, &vlan) == L7_SUCCESS)
  {
    rtrIntfMap[rtrIfNum].internalVlan = vlan;

    if (vlanId != L7_NULLPTR)
    {
      *vlanId = rtrIntfMap[rtrIfNum].internalVlan;

#ifdef L7_NSF_PACKAGE
      /* if backup elected, checkpoint rtrIntf data */
      if (ipMapBackupElected() == L7_TRUE)
      {
        ipstkCheckpointRtrIntfDataCallback(intIfNum);
      }
#endif
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
              "Assigning VLAN ID %u to port based routing interface %s.",
              vlan, ifName);
      return L7_SUCCESS;
    }
  }

  L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_IP_MAP_COMPONENT_ID,
          "Unable to assign an internal VLAN ID to interface %s.", ifName);

  return L7_FAILURE;
}



/*********************************************************************
* @purpose  Release a VLAN ID used for a port based routing interface.
*
* @param    intIfNum        internal interface number of port based routing interface
*
* @returns  L7_SUCCESS if VLAN ID released
* @returns  L7_FAILURE
*
* @notes    invoked under protection of ipstkSharedLibSema
*
* @end
*********************************************************************/
void ipstkInternalVlanRelease(L7_uint32 intIfNum)
{
  L7_uint32 vlanId;
  L7_uint32 rtrIfNum;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (ipstkIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
            "Cannot find rtrIntf for port based routing interface %s.",
            ifName);
    return;
  }

  vlanId = rtrIntfMap[rtrIfNum].internalVlan;
  rtrIntfMap[rtrIfNum].internalVlan = 0;

  if (dot1qInternalVlanRelease(vlanId, DOT1Q_IP_INTERNAL_VLAN) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
            "Cannot release VLAN %d for port based routing interface %s.",
            vlanId, ifName);
    return;
  }

  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
          "Releasing VLAN ID %u from port based routing interface %s.",
          vlanId, ifName);

#ifdef L7_NSF_PACKAGE
  /* if backup elected, checkpoint rtrIntf data */
  if (ipMapBackupElected() == L7_TRUE)
  {
    ipstkCheckpointRtrIntfDataCallback(intIfNum);
  }
#endif

  return;
}


/*********************************************************************
* @purpose  Get the internal interface number associated with the
*           port-based routing interface corresponding to this
*          internal VLAN
*
* @param    vlanid          id of the vlan
* @param    intIfNum        Internal interface number of port-based routing interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if VLAN ID does not correspond to a port-based routing interface
*
* @notes    Cannot be used to find the intIfNum for a VLAN ID used for
*           a VLAN routing interface.
*
* @end
*********************************************************************/
L7_RC_t ipstkInternalVlanIdToRtrIntIfNum(L7_uint32 vlanId, L7_uint32 *intIfNum)
{
  L7_uint32 rtrIfNum;

  if (sysNetIsValid8021qVlanId((L7_ushort16)vlanId) == L7_FALSE)
    return L7_FAILURE;

  if (osapiSemaTake(ipstkSharedLibSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  for (rtrIfNum = 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
  {
    if (rtrIntfMap[rtrIfNum].internalVlan == vlanId)
    {
      *intIfNum = rtrIntfMap[rtrIfNum].intIfNum;
      osapiSemaGive(ipstkSharedLibSema);
      return L7_SUCCESS;
    }
  }

  osapiSemaGive(ipstkSharedLibSema);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the port-based routing interface for the given assigned VLANID
*
* @param    vlanId VLAN ID
*
* @returns  non-zero if Routing interface is found.
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ipstkInternalVlanRoutingInterfaceGet(L7_uint32 vlanId)
{
  L7_uint32 rtrIfNum;

  if (sysNetIsValid8021qVlanId((L7_ushort16)vlanId) == L7_FALSE)
    return 0;

  if (osapiSemaTake(ipstkSharedLibSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    return 0;

  for (rtrIfNum = 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
  {
    if (rtrIntfMap[rtrIfNum].internalVlan == vlanId)
    {
      osapiSemaGive(ipstkSharedLibSema);
      return rtrIntfMap[rtrIfNum].intIfNum;
    }
  }
  osapiSemaGive(ipstkSharedLibSema);
  return 0;
}



/*********************************************************************
* @purpose  Get the internal VLAN assigned to a port based routing interface
*
* @param    intIfNum        internal interface number of port based routing interface
*
* @returns  non-zero if VLAN ID found
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ipstkInternalVlanIdGet(L7_uint32 intIfNum)
{
  L7_uint32 vlanId;
  L7_uint32 rtrIfNum;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  if (osapiSemaTake(ipstkSharedLibSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    return 0;

  if (ipstkIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
            "Cannot find rtrIntf for port based routing interface %s.",
            ifName);
    osapiSemaGive(ipstkSharedLibSema);
    return L7_FAILURE;
  }

  vlanId = rtrIntfMap[rtrIfNum].internalVlan;

  osapiSemaGive(ipstkSharedLibSema);
  return vlanId;
}

/*********************************************************************
* @purpose  Find the first VLAN ID (lowest numerical ID) claimed for internal use
*           for a port-based routing interface
*
* @param    vlanId          vlanId
* @param    intIfNum        corresponding interface
*
* @returns  L7_SUCCESS if a VLAN ID found
* @returns  L7_FAILURE if no VLAN IDs used internally
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipstkInternalVlanFindFirst(L7_uint32 *vlanId, L7_uint32 *intIfNum)
{
  L7_uint32 rtrIfNum;
  L7_uint32 assocIntIfNum = 0;
  L7_uint32 lowestVlanId;

  /* invalid VLAN ID*/
  lowestVlanId   = 0xFFFF;

  if (osapiSemaTake(ipstkSharedLibSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  for (rtrIfNum = 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
  {
    if (rtrIntfMap[rtrIfNum].internalVlan != 0)
    {
      if (rtrIntfMap[rtrIfNum].internalVlan < lowestVlanId)
      {
        lowestVlanId =  rtrIntfMap[rtrIfNum].internalVlan;
        assocIntIfNum = rtrIntfMap[rtrIfNum].intIfNum;
      }
    }
  }

  if ( lowestVlanId < 0xFFFF)
  {
    /* Entry is found */
    *vlanId     = lowestVlanId;
    *intIfNum   = assocIntIfNum;
    osapiSemaGive(ipstkSharedLibSema);
    return L7_SUCCESS;
  }

  /* Entry is not found */
  *vlanId     = 0;
  *intIfNum   = 0;
  osapiSemaGive(ipstkSharedLibSema);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Find the next VLAN ID (in numeric order) claimed for internal use
*           for a port-based routing interface
*
* @param    prevVid      previous VLAN ID
* @param    vlanId       next vlanId
* @param    intIfNum     interface corresponding to next VLAN ID
*
* @returns  L7_SUCCESS if next VLAN ID found
* @returns  L7_FAILURE if no more VLAN IDs used internally
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipstkInternalVlanFindNext(L7_uint32 prevVid, L7_uint32 *vlanId, L7_uint32 *intIfNum)
{

  L7_uint32 rtrIfNum;
  L7_uint32 assocIntIfNum = 0;
  L7_uint32 nextLowestVlanId;
  L7_uint32 vlan;

  if (osapiSemaTake(ipstkSharedLibSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* invalid VLAN ID*/
  nextLowestVlanId   = 0xFFFF;

  for (rtrIfNum = 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
  {
    if (rtrIntfMap[rtrIfNum].internalVlan != 0)
    {
      vlan = rtrIntfMap[rtrIfNum].internalVlan;
      if ( (vlan > prevVid) && (vlan < nextLowestVlanId) )
      {
        nextLowestVlanId = vlan;
        assocIntIfNum = rtrIntfMap[rtrIfNum].intIfNum;
      }
    }
  }

  if ( nextLowestVlanId < 0xFFFF)
  {
    /* Entry is found */
    *vlanId     = nextLowestVlanId;
    *intIfNum   = assocIntIfNum;
    osapiSemaGive(ipstkSharedLibSema);
    return L7_SUCCESS;
  }

  /* Entry is not found */
  *vlanId     = 0;
  *intIfNum   = 0;
  osapiSemaGive(ipstkSharedLibSema);
  return L7_FAILURE;

}


/* Temporary debug routine to print internal VLAN use */
void ipstkShowVlanInternalUsage(void);

void ipstkShowVlanInternalUsage(void)
{
  L7_uint32 vlanId;
  L7_uint32 intIfNum;
  L7_RC_t rc = L7_SUCCESS;

  printf("\n%6s %10s", "VLAN ID", "Interface");

  if (ipstkInternalVlanFindFirst(&vlanId, &intIfNum) != L7_SUCCESS)
    return;

  while (rc == L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    printf("\n%6u %10s", vlanId, ifName);
    rc = ipstkInternalVlanFindNext(vlanId, &vlanId, &intIfNum);
  }
  printf("\n");
}


#endif /* L7_ROUTING_PACKAGE */

#ifdef L7_NSF_PACKAGE

#ifdef L7_ROUTING_PACKAGE
/*********************************************************************
* @purpose  Init ipstk checkpoint code.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipstkCheckpointInit(void)
{
  /* Synchronize accesses to ipstkCkptInternalVlanTlv[]  */
  ipstkCkptSem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ipstkCkptSem == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Failed to create semaphore for internal VLAN checkpoint.");
    return L7_FAILURE;
  }

  memset((void *)ipstkCkptInternalVlanTlv, 0, sizeof(ipstkCkptInternalVlanTlv_t)*(L7_RTR_MAX_RTR_INTERFACES + 1));


  /* allocate memory for request queue*/
  ipstkCkptQueue = (void*)osapiMsgQueueCreate(IPSTK_CKPT_REQ_QUEUE, IPSTK_CKPT_MSG_COUNT,
                                              (L7_uint32) sizeof(ipstkCkptInternalVlanTlv_t));

  if (ipstkCkptQueue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "ipstkCheckpointInit: Error allocating data for checkpoint queue");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


#else

/*********************************************************************
* @purpose  Init ipstk checkpoint code.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipstkCheckpointInit(void)
{
  return L7_SUCCESS;
}

#endif /* L7_ROUTING_PACKAGE */

#ifdef L7_ROUTING_PACKAGE
/*********************************************************************
* @purpose  Write as many internal VLAN mappings as will fit in checkpoint message.
*
* @param  checkpointData  @b{(input)}  Checkpoint message buffer.
* @param  maxDataLen      @b{(input)}  Number of bytes that can be written to
*                                        checkpoint message
* @param  msgLen          @b{(output)} Number of bytes written to checkpoint msg
* @param  moreData        @b{(output)} Set to L7_TRUE if there is more data to
*                                        checkpoint
*
* @returns  L7_SUCCESS
*
* @notes  runs on the checkpoint service thread
*
* @end
*********************************************************************/
L7_RC_t ipstkCheckpointIntlVlansWrite(L7_uchar8 *checkpointData,
                                      L7_uint32 maxDataLen,
                                      L7_uint32 *dataLen,
                                      L7_BOOL  *moreData)
{
  L7_uint32     tlvLength;
  L7_uint32     bytesWritten = 0;
  L7_RC_t      rc;
  ipstkCkptInternalVlanData_t  intlVlanMap;
  ipstkCkptInternalVlanTlv_t *tlv = (ipstkCkptInternalVlanTlv_t*) checkpointData;

  if ((checkpointData == L7_NULLPTR) || (dataLen == L7_NULLPTR) || (moreData == L7_NULLPTR))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
            "Error checkpointing internal VLAN mapping for port-based routing interface.");
    return L7_FAILURE;
  }

  /* Assume there is more data to be read */
  *moreData = L7_TRUE;

  tlvLength = sizeof(ipstkCkptInternalVlanTlv_t);

  /* keep adding interface data to checkpoint message until there is no more data
   * or the checkpoint message is full. */
  while ((bytesWritten + tlvLength) < maxDataLen)
  {
    rc = osapiMessageReceive(ipstkCkptQueue, &intlVlanMap, sizeof(ipstkCkptInternalVlanData_t), L7_NO_WAIT);
    if (rc != L7_SUCCESS)
    {
      /* No more internal VLAN mappings waiting to be checkpointed. */
      *moreData = L7_FALSE;
      break;
    }

    /* TLV Type */
    tlv->type = osapiHtons(IPMAP_CKPT_TLV_INTL_VLAN);
    tlv->length = osapiHtons(sizeof(ipstkCkptInternalVlanData_t));
    tlv->value.intIfNum = osapiHtonl(intlVlanMap.intIfNum);
    tlv->value.internalVlan = osapiHtonl(intlVlanMap.internalVlan);

    bytesWritten += tlvLength;
    ckptRtrIntfSent++;
    tlv++;
  }

  ckptBytesSent += bytesWritten;

  *dataLen = bytesWritten;

  return L7_SUCCESS;
}

#endif /* L7_ROUTING_PACKAGE */


#ifdef L7_ROUTING_PACKAGE
/*********************************************************************
* @purpose  On a warm restart, apply checkpoint rtr interface data for
*           ports which are still operational.
*
* @param  intIfNum   @b{(input)}  internal interface number
*
* @returns  void
*
* @notes    invoked under protection of ipstkSharedLibSema
*
* @end
*********************************************************************/
L7_RC_t ipstkCheckpointRtrIntfDataApply(L7_uint32 intIfNum )
{
  L7_uint32 index;
  L7_uint32 rtrIfNum;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_BOOL entryFound;
  L7_INTF_TYPES_t sysIntfType;

  if (nimGetIntfName(intIfNum, L7_SYSNAME, ifName) != L7_SUCCESS)
  {
    sprintf(ifName, "intIfNum %d", intIfNum);
  }

  /* If port based routing interface, get internal vlan. */
  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
            "Failed to get interface type for interface %s.",
            ifName);
    return L7_FAILURE;
  }
  if (sysIntfType != L7_PHYSICAL_INTF)
  {
    /* Nothing to do */
    return L7_SUCCESS;
  }

  if (ipstkIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
            "Cannot find router interface number for port based routing interface %s.",
            ifName);
    return L7_FAILURE;
  }

  entryFound = L7_FALSE;

  osapiSemaTake(ipstkCkptSem, L7_WAIT_FOREVER);
  for (index = 1; index <= L7_RTR_MAX_RTR_INTERFACES; index++)
  {
    if (ipstkCkptInternalVlanTlv[index].value.intIfNum == intIfNum)
    {
      /* An interface number to internal VLAN mapping was checkpointed
       * for this interface. */
      entryFound = L7_TRUE;
      break;
    }
  }

  /* Add entry, if it does not exist */
  if (entryFound == L7_TRUE)
  {
    rtrIntfMap[rtrIfNum].internalVlan = ipstkCkptInternalVlanTlv[index].value.internalVlan;
  }
  osapiSemaGive(ipstkCkptSem);

  return L7_SUCCESS;
}

#else

/*********************************************************************
* @purpose  On a warm restart, apply checkpoint rtr interface data for
*           ports which are still operational.
*
* @param  intIfNum   @b{(input)}  internal interface number
*
* @returns  void
*
* @notes    invoked under protection of ipstkSharedLibSema
*
* @end
*********************************************************************/
L7_RC_t ipstkCheckpointRtrIntfDataApply(L7_uint32 intIfNum)
{
  return L7_SUCCESS;
}

#endif /* L7_ROUTING_PACKAGE */

#ifdef L7_ROUTING_PACKAGE
/*********************************************************************
* @purpose  When dot1q assigns an internal VLAN ID to a port based
*           routing interface, checkpoint the mapping to the
*           backup unit in a stack.
*
* @param  intIfNum   @b{(input)}  internal interface number
*
* @returns  void
*
* @notes    called under protection of ipstkSharedLibSema
*
* @end
*********************************************************************/
L7_RC_t ipstkCheckpointRtrIntfDataCallback(L7_uint32 intIfNum)
{
  L7_uint32 rtrIfNum;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_INTF_TYPES_t sysIntfType;
  L7_RC_t rc;
  ipstkCkptInternalVlanData_t  vlanMapMsg;

  if (nimGetIntfName(intIfNum, L7_SYSNAME, ifName) != L7_SUCCESS)
  {
    sprintf(ifName, "intIfNum %d", intIfNum);
  }

  /* If port based routing interface, get internal vlan. */
  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
            "Failed to get interface type for interface %s.", ifName);
    return L7_FAILURE;
  }
  if (sysIntfType != L7_PHYSICAL_INTF)
  {
    /* Nothing to do */
    return L7_SUCCESS;
  }

  if (ipstkIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
            "Failed to get router interface number for port based routing interface %s.",
            ifName);
    return L7_FAILURE;
  }

  vlanMapMsg.internalVlan = rtrIntfMap[rtrIfNum].internalVlan;
  vlanMapMsg.intIfNum = intIfNum;

  /* This function can be called on checkpoint service thread. That's the same thread
   * that reads from the message queue. If the queue is already full, doing L7_WAIT_FOREVER
   * will cause a system deadlock. So use L7_NO_WAIT. */
  rc = osapiMessageSend(ipstkCkptQueue, &vlanMapMsg, sizeof(ipstkCkptInternalVlanData_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  if (rc != L7_SUCCESS)
  {
    /* Only ask checkpoint service for checkpoint callback if new mapping
     * was added to the message queue. */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
            "Failed to checkpoint internal VLAN ID for port based routing interface %s.",
            ifName);
    return rc;
  }

#if L7_NSF_PACKAGE
  rc = ckptMgrCheckpointStart(L7_IP_MAP_COMPONENT_ID);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Request for checkpoint message callback failed with error %d", rc);
  }
#endif
  return rc;
}

/*********************************************************************
* @purpose  Mark the information for router interfaces
*           that may need to be checkpointed to Standby unit
*
* @param  void
*
* @returns  void
*
* @notes    Can be called when a new backup unit is reported
*
* @notes    invoked under thread of checkpoint task
*
* @end
*********************************************************************/
void ipstkCheckpointRtrIntfDataPopulate(void)
{
  L7_uint32 index;

  if (osapiSemaTake(ipstkSharedLibSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    return;

  for (index = 1; index <= L7_RTR_MAX_RTR_INTERFACES; index++)
  {
    if ((rtrIntfMap[index].present == L7_TRUE) &&
        (rtrIntfMap[index].internalVlan != 0))
    {
      /* Entry exists. Checkpoint it */
      ipstkCheckpointRtrIntfDataCallback(rtrIntfMap[index].intIfNum);
    }
  }
  osapiSemaGive(ipstkSharedLibSema);
  return;
}


/*********************************************************************
* @purpose  Flush any unapplied interface data on the management unit
*
* @param  void
*
* @returns  void
*
* @notes  There is no osapiMessageQueueFlush api, so read all events
*         out of the message queue
*
* @end
*********************************************************************/
L7_RC_t ipstkCheckpointRtrIntfDataFlush(void)
{

  L7_RC_t         rc = L7_SUCCESS;
  ipstkCkptInternalVlanData_t  vlanMapMsg;

  while (rc == L7_SUCCESS)
  {
    rc = osapiMessageReceive(ipstkCkptQueue, &vlanMapMsg,
                             sizeof(ipstkCkptInternalVlanData_t), L7_NO_WAIT);
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Flush any unapplied interface data on the backup unit
*
* @param  void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipstkCheckpointBackupUnitRtrIntfDataFlush(void)
{
  osapiSemaTake(ipstkCkptSem, L7_WAIT_FOREVER);
  memset((void *)ipstkCkptInternalVlanTlv, 0,
         sizeof(ipstkCkptInternalVlanTlv_t) * (L7_RTR_MAX_RTR_INTERFACES + 1));
  osapiSemaGive(ipstkCkptSem);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Process a single internal VLAN TLV from a checkpoint message
*           received on the backup unit.
*
* @param    intlVlanTlv   @b{(input)}  Start of internal VLAN TLV
*
* @returns  Number of bytes processed from the checkpoint message
*
* @notes    This function executes while IP MAP is in WMU state. It runs
*           on the checkpoint service thread.
*
* @end
*********************************************************************/
L7_uint32 ipstkCheckpointIntlVlanProcess(L7_uchar8 *ckptData)
{
  ipstkCkptInternalVlanTlv_t *pCkptData;
  L7_uint32 index;
  L7_uint32 intIfNum;
  L7_uint32 vlanId;
  L7_uint32 firstAvailable;
  L7_BOOL   entryFound;
  L7_uint32 bytesProcessed;

  firstAvailable = 0;
  entryFound = L7_FALSE;
  bytesProcessed = 0;

  pCkptData = (ipstkCkptInternalVlanTlv_t*)(ckptData);
  intIfNum = pCkptData->value.intIfNum;
  vlanId = pCkptData->value.internalVlan;

  ckptRtrIntfRecd++;

  osapiSemaTake(ipstkCkptSem, L7_WAIT_FOREVER);

  /* Update checkpoint database */
  for (index = 1; index <= L7_RTR_MAX_RTR_INTERFACES; index++)
  {
    if (ipstkCkptInternalVlanTlv[index].value.intIfNum == 0)
    {
      /* Save first empty entry */
      if (firstAvailable == 0)
      {
        firstAvailable = index;
      }
    }
    else
      if (ipstkCkptInternalVlanTlv[index].value.intIfNum == intIfNum)
    {
      /* Entry already exists.*/
      entryFound = L7_TRUE;
      break;
    }
  }

  /* Add entry, if it does not exist */
  if (entryFound ==  L7_FALSE)
  {
    if (firstAvailable != 0)
    {
      index = firstAvailable;
    }
    else
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      /* No entry to update. Cannot convert intIfNum to interface name because
       * this function executes in WMU, when NIM is not populated with interface
       * data. */
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Failed to store checkpoint data for port-based router interface %s",
              ifName);
      osapiSemaGive(ipstkCkptSem);
      return 0;
    }
  }

  ipstkCkptInternalVlanTlv[index].value.internalVlan = vlanId;
  if (vlanId != 0)
  {
    ipstkCkptInternalVlanTlv[index].value.intIfNum = intIfNum;
  }
  else
  {
    /* If VLAN ID is 0, delete the entry from the table by zeroing intIfNum. If
     * we don't do this, we can never learn more than the first 128 mappings,
     * even if some get deleted. */
    ipstkCkptInternalVlanTlv[index].value.intIfNum = 0;
  }

  bytesProcessed = sizeof(ipstkCkptInternalVlanTlv_t);
  ckptBytesRecd += bytesProcessed;

  osapiSemaGive(ipstkCkptSem);

  return bytesProcessed;
}



/*********************************************************************
* @purpose Display checkpoint data
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ipstkCheckpointDebugInfoShow(void)
{

  L7_uint32 index;
  printf("\nTransmit checkpoint stats:");
  printf("\n  Bytes:            %u", ckptBytesSent);
  printf("\n  ckptRtrIntfSent:  %u", ckptRtrIntfSent);

  printf("\nReceive checkpoint stats:");
  printf("\n  Bytes:            %u", ckptBytesRecd);
  printf("\n  ckptRtrIntfRecd:  %u", ckptRtrIntfRecd);


  printf("\n");
  printf("\n");
  /* Print checkpoint info */
  for (index = 1; index <= L7_RTR_MAX_RTR_INTERFACES; index++)
  {
     if (ipstkCkptInternalVlanTlv[index].value.intIfNum != 0)
     {
         printf("\n");
         printf("Printing Entry\n");
         printf("--------------\n");
         printf("   index  = %d\n", index);
         printf("   intIfNum  = %d\n", ipstkCkptInternalVlanTlv[index].value.intIfNum);
         printf("   internalVlan  = %d\n", ipstkCkptInternalVlanTlv[index].value.internalVlan);
     }
  }
}


/*********************************************************************
* @purpose Reset checkpoint statistics
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ipstkCheckpointDebugStatsReset(void)
{
  ckptBytesSent = 0;
  ckptBytesRecd = 0;
  ckptRtrIntfRecd = 0;
  ckptRtrIntfSent = 0;
}


#endif /* L7_ROUTING_PACKAGE */
#endif /* L7_NSF_PACKAGE */



#ifdef L7_ROUTING_PACKAGE
/* debug */

/*********************************************************************
* @purpose Display checkpoint data
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ipstkDebugShow(void)
{
    L7_uint32 i;

    for (i = 1; i <= L7_RTR_MAX_RTR_INTERFACES; i++)
    {
      if(rtrIntfMap[i].present == L7_TRUE)
      {
          printf("Printing present entries\n");
          printf("------------------------\n");
          printf("   index  = %d\n", i);
          printf("   intIfNum  = %d\n", rtrIntfMap[i].intIfNum);
          printf("   internalVlan  = %d\n", rtrIntfMap[i].internalVlan);
          printf("   allocFlags  = 0x%x\n", rtrIntfMap[i].allocFlags);
          printf("   is_attached  = %d\n", rtrIntfMap[i].is_attached);
          printf("   upFlags  = %d\n", rtrIntfMap[i].upFlags);
      }
    }
}

#endif /* L7_ROUTING_PACKAGE */
