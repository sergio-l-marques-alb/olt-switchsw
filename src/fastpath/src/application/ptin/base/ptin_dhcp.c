/**
 * ptin_dhcp.c 
 *  
 * Implements the DHCP snooping interface module
 *
 * Created on: 2012/01/06
 * Author: Milton Ruas (milton-r-silva@ext.ptinovacao.pt)
 *  
 * Notes:
 */

#include "ptin_dhcp.h"
#include "ptin_xlate_api.h"
#include "ptin_intf.h"
#include "ptin_utils.h"
#include "ptin_evc.h"
#include "ptin_fieldproc.h"

#include "usmdb_dhcp_snooping.h"

/********************************************* 
 * Defines
 *********************************************/

#define DHCP_ACCEPT_UNSTACKED_PACKETS 0

#define DEFAULT_ACCESS_NODE_ID  "OLT360"   /* No more than 40 characters */

#define DHCP_INVALID_ENTRY    0xFF

#define DHCP_CLIENT_DEBUG 1
L7_BOOL ptin_debug_dhcp_snooping = 0;

void ptin_debug_dhcp_enable(L7_BOOL enable)
{
  ptin_debug_dhcp_snooping = enable;
}

/* Parameters to identify the client */
#define DHCP_CLIENT_INTERF_SUPPORTED    1
#if (PTIN_BOARD_IS_ACTIVETH)
 #define DHCP_CLIENT_OUTERVLAN_SUPPORTED 1
#else
 #define DHCP_CLIENT_OUTERVLAN_SUPPORTED 0
#endif
#define DHCP_CLIENT_INNERVLAN_SUPPORTED 1
#define DHCP_CLIENT_IPADDR_SUPPORTED    0
#define DHCP_CLIENT_MACADDR_SUPPORTED   0

/* At least one parameter must be active */
#if ( !( DHCP_CLIENT_INTERF_SUPPORTED    |  \
         DHCP_CLIENT_OUTERVLAN_SUPPORTED |  \
         DHCP_CLIENT_INNERVLAN_SUPPORTED |  \
         DHCP_CLIENT_IPADDR_SUPPORTED    |  \
         DHCP_CLIENT_MACADDR_SUPPORTED ) )
  #error "ptin_dhcp.c: At least one parameter must be defined!"
#endif

/* To validate mask according to the suuported parameters */
#define DHCP_CLIENT_MASK_UPDATE(mask) \
        ( ( (PTIN_CLIENT_MASK_FIELD_INTF      & DHCP_CLIENT_INTERF_SUPPORTED   ) |   \
            (PTIN_CLIENT_MASK_FIELD_OUTERVLAN & DHCP_CLIENT_OUTERVLAN_SUPPORTED) |   \
            (PTIN_CLIENT_MASK_FIELD_INNERVLAN & DHCP_CLIENT_INNERVLAN_SUPPORTED) |   \
            (PTIN_CLIENT_MASK_FIELD_IPADDR    & DHCP_CLIENT_IPADDR_SUPPORTED   ) |   \
            (PTIN_CLIENT_MASK_FIELD_MACADDR   & DHCP_CLIENT_MACADDR_SUPPORTED  ) ) & (mask) )


/*********************************************************** 
 * Typedefs
 ***********************************************************/

typedef struct
{
  L7_uint8  dhcp_instance;

  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  L7_uint8  ptin_port;                /* PTin port, which is attached */
  #endif
  #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
  L7_uint16 outerVlan;                /* Outer Vlan */
  #endif
  #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
  L7_uint16 innerVlan;                /* Inner Vlan */
  #endif
  #if (DHCP_CLIENT_IPADDR_SUPPORTED)
  L7_uint32 ipv4_addr;                /* IP address */
  #endif
  #if (DHCP_CLIENT_MACADDR_SUPPORTED)
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN]; /* Source MAC */
  #endif
} ptinDhcpClientDataKey_t;

typedef struct
{
  L7_BOOL                 useEvcDhcpOptions;
  L7_uint16               dhcp_options;
  ptin_clientCircuitId_t  circuitId;                                    /* Circuit ID parameters */
  L7_char8                circuitId_str[FD_DS_MAX_REMOTE_ID_STRING+1];  /* Circuit ID string */
  L7_char8                remoteId_str[FD_DS_MAX_REMOTE_ID_STRING+1];       /* Remote ID string */
} ptinDhcpData_t;

typedef struct
{
  ptinDhcpClientDataKey_t dhcpClientDataKey;
  L7_uint16               client_index;
  L7_uint16               uni_ovid;
  L7_uint16               uni_ivid;
  ptinDhcpData_t          client_data;
  ptin_DHCP_Statistics_t  client_stats;   /* Client statistics */
  void *next;
} ptinDhcpClientInfoData_t;

typedef struct {
    avlTree_t                 dhcpClientsAvlTree;
    avlTreeTables_t           *dhcpClientsTreeHeap;
    ptinDhcpClientInfoData_t  *dhcpClientsDataHeap;
} ptinDhcpClientsAvlTree_t;

/* DHCP AVL Tree data */
typedef struct {
  L7_uint16                 number_of_clients;
  ptinDhcpClientsAvlTree_t  avlTree;
} ptinDhcpClients_unified_t;

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

  ptinDhcpClientInfoData_t *client_info;
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

/* DHCP Instance config struct */
typedef struct {
  L7_BOOL                     inUse;
  L7_uint32                   evc_idx;
  L7_uint16                   nni_ovid;
  L7_uint16                   n_evcs;
  ptin_dhcp_flag_enum_t       dhcpFlags;
  L7_uint16                   evcDhcpOptions;   /* DHCP Options (0x01=Option82; 0x02=Option37; 0x02=Option18) */
  dl_queue_t                  queue_clients;
  ptin_DHCP_Statistics_t      stats_intf[PTIN_SYSTEM_N_INTERF];  /* DHCP statistics at interface level */
  ptin_AccessNodeCircuitId_t  circuitid;
} st_DhcpInstCfg_t;

#define QUATTRO_DHCP_TRAP_PREACTIVE     0   /* To always have this rule active, set 1 */
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
/* Global number of QUATTRO P2P flows */
static L7_uint32 dhcp_quattro_stacked_evcs = 0;
#endif

/*********************************************************** 
 * Data structs
 ***********************************************************/

/* Global DHCP statistics at interface level */
NIM_INTF_MASK_t dhcp_intIfNum_trusted;

/* DHCP instances array */
st_DhcpInstCfg_t  dhcpInstances[PTIN_SYSTEM_N_DHCP_INSTANCES];

/* DHCP clients */
ptinDhcpClients_unified_t dhcpClients_unified;

/* Global DHCP statistics at interface level */
static ptin_DHCP_Statistics_t global_stats_intf[PTIN_SYSTEM_N_INTERF];

/* Semaphores */
void *dhcp_sem = NULL;
void *ptin_dhcp_stats_sem = L7_NULLPTR;


static struct ptin_clientIdx_entry_s  clientIdx_pool[PTIN_SYSTEM_DHCP_MAXCLIENTS];
static struct ptin_clientInfo_entry_s clientInfo_pool[PTIN_SYSTEM_DHCP_MAXCLIENTS];

static dl_queue_t queue_free_clients;    /* Queue of free client entries */


/*********************************************************** 
 * Static prototypes
 ***********************************************************/

static L7_RC_t ptin_dhcp_client_find(L7_uint dhcp_idx, ptin_client_id_t *client_ref, ptinDhcpClientInfoData_t **client_info);
static L7_RC_t ptin_dhcp_instance_deleteAll_clients(L7_uint dhcp_idx);
static L7_RC_t ptin_dhcp_inst_get_fromIntVlan(L7_uint16 intVlan, st_DhcpInstCfg_t **dhcpInst, L7_uint *dhcpInst_idx);
static L7_RC_t ptin_dhcp_instance_find_free(L7_uint *idx);
static L7_RC_t ptin_dhcp_instance_find(L7_uint32 evc_idx, L7_uint *dhcp_idx);
static L7_RC_t ptin_dhcp_instance_find_agg(L7_uint16 nni_ovlan, L7_uint *dhcp_idx);
static L7_RC_t ptin_dhcp_trap_configure(L7_uint dhcp_idx, L7_BOOL enable, L7_uint8 family);
static void    ptin_dhcp_evc_ethprty_get(ptin_AccessNodeCircuitId_t *evc_circuitid, L7_uint8 *ethprty);
static L7_RC_t ptin_dhcp_circuitid_set_instance(L7_uint16 dhcp_idx, L7_char8 *template_str, L7_uint32 mask, L7_char8 *access_node_id, L7_uint8 chassis,
                                                L7_uint8 rack, L7_uint8 frame, L7_uint8 ethernet_priority, L7_uint16 s_vid);
static L7_RC_t ptin_dhcp_flags_set_instance(L7_uint16 dhcp_idx, L7_uchar8 mask, L7_char8 flags);
static void    ptin_dhcp_circuitId_build(ptin_AccessNodeCircuitId_t *evc_circuitid, ptin_clientCircuitId_t *client_circuitid, L7_char8 *circuitid);
static void    ptin_dhcp_circuitid_convert(L7_char8 *circuitid_str, L7_char8 *str_to_replace, L7_char8 *parameter);
static L7_RC_t ptin_dhcp_reconf_instance(L7_uint32 dhcp_instance_idx, L7_uint8 dhcp_flag, L7_uint32 options);

#if DHCP_ACCEPT_UNSTACKED_PACKETS
static L7_RC_t ptin_dhcp_strings_def_get(ptin_intf_t *ptin_intf, L7_uchar8 *macAddr, L7_char8 *circuitId, L7_char8 *remoteId);
#endif

static L7_RC_t ptin_dhcp_clientId_convert(L7_uint32 evc_idx, ptin_client_id_t *client);
//static L7_RC_t ptin_dhcp_clientId_restore(ptin_client_id_t *client);

/*********************************************************** 
 * INLINE FUNCTIONS
 ***********************************************************/

inline L7_BOOL dhcp_clientIndex_check_free(L7_uint8 dhcp_idx)
{
  /* Validate arguments */
  if (dhcp_idx >= PTIN_SYSTEM_N_DHCP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid DHCP instance %u", dhcp_idx);
    return -1;
  }

  return (dhcpClients_unified.number_of_clients < PTIN_SYSTEM_DHCP_MAXCLIENTS &&
          queue_free_clients.n_elems > 0);
}

inline L7_int dhcp_clientIndex_allocate(L7_uint8 dhcp_idx, ptinDhcpClientInfoData_t *infoData)
{
  L7_int  client_idx;
  struct ptin_clientIdx_entry_s  *clientIdx_pool_entry;
  struct ptin_clientInfo_entry_s *clientInfo_pool_entry;
  L7_RC_t rc;

  /* Validate arguments */
  if (dhcp_idx >= PTIN_SYSTEM_N_DHCP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid DHCP instance %u", dhcp_idx);
    return -1;
  }

  /* Check if there is free clients */
  if (dhcpClients_unified.number_of_clients >= PTIN_SYSTEM_DHCP_MAXCLIENTS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"No free clients available");
    return -1;
  }

  /* Check if queue has free elements */
  if (queue_free_clients.n_elems == 0)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"No free clients available in queue");
    return -1;
  }

  /* Try to get an entry from the pool of free elements */
  rc = dl_queue_remove_head(&queue_free_clients, (dl_queue_elem_t **) &clientIdx_pool_entry);
  if (rc != NOERR) {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "There are no free clients available! rc=%d", rc);
    return -1;
  }

  client_idx = clientIdx_pool_entry->client_id;

  LOG_DEBUG(LOG_CTX_PTIN_DHCP, "Selected index=%u, Free clients pool: %u of %u entries",
            client_idx, queue_free_clients.n_elems, PTIN_SYSTEM_DHCP_MAXCLIENTS);

  /* Assign AVL entry reference */
  if (infoData != L7_NULLPTR)
  {
    /* Update clients list on related instance */
    clientInfo_pool_entry = &clientInfo_pool[client_idx];

    memset(clientInfo_pool_entry, 0x00, sizeof(struct ptin_clientInfo_entry_s));
    clientInfo_pool_entry->client_info = infoData;

    rc = dl_queue_add_tail(&dhcpInstances[dhcp_idx].queue_clients, (dl_queue_elem_t *) clientInfo_pool_entry);
    if (rc != NOERR) {
      memset(clientInfo_pool_entry, 0x00, sizeof(struct ptin_clientInfo_entry_s));
      dl_queue_add_head(&queue_free_clients, (dl_queue_elem_t *) clientIdx_pool_entry);
      LOG_ERR(LOG_CTX_PTIN_DHCP, "Error adding element to queue! rc=%d", rc);
      return -1;
    }
  }

  /* One more client */
  dhcpClients_unified.number_of_clients++;

  /* Return new client id */
  return client_idx;
}

inline void dhcp_clientIndex_release(L7_uint8 dhcp_idx, L7_uint32 client_idx)
{
  struct ptin_clientIdx_entry_s  *clientIdx_pool_entry;
  struct ptin_clientInfo_entry_s *clientInfo_pool_entry;
  L7_RC_t rc;

  /* Validate arguments */
  if (dhcp_idx >= PTIN_SYSTEM_N_DHCP_INSTANCES || client_idx >= PTIN_SYSTEM_DHCP_MAXCLIENTS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Invalid DHCP instance %u, or client index %u", dhcp_idx, client_idx);
    return;
  }

  /* Check if there is busy clients in queue */
  if (queue_free_clients.n_elems >= PTIN_SYSTEM_DHCP_MAXCLIENTS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "There are no busy clients in queue!");
    return;
  }

  /* Get the client entry based on its index */
  clientIdx_pool_entry  = &clientIdx_pool[client_idx];
  clientInfo_pool_entry = &clientInfo_pool[client_idx];

  /* Remove element from clientInfo queue */
  rc = dl_queue_remove(&dhcpInstances[dhcp_idx].queue_clients, (dl_queue_elem_t *) clientInfo_pool_entry);
  if (rc != NOERR) {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Error removing element from queue! rc=%d", rc);
    return;
  }

  /* Add it to the free queue */
  rc = dl_queue_add_tail(&queue_free_clients, (dl_queue_elem_t *) clientIdx_pool_entry);
  if (rc != NOERR) {
    dl_queue_add_head(&dhcpInstances[dhcp_idx].queue_clients, (dl_queue_elem_t *) clientInfo_pool_entry);
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Error adding client to free queue! rc=%d", rc);
    return;
  }

  /* Clear client info from queue */
  memset(&clientInfo_pool[client_idx], 0x00, sizeof(struct ptin_clientInfo_entry_s));

  /* One less client */
  if (dhcpClients_unified.number_of_clients > 0)
    dhcpClients_unified.number_of_clients--;

  LOG_DEBUG(LOG_CTX_PTIN_DHCP, "Free client pool: %u of %u entries",
            queue_free_clients.n_elems, PTIN_SYSTEM_DHCP_MAXCLIENTS);
}

/*********************************************************** 
 * Global functions
 ***********************************************************/

/**
 * Initialize circuitId+remoteId database
 *  
 * @return none
 */
L7_RC_t ptin_dhcp_init(void)
{
  L7_uint dhcp_idx, i;
  ptinDhcpClientsAvlTree_t *avlTree;

  #if 0
  /* Clear database */
  memset(dhcp_relay_database, 0x00, sizeof(dhcp_relay_database));
  #endif

  /* All interfaces are untrusted */
  ptin_dhcp_intfTrusted_init();

  /* Reset instances array */
  memset(dhcpInstances, 0x00, sizeof(dhcpInstances));

  /* Initialize global DHCP statistics */
  memset(global_stats_intf,0x00,sizeof(global_stats_intf));

  /* Initialize sempahore */
  dhcp_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dhcp_sem == NULL)
  {
    LOG_TRACE(LOG_CTX_PTIN_DHCP, "Error creating a mutex for DHCP module");
    return L7_FAILURE;
  }

  /* DHCP clients */
  avlTree = &dhcpClients_unified.avlTree;
  dhcpClients_unified.number_of_clients = 0;

  avlTree->dhcpClientsTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_DHCP_MAXCLIENTS * sizeof(avlTreeTables_t)); 
  avlTree->dhcpClientsDataHeap = (ptinDhcpClientInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_DHCP_MAXCLIENTS * sizeof(ptinDhcpClientInfoData_t)); 

  if ((avlTree->dhcpClientsTreeHeap == L7_NULLPTR) ||
      (avlTree->dhcpClientsDataHeap == L7_NULLPTR))
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error allocating data for DHCP AVL Trees\n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset (&avlTree->dhcpClientsAvlTree, 0x00, sizeof(avlTree_t));
  memset (avlTree->dhcpClientsTreeHeap, 0x00, sizeof(avlTreeTables_t)*PTIN_SYSTEM_DHCP_MAXCLIENTS);
  memset (avlTree->dhcpClientsDataHeap, 0x00, sizeof(ptinDhcpClientInfoData_t)*PTIN_SYSTEM_DHCP_MAXCLIENTS);

  // AVL Tree creations - snoopIpAvlTree
  avlCreateAvlTree(&(avlTree->dhcpClientsAvlTree),
                   avlTree->dhcpClientsTreeHeap,
                   avlTree->dhcpClientsDataHeap,
                   PTIN_SYSTEM_DHCP_MAXCLIENTS, 
                   sizeof(ptinDhcpClientInfoData_t),
                   0x10,
                   sizeof(ptinDhcpClientDataKey_t));

  /* Initialize clients queue for each DHCP instance */
  memset(clientInfo_pool, 0x00, sizeof(clientInfo_pool));
  for (dhcp_idx = 0; dhcp_idx < PTIN_SYSTEM_N_DHCP_INSTANCES; dhcp_idx++)
  {
    dl_queue_init(&dhcpInstances[dhcp_idx].queue_clients);
  }

  /* Init Client index management */
  dl_queue_init(&queue_free_clients);
  memset(clientIdx_pool, 0x00, sizeof(clientIdx_pool));
  for (i=0; i<PTIN_SYSTEM_DHCP_MAXCLIENTS; i++)
  {
    clientIdx_pool[i].client_id = i;
    dl_queue_add(&queue_free_clients, (dl_queue_elem_t*) &clientIdx_pool[i]);
  }

  /* Semaphores */
  ptin_dhcp_stats_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_dhcp_stats_sem == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to create ptin_dhcp_stats_sem semaphore!");
    return L7_FAILURE;
  }

  LOG_INFO(LOG_CTX_PTIN_DHCP, "sizeof(dhcp_intIfNum_trusted)      = %u", sizeof(dhcp_intIfNum_trusted));
  LOG_INFO(LOG_CTX_PTIN_DHCP, "sizeof(dhcpInstances)              = %u", sizeof(dhcpInstances));
  LOG_INFO(LOG_CTX_PTIN_DHCP, "sizeof(global_stats_intf)          = %u", sizeof(global_stats_intf));
  LOG_INFO(LOG_CTX_PTIN_DHCP, "sizeof(dhcpClients_unified.avlTree)= %u",
           sizeof(avlTree_t) + sizeof(avlTreeTables_t)*PTIN_SYSTEM_DHCP_MAXCLIENTS + sizeof(ptinDhcpClientInfoData_t)*PTIN_SYSTEM_DHCP_MAXCLIENTS);

  LOG_INFO(LOG_CTX_PTIN_DHCP, "DHCP init OK");


#if 0
  if (usmDbDsAdminModeSet(L7_ENABLE)!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error with usmDbDsAdminModeSet");
    return L7_FAILURE;
  }
  if (usmDbDsL2RelayAdminModeSet(L7_ENABLE)!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error with usmDbDsL2RelayAdminModeSet");
    return L7_FAILURE;
  }
#endif

  return L7_SUCCESS;
}

/**
 * Set Global enable for DHCP packet trapping
 * 
 * @param enable : L7_ENABLE/L7_DISABLE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_enable(L7_BOOL enable)
{
  if (usmDbDsAdminModeSet(enable)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error applying DHCP module enable=%u",enable);
    return L7_FAILURE;
  }
  if (dsL2RelayAdminModeSet(enable)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error applying DHCP Relay Agent enable=%u",enable);
    return L7_FAILURE;
  }
  
  /* Global trap */
  if (ptin_dhcpPkts_global_trap(enable)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error setting DHCP global enable to %u",enable);
    dsL2RelayAdminModeSet(!enable);
    usmDbDsAdminModeSet(!enable);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_DHCP,"Success setting DHCP global enable to %u",enable);

#if (PTIN_QUATTRO_FLOWS_FEATURE_ENABLED && QUATTRO_DHCP_TRAP_PREACTIVE)
  /* Configure packet trapping for this VLAN  */
  if (ptin_dhcpPkts_vlan_trap(PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN, enable, L7_AF_INET) != L7_SUCCESS ||
      ptin_dhcpPkts_vlan_trap(PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN, enable, L7_AF_INET6) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error configuring packet trapping for QUATTRO VLANs (enable=%u)", enable);
    ptin_dhcpPkts_vlan_trap(PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN, !enable, L7_AF_INET6);
    ptin_dhcpPkts_vlan_trap(PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN, !enable, L7_AF_INET);
    ptin_dhcpPkts_global_trap(!enable);
    dsL2RelayAdminModeSet(!enable);
    usmDbDsAdminModeSet(!enable);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet trapping for QUATTRO VLANs configured (enable=%u)", enable);
#endif

  return L7_SUCCESS;
}

/**
 * Check if a EVC is being used in an DHCP instance
 * 
 * @param evc_idx : evc id
 * 
 * @return L7_RC_t : L7_TRUE or L7_FALSE
 */
L7_RC_t ptin_dhcp_is_evc_used(L7_uint32 evc_idx)
{
  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid eEVC id: evc_idx=%u",evc_idx);
    return L7_FALSE;
  }

  /* This evc must be active */
  if (!ptin_evc_is_in_use(evc_idx))
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"EVC id is not active: evc_idx=%u",evc_idx);
    return L7_FALSE;
  }

  /* Check if this EVC is being used by any DHCP instance */
  if (ptin_dhcp_instance_find(evc_idx,L7_NULLPTR)!=L7_SUCCESS)
    return L7_FALSE;

  return L7_TRUE;
}

/**
 * Creates an DHCP instance
 * 
 * @param evc_idx : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_instance_add(L7_uint32 evc_idx)
{
  L7_uint   dhcp_idx;
  L7_uint8  evc_type;
  L7_uint32 evc_flags;
  L7_RC_t   rc;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid eEVC id: ucEvcId=%u",evc_idx);
    return L7_FAILURE;
  }

  /* These evcs must be active */
  if (!ptin_evc_is_in_use(evc_idx))
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"eEVC id is not active: ucEvcId%u",evc_idx);
    return L7_FAILURE;
  }

  /* Get EVC type */
  if (ptin_evc_check_evctype(evc_idx, &evc_type) != L7_SUCCESS ||
      ptin_evc_flags_get(evc_idx, &evc_flags, L7_NULLPTR) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error getting eEVC %u type and flags", evc_idx);
    return L7_FAILURE;
  }

  /* Check if there is an instance with these parameters */
  if (ptin_dhcp_instance_find(evc_idx, &dhcp_idx) != L7_SUCCESS)
  {
    /* Check flags */
    if (!(evc_flags & PTIN_EVC_MASK_DHCPV4_PROTOCOL) &&
        !(evc_flags & PTIN_EVC_MASK_DHCPV6_PROTOCOL))
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"DHCP flag is not present for eEVC %u", evc_idx);
      return L7_FAILURE;
    }

    /* Find an empty instance to be used */
    if (ptin_dhcp_instance_find_free(&dhcp_idx)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"There is no free instances to be used");
      return L7_FAILURE;
    }

    /* Save direct referencing to dhcp index from evc ids */
    if (ptin_evc_dhcpInst_set(evc_idx, dhcp_idx) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error setting DHCP instance to ext evc id %u", evc_idx);
      return L7_FAILURE;
    }

    /* Save data in free instance */
    dhcpInstances[dhcp_idx].evc_idx  = evc_idx;
    dhcpInstances[dhcp_idx].nni_ovid = 0;
    dhcpInstances[dhcp_idx].n_evcs   = 1;
    dhcpInstances[dhcp_idx].inUse    = L7_TRUE;
  }
  else
  {
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"There is already an instance with eEvcId%u", evc_idx);
  }

  /* Configure trap rule for this instance */
  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  if (evc_type != PTIN_EVC_TYPE_QUATTRO_STACKED)
  #endif
  {
    /* Guarantee trap rules are deactivated */
    (void) ptin_dhcp_trap_configure(dhcp_idx, L7_DISABLE, L7_AF_INET);
    (void) ptin_dhcp_trap_configure(dhcp_idx, L7_DISABLE, L7_AF_INET6);

    rc = L7_NO_VALUE;
    if ((rc == L7_NO_VALUE || rc == L7_SUCCESS) &&
        (evc_flags & PTIN_EVC_MASK_DHCPV4_PROTOCOL))
    {
      rc = ptin_dhcp_trap_configure(dhcp_idx, L7_ENABLE, L7_AF_INET);
    }
    if ((rc == L7_NO_VALUE || rc == L7_SUCCESS) &&
        (evc_flags & PTIN_EVC_MASK_DHCPV6_PROTOCOL))
    {
      rc = ptin_dhcp_trap_configure(dhcp_idx, L7_ENABLE, L7_AF_INET6);
    }

    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error configuring DHCP snooping for dhcp_idx=%u",dhcp_idx);
      ptin_dhcp_trap_configure(dhcp_idx, L7_DISABLE, L7_AF_INET6);
      ptin_dhcp_trap_configure(dhcp_idx, L7_DISABLE, L7_AF_INET);
      memset(&dhcpInstances[dhcp_idx], 0x00, sizeof(st_DhcpInstCfg_t));
      ptin_evc_dhcpInst_set(evc_idx, DHCP_INVALID_ENTRY);
      return L7_FAILURE;
    }
  }

  /* DHCP index in use */

  return L7_SUCCESS;
}

/**
 * Removes an DHCP instance
 * 
 * @param evc_idx : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_instance_remove(L7_uint32 evc_idx)
{
  L7_uint dhcp_idx;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid eEVC ids: ucEvcId=%u",evc_idx);
    return L7_FAILURE;
  }

  /* Check if there is an instance with these parameters */
  if (ptin_dhcp_instance_find(evc_idx, &dhcp_idx)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"There is no instance with ucEvcId=%u",evc_idx);
    return L7_SUCCESS;
  }

  /* Remove all clients attached to this instance */
  if (ptin_dhcp_instance_deleteAll_clients(dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error removing all clients from dhcp_idx %u (evc_idx=%u)",dhcp_idx,evc_idx);
    return L7_FAILURE;
  }

  /* Configure packet trapping for this instance */
  (void) ptin_dhcp_trap_configure(dhcp_idx, L7_DISABLE, L7_AF_INET);
  (void) ptin_dhcp_trap_configure(dhcp_idx, L7_DISABLE, L7_AF_INET6);

  /* Save direct referencing to dhcp index from evc ids */
  if (ptin_evc_dhcpInst_set(evc_idx, DHCP_INVALID_ENTRY) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error resetting DHCP instance to ext evc id %u", evc_idx);
    return L7_FAILURE;
  }

  /* Clear data and free instance */
  dhcpInstances[dhcp_idx].evc_idx  = 0;
  dhcpInstances[dhcp_idx].nni_ovid = 0;
  dhcpInstances[dhcp_idx].n_evcs   = 0;
  dhcpInstances[dhcp_idx].inUse    = L7_FALSE;

  return L7_SUCCESS;
}

/**
 * Update DHCP entries, when EVCs are deleted
 * 
 * @param evc_idx : evc index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_instance_destroy(L7_uint32 evc_idx)
{
  return ptin_dhcp_instance_remove(evc_idx);
}


/**
 * Return number of QUATTRO instances
 * 
 * @return L7_uint : number
 */
//static L7_uint ptin_dhcp_get_quattro_instances(void);

/**
 * Associate an EVC to a DHCP instance
 * 
 * @param evc_idx : Unicast evc id 
 * @param nni_ovlan  : NNI outer vlan 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_evc_add(L7_uint32 evc_idx, L7_uint16 nni_ovlan)
{
  L7_uint   dhcp_idx;
  L7_uint8  evc_type;
  L7_BOOL   new_evc = L7_FALSE, new_instance = L7_FALSE;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid eEVC id: ucEvcId=%u",evc_idx);
    return L7_FAILURE;
  }

  /* These evcs must be active */
  if (!ptin_evc_is_in_use(evc_idx))
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"eEVC id is not active: ucEvcId%u",evc_idx);
    return L7_FAILURE;
  }

  /* Get EVC type */
  if (ptin_evc_check_evctype(evc_idx, &evc_type) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error getting eEVC %u type", evc_idx);
    return L7_FAILURE;
  }

  #if 0
  /* If EVC is not QUATTRO pointo-to-point, use tradittional isnatnce management */
  if (evc_type != PTIN_EVC_TYPE_QUATTRO_STACKED)
  {
    nni_ovlan = 0;
  }
  #endif

  /* Check if there is an instance with these parameters */
  if (ptin_dhcp_instance_find(evc_idx, &dhcp_idx) != L7_SUCCESS)
  {
    new_evc = L7_TRUE;

    /* Check if there is an instance with the same NNI outer vlan: use it! */
    /* Otherwise, create a new instance */
    if ((nni_ovlan < PTIN_VLAN_MIN || nni_ovlan > PTIN_VLAN_MAX) ||
        ptin_dhcp_instance_find_agg(nni_ovlan, &dhcp_idx) != L7_SUCCESS)
    {
      /* Find an empty instance to be used */
      if (ptin_dhcp_instance_find_free(&dhcp_idx) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_DHCP,"There is no free instances to be used");
        return L7_FAILURE;
      }
      else
      {
        new_instance = L7_TRUE;
      }
    }

    /* Save direct referencing to dhcp index from evc ids */
    if (ptin_evc_dhcpInst_set(evc_idx, dhcp_idx) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error setting DHCP instance to ext evc id %u", evc_idx);
      return L7_FAILURE;
    }

    /* Save data in free instance */
    if (new_instance)
    {
      dhcpInstances[dhcp_idx].evc_idx  = evc_idx;
      dhcpInstances[dhcp_idx].nni_ovid = (nni_ovlan>=PTIN_VLAN_MIN && nni_ovlan<=PTIN_VLAN_MAX) ? nni_ovlan : 0;
      dhcpInstances[dhcp_idx].n_evcs   = 0;
      dhcpInstances[dhcp_idx].inUse    = L7_TRUE;

      /* Clear configurations and statistics */
      dhcpInstances[dhcp_idx].dhcpFlags       = DHCP_BOOTP_FLAG_NONE;
      dhcpInstances[dhcp_idx].evcDhcpOptions  = 0;
      memset(&dhcpInstances[dhcp_idx].circuitid, 0x00, sizeof(dhcpInstances[dhcp_idx].circuitid));
      memset(dhcpInstances[dhcp_idx].stats_intf, 0x00, sizeof(dhcpInstances[dhcp_idx].stats_intf));
    }
  }
  else
  {
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"There is already an instance with eEvcId %u",evc_idx);
  }

  /* Use ptin_dhcp_evc_trap_configure, to set traps */
  #if 0
  /* Configure trap rule for this instance */
  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  if (evc_type != PTIN_EVC_TYPE_QUATTRO_STACKED || dhcp_quattro_stacked_evcs == 0)
  #endif
  {
    rc = ptin_dhcp_evc_trap_configure(evc_idx, L7_ENABLE, family);

    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error configuring DHCP snooping for dhcp_idx=%u",dhcp_idx);
      ptin_dhcp_evc_trap_configure(evc_idx, L7_DISABLE, L7_AF_INET6);
      ptin_dhcp_evc_trap_configure(evc_idx, L7_DISABLE, L7_AF_INET);
      memset(&dhcpInstances[dhcp_idx], 0x00, sizeof(st_DhcpInstCfg_t));
      ptin_evc_dhcpInst_set(evc_idx, DHCP_INVALID_ENTRY);
      return L7_FAILURE;
    }
  }
  #endif

  /* Only increment number of EVCs, if a new EVC was added */
  if (new_evc)
  {
    /* One more EVC associated to this instance */
    dhcpInstances[dhcp_idx].n_evcs++;

    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    /* Update number of QUATTRO-P2P evcs */
    if (evc_type == PTIN_EVC_TYPE_QUATTRO_STACKED)
    {
      dhcp_quattro_stacked_evcs++;
    }
    #endif
  }

  return L7_SUCCESS;
}

/**
 * Deassociate an EVC from a DHCP instance
 * 
 * @param evc_idx : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_evc_remove(L7_uint32 evc_idx)
{
  L7_uint   dhcp_idx;
  L7_uint8  evc_type;
  L7_uint16 nni_ovid;
  L7_BOOL remove_instance = L7_TRUE;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid eEVC ids: ucEvcId=%u",evc_idx);
    return L7_FAILURE;
  }

  /* Check if there is an instance with these parameters */
  if (ptin_dhcp_instance_find(evc_idx,&dhcp_idx)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"There is no instance with ucEvcId=%u",evc_idx);
    return L7_SUCCESS;
  }

  /* Get EVC type */
  if (ptin_evc_check_evctype(evc_idx, &evc_type) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error getting eEVC %u type", evc_idx);
    return L7_FAILURE;
  }

  /* Remove instance? */
  remove_instance = ((dhcpInstances[dhcp_idx].nni_ovid==0 || dhcpInstances[dhcp_idx].nni_ovid>4095) ||
                     (dhcpInstances[dhcp_idx].n_evcs <= 1));

  /* NNI outer vlan */
  nni_ovid = dhcpInstances[dhcp_idx].nni_ovid;

  /* Deconfigure packet trapping for this instance */
  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  if (evc_type != PTIN_EVC_TYPE_QUATTRO_STACKED || dhcp_quattro_stacked_evcs <= 1)
  #endif
  {
    (void) ptin_dhcp_evc_trap_configure(evc_idx, L7_DISABLE, L7_AF_INET); 
    (void) ptin_dhcp_evc_trap_configure(evc_idx, L7_DISABLE, L7_AF_INET6);
  }

  /* Remove clients */
  if (remove_instance)
  {
    /* Remove all clients attached to this instance */
    if (ptin_dhcp_instance_deleteAll_clients(dhcp_idx)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error removing all clients from dhcp_idx %u (evc_idx=%u)",dhcp_idx,evc_idx);
      return L7_FAILURE;
    }
  }

  /* Remove EVC index referencing */
  if (ptin_evc_dhcpInst_set(evc_idx, DHCP_INVALID_ENTRY) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error resetting DHCP instance to ext evc id %u", evc_idx);
    return L7_FAILURE;
  }

  /* Only clear instance, if there is no one using this NNI outer vlan */
  if (remove_instance)
  {
    /* Clear configurations and statistics */
    dhcpInstances[dhcp_idx].dhcpFlags       = DHCP_BOOTP_FLAG_NONE;
    dhcpInstances[dhcp_idx].evcDhcpOptions  = 0;
    memset(&dhcpInstances[dhcp_idx].circuitid, 0x00, sizeof(dhcpInstances[dhcp_idx].circuitid));
    memset(dhcpInstances[dhcp_idx].stats_intf, 0x00, sizeof(dhcpInstances[dhcp_idx].stats_intf));

    /* Clear data and free instance */
    dhcpInstances[dhcp_idx].evc_idx   = 0;
    dhcpInstances[dhcp_idx].nni_ovid  = 0;
    dhcpInstances[dhcp_idx].n_evcs    = 0;
    dhcpInstances[dhcp_idx].inUse     = L7_FALSE;
  }

  /* One less EVC */
  if (dhcpInstances[dhcp_idx].n_evcs > 0)
    dhcpInstances[dhcp_idx].n_evcs--;

  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  /* Update number of QUATTRO-P2P evcs */
  if (evc_type == PTIN_EVC_TYPE_QUATTRO_STACKED)
  {
    if (dhcp_quattro_stacked_evcs>0)  dhcp_quattro_stacked_evcs--;
  }
  #endif

  return L7_SUCCESS;
}

/**
 * Update DHCP entries, when EVCs are deleted
 * 
 * @param evc_idx : evc index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_evc_destroy(L7_uint32 evc_idx)
{
  return ptin_dhcp_evc_remove(evc_idx);
}


/**
 * Reconfigure global DHCP EVC
 *
 * @param evc_idx         : evc index
 * @param dhcp_flag     : DHCP flag (not used)
 * @param options       : options
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_reconf_evc(L7_uint32 evc_idx, L7_uint8 dhcp_flag, L7_uint32 options)
{
   L7_uint dhcp_idx;

   /* Get DHCP instance index */
   if (ptin_dhcp_instance_find(evc_idx, &dhcp_idx) != L7_SUCCESS)
   {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "There is no DHCP instance with EVC id %u", evc_idx);
    return L7_NOT_EXIST;
   }

   return ptin_dhcp_reconf_instance(dhcp_idx, dhcp_flag, options);
}

/**
 * Reconfigure global DHCP EVC (using root vlan)
 *
 * @param rootVid   : root vlan
 * @param dhcp_flag : DHCP flag (not used)
 * @param options   : options
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_reconf_rootVid(L7_uint16 rootVid, L7_uint8 dhcp_flag, L7_uint32 options)
{
  L7_uint dhcp_idx;

  /* Get DHCP instance index */
  if (ptin_dhcp_instance_find_agg(rootVid, &dhcp_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "There is no DHCP instance for root Vid %u", rootVid);
    return L7_NOT_EXIST;
  }

  return ptin_dhcp_reconf_instance(dhcp_idx, dhcp_flag, options);
}

/**
 * Get DHCP circuit-id global data
 *
 * @param evc_idx           : evc index
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
L7_RC_t ptin_dhcp_circuitid_get(L7_uint32 evc_idx, L7_char8 *template_str, L7_uint32 *mask, L7_char8 *access_node_id, L7_uint8 *chassis,
                                L7_uint8 *rack, L7_uint8 *frame, L7_uint8 *ethernet_priority, L7_uint16 *s_vid)
{
  L7_uint dhcp_idx;

  /* Validate arguments */
  if (template_str == L7_NULLPTR || access_node_id == L7_NULLPTR || mask == 0x00)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Get DHCP instance index */
  if (ptin_dhcp_instance_find(evc_idx, &dhcp_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "There is no DHCP instance with EVC id %u", evc_idx);
    return L7_FAILURE;
  }

  /* Validate dhcp instance */
  if (!dhcpInstances[dhcp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP instance %u is not in use", dhcp_idx);
    return L7_FAILURE;
  }

  strncpy(template_str,   dhcpInstances[dhcp_idx].circuitid.template_str,   CIRCUITID_TEMPLATE_MAX_STRING);
  template_str[CIRCUITID_TEMPLATE_MAX_STRING-1] = '\0';
  strncpy(access_node_id, dhcpInstances[dhcp_idx].circuitid.access_node_id, FD_DS_MAX_REMOTE_ID_STRING   );
  access_node_id[FD_DS_MAX_REMOTE_ID_STRING -1] = '\0';
  *mask              = dhcpInstances[dhcp_idx].circuitid.mask;
  *chassis           = dhcpInstances[dhcp_idx].circuitid.chassis;
  *rack              = dhcpInstances[dhcp_idx].circuitid.rack;
  *frame             = dhcpInstances[dhcp_idx].circuitid.frame;
  *ethernet_priority = dhcpInstances[dhcp_idx].circuitid.ethernet_priority;
  *s_vid             = dhcpInstances[dhcp_idx].circuitid.s_vid;

  return L7_SUCCESS;
}

/**
 * Set DHCP circuit-id global data from EVC id
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
L7_RC_t ptin_dhcp_circuitid_set_evc(L7_uint32 evc_idx, L7_char8 *template_str, L7_uint32 mask, L7_char8 *access_node_id, L7_uint8 chassis,
                                    L7_uint8 rack, L7_uint8 frame, L7_uint8 ethernet_priority, L7_uint16 s_vid)
{
  L7_uint dhcp_idx;

  /* Get DHCP instance index */
  if (ptin_dhcp_instance_find(evc_idx, &dhcp_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "There is no DHCP instance with EVC id %u", evc_idx);
    return L7_NOT_EXIST;
  }

  return ptin_dhcp_circuitid_set_instance(dhcp_idx, template_str, mask, access_node_id, chassis, rack, frame, ethernet_priority, s_vid);
}

/**
 * Set DHCP circuit-id global data from NNI SVlan
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
L7_RC_t ptin_dhcp_circuitid_set_nniVid(L7_uint16 nni_outerVid, L7_char8 *template_str, L7_uint32 mask, L7_char8 *access_node_id, L7_uint8 chassis,
                                       L7_uint8 rack, L7_uint8 frame, L7_uint8 ethernet_priority, L7_uint16 s_vid)
{
  L7_uint dhcp_idx;

  /* Get DHCP instance index */
  if (ptin_dhcp_instance_find_agg(nni_outerVid, &dhcp_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "There is no DHCP instance for NNI Vid %u", nni_outerVid);
    return L7_NOT_EXIST;
  }

  return ptin_dhcp_circuitid_set_instance(dhcp_idx, template_str, mask, access_node_id, chassis, rack, frame, ethernet_priority, s_vid);
}

/**
 * Set DHCP circuit-id global data
 *
 * @param dhcp_idx        : instance index
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
static L7_RC_t ptin_dhcp_circuitid_set_instance(L7_uint16 dhcp_idx, L7_char8 *template_str, L7_uint32 mask, L7_char8 *access_node_id, L7_uint8 chassis,
                                                L7_uint8 rack, L7_uint8 frame, L7_uint8 ethernet_priority, L7_uint16 s_vid)
{
  ptinDhcpClientDataKey_t avl_key;
  ptinDhcpClientInfoData_t *avl_info;

  /* Validate arguments */
  if (template_str == L7_NULLPTR || access_node_id == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }
  /* Validate string lengths */
  if ( strnlen(template_str, CIRCUITID_TEMPLATE_MAX_STRING) >= CIRCUITID_TEMPLATE_MAX_STRING )
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Template string length is invalid!");
    return L7_FAILURE;
  }
  if ( strnlen(access_node_id, FD_DS_MAX_REMOTE_ID_STRING) >= FD_DS_MAX_REMOTE_ID_STRING )
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Access node identifier length is invalid!");
    return L7_FAILURE;
  }

  /* Validate DHCP instance index */
  if (dhcp_idx >= PTIN_SYSTEM_N_DHCP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Invalid DHCP instance %u", dhcp_idx);
    return L7_FAILURE;
  }

  /* Validate dhcp instance */
  if (!dhcpInstances[dhcp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP instance %u is not in use", dhcp_idx);
    return L7_FAILURE;
  }

  strncpy(dhcpInstances[dhcp_idx].circuitid.template_str, template_str, CIRCUITID_TEMPLATE_MAX_STRING);
  dhcpInstances[dhcp_idx].circuitid.template_str[CIRCUITID_TEMPLATE_MAX_STRING-1] = '\0';      /* Just to be sure */

  dhcpInstances[dhcp_idx].circuitid.mask                = mask;

  strncpy(dhcpInstances[dhcp_idx].circuitid.access_node_id, access_node_id, FD_DS_MAX_REMOTE_ID_STRING);
  dhcpInstances[dhcp_idx].circuitid.access_node_id[FD_DS_MAX_REMOTE_ID_STRING-1] = '\0';       /* Just to be sure */

  dhcpInstances[dhcp_idx].circuitid.chassis             = chassis;
  dhcpInstances[dhcp_idx].circuitid.rack                = rack;
  dhcpInstances[dhcp_idx].circuitid.frame               = frame;
  dhcpInstances[dhcp_idx].circuitid.ethernet_priority   = 0x7 & ethernet_priority;
  dhcpInstances[dhcp_idx].circuitid.s_vid               = s_vid;

  /* TODO: Run all clients of this DHCP instance, and rebuild their circuit id strings */

  /* Run all cells in AVL tree */
  memset(&avl_key, 0x00, sizeof(ptinDhcpClientDataKey_t));
  while ( ( avl_info = (ptinDhcpClientInfoData_t *)
                        avlSearchLVL7(&dhcpClients_unified.avlTree.dhcpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->dhcpClientDataKey, sizeof(ptinDhcpClientDataKey_t));

    /* Only apply for this dhcp instance */
    if (avl_key.dhcp_instance != dhcp_idx)
      continue;

    /* Rebuild circuit id */
    ptin_dhcp_circuitId_build( &dhcpInstances[dhcp_idx].circuitid, &avl_info->client_data.circuitId, avl_info->client_data.circuitId_str);
  }

  return L7_SUCCESS;
}

/**
 * Get DHCP flags of a particular EVC
 *
 * @param evc_idx : evc index 
 * @param dhcp_mask  : flags mask 
 * @param dhcp_flags : DHCP flags 
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_evc_flags_get(L7_uint32 evc_idx, L7_uchar8 *dhcp_mask, L7_char8 *dhcp_flags)
{
  L7_uint dhcp_idx;

  /* Get DHCP instance index */
  if (ptin_dhcp_instance_find(evc_idx, &dhcp_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "There is no DHCP instance with EVC id %u", evc_idx);
    return L7_FAILURE;
  }

  /* Validate dhcp instance */
  if (ptin_dhcp_flags_get_instance(dhcp_idx, dhcp_mask, dhcp_flags) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Error getting DHCP flags for eEVC %u / dhcp_idx=%u", evc_idx, dhcp_idx);
    return L7_FAILURE;
  }

  return L7_SUCCESS; 
}

/**
 * Set DHCP flags for a particular EVC id
 *
 * @param evc_idx  : evc index 
 * @param mask     : flags mask 
 * @param flags    : DHCP flags
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_evc_flags_set(L7_uint32 evc_idx, L7_uchar8 mask, L7_uchar8 flags)
{
  L7_uint dhcp_idx;

  /* Get DHCP instance index */
  if (ptin_dhcp_instance_find(evc_idx, &dhcp_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "There is no DHCP instance with EVC id %u", evc_idx);
    return L7_NOT_EXIST;
  }

  return ptin_dhcp_flags_set_instance(dhcp_idx, mask, flags);
}

/**
 * Set DHCP flags for a particular NNI SVlan
 *
 * @param nni_outerVid    : NNI STAG 
 * @param mask     : flags mask 
 * @param flags    : DHCP flags
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_nniVid_flags_set(L7_uint16 nni_outerVid, L7_uchar8 mask, L7_uchar8 flags)
{
  L7_uint dhcp_idx;

  /* Get DHCP instance index */
  if (ptin_dhcp_instance_find_agg(nni_outerVid, &dhcp_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "There is no DHCP instance for NNI Vid %u", nni_outerVid);
    return L7_NOT_EXIST;
  }

  return ptin_dhcp_flags_set_instance(dhcp_idx, mask, flags);
}

/**
 * Get DHCP flags
 *
 * @param dhcp_idx : instance index 
 * @param mask     : flags mask
 * @param flags    : DHCP flags
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_flags_get_instance(L7_uint16 dhcp_idx, L7_uchar8 *mask, L7_char8 *flags)
{
  /* Validate DHCP instance index */
  if (dhcp_idx >= PTIN_SYSTEM_N_DHCP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Invalid DHCP instance %u", dhcp_idx);
    return L7_FAILURE;
  }

  /* Validate dhcp instance */
  if (!dhcpInstances[dhcp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP instance %u is not in use", dhcp_idx);
    return L7_FAILURE;
  }

  /* Clear mask */
  if (mask != L7_NULLPTR)
  {
    *mask = 0;
  }

  /* Get flags */
  if (flags != L7_NULLPTR)
  {
    *flags = dhcpInstances[dhcp_idx].dhcpFlags;
  }
  if (mask != L7_NULLPTR)
  {
    *mask |= DHCP_FLAGS_MASK_FLAGS;
  }

  return L7_SUCCESS;
}

/**
 * Set DHCP flags
 *
 * @param dhcp_idx : instance index 
 * @param mask     : flags mask 
 * @param flags    : DHCP flags
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_dhcp_flags_set_instance(L7_uint16 dhcp_idx, L7_uchar8 mask, L7_char8 flags)
{
  /* Validate DHCP instance index */
  if (dhcp_idx >= PTIN_SYSTEM_N_DHCP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Invalid DHCP instance %u", dhcp_idx);
    return L7_FAILURE;
  }

  /* Validate dhcp instance */
  if (!dhcpInstances[dhcp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP instance %u is not in use", dhcp_idx);
    return L7_FAILURE;
  }

  /* Update flags */
  if (mask & DHCP_FLAGS_MASK_FLAGS)
  {
    dhcpInstances[dhcp_idx].dhcpFlags = flags;
  }

  return L7_SUCCESS;
}

/**
 * Get DHCP client data (circuit and remote ids)
 * 
 * @param evc_idx        : Unicast evc id
 * @param client            : client identification parameters
 * @param options           : DHCP options
 * @param circuitId_data    : Circuit ID data 
 * @param circuitId         : Circuit ID string
 * @param remoteId          : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_client_get(L7_uint32 evc_idx, ptin_client_id_t *client, L7_uint16 *options,
                             ptin_clientCircuitId_t *circuitId_data, L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_uint dhcp_idx;
  ptinDhcpClientInfoData_t *client_info;

  /* Validate arguments */
  if (client==L7_NULLPTR || DHCP_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Get DHCP instance index */
  if (ptin_dhcp_instance_find(evc_idx, &dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"There is no DHCP instance with EVC id %u",evc_idx);
    return L7_FAILURE;
  }

  /* Find client information */
  if (ptin_dhcp_client_find(dhcp_idx,client,&client_info)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Non existent client in DHCP instance %u (EVC id %u)",dhcp_idx,evc_idx);
    return L7_FAILURE;
  }

  /* Return string ids */
  if (options!=L7_NULLPTR)
  {
     *options = client_info->client_data.dhcp_options;
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
    LOG_TRACE(LOG_CTX_PTIN_DHCP, "%s", circuitId);
  }
  if (remoteId!=L7_NULLPTR)
  {
    strncpy(remoteId ,client_info->client_data.remoteId_str ,FD_DS_MAX_REMOTE_ID_STRING);
    remoteId[FD_DS_MAX_REMOTE_ID_STRING-1] = '\0';
    LOG_TRACE(LOG_CTX_PTIN_DHCP, "%s", remoteId);
  }

  return L7_SUCCESS;
}

/**
 * Add a new DHCP client
 * 
 * @param evc_idx    : Unicast evc id
 * @param client     : client identification parameters 
 * @param uni_ovid   : External outer vlan 
 * @param uni_ivid   : External inner vlan  
 * @param options    : DHCP options
 * @param circuitId  : Circuit ID data 
 * @param remoteId   : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_client_add(L7_uint32 evc_idx, const ptin_client_id_t *client_id, L7_uint16 uni_ovid, L7_uint16 uni_ivid,
                             L7_uint16 options, ptin_clientCircuitId_t *circuitId, L7_char8 *remoteId)
{
  ptin_client_id_t client;
  L7_uint dhcp_idx;
  L7_int  client_idx = -1;
  ptinDhcpClientDataKey_t avl_key;
  ptinDhcpClientsAvlTree_t *avl_tree;
  ptinDhcpClientInfoData_t *avl_infoData;
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port, intIfNum;
  ptin_evc_intfCfg_t intfCfg;
  #endif

  /* Validate arguments */
  if (client_id == L7_NULLPTR || DHCP_CLIENT_MASK_UPDATE(client_id->mask) == 0x00)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Get DHCP instance index */
  if (ptin_dhcp_instance_find(evc_idx, &dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"There is no DHCP instance with EVC id %u",evc_idx);
    return L7_FAILURE;
  }

  memcpy(&client, client_id, sizeof(ptin_client_id_t));

  /* Validate and rearrange clientId info */
  if (ptin_dhcp_clientId_convert(evc_idx, &client) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid client id");
    return L7_FAILURE;
  }

  /* Get ptin_port value */
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  ptin_port = 0;
  if (client.mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    /* Get interface configuration in the UC EVC */
    if (ptin_evc_intfCfg_get(evc_idx, &client.ptin_intf, &intfCfg)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error acquiring intf configuration for intf %u/%u, evc=%u",client.ptin_intf.intf_type,client.ptin_intf.intf_id,evc_idx);
      return L7_FAILURE;
    }
    /* Validate interface configuration in EVC: must be in use, and be a leaf/client */
    if (!intfCfg.in_use || intfCfg.type!=PTIN_EVC_INTF_LEAF)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"intf %u/%u is not in use or is not a leaf in evc %u",client.ptin_intf.intf_type,client.ptin_intf.intf_id,evc_idx);
      return L7_FAILURE;
    }
    /* Convert to ptin_port format */
    if (ptin_intf_ptintf2port(&client.ptin_intf,&ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Cannot convert client intf %u/%u to ptin_port format",client.ptin_intf.intf_type,client.ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
  #endif

  /* If uni vlans are not provided, but interface is, get uni vlans from EVC data */
  if ( (uni_ovid<PTIN_VLAN_MIN || uni_ovid>PTIN_VLAN_MAX) &&
       (client.mask & PTIN_CLIENT_MASK_FIELD_INTF) &&
       (client.mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) )
  {
     /* Get interface as intIfNum format */
    if (ptin_intf_ptintf2intIfNum(&client.ptin_intf, &intIfNum)==L7_SUCCESS)
    {
      if (ptin_evc_extVlans_get(intIfNum, evc_idx,(L7_uint32)-1, client.innerVlan, &uni_ovid, &uni_ivid) == L7_SUCCESS)
      {
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"Ext vlans for ptin_intf %u/%u, cvlan %u: uni_ovid=%u, uni_ivid=%u",
                  client.ptin_intf.intf_type, client.ptin_intf.intf_id, client.innerVlan, uni_ovid, uni_ivid);
      }
      else
      {
        uni_ovid = uni_ivid = 0;
        LOG_ERR(LOG_CTX_PTIN_DHCP,"Cannot get ext vlans for ptin_intf %u/%u, cvlan %u",
                client.ptin_intf.intf_type, client.ptin_intf.intf_id, client.innerVlan);
      }
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid ptin_intf %u/%u", client.ptin_intf.intf_type, client.ptin_intf.intf_id);
    }
  }

  /* Check if this key already exists */
  avl_tree = &dhcpClients_unified.avlTree;

  memset(&avl_key,0x00,sizeof(ptinDhcpClientDataKey_t));

  avl_key.dhcp_instance = dhcp_idx;
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  avl_key.ptin_port = ptin_port;
  #endif
  #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
  avl_key.outerVlan = (client.mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN) ? client.outerVlan : 0;
  #endif
  #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
  avl_key.innerVlan = (client.mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) ? client.innerVlan : 0;
  #endif
  #if (DHCP_CLIENT_IPADDR_SUPPORTED)
  avl_key.ipv4_addr = (client.mask & PTIN_CLIENT_MASK_FIELD_IPADDR   ) ? client.ipv4_addr : 0;
  #endif
  #if (DHCP_CLIENT_MACADDR_SUPPORTED)
  if (client.mask & PTIN_CLIENT_MASK_FIELD_MACADDR)
    memcpy(avl_key.macAddr,client.macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  else
    memset(avl_key.macAddr,0x00,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  #endif

  #if (DHCP_CLIENT_DEBUG)
  LOG_TRACE(LOG_CTX_PTIN_DHCP,"Key {"
            #if (DHCP_CLIENT_INTERF_SUPPORTED)
                              "port=%u,"
            #endif
            #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                              "svlan=%u,"
            #endif
            #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                              "cvlan=%u,"
            #endif
            #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                              "ipAddr=%u.%u.%u.%u,"
            #endif
            #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
            #endif
                              "} will be added to dhcp_idx=%u",
            #if (DHCP_CLIENT_INTERF_SUPPORTED)
            avl_key.ptin_port,
            #endif
            #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
            avl_key.outerVlan,
            #endif
            #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
            avl_key.innerVlan,
            #endif
            #if (DHCP_CLIENT_IPADDR_SUPPORTED)
            (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
            #endif
            #if (DHCP_CLIENT_MACADDR_SUPPORTED)
            avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
            #endif
            dhcp_idx);
  #endif

  /* Check if this key already exists */
  if ((avl_infoData=(ptinDhcpClientInfoData_t *) avlSearchLVL7( &(avl_tree->dhcpClientsAvlTree), (void *)&avl_key, AVL_EXACT))!=L7_NULLPTR)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_DHCP,"This key {"
                #if (DHCP_CLIENT_INTERF_SUPPORTED)
                                  "port=%u,"
                #endif
                #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                                  "svlan=%u,"
                #endif
                #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                                  "cvlan=%u,"
                #endif
                #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                                  "ipAddr=%u.%u.%u.%u,"
                #endif
                #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                                  "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
                #endif
                                  "} already exists in dhcp_idx=%u",
                #if (DHCP_CLIENT_INTERF_SUPPORTED)
                avl_key.ptin_port,
                #endif
                #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                avl_key.outerVlan,
                #endif
                #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                avl_key.innerVlan,
                #endif
                #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
                #endif
                #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
                #endif
                dhcp_idx);
  }
  /* New client */
  else
  {
    /* Check if there is free clients */
    if ( !dhcp_clientIndex_check_free(dhcp_idx) )
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"There is no more free clients to be allocated for dhcp_idx=%u (evc=%u)", dhcp_idx, evc_idx);
      return L7_FAILURE;
    }

    /* Insert entry in AVL tree */
    if (avlInsertEntry(&(avl_tree->dhcpClientsAvlTree), (void *)&avl_key)!=L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error inserting key {"
              #if (DHCP_CLIENT_INTERF_SUPPORTED)
                                "port=%u,"
              #endif
              #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                                "svlan=%u,"
              #endif
              #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                                "cvlan=%u,"
              #endif
              #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                                "ipAddr=%u.%u.%u.%u,"
              #endif
              #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                                "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
              #endif
                                "} in dhcp_idx=%u",
              #if (DHCP_CLIENT_INTERF_SUPPORTED)
              avl_key.ptin_port,
              #endif
              #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
              avl_key.outerVlan,
              #endif
              #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
              avl_key.innerVlan,
              #endif
              #if (DHCP_CLIENT_IPADDR_SUPPORTED)
              (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
              #endif
              #if (DHCP_CLIENT_MACADDR_SUPPORTED)
              avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
              #endif
              dhcp_idx);
      return L7_FAILURE;
    }

    /* Find the inserted entry */
    if ((avl_infoData=(ptinDhcpClientInfoData_t *) avlSearchLVL7(&(avl_tree->dhcpClientsAvlTree),(void *)&avl_key, AVL_EXACT))==L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Cannot find key {"
              #if (DHCP_CLIENT_INTERF_SUPPORTED)
                                "port=%u,"
              #endif
              #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                                "svlan=%u,"
              #endif
              #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                                "cvlan=%u,"
              #endif
              #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                                "ipAddr=%u.%u.%u.%u,"
              #endif
              #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                                "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
              #endif
                                "} in dhcp_idx=%u",
              #if (DHCP_CLIENT_INTERF_SUPPORTED)
              avl_key.ptin_port,
              #endif
              #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
              avl_key.outerVlan,
              #endif
              #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
              avl_key.innerVlan,
              #endif
              #if (DHCP_CLIENT_IPADDR_SUPPORTED)
              (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
              #endif
              #if (DHCP_CLIENT_MACADDR_SUPPORTED)
              avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
              #endif
              dhcp_idx);
      return L7_FAILURE;
    }

    /* Allocate new client index */
    client_idx = dhcp_clientIndex_allocate(dhcp_idx, avl_infoData);

    if (client_idx < 0 || client_idx >= PTIN_SYSTEM_DHCP_MAXCLIENTS)
    {
      avlDeleteEntry(&(avl_tree->dhcpClientsAvlTree), (void *)&avl_key);
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error obtaining new client index for dhcp_idx=%u (evc=%u)",dhcp_idx, evc_idx);
      return L7_FAILURE;
    }

    /* Client index */
    avl_infoData->client_index = client_idx;

    /* Save UNI vlans (external vlans used for transmission) */
    avl_infoData->uni_ovid = uni_ovid;
    avl_infoData->uni_ivid = uni_ivid;

    /* Clear circuit and remote id strings */
    memset(&avl_infoData->client_data,0x00,sizeof(ptinDhcpData_t));

    /* Clear dhcp statistics */
    osapiSemaTake(ptin_dhcp_stats_sem,-1);
    memset(&avl_infoData->client_stats,0x00,sizeof(ptin_DHCP_Statistics_t));
    osapiSemaGive(ptin_dhcp_stats_sem);
  }

  /* Fill DHCP options, circuit and remote id fields */
  if( ((options&0x02) >> 1) == 1 ) // Check if this client is using the EVC options
  {
     avl_infoData->client_data.useEvcDhcpOptions   = L7_TRUE;
     avl_infoData->client_data.dhcp_options        = dhcpInstances[dhcp_idx].evcDhcpOptions;
  }
  else
  {
     avl_infoData->client_data.useEvcDhcpOptions   = L7_FALSE;
     avl_infoData->client_data.dhcp_options        = 0;
     avl_infoData->client_data.dhcp_options        |= (options & 0x0001);
     avl_infoData->client_data.dhcp_options        |= (options & 0x0004) >> 1;
     avl_infoData->client_data.dhcp_options        |= (options & 0x0010) >> 2;
  }
  avl_infoData->client_data.circuitId.onuid  = circuitId->onuid;
  avl_infoData->client_data.circuitId.slot   = circuitId->slot;
  avl_infoData->client_data.circuitId.port   = circuitId->port + 1;
  avl_infoData->client_data.circuitId.q_vid  = circuitId->q_vid;
  avl_infoData->client_data.circuitId.c_vid  = circuitId->c_vid;

  /* Build circuit id for this client */
  avl_infoData->client_data.circuitId_str[0] = '\0';
  ptin_dhcp_circuitId_build( &dhcpInstances[dhcp_idx].circuitid, &avl_infoData->client_data.circuitId, avl_infoData->client_data.circuitId_str );

  strncpy(avl_infoData->client_data.remoteId_str ,remoteId ,FD_DS_MAX_REMOTE_ID_STRING);
  avl_infoData->client_data.remoteId_str[FD_DS_MAX_REMOTE_ID_STRING-1] = '\0';

  LOG_TRACE(LOG_CTX_PTIN_DHCP,"Success inserting Key {"
            #if (DHCP_CLIENT_INTERF_SUPPORTED)
                              "port=%u,"
            #endif
            #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                              "svlan=%u,"
            #endif
            #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                              "cvlan=%u,"
            #endif
            #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                              "ipAddr=%u.%u.%u.%u,"
            #endif
            #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
            #endif
                              "} in dhcp_idx=%u",
            #if (DHCP_CLIENT_INTERF_SUPPORTED)
            avl_key.ptin_port,
            #endif
            #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
            avl_key.outerVlan,
            #endif
            #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
            avl_key.innerVlan,
            #endif
            #if (DHCP_CLIENT_IPADDR_SUPPORTED)
            (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
            #endif
            #if (DHCP_CLIENT_MACADDR_SUPPORTED)
            avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
            #endif
            dhcp_idx);

  return L7_SUCCESS;
}

/**
 * Remove a DHCP client
 * 
 * @param evc_idx  : Unicast evc id
 * @param client      : client identification parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_client_delete(L7_uint32 evc_idx, const ptin_client_id_t *client_id)
{
  ptin_client_id_t client;
  L7_uint dhcp_idx, client_idx;
  ptinDhcpClientDataKey_t avl_key;
  ptinDhcpClientsAvlTree_t *avl_tree;
  ptinDhcpClientInfoData_t *avl_infoData;
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port;
  #endif

  /* Validate arguments */
  if (client_id == L7_NULLPTR || DHCP_CLIENT_MASK_UPDATE(client_id->mask) == 0x00)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Get DHCP instance index */
  if (ptin_dhcp_instance_find(evc_idx, &dhcp_idx)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"There is no DHCP instance with EVC id %u",evc_idx);
    return L7_NOT_EXIST;
  }

  memcpy(&client, client_id, sizeof(ptin_client_id_t));

  /* Validate and rearrange clientId info */
  if (ptin_dhcp_clientId_convert(evc_idx, &client) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid client id");
    return L7_NOT_EXIST;
  }

  /* Convert interface to ptin_port format */
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  ptin_port = 0;
  if (client.mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    if (ptin_intf_ptintf2port(&client.ptin_intf,&ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Cannot convert client intf %u/%u to ptin_port format",client.ptin_intf.intf_type, client.ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
  #endif

  /* Check if this key does not exists */

  avl_tree = &dhcpClients_unified.avlTree;

  memset(&avl_key,0x00,sizeof(ptinDhcpClientDataKey_t));

  avl_key.dhcp_instance = dhcp_idx;
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  avl_key.ptin_port = ptin_port;
  #endif
  #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
  avl_key.outerVlan = (client.mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN) ? client.outerVlan : 0;
  #endif
  #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
  avl_key.innerVlan = (client.mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) ? client.innerVlan : 0;
  #endif
  #if (DHCP_CLIENT_IPADDR_SUPPORTED)
  avl_key.ipv4_addr = (client.mask & PTIN_CLIENT_MASK_FIELD_IPADDR   ) ? client.ipv4_addr : 0;
  #endif
  #if (DHCP_CLIENT_MACADDR_SUPPORTED)
  if (client.mask & PTIN_CLIENT_MASK_FIELD_MACADDR)
    memcpy(avl_key.macAddr,client.macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  else
    memset(avl_key.macAddr,0x00,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  #endif

  #if (DHCP_CLIENT_DEBUG)
  LOG_TRACE(LOG_CTX_PTIN_DHCP,"Key to search {"
            #if (DHCP_CLIENT_INTERF_SUPPORTED)
                              "port=%u,"
            #endif
            #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                              "svlan=%u,"
            #endif
            #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                              "cvlan=%u,"
            #endif
            #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                              "ipAddr=%u.%u.%u.%u,"
            #endif
            #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
            #endif
                              "} in dhcp_idx=%u",
            #if (DHCP_CLIENT_INTERF_SUPPORTED)
            avl_key.ptin_port,
            #endif
            #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
            avl_key.outerVlan,
            #endif
            #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
            avl_key.innerVlan,
            #endif
            #if (DHCP_CLIENT_IPADDR_SUPPORTED)
            (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
            #endif
            #if (DHCP_CLIENT_MACADDR_SUPPORTED)
            avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
            #endif
            dhcp_idx);
  #endif

  /* Check if this entry does not exist in AVL tree */
  if ((avl_infoData=(ptinDhcpClientInfoData_t *) avlSearchLVL7( &(avl_tree->dhcpClientsAvlTree), (void *)&avl_key, AVL_EXACT))==L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"This key {"
                #if (DHCP_CLIENT_INTERF_SUPPORTED)
                                  "port=%u,"
                #endif
                #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                                  "svlan=%u,"
                #endif
                #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                                  "cvlan=%u,"
                #endif
                #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                                  "ipAddr=%u.%u.%u.%u,"
                #endif
                #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                                  "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
                #endif
                                  "} does not exist in dhcp_idx=%u",
                #if (DHCP_CLIENT_INTERF_SUPPORTED)
                avl_key.ptin_port,
                #endif
                #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                avl_key.outerVlan,
                #endif
                #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                avl_key.innerVlan,
                #endif
                #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
                #endif
                #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
                #endif
                dhcp_idx);
    return L7_NOT_EXIST;
  }

  /* Extract client index */
  client_idx = avl_infoData->client_index;

  /* Remove entry from AVL tree */
  if (avlDeleteEntry(&(avl_tree->dhcpClientsAvlTree), (void *)&avl_key)==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error removing key {"
            #if (DHCP_CLIENT_INTERF_SUPPORTED)
                              "port=%u,"
            #endif
            #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                              "svlan=%u,"
            #endif
            #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                              "cvlan=%u,"
            #endif
            #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                              "ipAddr=%u.%u.%u.%u,"
            #endif
            #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
            #endif
                              "} from dhcp_idx=%u",
            #if (DHCP_CLIENT_INTERF_SUPPORTED)
            avl_key.ptin_port,
            #endif
            #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
            avl_key.outerVlan,
            #endif
            #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
            avl_key.innerVlan,
            #endif
            #if (DHCP_CLIENT_IPADDR_SUPPORTED)
            (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
            #endif
            #if (DHCP_CLIENT_MACADDR_SUPPORTED)
            avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
            #endif
            dhcp_idx);
    return L7_FAILURE;
  }

  /* Release client index */
  dhcp_clientIndex_release(dhcp_idx, client_idx);

  LOG_TRACE(LOG_CTX_PTIN_DHCP,"Success removing Key {"
            #if (DHCP_CLIENT_INTERF_SUPPORTED)
                              "port=%u,"
            #endif
            #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                              "svlan=%u,"
            #endif
            #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                              "cvlan=%u,"
            #endif
            #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                              "ipAddr=%u.%u.%u.%u,"
            #endif
            #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
            #endif
                              "} from dhcp_idx=%u",
            #if (DHCP_CLIENT_INTERF_SUPPORTED)
            avl_key.ptin_port,
            #endif
            #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
            avl_key.outerVlan,
            #endif
            #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
            avl_key.innerVlan,
            #endif
            #if (DHCP_CLIENT_IPADDR_SUPPORTED)
            (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
            #endif
            #if (DHCP_CLIENT_MACADDR_SUPPORTED)
            avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
            #endif
            dhcp_idx);

  /* Remove all associated DHCP leases */
  if (L7_SUCCESS != dsEvcBindingsClear(evc_idx))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to remove DHCP leases [evc_idx:%u]", evc_idx);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get DHCP Binding table
 * 
 * @param table       : Bin table
 * @param max_entries : Size of table
 * 
 * @return L7_RC_t : L7_FAILURE/L7_SUCCESS
 */
L7_RC_t ptin_dhcp82_bindtable_get(ptin_DHCP_bind_entry *table, L7_uint32 *max_entries)
{
  dhcpSnoopBinding_t  dsBinding;
  L7_uint32           index, i;
  ptin_intf_t         ptin_intf;
  L7_uint32           evc_idx;
  L7_uint32           n_max;

  n_max = (max_entries!=L7_NULLPTR && *max_entries<PLAT_MAX_FDB_MAC_ENTRIES) ? (*max_entries) : PLAT_MAX_FDB_MAC_ENTRIES;

  memset(&dsBinding,0x00,sizeof(dhcpSnoopBinding_t));
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
    memcpy(table[index].macAddr,dsBinding.key.macAddr,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
    table[index].ipAddr.s_addr  = dsBinding.ipAddr;
    table[index].remLeave       = dsBinding.remLease;
    table[index].bindingType    = dsBinding.bindingType;
    index++;
  }

  if (max_entries!=L7_NULLPTR)  *max_entries=index;

  return L7_SUCCESS;
}

/**
 * Get DHCP Binding table
 *
 * @param table       : Bin table
 * @param max_entries : Size of table
 *
 * @notes   IPv6 compatible
 *
 * @return L7_RC_t : L7_FAILURE/L7_SUCCESS
 */
L7_RC_t ptin_dhcpv4v6_bindtable_get(ptin_DHCPv4v6_bind_entry *table, L7_uint32 *max_entries)
{
  dhcpSnoopBinding_t  dsBinding;
  L7_uint32           index, i;
  ptin_intf_t         ptin_intf;
  L7_uint32           evc_idx;
  L7_uint32           n_max;

  n_max = (max_entries!=L7_NULLPTR && *max_entries<PLAT_MAX_FDB_MAC_ENTRIES) ? (*max_entries) : PLAT_MAX_FDB_MAC_ENTRIES;

  memset(&dsBinding,0x00,sizeof(dhcpSnoopBinding_t));
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
    memcpy(table[index].macAddr,dsBinding.key.macAddr,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
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
 * Removes an entry from the DHCP binding table
 * 
 * @param ptr : DHCP bind table entry
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp82_bindtable_remove(dhcpSnoopBinding_t *dsBinding)
{
  L7_enetMacAddr_t   macAddr;

  // Find This entry
  if (usmDbDsBindingGet(dsBinding)!=L7_SUCCESS) {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "This entry does not exist");
    return L7_FAILURE;
  }

  // Remove this entry
  memcpy(macAddr.addr,dsBinding->key.macAddr,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  if (usmDbDsBindingRemove(&macAddr)!=L7_SUCCESS) {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Error removing entry");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Removes all DHCP leases belonging to the given EVC
 * 
 * @param evc_ext_id : External Service ID
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_bindtable_service_remove(L7_uint32 evc_ext_id)
{
  LOG_DEBUG(LOG_CTX_PTIN_DHCP, "Removing all DHCP leases associated with external service ID %u", evc_ext_id);

  return L7_SUCCESS;
}


/**
 * Get global DHCP statistics
 * 
 * @param intIfNum    : interface
 * @param stat_port_g : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_intf_get(ptin_intf_t *ptin_intf, ptin_DHCP_Statistics_t *stat_port_g)
{
  L7_uint32 ptin_port;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid interface %u/%u",ptin_intf->intf_id,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Return pointer to stat structure */
  if (stat_port_g!=L7_NULLPTR)
  {
    osapiSemaTake(ptin_dhcp_stats_sem,-1);
    memcpy(stat_port_g, &global_stats_intf[ptin_port], sizeof(ptin_DHCP_Statistics_t));
    osapiSemaGive(ptin_dhcp_stats_sem);
  }

  return L7_SUCCESS;
}

/**
 * Get DHCP statistics of a particular DHCP instance and 
 * interface 
 * 
 * @param evc_idx   : Unicast EVC id
 * @param intIfNum  : interface
 * @param stat_port : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_instanceIntf_get(L7_uint32 evc_idx, ptin_intf_t *ptin_intf, ptin_DHCP_Statistics_t *stat_port)
{
  L7_uint32 ptin_port;
  L7_uint32 dhcp_idx;
  ptin_evc_intfCfg_t intfCfg;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid interface %u/%u",ptin_intf->intf_id,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Check if EVC is active, and if interface is part of the EVC */
  if (ptin_evc_intfCfg_get(evc_idx,ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error getting interface (%u/%u) configuration from EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,evc_idx);
    return L7_FAILURE;
  }
  if (!intfCfg.in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Interface %u/%u is not in use by EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,evc_idx);
    return L7_FAILURE;
  }

  /* Get Dhcp instance */
  if (ptin_dhcp_instance_find(evc_idx,&dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"EVC %u does not belong to any DHCP instance",evc_idx);
    return L7_FAILURE;
  }

  /* Return pointer to stat structure */
  if (stat_port!=L7_NULLPTR)
  {
    osapiSemaTake(ptin_dhcp_stats_sem,-1);
    memcpy(stat_port, &dhcpInstances[dhcp_idx].stats_intf[ptin_port], sizeof(ptin_DHCP_Statistics_t));
    osapiSemaGive(ptin_dhcp_stats_sem);
  }

  return L7_SUCCESS;
}

/**
 * Get DHCP statistics of a particular DHCP instance and 
 * client
 * 
 * @param evc_idx  : Unicast EVC id
 * @param client      : client reference
 * @param stat_port   : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_client_get(L7_uint32 evc_idx, const ptin_client_id_t *client_id, ptin_DHCP_Statistics_t *stat_client)
{
  L7_uint32 dhcp_idx;
  ptin_client_id_t client;
  ptinDhcpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if (client_id == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get Dhcp instance */
  if (ptin_dhcp_instance_find(evc_idx, &dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"EVC %u does not belong to any DHCP instance",evc_idx);
    return L7_FAILURE;
  }

  memcpy(&client, client_id, sizeof(ptin_client_id_t));

  /* Validate and rearrange clientId info */
  if (ptin_dhcp_clientId_convert(evc_idx, &client) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid client id");
    return L7_FAILURE;
  }

  /* Get client */
  if (ptin_dhcp_client_find(dhcp_idx, &client, &clientInfo)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,
            "Error searching for client {mask=0x%02x,"
            "port=%u/%u,"
            "svlan=%u,"
            "cvlan=%u,"
            "ipAddr=%u.%u.%u.%u,"
            "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} "
            "in dhcp_idx=%u",
            client.mask,
            client.ptin_intf.intf_type, client.ptin_intf.intf_id,
            client.outerVlan,
            client.innerVlan,
            (client.ipv4_addr>>24) & 0xff, (client.ipv4_addr>>16) & 0xff, (client.ipv4_addr>>8) & 0xff, client.ipv4_addr & 0xff,
            client.macAddr[0],client.macAddr[1],client.macAddr[2],client.macAddr[3],client.macAddr[4],client.macAddr[5],
            dhcp_idx);
    return L7_FAILURE;
  }

  /* Return pointer to stat structure */
  if (stat_client!=L7_NULLPTR)
  {
    osapiSemaTake(ptin_dhcp_stats_sem,-1);
#if 1 /* Daniel - Disabled this for now, as both APIs do not match. Hence, I cannot simply use memcpy */
    stat_client->dhcp_rx_intercepted                         = clientInfo->client_stats.dhcp_rx_intercepted;
    stat_client->dhcp_rx                                     = clientInfo->client_stats.dhcp_rx;
    stat_client->dhcp_rx_filtered                            = clientInfo->client_stats.dhcp_rx_filtered;
    stat_client->dhcp_tx_forwarded                           = clientInfo->client_stats.dhcp_tx_forwarded;
    stat_client->dhcp_tx_failed                              = clientInfo->client_stats.dhcp_tx_failed;

    stat_client->dhcp_rx_client_requests_without_options     = clientInfo->client_stats.dhcp_rx_client_requests_without_options;
    stat_client->dhcp_tx_client_requests_with_option82       = clientInfo->client_stats.dhcp_tx_client_requests_with_option82;
    stat_client->dhcp_tx_client_requests_with_option37       = clientInfo->client_stats.dhcp_tx_client_requests_with_option37;
    stat_client->dhcp_tx_client_requests_with_option18       = clientInfo->client_stats.dhcp_tx_client_requests_with_option18;
    stat_client->dhcp_rx_server_replies_with_option82        = clientInfo->client_stats.dhcp_rx_server_replies_with_option82;
    stat_client->dhcp_rx_server_replies_with_option37        = clientInfo->client_stats.dhcp_rx_server_replies_with_option37;
    stat_client->dhcp_rx_server_replies_with_option18        = clientInfo->client_stats.dhcp_rx_server_replies_with_option18;
    stat_client->dhcp_tx_server_replies_without_options      = clientInfo->client_stats.dhcp_tx_server_replies_without_options;

    stat_client->dhcp_rx_client_pkts_onTrustedIntf           = clientInfo->client_stats.dhcp_rx_client_pkts_onTrustedIntf;
    stat_client->dhcp_rx_client_pkts_withOps_onUntrustedIntf = clientInfo->client_stats.dhcp_rx_client_pkts_withOps_onUntrustedIntf;
    stat_client->dhcp_rx_server_pkts_onUntrustedIntf         = clientInfo->client_stats.dhcp_rx_server_pkts_onUntrustedIntf;
#else
    memcpy(stat_client, &clientInfo->client_stats, sizeof(ptin_DHCP_Statistics_t));
#endif
    osapiSemaGive(ptin_dhcp_stats_sem);
  }

  return L7_SUCCESS;
}

/**
 * Clear all DHCP statistics
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_clearAll(void)
{
  L7_uint dhcp_idx;
  ptinDhcpClientDataKey_t   avl_key;
  ptinDhcpClientInfoData_t *avl_info;

  osapiSemaTake(ptin_dhcp_stats_sem,-1);

  /* Clear global statistics */
  memset(global_stats_intf,0x00,sizeof(global_stats_intf));

  /* Run all DHCP instances */
  for (dhcp_idx=0; dhcp_idx<PTIN_SYSTEM_N_DHCP_INSTANCES; dhcp_idx++)
  {
    if (!dhcpInstances[dhcp_idx].inUse)  continue;

    /* Clear instance statistics */
    memset(dhcpInstances[dhcp_idx].stats_intf, 0x00, sizeof(dhcpInstances[dhcp_idx].stats_intf));
  }

  /* Run all cells in AVL tree related to this instance */
  memset(&avl_key,0x00,sizeof(ptinDhcpClientDataKey_t));
  while ( (avl_info = (ptinDhcpClientInfoData_t *)
                      avlSearchLVL7(&dhcpClients_unified.avlTree.dhcpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->dhcpClientDataKey, sizeof(ptinDhcpClientDataKey_t));

    /* Clear client statistics */
    memset(&avl_info->client_stats, 0x00, sizeof(ptin_DHCP_Statistics_t));
  }

  osapiSemaGive(ptin_dhcp_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear all statistics of one DHCP instance
 * 
 * @param evc_idx : Unicast EVC id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_instance_clear(L7_uint32 evc_idx)
{
  L7_uint dhcp_idx;
  struct ptin_clientInfo_entry_s *clientInfo_entry;

  /* Get Dhcp instance */
  if (ptin_dhcp_instance_find(evc_idx,&dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"EVC %u does not belong to any DHCP instance",evc_idx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_dhcp_stats_sem,-1);

  /* Clear instance statistics */
  memset(dhcpInstances[dhcp_idx].stats_intf, 0x00, sizeof(dhcpInstances[dhcp_idx].stats_intf));

  /* Clear statistics of belonging clients */
  clientInfo_entry = NULL;
  dl_queue_get_head(&dhcpInstances[dhcp_idx].queue_clients, (dl_queue_elem_t **)&clientInfo_entry);
  while (clientInfo_entry != NULL)
  {
    /* Clear client statistics */
    if (clientInfo_entry->client_info != L7_NULLPTR)
    {
      memset(&clientInfo_entry->client_info->client_stats, 0x00, sizeof(ptin_DHCP_Statistics_t)); 
    }

    /* Next queue element */
    clientInfo_entry = (struct ptin_clientInfo_entry_s *) dl_queue_get_next(&dhcpInstances[dhcp_idx].queue_clients, (dl_queue_elem_t *) clientInfo_entry);
  }

  osapiSemaGive(ptin_dhcp_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear interface DHCP statistics
 * 
 * @param intIfNum    : interface 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_intf_clear(ptin_intf_t *ptin_intf)
{
  L7_uint dhcp_idx;
  L7_uint32 ptin_port;
  ptinDhcpClientDataKey_t   avl_key;
  ptinDhcpClientInfoData_t *avl_info;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid interface %u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_dhcp_stats_sem,-1);

  /* Clear global statistics */
  memset(&global_stats_intf[ptin_port], 0x00, sizeof(ptin_DHCP_Statistics_t));

  /* Run all DHCP instances */
  for (dhcp_idx=0; dhcp_idx<PTIN_SYSTEM_N_DHCP_INSTANCES; dhcp_idx++)
  {
    if (!dhcpInstances[dhcp_idx].inUse)  continue;

    /* Clear instance statistics */
    memset(&dhcpInstances[dhcp_idx].stats_intf[ptin_port], 0x00, sizeof(ptin_DHCP_Statistics_t));
  }

  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  /* Run all cells in AVL tree related to this instance instance */
  memset(&avl_key,0x00,sizeof(ptinDhcpClientDataKey_t));
  while ( (avl_info = (ptinDhcpClientInfoData_t *)
                      avlSearchLVL7(&dhcpClients_unified.avlTree.dhcpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->dhcpClientDataKey, sizeof(ptinDhcpClientDataKey_t));

    /* Clear stats */
    if (avl_info->dhcpClientDataKey.ptin_port == ptin_port)
    {
      memset(&avl_info->client_stats, 0x00, sizeof(ptin_DHCP_Statistics_t));
    }
  }
  #endif

  osapiSemaGive(ptin_dhcp_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear statistics of a particular DHCP instance and interface
 * 
 * @param evc_idx  : Unicast EVC id
 * @param intIfNum    : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_instanceIntf_clear(L7_uint32 evc_idx, ptin_intf_t *ptin_intf)
{
  L7_uint dhcp_idx;
  L7_uint32 ptin_port;
  st_DhcpInstCfg_t *dhcpInst;
  ptin_evc_intfCfg_t intfCfg;
  struct ptin_clientInfo_entry_s *clientInfo_entry;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid interface %u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Check if EVC is active, and if interface is part of the EVC */
  if (ptin_evc_intfCfg_get(evc_idx,ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error getting interface (%u/%u) configuration from EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,evc_idx);
    return L7_FAILURE;
  }
  if (!intfCfg.in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Interface %u/%u is not in use by EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,evc_idx);
    return L7_FAILURE;
  }

  /* Get Dhcp instance */
  if (ptin_dhcp_instance_find(evc_idx,&dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"EVC %u does not belong to any DHCP instance",evc_idx);
    return L7_FAILURE;
  }

  /* Pointer to dhcp instance */
  dhcpInst = &dhcpInstances[dhcp_idx];

  osapiSemaTake(ptin_dhcp_stats_sem,-1);

  /* Clear instance statistics */
  memset(&dhcpInst->stats_intf[ptin_port], 0x00, sizeof(ptin_DHCP_Statistics_t));

  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  /* Clear statistics of belonging clients */
  clientInfo_entry = NULL;
  dl_queue_get_head(&dhcpInstances[dhcp_idx].queue_clients, (dl_queue_elem_t **)&clientInfo_entry);
  while (clientInfo_entry != NULL)
  {
    /* Clear client statistics, if port matches */
    if (clientInfo_entry->client_info != L7_NULLPTR &&
        clientInfo_entry->client_info->dhcpClientDataKey.ptin_port == ptin_port)
    {
      memset(&clientInfo_entry->client_info->client_stats, 0x00, sizeof(ptin_DHCP_Statistics_t));
    }

    /* Next queue element */
    clientInfo_entry = (struct ptin_clientInfo_entry_s *) dl_queue_get_next(&dhcpInstances[dhcp_idx].queue_clients, (dl_queue_elem_t *) clientInfo_entry);
  }
  #endif

  osapiSemaGive(ptin_dhcp_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear DHCP statistics of a particular DHCP instance and 
 * client
 * 
 * @param evc_idx  : Unicast EVC id
 * @param client      : client reference
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_client_clear(L7_uint32 evc_idx, const ptin_client_id_t *client_id)
{
  L7_uint dhcp_idx;
  ptin_client_id_t client;
  ptinDhcpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if (client_id == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get Dhcp instance */
  if (ptin_dhcp_instance_find(evc_idx, &dhcp_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"EVC %u does not belong to any DHCP instance",evc_idx);
    return L7_FAILURE;
  }

  memcpy(&client, client_id, sizeof(ptin_client_id_t));

  /* Validate and rearrange clientId info */
  if (ptin_dhcp_clientId_convert(evc_idx, &client) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid client id");
    return L7_FAILURE;
  }

  /* Find client */
  if (ptin_dhcp_client_find(dhcp_idx, &client, &clientInfo) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,
            "Error searching for client {mask=0x%02x,"
            "port=%u/%u,"
            "svlan=%u,"
            "cvlan=%u,"
            "ipAddr=%u.%u.%u.%u,"
            "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} "
            "in dhcp_idx=%u",
            client.mask,
            client.ptin_intf.intf_type, client.ptin_intf.intf_id,
            client.outerVlan,
            client.innerVlan,
            (client.ipv4_addr>>24) & 0xff, (client.ipv4_addr>>16) & 0xff, (client.ipv4_addr>>8) & 0xff, client.ipv4_addr & 0xff,
            client.macAddr[0],client.macAddr[1],client.macAddr[2],client.macAddr[3],client.macAddr[4],client.macAddr[5],
            dhcp_idx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_dhcp_stats_sem,-1);

  /* Clear client statistics (if port matches) */
  memset(&clientInfo->client_stats, 0x00, sizeof(ptin_DHCP_Statistics_t));

  osapiSemaGive(ptin_dhcp_stats_sem);

  return L7_SUCCESS;
}

/*********************************************************** 
 * Internal functions (for Fastpath usage)
 ***********************************************************/

/**
 * Validate ingress interface for a DHCP packet 
 * 
 * @param intIfNum    : interface
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_dhcp_intf_validate(L7_uint32 intIfNum)
{
  /* Validate arguments */
  if ( intIfNum==0 || intIfNum>=L7_MAX_INTERFACE_COUNT )
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments: intIfNum=%u",intIfNum);
    return L7_FALSE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_intIfNum2ptintf(intIfNum,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid intIfNum %u",intIfNum);
    return L7_FALSE;
  }

  return L7_TRUE;
}

/**
 * Validate internal vlan in a DHCP Packet 
 * 
 * @param intVlanId   : internal vlan
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_dhcp_vlan_validate(L7_uint16 intVlanId)
{
  /* Validate arguments */
  if ( intVlanId<1 || intVlanId>=4095 )
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments: intVlan=%u",intVlanId);
    return L7_FALSE;
  }

  /* DHCP instance, from internal vlan */
  if (ptin_dhcp_inst_get_fromIntVlan(intVlanId,L7_NULLPTR,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No DHCP instance associated to intVlan %u",intVlanId);
    return L7_FALSE;
  }

  return L7_TRUE;
}

/**
 * Validate interface, internal vlan and innervlan received in a 
 * DHCP packet 
 * 
 * @param intIfNum    : interface
 * @param intVlanId   : internal vlan
 * @param innerVlanId : client vlan
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_dhcp_intfVlan_validate(L7_uint32 intIfNum, L7_uint16 intVlanId /*, L7_uint16 innerVlanId*/)
{
  L7_uint dhcp_idx;
  L7_uint32 evc_id_ext;
  ptin_intf_t ptin_intf;
  ptin_evc_intfCfg_t intfCfg;
  st_DhcpInstCfg_t *dhcpInst;

  /* Validate arguments */
  if ( intIfNum==0 || intIfNum>=L7_MAX_INTERFACE_COUNT ||
       intVlanId<PTIN_VLAN_MIN || intVlanId>PTIN_VLAN_MAX )
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments: intIfNum=%u intVlan=%u",intIfNum,intVlanId);
    return L7_FALSE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_intIfNum2ptintf(intIfNum, &ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid intIfNum %u", intIfNum);
    return L7_FALSE;
  }

  /* DHCP instance, from internal vlan */
  if (ptin_dhcp_inst_get_fromIntVlan(intVlanId, &dhcpInst, &dhcp_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No DHCP instance associated to intVlan %u", intVlanId);
    return L7_FALSE;
  }

  /* DHCP instance, from internal vlan */
  if (ptin_evc_get_evcIdfromIntVlan(intVlanId, &evc_id_ext) != L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No EVC id associated to intVlan %u", intVlanId);
    return L7_FALSE;
  }

  /* Check if EVCs are in use */
  if (!ptin_evc_is_in_use(evc_id_ext))
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Inconsistency: eEVCid=%u (Vlan %u) not in use", evc_id_ext, intVlanId);
    return L7_FAILURE;
  }

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(evc_id_ext, &ptin_intf, &intfCfg)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error acquiring interface %u/%u configuarion from eEVC id %u",
              ptin_intf.intf_type, ptin_intf.intf_id, evc_id_ext);
    return L7_FALSE;
  }

  /* Interface must be in use */
  if (!intfCfg.in_use)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Interface %u/%u (intIfNum=%u) is not in use for eEVC %u",
              ptin_intf.intf_type, ptin_intf.intf_id, intIfNum, evc_id_ext);
    return L7_FALSE;
  }

  return L7_TRUE;
}

/**
 * Set all interfaces as untrusted
 */
void ptin_dhcp_intfTrusted_init(void)
{
  /* All ports as untrusted */
  memset(&dhcp_intIfNum_trusted, 0x00, sizeof(dhcp_intIfNum_trusted));

  LOG_INFO(LOG_CTX_PTIN_DHCP,"Trusted ports initialized");
}

/**
 * Set a particular interface as trusted or not
 * 
 * @param intIfNum : interface
 * @param trusted  : trusted
 */
void ptin_dhcp_intfTrusted_set(L7_uint32 intIfNum, L7_BOOL trusted)
{
  if (trusted)
  {
    L7_INTF_SETMASKBIT(dhcp_intIfNum_trusted, intIfNum);
  }
  else
  {
    L7_INTF_CLRMASKBIT(dhcp_intIfNum_trusted, intIfNum);
  }
}


/**
 * Check if a particular interface of one EVC is trusted
 * 
 * @param intIfNum    : interface
 * @param intVlanId   : internal vlan
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_dhcp_is_intfTrusted(L7_uint32 intIfNum, L7_uint16 intVlanId)
{
  L7_uint dhcp_idx;
  L7_uint32 evc_id_ext;
  ptin_intf_t ptin_intf;
  ptin_evc_intfCfg_t intfCfg;
  st_DhcpInstCfg_t *dhcpInst;

  /* Validate arguments */
  if ( intIfNum == 0 || intIfNum >= L7_MAX_INTERFACE_COUNT ||
      (intVlanId != 0 && (intVlanId < PTIN_VLAN_MIN || intVlanId > PTIN_VLAN_MAX)) )
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments: intIfNum=%u intVlan=%u",intIfNum,intVlanId);
    return L7_FALSE;
  }

  /* Mask with list of trusted ports */
  if (!L7_INTF_ISMASKBITSET(dhcp_intIfNum_trusted, intIfNum))
  {
    return L7_FALSE;
  }

  /* If VLAN is null, return general trusted state */
  if (intVlanId == 0)
  {
    return L7_TRUE;
  }

  /* Proceed to vlan validation */

  /* Convert interface to ptin_port */
  if (ptin_intf_intIfNum2ptintf(intIfNum, &ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid intIfNum %u", intIfNum);
    return L7_FALSE;
  }

  /* DHCP instance, from internal vlan */
  if (ptin_dhcp_inst_get_fromIntVlan(intVlanId, &dhcpInst, &dhcp_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No DHCP instance associated to intVlan %u", intVlanId);
    return L7_FALSE;
  }

  /* DHCP instance, from internal vlan */
  if (ptin_evc_get_evcIdfromIntVlan(intVlanId, &evc_id_ext) != L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No EVC id associated to intVlan %u", intVlanId);
    return L7_FALSE;
  }

  /* Check if EVCs are in use */
  if (!ptin_evc_is_in_use(evc_id_ext))
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Inconsistency: eEVCid=%u (Vlan %u) not in use", evc_id_ext, intVlanId);
    return L7_FAILURE;
  }

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(evc_id_ext, &ptin_intf, &intfCfg)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error acquiring interface %u/%u configuarion from eEVC id %u",
              ptin_intf.intf_type, ptin_intf.intf_id, evc_id_ext);
    return L7_FALSE;
  }

  /* Interface must be in use */
  if (!intfCfg.in_use)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Interface %u/%u (intIfNum=%u) is not in use for eEVC %u",
              ptin_intf.intf_type, ptin_intf.intf_id, intIfNum, evc_id_ext);
    return L7_FALSE;
  }

  return L7_TRUE;
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
L7_BOOL ptin_dhcp_intfTrusted_getList(L7_uint16 intVlanId, NIM_INTF_MASK_t *intfList)
{
  L7_uint32             i, intIfNum;
  ptin_intf_t           ptintf;
  L7_uint               dhcp_idx;
  st_DhcpInstCfg_t      *dhcpInst;
  L7_uint32             evc_id_ext;
  ptin_HwEthMef10Evc_t  evcConf;

  /* DHCP instance, from internal vlan */
  if (ptin_dhcp_inst_get_fromIntVlan(intVlanId, &dhcpInst, &dhcp_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No DHCP instance associated to intVlan %u", intVlanId);
    return L7_FALSE;
  }

  /* DHCP instance, from internal vlan */
  if (ptin_evc_get_evcIdfromIntVlan(intVlanId, &evc_id_ext) != L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No EVC id associated to intVlan %u", intVlanId);
    return L7_FALSE;
  }

  /* Check if EVCs are in use */
  if (!ptin_evc_is_in_use(evc_id_ext))
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Inconsistency: eEVCid=%u (Vlan %u) not in use", evc_id_ext, intVlanId);
    return L7_FAILURE;
  }

  /* Get interface configuration */
  memset(&evcConf, 0x00, sizeof(evcConf));
  evcConf.index = evc_id_ext;
  if (ptin_evc_get(&evcConf) != L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error acquiring eEVC %u configuration", evc_id_ext);
    return L7_FALSE;
  }

  /* Validate output list */
  if (intfList == L7_NULLPTR)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_DHCP,"Will not return data");
    return L7_SUCCESS;
  }

  if (ptin_debug_dhcp_snooping)
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"eEVC %u, has %u ports", evc_id_ext, evcConf.n_intf);

  /* Clear output mask ports */
  memset(intfList, 0x00, sizeof(NIM_INTF_MASK_t));

  /* Check all EVC ports for trusted ones */
  for (i = 0; i < evcConf.n_intf; i++)
  {
    ptintf.intf_type = evcConf.intf[i].intf_type;
    ptintf.intf_id   = evcConf.intf[i].intf_id;

    if (ptin_debug_dhcp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_DHCP,"Processing ptin_intf %u/%u", ptintf.intf_type, ptintf.intf_id);

    /* Convert interface to intIfNum */
    if (ptin_intf_ptintf2intIfNum(&ptintf, &intIfNum) != L7_SUCCESS)
    {
      if (ptin_debug_dhcp_snooping)
        LOG_WARNING(LOG_CTX_PTIN_DHCP,"Error converting ptin_intf %u/%u to intIfNum format", ptintf.intf_type, ptintf.intf_id);
      continue;
    }

    /* Mark interface as trusted, if it is */
    if (L7_INTF_ISMASKBITSET(dhcp_intIfNum_trusted, intIfNum))
    {
      if (ptin_debug_dhcp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"ptin_intf %u/%u or intIfNum %u is trusted", ptintf.intf_type, ptintf.intf_id, intIfNum);
      L7_INTF_SETMASKBIT(*intfList, intIfNum);
    }
    else
    {
      if (ptin_debug_dhcp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"ptin_intf %u/%u or intIfNum %u is UNtrusted", ptintf.intf_type, ptintf.intf_id, intIfNum);
    }
  }

  return L7_SUCCESS;
}

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
L7_RC_t ptin_dhcp_extVlans_get(L7_uint32 intIfNum, L7_uint16 intOVlan, L7_uint16 intIVlan,
                               L7_int client_idx, L7_uint16 *uni_ovid, L7_uint16 *uni_ivid)
{
  L7_uint dhcp_idx;
  L7_uint32 flags;
  L7_uint16 ovid, ivid;
  ptinDhcpClientInfoData_t *clientInfo;
  L7_uint8  intf_type;

  /* Get interface type */
  if (ptin_evc_intf_type_get(intOVlan, intIfNum, &intf_type) != L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_DHCP, "Error getting intf configuration for intVlan %u, intIfNum %u", intOVlan, intIfNum);
    return L7_FAILURE;
  }
  /* Validate interface type */
  if (intf_type != PTIN_EVC_INTF_ROOT && intf_type != PTIN_EVC_INTF_LEAF)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_DHCP, "intVlan %u / intIfNum %u is not used", intOVlan, intIfNum);
    return L7_FAILURE;
  }

  ovid = ivid = 0;
  /* If client is provided, go directly to client info */
  if ((intf_type == PTIN_EVC_INTF_LEAF) &&                            /* Is a leaf port */
      (client_idx>=0 && client_idx<PTIN_SYSTEM_DHCP_MAXCLIENTS) &&    /* Valid index */
      (clientInfo_pool[client_idx].client_info != L7_NULLPTR) &&      /* Client exists */
      ptin_dhcp_inst_get_fromIntVlan(intOVlan, L7_NULLPTR, &dhcp_idx)==L7_SUCCESS)  /* intOVlan is valid */
  {
    /* Get pointer to client structure in AVL tree */
    clientInfo = clientInfo_pool[client_idx].client_info;

    ovid = clientInfo->uni_ovid;
    ivid = clientInfo->uni_ivid;
  }

  if (ptin_debug_dhcp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_DHCP, "ovid=%u ivid=%u", ovid, ivid); 

  /* If no data was retrieved, goto EVC info */
  if (ovid == 0)
  {
    if (ptin_evc_extVlans_get_fromIntVlan(intIfNum, intOVlan, intIVlan, &ovid, &ivid) != L7_SUCCESS)
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

  if (ptin_debug_dhcp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_DHCP,"ovid=%u ivid=%u", ovid, ivid);

  /* Return vlans */
  if (uni_ovid != L7_SUCCESS)  *uni_ovid = ovid;
  if (uni_ivid != L7_SUCCESS)  *uni_ivid = ivid;

  return L7_SUCCESS;
}

/**
 * Get the client index associated to a DHCP client 
 * 
 * @param intIfNum      : interface number
 * @param intVlan       : internal vlan
 * @param client        : Client information parameters
 * @param client_index  : Client index to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_clientIndex_get(L7_uint32 intIfNum, L7_uint16 intVlan,
                                  ptin_client_id_t *client,
                                  L7_uint *client_index)
{
  L7_uint     dhcp_idx;
  ptin_intf_t ptin_intf;
  L7_uint     client_idx;
  ptinDhcpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if ( client==L7_NULLPTR || DHCP_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* DHCP instance, from internal vlan */
  if (ptin_dhcp_inst_get_fromIntVlan(intVlan,L7_NULLPTR,&dhcp_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No DHCP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* If the inner vlan is not valid, return -1 as client index */
  #if DHCP_CLIENT_INNERVLAN_SUPPORTED
  if ((client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) &&
      (client->innerVlan==0 || client->innerVlan>4095))
  {
    if (client_index!=L7_NULLPTR)  *client_index = (L7_uint)-1;
    return L7_SUCCESS;
  }
  #endif

  /* Get ptin_port format for the interface number */
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
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
        if (ptin_debug_dhcp_snooping)
          LOG_ERR(LOG_CTX_PTIN_DHCP,"Connot convert client intIfNum %u to ptin_port_format",intIfNum);
        return L7_FAILURE;
      }
    }
  }
  #endif

  /* Get client */
  if (ptin_dhcp_client_find(dhcp_idx, client, &clientInfo)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,
              "Error searching for client {mask=0x%02x,"
              "port=%u/%u,"
              "svlan=%u,"
              "cvlan=%u,"
              "ipAddr=%u.%u.%u.%u,"
              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} "
              "in dhcp_idx=%u",
              client->mask,
              client->ptin_intf.intf_type, client->ptin_intf.intf_id,
              client->outerVlan,
              client->innerVlan,
              (client->ipv4_addr>>24) & 0xff, (client->ipv4_addr>>16) & 0xff, (client->ipv4_addr>>8) & 0xff, client->ipv4_addr & 0xff,
              client->macAddr[0],client->macAddr[1],client->macAddr[2],client->macAddr[3],client->macAddr[4],client->macAddr[5],
              dhcp_idx);
    }
    return L7_FAILURE;
  }

  /* Update client index in data cell */
  client_idx = clientInfo->client_index;

  #if (DHCP_CLIENT_DEBUG)
  LOG_TRACE(LOG_CTX_PTIN_DHCP,"Client_idx=%u for key {"
            #if (DHCP_CLIENT_INTERF_SUPPORTED)
                              "port=%u,"
            #endif
            #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                              "svlan=%u,"
            #endif
            #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                              "cvlan=%u,"
            #endif
            #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                              "ipAddr=%u.%u.%u.%u,"
            #endif
            #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
            #endif
                              "}"
            ,client_idx
            #if (DHCP_CLIENT_INTERF_SUPPORTED)
            ,clientInfo->dhcpClientDataKey.ptin_port
            #endif
            #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
            ,clientInfo->dhcpClientDataKey.outerVlan
            #endif
            #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
            ,clientInfo->dhcpClientDataKey.innerVlan
            #endif
            #if (DHCP_CLIENT_IPADDR_SUPPORTED)
            ,(clientInfo->dhcpClientDataKey.ipv4_addr>>24) & 0xff, (clientInfo->dhcpClientDataKey.ipv4_addr>>16) & 0xff, (clientInfo->dhcpClientDataKey.ipv4_addr>>8) & 0xff, clientInfo->dhcpClientDataKey.ipv4_addr & 0xff
            #endif
            #if (DHCP_CLIENT_MACADDR_SUPPORTED)
            ,clientInfo->dhcpClientDataKey.macAddr[0],clientInfo->dhcpClientDataKey.macAddr[1],clientInfo->dhcpClientDataKey.macAddr[2],clientInfo->dhcpClientDataKey.macAddr[3],clientInfo->dhcpClientDataKey.macAddr[4],clientInfo->dhcpClientDataKey.macAddr[5]
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
L7_RC_t ptin_dhcp_clientData_get(L7_uint16 intVlan,
                                 L7_uint client_idx,
                                 ptin_client_id_t *client)
{
  ptin_intf_t ptin_intf;
  //st_DhcpInstCfg_t *dhcpInst;
  ptinDhcpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if ( client==L7_NULLPTR || client_idx>=PTIN_SYSTEM_DHCP_MAXCLIENTS )
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  #if 0
  /* DHCP instance, from internal vlan */
  if (ptin_dhcp_inst_get_fromIntVlan(intVlan,&dhcpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No DHCP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }
  #endif

  /* Get pointer to client structure in AVL tree */
  clientInfo = clientInfo_pool[client_idx].client_info;
  /* If does not exist... */
  if (clientInfo==L7_NULLPTR)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Provided client_idx (%u) does not exist",client_idx);
    return L7_FAILURE;
  }

  memset(client,0x00,sizeof(ptin_client_id_t));
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  if (ptin_intf_port2ptintf(clientInfo->dhcpClientDataKey.ptin_port,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Cannot convert client port %uu to ptin_intf format",clientInfo->dhcpClientDataKey.ptin_port);
    return L7_FAILURE;
  }
  client->ptin_intf = ptin_intf;
  client->mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  #endif
  #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
  client->outerVlan = clientInfo->dhcpClientDataKey.outerVlan;
  client->mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  #endif
  #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
  if (clientInfo->dhcpClientDataKey.innerVlan > 0 && clientInfo->dhcpClientDataKey.innerVlan < 4096)
  {
    client->innerVlan = clientInfo->dhcpClientDataKey.innerVlan;
    client->mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  }
  #endif
  #if (DHCP_CLIENT_IPADDR_SUPPORTED)
  client->ipv4_addr = clientInfo->dhcpClientDataKey.ipv4_addr;
  client->mask |= PTIN_CLIENT_MASK_FIELD_IPADDR;
  #endif
  #if (DHCP_CLIENT_MACADDR_SUPPORTED)
  memcpy(client->macAddr,clientInfo->dhcpClientDataKey.macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  client->mask |= PTIN_CLIENT_MASK_FIELD_MACADDR;
  #endif

  return L7_SUCCESS;
}

/**
 * Get DHCP client data (circuit and remote ids)
 * 
 * @param intIfNum   : FP interface
 * @param intVlan    : internal vlan
 * @param dhcp_flags : DHCP flags (output) 
 * @param evc_flags  : EVC flags (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_flags_get(L7_uint16 intVlan, L7_uint8 *dhcp_flags, L7_uint32 *evc_flags)
{
  L7_uint dhcp_idx;

  /* Validate arguments */
  if (intVlan < PTIN_VLAN_MIN || intVlan > PTIN_VLAN_MAX)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get dhcp instance */
  if (ptin_dhcp_inst_get_fromIntVlan(intVlan, L7_NULLPTR, &dhcp_idx) != L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Internal vlan %u does not correspond to any DHCP instance",intVlan);
    return L7_FAILURE;
  }

  /* Get flags */
  if (dhcp_flags != L7_NULLPTR)
  {
    if (ptin_dhcp_flags_get_instance(dhcp_idx, L7_NULLPTR, dhcp_flags) != L7_SUCCESS) 
    {
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP,"Error acquiring DHCP flags for internal vlan %u / dhcp_idx=%u", intVlan, dhcp_idx);
      return L7_FAILURE;
    }
  }

  /* Get EVC flags */
  if (evc_flags != L7_NULLPTR)
  {
    if (ptin_evc_flags_get_fromIntVlan(intVlan, evc_flags, L7_NULLPTR) != L7_SUCCESS)
    {
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP,"Error acquiring EVC flags for internal vlan %u", intVlan);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/**
 * Get DHCP client data (circuit and remote ids)
 * 
 * @param intIfNum    : FP interface
 * @param intVlan     : internal vlan
 * @param innerVlan   : inner/client vlan 
 * @param circuitId   : circuit id (output) 
 * @param remoteId    : remote id (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stringIds_get(L7_uint32 intIfNum, L7_uint16 intVlan, L7_uint16 innerVlan, L7_uchar8 *macAddr,
                                L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_uint dhcp_idx;
  ptin_intf_t ptin_intf;
  ptin_client_id_t client;
  ptinDhcpClientInfoData_t *client_info;

  /* Validate arguments */
  if (intIfNum==0 || intIfNum>=L7_MAX_INTERFACE_COUNT ||
      intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX /*||
      innerVlan==0 || innerVlan>=4096*/)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Convert interface to ptin format */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid intIfNum (%u)",intIfNum);
    return L7_FAILURE;
  }

  /* Get dhcp instance */
  if (ptin_dhcp_inst_get_fromIntVlan(intVlan,L7_NULLPTR,&dhcp_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Internal vlan %u does not correspond to any DHCP instance",intVlan);
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
    #if DHCP_CLIENT_INTERF_SUPPORTED
    client.ptin_intf.intf_type = ptin_intf.intf_type;
    client.ptin_intf.intf_id   = ptin_intf.intf_id;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
    #endif
    #if DHCP_CLIENT_OUTERVLAN_SUPPORTED
      client.outerVlan = intVlan;
      client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
    #endif
    #if DHCP_CLIENT_INNERVLAN_SUPPORTED
    if (innerVlan > 0 && innerVlan < 4096)
    {
      client.innerVlan = innerVlan;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
    }
    #endif

    /* Find client information */
    if (ptin_dhcp_client_find(dhcp_idx,&client,&client_info)!=L7_SUCCESS)
    {
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP,"Non existent client in DHCP instance %u (EVC id %u)",dhcp_idx,dhcpInstances[dhcp_idx].evc_idx);
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
      LOG_ERR(LOG_CTX_PTIN_DHCP, "circuitId is NULL");
    }
    if (remoteId!=L7_NULLPTR)
    {
      strncpy(remoteId ,client_info->client_data.remoteId_str ,FD_DS_MAX_REMOTE_ID_STRING);
      remoteId[FD_DS_MAX_REMOTE_ID_STRING-1] = '\0';
    }
  }
  else
  {
    #if DHCP_ACCEPT_UNSTACKED_PACKETS
    if (ptin_dhcp_strings_def_get(&ptin_intf,macAddr,circuitId,remoteId)!=L7_SUCCESS)
    {
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP,"Error getting default strings");
      return L7_FAILURE;
    }
    #else
    LOG_ERR(LOG_CTX_PTIN_DHCP,"No client defined!");
    return L7_FAILURE;
    #endif
  }

  return L7_SUCCESS;
}

/**
 * Get DHCP EVC ethernet priority
 * 
 * @param intVlan     : internal vlan
 * @param ethPrty     : priority (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_ethPrty_get(L7_uint16 intVlan, L7_uint8 *ethPrty)
{
  L7_uint dhcp_idx;

  /* Validate arguments */
  if (intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX  ||
      L7_NULLPTR == ethPrty)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get dhcp instance */
  if (ptin_dhcp_inst_get_fromIntVlan(intVlan,L7_NULLPTR,&dhcp_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Internal vlan %u does not correspond to any DHCP instance",intVlan);
    return L7_FAILURE;
  }

  ptin_dhcp_evc_ethprty_get(&dhcpInstances[dhcp_idx].circuitid, ethPrty);

  return L7_SUCCESS;
}

/**
 * Get DHCP client data (DHCP Options)
 *
 * @param intIfNum    : FP interface
 * @param intVlan     : internal vlan
 * @param innerVlan   : inner/client vlan
 * @param isActiveOp82: L7_TRUE if op82 is active for this client
 * @param isActiveOp37: L7_TRUE if op37 is active for this client
 * @param isActiveOp18: L7_TRUE if op18 is active for this client
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_client_options_get(L7_uint32 intIfNum, L7_uint16 intVlan, L7_uint16 innerVlan, L7_BOOL *isActiveOp82,
                                     L7_BOOL *isActiveOp37, L7_BOOL *isActiveOp18)
{
   L7_uint dhcp_idx;
   ptin_intf_t ptin_intf;
   ptin_client_id_t client;
   ptinDhcpClientInfoData_t *client_info;

   //Set DHCP options to L7_FALSE
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
   if (intIfNum == 0 || intIfNum >= L7_MAX_INTERFACE_COUNT || intVlan < PTIN_VLAN_MIN || intVlan > PTIN_VLAN_MAX
#if (PTIN_BOARD_IS_GPON)
       || innerVlan==0 || innerVlan>=4096
#endif
       )
   {
      if (ptin_debug_dhcp_snooping)
         LOG_ERR(LOG_CTX_PTIN_DHCP, "Invalid arguments");
      return L7_FAILURE;
   }

   /* Convert interface to ptin format */
   if (ptin_intf_intIfNum2ptintf(intIfNum, &ptin_intf) != L7_SUCCESS)
   {
      if (ptin_debug_dhcp_snooping)
         LOG_ERR(LOG_CTX_PTIN_DHCP, "Invalid intIfNum (%u)", intIfNum);
      return L7_FAILURE;
   }

   /* Get dhcp instance */
   if (ptin_dhcp_inst_get_fromIntVlan(intVlan, L7_NULLPTR, &dhcp_idx) != L7_SUCCESS)
   {
      if (ptin_debug_dhcp_snooping)
         LOG_ERR(LOG_CTX_PTIN_DHCP, "Internal vlan %u does not correspond to any DHCP instance", intVlan);
      return L7_FAILURE;
   }

#if (PTIN_BOARD_IS_GPON)
   if (innerVlan>0 && innerVlan<4096)
#else
   if (1)
#endif
   {
      /* Build client structure */
      memset(&client, 0x00, sizeof(ptin_client_id_t));
#if DHCP_CLIENT_INTERF_SUPPORTED
      client.ptin_intf.intf_type = ptin_intf.intf_type;
      client.ptin_intf.intf_id = ptin_intf.intf_id;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
#endif
#if DHCP_CLIENT_OUTERVLAN_SUPPORTED
      client.outerVlan = intVlan;
      client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
#endif
#if DHCP_CLIENT_INNERVLAN_SUPPORTED
      if (innerVlan > 0 && innerVlan < 4095)
      {
        client.innerVlan = innerVlan;
        client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
      }
#endif

      /* Find client information */
      if (ptin_dhcp_client_find(dhcp_idx, &client, &client_info) != L7_SUCCESS)
      {
         if (ptin_debug_dhcp_snooping)
            LOG_ERR(LOG_CTX_PTIN_DHCP, "Non existent client in DHCP instance %u (EVC id %u)",
                  dhcp_idx, dhcpInstances[dhcp_idx].evc_idx);
         return L7_FAILURE;
      }

      if (L7_NULLPTR != isActiveOp82) {
        *isActiveOp82 = 0x01 & client_info->client_data.dhcp_options;
      }
      if (L7_NULLPTR != isActiveOp37) {
        *isActiveOp37 = (0x02 & client_info->client_data.dhcp_options) >> 1;
      }
      if (L7_NULLPTR != isActiveOp18) {
        *isActiveOp18 = (0x04 & client_info->client_data.dhcp_options) >> 2;
      }
   }
   else
   {
      LOG_ERR(LOG_CTX_PTIN_DHCP, "No client defined!");
      return L7_FAILURE;
   }

   return L7_SUCCESS;
}

/**
 * Update DHCP snooping configuration, when interfaces are 
 * added/removed 
 * 
 * @param evc_idx     : EVC id 
 * @param ptin_intf : interface 
 * @param enable    : L7_TRUE when interface is added 
 *                    L7_FALSE when interface is removed
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_snooping_trap_interface_update(L7_uint32 evc_idx, ptin_intf_t *ptin_intf, L7_BOOL enable)
{
  return L7_SUCCESS;
}

/**
 * Increment DHCP statistics
 * 
 * @param intIfNum   : interface where the packet entered
 * @param vlan       : packet's interval vlan
 * @param client_idx : client index
 * @param field      : field to increment
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_increment_field(L7_uint32 intIfNum, L7_uint16 vlan, L7_uint32 client_idx, ptin_dhcp_stat_enum_t field)
{
  L7_uint32 ptin_port;
  st_DhcpInstCfg_t *dhcpInst;
  ptinDhcpClientInfoData_t *client;
  ptin_DHCP_Statistics_t *stat_port_g = L7_NULLPTR;
  ptin_DHCP_Statistics_t *stat_port   = L7_NULLPTR;
  ptin_DHCP_Statistics_t *stat_client = L7_NULLPTR;

  /* Validate field */
  if (field>=DHCP_STAT_FIELD_ALL)
  {
    return L7_FAILURE;
  }

  /* Get DHCP instance */
  dhcpInst = L7_NULLPTR;
  if (vlan>=PTIN_VLAN_MIN && vlan<=PTIN_VLAN_MAX)
  {
    if (ptin_dhcp_inst_get_fromIntVlan(vlan,&dhcpInst,L7_NULLPTR)!=L7_SUCCESS)
    {
      dhcpInst = L7_NULLPTR;
    }
  }

  /* If interface is valid... */
  if (intIfNum>0 && intIfNum<L7_MAX_INTERFACE_COUNT)
  {
    /* Check if interface exists */
    if (ptin_intf_intIfNum2port(intIfNum,&ptin_port)==L7_SUCCESS)
    {
      /* Global interface statistics at interface level */
      stat_port_g = &global_stats_intf[ptin_port];

      if (dhcpInst!=L7_NULLPTR)
      {
        /* interface statistics at dhcp instance and interface level */
        stat_port = &dhcpInst->stats_intf[ptin_port];
      }
    }
  }

  /* If client index is valid... */
  if (dhcpInst!=L7_NULLPTR && client_idx<PTIN_SYSTEM_DHCP_MAXCLIENTS)
  {
    client = clientInfo_pool[client_idx].client_info;
    if (client!=L7_NULLPTR)
    {
      /* Statistics at client level */
      stat_client = &client->client_stats;
    }
  }

  osapiSemaTake(ptin_dhcp_stats_sem,-1);

  switch (field) {
  case DHCP_STAT_FIELD_RX_INTERCEPTED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_intercepted++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_intercepted++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_intercepted++;
    break;

  case DHCP_STAT_FIELD_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx++;
    break;

  case DHCP_STAT_FIELD_RX_FILTERED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_filtered++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_filtered++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_filtered++;
    break;

  case DHCP_STAT_FIELD_TX_FORWARDED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_tx_forwarded++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_tx_forwarded++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_tx_forwarded++;
    break;

  case DHCP_STAT_FIELD_TX_FAILED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_tx_failed++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_tx_failed++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_tx_failed++;
    break;

  case DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_tx_client_requests_with_option82++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_tx_client_requests_with_option82++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_tx_client_requests_with_option82++;
    break;

  case DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION37:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_tx_client_requests_with_option37++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_tx_client_requests_with_option37++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_tx_client_requests_with_option37++;
    break;

  case DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION18:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_tx_client_requests_with_option18++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_tx_client_requests_with_option18++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_tx_client_requests_with_option18++;
    break;

  case DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_server_replies_with_option82++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_server_replies_with_option82++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_server_replies_with_option82++;
    break;

  case DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION37:
      if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_server_replies_with_option37++;
      if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_server_replies_with_option37++;
      if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_server_replies_with_option37++;
      break;

  case DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION18:
      if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_server_replies_with_option18++;
      if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_server_replies_with_option18++;
      if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_server_replies_with_option18++;
      break;

  case DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITHOUT_OPTIONS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_client_requests_without_options++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_client_requests_without_options++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_client_requests_without_options++;
    break;

  case DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITHOUT_OPTIONS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_tx_client_requests_without_options++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_tx_client_requests_without_options++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_tx_client_requests_without_options++;
    break;

  case DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITHOUT_OPTIONS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_server_replies_without_options++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_server_replies_without_options++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_server_replies_without_options++;
    break;

  case DHCP_STAT_FIELD_TX_SERVER_REPLIES_WITHOUT_OPTIONS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_tx_server_replies_without_options++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_tx_server_replies_without_options++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_tx_server_replies_without_options++;
    break;

  case DHCP_STAT_FIELD_RX_CLIENT_PKTS_ON_TRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_client_pkts_onTrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_client_pkts_onTrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_client_pkts_onTrustedIntf++;
    break;

  case DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOPS_ON_UNTRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_client_pkts_withOps_onUntrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_client_pkts_withOps_onUntrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_client_pkts_withOps_onUntrustedIntf++;
    break;

  case DHCP_STAT_FIELD_RX_SERVER_PKTS_ON_UNTRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_server_pkts_onUntrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_server_pkts_onUntrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_server_pkts_onUntrustedIntf++;
    break;

  default:
    break;
  }

  osapiSemaGive(ptin_dhcp_stats_sem);

  return L7_SUCCESS;
}

/*********************************************************** 
 * Static functions
 ***********************************************************/

/**
 * Gets the DHCP instance from the NNI ovlan
 * 
 * @param nni_ovlan  : NNI outer vlan 
 * @param dhcp_idx   : DHCP instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_dhcp_instance_find_agg(L7_uint16 nni_ovlan, L7_uint *dhcp_idx)
{
  L7_uint idx;

  /* Search for the provided Mcast and Ucast evcs */
  for (idx=0; idx<PTIN_SYSTEM_N_DHCP_INSTANCES; idx++)
  {
    if (!dhcpInstances[idx].inUse)  continue;

    if (dhcpInstances[idx].nni_ovid == nni_ovlan)
      break;
  }

  /* If not found empty instances, return error */
  if (idx>=PTIN_SYSTEM_N_DHCP_INSTANCES)
    return L7_FAILURE;

  /* Return instance index */
  if (dhcp_idx!=L7_NULLPTR)  *dhcp_idx = idx;

  return L7_SUCCESS;
}

/**
 * Find client information in a particulat DHCP instance
 * 
 * @param dhcp_idx    : DHCP instance index
 * @param client_ref  : client reference
 * @param client_info : client information pointer (output)
 * 
 * @return L7_RC_t : L7_SUCCESS - Client found 
 *                   L7_NOT_EXIST - Client does not exist
 *                   L7_FAILURE - Error
 */
static L7_RC_t ptin_dhcp_client_find(L7_uint dhcp_idx, ptin_client_id_t *client_ref, ptinDhcpClientInfoData_t **client_info)
{
  ptinDhcpClientDataKey_t avl_key;
  ptinDhcpClientsAvlTree_t *avl_tree;
  ptinDhcpClientInfoData_t *clientInfo;
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port;
  #endif

  /* Validate arguments */
  if (dhcp_idx>=PTIN_SYSTEM_N_DHCP_INSTANCES || client_ref==L7_NULLPTR)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate dhcp instance */
  if (!dhcpInstances[dhcp_idx].inUse)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"DHCP instance %u is not in use",dhcp_idx);
    return L7_FAILURE;
  }

  /* Get ptin_port value */
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  ptin_port = 0;
  if (client_ref->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    /* Convert to ptin_port format */
    if (ptin_intf_ptintf2port(&client_ref->ptin_intf,&ptin_port)!=L7_SUCCESS)
    {
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP,"Cannot convert client_ref intf %u/%u to ptin_port format",client_ref->ptin_intf.intf_type,client_ref->ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
  #endif

  /* Key to search for */
  avl_tree = &dhcpClients_unified.avlTree;

  memset(&avl_key,0x00,sizeof(ptinDhcpClientDataKey_t));

  avl_key.dhcp_instance = dhcp_idx;
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  avl_key.ptin_port = ptin_port;
  #endif
  #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
  avl_key.outerVlan = (client_ref->mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN) ? client_ref->outerVlan : 0;
  #endif
  #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
  avl_key.innerVlan = (client_ref->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) ? client_ref->innerVlan : 0;
  #endif
  #if (DHCP_CLIENT_IPADDR_SUPPORTED)
  avl_key.ipv4_addr = (client_ref->mask & PTIN_CLIENT_MASK_FIELD_IPADDR   ) ? client_ref->ipv4_addr : 0;
  #endif
  #if (DHCP_CLIENT_MACADDR_SUPPORTED)
  if (client_ref->mask & PTIN_CLIENT_MASK_FIELD_MACADDR)
    memcpy(avl_key.macAddr,client_ref->macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  else
    memset(avl_key.macAddr,0x00,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  #endif

  /* Search for this client */
  clientInfo = avlSearchLVL7( &(avl_tree->dhcpClientsAvlTree), (void *)&avl_key, AVL_EXACT);

  /* Check if this key already exists */
  if (clientInfo==L7_NULLPTR)
  {
    if (ptin_debug_dhcp_snooping)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Key {"
              #if (DHCP_CLIENT_INTERF_SUPPORTED)
                                "port=%u"
              #endif
              #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                                ",svlan=%u"
              #endif
              #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                                ",cvlan=%u"
              #endif
              #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                                ",ipAddr=%u.%u.%u.%u"
              #endif
              #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                                ",MacAddr=%02x:%02x:%02x:%02x:%02x:%02x"
              #endif
                                "} does not exist in dhcp_idx=%u",
              #if (DHCP_CLIENT_INTERF_SUPPORTED)
              avl_key.ptin_port,
              #endif
              #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
              avl_key.outerVlan,
              #endif
              #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
              avl_key.innerVlan,
              #endif
              #if (DHCP_CLIENT_IPADDR_SUPPORTED)
              (avl_key.ipv4_addr>>24) & 0xff, (avl_key.ipv4_addr>>16) & 0xff, (avl_key.ipv4_addr>>8) & 0xff, avl_key.ipv4_addr & 0xff,
              #endif
              #if (DHCP_CLIENT_MACADDR_SUPPORTED)
              avl_key.macAddr[0],avl_key.macAddr[1],avl_key.macAddr[2],avl_key.macAddr[3],avl_key.macAddr[4],avl_key.macAddr[5],
              #endif
              dhcp_idx);
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
 * Remove all DHCP clients
 * 
 * @param dhcp_idx : DHCP instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_dhcp_instance_deleteAll_clients(L7_uint dhcp_idx)
{
  ptinDhcpClientDataKey_t   avl_key;
  ptinDhcpClientInfoData_t *avl_info;
  L7_uint32 client_idx;

  /* Validate argument */
  if (dhcp_idx>=PTIN_SYSTEM_N_DHCP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid dhcp instance index (%u)",dhcp_idx);
    return L7_FAILURE;
  }
  /* DHCP instance must be in use */
  if (!dhcpInstances[dhcp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"dhcp instance index %u is not in use",dhcp_idx);
    return L7_FAILURE;
  }

  /* Run all cells in AVL tree related to this instance instance */
  memset(&avl_key,0x00,sizeof(ptinDhcpClientDataKey_t));
  while ( (avl_info = (ptinDhcpClientInfoData_t *)
                      avlSearchLVL7(&dhcpClients_unified.avlTree.dhcpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
     /* Prepare next key */
     memcpy(&avl_key, &avl_info->dhcpClientDataKey, sizeof(ptinDhcpClientDataKey_t));

     /* Skip items not belonging to this instance */
     if (avl_key.dhcp_instance != dhcp_idx)
       continue;

     /* Save client index */
     client_idx = avl_info->client_index;

     /* Delete node */
     if (avlDeleteEntry(&dhcpClients_unified.avlTree.dhcpClientsAvlTree, (void *)&avl_key) != L7_NULLPTR)
     {
       /* Release client index */
       dhcp_clientIndex_release(dhcp_idx, client_idx);
     }
  }

  LOG_TRACE(LOG_CTX_PTIN_DHCP,"Success removing all clients from dhcp_idx=%u",dhcp_idx);

  return L7_SUCCESS;
}

/**
 * Get DHCP instance from internal vlan
 * 
 * @param intVlan      : internal vlan
 * @param dhcpInst     : dhcp instance 
 * @param dhcpInst_idx : dhcp instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_dhcp_inst_get_fromIntVlan(L7_uint16 intVlan, st_DhcpInstCfg_t **dhcpInst, L7_uint *dhcpInst_idx)
{
  L7_uint32 evc_idx;
  L7_uint8  dhcp_idx;

  /* Verify if this internal vlan is associated to an EVC */
  if (ptin_evc_get_evcIdfromIntVlan(intVlan,&evc_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No EVC associated to internal vlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Check if the EVC has a DHCP instance */
  if (ptin_evc_dhcpInst_get(evc_idx, &dhcp_idx) != L7_SUCCESS ||
      dhcp_idx >= PTIN_SYSTEM_N_DHCP_INSTANCES)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No DHCP instance associated to evc_idx=%u (intVlan=%u)",evc_idx,intVlan);
    return L7_FAILURE;
  }

  /* Check if this instance is in use, and if evc_ids are valid */
  if (!dhcpInstances[dhcp_idx].inUse)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Inconsistency: DHCP index %u (EVCid=%u, Vlan %u) is not in use",dhcp_idx,evc_idx,intVlan);
    return L7_FAILURE;
  }

  /* Check if EVCs are in use */
  if (!ptin_evc_is_in_use(dhcpInstances[dhcp_idx].evc_idx))
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Inconsistency: DHCP index %u (EVCid=%u, Vlan %u) has EVC not in use (evc=%u)",dhcp_idx,evc_idx,intVlan,dhcpInstances[dhcp_idx].evc_idx);
    return L7_FAILURE;
  }

  /* Return dhcp instance */
  if (dhcpInst!=L7_NULLPTR)     *dhcpInst     = &dhcpInstances[dhcp_idx];
  if (dhcpInst_idx!=L7_NULLPTR) *dhcpInst_idx = dhcp_idx;

  return L7_SUCCESS;
}

/**
 * Gets a free DHCP instance entry
 * 
 * @param dhcp_idx : Output index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_dhcp_instance_find_free(L7_uint *dhcp_idx)
{
  L7_uint idx;

  /* Search for the first empty instance */
  for (idx=0; idx<PTIN_SYSTEM_N_DHCP_INSTANCES && dhcpInstances[idx].inUse; idx++);

  /* If not found empty instances, return error */
  if (idx>=PTIN_SYSTEM_N_DHCP_INSTANCES)
    return L7_FAILURE;

  /* Return instance index */
  if (dhcp_idx!=L7_NULLPTR)  *dhcp_idx = idx;

  return L7_SUCCESS;
}

/**
 * Gets the DHCP instance with a specific Ucast EVC ids 
 * 
 * @param evc_idx : Unicast EVC id
 * @param dhcp_idx   : DHCP instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_dhcp_instance_find(L7_uint32 evc_idx, L7_uint *dhcp_idx)
{
  #if 1
  L7_uint8 dhcp_inst;

  /* Validate evc index */
  if (evc_idx >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    return L7_FAILURE;
  }
  /* Check if there is an instance associated to this EVC */
  if (ptin_evc_dhcpInst_get(evc_idx, &dhcp_inst) != L7_SUCCESS ||
      dhcp_inst >= PTIN_SYSTEM_N_DHCP_INSTANCES)
  {
    return L7_FAILURE;
  }

  /* Return index */
  if (dhcp_idx!=L7_NULLPTR)  *dhcp_idx = dhcp_inst;

  return L7_SUCCESS;
  #else
  L7_uint idx;

  /* Search for the provided Mcast and Ucast evcs */
  for (idx=0; idx<PTIN_SYSTEM_N_DHCP_INSTANCES; idx++)
  {
    if (!dhcpInstances[idx].inUse)  continue;

    if (dhcpInstances[idx].evc_idx==evc_idx)
      break;
  }

  /* If not found empty instances, return error */
  if (idx>=PTIN_SYSTEM_N_DHCP_INSTANCES)
    return L7_FAILURE;

  /* Return instance index */
  if (dhcp_idx!=L7_NULLPTR)  *dhcp_idx = idx;

  return L7_SUCCESS;
  #endif
}

static L7_RC_t ptin_dhcp_trap_configure(L7_uint dhcp_idx, L7_BOOL enable, L7_uint8 family)
{
  /* Validate argument */
  if (dhcp_idx>=PTIN_SYSTEM_N_DHCP_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid dhcp instance index (%u)",dhcp_idx);
    return L7_FAILURE;
  }
  /* DHCP instance must be in use */
  if (!dhcpInstances[dhcp_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"dhcp instance index %u is not in use",dhcp_idx);
    return L7_FAILURE;
  }

  return ptin_dhcp_evc_trap_configure(dhcpInstances[dhcp_idx].evc_idx, enable, family);
}

/**
 * Configure VLAN trap regarding to one EVC
 * 
 * @param evc_idx : EVC index
 * @param enable : Enable
 * @param family : IP family
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_dhcp_evc_trap_configure(L7_uint32 evc_idx, L7_BOOL enable, L7_uint8 family)
{
  L7_uint16 vlan;

  enable &= 1;

  if (ptin_evc_intRootVlan_get(evc_idx, &vlan) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Can't get UC root vlan for evc id %u",evc_idx);
    return L7_FAILURE;
  }

#if (PTIN_QUATTRO_FLOWS_FEATURE_ENABLED && QUATTRO_DHCP_TRAP_PREACTIVE)
  if (!PTIN_VLAN_IS_QUATTRO(vlan))
#endif
  {
    if (ptin_dhcpPkts_vlan_trap(vlan, enable, family)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error configuring vlan %u for packet trapping", vlan);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_DHCP,"Success configuring vlan %u for packet trapping", vlan);
  }

  return L7_SUCCESS;
}


void ptin_dhcp_evc_ethprty_get(ptin_AccessNodeCircuitId_t *evc_circuitid, L7_uint8 *ethprty)
{
   if(L7_NULLPTR == evc_circuitid || L7_NULLPTR == ethprty)
   {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
   }

   *ethprty = evc_circuitid->ethernet_priority;
}


#define CIRCUITID_TEMPLATE_MAX_STRING_TMP   (CIRCUITID_TEMPLATE_MAX_STRING*2)

void ptin_dhcp_circuitId_build(ptin_AccessNodeCircuitId_t *evc_circuitid, ptin_clientCircuitId_t *client_circuitid, L7_char8 *circuitid)
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

  ptin_dhcp_circuitid_convert(temp_str, CIRCUITID_ACCESSNODEID_STR,     evc_circuitid->access_node_id);
  ptin_dhcp_circuitid_convert(temp_str, CIRCUITID_CHASSIS_STR,          chassis);
  ptin_dhcp_circuitid_convert(temp_str, CIRCUITID_RACK_STR,             rack);
  ptin_dhcp_circuitid_convert(temp_str, CIRCUITID_FRAME_STR,            frame);
  ptin_dhcp_circuitid_convert(temp_str, CIRCUITID_ETHERNETPRIORITY_STR, ethernet_priority);
  ptin_dhcp_circuitid_convert(temp_str, CIRCUITID_S_VID_STR,            s_vid);
  ptin_dhcp_circuitid_convert(temp_str, CIRCUITID_ONUID_STR,            onuid);
  ptin_dhcp_circuitid_convert(temp_str, CIRCUITID_SLOT_STR,             slot);
  ptin_dhcp_circuitid_convert(temp_str, CIRCUITID_PORT_STR,             port);
  ptin_dhcp_circuitid_convert(temp_str, CIRCUITID_Q_VID_STR,            q_vid);
  ptin_dhcp_circuitid_convert(temp_str, CIRCUITID_C_VID_STR,            c_vid);

  strncpy(circuitid, temp_str, FD_DS_MAX_REMOTE_ID_STRING);
  circuitid[FD_DS_MAX_REMOTE_ID_STRING-1] = '\0';
}

void ptin_dhcp_circuitid_convert(L7_char8 *circuitid_str, L7_char8 *str_to_replace, L7_char8 *parameter)
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

/**
 * Reconfigure global DHCP EVC (using instance id)
 *
 * @param dhcp_instance_idx : dhcp instance index
 * @param dhcp_flag         : DHCP flag (not used)
 * @param options           : options
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_reconf_instance(L7_uint32 dhcp_instance_idx, L7_uint8 dhcp_flag, L7_uint32 options)
{
   struct ptin_clientInfo_entry_s *clientInfo_entry;

   /* Validate dhcp instance */
   if (!dhcpInstances[dhcp_instance_idx].inUse)
   {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP instance %u is not in use", dhcp_instance_idx);
    return L7_FAILURE;
   }

   /* Save EVC DHCP Options */
   dhcpInstances[dhcp_instance_idx].evcDhcpOptions = options;

   /* Run all instance belonging clients */
   clientInfo_entry = NULL;
   dl_queue_get_head(&dhcpInstances[dhcp_instance_idx].queue_clients, (dl_queue_elem_t **)&clientInfo_entry);
   while (clientInfo_entry != NULL)
   {
     /* Reconfigure DHCP options for clients that are using Global EVC DHCP options */
     if(clientInfo_entry->client_info != L7_NULLPTR &&
        L7_TRUE == clientInfo_entry->client_info->client_data.useEvcDhcpOptions)
     {
        clientInfo_entry->client_info->client_data.dhcp_options = options;
     }

     /* Next queue element */
     clientInfo_entry = (struct ptin_clientInfo_entry_s *) dl_queue_get_next(&dhcpInstances[dhcp_instance_idx].queue_clients, (dl_queue_elem_t *) clientInfo_entry);
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
void ptin_dhcp_dump(L7_BOOL show_clients)
{
  L7_uint i, i_client;
  ptinDhcpClientInfoData_t *avl_info;
  struct ptin_clientInfo_entry_s *clientInfo_entry;

  for (i = 0; i < PTIN_SYSTEM_N_DHCP_INSTANCES; i++)
  {
    if (!dhcpInstances[i].inUse) {
      printf("*** Dhcp instance %02u not in use\r\n", i);
      continue;
    }

    printf("DHCP instance %02u: EVC_idx=%-5u NNI_VLAN=%-4u #evcs=%-5u options=0x%04x [CircuitId Template: %s]  ", i,
           dhcpInstances[i].evc_idx, dhcpInstances[i].nni_ovid, dhcpInstances[i].n_evcs,
           dhcpInstances[i].evcDhcpOptions, dhcpInstances[i].circuitid.template_str);
    printf("\r\n");

    if (show_clients)
    {
      i_client = 0;

      /* Run all instance belonging clients */
      clientInfo_entry = NULL;
      dl_queue_get_head(&dhcpInstances[i].queue_clients, (dl_queue_elem_t **)&clientInfo_entry);
      while (clientInfo_entry != NULL)
      {
        avl_info = clientInfo_entry->client_info;

        if (avl_info != L7_NULLPTR)
        {
          printf("   Client#%-5u: "
                 #if (DHCP_CLIENT_INTERF_SUPPORTED)
                 "ptin_port=%-2u "
                 #endif
                 #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                 "svlan=%-4u "
                 #endif
                 #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                 "cvlan=%-4u "
                 #endif
                 #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                 "IP=%03u.%03u.%03u.%03u "
                 #endif
                 #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                 "MAC=%02x:%02x:%02x:%02x:%02x:%02x "
                 #endif
                 ": [uni_vlans=%4u+%-4u] options=0x%04x circuitId=\"%s\" remoteId=\"%s\"\r\n",
                 avl_info->client_index,
                 #if (DHCP_CLIENT_INTERF_SUPPORTED)
                 avl_info->dhcpClientDataKey.ptin_port,
                 #endif
                 #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
                 avl_info->dhcpClientDataKey.outerVlan,
                 #endif
                 #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
                 avl_info->dhcpClientDataKey.innerVlan,
                 #endif
                 #if (DHCP_CLIENT_IPADDR_SUPPORTED)
                 (avl_info->dhcpClientDataKey.ipv4_addr>>24) & 0xff,
                  (avl_info->dhcpClientDataKey.ipv4_addr>>16) & 0xff,
                   (avl_info->dhcpClientDataKey.ipv4_addr>>8) & 0xff,
                    avl_info->dhcpClientDataKey.ipv4_addr & 0xff,
                 #endif
                 #if (DHCP_CLIENT_MACADDR_SUPPORTED)
                 avl_info->dhcpClientDataKey.macAddr[0],
                  avl_info->dhcpClientDataKey.macAddr[1],
                   avl_info->dhcpClientDataKey.macAddr[2],
                    avl_info->dhcpClientDataKey.macAddr[3],
                     avl_info->dhcpClientDataKey.macAddr[4],
                      avl_info->dhcpClientDataKey.macAddr[5],
                 #endif
                 avl_info->uni_ovid, avl_info->uni_ivid,
                 avl_info->client_data.dhcp_options,
                 avl_info->client_data.circuitId_str,
                 avl_info->client_data.remoteId_str);
        }
        else
        {
          printf("   Entry %u has a null pointer\r\n", i_client);
        }

        i_client++;

        /* Next queue element */
        clientInfo_entry = (struct ptin_clientInfo_entry_s *) dl_queue_get_next(&dhcpInstances[i].queue_clients, (dl_queue_elem_t *) clientInfo_entry);
      }
    }
  }

  printf("Total number of DHCP clients: %u\r\n", dhcpClients_unified.number_of_clients);
  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  printf("Total number of QUATTRO-STACKED evcs: %u\r\n", dhcp_quattro_stacked_evcs);
  #endif

  fflush(stdout);
}


void ptin_dhcpClients_dump(void)
{
  L7_int i_client = 0;
  ptinDhcpClientDataKey_t avl_key;
  ptinDhcpClientInfoData_t *avl_info;

  printf("Listing complete list of DHCP clients:\r\n");

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinDhcpClientDataKey_t));
  while ( ( avl_info = (ptinDhcpClientInfoData_t *)
                        avlSearchLVL7(&dhcpClients_unified.avlTree.dhcpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->dhcpClientDataKey, sizeof(ptinDhcpClientDataKey_t));

    printf("   Client#%-5u: Inst=%-2u "
           #if (DHCP_CLIENT_INTERF_SUPPORTED)
           "ptin_port=%-2u "
           #endif
           #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
           "svlan=%-4u "
           #endif
           #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
           "cvlan=%-4u "
           #endif
           #if (DHCP_CLIENT_IPADDR_SUPPORTED)
           "IP=%03u.%03u.%03u.%03u "
           #endif
           #if (DHCP_CLIENT_MACADDR_SUPPORTED)
           "MAC=%02x:%02x:%02x:%02x:%02x:%02x "
           #endif
           ": [uni_vlans=%4u+%-4u] options=0x%04x circuitId=\"%s\" remoteId=\"%s\"\r\n",
           avl_info->client_index,
           avl_info->dhcpClientDataKey.dhcp_instance,
           #if (DHCP_CLIENT_INTERF_SUPPORTED)
           avl_info->dhcpClientDataKey.ptin_port,
           #endif
           #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
           avl_info->dhcpClientDataKey.outerVlan,
           #endif
           #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
           avl_info->dhcpClientDataKey.innerVlan,
           #endif
           #if (DHCP_CLIENT_IPADDR_SUPPORTED)
           (avl_info->dhcpClientDataKey.ipv4_addr>>24) & 0xff,
            (avl_info->dhcpClientDataKey.ipv4_addr>>16) & 0xff,
             (avl_info->dhcpClientDataKey.ipv4_addr>>8) & 0xff,
              avl_info->dhcpClientDataKey.ipv4_addr & 0xff,
           #endif
           #if (DHCP_CLIENT_MACADDR_SUPPORTED)
           avl_info->dhcpClientDataKey.macAddr[0],
            avl_info->dhcpClientDataKey.macAddr[1],
             avl_info->dhcpClientDataKey.macAddr[2],
              avl_info->dhcpClientDataKey.macAddr[3],
               avl_info->dhcpClientDataKey.macAddr[4],
                avl_info->dhcpClientDataKey.macAddr[5],
           #endif
           avl_info->uni_ovid, avl_info->uni_ivid,
           avl_info->client_data.dhcp_options,
           avl_info->client_data.circuitId_str,
           avl_info->client_data.remoteId_str);

    i_client++;
  }

  printf("Total number of DHCP clients: %u\r\n", dhcpClients_unified.number_of_clients);
  fflush(stdout);
}


#if 0
#define DHCP_RELAY_DATABASE_MAX_ENTRIES 1024

#define DHCP_INTERFACE_FIELD_MASK       0x01
#define DHCP_INTERNAL_VLANID_FIELD_MASK 0x02
#define DHCP_INNER_VLANID_FIELD_MASK    0x04
#define DHCP_MAC_ADDRESS_FIELD_MASK     0x08

/********************************************* 
 * Typedefs
 *********************************************/

typedef struct _st_dhcp_relay_database {
  L7_uint8  active;
  struct {
    L7_uint32 intIfNum;       /* Interface number */
    L7_uint16 vlanId;         /* Internal vlan */
    L7_uint16 innerVlanId;    /* Inner vlan (client) */
    L7_enetMacAddr_t macAddr; /* MAC address */
  } params;
  struct {
    L7_char8 circuitId[FD_DS_MAX_REMOTE_ID_STRING];   /* Circuit ID string */
    L7_char8 remoteId[FD_DS_MAX_REMOTE_ID_STRING];    /* Remote ID string */
  } strings;
} st_dhcp_relay_database;

/********************************************* 
 * Internal variables
 *********************************************/

st_dhcp_relay_database dhcp_relay_database[DHCP_RELAY_DATABASE_MAX_ENTRIES];
 
 
/********************************************* 
 * Static function prototypes
 *********************************************/

/**
 * Add a circuitId+remoteId entry in database
 * 
 * @param intIfNum    : interface
 * @param intVlanId   : internal vlan
 * @param innerVlanId : client vlan 
 * @param macAddr     : MAC Address  
 * @param circuitId   : circuit id
 * @param remoteId    : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_dhcp82_database_add(L7_uint32 intIfNum, L7_uint16 intVlanId, L7_uint16 innerVlanId, L7_uint8 *macAddr,
                                        L7_char8 *circuitId, L7_char8 *remoteId);

/**
 * Remove a circuitId+remoteId entry in database
 * 
 * @param intIfNum    : interface
 * @param intVlanId   : internal vlan
 * @param innerVlanId : client vlan 
 * @param macAddr     : MAC Address  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_dhcp82_database_remove(L7_uint32 intIfNum, L7_uint16 intVlanId, L7_uint16 innerVlanId, L7_uint8 *macAddr);

/**
 * Get interface data (intIfNum and internal Vlan), from EVC id 
 * and ptin_intf index 
 * 
 * @param evc_idx   : evc id
 * @param ptin_intf : ptin interface #
 * @param intIfNum  : FP interface#
 * @param intVlanId : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_dhcp82_get_intfData(L7_uint32 evc_idx, ptin_intf_t *ptin_intf, L7_uint32 *intIfNum, L7_uint16 *intVlanId);

/**
 * Validate and correct input parameters for access in database
 * 
 * @param intIfNum    : interface#
 * @param intVlanId   : internal vlan
 * @param innerVlanId : inner vlan
 * @param enetMacAddr : Mac Address
 * 
 * @return L7_RC_t : L7_TRUE if inputs are valid 
 *                   L7_FALSE if inputs are not valid 
 */
static L7_BOOL ptin_dhcp82_validate_inputs(L7_uint32 *intIfNum, L7_uint16 *intVlanId, L7_uint16 *innerVlanId, L7_enetMacAddr_t *enetMacAddr); 
 
/**
 * Read DHCP Option 82 entry
 * 
 * @param evc_idx     : EVC id
 * @param ptin_intf   : Interface type and id
 * @param innerVlanId : client vlan 
 * @param macAddr     : MAC Address  
 * @param circuitId   : circuit id
 * @param remoteId    : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp82_get(L7_uint32 evc_idx, ptin_intf_t *ptin_intf, L7_uint16 innerVlanId, L7_uint8 *macAddr,
                        L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_uint32 intIfNum;
  L7_uint16 intVlanId;

  /* Get intIfNum and internal vlan */
  if (ptin_dhcp82_get_intfData(evc_idx, ptin_intf, &intIfNum, &intVlanId)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error getting intIfNum or internal Vlan");
    return L7_FAILURE;
  }

  /* Add circuitId and remoteId */
  if (ptin_dhcp82_database_get(intIfNum,intVlanId,innerVlanId,macAddr,circuitId,remoteId)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error reading circuitId+remoteId entry");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Configure DHCP Option 82
 * 
 * @param evc_idx     : EVC id
 * @param ptin_intf   : Interface type and id
 * @param innerVlanId : client vlan 
 * @param macAddr     : MAC Address  
 * @param circuitId   : circuit id
 * @param remoteId    : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp82_config(L7_uint32 evc_idx, ptin_intf_t *ptin_intf, L7_uint16 innerVlanId, L7_uint8 *macAddr,
                           L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_uint32 intIfNum;
  L7_uint16 intVlanId;

  /* Get intIfNum and internal vlan */
  if (ptin_dhcp82_get_intfData(evc_idx, ptin_intf, &intIfNum, &intVlanId)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error getting intIfNum or internal Vlan");
    return L7_FAILURE;
  }

  /* Add circuitId and remoteId */
  if (ptin_dhcp82_database_add(intIfNum,intVlanId,innerVlanId,L7_NULLPTR,circuitId,remoteId)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error adding circuitId+remoteId entry");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Unconfigure DHCP Option 82
 * 
 * @param evc_idx     : EVC id
 * @param ptin_intf   : Interface type and id
 * @param innerVlanId : client vlan 
 * @param macAddr     : MAC Address  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp82_unconfig(L7_uint32 evc_idx, ptin_intf_t *ptin_intf, L7_uint16 innerVlanId, L7_uint8 *macAddr)
{
  L7_uint32 intIfNum;
  L7_uint16 intVlanId;

  /* Get intIfNum and internal vlan */
  if (ptin_dhcp82_get_intfData(evc_idx, ptin_intf, &intIfNum, &intVlanId)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error getting intIfNum or internal Vlan");
    return L7_FAILURE;
  }

  /* Remove circuitId and remoteId */
  if (ptin_dhcp82_database_remove(intIfNum,intVlanId,innerVlanId,L7_NULLPTR)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error removing circuitId+remoteId entry");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get a circuitId+remoteId entry in database
 * 
 * @param intIfNum    : interface
 * @param intVlanId   : internal vlan
 * @param innerVlanId : client vlan 
 * @param macAddr     : MAC Address 
 * @param circuitId   : circuit id
 * @param remoteId    : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp82_database_get(L7_uint32 intIfNum, L7_uint16 intVlanId, L7_uint16 innerVlanId, L7_uint8 *macAddr,
                                 L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_int i;
  L7_enetMacAddr_t enetMacAddr;

  if ( macAddr==L7_NULLPTR)
    memset(enetMacAddr.addr,0x00,sizeof(L7_uchar8)*L7_ENET_MAC_ADDR_LEN);
  else
    memcpy(enetMacAddr.addr,macAddr,sizeof(L7_uchar8)*L7_ENET_MAC_ADDR_LEN);

  /* Validate inputs */
  if (!ptin_dhcp82_validate_inputs(&intIfNum, &intVlanId, &innerVlanId, &enetMacAddr))
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"No valid input fields provided");
    return L7_FAILURE;
  }

  // Find the position with the referred inputs
  for (i=0; i<DHCP_RELAY_DATABASE_MAX_ENTRIES; i++)
  {
    // If this entry is not active, there is nothing to do
    if (!dhcp_relay_database[i].active)  continue;

    // if this entry already exists, break loop
    if (dhcp_relay_database[i].params.intIfNum    == intIfNum &&
        dhcp_relay_database[i].params.vlanId      == intVlanId &&
        dhcp_relay_database[i].params.innerVlanId == innerVlanId &&
        memcmp(&dhcp_relay_database[i].params.macAddr, &enetMacAddr, sizeof(L7_enetMacAddr_t))==0 )
      break;
  }

  // Entry not found... leave function
  if (i>=DHCP_RELAY_DATABASE_MAX_ENTRIES)
  {
    if (circuitId!=L7_NULLPTR)  *circuitId='\0';
    if (remoteId!=L7_NULLPTR)   *remoteId ='\0';
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"Not found entry with the provided inputs (intIfNum=%u, intVlanId=%u, innerVlanId=%u)",intIfNum,intVlanId,innerVlanId);
    return L7_FAILURE;
  }

  // Fill strings with data
  if (circuitId!=L7_NULLPTR)
    memcpy(circuitId,dhcp_relay_database[i].strings.circuitId,sizeof(L7_char8)*FD_DS_MAX_REMOTE_ID_STRING);
  if (remoteId!=L7_NULLPTR)
    memcpy(remoteId,dhcp_relay_database[i].strings.remoteId,sizeof(L7_char8)*FD_DS_MAX_REMOTE_ID_STRING);

  return L7_SUCCESS;
}

/********************************************* 
 * Static functions implementation
 *********************************************/

/**
 * Add a circuitId+remoteId entry in database
 * 
 * @param intIfNum    : interface
 * @param intVlanId   : internal vlan
 * @param innerVlanId : client vlan 
 * @param macAddr     : MAC Address 
 * @param circuitId   : circuit id
 * @param remoteId    : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_dhcp82_database_add(L7_uint32 intIfNum, L7_uint16 intVlanId, L7_uint16 innerVlanId, L7_uint8 *macAddr,
                                        L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_int i, free_pos=-1;
  L7_enetMacAddr_t enetMacAddr;

  if ( macAddr==L7_NULLPTR)
    memset(enetMacAddr.addr,0x00,sizeof(L7_uchar8)*L7_ENET_MAC_ADDR_LEN);
  else
    memcpy(enetMacAddr.addr,macAddr,sizeof(L7_uchar8)*L7_ENET_MAC_ADDR_LEN);

  /* Validate inputs */
  if (!ptin_dhcp82_validate_inputs(&intIfNum, &intVlanId, &innerVlanId, &enetMacAddr))
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"No valid input fields provided");
    return L7_FAILURE;
  }

  // Find a free position
  for (i=0; i<DHCP_RELAY_DATABASE_MAX_ENTRIES; i++)
  {
    // Find the first free position
    if (free_pos<0 && !dhcp_relay_database[i].active)
      free_pos = i;

    // If this entry is not active, there is nothing to do
    if (!dhcp_relay_database[i].active)  continue;

    // if this entry already exists, break loop
    if ( dhcp_relay_database[i].params.intIfNum    == intIfNum &&
         dhcp_relay_database[i].params.vlanId      == intVlanId &&
         dhcp_relay_database[i].params.innerVlanId == innerVlanId &&
         memcmp(&dhcp_relay_database[i].params.macAddr, &enetMacAddr, sizeof(L7_enetMacAddr_t))==0 )
      break;
  }

  // Entry not found, and no free entries... leave function
  if (i>=DHCP_RELAY_DATABASE_MAX_ENTRIES && free_pos<0)
    return L7_FAILURE;

  // If not found a configured entry with these parameters, use an empty position
  if (i>=DHCP_RELAY_DATABASE_MAX_ENTRIES)
    i = free_pos;

  // Fill pos with data
  dhcp_relay_database[i].params.intIfNum    = intIfNum;
  dhcp_relay_database[i].params.vlanId      = intVlanId;
  dhcp_relay_database[i].params.innerVlanId = innerVlanId;
  memcpy(&dhcp_relay_database[i].params.macAddr,&enetMacAddr,sizeof(L7_enetMacAddr_t));

  if (circuitId!=L7_NULLPTR)
    memcpy(dhcp_relay_database[i].strings.circuitId,circuitId,sizeof(L7_char8)*FD_DS_MAX_REMOTE_ID_STRING);
  if (remoteId!=L7_NULLPTR)
    memcpy(dhcp_relay_database[i].strings.remoteId,remoteId,sizeof(L7_char8)*FD_DS_MAX_REMOTE_ID_STRING);

  dhcp_relay_database[i].active = L7_TRUE;

  return L7_SUCCESS;
}

/**
 * Remove a circuitId+remoteId entry in database
 * 
 * @param intIfNum    : interface
 * @param intVlanId   : internal vlan
 * @param innerVlanId : client vlan 
 * @param macAddr     : MAC Address
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_dhcp82_database_remove(L7_uint32 intIfNum, L7_uint16 intVlanId, L7_uint16 innerVlanId, L7_uint8 *macAddr)
{
  L7_int i;
  L7_enetMacAddr_t enetMacAddr;

  if ( macAddr==L7_NULLPTR)
    memset(enetMacAddr.addr,0x00,sizeof(L7_uchar8)*L7_ENET_MAC_ADDR_LEN);
  else
    memcpy(enetMacAddr.addr,macAddr,sizeof(L7_uchar8)*L7_ENET_MAC_ADDR_LEN);

  /* Validate inputs */
  if (!ptin_dhcp82_validate_inputs(&intIfNum, &intVlanId, &innerVlanId, &enetMacAddr))
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"No valid input fields provided");
    return L7_FAILURE;
  }

  // Find in database
  for (i=0; i<DHCP_RELAY_DATABASE_MAX_ENTRIES; i++)
  {
    // If this entry is not active, there is nothing to do
    if (!dhcp_relay_database[i].active)  continue;

    // if this entry exists, break loop
    if ( dhcp_relay_database[i].params.intIfNum    == intIfNum &&
         dhcp_relay_database[i].params.vlanId      == intVlanId &&
         dhcp_relay_database[i].params.innerVlanId == innerVlanId &&
         memcmp(&dhcp_relay_database[i].params.macAddr, &enetMacAddr, sizeof(L7_enetMacAddr_t))==0 )
      break;
  }

  // Entry not found... leave function... return success
  if (i>=DHCP_RELAY_DATABASE_MAX_ENTRIES)
  {
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"Not found entry");
    return L7_SUCCESS;
  }

  // Fill pos with data
  dhcp_relay_database[i].params.intIfNum    = 0;
  dhcp_relay_database[i].params.vlanId      = 0;
  dhcp_relay_database[i].params.innerVlanId = 0;
  memset(&dhcp_relay_database[i].params.macAddr,0x00,sizeof(L7_enetMacAddr_t));

  memset(dhcp_relay_database[i].strings.circuitId,0x00,sizeof(L7_char8)*FD_DS_MAX_REMOTE_ID_STRING);
  memset(dhcp_relay_database[i].strings.remoteId,0x00,sizeof(L7_char8)*FD_DS_MAX_REMOTE_ID_STRING);

  dhcp_relay_database[i].active = L7_FALSE;

  return L7_SUCCESS;
}

/**
 * Get interface data (intIfNum and internal Vlan), from EVC id 
 * and ptin_intf index 
 * 
 * @param evc_idx   : evc id
 * @param ptin_intf : ptin interface #
 * @param intIfNum  : FP interface#
 * @param intVlanId : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_dhcp82_get_intfData(L7_uint32 evc_idx, ptin_intf_t *ptin_intf, L7_uint32 *intIfNum, L7_uint16 *intVlanId)
{
  ptin_evc_intfCfg_t intf_cfg;

  /* Extract interface configuration */
  if (ptin_evc_intfCfg_get(evc_idx, ptin_intf, &intf_cfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error getting interface configuration for evc_idx=%u, intf=%u/%u",evc_idx,ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Check if interface is in use */
  if (!intf_cfg.in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Interface %u/%u is not in use in evc_idx=%u",ptin_intf->intf_type,ptin_intf->intf_id,evc_idx);
    return L7_FAILURE;
  }

  /* Get interface# */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Caanot calculate intIfNum (evc_idx=%u, intf=%u/%u)",evc_idx,ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Validate internal vlan */
  if (intf_cfg.int_vlan<PTIN_VLAN_MIN || intf_cfg.int_vlan>PTIN_VLAN_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"intVlan %u is not valid (evc_idx=%u, intf=%u/%u)",intf_cfg.int_vlan,evc_idx,ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  /* Get internal vlan */
  if (intVlanId!=L7_NULLPTR)  *intVlanId = intf_cfg.int_vlan;

  return L7_SUCCESS;
}

/**
 * Validate and correct input parameters for access in database
 * 
 * @param intIfNum    : interface#
 * @param intVlanId   : internal vlan
 * @param innerVlanId : inner vlan
 * @param enetMacAddr : Mac Address
 * 
 * @return L7_RC_t : L7_TRUE if inputs are valid 
 *                   L7_FALSE if inputs are not valid 
 */
static L7_BOOL ptin_dhcp82_validate_inputs(L7_uint32 *intIfNum, L7_uint16 *intVlanId, L7_uint16 *innerVlanId, L7_enetMacAddr_t *enetMacAddr)
{
  L7_BOOL valid_inputs = L7_FALSE;

  /* Interface */
  if (intIfNum!=L7_NULLPTR)
  {
    if (*intIfNum>=L7_MAX_INTERFACE_COUNT)
      *intIfNum = 0;
    if (*intIfNum!=0)
      valid_inputs = L7_TRUE;
  }
  /* Internal vlan */
  if (intVlanId!=L7_NULLPTR)
  {
    if (*intVlanId<PTIN_VLAN_MIN || *intVlanId>PTIN_VLAN_MAX)
      *intVlanId = 0;
    if (*intVlanId!=0)
      valid_inputs = L7_TRUE;
  }
  /* Inner vlan */
  if (innerVlanId!=L7_NULLPTR)
  {
    if (*innerVlanId>=4096)
      *innerVlanId = 0;
    if (*innerVlanId!=0)
      valid_inputs = L7_TRUE;
  }
  /* MAC address */
  if (enetMacAddr!=L7_NULLPTR)
  {
    if ( enetMacAddr->addr[0]!=0x00 ||
         enetMacAddr->addr[1]!=0x00 ||
         enetMacAddr->addr[2]!=0x00 ||
         enetMacAddr->addr[3]!=0x00 ||
         enetMacAddr->addr[4]!=0x00 ||
         enetMacAddr->addr[5]!=0x00   )
      valid_inputs = L7_TRUE;
  }

  return valid_inputs;
}
#endif

#if DHCP_ACCEPT_UNSTACKED_PACKETS
static L7_RC_t ptin_dhcp_strings_def_get(ptin_intf_t *ptin_intf, L7_uchar8 *macAddr, L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_uint slot = 0;
  L7_uint port_type = 0;
  L7_uint port_id = 0;

  /* Circuit id */
  if (circuitId!=L7_NULLPTR)
  {
    slot = ptin_fpga_board_slot();

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
static L7_RC_t ptin_dhcp_clientId_convert(L7_uint32 evc_idx, ptin_client_id_t *client)
{
  L7_uint16 intVlan, innerVlan;

  /* Validate evc index  */
  if (evc_idx>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid eEVC id: evc_idx=%u",evc_idx);
    return L7_FAILURE;
  }
  /* This evc must be active */
  if (!ptin_evc_is_in_use(evc_idx))
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"eEVC id is not active: evc_idx=%u",evc_idx);
    return L7_FAILURE;
  }
  /* Validate client */
  if (client==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid client pointer");
    return L7_FAILURE;
  }

  /* Check mask */
  if (DHCP_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"Client mask is null");
    return L7_FAILURE;
  }

  innerVlan = 0;
  /* Validate inner vlan */
  #if DHCP_CLIENT_INNERVLAN_SUPPORTED
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN)
  {
    /* Validate inner vlan */
    if (client->innerVlan>4095)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid inner vlan (%u)",client->innerVlan);
      return L7_FAILURE;
    }
    innerVlan = client->innerVlan;
  }
  #endif

  /* Update outer vlan */
  #if defined(DHCP_CLIENT_INTERF_SUPPORTED) && defined(DHCP_CLIENT_OUTERVLAN_SUPPORTED)
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
        LOG_ERR(LOG_CTX_PTIN_DHCP,"Error obtaining internal vlan for evc_idx=%u, ptin_intf=%u/%u",
                evc_idx, client->ptin_intf.intf_type, client->ptin_intf.intf_id);
        return L7_FAILURE;
      }
    }
    else
    {
      /* Obtain intVlan from the outer vlan */
      if (ptin_evc_intVlan_get_fromOVlan(&client->ptin_intf, client->outerVlan, innerVlan, &intVlan)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_DHCP,"Error obtaining internal vlan for OVid=%u, IVid=%u, ptin_intf=%u/%u",
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
static L7_RC_t ptin_dhcp_clientId_restore(ptin_client_id_t *client)
{
  L7_uint32 intIfNum;
  L7_uint16 extVlan, innerVlan;

  /* Validate client */
  if (client==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Check mask */
  if (DHCP_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"Client mask is null");
    return L7_FAILURE;
  }

  innerVlan = 0;
  #if DHCP_CLIENT_INNERVLAN_SUPPORTED
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN)
  {
    /* Validate inner vlan */
    if (client->innerVlan>4095)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid inner vlan (%u)",client->innerVlan);
      return L7_FAILURE;
    }
    innerVlan = client->innerVlan;
  }
  #endif

  #if defined(DHCP_CLIENT_INTERF_SUPPORTED) && defined(DHCP_CLIENT_OUTERVLAN_SUPPORTED)
  /* Is interface and outer vlan provided? If so, replace it with the internal vlan */
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF &&
      client->mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN)
  {
    /* Convert to intIfNum format */
    if (ptin_intf_ptintf2intIfNum(&client->ptin_intf, &intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Cannot convert client intf %u/%u to intIfNum format",
              client->ptin_intf.intf_type,client->ptin_intf.intf_id);
      return L7_FAILURE;
    }

    /* Validate outer vlan */
    if (client->outerVlan<PTIN_VLAN_MIN || client->outerVlan>PTIN_VLAN_MAX)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid outer vlan (%u)",client->outerVlan);
      return L7_FAILURE;
    }
    /* Replace the outer vlan, with the internal vlan relative to the leaf interface */
    if (ptin_evc_extVlans_get_fromIntVlan(intIfNum, client->outerVlan, innerVlan, &extVlan, L7_NULLPTR)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Could not obtain external vlan for intVlan %u, ptin_intf %u/%u",
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

