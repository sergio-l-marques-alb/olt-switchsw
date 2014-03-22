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
#include "ptin_fieldproc.h"
#include "ptin_mgmd_eventqueue.h"
#include "ptin_mgmd_ctrl.h"
#include "ptin_cnfgr.h"

#define IGMP_INVALID_ENTRY    0xFF

/******************************* 
 * FEATURES
 *******************************/

/* Comment the follwing line, if you don't want to use client timers */
#define CLIENT_TIMERS_SUPPORTED

#define PTIN_MGMD_MC_SERVICE_ID_IN_USE 

/******************************* 
 * Debug procedures
 *******************************/

L7_BOOL ptin_debug_igmp_snooping = 0;

L7_BOOL ptin_debug_igmp_packet_trace = 0;

void ptin_debug_igmp_enable(L7_BOOL enable)
{
  ptin_debug_igmp_snooping = enable;
}

void ptin_debug_igmp_packet_trace_enable(L7_BOOL enable)
{
  ptin_debug_igmp_packet_trace = enable;
}

/******************************* 
 * ATTRIBUTES
 *******************************/

/* Parameters to identify the client */
#define MC_CLIENT_INTERF_SUPPORTED    1
#define MC_CLIENT_OUTERVLAN_SUPPORTED 1
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

/* If you want to repeat the client index range for each interface, activate this define */
/* Otherwise will be used a unique range for all interfaces */
#define PTIN_IGMP_CLIENTS_ISOLATED_PER_INTF 1

#if PTIN_IGMP_CLIENTS_ISOLATED_PER_INTF
 #define PTIN_IGMP_CLIENTIDX_MAX      (PTIN_SYSTEM_IGMP_MAXCLIENTS_PER_INTF)
 #define PTIN_IGMP_INTFPORT_MAX       (PTIN_SYSTEM_N_INTERF)
 #define PTIN_IGMP_CLIENT_PORT(port)  ((port < PTIN_SYSTEM_N_INTERF) ? (port) : 0)
#else
 #define PTIN_IGMP_CLIENTIDX_MAX      (PTIN_SYSTEM_IGMP_MAXCLIENTS)
 #define PTIN_IGMP_INTFPORT_MAX       (1)
 #define PTIN_IGMP_CLIENT_PORT(port)  (0)
#endif


/******************************* 
 * QUEUES
 *******************************/

struct ptinIgmpClientInfoData_s;

/* Client indexes pool */
typedef struct ptinIgmpClientIdx_s
{
  struct ptinIgmpClientIdx_s *next;
  struct ptinIgmpClientIdx_s *prev;

  L7_BOOL   in_use;
  L7_uint16 client_idx;
} ptinIgmpClientIdx_t;

/* Client info pool */
typedef struct ptinIgmpClientDevice_s
{
  struct ptinIgmpClientDevice_s *next;
  struct ptinIgmpClientDevice_s *prev;

  struct ptinIgmpClientInfoData_s *client;
} ptinIgmpClientDevice_t;

/* Client index pool */
struct ptinIgmpClientIdx_s clientIdx_pool[PTIN_IGMP_INTFPORT_MAX][PTIN_IGMP_CLIENTIDX_MAX];
/* Queue of free client indexes */
static dl_queue_t queue_free_clientIdx[PTIN_IGMP_INTFPORT_MAX];

/*********************************************************** 
 * Typedefs
 ***********************************************************/

/* Client data */
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

struct ptinIgmpClientInfoData_s;
struct ptinIgmpClientGroupInfoData_s;

/* Client list snapshot */
typedef struct ptinIgmpClientGroupsSnapshotInfoData_s
{
  ptin_client_id_t key;
  L7_BOOL          in_use;
  void             *next;
} ptinIgmpClientGroupsSnapshotInfoData_t;

typedef struct {
    avlTree_t                              avlTree;
    avlTreeTables_t                        *treeHeap;
    ptinIgmpClientGroupsSnapshotInfoData_t *dataHeap;
} ptinIgmpClientGroupsSnapshotAvlTree_t;

/* Client Groups */
typedef struct ptinIgmpClientGroupInfoData_s
{
  ptinIgmpClientDataKey_t   igmpClientDataKey;
  L7_uint16                 uni_ovid;               /* Ext. OVID to be used for packet transmission */
  L7_uint16                 uni_ivid;               /* Ext. IVID to be used for packet transmission */
  L7_uint8                  ptin_port;              /* Port */
  L7_uint32                 client_bmp_list[PTIN_IGMP_CLIENTIDX_MAX/(sizeof(L7_uint32)*8)+1];  /* Clients (children) bitmap (only for one interface) */
  dl_queue_t                queue_clientDevices;
  ptin_IGMP_Statistics_t    stats_client;
  void *next;
} ptinIgmpClientGroupInfoData_t;

typedef struct {
    avlTree_t                     igmpClientsAvlTree;
    avlTreeTables_t               *igmpClientsTreeHeap;
    ptinIgmpClientGroupInfoData_t *igmpClientsDataHeap;
} ptinIgmpClientGroupAvlTree_t;

typedef struct
{
  L7_uint16 number_of_clients;                                  /* Total number of clients */
  ptinIgmpClientGroupAvlTree_t avlTree;
} ptinIgmpClientGroups_t;


/* Child clients (devices) */
typedef struct ptinIgmpClientInfoData_s
{
  ptinIgmpClientDataKey_t   igmpClientDataKey;
  L7_uint8                  ptin_port;              /* Port */
  L7_uint16                 client_index;
  L7_uint16                 uni_ovid;               /* Ext. OVID to be used for packet transmission */
  L7_uint16                 uni_ivid;               /* Ext. IVID to be used for packet transmission */
  L7_BOOL                   isDynamic;
  L7_uint16                 number_of_mc_services;  /* A client may be part of several MC services: this will benused for AVL tree management */
  struct ptinIgmpClientGroupInfoData_s *pClientGroup;    /* Pointer to ClientGroup (null for client groups avl tree) */
  void *next;
} ptinIgmpClientInfoData_t;

typedef struct {
    avlTree_t                 igmpClientsAvlTree;
    avlTreeTables_t           *igmpClientsTreeHeap;
    ptinIgmpClientInfoData_t  *igmpClientsDataHeap;
} ptinIgmpClientsAvlTree_t;

typedef struct
{
  L7_uint16 number_of_clients;                                  /* Total number of clients */
  L7_uint16 number_of_clients_per_intf[PTIN_SYSTEM_N_INTERF];   /* Number of clients per interface for one IGMP instance */

  ptinIgmpClientDevice_t    client_devices[PTIN_IGMP_INTFPORT_MAX][PTIN_IGMP_CLIENTIDX_MAX];
  dl_queue_t                queue_free_clientDevices[PTIN_IGMP_INTFPORT_MAX]; /* Queue with free (device) clients */

  ptinIgmpClientsAvlTree_t  avlTree;
  L7_APP_TMR_CTRL_BLK_t     timerCB;       /* Entry App Timer Control Block */

  L7_sll_t                  ll_timerList;  /* Linked list of timer data nodes */
  L7_uint32                 ctrlBlkBufferPoolId;
  L7_uint32                 appTimerBufferPoolId;
  handle_list_t            *appTimer_handle_list;
  void                     *appTimer_handleListMemHndl;
} ptinIgmpClients_unified_t;

/******************************* 
 * GLOBAL STRUCTS
 *******************************/

/* Snapshot list of client groups, required for the GROUP_CLIENTS API */
ptinIgmpClientGroupsSnapshotAvlTree_t igmpSnapshotClientGroups;

/* Client Groups (to be added manually) */
ptinIgmpClientGroups_t igmpClientGroups;

/* Unified list with all clients (to be added dynamically) */
ptinIgmpClients_unified_t igmpClients_unified;


/******************************* 
 * MULTI MULTICAST FEATURE
 *******************************/

/** Service association AVL Tree */
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED

/* Optional */
#define IGMPASSOC_CHANNEL_UC_EVC_ISOLATION  0
#define IGMPASSOC_CHANNEL_SOURCE_SUPPORTED  0

/* IGMP associations */
typedef struct
{
  L7_inet_addr_t  channel_group;

  #if (IGMPASSOC_CHANNEL_SOURCE_SUPPORTED)
  L7_inet_addr_t  channel_source;
  #endif

  #if IGMPASSOC_CHANNEL_UC_EVC_ISOLATION
  L7_uint32 evc_uc;
  #endif
} ptinIgmpPairDataKey_t;

typedef struct
{
  ptinIgmpPairDataKey_t     igmpPairDataKey;
  L7_uint32                 evc_mc;
  L7_uint32                 evc_uc;
  L7_uint16                 igmp_idx;
  L7_BOOL                   is_static;
  void *next;
} ptinIgmpPairInfoData_t;

typedef struct {
    L7_uint16 number_of_entries;

    avlTree_t               igmpPairAvlTree;
    avlTreeTables_t         *igmpPairTreeHeap;
    ptinIgmpPairInfoData_t  *igmpPairDataHeap;
} ptinIgmpPairAvlTree_t;

/* List of all IGMP associations */
ptinIgmpPairAvlTree_t igmpPairDB;

/* Prototypes */
static L7_RC_t igmp_assoc_pair_get( L7_uint32 evc_uc,
                                    L7_inet_addr_t *channel_group, L7_inet_addr_t *channel_source,
                                    L7_uint32 *evc_mc );
static L7_RC_t igmp_assoc_channelIP_prepare( L7_inet_addr_t *channel_in, L7_uint16 channel_mask,
                                             L7_inet_addr_t *channel_out, L7_uint32 *number_of_channels);
static L7_RC_t igmp_assoc_avlTree_insert( ptinIgmpPairInfoData_t *node );
static L7_RC_t igmp_assoc_avlTree_remove( ptinIgmpPairDataKey_t *avl_key );
static L7_RC_t igmp_assoc_avlTree_clear ( L7_uint32 evc_uc, L7_uint32 evc_mc );
static L7_RC_t igmp_assoc_avlTree_purge ( void );
#endif

/******************************* 
 * IGMP INSTANCES STRUCTS
 *******************************/

/* IGMP Instance config struct
 * IMPORTANT:
 *   1. only ONE root is allowed per instance
 *   2. only ONE vlan is allowed for all client interfaces
 *   3. the unicast VLAN cannot be used on multiple IGMP instances */ 
typedef struct {
  L7_BOOL   inUse;
  L7_uint32 McastEvcId;
  L7_uint32 UcastEvcId;
  L7_uint16 nni_ovid;         /* NNI outer vlan used for EVC aggregation in one instance */
  L7_uint16 n_evcs;
  ptin_IGMP_Statistics_t stats_intf[PTIN_SYSTEM_N_INTERF];  /* IGMP statistics at interface level */
} st_IgmpInstCfg_t;

/* IGMP instances array
 * NOTE: each instance is uniquely identified by the router internal VLAN.
 * Notice that the external router VLAN (or other VLANs) can be used on
 * other instances on different interfaces. The uniqueness is guaranteed
 * by the interval VLANs. */ 
st_IgmpInstCfg_t  igmpInstances[PTIN_SYSTEM_N_IGMP_INSTANCES];

/* MGMD Query Instance Array
 * This structure is used to save the Query Instances currently configured on the MGMD
 *
 *If modified please update also on snooping_mgmd_api.c! */
#if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
typedef struct {
  L7_BOOL   inUse;  
  L7_uint32 UcastEvcId;
} mgmdQueryInstances_t;
mgmdQueryInstances_t  mgmdQueryInstances[PTIN_SYSTEM_N_EVCS];
L7_uint16             mgmdNumberOfQueryInstances=0;
#endif

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

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
/* Global number of QUATTRO P2P flows */
static L7_uint32 igmp_quattro_p2p_evcs = 0;
#endif


/* Local functions prototypes */
static L7_RC_t ptin_igmp_clientGroup_find(ptin_client_id_t *client_ref, ptinIgmpClientGroupInfoData_t **client_info);

static L7_RC_t ptin_igmp_client_find(ptin_client_id_t *client_ref, ptinIgmpClientInfoData_t **client_info);
static L7_RC_t ptin_igmp_new_client(ptin_client_id_t *client,
                                    L7_uint16 uni_ovid, L7_uint16 uni_ivid,
                                    L7_BOOL isDynamic, L7_uint *client_idx_ret);
#if 0
static L7_RC_t ptin_igmp_rm_client(L7_uint igmp_idx, ptin_client_id_t *client, L7_BOOL remove_static);
#endif
static L7_RC_t ptin_igmp_rm_all_clients(L7_BOOL isDynamic, L7_BOOL only_wo_channels);
static L7_RC_t ptin_igmp_rm_clientIdx(L7_uint ptin_port, L7_uint client_idx, L7_BOOL remove_static, L7_BOOL force_remove);

//static L7_RC_t ptin_igmp_global_configuration(void);
static L7_RC_t ptin_igmp_trap_configure(L7_uint igmp_idx, L7_BOOL enable);
static L7_RC_t ptin_igmp_evc_trap_set(L7_uint32 evc_idx_mc, L7_uint32 evc_idx_uc, L7_BOOL enable);

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
static L7_RC_t ptin_igmp_evc_trap_configure(L7_uint32 evc_idx, L7_BOOL enable, ptin_dir_t direction);
#endif
static L7_RC_t ptin_igmp_instance_find_agg(L7_uint16 nni_ovlan, L7_uint *igmp_idx);

static L7_RC_t ptin_igmp_querier_configure(L7_uint igmp_idx, L7_BOOL enable);
static L7_RC_t ptin_igmp_evc_querier_configure(L7_uint32 evc_idx, L7_BOOL enable);
L7_RC_t ptin_igmp_mgmd_whitelist_add(L7_uint16 serviceId, L7_uint32 groupAddr, L7_uint8 groupMaskLen, L7_uint32 sourceAddr, L7_uint8 sourceMaskLen);
L7_RC_t ptin_igmp_mgmd_whitelist_remove(L7_uint16 serviceId, L7_uint32 groupAddr, L7_uint8 groupMaskLen, L7_uint32 sourceAddr, L7_uint8 sourceMaskLen);
/* Not used */
#if 0
static L7_RC_t ptin_igmp_instance_deleteAll_clients(L7_uint igmp_idx);
#endif
static L7_RC_t ptin_igmp_inst_get_fromIntVlan(L7_uint16 intVlan, st_IgmpInstCfg_t **igmpInst, L7_uint *igmpInst_idx);
static L7_RC_t ptin_igmp_instance_find_free(L7_uint *idx);
static L7_RC_t ptin_igmp_instance_find(L7_uint32 McastEvcId, L7_uint32 UcastEvcId, L7_uint *igmp_idx);
static L7_RC_t ptin_igmp_instance_find_fromSingleEvcId(L7_uint32 evc_idx, L7_uint *igmp_idx);
static L7_RC_t ptin_igmp_instance_find_fromMcastEvcId(L7_uint32 McastEvcId, L7_uint *igmp_idx);
static L7_BOOL ptin_igmp_instance_conflictFree(L7_uint32 McastEvcId, L7_uint32 UcastEvcId);

static L7_RC_t ptin_igmp_instance_delete(L7_uint16 igmp_idx);

static L7_RC_t ptin_igmp_clientId_convert(L7_uint32 evc_idx, ptin_client_id_t *client);
static L7_RC_t ptin_igmp_clientId_restore(ptin_client_id_t *client);


/******************************* 
 * TIMERS DATA
 *******************************/

#ifdef CLIENT_TIMERS_SUPPORTED

/* Semaphore for timers access */
void *ptin_igmp_timers_sem = L7_NULLPTR;

/* Timers variables */
L7_uint32 clientsMngmt_TaskId = L7_ERROR;
void     *clientsMngmt_queue  = L7_NULLPTR;

typedef struct ptinIgmpTimerParams_s
{
  //L7_uint32          igmp_idx;
  L7_uint32          dummy;
} ptinIgmpTimerParams_t;
#define PTIN_IGMP_TIMER_MSG_SIZE  sizeof(ptinIgmpTimerParams_t)

typedef struct igmpTimerData_s
{
  L7_sll_member_t   *next;
  L7_uint32         ptin_port;
  L7_uint32         client_idx;

  L7_uchar8         timerType;
  L7_APP_TMR_HNDL_t timer;
  L7_uint32         timerHandle;
} igmpTimerData_t;

L7_RC_t ptin_igmp_timersMng_init(void);
L7_RC_t ptin_igmp_timersMng_deinit(void);
L7_RC_t ptin_igmp_timer_start (L7_uint32 ptin_port, L7_uint32 client_idx);
L7_RC_t ptin_igmp_timer_update(L7_uint32 ptin_port, L7_uint32 client_idx);
L7_RC_t ptin_igmp_timer_stop  (L7_uint32 ptin_port, L7_uint32 client_idx);

void igmp_timersMng_task(void);
void igmp_timerExpiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* ptrData);
void igmp_timer_expiry(void *param);
L7_RC_t igmp_timer_dataDestroy (L7_sll_member_t *ll_member);
L7_int32 igmp_timer_dataCmp(void *p, void *q, L7_uint32 key);
#endif


/******************************* 
 * SEMAPHORES
 *******************************/

/* Semaphores */
void *igmp_sem = NULL;

/* Semaphore to access IGMP stats */
void *ptin_igmp_stats_sem = L7_NULLPTR;
void *ptin_igmp_clients_sem = L7_NULLPTR;
void *ptin_igmp_clients_snapshot_sem = L7_NULLPTR;

/*********************************************************** 
 * MACRO TOOLS
 ***********************************************************/
#define UINT32_BITSIZE  (sizeof(L7_uint32)*8)
#define IS_BITMAP_BIT_SET(array, index, size) ( ( array[(index)/((size)*8)] >> ((index)%((size)*8)) ) & 1 )
#define BITMAP_BIT_SET(array, index, size)    array[(index)/((size)*8)] |=  ((L7_uint32) 1 << ((index)%((size)*8)))
#define BITMAP_BIT_CLR(array, index, size)    array[(index)/((size)*8)] &= ~((L7_uint32) 1 << ((index)%((size)*8)))


/*********************************************************** 
 * QUEUES MANAGEMENT FUNCTIONS
 ***********************************************************/

/* Check if a client index is present in a ONU */
static L7_uint8 igmp_clientDevice_get_devices_number(struct ptinIgmpClientGroupInfoData_s *clientGroup);
/* Find a particular client in the client devices queue */
static struct ptinIgmpClientDevice_s *igmp_clientDevice_find(struct ptinIgmpClientGroupInfoData_s *clientGroup, struct ptinIgmpClientInfoData_s *clientInfo);
/* Get the next client withing client devices queue */
static struct ptinIgmpClientDevice_s *igmp_clientDevice_next(struct ptinIgmpClientGroupInfoData_s *clientGroup, struct ptinIgmpClientDevice_s *pelem);
/* Add a client within the client devices queue */
static struct ptinIgmpClientDevice_s *igmp_clientDevice_add(struct ptinIgmpClientGroupInfoData_s *clientGroup, struct ptinIgmpClientInfoData_s *clientInfo);
/* Remove a client from the client devices queue */
static struct ptinIgmpClientDevice_s *igmp_clientDevice_remove(struct ptinIgmpClientGroupInfoData_s *clientGroup, struct ptinIgmpClientInfoData_s *clientInfo);

/* Get new client index */
static L7_uint16 igmp_clientIndex_get_new(L7_uint ptin_port);
/* Get new client index */
static void igmp_clientIndex_free(L7_uint ptin_port, L7_uint16 client_idx);
#if 0
 /* Check if a client is being used */
static L7_BOOL igmp_clientIndex_is_marked(L7_uint client_idx);
#endif
/* Mark a client device as being used */
static void igmp_clientIndex_mark(L7_uint ptin_port, L7_uint client_idx, ptinIgmpClientInfoData_t *infoData);
/* Unmark a client device as being free */
static void igmp_clientIndex_unmark(L7_uint ptin_port, L7_uint client_idx);



/*********************************************************** 
 * FUNCTIONS 
 ***********************************************************/


/**
 * Initializes IGMP module
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_proxy_init(void)
{
  L7_uint i;
  L7_uint port;

  /* Reset instances array */
  memset(&igmpProxyCfg, 0x00, sizeof(igmpProxyCfg));
  memset(igmpInstances, 0x00, sizeof(igmpInstances));

  /* Initialize interfaces lists */
  memset(igmpClientsIntf, 0x00, sizeof(igmpClientsIntf));
  memset(igmpRoutersIntf, 0x00, sizeof(igmpRoutersIntf));

  /* Initialize lookup tables */
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

  /* CLIENT GROUPS INITIALIZATION */
  /* GroupClients snapshot */
  memset(&igmpSnapshotClientGroups, 0x00, sizeof(igmpSnapshotClientGroups));

  igmpSnapshotClientGroups.treeHeap = (avlTreeTables_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_IGMP_MAXONUS * sizeof(avlTreeTables_t)); 
  igmpSnapshotClientGroups.dataHeap = (ptinIgmpClientGroupsSnapshotInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_IGMP_MAXONUS * sizeof(ptinIgmpClientGroupsSnapshotInfoData_t)); 

  if ((igmpSnapshotClientGroups.treeHeap == L7_NULLPTR) ||
      (igmpSnapshotClientGroups.dataHeap == L7_NULLPTR))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error allocating data for IGMP Snapshot AVL Trees\n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset (&igmpSnapshotClientGroups.avlTree, 0x00, sizeof(avlTree_t));
  memset ( igmpSnapshotClientGroups.treeHeap, 0x00, sizeof(avlTreeTables_t)*PTIN_SYSTEM_IGMP_MAXONUS);
  memset ( igmpSnapshotClientGroups.dataHeap, 0x00, sizeof(ptinIgmpClientGroupsSnapshotInfoData_t)*PTIN_SYSTEM_IGMP_MAXONUS);

  // AVL Tree creations - snoopIpAvlTree
  avlCreateAvlTree(&(igmpSnapshotClientGroups.avlTree),
                   igmpSnapshotClientGroups.treeHeap,
                   igmpSnapshotClientGroups.dataHeap,
                   PTIN_SYSTEM_IGMP_MAXONUS, 
                   sizeof(ptinIgmpClientGroupsSnapshotInfoData_t),
                   0x10,
                   sizeof(ptinIgmpClientDataKey_t));

  if(L7_SUCCESS != ptin_igmp_clientGroupSnapshot_clean())
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to clear clientGroupSnapshot avlTree");
    return L7_FAILURE;
  }

  /* Client group */
  memset(&igmpClientGroups, 0x00, sizeof(igmpClientGroups));

  igmpClientGroups.avlTree.igmpClientsTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_IGMP_MAXONUS * sizeof(avlTreeTables_t)); 
  igmpClientGroups.avlTree.igmpClientsDataHeap = (ptinIgmpClientGroupInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_IGMP_MAXONUS * sizeof(ptinIgmpClientGroupInfoData_t)); 

  if ((igmpClientGroups.avlTree.igmpClientsTreeHeap == L7_NULLPTR) ||
      (igmpClientGroups.avlTree.igmpClientsDataHeap == L7_NULLPTR))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error allocating data for IGMP AVL Trees\n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset (&igmpClientGroups.avlTree.igmpClientsAvlTree, 0x00, sizeof(avlTree_t));
  memset ( igmpClientGroups.avlTree.igmpClientsTreeHeap, 0x00, sizeof(avlTreeTables_t)*PTIN_SYSTEM_IGMP_MAXONUS);
  memset ( igmpClientGroups.avlTree.igmpClientsDataHeap, 0x00, sizeof(ptinIgmpClientGroupInfoData_t)*PTIN_SYSTEM_IGMP_MAXONUS);

  // AVL Tree creations - snoopIpAvlTree
  avlCreateAvlTree(&(igmpClientGroups.avlTree.igmpClientsAvlTree),
                   igmpClientGroups.avlTree.igmpClientsTreeHeap,
                   igmpClientGroups.avlTree.igmpClientsDataHeap,
                   PTIN_SYSTEM_IGMP_MAXONUS, 
                   sizeof(ptinIgmpClientGroupInfoData_t),
                   0x10,
                   sizeof(ptinIgmpClientDataKey_t));

  /* No of client groups */
  igmpClientGroups.number_of_clients = 0;

  /* INDIVIUAL CLIENTS INITIALIZATION */
  /* Reset unified list of clients */
  memset(&igmpClients_unified, 0x00, sizeof(igmpClients_unified));

  igmpClients_unified.avlTree.igmpClientsTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_IGMP_MAXCLIENTS * sizeof(avlTreeTables_t)); 
  igmpClients_unified.avlTree.igmpClientsDataHeap = (ptinIgmpClientInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_IGMP_MAXCLIENTS * sizeof(ptinIgmpClientInfoData_t)); 

  if ((igmpClients_unified.avlTree.igmpClientsTreeHeap == L7_NULLPTR) ||
      (igmpClients_unified.avlTree.igmpClientsDataHeap == L7_NULLPTR))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error allocating data for IGMP AVL Trees\n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset (&igmpClients_unified.avlTree.igmpClientsAvlTree, 0x00, sizeof(avlTree_t));
  memset ( igmpClients_unified.avlTree.igmpClientsTreeHeap, 0x00, sizeof(avlTreeTables_t)*PTIN_SYSTEM_IGMP_MAXCLIENTS);
  memset ( igmpClients_unified.avlTree.igmpClientsDataHeap, 0x00, sizeof(ptinIgmpClientInfoData_t)*PTIN_SYSTEM_IGMP_MAXCLIENTS);

  // AVL Tree creations - snoopIpAvlTree
  avlCreateAvlTree(&(igmpClients_unified.avlTree.igmpClientsAvlTree),
                   igmpClients_unified.avlTree.igmpClientsTreeHeap,
                   igmpClients_unified.avlTree.igmpClientsDataHeap,
                   PTIN_SYSTEM_IGMP_MAXCLIENTS, 
                   sizeof(ptinIgmpClientInfoData_t),
                   0x10,
                   sizeof(ptinIgmpClientDataKey_t));

  /* No clients */
  igmpClients_unified.number_of_clients = 0;
  memset(igmpClients_unified.number_of_clients_per_intf,0x00,sizeof(igmpClients_unified.number_of_clients_per_intf));
  /* Initialize clients list */
  memset(&igmpClients_unified.client_devices, 0x00, sizeof(igmpClients_unified.client_devices));

  /* Client indexes pool */
  memset(clientIdx_pool, 0x00, sizeof(clientIdx_pool));

  for (port=0; port<PTIN_IGMP_INTFPORT_MAX; port++)
  {
    dl_queue_init(&queue_free_clientIdx[port]);

    for (i = 0; i < PTIN_IGMP_CLIENTIDX_MAX; i++) 
    {
      clientIdx_pool[port][i].client_idx = i;
      clientIdx_pool[port][i].in_use     = L7_FALSE;
      dl_queue_add_tail(&queue_free_clientIdx[port], (dl_queue_elem_t *) &clientIdx_pool[port][i]);
    }

    dl_queue_init(&igmpClients_unified.queue_free_clientDevices[port]);

    for (i=0; i<PTIN_IGMP_CLIENTIDX_MAX; i++)
    {
      igmpClients_unified.client_devices[port][i].client = L7_NULLPTR;   /* Pointer to client structure */
      dl_queue_add_tail(&igmpClients_unified.queue_free_clientDevices[port], (dl_queue_elem_t *) &igmpClients_unified.client_devices[port][i]);
    }
  }

  /* IGMP associaations */
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED

  memset(&igmpPairDB, 0x00, sizeof(igmpPairDB));

  igmpPairDB.number_of_entries = 0;

  igmpPairDB.igmpPairTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, IGMPASSOC_CHANNELS_MAX * sizeof(avlTreeTables_t)); 
  igmpPairDB.igmpPairDataHeap = (ptinIgmpPairInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, IGMPASSOC_CHANNELS_MAX * sizeof(ptinIgmpPairInfoData_t)); 

  if ((igmpPairDB.igmpPairTreeHeap == L7_NULLPTR) ||
      (igmpPairDB.igmpPairDataHeap == L7_NULLPTR))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error allocating data for IGMP Pairing AVL Trees\n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all IGMP associations */
  memset (&igmpPairDB.igmpPairAvlTree , 0x00, sizeof(avlTree_t));
  memset ( igmpPairDB.igmpPairTreeHeap, 0x00, sizeof(avlTreeTables_t)*IGMPASSOC_CHANNELS_MAX);
  memset ( igmpPairDB.igmpPairDataHeap, 0x00, sizeof(ptinIgmpPairInfoData_t)*IGMPASSOC_CHANNELS_MAX);

  // AVL Tree creations - snoopIpAvlTree
  avlCreateAvlTree(&(igmpPairDB.igmpPairAvlTree),
                   igmpPairDB.igmpPairTreeHeap,
                   igmpPairDB.igmpPairDataHeap,
                   IGMPASSOC_CHANNELS_MAX, 
                   sizeof(ptinIgmpPairInfoData_t),
                   0x10,
                   sizeof(ptinIgmpPairDataKey_t));
#endif

  ptin_igmp_stats_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_igmp_stats_sem == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to create ptin_igmp_stats_sem semaphore!");
    return L7_FAILURE;
  }

  ptin_igmp_clients_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_igmp_clients_sem == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to create ptin_igmp_clients_sem semaphore!");
    return L7_FAILURE;
  }

  ptin_igmp_clients_snapshot_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_igmp_clients_snapshot_sem == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to create ptin_igmp_clients_snapshot_sem semaphore!");
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
#ifdef CLIENT_TIMERS_SUPPORTED
  /* Timers init */
  ptin_igmp_timersMng_deinit();
#endif

  /* CLIENTS CLEANUP */

  /* Clean list of clients */
  igmpClients_unified.number_of_clients = 0;
  memset(igmpClients_unified.number_of_clients_per_intf,0x00,sizeof(igmpClients_unified.number_of_clients_per_intf));
  memset(&igmpClients_unified.client_devices, 0x00, sizeof(igmpClients_unified.client_devices));

  /* Clean pools */
  memset(clientIdx_pool, 0x00, sizeof(clientIdx_pool));

  // AVL Tree creations - snoopIpAvlTree
  avlPurgeAvlTree(&(igmpClients_unified.avlTree.igmpClientsAvlTree), PTIN_SYSTEM_IGMP_MAXCLIENTS);

  osapiFree(L7_PTIN_COMPONENT_ID, igmpClients_unified.avlTree.igmpClientsTreeHeap); 
  osapiFree(L7_PTIN_COMPONENT_ID, igmpClients_unified.avlTree.igmpClientsDataHeap); 

  /* Reset structure of unified list of clients */
  memset(&igmpClients_unified, 0x00, sizeof(igmpClients_unified));

  /* CLIENT GROUPS CLEANUP */

  // AVL Tree creations - snoopIpAvlTree
  avlPurgeAvlTree(&(igmpClientGroups.avlTree.igmpClientsAvlTree),PTIN_SYSTEM_IGMP_MAXONUS);

  osapiFree(L7_PTIN_COMPONENT_ID, igmpClientGroups.avlTree.igmpClientsTreeHeap);
  osapiFree(L7_PTIN_COMPONENT_ID, igmpClientGroups.avlTree.igmpClientsDataHeap);

  /* Reset structure of unified list of clients */
  memset(&igmpClientGroups, 0x00, sizeof(igmpClientGroups));

  osapiSemaDelete(ptin_igmp_stats_sem);
  ptin_igmp_stats_sem = L7_NULLPTR;

  osapiSemaDelete(ptin_igmp_clients_sem);
  ptin_igmp_clients_sem = L7_NULLPTR;

  osapiSemaDelete(ptin_igmp_clients_snapshot_sem);
  ptin_igmp_clients_snapshot_sem = L7_NULLPTR;

  LOG_INFO(LOG_CTX_PTIN_IGMP, "IGMP deinit OK");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function is used exclusively for encoding the floating
*           point representation as described in RFC 3376 section 4.1.1
*           (Max Resp Code) and section 4.1.7 (Querier's * Query Interval Code).
*           An out of range parameter causes the output parm "code" to
*           be set to 0.
*
* @param    num   @b{ (input) }    Number to be encoded
* @param    code  @b{ (output) }   Coded value
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
//static void snoop_fp_encode(L7_uchar8 family,L7_int32 num, void *code)
//{
//  L7_int32 exp, mant;
//  L7_uchar8 *codev4;
//  L7_ushort16 *codev6;
//
//  if (family == L7_AF_INET)
//  {
//    codev4 = (L7_uchar8 *)code;
//    if (num < 128)
//    {
//      *codev4 = num;
//    }
//    else
//    {
//      mant = num >> 3;
//      exp = 0;
//      for (;;)
//      {
//        if ((mant & 0xfffffff0) == 0x00000010)
//          break;
//        mant = mant >> 1;
//        exp++;
//        /* Check for out of range */
//        if (exp > 7)
//        {
//          *codev4 = 0;
//          return;
//        }
//      }
//
//      mant = mant & 0x0f;
//      *codev4 = (L7_uchar8)(0x80 | (exp<<4) | mant);
//    }
//  }
//  else if (family == L7_AF_INET6)
//  {
//    codev6 = (L7_ushort16 *)code;
//    if (num < 32768)
//    {
//      *codev6 = num;
//    }
//    else
//    {
//      mant = num >> 3;
//      exp = 0;
//      for (;;)
//      {
//        if ((mant & 0xfffffff0) == 0x00000010)
//          break;
//        mant = mant >> 1;
//        exp++;
//        /* Check for out of range */
//        if (exp > 7)
//        {
//          *codev6 = 0;
//          return;
//        }
//      }
//
//      mant = mant & 0x0f;
//      *codev6 = (L7_ushort16)(0x80 | (exp<<4) | mant);
//    }
//  }
//}

/**
 * Applies IGMP Proxy configuration
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_proxy_config_set__snooping_old(ptin_IgmpProxyCfg_t *igmpProxy)
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

  /* Network Version */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_NETWORKVERSION
      && igmpProxyCfg.networkVersion != igmpProxy->networkVersion)
  {
    igmpProxyCfg.networkVersion = igmpProxy->networkVersion;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "  IGMP Network Version:                     %u", igmpProxyCfg.networkVersion);
  }

  /* Client Version */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_CLIENTVERSION
      && igmpProxyCfg.clientVersion != igmpProxy->clientVersion)
  {
    igmpProxyCfg.clientVersion = igmpProxy->clientVersion;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "  IGMP Client Version:                      %u", igmpProxyCfg.clientVersion);
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
  if(igmpProxy->mask & PTIN_IGMP_PROXY_MASK_QUERIER)
  {
    /* Querier Robustness */
    if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_RV
        && igmpProxyCfg.querier.robustness != igmpProxy->querier.robustness && igmpProxy->querier.robustness>=PTIN_MIN_ROBUSTNESS_VARIABLE && igmpProxy->querier.robustness<=PTIN_MAX_ROBUSTNESS_VARIABLE)
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

    /* Update AUTO flags */
    igmpProxyCfg.querier.flags = igmpProxy->querier.flags;
  }

  /* *******************
   * IGMP Host config
   * *******************/
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "  Host config:");
  if(igmpProxy->mask & PTIN_IGMP_PROXY_MASK_QUERIER)
  {
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

    /* Max Records per Report */
    if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_MRPR && igmpProxyCfg.host.max_records_per_report != igmpProxy->host.max_records_per_report )
    {
      igmpProxyCfg.host.max_records_per_report = igmpProxy->host.max_records_per_report;
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Max Records per Report:                %u (s)", igmpProxyCfg.host.max_records_per_report);
    }

    /* Update AUTO flags */
    igmpProxyCfg.host.flags    = igmpProxy->host.flags;
  }

  /* Finally, (de)activate IGMP module */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_ADMIN
      && igmpProxyCfg.admin != igmpProxy->admin)
  {
    igmpProxyCfg.admin = igmpProxy->admin;
  }

  osapiSemaGive(igmp_sem);

  return L7_SUCCESS;
}
L7_RC_t ptin_igmp_proxy_config_set(PTIN_MGMD_CTRL_MGMD_CONFIG_t *igmpProxy)
{
  PTIN_MGMD_EVENT_t      inEventMsg = {0}, outEventMsg = {0};
  PTIN_MGMD_EVENT_CTRL_t ctrlResMsg = {0};

  /* Create and send a PTIN_MGMD_EVENT_CTRL_PROXY_CONFIG_SET event to MGMD */
  ptin_mgmd_event_ctrl_create(&inEventMsg, PTIN_MGMD_EVENT_CTRL_PROXY_CONFIG_SET, rand(), 0, ptinMgmdTxQueueId, (void*) igmpProxy, (uint32) sizeof(PTIN_MGMD_CTRL_MGMD_CONFIG_t));
  ptin_mgmd_sendCtrlEvent(&inEventMsg, &outEventMsg);

  /* Parse the received reply */
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "MGMD replied");
  ptin_mgmd_event_ctrl_parse(&outEventMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Res     : %u",   ctrlResMsg.res);

  /* If sucesseful, configure the old PTIN_IGMP struct, required for dynamic clients */
  if(0 == ctrlResMsg.res)
  {
    ptin_IgmpProxyCfg_t oldIgmpConfig = {0};

    oldIgmpConfig.mask                                   = igmpProxy->mask;
    oldIgmpConfig.admin                                  = igmpProxy->admin;
    oldIgmpConfig.networkVersion                         = igmpProxy->networkVersion;
    oldIgmpConfig.clientVersion                          = igmpProxy->clientVersion;
    oldIgmpConfig.ipv4_addr.s_addr                       = igmpProxy->ipv4Addr;
    oldIgmpConfig.igmp_cos                               = igmpProxy->igmpCos;
    oldIgmpConfig.fast_leave                             = igmpProxy->fastLeave;

    oldIgmpConfig.querier.mask                           = igmpProxy->querier.mask;
    oldIgmpConfig.querier.flags                          = igmpProxy->querier.flags;
    oldIgmpConfig.querier.robustness                     = igmpProxy->querier.robustness;
    oldIgmpConfig.querier.query_interval                 = igmpProxy->querier.queryInterval;
    oldIgmpConfig.querier.query_response_interval        = igmpProxy->querier.queryResponseInterval;
    oldIgmpConfig.querier.group_membership_interval      = igmpProxy->querier.groupMembershipInterval;
    oldIgmpConfig.querier.other_querier_present_interval = igmpProxy->querier.otherQuerierPresentInterval;
    oldIgmpConfig.querier.startup_query_interval         = igmpProxy->querier.startupQueryInterval;
    oldIgmpConfig.querier.startup_query_count            = igmpProxy->querier.startupQueryCount;
    oldIgmpConfig.querier.last_member_query_interval     = igmpProxy->querier.lastMemberQueryInterval;
    oldIgmpConfig.querier.last_member_query_count        = igmpProxy->querier.lastMemberQueryCount;
    oldIgmpConfig.querier.older_host_present_timeout     = igmpProxy->querier.olderHostPresentTimeout;

    oldIgmpConfig.host.mask                              = igmpProxy->host.mask;
    oldIgmpConfig.host.flags                             = igmpProxy->host.flags;
    oldIgmpConfig.host.robustness                        = igmpProxy->host.robustness;
    oldIgmpConfig.host.unsolicited_report_interval       = igmpProxy->host.unsolicitedReportInterval;
    oldIgmpConfig.host.older_querier_present_timeout     = igmpProxy->host.olderQuerierPresentTimeout;
    oldIgmpConfig.host.max_records_per_report            = igmpProxy->host.maxRecordsPerReport;

    ptin_igmp_proxy_config_set__snooping_old(&oldIgmpConfig);
  }

  return ctrlResMsg.res;
}

/**
 * Gets IGMP Proxy configuration
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_proxy_config_get__snooping_old(ptin_IgmpProxyCfg_t *igmpProxy)
{
  *igmpProxy = igmpProxyCfg;

  igmpProxy->mask         = 0xFF;
  igmpProxy->querier.mask = 0xFFFF;
  igmpProxy->host.mask    = 0xFF;

  return L7_SUCCESS;
}
L7_RC_t ptin_igmp_proxy_config_get(PTIN_MGMD_CTRL_MGMD_CONFIG_t *igmpProxy)
{
  PTIN_MGMD_EVENT_t      inEventMsg = {0}, outEventMsg = {0};
  PTIN_MGMD_EVENT_CTRL_t ctrlResMsg = {0};

  /* Create and send a PTIN_MGMD_EVENT_CTRL_PROXY_CONFIG_GET event to MGMD */
  ptin_mgmd_event_ctrl_create(&inEventMsg, PTIN_MGMD_EVENT_CTRL_PROXY_CONFIG_GET, rand(), 0, ptinMgmdTxQueueId, (void*) igmpProxy, (uint32) sizeof(PTIN_MGMD_CTRL_MGMD_CONFIG_t));
  ptin_mgmd_sendCtrlEvent(&inEventMsg, &outEventMsg);

  /* Parse the received reply */
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "MGMD replied");
  ptin_mgmd_event_ctrl_parse(&outEventMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Res     : %u",   ctrlResMsg.res);
 
  /* Copy the response contents to igmpProxy */
  if(sizeof(PTIN_MGMD_CTRL_MGMD_CONFIG_t) != ctrlResMsg.dataLength)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unexpected size in the MGMD response [dataLength:%u/%u]", ctrlResMsg.dataLength, sizeof(PTIN_MGMD_CTRL_MGMD_CONFIG_t));
    return L7_FAILURE;
  }
  else
  {
    memcpy(igmpProxy, ctrlResMsg.data, ctrlResMsg.dataLength);
  }

  return ctrlResMsg.res;
}

/**
 * Reset Proxy machine
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_igmp_proxy_reset(void)
{
  L7_uint32 admin;

  LOG_INFO(LOG_CTX_PTIN_IGMP,"Multicast queriers reset:");

  /* Read querier admin status */
  if (usmDbSnoopQuerierAdminModeGet(&admin, L7_AF_INET)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error reading Querier Admin status");
    return L7_FAILURE;
  }
  /* If disabled, there is nothing to be done */
  if (!admin)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Multicast querier is disabled... nothing to be done!");
    return L7_SUCCESS;
  }

  LOG_INFO(LOG_CTX_PTIN_IGMP,"Going to reset Multicast queriers...");

  /* Disable, and reenable querier mechanism for all vlans */
  if (usmDbSnoopQuerierAdminModeSet(L7_DISABLE, L7_AF_INET)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Failed Querier disable operation");
    return L7_FAILURE;
  }

  /* Wait a while */
  //osapiSleepMSec(100);

  if (usmDbSnoopQuerierAdminModeSet(L7_ENABLE, L7_AF_INET)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed Querier reenable operation");
    return L7_FAILURE;
  }

  LOG_INFO(LOG_CTX_PTIN_IGMP,"Multicast queriers reenabled!");

  return L7_SUCCESS;
}

/**
 * Check if a EVC is being used in an IGMP instance
 * 
 * @param evc_idx : evc id
 * 
 * @return L7_RC_t : L7_TRUE or L7_FALSE
 */
L7_RC_t ptin_igmp_is_evc_used(L7_uint32 evc_idx)
{
  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid eEVC id: evc_idx=%u",evc_idx);
    return L7_FALSE;
  }

  /* This evc must be active */
  if (!ptin_evc_is_in_use(evc_idx))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"eEVC id is not active: evc_idx=%u",evc_idx);
    return L7_FALSE;
  }

  /* Check if this EVC is being used by any igmp instance */
  if (ptin_igmp_instance_find_fromSingleEvcId(evc_idx,L7_NULLPTR)!=L7_SUCCESS)
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
L7_RC_t ptin_igmp_instance_add(L7_uint32 McastEvcId, L7_uint32 UcastEvcId)
{
  L7_uint igmp_idx;

  /* Validate arguments */
  if (McastEvcId>=PTIN_SYSTEM_N_EXTENDED_EVCS ||
      UcastEvcId>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid eEVC ids: [mcEvcId,ucEvcId]=[%u,%u]",McastEvcId,UcastEvcId);
    return L7_FAILURE;
  }

  /* These evcs must be active */
  if (!ptin_evc_is_in_use(McastEvcId)
      #if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
      || !ptin_evc_is_in_use(UcastEvcId)
      #endif
     )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"eEVC ids are not active: [mcEvcId,ucEvcId]=[%u,%u]",McastEvcId,UcastEvcId);
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

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Using free index %u",igmp_idx);

  /* Save direct referencing to igmp index from evc ids */
  if (ptin_evc_igmpInst_set(McastEvcId, igmp_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error saving igmp instance to ext evc id %u", McastEvcId);
    return L7_FAILURE;
  }
  #if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
  if (ptin_evc_igmpInst_set(UcastEvcId, igmp_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error saving igmp instance to ext evc id %u", UcastEvcId);
    ptin_evc_igmpInst_set(McastEvcId, IGMP_INVALID_ENTRY);
    return L7_FAILURE;
  }
  #endif

  /* Save data in free instance */
  igmpInstances[igmp_idx].McastEvcId      = McastEvcId;
  igmpInstances[igmp_idx].UcastEvcId      = UcastEvcId;
  igmpInstances[igmp_idx].nni_ovid        = 0;
  igmpInstances[igmp_idx].n_evcs          = 1;
  igmpInstances[igmp_idx].inUse           = L7_TRUE;

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMP index %u",igmp_idx);

  /* Configure querier for this instance */
  if (ptin_igmp_querier_configure(igmp_idx,L7_ENABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error setting querier configuration for igmp_idx=%u",igmp_idx);
    memset(&igmpInstances[igmp_idx],0x00,sizeof(st_IgmpInstCfg_t));
    ptin_evc_igmpInst_set(McastEvcId, IGMP_INVALID_ENTRY);
    #if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
    ptin_evc_igmpInst_set(UcastEvcId, IGMP_INVALID_ENTRY);
    #endif
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMP index %u",igmp_idx);

  /* Configure trapping for this instance */
  if (ptin_igmp_trap_configure(igmp_idx,L7_ENABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring IGMP snooping for igmp_idx=%u",igmp_idx);
    ptin_igmp_querier_configure(igmp_idx,L7_DISABLE);
    memset(&igmpInstances[igmp_idx],0x00,sizeof(st_IgmpInstCfg_t));
    ptin_evc_igmpInst_set(McastEvcId, IGMP_INVALID_ENTRY);
    #if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
    ptin_evc_igmpInst_set(UcastEvcId, IGMP_INVALID_ENTRY);
    #endif
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
L7_RC_t ptin_igmp_instance_remove(L7_uint32 McastEvcId, L7_uint32 UcastEvcId)
{
  L7_uint igmp_idx;

  /* Validate arguments */
  if (McastEvcId>=PTIN_SYSTEM_N_EXTENDED_EVCS ||
      UcastEvcId>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid eEVC ids: [mcEvcId,ucEvcId]=[%u,%u]",McastEvcId,UcastEvcId);
    return L7_FAILURE;
  }

  /* Check if there is an instance with these parameters */
  if (ptin_igmp_instance_find(McastEvcId,UcastEvcId,&igmp_idx)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"There is no instance with [mcEvcId,ucEvcId]=[%u,%u]",McastEvcId,UcastEvcId);
    return L7_SUCCESS;
  }

  /* Deconfigure querier for this instance */
  if (ptin_igmp_querier_configure(igmp_idx,L7_DISABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error clearing querier configuration for igmp_idx=%u",igmp_idx);
    return L7_FAILURE;
  }

  /* Configure querier for this instance */
  if (ptin_igmp_trap_configure(igmp_idx, L7_DISABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring IGMP snooping for igmp_idx=%u",igmp_idx);
    ptin_igmp_querier_configure(igmp_idx,L7_ENABLE);
    return L7_FAILURE;
  }

  /* Reset direct referencing to igmp index from evc ids */
  if (ptin_evc_igmpInst_set(McastEvcId, IGMP_INVALID_ENTRY) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error resetting igmp instance to ext evc id %u", McastEvcId);
    return L7_FAILURE;
  }
  #if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
  if (ptin_evc_igmpInst_set(UcastEvcId, IGMP_INVALID_ENTRY) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error resetting igmp instance to ext evc id %u", UcastEvcId);
    return L7_FAILURE;
  }
  #endif

  /* Clear data and free instance */
  igmpInstances[igmp_idx].McastEvcId      = 0;
  igmpInstances[igmp_idx].UcastEvcId      = 0;
  igmpInstances[igmp_idx].nni_ovid        = 0;
  igmpInstances[igmp_idx].n_evcs          = 0;
  igmpInstances[igmp_idx].inUse           = L7_FALSE;

#ifdef PTIN_MGMD_MC_SERVICE_ID_IN_USE//This is only applicable when MGMD is configured to used the Multicast Service Id
  /* If we are removing the service, force a clear of all it's records on MGMD as well */
  if ((ptin_igmp_mgmd_service_remove(McastEvcId)!=L7_SUCCESS))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Evc index %u: Unable to remove service from MGMD",McastEvcId);
    return L7_FAILURE;
  }
#endif

  return L7_SUCCESS;
}

/**
 * Removes all IGMP instances
 * 
 * @param McastEvcId : Multicast evc id 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_clean_all(void)
{
  L7_uint igmp_idx;
  L7_RC_t rc, rc_global = L7_SUCCESS;

  /* Remove all instances */
  for (igmp_idx=0; igmp_idx<PTIN_SYSTEM_N_IGMP_INSTANCES; igmp_idx++)
  {
    if ((rc=ptin_igmp_instance_delete(igmp_idx))!=L7_SUCCESS)
    {
      if (rc_global == L7_SUCCESS)
        rc_global = rc;
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error clearing igmp_idx=%u",igmp_idx);
    }
  }

  #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  /* Remove Multicast associations */
  if ((rc=igmp_assoc_clean_all())!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error clearing igmp associations");
    rc_global = rc;
  }
  #endif

  /* Now, remove all clients */
  if ((rc=ptin_igmp_all_clients_flush())!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error clearing igmp clients");
    rc_global = rc;
  }

  return rc_global;
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
 * @param evc_idx : evc index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_instance_destroy(L7_uint32 evc_idx)
{
  L7_uint igmp_idx;

  /* Check if this evc index is used in any IGMP instance */
  if (ptin_igmp_instance_find_fromSingleEvcId(evc_idx,&igmp_idx)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"EVC id %u is not used in any IGMP instance",evc_idx);
    return L7_SUCCESS;
  }

  return ptin_igmp_instance_remove(igmpInstances[igmp_idx].McastEvcId,igmpInstances[igmp_idx].UcastEvcId);
}

/**
 * Associate an EVC to an IGMP instance
 * 
 * @param evc_idx : Multicast evc id 
 * @param nni_ovlan  : Network outer vlan (used to aggregate 
 *                   evcs in one instance: 0 to not be used)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_evc_add(L7_uint32 evc_idx, L7_uint16 nni_ovlan)
{
  L7_uint igmp_idx;
  L7_uint evc_type;
  L7_BOOL new_instance = L7_FALSE;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid eEVC ids: mcEvcId=%u",evc_idx);
    return L7_FAILURE;
  }

  /* These evcs must be active */
  if (!ptin_evc_is_in_use(evc_idx))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"eEVC ids are not active: mcEvcId=%u",evc_idx);
    return L7_FAILURE;
  }

  /* Get EVC type */
  if (ptin_evc_check_evctype(evc_idx, &evc_type) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting eEVC %u type",evc_idx);
    return L7_FAILURE;
  }

  /* If EVC is not QUATTRO pointo-to-point, use tradittional isnatnce management */
  if (evc_type != PTIN_EVC_TYPE_QUATTRO_P2P)
  {
    nni_ovlan = 0;
  }

  /* Is EVC associated to an IGMP instance? */
  if ( ptin_igmp_instance_find(evc_idx, 0 /*Not used*/, L7_NULLPTR)==L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"There is already an instance with mcEvcId=%u",evc_idx);
    return L7_SUCCESS;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to add evc %u from igmp_idx=%u",evc_idx, igmp_idx);

  /* Check if there is an instance with the same NNI outer vlan: use it! */
  /* Otherwise, create a new instance */
  if ((nni_ovlan < PTIN_VLAN_MIN || nni_ovlan > PTIN_VLAN_MAX) ||
      ptin_igmp_instance_find_agg(nni_ovlan, &igmp_idx)!=L7_SUCCESS)
  {
    /* Find an empty instance to be used */
    if (ptin_igmp_instance_find_free(&igmp_idx)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"There is no free instances to be used");
      return L7_FAILURE;
    }
    else
    {
      new_instance = L7_TRUE;
    }
  }

  /* Save direct referencing to igmp index from evc ids */
  if (ptin_evc_igmpInst_set(evc_idx, igmp_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error setting igmp instance to ext evc id %u", evc_idx);
    return L7_FAILURE;
  }

  /* Save data in free instance */
  if (new_instance)
  {
    igmpInstances[igmp_idx].McastEvcId      = evc_idx;
    igmpInstances[igmp_idx].UcastEvcId      = 0;
    igmpInstances[igmp_idx].nni_ovid        = (nni_ovlan>=PTIN_VLAN_MIN && nni_ovlan<=PTIN_VLAN_MAX) ? nni_ovlan : 0;
    igmpInstances[igmp_idx].n_evcs          = 0;
    igmpInstances[igmp_idx].inUse           = L7_TRUE;
  }

  /* Querier */
  if (ptin_igmp_evc_querier_configure(evc_idx, L7_ENABLE) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error setting querier configuration for evc_idx=%u (igmp_idx=%u)",evc_idx,igmp_idx);
    if (new_instance)
      memset(&igmpInstances[igmp_idx], 0x00, sizeof(st_IgmpInstCfg_t));
    ptin_evc_igmpInst_set(evc_idx, IGMP_INVALID_ENTRY);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to enable trap rule of igmp_idx=%u",evc_idx, igmp_idx);

  /* Trap rule */
  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  if (evc_type!=PTIN_EVC_TYPE_QUATTRO_P2P || igmp_quattro_p2p_evcs==0)
  #endif
  {
    if (ptin_igmp_evc_trap_set(evc_idx, 0 /* Not used*/, L7_ENABLE)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring IGMP snooping for igmp_idx=%u",igmp_idx);
      ptin_igmp_evc_querier_configure(evc_idx, L7_DISABLE);
      memset(&igmpInstances[igmp_idx], 0x00, sizeof(st_IgmpInstCfg_t));
      ptin_evc_igmpInst_set(evc_idx, IGMP_INVALID_ENTRY);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success enabling trap rule of igmp_idx=%u",igmp_idx);
  }

  /* One more EVC associated to this instance */
  igmpInstances[igmp_idx].n_evcs++;

  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  /* Update number of QUATTRO-P2P evcs */
  if (evc_type == PTIN_EVC_TYPE_QUATTRO_P2P)
  {
    igmp_quattro_p2p_evcs++;
  }
  #endif

  return L7_SUCCESS;
}


/**
 * Deassociate an EVC from an IGMP instance
 * 
 * @param evc_idx : Multicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_evc_remove(L7_uint32 evc_idx)
{
  L7_uint igmp_idx;
  L7_uint evc_type;
  L7_uint16 nni_ovlan;
  L7_BOOL remove_instance = L7_TRUE;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid eEVC ids: mcEvcId=%u",evc_idx);
    return L7_FAILURE;
  }

  /* Check if there is an instance with these parameters */
  if ( ptin_igmp_instance_find(evc_idx, 0 /*Not used*/, &igmp_idx)!=L7_SUCCESS )
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"There is no instance with [mcEvcId,ucEvcId]=[%u,%u]",evc_idx);
    return L7_SUCCESS;
  }

  /* Get EVC type */
  if (ptin_evc_check_evctype(evc_idx, &evc_type) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting eEVC %u type", evc_idx);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to remove evc %u from igmp_idx=%u",evc_idx, igmp_idx);

  /* Remove instance? */
  remove_instance = ((igmpInstances[igmp_idx].nni_ovid==0 || igmpInstances[igmp_idx].nni_ovid>4095) ||
                     (igmpInstances[igmp_idx].n_evcs <= 1));

  /* NNI outer vlan */
  nni_ovlan = igmpInstances[igmp_idx].nni_ovid;

  /* Deconfigure querier for this instance */
  if (ptin_igmp_evc_querier_configure(evc_idx, L7_DISABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error clearing querier configuration for igmp_idx=%u",igmp_idx);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to disable trap rule of igmp_idx=%u", igmp_idx);

  /* Configure querier for this instance */
  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  if (evc_type!=PTIN_EVC_TYPE_QUATTRO_P2P || igmp_quattro_p2p_evcs<=1)
  #endif
  {
    if (ptin_igmp_evc_trap_set(evc_idx, 0 /*Not used*/, L7_DISABLE)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring IGMP snooping for igmp_idx=%u",igmp_idx);
      ptin_igmp_evc_querier_configure(evc_idx, L7_ENABLE);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success disabling trap rule of igmp_idx=%u",igmp_idx);
  }

  /* Reset direct referencing to igmp index from evc ids */
  if (ptin_evc_igmpInst_set(evc_idx, IGMP_INVALID_ENTRY) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error resetting igmp instance to ext evc id %u", evc_idx);
    return L7_FAILURE;
  }

  /* Only clear instance, if there is no one using this NNI outer vlan */
  if (remove_instance)
  {
    igmpInstances[igmp_idx].McastEvcId      = 0;
    igmpInstances[igmp_idx].UcastEvcId      = 0;
    igmpInstances[igmp_idx].nni_ovid        = 0;
    igmpInstances[igmp_idx].n_evcs          = 0;
    igmpInstances[igmp_idx].inUse           = L7_FALSE;
  }

  /* One less EVC */
  if (igmpInstances[igmp_idx].n_evcs > 0)
    igmpInstances[igmp_idx].n_evcs--;

  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  /* Update number of QUATTRO-P2P evcs */
  if (evc_type == PTIN_EVC_TYPE_QUATTRO_P2P)
  {
    if (igmp_quattro_p2p_evcs>0)  igmp_quattro_p2p_evcs--;
  }
  #endif

  return L7_SUCCESS;
}


/**
 * Reactivate all IGMP instances
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_evcs_reactivate(void)
{
  L7_uint  evc_idx;
  L7_uint8 igmp_idx;
  L7_RC_t  rc = L7_SUCCESS;

  /* Run all EVCs with IGMP instance association */
  for (evc_idx=0; evc_idx<PTIN_SYSTEM_N_EXTENDED_EVCS; evc_idx++)
  {
    if (ptin_evc_igmpInst_get(evc_idx, &igmp_idx) != L7_SUCCESS ||
        igmp_idx >= PTIN_SYSTEM_N_IGMP_INSTANCES)
      continue;

    /* Disable, and reenable querier for this instance */
    if (ptin_igmp_evc_querier_configure(evc_idx, L7_DISABLE)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error disabling querier for igmp_idx=%u",igmp_idx);
      rc = L7_FAILURE;
      continue;
    }
    if (ptin_igmp_evc_querier_configure(evc_idx, L7_ENABLE)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error enabling querier for igmp_idx=%u",igmp_idx);
      rc = L7_FAILURE;
      continue;
    }
  }

  return rc;
}


/**
 * Remove an EVC from a IGMP instance
 * 
 * @param evc_idx : evc index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_evc_destroy(L7_uint32 evc_idx)
{
  return ptin_igmp_evc_remove(evc_idx);
}

/**
 * Update snooping configuration, when interfaces are 
 * added/removed 
 * 
 * @param evc_idx     : EVC id 
 * @param ptin_intf : interface 
 * @param enable    : L7_TRUE when interface is added 
 *                    L7_FALSE when interface is removed
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_snooping_trap_interface_update(L7_uint32 evc_idx, ptin_intf_t *ptin_intf, L7_BOOL enable)
{
  return L7_SUCCESS;
}

/**
 * Add a new Multicast client
 * 
 * @param evc_idx     : evc id
 * @param client      : client identification parameters 
 * @param uni_ovid    : External Outer vlan 
 * @param uni_ivid    : External Inner vlan 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_client_add(L7_uint32 evc_idx, ptin_client_id_t *client, L7_uint16 uni_ovid, L7_uint16 uni_ivid)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;

  /* Validate, and rearrange, client info */
  if (ptin_igmp_clientId_convert(evc_idx, client)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client id");
    return L7_FAILURE;
  }

  /* If uni vlans are not provided, but interface is, get uni vlans from EVC data */
  if ( (uni_ovid<PTIN_VLAN_MIN || uni_ovid>PTIN_VLAN_MAX) &&
       (client->mask & PTIN_CLIENT_MASK_FIELD_INTF) &&
       (client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) )
  {
     /* Get interface as intIfNum format */
    if (ptin_intf_ptintf2intIfNum(&client->ptin_intf, &intIfNum)==L7_SUCCESS)
    {
      if (ptin_evc_extVlans_get(intIfNum, evc_idx, client->innerVlan, &uni_ovid, &uni_ivid) == L7_SUCCESS)
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Ext vlans for ptin_intf %u/%u, cvlan %u: uni_ovid=%u, uni_ivid=%u",
                  client->ptin_intf.intf_type,client->ptin_intf.intf_id, client->innerVlan, uni_ovid, uni_ivid);
      }
      else
      {
        uni_ovid = uni_ivid = 0;
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot get ext vlans for ptin_intf %u/%u, cvlan %u",
                client->ptin_intf.intf_type,client->ptin_intf.intf_id, client->innerVlan);
      }
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid ptin_intf %u/%u", client->ptin_intf.intf_type,client->ptin_intf.intf_id);
    }
  }

  /* Create new static client */
  rc = ptin_igmp_clientGroup_add(client, uni_ovid, uni_ivid);

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error creating static client");
    return L7_FAILURE;
  }

  return rc;
}

/**
 * Remove a Multicast client
 * 
 * @param evc_idx : evc id
 * @param client  : client identification parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_client_delete(L7_uint32 evc_idx, ptin_client_id_t *client)
{
  L7_RC_t rc;

  /* Validate, and rearrange, client info */
  /* If error, client does not exist: return success */
  if (ptin_igmp_clientId_convert(evc_idx, client)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client id");
    return L7_NOT_EXIST;
  }

  /* Remove client */
  rc = ptin_igmp_clientGroup_remove(client);

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error deleting static client");
  }

  return rc;
}

/**
 * Remove all Multicast clients 
 * 
 * @param McastEvcId  : Multicast evc id
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_all_clients_flush(void)
{
  /* Remove all clients */
  if ( ptin_igmp_clientGroup_clean()!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error flushing all clients");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Clients flushed!");

  return L7_SUCCESS;
}

/**
 * Reset all MGMD General Queriers 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_generalquerier_reset(void)
{
  PTIN_MGMD_EVENT_t             reqMsg       = {0};
  PTIN_MGMD_EVENT_t             resMsg       = {0};
  PTIN_MGMD_EVENT_CTRL_t        ctrlResMsg   = {0};
  PTIN_MGMD_CTRL_QUERY_CONFIG_t mgmdQuerierConfigMsg = {0}; 

  mgmdQuerierConfigMsg.family = PTIN_MGMD_AF_INET;
  ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_GENERAL_QUERY_RESET, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdQuerierConfigMsg, sizeof(PTIN_MGMD_CTRL_QUERY_CONFIG_t));
  ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
  ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Response");
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "  CTRL Res     : %u",   ctrlResMsg.res);

  return (L7_RC_t)ctrlResMsg.res;
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
//static L7_uint16              channelList_size=0;
//static ptin_igmpChannelInfo_t channelList[L7_MAX_GROUP_REGISTRATION_ENTRIES*PTIN_SYSTEM_IGMP_MAXSOURCES_PER_GROUP];
//static ptin_igmpChannelInfo_t channelList_tmp[L7_MAX_GROUP_REGISTRATION_ENTRIES*PTIN_SYSTEM_IGMP_MAXSOURCES_PER_GROUP];

L7_RC_t ptin_igmp_channelList_get(L7_uint32 McastEvcId, ptin_client_id_t *client, L7_uint16 channel_index, L7_uint16 *number_of_channels, ptin_igmpChannelInfo_t *channel_list, L7_uint16 *total_channels)
{
  L7_uint                                igmp_idx;
  L7_uint16                              McastRootVlan;
  PTIN_MGMD_EVENT_t                      reqMsg        = {0};
  PTIN_MGMD_EVENT_t                      resMsg        = {0};
  PTIN_MGMD_EVENT_CTRL_t                 ctrlResMsg    = {0};
  PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t  mgmdGroupsMsg = {0}; 
  PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t mgmdGroupsRes = {0};

  /* Validate arguments */
  if (channel_list==L7_NULLPTR || number_of_channels==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Null parameters");
    return L7_FAILURE;
  }

  /* Validate client */
  if (client==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client pointer");
    return L7_FAILURE;
  }

  /* Validate and rearrange clientId info */
  if (client->mask != 0)
  {
    if (ptin_igmp_clientId_convert(McastEvcId, client)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client id");
      return L7_FAILURE;
    }
  }

  /* Get IGMP instance index */
  if (ptin_igmp_instance_find_fromMcastEvcId(McastEvcId, &igmp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"There is no IGMP instance with MC EVC id %u",McastEvcId);
    return L7_FAILURE;
  }

  /* Get Multicast root vlan */
  if (ptin_evc_intRootVlan_get(igmpInstances[igmp_idx].McastEvcId, &McastRootVlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting McastRootVlan for MCEvcId=%u (intVlan=%u)",McastEvcId, McastRootVlan);
    return L7_FAILURE;
  }

  if(client->mask == 0)
  {
    mgmdGroupsMsg.serviceId = McastEvcId;
    mgmdGroupsMsg.entryId   = (channel_index==0)?(PTIN_MGMD_CTRL_ACTIVEGROUPS_FIRST_ENTRY):(channel_index);
    ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_GROUPS_GET, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdGroupsMsg, sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t));
    ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
    ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
    LOG_INFO(LOG_CTX_PTIN_IGMP, "Response");
    LOG_INFO(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code: %08X",      ctrlResMsg.msgCode);
    LOG_INFO(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id  : %08X",      ctrlResMsg.msgId);
    LOG_INFO(LOG_CTX_PTIN_IGMP, "  CTRL Res     : %u",        ctrlResMsg.res);
    LOG_INFO(LOG_CTX_PTIN_IGMP, "  CTRL Length  : %u (%.1f)", ctrlResMsg.dataLength, ((double)ctrlResMsg.dataLength)/sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));

    if (0 == ctrlResMsg.dataLength%sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t))
    {
      uint32 groupCount = 0; 

      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Active groups (Service:%u)", McastEvcId);
      while((ctrlResMsg.dataLength > 0) && (groupCount < *number_of_channels))
      {
        memcpy(&mgmdGroupsRes, ctrlResMsg.data + groupCount*sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t), sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));

        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  Entry [%u]", mgmdGroupsRes.entryId);
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Type:           %s",   mgmdGroupsRes.groupType==PTIN_MGMD_CTRL_GROUPTYPE_DYNAMIC? ("Dynamic"):("Static"));
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Filter-Mode:    %s",   mgmdGroupsRes.filterMode==PTIN_MGMD_CTRL_FILTERMODE_INCLUDE? ("Include"):("Exclude"));
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Group Timer:    %u",   mgmdGroupsRes.groupTimer);
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Groups Address: %08X", mgmdGroupsRes.groupIP);
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Source Timer:   %u",   mgmdGroupsRes.sourceTimer);
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Source Address: %08X", mgmdGroupsRes.sourceIP);

        inetAddressSet(L7_AF_INET, &mgmdGroupsRes.groupIP, &channel_list[groupCount].groupAddr);
        inetAddressSet(L7_AF_INET, &mgmdGroupsRes.sourceIP, &channel_list[groupCount].sourceAddr);
        channel_list[groupCount].static_type = mgmdGroupsRes.groupType;

        ctrlResMsg.dataLength -= sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t);
        ++groupCount;
      }

      *number_of_channels = groupCount;
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid response size from MGMD [size:%u]", ctrlResMsg.dataLength);
      return L7_FAILURE;
    }
  }
  else
  {
    L7_uint32                     clientId;
    ptinIgmpClientGroupInfoData_t *clientGroup;
    L7_uint32                     intIfNum;
    uint32                        groupCount = 0;

    /* Find client */
    if (ptin_igmp_clientGroup_find(client, &clientGroup)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,
              "Error searching for client {mask=0x%02x,"
              "port=%u/%u,"
              "svlan=%u,"
              "cvlan=%u,"
              "ipAddr=%u.%u.%u.%u,"
              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} ",
              client->mask,
              client->ptin_intf.intf_type, client->ptin_intf.intf_id,
              client->outerVlan,
              client->innerVlan,
              (client->ipv4_addr>>24) & 0xff, (client->ipv4_addr>>16) & 0xff, (client->ipv4_addr>>8) & 0xff, client->ipv4_addr & 0xff,
              client->macAddr[0],client->macAddr[1],client->macAddr[2],client->macAddr[3],client->macAddr[4],client->macAddr[5]);
      return L7_FAILURE;
    }

    /* Validate interface */
    if (ptin_intf_ptintf2intIfNum(&client->ptin_intf, &intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting port %u/%u to intIfNum",client->ptin_intf.intf_type,client->ptin_intf.intf_id);
      return L7_FAILURE;
    }

    for(clientId=0; clientId<(sizeof(clientGroup->client_bmp_list)*8); ++clientId)
    {
      if(IS_BITMAP_BIT_SET(clientGroup->client_bmp_list, clientId, sizeof(L7_uint32)))
      {
        mgmdGroupsMsg.serviceId = McastEvcId;
        mgmdGroupsMsg.portId    = intIfNum;
        mgmdGroupsMsg.clientId  = clientId;
        mgmdGroupsMsg.entryId   = (channel_index==0)?(PTIN_MGMD_CTRL_ACTIVEGROUPS_FIRST_ENTRY):(channel_index);
        ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_CLIENT_GROUPS_GET, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdGroupsMsg, sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t));
        ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
        ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
        LOG_INFO(LOG_CTX_PTIN_IGMP, "Response");
        LOG_INFO(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code: %08X",      ctrlResMsg.msgCode);
        LOG_INFO(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id  : %08X",      ctrlResMsg.msgId);
        LOG_INFO(LOG_CTX_PTIN_IGMP, "  CTRL Res     : %u",        ctrlResMsg.res);
        LOG_INFO(LOG_CTX_PTIN_IGMP, "  CTRL Length  : %u (%.1f)", ctrlResMsg.dataLength, ((double)ctrlResMsg.dataLength)/sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));
      }
    }

    if (0 == ctrlResMsg.dataLength%sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Active groups (Service:%u)", McastEvcId);
      while((ctrlResMsg.dataLength > 0) && (groupCount < *number_of_channels))
      {
        memcpy(&mgmdGroupsRes, ctrlResMsg.data + groupCount*sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t), sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));

        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  Entry [%u]", mgmdGroupsRes.entryId);
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Type:           %s",   mgmdGroupsRes.groupType==PTIN_MGMD_CTRL_GROUPTYPE_DYNAMIC? ("Dynamic"):("Static"));
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Filter-Mode:    %s",   mgmdGroupsRes.filterMode==PTIN_MGMD_CTRL_FILTERMODE_INCLUDE? ("Include"):("Exclude"));
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Group Timer:    %u",   mgmdGroupsRes.groupTimer);
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Groups Address: %08X", mgmdGroupsRes.groupIP);
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Source Timer:   %u",   mgmdGroupsRes.sourceTimer);
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Source Address: %08X", mgmdGroupsRes.sourceIP);

        inetAddressSet(L7_AF_INET, &mgmdGroupsRes.groupIP, &channel_list[groupCount].groupAddr);
        inetAddressSet(L7_AF_INET, &mgmdGroupsRes.sourceIP, &channel_list[groupCount].sourceAddr);
        channel_list[groupCount].static_type = mgmdGroupsRes.groupType;

        ctrlResMsg.dataLength -= sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t);
        ++groupCount;
      }

      *number_of_channels = groupCount;
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid response size from MGMD [size:%u]", ctrlResMsg.dataLength);
      return L7_FAILURE;
    }
  }

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
//static L7_uint16 clientList_size=0;
//static ptin_client_id_t clientList[PTIN_SYSTEM_IGMP_MAXONUS];

L7_RC_t ptin_igmp_clientList_get(L7_uint32 McastEvcId, L7_in_addr_t *ipv4_channel,
                                 L7_uint16 client_index, L7_uint16 *number_of_clients, ptin_client_id_t *client_list,
                                 L7_uint16 *total_clients)
{
  L7_uint                                igmp_idx;
  PTIN_MGMD_EVENT_t                      reqMsg          = {0};
  PTIN_MGMD_EVENT_t                      resMsg          = {0};
  PTIN_MGMD_EVENT_CTRL_t                 ctrlResMsg      = {0};
  PTIN_MGMD_CTRL_GROUPCLIENTS_REQUEST_t  mgmdGroupsMsg   = {0}; 
  L7_uint32                              currentClientId = 0;
  L7_uint32                              clientBufferIdx = 0;
  ptin_client_id_t                       avl_key;
  ptinIgmpClientGroupsSnapshotInfoData_t *avl_infoData;
  L7_uint32                              totalClientCount = 0; 
  
  /* Validate arguments */
  if (client_list==L7_NULLPTR || number_of_clients==L7_NULLPTR || total_clients==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Null parameters");
    return L7_FAILURE;
  }

  /* Get IGMP instance index */
  if (ptin_igmp_instance_find_fromMcastEvcId(McastEvcId, &igmp_idx)!=L7_SUCCESS)
  {
    *number_of_clients=0;
    LOG_ERR(LOG_CTX_PTIN_IGMP,"There is no IGMP instance with MC EVC id %u",McastEvcId);
    return L7_NOT_EXIST;
  }

  /* Clean the current clientGroup snapshot */
  if(L7_SUCCESS != ptin_igmp_clientGroupSnapshot_clean())
  {
    *number_of_clients=0;
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to clean the current clientGroup snapshot");
    return L7_FAILURE;
  }

  /* If the entry index is 0, request the client list from MGMD. Otherwise, read from our local snapshot */
  if(0 == client_index)
  {
    L7_uint32 maxResponseEntries = PTIN_MGMD_EVENT_CTRL_DATA_SIZE_MAX/sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t);
    L7_uint32 pageClientCount = 0; 

    do
    {
      //Save current page and total client context
      totalClientCount += pageClientCount;
      pageClientCount   = 0; 

      mgmdGroupsMsg.serviceId = McastEvcId;
      mgmdGroupsMsg.groupIP   = ipv4_channel->s_addr;
      mgmdGroupsMsg.sourceIP  = 0x00000000;
      mgmdGroupsMsg.entryId   = (totalClientCount==0)?(PTIN_MGMD_CTRL_GROUPCLIENTS_FIRST_ENTRY):(totalClientCount);
      ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_GROUP_CLIENTS_GET, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdGroupsMsg, sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_REQUEST_t));
      ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
      ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code: %08X",      ctrlResMsg.msgCode);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id  : %08X",      ctrlResMsg.msgId);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Res     : %u",        ctrlResMsg.res);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Length  : %u (%.1f)", ctrlResMsg.dataLength, ((double)ctrlResMsg.dataLength)/sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t));

      if (0 == ctrlResMsg.dataLength%sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t))
      {
        PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t mgmdGroupsRes = {0};

        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Active groups (Service:%u GroupAddr:%08X)", McastEvcId, ipv4_channel->s_addr);
        while(ctrlResMsg.dataLength > 0)
        {
          ptin_client_id_t               newClientEntry;    
          ptinIgmpClientInfoData_t      *client;
          ptinIgmpClientGroupInfoData_t *clientGroup;
          L7_uint32                      ptinPort;

          memcpy(&mgmdGroupsRes, ctrlResMsg.data + pageClientCount*sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t), sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t));

          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  Entry [%u]",   mgmdGroupsRes.entryId);
          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Port:   %u", mgmdGroupsRes.portId);
          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Client: %u", mgmdGroupsRes.clientId);

          if (ptin_intf_intIfNum2port(mgmdGroupsRes.portId,&ptinPort) != L7_SUCCESS)
          {
            *number_of_clients=0;
            LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to convert intIfNum [%u] to ptinPort",mgmdGroupsRes.portId);
            return L7_FAILURE;
          }
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "Converted   intIfNum [%u] to ptinPort [%u]", mgmdGroupsRes.portId,ptinPort);
          
          /* Save entry in the clientGroup snapshot avlTree */
          if(L7_NULLPTR == (client = igmpClients_unified.client_devices[ptinPort][mgmdGroupsRes.clientId].client))
          {
            *number_of_clients=0;
            LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client returned from MGMD");
            return L7_FAILURE;
          }
          if(L7_NULLPTR == (clientGroup = client->pClientGroup))
          {
            *number_of_clients=0;
            LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client returned from MGMD");
            return L7_FAILURE;
          }
          #if (MC_CLIENT_INTERF_SUPPORTED)
                if (ptin_intf_port2ptintf(clientGroup->ptin_port, &newClientEntry.ptin_intf)!=L7_SUCCESS)
                {
                  *number_of_clients=0;
                  LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to Convert intfNum[%u]", clientGroup->ptin_port);
                  return L7_FAILURE;
                }
                newClientEntry.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
                LOG_INFO(LOG_CTX_PTIN_IGMP, "Port: %u", clientGroup->ptin_port);
          #endif
          #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                newClientEntry.outerVlan = clientGroup->igmpClientDataKey.outerVlan;
                newClientEntry.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
                LOG_INFO(LOG_CTX_PTIN_IGMP, "oVlan: %u", newClientEntry.outerVlan);
          #endif
          #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                newClientEntry.innerVlan = clientGroup->igmpClientDataKey.innerVlan;
                newClientEntry.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
                LOG_INFO(LOG_CTX_PTIN_IGMP, "iVlan: %u", newClientEntry.innerVlan);
          #endif
          #if 0
          #if (MC_CLIENT_IPADDR_SUPPORTED)
                newClientEntry.ipv4_addr = clientGroup->ipv4_addr;
                newClientEntry.mask |= PTIN_CLIENT_MASK_FIELD_IPADDR;
          #endif
          #if (MC_CLIENT_MACADDR_SUPPORTED)
                memcpy(newClientEntry.macAddr, clientGroup->macAddr, sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
                newClientEntry.mask |= PTIN_CLIENT_MASK_FIELD_MACADDR;
          #endif
          #endif
          if(L7_SUCCESS != ptin_igmp_clientGroupSnapshot_add(&newClientEntry))
          {
            *number_of_clients=0;
            LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to add this clientIdx[%u] port portId[%u] to the clientGroupSnapshot avlTree", mgmdGroupsRes.clientId, mgmdGroupsRes.portId);
            return L7_FAILURE;
          }

          ctrlResMsg.dataLength -= sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t);
          ++totalClientCount;
          ++pageClientCount;
        }
      }
    } while(pageClientCount == maxResponseEntries); //While the number of clients returned equals the max number of clients per page
  }

  /* Get all clients */
  memset(&avl_key,0x00,sizeof(ptin_client_id_t));
  while (L7_NULLPTR != (avl_infoData = (ptinIgmpClientGroupsSnapshotInfoData_t *)avlSearchLVL7(&(igmpSnapshotClientGroups.avlTree), &avl_key, L7_MATCH_GETNEXT)))
  {
    ptin_client_id_t tempKey;

    /* Prepare next key */
    memcpy(&avl_key, &avl_infoData->key, sizeof(ptin_client_id_t));

    /* Copy the key data to a temporary buffer. This is necessary so we don't change the AVLTree when restoring the clientData */
    memcpy(&tempKey, &avl_infoData->key, sizeof(ptin_client_id_t));

    /* Ignore this entry if it's not in use */
    if(avl_infoData->in_use != L7_TRUE)
    {
      continue;
    }

    /* If we still haven't reached the desired clientID, continue */
    if(currentClientId < client_index)
    {
      ++currentClientId;
      continue;
    }

    /* Copy client contents */
    if(L7_SUCCESS != ptin_igmp_clientId_restore(&tempKey))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to convert client[mask:%02X intf:%u/%u oVlan:%u iVlan:%u]", avl_infoData->key.mask, avl_infoData->key.ptin_intf.intf_type, avl_infoData->key.ptin_intf.intf_id,
                                                                                                     avl_infoData->key.outerVlan, avl_infoData->key.innerVlan);
      return L7_FAILURE;
    }
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "      Idx:   %u",      clientBufferIdx);
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "        Mask:  %02X",  tempKey.mask);
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "        Intf:  %u/%u", tempKey.ptin_intf.intf_type, tempKey.ptin_intf.intf_id);
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "        oVlan: %u",    tempKey.outerVlan);
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "        iVlan: %u",    tempKey.innerVlan);
    memcpy(&client_list[clientBufferIdx], &tempKey, sizeof(tempKey));

    /* Increase the ID of the read clientGroup */
    ++clientBufferIdx;
    ++currentClientId;
  }

  *total_clients     = totalClientCount;
  *number_of_clients = clientBufferIdx;
    
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
L7_RC_t ptin_igmp_static_channel_add(PTIN_MGMD_CTRL_STATICGROUP_t* channel)
{
  PTIN_MGMD_EVENT_t      inEventMsg = {0}, outEventMsg = {0};
  PTIN_MGMD_EVENT_CTRL_t ctrlResMsg = {0};

  /* Create and send a PTIN_MGMD_EVENT_CTRL_STATIC_GROUP_ADD event to MGMD */
  ptin_mgmd_event_ctrl_create(&inEventMsg, PTIN_MGMD_EVENT_CTRL_STATIC_GROUP_ADD, rand(), 0, ptinMgmdTxQueueId, (void*) channel, (uint32) sizeof(PTIN_MGMD_CTRL_STATICGROUP_t));
  ptin_mgmd_sendCtrlEvent(&inEventMsg, &outEventMsg);

  /* Parse the received reply */
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "MGMD replied");
  ptin_mgmd_event_ctrl_parse(&outEventMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Res     : %u",   ctrlResMsg.res);

  return ctrlResMsg.res;
}

/**
 * Remove an existent channel
 * 
 * @param McastEvcId   : Multicast EVC id
 * @param ipv4_channel : Channel IP
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_channel_remove(PTIN_MGMD_CTRL_STATICGROUP_t* channel)
{
  PTIN_MGMD_EVENT_t      inEventMsg = {0}, outEventMsg = {0};
  PTIN_MGMD_EVENT_CTRL_t ctrlResMsg = {0};

  /* Create and send a PTIN_MGMD_EVENT_CTRL_STATIC_GROUP_REMOVE event to MGMD */
  ptin_mgmd_event_ctrl_create(&inEventMsg, PTIN_MGMD_EVENT_CTRL_STATIC_GROUP_REMOVE, rand(), 0, ptinMgmdTxQueueId, (void*) channel, (uint32) sizeof(PTIN_MGMD_CTRL_STATICGROUP_t));
  ptin_mgmd_sendCtrlEvent(&inEventMsg, &outEventMsg);

  /* Parse the received reply */
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "MGMD replied");
  ptin_mgmd_event_ctrl_parse(&outEventMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Res     : %u",   ctrlResMsg.res);

  return ctrlResMsg.res;
}


/******************************************************** 
 * FOR FASTPATH INTERNAL MODULES USAGE
 ********************************************************/

/**
 * Get external vlans
 * 
 * @param intIfNum 
 * @param intOVlan 
 * @param intIVlan 
 * @param client_idx 
 * @param uni_ovid : External Outer Vlan
 * @param uni_ivid : External Inner Vlan
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_igmp_extVlans_get(L7_uint32 intIfNum, L7_uint16 intOVlan, L7_uint16 intIVlan,
                               L7_int client_idx, L7_uint16 *uni_ovid, L7_uint16 *uni_ivid)
{
  L7_uint32 flags;
  L7_uint16 ovid, ivid;
  ptinIgmpClientInfoData_t *clientInfo;
  L7_uint   ptin_port;

  /* Get ptin_port value */
  ptin_port = 0;
  if (ptin_intf_intIfNum2port(intIfNum, &ptin_port) != L7_SUCCESS || 
      ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting ptin_port for intIfNum %u", intIfNum);
    return L7_FAILURE;
  }

  ovid = ivid = 0;
  /* If client is provided, go directly to client info */
  if (ptin_igmp_clientIntfVlan_validate(intIfNum, intOVlan) == L7_SUCCESS &&
      (client_idx >= 0 && client_idx < PTIN_IGMP_CLIENTIDX_MAX))
  {
    /* Get pointer to client structure in AVL tree */
    clientInfo = igmpClients_unified.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;

    ovid = clientInfo->uni_ovid;
    ivid = clientInfo->uni_ivid;

    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Caught vlans: %u+%u", ovid, ivid);
  }

  /* If no data was retrieved, goto EVC info */
  if (ovid == 0)
  {
    if (ptin_evc_extVlans_get_fromIntVlan(intIfNum, intOVlan, intIVlan, &ovid, &ivid) != L7_SUCCESS)
    {
      ovid = intOVlan;
      ivid = intIVlan;
    }
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Vlans obtained from EVC: %u+%u", ovid, ivid);
  }

  /* For packets sent to root ports, belonging to unstacked EVCs, remove inner vlan */
  if (ptin_evc_flags_get_fromIntVlan(intOVlan, &flags, L7_NULLPTR) == L7_SUCCESS &&
      !(flags & PTIN_EVC_MASK_QUATTRO) &&
      !(flags & PTIN_EVC_MASK_STACKED))
  {
    ivid = 0;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Vlans to retrieve: %u+%u", ovid, ivid);

  /* Return vlans */
  if (uni_ovid != L7_NULLPTR)  *uni_ovid = ovid;
  if (uni_ivid != L7_NULLPTR)  *uni_ivid = ivid;

  return L7_SUCCESS;
}

#if 0
/**
 * Get next IGMP client
 * 
 * @param intIfNum : interface
 * @param intVlan  : internal vlan
 * @param inner_vlan : inner vlan used as base reference 
 *                   (processed and returned next inner vlan)
 * @param inner_vlan_next : next inner vlan
 * @param uni_ovid : external outer vid
 * @param uni_ivid : external inner vid
 * 
 * @return L7_RC_t : 
 *  L7_SUCCESS tells a next client was returned
 *  L7_NO_VALUE tells there is no more clients
 *  L7_FAILURE in case of error
 */
L7_RC_t ptin_igmp_client_next(L7_uint32 intIfNum, L7_uint16 intVlan, L7_uint16 inner_vlan,
                              L7_uint16 *inner_vlan_next, L7_uint16 *uni_ovid, L7_uint16 *uni_ivid)
{
  L7_uint next, evc_type;
  L7_RC_t rc;

  /* Get evc index, of this internal vlan */
  if (ptin_evc_check_evctype_fromIntVlan(intVlan, &evc_type) != L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid internal vlan %u", intVlan);
    return L7_FAILURE;
  }

  /* For standard EVC types, use old scheme */
  if (evc_type == PTIN_EVC_TYPE_STD_P2MP || evc_type == PTIN_EVC_TYPE_STD_P2MP)
  {
    if ((rc=ptin_evc_vlan_client_next(intVlan, intIfNum, inner_vlan, &next, L7_NULLPTR)) != L7_SUCCESS)
      return rc;

    if (uni_ovid != L7_SUCCESS)  *uni_ovid = intVlan;
    if (uni_ivid != L7_SUCCESS)  *uni_ivid = next;
    if (inner_vlan_next != L7_NULLPTR)  *inner_vlan_next = next;

    return L7_SUCCESS;
  }

  /* QUATTRO scheme, require MC_CLIENT_INTERF_SUPPORTED, MC_CLIENT_OUTERVLAN_SUPPORTED and MC_CLIENT_INNERVLAN_SUPPORTED */
  #if (MC_CLIENT_INTERF_SUPPORTED && MC_CLIENT_OUTERVLAN_SUPPORTED && MC_CLIENT_INNERVLAN_SUPPORTED)

  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientsAvlTree_t *avl_tree;
  ptinIgmpClientInfoData_t *clientInfo;
  L7_uint32 ptin_port = 0;

  /* Get ptin_port value */
  ptin_port = 0;
  /* Convert to ptin_port format */
  if (ptin_intf_intIfNum2port(intIfNum, &ptin_port)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot convert client_ref intIfNum to ptin_port format", intIfNum);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* Key to search for */
  avl_tree = &igmpClients_unified.avlTree;
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));

  /* First client */
  if (inner_vlan != 0)
  {
    avl_key.ptin_port = ptin_port;
    avl_key.outerVlan = intVlan;
    avl_key.innerVlan = inner_vlan;

    /* Search for a client with these inputs */
    clientInfo = avlSearchLVL7( &(avl_tree->igmpClientsAvlTree), (void *)&avl_key, AVL_NEXT);
  }
  else
  {
    /* Search for the first client with this ptin_port and internal vlan */
    do
    {
      clientInfo = avlSearchLVL7( &(avl_tree->igmpClientsAvlTree), (void *)&avl_key, AVL_NEXT);
      if (clientInfo == L7_NULLPTR)  break;
      /* Prepare next key */
      memcpy(&avl_key, &clientInfo->igmpClientDataKey, sizeof(ptinIgmpClientDataKey_t));
    } while ( clientInfo->igmpClientDataKey.ptin_port != ptin_port ||
              clientInfo->igmpClientDataKey.outerVlan != intVlan );
  }

  /* No client found? */
  if (clientInfo == L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No more clients found!");
    return L7_NO_VALUE;
  }

  /* Return vlans */
  if (uni_ovid != L7_NULLPTR)         *uni_ovid   = clientInfo->uni_ovid;
  if (uni_ivid != L7_NULLPTR)         *uni_ivid   = clientInfo->uni_ivid;
  if (inner_vlan_next != L7_NULLPTR)  *inner_vlan_next = clientInfo->igmpClientDataKey.innerVlan;

  return L7_SUCCESS;

  #else
  return L7_FAILURE;
  #endif
}
#endif

/**
 * Build client id structure
 * 
 * @param intIfNum  : interface
 * @param intVlan   : internal outer vlan
 * @param innerVlan : inner vlan
 * @param smac      : source MAC address
 * @param client    : client id pointer (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_igmp_clientId_build(L7_uint32 intIfNum,
                                 L7_uint16 intVlan, L7_uint16 innerVlan,
                                 L7_uchar8 *smac,
                                 ptin_client_id_t *client)
{
  ptin_intf_t ptin_intf;

  /* Validate clientid */
  if (client==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client pointer");
    return L7_FAILURE;
  }

  /* Clear client structure */
  memset(client, 0x00, sizeof(ptin_client_id_t));

  /* Interface reference */
  #if (MC_CLIENT_INTERF_SUPPORTED)
  /* Validate intIfNum */
  if (intIfNum>0 && intIfNum<L7_ALL_INTERFACES)
  {
    if (ptin_intf_intIfNum2ptintf(intIfNum, &ptin_intf)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error converting intIfNum %u to ptin_intf format",intIfNum);
      return L7_FAILURE;
    }
    client->ptin_intf.intf_type = ptin_intf.intf_type;
    client->ptin_intf.intf_id   = ptin_intf.intf_id;
    client->mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  }
  #endif

  /* Outer vlan reference */
  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
  /* Validate outer vlan */
  if (intVlan>=PTIN_VLAN_MIN && intVlan<=PTIN_VLAN_MAX)
  {
    client->outerVlan = intVlan;
    client->mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  }
  #endif

  /* Inner vlan reference */
  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
  /* Validate outer vlan */
  if (innerVlan>0 && innerVlan<=4095)
  {
    client->innerVlan = innerVlan;
    client->mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  }
  #endif

  /* MAC reference */
  #if (MC_CLIENT_IPADDR_SUPPORTED)
  #error "IP address to identify clients, not supported!"
  #endif

  /* MAC reference */
  #if (MC_CLIENT_MACADDR_SUPPORTED)
  if (smac!=L7_NULLPTR)
  {
    memcpy(client->macAddr, smac, sizeof(client->macAddr));
    client->mask |= PTIN_CLIENT_MASK_FIELD_MACADDR;
  }
  #endif

  return L7_SUCCESS;
}

/**
 * Get the client index associated to a Multicast client 
 * 
 * @param intIfNum      : interface number
 * @param intVlan       : internal vlan
 * @param innerVlan     : inner vlan
 * @param client_index  : Client index to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_clientIndex_get(L7_uint32 intIfNum,
                                  L7_uint16 intVlan, L7_uint16 innerVlan,
                                  L7_uchar8 *smac,
                                  L7_uint *client_index)
{
  L7_uint     client_idx;
  ptin_client_id_t client;
  ptinIgmpClientInfoData_t *clientInfo;

  /* Build client structure */
  if (ptin_igmp_clientId_build(intIfNum, intVlan, innerVlan, smac, &client)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get client */
  if (ptin_igmp_client_find(&client, &clientInfo)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,
              "Error searching for client {mask=0x%02x,"
              "port=%u/%u,"
              "svlan=%u,"
              "cvlan=%u,"
              "ipAddr=%u.%u.%u.%u,"
              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} ",
              client.mask,
              client.ptin_intf.intf_type, client.ptin_intf.intf_id,
              client.outerVlan,
              client.innerVlan,
              (client.ipv4_addr>>24) & 0xff, (client.ipv4_addr>>16) & 0xff, (client.ipv4_addr>>8) & 0xff, client.ipv4_addr & 0xff,
              client.macAddr[0],client.macAddr[1],client.macAddr[2],client.macAddr[3],client.macAddr[4],client.macAddr[5]);
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
 * @param intIfNum      : Interface number 
 * @param client_index  : Client index
 * @param isDynamic     : client type (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_client_type(L7_uint32 intIfNum,
                              L7_uint client_idx,
                              L7_BOOL *isDynamic)
{
  L7_uint32 ptin_port;
  ptinIgmpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if (client_idx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client_idx %u", client_idx);
    return L7_FAILURE;
  }
  /* Validate interface */
  if (ptin_intf_intIfNum2port(intIfNum, &ptin_port) != L7_SUCCESS ||
      ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error converting intIfNum %u to ptin_port format", intIfNum);
    return L7_FAILURE;
  }

  /* Get pointer to client structure in AVL tree */
  clientInfo = igmpClients_unified.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;

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
 * @param intIfNum    : Interface Number
 * @param client_idx  : client index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_client_timer_start(L7_uint32 intIfNum, L7_uint32 client_idx)
{
#ifdef CLIENT_TIMERS_SUPPORTED
  L7_uint32 ptin_port;
  L7_RC_t   rc;

  /* Validate arguments */
  if (client_idx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client_idx %u", client_idx);
    return L7_FAILURE;
  }
  /* Validate interface */
  if (ptin_intf_intIfNum2port(intIfNum, &ptin_port) != L7_SUCCESS ||
      ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error converting intIfNum %u to ptin_port format", intIfNum);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* Add client */
  rc = ptin_igmp_timer_start(ptin_port, client_idx);

  osapiSemaGive(ptin_igmp_clients_sem);

  if (rc!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error (re)starting timer for this client (ptin_port=%u client_idx=%u)", ptin_port, client_idx);
  }

  return rc;
#else
  return L7_SUCCESS;
#endif
}

/**
 * Add a new Multicast client group
 * 
 * @param client      : client group identification parameters 
 * @param intVid      : Internal vlan
 * @param uni_ovid    : External Outer vlan 
 * @param uni_ivid    : External Inner vlan 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_clientGroup_add(ptin_client_id_t *client, L7_uint16 uni_ovid, L7_uint16 uni_ivid)
{
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientGroupAvlTree_t *avl_tree;
  ptinIgmpClientGroupInfoData_t *avl_infoData;
  L7_uint32 ptin_port;

  /* Get ptin_port value */
  ptin_port = 0;
  #if (MC_CLIENT_INTERF_SUPPORTED)
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    /* Convert to ptin_port format */
    if (ptin_intf_ptintf2port(&client->ptin_intf, &ptin_port) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot convert client intf %u/%u to ptin_port format",
              client->ptin_intf.intf_type, client->ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
  #endif

  /* Check if this key already exists */
  avl_tree = &igmpClientGroups.avlTree;
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
                                "} will be added"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
  }

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* Check if this key already exists */
  if ((avl_infoData=avlSearchLVL7( &(avl_tree->igmpClientsAvlTree), (void *)&avl_key, AVL_EXACT)) == L7_NULLPTR)
  {
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
                                "}"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
      osapiSemaGive(ptin_igmp_clients_sem);
      return L7_FAILURE;
    }

    /* Find the inserted entry */
    if ((avl_infoData=(ptinIgmpClientGroupInfoData_t *) avlSearchLVL7(&(avl_tree->igmpClientsAvlTree),(void *)&avl_key, AVL_EXACT))==L7_NULLPTR)
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
                                "}"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
      osapiSemaGive(ptin_igmp_clients_sem);
      return L7_FAILURE;
    }

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
                                  "}"
                #if (MC_CLIENT_INTERF_SUPPORTED)
                ,avl_key.ptin_port
                #endif
                #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                ,avl_key.outerVlan
                #endif
                #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                ,avl_key.innerVlan
                #endif
                #if (MC_CLIENT_IPADDR_SUPPORTED)
                ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
                #endif
                #if (MC_CLIENT_MACADDR_SUPPORTED)
                ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
                #endif
               );
    }

    /* Save associated vlans */
    avl_infoData->uni_ovid = uni_ovid;
    avl_infoData->uni_ivid = uni_ivid;


    /* Clear list of device clients */
    avl_infoData->ptin_port = ptin_port;
    memset(avl_infoData->client_bmp_list, 0x00, sizeof(avl_infoData->client_bmp_list));

    /* Initialize client devices queue */
    dl_queue_init(&avl_infoData->queue_clientDevices);

    /* Clear igmp statistics */
    osapiSemaTake(ptin_igmp_stats_sem,L7_WAIT_FOREVER);
    memset(&avl_infoData->stats_client,0x00,sizeof(ptin_IGMP_Statistics_t));
    osapiSemaGive(ptin_igmp_stats_sem);

    /* Update global data (one more group of clients) */
    igmpClientGroups.number_of_clients++;
  }
  /* ClientGroup already present */
  else
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
                                    "} already exists"
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                  ,avl_key.ptin_port
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                  ,avl_key.outerVlan
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                  ,avl_key.innerVlan
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                  ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                  ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
                  #endif
                 );
    }
  }

  osapiSemaGive(ptin_igmp_clients_sem);

  return L7_SUCCESS;
}

/**
 * Add a new Multicast client group
 * 
 * @param client      : client group identification parameters 
 * @param intVid      : Internal vlan
 * @param uni_ovid    : External Outer vlan 
 * @param uni_ivid    : External Inner vlan 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_clientGroupSnapshot_add(ptin_client_id_t *client)
{
  ptin_client_id_t                       avl_key;
  ptinIgmpClientGroupsSnapshotAvlTree_t  *avl_tree;
  ptinIgmpClientGroupsSnapshotInfoData_t *avl_infoData;

  /* Check if this key already exists */
  avl_tree = &igmpSnapshotClientGroups;
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  #if (MC_CLIENT_INTERF_SUPPORTED)
  avl_key.ptin_intf.intf_id   = client->ptin_intf.intf_id;
  avl_key.ptin_intf.intf_type = client->ptin_intf.intf_type;
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
  avl_key.mask = client->mask;

  if (ptin_debug_igmp_snooping)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Key {"
              #if (MC_CLIENT_INTERF_SUPPORTED)
                                "port=%u/%u,"
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
                                "} will be added"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_intf.intf_type,avl_key.ptin_intf.intf_id
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
  }

  osapiSemaTake(ptin_igmp_clients_snapshot_sem, L7_WAIT_FOREVER);

  /* Check if this key already exists */
  if ((avl_infoData=avlSearchLVL7( &(avl_tree->avlTree), (void *)&avl_key, AVL_EXACT)) == L7_NULLPTR)
  {
    /* Insert entry in AVL tree */
    if (avlInsertEntry(&(avl_tree->avlTree), (void *)&avl_key)!=L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error inserting key {"
              #if (MC_CLIENT_INTERF_SUPPORTED)
                                "port=%u/%u,"
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
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_intf.intf_type,avl_key.ptin_intf.intf_id
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
      osapiSemaGive(ptin_igmp_clients_snapshot_sem);
      return L7_FAILURE;
    }

    /* Find the inserted entry */
    if ((avl_infoData=(ptinIgmpClientGroupsSnapshotInfoData_t *) avlSearchLVL7(&(avl_tree->avlTree),(void *)&avl_key, AVL_EXACT))==L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot find key {"
              #if (MC_CLIENT_INTERF_SUPPORTED)
                                "port=%u/%u,"
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
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_intf.intf_type,avl_key.ptin_intf.intf_id
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
      osapiSemaGive(ptin_igmp_clients_snapshot_sem);
      return L7_FAILURE;
    }

    if (ptin_debug_igmp_snooping)
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success inserting Key {"
                #if (MC_CLIENT_INTERF_SUPPORTED)
                                  "port=%u/%u,"
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
                #if (MC_CLIENT_INTERF_SUPPORTED)
                ,avl_key.ptin_intf.intf_type,avl_key.ptin_intf.intf_id
                #endif
                #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                ,avl_key.outerVlan
                #endif
                #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                ,avl_key.innerVlan
                #endif
                #if (MC_CLIENT_IPADDR_SUPPORTED)
                ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
                #endif
                #if (MC_CLIENT_MACADDR_SUPPORTED)
                ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
                #endif
               );
    }
  }
  /* ClientGroup already present */
  else
  {
    if (ptin_debug_igmp_snooping)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"This key {"
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                                    "port=%u/%u,"
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
                                    "} already exists"
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                  ,avl_key.ptin_intf.intf_type,avl_key.ptin_intf.intf_id
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                  ,avl_key.outerVlan
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                  ,avl_key.innerVlan
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                  ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                  ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
                  #endif
                 );
    }
  }

  avl_infoData->in_use = L7_TRUE;

  osapiSemaGive(ptin_igmp_clients_snapshot_sem);

  return L7_SUCCESS;
}

/* Remove child clients belonging to a client group */
static L7_RC_t ptin_igmp_clean_deviceClients(ptinIgmpClientGroupInfoData_t *avl_infoData_clientGroup);

/**
 * Remove a Multicast client group
 * 
 * @param client      : client group identification parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_clientGroup_remove(ptin_client_id_t *client)
{
  ptinIgmpClientDataKey_t   avl_key;
  ptinIgmpClientGroupAvlTree_t *avl_tree;
  ptinIgmpClientGroupInfoData_t *avl_infoData;
  L7_uint32 ptin_port;

  /* Get ptin_port value */
  ptin_port = 0;
  #if (MC_CLIENT_INTERF_SUPPORTED)
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    /* Convert to ptin_port format */
    if (ptin_intf_ptintf2port(&client->ptin_intf,&ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot convert client intf %u/%u to ptin_port format",
              client->ptin_intf.intf_type,client->ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
  #endif

  /* Check if this key does not exists */

  avl_tree = &igmpClientGroups.avlTree;
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
                                "}"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
  }

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* Check if this entry does not exist in AVL tree */
  if ((avl_infoData=(ptinIgmpClientGroupInfoData_t *) avlSearchLVL7( &(avl_tree->igmpClientsAvlTree), (void *)&avl_key, AVL_EXACT))==L7_NULLPTR)
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
                                    "} does not exist"
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                  ,avl_key.ptin_port
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                  ,avl_key.outerVlan
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                  ,avl_key.innerVlan
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                  ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                  ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
                  #endif
                 );
    }
    osapiSemaGive(ptin_igmp_clients_sem);
    return L7_NOT_EXIST;
  }

  /* Remove all child clients, belonging to this client group */
  if (ptin_igmp_clean_deviceClients(avl_infoData) != L7_SUCCESS)
  {
    osapiSemaGive(ptin_igmp_clients_sem);
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not remove child clients!");
    return L7_FAILURE;
  }

  /* Finally remove the client group */
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
                              "}"
            #if (MC_CLIENT_INTERF_SUPPORTED)
            ,avl_key.ptin_port
            #endif
            #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
            ,avl_key.outerVlan
            #endif
            #if (MC_CLIENT_INNERVLAN_SUPPORTED)
            ,avl_key.innerVlan
            #endif
            #if (MC_CLIENT_IPADDR_SUPPORTED)
            ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
            #endif
            #if (MC_CLIENT_MACADDR_SUPPORTED)
            ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
            #endif
           );
    osapiSemaGive(ptin_igmp_clients_sem);
    return L7_FAILURE;
  }

  /* Update global data */
  if (igmpClientGroups.number_of_clients>0)
    igmpClientGroups.number_of_clients--;

  osapiSemaGive(ptin_igmp_clients_sem);

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
                                "}"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
  }

  return L7_SUCCESS;
}


/**
 * Remove all Multicast client groups
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_clientGroup_clean(void)
{
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientGroupAvlTree_t *avl_tree;
  ptinIgmpClientGroupInfoData_t *avl_infoData;
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* AVL tree refrence */
  avl_tree = &igmpClientGroups.avlTree;

  /* Get all clients */
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  while ( (avl_infoData=(ptinIgmpClientGroupInfoData_t *) avlSearchLVL7(&(avl_tree->igmpClientsAvlTree), &avl_key, L7_MATCH_GETNEXT))!=L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_infoData->igmpClientDataKey, sizeof(ptinIgmpClientDataKey_t));

    /* Remove all child clients, belonging to this client group */
    if (ptin_igmp_clean_deviceClients(avl_infoData) != L7_SUCCESS)
    {
      osapiSemaGive(ptin_igmp_clients_sem);
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not remove child clients!");
      return L7_FAILURE;
    }

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
                                "}"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
        rc = L7_FAILURE;
    }
    else
    {
      /* Update global data */
      if (igmpClientGroups.number_of_clients>0)
        igmpClientGroups.number_of_clients--;

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
                                    "}"
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                  ,avl_key.ptin_port
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                  ,avl_key.outerVlan
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                  ,avl_key.innerVlan
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                  ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                  ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
                  #endif
                 );
      }
    }
  }

  /* If everything went well... */
  if (rc == L7_SUCCESS)
  {
    igmpClientGroups.number_of_clients = 0;
  }

  osapiSemaGive(ptin_igmp_clients_sem);

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
 * Remove all Multicast client groups
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_clientGroupSnapshot_clean(void)
{
  ptin_client_id_t                       avl_key;
  ptinIgmpClientGroupsSnapshotAvlTree_t  *avl_tree;
  ptinIgmpClientGroupsSnapshotInfoData_t *avl_infoData;
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(ptin_igmp_clients_snapshot_sem, L7_WAIT_FOREVER);

  /* AVL tree refrence */
  avl_tree = &igmpSnapshotClientGroups;

  /* Get all clients */
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  while ( (avl_infoData=(ptinIgmpClientGroupsSnapshotInfoData_t *) avlSearchLVL7(&(avl_tree->avlTree), &avl_key, L7_MATCH_GETNEXT))!=L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_infoData->key, sizeof(ptin_client_id_t));

    avl_infoData->in_use = L7_FALSE;

    /* Remove this entry */
    if (avlDeleteEntry(&(avl_tree->avlTree), (void *)&avl_key)==L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing key {"
              #if (MC_CLIENT_INTERF_SUPPORTED)
                                "port=%u/%u,"
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
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_intf.intf_type,avl_key.ptin_intf.intf_id
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
        rc = L7_FAILURE;
    }
    else
    {
      if (ptin_debug_igmp_snooping)
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success removing Key {"
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                                    "port=%u/%u,"
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
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                  ,avl_key.ptin_intf.intf_type,avl_key.ptin_intf.intf_id
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                  ,avl_key.outerVlan
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                  ,avl_key.innerVlan
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                  ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                  ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
                  #endif
                 );
      }
    }
  }

  osapiSemaGive(ptin_igmp_clients_snapshot_sem);

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
 * Add a dynamic client
 *  
 * @param intIfNum    : interface number  
 * @param intVlan     : Internal vlan
 * @param innerVlan   : Inner vlan
 * @param client_idx_ret : client index (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_dynamic_client_add(L7_uint32 intIfNum,
                                     L7_uint16 intVlan, L7_uint16 innerVlan,
                                     L7_uchar8 *smac,
                                     L7_uint *client_idx_ret)
{
  ptin_client_id_t client;
  L7_uint16 uni_ovid=0, uni_ivid=0;
  L7_RC_t   rc;

  /* Build client structure */
  if (ptin_igmp_clientId_build(intIfNum, intVlan, innerVlan, smac, &client) != L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* If uni vlans are not provided, but interface is, get uni vlans from EVC data */
  if (intIfNum > 0 && intVlan > 0 && innerVlan > 0)
  {
    if (ptin_evc_extVlans_get_fromIntVlan(intIfNum, intVlan, innerVlan, &uni_ovid, &uni_ivid) == L7_SUCCESS)
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Ext vlans for intIfNum %u, cvlan %u: uni_ovid=%u, uni_ivid=%u",
                intIfNum, innerVlan, uni_ovid, uni_ivid);
    }
    else
    {
      uni_ovid = uni_ivid = 0;
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot get ext vlans for intIfNum %u, cvlan %u", intIfNum, innerVlan);
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to create new client: intIfNum %u, intVlan %u, innerVlan %u",
            intIfNum, intIfNum, innerVlan);

  /* Add client */
  rc = ptin_igmp_new_client(&client, uni_ovid, uni_ivid, L7_TRUE, client_idx_ret);

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"New client created: intIfNum %u, intVlan %u, innerVlan %u",
            intIfNum, intIfNum, innerVlan);

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
 * @param intIfNum      : Interface Number
 * @param client_index  : Client index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_dynamic_client_flush(L7_uint32 intIfNum, L7_uint client_idx)
{
  L7_uint32 ptin_port;

  /* Validate arguments */
  if (client_idx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    //if (ptin_debug_igmp_snooping)
    //  LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }
  /* Validate interface */
  if (ptin_intf_intIfNum2port(intIfNum, &ptin_port) != L7_SUCCESS ||
      ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
//  if (ptin_debug_igmp_snooping)
//    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error converting intIfNum %u to ptin_port format", intIfNum);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  if (ptin_igmp_rm_clientIdx(ptin_port, client_idx, L7_FALSE, L7_FALSE)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Error flushing dynamic client (ptin_port=%u client_idx=%u)", ptin_port, client_idx);
    osapiSemaGive(ptin_igmp_clients_sem);
    return L7_FAILURE;
  }

  osapiSemaGive(ptin_igmp_clients_sem);

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success flushing client");

  return L7_SUCCESS;
}

/**
 * Remove all dynamic client
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_dynamic_all_clients_flush(void)
{
  L7_RC_t rc;

  /* Remove all dynamic clients (only with channels) */
  rc = ptin_igmp_rm_all_clients(L7_TRUE,  L7_TRUE);

  if ( rc!=L7_SUCCESS )
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error flushing all dynamic clients");
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Clients flushed!");

  return L7_SUCCESS;
}


/**
 * Get client information from its index. 
 * 
 * @param intIfNum      : Interface Number
 * @param client_index  : Client index
 * @param client        : Client information (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_clientData_get(L7_uint32 intIfNum,
                                 L7_uint client_idx,
                                 ptin_client_id_t *client)
{
  L7_uint32   ptin_port;
  ptin_intf_t ptin_intf;
  ptinIgmpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if ( client==L7_NULLPTR || client_idx>=PTIN_IGMP_CLIENTIDX_MAX )
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }
  /* Validate interface */
  if (ptin_intf_intIfNum2port(intIfNum, &ptin_port) != L7_SUCCESS ||
      ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error converting intIfNum %u to ptin_port format", intIfNum);
    return L7_FAILURE;
  }

  /* Get pointer to client structure in AVL tree */
  clientInfo = igmpClients_unified.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;

  /* If does not exist... */
  if (clientInfo==L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Provided client (ptin_port=%u client_idx=%u) does not exist", ptin_port, client_idx);
    return L7_FAILURE;
  }

  memset(client,0x00,sizeof(ptin_client_id_t));
  #if (MC_CLIENT_INTERF_SUPPORTED)
  if (ptin_intf_port2ptintf(clientInfo->igmpClientDataKey.ptin_port, &ptin_intf) != L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot convert client port %u to ptin_intf format",clientInfo->igmpClientDataKey.ptin_port);
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
  //st_IgmpInstCfg_t *igmpInst;
  L7_uint32   evc_uc, evc_mc;
  ptin_intf_t ptin_intf;
  ptin_evc_intfCfg_t mc_intf_cfg;
  #if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
  ptin_evc_intfCfg_t uc_intf_cfg;
  #endif

  #if 0
  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }
  evc_mc = igmpInst->McastEvcId;
  evc_uc = igmpInst->UcastEvcId;
  #else
  /* Get EVC id */
  if (ptin_evc_get_evcIdfromIntVlan(intVlan, &evc_mc)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No EVC associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }
  evc_uc = evc_mc;
  #endif

  /* Get ptin_intf format for the interface number */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting ptin_intf from intIfNum %u",intIfNum);
    return L7_FAILURE;
  }

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(evc_mc, &ptin_intf,&mc_intf_cfg)!=L7_SUCCESS
      #if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
      || ptin_evc_intfCfg_get(evc_uc, &ptin_intf,&uc_intf_cfg)!=L7_SUCCESS
      #endif
     )
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting intf configuration for ptin_intf=%u/%u (intIfNum=%u), mcEvcId=%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum, evc_mc, intVlan);
    return L7_FAILURE;
  }

  /* Interfaces must be in use in both evcs */
  if (!mc_intf_cfg.in_use
      #if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
      || !uc_intf_cfg.in_use
      #endif
     )
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Interface ptin_intf=%u/%u (intIfNum=%u) not in use for mcEvcId=%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum, evc_mc, intVlan);
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
  /* Querier will use this routine */
  #if (0 /*!defined IGMP_QUERIER_IN_UC_EVC*/)
  st_IgmpInstCfg_t *igmpInst;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }
  #else
  L7_uint32 evc_idx;

  /* Get EVC id */
  if (ptin_evc_get_evcIdfromIntVlan(intVlan, &evc_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No EVC associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }
  /* Check if EVC is in use */
  if (!ptin_evc_is_in_use(evc_idx))
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC %u (intVlan=%u) is not active!",evc_idx,intVlan);
    return L7_FAILURE;
  }
  #endif

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
  #if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)

  //st_IgmpInstCfg_t *igmpInst;
  L7_uint32 evc_id;
  ptin_HwEthMef10Evc_t evcConf;

  #if 0
  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }
  evc_id = igmpInst->UcastEvcId;
  #else
  if (ptin_evc_get_evcIdfromIntVlan(intVlan, &evc_id)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No EVC id associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }
  #endif

  /* Get EVC configuration for the UC EVC */
  memset(&evcConf,0x00,sizeof(ptin_HwEthMef10Evc_t));
  evcConf.index = evc_id;
  if (ptin_evc_get(&evcConf)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting EVC configuration for ucEvcId=%u (intVlan=%u)",
              evc_id, intVlan);
    return L7_FAILURE;
  }

  if (is_unstacked!=L7_NULLPTR)
  {
    *is_unstacked = ((evcConf.flags & PTIN_EVC_MASK_STACKED) == 0);
  }

  #else

  L7_uint32 evc_uc;
  ptin_HwEthMef10Evc_t evcConf;

  /* Get EVC id from this internal vlan */
  if (ptin_evc_get_evcIdfromIntVlan(intVlan, &evc_uc)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No EVC associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get EVC configuration for the UC EVC */
  memset(&evcConf,0x00,sizeof(ptin_HwEthMef10Evc_t));
  evcConf.index = evc_uc;
  if (ptin_evc_get(&evcConf)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting EVC configuration for ucEvcId=%u (intVlan=%u)",
              evc_uc, intVlan);
    return L7_FAILURE;
  }

  if (is_unstacked!=L7_NULLPTR)
  {
    *is_unstacked = ((evcConf.flags & PTIN_EVC_MASK_STACKED) == 0);
  }

  #endif

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
  //st_IgmpInstCfg_t *igmpInst;
  L7_uint32   evc_id;
  ptin_intf_t ptin_intf;
  ptin_evc_intfCfg_t mc_intf_cfg;

  #if 0
  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }
  evc_id = igmpInst->McastEvcId;
  #else
  if (ptin_evc_get_evcIdfromIntVlan(intVlan, &evc_id)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No EVC id associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }
  #endif

  /* Get ptin_intf format for the interface number */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting ptin_intf from intIfNum %u",intIfNum);
    return L7_FAILURE;
  }

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(evc_id, &ptin_intf,&mc_intf_cfg)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting intf configuration for ptin_intf=%u/%u (intIfNum=%u), mcEvcId=%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum, evc_id, intVlan);
    return L7_FAILURE;
  }

  /* Interfaces must be in use in both evcs */
  if (!mc_intf_cfg.in_use)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Interface ptin_intf=%u/%u (intIfNum=%u) not in use for mcEvcId=%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum, evc_id, intVlan);
    return L7_FAILURE;
  }

  /* Interface must be Root */
  if (mc_intf_cfg.type!=PTIN_EVC_INTF_ROOT)
  {
    if (ptin_debug_igmp_snooping)
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Interface ptin_intf=%u/%u (intIfNum=%u) is not root for mcEvcId=%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum, evc_id, intVlan);
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
  //st_IgmpInstCfg_t *igmpInst;
  ptin_intf_t ptin_intf;
  ptin_evc_intfCfg_t intf_cfg;
  L7_uint32 evc_id;

  #if 0
  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }
  #endif

  /* Get ptin_intf format for the interface number */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting ptin_intf from intIfNum %u",intIfNum);
    return L7_FAILURE;
  }

  #if 0
  #if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
  evc_id = igmpInst->UcastEvcId;
  #else
  evc_id = igmpInst->McastEvcId;
  #endif
  #else
  if (ptin_evc_get_evcIdfromIntVlan(intVlan, &evc_id)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No EVC id associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }
  #endif

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(evc_id, &ptin_intf,&intf_cfg)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting intf configuration for ptin_intf=%u/%u (intIfNum=%u), EvcId=%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,evc_id,intVlan);
    return L7_FAILURE;
  }

  /* Interfaces must be in use in both evcs */
  if (!intf_cfg.in_use)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Interface ptin_intf=%u/%u (intIfNum=%u) not in use for EvcId=%u (intVlan=%u)",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,evc_id,intVlan);
    return L7_FAILURE;
  }

  /* Interface must be Leaf */
  if (intf_cfg.type!=PTIN_EVC_INTF_LEAF)
  {
//  if (ptin_debug_igmp_snooping)
//    LOG_ERR(LOG_CTX_PTIN_IGMP,"Interface ptin_intf=%u/%u (intIfNum=%u) is not client/leaf for EvcId=%u (intVlan=%u)",
//            ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,evc_id,intVlan);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
/**
 * Get the MC root vlan associated to the internal vlan
 *  
 * @param groupChannel  : Channel Group address 
 * @param sourceChannel : Channel Source address 
 * @param intVlan       : internal vlan
 * @param McastRootVlan : multicast root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_McastRootVlan_get(L7_inet_addr_t *groupChannel, L7_inet_addr_t *sourceChannel,
                                    L7_uint16 intVlan, L7_uint16 *McastRootVlan)
{
  st_IgmpInstCfg_t *igmpInst;
  L7_uint16 intRootVlan;
  L7_uint32 evcId_uc;
  L7_uint32 evcId_mc;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)==L7_SUCCESS)
  {
    /* This vlan is related to an EVC belonging to an IGMP instance: use its evc id */
    evcId_mc = igmpInst->McastEvcId;
  }
  else
  {
    /* Which UC service to use as reference? */
    evcId_uc = 0;

    #if (IGMPASSOC_CHANNEL_UC_EVC_ISOLATION)
    /* Find the EVC id related to this internal vlan */
    if (ptin_evc_get_evcIdfromIntVlan(intVlan, &uc_evcId)!=L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"No EVC associated to internal vlan %u",intVlan);
      return L7_FAILURE;
    }
    #endif

    /* Find associated MC service */
    if (igmp_assoc_pair_get( evcId_uc, groupChannel, sourceChannel, &evcId_mc )!=L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"No EVC associated to internal vlan %u",intVlan);
      return L7_FAILURE;
    }
    
  }

  /* Get Multicast root vlan */
  if (ptin_evc_intRootVlan_get(evcId_mc, &intRootVlan)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting McastRootVlan for MCEvcId=%u (intVlan=%u)",igmpInst->McastEvcId,intVlan);
    return L7_FAILURE;
  }

  /* Return Multicast root vlan */
  if (McastRootVlan!=L7_SUCCESS)  *McastRootVlan = intRootVlan;

  return L7_SUCCESS;
}
#else
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
  if (ptin_evc_intRootVlan_get(igmpInst->McastEvcId,&intRootVlan)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting McastRootVlan for MCEvcId=%u (intVlan=%u)",igmpInst->McastEvcId,intVlan);
    return L7_FAILURE;
  }

  /* Return Multicast root vlan */
  if (McastRootVlan!=L7_SUCCESS)  *McastRootVlan = intRootVlan;

  return L7_SUCCESS;
}
#endif

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
  ptin_HwEthMef10Evc_t evcCfg;
  L7_uint     intf_idx;
  L7_uint32   intIfNum;
  L7_uint     ptin_port;
  ptin_intf_t ptin_intf;
  L7_uint32   evc_idx;

  #if (!defined IGMP_QUERIER_IN_UC_EVC)
  st_IgmpInstCfg_t *igmpInst;

  /* IGMP instance, from internal vlan */
  if (ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get MC EVC configuration */
  memset(&evcCfg,0x00,sizeof(ptin_HwEthMef10Evc_t));
  #if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
  evc_idx = igmpInst->UcastEvcId;
  #else
  evc_idx = igmpInst->McastEvcId;
  #endif

  #else

  /* IGMP instance, from internal vlan */
  if (ptin_evc_get_evcIdfromIntVlan(intVlan, &evc_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No EVC associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }
  #endif

  evcCfg.index = evc_idx;

  if (ptin_evc_get(&evcCfg)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting evc %u configuration (intVlan=%u)",evcCfg.index,intVlan);
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
    /* Client ports are EVC leafs */
    if (evcCfg.intf[intf_idx].mef_type==PTIN_EVC_INTF_LEAF)
    {
      ptin_intf.intf_type = evcCfg.intf[intf_idx].intf_type;
      ptin_intf.intf_id   = evcCfg.intf[intf_idx].intf_id;

      /* Validate interface */
      if (ptin_intf_ptintf2port(&ptin_intf, &ptin_port)!=L7_SUCCESS ||
          ptin_intf_ptintf2intIfNum(&ptin_intf, &intIfNum)!=L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* It must have at least one client on this interface */
      #if PTIN_BOARD_IS_MATRIX
      /* No validation */
      #elif (!defined IGMP_QUERIER_IN_UC_EVC)
      if (igmpClients_unified.number_of_clients_per_intf[ptin_port] > 0)
      #else
      if (!(evcCfg.flags & PTIN_EVC_MASK_STACKED) || evcCfg.n_clients > 0)
      #endif
      {
        L7_INTF_SETMASKBIT(*intfList,intIfNum);
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

#if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
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
#endif

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
                                PTIN_SYSTEM_IGMP_MAXCLIENTS, PTIN_IGMP_TIMER_MSG_SIZE);
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

  /* Create timer buffer bools, and timer control blocks */

  bufferPoolId = 0;
  /* Timer Initializations */

  /* Control block buffer pool */
  if(bufferPoolInit(PTIN_SYSTEM_IGMP_MAXCLIENTS,
                    sizeof(timerNode_t) /*L7_APP_TMR_NODE_SIZE*/,
                    "PTin_IGMP_CtrlBlk_Timer_Bufs",
                    &bufferPoolId) != L7_SUCCESS)
  {
      LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to allocate memory for IGMP Control Block timer buffers");
      return L7_FAILURE;
  }
  igmpClients_unified.ctrlBlkBufferPoolId = bufferPoolId;

  /* Timers buffer pool */
  if(bufferPoolInit(PTIN_SYSTEM_IGMP_MAXCLIENTS,
                    sizeof(igmpTimerData_t),
                    "PTin_IGMP_Timer_Bufs",
                    &bufferPoolId) != L7_SUCCESS)
  {
      LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to allocate memory for IGMP client timer buffers");
      return L7_FAILURE;
  }
  igmpClients_unified.appTimerBufferPoolId = bufferPoolId;
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Allocated buffer pools");

  /* Create SLL list for each IGMP instance */
  if (SLLCreate(L7_PTIN_COMPONENT_ID, L7_SLL_NO_ORDER,
               sizeof(L7_uint32)*2, igmp_timer_dataCmp, igmp_timer_dataDestroy,
               &(igmpClients_unified.ll_timerList)) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Failed to create timer linked list");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"SLL list created");

  /* Create timer handles */
  /* Allocate memory for the Handle List */
  handleListMemHndl = (handle_member_t*) osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_IGMP_MAXCLIENTS*sizeof(handle_member_t));
  if (handleListMemHndl == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Error allocating Handle List Buffers");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Allocated memory for handle list");
  /* Create timers handle list for this IGMP instance  */
  if(handleListInit(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_IGMP_MAXCLIENTS,
                    &handle_list, handleListMemHndl) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Unable to create timer handle list");
    return L7_FAILURE;
  }
  igmpClients_unified.appTimer_handleListMemHndl = handleListMemHndl;
  igmpClients_unified.appTimer_handle_list = handle_list;
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Handle list created");

  /* Initialize timer control blocks */
  timerCB = appTimerInit(L7_PTIN_COMPONENT_ID, igmp_timerExpiryHdlr,
                         (void *) 0, L7_APP_TMR_1SEC,
                         igmpClients_unified.ctrlBlkBufferPoolId);
  if (timerCB  == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR,"snoopEntry App Timer Initialization Failed.");
    return L7_FAILURE;
  }
  igmpClients_unified.timerCB = timerCB;
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Timer initialized");

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
  /* Deinitialize timer control blocks */
  if (igmpClients_unified.timerCB != (L7_APP_TMR_CTRL_BLK_t) NULL)
  {
    appTimerDeInit(igmpClients_unified.timerCB);
    igmpClients_unified.timerCB = (L7_APP_TMR_CTRL_BLK_t) NULL;
  }

  /* Remove timers handle list for this IGMP instance  */
  handleListDeinit(L7_PTIN_COMPONENT_ID, igmpClients_unified.appTimer_handle_list);

  /* Free memory for the Handle List */
  if (igmpClients_unified.appTimer_handleListMemHndl != L7_NULLPTR)
  {
    osapiFree(L7_PTIN_COMPONENT_ID, igmpClients_unified.appTimer_handleListMemHndl);
    igmpClients_unified.appTimer_handleListMemHndl = L7_NULLPTR;
  }

  /* Destroy SLL list for each IGMP instance */
  SLLDestroy(L7_PTIN_COMPONENT_ID, &igmpClients_unified.ll_timerList);

  /* Buffer pool termination */
  if (igmpClients_unified.appTimerBufferPoolId != 0)
  {
    bufferPoolTerminate(igmpClients_unified.appTimerBufferPoolId);
    igmpClients_unified.appTimerBufferPoolId = 0;
  }
  if (igmpClients_unified.ctrlBlkBufferPoolId != 0)
  {
    bufferPoolTerminate(igmpClients_unified.ctrlBlkBufferPoolId);
    igmpClients_unified.ctrlBlkBufferPoolId = 0;
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
 * @param ptin_port  : Interface port
 * @param client_idx : client index
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_igmp_timer_start(L7_uint32 ptin_port, L7_uint32 client_idx)
{
  L7_BOOL timer_exists = L7_FALSE;
  igmpTimerData_t *pTimerData, timerData;
  L7_uint16 timeout;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to start timer for ptin_port=%u client_idx=%u", ptin_port, client_idx);

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }
  if (client_idx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client index %u", client_idx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_igmp_timers_sem, L7_WAIT_FOREVER);

  /* Check if this timer already exists */
  memset(&timerData, 0x00, sizeof(igmpTimerData_t));

  timerData.ptin_port  = PTIN_IGMP_CLIENT_PORT(ptin_port);
  timerData.client_idx = client_idx;

  if ( (pTimerData = (igmpTimerData_t *)SLLFind(&igmpClients_unified.ll_timerList, (void *)&timerData)) != L7_NULLPTR )
  {
    timer_exists = L7_TRUE;
  }

  if ( timer_exists )
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Already exists a timer running for ptin_port=%u client_idx=%u", ptin_port, client_idx);

    #if 1
    if (pTimerData->timer != L7_NULL)
    {
      if (appTimerDelete(igmpClients_unified.timerCB, (void *) pTimerData->timer) != L7_SUCCESS)
      {
        osapiSemaGive(ptin_igmp_timers_sem);
        if (ptin_debug_igmp_snooping)
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed removing timer");
        return L7_FAILURE;
      }
      pTimerData->timer = L7_NULLPTR;
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer removed!");

      /* Remove timer handle */
      handleListNodeDelete(igmpClients_unified.appTimer_handle_list, &pTimerData->timerHandle);
      pTimerData->timerHandle = 0;
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removed node from handle list (ptin_port=%u client_idx=%u)", ptin_port, client_idx);
    }
    #else
    if (pTimerData->timer != L7_NULL)
    {
      /* If exists, only update it */
      if (appTimerUpdate(igmpClients_unified.timerCB, pTimerData->timer,
                         (void *) igmp_timer_expiry, (void *) pTimerData->timerHandle, 30,
                         "PTIN_TIMER") != L7_SUCCESS)
      {
        osapiSemaGive(ptin_igmp_timers_sem);
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to update client timer");
        return L7_FAILURE;
      }
      osapiSemaGive(ptin_igmp_timers_sem);
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer updated for ptin_port=%u client_idx=%u", ptin_port, client_idx);
      return L7_SUCCESS;
    }
    else
    {
      //handleListNodeDelete(igmpClients_unified.appTimer_handle_list, &pTimerData->timerHandle);
      //pTimerData->timerHandle = 0;
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Handle node removed for ptin_port=%u client_idx=%u", ptin_port, client_idx);
    }
    #endif
  }
  else
  {
    /* Check if there is room for one more timer */
    if (igmpClients_unified.number_of_clients >= PTIN_IGMP_CLIENTIDX_MAX)
    {
      osapiSemaGive(ptin_igmp_timers_sem);
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not start timer. There is no room for more timers!");
      return L7_FAILURE;
    }

    /* Buffer pool allocation for pTimerData*/
    if (bufferPoolAllocate(igmpClients_unified.appTimerBufferPoolId, (L7_uchar8 **) &pTimerData) != L7_SUCCESS)
    {
      osapiSemaGive(ptin_igmp_timers_sem);
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not start timer. Insufficient memory.");
      return L7_FAILURE;
    }
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Allocated one Buffer node (ptin_port=%u client_idx=%u)", ptin_port, client_idx);

    /* Timer description */
    pTimerData->ptin_port  = PTIN_IGMP_CLIENT_PORT(ptin_port);
    pTimerData->client_idx = client_idx;
  }

  /* New timer handle */
  if ((pTimerData->timerHandle = handleListNodeStore(igmpClients_unified.appTimer_handle_list, pTimerData)) == 0)
  {
    /* Free the previously allocated bufferpool */
    bufferPoolFree(igmpClients_unified.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
    osapiSemaGive(ptin_igmp_timers_sem);
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not get the handle node to store the timer data.");
    return L7_FAILURE;
  }
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Stored node in handle list (ptin_port=%u client_idx=%u)", ptin_port, client_idx);

  timeout = (igmpProxyCfg.querier.group_membership_interval*3)/2;

  /* Add a new timer */
  pTimerData->timer = appTimerAdd( igmpClients_unified.timerCB, igmp_timer_expiry,
                                  (void *) pTimerData->timerHandle, timeout,
                                  "PTIN_TIMER");
  if (pTimerData->timer == NULL)
  {
    /* Free the previously allocated bufferpool */
    handleListNodeDelete(igmpClients_unified.appTimer_handle_list, &pTimerData->timerHandle);
    pTimerData->timerHandle = 0;
    bufferPoolFree(igmpClients_unified.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
    osapiSemaGive(ptin_igmp_timers_sem);
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not Start the Group timer.");
    return L7_FAILURE;
  }
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer added (ptin_port=%u client_idx=%u) with timeout=%u", ptin_port, client_idx, timeout);

  if ( !timer_exists )
  {
    /* Add timer to SLL */
    if (SLLAdd(&igmpClients_unified.ll_timerList, (L7_sll_member_t *)pTimerData) != L7_SUCCESS)
    {
      /* Free the previously allocated bufferpool */
//    LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not add new timer data node");
      if (appTimerDelete( igmpClients_unified.timerCB, pTimerData->timer) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to delete timer");
      }
      pTimerData->timer = L7_NULLPTR;
      handleListNodeDelete(igmpClients_unified.appTimer_handle_list, &pTimerData->timerHandle);
      pTimerData->timerHandle = 0;
      bufferPoolFree(igmpClients_unified.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
      osapiSemaGive(ptin_igmp_timers_sem);
      return L7_FAILURE;
    }
  }

  osapiSemaGive(ptin_igmp_timers_sem);

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer started successfully for ptin_port=%u client_idx=%u", ptin_port, client_idx);

  return L7_SUCCESS;
}

/**
 * Restart a timer for an existent client
 * 
 * @param ptin_port  : Interface port
 * @param client_idx : client index
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_igmp_timer_update(L7_uint32 ptin_port, L7_uint32 client_idx)
{
  igmpTimerData_t timerData, *pTimerData;
  L7_uint16 timeout;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to update an existent timer (ptin_port=%u client_idx=%u)", ptin_port, client_idx);

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }
  if (client_idx>=PTIN_IGMP_CLIENTIDX_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client index %u", client_idx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_igmp_timers_sem, L7_WAIT_FOREVER);

  memset(&timerData, 0x00, sizeof(igmpTimerData_t));

  timerData.ptin_port  = PTIN_IGMP_CLIENT_PORT(ptin_port);
  timerData.client_idx = client_idx;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Searching for an SLL node (ptin_port=%u client_idx=%u)", ptin_port, client_idx);

  /* Searching for the client timer */
  if ((pTimerData = (igmpTimerData_t *)SLLFind(&igmpClients_unified.ll_timerList, (void *)&timerData)) == L7_NULLPTR)
  {
    osapiSemaGive(ptin_igmp_timers_sem);
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Client timer not found");
    return L7_FAILURE;
  }

  if (pTimerData->timer == L7_NULL)
  {
    osapiSemaGive(ptin_igmp_timers_sem);
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Client timer not running");
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Updating timer (ptin_port=%u client_idx=%u)", ptin_port, client_idx);

  timeout = (igmpProxyCfg.querier.group_membership_interval*3)/2;

  if (appTimerUpdate(igmpClients_unified.timerCB, pTimerData->timer,
                     (void *) igmp_timer_expiry, (void *) pTimerData->timerHandle, timeout,
                     "PTIN_TIMER") != L7_SUCCESS)
  {
    osapiSemaGive(ptin_igmp_timers_sem);
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to update group membership timer");
    return L7_FAILURE;
  }

  osapiSemaGive(ptin_igmp_timers_sem);

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer updated successfully for ptin_port=%u client_idx=%u (timeout=%u)", ptin_port, client_idx, timeout);

  return L7_SUCCESS;
}

/**
 * Stops a running timer, andf remove all related resources
 * 
 * @param ptin_port  : Interface port
 * @param client_idx : client index
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_igmp_timer_stop(L7_uint32 ptin_port, L7_uint32 client_idx)
{
  igmpTimerData_t timerData;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to stop a timer (ptin_port=%u client_idx=%u)", ptin_port, client_idx);

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }
  if (client_idx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client index %u", client_idx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_igmp_timers_sem, L7_WAIT_FOREVER);

  memset(&timerData, 0x00, sizeof(igmpTimerData_t));

  timerData.ptin_port  = PTIN_IGMP_CLIENT_PORT(ptin_port);
  timerData.client_idx = client_idx;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Deleting SLL node (ptin_port=%u client_idx=%u)", ptin_port, client_idx);

  /* Remove node for SLL list */
  if (SLLDelete(&igmpClients_unified.ll_timerList, (L7_sll_member_t *)&timerData) != L7_SUCCESS)
  {
    osapiSemaGive(ptin_igmp_timers_sem);
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Failed to delete timer node");
    return L7_FAILURE;
  }

  osapiSemaGive(ptin_igmp_timers_sem);

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer stopped successfully for ptin_port=%u client_idx=%u", ptin_port, client_idx);

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
  L7_uint ptin_port, client_idx;

  igmpTimerData_t *pTimerData;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Expiration event ocurred for timerHandle %u!",timerHandle);

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);
  osapiSemaTake(ptin_igmp_timers_sem, L7_WAIT_FOREVER);

  /* Get timer handler */
  pTimerData = (igmpTimerData_t *) handleListNodeRetrieve(timerHandle);
  if (pTimerData == L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Failed to retrieve handle");
    osapiSemaGive(ptin_igmp_timers_sem);
    osapiSemaGive(ptin_igmp_clients_sem);
    return;
  }

  if (timerHandle != pTimerData->timerHandle)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Weird situation!");
  }

  /* Save client information */
  ptin_port  = PTIN_IGMP_CLIENT_PORT(pTimerData->ptin_port);
  client_idx = pTimerData->client_idx;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Expiration event ocurred for ptin_port=%u client_idx=%u", ptin_port, client_idx);

  /* Delete timer */
  if (appTimerDelete(igmpClients_unified.timerCB, pTimerData->timer)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Cannot delete timer (ptin_port=%u client_idx=%u)", ptin_port, client_idx);
  }
  pTimerData->timer = (L7_APP_TMR_HNDL_t) NULL;
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer deleted for ptin_port=%u client_idx=%u", ptin_port, client_idx);

  /* Delete the handle we had created */
  handleListNodeDelete(igmpClients_unified.appTimer_handle_list, &pTimerData->timerHandle);
  pTimerData->timerHandle = 0;

  osapiSemaGive(ptin_igmp_timers_sem);

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removing client (ptin_port=%u client_idx=%u)", ptin_port, client_idx);

  /* Remove client (only if it is dynamic. For static ones, only is removed from snooping entries) */
  if (ptin_igmp_rm_clientIdx(ptin_port, client_idx, L7_FALSE, L7_TRUE)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Failed removing client (ptin_port=%u client_idx=%u)!", ptin_port, client_idx);
  }
  else
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Client removed (ptin_port=%u client_idx=%u)", ptin_port, client_idx);
  }

  osapiSemaGive(ptin_igmp_clients_sem);
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

      appTimerProcess( igmpClients_unified.timerCB );
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
  ptinIgmpTimerParams_t msg;

  //LOG_TRACE(LOG_CTX_PTIN_IGMP,"Received a timer event");

  msg.dummy = 0;

  //LOG_TRACE(LOG_CTX_PTIN_IGMP,"Sending timer event to queue");

  rc = osapiMessageSend(clientsMngmt_queue, &msg, PTIN_IGMP_TIMER_MSG_SIZE, L7_NO_WAIT,L7_MSG_PRIORITY_NORM);

  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Client timer tick send failed");
    return;
  }

  //LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer event sent to queue");
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

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to destroy timer");

  /* Validate argument */
  if (ll_member==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Null arguments");
    return L7_FAILURE;
  }

  pTimerData = (igmpTimerData_t *)ll_member;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Ready to destroy timer for ptin_port=%u client_idx=%u", pTimerData->ptin_port, pTimerData->client_idx);

  /* Destroy timer */
  if (pTimerData->timer != L7_NULL)
  {
    /* Delete the apptimer node */
    if (appTimerDelete(igmpClients_unified.timerCB, pTimerData->timer)!=L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Cannot delete timer (ptin_port=%u client_idx=%u)", pTimerData->ptin_port, pTimerData->client_idx);
    }
    pTimerData->timer = (L7_APP_TMR_HNDL_t) NULL;

    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer deleted for ptin_port=%u client_idx=%u", pTimerData->ptin_port, pTimerData->client_idx);

    /* Delete the handle we had created */
    handleListNodeDelete(igmpClients_unified.appTimer_handle_list, &pTimerData->timerHandle);
    pTimerData->timerHandle = 0;

    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer handle removed for ptin_port=%u client_idx=%u", pTimerData->ptin_port, pTimerData->client_idx);
  }
  else
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Timer not running for ptin_port=%u client_idx=%u", pTimerData->ptin_port, pTimerData->client_idx);
  }

  bufferPoolFree(igmpClients_unified.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Buffer node removed for ptin_port=%u client_idx=%u", pTimerData->ptin_port, pTimerData->client_idx);

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer destroyed successfully for ptin_port=%u client_idx=%u", pTimerData->ptin_port, pTimerData->client_idx);

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
#if PTIN_IGMP_CLIENTS_ISOLATED_PER_INTF
  if ( ((igmpTimerData_t *) p)->ptin_port < ((igmpTimerData_t *) q)->ptin_port )
    return -1;
  if ( ((igmpTimerData_t *) p)->ptin_port > ((igmpTimerData_t *) q)->ptin_port )
    return 1;
#endif

  if ( ((igmpTimerData_t *) p)->client_idx < ((igmpTimerData_t *) q)->client_idx )
    return -1;

  if ( ((igmpTimerData_t *) p)->client_idx > ((igmpTimerData_t *) q)->client_idx )
    return 1;

  return 0;
}
#endif

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED

/**
 * Clear all IGMP associations
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t igmp_assoc_init( void )
{
  /* Purge all AVL tree, but the root node */
  return igmp_assoc_avlTree_purge();
}

/**
 * Get the association of a particular dst/src channel.
 * 
 * @param evc_uc : UC EVC index
 * @param channel_group   : Group address
 * @param channel_source  : Source address
 * @param evc_mc : MC EVC pair (out)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t igmp_assoc_pair_get( L7_uint32 evc_uc,
                                    L7_inet_addr_t *channel_group,
                                    L7_inet_addr_t *channel_source,
                                    L7_uint32 *evc_mc )
{
  ptinIgmpPairDataKey_t  avl_key;
  ptinIgmpPairInfoData_t *avl_infoData;
  L7_inet_addr_t group_address;
  #if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
  L7_inet_addr_t source_address;
  #endif

  #if ( IGMPASSOC_CHANNEL_UC_EVC_ISOLATION )
  /* Validate unicast service */
  if ( evc_uc >= PTIN_SYSTEM_N_EXTENDED_EVCS )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid UC eEVC (%u)", evc_uc);
    return L7_FAILURE;
  }
  /* Check if UC EVC is active */
  if ( !ptin_evc_is_in_use(evc_uc) )
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"UC eEVC %u is not in use!", evc_uc);
    return L7_FAILURE;
  }
  #endif

  /* Validate group address */
  if (igmp_assoc_channelIP_prepare(channel_group, 32, &group_address, L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid group address");
    return L7_FAILURE;
  }  

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Channel_group=0x%08x, group_address=0x%08x!",
              channel_group->addr.ipv4.s_addr, group_address.addr.ipv4.s_addr);
  

  #if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
  /* Validate source address */
  if (igmp_assoc_channelIP_prepare(channel_source, 32, &source_address, L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid source address");
    return L7_FAILURE;
  }
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Channel_source=0x%08x, source_address=0x%08x!",
              channel_source->addr.ipv4.s_addr, source_address.addr.ipv4.s_addr);
  #endif

  /* Prepare key */
  memset( &avl_key, 0x00, sizeof(ptinIgmpPairDataKey_t) );
  #if ( IGMPASSOC_CHANNEL_UC_EVC_ISOLATION )
  avl_key.evc_uc = evc_uc;
  #endif

  memcpy(&avl_key.channel_group, &group_address, sizeof(L7_inet_addr_t));

  #if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
  memcpy(&avl_key.channel_source, &source_address, sizeof(L7_inet_addr_t));
  #endif

  /* Check if this key does not exist */
  if ((avl_infoData=(ptinIgmpPairInfoData_t *) avlSearchLVL7( &(igmpPairDB.igmpPairAvlTree), (void *)&avl_key, AVL_EXACT)) == L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Group channel 0x%08x does not exist!",group_address.addr.ipv4.s_addr);
    return L7_FAILURE;
  }

  /* Return MC EVC */
  if ( evc_mc != L7_NULLPTR )
  {
    *evc_mc = avl_infoData->evc_mc;
  }

  
  return L7_SUCCESS;
}

/**
 * Get the association of a particular dst/src channel using 
 * vlans. 
 * 
 * @param vlan_uc : UC vlan
 * @param channel_group   : Group address
 * @param channel_source  : Source address
 * @param vlan_mc : MC vlan pair (out)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t igmp_assoc_vlanPair_get( L7_uint16 vlan_uc,
                                 L7_inet_addr_t *channel_group,
                                 L7_inet_addr_t *channel_source,
                                 L7_uint16 *vlan_mc )
{
  L7_uint32 evc_uc=0;
  L7_uint32 evc_mc=0;

  #if ( IGMPASSOC_CHANNEL_UC_EVC_ISOLATION )
  if ( vlan_uc >= PTIN_VLAN_MIN && vlan_uc <= PTIN_VLAN_MAX )
  {
    /* Verify if this internal vlan is associated to an EVC */
    if (ptin_evc_get_evcIdfromIntVlan(vlan_uc, &evc_uc)!=L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"No EVC associated to internal vlan %u",vlan_uc);
      return L7_FAILURE;
    }
  }
  #endif

  /* Get MC evc id */
  if (igmp_assoc_pair_get(evc_uc, channel_group, channel_source, &evc_mc) != L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No MC EVC associated to vlan_uc %u, group=0x%08x",vlan_uc, channel_group->addr.ipv4.s_addr);
    return L7_FAILURE;
  }

  /* Return the root vlan associated to the MC service */
  if ( ptin_evc_intRootVlan_get( evc_mc, vlan_mc ) != L7_SUCCESS )
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error obtaining MC vlan of MC evc %u (associated to evc_uc %u, group=0x%08x)",
              evc_mc, evc_uc, channel_group->addr.ipv4.s_addr);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get the the list of channels of a UC+MC association
 * 
 * @param evc_uc : UC EVC index (0 to list all)
 * @param evc_mc : MC EVC pair  (0 to list all)
 * @param channel_group   : Array of group channels - max of 
 *                          *channels_number (output)
 * @param channel_source  : Array of source channels - max of
 *                          *channels_number (output)
 * @param channels_number : In - Max #channels to get 
 *                          Out - Effective #channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t igmp_assoc_channelList_get( L7_uint32 evc_uc, L7_uint32 evc_mc,
                                    igmpAssoc_entry_t *channel_list,
                                    L7_uint16 *channels_number )
{
  ptinIgmpPairDataKey_t avl_key;
  ptinIgmpPairInfoData_t *avl_info;
  L7_uint16 channel_i, channels_max = IGMPASSOC_CHANNELS_MAX;

  /* Validate arguments */
  if ( channels_number == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Define maximum number of channels to be read */
  if ( *channels_number > 0 && *channels_number < IGMPASSOC_CHANNELS_MAX )
  {
    channels_max = *channels_number;
  }

  /* Run all cells in AVL tree */
  memset(&avl_key, 0x00, sizeof(ptinIgmpPairDataKey_t));

  channel_i = 0;
  while ( channel_i < channels_max &&
          (avl_info=(ptinIgmpPairInfoData_t *) avlSearchLVL7(&igmpPairDB.igmpPairAvlTree, (void *)&avl_key, AVL_NEXT)) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->igmpPairDataKey, sizeof(ptinIgmpPairDataKey_t));

    /* Verify uc evc */
    #if ( IGMPASSOC_CHANNEL_UC_EVC_ISOLATION )
    if ( evc_uc != 0 && evc_uc != (L7_uint16)-1 &&
         evc_uc != avl_info->igmpPairDataKey.evc_uc )
    {
      continue;
    }
    #endif

    /* Verify MC evc */
    if ( evc_mc != 0 && evc_mc != (L7_uint16)-1 &&
         avl_info->evc_mc != evc_mc )
    {
      continue;
    }

    /* Clear data */
    memset(&channel_list[channel_i], 0x00, sizeof(igmpAssoc_entry_t));

    /* UC evc */
    #if ( IGMPASSOC_CHANNEL_UC_EVC_ISOLATION )
    channel_list[channel_i].evc_uc = avl_info->igmpPairDataKey.evc_uc;
    #endif
    /* Group address */
    memcpy( &channel_list[channel_i].groupAddr, &avl_info->igmpPairDataKey.channel_group, sizeof(L7_inet_addr_t) );
    /* Source address */
    #if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
    memcpy( &channel_list[channel_i].sourceAddr, &avl_info->igmpPairDataKey.channel_source, sizeof(L7_inet_addr_t) );
    #endif
    /* MC evc */
    channel_list[channel_i].evc_mc = avl_info->evc_mc;
    /* Is static? */
    channel_list[channel_i].is_static = avl_info->is_static;

    /* One more channel */
    channel_i++;
  }

  /* Return number of read channels */
  *channels_number = channel_i;

  return L7_SUCCESS;
}

/**
 * Add a new association to a MC service, applied only to a 
 * specific dst/src channel.
 * 
 * @param evc_uc : UC EVC index
 * @param evc_mc : MC EVC index
 * @param channel_group   : Group channel
 * @param channel_grpMask : Number of masked bits
 * @param channel_source  : Source channel
 * @param channel_srcMask : Number of masked bits
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t igmp_assoc_channel_add( L7_uint32 evc_uc, L7_uint32 evc_mc,
                                L7_inet_addr_t *channel_group , L7_uint16 channel_grpMask,
                                L7_inet_addr_t *channel_source, L7_uint16 channel_srcMask,
                                L7_BOOL is_static )
{
  L7_uint         igmpInst_idx;
  L7_inet_addr_t  group, source;
  L7_int32        i, n_groups=1;
  L7_int32        j, n_sources=1;
  ptinIgmpPairInfoData_t avl_node;
  L7_RC_t rc;

  /* Validate and prepare channel group Address*/
  if (igmp_assoc_channelIP_prepare( channel_group, channel_grpMask, &group, &n_groups)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error preparing groupAddr");
    //return L7_FAILURE;
    return L7_SUCCESS;
  }
  /* Validate output ip address */
  if ( n_groups == 0 )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Group address is not valid!");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to add group 0x%08x (%u addresses)", group.addr.ipv4.s_addr, n_groups);

  /* source ip */
  memset(&source, 0x00, sizeof(L7_inet_addr_t));
  n_sources = 1;

  #if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
  /* Prepare source channel */
  igmp_assoc_channelIP_prepare( channel_source, channel_srcMask, &source, &n_sources);
  /* Validate output ip address */
  if ( n_sources == 0 )
  {
    n_sources = 1;
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Source address is not valid!");
  }
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to add source 0x%08x (%u addresses)", source.addr.ipv4.s_addr, n_sources);
  #endif

  /* Validate multicast service */
  if ( evc_mc >= PTIN_SYSTEM_N_EXTENDED_EVCS )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid MC evc (%u)", evc_mc);
    return L7_FAILURE;
  }
  /* Check if MC EVC is active */
  if ( !ptin_evc_is_in_use(evc_mc) )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"MC evc %u is not in use!", evc_mc);
    return L7_DEPENDENCY_NOT_MET;
  }
  /* MC EVC should be part of an IGMP instance */
  if (ptin_igmp_instance_find_fromMcastEvcId( evc_mc, &igmpInst_idx ) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"MC evc %u does not belong to any IGMP instance!", evc_mc);
    return L7_FAILURE;
  }

  /* Validate number of channels */
  if ( n_groups > IGMPASSOC_CHANNELS_MAX ||
       n_sources > IGMPASSOC_CHANNELS_MAX ||
       (igmpPairDB.number_of_entries + n_groups*n_sources) > IGMPASSOC_CHANNELS_MAX )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot add more than %u channels (already present: %u)", IGMPASSOC_CHANNELS_MAX, igmpPairDB.number_of_entries);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Maximum addresses to be added: %u", n_groups*n_sources);

  memset( &avl_node, 0x00, sizeof(ptinIgmpPairInfoData_t));
  avl_node.evc_uc     = evc_uc;
  avl_node.evc_mc     = evc_mc;
  avl_node.igmp_idx   = igmpInst_idx;
  avl_node.is_static  = is_static & 1;

  /* Add channels */
  i = j = 0;
  rc = L7_SUCCESS;

  /* Run all group addresses */
  while ((rc==L7_SUCCESS && i<n_groups) || (rc!=L7_SUCCESS && i>=0))
  {
    #if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
    /* Run all source addresses */
    while ((rc==L7_SUCCESS && j<n_sources) || (rc!=L7_SUCCESS && j>=0))
    #endif
    {
      /* Prepare key */
      #if ( IGMPASSOC_CHANNEL_UC_EVC_ISOLATION )
      avl_node.igmpPairDataKey.evc_uc = evc_uc;
      #endif

      memcpy(&avl_node.igmpPairDataKey.channel_group, &group, sizeof(L7_inet_addr_t));

      #if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
      memcpy(&avl_node.igmpPairDataKey.channel_source, &source, sizeof(L7_inet_addr_t));
      #endif

      /* In case of success, continue adding nodes into avl tree */
      if (rc == L7_SUCCESS)
      {
        if (igmp_assoc_avlTree_insert( &avl_node ) != L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Error inserting group channel 0x%08x, source=0x%08x for UC_EVC=%u",
                  group.addr.ipv4.s_addr, source.addr.ipv4.s_addr, evc_uc);
          rc = L7_FAILURE;
        }
        else
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Added group 0x%08x, source 0x%08x", group.addr.ipv4.s_addr, source.addr.ipv4.s_addr);
        }
      }
      /* If one error ocurred, remove previously added nodes */
      else
      {
        if (igmp_assoc_avlTree_remove( &avl_node.igmpPairDataKey ) != L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing group channel 0x%08x, source=0x%08x for UC_EVC=%u",
                  group.addr.ipv4.s_addr, source.addr.ipv4.s_addr, evc_uc);
        }
        else
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removed group 0x%08x, source 0x%08x", group.addr.ipv4.s_addr, source.addr.ipv4.s_addr);
        }
      }

      #if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
      /* Next source ip address */
      if (source.family != L7_AF_INET6)
      {
        if (rc==L7_SUCCESS)
        {
          source.addr.ipv4.s_addr++;  j++;
        }
        else
        {
          source.addr.ipv4.s_addr--;  j--;
        }
      }
      else
        break;
      #endif
    }
    /* Next group address */
    if (group.family != L7_AF_INET6)
    {
      if (rc==L7_SUCCESS)
      {
        group.addr.ipv4.s_addr++;   i++;
      }
      else
      {
        group.addr.ipv4.s_addr--;   i--;
      }
    }
    else
      break;
  }

#if 0
  if (rc==L7_SUCCESS)
 {
#if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
   //Only IPv4 is supported!
   if(channel_group->family==L7_AF_INET && channel_source->family==L7_AF_INET)
   {
     LOG_ERR(LOG_CTX_PTIN_IGMP,"IPv6 not supported for MGMD [UC_EVC=%u MC_EVC]",evc_uc,evc_mc);
     return L7_FAILURE;                       
   }
   if(L7_SUCCESS != ptin_igmp_mgmd_whitelist_add(evc_mc,channel_group->addr.ipv4.s_addr,channel_grpMask,channel_source->addr.ipv4.s_addr,channel_srcMask))
   {
     LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to create requested entry in the whitelist");
     return L7_FAILURE;
   }
#else
   //Only IPv4 is supported!
   if(channel_group->family!=L7_AF_INET)
   {
     LOG_ERR(LOG_CTX_PTIN_IGMP,"IPv6 not supported for MGMD [UC_EVC=%u MC_EVC]",evc_uc,evc_mc);
     return L7_FAILURE;     
   }
   if(L7_SUCCESS != ptin_igmp_mgmd_whitelist_add(evc_mc,channel_group->addr.ipv4.s_addr,channel_grpMask,0,0))
   {
     LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to create requested entry in the whitelist");
     return L7_FAILURE;
   }
#endif//IGMPASSOC_CHANNEL_SOURCE_SUPPORTED    
 }
#endif

 return rc; 
}

/**
 * Remove an association to a MC service, applied only to a 
 * specific dst/src channel. 
 * 
 * @param evc_uc : UC EVC index
 * @param channel_group   : Group channel
 * @param channel_grpMask : Number of masked bits
 * @param channel_source  : Source channel
 * @param channel_srcMask : Number of masked bits
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t igmp_assoc_channel_remove( L7_uint32 evc_uc,
                                   L7_inet_addr_t *channel_group, L7_uint16 channel_grpMask,
                                   L7_inet_addr_t *channel_source, L7_uint16 channel_srcMask)
{
  L7_inet_addr_t        group, source;
  L7_uint32             i, n_groups=1;
  L7_uint32             j, n_sources=1;
  ptinIgmpPairDataKey_t avl_key;
  L7_uint32             evc_mc;//Added to support MGMD
  L7_RC_t rc;

  /* Validate and prepare channel group Address*/
  if (igmp_assoc_channelIP_prepare( channel_group, channel_grpMask, &group, &n_groups)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error preparing groupAddr");
    //return L7_FAILURE;
    return L7_SUCCESS;
  }
  /* Validate output ip address */
  if ( n_groups == 0 )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Group address is not valid!");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to remove group 0x%08x (%u addresses)", group.addr.ipv4.s_addr, n_groups);

  /* source ip */
  memset(&source, 0x00, sizeof(L7_inet_addr_t));
  n_sources = 1;

  #if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
  /* Prepare source channel */
  igmp_assoc_channelIP_prepare( channel_source, channel_srcMask, &source, &n_sources);
  /* Validate output ip address */
  if ( n_sources == 0 )
  {
    n_sources = 1;
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Source address is not valid!");
  }
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to remove source 0x%08x (%u addresses)", source.addr.ipv4.s_addr, n_sources);
  #endif

  /* Validate number of channels */
  if ( n_groups > IGMPASSOC_CHANNELS_MAX ||
       n_sources > IGMPASSOC_CHANNELS_MAX ||
       n_groups*n_sources > IGMPASSOC_CHANNELS_MAX
     )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot add more than %u channels", IGMPASSOC_CHANNELS_MAX);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Maximum addresses to be removed: %u", n_groups*n_sources);

  rc = L7_SUCCESS;

  memset( &avl_key, 0x00, sizeof(ptinIgmpPairDataKey_t));

  /* Add channels */
  for (i=0; i<n_groups; i++)
  {
    for (j=0; j<n_sources; j++)
    {
      /* Prepare key */
      #if ( IGMPASSOC_CHANNEL_UC_EVC_ISOLATION )
      avl_key.evc_uc = evc_uc;
      #endif

      memcpy(&avl_key.channel_group, &group, sizeof(L7_inet_addr_t));

      #if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
      memcpy(&avl_key.channel_source, &source, sizeof(L7_inet_addr_t));
      #endif

      /* Find associated MC service */
      if (igmp_assoc_pair_get( evc_uc, &group, &source, &evc_mc )!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"No MC EVC associated with UC EVC %u",evc_uc);
        rc = L7_FAILURE;
      }      

      /* Add node into avl tree */
      if ( (rc == L7_SUCCESS) && (igmp_assoc_avlTree_remove( &avl_key ) != L7_SUCCESS))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing group channel 0x%08x, source=0x%08x for UC_EVC=%u",
                group.addr.ipv4.s_addr, source.addr.ipv4.s_addr, evc_uc);
        rc = L7_FAILURE;
      }

      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removed group 0x%08x, source 0x%08x", group.addr.ipv4.s_addr, source.addr.ipv4.s_addr);

      /* Next source ip address */
      if (source.family != L7_AF_INET6)
        source.addr.ipv4.s_addr++;
      else
        break;
    }
    /* Next group address */
    if (group.family != L7_AF_INET6)
      group.addr.ipv4.s_addr++; 
    else
      break;
  }

#if 0
 if (rc==L7_SUCCESS)
 {
#if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
   //Only IPv4 is supported!
   if(channel_group->family==L7_AF_INET && channel_source->family!=L7_AF_INET)
   {
     LOG_ERR(LOG_CTX_PTIN_IGMP,"IPv6 not supported for MGMD [UC_EVC=%u MC_EVC]",evc_uc,evc_mc);
     return FAILURE;                       
   }
   ptin_igmp_mgmd_whitelist_remove(evc_mc,channel_group->addr.ipv4.s_addr,channel_grpMask,channel_source->addr.ipv4.s_addr,channel_srcMask);   
#else
   if(channel_group->family!=L7_AF_INET)
   {
     LOG_ERR(LOG_CTX_PTIN_IGMP,"IPv6 not supported for MGMD [UC_EVC=%u MC_EVC]",evc_uc,evc_mc);
     return FAILURE;     
   }
   ptin_igmp_mgmd_whitelist_remove(evc_mc,channel_group->addr.ipv4.s_addr,channel_grpMask,0,0);
#endif//IGMPASSOC_CHANNEL_SOURCE_SUPPORTED    
 }
#endif

  return L7_SUCCESS;
}

/**
 * Remove all associations of a MC instance
 * 
 * @param evc_uc : UC EVC index 
 * @param evc_uc : MC EVC index 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t igmp_assoc_channel_clear( L7_uint32 evc_uc, L7_uint32 evc_mc )
{
  /* Validate multicast service */
  if ( evc_mc >= PTIN_SYSTEM_N_EXTENDED_EVCS )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid MC eEVC (%u)", evc_mc);
    return L7_FAILURE;
  }
  /* Check if MC EVC is active */
  if ( !ptin_evc_is_in_use(evc_mc) )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"MC eEVC %u is not in use!", evc_mc);
    return L7_FAILURE;
  }

  /* Clear entries */
  if (igmp_assoc_avlTree_clear(evc_uc, evc_mc)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing all channels to MC evc %u, UC evc %u !", evc_mc, evc_uc);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removed all channels to MC evc %u, UC evc %u !", evc_mc, evc_uc);

  return L7_SUCCESS;
}

/**
 * Remove all associations
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t igmp_assoc_clean_all(void)
{
  return igmp_assoc_avlTree_purge();
}


/**
 * Prepare an ip address to be used for the AVL trees
 * 
 * @param channel_in  : ip address (in) 
 * @param channel_mask : number of bits to be masked (in)
 * @param channel_out : ip address to be returned (out)
 * @param number_of_channels: number of used channels (out)
 * 
 * @return L7_RC_t 
 */
static L7_RC_t igmp_assoc_channelIP_prepare( L7_inet_addr_t *channel_in, L7_uint16 channel_mask,
                                             L7_inet_addr_t *channel_out, L7_uint32 *number_of_channels)
{
  L7_uint16 mask_inv;

  /* Initialize output variables */
  if ( channel_out != L7_NULLPTR )
    memset(channel_out, 0x00, sizeof(L7_inet_addr_t));
  if ( number_of_channels != L7_NULLPTR )
    *number_of_channels = 0;

  /* Validate channel IP */
  if ( channel_in == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Null pointer of provided address");
    return L7_FAILURE;
  }

  /* Only IPv4 is supported */
  if ( channel_in->family == L7_AF_INET6 )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Not supported IPv6");
    return L7_FAILURE;
  }

  /* Invalid IP value */
  if ( channel_in->addr.ipv4.s_addr == 0 || channel_in->addr.ipv4.s_addr == 0xffffffff )
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Not valid address: 0x%08x",channel_in->addr.ipv4.s_addr);
    return L7_FAILURE;
  }

  /* Limit number of bits to be used */
  if ( channel_mask > sizeof(L7_uint32)*8 )  channel_mask = sizeof(L7_uint32)*8;

  mask_inv = sizeof(L7_uint32)*8 - channel_mask;

  /* Channel IP to be returned */
  if ( channel_out != L7_NULLPTR )
  {
    /* IP Address */
    channel_out->family = L7_AF_INET;
    channel_out->addr.ipv4.s_addr = channel_in->addr.ipv4.s_addr;

    /* Clear bits not covered by the mask */
    channel_out->addr.ipv4.s_addr >>= mask_inv;
    channel_out->addr.ipv4.s_addr <<= mask_inv;
  }

  /* Number of channels */
  if ( number_of_channels != L7_NULLPTR )
  {
    L7_uint16 i, n;

    n = 1;
    for (i=0; i<mask_inv; i++)
    {
      n *= 2;
      if (n > IGMPASSOC_CHANNELS_MAX)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Mask too small (%u bits)",channel_mask);
        return L7_FAILURE;
      }
    }
    *number_of_channels = n;
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Bit mask = %u, number of = %u", channel_mask, n);
  }

  return L7_SUCCESS;
}

/**
 * Insert a node to the IGMPpair tree
 * 
 * @param node : node information to be added
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE;
 */
static L7_RC_t igmp_assoc_avlTree_insert( ptinIgmpPairInfoData_t *node )
{
  ptinIgmpPairDataKey_t  avl_key;
  ptinIgmpPairInfoData_t *avl_infoData;

  /* Check if there is enough room for one more channels */
  if (igmpPairDB.number_of_entries >= IGMPASSOC_CHANNELS_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"No more free entries!");
    return L7_FAILURE;
  }

  /* Prepare key */
  memset( &avl_key, 0x00, sizeof(ptinIgmpPairDataKey_t) );

  #if ( IGMPASSOC_CHANNEL_UC_EVC_ISOLATION )
  avl_key.evc_uc = node->igmpPairDataKey.evc_uc;
  #endif

  memcpy(&avl_key.channel_group, &node->igmpPairDataKey.channel_group, sizeof(L7_inet_addr_t));

  #if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
  memcpy(&avl_key.channel_source, &node->igmpPairDataKey.channel_source, sizeof(L7_inet_addr_t));
  #endif

  /* Check if this key already exists */
  if ((avl_infoData=(ptinIgmpPairInfoData_t *) avlSearchLVL7( &(igmpPairDB.igmpPairAvlTree), (void *)&avl_key, AVL_EXACT)) != L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Group channel 0x%08x already exists",
                node->igmpPairDataKey.channel_group.addr.ipv4.s_addr);
    return L7_SUCCESS;
  }

  /* Add key */
  if (avlInsertEntry(&(igmpPairDB.igmpPairAvlTree), (void *)&avl_key) != L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error inserting group channel 0x%08x",
            node->igmpPairDataKey.channel_group.addr.ipv4.s_addr);
    return L7_FAILURE;
  }

  /* Search for inserted key */
  if ((avl_infoData=(ptinIgmpPairInfoData_t *) avlSearchLVL7(&(igmpPairDB.igmpPairAvlTree),(void *)&avl_key, AVL_EXACT)) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Group channel 0x%08x was added, but does not exist",
            node->igmpPairDataKey.channel_group.addr.ipv4.s_addr);

    return L7_FAILURE;
  }

  /* One more entry */
  igmpPairDB.number_of_entries++;

  /* Fill with remaining data */
  avl_infoData->evc_mc    = node->evc_mc;
  #if ( IGMPASSOC_CHANNEL_UC_EVC_ISOLATION )
  avl_infoData->evc_uc    = node->evc_uc;
  #else
  avl_infoData->evc_uc    = 0;
  #endif
  avl_infoData->igmp_idx  = node->igmp_idx;
  avl_infoData->is_static = node->is_static & 1;

  return L7_SUCCESS;
}

/**
 * Remove a node to the IGMPpair tree
 * 
 * @param key : key information to be removed
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE;
 */
static L7_RC_t igmp_assoc_avlTree_remove( ptinIgmpPairDataKey_t *avl_key )
{
  ptinIgmpPairInfoData_t *avl_infoData;

  /* Check if this key does not exists */
  if ((avl_infoData=(ptinIgmpPairInfoData_t *) avlSearchLVL7( &(igmpPairDB.igmpPairAvlTree), (void *) avl_key, AVL_EXACT)) == L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Group channel 0x%08x does not exist",
                avl_key->channel_group.addr.ipv4.s_addr);
    return L7_SUCCESS;
  }

  #if 0
  L7_uint16 i;

  printf("Printing provided key:");
  for (i=0; i<sizeof(ptinIgmpPairDataKey_t); i++)
  {
    if (i%16==0)  printf("\r\n0x%04x:",i);
    printf(" %02x",*(((L7_uchar8 *) avl_key)+i) );
  }
  printf("\r\ndone!\r\n");
  #endif

  /* Remove key */
  if (avlDeleteEntry(&(igmpPairDB.igmpPairAvlTree), (void *) avl_key) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing group channel 0x%08x",
            avl_key->channel_group.addr.ipv4.s_addr);
    return L7_FAILURE;
  }

  /* One less entry */
  if (igmpPairDB.number_of_entries>0)
    igmpPairDB.number_of_entries--;

  return L7_SUCCESS;
}

/**
 * Remove all nodes from the IGMPpair tree related to a UC/MC 
 * service. 
 * 
 * @param evc_uc : Unicast evc
 * @param evc_mc : Multicast evc 
 *  
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE;
 */
static L7_RC_t igmp_assoc_avlTree_clear( L7_uint32 evc_uc, L7_uint32 evc_mc )
{
  ptinIgmpPairDataKey_t avl_key;
  ptinIgmpPairInfoData_t *avl_info;
  L7_RC_t rc = L7_SUCCESS;

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinIgmpPairDataKey_t));

  while ( ( avl_info = (ptinIgmpPairInfoData_t *)
                        avlSearchLVL7(&igmpPairDB.igmpPairAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->igmpPairDataKey, sizeof(ptinIgmpPairDataKey_t));

    /* Check if this node will be removed */
    if (avl_info->evc_mc == evc_mc
      #if IGMPASSOC_CHANNEL_UC_EVC_ISOLATION
        && avl_info->evc_uc == evc_uc
      #endif
       )
    {
      /* Remove key */
      if ( avlDeleteEntry(&(igmpPairDB.igmpPairAvlTree), (void *)&avl_key) == L7_NULLPTR )
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing group channel 0x%08x to EVC_MC=%u, EVC=UC=%u",
                avl_key.channel_group.addr.ipv4.s_addr, evc_mc, evc_uc);
        rc = L7_FAILURE;
      }
      else
      {
        if (igmpPairDB.number_of_entries>0)
          igmpPairDB.number_of_entries--;

        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removed group channel 0x%08x to EVC_MC=%u, EVC=UC=%u",
                  avl_key.channel_group.addr.ipv4.s_addr, evc_mc, evc_uc);
      }
    }
  }

  return rc;
}

/**
 * Remove all nodes from the IGMPpair tree. 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE;
 */
static L7_RC_t igmp_assoc_avlTree_purge( void )
{
  /* Purge all AVL tree, but the root node */
  avlPurgeAvlTree( &igmpPairDB.igmpPairAvlTree, IGMPASSOC_CHANNELS_MAX );

  /* No entries */
  igmpPairDB.number_of_entries = 0;

  return L7_SUCCESS;
}

#endif

/**
 * Configure an IGMP evc with the necessary procedures 
 * 
 * @param evc_idx   : evc index
 * @param enable    : enable flag 
 * @param set_trap  : configure trap rule? 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_igmp_evc_configure(L7_uint32 evc_idx, L7_BOOL enable, L7_BOOL set_trap)
{
  #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  /* IGMP instance management already deal with trp rules */
  if (ptin_igmp_is_evc_used(evc_idx))
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Evc index %u is already being used in an IGMP instance",evc_idx);
    return L7_SUCCESS;
  }

  if (set_trap)
  {
    /* Configure trap rule */
    if (ptin_igmp_evc_trap_configure(evc_idx, enable, PTIN_DIR_BOTH) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Evc index %u: Error configuring trap rule to %u",evc_idx,enable);
      return L7_FAILURE;
    }
  }

  /* Only activate queriers if is allowed for UC services */
  #ifdef IGMP_QUERIER_IN_UC_EVC
  /* Configure querier */
  if (ptin_igmp_evc_querier_configure(evc_idx,enable)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Evc index %u: Error configuring querier to %u",evc_idx,enable);
    ptin_igmp_evc_trap_configure(evc_idx, !enable, PTIN_DIR_BOTH);
    return L7_FAILURE;
  }
  #endif
  #endif

#ifndef PTIN_MGMD_MC_SERVICE_ID_IN_USE//This is only applicable when MGMD is configured to used the Unicast Service Id
  /* If we are removing the service, force a clear of all it's records on MGMD as well */
  if ((L7_FALSE == enable) && (ptin_igmp_mgmd_service_remove(evc_idx)!=L7_SUCCESS))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Evc index %u: Unable to remove service from MGMD",evc_idx);
    return L7_FAILURE;
  }
#endif
  return L7_SUCCESS;
}

/****************************************************************************** 
 * STATIC FUNCTIONS
 ******************************************************************************/

/**
 * Delete an IGMP instance
 * 
 * @param McastEvcId : Multicast evc id 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_instance_delete(L7_uint16 igmp_idx)
{
  L7_uint32 i;
  L7_uint8 igmp_inst;

  /* Validate arguments */
  if (igmp_idx >= PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid IGMP instance %u", igmp_idx);
    return L7_FAILURE;
  }

  /* IGMP instance must be active */
  if (!igmpInstances[igmp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"IGMP instance %u is not active", igmp_idx);
    return L7_SUCCESS;
  }

  /* Deconfigure querier for this instance */
  if (ptin_igmp_querier_configure(igmp_idx,L7_DISABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error clearing querier configuration for igmp_idx=%u",igmp_idx);
    return L7_FAILURE;
  }

  /* Configure querier for this instance */
  if (ptin_igmp_trap_configure(igmp_idx, L7_DISABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring IGMP snooping for igmp_idx=%u",igmp_idx);
    ptin_igmp_querier_configure(igmp_idx,L7_ENABLE);
    return L7_FAILURE;
  }

  /* Clear data and free instance */
  igmpInstances[igmp_idx].McastEvcId      = 0;
  igmpInstances[igmp_idx].UcastEvcId      = 0;
  igmpInstances[igmp_idx].nni_ovid        = 0;
  igmpInstances[igmp_idx].n_evcs          = 0;
  igmpInstances[igmp_idx].inUse           = L7_FALSE;

  /* Reset direct referencing to igmp index from evc ids */
  for (i=0; i<PTIN_SYSTEM_N_EXTENDED_EVCS; i++)
  {
    if (ptin_evc_igmpInst_get(i, &igmp_inst) == L7_SUCCESS &&
        igmp_inst == igmp_idx)
    {
      ptin_evc_igmpInst_set(i, IGMP_INVALID_ENTRY);
    }
  }

  return L7_SUCCESS;
}

/**
 * Clean child clients belonging to a client group
 * 
 * @param avl_infoData_parent : client group 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_clean_deviceClients(ptinIgmpClientGroupInfoData_t *clientGroup)
{
  L7_uint ptin_port, client_idx;
  ptinIgmpClientDevice_t *client_device;

  /* Validate arguments */
  if (clientGroup == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Remove all child clients, belonging to this client group */

  client_device = L7_NULLPTR;
  while ((client_device=igmp_clientDevice_next(clientGroup, client_device)) != L7_NULLPTR)
  {
    /* Validate client index */
    if (client_device->client == L7_NULLPTR || client_device->client->client_index >= PTIN_IGMP_CLIENTIDX_MAX)
      continue;

    /* Client index */
    ptin_port  = client_device->client->ptin_port;
    client_idx = client_device->client->client_index;

    if (ptin_igmp_rm_clientIdx(ptin_port, client_idx, L7_FALSE, L7_TRUE) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing client index %u", client_idx);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Client index %u removed", client_idx);
  }

  return L7_SUCCESS;
}

/**
 * Find clientGroup information in a particulat IGMP instance
 * 
 * @param client_ref  : client group reference
 * @param client_info : client information pointer (output)
 * 
 * @return L7_RC_t : L7_SUCCESS - Client found 
 *                   L7_NOT_EXIST - Client does not exist
 *                   L7_FAILURE - Error
 */
static L7_RC_t ptin_igmp_clientGroup_find(ptin_client_id_t *client_ref, ptinIgmpClientGroupInfoData_t **client_info)
{
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientGroupAvlTree_t  *avl_tree;
  ptinIgmpClientGroupInfoData_t *clientInfo;
  #if (MC_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port;
  #endif

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

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* Key to search for */
  avl_tree = &igmpClientGroups.avlTree;
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
                                "} does not exist"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
    }
    osapiSemaGive(ptin_igmp_clients_sem);
    return L7_NOT_EXIST;
  }

  osapiSemaGive(ptin_igmp_clients_sem);

  /* Return client info */
  if (client_info!=L7_NULLPTR)
  {
    *client_info = clientInfo;
  }

  return L7_SUCCESS;
}

/**
 * Add a new Multicast client
 * 
 * @param client      : client identification parameters 
 * @param intVid      : Internal vlan
 * @param uni_ovid    : External Outer vlan 
 * @param uni_ivid    : External Inner vlan 
 * @param isDynamic   : client type 
 * @param client_idx_ret : client index (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_new_client(ptin_client_id_t *client,
                                    L7_uint16 uni_ovid, L7_uint16 uni_ivid,
                                    L7_BOOL isDynamic, L7_uint *client_idx_ret)
{
  L7_uint client_idx;
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientsAvlTree_t *avl_tree;
  ptinIgmpClientInfoData_t *avl_infoData;
  L7_uint32 ptin_port;

  /* Get ptin_port value */
  ptin_port = 0;
  #if (MC_CLIENT_INTERF_SUPPORTED)
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    /* Convert to ptin_port format */
    if (ptin_intf_ptintf2port(&client->ptin_intf, &ptin_port) != L7_SUCCESS || ptin_port >= PTIN_SYSTEM_N_INTERF)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot convert client intf %u/%u to ptin_port format",
              client->ptin_intf.intf_type, client->ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
  #endif

  /* Check if this key already exists */
  avl_tree = &igmpClients_unified.avlTree;
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
                                "} will be added"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
  }

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* Check if this key already exists */
  if ((avl_infoData=avlSearchLVL7( &(avl_tree->igmpClientsAvlTree), (void *)&avl_key, AVL_EXACT)) == L7_NULLPTR)
  {
    /* To accept this client, the client group must exist */
    /* Only allow clients without its client group, in matrix board */
    ptinIgmpClientDataKey_t avl_key_group;
    ptinIgmpClientGroupAvlTree_t *avl_tree_group;
    ptinIgmpClientGroupInfoData_t *clientGroup = L7_NULLPTR;

    avl_tree_group = &igmpClientGroups.avlTree;

    /* Client group only has port, outer and inner vlan as identification */
    memset(&avl_key_group, 0x00, sizeof(ptinIgmpClientDataKey_t));
    #if (MC_CLIENT_INTERF_SUPPORTED)
    avl_key_group.ptin_port = avl_key.ptin_port;
    #endif
    #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
    avl_key_group.outerVlan = avl_key.outerVlan;
    #endif
    #if (MC_CLIENT_INNERVLAN_SUPPORTED)
    avl_key_group.innerVlan = avl_key.innerVlan;
    #endif

    /* Get client group */
    clientGroup = avlSearchLVL7( &(avl_tree_group->igmpClientsAvlTree), (void *)&avl_key_group, AVL_EXACT);

    #if (!PTIN_BOARD_IS_MATRIX)
    /* If not found the client group, return error */
    if (clientGroup == L7_NULLPTR)
    {
      osapiSemaGive(ptin_igmp_clients_sem);
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Client Group not found!");
      return L7_FAILURE;
    }

    /* Check if can be added more devices */
    if (igmp_clientDevice_get_devices_number(clientGroup) >= PTIN_SYSTEM_IGMP_MAXDEVICES_PER_ONU)
    {
      osapiSemaGive(ptin_igmp_clients_sem);
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot be added more than %u devices!", PTIN_SYSTEM_IGMP_MAXDEVICES_PER_ONU);
      return L7_FAILURE;
    }
    #endif

    /* Check if there is free clients to be allocated (look to free clients queue) */
    if (igmpClients_unified.queue_free_clientDevices[PTIN_IGMP_CLIENT_PORT(ptin_port)].n_elems == 0)
    {
      osapiSemaGive(ptin_igmp_clients_sem);
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"No more free clients available!");
      return L7_FAILURE;
    }

    /* Get new client index */
    if ((client_idx=igmp_clientIndex_get_new(ptin_port)) >= PTIN_IGMP_CLIENTIDX_MAX)
    {
      osapiSemaGive(ptin_igmp_clients_sem);
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot get new client index for ptin_port %u", ptin_port);
      return L7_FAILURE;
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
                                "}"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );

      igmp_clientIndex_free(ptin_port, client_idx);
      osapiSemaGive(ptin_igmp_clients_sem);
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
                                "}"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );

      igmp_clientIndex_free(ptin_port, client_idx);
      osapiSemaGive(ptin_igmp_clients_sem);
      return L7_FAILURE;
    }

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
                                  "} (entry is %s)"
                #if (MC_CLIENT_INTERF_SUPPORTED)
                ,avl_key.ptin_port
                #endif
                #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                ,avl_key.outerVlan
                #endif
                #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                ,avl_key.innerVlan
                #endif
                #if (MC_CLIENT_IPADDR_SUPPORTED)
                ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
                #endif
                #if (MC_CLIENT_MACADDR_SUPPORTED)
                ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
                #endif
                ,((isDynamic) ? "dynamic" : "static"));
    }

    /* Update client index in data cell */
    avl_infoData->ptin_port    = ptin_port;
    avl_infoData->client_index = client_idx;

    /* Save associated vlans */
    avl_infoData->uni_ovid = uni_ovid;
    avl_infoData->uni_ivid = uni_ivid;

    /* Dynamic entry? */
    avl_infoData->isDynamic = isDynamic & 1;

    /* Update client group data */
    /* Client idx information */
    avl_infoData->pClientGroup = clientGroup;

    if (clientGroup != L7_NULLPTR)
    {
      /* Do not clear igmp statistics */
      #if 0
      osapiSemaTake(ptin_igmp_stats_sem,L7_WAIT_FOREVER);
      memset(&clientGroup->stats_client, 0x00, sizeof(ptin_IGMP_Statistics_t));
      osapiSemaGive(ptin_igmp_stats_sem);
      #endif

      /* Add device to client group */
      igmp_clientDevice_add(clientGroup, avl_infoData);
    }

    /* Mark one more client for unified list of clients */
    igmp_clientIndex_mark(ptin_port, client_idx, avl_infoData);
  }
  else
  {
    ptin_port  = avl_infoData->ptin_port;
    client_idx = avl_infoData->client_index;

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
                                    "} already exists (ptin_port=%u client_idx=%u)"
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                  ,avl_key.ptin_port
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                  ,avl_key.outerVlan
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                  ,avl_key.innerVlan
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                  ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                  ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
                  #endif
                  ,avl_infoData->ptin_port
                  ,avl_infoData->client_index);
    }

    /* If new type is static, always change to it */
    if (!isDynamic)
    {
      avl_infoData->isDynamic = L7_FALSE;
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Entry is static!");
    }
  }

  osapiSemaGive(ptin_igmp_clients_sem);

  /* Output client index */
  if (client_idx_ret!=L7_NULLPTR)
  {
    *client_idx_ret = avl_infoData->client_index;
  }

  return L7_SUCCESS;
}

#if 0
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
  L7_uint32 intIfNum;
  L7_uint   client_idx;
  ptinIgmpClientDataKey_t   avl_key;
  ptinIgmpClientsAvlTree_t *avl_tree;
  ptinIgmpClientInfoData_t *avl_infoData;
  ptinIgmpClientGroupInfoData_t *clientGroup = L7_NULLPTR;
  L7_uint32 ptin_port;

  /* Convert interface to ptin_port format */
  intIfNum = 0;
  ptin_port = 0;
  #if (MC_CLIENT_INTERF_SUPPORTED)
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

  avl_tree = &igmpClients_unified.avlTree;
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
                                "}"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
  }

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

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
                                    "} does not exist"
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                  ,avl_key.ptin_port
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                  ,avl_key.outerVlan
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                  ,avl_key.innerVlan
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                  ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                  ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
                  #endif
                 );
    }
    osapiSemaGive(ptin_igmp_clients_sem);
    return L7_NOT_EXIST;
  }

  /* Extract client index */
  client_idx = avl_infoData->client_index;

#ifdef CLIENT_TIMERS_SUPPORTED
  /* Stop timers related to this client */
  if (ptin_igmp_timer_stop(0 /*Not used*/, client_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Error stoping timer for client_idx=%u)",client_idx);
    //osapiSemaGive(ptin_igmp_clients_sem);
    //return L7_FAILURE;
  }
#endif

  /* Only remove entry if it is dynamic, or else, if the remove_static flag is given */
  if (remove_static || avl_infoData->isDynamic)
  {
    /* Save group client reference */
    clientGroup = avl_infoData->pClientGroup;

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
                                "}"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
      osapiSemaGive(ptin_igmp_clients_sem);
      return L7_FAILURE;
    }

    /* Remove device from client group */
    if (clientGroup != L7_NULLPTR)
    {  
      igmp_clientDevice_remove(clientGroup, avl_infoData);
    }

    /* Remove client for AVL tree */
    igmp_clientIndex_unmark(client_idx, ptin_port);

    /* Free client index */
    igmp_clientIndex_free(client_idx);

    osapiSemaGive(ptin_igmp_clients_sem);

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
                                  "}"
                #if (MC_CLIENT_INTERF_SUPPORTED)
                ,avl_key.ptin_port
                #endif
                #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                ,avl_key.outerVlan
                #endif
                #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                ,avl_key.innerVlan
                #endif
                #if (MC_CLIENT_IPADDR_SUPPORTED)
                ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
                #endif
                #if (MC_CLIENT_MACADDR_SUPPORTED)
                ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
                #endif
               );
    }
  }

  /* Remove client from all snooping entries */
  if (ptin_snoop_client_remove(0 /*All vlans*/, client_idx, intIfNum)!=L7_SUCCESS)
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
#endif

/**
 * Remove all Multicast clients 
 * 
 * @param isDynamic   : Remove only this type of clients 
 *                      (static=0/dynamic=1)
 * @param only_wo_channels:only remove clients without channels.
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_rm_all_clients(L7_BOOL isDynamic, L7_BOOL only_wo_channels)
{
  L7_uint   client_idx;
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientsAvlTree_t *avl_tree;
  ptinIgmpClientInfoData_t *avl_infoData;
  ptinIgmpClientGroupInfoData_t *clientGroup = L7_NULLPTR;
  L7_uint   ptin_port;
  L7_uint32 intIfNum;
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* AVL tree refrence */
  avl_tree = &igmpClients_unified.avlTree;

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
    if (only_wo_channels &&
        (avl_infoData->pClientGroup==L7_NULLPTR || avl_infoData->pClientGroup->stats_client.active_groups>0))
      continue;    

    /* Save client index */
    ptin_port  = avl_infoData->ptin_port;
    client_idx = avl_infoData->client_index;

    /* Convert interface port to intIfNum format */
    if (ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot convert client port %u to intIfNum format", ptin_port);
      continue;
    }

  #ifdef CLIENT_TIMERS_SUPPORTED
    /* Stop timers */
    if (ptin_igmp_timer_stop(ptin_port, client_idx)!=L7_SUCCESS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Error stoping timer for client_idx=%u)",client_idx);
      //rc = L7_FAILURE;
    }
  #endif

    /* Save group client reference */
    clientGroup = avl_infoData->pClientGroup;

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
                                "}"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
        rc = L7_FAILURE;
    }
    else
    {
      if (clientGroup != L7_NULLPTR)
      {
        /* Remove device from client group */
        igmp_clientDevice_remove(clientGroup, avl_infoData);
      }

      /* Remove client from unified list of clients */
      igmp_clientIndex_unmark(ptin_port, client_idx);

      /* Free client index */
      igmp_clientIndex_free(ptin_port, client_idx);

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
                                    "}"
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                  ,avl_key.ptin_port
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                  ,avl_key.outerVlan
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                  ,avl_key.innerVlan
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                  ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                  ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
                  #endif
                 );
      }

      if (!only_wo_channels)
      {
        /* Remove client from all snooping entries */
        if (ptin_snoop_client_remove(0 /*All vlans*/, client_idx, intIfNum) != L7_SUCCESS)
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

  osapiSemaGive(ptin_igmp_clients_sem);

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
 * @param ptin_port     : Interface port
 * @param client_index  : Client index 
 * @param remove_static : Remove all types of clients
 * @param force_remove  : Only remove if the client is not 
 *                        watching any channel
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_rm_clientIdx(L7_uint ptin_port, L7_uint client_idx, L7_BOOL remove_static, L7_BOOL force_remove )
{
  L7_uint32 intIfNum;
  ptinIgmpClientDataKey_t  *avl_key;
  ptinIgmpClientsAvlTree_t *avl_tree;
  ptinIgmpClientInfoData_t *clientInfo;
  ptinIgmpClientGroupInfoData_t *clientGroup = L7_SUCCESS;
  L7_RC_t rc;

  if (ptin_port >= PTIN_SYSTEM_N_INTERF || client_idx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    //if (ptin_debug_igmp_snooping)
    //  LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get pointer to client structure in AVL tree */
  clientInfo = igmpClients_unified.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;

  /* If does not exist... */
  if (clientInfo==L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Provided ptin_port=%u client_idx=%u does not exist", ptin_port, client_idx);
    return L7_FAILURE;
  }

  /* Get intIfNum related to this client */
  intIfNum = 0;
  if ( ptin_intf_port2intIfNum(ptin_port, &intIfNum)!=L7_SUCCESS )
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"Error getting intIfNum for ptin_port %u", ptin_port);
    rc = L7_FAILURE;
  }

  /* If there is no channels, or channels are forced to be removed... */
  if ( force_remove ||
       (clientInfo->pClientGroup!=L7_NULLPTR && clientInfo->pClientGroup->stats_client.active_groups==0) )
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Stopping timer");

    /* Do not insert clients semaphore control here... calling functions already do that! */

  #ifdef CLIENT_TIMERS_SUPPORTED
    /* Stop timers related to this client */
    if (ptin_igmp_timer_stop(ptin_port, client_idx)!=L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Cannot stop timer for ptin_port=%u client_idx=%u", ptin_port, client_idx);
      //return L7_FAILURE;
    }
  #endif

    /* Remove client if channel is dynamic, or if remove_static flag is given */
    if ( remove_static || clientInfo->isDynamic )
    {
      /* Do not insert clients semaphore control here... calling functions already do that! */

      avl_key  = (ptinIgmpClientDataKey_t *) &clientInfo->igmpClientDataKey;
      avl_tree = &igmpClients_unified.avlTree;

      /* Save client group reference */
      clientGroup = clientInfo->pClientGroup;

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
                                    "}"
                  #if (MC_CLIENT_INTERF_SUPPORTED)
                  ,avl_key->ptin_port
                  #endif
                  #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
                  ,avl_key->outerVlan
                  #endif
                  #if (MC_CLIENT_INNERVLAN_SUPPORTED)
                  ,avl_key->innerVlan
                  #endif
                  #if (MC_CLIENT_IPADDR_SUPPORTED)
                  ,(avl_key->ipv4_addr>>24) & 0xff, (avl_key->ipv4_addr>>16) & 0xff, (avl_key->ipv4_addr>>8) & 0xff, avl_key->ipv4_addr & 0xff
                  #endif
                  #if (MC_CLIENT_MACADDR_SUPPORTED)
                  ,avl_key->macAddr[0],avl_key->macAddr[1],avl_key->macAddr[2],avl_key->macAddr[3],avl_key->macAddr[4],avl_key->macAddr[5]
                  #endif
                 );
        return L7_FAILURE;
      }

      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to unmark ptin_port=%u client_idx=%u", ptin_port, client_idx);

      /* Remove device from client group */
      if (clientGroup != L7_NULLPTR)
      {
        igmp_clientDevice_remove(clientGroup, clientInfo);
      }

      /* Remove client from unified list of clients */
      igmp_clientIndex_unmark(ptin_port, client_idx);

      /* Free client index */
      igmp_clientIndex_free(ptin_port, client_idx);
    }
  }

  /* Only clean channels associated to this client, if force_remove flag is given */
  if ( force_remove )
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to remove channels");

    rc = L7_SUCCESS;

    if ( rc == L7_SUCCESS )
    {
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Proceeding for snoop channels remotion: client_idx=%u, intIfNum=%u (port=%u)",
                  client_idx, intIfNum, ptin_port);

//    /* Remove client from all snooping entries */
//    if (ptin_snoop_client_remove(0 /*All vlans*/,client_idx,intIfNum)!=L7_SUCCESS)
//    {
//      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing client from snooping entries");
//    }
//    else
//    {
//      if (ptin_debug_igmp_snooping)
//        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Client removed from snooping entries");
//    }
    }
    else
    {
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Cannot proceed to snoop channels remotion");
    }
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success flushing client (client_idx=%u)", client_idx);

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
  L7_uint32 evc_idx;
  L7_uint8  igmp_idx;

  /* Verify if this internal vlan is associated to an EVC */
  if (ptin_evc_get_evcIdfromIntVlan(intVlan,&evc_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No EVC associated to internal vlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Check if the EVC has a IGMP instance */
  if (ptin_evc_igmpInst_get(evc_idx, &igmp_idx) != L7_SUCCESS ||
      igmp_idx >= PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
//  if (ptin_debug_igmp_snooping)
//    LOG_ERR(LOG_CTX_PTIN_IGMP,"No IGMP instance associated to evc_idx=%u (intVlan=%u)",evc_idx,intVlan);
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
  if (!ptin_evc_is_in_use(igmpInstances[igmp_idx].McastEvcId)
      #if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
      || !ptin_evc_is_in_use(igmpInstances[igmp_idx].UcastEvcId)
      #endif
     )
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Inconsistency: IGMP index %u (EVCid=%u, Vlan %u) has EVCs not in use (MC=%u)",igmp_idx,evc_idx,intVlan,igmpInstances[igmp_idx].McastEvcId);
    return L7_FAILURE;
  }

  /* Return igmp instance */
  if (igmpInst!=L7_NULLPTR)     *igmpInst     = &igmpInstances[igmp_idx];
  if (igmpInst_idx!=L7_NULLPTR) *igmpInst_idx = igmp_idx;

  return L7_SUCCESS;
}

/** 
 * 
 * 
 * 
 * @return L7_RC_t
 */
//static L7_RC_t ptin_igmp_global_configuration(void)
//{
//  L7_uint igmp_idx;
//
//  if (igmpProxyCfg.admin)
//  {
//    // Querier IP address
//    if (usmDbSnoopQuerierAddressSet( (void *) &igmpProxyCfg.ipv4_addr, L7_AF_INET)!=L7_SUCCESS)  {
//      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopQuerierAddressSet");
//      return L7_FAILURE;
//    }
//    // IGMP version
//    if (usmDbSnoopQuerierVersionSet( igmpProxyCfg.clientVersion, L7_AF_INET)!=L7_SUCCESS)  {
//      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopQuerierVersionSet");
//      return L7_FAILURE;
//    }
//    // Querier interval
//    if (usmDbSnoopQuerierQueryIntervalSet( igmpProxyCfg.querier.query_interval, L7_AF_INET)!=L7_SUCCESS)  {
//      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopQuerierQueryIntervalSet");
//      return L7_FAILURE;
//    }
//
//    LOG_TRACE(LOG_CTX_PTIN_IGMP,"IPv4 address set to %u.%u.%u.%u",
//              (igmpProxyCfg.ipv4_addr.s_addr>>24) & 0xff,
//              (igmpProxyCfg.ipv4_addr.s_addr>>16) & 0xff,
//              (igmpProxyCfg.ipv4_addr.s_addr>> 8) & 0xff,
//               igmpProxyCfg.ipv4_addr.s_addr & 0xff);
//    LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMP version set to %u",igmpProxyCfg.clientVersion);
//    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Querier interval set to %u",igmpProxyCfg.querier.query_interval);
//  }
//
//  // Attrib IGMP packets priority
//  if ( usmDbSnoopPrioModeSet(1, igmpProxyCfg.igmp_cos, L7_AF_INET) != L7_SUCCESS )
//  {
//    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopPrioModeSet");
//    return L7_FAILURE;
//  }
//
//  // Snooping global activation
//  if (usmDbSnoopAdminModeSet( 1, igmpProxyCfg.admin, L7_AF_INET)!=L7_SUCCESS)  {
//    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopAdminModeSet");
//    return L7_FAILURE;
//  }
//
//  // Querier admin
//  if (usmDbSnoopQuerierAdminModeSet(igmpProxyCfg.admin, L7_AF_INET)!=L7_SUCCESS)  {
//    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopQuerierAdminModeSet");
//    if (igmpProxyCfg.admin)  usmDbSnoopAdminModeSet( 1, L7_DISABLE, L7_AF_INET);
//    return L7_FAILURE;
//  }
//  LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMP snooping enable set to %u",igmpProxyCfg.admin);
//
//  /* Run all IGMP instances and reset the IP address for each one */
//  for (igmp_idx=0; igmp_idx<PTIN_SYSTEM_N_IGMP_INSTANCES; igmp_idx++)
//  {
//    if (igmpInstances[igmp_idx].inUse)
//    {
//      if (ptin_igmp_querier_configure(igmp_idx,L7_ENABLE)!=L7_SUCCESS)
//        return L7_FAILURE;
//    }
//  }
//
//  return L7_SUCCESS;
//}

static L7_RC_t ptin_igmp_trap_configure(L7_uint igmp_idx, L7_BOOL enable)
{
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMP index %u",igmp_idx);

  /* Validate argument */
  if (igmp_idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid igmp instance index (%u)",igmp_idx);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMP index %u",igmp_idx);

  /* IGMP instance must be in use */
  if (!igmpInstances[igmp_idx].inUse)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMP index %u",igmp_idx);
    LOG_ERR(LOG_CTX_PTIN_IGMP,"igmp instance index %u is not in use",igmp_idx);
    return L7_FAILURE;
  }

  return ptin_igmp_evc_trap_set(igmpInstances[igmp_idx].McastEvcId, igmpInstances[igmp_idx].UcastEvcId, enable);
}

static L7_RC_t ptin_igmp_evc_trap_set(L7_uint32 evc_idx_mc, L7_uint32 evc_idx_uc, L7_BOOL enable)
{
  L7_uint16   idx, mc_vlan;
  L7_uint16 vlans_number, vlan_list[PTIN_SYSTEM_MAX_N_PORTS];
  L7_uint32 flags, mc_flood;

  enable &= 1;

  /* Initialize number of vlans to be configured */
  vlans_number = 0;

  /* Get root vlan for MC evc, and add it for packet trapping */
  if (ptin_evc_intRootVlan_get(evc_idx_mc, &mc_vlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Can't get MC root vlan for evc id %u",evc_idx_mc);
    return L7_FAILURE;
  }
  if (mc_vlan>=PTIN_VLAN_MIN && mc_vlan<=PTIN_VLAN_MAX)
  {
    vlan_list[vlans_number++] = mc_vlan;
  }

#if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
  ptin_HwEthMef10Evc_t evcCfg;
  L7_uint16 vlan;

  /* Get Unicast EVC configuration */
  evcCfg.index = evc_idx_uc;
  if (ptin_evc_get(&evcCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting UC EVC %u configuration",evc_idx_uc);
    return L7_FAILURE;
  }

  if (ptin_evc_intRootVlan_get(evc_idx_uc,&vlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Can't get UC root vlan for evc id %u",evc_idx_uc);
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

  /* Disable/Reenable Multicast rate limit */
  if (ptin_evc_flags_get(evc_idx_mc, &flags, &mc_flood)==L7_SUCCESS &&
      (flags & PTIN_EVC_MASK_CPU_TRAPPING) && mc_flood)     /* Get EVC options and check if CPU trapping and MC flooding is active */
  {
    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    /* Apply only to non QUATTRO-P2P or unique QUATTRO-P2P evcs */
    if ( !(flags & PTIN_EVC_MASK_QUATTRO) || !(flags & PTIN_EVC_MASK_P2P) || (igmp_quattro_p2p_evcs <= 1) )
    #endif
    {
      #if 0
      /* If multicast rate limit is disabled, broadcast rate limiter should be enabled */
      if (enable)
      {
        ptin_multicast_rateLimit(L7_DISABLE, mc_vlan);
        ptin_broadcast_rateLimit(L7_ENABLE , mc_vlan);
      }
      /* And vice-versa */
      else
      {
        ptin_broadcast_rateLimit(L7_DISABLE, mc_vlan);
        ptin_multicast_rateLimit(L7_ENABLE , mc_vlan);
      }
      #endif
    }
  }

  return L7_SUCCESS;
}


#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
/**
 * Configure an IGMP vlan trapping rule (most essentally for the
 * UC services) 
 * 
 * @param evc_idx   : evc index
 * @param enable    : enable flag 
 * @param direction : Ports to be considered (PTIN_DIR_UPLINK, 
 *                    PTIN_DIR_DOWNLINK, PTIN_DIR_BOTH).
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_igmp_evc_trap_configure(L7_uint32 evc_idx, L7_BOOL enable, ptin_dir_t direction)
{
  L7_uint16   idx, vlan;
  L7_uint16 vlans_number, vlan_list[PTIN_SYSTEM_MAX_N_PORTS];
  ptin_HwEthMef10Evc_t evcCfg;

  /* IGMP instance management already deal with trp rules */
  if (ptin_igmp_is_evc_used(evc_idx))
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Evc index %u is already being used in an IGMP instance",evc_idx);
    return L7_SUCCESS;
  }

  enable &= 1;

  /* Validate argument */
  if (evc_idx>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid evc index %u",evc_idx);
    return L7_FAILURE;
  }
  /* Check if EVC is in use */
  if ( !ptin_evc_is_in_use(evc_idx) )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC  %u is not in use",evc_idx);
    return L7_FAILURE;
  }

  /* Initialize number of vlans to be configured */
  vlans_number = 0;

  /* Get EVC configuration */
  evcCfg.index = evc_idx;
  if (ptin_evc_get(&evcCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting EVC %u configuration",evc_idx);
    return L7_FAILURE;
  }

  /* Only for uplink ports, or both */
  if ( direction == PTIN_DIR_UPLINK || direction == PTIN_DIR_BOTH )
  {
    /* Configure root vlan (stacked and unstacked services) */
    if (ptin_evc_intRootVlan_get(evc_idx, &vlan)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Can't get root vlan for evc id %u",evc_idx);
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

  /* Configure vlans */
  for (idx=0; idx<vlans_number; idx++)
  {
    if (usmDbSnoopVlanModeSet(1, vlan_list[idx], enable, L7_AF_INET) != L7_SUCCESS)
    //if (ptin_igmpPkts_vlan_trap(vlan_list[idx], enable)!=L7_SUCCESS)
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
      usmDbSnoopVlanModeSet(1, vlan_list[idx], !enable, L7_AF_INET);
      //ptin_igmpPkts_vlan_trap(vlan_list[idx],!enable);
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"Unconfiguring vlan %u for packet trapping",vlan_list[idx]);
    }
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "%s IGMP trap rules for evc %u", ((enable) ? "Added" : "Removed"), evc_idx);

  return L7_SUCCESS;
}
#endif

static L7_RC_t ptin_igmp_querier_configure(L7_uint igmp_idx, L7_BOOL enable)
{
  L7_RC_t rc = L7_SUCCESS;

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

  /* Only configure MC querier, if no specific querier is provided */
  //#if (PTIN_BOARD_IS_MATRIX || (!defined (IGMP_QUERIER_IN_MC_EVC) && !defined (IGMP_QUERIER_IN_UC_EVC)))
  #if (PTIN_BOARD_IS_MATRIX || (defined (IGMP_QUERIER_IN_MC_EVC) || !defined (IGMP_QUERIER_IN_UC_EVC)))
  if ((rc = ptin_igmp_evc_querier_configure(igmpInstances[igmp_idx].McastEvcId, enable)) != L7_SUCCESS)
  {
    return rc;
  }
  #endif

  /* If querier uses UC service, and Multi-MC is disabled */
  #if (defined (IGMP_QUERIER_IN_UC_EVC) && !defined IGMPASSOC_MULTI_MC_SUPPORTED)
  if ((rc = ptin_igmp_evc_querier_configure(igmpInstances[igmp_idx].UcastEvcId, enable)) != L7_SUCCESS)
  {
    return rc;
  }
  #endif

  return rc;
}

#if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
void* ptin_mgmd_query_instances_get(void)
{
  return ((void*) &mgmdQueryInstances);  
}

L7_uint16 ptin_mgmd_number_of_query_instances_get(void)
{
  return mgmdNumberOfQueryInstances;  
}
#endif

static L7_RC_t ptin_igmp_evc_querier_configure(L7_uint32 evc_idx, L7_BOOL enable)
{
  PTIN_MGMD_EVENT_t             reqMsg       = {0};
  PTIN_MGMD_EVENT_t             resMsg       = {0};
  PTIN_MGMD_EVENT_CTRL_t        ctrlResMsg   = {0};
  PTIN_MGMD_CTRL_QUERY_CONFIG_t mgmdStatsMsg = {0}; 

  enable &= 1;

  /* Send configurations to MGMD */
  mgmdStatsMsg.admin     = enable;
  mgmdStatsMsg.serviceId = evc_idx;
  mgmdStatsMsg.family    = L7_AF_INET;
  ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_GENERAL_QUERY_ADMIN, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdStatsMsg, sizeof(PTIN_MGMD_CTRL_QUERY_CONFIG_t));
  ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
  ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Res     : %u",   ctrlResMsg.res);

  //Save the unicast EVC Id on which this Query was configured.
  // This is required for the Group Specific Queries
  #if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
  if(ctrlResMsg.res==L7_SUCCESS)
  {
    L7_uint16 iterator;
    for (iterator=0; iterator<PTIN_SYSTEM_N_EVCS; iterator++)
    {
      if (enable==L7_TRUE)
      {
        if(mgmdQueryInstances[iterator].inUse==L7_FALSE)
        {
          mgmdQueryInstances[iterator].UcastEvcId=evc_idx;
          mgmdQueryInstances[iterator].inUse=L7_TRUE;
          ++mgmdNumberOfQueryInstances;
          break;
        }
      }
      else
      {
        if(mgmdQueryInstances[iterator].inUse==L7_TRUE && mgmdQueryInstances[iterator].UcastEvcId==evc_idx)
        {          
          mgmdQueryInstances[iterator].inUse=L7_FALSE;
          --mgmdNumberOfQueryInstances;
          break;
        }
      }
    }
  }
  #endif
  //End Save

  return ctrlResMsg.res;
}

/**
 * Removes all groups related to this Service ID
 * 
 * @param EvcId : Multicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_mgmd_service_remove(L7_uint32 evc_idx)
{
  PTIN_MGMD_EVENT_t               reqMsg        = {0};
  PTIN_MGMD_EVENT_t               resMsg        = {0};
  PTIN_MGMD_EVENT_CTRL_t          ctrlResMsg    = {0};
  PTIN_MGMD_CTRL_SERVICE_REMOVE_t mgmdConfigMsg = {0}; 

  mgmdConfigMsg.serviceId = evc_idx;
  ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_SERVICE_REMOVE, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdConfigMsg, sizeof(PTIN_MGMD_CTRL_SERVICE_REMOVE_t));
  ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
  ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Res     : %u",   ctrlResMsg.res);

  return ctrlResMsg.res;
}

L7_RC_t ptin_igmp_mgmd_whitelist_add(L7_uint16 serviceId, L7_uint32 groupAddr, L7_uint8 groupMaskLen, L7_uint32 sourceAddr, L7_uint8 sourceMaskLen)
{
  PTIN_MGMD_EVENT_t                 reqMsg        = {0};
  PTIN_MGMD_EVENT_t                 resMsg        = {0};
  PTIN_MGMD_EVENT_CTRL_t            ctrlResMsg    = {0};
  PTIN_MGMD_CTRL_WHITELIST_CONFIG_t mgmdConfigMsg = {0}; 

  mgmdConfigMsg.serviceId = serviceId;
  mgmdConfigMsg.groupIp   = groupAddr;
  mgmdConfigMsg.groupMaskLen = groupMaskLen;
  mgmdConfigMsg.sourceIp  = sourceAddr;
  mgmdConfigMsg.sourceMaskLen= sourceMaskLen;

  ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_WHITELIST_ADD, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdConfigMsg, sizeof(PTIN_MGMD_CTRL_WHITELIST_CONFIG_t));
  ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
  ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Res     : %u",   ctrlResMsg.res);

  return ctrlResMsg.res;
}

L7_RC_t ptin_igmp_mgmd_whitelist_remove(L7_uint16 serviceId, L7_uint32 groupAddr, L7_uint8 groupMaskLen, L7_uint32 sourceAddr, L7_uint8 sourceMaskLen)
{
  PTIN_MGMD_EVENT_t                 reqMsg        = {0};
  PTIN_MGMD_EVENT_t                 resMsg        = {0};
  PTIN_MGMD_EVENT_CTRL_t            ctrlResMsg    = {0};
  PTIN_MGMD_CTRL_WHITELIST_CONFIG_t mgmdConfigMsg = {0}; 

  mgmdConfigMsg.serviceId     = serviceId;
  mgmdConfigMsg.groupIp       = groupAddr;
  mgmdConfigMsg.groupMaskLen  = groupMaskLen;
  mgmdConfigMsg.sourceIp      = sourceAddr;
  mgmdConfigMsg.sourceMaskLen = sourceMaskLen;
  ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_WHITELIST_REMOVE, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdConfigMsg, sizeof(PTIN_MGMD_CTRL_WHITELIST_CONFIG_t));
  ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
  ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Res     : %u",   ctrlResMsg.res);

  return ctrlResMsg.res;
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
 * Gets the IGMP instance from the NNI ovlan
 * 
 * @param nni_ovlan  : NNI outer vlan 
 * @param igmp_idx   : IGMP instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_uint ptin_igmp_instance_find_agg(L7_uint16 nni_ovlan, L7_uint *igmp_idx)
{
  L7_uint idx;

  /* Search for the provided Mcast and Ucast evcs */
  for (idx=0; idx<PTIN_SYSTEM_N_IGMP_INSTANCES; idx++)
  {
    if (!igmpInstances[idx].inUse)  continue;

    if (igmpInstances[idx].nni_ovid == nni_ovlan)
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
 * Gets the IGMP instance with a specific Mcast and Ucast EVC 
 * ids 
 * 
 * @param McastEvcId : Multicast EVC id
 * @param UcastEvcId : Unicast EVC id
 * @param igmp_idx   : IGMP instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_instance_find(L7_uint32 McastEvcId, L7_uint32 UcastEvcId, L7_uint *igmp_idx)
{
  #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  L7_uint8 igmp_inst;

  if (McastEvcId >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    return L7_FAILURE;
  }

  if (ptin_evc_igmpInst_get(McastEvcId, &igmp_inst) != L7_SUCCESS ||
      igmp_inst >= PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    return L7_FAILURE;
  }
  if (igmp_idx != L7_NULLPTR)  *igmp_idx = igmp_inst;
  #else
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
  #endif

  return L7_SUCCESS;
}

/**
 * Gets the IGMP instance wich is using an EVC id
 * 
 * @param evc_idx    : EVC id
 * @param igmp_idx : IGMP instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_instance_find_fromSingleEvcId(L7_uint32 evc_idx, L7_uint *igmp_idx)
{
  #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  L7_uint8 igmp_inst;

  if (evc_idx >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    return L7_FAILURE;
  }

  if (ptin_evc_igmpInst_get(evc_idx, &igmp_inst) != L7_SUCCESS ||
      igmp_inst >= PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    return L7_FAILURE;
  }
  if (igmp_idx != L7_NULLPTR)  *igmp_idx = igmp_inst;
  #else

  L7_uint idx;

  /* Search for the provided Mcast and Ucast evcs */
  for (idx=0; idx<PTIN_SYSTEM_N_IGMP_INSTANCES; idx++)
  {
    if (!igmpInstances[idx].inUse)  continue;

    if (igmpInstances[idx].McastEvcId==evc_idx
        || igmpInstances[idx].UcastEvcId==evc_idx
       )
      break;
  }

  /* If not found empty instances, return error */
  if (idx>=PTIN_SYSTEM_N_IGMP_INSTANCES)
    return L7_FAILURE;

  /* Return instance index */
  if (igmp_idx!=L7_NULLPTR)  *igmp_idx = idx;
  #endif

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
static L7_RC_t ptin_igmp_instance_find_fromMcastEvcId(L7_uint32 McastEvcId, L7_uint *igmp_idx)
{
  #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  L7_uint8 igmp_inst;

  if (McastEvcId >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    return L7_FAILURE;
  }

  if (ptin_evc_igmpInst_get(McastEvcId, &igmp_inst) != L7_SUCCESS ||
      igmp_inst >= PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    return L7_FAILURE;
  }
  if (igmp_idx != L7_NULLPTR)  *igmp_idx = igmp_inst;
  #else

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
  #endif

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
static L7_BOOL ptin_igmp_instance_conflictFree(L7_uint32 McastEvcId, L7_uint32 UcastEvcId)
{
  L7_uint idx;

  /* Search for the provided Mcast and Ucast evcs */
  for (idx=0; idx<PTIN_SYSTEM_N_IGMP_INSTANCES; idx++)
  {
    if (!igmpInstances[idx].inUse)  continue;

    if (igmpInstances[idx].McastEvcId==McastEvcId
        #if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
        || igmpInstances[idx].UcastEvcId==UcastEvcId
        || igmpInstances[idx].McastEvcId==UcastEvcId
        || igmpInstances[idx].UcastEvcId==McastEvcId
        #endif
       )
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
 * @param client_ref  : client reference
 * @param client_info : client information pointer (output)
 * 
 * @return L7_RC_t : L7_SUCCESS - Client found 
 *                   L7_NOT_EXIST - Client does not exist
 *                   L7_FAILURE - Error
 */
static L7_RC_t ptin_igmp_client_find(ptin_client_id_t *client_ref, ptinIgmpClientInfoData_t **client_info)
{
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientsAvlTree_t *avl_tree;
  ptinIgmpClientInfoData_t *clientInfo;
  #if (MC_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port;
  #endif

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

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* Key to search for */
  avl_tree = &igmpClients_unified.avlTree;
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
                                "} does not exist"
              #if (MC_CLIENT_INTERF_SUPPORTED)
              ,avl_key.ptin_port
              #endif
              #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
              ,avl_key.outerVlan
              #endif
              #if (MC_CLIENT_INNERVLAN_SUPPORTED)
              ,avl_key.innerVlan
              #endif
              #if (MC_CLIENT_IPADDR_SUPPORTED)
              ,(avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff
              #endif
              #if (MC_CLIENT_MACADDR_SUPPORTED)
              ,avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5]
              #endif
             );
    }
    osapiSemaGive(ptin_igmp_clients_sem);
    return L7_NOT_EXIST;
  }

  osapiSemaGive(ptin_igmp_clients_sem);

  /* Return client info */
  if (client_info!=L7_NULLPTR)
  {
    *client_info = clientInfo;
  }

  return L7_SUCCESS;
}

/**
 * Validate and rearrange client id info. 
 * This should only be applied for client infos coming from 
 * manager. 
 * 
 * @author mruas (8/6/2013)
 * 
 * @param client : client info
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_igmp_clientId_convert(L7_uint32 evc_idx, ptin_client_id_t *client)
{
  L7_uint16 intVlan, innerVlan;

  /* Validate evc index  */
  if (evc_idx>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid eEVC id: evc_idx=%u",evc_idx);
    return L7_FAILURE;
  }
  /* This evc must be active */
  if (!ptin_evc_is_in_use(evc_idx))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"eEVC id is not active: evc_idx=%u",evc_idx);
    return L7_FAILURE;
  }
  /* Validate client */
  if (client==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client pointer");
    return L7_FAILURE;
  }

  /* Check mask */
  if (MC_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Client mask is null");
    return L7_FAILURE;
  }

  innerVlan = 0;
  /* Validate inner vlan */
  #if MC_CLIENT_INNERVLAN_SUPPORTED
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN)
  {
    /* Validate inner vlan */
    if (client->innerVlan>4095)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid inner vlan (%u)",client->innerVlan);
      return L7_FAILURE;
    }
    innerVlan = client->innerVlan;
  }
  #endif

  /* Update outer vlan */
  #if defined(MC_CLIENT_INTERF_SUPPORTED) && defined(MC_CLIENT_OUTERVLAN_SUPPORTED)
  /* Is interface and outer vlan provided? If so, replace it with the internal vlan */
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF &&
      client->mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN)
  {
    /* Validate outer vlan, only if provided */
    if (client->outerVlan<PTIN_VLAN_MIN || client->outerVlan>PTIN_VLAN_MAX)
    {
      /* Obtain intVlan */
      if (ptin_evc_intVlan_get(evc_idx, &client->ptin_intf, &intVlan)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error obtaining internal vlan for evc_idx=%u, ptin_intf=%u/%u",
                evc_idx, client->ptin_intf.intf_type, client->ptin_intf.intf_id);
        return L7_FAILURE;
      }
    }
    else
    {
      /* Obtain intVlan from the outer vlan */
      if (ptin_evc_intVlan_get_fromOVlan(&client->ptin_intf, client->outerVlan, innerVlan, &intVlan)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error obtaining internal vlan for OVid=%u, IVid=%u, ptin_intf=%u/%u",
                client->outerVlan, innerVlan, client->ptin_intf.intf_type, client->ptin_intf.intf_id);
        return L7_FAILURE;
      }
    }
    /* Replace outer vlan with the internal one */
    client->outerVlan = intVlan;
  }
  #endif

  return L7_SUCCESS;
}

/**
 * Restore client id structure to values which manager 
 * understands.
 * 
 * @param client : client id
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_igmp_clientId_restore(ptin_client_id_t *client)
{
  L7_uint32 intIfNum;
  L7_uint16 extVlan, innerVlan;

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
    return L7_FAILURE;
  }

  innerVlan = 0;
  #if MC_CLIENT_INNERVLAN_SUPPORTED
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN)
  {
    /* Validate inner vlan */
    if (client->innerVlan>4095)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid inner vlan (%u)",client->innerVlan);
      return L7_FAILURE;
    }
    innerVlan = client->innerVlan;
  }
  #endif

  #if defined(MC_CLIENT_INTERF_SUPPORTED) && defined(MC_CLIENT_OUTERVLAN_SUPPORTED)
  /* Is interface and outer vlan provided? If so, replace it with the internal vlan */
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF &&
      client->mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN)
  {
    /* Convert to intIfNum format */
    if (ptin_intf_ptintf2intIfNum(&client->ptin_intf, &intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot convert client intf %u/%u to intIfNum format",
              client->ptin_intf.intf_type,client->ptin_intf.intf_id);
      return L7_FAILURE;
    }

    /* Validate outer vlan */
    if (client->outerVlan<PTIN_VLAN_MIN || client->outerVlan>PTIN_VLAN_MAX)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid outer vlan (%u)",client->outerVlan);
      return L7_FAILURE;
    }
    /* Replace the outer vlan, with the internal vlan relative to the leaf interface */
    if (ptin_evc_extVlans_get_fromIntVlan(intIfNum, client->outerVlan, innerVlan, &extVlan, L7_NULLPTR)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not obtain external vlan for intVlan %u, ptin_intf %u/%u",
              client->outerVlan, client->ptin_intf.intf_type, client->ptin_intf.intf_id);
      return L7_FAILURE;
    }
    /* Replace outer vlan with the internal one */
    client->outerVlan = extVlan;
  }
  #endif

  return L7_SUCCESS;
}

/**************************** 
 * IGMP statistics
 ****************************/

/**
 * Get global IGMP statistics
 * 
 * @param intIfNum   : interface
 * @param statistics : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_intf_get(ptin_intf_t *ptin_intf, PTIN_MGMD_CTRL_STATS_RESPONSE_t *statistics)
{
  L7_uint32                       intIfNum;
  PTIN_MGMD_EVENT_t               reqMsg          = {0};
  PTIN_MGMD_EVENT_t               resMsg          = {0};
  PTIN_MGMD_EVENT_CTRL_t          ctrlResMsg      = {0};
  PTIN_MGMD_CTRL_STATS_REQUEST_t  mgmdStatsReqMsg = {0};

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting port %u/%u to intIfNum",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Request port statistics to MGMD */
  mgmdStatsReqMsg.portId = intIfNum;
  ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_INTF_STATS_GET, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdStatsReqMsg, sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t));
  ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
  ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Res     : %u",   ctrlResMsg.res);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Length  : %u",   ctrlResMsg.dataLength);
  if(SUCCESS == ctrlResMsg.res)
  {
    memcpy(statistics, ctrlResMsg.data, sizeof(PTIN_MGMD_CTRL_STATS_RESPONSE_t));
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}

/**
 * GetIGMP statistics of a particular IGMP instance and 
 * interface 
 * 
 * @param evc_idx    : Multicast EVC id
 * @param intIfNum   : interface
 * @param statistics : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_instanceIntf_get(L7_uint32 evc_idx, ptin_intf_t *ptin_intf, PTIN_MGMD_CTRL_STATS_RESPONSE_t *statistics)
{
  L7_uint                         intIfNum       = 0;
  PTIN_MGMD_EVENT_t               reqMsg          = {0};
  PTIN_MGMD_EVENT_t               resMsg          = {0};
  PTIN_MGMD_EVENT_CTRL_t          ctrlResMsg      = {0};
  PTIN_MGMD_CTRL_STATS_REQUEST_t  mgmdStatsReqMsg = {0};

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting port %u/%u to intIfNum",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Request evc statistics to MGMD */
  mgmdStatsReqMsg.portId    = intIfNum;
  mgmdStatsReqMsg.serviceId = evc_idx;
  ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_INTF_STATS_GET, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdStatsReqMsg, sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t));
  ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
  ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Res     : %u",   ctrlResMsg.res);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Length  : %u",   ctrlResMsg.dataLength);
  if(SUCCESS == ctrlResMsg.res)
  {
    memcpy(statistics, ctrlResMsg.data, sizeof(PTIN_MGMD_CTRL_STATS_RESPONSE_t));
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * GetIGMP statistics of a particular IGMP instance and 
 * client
 * 
 * @param evc_idx   : Multicast EVC id
 * @param client    : client reference
 * @param stat_port : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_client_get(L7_uint32 evc_idx, ptin_client_id_t *client, PTIN_MGMD_CTRL_STATS_RESPONSE_t *statistics)
{
  ptinIgmpClientGroupInfoData_t   *clientInfo;
  PTIN_MGMD_EVENT_t               reqMsg          = {0};
  PTIN_MGMD_EVENT_t               resMsg          = {0};
  PTIN_MGMD_EVENT_CTRL_t          ctrlResMsg      = {0};
  PTIN_MGMD_CTRL_STATS_REQUEST_t  mgmdStatsReqMsg = {0};
  PTIN_MGMD_CTRL_STATS_RESPONSE_t mgmdStatsResMsg = {0};
  L7_uint32                       clientId;
  L7_uint32                       intIfNum;

  memset(statistics, 0x00, sizeof(PTIN_MGMD_CTRL_STATS_RESPONSE_t));

  /* Validate and rearrange clientId info */
  if (ptin_igmp_clientId_convert(evc_idx,client) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client id");
    return L7_FAILURE;
  }

  /* Get client */
  if (ptin_igmp_clientGroup_find(client, &clientInfo) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,
            "Error searching for client {mask=0x%02x,"
            "port=%u/%u,"
            "svlan=%u,"
            "cvlan=%u,"
            "ipAddr=%u.%u.%u.%u,"
            "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} ",
            client->mask,
            client->ptin_intf.intf_type, client->ptin_intf.intf_id,
            client->outerVlan,
            client->innerVlan,
            (client->ipv4_addr>>24) & 0xff, (client->ipv4_addr>>16) & 0xff, (client->ipv4_addr>>8) & 0xff, client->ipv4_addr & 0xff,
            client->macAddr[0],client->macAddr[1],client->macAddr[2],client->macAddr[3],client->macAddr[4],client->macAddr[5]);
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(&client->ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting port %u/%u to intIfNum",client->ptin_intf.intf_type,client->ptin_intf.intf_id);
    return L7_FAILURE;
  }

  for(clientId=0; clientId<(sizeof(clientInfo->client_bmp_list)*8); ++clientId)
  {
    if(IS_BITMAP_BIT_SET(clientInfo->client_bmp_list, clientId, sizeof(L7_uint32)))
    {
      /* Request client statistics to MGMD */
      mgmdStatsReqMsg.portId   = intIfNum;
      mgmdStatsReqMsg.clientId = clientId;
      ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_CLIENT_STATS_GET, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdStatsReqMsg, sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t));
      ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
      ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Res     : %u",   ctrlResMsg.res);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Length  : %u",   ctrlResMsg.dataLength);

      if(L7_SUCCESS != ctrlResMsg.res)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Error reading clientId[%u] statistics", clientId);
        return ctrlResMsg.res;
      }

      //Sum the current statistics on all set-top-boxes
      memcpy(&mgmdStatsResMsg, ctrlResMsg.data, sizeof(PTIN_MGMD_CTRL_STATS_RESPONSE_t));
      statistics->activeGroups                       += mgmdStatsResMsg.activeGroups;            
      statistics->activeClients                      += mgmdStatsResMsg.activeClients; 
                
      statistics->igmpTx                             += mgmdStatsResMsg.igmpTx;
      statistics->igmpValidRx                        += mgmdStatsResMsg.igmpValidRx;
      statistics->igmpInvalidRx                      += mgmdStatsResMsg.igmpInvalidRx;    
      statistics->igmpDroppedRx                      += mgmdStatsResMsg.igmpDroppedRx; 
      statistics->igmpTotalRx                        += mgmdStatsResMsg.igmpTotalRx;  
                                                         
      statistics->v2.joinTx                          += mgmdStatsResMsg.v2.joinTx;               
      statistics->v2.joinValidRx                     += mgmdStatsResMsg.v2.joinValidRx;   
      statistics->v2.joinInvalidRx                   += mgmdStatsResMsg.v2.joinInvalidRx;    
      statistics->v2.leaveTx                         += mgmdStatsResMsg.v2.leaveTx;              
      statistics->v2.leaveValidRx                    += mgmdStatsResMsg.v2.leaveValidRx;    
                                                         
      statistics->v3.membershipReportTx              += mgmdStatsResMsg.v3.membershipReportTx; 
      statistics->v3.membershipReportValidRx         += mgmdStatsResMsg.v3.membershipReportValidRx;      
      statistics->v3.membershipReportInvalidRx       += mgmdStatsResMsg.v3.membershipReportInvalidRx;          
      statistics->v3.groupRecords.allowTx            += mgmdStatsResMsg.v3.groupRecords.allowTx;
      statistics->v3.groupRecords.allowValidRx       += mgmdStatsResMsg.v3.groupRecords.allowValidRx;
      statistics->v3.groupRecords.allowInvalidRx     += mgmdStatsResMsg.v3.groupRecords.allowInvalidRx;
      statistics->v3.groupRecords.blockTx            += mgmdStatsResMsg.v3.groupRecords.blockTx;
      statistics->v3.groupRecords.blockValidRx       += mgmdStatsResMsg.v3.groupRecords.blockValidRx;
      statistics->v3.groupRecords.blockInvalidRx     += mgmdStatsResMsg.v3.groupRecords.blockInvalidRx;
      statistics->v3.groupRecords.isIncludeTx        += mgmdStatsResMsg.v3.groupRecords.isIncludeTx;
      statistics->v3.groupRecords.isIncludeValidRx   += mgmdStatsResMsg.v3.groupRecords.isIncludeValidRx;
      statistics->v3.groupRecords.isIncludeInvalidRx += mgmdStatsResMsg.v3.groupRecords.isIncludeInvalidRx;
      statistics->v3.groupRecords.isExcludeTx        += mgmdStatsResMsg.v3.groupRecords.isExcludeTx;
      statistics->v3.groupRecords.isExcludeValidRx   += mgmdStatsResMsg.v3.groupRecords.isExcludeValidRx;
      statistics->v3.groupRecords.isExcludeInvalidRx += mgmdStatsResMsg.v3.groupRecords.isExcludeInvalidRx;
      statistics->v3.groupRecords.toIncludeTx        += mgmdStatsResMsg.v3.groupRecords.toIncludeTx;
      statistics->v3.groupRecords.toIncludeValidRx   += mgmdStatsResMsg.v3.groupRecords.toIncludeValidRx;
      statistics->v3.groupRecords.toIncludeInvalidRx += mgmdStatsResMsg.v3.groupRecords.toIncludeInvalidRx;
      statistics->v3.groupRecords.toExcludeTx        += mgmdStatsResMsg.v3.groupRecords.toExcludeTx;
      statistics->v3.groupRecords.toExcludeValidRx   += mgmdStatsResMsg.v3.groupRecords.toExcludeValidRx;
      statistics->v3.groupRecords.toExcludeInvalidRx += mgmdStatsResMsg.v3.groupRecords.toExcludeInvalidRx;                                  
                                                         
      statistics->query.generalQueryTx               += mgmdStatsResMsg.query.generalQueryTx;     
      statistics->query.generalQueryValidRx          += mgmdStatsResMsg.query.generalQueryValidRx;
      statistics->query.groupQueryTx                 += mgmdStatsResMsg.query.groupQueryTx;       
      statistics->query.groupQueryValidRx            += mgmdStatsResMsg.query.groupQueryValidRx;  
      statistics->query.sourceQueryTx                += mgmdStatsResMsg.query.sourceQueryTx;      
      statistics->query.sourceQueryValidRx           += mgmdStatsResMsg.query.sourceQueryValidRx; 
    }
  }

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
  L7_uint32                       intIfNum;
  PTIN_MGMD_EVENT_t               reqMsg          = {0};
  PTIN_MGMD_EVENT_t               resMsg          = {0};
  PTIN_MGMD_EVENT_CTRL_t          ctrlResMsg      = {0};
  PTIN_MGMD_CTRL_STATS_REQUEST_t  mgmdStatsReqMsg = {0};

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting port %u/%u to intIfNum",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  mgmdStatsReqMsg.portId = intIfNum;
  ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_INTF_STATS_CLEAR, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdStatsReqMsg, sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t));
  ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
  ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Res     : %u",   ctrlResMsg.res);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Length  : %u",   ctrlResMsg.dataLength);

  return L7_SUCCESS;
}

/**
 * Clear statistics of a particular IGMP instance and interface
 * 
 * @param evc_idx  : Multicast EVC id
 * @param intIfNum    : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_instanceIntf_clear(L7_uint32 evc_idx, ptin_intf_t *ptin_intf)
{
  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  #if PTIN_IGMP_STATS_IN_EVCS
  L7_RC_t rc;

  /* Get stats pointer */
  osapiSemaTake(ptin_igmp_stats_sem, L7_WAIT_FOREVER);
  rc = ptin_evc_igmp_stats_clear(evc_idx, ptin_intf);
  osapiSemaGive(ptin_igmp_stats_sem);

  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting IGMP stats for EVC %u",evc_idx);
    return L7_FAILURE;
  }
  #else
  L7_uint igmp_idx, ptin_port;

  /* Validate interface */
  if (ptin_intf_ptintf2port(ptin_intf, &ptin_port)!=L7_SUCCESS || ptin_port>=PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "ptin_intf %u/%u is invalid", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Get IGMP instance */
  if (ptin_igmp_instance_find_fromSingleEvcId(evc_idx, &igmp_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"No Igmp instance found for EVC %u",evc_idx);
    return L7_FAILURE;
  }

  /* Clear stats */
  osapiSemaTake(ptin_igmp_stats_sem, L7_WAIT_FOREVER);
  memset(&igmpInstances[igmp_idx].stats_intf[ptin_port], 0x00, sizeof(ptin_IGMP_Statistics_t));
  osapiSemaGive(ptin_igmp_stats_sem);
  #endif

  return L7_SUCCESS;
}

/**
 * Clear all statistics of one IGMP instance
 * 
 * @param evc_idx : Multicast EVC id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_instance_clear(L7_uint32 evc_idx)
{
  #if PTIN_IGMP_STATS_IN_EVCS
  L7_RC_t rc;

  /* Clear all stats of this EVC */
  osapiSemaTake(ptin_igmp_stats_sem, L7_WAIT_FOREVER);
  rc = ptin_evc_igmp_stats_clear_all(evc_idx);
  osapiSemaGive(ptin_igmp_stats_sem);

  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting IGMP stats for EVC %u",evc_idx);
    return L7_FAILURE;
  }
  #else
  L7_uint igmp_idx;

  /* Get IGMP instance */
  if (ptin_igmp_instance_find_fromSingleEvcId(evc_idx, &igmp_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"No Igmp instance found for EVC %u",evc_idx);
    return L7_FAILURE;
  }

  /* Clear stats */
  osapiSemaTake(ptin_igmp_stats_sem, L7_WAIT_FOREVER);
  memset(igmpInstances[igmp_idx].stats_intf, 0x00, sizeof(igmpInstances[igmp_idx].stats_intf));
  osapiSemaGive(ptin_igmp_stats_sem);
  #endif

  return L7_SUCCESS;
}

/**
 * Clear all IGMP statistics
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_clearAll(void)
{
  osapiSemaTake(ptin_igmp_stats_sem, L7_WAIT_FOREVER);

  /* Clear global statistics */
  memset(global_stats_intf,0x00,sizeof(global_stats_intf));

  osapiSemaGive(ptin_igmp_stats_sem);

  return L7_SUCCESS;
}


/**
 * Clear IGMP statistics of a particular IGMP instance and 
 * client
 * 
 * @param evc_idx  : Multicast EVC id
 * @param client      : client reference
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_client_clear(L7_uint32 evc_idx, ptin_client_id_t *client)
{
  ptinIgmpClientGroupInfoData_t  *clientInfo;
  PTIN_MGMD_EVENT_t              reqMsg          = {0};
  PTIN_MGMD_EVENT_t              resMsg          = {0};
  PTIN_MGMD_EVENT_CTRL_t         ctrlResMsg      = {0};
  PTIN_MGMD_CTRL_STATS_REQUEST_t mgmdStatsReqMsg = {0};
  L7_uint32                      clientId;
  L7_uint32                      intIfNum;

  /* Validate and rearrange clientId info */
  if (ptin_igmp_clientId_convert(evc_idx, client) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client id");
    return L7_FAILURE;
  }

  /* Find client */
  if (ptin_igmp_clientGroup_find(client, &clientInfo) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,
            "Error searching for client {mask=0x%02x,"
            "port=%u/%u,"
            "svlan=%u,"
            "cvlan=%u,"
            "ipAddr=%u.%u.%u.%u,"
            "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} ",
            client->mask,
            client->ptin_intf.intf_type, client->ptin_intf.intf_id,
            client->outerVlan,
            client->innerVlan,
            (client->ipv4_addr>>24) & 0xff, (client->ipv4_addr>>16) & 0xff, (client->ipv4_addr>>8) & 0xff, client->ipv4_addr & 0xff,
            client->macAddr[0],client->macAddr[1],client->macAddr[2],client->macAddr[3],client->macAddr[4],client->macAddr[5]);
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(&client->ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting port %u/%u to intIfNum",client->ptin_intf.intf_type,client->ptin_intf.intf_id);
    return L7_FAILURE;
  }

  for(clientId=0; clientId<(sizeof(clientInfo->client_bmp_list)*8); ++clientId)
  {
    if(IS_BITMAP_BIT_SET(clientInfo->client_bmp_list, clientId, sizeof(L7_uint32)))
    {
      /* Request client statistics to MGMD */
      mgmdStatsReqMsg.portId   = intIfNum;
      mgmdStatsReqMsg.clientId = clientId;
      ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_CLIENT_STATS_CLEAR, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdStatsReqMsg, sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t));
      ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
      ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Res     : %u",   ctrlResMsg.res);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Length  : %u",   ctrlResMsg.dataLength);

      if(L7_SUCCESS != ctrlResMsg.res)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Error reading clientId[%u] statistics", clientId);
        return ctrlResMsg.res;
      }
    }
  }

  return L7_SUCCESS;
}

/**
 * Get IGMP statistics
 * 
 * @param intIfNum   : interface where the packet entered
 * @param vlan       : packet's interval vlan
 * @param client_idx : client index
 * @param field      : field to increment
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_get_field(L7_uint32 intIfNum, L7_uint16 vlan, L7_uint32 client_idx, ptin_snoop_stat_enum_t field)
{
  L7_uint32 ptin_port;
  ptinIgmpClientInfoData_t *client;
  #if (!PTIN_IGMP_STATS_IN_EVCS)
  st_IgmpInstCfg_t *igmpInst;
  #endif

  ptin_IGMP_Statistics_t *stat_port_g = L7_NULLPTR;
  ptin_IGMP_Statistics_t *stat_port   = L7_NULLPTR;
  ptin_IGMP_Statistics_t *stat_client = L7_NULLPTR;

  L7_uint32 statPortG=0;
  L7_uint32 statPort=0;
  L7_uint32 statClient=0;

  /* Validate field */
  if (field>=SNOOP_STAT_FIELD_ALL)
  {
    return L7_FAILURE;
  }

  /* If interface is valid... */
  ptin_port = 0;
  if (intIfNum > 0 && intIfNum < L7_MAX_INTERFACE_COUNT)
  {
    /* Check if interface exists */
    if (ptin_intf_intIfNum2port(intIfNum, &ptin_port) == L7_SUCCESS &&
        ptin_port < PTIN_SYSTEM_N_INTERF)
    {
      /* Global interface statistics at interface level */
      stat_port_g = &global_stats_intf[ptin_port];

      #if (!PTIN_IGMP_STATS_IN_EVCS)
      /* Get IGMP instance */
      if (ptin_igmp_inst_get_fromIntVlan(vlan, &igmpInst, L7_NULLPTR) == L7_SUCCESS)
      {
        stat_port = &igmpInst->stats_intf[ptin_port];
      }
      #endif
    }
  }

  /* Pointer to EVC stats */
  #if PTIN_IGMP_STATS_IN_EVCS
  (void) ptin_evc_igmp_stats_get_fromIntVlan(vlan, intIfNum, &stat_port);
  #endif

  /* If client index is valid... */
  if (client_idx < PTIN_IGMP_CLIENTIDX_MAX && ptin_port < PTIN_SYSTEM_N_INTERF)
  {
    client = igmpClients_unified.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;
    if (client != L7_NULLPTR && client->pClientGroup != L7_NULLPTR)
    {
      /* Statistics at client level: point to clientGroup data */
      stat_client = &client->pClientGroup->stats_client;
    }
  }

  switch (field) {
  case SNOOP_STAT_FIELD_ACTIVE_GROUPS:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->active_groups;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->active_groups;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->active_groups;
    break;

  case SNOOP_STAT_FIELD_ACTIVE_CLIENTS:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->active_clients;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->active_clients;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->active_clients;
    break;

/*Global Counters*/
  case SNOOP_STAT_FIELD_IGMP_SENT:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmp_sent;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmp_sent;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmp_sent;
    break;

  case SNOOP_STAT_FIELD_IGMP_TX_FAILED:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmp_tx_failed;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmp_tx_failed;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmp_tx_failed;
    break;

  case SNOOP_STAT_FIELD_IGMP_INTERCEPTED:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmp_intercepted;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmp_intercepted;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmp_intercepted;
    break;

  case SNOOP_STAT_FIELD_IGMP_DROPPED:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmp_dropped;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmp_dropped;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmp_dropped;
    break;

  case SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmp_received_valid;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmp_received_valid;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmp_received_valid;
    break;

  case SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmp_received_invalid;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmp_received_invalid;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmp_received_invalid;
    break;
/*End Global Counters*/

/*Query Counters*/
/*Generic Query*/
  case SNOOP_STAT_FIELD_GENERIC_QUERY_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpquery.generic_query_invalid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpquery.generic_query_invalid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpquery.generic_query_invalid_rx;
    break;
/*End Generic Query*/

/*General Query*/
  case SNOOP_STAT_FIELD_GENERAL_QUERY_TX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpquery.general_query_tx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpquery.general_query_tx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpquery.general_query_tx;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpquery.general_query_total_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpquery.general_query_total_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpquery.general_query_total_rx;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpquery.general_query_valid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpquery.general_query_valid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpquery.general_query_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpquery.general_query_dropped_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpquery.general_query_dropped_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpquery.general_query_dropped_rx;
    break;
/*End General Query*/

/*Group Specifc Query*/
  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpquery.group_query_tx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpquery.group_query_tx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpquery.group_query_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpquery.group_query_total_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpquery.group_query_total_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpquery.group_query_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpquery.group_query_valid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpquery.group_query_valid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpquery.group_query_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpquery.group_query_dropped_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpquery.group_query_dropped_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpquery.group_query_dropped_rx;
    break;
/*End Group Specifc Query*/

/*Group & Source Specifc Query*/
  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_TX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpquery.source_query_tx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpquery.source_query_tx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpquery.source_query_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpquery.source_query_total_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpquery.source_query_total_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpquery.source_query_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpquery.source_query_valid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpquery.source_query_valid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpquery.source_query_valid_rx;
    break;

   case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpquery.source_query_dropped_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpquery.source_query_dropped_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpquery.source_query_dropped_rx;
    break;
/*End Group & Source Specifc Query*/    
/*End Query Counters*/

/*To be replaced with the new structure*/
/*IGMP v2 Counters*/
  case SNOOP_STAT_FIELD_JOINS_SENT:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->joins_sent;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->joins_sent;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->joins_sent;
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->joins_received_success;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->joins_received_success;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->joins_received_success;
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->joins_received_failed;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->joins_received_failed;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->joins_received_failed;
    break;

  case SNOOP_STAT_FIELD_LEAVES_SENT:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->leaves_sent;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->leaves_sent;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->leaves_sent;
    break;

  case SNOOP_STAT_FIELD_LEAVES_RECEIVED:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->leaves_received;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->leaves_received;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->leaves_received;
    break;
/*End IGMPv2 Counters*/
  
/*IGMPv3 Counters*/
/*Mmbership Report Message*/
  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TX:
   if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.membership_report_tx;    
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.membership_report_tx;    
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.membership_report_tx;    
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.membership_report_total_rx;    
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.membership_report_total_rx;    
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.membership_report_total_rx;    
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.membership_report_valid_rx;    
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.membership_report_valid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.membership_report_valid_rx;
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.membership_report_invalid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.membership_report_invalid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.membership_report_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.membership_report_dropped_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.membership_report_dropped_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.membership_report_dropped_rx;
    break;

        /*GROUP RECORD*/
    /*Allow*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.allow_tx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.allow_tx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.allow_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.allow_total_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.allow_total_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.allow_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.allow_valid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.allow_valid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.allow_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.allow_invalid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.allow_invalid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.allow_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.allow_dropped_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.allow_dropped_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.allow_dropped_rx;
    break;
    /*End Allow*/   

  /*Block*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.block_tx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.block_tx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.block_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.block_total_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.block_total_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.block_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.block_valid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.block_valid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.block_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.block_invalid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.block_invalid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.block_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.block_dropped_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.block_dropped_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.block_dropped_rx;
    break;
    /*End Block*/   

    /*To_In*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.to_include_tx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.to_include_tx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.to_include_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.to_include_total_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.to_include_total_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.to_include_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.to_include_valid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.to_include_valid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.to_include_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.to_include_invalid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.to_include_invalid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.to_include_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.to_include_dropped_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.to_include_dropped_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.to_include_dropped_rx;
    break;
    /*End To_In*/   

     /*To_Ex*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.to_exclude_tx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.to_exclude_tx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.to_exclude_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.to_exclude_total_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.to_exclude_total_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.to_exclude_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.to_exclude_valid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.to_exclude_valid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.to_exclude_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.to_exclude_invalid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.to_exclude_invalid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.to_exclude_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.to_exclude_dropped_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.to_exclude_dropped_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.to_exclude_dropped_rx;
    break;
    /*End To_Ex*/   

     /*Is_In*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.is_include_tx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.is_include_tx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.is_include_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.is_include_total_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.is_include_total_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.is_include_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.is_include_valid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.is_include_valid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.is_include_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.is_include_invalid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.is_include_invalid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.is_include_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.is_include_dropped_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.is_include_dropped_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.is_include_dropped_rx;
    break;
    /*End Is_In*/   

     /*Is_Ex*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.is_exclude_tx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.is_exclude_tx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.is_exclude_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.is_exclude_total_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.is_exclude_total_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.is_exclude_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.is_exclude_valid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.is_exclude_valid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.is_exclude_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.is_exclude_invalid_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.is_exclude_invalid_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.is_exclude_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->igmpv3.group_record.is_exclude_dropped_rx;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->igmpv3.group_record.is_exclude_dropped_rx;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->igmpv3.group_record.is_exclude_dropped_rx;
    break;
    /*End Is_Ex*/   
/*END GROUP RECORD*/
/*End Membership Report Message*/
/*End IGMPv3 Counters*/
 
  default:
    break;
  }

  LOG_NOTICE(LOG_CTX_PTIN_IGMP,"statistics Field:%u",field);
  LOG_NOTICE(LOG_CTX_PTIN_IGMP,"statPortG:%u",statPortG);
  LOG_NOTICE(LOG_CTX_PTIN_IGMP,"statPort:%u",statPort);
  LOG_NOTICE(LOG_CTX_PTIN_IGMP,"statClient:%u",statClient);

  return L7_SUCCESS;
}

/**
 * Reset IGMP statistics
 * 
 * @param intIfNum   : interface where the packet entered
 * @param vlan       : packet's interval vlan
 * @param client_idx : client index
 * @param field      : field to increment
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_reset_field(L7_uint32 intIfNum, L7_uint16 vlan, L7_uint32 client_idx, ptin_snoop_stat_enum_t field)
{
  L7_uint32 ptin_port;
  ptinIgmpClientInfoData_t *client;
  #if (!PTIN_IGMP_STATS_IN_EVCS)
  st_IgmpInstCfg_t *igmpInst = L7_NULLPTR;
  #endif

  ptin_IGMP_Statistics_t *stat_port_g = L7_NULLPTR;
  ptin_IGMP_Statistics_t *stat_port   = L7_NULLPTR;
  ptin_IGMP_Statistics_t *stat_client = L7_NULLPTR;

  /* Validate field */
  if (field>=SNOOP_STAT_FIELD_ALL)
  {
    return L7_FAILURE;
  }

  #if (!PTIN_IGMP_STATS_IN_EVCS)
  /* Get IGMP instance */
  if (vlan>=PTIN_VLAN_MIN && vlan<=PTIN_VLAN_MAX)
  {
    if (ptin_igmp_inst_get_fromIntVlan(vlan, &igmpInst, L7_NULLPTR)!=L7_SUCCESS)
    {
      igmpInst = L7_NULLPTR;
    }
  }
  #endif

  /* If interface is valid... */
  ptin_port = 0;
  if (intIfNum>0 && intIfNum<L7_MAX_INTERFACE_COUNT)
  {
    /* Check if interface exists */
    if (ptin_intf_intIfNum2port(intIfNum, &ptin_port) == L7_SUCCESS && ptin_port < PTIN_SYSTEM_N_INTERF)
    {
      /* Global interface statistics at interface level */
      stat_port_g = &global_stats_intf[ptin_port];

      #if (!PTIN_IGMP_STATS_IN_EVCS)
      if (igmpInst!=L7_NULLPTR)
      {
        /* interface statistics at igmp instance and interface level */
        stat_port = &igmpInst->stats_intf[ptin_port];
      }
      #endif
    }
  }

  /* Pointer to EVC stats */
  #if PTIN_IGMP_STATS_IN_EVCS
  (void) ptin_evc_igmp_stats_get_fromIntVlan(vlan, intIfNum, &stat_port);
  #endif

  /* If client index is valid... */
  if (client_idx < PTIN_IGMP_CLIENTIDX_MAX && ptin_port < PTIN_SYSTEM_N_INTERF)
  {
    client = igmpClients_unified.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;
    if (client != L7_NULLPTR && client->pClientGroup != L7_NULLPTR)
    {
      /* Statistics at client level */
      stat_client = &client->pClientGroup->stats_client;
    }
  }

  switch (field) {
  case SNOOP_STAT_FIELD_ACTIVE_GROUPS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->active_groups = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->active_groups  = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->active_groups = 0;
    break;

  case SNOOP_STAT_FIELD_ACTIVE_CLIENTS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->active_clients = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->active_clients = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->active_clients = 0;
    break;

/*Global Counters*/
  case SNOOP_STAT_FIELD_IGMP_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_sent = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_sent = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_sent = 0;
    break;

  case SNOOP_STAT_FIELD_IGMP_TX_FAILED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_tx_failed = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_tx_failed = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_tx_failed = 0;
    break;

  case SNOOP_STAT_FIELD_IGMP_INTERCEPTED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_intercepted = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_intercepted = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_intercepted = 0;
    break;

  case SNOOP_STAT_FIELD_IGMP_DROPPED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_dropped = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_dropped = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_dropped = 0;
    break;

  case SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_received_valid = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_received_valid = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_received_valid = 0;
    break;

  case SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_received_invalid = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_received_invalid = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_received_invalid = 0;
    break;
/*End Global Counters*/

/*Query Counters*/
/*Generic Query*/
  case SNOOP_STAT_FIELD_GENERIC_QUERY_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->general_queries_sent = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->general_queries_sent = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->general_queries_sent = 0;
    break;
/*End Generic Query*/

/*General Query*/
  case SNOOP_STAT_FIELD_GENERAL_QUERY_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.general_query_tx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.general_query_tx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.general_query_tx = 0;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.general_query_total_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.general_query_total_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.general_query_total_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.general_query_valid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.general_query_valid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.general_query_valid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.general_query_dropped_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.general_query_dropped_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.general_query_dropped_rx = 0;
    break;
/*End General Query*/

/*Group Specifc Query*/
  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.group_query_tx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.group_query_tx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.group_query_tx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.group_query_total_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.group_query_total_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.group_query_total_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.group_query_valid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.group_query_valid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.group_query_valid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.group_query_dropped_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.group_query_dropped_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.group_query_dropped_rx = 0;
    break;
/*End Group Specifc Query*/

/*Group & Source Specifc Query*/
  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.source_query_tx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.source_query_tx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.source_query_tx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.source_query_total_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.source_query_total_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.source_query_total_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.source_query_valid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.source_query_valid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.source_query_valid_rx = 0;
    break;

   case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.source_query_dropped_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.source_query_dropped_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.source_query_dropped_rx = 0;
    break;
/*End Group & Source Specifc Query*/    
/*End Query Counters*/

/*To be replaced with the new structure*/
/*IGMP v2 Counters*/
  case SNOOP_STAT_FIELD_JOINS_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_sent = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_sent = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_sent = 0;
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_received_success = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_received_success = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_received_success = 0;
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_received_failed = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_received_failed = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_received_failed = 0;
    break;

  case SNOOP_STAT_FIELD_LEAVES_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->leaves_sent = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->leaves_sent = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->leaves_sent = 0;
    break;

  case SNOOP_STAT_FIELD_LEAVES_RECEIVED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->leaves_received = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->leaves_received = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->leaves_received = 0;
    break;
/*End IGMPv2 Counters*/
  
/*IGMPv3 Counters*/
/*Mmbership Report Message*/
  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TX:
   if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.membership_report_tx = 0;    
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.membership_report_tx = 0;    
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.membership_report_tx = 0;    
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.membership_report_total_rx = 0;    
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.membership_report_total_rx = 0;    
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.membership_report_total_rx = 0;    
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.membership_report_valid_rx = 0;    
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.membership_report_valid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.membership_report_valid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.membership_report_invalid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.membership_report_invalid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.membership_report_invalid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.membership_report_dropped_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.membership_report_dropped_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.membership_report_dropped_rx = 0;
    break;

    /*GROUP RECORD*/
    /*Allow*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.allow_tx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.allow_tx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.allow_tx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.allow_total_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.allow_total_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.allow_total_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.allow_valid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.allow_valid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.allow_valid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.allow_invalid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.allow_invalid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.allow_invalid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.allow_dropped_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.allow_dropped_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.allow_dropped_rx = 0;
    break;
    /*End Allow*/   

  /*Block*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.block_tx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.block_tx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.block_tx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.block_total_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.block_total_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.block_total_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.block_valid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.block_valid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.block_valid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.block_invalid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.block_invalid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.block_invalid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.block_dropped_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.block_dropped_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.block_dropped_rx = 0;
    break;
    /*End Block*/   

    /*To_In*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_include_tx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_include_tx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_include_tx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_include_total_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_include_total_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_include_total_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_include_valid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_include_valid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_include_valid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_include_invalid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_include_invalid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_include_invalid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_include_dropped_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_include_dropped_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_include_dropped_rx = 0;
    break;
    /*End To_In*/   

     /*To_Ex*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_exclude_tx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_exclude_tx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_exclude_tx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_exclude_total_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_exclude_total_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_exclude_total_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_exclude_valid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_exclude_valid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_exclude_valid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_exclude_invalid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_exclude_invalid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_exclude_invalid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_exclude_dropped_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_exclude_dropped_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_exclude_dropped_rx = 0;
    break;
    /*End To_Ex*/   

     /*Is_In*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_include_tx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_include_tx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_include_tx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_include_total_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_include_total_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_include_total_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_include_valid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_include_valid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_include_valid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_include_invalid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_include_invalid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_include_invalid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_include_dropped_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_include_dropped_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_include_dropped_rx = 0;
    break;
    /*End Is_In*/   

     /*Is_Ex*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_exclude_tx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_exclude_tx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_exclude_tx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_exclude_total_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_exclude_total_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_exclude_total_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_exclude_valid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_exclude_valid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_exclude_valid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_exclude_invalid_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_exclude_invalid_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_exclude_invalid_rx = 0;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_exclude_dropped_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_exclude_dropped_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_exclude_dropped_rx = 0;
    break;
    /*End Is_Ex*/   
/*END GROUP RECORD*/
/*End Membership Report Message*/
/*End IGMPv3 Counters*/
 
  default:
    break;
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
  L7_uint32 ptin_port = (L7_uint32)-1;
  ptinIgmpClientInfoData_t *client;
  #if (!PTIN_IGMP_STATS_IN_EVCS)
  st_IgmpInstCfg_t *igmpInst;
  #endif

  ptin_IGMP_Statistics_t *stat_port_g = L7_NULLPTR;
  ptin_IGMP_Statistics_t *stat_port   = L7_NULLPTR;
  ptin_IGMP_Statistics_t *stat_client = L7_NULLPTR;

  /* Validate field */
  if (field>=SNOOP_STAT_FIELD_ALL)
  {
    return L7_FAILURE;
  }

  /* If interface is valid... */
  if (intIfNum > 0 && intIfNum < L7_MAX_INTERFACE_COUNT)
  {
    /* Check if interface exists */
    if (ptin_intf_intIfNum2port(intIfNum, &ptin_port) == L7_SUCCESS && ptin_port < PTIN_SYSTEM_N_INTERF)
    {
      /* Global interface statistics at interface level */
      stat_port_g = &global_stats_intf[ptin_port];

      #if (!PTIN_IGMP_STATS_IN_EVCS)
      /* Get IGMP instance */
      if (ptin_igmp_inst_get_fromIntVlan(vlan, &igmpInst, L7_NULLPTR) == L7_SUCCESS)
      {
        stat_port = &igmpInst->stats_intf[ptin_port];
      }
      #endif
    }
  }

  #if PTIN_IGMP_STATS_IN_EVCS
  /* Pointer to IGMP stats */
  (void) ptin_evc_igmp_stats_get_fromIntVlan(vlan, intIfNum, &stat_port);
  #endif

  /* If client index is valid... */
  if (client_idx < PTIN_IGMP_CLIENTIDX_MAX)
  {
    if (ptin_port < PTIN_SYSTEM_N_PONS || ptin_port < PTIN_SYSTEM_N_ETH)
    {
      client = igmpClients_unified.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;
      if (client != L7_NULLPTR && client->pClientGroup != L7_NULLPTR)
      {
        /* Statistics at client level */
        stat_client = &client->pClientGroup->stats_client;
      }
    }
  }

  osapiSemaTake(ptin_igmp_stats_sem, L7_WAIT_FOREVER);

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

/*Global Counters*/
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
/*End Global Counters*/

/*Query Counters*/

/*To be removed*/
 /*Old Counters*/
#if 0
case SNOOP_STAT_FIELD_GENERAL_QUERY_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->general_queries_sent++;
    if (stat_port  !=L7_NULLPTR)  stat_port->general_queries_sent++;
    if (stat_client!=L7_NULLPTR)  stat_client->general_queries_sent++;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->general_queries_received++;
    if (stat_port  !=L7_NULLPTR)  stat_port->general_queries_received++;
    if (stat_client!=L7_NULLPTR)  stat_client->general_queries_received++;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->specific_queries_sent++;
    if (stat_port  !=L7_NULLPTR)  stat_port->specific_queries_sent++;
    if (stat_client!=L7_NULLPTR)  stat_client->specific_queries_sent++;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->specific_queries_received++;
    if (stat_port  !=L7_NULLPTR)  stat_port->specific_queries_received++;
    if (stat_client!=L7_NULLPTR)  stat_client->specific_queries_received++;
    break;
#endif
/*End Old Counters*/

/*Generic Query*/
  case SNOOP_STAT_FIELD_GENERIC_QUERY_INVALID_RX:  
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.generic_query_invalid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.generic_query_invalid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.generic_query_invalid_rx++;
    break;
/*End Generic Query*/

/*General Query*/
  case SNOOP_STAT_FIELD_GENERAL_QUERY_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.general_query_tx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.general_query_tx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.general_query_tx++;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.general_query_total_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.general_query_total_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.general_query_total_rx++;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.general_query_valid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.general_query_valid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.general_query_valid_rx++;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.general_query_dropped_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.general_query_dropped_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.general_query_dropped_rx++;
    break;
/*End General Query*/

/*Group Specifc Query*/
  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.group_query_tx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.group_query_tx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.group_query_tx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.group_query_total_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.group_query_total_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.group_query_total_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.group_query_valid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.group_query_valid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.group_query_valid_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.group_query_dropped_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.group_query_dropped_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.group_query_dropped_rx++;
    break;
/*End Group Specifc Query*/

/*Group & Source Specifc Query*/
  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.source_query_tx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.source_query_tx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.source_query_tx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.source_query_total_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.source_query_total_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.source_query_total_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.source_query_valid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.source_query_valid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.source_query_valid_rx++;
    break;

   case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpquery.source_query_dropped_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpquery.source_query_dropped_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpquery.source_query_dropped_rx++;
    break;
/*End Group & Source Specifc Query*/    
/*End Query Counters*/

/*To be replaced with the new structure*/
/*IGMP v2 Counters*/
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
/*End IGMPv2 Counters*/
  
/*IGMPv3 Counters*/
/*Mmbership Report Message*/
  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TX:
   if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.membership_report_tx++;    
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.membership_report_tx++;    
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.membership_report_tx++;    
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.membership_report_total_rx++;    
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.membership_report_total_rx++;    
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.membership_report_total_rx++;    
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.membership_report_valid_rx++;    
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.membership_report_valid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.membership_report_valid_rx++;
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.membership_report_invalid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.membership_report_invalid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.membership_report_invalid_rx++;
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.membership_report_dropped_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.membership_report_dropped_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.membership_report_dropped_rx++;
    break;

/*GROUP RECORD*/
    /*Allow*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.allow_tx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.allow_tx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.allow_tx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.allow_total_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.allow_total_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.allow_total_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.allow_valid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.allow_valid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.allow_valid_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.allow_invalid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.allow_invalid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.allow_invalid_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.allow_dropped_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.allow_dropped_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.allow_dropped_rx++;
    break;
    /*End Allow*/   


  /*Block*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.block_tx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.block_tx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.block_tx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.block_total_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.block_total_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.block_total_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.block_valid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.block_valid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.block_valid_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.block_invalid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.block_invalid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.block_invalid_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.block_dropped_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.block_dropped_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.block_dropped_rx++;
    break;
    /*End Block*/   


    /*To_In*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_include_tx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_include_tx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_include_tx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_include_total_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_include_total_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_include_total_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_include_valid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_include_valid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_include_valid_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_include_invalid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_include_invalid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_include_invalid_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_include_dropped_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_include_dropped_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_include_dropped_rx++;
    break;
    /*End To_In*/   

     /*To_Ex*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_exclude_tx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_exclude_tx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_exclude_tx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_exclude_total_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_exclude_total_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_exclude_total_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_exclude_valid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_exclude_valid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_exclude_valid_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_exclude_invalid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_exclude_invalid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_exclude_invalid_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.to_exclude_dropped_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.to_exclude_dropped_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.to_exclude_dropped_rx++;
    break;
    /*End To_Ex*/   

     /*Is_In*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_include_tx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_include_tx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_include_tx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_include_total_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_include_total_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_include_total_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_include_valid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_include_valid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_include_valid_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_include_invalid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_include_invalid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_include_invalid_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_include_dropped_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_include_dropped_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_include_dropped_rx++;
    break;
    /*End Is_In*/   

     /*Is_Ex*/
  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_exclude_tx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_exclude_tx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_exclude_tx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TOTAL_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_exclude_total_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_exclude_total_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_exclude_total_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_VALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_exclude_valid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_exclude_valid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_exclude_valid_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_INVALID_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_exclude_invalid_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_exclude_invalid_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_exclude_invalid_rx++;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_DROPPED_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmpv3.group_record.is_exclude_dropped_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmpv3.group_record.is_exclude_dropped_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmpv3.group_record.is_exclude_dropped_rx++;
    break;
    /*End Is_Ex*/   
/*END GROUP RECORD*/
/*End Membership Report Message*/
/*End IGMPv3 Counters*/
 
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
  ptinIgmpClientInfoData_t *client;
  #if (!PTIN_IGMP_STATS_IN_EVCS)
  st_IgmpInstCfg_t *igmpInst;
  #endif

  ptin_IGMP_Statistics_t *stat_port_g = L7_NULLPTR;
  ptin_IGMP_Statistics_t *stat_port   = L7_NULLPTR;
  ptin_IGMP_Statistics_t *stat_client = L7_NULLPTR;

  /* Validate field */
  if (field>=SNOOP_STAT_FIELD_ALL)
  {
    return L7_FAILURE;
  }

  /* If interface is valid... */
  ptin_port = 0;
  if (intIfNum > 0 && intIfNum < L7_MAX_INTERFACE_COUNT)
  {
    /* Check if interface exists */
    if (ptin_intf_intIfNum2port(intIfNum, &ptin_port) == L7_SUCCESS && ptin_port < PTIN_SYSTEM_N_INTERF)
    {
      /* Global interface statistics at interface level */
      stat_port_g = &global_stats_intf[ptin_port];

      #if (!PTIN_IGMP_STATS_IN_EVCS)
      /* Get IGMP instance */
      if (ptin_igmp_inst_get_fromIntVlan(vlan, &igmpInst, L7_NULLPTR) == L7_SUCCESS)
      {
        stat_port = &igmpInst->stats_intf[ptin_port];
      }
      #endif
    }
  }

  #if PTIN_IGMP_STATS_IN_EVCS
  /* Pointer to IGMP stats */
  (void) ptin_evc_igmp_stats_get_fromIntVlan(vlan, intIfNum, &stat_port);
  #endif

  /* If client index is valid... */
  if (client_idx < PTIN_IGMP_CLIENTIDX_MAX && ptin_port < PTIN_SYSTEM_N_INTERF)
  {
    client = igmpClients_unified.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;
    if (client != L7_NULLPTR && client->pClientGroup != L7_NULLPTR)
    {
      /* Statistics at client level */
      stat_client = &client->pClientGroup->stats_client;
    }
  }

  osapiSemaTake(ptin_igmp_stats_sem, L7_WAIT_FOREVER);

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


/*********************************************************** 
 * QUEUES MANAGEMENT FUNCTIONS
 ***********************************************************/

/**
 * Find a particular client in the client devices queue
 */
static struct ptinIgmpClientDevice_s *igmp_clientDevice_find(struct ptinIgmpClientGroupInfoData_s *clientGroup, struct ptinIgmpClientInfoData_s *clientInfo)
{
  struct ptinIgmpClientDevice_s *clientDevice = L7_NULLPTR;

  /* Validate arguments */
  if (clientGroup == L7_NULLPTR || clientInfo == L7_NULLPTR)
    return L7_FALSE;

  dl_queue_get_head(&clientGroup->queue_clientDevices, (dl_queue_elem_t **)&clientDevice);

  /* Run all client nodes */
  while (clientDevice != NULL)
  {
    /* If client index was found, break cycle */
    if (clientDevice->client == clientInfo)  break;

    clientDevice = (struct ptinIgmpClientDevice_s *) dl_queue_get_next(&clientGroup->queue_clientDevices, (dl_queue_elem_t *)clientDevice);
  }

  /* return client device pointer */
  return clientDevice;
}

/**
 * Get the next client withing client devices queue
 */
static struct ptinIgmpClientDevice_s *igmp_clientDevice_next(struct ptinIgmpClientGroupInfoData_s *clientGroup, struct ptinIgmpClientDevice_s *pelem)
{
  if (clientGroup == L7_NULLPTR)
    return L7_NULLPTR;

  /* If pelem is NULL, return head pointer */
  if (pelem == L7_NULLPTR)
    return (struct ptinIgmpClientDevice_s *) clientGroup->queue_clientDevices.head;

  /* Otherwise, return next value */
  return (struct ptinIgmpClientDevice_s *) pelem->next;
}

/** Check if a client index is present in a ONU */
static L7_uint8 igmp_clientDevice_get_devices_number(struct ptinIgmpClientGroupInfoData_s *clientGroup)
{
  if (clientGroup == L7_NULLPTR)
    return 0;

  /* Return number of elements in queue */
  return clientGroup->queue_clientDevices.n_elems;
}

/**
 * Add a client within the client devices queue
 */
static struct ptinIgmpClientDevice_s *igmp_clientDevice_add(struct ptinIgmpClientGroupInfoData_s *clientGroup, struct ptinIgmpClientInfoData_s *clientInfo)
{
  L7_uint ptin_port;
  struct ptinIgmpClientDevice_s *clientDevice;
  L7_uint32 clientIdx;

  /* Validate arguments */
  if (clientGroup == L7_NULLPTR || clientInfo == L7_NULLPTR)
  {
    return L7_NULLPTR;
  }

  /* Validate client idx */
  clientIdx = clientInfo->client_index;
  if(clientIdx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    return L7_NULLPTR;
  }

  /* Check if the provided client already exists */
  if (igmp_clientDevice_find(clientGroup, clientInfo) != L7_NULLPTR)
  {
    return L7_NULLPTR;
  }

  /* Set clientIdx in the client bitmap */
  BITMAP_BIT_SET(clientGroup->client_bmp_list, clientIdx, sizeof(L7_uint32));

  /* Client port */
  ptin_port = clientGroup->ptin_port;
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    return L7_NULLPTR;
  }

  /* Add client to the EVC struct */
  dl_queue_remove_head(&igmpClients_unified.queue_free_clientDevices[PTIN_IGMP_CLIENT_PORT(ptin_port)], (dl_queue_elem_t**) &clientDevice);
  clientDevice->client = clientInfo;
  dl_queue_add_tail(&clientGroup->queue_clientDevices, (dl_queue_elem_t*) clientDevice);

  /* Return pointer to new node */
  return clientDevice;
}

/**
 * Remove a client from the client devices queue
 */
static struct ptinIgmpClientDevice_s *igmp_clientDevice_remove(struct ptinIgmpClientGroupInfoData_s *clientGroup, struct ptinIgmpClientInfoData_s *clientInfo)
{
  L7_uint ptin_port;
  struct ptinIgmpClientDevice_s *clientDevice;
  L7_uint32 clientIdx;

  /* Validate arguments */
  if (clientGroup == L7_NULLPTR || clientInfo == L7_NULLPTR)
  {
    return L7_NULLPTR;
  }

  /* Validate client idx */
  clientIdx = clientInfo->client_index;
  if(clientIdx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    return L7_NULLPTR;
  }

  /* Check if the provided client already exists. If not return success */
  if ((clientDevice=igmp_clientDevice_find(clientGroup, clientInfo)) == L7_NULLPTR)
  {
    return L7_NULLPTR;
  }

  /* Client port */
  ptin_port = clientGroup->ptin_port;
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    return L7_NULLPTR;
  }

  /* Set clientIdx in the client bitmap */
  BITMAP_BIT_CLR(clientGroup->client_bmp_list, clientIdx, sizeof(L7_uint32));

  /* Remove node from client devices queue */
  dl_queue_remove(&clientGroup->queue_clientDevices, (dl_queue_elem_t*) clientDevice);
  clientDevice->client = L7_NULLPTR;
  dl_queue_add_tail(&igmpClients_unified.queue_free_clientDevices[PTIN_IGMP_CLIENT_PORT(ptin_port)], (dl_queue_elem_t*) clientDevice);

  return L7_SUCCESS;
}


/**
 * Get new client index 
 */
static L7_uint16 igmp_clientIndex_get_new(L7_uint ptin_port)
{
  struct ptinIgmpClientIdx_s *pClientIdx;

  /* Validate port */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    return (L7_uint16)-1;
  }

  /* Find a free client index */
  do
  {
    /* Extract one index */
    pClientIdx = L7_NULLPTR;

    dl_queue_remove_head(&queue_free_clientIdx[PTIN_IGMP_CLIENT_PORT(ptin_port)], (dl_queue_elem_t **) &pClientIdx);

    /* Check if there is free indexes */
    if (pClientIdx == L7_NULLPTR)
    {
      /* Not found: return -1 */
      return (L7_uint16)-1;
    }
  } while (pClientIdx->in_use);

  /* Mark index as being used */
  pClientIdx->in_use = L7_TRUE;

  /* Return client index */
  return pClientIdx->client_idx;
}

/**
 * Get new client index 
 */
static void igmp_clientIndex_free(L7_uint ptin_port, L7_uint16 client_idx)
{
  struct ptinIgmpClientIdx_s *pClientIdx;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      client_idx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    return;
  }

  /* Check if free indexes pool is already full */
  if (queue_free_clientIdx[PTIN_IGMP_CLIENT_PORT(ptin_port)].n_elems >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    return;
  }

  /* Node to be added */
  pClientIdx = &clientIdx_pool[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx];

  /* Just to be sure it has the right value */
  pClientIdx->client_idx = client_idx;

  /* Check if this entry is already free */
  if (!pClientIdx->in_use)
    return;

  /* Add index to free indexes queue */
  if (dl_queue_add_head(&queue_free_clientIdx[PTIN_IGMP_CLIENT_PORT(ptin_port)], (dl_queue_elem_t *) pClientIdx) != NOERR)
  {
    return;
  }

  /* Update in_use flag */
  pClientIdx->in_use = L7_FALSE;
}

#if 0
/**
 * Check if a client is being used
 */
static L7_BOOL igmp_clientIndex_is_marked(L7_uint client_idx)
{
  if (client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
    return L7_FALSE;

  /* Return client existence status */
  return (igmpClients_unified.client_devices[client_idx].client!=L7_NULLPTR);
}
#endif

/**
 * Mark a client device as being used 
 */
static void igmp_clientIndex_mark(L7_uint ptin_port, L7_uint client_idx, ptinIgmpClientInfoData_t *infoData)
{
  if (ptin_port >= PTIN_SYSTEM_N_INTERF || client_idx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    return;
  }

  /* Update number of clients */
  if (igmpClients_unified.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client == L7_NULLPTR)
  {
    if (igmpClients_unified.number_of_clients < PTIN_IGMP_CLIENTIDX_MAX)
      igmpClients_unified.number_of_clients++;

    if (ptin_port<PTIN_SYSTEM_N_INTERF)
    {
      if (igmpClients_unified.number_of_clients_per_intf[ptin_port] < PTIN_IGMP_CLIENTIDX_MAX )
        igmpClients_unified.number_of_clients_per_intf[ptin_port]++;
    }
  }

  igmpClients_unified.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client = infoData;
}

/**
 * Unmark a client device as being free
 */
static void igmp_clientIndex_unmark(L7_uint ptin_port, L7_uint client_idx)
{
  if (ptin_port >= PTIN_SYSTEM_N_INTERF || client_idx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    return;
  }

  /* Update number of clients */
  if (igmpClients_unified.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client != L7_NULLPTR)
  {
    if (igmpClients_unified.number_of_clients>0)
    {
      igmpClients_unified.number_of_clients--;
    }

    if (ptin_port<PTIN_SYSTEM_N_INTERF)
    {
      if (igmpClients_unified.number_of_clients_per_intf[ptin_port] > 0 )
      {
        igmpClients_unified.number_of_clients_per_intf[ptin_port]--;
      }
    }
  }

  igmpClients_unified.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client = L7_NULLPTR;
}


/* DEBUG Functions ************************************************************/
/**
 * Dumps EVC detailed info 
 * If evc_idx is invalid, all EVCs are dumped 
 * 
 * @param evc_idx 
 */
void ptin_igmp_dump(void)
{
  L7_uint i;

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  for (i = 0; i < PTIN_SYSTEM_N_IGMP_INSTANCES; i++)
  {
    if (!igmpInstances[i].inUse) {
      printf("*** Igmp instance %02u not in use\r\n", i);
      continue;
    }

    printf("IGMP instance %02u   ", i);
    printf("\n");
    printf("   MC evcId = %4u   NNI VLAN = %u\r\n",igmpInstances[i].McastEvcId, igmpInstances[i].nni_ovid);
    printf("   UC evcId = %4u     #evc's = %u\r\n",igmpInstances[i].UcastEvcId, igmpInstances[i].n_evcs);
  }
  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  printf("\r\nTotal number of QUATTRO-P2P evcs: %u\r\n", igmp_quattro_p2p_evcs);
  #endif
  printf("Clients are not associated to IGMP instances any more!!!\r\n");

  osapiSemaGive(ptin_igmp_clients_sem);
}

/**
 * Dumps EVC detailed info 
 * If evc_idx is invalid, all EVCs are dumped 
 * 
 * @param evc_idx 
 */
void ptin_igmp_groupclients_dump(void)
{
  L7_uint i_client, child_clients;
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientGroupInfoData_t *clientGroup;

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  printf("List of Group clients (%u clients):\n",igmpClientGroups.number_of_clients);

  i_client = 0;

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  while ( ( clientGroup = (ptinIgmpClientGroupInfoData_t *)
                        avlSearchLVL7(&igmpClientGroups.avlTree.igmpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &clientGroup->igmpClientDataKey, sizeof(ptinIgmpClientDataKey_t));
    
    /* Count number of child clients */
    child_clients = igmp_clientDevice_get_devices_number(clientGroup);

    printf("      Client#%u: "
           #if (MC_CLIENT_INTERF_SUPPORTED)
           "ptin_port=%-2u "
           #endif
           #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
           "svlan=%-4u (intVlan=%-4u) "
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
           ": port=%-2u uni_vid=%4u+%-4u (#devices=%u)\r\n",
           i_client,
           #if (MC_CLIENT_INTERF_SUPPORTED)
           clientGroup->igmpClientDataKey.ptin_port,
           #endif
           #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
           clientGroup->uni_ovid, clientGroup->igmpClientDataKey.outerVlan,
           #endif
           #if (MC_CLIENT_INNERVLAN_SUPPORTED)
           clientGroup->igmpClientDataKey.innerVlan,
           #endif
           #if (MC_CLIENT_IPADDR_SUPPORTED)
           (clientGroup->igmpClientDataKey.ipv4_addr>>24) & 0xff,
            (clientGroup->igmpClientDataKey.ipv4_addr>>16) & 0xff,
             (clientGroup->igmpClientDataKey.ipv4_addr>>8) & 0xff,
              clientGroup->igmpClientDataKey.ipv4_addr & 0xff,
           #endif
           #if (MC_CLIENT_MACADDR_SUPPORTED)
           clientGroup->igmpClientDataKey.macAddr[0],
            clientGroup->igmpClientDataKey.macAddr[1],
             clientGroup->igmpClientDataKey.macAddr[2],
              clientGroup->igmpClientDataKey.macAddr[3],
               clientGroup->igmpClientDataKey.macAddr[4],
                clientGroup->igmpClientDataKey.macAddr[5],
           #endif
           clientGroup->ptin_port,
           clientGroup->uni_ovid, clientGroup->uni_ivid,
           child_clients);

    i_client++;
  }

  printf("Done!\n");

  osapiSemaGive(ptin_igmp_clients_sem);
}

/**
 * Dumps EVC detailed info 
 * If evc_idx is invalid, all EVCs are dumped 
 * 
 * @param evc_idx 
 */
void ptin_igmp_clients_dump(void)
{
  L7_uint i_client;
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientInfoData_t *avl_info;

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  printf("List of clients (%u clients):\n",igmpClients_unified.number_of_clients);

  i_client = 0;

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  while ( ( avl_info = (ptinIgmpClientInfoData_t *)
                        avlSearchLVL7(&igmpClients_unified.avlTree.igmpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->igmpClientDataKey, sizeof(ptinIgmpClientDataKey_t));
    
    printf("      Client#%u: "
           #if (MC_CLIENT_INTERF_SUPPORTED)
           "ptin_port=%-2u "
           #endif
           #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
           "svlan=%-4u (intVlan=%-4u) "
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
           ": port=%-2u/index=%-3u  uni_vid=%4u+%-4u [%s] #channels=%u\r\n",
           i_client,
           #if (MC_CLIENT_INTERF_SUPPORTED)
           avl_info->igmpClientDataKey.ptin_port,
           #endif
           #if (MC_CLIENT_OUTERVLAN_SUPPORTED)
           avl_info->uni_ovid, avl_info->igmpClientDataKey.outerVlan,
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
           avl_info->ptin_port,
           avl_info->client_index,
           avl_info->uni_ovid, avl_info->uni_ivid,
           ((avl_info->isDynamic) ? "dynamic" : "static "),
           (avl_info->pClientGroup != L7_NULLPTR) ? avl_info->pClientGroup->stats_client.active_groups : 0);

    i_client++;
  }

  printf("Done!\n");

  osapiSemaGive(ptin_igmp_clients_sem);
}

/**
 * Dumps all IGMP associations 
 */
void ptin_igmp_assoc_dump(L7_int evc_mc, L7_int evc_uc)
{
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  ptinIgmpPairDataKey_t avl_key;
  ptinIgmpPairInfoData_t *avl_info;
  L7_uint16 n_entries;

  /* Hello */
  if ( evc_mc <= 0 )
  {
    printf("Printing all IGMP association entries (%u total):\r\n",igmpPairDB.number_of_entries);
  }
  else
  {
    printf("Printing only IGMP association entries related to EVC_MC %u:\r\n",evc_mc);
  }

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinIgmpPairDataKey_t));

  n_entries = 0;

  while ( ( avl_info = (ptinIgmpPairInfoData_t *)
                        avlSearchLVL7(&igmpPairDB.igmpPairAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->igmpPairDataKey, sizeof(ptinIgmpPairDataKey_t));

    /* Skip entry, if MC evc is provided and does not match to this entry */
    if ( evc_mc > 0 && avl_info->evc_mc != evc_mc )
    {
      continue;
    }
    #if (IGMPASSOC_CHANNEL_UC_EVC_ISOLATION)
    if ( evc_uc > 0 && avl_info->igmpPairDataKey.evc_uc != evc_uc )
    {
      continue;
    }
    #endif

    printf(
           #if (IGMPASSOC_CHANNEL_UC_EVC_ISOLATION)
           "EVC_UC=%-3u "
           #endif
           "EVC_MC=%-3u "
           "groupAddr=%03u.%03u.%03u.%03u (%s) "
           #if (IGMPASSOC_CHANNEL_SOURCE_SUPPORTED)
           "srcIPAddr=%03u.%03u.%03u.%03u (%s) "
           #endif
           "(%s)\r\n"
           #if (IGMPASSOC_CHANNEL_UC_EVC_ISOLATION)
           , avl_info->igmpPairDataKey.evc_uc
           #endif
           , avl_info->evc_mc
           , (avl_info->igmpPairDataKey.channel_group.addr.ipv4.s_addr>>24) & 0xff,
              (avl_info->igmpPairDataKey.channel_group.addr.ipv4.s_addr>>16) & 0xff,
               (avl_info->igmpPairDataKey.channel_group.addr.ipv4.s_addr>>8) & 0xff,
                avl_info->igmpPairDataKey.channel_group.addr.ipv4.s_addr & 0xff,
           ((avl_info->igmpPairDataKey.channel_group.family==L7_AF_INET6) ? "IPv6" : "IPv4")
           #if (IGMPASSOC_CHANNEL_SOURCE_SUPPORTED)
           , (avl_info->igmpPairDataKey.channel_source.addr.ipv4.s_addr>>24) & 0xff,
              (avl_info->igmpPairDataKey.channel_source.addr.ipv4.s_addr>>16) & 0xff,
               (avl_info->igmpPairDataKey.channel_source.addr.ipv4.s_addr>>8) & 0xff,
                avl_info->igmpPairDataKey.channel_source.addr.ipv4.s_addr & 0xff,
           ((avl_info->igmpPairDataKey.channel_source.family==L7_AF_INET6) ? "IPv6" : "IPv4")
           #endif
           , ((avl_info->is_static) ? "static " : "dynamic")
           );
    n_entries++;
  }

  printf("Done! %u entries displayed.\r\n",n_entries);

#else
  printf("IGMP Multi-MC not supported on this version\r\n");
#endif
}

/**
 * Dumps IGMP proxy configuration
 */
void ptin_igmp_proxy_dump(void)
{
  printf("IGMP Proxy global:\r\n");
  printf(" Mask            = 0x%04x\r\n", igmpProxyCfg.mask);
  printf(" Admin           = %u\r\n",     igmpProxyCfg.admin);
  printf(" Network Version = %u\r\n",     igmpProxyCfg.networkVersion);
  printf(" Client  Version = %u\r\n",     igmpProxyCfg.clientVersion);
  printf(" IPv4 Address    = %03u.%03u.%03u.%03u\r\n",
         (igmpProxyCfg.ipv4_addr.s_addr>>24) & 0xff, (igmpProxyCfg.ipv4_addr.s_addr>>16) & 0xff, (igmpProxyCfg.ipv4_addr.s_addr>>8) & 0xff, (igmpProxyCfg.ipv4_addr.s_addr) & 0xff);
  printf(" COS             = %u\r\n",     igmpProxyCfg.igmp_cos);
  printf(" Fast Leave?     = %u\r\n",     igmpProxyCfg.fast_leave);

  printf("IGMP Querier:\r\n");
  printf(" Mask                           = 0x%04x\r\n", igmpProxyCfg.querier.mask);
  printf(" Flags                          = 0x%04x\r\n", igmpProxyCfg.querier.flags);
  printf(" Robustness                     = %u\r\n",    igmpProxyCfg.querier.robustness);
  printf(" Query Interval                 = %u\r\n",    igmpProxyCfg.querier.query_interval);
  printf(" Query Response Interval        = %u\r\n",    igmpProxyCfg.querier.query_response_interval);
  printf(" Group Mmbership Interval       = %u\r\n",    igmpProxyCfg.querier.group_membership_interval);
  printf(" Other Querier Present Interval = %u\r\n",    igmpProxyCfg.querier.other_querier_present_interval);
  printf(" Startup Querier Interval       = %u\r\n",    igmpProxyCfg.querier.startup_query_interval);
  printf(" Startup Query Count            = %u\r\n",    igmpProxyCfg.querier.startup_query_count);
  printf(" Last Member Query Interval     = %u\r\n",    igmpProxyCfg.querier.last_member_query_interval);
  printf(" Last Member Query Count        = %u\r\n",    igmpProxyCfg.querier.last_member_query_count);
  printf(" Older Host Present Timeout     = %u\r\n",    igmpProxyCfg.querier.older_host_present_timeout);

  printf("IGMP Host:\r\n");
  printf(" Mask                           = 0x%04x\r\n", igmpProxyCfg.host.mask);
  printf(" Flags                          = 0x%04x\r\n", igmpProxyCfg.host.flags);
  printf(" Robustness                     = %u\r\n",     igmpProxyCfg.host.robustness);
  printf(" Unsolicited Report Interval    = %u\r\n",     igmpProxyCfg.host.unsolicited_report_interval);
  printf(" Older Querier Present Timeout  = %u\r\n",     igmpProxyCfg.host.older_querier_present_timeout);
  printf(" Maximum Records per Report     = %u\r\n",     igmpProxyCfg.host.max_records_per_report);

  printf("Done!\r\n");
}

/**
 * Dumps IGMP queriers configuration
 * 
 * @param evc_idx : evc index
 */
void ptin_igmp_querier_dump(L7_int evc_idx)
{
  L7_uint16 vlanId;
  ptin_HwEthMef10Evc_t evcConf;
  L7_inet_addr_t address;
  L7_uint32 query_interval, expiry_interval;
  L7_uint32 mode, version, participate, maxRespTime, operState;

  /* Global configurations */
  if (snoopQuerierAdminModeGet(&mode, L7_AF_INET)==L7_SUCCESS &&
      snoopQuerierAddressGet(&address, L7_AF_INET)==L7_SUCCESS &&
      snoopQuerierVersionGet(&version, L7_AF_INET)==L7_SUCCESS &&
      snoopQuerierQueryIntervalGet(&query_interval, L7_AF_INET)==L7_SUCCESS &&
      snoopQuerierExpiryIntervalGet(&expiry_interval, L7_AF_INET)==L7_SUCCESS)
  {
    printf("Global Querier Definitions:\r\n");
    printf(" Admin    : %u\r\n", mode);
    printf(" Address  : %03u.%03u.%03u.%03u\r\n",
           (address.addr.ipv4.s_addr>>24) & 0xff,
           (address.addr.ipv4.s_addr>>16) & 0xff,
           (address.addr.ipv4.s_addr>> 8) & 0xff,
           (address.addr.ipv4.s_addr) & 0xff );
    printf(" Version  : %u\r\n", version);
    printf(" Query Interval : %u\r\n", query_interval );
    printf(" Expiry Interval: %u\r\n", expiry_interval);
  }
  else
  {
    printf("Error getting global querier definitions\r\n");
  }

  /* Hello */
  if ( evc_idx <= 0 )
  {
    printf("\nPrinting all IGMP UC services.\r\n");
  }
  else
  {
    printf("\nPrinting only IGMP UC service provided %u:\r\n",evc_idx);
  }

  for (evc_idx=0; evc_idx<PTIN_SYSTEM_N_EXTENDED_EVCS; evc_idx++)
  {
    /* Print this? */
    if (evc_idx>0 && evc_idx!=evc_idx)
      continue;

    /* EVC must be active */
    if (!ptin_evc_is_in_use(evc_idx))
    {
      if (evc_idx>0)
        printf("EVC %u does not exist!\r\n",evc_idx);
      continue;
    }

    /* Get EVC configuration */
    if (ptin_evc_get(&evcConf) != L7_SUCCESS)
    {
      printf("Error getting EVC %u configuration!\r\n",evc_idx);
      continue;
    }

    /* IGMP flag should be active */
    if (!(evcConf.flags & PTIN_EVC_MASK_IGMP_PROTOCOL))
    {
      if (evc_idx>0)
        printf("EVC %u does not have IGMP flag active!\r\n",evc_idx);
      continue;
    }

    /* Extract root vlan */
    if (ptin_evc_intRootVlan_get(evc_idx, &vlanId)!=L7_SUCCESS)
    {
      printf("Error getting EVC %u internal root vlan!\r\n",evc_idx);
      continue;
    }

    /* Validate vlan */
    if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
    {
      printf("Invalid vlan for EVC %u (%u)!\r\n",evc_idx,vlanId);
      continue;
    }

    /* Get vlan configurations and states */
    printf("Querier definitions for EVC %u / internal vlan %u:\r\n",evc_idx,vlanId);
    if (snoopQuerierVlanModeGet(vlanId, &mode, L7_AF_INET)==L7_SUCCESS)
      printf(" Admin   : %u\r\n", mode);
    else
      printf(" Admin   : error\r\n");

    if (snoopQuerierVlanAddressGet(vlanId, &address, L7_AF_INET)==L7_SUCCESS)
    {
      printf(" Address : %03u.%03u.%03u.%03u\r\n",
             (address.addr.ipv4.s_addr>>24) & 0xff,
             (address.addr.ipv4.s_addr>>16) & 0xff,
             (address.addr.ipv4.s_addr>> 8) & 0xff,
             (address.addr.ipv4.s_addr) & 0xff );
    }
    if (snoopQuerierVlanElectionModeGet(vlanId, &participate, L7_AF_INET)==L7_SUCCESS)
      printf(" Election mode: %u\r\n", participate );
    else
      printf(" Election mode: error\r\n");

    if (snoopQuerierOperVersionGet(vlanId, &version, L7_AF_INET)==L7_SUCCESS)
      printf(" Version      : %u\r\n", version);
    else
      printf(" Version      : error\r\n");

    if (snoopQuerierOperStateGet(vlanId, &operState, L7_AF_INET)==L7_SUCCESS)
      printf(" OperState    : %u\r\n", operState);
    else
      printf(" OperState    : error\r\n");

    if (snoopQuerierOperMaxRespTimeGet(vlanId, &maxRespTime, L7_AF_INET)==L7_SUCCESS)
      printf(" MaxRespTime  : %u\r\n", maxRespTime);
    else
      printf(" MaxRespTime  : error\r\n");
  }

  printf("Done!\r\n");
}

