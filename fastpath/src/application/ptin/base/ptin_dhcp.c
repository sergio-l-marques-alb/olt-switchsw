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
#define DHCP_CLIENT_OUTERVLAN_SUPPORTED 0
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
  L7_char8 circuitId[FD_DS_MAX_REMOTE_ID_STRING+1];   /* Circuit ID string */
  L7_char8 remoteId[FD_DS_MAX_REMOTE_ID_STRING+1];    /* Remote ID string */
} ptinDhcpData_t;

typedef struct
{
  ptinDhcpClientDataKey_t dhcpClientDataKey;
  L7_uint16               client_index;
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
  L7_uint16 number_of_clients;
  ptinDhcpClientInfoData_t *clients_in_use[PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE];
  ptinDhcpClientsAvlTree_t avlTree;
} ptinDhcpClients_t;

/* DHCP Instance config struct */
typedef struct {
  L7_BOOL   inUse;
  L7_uint16 UcastEvcId;
  ptinDhcpClients_t dhcpClients;
  ptin_DHCP_Statistics_t stats_intf[PTIN_SYSTEM_N_INTERF];  /* DHCP statistics at interface level */
} st_DhcpInstCfg_t;

/*********************************************************** 
 * Data structs
 ***********************************************************/

/* DHCP instances array */
st_DhcpInstCfg_t  dhcpInstances[PTIN_SYSTEM_N_DHCP_INSTANCES];

/* Reference of evcid using internal vlan as reference */
static L7_uint8 dhcpInst_fromEvcId[PTIN_SYSTEM_N_EVCS];

/* Global DHCP statistics at interface level */
ptin_DHCP_Statistics_t global_stats_intf[PTIN_SYSTEM_N_INTERF];

/* Semaphores */
void *dhcp_sem = NULL;
void *ptin_dhcp_stats_sem = L7_NULLPTR;


/*********************************************************** 
 * Static prototypes
 ***********************************************************/

static L7_RC_t ptin_dhcp_client_find(L7_uint dhcp_idx, ptin_client_id_t *client_ref, ptinDhcpClientInfoData_t **client_info);
static L7_RC_t ptin_dhcp_instance_deleteAll_clients(L7_uint dhcp_idx);
static L7_RC_t ptin_dhcp_inst_get_fromIntVlan(L7_uint16 intVlan, st_DhcpInstCfg_t **dhcpInst, L7_uint *dhcpInst_idx);
static L7_RC_t ptin_dhcp_instance_find_free(L7_uint *idx);
static L7_RC_t ptin_dhcp_instance_find(L7_uint16 UcastEvcId, L7_uint *dhcp_idx);
static L7_RC_t ptin_dhcp_trap_configure(L7_uint dhcp_idx, L7_BOOL enable);

#if DHCP_ACCEPT_UNSTACKED_PACKETS
static L7_RC_t ptin_dhcp_strings_def_get(ptin_intf_t *ptin_intf, L7_uchar8 *macAddr, L7_char8 *circuitId, L7_char8 *remoteId);
#endif

/*********************************************************** 
 * INLINE FUNCTIONS
 ***********************************************************/

inline L7_int dhcp_clientIndex_get_new(L7_uint8 dhcp_idx)
{
  L7_uint i;

  if (dhcp_idx>=PTIN_SYSTEM_N_DHCP_INSTANCES)
    return -1;

  /* Search for the first free client index */
  for (i=0; i<PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE && dhcpInstances[dhcp_idx].dhcpClients.clients_in_use[i]!=L7_NULLPTR; i++);

  if (i>=PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE)
    return -1;
  return i;
}

inline void dhcp_clientIndex_mark(L7_uint8 dhcp_idx, L7_uint client_idx, ptinDhcpClientInfoData_t *infoData)
{
  ptinDhcpClients_t *clients;

  if (dhcp_idx>=PTIN_SYSTEM_N_DHCP_INSTANCES || client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE)
    return;

  clients = &dhcpInstances[dhcp_idx].dhcpClients;

  if (clients->clients_in_use[client_idx]==L7_NULLPTR && clients->number_of_clients<PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE)
    clients->number_of_clients++;

  clients->clients_in_use[client_idx] = infoData;
}

inline void dhcp_clientIndex_unmark(L7_uint8 dhcp_idx, L7_uint client_idx)
{
  ptinDhcpClients_t *clients;

  if (dhcp_idx>=PTIN_SYSTEM_N_DHCP_INSTANCES || client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE)
    return;

  clients = &dhcpInstances[dhcp_idx].dhcpClients;

  if (clients->clients_in_use[client_idx]!=L7_NULLPTR && clients->number_of_clients>0)
    clients->number_of_clients--;

  clients->clients_in_use[client_idx] = L7_NULLPTR;
}

inline void dhcp_clientIndex_clearAll(L7_uint8 dhcp_idx)
{
  if (dhcp_idx>=PTIN_SYSTEM_N_DHCP_INSTANCES)
    return;

  memset(dhcpInstances[dhcp_idx].dhcpClients.clients_in_use,0x00,sizeof(dhcpInstances[dhcp_idx].dhcpClients.clients_in_use));
  dhcpInstances[dhcp_idx].dhcpClients.number_of_clients = 0;
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
  L7_uint dhcp_idx;
  ptinDhcpClientsAvlTree_t *avlTree;

  #if 0
  /* Clear database */
  memset(dhcp_relay_database, 0x00, sizeof(dhcp_relay_database));
  #endif

  /* Reset instances array */
  memset(dhcpInstances, 0x00, sizeof(dhcpInstances));

  /* Initialize lookup tables */
  memset(dhcpInst_fromEvcId, 0xFF, sizeof(dhcpInst_fromEvcId));

  /* Initialize global DHCP statistics */
  memset(global_stats_intf,0x00,sizeof(global_stats_intf));

  /* Initialize sempahore */
  dhcp_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dhcp_sem == NULL)
  {
    LOG_TRACE(LOG_CTX_PTIN_DHCP, "Error creating a mutex for DHCP module");
    return L7_FAILURE;
  }

  /* Initialize AVL trees */
  for (dhcp_idx=0; dhcp_idx<PTIN_SYSTEM_N_DHCP_INSTANCES; dhcp_idx++)
  {
    avlTree = &dhcpInstances[dhcp_idx].dhcpClients.avlTree;

    dhcp_clientIndex_clearAll(dhcp_idx);

    avlTree->dhcpClientsTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE * sizeof(avlTreeTables_t)); 
    avlTree->dhcpClientsDataHeap = (ptinDhcpClientInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE * sizeof(ptinDhcpClientInfoData_t)); 

    if ((avlTree->dhcpClientsTreeHeap == L7_NULLPTR) ||
        (avlTree->dhcpClientsDataHeap == L7_NULLPTR))
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error allocating data for DHCP AVL Trees\n");
      return L7_FAILURE;
    }

    /* Initialize the storage for all the AVL trees */
    memset (&avlTree->dhcpClientsAvlTree, 0x00, sizeof(avlTree_t));
    memset (avlTree->dhcpClientsTreeHeap, 0x00, sizeof(avlTreeTables_t)*PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE);
    memset (avlTree->dhcpClientsDataHeap, 0x00, sizeof(ptinDhcpClientInfoData_t)*PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE);

    // AVL Tree creations - snoopIpAvlTree
    avlCreateAvlTree(&(avlTree->dhcpClientsAvlTree),
                     avlTree->dhcpClientsTreeHeap,
                     avlTree->dhcpClientsDataHeap,
                     PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE, 
                     sizeof(ptinDhcpClientInfoData_t),
                     0x10,
                     sizeof(ptinDhcpClientDataKey_t));
  }

  ptin_dhcp_stats_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_dhcp_stats_sem == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to create ptin_dhcp_stats_sem semaphore!");
    return L7_FAILURE;
  }

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
  
  if (ptin_dhcpPkts_global_trap(enable)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error setting DHCP global enable to %u",enable);
    dsL2RelayAdminModeSet(!enable);
    usmDbDsAdminModeSet(!enable);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_DHCP,"Success setting DHCP global enable to %u",enable);
  return L7_SUCCESS;
}

/**
 * Check if a EVC is being used in an DHCP instance
 * 
 * @param evcId : evc id
 * 
 * @return L7_RC_t : L7_TRUE or L7_FALSE
 */
L7_RC_t ptin_dhcp_is_evc_used(L7_uint16 evcId)
{
  /* Validate arguments */
  if (evcId>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid EVC id: evcId=%u",evcId);
    return L7_FALSE;
  }

  /* This evc must be active */
  if (!ptin_evc_is_in_use(evcId))
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"EVC id is not active: evcId=%u",evcId);
    return L7_FALSE;
  }

  /* Check if this EVC is being used by any DHCP instance */
  if (ptin_dhcp_instance_find(evcId,L7_NULLPTR)!=L7_SUCCESS)
    return L7_FALSE;

  return L7_TRUE;
}

/**
 * Creates an DHCP instance
 * 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_instance_add(L7_uint16 UcastEvcId)
{
  L7_uint dhcp_idx;

  /* Validate arguments */
  if (UcastEvcId>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid EVC id: ucEvcId=%u",UcastEvcId);
    return L7_FAILURE;
  }

  /* These evcs must be active */
  if (!ptin_evc_is_in_use(UcastEvcId))
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"EVC id is not active: ucEvcId%u",UcastEvcId);
    return L7_FAILURE;
  }

  /* Check if there is an instance with these parameters */
  if (ptin_dhcp_instance_find(UcastEvcId,L7_NULLPTR)==L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"There is already an instance with ucEvcId%u",UcastEvcId);
    return L7_SUCCESS;
  }

  /* Find an empty instance to be used */
  if (ptin_dhcp_instance_find_free(&dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"There is no free instances to be used");
    return L7_FAILURE;
  }

  /* Save data in free instance */
  dhcpInstances[dhcp_idx].UcastEvcId = UcastEvcId;
  dhcpInstances[dhcp_idx].inUse = L7_TRUE;

  /* Configure querier for this instance */
  if (ptin_dhcp_trap_configure(dhcp_idx,L7_ENABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error configuring DHCP snooping for dhcp_idx=%u",dhcp_idx);
    memset(&dhcpInstances[dhcp_idx],0x00,sizeof(st_DhcpInstCfg_t));
    return L7_FAILURE;
  }

  /* DHCP index in use */

  /* Save direct referencing to dhcp index from evc ids */
  dhcpInst_fromEvcId[UcastEvcId] = dhcp_idx;

  return L7_SUCCESS;
}

/**
 * Removes an DHCP instance
 * 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_instance_remove(L7_uint16 UcastEvcId)
{
  L7_uint dhcp_idx;

  /* Validate arguments */
  if (UcastEvcId>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid EVC ids: ucEvcId=%u",UcastEvcId);
    return L7_FAILURE;
  }

  /* Check if there is an instance with these parameters */
  if (ptin_dhcp_instance_find(UcastEvcId,&dhcp_idx)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"There is no instance with ucEvcId=%u",UcastEvcId);
    return L7_SUCCESS;
  }

  /* Remove all clients attached to this instance */
  if (ptin_dhcp_instance_deleteAll_clients(dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error removing all clients from dhcp_idx %u (UcastEvcId=%u)",dhcp_idx,UcastEvcId);
    return L7_FAILURE;
  }

  /* Configure packet trapping for this instance */
  if (ptin_dhcp_trap_configure(dhcp_idx,L7_DISABLE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error unconfiguring DHCP snooping for dhcp_idx=%u",dhcp_idx);
    return L7_FAILURE;
  }

  /* Clear data and free instance */
  dhcpInstances[dhcp_idx].UcastEvcId = 0;
  dhcpInstances[dhcp_idx].inUse = L7_FALSE;

  /* Reset direct referencing to dhcp index from evc ids */
  dhcpInst_fromEvcId[UcastEvcId] = DHCP_INVALID_ENTRY;

  return L7_SUCCESS;
}

/**
 * Update DHCP entries, when EVCs are deleted
 * 
 * @param evcId : evc index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_instance_destroy(L7_uint16 evcId)
{
  return ptin_dhcp_instance_remove(evcId);
}

/**
 * Get DHCP client data (circuit and remote ids)
 * 
 * @param UcastEvcId  : Unicast evc id
 * @param client      : client identification parameters
 * @param circuitId   : circuit id (output)
 * @param remoteId    : remote id (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_client_get(L7_uint16 UcastEvcId, ptin_client_id_t *client, L7_char8 *circuitId, L7_char8 *remoteId)
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
  if (ptin_dhcp_instance_find(UcastEvcId, &dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"There is no DHCP instance with EVC id %u",UcastEvcId);
    return L7_FAILURE;
  }

  /* Find client information */
  if (ptin_dhcp_client_find(dhcp_idx,client,&client_info)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Non existent client in DHCP instance %u (EVC id %u)",dhcp_idx,UcastEvcId);
    return L7_FAILURE;
  }

  /* Return string ids */
  if (circuitId!=L7_NULLPTR)
    strncpy(circuitId,client_info->client_data.circuitId,FD_DS_MAX_REMOTE_ID_STRING);
  if (remoteId!=L7_NULLPTR)
    strncpy(remoteId ,client_info->client_data.remoteId ,FD_DS_MAX_REMOTE_ID_STRING);

  return L7_SUCCESS;
}

/**
 * Add a new DHCP client
 * 
 * @param UcastEvcId  : Unicast evc id
 * @param client      : client identification parameters
 * @param circuitId   : circuit id
 * @param remoteId    : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_client_add(L7_uint16 UcastEvcId, ptin_client_id_t *client, L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_uint dhcp_idx, client_idx;
  ptinDhcpClientDataKey_t avl_key;
  ptinDhcpClientsAvlTree_t *avl_tree;
  ptinDhcpClientInfoData_t *avl_infoData;
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port;
  ptin_evc_intfCfg_t intfCfg;
  #endif

  /* Validate arguments */
  if (client==L7_NULLPTR || DHCP_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Get DHCP instance index */
  if (ptin_dhcp_instance_find(UcastEvcId, &dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"There is no DHCP instance with EVC id %u",UcastEvcId);
    return L7_FAILURE;
  }

  #if DHCP_CLIENT_INNERVLAN_SUPPORTED
  /* Do not process null cvlans */
  if ((client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) &&
      (client->innerVlan==0 || client->innerVlan>4095))
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid inner vlan (%u)",client->innerVlan);
    return L7_SUCCESS;
  }
  #endif

  /* Get ptin_port value */
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  ptin_port = 0;
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    /* Get interface configuration in the UC EVC */
    if (ptin_evc_intfCfg_get(UcastEvcId, &client->ptin_intf, &intfCfg)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error acquiring intf configuration for intf %u/%u, evc=%u",client->ptin_intf.intf_type,client->ptin_intf.intf_id,UcastEvcId);
      return L7_FAILURE;
    }
    /* Validate interface configuration in EVC: must be in use, and be a leaf/client */
    if (!intfCfg.in_use || intfCfg.type!=PTIN_EVC_INTF_LEAF)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"intf %u/%u is not in use or is not a leaf in evc %u",client->ptin_intf.intf_type,client->ptin_intf.intf_id,UcastEvcId);
      return L7_FAILURE;
    }
    /* Convert to ptin_port format */
    if (ptin_intf_ptintf2port(&client->ptin_intf,&ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Cannot convert client intf %u/%u to ptin_port format",client->ptin_intf.intf_type,client->ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
  #endif

  /* Check if this key already exists */
  avl_tree = &dhcpInstances[dhcp_idx].dhcpClients.avlTree;
  memset(&avl_key,0x00,sizeof(ptinDhcpClientDataKey_t));
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  avl_key.ptin_port = ptin_port;
  #endif
  #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
  avl_key.outerVlan = (client->mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN) ? client->outerVlan : 0;
  #endif
  #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
  avl_key.innerVlan = (client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) ? client->innerVlan : 0;
  #endif
  #if (DHCP_CLIENT_IPADDR_SUPPORTED)
  avl_key.ipv4_addr = (client->mask & PTIN_CLIENT_MASK_FIELD_IPADDR   ) ? client->ipv4_addr : 0;
  #endif
  #if (DHCP_CLIENT_MACADDR_SUPPORTED)
  if (client->mask & PTIN_CLIENT_MASK_FIELD_MACADDR)
    memcpy(avl_key.macAddr,client->macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
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
    /* Get new client index */
    if ((client_idx=dhcp_clientIndex_get_new(dhcp_idx))<0)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Cannot get new client index for dhcp_idx=%u (evc=%u)",dhcp_idx,UcastEvcId);
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

    /* Client index */
    avl_infoData->client_index = client_idx;

    /* Mark one more client for AVL tree */
    dhcp_clientIndex_mark(dhcp_idx,client_idx,avl_infoData);

    /* Clear circuit and remote id strings */
    memset(&avl_infoData->client_data,0x00,sizeof(ptinDhcpData_t));

    /* Clear dhcp statistics */
    osapiSemaTake(ptin_dhcp_stats_sem,-1);
    memset(&avl_infoData->client_stats,0x00,sizeof(ptin_DHCP_Statistics_t));
    osapiSemaGive(ptin_dhcp_stats_sem);
  }

  /* Fill circuit and remote id fields */
  strncpy(avl_infoData->client_data.circuitId,circuitId,FD_DS_MAX_REMOTE_ID_STRING);
  strncpy(avl_infoData->client_data.remoteId ,remoteId ,FD_DS_MAX_REMOTE_ID_STRING);

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
 * @param UcastEvcId  : Unicast evc id
 * @param client      : client identification parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_client_delete(L7_uint16 UcastEvcId, ptin_client_id_t *client)
{
  L7_uint dhcp_idx, client_idx;
  ptinDhcpClientDataKey_t avl_key;
  ptinDhcpClientsAvlTree_t *avl_tree;
  ptinDhcpClientInfoData_t *avl_infoData;
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port;
  #endif

  /* Validate arguments */
  if (client==L7_NULLPTR || DHCP_CLIENT_MASK_UPDATE(client->mask)==0x00)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments or no parameters provided");
    return L7_FAILURE;
  }

  /* Get DHCP instance index */
  if (ptin_dhcp_instance_find(UcastEvcId, &dhcp_idx)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"There is no DHCP instance with EVC id %u",UcastEvcId);
    return L7_NOT_EXIST;
  }

  #if DHCP_CLIENT_INNERVLAN_SUPPORTED
  /* Do not process null cvlans */
  if ((client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) &&
      (client->innerVlan==0 || client->innerVlan>4095))
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid inner vlan (%u)",client->innerVlan);
    return L7_SUCCESS;
  }
  #endif

  /* Convert interface to ptin_port format */
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  ptin_port = 0;
  if (client->mask & PTIN_CLIENT_MASK_FIELD_INTF)
  {
    if (ptin_intf_ptintf2port(&client->ptin_intf,&ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Cannot convert client intf %u/%u to ptin_port format",client->ptin_intf.intf_type,client->ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }
  #endif

  /* Check if this key does not exists */

  avl_tree = &dhcpInstances[dhcp_idx].dhcpClients.avlTree;
  memset(&avl_key,0x00,sizeof(ptinDhcpClientDataKey_t));
  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  avl_key.ptin_port = ptin_port;
  #endif
  #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
  avl_key.outerVlan = (client->mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN) ? client->outerVlan : 0;
  #endif
  #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
  avl_key.innerVlan = (client->mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN) ? client->innerVlan : 0;
  #endif
  #if (DHCP_CLIENT_IPADDR_SUPPORTED)
  avl_key.ipv4_addr = (client->mask & PTIN_CLIENT_MASK_FIELD_IPADDR   ) ? client->ipv4_addr : 0;
  #endif
  #if (DHCP_CLIENT_MACADDR_SUPPORTED)
  if (client->mask & PTIN_CLIENT_MASK_FIELD_MACADDR)
    memcpy(avl_key.macAddr,client->macAddr,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
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

  /* Remove client for AVL tree */
  dhcp_clientIndex_unmark(dhcp_idx,client_idx);

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
L7_RC_t ptin_dhcp82_bindtable_get(ptin_DHCP_bind_entry *table, L7_uint16 *max_entries)
{
  dhcpSnoopBinding_t  dsBinding;
  L7_uint16           index, i;
  ptin_intf_t         ptin_intf;
  L7_uint16           evc_idx;
  L7_uint16           n_max;

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
  memcpy(macAddr.addr,dsBinding->macAddr,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  if (usmDbDsBindingRemove(&macAddr)!=L7_SUCCESS) {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Error removing entry");
    return L7_FAILURE;
  }

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
 * @param UcastEvcId  : Unicast EVC id
 * @param intIfNum    : interface
 * @param stat_port   : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_instanceIntf_get(L7_uint16 UcastEvcId, ptin_intf_t *ptin_intf, ptin_DHCP_Statistics_t *stat_port)
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
  if (ptin_evc_intfCfg_get(UcastEvcId,ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error getting interface (%u/%u) configuration from EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,UcastEvcId);
    return L7_FAILURE;
  }
  if (!intfCfg.in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Interface %u/%u is not in use by EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,UcastEvcId);
    return L7_FAILURE;
  }

  /* Get Dhcp instance */
  if (ptin_dhcp_instance_find(UcastEvcId,&dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"EVC %u does not belong to any DHCP instance",UcastEvcId);
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
 * @param UcastEvcId  : Unicast EVC id
 * @param client      : client reference
 * @param stat_port   : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_client_get(L7_uint16 UcastEvcId, ptin_client_id_t *client, ptin_DHCP_Statistics_t *stat_client)
{
  L7_uint32 dhcp_idx;
  ptinDhcpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if (client==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get Dhcp instance */
  if (ptin_dhcp_instance_find(UcastEvcId,&dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"EVC %u does not belong to any DHCP instance",UcastEvcId);
    return L7_FAILURE;
  }

  /* Get client */
  if (ptin_dhcp_client_find(dhcp_idx, client, &clientInfo)!=L7_SUCCESS)
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
    return L7_FAILURE;
  }

  /* Return pointer to stat structure */
  if (stat_client!=L7_NULLPTR)
  {
    osapiSemaTake(ptin_dhcp_stats_sem,-1);
    memcpy(stat_client, &clientInfo->client_stats, sizeof(ptin_DHCP_Statistics_t));
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
  L7_uint client_idx;

  osapiSemaTake(ptin_dhcp_stats_sem,-1);

  /* Run all DHCP instances */
  for (dhcp_idx=0; dhcp_idx<PTIN_SYSTEM_N_DHCP_INSTANCES; dhcp_idx++)
  {
    if (!dhcpInstances[dhcp_idx].inUse)  continue;

    /* Clear instance statistics */
    memset(dhcpInstances[dhcp_idx].stats_intf, 0x00, sizeof(dhcpInstances[dhcp_idx].stats_intf));

    /* Run all clients */
    for (client_idx=0; client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE; client_idx++)
    {
      if (dhcpInstances[dhcp_idx].dhcpClients.clients_in_use[client_idx]==L7_NULLPTR)  continue;

      /* Clear client statistics */
      memset(&dhcpInstances[dhcp_idx].dhcpClients.clients_in_use[client_idx]->client_stats, 0x00, sizeof(ptin_DHCP_Statistics_t));
    }
  }

  /* Clear global statistics */
  memset(global_stats_intf,0x00,sizeof(global_stats_intf));

  osapiSemaGive(ptin_dhcp_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear all statistics of one DHCP instance
 * 
 * @param UcastEvcId : Unicast EVC id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_instance_clear(L7_uint16 UcastEvcId)
{
  L7_uint dhcp_idx;
  L7_uint client_idx;

  /* Get Dhcp instance */
  if (ptin_dhcp_instance_find(UcastEvcId,&dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"EVC %u does not belong to any DHCP instance",UcastEvcId);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_dhcp_stats_sem,-1);

  /* Clear instance statistics */
  memset(dhcpInstances[dhcp_idx].stats_intf, 0x00, sizeof(dhcpInstances[dhcp_idx].stats_intf));

  /* Run all clients */
  for (client_idx=0; client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE; client_idx++)
  {
    if (dhcpInstances[dhcp_idx].dhcpClients.clients_in_use[client_idx]==L7_NULLPTR)  continue;

    /* Clear client statistics */
    memset(&dhcpInstances[dhcp_idx].dhcpClients.clients_in_use[client_idx]->client_stats, 0x00, sizeof(ptin_DHCP_Statistics_t));
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
  L7_uint client_idx;
  L7_uint32 ptin_port;
  st_DhcpInstCfg_t *dhcpInst;
  ptinDhcpClientInfoData_t *clientInfo;

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

  /* Run all DHCP instances */
  for (dhcp_idx=0; dhcp_idx<PTIN_SYSTEM_N_DHCP_INSTANCES; dhcp_idx++)
  {
    if (!dhcpInstances[dhcp_idx].inUse)  continue;

    dhcpInst = &dhcpInstances[dhcp_idx];

    /* Clear instance statistics */
    memset(&dhcpInst->stats_intf[ptin_port], 0x00, sizeof(ptin_DHCP_Statistics_t));

    #if (DHCP_CLIENT_INTERF_SUPPORTED)
    /* Run all clients */
    for (client_idx=0; client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE; client_idx++)
    {
      if (dhcpInst->dhcpClients.clients_in_use[client_idx]==L7_NULLPTR)  continue;

      /* Clear client statistics (if port matches) */
      clientInfo = dhcpInst->dhcpClients.clients_in_use[client_idx];
      if (clientInfo->dhcpClientDataKey.ptin_port==ptin_port)
      {
        memset(&clientInfo->client_stats, 0x00, sizeof(ptin_DHCP_Statistics_t));
      }
    }
    #endif
  }

  /* Clear global statistics */
  memset(&global_stats_intf[ptin_port], 0x00, sizeof(ptin_DHCP_Statistics_t));

  osapiSemaGive(ptin_dhcp_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear statistics of a particular DHCP instance and interface
 * 
 * @param UcastEvcId  : Unicast EVC id
 * @param intIfNum    : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_instanceIntf_clear(L7_uint16 UcastEvcId, ptin_intf_t *ptin_intf)
{
  L7_uint dhcp_idx;
  L7_uint client_idx;
  L7_uint32 ptin_port;
  st_DhcpInstCfg_t *dhcpInst;
  ptinDhcpClientInfoData_t *clientInfo;
  ptin_evc_intfCfg_t intfCfg;

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
  if (ptin_evc_intfCfg_get(UcastEvcId,ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error getting interface (%u/%u) configuration from EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,UcastEvcId);
    return L7_FAILURE;
  }
  if (!intfCfg.in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Interface %u/%u is not in use by EVC %u",ptin_intf->intf_id,ptin_intf->intf_id,UcastEvcId);
    return L7_FAILURE;
  }

  /* Get Dhcp instance */
  if (ptin_dhcp_instance_find(UcastEvcId,&dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"EVC %u does not belong to any DHCP instance",UcastEvcId);
    return L7_FAILURE;
  }

  /* Pointer to dhcp instance */
  dhcpInst = &dhcpInstances[dhcp_idx];

  osapiSemaTake(ptin_dhcp_stats_sem,-1);

  /* Clear instance statistics */
  memset(&dhcpInst->stats_intf[ptin_port], 0x00, sizeof(ptin_DHCP_Statistics_t));

  #if (DHCP_CLIENT_INTERF_SUPPORTED)
  /* Run all clients */
  for (client_idx=0; client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE; client_idx++)
  {
    if (dhcpInst->dhcpClients.clients_in_use[client_idx]==L7_NULLPTR)  continue;

    /* Clear client statistics (if port matches) */
    clientInfo = dhcpInst->dhcpClients.clients_in_use[client_idx];
    if (clientInfo->dhcpClientDataKey.ptin_port==ptin_port)
    {
      memset(&clientInfo->client_stats, 0x00, sizeof(ptin_DHCP_Statistics_t));
    }
  }
  #endif

  osapiSemaGive(ptin_dhcp_stats_sem);

  return L7_SUCCESS;
}

/**
 * Clear DHCP statistics of a particular DHCP instance and 
 * client
 * 
 * @param UcastEvcId  : Unicast EVC id
 * @param client      : client reference
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_client_clear(L7_uint16 UcastEvcId, ptin_client_id_t *client)
{
  L7_uint dhcp_idx;
  ptinDhcpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if (client==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get Dhcp instance */
  if (ptin_dhcp_instance_find(UcastEvcId,&dhcp_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"EVC %u does not belong to any DHCP instance",UcastEvcId);
    return L7_FAILURE;
  }

  /* Find client */
  if (ptin_dhcp_client_find(dhcp_idx,client,&clientInfo)!=L7_SUCCESS)
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
L7_BOOL ptin_dhcp82_intf_validate(L7_uint32 intIfNum)
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
L7_BOOL ptin_dhcp82_vlan_validate(L7_uint16 intVlanId)
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
L7_BOOL ptin_dhcp82_intfVlan_validate(L7_uint32 intIfNum, L7_uint16 intVlanId /*, L7_uint16 innerVlanId*/)
{
  L7_uint dhcp_idx;
  ptin_intf_t ptin_intf;
  ptin_evc_intfCfg_t intfCfg;
  st_DhcpInstCfg_t *dhcpInst;

  /* Validate arguments */
  if ( intIfNum==0 || intIfNum>=L7_MAX_INTERFACE_COUNT ||
       intVlanId<PTIN_VLAN_MIN || intVlanId>PTIN_VLAN_MAX )
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments: intIfNum=%u intVlan=%u",intIfNum,intVlanId);
    return L7_FALSE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid intIfNum %u",intIfNum);
    return L7_FALSE;
  }

  /* DHCP instance, from internal vlan */
  if (ptin_dhcp_inst_get_fromIntVlan(intVlanId,&dhcpInst,&dhcp_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No DHCP instance associated to intVlan %u",intVlanId);
    return L7_FALSE;
  }

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(dhcpInst->UcastEvcId,&ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error acquiring interface %u/%u configuarion from EVC id %u",
              ptin_intf.intf_type,ptin_intf.intf_id,dhcpInst->UcastEvcId);
    return L7_FALSE;
  }

  /* Interface must be in use */
  if (!intfCfg.in_use)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Interface %u/%u (intIfNum=%u) is not in use for EVC %u",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,dhcpInst->UcastEvcId);
    return L7_FALSE;
  }

  #if 0
  /* For untrusted interfaces, we must have an inner vlan (for non CXP360G) */
  #if ( PTIN_BOARD != PTIN_BOARD_CXP360G )
  if ( intfCfg.type==PTIN_EVC_INTF_LEAF && (innerVlanId==0 || innerVlanId>=4095) )
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"For unstrusted interface %u/%u (evc=%u) inner vlan must be used",
              ptin_intf.intf_type,ptin_intf.intf_id,dhcpInst->UcastEvcId);
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
L7_BOOL ptin_dhcp82_is_intfTrusted(L7_uint32 intIfNum, L7_uint16 intVlanId)
{
  L7_uint dhcp_idx;
  ptin_intf_t ptin_intf;
  ptin_evc_intfCfg_t intfCfg;
  st_DhcpInstCfg_t *dhcpInst;

  /* Validate arguments */
  if ( intIfNum==0 || intIfNum>=L7_MAX_INTERFACE_COUNT ||
       intVlanId<PTIN_VLAN_MIN || intVlanId>PTIN_VLAN_MAX )
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments: intIfNum=%u intVlan=%u",intIfNum,intVlanId);
    return L7_FALSE;
  }

  /* Convert interface to ptin_port */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid intIfNum %u",intIfNum);
    return L7_FALSE;
  }

  /* DHCP instance, from internal vlan */
  if (ptin_dhcp_inst_get_fromIntVlan(intVlanId,&dhcpInst,&dhcp_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No DHCP instance associated to intVlan %u",intVlanId);
    return L7_FALSE;
  }

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(dhcpInst->UcastEvcId,&ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error acquiring interface %u/%u configuarion from EVC id %u",
              ptin_intf.intf_type,ptin_intf.intf_id,dhcpInst->UcastEvcId);
    return L7_FALSE;
  }

  /* Interface must be in use */
  if (!intfCfg.in_use)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Interface %u/%u (intIfNum=%u) is not in use for EVC %u",
              ptin_intf.intf_type,ptin_intf.intf_id,intIfNum,dhcpInst->UcastEvcId);
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
  st_DhcpInstCfg_t *dhcpInst;
  ptinDhcpClientInfoData_t *clientInfo;

  /* Validate arguments */
  if ( client==L7_NULLPTR || client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE )
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* DHCP instance, from internal vlan */
  if (ptin_dhcp_inst_get_fromIntVlan(intVlan,&dhcpInst,L7_NULLPTR)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No DHCP instance associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Get pointer to client structure in AVL tree */
  clientInfo = dhcpInst->dhcpClients.clients_in_use[client_idx];
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
  client->innerVlan = clientInfo->dhcpClientDataKey.innerVlan;
  client->mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
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

  if (innerVlan>0 && innerVlan<4096)
  {
    /* Build client structure */
    memset(&client,0x00,sizeof(ptin_client_id_t));
    #if DHCP_CLIENT_INTERF_SUPPORTED
    client.ptin_intf.intf_type = ptin_intf.intf_type;
    client.ptin_intf.intf_id   = ptin_intf.intf_id;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
    #endif
    #if DHCP_CLIENT_INNERVLAN_SUPPORTED
    client.innerVlan = innerVlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
    #endif

    /* Find client information */
    if (ptin_dhcp_client_find(dhcp_idx,&client,&client_info)!=L7_SUCCESS)
    {
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP,"Non existent client in DHCP instance %u (EVC id %u)",dhcp_idx,dhcpInstances[dhcp_idx].UcastEvcId);
      return L7_FAILURE;
    }

    /* Return string ids */
    if (circuitId!=L7_NULLPTR)
      strncpy(circuitId,client_info->client_data.circuitId,FD_DS_MAX_REMOTE_ID_STRING);
    if (remoteId!=L7_NULLPTR)
      strncpy(remoteId ,client_info->client_data.remoteId ,FD_DS_MAX_REMOTE_ID_STRING);
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
 * Update DHCP snooping configuration, when interfaces are 
 * added/removed 
 * 
 * @param evcId     : EVC id 
 * @param ptin_intf : interface 
 * @param enable    : L7_TRUE when interface is added 
 *                    L7_FALSE when interface is removed
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_snooping_trap_interface_update(L7_uint16 evcId, ptin_intf_t *ptin_intf, L7_BOOL enable)
{
#if (!PTIN_SYSTEM_GROUP_VLANS)
  ptin_evc_intfCfg_t intfCfg;

  /* Validate arguments */
  if (evcId>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Invalid EVC id: evcId=%u",evcId);
    return L7_FAILURE;
  }

  /* This evc must be active */
  if (!ptin_evc_is_in_use(evcId))
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"EVC id is not active: evcId=%u",evcId);
    return L7_FAILURE;
  }

  /* Check if this EVC is being used by any DHCP instance */
  if (ptin_dhcp_instance_find(evcId,L7_NULLPTR)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_DHCP,"EVC %u is not used in any DHCP instance... nothing to do",evcId);
    return L7_SUCCESS;
  }

  /* Get interface configuration */
  if (ptin_evc_intfCfg_get(evcId,ptin_intf,&intfCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error acquiring interface %u/%u configuarion from EVC id %u",ptin_intf->intf_type,ptin_intf->intf_id,evcId);
    return L7_FAILURE;
  }

  /* If internal vlan associated to interface is valid, use it */
  if (intfCfg.int_vlan>=PTIN_VLAN_MIN && intfCfg.int_vlan<=PTIN_VLAN_MAX)
  {
    if (ptin_dhcpPkts_vlan_trap(intfCfg.int_vlan,enable)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error configuring to %u int_vlan %u of interface %u/%u (EVC id %u)",enable,intfCfg.int_vlan,ptin_intf->intf_type,ptin_intf->intf_id,evcId);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_DHCP,"DHCP trapping configured to %u, for vlan %u (interface %u/%u)",enable,intfCfg.int_vlan,ptin_intf->intf_type,ptin_intf->intf_id);
  }
#endif
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
    if (ptin_intf_intIfNum2port(intIfNum,&ptin_port)==L7_SUCCESS && ptin_port<PTIN_SYSTEM_N_INTERF)
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
  if (dhcpInst!=L7_NULLPTR && client_idx<PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE)
  {
    client = dhcpInst->dhcpClients.clients_in_use[client_idx];
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

  case DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITH_OPTION82:
    break;

  case DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_tx_client_requests_with_option82++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_tx_client_requests_with_option82++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_tx_client_requests_with_option82++;
    break;

  case DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_server_replies_with_option82++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_server_replies_with_option82++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_server_replies_with_option82++;
    break;

  case DHCP_STAT_FIELD_TX_SERVER_REPLIES_WITH_OPTION82:
    break;

  case DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITHOUT_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_client_requests_without_option82++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_client_requests_without_option82++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_client_requests_without_option82++;
    break;

  case DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITHOUT_OPTION82:
    break;

  case DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITHOUT_OPTION82:
    break;

  case DHCP_STAT_FIELD_TX_SERVER_REPLIES_WITHOUT_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_tx_server_replies_without_option82++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_tx_server_replies_without_option82++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_tx_server_replies_without_option82++;
    break;

  case DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOUTOP82_ON_TRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_client_pkts_withoutOp82_onTrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_client_pkts_withoutOp82_onTrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_client_pkts_withoutOp82_onTrustedIntf++;
    break;

  case DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOP82_ON_UNTRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_client_pkts_withOp82_onUntrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_client_pkts_withOp82_onUntrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_client_pkts_withOp82_onUntrustedIntf++;
    break;

  case DHCP_STAT_FIELD_RX_SERVER_PKTS_WITHOP82_ON_UNTRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_server_pkts_withOp82_onUntrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_server_pkts_withOp82_onUntrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_server_pkts_withOp82_onUntrustedIntf++;
    break;

  case DHCP_STAT_FIELD_RX_SERVER_PKTS_WITHOUTOP82_ON_TRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->dhcp_rx_server_pkts_withoutOp82_onTrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port  ->dhcp_rx_server_pkts_withoutOp82_onTrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client->dhcp_rx_server_pkts_withoutOp82_onTrustedIntf++;
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
  avl_tree = &dhcpInstances[dhcp_idx].dhcpClients.avlTree;
  memset(&avl_key,0x00,sizeof(ptinDhcpClientDataKey_t));
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
  ptinDhcpClientsAvlTree_t *avl_tree;

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

  avl_tree = &dhcpInstances[dhcp_idx].dhcpClients.avlTree;

  /* Remove all entries from AVL tree */
  avlPurgeAvlTree(&(avl_tree->dhcpClientsAvlTree), PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE);

  /* Remove all clients of AVL tree */
  dhcp_clientIndex_clearAll(dhcp_idx);

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
  L7_uint16 evc_idx, dhcp_idx;

  /* Verify if this internal vlan is associated to an EVC */
  if (ptin_evc_get_evcIdfromIntVlan(intVlan,&evc_idx)!=L7_SUCCESS)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No EVC associated to internal vlan %u",intVlan);
    return L7_FAILURE;
  }

  /* Check if the EVC has a DHCP instance */
  dhcp_idx = dhcpInst_fromEvcId[evc_idx];

  if (dhcp_idx>=PTIN_SYSTEM_N_DHCP_INSTANCES)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"No DHCP instance associated to evcId=%u (intVlan=%u)",evc_idx,intVlan);
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
  if (!ptin_evc_is_in_use(dhcpInstances[dhcp_idx].UcastEvcId))
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Inconsistency: DHCP index %u (EVCid=%u, Vlan %u) has EVC not in use (evc=%u)",dhcp_idx,evc_idx,intVlan,dhcpInstances[dhcp_idx].UcastEvcId);
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
 * @param UcastEvcId : Unicast EVC id
 * @param dhcp_idx   : DHCP instance index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_dhcp_instance_find(L7_uint16 UcastEvcId, L7_uint *dhcp_idx)
{
  L7_uint idx;

  /* Search for the provided Mcast and Ucast evcs */
  for (idx=0; idx<PTIN_SYSTEM_N_DHCP_INSTANCES; idx++)
  {
    if (!dhcpInstances[idx].inUse)  continue;

    if (dhcpInstances[idx].UcastEvcId==UcastEvcId)
      break;
  }

  /* If not found empty instances, return error */
  if (idx>=PTIN_SYSTEM_N_DHCP_INSTANCES)
    return L7_FAILURE;

  /* Return instance index */
  if (dhcp_idx!=L7_NULLPTR)  *dhcp_idx = idx;

  return L7_SUCCESS;
}

static L7_RC_t ptin_dhcp_trap_configure(L7_uint dhcp_idx, L7_BOOL enable)
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

  /* Initialize number of vlans to be configured */
  vlans_number = 0;
  uc_evcId = dhcpInstances[dhcp_idx].UcastEvcId;

  /* Get Unicast EVC configuration */
  evcCfg.index = uc_evcId;
  if (ptin_evc_get(&evcCfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error getting UC EVC %u configuration",uc_evcId);
    return L7_FAILURE;
  }
#if (!PTIN_SYSTEM_GROUP_VLANS)
  /* If UC EVC is stacked, use its root vlan */
  if (evcCfg.flags & PTIN_EVC_MASK_STACKED)
#endif
  {
    if (ptin_evc_get_intRootVlan(uc_evcId,&vlan)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Can't get UC root vlan for evc id %u",uc_evcId);
      return L7_FAILURE;
    }
    if (vlan>=PTIN_VLAN_MIN && vlan<=PTIN_VLAN_MAX)
    {
      vlan_list[vlans_number++] = vlan;
    }
  }
#if (!PTIN_SYSTEM_GROUP_VLANS)
  /* If unstacked... */
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
        LOG_ERR(LOG_CTX_PTIN_DHCP,"Error getting interface %u/%u configuration from UC EVC %u",ptin_intf.intf_type,ptin_intf.intf_id,uc_evcId);
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
          LOG_ERR(LOG_CTX_PTIN_DHCP,"Excessive number of vlans to be configured (morte than %u)",PTIN_SYSTEM_MAX_N_PORTS);
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
    if (ptin_dhcpPkts_vlan_trap(vlan_list[idx],enable)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error configuring vlan %u for packet trapping",vlan_list[idx]);
      break;
    }
    LOG_TRACE(LOG_CTX_PTIN_DHCP,"Success configuring vlan %u for packet trapping",vlan_list[idx]);
  }
  /* If something went wrong, undo configurations */
  if (idx<vlans_number)
  {
    vlans_number = idx;
    for (idx=0; idx<vlans_number; idx++)
    {
      ptin_dhcpPkts_vlan_trap(vlan_list[idx],!enable);
      LOG_WARNING(LOG_CTX_PTIN_DHCP,"Unconfiguring vlan %u for packet trapping",vlan_list[idx]);
    }
    return L7_FAILURE;
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
void ptin_dhcp_dump(void)
{
  L7_uint i, i_client;
  ptinDhcpClientDataKey_t avl_key;
  ptinDhcpClientInfoData_t *avl_info;

  for (i = 0; i < PTIN_SYSTEM_N_DHCP_INSTANCES; i++)
  {
    if (!dhcpInstances[i].inUse) {
      printf("*** Dhcp instance %02u not in use\r\n", i);
      continue;
    }

    printf("DHCP instance %02u: EVC_idx = %u\r\n", i, dhcpInstances[i].UcastEvcId);

    i_client = 0;

    /* Run all cells in AVL tree */
    memset(&avl_key,0x00,sizeof(ptinDhcpClientDataKey_t));
    while ( ( avl_info = (ptinDhcpClientInfoData_t *)
                          avlSearchLVL7(&dhcpInstances[i].dhcpClients.avlTree.dhcpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
            ) != L7_NULLPTR )
    {
      /* Prepare next key */
      memcpy(&avl_key, &avl_info->dhcpClientDataKey, sizeof(ptinDhcpClientDataKey_t));

      printf("   Client#%u: "
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
             ": index=%-4u circuitId=\"%s\" remoteId=\"%s\"\r\n",
             i_client,
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
             avl_info->client_index,
             avl_info->client_data.circuitId,
             avl_info->client_data.remoteId);

      i_client++;
    }
  }
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
static L7_RC_t ptin_dhcp82_get_intfData(L7_uint evc_idx, ptin_intf_t *ptin_intf, L7_uint32 *intIfNum, L7_uint16 *intVlanId);

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
L7_RC_t ptin_dhcp82_get(L7_uint evc_idx, ptin_intf_t *ptin_intf, L7_uint16 innerVlanId, L7_uint8 *macAddr,
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
L7_RC_t ptin_dhcp82_config(L7_uint evc_idx, ptin_intf_t *ptin_intf, L7_uint16 innerVlanId, L7_uint8 *macAddr,
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
L7_RC_t ptin_dhcp82_unconfig(L7_uint evc_idx, ptin_intf_t *ptin_intf, L7_uint16 innerVlanId, L7_uint8 *macAddr)
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
static L7_RC_t ptin_dhcp82_get_intfData(L7_uint evc_idx, ptin_intf_t *ptin_intf, L7_uint32 *intIfNum, L7_uint16 *intVlanId)
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

