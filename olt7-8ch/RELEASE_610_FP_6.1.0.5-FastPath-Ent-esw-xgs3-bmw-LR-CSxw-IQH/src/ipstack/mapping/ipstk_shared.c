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


typedef struct L7_ipstkRtrInterface_s
{
  L7_BOOL             present;
  L7_uint32           intIfNum;
  L7_uint32           allocFlags;
  L7_BOOL             is_attached;
  L7_BOOL             is_up;
  L7_uint32           upFlags;

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


/*********************************************************************
* @purpose  initialize  shared l3 library 
*
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
             LOG_MSG("ipstkRtrIfNumGet: IfIndexGet failed");
       }
       else
       {
             ifmap = &ifIndexMap[intIfNum];
             ifmap->s_intIfNum = intIfNum;
             if(ifmap->s_ifIndex != 0){
                 LOG_MSG("ipstkRtrIfNumGet: corrupt ifindex map\n");
             }
             ifmap->s_ifIndex = stackIfIndex;
             /* link into hash bin */
             head = &stifi_hash[stackIfIndex & STIFI_HASH_MASK];
             ifmap->s_fwd = head->b_top;
             head->b_top = ifmap;
             
       }

       *rtrIfNum = i;
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
L7_RC_t ipstkRtrIfNumRelease(L7_uint32 rtrIfNum,L7_uint32 compId)
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
            LOG_MSG("ipstkRtrIfNumRelease: bad ifindex unlink\n");
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
      if (dot1qInternalVlanAssign(intIfNum, &vlanId) != L7_SUCCESS)
      {
        /* clear flag so we can try again next time. */
        rtrDtlRoutingFlags[intIfNum] &= ~compFl;
        osapiSemaGive(ipstkSharedLibSema);
        return L7_FAILURE;
      }
    }
    else if (event == L7_DISABLE)
    {
      /* ignore return */
      dot1qInternalVlanRelease(intIfNum);
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
  memset(rtrPortRoutingFlags ,0,sizeof(L7_uint32) *(L7_MAX_INTERFACE_COUNT+1));
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



