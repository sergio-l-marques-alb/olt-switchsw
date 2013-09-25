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
#include "ptin_intf.h"
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
#define PPPOE_CLIENT_OUTERVLAN_SUPPORTED 0
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
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  L7_uint8  ptin_port;                /* PTin port, which is attached */
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
  ptin_PPPOE_Statistics_t  client_stats;   /* Client statistics   */
  void *next;
} ptinPppoeClientInfoData_t;

typedef struct {
    avlTree_t                 pppoeClientsAvlTree;
    avlTreeTables_t           *pppoeClientsTreeHeap;
    ptinPppoeClientInfoData_t  *pppoeClientsDataHeap;
} ptinPppoeClientsAvlTree_t;

/* PPPOE AVL Tree data */
typedef struct {
  L7_uint16                number_of_clients;
  ptinPppoeClientInfoData_t *clients_in_use[PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE];
  ptinPppoeClientsAvlTree_t avlTree;
} ptinPppoeClients_t;

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
  L7_uint16                   UcastEvcId;
  L7_uint16                   nni_ovid;          /* NNI outer vlan */
  ptinPppoeClients_t          pppoeClients;
  L7_uint16                   evcPppoeOptions;   /* PPPOE Options (0x01=Option82; 0x02=Option37; 0x02=Option18) */
  ptin_PPPOE_Statistics_t     stats_intf[PTIN_SYSTEM_N_INTERF];  /* PPPOE statistics at interface level */
  ptin_AccessNodeCircuitId_t  circuitid;
} st_PppoeInstCfg_t;

/*********************************************************** 
 * Data structs
 ***********************************************************/

/* PPPOE instances array */
st_PppoeInstCfg_t  pppoeInstances[PTIN_SYSTEM_N_PPPOE_INSTANCES];

/* Reference of evcid using internal vlan as reference */
static L7_uint8 pppoeInst_fromEvcId[PTIN_SYSTEM_N_EXTENDED_EVCS];

/* Global PPPOE statistics at interface level */
ptin_PPPOE_Statistics_t global_stats_intf[PTIN_SYSTEM_N_INTERF];

/* Semaphores */
void *pppoe_sem = NULL;
void *ptin_pppoe_stats_sem = L7_NULLPTR;


/*********************************************************** 
 * Static prototypes
 ***********************************************************/

static L7_RC_t ptin_pppoe_client_find(L7_uint pppoe_idx, ptin_client_id_t *client_ref, ptinPppoeClientInfoData_t **client_info);
static L7_RC_t ptin_pppoe_instance_deleteAll_clients(L7_uint pppoe_idx);
static L7_RC_t ptin_pppoe_inst_get_fromIntVlan(L7_uint16 intVlan, st_PppoeInstCfg_t **pppoeInst, L7_uint *pppoeInst_idx);
static L7_RC_t ptin_pppoe_instance_find_free(L7_uint *idx);
static L7_RC_t ptin_pppoe_instance_find(L7_uint32 UcastEvcId, L7_uint *pppoe_idx);
#ifdef EVC_QUATTRO_FLOWS_FEATURE
static L7_RC_t ptin_pppoe_instance_find_agg(L7_uint16 nni_ovlan, L7_uint *pppoe_idx);
#endif
static L7_RC_t ptin_pppoe_trap_configure(L7_uint pppoe_idx, L7_BOOL enable);
static L7_RC_t ptin_pppoe_evc_trap_configure(L7_uint evc_idx, L7_BOOL enable);
static void    ptin_pppoe_evc_ethprty_get(ptin_AccessNodeCircuitId_t *evc_circuitid, L7_uint8 *ethprty);
static void    ptin_pppoe_circuitId_build(ptin_AccessNodeCircuitId_t *evc_circuitid, ptin_clientCircuitId_t *client_circuitid, L7_char8 *circuitid);
static void    ptin_pppoe_circuitid_convert(L7_char8 *circuitid_str, L7_char8 *str_to_replace, L7_char8 *parameter);

#if PPPOE_ACCEPT_UNSTACKED_PACKETS
static L7_RC_t ptin_pppoe_strings_def_get(ptin_intf_t *ptin_intf, L7_uchar8 *macAddr, L7_char8 *circuitId, L7_char8 *remoteId);
#endif

/*********************************************************** 
 * INLINE FUNCTIONS
 ***********************************************************/

inline L7_int pppoe_clientIndex_get_new(L7_uint8 pppoe_idx)
{
  L7_uint i;

  if (pppoe_idx>=PTIN_SYSTEM_N_PPPOE_INSTANCES)
    return -1;

  /* Search for the first free client index */
  for (i=0; i<PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE && pppoeInstances[pppoe_idx].pppoeClients.clients_in_use[i]!=L7_NULLPTR; i++);

  if (i>=PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE)
    return -1;
  return i;
}

inline void pppoe_clientIndex_mark(L7_uint8 pppoe_idx, L7_uint client_idx, ptinPppoeClientInfoData_t *infoData)
{
  ptinPppoeClients_t *clients;

  if (pppoe_idx>=PTIN_SYSTEM_N_PPPOE_INSTANCES || client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE)
    return;

  clients = &pppoeInstances[pppoe_idx].pppoeClients;

  if (clients->clients_in_use[client_idx]==L7_NULLPTR && clients->number_of_clients<PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE)
    clients->number_of_clients++;

  clients->clients_in_use[client_idx] = infoData;
}

inline void pppoe_clientIndex_unmark(L7_uint8 pppoe_idx, L7_uint client_idx)
{
  ptinPppoeClients_t *clients;

  if (pppoe_idx>=PTIN_SYSTEM_N_PPPOE_INSTANCES || client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE)
    return;

  clients = &pppoeInstances[pppoe_idx].pppoeClients;

  if (clients->clients_in_use[client_idx]!=L7_NULLPTR && clients->number_of_clients>0)
    clients->number_of_clients--;

  clients->clients_in_use[client_idx] = L7_NULLPTR;
}

inline void pppoe_clientIndex_clearAll(L7_uint8 pppoe_idx)
{
  if (pppoe_idx>=PTIN_SYSTEM_N_PPPOE_INSTANCES)
    return;

  memset(pppoeInstances[pppoe_idx].pppoeClients.clients_in_use,0x00,sizeof(pppoeInstances[pppoe_idx].pppoeClients.clients_in_use));
  pppoeInstances[pppoe_idx].pppoeClients.number_of_clients = 0;
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
  L7_uint pppoe_idx;
  ptinPppoeClientsAvlTree_t *avlTree;

  #if 0
  /* Clear database */
  memset(pppoe_relay_database, 0x00, sizeof(pppoe_relay_database));
  #endif

  /* Reset instances array */
  memset(pppoeInstances, 0x00, sizeof(pppoeInstances));

  /* Initialize lookup tables */
  memset(pppoeInst_fromEvcId, 0xFF, sizeof(pppoeInst_fromEvcId));

  /* Initialize global PPPOE statistics */
  memset(global_stats_intf,0x00,sizeof(global_stats_intf));

  /* Initialize sempahore */
  pppoe_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (pppoe_sem == NULL)
  {
    LOG_TRACE(LOG_CTX_PTIN_PPPOE, "Error creating a mutex for PPPOE module");
    return L7_FAILURE;
  }

  /* Initialize AVL trees */
  for (pppoe_idx=0; pppoe_idx<PTIN_SYSTEM_N_PPPOE_INSTANCES; pppoe_idx++)
  {
    avlTree = &pppoeInstances[pppoe_idx].pppoeClients.avlTree;

    pppoe_clientIndex_clearAll(pppoe_idx);

    avlTree->pppoeClientsTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE * sizeof(avlTreeTables_t)); 
    avlTree->pppoeClientsDataHeap = (ptinPppoeClientInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE * sizeof(ptinPppoeClientInfoData_t)); 

    if ((avlTree->pppoeClientsTreeHeap == L7_NULLPTR) ||
        (avlTree->pppoeClientsDataHeap == L7_NULLPTR))
    {
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error allocating data for PPPOE AVL Trees\n");
      return L7_FAILURE;
    }

    /* Initialize the storage for all the AVL trees */
    memset (&avlTree->pppoeClientsAvlTree, 0x00, sizeof(avlTree_t));
    memset (avlTree->pppoeClientsTreeHeap, 0x00, sizeof(avlTreeTables_t)*PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE);
    memset (avlTree->pppoeClientsDataHeap, 0x00, sizeof(ptinPppoeClientInfoData_t)*PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE);

    // AVL Tree creations - snoopIpAvlTree
    avlCreateAvlTree(&(avlTree->pppoeClientsAvlTree),
                     avlTree->pppoeClientsTreeHeap,
                     avlTree->pppoeClientsDataHeap,
                     PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE, 
                     sizeof(ptinPppoeClientInfoData_t),
                     0x10,
                     sizeof(ptinPppoeClientDataKey_t));
  }

  ptin_pppoe_stats_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_pppoe_stats_sem == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to create ptin_pppoe_stats_sem semaphore!");
    return L7_FAILURE;
  }

  LOG_INFO(LOG_CTX_PTIN_PPPOE, "PPPOE init OK");

  /* To be removed */
#if 0
  if (usmDbDsAdminModeSet(L7_ENABLE)!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error with usmDbDsAdminModeSet");
    return L7_FAILURE;
  }
  if (usmDbDsL2RelayAdminModeSet(L7_ENABLE)!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error with usmDbDsL2RelayAdminModeSet");
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
  /* To be removed */
  #if 0
  if (usmDbDsAdminModeSet(enable)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error applying PPPOE module enable=%u",enable);
    return L7_FAILURE;
  }
  if (dsL2RelayAdminModeSet(enable)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error applying PPPOE Relay Agent enable=%u",enable);
    return L7_FAILURE;
  }
  #endif
  
  if (ptin_pppoePkts_global_trap(enable)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error setting PPPOE global enable to %u",enable);
    /* To be removed */
    #if 0
    dsL2RelayAdminModeSet(!enable);
    usmDbDsAdminModeSet(!enable);
    #endif
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_PPPOE,"Success setting PPPOE global enable to %u",enable);
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
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid eEVC id: evcId=%u",evcId);
    return L7_FALSE;
  }

  /* This evc must be active */
  if (!ptin_evc_is_in_use(evcId))
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"eEVC id is not active: evcId=%u",evcId);
    return L7_FALSE;
  }

  /* Check if this EVC is being used by any PPPOE instance */
  if (ptin_pppoe_instance_find(evcId,L7_NULLPTR)!=L7_SUCCESS)
    return L7_FALSE;

  return L7_TRUE;
}

/**
 * Creates an PPPOE instance
 * 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_instance_add(L7_uint32 UcastEvcId)
{
  L7_uint pppoe_idx;

  /* Validate arguments */
  if (UcastEvcId>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid eEVC id: ucEvcId=%u",UcastEvcId);
    return L7_FAILURE;
  }

  /* These evcs must be active */
  if (!ptin_evc_is_in_use(UcastEvcId))
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"eEVC id is not active: ucEvcId%u",UcastEvcId);
    return L7_FAILURE;
  }

  /* Check if there is an instance with these parameters */
  if (ptin_pppoe_instance_find(UcastEvcId,L7_NULLPTR)==L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_PPPOE,"There is already an instance with ucEvcId%u",UcastEvcId);
    return L7_SUCCESS;
  }

  /* Find an empty instance to be used */
  if (ptin_pppoe_instance_find_free(&pppoe_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"There is no free instances to be used");
    return L7_FAILURE;
  }

  /* Save data in free instance */
  pppoeInstances[pppoe_idx].UcastEvcId = UcastEvcId;
  pppoeInstances[pppoe_idx].nni_ovid   = 0;
  pppoeInstances[pppoe_idx].inUse = L7_TRUE;

  /* Configure querier for this instance */
  if (ptin_pppoe_trap_configure(pppoe_idx,L7_ENABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error configuring PPPOE snooping for pppoe_idx=%u",pppoe_idx);
    memset(&pppoeInstances[pppoe_idx],0x00,sizeof(st_PppoeInstCfg_t));
    return L7_FAILURE;
  }

  /* PPPOE index in use */

  /* Save direct referencing to pppoe index from evc ids */
  pppoeInst_fromEvcId[UcastEvcId] = pppoe_idx;

  return L7_SUCCESS;
}

/**
 * Removes an PPPOE instance
 * 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_instance_remove(L7_uint32 UcastEvcId)
{
  L7_uint pppoe_idx;

  /* Validate arguments */
  if (UcastEvcId>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid eEVC ids: ucEvcId=%u",UcastEvcId);
    return L7_FAILURE;
  }

  /* Check if there is an instance with these parameters */
  if (ptin_pppoe_instance_find(UcastEvcId,&pppoe_idx)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_PPPOE,"There is no instance with ucEvcId=%u",UcastEvcId);
    return L7_SUCCESS;
  }

  /* Remove all clients attached to this instance */
  if (ptin_pppoe_instance_deleteAll_clients(pppoe_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error removing all clients from pppoe_idx %u (UcastEvcId=%u)",pppoe_idx,UcastEvcId);
    return L7_FAILURE;
  }

  /* Configure packet trapping for this instance */
  if (ptin_pppoe_trap_configure(pppoe_idx,L7_DISABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error unconfiguring PPPOE snooping for pppoe_idx=%u",pppoe_idx);
    return L7_FAILURE;
  }

  /* Clear data and free instance */
  pppoeInstances[pppoe_idx].UcastEvcId = 0;
  pppoeInstances[pppoe_idx].nni_ovid   = 0;
  pppoeInstances[pppoe_idx].inUse = L7_FALSE;

  /* Reset direct referencing to pppoe index from evc ids */
  pppoeInst_fromEvcId[UcastEvcId] = PPPOE_INVALID_ENTRY;

  return L7_SUCCESS;
}

/**
 * Update PPPOE entries, when EVCs are deleted
 * 
 * @param evcId : evc index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_instance_destroy(L7_uint32 evcId)
{
  return ptin_pppoe_instance_remove(evcId);
}

#ifdef EVC_QUATTRO_FLOWS_FEATURE
/**
 * Associate an EVC to a PPPOE instance
 * 
 * @param UcastEvcId : Unicast evc id 
 * @param nni_ovlan  : NNI outer vlan
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_evc_add(L7_uint32 UcastEvcId, L7_uint16 nni_ovlan)
{
  L7_uint pppoe_idx;

  /* Validate arguments */
  if (UcastEvcId>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid eEVC id: ucEvcId=%u",UcastEvcId);
    return L7_FAILURE;
  }

  /* These evcs must be active */
  if (!ptin_evc_is_in_use(UcastEvcId))
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"eEVC id is not active: ucEvcId%u",UcastEvcId);
    return L7_FAILURE;
  }

  /* Check if there is an instance with these parameters */
  if (ptin_pppoe_instance_find(UcastEvcId,L7_NULLPTR)==L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_PPPOE,"There is already an instance with ucEvcId%u",UcastEvcId);
    return L7_SUCCESS;
  }

  /* Check if there is an instance with the same NNI outer vlan: use it! */
  /* Otherwise, create a new instance */
  if ((nni_ovlan == 0 || nni_ovlan > 4095) ||
      ptin_pppoe_instance_find_agg(nni_ovlan, &pppoe_idx)!=L7_SUCCESS)
  {
    /* Find an empty instance to be used */
    if (ptin_pppoe_instance_find_free(&pppoe_idx)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"There is no free instances to be used");
      return L7_FAILURE;
    }
  }

  /* Configure querier for this instance */
  if (ptin_pppoe_evc_trap_configure(UcastEvcId, L7_ENABLE) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error configuring PPPOE snooping for pppoe_idx=%u",pppoe_idx);
    return L7_FAILURE;
  }

  /* Save data in free instance */
  if (!pppoeInstances[pppoe_idx].inUse)
  {
    pppoeInstances[pppoe_idx].UcastEvcId  = UcastEvcId;
    pppoeInstances[pppoe_idx].nni_ovid    = nni_ovlan;
    pppoeInstances[pppoe_idx].inUse       = L7_TRUE;
  }

  /* Save direct referencing to pppoe index from evc ids */
  pppoeInst_fromEvcId[UcastEvcId] = pppoe_idx;

  return L7_SUCCESS;
}

/**
 * Deassociate an EVC from a PPPOE instance
 * 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_evc_remove(L7_uint32 UcastEvcId)
{
  L7_uint pppoe_idx;
  L7_uint16 nni_ovid;

  /* Validate arguments */
  if (UcastEvcId>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid eEVC ids: ucEvcId=%u",UcastEvcId);
    return L7_FAILURE;
  }

  /* Check if there is an instance with these parameters */
  if (ptin_pppoe_instance_find(UcastEvcId,&pppoe_idx)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_PPPOE,"There is no instance with ucEvcId=%u",UcastEvcId);
    return L7_SUCCESS;
  }

  /* NNI outer vlan */
  nni_ovid = pppoeInstances[pppoe_idx].nni_ovid;

  /* Configure packet trapping for this instance */
  if (ptin_pppoe_evc_trap_configure(pppoe_idx, L7_DISABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error unconfiguring PPPOE snooping for pppoe_idx=%u",pppoe_idx);
    return L7_FAILURE;
  }

  /* Reset direct referencing to pppoe index from evc ids */
  pppoeInst_fromEvcId[UcastEvcId] = PPPOE_INVALID_ENTRY;

    /* Only clear instance, if there is no one using this NNI outer vlan */
  if ( (pppoeInstances[pppoe_idx].nni_ovid==0 || pppoeInstances[pppoe_idx].nni_ovid>4095) ||
       (ptin_pppoe_instance_find_agg(nni_ovid, &pppoe_idx) != L7_SUCCESS) )
  {
    /* Remove all clients attached to this instance */
    if (ptin_pppoe_instance_deleteAll_clients(pppoe_idx)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error removing all clients from pppoe_idx %u (UcastEvcId=%u)",pppoe_idx,UcastEvcId);
      return L7_FAILURE;
    }

    /* Clear data and free instance */
    pppoeInstances[pppoe_idx].UcastEvcId  = 0;
    pppoeInstances[pppoe_idx].nni_ovid    = 0;
    pppoeInstances[pppoe_idx].inUse       = L7_FALSE;
  }

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
#endif

/**
 * Reconfigure global PPPOE EVC
 *
 * @param evcId         : evc index
 * @param pppoe_flag     : PPPOE flag (not used)
 * @param options       : options
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_evc_reconf(L7_uint32 evcId, L7_uint8 pppoe_flag, L7_uint32 options)
{
   L7_uint pppoe_idx;
   ptinPppoeClientDataKey_t avl_key;
   ptinPppoeClientInfoData_t *avl_info;

   /* Get PPPOE instance index */
   if (ptin_pppoe_instance_find(evcId, &pppoe_idx) != L7_SUCCESS)
   {
    LOG_ERR(LOG_CTX_PTIN_PPPOE, "There is no PPPOE instance with EVC id %u", evcId);
    return L7_FAILURE;
   }

   /* Validate pppoe instance */
   if (!pppoeInstances[pppoe_idx].inUse)
   {
    LOG_ERR(LOG_CTX_PTIN_PPPOE, "PPPOE instance %u is not in use", pppoe_idx);
    return L7_FAILURE;
   }

   /* Save EVC PPPOE Options */
   pppoeInstances[pppoe_idx].evcPppoeOptions = options;

   /* Run all cells in AVL tree */
   memset(&avl_key,0x00,sizeof(ptinPppoeClientDataKey_t));
   while ( ( avl_info = (ptinPppoeClientInfoData_t *)
                       avlSearchLVL7(&pppoeInstances[pppoe_idx].pppoeClients.avlTree.pppoeClientsAvlTree, (void *)&avl_key, AVL_NEXT)
         ) != L7_NULLPTR )
   {
      /* Prepare next key */
      memcpy(&avl_key, &avl_info->pppoeClientDataKey, sizeof(ptinPppoeClientDataKey_t));

      /* Reconfigure PPPOE options for clients that are using Global EVC PPPOE options */
      if(L7_TRUE == avl_info->client_data.useEvcPppoeOptions)
      {
         avl_info->client_data.pppoe_options = options;
      }
   }

   return L7_SUCCESS;
}

/**
 * Set PPPOE circuit-id global data
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
L7_RC_t ptin_pppoe_circuitid_set(L7_uint32 evcId, L7_char8 *template_str, L7_uint32 mask, L7_char8 *access_node_id, L7_uint8 chassis,
                                L7_uint8 rack, L7_uint8 frame, L7_uint8 ethernet_priority, L7_uint16 s_vid)
{
  L7_uint pppoe_idx;
  ptinPppoeClientDataKey_t avl_key;
  ptinPppoeClientInfoData_t *avl_info;

  /* Validate arguments */
  if (template_str == L7_NULLPTR || access_node_id == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE, "Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }
  /* Validate string lengths */
  if ( strnlen(template_str, CIRCUITID_TEMPLATE_MAX_STRING) >= CIRCUITID_TEMPLATE_MAX_STRING )
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE, "Template string length is invalid!");
    return L7_FAILURE;
  }
  if ( strnlen(access_node_id, FD_DS_MAX_REMOTE_ID_STRING) >= FD_DS_MAX_REMOTE_ID_STRING )
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE, "Access node identifier length is invalid!");
    return L7_FAILURE;
  }

  /* Get PPPOE instance index */
  if (ptin_pppoe_instance_find(evcId, &pppoe_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE, "There is no PPPOE instance with EVC id %u", evcId);
    return L7_FAILURE;
  }

  /* Validate pppoe instance */
  if (!pppoeInstances[pppoe_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE, "PPPOE instance %u is not in use", pppoe_idx);
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
                        avlSearchLVL7(&pppoeInstances[pppoe_idx].pppoeClients.avlTree.pppoeClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->pppoeClientDataKey, sizeof(ptinPppoeClientDataKey_t));

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
    LOG_ERR(LOG_CTX_PTIN_PPPOE, "Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Get PPPOE instance index */
  if (ptin_pppoe_instance_find(evcId, &pppoe_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE, "There is no PPPOE instance with EVC id %u", evcId);
    return L7_FAILURE;
  }

  /* Validate pppoe instance */
  if (!pppoeInstances[pppoe_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE, "PPPOE instance %u is not in use", pppoe_idx);
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
 * @param UcastEvcId        : Unicast evc id
 * @param client            : client identification parameters
 * @param options           : PPPOE options
 * @param circuitId_data    : Circuit ID data 
 * @param circuitId         : Circuit ID string
 * @param remoteId          : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_client_get(L7_uint32 UcastEvcId, ptin_client_id_t *client, L7_uint16 *options,
                              ptin_clientCircuitId_t *circuitId_data,
                              L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_uint pppoe_idx;
  ptinPppoeClientInfoData_t *client_info;

  /* Validate arguments */
  if (client==L7_NULLPTR || PPPOE_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Get PPPOE instance index */
  if (ptin_pppoe_instance_find(UcastEvcId, &pppoe_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"There is no PPPOE instance with EVC id %u",UcastEvcId);
    return L7_FAILURE;
  }

  /* Find client information */
  if (ptin_pppoe_client_find(pppoe_idx,client,&client_info)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Non existent client in PPPOE instance %u (EVC id %u)",pppoe_idx,UcastEvcId);
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
    LOG_TRACE(LOG_CTX_PTIN_PPPOE, "%s", circuitId);
  }
  if (remoteId!=L7_NULLPTR)
  {
    strncpy(remoteId ,client_info->client_data.remoteId_str ,FD_DS_MAX_REMOTE_ID_STRING);
    remoteId[FD_DS_MAX_REMOTE_ID_STRING-1] = '\0';
    LOG_TRACE(LOG_CTX_PTIN_PPPOE, "%s", remoteId);
  }

  return L7_SUCCESS;
}

/**
 * Add a new PPPOE client
 * 
 * @param UcastEvcId        : Unicast evc id
 * @param client            : client identification parameters 
 * @param uni_ovid          : External outer vlan 
 * @param uni_ivid          : External inner vlan  
 * @param options           : PPPOE options
 * @param circuitId         : Circuit ID data 
 * @param remoteId          : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_client_add(L7_uint32 UcastEvcId, ptin_client_id_t *client, L7_uint16 uni_ovid, L7_uint16 uni_ivid,
                              L7_uint16 options, ptin_clientCircuitId_t *circuitId, L7_char8 *remoteId)
{
  L7_uint pppoe_idx, client_idx;
  ptinPppoeClientDataKey_t avl_key;
  ptinPppoeClientsAvlTree_t *avl_tree;
  ptinPppoeClientInfoData_t *avl_infoData;
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port;
  ptin_evc_intfCfg_t intfCfg;
  #endif
  L7_uint32 intIfNum;

  /* Validate arguments */
  if (client==L7_NULLPTR || PPPOE_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Get PPPOE instance index */
  if (ptin_pppoe_instance_find(UcastEvcId, &pppoe_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"There is no PPPOE instance with EVC id %u",UcastEvcId);
    return L7_FAILURE;
  }

  #if PPPOE_CLIENT_INNERVLAN_SUPPORTED
  /* Do not process null cvlans */
  if ((client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) &&
      (client->innerVlan==0 || client->innerVlan>4095))
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid inner vlan (%u)",client->innerVlan);
    return L7_SUCCESS;
  }
  #endif

  /* Get ptin_port value */
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  ptin_port = 0;
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    /* Get interface configuration in the UC EVC */
    if (ptin_evc_intfCfg_get(UcastEvcId, &client->ptin_intf, &intfCfg)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error acquiring intf configuration for intf %u/%u, evc=%u",client->ptin_intf.intf_type,client->ptin_intf.intf_id,UcastEvcId);
      return L7_FAILURE;
    }
    /* Validate interface configuration in EVC: must be in use, and be a leaf/client */
    if (!intfCfg.in_use || intfCfg.type!=PTIN_EVC_INTF_LEAF)
    {
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"intf %u/%u is not in use or is not a leaf in evc %u",client->ptin_intf.intf_type,client->ptin_intf.intf_id,UcastEvcId);
      return L7_FAILURE;
    }
    /* Convert to ptin_port format */
    if (ptin_intf_ptintf2port(&client->ptin_intf,&ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Cannot convert client intf %u/%u to ptin_port format",client->ptin_intf.intf_type,client->ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
  #endif

  /* If uni vlans are not provided, but interface is, get uni vlans from EVC data */
  if ( (uni_ovid<PTIN_VLAN_MIN || uni_ovid>PTIN_VLAN_MAX) &&
       (client->mask & PTIN_CLIENT_MASK_FIELD_INTF) &&
       (client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) )
  {
     /* Get interface as intIfNum format */
    if (ptin_intf_ptintf2intIfNum(&client->ptin_intf, &intIfNum)==L7_SUCCESS)
    {
      if (ptin_evc_extVlans_get(intIfNum, UcastEvcId, client->innerVlan, &uni_ovid, &uni_ivid) == L7_SUCCESS)
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

  /* Check if this key already exists */
  avl_tree = &pppoeInstances[pppoe_idx].pppoeClients.avlTree;
  memset(&avl_key,0x00,sizeof(ptinPppoeClientDataKey_t));
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  avl_key.ptin_port = ptin_port;
  #endif
  #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
  avl_key.outerVlan = (client->mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN) ? client->outerVlan : 0;
  #endif
  #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
  avl_key.innerVlan = (client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) ? client->innerVlan : 0;
  #endif
  #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
  avl_key.ipv4_addr = (client->mask & PTIN_CLIENT_MASK_FIELD_IPADDR   ) ? client->ipv4_addr : 0;
  #endif
  #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
  if (client->mask & PTIN_CLIENT_MASK_FIELD_MACADDR)
    memcpy(avl_key.macAddr,client->macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  else
    memset(avl_key.macAddr,0x00,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  #endif

  #if (PPPOE_CLIENT_DEBUG)
  LOG_TRACE(LOG_CTX_PTIN_PPPOE,"Key {"
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                              "port=%u,"
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
            avl_key.ptin_port,
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
    LOG_WARNING(LOG_CTX_PTIN_PPPOE,"This key {"
                #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                                  "port=%u,"
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
                avl_key.ptin_port,
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
    /* Get new client index */
    if ((client_idx=pppoe_clientIndex_get_new(pppoe_idx))<0)
    {
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Cannot get new client index for pppoe_idx=%u (evc=%u)",pppoe_idx,UcastEvcId);
      return L7_FAILURE;
    }

    /* Insert entry in AVL tree */
    if (avlInsertEntry(&(avl_tree->pppoeClientsAvlTree), (void *)&avl_key)!=L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error inserting key {"
              #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                                "port=%u,"
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
              avl_key.ptin_port,
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
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Cannot find key {"
              #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                                "port=%u,"
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
              avl_key.ptin_port,
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

    /* Client index */
    avl_infoData->client_index = client_idx;

    /* Save UNI vlans (external vlans used for transmission) */
    avl_infoData->uni_ovid = uni_ovid;
    avl_infoData->uni_ivid = uni_ivid;

    /* Mark one more client for AVL tree */
    pppoe_clientIndex_mark(pppoe_idx,client_idx,avl_infoData);

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

  LOG_TRACE(LOG_CTX_PTIN_PPPOE,"Success inserting Key {"
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                              "port=%u,"
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
            avl_key.ptin_port,
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
 * @param UcastEvcId  : Unicast evc id
 * @param client      : client identification parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_client_delete(L7_uint32 UcastEvcId, ptin_client_id_t *client)
{
  L7_uint pppoe_idx, client_idx;
  ptinPppoeClientDataKey_t avl_key;
  ptinPppoeClientsAvlTree_t *avl_tree;
  ptinPppoeClientInfoData_t *avl_infoData;
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port;
  #endif

  /* Validate arguments */
  if (client==L7_NULLPTR || PPPOE_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Get PPPOE instance index */
  if (ptin_pppoe_instance_find(UcastEvcId, &pppoe_idx)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_PPPOE,"There is no PPPOE instance with EVC id %u",UcastEvcId);
    return L7_NOT_EXIST;
  }

  #if PPPOE_CLIENT_INNERVLAN_SUPPORTED
  /* Do not process null cvlans */
  if ((client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) &&
      (client->innerVlan==0 || client->innerVlan>4095))
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid inner vlan (%u)",client->innerVlan);
    return L7_SUCCESS;
  }
  #endif

  /* Convert interface to ptin_port format */
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  ptin_port = 0;
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    if (ptin_intf_ptintf2port(&client->ptin_intf,&ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Cannot convert client intf %u/%u to ptin_port format",client->ptin_intf.intf_type,client->ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
  #endif

  /* Check if this key does not exists */

  avl_tree = &pppoeInstances[pppoe_idx].pppoeClients.avlTree;
  memset(&avl_key,0x00,sizeof(ptinPppoeClientDataKey_t));
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  avl_key.ptin_port = ptin_port;
  #endif
  #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
  avl_key.outerVlan = (client->mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN) ? client->outerVlan : 0;
  #endif
  #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
  avl_key.innerVlan = (client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) ? client->innerVlan : 0;
  #endif
  #if (PPPOE_CLIENT_IPADDR_SUPPORTED)
  avl_key.ipv4_addr = (client->mask & PTIN_CLIENT_MASK_FIELD_IPADDR   ) ? client->ipv4_addr : 0;
  #endif
  #if (PPPOE_CLIENT_MACADDR_SUPPORTED)
  if (client->mask & PTIN_CLIENT_MASK_FIELD_MACADDR)
    memcpy(avl_key.macAddr,client->macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  else
    memset(avl_key.macAddr,0x00,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  #endif

  #if (PPPOE_CLIENT_DEBUG)
  LOG_TRACE(LOG_CTX_PTIN_PPPOE,"Key to search {"
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                              "port=%u,"
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
            avl_key.ptin_port,
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
    LOG_WARNING(LOG_CTX_PTIN_PPPOE,"This key {"
                #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                                  "port=%u,"
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
                avl_key.ptin_port,
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
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error removing key {"
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                              "port=%u,"
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
            avl_key.ptin_port,
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

  /* Remove client for AVL tree */
  pppoe_clientIndex_unmark(pppoe_idx,client_idx);

  LOG_TRACE(LOG_CTX_PTIN_PPPOE,"Success removing Key {"
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                              "port=%u,"
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
            avl_key.ptin_port,
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
    LOG_ERR(LOG_CTX_PTIN_PPPOE, "This entry does not exist");
    return L7_FAILURE;
  }

  // Remove this entry
  memcpy(macAddr.addr,dsBinding->macAddr,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  if (usmDbDsBindingRemove(&macAddr)!=L7_SUCCESS) {
    LOG_ERR(LOG_CTX_PTIN_PPPOE, "Error removing entry");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
#endif

/**
 * Get global PPPOE statistics
 * 
 * @param intIfNum    : interface
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
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid interface %u/%u",ptin_intf->intf_id,ptin_intf->intf_id);
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
 * @param UcastEvcId  : Unicast EVC id
 * @param intIfNum    : interface
 * @param stat_port   : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_instanceIntf_get(L7_uint32 UcastEvcId, ptin_intf_t *ptin_intf, ptin_PPPOE_Statistics_t *stat_port)
{
  L7_uint32 ptin_port;
  L7_uint32 pppoe_idx;
  ptin_evc_intfCfg_t intfCfg;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid interface %u/%u",ptin_intf->intf_id,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Check if EVC is active, and if interface is part of the EVC */
  if (ptin_evc_intfCfg_get(UcastEvcId,ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error getting interface (%u/%u) configuration from EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,UcastEvcId);
    return L7_FAILURE;
  }
  if (!intfCfg.in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Interface %u/%u is not in use by EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,UcastEvcId);
    return L7_FAILURE;
  }

  /* Get Pppoe instance */
  if (ptin_pppoe_instance_find(UcastEvcId,&pppoe_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"EVC %u does not belong to any PPPOE instance",UcastEvcId);
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
 * @param UcastEvcId  : Unicast EVC id
 * @param client      : client reference
 * @param stat_port   : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_client_get(L7_uint32 UcastEvcId, ptin_client_id_t *client, ptin_PPPOE_Statistics_t *stat_client)
{
  L7_uint32 pppoe_idx;
  ptinPppoeClientInfoData_t *clientInfo;

  /* Validate arguments */
  if (client==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get Pppoe instance */
  if (ptin_pppoe_instance_find(UcastEvcId,&pppoe_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"EVC %u does not belong to any PPPOE instance",UcastEvcId);
    return L7_FAILURE;
  }

  /* Get client */
  if (ptin_pppoe_client_find(pppoe_idx, client, &clientInfo)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,
            "Error searching for client {mask=0x%02x,"
            "port=%u/%u,"
            "svlan=%u,"
            "cvlan=%u,"
            "ipAddr=%u.%u.%u.%u,"
            "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} "
            "in pppoe_idx=%u",
            client->mask,
            client->ptin_intf.intf_type, client->ptin_intf.intf_id,
            client->outerVlan,
            client->innerVlan,
            (client->ipv4_addr>>24) & 0xff, (client->ipv4_addr>>16) & 0xff, (client->ipv4_addr>>8) & 0xff, client->ipv4_addr & 0xff,
            client->macAddr[0],client->macAddr[1],client->macAddr[2],client->macAddr[3],client->macAddr[4],client->macAddr[5],
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
  L7_uint client_idx;

  osapiSemaTake(ptin_pppoe_stats_sem,-1);

  /* Run all PPPOE instances */
  for (pppoe_idx=0; pppoe_idx<PTIN_SYSTEM_N_PPPOE_INSTANCES; pppoe_idx++)
  {
    if (!pppoeInstances[pppoe_idx].inUse)  continue;

    /* Clear instance statistics */
    memset(pppoeInstances[pppoe_idx].stats_intf, 0x00, sizeof(pppoeInstances[pppoe_idx].stats_intf));

    /* Run all clients */
    for (client_idx=0; client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE; client_idx++)
    {
      if (pppoeInstances[pppoe_idx].pppoeClients.clients_in_use[client_idx]==L7_NULLPTR)  continue;

      /* Clear client statistics */
      memset(&pppoeInstances[pppoe_idx].pppoeClients.clients_in_use[client_idx]->client_stats, 0x00, sizeof(ptin_PPPOE_Statistics_t));
    }
  }

  /* Clear global statistics */
  memset(global_stats_intf,0x00,sizeof(global_stats_intf));

  osapiSemaGive(ptin_pppoe_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear all statistics of one PPPOE instance
 * 
 * @param UcastEvcId : Unicast EVC id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_instance_clear(L7_uint32 UcastEvcId)
{
  L7_uint pppoe_idx;
  L7_uint client_idx;

  /* Get Pppoe instance */
  if (ptin_pppoe_instance_find(UcastEvcId,&pppoe_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"EVC %u does not belong to any PPPOE instance",UcastEvcId);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_pppoe_stats_sem,-1);

  /* Clear instance statistics */
  memset(pppoeInstances[pppoe_idx].stats_intf, 0x00, sizeof(pppoeInstances[pppoe_idx].stats_intf));

  /* Run all clients */
  for (client_idx=0; client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE; client_idx++)
  {
    if (pppoeInstances[pppoe_idx].pppoeClients.clients_in_use[client_idx]==L7_NULLPTR)  continue;

    /* Clear client statistics */
    memset(&pppoeInstances[pppoe_idx].pppoeClients.clients_in_use[client_idx]->client_stats, 0x00, sizeof(ptin_PPPOE_Statistics_t));
  }

  osapiSemaGive(ptin_pppoe_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear interface PPPOE statistics
 * 
 * @param intIfNum    : interface 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_intf_clear(ptin_intf_t *ptin_intf)
{
  L7_uint pppoe_idx;
  L7_uint client_idx;
  L7_uint32 ptin_port;
  st_PppoeInstCfg_t *pppoeInst;
  ptinPppoeClientInfoData_t *clientInfo;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid interface %u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_pppoe_stats_sem,-1);

  /* Run all PPPOE instances */
  for (pppoe_idx=0; pppoe_idx<PTIN_SYSTEM_N_PPPOE_INSTANCES; pppoe_idx++)
  {
    if (!pppoeInstances[pppoe_idx].inUse)  continue;

    pppoeInst = &pppoeInstances[pppoe_idx];

    /* Clear instance statistics */
    memset(&pppoeInst->stats_intf[ptin_port], 0x00, sizeof(ptin_PPPOE_Statistics_t));

    #if (PPPOE_CLIENT_INTERF_SUPPORTED)
    /* Run all clients */
    for (client_idx=0; client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE; client_idx++)
    {
      if (pppoeInst->pppoeClients.clients_in_use[client_idx]==L7_NULLPTR)  continue;

      /* Clear client statistics (if port matches) */
      clientInfo = pppoeInst->pppoeClients.clients_in_use[client_idx];
      if (clientInfo->pppoeClientDataKey.ptin_port==ptin_port)
      {
        memset(&clientInfo->client_stats, 0x00, sizeof(ptin_PPPOE_Statistics_t));
      }
    }
    #endif
  }

  /* Clear global statistics */
  memset(&global_stats_intf[ptin_port], 0x00, sizeof(ptin_PPPOE_Statistics_t));

  osapiSemaGive(ptin_pppoe_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear statistics of a particular PPPOE instance and interface
 * 
 * @param UcastEvcId  : Unicast EVC id
 * @param intIfNum    : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_instanceIntf_clear(L7_uint32 UcastEvcId, ptin_intf_t *ptin_intf)
{
  L7_uint pppoe_idx;
  L7_uint client_idx;
  L7_uint32 ptin_port;
  st_PppoeInstCfg_t *pppoeInst;
  ptinPppoeClientInfoData_t *clientInfo;
  ptin_evc_intfCfg_t intfCfg;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid interface %u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Check if EVC is active, and if interface is part of the EVC */
  if (ptin_evc_intfCfg_get(UcastEvcId,ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error getting interface (%u/%u) configuration from EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,UcastEvcId);
    return L7_FAILURE;
  }
  if (!intfCfg.in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Interface %u/%u is not in use by EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,UcastEvcId);
    return L7_FAILURE;
  }

  /* Get Pppoe instance */
  if (ptin_pppoe_instance_find(UcastEvcId,&pppoe_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"EVC %u does not belong to any PPPOE instance",UcastEvcId);
    return L7_FAILURE;
  }

  /* Pointer to pppoe instance */
  pppoeInst = &pppoeInstances[pppoe_idx];

  osapiSemaTake(ptin_pppoe_stats_sem,-1);

  /* Clear instance statistics */
  memset(&pppoeInst->stats_intf[ptin_port], 0x00, sizeof(ptin_PPPOE_Statistics_t));

  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  /* Run all clients */
  for (client_idx=0; client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE; client_idx++)
  {
    if (pppoeInst->pppoeClients.clients_in_use[client_idx]==L7_NULLPTR)  continue;

    /* Clear client statistics (if port matches) */
    clientInfo = pppoeInst->pppoeClients.clients_in_use[client_idx];
    if (clientInfo->pppoeClientDataKey.ptin_port==ptin_port)
    {
      memset(&clientInfo->client_stats, 0x00, sizeof(ptin_PPPOE_Statistics_t));
    }
  }
  #endif

  osapiSemaGive(ptin_pppoe_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear PPPOE statistics of a particular PPPOE instance and 
 * client
 * 
 * @param UcastEvcId  : Unicast EVC id
 * @param client      : client reference
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_client_clear(L7_uint32 UcastEvcId, ptin_client_id_t *client)
{
  L7_uint pppoe_idx;
  ptinPppoeClientInfoData_t *clientInfo;

  /* Validate arguments */
  if (client==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get Pppoe instance */
  if (ptin_pppoe_instance_find(UcastEvcId,&pppoe_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"EVC %u does not belong to any PPPOE instance",UcastEvcId);
    return L7_FAILURE;
  }

  /* Find client */
  if (ptin_pppoe_client_find(pppoe_idx,client,&clientInfo)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,
            "Error searching for client {mask=0x%02x,"
            "port=%u/%u,"
            "svlan=%u,"
            "cvlan=%u,"
            "ipAddr=%u.%u.%u.%u,"
            "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} "
            "in pppoe_idx=%u",
            client->mask,
            client->ptin_intf.intf_type, client->ptin_intf.intf_id,
            client->outerVlan,
            client->innerVlan,
            (client->ipv4_addr>>24) & 0xff, (client->ipv4_addr>>16) & 0xff, (client->ipv4_addr>>8) & 0xff, client->ipv4_addr & 0xff,
            client->macAddr[0],client->macAddr[1],client->macAddr[2],client->macAddr[3],client->macAddr[4],client->macAddr[5],
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
  st_PppoeInstCfg_t  *pppoeInst;
  L7_uint16           intRootVlan;

  /* IGMP instance, from internal vlan */
  if (ptin_pppoe_inst_get_fromIntVlan(intVlan,&pppoeInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"No PPPoE instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get Multicast root vlan */
  if (ptin_evc_intRootVlan_get(pppoeInst->UcastEvcId, &intRootVlan)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error getting rootVlan for EvcId=%u (intVlan=%u)",pppoeInst->UcastEvcId, intVlan);
    return L7_FAILURE;
  }

  /* Return Multicast root vlan */
  if (rootVlan!=L7_SUCCESS)  *rootVlan = intRootVlan;

  return L7_SUCCESS;
}

/**
 * Validate ingress interface for a PPPOE packet 
 * 
 * @param intIfNum    : interface
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_pppoe_intf_validate(L7_uint32 intIfNum)
{
  /* Validate arguments */
  if ( intIfNum==0 || intIfNum>=L7_MAX_INTERFACE_COUNT )
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments: intIfNum=%u",intIfNum);
    return L7_FALSE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_intIfNum2ptintf(intIfNum,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid intIfNum %u",intIfNum);
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
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments: intVlan=%u",intVlanId);
    return L7_FALSE;
  }

  /* PPPOE instance, from internal vlan */
  if (ptin_pppoe_inst_get_fromIntVlan(intVlanId,L7_NULLPTR,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"No PPPOE instance associated to intVlan %u",intVlanId);
    return L7_FALSE;
  }

  return L7_TRUE;
}

/**
 * Validate interface, internal vlan and innervlan received in a 
 * PPPOE packet 
 * 
 * @param intIfNum    : interface
 * @param intVlanId   : internal vlan
 * @param innerVlanId : client vlan
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_pppoe_intfVlan_validate(L7_uint32 intIfNum, L7_uint16 intVlanId /*, L7_uint16 innerVlanId*/)
{
  L7_uint pppoe_idx;
  ptin_intf_t ptin_intf;
  ptin_evc_intfCfg_t intfCfg;
  st_PppoeInstCfg_t *pppoeInst;

  /* Validate arguments */
  if ( intIfNum==0 || intIfNum>=L7_MAX_INTERFACE_COUNT ||
       intVlanId<PTIN_VLAN_MIN || intVlanId>PTIN_VLAN_MAX )
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments: intIfNum=%u intVlan=%u",intIfNum,intVlanId);
    return L7_FALSE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid intIfNum %u",intIfNum);
    return L7_FALSE;
  }

  /* PPPOE instance, from internal vlan */
  if (ptin_pppoe_inst_get_fromIntVlan(intVlanId,&pppoeInst,&pppoe_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"No PPPOE instance associated to intVlan %u",intVlanId);
    return L7_FALSE;
  }

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(pppoeInst->UcastEvcId,&ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error acquiring interface %u/%u configuarion from EVC id %u",
              ptin_intf.intf_type,ptin_intf.intf_id,pppoeInst->UcastEvcId);
    return L7_FALSE;
  }

  /* Interface must be in use */
  if (!intfCfg.in_use)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Interface %u/%u (intIfNum=%u) is not in use for EVC %u",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,pppoeInst->UcastEvcId);
    return L7_FALSE;
  }

  #if 0
  /* For untrusted interfaces, we must have an inner vlan (for non CXP360G) */
  #if ( PTIN_BOARD != PTIN_BOARD_CXP360G )
  if ( intfCfg.type==PTIN_EVC_INTF_LEAF && (innerVlanId==0 || innerVlanId>=4095) )
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"For unstrusted interface %u/%u (evc=%u) inner vlan must be used",
              ptin_intf.intf_type,ptin_intf.intf_id,pppoeInst->UcastEvcId);
    return L7_FALSE;
  }
  #endif
  #endif

  return L7_TRUE;
}

/**
 * Check if a particular interface of one EVC is trusted
 * 
 * @param intIfNum    : interface
 * @param intVlanId   : internal vlan
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_pppoe_is_intfTrusted(L7_uint32 intIfNum, L7_uint16 intVlanId)
{
  L7_uint pppoe_idx;
  ptin_intf_t ptin_intf;
  ptin_evc_intfCfg_t intfCfg;
  st_PppoeInstCfg_t *pppoeInst;

  /* Validate arguments */
  if ( intIfNum==0 || intIfNum>=L7_MAX_INTERFACE_COUNT ||
       intVlanId<PTIN_VLAN_MIN || intVlanId>PTIN_VLAN_MAX )
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments: intIfNum=%u intVlan=%u",intIfNum,intVlanId);
    return L7_FALSE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid intIfNum %u",intIfNum);
    return L7_FALSE;
  }

  /* PPPOE instance, from internal vlan */
  if (ptin_pppoe_inst_get_fromIntVlan(intVlanId,&pppoeInst,&pppoe_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"No PPPOE instance associated to intVlan %u",intVlanId);
    return L7_FALSE;
  }

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(pppoeInst->UcastEvcId,&ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error acquiring interface %u/%u configuarion from EVC id %u",
              ptin_intf.intf_type,ptin_intf.intf_id,pppoeInst->UcastEvcId);
    return L7_FALSE;
  }

  /* Interface must be in use */
  if (!intfCfg.in_use)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Interface %u/%u (intIfNum=%u) is not in use for EVC %u",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,pppoeInst->UcastEvcId);
    return L7_FALSE;
  }

  /* Root ports are trusted */
  if ( intfCfg.type != PTIN_EVC_INTF_ROOT )
  {
    return L7_FALSE;
  }

  return L7_TRUE;
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
L7_RC_t ptin_pppoe_extVlans_get(L7_uint32 intIfNum, L7_uint16 intOVlan, L7_uint16 intIVlan,
                                L7_int client_idx, L7_uint16 *uni_ovid, L7_uint16 *uni_ivid)
{
  L7_uint pppoe_idx;
  ptinPppoeClientInfoData_t *clientInfo;

  /* If leaf interface, and client is provided, go directly to client info */
  if (!ptin_pppoe_is_intfTrusted(intIfNum, intOVlan) &&
      client_idx < PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE)
  {
    /* Get PPPOE instance from internal vlan */
    if (ptin_pppoe_inst_get_fromIntVlan(intOVlan, L7_NULLPTR, &pppoe_idx) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    /* Get pointer to client structure in AVL tree */
    clientInfo = pppoeInstances[pppoe_idx].pppoeClients.clients_in_use[client_idx];

    /* Return vlans */
    if (uni_ovid != L7_SUCCESS)  *uni_ovid = clientInfo->uni_ovid;
    if (uni_ivid != L7_SUCCESS)  *uni_ovid = clientInfo->uni_ivid;
  }
  /* Otherwise, goto EVC data */
  else
  {
    return ptin_evc_extVlans_get_fromIntVlan(intIfNum, intOVlan, intIVlan, uni_ovid, uni_ivid);
  }

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
  ptin_intf_t ptin_intf;
  L7_uint     client_idx;
  ptinPppoeClientInfoData_t *clientInfo;

  /* Validate arguments */
  if ( client==L7_NULLPTR || PPPOE_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* PPPOE instance, from internal vlan */
  if (ptin_pppoe_inst_get_fromIntVlan(intVlan,L7_NULLPTR,&pppoe_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"No PPPOE instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* If the inner vlan is not valid, return -1 as client index */
  #if PPPOE_CLIENT_INNERVLAN_SUPPORTED
  if ((client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) &&
      (client->innerVlan==0 || client->innerVlan>4095))
  {
    if (client_index!=L7_NULLPTR)  *client_index = (L7_uint)-1;
    return L7_SUCCESS;
  }
  #endif

  /* Get ptin_port format for the interface number */
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
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
        if (ptin_debug_pppoe_snooping)
          LOG_ERR(LOG_CTX_PTIN_PPPOE,"Connot convert client intIfNum %u to ptin_port_format",intIfNum);
        return L7_FAILURE;
      }
    }
  }
  #endif

  /* Get client */
  if (ptin_pppoe_client_find(pppoe_idx, client, &clientInfo)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
    {
      LOG_ERR(LOG_CTX_PTIN_PPPOE,
              "Error searching for client {mask=0x%02x,"
              "port=%u/%u,"
              "svlan=%u,"
              "cvlan=%u,"
              "ipAddr=%u.%u.%u.%u,"
              "MacAddr=%02x:%02x:%02x:%02x:%02x:%02x} "
              "in pppoe_idx=%u",
              client->mask,
              client->ptin_intf.intf_type, client->ptin_intf.intf_id,
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
  LOG_TRACE(LOG_CTX_PTIN_PPPOE,"Client_idx=%u for key {"
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                              "port=%u,"
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
            #if (PPPOE_CLIENT_INTERF_SUPPORTED)
            ,clientInfo->pppoeClientDataKey.ptin_port
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
  ptin_intf_t ptin_intf;
  st_PppoeInstCfg_t *pppoeInst;
  ptinPppoeClientInfoData_t *clientInfo;

  /* Validate arguments */
  if ( client==L7_NULLPTR || client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE )
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* PPPOE instance, from internal vlan */
  if (ptin_pppoe_inst_get_fromIntVlan(intVlan,&pppoeInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"No PPPOE instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get pointer to client structure in AVL tree */
  clientInfo = pppoeInst->pppoeClients.clients_in_use[client_idx];
  /* If does not exist... */
  if (clientInfo==L7_NULLPTR)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Provided client_idx (%u) does not exist",client_idx);
    return L7_FAILURE;
  }

  memset(client,0x00,sizeof(ptin_client_id_t));
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  if (ptin_intf_port2ptintf(clientInfo->pppoeClientDataKey.ptin_port,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Cannot convert client port %uu to ptin_intf format",clientInfo->pppoeClientDataKey.ptin_port);
    return L7_FAILURE;
  }
  client->ptin_intf = ptin_intf;
  client->mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  #endif
  #if (PPPOE_CLIENT_OUTERVLAN_SUPPORTED)
  client->outerVlan = clientInfo->pppoeClientDataKey.outerVlan;
  client->mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  #endif
  #if (PPPOE_CLIENT_INNERVLAN_SUPPORTED)
  client->innerVlan = clientInfo->pppoeClientDataKey.innerVlan;
  client->mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
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
 * @param intIfNum    : FP interface
 * @param intVlan     : internal vlan
 * @param innerVlan   : inner/client vlan 
 * @param circuitId   : circuit id (output) 
 * @param remoteId    : remote id (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stringIds_get(L7_uint32 intIfNum, L7_uint16 intVlan, L7_uint16 innerVlan, L7_uchar8 *macAddr,
                                L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_uint pppoe_idx;
  ptin_intf_t ptin_intf;
  ptin_client_id_t client;
  ptinPppoeClientInfoData_t *client_info;

  /* Validate arguments */
  if (intIfNum==0 || intIfNum>=L7_MAX_INTERFACE_COUNT ||
      intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX /*||
      innerVlan==0 || innerVlan>=4096*/)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Convert interface to ptin format */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid intIfNum (%u)",intIfNum);
    return L7_FAILURE;
  }

  /* Get pppoe instance */
  if (ptin_pppoe_inst_get_fromIntVlan(intVlan,L7_NULLPTR,&pppoe_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Internal vlan %u does not correspond to any PPPOE instance",intVlan);
    return L7_FAILURE;
  }

  if (innerVlan>0 && innerVlan<4096)
  {
    /* Build client structure */
    memset(&client,0x00,sizeof(ptin_client_id_t));
    #if PPPOE_CLIENT_INTERF_SUPPORTED
    client.ptin_intf.intf_type = ptin_intf.intf_type;
    client.ptin_intf.intf_id   = ptin_intf.intf_id;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
    #endif
    #if PPPOE_CLIENT_INNERVLAN_SUPPORTED
    client.innerVlan = innerVlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
    #endif

    /* Find client information */
    if (ptin_pppoe_client_find(pppoe_idx,&client,&client_info)!=L7_SUCCESS)
    {
      if (ptin_debug_pppoe_snooping)
        LOG_ERR(LOG_CTX_PTIN_PPPOE,"Non existent client in PPPOE instance %u (EVC id %u)",pppoe_idx,pppoeInstances[pppoe_idx].UcastEvcId);
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
      LOG_ERR(LOG_CTX_PTIN_PPPOE, "circuitId is NULL");
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
        LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error getting default strings");
      return L7_FAILURE;
    }
    #else
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"No client defined!");
    return L7_FAILURE;
    #endif
  }

  return L7_SUCCESS;
}

/**
 * Get PPPOE EVC ethernet priority
 * 
 * @param intIfNum    : FP interface
 * @param intVlan     : internal vlan
 * @param innerVlan   : inner/client vlan 
 * @param circuitId   : circuit id (output) 
 * @param remoteId    : remote id (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_ethPrty_get(L7_uint32 intIfNum, L7_uint16 intVlan, L7_uint16 innerVlan, L7_uint8 *ethPrty)
{
  L7_uint pppoe_idx;
  ptin_intf_t ptin_intf;
  ptin_client_id_t client;
  ptinPppoeClientInfoData_t *client_info;

  /* Validate arguments */
  if (intIfNum==0 || intIfNum>=L7_MAX_INTERFACE_COUNT ||
      intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX  ||
      L7_NULLPTR == ethPrty                         /*||
      innerVlan==0 || innerVlan>=4096*/)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Convert interface to ptin format */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid intIfNum (%u)",intIfNum);
    return L7_FAILURE;
  }

  /* Get pppoe instance */
  if (ptin_pppoe_inst_get_fromIntVlan(intVlan,L7_NULLPTR,&pppoe_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Internal vlan %u does not correspond to any PPPOE instance",intVlan);
    return L7_FAILURE;
  }

  if (innerVlan>0 && innerVlan<4096)
  {
    /* Build client structure */
    memset(&client,0x00,sizeof(ptin_client_id_t));
    #if PPPOE_CLIENT_INTERF_SUPPORTED
    client.ptin_intf.intf_type = ptin_intf.intf_type;
    client.ptin_intf.intf_id   = ptin_intf.intf_id;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
    #endif
    #if PPPOE_CLIENT_INNERVLAN_SUPPORTED
    client.innerVlan = innerVlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
    #endif

    /* Find client information */
    if (ptin_pppoe_client_find(pppoe_idx,&client,&client_info)!=L7_SUCCESS)
    {
      if (ptin_debug_pppoe_snooping)
        LOG_ERR(LOG_CTX_PTIN_PPPOE,"Non existent client in PPPOE instance %u (EVC id %u)",pppoe_idx,pppoeInstances[pppoe_idx].UcastEvcId);
      return L7_FAILURE;
    }

    ptin_pppoe_evc_ethprty_get(&pppoeInstances[pppoe_idx].circuitid, ethPrty);
  }

  return L7_SUCCESS;
}

/**
 * Get PPPOE client data (PPPOE Options)
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
L7_RC_t ptin_pppoe_client_options_get(L7_uint32 intIfNum, L7_uint16 intVlan, L7_uint16 innerVlan, L7_BOOL *isActiveOp82,
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
   if (intIfNum == 0 || intIfNum >= L7_MAX_INTERFACE_COUNT || intVlan < PTIN_VLAN_MIN || intVlan > PTIN_VLAN_MAX ||
    innerVlan==0 || innerVlan>=4096)
   {
      if (ptin_debug_pppoe_snooping)
         LOG_ERR(LOG_CTX_PTIN_PPPOE, "Invalid arguments");
      return L7_FAILURE;
   }

   /* Convert interface to ptin format */
   if (ptin_intf_intIfNum2ptintf(intIfNum, &ptin_intf) != L7_SUCCESS)
   {
      if (ptin_debug_pppoe_snooping)
         LOG_ERR(LOG_CTX_PTIN_PPPOE, "Invalid intIfNum (%u)", intIfNum);
      return L7_FAILURE;
   }

   /* Get pppoe instance */
   if (ptin_pppoe_inst_get_fromIntVlan(intVlan, L7_NULLPTR, &pppoe_idx) != L7_SUCCESS)
   {
      if (ptin_debug_pppoe_snooping)
         LOG_ERR(LOG_CTX_PTIN_PPPOE, "Internal vlan %u does not correspond to any PPPOE instance", intVlan);
      return L7_FAILURE;
   }

   if (innerVlan > 0 && innerVlan < 4096)
   {
      /* Build client structure */
      memset(&client, 0x00, sizeof(ptin_client_id_t));
#if PPPOE_CLIENT_INTERF_SUPPORTED
      client.ptin_intf.intf_type = ptin_intf.intf_type;
      client.ptin_intf.intf_id = ptin_intf.intf_id;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
#endif
#if PPPOE_CLIENT_INNERVLAN_SUPPORTED
      client.innerVlan = innerVlan;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
#endif

      /* Find client information */
      if (ptin_pppoe_client_find(pppoe_idx, &client, &client_info) != L7_SUCCESS)
      {
         if (ptin_debug_pppoe_snooping)
            LOG_ERR(LOG_CTX_PTIN_PPPOE, "Non existent client in PPPOE instance %u (EVC id %u)",
                  pppoe_idx, pppoeInstances[pppoe_idx].UcastEvcId);
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
      LOG_ERR(LOG_CTX_PTIN_PPPOE, "No client defined!");
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
#if (!PTIN_SYSTEM_GROUP_VLANS)
  ptin_evc_intfCfg_t intfCfg;

  /* Validate arguments */
  if (evcId>=PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid eEVC id: evcId=%u",evcId);
    return L7_FAILURE;
  }

  /* This evc must be active */
  if (!ptin_evc_is_in_use(evcId))
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"eEVC id is not active: evcId=%u",evcId);
    return L7_FAILURE;
  }

  /* Check if this EVC is being used by any PPPOE instance */
  if (ptin_pppoe_instance_find(evcId,L7_NULLPTR)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_PPPOE,"EVC %u is not used in any PPPOE instance... nothing to do",evcId);
    return L7_SUCCESS;
  }

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(evcId,ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error acquiring interface %u/%u configuarion from EVC id %u",ptin_intf->intf_type,ptin_intf->intf_id,evcId);
    return L7_FAILURE;
  }

  /* If internal vlan associated to interface is valid, use it */
  if (intfCfg.int_vlan>=PTIN_VLAN_MIN && intfCfg.int_vlan<=PTIN_VLAN_MAX)
  {
    if (ptin_pppoePkts_vlan_trap(intfCfg.int_vlan,enable)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error configuring to %u int_vlan %u of interface %u/%u (EVC id %u)",enable,intfCfg.int_vlan,ptin_intf->intf_type,ptin_intf->intf_id,evcId);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_PPPOE,"PPPOE trapping configured to %u, for vlan %u (interface %u/%u)",enable,intfCfg.int_vlan,ptin_intf->intf_type,ptin_intf->intf_id);
  }
#endif
  return L7_SUCCESS;
}

/**
 * Increment PPPOE statistics
 * 
 * @param intIfNum   : interface where the packet entered
 * @param vlan       : packet's interval vlan
 * @param client_idx : client index
 * @param field      : field to increment
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_increment_field(L7_uint32 intIfNum, L7_uint16 vlan, L7_uint32 client_idx, ptin_pppoe_stat_enum_t field)
{
  L7_uint32 ptin_port;
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
  if (intIfNum>0 && intIfNum<L7_MAX_INTERFACE_COUNT)
  {
    /* Check if interface exists */
    if (ptin_intf_intIfNum2port(intIfNum,&ptin_port)==L7_SUCCESS)
    {
      /* Global interface statistics at interface level */
      stat_port_g = &global_stats_intf[ptin_port];

      if (pppoeInst!=L7_NULLPTR)
      {
        /* interface statistics at pppoe instance and interface level */
        stat_port = &pppoeInst->stats_intf[ptin_port];
      }
    }
  }

  /* If client index is valid... */
  if (pppoeInst!=L7_NULLPTR && client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE)
  {
    client = pppoeInst->pppoeClients.clients_in_use[client_idx];
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

#ifdef EVC_QUATTRO_FLOWS_FEATURE
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
#endif

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
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port;
  #endif

  /* Validate arguments */
  if (pppoe_idx>=PTIN_SYSTEM_N_PPPOE_INSTANCES || client_ref==L7_NULLPTR)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate pppoe instance */
  if (!pppoeInstances[pppoe_idx].inUse)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"PPPOE instance %u is not in use",pppoe_idx);
    return L7_FAILURE;
  }

  /* Get ptin_port value */
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  ptin_port = 0;
  if (client_ref->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    /* Convert to ptin_port format */
    if (ptin_intf_ptintf2port(&client_ref->ptin_intf,&ptin_port)!=L7_SUCCESS)
    {
      if (ptin_debug_pppoe_snooping)
        LOG_ERR(LOG_CTX_PTIN_PPPOE,"Cannot convert client_ref intf %u/%u to ptin_port format",client_ref->ptin_intf.intf_type,client_ref->ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
  #endif

  /* Key to search for */
  avl_tree = &pppoeInstances[pppoe_idx].pppoeClients.avlTree;
  memset(&avl_key,0x00,sizeof(ptinPppoeClientDataKey_t));
  #if (PPPOE_CLIENT_INTERF_SUPPORTED)
  avl_key.ptin_port = ptin_port;
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
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Key {"
              #if (PPPOE_CLIENT_INTERF_SUPPORTED)
                                "port=%u"
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
              avl_key.ptin_port,
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
  ptinPppoeClientsAvlTree_t *avl_tree;

  /* Validate argument */
  if (pppoe_idx>=PTIN_SYSTEM_N_PPPOE_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid pppoe instance index (%u)",pppoe_idx);
    return L7_FAILURE;
  }
  /* PPPOE instance must be in use */
  if (!pppoeInstances[pppoe_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"pppoe instance index %u is not in use",pppoe_idx);
    return L7_FAILURE;
  }

  avl_tree = &pppoeInstances[pppoe_idx].pppoeClients.avlTree;

  /* Remove all entries from AVL tree */
  avlPurgeAvlTree(&(avl_tree->pppoeClientsAvlTree), PTIN_SYSTEM_MAXCLIENTS_PER_PPPOE_INSTANCE);

  /* Remove all clients of AVL tree */
  pppoe_clientIndex_clearAll(pppoe_idx);

  LOG_TRACE(LOG_CTX_PTIN_PPPOE,"Success removing all clients from pppoe_idx=%u",pppoe_idx);

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
  L7_uint32 evc_idx, pppoe_idx;

  /* Verify if this internal vlan is associated to an EVC */
  if (ptin_evc_get_evcIdfromIntVlan(intVlan, &evc_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"No EVC associated to internal vlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Check if the EVC has a PPPOE instance */
  pppoe_idx = pppoeInst_fromEvcId[evc_idx];

  if (pppoe_idx>=PTIN_SYSTEM_N_PPPOE_INSTANCES)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"No PPPOE instance associated to evcId=%u (intVlan=%u)",evc_idx,intVlan);
    return L7_FAILURE;
  }

  /* Check if this instance is in use, and if evc_ids are valid */
  if (!pppoeInstances[pppoe_idx].inUse)
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Inconsistency: PPPOE index %u (EVCid=%u, Vlan %u) is not in use",pppoe_idx,evc_idx,intVlan);
    return L7_FAILURE;
  }

  /* Check if EVCs are in use */
  if (!ptin_evc_is_in_use(pppoeInstances[pppoe_idx].UcastEvcId))
  {
    if (ptin_debug_pppoe_snooping)
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Inconsistency: PPPOE index %u (EVCid=%u, Vlan %u) has EVC not in use (evc=%u)",pppoe_idx,evc_idx,intVlan,pppoeInstances[pppoe_idx].UcastEvcId);
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
 * @param UcastEvcId : Unicast EVC id
 * @param pppoe_idx   : PPPOE instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_pppoe_instance_find(L7_uint32 UcastEvcId, L7_uint *pppoe_idx)
{
  #if 1
  /* Validate evc index */
  if (UcastEvcId >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    return L7_FAILURE;
  }
  /* Check if there is an instance associated to this EVC */
  if (pppoeInst_fromEvcId[UcastEvcId] >= PTIN_SYSTEM_N_PPPOE_INSTANCES)
  {
    return L7_FAILURE;
  }

  /* Return index */
  if (pppoe_idx!=L7_NULLPTR)  *pppoe_idx = pppoeInst_fromEvcId[UcastEvcId];

  return L7_SUCCESS;
  #else
  L7_uint idx;

  /* Search for the provided Mcast and Ucast evcs */
  for (idx=0; idx<PTIN_SYSTEM_N_PPPOE_INSTANCES; idx++)
  {
    if (!pppoeInstances[idx].inUse)  continue;

    if (pppoeInstances[idx].UcastEvcId==UcastEvcId)
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
static L7_RC_t ptin_pppoe_trap_configure(L7_uint pppoe_idx, L7_BOOL enable)
{
  /* Validate argument */
  if (pppoe_idx>=PTIN_SYSTEM_N_PPPOE_INSTANCES)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid pppoe instance index (%u)",pppoe_idx);
    return L7_FAILURE;
  }
  /* PPPOE instance must be in use */
  if (!pppoeInstances[pppoe_idx].inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"pppoe instance index %u is not in use",pppoe_idx);
    return L7_FAILURE;
  }

  /* Apply configurations for this evc */
  return ptin_pppoe_evc_trap_configure(pppoeInstances[pppoe_idx].UcastEvcId, enable);
}

/**
 * Configure trapping vlans
 * 
 * @param evc_idx 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t ptin_pppoe_evc_trap_configure(L7_uint evc_idx, L7_BOOL enable)
{
  L7_uint16   idx, vlan, uc_evcId;
  ptin_HwEthMef10Evc_t evcCfg;
  L7_uint16 vlans_number, vlan_list[PTIN_SYSTEM_MAX_N_PORTS];
#if (!PTIN_SYSTEM_GROUP_VLANS)
  ptin_intf_t ptin_intf;
  L7_uint16            intf_idx;
  ptin_evc_intfCfg_t   intfCfg;
#endif

  enable &= 1;

  /* Initialize number of vlans to be configured */
  vlans_number = 0;
  uc_evcId = evc_idx;

  /* Get Unicast EVC configuration */
  evcCfg.index = uc_evcId;
  if (ptin_evc_get(&evcCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error getting UC EVC %u configuration",uc_evcId);
    return L7_FAILURE;
  }
#if (!PTIN_SYSTEM_GROUP_VLANS)
  /* If UC EVC is point-to-point, use its root vlan */
  if ((evcCfg.flags & PTIN_EVC_MASK_P2P     ) == PTIN_EVC_MASK_P2P  || 
      (evcCfg.flags & PTIN_EVC_MASK_QUATTRO ) == PTIN_EVC_MASK_QUATTRO)
#endif
  {
    if (ptin_evc_intRootVlan_get(uc_evcId,&vlan)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Can't get UC root vlan for evc id %u",uc_evcId);
      return L7_FAILURE;
    }
    if (vlan>=PTIN_VLAN_MIN && vlan<=PTIN_VLAN_MAX)
    {
      vlan_list[vlans_number++] = vlan;
    }
  }
#if (!PTIN_SYSTEM_GROUP_VLANS)
  /* If point-to-multipoint... */
  else
  {
    /* Run all interfaces, and get its configurations */
    for (intf_idx=0; intf_idx<evcCfg.n_intf; intf_idx++)
    {
      /* Get interface configuarions */
      ptin_intf.intf_type = evcCfg.intf[intf_idx].intf_type;
      ptin_intf.intf_id   = evcCfg.intf[intf_idx].intf_id;
      if (ptin_evc_intfCfg_get(uc_evcId, &ptin_intf, &intfCfg)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error getting interface %u/%u configuration from UC EVC %u",ptin_intf.intf_type,ptin_intf.intf_id,uc_evcId);
        return L7_FAILURE;
      }
      /* Extract internal vlan */
      vlan = intfCfg.int_vlan;
      if (vlan>=PTIN_VLAN_MIN && vlan<=PTIN_VLAN_MAX)
      {
        /* Verify if this vlan is scheduled to be configured */
        for (idx=0; idx<vlans_number; idx++)
          if (vlan_list[idx]==vlan)  break;
        if (idx<vlans_number)  continue;

        /* Can this vlan be configured? */
        if (vlans_number>=PTIN_SYSTEM_MAX_N_PORTS)
        {
          LOG_ERR(LOG_CTX_PTIN_PPPOE,"Excessive number of vlans to be configured (morte than %u)",PTIN_SYSTEM_MAX_N_PORTS);
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
    if (ptin_pppoePkts_vlan_trap(vlan_list[idx],enable)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Error configuring vlan %u for packet trapping",vlan_list[idx]);
      break;
    }
    LOG_TRACE(LOG_CTX_PTIN_PPPOE,"Success configuring vlan %u for packet trapping",vlan_list[idx]);
  }
  /* If something went wrong, undo configurations */
  if (idx<vlans_number)
  {
    vlans_number = idx;
    for (idx=0; idx<vlans_number; idx++)
    {
      ptin_pppoePkts_vlan_trap(vlan_list[idx],!enable);
      LOG_WARNING(LOG_CTX_PTIN_PPPOE,"Unconfiguring vlan %u for packet trapping",vlan_list[idx]);
    }
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

void ptin_pppoe_evc_ethprty_get(ptin_AccessNodeCircuitId_t *evc_circuitid, L7_uint8 *ethprty)
{
   if(L7_NULLPTR == evc_circuitid || L7_NULLPTR == ethprty)
   {
      LOG_ERR(LOG_CTX_PTIN_PPPOE,"Invalid arguments");
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

/* DEBUG Functions ************************************************************/
/**
 * Dumps EVC detailed info 
 * If evc_idx is invalid, all EVCs are dumped 
 * 
 * @param evc_idx 
 */
void ptin_pppoe_dump(void)
{
  L7_uint i, i_client;
  ptinPppoeClientDataKey_t avl_key;
  ptinPppoeClientInfoData_t *avl_info;

  for (i = 0; i < PTIN_SYSTEM_N_PPPOE_INSTANCES; i++)
  {
    if (!pppoeInstances[i].inUse) {
      printf("*** PPPoE instance %02u not in use\r\n", i);
      continue;
    }

    printf("PPPoE instance %02u: EVC_idx = %u \t[CircuitId Template: %s]\r\n", i,
           pppoeInstances[i].UcastEvcId, pppoeInstances[i].circuitid.template_str);

    i_client = 0;

    /* Run all cells in AVL tree */
    memset(&avl_key,0x00,sizeof(ptinPppoeClientDataKey_t));
    while ( ( avl_info = (ptinPppoeClientInfoData_t *)
                          avlSearchLVL7(&pppoeInstances[i].pppoeClients.avlTree.pppoeClientsAvlTree, (void *)&avl_key, AVL_NEXT)
            ) != L7_NULLPTR )
    {
      /* Prepare next key */
      memcpy(&avl_key, &avl_info->pppoeClientDataKey, sizeof(ptinPppoeClientDataKey_t));

      printf("   Client#%u: "
             #if (PPPOE_CLIENT_INTERF_SUPPORTED)
             "ptin_port=%-2u "
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
             ": index=%-4u  [uni_vlans=%4u+%-4u] circuitId=\"%s\" remoteId=\"%s\"\r\n",
             i_client,
             #if (PPPOE_CLIENT_INTERF_SUPPORTED)
             avl_info->pppoeClientDataKey.ptin_port,
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
             avl_info->client_index,
             avl_info->uni_ovid, avl_info->uni_ivid,
             avl_info->client_data.circuitId_str,
             avl_info->client_data.remoteId_str);

      i_client++;
    }
  }
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
    slot = cpld_map->reg.slot_id + 2;
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

