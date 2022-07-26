/**
 * ptin_pppoe.c 
 *  
 * Implements the PPPOE snooping interface module
 *
 * Created on: 2012/01/06
 * Author: Milton Ruas (milton-r-silva@ext.ptinovacao.pt)
 *  
 * Notes:
 */

#include "ptin_pppoe.h"
#include "ptin_xlate_api.h"
#include "ptin_utils.h"
#include "ptin_evc.h"
#include "ptin_fieldproc.h"

#include "usmdb_dhcp_snooping.h"

/********************************************* 
 * Defines
 *********************************************/

#define PPPOE_ACCEPT_UNSTACKED_PACKETS 0

#define DEFAULT_ACCESS_NODE_ID  "OLT360"   /* No more than 40 characters */

#define PPPOE_INVALID_ENTRY    0xFF

#define PPPOE_CLIENT_DEBUG 1
L7_BOOL ptin_debug_pppoe_snooping = 0;

void ptin_debug_pppoe_enable(L7_BOOL enable)
{
  ptin_debug_pppoe_snooping = enable;
}


/* Parameters to identify the client */
#define PPPOE_CLIENT_INTERF_SUPPORTED    1
#if (PTIN_BOARD_IS_ACTIVETH)
 #define PPPOE_CLIENT_OUTERVLAN_SUPPORTED 1
#else
 #define PPPOE_CLIENT_OUTERVLAN_SUPPORTED 0
#endif
#define PPPOE_CLIENT_INNERVLAN_SUPPORTED 1
#define PPPOE_CLIENT_IPADDR_SUPPORTED    0
#define PPPOE_CLIENT_MACADDR_SUPPORTED   0

/* At least one parameter must be active */
#if ( !( PPPOE_CLIENT_INTERF_SUPPORTED    |  \
         PPPOE_CLIENT_OUTERVLAN_SUPPORTED |  \
         PPPOE_CLIENT_INNERVLAN_SUPPORTED |  \
         PPPOE_CLIENT_IPADDR_SUPPORTED    |  \
         PPPOE_CLIENT_MACADDR_SUPPORTED ) )
  #error "ptin_pppoe.c: At least one parameter must be defined!"
#endif

/* To validate mask according to the suuported parameters */
#define PPPOE_CLIENT_MASK_UPDATE(mask) \
        ( ( (PTIN_CLIENT_MASK_FIELD_INTF      & PPPOE_CLIENT_INTERF_SUPPORTED   ) |   \
            (PTIN_CLIENT_MASK_FIELD_OUTERVLAN & PPPOE_CLIENT_OUTERVLAN_SUPPORTED) |   \
            (PTIN_CLIENT_MASK_FIELD_INNERVLAN & PPPOE_CLIENT_INNERVLAN_SUPPORTED) |   \
            (PTIN_CLIENT_MASK_FIELD_IPADDR    & PPPOE_CLIENT_IPADDR_SUPPORTED   ) |   \
            (PTIN_CLIENT_MASK_FIELD_MACADDR   & PPPOE_CLIENT_MACADDR_SUPPORTED  ) ) & (mask) )


/*********************************************************** 
 * Typedefs
 ***********************************************************/

typedef struct
{
  L7_uint16 pppoe_instance;

  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  L7_uint8  intIfNum;                /* intIfNum, which is attached */
  #endif
  #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
  L7_uint16 outerVlan;                /* Outer Vlan */
  #endif
  #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
  L7_uint16 innerVlan;                /* Inner Vlan */
  #endif
  #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
  L7_uint32 ipv4_addr;                /* IP address */
  #endif
  #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN]; /* Source MAC */
  #endif
} ptinPppoeClientDataKey_t;

typedef struct
{
  L7_BOOL                 useEvcPppoeOptions;
  L7_uint16               pppoe_options;
  ptin_clientCircuitId_t  circuitId;                                    /* Circuit ID parameters */
  L7_char8                circuitId_str[FD_DS_MAX_REMOTE_ID_STRING+1];  /* Circuit ID string */
  L7_char8                remoteId_str[FD_DS_MAX_REMOTE_ID_STRING+1];       /* Remote ID string */
} ptinPppoeData_t;

typedef struct
{
  ptinPppoeClientDataKey_t pppoeClientDataKey;
  L7_uint16                client_index;
  ptinPppoeData_t          client_data;
  L7_uint16                uni_ovid;       /* External outer vlan */
  L7_uint16                uni_ivid;       /* External inner vlan */
  L7_uint32                ptin_port;
  ptin_PPPOE_Statistics_t  client_stats;   /* Client statistics   */
  void *next;
} ptinPppoeClientInfoData_t;

typedef struct {
    avlTree_t                 pppoeClientsAvlTree;
    avlTreeTables_t           *pppoeClientsTreeHeap;
    ptinPppoeClientInfoData_t *pppoeClientsDataHeap;
} ptinPppoeClientsAvlTree_t;

/* PPPOE AVL Tree data */
typedef struct {
  L7_uint16                  number_of_clients;
  ptinPppoeClientsAvlTree_t  avlTree;
} ptinPppoeClients_unified_t;

/* Client entries pool */
struct ptin_clientIdx_entry_s {
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct ptin_clientIdx_entry_s *next;
  struct ptin_clientIdx_entry_s *prev;

  L7_uint client_id;  /* One index of  array */
};
struct ptin_clientInfo_entry_s {
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct ptin_clientInfo_entry_s *next;
  struct ptin_clientInfo_entry_s *prev;

  ptinPppoeClientInfoData_t *client_info;
};

#define CIRCUITID_TEMPLATE_MAX_STRING   256

typedef struct {
  char template_str[CIRCUITID_TEMPLATE_MAX_STRING];
  L7_uint32 mask;

  char access_node_id[FD_DS_MAX_REMOTE_ID_STRING];
  L7_uint8    chassis;
  L7_uint8    rack;
  L7_uint8    frame;
  L7_uint8    ethernet_priority;
  L7_uint16   s_vid;
} ptin_AccessNodeCircuitId_t;

/* PPPOE Instance config struct */
typedef struct {
  L7_BOOL                     inUse;
  L7_uint16                   nni_ovid;          /* NNI outer vlan */
  L7_uint16                   n_evcs;
  L7_uint16                   evcPppoeOptions;   /* PPPOE Options (0x01=Option82; 0x02=Option37; 0x02=Option18) */
  dl_queue_t                  queue_clients;
  ptin_PPPOE_Statistics_t     stats_intf[PTIN_SYSTEM_N_INTERF];  /* PPPOE statistics at interface level */
  ptin_AccessNodeCircuitId_t  circuitid;
} st_PppoeInstCfg_t;

#define QUATTRO_PPPOE_TRAP_PREACTIVE     0   /* To always have this rule active, set 1 */
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
/* Global number of QUATTRO P2P flows */
static L7_uint32 pppoe_quattro_stacked_evcs = 0;
#endif

/*********************************************************** 
 * Data structs
 ***********************************************************/

/* Global PPPoE statistics at interface level */
ptin_port_bmp_t pppoe_intf_trusted;

/* PPPOE instances array */
st_PppoeInstCfg_t  pppoeInstances[PTIN_SYSTEM_N_PPPOE_INSTANCES];

/* PPPoE clients */
ptinPppoeClients_unified_t pppoeClients_unified;

/* Global PPPOE statistics at interface level */
static ptin_PPPOE_Statistics_t global_stats_intf[PTIN_SYSTEM_N_INTERF];

/* Semaphores */
void *pppoe_sem = NULL;
void *ptin_pppoe_stats_sem = L7_NULLPTR;


static struct ptin_clientIdx_entry_s  clientIdx_pool[PTIN_SYSTEM_PPPOE_MAXCLIENTS];  /* Array with all the indexes of clients to be used in a queue */
static struct ptin_clientInfo_entry_s clientInfo_pool[PTIN_SYSTEM_PPPOE_MAXCLIENTS];

static dl_queue_t queue_free_clients;    /* Queue of free client entries */


/*********************************************************** 
 * Static prototypes
 ***********************************************************/

static L7_RC_t ptin_pppoe_client_find(L7_uint pppoe_idx, ptin_client_id_t *client_ref, ptinPppoeClientInfoData_t **client_info);
static L7_RC_t ptin_pppoe_instance_deleteAll_clients(L7_uint pppoe_idx);
static L7_RC_t ptin_pppoe_inst_get_fromIntVlan(L7_uint16 intVlan, st_PppoeInstCfg_t **pppoeInst, L7_uint *pppoeInst_idx);
static L7_RC_t ptin_pppoe_instance_find_free(L7_uint *idx);
static L7_RC_t ptin_pppoe_instance_find(L7_uint32 evc_idx, L7_uint *pppoe_idx);
static L7_RC_t ptin_pppoe_instance_find_agg(L7_uint16 nni_ovlan, L7_uint *pppoe_idx);
static L7_RC_t ptin_pppoe_evc_trap_configure(L7_uint32 evc_idx, L7_BOOL enable);
static void    ptin_pppoe_evc_ethprty_get(ptin_AccessNodeCircuitId_t *evc_circuitid, L7_uint8 *ethprty);
static L7_RC_t ptin_pppoe_circuitid_set_instance(L7_uint32 pppoe_idx, L7_char8 *template_str, L7_uint32 mask, L7_char8 *access_node_id, L7_uint8 chassis,
                                                 L7_uint8 rack, L7_uint8 frame, L7_uint8 ethernet_priority, L7_uint16 s_vid);
static void    ptin_pppoe_circuitId_build(ptin_AccessNodeCircuitId_t *evc_circuitid, ptin_clientCircuitId_t *client_circuitid, L7_char8 *circuitid);
static void    ptin_pppoe_circuitid_convert(L7_char8 *circuitid_str, L7_char8 *str_to_replace, L7_char8 *parameter);
static L7_RC_t ptin_pppoe_reconf_instance(L7_uint32 pppoe_instance_idx, L7_uint8 pppoe_flag, L7_uint32 options);

#if PPPOE_ACCEPT_UNSTACKED_PACKETS
static L7_RC_t ptin_pppoe_strings_def_get(ptin_intf_t *ptin_intf, L7_uchar8 *macAddr, L7_char8 *circuitId, L7_char8 *remoteId);
#endif

static L7_RC_t ptin_pppoe_clientId_convert(L7_uint32 evc_idx, ptin_client_id_t *client);
//static L7_RC_t ptin_pppoe_clientId_restore(ptin_client_id_t *client);

/*********************************************************** 
 * INLINE FUNCTIONS
 ***********************************************************/

static L7_BOOL pppoe_clientIndex_check_free(L7_uint pppoe_idx)
{
  /* Validate arguments */
  if (pppoe_idx >= PTIN_SYSTEM_N_PPPOE_INSTANCES)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid PPPOE instance %u", pppoe_idx);
    return -1;
  }

  return (pppoeClients_unified.number_of_clients < PTIN_SYSTEM_PPPOE_MAXCLIENTS &&
          queue_free_clients.n_elems > 0);
}

static L7_int pppoe_clientIndex_allocate(L7_uint pppoe_idx, ptinPppoeClientInfoData_t *infoData)
{
  L7_int  client_idx;
  struct ptin_clientIdx_entry_s  *clientIdx_pool_entry;
  struct ptin_clientInfo_entry_s *clientInfo_pool_entry;
  L7_RC_t rc;

  /* Validate arguments */
  if (pppoe_idx >= PTIN_SYSTEM_N_PPPOE_INSTANCES)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid PPPOE instance %u", pppoe_idx);
    return -1;
  }

  /* Check if there is free clients */
  if (pppoeClients_unified.number_of_clients >= PTIN_SYSTEM_PPPOE_MAXCLIENTS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"No free clients available");
    return -1;
  }

  /* Check if queue has free clients */
  if (queue_free_clients.n_elems == 0)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"No free clients available in queue");
    return -1;
  }

  /* Try to get an entry from the pool of free elements */
  rc = dl_queue_remove_head(&queue_free_clients, (dl_queue_elem_t **) &clientIdx_pool_entry);
  if (rc != NOERR) {
    PT_LOG_ERR(LOG_CTX_PPPOE, "There are no free clients available! rc=%d", rc);
    return -1;
  }

  client_idx = clientIdx_pool_entry->client_id;

  PT_LOG_DEBUG(LOG_CTX_PPPOE, "Selected index=%u, Free clients pool: %u of %u entries",
            client_idx, queue_free_clients.n_elems, PTIN_SYSTEM_PPPOE_MAXCLIENTS);

  /* Assign AVL entry reference */
  if (infoData != L7_NULLPTR)
  {
    /* Update clients list on related instance */
    clientInfo_pool_entry = &clientInfo_pool[client_idx];

    memset(clientInfo_pool_entry, 0x00, sizeof(struct ptin_clientInfo_entry_s));
    clientInfo_pool_entry->client_info = infoData;

    rc = dl_queue_add_tail(&pppoeInstances[pppoe_idx].queue_clients, (dl_queue_elem_t *) clientInfo_pool_entry);
    if (rc != NOERR) {
      memset(clientInfo_pool_entry, 0x00, sizeof(struct ptin_clientInfo_entry_s));
      dl_queue_add_head(&queue_free_clients, (dl_queue_elem_t *) clientIdx_pool_entry);
      PT_LOG_ERR(LOG_CTX_PPPOE, "Error adding element to queue! rc=%d", rc);
      return -1;
    }
  }

  /* One more client */
  pppoeClients_unified.number_of_clients++;

  /* Return new client id */
  return client_idx;
}

static void pppoe_clientIndex_release(L7_uint pppoe_idx, L7_uint32 client_idx)
{
  struct ptin_clientIdx_entry_s  *clientIdx_pool_entry;
  struct ptin_clientInfo_entry_s *clientInfo_pool_entry;
  L7_RC_t rc;

  /* Validate arguments */
  if (pppoe_idx >= PTIN_SYSTEM_N_PPPOE_INSTANCES || client_idx >= PTIN_SYSTEM_PPPOE_MAXCLIENTS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE, "Invalid PPPOE instance %u, or client index %u", pppoe_idx, client_idx);
    return;
  }

  /* Check if there is busy clients */
  if (queue_free_clients.n_elems >= PTIN_SYSTEM_PPPOE_MAXCLIENTS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE, "There are no busy clients!");
    return;
  }

  /* Get the client entry based on its index */
  clientIdx_pool_entry  = &clientIdx_pool[client_idx];
  clientInfo_pool_entry = &clientInfo_pool[client_idx];

  /* Remove element from clientInfo queue */
  rc = dl_queue_remove(&pppoeInstances[pppoe_idx].queue_clients, (dl_queue_elem_t *) clientInfo_pool_entry);
  if (rc != NOERR) {
    PT_LOG_ERR(LOG_CTX_PPPOE, "Error removing element from queue! rc=%d", rc);
    return;
  }

  /* Add it to the free queue */
  rc = dl_queue_add_tail(&queue_free_clients, (dl_queue_elem_t *) clientIdx_pool_entry);
  if (rc != NOERR) {
    dl_queue_add_head(&pppoeInstances[pppoe_idx].queue_clients, (dl_queue_elem_t *) clientInfo_pool_entry);
    PT_LOG_ERR(LOG_CTX_PPPOE, "Error adding client to free queue! rc=%d", rc);
    return;
  }

  /* Clear client info from queue */
  memset(&clientInfo_pool[client_idx], 0x00, sizeof(struct ptin_clientInfo_entry_s));

  /* One less client */
  if (pppoeClients_unified.number_of_clients > 0)
    pppoeClients_unified.number_of_clients--;

  PT_LOG_DEBUG(LOG_CTX_EVC, "Free client pool: %u of %u entries",
            queue_free_clients.n_elems, PTIN_SYSTEM_PPPOE_MAXCLIENTS);
}

/*********************************************************** 
 * Global functions
 ***********************************************************/

/**
 * Initialize circuitId+remoteId database
 *  
 * @return none
 */
L7_RC_t ptin_pppoe_init(void)
{
  L7_uint pppoe_idx, i;
  ptinPppoeClientsAvlTree_t *avlTree;

  #if 0
  /* Clear database */
  memset(pppoe_relay_database, 0x00, sizeof(pppoe_relay_database));
  #endif

  /* All ports are untrusted by default */
  ptin_pppoe_intfTrusted_init();

  /* Reset instances array */
  memset(pppoeInstances, 0x00, sizeof(pppoeInstances));

  /* Initialize global PPPOE statistics */
  memset(global_stats_intf,0x00,sizeof(global_stats_intf));

  /* Initialize sempahore */
  pppoe_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (pppoe_sem == NULL)
  {
    PT_LOG_TRACE(LOG_CTX_PPPOE, "Error creating a mutex for PPPOE module");
    return L7_FAILURE;
  }

  /* PPPOE clients */
  avlTree = &pppoeClients_unified.avlTree;
  pppoeClients_unified.number_of_clients = 0;

  avlTree->pppoeClientsTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_PPPOE_MAXCLIENTS * sizeof(avlTreeTables_t)); 
  avlTree->pppoeClientsDataHeap = (ptinPppoeClientInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_PPPOE_MAXCLIENTS * sizeof(ptinPppoeClientInfoData_t)); 

  if ((avlTree->pppoeClientsTreeHeap == L7_NULLPTR) ||
      (avlTree->pppoeClientsDataHeap == L7_NULLPTR))
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Error allocating data for PPPOE AVL Trees\n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset (&avlTree->pppoeClientsAvlTree, 0x00, sizeof(avlTree_t));
  memset (avlTree->pppoeClientsTreeHeap, 0x00, sizeof(avlTreeTables_t)*PTIN_SYSTEM_PPPOE_MAXCLIENTS);
  memset (avlTree->pppoeClientsDataHeap, 0x00, sizeof(ptinPppoeClientInfoData_t)*PTIN_SYSTEM_PPPOE_MAXCLIENTS);

  // AVL Tree creations - snoopIpAvlTree
  avlCreateAvlTree(&(avlTree->pppoeClientsAvlTree),
                   avlTree->pppoeClientsTreeHeap,
                   avlTree->pppoeClientsDataHeap,
                   PTIN_SYSTEM_PPPOE_MAXCLIENTS, 
                   sizeof(ptinPppoeClientInfoData_t),
                   0x10,
                   sizeof(ptinPppoeClientDataKey_t));

  /* Initialize clients queue for each PPPOE instance */
  memset(clientInfo_pool, 0x00, sizeof(clientInfo_pool));
  for (pppoe_idx = 0; pppoe_idx < PTIN_SYSTEM_N_PPPOE_INSTANCES; pppoe_idx++)
  {
    dl_queue_init(&pppoeInstances[pppoe_idx].queue_clients);
  }

  /* Init Client index management */
  dl_queue_init(&queue_free_clients);
  memset(clientIdx_pool, 0x00, sizeof(clientIdx_pool));
  for (i=0; i<PTIN_SYSTEM_PPPOE_MAXCLIENTS; i++)
  {
    clientIdx_pool[i].client_id = i;
    dl_queue_add(&queue_free_clients, (dl_queue_elem_t*) &clientIdx_pool[i]);
  }

  /* Semaphores */
  ptin_pppoe_stats_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_pppoe_stats_sem == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Failed to create ptin_pppoe_stats_sem semaphore!");
    return L7_FAILURE;
  }

  PT_LOG_INFO(LOG_CTX_PPPOE, "sizeof(pppoe_intf_trusted)          = %zu", sizeof(pppoe_intf_trusted));
  PT_LOG_INFO(LOG_CTX_PPPOE, "sizeof(pppoeInstances)              = %zu", sizeof(pppoeInstances));
  PT_LOG_INFO(LOG_CTX_PPPOE, "sizeof(global_stats_intf)           = %zu", sizeof(global_stats_intf));
  PT_LOG_INFO(LOG_CTX_PPPOE, "sizeof(pppoeClients_unified.avlTree)= %zu",
              sizeof(avlTree_t) + sizeof(avlTreeTables_t)*PTIN_SYSTEM_PPPOE_MAXCLIENTS + sizeof(ptinPppoeClientInfoData_t)*PTIN_SYSTEM_PPPOE_MAXCLIENTS);

  PT_LOG_INFO(LOG_CTX_PPPOE, "PPPOE init OK");

  /* To be removed */
#if 0
  if (usmDbDsAdminModeSet(L7_ENABLE)!=L7_SUCCESS)  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Error with usmDbDsAdminModeSet");
    return L7_FAILURE;
  }
  if (usmDbDsL2RelayAdminModeSet(L7_ENABLE)!=L7_SUCCESS)  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Error with usmDbDsL2RelayAdminModeSet");
    return L7_FAILURE;
  }
#endif

  return L7_SUCCESS;
}

/**
 * Set Global enable for PPPOE packet trapping
 * 
 * @param enable : L7_ENABLE/L7_DISABLE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_enable(L7_BOOL enable)
{
  if (ptin_pppoePkts_global_trap(enable)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Error setting PPPOE global enable to %u",enable);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_PPPOE,"Success setting PPPOE global enable to %u",enable);

#if (PTIN_QUATTRO_FLOWS_FEATURE_ENABLED && QUATTRO_PPPOE_TRAP_PREACTIVE)
  /* Configure packet trapping for this VLAN  */
  if (ptin_pppoePkts_vlan_trap(PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN, enable) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Error configuring packet trapping for QUATTRO VLANs (enable=%u)", enable);
    ptin_pppoePkts_global_trap(!enable);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_PPPOE,"Packet trapping for QUATTRO VLANs configured (enable=%u)", enable);
#endif

  return L7_SUCCESS;
}

/**
 * Check if a EVC is being used in an PPPOE instance
 * 
 * @param evcId : evc id
 * 
 * @return L7_RC_t : L7_TRUE or L7_FALSE
 */
L7_RC_t ptin_pppoe_is_evc_used(L7_uint32 evcId)
{
  /* Validate arguments */
  if (evcId>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid eEVC id: evcId=%u",evcId);
    return L7_FALSE;
  }

  /* This evc must be active */
  if (!ptin_evc_is_in_use(evcId))
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"eEVC id is not active: evcId=%u",evcId);
    return L7_FALSE;
  }

  /* Check if this EVC is being used by any PPPOE instance */
  if (ptin_pppoe_instance_find(evcId,L7_NULLPTR)!=L7_SUCCESS)
    return L7_FALSE;

  return L7_TRUE;
}

/**
 * Associate an EVC to a PPPOE instance
 * 
 * @param evc_idx : Unicast evc id 
 * @param nni_ovlan  : NNI outer vlan
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_evc_add(L7_uint32 evc_idx, L7_uint16 nni_ovlan)
{
  L7_uint   pppoe_idx;
  L7_uint8  evc_type;
  L7_BOOL   new_evc = L7_FALSE, new_instance = L7_FALSE;
  L7_RC_t   rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid eEVC id: ucEvcId=%u",evc_idx);
    return L7_FAILURE;
  }

  /* These evcs must be active */
  if (!ptin_evc_is_in_use(evc_idx))
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"eEVC id is not active: ucEvcId%u",evc_idx);
    return L7_FAILURE;
  }

  /* Get EVC type */
  if (ptin_evc_check_evctype(evc_idx, &evc_type) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Error getting eEVC %u type",evc_idx);
    return L7_FAILURE;
  }

  #if 0
  /* If EVC is not QUATTRO pointo-to-point, use tradittional isnatnce management */
  if (evc_type!=PTIN_EVC_TYPE_QUATTRO_STACKED)
  {
    nni_ovlan = 0;
  }
  #endif

  /* Check if there is an instance with these parameters */
  if (ptin_pppoe_instance_find(evc_idx, &pppoe_idx) != L7_SUCCESS)
  {
    new_evc = L7_TRUE;

    /* Check if there is an instance with the same NNI outer vlan: use it! */
    /* Otherwise, create a new instance */
    if ((nni_ovlan < PTIN_VLAN_MIN || nni_ovlan > PTIN_VLAN_MAX) ||
        ptin_pppoe_instance_find_agg(nni_ovlan, &pppoe_idx)!=L7_SUCCESS)
    {
      /* Find an empty instance to be used */
      if (ptin_pppoe_instance_find_free(&pppoe_idx)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_PPPOE,"There is no free instances to be used");
        return L7_FAILURE;
      }
      else
      {
        new_instance = L7_TRUE;
      }
    }

    /* Save direct referencing to pppoe index from evc ids */
    if (ptin_evc_pppoeInst_set(evc_idx, pppoe_idx) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_IGMP,"Error setting PPPoE instance to ext evc id %u", evc_idx);
      return L7_FAILURE;
    }

    if (new_instance)
    {
      /* Save data in free instance */
      pppoeInstances[pppoe_idx].nni_ovid  = (nni_ovlan>=PTIN_VLAN_MIN && nni_ovlan<=PTIN_VLAN_MAX) ? nni_ovlan : 0;;
      pppoeInstances[pppoe_idx].n_evcs    = 0;
      pppoeInstances[pppoe_idx].inUse     = L7_TRUE;
    }
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_PPPOE,"There is already an instance with eEvcId%u", evc_idx);
  }

  /* Configure querier for this instance */
  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  if (evc_type != PTIN_EVC_TYPE_QUATTRO_STACKED || pppoe_quattro_stacked_evcs == 0)
  #endif
  {
    rc = ptin_pppoe_evc_trap_configure(evc_idx, L7_ENABLE);

    if (rc != L7_SUCCESS) 
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Error configuring PPPOE snooping for pppoe_idx=%u",pppoe_idx);
      memset(&pppoeInstances[pppoe_idx], 0x00, sizeof(st_PppoeInstCfg_t));
      ptin_evc_pppoeInst_set(evc_idx, PPPOE_INVALID_ENTRY);
      return L7_FAILURE;
    }
  }

  if (new_evc)
  {
    /* One more EVC associated to this instance */
    pppoeInstances[pppoe_idx].n_evcs++;

    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    /* Update number of QUATTRO-P2P evcs */
    if (evc_type == PTIN_EVC_TYPE_QUATTRO_STACKED)
    {
      pppoe_quattro_stacked_evcs++;
    }
    #endif
  }

  return L7_SUCCESS;
}

/**
 * Deassociate an EVC from a PPPOE instance
 * 
 * @param evc_idx : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_evc_remove(L7_uint32 evc_idx)
{
  L7_uint   pppoe_idx;
  L7_uint8  evc_type;
  L7_uint16 nni_ovid;
  L7_BOOL remove_instance = L7_TRUE;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid eEVC ids: ucEvcId=%u",evc_idx);
    return L7_FAILURE;
  }

  /* Check if there is an instance with these parameters */
  if (ptin_pppoe_instance_find(evc_idx, &pppoe_idx)!=L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_PPPOE,"There is no instance with ucEvcId=%u",evc_idx);
    return L7_SUCCESS;
  }

  /* Get EVC type */
  if (ptin_evc_check_evctype(evc_idx, &evc_type) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Error getting eEVC %u type", evc_idx);
    return L7_FAILURE;
  }

  /* Remove instance? */
  remove_instance = ((pppoeInstances[pppoe_idx].nni_ovid==0 || pppoeInstances[pppoe_idx].nni_ovid>4095) ||
                     (pppoeInstances[pppoe_idx].n_evcs <= 1));

  /* NNI outer vlan */
  nni_ovid = pppoeInstances[pppoe_idx].nni_ovid;

  /* Configure packet trapping for this instance */
  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  if (evc_type != PTIN_EVC_TYPE_QUATTRO_STACKED || pppoe_quattro_stacked_evcs <= 1)
  #endif
  {
    if (ptin_pppoe_evc_trap_configure(evc_idx, L7_DISABLE)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Error unconfiguring PPPOE snooping for evc_idx=%u",evc_idx);
      return L7_FAILURE;
    }
  }

  /* Remove pppoe clients */
  if (remove_instance)
  {
    /* Remove all clients attached to this instance */
    if (ptin_pppoe_instance_deleteAll_clients(pppoe_idx)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Error removing all clients from pppoe_idx %u (evc_idx=%u)",pppoe_idx,evc_idx);
      return L7_FAILURE;
    }
  }

  /* Reset direct referencing to pppoe index from evc ids */
  if (ptin_evc_pppoeInst_set(evc_idx, PPPOE_INVALID_ENTRY) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Error resetting PPPoE instance to ext evc id %u", evc_idx);
    return L7_FAILURE;
  }

    /* Only clear instance, if there is no one using this NNI outer vlan */
  if ( remove_instance )
  {
    /* Clear data and free instance */
    pppoeInstances[pppoe_idx].nni_ovid  = 0;
    pppoeInstances[pppoe_idx].n_evcs    = 0;
    pppoeInstances[pppoe_idx].inUse     = L7_FALSE;
  }

  /* One less EVC */
  if (pppoeInstances[pppoe_idx].n_evcs > 0)
    pppoeInstances[pppoe_idx].n_evcs--;

  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  /* Update number of QUATTRO-P2P evcs */
  if (evc_type == PTIN_EVC_TYPE_QUATTRO_STACKED)
  {
    if (pppoe_quattro_stacked_evcs>0)  pppoe_quattro_stacked_evcs--;
  }
  #endif

  return L7_SUCCESS;
}

/**
 * Update PPPOE entries, when EVCs are deleted
 * 
 * @param evcId : evc index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_evc_destroy(L7_uint32 evcId)
{
  return ptin_pppoe_evc_remove(evcId);
}

/**
 * Reconfigure global PPPOE EVC
 *
 * @param evcId         : evc index
 * @param pppoe_flag     : PPPOE flag (not used)
 * @param options       : options
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_reconf_evc(L7_uint32 evcId, L7_uint8 pppoe_flag, L7_uint32 options)
{
   L7_uint pppoe_idx;

   /* Get PPPOE instance index */
   if (ptin_pppoe_instance_find(evcId, &pppoe_idx) != L7_SUCCESS)
   {
     if (ptin_debug_pppoe_snooping)
       PT_LOG_WARN(LOG_CTX_PPPOE, "There is no PPPOE instance with EVC id %u", evcId);
    return L7_SUCCESS;
   }

   return ptin_pppoe_reconf_instance(pppoe_idx, pppoe_flag, options);
}

/**
 * Reconfigure global PPPOE EVC (using root vlan)
 *
 * @param rootVid       : root vlan
 * @param pppoe_flag    : PPPOE flag (not used)
 * @param options       : options
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_reconf_rootVid(L7_uint32 rootVid, L7_uint8 pppoe_flag, L7_uint32 options)
{
  L7_uint pppoe_idx;

  /* Get PPPOE instance index */
  if (ptin_pppoe_instance_find_agg(rootVid, &pppoe_idx) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE, "There is no PPPOE instance for root Vid %u", rootVid);
    return L7_NOT_EXIST;
  }

  return ptin_pppoe_reconf_instance(pppoe_idx, pppoe_flag, options);
}

/**
 * Set PPPOE circuit-id global data from NNI SVlan
 *
 * @param nni_outerVid    : NNI STAG
 * @param template_str    : Circuit-id template string
 * @param mask            : Circuit-id mask
 * @param access_node_id  : Access Node ID
 * @param chassis         : Access Node Chassis
 * @param rack            : Access Node Rack
 * @param frame           : Access Node Frame
 * @param slot            : Access Node Chassis/Rack/Frame Slot
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_circuitid_set_nniVid(L7_uint16 nni_outerVid, L7_char8 *template_str, L7_uint32 mask, L7_char8 *access_node_id, L7_uint8 chassis,
                                        L7_uint8 rack, L7_uint8 frame, L7_uint8 ethernet_priority, L7_uint16 s_vid)
{
  L7_uint pppoe_idx;

  /* Get PPPOE instance index */
  if (ptin_pppoe_instance_find_agg(nni_outerVid, &pppoe_idx) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE, "There is no PPPOE instance for NNI Vid %u", nni_outerVid);
    return L7_FAILURE;
  }

  return ptin_pppoe_circuitid_set_instance(pppoe_idx, template_str, mask, access_node_id, chassis, rack, frame, ethernet_priority, s_vid);
}

/**
 * Set PPPOE circuit-id global data from EVC id
 *
 * @param evc_idx         : evc index
 * @param template_str    : Circuit-id template string
 * @param mask            : Circuit-id mask
 * @param access_node_id  : Access Node ID
 * @param chassis         : Access Node Chassis
 * @param rack            : Access Node Rack
 * @param frame           : Access Node Frame
 * @param slot            : Access Node Chassis/Rack/Frame Slot
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_circuitid_set_evc(L7_uint32 evc_idx, L7_char8 *template_str, L7_uint32 mask, L7_char8 *access_node_id, L7_uint8 chassis,
                                     L7_uint8 rack, L7_uint8 frame, L7_uint8 ethernet_priority, L7_uint16 s_vid)
{
  L7_uint pppoe_idx;

  /* Get PPPOE instance index */
  if (ptin_pppoe_instance_find(evc_idx, &pppoe_idx) != L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_WARN(LOG_CTX_PPPOE, "There is no PPPOE instance with EVC id %u", evc_idx);
    return L7_SUCCESS;
  }

  return ptin_pppoe_circuitid_set_instance(pppoe_idx, template_str, mask, access_node_id, chassis, rack, frame, ethernet_priority, s_vid);
}

/**
 * Set PPPOE circuit-id global data
 *
 * @param pppoe_idx       : PPPoE instance index
 * @param template_str    : Circuit-id template string
 * @param mask            : Circuit-id mask
 * @param access_node_id  : Access Node ID
 * @param chassis         : Access Node Chassis
 * @param rack            : Access Node Rack
 * @param frame           : Access Node Frame
 * @param slot            : Access Node Chassis/Rack/Frame Slot
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_circuitid_set_instance(L7_uint32 pppoe_idx, L7_char8 *template_str, L7_uint32 mask, L7_char8 *access_node_id, L7_uint8 chassis,
                                          L7_uint8 rack, L7_uint8 frame, L7_uint8 ethernet_priority, L7_uint16 s_vid)
{
  ptinPppoeClientDataKey_t avl_key;
  ptinPppoeClientInfoData_t *avl_info;

  /* Validate arguments */
  if (template_str == L7_NULLPTR || access_node_id == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE, "Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }
  /* Validate string lengths */
  if ( strnlen(template_str, CIRCUITID_TEMPLATE_MAX_STRING) >= CIRCUITID_TEMPLATE_MAX_STRING )
  {
    PT_LOG_ERR(LOG_CTX_PPPOE, "Template string length is invalid!");
    return L7_FAILURE;
  }
  if ( strnlen(access_node_id, FD_DS_MAX_REMOTE_ID_STRING) >= FD_DS_MAX_REMOTE_ID_STRING )
  {
    PT_LOG_ERR(LOG_CTX_PPPOE, "Access node identifier length is invalid!");
    return L7_FAILURE;
  }

  /* Get PPPOE instance index */
  if (pppoe_idx >= PTIN_SYSTEM_N_PPPOE_INSTANCES)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE, "Invalid PPPOE instance %u", pppoe_idx);
    return L7_FAILURE;
  }

  /* Validate pppoe instance */
  if (!pppoeInstances[pppoe_idx].inUse)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE, "PPPOE instance %u is not in use", pppoe_idx);
    return L7_FAILURE;
  }

  strncpy(pppoeInstances[pppoe_idx].circuitid.template_str, template_str, CIRCUITID_TEMPLATE_MAX_STRING);
  pppoeInstances[pppoe_idx].circuitid.template_str[CIRCUITID_TEMPLATE_MAX_STRING-1] = '\0';      /* Just to be sure */

  pppoeInstances[pppoe_idx].circuitid.mask                = mask;

  strncpy(pppoeInstances[pppoe_idx].circuitid.access_node_id, access_node_id, FD_DS_MAX_REMOTE_ID_STRING);
  pppoeInstances[pppoe_idx].circuitid.access_node_id[FD_DS_MAX_REMOTE_ID_STRING-1] = '\0';       /* Just to be sure */

  pppoeInstances[pppoe_idx].circuitid.chassis             = chassis;
  pppoeInstances[pppoe_idx].circuitid.rack                = rack;
  pppoeInstances[pppoe_idx].circuitid.frame               = frame;
  pppoeInstances[pppoe_idx].circuitid.ethernet_priority   = 0x7 & ethernet_priority;
  pppoeInstances[pppoe_idx].circuitid.s_vid               = s_vid;

  /* TODO: Run all clients of this PPPOE instance, and rebuild their circuit id strings */

  /* Run all cells in AVL tree */
  memset(&avl_key, 0x00, sizeof(ptinPppoeClientDataKey_t));
  while ( ( avl_info = (ptinPppoeClientInfoData_t *)
                        avlSearchLVL7(&pppoeClients_unified.avlTree.pppoeClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->pppoeClientDataKey, sizeof(ptinPppoeClientDataKey_t));

    /* Only apply for this instance */
    if (avl_key.pppoe_instance != pppoe_idx)
      continue;

    /* Rebuild circuit id */
    ptin_pppoe_circuitId_build( &pppoeInstances[pppoe_idx].circuitid, &avl_info->client_data.circuitId, avl_info->client_data.circuitId_str);
  }

  return L7_SUCCESS;
}

/**
 * Get PPPOE circuit-id global data
 *
 * @param evcId           : evc index
 * @param template_str    : Circuit-id template string
 * @param mask            : Circuit-id mask
 * @param access_node_id  : Access Node ID
 * @param chassis         : Access Node Chassis
 * @param rack            : Access Node Rack
 * @param frame           : Access Node Frame
 * @param slot            : Access Node Chassis/Rack/Frame Slot
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_circuitid_get(L7_uint32 evcId, L7_char8 *template_str, L7_uint32 *mask, L7_char8 *access_node_id, L7_uint8 *chassis,
                                L7_uint8 *rack, L7_uint8 *frame, L7_uint8 *ethernet_priority, L7_uint16 *s_vid)
{
  L7_uint pppoe_idx;

  /* Validate arguments */
  if (template_str == L7_NULLPTR || access_node_id == L7_NULLPTR || mask == 0x00)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE, "Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Get PPPOE instance index */
  if (ptin_pppoe_instance_find(evcId, &pppoe_idx) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE, "There is no PPPOE instance with EVC id %u", evcId);
    return L7_FAILURE;
  }

  /* Validate pppoe instance */
  if (!pppoeInstances[pppoe_idx].inUse)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE, "PPPOE instance %u is not in use", pppoe_idx);
    return L7_FAILURE;
  }

  strncpy(template_str,   pppoeInstances[pppoe_idx].circuitid.template_str,   CIRCUITID_TEMPLATE_MAX_STRING);
  template_str[CIRCUITID_TEMPLATE_MAX_STRING-1] = '\0';
  strncpy(access_node_id, pppoeInstances[pppoe_idx].circuitid.access_node_id, FD_DS_MAX_REMOTE_ID_STRING   );
  access_node_id[FD_DS_MAX_REMOTE_ID_STRING -1] = '\0';
  *mask              = pppoeInstances[pppoe_idx].circuitid.mask;
  *chassis           = pppoeInstances[pppoe_idx].circuitid.chassis;
  *rack              = pppoeInstances[pppoe_idx].circuitid.rack;
  *frame             = pppoeInstances[pppoe_idx].circuitid.frame;
  *ethernet_priority = pppoeInstances[pppoe_idx].circuitid.ethernet_priority;
  *s_vid             = pppoeInstances[pppoe_idx].circuitid.s_vid;

  return L7_SUCCESS;
}

/**
 * Get PPPOE client data (circuit and remote ids)
 * 
 * @param evc_idx        : Unicast evc id
 * @param client            : client identification parameters
 * @param options           : PPPOE options
 * @param circuitId_data    : Circuit ID data 
 * @param circuitId         : Circuit ID string
 * @param remoteId          : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_client_get(L7_uint32 evc_idx, ptin_client_id_t *client, L7_uint16 *options,
                              ptin_clientCircuitId_t *circuitId_data,
                              L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_uint pppoe_idx;
  ptinPppoeClientInfoData_t *client_info;

  /* Validate arguments */
  if (client==L7_NULLPTR || PPPOE_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Get PPPOE instance index */
  if (ptin_pppoe_instance_find(evc_idx, &pppoe_idx)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"There is no PPPOE instance with EVC id %u",evc_idx);
    return L7_FAILURE;
  }

  /* Find client information */
  if (ptin_pppoe_client_find(pppoe_idx,client,&client_info)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Non existent client in PPPOE instance %u (EVC id %u)",pppoe_idx,evc_idx);
    return L7_FAILURE;
  }

  /* Return string ids */
  if (options!=L7_NULLPTR)
  {
     *options = client_info->client_data.pppoe_options;
  }

  if (circuitId_data != L7_NULLPTR)
  {
    circuitId_data->onuid = client_info->client_data.circuitId.onuid;
    circuitId_data->slot  = client_info->client_data.circuitId.slot;
    circuitId_data->port  = client_info->client_data.circuitId.port;
    circuitId_data->q_vid = client_info->client_data.circuitId.q_vid;
    circuitId_data->c_vid = client_info->client_data.circuitId.c_vid;
  }

  if (circuitId!=L7_NULLPTR)
  {
    strncpy(circuitId ,client_info->client_data.circuitId_str ,FD_DS_MAX_REMOTE_ID_STRING);
    circuitId[FD_DS_MAX_REMOTE_ID_STRING-1] = '\0';
    PT_LOG_TRACE(LOG_CTX_PPPOE, "%s", circuitId);
  }
  if (remoteId!=L7_NULLPTR)
  {
    strncpy(remoteId ,client_info->client_data.remoteId_str ,FD_DS_MAX_REMOTE_ID_STRING);
    remoteId[FD_DS_MAX_REMOTE_ID_STRING-1] = '\0';
    PT_LOG_TRACE(LOG_CTX_PPPOE, "%s", remoteId);
  }

  return L7_SUCCESS;
}

/**
 * Add a new PPPOE client
 * 
 * @param evc_idx        : Unicast evc id
 * @param client            : client identification parameters 
 * @param uni_ovid          : External outer vlan 
 * @param uni_ivid          : External inner vlan  
 * @param options           : PPPOE options
 * @param circuitId         : Circuit ID data 
 * @param remoteId          : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_client_add(L7_uint32 evc_idx, const ptin_client_id_t *client_cfg, L7_uint16 uni_ovid, L7_uint16 uni_ivid,
                              L7_uint16 options, ptin_clientCircuitId_t *circuitId, L7_char8 *remoteId)
{
  ptin_client_id_t client;
  L7_uint pppoe_idx;
  L7_int  client_idx = -1;
  ptinPppoeClientDataKey_t avl_key;
  ptinPppoeClientsAvlTree_t *avl_tree;
  ptinPppoeClientInfoData_t *avl_infoData;
#if (PPPOE_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port;
  L7_uint32 intIfNum;
  ptin_evc_intfCfg_t intfCfg;
#endif

  PT_LOG_ERR(LOG_CTX_PPPOE,"Adding PPPoE at evc_idx=%u, uni_ovid=%u, uni_ivid=%u", evc_idx, uni_ovid, uni_ivid);

  /* Validate arguments */
  if (client_cfg == L7_NULLPTR || PPPOE_CLIENT_MASK_UPDATE(client_cfg->mask) == 0x00)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Get PPPOE instance index */
  if (ptin_pppoe_instance_find(evc_idx, &pppoe_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
       PT_LOG_WARN(LOG_CTX_PPPOE,"There is no PPPOE instance with EVC id %u",evc_idx);
    return L7_SUCCESS;
  }

  memcpy(&client, client_cfg, sizeof(ptin_client_id_t));

  /* Validate and rearrange clientId info */
  if (ptin_pppoe_clientId_convert(evc_idx, &client) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid client id");
    return L7_FAILURE;
  }

  /* Get ptin_port value */
#if (PPPOE_CLIENT_INTERF_SUPPORTED)
  ptin_port = (L7_uint32)-1;
  if (client.mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    /* Get interface configuration in the UC EVC */
    if (ptin_evc_intfCfg_get(evc_idx, &client.ptin_intf, &intfCfg)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Error acquiring intf configuration for intf %u/%u, evc=%u",client.ptin_intf.intf_type,client.ptin_intf.intf_id,evc_idx);
      return L7_FAILURE;
    }
    /* Validate interface configuration in EVC: must be in use, and be a leaf/client */
    if (!intfCfg.in_use || intfCfg.type!=PTIN_EVC_INTF_LEAF)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"intf %u/%u is not in use or is not a leaf in evc %u",client.ptin_intf.intf_type,client.ptin_intf.intf_id,evc_idx);
      return L7_FAILURE;
    }

    /* Get ptin_port */
    ptin_port = client.ptin_port;

    /* Convert to intIfNum format */
    if (ptin_intf_ptintf2intIfNum(&client.ptin_intf, &intIfNum)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Cannot convert client intf %u/%u to intIfNum format",client.ptin_intf.intf_type,client.ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
#endif

  /* If uni vlans are not provided, but interface is, get uni vlans from EVC data */
  if ( (uni_ovid<PTIN_VLAN_MIN || uni_ovid>PTIN_VLAN_MAX) &&
       (client.mask & PTIN_CLIENT_MASK_FIELD_INTF) &&
       (client.mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) )
  {
    if (ptin_port < PTIN_SYSTEM_N_INTERF)
    {
      if (ptin_evc_extVlans_get(ptin_port, evc_idx,(L7_uint32)-1, client.innerVlan, &uni_ovid, &uni_ivid) == L7_SUCCESS)
      {
        PT_LOG_TRACE(LOG_CTX_PPPOE,"Ext vlans for ptin_intf %u/%u, cvlan %u: uni_ovid=%u, uni_ivid=%u",
                  client.ptin_intf.intf_type,client.ptin_intf.intf_id, client.innerVlan, uni_ovid, uni_ivid);
      }
      else
      {
        uni_ovid = uni_ivid = 0;
        PT_LOG_ERR(LOG_CTX_PPPOE,"Cannot get ext vlans for ptin_intf %u/%u, cvlan %u",
                client.ptin_intf.intf_type,client.ptin_intf.intf_id, client.innerVlan);
      }
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid ptin_port %u", ptin_port);
    }
  }

  /* Check if this key already exists */
  avl_tree = &pppoeClients_unified.avlTree;

  memset(&avl_key,0x00,sizeof(ptinPppoeClientDataKey_t));

  avl_key.pppoe_instance = pppoe_idx;
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  avl_key.intIfNum = intIfNum;
  #endif
  #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
  avl_key.outerVlan = (client.mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN) ? client.outerVlan : 0;
  #endif
  #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
  avl_key.innerVlan = (client.mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) ? client.innerVlan : 0;
  #endif
  #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
  avl_key.ipv4_addr = (client.mask & PTIN_CLIENT_MASK_FIELD_IPADDR   ) ? client.ipv4_addr : 0;
  #endif
  #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
  if (client.mask & PTIN_CLIENT_MASK_FIELD_MACADDR)
    memcpy(avl_key.macAddr,client.macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  else
    memset(avl_key.macAddr,0x00,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  #endif

  #if (PPPOE_CLIENT_DEBUG)
  PT_LOG_TRACE(LOG_CTX_PPPOE,"Key {"
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                              "intIfNum=%u,"
            #endif
            #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                              "svlan=%u,"
            #endif
            #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                              "cvlan=%u,"
            #endif
            #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                              "ipAddr=%u.%u.%u.%u,"
            #endif
            #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
            #endif
                              "} will be added to pppoe_idx=%u",
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
            avl_key.intIfNum,
            #endif
            #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
            avl_key.outerVlan,
            #endif
            #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
            avl_key.innerVlan,
            #endif
            #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
            (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
            #endif
            #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
            avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
            #endif
            pppoe_idx);
  #endif

  /* Check if this key already exists */
  if ((avl_infoData=(ptinPppoeClientInfoData_t *) avlSearchLVL7( &(avl_tree->pppoeClientsAvlTree), (void *)&avl_key, AVL_EXACT))!=L7_NULLPTR)
  {
    PT_LOG_WARN(LOG_CTX_PPPOE,"This key {"
                #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                                  "intIfNum=%u,"
                #endif
                #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                                  "svlan=%u,"
                #endif
                #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                                  "cvlan=%u,"
                #endif
                #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                                  "ipAddr=%u.%u.%u.%u,"
                #endif
                #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                                  "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
                #endif
                                  "} already exists in pppoe_idx=%u",
                #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                avl_key.intIfNum,
                #endif
                #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                avl_key.outerVlan,
                #endif
                #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                avl_key.innerVlan,
                #endif
                #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
                #endif
                #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
                #endif
                pppoe_idx);
  }
  /* New client */
  else
  {
    /* Check if there is free clients */
    if ( !pppoe_clientIndex_check_free(pppoe_idx) )
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"There is no more free clients to be allocated for pppoe_idx=%u (evc=%u)", pppoe_idx, evc_idx);
      return L7_FAILURE;
    }

    /* Insert entry in AVL tree */
    if (avlInsertEntry(&(avl_tree->pppoeClientsAvlTree), (void *)&avl_key)!=L7_NULLPTR)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Error inserting key {"
              #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                                "intIfNum=%u,"
              #endif
              #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                                "svlan=%u,"
              #endif
              #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                                "cvlan=%u,"
              #endif
              #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                                "ipAddr=%u.%u.%u.%u,"
              #endif
              #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                                "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
              #endif
                                "} in pppoe_idx=%u",
              #if (PPPOE_CLIENT_INTERF_SUPPORTED)
              avl_key.intIfNum,
              #endif
              #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
              avl_key.outerVlan,
              #endif
              #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
              avl_key.innerVlan,
              #endif
              #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
              (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
              #endif
              #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
              avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
              #endif
              pppoe_idx);
      return L7_FAILURE;
    }

    /* Find the inserted entry */
    if ((avl_infoData=(ptinPppoeClientInfoData_t *) avlSearchLVL7(&(avl_tree->pppoeClientsAvlTree),(void *)&avl_key, AVL_EXACT))==L7_NULLPTR)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Cannot find key {"
              #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                                "intIfNum=%u,"
              #endif
              #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                                "svlan=%u,"
              #endif
              #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                                "cvlan=%u,"
              #endif
              #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                                "ipAddr=%u.%u.%u.%u,"
              #endif
              #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                                "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
              #endif
                                "} in pppoe_idx=%u",
              #if (PPPOE_CLIENT_INTERF_SUPPORTED)
              avl_key.intIfNum,
              #endif
              #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
              avl_key.outerVlan,
              #endif
              #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
              avl_key.innerVlan,
              #endif
              #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
              (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
              #endif
              #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
              avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
              #endif
              pppoe_idx);
      return L7_FAILURE;
    }

    /* Allocate new client index */
    client_idx = pppoe_clientIndex_allocate(pppoe_idx, avl_infoData);

    if (client_idx < 0 || client_idx >= PTIN_SYSTEM_PPPOE_MAXCLIENTS)
    {
      avlDeleteEntry(&(avl_tree->pppoeClientsAvlTree), (void *)&avl_key);
      PT_LOG_ERR(LOG_CTX_PPPOE,"Error obtaining new client index for pppoe_idx=%u (evc=%u)", pppoe_idx, evc_idx);
      return L7_FAILURE;
    }

    /* Client index */
    avl_infoData->client_index = client_idx;
    avl_infoData->ptin_port = ptin_port;

    /* Save UNI vlans (external vlans used for transmission) */
    avl_infoData->uni_ovid = uni_ovid;
    avl_infoData->uni_ivid = uni_ivid;

    /* Clear circuit and remote id strings */
    memset(&avl_infoData->client_data,0x00,sizeof(ptinPppoeData_t));

    /* Clear pppoe statistics */
    osapiSemaTake(ptin_pppoe_stats_sem,-1);
    memset(&avl_infoData->client_stats,0x00,sizeof(ptin_PPPOE_Statistics_t));
    osapiSemaGive(ptin_pppoe_stats_sem);
  }

  /* Fill PPPOE options, circuit and remote id fields */
  if( ((options&0x02) >> 1) == 1 ) // Check if this client is using the EVC options
  {
     avl_infoData->client_data.useEvcPppoeOptions   = L7_TRUE;
     avl_infoData->client_data.pppoe_options        = pppoeInstances[pppoe_idx].evcPppoeOptions;
  }
  else
  {
     avl_infoData->client_data.useEvcPppoeOptions   = L7_FALSE;
     avl_infoData->client_data.pppoe_options        = 0;
     avl_infoData->client_data.pppoe_options        |= (options & 0x0001);
     avl_infoData->client_data.pppoe_options        |= (options & 0x0004) >> 1;
     avl_infoData->client_data.pppoe_options        |= (options & 0x0010) >> 2;
  }

  avl_infoData->client_data.circuitId.onuid  = circuitId->onuid;
  avl_infoData->client_data.circuitId.slot   = circuitId->slot;
  avl_infoData->client_data.circuitId.port   = circuitId->port + 1;
  avl_infoData->client_data.circuitId.q_vid  = circuitId->q_vid;
  avl_infoData->client_data.circuitId.c_vid  = circuitId->c_vid;

  /* Build circuit id for this client */
  avl_infoData->client_data.circuitId_str[0] = '\0';
  ptin_pppoe_circuitId_build( &pppoeInstances[pppoe_idx].circuitid, &avl_infoData->client_data.circuitId, avl_infoData->client_data.circuitId_str );

  strncpy(avl_infoData->client_data.remoteId_str ,remoteId ,FD_DS_MAX_REMOTE_ID_STRING);
  avl_infoData->client_data.remoteId_str[FD_DS_MAX_REMOTE_ID_STRING-1] = '\0';

  PT_LOG_TRACE(LOG_CTX_PPPOE,"Success inserting Key {"
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                              "intIfNum=%u,"
            #endif
            #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                              "svlan=%u,"
            #endif
            #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                              "cvlan=%u,"
            #endif
            #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                              "ipAddr=%u.%u.%u.%u,"
            #endif
            #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
            #endif
                              "} in pppoe_idx=%u",
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
            avl_key.intIfNum,
            #endif
            #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
            avl_key.outerVlan,
            #endif
            #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
            avl_key.innerVlan,
            #endif
            #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
            (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
            #endif
            #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
            avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
            #endif
            pppoe_idx);

  return L7_SUCCESS;
}

/**
 * Remove a PPPOE client
 * 
 * @param evc_idx  : Unicast evc id
 * @param client      : client identification parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_client_delete(L7_uint32 evc_idx, const ptin_client_id_t *client_id)
{
  ptin_client_id_t client;
  L7_uint pppoe_idx, client_idx;
  ptinPppoeClientDataKey_t avl_key;
  ptinPppoeClientsAvlTree_t *avl_tree;
  ptinPppoeClientInfoData_t *avl_infoData;
#if (PPPOE_CLIENT_INTERF_SUPPORTED)
  L7_uint32 intIfNum;
#endif

  /* Validate arguments */
  if (client_id == L7_NULLPTR || PPPOE_CLIENT_MASK_UPDATE(client_id->mask) == 0x00)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Get PPPOE instance index */
  if (ptin_pppoe_instance_find(evc_idx, &pppoe_idx)!=L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_PPPOE,"There is no PPPOE instance with EVC id %u",evc_idx);
    return L7_NOT_EXIST;
  }

  memcpy(&client, client_id, sizeof(ptin_client_id_t));

  /* Validate and rearrange clientId info */
  if (ptin_pppoe_clientId_convert(evc_idx, &client) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid client id");
    return L7_FAILURE;
  }

  /* Convert interface to intIfNum format */
#if (PPPOE_CLIENT_INTERF_SUPPORTED)
  intIfNum = 0;
  if (client.mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    if (ptin_intf_ptintf2intIfNum(&client.ptin_intf, &intIfNum) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Cannot convert client intf %u/%u to intIfNum format",client.ptin_intf.intf_type,client.ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
#endif

  /* Check if this key does not exists */
  avl_tree = &pppoeClients_unified.avlTree;

  memset(&avl_key,0x00,sizeof(ptinPppoeClientDataKey_t));

  avl_key.pppoe_instance = pppoe_idx;
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  avl_key.intIfNum = intIfNum;
  #endif
  #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
  avl_key.outerVlan = (client.mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN) ? client.outerVlan : 0;
  #endif
  #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
  avl_key.innerVlan = (client.mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) ? client.innerVlan : 0;
  #endif
  #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
  avl_key.ipv4_addr = (client.mask & PTIN_CLIENT_MASK_FIELD_IPADDR   ) ? client.ipv4_addr : 0;
  #endif
  #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
  if (client.mask & PTIN_CLIENT_MASK_FIELD_MACADDR)
    memcpy(avl_key.macAddr,client.macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  else
    memset(avl_key.macAddr,0x00,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  #endif

  #if (PPPOE_CLIENT_DEBUG)
  PT_LOG_TRACE(LOG_CTX_PPPOE,"Key to search {"
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                              "intIfNum=%u,"
            #endif
            #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                              "svlan=%u,"
            #endif
            #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                              "cvlan=%u,"
            #endif
            #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                              "ipAddr=%u.%u.%u.%u,"
            #endif
            #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
            #endif
                              "} in pppoe_idx=%u",
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
            avl_key.intIfNum,
            #endif
            #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
            avl_key.outerVlan,
            #endif
            #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
            avl_key.innerVlan,
            #endif
            #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
            (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
            #endif
            #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
            avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
            #endif
            pppoe_idx);
  #endif

  /* Check if this entry does not exist in AVL tree */
  if ((avl_infoData=(ptinPppoeClientInfoData_t *) avlSearchLVL7( &(avl_tree->pppoeClientsAvlTree), (void *)&avl_key, AVL_EXACT))==L7_NULLPTR)
  {
    PT_LOG_WARN(LOG_CTX_PPPOE,"This key {"
                #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                                  "intIfNum=%u,"
                #endif
                #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                                  "svlan=%u,"
                #endif
                #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                                  "cvlan=%u,"
                #endif
                #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                                  "ipAddr=%u.%u.%u.%u,"
                #endif
                #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                                  "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
                #endif
                                  "} does not exist in pppoe_idx=%u",
                #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                avl_key.intIfNum,
                #endif
                #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                avl_key.outerVlan,
                #endif
                #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                avl_key.innerVlan,
                #endif
                #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
                #endif
                #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
                #endif
                pppoe_idx);
    return L7_NOT_EXIST;
  }

  /* Extract client index */
  client_idx = avl_infoData->client_index;

  /* Remove entry from AVL tree */
  if (avlDeleteEntry(&(avl_tree->pppoeClientsAvlTree), (void *)&avl_key)==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Error removing key {"
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                              "intIfNum=%u,"
            #endif
            #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                              "svlan=%u,"
            #endif
            #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                              "cvlan=%u,"
            #endif
            #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                              "ipAddr=%u.%u.%u.%u,"
            #endif
            #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
            #endif
                              "} from pppoe_idx=%u",
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
            avl_key.intIfNum,
            #endif
            #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
            avl_key.outerVlan,
            #endif
            #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
            avl_key.innerVlan,
            #endif
            #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
            (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
            #endif
            #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
            avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
            #endif
            pppoe_idx);
    return L7_FAILURE;
  }

  /* Release client index */
  pppoe_clientIndex_release(pppoe_idx, client_idx);

  PT_LOG_TRACE(LOG_CTX_PPPOE,"Success removing Key {"
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                              "intIfNum=%u,"
            #endif
            #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                              "svlan=%u,"
            #endif
            #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                              "cvlan=%u,"
            #endif
            #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                              "ipAddr=%u.%u.%u.%u,"
            #endif
            #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
            #endif
                              "} from pppoe_idx=%u",
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
            avl_key.intIfNum,
            #endif
            #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
            avl_key.outerVlan,
            #endif
            #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
            avl_key.innerVlan,
            #endif
            #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
            (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
            #endif
            #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
            avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
            #endif
            pppoe_idx);

  return L7_SUCCESS;
}

#if 0
/**
 * Get PPPOE Binding table
 * 
 * @param table       : Bin table
 * @param max_entries : Size of table
 * 
 * @return L7_RC_t : L7_FAILURE/L7_SUCCESS
 */
L7_RC_t ptin_pppoe82_bindtable_get(ptin_PPPOE_bind_entry *table, L7_uint16 *max_entries)
{
  pppoeSnoopBinding_t  dsBinding;
  L7_uint16           index, i;
  ptin_intf_t         ptin_intf;
  L7_uint16           evc_idx;
  L7_uint16           n_max;

  n_max = (max_entries!=L7_NULLPTR && *max_entries<PLAT_MAX_FDB_MAC_ENTRIES) ? (*max_entries) : PLAT_MAX_FDB_MAC_ENTRIES;

  memset(&dsBinding,0x00,sizeof(pppoeSnoopBinding_t));
  for (i=0,index=0; i<n_max && usmDbDsBindingGetNext(&dsBinding)==L7_SUCCESS; i++)
  {
    // Calculate port reference and validate it
    if (ptin_intf_intIfNum2ptintf(dsBinding.intIfNum,&ptin_intf)!=L7_SUCCESS)
      continue;
    // Extract vlan and validate it
    if (dsBinding.vlanId<PTIN_VLAN_MIN || dsBinding.vlanId>PTIN_VLAN_MAX)
      continue;
    // Calculate flow id and validate it
    if (ptin_evc_get_evcIdfromIntVlan(dsBinding.vlanId,&evc_idx)!=L7_SUCCESS)
      evc_idx = (L7_uint16)-1;

    // Fill mac-table entry
    table[index].entry_index    = index;
    table[index].evc_idx        = evc_idx;
    table[index].ptin_intf      = ptin_intf;
    table[index].outer_vlan     = dsBinding.vlanId;
    table[index].inner_vlan     = dsBinding.innerVlanId;
    memcpy(table[index].macAddr,dsBinding.macAddr,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
    table[index].ipAddr.s_addr  = dsBinding.ipAddr;
    table[index].remLeave       = dsBinding.remLease;
    table[index].bindingType    = dsBinding.bindingType;
    index++;
  }

  if (max_entries!=L7_NULLPTR)  *max_entries=index;

  return L7_SUCCESS;
}

/**
 * Get PPPOE Binding table
 *
 * @param table       : Bin table
 * @param max_entries : Size of table
 *
 * @notes   IPv6 compatible
 *
 * @return L7_RC_t : L7_FAILURE/L7_SUCCESS
 */
L7_RC_t ptin_pppoev4v6_bindtable_get(ptin_PPPOEv4v6_bind_entry *table, L7_uint16 *max_entries)
{
  pppoeSnoopBinding_t  dsBinding;
  L7_uint16           index, i;
  ptin_intf_t         ptin_intf;
  L7_uint16           evc_idx;
  L7_uint16           n_max;

  n_max = (max_entries!=L7_NULLPTR && *max_entries<PLAT_MAX_FDB_MAC_ENTRIES) ? (*max_entries) : PLAT_MAX_FDB_MAC_ENTRIES;

  memset(&dsBinding,0x00,sizeof(pppoeSnoopBinding_t));
  for (i=0,index=0; i<n_max && usmDbDsBindingGetNext(&dsBinding)==L7_SUCCESS; i++)
  {
    // Calculate port reference and validate it
    if (ptin_intf_intIfNum2ptintf(dsBinding.intIfNum,&ptin_intf)!=L7_SUCCESS)
      continue;
    // Extract vlan and validate it
    if (dsBinding.vlanId<PTIN_VLAN_MIN || dsBinding.vlanId>PTIN_VLAN_MAX)
      continue;
    // Calculate flow id and validate it
    if (ptin_evc_get_evcIdfromIntVlan(dsBinding.vlanId,&evc_idx)!=L7_SUCCESS)
      evc_idx = (L7_uint16)-1;

    // Fill mac-table entry
    table[index].entry_index    = index;
    table[index].evc_idx        = evc_idx;
    table[index].ptin_intf      = ptin_intf;
    table[index].outer_vlan     = dsBinding.vlanId;
    table[index].inner_vlan     = dsBinding.innerVlanId;
    memcpy(table[index].macAddr,dsBinding.macAddr,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
   if ( dsBinding.ipFamily == L7_AF_INET)
   {
      table[index].ipAddr.family = 0;
      table[index].ipAddr.addr.ipv4 = dsBinding.ipAddr;
   }
   else if ( dsBinding.ipFamily == L7_AF_INET6)
   {
      table[index].ipAddr.family = 1;
      memcpy(table[index].ipAddr.addr.ipv6, dsBinding.ipv6Addr, 16*sizeof(L7_uchar8));
   }

    table[index].remLeave       = dsBinding.remLease;
    table[index].bindingType    = dsBinding.bindingType;
    index++;
  }

  if (max_entries!=L7_NULLPTR)  *max_entries=index;

  return L7_SUCCESS;
}

/**
 * Removes an entry from the PPPOE binding table
 * 
 * @param ptr : PPPOE bind table entry
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe82_bindtable_remove(pppoeSnoopBinding_t *dsBinding)
{
  L7_enetMacAddr_t   macAddr;

  // Find This entry
  if (usmDbDsBindingGet(dsBinding)!=L7_SUCCESS) {
    PT_LOG_ERR(LOG_CTX_PPPOE, "This entry does not exist");
    return L7_FAILURE;
  }

  // Remove this entry
  memcpy(macAddr.addr,dsBinding->macAddr,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  if (usmDbDsBindingRemove(&macAddr)!=L7_SUCCESS) {
    PT_LOG_ERR(LOG_CTX_PPPOE, "Error removing entry");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
#endif

/**
 * Get global PPPOE statistics
 * 
 * @param ptin_intf   : interface
 * @param stat_port_g : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_intf_get(ptin_intf_t *ptin_intf, ptin_PPPOE_Statistics_t *stat_port_g)
{
  L7_uint32 ptin_port;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid interface %u/%u",ptin_intf->intf_id,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Return pointer to stat structure */
  if (stat_port_g!=L7_NULLPTR)
  {
    osapiSemaTake(ptin_pppoe_stats_sem,-1);
    memcpy(stat_port_g, &global_stats_intf[ptin_port], sizeof(ptin_PPPOE_Statistics_t));
    osapiSemaGive(ptin_pppoe_stats_sem);
  }

  return L7_SUCCESS;
}

/**
 * Get PPPOE statistics of a particular PPPOE instance and 
 * interface 
 * 
 * @param evc_idx  : Unicast EVC id
 * @param ptin_intf : interface
 * @param stat_port : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_instanceIntf_get(L7_uint32 evc_idx, ptin_intf_t *ptin_intf, ptin_PPPOE_Statistics_t *stat_port)
{
  L7_uint32 ptin_port;
  L7_uint32 pppoe_idx;
  ptin_evc_intfCfg_t intfCfg;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid interface %u/%u",ptin_intf->intf_id,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Check if EVC is active, and if interface is part of the EVC */
  if (ptin_evc_intfCfg_get(evc_idx,ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Error getting interface (%u/%u) configuration from EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,evc_idx);
    return L7_FAILURE;
  }
  if (!intfCfg.in_use)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Interface %u/%u is not in use by EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,evc_idx);
    return L7_FAILURE;
  }

  /* Get Pppoe instance */
  if (ptin_pppoe_instance_find(evc_idx,&pppoe_idx)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"EVC %u does not belong to any PPPOE instance",evc_idx);
    return L7_FAILURE;
  }

  /* Return pointer to stat structure */
  if (stat_port!=L7_NULLPTR)
  {
    osapiSemaTake(ptin_pppoe_stats_sem,-1);
    memcpy(stat_port, &pppoeInstances[pppoe_idx].stats_intf[ptin_port], sizeof(ptin_PPPOE_Statistics_t));
    osapiSemaGive(ptin_pppoe_stats_sem);
  }

  return L7_SUCCESS;
}

/**
 * Get PPPOE statistics of a particular PPPOE instance and 
 * client
 * 
 * @param evc_idx  : Unicast EVC id
 * @param client      : client reference
 * @param stat_port   : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_client_get(L7_uint32 evc_idx, const ptin_client_id_t *client_id, ptin_PPPOE_Statistics_t *stat_client)
{
  L7_uint32 pppoe_idx;
  ptin_client_id_t client;
  ptinPppoeClientInfoData_t *clientInfo;

  /* Validate arguments */
  if (client_id == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get Pppoe instance */
  if (ptin_pppoe_instance_find(evc_idx, &pppoe_idx)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"EVC %u does not belong to any PPPOE instance",evc_idx);
    return L7_FAILURE;
  }

  memcpy(&client, client_id, sizeof(ptin_client_id_t));

  /* Validate and rearrange clientId info */
  if (ptin_pppoe_clientId_convert(evc_idx, &client) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid client id");
    return L7_FAILURE;
  }

  /* Get client */
  if (ptin_pppoe_client_find(pppoe_idx, &client, &clientInfo)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,
            "Error searching for client {mask=0x%02x,"
            "port=%u/%u,"
            "ptin_port=%u,"
            "svlan=%u,"
            "cvlan=%u,"
            "ipAddr=%u.%u.%u.%u,"
            "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} "
            "in pppoe_idx=%u",
            client.mask,
            client.ptin_intf.intf_type, client.ptin_intf.intf_id,
            client.ptin_port,
            client.outerVlan,
            client.innerVlan,
            (client.ipv4_addr>>24) & 0xff, (client.ipv4_addr>>16) & 0xff, (client.ipv4_addr>>8) & 0xff, client.ipv4_addr & 0xff,
            client.macAddr[0],client.macAddr[1],client.macAddr[2],client.macAddr[3],client.macAddr[4],client.macAddr[5],
            pppoe_idx);
    return L7_FAILURE;
  }

  /* Return pointer to stat structure */
  if (stat_client!=L7_NULLPTR)
  {
    osapiSemaTake(ptin_pppoe_stats_sem,-1);
    memcpy(stat_client, &clientInfo->client_stats, sizeof(ptin_PPPOE_Statistics_t));
    osapiSemaGive(ptin_pppoe_stats_sem);
  }

  return L7_SUCCESS;
}

/**
 * Clear all PPPOE statistics
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_clearAll(void)
{
  L7_uint pppoe_idx;
  ptinPppoeClientDataKey_t   avl_key;
  ptinPppoeClientInfoData_t *avl_info;

  osapiSemaTake(ptin_pppoe_stats_sem,-1);

  /* Clear global statistics */
  memset(global_stats_intf,0x00,sizeof(global_stats_intf));

  /* Run all PPPOE instances */
  for (pppoe_idx=0; pppoe_idx<PTIN_SYSTEM_N_PPPOE_INSTANCES; pppoe_idx++)
  {
    if (!pppoeInstances[pppoe_idx].inUse)  continue;

    /* Clear instance statistics */
    memset(pppoeInstances[pppoe_idx].stats_intf, 0x00, sizeof(pppoeInstances[pppoe_idx].stats_intf));
  }

  /* Run all cells in AVL tree related to this instance */
  memset(&avl_key,0x00,sizeof(ptinPppoeClientDataKey_t));
  while ( (avl_info = (ptinPppoeClientInfoData_t *)
                      avlSearchLVL7(&pppoeClients_unified.avlTree.pppoeClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->pppoeClientDataKey, sizeof(ptinPppoeClientDataKey_t));

    /* Clear client statistics */
    memset(&avl_info->client_stats, 0x00, sizeof(ptin_PPPOE_Statistics_t));
  }

  osapiSemaGive(ptin_pppoe_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear all statistics of one PPPOE instance
 * 
 * @param evc_idx : Unicast EVC id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_instance_clear(L7_uint32 evc_idx)
{
  L7_uint pppoe_idx;
  struct ptin_clientInfo_entry_s *clientInfo_entry;

  /* Get Pppoe instance */
  if (ptin_pppoe_instance_find(evc_idx,&pppoe_idx)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"EVC %u does not belong to any PPPOE instance",evc_idx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_pppoe_stats_sem,-1);

  /* Clear instance statistics */
  memset(pppoeInstances[pppoe_idx].stats_intf, 0x00, sizeof(pppoeInstances[pppoe_idx].stats_intf));

  /* Clear statistics of belonging clients */
  clientInfo_entry = NULL;
  dl_queue_get_head(&pppoeInstances[pppoe_idx].queue_clients, (dl_queue_elem_t **)&clientInfo_entry);
  while (clientInfo_entry != NULL)
  {
    /* Clear client statistics */
    if (clientInfo_entry->client_info != L7_NULLPTR)
    {
      memset(&clientInfo_entry->client_info->client_stats, 0x00, sizeof(ptin_PPPOE_Statistics_t)); 
    }

    /* Next queue element */
    clientInfo_entry = (struct ptin_clientInfo_entry_s *) dl_queue_get_next(&pppoeInstances[pppoe_idx].queue_clients, (dl_queue_elem_t *) clientInfo_entry);
  }

  osapiSemaGive(ptin_pppoe_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear interface PPPOE statistics
 * 
 * @param ptin_intf : interface 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_intf_clear(ptin_intf_t *ptin_intf)
{
  L7_uint pppoe_idx;
  L7_uint32 ptin_port;
  L7_uint32 intIfNum;
  ptinPppoeClientDataKey_t   avl_key;
  ptinPppoeClientInfoData_t *avl_info;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid interface %u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Convert interface to intIfNum */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_DHCP,"Invalid interface %u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_pppoe_stats_sem,-1);

  /* Clear global statistics */
  memset(&global_stats_intf[ptin_port], 0x00, sizeof(ptin_PPPOE_Statistics_t));

  /* Run all PPPOE instances */
  for (pppoe_idx=0; pppoe_idx<PTIN_SYSTEM_N_PPPOE_INSTANCES; pppoe_idx++)
  {
    if (!pppoeInstances[pppoe_idx].inUse)  continue;

    /* Clear instance statistics */
    memset(&pppoeInstances[pppoe_idx].stats_intf[ptin_port], 0x00, sizeof(ptin_PPPOE_Statistics_t));
  }

#if (PPPOE_CLIENT_INTERF_SUPPORTED)
  /* Run all cells in AVL tree related to this instance */
  memset(&avl_key,0x00,sizeof(ptinPppoeClientDataKey_t));
  while ( (avl_info = (ptinPppoeClientInfoData_t *)
                      avlSearchLVL7(&pppoeClients_unified.avlTree.pppoeClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->pppoeClientDataKey, sizeof(ptinPppoeClientDataKey_t));

    /* Clear client statistics */
    if (avl_info->pppoeClientDataKey.intIfNum == intIfNum)
    {
      memset(&avl_info->client_stats, 0x00, sizeof(ptin_PPPOE_Statistics_t));
    }
  }
#endif

  osapiSemaGive(ptin_pppoe_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear statistics of a particular PPPOE instance and interface
 * 
 * @param evc_idx  : Unicast EVC id
 * @param ptin_intf : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_instanceIntf_clear(L7_uint32 evc_idx, ptin_intf_t *ptin_intf)
{
  L7_uint pppoe_idx;
  L7_uint32 ptin_port;
  L7_uint32 intIfNum;
  st_PppoeInstCfg_t *pppoeInst;
  ptin_evc_intfCfg_t intfCfg;
  struct ptin_clientInfo_entry_s *clientInfo_entry;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid interface %u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Convert interface to intIfNum */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_DHCP,"Invalid interface %u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Check if EVC is active, and if interface is part of the EVC */
  if (ptin_evc_intfCfg_get(evc_idx,ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Error getting interface (%u/%u) configuration from EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,evc_idx);
    return L7_FAILURE;
  }
  if (!intfCfg.in_use)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Interface %u/%u is not in use by EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,evc_idx);
    return L7_FAILURE;
  }

  /* Get Pppoe instance */
  if (ptin_pppoe_instance_find(evc_idx,&pppoe_idx)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"EVC %u does not belong to any PPPOE instance",evc_idx);
    return L7_FAILURE;
  }

  /* Pointer to pppoe instance */
  pppoeInst = &pppoeInstances[pppoe_idx];

  osapiSemaTake(ptin_pppoe_stats_sem,-1);

  /* Clear instance statistics */
  memset(&pppoeInst->stats_intf[ptin_port], 0x00, sizeof(ptin_PPPOE_Statistics_t));

#if (PPPOE_CLIENT_INTERF_SUPPORTED)
  /* Clear statistics of belonging clients */
  clientInfo_entry = NULL;
  dl_queue_get_head(&pppoeInstances[pppoe_idx].queue_clients, (dl_queue_elem_t **)&clientInfo_entry);
  while (clientInfo_entry != NULL)
  {
    if (clientInfo_entry->client_info != L7_NULLPTR &&
        clientInfo_entry->client_info->pppoeClientDataKey.intIfNum == intIfNum) 
    {
      memset(&clientInfo_entry->client_info->client_stats, 0x00, sizeof(ptin_PPPOE_Statistics_t));
    }

    /* Next queue element */
    clientInfo_entry = (struct ptin_clientInfo_entry_s *) dl_queue_get_next(&pppoeInstances[pppoe_idx].queue_clients, (dl_queue_elem_t *) clientInfo_entry);
  }
#endif

  osapiSemaGive(ptin_pppoe_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear PPPOE statistics of a particular PPPOE instance and 
 * client
 * 
 * @param evc_idx  : Unicast EVC id
 * @param client      : client reference
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_client_clear(L7_uint32 evc_idx, const ptin_client_id_t *client_id)
{
  L7_uint pppoe_idx;
  ptin_client_id_t client;
  ptinPppoeClientInfoData_t *clientInfo;

  /* Validate arguments */
  if (client_id == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get Pppoe instance */
  if (ptin_pppoe_instance_find(evc_idx, &pppoe_idx)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"EVC %u does not belong to any PPPOE instance",evc_idx);
    return L7_FAILURE;
  }

  memcpy(&client, client_id, sizeof(ptin_client_id_t));

  /* Validate and rearrange clientId info */
  if (ptin_pppoe_clientId_convert(evc_idx, &client) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid client id");
    return L7_FAILURE;
  }

  /* Find client */
  if (ptin_pppoe_client_find(pppoe_idx, &client, &clientInfo)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,
            "Error searching for client {mask=0x%02x,"
            "port=%u/%u,"
            "ptin_port=%u, "
            "svlan=%u,"
            "cvlan=%u,"
            "ipAddr=%u.%u.%u.%u,"
            "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} "
            "in pppoe_idx=%u",
            client.mask,
            client.ptin_intf.intf_type, client.ptin_intf.intf_id,
            client.ptin_port,
            client.outerVlan,
            client.innerVlan,
            (client.ipv4_addr>>24) & 0xff, (client.ipv4_addr>>16) & 0xff, (client.ipv4_addr>>8) & 0xff, client.ipv4_addr & 0xff,
            client.macAddr[0],client.macAddr[1],client.macAddr[2],client.macAddr[3],client.macAddr[4],client.macAddr[5],
            pppoe_idx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_pppoe_stats_sem,-1);

  /* Clear client statistics (if port matches) */
  memset(&clientInfo->client_stats, 0x00, sizeof(ptin_PPPOE_Statistics_t));

  osapiSemaGive(ptin_pppoe_stats_sem);

  return L7_SUCCESS;
}

/*********************************************************** 
 * Internal functions (for Fastpath usage)
 ***********************************************************/

/**
 * Get the root vlan associated to the internal vlan
 * 
 * @param intVlan  : internal vlan
 * @param rootVlan : root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_rootVlan_get(L7_uint16 intVlan, L7_uint16 *rootVlan)
{
  if (ptin_evc_intRootVlan_get_fromIntVlan(intVlan, rootVlan)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"Error getting rootVlan for intVlan=%u", intVlan);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Validate ingress interface for a PPPOE packet 
 * 
 * @param ptin_port : interface
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_pppoe_intf_validate(L7_uint32 ptin_port)
{
  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_PORTS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments: ptin_port=%u", ptin_port);
    return L7_FALSE;
  }

  return L7_TRUE;
}

/**
 * Validate internal vlan in a PPPOE Packet 
 * 
 * @param intVlanId   : internal vlan
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_pppoe_vlan_validate(L7_uint16 intVlanId)
{
  /* Validate arguments */
  if ( intVlanId<1 || intVlanId>=4095 )
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments: intVlan=%u",intVlanId);
    return L7_FALSE;
  }

  /* PPPOE instance, from internal vlan */
  if (ptin_pppoe_inst_get_fromIntVlan(intVlanId,L7_NULLPTR,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"No PPPOE instance associated to intVlan %u",intVlanId);
    return L7_FALSE;
  }

  return L7_TRUE;
}

/**
 * Set all interfaces as untrusted
 */
void ptin_pppoe_intfTrusted_init(void)
{
  memset(&pppoe_intf_trusted, 0x00, sizeof(pppoe_intf_trusted));

  PT_LOG_INFO(LOG_CTX_PPPOE,"Trusted ports initialized");
}

/**
 * Set a particular interface as trusted or not
 * 
 * @param ptin_port : interface
 * @param trusted  : trusted
 */
void ptin_pppoe_intfTrusted_set(L7_uint32 ptin_port, L7_BOOL trusted)
{
  PTINPORT_BITMAP_SET(pppoe_intf_trusted, ptin_port);
}


/**
 * Check if a particular interface of one EVC is trusted
 * 
 * @param ptin_port    : interface
 * @param intVlanId   : internal vlan
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_pppoe_is_intfRoot(L7_uint32 ptin_port, L7_uint16 intVlanId)
{
  return ptin_evc_intf_isRoot(intVlanId, ptin_port);
}

/**
 * Get the list of trusted interfaces associated to a internal 
 * vlan 
 * 
 * @param intVlan  : Internal vlan 
 * @param intfList : List of interfaces
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_BOOL ptin_pppoe_intfTrusted_getList(L7_uint16 intVlanId, ptin_port_bmp_t *intfList)
{
  L7_uint32             i, ptin_port;
  L7_uint               pppoe_idx;
  st_PppoeInstCfg_t     *pppoeInst;
  L7_uint32             evc_id_ext;
  ptin_HwEthMef10Evc_t  evcConf;

  /* PPPOE instance, from internal vlan */
  if (ptin_pppoe_inst_get_fromIntVlan(intVlanId, &pppoeInst, &pppoe_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"No PPPOE instance associated to intVlan %u", intVlanId);
    return L7_FALSE;
  }

  /* PPPOE instance, from internal vlan */
  if (ptin_evc_get_evcIdfromIntVlan(intVlanId, &evc_id_ext) != L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"No EVC id associated to intVlan %u", intVlanId);
    return L7_FALSE;
  }

  /* Check if EVCs are in use */
  if (!ptin_evc_is_in_use(evc_id_ext))
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"Inconsistency: eEVCid=%u (Vlan %u) not in use", evc_id_ext, intVlanId);
    return L7_FAILURE;
  }

  /* Get interface configuration */
  memset(&evcConf, 0x00, sizeof(evcConf));
  evcConf.index = evc_id_ext;
  if (ptin_evc_get(&evcConf) != L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"Error acquiring eEVC %u configuration", evc_id_ext);
    return L7_FALSE;
  }

  /* Validate output list */
  if (intfList == L7_NULLPTR)
  {
    return L7_SUCCESS;
  }

  /* Clear output mask ports */
  memset(intfList, 0x00, sizeof(ptin_port_bmp_t));

  /* Check all EVC ports for trusted ones */
  for (i = 0; i < evcConf.n_intf; i++)
  {
    ptin_port = evcConf.intf[i].intf.value.ptin_port;

    /* Mark interface as trusted, if it is */
    if (PTINPORT_BITMAP_IS_SET(pppoe_intf_trusted, ptin_port))
    {
      PTINPORT_BITMAP_SET(*intfList, ptin_port);
    }
  }

  return L7_SUCCESS;
}

/**
 * Get external vlans
 * 
 * @param ptin_port 
 * @param intOVlan 
 * @param intIVlan 
 * @param client_idx 
 * @param uni_ovid : External Outer Vlan
 * @param uni_ivid : External Inner Vlan
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_pppoe_extVlans_get(L7_uint32 ptin_port, L7_uint16 intOVlan, L7_uint16 intIVlan,
                                L7_int client_idx, L7_uint16 *uni_ovid, L7_uint16 *uni_ivid)
{
  L7_uint pppoe_idx;
  L7_uint32 flags;
  L7_uint16 ovid, ivid;
  ptinPppoeClientInfoData_t *clientInfo;
  L7_uint8  intf_type;

  /* Get interface type */
  if (ptin_evc_intf_type_get(intOVlan, ptin_port, &intf_type) != L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_TRACE(LOG_CTX_PPPOE, "Error getting intf configuration for intVlan %u, ptin_port %u",
                   intOVlan, ptin_port);
    return L7_FAILURE;
  }
  /* Validate interface type */
  if (intf_type != PTIN_EVC_INTF_ROOT && intf_type != PTIN_EVC_INTF_LEAF)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_TRACE(LOG_CTX_PPPOE, "intVlan %u / ptin_port %u is not used", intOVlan, ptin_port);
    return L7_FAILURE;
  }

  ovid = ivid = 0;
  /* If client is provided, go directly to client info */
  if ((intf_type == PTIN_EVC_INTF_LEAF) &&                            /* Is a leaf port */
      (client_idx>=0 && client_idx<PTIN_SYSTEM_PPPOE_MAXCLIENTS) &&   /* Valid index */
      (clientInfo_pool[client_idx].client_info != L7_NULLPTR) &&      /* Client exists */
      ptin_pppoe_inst_get_fromIntVlan(intOVlan, L7_NULLPTR, &pppoe_idx)==L7_SUCCESS)  /* intOVlan is valid */
  {
    /* Get pointer to client structure in AVL tree */
    clientInfo = clientInfo_pool[client_idx].client_info;

    ovid = clientInfo->uni_ovid;
    ivid = clientInfo->uni_ivid;
  }

  if (ptin_debug_pppoe_snooping)
    PT_LOG_TRACE(LOG_CTX_PPPOE,"ovid=%u ivid=%u", ovid, ivid);

  /* If no data was retrieved, goto EVC info */
  if (ovid == 0)
  {
    if (ptin_evc_extVlans_get_fromIntVlan(ptin_port, intOVlan, intIVlan, &ovid, &ivid) != L7_SUCCESS)
    {
      ovid = intOVlan;
      ivid = intIVlan;
    }
  }

  /* For packets sent to root ports, belonging to unstacked EVCs, remove inner vlan */
  if (ptin_evc_flags_get_fromIntVlan(intOVlan, &flags, L7_NULLPTR) == L7_SUCCESS)
  {
    if ( ( (flags & PTIN_EVC_MASK_QUATTRO) && !(flags & PTIN_EVC_MASK_STACKED) && (intf_type == PTIN_EVC_INTF_ROOT) ) ||
         (!(flags & PTIN_EVC_MASK_QUATTRO) && !(flags & PTIN_EVC_MASK_STACKED)) )
    {
      ivid = 0;
    }
  }

  if (ptin_debug_pppoe_snooping)
    PT_LOG_TRACE(LOG_CTX_PPPOE,"ovid=%u ivid=%u", ovid, ivid);

  /* Return vlans */
  if (uni_ovid != L7_SUCCESS)  *uni_ovid = ovid;
  if (uni_ivid != L7_SUCCESS)  *uni_ivid = ivid;

  return L7_SUCCESS;
}

/**
 * Get the client index associated to a PPPOE client 
 * 
 * @param intIfNum      : interface number
 * @param intVlan       : internal vlan
 * @param client        : Client information parameters
 * @param client_index  : Client index to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_clientIndex_get(L7_uint32 intIfNum, L7_uint16 intVlan,
                                  ptin_client_id_t *client,
                                  L7_uint *client_index)
{
  L7_uint     pppoe_idx;
  //ptin_intf_t ptin_intf;
  L7_uint     client_idx;
  ptinPppoeClientInfoData_t *clientInfo;

  /* Validate arguments */
  if ( client==L7_NULLPTR || PPPOE_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    if (ptin_debug_pppoe_snooping)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments");
    }
    return L7_FAILURE;
  }

  /* PPPOE instance, from internal vlan */
  if (ptin_pppoe_inst_get_fromIntVlan(intVlan,L7_NULLPTR,&pppoe_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"No PPPOE instance associated to intVlan %u",intVlan);
    }
    return L7_FAILURE;
  }

  /* If the inner vlan is not valid, return -1 as client index */
#if PPPOE_CLIENT_INNERVLAN_SUPPORTED
  if ((client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) &&
      (client->innerVlan==0 || client->innerVlan>4095))
  {
    if (client_index!=L7_NULLPTR)
    {
      *client_index = (L7_uint)-1;
    }
    return L7_SUCCESS;
  }
#endif

  /* Get intIfNum format for the interface number */
#if (PPPOE_CLIENT_INTERF_SUPPORTED)
  if (intIfNum != (L7_uint32)-1 /*All*/)
  {
    client->intIfNum = intIfNum;
    client->mask |= PTIN_CLIENT_MASK_FIELD_INTIFNUM;
 #if 1
    client->mask &= ~PTIN_CLIENT_MASK_FIELD_INTF;
 #else
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    if (ptin_intf_intIfNum2ptintf(intIfNum, &ptin_intf) == L7_SUCCESS)
    {
      client->ptin_intf.intf_type = ptin_intf.intf_type;
      client->ptin_intf.intf_id   = ptin_intf.intf_id;
    }
    else
    {
      if (ptin_debug_pppoe_snooping)
      {
        PT_LOG_ERR(LOG_CTX_PPPOE,"Cannot convert client intIfNum %u to ptin_intf format", intIfNum);
      }
      return L7_FAILURE;
    }
 #endif
  }
#endif

  /* Get client */
  if (ptin_pppoe_client_find(pppoe_idx, client, &clientInfo)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,
              "Error searching for client {mask=0x%02x,"
              "intIfNum=%u,"
              "port=%u/%u,"
              "ptin_port=%u,"
              "svlan=%u,"
              "cvlan=%u,"
              "ipAddr=%u.%u.%u.%u,"
              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} "
              "in pppoe_idx=%u",
              client->mask,
              client->intIfNum,
              client->ptin_intf.intf_type, client->ptin_intf.intf_id,
              client->ptin_port,
              client->outerVlan,
              client->innerVlan,
              (client->ipv4_addr>>24) & 0xff, (client->ipv4_addr>>16) & 0xff, (client->ipv4_addr>>8) & 0xff, client->ipv4_addr & 0xff,
              client->macAddr[0],client->macAddr[1],client->macAddr[2],client->macAddr[3],client->macAddr[4],client->macAddr[5],
              pppoe_idx);
    }
    return L7_FAILURE;
  }

  /* Update client index in data cell */
  client_idx = clientInfo->client_index;

  #if (PPPOE_CLIENT_DEBUG)
  PT_LOG_TRACE(LOG_CTX_PPPOE,"Client_idx=%u for key {"
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                              "intIfNum=%u,"
                              "ptin_port=%u,"
            #endif
            #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                              "svlan=%u,"
            #endif
            #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                              "cvlan=%u,"
            #endif
            #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                              "ipAddr=%u.%u.%u.%u,"
            #endif
            #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
            #endif
                              "}"
            ,client_idx
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
            ,clientInfo->pppoeClientDataKey.intIfNum
            ,clientInfo->ptin_port
            #endif
            #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
            ,clientInfo->pppoeClientDataKey.outerVlan
            #endif
            #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
            ,clientInfo->pppoeClientDataKey.innerVlan
            #endif
            #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
            ,(clientInfo->pppoeClientDataKey.ipv4_addr>>24) & 0xff, (clientInfo->pppoeClientDataKey.ipv4_addr>>16) & 0xff, (clientInfo->pppoeClientDataKey.ipv4_addr>>8) & 0xff, clientInfo->pppoeClientDataKey.ipv4_addr & 0xff
            #endif
            #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
            ,clientInfo->pppoeClientDataKey.macAddr[0],clientInfo->pppoeClientDataKey.macAddr[1],clientInfo->pppoeClientDataKey.macAddr[2],clientInfo->pppoeClientDataKey.macAddr[3],clientInfo->pppoeClientDataKey.macAddr[4],clientInfo->pppoeClientDataKey.macAddr[5]
            #endif
            );
  #endif

  /* Return client index */
  if (client_index!=L7_NULLPTR)  *client_index = client_idx;

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
L7_RC_t ptin_pppoe_clientData_get(L7_uint16 intVlan,
                                 L7_uint client_idx,
                                 ptin_client_id_t *client)
{
  ptinPppoeClientInfoData_t *clientInfo;

  /* Validate arguments */
  if ( client==L7_NULLPTR || client_idx>=PTIN_SYSTEM_PPPOE_MAXCLIENTS )
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get pointer to client structure in AVL tree */
  clientInfo = clientInfo_pool[client_idx].client_info;
  /* If does not exist... */
  if (clientInfo==L7_NULLPTR)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"Provided client_idx (%u) does not exist",client_idx);
    return L7_FAILURE;
  }

  memset(client,0x00,sizeof(ptin_client_id_t));
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  if (ptin_intf_port2typeId(clientInfo->ptin_port,
                            &client->ptin_intf.intf_type, &client->ptin_intf.intf_id) != L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Cannot convert client port %u to ptin_intf format",
                 clientInfo->ptin_port);
    }
    return L7_FAILURE;
  }

  client->ptin_port = clientInfo->ptin_port;
  client->mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  #endif
  #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
  client->outerVlan = clientInfo->pppoeClientDataKey.outerVlan;
  client->mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  #endif
  #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
  if (clientInfo->pppoeClientDataKey.innerVlan > 0 && clientInfo->pppoeClientDataKey.innerVlan < 4096)
  {
    client->innerVlan = clientInfo->pppoeClientDataKey.innerVlan;
    client->mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  }
  #endif
  #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
  client->ipv4_addr = clientInfo->pppoeClientDataKey.ipv4_addr;
  client->mask |= PTIN_CLIENT_MASK_FIELD_IPADDR;
  #endif
  #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
  memcpy(client->macAddr,clientInfo->pppoeClientDataKey.macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  client->mask |= PTIN_CLIENT_MASK_FIELD_MACADDR;
  #endif

  return L7_SUCCESS;
}

/**
 * Get PPPOE client data (circuit and remote ids)
 * 
 * @param ptin_port   : FP interface
 * @param intVlan     : internal vlan
 * @param innerVlan   : inner/client vlan 
 * @param circuitId   : circuit id (output) 
 * @param remoteId    : remote id (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stringIds_get(L7_uint32 ptin_port, L7_uint16 intVlan, L7_uint16 innerVlan, L7_uchar8 *macAddr,
                                 L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_uint pppoe_idx;
  ptin_intf_t ptin_intf;
  ptin_client_id_t client;
  ptinPppoeClientInfoData_t *client_info;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX /*||
      innerVlan==0 || innerVlan>=4096*/)
  {
    if (ptin_debug_pppoe_snooping)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments");
    }
    return L7_FAILURE;
  }

  /* Convert interface to ptin format */
  if (ptin_intf_port2ptintf(ptin_port, &ptin_intf) != L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid ptin_port (%u)", ptin_port);
    return L7_FAILURE;
  }

  /* Get pppoe instance */
  if (ptin_pppoe_inst_get_fromIntVlan(intVlan,L7_NULLPTR,&pppoe_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"Internal vlan %u does not correspond to any PPPOE instance",intVlan);
    return L7_FAILURE;
  }

#if (PTIN_BOARD_IS_GPON)
  if (innerVlan > 0 && innerVlan < 4096) 
#else
  if (1)
#endif
  {
    /* Build client structure */
    memset(&client,0x00,sizeof(ptin_client_id_t));

#if PPPOE_CLIENT_INTERF_SUPPORTED
    if(ptin_intf_ptintf2intIfNum(&ptin_intf, &client.intIfNum) != L7_SUCCESS)
    {
      if (ptin_debug_pppoe_snooping)
      {
        PT_LOG_ERR(LOG_CTX_PPPOE, "Cannot convert from ptinf to intIfNum, ptin_intf %u,%u", 
                   ptin_intf.intf_type, ptin_intf.intf_id);
      }
      return L7_FAILURE;
    }

    client.mask |= PTIN_CLIENT_MASK_FIELD_INTIFNUM;
    client.ptin_intf.intf_type = ptin_intf.intf_type;
    client.ptin_intf.intf_id   = ptin_intf.intf_id;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
#endif

#if PPPOE_CLIENT_INNERVLAN_SUPPORTED
    if (innerVlan > 0 && innerVlan < 4096)
    {
      client.innerVlan = innerVlan;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
    }
#endif

    if (ptin_debug_pppoe_snooping)
    {
      PT_LOG_TRACE(LOG_CTX_PPPOE,"Interface %u/%u (intIfNum=%u)",
                   client.ptin_intf.intf_type, client.ptin_intf.intf_id, client.intIfNum);
    }

    /* Find client information */
    if (ptin_pppoe_client_find(pppoe_idx,&client,&client_info)!=L7_SUCCESS)
    {
      if (ptin_debug_pppoe_snooping)
        PT_LOG_ERR(LOG_CTX_PPPOE,"Non existent client in PPPOE instance %u", pppoe_idx);
      return L7_FAILURE;
    }

    /* Return string ids */
    if (circuitId!=L7_NULLPTR)
    {
      strncpy(circuitId, client_info->client_data.circuitId_str, FD_DS_MAX_REMOTE_ID_STRING);
      circuitId[FD_DS_MAX_REMOTE_ID_STRING-1] = '\0';
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_PPPOE, "circuitId is NULL");
    }
    if (remoteId!=L7_NULLPTR)
    {
      strncpy(remoteId ,client_info->client_data.remoteId_str ,FD_DS_MAX_REMOTE_ID_STRING);
      remoteId[FD_DS_MAX_REMOTE_ID_STRING-1] = '\0';
    }
  }
  else
  {
    #if PPPOE_ACCEPT_UNSTACKED_PACKETS
    if (ptin_pppoe_strings_def_get(&ptin_intf,macAddr,circuitId,remoteId)!=L7_SUCCESS)
    {
      if (ptin_debug_pppoe_snooping)
        PT_LOG_ERR(LOG_CTX_PPPOE,"Error getting default strings");
      return L7_FAILURE;
    }
    #else
    PT_LOG_ERR(LOG_CTX_PPPOE,"No client defined!");
    return L7_FAILURE;
    #endif
  }

  return L7_SUCCESS;
}

/**
 * Get PPPOE EVC ethernet priority
 * 
 * @param intVlan     : internal vlan
 * @param ethPrty     : priority (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_ethPrty_get(L7_uint16 intVlan, L7_uint8 *ethPrty)
{
  L7_uint pppoe_idx;

  /* Validate arguments */
  if (intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX  ||
      L7_NULLPTR == ethPrty)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get pppoe instance */
  if (ptin_pppoe_inst_get_fromIntVlan(intVlan,L7_NULLPTR,&pppoe_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"Internal vlan %u does not correspond to any PPPOE instance",intVlan);
    return L7_FAILURE;
  }

  ptin_pppoe_evc_ethprty_get(&pppoeInstances[pppoe_idx].circuitid, ethPrty);

  return L7_SUCCESS;
}

/**
 * Get PPPOE client data (PPPOE Options)
 *
 * @param ptin_port   : FP interface
 * @param intVlan     : internal vlan
 * @param innerVlan   : inner/client vlan
 * @param isActiveOp82: L7_TRUE if op82 is active for this client
 * @param isActiveOp37: L7_TRUE if op37 is active for this client
 * @param isActiveOp18: L7_TRUE if op18 is active for this client
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_client_options_get(L7_uint32 ptin_port, L7_uint16 intVlan, L7_uint16 innerVlan, L7_BOOL *isActiveOp82,
                                      L7_BOOL *isActiveOp37, L7_BOOL *isActiveOp18)
{
   L7_uint pppoe_idx;
   ptin_intf_t ptin_intf;
   ptin_client_id_t client;
   ptinPppoeClientInfoData_t *client_info;

   //Set PPPOE options to L7_FALSE
   if(L7_NULLPTR != isActiveOp82)
   {
     *isActiveOp82 = L7_FALSE;
   }
  if (L7_NULLPTR != isActiveOp37)
  {
    *isActiveOp37 = L7_FALSE;
  }
  if (L7_NULLPTR != isActiveOp18)
  {
    *isActiveOp18 = L7_FALSE;
  }

   /* Validate arguments */
   if (ptin_port >= PTIN_SYSTEM_N_INTERF || intVlan < PTIN_VLAN_MIN || intVlan > PTIN_VLAN_MAX
#if (PTIN_BOARD_IS_GPON)
       || innerVlan==0 || innerVlan>=4096
#endif
      )
   {
      if (ptin_debug_pppoe_snooping)
         PT_LOG_ERR(LOG_CTX_PPPOE, "Invalid arguments");
      return L7_FAILURE;
   }

   /* Convert interface to ptin format */
   if (ptin_intf_port2typeId(ptin_port, &ptin_intf.intf_type, &ptin_intf.intf_id) != L7_SUCCESS)
   {
      if (ptin_debug_pppoe_snooping)
         PT_LOG_ERR(LOG_CTX_PPPOE, "Invalid ptin_port (%u)", ptin_port);
      return L7_FAILURE;
   }

   /* Get pppoe instance */
   if (ptin_pppoe_inst_get_fromIntVlan(intVlan, L7_NULLPTR, &pppoe_idx) != L7_SUCCESS)
   {
      if (ptin_debug_pppoe_snooping)
         PT_LOG_ERR(LOG_CTX_PPPOE, "Internal vlan %u does not correspond to any PPPOE instance", intVlan);
      return L7_FAILURE;
   }

#if (PTIN_BOARD_IS_GPON)
   if (innerVlan > 0 && innerVlan < 4096)
#else
   if (1)
#endif
   {
      /* Build client structure */
      memset(&client, 0x00, sizeof(ptin_client_id_t));

#if PPPOE_CLIENT_INTERF_SUPPORTED
      /* Convert to intIfNum format */
      if (ptin_intf_ptintf2intIfNum(&ptin_intf, &client.intIfNum) != L7_SUCCESS)
      {
        if (ptin_debug_pppoe_snooping)
           PT_LOG_ERR(LOG_CTX_PPPOE, "Cannot convert client_ref intf %u/%u to intIfnUm format",ptin_intf.intf_type, ptin_intf.intf_id);
        return L7_FAILURE;
      }
      else
      {
          client.mask |= PTIN_CLIENT_MASK_FIELD_INTIFNUM;
      }
      client.ptin_intf.intf_type = ptin_intf.intf_type;
      client.ptin_intf.intf_id = ptin_intf.intf_id;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
#endif
#if PPPOE_CLIENT_INNERVLAN_SUPPORTED
      if (innerVlan > 0 && innerVlan < 4096)
      {
        client.innerVlan = innerVlan;
        client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
      }
#endif

      /* Find client information */
      if (ptin_pppoe_client_find(pppoe_idx, &client, &client_info) != L7_SUCCESS)
      {
         if (ptin_debug_pppoe_snooping)
            PT_LOG_ERR(LOG_CTX_PPPOE, "Non existent client in PPPOE instance %u", pppoe_idx);
         return L7_FAILURE;
      }

      if (L7_NULLPTR != isActiveOp82) {
        *isActiveOp82 = 0x01 & client_info->client_data.pppoe_options;
      }
      if (L7_NULLPTR != isActiveOp37) {
        *isActiveOp37 = (0x02 & client_info->client_data.pppoe_options) >> 1;
      }
      if (L7_NULLPTR != isActiveOp18) {
        *isActiveOp18 = (0x04 & client_info->client_data.pppoe_options) >> 2;
      }
   }
   else
   {
      PT_LOG_ERR(LOG_CTX_PPPOE, "No client defined!");
      return L7_FAILURE;
   }

   return L7_SUCCESS;
}

/**
 * Update PPPOE snooping configuration, when interfaces are 
 * added/removed 
 * 
 * @param evcId     : EVC id 
 * @param ptin_intf : interface 
 * @param enable    : L7_TRUE when interface is added 
 *                    L7_FALSE when interface is removed
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_snooping_trap_interface_update(L7_uint32 evcId, ptin_intf_t *ptin_intf, L7_BOOL enable)
{
  return L7_SUCCESS;
}

/**
 * Increment PPPOE statistics
 * 
 * @param ptin_port  : interface where the packet entered
 * @param vlan       : packet's interval vlan
 * @param client_idx : client index
 * @param field      : field to increment
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_increment_field(L7_uint32 ptin_port, L7_uint16 vlan, L7_uint32 client_idx, ptin_pppoe_stat_enum_t field)
{
  st_PppoeInstCfg_t *pppoeInst;
  ptinPppoeClientInfoData_t *client;
  ptin_PPPOE_Statistics_t *stat_port_g = L7_NULLPTR;
  ptin_PPPOE_Statistics_t *stat_port   = L7_NULLPTR;
  ptin_PPPOE_Statistics_t *stat_client = L7_NULLPTR;

  /* Validate field */
  if (field>=PPPOE_STAT_FIELD_ALL)
  {
    return L7_FAILURE;
  }

  /* Get PPPOE instance */
  pppoeInst = L7_NULLPTR;
  if (vlan>=PTIN_VLAN_MIN && vlan<=PTIN_VLAN_MAX)
  {
    if (ptin_pppoe_inst_get_fromIntVlan(vlan,&pppoeInst,L7_NULLPTR)!=L7_SUCCESS)
    {
      pppoeInst = L7_NULLPTR;
    }
  }

  /* If interface is valid... */
  if (ptin_port < PTIN_SYSTEM_N_INTERF)
  {
    /* Global interface statistics at interface level */
    stat_port_g = &global_stats_intf[ptin_port];

    if (pppoeInst!=L7_NULLPTR)
    {
      /* interface statistics at pppoe instance and interface level */
      stat_port = &pppoeInst->stats_intf[ptin_port];
    }
  }

  /* If client index is valid... */
  if (pppoeInst!=L7_NULLPTR && client_idx<PTIN_SYSTEM_PPPOE_MAXCLIENTS)
  {
    client = clientInfo_pool[client_idx].client_info;
    if (client!=L7_NULLPTR)
    {
      /* Statistics at client level */
      stat_client = &client->client_stats;
    }
  }

  osapiSemaTake(ptin_pppoe_stats_sem,-1);

  switch (field) {
  case PPPOE_STAT_FIELD_RX_INTERCEPTED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_rx_intercepted++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_rx_intercepted++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_rx_intercepted++;
    break;

  case PPPOE_STAT_FIELD_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_rx++;
    break;

  case PPPOE_STAT_FIELD_RX_FILTERED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_rx_filtered++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_rx_filtered++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_rx_filtered++;
    break;

  case PPPOE_STAT_FIELD_TX_FORWARDED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_tx_forwarded++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_tx_forwarded++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_tx_forwarded++;
    break;

  case PPPOE_STAT_FIELD_TX_FAILED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_tx_failed++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_tx_failed++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_tx_failed++;
    break;

  case PPPOE_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_tx_client_requests_with_option82++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_tx_client_requests_with_option82++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_tx_client_requests_with_option82++;
    break;

  case PPPOE_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION37:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_tx_client_requests_with_option37++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_tx_client_requests_with_option37++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_tx_client_requests_with_option37++;
    break;

  case PPPOE_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION18:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_tx_client_requests_with_option18++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_tx_client_requests_with_option18++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_tx_client_requests_with_option18++;
    break;

  case PPPOE_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_rx_server_replies_with_option82++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_rx_server_replies_with_option82++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_rx_server_replies_with_option82++;
    break;

  case PPPOE_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION37:
      if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_rx_server_replies_with_option37++;
      if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_rx_server_replies_with_option37++;
      if (stat_client!=L7_NULLPTR)  stat_client->pppoe_rx_server_replies_with_option37++;
      break;

  case PPPOE_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION18:
      if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_rx_server_replies_with_option18++;
      if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_rx_server_replies_with_option18++;
      if (stat_client!=L7_NULLPTR)  stat_client->pppoe_rx_server_replies_with_option18++;
      break;

  case PPPOE_STAT_FIELD_RX_CLIENT_REQUESTS_WITHOUT_OPTIONS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_rx_client_requests_without_options++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_rx_client_requests_without_options++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_rx_client_requests_without_options++;
    break;

  case PPPOE_STAT_FIELD_TX_SERVER_REPLIES_WITHOUT_OPTIONS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_tx_server_replies_without_options++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_tx_server_replies_without_options++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_tx_server_replies_without_options++;
    break;

  case PPPOE_STAT_FIELD_RX_CLIENT_PKTS_ON_TRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_rx_client_pkts_onTrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_rx_client_pkts_onTrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_rx_client_pkts_onTrustedIntf++;
    break;

  case PPPOE_STAT_FIELD_RX_CLIENT_PKTS_WITHOPS_ON_UNTRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_rx_client_pkts_withOps_onUntrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_rx_client_pkts_withOps_onUntrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_rx_client_pkts_withOps_onUntrustedIntf++;
    break;

  case PPPOE_STAT_FIELD_RX_SERVER_PKTS_ON_UNTRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_rx_server_pkts_onUntrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_rx_server_pkts_onUntrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_rx_server_pkts_onUntrustedIntf++;
    break;

  case PPPOE_STAT_FIELD_RX_SERVER_PKTS_WITHOUTOPS_ON_TRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->pppoe_rx_server_pkts_withoutOps_onTrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->pppoe_rx_server_pkts_withoutOps_onTrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client->pppoe_rx_server_pkts_withoutOps_onTrustedIntf++;
    break;

  default:
    break;
  }

  osapiSemaGive(ptin_pppoe_stats_sem);

  return L7_SUCCESS;
}

/*********************************************************** 
 * Static functions
 ***********************************************************/

/**
 * Gets the PPPoE instance from the NNI ovlan
 * 
 * @param nni_ovlan  : NNI outer vlan 
 * @param pppoe_idx  : PPPoE instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_pppoe_instance_find_agg(L7_uint16 nni_ovlan, L7_uint *pppoe_idx)
{
  L7_uint idx;

  /* Search for the provided Mcast and Ucast evcs */
  for (idx=0; idx<PTIN_SYSTEM_N_PPPOE_INSTANCES; idx++)
  {
    if (!pppoeInstances[idx].inUse)  continue;

    if (pppoeInstances[idx].nni_ovid == nni_ovlan)
      break;
  }

  /* If not found empty instances, return error */
  if (idx>=PTIN_SYSTEM_N_PPPOE_INSTANCES)
    return L7_FAILURE;

  /* Return instance index */
  if (pppoe_idx!=L7_NULLPTR)  *pppoe_idx = idx;

  return L7_SUCCESS;
}

/**
 * Find client information in a particulat PPPOE instance
 * 
 * @param pppoe_idx    : PPPOE instance index
 * @param client_ref  : client reference
 * @param client_info : client information pointer (output)
 * 
 * @return L7_RC_t : L7_SUCCESS - Client found 
 *                   L7_NOT_EXIST - Client does not exist
 *                   L7_FAILURE - Error
 */
static L7_RC_t ptin_pppoe_client_find(L7_uint pppoe_idx, ptin_client_id_t *client_ref, ptinPppoeClientInfoData_t **client_info)
{
  ptinPppoeClientDataKey_t avl_key;
  ptinPppoeClientsAvlTree_t *avl_tree;
  ptinPppoeClientInfoData_t *clientInfo;

  /* Validate arguments */
  if (pppoe_idx>=PTIN_SYSTEM_N_PPPOE_INSTANCES || client_ref==L7_NULLPTR)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate pppoe instance */
  if (!pppoeInstances[pppoe_idx].inUse)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"PPPOE instance %u is not in use",pppoe_idx);
    return L7_FAILURE;
  }

  if ( (client_ref->mask & PTIN_CLIENT_MASK_FIELD_INTF) && !(client_ref->mask & PTIN_CLIENT_MASK_FIELD_INTIFNUM)) 
  {
      client_ref->intIfNum = port2intIfNum(client_ref->ptin_port);
      client_ref->mask |= PTIN_CLIENT_MASK_FIELD_INTIFNUM;
  }

  /* Key to search for */
  avl_tree = &pppoeClients_unified.avlTree;

  memset(&avl_key,0x00,sizeof(ptinPppoeClientDataKey_t));

  avl_key.pppoe_instance = pppoe_idx;
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  avl_key.intIfNum = (client_ref->mask & PTIN_CLIENT_MASK_FIELD_INTIFNUM) ? client_ref->intIfNum : 0;
  #endif
  #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
  avl_key.outerVlan = (client_ref->mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN) ? client_ref->outerVlan : 0;
  #endif
  #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
  avl_key.innerVlan = (client_ref->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) ? client_ref->innerVlan : 0;
  #endif
  #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
  avl_key.ipv4_addr = (client_ref->mask & PTIN_CLIENT_MASK_FIELD_IPADDR   ) ? client_ref->ipv4_addr : 0;
  #endif
  #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
  if (client_ref->mask & PTIN_CLIENT_MASK_FIELD_MACADDR)
    memcpy(avl_key.macAddr,client_ref->macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  else
    memset(avl_key.macAddr,0x00,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  #endif

  /* Search for this client */
  clientInfo = avlSearchLVL7( &(avl_tree->pppoeClientsAvlTree), (void *)&avl_key, AVL_EXACT);

  /* Check if this key already exists */
  if (clientInfo==L7_NULLPTR)
  {
    if (ptin_debug_pppoe_snooping)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Key {"
              #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                                "intIfNum=%u"
              #endif
              #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                                ",svlan=%u"
              #endif
              #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                                ",cvlan=%u"
              #endif
              #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                                ",ipAddr=%u.%u.%u.%u"
              #endif
              #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                                ",MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
              #endif
                                "} does not exist in pppoe_idx=%u",
              #if (PPPOE_CLIENT_INTERF_SUPPORTED)
              avl_key.intIfNum,
              #endif
              #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
              avl_key.outerVlan,
              #endif
              #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
              avl_key.innerVlan,
              #endif
              #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
              (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
              #endif
              #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
              avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
              #endif
              pppoe_idx);
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
 * Remove all PPPOE clients
 * 
 * @param pppoe_idx : PPPOE instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_pppoe_instance_deleteAll_clients(L7_uint pppoe_idx)
{  
  ptinPppoeClientDataKey_t   avl_key;
  ptinPppoeClientInfoData_t *avl_info;
  L7_uint32 client_idx;

  /* Validate argument */
  if (pppoe_idx>=PTIN_SYSTEM_N_PPPOE_INSTANCES)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid pppoe instance index (%u)",pppoe_idx);
    return L7_FAILURE;
  }
  /* PPPOE instance must be in use */
  if (!pppoeInstances[pppoe_idx].inUse)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"pppoe instance index %u is not in use",pppoe_idx);
    return L7_FAILURE;
  }

  /* Run all cells in AVL tree related to this instance instance */
  memset(&avl_key,0x00,sizeof(ptinPppoeClientDataKey_t));
  while ( (avl_info = (ptinPppoeClientInfoData_t *)
                      avlSearchLVL7(&pppoeClients_unified.avlTree.pppoeClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
     /* Prepare next key */
     memcpy(&avl_key, &avl_info->pppoeClientDataKey, sizeof(ptinPppoeClientDataKey_t));

     /* Skip items not belonging to this instance */
     if (avl_key.pppoe_instance != pppoe_idx)
       continue;

     /* Save client index */
     client_idx = avl_info->client_index;

     /* Delete node */
     if (avlDeleteEntry(&pppoeClients_unified.avlTree.pppoeClientsAvlTree, (void *)&avl_key) != L7_NULLPTR)
     {
       /* Release client index */
       pppoe_clientIndex_release(pppoe_idx, client_idx);
     }
  }

  PT_LOG_TRACE(LOG_CTX_PPPOE,"Success removing all clients from pppoe_idx=%u",pppoe_idx);

  return L7_SUCCESS;
}

/**
 * Get PPPOE instance from internal vlan
 * 
 * @param intVlan      : internal vlan
 * @param pppoeInst     : pppoe instance 
 * @param pppoeInst_idx : pppoe instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_pppoe_inst_get_fromIntVlan(L7_uint16 intVlan, st_PppoeInstCfg_t **pppoeInst, L7_uint *pppoeInst_idx)
{
  L7_uint32 evc_idx;
  L7_uint   pppoe_idx;

  /* Verify if this internal vlan is associated to an EVC */
  if (ptin_evc_get_evcIdfromIntVlan(intVlan, &evc_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"No EVC associated to internal vlan %u",intVlan);
    return L7_FAILURE;
  }

  if (ptin_evc_pppoeInst_get(evc_idx, &pppoe_idx) != L7_SUCCESS ||
      pppoe_idx >= PTIN_SYSTEM_N_PPPOE_INSTANCES)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"No PPPOE instance associated to evcId=%u (intVlan=%u)",evc_idx,intVlan);
    return L7_FAILURE;
  }

  /* Check if this instance is in use, and if evc_ids are valid */
  if (!pppoeInstances[pppoe_idx].inUse)
  {
    if (ptin_debug_pppoe_snooping)
      PT_LOG_ERR(LOG_CTX_PPPOE,"Inconsistency: PPPOE index %u (EVCid=%u, Vlan %u) is not in use",pppoe_idx,evc_idx,intVlan);
    return L7_FAILURE;
  }

  /* Return pppoe instance */
  if (pppoeInst!=L7_NULLPTR)     *pppoeInst     = &pppoeInstances[pppoe_idx];
  if (pppoeInst_idx!=L7_NULLPTR) *pppoeInst_idx = pppoe_idx;

  return L7_SUCCESS;
}

/**
 * Gets a free PPPOE instance entry
 * 
 * @param pppoe_idx : Output index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_pppoe_instance_find_free(L7_uint *pppoe_idx)
{
  L7_uint idx;

  /* Search for the first empty instance */
  for (idx=0; idx<PTIN_SYSTEM_N_PPPOE_INSTANCES && pppoeInstances[idx].inUse; idx++);

  /* If not found empty instances, return error */
  if (idx>=PTIN_SYSTEM_N_PPPOE_INSTANCES)
    return L7_FAILURE;

  /* Return instance index */
  if (pppoe_idx!=L7_NULLPTR)  *pppoe_idx = idx;

  return L7_SUCCESS;
}

/**
 * Gets the PPPOE instance with a specific Ucast EVC ids 
 * 
 * @param evc_idx : Unicast EVC id
 * @param pppoe_idx   : PPPOE instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_pppoe_instance_find(L7_uint32 evc_idx, L7_uint *pppoe_idx)
{
  #if 1
  L7_uint pppoe_inst;

  /* Validate evc index */
  if (evc_idx >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    return L7_FAILURE;
  }
  /* Check if there is an instance associated to this EVC */
  if (ptin_evc_pppoeInst_get(evc_idx, &pppoe_inst) != L7_SUCCESS ||
      pppoe_inst >= PTIN_SYSTEM_N_PPPOE_INSTANCES)
  {
    return L7_FAILURE;
  }

  /* Return index */
  if (pppoe_idx!=L7_NULLPTR)  *pppoe_idx = pppoe_inst;

  return L7_SUCCESS;
  #else
  L7_uint idx;

  /* Search for the provided Mcast and Ucast evcs */
  for (idx=0; idx<PTIN_SYSTEM_N_PPPOE_INSTANCES; idx++)
  {
    if (!pppoeInstances[idx].inUse)  continue;

    if (pppoeInstances[idx].evc_idx==evc_idx)
      break;
  }

  /* If not found empty instances, return error */
  if (idx>=PTIN_SYSTEM_N_PPPOE_INSTANCES)
    return L7_FAILURE;

  /* Return instance index */
  if (pppoe_idx!=L7_NULLPTR)  *pppoe_idx = idx;

  return L7_SUCCESS;
  #endif
}

/**
 * Configure trapping vlans
 * 
 * @param evc_idx 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t ptin_pppoe_evc_trap_configure(L7_uint32 evc_idx, L7_BOOL enable)
{
  L7_uint16 vlan;

  enable &= 1;

  if (ptin_evc_intRootVlan_get(evc_idx, &vlan) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Can't get root vlan for evc id %u", evc_idx);
    return L7_FAILURE;
  }

#if (PTIN_QUATTRO_FLOWS_FEATURE_ENABLED && QUATTRO_PPPOE_TRAP_PREACTIVE)
  if (!PTIN_VLAN_IS_QUATTRO(vlan))
#endif
  {
    if (ptin_pppoePkts_vlan_trap(vlan, enable)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Error configuring vlan %u for packet trapping", vlan);
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_PPPOE,"Success configuring vlan %u for packet trapping", vlan);
  }

  return L7_SUCCESS;
}

void ptin_pppoe_evc_ethprty_get(ptin_AccessNodeCircuitId_t *evc_circuitid, L7_uint8 *ethprty)
{
   if(L7_NULLPTR == evc_circuitid || L7_NULLPTR == ethprty)
   {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments");
   }

   *ethprty = evc_circuitid->ethernet_priority;
}


#define CIRCUITID_TEMPLATE_MAX_STRING_TMP   (CIRCUITID_TEMPLATE_MAX_STRING*2)

void ptin_pppoe_circuitId_build(ptin_AccessNodeCircuitId_t *evc_circuitid, ptin_clientCircuitId_t *client_circuitid, L7_char8 *circuitid)
{
  L7_uchar8 temp_str[CIRCUITID_TEMPLATE_MAX_STRING_TMP] = { 0 };
  L7_uchar8 chassis[3] = { 0 };
  L7_uchar8 rack[3] = { 0 };
  L7_uchar8 frame[3] = { 0 };
  L7_uchar8 ethernet_priority[2] = { 0 };
  L7_uchar8 s_vid[5] = { 0 };
  L7_uchar8 onuid[4] = { 0 };
  L7_uchar8 slot[3] = { 0 };
  L7_uchar8 port[4] = { 0 };
  L7_uchar8 q_vid[5] = { 0 };
  L7_uchar8 c_vid[5] = { 0 };

  sprintf(chassis,            "%d", evc_circuitid->chassis);
  sprintf(rack,               "%d", evc_circuitid->rack);
  sprintf(frame,              "%d", evc_circuitid->frame);
  sprintf(ethernet_priority,  "%d", evc_circuitid->ethernet_priority);
  sprintf(s_vid,              "%d", evc_circuitid->s_vid);
  sprintf(onuid,              "%d", client_circuitid->onuid);
  sprintf(slot,               "%d", client_circuitid->slot);
  sprintf(port,               "%d", client_circuitid->port);
  sprintf(q_vid,              "%d", client_circuitid->q_vid);
  sprintf(c_vid,              "%d", client_circuitid->c_vid);

  strncpy(temp_str, evc_circuitid->template_str, CIRCUITID_TEMPLATE_MAX_STRING);
  temp_str[CIRCUITID_TEMPLATE_MAX_STRING-1] = '\0';

  ptin_pppoe_circuitid_convert(temp_str, CIRCUITID_ACCESSNODEID_STR,     evc_circuitid->access_node_id);
  ptin_pppoe_circuitid_convert(temp_str, CIRCUITID_CHASSIS_STR,          chassis);
  ptin_pppoe_circuitid_convert(temp_str, CIRCUITID_RACK_STR,             rack);
  ptin_pppoe_circuitid_convert(temp_str, CIRCUITID_FRAME_STR,            frame);
  ptin_pppoe_circuitid_convert(temp_str, CIRCUITID_ETHERNETPRIORITY_STR, ethernet_priority);
  ptin_pppoe_circuitid_convert(temp_str, CIRCUITID_S_VID_STR,            s_vid);
  ptin_pppoe_circuitid_convert(temp_str, CIRCUITID_ONUID_STR,            onuid);
  ptin_pppoe_circuitid_convert(temp_str, CIRCUITID_SLOT_STR,             slot);
  ptin_pppoe_circuitid_convert(temp_str, CIRCUITID_PORT_STR,             port);
  ptin_pppoe_circuitid_convert(temp_str, CIRCUITID_Q_VID_STR,            q_vid);
  ptin_pppoe_circuitid_convert(temp_str, CIRCUITID_C_VID_STR,            c_vid);

  strncpy(circuitid, temp_str, FD_DS_MAX_REMOTE_ID_STRING);
  circuitid[FD_DS_MAX_REMOTE_ID_STRING-1] = '\0';
}

void ptin_pppoe_circuitid_convert(L7_char8 *circuitid_str, L7_char8 *str_to_replace, L7_char8 *parameter)
{
  L7_uint32 aux_len = 0;
  L7_uint32 copy_len;
  L7_uchar8 copy_circuitid[CIRCUITID_TEMPLATE_MAX_STRING_TMP] = { 0 };
  L7_char8 *found_pos = circuitid_str;

  /* Search for the pointer to the field to search... if not null, it was found! */
  while (L7_NULLPTR != (found_pos = strstr(found_pos, str_to_replace)))
  {
    /* Save original circuitId_str (template), and then clear it */
    strncpy(copy_circuitid, circuitid_str, CIRCUITID_TEMPLATE_MAX_STRING_TMP);
    copy_circuitid[CIRCUITID_TEMPLATE_MAX_STRING_TMP-1] = '\0';

    /* Copy beginning of the template string (before the field id) */
    copy_len = found_pos - circuitid_str;
    strncpy(circuitid_str, copy_circuitid, copy_len);

    /* Copy parameter value */
    aux_len = found_pos - circuitid_str;
    copy_len = strlen(parameter);
    if ( (aux_len + copy_len) >= CIRCUITID_TEMPLATE_MAX_STRING_TMP )
      copy_len = CIRCUITID_TEMPLATE_MAX_STRING_TMP - aux_len - 1;

    strncpy(circuitid_str + aux_len, parameter, copy_len );

    /* Copy the remainning template string (after field id) */
    aux_len += copy_len;
    copy_len = CIRCUITID_TEMPLATE_MAX_STRING_TMP-((found_pos-circuitid_str)+strlen(str_to_replace));
    if ( (aux_len + copy_len) >= CIRCUITID_TEMPLATE_MAX_STRING_TMP )
      copy_len = CIRCUITID_TEMPLATE_MAX_STRING_TMP - aux_len - 1;

    strncpy(circuitid_str + aux_len, copy_circuitid+(found_pos-circuitid_str)+strlen(str_to_replace), copy_len);
    circuitid_str[aux_len + copy_len] = '\0';

    found_pos = &circuitid_str[aux_len];
  }
}

L7_RC_t ptin_pppoe_reconf_instance(L7_uint32 pppoe_instance_idx, L7_uint8 pppoe_flag, L7_uint32 options)
{
   struct ptin_clientInfo_entry_s *clientInfo_entry;

   /* Validate pppoe instance */
   if (!pppoeInstances[pppoe_instance_idx].inUse)
   {
    PT_LOG_ERR(LOG_CTX_PPPOE, "PPPOE instance %u is not in use", pppoe_instance_idx);
    return L7_FAILURE;
   }

   /* Save EVC PPPOE Options */
   pppoeInstances[pppoe_instance_idx].evcPppoeOptions = options;

   /* Clear statistics of belonging clients */
   clientInfo_entry = NULL;
   dl_queue_get_head(&pppoeInstances[pppoe_instance_idx].queue_clients, (dl_queue_elem_t **)&clientInfo_entry);
   while (clientInfo_entry != NULL)
   {
     /* Reconfigure PPPOE options for clients that are using Global EVC PPPOE options */
     if(clientInfo_entry->client_info != L7_NULLPTR &&
        L7_TRUE == clientInfo_entry->client_info->client_data.useEvcPppoeOptions)
     {
        clientInfo_entry->client_info->client_data.pppoe_options = options;
     }

     /* Next queue element */
     clientInfo_entry = (struct ptin_clientInfo_entry_s *) dl_queue_get_next(&pppoeInstances[pppoe_instance_idx].queue_clients, (dl_queue_elem_t *) clientInfo_entry);
   }

   return L7_SUCCESS;
}

/* DEBUG Functions ************************************************************/
/**
 * Dumps EVC detailed info 
 * If evc_idx is invalid, all EVCs are dumped 
 * 
 * @param evc_idx 
 */
void ptin_pppoe_dump(L7_BOOL show_clients)
{
  L7_uint i, i_client;
  ptinPppoeClientInfoData_t *avl_info;
  struct ptin_clientInfo_entry_s *clientInfo_entry;

  for (i = 0; i < PTIN_SYSTEM_N_PPPOE_INSTANCES; i++)
  {
    if (!pppoeInstances[i].inUse) 
    {
      printf("*** PPPoE instance %02u not in use\r\n", i);
      continue;
    }

    printf("PPPoE instance %02u: NNI_VLAN=%-4u #evcs=%-5u options=0x%04x [CircuitId Template: %s]  ", i,
           pppoeInstances[i].nni_ovid, pppoeInstances[i].n_evcs,
           pppoeInstances[i].evcPppoeOptions, pppoeInstances[i].circuitid.template_str);
    printf("\r\n");

    if (show_clients)
    {
      i_client = 0; 

      /* Run all instance belonging clients */
      clientInfo_entry = NULL;
      dl_queue_get_head(&pppoeInstances[i].queue_clients, (dl_queue_elem_t **)&clientInfo_entry);
      while (clientInfo_entry != NULL)
      {
        avl_info = clientInfo_entry->client_info;

        if (avl_info != L7_NULLPTR)
        {
          printf("   Client#%-5u: "
                 #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                 "intIfNum=%-2u "
                 #endif
                 #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                 "svlan=%-4u "
                 #endif
                 #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                 "cvlan=%-4u "
                 #endif
                 #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                 "IP=%03u.%03u.%03u.%03u "
                 #endif
                 #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                 "MAC=%02x:%02x:%02x:%02x:%02x:%02x "
                 #endif
                 ": ptin_port=%-2u [uni_vlans=%4u+%-4u] options=0x%04x circuitId=\"%s\" remoteId=\"%s\"\r\n",
                 avl_info->client_index,
                 #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                 avl_info->pppoeClientDataKey.intIfNum,
                 #endif
                 #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
                 avl_info->pppoeClientDataKey.outerVlan,
                 #endif
                 #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
                 avl_info->pppoeClientDataKey.innerVlan,
                 #endif
                 #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
                 (avl_info->pppoeClientDataKey.ipv4_addr>>24) & 0xff,
                  (avl_info->pppoeClientDataKey.ipv4_addr>>16) & 0xff,
                   (avl_info->pppoeClientDataKey.ipv4_addr>>8) & 0xff,
                    avl_info->pppoeClientDataKey.ipv4_addr & 0xff,
                 #endif
                 #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
                 avl_info->pppoeClientDataKey.macAddr[0],
                  avl_info->pppoeClientDataKey.macAddr[1],
                   avl_info->pppoeClientDataKey.macAddr[2],
                    avl_info->pppoeClientDataKey.macAddr[3],
                     avl_info->pppoeClientDataKey.macAddr[4],
                      avl_info->pppoeClientDataKey.macAddr[5],
                 #endif
                 avl_info->ptin_port,
                 avl_info->uni_ovid, avl_info->uni_ivid,
                 avl_info->client_data.pppoe_options,
                 avl_info->client_data.circuitId_str,
                 avl_info->client_data.remoteId_str);
        }
        else
        {
          printf("   Entry %u has a null pointer\r\n", i_client);
        }

        i_client++;

        /* Next queue element */
        clientInfo_entry = (struct ptin_clientInfo_entry_s *) dl_queue_get_next(&pppoeInstances[i].queue_clients, (dl_queue_elem_t *) clientInfo_entry);
      }
    }
  }

  printf("Total number of PPPoE clients: %u\r\n", pppoeClients_unified.number_of_clients);
  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  printf("Total number of QUATTRO-STACKED evcs: %u\r\n", pppoe_quattro_stacked_evcs);
  #endif

  fflush(stdout);
}

void ptin_pppoeClients_dump(void)
{
  L7_uint i_client = 0;
  ptinPppoeClientDataKey_t avl_key;
  ptinPppoeClientInfoData_t *avl_info;

  printf("Listing complete list of PPPoE clients:\r\n");

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinPppoeClientDataKey_t));
  while ( ( avl_info = (ptinPppoeClientInfoData_t *)
                        avlSearchLVL7(&pppoeClients_unified.avlTree.pppoeClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->pppoeClientDataKey, sizeof(ptinPppoeClientDataKey_t));

    printf("   Client#%-5u: Inst=%-2u "
           #if (PPPOE_CLIENT_INTERF_SUPPORTED)
           "intIfNum=%-2u "
           #endif
           #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
           "svlan=%-4u "
           #endif
           #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
           "cvlan=%-4u "
           #endif
           #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
           "IP=%03u.%03u.%03u.%03u "
           #endif
           #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
           "MAC=%02x:%02x:%02x:%02x:%02x:%02x "
           #endif
           ": [uni_vlans=%4u+%-4u] options=0x%04x circuitId=\"%s\" remoteId=\"%s\"\r\n",
           avl_info->client_index,
           avl_info->pppoeClientDataKey.pppoe_instance,
           #if (PPPOE_CLIENT_INTERF_SUPPORTED)
           avl_info->pppoeClientDataKey.intIfNum,
           #endif
           #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
           avl_info->pppoeClientDataKey.outerVlan,
           #endif
           #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
           avl_info->pppoeClientDataKey.innerVlan,
           #endif
           #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
           (avl_info->pppoeClientDataKey.ipv4_addr>>24) & 0xff,
            (avl_info->pppoeClientDataKey.ipv4_addr>>16) & 0xff,
             (avl_info->pppoeClientDataKey.ipv4_addr>>8) & 0xff,
              avl_info->pppoeClientDataKey.ipv4_addr & 0xff,
           #endif
           #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
           avl_info->pppoeClientDataKey.macAddr[0],
            avl_info->pppoeClientDataKey.macAddr[1],
             avl_info->pppoeClientDataKey.macAddr[2],
              avl_info->pppoeClientDataKey.macAddr[3],
               avl_info->pppoeClientDataKey.macAddr[4],
                avl_info->pppoeClientDataKey.macAddr[5],
           #endif
           avl_info->uni_ovid, avl_info->uni_ivid,
           avl_info->client_data.pppoe_options,
           avl_info->client_data.circuitId_str,
           avl_info->client_data.remoteId_str);

    i_client++;
  }

  printf("Total number of PPPoE clients: %u\r\n", pppoeClients_unified.number_of_clients);
  fflush(stdout);
}

#if PPPOE_ACCEPT_UNSTACKED_PACKETS
static L7_RC_t ptin_pppoe_strings_def_get(ptin_intf_t *ptin_intf, L7_uchar8 *macAddr, L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_uint slot = 0;
  L7_uint port_type = 0;
  L7_uint port_id = 0;

  /* Circuit id */
  if (circuitId!=L7_NULLPTR)
  {
    slot = ptin_fpga_board_slot_get();

    if (slot>99)  slot = 99;

    if (ptin_intf!=L7_NULLPTR)
    {
      port_type = ptin_intf->intf_type;
      if (port_type>9)  port_type = 9;

      port_id   = ptin_intf->intf_id;
      if (port_id>99)   port_id = 99;
    }
    sprintf(circuitId,"%.40s eth %02u-000/%u%02u/0/0/0",DEFAULT_ACCESS_NODE_ID,slot,port_type,port_id);
  }

  /* Remote id */
  if (remoteId!=L7_NULLPTR)
  {
    if (macAddr!=L7_NULLPTR)
    {
      /* Remote id is the MAC address */
      sprintf(remoteId,"%02x:%02x:%02x:%02x:%02x:%02x",macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
    }
    else
    {
      sprintf(remoteId,"%s","Unknown");
    }
  }

  return L7_SUCCESS;
}
#endif

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
static L7_RC_t ptin_pppoe_clientId_convert(L7_uint32 evc_idx, ptin_client_id_t *client)
{
  L7_uint16 intVlan, innerVlan;

  /* Validate evc index  */
  if (evc_idx>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid eEVC id: evc_idx=%u",evc_idx);
    return L7_FAILURE;
  }
  /* This evc must be active */
  if (!ptin_evc_is_in_use(evc_idx))
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"eEVC id is not active: evc_idx=%u",evc_idx);
    return L7_FAILURE;
  }
  /* Validate client */
  if (client==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid client pointer");
    return L7_FAILURE;
  }

  /* Check mask */
  if (PPPOE_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    PT_LOG_WARN(LOG_CTX_PPPOE,"Client mask is null");
    return L7_FAILURE;
  }

  innerVlan = 0;
  /* Validate inner vlan */
  #if PPPOE_CLIENT_INNERVLAN_SUPPORTED
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN)
  {
    /* Validate inner vlan */
    if (client->innerVlan>4095)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid inner vlan (%u)",client->innerVlan);
      return L7_FAILURE;
    }
    innerVlan = client->innerVlan;
  }
  #endif

  /* Update outer vlan */
  #if defined(PPPOE_CLIENT_INTERF_SUPPORTED) && defined(PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
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
        PT_LOG_ERR(LOG_CTX_PPPOE,"Error obtaining internal vlan for evc_idx=%u, ptin_intf=%u/%u",
                evc_idx, client->ptin_intf.intf_type, client->ptin_intf.intf_id);
        return L7_FAILURE;
      }
    }
    else
    {
      /* Obtain intVlan from the outer vlan */
      if (ptin_evc_intVlan_get_fromOVlan(&client->ptin_intf, client->outerVlan, innerVlan, &intVlan)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_PPPOE,"Error obtaining internal vlan for OVid=%u, IVid=%u, ptin_intf=%u/%u",
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

#if 0
/**
 * Restore client id structure to values which manager 
 * understands.
 * 
 * @param client : client id
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_pppoe_clientId_restore(ptin_client_id_t *client)
{
  L7_uint32 intIfNum;
  L7_uint16 extVlan, innerVlan;

  /* Validate client */
  if (client==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Check mask */
  if (PPPOE_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    PT_LOG_WARN(LOG_CTX_PPPOE,"Client mask is null");
    return L7_FAILURE;
  }

  innerVlan = 0;
  #if PPPOE_CLIENT_INNERVLAN_SUPPORTED
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN)
  {
    /* Validate inner vlan */
    if (client->innerVlan>4095)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid inner vlan (%u)",client->innerVlan);
      return L7_FAILURE;
    }
    innerVlan = client->innerVlan;
  }
  #endif

  #if defined(PPPOE_CLIENT_INTERF_SUPPORTED) && defined(PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
  /* Is interface and outer vlan provided? If so, replace it with the internal vlan */
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF &&
      client->mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN)
  {
    /* Convert to intIfNum format */
    if (ptin_intf_ptintf2intIfNum(&client->ptin_intf, &intIfNum)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Cannot convert client intf %u/%u to intIfNum format",
              client->ptin_intf.intf_type,client->ptin_intf.intf_id);
      return L7_FAILURE;
    }

    /* Validate outer vlan */
    if (client->outerVlan<PTIN_VLAN_MIN || client->outerVlan>PTIN_VLAN_MAX)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Invalid outer vlan (%u)",client->outerVlan);
      return L7_FAILURE;
    }
    /* Replace the outer vlan, with the internal vlan relative to the leaf interface */
    if (ptin_evc_extVlans_get_fromIntVlan(intIfNum, client->outerVlan, innerVlan, &extVlan, L7_NULLPTR)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_PPPOE,"Could not obtain external vlan for intVlan %u, ptin_intf %u/%u",
              client->outerVlan, client->ptin_intf.intf_type, client->ptin_intf.intf_id);
      return L7_FAILURE;
    }
    /* Replace outer vlan with the internal one */
    client->outerVlan = extVlan;
  }
  #endif

  return L7_SUCCESS;
}
#endif

