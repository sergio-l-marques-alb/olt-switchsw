/**
 * ptin_igmp.c 
 *  
 * Implements the IGMP Proxy interface module
 *
 * Created on: 2011/09/19
 * Author: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

#include <unistd.h>

#include "ptin_igmp.h"
#include "ptin_xlate_api.h"
#include "ptin_intf.h"
#include "ptin_utils.h"
#include "ptin_evc.h"
#include "usmdb_snooping_api.h"
#include "snooping_api.h"
#include "avl_api.h"
#include "buff_api.h"
#include "l7apptimer_api.h"
#include "l7handle_api.h"

#define IGMP_INVALID_ENTRY    0xFF

/* Comment the follwing line, if you don't want to use client timers */
#define CLIENT_TIMERS_SUPPORTED

L7_BOOL ptin_debug_igmp_snooping = 0;

void ptin_debug_igmp_enable(L7_BOOL enable)
{
  ptin_debug_igmp_snooping = enable;
}

/* Parameters to identify the client */
#define MC_CLIENT_INTERF_SUPPORTED    1
#define MC_CLIENT_OUTERVLAN_SUPPORTED 0
#define MC_CLIENT_INNERVLAN_SUPPORTED 1
#define MC_CLIENT_IPADDR_SUPPORTED    0
#define MC_CLIENT_MACADDR_SUPPORTED   1

/* At least one parameter must be active */
#if ( !( MC_CLIENT_INTERF_SUPPORTED    |  \
         MC_CLIENT_OUTERVLAN_SUPPORTED |  \
         MC_CLIENT_INNERVLAN_SUPPORTED |  \
         MC_CLIENT_IPADDR_SUPPORTED    |  \
         MC_CLIENT_MACADDR_SUPPORTED ) )
  #error "ptin_igmp.c: At least one parameter must be defined!"
#endif

/* To validate mask according to the suuported parameters */
#define MC_CLIENT_MASK_UPDATE(mask) \
        ( ( (PTIN_CLIENT_MASK_FIELD_INTF      & MC_CLIENT_INTERF_SUPPORTED   ) |   \
            (PTIN_CLIENT_MASK_FIELD_OUTERVLAN & MC_CLIENT_OUTERVLAN_SUPPORTED) |   \
            (PTIN_CLIENT_MASK_FIELD_INNERVLAN & MC_CLIENT_INNERVLAN_SUPPORTED) |   \
            (PTIN_CLIENT_MASK_FIELD_IPADDR    & MC_CLIENT_IPADDR_SUPPORTED   ) |   \
            (PTIN_CLIENT_MASK_FIELD_MACADDR   & MC_CLIENT_MACADDR_SUPPORTED  ) ) & (mask) )


/*********************************************************** 
 * Typedefs
 ***********************************************************/

typedef struct
{
  #if (MC_CLIENT_INTERF_SUPPORTED)
  L7_uint8  ptin_port;                /* PTin port, which is attached */
  #endif
  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
  L7_uint16 outerVlan;                /* Outer Vlan */
  #endif
  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
  L7_uint16 innerVlan;                /* Inner Vlan */
  #endif
  #if (MC_CLIENT_IPADDR_SUPPORTED)
  L7_uint32 ipv4_addr;                /* IP address */
  #endif
  #if (MC_CLIENT_MACADDR_SUPPORTED)
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN]; /* Source MAC */
  #endif
} ptinIgmpClientDataKey_t;

typedef struct
{
  ptinIgmpClientDataKey_t   igmpClientDataKey;
  L7_uint16                 client_index;
  ptin_IGMP_Statistics_t    stats_client;  /* Client statistics */
  L7_BOOL                   isDynamic;
  //L7_APP_TMR_HNDL_t         timer;
  //L7_uint32                 timerHandle;
  void *next;
} ptinIgmpClientInfoData_t;

typedef struct {
    avlTree_t                 igmpClientsAvlTree;
    avlTreeTables_t           *igmpClientsTreeHeap;
    ptinIgmpClientInfoData_t  *igmpClientsDataHeap;
} ptinIgmpClientsAvlTree_t;

/* IGMP AVL Tree data */
typedef struct {
  L7_uint16 number_of_clients;
  ptinIgmpClientInfoData_t *clients_in_use[PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE];
  ptinIgmpClientsAvlTree_t  avlTree;
  L7_APP_TMR_CTRL_BLK_t     timerCB;       /* Entry App Timer Control Block */

  L7_sll_t                  ll_timerList;  /* Linked list of timer data nodes */
  L7_uint32                 appTimerBufferPoolId;
  handle_list_t            *appTimer_handle_list;
  void                     *appTimer_handleListMemHndl;
} ptinIgmpClients_t;


/* IGMP Instance config struct
 * IMPORTANT:
 *   1. only ONE root is allowed per instance
 *   2. only ONE vlan is allowed for all client interfaces
 *   3. the unicast VLAN cannot be used on multiple IGMP instances */ 
typedef struct {
  L7_BOOL   inUse;
  L7_uint16 McastEvcId;
  L7_uint16 UcastEvcId;
  ptinIgmpClients_t igmpClients;
  ptin_IGMP_Statistics_t stats_intf[PTIN_SYSTEM_N_INTERF];  /* IGMP statistics at interface level */
} st_IgmpInstCfg_t;

/* IGMP instances array
 * NOTE: each instance is uniquely identified by the router internal VLAN.
 * Notice that the external router VLAN (or other VLANs) can be used on
 * other instances on different interfaces. The uniqueness is guaranteed
 * by the interval VLANs. */ 
st_IgmpInstCfg_t  igmpInstances[PTIN_SYSTEM_N_IGMP_INSTANCES];

/* Reference of evcid using internal vlan as reference */
static L7_uint8 igmpInst_fromEvcId[PTIN_SYSTEM_N_EVCS];

/* Configuration structures */
ptin_IgmpProxyCfg_t igmpProxyCfg;

/* Arrays to map the number of IGMP instances that use each interface */
L7_uint8 igmpClientsIntf[PTIN_SYSTEM_N_INTERF];
L7_uint8 igmpRoutersIntf[PTIN_SYSTEM_N_INTERF];

/* Global IGMP statistics at interface level */
ptin_IGMP_Statistics_t global_stats_intf[PTIN_SYSTEM_N_INTERF];

/* Lookup tables (invalid entry: 0xFF - IGMP_INVALID_ENTRY)
 * IMPORTANT:
 *   These indexed VLANs are the ones after translation (internal VLANs).
 *   The main reason is that there might be multiple IGMP instances that use
 *   the same VLANs, but on different interfaces. Only the interval VLAN will
 *   differenciate them. */ 
L7_uint8 igmpInst_fromRouterVlan[4096];  /* Lookup table to get IGMP instance index based on Router (root) VLAN */
L7_uint8 igmpInst_fromUCVlan[4096];      /* Lookup table to get IGMP instance index based on Unicast (clients uplink) VLAN */

/* Semaphore to access IGMP stats */
void *ptin_igmp_stats_sem = L7_NULLPTR;


/*********************************************************** 
 * INLINE FUNCTIONS
 ***********************************************************/

inline L7_int igmp_clientIndex_get_new(L7_uint8 igmp_idx)
{
  L7_uint i;

  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
    return -1;

  /* Search for the first free client index */
  for (i=0; i<PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE && igmpInstances[igmp_idx].igmpClients.clients_in_use[i]!=L7_NULLPTR; i++);

  if (i>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
    return -1;
  return i;
}

inline void igmp_clientIndex_mark(L7_uint8 igmp_idx, L7_uint client_idx, ptinIgmpClientInfoData_t *infoData)
{
  ptinIgmpClients_t *clients;

  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES || client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
    return;

  clients = &igmpInstances[igmp_idx].igmpClients;

  if (clients->clients_in_use[client_idx]==L7_NULLPTR && clients->number_of_clients<PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
    clients->number_of_clients++;

  clients->clients_in_use[client_idx] = infoData;
}

inline void igmp_clientIndex_unmark(L7_uint8 igmp_idx, L7_uint client_idx)
{
  ptinIgmpClients_t *clients;

  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES || client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
    return;

  clients = &igmpInstances[igmp_idx].igmpClients;

  if (clients->clients_in_use[client_idx]!=L7_NULLPTR && clients->number_of_clients>0)
    clients->number_of_clients--;

  clients->clients_in_use[client_idx] = L7_NULLPTR;
}

inline void igmp_clientIndex_clearAll(L7_uint8 igmp_idx)
{
  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
    return;

  memset(igmpInstances[igmp_idx].igmpClients.clients_in_use,0x00,sizeof(igmpInstances[igmp_idx].igmpClients.clients_in_use));
  igmpInstances[igmp_idx].igmpClients.number_of_clients = 0;
}

/* Semaphores */
void *igmp_sem = NULL;

#ifdef CLIENT_TIMERS_SUPPORTED

/* Semaphore for timers access */
void *ptin_igmp_timers_sem = L7_NULLPTR;

/* Timers variables */
L7_uint32 clientsMngmt_TaskId = L7_ERROR;
void     *clientsMngmt_queue  = L7_NULLPTR;

typedef struct ptinIgmpTimerParams_s
{
  L7_uint32          igmp_idx;
} ptinIgmpTimerParams_t;
#define PTIN_IGMP_TIMER_MSG_SIZE  sizeof(ptinIgmpTimerParams_t)

typedef struct igmpTimerData_s
{
  L7_sll_member_t   *next;
  L7_uint32         igmp_idx;
  L7_uint32         client_idx;

  L7_uchar8         timerType;
  L7_APP_TMR_HNDL_t timer;
  L7_uint32         timerHandle;
} igmpTimerData_t;

L7_RC_t ptin_igmp_timersMng_init(void);
L7_RC_t ptin_igmp_timersMng_deinit(void);
L7_RC_t ptin_igmp_timer_start (L7_uint igmp_idx, L7_uint32 client_idx);
L7_RC_t ptin_igmp_timer_update(L7_uint igmp_idx, L7_uint32 client_idx);
L7_RC_t ptin_igmp_timer_stop  (L7_uint igmp_idx, L7_uint32 client_idx);

void igmp_timersMng_task(void);
void igmp_timerExpiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* ptrData);
void igmp_timer_expiry(void *param);
L7_RC_t igmp_timer_dataDestroy (L7_sll_member_t *ll_member);
L7_int32 igmp_timer_dataCmp(void *p, void *q, L7_uint32 key);
#endif

/* Local functions prototypes */
static L7_RC_t ptin_igmp_new_client(L7_uint igmp_idx, ptin_client_id_t *client, L7_BOOL isDynamic, L7_uint *client_idx_ret);
static L7_RC_t ptin_igmp_rm_client(L7_uint igmp_idx, ptin_client_id_t *client, L7_BOOL remove_static);
static L7_RC_t ptin_igmp_rm_all_clients(L7_uint igmp_idx, L7_BOOL isDynamic, L7_BOOL only_wo_channels);
static L7_RC_t ptin_igmp_rm_clientIdx(L7_uint igmp_idx, L7_uint client_idx, L7_BOOL remove_static, L7_BOOL force_remove);

static L7_RC_t ptin_igmp_global_configuration(void);
static L7_RC_t ptin_igmp_trap_configure(L7_uint igmp_idx, L7_BOOL enable);
static L7_RC_t ptin_igmp_querier_configure(L7_uint igmp_idx, L7_BOOL enable);
static L7_RC_t ptin_igmp_instance_deleteAll_clients(L7_uint igmp_idx);
static L7_RC_t ptin_igmp_inst_get_fromIntVlan(L7_uint16 intVlan, st_IgmpInstCfg_t **igmpInst, L7_uint *igmpInst_idx);
//static L7_RC_t ptin_igmp_inst_validate(ptin_IgmpInstCfg_t *igmpInst, L7_uint *idx);
static L7_RC_t ptin_igmp_instance_find_free(L7_uint *idx);
static L7_RC_t ptin_igmp_instance_find(L7_uint16 McastEvcId, L7_uint16 UcastEvcId, L7_uint *igmp_idx);
static L7_RC_t ptin_igmp_instance_find_fromSingleEvcId(L7_uint16 evcId, L7_uint *igmp_idx);
static L7_RC_t ptin_igmp_instance_find_fromMcastEvcId(L7_uint16 McastEvcId, L7_uint *igmp_idx);
static L7_BOOL ptin_igmp_instance_conflictFree(L7_uint16 McastEvcId, L7_uint16 UcastEvcId);

static L7_RC_t ptin_igmp_client_find(L7_uint igmp_idx, ptin_client_id_t *client_ref, ptinIgmpClientInfoData_t **client_info);
//static L7_RC_t ptin_igmp_router_intf_config(L7_uint router_intf, L7_uint16 router_vlan, L7_uint admin);
//static L7_RC_t ptin_igmp_clients_intf_config(L7_uint client_intf, L7_uint admin);

/**
 * Initializes IGMP module
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_proxy_init(void)
{
  L7_uint igmp_idx;
  ptinIgmpClientsAvlTree_t *avlTree;

  /* Reset instances array */
  memset(&igmpProxyCfg, 0x00, sizeof(igmpProxyCfg));
  memset(igmpInstances, 0x00, sizeof(igmpInstances));

  /* Initialize interfaces lists */
  memset(igmpClientsIntf, 0x00, sizeof(igmpClientsIntf));
  memset(igmpRoutersIntf, 0x00, sizeof(igmpRoutersIntf));

  /* Initialize lookup tables */
  memset(igmpInst_fromEvcId, 0xFF, sizeof(igmpInst_fromEvcId));
  memset(igmpInst_fromRouterVlan, 0xFF, sizeof(igmpInst_fromRouterVlan));
  memset(igmpInst_fromUCVlan, 0xFF, sizeof(igmpInst_fromUCVlan));

  /* Initialize global IGMP statistics */
  memset(global_stats_intf,0x00,sizeof(global_stats_intf));

  /* Initialize sempahore */
  igmp_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (igmp_sem == NULL)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Error creating a mutex for IGMP module");
    return L7_FAILURE;
  }

  /* Initialize AVL trees */
  for (igmp_idx=0; igmp_idx<PTIN_SYSTEM_N_IGMP_INSTANCES; igmp_idx++)
  {
    avlTree = &igmpInstances[igmp_idx].igmpClients.avlTree;

    igmp_clientIndex_clearAll(igmp_idx);

    avlTree->igmpClientsTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE * sizeof(avlTreeTables_t)); 
    avlTree->igmpClientsDataHeap = (ptinIgmpClientInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE * sizeof(ptinIgmpClientInfoData_t)); 

    if ((avlTree->igmpClientsTreeHeap == L7_NULLPTR) ||
        (avlTree->igmpClientsDataHeap == L7_NULLPTR))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error allocating data for IGMP AVL Trees\n");
      return L7_FAILURE;
    }

    /* Initialize the storage for all the AVL trees */
    memset (&avlTree->igmpClientsAvlTree, 0x00, sizeof(avlTree_t));
    memset (avlTree->igmpClientsTreeHeap, 0x00, sizeof(avlTreeTables_t)*PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE);
    memset (avlTree->igmpClientsDataHeap, 0x00, sizeof(ptinIgmpClientInfoData_t)*PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE);

    // AVL Tree creations - snoopIpAvlTree
    avlCreateAvlTree(&(avlTree->igmpClientsAvlTree),
                     avlTree->igmpClientsTreeHeap,
                     avlTree->igmpClientsDataHeap,
                     PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE, 
                     sizeof(ptinIgmpClientInfoData_t),
                     0x10,
                     sizeof(ptinIgmpClientDataKey_t));
  }

  ptin_igmp_stats_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_igmp_stats_sem == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to create ptin_igmp_stats_sem semaphore!");
    return L7_FAILURE;
  }

#ifdef CLIENT_TIMERS_SUPPORTED
  /* Timers init */
  if (ptin_igmp_timersMng_init()!=L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to initialize timers!");
    return L7_FAILURE;
  }
#endif

  LOG_INFO(LOG_CTX_PTIN_IGMP, "IGMP init OK");

  return L7_SUCCESS;
}

/**
 * Deinitializes IGMP module
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_proxy_deinit(void)
{
  L7_uint igmp_idx;
  ptinIgmpClientsAvlTree_t *avlTree;

#ifdef CLIENT_TIMERS_SUPPORTED
  /* Timers init */
  ptin_igmp_timersMng_deinit();
#endif

  /* Initialize AVL trees */
  for (igmp_idx=0; igmp_idx<PTIN_SYSTEM_N_IGMP_INSTANCES; igmp_idx++)
  {
    avlTree = &igmpInstances[igmp_idx].igmpClients.avlTree;

    igmp_clientIndex_clearAll(igmp_idx);

    // AVL Tree creations - snoopIpAvlTree
    avlPurgeAvlTree(&(avlTree->igmpClientsAvlTree),PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE);

    osapiFree(L7_PTIN_COMPONENT_ID, avlTree->igmpClientsTreeHeap); 
    osapiFree(L7_PTIN_COMPONENT_ID, avlTree->igmpClientsDataHeap); 
    avlTree->igmpClientsTreeHeap = L7_NULLPTR;
    avlTree->igmpClientsDataHeap = L7_NULLPTR;
  }

  osapiSemaDelete(ptin_igmp_stats_sem);
  ptin_igmp_stats_sem = L7_NULLPTR;

  LOG_INFO(LOG_CTX_PTIN_IGMP, "IGMP deinit OK");

  return L7_SUCCESS;
}

/**
 * Load IGMP proxy default configuraion parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_proxy_defaultcfg_load(void)
{
  ptin_IgmpProxyCfg_t igmpProxy;
  L7_RC_t rc;

  igmpProxy.mask                                   = 0xFF;
  igmpProxy.admin                                  = 0;
  igmpProxy.version                                = PTIN_IGMP_DEFAULT_VERSION;
  igmpProxy.ipv4_addr.s_addr                       = PTIN_IGMP_DEFAULT_IPV4;
  igmpProxy.igmp_cos                               = PTIN_IGMP_DEFAULT_COS;
  igmpProxy.fast_leave                             = PTIN_IGMP_DEFAULT_FASTLEAVEMODE;

  igmpProxy.querier.mask                           = 0xFFFF;
  igmpProxy.querier.flags                          = 0;
  igmpProxy.querier.version                        = PTIN_IGMP_DEFAULT_VERSION;
  igmpProxy.querier.robustness                     = PTIN_IGMP_DEFAULT_ROBUSTNESS;
  igmpProxy.querier.query_interval                 = PTIN_IGMP_DEFAULT_QUERYINTERVAL;
  igmpProxy.querier.query_response_interval        = PTIN_IGMP_DEFAULT_QUERYRESPONSEINTERVAL;
  igmpProxy.querier.group_membership_interval      = PTIN_IGMP_DEFAULT_GROUPMEMBERSHIPINTERVAL;
  igmpProxy.querier.other_querier_present_interval = PTIN_IGMP_DEFAULT_OTHERQUERIERPRESENTINTERVAL;
  igmpProxy.querier.startup_query_interval         = PTIN_IGMP_DEFAULT_STARTUPQUERYINTERVAL;
  igmpProxy.querier.startup_query_count            = PTIN_IGMP_DEFAULT_STARTUPQUERYCOUNT;
  igmpProxy.querier.last_member_query_interval     = PTIN_IGMP_DEFAULT_LASTMEMBERQUERYINTERVAL;
  igmpProxy.querier.last_member_query_count        = PTIN_IGMP_DEFAULT_LASTMEMBERQUERYCOUNT;
  igmpProxy.querier.older_host_present_timeout     = PTIN_IGMP_DEFAULT_OLDERHOSTPRESENTTIMEOUT;

  igmpProxy.host.mask                              = 0xFF;
  igmpProxy.host.flags                             = 0;
  igmpProxy.host.version                           = PTIN_IGMP_DEFAULT_VERSION;
  igmpProxy.host.robustness                        = PTIN_IGMP_DEFAULT_ROBUSTNESS;
  igmpProxy.host.unsolicited_report_interval       = PTIN_IGMP_DEFAULT_UNSOLICITEDREPORTINTERVAL;
  igmpProxy.host.older_querier_present_timeout     = PTIN_IGMP_DEFAULT_OLDERQUERIERPRESENTTIMEOUT;

  /* Apply default config */
  rc = ptin_igmp_proxy_config_set(&igmpProxy);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "IGMP default config failed to be load");
    return L7_FAILURE;
  }

  LOG_INFO(LOG_CTX_PTIN_IGMP, "IGMP default config loaded OK");

  return L7_SUCCESS;
}

/**
 * Applies IGMP Proxy configuration
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_proxy_config_set(ptin_IgmpProxyCfg_t *igmpProxy)
{
  L7_RC_t rc;

  /* Get mutex to change configurations without interfering with normal operation */
  rc = osapiSemaTake(igmp_sem, L7_WAIT_FOREVER);
  if (rc == L7_FAILURE)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to get IGMP mutex");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Applying new config to IGMP Proxy...");

  /* *******************
   * IGMP general config
   * *******************/
  /* Output admin state, but only apply changes in the end... */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_ADMIN
      && igmpProxyCfg.admin != igmpProxy->admin)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "  Admin:                                   %s", igmpProxyCfg.admin != 0 ? "ON":"OFF");
  }

  /* Version */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_VERSION
      && igmpProxyCfg.version != igmpProxy->version)
  {
    igmpProxyCfg.version = igmpProxy->version;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "  IGMP Version:                            %u", igmpProxyCfg.version);
  }

  /* Proxy IP */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_IPV4
      && igmpProxyCfg.ipv4_addr.s_addr != igmpProxy->ipv4_addr.s_addr)
  {
    igmpProxyCfg.ipv4_addr.s_addr = igmpProxy->ipv4_addr.s_addr;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "  IPv4:                                    %u.%u.%u.%u",
              (igmpProxyCfg.ipv4_addr.s_addr >> 24) & 0xFF, (igmpProxyCfg.ipv4_addr.s_addr >> 16) & 0xFF,
              (igmpProxyCfg.ipv4_addr.s_addr >>  8) & 0xFF,  igmpProxyCfg.ipv4_addr.s_addr        & 0xFF);
  }

  /* Class-Of-Service (COS) */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_COS
      && igmpProxyCfg.igmp_cos != igmpProxy->igmp_cos)
  {
    igmpProxyCfg.igmp_cos = igmpProxy->igmp_cos;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "  IGMP COS:                                %u", igmpProxyCfg.igmp_cos);
  }

  /* Fast-Leave mode */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_FASTLEAVE
      && igmpProxyCfg.fast_leave != igmpProxy->fast_leave)
  {
    igmpProxyCfg.fast_leave = igmpProxy->fast_leave;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "  Fast-Leave mode:                         %s", igmpProxyCfg.fast_leave != 0 ? "ON":"OFF");
  }

  /* *******************
   * IGMP Querier config
   * *******************/
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "  Querier config:");

  /* Querier Version */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_VER
      && igmpProxyCfg.querier.version != igmpProxy->querier.version
      && !(igmpProxy->mask & PTIN_IGMP_PROXY_MASK_VERSION))
  {
    igmpProxyCfg.querier.version = igmpProxy->querier.version;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Version:                               %u", igmpProxyCfg.querier.version);
  }

  /* Querier Robustness */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_RV
      && igmpProxyCfg.querier.robustness != igmpProxy->querier.robustness)
  {
    igmpProxyCfg.querier.robustness = igmpProxy->querier.robustness;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Robustness:                            %u", igmpProxyCfg.querier.robustness);
  }

  /* Query Interval */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_QI
      && igmpProxyCfg.querier.query_interval != igmpProxy->querier.query_interval)
  {
    igmpProxyCfg.querier.query_interval = igmpProxy->querier.query_interval;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Query Interval:                        %u (s)", igmpProxyCfg.querier.query_interval);
  }

  /* Query Response Interval */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_QRI
      && igmpProxyCfg.querier.query_response_interval != igmpProxy->querier.query_response_interval)
  {
    igmpProxyCfg.querier.query_response_interval = igmpProxy->querier.query_response_interval;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Query Response Interval:               %u (1/10s)", igmpProxyCfg.querier.query_response_interval);
  }

  /* Group Membership Interval */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_GMI)
  {
    igmpProxyCfg.querier.group_membership_interval = PTIN_IGMP_AUTO_GMI(igmpProxyCfg.querier.robustness,
                                                                        igmpProxyCfg.querier.query_interval,
                                                                        igmpProxyCfg.querier.query_response_interval);
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Group Membership Interval (AUTO):      %u (s)", igmpProxyCfg.querier.group_membership_interval);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_GMI
           && igmpProxyCfg.querier.group_membership_interval != igmpProxy->querier.group_membership_interval)
  {
    igmpProxyCfg.querier.group_membership_interval = igmpProxy->querier.group_membership_interval;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Group Membership Interval:             %u (s)", igmpProxyCfg.querier.group_membership_interval);
  }

  /* Other Querier Present Interval */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_OQPI)
  {
    igmpProxyCfg.querier.other_querier_present_interval = PTIN_IGMP_AUTO_OQPI(igmpProxyCfg.querier.robustness,
                                                                              igmpProxyCfg.querier.query_interval,
                                                                              igmpProxyCfg.querier.query_response_interval);
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Other Querier Present Interval (AUTO): %u (s)", igmpProxyCfg.querier.other_querier_present_interval);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_OQPI
           && igmpProxyCfg.querier.other_querier_present_interval != igmpProxy->querier.other_querier_present_interval)
  {
    igmpProxyCfg.querier.other_querier_present_interval = igmpProxy->querier.other_querier_present_interval;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Other Querier Present Interval:        %u (s)", igmpProxyCfg.querier.other_querier_present_interval);
  }

  /* Startup Query Interval */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_SQI)
  {
    igmpProxyCfg.querier.startup_query_interval = PTIN_IGMP_AUTO_SQI(igmpProxyCfg.querier.query_interval);
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Startup Query Interval (AUTO):         %u (s)", igmpProxyCfg.querier.startup_query_interval);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_SQI
           && igmpProxyCfg.querier.startup_query_interval != igmpProxy->querier.startup_query_interval)
  {
    igmpProxyCfg.querier.startup_query_interval = igmpProxy->querier.startup_query_interval;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Startup Query Interval:                %u (s)", igmpProxyCfg.querier.startup_query_interval);
  }

  /* Startup Query Count */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_SQC)
  {
    igmpProxyCfg.querier.startup_query_count = PTIN_IGMP_AUTO_SQC(igmpProxyCfg.querier.robustness);
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Startup Query Count (AUTO):            %u (s)", igmpProxyCfg.querier.startup_query_count);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_SQC
           && igmpProxyCfg.querier.startup_query_count != igmpProxy->querier.startup_query_count)
  {
    igmpProxyCfg.querier.startup_query_count = igmpProxy->querier.startup_query_count;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Startup Query Count:                   %u (s)", igmpProxyCfg.querier.startup_query_count);
  }

  /* Last Member Query Interval */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_LMQI
      && igmpProxyCfg.querier.last_member_query_interval != igmpProxy->querier.last_member_query_interval)
  {
    igmpProxyCfg.querier.last_member_query_interval = igmpProxy->querier.last_member_query_interval;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Last Member Query Interval:            %u (1/10s)", igmpProxyCfg.querier.last_member_query_interval);
  }

  /* Last Member Query Count */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_LMQC)
  {
    igmpProxyCfg.querier.last_member_query_count = PTIN_IGMP_AUTO_LMQC(igmpProxyCfg.querier.robustness);
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Last Member Query Count (AUTO):        %u (s)", igmpProxyCfg.querier.last_member_query_count);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_LMQC
           && igmpProxyCfg.querier.last_member_query_count != igmpProxy->querier.last_member_query_count)
  {
    igmpProxyCfg.querier.last_member_query_count = igmpProxy->querier.last_member_query_count;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Last Member Query Count:               %u (s)", igmpProxyCfg.querier.last_member_query_count);
  }

  /* Older Host Present Timeout */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_LMQC)
  {
    igmpProxyCfg.querier.older_host_present_timeout = PTIN_IGMP_AUTO_OHPT(igmpProxyCfg.querier.robustness,
                                                                          igmpProxyCfg.querier.query_interval,
                                                                          igmpProxyCfg.querier.query_response_interval);
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Older Host Present Timeout (AUTO):     %u (s)", igmpProxyCfg.querier.older_host_present_timeout);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_LMQC
           && igmpProxyCfg.querier.older_host_present_timeout != igmpProxy->querier.older_host_present_timeout)
  {
    igmpProxyCfg.querier.older_host_present_timeout = igmpProxy->querier.older_host_present_timeout;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Older Host Present Timeout:            %u (s)", igmpProxyCfg.querier.older_host_present_timeout);
  }

  /* *******************
   * IGMP Host config
   * *******************/
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "  Host config:");

  /* Host Version */
  if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_VER
      && igmpProxyCfg.host.version != igmpProxy->host.version
      && !(igmpProxy->mask & PTIN_IGMP_PROXY_MASK_VERSION))
  {
    igmpProxyCfg.host.version = igmpProxy->host.version;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Version:                               %u", igmpProxyCfg.host.version);
  }

  /* Host Robustness */
  if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_RV
      && igmpProxyCfg.host.robustness != igmpProxy->host.robustness)
  {
    igmpProxyCfg.host.robustness = igmpProxy->host.robustness;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Robustness:                            %u", igmpProxyCfg.host.robustness);
  }

  /* Unsolicited Report Interval */
  if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_URI
      && igmpProxyCfg.host.unsolicited_report_interval != igmpProxy->host.unsolicited_report_interval)
  {
    igmpProxyCfg.host.unsolicited_report_interval = igmpProxy->host.unsolicited_report_interval;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Unsolicited Report Interval:           %u (s)", igmpProxyCfg.host.unsolicited_report_interval);
  }

  /* Older Querier Present Timeout */
  if (igmpProxy->host.flags & PTIN_IGMP_HOST_MASK_OQPT)
  {
    igmpProxyCfg.host.older_querier_present_timeout = PTIN_IGMP_AUTO_OQPT(igmpProxyCfg.host.robustness,
                                                                          igmpProxyCfg.querier.query_interval,
                                                                          igmpProxyCfg.querier.query_response_interval);
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Older Querier Present Timeout (AUTO):  %u (s)", igmpProxyCfg.host.older_querier_present_timeout);
  }
  else if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_OQPT
           && igmpProxyCfg.host.older_querier_present_timeout != igmpProxy->host.older_querier_present_timeout)
  {
    igmpProxyCfg.host.older_querier_present_timeout = igmpProxy->host.older_querier_present_timeout;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Older Querier Present Timeout:         %u (s)", igmpProxyCfg.host.older_querier_present_timeout);
  }

  /* Update AUTO flags */
  igmpProxyCfg.querier.flags = igmpProxy->querier.flags;
  igmpProxyCfg.host.flags    = igmpProxy->host.flags;

  /* Finally, (de)activate IGMP module */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_ADMIN
      && igmpProxyCfg.admin != igmpProxy->admin)
  {
    // TODO apply changes
//  rc = snoopIGMPAdminModeApply();

    igmpProxyCfg.admin = igmpProxy->admin;
  }

  /* Global configuration */
  if (ptin_igmp_global_configuration()!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error applying configurations");
    osapiSemaGive(igmp_sem);
    return L7_FAILURE;
  }

  osapiSemaGive(igmp_sem);

  return L7_SUCCESS;
}

/**
 * Gets IGMP Proxy configuration
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_proxy_config_get(ptin_IgmpProxyCfg_t *igmpProxy)
{
  *igmpProxy = igmpProxyCfg;

  igmpProxy->mask         = 0xFF;
  igmpProxy->querier.mask = 0xFFFF;
  igmpProxy->host.mask    = 0xFF;

  return L7_SUCCESS;
}

/**
 * Check if a EVC is being used in an IGMP instance
 * 
 * @param evcId : evc id
 * 
 * @return L7_RC_t : L7_TRUE or L7_FALSE
 */
L7_RC_t ptin_igmp_is_evc_used(L7_uint16 evcId)
{
  /* Validate arguments */
  if (evcId>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid EVC id: evcId=%u",evcId);
    return L7_FALSE;
  }

  /* This evc must be active */
  if (!ptin_evc_is_in_use(evcId))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC id is not active: evcId=%u",evcId);
    return L7_FALSE;
  }

  /* Check if this EVC is being used by any igmp instance */
  if (ptin_igmp_instance_find_fromSingleEvcId(evcId,L7_NULLPTR)!=L7_SUCCESS)
    return L7_FALSE;

  return L7_TRUE;
}

/**
 * Creates an IGMP instance
 * 
 * @param McastEvcId : Multicast evc id 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_instance_add(L7_uint16 McastEvcId, L7_uint16 UcastEvcId)
{
  L7_uint igmp_idx;

  /* Validate arguments */
  if (McastEvcId>=PTIN_SYSTEM_N_EVCS || UcastEvcId>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid EVC ids: [mcEvcId,ucEvcId]=[%u,%u]",McastEvcId,UcastEvcId);
    return L7_FAILURE;
  }

  /* These evcs must be active */
  if (!ptin_evc_is_in_use(McastEvcId) || !ptin_evc_is_in_use(UcastEvcId))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC ids are not active: [mcEvcId,ucEvcId]=[%u,%u]",McastEvcId,UcastEvcId);
    return L7_FAILURE;
  }

  /* Check if there is an instance with these parameters */
  if (ptin_igmp_instance_find(McastEvcId,UcastEvcId,L7_NULLPTR)==L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"There is already an instance with [mcEvcId,ucEvcId]=[%u,%u]",McastEvcId,UcastEvcId);
    return L7_SUCCESS;
  }

  /* Check if there is any conflict with the existent IGMP instances */
  if (!ptin_igmp_instance_conflictFree(McastEvcId,UcastEvcId))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"There is conflict with these parameters: [mcEvcId,ucEvcId]=[%u,%u]",McastEvcId,UcastEvcId);
    return L7_FAILURE;
  }

  /* Find an empty instance to be used */
  if (ptin_igmp_instance_find_free(&igmp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"There is no free instances to be used");
    return L7_FAILURE;
  }

  /* Save data in free instance */
  igmpInstances[igmp_idx].McastEvcId = McastEvcId;
  igmpInstances[igmp_idx].UcastEvcId = UcastEvcId;
  igmpInstances[igmp_idx].inUse = L7_TRUE;

  /* Save direct referencing to igmp index from evc ids */
  igmpInst_fromEvcId[McastEvcId] = igmp_idx;
  igmpInst_fromEvcId[UcastEvcId] = igmp_idx;

  /* Configure querier for this instance */
  if (ptin_igmp_querier_configure(igmp_idx,L7_ENABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error setting querier configuration for igmp_idx=%u",igmp_idx);
    memset(&igmpInstances[igmp_idx],0x00,sizeof(st_IgmpInstCfg_t));
    igmpInst_fromEvcId[McastEvcId] = IGMP_INVALID_ENTRY;
    igmpInst_fromEvcId[UcastEvcId] = IGMP_INVALID_ENTRY;
    return L7_FAILURE;
  }

  /* Configure trapping for this instance */
  if (ptin_igmp_trap_configure(igmp_idx,L7_ENABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring IGMP snooping for igmp_idx=%u",igmp_idx);
    ptin_igmp_querier_configure(igmp_idx,L7_DISABLE);
    memset(&igmpInstances[igmp_idx],0x00,sizeof(st_IgmpInstCfg_t));
    igmpInst_fromEvcId[McastEvcId] = IGMP_INVALID_ENTRY;
    igmpInst_fromEvcId[UcastEvcId] = IGMP_INVALID_ENTRY;
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/**
 * Removes an IGMP instance
 * 
 * @param McastEvcId : Multicast evc id 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_instance_remove(L7_uint16 McastEvcId, L7_uint16 UcastEvcId)
{
  L7_uint igmp_idx;

  /* Validate arguments */
  if (McastEvcId>=PTIN_SYSTEM_N_EVCS || UcastEvcId>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid EVC ids: [mcEvcId,ucEvcId]=[%u,%u]",McastEvcId,UcastEvcId);
    return L7_FAILURE;
  }

  /* Check if there is an instance with these parameters */
  if (ptin_igmp_instance_find(McastEvcId,UcastEvcId,&igmp_idx)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"There is no instance with [mcEvcId,ucEvcId]=[%u,%u]",McastEvcId,UcastEvcId);
    return L7_SUCCESS;
  }

  /* Remove all clients attached to this instance */
  if (ptin_igmp_instance_deleteAll_clients(igmp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing all clients from igmp_idx %u (McastEvcId=%u,UcastEvcId=%u)",igmp_idx,McastEvcId,UcastEvcId);
    return L7_FAILURE;
  }

  /* Deconfigure querier for this instance */
  if (ptin_igmp_querier_configure(igmp_idx,L7_DISABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error clearing querier configuration for igmp_idx=%u",igmp_idx);
    return L7_FAILURE;
  }

  /* Configure querier for this instance */
  if (ptin_igmp_trap_configure(igmp_idx,L7_DISABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring IGMP snooping for igmp_idx=%u",igmp_idx);
    ptin_igmp_querier_configure(igmp_idx,L7_ENABLE);
    return L7_FAILURE;
  }

  /* Clear data and free instance */
  igmpInstances[igmp_idx].McastEvcId = 0;
  igmpInstances[igmp_idx].UcastEvcId = 0;
  igmpInstances[igmp_idx].inUse = L7_FALSE;

  /* Reset direct referencing to igmp index from evc ids */
  igmpInst_fromEvcId[McastEvcId] = IGMP_INVALID_ENTRY;
  igmpInst_fromEvcId[UcastEvcId] = IGMP_INVALID_ENTRY;

  return L7_SUCCESS;
}


/**
 * Reactivate all IGMP instances
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_instances_reactivate(void)
{
  L7_uint8 igmp_idx;
  L7_RC_t  rc = L7_SUCCESS;

  for (igmp_idx=0; igmp_idx<PTIN_SYSTEM_N_IGMP_INSTANCES; igmp_idx++)
  {
    if (!igmpInstances[igmp_idx].inUse)  continue;

    /* Disable, and reenable querier for this instance */
    if (ptin_igmp_querier_configure(igmp_idx,L7_DISABLE)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error disabling querier for igmp_idx=%u",igmp_idx);
      rc = L7_FAILURE;
      continue;
    }
    if (ptin_igmp_querier_configure(igmp_idx,L7_ENABLE)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error enabling querier for igmp_idx=%u",igmp_idx);
      rc = L7_FAILURE;
      continue;
    }
  }

  return rc;
}


/**
 * Update IGMP entries, when EVCs are deleted
 * 
 * @param evcId : evc index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_instance_destroy(L7_uint16 evcId)
{
  L7_uint igmp_idx;

  /* Check if this evc index is used in any IGMP instance */
  if (ptin_igmp_instance_find_fromSingleEvcId(evcId,&igmp_idx)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"EVC id %u is not used in any IGMP instance",evcId);
    return L7_SUCCESS;
  }

  return ptin_igmp_instance_remove(igmpInstances[igmp_idx].McastEvcId,igmpInstances[igmp_idx].UcastEvcId);
}

/**
 * Update snooping configuration, when interfaces are 
 * added/removed 
 * 
 * @param evcId     : EVC id 
 * @param ptin_intf : interface 
 * @param enable    : L7_TRUE when interface is added 
 *                    L7_FALSE when interface is removed
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_snooping_trap_interface_update(L7_uint16 evcId, ptin_intf_t *ptin_intf, L7_BOOL enable)
{
#if (!PTIN_SYSTEM_GROUP_VLANS)
  ptin_evc_intfCfg_t intfCfg;
  ptin_HwEthMef10Evc_t evcCfg;

  /* Validate arguments */
  if (evcId>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid EVC id: evcId=%u",evcId);
    return L7_FAILURE;
  }

  /* This evc must be active */
  if (!ptin_evc_is_in_use(evcId))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC id is not active: evcId=%u",evcId);
    return L7_FAILURE;
  }

  /* Check if this EVC is being used by any igmp instance */
  if (ptin_igmp_instance_find_fromSingleEvcId(evcId,L7_NULLPTR)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"EVC %u is not used in any IGMP instance... nothing to do",evcId);
    return L7_SUCCESS;
  }

  /* Get EVC configuration */
  evcCfg.index = evcId;
  if (ptin_evc_get(&evcCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error acquiring EVC %u configuration",evcId);
    return L7_FAILURE;
  }

  #if 0
  /* If EVC is stacked, there is nothing to be done */
  if (evcCfg.flags & PTIN_EVC_MASK_STACKED)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"EVC %u is stacked: there is no need for update",evcId);
    return L7_SUCCESS;
  }
  /* At this point EVC is unstacked! */
  #endif

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(evcId,ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error acquiring interface %u/%u configuarion from EVC id %u",ptin_intf->intf_type,ptin_intf->intf_id,evcId);
    return L7_FAILURE;
  }

  #if 0
  /* Root interfaces do not need any intervention */
  if (intfCfg.type!=PTIN_EVC_INTF_LEAF)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Interface %u/%u of Unstacked EVC %u is not a leaf: there is no need for update",ptin_intf->intf_type,ptin_intf->intf_id,evcId);
    return L7_SUCCESS;
  }
  /* At this point EVC is unstacked, and interface is leaf! */
  #endif

  /* If internal vlan associated to interface is valid, use it */
  if (intfCfg.int_vlan>=PTIN_VLAN_MIN && intfCfg.int_vlan<=PTIN_VLAN_MAX)
  {
    if (usmDbSnoopVlanModeSet(1,intfCfg.int_vlan,enable,L7_AF_INET)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring to %u int_vlan %u of interface %u/%u (EVC id %u)",enable,intfCfg.int_vlan,ptin_intf->intf_type,ptin_intf->intf_id,evcId);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMP trapping configured to %u, for vlan %u (interface %u/%u)",enable,intfCfg.int_vlan,ptin_intf->intf_type,ptin_intf->intf_id);
  }
#endif
  return L7_SUCCESS;
}

/**
 * Add a new Multicast client
 * 
 * @param McastEvcId  : Multicast evc id
 * @param client      : client identification parameters 
 * @param isDynamic   : client type 
 * @param client_idx_ret : client index (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_client_add(L7_uint16 McastEvcId, ptin_client_id_t *client)
{
  L7_uint igmp_idx;
  L7_RC_t rc;

  /* Get IGMP instance index */
  if (ptin_igmp_instance_find_fromMcastEvcId(McastEvcId, &igmp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"There is no IGMP instance with MC EVC id %u",McastEvcId);
    return L7_FAILURE;
  }

  /* Create new static client */
  rc = ptin_igmp_new_client(igmp_idx,client,L7_FALSE,L7_NULLPTR);

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error creating static client for igmp_idx=%u (mcEVC=%u)",igmp_idx,McastEvcId);
    return L7_FAILURE;
  }

  return rc;
}

/**
 * Remove a Multicast client
 * 
 * @param McastEvcId  : Multicast evc id
 * @param client      : client identification parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_client_delete(L7_uint16 McastEvcId, ptin_client_id_t *client)
{
  L7_uint igmp_idx;
  L7_RC_t rc;

  /* Get IGMP instance index */
  if (ptin_igmp_instance_find_fromMcastEvcId(McastEvcId, &igmp_idx)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"There is no IGMP instance with MC EVC id %u",McastEvcId);
    return L7_NOT_EXIST;
  }

  /* Remove client */
  rc = ptin_igmp_rm_client(igmp_idx,client, L7_TRUE);

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error deleting static client for igmp_idx=%u (mcEVC=%u)",igmp_idx,McastEvcId);
  }

  return rc;
}

/**
 * Remove all Multicast clients 
 * 
 * @param McastEvcId  : Multicast evc id
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_all_clients_flush(L7_uint16 McastEvcId)
{
  L7_uint igmp_idx;

  /* Get IGMP instance index */
  if (ptin_igmp_instance_find_fromMcastEvcId(McastEvcId, &igmp_idx)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"There is no IGMP instance with MC EVC id %u",McastEvcId);
    return L7_NOT_EXIST;
  }

  /* Remove all clients */
  if ( ptin_igmp_rm_all_clients(igmp_idx, L7_FALSE, L7_TRUE)!=L7_SUCCESS ||
       ptin_igmp_rm_all_clients(igmp_idx, L7_TRUE,  L7_TRUE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error flushing all clients for MCEvcId=%u (igmp_idx=%u)",McastEvcId,igmp_idx);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Clients flushed!");

  return L7_SUCCESS;
}

/**
 * Get list of channels, starting from a specific channel index
 * 
 * @param McastEvcId         : (in) Multicast EVC id
 * @param client             : (in) Client information
 * @param channel_index      : (in) First channel index
 * @param number_of_channels : (in) Maximum number of channels 
 *                             (out) Number of channels 
 * @param channel_list       : (out) Channels array 
 * @param total_channels     : (out) Total number of channels
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_uint16 channelList_size=0;
static L7_inet_addr_t channelList[L7_MAX_GROUP_REGISTRATION_ENTRIES];

L7_RC_t ptin_igmp_channelList_get(L7_uint16 McastEvcId, ptin_client_id_t *client,
                                  L7_uint16 channel_index, L7_uint16 *number_of_channels, L7_in_addr_t *channel_list,
                                  L7_uint16 *total_channels)
{
  L7_uint16 McastRootVlan;
  L7_uint igmp_idx, client_idx;
  ptinIgmpClientInfoData_t *clientInfo;
  L7_uint16 i, max_channels, n_channels;

  /* Validate arguments */
  if (channel_list==L7_NULLPTR || number_of_channels==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Null parameters");
    return L7_FAILURE;
  }

  /* Get IGMP instance index */
  if (ptin_igmp_instance_find_fromMcastEvcId(McastEvcId, &igmp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"There is no IGMP instance with MC EVC id %u",McastEvcId);
    return L7_FAILURE;
  }

  /* Get Multicast root vlan */
  if (ptin_evc_get_intRootVlan(igmpInstances[igmp_idx].McastEvcId,&McastRootVlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting McastRootVlan for MCEvcId=%u (intVlan=%u)",McastEvcId,McastRootVlan);
    return L7_FAILURE;
  }

  /* If a client is provided, calculate client index */
  if (MC_CLIENT_MASK_UPDATE(client->mask)!=0x00)
  {
    /* Find client */
    if (ptin_igmp_client_find(igmp_idx,client,&clientInfo)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,
              "Error searching for client {mask=0x%02x,"
              "port=%u/%u,"
              "svlan=%u,"
              "cvlan=%u,"
              "ipAddr=%u.%u.%u.%u,"
              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} "
              "in igmp_idx=%u",
              client->mask,
              client->ptin_intf.intf_type, client->ptin_intf.intf_id,
              client->outerVlan,
              client->innerVlan,
              (client->ipv4_addr>>24) & 0xff, (client->ipv4_addr>>16) & 0xff, (client->ipv4_addr>>8) & 0xff, client->ipv4_addr & 0xff,
              client->macAddr[0],client->macAddr[1],client->macAddr[2],client->macAddr[3],client->macAddr[4],client->macAddr[5],
              igmp_idx);
      return L7_FAILURE;
    }
    /* Extract client index */
    client_idx = clientInfo->client_index;
  }
  else
  {
    /* No client provided */
    client_idx = (L7_uint)-1;
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"No client provided");
  }

  /* If channel index is null, get all channels */
  if (channel_index==0)
  {
    memset(channelList,0x00,sizeof(channelList));
    channelList_size = 0;

    n_channels = L7_MAX_GROUP_REGISTRATION_ENTRIES;
    if (ptin_snoop_activeChannels_get(McastRootVlan,client_idx,channelList,&n_channels)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting channels list");
      return L7_FAILURE;
    }
    channelList_size = n_channels;
  }
  /* Validate channel index */
  if (channel_index>=channelList_size)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Channel index is invalid");
    return L7_NOT_EXIST;
  }
  /* Max size to be extracted */
  max_channels = channelList_size - channel_index;
  if (*number_of_channels<max_channels)
    max_channels = *number_of_channels;

  /* Copy channel adddresses */
  n_channels=0;
  for (i=0; i<max_channels; i++)
  {
    if (channelList[channel_index+i].family==L7_AF_INET)
    {
      channel_list[i].s_addr = channelList[channel_index+i].addr.ipv4.s_addr;
      n_channels++;
    }
  }
  /* Return number of read channels */
  *number_of_channels = n_channels;

  /* Total number of channels */
  if (total_channels!=L7_NULLPTR)  *total_channels = channelList_size;

  return L7_SUCCESS;
}

/**
 * Get list of clients, watching a specific channel 
 * 
 * @param McastEvcId         : (in) Multicast EVC id
 * @param channel            : (in) IP Channel
 * @param client_index       : (in) First client index
 * @param number_of_clients  : (in) Maximum number of clients 
 *                             (out) Number of clients 
 * @param client_list        : (out) Clients array 
 * @param total_clients      : (out) Total number of clients
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_uint16 clientList_size=0;
static ptin_client_id_t clientList[PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE];
#define UINT32_BITSIZE  (sizeof(L7_uint32)*8)

L7_RC_t ptin_igmp_clientList_get(L7_uint16 McastEvcId, L7_in_addr_t *ipv4_channel,
                                 L7_uint16 client_index, L7_uint16 *number_of_clients, ptin_client_id_t *client_list,
                                 L7_uint16 *total_clients)
{
  ptin_intf_t     ptin_intf;
  L7_uint16       McastRootVlan;
  L7_uint         igmp_idx, client_idx;
  L7_inet_addr_t  channel;
  L7_uint16       n_clients, max_clients;
  L7_uint32       clientIdx_bmp_list[PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE/UINT32_BITSIZE];
  avlTree_t               *avl_tree;
  ptinIgmpClientDataKey_t key;
  ptinIgmpClientInfoData_t *infoData;

  /* Validate arguments */
  if (client_list==L7_NULLPTR || number_of_clients==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Null parameters");
    return L7_FAILURE;
  }

  /* Get IGMP instance index */
  if (ptin_igmp_instance_find_fromMcastEvcId(McastEvcId, &igmp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"There is no IGMP instance with MC EVC id %u",McastEvcId);
    return L7_FAILURE;
  }

  /* For the first reading, get the clients list */
  if (client_index==0)
  {
    /* Get client indexes watching a particular channel, if provided */
    if (ipv4_channel!=L7_NULLPTR && ipv4_channel->s_addr!=0)
    {
      /* Get Multicast root vlan */
      if (ptin_evc_get_intRootVlan(igmpInstances[igmp_idx].McastEvcId,&McastRootVlan)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting McastRootVlan for MCEvcId=%u (intVlan=%u)",McastEvcId,McastRootVlan);
        return L7_FAILURE;
      }

      /* Channel to search for */
      channel.family = L7_AF_INET;
      channel.addr.ipv4.s_addr = ipv4_channel->s_addr;

      /* Get list of client indexes for this vlan */
      if (ptin_snoop_clientsList_get(&channel,McastRootVlan,clientIdx_bmp_list,L7_NULLPTR)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting list of clients");
        return L7_FAILURE;
      }
    }
    else
    {
      /* All clients will be returned */
      memset(clientIdx_bmp_list,0xff,sizeof(clientIdx_bmp_list));
    }

    /* Clear database */
    memset(clientList,0x00,sizeof(clientList));
    clientList_size = 0;

    avl_tree = &igmpInstances[igmp_idx].igmpClients.avlTree.igmpClientsAvlTree;

    n_clients=0;
    memset(&key,0x00,sizeof(ptinIgmpClientDataKey_t));
    while (n_clients<PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE &&
           (infoData=(ptinIgmpClientInfoData_t *) avlSearchLVL7(avl_tree,&key,L7_MATCH_GETNEXT))!=L7_NULLPTR)
    {
      /* Update key */
      key = infoData->igmpClientDataKey;

      /* Extract client index */
      client_idx = infoData->client_index;

      /* If channel is provided, but it is not being consumed by this client, skip to the next one */
      if ( !((clientIdx_bmp_list[client_idx/UINT32_BITSIZE]>>(client_idx%UINT32_BITSIZE)) & 1) )
        continue;

      #if (MC_CLIENT_INTERF_SUPPORTED)
      /* Convert port to interface */
      if (ptin_intf_port2ptintf(infoData->igmpClientDataKey.ptin_port, &ptin_intf)!=L7_SUCCESS)
        continue;
      clientList[n_clients].ptin_intf = ptin_intf;
      clientList[n_clients].mask |= PTIN_CLIENT_MASK_FIELD_INTF;
      #endif
      #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
      clientList[n_clients].outerVlan = infoData->igmpClientDataKey.outerVlan;
      clientList[n_clients].mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
      #endif
      #if (MC_CLIENT_INNERVLAN_SUPPORTED)
      clientList[n_clients].innerVlan = infoData->igmpClientDataKey.innerVlan;
      clientList[n_clients].mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
      #endif
      #if (MC_CLIENT_IPADDR_SUPPORTED)
      clientList[n_clients].ipv4_addr = infoData->igmpClientDataKey.ipv4_addr;
      clientList[n_clients].mask |= PTIN_CLIENT_MASK_FIELD_IPADDR;
      #endif
      #if (MC_CLIENT_MACADDR_SUPPORTED)
      memcpy(clientList[n_clients].macAddr, infoData->igmpClientDataKey.macAddr, sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
      clientList[n_clients].mask |= PTIN_CLIENT_MASK_FIELD_MACADDR;
      #endif

      /* One more client */
      n_clients++;
    }

    clientList_size = n_clients;
  }

  /* Validate client index */
  if (client_index>=clientList_size)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Client index is invalid");
    return L7_NOT_EXIST;
  }

  /* Max size to be extracted */
  max_clients = clientList_size - client_index;
  if (*number_of_clients<max_clients)
    max_clients = *number_of_clients;

  /* Copy clients info */
  memcpy(client_list,&clientList[client_index],sizeof(ptin_client_id_t)*max_clients);
  /* Return number of read channels */
  *number_of_clients = max_clients;

  /* Total number of channels */
  if (total_clients!=L7_NULLPTR)  *total_clients = clientList_size;

  return L7_SUCCESS;
}

/**
 * Add a new static channel
 * 
 * @param McastEvcId   : Multicast EVC id
 * @param ipv4_channel : Channel IP
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_static_channel_add(L7_uint16 McastEvcId, L7_in_addr_t *ipv4_channel)
{
  L7_uint igmp_idx;
  L7_uint16 McastRootVlan;
  L7_inet_addr_t channel;

  /* Validate arguments */
  if (ipv4_channel==L7_NULLPTR || ipv4_channel->s_addr==0)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get IGMP instance index */
  if (ptin_igmp_instance_find_fromMcastEvcId(McastEvcId, &igmp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"There is no IGMP instance with MC EVC id %u",McastEvcId);
    return L7_FAILURE;
  }

  /* Get Multicast root vlan */
  if (ptin_evc_get_intRootVlan(igmpInstances[igmp_idx].McastEvcId,&McastRootVlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting McastRootVlan for MCEvcId=%u",McastEvcId);
    return L7_FAILURE;
  }

  /* Channel */
  channel.family = L7_AF_INET;
  channel.addr.ipv4.s_addr = ipv4_channel->s_addr;

  /* Add static channel */
  if (ptin_snoop_static_channel_add(McastRootVlan,&channel)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error adding static channel");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Remove an existent channel
 * 
 * @param McastEvcId   : Multicast EVC id
 * @param ipv4_channel : Channel IP
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_channel_remove(L7_uint16 McastEvcId, L7_in_addr_t *ipv4_channel)
{
  L7_uint igmp_idx;
  L7_uint16 McastRootVlan;
  L7_inet_addr_t channel;

  /* Validate arguments */
  if (ipv4_channel==L7_NULLPTR || ipv4_channel->s_addr==0)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get IGMP instance index */
  if (ptin_igmp_instance_find_fromMcastEvcId(McastEvcId, &igmp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"There is no IGMP instance with MC EVC id %u",McastEvcId);
    return L7_FAILURE;
  }

  /* Get Multicast root vlan */
  if (ptin_evc_get_intRootVlan(igmpInstances[igmp_idx].McastEvcId,&McastRootVlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting McastRootVlan for MCEvcId=%u",McastEvcId);
    return L7_FAILURE;
  }

  /* Channel */
  channel.family = L7_AF_INET;
  channel.addr.ipv4.s_addr = ipv4_channel->s_addr;

  /* Remove channel */
  if (ptin_snoop_channel_remove(McastRootVlan,&channel)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing channel");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get global IGMP statistics
 * 
 * @param intIfNum    : interface
 * @param stat_port_g : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_intf_get(ptin_intf_t *ptin_intf, ptin_IGMP_Statistics_t *stat_port_g)
{
  L7_uint32 ptin_port;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid interface %u/%u",ptin_intf->intf_id,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Return pointer to stat structure */
  if (stat_port_g!=L7_NULLPTR)
  {
    osapiSemaTake(ptin_igmp_stats_sem,-1);
    memcpy(stat_port_g, &global_stats_intf[ptin_port], sizeof(ptin_IGMP_Statistics_t));
    osapiSemaGive(ptin_igmp_stats_sem);
  }

  return L7_SUCCESS;
}

/**
 * GetIGMP statistics of a particular IGMP instance and 
 * interface 
 * 
 * @param McastEvcId  : Multicast EVC id
 * @param intIfNum    : interface
 * @param stat_port   : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_instanceIntf_get(L7_uint16 McastEvcId, ptin_intf_t *ptin_intf, ptin_IGMP_Statistics_t *stat_port)
{
  L7_uint32 ptin_port;
  L7_uint32 igmp_idx;
  ptin_evc_intfCfg_t intfCfg;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid interface %u/%u",ptin_intf->intf_id,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Check if EVC is active, and if interface is part of the EVC */
  if (ptin_evc_intfCfg_get(McastEvcId,ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting interface (%u/%u) configuration from EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,McastEvcId);
    return L7_FAILURE;
  }
  if (!intfCfg.in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Interface %u/%u is not in use by EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,McastEvcId);
    return L7_FAILURE;
  }

  /* Get Igmp instance */
  if (ptin_igmp_instance_find_fromMcastEvcId(McastEvcId,&igmp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC %u is not part of an IGMP instance",McastEvcId);
    return L7_FAILURE;
  }

  /* Return pointer to stat structure */
  if (stat_port!=L7_NULLPTR)
  {
    osapiSemaTake(ptin_igmp_stats_sem,-1);
    memcpy(stat_port, &igmpInstances[igmp_idx].stats_intf[ptin_port], sizeof(ptin_IGMP_Statistics_t));
    osapiSemaGive(ptin_igmp_stats_sem);
  }

  return L7_SUCCESS;
}

/**
 * GetIGMP statistics of a particular IGMP instance and 
 * client
 * 
 * @param McastEvcId  : Multicast EVC id
 * @param client      : client reference
 * @param stat_port   : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_client_get(L7_uint16 McastEvcId, ptin_client_id_t *client, ptin_IGMP_Statistics_t *stat_client)
{
  L7_uint32 igmp_idx;
  ptinIgmpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if (client==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get Igmp instance */
  if (ptin_igmp_instance_find_fromMcastEvcId(McastEvcId,&igmp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC %u is not part of an IGMP instance",McastEvcId);
    return L7_FAILURE;
  }

  /* Get client */
  if (ptin_igmp_client_find(igmp_idx, client, &clientInfo)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,
            "Error searching for client {mask=0x%02x,"
            "port=%u/%u,"
            "svlan=%u,"
            "cvlan=%u,"
            "ipAddr=%u.%u.%u.%u,"
            "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} "
            "in igmp_idx=%u",
            client->mask,
            client->ptin_intf.intf_type, client->ptin_intf.intf_id,
            client->outerVlan,
            client->innerVlan,
            (client->ipv4_addr>>24) & 0xff, (client->ipv4_addr>>16) & 0xff, (client->ipv4_addr>>8) & 0xff, client->ipv4_addr & 0xff,
            client->macAddr[0],client->macAddr[1],client->macAddr[2],client->macAddr[3],client->macAddr[4],client->macAddr[5],
            igmp_idx);
    return L7_FAILURE;
  }

  /* Return pointer to stat structure */
  if (stat_client!=L7_NULLPTR)
  {
    osapiSemaTake(ptin_igmp_stats_sem,-1);
    memcpy(stat_client, &clientInfo->stats_client, sizeof(ptin_IGMP_Statistics_t));
    osapiSemaGive(ptin_igmp_stats_sem);
  }

  return L7_SUCCESS;
}

/**
 * Clear all IGMP statistics
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_clearAll(void)
{
  L7_uint igmp_idx;
  L7_uint client_idx;

  osapiSemaTake(ptin_igmp_stats_sem,-1);

  /* Run all IGMP instances */
  for (igmp_idx=0; igmp_idx<PTIN_SYSTEM_N_IGMP_INSTANCES; igmp_idx++)
  {
    if (!igmpInstances[igmp_idx].inUse)  continue;

    /* Clear instance statistics */
    memset(igmpInstances[igmp_idx].stats_intf, 0x00, sizeof(igmpInstances[igmp_idx].stats_intf));

    /* Run all clients */
    for (client_idx=0; client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE; client_idx++)
    {
      if (igmpInstances[igmp_idx].igmpClients.clients_in_use[client_idx]==L7_NULLPTR)  continue;

      /* Clear client statistics */
      memset(&igmpInstances[igmp_idx].igmpClients.clients_in_use[client_idx]->stats_client, 0x00, sizeof(ptin_IGMP_Statistics_t));
    }
  }

  /* Clear global statistics */
  memset(global_stats_intf,0x00,sizeof(global_stats_intf));

  osapiSemaGive(ptin_igmp_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear all statistics of one IGMP instance
 * 
 * @param McastEvcId : Multicast EVC id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_instance_clear(L7_uint16 McastEvcId)
{
  L7_uint igmp_idx;
  L7_uint client_idx;

  /* Get Igmp instance */
  if (ptin_igmp_instance_find_fromMcastEvcId(McastEvcId,&igmp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC %u is not part of an IGMP instance",McastEvcId);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_igmp_stats_sem,-1);

  /* Clear instance statistics */
  memset(igmpInstances[igmp_idx].stats_intf, 0x00, sizeof(igmpInstances[igmp_idx].stats_intf));

  /* Run all clients */
  for (client_idx=0; client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE; client_idx++)
  {
    if (igmpInstances[igmp_idx].igmpClients.clients_in_use[client_idx]==L7_NULLPTR)  continue;

    /* Clear client statistics */
    memset(&igmpInstances[igmp_idx].igmpClients.clients_in_use[client_idx]->stats_client, 0x00, sizeof(ptin_IGMP_Statistics_t));
  }

  osapiSemaGive(ptin_igmp_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear interface IGMP statistics
 * 
 * @param intIfNum    : interface 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_intf_clear(ptin_intf_t *ptin_intf)
{
  L7_uint igmp_idx;
  L7_uint client_idx;
  L7_uint32 ptin_port;
  st_IgmpInstCfg_t *igmpInst;
  ptinIgmpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid interface %u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_igmp_stats_sem,-1);

  /* Run all IGMP instances */
  for (igmp_idx=0; igmp_idx<PTIN_SYSTEM_N_IGMP_INSTANCES; igmp_idx++)
  {
    if (!igmpInstances[igmp_idx].inUse)  continue;

    igmpInst = &igmpInstances[igmp_idx];

    /* Clear instance statistics */
    memset(&igmpInst->stats_intf[ptin_port], 0x00, sizeof(ptin_IGMP_Statistics_t));

    #if (MC_CLIENT_INTERF_SUPPORTED)
    /* Run all clients */
    for (client_idx=0; client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE; client_idx++)
    {
      if (igmpInst->igmpClients.clients_in_use[client_idx]==L7_NULLPTR)  continue;

      /* Clear client statistics (if port matches) */
      clientInfo = igmpInst->igmpClients.clients_in_use[client_idx];
      if (clientInfo->igmpClientDataKey.ptin_port==ptin_port)
      {
        memset(&clientInfo->stats_client, 0x00, sizeof(ptin_IGMP_Statistics_t));
      }
    }
    #endif
  }

  /* Clear global statistics */
  memset(&global_stats_intf[ptin_port], 0x00, sizeof(ptin_IGMP_Statistics_t));

  osapiSemaGive(ptin_igmp_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear statistics of a particular IGMP instance and interface
 * 
 * @param McastEvcId  : Multicast EVC id
 * @param intIfNum    : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_instanceIntf_clear(L7_uint16 McastEvcId, ptin_intf_t *ptin_intf)
{
  L7_uint igmp_idx;
  L7_uint client_idx;
  L7_uint32 ptin_port;
  st_IgmpInstCfg_t *igmpInst;
  ptinIgmpClientInfoData_t *clientInfo;
  ptin_evc_intfCfg_t intfCfg;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid interface %u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Check if EVC is active, and if interface is part of the EVC */
  if (ptin_evc_intfCfg_get(McastEvcId,ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting interface (%u/%u) configuration from EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,McastEvcId);
    return L7_FAILURE;
  }
  if (!intfCfg.in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Interface %u/%u is not in use by EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,McastEvcId);
    return L7_FAILURE;
  }

  /* Get Igmp instance */
  if (ptin_igmp_instance_find_fromMcastEvcId(McastEvcId,&igmp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC %u is not part of an IGMP instance",McastEvcId);
    return L7_FAILURE;
  }

  /* Pointer to igmp instance */
  igmpInst = &igmpInstances[igmp_idx];

  osapiSemaTake(ptin_igmp_stats_sem,-1);

  /* Clear instance statistics */
  memset(&igmpInst->stats_intf[ptin_port], 0x00, sizeof(ptin_IGMP_Statistics_t));

  #if (MC_CLIENT_INTERF_SUPPORTED)
  /* Run all clients */
  for (client_idx=0; client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE; client_idx++)
  {
    if (igmpInst->igmpClients.clients_in_use[client_idx]==L7_NULLPTR)  continue;

    /* Clear client statistics (if port matches) */
    clientInfo = igmpInst->igmpClients.clients_in_use[client_idx];
    if (clientInfo->igmpClientDataKey.ptin_port==ptin_port)
    {
      memset(&clientInfo->stats_client, 0x00, sizeof(ptin_IGMP_Statistics_t));
    }
  }
  #endif

  osapiSemaGive(ptin_igmp_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear IGMP statistics of a particular IGMP instance and 
 * client
 * 
 * @param McastEvcId  : Multicast EVC id
 * @param client      : client reference
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_client_clear(L7_uint16 McastEvcId, ptin_client_id_t *client)
{
  L7_uint igmp_idx;
  ptinIgmpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if (client==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get Igmp instance */
  if (ptin_igmp_instance_find_fromMcastEvcId(McastEvcId,&igmp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC %u is not part of an IGMP instance",McastEvcId);
    return L7_FAILURE;
  }

  /* Find client */
  if (ptin_igmp_client_find(igmp_idx,client,&clientInfo)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,
            "Error searching for client {mask=0x%02x,"
            "port=%u/%u,"
            "svlan=%u,"
            "cvlan=%u,"
            "ipAddr=%u.%u.%u.%u,"
            "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} "
            "in igmp_idx=%u",
            client->mask,
            client->ptin_intf.intf_type, client->ptin_intf.intf_id,
            client->outerVlan,
            client->innerVlan,
            (client->ipv4_addr>>24) & 0xff, (client->ipv4_addr>>16) & 0xff, (client->ipv4_addr>>8) & 0xff, client->ipv4_addr & 0xff,
            client->macAddr[0],client->macAddr[1],client->macAddr[2],client->macAddr[3],client->macAddr[4],client->macAddr[5],
            igmp_idx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_igmp_stats_sem,-1);

  /* Clear client statistics (if port matches) */
  memset(&clientInfo->stats_client, 0x00, sizeof(ptin_IGMP_Statistics_t));

  osapiSemaGive(ptin_igmp_stats_sem);

  return L7_SUCCESS;
}

/******************************************************** 
 * FOR FASTPATH INTERNAL MODULES USAGE
 ********************************************************/

/**
 * Get the client index associated to a Multicast client 
 * 
 * @param intIfNum      : interface number
 * @param intVlan       : internal vlan
 * @param client        : Client information parameters
 * @param client_index  : Client index to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_clientIndex_get(L7_uint32 intIfNum, L7_uint16 intVlan,
                                  ptin_client_id_t *client,
                                  L7_uint *client_index)
{
  L7_uint     igmp_idx;
  ptin_intf_t ptin_intf;
  L7_uint     client_idx;
  ptinIgmpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if ( client==L7_NULLPTR || MC_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,L7_NULLPTR,&igmp_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get ptin_port format for the interface number */
  #if (MC_CLIENT_INTERF_SUPPORTED)
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    if (intIfNum!=0 && intIfNum!=L7_ALL_INTERFACES)
    {
      if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)==L7_SUCCESS)
      {
        client->ptin_intf.intf_type = ptin_intf.intf_type;
        client->ptin_intf.intf_id   = ptin_intf.intf_id;
      }
      else
      {
        if (ptin_debug_igmp_snooping)
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Connot convert client intIfNum %u to ptin_port_format",intIfNum);
        return L7_FAILURE;
      }
    }
  }
  #endif

  /* Get client */
  if (ptin_igmp_client_find(igmp_idx, client, &clientInfo)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,
              "Error searching for client {mask=0x%02x,"
              "port=%u/%u,"
              "svlan=%u,"
              "cvlan=%u,"
              "ipAddr=%u.%u.%u.%u,"
              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} "
              "in igmp_idx=%u",
              client->mask,
              client->ptin_intf.intf_type, client->ptin_intf.intf_id,
              client->outerVlan,
              client->innerVlan,
              (client->ipv4_addr>>24) & 0xff, (client->ipv4_addr>>16) & 0xff, (client->ipv4_addr>>8) & 0xff, client->ipv4_addr & 0xff,
              client->macAddr[0],client->macAddr[1],client->macAddr[2],client->macAddr[3],client->macAddr[4],client->macAddr[5],
              igmp_idx);
    }
    return L7_FAILURE;
  }

  /* Update client index in data cell */
  client_idx = clientInfo->client_index;

  if (ptin_debug_igmp_snooping)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Client_idx=%u for key {"
              #if (MC_CLIENT_INTERF_SUPPORTED)
                                "port=%u,"
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                                "svlan=%u,"
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                                "cvlan=%u,"
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
                                "ipAddr=%u.%u.%u.%u,"
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
                                "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
              #endif
                                "}"
              ,client_idx
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,clientInfo->igmpClientDataKey.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,clientInfo->igmpClientDataKey.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,clientInfo->igmpClientDataKey.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(clientInfo->igmpClientDataKey.ipv4_addr>>24) & 0xff, (clientInfo->igmpClientDataKey.ipv4_addr>>16) & 0xff, (clientInfo->igmpClientDataKey.ipv4_addr>>8) & 0xff, clientInfo->igmpClientDataKey.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,clientInfo->igmpClientDataKey.macAddr[0],clientInfo->igmpClientDataKey.macAddr[1],clientInfo->igmpClientDataKey.macAddr[2],clientInfo->igmpClientDataKey.macAddr[3],clientInfo->igmpClientDataKey.macAddr[4],clientInfo->igmpClientDataKey.macAddr[5]
              #endif
              );
  }

  /* Return client index */
  if (client_index!=L7_NULLPTR)  *client_index = client_idx;

  return L7_SUCCESS;
}


/**
 * Get client type from its index. 
 * 
 * @param intVlan       : internal vlan
 * @param client_index  : Client index
 * @param isDynamic     : client type (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_client_type(L7_uint16 intVlan,
                              L7_uint client_idx,
                              L7_BOOL *isDynamic)
{
  st_IgmpInstCfg_t *igmpInst;
  ptinIgmpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if ( client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE )
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get pointer to client structure in AVL tree */
  clientInfo = igmpInst->igmpClients.clients_in_use[client_idx];
  /* If does not exist... */
  if (clientInfo==L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Provided client_idx (%u) does not exist",client_idx);
    return L7_FAILURE;
  }

  /* Return client type */
  if (isDynamic!=L7_NULLPTR)
  {
    *isDynamic = clientInfo->isDynamic;
  }

  return L7_SUCCESS;
}

/**
 * (Re)start the timer for this client
 *  
 * @param intVlan     : Internal vlan
 * @param client_idx  : client index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_client_timer_start(L7_uint16 intVlan, L7_uint32 client_idx)
{
#ifdef CLIENT_TIMERS_SUPPORTED
  L7_uint     igmp_idx;
  L7_RC_t     rc;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan, L7_NULLPTR, &igmp_idx) != L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Add client */
  osapiSemaTake(ptin_igmp_timers_sem, L7_WAIT_FOREVER);
  rc = ptin_igmp_timer_start(igmp_idx, client_idx);
  osapiSemaGive(ptin_igmp_timers_sem);

  if (rc!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error (re)starting timer for this client (intVlan=%u, igmp_idx=%u, client_idx=%u)",
              intVlan, igmp_idx, client_idx);
  }

  return rc;
#else
  return L7_SUCCESS;
#endif
}

/**
 * Add a dynamic client
 *  
 * @param intIfNum    : interface number  
 * @param intVlan     : Internal vlan
 * @param client      : client identification parameters 
 * @param client_idx_ret : client index (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_dynamic_client_add(L7_uint32 intIfNum, L7_uint16 intVlan, ptin_client_id_t *client, L7_uint *client_idx_ret)
{
  L7_uint     igmp_idx;
  ptin_intf_t ptin_intf;
  L7_RC_t     rc;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan, L7_NULLPTR, &igmp_idx) != L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get ptin_port format for the interface number */
  #if (MC_CLIENT_INTERF_SUPPORTED)
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    if (intIfNum!=0 && intIfNum!=L7_ALL_INTERFACES)
    {
      if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)==L7_SUCCESS)
      {
        client->ptin_intf.intf_type = ptin_intf.intf_type;
        client->ptin_intf.intf_id   = ptin_intf.intf_id;
      }
      else
      {
        if (ptin_debug_igmp_snooping)
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Connot convert client intIfNum %u to ptin_port_format",intIfNum);
        return L7_FAILURE;
      }
    }
  }
  #endif

  /* Add client */
  rc = ptin_igmp_new_client(igmp_idx,client,L7_TRUE,client_idx_ret);

  if (rc!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error adding dynamic client");
  }

  return rc;
}

/**
 * Remove a particular client
 * 
 * @param intVlan       : internal vlan
 * @param client_index  : Client index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_dynamic_client_flush(L7_uint16 intVlan, L7_uint client_idx)
{
  L7_uint                   igmp_idx;
  #if 0
  st_IgmpInstCfg_t         *igmpInst;
  ptinIgmpClientInfoData_t *clientInfo;
  ptinIgmpClientDataKey_t  *avl_key;
  ptinIgmpClientsAvlTree_t *avl_tree;
  #endif

  /* Validate arguments */
  if ( client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE )
  {
    //if (ptin_debug_igmp_snooping)
    //  LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan, L7_NULLPTR, &igmp_idx) != L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  if (ptin_igmp_rm_clientIdx(igmp_idx, client_idx, L7_FALSE, L7_FALSE)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error flushing dynamic client (intVlan=%u, igmp_idx=%u, client_idx=%u)",intVlan,igmp_idx,client_idx);
    return L7_FAILURE;
  }

  #if 0
  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan, &igmpInst, &igmp_idx) != L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get pointer to client structure in AVL tree */
  clientInfo = igmpInst->igmpClients.clients_in_use[client_idx];
  /* If does not exist... */
  if (clientInfo==L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Provided client_idx (%u) does not exist",client_idx);
    return L7_FAILURE;
  }

  /* Only flush this client, if client matches */
  if ( !clientInfo->isDynamic )
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"Client (%u) is static... nothing to do.",client_idx);
    return L7_SUCCESS;
  }

  /* If this client have channels, do nothing */
  if ( clientInfo->stats_client.active_groups>0 )
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"Client (%u) is watching channels... nothing to do.",client_idx);
    return L7_SUCCESS;
  }

  avl_key  = (ptinIgmpClientDataKey_t *) clientInfo;
  avl_tree = &igmpInst->igmpClients.avlTree;

  /* Remove this client */
  if (avlDeleteEntry(&(avl_tree->igmpClientsAvlTree), (void *) avl_key)==L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error flushing key {"
              #if (MC_CLIENT_INTERF_SUPPORTED)
                                "port=%u,"
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                                "svlan=%u,"
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                                "cvlan=%u,"
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
                                "ipAddr=%u.%u.%u.%u,"
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
                                "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
              #endif
                                "} from igmp_idx=%u",
              #if (MC_CLIENT_INTERF_SUPPORTED)
              avl_key->ptin_port,
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              avl_key->outerVlan,
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              avl_key->innerVlan,
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              (avl_key->ipv4_addr>>24) & 0xff, (avl_key->ipv4_addr>>16) & 0xff, (avl_key->ipv4_addr>>8) & 0xff, avl_key->ipv4_addr & 0xff,
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              avl_key->macAddr[0],avl_key->macAddr[1],avl_key->macAddr[2],avl_key->macAddr[3],avl_key->macAddr[4],avl_key->macAddr[5],
              #endif
              igmp_idx);
    return L7_FAILURE;
  }

  /* Remove client for AVL tree */
  igmp_clientIndex_unmark(igmp_idx,client_idx);
  #endif

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success flushing client");

  return L7_SUCCESS;
}

/**
 * Remove all dynamic client
 * 
 * @param intVlan       : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_dynamic_all_clients_flush(L7_uint16 intVlan)
{
  L7_uint igmp_idx;
  L7_RC_t rc;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan, L7_NULLPTR, &igmp_idx) != L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Remove all dynamic clients (only with channels) */
  rc = ptin_igmp_rm_all_clients(igmp_idx, L7_TRUE,  L7_TRUE);

  if ( rc!=L7_SUCCESS )
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error flushing all dynamic clients for intVlan=%u (igmp_idx=%u)",intVlan,igmp_idx);
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Clients flushed for intVlan %u!",intVlan);

  return L7_SUCCESS;
}


/**
 * Get client information from its index. 
 * 
 * @param intVlan       : internal vlan
 * @param client_index  : Client index
 * @param client        : Client information (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_clientData_get(L7_uint16 intVlan,
                                 L7_uint client_idx,
                                 ptin_client_id_t *client)
{
  ptin_intf_t ptin_intf;
  st_IgmpInstCfg_t *igmpInst;
  ptinIgmpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if ( client==L7_NULLPTR || client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE )
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get pointer to client structure in AVL tree */
  clientInfo = igmpInst->igmpClients.clients_in_use[client_idx];
  /* If does not exist... */
  if (clientInfo==L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Provided client_idx (%u) does not exist",client_idx);
    return L7_FAILURE;
  }

  memset(client,0x00,sizeof(ptin_client_id_t));
  #if (MC_CLIENT_INTERF_SUPPORTED)
  if (ptin_intf_port2ptintf(clientInfo->igmpClientDataKey.ptin_port,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot convert client port %uu to ptin_intf format",clientInfo->igmpClientDataKey.ptin_port);
    return L7_FAILURE;
  }
  client->ptin_intf = ptin_intf;
  client->mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  #endif
  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
  client->outerVlan = clientInfo->igmpClientDataKey.outerVlan;
  client->mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  #endif
  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
  client->innerVlan = clientInfo->igmpClientDataKey.innerVlan;
  client->mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  #endif
  #if (MC_CLIENT_IPADDR_SUPPORTED)
  client->ipv4_addr = clientInfo->igmpClientDataKey.ipv4_addr;
  client->mask |= PTIN_CLIENT_MASK_FIELD_IPADDR;
  #endif
  #if (MC_CLIENT_MACADDR_SUPPORTED)
  memcpy(client->macAddr,clientInfo->igmpClientDataKey.macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  client->mask |= PTIN_CLIENT_MASK_FIELD_MACADDR;
  #endif

  return L7_SUCCESS;
}

/**
 * Validate igmp packet checking if the input intIfNum and 
 * internal Vlan are valid
 * 
 * @param intIfNum : source interface number
 * @param intVlan  : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS: Parameters are valid
 *                   L7_FAILURE: Not valid
 */
L7_RC_t ptin_igmp_intfVlan_validate(L7_uint32 intIfNum, L7_uint16 intVlan)
{
  st_IgmpInstCfg_t *igmpInst;
  ptin_intf_t ptin_intf;
  ptin_evc_intfCfg_t mc_intf_cfg, uc_intf_cfg;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get ptin_intf format for the interface number */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting ptin_intf from intIfNum %u",intIfNum);
    return L7_FAILURE;
  }

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(igmpInst->McastEvcId,&ptin_intf,&mc_intf_cfg)!=L7_SUCCESS ||
      ptin_evc_intfCfg_get(igmpInst->UcastEvcId,&ptin_intf,&uc_intf_cfg)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting intf configuration for ptin_intf=%u/%u (intIfNum=%u), [mcEvcId,ucEvcId]=%u/%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,igmpInst->McastEvcId,igmpInst->UcastEvcId,intVlan);
    return L7_FAILURE;
  }

  /* Interfaces must be in use in both evcs */
  if (!mc_intf_cfg.in_use || !uc_intf_cfg.in_use)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Interface ptin_intf=%u/%u (intIfNum=%u) not in use for [mcEvcId,ucEvcId]=%u/%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,igmpInst->McastEvcId,igmpInst->UcastEvcId,intVlan);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Validate igmp packet checking if the input internal Vlan are 
 * valid 
 * 
 * @param intVlan  : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS: Parameters are valid
 *                   L7_FAILURE: Not valid
 */
L7_RC_t ptin_igmp_vlan_validate(L7_uint16 intVlan)
{
  st_IgmpInstCfg_t *igmpInst;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Check if an internal vlan is associated to an unstacked 
 * Unicast EVC 
 * 
 * @param intVlan : internal vlan
 * @param is_unstacked : is unstacked? (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_igmp_vlan_UC_is_unstacked(L7_uint16 intVlan, L7_BOOL *is_unstacked)
{
  st_IgmpInstCfg_t *igmpInst;
  ptin_HwEthMef10Evc_t evcConf;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get EVC configuration for the UC EVC */
  memset(&evcConf,0x00,sizeof(ptin_HwEthMef10Evc_t));
  evcConf.index = igmpInst->UcastEvcId;
  if (ptin_evc_get(&evcConf)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting EVC configuration for ucEvcId=%u (intVlan=%u)",
              igmpInst->UcastEvcId,intVlan);
    return L7_FAILURE;
  }

  if (is_unstacked!=L7_NULLPTR)
  {
    *is_unstacked = ((evcConf.flags & PTIN_EVC_MASK_STACKED) == 0);
  }

  return L7_SUCCESS;
}


/**
 * Validate igmp packet checking if the input intIfNum is a root
 * interface and internal Vlan is valid 
 * 
 * @param intIfNum : source interface number
 * @param intVlan  : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS: Parameters are valid
 *                   L7_FAILURE: Not valid
 *  
 * This function will only apply to input packets, and so, input 
 * packets in root interfaces must use the Multicast vlans. 
 * Therefore we must use the Multicast EVC to validate a root 
 * interface. 
 */
L7_RC_t ptin_igmp_rootIntfVlan_validate(L7_uint32 intIfNum, L7_uint16 intVlan)
{
  st_IgmpInstCfg_t *igmpInst;
  ptin_intf_t ptin_intf;
  ptin_evc_intfCfg_t mc_intf_cfg;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get ptin_intf format for the interface number */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting ptin_intf from intIfNum %u",intIfNum);
    return L7_FAILURE;
  }

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(igmpInst->McastEvcId,&ptin_intf,&mc_intf_cfg)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting intf configuration for ptin_intf=%u/%u (intIfNum=%u), mcEvcId=%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,igmpInst->McastEvcId,intVlan);
    return L7_FAILURE;
  }

  /* Interfaces must be in use in both evcs */
  if (!mc_intf_cfg.in_use)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Interface ptin_intf=%u/%u (intIfNum=%u) not in use for mcEvcId=%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,igmpInst->McastEvcId,intVlan);
    return L7_FAILURE;
  }

  /* Interface must be Root */
  if (mc_intf_cfg.type!=PTIN_EVC_INTF_ROOT)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Interface ptin_intf=%u/%u (intIfNum=%u) is not root for mcEvcId=%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,igmpInst->McastEvcId,intVlan);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Validate igmp packet checking if the input intIfNum is a 
 * client (leaf) interface and internal Vlan is valid 
 * 
 * @param intIfNum : source interface number
 * @param intVlan  : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS: Parameters are valid
 *                   L7_FAILURE: Not valid
 *  
 * This function will only apply to input packets, and so, input 
 * packets in client/leaf interfaces must use the Unicast vlans. 
 * Therefore we must use the Unicast EVC to validate a client 
 * interface. 
 */
L7_RC_t ptin_igmp_clientIntfVlan_validate(L7_uint32 intIfNum, L7_uint16 intVlan)
{
  st_IgmpInstCfg_t *igmpInst;
  ptin_intf_t ptin_intf;
  ptin_evc_intfCfg_t uc_intf_cfg;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get ptin_intf format for the interface number */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting ptin_intf from intIfNum %u",intIfNum);
    return L7_FAILURE;
  }

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(igmpInst->UcastEvcId,&ptin_intf,&uc_intf_cfg)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting intf configuration for ptin_intf=%u/%u (intIfNum=%u), ucEvcId=%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,igmpInst->UcastEvcId,intVlan);
    return L7_FAILURE;
  }

  /* Interfaces must be in use in both evcs */
  if (!uc_intf_cfg.in_use)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Interface ptin_intf=%u/%u (intIfNum=%u) not in use for ucEvcId=%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,igmpInst->UcastEvcId,intVlan);
    return L7_FAILURE;
  }

  /* Interface must be Root */
  if (uc_intf_cfg.type!=PTIN_EVC_INTF_LEAF)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Interface ptin_intf=%u/%u (intIfNum=%u) is not client/leaf for ucEvcId=%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,igmpInst->UcastEvcId,intVlan);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get the MC root vlan associated to the internal vlan
 * 
 * @param intVlan       : internal vlan
 * @param McastRootVlan : multicast root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_McastRootVlan_get(L7_uint16 intVlan, L7_uint16 *McastRootVlan)
{
  st_IgmpInstCfg_t *igmpInst;
  L7_uint16 intRootVlan;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get Multicast root vlan */
  if (ptin_evc_get_intRootVlan(igmpInst->McastEvcId,&intRootVlan)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting McastRootVlan for MCEvcId=%u (intVlan=%u)",igmpInst->McastEvcId,intVlan);
    return L7_FAILURE;
  }

  /* Return Multicast root vlan */
  if (McastRootVlan!=L7_SUCCESS)  *McastRootVlan = intRootVlan;

  return L7_SUCCESS;
}

/**
 * Get the list of root interfaces associated to a internal vlan
 * 
 * @param intVlan  : Internal vlan
 * @param intfList : List of interfaces
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_rootIntfs_getList(L7_uint16 intVlan, L7_INTF_MASK_t *intfList)
{
  st_IgmpInstCfg_t *igmpInst;
  ptin_HwEthMef10Evc_t evcCfg;
  L7_uint intf_idx;
  L7_uint32 intIfNum;
  ptin_intf_t ptin_intf;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get MC EVC configuration */
  memset(&evcCfg,0x00,sizeof(ptin_HwEthMef10Evc_t));
  evcCfg.index = igmpInst->McastEvcId;
  if (ptin_evc_get(&evcCfg)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting evc %u configuration (intVlan=%u)",igmpInst->McastEvcId,intVlan);
    return L7_FAILURE;
  }

  /* interface list pointer must not be null */
  if (intfList==L7_NULLPTR)
  {
    return L7_SUCCESS;
  }

  /* clear interface list */
  memset(intfList,0x00,sizeof(L7_INTF_MASK_t));

  /* Run all interfaces */
  for (intf_idx=0; intf_idx<evcCfg.n_intf; intf_idx++)
  {
    if (evcCfg.intf[intf_idx].mef_type==PTIN_EVC_INTF_ROOT)
    {
      ptin_intf.intf_type = evcCfg.intf[intf_idx].intf_type;
      ptin_intf.intf_id   = evcCfg.intf[intf_idx].intf_id;

      if (ptin_intf_ptintf2intIfNum(&ptin_intf,&intIfNum)==L7_SUCCESS)
      {
        L7_INTF_SETMASKBIT(*intfList,intIfNum);
      }
      else
      {
        return L7_FAILURE;
      }
    }
  }

  return L7_SUCCESS;
}

/**
 * Get the list of client (leaf) interfaces associated to a 
 * internal vlan 
 * 
 * @param intVlan  : Internal vlan
 * @param intfList : List of interfaces
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_clientIntfs_getList(L7_uint16 intVlan, L7_INTF_MASK_t *intfList)
{
  st_IgmpInstCfg_t *igmpInst;
  ptin_HwEthMef10Evc_t evcCfg;
  L7_uint intf_idx;
  L7_uint32 intIfNum;
  ptin_intf_t ptin_intf;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get MC EVC configuration */
  memset(&evcCfg,0x00,sizeof(ptin_HwEthMef10Evc_t));
  evcCfg.index = igmpInst->UcastEvcId;
  if (ptin_evc_get(&evcCfg)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting evc %u configuration (intVlan=%u)",igmpInst->UcastEvcId,intVlan);
    return L7_FAILURE;
  }

  /* interface list pointer must not be null */
  if (intfList==L7_NULLPTR)
  {
    return L7_SUCCESS;
  }

  /* clear interface list */
  memset(intfList,0x00,sizeof(L7_INTF_MASK_t));

  /* Run all interfaces */
  for (intf_idx=0; intf_idx<evcCfg.n_intf; intf_idx++)
  {
    if (evcCfg.intf[intf_idx].mef_type==PTIN_EVC_INTF_LEAF)
    {
      ptin_intf.intf_type = evcCfg.intf[intf_idx].intf_type;
      ptin_intf.intf_id   = evcCfg.intf[intf_idx].intf_id;

      if (ptin_intf_ptintf2intIfNum(&ptin_intf,&intIfNum)==L7_SUCCESS)
      {
        L7_INTF_SETMASKBIT(*intfList,intIfNum);
      }
      else
      {
        return L7_FAILURE;
      }
    }
  }

  return L7_SUCCESS;
}

/**
 * Get the external outer+inner vlan asociated to the MC EVC
 * 
 * @param intIfNum     : interface number
 * @param intOVlan     : Internal outer vlan 
 * @param intIVlan     : Internal inner vlan 
 * @param extMcastVlan : external vlan associated to MC EVC
 * @param extIVlan     : ext. innerVlan associated to MC EVC
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_extMcastVlan_get(L7_uint32 intIfNum, L7_uint16 intOVlan, L7_uint16 intIVlan, L7_uint16 *extMcastVlan, L7_uint16 *extIVlan)
{
  st_IgmpInstCfg_t *igmpInst;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intOVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intOVlan);
    return L7_FAILURE;
  }

  /* Get external vlans */
  if (ptin_evc_extVlans_get(intIfNum,igmpInst->McastEvcId,intIVlan,extMcastVlan,extIVlan)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting external vlans to intIfNum=%u, intOVlan=%u, intIVlan=%u (MCEvcId=%u)",intIfNum,intOVlan,intIVlan,igmpInst->McastEvcId);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get the external outer+inner vlan asociated to the UC EVC
 * 
 * @param intIfNum     : interface number
 * @param intOVlan     : Internal outer vlan 
 * @param intIVlan     : Internal inner vlan 
 * @param extMcastVlan : external vlan associated to MC EVC
 * @param extIVlan     : ext. innerVlan associated to MC EVC
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_extUcastVlan_get(L7_uint32 intIfNum, L7_uint16 intOVlan, L7_uint16 intIVlan, L7_uint16 *extUcastVlan, L7_uint16 *extIVlan)
{
  st_IgmpInstCfg_t *igmpInst;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intOVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intOVlan);
    return L7_FAILURE;
  }

  /* Get external vlans */
  if (ptin_evc_extVlans_get(intIfNum,igmpInst->UcastEvcId,intIVlan,extUcastVlan,extIVlan)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting external vlans to intIfNum=%u, intOVlan=%u, intIVlan=%u (UCEvcId=%u)",intIfNum,intOVlan,intIVlan,igmpInst->UcastEvcId);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/****************************************************************************** 
 * INTERNAL FUNCTIONS
 ******************************************************************************/

#ifdef CLIENT_TIMERS_SUPPORTED
/*********************************************************************
* @purpose  Start Snooping task
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ptin_igmp_timersMng_init(void)
{
  L7_int igmp_idx;
  L7_uint32 bufferPoolId;
  void *handleListMemHndl;
  handle_list_t *handle_list;
  L7_APP_TMR_CTRL_BLK_t timerCB;

  ptin_igmp_timers_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_igmp_timers_sem == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to create ptin_igmp_timers_sem semaphore!");
    return L7_FAILURE;
  }

  /* Queue that will process timer events */
  clientsMngmt_queue = (void *) osapiMsgQueueCreate("PTin_IGMP_Timer_Queue",
                                PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE, PTIN_IGMP_TIMER_MSG_SIZE);
  if (clientsMngmt_queue == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR,"PTIN Timer msgQueue creation error.");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"PTIN Timer msgQueue created.");

  /* Create task for clients management */
  clientsMngmt_TaskId = osapiTaskCreate("ptin_igmp_clients_task", igmp_timersMng_task, 0, 0,
                                        L7_DEFAULT_STACK_SIZE,
                                        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                        L7_DEFAULT_TASK_SLICE);

  if (clientsMngmt_TaskId == L7_ERROR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Could not create task clientsMngmt_task");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Task clientsMngmt_task created");

  if (osapiWaitForTaskInit (L7_PTIN_IGMP_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Unable to initialize clientsMngmt_task()\n");
    return(L7_FAILURE);
  }
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Task clientsMngmt_task initialized");

  /* Create timer buffer bools, and timer control blocks for each IGMP instance */
  for (igmp_idx=0; igmp_idx<PTIN_SYSTEM_N_IGMP_INSTANCES; igmp_idx++)
  {
    bufferPoolId = 0;
    /* Timer Initializations */
    if(bufferPoolInit(PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE,
                      sizeof(igmpTimerData_t) /*L7_APP_TMR_NODE_SIZE*/,
                      "PTin_IGMP_Timer_Bufs",
                      &bufferPoolId) != L7_SUCCESS)
    {
        LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to allocate memory for IGMP client timer buffers");
        return L7_FAILURE;
    }
    igmpInstances[igmp_idx].igmpClients.appTimerBufferPoolId = bufferPoolId;
    LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Allocate buffer pool for igmp_idx %u",igmp_idx);

    /* Create SLL list for each IGMP instance */
    if (SLLCreate(L7_PTIN_COMPONENT_ID, L7_SLL_NO_ORDER,
                 sizeof(L7_uint32)*2, igmp_timer_dataCmp, igmp_timer_dataDestroy,
                 &(igmpInstances[igmp_idx].igmpClients.ll_timerList)) != L7_SUCCESS)
    {
      LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Failed to create timer linked list");
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_CNFGR,"SLL list created for igmp_idx %u",igmp_idx);

    /* Create timer handles */
    /* Allocate memory for the Handle List */
    handleListMemHndl = (handle_member_t*) osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE*sizeof(handle_member_t));
    if (handleListMemHndl == L7_NULLPTR)
    {
      LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Error allocating Handle List Buffers");
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Allocated memory for handle list in igmp_idx %u",igmp_idx);
    /* Create timers handle list for this IGMP instance  */
    if(handleListInit(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE,
                      &handle_list, handleListMemHndl) != L7_SUCCESS)
    {
      LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Unable to create timer handle list");
      return L7_FAILURE;
    }
    igmpInstances[igmp_idx].igmpClients.appTimer_handleListMemHndl = handleListMemHndl;
    igmpInstances[igmp_idx].igmpClients.appTimer_handle_list = handle_list;
    LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Handle list created for igmp_idx %u",igmp_idx);

    /* Initialize timer control blocks */
    timerCB = appTimerInit(L7_PTIN_COMPONENT_ID, igmp_timerExpiryHdlr,
                           (void *) (igmp_idx), L7_APP_TMR_1SEC,
                           igmpInstances[igmp_idx].igmpClients.appTimerBufferPoolId);
    if (timerCB  == L7_NULLPTR)
    {
      LOG_FATAL(LOG_CTX_PTIN_CNFGR,"snoopEntry App Timer[%d] Initialization Failed.\n",igmp_idx);
      return L7_FAILURE;
    }
    igmpInstances[igmp_idx].igmpClients.timerCB = timerCB;
    LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Timer initialized for igmp_idx %u",igmp_idx);
  }

  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Initializations for IGMP timers finished");

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Start Snooping task
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ptin_igmp_timersMng_deinit(void)
{
  L7_int igmp_idx;

  /* Create timer buffer bools, and timer control blocks for each IGMP instance */
  for (igmp_idx=0; igmp_idx<PTIN_SYSTEM_N_IGMP_INSTANCES; igmp_idx++)
  {
    /* Deinitialize timer control blocks */
    if (igmpInstances[igmp_idx].igmpClients.timerCB != (L7_APP_TMR_CTRL_BLK_t) NULL)
    {
      appTimerDeInit(igmpInstances[igmp_idx].igmpClients.timerCB);
      igmpInstances[igmp_idx].igmpClients.timerCB = (L7_APP_TMR_CTRL_BLK_t) NULL;
    }

    /* Remove timers handle list for this IGMP instance  */
    handleListDeinit(L7_PTIN_COMPONENT_ID, igmpInstances[igmp_idx].igmpClients.appTimer_handle_list);

    /* Free memory for the Handle List */
    if (igmpInstances[igmp_idx].igmpClients.appTimer_handleListMemHndl != L7_NULLPTR)
    {
      osapiFree(L7_PTIN_COMPONENT_ID, igmpInstances[igmp_idx].igmpClients.appTimer_handleListMemHndl);
      igmpInstances[igmp_idx].igmpClients.appTimer_handleListMemHndl = L7_NULLPTR;
    }

    /* Destroy SLL list for each IGMP instance */
    SLLDestroy(L7_PTIN_COMPONENT_ID, &igmpInstances[igmp_idx].igmpClients.ll_timerList);

    /* Buffer pool termination */
    if (igmpInstances[igmp_idx].igmpClients.appTimerBufferPoolId != 0)
    {
      bufferPoolTerminate(igmpInstances[igmp_idx].igmpClients.appTimerBufferPoolId);
      igmpInstances[igmp_idx].igmpClients.appTimerBufferPoolId = 0;
    }
  }

  /* Delete task for clients management */
  if ( clientsMngmt_TaskId != L7_ERROR )
  {
    osapiTaskDelete(clientsMngmt_TaskId);
    clientsMngmt_TaskId = L7_ERROR;
  }

  /* Queue that will process timer events */
  if (clientsMngmt_queue != L7_NULLPTR)
  {
    osapiMsgQueueDelete(clientsMngmt_queue);
    clientsMngmt_queue = L7_NULLPTR;
  }

  osapiSemaDelete(ptin_igmp_timers_sem);
  ptin_igmp_timers_sem = L7_NULLPTR;

  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Deinitializations for IGMP timers finished");

  return(L7_SUCCESS);
}

/**
 * Start a new timer for a recently added client
 * 
 * @param igmp_idx : IGMP instance index
 * @param client_idx : client index
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_igmp_timer_start(L7_uint igmp_idx, L7_uint32 client_idx)
{
  L7_BOOL timer_exists = L7_FALSE;
  igmpTimerData_t *pTimerData, timerData;
  ptinIgmpClients_t *igmpClients;
  L7_uint16 timeout;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to start timer for igmp_idx %u, client_idx=%u",igmp_idx,client_idx);

  /* Validate argument */
  if (client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client index (%u)",client_idx);
    return L7_FAILURE;
  }
  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid igmp instance index (%u)",igmp_idx);
    return L7_FAILURE;
  }
  /* IGMP instance must be in use */
  if (!igmpInstances[igmp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"igmp instance index %u is not in use",igmp_idx);
    return L7_FAILURE;
  }

  igmpClients = &igmpInstances[igmp_idx].igmpClients;

  /* Check if this timer already exists */
  memset(&timerData, 0x00, sizeof(igmpTimerData_t));
  timerData.igmp_idx  = igmp_idx;
  timerData.client_idx = client_idx;

  if ( (pTimerData = (igmpTimerData_t *)SLLFind(&igmpClients->ll_timerList, (void *)&timerData)) != L7_NULLPTR )
  {
    timer_exists = L7_TRUE;
  }

  if ( timer_exists )
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Already exists a timer running for igmp_idx %u, client_idx=%u",igmp_idx,client_idx);

    #if 1
    if (pTimerData->timer != L7_NULL)
    {
      if (appTimerDelete(igmpClients->timerCB, (void *) pTimerData->timerHandle) != L7_SUCCESS)
      {
        if (ptin_debug_igmp_snooping)
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to removing timer");
        return L7_FAILURE;
      }
      pTimerData->timer = L7_NULLPTR;
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer removed!");

      /* Remove timer handle */
      handleListNodeDelete(igmpClients->appTimer_handle_list, &pTimerData->timerHandle);
      pTimerData->timerHandle = 0;
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removed node from handle list (igmp_idx %u, client_idx=%u)",igmp_idx,client_idx);
    }
    #else
    if (pTimerData->timer != L7_NULL)
    {
      /* If exists, only update it */
      if (appTimerUpdate(igmpClients->timerCB, pTimerData->timer,
                         (void *) igmp_timer_expiry, (void *) pTimerData->timerHandle, 30,
                         "PTIN_TIMER") != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to update client timer");
        return L7_FAILURE;
      }
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer updated for igmp_idx %u, client_idx=%u",igmp_idx,client_idx);
      return L7_SUCCESS;
    }
    else
    {
      //handleListNodeDelete(igmpClients->appTimer_handle_list, &pTimerData->timerHandle);
      //pTimerData->timerHandle = 0;
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Handle node removed for igmp_idx %u, client_idx=%u",igmp_idx,client_idx);
    }
    #endif
  }
  else
  {
    if (igmpClients->number_of_clients >= PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not start timer. There is no room for more timers!");
      return L7_FAILURE;
    }

    /* Buffer pool allocation for pTimerData*/
    if (bufferPoolAllocate(igmpClients->appTimerBufferPoolId, (L7_uchar8 **) &pTimerData) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not start timer. Insufficient memory.");
      return L7_FAILURE;
    }
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Allocated one Buffer node (igmp_idx %u, client_idx=%u)",igmp_idx,client_idx);

    /* Timer description */
    pTimerData->igmp_idx = igmp_idx;
    pTimerData->client_idx = client_idx;
  }

  /* New timer handle */
  if ((pTimerData->timerHandle = handleListNodeStore(igmpClients->appTimer_handle_list, pTimerData)) == 0)
  {
    /* Free the previously allocated bufferpool */
    bufferPoolFree(igmpClients->appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not get the handle node to store the timer data.");
    return L7_FAILURE;
  }
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Stored node in handle list (igmp_idx %u, client_idx=%u)",igmp_idx,client_idx);

  timeout = (igmpProxyCfg.querier.group_membership_interval*3)/2;

  /* Add a new timer */
  pTimerData->timer = appTimerAdd( igmpClients->timerCB, igmp_timer_expiry,
                                  (void *) pTimerData->timerHandle, timeout,
                                  "PTIN_TIMER");
  if (pTimerData->timer == NULL)
  {
    /* Free the previously allocated bufferpool */
    handleListNodeDelete(igmpClients->appTimer_handle_list, &pTimerData->timerHandle);
    pTimerData->timerHandle = 0;
    bufferPoolFree(igmpClients->appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not Start the Group timer.");
    return L7_FAILURE;
  }
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer added (igmp_idx %u, client_idx=%u) with timeout=%u",igmp_idx,client_idx,timeout);

  if ( !timer_exists )
  {
    /* Add timer to SLL */
    if (SLLAdd(&igmpClients->ll_timerList, (L7_sll_member_t *)pTimerData) != L7_SUCCESS)
    {
      /* Free the previously allocated bufferpool */
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not add new timer data node");
      if (appTimerDelete( igmpClients->timerCB, pTimerData->timer) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to delete timer");
      }
      pTimerData->timer = L7_NULLPTR;
      handleListNodeDelete(igmpClients->appTimer_handle_list, &pTimerData->timerHandle);
      pTimerData->timerHandle = 0;
      bufferPoolFree(igmpClients->appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
      return L7_FAILURE;
    }
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer started successfully for igmp_idx %u, client_idx=%u",igmp_idx,client_idx);

  return L7_SUCCESS;
}

/**
 * Restart a timer for an existent client
 * 
 * @param igmp_idx : IGMP instance index
 * @param client_idx : client index
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_igmp_timer_update(L7_uint igmp_idx, L7_uint32 client_idx)
{
  igmpTimerData_t timerData, *pTimerData;
  ptinIgmpClients_t *igmpClients;
  L7_uint16 timeout;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to update an existent timer (igmp_idx=%u, client_idx=%u)",igmp_idx,client_idx);

  /* Validate argument */
  if (client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client index (%u)",client_idx);
    return L7_FAILURE;
  }
  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid igmp instance index (%u)",igmp_idx);
    return L7_FAILURE;
  }
  /* IGMP instance must be in use */
  if (!igmpInstances[igmp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"igmp instance index %u is not in use",igmp_idx);
    return L7_FAILURE;
  }

  igmpClients = &igmpInstances[igmp_idx].igmpClients;

  memset(&timerData, 0x00, sizeof(igmpTimerData_t));
  timerData.igmp_idx = igmp_idx;
  timerData.client_idx = client_idx;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Searching for an SLL node (igmp_idx=%u, client_idx=%u)",igmp_idx,client_idx);

  /* Searching for the client timer */
  if ((pTimerData = (igmpTimerData_t *)SLLFind(&igmpClients->ll_timerList, (void *)&timerData)) == L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Client timer not found");
    return L7_FAILURE;
  }

  if (pTimerData->timer == L7_NULL)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Client timer not running");
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Updating timer (igmp_idx=%u, client_idx=%u)",igmp_idx,client_idx);

  timeout = (igmpProxyCfg.querier.group_membership_interval*3)/2;

  if (appTimerUpdate(igmpClients->timerCB, pTimerData->timer,
                     (void *) igmp_timer_expiry, (void *) pTimerData->timerHandle, timeout,
                     "PTIN_TIMER") != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to update group membership timer");
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer updated successfully for igmp_idx=%u, client_idx=%u (timeout=%u)",igmp_idx,client_idx,timeout);

  return L7_SUCCESS;
}

/**
 * Stops a running timer, andf remove all related resources
 * 
 * @param igmp_idx : IGMP instance index
 * @param client_idx : client index
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_igmp_timer_stop(L7_uint igmp_idx, L7_uint32 client_idx)
{
  igmpTimerData_t timerData;
  ptinIgmpClients_t *igmpClients;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to stop a timer (igmp_idx=%u, client_idx=%u)",igmp_idx,client_idx);

  /* Validate argument */
  if (client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client index (%u)",client_idx);
    return L7_FAILURE;
  }
  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid igmp instance index (%u)",igmp_idx);
    return L7_FAILURE;
  }
  /* IGMP instance must be in use */
  if (!igmpInstances[igmp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"igmp instance index %u is not in use",igmp_idx);
    return L7_FAILURE;
  }

  igmpClients = &igmpInstances[igmp_idx].igmpClients;

  memset(&timerData, 0x00, sizeof(igmpTimerData_t));
  timerData.igmp_idx = igmp_idx;
  timerData.client_idx = client_idx;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Deleting SLL node (igmp_idx=%u, client_idx=%u)",igmp_idx,client_idx);

  /* Remove node for SLL list */
  if (SLLDelete(&igmpClients->ll_timerList, (L7_sll_member_t *)&timerData) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to delete timer node");
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer stopped successfully for igmp_idx=%u, client_idx=%u",igmp_idx,client_idx);

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Process the expiry timer events
*
* @param    param    @b{(input)}  Pointer to added group member interface
*                                 timer handle
*
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
void igmp_timer_expiry(void *param)
{
  L7_uint32 timerHandle = (L7_uint32) param;
  L7_uint             igmp_idx, client_idx;

  igmpTimerData_t *pTimerData;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Expiration event ocurred for timerHandle %u!",timerHandle);

  /* Get timer handler */
  pTimerData = (igmpTimerData_t *) handleListNodeRetrieve(timerHandle);
  if (pTimerData == L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to retrieve handle");
    return;
  }

  if (timerHandle != pTimerData->timerHandle)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Weird situation!");
  }

  /* Save client information */
  igmp_idx = pTimerData->igmp_idx;
  client_idx = pTimerData->client_idx;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Expiration event ocurred for igmp_idx=%u, client_idx=%u",igmp_idx,client_idx);

  /* Delete the apptimer */
  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"IGMP index is invalid! (%u)",igmp_idx);
    return;
  }

  /* Delete timer */
  if (appTimerDelete(igmpInstances[igmp_idx].igmpClients.timerCB, pTimerData->timer)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Cannot delete timer (igmp_idx %u, client_idx=%u)",pTimerData->igmp_idx,pTimerData->client_idx);
  }
  pTimerData->timer = (L7_APP_TMR_HNDL_t) NULL;
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer deleted for igmp_idx %u, client_idx=%u",pTimerData->igmp_idx,pTimerData->client_idx);

  /* Delete the handle we had created */
  handleListNodeDelete(igmpInstances[igmp_idx].igmpClients.appTimer_handle_list, &pTimerData->timerHandle);
  pTimerData->timerHandle = 0;

  #if 0
  if (igmp_idx<PTIN_SYSTEM_N_IGMP_INSTANCES && igmpInstances[igmp_idx].inUse)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Deleting SLL node (igmp_idx=%u, client_idx=%u)",igmp_idx,client_idx);
    /* Remove node for SLL list */
    if (SLLDelete(&igmpInstances[igmp_idx].igmpClients.ll_timerList,
                  (L7_sll_member_t *) pTimerData) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to delete timer node");
      return;
    }
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"SLL node deleted (igmp_idx=%u, client_idx=%u)",igmp_idx,client_idx);
  }
  #endif

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removing client (igmp_idx=%u, client_idx=%u)",igmp_idx,client_idx);

  /* Remove client (only if it is dynamic. For static ones, only is removed from snooping entries) */
  if (ptin_igmp_rm_clientIdx(igmp_idx, client_idx, L7_FALSE, L7_TRUE)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed removing client!");
  }
  else
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Client removed (igmp_idx=%u, client_idx=%u)",igmp_idx,client_idx);
  }
}

/**
 * Task that makes the timer processing for the clients manageme
 */
void igmp_timersMng_task(void)
{
  L7_uint32 status;
  ptinIgmpTimerParams_t msg;

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer event process task started");

  if (osapiTaskInitDone(L7_PTIN_IGMP_TASK_SYNC)!=L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_SSM, "Error syncing task");
    PTIN_CRASH();
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer task ready to process events");

  /* Loop */
  while (1)
  {
    //LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer task ready for new events");

    status = (L7_uint32) osapiMessageReceive(clientsMngmt_queue,
                                             (void*)&msg,
                                             PTIN_IGMP_TIMER_MSG_SIZE,
                                             L7_WAIT_FOREVER);

    //LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer task received an event");

    /* TODO: Process message */
    if (status == L7_SUCCESS)
    {
      //LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer event for igmp_idx %u received",msg.igmp_idx);

      if (msg.igmp_idx<PTIN_SYSTEM_N_IGMP_INSTANCES /*&& igmpInstances[msg.igmp_idx].inUse*/)
      {
        //LOG_TRACE(LOG_CTX_PTIN_IGMP,"Processing timer events for igmp_idx %u",msg.igmp_idx);
        appTimerProcess( igmpInstances[msg.igmp_idx].igmpClients.timerCB );
      }
      else
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Bad igmp index %u",msg.igmp_idx);
      }
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"This is an invalid event");
    }
  }
}

/*********************************************************************
* @purpose   This function is used to send timer events
*
* @param     timerCtrlBlk    @b{(input)}   Timer Control Block
* @param     ptrData         @b{(input)}   Ptr to passed data
*
* @returns   None
*
* @notes     None
* @end
*********************************************************************/
void igmp_timerExpiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* ptrData)
{
  L7_RC_t rc;
  L7_uint32 igmp_idx;
  ptinIgmpTimerParams_t msg;

  igmp_idx = (L7_uint32) ptrData;

  //LOG_TRACE(LOG_CTX_PTIN_IGMP,"Received a timer event (igmp_idx %u)",igmp_idx);

  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES /*&& igmpInstances[igmp_idx].inUse*/)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid IGMP index (%u)",igmp_idx);
    return;
  }

  msg.igmp_idx    = igmp_idx;

  //LOG_TRACE(LOG_CTX_PTIN_IGMP,"Sending timer event to queue (igmp_idx %u)",igmp_idx);

  rc = osapiMessageSend(clientsMngmt_queue, &msg, PTIN_IGMP_TIMER_MSG_SIZE, L7_NO_WAIT,L7_MSG_PRIORITY_NORM);

  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Client timer tick send failed");
    return;
  }

  //LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer event sent to queue (igmp_idx %u)",igmp_idx);
}


/*************************************************************************
* @purpose  API to destroy the group membership timer data node
*
* @param    ll_member  @b{(input)}  Linked list node containing the
*                                   timer to be destroyed
*
* @returns  L7_SUCCESS
*
* @comments This is called by SLL library when a node is being deleted
*
* @end
*************************************************************************/
L7_RC_t igmp_timer_dataDestroy (L7_sll_member_t *ll_member)
{
  igmpTimerData_t *pTimerData;
  ptinIgmpClients_t *igmpClients;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to destroy timer");

  /* Validate argument */
  if (ll_member==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Null arguments");
    return L7_FAILURE;
  }

  pTimerData = (igmpTimerData_t *)ll_member;

  /* Validate igmp index */
  if (pTimerData->igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid igmp instance index (%u)",pTimerData->igmp_idx);
    return L7_FAILURE;
  }
  /* IGMP instance must be in use */
  if (!igmpInstances[pTimerData->igmp_idx].inUse)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"igmp instance index %u is not in use",pTimerData->igmp_idx);
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Ready to destroy timer for igmp_idx %u, client_idx=%u",pTimerData->igmp_idx,pTimerData->client_idx);

  igmpClients = &igmpInstances[pTimerData->igmp_idx].igmpClients;

  /* Destroy timer */
  if (pTimerData->timer != L7_NULL)
  {
    /* Delete the apptimer node */
    (void)appTimerDelete(igmpClients->timerCB, pTimerData->timer);
    pTimerData->timer = (L7_APP_TMR_HNDL_t) NULL;

    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer deleted for igmp_idx %u, client_idx=%u",pTimerData->igmp_idx,pTimerData->client_idx);

    /* Delete the handle we had created */
    handleListNodeDelete(igmpClients->appTimer_handle_list, &pTimerData->timerHandle);
    pTimerData->timerHandle = 0;

    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer handle removed for igmp_idx %u, client_idx=%u",pTimerData->igmp_idx,pTimerData->client_idx);
  }
  else
  {
    if (ptin_debug_igmp_snooping)
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Timer not running for igmp_idx %u, client_idx=%u",pTimerData->igmp_idx,pTimerData->client_idx);
  }

  bufferPoolFree(igmpClients->appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Buffer node removed for igmp_idx %u, client_idx=%u",pTimerData->igmp_idx,pTimerData->client_idx);

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer destroyed successfully for igmp_idx %u, client_idx=%u",pTimerData->igmp_idx,pTimerData->client_idx);

  return L7_SUCCESS;
}


/*************************************************************************
* @purpose  Helper API to compare two group membership timer nodes  and
*           return the result
*
* @param     p  @b{(input)}  Pointer to Candidate 1 for comparison
* @param     q  @b{(input)}  Pointer to Candidate 2 for comparison
*
* @returns   0   p = q
* @returns  -1   p < q
* @returns  +1   p > q
*
* @comments This is called by SLL library when a nodes are compared
*
* @end
*************************************************************************/
L7_int32 igmp_timer_dataCmp(void *p, void *q, L7_uint32 key)
{
  if ( ((igmpTimerData_t *) p)->igmp_idx == ((igmpTimerData_t *) q)->igmp_idx &&
       ((igmpTimerData_t *) p)->client_idx == ((igmpTimerData_t *) q)->client_idx )
    return 0;

  if ( ((igmpTimerData_t *) p)->igmp_idx < ((igmpTimerData_t *) q)->igmp_idx )
    return -1;
  if ( ((igmpTimerData_t *) p)->igmp_idx > ((igmpTimerData_t *) q)->igmp_idx )
    return 1;

  if ( ((igmpTimerData_t *) p)->client_idx < ((igmpTimerData_t *) q)->client_idx )
    return -1;

  return 1;
}
#endif

/****************************************************************************** 
 * STATIC FUNCTIONS
 ******************************************************************************/

/**
 * Add a new Multicast client
 * 
 * @param igmp_idx    : IGMP index
 * @param client      : client identification parameters 
 * @param isDynamic   : client type 
 * @param client_idx_ret : client index (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_new_client(L7_uint igmp_idx, ptin_client_id_t *client, L7_BOOL isDynamic, L7_uint *client_idx_ret)
{
  L7_uint client_idx;
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientsAvlTree_t *avl_tree;
  ptinIgmpClientInfoData_t *avl_infoData;
  #if (MC_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port;
  ptin_evc_intfCfg_t intfCfg;
  #endif

  /* Validate igmp index */
  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid IGMP index",igmp_idx);
    return L7_FAILURE;
  }

  /* Check if this instance is in use, and if evc_ids are valid */
  if (!igmpInstances[igmp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Inconsistency: IGMP index %u is not in use",igmp_idx);
    return L7_FAILURE;
  }

  /* Validate client */
  if (client==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Check mask */
  if (MC_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Client mask is null");
    return L7_SUCCESS;
  }

  #if MC_CLIENT_INNERVLAN_SUPPORTED
  /* Do not process null cvlans */
  if ((client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) &&
      (client->innerVlan==0 || client->innerVlan>4095))
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid inner vlan (%u)",client->innerVlan);
    return L7_SUCCESS;
  }
  #endif

  /* Get ptin_port value */
  #if (MC_CLIENT_INTERF_SUPPORTED)
  ptin_port = 0;
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    /* Get interface configuration in the MC EVC */
    if (ptin_evc_intfCfg_get(igmpInstances[igmp_idx].McastEvcId, &client->ptin_intf, &intfCfg)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error acquiring intf configuration for intf %u/%u, evc=%u",
              client->ptin_intf.intf_type,client->ptin_intf.intf_id,igmpInstances[igmp_idx].McastEvcId);
      return L7_FAILURE;
    }
    /* Validate interface configuration in EVC: must be in use, and be a leaf/client */
    if (!intfCfg.in_use || intfCfg.type!=PTIN_EVC_INTF_LEAF)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"intf %u/%u is not in use or is not a leaf in evc %u",
              client->ptin_intf.intf_type,client->ptin_intf.intf_id,igmpInstances[igmp_idx].McastEvcId);
      return L7_FAILURE;
    }
    /* Convert to ptin_port format */
    if (ptin_intf_ptintf2port(&client->ptin_intf,&ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot convert client intf %u/%u to ptin_port format",
              client->ptin_intf.intf_type,client->ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
  #endif

  /* Get new client index */
  if ((client_idx=igmp_clientIndex_get_new(igmp_idx))<0)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot get new client index for igmp_idx=%u (mcEvc=%u)",
            igmp_idx,igmpInstances[igmp_idx].McastEvcId);
    return L7_FAILURE;
  }

  /* Check if this key already exists */
  avl_tree = &igmpInstances[igmp_idx].igmpClients.avlTree;
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  #if (MC_CLIENT_INTERF_SUPPORTED)
  avl_key.ptin_port = ptin_port;
  #endif
  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
  avl_key.outerVlan = (client->mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN) ? client->outerVlan : 0;
  #endif
  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
  avl_key.innerVlan = (client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) ? client->innerVlan : 0;
  #endif
  #if (MC_CLIENT_IPADDR_SUPPORTED)
  avl_key.ipv4_addr = (client->mask & PTIN_CLIENT_MASK_FIELD_IPADDR   ) ? client->ipv4_addr : 0;
  #endif
  #if (MC_CLIENT_MACADDR_SUPPORTED)
  if (client->mask & PTIN_CLIENT_MASK_FIELD_MACADDR)
    memcpy(avl_key.macAddr,client->macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  else
    memset(avl_key.macAddr,0x00,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  #endif

  if (ptin_debug_igmp_snooping)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Key {"
              #if (MC_CLIENT_INTERF_SUPPORTED)
                                "port=%u,"
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                                "svlan=%u,"
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                                "cvlan=%u,"
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
                                "ipAddr=%u.%u.%u.%u,"
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
                                "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
              #endif
                                "} will be added to igmp_idx=%u",
              #if (MC_CLIENT_INTERF_SUPPORTED)
              avl_key.ptin_port,
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              avl_key.outerVlan,
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              avl_key.innerVlan,
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
              #endif
              igmp_idx);
  }

  /* Check if this key already exists */
  if ((avl_infoData=avlSearchLVL7( &(avl_tree->igmpClientsAvlTree), (void *)&avl_key, AVL_EXACT)) != L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"This key {"
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                                    "port=%u,"
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                                    "svlan=%u,"
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                                    "cvlan=%u,"
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                                    "ipAddr=%u.%u.%u.%u,"
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                                    "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
                  #endif
                                    "} already exists in igmp_idx=%u (client_idx=%u)",
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                  avl_key.ptin_port,
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                  avl_key.outerVlan,
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                  avl_key.innerVlan,
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                  (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                  avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
                  #endif
                  igmp_idx,
                  avl_infoData->client_index);
    }

    /* If new type is static, always change to it */
    if (!isDynamic)
    {
      avl_infoData->isDynamic = L7_FALSE;
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Entry is static!");
    }
      /* Output client index */
    if (client_idx_ret!=L7_NULLPTR)
    {
      *client_idx_ret = avl_infoData->client_index;
    }
    return L7_SUCCESS;
  }

  /* Insert entry in AVL tree */
  if (avlInsertEntry(&(avl_tree->igmpClientsAvlTree), (void *)&avl_key)!=L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error inserting key {"
            #if (MC_CLIENT_INTERF_SUPPORTED)
                              "port=%u,"
            #endif
            #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                              "svlan=%u,"
            #endif
            #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                              "cvlan=%u,"
            #endif
            #if (MC_CLIENT_IPADDR_SUPPORTED)
                              "ipAddr=%u.%u.%u.%u,"
            #endif
            #if (MC_CLIENT_MACADDR_SUPPORTED)
                              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
            #endif
                              "} in igmp_idx=%u",
            #if (MC_CLIENT_INTERF_SUPPORTED)
            avl_key.ptin_port,
            #endif
            #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
            avl_key.outerVlan,
            #endif
            #if (MC_CLIENT_INNERVLAN_SUPPORTED)
            avl_key.innerVlan,
            #endif
            #if (MC_CLIENT_IPADDR_SUPPORTED)
            (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
            #endif
            #if (MC_CLIENT_MACADDR_SUPPORTED)
            avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
            #endif
            igmp_idx);
    return L7_FAILURE;
  }

  /* Find the inserted entry */
  if ((avl_infoData=(ptinIgmpClientInfoData_t *) avlSearchLVL7(&(avl_tree->igmpClientsAvlTree),(void *)&avl_key, AVL_EXACT))==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot find key {"
            #if (MC_CLIENT_INTERF_SUPPORTED)
                              "port=%u,"
            #endif
            #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                              "svlan=%u,"
            #endif
            #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                              "cvlan=%u,"
            #endif
            #if (MC_CLIENT_IPADDR_SUPPORTED)
                              "ipAddr=%u.%u.%u.%u,"
            #endif
            #if (MC_CLIENT_MACADDR_SUPPORTED)
                              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
            #endif
                              "} in igmp_idx=%u",
            #if (MC_CLIENT_INTERF_SUPPORTED)
            avl_key.ptin_port,
            #endif
            #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
            avl_key.outerVlan,
            #endif
            #if (MC_CLIENT_INNERVLAN_SUPPORTED)
            avl_key.innerVlan,
            #endif
            #if (MC_CLIENT_IPADDR_SUPPORTED)
            (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
            #endif
            #if (MC_CLIENT_MACADDR_SUPPORTED)
            avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
            #endif
            igmp_idx);
    return L7_FAILURE;
  }

  /* Update client index in data cell */
  avl_infoData->client_index = client_idx;

  /* Dynamic entry? */
  avl_infoData->isDynamic = isDynamic & 1;

  /* Mark one more client for AVL tree */
  igmp_clientIndex_mark(igmp_idx,client_idx,avl_infoData);

  /* Clear igmp statistics */
  osapiSemaTake(ptin_igmp_stats_sem,-1);
  memset(&avl_infoData->stats_client,0x00,sizeof(ptin_IGMP_Statistics_t));
  osapiSemaGive(ptin_igmp_stats_sem);

  if (ptin_debug_igmp_snooping)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success inserting Key {"
              #if (MC_CLIENT_INTERF_SUPPORTED)
                                "port=%u,"
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                                "svlan=%u,"
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                                "cvlan=%u,"
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
                                "ipAddr=%u.%u.%u.%u,"
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
                                "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
              #endif
                                "} in igmp_idx=%u (entry is %s)",
              #if (MC_CLIENT_INTERF_SUPPORTED)
              avl_key.ptin_port,
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              avl_key.outerVlan,
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              avl_key.innerVlan,
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
              #endif
              igmp_idx,
              ((isDynamic) ? "dynamic" : "static"));
  }

  /* Output client index */
  if (client_idx_ret!=L7_NULLPTR)
  {
    *client_idx_ret = client_idx;
  }

  return L7_SUCCESS;
}

/**
 * Remove a Multicast client
 * 
 * @param igmp_idx    : IGMP index
 * @param client      : client identification parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_rm_client(L7_uint igmp_idx, ptin_client_id_t *client, L7_BOOL remove_static)
{
  L7_uint16 McastRootVlan;
  L7_uint32 intIfNum;
  L7_uint   client_idx;
  ptinIgmpClientDataKey_t   avl_key;
  ptinIgmpClientsAvlTree_t *avl_tree;
  ptinIgmpClientInfoData_t *avl_infoData;
  #if (MC_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port;
  #endif

  /* Validate igmp index */
  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid IGMP index",igmp_idx);
    return L7_FAILURE;
  }

  /* Check if this instance is in use, and if evc_ids are valid */
  if (!igmpInstances[igmp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Inconsistency: IGMP index %u is not in use",igmp_idx);
    return L7_FAILURE;
  }

  /* Validate client */
  if (client==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Check mask */
  if (MC_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Client mask is null");
    return L7_SUCCESS;
  }

  #if MC_CLIENT_INNERVLAN_SUPPORTED
  /* Do not process null cvlans */
  if ((client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) &&
      (client->innerVlan==0 || client->innerVlan>4095))
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid inner vlan (%u)",client->innerVlan);
    return L7_SUCCESS;
  }
  #endif

  /* Get Multicast root vlan */
  if (ptin_evc_get_intRootVlan(igmpInstances[igmp_idx].McastEvcId,&McastRootVlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting McastRootVlan for MCEvcId=%u",igmpInstances[igmp_idx].McastEvcId);
    return L7_FAILURE;
  }

  /* Convert interface to ptin_port format */
  intIfNum = 0;
  #if (MC_CLIENT_INTERF_SUPPORTED)
  ptin_port = 0;
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    if (ptin_intf_ptintf2port(&client->ptin_intf,&ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot convert client intf %u/%u to ptin_port format",client->ptin_intf.intf_type,client->ptin_intf.intf_id);
      return L7_FAILURE;
    }
    if (ptin_intf_ptintf2intIfNum(&client->ptin_intf,&intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot convert client intf %u/%u to intIfNum format",client->ptin_intf.intf_type,client->ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
  #endif

  /* Check if this key does not exists */

  avl_tree = &igmpInstances[igmp_idx].igmpClients.avlTree;
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  #if (MC_CLIENT_INTERF_SUPPORTED)
  avl_key.ptin_port = ptin_port;
  #endif
  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
  avl_key.outerVlan = (client->mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN) ? client->outerVlan : 0;
  #endif
  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
  avl_key.innerVlan = (client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) ? client->innerVlan : 0;
  #endif
  #if (MC_CLIENT_IPADDR_SUPPORTED)
  avl_key.ipv4_addr = (client->mask & PTIN_CLIENT_MASK_FIELD_IPADDR   ) ? client->ipv4_addr : 0;
  #endif
  #if (MC_CLIENT_MACADDR_SUPPORTED)
  if (client->mask & PTIN_CLIENT_MASK_FIELD_MACADDR)
    memcpy(avl_key.macAddr,client->macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  else
    memset(avl_key.macAddr,0x00,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  #endif

  if (ptin_debug_igmp_snooping)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Key to search {"
              #if (MC_CLIENT_INTERF_SUPPORTED)
                                "port=%u,"
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                                "svlan=%u,"
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                                "cvlan=%u,"
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
                                "ipAddr=%u.%u.%u.%u,"
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
                                "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
              #endif
                                "} in igmp_idx=%u",
              #if (MC_CLIENT_INTERF_SUPPORTED)
              avl_key.ptin_port,
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              avl_key.outerVlan,
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              avl_key.innerVlan,
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
              #endif
              igmp_idx);
  }

  /* Check if this entry does not exist in AVL tree */
  if ((avl_infoData=(ptinIgmpClientInfoData_t *) avlSearchLVL7( &(avl_tree->igmpClientsAvlTree), (void *)&avl_key, AVL_EXACT))==L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"This key {"
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                                    "port=%u,"
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                                    "svlan=%u,"
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                                    "cvlan=%u,"
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                                    "ipAddr=%u.%u.%u.%u,"
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                                    "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
                  #endif
                                    "} does not exist in igmp_idx=%u",
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                  avl_key.ptin_port,
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                  avl_key.outerVlan,
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                  avl_key.innerVlan,
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                  (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                  avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
                  #endif
                  igmp_idx);
    }
    return L7_NOT_EXIST;
  }

  /* Extract client index */
  client_idx = avl_infoData->client_index;

#ifdef CLIENT_TIMERS_SUPPORTED
  /* Stop timers related to this client */
  osapiSemaTake(ptin_igmp_timers_sem, L7_WAIT_FOREVER);
  if (ptin_igmp_timer_stop(igmp_idx, client_idx)!=L7_SUCCESS)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Error stoping timer for client in igmp_idx=%u and client_idx=%u)",igmp_idx,client_idx);
    //osapiSemaGive(ptin_igmp_timers_sem);
    //return L7_FAILURE;
  }
  osapiSemaGive(ptin_igmp_timers_sem);
#endif

  /* Only remove entry if it is dynamic, or else, if the remove_static flag is given */
  if (remove_static || avl_infoData->isDynamic)
  {
    /* Remove entry from AVL tree */
    if (avlDeleteEntry(&(avl_tree->igmpClientsAvlTree), (void *)&avl_key)==L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing key {"
              #if (MC_CLIENT_INTERF_SUPPORTED)
                                "port=%u,"
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                                "svlan=%u,"
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                                "cvlan=%u,"
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
                                "ipAddr=%u.%u.%u.%u,"
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
                                "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
              #endif
                                "} from igmp_idx=%u",
              #if (MC_CLIENT_INTERF_SUPPORTED)
              avl_key.ptin_port,
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              avl_key.outerVlan,
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              avl_key.innerVlan,
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
              #endif
              igmp_idx);
      return L7_FAILURE;
    }

    /* Remove client for AVL tree */
    igmp_clientIndex_unmark(igmp_idx,client_idx);

    if (ptin_debug_igmp_snooping)
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success removing Key {"
                #if (MC_CLIENT_INTERF_SUPPORTED)
                                  "port=%u,"
                #endif
                #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                                  "svlan=%u,"
                #endif
                #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                                  "cvlan=%u,"
                #endif
                #if (MC_CLIENT_IPADDR_SUPPORTED)
                                  "ipAddr=%u.%u.%u.%u,"
                #endif
                #if (MC_CLIENT_MACADDR_SUPPORTED)
                                  "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
                #endif
                                  "} from igmp_idx=%u",
                #if (MC_CLIENT_INTERF_SUPPORTED)
                avl_key.ptin_port,
                #endif
                #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                avl_key.outerVlan,
                #endif
                #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                avl_key.innerVlan,
                #endif
                #if (MC_CLIENT_IPADDR_SUPPORTED)
                (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
                #endif
                #if (MC_CLIENT_MACADDR_SUPPORTED)
                avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
                #endif
                igmp_idx);
    }
  }

  /* Remove client from all snooping entries */
  if (ptin_snoop_client_remove(McastRootVlan,client_idx,intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing client from snooping entries");
    //return L7_FAILURE;
  }
  else
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Client removed from snooping entries");
  }

  return L7_SUCCESS;
}

/**
 * Remove all Multicast clients 
 * 
 * @param igmp_idx    : Igmp index
 * @param isDynamic   : Remove only this type of clients 
 *                      (static=0/dynamic=1)
 * @param only_wo_channels:only remove clients without channels.
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_rm_all_clients(L7_uint igmp_idx, L7_BOOL isDynamic, L7_BOOL only_wo_channels)
{
  L7_uint16 McastRootVlan;
  L7_uint   client_idx;
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientsAvlTree_t *avl_tree;
  ptinIgmpClientInfoData_t *avl_infoData;
  L7_uint32 intIfNum;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate igmp index */
  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid IGMP index",igmp_idx);
    return L7_FAILURE;
  }

  /* Check if this instance is in use, and if evc_ids are valid */
  if (!igmpInstances[igmp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Inconsistency: IGMP index %u is not in use",igmp_idx);
    return L7_FAILURE;
  }

  /* Get Multicast root vlan */
  if (ptin_evc_get_intRootVlan(igmpInstances[igmp_idx].McastEvcId,&McastRootVlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting McastRootVlan for MCEvcId=%u",igmpInstances[igmp_idx].McastEvcId);
    return L7_FAILURE;
  }

  /* AVL tree refrence */
  avl_tree = &igmpInstances[igmp_idx].igmpClients.avlTree;

  /* Get all clients */
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  while ( (avl_infoData=(ptinIgmpClientInfoData_t *) avlSearchLVL7(&(avl_tree->igmpClientsAvlTree), &avl_key, L7_MATCH_GETNEXT))!=L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_infoData->igmpClientDataKey, sizeof(ptinIgmpClientDataKey_t));

    /* Client type must match to proceed */
    if (isDynamic != avl_infoData->isDynamic)
      continue;

    /* Check only_wo_channels parameter */
    if (only_wo_channels && avl_infoData->stats_client.active_groups>0)
      continue;

    /* Convert interface port to intIfNum format */
    intIfNum = 0;
    #if (MC_CLIENT_INTERF_SUPPORTED)
    if (ptin_intf_port2intIfNum(avl_infoData->igmpClientDataKey.ptin_port, &intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot convert client port %u to intIfNum format",avl_infoData->igmpClientDataKey.ptin_port);
      continue;
    }
    #endif

    /* Save client index */
    client_idx = avl_infoData->client_index;

  #ifdef CLIENT_TIMERS_SUPPORTED
    /* Stop timers */
    osapiSemaTake(ptin_igmp_timers_sem, L7_WAIT_FOREVER);
    if (ptin_igmp_timer_stop(igmp_idx,client_idx)!=L7_SUCCESS)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Error stoping timer for client in igmp_idx=%u and client_idx=%u)",igmp_idx,client_idx);
      //rc = L7_FAILURE;
    }
    osapiSemaGive(ptin_igmp_timers_sem);
  #endif

    /* Remove this entry */
    if (avlDeleteEntry(&(avl_tree->igmpClientsAvlTree), (void *)&avl_key)==L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing key {"
              #if (MC_CLIENT_INTERF_SUPPORTED)
                                "port=%u,"
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                                "svlan=%u,"
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                                "cvlan=%u,"
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
                                "ipAddr=%u.%u.%u.%u,"
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
                                "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
              #endif
                                "} from igmp_idx=%u",
              #if (MC_CLIENT_INTERF_SUPPORTED)
              avl_key.ptin_port,
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              avl_key.outerVlan,
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              avl_key.innerVlan,
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
              #endif
              igmp_idx);
        rc = L7_FAILURE;
    }
    else
    {
      /* Remove client for AVL tree */
      igmp_clientIndex_unmark(igmp_idx,client_idx);

      if (ptin_debug_igmp_snooping)
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success removing Key {"
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                                    "port=%u,"
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                                    "svlan=%u,"
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                                    "cvlan=%u,"
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                                    "ipAddr=%u.%u.%u.%u,"
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                                    "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
                  #endif
                                    "} from igmp_idx=%u",
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                  avl_key.ptin_port,
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                  avl_key.outerVlan,
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                  avl_key.innerVlan,
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                  (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                  avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
                  #endif
                  igmp_idx);
      }

      if (!only_wo_channels)
      {
        /* Remove client from all snooping entries */
        if (ptin_snoop_client_remove(McastRootVlan,client_idx,intIfNum)!=L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing client from snooping entries");
        }
        else
        {
          if (ptin_debug_igmp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_IGMP,"Client removed from snooping entries");
        }
      }
    }
  }

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"An error ocurred during clients remotion.");
  }
  else
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Clients removed!");
  }

  return rc;
}

/**
 * Remove a particular client
 * 
 * @param igmp_idx      : IGMP instance index
 * @param client_index  : Client index 
 * @param remove_static : Remove all types of clients
 * @param force_remove  : Only remove if the client is not 
 *                        watching any channel
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_rm_clientIdx(L7_uint igmp_idx, L7_uint client_idx, L7_BOOL remove_static, L7_BOOL force_remove )
{
  L7_uint32 intIfNum;
  L7_uint16 McastRootVlan;
  st_IgmpInstCfg_t         *igmpInst;
  ptinIgmpClientInfoData_t *clientInfo;
  ptinIgmpClientDataKey_t  *avl_key;
  ptinIgmpClientsAvlTree_t *avl_tree;
  L7_RC_t rc;

  /* Validate arguments */
  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid igmp instance (%u)",igmp_idx);
    return L7_FAILURE;
  }
  if (!igmpInstances[igmp_idx].inUse)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"IGMP index %u not in use",igmp_idx);
    return L7_FAILURE;
  }
  if ( client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE )
  {
    //if (ptin_debug_igmp_snooping)
    //  LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* IGMP instance */
  igmpInst = &igmpInstances[igmp_idx];

  /* Get pointer to client structure in AVL tree */
  clientInfo = igmpInst->igmpClients.clients_in_use[client_idx];
  /* If does not exist... */
  if (clientInfo==L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Provided client_idx (%u) does not exist",client_idx);
    return L7_FAILURE;
  }

  /* If there is no channels, or channels are forced to be removed... */
  if ( force_remove || clientInfo->stats_client.active_groups==0 )
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Stopping timer");

  #ifdef CLIENT_TIMERS_SUPPORTED
    /* Stop timers related to this client */
    osapiSemaTake(ptin_igmp_timers_sem, L7_WAIT_FOREVER);
    if (ptin_igmp_timer_stop(igmp_idx, client_idx)!=L7_SUCCESS)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Cannot stop timer for client in igmp_idx=%u and client_idx=%u)",igmp_idx,client_idx);
      //return L7_FAILURE;
    }
    osapiSemaGive(ptin_igmp_timers_sem);
  #endif

    /* Remove client if channel is dynamic, or if remove_static flag is given */
    if ( remove_static || clientInfo->isDynamic )
    {
      avl_key  = (ptinIgmpClientDataKey_t *) clientInfo;
      avl_tree = &igmpInst->igmpClients.avlTree;

      /* Remove this client */
      if (avlDeleteEntry(&(avl_tree->igmpClientsAvlTree), (void *) avl_key)==L7_NULLPTR)
      {
        if (ptin_debug_igmp_snooping)
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Error flushing key {"
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                                    "port=%u,"
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                                    "svlan=%u,"
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                                    "cvlan=%u,"
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                                    "ipAddr=%u.%u.%u.%u,"
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                                    "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
                  #endif
                                    "} from igmp_idx=%u",
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                  avl_key->ptin_port,
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                  avl_key->outerVlan,
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                  avl_key->innerVlan,
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                  (avl_key->ipv4_addr>>24) & 0xff, (avl_key->ipv4_addr>>16) & 0xff, (avl_key->ipv4_addr>>8) & 0xff, avl_key->ipv4_addr & 0xff,
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                  avl_key->macAddr[0],avl_key->macAddr[1],avl_key->macAddr[2],avl_key->macAddr[3],avl_key->macAddr[4],avl_key->macAddr[5],
                  #endif
                  igmp_idx);
        return L7_FAILURE;
      }

      /* Remove client for AVL tree */
      igmp_clientIndex_unmark(igmp_idx,client_idx);
    }
  }

  /* Only clean channels associated to this client, if force_remove flag is given */
  if ( force_remove )
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to remove channels");

    rc = L7_SUCCESS;

    /* Get Multicast root vlan */
    if ( (ptin_evc_get_intRootVlan(igmpInst->McastEvcId, &McastRootVlan))!=L7_SUCCESS )
    {
      if (ptin_debug_igmp_snooping)
        LOG_WARNING(LOG_CTX_PTIN_IGMP,"Error getting root vlan of MC EVC id (%u)",igmpInst->McastEvcId);
      rc = L7_FAILURE;
    }
    /* Get intIfNum related to this client */
    intIfNum = 0;
    #if (MC_CLIENT_INTERF_SUPPORTED)
    if ( ptin_intf_port2intIfNum(clientInfo->igmpClientDataKey.ptin_port, &intIfNum)!=L7_SUCCESS )
    {
      if (ptin_debug_igmp_snooping)
        LOG_WARNING(LOG_CTX_PTIN_IGMP,"Error getting port of this client");
      rc = L7_FAILURE;
    }
    #endif

    if ( rc == L7_SUCCESS )
    {
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Proceeding for snoop channels remotion: McastVlan=%u (evcId=%u), intIfNum=%u (port=%u)",
                  McastRootVlan,igmpInst->McastEvcId,intIfNum,clientInfo->igmpClientDataKey.ptin_port);

      /* Remove client from all snooping entries */
      if (ptin_snoop_client_remove(McastRootVlan,client_idx,intIfNum)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing client from snooping entries");
      }
      else
      {
        if (ptin_debug_igmp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Client removed from snooping entries");
      }
    }
    else
    {
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Cannot proceed to snoop channels remotion");
    }
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success flushing client (igmp_idx=%u, client_idx=%u)",igmp_idx,client_idx);

  return L7_SUCCESS;
}


/**
 * Get IGMP instance from internal vlan
 * 
 * @param intVlan      : internal vlan
 * @param igmpInst     : igmp instance 
 * @param igmpInst_idx : igmp instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_inst_get_fromIntVlan(L7_uint16 intVlan, st_IgmpInstCfg_t **igmpInst, L7_uint *igmpInst_idx)
{
  L7_uint16 evc_idx, igmp_idx;

  /* Verify if this internal vlan is associated to an EVC */
  if (ptin_evc_get_evcIdfromIntVlan(intVlan,&evc_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No EVC associated to internal vlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Check if the EVC has a IGMP instance */
  igmp_idx = igmpInst_fromEvcId[evc_idx];

  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to evcId=%u (intVlan=%u)",evc_idx,intVlan);
    return L7_FAILURE;
  }

  /* Check if this instance is in use, and if evc_ids are valid */
  if (!igmpInstances[igmp_idx].inUse)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Inconsistency: IGMP index %u (EVCid=%u, Vlan %u) is not in use",igmp_idx,evc_idx,intVlan);
    return L7_FAILURE;
  }

  /* Check if EVCs are in use */
  if (!ptin_evc_is_in_use(igmpInstances[igmp_idx].McastEvcId) ||
      !ptin_evc_is_in_use(igmpInstances[igmp_idx].UcastEvcId))
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Inconsistency: IGMP index %u (EVCid=%u, Vlan %u) has EVCs not in use (UC,MC=%u,%u)",igmp_idx,evc_idx,intVlan,igmpInstances[igmp_idx].McastEvcId,igmpInstances[igmp_idx].UcastEvcId);
    return L7_FAILURE;
  }

  /* Return igmp instance */
  if (igmpInst!=L7_NULLPTR)     *igmpInst     = &igmpInstances[igmp_idx];
  if (igmpInst_idx!=L7_NULLPTR) *igmpInst_idx = igmp_idx;

  return L7_SUCCESS;
}

static L7_RC_t ptin_igmp_global_configuration(void)
{
  L7_uint igmp_idx;

  if (igmpProxyCfg.admin)
  {
    // Querier IP address
    if (usmDbSnoopQuerierAddressSet( (void *) &igmpProxyCfg.ipv4_addr, L7_AF_INET)!=L7_SUCCESS)  {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopQuerierAddressSet");
      return L7_FAILURE;
    }
    // IGMP version
    if (usmDbSnoopQuerierVersionSet( igmpProxyCfg.version, L7_AF_INET)!=L7_SUCCESS)  {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopQuerierVersionSet");
      return L7_FAILURE;
    }
    // Querier interval
    if (usmDbSnoopQuerierQueryIntervalSet( igmpProxyCfg.querier.query_interval, L7_AF_INET)!=L7_SUCCESS)  {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopQuerierQueryIntervalSet");
      return L7_FAILURE;
    }

    LOG_TRACE(LOG_CTX_PTIN_IGMP,"IPv4 address set to %u.%u.%u.%u",
              (igmpProxyCfg.ipv4_addr.s_addr>>24) & 0xff,
              (igmpProxyCfg.ipv4_addr.s_addr>>16) & 0xff,
              (igmpProxyCfg.ipv4_addr.s_addr>> 8) & 0xff,
               igmpProxyCfg.ipv4_addr.s_addr & 0xff);
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMP version set to %u",igmpProxyCfg.version);
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Querier interval set to %u",igmpProxyCfg.querier.query_interval);
  }

  // Attrib IGMP packets priority
  if ( usmDbSnoopPrioModeSet(1, igmpProxyCfg.igmp_cos, L7_AF_INET) != L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopPrioModeSet");
    return L7_FAILURE;
  }

  // Snooping global activation
  if (usmDbSnoopAdminModeSet( 1, igmpProxyCfg.admin, L7_AF_INET)!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopAdminModeSet");
    return L7_FAILURE;
  }

  // Querier admin
  if (usmDbSnoopQuerierAdminModeSet(igmpProxyCfg.admin, L7_AF_INET)!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopQuerierAdminModeSet");
    if (igmpProxyCfg.admin)  usmDbSnoopAdminModeSet( 1, L7_DISABLE, L7_AF_INET);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMP snooping enable set to %u",igmpProxyCfg.admin);

  /* Run all IGMP instances and reset the IP address for each one */
  for (igmp_idx=0; igmp_idx<PTIN_SYSTEM_N_IGMP_INSTANCES; igmp_idx++)
  {
    if (igmpInstances[igmp_idx].inUse)
    {
      if (ptin_igmp_querier_configure(igmp_idx,L7_ENABLE)!=L7_SUCCESS)
        return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

static L7_RC_t ptin_igmp_trap_configure(L7_uint igmp_idx, L7_BOOL enable)
{
  L7_uint16   idx, vlan, uc_evcId, mc_evcId;
  ptin_HwEthMef10Evc_t evcCfg;
  L7_uint16 vlans_number, vlan_list[PTIN_SYSTEM_MAX_N_PORTS];
#if (!PTIN_SYSTEM_GROUP_VLANS)
  ptin_intf_t          ptin_intf;
  L7_uint16            intf_idx;
  ptin_evc_intfCfg_t   intfCfg;
#endif

  enable &= 1;

  /* Validate argument */
  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid igmp instance index (%u)",igmp_idx);
    return L7_FAILURE;
  }
  /* IGMP instance must be in use */
  if (!igmpInstances[igmp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"igmp instance index %u is not in use",igmp_idx);
    return L7_FAILURE;
  }

  /* Initialize number of vlans to be configured */
  vlans_number = 0;

  mc_evcId = igmpInstances[igmp_idx].McastEvcId;
  uc_evcId = igmpInstances[igmp_idx].UcastEvcId;

  /* Get root vlan for MC evc, and add it for packet trapping */
  if (ptin_evc_get_intRootVlan(mc_evcId,&vlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Can't get MC root vlan for evc id %u",mc_evcId);
    return L7_FAILURE;
  }
  if (vlan>=PTIN_VLAN_MIN && vlan<=PTIN_VLAN_MAX)
  {
    vlan_list[vlans_number++] = vlan;
  }

  /* Get Unicast EVC configuration */
  evcCfg.index = uc_evcId;
  if (ptin_evc_get(&evcCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting UC EVC %u configuration",uc_evcId);
    return L7_FAILURE;
  }
#if (!PTIN_SYSTEM_GROUP_VLANS)
  /* If UC EVC is stacked, use its root vlan */
  if (evcCfg.flags & PTIN_EVC_MASK_STACKED)
#endif
  {
    if (ptin_evc_get_intRootVlan(uc_evcId,&vlan)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Can't get UC root vlan for evc id %u",uc_evcId);
      return L7_FAILURE;
    }
    if (vlan>=PTIN_VLAN_MIN && vlan<=PTIN_VLAN_MAX)
    {
      /* Verify if this vlan is scheduled to be configured */
      for (idx=0; idx<vlans_number; idx++)
      {
        if (vlan_list[idx]==vlan)  break;
      }
      /* If not found, add this vlan */
      if (idx>=vlans_number)
      {
        vlan_list[vlans_number++] = vlan;
      }
    }
  }
#if (!PTIN_SYSTEM_GROUP_VLANS)
  /* If unstacked, use leaf vlans */
  else
  {
    /* Run all interfaces, and get its configurations */
    for (intf_idx=0; intf_idx<evcCfg.n_intf; intf_idx++)
    {
      /* Only leaf interfaces are considered */
      if (evcCfg.intf[intf_idx].mef_type!=PTIN_EVC_INTF_LEAF)
        continue;

      /* Get interface configuarions */
      ptin_intf.intf_type = evcCfg.intf[intf_idx].intf_type;
      ptin_intf.intf_id   = evcCfg.intf[intf_idx].intf_id;
      if (ptin_evc_intfCfg_get(uc_evcId, &ptin_intf, &intfCfg)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting interface %u/%u configuration from UC EVC %u",ptin_intf.intf_type,ptin_intf.intf_id,uc_evcId);
        return L7_FAILURE;
      }
      /* Extract internal vlan */
      vlan = intfCfg.int_vlan;
      if (vlan>=PTIN_VLAN_MIN && vlan<=PTIN_VLAN_MAX)
      {
        /* Verify if this vlan is scheduled to be configured */
        for (idx=0; idx<vlans_number; idx++)
        {
          if (vlan_list[idx]==vlan)  break;
        }
        if (idx<vlans_number)  continue;

        /* Can this vlan be configured? */
        if (vlans_number>=PTIN_SYSTEM_MAX_N_PORTS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Excessive number of vlans to be configured (morte than %u)",PTIN_SYSTEM_MAX_N_PORTS);
          return L7_FAILURE;
        }

        /* Schedule this vlan to be configured */
        vlan_list[vlans_number++] = vlan;
      }
    }
  }
#endif

  /* Configure vlans */
  for (idx=0; idx<vlans_number; idx++)
  {
    if (usmDbSnoopVlanModeSet(1,vlan_list[idx],enable,L7_AF_INET)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring vlan %u for packet trapping",vlan_list[idx]);
      break;
    }
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success configuring vlan %u for packet trapping",vlan_list[idx]);
  }
  /* If something went wrong, undo configurations */
  if (idx<vlans_number)
  {
    vlans_number = idx;
    for (idx=0; idx<vlans_number; idx++)
    {
      usmDbSnoopVlanModeSet(1,vlan_list[idx],!enable,L7_AF_INET);
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"Unconfiguring vlan %u for packet trapping",vlan_list[idx]);
    }
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

static L7_RC_t ptin_igmp_querier_configure(L7_uint igmp_idx, L7_BOOL enable)
{
  L7_uint16 vlan;

  enable &= 1;

  /* Validate argument */
  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid igmp instance index (%u)",igmp_idx);
    return L7_FAILURE;
  }
  /* IGMP instance must be in use */
  if (!igmpInstances[igmp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"igmp instance index %u is not in use",igmp_idx);
    return L7_FAILURE;
  }
  /* Get root vlan for MC evc */
  if (ptin_evc_get_intRootVlan(igmpInstances[igmp_idx].McastEvcId,&vlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Can't get MC root vlan for igmp_idx %u",igmp_idx);
    return L7_FAILURE;
  }

  /* Configure vlan */
  if (usmDbSnoopQuerierVlanModeSet(vlan,enable,L7_AF_INET))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopQuerierVlanModeSet for vlan %u",vlan);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Vlan %u querier mode set to %u",vlan,enable);

  if (enable)
  {
    /* Configure querier address for this vlan */
    if (usmDbSnoopQuerierVlanAddressSet(vlan,(void *) &igmpProxyCfg.ipv4_addr.s_addr,L7_AF_INET)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopQuerierVlanAddressSet for vlan %u",vlan);
      usmDbSnoopQuerierVlanModeSet(vlan,L7_DISABLE,L7_AF_INET);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Address for vlan %u configured to %u.%u.%u.%u",
              vlan,
              (igmpProxyCfg.ipv4_addr.s_addr>>24) & 0xff,
              (igmpProxyCfg.ipv4_addr.s_addr>>16) & 0xff,
              (igmpProxyCfg.ipv4_addr.s_addr>>8) & 0xff,
              (igmpProxyCfg.ipv4_addr.s_addr) & 0xff);

    /* Election mode */
    if (usmDbSnoopQuerierVlanElectionModeSet(vlan,L7_ENABLE,L7_AF_INET)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopQuerierVlanElectionModeSet for vlan %u",vlan);
      usmDbSnoopQuerierVlanModeSet(vlan,L7_DISABLE,L7_AF_INET);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Election mode for vlan %u set to enable",vlan);
  }

  return L7_SUCCESS;
}

/**
 * Remove all Multicast client
 * 
 * @param igmp_idx : IGMP instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_instance_deleteAll_clients(L7_uint igmp_idx)
{
  L7_uint16 McastRootVlan;
  ptinIgmpClientsAvlTree_t *avl_tree;

  /* Validate argument */
  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid igmp instance index (%u)",igmp_idx);
    return L7_FAILURE;
  }
  /* IGMP instance must be in use */
  if (!igmpInstances[igmp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"igmp instance index %u is not in use",igmp_idx);
    return L7_FAILURE;
  }

  /* Get Multicast root vlan */
  if (ptin_evc_get_intRootVlan(igmpInstances[igmp_idx].McastEvcId,&McastRootVlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting McastRootVlan for MCEvcId=%u",igmpInstances[igmp_idx].McastEvcId);
    return L7_FAILURE;
  }

  avl_tree = &igmpInstances[igmp_idx].igmpClients.avlTree;

  /* Remove all entries from AVL tree */
  avlPurgeAvlTree(&(avl_tree->igmpClientsAvlTree), PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE);

  /* Remove all clients for AVL tree */
  igmp_clientIndex_clearAll(igmp_idx);

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success removing all clients from igmp_idx=%u",igmp_idx);

  /* Remove all channels from snooping entries */
  if (ptin_snoop_channel_removeAll(McastRootVlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing all channels/clients from snooping entries");
    //return L7_FAILURE;
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"All channels/clients removed from snooping entries");
  }

  return L7_SUCCESS;
}

/**
 * Validate IGMP instance configure message 
 * 1. VLANs ranges 
 * 2. VLAN/interface overlapping between multiple instances 
 * 3. Get internal VLANs for Router MC and Clients UC VLANs 
 * 
 * @param igmpInst Pointer to an instance message struct
 * @param idx      Index of the IGMP instance if found
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
//static L7_RC_t ptin_igmp_inst_validate(ptin_IgmpInstCfg_t *igmpInst, L7_uint *idx)
//{
//  return L7_SUCCESS;
//}


/**
 * Gets a free IGMP instance entry
 * 
 * @param igmp_idx : Output index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_instance_find_free(L7_uint *igmp_idx)
{
  L7_uint idx;

  /* Search for the first empty instance */
  for (idx=0; idx<PTIN_SYSTEM_N_IGMP_INSTANCES && igmpInstances[idx].inUse; idx++);

  /* If not found empty instances, return error */
  if (idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
    return L7_FAILURE;

  /* Return instance index */
  if (igmp_idx!=L7_NULLPTR)  *igmp_idx = idx;

  return L7_SUCCESS;
}

/**
 * Gets the IGMP instance with a specific Mcast and Ucast EVC 
 * ids 
 * 
 * @param McastEvcId : Multicast EVC id
 * @param UcastEvcId : Unicast EVC id
 * @param igmp_idx   : IGMP instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_instance_find(L7_uint16 McastEvcId, L7_uint16 UcastEvcId, L7_uint *igmp_idx)
{
  L7_uint idx;

  /* Search for the provided Mcast and Ucast evcs */
  for (idx=0; idx<PTIN_SYSTEM_N_IGMP_INSTANCES; idx++)
  {
    if (!igmpInstances[idx].inUse)  continue;

    if (igmpInstances[idx].McastEvcId==McastEvcId &&
        igmpInstances[idx].UcastEvcId==UcastEvcId)
      break;
  }

  /* If not found empty instances, return error */
  if (idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
    return L7_FAILURE;

  /* Return instance index */
  if (igmp_idx!=L7_NULLPTR)  *igmp_idx = idx;

  return L7_SUCCESS;
}

/**
 * Gets the IGMP instance wich is using an EVC id
 * 
 * @param evcId    : EVC id
 * @param igmp_idx : IGMP instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_instance_find_fromSingleEvcId(L7_uint16 evcId, L7_uint *igmp_idx)
{
  L7_uint idx;

  /* Search for the provided Mcast and Ucast evcs */
  for (idx=0; idx<PTIN_SYSTEM_N_IGMP_INSTANCES; idx++)
  {
    if (!igmpInstances[idx].inUse)  continue;

    if (igmpInstances[idx].McastEvcId==evcId ||
        igmpInstances[idx].UcastEvcId==evcId)
      break;
  }

  /* If not found empty instances, return error */
  if (idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
    return L7_FAILURE;

  /* Return instance index */
  if (igmp_idx!=L7_NULLPTR)  *igmp_idx = idx;

  return L7_SUCCESS;
}

/**
 * Gets the IGMP instance wich is using a specific Multicast EVC
 * id 
 * 
 * @param McastEvcId : MC EVC id
 * @param igmp_idx   : IGMP instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_instance_find_fromMcastEvcId(L7_uint16 McastEvcId, L7_uint *igmp_idx)
{
  L7_uint idx;

  /* Search for the provided Mcast and Ucast evcs */
  for (idx=0; idx<PTIN_SYSTEM_N_IGMP_INSTANCES; idx++)
  {
    if (!igmpInstances[idx].inUse)  continue;

    if (igmpInstances[idx].McastEvcId==McastEvcId)
      break;
  }

  /* If not found empty instances, return error */
  if (idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
    return L7_FAILURE;

  /* Return instance index */
  if (igmp_idx!=L7_NULLPTR)  *igmp_idx = idx;

  return L7_SUCCESS;
}

/**
 * Checks if the provided MC and UC evc id's are free of 
 * conflict relatively to the existent igmp instances.
 * 
 * @param McastEvcId : Multicast EVC id
 * @param UcastEvcId : Unicast EVC id
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_BOOL ptin_igmp_instance_conflictFree(L7_uint16 McastEvcId, L7_uint16 UcastEvcId)
{
  L7_uint idx;

  /* Search for the provided Mcast and Ucast evcs */
  for (idx=0; idx<PTIN_SYSTEM_N_IGMP_INSTANCES; idx++)
  {
    if (!igmpInstances[idx].inUse)  continue;

    if (igmpInstances[idx].McastEvcId==McastEvcId ||
        igmpInstances[idx].UcastEvcId==UcastEvcId ||
        igmpInstances[idx].McastEvcId==UcastEvcId ||
        igmpInstances[idx].UcastEvcId==McastEvcId)
      break;
  }

  /* If not found empty instances, return error */
  if (idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
    return L7_TRUE;

  return L7_FALSE;
}

/**
 * Find client information in a particulat IGMP instance
 * 
 * @param igmp_idx    : IGMP instance index
 * @param client_ref  : client reference
 * @param client_info : client information pointer (output)
 * 
 * @return L7_RC_t : L7_SUCCESS - Client found 
 *                   L7_NOT_EXIST - Client does not exist
 *                   L7_FAILURE - Error
 */
static L7_RC_t ptin_igmp_client_find(L7_uint igmp_idx, ptin_client_id_t *client_ref, ptinIgmpClientInfoData_t **client_info)
{
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientsAvlTree_t *avl_tree;
  ptinIgmpClientInfoData_t *clientInfo;
  #if (MC_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port;
  #endif

  /* Validate arguments */
  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES || client_ref==L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate igmp instance */
  if (!igmpInstances[igmp_idx].inUse)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"IGMP instance %u is not in use",igmp_idx);
    return L7_FAILURE;
  }

  /* Get ptin_port value */
  #if (MC_CLIENT_INTERF_SUPPORTED)
  ptin_port = 0;
  if (client_ref->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    /* Convert to ptin_port format */
    if (ptin_intf_ptintf2port(&client_ref->ptin_intf,&ptin_port)!=L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot convert client_ref intf %u/%u to ptin_port format",client_ref->ptin_intf.intf_type,client_ref->ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
  #endif

  /* Key to search for */
  avl_tree = &igmpInstances[igmp_idx].igmpClients.avlTree;
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  #if (MC_CLIENT_INTERF_SUPPORTED)
  avl_key.ptin_port = ptin_port;
  #endif
  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
  avl_key.outerVlan = (client_ref->mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN) ? client_ref->outerVlan : 0;
  #endif
  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
  avl_key.innerVlan = (client_ref->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) ? client_ref->innerVlan : 0;
  #endif
  #if (MC_CLIENT_IPADDR_SUPPORTED)
  avl_key.ipv4_addr = (client_ref->mask & PTIN_CLIENT_MASK_FIELD_IPADDR   ) ? client_ref->ipv4_addr : 0;
  #endif
  #if (MC_CLIENT_MACADDR_SUPPORTED)
  if (client_ref->mask & PTIN_CLIENT_MASK_FIELD_MACADDR)
    memcpy(avl_key.macAddr,client_ref->macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  else
    memset(avl_key.macAddr,0x00,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  #endif

  /* Search for this client */
  clientInfo = avlSearchLVL7( &(avl_tree->igmpClientsAvlTree), (void *)&avl_key, AVL_EXACT);

  /* Check if this key already exists */
  if (clientInfo==L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Key {"
              #if (MC_CLIENT_INTERF_SUPPORTED)
                                "port=%u"
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                                ",svlan=%u"
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                                ",cvlan=%u"
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
                                ",ipAddr=%u.%u.%u.%u"
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
                                ",MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
              #endif
                                "} does not exist in igmp_idx=%u",
              #if (MC_CLIENT_INTERF_SUPPORTED)
              avl_key.ptin_port,
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              avl_key.outerVlan,
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              avl_key.innerVlan,
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
              #endif
              igmp_idx);
    }
    return L7_NOT_EXIST;
  }

  /* Return client info */
  if (client_info!=L7_NULLPTR)
  {
    *client_info = clientInfo;
  }

  return L7_SUCCESS;
}

/**
 * Increment IGMP statistics
 * 
 * @param intIfNum   : interface where the packet entered
 * @param vlan       : packet's interval vlan
 * @param client_idx : client index
 * @param field      : field to increment
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_increment_field(L7_uint32 intIfNum, L7_uint16 vlan, L7_uint32 client_idx, ptin_snoop_stat_enum_t field)
{
  L7_uint32 ptin_port;
  st_IgmpInstCfg_t *igmpInst;
  ptinIgmpClientInfoData_t *client;
  ptin_IGMP_Statistics_t *stat_port_g = L7_NULLPTR;
  ptin_IGMP_Statistics_t *stat_port   = L7_NULLPTR;
  ptin_IGMP_Statistics_t *stat_client = L7_NULLPTR;

  /* Validate field */
  if (field>=SNOOP_STAT_FIELD_ALL)
  {
    return L7_FAILURE;
  }

  /* Get IGMP instance */
  igmpInst = L7_NULLPTR;
  if (vlan>=PTIN_VLAN_MIN && vlan<=PTIN_VLAN_MAX)
  {
    if (ptin_igmp_inst_get_fromIntVlan(vlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
    {
      igmpInst = L7_NULLPTR;
    }
  }

  /* If interface is valid... */
  if (intIfNum>0 && intIfNum<L7_MAX_INTERFACE_COUNT)
  {
    /* Check if interface exists */
    if (ptin_intf_intIfNum2port(intIfNum,&ptin_port)==L7_SUCCESS && ptin_port<PTIN_SYSTEM_N_INTERF)
    {
      /* Global interface statistics at interface level */
      stat_port_g = &global_stats_intf[ptin_port];

      if (igmpInst!=L7_NULLPTR)
      {
        /* interface statistics at igmp instance and interface level */
        stat_port = &igmpInst->stats_intf[ptin_port];
      }
    }
  }

  /* If client index is valid... */
  if (igmpInst!=L7_NULLPTR && client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
  {
    client = igmpInst->igmpClients.clients_in_use[client_idx];
    if (client!=L7_NULLPTR)
    {
      /* Statistics at client level */
      stat_client = &client->stats_client;
    }
  }

  osapiSemaTake(ptin_igmp_stats_sem,-1);

  switch (field) {
  case SNOOP_STAT_FIELD_ACTIVE_GROUPS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->active_groups++;
    if (stat_port  !=L7_NULLPTR)  stat_port->active_groups++;
    if (stat_client!=L7_NULLPTR)  stat_client->active_groups++;
    break;

  case SNOOP_STAT_FIELD_ACTIVE_CLIENTS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->active_clients++;
    if (stat_port  !=L7_NULLPTR)  stat_port->active_clients++;
    if (stat_client!=L7_NULLPTR)  stat_client->active_clients++;
    break;

  case SNOOP_STAT_FIELD_IGMP_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_sent++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_sent++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_sent++;
    break;

  case SNOOP_STAT_FIELD_IGMP_TX_FAILED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_tx_failed++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_tx_failed++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_tx_failed++;
    break;

  case SNOOP_STAT_FIELD_IGMP_INTERCEPTED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_intercepted++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_intercepted++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_intercepted++;
    break;

  case SNOOP_STAT_FIELD_IGMP_DROPPED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_dropped++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_dropped++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_dropped++;
    break;

  case SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_received_valid++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_received_valid++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_received_valid++;
    break;

  case SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_received_invalid++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_received_invalid++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_received_invalid++;
    break;

  case SNOOP_STAT_FIELD_JOINS_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_sent++;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_sent++;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_sent++;
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_received_success++;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_received_success++;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_received_success++;
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_received_failed++;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_received_failed++;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_received_failed++;
    break;

  case SNOOP_STAT_FIELD_LEAVES_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->leaves_sent++;
    if (stat_port  !=L7_NULLPTR)  stat_port->leaves_sent++;
    if (stat_client!=L7_NULLPTR)  stat_client->leaves_sent++;
    break;

  case SNOOP_STAT_FIELD_LEAVES_RECEIVED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->leaves_received++;
    if (stat_port  !=L7_NULLPTR)  stat_port->leaves_received++;
    if (stat_client!=L7_NULLPTR)  stat_client->leaves_received++;
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_V3:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->membership_report_v3++;
    if (stat_port  !=L7_NULLPTR)  stat_port->membership_report_v3++;
    if (stat_client!=L7_NULLPTR)  stat_client->membership_report_v3++;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERIES_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->general_queries_sent++;
    if (stat_port  !=L7_NULLPTR)  stat_port->general_queries_sent++;
    if (stat_client!=L7_NULLPTR)  stat_client->general_queries_sent++;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->general_queries_received++;
    if (stat_port  !=L7_NULLPTR)  stat_port->general_queries_received++;
    if (stat_client!=L7_NULLPTR)  stat_client->general_queries_received++;
    break;

  case SNOOP_STAT_FIELD_SPECIFIC_QUERIES_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->specific_queries_sent++;
    if (stat_port  !=L7_NULLPTR)  stat_port->specific_queries_sent++;
    if (stat_client!=L7_NULLPTR)  stat_client->specific_queries_sent++;
    break;

  case SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->specific_queries_received++;
    if (stat_port  !=L7_NULLPTR)  stat_port->specific_queries_received++;
    if (stat_client!=L7_NULLPTR)  stat_client->specific_queries_received++;
    break;

  default:
    break;
  }

  osapiSemaGive(ptin_igmp_stats_sem);

  return L7_SUCCESS;
}

/**
 * Decrement IGMP statistics
 * 
 * @param intIfNum   : interface where the packet entered
 * @param vlan       : packet's interval vlan
 * @param client_idx : client index
 * @param field      : field to increment
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_decrement_field(L7_uint32 intIfNum, L7_uint16 vlan, L7_uint32 client_idx, ptin_snoop_stat_enum_t field)
{
  L7_uint32 ptin_port;
  st_IgmpInstCfg_t *igmpInst;
  ptinIgmpClientInfoData_t *client;
  ptin_IGMP_Statistics_t *stat_port_g = L7_NULLPTR;
  ptin_IGMP_Statistics_t *stat_port   = L7_NULLPTR;
  ptin_IGMP_Statistics_t *stat_client = L7_NULLPTR;

  /* Validate field */
  if (field>=SNOOP_STAT_FIELD_ALL)
  {
    return L7_FAILURE;
  }

  /* Get IGMP instance */
  igmpInst = L7_NULLPTR;
  if (vlan>=PTIN_VLAN_MIN && vlan<=PTIN_VLAN_MAX)
  {
    if (ptin_igmp_inst_get_fromIntVlan(vlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
    {
      igmpInst = L7_NULLPTR;
    }
  }

  /* If interface is valid... */
  if (intIfNum>0 && intIfNum<L7_MAX_INTERFACE_COUNT)
  {
    /* Check if interface exists */
    if (ptin_intf_intIfNum2port(intIfNum,&ptin_port)==L7_SUCCESS && ptin_port<PTIN_SYSTEM_N_INTERF)
    {
      /* Global interface statistics at interface level */
      stat_port_g = &global_stats_intf[ptin_port];

      if (igmpInst!=L7_NULLPTR)
      {
        /* interface statistics at igmp instance and interface level */
        stat_port = &igmpInst->stats_intf[ptin_port];
      }
    }
  }

  /* If client index is valid... */
  if (igmpInst!=L7_NULLPTR && client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
  {
    client = igmpInst->igmpClients.clients_in_use[client_idx];
    if (client!=L7_NULLPTR)
    {
      /* Statistics at client level */
      stat_client = &client->stats_client;
    }
  }

  osapiSemaTake(ptin_igmp_stats_sem,-1);

  switch (field) {
  case SNOOP_STAT_FIELD_ACTIVE_GROUPS:
    if (stat_port_g!=L7_NULLPTR)
    {
      if (stat_port_g->active_groups>0)   stat_port_g->active_groups--;
    }
    if (stat_port  !=L7_NULLPTR)
    {
      if (stat_port->active_groups>0)     stat_port->active_groups--;
    }
    if (stat_client!=L7_NULLPTR)
    {
      if (stat_client->active_groups>0)   stat_client->active_groups--;
    }
    break;

  case SNOOP_STAT_FIELD_ACTIVE_CLIENTS:
    if (stat_port_g!=L7_NULLPTR)
    {
      if (stat_port_g->active_clients>0)  stat_port_g->active_clients--;
    }
    if (stat_port  !=L7_NULLPTR)
    {
      if (stat_port->active_clients>0)    stat_port->active_clients--;
    }
    if (stat_client!=L7_NULLPTR)
    {
      if (stat_client->active_clients>0)  stat_client->active_clients--;
    }
    break;

  default:
    break;
  }

  osapiSemaGive(ptin_igmp_stats_sem);

  return L7_SUCCESS;
}



#ifndef L7_LEFT
#define L7_LEFT   0
#endif
#ifndef L7_RIGHT
#define L7_RIGHT  1
#endif

/**
 * Configures (enable/disable) a host interface
 * 
 * @param router_intf 
 * @param router_vlan 
 * @param admin L7_ENABLE/L7_DISABLE
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
//static L7_RC_t ptin_igmp_router_intf_config(L7_uint router_intf, L7_uint16 router_vlan, L7_uint admin)
//{
//L7_uint32 intIfNum;
//L7_uint16 router_int_vlan;
//L7_RC_t   rc;
//
//ptin_intf_port2intf(router_intf, &intIfNum);  /* already validated.. no error should occur */
//
//if (admin)
//  igmpRoutersIntf[router_intf]++;   /* one more instance is using this interface as router */
//else
//  igmpRoutersIntf[router_intf]--;
//
//if ((igmpRoutersIntf[router_intf] == 1 && admin) ||
//    (igmpRoutersIntf[router_intf] == 0 && !admin))
//{
//  rc = usmDbSnoopIntfMrouterSet(1, intIfNum, admin, L7_AF_INET);
//  if (rc != L7_SUCCESS)
//  {
//    LOG_CRITICAL(LOG_CTX_PTIN_IGMP, "Error %s ptin intf# %u as router interface",
//                 admin?"enable":"disable", router_intf);
//    return L7_FAILURE;
//  }
//}
//
//rc = usmDbsnoopIntfApiVlanStaticMcastRtrSet(1, intIfNum, router_mc_int_vlan, admin, L7_AF_INET);
//if (rc != L7_SUCCESS)
//{
//  LOG_CRITICAL(LOG_CTX_PTIN_IGMP, "Error %s VLAN %u at ptin intf# %u as router interface",
//               admin?"enable":"disable", router_vlan, router_intf);
//  return 0;
//}
//
//  return L7_FAILURE;
//}


/**
 * Configures (enable/disable) a client interface (querier)
 * 
 * @param client_intf 
 * @param admin L7_ENABLE/L7_DISABLE
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
//static L7_RC_t ptin_igmp_clients_intf_config(L7_uint client_intf, L7_uint admin)
//{
//
//ptin_intf_port2intf(client_intf, &intIfNum);  /* already validated.. no error should occur */
//
//if (admin)
//  igmpClientsIntf[client_intf]++;
//else
//  igmpClientsIntf[client_intf]--;
//
///* If this interface is already configured, ignore it */
//if (igmpClientsIntf[client_intf] > 1)
//  continue;
//
///* Configure interface */
//if ((igmpClientsIntf[client_intf] == 1 && admin) ||
//    (igmpClientsIntf[client_intf] == 0 && !admin))
//{
//  rc = usmDbSnoopIntfModeSet(1, intIfNum, admin, L7_AF_INET);
//  if (rc != L7_SUCCESS)
//  {
//    LOG_CRITICAL(LOG_CTX_PTIN_IGMP, "Error %s ptin intf# %u as querier interface",
//                 admin?"enable":"disable", router_intf);
//    return L7_FAILURE;
//  }
//}
//
///* Configure parameters on this interface */
//rc = usmDbSnoopIntfGroupMembershipIntervalSet(1, intIfNum, igmpProxyCfg.querier.group_membership_interval, L7_AF_INET);
//if (rc != L7_SUCCESS)
//{
//  LOG_CRITICAL(LOG_CTX_PTIN_IGMP, "Error setting Group Membership Interval %u on ptin intf# %u",
//               igmpProxyCfg.querier.group_membership_interval, i);
//  return L7_FAILURE;
//}
//
//rc = usmDbSnoopIntfResponseTimeSet(1, intIfNum, igmpProxyCfg.querier.query_response_interval/10, L7_AF_INET);
//if (rc != L7_SUCCESS)
//{
//  LOG_CRITICAL(LOG_CTX_PTIN_IGMP, "Error setting Response Time Interval %u on ptin intf# %u",
//               igmpProxyCfg.querier.query_response_interval, i);
//  return L7_FAILURE;
//}
//
//rc = usmDbSnoopIntfMcastRtrExpiryTimeSet(1, intIfNum, 0, L7_AF_INET);
//if (rc != L7_SUCCESS)
//{
//  LOG_CRITICAL(LOG_CTX_PTIN_IGMP, "Error setting MC Router Expiry Time %u on ptin intf %u", 0, i);
//  return L7_FAILURE;
//}
//
//rc = usmDbSnoopIntfFastLeaveAdminModeSet(1, intIfNum, igmpProxyCfg.fast_leave, L7_AF_INET);
//if (rc != L7_SUCCESS)
//{
//  LOG_CRITICAL(LOG_CTX_PTIN_IGMP, "Error setting Fast-Leave mode %s on ptin intf# %u",
//               igmpProxyCfg.fast_leave != 0 ? "ON":"OFF", i);
//  return L7_FAILURE;
//}
//
//  return L7_FAILURE;
//}

#if 0
static void ptin_igmp_vlan_intfs_get(L7_uint16 vlan, L7_uint8 intfs[], L7_BOOL include_router_intf)
{
  L7_uint i, j;

  /* Get the list of interfaces that use */
  for (i=0; i<PTIN_SYSTEM_N_IGMP_INSTANCES; i++)
  {
    if (igmpInstances[i].router_vlan == 0)
      continue;

    if (igmpInstances[i].router_vlan  == vlan ||
        igmpInstances[i].clients_vlan == vlan ||
        igmpInstances[i].uc_vlan      == vlan)
    {
      for (j=0; j<igmpInstances[i].n_intf; j++)
      {

      }
    }
  }
}
#endif

#if 0
/**
 * Gets a list of IGMP clients and routers interfaces
 * 
 * @param clients_intf_bmp Bitmap with clients interfaces
 * @param router_intf_bmp Bitmap with routers interfaces
 */
static void ptin_igmp_intf_list_get(L7_uint64 *clients_intf_bmp, L7_uint64 *routers_intf_bmp)
{
  L7_uint i, j;
  L7_uint bit;

  *clients_intf_bmp = 0;
  *routers_intf_bmp = 0;

  for (i=0; i<PTIN_SYSTEM_N_IGMP_INSTANCES; i++)
  {
    if (igmpInstances[i].router_vlan == 0)
      continue;

    for (j=0; j<igmpInstances[i].n_intf; j++)
    {
      bit = igmpInstances[i].intf[j].intf_id +
            (igmpInstances[i].intf[j].intf_type == PTIN_EVC_INTF_LOGICAL ? PTIN_SYSTEM_N_PORTS-1 : 0);

      if (igmpInstances[i].router_idx == j)
        *routers_intf_bmp |= 1 << bit;
      else
        *clients_intf_bmp |= 1 << bit;
    }
  }

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "IGMP clients intf bmp: 0x%08llX\trouters intf bmp: 0x%08llX",
            *clients_intf_bmp, *routers_intf_bmp);
}
#endif

/* DEBUG Functions ************************************************************/
/**
 * Dumps EVC detailed info 
 * If evc_idx is invalid, all EVCs are dumped 
 * 
 * @param evc_idx 
 */
void ptin_igmp_dump(void)
{
  L7_uint i, i_client;
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientInfoData_t *avl_info;


  for (i = 0; i < PTIN_SYSTEM_N_IGMP_INSTANCES; i++)
  {
    if (!igmpInstances[i].inUse) {
      printf("*** Igmp instance %02u not in use\r\n", i);
      continue;
    }

    printf("IGMP instance %02u\n", i);
    printf("   MC evc_idx = %u\r\n",igmpInstances[i].McastEvcId);
    printf("   UC evc_idx = %u\r\n",igmpInstances[i].UcastEvcId);

    i_client = 0;

    /* Run all cells in AVL tree */
    memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
    while ( ( avl_info = (ptinIgmpClientInfoData_t *)
                          avlSearchLVL7(&igmpInstances[i].igmpClients.avlTree.igmpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
            ) != L7_NULLPTR )
    {
      /* Prepare next key */
      memcpy(&avl_key, &avl_info->igmpClientDataKey, sizeof(ptinIgmpClientDataKey_t));

      printf("      Client#%u: "
             #if (MC_CLIENT_INTERF_SUPPORTED)
             "ptin_port=%-2u "
             #endif
             #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
             "svlan=%-4u "
             #endif
             #if (MC_CLIENT_INNERVLAN_SUPPORTED)
             "cvlan=%-4u "
             #endif
             #if (MC_CLIENT_IPADDR_SUPPORTED)
             "IP=%03u.%03u.%03u.%03u "
             #endif
             #if (MC_CLIENT_MACADDR_SUPPORTED)
             "MAC=%02x:%02x:%02x:%02x:%02x:%02x "
             #endif
             ": index=%-3u [%s] #channels=%u\r\n",
             i_client,
             #if (MC_CLIENT_INTERF_SUPPORTED)
             avl_info->igmpClientDataKey.ptin_port,
             #endif
             #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
             avl_info->igmpClientDataKey.outerVlan,
             #endif
             #if (MC_CLIENT_INNERVLAN_SUPPORTED)
             avl_info->igmpClientDataKey.innerVlan,
             #endif
             #if (MC_CLIENT_IPADDR_SUPPORTED)
             (avl_info->igmpClientDataKey.ipv4_addr>>24) & 0xff,
              (avl_info->igmpClientDataKey.ipv4_addr>>16) & 0xff,
               (avl_info->igmpClientDataKey.ipv4_addr>>8) & 0xff,
                avl_info->igmpClientDataKey.ipv4_addr & 0xff,
             #endif
             #if (MC_CLIENT_MACADDR_SUPPORTED)
             avl_info->igmpClientDataKey.macAddr[0],
              avl_info->igmpClientDataKey.macAddr[1],
               avl_info->igmpClientDataKey.macAddr[2],
                avl_info->igmpClientDataKey.macAddr[3],
                 avl_info->igmpClientDataKey.macAddr[4],
                  avl_info->igmpClientDataKey.macAddr[5],
             #endif
             avl_info->client_index,
             ((avl_info->isDynamic) ? "dynamic" : "static "),
             avl_info->stats_client.active_groups);

      i_client++;
    }
  }
}

