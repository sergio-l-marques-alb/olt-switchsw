/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_l2_std.c
*
* @purpose   This file contains the entry points to the Layer 2 interface
*
* @component hapi
*
* @comments
*
* @create    2/22/2001
*
* @author    nsummers
*
* @end
*
**********************************************************************/

#include <string.h>

#include "commdefs.h"
#include "datatypes.h"
#include "osapi.h"
#include "sysapi.h"

#include "broad_common.h"
#include "broad_l2_std.h"
#include "broad_l2_mcast.h"
#include "broad_l3.h"
#ifdef L7_WIRELESS_PACKAGE
#include "broad_l2_tunnel.h"
#endif
#include "broad_l2_vlan.h"
#include "broad_l2_lag.h"
#include "broad_l2_ipsg.h"
#include "broad_policy.h"
#include "broad_cos_util.h"
#include "bcm/l2.h"
#include "soc/drv.h"
#include "bcm/mcast.h"
#include "bcm/vlan.h"
#include "soc/mem.h"
#include "soc/arl.h"
#include "bcm/stack.h"

#include "bcmx/port.h"
#include "bcmx/lport.h"
#include "bcmx/l2.h"
#include "bcmx/l3.h"
#include "bcmx/stg.h"
#include "bcmx/auth.h"
#include "bcmx/vlan.h"
/* PTin removed: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
/* No include */
#else
#include "bcm_int/esw/draco.h"
#endif
#include "bcm_int/robo/l2.h"
#include "bcm_int/esw/firebolt.h"
/* PTin removed: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
/* No include */
#else
#include "bcm_int/esw/easyrider.h"
#endif
#include "ibde.h"
#include "l7_usl_bcmx_l2.h"
#include "l7_usl_bcmx_port.h"
#include "l7_usl_api.h"
#include "bcm/auth.h"
#include "bcm_int/robo/auth.h"
#include "osapi_support.h"
#include "dot1s_exports.h"
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
#include "broad_dot1ad.h"
#endif

#include "logger.h"
#include "ptin_hapi_xconnect.h"
#include "ptin_hapi_l2.h"

extern int _bcm_esw_l2_from_l2x(int unit, soc_mem_t mem, bcm_l2_addr_t *l2addr, uint32 *l2_entry);

//L7_BOOL ptin_learnEnabled[L7_MAX_INTERFACE_COUNT];    /* PTin added: MAC learning */

/* keep stats regarding the number of learns/ages processed */
typedef struct
{
  L7_uint32 age;
  L7_uint32 ageErr;
  L7_uint32 nonNativeAge;
  L7_uint32 learn;
  L7_uint32 learnErr;
  L7_uint32 learnFantom;
  L7_uint32 nonNativeLearn;
} hapiMacStats_t;

static hapiMacStats_t hapiMacStats;

/* stats regarding the number of flushes */
typedef struct
{
  L7_uint32  hapiBroadMaxFlushesInLoop;
  L7_uint32  hapiBroadPortFlushesDone;
  L7_uint32  hapiBroadPortFlushesIssued;
  L7_uint32  hapiBroadVlanFlushesIssued;
  L7_uint32  hapiBroadTrunkFlushesIssued;
  L7_uint32  hapiBroadMacFlushesIssued;
  L7_uint32  hapiBroadInvalidFlushesIssued;
} hapiBroadFlushStats_t;

static void *hapiL2AddrFlushQueue;
static void *hapiFrameLearnAsyncQueue;

/* Flush requests for physical ports are enqued by the application in this lplist */
static bcmx_lplist_t  hapiBroadFlushApplpList;

/* Hapi Flush task works on this lplist */
static bcmx_lplist_t  hapiBroadFlushTasklpList;

/* Semaphore to protect hapiBroadFlushApplpList */
static void *hapiBroadFlushSema = L7_NULLPTR;

static L7_uint32 *hapiBroadFlushTgidList  = L7_NULLPTR;
static L7_uint32 *hapiBroadFlushVlanList  = L7_NULLPTR;
static bcm_mac_t hapiBroadFlushMac = {0,};
static hapiBroadFlushStats_t hapiBroadFlushStats_g;
void hapiBroadL2FlushRequest(BROAD_L2ADDR_FLUSH_t flushReq);

/* Double Vlan Tag Ethertype initialized in  hapiBroadL2Init()*/
L7_ushort16 hapiBroadDvlanEthertype = L7_NULL;

/* Static MAC Filters */
/* Track which filters are applied to which ports. */
typedef struct
{
  L7_BOOL           inUse;
  L7_enetMacAddr_t  mac;
  L7_ushort16       vlanID;
  BROAD_POLICY_t    policyId;
  L7_int32          numOfSrcUsp;
  DAPI_USP_t        srcUsp[L7_FILTER_MAX_INTF];
}
BROAD_MAC_FILTERS_t;

BROAD_MAC_FILTERS_t *broadMacFilters;

static void *broadMacFilterSema = L7_NULLPTR;

/* DHCP Filtering PolicyID */
extern BROAD_POLICY_t    dhcpFilterPolicyId;

typedef struct
{
  bcm_stg_t stg;
  L7_uint32 instNumber;

} HAPI_BROAD_STG_t;

/* We have to map from application layer MST instance ID to */
/* Broadcom's 1-255 index where 1 is already the default in BCMX layer */
static HAPI_BROAD_STG_t broadDot1sMapping[HAPI_MAX_MULTIPLE_STP_INSTANCES];

/* We want to reduce number of calls to bcm_l2_addr_remove_by_port
 * hence, we are going to use this structure to check when the above
 * was called for a particular port.
 */
#define  L7_MIN_L2_ADDR_REMOVAL_TIMEOUT 15
#define  L7_L2ADDR_REMOVE_MAX_PORTS     (L7_MAX_PORT_COUNT + L7_MAX_LOGICAL_PORTS_PER_SLOT)

typedef struct LportL2AddrRemoveTime
{
  bcmx_lport_t   lport;
  L7_uint32      lastRemoveTime;
  L7_BOOL        port_is_lag;

}  LportL2AddrRemoveTime;

#define HAPI_BROAD_DOT1S_DEBUG

#ifdef HAPI_BROAD_DOT1S_DEBUG
L7_uint32           hapiDot1sDebug = 0;
#define HAPI_BROAD_DOT1S_DEBUG_ENQUEUE 1
#define HAPI_BROAD_DOT1S_DEBUG_DEQUEUE 2

void hapiBroadDebugDot1sEnable(L7_uint32 enable)
{
  hapiDot1sDebug = enable;
}
#endif

void hapiBroadAddrMacUpdateLearn(bcmx_l2_addr_t *bcmx_l2_addr, DAPI_t *dapi_g);
void hapiBroadAddrMacUpdateAge(bcmx_l2_addr_t *bcmx_l2_addr, DAPI_t *dapi_g);
static L7_RC_t hapiBroadPortDoubleVlanTagConfig(DAPI_USP_t *usp,
                                                DAPI_INTF_MGMT_CMD_t *dapiCmd,
                                                DAPI_t *dapi_g);
static L7_uint32         dtagPortCount = 0;

static L7_BOOL  l2_inited = L7_FALSE;

/*static L7_BOOL hapiVoiceVlanDebug = 0;

void hapiBroadStdDebugSet(L7_BOOL val)
{
   hapiVoiceVlanDebug=val;
}*/

/* Stp Async queue and async done sema.
*/
static void *hapiBroadDot1sAsyncCmdQueue, *hapiBroadDot1sAsyncDoneSema;

/* L2 Async queue.
*/
static void *hapiBroadL2AsyncCmdQueue;

/* LAG Async queue and async done sema.
*/
void *hapiBroadLagAsyncCmdQueue, *hapiBroadLagAsyncDoneSema;

/* synchronization sema for COSQ policies */
void  *broadCOSQsema = L7_NULLPTR;

/* synchronization sema to obtain the driver VLAN table */
void  *broadVlanSema = L7_NULLPTR;

/*******************************************************************************
*
* @purpose Re-Initialize L2 functions.
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
void hapiBroadL2ReInit(void)
{
  L7_uint32                    index;
  L7_RC_t                      rc;

  if (l2_inited == L7_FALSE)
  {
    return;
  }

  if (hapiBroadCosPolicyUtilInit() != L7_SUCCESS)
  {
    return;
  }

  hapiBroadL2VlanReInit ();
  hapiBroadL2McastReInit ();
  memset(broadMacFilters,0,(L7_MAX_FDB_STATIC_FILTER_ENTRIES * sizeof(BROAD_MAC_FILTERS_t)));

  /* Initialize the Spanning Tree Mapping */
  /* Our CIST Id 0 must get mapped to STP Id 1 in BCMX */
  /* (BCMX's index - 1) would be the array index */
  for (index = 0; index < HAPI_MAX_MULTIPLE_STP_INSTANCES; index++)
  {
    broadDot1sMapping[index].stg        = BCM_STG_INVALID;
    broadDot1sMapping[index].instNumber = -1;
  }
  rc = hapiBroadDot1sBcmStgMapSet(DOT1S_CIST_ID, BCM_STG_DEFAULT);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

///* PTin added: MAC learning */
//for (index = 0; index < L7_MAX_INTERFACE_COUNT; index++)
//  ptin_learnEnabled[index] = L7_TRUE;
///* PTin end */
}

/*******************************************************************************
*
* @purpose Initialize the non port specific Layer 2 functionality
*
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL2Init(DAPI_t *dapi_g)
{

  L7_RC_t result = L7_SUCCESS;
  L7_uint32 filter_size;

  l2_inited = L7_TRUE;

  memset((void *) &hapiMacStats,0,sizeof(hapiMacStats));

  filter_size = L7_MAX_FDB_STATIC_FILTER_ENTRIES * sizeof (BROAD_MAC_FILTERS_t);
  broadMacFilters = osapiMalloc (L7_DRIVER_COMPONENT_ID, filter_size);
  if (filter_size == 0)
  {
    LOG_ERROR (0);
  }
  memset (broadMacFilters, 0, filter_size);

  /* Create the flushing task and its message queue */
  (void)hapiBroadL2AddrFlushInit (dapi_g);

  broadMacFilterSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);

  if (broadMacFilterSema == L7_NULLPTR)
    LOG_ERROR(0);

  broadCOSQsema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (broadCOSQsema == L7_NULL)
  {
    LOG_ERROR(0);
  }


  broadVlanSema = osapiSemaMCreate(OSAPI_SEM_Q_PRIORITY);
  if (broadVlanSema == L7_NULLPTR)
  {
    LOG_ERROR(0);
  }

  hapiBroadL2ReInit();

  result = hapiBroadL2McastInit(dapi_g);

  hapiBroadLagInit ();

  /* Start the mac sync task for non-stacking package */
#ifndef L7_STACKING_PACKAGE
  hapiBroadL2AddrMacSyncInit(dapi_g);
#endif

  /* Create the L2 Async message queues and task */
  (void)hapiBroadL2AsyncInit(dapi_g);

  hapiBroadIpsgInit(dapi_g);
  hapiBroadDot1xInit(dapi_g);

  /*voice vlan change*/
  hapiBroadVoiceVlanInit(dapi_g);

  /* Assign Default TPID */
  if (hapiBroadDvlanEthertype == L7_NULL)
  {
    hapiBroadDvlanEthertype = platDVLANTagDefaultEthTypeGet();
  }

  result = hapiBroadCosPolicyUtilInit();

  return result;
}



/*********************************************************************
*
* @purpose Hook in the L2 functionality
*
* @param   DAPI_PORT_t *dapiPortPtr - generic driver port
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2StdPortInit(DAPI_PORT_t *dapiPortPtr)
{

  L7_RC_t result = L7_SUCCESS;

  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_DOT1S_INSTANCE_CREATE     ] = (HAPICTLFUNCPTR_t)hapiBroadSystemDot1sInstanceCreate;
  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_DOT1S_INSTANCE_DELETE     ] = (HAPICTLFUNCPTR_t)hapiBroadSystemDot1sInstanceDelete;
  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_ADD]    = (HAPICTLFUNCPTR_t)hapiBroadSystemDot1sInstanceVlanAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_REMOVE] = (HAPICTLFUNCPTR_t)hapiBroadSystemDot1sInstanceVlanRemove;

  dapiPortPtr->cmdTable[DAPI_CMD_INTF_DOT1S_STATE                 ] = (HAPICTLFUNCPTR_t)hapiBroadIntfDot1sState;

  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_DOT1X_CONFIG          ] = (HAPICTLFUNCPTR_t)hapiBroadSystemDot1xConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_DOT1X_STATUS            ] = (HAPICTLFUNCPTR_t)hapiBroadIntfDot1xStatus;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_DOT1X_CLIENT_ADD        ] = (HAPICTLFUNCPTR_t)hapiBroadIntfDot1xClientAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_DOT1X_CLIENT_REMOVE     ] = (HAPICTLFUNCPTR_t)hapiBroadIntfDot1xClientRemove;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_DOT1X_CLIENT_TIMEOUT    ] = (HAPICTLFUNCPTR_t)hapiBroadIntfDot1xClientTimeout;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_DOT1X_CLIENT_BLOCK      ] = (HAPICTLFUNCPTR_t)hapiBroadIntfDot1xClientBlock;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_DOT1X_CLIENT_UNBLOCK    ] = (HAPICTLFUNCPTR_t)hapiBroadIntfDot1xClientUnblock;

  dapiPortPtr->cmdTable[DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG   ] = (HAPICTLFUNCPTR_t)hapiBroadIntfDoubleVlanTagConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_MULTI_TPID_DOUBLEVLAN_TAG_CONFIG   ] = (HAPICTLFUNCPTR_t)hapiBroadIntfDoubleVlanTagMultiTpidConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_DEFAULT_TPID_DOUBLEVLAN_TAG_CONFIG  ] = (HAPICTLFUNCPTR_t)hapiBroadDoubleVlanTagDefaultTpidConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_MAC_LOCK_CONFIG         ] = (HAPICTLFUNCPTR_t)hapiBroadIntfMacLockConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_LLDP_CONFIG             ] = (HAPICTLFUNCPTR_t)hapiBroadIntfLldpConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_DHCP_SNOOPING_CONFIG         ] = (HAPICTLFUNCPTR_t)hapiBroadIntfDhcpSnoopingConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_DYNAMIC_ARP_INSPECTION_CONFIG] = (HAPICTLFUNCPTR_t)hapiBroadIntfDynamicArpInspectionConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_IPSG_CONFIG             ] = (HAPICTLFUNCPTR_t)hapiBroadIntfIpsgConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_IPSG_STATS_GET          ] = (HAPICTLFUNCPTR_t)hapiBroadIntfIpsgStatsGet;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_IPSG_CLIENT_ADD         ] = (HAPICTLFUNCPTR_t)hapiBroadIntfIpsgClientAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_IPSG_CLIENT_DELETE      ] = (HAPICTLFUNCPTR_t)hapiBroadIntfIpsgClientDelete;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_CAPTIVE_PORTAL_CONFIG   ] = (HAPICTLFUNCPTR_t)hapiBroadIntfCaptivePortalConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_ADD   ] = (HAPICTLFUNCPTR_t)hapiBroadAddrMacAddressEntryAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_MODIFY] = (HAPICTLFUNCPTR_t)hapiBroadAddrMacAddressEntryModify;
  dapiPortPtr->cmdTable[DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_DELETE] = (HAPICTLFUNCPTR_t)hapiBroadAddrMacAddressEntryDelete;
  dapiPortPtr->cmdTable[DAPI_CMD_ADDR_AGING_TIME              ] = (HAPICTLFUNCPTR_t)hapiBroadAddrAgingTime;
  dapiPortPtr->cmdTable[DAPI_CMD_ADDR_FLUSH                   ] = (HAPICTLFUNCPTR_t)hapiBroadAddrFlush;
  dapiPortPtr->cmdTable[DAPI_CMD_ADDR_FLUSH_VLAN              ] = (HAPICTLFUNCPTR_t)hapiBroadAddrFlushByVlan;
  dapiPortPtr->cmdTable[DAPI_CMD_ADDR_FLUSH_MAC               ] = (HAPICTLFUNCPTR_t)hapiBroadAddrFlushByMac;
  dapiPortPtr->cmdTable[DAPI_CMD_ADDR_FLUSH_ALL               ] = (HAPICTLFUNCPTR_t)hapiBroadAddrFlushAll;
  dapiPortPtr->cmdTable[DAPI_CMD_ADDR_SYNC                    ] = (HAPICTLFUNCPTR_t)hapiBroadAddrSync;
//dapiPortPtr->cmdTable[DAPI_CMD_ADDR_SET_LEARN_MODE          ] = (HAPICTLFUNCPTR_t)hapiBroadAddrSetLearnMode;  /* PTin added: MAC learning */


  dapiPortPtr->cmdTable[DAPI_CMD_ADDR_MAC_FILTER_ADD          ] = (HAPICTLFUNCPTR_t)hapiBroadAddrMacFilterAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_ADDR_MAC_FILTER_DELETE       ] = (HAPICTLFUNCPTR_t)hapiBroadAddrMacFilterDelete;

  dapiPortPtr->cmdTable[DAPI_CMD_LOGICAL_INTF_CREATE          ] = (HAPICTLFUNCPTR_t)hapiBroadAddrLogicalIntfCreate;
  dapiPortPtr->cmdTable[DAPI_CMD_LOGICAL_INTF_DELETE          ] = (HAPICTLFUNCPTR_t)hapiBroadAddrLogicalIntfDelete;

  dapiPortPtr->cmdTable[DAPI_CMD_PROTECTED_PORT_ADD           ] = (HAPICTLFUNCPTR_t)hapiBroadL2ProtectedGroupConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_PROTECTED_PORT_DELETE        ] = (HAPICTLFUNCPTR_t)hapiBroadL2ProtectedPortDelete;

#ifdef L7_IPVLAN_PACKAGE
  dapiPortPtr->cmdTable[DAPI_CMD_IPSUBNET_VLAN_CREATE         ] = (HAPICTLFUNCPTR_t)hapiBroadL2VlanIpSubnetEntryAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_IPSUBNET_VLAN_DELETE         ] = (HAPICTLFUNCPTR_t)hapiBroadL2VlanIpSubnetEntryDelete;
#endif
#ifdef L7_MACVLAN_PACKAGE
 dapiPortPtr->cmdTable[DAPI_CMD_MAC_VLAN_CREATE             ] = (HAPICTLFUNCPTR_t)hapiBroadL2VlanMacEntryAdd;
 dapiPortPtr->cmdTable[DAPI_CMD_MAC_VLAN_DELETE              ] = (HAPICTLFUNCPTR_t)hapiBroadL2VlanMacEntryDelete;
#endif
 dapiPortPtr->cmdTable[DAPI_CMD_VOICE_VLAN_DEVICE_ADD        ] = (HAPICTLFUNCPTR_t)hapiBroadVoiceVlanDeviceAdd;
 dapiPortPtr->cmdTable[DAPI_CMD_VOICE_VLAN_DEVICE_REMOVE     ] = (HAPICTLFUNCPTR_t)hapiBroadVoiceVlanDeviceRemove;
 dapiPortPtr->cmdTable[DAPI_CMD_VOICE_VLAN_COS_OVERRIDE      ] = (HAPICTLFUNCPTR_t)hapiBroadVoiceVlanCosOverride;
 dapiPortPtr->cmdTable[DAPI_CMD_VOICE_VLAN_AUTH              ] = (HAPICTLFUNCPTR_t)hapiBroadVoiceVlanAuth;

return result;

}

/*********************************************************************
*
* @purpose Create a dot1s spanning tree instance
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_DOT1S_INSTANCE_CREATE
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dot1sInstanceCreate
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDot1sInstanceCreate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t         result = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t    *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  bcm_stg_t             stg;
  L7_uint32              instNumber;
  L7_int32              rc;


  /* Get the spanning tree instance ID */
  instNumber = dapiCmd->cmdData.dot1sInstanceCreate.instNumber;

#ifdef HAPI_BROAD_DOT1S_DEBUG
  if (hapiDot1sDebug & HAPI_BROAD_DOT1S_DEBUG_ENQUEUE)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                   "\nCreating Instance: %d\n",
                   instNumber);
  }
#endif

  /* If request to create the default id then nothing to do */
  if (DOT1S_CIST_ID == instNumber)
  {
    /* Broadcom reserves stg=1 during init.
    ** The mapping is already set up.
    */
    return L7_SUCCESS;
  }

  /* Call BCMX to create the STG instance */
  rc = usl_bcmx_stg_create (instNumber, &stg);

  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'bcmx_stg_create' - FAILED : %d\n",
                   __FILE__, __LINE__, __FUNCTION__, rc);
  } else
  {
    /* Since creation was successful, set the mapping */
    if (hapiBroadDot1sBcmStgMapSet(instNumber,stg) == L7_FAILURE)
      return L7_FAILURE;
  }

  return result;
}

/*********************************************************************
*
* @purpose Delete a dot1s spanning tree instance
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_DOT1S_INSTANCE_DELETE
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dot1sInstanceDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDot1sInstanceDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t               result = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t    *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  bcm_stg_t             stg;
  L7_uint32             instNumber, stg_index;
  L7_int32              rc;

  /* Get the spanning tree instance ID */
  instNumber = dapiCmd->cmdData.dot1sInstanceDelete.instNumber;

#ifdef HAPI_BROAD_DOT1S_DEBUG
  if (hapiDot1sDebug)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                   "\nDeleting Instance: %d\n",
                   instNumber);
  }
#endif

  /* If request to delete the default id */
  if (DOT1S_CIST_ID == instNumber)
    return L7_FAILURE;

  if (hapiBroadDot1sBcmStgMapGet(instNumber,&stg,&stg_index) == L7_FAILURE)
    return L7_FAILURE;

  /* Call BCMX to delete the STG instance */
  rc = usl_bcmx_stg_destroy (instNumber, stg);

  if (BCM_E_NOT_FOUND == rc)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI,
                   "\n%s %d: In %s call to 'bcmx_stg_destroy' - FAILED : %s\n",
                   __FILE__, __LINE__, __FUNCTION__, bcm_errmsg(rc));
  } else if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'bcmx_stg_destroy' - FAILED : %s\n",
                   __FILE__, __LINE__, __FUNCTION__, bcm_errmsg(rc));
  } else
  {
    /* Reset the entry */
    if (hapiBroadDot1sBcmStgMapClear(instNumber) == L7_FAILURE)
      return L7_FAILURE;
  }

  return result;

}


/*********************************************************************
*
* @purpose Add VLAN to a particular dot1s spanning tree
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_ADD
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dot1sInstanceVlanAdd
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDot1sInstanceVlanAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t         result = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t    *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  bcm_stg_t             stg;
  bcm_vlan_t            vlanId;
  L7_uint32             instNumber;
  L7_int32              rc;
  L7_uint32             stg_index;
  BROAD_SYSTEM_t       *hapiSystemPtr;

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  /* Get the spanning tree instance ID  */
  instNumber = dapiCmd->cmdData.dot1sInstanceVlanAdd.instNumber;
  /* Get the VLAN Id */
  vlanId = (bcm_vlan_t) dapiCmd->cmdData.dot1sInstanceVlanAdd.vlanId;


  if (dapiCmd->cmdData.dot1sInstanceVlanAdd.vlanId > L7_PLATFORM_MAX_VLAN_ID)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s - Invalid VLAN ID: %d\n",
                   __FILE__, __LINE__, __FUNCTION__,
                   dapiCmd->cmdData.dot1sInstanceVlanAdd.vlanId);
    return L7_FAILURE;
  }

  /* Get the BCMX mapping for this instance ID */
  if (hapiBroadDot1sBcmStgMapGet(instNumber,&stg,&stg_index) == L7_FAILURE)
    return L7_FAILURE;

  if (hapiSystemPtr->stg_instNumber[vlanId] == instNumber)
    return result;

  /* Store instNumber for vlan mapping */
  hapiSystemPtr->stg_instNumber[vlanId] = instNumber;

  rc = usl_bcmx_stg_vlan_add (instNumber, stg, vlanId);

#ifdef HAPI_BROAD_DOT1S_DEBUG
  if (hapiDot1sDebug)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                   "  Adding VLAN %d to instance %d\n",
                   vlanId, instNumber);
  }
#endif

  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'bcmx_stg_vlan_add' - FAILED : %d\n",
                   __FILE__, __LINE__, __FUNCTION__, rc);
  }

  return result;

}


/*********************************************************************
*
* @purpose Remove VLAN from a particular dot1s spanning tree
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_REMOVE
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dot1sInstanceVlanRemove
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDot1sInstanceVlanRemove(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t         result = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t    *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  bcm_stg_t             stg;
  bcm_vlan_t            vlanId;
  L7_uint32             instNumber;
  L7_int32              rc;
  L7_uint32             stg_index;
  BROAD_SYSTEM_t       *hapiSystemPtr;

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  /* Get the spanning tree instance ID */
  instNumber = dapiCmd->cmdData.dot1sInstanceVlanRemove.instNumber;
  /* Get the VLAN Id */
  vlanId = (bcm_vlan_t) dapiCmd->cmdData.dot1sInstanceVlanAdd.vlanId;


  if (dapiCmd->cmdData.dot1sInstanceVlanRemove.vlanId > L7_PLATFORM_MAX_VLAN_ID)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s - Invalid VLAN ID: %d\n",
                   __FILE__, __LINE__, __FUNCTION__,
                   dapiCmd->cmdData.dot1sInstanceVlanRemove.vlanId);
    return L7_FAILURE;
  }

  /* Get the BCMX mapping for this instance ID */
  if (hapiBroadDot1sBcmStgMapGet(instNumber,&stg,&stg_index) == L7_FAILURE)
    return L7_FAILURE;

  if (hapiSystemPtr->stg_instNumber[vlanId] != instNumber)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s - VLAN ID: %d not a member of instance %d\n",
                   __FILE__, __LINE__, __FUNCTION__,
                   dapiCmd->cmdData.dot1sInstanceVlanRemove.vlanId, instNumber);
    return L7_FAILURE;
  }

#ifdef HAPI_BROAD_DOT1S_DEBUG
  if (hapiDot1sDebug)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                   "Removing VLAN %d from instance %d\n",
                   vlanId, instNumber);
  }
#endif


  /* Reset instNumber to vlan mapping */
  hapiSystemPtr->stg_instNumber[vlanId] = -1;

  rc = usl_bcmx_stg_vlan_remove(instNumber, stg, vlanId);

  if ((L7_BCMX_OK(rc) != L7_TRUE) &&
      (rc != BCM_E_NOT_FOUND))
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'bcmx_stg_vlan_remove' - FAILED : %d\n",
                   __FILE__, __LINE__, __FUNCTION__, rc);
    return(L7_FAILURE);
  }

  return result;
}

/*********************************************************************
*
* @purpose Set the port state for this usp in the specified spanning tree
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_DOT1S_STATE
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dot1sState
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDot1sStateAsyncSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                   result = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t     *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_DOT1S_STATE_t   state, old_dot1s_state;
  BROAD_PORT_t             *hapiPortPtr;
  DAPI_PORT_t              *dapiPortPtr;
  BROAD_PORT_t             *hapiLagMemberPortPtr;
  bcm_stg_t                 stg;
  bcm_stg_stp_t             stgState = BCM_STG_STP_LISTEN, old_hw_dot1s_state;
  L7_uint32                 instNumber;
  L7_uint32                 i, stg_index;
  L7_int32                  rc;
  bcmx_lport_t              lport;


  /* Get the spanning tree instance ID, action to take and the new state */
  instNumber = dapiCmd->cmdData.dot1sState.instNumber;
  state = dapiCmd->cmdData.dot1sState.state;

  do
  {
    /* Get the BCMX mapping for this instance ID */
    if (hapiBroadDot1sBcmStgMapGet(instNumber,&stg,&stg_index) == L7_FAILURE)
    {
      result = L7_SUCCESS;

      #ifdef HAPI_BROAD_DOT1S_DEBUG
      if (hapiDot1sDebug & HAPI_BROAD_DOT1S_DEBUG_ENQUEUE)
      {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'hapiBroadDot1sBcmStgMapGet' - FAILED : %d (Spanning Tree Instance %d not found)\n",
                         __FILE__, __LINE__, __FUNCTION__, result,instNumber);
      }
      #endif
      break;
    }

  /* Map DAPI state to BCMX state */
  switch (state)
  {
    case DAPI_PORT_DOT1S_DISCARDING:
      stgState = BCM_STG_STP_LISTEN;
      break;
    case DAPI_PORT_DOT1S_LEARNING:
      stgState = BCM_STG_STP_LEARN;
      break;
    case DAPI_PORT_DOT1S_FORWARDING:
      stgState = BCM_STG_STP_FORWARD;
      break;
    case DAPI_PORT_DOT1S_ADMIN_DISABLED:
      stgState = BCM_STG_STP_DISABLE;
      break;
    case DAPI_PORT_DOT1S_NOT_PARTICIPATING:
      stgState = BCM_STG_STP_FORWARD;
      break;
  }

  #ifdef HAPI_BROAD_DOT1S_DEBUG
  if (hapiDot1sDebug & HAPI_BROAD_DOT1S_DEBUG_DEQUEUE)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                   "\nSetting dot1s state for port %d.%d.%d on instance %d to ",
                   usp->unit,usp->slot,usp->port,instNumber);
    switch (state)
    {
      case DAPI_PORT_DOT1S_DISCARDING:
        SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                       "DAPI_PORT_DOT1S_DISCARDING\n")
        break;
      case DAPI_PORT_DOT1S_LEARNING:
        SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                       "DAPI_PORT_DOT1S_LEARNING\n")
        break;
      case DAPI_PORT_DOT1S_FORWARDING:
        SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                       "DAPI_PORT_DOT1S_FORWARDING\n")
        break;
      case DAPI_PORT_DOT1S_ADMIN_DISABLED:
        SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                       "DAPI_PORT_DOT1S_ADMIN_DISABLED\n")
        break;
      case DAPI_PORT_DOT1S_NOT_PARTICIPATING:
        SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                       "DAPI_PORT_DOT1S_NOT_PARTICIPATING\n")
        break;
    }
  }
#endif

  /* Get the logical BCMX port */
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  old_dot1s_state = hapiPortPtr->dot1s_state[stg_index];
  old_hw_dot1s_state = hapiPortPtr->hw_dot1s_state[stg_index];

  hapiPortPtr->dot1s_state[stg_index]    = state;
  hapiPortPtr->hw_dot1s_state[stg_index] = stgState;

  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
      {

        hapiLagMemberPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);

        lport = hapiLagMemberPortPtr->bcmx_lport;

        /* Call BCMX to set the state for a port in a STG instance */
        rc = usl_bcmx_stg_stp_set (stg, lport, stgState);

        if (BCM_E_NOT_FOUND == rc)
        {
          result = L7_SUCCESS;
          #ifdef HAPI_BROAD_DOT1S_DEBUG
          if (hapiDot1sDebug)
          {

            SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                           "\n%s %d: In %s call to 'bcmx_stg_stp_set' - FAILED : %d (Spanning Tree Instance not found)\n",
                           __FILE__, __LINE__, __FUNCTION__, rc);
          }
          #endif
        } else if (L7_BCMX_OK(rc) != L7_TRUE)
        {
          result = L7_FAILURE;
          SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                         "\n%s %d: In %s call to 'bcmx_stg_stp_set' - FAILED : %d\n",
                         __FILE__, __LINE__, __FUNCTION__, rc);
        }
      }
    }
  } else if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {

    if (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE)
    {
        result = L7_SUCCESS;
        break;
    }

    if ((hapiPortPtr->hapiModeparm.physical.isLagAddPending == L7_TRUE) ||
        (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE))
    {
        result = L7_SUCCESS;
        break;
      }

      /* Uncommente bellow to allow hw reconfiguration, even if no change is registered */
      #if 1
      if ((old_dot1s_state == state) &&
          (old_hw_dot1s_state == stgState))
      {
        result = L7_SUCCESS;
        break;
      }
      #endif

      lport = hapiPortPtr->bcmx_lport;

      /* Call BCMX to set the state for a port in a STG instance */
      rc = usl_bcmx_stg_stp_set (stg, lport, stgState);

      if (BCM_E_NOT_FOUND == rc)
      {
        result = L7_SUCCESS;
      } else if (L7_BCMX_OK(rc) != L7_TRUE)
      {
        result = L7_FAILURE;
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'bcmx_stg_stp_set' - FAILED : %d\n",
                       __FILE__, __LINE__, __FUNCTION__, rc);
      }
    }
  }
  while (0);


  dapiCmd->cmdData.dot1sState.rc = result;

  #ifdef HAPI_BROAD_DOT1S_DEBUG
  if (hapiDot1sDebug & HAPI_BROAD_DOT1S_DEBUG_DEQUEUE)
  {
    printf("Passing rc %d for application reference %d\n", result, dapiCmd->cmdData.dot1sState.applicationReference);
  }
#endif

  dapiCallback(usp,
               DAPI_FAMILY_INTF_MGMT,
               DAPI_CMD_INTF_DOT1S_STATE,
               DAPI_EVENT_INTF_DOT1S_STATE_SET_RESPONSE,
               (void *)dapiCmd);

  return result;

}

L7_uint32 maxDot1sQueueProcessTime = 0;
void hapiBroadDebugDumpDot1sQueueStats(L7_BOOL reset)
{
   printf("maxProcessTime %d\n", maxDot1sQueueProcessTime);
   if (reset)
   {
       maxDot1sQueueProcessTime = 0;
   }
}
/*********************************************************************
*
* @purpose Process asynchronous Dot1s commands.
*
* @param   num_msgs  Number of Dot1s messages to be processed
* @param   dapi_g    Driver object
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
static void hapiBroadDot1sAsyncProcessMessages(L7_uint32 num_msgs, DAPI_t *dapi_g)
{
  DAPI_INTF_MGMT_CMD_t          cmdDot1sStateSet;
  hapi_broad_dot1s_async_msg_t  dot1s_cmd;
  L7_uint64                     processTime;

  while (num_msgs--)
  {
    if (osapiMessageReceive(hapiBroadDot1sAsyncCmdQueue, (void *)&dot1s_cmd,
                            sizeof(dot1s_cmd),
                            L7_NO_WAIT) != L7_SUCCESS)
    {
      break;
    }

    switch (dot1s_cmd.async_cmd)
    {
      case HAPI_BROAD_DOT1S_STATE_SET:
        cmdDot1sStateSet.cmdData.dot1sState.getOrSet = DAPI_CMD_SET;
        cmdDot1sStateSet.cmdData.dot1sState.instNumber = (L7_uint32)dot1s_cmd.instNumber;
        cmdDot1sStateSet.cmdData.dot1sState.applicationReference = dot1s_cmd.applicationReference;
        cmdDot1sStateSet.cmdData.dot1sState.state = (DAPI_PORT_DOT1S_STATE_t)dot1s_cmd.state;
        (void)hapiBroadDot1sStateAsyncSet(&dot1s_cmd.usp, DAPI_CMD_INTF_DOT1S_STATE, &cmdDot1sStateSet, dapi_g);
        processTime = osapiTimeMillisecondsGet64() - dot1s_cmd.enqueTime;
        if (processTime > maxDot1sQueueProcessTime)
        {
          maxDot1sQueueProcessTime = processTime;
        }
         break;

      case HAPI_BROAD_DOT1S_ASYNC_DONE:
        osapiSemaGive(hapiBroadDot1sAsyncDoneSema);
        break;

      default:
        LOG_ERROR (dot1s_cmd.async_cmd);
    }
  }
}


/*********************************************************************
*
* @purpose Set the port state for this usp in the specified spanning tree
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_DOT1S_STATE
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dot1sState
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDot1sState(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                    result = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t      *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_DOT1S_STATE_t    state;
  L7_uint32                  instNumber;
  hapi_broad_dot1s_async_msg_t dot1s_cmd;


  /* Get the spanning tree instance ID, action to take and the new state */
  instNumber = dapiCmd->cmdData.dot1sState.instNumber;
  state = dapiCmd->cmdData.dot1sState.state;

#ifdef HAPI_BROAD_DOT1S_DEBUG
  if (hapiDot1sDebug & HAPI_BROAD_DOT1S_DEBUG_ENQUEUE)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                   "\nReceived dot1s state set for port %d.%d.%d on instance %d (AR %d) to ",
                   usp->unit,usp->slot,usp->port,instNumber,dapiCmd->cmdData.dot1sState.applicationReference);
    switch (state)
    {
      case DAPI_PORT_DOT1S_DISCARDING:
        SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                       "DAPI_PORT_DOT1S_DISCARDING\n")
        break;
      case DAPI_PORT_DOT1S_LEARNING:
        SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                       "DAPI_PORT_DOT1S_LEARNING\n")
        break;
      case DAPI_PORT_DOT1S_FORWARDING:
        SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                       "DAPI_PORT_DOT1S_FORWARDING\n")
        break;
      case DAPI_PORT_DOT1S_ADMIN_DISABLED:
        SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                       "DAPI_PORT_DOT1S_ADMIN_DISABLED\n")
        break;
      case DAPI_PORT_DOT1S_NOT_PARTICIPATING:
        SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                       "DAPI_PORT_DOT1S_NOT_PARTICIPATING\n")
        break;
    }
  }
#endif

  dot1s_cmd.async_cmd = (L7_uchar8)HAPI_BROAD_DOT1S_STATE_SET;
  dot1s_cmd.usp.unit = usp->unit;
  dot1s_cmd.usp.slot = usp->slot;
  dot1s_cmd.usp.port = usp->port;

  dot1s_cmd.applicationReference = dapiCmd->cmdData.dot1sState.applicationReference;
  dot1s_cmd.instNumber = (L7_ushort16)instNumber;
  dot1s_cmd.state      = (L7_uchar8)state;
  dot1s_cmd.enqueTime = osapiTimeMillisecondsGet64();
  result = osapiMessageSend (hapiBroadDot1sAsyncCmdQueue,
                             (void*)&dot1s_cmd,
                             sizeof (dot1s_cmd),
                             L7_NO_WAIT,
                             L7_MSG_PRIORITY_NORM);

  /* Signal the L2 Async task that work is pending. */
  (void)hapiBroadL2AsyncTaskSignal();

  return result;

}

/*********************************************************************
*
* @purpose Set the mapping between instNumber and the Broadcom STG
*
* @param   DAPI_USP_t *destUsp
* @param   DAPI_USP_t *srcUsp
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   The destination port must be a physical port.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDot1sBcmStgMapSet(L7_int32 instNumber, bcm_stg_t stg)
{
  L7_uint32 index;

  for (index=0;index<HAPI_MAX_MULTIPLE_STP_INSTANCES;index++)
  {
    if (broadDot1sMapping[index].instNumber == instNumber)
    {
      broadDot1sMapping[index].stg = stg;
      return L7_SUCCESS;
    }
  }
  for (index=0;index<HAPI_MAX_MULTIPLE_STP_INSTANCES;index++)
  {
    if (broadDot1sMapping[index].instNumber == -1)
    {
      broadDot1sMapping[index].instNumber = instNumber;
      broadDot1sMapping[index].stg        = stg;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the mapping between instNumber and the Broadcom STG
*
* @param   DAPI_USP_t *destUsp
* @param   DAPI_USP_t *srcUsp
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   The destination port must be a physical port.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDot1sBcmStgMapGet(L7_uint32 instNumber, bcm_stg_t *stg, L7_uint32 *stg_index)
{
  L7_uint32 index;

  for (index=0;index<HAPI_MAX_MULTIPLE_STP_INSTANCES;index++)
  {
    if (broadDot1sMapping[index].instNumber == instNumber)
    {
      *stg       = broadDot1sMapping[index].stg;
      *stg_index = index;
      return L7_SUCCESS;
    }
  }
  *stg       = BCM_STG_INVALID;
  *stg_index = 0;

  return L7_FAILURE;
}

/******************************************************************************
* @purpose  Get the list of all spanning tree instances
*
* @param    pStgList   - @b{(output)}  Pointer to an array that stores stg id's
*           pStgCount  - @b{(output)}  Number of stp instances
*
* @returns  none
*
* @notes    Memory should be allocated in the calling function for bcm_stg_t*
*
* @end
******************************************************************************/
void hapiBroadDot1sBcmStgListGet(bcm_stg_t* pStgList, L7_int32* pStgCount)
{

  L7_uint32 index;
  L7_int32  count;

  for (index=0, count=0;index<HAPI_MAX_MULTIPLE_STP_INSTANCES;index++)
  {
    if (broadDot1sMapping[index].instNumber != -1)
    {
      pStgList[count++] = broadDot1sMapping[index].stg;
    }
  }

  *pStgCount = count;
}

/* Access routine to support debug routines for iterating */
L7_RC_t hapiBroadDebugDot1sIndexMapGet(L7_uint32 stg_index, L7_int32 *instNumber, bcm_stg_t *stg)
{
  *instNumber = broadDot1sMapping[stg_index].instNumber;
  *stg        = broadDot1sMapping[stg_index].stg;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Clear the mapping between instNumber and the Broadcom STG
*
* @param   DAPI_USP_t *destUsp
* @param   DAPI_USP_t *srcUsp
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   The destination port must be a physical port.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDot1sBcmStgMapClear(L7_int32 instNumber)
{
  L7_uint32 index;

  for (index=0;index<HAPI_MAX_MULTIPLE_STP_INSTANCES;index++)
  {
    if (broadDot1sMapping[index].instNumber == instNumber)
    {
      broadDot1sMapping[index].instNumber = -1;
      broadDot1sMapping[index].stg        = BCM_STG_INVALID;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Copy the dot1s port states from the src USP to dest USP
*
* @param   DAPI_USP_t *destUsp
* @param   DAPI_USP_t *srcUsp
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   The destination port must be a physical port.
*
* @end
*
*********************************************************************/
void hapiBroadDot1sPortStateCopy(DAPI_USP_t *destUsp, DAPI_USP_t *srcUsp, DAPI_t *dapi_g)
{
  DAPI_PORT_t  *dapiDestPortPtr;
  BROAD_PORT_t *hapiDestPortPtr;
  BROAD_PORT_t *hapiSrcPortPtr;
  bcm_stg_t     stg;
  bcmx_lport_t  lport;
  L7_uint32     index;
  L7_uint32     rc;

  dapiDestPortPtr = DAPI_PORT_GET(destUsp, dapi_g);
  hapiDestPortPtr = HAPI_PORT_GET(destUsp, dapi_g);

  hapiSrcPortPtr  = HAPI_PORT_GET(srcUsp, dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiDestPortPtr) == L7_FALSE)
  {
    LOG_ERROR(0);
  }

  for (index = 0; index < HAPI_MAX_MULTIPLE_STP_INSTANCES; index++)
  {
    if (broadDot1sMapping[index].stg != BCM_STG_INVALID)
    {
      stg = broadDot1sMapping[index].stg;

      lport = hapiDestPortPtr->bcmx_lport;

      /* Call BCMX to set the state for a port in a STG instance */
      rc = usl_bcmx_stg_stp_set (stg, lport, hapiSrcPortPtr->hw_dot1s_state[index]);
      if (L7_BCMX_OK(rc) != L7_TRUE)
      {
        LOG_ERROR(rc);
      }
    }
  }
}

/*********************************************************************
*
* @purpose Set the stp state for all groups on a port
*
* @param   DAPI_USP_t *usp
* @param   bcm_stg_stp_t state - the stp state to set the port to
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   The destination port must be a physical port.
*
* @end
*
*********************************************************************/
void hapiBroadDot1sPortAllGroupsStateSet(DAPI_USP_t *usp,
                                         bcm_stg_stp_t state,
                                         DAPI_t *dapi_g)
{
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  bcm_stg_t     stg;
  bcmx_lport_t  lport;
  L7_uint32     index;
  L7_uint32     rc;

  dapiPortPtr  = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr  = HAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE)
  {
    LOG_ERROR(0);
  }

  for (index = 0; index < HAPI_MAX_MULTIPLE_STP_INSTANCES; index++)
  {
    if (broadDot1sMapping[index].stg != BCM_STG_INVALID)
    {
      stg = broadDot1sMapping[index].stg;

      lport = hapiPortPtr->bcmx_lport;

      /* Call BCMX to set the state for a port in a STG instance */
      rc = usl_bcmx_stg_stp_set (stg, lport, state);
      if (L7_BCMX_OK(rc) != L7_TRUE)
      {
        LOG_ERROR(rc);
      }
    }
  }
}

/*********************************************************************
*
* @purpose This routine get the port bit map of ports that are not participating
*          in STP
*
* @param   L7_int32    unit
* @param   DAPI_USP_t *usp
* @param   L7_uint32  *pbmp
* @param   DAPI_t     *dapi_g
*
* @returns NONE
*
* @notes   This routine is for use on reception... it includes all ports
*          w/ link up that are not participating in STP, but excludes the port
*          that originally received the pkt.
*
* @end
*
*********************************************************************/
void hapiBroadGetStpNotParticipatingPbmp(DAPI_USP_t *usp, bcmx_lplist_t *lplist, DAPI_t *dapi_g)
{
  L7_uint32     i;
  DAPI_USP_t    searchUsp;
  BROAD_PORT_t *hapiPortPtr;
  DAPI_PORT_t  *dapiPortPtr;
  DAPI_USP_t    lag_member_usp;

  bcmx_lplist_clear(lplist);

  for (searchUsp.unit=0;searchUsp.unit < dapi_g->system->totalNumOfUnits; searchUsp.unit++)
  {
    for (searchUsp.slot=0;searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots; searchUsp.slot++)
    {
      if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardPresent == L7_TRUE)
      {
        for (searchUsp.port=0;
            searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot;
            searchUsp.port++)
        {
          if (isValidUsp (&searchUsp, dapi_g) != L7_TRUE)
          {
            continue;
          }

          /* Only LAG and Physical ports are supported for transmit bitmap */
          if ((IS_SLOT_TYPE_PHYSICAL(&searchUsp, dapi_g) != L7_TRUE) &&
              (IS_SLOT_TYPE_LOGICAL_LAG(&searchUsp, dapi_g) != L7_TRUE))
          {
            continue;
          }

          hapiPortPtr = HAPI_PORT_GET(&searchUsp,dapi_g);
     
          /* Check if the bpdu flood is enabled */
          if ((hapiPortPtr->dot1s_state[0] != DAPI_PORT_DOT1S_NOT_PARTICIPATING) ||
              (hapiPortPtr->bpduFloodMode != L7_TRUE))
          {
            continue;
          }

          dapiPortPtr = DAPI_PORT_GET(&searchUsp,dapi_g);

          if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
          {
            /* For LAGs, flood BPDU on to first member port in LAG */
            for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
            {
              lag_member_usp = dapiPortPtr->modeparm.lag.memberSet[i].usp;
              if ((dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE) &&
                  (isValidUsp (&lag_member_usp, dapi_g) == L7_TRUE))
              {

                hapiPortPtr = HAPI_PORT_GET(&lag_member_usp,dapi_g);
                break;
              }
            }

            /* If there are no members in LAG, continue */
            if (i == L7_MAX_MEMBERS_PER_LAG)
            {
              continue;
            }
          }

          bcmx_lplist_add(lplist, hapiPortPtr->bcmx_lport);
        }
      }
    }
  }

  hapiBroadPruneRxPort(usp, lplist, dapi_g);
  hapiBroadPruneTxPorts(lplist, dapi_g);

  return;
}

/*********************************************************************
*
* @purpose  Used to check for interface type and call hapiBroadPortDoubleVlanTagConfig
*           accordingly.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.doubleVlanTagConfig
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadIntfDoubleVlanTagConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_SUCCESS;
  DAPI_PORT_t             *dapiPortPtr;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t *)data;
  L7_int32                 i;
  BROAD_PORT_t            *hapiPortPtr;
  L7_uint32                mode;
  bcm_chip_family_t        board_family;
  L7_BOOL                  isTriumphFamily = L7_FALSE;
  int                      rv;


  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);

  if (hapiBroadGetSystemBoardFamily(&board_family) == L7_SUCCESS)
  {
    if ( (board_family == BCM_FAMILY_TRIUMPH) ||
         (board_family == BCM_FAMILY_TRIUMPH2) ||
         (board_family == BCM_FAMILY_SCORPION) /* Scorpion has DVLAN capabilities similar to Triumph. */
        )
    {
      isTriumphFamily = L7_TRUE;
    }
  }

  if (dapiCmd->cmdData.doubleVlanTagConfig.getOrSet == DAPI_CMD_SET)
  {
    /*Check to see if its the first port to be enabled. Make all the other ports uplink*/
    if((dtagPortCount == 0) &&
            (dapiCmd->cmdData.doubleVlanTagConfig.enable == L7_TRUE))
    {
        dapi_g->system->dvlanEnable = L7_TRUE;
        mode = HAPI_BROAD_DTAG_MODE_EXTERNAL;
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
        /* Need Metro (for NNI and UNI_S ports) specific condition */
        if((dapiCmd->cmdData.doubleVlanTagConfig.dot1adInterfaceType == DOT1AD_INTFERFACE_TYPE_NNI ||
            dapiCmd->cmdData.doubleVlanTagConfig.dot1adInterfaceType == DOT1AD_INTFERFACE_TYPE_UNI_S) &&
            isTriumphFamily == L7_TRUE)
        {
          mode = HAPI_BROAD_DTAG_MODE_NONE;
        }
#endif
        /* This function needs to set the port as UNI/NNI port */
        rv = usl_bcmx_dvlan_mode_set(mode);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          return L7_FAILURE;
        }

        rv = usl_bcmx_dvlan_default_tpid_set(dapiCmd->cmdData.doubleVlanTagConfig.etherType);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          return L7_FAILURE;
        }

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
      if(hapiBroadRoboVariantCheck() == __BROADCOM_53115_ID)
      {
         if (hapiBroadRoboDoubleVlanDenyRule(dapi_g, L7_TRUE)!= L7_SUCCESS)
         {
            SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'hapiBroadRoboDoubleVlanDenyRule' - FAILED\n",
                     __FILE__, __LINE__, __FUNCTION__);
         }
         if (hapiBroadDot1adProtoSnoopInstall(dapi_g, L7_TRUE)!= L7_SUCCESS)
         {
            SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'hapiBroadDot1adProtoSnoopInstall' - FAILED\n",
                     __FILE__, __LINE__, __FUNCTION__);
         }
      }
#endif
    }

    if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
    {
      /* This function sets actually the configured TPID */
      result = L7_SUCCESS;
      if (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_FALSE)
      {
        result =  hapiBroadPortDoubleVlanTagConfig(usp,dapiCmd,dapi_g);
        if (result != L7_SUCCESS)
              return result;
      }

    } else if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
    {
      /* iterate over the members and set the values */
      hapiBroadLagCritSecEnter ();
      for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
        {
          result =  hapiBroadPortDoubleVlanTagConfig(&dapiPortPtr->modeparm.lag.memberSet[i].usp,
                                                     dapiCmd,dapi_g);
          if (result != L7_SUCCESS)
            {
              hapiBroadLagCritSecExit ();
              return result;
            }
        }
      }

      hapiBroadLagCritSecExit ();
    }
    /*Check to see if the last port enabled for DTAG just got disabled*/
    if ((dtagPortCount == 1) &&
             (dapiCmd->cmdData.doubleVlanTagConfig.enable == L7_FALSE) &&
             (hapiPortPtr->dtag_mode == L7_TRUE))
    {
        rv = usl_bcmx_dvlan_mode_set(HAPI_BROAD_DTAG_MODE_NONE);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          return L7_FAILURE;
        }
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
      if(hapiBroadRoboVariantCheck() == __BROADCOM_53115_ID)
      {
         if (hapiBroadRoboDoubleVlanDenyRule(dapi_g,L7_FALSE)!= L7_SUCCESS)
         {
            SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'hapiBroadRoboDoubleVlanDenyRule' - FAILED\n",
                     __FILE__, __LINE__, __FUNCTION__);
         }
         if (hapiBroadDot1adProtoSnoopInstall(dapi_g, L7_FALSE)!= L7_SUCCESS)
         {
            SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'hapiBroadDot1adProtoSnoopInstall' - FAILED\n",
                     __FILE__, __LINE__, __FUNCTION__);
         }
      }
#endif
      dapi_g->system->dvlanEnable = L7_FALSE;
    }
    /*Only modify the counter in case the mode is different from the previous setting*/
    if(dapiCmd->cmdData.doubleVlanTagConfig.enable != hapiPortPtr->dtag_mode)
    {
        if(dapiCmd->cmdData.doubleVlanTagConfig.enable)
            dtagPortCount++;
        else
            dtagPortCount--;

    }

    if (result == L7_SUCCESS)
    {
      hapiPortPtr->dtag_mode = dapiCmd->cmdData.doubleVlanTagConfig.enable;
      hapiBroadDvlanEthertype = dapiCmd->cmdData.doubleVlanTagConfig.etherType;
    }
  } else
  {

    dapiCmd->cmdData.doubleVlanTagConfig.enable = hapiPortPtr->dtag_mode;
    dapiCmd->cmdData.doubleVlanTagConfig.etherType = hapiBroadDvlanEthertype;

    result = L7_SUCCESS;
  }

  return result;
}

/*********************************************************************
*
* @purpose  Used to enable/disable Double Vlan Tagging and set the ethertype
*           on the ports.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param   *dapiCmd       @b{(input)} The data to be set
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/

static L7_RC_t hapiBroadPortDoubleVlanTagConfig(DAPI_USP_t *usp,
                                                DAPI_INTF_MGMT_CMD_t *dapiCmd,
                                                DAPI_t *dapi_g)
{
  L7_RC_t                     result  = L7_SUCCESS;
  BROAD_PORT_t               *hapiPortPtr;
  L7_int32                    rc;
  usl_bcm_port_dtag_mode_t   dtagMode;
  usl_bcm_port_tpid_t        dtagTpid;
  L7_BOOL                  isTriumphFamily = L7_FALSE;
  bcm_chip_family_t        board_family;


  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (hapiBroadGetSystemBoardFamily(&board_family) == L7_SUCCESS)
  {
    if ( (board_family == BCM_FAMILY_TRIUMPH) ||
         (board_family == BCM_FAMILY_TRIUMPH2) ||
         (board_family == BCM_FAMILY_SCORPION) /* Scorpion has DVLAN capabilities similar to Triumph. */
        )
    {
      isTriumphFamily = L7_TRUE;
    }
  }


  do
  {
    if (dapiCmd->cmdData.doubleVlanTagConfig.enable != L7_TRUE)
    {
      /*
       * If the disable command is called, disable dvlan tagging on that port
       * Reset the mode to none if no ports are uplink and ethertype = 8100 (unconfig).
       * Else make it an access port.
       */

        if((dtagPortCount == 0))
           dtagMode = HAPI_BROAD_DTAG_MODE_NONE;
        else
        {
          dtagMode = HAPI_BROAD_DTAG_MODE_EXTERNAL;
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
          /* For Metro (NNI and UNI_S ports) this should be Internal. */
          if((dapiCmd->cmdData.doubleVlanTagConfig.dot1adInterfaceType == DOT1AD_INTFERFACE_TYPE_NNI ||
              dapiCmd->cmdData.doubleVlanTagConfig.dot1adInterfaceType == DOT1AD_INTFERFACE_TYPE_UNI_S) &&
              isTriumphFamily == L7_TRUE)
          {
            dtagMode = HAPI_BROAD_DTAG_MODE_INTERNAL;
          }
#endif
        }

    } else
    {
      /*
       *  Make it an uplink port
       */
        dtagMode = HAPI_BROAD_DTAG_MODE_INTERNAL;
    }

    rc = usl_bcmx_port_dtag_mode_set(hapiPortPtr->bcmx_lport, dtagMode);

    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'bcmx_port_dtag_mode_set' - FAILED : %d\n",
                     __FILE__, __LINE__, __FUNCTION__, rc);
      result = L7_FAILURE;
      break;
    }

    /*
     * Set the Ethertype (tpid) of the interface */

    /* This logic needs to be handled properly for Triumph
       only.
     */

    if ((dtagMode == HAPI_BROAD_DTAG_MODE_INTERNAL) || (isTriumphFamily == L7_TRUE))
    {
      dtagTpid = dapiCmd->cmdData.doubleVlanTagConfig.etherType;
      rc = usl_bcmx_port_tpid_set(hapiPortPtr->bcmx_lport, dtagTpid);

      if (L7_BCMX_OK(rc) != L7_TRUE)
      {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'bcmx_port_tpid_set' - FAILED : %d\n",
                     __FILE__, __LINE__, __FUNCTION__, rc);
        result = L7_FAILURE;
        break;
      }
    }

  } while ( 0 );

  return result;
}

/*********************************************************************
*
* @purpose  Used to enable/disable Double Vlan Tagging and set the ethertype
*           on the ports.
*
* @param   *lag_usp       @b{(input)} The USP of the port that is to be acted upon
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    added         @b{(input)} L7_TRUE if port added, L7_FALSE if port removed
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadDoubleVlanLagNotify(DAPI_USP_t *lag_usp,
                                     DAPI_USP_t *usp,
                                     L7_BOOL added,
                                     DAPI_t *dapi_g)
{
  BROAD_PORT_t      *hapiLagPortPtr, *hapiPortPtr;
  L7_BOOL            modifyEntry = L7_FALSE;
  L7_RC_t           rc = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    dapiCmd;

  hapiLagPortPtr = HAPI_PORT_GET(lag_usp,dapi_g);
  hapiPortPtr    = HAPI_PORT_GET(usp,dapi_g);


  memset(&dapiCmd,0,sizeof(dapiCmd));
  dapiCmd.cmdData.doubleVlanTagConfig.getOrSet  = DAPI_CMD_SET;
  dapiCmd.cmdData.doubleVlanTagConfig.etherType = hapiBroadDvlanEthertype;

  if (added == L7_TRUE)
  {
    if (hapiLagPortPtr->dtag_mode == L7_TRUE)
    {
      /* add the dvlan taggin info to the port */
      dapiCmd.cmdData.doubleVlanTagConfig.enable   = L7_TRUE;
      modifyEntry = L7_TRUE;
    } else if ((hapiPortPtr->dtag_mode == L7_TRUE) &&
               (hapiLagPortPtr->dtag_mode == L7_FALSE ))
    {
      /* remove the dvlan tagging from the port */
      dapiCmd.cmdData.doubleVlanTagConfig.enable   = L7_FALSE;
      modifyEntry = L7_TRUE;
    } else
    {
      modifyEntry = L7_FALSE;
    }
  } else
  {
    if (hapiPortPtr->dtag_mode == L7_TRUE)
    {
      /* add the dvlan taggin info to the port */
      dapiCmd.cmdData.doubleVlanTagConfig.enable   = L7_TRUE;
      modifyEntry = L7_TRUE;
    } else if ((hapiPortPtr->dtag_mode == L7_FALSE) &&
               (hapiLagPortPtr->dtag_mode == L7_TRUE))
    {
      /* remove the dvlan tagging from the port */
      dapiCmd.cmdData.doubleVlanTagConfig.enable   = L7_FALSE;
      modifyEntry = L7_TRUE;
    } else
    {
      modifyEntry = L7_FALSE;
    }
  }

  if (modifyEntry == L7_TRUE)
  {
    rc = hapiBroadPortDoubleVlanTagConfig(usp,&dapiCmd,dapi_g);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Used to configure the learning mode on a port.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    flags         @b{(input)} flags to define the learning mode
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  void
*
* @end
*
*********************************************************************/
void hapiBroadLearnSet(DAPI_USP_t *usp, L7_uint32 flags, DAPI_t *dapi_g)
{
  L7_int32                    rc;
  DAPI_PORT_t                *dapiPortPtr;
  BROAD_PORT_t               *hapiPortPtr;
  BROAD_PORT_t               *hapiLagMemberPortPtr;
  bcmx_lport_t                lport;
  L7_uint32                   i;
  usl_bcm_port_learn_mode_t  learnMode;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  lport       = hapiPortPtr->bcmx_lport;

  learnMode   = flags;

  /* assumes this function is only called w/ physical and LAG ports */
  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
  {
    rc = usl_bcmx_port_learn_set(lport, learnMode);
    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'bcmx_port_learn_set' - FAILED : %d\n",
                     __FILE__, __LINE__, __FUNCTION__, rc);
    }
  } else if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr))
  {
    hapiBroadLagCritSecEnter ();
    if (hapiPortPtr->hapiModeparm.lag.numMembers > 0)
    {
       for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
       {
         if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
         {
           hapiLagMemberPortPtr = 
             HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);

           lport = hapiLagMemberPortPtr->bcmx_lport;

           rc = usl_bcmx_port_learn_set(lport, learnMode);
          if (L7_BCMX_OK(rc) != L7_TRUE)
          {
            SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                          "\n%s %d: In %s call to 'bcmx_port_learn_set' - FAILED : %d\n",
                         __FILE__, __LINE__, __FUNCTION__, rc);
          }
        }
      }       
   
      usl_bcmx_trunk_learn_mode_set(usp->port, 
                                    hapiPortPtr->hapiModeparm.lag.tgid,
                                   (flags == BCM_PORT_LEARN_CPU) ? 1 : 0);
    }
    hapiBroadLagCritSecExit ();
  }

  return;
}

/*********************************************************************
*
* @purpose  Used to configure MAC locking on a port.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_MAC_LOCK_CONFIG
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.macLockConfig
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfMacLockConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;
  bcmx_lport_t            lport;
#ifdef L7_ROBO_SUPPORT
  int mode = 0, rv;
#endif
  if (dapiCmd->cmdData.macLockConfig.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  lport       = hapiPortPtr->bcmx_lport;
  if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE) && (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_FALSE))
  {
    return L7_FAILURE;
  }

  hapiPortPtr->locked = dapiCmd->cmdData.macLockConfig.lock;

  if (hapiPortPtr->l2FlushInProgress == L7_TRUE)
  {
    return result;
  }

  if ((hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED) ||
      (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr)))
  {
    if (dapiCmd->cmdData.macLockConfig.lock == L7_TRUE)
    {
#ifdef L7_ROBO_SUPPORT
      mode = (BCM_AUTH_MODE_AUTH |BCM_AUTH_SEC_SIMPLIFY_MODE);

      /* Always start with  uncontrolled */
      rv = bcmx_auth_mode_set (lport, BCM_AUTH_MODE_UNCONTROLLED);
      if (rv != BCM_E_NONE) {
        printf("[%s] Error setting AUTH mode flags 'Uncontrolled' for port %d rv %d\n",
                __FUNCTION__, lport, rv);
      }

      /* Set the BCM_AUTH_MODE_AUTH */
      rv = bcmx_auth_mode_set (lport, mode);
      if (rv != BCM_E_NONE) {
        printf("[%s] Error setting AUTH mode flags %d for port %d rv %d\n",
                __FUNCTION__, mode, lport, rv);
      }
#endif
      /* disable HW learning on this port */
      hapiBroadLearnSet(usp, BCM_PORT_LEARN_CPU, dapi_g);

    } else
    {
      /* enable learning on this port */
#ifdef L7_ROBO_SUPPORT
      mode = BCM_AUTH_MODE_UNCONTROLLED;
      rv = bcmx_auth_mode_set (lport, mode);
      if (rv != BCM_E_NONE && !SOC_IS_ROBO53115(hapiPortPtr->bcm_unit))
      {
        printf("[%s] Error setting auth mode flag %d for port %d",
                __FUNCTION__, mode, lport);
      }
#endif
       /* enable HW learning on this port if CP is not enabled */
      if (hapiPortPtr->cpEnabled == L7_FALSE)
      {
        if (hpcSoftwareLearningEnabled () == L7_TRUE)
          hapiBroadLearnSet(usp, (BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_FWD), dapi_g);
        else
          hapiBroadLearnSet(usp, (BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD), dapi_g);
      }
    }
  }


  return result;
}

/*********************************************************************
*
* @purpose  Used to configure LLDP on a physical port.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_LLDP_CONFIG
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.lldpConfig
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfLldpConfig(DAPI_USP_t *usp,
                                DAPI_CMD_t cmd,
                                void *data,
                                DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;

  if (dapiCmd->cmdData.lldpConfig.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* LLDP is only valid for physical ports */
  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  /* For broadcom the hardware has already been setup to
   * forward all multicast frames including 0180C200000e
   * to the CPU, so we don't need to configure the
   * hardware just set a value indicating the driver should
   * pass frames received on this port to the application.
   */

  if (dapiCmd->cmdData.lldpConfig.enable == L7_TRUE)
  {
    /* Set the mode in HAPI port struct so that it sends 802.1AB PDUs to the CPU */
    hapiPortPtr->hapiModeparm.physical.acceptLLDPDU = L7_TRUE;
  }
  else
  {
    /* Set the mode in HAPI port struct so that it drops 802.1AB PDUs */
    hapiPortPtr->hapiModeparm.physical.acceptLLDPDU = L7_FALSE;
  }

  return result;
}

/*********************************************************************
*
* @purpose Add a MAC address to the network processor's table
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_ADD
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.macAddressEntryAdd
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacAddressEntryAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                       result = L7_SUCCESS;
  DAPI_ADDR_MGMT_CMD_t         *dapiCmd = (DAPI_ADDR_MGMT_CMD_t*)data;
  DAPI_PORT_t                  *dapiPortPtr;
  BROAD_PORT_t                 *hapiPortPtr;
  DAPI_USP_t                    cpuUsp;
  bcmx_l2_addr_t                l2Addr;
  mac_addr_t                    macAddr;
  L7_int32                      rc=0;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (dapiCmd->cmdData.macAddressEntryAdd.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  /* Add this MAC Address to the L2 Forwarding Table stored in the NP. */
  memcpy(macAddr, dapiCmd->cmdData.macAddressEntryAdd.macAddr.addr, 6);

  memset(&l2Addr, 0, sizeof (bcmx_l2_addr_t));
  memcpy(l2Addr.mac, macAddr, sizeof (mac_addr_t));
  l2Addr.vid = dapiCmd->cmdData.macAddressEntryAdd.vlanID ;

  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    /* update l2Addr.flags to reflect that it is adding to a lag port */
    l2Addr.flags |= BCM_L2_TRUNK_MEMBER;
    if (hapiPortPtr->hapiModeparm.lag.numMembers > 0)
    {
      l2Addr.tgid  = hapiPortPtr->hapiModeparm.lag.tgid;
    }
    else
    {
      l2Addr.tgid  = hapiPortPtr->bcmx_lport;
    }
  }

  if (dapiCmd->cmdData.macAddressEntryAdd.flags == DAPI_ADDR_FLAG_SELF)
  {
    /* Discard the packet */
    l2Addr.flags = (BCM_L2_DISCARD_DST | BCM_L2_STATIC | BCM_L2_REPLACE_DYNAMIC);
    l2Addr.lport  = hapiPortPtr->bcmx_lport;
  } else if (dapiCmd->cmdData.macAddressEntryAdd.flags == DAPI_ADDR_FLAG_LEARNED)
  {
    /* add the individual mac addr */
    l2Addr.flags |= (BCM_L2_HIT | BCM_L2_SRC_HIT);
    l2Addr.lport  = hapiPortPtr->bcmx_lport;
  } else if (dapiCmd->cmdData.macAddressEntryAdd.flags == DAPI_ADDR_FLAG_STATIC)
  {
    /* add the individual mac addr */
#if defined(FEAT_METRO_CPE_V1_0)
    /* For this feature we should not compete for the DYNAMIC entries */
    l2Addr.flags |= (BCM_L2_STATIC);
#else
    l2Addr.flags |= (BCM_L2_STATIC | BCM_L2_REPLACE_DYNAMIC);
#endif

    l2Addr.lport  = hapiPortPtr->bcmx_lport;
  } else if ((dapiCmd->cmdData.macAddressEntryAdd.flags == DAPI_ADDR_FLAG_MANAGEMENT) ||
             (dapiCmd->cmdData.macAddressEntryAdd.flags == DAPI_ADDR_FLAG_L3_MANAGEMENT))
  {
    /* Configure management MAC address, to come to CPU */
    if (CPU_USP_GET(&cpuUsp) == L7_FAILURE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'CPU_USP_GET' - FAILED : %d\n",
                     __FILE__, __LINE__, __FUNCTION__, rc);
      return result;
    }
    hapiPortPtr = HAPI_PORT_GET(&cpuUsp, dapi_g);

    /* Set the L3 bit set for Router mac-addresses */
    if (dapiCmd->cmdData.macAddressEntryAdd.flags == DAPI_ADDR_FLAG_L3_MANAGEMENT)
    {
      l2Addr.flags = (BCM_L2_STATIC | BCM_L2_L3LOOKUP | BCM_L2_REPLACE_DYNAMIC | BCM_L2_COPY_TO_CPU);
    }
    else
    {
      l2Addr.flags = (BCM_L2_STATIC | BCM_L2_COPY_TO_CPU | BCM_L2_REPLACE_DYNAMIC);
    }



    l2Addr.lport = hapiPortPtr->bcmx_lport;
  }

  /* Add MAC addr to hw ARL table */
  rc = usl_bcmx_l2_addr_add(&l2Addr, L7_NULL);
  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    result = L7_FAILURE;

/* In metro feature Failure for static entries are
   also pretty normal :) */
#ifndef FEAT_METRO_CPE_V1_0
    /* Failing to add dynamic entries is pretty normal, so print an error
    ** only for the static entries.
    */
    if ((l2Addr.flags & BCM_L2_STATIC) != 0)
    {
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'bcmx_l2_addr_add' - FAILED : %d\n",
                     __FILE__, __LINE__, __FUNCTION__, rc);
    }
#endif
    return result;
  } else
  {
    hapiBroadL3UpdateMacLearn(l2Addr.mac, l2Addr.vid, usp, dapi_g);
  }

  return result;
}



/*********************************************************************
*
* @purpose Add a MAC address to the network processor's table
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_MODIFY
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.macAddressEntryModify
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacAddressEntryModify(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 result = L7_SUCCESS;
  DAPI_ADDR_MGMT_CMD_t         *dapiCmd = (DAPI_ADDR_MGMT_CMD_t*)data;
  DAPI_PORT_t                  *dapiPortPtr;
  BROAD_PORT_t                 *hapiPortPtr;
  DAPI_USP_t                    cpuUsp;
  bcmx_l2_addr_t                l2Addr;
  mac_addr_t                    macAddr;
  L7_int32                      rc=0;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (dapiCmd->cmdData.macAddressEntryModify.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  /* Add this MAC Address to the L2 Forwarding Table stored in the NP. */
  memcpy(macAddr, dapiCmd->cmdData.macAddressEntryModify.macAddr.addr, 6);

  memset(&l2Addr, 0, sizeof (bcmx_l2_addr_t));
  memcpy(l2Addr.mac, macAddr, sizeof (mac_addr_t));
  l2Addr.vid = dapiCmd->cmdData.macAddressEntryModify.vlanID ;

  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    /* update l2Addr.flags to reflect that it is adding to a lag member port */
    l2Addr.flags |= BCM_L2_TRUNK_MEMBER;
    l2Addr.tgid  = hapiPortPtr->bcmx_lport;
  }

  if (dapiCmd->cmdData.macAddressEntryModify.flags == DAPI_ADDR_FLAG_SELF)
  {
    /* Discard the packet */
    l2Addr.flags = (BCM_L2_DISCARD_DST | BCM_L2_STATIC | BCM_L2_REPLACE_DYNAMIC);
    l2Addr.lport  = hapiPortPtr->bcmx_lport;
  } else if (dapiCmd->cmdData.macAddressEntryModify.flags == DAPI_ADDR_FLAG_LEARNED)
  {
    /* add the individual mac addr */
    l2Addr.lport  = hapiPortPtr->bcmx_lport;
  } else if (dapiCmd->cmdData.macAddressEntryModify.flags == DAPI_ADDR_FLAG_STATIC)
  {
    /* add the individual mac addr */
    l2Addr.flags |= (BCM_L2_STATIC | BCM_L2_REPLACE_DYNAMIC);
    l2Addr.lport  = hapiPortPtr->bcmx_lport;
  } else if ((dapiCmd->cmdData.macAddressEntryModify.flags == DAPI_ADDR_FLAG_MANAGEMENT) ||
             (dapiCmd->cmdData.macAddressEntryModify.flags == DAPI_ADDR_FLAG_L3_MANAGEMENT))
  {
    /* Configure management MAC address, to come to CPU */
    if (CPU_USP_GET(&cpuUsp) == L7_FAILURE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'CPU_USP_GET' - FAILED\n",
                     __FILE__, __LINE__, __FUNCTION__);
      return result;
    }
    hapiPortPtr = HAPI_PORT_GET(&cpuUsp, dapi_g);


    /* Set the L3 bit set for Router mac-addresses */
    if (dapiCmd->cmdData.macAddressEntryModify.flags == DAPI_ADDR_FLAG_L3_MANAGEMENT)
    {
      l2Addr.flags = (BCM_L2_STATIC | BCM_L2_L3LOOKUP | BCM_L2_REPLACE_DYNAMIC | BCM_L2_COPY_TO_CPU);
    }
    else
    {
      l2Addr.flags = (BCM_L2_STATIC | BCM_L2_COPY_TO_CPU | BCM_L2_REPLACE_DYNAMIC);
    }

    l2Addr.lport = hapiPortPtr->bcmx_lport;
  }

  rc = usl_bcmx_l2_addr_add(&l2Addr, L7_NULL);
  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'bcmx_l2_addr_add' - FAILED : %d\n",
                   __FILE__, __LINE__, __FUNCTION__, rc);
    return result;
  } else
  {
    hapiBroadL3UpdateMacLearn(l2Addr.mac, l2Addr.vid, usp, dapi_g);
  }

  return result;

}



/*********************************************************************
*
* @purpose Add a MAC address to the network processor's table
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_DELETE
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.macAddressEntryDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacAddressEntryDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 result  = L7_SUCCESS;
  DAPI_ADDR_MGMT_CMD_t         *dapiCmd = (DAPI_ADDR_MGMT_CMD_t*)data;
  bcmx_l2_addr_t                 l2Addr;
  mac_addr_t                    macAddr;
  L7_int32                      rc=0;

  if (dapiCmd->cmdData.macAddressEntryDelete.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  /* The array macAddr now contains the MAC Address to be deleted. */
  memcpy(macAddr, dapiCmd->cmdData.macAddressEntryDelete.macAddr.addr, 6);

  memset(&l2Addr,0,sizeof(l2Addr));
  rc = bcmx_l2_addr_get(macAddr, dapiCmd->cmdData.macAddressEntryDelete.vlanID, &l2Addr, L7_NULL);

  if (rc == BCM_E_NOT_FOUND)
  {
    /* Entry is not present in hw table, so just return success */
    return result;
  } else if (rc == BCM_E_NONE)
  {
    /* Delete this MAC Address from the L2 Forwarding Table of BCOM ARL. */
    rc = usl_bcmx_l2_addr_delete(macAddr, dapiCmd->cmdData.macAddressEntryDelete.vlanID);
    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'usl_bcmx_l2_addr_remove' - FAILED : %d\n",
                     __FILE__, __LINE__, __FUNCTION__, rc);
      return result;
    }
  }

  return result;
}

///* PTin added: MAC learning */
///*********************************************************************
//*
//* @purpose Set MAC Learn Mode
//*
//* @param   DAPI_USP_t *usp    - needs to be a valid usp
//* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_SET_LEARN_MODE
//* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.portAddressSetLearnMode
//* @param   DAPI_t     *dapi_g - the driver object
//*
//* @returns L7_RC_t result
//*
//* @end
//*
//*********************************************************************/
//L7_RC_t hapiBroadAddrSetLearnMode(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
//{
//  L7_RC_t               result  = L7_SUCCESS;
//  DAPI_ADDR_MGMT_CMD_t *dapiCmd = (DAPI_ADDR_MGMT_CMD_t*)data;
//  BROAD_PORT_t         *hapiPortPtr;
//  L7_uint32             lport, port;
//  L7_int32              learnEnabled;
//
//  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);
//  lport = hapiPortPtr->bcmx_lport;
//
//  /* Port should be local */
//  if (!BCMX_LPORT_IS_PHYSICAL(lport))
//    return L7_FAILURE;
//
//  /* Get local port */
//  port = BCMX_LPORT_BCM_PORT(lport);
//
//  learnEnabled = (dapiCmd->cmdData.portAddressSetLearnMode.learn_enabled & 1);
//
//  /* The learn mode should be SET, if necessary with the value passed down
//   * from the DTL call */
//  if (dapiCmd->cmdData.portAddressSetLearnMode.getOrSet == DAPI_CMD_SET)
//  {
//    /* Apply new state, only if new state is different */
//    if (ptin_learnEnabled[port]!=learnEnabled)
//    {
//      /* Save new state
//       * It should be done before changing effectively the state,
//       * because it depends on this variable  */
//      ptin_learnEnabled[port] = learnEnabled;
//
//      /* Only update learn state, if there isn't any flush operation in course */
//      if (!(hapiPortPtr->l2FlushInProgress))
//      {
//        if ( learnEnabled  )
//        {
//          /* Activate learning */
//          hapiBroadLearnSet(usp, (BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD), dapi_g);
//        }
//        else
//        {
//          /* Deactivate learning, and clear adresses related to this port */
//          hapiBroadLearnSet(usp, BCM_PORT_LEARN_FWD, dapi_g);
//          usl_bcmx_l2_addr_remove_by_port(lport,L7_FALSE);
//        }
//      }
//    }
//  }
//  else
//  {
//    dapiCmd->cmdData.portAddressSetLearnMode.learn_enabled = ptin_learnEnabled[port];
//  }
//
//  return result;
//}
//* PTin end */


/*********************************************************************
*
* @purpose Set the aging time
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_AGING_TIME
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.agingTime
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   Broadcom does not support per-VLAN aging, so this function
*          will only work on commands for VLAN 1.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrAgingTime(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t         result  = L7_SUCCESS;
  DAPI_ADDR_MGMT_CMD_t *dapiCmd = (DAPI_ADDR_MGMT_CMD_t*)data;
  BROAD_PORT_t         *hapiPortPtr;
  L7_int32              ageTime;
  L7_int32              rc=0;
  BROAD_SYSTEM_t        *hapiSystemPtr;
  static L7_uint32      last_age_time = 0;

  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);
  ageTime     = dapiCmd->cmdData.agingTime.agingTime;
  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  /* The aging time for the NP should be set, if necessary with the value passed
     down from the DTL call. */
  if (dapiCmd->cmdData.agingTime.getOrSet == DAPI_CMD_SET)
  {
    /* only allow the application to set the age time for VLAN 1, which will
       affect the age time for all VLANs */
    if (dapiCmd->cmdData.agingTime.vlanID != FD_SIM_DEFAULT_MGMT_VLAN_ID)
    {
      return L7_SUCCESS;
    }

    rc = usl_bcmx_l2_age_timer_set(ageTime);
    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'bcmx_l2_age_timer_set' - FAILED : %d\n",
                     __FILE__, __LINE__, __FUNCTION__, rc);
      return result;
    }
    last_age_time = ageTime;
  } else
  {
    dapiCmd->cmdData.agingTime.agingTime = last_age_time;
  }

  return result;
}


/*********************************************************************
*
* @purpose Set the aging time
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_FLUSH
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.agingTime
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   Broadcom does not support per-VLAN aging, so this function
*          will only work on commands for VLAN 1.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrFlush(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t               result  = L7_SUCCESS;
  DAPI_PORT_t          *dapiPortPtr;
  BROAD_PORT_t         *hapiPortPtr;
  BROAD_L2ADDR_FLUSH_t   l2addr_port;


  /* Get the port info */
  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  l2addr_port.vlanID = 0;
  l2addr_port.bcmx_lport  = 0;
  l2addr_port.tgid = 0;
  l2addr_port.flushtype = BROAD_FLUSH_BY_PORT;

  memset(l2addr_port.mac.addr, 0, L7_ENET_MAC_ADDR_LEN);

  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    if (hapiPortPtr->hapiModeparm.lag.tgid == -1)
    {
      if (hapiPortPtr->hapiModeparm.lag.lastValidTgid == -1)
      {
      /* Trunk doesn't exist and never existed.
      */
      return L7_SUCCESS;
      }
      l2addr_port.tgid = hapiPortPtr->hapiModeparm.lag.lastValidTgid;
    } else
    {
      l2addr_port.tgid = hapiPortPtr->hapiModeparm.lag.tgid;
    }
    l2addr_port.port_is_lag = L7_TRUE;
  } else
  {
    l2addr_port.bcmx_lport = hapiPortPtr->bcmx_lport;
    l2addr_port.port_is_lag = L7_FALSE;
  }

  hapiBroadL2FlushRequest(l2addr_port);

  return result;
}

/*********************************************************************
*
* @purpose Flush the VLAN specific MAC Table dynamic entries.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_FLUSH
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.agingTime
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrFlushByVlan(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t               result  = L7_SUCCESS;
  DAPI_ADDR_MGMT_CMD_t *dapiCmd = (DAPI_ADDR_MGMT_CMD_t*)data;
  BROAD_PORT_t         *hapiPortPtr;
  BROAD_L2ADDR_FLUSH_t  l2addr_vlan;

  /* Get the port info */
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* Fill in the structure */
  l2addr_vlan.bcmx_lport = hapiPortPtr->bcmx_lport;
  l2addr_vlan.vlanID = dapiCmd->cmdData.agingTime.vlanID;
  l2addr_vlan.flushtype = BROAD_FLUSH_BY_VLAN;
  l2addr_vlan.port_is_lag = L7_FALSE;
  l2addr_vlan.tgid        = 0;
  memset(l2addr_vlan.mac.addr, 0, L7_ENET_MAC_ADDR_LEN);

  /* Send a message to L2 address flushing task with the vlan info */
  hapiBroadL2FlushRequest(l2addr_vlan);

  return result;
}

/*********************************************************************
*
* @purpose Flush the MAC specific MAC Table dynamic entries.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_FLUSH
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.agingTime
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrFlushByMac(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t               result  = L7_SUCCESS;
  DAPI_ADDR_MGMT_CMD_t *dapiCmd = (DAPI_ADDR_MGMT_CMD_t*)data;
  BROAD_PORT_t         *hapiPortPtr;
  BROAD_L2ADDR_FLUSH_t  l2addr_mac;

  /* Get the port info */
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* Fill in the structure */
  l2addr_mac.bcmx_lport = hapiPortPtr->bcmx_lport;
  l2addr_mac.vlanID = 0;
  l2addr_mac.flushtype = BROAD_FLUSH_BY_MAC;
  l2addr_mac.port_is_lag = L7_FALSE;
  l2addr_mac.tgid        = 0;
  memcpy(l2addr_mac.mac.addr,
                 dapiCmd->cmdData.portAddressFlushMac.macAddr.addr,
                 L7_ENET_MAC_ADDR_LEN);

  /* Send a message to L2 address flushing task with the vlan info */
  hapiBroadL2FlushRequest(l2addr_mac);

  return result;
}

/*********************************************************************
*
* @purpose Flash all MAC address for all Ports
*
* @param   DAPI_USP_t *usp    - not used in this function
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_FLUSH_ALL_NAME
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.l2FlushAll
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrFlushAll(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  DAPI_SYSTEM_CMD_t         *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  L7_RC_t result = L7_SUCCESS;

  if (dapiCmd->cmdData.l2FlushAll.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  usl_bcmx_l2_addr_remove_all();

  return result;
}

/*********************************************************************
*
* @purpose Synchronize all the dynamic L2 entries w/ the FDB application
*
* @param   DAPI_USP_t *usp    - not used in this function
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_SYNC
* @param   void       *data   - not used
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrSync(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  usl_bcmx_l2_addr_sync();

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Create a logical interface
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LOGICAL_INTF_CREATE
* @param   void       *data   - DAPI_LOGICAL_INTF_MGMT_CMD_t.cmdData.logicalIntfCreate
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   The implementation of this routine currently only supports
* @notes   the creation of a VLAN interface that is used for routing.
* @notes   FUTURE_FUNC : Likely to be enhanced in the future
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrLogicalIntfCreate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{

  L7_RC_t                  result       = L7_SUCCESS;
  DAPI_LOGICAL_INTF_MGMT_CMD_t  *dapiCmd      = (DAPI_LOGICAL_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t                   *dapiPortPtr;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  /* verify port modes are both logical... */
  if (dapiPortPtr->type != dapiCmd->cmdData.logicalIntfCreate.type)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid mode\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  dapiPortPtr->isLogicalIntfCreated = L7_TRUE;

  return result;

}



/*********************************************************************
*
* @purpose Delete a logical interface
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LOGICAL_INTF_DELETE
* @param   void       *data   - DAPI_LOGICAL_INTF_MGMT_CMD_t.cmdData.logicalIntfDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrLogicalIntfDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{

  L7_RC_t                  result       = L7_SUCCESS;
  DAPI_PORT_t                   *dapiPortPtr;

  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);

  if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) ||
      (IS_PORT_TYPE_CPU(dapiPortPtr)      == L7_TRUE))
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid mode\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  dapiPortPtr->isLogicalIntfCreated = L7_FALSE;

  return result;

}


/*********************************************************************
*
* @purpose Acquire the Broad MAC Filter Table
*
* @param   void
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
void hapiBroadMacFilterLock(void)
{
  (void)osapiSemaTake(broadMacFilterSema, L7_WAIT_FOREVER);
}

/*********************************************************************
*
* @purpose Release the Broad MAC Filter Table
*
* @param   void
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
void hapiBroadMacFilterUnlock(void)
{
  (void)osapiSemaGive(broadMacFilterSema);
}

/*********************************************************************
*
* @purpose Find matching entry in Static MAC database
*
* @param   void
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacFilterFind(L7_enetMacAddr_t macAddr, L7_ushort16 vlanID, L7_uint32 *idx)
{
  L7_uint32 i;

  /* check to see if this entry already exists */
  for (i = 0; i < L7_MAX_FDB_STATIC_FILTER_ENTRIES; i++)
  {
    if (L7_TRUE == broadMacFilters[i].inUse)
    {
      if ((memcmp(macAddr.addr, broadMacFilters[i].mac.addr, sizeof(L7_enetMacAddr_t)) == 0) &&
          (vlanID == broadMacFilters[i].vlanID))
      {
        *idx = i;
        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Find free entry in Static MAC database
*
* @param   void
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacFilterAlloc(L7_uint32 *idx)
{
  L7_uint32  i;

  /* find a free entry in the database */
  for (i = 0; i < L7_MAX_FDB_STATIC_FILTER_ENTRIES; i++)
  {
    if (L7_FALSE == broadMacFilters[i].inUse)
    {
      *idx = i;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Installs Static MAC filter at given index
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacFilterInstall(L7_uint32 idx, DAPI_t *dapi_g)
{
  L7_RC_t              result = L7_SUCCESS;
  BROAD_MAC_FILTERS_t *filterPtr;
  L7_uint32            i;

  filterPtr = &broadMacFilters[idx];

  /* apply policy to all ports by default */
  if (hapiBroadPolicyApplyToAll(filterPtr->policyId) != L7_SUCCESS)
    return L7_FAILURE;

  /* remove the policy from specific source ports */
  for (i = 0; i < filterPtr->numOfSrcUsp; i++)
  {
    if (isValidUsp(&filterPtr->srcUsp[i], dapi_g))
    {
      DAPI_PORT_t  *dapiPortPtr;
      BROAD_PORT_t *hapiPortPtr;

      dapiPortPtr = DAPI_PORT_GET(&filterPtr->srcUsp[i], dapi_g);
      hapiPortPtr = HAPI_PORT_GET(&filterPtr->srcUsp[i], dapi_g);

      if (L7_TRUE == hapiPortPtr->port_is_lag)
      {
        int               lm;
        DAPI_LAG_ENTRY_t *lagMemberSet;
        BROAD_PORT_t     *lagMemberPtr;

        lagMemberSet = dapiPortPtr->modeparm.lag.memberSet;

        /* remove policy from each LAG member */
        for (lm = 0; lm < L7_MAX_MEMBERS_PER_LAG; lm++)
        {
          if (L7_TRUE == lagMemberSet[lm].inUse)
          {
            lagMemberPtr = HAPI_PORT_GET(&lagMemberSet[lm].usp, dapi_g);
            if (hapiBroadPolicyRemoveFromIface(filterPtr->policyId, lagMemberPtr->bcmx_lport) != L7_SUCCESS)
              result = L7_FAILURE;
          }
        }
      } else
      {
        /* remove policy from phy port */
        if (hapiBroadPolicyRemoveFromIface(filterPtr->policyId, hapiPortPtr->bcmx_lport) != L7_SUCCESS)
          result = L7_FAILURE;
      }
    }
  }

  return result;
}

/*********************************************************************
*
* @purpose Adds a static filter to the forwarding engines
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_MAC_FILTER_ADD
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.macFilterAdd
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   We do not support specification of the destination ports
*          on Broadcom.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacFilterAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 result       = L7_SUCCESS;
  DAPI_ADDR_MGMT_CMD_t   *dapiCmd      = (DAPI_ADDR_MGMT_CMD_t*)data;
  L7_enetMacAddr_t        macAddr;
  L7_ushort16             vlanID;
  L7_uint32               idx;
  L7_uint32               srcPort;

  if (dapiCmd->cmdData.macFilterAdd.getOrSet != DAPI_CMD_SET)
    return L7_FAILURE;

  /* Acquire the broadMacFilters[] database */
  hapiBroadMacFilterLock();

  memcpy(macAddr.addr, dapiCmd->cmdData.macFilterAdd.macAddr.addr, sizeof(macAddr));
  vlanID = dapiCmd->cmdData.macFilterAdd.vlanID;

  if (hapiBroadAddrMacFilterFind(macAddr, vlanID, &idx) != L7_SUCCESS)
  {
    BROAD_POLICY_t      policyId;
    BROAD_POLICY_RULE_t ruleId;
    L7_uchar8           exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
      FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};

    /* create a new policy */
    hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
    hapiBroadPolicyRuleAdd(&ruleId);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, macAddr.addr, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8*)&vlanID, exact_match);
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0); /* matching rules should drop the packet */
    if (hapiBroadPolicyCommit(&policyId) != L7_SUCCESS)
    {
      hapiBroadPolicyDelete(policyId);
      hapiBroadMacFilterUnlock();
      return L7_FAILURE;
    }

    if (hapiBroadAddrMacFilterAlloc(&idx) != L7_SUCCESS)
    {
      hapiBroadPolicyDelete(policyId);
      hapiBroadMacFilterUnlock();
      return L7_FAILURE;
    }

    /* store policy in database */
    broadMacFilters[idx].inUse    = L7_TRUE;
    broadMacFilters[idx].policyId = policyId;
    broadMacFilters[idx].vlanID   = vlanID;
    memcpy(broadMacFilters[idx].mac.addr, macAddr.addr, sizeof(macAddr));
  }

  /* maintain list of valid source ports */
  broadMacFilters[idx].numOfSrcUsp = 0;
  for (srcPort = 0; srcPort < dapiCmd->cmdData.macFilterAdd.numOfSrcPortEntries; srcPort++)
  {
    memcpy(&broadMacFilters[idx].srcUsp[broadMacFilters[idx].numOfSrcUsp],
           &dapiCmd->cmdData.macFilterAdd.srcPorts[srcPort],
           sizeof(DAPI_USP_t));

    broadMacFilters[idx].numOfSrcUsp++;
  }

  /* install the policy on specific ports */
  if (hapiBroadAddrMacFilterInstall(idx, dapi_g) != L7_SUCCESS)
    result = L7_FAILURE;

  if (L7_SUCCESS != result)
  {
    (void)hapiBroadPolicyDelete(broadMacFilters[idx].policyId);
    broadMacFilters[idx].inUse = L7_FALSE;
  }

  hapiBroadMacFilterUnlock();

  return result;
}

/*********************************************************************
*
* @purpose Removes a static filter from the forwarding engines
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_MAC_FILTER_DELETE
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.macFilterDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacFilterDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result        = L7_SUCCESS;
  DAPI_ADDR_MGMT_CMD_t    *dapiCmd      = (DAPI_ADDR_MGMT_CMD_t*)data;
  L7_enetMacAddr_t         macAddr;
  L7_ushort16              vlanID;
  L7_uint32                idx;

  if (dapiCmd->cmdData.macFilterAdd.getOrSet != DAPI_CMD_SET)
    return L7_FAILURE;

  /* Acquire the broadMacFilters[] database */
  hapiBroadMacFilterLock();

  memcpy(macAddr.addr, &dapiCmd->cmdData.macFilterAdd.macAddr.addr, sizeof(macAddr));
  vlanID = dapiCmd->cmdData.macFilterAdd.vlanID;

  if (hapiBroadAddrMacFilterFind(macAddr, vlanID, &idx) == L7_SUCCESS)
  {
    if (hapiBroadPolicyDelete(broadMacFilters[idx].policyId) != L7_SUCCESS)
      result = L7_FAILURE;

    memset(&broadMacFilters[idx], 0, sizeof(broadMacFilters[idx]));
  } else
  {
    result = L7_FAILURE;   /* not found */
  }

  hapiBroadMacFilterUnlock();

  return result;
}

/*********************************************************************
*
* @purpose Apply the MAC filter configuration
*
* @param   void
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacFilterConfigApply(DAPI_t *dapi_g)
{
  L7_RC_t        result = L7_SUCCESS;
  L7_uint32      i;

  /* Acquire the broadMacFilters[] database */
  hapiBroadMacFilterLock();

  /* Mac Filters */
  for (i = 0; i < L7_MAX_FDB_STATIC_FILTER_ENTRIES; i++)
  {
    if (broadMacFilters[i].inUse == L7_TRUE)
    {
      if (hapiBroadAddrMacFilterInstall(i, dapi_g) != L7_SUCCESS)
        result = L7_FAILURE;
    }
  }

  /* release the lock */
  hapiBroadMacFilterUnlock();

  return result;
}

/*********************************************************************
*
* @purpose Notify MAC filters of a LAG modification
*
* @param   void
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadMacFilterLagModifiedNotify(DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  L7_RC_t    result = L7_SUCCESS;
  L7_uint32  i, j;

  /* Acquire the broadMacFilters[] database */
  hapiBroadMacFilterLock();

  /* loop through all of the MAC filters to see if any apply to this iface */
  for (i = 0; i < L7_MAX_FDB_STATIC_FILTER_ENTRIES; i++)
  {
    if (broadMacFilters[i].inUse == L7_TRUE)
    {
      for (j = 0; j < L7_FILTER_MAX_INTF; j++ )
      {
        if (memcmp(&broadMacFilters[i].srcUsp[j], usp, sizeof(DAPI_USP_t)) == 0)
        {
          hapiBroadAddrMacFilterInstall(i, dapi_g);
        }
      }
    }
  }

  /* release the lock before calling the add */
  hapiBroadMacFilterUnlock();

  return result;
}

/*********************************************************************
*
* @purpose MAC address learn/age callback to DAPI
*
* @param int            unit
* @param bcmx_l2_addr_t *bcmx_l2_addr
* @param int            insert
* @param DAPI_t*        dapi_g
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadAddrMacUpdate(void *bcmx_l2, int insert, DAPI_t *dapi_g)
{
  bcmx_l2_addr_t *bcmx_l2_addr = bcmx_l2;

  /* intercept addresses that are not really learned */
  if (bcmx_l2_addr->mac[0] & 0x01)
  {
    return;
  }
  if (bcmx_l2_addr->vid > L7_PLATFORM_MAX_VLAN_ID)
  {
    return;
  }

  if ((bcmx_l2_addr->flags & BCM_L2_L3LOOKUP) ||
      (bcmx_l2_addr->flags & BCM_L2_STATIC))
  {
    return;
  }

  if (insert)
  {
    /* Ignore inserts if software learning is enabled.
    */
    if (hpcSoftwareLearningEnabled() == L7_TRUE)
    {
      return;
    }

    hapiBroadAddrMacUpdateLearn(bcmx_l2_addr, dapi_g);
  } else
  {
    hapiBroadAddrMacUpdateAge(bcmx_l2_addr, dapi_g);
  }

  return;

}
/*********************************************************************
*
* @purpose Convert trunk group ID into USP.
*
* @param   tgid         - Broadcom driver trunk group identifier.
* @param   DAPI_USP_t      *usp      - Fastpath USP
* @param   DAPI_t          *dapi_g   - the driver object
*
* @returns L7_RC_t L7_SUCCESS - USP found.
* @returns L7_RC_t L7_FAILURE - USP does not exist for this trunk.
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadTgidToUspConvert(L7_uint32 tgid, DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  DAPI_USP_t lagUsp;
  BROAD_PORT_t          *hapiLagPortPtr;


  lagUsp.unit = L7_LOGICAL_UNIT;
  lagUsp.slot = L7_LAG_SLOT_NUM;

  for (lagUsp.port = 0; lagUsp.port < L7_MAX_NUM_LAG_INTF; lagUsp.port++)
  {
    hapiLagPortPtr = HAPI_PORT_GET(&lagUsp,dapi_g);

    if (hapiLagPortPtr->hapiModeparm.lag.tgid == tgid)
    {
      memcpy (usp, &lagUsp, sizeof (DAPI_USP_t));
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}
/*************************************************************************
*
*
* @purpose Convert Physical port to Lag USP it is part of
*
*
*************************************************************************/
void hapiBroadLportToTgidUspConvert(L7_uint32 lport, DAPI_USP_t *usp, DAPI_t *dapi_g)
{

  DAPI_USP_t lagUsp;
  BROAD_PORT_t          *hapiLagMemberPortPtr;
  bcmx_uport_t           uport;
  uport = BCMX_UPORT_GET(lport);

  HAPI_BROAD_UPORT_TO_USP(uport,usp);
  hapiLagMemberPortPtr = HAPI_PORT_GET(usp,dapi_g);
  if (hapiLagMemberPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
  {
    lagUsp= hapiLagMemberPortPtr->hapiModeparm.physical.lagUsp;
     memcpy (usp, &lagUsp, sizeof (DAPI_USP_t));
  }
   return;
}

/*********************************************************************
*
* @purpose Task for learning MACs from incoming frames.
*
* @returns L7_RC_t result
*
* @notes   The reason we need this task is because the hapiBroadAddrMacpdateLearn
*          function manipulates the MAC address table in the hardware. This can
*          take on the order of seconds when the system is busy and when dual
*          hashing needs to be done for the new entry.
*          Frame processing can't be blocked while the hardware MAC table is accessed
*          because it would cause spanning tree failures and stack failures in a
*          stacking environment.
*
* @end
*
*********************************************************************/
void hapiBroadAddrMacFrameAsyncLearn(L7_uint32 numArgs, DAPI_t *dapi_g)
{
    bcmx_l2_addr_t bcmx_l2_addr;

    do {
        if (osapiMessageReceive(hapiFrameLearnAsyncQueue, (void *)&bcmx_l2_addr,
                                sizeof(bcmx_l2_addr_t),
                                L7_WAIT_FOREVER) != L7_SUCCESS)
        {
            LOG_ERROR (0);
        }

        hapiBroadAddrMacUpdateLearn(&bcmx_l2_addr, dapi_g);
    } while (1);
}

/*********************************************************************
*
* @purpose Create the Async Learning Task.
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
static void hapiBroadAddrMacFrameAsyncLearnCreate(DAPI_t *dapi_g)
{
    /* The learn queue is not very big. If the queue fills up then the
    ** learning doesn't take place until the next frame.
    */
    hapiFrameLearnAsyncQueue = (void *)osapiMsgQueueCreate("hapiFrameAsyncLearnQ",
                                                        100,
                                                        sizeof(bcmx_l2_addr_t));
    if (hapiFrameLearnAsyncQueue == L7_NULLPTR)
    {
        LOG_ERROR (0);
    }

    if (osapiTaskCreate("hapiBroadAddrMacFrameAsyncLearn",
                        hapiBroadAddrMacFrameAsyncLearn,
                        1, dapi_g, (1024*8),
                        L7_DEFAULT_TASK_PRIORITY,
                        L7_DEFAULT_TASK_SLICE) == L7_ERROR)
    {
      LOG_ERROR(0);
    }
}

/*********************************************************************
*
* @purpose MAC address learn from incoming frame.
*
* @param bcm_pkt_t*     bcm_pkt - Pointer to the packet.
* @param DAPI_t*        dapi_g
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadAddrMacFrameLearn(bcm_pkt_t *bcm_pkt, DAPI_t *dapi_g)
{
  bcmx_l2_addr_t bcmx_l2_addr;
  L7_ushort16    vid;
  L7_8021QTag_t       *vlanTagPtr;
  static L7_BOOL async_task_created = L7_FALSE;

  if (async_task_created == L7_FALSE)
  {
      hapiBroadAddrMacFrameAsyncLearnCreate(dapi_g);
      async_task_created = L7_TRUE;
  }

  memset (&bcmx_l2_addr, 0, sizeof (bcmx_l2_addr_t));

  memcpy (bcmx_l2_addr.mac, &bcm_pkt->pkt_data->data[6], 6);

  /* parse vlan from frame */
  vlanTagPtr = (L7_8021QTag_t*) &bcm_pkt->pkt_data->data[12];

  if (osapiNtohs(vlanTagPtr->tpid) == L7_ETYPE_8021Q)
    vid = osapiNtohs(vlanTagPtr->tci) & L7_VLAN_TAG_VLAN_ID_MASK;
  else
    vid = 1;

  bcmx_l2_addr.vid = vid;

  /* need to set the NATIVE flag, since we will only learn native */
  bcmx_l2_addr.flags = BCM_L2_NATIVE;

  if (bcm_pkt->flags & BCM_PKT_F_TRUNK)
  {
    /* The frame is received on a trunk.
    */
    bcmx_l2_addr.tgid = bcm_pkt->src_trunk;
    bcmx_l2_addr.flags |= BCM_L2_TRUNK_MEMBER;

    /* Fake out the lport. The lport is used by bcmx to retrieve the souce module.
    */
    bcmx_l2_addr.lport = bcmx_modid_port_to_lport (bcm_pkt->src_mod, 0);
  }
  else if (BCM_GPORT_IS_WLAN_PORT(bcm_pkt->src_gport))
  {
    /*bcmx_l2_addr.lport = bcm_pkt->src_gport;*/
    bcmx_l2_addr.lport = bcm_pkt->src_gport;
  }
  else
  {
    bcmx_l2_addr.lport = bcmx_modid_port_to_lport(bcm_pkt->src_mod, bcm_pkt->src_port);
  }

  (void)osapiMessageSend (hapiFrameLearnAsyncQueue, (void*)&bcmx_l2_addr,
                          sizeof (bcmx_l2_addr_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  return;

}

/*********************************************************************
*
* @purpose Handle Learn indication from vendor driver
*
* @param int            unit
* @param bcm_l2_addr_t *l2addr
* @param DAPI_t*        dapi_g
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/

static int ptin_l2_addr_add = 1;
void ptin_l2_addr_add_enable(int enable)
{
  ptin_l2_addr_add = enable & 1;
}

void hapiBroadAddrMacUpdateLearn(bcmx_l2_addr_t *bcmx_l2_addr, DAPI_t *dapi_g)
{
  DAPI_ADDR_MGMT_CMD_t   macAddressInfo;
  DAPI_USP_t             usp;
  bcmx_uport_t           uport;
  DAPI_USP_t     cpuUsp;
  L7_RC_t rc1;
  L7_uint32     retry_count;
  int rv;

  if (CPU_USP_GET(&cpuUsp) == L7_FAILURE)
  {
    /* If CPU card is not plugged in then exit.
    */
    return;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "MacUpdateLearn: New MAC [%02X:%02X:%02X:%02X:%02X:%02X] with VID %d, GPORT 0x%08X, flags 0x%x",
            bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
            bcmx_l2_addr->vid, bcmx_l2_addr->lport, bcmx_l2_addr->flags);

  /* only process learns on Native(front panel) ports */
  if ((~bcmx_l2_addr->flags & BCM_L2_NATIVE) && !BCM_GPORT_IS_WLAN_PORT(bcmx_l2_addr->lport))
  {
    hapiMacStats.nonNativeLearn++;
    /* PTin removed: Bug fix */
    #if 0
    return;
    #endif
  }

  if (bcmx_l2_addr->flags & BCM_L2_TRUNK_MEMBER)
  {
    rc1 = hapiBroadTgidToUspConvert(bcmx_l2_addr->tgid, &usp, dapi_g);
    if (rc1 != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
            "Invalid LAG id %d."
            " Possible synchronization issue between the BCM driver and HAPI",
             bcmx_l2_addr->tgid);
      return;
    }
  } else
  {
    if (BCM_GPORT_IS_WLAN_PORT(bcmx_l2_addr->lport))
    {
#ifdef L7_WIRELESS_PACKAGE
      if (hapiBroadWlanUspGet(dapi_g, bcmx_l2_addr->lport, &usp) != L7_SUCCESS)
      {
        /* WLAN port not ready yet */
        return;
      }
#endif
    }
    /* PTin added: virtual ports */
    #if 1
    else if (BCM_GPORT_IS_VLAN_PORT(bcmx_l2_addr->lport))
    {
      usp.unit = (L7_uchar8)L7_LOGICAL_UNIT;                //1;
      usp.slot = (L7_uchar8)platSlotVlanPortSlotNumGet (); //L7_VLAN_PORT_SLOT_NUM;
      usp.port = 0;
      //HAPI_BROAD_LPORT_TO_USP(bcmx_l2_addr->lport,&usp); unusable: UPORTS aren't fixed in case of virtual VLAN PORTs
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DRIVER_COMPONENT_ID, "BCM_GPORT_IS_VLAN_PORT usp=(%d,%d,%d)", usp.unit, usp.slot, usp.port);
    }
    #endif
    else
    {
      uport = BCMX_UPORT_GET(bcmx_l2_addr->lport);

      if (uport == BCMX_UPORT_INVALID_DEFAULT)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
              "Invalid uport calculated from the BCM uport\nbcmx_l2_addr->lport = 0x%x."
              " Uport not valid from BCM driver.", bcmx_l2_addr->lport);
        return;
      }

      HAPI_BROAD_UPORT_TO_USP(uport,&usp);
    }
  }

  if(hapiBroadRoboCheck() == L7_TRUE)
  {
    hapiBroadLportToTgidUspConvert(bcmx_l2_addr->lport, &usp, dapi_g);
  }


  /* Make sure that card is not removed while we are processing the callback.
  */
  dapiCardRemovalReadLockTake ();

  if (!BCM_GPORT_IS_VLAN_PORT(bcmx_l2_addr->lport) && isValidUsp(&usp,dapi_g) == L7_FALSE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
            "Invalid USP calculated from the BCM uport\nbcmx_l2_addr->lport = 0x%x."
            " USP not able to be calculated from the learn event for BCM driver.",
             bcmx_l2_addr->lport);
    dapiCardRemovalReadLockGive ();
    return;
  }

  #ifdef BCM_ROBO_SUPPORT
  /* Add the address to the non-native devices */
  if (hapiBroadRoboVariantCheck() != __BROADCOM_53115_ID)
  {
    rv = usl_bcmx_l2_addr_add(bcmx_l2_addr, L7_NULL);
  }
  else
  {
    rv = BCM_E_NONE;
  }
  #else
  rv = BCM_E_NONE;
  /* If pending flag is active, check if MAC should be learnt */
  if (bcmx_l2_addr->flags & BCM_L2_PENDING)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "MacUpdateLearn: VID %d, GPORT 0x%08X, flags 0x%x",
                  bcmx_l2_addr->vid, bcmx_l2_addr->lport, bcmx_l2_addr->flags);

    #if 1
    if (bcmx_l2_addr->flags & BCM_L2_MOVE)
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, " Decrease learned mac ");
      ptin_hapi_maclimit_dec(bcmx_l2_addr);
    }
    else
    {
      if (ptin_hapi_maclimit_inc(bcmx_l2_addr) != L7_FAILURE)
      {
        LOG_TRACE(LOG_CTX_PTIN_HAPI, " Increase learned mac ");
        bcmx_l2_addr->flags &= ~((L7_uint32)BCM_L2_PENDING);
        rv = usl_bcmx_l2_addr_add(bcmx_l2_addr, L7_NULL);
      }
      else
      {
        /* This action allows rejected MACs to appear in Fdb table */
        //bcmx_l2_addr->flags &= ~((L7_uint32)BCM_L2_PENDING);
        //rv = usl_bcmx_l2_addr_delete(bcmx_l2_addr->mac, bcmx_l2_addr->vid);
        LOG_TRACE(LOG_CTX_PTIN_HAPI, " Warning ");
        LOG_WARNING(LOG_CTX_PTIN_HAPI, "MAC limit has been reached for VID %d, GPORT 0x%08X",
                    bcmx_l2_addr->vid, bcmx_l2_addr->lport);
      }
    }
    #endif

    dapiCardRemovalReadLockGive();
    return;
  }
  else
  {
     /* PTin added: virtual ports */
      //ptin_hapi_maclimit_inc(bcmx_l2_addr);
      /* PTin ended */

    rv = usl_bcmx_l2_addr_add(bcmx_l2_addr, L7_NULL);
    //printf("%s(%d) Yeah!\r\n",__FUNCTION__,__LINE__);
  }
  #endif

  if (rv == BCM_E_NONE)
  {
      hapiBroadL3UpdateMacLearn(bcmx_l2_addr->mac, bcmx_l2_addr->vid, &usp, dapi_g);
#ifdef L7_WIRELESS_PACKAGE
      hapiBroadL2TunnelUpdateMacLearn(bcmx_l2_addr->mac, bcmx_l2_addr->vid, &usp, dapi_g);
#endif

      macAddressInfo.cmdData.unsolLearnedAddress.getOrSet        = DAPI_CMD_SET;
      macAddressInfo.cmdData.unsolLearnedAddress.vlanID          = bcmx_l2_addr->vid;
      macAddressInfo.cmdData.unsolLearnedAddress.flags           = DAPI_ADDR_FLAG_LEARNED;
      macAddressInfo.cmdData.unsolLearnedAddress.macAddr.addr[0] = bcmx_l2_addr->mac[0];
      macAddressInfo.cmdData.unsolLearnedAddress.macAddr.addr[1] = bcmx_l2_addr->mac[1];
      macAddressInfo.cmdData.unsolLearnedAddress.macAddr.addr[2] = bcmx_l2_addr->mac[2];
      macAddressInfo.cmdData.unsolLearnedAddress.macAddr.addr[3] = bcmx_l2_addr->mac[3];
      macAddressInfo.cmdData.unsolLearnedAddress.macAddr.addr[4] = bcmx_l2_addr->mac[4];
      macAddressInfo.cmdData.unsolLearnedAddress.macAddr.addr[5] = bcmx_l2_addr->mac[5];

      /* PTin added: virtual ports */
      #if 1
      /* Save virtual port */
      if (BCM_GPORT_IS_VLAN_PORT(bcmx_l2_addr->lport))
      {
        LOG_TRACE(LOG_CTX_PTIN_HAPI, " Warning ");
        macAddressInfo.virtual_port = _SHR_GPORT_VLAN_PORT_ID_GET(bcmx_l2_addr->lport);
      }
      else
      {
        macAddressInfo.virtual_port = 0;
      }
      #endif

      /* PTin added: physical ports */
      if (BCMX_LPORT_VALID(bcmx_l2_addr->lport))
      {
        LOG_TRACE(LOG_CTX_PTIN_HAPI, " Physical");
        ptin_hapi_maclimit_inc(bcmx_l2_addr);
      } 
      /* PTin added: LAGS ports */
      else if((bcmx_l2_addr->tgid > 0) && ((bcmx_l2_addr->tgid < PTIN_SYSTEM_N_LAGS)))
      {
        LOG_TRACE(LOG_CTX_PTIN_HAPI, " LAG ");
        ptin_hapi_maclimit_inc(bcmx_l2_addr);
      }
      

      /* increment the learn counter regardless of failure */
      hapiMacStats.learn++;

      retry_count = 0;
      do
      {
        rc1 = dapiCallback(&usp,
                           DAPI_FAMILY_ADDR_MGMT,
                           DAPI_CMD_ADDR_UNSOLICITED_EVENT,
                           DAPI_EVENT_ADDR_LEARNED_ADDRESS,
                           &macAddressInfo);
        if (rc1 != L7_SUCCESS)
        {
          retry_count++;
          osapiSleepMSec (100);
        }
      } while ((rc1 != L7_SUCCESS) && (retry_count < 3));

      if (rc1 != L7_SUCCESS)
      {


        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: call to dapiCallback\n",
                       __FILE__, __LINE__);
        hapiMacStats.learnErr++;
      }

  }
  dapiCardRemovalReadLockGive ();
}

/*********************************************************************
*
* @purpose Handle Age indication from vendor driver. Note that this age
*          indication may be part of a re-learn indication which is
*          composed of one age and one learn indication.
*
* @param int            unit
* @param bcm_l2_addr_t *bcmx_l2_addr
* @param DAPI_t*        dapi_g
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadAddrMacUpdateAge(bcmx_l2_addr_t *bcmx_l2_addr, DAPI_t *dapi_g)
{
  DAPI_ADDR_MGMT_CMD_t   macAddressInfo;
  DAPI_USP_t             usp;
  DAPI_USP_t             cpuUsp;
  bcm_l2_addr_t          l2addr;
  L7_BOOL                ageAddr = L7_TRUE, hitSet = L7_FALSE;
  L7_uint32              i, hitUnit = 0;
  int                    rv = 0;

  if (CPU_USP_GET(&cpuUsp) == L7_FAILURE)
  {
    /* If CPU card is not plugged in then exit.
    */
    return;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "hapiBroadAddrMacUpdateAge: Aged MAC [%02X:%02X:%02X:%02X:%02X:%02X] with VID %d, GPORT 0x%08X, flags 0x%x",
            bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
            bcmx_l2_addr->vid, bcmx_l2_addr->lport, bcmx_l2_addr->flags);

  /* If move, it will be handled in the Learn Callback */
  if (bcmx_l2_addr->flags & BCM_L2_MOVE)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "hapiBroadAddrMacUpdateAge: BCM_L2_MOVE flag causes event drop");
    return;
  }

  /* Special handling for addresses learned on a trunk.
  ** Issue the age to application only if the mac has
  ** aged out of all the units. If mac is present on a
  ** unit with HIT bit set, then refresh the mac on
  ** other units if the mac is not present on other units.
  */

  if (bcmx_l2_addr->flags & BCM_L2_TRUNK_MEMBER ||
      BCM_GPORT_IS_WLAN_PORT(bcmx_l2_addr->lport))
  {
    /* Check if the L2 entry is gone from all the chips */
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      rv = bcm_l2_addr_get(i, bcmx_l2_addr->mac, bcmx_l2_addr->vid, &l2addr);
      if (rv == BCM_E_NONE)
      {
        ageAddr = L7_FALSE;
        if (l2addr.flags & (BCM_L2_HIT | BCM_L2_SRC_HIT))
        {
          hitSet = L7_TRUE;
          hitUnit = i;
          break;
        }
      }
    }

    if (ageAddr == L7_FALSE && hitSet == L7_TRUE)
    {
      /* Check if the L2 entry should be refreshed on other units */
      for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
      {
        bcm_l2_addr_t tmpL2Addr;

        /* Skip this unit */
        if (i == hitUnit)
        {
          continue;
        }

        rv = bcm_l2_addr_get(i, bcmx_l2_addr->mac, bcmx_l2_addr->vid, &tmpL2Addr);
        if (rv == BCM_E_NOT_FOUND)
        {
          /* Clear the HIT bit and add the entry on this unit */
          l2addr.flags &= ~(BCM_L2_HIT | BCM_L2_SRC_HIT);
          (void)bcm_l2_addr_add(i, &l2addr);

        }
      }
    }
  }

  /* PTin added: virtual ports */
  #if 1
  if (BCM_GPORT_IS_VLAN_PORT(bcmx_l2_addr->lport))
  {
    usp.unit = (L7_uchar8) L7_LOGICAL_UNIT;                //1;
    usp.slot = (L7_uchar8) platSlotVlanPortSlotNumGet (); //L7_VLAN_PORT_SLOT_NUM;
    usp.port = 0;
    //HAPI_BROAD_LPORT_TO_USP(bcmx_l2_addr->lport,&usp); unusable: UPORTS aren't fixed in case of virtual VLAN PORTs
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DRIVER_COMPONENT_ID, "BCM_GPORT_IS_VLAN_PORT usp=(%d,%d,%d)", usp.unit, usp.slot, usp.port);
  }
  else
  {
    usp = cpuUsp;
  }
  #endif

  if (((bcmx_l2_addr->flags & BCM_L2_NATIVE) && (!(bcmx_l2_addr->flags & BCM_L2_TRUNK_MEMBER)))
      || ((bcmx_l2_addr->flags & BCM_L2_TRUNK_MEMBER) && ageAddr == L7_TRUE)
      || (BCM_GPORT_IS_WLAN_PORT(bcmx_l2_addr->lport) && ageAddr == L7_TRUE)
      || (BCM_GPORT_IS_VLAN_PORT(bcmx_l2_addr->lport) && ageAddr == L7_TRUE))   /* PTin added: virtual ports */
  {
    /* Make sure that card is not removed while we are processing the callback.
    */
    dapiCardRemovalReadLockTake ();

    if (!(bcmx_l2_addr->flags & BCM_L2_TRUNK_MEMBER))
    {
      /* Remove address from hardware */
      /* Do not check return code, no guarentee that the address will be present in all devices */
      if(hapiBroadRoboVariantCheck() != __BROADCOM_53115_ID)
      {
        LOG_TRACE(LOG_CTX_PTIN_HAPI, "hapiBroadAddrMacUpdateAge: VID %d, GPORT 0x%08X, flags 0x%x",
                  bcmx_l2_addr->vid, bcmx_l2_addr->lport, bcmx_l2_addr->flags);

        rv = usl_bcmx_l2_addr_delete(bcmx_l2_addr->mac,bcmx_l2_addr->vid);
        /* PTin added: MAC learning limit */
        ptin_hapi_maclimit_dec(bcmx_l2_addr);
      }
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        /* Keep coverity happy by checking return code */
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI,
                      "\n%s %d: In %s call to 'usl_bcmx_l2_addr_remove' - FAILED : %d\n",
                      __FILE__, __LINE__, __FUNCTION__, rv);
      }
    }

    macAddressInfo.cmdData.unsolAgedAddress.getOrSet        = DAPI_CMD_SET;
    macAddressInfo.cmdData.unsolAgedAddress.vlanID          = bcmx_l2_addr->vid;
    macAddressInfo.cmdData.unsolAgedAddress.flags           = DAPI_ADDR_FLAG_LEARNED;
    macAddressInfo.cmdData.unsolAgedAddress.macAddr.addr[0] = bcmx_l2_addr->mac[0];
    macAddressInfo.cmdData.unsolAgedAddress.macAddr.addr[1] = bcmx_l2_addr->mac[1];
    macAddressInfo.cmdData.unsolAgedAddress.macAddr.addr[2] = bcmx_l2_addr->mac[2];
    macAddressInfo.cmdData.unsolAgedAddress.macAddr.addr[3] = bcmx_l2_addr->mac[3];
    macAddressInfo.cmdData.unsolAgedAddress.macAddr.addr[4] = bcmx_l2_addr->mac[4];
    macAddressInfo.cmdData.unsolAgedAddress.macAddr.addr[5] = bcmx_l2_addr->mac[5];

    /* PTin added: virtual ports */
    #if 1
    /* Save virtual port */
    if (BCM_GPORT_IS_VLAN_PORT(bcmx_l2_addr->lport))
    {
      macAddressInfo.virtual_port = _SHR_GPORT_VLAN_PORT_ID_GET(bcmx_l2_addr->lport);
    }
    else
    {
      macAddressInfo.virtual_port = 0;
    }
    #endif

    hapiBroadL3UpdateMacAge(bcmx_l2_addr->mac, bcmx_l2_addr->vid, dapi_g);

    /* increment the age counter regardless of the failure */
    hapiMacStats.age++;

    if (dapiCallback(&usp,    /* PTin modified: Virtual ports */
                     DAPI_FAMILY_ADDR_MGMT,
                     DAPI_CMD_ADDR_UNSOLICITED_EVENT,
                     DAPI_EVENT_ADDR_AGED_ADDRESS,
                     &macAddressInfo) == L7_FAILURE)
    {

      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: call to dapiCallback\n",
                     __FILE__, __LINE__);
      hapiMacStats.ageErr++;
    }
    dapiCardRemovalReadLockGive ();
  }
  else
  {
    if (!(bcmx_l2_addr->flags & BCM_L2_TRUNK_MEMBER))
    {
    /* Age from a non native device, refreash the address */
    bcmx_l2_addr_refresh(bcmx_l2_addr, L7_NULL);

    /* keep up with the number of nonNativeAge messages */
    hapiMacStats.nonNativeAge++;
    }

  }

}

void hapiDebugMacStatsPrint()
{
#ifndef L7_STACKING_PACKAGE
  sysapiPrintf("Age           = %d\n",hapiMacStats.age);
  sysapiPrintf("ageErr        = %d\n",hapiMacStats.ageErr);
  sysapiPrintf("nonNativeAge  = %d\n",hapiMacStats.nonNativeAge);
  sysapiPrintf("learn         = %d\n",hapiMacStats.learn);
  sysapiPrintf("learnErr      = %d\n",hapiMacStats.learnErr);
  sysapiPrintf("learnFantom   = %d\n",hapiMacStats.learnFantom);
  sysapiPrintf("nonNativeLearn= %d\n",hapiMacStats.nonNativeLearn);
#else
  sysapiPrintf("Use usl_macsync_stats_print on Stacking build\n");
#endif
}

#ifndef L7_STACKING_PACKAGE
#ifdef BCM_ROBO_SUPPORT
extern void
_bcm_robo_l2_from_arl(int unit, bcm_l2_addr_t *l2addr, l2_arl_sw_entry_t *arl_entry);
/*********************************************************************
** Callback function for MAC synchronization.
**
*********************************************************************/
int mac_sync_callback (void *user_data, shr_avl_datum_t *datum , void *extra_data)
{
  int                  unit, i;
  l2_arl_sw_entry_t    *l2x_entry;
  bcm_l2_addr_t        l2Addr, *macAddr;
  int rv;

  if (NULL == datum)
  {
    return BCM_E_NONE;
  }

  unit = (int) extra_data;
  l2x_entry = (l2_arl_sw_entry_t *) datum;

  if (SOC_IS_ROBO(unit))
  {
    _bcm_robo_l2_from_arl(unit, &l2Addr,l2x_entry);
  }

  macAddr = &l2Addr;

  /* Sync the mac learned on trunks only */
  if (!(macAddr->flags & BCM_L2_TRUNK_MEMBER))
   return BCM_E_NONE;

  /* Get the entry on the local unit */
  rv = bcm_l2_addr_get(unit, macAddr->mac,macAddr->vid, &l2Addr);

  /* If the entry is present in the hardware table and the HIT bit is set,
  ** then check if entry should be added on other units.
  */
  if ((rv == BCM_E_NONE) && (!(l2Addr.flags & BCM_L2_STATIC)) &&
    (l2Addr.flags & (BCM_L2_HIT | BCM_L2_SRC_HIT)))
  {
  /* Check if the L2 entry is gone from all the chips */
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
    bcm_l2_addr_t tmpL2Addr;

    /* Skip this unit */
    if (i == unit)
    {
    continue;
    }

    rv = bcm_l2_addr_get(i, macAddr->mac,macAddr->vid, &tmpL2Addr);
    if (rv == BCM_E_NOT_FOUND)
    {
    /* Clear the HIT bit and add the entry on this unit */
    l2Addr.flags &= ~(BCM_L2_HIT | BCM_L2_SRC_HIT);
    rv = bcm_l2_addr_add(i, &l2Addr);

    }
  }

  }

  osapiTaskYield ();

  return BCM_E_NONE;

}
#else

/*********************************************************************
** Callback function for MAC synchronization.
**
*********************************************************************/
int mac_sync_callback (void *user_data, shr_avl_datum_t *datum , void *extra_data)
{
  int                  unit, i;
  l2x_entry_t          *l2x_entry;
  bcm_l2_addr_t        l2Addr, *macAddr;
  int rv;


  if (NULL == datum)
  {
    return BCM_E_NONE;
  }

  unit = (int) extra_data;
  l2x_entry = (l2x_entry_t *) datum;

  if (_bcm_esw_l2_from_l2x(unit, 0, &l2Addr, (uint32 *) l2x_entry) != BCM_E_NONE)
  {
    printf(" %s: _bcm_esw_l2_from_l2x failed \n", __FUNCTION__);
  }

  macAddr = &l2Addr;

  /* Sync the mac learned on trunks only */
  if (!(macAddr->flags & BCM_L2_TRUNK_MEMBER) &&
      !BCM_GPORT_IS_WLAN_PORT(macAddr->port))
     return BCM_E_NONE;

  /* Get the entry on the local unit */
  rv = bcm_l2_addr_get(unit, macAddr->mac,macAddr->vid, &l2Addr);

  /* If the entry is present in the hardware table and the HIT bit is set,
  ** then check if entry should be added on other units.
  */
  if ((rv == BCM_E_NONE) && (!(l2Addr.flags & BCM_L2_STATIC)) &&
      (l2Addr.flags & (BCM_L2_HIT | BCM_L2_SRC_HIT)))
  {
    /* Check if the L2 entry is gone from all the chips */
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      bcm_l2_addr_t tmpL2Addr;

      /* Skip this unit */
      if (i == unit)
      {
        continue;
      }

      rv = bcm_l2_addr_get(i, macAddr->mac,macAddr->vid, &tmpL2Addr);
      if (rv == BCM_E_NOT_FOUND)
      {
        /* Clear the HIT bit and add the entry on this unit */
        l2Addr.flags &= ~(BCM_L2_HIT | BCM_L2_SRC_HIT);
        rv = bcm_l2_addr_add(i, &l2Addr);
        if (rv != BCM_E_NONE)
        {
          printf(" %s: bcm_l2_addr_add failed rv %d\n", __FUNCTION__, rv);
        }

      }
    }

  }

  osapiTaskYield ();

  return BCM_E_NONE;
}
#endif

/*********************************************************************
** Walk the shadow table to synchronize tables
**
*********************************************************************/
L7_RC_t mac_hardware_sync (void)
{
  int bcm_unit;
  soc_control_t *soc;

  for (bcm_unit=0;bcm_unit < soc_ndev;bcm_unit++)
  {
    soc = SOC_CONTROL(bcm_unit);
    if (soc->arlShadowMutex == 0)
    {
     /* Shadow table is not supported on this unit.
     */
     continue;
    }

    sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);

    shr_avl_traverse (soc->arlShadow, mac_sync_callback, (void *) bcm_unit);

    sal_mutex_give(soc->arlShadowMutex);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Syncs the L2 Addresses learned on trunks
*
* @param   L7_uint32        numArgs  - number of arguments
* @param   DAPI_t          *dapi_g   - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadL2AddrMacSyncTask(L7_uint32 numArgs, DAPI_t *dapi_g)
{


  do
  {
    /* Wake up periodically to sync the mac tables.*/
    osapiSleep (60);
    mac_hardware_sync ();
  }
  while (1);

}


/*********************************************************************
*
* @purpose Create a task to handle mac table synchronization
*
* @param   DAPI_t *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes Currently mac-sync task syncs addresses learned on trunk
*        between units on a multiple unit configuration.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2AddrMacSyncInit (DAPI_t *dapi_g)
{

  L7_RC_t  result = L7_SUCCESS;

  /* No need to run mac sync task on single unit devices */
  if (bde->num_devices(BDE_SWITCH_DEVICES) == 1)
  {
    return result;
  }

  /* spawn task */
  if (osapiTaskCreate("hapiL2AddrMacSyncTask", hapiBroadL2AddrMacSyncTask,
                      1, dapi_g, L7_DEFAULT_STACK_SIZE,
                      L7_DEFAULT_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    LOG_ERROR(0);
  }

  return result;
}
#endif

extern DAPI_t *dapi_g;
/*********************************************************************
*
* @purpose Enable/Disable learning on a port or trunk during flush operation.
*
* @param   portInfo  - Learn mode of lport/tgid to be changed
* @param   learnMode - L7_ENABLE: Enable learning
*                      L7_DISABLE: Disable learning
*
* @returns L7_RC_t    result
*
* @notes  In stackable environment we can run into scenarios where the
*         native unit finishes the flush quickly, re-learns new
*         addresses and notifies the manager of the new addresses before
*         the management unit has executed the particular port flush. So
*         management unit will loose all the newly learned port mac-addresses
*         after the flush, causing L2 tables to go out of sync.
*         With this approach, learning is disabled on the port until the management
*         unit finishes the flush. This should keep the management unit in
*         sync with other units. There is still a chance that non-management
*         units may go out of sync...
*
*
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadFlushL2LearnModeSet (BROAD_L2ADDR_FLUSH_t portInfo, L7_uint32 learnMode)
{
  DAPI_USP_t    usp;
  BROAD_PORT_t  *hapiPortPtr = L7_NULLPTR, *hapiLagPortPtr;
  DAPI_PORT_t   *dapiPortPtr;
  bcmx_uport_t  uport;
  L7_BOOL       locked;

  /* When software learning is enabled the synchronization issues don't happen,
  ** so we don't need to disable learning.
  */
  if ( hpcSoftwareLearningEnabled () == L7_TRUE)
  {
      return L7_SUCCESS;
  }

  /* Physical port */
  if (portInfo.port_is_lag == L7_FALSE)
  {
    uport = BCMX_UPORT_GET(portInfo.bcmx_lport);

    if (uport == BCMX_UPORT_INVALID_DEFAULT)
      return L7_FAILURE;

    HAPI_BROAD_UPORT_TO_USP(uport,&usp);

    if (isValidUsp(&usp,dapi_g) == L7_FALSE)
      return L7_FAILURE;

     hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);
     if (hapiPortPtr == L7_NULLPTR)
     {
       return L7_FAILURE;
     }
  }
  else /* trunk */
  {
    usp.unit = L7_LOGICAL_UNIT;
    usp.slot = L7_LAG_SLOT_NUM;

    for (usp.port = 0; usp.port < L7_MAX_NUM_LAG_INTF; usp.port++)
    {
      if (isValidUsp(&usp, dapi_g) == L7_FALSE)
      {
        continue;
      }

      hapiPortPtr = HAPI_PORT_GET(&usp,dapi_g);
      if (hapiPortPtr == L7_NULLPTR)
      {
        continue;
      }

      if (hapiPortPtr->hapiModeparm.lag.tgid == portInfo.tgid)
        break;
    }

    /* No trunk found with matching tgid. */
    if (usp.port >= L7_MAX_NUM_LAG_INTF)
    {
      return L7_FAILURE;
    }

    if (isValidUsp(&usp,dapi_g) == L7_FALSE)
      return L7_FAILURE;
  }

  dapiPortPtr = DAPI_PORT_GET(&usp, dapi_g);

  osapiSemaTake(hapiPortPtr->l2FlushSyncSem, L7_WAIT_FOREVER);

  if (learnMode == L7_ENABLE) /* Enable learning */
  {
    hapiPortPtr->l2FlushReq--;
    if (hapiPortPtr->l2FlushReq <= 0)
    {
      /* We should never get into this state, but to be on the safe side. */
      if (hapiPortPtr->l2FlushReq < 0)
      {
        hapiPortPtr->l2FlushReq = 0;
      }
      hapiPortPtr->l2FlushInProgress = L7_FALSE;

      /*Check if dot1x has set the port status to authorize.Allow learning only when dot1x port status is authorized */
      if((hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED) || (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr)))
      {
          locked = hapiPortPtr->locked;

          if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
          {
            if (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
            {
              hapiLagPortPtr = HAPI_PORT_GET(&hapiPortPtr->hapiModeparm.physical.lagUsp, dapi_g);
              locked = hapiLagPortPtr->locked;
            }
          }
        if (hapiPortPtr->cpBlocked == L7_TRUE)
        {
          /* Configure hw to drop unknown SA on this port */
          hapiBroadLearnSet(&usp, 0, dapi_g);
        }
        /* Check the port-security mode of the port */
        else if ((locked == L7_TRUE) ||
                 (hapiPortPtr->cpEnabled == L7_TRUE))
          {
            /* disable HW learning on this port */
            hapiBroadLearnSet(&usp, BCM_PORT_LEARN_CPU, dapi_g);
          } else
          {
            /* enable HW learning on this port */
            hapiBroadLearnSet(&usp, (BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD), dapi_g);
          }
      }
    }
  }
  else /* Disable learning */
  {
    hapiPortPtr->l2FlushReq++;
    if (hapiPortPtr->l2FlushInProgress == L7_FALSE)
    {
      hapiPortPtr->l2FlushInProgress = L7_TRUE;
      /* Change the Learn flags for the port during the flush, only if dot1x port status is authorized*/
      if((hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED) || (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr)))
      {
          locked = hapiPortPtr->locked;

          if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
          {
            if (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
            {
              hapiLagPortPtr = HAPI_PORT_GET(&hapiPortPtr->hapiModeparm.physical.lagUsp, dapi_g);
              locked = hapiLagPortPtr->locked;
            }
          }
          if (hapiPortPtr->cpBlocked == L7_TRUE)
          {
            /* Configure hw to drop unknown SA on this port */
            hapiBroadLearnSet(&usp, 0, dapi_g);
          }
          else if (hapiPortPtr->cpEnabled == L7_TRUE)
          {
            /* disable HW learning on this port */
            hapiBroadLearnSet(&usp, BCM_PORT_LEARN_CPU, dapi_g);
          }
          else if (locked == L7_TRUE)
          {
              hapiBroadLearnSet(&usp, 0, dapi_g);
          }
          else
          {
              /* disable HW learning on this port */
              hapiBroadLearnSet(&usp, BCM_PORT_LEARN_FWD, dapi_g);
          }
      }
    }
  }

  osapiSemaGive(hapiPortPtr->l2FlushSyncSem);

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Create a message queue and task to handle requests to flush
*          L2 address in the hardware either for a port or a lag
*
* @param   DAPI_t *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   Since dynamic L2 addr flushing takes a good 2 sec on 5690
*          we are running that in a different thread
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2AddrFlushInit (DAPI_t *dapi_g)
{
  L7_RC_t  result = L7_SUCCESS;
  L7_uint32  size;

  memset((void *) &hapiBroadFlushStats_g,0,sizeof(hapiBroadFlushStats_g));

  /* an lplist to indicate which lport should be flushed */
  size  = platIntfPhysicalIntfMaxCountGet();

  if (BCMX_LPLIST_IS_NULL(&hapiBroadFlushApplpList))
  {
    bcmx_lplist_init(&hapiBroadFlushApplpList, size, 0);
  }

  if (BCMX_LPLIST_IS_NULL(&hapiBroadFlushTasklpList))
  {
    bcmx_lplist_init(&hapiBroadFlushTasklpList, size, 0);
  }

  hapiBroadFlushSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (hapiBroadFlushSema == L7_NULL)
  {
    LOG_ERROR(0);
  }

  /* an array to indicate each trunk being flushed */
  size = (platIntfLagIntfMaxCountGet() / (sizeof(L7_uint32)*8)) +1; /* number of words */
  size  = size * sizeof(L7_uint32); /* number of bytes */

  if (hapiBroadFlushTgidList  == L7_NULLPTR)
    hapiBroadFlushTgidList = osapiMalloc(L7_DRIVER_COMPONENT_ID,size);

  if (hapiBroadFlushTgidList == L7_NULLPTR) LOG_ERROR(hapiBroadFlushTgidList);

  memset((void *)hapiBroadFlushTgidList,0,size);

  /* an array to indicate flushes for vlans */
  size = (platVlanVlanIdMaxGet() / (sizeof(L7_uint32)*8)) +1; /* number of words */
  size  = size * sizeof(L7_uint32); /* number of bytes */

  if (hapiBroadFlushVlanList == L7_NULLPTR)
    hapiBroadFlushVlanList = osapiMalloc(L7_DRIVER_COMPONENT_ID,size);

  if (hapiBroadFlushVlanList == L7_NULLPTR) LOG_ERROR(hapiBroadFlushVlanList);


  memset(hapiBroadFlushMac, 0, L7_ENET_MAC_ADDR_LEN);

  /* create a queue to receive requests to flush L2 address */
  hapiL2AddrFlushQueue = (void *)osapiMsgQueueCreate("hapiL2AddrFlushQueue",
                                                      1,
                                                      sizeof(BROAD_L2ADDR_FLUSH_t));

  if (hapiL2AddrFlushQueue == L7_NULL) LOG_ERROR(hapiL2AddrFlushQueue);

  /* spawn task */
  if (osapiTaskCreate("hapiL2AddrFlushTask", hapiBroadL2AddrFlushTask,
                      1, dapi_g, L7_DEFAULT_STACK_SIZE,
                      L7_DEFAULT_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    LOG_ERROR(0);
  }

  return result;
}

/*********************************************************************
*
* @purpose Flushes the L2 Address for the messsages received
*
* @param   L7_uint32        numArgs  - number of arguments
* @param   DAPI_t          *dapi_g   - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadL2AddrFlushTask(L7_uint32 numArgs, DAPI_t *dapi_g)
{
  BROAD_L2ADDR_FLUSH_t   l2addr_flush;
  bcmx_lport_t           lport;
  L7_int32               index;
  L7_uint32              tgid = 0;
  L7_int32               count = 0;
  L7_uint32              max = 0;
  L7_uint32              vlan = 0;
  L7_uint32              bitpos = 0;
  bcm_mac_t mac;

  while (1)
  {
    if (osapiMessageReceive(hapiL2AddrFlushQueue, (void *)&l2addr_flush,
                            sizeof(BROAD_L2ADDR_FLUSH_t),
                            L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      /* no flush requested */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "hapiL2AddrFlushQueue: failed in recv message\n");
      continue;
    }

    count = 0;

    memset((void *)&l2addr_flush, 0, sizeof(l2addr_flush));

    bcmx_lplist_clear(&hapiBroadFlushTasklpList);

    /* Make a copy of the hapiBroadFlushApplpList and clear it */
    osapiSemaTake(hapiBroadFlushSema, L7_WAIT_FOREVER);
    (void)bcmx_lplist_copy(&hapiBroadFlushTasklpList,  &hapiBroadFlushApplpList);
    (void)bcmx_lplist_clear(&hapiBroadFlushApplpList);
    osapiSemaGive(hapiBroadFlushSema);

    (void)bcmx_lplist_uniq(&hapiBroadFlushTasklpList);

    /* Prevent the case where we process a FLUSH during a bcm_clear */
    usl_mac_table_sync_suspend();

    max  = platIntfPhysicalIntfMaxCountGet();

    /* iterate over the ports */

    BCMX_LPLIST_ITER(hapiBroadFlushTasklpList, lport, count)
    {
      if (hapiDot1sDebug & HAPI_BROAD_DOT1S_DEBUG_ENQUEUE)
         printf ("%s : Flushing physical port 0x%x\n", __FUNCTION__, lport);
      if (BCMX_LPORT_VALID(lport))
      {
        /* Disable learning on the port */
        l2addr_flush.bcmx_lport = lport;
        l2addr_flush.port_is_lag = L7_FALSE;
        hapiBroadFlushL2LearnModeSet(l2addr_flush, L7_DISABLE);

        hapiBroadFlushStats_g.hapiBroadPortFlushesDone++;
        (void) usl_bcmx_l2_addr_remove_by_port(lport);
      }
    }

    max = (platIntfLagIntfMaxCountGet() / (sizeof(L7_uint32) * 8)) + 1;

    /* iterate over the lags */
    for (index = 0; index < max;index++)
    {
      if (hapiBroadFlushTgidList[index] != 0)
      {
        for (bitpos = 0;bitpos < 32;bitpos++)
        {
          if ( hapiBroadFlushTgidList[index] & (1 << bitpos))
          {
            hapiBroadFlushTgidList[index] &= ~(1 << bitpos);
            tgid = (index * 32) + bitpos;
            if (hapiDot1sDebug & HAPI_BROAD_DOT1S_DEBUG_ENQUEUE)
              printf ("%s : Flushing lag port %d\n", __FUNCTION__, tgid);

            /* Disable learning on the port */
            l2addr_flush.tgid = tgid;
            l2addr_flush.port_is_lag = L7_TRUE;
            hapiBroadFlushL2LearnModeSet(l2addr_flush, L7_DISABLE);

            (void) usl_bcmx_l2_addr_remove_by_trunk (tgid);
            count++;
          }
        }
      }
    }

    max = (platVlanVlanIdMaxGet() / (sizeof(L7_uint32) * 8)) + 1;

    /* iterate over the VLANs */
    for (index = 0; index < max;index++)
    {
      if (hapiBroadFlushVlanList[index] != 0)
      {
        for (bitpos = 0;bitpos < 32;bitpos++)
        {
          if ( hapiBroadFlushVlanList[index] & (1 << bitpos))
          {
            hapiBroadFlushVlanList[index] &= ~(1 << bitpos);

            vlan = (index * 32) + bitpos;
            if (hapiDot1sDebug & HAPI_BROAD_DOT1S_DEBUG_ENQUEUE)
              printf ("%s : Flushing vlan %d\n", __FUNCTION__, vlan);

            (void) usl_bcmx_l2_addr_remove_by_vlan (vlan);
            count++;
          }
        }
      }
    }

    /*supports flush by only one MAC at a time.i.e should issue 
       another flush by mac only after completion of previous flush by mac */
    memcpy(mac, hapiBroadFlushMac, L7_ENET_MAC_ADDR_LEN);
    (void) usl_bcmx_l2_addr_remove_by_mac (mac);

    usl_mac_table_sync_resume();

    if (hapiBroadFlushStats_g.hapiBroadMaxFlushesInLoop < count)
      hapiBroadFlushStats_g.hapiBroadMaxFlushesInLoop = count;
  }

  return ;
}


/*********************************************************************
 *
 * @purpose Installs the DHCP policies for DHCP snooping and elevating
 *          DHCP packet priority
 *
 * @param *dapi_g
 *
 * @returns L7_RC_t
 *
 * @notes   This function is called from hapiBroadSystemPolicyInstall()
            and hapiBroadSystemPolicyInstallRaptor().
 *
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadDhcpPolicyInstall(DAPI_t *dapi_g)
{
  /* PTin removed: DHCP snooping */
  #if 0
  L7_RC_t             result;
  L7_ushort16         ip_ethtype  = L7_ETYPE_IP;
  L7_uchar8           udp_proto[]   = {IP_PROT_UDP};
  L7_ushort16         dhcpc_dport   = UDP_PORT_DHCP_CLNT;
  L7_ushort16         dhcps_dport   = UDP_PORT_DHCP_SERV;
  L7_uchar8           exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                       FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  BROAD_SYSTEM_t     *hapiSystem;
  BROAD_POLICY_RULE_t ruleId;
  BROAD_METER_ENTRY_t meterInfo;
  BROAD_POLICY_TYPE_t policyType = BROAD_POLICY_TYPE_SYSTEM;

  if (hapiBroadRaptorCheck() == L7_TRUE)
  {
    /* On Raptor, the system slice cannot match on UDF fields. Only L2 fields
     * can be matched. So, make DHCP policies port type instead.
     */
    policyType = BROAD_POLICY_TYPE_PORT;
  }

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  /* There are 3 set of policies for DHCP packets. Here is why.
   * When DHCP snooping is enabled, we will have a set of trusted and
   * untrusted ports. DHCP packets on trusted ports are copied to CPU
   * with elevated priorirty. DHCP packets on un-trusted ports are trapped
   * to CPU and are rate-limited (priority is not elevated).
   *
   * When DHCP snooping is disabled (default), priority of all DHCP packets
   * is elevated so that a bcast/mcast flood doesn't impact the DHCP leasing.
   * This is the default system-wide policy for DHCP packets, unless DHCP
   * snooping overrides this. Note, priority is elevated to just above
   * mcast/bcast/l3 miss packets.
   */
  /* DHCP packets on untrusted ports must go to the CPU and be rate limited to 64 kbps */
  meterInfo.cir       = 512;
  meterInfo.cbs       = 512;
  meterInfo.pir       = 512;
  meterInfo.pbs       = 512;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;

  hapiBroadPolicyCreate(policyType);

  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO, udp_proto, exact_match);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DPORT, (L7_uchar8*)&dhcpc_dport, exact_match);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
  hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);

  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO, udp_proto, exact_match);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DPORT, (L7_uchar8*)&dhcps_dport, exact_match);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
  hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);

  result = hapiBroadPolicyCommit(&hapiSystem->dhcpSnoopingPolicyId);
  if (L7_SUCCESS != result)
     return result;

  /* PTin removed: DHCP snooping */
  #if 0
  if (policyType == BROAD_POLICY_TYPE_SYSTEM)
  {
    result = hapiBroadPolicyRemoveFromAll(hapiSystem->dhcpSnoopingPolicyId);
    if (L7_SUCCESS != result)
       return result;
  }
  #endif

  /* DHCP packets are broadcast and contend with all multicast data traffic, L3 DST miss traffic.
   * Elevate the priority of DHCP packets, so that multicast/bcast flood or L3 dest miss traffic
   * doesn't prevent a DHCP lease (both when FastPath is a client or a server. If DHCP snooping is
   * enabled, the above trusted/untrusted policies must have higher precedence, so order matters.
   */
  if (hapiBroadRaptorCheck() == L7_FALSE) /* Can't do this on raptor */
  {
    hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);

    hapiBroadPolicyRuleAdd(&ruleId);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO, udp_proto, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DPORT, (L7_uchar8*)&dhcpc_dport, exact_match);
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_LOW_PRIORITY_COS, 0, 0);

    hapiBroadPolicyRuleAdd(&ruleId);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO, udp_proto, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DPORT, (L7_uchar8*)&dhcps_dport, exact_match);
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_LOW_PRIORITY_COS, 0, 0);

    result = hapiBroadPolicyCommit(&hapiSystem->dhcpDefaultPolicyId);
  }
  else
  {
    hapiSystem->dhcpDefaultPolicyId = BROAD_POLICY_INVALID;
  }

  return result;
  #else
  return L7_SUCCESS;
  #endif
}

/*********************************************************************
*
* @purpose Updates the DHCP Snooping policy memberships for a port.
*
* @param   DAPI_USP_t       *portUsp
* @param   L7_BOOL           enabled
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDhcpSnoopingPortUpdate(DAPI_USP_t *portUsp,
                                        L7_BOOL     enabled,
                                        DAPI_t     *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  BROAD_PORT_t   *hapiPortPtr;
  bcmx_lport_t    lport;
  BROAD_SYSTEM_t *hapiSystem;

  hapiSystem     = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;
  hapiPortPtr    = HAPI_PORT_GET(portUsp, dapi_g);

  if (BROAD_POLICY_INVALID != hapiSystem->dhcpSnoopingPolicyId)
  {
    lport = hapiPortPtr->bcmx_lport;
    if (enabled == L7_TRUE)
    {
      rc = hapiBroadPolicyApplyToIface(hapiSystem->dhcpSnoopingPolicyId, lport);
    }
    else
    {
      rc = hapiBroadPolicyRemoveFromIface(hapiSystem->dhcpSnoopingPolicyId, lport);
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Used to configure DHCP Snooping on a port.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param   oldEnabled     @b{(input)} Old Enabled mode
* @param   oldTrusted     @b{(input)} Old Trusted mode
* @param   newEnabled     @b{(input)} New Enabled mode
* @param   newTrusted     @b{(input)} New Trusted mode
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  void
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDhcpSnoopingSet( DAPI_USP_t *usp,
                                  L7_BOOL     enabled,
                                  DAPI_t     *dapi_g )
{
  L7_int32                 rc = L7_SUCCESS;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;
  L7_uint32                i;

  dapiPortPtr = DAPI_PORT_GET( usp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( usp, dapi_g );

  /* assumes this function is only called w/ physical and LAG ports */
  if ( IS_PORT_TYPE_PHYSICAL( dapiPortPtr ) )
  {
    if ( hapiPortPtr->hapiModeparm.physical.isMemberOfLag != L7_TRUE )
    {
      rc = hapiBroadDhcpSnoopingPortUpdate(usp, enabled, dapi_g);
    }
  }
  else if ( IS_PORT_TYPE_LOGICAL_LAG( dapiPortPtr ) )
  {
    hapiBroadLagCritSecEnter();

    for ( i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++ )
    {
      if ( L7_TRUE == dapiPortPtr->modeparm.lag.memberSet[i].inUse )
      {
        rc = hapiBroadDhcpSnoopingPortUpdate(&dapiPortPtr->modeparm.lag.memberSet[i].usp,
                                             enabled, dapi_g);
      }
    }
    hapiBroadLagCritSecExit();
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Used to configure DHCP Snooping on a port.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DHCP_SNOOPING_CONFIG
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dhcpSnoopingConfig
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDhcpSnoopingConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;

  if (dapiCmd->cmdData.dhcpSnoopingConfig.getOrSet != DAPI_CMD_SET)
    return L7_FAILURE;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if ( ( L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ) ) &&
       ( L7_FALSE == IS_PORT_TYPE_LOGICAL_LAG( dapiPortPtr ) ) )
  {
    return( L7_FAILURE );
  }

  if (dapiCmd->cmdData.dhcpSnoopingConfig.enabled == hapiPortPtr->dhcpSnoopingEnabled)
  {
    return L7_SUCCESS;
  }
  result = hapiBroadDhcpSnoopingSet(usp, dapiCmd->cmdData.dhcpSnoopingConfig.enabled,
                                      dapi_g);
  if (result == L7_SUCCESS)
  {
    hapiPortPtr->dhcpSnoopingEnabled = dapiCmd->cmdData.dhcpSnoopingConfig.enabled;
  }

  return( result );
}

/*********************************************************************
*
* @purpose Notifies the DHCP Snooping component that a physical port has been
*          added to a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDhcpSnoopingLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  BROAD_PORT_t   *hapiPortPtr;
  BROAD_PORT_t   *hapiLagPtr;

  hapiPortPtr    = HAPI_PORT_GET(portUsp, dapi_g);
  hapiLagPtr     = HAPI_PORT_GET(lagUsp,  dapi_g);

  if (hapiPortPtr->dhcpSnoopingEnabled != hapiLagPtr->dhcpSnoopingEnabled)
  {
    /* If the newly added port has different status than LAG port, then
       apply the LAG port state to the newly added port also.*/
    rc = hapiBroadDhcpSnoopingPortUpdate(portUsp,hapiLagPtr->dhcpSnoopingEnabled,
                                         dapi_g);
  }
  return rc;
}

/*********************************************************************
*
* @purpose Notifies the DHCP Snooping component that a physical port has been
*          removed from a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadDhcpSnoopingLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  BROAD_PORT_t   *hapiPortPtr;
  BROAD_PORT_t   *hapiLagPtr;

  hapiPortPtr    = HAPI_PORT_GET(portUsp, dapi_g);
  hapiLagPtr     = HAPI_PORT_GET(lagUsp,  dapi_g);

  if (hapiPortPtr->dhcpSnoopingEnabled != hapiLagPtr->dhcpSnoopingEnabled)
  {
    /* If the deleted port has different status than LAG port, then
       apply the port's state back to the deleted port. */
    rc = hapiBroadDhcpSnoopingPortUpdate(portUsp,hapiPortPtr->dhcpSnoopingEnabled,
                                         dapi_g);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Configure a group of protected ports
*
* @param   *usp    @b{(input)}  Dummy USP
* @param   cmd     @b{(input)}  DAPI_CMD_PROTECTED_PORT_ADD
* @param   *data   @b{(input)}  DAPI_INTF_MGMT_CMD_t.cmdData.protectedPortConfig
* @param   *dapi_g @b{(input)}  The driver object
*
* @returns L7_RC_t result
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2ProtectedGroupConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd,
                                       void *data, DAPI_t *dapi_g)
{
  DAPI_INTF_MGMT_CMD_t          *dapiCmd;
  DAPI_USP_t                     dapiUsp;
  BROAD_PORT_t                  *hapiPortPtr;
  L7_uint32                      index;
  L7_RC_t                        result = L7_SUCCESS;
  int                            rv, modid, modport;
  usl_bcm_protected_group_t      groupInfo;

  memset(&groupInfo, 0, sizeof(groupInfo));

  dapiCmd = (DAPI_INTF_MGMT_CMD_t *) data;

  groupInfo.groupId = dapiCmd->cmdData.protectedPortConfig.groupId;

  for (index = 0; index < L7_MAX_INTERFACE_COUNT; index++)
  {
    dapiUsp.unit = dapiCmd->cmdData.protectedPortConfig.ports[index].unit;
    dapiUsp.slot = dapiCmd->cmdData.protectedPortConfig.ports[index].slot;
    dapiUsp.port = dapiCmd->cmdData.protectedPortConfig.ports[index].port;

    if (isValidUsp (&dapiUsp, dapi_g) != L7_TRUE)
    {
      break;                    /* First invalid USP implies end of list */
    }

    hapiPortPtr = HAPI_PORT_GET(&dapiUsp, dapi_g);
    modid = BCM_GPORT_MODPORT_MODID_GET(hapiPortPtr->bcmx_lport);
    modport = BCM_GPORT_MODPORT_PORT_GET(hapiPortPtr->bcmx_lport);
    if (modid < 0 || modport < 0)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
              "Failed to get modid/modport for lport %x: modid %d modport %d\n",
              hapiPortPtr->bcmx_lport, modid, modport);
      continue;
    }

    BCM_PBMP_PORT_ADD(groupInfo.mod_pbmp[modid], modport);
  }

  if (index == 0)  /* No members in the protected group */
  {
    return L7_FAILURE;
  }


  rv = usl_bcmx_protected_group_set(&groupInfo);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_HAPI_ERROR,
                  "%s %d: Failure in %s! rc = %d !\n",
                  __FILE__, __LINE__, __FUNCTION__, rv);


    result = L7_FAILURE;
  }

  return result;
}
/*********************************************************************
*
* @purpose  Remove a port from a protected group
*
* @param   *usp    @b{(input)}  USP of the port to be removed
* @param   cmd     @b{(input)}  DAPI_CMD_PROTECTED_PORT_DELETE
* @param   *data   @b{(input)}  DAPI_INTF_MGMT_CMD_t.cmdData.protectedPortConfig
* @param   *dapi_g @b{(input)}  The driver object
*
* @returns L7_RC_t result
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2ProtectedPortDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd,
                                       void *data, DAPI_t *dapi_g)
{
  L7_RC_t               result = L7_SUCCESS;
  BROAD_PORT_t         *hapiPortPtr;
  L7_uint32             groupId;
  DAPI_INTF_MGMT_CMD_t *dapiCmd;
  int                   rv;

  if (isValidUsp(usp, dapi_g) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  dapiCmd = (DAPI_INTF_MGMT_CMD_t *) data;
  groupId = dapiCmd->cmdData.protectedPortConfig.groupId;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  rv = usl_bcmx_protected_group_port_remove(hapiPortPtr->bcmx_lport, groupId);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    result = L7_FAILURE;
  }

  return result;
}

void hapiBroadL2FlushRequest(BROAD_L2ADDR_FLUSH_t flushReq)
{

  do {
    if (flushReq.flushtype == BROAD_FLUSH_BY_PORT)
    {
      if (flushReq.port_is_lag)
      {
        hapiBroadFlushTgidList[(flushReq.tgid/32)] |= (1 << (flushReq.tgid % 32));
        hapiBroadFlushStats_g.hapiBroadTrunkFlushesIssued++;
      }
      else
      {
        osapiSemaTake(hapiBroadFlushSema, L7_WAIT_FOREVER);
        BCMX_LPLIST_ADD(&hapiBroadFlushApplpList, flushReq.bcmx_lport);
        osapiSemaGive(hapiBroadFlushSema);
        hapiBroadFlushStats_g.hapiBroadPortFlushesIssued++;
      }
    }
    else if (flushReq.flushtype == BROAD_FLUSH_BY_VLAN)
    {
      hapiBroadFlushVlanList[flushReq.vlanID/32] |= ( 1 << (flushReq.vlanID % 32));
      hapiBroadFlushStats_g.hapiBroadVlanFlushesIssued++;
    }
    else if (flushReq.flushtype == BROAD_FLUSH_BY_MAC)
    {
    /*supports flush by only one MAC at a time.i.e should issue 
       another flush by mac only after completion of previous flush by mac */
      memcpy(hapiBroadFlushMac, flushReq.mac.addr, L7_ENET_MAC_ADDR_LEN);
      hapiBroadFlushStats_g.hapiBroadMacFlushesIssued++;
    }
    else
    {
      /* not a valid FLUSH type */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "Invalid Flush type %d requested\n",flushReq.flushtype);
      hapiBroadFlushStats_g.hapiBroadInvalidFlushesIssued++;
      break;
    }

    /* Send a message to L2 address flushing task with the port info */
    (void)osapiMessageSend (hapiL2AddrFlushQueue, (void*)&flushReq,
                            sizeof (BROAD_L2ADDR_FLUSH_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  } while (0);

  return;
}

/*********************************************************************
*
* @purpose Wakeup the L2 Async task.
*
* @param none
*
* @returns L7_RC_t
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2AsyncTaskSignal(void)
{
  hapi_broad_l2_async_msg_t l2async_cmd;

  l2async_cmd.dummy = 0;

  (void) osapiMessageSend (hapiBroadL2AsyncCmdQueue,
                           (void*)&l2async_cmd,
                           sizeof (l2async_cmd),
                           L7_NO_WAIT,
                           L7_MSG_PRIORITY_NORM);
  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose Wait for Dot1s async work to finish.
*
* @param none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadDot1sAsyncDoneWait(void)
{

  hapi_broad_dot1s_async_msg_t   dot1s_cmd;
  L7_RC_t     rc;

  dot1s_cmd.async_cmd = (L7_uchar8)HAPI_BROAD_DOT1S_ASYNC_DONE;

  rc = osapiMessageSend (hapiBroadDot1sAsyncCmdQueue,
                         (void*)&dot1s_cmd,
                         sizeof (dot1s_cmd),
                         L7_WAIT_FOREVER,
                         L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  /* Signal the L2 Async task that work is pending. */
  (void)hapiBroadL2AsyncTaskSignal();


  osapiSemaTake(hapiBroadDot1sAsyncDoneSema,L7_WAIT_FOREVER);
}

/*********************************************************************
*
* @purpose The L2 Async task processes Lag membership updates and
*          spanning-tree state sets asynchronously.
*
* @param   num_args    @b{(input)}  Number of arguments
* @param   dapi_g      @b{(input)}  Driver object
*
* @returns none
*
* @notes  Lag membership updates and dot1s state sets are processed
*         in a weighted fashion. The default weight is set to be 1.
*
* @end
*
*********************************************************************/
static void hapiBroadL2AsyncTask(L7_uint32 num_args, DAPI_t *dapi_g)
{
  L7_uint32 num_dot1s_msg, num_lag_msg;
  L7_RC_t   rc;
  hapi_broad_l2_async_msg_t  l2async_cmd;

  while (1) {

      if (osapiMessageReceive(hapiBroadL2AsyncCmdQueue, (void *)&l2async_cmd,
                              sizeof(l2async_cmd),
                              L7_WAIT_FOREVER) != L7_SUCCESS)
      {
        LOG_ERROR(0);
      }

      while (1) {

        num_dot1s_msg = num_lag_msg = 0;
        rc = osapiMsgQueueGetNumMsgs(hapiBroadDot1sAsyncCmdQueue, &num_dot1s_msg);
        if (rc != L7_SUCCESS)
        {
          LOG_ERROR (rc);
        }

        rc = osapiMsgQueueGetNumMsgs(hapiBroadLagAsyncCmdQueue, &num_lag_msg);
        if (rc != L7_SUCCESS)
        {
          LOG_ERROR (rc);
        }

        /* No more messages left to be processed */
        if ((num_dot1s_msg == 0) && (num_lag_msg == 0)) {
          break;
        }

        (void)hapiBroadLagAsyncProcessMessages(HAPI_BROAD_L2_ASYNC_LAG_MSG_WEIGHT, dapi_g);
        (void)hapiBroadDot1sAsyncProcessMessages(HAPI_BROAD_L2_ASYNC_DOT1S_MSG_WEIGHT, dapi_g);
    }

    hapiBroadL2AsyncMcastRetryFailures();
  }
}
/*********************************************************************
*
* @purpose Create message queues and task to handle spanning-tree state
*          sets and Lag port management asynchronously.
*
* @param   DAPI_t *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   Since Protocol threads get blocked for an unacceptable amount
*          of time for RPCs to finish, we need to handle these requests
*          asynchronously in the driver. For spanning-tree state sets,
*          application expects a callback indicating whether the request
*          was successfully processed or not. For Lags, there is no such
*          mechanism.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2AsyncInit(DAPI_t *dapi_g)
{
  L7_uint32 async_cmd_size;

 /* Create the spanning-tree async state set message queue */

  async_cmd_size = sizeof (hapi_broad_dot1s_async_msg_t);

  hapiBroadDot1sAsyncCmdQueue = osapiMsgQueueCreate ("hapiDot1sAsyncCmdQ",
                                                     HAPI_BROAD_DOT1S_ASYNC_CMD_QUEUE_SIZE,
                                                     async_cmd_size);
  if (hapiBroadDot1sAsyncCmdQueue == L7_NULL)
  {
    LOG_ERROR (0);
  }

  hapiBroadDot1sAsyncDoneSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO,OSAPI_SEM_EMPTY);
  if (hapiBroadDot1sAsyncDoneSema == L7_NULL)
  {
    LOG_ERROR(0);
  }

  async_cmd_size = sizeof (hapi_broad_lag_async_msg_t);

  hapiBroadLagAsyncCmdQueue = osapiMsgQueueCreate ("hapiLagAsyncCmdQ",
                                                   HAPI_BROAD_LAG_ASYNC_CMD_QUEUE_SIZE,
                                                   async_cmd_size);
  if (hapiBroadLagAsyncCmdQueue == L7_NULL)
  {
    LOG_ERROR (0);
  }

  hapiBroadLagAsyncDoneSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO,OSAPI_SEM_EMPTY);
  if (hapiBroadLagAsyncDoneSema == L7_NULL)
  {
    LOG_ERROR(0);
  }


  /* Create the L2 async message queue */

  async_cmd_size = sizeof (hapi_broad_l2_async_msg_t);

  hapiBroadL2AsyncCmdQueue = osapiMsgQueueCreate ("hapiL2AsyncCmdQ",
                                                   HAPI_BROAD_L2_ASYNC_CMD_QUEUE_SIZE,
                                                   async_cmd_size);
  if (hapiBroadL2AsyncCmdQueue == L7_NULL)
  {
    LOG_ERROR (0);
  }

  /* Start the L2 async task */
  /* Note: The stack size for L2 async task is set to DOT1S stack size,
   * instead of FastPath default stack size. The hapiBroadL2AsyncTask handles
   * the DOT1S DAPI callback, which can possibly hit deeper recursion levels in
   * the DOT1S state machine.
   */
  if (osapiTaskCreate("hapiL2AsyncTask", hapiBroadL2AsyncTask,
                      1, dapi_g, L7_DOT1S_STACK_SIZE,
                      L7_DEFAULT_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    LOG_ERROR(0);
  }

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Updates the Dynamic Arp Inspection policy memberships for a port.
*          added to a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   L7_BOOL           oldTrusted
* @param   L7_BOOL           newTrusted
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDynamicArpInspectionPortUpdate(DAPI_USP_t *portUsp,
                                                L7_BOOL     oldTrusted,
                                                L7_BOOL     newTrusted,
                                                DAPI_t     *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  BROAD_PORT_t   *hapiPortPtr;
  bcmx_lport_t    lport;
  BROAD_SYSTEM_t *hapiSystem;
  L7_ushort16     temp16;

  hapiSystem     = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;
  hapiPortPtr    = HAPI_PORT_GET(portUsp, dapi_g);

  if (BROAD_POLICY_INVALID != hapiSystem->dynamicArpInspectUntrustedPolicyId)
  {
    lport = hapiPortPtr->bcmx_lport;

    temp16 = (oldTrusted << 8) |
             (newTrusted);

    switch (temp16)                                          /*       Old             New       */
    {                                                        /*    Trusted          Trusted     */

    case 0x0000:                                             /*        F               F        */
    case 0x0101:                                             /*        T               T        */
      /* No action needed */
      break;
    case 0x0001:                                             /*        F               T        */
      rc = hapiBroadPolicyRemoveFromIface(hapiSystem->dynamicArpInspectUntrustedPolicyId, lport);
      break;
    case 0x0100:                                             /*        T               F        */
      rc = hapiBroadPolicyApplyToIface(hapiSystem->dynamicArpInspectUntrustedPolicyId, lport);
      break;

    default:
      LOG_ERROR(temp16);
      break;
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Used to configure Dynamic Arp Inspection on a port.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param   oldTrusted     @b{(input)} Old Trusted mode
* @param   newTrusted     @b{(input)} New Trusted mode
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  void
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDynamicArpInspectionSet( DAPI_USP_t *usp,
                                          L7_BOOL     oldTrusted,
                                          L7_BOOL     newTrusted,
                                          DAPI_t     *dapi_g )
{
  L7_int32                 rc = L7_SUCCESS;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;
  L7_uint32                i;

  dapiPortPtr = DAPI_PORT_GET( usp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( usp, dapi_g );

  /* assumes this function is only called w/ physical and LAG ports */
  if ( IS_PORT_TYPE_PHYSICAL( dapiPortPtr ) )
  {
    if ( hapiPortPtr->hapiModeparm.physical.isMemberOfLag != L7_TRUE )
    {
      rc = hapiBroadDynamicArpInspectionPortUpdate(usp,
                                                   oldTrusted,
                                                   newTrusted,
                                                   dapi_g);
    }
  }
  else if ( IS_PORT_TYPE_LOGICAL_LAG( dapiPortPtr ) )
  {
    hapiBroadLagCritSecEnter();

    for ( i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++ )
    {
      if ( L7_TRUE == dapiPortPtr->modeparm.lag.memberSet[i].inUse )
      {
        rc = hapiBroadDynamicArpInspectionPortUpdate(&dapiPortPtr->modeparm.lag.memberSet[i].usp,
                                                     oldTrusted,
                                                     newTrusted,
                                                     dapi_g);
      }
    }
    hapiBroadLagCritSecExit();
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Used to configure Dynamic Arp Inspection on a port.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DYNAMIC_ARP_INSPECTION_CONFIG
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dynamicArpInspectionConfig
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDynamicArpInspectionConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;

  if (dapiCmd->cmdData.dynamicArpInspectionConfig.getOrSet != DAPI_CMD_SET)
    return L7_FAILURE;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if ( ( L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ) ) &&
       ( L7_FALSE == IS_PORT_TYPE_LOGICAL_LAG( dapiPortPtr ) ) )
  {
    return( L7_FAILURE );
  }

  result = hapiBroadDynamicArpInspectionSet( usp,
                                             hapiPortPtr->dynamicArpInspectionTrusted,
                                             dapiCmd->cmdData.dynamicArpInspectionConfig.trusted,
                                             dapi_g );
  if (result == L7_SUCCESS)
  {
    hapiPortPtr->dynamicArpInspectionTrusted = dapiCmd->cmdData.dynamicArpInspectionConfig.trusted;
  }

  return( result );
}

/*********************************************************************
*
* @purpose Notifies the DynamicArpInspection component that a physical port has been
*          added to a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDynamicArpInspectionLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  BROAD_PORT_t   *hapiPortPtr;
  BROAD_PORT_t   *hapiLagPtr;

  hapiPortPtr    = HAPI_PORT_GET(portUsp, dapi_g);
  hapiLagPtr     = HAPI_PORT_GET(lagUsp,  dapi_g);

  rc = hapiBroadDynamicArpInspectionPortUpdate(portUsp,
                                               hapiPortPtr->dynamicArpInspectionTrusted,
                                               hapiLagPtr->dynamicArpInspectionTrusted,
                                               dapi_g);

  return rc;
}

/*********************************************************************
*
* @purpose Notifies the DynamicArpInspection component that a physical port has been
*          removed from a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDynamicArpInspectionLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  BROAD_PORT_t   *hapiPortPtr;
  BROAD_PORT_t   *hapiLagPtr;

  hapiPortPtr    = HAPI_PORT_GET(portUsp, dapi_g);
  hapiLagPtr     = HAPI_PORT_GET(lagUsp,  dapi_g);

  rc = hapiBroadDynamicArpInspectionPortUpdate(portUsp,
                                               hapiLagPtr->dynamicArpInspectionTrusted,
                                               hapiPortPtr->dynamicArpInspectionTrusted,
                                               dapi_g);

  return rc;
}

/*********************************************************************
*
* @purpose Configure the Captive Portal parameters on a port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_CAPTIVE_PORTAL_CONFIG
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes  1) CP can be enabled on Physical ports only. If the port
*            becomes a Lag member, it loses it's CP config. The CP
*            configuration is restored by the app. once the port
*            is removed from the Lag.
*         2) Assumes CP is not be enabled with Dot1x and Port-Mac locking.
*         3) As client's get authenticated, Application adds the
*            client's mac-address statically using
*            DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_ADD.
*
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfCaptivePortalConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 result       = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t   *dapiCmd      = (DAPI_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t           *hapiPortPtr;
  DAPI_PORT_t            *dapiPortPtr;
  BROAD_L2ADDR_FLUSH_t    flushReq;

  if (dapiCmd->cmdData.captivePortalPortState.getOrSet != DAPI_CMD_SET)
    return L7_FAILURE;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  switch (dapiCmd->cmdData.captivePortalPortState.cpState)
  {
    case DAPI_PORT_CAPTIVE_PORTAL_ENABLED:

      hapiPortPtr->cpEnabled = L7_TRUE;

      /* Send Unknown SA to cpu */
      hapiBroadLearnSet(usp, BCM_PORT_LEARN_CPU, dapi_g);

      /* Flush all the dynamic mac-addresses */
      flushReq.vlanID = 0;
      flushReq.tgid = 0;
      flushReq.flushtype = BROAD_FLUSH_BY_PORT;
      flushReq.bcmx_lport = hapiPortPtr->bcmx_lport;
      flushReq.port_is_lag = L7_FALSE;
      hapiBroadL2FlushRequest(flushReq);

      break;

    case DAPI_PORT_CAPTIVE_PORTAL_DISABLED:

      hapiPortPtr->cpBlocked = L7_FALSE;
      hapiPortPtr->cpEnabled = L7_FALSE;

      /* enable learning on this port */
      if (hpcSoftwareLearningEnabled () == L7_TRUE)
        hapiBroadLearnSet(usp, (BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_FWD), dapi_g);
      else
        hapiBroadLearnSet(usp, (BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD), dapi_g);

      break;

    case DAPI_PORT_CAPTIVE_PORTAL_BLOCKED:

      hapiPortPtr->cpBlocked = L7_TRUE;
      /* Configure hw to drop all the unknown SA's. Application
      ** will flush all the authenticated client's static
      **  mac-addresses.
      */
      hapiBroadLearnSet(usp, 0, dapi_g);

      break;

    case DAPI_PORT_CAPTIVE_PORTAL_UNBLOCKED:

      hapiPortPtr->cpBlocked = L7_FALSE;

      if (hapiPortPtr->cpEnabled)
      {
        hapiBroadLearnSet(usp, BCM_PORT_LEARN_CPU, dapi_g);
      }
      else
      {
        /* enable learning on this port */
        if (hpcSoftwareLearningEnabled () == L7_TRUE)
          hapiBroadLearnSet(usp, (BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_FWD), dapi_g);
        else
          hapiBroadLearnSet(usp, (BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD), dapi_g);
      }

      break;

    default:
      result = L7_FAILURE;
      break;
  }

  return result;
}


void hapiBroadDebugFlushStats(void)
{
  sysapiPrintf("Max Number of Flushes in a single pass of the task = %d\n",
                hapiBroadFlushStats_g.hapiBroadMaxFlushesInLoop);
  sysapiPrintf("Number of Trunk Flushes issued    = %d\n",
                hapiBroadFlushStats_g.hapiBroadTrunkFlushesIssued);
  sysapiPrintf("Number of Port Flushes issued     = %d\n",
                hapiBroadFlushStats_g.hapiBroadPortFlushesIssued);
  sysapiPrintf("Number of Port Flushes done     = %d\n",
                hapiBroadFlushStats_g.hapiBroadPortFlushesDone);
  sysapiPrintf("Number of Vlan Flushes issued     = %d\n",
                hapiBroadFlushStats_g.hapiBroadVlanFlushesIssued);
  sysapiPrintf("Number of Invalid Flushes issued  = %d\n",
                hapiBroadFlushStats_g.hapiBroadInvalidFlushesIssued);
}


/*********************************************************************
*
* @purpose  Used to configure Given outer TPID on the port
*           accordingly.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.doubleVlanTagConfig
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDoubleVlanTagMultiTpidConfig(DAPI_USP_t *usp,
                         DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_SUCCESS;
  DAPI_PORT_t             *dapiPortPtr;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t *)data;
  BROAD_PORT_t            *hapiPortPtr,*hapiLagMemberPortPtr;
  L7_uint32               i;
  int                     rv = BCM_E_NONE;

  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);

  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    hapiBroadLagCritSecEnter ();
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
      {
        hapiLagMemberPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);
        if (dapiCmd->cmdData.doubleVlanTagConfig.getOrSet == DAPI_CMD_SET)
        {
          if (dapiCmd->cmdData.doubleVlanTagConfig.enable == L7_TRUE)
          {
            rv = usl_bcmx_port_tpid_add(hapiLagMemberPortPtr->bcmx_lport, dapiCmd->cmdData.doubleVlanTagConfig.etherType);
          }

          else
          {
            rv = usl_bcmx_port_tpid_delete(hapiLagMemberPortPtr->bcmx_lport, dapiCmd->cmdData.doubleVlanTagConfig.etherType);
          }
          if ( (L7_BCMX_OK(rv) != L7_TRUE) &&
               (rv != BCM_E_UNAVAIL)
              )
          {
            break;
          }
        }
      }
    }
    hapiBroadLagCritSecExit ();
  }
  else if(IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)/*Physical Ports */
  {
    if (dapiCmd->cmdData.doubleVlanTagConfig.getOrSet == DAPI_CMD_SET)
    {
      if (dapiCmd->cmdData.doubleVlanTagConfig.enable == L7_TRUE)
      {
        rv = usl_bcmx_port_tpid_add(hapiPortPtr->bcmx_lport, dapiCmd->cmdData.doubleVlanTagConfig.etherType);
      }
      else
      {
        rv = usl_bcmx_port_tpid_delete(hapiPortPtr->bcmx_lport, dapiCmd->cmdData.doubleVlanTagConfig.etherType);
      }
    }
  }

  if ( (L7_BCMX_OK(rv) != L7_TRUE) &&
       (rv != BCM_E_UNAVAIL)
     )
  {
    result = L7_FAILURE;
  }
  else
  {
    result = L7_SUCCESS;
  }
  return result;
}


/*********************************************************************
*
* @purpose  Used to configure Given outer TPID on the port
*           accordingly.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.doubleVlanTagConfig
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDoubleVlanTagDefaultTpidConfig(DAPI_USP_t *usp,
                         DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t *)data;
  int                      rv;

  if (dapiCmd->cmdData.doubleVlanTagConfig.getOrSet == DAPI_CMD_SET)
  {
    hapiBroadDvlanEthertype = dapiCmd->cmdData.doubleVlanTagConfig.etherType;

    /* Allow TPID to set as per application requirements. Do not cross-check
     * whether DVLAN is enabled or not (dtagPortCount).
     */
    rv = usl_bcmx_dvlan_default_tpid_set(hapiBroadDvlanEthertype);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      result = L7_FAILURE;
    }
  }
  return result;
}

