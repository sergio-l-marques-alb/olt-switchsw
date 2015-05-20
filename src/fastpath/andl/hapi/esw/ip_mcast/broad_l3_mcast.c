/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: broad_l3_mcast.c
*
* Purpose: This file contains the entry point to the Layer 3 IP Multicast hapi interface
*
* Component: hapi
*
* Comments:
*
* Created by: Grant Castrodale 12/2/2002
*
*********************************************************************/
#include <string.h>

#include "l7_common.h"
#include "sysapi.h"
#include "osapi.h"
#include "sysnet_api.h"

#include "broad_common.h"
#include "broad_l3_mcast.h"
#include "broad_l2_lag.h"
#include "broad_policy.h"

#include "bcm/ipmc.h"
#include "soc/macipadr.h"
#include "bcmx/ipmc.h"
#include "bcmx/l3.h"
#include "bcmx/bcmx_int.h"
#include "bcmx/switch.h"
#include "l7_usl_bcmx_l3.h"
#include "l7_usl_bcmx_port.h"
#include "l7_usl_bcmx_l2.h"
#include "l7_usl_bcmx_ipmcast.h"

#include "osapi_support.h"
#ifdef L7_WIRELESS_PACKAGE
#include "broad_l2_tunnel.h"
#endif

#define BROAD_L3MCAST_DEBUG
#ifdef BROAD_L3MCAST_DEBUG
#define HAPI_L3_MCAST_DEBUG(fmt, args...)         {                                                 \
                                                    if(broadL3McastDebug)                           \
                                                    { sysapiPrintf("\n%s [%d]: ", __FUNCTION__,__LINE__); \
                                                      sysapiPrintf(fmt, ##args);                          \
                                                    }                                               \
                                                  }

#define HAPI_L3_ADDR_PRINT_DEBUG(addr, addrType)  {                                                 \
                                                    if(broadL3McastDebug)                           \
                                                    { sysapiPrintf("\n%s [%d]: ", __FUNCTION__,__LINE__); \
                                                      printfIpAddress(addr, addrType);              \
                                                    }                                               \
                                                  }
#define HAPI_L3_PRINT_IPMC_DEBUG(ipmc)            {                                                 \
                                                    if(broadL3McastDebug)                           \
                                                    { sysapiPrintf("\n%s [%d]: ", __FUNCTION__,__LINE__); \
                                                      printfIPMC(ipmc);                             \
                                                    }                                               \
                                                  }

static L7_BOOL broadL3McastDebug = L7_FALSE;


void hapiBroadDebugL3McastEnable(L7_BOOL enable)  { broadL3McastDebug = enable; }

static void printfIPMC(usl_bcm_ipmc_addr_t *impc_info)
{
  int index;
  sysapiPrintf("\n***********************************");
  sysapiPrintf("\nflags          : 0x%x", impc_info->flags);
  sysapiPrintf("\nipmc_index     :   %d", impc_info->ipmc_index);
  sysapiPrintf("\nv              :   %d", impc_info->v);
  sysapiPrintf("\nts             :   %d", impc_info->ts);
  sysapiPrintf("\ncos            :   %d", impc_info->cos);
  sysapiPrintf("\nvrf            :   %d", impc_info->vrf);
  sysapiPrintf("\nvid            :   %d", impc_info->vid);
  sysapiPrintf("\nport_tgid      :   %d", impc_info->port_tgid);
  sysapiPrintf("\nmodid          :   %d", impc_info->mod_id);
  if (impc_info->flags & BCM_IPMC_IP6)
  {
    sysapiPrintf("\nSRC            :   ");
    for(index  = 0; index < 16; index++)
        sysapiPrintf("%x ",impc_info->s_ip_addr[index]);
    sysapiPrintf("\n");
    sysapiPrintf("\nGRP            :   ");
    for(index  = 0; index < 16; index++)
        sysapiPrintf("%x ",impc_info->mc_ip_addr[index]);
    sysapiPrintf("\n");
  }
  else
  {
    sysapiPrintf("\nSRC            :   ");
    sysapiPrintf("%x \n", impc_info->s_ip_addr);
    sysapiPrintf("\nGRP            :   ");
    sysapiPrintf("%x \n", impc_info->mc_ip_addr);
  }
  return;
}

static void printfIpAddress(L7_inet_addr_t ipAddr, L7_uchar8 *addrTpye)
{
  L7_uint32 index ;

  sysapiPrintf("%s : ", addrTpye);

  if(ipAddr.family == L7_AF_INET6)
  {
    for(index  = 0; index < 16; index++)
      sysapiPrintf("%x:",ipAddr.addr.ipv6.in6.addr8[index]);
    sysapiPrintf("\n");
  }
  else if(ipAddr.family == L7_AF_INET)
  {
    sysapiPrintf("%x \n", ipAddr.addr.ipv4.s_addr);
  }
  else
  {
    sysapiPrintf("Wrong Family Type %d \n", ipAddr.family);
  }
  return;
}
#else
#define HAPI_L3_MCAST_DEBUG(fmt, args...)
#define HAPI_L3_ADDR_PRINT_DEBUG(addr, addrType)
#define HAPI_L3_PRINT_IPMC_DEBUG(ipmc)
#endif

#define GEN_ADDR_STR_LEN sizeof("01:02:03:04:05:06:07:08:09:10:11:12:13:14:15:16")

static void hapiBroadL3McastSetIpAddress(L7_inet_addr_t *srcIp, L7_inet_addr_t *grpIp, usl_bcm_ipmc_addr_t *ipmc)
{
  memset(ipmc->s_ip_addr,  0, sizeof(ipmc->s_ip_addr));
  memset(ipmc->mc_ip_addr, 0, sizeof(ipmc->mc_ip_addr));

  if ((srcIp->family == L7_AF_INET6) && (grpIp->family == L7_AF_INET6))
  {
    memcpy((ipmc)->s_ip_addr, (L7_uchar8 *)(srcIp)->addr.ipv6.in6.addr8, 16);
    memcpy((ipmc)->mc_ip_addr,(L7_uchar8 *)(grpIp)->addr.ipv6.in6.addr8, 16);
    (ipmc)->flags |= BCM_IPMC_IP6;
  }
  else
  {
    memcpy((ipmc)->s_ip_addr, &(srcIp)->addr.ipv4.s_addr, 4);
    memcpy((ipmc)->mc_ip_addr, &(grpIp)->addr.ipv4.s_addr, 4);
  }
}

/* This is the list of flags (yes, BITS!) for a logical port entry. */
typedef enum {
  BROAD_GROUP_LOGICAL_PORT_FLAGS_CLEAR  =  0, /* used for clearing */
  BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_USE =  1, /* if set, indicates port is intended to be in use */
  BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_HW  =  2, /* if set, indicates port is set in hardware */
  BROAD_GROUP_LOGICAL_PORT_FLAGS_REMOVE =  4, /* if set, indicates a deleted interface to be removed */
  BROAD_GROUP_LOGICAL_PORT_FLAGS_EGRESS_SNOOP_MODIFY  = 0x08  /* if set, indicates this entry's egress path is modified by snooping */
} BroadGroupLogicalPortFlags;

typedef struct
{
  BroadGroupLogicalPortFlags flags;

  /* Router interface USP. For port-based router interfaces this value is equal
  ** to the physical USP. For VLAN-based interefaces all ports that are members of
  ** the VLAN are also downstream ports.
  */
  DAPI_USP_t  usp;

  L7_uint32     ttl;

} BroadGroupLogicalPortEntryType;

/* This is the list of flags (yes, BITS!) for a table entry */
typedef enum
{
  BROAD_GROUP_ENTRY_FLAGS_CLEAR         = 0x00, /* Used to clear the flags */
  BROAD_GROUP_ENTRY_FLAGS_IN_USE        = 0x01, /* if set, indicates this entry is in use */
  BROAD_GROUP_ENTRY_FLAGS_IN_HW         = 0x02, /* if set, indicates this entry is in the hardware */
  BROAD_GROUP_ENTRY_FLAGS_IN_HW_FAIL    = 0x04, /* if set, indicates this entry failed to be inserted to hardware */
  BROAD_GROUP_ENTRY_FLAGS_INGRESS_IN_HW = 0x08, /* if set, indicates this entry's ingress path is in the hardware */
  BROAD_GROUP_ENTRY_FLAGS_INGRESS_SNOOP_MODIFY = 0x10  /* if set, indicates this entry's ingress path is modified by snooping */
} BroadGroupEntryFlags;

typedef struct
{
  BroadGroupEntryFlags flags;

  /* Group address
  */
  L7_inet_addr_t  groupIp;

  /* Source address
  */
  L7_inet_addr_t srcAddr;

  /* Vlan of incoming IP mcast traffic */
  L7_ushort16   vlanId;

  /* RPF Information.
  ** Note: For Broadcom, only the incoming interface method is supported for RPF. Also, RPF checking
  ** is enabled per chip, not per multicast routing entry. All FASTPATH multicast schemes use
  ** RPF so it is enabled per chip and the value in the mcastAdd command is ignored. There are 2
  ** actions for RPF failures: drop and copy to CPU. This is controlled per multicast routing entry.
  ** For VLAN routing interfaces, the RPF action must be set to copy to CPU so that the driver can
  ** intercept the packet and determine whether it's a real RPF failure (in which case it can drop
  ** if needed) or just a port move within the incoming-vlan.
  */
  L7_BOOL               rpf_check_enabled; /* Broadcom does RPF check for all entries */
  DAPI_MCAST_RPF_CHECK_FAIL_ACTION_t  action_on_rpf_failure;
  DAPI_MCAST_RPF_CHECK_METHOD_t     rpf_check_type; /* Broadcom does only incoming interface check */

  DAPI_USP_t  rpf_usp;  /* Unit.Slot.Port for the RPF check. */

  /* For port-based VLAN interfaces this value is the same as rpf_usp. For vlan-based
  ** routing interfaces this value is the actual physical (or LAG) port on which we expect to
  ** receive the frame.
  */
  DAPI_USP_t  rpf_physical_usp;

  /* Downstream router interface information.
  */
  BroadGroupLogicalPortEntryType downstream_if[L7_MAX_NUM_ROUTER_INTF];

  /* Number of downstream router interfaces.
  */
  L7_uint32  num_ifs;

  /* Count of routed IP multicast packets for this group.
  */
  L7_uint32 last_count;

  L7_uint32 ipmc_index;

} BroadGroupEntryType;

/* Last time we emitted a collision alert callback */
static L7_uint32 lastCollisionCallbackTime = 0;
/* Minimum delay between collision alert callbacks */
#define BROAD_MCAST_COLLISION_ALERT_DELAY 300

/*
** How to handle an add with the table full...
**    zero = log problem (critical), make table full callback, discard new add
**    nonzero = log problem (emergency), display some data, invoke L7_LOG_ERROR
*/
#define BROAD_MCAST_CRASH_ON_TABLE_FULL 0

/* Multicast group database. */
static BroadGroupEntryType BroadGroupList[L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL]; /* group information */
static L7_uint32  groupEntriesInUse    = 0;        /* number of groups in use */
static L7_uint32  groupEntriesInHw     = 0;        /* number of groups in hardware */
static L7_uint32  groupAppInsert       = 0;        /* number of application group inserts attempted */
static L7_uint32  groupAppInsertFail   = 0;        /* number of application group inserts that failed */
static L7_uint32  groupRetryInsert     = 0;        /* number of retry group inserts attempted */
static L7_uint32  groupRetryInsertFail = 0;        /* number of retry group inserts that failed */
static L7_BOOL    groupWasDeleted      = L7_FALSE; /* indicates rescan pending adds; set true after usl_bcmx_ipmc_remove */
static void      *hapiBroadL3McastSema = L7_NULL;  /* used for locking the above items */

/* Multicast async task declarations. */
static void *hapiBroadRoutingMcastQueue;       /* Work available queue. */
static void *hapiBroadMcastAsyncCmdQueue;      /* Multicast group add/remove queue. */
static void *hapiBroadMcastAsyncRpfQueue;      /* Queue for RPF resolution events. */
static void *hapiBroadL3McastAsyncWaitSema;    /* Wait for the mcast async task. */
void hapiBroadRoutingMcastAsyncTask(L7_uint32 num_args, DAPI_t *dapi_g); /* The async task main loop */
void hapiBroadL3McastAsyncFailureCallback(void *data);
void hapiBroadL3McastAsyncNotify(void);        /* API to tell Mcast async task that there is work to do. */

/* Various statistics */
static struct
{
  L7_uint32 high_route_queue_length;           /* Max route queue depth */
  L7_uint32 total_rpf_callbacks;               /* RPF callbacks */
  L7_uint32 rpf_queue_full_drops;              /* RPF callbacks dropped -- queue full */
  L7_uint32 rpf_duplicate_drops;               /* RPF callbacks dropped -- duplicate */
  L7_uint32 rpf_handle_count;                  /* RPF handler invocation count */
} mcast_stats;

/* Indicates initialisation has been performed */
static L7_BOOL hapi_mcast_inited = L7_FALSE;

/*********************************************************************
** Display Mcast async task stats.
**
*********************************************************************/
L7_uint32 hapiBroadL3McastStats (void)
{
  L7_RC_t rc;
  L7_uint32 num_msgs;

  sysapiPrintf ("Group add/delete queue size = %d\n", HAPI_BROAD_MCAST_ASYNC_CMD_QUEUE_SIZE);
  sysapiPrintf ("Group add/delete queue high = %d\n", mcast_stats.high_route_queue_length);

  rc = osapiMsgQueueGetNumMsgs (hapiBroadMcastAsyncCmdQueue, &num_msgs);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR (rc);
  }
  sysapiPrintf ("Group add/delete queue current = %d\n", num_msgs);
  sysapiPrintf("Total RPF callbacks = %d\n", mcast_stats.total_rpf_callbacks);
  sysapiPrintf("RPF Queue Full Drops = %d\n", mcast_stats.rpf_queue_full_drops);
  sysapiPrintf("RPF Duplicate Drops = %d\n", mcast_stats.rpf_duplicate_drops);
  sysapiPrintf("RPF Handle Count = %d\n", mcast_stats.rpf_handle_count);

  return 0;
}

/*********************************************************************
*
* @purpose Re-initialize multicast global data.
*
* @param   none
*
* @returns none
*
* @notes
*
* @end
*
*********************************************************************/
void hapiBroadL3McastReInit (void)
{
  L7_uint32 i;

  if (hapi_mcast_inited == L7_FALSE)
  {
    return;
  }

  hapi_mcast_inited = L7_FALSE;

  /* This function is called to clear out any global state
  ** maintained by the IP multicast code. The function should be
  ** invoked in conjunction with soft-reset.
  */
  memset(BroadGroupList, 0, sizeof(BroadGroupList));
  groupEntriesInUse    = 0;
  groupEntriesInHw     = 0;
  groupAppInsert       = 0;
  groupAppInsertFail   = 0;
  groupRetryInsert     = 0;
  groupRetryInsertFail = 0;
  groupWasDeleted      = L7_FALSE;

  for (i = 0; i < L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL; i++)
  {
    BroadGroupList[i].ipmc_index = i;
  }

  hapi_mcast_inited = L7_TRUE;

  return;
}

/*********************************************************************
*
* @purpose Enter L3 Multicast critical section.
*
*
* @param   none
*
* @returns none
*
* @notes
*
* @end
*
*********************************************************************/
void hapiBroadL3McastCritEnter (void)
{
  L7_RC_t rc;

  /* See if the semaphore already exists; create it if not */
  if (!hapiBroadL3McastSema)
  {
    /* First time in this function create the semaphore. */
    hapiBroadL3McastSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
    if (!hapiBroadL3McastSema)
    {
      /* Not good - semaphore create failed */
      L7_LOG_ERROR(0xDEAD1055);
    }
  }

  /* Now we have a semaphore; take it */
  rc = osapiSemaTake(hapiBroadL3McastSema,L7_WAIT_FOREVER);
  if (rc != L7_SUCCESS)
  {
    /* Take failed for some reason! */
    L7_LOG_ERROR (rc);
  }
}

/*********************************************************************
*
* @purpose Exit L3 Multicast critical section.
*
*
* @param   none
*
* @returns none
*
* @notes
*
* @end
*
*********************************************************************/
void hapiBroadL3McastCritExit (void)
{
  L7_RC_t rc;

  rc = osapiSemaGive(hapiBroadL3McastSema);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR (rc);
  }
}

/*********************************************************************
*
* @purpose Return index of the matching downstream entry in the BroadGroupList.
*
* @param usp - USP to match
* @param table_index - BroadGroupList index to search.
* @param index     - Matching index or first empty slot.
*
* @returns L7_SUCCESS - Match found.
*          L7_FAILURE - Match is not found, index is set to first empty slot.
*
* @notes If table is full and match is not found then index is
*        set to L7_MAX_NUM_ROUTER_INTF.
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadL3McastFindPort(DAPI_USP_t *usp,
                                        L7_uint32 table_index,
                                        L7_uint32 *index)
{
  L7_uint32 i;
  L7_uint32 first_empty_index = L7_MAX_NUM_ROUTER_INTF;

  for (i = 0; i < L7_MAX_NUM_ROUTER_INTF; i++ )
  {
    if (BroadGroupList[table_index].downstream_if[i].flags & BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_USE)
    {
      if (memcmp (usp, &BroadGroupList[table_index].downstream_if[i].usp,
                  sizeof (DAPI_USP_t)) == 0)
      {
        *index = i;
        return L7_SUCCESS;
      }
    }
    else
    {
      if (i < first_empty_index)
      {
        first_empty_index = i;
      }
    }
  }

  *index = first_empty_index;
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Return index in the BroadGroupList that matches the
*          input group/source addresses. If group is not found
*          then group index points to the first empty slot. If table
*          is full then the index is L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL
*
* @param group_addr - Group address
* @param source_addr - Source address
* @param index     - Matching index or first empty slot.
*
* @returns L7_SUCCESS - Match found.
*          L7_FAILURE - Match is not found, index is set to first empty slot.
*
* @notes none
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadL3McastFindGroup(L7_inet_addr_t *group_addr,
                                         L7_inet_addr_t *source_addr,
                                         L7_uint32 *index)
{
  L7_uint32 i;
  L7_uint32 first_empty_index = L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL;

  for (i = 0; i < L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL; i++ )
  {
    if (BroadGroupList[i].flags & BROAD_GROUP_ENTRY_FLAGS_IN_USE)
    {
      if ((L7_INET_ADDR_COMPARE(group_addr,&BroadGroupList[i].groupIp) == 0) &&
          (L7_INET_ADDR_COMPARE(source_addr,&BroadGroupList[i].srcAddr) == 0))
      {
        *index = i;
        HAPI_L3_MCAST_DEBUG("L7_SUCCESS = hapiBroadL3McastFindGroup(...,...,%d)",i);
        return L7_SUCCESS;
      }
    }
    else
    {
      if (i < first_empty_index)
      {
        first_empty_index = i;
      }
    }
  }

  *index = first_empty_index;
  HAPI_L3_MCAST_DEBUG("L7_FAILURE = hapiBroadL3McastFindGroup(...,...,%d)",first_empty_index);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Add, modify, or delete incoming vlan members as bridge ports
*
* @param  usp
* @param  groupIp
* @param  srcAddr
* @param  vid
* @param *dapi_g
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
static void hapiBroadL3McastAddVlanMembersAsL2Ports (DAPI_USP_t *usp,
                                        L7_inet_addr_t *groupIp,
                                        L7_inet_addr_t *srcAddr,
                                        bcm_vlan_t *vid,
                                        L7_uint32  ipmc_index,
                                        DAPI_t     *dapi_g)
{
  L7_int32                      rv;
  DAPI_PORT_t                  *dapiPortPtr2;
  DAPI_PORT_t                  *dapiPortPtr;
  BROAD_PORT_t                 *hapiPortPtr;
  BROAD_PORT_t                 *hapiLagMemberPortPtr;
  DAPI_USP_t                    searchUsp;
  DAPI_LAG_ENTRY_t             *lagMemberSet;
  L7_uint32                     lagMemberIndex;
  bcm_chip_family_t             board_family=0; 
  usl_bcm_ipmc_addr_t           ipmcAddress;
  L7_int32                      bcmPort, modid;

  memset(&ipmcAddress, 0, sizeof(ipmcAddress));
 
  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_TRUE)
  {

    /* For VLAN-based routing interface find all ports that are members of the
    ** incoming VLAN and add them to the list of ports to bridge the frame to.
    ** Skip ports that are LAG members or port-based routers.
    */

    for (searchUsp.unit=0; searchUsp.unit < dapi_g->system->totalNumOfUnits; searchUsp.unit++)
    {
      for (searchUsp.slot=0; searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots; searchUsp.slot++)
      {
        if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardPresent == L7_TRUE)
        {
          for (searchUsp.port=0; searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot; searchUsp.port++)
          {
            if (isValidUsp (&searchUsp, dapi_g) != L7_TRUE)
            {
             continue;
            }

            dapiPortPtr2 = dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->port[searchUsp.port];
            hapiPortPtr  = dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->port[searchUsp.port]->hapiPort;

            if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr2) == L7_TRUE) ||
               (IS_PORT_TYPE_CAPWAP_TUNNEL(dapiPortPtr2) == L7_TRUE))
            {
              if ((hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE) ||
                  (dapiPortPtr2->modeparm.physical.routerIntfEnabled == L7_TRUE))
              {
                continue;
              }

              if (BROAD_IS_VLAN_MEMBER(&searchUsp, *vid, dapi_g) )
              {
                modid   = BCM_GPORT_MODPORT_MODID_GET(hapiPortPtr->bcmx_lport);
                bcmPort = BCM_GPORT_MODPORT_PORT_GET(hapiPortPtr->bcmx_lport);

                if ((modid < 0) || (bcmPort < 0))
                {
                  continue;    
                }
 
                BCM_PBMP_PORT_ADD(ipmcAddress.l2_pbmp[modid], bcmPort);

                HAPI_L3_MCAST_DEBUG("Inserted port %d to the l2_port_list", hapiPortPtr->bcmx_lport);  
                if (!BROAD_IS_VLAN_TAGGING(&searchUsp, *vid, dapi_g))
                {
                  BCM_PBMP_PORT_ADD(ipmcAddress.l2_ubmp[modid], bcmPort);
                }
              }
            }
          }
        }
      }
    }

    /* Now go through the LAG interfaces. For XGS3, add all member ports to the bitmap. 
       Otherwise add only one member port. */
    hapiBroadGetSystemBoardFamily(&board_family);

    searchUsp.unit = L7_LOGICAL_UNIT;
    searchUsp.slot = L7_LAG_SLOT_NUM;
    for (searchUsp.port=0; searchUsp.port < L7_MAX_NUM_LAG_INTF; searchUsp.port++)
    {
      dapiPortPtr2 = dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->port[searchUsp.port];
      hapiPortPtr  = dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->port[searchUsp.port]->hapiPort;

      if (BROAD_IS_VLAN_MEMBER(&searchUsp, *vid, dapi_g) )
      {
        lagMemberSet = DAPI_PORT_GET(&searchUsp, dapi_g)->modeparm.lag.memberSet;

        for (lagMemberIndex = 0; lagMemberIndex < L7_MAX_MEMBERS_PER_LAG; lagMemberIndex++)
        {
          if (lagMemberSet[lagMemberIndex].inUse == L7_TRUE)
          {
            hapiLagMemberPortPtr = HAPI_PORT_GET(&lagMemberSet[lagMemberIndex].usp,dapi_g);

            modid   = BCM_GPORT_MODPORT_MODID_GET(hapiLagMemberPortPtr->bcmx_lport);
            bcmPort = BCM_GPORT_MODPORT_PORT_GET(hapiLagMemberPortPtr->bcmx_lport);
  
            if ((modid < 0) || (bcmPort < 0))
            {
              continue;    
            }

            BCM_PBMP_PORT_ADD(ipmcAddress.l2_pbmp[modid], bcmPort);

            HAPI_L3_MCAST_DEBUG("Inserted port %d to the l2_port_list", hapiPortPtr->bcmx_lport); 
            if (!BROAD_IS_VLAN_TAGGING(&searchUsp, *vid, dapi_g))
            {
              BCM_PBMP_PORT_ADD(ipmcAddress.l2_ubmp[modid], bcmPort);
            }

            /*For XGS3 we can distribute non-unicast
                     traffic across the lag members*/
            if ((board_family == BCM_FAMILY_DRACO) ||
                (board_family == BCM_FAMILY_TUCANA))
            {
              break;
            }
          }
        }
      }
    }

    ipmcAddress.vid = *vid;
    ipmcAddress.ipmc_index = ipmc_index;
    hapiBroadL3McastSetIpAddress(srcAddr, groupIp, &ipmcAddress);
    rv = usl_bcmx_ipmc_set_l2_ports(&ipmcAddress);

    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      HAPI_L3_MCAST_DEBUG(" rv is %d ", rv);  
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set L2 ports in IPMC group, rv = %d", rv);
    }
  }
}
/*********************************************************************
*
* @purpose Add, modify, or delete incoming vlan bridge ports
*
* @param  usp
* @param  groupIp
* @param  srcAddr
* @param  vid
* @param  l2BitmapList      List of ports to be added in L2 Bitmap
* @param  l2BitmapListCount The number of ports sent in bitmap list
* @param *dapi_g
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
static void hapiBroadL3McastAddL2Ports (DAPI_USP_t *usp,
                                        L7_inet_addr_t *groupIp,
                                        L7_inet_addr_t *srcAddr,
                                        bcm_vlan_t *vid,
                                        L7_uint32  ipmc_index,
                                        DAPI_USP_t *l2BitmapList,
                                        L7_uint32  l2BitmapListCount,
                                        DAPI_t     *dapi_g)
{
  L7_int32                      rv;
  DAPI_PORT_t                  *dapiPortPtr2;
  DAPI_PORT_t                  *dapiPortPtr;
  BROAD_PORT_t                 *hapiPortPtr;
  BROAD_PORT_t                 *hapiLagMemberPortPtr;
  DAPI_LAG_ENTRY_t             *lagMemberSet;
  L7_uint32                     i;
  usl_bcm_ipmc_addr_t           ipmcAddress;
  L7_int32                      bcmPort, modid;

  memset(&ipmcAddress, 0, sizeof(ipmcAddress));

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_TRUE)
  {
    /* For VLAN-based routing interface find all ports that are members of the
    ** incoming VLAN and add them to the list of ports to bridge the frame to.
    ** Skip ports that are LAG members or port-based routers.
    */

    if (l2BitmapListCount)
    {
      if (l2BitmapList != L7_NULLPTR)
      {
        for (i = 0; i < l2BitmapListCount; i++)
        {
          dapiPortPtr2 = dapi_g->unit[l2BitmapList[i].unit]->slot[l2BitmapList[i].slot]->port[l2BitmapList[i].port];
          hapiPortPtr  = dapi_g->unit[l2BitmapList[i].unit]->slot[l2BitmapList[i].slot]->port[l2BitmapList[i].port]->hapiPort;
          if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr2) == L7_TRUE) ||
              (IS_PORT_TYPE_CAPWAP_TUNNEL(dapiPortPtr2) == L7_TRUE))
          {
            modid   = BCM_GPORT_MODPORT_MODID_GET(hapiPortPtr->bcmx_lport);
            bcmPort = BCM_GPORT_MODPORT_PORT_GET(hapiPortPtr->bcmx_lport);

            if ((modid != HAPI_BROAD_INVALID_MODID) &&  
                (bcmPort != HAPI_BROAD_INVALID_MODPORT))
            {
              BCM_PBMP_PORT_ADD(ipmcAddress.l2_pbmp[modid], bcmPort);

              HAPI_L3_MCAST_DEBUG("Inserted port %d to the l2_port_list", 
                                  hapiPortPtr->bcmx_lport);
              if (!BROAD_IS_VLAN_TAGGING(&l2BitmapList[i], *vid, dapi_g))
              {
                BCM_PBMP_PORT_ADD(ipmcAddress.l2_ubmp[modid], bcmPort);
              }
            }
          }
          else if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr2))
          {
            /* Add only one port */
            lagMemberSet = dapiPortPtr2->modeparm.lag.memberSet;
            if (lagMemberSet[0].inUse == L7_TRUE)
            {
              hapiLagMemberPortPtr = HAPI_PORT_GET(&lagMemberSet[0].usp,dapi_g);
              modid   = BCM_GPORT_MODPORT_MODID_GET(hapiLagMemberPortPtr->bcmx_lport);
              bcmPort = BCM_GPORT_MODPORT_PORT_GET(hapiLagMemberPortPtr->bcmx_lport);

              if ((modid != HAPI_BROAD_INVALID_MODID) &&
                  (bcmPort != HAPI_BROAD_INVALID_MODPORT))
              {
                BCM_PBMP_PORT_ADD(ipmcAddress.l2_pbmp[modid], bcmPort);

                HAPI_L3_MCAST_DEBUG("Inserted port %d to the l2_port_list", 
                                    hapiLagMemberPortPtr->bcmx_lport);
                if (!BROAD_IS_VLAN_TAGGING(&l2BitmapList[i], *vid, dapi_g))
                {
                  BCM_PBMP_PORT_ADD(ipmcAddress.l2_ubmp[modid], bcmPort);
                }
              }
            }
          }
        }
      }
    }

    ipmcAddress.vid = *vid;
    ipmcAddress.ipmc_index = ipmc_index;
    hapiBroadL3McastSetIpAddress(srcAddr, groupIp, &ipmcAddress);
    rv = usl_bcmx_ipmc_set_l2_ports(&ipmcAddress);

    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      HAPI_L3_MCAST_DEBUG(" rv is %d ", rv);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set L2 ports in IPMC group, rv = %d", rv);
    }

  }
}

/*********************************************************************
* Delete L3 port from a multicast group on Strata or XGS.
*
*********************************************************************/
static void hapiBroadL3McastDeleteL3Port (bcmx_lport_t lport,
                                       int        ipmc_index,
                                       L7_inet_addr_t groupIp,
                                       L7_inet_addr_t srcAddr,
                                       bcm_vlan_t group_vid,
                                       bcm_vlan_t vlan_id
                                       )
{
  int rv;

  rv = usl_bcmx_ipmc_delete_l3_port_groups(lport,
                                    &ipmc_index,
                                    1,
                                           vlan_id, 0, L7_NULLPTR, 0);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't delete L3 port 0x%x from IPMC index %d, rv = %d", lport, ipmc_index, rv);
  }

}

/*********************************************************************
* Add L3 port to a multicast group on Strata or XGS.
*
*********************************************************************/
static void hapiBroadL3McastAddL3Port (bcmx_lport_t lport,
                                       int        ipmc_index,
                                       L7_inet_addr_t groupIp,
                                       L7_inet_addr_t srcAddr,
                                       bcm_vlan_t group_vid,
                                       bcm_vlan_t vlan_id,
                                       int        ipmc_tagged,
                                       L7_uchar8  *mac,
                                       L7_uint32  ttl)
{
  int             rv;
  int             ipmc_untag_flag;

  HAPI_L3_MCAST_DEBUG("Entered lport %d vlan %d", lport, vlan_id);

  if (ipmc_tagged)
  {
    ipmc_untag_flag = FALSE;
  } else
  {
    ipmc_untag_flag = TRUE;
  }

  rv = usl_bcmx_ipmc_add_l3_port_groups (lport,
                                 &ipmc_index,
                                 1,
                                 vlan_id,
                                 ipmc_tagged,
                                 mac,
                                 ttl);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    HAPI_L3_MCAST_DEBUG("customx_ipmc_l3_port_add failed");
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't add L3 port 0x%x to IPMC index %d, rv = %d", lport, ipmc_index, rv);
  }
  HAPI_L3_MCAST_DEBUG("customx_ipmc_l3_port_add rv %d", rv);
}


/*********************************************************************
*
* @purpose Add, modify, or delete downstream port.
*
* @param  usp
* @param  groupIp
* @param  srcAddr
* @param  vid
* @param  ttl
* @param  add_remove
* @param *dapi_g
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
static void hapiBroadL3McastAddRemoveL3Ports (DAPI_USP_t *usp,
                                              L7_inet_addr_t *groupIp,
                                              L7_inet_addr_t *srcAddr,
                                              bcm_vlan_t *vid,
                                              L7_uint32  ttl,
                                              L7_BOOL    add_remove,
                                              DAPI_t     *dapi_g)
{
  L7_ushort16                   vlan_id;
  DAPI_PORT_t                  *dapiPortPtr2;
  DAPI_PORT_t                  *dapiPortPtr;
  BROAD_PORT_t                 *hapiPortPtr;
  BROAD_PORT_t                 *hapiLagMemberPortPtr;
  DAPI_USP_t                    searchUsp;
  int                           ipmc_tagged;
  DAPI_LAG_ENTRY_t             *lagMemberSet;
  L7_uint32                     ipmc_index;
  L7_int32                      tmpIndex;
  L7_uint32                     lagMemberIndex;
  bcm_chip_family_t             board_family=0;
  L7_uint32                     l3_interface_id=0;

  HAPI_L3_MCAST_DEBUG("Entered add_remove %d vlan id %d ", add_remove, *vid);

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* get the ipmc index */
  if (hapiBroadL3McastFindGroup(groupIp,srcAddr,&tmpIndex) != L7_SUCCESS)
  {
    HAPI_L3_MCAST_DEBUG("Error : Could not find the entry ");
    return;
  }

  ipmc_index = BroadGroupList[tmpIndex].ipmc_index;

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    /* setup the egress port */

    ipmc_tagged = FALSE; /* untagged */
    vlan_id = hapiPortPtr->port_based_routing_vlanid;

    if (add_remove)
    {
      hapiBroadL3McastAddL3Port (hapiPortPtr->bcmx_lport,
                                 ipmc_index,
                                 *groupIp,
                                 *srcAddr,
                                 *vid,
                                 vlan_id,
                                 ipmc_tagged,
                                 dapiPortPtr->modeparm.physical.macAddr,
                                 ttl);
    } else
    {
      hapiBroadL3McastDeleteL3Port (hapiPortPtr->bcmx_lport,
                                       ipmc_index,
                                        *groupIp,
                                       *srcAddr,
                                        *vid,
                                       vlan_id);
    }
  }
  else
  {
    vlan_id = dapiPortPtr->modeparm.router.vlanID;
    l3_interface_id = hapiPortPtr->l3_interface_id;
    /* For VLAN-based routing interface find all ports that are members of the
    ** VLAN and add them to the multicast group.
    ** Skip ports that are members of LAGs or port-based router.
    */

    for (searchUsp.unit=0; searchUsp.unit < dapi_g->system->totalNumOfUnits; searchUsp.unit++)
    {
      for (searchUsp.slot=0; searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots; searchUsp.slot++)
      {
        if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardPresent == L7_TRUE)
        {
          for (searchUsp.port=0; searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot; searchUsp.port++)
          {
            if (isValidUsp (&searchUsp, dapi_g) != L7_TRUE)
            {
             continue;
            }

            dapiPortPtr2 = dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->port[searchUsp.port];
            hapiPortPtr  = dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->port[searchUsp.port]->hapiPort;

            if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr2) == L7_TRUE) ||
               (IS_PORT_TYPE_CAPWAP_TUNNEL(dapiPortPtr2) == L7_TRUE))
            {
              if ((hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE) ||
                  (dapiPortPtr2->modeparm.physical.routerIntfEnabled == L7_TRUE))
              {
                continue;
              }

              if (BROAD_IS_VLAN_MEMBER(&searchUsp, vlan_id, dapi_g) )
              {
                /* setup the egress port */
                if (BROAD_IS_VLAN_TAGGING(&searchUsp, vlan_id, dapi_g))
                {
                  ipmc_tagged = TRUE; /* tagged */
                }
                else
                {
                  ipmc_tagged = FALSE; /* untagged */
                }

                if (BCM_GPORT_IS_WLAN_PORT(hapiPortPtr->bcmx_lport))
                {
              #ifdef L7_WIRELESS_PACKAGE
                  BROAD_WLAN_TUNNEL_ENTRY_t tunnelEntry;
                  if (hapiBroadWlanInfoGet(dapi_g, hapiPortPtr->bcmx_lport, &tunnelEntry) == L7_SUCCESS)
                  {
                    if (add_remove)
                    {
                      usl_bcmx_ipmc_wlan_l3_port_add(ipmc_index, l3_interface_id, vlan_id,
                                                     hapiPortPtr->bcmx_lport,
                                                     tunnelEntry.wlan_port.port);
                    }
                    else
                    {
                      usl_bcmx_ipmc_wlan_l3_port_delete(ipmc_index, l3_interface_id, vlan_id,
                                                     hapiPortPtr->bcmx_lport,
                                                     tunnelEntry.wlan_port.port);

                    }
                  }
              #endif
                }
                else
                {
                  if (add_remove)
                  {
                    hapiBroadL3McastAddL3Port (hapiPortPtr->bcmx_lport,
                                               ipmc_index,
                                                    *groupIp,
                                               *srcAddr,
                                                    *vid,
                                               vlan_id,
                                               ipmc_tagged,
                                               dapiPortPtr->modeparm.router.macAddr,
                                               ttl);
                  }
                  else
                  {
                    hapiBroadL3McastDeleteL3Port (hapiPortPtr->bcmx_lport,
                                                     ipmc_index,
                                                      *groupIp,
                                                     *srcAddr,
                                                      *vid,
                                                     vlan_id);

                  }
                }
              }
            }
          }
        }
      }
    }
    /* Add LAGs to the downstream interfaces list.
    */
    /* Now go through the LAG interfaces. For XGS3, add all member ports to the bitmap.
       Otherwise add only one member port. */
    hapiBroadGetSystemBoardFamily(&board_family);

    searchUsp.unit = L7_LOGICAL_UNIT;
    searchUsp.slot = L7_LAG_SLOT_NUM;
    for (searchUsp.port = 0; searchUsp.port < L7_MAX_NUM_LAG_INTF; searchUsp.port++)
    {
      dapiPortPtr2 = dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->port[searchUsp.port];
      hapiPortPtr = dapiPortPtr2->hapiPort;

      if (BROAD_IS_VLAN_MEMBER(&searchUsp, vlan_id, dapi_g) )
      {
        lagMemberSet = DAPI_PORT_GET(&searchUsp, dapi_g)->modeparm.lag.memberSet;

        for (lagMemberIndex = 0; lagMemberIndex < L7_MAX_MEMBERS_PER_LAG; lagMemberIndex++)
        {
          if (lagMemberSet[lagMemberIndex].inUse == L7_TRUE)
          {
            hapiLagMemberPortPtr = HAPI_PORT_GET(&lagMemberSet[lagMemberIndex].usp, dapi_g);

            /* setup the egress port */

            if (BROAD_IS_VLAN_TAGGING(&searchUsp, vlan_id, dapi_g))
              ipmc_tagged = TRUE; /* tagged */
            else
              ipmc_tagged = FALSE; /* untagged */

            if (add_remove)
            {
              hapiBroadL3McastAddL3Port (hapiLagMemberPortPtr->bcmx_lport,
                                         ipmc_index,
                                         *groupIp,
                                         *srcAddr,
                                         *vid,
                                         vlan_id,
                                         ipmc_tagged,
                                         dapiPortPtr->modeparm.router.macAddr,
                                         ttl);

            }
            else
            {
              hapiBroadL3McastDeleteL3Port (hapiLagMemberPortPtr->bcmx_lport,
                                            ipmc_index,
                                            *groupIp,
                                            *srcAddr,
                                            *vid,
                                            vlan_id);
            }
            /*For XGS3 we can distribute non-unicast
                     traffic across the lag members*/
            if ((board_family == BCM_FAMILY_DRACO) ||
                (board_family == BCM_FAMILY_TUCANA))
            {
              break;
            }
          }
        }
      }
    }
  }
}


/*********************************************************************
*
* @purpose Init software for IP Mcast package
*
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   No initialization needed for Broadcom. We do not want to
*          call bcm_ipmc_init here since that enables IPMC.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL3McastInit(DAPI_t *dapi_g)
{
  L7_uint32  async_cmd_size;

  memset(BroadGroupList, 0, sizeof(BroadGroupList));
  groupEntriesInUse    = 0;
  groupEntriesInHw     = 0;
  groupAppInsert       = 0;
  groupAppInsertFail   = 0;
  groupRetryInsert     = 0;
  groupRetryInsertFail = 0;
  groupWasDeleted      = L7_FALSE;

  hapiBroadL3McastAsyncWaitSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO,OSAPI_SEM_EMPTY);
  if (hapiBroadL3McastAsyncWaitSema == L7_NULL)
  {
    L7_LOG_ERROR (0);
  }

  async_cmd_size = sizeof (hapi_broad_mcast_async_msg_t);

  hapiBroadMcastAsyncCmdQueue = osapiMsgQueueCreate ("hapiMcastAsyncCmdQ",
                                                HAPI_BROAD_MCAST_ASYNC_CMD_QUEUE_SIZE,
                                                async_cmd_size);
  if (hapiBroadMcastAsyncCmdQueue == L7_NULL)
  {
    L7_LOG_ERROR (0);
  }


  hapiBroadMcastAsyncRpfQueue = osapiMsgQueueCreate ("hapiMcastRpfAsyncQ",
                                                      HAPI_BROAD_MCAST_ASYNC_RPF_QUEUE_SIZE,
                                                     sizeof (hapi_broad_mcast_rpf_async_t));
  if (hapiBroadMcastAsyncRpfQueue == L7_NULL)
  {
    L7_LOG_ERROR (0);
  }


  /* This queue is used to notify the Multicast async task that
  ** there is work to do. The actual work items are enqueued to
  ** different queues.
  */
  hapiBroadRoutingMcastQueue = (void *)osapiMsgQueueCreate("hapiMcAsyncQ",
                                                     1,
                                                     sizeof(L7_uint32));

  if (hapiBroadRoutingMcastQueue == L7_NULL)
  {
    L7_LOG_ERROR(0);
  }

  usl_db_sync_failure_notify_callback_register(USL_IPMC_ROUTE_DB_ID, hapiBroadL3McastAsyncFailureCallback);

  /* spawn task */
  if (osapiTaskCreate("hapiMcAsyncTask", hapiBroadRoutingMcastAsyncTask,
                      1, dapi_g, (1024 * 48),
                      L7_DEFAULT_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    L7_LOG_ERROR(0);
  }

  hapi_mcast_inited = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Init IP Mcast pkg for all ports on this card
*
* @param   L7_ushort16  unitNum - unit number for this card
* @param   L7_ushort16  slotNum - slot number for this card
* @param   DAPI_t *dapi_g - the driver object
*
* @returns No card initialization is needed for Broadcom.
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL3McastCardInit(L7_ushort16 unitNum,L7_ushort16 slotNum,DAPI_t *dapi_g)
{
  /* Init IP Mcast pkg for all ports on this card */

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Hook in IP Mcast functionality and initialize any data
*
* @param   DAPI_PORT_t *dapiPortPtr - generic port instance
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL3McastPortInit(DAPI_PORT_t *dapiPortPtr)
{
  L7_RC_t result = L7_SUCCESS;

  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_MCAST_FWD_CONFIG             ] = (HAPICTLFUNCPTR_t)hapiBroadRoutingIntfMcastFwdConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_MCAST_ADD                    ] = (HAPICTLFUNCPTR_t)hapiBroadRoutingIntfMcastAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_MCAST_DELETE                 ] = (HAPICTLFUNCPTR_t)hapiBroadRoutingIntfMcastDelete;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_MCAST_COUNTERS_GET           ] = (HAPICTLFUNCPTR_t)hapiBroadRoutingIntfMcastForwEntryCtrGet;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_MCAST_USE_GET                ] = (HAPICTLFUNCPTR_t)hapiBroadRoutingIntfMcastForwEntryUseGet;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD               ] = (HAPICTLFUNCPTR_t)hapiBroadRoutingIntfMcastPortAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_MCAST_PORT_DELETE            ] = (HAPICTLFUNCPTR_t)hapiBroadRoutingIntfMcastPortDelete;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_MCAST_VLAN_PORT_MEMBER_UPDATE] = (HAPICTLFUNCPTR_t)hapiBroadRoutingIntfMcastVlanPortMemberUpdate;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET                ] = (HAPICTLFUNCPTR_t)hapiBroadRoutingIntfMcastTtlSet;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG     ] = (HAPICTLFUNCPTR_t)hapiBroadRoutingMcastFwdConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_ROUTE_MCAST_IGMP_CONFIG           ] = (HAPICTLFUNCPTR_t)hapiBroadRoutingMcastIgmpConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_MCAST_SNOOP_NOTIFY           ] = (HAPICTLFUNCPTR_t)hapiBroadRoutingSnoopNotify;
  return result;
}

/*********************************************************************
*
* @purpose Enable multicast routing on the specified interface.
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_FWD_CONFIG
* @param   void       *data   - Multicast routing enable/disable command
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   Per-interface enable/disable of IP multicast is not
*          supported on Broadcom
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastFwdConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 result = L7_SUCCESS;
  /* DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_INTF_MGMT_CMD_t *)data; */

  return result;

}
/*********************************************************************
*
* @purpose Add a multicast routing entry.
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_ADD
* @param   void       *data   - New route information
* @param   DAPI_t     *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd      = (DAPI_ROUTING_INTF_MGMT_CMD_t*)data;
  hapi_broad_mcast_async_msg_t mcast;
  L7_RC_t   rc;

  mcast.async_cmd = HAPI_BROAD_MCAST_ASYNC_ADD;
  mcast.usp = *usp;

  mcast.asyncData.mcastAdd.getOrSet = dapiCmd->cmdData.mcastAdd.getOrSet;

  inetCopy(&mcast.asyncData.mcastAdd.mcastGroupAddr,
                                                &dapiCmd->cmdData.mcastAdd.mcastGroupAddr);
  inetCopy(&mcast.asyncData.mcastAdd.srcIpAddr, &dapiCmd->cmdData.mcastAdd.srcIpAddr);

  mcast.asyncData.mcastAdd.matchSrcAddr = dapiCmd->cmdData.mcastAdd.matchSrcAddr;
  mcast.asyncData.mcastAdd.rpfCheckEnable = dapiCmd->cmdData.mcastAdd.rpfCheckEnable;
  mcast.asyncData.mcastAdd.rpfType = dapiCmd->cmdData.mcastAdd.rpfType;
  mcast.asyncData.mcastAdd.rpfCheckFailAction = dapiCmd->cmdData.mcastAdd.rpfCheckFailAction;
  mcast.asyncData.mcastAdd.outUspCount = dapiCmd->cmdData.mcastAdd.outUspCount;
  mcast.asyncData.mcastAdd.outGoingIntfPresent = dapiCmd->cmdData.mcastAdd.outGoingIntfPresent;

  if (mcast.asyncData.mcastAdd.outGoingIntfPresent == L7_TRUE)
  {
    memcpy (mcast.asyncData.mcastAdd.outUspList,
            dapiCmd->cmdData.mcastAdd.outUspList,
            sizeof(mcast.asyncData.mcastAdd.outUspList));
  }
  mcast.asyncData.mcastAdd.ingressPhysicalPort = dapiCmd->cmdData.mcastAdd.ingressPhysicalPort;

  /* If queue is full then we block.
  */
  rc = osapiMessageSend (hapiBroadMcastAsyncCmdQueue,
                           (void*)&mcast,
                           sizeof (mcast),
                           L7_WAIT_FOREVER,
                           L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR (rc);
  }

  /* Notify multicast async task that there is work to do.
  */
  hapiBroadL3McastAsyncNotify();

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose Notify L3 IPMC of updated out port list in port list.
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_SNOOP_NOTIFY
* @param   void       *data   - New outport list information
* @param   DAPI_t     *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingSnoopNotify(DAPI_USP_t *usp, DAPI_CMD_t cmd,
                                    void *data, DAPI_t *dapi_g)
{
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd      = (DAPI_ROUTING_INTF_MGMT_CMD_t*)data;
  hapi_broad_mcast_async_msg_t mcast;
  L7_RC_t   rc;

  mcast.async_cmd = HAPI_BROAD_MCAST_ASYNC_SNOOP_NOTIFY;

  memcpy(mcast.asyncData.mcastModify.mcastMacAddr,
         dapiCmd->cmdData.mcastModify.mcastMacAddr, L7_MAC_ADDR_LEN);
  /* Get -> Source Interfaces, Set -> Destination Interfaces */
  mcast.asyncData.mcastModify.getOrSet = dapiCmd->cmdData.mcastModify.getOrSet;
  mcast.asyncData.mcastModify.outIntfDirection = dapiCmd->cmdData.mcastModify.outIntfDirection;
  mcast.asyncData.mcastModify.snoopVlanOperState = dapiCmd->cmdData.mcastModify.snoopVlanOperState;
  mcast.asyncData.mcastModify.outUspCount = dapiCmd->cmdData.mcastModify.outUspCount;
  mcast.usp.unit = usp->unit;
  mcast.usp.slot = usp->slot;
  mcast.usp.port = usp->port;

   /* if there are any outgoing interfaces, create array of USPs, one for eact out router interface */
  if (mcast.asyncData.mcastModify.outUspCount)
  {
    memcpy (mcast.asyncData.mcastModify.outUspList,
            dapiCmd->cmdData.mcastModify.outUspList,
            sizeof(mcast.asyncData.mcastModify.outUspList));
  }

  /* If queue is full then we block.
  */
  rc = osapiMessageSend (hapiBroadMcastAsyncCmdQueue,
                           (void*)&mcast,
                           sizeof (mcast),
                           L7_WAIT_FOREVER,
                           L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR (rc);
  }

  /* Notify multicast async task that there is work to do.
  */
  hapiBroadL3McastAsyncNotify();

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Given an address and a buffer, fill in the buffer with a
*          displayable rendition of the address, appropriate to the
*          address family.
*
* @param   L7_uchar8       *buf   (output buffer)
* @param   L7_uint32        len   (length of buffer)
* @param   L7_inet_addr_t  *addr  (address)
*
* @returns none
*
* @notes   Fills in address; returns '<INV_FAM_xx>' where xx is the
*          hexadecimal value of the family if it's not IPv4 or IPv6.
*          Assumes valid buf, len, addr are passed in.
*
* @end
*
*********************************************************************/
static void hapiBroadMcastFillAddressString(L7_uchar8 *buf, const L7_uint32 len,const L7_inet_addr_t *addr)
{
  if (L7_AF_INET == addr->family)
  {
    /* It's IPv4; parse as IPv4 */
    osapiInetNtop(L7_AF_INET, (L7_uchar8*)(&(addr->addr.ipv4.s_addr)), buf, len);
  }
  else if (L7_AF_INET6 == addr->family)
  {
    /* It's IPv6; parse as IPv6 */
    osapiInetNtop(L7_AF_INET6, (L7_uchar8*)(&(addr->addr.ipv6.in6)), buf, len);
  }
  else
  {
    /* It's not covered already; indicate invalid */
    osapiSnprintf(buf, len-1, "<INV_FAM_%02X>", addr->family);
  }
}

/*********************************************************************
*
* @purpose Given a multicast software table entry, add it to the
*          hardware table.  Does not add incoming or outgoing
*          interfaces to the group, nor does it remove them.
*
* @param   BroadGroupEntryType *entry
* @param   DAPI_t *dapi_g  (DAPI context)
*
* @returns Result code from add -- value passes L7_BCMX_OK() if success
*
* @notes   none
*
* @end
*
*********************************************************************/
static int hapiBroadMcastHardwareAddEntry(BroadGroupEntryType *entry,DAPI_t *dapi_g)
{
  BROAD_PORT_t     *hapiPortPtr = L7_NULL;   /* working HAPI port information pointer */
  DAPI_PORT_t      *dapiPortPtr;             /* working DAPI port information pointer */
  DAPI_USP_t       *usp;                     /* working port USP pointer */
  DAPI_USP_t       nullUsp = { 0, 0, 0};     /* NULL USP for adding without knowing physical interface */
  usl_bcm_ipmc_addr_t ipmc_info;                /* IP Multicast information entry workspace */
  L7_uint32        currentTime;              /* current time (for throttling of collision alerts) */
  int              rv = BCM_E_NONE;                       /* Broadcom result code workspace */
  L7_uchar8        grpStr[GEN_ADDR_STR_LEN]; /* buffer for group address string if needed for logging */
  L7_uchar8        srcStr[GEN_ADDR_STR_LEN]; /* buffer for source address string if needed for logging */

  if (!entry)
  {
    /* passed entry was NULL; that's invalid */
    L7_LOG_ERROR(0);
  }

  /* Initialise the hardware entry buffer */
  memset(&ipmc_info, 0, sizeof(ipmc_info));
  ipmc_info.vid = entry->vlanId;

  hapiBroadL3McastSetIpAddress(&(entry->srcAddr),&(entry->groupIp),&ipmc_info);

  ipmc_info.ts = FALSE; /* port_tgid contains port */
  /* ipmc_info.cos = -1; */ /* Ensure that cos priority remap is disabled - 46356 */ /* or not! */
  /* Get the reverse path USP (ingress interface USP) */
  usp = &(entry->rpf_usp);
  /* Get the ingress interface information */
  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);
  if (L7_TRUE == IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
  {
    /* The port is physical; get the proper hardware port info & vlan ID */
    hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);
    ipmc_info.port_tgid = BCMX_LPORT_MODPORT(hapiPortPtr->bcmx_lport);
    ipmc_info.mod_id = BCMX_LPORT_MODID(hapiPortPtr->bcmx_lport);
  }
  else /* (L7_TRUE == IS_PORT_TYPE_PHYSICAL(dapiPortPtr)) */
  {
    /* The port is logical for VLAN routing; get what we can find about it */
    usp = &(entry->rpf_physical_usp);

    /* check for WLAN port and find the physical port associated with it */
    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr))
    {
      ipmc_info.port_tgid = hapiPortPtr->hapiModeparm.lag.tgid;
      ipmc_info.ts = 1;
    }

#ifdef L7_WIRELESS_PACKAGE
    else if (IS_PORT_TYPE_CAPWAP_TUNNEL(dapiPortPtr))
    {
      BROAD_WLAN_TUNNEL_ENTRY_t tunnelEntry;
      hapiBroadWlanInfoGet(dapi_g, hapiPortPtr->bcmx_lport, &tunnelEntry);
      ipmc_info.port_tgid = tunnelEntry.wlan_port.port;
      HAPI_BROAD_LPORT_TO_USP(tunnelEntry.wlan_port.port,usp);
    }
#endif
    else
    {
      ipmc_info.port_tgid = BCMX_LPORT_MODPORT(hapiPortPtr->bcmx_lport);
      ipmc_info.mod_id = BCMX_LPORT_MODID(hapiPortPtr->bcmx_lport);
    }
  } /* (L7_TRUE == IS_PORT_TYPE_PHYSICAL(dapiPortPtr)) */

  /*
  ** On XGS devices always insert entries using the index --
  ** Strata doesn't support application-supplied indexes.
  */

  ipmc_info.ipmc_index = USL_BCM_IPMC_INVALID_INDEX;

  /* Put some debugging information */
  HAPI_L3_ADDR_PRINT_DEBUG(entry->srcAddr, "SRC");
  HAPI_L3_ADDR_PRINT_DEBUG(entry->groupIp, "GRP");
  HAPI_L3_MCAST_DEBUG("Src intf %d  vlan %d", ipmc_info.port_tgid, entry->vlanId);
  /* finally, try to add it to the hardware */
  HAPI_L3_PRINT_IPMC_DEBUG(&ipmc_info);
  rv = usl_bcmx_ipmc_add(&ipmc_info);
  if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
  {
    /* the entry was not added for some reason; log it */
    hapiBroadMcastFillAddressString((L7_uchar8*)(&(grpStr)),sizeof(grpStr),&(entry->groupIp));
    hapiBroadMcastFillAddressString((L7_uchar8*)(&(srcStr)),sizeof(srcStr),&(entry->srcAddr));
    L7_LOGF (L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID,
             "usl_bcmx_ipmc_add %s: group = %s, source = %s, vlan = %d, port = %d, rv = %d\n",
             (entry->flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW_FAIL)?"still failed":"failed",
             grpStr,
             srcStr,
             entry->vlanId, ipmc_info.port_tgid,
             rv);
    /* mark the entry as not in hardare, and tried to be in hardware */
    entry->flags &= ~BROAD_GROUP_ENTRY_FLAGS_IN_HW;
    entry->flags |= BROAD_GROUP_ENTRY_FLAGS_IN_HW_FAIL;
    /* make a callback to the application, but only every so often... */
    currentTime = osapiUpTimeRaw();
    if ((currentTime<lastCollisionCallbackTime) ||
        (currentTime>(lastCollisionCallbackTime+BROAD_MCAST_COLLISION_ALERT_DELAY)))
    {
      /* ...it has been long enough; make the callback */
      dapiCallback(&nullUsp,
                   DAPI_FAMILY_ROUTING_INTF_MCAST,
                   DAPI_CMD_ROUTING_INTF_MCAST_UNSOLICITED_EVENT,
                   DAPI_EVENT_MCAST_ENTRY_COLLISION,
                   L7_NULL);
      lastCollisionCallbackTime = currentTime;
    }
  }
  else /* ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE)) */
  {
    /* Check and report unusual conditions */
    if ((BCM_E_EXISTS == rv) || (entry->flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW_FAIL))
    {
      /*
      ** We're going to need to log the entry because it was already in
      ** hardware, or we ealier logged that we could not add it.  Note
      ** it is possible that this 'or' is inclusive, though it would be
      ** an exceptionally strange condition (somehow the entry made it
      ** into the table but we thought it did not).  Build the address
      ** strings used for these log entries first.
      */
      hapiBroadMcastFillAddressString((L7_uchar8*)(&(grpStr)),sizeof(grpStr),&(entry->groupIp));
      hapiBroadMcastFillAddressString((L7_uchar8*)(&(srcStr)),sizeof(srcStr),&(entry->srcAddr));
      /* If the entry was already in, something is wrong; log it */
      if (BCM_E_EXISTS == rv)
      {
        /* The entry was already in hardware and we didn't think it was */
        /* This could indicate corruption; better flag it */
        L7_LOGF (L7_LOG_SEVERITY_ALERT, L7_DRIVER_COMPONENT_ID,
                 "usl_bcmx_ipmc_add existing: group = %s, source = %s, vlan = %d, rv = %d\n",
                 grpStr, srcStr, entry->vlanId, rv);
      }
      /* If we logged that this entry did not go in, log that it has now */
      if (entry->flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW_FAIL)
      {
        /* we tried this entry earlier and it failed; indicate that it's finally in there */
        entry->flags &= ~BROAD_GROUP_ENTRY_FLAGS_IN_HW_FAIL;
        L7_LOGF (L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
                 "usl_bcmx_ipmc_add success (previously failed): group = %s, source = %s, vlan = %d, rv = %d\n",
                 grpStr, srcStr, entry->vlanId, rv);
      }
    } /* ((BCM_E_EXISTS == rv) || (entry->flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW_FAIL)) */
    /* Since it is now (or was already) in the hardware, consider the add to have succeeded */
    entry->flags |= BROAD_GROUP_ENTRY_FLAGS_IN_HW;
    entry->ipmc_index = ipmc_info.ipmc_index;
    groupEntriesInHw++;
  } /* ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE)) */
  return rv;
}


/*********************************************************************
*
* @purpose Given a multicast software table entry, add the upstream
*          interfaces to the L2 bitmap, and make sure the downstream
*          interfaces are correct (remove any that should not be in
*          the list and add any that should be in the list).
*
* @param   DAPI_t *dapi_g  (DAPI context)
* @param   BroadGroupEntryType *entry
*
* @returns none
*
* @notes   none
*
* @end
*
*********************************************************************/
static void hapiBroadMcastHardwareSetIfs(BroadGroupEntryType *entry,DAPI_t *dapi_g)
{
  DAPI_USP_t   *portUsp;                 /* pointer to the port's USP */
  BROAD_PORT_t *hapiPortPtr;             /* pointer to the ports HAPI info */
  int          index;                    /* index for port list scan */
  L7_uchar8    grpStr[GEN_ADDR_STR_LEN]; /* buffer for group address string if needed for logging */
  L7_uchar8    srcStr[GEN_ADDR_STR_LEN]; /* buffer for source address string if needed for logging */

  if (!entry)
  {
    /* passed entry was NULL; that's invalid */
    L7_LOG_ERROR(0);
  }

  /* Only do something if the entry appears to be in hardware already */
  if (entry->flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW)
  {
    /* The entry is in hardware; ensure the interfaces are connected properly */
    /* Make sure the incoming interface ports are added to the L2 bitmap, if needed */
    if (!(entry->flags & BROAD_GROUP_ENTRY_FLAGS_INGRESS_IN_HW))
    {
      hapiBroadL3McastAddVlanMembersAsL2Ports(&(entry->rpf_usp),&(entry->groupIp),&(entry->srcAddr),&(entry->vlanId),
                                              entry->ipmc_index, dapi_g);
      entry->flags |= BROAD_GROUP_ENTRY_FLAGS_INGRESS_IN_HW;
      entry->flags &= ~BROAD_GROUP_ENTRY_FLAGS_INGRESS_SNOOP_MODIFY;
    }
    /*
    ** now scan the interface list --
    **  1) remove any 'deleted' interfaces (from hardware and then the table)
    **  2) add any 'in use' interfaces that are not in hardware to the hardware
    */
    for (index = 0; index < L7_MAX_NUM_ROUTER_INTF; index++)
    {
      if (entry->downstream_if[index].flags & BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_USE)
      {
        /* Check every entry that is in use */
        if (entry->downstream_if[index].flags & BROAD_GROUP_LOGICAL_PORT_FLAGS_REMOVE)
        {
          /* this port is not meant to be in use (it was removed from use); delete it */
          if (entry->downstream_if[index].flags & BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_HW)
          {
            /* this port is in hardware; remove it from the hardware first */
            portUsp = &(entry->downstream_if[index].usp);
            hapiPortPtr = dapi_g->unit[portUsp->unit]->slot[portUsp->slot]->port[portUsp->port]->hapiPort;
            hapiBroadL3McastAddRemoveL3Ports(portUsp,&(entry->groupIp),&(entry->srcAddr),&(entry->vlanId),hapiPortPtr->multicast_ttl_limit,L7_FALSE,dapi_g);
            entry->downstream_if[index].flags &= ~BROAD_GROUP_LOGICAL_PORT_FLAGS_EGRESS_SNOOP_MODIFY;
          }
          memset(&(entry->downstream_if[index]),0,sizeof(BroadGroupLogicalPortEntryType));
        } /* (entry->downstream_if[index].flags & BROAD_GROUP_LOGICAL_PORT_FLAGS_REMOVE) */
        else if (!(entry->downstream_if[index].flags & BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_HW))
        {
          /* this port is meant to be in use, but it is not in hardware; add it */
          portUsp = &(entry->downstream_if[index].usp);
          hapiPortPtr = dapi_g->unit[portUsp->unit]->slot[portUsp->slot]->port[portUsp->port]->hapiPort;
          hapiBroadL3McastAddRemoveL3Ports(portUsp,&(entry->groupIp),&(entry->srcAddr),&(entry->vlanId),hapiPortPtr->multicast_ttl_limit,L7_TRUE,dapi_g);
          entry->downstream_if[index].flags |= BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_HW;
          entry->downstream_if[index].flags &= ~BROAD_GROUP_LOGICAL_PORT_FLAGS_EGRESS_SNOOP_MODIFY;
        } /* (!(entry->downstream_if[index].flags & BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_HW)) */
      } /* (entry->downstream_if[index].flags & BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_USE) */
    } /* (index = 0; index < L7_MAX_NUM_ROUTER_INTF; i++) */
  }
  else /* (entry->flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW) */
  {
    /* the entry is not in hardware; we should not have been here; log as a problem */
    hapiBroadMcastFillAddressString((L7_uchar8*)(&(grpStr)),sizeof(grpStr),&(entry->groupIp));
    hapiBroadMcastFillAddressString((L7_uchar8*)(&(srcStr)),sizeof(srcStr),&(entry->srcAddr));
    L7_LOGF (L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID,
             "hapiBroadMcastHardwareSetIfs: not in hardware group = %s, source = %s, vlan = %5d\n",
             grpStr,srcStr,entry->vlanId);
  } /* (entry->flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW) */
}
void hapiBroadRoutingMulticastMacFromIpAddr(L7_inet_addr_t *ipAddr, L7_uchar8 *macAddr)
{
  L7_uint32  ipv4Addr;
  L7_uchar8  ip6Addr[L7_IP6_ADDR_LEN];

  if (ipAddr->family == L7_AF_INET)
  {
    if (inetAddressGet(L7_AF_INET, ipAddr, &ipv4Addr) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "hapiBroadRoutingMulticastMacFromIpAddr: Invalid IPv4Address");
      memset(macAddr, 0x00, L7_MAC_ADDR_LEN);
      return;
    }

    macAddr[0] = 0x01;
    macAddr[1] = 0x00;
    macAddr[2] = 0x5E;
    macAddr[3] = (ipv4Addr & 0x007f0000) >> 16;
    macAddr[4] = (ipv4Addr & 0x0000ff00) >> 8;
    macAddr[5] = (ipv4Addr & 0x000000ff);
  }
  else if (ipAddr->family == L7_AF_INET6)
  {
     /* MLD */
    macAddr[0] = 0x33;
    macAddr[1] = 0x33;

    if (inetAddressGet(L7_AF_INET6, ipAddr, ip6Addr) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "hapiBroadRoutingMulticastMacFromIpAddr: Invalid IPv6Address");
      memset(macAddr, 0x00, L7_MAC_ADDR_LEN);
      return;
    }
    memcpy(&macAddr[2], &ip6Addr[12], 4);
  }
}
/*********************************************************************
*
* @purpose Fine tune IPMC out port list according to the port list
*          maintained by snooping component
*
*
* @param   L7_uchar8 *mcastMacAddr  (Multicast Group MAC Address)
* @param   DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd  (Multicast Group MAC Address)
* @param   DAPI_USP_t *outRtrPortUsp  (Source/Destination Routing interface USP)
* @param   DAPI_t *dapi_g  (DAPI context)
*
* @returns none
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastSnoopAsyncNotifyProcess(L7_uchar8 *mcastMacAddr,
                                                         DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd,
                                                         DAPI_USP_t *outRtrPortUsp,
                                                         DAPI_t *dapi_g)
{
  L7_uint32 tableIndex, i, portIndex, vlan_id,j;
  L7_uchar8 tempMcastMacAddr[L7_MAC_ADDR_LEN], ttl;
  DAPI_USP_t                   searchUsp;
  L7_RC_t searchResult;
  BROAD_PORT_t                 *hapiPortPtr;
  DAPI_PORT_t                  *dapiPortPtr;
  int                           ipmc_tagged;
  DAPI_LAG_ENTRY_t             *lagMemberSet;
  L7_uint32                     l3_interface_id=0;


  /* search the tabe for the entries containing this McastMacAddress */
  for (tableIndex = 0; tableIndex < L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL; tableIndex++ )
  {
    /* lock the table */
    hapiBroadL3McastCritEnter ();
    if (BroadGroupList[tableIndex].flags & BROAD_GROUP_ENTRY_FLAGS_IN_USE)
    {
      hapiBroadRoutingMulticastMacFromIpAddr(&BroadGroupList[tableIndex].groupIp, tempMcastMacAddr);
      if (memcmp(mcastMacAddr, tempMcastMacAddr, L7_MAC_ADDR_LEN) == 0)
      {
/************** RPF interface modify code ***********************/
        if (dapiCmd->cmdData.mcastModify.outIntfDirection == DAPI_SNOOP_L3_NOTIFY_INGRESS)
        {
          if (memcmp(&(BroadGroupList[tableIndex].rpf_usp),
                     outRtrPortUsp, sizeof(outRtrPortUsp)) == 0)
          {
            if (dapiCmd->cmdData.mcastModify.snoopVlanOperState == L7_TRUE)
            {
             HAPI_L3_MCAST_DEBUG("\nTable Index %u L2 Bitmap of entries with rpf_us %u/%u/%u modified. Make specified list as part of L2 Bitmap.",
                               tableIndex,
                               BroadGroupList[tableIndex].rpf_usp.unit,
                               BroadGroupList[tableIndex].rpf_usp.slot,
                               BroadGroupList[tableIndex].rpf_usp.port);
            hapiBroadL3McastAddL2Ports (&(BroadGroupList[tableIndex].rpf_usp),
                                        &(BroadGroupList[tableIndex].groupIp),
                                        &(BroadGroupList[tableIndex].srcAddr),
                                        &(BroadGroupList[tableIndex].vlanId),
                                        BroadGroupList[tableIndex].ipmc_index,
                                        dapiCmd->cmdData.mcastModify.outUspList,
                                        dapiCmd->cmdData.mcastModify.outUspCount,
                                        dapi_g);
              BroadGroupList[tableIndex].flags |= BROAD_GROUP_ENTRY_FLAGS_INGRESS_SNOOP_MODIFY;
          }
            else
            {
             HAPI_L3_MCAST_DEBUG("\nTable Index %u L2 Bitmap of entries with rpf_us %u/%u/%u modified. Make VLAN member list as part of L2 Bitmap.",
                               tableIndex, 
                               BroadGroupList[tableIndex].rpf_usp.unit,
                               BroadGroupList[tableIndex].rpf_usp.slot,
                               BroadGroupList[tableIndex].rpf_usp.port);
              hapiBroadL3McastAddVlanMembersAsL2Ports(&(BroadGroupList[tableIndex].rpf_usp),
                                                      &(BroadGroupList[tableIndex].groupIp),
                                                      &(BroadGroupList[tableIndex].srcAddr),
                                                      &(BroadGroupList[tableIndex].vlanId),
                                                        BroadGroupList[tableIndex].ipmc_index,
                                                        dapi_g);
              BroadGroupList[tableIndex].flags &= ~BROAD_GROUP_ENTRY_FLAGS_INGRESS_SNOOP_MODIFY;
            }
          }
          hapiBroadL3McastCritExit();
          continue;
        }
/************** Down stream interfaces list modify code *********/
  searchResult = hapiBroadL3McastFindPort(outRtrPortUsp,tableIndex,&portIndex);
  /* check if the notified VLAN routing interface is part of the donwstream interfaces */
  if (L7_SUCCESS == searchResult)
  {
    dapiPortPtr = dapi_g->unit[outRtrPortUsp->unit]->slot[outRtrPortUsp->slot]->port[outRtrPortUsp->port];
    hapiPortPtr = dapi_g->unit[outRtrPortUsp->unit]->slot[outRtrPortUsp->slot]->port[outRtrPortUsp->port]->hapiPort;
    ttl = hapiPortPtr->multicast_ttl_limit;
    vlan_id = dapiPortPtr->modeparm.router.vlanID;
    l3_interface_id = hapiPortPtr->l3_interface_id;

    for (searchUsp.unit=0; searchUsp.unit < dapi_g->system->totalNumOfUnits; searchUsp.unit++)
    {
      for (searchUsp.slot=0; searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots; searchUsp.slot++)
      {
        if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardPresent == L7_TRUE)
        {
           for (searchUsp.port=0;
                searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot;
                searchUsp.port++)
           {
             /* Is a valid port */
             if (isValidUsp (&searchUsp, dapi_g) != L7_TRUE)
             {
               continue;
             }

             dapiPortPtr = dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->port[searchUsp.port];
             hapiPortPtr = dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->port[searchUsp.port]->hapiPort;

              /* if member of a lag or port based routing interface skip it */
              if ((hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE) ||
                  (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE))
              {
                continue;
              }

              if (BROAD_IS_VLAN_MEMBER(&searchUsp, vlan_id, dapi_g) )
              {
               /* setup the egress port */
               if (BROAD_IS_VLAN_TAGGING(&searchUsp, vlan_id, dapi_g))
               {
                 ipmc_tagged = TRUE; /* tagged */
               }
               else
               {
                 ipmc_tagged = FALSE; /* untagged */
               }

               if (dapiCmd->cmdData.mcastModify.outUspCount)
               {
                 BroadGroupList[tableIndex].downstream_if[portIndex].flags |= BROAD_GROUP_LOGICAL_PORT_FLAGS_EGRESS_SNOOP_MODIFY;
                 for (i = 0; i < dapiCmd->cmdData.mcastModify.outUspCount; i++)
                 {
                   if (memcmp(&dapiCmd->cmdData.mcastModify.outUspList[i], &searchUsp, sizeof(searchUsp)) == 0)
                   {
                     /* For lag interfaces, add only the first lag member port */
                     if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
                     {
                       lagMemberSet = DAPI_PORT_GET(&searchUsp, dapi_g)->modeparm.lag.memberSet;
                       for (j = 0; j < L7_MAX_MEMBERS_PER_LAG; j++)
                       {
                         if (dapiPortPtr->modeparm.lag.memberSet[j].inUse == L7_TRUE)
                         {
                           hapiPortPtr = HAPI_PORT_GET(&lagMemberSet[j].usp, dapi_g);
                           hapiBroadL3McastAddL3Port (hapiPortPtr->bcmx_lport,
                                                      BroadGroupList[tableIndex].ipmc_index,
                                                      BroadGroupList[tableIndex].groupIp,
                                                      BroadGroupList[tableIndex].srcAddr,
                                                      BroadGroupList[tableIndex].vlanId,
                                                      vlan_id,
                                                      ipmc_tagged,
                                                      dapiPortPtr->modeparm.router.macAddr,
                                                      ttl);
                         }
                       }
                     }
                     else
                     {
                       if (BCM_GPORT_IS_WLAN_PORT(hapiPortPtr->bcmx_lport))
                       {
                     #ifdef L7_WIRELESS_PACKAGE
                         BROAD_WLAN_TUNNEL_ENTRY_t tunnelEntry;
                         if (hapiBroadWlanInfoGet(dapi_g, hapiPortPtr->bcmx_lport, &tunnelEntry) == L7_SUCCESS)
                         {
                           usl_bcmx_ipmc_wlan_l3_port_add(BroadGroupList[tableIndex].ipmc_index, l3_interface_id, vlan_id,
                                                          hapiPortPtr->bcmx_lport,
                                                          tunnelEntry.wlan_port.port);
                         }
                     #endif
                       }
                       else
                       {
                        hapiBroadL3McastAddL3Port (hapiPortPtr->bcmx_lport,
                                                   BroadGroupList[tableIndex].ipmc_index,
                                                   BroadGroupList[tableIndex].groupIp,
                                                   BroadGroupList[tableIndex].srcAddr,
                                                   BroadGroupList[tableIndex].vlanId,
                                                   vlan_id,
                                                   ipmc_tagged,
                                                   dapiPortPtr->modeparm.router.macAddr,
                                                   ttl);
                       }
                     }
                     break;
                   }/* check if searchUsp is part of L2 entry members */
                 }/* end of L2 entry members iterations */

                 if (i == dapiCmd->cmdData.mcastModify.outUspCount)
                 {
                   /* For lag interfaces, delete only the first lag member port */
                   if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
                   {
                     lagMemberSet = DAPI_PORT_GET(&searchUsp, dapi_g)->modeparm.lag.memberSet;
                     for (j = 0; j < L7_MAX_MEMBERS_PER_LAG; j++)
                     {
                       if (dapiPortPtr->modeparm.lag.memberSet[j].inUse == L7_TRUE)
                       {
                         hapiPortPtr = HAPI_PORT_GET(&lagMemberSet[j].usp, dapi_g);
                         hapiBroadL3McastDeleteL3Port (hapiPortPtr->bcmx_lport,
                                                       BroadGroupList[tableIndex].ipmc_index,
                                                       BroadGroupList[tableIndex].groupIp,
                                                       BroadGroupList[tableIndex].srcAddr,
                                                       BroadGroupList[tableIndex].vlanId,
                                                       vlan_id);
                       }/* atleast one member port present check */
                     }
                   }/* Is lag port check */
                   else
                   {
                     if (BCM_GPORT_IS_WLAN_PORT(hapiPortPtr->bcmx_lport))
                     {
                   #ifdef L7_WIRELESS_PACKAGE
                       BROAD_WLAN_TUNNEL_ENTRY_t tunnelEntry;
                       if (hapiBroadWlanInfoGet(dapi_g, hapiPortPtr->bcmx_lport, &tunnelEntry) == L7_SUCCESS)
                       {
                         usl_bcmx_ipmc_wlan_l3_port_delete(BroadGroupList[tableIndex].ipmc_index, l3_interface_id, vlan_id,
                                                           hapiPortPtr->bcmx_lport,
                                                           tunnelEntry.wlan_port.port);
                       }
                   #endif
                     }
                     else
                     {
                       hapiBroadL3McastDeleteL3Port (hapiPortPtr->bcmx_lport,
                                                     BroadGroupList[tableIndex].ipmc_index,
                                                     BroadGroupList[tableIndex].groupIp,
                                                     BroadGroupList[tableIndex].srcAddr,
                                                     BroadGroupList[tableIndex].vlanId,
                                                     vlan_id);
                     }
                   }
                 }/* end of port not found check */
               }/* if no members in this L2 entry, add all member ports to L3 entry */
               else
               {
                 BroadGroupList[tableIndex].downstream_if[portIndex].flags &= ~BROAD_GROUP_LOGICAL_PORT_FLAGS_EGRESS_SNOOP_MODIFY;
                 /* For lag interfaces, add only the first lag member port */
                 if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
                 {
                   for (j = 0; j < L7_MAX_MEMBERS_PER_LAG; j++)
                   {
                     lagMemberSet = DAPI_PORT_GET(&searchUsp, dapi_g)->modeparm.lag.memberSet;
                     if (dapiPortPtr->modeparm.lag.memberSet[j].inUse == L7_TRUE)
                     {
                       hapiPortPtr = HAPI_PORT_GET(&lagMemberSet[j].usp, dapi_g);
                       hapiBroadL3McastAddL3Port (hapiPortPtr->bcmx_lport,
                                                  BroadGroupList[tableIndex].ipmc_index,
                                                  BroadGroupList[tableIndex].groupIp,
                                                  BroadGroupList[tableIndex].srcAddr,
                                                  BroadGroupList[tableIndex].vlanId,
                                                  vlan_id,
                                                  ipmc_tagged,
                                                  dapiPortPtr->modeparm.router.macAddr,
                                                  ttl);
                     }
                   }/* Is lag member present check */
                 }/* Is lag interface check */
                 else
                 {
                   if (BCM_GPORT_IS_WLAN_PORT(hapiPortPtr->bcmx_lport))
                   {
                 #ifdef L7_WIRELESS_PACKAGE
                     BROAD_WLAN_TUNNEL_ENTRY_t tunnelEntry;
                     if (hapiBroadWlanInfoGet(dapi_g, hapiPortPtr->bcmx_lport, &tunnelEntry) == L7_SUCCESS)
                     {
                       usl_bcmx_ipmc_wlan_l3_port_add(BroadGroupList[tableIndex].ipmc_index, l3_interface_id, vlan_id,
                                                      hapiPortPtr->bcmx_lport,
                                                      tunnelEntry.wlan_port.port);
                     }
                 #endif
                   }
                   else
                   {
                     hapiBroadL3McastAddL3Port (hapiPortPtr->bcmx_lport,
                                                BroadGroupList[tableIndex].ipmc_index,
                                                BroadGroupList[tableIndex].groupIp,
                                                BroadGroupList[tableIndex].srcAddr,
                                                BroadGroupList[tableIndex].vlanId,
                                                vlan_id,
                                                ipmc_tagged,
                                                dapiPortPtr->modeparm.router.macAddr,
                                                ttl);
                   }
                 }
               }/* End of empty L2 entry check */
              }/* Is VLAN member check */
           }/* search usp port iterations */
        }/* Slot in use check */
      }/* search usp slot iteration */
    }/* search usp unit iteration */
  }/* Destination Interface found in  L3 Entrycheck */
      }/* L3 Entry Found check */
    }/* Entry in hardware check */
  hapiBroadL3McastCritExit();
  }/* L3 table iterations */
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Add a multicast routing entry.
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_ADD
* @param   void       *data   - New route information
* @param   DAPI_t     *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastAsyncAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 result       = L7_SUCCESS;
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd      = (DAPI_ROUTING_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t                 *hapiPortPtr;
  DAPI_PORT_t                  *dapiPortPtr;
  usl_bcm_ipmc_addr_t            ipmc_info;
  L7_int32                      rv;
  bcm_vlan_t                    vlan_id = 0;
  DAPI_USP_t                    physUsp;
  L7_inet_addr_t                groupIp;
  L7_inet_addr_t                srcAddr;
  L7_RC_t                       searchResult;
  L7_uint32                     tableIndex; /* Index into the BroadGroupList */
  L7_uint32                     i, j;
  L7_BOOL                   set_group_address = L7_TRUE;
  DAPI_USP_t                   *portUsp;
  L7_uint32                     portIndex;  /* Index into the downstream_if table */
  L7_uint32                     outUspCount;
  L7_BOOL                       override_existing_entry = L7_FALSE;
  bcm_vlan_t                    old_vlan_id = 0;
  bcmx_lport_t                  rpf_lport;

  /* determine number of outgoing interfaces */
  if (dapiCmd->cmdData.mcastAdd.outGoingIntfPresent == L7_TRUE)
  {
    outUspCount = dapiCmd->cmdData.mcastAdd.outUspCount;
  }
  else
  {
    outUspCount = 0;
  }
  /* debugging information */
  HAPI_L3_ADDR_PRINT_DEBUG(dapiCmd->cmdData.mcastAdd.srcIpAddr, "SRC");
  HAPI_L3_ADDR_PRINT_DEBUG(dapiCmd->cmdData.mcastAdd.mcastGroupAddr, "GRP");

  /* lock the table */
  hapiBroadL3McastCritEnter ();
  /* get information about the ingress interface */
  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    /* ingress interface is physical */
    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    vlan_id = hapiPortPtr->port_based_routing_vlanid;
    physUsp = *usp;
    HAPI_L3_MCAST_DEBUG("Src intf %d src usp %d.%d.%d vlan %d outUspCount %d",
       hapiPortPtr->bcmx_lport, usp->unit, usp->slot, usp->port, vlan_id, outUspCount);
  }
  else
  {
    /* ingress interface is logical */
    vlan_id = dapiPortPtr->modeparm.router.vlanID;
    physUsp = dapiCmd->cmdData.mcastAdd.ingressPhysicalPort;
    hapiPortPtr = HAPI_PORT_GET(&physUsp, dapi_g);
    HAPI_L3_MCAST_DEBUG("Src intf vlan intf src usp %d.%d.%d vlan %d outUspCount %d",
        physUsp.unit, physUsp.slot, physUsp.port, vlan_id, outUspCount);
  }
  /* copy IP addresses into local workspace */
  inetCopy(&groupIp, &dapiCmd->cmdData.mcastAdd.mcastGroupAddr);
  inetCopy(&srcAddr, &dapiCmd->cmdData.mcastAdd.srcIpAddr);
  /* see if this group already exists in the table (get index for it if so) */
  searchResult = hapiBroadL3McastFindGroup(&groupIp, &srcAddr, &tableIndex);
  if (L7_SUCCESS == searchResult)
  {
    /* This group exists.  See if we're changing anything that the hardware needs */
    if ((memcmp (usp, &BroadGroupList[tableIndex].rpf_usp, sizeof (DAPI_USP_t)) == 0 ) &&
        (BroadGroupList[tableIndex].rpf_check_enabled == dapiCmd->cmdData.mcastAdd.rpfCheckEnable))
    {
      /* No change to hardware fields for the base entry. */
      /* Modify informational flags in case they changed. */
      BroadGroupList[tableIndex].action_on_rpf_failure = dapiCmd->cmdData.mcastAdd.rpfCheckFailAction;
      BroadGroupList[tableIndex].rpf_check_type = dapiCmd->cmdData.mcastAdd.rpfType; /* Not used */
      /* If the entry was in hardware, indicate we don't change it */
      if (BroadGroupList[tableIndex].flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW)
      {
        HAPI_L3_MCAST_DEBUG("Entry found and hw image does not need to change");
        /* This entry is in hardware and the hardware image doesn't need to change */
        set_group_address = L7_FALSE;
      }
    }
    else /* changing anything that the hardware needs */
    {
      /* Something in hardware data was changed; see if it's in the hardware already */
      if (BroadGroupList[tableIndex].flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW)
      {
        HAPI_L3_MCAST_DEBUG("Entry found and sth changed and entry in hw, Overwide");
        /* Entry is in hardware; need to update it instead of add it */
        override_existing_entry = L7_TRUE;
        old_vlan_id = BroadGroupList[tableIndex].vlanId;
      }
    } /* changing anything that the hardware needs */
  } /* (L7_SUCCESS == searchResult) */
  /* Make sure we're using a valid entry in the table */
  if (tableIndex >= L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL)
  {
    /* The table index is too large (ran out of free entries during add) */
    /*
    ** Note : It's possible that we might want to be able to handle this
    ** condition more gracefully than a halt.  The only alternatives by the
    ** time we're here seem to be either crash or discard the route.  We
    ** could be nice and send a message indicating the condition to the app...
    */
#if BROAD_MCAST_CRASH_ON_TABLE_FULL
    /* Log the problem, in the hope that it will be committed to nonvolatile storage */
    L7_LOGF (L7_LOG_SEVERITY_EMERGENCY, L7_DRIVER_COMPONENT_ID,
             "L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL (%d) reached, trying to add more",
             L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL);
    /* Print a message... */
      HAPI_L3_MCAST_DEBUG("L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL reached, tryting to add more");
    /* ...and then die logging the module, line, and ... and ... address family??? */
      L7_LOG_ERROR (dapiCmd->cmdData.mcastAdd.mcastGroupAddr.family);
#else /* BROAD_MCAST_CRASH_ON_TABLE_FULL */
    /* Log the problem */
    L7_LOGF (L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID,
             "L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL (%d) reached, trying to add more",
             L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL);
    /* Tell the application (if it's listening) */
    dapiCallback(usp,
                 DAPI_FAMILY_ROUTING_INTF_MCAST,
                 DAPI_CMD_ROUTING_INTF_MCAST_UNSOLICITED_EVENT,
                 DAPI_EVENT_MCAST_TABLE_FULL,
                 L7_NULL);
    /* Release the table lock */
    hapiBroadL3McastCritExit();
    /* Propagate an error code that'll most likely be ignored (this was called async) */
    return L7_FAILURE;
#endif /* BROAD_MCAST_CRASH_ON_TABLE_FULL */
  } /* (tableIndex >= L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL) */
  /* Build or update the entry if we need to */
  if (set_group_address == L7_TRUE)
  {
    /* Entry is not in hardware or hardware data for entry need to be updated. */
    if (L7_SUCCESS == searchResult)
    {
      /* Search was successful, so we're updating an existing entry */
      /* Mark the ingress interface as not in hardware (force it to be updated) */
      BroadGroupList[tableIndex].flags &= ~BROAD_GROUP_ENTRY_FLAGS_INGRESS_IN_HW;
    }
    else /* (L7_SUCCESS == searchResult) */
    {
      /* Search was not successful, so this is a new entry */
      /* Mark it used (and *only* used) */
      BroadGroupList[tableIndex].flags = BROAD_GROUP_ENTRY_FLAGS_IN_USE;
      /* Account for one more entry in use */
      groupEntriesInUse++;
    } /* (L7_SUCCESS == searchResult) */
    /* Copy the source and group addresses */
    inetCopy(&BroadGroupList[tableIndex].groupIp,&groupIp);
    inetCopy(&BroadGroupList[tableIndex].srcAddr,&srcAddr);
    /* Copy the VLAN ID */
    memcpy (&BroadGroupList[tableIndex].vlanId, &vlan_id, sizeof (L7_ushort16));
    /* Set reverse path flags */
    BroadGroupList[tableIndex].rpf_check_enabled = dapiCmd->cmdData.mcastAdd.rpfCheckEnable; /* Always enabled(?) */
    BroadGroupList[tableIndex].action_on_rpf_failure = dapiCmd->cmdData.mcastAdd.rpfCheckFailAction;
    BroadGroupList[tableIndex].rpf_check_type = dapiCmd->cmdData.mcastAdd.rpfType; /* Not used */
    /* Set reverse path interface (ingress interface) */
    memcpy (&BroadGroupList[tableIndex].rpf_usp, usp, sizeof (DAPI_USP_t));
    memcpy (&BroadGroupList[tableIndex].rpf_physical_usp, &physUsp, sizeof (DAPI_USP_t));
    rpf_lport = hapiPortPtr->bcmx_lport;
    /* Check whether replacing an existing entry */
    if (override_existing_entry == L7_TRUE)
    {
      /* We're replacing an existing entry; delete it first */
      memset(&ipmc_info, 0, sizeof(ipmc_info));
      ipmc_info.vid = old_vlan_id;
      hapiBroadL3McastSetIpAddress(&srcAddr, &groupIp, &ipmc_info);
      ipmc_info.port_tgid = BCMX_LPORT_MODPORT(rpf_lport);
      ipmc_info.mod_id = BCMX_LPORT_MODID(rpf_lport);
      ipmc_info.ipmc_index = BroadGroupList[tableIndex].ipmc_index;

      /* Delete the exiting entry with (S G V) */
      rv = usl_bcmx_ipmc_remove(&ipmc_info, 0);
      if ((L7_BCMX_OK(rv) != L7_TRUE))
      {
        /* Delete failed; complain */
        L7_LOGF (L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID,
                 "usl_bcmx_ipmc_remove failed: group = %08X, source = %08X, vlan = %5d, rv = %d\n",
                       groupIp, srcAddr, old_vlan_id, rv);
      }
      else /* ((L7_BCMX_OK(rv) != L7_TRUE)) */
      {
        /* Delete successful; account for it no longer being in hardware */
        BroadGroupList[tableIndex].flags &= ~BROAD_GROUP_ENTRY_FLAGS_IN_HW;
        groupEntriesInHw--;
        groupWasDeleted = L7_TRUE;
      } /* ((L7_BCMX_OK(rv) != L7_TRUE)) */
    } /* (override_existing_entry == L7_TRUE) */
    /* Set index */
    BroadGroupList[tableIndex].ipmc_index = tableIndex;
    /* Add the entry to the hardware */
    groupAppInsert++;
    rv = hapiBroadMcastHardwareAddEntry(&(BroadGroupList[tableIndex]),dapi_g);
    if (!(L7_BCMX_OK(rv) || (rv == BCM_E_EXISTS)))
    {
      /* well, that didn't work! */
      groupAppInsertFail++;
    }
  }
  else
  {
    /* Not setting group address, so indicate we did that successfully. */
    rv = BCM_E_NONE;
  } /* (set_group_address == L7_TRUE) */

  /* If we are modifying an existing group, check for interfaces to be deleted */
  if (L7_SUCCESS == searchResult)
  {
    /* We are modifying this group instead of adding; remove any no-longer-wanted interfaces */
    for (i = 0; i < L7_MAX_NUM_ROUTER_INTF; i++)
    {
      /* Don't bother with this interface unless it's in use */
      if (BroadGroupList[tableIndex].downstream_if[i].flags & BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_USE)
      {
        /* this interface is tagged as in use; scan for it in the provided list of interfaces */
        portUsp = &(BroadGroupList[tableIndex].downstream_if[i].usp);
        for (j = 0; j < outUspCount; j++)
        {
          /* Is this new list entry the same as the one we're checking? */
          if (0 == memcmp(&(dapiCmd->cmdData.mcastAdd.outUspList[j]),portUsp,sizeof(DAPI_USP_t)))
          {
            /* yes; stop comparing list entries to this interface */
            break;
          }
        }
        /* Mark this interface for deletion if it's not wanted anymore */
        if (j >= outUspCount)
        {
          /* This interface is not on the list that we were provided; flag for removal */
          BroadGroupList[tableIndex].downstream_if[i].flags |= BROAD_GROUP_LOGICAL_PORT_FLAGS_REMOVE;
          /* account for removal of this interface */
          BroadGroupList[tableIndex].num_ifs--;
        }
      } /* (BroadGroupList[table_index].downstream_if[i].flags & BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_USE) */
    } /* (i = 0; i < L7_MAX_NUM_ROUTER_INTF; i++ ) */
  }

  /* Scan the downstream interfaces provided, and add any missing ones to the group */
  for (i = 0; i < outUspCount; i++)
  {
    /* Search for this interface in the group */
    portUsp = &(dapiCmd->cmdData.mcastAdd.outUspList[i]);
    searchResult = hapiBroadL3McastFindPort(portUsp,tableIndex,&portIndex);
    if (L7_SUCCESS == searchResult)
    {
      /* This interface is already in the group; make sure it's not marked for deletion */
      BroadGroupList[tableIndex].downstream_if[portIndex].flags &= ~(BROAD_GROUP_LOGICAL_PORT_FLAGS_REMOVE);
      BroadGroupList[tableIndex].downstream_if[portIndex].flags &= ~(BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_HW);
      BroadGroupList[tableIndex].downstream_if[portIndex].flags &= ~BROAD_GROUP_LOGICAL_PORT_FLAGS_EGRESS_SNOOP_MODIFY;
    }
    else /* (L7_SUCCESS == searchResult) */
    {
      /* This interface is not in the group; try to add it */
      if (portIndex >= L7_MAX_NUM_ROUTER_INTF)
      {
        /* This interface was not in the group, but for some reason we have too many */
        /* Since this practically guarantees corruption (impossible condition), die. */
        L7_LOG_ERROR(portIndex);
      }
      /* Okay, it wasn't in the list and we're not in any impossible conditions; fill it in */
      memcpy(&(BroadGroupList[tableIndex].downstream_if[portIndex].usp),portUsp,sizeof(DAPI_USP_t));
      hapiPortPtr = dapi_g->unit[portUsp->unit]->slot[portUsp->slot]->port[portUsp->port]->hapiPort;
      BroadGroupList[tableIndex].downstream_if[portIndex].ttl = hapiPortPtr->multicast_ttl_limit;
      /* This entry is now in use, but not in hardware and not scheduled for deletion */
      BroadGroupList[tableIndex].downstream_if[portIndex].flags = BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_USE;
      BroadGroupList[tableIndex].downstream_if[portIndex].flags &= ~BROAD_GROUP_LOGICAL_PORT_FLAGS_EGRESS_SNOOP_MODIFY;
      /* account for using another interface */
      BroadGroupList[tableIndex].num_ifs++;
    } /* (L7_SUCCESS == searchResult) */
  } /* (i = 0; i < outUspCount; i++) */

  /* Make sure things are still okay; add ingress & egress interfaces if so */
  if (L7_BCMX_OK(rv))
  {
    /* The add, if needed, went well.  Commit ingress & egress interface updates */
    hapiBroadMcastHardwareSetIfs(&(BroadGroupList[tableIndex]),dapi_g);
  }

  hapiBroadL3McastCritExit();
  return result;
}


/*********************************************************************
*
* @purpose Remove a multicast routing entry.
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_DELETE
* @param   void       *data   - route information
* @param   DAPI_t     *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   Downstream ports configured for this interface are also deleted.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd      = (DAPI_ROUTING_INTF_MGMT_CMD_t*)data;
  hapi_broad_mcast_async_msg_t mcast;
  L7_RC_t   rc;

  mcast.async_cmd = HAPI_BROAD_MCAST_ASYNC_DELETE;
  mcast.usp = *usp;

  mcast.asyncData.mcastDelete.getOrSet = dapiCmd->cmdData.mcastDelete.getOrSet;
  inetCopy(&mcast.asyncData.mcastDelete.mcastGroupAddr,
                               &dapiCmd->cmdData.mcastDelete.mcastGroupAddr);
  inetCopy(&mcast.asyncData.mcastDelete.srcIpAddr,
                                         &dapiCmd->cmdData.mcastDelete.srcIpAddr);
  mcast.asyncData.mcastDelete.matchSrcAddr = dapiCmd->cmdData.mcastDelete.matchSrcAddr;
  mcast.asyncData.mcastDelete.rpfCheckEnable = dapiCmd->cmdData.mcastDelete.rpfCheckEnable;
  mcast.asyncData.mcastDelete.rpfType = dapiCmd->cmdData.mcastDelete.rpfType;
  mcast.asyncData.mcastDelete.rpfCheckFailAction = dapiCmd->cmdData.mcastDelete.rpfCheckFailAction;


  /* If queue is full then we block.
  */
  rc = osapiMessageSend (hapiBroadMcastAsyncCmdQueue,
                           (void*)&mcast,
                           sizeof (mcast),
                           L7_WAIT_FOREVER,
                           L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR (rc);
  }

  /* Notify multicast async task that there is work to do.
  */
  hapiBroadL3McastAsyncNotify();

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Remove a multicast routing entry.
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_DELETE
* @param   void       *data   - route information
* @param   DAPI_t     *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   Downstream ports configured for this interface are also deleted.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastAsyncDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd      = (DAPI_ROUTING_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t                  *dapiPortPtr;
  BROAD_PORT_t                 *hapiPortPtr;
  L7_inet_addr_t                groupIp;
  L7_inet_addr_t                srcAddr;
  bcm_vlan_t                    vlan_id;
  L7_int32                      rv;
  L7_RC_t                       rc1;
  L7_uint32                     table_index; /* Index into the BroadGroupList */
  DAPI_USP_t                    rpf_usp;
  usl_bcm_ipmc_addr_t            ipmc;

  hapiBroadL3McastCritEnter ();

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    vlan_id = hapiPortPtr->port_based_routing_vlanid;
  }
  else
  {
    vlan_id = dapiPortPtr->modeparm.router.vlanID;
  }

  inetCopy(&groupIp, &dapiCmd->cmdData.mcastDelete.mcastGroupAddr);
  inetCopy(&srcAddr, &dapiCmd->cmdData.mcastDelete.srcIpAddr);

  rc1 = hapiBroadL3McastFindGroup(&groupIp, &srcAddr, &table_index);
  if (rc1 == L7_FAILURE)
  {
    /* Specified group does not exist. Nothing to do.
    */
    hapiBroadL3McastCritExit ();
    return L7_FAILURE;
  }

  memcpy (&rpf_usp, &BroadGroupList[table_index].rpf_usp, sizeof (DAPI_USP_t));

  /* If we are here it means that the find was a success */

  /* Delete the group and all ports that are members of the group.
  */
  HAPI_L3_MCAST_DEBUG("Going to delete the entry");
  HAPI_L3_ADDR_PRINT_DEBUG(srcAddr, "SRC");
  HAPI_L3_ADDR_PRINT_DEBUG(groupIp, "GRP");
  HAPI_L3_MCAST_DEBUG("src usp %d.%d.%d vlan %d ",usp->unit, usp->slot, usp->port, vlan_id);

  memset(&ipmc, 0, sizeof(ipmc));
  ipmc.vid = vlan_id;
  hapiBroadL3McastSetIpAddress(&srcAddr, &groupIp, &ipmc);
  ipmc.ipmc_index = BroadGroupList[table_index].ipmc_index;

  /* Remove the entry from the hardware table */
  rv = usl_bcmx_ipmc_remove(&ipmc, 0);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    if (BroadGroupList[table_index].flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW)
    {
      /* the entry *should* have been in the hardware table, but we couldn't delete it */
      /* since we just found it, this could indicate problems, but *maybe* not fatal */
      L7_LOGF (L7_LOG_SEVERITY_ALERT, L7_DRIVER_COMPONENT_ID,
               "usl_bcmx_ipmc_remove failed: group = %x, source = %x, vlan = %d, rv = %d\n",
               groupIp, srcAddr, vlan_id, rv);
    }
  }
  if (BroadGroupList[table_index].flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW)
  {
    /* Account for the entry not being in hardware now */
    groupEntriesInHw--;
    groupWasDeleted = L7_TRUE;
  }

  /* Clear out all group information -- including flags */
  memset (&BroadGroupList[table_index], 0,
          sizeof (BroadGroupList[table_index]));
  BroadGroupList[table_index].ipmc_index = table_index;

  /* account for removal of this group from our table */
  groupEntriesInUse--;

  /* allow others to access the table */
  hapiBroadL3McastCritExit ();

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Ask the driver whether the specified route has been used since the last
*          time the function was called
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_USE_GET
* @param   void       *data   - counter values
* @param   DAPI_t     *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   The Broadcom hardware does not support per-group counters. The in-use flag
*          represents usage for all groups combined.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastForwEntryUseGet(DAPI_USP_t *usp,
                                                 DAPI_CMD_t cmd,
                                                 void *data,
                                                 DAPI_t *dapi_g)
{
  L7_RC_t                 result       = L7_SUCCESS;
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd      = (DAPI_ROUTING_INTF_MGMT_CMD_t*)data;
  L7_RC_t                      rc1;
  L7_uint32                    table_index;
  L7_inet_addr_t               groupIp;
  L7_inet_addr_t               srcAddr;
  L7_ushort16                  vid;
  DAPI_PORT_t                  *dapiPortPtr;
  BROAD_PORT_t                 *hapiPortPtr;
  usl_bcm_ipmc_addr_t           ipmc;
  L7_BOOL                       inuse = L7_FALSE;
  int                           rv;

  hapiBroadL3McastCritEnter ();

  inetCopy(&groupIp, &dapiCmd->cmdData.mcastUseQuery.mcastGroupAddr);
  inetCopy(&srcAddr, &dapiCmd->cmdData.mcastUseQuery.srcIpAddr);

  rc1 = hapiBroadL3McastFindGroup(&groupIp, &srcAddr, &table_index);
  if (rc1 == L7_FAILURE)
  {
    /* Specified group does not exist. Nothing to do.
    */
    hapiBroadL3McastCritExit ();
    dapiCmd->cmdData.mcastUseQuery.entryUsed = L7_FALSE;
    HAPI_L3_MCAST_DEBUG("Could not find the entry");
    return result;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    vid = hapiPortPtr->port_based_routing_vlanid;
  }
  else
  {
    vid = dapiPortPtr->modeparm.router.vlanID;
  }
  memset(&ipmc, 0, sizeof(ipmc));

  ipmc.vid = vid;
  ipmc.ipmc_index = table_index;
  hapiBroadL3McastSetIpAddress(&srcAddr, &groupIp, &ipmc);
  rv = usl_bcmx_ipmc_inuse_get(&ipmc, &inuse);
  dapiCmd->cmdData.mcastUseQuery.entryUsed  = inuse;
  hapiBroadL3McastCritExit ();
  return result;
}

/*********************************************************************
*
* @purpose Ask the driver how many frames are received for a specific group
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_COUNTERS_GET
* @param   void       *data   - counter values
* @param   DAPI_t     *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   The Broadcom hardware does not support per-group counters.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastForwEntryCtrGet(DAPI_USP_t *usp,
                                                 DAPI_CMD_t cmd,
                                                 void *data,
                                                 DAPI_t *dapi_g)
{
  L7_RC_t                 result       = L7_SUCCESS;
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd      = (DAPI_ROUTING_INTF_MGMT_CMD_t*)data;

  dapiCmd->cmdData.mcastCountQuery.byteCount               = 0;
  dapiCmd->cmdData.mcastCountQuery.packetCount             = 0;
  dapiCmd->cmdData.mcastCountQuery.wrongIntfCount           = 0;
  dapiCmd->cmdData.mcastCountQuery.byteCountSupported     = L7_FALSE;
  dapiCmd->cmdData.mcastCountQuery.packetCountSupported   = L7_FALSE;
  dapiCmd->cmdData.mcastCountQuery.wrongIntfCountSupported = L7_FALSE;

  return result;

}

/*********************************************************************
*
* @purpose Add a port to the multicast group entry.
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD
* @param   void       *data   - Group and port information
* @param   DAPI_t     *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   The Broadcom implementation does not use this function.
*          The downstream link information is derived from the
*          address add message.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastPortAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 result       = L7_SUCCESS;
  return result;

}

/*********************************************************************
*
* @purpose Delete a port from the multicast group entry.
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_PORT_DELETE
* @param   void       *data   - Group and port information
* @param   DAPI_t     *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   The Broadcom driver does not use this function.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastPortDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 result       = L7_SUCCESS;

  return result;
}


/*********************************************************************
*
* @purpose Notify HAPI that VLAN config changed.
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_VLAN_PORT_MEMBER_UPDATE
* @param   void       *data   - VLAN and port information
* @param   DAPI_t     *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   The Broadcom implementation does not do anything with
*          this call. The changes in VLAN membership and tagging
*          are passed to the IP Mcast components from the HAPI
*          VLAN management code.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastVlanPortMemberUpdate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 result       = L7_SUCCESS;

  return result;
}

/*********************************************************************
*
* @purpose Set time to live limit for a specific interface.
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET
* @param   void       *data   - TTL
* @param   DAPI_t     *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   Broadcom actually supports a per route entry TTL and a per egress port TTL.
*          For FASTPATH, the TTL is per egress interface so the hapi driver sets the
*          route entry TTL to 0 and sets the egress port TTL with this command.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastTtlSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 result = L7_SUCCESS;
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t                 *hapiPortPtr;
  BROAD_PORT_t                 *hapiLagMemberPortPtr;
  DAPI_PORT_t                  *dapiPortPtr;
  L7_int32                      rv;
  L7_ushort16                   vlan_id;
  DAPI_PORT_t                  *dapiPortPtr2;
  DAPI_USP_t                    searchUsp;
  int                           ipmc_untag_flag;
  bcm_mac_t                     mac_old;
  bcm_vlan_t                    vlan_old;
  int                           ttl_old;
  DAPI_LAG_ENTRY_t             *lagMemberSet;
  usl_bcm_l3_intf_t             intfInfo;

  HAPI_L3_MCAST_DEBUG("usp = %d %d %d ttl = %lu\n", usp->unit, usp->slot, usp->port,
                           dapiCmd->cmdData.ttlMcastVal.ttlVal);
  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* If TTL is the same then don't do anything.
  */
  if (hapiPortPtr->multicast_ttl_limit == dapiCmd->cmdData.ttlMcastVal.ttlVal)
  {
    return result;
  }

  hapiBroadL3McastCritEnter ();

  hapiPortPtr->multicast_ttl_limit = dapiCmd->cmdData.ttlMcastVal.ttlVal;

  /* Modify the L3 interface to set the Mutlicast TTL threshold */
  if (hapiPortPtr->l3_interface_id != HAPI_BROAD_INVALID_L3_INTF_ID)
  {
    usl_bcm_l3_intf_t_init(&intfInfo);
    if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
    {
      memcpy(intfInfo.bcm_data.l3a_mac_addr, dapiPortPtr->modeparm.physical.macAddr, sizeof(bcm_mac_t));
      intfInfo.bcm_data.l3a_vid = hapiPortPtr->port_based_routing_vlanid;
    }
    else
    {
      memcpy(intfInfo.bcm_data.l3a_mac_addr, dapiPortPtr->modeparm.router.macAddr, sizeof(bcm_mac_t));
      intfInfo.bcm_data.l3a_vid = dapiPortPtr->modeparm.router.vlanID;
    }
    intfInfo.bcm_data.l3a_flags |= (BCM_L3_ADD_TO_ARL  | BCM_L3_REPLACE | BCM_L3_WITH_ID);
    intfInfo.bcm_data.l3a_ttl =  hapiPortPtr->multicast_ttl_limit;
    intfInfo.bcm_data.l3a_intf_id = hapiPortPtr->l3_interface_id;
    rv = usl_bcmx_l3_intf_create(&intfInfo);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      HAPI_L3_MCAST_DEBUG("Modify the ttl failed return %d", rv);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't create the L3 intf %d, rv = %d", hapiPortPtr->l3_interface_id, rv);
    }
  }


  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    /* Setup the ipmc egress port config */

    ipmc_untag_flag = TRUE; /* untagged */
    rv = bcmx_ipmc_egress_port_set(hapiPortPtr->bcmx_lport,
                                   dapiPortPtr->modeparm.physical.macAddr, /* mac */
                                   ipmc_untag_flag, /* untag / tag */
                                   hapiPortPtr->port_based_routing_vlanid, /* vid */
                                   hapiPortPtr->multicast_ttl_limit); /* ttl threshold */
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set the egress port info for port 0x%x, rv = %d", hapiPortPtr->bcmx_lport, rv);
    }
  }
  else
  {
    vlan_id = dapiPortPtr->modeparm.router.vlanID;


    /* For VLAN-based routing interface find all ports that are members of the
    ** VLAN and change their ttl
    ** Skip ports that are members of LAGs or port-based router.
    */
    for (searchUsp.unit=0; searchUsp.unit < dapi_g->system->totalNumOfUnits; searchUsp.unit++)
    {
      for (searchUsp.slot=0; searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots; searchUsp.slot++)
      {
        if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardPresent == L7_TRUE)
        {
          for (searchUsp.port=0; searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot; searchUsp.port++)
          {
            if (isValidUsp (&searchUsp, dapi_g) != L7_TRUE)
            {
             continue;
            }

            dapiPortPtr2 = dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->port[searchUsp.port];
            hapiPortPtr  = dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->port[searchUsp.port]->hapiPort;

            if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr2) == L7_TRUE))
            {
              if ((hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE) ||
                  (dapiPortPtr2->modeparm.physical.routerIntfEnabled == L7_TRUE))
              {
                continue;
              }

              if (BROAD_IS_VLAN_MEMBER(&searchUsp, vlan_id, dapi_g) )
              {
                /* Get the tagging so we can preserve */
                rv = bcmx_ipmc_egress_port_get(hapiPortPtr->bcmx_lport, /* logical port */
                                               mac_old, /* mac */
                                               &ipmc_untag_flag, /* untag / tag */
                                               &vlan_old, /* vid */
                                               &ttl_old); /* ttl threshold */
                if (L7_BCMX_OK(rv) != L7_TRUE)
                {
                  L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't get the egress port info for port 0x%x, rv = %d", hapiPortPtr->bcmx_lport, rv);
                }

                /* Setup the ipmc egress port config */
                rv = bcmx_ipmc_egress_port_set(hapiPortPtr->bcmx_lport, /* logical port */
                                               dapiPortPtr->modeparm.router.macAddr, /* mac */
                                               ipmc_untag_flag, /* untag / tag */
                                               vlan_id, /* vid */
                                               dapiCmd->cmdData.ttlMcastVal.ttlVal); /* ttl threshold */
                if (L7_BCMX_OK(rv) != L7_TRUE)
                {
                  L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set the egress port info for port 0x%x, rv = %d", hapiPortPtr->bcmx_lport, rv);
                }
              }
            }
          }
        }
      }
    }

    searchUsp.unit = L7_LOGICAL_UNIT;
    searchUsp.slot = L7_LAG_SLOT_NUM;
    for (searchUsp.port=0; searchUsp.port < L7_MAX_NUM_LAG_INTF; searchUsp.port++)
    {
      dapiPortPtr2 = dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->port[searchUsp.port];
      hapiPortPtr  = dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->port[searchUsp.port]->hapiPort;

      if (BROAD_IS_VLAN_MEMBER(&searchUsp, vlan_id, dapi_g) )
      {
        lagMemberSet = DAPI_PORT_GET(&searchUsp, dapi_g)->modeparm.lag.memberSet;

        if (lagMemberSet[0].inUse == L7_TRUE)
        {
            hapiLagMemberPortPtr = HAPI_PORT_GET(&lagMemberSet[0].usp, dapi_g);

            /* Get the tagging so we can preserve */
            rv = bcmx_ipmc_egress_port_get(hapiLagMemberPortPtr->bcmx_lport, /* logical port */
                                           mac_old, /* mac */
                                           &ipmc_untag_flag, /* untag / tag */
                                           &vlan_old, /* vid */
                                           &ttl_old); /* ttl threshold */
            if (L7_BCMX_OK(rv) != L7_TRUE)
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't get the egress port info for port 0x%x, rv = %d", hapiPortPtr->bcmx_lport, rv);
            }

            /* Setup the ipmc egress port config */
            rv = bcmx_ipmc_egress_port_set(hapiLagMemberPortPtr->bcmx_lport, /* logical port */
                                           dapiPortPtr->modeparm.router.macAddr, /* mac */
                                           ipmc_untag_flag, /* untag / tag */
                                           vlan_id, /* vid */
                                           dapiCmd->cmdData.ttlMcastVal.ttlVal); /* ttl threshold */
            if (L7_BCMX_OK(rv) != L7_TRUE)
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set the egress port info for port 0x%x, rv = %d", hapiPortPtr->bcmx_lport, rv);
            }
        }
      }
    }
  }

  hapiBroadL3McastCritExit ();
  return result;
}

/*********************************************************************
*
* @purpose Inform the driver that the application wants to see unregistered
           IP multicast frames.
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG
* @param   void       *data   - Multicast routing enable/disable command
* @param   DAPI_t     *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingMcastFwdConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{

  DAPI_ROUTING_MGMT_CMD_t    *dapiCmd      = (DAPI_ROUTING_MGMT_CMD_t*)data;
  L7_RC_t                    result       = L7_SUCCESS;
  L7_int32                   rv;

  if (dapiCmd->cmdData.mcastforwardConfig.enable == L7_TRUE)
  {
    /* Enable IP multicast */
    rv = usl_bcmx_ipmc_enable(1);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      HAPI_L3_MCAST_DEBUG(" IPMC enabled failed");
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't enable IPMC mode, rv = %d", rv);
    }
    dapi_g->system->mcastEnable = L7_TRUE;
  }
  else
  {
    /* Disable IP multicast */
    rv = usl_bcmx_ipmc_enable(0);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      HAPI_L3_MCAST_DEBUG(" IPMC disabled failed ");
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't disable IPMC mode, rv = %d", rv);
    }
    dapi_g->system->mcastEnable = L7_FALSE;
  }
  return result;

}

/*********************************************************************
*
* @purpose Inform the driver that the application wants to see IGMP frames.
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_ROUTE_MCAST_IGMP_CONFIG
* @param   void       *data   - IGMP enable/disable command
* @param   DAPI_t     *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes This function enables reception of IGMP frames on all interfaces.
* Its intention is to receive IGMP messages on interfaces where multicast
* routing is not enabled.  For Broadcom, IP multicast is enabled for the
* entire chip (not per interface) so the IGMP messages will come in due to
* the default multicast route. The filter to prioritize the IGMP packets
* have to be created if IGMP is enabled
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingMcastIgmpConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                   result       = L7_SUCCESS;
  DAPI_ROUTING_MGMT_CMD_t   *dapiCmd   = (DAPI_ROUTING_MGMT_CMD_t*)data;
  L7_int32                  rv;
  L7_BOOL                   switchFrame = L7_FALSE;

  if (dapiCmd->cmdData.mcastIgmpConfig.getOrSet != DAPI_CMD_SET)
  {
    result =  L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: Failure in %s!\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  if (dapiCmd->cmdData.mcastforwardConfig.family == L7_AF_INET)
  {
    if (dapiCmd->cmdData.mcastIgmpConfig.enable == L7_TRUE)
    {
      dapi_g->system->igmpEnable = L7_TRUE;
      if (!dapi_g->system->igmpSnoopingEnable)
      {
        /* Since IGMPsnooping is not yet enabled, filter does not exist */
        /* Enable forwarding of IGMP frames when L3 IGMP is configured */
        switchFrame = L7_TRUE;
        hapiBroadConfigIgmpFilter(L7_TRUE, L7_NULL, switchFrame, dapi_g);       /* PTin modified: IGMP snooping */
      }
      else
      {
        /* Since IGMPsnooping is already enabled, then reinstall this
         * filter to have proper ordering of rules. Make sure that
         * IGMP frames are trapped to CPU as snooping application
         * will take care of forwarding it to eligible interfaces.
         */
        hapiBroadConfigIgmpFilter(L7_FALSE, L7_NULL, L7_FALSE, dapi_g);         /* PTin modified: IGMP snooping */
        switchFrame = L7_FALSE;
        hapiBroadConfigIgmpFilter(L7_TRUE, L7_NULL, switchFrame, dapi_g);       /* PTin modified: IGMP snooping */
      }
    }
    else
    {
      dapi_g->system->igmpEnable = L7_FALSE;
      if (!dapi_g->system->igmpSnoopingEnable)
      {
        /* Since IGMPsnooping is also not enabled, filter can be removed */
        hapiBroadConfigIgmpFilter(L7_FALSE, L7_NULL, L7_FALSE, dapi_g);         /* PTin modified: IGMP snooping */
      }
    }
  }
  else
  { /* For IPv6 family MLD will be applicable instead of IGMP */
    hapiBroadL3McastCritEnter();
    if (dapiCmd->cmdData.mcastIgmpConfig.enable == L7_TRUE)
    {
      if (dapi_g->system->mldEnable != L7_TRUE)
      {
        /* we don't want to tunnel, so get the packets via a filter
         * the priority must be set
         * */
        hapiBroadConfigL3V6McastFilter(L7_TRUE);

        rv = bcmx_switch_control_set(bcmSwitchMldPktDrop, 0);
        if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
        {
          L7_LOG_ERROR(rv);
        }
        rv = bcmx_switch_control_set(bcmSwitchMldPktToCpu, 1);
        if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
        {
          L7_LOG_ERROR(rv);
        }

       /* Set the MLD packet priority. This is required for FB2 devices or later.
        * For other platforms, MLD packets will follow protocol priority.
        * The return code of E_UNAVAIL is masked by the API.
        */
        rv = bcmx_switch_control_set(bcmSwitchCpuProtoIgmpPriority,
                                    HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                "\n%s %d: In %s Failed to set hapiBroadRoutingMcastIgmpConfig - %d\n",
                __FILE__, __LINE__, __FUNCTION__, rv);
        }

        dapi_g->system->mldEnable = L7_TRUE;
      }
    }
    else
    {
      if (dapi_g->system->mldEnable != L7_FALSE)
      {
        hapiBroadConfigL3V6McastFilter(L7_FALSE);

        rv = bcmx_switch_control_set(bcmSwitchMldPktDrop, 0);
        if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
        {
          L7_LOG_ERROR(rv);
        }
        rv = bcmx_switch_control_set(bcmSwitchMldPktToCpu, 0);
        if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
        {
          L7_LOG_ERROR(rv);
        }
        dapi_g->system->mldEnable = L7_FALSE;
      }
    }
     hapiBroadL3McastCritExit();
  }
  return result;
}

/*********************************************************************
*
* @purpose Create a ingress filter to prioratize PIM and DVMRP packets
*
* @param    @b{(input)} enableFilter
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadConfigL3McastFilter(L7_BOOL enableFilter)
{
  L7_RC_t                 result = L7_SUCCESS;
  static BROAD_POLICY_t   mrpFilterId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t     ruleId = BROAD_POLICY_RULE_INVALID;
  L7_ushort16             ip_ethtype = L7_ETYPE_IP;
  L7_uchar8               igmp_proto[]  = {IP_PROT_IGMP};
  L7_uchar8               pim_proto[]  = {IP_PROT_PIM};
  L7_uchar8               exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                       FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  L7_ushort16             dvmrp_type  = 0x1300; /* DVMRP packet type */
  L7_ushort16             dvmrp_type_match = 0xff00;

   /*
   * Only apply the filter once, stack members added after the fact will be
   * synchronized by the policy manager code
   */
  if ((enableFilter == L7_TRUE) && (mrpFilterId == BROAD_POLICY_INVALID))
  {
    do
    {
      if ((result = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM)) != L7_SUCCESS)
        break;

      /* Give DVMRP frames high proority. */
      if ((result = hapiBroadPolicyRuleAdd(&ruleId)) != L7_SUCCESS)
        break;

      if ((result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE,(L7_uchar8 *)&ip_ethtype, exact_match)) != L7_SUCCESS)
        break;

      if ((result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO,igmp_proto, exact_match)) != L7_SUCCESS)
        break;

      if ((result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_SPORT,(L7_uchar8 *)&dvmrp_type, (L7_uchar8 *)&dvmrp_type_match)) != L7_SUCCESS)
        break;

      if ((result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0)) != L7_SUCCESS)
        break;

      /* Give PIM frames high priority */
      if ((result = hapiBroadPolicyRuleAdd(&ruleId)) != L7_SUCCESS)
        break;

      if ((result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE,(L7_uchar8 *)&ip_ethtype, exact_match)) != L7_SUCCESS)
        break;

      if ((result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO,pim_proto, exact_match)) != L7_SUCCESS)
        break;

      if ((result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0)) != L7_SUCCESS)
        break;

    } while ( 0 );

    if (result == L7_SUCCESS)
    {
      result = hapiBroadPolicyCommit(&mrpFilterId);
    }
    else
    {
      hapiBroadPolicyCreateCancel();
    }
    if (result != L7_SUCCESS)
    {
      if (mrpFilterId != BROAD_POLICY_INVALID)
      {
        /* attempt to delete the policy in case it was created */
        (void)hapiBroadPolicyDelete(mrpFilterId);
        mrpFilterId = BROAD_POLICY_INVALID;
      }
    }
  }
  else if (enableFilter != L7_TRUE)
  {
    if (mrpFilterId != BROAD_POLICY_INVALID )
    {
      result = hapiBroadPolicyDelete(mrpFilterId);
      mrpFilterId = BROAD_POLICY_INVALID;
    }
  }

  return result;
}


/*********************************************************************
*
* @purpose Create a ingress filter to prioratize PIM and DVMRP packets
*
* @param    @b{(input)} enableFilter
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadConfigL3V6McastFilter(L7_BOOL enableFilter)
{
  L7_RC_t                 result = L7_SUCCESS;
  BROAD_POLICY_RULE_t     ruleId = BROAD_POLICY_RULE_INVALID;
  L7_uchar8               exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                       FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};

  static BROAD_POLICY_t   Ip6MrpId = BROAD_POLICY_INVALID;
  L7_ushort16             ip6_ethtype   = L7_ETYPE_IPV6;
  L7_uchar8               pim6_proto[]  = {IP_PROT_PIM};
   /*
   * Only apply the filter once, stack members added after the fact will be
   * synchronized by the policy manager code
   */
  if ((enableFilter == L7_TRUE) && (Ip6MrpId == BROAD_POLICY_INVALID))
  {
    do
    {
      /* Add a rule for PIM IPv6 to elevate it's priority */
      ruleId = BROAD_POLICY_RULE_INVALID;
      if (( result = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM)) != L7_SUCCESS)
        break;
      if (( result = hapiBroadPolicyRuleAdd(&ruleId)) != L7_SUCCESS)
        break;
      if (( result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip6_ethtype, exact_match)) != L7_SUCCESS)
        break;
      if (( result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IP6_NEXTHEADER,   pim6_proto, exact_match)) != L7_SUCCESS)
        break;
      if (( result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0,0)) != L7_SUCCESS)
        break;
    } while ( 0 );

    if (result == L7_SUCCESS)
    {
      result = hapiBroadPolicyCommit(&Ip6MrpId);
    }
    else
    {
      hapiBroadPolicyCreateCancel();
    }

    if (result != L7_SUCCESS)
    {
      if (Ip6MrpId != BROAD_POLICY_INVALID)
      {
        (void)hapiBroadPolicyDelete(Ip6MrpId);
        Ip6MrpId = BROAD_POLICY_INVALID;
      }
    }
  }
  else if (enableFilter != L7_TRUE)
  {
    if (Ip6MrpId != BROAD_POLICY_INVALID)
    {
      L7_RC_t tmp_result = L7_SUCCESS;
      tmp_result = hapiBroadPolicyDelete(Ip6MrpId);
      Ip6MrpId = BROAD_POLICY_INVALID;
      if (result == L7_SUCCESS) result = tmp_result;
    }
  }

  return result;
}

/*********************************************************************
* @purpose  Update L3 Mcast entries to reflect the fact that a port
*           has been added to/removed from the specified VLAN.
*
* @param    DAPI_USP_t *usp    - unit slot port
* @param                vlan_id - VLAN ID
* @param                add_remove L7_TRUE - Port has been added to the VLAN
*                                  L7_FALSE - Port has been removed from the VLAN
* @param    DAPI_t     *dapi_g - system information
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void hapiBroadL3McastPortVlanAddDelete (DAPI_USP_t *usp,
                                               L7_ushort16 vlan_id,
                                               L7_BOOL add_remove,
                                               DAPI_t *dapi_g)
{
  L7_uint32 table_index;
  L7_uint32 port_index;
  DAPI_USP_t                    searchUsp;
  DAPI_PORT_t                  *dapiPortPtr;
  BROAD_PORT_t                 *hapiPortPtr;
  BROAD_PORT_t                 *hapiLagMemberPortPtr;
  L7_int32                      rv;
  DAPI_LAG_ENTRY_t             *lagMemberSet;
  L7_int32                    ipmc_index;

  static L7_uint32              ipmc_group_index [L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL];
  L7_uint32                     num_ipmc_groups;
  L7_uint32                     tagged;
  bcmx_lport_t                  lport[L7_MAX_MEMBERS_PER_LAG];
  L7_uint32                     lport_count = 0;
  L7_uint32                     lport_idx;

  L7_uchar8                     mac[6];
  L7_uint32                     ttl = 0;

  L7_uint32                     i;
  bcm_chip_family_t             board_family=0;

  HAPI_L3_MCAST_DEBUG("hapiBroadL3McastPortVlanAddDelete: usp %d:%d:%d vlan id %d add_remove %d",
         usp->unit, usp->slot, usp->port, vlan_id, add_remove);

  hapiBroadL3McastCritEnter ();

  /* If no multicast groups are configured then there is nothing to be updated. */
  if (groupEntriesInUse == 0)
  {
    hapiBroadL3McastCritExit ();
    return;
  }

  hapiBroadL3McastCritExit ();


  /* We are only interested in VLANs that are enabled for routing.
   * For port based routing interfaces, we cannot add ports to reserved vlans.
   * Also, this callback ignores all port based routing intfs anyway
   */
  searchUsp.unit = L7_LOGICAL_UNIT;
  searchUsp.slot = L7_VLAN_SLOT_NUM;
  for (searchUsp.port = 0; searchUsp.port < L7_MAX_NUM_VLAN_INTF; searchUsp.port++)
  {
    dapiPortPtr = DAPI_PORT_GET(&searchUsp, dapi_g);
    if (dapiPortPtr->modeparm.router.vlanID == vlan_id)
       break;
  }

  if (searchUsp.port == L7_MAX_NUM_VLAN_INTF)
      return;

  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);

  /* Ignore ports that are LAG members.
  */
  /* Only disallow LAG members for non-XGS3 devices. Otherwise we allow
     LAG members to be added since XGS supports non-unicast traffic
     distribution over the LAG. */
  hapiBroadGetSystemBoardFamily(&board_family);
  if ((board_family == BCM_FAMILY_DRACO) ||
      (board_family == BCM_FAMILY_TUCANA))
  {
    if (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
    {
      HAPI_L3_MCAST_DEBUG("Port is member of LAG");
      return;
    }
  }

  if (usp->slot == L7_LAG_SLOT_NUM)
  {
    /* Now go through the LAG interfaces. For XGS3, add all member ports to the bitmap.
       Otherwise add only one member port. */
    lagMemberSet = dapiPortPtr->modeparm.lag.memberSet;
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (lagMemberSet[i].inUse == L7_TRUE)
      {
          hapiLagMemberPortPtr = HAPI_PORT_GET(&lagMemberSet[i].usp,dapi_g);
          lport[lport_count++] = hapiLagMemberPortPtr->bcmx_lport;
      }
      /*For XGS3 we can distribute non-unicast
               traffic across the lag members*/
      if ((board_family == BCM_FAMILY_DRACO) ||
          (board_family == BCM_FAMILY_TUCANA))
      {
        if (lagMemberSet[0].inUse == L7_FALSE)
        {
          return;
        }
        else
        {
          break;
        }
      }
    }
  } else
  {
    lport[lport_count++] = hapiPortPtr->bcmx_lport;
  }

  hapiBroadL3McastCritEnter ();

  num_ipmc_groups = 0;
  if (BROAD_IS_VLAN_TAGGING(usp, vlan_id, dapi_g))
  {
    tagged = 1;
  } else
  {
    tagged = 0;
  }

  /* scan through the Group List table */
  for (table_index = 0; table_index < L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL; table_index++)
  {
    if ((BROAD_GROUP_ENTRY_FLAGS_IN_USE | BROAD_GROUP_ENTRY_FLAGS_IN_HW) !=
        (BroadGroupList[table_index].flags & (BROAD_GROUP_ENTRY_FLAGS_IN_USE | BROAD_GROUP_ENTRY_FLAGS_IN_HW)))
    {
      /* entry is not in use or not in hardware; skip it */
      continue;
    }

    if ((add_remove == L7_TRUE) && 
        (BROAD_GROUP_ENTRY_FLAGS_INGRESS_SNOOP_MODIFY == 
        (BroadGroupList[table_index].flags & BROAD_GROUP_ENTRY_FLAGS_INGRESS_SNOOP_MODIFY)))
    {
      /* entry is owned by snooping; skip addition of interface */
      continue;
    }

    if (isValidUsp (&BroadGroupList[table_index].rpf_usp, dapi_g) == L7_FALSE)
    {
      /* entry ingress path is not valid; skip it */
      continue;
    }

    /* handle update of incoming vlan interface */
    dapiPortPtr = DAPI_PORT_GET(&BroadGroupList[table_index].rpf_usp,dapi_g);

    /* If VLAN-routing interface VLAN ID does not match the new VLAN ID then ignore this
    ** interface.
    */
    if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE) &&
        (vlan_id == dapiPortPtr->modeparm.router.vlanID))
    {
      /* but it does match, and it's a VLAN interface, so add it */
      ipmc_index = BroadGroupList[table_index].ipmc_index;

      ipmc_group_index [num_ipmc_groups] = ipmc_index;
      num_ipmc_groups++;

    }
  }

  for (lport_idx = 0; lport_idx < lport_count; lport_idx++)
  {
    /* Add or remove L2 ports for all groups to/from the hardware.
    */
    if (add_remove)
    {
      if (num_ipmc_groups > 0)
      {
        HAPI_L3_MCAST_DEBUG("Going to add port %d to vlan %d, groups %d", lport[lport_idx], vlan_id, num_ipmc_groups);
        rv = usl_bcmx_ipmc_add_l2_port_groups (lport[lport_idx],
                                               ipmc_group_index,
                                               num_ipmc_groups,
                                               vlan_id,
                                               tagged);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't add L2 port 0x%x to IPMC indices, rv = %d", lport[lport_idx], rv);
        }
      }
    } else
    {
      if (num_ipmc_groups > 0)
      {
        HAPI_L3_MCAST_DEBUG("Going to delete port %d to vlan %d, groups %d", lport[lport_idx], vlan_id, num_ipmc_groups);
        rv = usl_bcmx_ipmc_delete_l2_port_groups (lport[lport_idx],
                                                  ipmc_group_index,
                                                  num_ipmc_groups,
                                                  vlan_id, 0);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't delete L2 port 0x%x from IPMC indices, rv = %d", lport[lport_idx], rv);
        }
      }
    }
  }

  /* handle update of outgoing vlan interfaces
  */
  num_ipmc_groups = 0;
  for (table_index = 0; table_index < L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL; table_index++)
  {
    if ((BROAD_GROUP_ENTRY_FLAGS_IN_USE | BROAD_GROUP_ENTRY_FLAGS_IN_HW) !=
        (BroadGroupList[table_index].flags & (BROAD_GROUP_ENTRY_FLAGS_IN_USE|BROAD_GROUP_ENTRY_FLAGS_IN_HW)))
    {
      /* entry is not in use or not in hardware; skip it */
      continue;
    }

    for (port_index = 0; port_index < L7_MAX_NUM_ROUTER_INTF; port_index++ )
    {
      if (!(BroadGroupList[table_index].downstream_if[port_index].flags & BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_USE))
      {
        /* this downstream interface is not in use; skip it */
        continue;
      }

      if ((add_remove == L7_TRUE) &&
          (BROAD_GROUP_LOGICAL_PORT_FLAGS_EGRESS_SNOOP_MODIFY ==
          (BroadGroupList[table_index].downstream_if[port_index].flags &  BROAD_GROUP_LOGICAL_PORT_FLAGS_EGRESS_SNOOP_MODIFY)))
      {
        /* entry is owned by snooping; skip addition of interface */
        continue;
      }

      if (isValidUsp (&BroadGroupList[table_index].downstream_if[port_index].usp, dapi_g) == L7_FALSE)
      {
        /* entry ingress path is not valid; skip it */
        continue;
      }

      dapiPortPtr = DAPI_PORT_GET(&BroadGroupList[table_index].downstream_if[port_index].usp,dapi_g);
      hapiPortPtr = HAPI_PORT_GET(&BroadGroupList[table_index].downstream_if[port_index].usp,dapi_g);

      /* If port is a port-based router port then ignore it.
      */
      if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) &&
          (dapiPortPtr->modeparm.physical.routerIntfEnabled))
      {
        continue;
      }

      /* If VLAN-routing interface VLAN ID does not match the new VLAN ID then ignore this
      ** interface.
      */
      if (vlan_id != dapiPortPtr->modeparm.router.vlanID)
      {
        continue;
      }

      ipmc_index = BroadGroupList[table_index].ipmc_index;

      ipmc_group_index [num_ipmc_groups] = ipmc_index;
      num_ipmc_groups++;

      ttl = hapiPortPtr->multicast_ttl_limit;
      memcpy (mac, dapiPortPtr->modeparm.router.macAddr, 6);

    }
  }

  for (lport_idx = 0; lport_idx < lport_count; lport_idx++)
  {
    /* Add or remove L3 ports for all groups to/from the hardware.
    */
    if (num_ipmc_groups > 0)
    {
      if (add_remove)
      {
        rv = usl_bcmx_ipmc_add_l3_port_groups (lport[lport_idx],
                                               ipmc_group_index,
                                               num_ipmc_groups,
                                               vlan_id,
                                               tagged,
                                               mac,
                                               ttl);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't add L3 port 0x%x to IPMC indices, rv = %d", lport[lport_idx], rv);
        }
      }
      else
      {
        rv = usl_bcmx_ipmc_delete_l3_port_groups (lport[lport_idx],
                                                  ipmc_group_index,
                                                  num_ipmc_groups,
                                                  vlan_id,
                                                  tagged,
                                                  mac,
                                                  ttl);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't delete L3 port 0x%x from IPMC indices, rv = %d", lport[lport_idx], rv);
        }
      }
    }
  }

  hapiBroadL3McastCritExit ();
}

/*********************************************************************
* @purpose  Notify L3 Multicast that a port or LAG was added to a VLAN
*
* @param    *dapiPortPtr  Pointer to the port structure to be added.
* @param    vlan_id       VLAN ID
*
* @returns  none
*
* @notes    The function must be called AFTER the port is added to
*           the VLAN hardware.
*
* @end
*********************************************************************/
void hapiBroadL3McastPortVlanAddNotify (DAPI_USP_t *usp,
                                        L7_ushort16 vlan_id,
                                        DAPI_t   *dapi_g)
{
  hapiBroadL3McastPortVlanAddDelete (usp,
                                     vlan_id,
                                     L7_TRUE,
                                     dapi_g);
}

/*********************************************************************
* @purpose  Notify L3 Multicast that a port or LAG was removed from a VLAN
*
* @param    *dapiPortPtr  Pointer to the port structure to be added.
* @param    vlan_id       VLAN ID
*
* @returns  none
*
* @notes    This function must be called BEFORE the port is removed
*           from the VLAN hardware.
*
* @end
*********************************************************************/
void hapiBroadL3McastPortVlanRemoveNotify (DAPI_USP_t *usp,
                                           L7_ushort16 vlan_id,
                                           DAPI_t *dapi_g)
{
  hapiBroadL3McastPortVlanAddDelete (usp,
                                     vlan_id,
                                     L7_FALSE,
                                     dapi_g);
}

/*********************************************************************
* @purpose  Check to see if IP multicast frames fail RPF check
*
* @param    L7_netBufHandle frameHdl
* @param    L7_ushort16 vlanID
* @param    DAPI_USP_t *usp
* @param    DAPI_t *dapi_g
*
* @returns  L7_SUCCESS - Frame has not been taken .
*           L7_FAILURE - Frame has been consumed.
*
* @notes    If this function returns L7_SUCCESS then the caller
*           should continue processing the frame same way as any Rx frame.
*           This function expects that *usp is a physical or LAG interface.
*
* @end
*********************************************************************/
L7_RC_t hapiBroadL3McastRPF(L7_netBufHandle frameHdl, L7_ushort16 vlanID, DAPI_USP_t *usp,
                            DAPI_t *dapi_g)
{
  DAPI_USP_t                   *rpf_usp;
  BROAD_PORT_t                 *hapiPortPtr;
  DAPI_PORT_t                  *dapiRpfPortPtr;
  L7_uchar8                    *user_data;
  L7_RC_t                       rc1;
  L7_uint32                     table_index;
  L7_uint32                     data_offset;
  L7_inet_addr_t                groupIp;
  L7_inet_addr_t                srcAddr;
  hapi_broad_mcast_rpf_async_t  rpf_frame;
  L7_RC_t                       rc;
  DAPI_USP_t                    master_cpu_usp;
  BROAD_PORT_t                 *master_cpu_port;
  L7_uchar8                     ipProtcolVersion;

  mcast_stats.total_rpf_callbacks++;

  user_data = sysapiNetMbufGetDataStart (frameHdl);

  if (isValidUsp(usp,dapi_g) == L7_FALSE)
  {
    HAPI_L3_MCAST_DEBUG("Invalid usp");
    L7_LOG_ERROR (usp->port);
  }

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* Retrieve source and destination IP addresses from the frame.
  */
  data_offset = sysNetDataOffsetGet (user_data);

  /* Check if this is the IPv4 or IPv6 */
  ipProtcolVersion =  *(L7_uchar8 *)(user_data + data_offset);
  ipProtcolVersion =  (ipProtcolVersion >> 4);

  if (ipProtcolVersion == 4)
  { /* IPv4 packet */
    HAPI_L3_MCAST_DEBUG("IPv4 IPMC packet arrived as RPF failure ipProtcolVersion %d ", ipProtcolVersion);
    memcpy(&groupIp.addr.ipv4.s_addr,(user_data + data_offset + 16),4);
    memcpy(&srcAddr.addr.ipv4.s_addr,(user_data + data_offset + 12),4);

    groupIp.family  =  L7_AF_INET;
    srcAddr.family =  L7_AF_INET;
  }
  else
  { /* IPv6 packet */
    HAPI_L3_MCAST_DEBUG("IPv6 IPMC packet arrived as RPF failure ipProtcolVersion %d ", ipProtcolVersion);
    memcpy((L7_uchar8 *) groupIp.addr.ipv6.in6.addr8,
           (L7_uchar8 *)(user_data + data_offset + 24),16);

    memcpy((L7_uchar8 *) srcAddr.addr.ipv6.in6.addr8,
           (L7_uchar8 *)(user_data + data_offset + 8),16);

    groupIp.family  =  L7_AF_INET6;
    srcAddr.family =  L7_AF_INET6;
  }
  HAPI_L3_MCAST_DEBUG("RFP failure for the address");
  HAPI_L3_ADDR_PRINT_DEBUG(srcAddr, "SRC");
  HAPI_L3_ADDR_PRINT_DEBUG(groupIp, "GRP");
  /* Find a group with the matching IP address.
  */
  rc1 = hapiBroadL3McastFindGroup(&groupIp, &srcAddr, &table_index);
  if (rc1 == L7_FAILURE)
  {
    HAPI_L3_MCAST_DEBUG("Entry was not found in the table");
    return L7_SUCCESS;
  }

  rpf_usp = &BroadGroupList[table_index].rpf_usp;
  if (isValidUsp(rpf_usp,dapi_g) == L7_FALSE)
  {
    HAPI_L3_MCAST_DEBUG("Invalid usp");
    return L7_SUCCESS;
  }
  dapiRpfPortPtr = DAPI_PORT_GET(rpf_usp,dapi_g);

  /* If this packet was received for a mcast group whose
     source port is a physical router intf, it
     is guaranteed to be an RPF failure */
  if (IS_PORT_TYPE_PHYSICAL(dapiRpfPortPtr) == L7_TRUE)
  {
    /* True RPF failure. If the RPF action was drop, the HW
       would have dropped it. So just pass it up to the application.
    */
    HAPI_L3_MCAST_DEBUG("Drop action is not there pass to CPU");
    return L7_SUCCESS;
  }

  /* At this point, we know the packet was received for a mcast group
     whose source port is a VLAN router intf. */
  /* If the VLAN of expected reverse path is the same as the VLAN on which we
  ** received the frame then this RPF failure is simply due to the station
  ** moving to a different port on a VLAN-routing interface.
  */
  if (vlanID != dapiRpfPortPtr->modeparm.router.vlanID)
  {
    /* True RPF failure.
    */

    /* If RPF action for the group is to discard RPF failures then drop the frame.
    */
    if (BroadGroupList[table_index].action_on_rpf_failure == DAPI_MCAST_RPF_CHECK_FAIL_ACTION_DROP)
    {
      /* Free the original frame
      */
      HAPI_L3_MCAST_DEBUG(" Dropping the frame");
      return L7_FAILURE;             /* indicate frame consumed */
    }
    else
    {
      HAPI_L3_MCAST_DEBUG("Return success");
      return L7_SUCCESS;
    }
  }
#ifdef L7_WIRELESS_PACKAGE
  if (IS_SLOT_TYPE_CAPWAP_TUNNEL(usp,dapi_g))
  {
    BROAD_WLAN_TUNNEL_ENTRY_t tunnelEntry;
    hapiBroadWlanInfoGet(dapi_g, hapiPortPtr->bcmx_lport, &tunnelEntry);
    HAPI_BROAD_LPORT_TO_USP(tunnelEntry.wlan_port.port,usp);
    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  }
#endif
  rpf_frame.usp = *usp;
  inetCopy(&rpf_frame.srcAddr, &srcAddr);
  inetCopy(&rpf_frame.groupIp, &groupIp);
  rpf_frame.vlanID = vlanID;

  /* If queue is full then we simply don't send the message. We will get another RPF
  ** failure to prompt us to do work.
  */
  rc = osapiMessageSend (hapiBroadMcastAsyncRpfQueue,
                           (void*)&rpf_frame,
                           sizeof (rpf_frame),
                           L7_NO_WAIT,
                           L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    mcast_stats.rpf_queue_full_drops++;
  }

  /* Notify multicast async task that there is work to do.
  */
  hapiBroadL3McastAsyncNotify();

  /* If the packet is received from a remote CPU, discard it at this point
   * as we will get another copy of the packet through VLAN flooding */
  if (CPU_USP_GET(&master_cpu_usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  master_cpu_port = HAPI_PORT_GET(&master_cpu_usp, dapi_g);

  if(bcmx_lport_modid(hapiPortPtr->bcmx_lport) == bcmx_lport_modid(master_cpu_port->bcmx_lport))
  {
    /* Pass to application */
    HAPI_L3_MCAST_DEBUG("Passing to application");
    return L7_SUCCESS;
  } else
  {
    /* Discard */
    HAPI_L3_MCAST_DEBUG("Discarding the packet");
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Handle RPF failure frames.
*
* @param    dapi_g
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void hapiBroadL3McastAsyncRpfHandle(DAPI_t *dapi_g)
{
  hapi_broad_mcast_rpf_async_t rpf_frame;
  L7_RC_t rc;
  int     rv;
  BROAD_PORT_t                 *hapiPortPtr;
  L7_uint32                     table_index;
  usl_bcm_ipmc_addr_t            ipmc;

  memset(&ipmc, 0, sizeof(ipmc));

  do
  {
    rc = osapiMessageReceive(hapiBroadMcastAsyncRpfQueue,
                             (void *)&rpf_frame,
                             sizeof(rpf_frame),
                             L7_NO_WAIT);
    if (rc != L7_SUCCESS)
    {
      HAPI_L3_MCAST_DEBUG("osapiMessageReceive failed");
      break;
    }

    if (isValidUsp(&rpf_frame.usp, dapi_g) == L7_FALSE)
    {
      mcast_stats.rpf_duplicate_drops++;
      HAPI_L3_MCAST_DEBUG("Invalid usp");
      continue;
    }

    hapiPortPtr = HAPI_PORT_GET(&rpf_frame.usp, dapi_g);

    hapiBroadL3McastCritEnter ();

    /* Find a group with the matching IP address.
    */
    rc = hapiBroadL3McastFindGroup(&rpf_frame.groupIp, &rpf_frame.srcAddr, &table_index);
    if (rc == L7_FAILURE)
    {
      hapiBroadL3McastCritExit ();
      mcast_stats.rpf_duplicate_drops++;
      HAPI_L3_MCAST_DEBUG("entry not in the table");
      continue;
    }

    /* If multicast traffic rate is high then we can receive several
    ** RPF failures before we get a chance to update the routing entry.
    ** Don't update the routing entry if there is no real RPF failure.
    ** Let application handle the frame.
    */
    if (memcmp (&rpf_frame.usp, &BroadGroupList[table_index].rpf_physical_usp, sizeof (DAPI_USP_t)) == 0)
    {
      hapiBroadL3McastCritExit ();
      mcast_stats.rpf_duplicate_drops++;
      HAPI_L3_MCAST_DEBUG("Traffic is too high so not doing anything");
      continue;
    }

    /* Modify the RPF interface to match the incoming physical port.
    */
    BroadGroupList[table_index].rpf_physical_usp.unit = rpf_frame.usp.unit;
    BroadGroupList[table_index].rpf_physical_usp.slot = rpf_frame.usp.slot;
    BroadGroupList[table_index].rpf_physical_usp.port = rpf_frame.usp.port;

    if (rpf_frame.usp.slot == L7_LAG_SLOT_NUM)
    {
      /* Incoming port is a trunk. Check trunk state */
      if ((hapiPortPtr->hapiModeparm.lag.numMembers == 0) ||
          (hapiPortPtr->hapiModeparm.lag.tgid == -1))
      {
        hapiBroadL3McastCritExit ();
        continue;
      }
      else
      {
        HAPI_L3_MCAST_DEBUG("Setting ts true");
        ipmc.ts = TRUE;
        ipmc.port_tgid = hapiPortPtr->hapiModeparm.lag.tgid;
      }
    }
    else
    {
      /* port: specify the source logical port */
      HAPI_L3_MCAST_DEBUG("Setting ts false");
      ipmc.ts = FALSE;
      ipmc.port_tgid = BCMX_LPORT_MODPORT(hapiPortPtr->bcmx_lport);
      ipmc.mod_id = BCMX_LPORT_MODID(hapiPortPtr->bcmx_lport);
    }

    mcast_stats.rpf_handle_count++;

    ipmc.vid = rpf_frame.vlanID;
    hapiBroadL3McastSetIpAddress(&rpf_frame.srcAddr, &rpf_frame.groupIp, &ipmc);
    ipmc.ipmc_index = BroadGroupList[table_index].ipmc_index;

    /* modify the expected source port */
    HAPI_L3_MCAST_DEBUG("Calling the usl_bcmx_ipmc_port_tgid_set ts=%d port_tgid=%x",ipmc.ts,ipmc.port_tgid);
    rv = usl_bcmx_ipmc_port_tgid_set(&ipmc);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set TGID for IPMC index %d, rv = %d", ipmc.ipmc_index, rv);
    }

    hapiBroadL3McastCritExit ();


  } while (1);

}


/*********************************************************************
*
* @purpose Scan multicast paths list and add any that are not in
*          hardware to the hardware table.  Checks some basic table
*          consistency and tries to repair if inconsistent.
*
* @param   DAPI_t *dapi_g  (DAPI context)
*
* @returns Whether it wants to run again next time (boolean).
*
* @notes   Only requests a retry when it encounters an invalid
*          condition in the table and has tried to repair it.
*
* @end
*
*********************************************************************/
static L7_BOOL hapiBroadMcastAsyncGroupCheck(DAPI_t *dapi_g)
{
  L7_uint32        index;            /* index for scanning table */
  int              rv;               /* result value for checking insert success */
  L7_BOOL          retry = L7_FALSE; /* flag indicating retry still needed */

  /* Check that entries in hardware == entries in table */
  if (groupEntriesInHw != groupEntriesInUse)
  {
    /* not equal; figure out why */
    if (groupEntriesInHw > groupEntriesInUse)
    {
      L7_uint32 tmpGroupEntriesInUse = 0, tmpGroupEntriesInHw = 0;
      /* Something is out of sync; complain */
      L7_LOGF (L7_LOG_SEVERITY_ALERT, L7_DRIVER_COMPONENT_ID,
               "Multicast groups in hw (%d) > in table (%d); verify table\n",
               groupEntriesInHw,groupEntriesInUse);
      /* Scan the entire table this pass...*/
      hapiBroadL3McastCritEnter();
      for (index = 0; index < L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL; index++)
      {
        /* ...taking account of each entry */
        if (BroadGroupList[index].flags & BROAD_GROUP_ENTRY_FLAGS_IN_USE)
        {
          /* This entry is in use */
          tmpGroupEntriesInUse++;
          if (BroadGroupList[index].flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW)
          {
            /* This entry is also in the hardware */
            tmpGroupEntriesInHw++;
          }
        }
        else /* (BroadGroupList[index].flags & BROAD_GROUP_ENTRY_FLAGS_IN_USE) */
        {
          /* This entry is not in use, but check for hardware flag */
          if (BroadGroupList[index].flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW)
          {
            /* This entry is in the hardware, but not in use; invalid condition */
            L7_LOGF (L7_LOG_SEVERITY_ALERT, L7_DRIVER_COMPONENT_ID,
                     "Multicast group %d marked unused but in hardware\n",
                     index);
            /*
            ** should we do something further here, like scan the table for the
            ** offending address, and if not found, ask the hardware to remove it?
            */
          } /* (BroadGroupList[index].flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW) */
        } /* (BroadGroupList[index].flags & BROAD_GROUP_ENTRY_FLAGS_IN_USE) */
      } /* (index = 0; index < L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL; index++) */

      /* Regenerate the numbers based upon the table state */
      groupEntriesInUse = tmpGroupEntriesInUse;
      groupEntriesInHw = tmpGroupEntriesInHw;

      hapiBroadL3McastCritExit();
      L7_LOGF (L7_LOG_SEVERITY_ALERT, L7_DRIVER_COMPONENT_ID,
               "After table scan: Multicast groups in hw (%d) and in table (%d)\n",
               groupEntriesInHw,groupEntriesInUse);
      /* indicate we want another go at this later */
      retry = L7_TRUE;
    }
    else /* (groupEntriesInHw > groupEntriesInUse) */
    {
      /* Not all entries in the table are in hardware */
      /* Scan all entries in the table and retry (once) add for any missing ones */
      for (index = 0; index < L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL; index++)
      {
        /* For all entries in the multicast table... */
        hapiBroadL3McastCritEnter();
        if ((BroadGroupList[index].flags & BROAD_GROUP_ENTRY_FLAGS_IN_USE) &&
            !(BroadGroupList[index].flags & BROAD_GROUP_ENTRY_FLAGS_IN_HW))
        {
          /* This entry is not in hardware; try to add it again. */
          HAPI_L3_MCAST_DEBUG("Entry in software table but not in hardware; retry add...");
          groupRetryInsert++;
          rv = hapiBroadMcastHardwareAddEntry(&(BroadGroupList[index]),dapi_g);
          if (L7_BCMX_OK(rv) || (BCM_E_EXISTS == rv))
          {
            /* Got the entry into the hardware; add interfaces */
            hapiBroadMcastHardwareSetIfs(&(BroadGroupList[index]),dapi_g);
          }
          else
          {
            /* it didn't go in this time either */
            groupRetryInsertFail++;
          }
        }
        hapiBroadL3McastCritExit();
      } /* (index = 0; index < L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL; index++) */
    } /* (groupEntriesInHw > groupEntriesInUse) */
  } /* (groupEntriesInHw != groupEntriesInUse) */
  return retry;
}


/*********************************************************************
* @purpose  Handle addition and removal of IP multicast groups.
*
* @param    dapi_g
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void hapiBroadL3McastAsyncRouteAddDeleteHandle(DAPI_t *dapi_g)
{
  hapi_broad_mcast_async_msg_t  async_cmd;
  L7_RC_t rc;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  L7_uint32  num_msgs;
  L7_uint32  num_iterations = 0;
  L7_uchar8  mcastMacAddr[L7_MAC_ADDR_LEN];
  L7_uint32  entryIndex;
  usl_bcm_ipmc_addr_t ipmc;

  do
  {
    /* Adding groups may be a slow process, so handle some RPF events.
    */
    num_iterations++;
    if (num_iterations > 5)
    {
      hapiBroadL3McastAsyncRpfHandle(dapi_g);
      num_iterations = 0;
    }

    rc = osapiMsgQueueGetNumMsgs (hapiBroadMcastAsyncCmdQueue, &num_msgs);
    if (rc != L7_SUCCESS)
    {
      L7_LOG_ERROR (rc);
    }

    if (num_msgs > mcast_stats.high_route_queue_length)
    {
      mcast_stats.high_route_queue_length = num_msgs;
    }

    rc = osapiMessageReceive(hapiBroadMcastAsyncCmdQueue,
                             (void *)&async_cmd,
                             sizeof(async_cmd),
                             L7_NO_WAIT);
    if (rc != L7_SUCCESS)
    {
      break;
    }

    switch (async_cmd.async_cmd)
    {
    case HAPI_BROAD_MCAST_ASYNC_ADD:
      dapiCmd.cmdData.mcastAdd.getOrSet = async_cmd.asyncData.mcastAdd.getOrSet;
      inetCopy(&dapiCmd.cmdData.mcastAdd.mcastGroupAddr,
                                &async_cmd.asyncData.mcastAdd.mcastGroupAddr);
      inetCopy(&dapiCmd.cmdData.mcastAdd.srcIpAddr,
                                &async_cmd.asyncData.mcastAdd.srcIpAddr);
      dapiCmd.cmdData.mcastAdd.matchSrcAddr = async_cmd.asyncData.mcastAdd.matchSrcAddr;
      dapiCmd.cmdData.mcastAdd.rpfCheckEnable = async_cmd.asyncData.mcastAdd.rpfCheckEnable;
      dapiCmd.cmdData.mcastAdd.rpfType = async_cmd.asyncData.mcastAdd.rpfType;
      dapiCmd.cmdData.mcastAdd.rpfCheckFailAction = async_cmd.asyncData.mcastAdd.rpfCheckFailAction;
      dapiCmd.cmdData.mcastAdd.outUspCount = async_cmd.asyncData.mcastAdd.outUspCount;
      dapiCmd.cmdData.mcastAdd.outGoingIntfPresent = async_cmd.asyncData.mcastAdd.outGoingIntfPresent;


      dapiCmd.cmdData.mcastAdd.outUspList = async_cmd.asyncData.mcastAdd.outUspList;
      dapiCmd.cmdData.mcastAdd.ingressPhysicalPort = async_cmd.asyncData.mcastAdd.ingressPhysicalPort;

      rc = hapiBroadRoutingIntfMcastAsyncAdd(&async_cmd.usp,
                                             DAPI_CMD_ROUTING_INTF_MCAST_ADD,
                                             &dapiCmd,
                                             dapi_g);
      break;

    case HAPI_BROAD_MCAST_ASYNC_DELETE:
      dapiCmd.cmdData.mcastDelete.getOrSet = async_cmd.asyncData.mcastDelete.getOrSet;
      inetCopy(&dapiCmd.cmdData.mcastDelete.mcastGroupAddr,
                                      &async_cmd.asyncData.mcastDelete.mcastGroupAddr);
      inetCopy(&dapiCmd.cmdData.mcastDelete.srcIpAddr,
                                      &async_cmd.asyncData.mcastDelete.srcIpAddr);
      dapiCmd.cmdData.mcastDelete.matchSrcAddr = async_cmd.asyncData.mcastDelete.matchSrcAddr;
      dapiCmd.cmdData.mcastDelete.rpfCheckEnable = async_cmd.asyncData.mcastDelete.rpfCheckEnable;
      dapiCmd.cmdData.mcastDelete.rpfType = async_cmd.asyncData.mcastDelete.rpfType;
      dapiCmd.cmdData.mcastDelete.rpfCheckFailAction = async_cmd.asyncData.mcastDelete.rpfCheckFailAction;

      rc = hapiBroadRoutingIntfMcastAsyncDelete(&async_cmd.usp,
                                             DAPI_CMD_ROUTING_INTF_MCAST_DELETE,
                                             &dapiCmd,
                                             dapi_g);
      break;

    case HAPI_BROAD_MCAST_ASYNC_SYNC:
      rc = osapiSemaGive (hapiBroadL3McastAsyncWaitSema);
      if (rc != L7_SUCCESS)
      {
        L7_LOG_ERROR (rc);
      }
      break;
    case HAPI_BROAD_MCAST_ASYNC_SNOOP_NOTIFY:
      /* Get -> Source Interfaces, Set -> Destination Interfaces */
      dapiCmd.cmdData.mcastModify.getOrSet = async_cmd.asyncData.mcastModify.getOrSet;
      dapiCmd.cmdData.mcastModify.outIntfDirection = async_cmd.asyncData.mcastModify.outIntfDirection;
      memcpy(mcastMacAddr, async_cmd.asyncData.mcastModify.mcastMacAddr,
             L7_MAC_ADDR_LEN);
      dapiCmd.cmdData.mcastModify.outUspCount = async_cmd.asyncData.mcastModify.outUspCount;
      dapiCmd.cmdData.mcastModify.outUspList = async_cmd.asyncData.mcastModify.outUspList;
      dapiCmd.cmdData.mcastModify.snoopVlanOperState = async_cmd.asyncData.mcastModify.snoopVlanOperState;
      hapiBroadRoutingIntfMcastSnoopAsyncNotifyProcess(mcastMacAddr, &dapiCmd, &async_cmd.usp, dapi_g);
      break;

    case HAPI_BROAD_MCAST_ASYNC_ADD_FAILURE_NOTIFY:
      hapiBroadL3McastCritEnter();

      memset(&ipmc, 0, sizeof(ipmc));
      ipmc.vid = async_cmd.asyncData.mcastAddFailureNotify.vlanId;
      hapiBroadL3McastSetIpAddress(&async_cmd.asyncData.mcastAddFailureNotify.srcIpAddr, &async_cmd.asyncData.mcastAddFailureNotify.mcastGroupAddr, &ipmc);
      ipmc.ipmc_index = async_cmd.asyncData.mcastAddFailureNotify.ipmcIndex;

      /* Don't care about the return code here. */
      (void)usl_bcmx_ipmc_remove(&ipmc, 0);

      if (hapiBroadL3McastFindGroup(&async_cmd.asyncData.mcastAddFailureNotify.mcastGroupAddr,
                                    &async_cmd.asyncData.mcastAddFailureNotify.srcIpAddr,
                                    &entryIndex) == L7_SUCCESS)
      {
        /* mark the entry as not in hardare, and tried to be in hardware */
        BroadGroupList[entryIndex].flags &= ~BROAD_GROUP_ENTRY_FLAGS_IN_HW;
        BroadGroupList[entryIndex].flags |= BROAD_GROUP_ENTRY_FLAGS_IN_HW_FAIL;
        groupEntriesInHw--;
      }

      hapiBroadL3McastCritExit();
      break;

    default:
      L7_LOG_ERROR (async_cmd.async_cmd);
      break;
    }

  } while (1);
}


/*********************************************************************
*
* @purpose Handle route adds/deletes, RPF events, and VLAN membership
*          changes.
*
* @param  none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadRoutingMcastAsyncTask(L7_uint32 num_args, DAPI_t *dapi_g)
{
  L7_uint32 work_available;

  while (1)
  {
    if (osapiMessageReceive(hapiBroadRoutingMcastQueue, (void *)&work_available,
                          sizeof(work_available),
                          L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      L7_LOG_ERROR(0);
    }

    /* Handle any RPFs.
    */
    hapiBroadL3McastAsyncRpfHandle(dapi_g);

    /* Handle route adds and deletes.
    */
    hapiBroadL3McastAsyncRouteAddDeleteHandle(dapi_g);

    /* check the table for unadded groups, if appropriate */
    if (groupWasDeleted)
    {
      /*
      ** A group was recently deleted.  Check the table for pending adds
      ** to see if any of them can be stuck into the newly open slot.  Also
      ** does some table integrity checks (and requests a rescan if it
      ** tried to do any table repairs).  See hapiBroadMcastAsyncGroupCheck
      ** for details of integrity/repair and meaning of the return value.
      */
      groupWasDeleted = hapiBroadMcastAsyncGroupCheck(dapi_g);
    }
  }
}

/*******************************************************************************
* @purpose  Callback function from USL to indicate that an entry could not
*           be inserted into the HW. HAPI acts on this by utilizing the
*           retry mechanism.
*
* @param    data  {(input)} Pointer to entry info
*
* @returns
*
* @comments
*
* @end
*******************************************************************************/
void hapiBroadL3McastAsyncFailureCallback(void *data)
{
  usl_bcm_ipmc_addr_t         *bcm_data = data;
  hapi_broad_mcast_async_msg_t mcastMsg;
  L7_RC_t                      rc;

  memset(&mcastMsg, 0, sizeof(mcastMsg));
  mcastMsg.async_cmd = HAPI_BROAD_MCAST_ASYNC_ADD_FAILURE_NOTIFY;
  mcastMsg.asyncData.mcastAddFailureNotify.vlanId = bcm_data->vid;
  mcastMsg.asyncData.mcastAddFailureNotify.ipmcIndex = bcm_data->ipmc_index;

  if (bcm_data->flags & BCM_IPMC_IP6)
  {
    memcpy(mcastMsg.asyncData.mcastAddFailureNotify.srcIpAddr.addr.ipv6.in6.addr8,
           bcm_data->s_ip_addr,
           sizeof(mcastMsg.asyncData.mcastAddFailureNotify.srcIpAddr.addr.ipv6.in6.addr8));
    mcastMsg.asyncData.mcastAddFailureNotify.srcIpAddr.family = L7_AF_INET6;

    memcpy(mcastMsg.asyncData.mcastAddFailureNotify.mcastGroupAddr.addr.ipv6.in6.addr8,
           bcm_data->mc_ip_addr,
           sizeof(mcastMsg.asyncData.mcastAddFailureNotify.mcastGroupAddr.addr.ipv6.in6.addr8));
    mcastMsg.asyncData.mcastAddFailureNotify.mcastGroupAddr.family = L7_AF_INET6;
  }
  else
  {
    memcpy(&mcastMsg.asyncData.mcastAddFailureNotify.srcIpAddr.addr.ipv4.s_addr,
           bcm_data->s_ip_addr,
           sizeof(mcastMsg.asyncData.mcastAddFailureNotify.srcIpAddr.addr.ipv4.s_addr));
    mcastMsg.asyncData.mcastAddFailureNotify.srcIpAddr.family = L7_AF_INET;

    memcpy(&mcastMsg.asyncData.mcastAddFailureNotify.mcastGroupAddr.addr.ipv4.s_addr,
           bcm_data->mc_ip_addr,
           sizeof(mcastMsg.asyncData.mcastAddFailureNotify.mcastGroupAddr.addr.ipv4.s_addr));
    mcastMsg.asyncData.mcastAddFailureNotify.mcastGroupAddr.family = L7_AF_INET;
  }

  /* If queue is full then we block.
  */
  rc = osapiMessageSend (hapiBroadMcastAsyncCmdQueue,
                           (void*)&mcastMsg,
                           sizeof (mcastMsg),
                           L7_WAIT_FOREVER,
                           L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR (rc);
  }

  /* Notify multicast async task that there is work to do.
  */
  hapiBroadL3McastAsyncNotify();
}

/*********************************************************************
*
* @purpose Informs Mcast async task that there is work to do.
*
* @param  none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadL3McastAsyncNotify(void)
{
  L7_uint32 cmd = 1;

  (void) osapiMessageSend (hapiBroadRoutingMcastQueue,
                           (void*)&cmd,
                           sizeof (cmd),
                           L7_NO_WAIT,
                           L7_MSG_PRIORITY_NORM);

}

/*********************************************************************
*
* @purpose Waits for all pending group add/delete commands to finish.
*
* @param  none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadL3McastWait (void)
{
  hapi_broad_mcast_async_msg_t mcast;
  L7_RC_t    rc;


  mcast.async_cmd =  HAPI_BROAD_MCAST_ASYNC_SYNC;
  rc = osapiMessageSend (hapiBroadMcastAsyncCmdQueue,
                           (void*)&mcast,
                           sizeof (mcast),
                           L7_WAIT_FOREVER,
                           L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR (rc);
  }

  hapiBroadL3McastAsyncNotify ();

  rc = osapiSemaTake (hapiBroadL3McastAsyncWaitSema, L7_WAIT_FOREVER);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR (rc);
  }
}


/*********************************************************************
*
* @purpose Dumps the contents of an entry in the muticast table
*
* @param   none
*
* @returns none
*
* @notes   none
*
* @end
*
*********************************************************************/
static void dumpMcastSwTableEntry(const L7_uint32 tableIndex, const BroadGroupEntryType *entry)
{
  L7_uint32 index;
  L7_uint32 i;

  sysapiPrintf("\n%5d %08X %08X ", tableIndex, entry->ipmc_index, entry->flags);
  /* Now display source address and interface */
  if (L7_AF_INET6 == entry->srcAddr.family)
  {
    /* It's IPv6; display as that */
    for (index = 0; index < 16; index++)
    {
      /* for each byte of the address... */
      if ((index & 1) && (index < 15))
      {
        /* this is an odd octet and not the final one; add ':' after it */
        sysapiPrintf("%02X:", entry->srcAddr.addr.ipv6.in6.addr8[index]);
      }
      else
      {
        /* this is an even octet or the final octet; just display the octet */
        sysapiPrintf("%02X", entry->srcAddr.addr.ipv6.in6.addr8[index]);
      }
    }
  }
  else /* (L7_AF_INET6 == entry->srcAddr.family) */
  {
    /* display the IPv4 address as a quadbyte */
    /* but note wrong byte order if not-big-endian and granularity<32b */
    sysapiPrintf("%08X                                ", entry->srcAddr.addr.ipv4.s_addr);
  } /* (L7_AF_INET6 == entry->srcAddr.family) */
  /* Print the USP data for the ingress interface*/
  sysapiPrintf(" %3d.%3d.%3d",
               entry->rpf_physical_usp.unit,
               entry->rpf_physical_usp.slot,
               entry->rpf_physical_usp.port);
  /* Print the group address */
  sysapiPrintf("\n                        ");
  if (L7_AF_INET6 == entry->groupIp.family)
  {
    /* It's IPv6; display as that */
    for (index = 0; index < 16; index++)
    {
      if ((index & 1) && (index < 15))
      {
        sysapiPrintf("%02X:", entry->groupIp.addr.ipv6.in6.addr8[index]);
      }
      else
      {
        sysapiPrintf("%02X", entry->groupIp.addr.ipv6.in6.addr8[index]);
      }
    }
  }
  else /* (L7_AF_INET6 == entry->srcAddr.family) */
  {
    sysapiPrintf("%08X                                ", entry->groupIp.addr.ipv4.s_addr);
  } /* (L7_AF_INET6 == entry->srcAddr.family) */
  /* Display number of interfaces */
  sysapiPrintf(" OutIfs=%4d",entry->num_ifs);
  /* Now display the egress interface list (start on group IPA line)*/
  for (index = 0, i = 0; index < L7_MAX_NUM_ROUTER_INTF; index++)
  {
    if (entry->downstream_if[index].flags & (BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_USE | BROAD_GROUP_LOGICAL_PORT_FLAGS_IN_HW))
    {
      if (0 == i)
      {
        /* We need to begin a new line; do so */
        sysapiPrintf("\n                        ");
      }
      /* Display this port's USP and flags */
      sysapiPrintf(" %3d.%3d.%3d:%02X     ",
                   entry->downstream_if[index].usp.unit,
                   entry->downstream_if[index].usp.slot,
                   entry->downstream_if[index].usp.port,
                   entry->downstream_if[index].flags);
      /* Adjust how many entries so far on this line (wrap if needed) */
      i = (i + 1) % 3;
    }
  } /* (index = 0, i = 0; index < L7_MAX_NUM_ROUTER_INTF; index++) */
}


/*********************************************************************
*
* @purpose Dumps multicast statistics
*
* @param   none
*
* @returns none
*
* @notes   none
*
* @end
*
*********************************************************************/
void dumpMcastSwStats()
{
  /* Display the multicast statistics */
  sysapiPrintf("\nMulticast s/w table information\n");
  sysapiPrintf("\nEntries in s/w table = %12d; in h/w  = %12d",groupEntriesInUse,groupEntriesInHw);
  sysapiPrintf("\nApp. group inserts   = %12d; failed  = %12d",groupAppInsert,groupAppInsertFail);
  sysapiPrintf("\nRetry group inserts  = %12d; failed  = %12d",groupRetryInsert,groupRetryInsertFail);
  sysapiPrintf("\nLast insert fail c/b @ %12d; current = %12d",lastCollisionCallbackTime,osapiUpTimeRaw());
  sysapiPrintf("\nEntry del since try? = %s\n",groupWasDeleted?"TRUE":"FALSE");
}

/*********************************************************************
*
* @purpose Dumps the contents of the multicast table
*
* @param   none
*
* @returns none
*
* @notes   none
*
* @end
*
*********************************************************************/
void dumpMcastSwTable()
{
  L7_uint32   tableIndex;

  /* Display the multicast statistics first */
  dumpMcastSwStats();
  /* Display the table header */
  sysapiPrintf("\nSer   T.idx    Flags    Source                                   Src_Intf");
  sysapiPrintf("\n                        Group                                    Out_Intf_Count");
  sysapiPrintf("\n                        (Out Intf list if applicable; might span rows)");
  sysapiPrintf("\n----- -------- -------- ---------------------------------------- --------------");
  /* scan through the Group List table */
  for (tableIndex = 0; tableIndex < L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL; tableIndex++)
  {
    /* Only print entries that are of interest... */
    if (BroadGroupList[tableIndex].flags & (BROAD_GROUP_ENTRY_FLAGS_IN_USE | BROAD_GROUP_ENTRY_FLAGS_IN_HW))
    {
      /* This one is interesting; print the serial, table index, flags */
      dumpMcastSwTableEntry(tableIndex,&(BroadGroupList[tableIndex]));
    }
  }
  /* Now end the table */
  sysapiPrintf("\n\n");
}


