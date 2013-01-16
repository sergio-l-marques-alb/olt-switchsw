/**
 * ptin_evc.c 
 *  
 * Implements the EVC configuration and management 
 *
 * Created on: 2011/07/15
 * Author: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

#include <string.h>

#include "ptin_evc.h"
#include "ptin_utils.h"
#include "ptin_intf.h"
#include "ptin_xlate_api.h"
#include "ptin_xconnect_api.h"
#include "ptin_fieldproc.h"
#include "ptin_igmp.h"
#include "ptin_dhcp.h"

#include "dot3ad_api.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_filter_api.h"
#include "usmdb_mib_vlan_api.h"

//#define EVC_COUNTERS_REQUIRE_CLEANUP_BEFORE_REMOVAL   1       /* Used for EVC remotion */
#define EVC_BWPROFILES_REQUIRE_CLEANUP_BEFORE_REMOVAL 1       /* Used for EVC remotion */
#define EVC_CLIENTS_REQUIRE_CLEANUP_BEFORE_REMOVAL    1       /* Used for EVC remotion */

/* EVC Client entry (ONLY APPLICABLE to Stacked EVCs) */
struct ptin_evc_client_s {
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct ptin_evc_client_s *next;
  struct ptin_evc_client_s *prev;

  L7_BOOL    in_use;        /* When set, indicates that this entry is valid */

  L7_uint16  inn_vlan;      /* Inner VLAN (Client VLAN) (ignored on root ports) */
  L7_uint16  out_vlan;      /* S' -> Translated S-VLAN (depends on S+C) */

  /* Counters/Profiles per client on stacked EVCs (S+C) */
  void      *counter[2];    /* Pointer to a counter struct entry */
  void      *bwprofile[2];  /* Pointer to a BW profile struct entry */
};  // sizeof=24


/* EVC Interface entry */
struct ptin_evc_intf_s {
  L7_BOOL    in_use;        /* When set, indicates that this entry is valid */

  L7_uint8   type;          /* 0:root; 1:leaf;*/

  L7_uint16  out_vlan;      /* Outer VLAN:
                             *  Root: S-VLAN
                             *  Leaf: S' (ONLY applicable to unstacked services)
                             *        (on unstacked services we allow a S->S'
                             *         xlate per leaf port) */

  L7_uint16  int_vlan;      /* Internal VLAN:
                             *  stacked   - NOT APPLICABLE
                             *  unstacked - one internal VLAN per interface */

  /* Counters/Profiles per client on unstacked EVCs (counter per leaf port) */
  void      *counter;       /* Pointer to a counter struct entry */
  void      *bwprofile;     /* Pointer to a BW profile struct entry */

  /* Clients queue */
  dl_queue_t clients;
};  // sizeof=20+12=32


/* EVC entry */
struct ptin_evc_s {
  L7_BOOL    in_use;        /* When set, indicates that this entry is valid */

  L7_uint32  flags;         /* PTIN_EVC_MASK_xxx:
                             *   0x0001 - Bundling      (not implemented)
                             *   0x0002 - All to One    (not implemented)
                             *   0x0004 - Stacked       (PTin custom field)
                             *   0x0008 - Mac Learning  (PTin custom field)
                             *   0x0100 - DHCP protocol (PTin custom field) */

  L7_uint8   type;          /* (not used)
                             *   0 - p2p
                             *   1 - mp2mp
                             *   2 - rooted mp */

  L7_uint8   mc_flood;      /* 0 - FLOOD_ALL
                             * 1 - FLOOD_UNKNOWN
                             * 2 - FLOOD_NONE */

  /* Interfaces */
  struct ptin_evc_intf_s intf[PTIN_SYSTEM_N_INTERF];

  /* Auxiliary (redundant) information */
  L7_uint32  n_clients;     /* Number of clients associated with this EVC (ONLY on stacked services) */
  L7_uint32  n_counters;    /* Number of counters associated with this EVC */
  L7_uint32  n_bwprofiles;  /* Number of BW profiles associated with this EVC */

  L7_uint8   n_roots;       /* Number of roots */
  L7_uint8   n_leafs;       /* Number of leafs */

  L7_uint16  rvlan;         /* Root VLAN (internal VLAN) 
                             *   Unstacked: Internal VLAN shared among all interfaces
                             *              to allow downstream flow (flood)
                             *   Stacked:   Unique internal VLAN among all interfaces
                             *              Cross-connects are used to forward P2P
                             *              traffic on downstream and upstream
                             */
  //struct ptin_evc_client_s *client_ref[4096];   /* Direct reference to clients information */
  dl_queue_t *queue_free_vlans;   /* Pointer to queue of free vlans */
};  // sizeof=36+32*36=1188


/* VLAN entry (for pool queue) */
struct ptin_vlan_s {
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct ptin_vlan_s *next;
  struct ptin_vlan_s *prev;

  L7_uint16 vid;
};

/* Queue entry (for pool queue) */
struct ptin_queue_s {
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct ptin_queue_s *next;
  struct ptin_queue_s *prev;

  dl_queue_t *queue;
};

/* Static array structures: EVCs + Clients + VLANs */
static struct ptin_evc_s        evcs[PTIN_SYSTEM_N_EVCS];       // sizeof=65*1188=77220
static struct ptin_evc_client_s clients[PTIN_SYSTEM_N_CLIENTS]; // sizeof=24*1024=24576
static struct ptin_vlan_s       vlans_pool[1<<12];              /* 4096 VLANs */
#if (PTIN_SYSTEM_GROUP_VLANS)
static struct ptin_queue_s      queues_pool[PTIN_SYSTEM_EVC_UNSTACKED_VLAN_BLOCKS];
#endif

/* Queues */
static dl_queue_t queue_evcs;         /* Queue of busy EVCs entries */
static dl_queue_t queue_free_evcs;    /* Queue of free EVCs entries */
static dl_queue_t queue_free_clients; /* Clients (busy) queues are mapped on each interface per EVC */

#if (PTIN_SYSTEM_GROUP_VLANS)
static dl_queue_t queue_unstacked_freeVlan_queues;
static dl_queue_t queue_stacked_free_vlans; /* Pool of free internal VLANs */
static dl_queue_t queue_unstacked_free_vlans[PTIN_SYSTEM_EVC_UNSTACKED_VLAN_BLOCKS]; /* Pool of free internal VLANs */
#else
static dl_queue_t queue_free_vlans; /* Pool of free internal VLANs */
#endif

/* List with all the ports/lags used by EVCs */
static L7_uint8 evcs_intfs_in_use[PTIN_SYSTEM_N_INTERF];

/* Reference of evcid using internal vlan as reference */
static L7_uint8 evcId_from_internalVlan[4096];

/* Local Macros */
#define IS_EVC_STACKED(evc_idx)         ((evcs[evc_idx].flags & PTIN_EVC_MASK_STACKED     ) == PTIN_EVC_MASK_STACKED)
#define IS_EVC_UNSTACKED(evc_idx)       ((evcs[evc_idx].flags & PTIN_EVC_MASK_STACKED     ) == 0 )
#define IS_EVC_WITH_CPU_TRAP(evc_idx)   ((evcs[evc_idx].flags & PTIN_EVC_MASK_CPU_TRAPPING) == PTIN_EVC_MASK_CPU_TRAPPING)

#define IS_EVC_INTF_USED(evc_idx,ptin_port) (evcs[evc_idx].intf[ptin_port].in_use)
#define IS_EVC_INTF_ROOT(evc_idx,ptin_port) (IS_EVC_INTF_USED(evc_idx,ptin_port) && evcs[evc_idx].intf[ptin_port].type==PTIN_EVC_INTF_ROOT)
#define IS_EVC_INTF_LEAF(evc_idx,ptin_port) (IS_EVC_INTF_USED(evc_idx,ptin_port) && evcs[evc_idx].intf[ptin_port].type==PTIN_EVC_INTF_LEAF)

/* DriveShell functions */
L7_RC_t ptin_evc_allclients_remove( L7_uint evc_idx );                                                    /* Used by ptin_evc_delete */
L7_RC_t ptin_evc_intfclients_remove( L7_uint evc_idx, L7_uint8 intf_type, L7_uint8 intf_id );             /* Used by ptin_evc_destroy */
L7_RC_t ptin_evc_client_remove( L7_uint evc_idx, L7_uint8 intf_type, L7_uint8 intf_id, L7_uint cvlan );

void ptin_evc_clean_counters_enable( L7_BOOL enable );
void ptin_evc_clean_profiles_enable( L7_BOOL enable );

L7_RC_t ptin_evc_clean_all( L7_uint evc_idx, L7_BOOL force );                                             /* Used by ptin_evc_delete */
L7_RC_t ptin_evc_intf_clean_all( L7_uint evc_idx, L7_uint8 intf_type, L7_uint8 intf_id, L7_BOOL force );  /* Used by ptin_evc_destroy */

L7_RC_t ptin_evc_allintfs_clean( L7_uint evc_idx, L7_BOOL force );                                        /* Used by ptin_evc_delete */
L7_RC_t ptin_evc_intf_clean( L7_uint evc_idx, L7_uint8 intf_type, L7_uint8 intf_id, L7_BOOL force );      /* Used by ptin_evc_destroy */

L7_RC_t ptin_evc_allclients_clean( L7_uint evc_idx, L7_BOOL force );
L7_RC_t ptin_evc_intfclients_clean( L7_uint evc_idx, L7_uint8 intf_type, L7_uint8 intf_id, L7_BOOL force );
L7_RC_t ptin_evc_client_clean( L7_uint evc_idx, L7_uint8 intf_type, L7_uint8 intf_id, L7_uint cvlan, L7_BOOL force );

/* Local functions prototypes */
static L7_RC_t ptin_evc_pclient_clean( L7_uint evc_idx, struct ptin_evc_client_s *pclient, L7_BOOL force );

static void    ptin_evc_entry_reset(L7_uint evc_idx);
static void    ptin_evc_vlan_pool_init(void);

static L7_RC_t ptin_evc_freeVlanQueue_allocate(L7_uint16 evc_idx, L7_BOOL is_stacked, dl_queue_t **freeVlan_queue);
static L7_RC_t ptin_evc_freeVlanQueue_free(dl_queue_t *freeVlan_queue);
static L7_RC_t ptin_evc_vlan_allocate(L7_uint16 *vlan, dl_queue_t *queue_vlans, L7_uint16 evc_idx);
static L7_RC_t ptin_evc_vlan_free(L7_uint16 vlan, dl_queue_t *queue_vlans);

static L7_RC_t ptin_evc_intf_add(L7_uint evc_idx, L7_uint ptin_intf, ptin_HwEthMef10Intf_t *intf_cfg);
static L7_RC_t ptin_evc_intf_remove(L7_uint evc_idx, L7_uint ptin_intf);
static L7_RC_t ptin_evc_intf_remove_all(L7_uint evc_idx);

static L7_RC_t ptin_evc_unstacked_intf_add(L7_uint evc_idx, L7_uint ptin_intf);
static L7_RC_t ptin_evc_unstacked_intf_remove(L7_uint evc_idx, L7_uint ptin_intf);
static L7_RC_t ptin_evc_unstacked_intf_remove_all(L7_uint evc_idx);

static void    ptin_evc_intf_list_get(L7_uint evc_idx, L7_uint8 mef_type, L7_uint intf_list[], L7_uint *n_elems);
static void    ptin_evc_find_client(L7_uint16 inn_vlan, dl_queue_t *queue, dl_queue_elem_t **pelem);

static L7_RC_t switching_root_add(L7_uint root_intf, L7_uint16 out_vlan, L7_uint16 int_vlan);
static L7_RC_t switching_root_remove(L7_uint root_intf, L7_uint16 out_vlan, L7_uint16 int_vlan);
static L7_RC_t switching_leaf_add(L7_uint leaf_intf, L7_uint16 leaf_int_vlan);
static L7_RC_t switching_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_int_vlan);

static L7_RC_t switching_stacked_leaf_add(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan);
static L7_RC_t switching_stacked_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan);

static L7_RC_t switching_unstacked_root_add(L7_uint root_intf, L7_uint16 root_out_vlan, L7_uint16 leaf_int_vlan);
static L7_RC_t switching_unstacked_root_remove(L7_uint root_intf, L7_uint16 root_out_vlan, L7_uint16 leaf_int_vlan);
static L7_RC_t switching_unstacked_leaf_add(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_int_vlan, L7_uint16 root_int_vlan);
static L7_RC_t switching_unstacked_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_int_vlan, L7_uint16 root_int_vlan);

static L7_RC_t switching_stacked_bridge_add(L7_uint root_intf, L7_uint16 root_int_vid, L7_uint leaf_intf, L7_uint16 leaf_out_vid, L7_uint16 leaf_inn_vid);
static L7_RC_t switching_stacked_bridge_remove(L7_uint root_intf, L7_uint16 root_int_vid, L7_uint leaf_intf, L7_uint16 leaf_out_vid, L7_uint16 leaf_inn_vid);

static L7_RC_t switching_vlan_create(L7_uint16 vid);
static L7_RC_t switching_vlan_delete(L7_uint16 vid);
static L7_RC_t switching_vlan_config(L7_uint16 vid, L7_uint16 fwd_vid, L7_BOOL mac_learning, L7_uint8 mc_flood, L7_uint8 cpu_trap);

static L7_RC_t ptin_evc_param_verify(ptin_HwEthMef10Evc_t *evcConf);
static L7_RC_t ptin_evc_bwProfile_verify(L7_uint evc_idx, ptin_bw_profile_t *profile, void ***bwPolicer_ptr);
static L7_RC_t ptin_evc_evcStats_verify(L7_uint evc_idx, ptin_evcStats_profile_t *profile, void ***counters_ptr);

/* EVC manipulation functions *************************************************/
/**
 * Initializes EVCs data structures
 * 
 * @return L7_RC_t L7_SUCCESS
 */
L7_RC_t ptin_evc_init(void)
{
  L7_uint i;

  LOG_INFO(LOG_CTX_PTIN_EVC, "EVC init started...");

  /* Initialize EVCs queues */
  dl_queue_init(&queue_evcs);
  dl_queue_init(&queue_free_evcs);

  /* Initialize free clients queues */
  dl_queue_init(&queue_free_clients);
  for (i=0; i<(sizeof(clients)/sizeof(clients[0])); i++)
    dl_queue_add(&queue_free_clients, (dl_queue_elem_t*)&clients[i]);

  /* Reset EVCs structs */
  for (i=0; i<(sizeof(evcs)/sizeof(evcs[0])); i++)
    ptin_evc_entry_reset(i);

  /* Reset EVCs ports/lags in use */
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
    evcs_intfs_in_use[i] = 0;

  ptin_evc_vlan_pool_init();

  LOG_INFO(LOG_CTX_PTIN_EVC, "EVC init OK");

  return L7_SUCCESS;
}

/**
 * Determines if a particular EVC is in use
 * 
 * @param evc_idx : EVC id
 * 
 * @return L7_BOOL L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_evc_is_in_use(L7_uint evc_idx)
{
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
    return L7_FALSE;

  return evcs[evc_idx].in_use;
}

/**
 * Determines if a particular Port/LAG is being used on any EVC
 * 
 * @param intf_idx PTin interface#
 * 
 * @return L7_BOOL L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_evc_is_intf_in_use(L7_uint intf_idx)
{
  if (intf_idx>=PTIN_SYSTEM_N_INTERF)
    return L7_FALSE;

  return (evcs_intfs_in_use[intf_idx] != 0);
}

/**
 * Get interface configuration within an EVC
 *  
 * @param evc_idx   : EVC index
 * @param ptin_intf : PTin interface 
 * @param cfg       : interface configuration 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intfCfg_get(L7_uint evc_idx, ptin_intf_t *ptin_intf, ptin_evc_intfCfg_t *cfg)
{
  L7_uint32 intf_idx;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_idx >= PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u is out of range [0..%u]", evc_idx, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2port(ptin_intf,&intf_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "ptin_intf is invalid");
    return L7_FAILURE;
  }

  /* Check if EVC is active */
  if (!evcs[evc_idx].in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "evc_idx %u is inactive", evc_idx);
    return L7_FAILURE;
  }

  /* Extract configuration */
  if (cfg!=L7_NULLPTR)
  {
    cfg->in_use   = evcs[evc_idx].intf[intf_idx].in_use;
    cfg->type     = evcs[evc_idx].intf[intf_idx].type;
    cfg->out_vlan = evcs[evc_idx].intf[intf_idx].out_vlan;
    cfg->int_vlan = evcs[evc_idx].intf[intf_idx].int_vlan;
  }

  return L7_SUCCESS;
}

/**
 * Gets an EVC configuration
 * 
 * @param evcConf Pointer to the output struct (index field is used as input param)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_get(ptin_HwEthMef10Evc_t *evcConf)
{
  L7_uint evc_idx;
  L7_uint i;

  evc_idx = evcConf->index;

  //LOG_INFO(LOG_CTX_PTIN_EVC, "Reading EVC# %u...", evc_idx);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_idx >= PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u is out of range [0..%u]", evc_idx, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (!evcs[evc_idx].in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u is not in use", evc_idx);
    return L7_FAILURE;
  }

  /* Copy data to the output struct */
  evcConf->flags          = evcs[evc_idx].flags;
  evcConf->type           = evcs[evc_idx].type;
  evcConf->mc_flood       = evcs[evc_idx].mc_flood;
  memset(evcConf->ce_vid_bmp, 0x00, sizeof(evcConf->ce_vid_bmp));

  evcConf->n_intf = 0;
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
  {
    if (evcs[evc_idx].intf[i].in_use)
    {
      if (i < PTIN_SYSTEM_N_PORTS)
      {
        evcConf->intf[evcConf->n_intf].intf_id   = i;
        evcConf->intf[evcConf->n_intf].intf_type = PTIN_EVC_INTF_PHYSICAL;
      }
      else
      {
        evcConf->intf[evcConf->n_intf].intf_id   = i - PTIN_SYSTEM_N_PORTS;
        evcConf->intf[evcConf->n_intf].intf_type = PTIN_EVC_INTF_LOGICAL;
      }
      evcConf->intf[evcConf->n_intf].mef_type = evcs[evc_idx].intf[i].type;
      evcConf->intf[evcConf->n_intf].vid      = evcs[evc_idx].intf[i].out_vlan;
      evcConf->n_intf++;
    }
  }

  /* NOTE: please refer to the ptin_HwEthMef10Evc struct definition in order to
   *       understand how the intf array is managed */

  return L7_SUCCESS;
}

/**
 * Gets an EVC configuration from an internal vlan as input 
 * parameter 
 *  
 * @param internalVlan : Internal vlan
 * @param evcConf      : Pointer to the evc configuration struct
 * 
 * @return L7_RC_t : L7_SUCCESS   - Success extracting evc data
 *                   L7_FAILURE   - Failure reading evc data
 */
L7_RC_t ptin_evc_get_fromIntVlan(L7_uint16 internalVlan, ptin_HwEthMef10Evc_t *evcConf)
{
  L7_uint evc_idx;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if (internalVlan>=4096)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments (intVlan=%u)",internalVlan);
    return L7_FAILURE;
  }

  /* Get evc id */
  evc_idx = evcId_from_internalVlan[internalVlan];

  /* Check if this internal vlan is in use by any evc */
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    return L7_FAILURE;
  }

  /* Check if this evc is in use... if not we have a non-consistent situation */
  if (!evcs[evc_idx].in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Non-consistent situation: evc %u (intVlan=%u) should be in use",evc_idx,internalVlan);
    return L7_FAILURE;
  }

  /* Get evc data */
  if (evcConf!=L7_NULLPTR)
  {
    evcConf->index = evc_idx;
    rc = ptin_evc_get(evcConf);
    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"Error getting evc data from internalVlan=%u, evcId=%u",internalVlan,evc_idx);
    }
  }

  return rc;
}

/**
 * Gets an EVC configuration from an internal vlan as input 
 * parameter 
 *  
 * @param internalVlan : Internal vlan
 * @param evc_id       : EVC id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_get_evcIdfromIntVlan(L7_uint16 internalVlan, L7_uint16 *evc_id)
{
  L7_uint evc_idx;

  /* Validate arguments */
  if (internalVlan>=4096)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments (intVlan=%u)",internalVlan);
    return L7_FAILURE;
  }

  /* Get evc id */
  evc_idx = evcId_from_internalVlan[internalVlan];

  /* Check if this internal vlan is in use by any evc */
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    return L7_FAILURE;
  }

  /* Check if this evc is in use... if not we have a non-consistent situation */
  if (!evcs[evc_idx].in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Non-consistent situation: evc %u (intVlan=%u) should be in use",evc_idx,internalVlan);
    return L7_FAILURE;
  }

  if (evc_id!=L7_NULLPTR)  *evc_id = evc_idx;

  return L7_SUCCESS;
}

/**
 * Gets the root vlan (internal) for a particular evc
 * 
 * @param evc_id      : EVC id 
 * @param intRootVlan : Internal root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_get_intRootVlan(L7_uint16 evc_id, L7_uint16 *intRootVlan)
{
  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid evc %u",evc_id);
    return L7_FAILURE;
  }

  /* EVC must be in use */
  if (!evcs[evc_id].in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Evc %u is not in use",evc_id);
    return L7_FAILURE;
  }

  /* Root vlan must be defined */
  if (evcs[evc_id].rvlan<PTIN_VLAN_MIN || evcs[evc_id].rvlan>PTIN_VLAN_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Evc %u has an invalid root vlan (%u)",evc_id,evcs[evc_id].rvlan);
    return L7_FAILURE;
  }

  /* Return root vlan */
  if (intRootVlan!=L7_NULLPTR)  *intRootVlan = evcs[evc_id].rvlan;

  return L7_SUCCESS;
}

/**
 * Get the outer+inner external vlan for a specific 
 * interface+evc_idx+innerVlan. 
 *  
 * @param intIfNum  : FP interface#
 * @param evc_idx   : EVC index
 * @param innerVlan : Inner vlan
 * @param extOVlan  : External outer-vlan 
 * @param extIVlan  : External inner-vlan (01 means that there 
 *                     is no inner vlan)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_extVlans_get(L7_uint32 intIfNum, L7_uint16 evc_idx, L7_uint16 innerVlan, L7_uint16 *extOVlan, L7_uint16 *extIVlan)
{
  L7_uint32 ptin_port;
  L7_uint16 ovid, ivid;
  struct ptin_evc_client_s *pclient;

  /* Validate arguments */
  if (intIfNum==0 || evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_idx].in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Non-consistent situation: evc %u should be in use",evc_idx);
    return L7_FAILURE;
  }

  /* Extract and validate intIfNum */
  if (ptin_intf_intIfNum2port(intIfNum,&ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid intIfNum (%u)",intIfNum);
    return L7_FAILURE;
  }
  if (!evcs[evc_idx].intf[ptin_port].in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"IntIfNum=%u/ptin_port=%u is not used in EVC=%u",intIfNum,ptin_port,evc_idx);
    return L7_FAILURE;
  }

  /* Initialize external outer+inner vlans */
  ovid = 0;
  ivid = 0;

  /* For stacked EVCs... */
  if (IS_EVC_STACKED(evc_idx))
  {
    /* Interface is root? */
    if (evcs[evc_idx].intf[ptin_port].type==PTIN_EVC_INTF_ROOT)
    {
      ovid = evcs[evc_idx].intf[ptin_port].out_vlan;
      ivid = innerVlan;
    }
    /* Interface is leaf? */
    else
    {
      /* Find this client vlan in EVC */
      ptin_evc_find_client(innerVlan, &(evcs[evc_idx].intf[ptin_port].clients), (dl_queue_elem_t **) &pclient);
      if (pclient==NULL)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC,"There is no client with cvid=%u in IntIfNum=%u/ptin_port=%u and EVC=%u",innerVlan,intIfNum,ptin_port,evc_idx);
        return L7_FAILURE;
      }
      ovid = pclient->out_vlan;
      ivid = 0;                 /* Inner vlan will be removed */
    }
  }
  /* For unstacked EVCs... */
  else
  {
    ovid = evcs[evc_idx].intf[ptin_port].out_vlan;
    ivid = innerVlan;
  }

  /* Return output values */
  if (extOVlan!=L7_NULLPTR)  *extOVlan = ovid;
  if (extIVlan!=L7_NULLPTR)  *extIVlan = ivid;

  return L7_SUCCESS;
}

/**
 * Get the list of interfaces associated to a internal vlan
 * 
 * @param intVlan  : Internal vlan 
 * @param type     : Interface type 
 *                    PTIN_EVC_INTF_ROOT=0,
 *                    PTIN_EVC_INTF_LEAF=1
 * @param intfList : List of interfaces
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intfType_getList(L7_uint16 intVlan, L7_uint8 type, NIM_INTF_MASK_t *intfList)
{
  ptin_HwEthMef10Evc_t evcCfg;
  L7_uint intf_idx;
  L7_uint32 intIfNum;
  ptin_intf_t ptin_intf;
  L7_uint16 evc_idx;

  /* Validate arguments */
  if (intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get evc id and validate it */
  evc_idx = evcId_from_internalVlan[intVlan];
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Internal Outer vlan (%u) is not used in any EVC",intVlan);
    return L7_FAILURE;
  }

  /* Get MC EVC configuration */
  memset(&evcCfg,0x00,sizeof(ptin_HwEthMef10Evc_t));
  evcCfg.index = evc_idx;
  if (ptin_evc_get(&evcCfg)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting evc %u configuration (intVlan=%u)",evc_idx,intVlan);
    return L7_FAILURE;
  }

  /* interface list pointer must not be null */
  if (intfList==L7_NULLPTR)
  {
    return L7_SUCCESS;
  }

  /* clear interface list */
  memset(intfList,0x00,sizeof(NIM_INTF_MASK_t));

  /* Run all interfaces */
  for (intf_idx=0; intf_idx<evcCfg.n_intf; intf_idx++)
  {
    if (evcCfg.intf[intf_idx].mef_type==type)
    {
      ptin_intf.intf_type = evcCfg.intf[intf_idx].intf_type;
      ptin_intf.intf_id   = evcCfg.intf[intf_idx].intf_id;

      if (ptin_intf_ptintf2intIfNum(&ptin_intf,&intIfNum)==L7_SUCCESS)
      {
        NIM_INTF_SETMASKBIT(*intfList,intIfNum);
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
 * Get the outer+inner external vlan for a specific 
 * interface+outer+inner internal vlan. 
 *  
 * @param intIfNum  : FP interface#
 * @param intOVlan   : Internal outer-vlan 
 * @param intIVlan   : Internal inner-vlan (0 means that there 
 *                     is no inner vlan)
 * @param extOVlan   : External outer-vlan 
 * @param extIVlan   : External inner-vlan (01 means that there 
 *                     is no inner vlan)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_extVlans_get_fromIntVlan(L7_uint32 intIfNum, L7_uint16 intOVlan, L7_uint16 intIVlan, L7_uint16 *extOVlan, L7_uint16 *extIVlan)
{
  L7_uint evc_idx;

  /* Validate arguments */
  if (intIfNum==0 || intOVlan<PTIN_VLAN_MIN || intOVlan>PTIN_VLAN_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get evc id and validate it */
  evc_idx = evcId_from_internalVlan[intOVlan];
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Internal Outer vlan (%u) is not used in any EVC",intOVlan);
    return L7_FAILURE;
  }

  /* Get external vlans */
  if (ptin_evc_extVlans_get(intIfNum, evc_idx, intIVlan, extOVlan, extIVlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Error getting external vlans for intIfNum=%u, evc_idx=%u, intIVlan=%u",intIfNum,evc_idx,intIVlan);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Creates or reconfigures an EVC
 * 
 * @param evcConf Pointer to the input struct
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_create(ptin_HwEthMef10Evc_t *evcConf)
{
  L7_uint   i;
  L7_uint   evc_idx;
  L7_int    intf2cfg[PTIN_SYSTEM_N_INTERF]; /* Lookup array to map sequential to indexed intf */
  L7_BOOL   stacked;
  L7_BOOL   maclearning;
  L7_BOOL   dhcp_enabled;
  L7_BOOL   cpu_trap;
  L7_BOOL   error = L7_FALSE;
  L7_uint   n_roots;
  L7_uint   n_leafs;
  L7_uint16 root_vlan;
  L7_uint   ptin_intf;
  L7_uint32 intIfNum;
  dl_queue_t *freeVlan_queue;

  evc_idx = evcConf->index;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Configuring EVC# %u...", evc_idx);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_idx >= PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u is out of range [0..%u]", evc_idx, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  /* Validate parameters */
  if (ptin_evc_param_verify(evcConf)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Given EVC has invalid parameters... nothing done!");
    return L7_FAILURE;
  }

  stacked      = (evcConf->flags & PTIN_EVC_MASK_STACKED)       == PTIN_EVC_MASK_STACKED;
  maclearning  = (evcConf->flags & PTIN_EVC_MASK_MACLEARNING)   == PTIN_EVC_MASK_MACLEARNING;
  dhcp_enabled = (evcConf->flags & PTIN_EVC_MASK_DHCP_PROTOCOL) == PTIN_EVC_MASK_DHCP_PROTOCOL;
  cpu_trap     = (evcConf->flags & PTIN_EVC_MASK_CPU_TRAPPING)  == PTIN_EVC_MASK_CPU_TRAPPING;

  /* Get the number of Roots and Leafs of received msg (for validation purposes) */
  n_roots = 0;
  n_leafs = 0;
  for (i=0; i<evcConf->n_intf; i++)
  {
    if (evcConf->intf[i].mef_type == PTIN_EVC_INTF_ROOT)
      n_roots++;
    else
      n_leafs++;
  }

  /* Do not accept:
   *   1. no roots
   *   2. only one root without leafs (except for inBand EVC# PTIN_EVC_INBAND)
   *   3. leafs in EVC# PTIN_EVC_INBAND */
  if ((n_roots == 0) ||
      /*(n_roots != 0 && n_leafs == 0 && evc_idx != PTIN_EVC_INBAND) ||*/
      (evc_idx == PTIN_EVC_INBAND && n_leafs != 0))
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: combination of roots/leafs is invalid! [roots=%u leafs=%u]",
            evc_idx, n_roots, n_leafs);
    return L7_FAILURE;
  }

  /* For EVC# PTIN_EVC_INBAND, it must be unstacked! */
  if (evc_idx == PTIN_EVC_INBAND && stacked)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: inBand EVC cannot be stacked!", evc_idx);
    return L7_FAILURE;
  }

  /* For Stacked EVCs, no more than one root is allowed */
  if (stacked && (n_roots > 1))
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: stacked EVCs cannot have more than one root port", evc_idx);
    return L7_FAILURE;
  }

  /* Check if phy ports are already assigned to LAGs */
  for (i=0; i<evcConf->n_intf; i++)
  {
    if (evcConf->intf[i].intf_type == PTIN_EVC_INTF_PHYSICAL)
    {
      ptin_intf_port2intIfNum(evcConf->intf[i].intf_id, &intIfNum);
      if (dot3adAggGet(intIfNum, &intIfNum) == L7_SUCCESS)
      {
        ptin_intf_intIfNum2port(intIfNum, &ptin_intf);
        LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: port# %u belongs to LAG# %u", evc_idx,
                evcConf->intf[i].intf_id, ptin_intf - PTIN_SYSTEM_N_PORTS);
        return L7_FAILURE;
      }
    }
  }

  /* New EVC ? */
  if (!evcs[evc_idx].in_use)
  {
    LOG_INFO(LOG_CTX_PTIN_EVC, "EVC# %u: creating a new entry...", evc_idx);

    /* Allocate queue of free vlans */
    if (ptin_evc_freeVlanQueue_allocate(evc_idx,stacked,&freeVlan_queue)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: Error allocating free vlan queue", evc_idx);
      return L7_FAILURE;
    }

    /* EXCEPTION: EVC# PTIN_EVC_INBAND is for inBand management, which means a fixed root VLAN ID */
    if (evc_idx == PTIN_EVC_INBAND)
    {
      if (switching_vlan_create(PTIN_VLAN_INBAND) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error creating VLAN ID %u for inBand management purposes", evc_idx, PTIN_VLAN_INBAND);
        ptin_evc_freeVlanQueue_free(freeVlan_queue);
        return L7_FAILURE;
      }
      root_vlan = PTIN_VLAN_INBAND;
    }
    else
    {
      /* Check if there are enough internal VLANs on the pool
       *  stacked:   only one internal VLAN is needed (shared among all the ports)
       *  unstacked: one VLAN is needed per leaf port plus one for all the root ports */
      if ( ( stacked  && (freeVlan_queue->n_elems < 1) ) ||
           ( !stacked && (freeVlan_queue->n_elems < (n_leafs + 1)) ) )
      {
        LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: not enough internal VLANs available", evc_idx);
        ptin_evc_freeVlanQueue_free(freeVlan_queue);
        return L7_FAILURE;
      }

      /* Get a VLAN from the pool to use as Internal Root VLAN */
      ptin_evc_vlan_allocate(&root_vlan, freeVlan_queue, evc_idx);  /* cannot fail! */
    }

    /* For stacked EVCs, we need to enable forwarding mode to OVID+IVID */
    if (stacked)
      ptin_crossconnect_enable(root_vlan, L7_TRUE);

    /* Update EVC entry (this info will be used on the configuration functions) */
    evcs[evc_idx].in_use           = L7_TRUE;
    evcs[evc_idx].flags            = evcConf->flags;
    evcs[evc_idx].type             = evcConf->type;
    evcs[evc_idx].mc_flood         = evcConf->mc_flood;
    evcs[evc_idx].rvlan            = root_vlan;         /* ...above created */
    evcs[evc_idx].queue_free_vlans = freeVlan_queue;

    error = L7_FALSE;

    /* Add a broadcast rate limiter for unstacked services */
    if (!stacked && cpu_trap)
    {
      if (ptin_broadcast_rateLimit(L7_ENABLE,root_vlan)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: Error applying rate limit for broadcast traffic", evc_idx);
        error = L7_TRUE;
      }
      else
      {
        LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: Success applying rate limit for broadcast traffic", evc_idx);
      }
    }

    /* Configure each interface */
    for (i=0; i<evcConf->n_intf; i++)
    {
      /* Convert Phy/Lag# into PTin Intf index */
      if (evcConf->intf[i].intf_type == PTIN_EVC_INTF_PHYSICAL)
        ptin_intf = evcConf->intf[i].intf_id;
      else
        ptin_intf = evcConf->intf[i].intf_id + PTIN_SYSTEM_N_PORTS;

      /* Apply config */
      if (ptin_evc_intf_add(evc_idx, ptin_intf, &evcConf->intf[i]) != L7_SUCCESS)
      {
        error = L7_TRUE;
        break;
      }

      /* On Unstacked EVCs, a "bridge" needs to be established between each leaf and all root interfaces */
      if (IS_EVC_UNSTACKED(evc_idx))
      {
        if (ptin_evc_unstacked_intf_add(evc_idx, ptin_intf) != L7_SUCCESS)
        {
          error = L7_TRUE;
          break;
        }
      }
    }

    /* If DHCP is enabled, add DHCP instance */
    if (dhcp_enabled)
    {
      if (ptin_dhcp_instance_add(evc_idx)!=L7_SUCCESS)
      {
        error = L7_TRUE;
      }
    }

    /* Error occured ? */
    if (error)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: failed to be created", evc_idx);

      /* Remove DHCP instance */
      if (dhcp_enabled)
      {
        ptin_dhcp_instance_remove(evc_idx);
      }

      /* Remove bridges on unstacked EVCs */
      if (IS_EVC_UNSTACKED(evc_idx))
        ptin_evc_unstacked_intf_remove_all(evc_idx);
      
      /* Remove all previously configured interfaces */
      ptin_evc_intf_remove_all(evc_idx);

      /* Remove the broadcast rate limiter for unstacked services */
      if (!stacked && cpu_trap)
      {
        ptin_broadcast_rateLimit(L7_DISABLE,root_vlan);
      }

      if (evc_idx == PTIN_EVC_INBAND)
        switching_vlan_delete(PTIN_VLAN_INBAND);
      else
        ptin_evc_vlan_free(root_vlan, freeVlan_queue);
      
      ptin_evc_entry_reset(evc_idx);

      return L7_FAILURE;
    }
  }
  /* EVC is in use: ONLY allow adding or removing ports */
  else
  {
    LOG_INFO(LOG_CTX_PTIN_EVC, "EVC# %u: applying new config...", evc_idx);

    /* Create a lookup table to map the evcConf.intf (sequecial) to the evc.intf (indexed) */
    for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
      intf2cfg[i] = -1;   /* A negative value means no mapping... */
    for (i=0; i<evcConf->n_intf; i++)
    {
      if (evcConf->intf[i].intf_type == PTIN_EVC_INTF_PHYSICAL)
        intf2cfg[evcConf->intf[i].intf_id] = i;
      else
        intf2cfg[evcConf->intf[i].intf_id + PTIN_SYSTEM_N_PORTS] = i;
    }

    /* Check if ports are added or removed */
    error = L7_FALSE;
    for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
    {
      /* Port added ? */
      if ((evcs[evc_idx].intf[i].in_use == 0) && (intf2cfg[i] >= 0))
      {
        LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: adding interface# %u...", evc_idx, i);

        if (ptin_evc_intf_add(evc_idx, i, &evcConf->intf[intf2cfg[i]]) != L7_SUCCESS)
        {
          /* Signal error, but try to process the rest of the config */
          error = L7_TRUE;
          continue;
        }

        /* NOTE: in unstacked EVCs, a bridge needs to be added between each leaf
         * and all the root interfaces */
        if (IS_EVC_UNSTACKED(evc_idx))
        {
          if (ptin_evc_unstacked_intf_add(evc_idx, i) != L7_SUCCESS)
          {
            /* Signal error, but try to process the rest of the config */
            error = L7_TRUE;
            continue;
          }
        }
      }

      /* Port removed ? */
      else if ((evcs[evc_idx].intf[i].in_use) && (intf2cfg[i] < 0))
      {
        LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: removing interface# %u...", evc_idx, i);

        /* Do not allow port removal if counters or BW profiles are configured */
        if ((evcs[evc_idx].intf[i].counter  != NULL) ||
            (evcs[evc_idx].intf[i].bwprofile!= NULL))
        {
          LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: %s# %u has counter/BW profile configured! Cannot remove it!",
                  evc_idx,
                  i < PTIN_SYSTEM_N_PORTS ? "PHY":"LAG",
                  i < PTIN_SYSTEM_N_PORTS ? i : i - PTIN_SYSTEM_N_PORTS);

          /* Signal error, but try to process the rest of the config */
          error = L7_TRUE;
          continue;
        }

        /* If it is an unstacked EVC, we need to remove the bridge before removing the interface */
        if (IS_EVC_UNSTACKED(evc_idx))
        {
          if (ptin_evc_unstacked_intf_remove(evc_idx, i) != L7_SUCCESS)
          {
            /* Signal error, but try to process the rest of the config */
            error = L7_TRUE;
            continue;
          }
        }

        if (ptin_evc_intf_remove(evc_idx, i) != L7_SUCCESS)
        {
          /* Signal error, but try to process the rest of the config */
          error = L7_TRUE;
          continue;
        }
      }
    }

    /* If DHCP is enabled, add DHCP instance */
    if (dhcp_enabled)
    {
      ptin_dhcp_instance_add(evc_idx);
    }
  }

  LOG_INFO(LOG_CTX_PTIN_EVC, "EVC# %u successfully created", evc_idx);

  return L7_SUCCESS;
}

/**
 * Deletes an EVC
 * 
 * @param evc_idx
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_delete(L7_uint evc_idx)
{
  LOG_INFO(LOG_CTX_PTIN_EVC, "Deleting EVC# %u...", evc_idx);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_idx >= PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u is out of range [0..%u]", evc_idx, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (!evcs[evc_idx].in_use)
  {
    LOG_WARNING(LOG_CTX_PTIN_EVC, "EVC# %u is not in use", evc_idx);
    return L7_SUCCESS;
  }

  /* IF this EVC belongs to an IGMP instance, stop procedure */
  if (ptin_igmp_is_evc_used(evc_idx))
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: This EVC belongs to an IGMP instance... So it cannot be removed!",
            evc_idx);
    return L7_FAILURE;
  }

  #ifdef EVC_COUNTERS_REQUIRE_CLEANUP_BEFORE_REMOVAL
  /* Check if there are counters pending... */
  if (evcs[evc_idx].n_counters > 0)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: %u counters are still configured! EVC cannot be removed!",
            evc_idx, evcs[evc_idx].n_counters);
    return L7_FAILURE;
  }
  #endif
  #ifdef EVC_BWPROFILES_REQUIRE_CLEANUP_BEFORE_REMOVAL
  /* Check if there are BW profiles pending... */
  if (evcs[evc_idx].n_bwprofiles > 0)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: %u BW profiles are still configured! EVC cannot be removed!",
            evc_idx, evcs[evc_idx].n_bwprofiles);
    return L7_FAILURE;
  }
  #endif
  #ifdef EVC_CLIENTS_REQUIRE_CLEANUP_BEFORE_REMOVAL
  /* Check if there are clients pending... */
  if (evcs[evc_idx].n_clients > 0)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: %u clients are still configured! EVC cannot be removed!",
            evc_idx, evcs[evc_idx].n_clients);
    return L7_FAILURE;
  }
  #endif

  #if 0
  {
    /* Clean profiles and counters to all EVC interfaces */
    if (ptin_evc_clean_all(evc_idx, L7_TRUE)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: Error cleaning profiles and counters! EVC cannot be removed!",
              evc_idx);
      return L7_FAILURE;
    }
  }
  #endif

  /* Only clean service resources... Clients remotion will do the rest */
  if (ptin_evc_allintfs_clean(evc_idx, L7_TRUE)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: Error cleaning service profiles and counters! EVC cannot be removed!",
            evc_idx);
    return L7_FAILURE;
  }
  /* Clean any clients that may exist */
  if (ptin_evc_allclients_remove(evc_idx)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: Error removing all clients! EVC cannot be removed!",
            evc_idx);
    return L7_FAILURE;
  }

  /* Remove bridges on unstacked EVCs */
  if (IS_EVC_UNSTACKED(evc_idx))
  {
    if (ptin_evc_unstacked_intf_remove_all(evc_idx) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error removing (unstacked) bridges config", evc_idx);
      return L7_FAILURE;
    }
  }

  /* Remove all configured interfaces */
  if (ptin_evc_intf_remove_all(evc_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error removing interfaces config", evc_idx);
    return L7_FAILURE;
  }

  /* Remove BCast rate limit for unstacked services */
  if (IS_EVC_UNSTACKED(evc_idx) && IS_EVC_WITH_CPU_TRAP(evc_idx))
  {
    ptin_broadcast_rateLimit(L7_DISABLE, evcs[evc_idx].rvlan);
    LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: Broadcast rate limit removed", evc_idx);
  }

  /* If this EVC is for InBand, the allocated VLAN must be deleted directly! */
  if (evc_idx == PTIN_EVC_INBAND)
    switching_vlan_delete(PTIN_VLAN_INBAND);
  else
    ptin_evc_vlan_free(evcs[evc_idx].rvlan, evcs[evc_idx].queue_free_vlans);

  /* For DHCP enabled EVCs */
  if (ptin_dhcp_is_evc_used(evc_idx))
    ptin_dhcp_instance_remove(evc_idx);

  ptin_evc_entry_reset(evc_idx);

  LOG_INFO(LOG_CTX_PTIN_EVC, "EVC# %u successfully removed", evc_idx);

  return L7_SUCCESS;
}


/**
 * Destroys an EVC
 * 
 * @param evc_idx
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_destroy(L7_uint evc_idx)
{
  L7_uint     intf_idx;
  ptin_intf_t ptin_intf;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Destroying EVC# %u...", evc_idx);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_idx >= PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u is out of range [0..%u]", evc_idx, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (!evcs[evc_idx].in_use)
  {
    LOG_WARNING(LOG_CTX_PTIN_EVC, "EVC# %u is not in use", evc_idx);
    return L7_SUCCESS;
  }

  /* IF this EVC belongs to an IGMP instance, destroy that instance */
  if (ptin_igmp_is_evc_used(evc_idx))
    ptin_igmp_instance_destroy(evc_idx);
  /* IF this EVC belongs to a DHCP instance, destroy that instance */
  if (ptin_dhcp_is_evc_used(evc_idx))
    ptin_dhcp_instance_destroy(evc_idx);

  /* IF this EVC belongs to an DHCP instance, destroy that instance */
  if (evcs[evc_idx].flags & PTIN_EVC_MASK_DHCP_PROTOCOL)
  {
    LOG_WARNING(LOG_CTX_PTIN_EVC, "EVC# %u: DHCP is not cleared!!!", evc_idx);
//    TODO !!!
//    ptin_dhcp_instance_destroy(evc_idx);
  }

  /* For each interface... */
  for (intf_idx=0; intf_idx<PTIN_SYSTEM_N_INTERF; intf_idx++)
  {
    if (!evcs[evc_idx].intf[intf_idx].in_use)
      continue;

    if (ptin_intf_port2ptintf(intf_idx,&ptin_intf)!=L7_SUCCESS)
      continue;

    /* Clean EVC */
    #if 0
    {
      if (ptin_evc_intf_clean_all(evc_idx, ptin_intf.intf_type, ptin_intf.intf_id, L7_TRUE)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: Error cleaning profiles and counters!!!", evc_idx);
        return L7_FAILURE;
      }
    }
    #else
    {
      /* Only clean service resources... Clients remotion will do the rest */
      if (ptin_evc_intf_clean(evc_idx, ptin_intf.intf_type, ptin_intf.intf_id, L7_TRUE)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: Error cleaning service profiles and counters!!!", evc_idx);
        return L7_FAILURE;
      }
    }
    #endif

    /* Remove all clients */
    if (ptin_evc_intfclients_remove(evc_idx, ptin_intf.intf_type, ptin_intf.intf_id)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: Error removing clients!!!", evc_idx);
      return L7_FAILURE;
    }
  }

  /* Check if there are counters pending... */
  if (evcs[evc_idx].n_counters > 0)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: %u counters are still configured! EVC cannot be destroyed!",
            evc_idx, evcs[evc_idx].n_counters);
    return L7_FAILURE;
  }
  /* Check if there are BW profiles pending... */
  if (evcs[evc_idx].n_bwprofiles > 0)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: %u BW profiles are still configured! EVC cannot be destroyed!",
            evc_idx, evcs[evc_idx].n_bwprofiles);
    return L7_FAILURE;
  }
  /* Check if there are clients pending... */
  if (evcs[evc_idx].n_clients > 0)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: %u clients are still configured! EVC cannot be destroyed!",
            evc_idx, evcs[evc_idx].n_clients);
    return L7_FAILURE;
  }

  /* Remove bridges on unstacked EVCs */
  if (IS_EVC_UNSTACKED(evc_idx))
  {
    if (ptin_evc_unstacked_intf_remove_all(evc_idx) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error removing (unstacked) bridges config", evc_idx);
      return L7_FAILURE;
    }
  }

  /* Remove all previously configured interfaces */
  if (ptin_evc_intf_remove_all(evc_idx) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error removing interfaces config", evc_idx);
    return L7_FAILURE;
  }

  /* Remove BCast rate limit for unstacked services */
  if (IS_EVC_UNSTACKED(evc_idx) && IS_EVC_WITH_CPU_TRAP(evc_idx))
  {
    ptin_broadcast_rateLimit(L7_DISABLE, evcs[evc_idx].rvlan);
    LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: Broadcast rate limit removed", evc_idx);
  }

  /* If this EVC is for InBand, the allocated VLAN must be deleted directly! */
  if (evc_idx == PTIN_EVC_INBAND)
    switching_vlan_delete(PTIN_VLAN_INBAND);
  else
    ptin_evc_vlan_free(evcs[evc_idx].rvlan, evcs[evc_idx].queue_free_vlans);

  ptin_evc_entry_reset(evc_idx);

  LOG_INFO(LOG_CTX_PTIN_EVC, "EVC# %u successfully destroyed", evc_idx);

  return L7_SUCCESS;
}


/**
 * Destroys all EVCs (except INBAND!)
 * 
 * @return L7_RC_t L7_SUCCESS
 */
L7_RC_t ptin_evc_destroy_all(void)
{
  L7_uint i;

  for (i=1; i<PTIN_SYSTEM_N_EVCS; i++)
    if (evcs[i].in_use)
      ptin_evc_destroy(i);

  return L7_SUCCESS;
}


/**
 * Adds a bridge to a stacked EVC between the root and a particular interface
 * 
 * @param evcBridge Bridge info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_stacked_bridge_add(ptin_HwEthEvcBridge_t *evcBridge)
{
  L7_uint evc_idx;
  L7_uint root_intf;
  L7_uint leaf_intf;
  L7_uint i;
  L7_RC_t rc = L7_SUCCESS;
  struct ptin_evc_client_s *pclient;

  evc_idx = evcBridge->index;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Adding EVC# %u bridge connection...", evc_idx);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_idx >= PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u is out of range [0..%u]", evc_idx, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (!evcs[evc_idx].in_use)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u is not in use", evc_idx);
    return L7_FAILURE;
  }

  /* Check if the EVC is stacked */
  if (IS_EVC_UNSTACKED(evc_idx))
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u is not stacked!!!", evc_idx);
    return L7_FAILURE;
  }

  /* Determine leaf ptin_intf */
  if (evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL)
    leaf_intf = evcBridge->intf.intf_id;
  else
    leaf_intf = evcBridge->intf.intf_id + PTIN_SYSTEM_N_PORTS;

  /* Validate leaf interface (from received message) */
  if ((leaf_intf >= PTIN_SYSTEM_N_INTERF) ||
      (!evcs[evc_idx].intf[leaf_intf].in_use) ||
      (evcs[evc_idx].intf[leaf_intf].type != PTIN_EVC_INTF_LEAF))
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: %s# %u is invalid", evc_idx,
            evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id);
    return L7_FAILURE;
  }

  /* Get root port */
  root_intf = PTIN_SYSTEM_N_INTERF;
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
  {
    if (evcs[evc_idx].intf[i].in_use &&
        evcs[evc_idx].intf[i].type == PTIN_EVC_INTF_ROOT)
    {
      root_intf = i;
      break;
    }
  }

  /* Check if client entry already exists */
  ptin_evc_find_client(evcBridge->inn_vlan, &evcs[evc_idx].intf[leaf_intf].clients, (dl_queue_elem_t**) &pclient);
  if (pclient != NULL)
  {
    LOG_WARNING(LOG_CTX_PTIN_EVC, "EVC# %u: %s# %u already have a bridge with Inner VID = %u", evc_idx,
                evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id, evcBridge->inn_vlan);
    return L7_SUCCESS;
  }

  LOG_INFO(LOG_CTX_PTIN_EVC, "EVC# %u: adding bridge [Root: Intf=%u IntVID=%u] <=> [Leaf: Intf=%u NEW Out.VID=%u Inn.VID=%u] ...", evc_idx,
           root_intf, evcs[evc_idx].rvlan, leaf_intf, evcBridge->intf.vid, evcBridge->inn_vlan);

  /* Add bridge between root and leaf port (Proot, Vr, Pleaf, Vs', Vc) */
  rc = switching_stacked_bridge_add(root_intf, evcs[evc_idx].rvlan, leaf_intf, evcBridge->intf.vid, evcBridge->inn_vlan);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error adding bridge", evc_idx,
            evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id);
    return L7_FAILURE;
  }

  /* Add client to the EVC struct */
  dl_queue_remove_head(&queue_free_clients, (dl_queue_elem_t**) &pclient);  /* get a free client entry */
  pclient->in_use    = L7_TRUE;                                              /* update it */
  pclient->inn_vlan  = evcBridge->inn_vlan;
  pclient->out_vlan  = evcBridge->intf.vid;
  pclient->bwprofile[PTIN_EVC_INTF_ROOT] = L7_NULLPTR;
  pclient->bwprofile[PTIN_EVC_INTF_LEAF] = L7_NULLPTR;
  pclient->counter  [PTIN_EVC_INTF_ROOT] = L7_NULLPTR;
  pclient->counter  [PTIN_EVC_INTF_LEAF] = L7_NULLPTR;
  dl_queue_add_tail(&evcs[evc_idx].intf[leaf_intf].clients, (dl_queue_elem_t*) pclient); /* add it to the corresponding interface */
  evcs[evc_idx].n_clients++;

  /* Update client direct referencing */
  //if (evcBridge->inn_vlan<4096)
  //  evcs[evc_idx].client_ref[evcBridge->inn_vlan] = pclient;
  
  LOG_INFO(LOG_CTX_PTIN_EVC, "EVC# %u: bridge successfully added", evc_idx);

  return L7_SUCCESS;
}

/**
 * Removes a bridge from a stacked EVC between the root and a particular interface 
 *  
 * NOTE: if there is no bridge, L7_SUCCESS is returned anyway
 * 
 * @param evcBridge Bridge info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_stacked_bridge_remove(ptin_HwEthEvcBridge_t *evcBridge)
{
  L7_uint evc_idx;
  L7_uint root_intf;
  L7_uint leaf_intf;
  L7_uint i;
  L7_RC_t rc = L7_SUCCESS;
  struct ptin_evc_client_s *pclient;

  evc_idx = evcBridge->index;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Removing EVC# %u bridge connection...", evc_idx);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_idx >= PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u is out of range [0..%u]", evc_idx, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (!evcs[evc_idx].in_use)
  {
    LOG_WARNING(LOG_CTX_PTIN_EVC, "EVC# %u is not in use", evc_idx);
    return L7_NOT_EXIST;
  }

  /* Check if the EVC is stacked */
  if (IS_EVC_UNSTACKED(evc_idx))
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u is not stacked!!!", evc_idx);
    return L7_FAILURE;
  }

  /* Determine leaf ptin_intf */
  if (evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL)
    leaf_intf = evcBridge->intf.intf_id;
  else
    leaf_intf = evcBridge->intf.intf_id + PTIN_SYSTEM_N_PORTS;

  /* Validate leaf interface (from received message) */
  if ( leaf_intf >= PTIN_SYSTEM_N_INTERF )
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: %s# %u is invalid", evc_idx,
            evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id);
    return L7_FAILURE;
  }
  if ( !evcs[evc_idx].intf[leaf_intf].in_use )
  {
    LOG_WARNING(LOG_CTX_PTIN_EVC, "EVC# %u: %s# %u is not active in this EVC", evc_idx,
            evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id);
    return L7_NOT_EXIST;
  }
  if ( evcs[evc_idx].intf[leaf_intf].type != PTIN_EVC_INTF_LEAF )
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: %s# %u is not a leaf interface", evc_idx,
            evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id);
    return L7_FAILURE;
  }

  /* Get root port */
  root_intf = PTIN_SYSTEM_N_INTERF;
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
  {
    if (evcs[evc_idx].intf[i].in_use &&
        evcs[evc_idx].intf[i].type == PTIN_EVC_INTF_ROOT)
    {
      root_intf = i;
      break;
    }
  }

  /* Get client entry */
  ptin_evc_find_client(evcBridge->inn_vlan, &evcs[evc_idx].intf[leaf_intf].clients, (dl_queue_elem_t**) &pclient);
  if (pclient == NULL)
  {
    LOG_WARNING(LOG_CTX_PTIN_EVC, "EVC# %u: %s# %u is does not have any bridge with Inner VID = %u", evc_idx,
                evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id, evcBridge->intf.vid);
    return L7_NOT_EXIST;
  }

  LOG_INFO(LOG_CTX_PTIN_EVC, "EVC# %u: deleting bridge [Root: Intf=%u IntVID=%u] <=> [Leaf: Intf=%u NEW Out.VID=%u Inn.VID=%u] ...", evc_idx,
           root_intf, evcs[evc_idx].rvlan, leaf_intf, pclient->out_vlan, pclient->inn_vlan);

  /* Check if there are counters attached */
  #ifdef EVC_COUNTERS_REQUIRE_CLEANUP_BEFORE_REMOVAL
  if ( pclient->counter[PTIN_EVC_INTF_ROOT]!=NULL || pclient->counter[PTIN_EVC_INTF_LEAF]!=NULL )
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u have counters attached... please, remove them first!", evc_idx);
    return L7_FAILURE;
  }
  #endif
  /* Check if there are bw profiles attached */
  #ifdef EVC_BWPROFILES_REQUIRE_CLEANUP_BEFORE_REMOVAL
  if ( pclient->bwprofile[PTIN_EVC_INTF_ROOT]!=NULL || pclient->bwprofile[PTIN_EVC_INTF_LEAF]!=NULL )
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u have profiles attached... please, remove them first!", evc_idx);
    return L7_FAILURE;
  }
  #endif

  /* Remove profiles and counters to this client */
  if ( ptin_evc_pclient_clean(evc_idx,pclient, L7_TRUE) != L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: can't remove profiles and counters to client!", evc_idx);
    return L7_FAILURE;
  }

  /* Delete bridge between root and leaf port (Proot, Vr, Pleaf, Vs', Vc) */
  rc = switching_stacked_bridge_remove(root_intf, evcs[evc_idx].rvlan, leaf_intf, pclient->out_vlan, pclient->inn_vlan);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error deleting bridge", evc_idx,
            evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id);
    return L7_FAILURE;
  }

  /* Delete client from the EVC struct */
  dl_queue_remove(&evcs[evc_idx].intf[leaf_intf].clients, (dl_queue_elem_t*) pclient);
  pclient->in_use   = L7_FALSE;
  pclient->inn_vlan = 0;
  pclient->out_vlan = 0;
  dl_queue_add_tail(&queue_free_clients, (dl_queue_elem_t*) pclient);
  evcs[evc_idx].n_clients--;

  /* Update client direct referencing */
  //if (evcBridge->inn_vlan<4096)
  //  evcs[evc_idx].client_ref[evcBridge->inn_vlan] = L7_NULLPTR;

  LOG_INFO(LOG_CTX_PTIN_EVC, "EVC# %u: bridge successfully removed", evc_idx);

  return L7_SUCCESS;
}

/**
 * Bandwidth Policers management functions
 */

/**
 * Read data of a bandwidth profile
 * 
 * @param evc_idx : EVC index
 * @param profile : bw profile (input and output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
L7_RC_t ptin_evc_bwProfile_get(L7_uint evc_idx, ptin_bw_profile_t *profile)
{
  ptin_bw_policy_t **policy_loc=L7_NULLPTR;
  L7_RC_t rc = L7_SUCCESS;

  /* Verify and update profile data */
  if (ptin_evc_bwProfile_verify(evc_idx,profile,(void ***) &policy_loc)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Profile data have conflicts");
    return L7_FAILURE;
  }
  /* Validate policer location */
  if (policy_loc==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"We have no location to store policer address");
    return L7_FAILURE;
  }

  /* Read policer information */
  if ((rc = ptin_bwPolicer_get(profile,*policy_loc))!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Error reading policer");
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Apply a bandwidth profile to an EVC and (optionally) to a 
 * specific client 
 * 
 * @param evc_idx : EVC index
 * @param profile : bw profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
L7_RC_t ptin_evc_bwProfile_set(L7_uint evc_idx, ptin_bw_profile_t *profile)
{
  ptin_bw_policy_t **policy_loc=L7_NULLPTR;
  ptin_bw_policy_t *policy_old, *policy_new;
  L7_RC_t rc = L7_SUCCESS;

  /* Verify and update profile data */
  if (ptin_evc_bwProfile_verify(evc_idx,profile,(void ***) &policy_loc)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Profile data have conflicts");
    return L7_FAILURE;
  }

  /* Validate policer location */
  if (policy_loc==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"We have no location to store policer address");
    return L7_FAILURE;
  }
  else if (*policy_loc==L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Policer address is null");
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Policer address has a valid value");
  }

  /* Save old policer pointer */
  policy_old = *policy_loc;

  /* Apply policer */
  if ((rc = ptin_bwPolicer_set(profile,policy_loc))!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Error applying policer");
  }

  /* New policy pointer */
  policy_new = *policy_loc;

  /* One more profile */
  if (policy_old==L7_NULLPTR && policy_new!=L7_NULLPTR)
  {
    evcs[evc_idx].n_bwprofiles++;
  }
  /* One less profile */
  else if (policy_old!=L7_NULLPTR && policy_new==L7_NULLPTR)
  {
    if (evcs[evc_idx].n_bwprofiles>0)  evcs[evc_idx].n_bwprofiles--;
  }

  if (*policy_loc==L7_NULLPTR)
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Returned policer pointer is null");
  else
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Returned policer pointer is valid");

  return rc;
}

/**
 * Remove a bandwidth profile to an EVC 
 * 
 * @param evc_idx : EVC index
 * @param profile : bw profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
L7_RC_t ptin_evc_bwProfile_delete(L7_uint evc_idx, ptin_bw_profile_t *profile)
{
  ptin_bw_policy_t **policy_loc=L7_NULLPTR;
  L7_RC_t rc = L7_SUCCESS;

  /* Verify and update profile data */
  rc = ptin_evc_bwProfile_verify(evc_idx,profile,(void ***) &policy_loc);
  if ( rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Profile data have conflicts");
    return rc;
  }
  /* Validate policer location */
  if (policy_loc==L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_EVC,"Policer does not exist... success");
    return L7_NOT_EXIST;
  }

  /* Apply policer */
  rc = ptin_bwPolicer_delete(*policy_loc);
  if ( rc != L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Error removing policer");
    return rc;
  }

  /* Clear policer address */
  *policy_loc = L7_NULLPTR;

  /* One less profile */
  if (evcs[evc_idx].n_bwprofiles>0)  evcs[evc_idx].n_bwprofiles--;

  return L7_SUCCESS;
}


/**
 * EVC Counters management functions
 */

/**
 * Read statistics of a particular EVC
 * 
 * @param evc_idx : EVC index
 * @param profile : evcStats profile 
 * @param stats : Statistics data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
L7_RC_t ptin_evc_evcStats_get(L7_uint evc_idx, ptin_evcStats_profile_t *profile, ptin_evcStats_counters_t *stats)
{
  ptin_evcStats_policy_t **policy_loc=L7_NULLPTR;
  L7_RC_t rc = L7_SUCCESS;

  /* Verify and update profile data */
  if (ptin_evc_evcStats_verify(evc_idx,profile,(void ***) &policy_loc)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Profile data have conflicts");
    return L7_FAILURE;
  }
  /* Validate policy location */
  if (policy_loc==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"We have no location to store evcStats address");
    return L7_FAILURE;
  }

  /* Read policy information */
  if ((rc = ptin_evcStats_get(stats,*policy_loc))!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_EVC,"Error reading policer");
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Add a rule to make packets counting of a specific EVC
 * 
 * @param evc_idx : EVC index
 * @param profile : evcStats profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
L7_RC_t ptin_evc_evcStats_set(L7_uint evc_idx, ptin_evcStats_profile_t *profile)
{
  ptin_evcStats_policy_t **policy_loc=L7_NULLPTR;
  ptin_evcStats_policy_t *policy_old, *policy_new;
  L7_RC_t rc = L7_SUCCESS;

  /* Verify and update profile data */
  if (ptin_evc_evcStats_verify(evc_idx,profile,(void ***) &policy_loc)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Profile data have conflicts");
    return L7_FAILURE;
  }

  /* Validate policy location */
  if (policy_loc==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"We have no location to store evcStats address");
    return L7_FAILURE;
  }
  else if (*policy_loc==L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_EVC,"evcStats address is null");
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Policer address has a valid value");
  }

  /* Save old policy pointer */
  policy_old = *policy_loc;

  /* Apply policy */
  if ((rc = ptin_evcStats_set(profile,policy_loc))!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Error adding evcStats");
  }

  /* New policy pointer */
  policy_new = *policy_loc;

  /* One more counter */
  if (policy_old==L7_NULLPTR && policy_new!=L7_NULLPTR)
  {
    evcs[evc_idx].n_counters++;
  }
  /* One less counter */
  else if (policy_old!=L7_NULLPTR && policy_new==L7_NULLPTR)
  {
    if (evcs[evc_idx].n_counters>0)  evcs[evc_idx].n_counters--;
  }

  if (*policy_loc==L7_NULLPTR)
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Returned evcStats pointer is null");
  else
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Returned evcStats pointer is valid");

  return rc;
}

/**
 * Delete a rule to make packets counting of a specific EVC
 * 
 * @param evc_idx : EVC index
 * @param profile : evcStats profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
L7_RC_t ptin_evc_evcStats_delete(L7_uint evc_idx, ptin_evcStats_profile_t *profile)
{
  ptin_evcStats_policy_t **policy_loc=L7_NULLPTR;
  L7_RC_t rc = L7_SUCCESS;

  /* Verify and update profile data */
  rc = ptin_evc_evcStats_verify(evc_idx,profile,(void ***) &policy_loc);
  if ( rc != L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Profile data have conflicts");
    return rc;
  }

  /* Validate policy location */
  if (policy_loc==L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_EVC,"evcStats does not exist... success");
    return L7_NOT_EXIST;
  }

  /* Apply policy */
  rc = ptin_evcStats_delete(*policy_loc);
  if ( rc != L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Error removing evcStats");
    return rc;
  }

  /* Clear policer address */
  *policy_loc = L7_NULLPTR;

  /* One less counter */
  if (evcs[evc_idx].n_counters>0)  evcs[evc_idx].n_counters--;

  return L7_SUCCESS;
}

/****************************************************************************** 
 * FUNCTIONS AVAILABLE FOR DRIVSHELL
 ******************************************************************************/

static L7_BOOL ptin_clean_counters = L7_TRUE;
static L7_BOOL ptin_clean_profiles = L7_FALSE;

/**
 * Allow counters to be removed during cleanup
 * 
 * @param enable : 1 or 0
 */
void ptin_evc_clean_counters_enable( L7_BOOL enable )
{
  ptin_clean_counters = enable;
}

/**
 * Allow profiles to be removed during cleanup
 * 
 * @param enable : 1 or 0
 */
void ptin_evc_clean_profiles_enable( L7_BOOL enable )
{
  ptin_clean_profiles = enable;
}

/**
 * Remove all clients for a specific evc
 * 
 * @param evc_idx : evc index 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_allclients_remove( L7_uint evc_idx )
{
  L7_uint     intf_idx;
  ptin_intf_t ptin_intf;
  L7_RC_t     rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_idx].in_use)
    return L7_SUCCESS;

  /* Only stacked services have clients */
  if (!IS_EVC_STACKED(evc_idx))
    return L7_SUCCESS;

  /* Run all interfaces */
  for (intf_idx=0; intf_idx<PTIN_SYSTEM_N_INTERF; intf_idx++)
  {
    if (!evcs[evc_idx].intf[intf_idx].in_use)
      continue;

    if (ptin_intf_port2ptintf(intf_idx,&ptin_intf)!=L7_SUCCESS)
      continue;

    /* Remove all clients of this interface */
    if (ptin_evc_intfclients_remove(evc_idx, ptin_intf.intf_type, ptin_intf.intf_id)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"EVC #%u: Error removing all clients from intf=%u/%u",evc_idx,ptin_intf.intf_type,ptin_intf.intf_id);
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**
 * Remove all clients for a specific evc and interface
 * 
 * @param evc_idx : evc index
 * @param intf_idx : interface index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intfclients_remove( L7_uint evc_idx, L7_uint8 intf_type, L7_uint8 intf_id )
{
  L7_uint     intf_idx;
  ptin_intf_t ptin_intf;
  struct  ptin_evc_client_s *pclient;
  ptin_HwEthEvcBridge_t     bridge;
  L7_RC_t                   res;
  L7_RC_t                   rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get intf_idx, and validate it */
  ptin_intf.intf_type = intf_type;
  ptin_intf.intf_id   = intf_id;
  if (ptin_intf_ptintf2port(&ptin_intf, &intf_idx)!=L7_SUCCESS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Interface %u/%u not valid!",intf_type,intf_id);
    return L7_FAILURE;
  }

  if (!evcs[evc_idx].in_use || !evcs[evc_idx].intf[intf_idx].in_use)
    return L7_SUCCESS;

  /* Only stacked services have clients */
  if (!IS_EVC_STACKED(evc_idx))
    return L7_SUCCESS;

  bridge.index          = evc_idx;
  bridge.intf.intf_type = intf_idx < PTIN_SYSTEM_N_PORTS ? PTIN_EVC_INTF_PHYSICAL : PTIN_EVC_INTF_LOGICAL;
  bridge.intf.intf_id   = intf_idx < PTIN_SYSTEM_N_PORTS ? intf_idx : intf_idx - PTIN_SYSTEM_N_PORTS;
  bridge.intf.mef_type  = evcs[evc_idx].intf[intf_idx].type;

  /* Get all clients */
  while (dl_queue_get_head(&evcs[evc_idx].intf[intf_idx].clients, (dl_queue_elem_t **) &pclient) == NOERR)
  {
    bridge.intf.vid = pclient->out_vlan;
    bridge.inn_vlan = pclient->inn_vlan;

    /* Clean client */
    res = ptin_evc_pclient_clean(evc_idx,pclient, L7_TRUE);
    if ( res != L7_SUCCESS )
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"EVC #%u: Error cleaning client intf=%u/%u,cvlan=%",evc_idx,bridge.intf.intf_type,bridge.intf.intf_id,bridge.inn_vlan);
      rc = L7_FAILURE;
    }

    /* Remove this bridge (client) */
    if (ptin_evc_stacked_bridge_remove(&bridge)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"EVC #%u: Error removing bridge for client of cvlan=%u attached to intf=%u/%u",
              evc_idx, bridge.inn_vlan, bridge.intf.intf_type, bridge.intf.intf_id);
      rc = L7_FAILURE;
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_EVC,"EVC #%u: Client of cvlan %u attached to intf=%u/%u removed",
                evc_idx, bridge.inn_vlan, bridge.intf.intf_type, bridge.intf.intf_id);
    }
  }

  return rc;
}

/**
 * Remove client
 * 
 * @param evc_idx 
 * @param intf_type 
 * @param intf_id 
 * @param cvlan 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_client_remove( L7_uint evc_idx, L7_uint8 intf_type, L7_uint8 intf_id, L7_uint cvlan )
{
  L7_uint     intf_idx;
  ptin_intf_t ptin_intf;
  ptin_HwEthEvcBridge_t bridge;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get intf_idx, and validate it */
  ptin_intf.intf_type = intf_type;
  ptin_intf.intf_id   = intf_id;
  if (ptin_intf_ptintf2port(&ptin_intf, &intf_idx)!=L7_SUCCESS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Interface %u/%u not valid!",intf_type,intf_id);
    return L7_FAILURE;
  }

  if (!evcs[evc_idx].in_use || !evcs[evc_idx].intf[intf_idx].in_use)
    return L7_SUCCESS;

  /* Only stacked services have clients */
  if (!IS_EVC_STACKED(evc_idx))
  {
    LOG_TRACE(LOG_CTX_PTIN_EVC,"This is an unstacked EVC... nothing to do!");
    return L7_SUCCESS;
  }

  /* Clean client */
  if ( ptin_evc_client_clean(evc_idx, ptin_intf.intf_type, ptin_intf.intf_id, cvlan, L7_TRUE) != L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"EVC #%u: Error cleaning client of cvlan=%u attached to intf=%u/%u!",evc_idx, cvlan, ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }

  /* Build struct to remove bridge */
  bridge.index          = evc_idx;
  bridge.intf.intf_type = ptin_intf.intf_type;
  bridge.intf.intf_id   = ptin_intf.intf_id;
  bridge.intf.mef_type  = evcs[evc_idx].intf[intf_idx].type;
  bridge.intf.vid       = 0;
  bridge.inn_vlan       = cvlan;

  /* Remove this bridge (client) */
  if (ptin_evc_stacked_bridge_remove(&bridge)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"EVC #%u: Error removing bridge for client of cvlan=%u attached to intf=%u/%u",
            evc_idx, bridge.inn_vlan, bridge.intf.intf_type, bridge.intf.intf_id);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_EVC,"EVC #%u: Client of cvlan %u attached to intf=%u/%u removed",
            evc_idx, bridge.inn_vlan, bridge.intf.intf_type, bridge.intf.intf_id);

  return L7_SUCCESS;
}

/**
 * This function will clean all counters and profiles associated 
 * to all interfaces and clients of an EVC
 *  
 * @param evc_idx : EVC index 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_clean_all( L7_uint evc_idx, L7_BOOL force )
{
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_idx].in_use)
    return L7_SUCCESS;

  /* Clean all interfaces profiles and counters */
  if (ptin_evc_allintfs_clean(evc_idx, force)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"EVC #%u: Error cleaning interfaces",evc_idx);
    rc = L7_FAILURE;
  }

  /* Clean all clients profiles and counters */
  if (ptin_evc_allclients_clean(evc_idx, force)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"EVC #%u: Error cleaning all clients",evc_idx);
    rc = L7_FAILURE;
  }

  return rc;
}

/**
 * This function will clean all counters and profiles associated 
 * to one interface of an EVC, as well to its clients
 *  
 * @param evc_idx : EVC index 
 * @param intf_type 
 * @param intf_id 
 *  
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_intf_clean_all( L7_uint evc_idx, L7_uint8 intf_type, L7_uint8 intf_id, L7_BOOL force )
{
  L7_RC_t     rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_idx].in_use)
    return L7_SUCCESS;

  /* Clean all interfaces profiles and counters */
  if (ptin_evc_intf_clean(evc_idx,intf_type,intf_id, force)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"EVC #%u: Error cleaning interface %u/%u",evc_idx,intf_type,intf_id);
    rc = L7_FAILURE;
  }

  /* Clean all clients profiles and counters */
  if (ptin_evc_intfclients_clean(evc_idx,intf_type,intf_id, force)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"EVC #%u: Error cleaning all clients of intf=%u/%u",evc_idx,intf_type,intf_id);
    rc = L7_FAILURE;
  }

  return rc;
}

/**
 * This function will clean all counters and profiles associated 
 * to a specific evc 
 *  
 * @param evc_idx : EVC index 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_allintfs_clean( L7_uint evc_idx, L7_BOOL force )
{
  L7_uint     intf_idx;
  ptin_intf_t ptin_intf;
  L7_RC_t     rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_idx].in_use)
    return L7_SUCCESS;

  /* Run all interfaces */
  for (intf_idx=0; intf_idx<PTIN_SYSTEM_N_INTERF; intf_idx++)
  {
    if (!evcs[evc_idx].intf[intf_idx].in_use)
      continue;

    if (ptin_intf_port2ptintf(intf_idx, &ptin_intf)!=L7_SUCCESS)
      continue;

    /* Clean interface */
    if (ptin_evc_intf_clean(evc_idx, ptin_intf.intf_type, ptin_intf.intf_id, force)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"EVC #%u: Error cleaning profiles and counters of intf=%u/%u",evc_idx,ptin_intf.intf_type,ptin_intf.intf_id);
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**
 * Clean all clients for a specific evc
 * 
 * @param evc_idx : evc index 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_allclients_clean( L7_uint evc_idx, L7_BOOL force )
{
  L7_uint     intf_idx;
  ptin_intf_t ptin_intf;
  L7_RC_t     rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_idx].in_use)
    return L7_SUCCESS;

  /* Only stacked services have clients */
  if (!IS_EVC_STACKED(evc_idx))
    return L7_SUCCESS;

  /* Run all interfaces */
  for (intf_idx=0; intf_idx<PTIN_SYSTEM_N_INTERF; intf_idx++)
  {
    if (!evcs[evc_idx].intf[intf_idx].in_use)
      continue;

    if (ptin_intf_port2ptintf(intf_idx,&ptin_intf)!=L7_SUCCESS)
      continue;

    /* Clean all clients of this interface */
    if (ptin_evc_intfclients_clean(evc_idx, ptin_intf.intf_type, ptin_intf.intf_id, force)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"EVC #%u: Error cleaning all clients from intf=%u/%u",evc_idx,ptin_intf.intf_type,ptin_intf.intf_id);
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**
 * Clean all clients for a specific evc and interface
 * 
 * @param evc_idx : evc index
 * @param intf_idx : interface index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intfclients_clean( L7_uint evc_idx, L7_uint8 intf_type, L7_uint8 intf_id, L7_BOOL force )
{
  L7_uint     intf_idx;
  ptin_intf_t ptin_intf;
  struct  ptin_evc_client_s *pclient;
  L7_RC_t     rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get intf_idx, and validate it */
  ptin_intf.intf_type = intf_type;
  ptin_intf.intf_id   = intf_id;
  if (ptin_intf_ptintf2port(&ptin_intf, &intf_idx)!=L7_SUCCESS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Interface %u/%u not valid!",intf_type,intf_id);
    return L7_FAILURE;
  }

  if (!evcs[evc_idx].in_use || !evcs[evc_idx].intf[intf_idx].in_use)
    return L7_SUCCESS;

  /* Only stacked services have clients */
  if (!IS_EVC_STACKED(evc_idx))
    return L7_SUCCESS;

  /* Get all clients */
  if (dl_queue_get_head(&evcs[evc_idx].intf[intf_idx].clients, (dl_queue_elem_t **) &pclient)==NOERR)
  {
    while ( pclient != L7_NULLPTR )
    {
      /* Clean client */
      if (ptin_evc_pclient_clean(evc_idx,pclient, force)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC,"EVC #%u: Error cleaning client intf=%u/%u,cvlan=%u",
                evc_idx, ptin_intf.intf_type, ptin_intf.intf_id, pclient->inn_vlan);
        rc = L7_FAILURE;
      }

      /* Get next client */
      pclient = (struct ptin_evc_client_s *) dl_queue_get_next(&evcs[evc_idx].intf[intf_idx].clients, (dl_queue_elem_t *) pclient);
    }
  }

  return rc;
}

/**
 * This function will clean all counters and profiles associated 
 * to a specific evc and interface
 *  
 * @param evc_idx : EVC index 
 * @param intf_idx : intferface index  
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_intf_clean( L7_uint evc_idx, L7_uint8 intf_type, L7_uint8 intf_id, L7_BOOL force )
{
  L7_uint     intf_idx;
  ptin_intf_t ptin_intf;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get intf_idx, and validate it */
  ptin_intf.intf_type = intf_type;
  ptin_intf.intf_id   = intf_id;
  if (ptin_intf_ptintf2port(&ptin_intf, &intf_idx)!=L7_SUCCESS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Interface %u/%u not valid!",intf_type,intf_id);
    return L7_FAILURE;
  }

  if (!evcs[evc_idx].in_use || !evcs[evc_idx].intf[intf_idx].in_use)
    return L7_SUCCESS;

  /* Remove counter on this interface */
  if (force /*ptin_clean_force*/ || ptin_clean_counters)
  {
    if (evcs[evc_idx].intf[intf_idx].counter != L7_NULL)
    {
      ptin_evcStats_delete(evcs[evc_idx].intf[intf_idx].counter);
      evcs[evc_idx].intf[intf_idx].counter = L7_NULL;
      if (evcs[evc_idx].n_counters>0)  evcs[evc_idx].n_counters--;
      LOG_TRACE(LOG_CTX_PTIN_EVC,"EVC #%u: Counter removed from intf=%u/%u",evc_idx,ptin_intf.intf_type,ptin_intf.intf_id);
    }
  }

  /* Remove bw profile on this interface */
  if (force /*ptin_clean_force*/ || ptin_clean_profiles)
  {
    if (evcs[evc_idx].intf[intf_idx].bwprofile != L7_NULL)
    {
      ptin_bwPolicer_delete(evcs[evc_idx].intf[intf_idx].bwprofile);
      evcs[evc_idx].intf[intf_idx].bwprofile = L7_NULL;
      if (evcs[evc_idx].n_bwprofiles>0)  evcs[evc_idx].n_bwprofiles--;
      LOG_TRACE(LOG_CTX_PTIN_EVC,"EVC #%u: Profile removed from intf=%u/%u",evc_idx,ptin_intf.intf_type,ptin_intf.intf_id);
    }
  }

  return L7_SUCCESS;
}

/**
 * Clean client profiles and counters
 * 
 * @param evc_idx 
 * @param intf_type 
 * @param intf_id 
 * @param cvlan 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_client_clean( L7_uint evc_idx, L7_uint8 intf_type, L7_uint8 intf_id, L7_uint cvlan, L7_BOOL force )
{
  L7_uint     intf_idx;
  ptin_intf_t ptin_intf;
  struct ptin_evc_client_s *pclient;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get intf_idx, and validate it */
  ptin_intf.intf_type = intf_type;
  ptin_intf.intf_id   = intf_id;
  if (ptin_intf_ptintf2port(&ptin_intf, &intf_idx)!=L7_SUCCESS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Interface %u/%u not valid!",intf_type,intf_id);
    return L7_FAILURE;
  }

  if (!evcs[evc_idx].in_use || !evcs[evc_idx].intf[intf_idx].in_use)
    return L7_SUCCESS;

  /* Only stacked services have clients */
  if (!IS_EVC_STACKED(evc_idx))
  {
    LOG_TRACE(LOG_CTX_PTIN_EVC,"This is an unstacked EVC... nothing to do!");
    return L7_SUCCESS;
  }

  /* Find provided client */
  ptin_evc_find_client(cvlan, &(evcs[evc_idx].intf[intf_idx].clients), (dl_queue_elem_t **) &pclient);

  if (pclient==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"EVC #%u: Client of cvlan=%u attached to intf=%u/%u not found!",evc_idx, cvlan, ptin_intf.intf_type,ptin_intf.intf_id);
    return L7_FAILURE;
  }

  /* Clean client */
  if (ptin_evc_pclient_clean(evc_idx, pclient, force)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"EVC #%u: Error cleaning client of cvlan=%u attached to intf=%u/%u!",evc_idx, cvlan, ptin_intf.intf_type,ptin_intf.intf_id);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/****************************************************************************** 
 * STATIC FUNCTIONS
 ******************************************************************************/

/**
 * This function will clean all counters and profiles associated 
 * to the provided client 
 *  
 * @param evc_idx : EVC index 
 * @param pclient : client pointer
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_evc_pclient_clean( L7_uint evc_idx, struct ptin_evc_client_s *pclient, L7_BOOL force )
{
  L7_uint i;

  /* Validate arguments */
  if (evc_idx>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid arguments");
    return L7_FAILURE;
  }
  if (!evcs[evc_idx].in_use || pclient==L7_NULLPTR)
    return L7_SUCCESS;

  /* Only stacked services have clients */
  if (!IS_EVC_STACKED(evc_idx))
    return L7_SUCCESS;

  /* Remove counters */
  if (force /*ptin_clean_force*/ || ptin_clean_counters)
  {
    for (i=0; i<(sizeof(pclient->counter)/sizeof(pclient->counter[0])); i++)
    {
      if (pclient->counter[i] != L7_NULL)
      {
        ptin_evcStats_delete(pclient->counter[i]);
        pclient->counter[i] = L7_NULL;
        if (evcs[evc_idx].n_counters>0)  evcs[evc_idx].n_counters--;
        LOG_TRACE(LOG_CTX_PTIN_EVC,"EVC #%u: Counter removed from client of cvlan=%u (outerVlan=%u)",evc_idx,pclient->inn_vlan,pclient->out_vlan);
      }
    }
  }

  /* Remove BW Profiles */
  if (force /*ptin_clean_force*/ || ptin_clean_profiles)
  {
    for (i=0; i<(sizeof(pclient->bwprofile)/sizeof(pclient->bwprofile[0])); i++)
    {
      if (pclient->bwprofile[i] != L7_NULL)
      {
        ptin_bwPolicer_delete(pclient->bwprofile[i]);
        pclient->bwprofile[i] = L7_NULL;
        if (evcs[evc_idx].n_bwprofiles>0)  evcs[evc_idx].n_bwprofiles--;
        LOG_TRACE(LOG_CTX_PTIN_EVC,"EVC #%u: Profile removed from client of cvlan=%u (outerVlan=%u)",evc_idx,pclient->inn_vlan,pclient->out_vlan);
      }
    }
  }

  return L7_SUCCESS;
}

/**
 * Reset an EVC entry to the default parameters (empty)
 * 
 * @param evc_idx 
 */
static void ptin_evc_entry_reset(L7_uint evc_idx)
{
  L7_uint i;

  /* Release free vlan queue */
  ptin_evc_freeVlanQueue_free(evcs[evc_idx].queue_free_vlans);

  /* Reset EVC entry (set all memory to 0x00) */
  memset(&evcs[evc_idx], 0x00, sizeof(evcs[evc_idx]));

  /* Initialize free clients queue (per interface) */
  for (i=0; i<(sizeof(evcs[0].intf)/sizeof(evcs[0].intf[0])); i++)
    dl_queue_init(&evcs[evc_idx].intf[i].clients);
}


/**
 * Configure an interface of a particular EVC
 * (and updates EVCs struct with the configuration)
 * 
 * @param evc_idx   EVC #
 * @param ptin_intf Interface #
 * @param intf_cfg  Pointer to the interface config data
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_evc_intf_add(L7_uint evc_idx, L7_uint ptin_intf, ptin_HwEthMef10Intf_t *intf_cfg)
{
  L7_BOOL is_stacked;
  L7_BOOL is_root;
  L7_BOOL mac_learning;
  L7_BOOL cpu_trap;
  L7_uint16 int_vlan;
  L7_uint16 root_vlan;
  ptin_intf_t intf;
  L7_RC_t rc = L7_SUCCESS;

  is_stacked   = (evcs[evc_idx].flags & PTIN_EVC_MASK_STACKED) == PTIN_EVC_MASK_STACKED;
  is_root      = intf_cfg->mef_type == PTIN_EVC_INTF_ROOT;
  mac_learning = (evcs[evc_idx].flags & PTIN_EVC_MASK_MACLEARNING) == PTIN_EVC_MASK_MACLEARNING;
  cpu_trap     = (evcs[evc_idx].flags & PTIN_EVC_MASK_CPU_TRAPPING) == PTIN_EVC_MASK_CPU_TRAPPING;
  root_vlan    = evcs[evc_idx].rvlan;

  LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: adding %s# %02u (MEF %s) ...",
            evc_idx,
            ptin_intf < PTIN_SYSTEM_N_PORTS ? "PHY":"LAG",
            ptin_intf < PTIN_SYSTEM_N_PORTS ? ptin_intf : ptin_intf - PTIN_SYSTEM_N_PORTS,
            is_root ? "Root":"Leaf");

  if (is_root)
  {
    int_vlan = root_vlan;   /* Vroot is the same for all the root interfaces */

    rc = switching_root_add(ptin_intf, intf_cfg->vid, int_vlan);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error adding root interface [ptin_intf=%u Vs=%u Vr=%u]",
              evc_idx, ptin_intf, intf_cfg->vid, int_vlan);
      return L7_FAILURE;
    }
    evcs[evc_idx].n_roots++;

    rc = switching_vlan_config(int_vlan, root_vlan, mac_learning, evcs[evc_idx].mc_flood, cpu_trap );
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error configuring VLAN %u [FwdVlan=%u MACLearning=%u MCFlood=%u]",
              evc_idx, int_vlan, root_vlan, mac_learning, evcs[evc_idx].mc_flood);
      return L7_FAILURE;
    }
  }
  else
  {
    if (is_stacked)
      int_vlan = evcs[evc_idx].rvlan;     /* Internal VLAN is the same for all interfaces, including leafs */
    else
      ptin_evc_vlan_allocate(&int_vlan, evcs[evc_idx].queue_free_vlans, evc_idx); /* One VLAN for each unstacked leaf */

    if (switching_leaf_add(ptin_intf, int_vlan) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error adding leaf [ptin_intf=%u Vl=%u]",
              evc_idx, ptin_intf, int_vlan);

      ptin_evc_vlan_free(int_vlan, evcs[evc_idx].queue_free_vlans);       /* free VLAN */
      return L7_FAILURE;
    }
    evcs[evc_idx].n_leafs++;

    rc = switching_vlan_config(int_vlan, root_vlan, mac_learning, evcs[evc_idx].mc_flood, cpu_trap);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error configuring VLAN %u [FwdVlan=%u MACLearning=%u MCFlood=%u]",
              evc_idx, int_vlan, root_vlan, mac_learning, evcs[evc_idx].mc_flood);
      return L7_FAILURE;
    }

    if (is_stacked)
    {
      /* Add translations for leaf ports, only if we are in matrix board */
      #if ( PTIN_BOARD_IS_MATRIX )
      rc = switching_stacked_leaf_add(ptin_intf, intf_cfg->vid, 0, int_vlan);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC, "Error adding translations for leaf interface %u (rc=%d)",ptin_intf, rc);
        return L7_FAILURE;
      }
      #endif
    }
  }

  evcs_intfs_in_use[ptin_intf]++; /* Add this interface to the list of members in use */

  /* Update intf struct (common info) */
  evcs[evc_idx].intf[ptin_intf].in_use   = L7_TRUE;
  evcs[evc_idx].intf[ptin_intf].type     = intf_cfg->mef_type;
  evcs[evc_idx].intf[ptin_intf].int_vlan = int_vlan;
  #if ( !PTIN_BOARD_IS_MATRIX )
  if (is_stacked && (intf_cfg->mef_type == PTIN_EVC_INTF_LEAF))
  {
    evcs[evc_idx].intf[ptin_intf].out_vlan = 0xFFFF;  /* on stacked EVCs, leafs out.vid is defined per client and not per interface */
  }
  else
  #endif
  {
    evcs[evc_idx].intf[ptin_intf].out_vlan = intf_cfg->vid;
  }

  evcs[evc_idx].intf[ptin_intf].counter   = L7_NULLPTR;
  evcs[evc_idx].intf[ptin_intf].bwprofile = L7_NULLPTR;
  evcs[evc_idx].intf[ptin_intf].clients.head    = L7_NULLPTR;
  evcs[evc_idx].intf[ptin_intf].clients.tail    = L7_NULLPTR;
  evcs[evc_idx].intf[ptin_intf].clients.n_elems = 0;

  /* Update snooping configuration */
  if (ptin_intf_port2ptintf(ptin_intf,&intf)==L7_SUCCESS)
  {
    if (ptin_igmp_is_evc_used(evc_idx))
    {
      ptin_igmp_snooping_trap_interface_update(evc_idx,&intf,L7_TRUE);
      LOG_TRACE(LOG_CTX_PTIN_EVC,"IGMP packet trapping updated for interface %u/%u",intf.intf_type,intf.intf_id);
    }
    if (ptin_dhcp_is_evc_used(evc_idx))
    {
      ptin_dhcp_snooping_trap_interface_update(evc_idx,&intf,L7_TRUE);
      LOG_TRACE(LOG_CTX_PTIN_EVC,"DHCP packet trapping updated for interface %u/%u",intf.intf_type,intf.intf_id);
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: %s# %02u (MEF %s Out.VID=%04hu Int.VID=%04hu) successfully added",
            evc_idx,
            ptin_intf < PTIN_SYSTEM_N_PORTS ? "PHY":"LAG",
            ptin_intf < PTIN_SYSTEM_N_PORTS ? ptin_intf : ptin_intf - PTIN_SYSTEM_N_PORTS,
            evcs[evc_idx].intf[ptin_intf].type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf",
            evcs[evc_idx].intf[ptin_intf].out_vlan,
            evcs[evc_idx].intf[ptin_intf].int_vlan);

  return L7_SUCCESS;
}

/**
 * Removes an interface configuration (translation entries)
 * NOTE: the intf entry is removed from EVC struct 
 * 
 * @param evc_idx EVC #
 * @param ptin_intf Interface #
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_evc_intf_remove(L7_uint evc_idx, L7_uint ptin_intf)
{
  L7_BOOL is_stacked;
  L7_BOOL is_root;
  L7_uint16 out_vlan;
  L7_uint16 int_vlan;
  ptin_intf_t intf;

  is_stacked = (evcs[evc_idx].flags & PTIN_EVC_MASK_STACKED) == PTIN_EVC_MASK_STACKED;
  is_root    = evcs[evc_idx].intf[ptin_intf].type == PTIN_EVC_INTF_ROOT;
  out_vlan   = evcs[evc_idx].intf[ptin_intf].out_vlan;
  int_vlan   = evcs[evc_idx].intf[ptin_intf].int_vlan;

  LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: removing %s# %02u (MEF %s Out.VID=%04u Int.VID=%04u) ...",
            evc_idx,
            ptin_intf < PTIN_SYSTEM_N_PORTS ? "PHY":"LAG",
            ptin_intf < PTIN_SYSTEM_N_PORTS ? ptin_intf : ptin_intf - PTIN_SYSTEM_N_PORTS,
            evcs[evc_idx].intf[ptin_intf].type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf",
            evcs[evc_idx].intf[ptin_intf].out_vlan,
            evcs[evc_idx].intf[ptin_intf].int_vlan);

  if (is_root)
  {
    int_vlan = evcs[evc_idx].rvlan;

    if (switching_root_remove(ptin_intf, out_vlan, int_vlan) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error removing root interface [ptin_intf=%u Vs=%u Vr=%u]",
              evc_idx, ptin_intf, out_vlan, int_vlan);
      return L7_FAILURE;
    }
    evcs[evc_idx].n_roots--;
  }
  else
  {
    if (switching_leaf_remove(ptin_intf, int_vlan) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error removing leaf [ptin_intf=%u Vl=%u]",
              evc_idx, ptin_intf, int_vlan);
      return L7_FAILURE;
    }
    evcs[evc_idx].n_leafs--;

    if (is_stacked)
    {
      /* Add translations for leaf ports, only if we are in matrix board */
      #if ( PTIN_BOARD_IS_MATRIX )
      L7_RC_t rc;
      rc = switching_stacked_leaf_remove(ptin_intf, out_vlan, 0, int_vlan);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC, "Error removing translations for leaf interface %u (rc=%d)",ptin_intf, rc);
        return L7_FAILURE;
      }
      #endif
    }
    else
    {
      ptin_evc_vlan_free(int_vlan, evcs[evc_idx].queue_free_vlans); /* free VLAN */
    }
  }

  /* Update snooping configuration */
  if (ptin_intf_port2ptintf(ptin_intf,&intf)==L7_SUCCESS)
  {
    if (ptin_igmp_is_evc_used(evc_idx))
    {
      ptin_igmp_snooping_trap_interface_update(evc_idx,&intf,L7_FALSE);
      LOG_TRACE(LOG_CTX_PTIN_EVC,"IGMP packet trapping updated for interface %u/%u",intf.intf_type,intf.intf_id);
    }
    if (ptin_dhcp_is_evc_used(evc_idx))
    {
      ptin_dhcp_snooping_trap_interface_update(evc_idx,&intf,L7_FALSE);
      LOG_TRACE(LOG_CTX_PTIN_EVC,"DHCP packet trapping updated for interface %u/%u",intf.intf_type,intf.intf_id);
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: %s# %02u (MEF %s Out.VID=%04u Int.VID=%04u) successfully removed",
            evc_idx,
            ptin_intf < PTIN_SYSTEM_N_PORTS ? "PHY":"LAG",
            ptin_intf < PTIN_SYSTEM_N_PORTS ? ptin_intf : ptin_intf - PTIN_SYSTEM_N_PORTS,
            evcs[evc_idx].intf[ptin_intf].type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf",
            evcs[evc_idx].intf[ptin_intf].out_vlan,
            evcs[evc_idx].intf[ptin_intf].int_vlan);

  evcs_intfs_in_use[ptin_intf]--; /* Remove this interface from the list of members in use */

  /* Clear intf struct (common info) */
  evcs[evc_idx].intf[ptin_intf].in_use   = L7_FALSE;
  evcs[evc_idx].intf[ptin_intf].type     = 0;
  evcs[evc_idx].intf[ptin_intf].out_vlan = 0;
  evcs[evc_idx].intf[ptin_intf].int_vlan = 0;

  return L7_SUCCESS;
}

/**
 * Removes all interfaces from an EVC
 * NOTE: EVC interfaces data is cleared
 * 
 * @param evc_idx 
 */
static L7_RC_t ptin_evc_intf_remove_all(L7_uint evc_idx)
{
  L7_uint i;
  L7_RC_t rc = L7_SUCCESS;

  /* Remove all configured interfaces */
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
  {
    if (!evcs[evc_idx].intf[i].in_use)
      continue;

    if (ptin_evc_intf_remove(evc_idx, i) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: failed to remove intf# %u", evc_idx, i);
      rc = L7_FAILURE;
    }
  }

  return rc;
}


/**
 * Adds translation entries to the leaf interface and also to the root ports 
 * in order to create an E-Tree
 * 
 * @param evc_idx    EVC #
 * @param leaf_intf  Leaf interface #
 * @param roots_intf Root interfaces list
 * @param n_roots    Nr of root intf present in the roots_intf array
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_evc_unstacked_intf_add(L7_uint evc_idx, L7_uint ptin_intf)
{
  L7_uint   l, r;
  L7_uint   intf_list[PTIN_SYSTEM_N_INTERF];
  L7_uint   n_intf;
  L7_RC_t   rc = L7_SUCCESS;

  /* Root intf ? */
  if (evcs[evc_idx].intf[ptin_intf].type == PTIN_EVC_INTF_ROOT)
  {
    /* Get all leaf interfaces... */
    ptin_evc_intf_list_get(evc_idx, PTIN_EVC_INTF_LEAF, intf_list, &n_intf);

    LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: configuring %u leafs on root intf# %02u (Out.VID=%04u) ...",
              evc_idx,
              n_intf,
              ptin_intf,
              evcs[evc_idx].intf[ptin_intf].out_vlan);

    /* Add all leaf xlate entries on the root port */
    for (l=0; l<n_intf; l++)
    {
      rc = switching_unstacked_root_add(ptin_intf,                                    /* Root intf */
                                        evcs[evc_idx].intf[ptin_intf].out_vlan,       /* Vs */
                                        evcs[evc_idx].intf[intf_list[l]].int_vlan);   /* Vl */

      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error adding to root a bridge leaf entry [root_intf=%u Vs=%u Vl=%u]",
                evc_idx, ptin_intf, evcs[evc_idx].intf[ptin_intf].out_vlan, evcs[evc_idx].intf[intf_list[l]].int_vlan);
        break;
      }
    }
  }
  /* Leaf... */
  else
  {
    /* Get all root interfaces... */
    ptin_evc_intf_list_get(evc_idx, PTIN_EVC_INTF_ROOT, intf_list, &n_intf);

    LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: configuring leaf intf# %u (Int.VID=%04u) on %u root interfaces...",
              evc_idx,
              ptin_intf,
              evcs[evc_idx].intf[ptin_intf].int_vlan,
              n_intf);

    /* Add leaf xlate entry on all root ports */
    for (r=0; r<n_intf; r++)
    {
      rc = switching_unstacked_root_add(intf_list[r],                                 /* Root intf */
                                        evcs[evc_idx].intf[intf_list[r]].out_vlan,    /* Vs */
                                        evcs[evc_idx].intf[ptin_intf].int_vlan);      /* Vl */

      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error adding to root a bridge leaf entry [root_intf=%u Vs=%u Vl=%u]",
                evc_idx, intf_list[r], evcs[evc_idx].intf[intf_list[r]].out_vlan, evcs[evc_idx].intf[ptin_intf].int_vlan);
        break;
      }
    }

    LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: configuring leaf intf# %02u (Out.VID=%04u Int.VID=%04u) on root Int.VLAN=%04u",
              evc_idx,
              ptin_intf,
              evcs[evc_idx].intf[ptin_intf].out_vlan,
              evcs[evc_idx].intf[ptin_intf].int_vlan,
              evcs[evc_idx].rvlan);

    /* Finally add the bridge leaf entry */
    rc = switching_unstacked_leaf_add(ptin_intf,                                      /* Leaf intf */
                                      evcs[evc_idx].intf[ptin_intf].out_vlan,         /* Vs' */
                                      evcs[evc_idx].intf[ptin_intf].int_vlan,         /* Vl */
                                      evcs[evc_idx].rvlan);                           /* Vr */

    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error adding bridge leaf entry [leaf_intf=%u Vs'=%u Vl=%u Vr=%u]",
              evc_idx, ptin_intf, evcs[evc_idx].intf[ptin_intf].out_vlan,
              evcs[evc_idx].intf[ptin_intf].int_vlan, evcs[evc_idx].rvlan);
    }
  }

  return rc;
}

/**
 * Removes full bridge configuration associated with a given interface
 * ONLY applies to unstacked EVCs
 * 
 * NOTE: When a bridge root is removed, only the configuration from the root is 
 * removed, and not the associated leafs. However, if a bridge leaf is removed, 
 * the configuration associated with all the roots is also removed. 
 * 
 * @param evc_idx   EVC #
 * @param ptin_intf Interface #
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_evc_unstacked_intf_remove(L7_uint evc_idx, L7_uint ptin_intf)
{
  L7_uint   l, r;
  L7_uint   intf_list[PTIN_SYSTEM_N_INTERF];
  L7_uint   n_intf;
  L7_RC_t   rc = L7_SUCCESS;

  /* Root intf ? */
  if (evcs[evc_idx].intf[ptin_intf].type == PTIN_EVC_INTF_ROOT)
  {
    /* Get all leaf interfaces... */
    ptin_evc_intf_list_get(evc_idx, PTIN_EVC_INTF_LEAF, intf_list, &n_intf);

    LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: removing %u leafs of root intf# %02u (Out.VID=%04u) ...",
              evc_idx,
              n_intf,
              ptin_intf,
              evcs[evc_idx].intf[ptin_intf].out_vlan);

    /* Remove all leaf xlate entries on the root port */
    for (l=0; l<n_intf; l++)
    {
      rc = switching_unstacked_root_remove(ptin_intf,                                   /* Root intf */
                                           evcs[evc_idx].intf[ptin_intf].out_vlan,      /* Vs */
                                           evcs[evc_idx].intf[intf_list[l]].int_vlan);  /* Vl */

      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error removing from root a bridge leaf entry [root_intf=%u Vs=%u Vl=%u]",
                evc_idx, ptin_intf, evcs[evc_idx].intf[ptin_intf].out_vlan, evcs[evc_idx].intf[intf_list[l]].int_vlan);
        break;
      }
    }
  }
  /* Leaf... */
  else
  {
    /* Get all root interfaces... */
    ptin_evc_intf_list_get(evc_idx, PTIN_EVC_INTF_ROOT, intf_list, &n_intf);

    LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: removing leaf intf# %u (Int.VID=%04u) of %u root interfaces...",
              evc_idx,
              ptin_intf,
              evcs[evc_idx].intf[ptin_intf].int_vlan,
              n_intf);

    /* Remove leaf xlate entry on all root ports */
    for (r=0; r<n_intf; r++)
    {
      rc = switching_unstacked_root_remove(intf_list[r],                                /* Root intf */
                                           evcs[evc_idx].intf[intf_list[r]].out_vlan,   /* Vs */
                                           evcs[evc_idx].intf[ptin_intf].int_vlan);     /* Vl */

      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error removing from root a bridge leaf entry [root_intf=%u Vs=%u Vl=%u]",
                evc_idx, intf_list[r], evcs[evc_idx].intf[intf_list[r]].out_vlan, evcs[evc_idx].intf[ptin_intf].int_vlan);
        break;
      }
    }

    LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: removing leaf intf# %02u (Out.VID=%04u Int.VID=%04u) of root Int.VLAN=%04u",
              evc_idx,
              ptin_intf,
              evcs[evc_idx].intf[ptin_intf].out_vlan,
              evcs[evc_idx].intf[ptin_intf].int_vlan,
              evcs[evc_idx].rvlan);

    /* Finally remove the bridge leaf entry */
    rc = switching_unstacked_leaf_remove(ptin_intf,                                     /* Leaf intf */
                                         evcs[evc_idx].intf[ptin_intf].out_vlan,        /* Vs' */
                                         evcs[evc_idx].intf[ptin_intf].int_vlan,        /* Vl */
                                         evcs[evc_idx].rvlan);                          /* Vr */

    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: error removing bridge leaf entry [leaf_intf=%u Vs'=%u Vl=%u Vr=%u]",
              evc_idx, ptin_intf, evcs[evc_idx].intf[ptin_intf].out_vlan,
              evcs[evc_idx].intf[ptin_intf].int_vlan, evcs[evc_idx].rvlan);
    }
  }

  return rc;
}

/**
 * Removes all bridges interfaces from an EVC (unstacked) 
 * NOTE: EVC interfaces data is not cleared
 * 
 * @param evc_idx 
 */
static L7_RC_t ptin_evc_unstacked_intf_remove_all(L7_uint evc_idx)
{
  L7_uint i;
  L7_RC_t rc = L7_SUCCESS;

  /* Remove bridges (unstacked EVCs) */
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
  {
    if (!evcs[evc_idx].intf[i].in_use)
      continue;

    if (ptin_evc_unstacked_intf_remove(evc_idx, i) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u: failed to remove intf# %u bridge config", evc_idx, i);
      rc = L7_FAILURE;
    }
  }

  return rc;
}


/**
 * Gets the list of root/leaf interfaces of an EVC
 * 
 * @param evc_idx 
 * @param mef_type  PTIN_EVC_INTF_ROOT / PTIN_EVC_INTF_LEAF 
 * @param intf_list (output) Array with the root/leaf list
 * @param n_elems   (output) Nr of elems of list array
 */
static void ptin_evc_intf_list_get(L7_uint evc_idx, L7_uint8 mef_type, L7_uint intf_list[], L7_uint *n_elems)
{
  L7_uint i;

  *n_elems = 0;
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
  {
    if ((evcs[evc_idx].intf[i].in_use) &&
        (evcs[evc_idx].intf[i].type == mef_type))
    {
      intf_list[*n_elems] = i;
      (*n_elems)++;
    }
  }

//LOG_TRACE(LOG_CTX_PTIN_EVC, "EVC# %u: %u %s", evc_idx,
//          *n_elems, mef_type == PTIN_EVC_INTF_ROOT ? "Roots":"Leafs");
}

/**
 * Search client based on inner VID on a double-linked queue
 * 
 * @param queue 
 * @param pelem 
 */
static void ptin_evc_find_client(L7_uint16 inn_vlan, dl_queue_t *queue, dl_queue_elem_t **pelem)
{
  struct ptin_evc_client_s *pclient = NULL;

  *pelem = NULL;

  dl_queue_get_head(queue, (dl_queue_elem_t **)&pclient);
  if (pclient == NULL)
    return;

  do
  {
    if (pclient->inn_vlan == inn_vlan)
    {
      *pelem = (dl_queue_elem_t *) pclient;
      return;
    }
    pclient = (struct ptin_evc_client_s *) dl_queue_get_next(queue, (dl_queue_elem_t *)pclient);

  } while (pclient != NULL);

  return;
}


/* Internal VLANs management functions ****************************************/

/**
 * Initializes the VLANs pool (queue of free internal VLANs)
 */
static void ptin_evc_vlan_pool_init(void)
{
  L7_uint i;
#if (PTIN_SYSTEM_GROUP_VLANS)
  L7_uint block;

  /* Stacked block */
  dl_queue_init(&queue_stacked_free_vlans);

  for (i=PTIN_VLAN_MIN; i<PTIN_VLAN_MAX && i<PTIN_SYSTEM_EVC_STACKED_VLAN_MAX; i++)
  {
    vlans_pool[i].vid = i;
    dl_queue_add(&queue_stacked_free_vlans, (dl_queue_elem_t*)&vlans_pool[i]);
  }

  /* Unstacked blocks */
  for (block=0; block<PTIN_SYSTEM_EVC_UNSTACKED_VLAN_BLOCKS; block++)
  {
    dl_queue_init(&queue_unstacked_free_vlans[block]);

    for (i = PTIN_SYSTEM_EVC_UNSTACKED_VLAN_MIN + block*PTIN_SYSTEM_EVC_VLANS_PER_BLOCK;
         i < PTIN_SYSTEM_EVC_UNSTACKED_VLAN_MIN + (block+1)*PTIN_SYSTEM_EVC_VLANS_PER_BLOCK;
         i++)
    {
      if (i<PTIN_VLAN_MIN || i>PTIN_VLAN_MAX)  continue;

      vlans_pool[i].vid = i;
      dl_queue_add(&queue_unstacked_free_vlans[block], (dl_queue_elem_t*)&vlans_pool[i]);
    }
  }

  /* Unstacked free vlan queues */
  dl_queue_init(&queue_unstacked_freeVlan_queues);

  for (i=0; i<PTIN_SYSTEM_EVC_UNSTACKED_VLAN_BLOCKS; i++)
  {
    queues_pool[i].queue = &queue_unstacked_free_vlans[i];
    dl_queue_add(&queue_unstacked_freeVlan_queues, (dl_queue_elem_t*)&queues_pool[i]);
  }

#else
  dl_queue_init(&queue_free_vlans);

  for (i=PTIN_VLAN_MIN; i<=PTIN_VLAN_MAX; i++)
  {
    vlans_pool[i].vid = i;
    dl_queue_add(&queue_free_vlans, (dl_queue_elem_t*)&vlans_pool[i]);
  }
#endif

  /* Reset 'evcId reference from internal vlan' array*/
  memset(evcId_from_internalVlan, 0xff, sizeof(evcId_from_internalVlan));
}

/**
 * Allocates a free VLAN queue from the pool
 * 
 * @param evc_idx    : EVC index 
 * @param is_stacked : Is EVC stacked 
 * @param queue : Pointer to free vlan queue (output)
 * 
 * @return L7_RC_t L7_SUCCESS if success
 * @return L7_RC_t L7_FAILURE if there are no VLANs available
 */
static L7_RC_t ptin_evc_freeVlanQueue_allocate(L7_uint16 evc_idx, L7_BOOL is_stacked, dl_queue_t **freeVlan_queue)
{
 #if (PTIN_SYSTEM_GROUP_VLANS)
  struct ptin_queue_s *fv_queue;

  if (evc_idx>PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid EVC index (%u)",evc_idx);
    return L7_FAILURE;
  }

  /* evc is stacked, use apropriate free vlan queue */
  if (is_stacked)
  {
    *freeVlan_queue = &queue_stacked_free_vlans;
    LOG_TRACE(LOG_CTX_PTIN_EVC, "Stacked Free Vlan Queue selected!");
    return L7_SUCCESS;
  }

  if (queue_unstacked_freeVlan_queues.n_elems == 0)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "There is no free VLAN queues available");
    return L7_FAILURE;
  }

  dl_queue_remove_head(&queue_unstacked_freeVlan_queues, (dl_queue_elem_t**)&fv_queue);

  *freeVlan_queue = fv_queue->queue;
  LOG_TRACE(LOG_CTX_PTIN_EVC, "Allocated free vlan queue index=%u (%u available)",
            ((L7_uint32) *freeVlan_queue - (L7_uint32) queue_unstacked_free_vlans)/sizeof(dl_queue_t),
            queue_unstacked_freeVlan_queues.n_elems);
 #else
  *freeVlan_queue = &queue_free_vlans;
 #endif

  return L7_SUCCESS;
}

/**
 * Frees a free VLAN queue from the pool
 * 
 * @param vlan (output) Pointer where the VLAN will be stored
 * 
 * @return L7_RC_t L7_SUCCESS if success
 * @return L7_RC_t L7_FAILURE if there are no VLANs available
 */
static L7_RC_t ptin_evc_freeVlanQueue_free(dl_queue_t *freeVlan_queue)
{
 #if (PTIN_SYSTEM_GROUP_VLANS)
  L7_uint32 pool_index;

  /* No (free vlan) queue provided */
  if (freeVlan_queue == L7_NULLPTR)
  {
    //LOG_WARNING(LOG_CTX_PTIN_EVC, "Null pointer given as free vlan queue... do nothing!");
    return L7_SUCCESS;
  }

  /* If (free vlan) queue is the stacked one, do nothing */
  if (freeVlan_queue == &queue_stacked_free_vlans)
  {
    LOG_TRACE(LOG_CTX_PTIN_EVC, "Stacked Free Vlan Queue given... do nothing!");
    return L7_SUCCESS;
  }

  pool_index = ((L7_uint32) freeVlan_queue - (L7_uint32) &queue_unstacked_free_vlans[0])/sizeof(dl_queue_t);

  if ((L7_uint32) freeVlan_queue < (L7_uint32) &queue_unstacked_free_vlans[0] ||
      (L7_uint32) freeVlan_queue > (L7_uint32) &queue_unstacked_free_vlans[PTIN_SYSTEM_EVC_UNSTACKED_VLAN_BLOCKS-1] ||
      ( ((L7_uint32) freeVlan_queue - (L7_uint32) &queue_unstacked_free_vlans[0])%sizeof(dl_queue_t) ) != 0 )
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "freeVlan Queue pointer value is invalid (%u)!",pool_index);
    return L7_FAILURE;
  }

  /* Index directly to the pool array and add the element to the free queue */
  dl_queue_add_head(&queue_unstacked_freeVlan_queues, (dl_queue_elem_t *) &queues_pool[pool_index]);

  LOG_TRACE(LOG_CTX_PTIN_EVC, "Freed free vlan queue index=%u (%u available)",pool_index,queue_unstacked_freeVlan_queues.n_elems);
 #endif

  return L7_SUCCESS;
}

/**
 * Allocates a VLANs from the pool and creates it
 * 
 * @param vlan (output) Pointer where the VLAN will be stored
 * 
 * @return L7_RC_t L7_SUCCESS if success
 * @return L7_RC_t L7_FAILURE if there are no VLANs available
 */
static L7_RC_t ptin_evc_vlan_allocate(L7_uint16 *vlan, dl_queue_t *queue_vlans, L7_uint16 evc_idx)
{
  struct ptin_vlan_s *pvlan;

  if (queue_vlans->n_elems == 0)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "There no VLANs available");
    return L7_FAILURE;
  }

  dl_queue_remove_head(queue_vlans, (dl_queue_elem_t**)&pvlan);

  if (switching_vlan_create(pvlan->vid) != L7_SUCCESS)
  {
    dl_queue_add_head(queue_vlans, (dl_queue_elem_t *) pvlan);

    LOG_ERR(LOG_CTX_PTIN_EVC, "Error creating VLAN %u", pvlan->vid);
    return L7_FAILURE;
  }

  *vlan = pvlan->vid;
  LOG_TRACE(LOG_CTX_PTIN_EVC, "Allocated Internal VLAN %04u (%u available)", *vlan, queue_vlans->n_elems);

  /* Mark this internal vlan, as being used by the given evc id */
  if (pvlan->vid<4096)
  {
    if (evc_idx<PTIN_SYSTEM_N_EVCS)
    {
      evcId_from_internalVlan[pvlan->vid] = evc_idx;
    }
    else
    {
      evcId_from_internalVlan[pvlan->vid] = (L7_uint8) -1;
    }
  }

  return L7_SUCCESS;
}

/**
 * Free a VLAN (add it to the pool again) and deletes it
 * 
 * @param vlan
 * 
 * @return L7_RC_t L7_SUCCESS
 */
static L7_RC_t ptin_evc_vlan_free(L7_uint16 vlan, dl_queue_t *queue_vlans)
{
  if (switching_vlan_delete(vlan) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error deleting VLAN %u", vlan);
    return L7_FAILURE;
  }

  /* Index directly to the pool array and add the element to the free queue */
  dl_queue_add_head(queue_vlans, (dl_queue_elem_t *) &vlans_pool[vlan]);

  LOG_TRACE(LOG_CTX_PTIN_EVC, "Freed Internal VLAN %04u (%u available)", vlan, queue_vlans->n_elems);

  /* Free this internal vlan, as not being used by any evc */
  if (vlan<4096)
  {
    evcId_from_internalVlan[vlan] = (L7_uint8) -1;
  }

  return L7_SUCCESS;
}


/* Switching configuration functions ******************************************/

/**
 * Configures a root port (for stacked and unstacked EVCs) 
 *  1. Associates the internal VLAN to the root intf
 *  2. Adds egress and ingress xlate entries (Out.VLAN<=>Int.VLAN)
 * 
 * @param root_intf     Root interface (ptin_intf)
 * @param out_vlan      Outer VLAN
 * @param int_vlan      Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_root_add(L7_uint root_intf, L7_uint16 out_vlan, L7_uint16 int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Adding root intf# %u [Out.VID=%u Int.VLAN=%u]...",
           root_intf, out_vlan, int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(root_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Interface is invalid: %u", root_intf);
    return L7_FAILURE;
  }

  /* Associate root internal vlan to the root intf */
  rc = usmDbVlanMemberSet(1, int_vlan, intIfNum, L7_DOT1Q_FIXED, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error associating root Int.VLAN %u to root intIfNum# %u (rc=%d)", int_vlan, intIfNum, rc);
    return L7_FAILURE;
  }

  /* Configure the internal VLAN on this interface as tagged */
  rc = usmDbVlanTaggedSet(1, int_vlan, intIfNum, L7_DOT1Q_TAGGED);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error setting intIfNum# %u internal VLAN %u as tagged (rc=%d)", intIfNum, int_vlan, rc);
    return L7_FAILURE;
  }

  /* Add ingress xlate entry: (root_intf) out_vlan -> int_vlan */
  rc = ptin_xlate_ingress_add(intIfNum, out_vlan, 0, int_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error adding intIfNum# %u xlate Ingress entry [Out.VLAN %u => Int.VLAN] %u (rc=%d)",
            intIfNum, out_vlan, int_vlan, rc);
    return L7_FAILURE;
  }

  /* Add egress xlate entry: (root_intf) int_vlan -> out_vlan */
  rc = ptin_xlate_egress_add(intIfNum, int_vlan, 0, out_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error adding intIfNum# %u xlate Egress entry [Int.VLAN %u => Out.VLAN %u] (rc=%d)",
            intIfNum, int_vlan, out_vlan, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Removes a root port (for stacked and unstacked EVCs) 
 * 
 * @param root_intf Root interface (ptin_intf)
 * @param out_vlan  Outer VLAN
 * @param int_vlan  Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_root_remove(L7_uint root_intf, L7_uint16 out_vlan, L7_uint16 int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Removing root intf# %u [Out.VID=%u Int.VLAN=%u]...",
           root_intf, out_vlan, int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(root_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Interface is invalid: %u", root_intf);
    return L7_FAILURE;
  }

  /* Delete egress xlate entry: (root_intf) int_vlan -> out_vlan */
  rc = ptin_xlate_egress_delete(intIfNum, int_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error deleting intIfNum# %u xlate Egress entry [Int.VLAN %u => Out.VLAN %u} (rc=%d)",
            intIfNum, int_vlan, out_vlan, rc);
    return L7_FAILURE;
  }

  /* Delete ingress xlate entry: (root_intf) out_vlan -> int_vlan */
  rc = ptin_xlate_ingress_delete(intIfNum, out_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error deleting intIfNum# %u xlate Ingress entry [Out.VLAN %u => Int.VLAN %u] (rc=%d)",
            intIfNum, out_vlan, int_vlan, rc);
    return L7_FAILURE;
  }

  /* Delete intIfNum from int_vlan */
  rc = usmDbVlanMemberSet(1, int_vlan, intIfNum, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error deleting intIfNum# %u from Int.VLAN %u (rc=%d)", intIfNum, int_vlan, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Configures a leaf port (for stacked and unstacked EVCs) 
 *  1. Associates the internal VLAN to the leaf intf
 * 
 * @param leaf_intf Leaf interface (ptin_intf)
 * @param int_vlan  Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_leaf_add(L7_uint leaf_intf, L7_uint16 leaf_int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Adding leaf intf# %u [Int.VLAN=%u]...",
           leaf_intf, leaf_int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(leaf_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Associate leaf internal vlan to the leaf intf */
  rc = usmDbVlanMemberSet(1, leaf_int_vlan, intIfNum, L7_DOT1Q_FIXED, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error associating leaf Int.VLAN %u to leaf intIfNum# %u (rc=%d)", leaf_int_vlan, intIfNum, rc);
    return L7_FAILURE;
  }

  /* Configure the internal VLAN on this interface as tagged */
  rc = usmDbVlanTaggedSet(1, leaf_int_vlan, intIfNum, L7_DOT1Q_TAGGED);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error setting intIfNum# %u internal VLAN %u as tagged (rc=%d)", intIfNum, leaf_int_vlan, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Removes a leaf port (for stacked and unstacked EVCs) 
 * 
 * @param leaf_intf Leaf interface (ptin_intf)
 * @param int_vlan  Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Removing leaf intf# %u [Int.VLAN=%u]...",
           leaf_intf, leaf_int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(leaf_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Delete intIfNum from leaf_int_vlan */
  rc = usmDbVlanMemberSet(1, leaf_int_vlan, intIfNum, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error deleting intIfNum# %u from Int.VLAN %u (rc=%d)", intIfNum, leaf_int_vlan, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Configures a leaf port (only for stacked EVCs) 
 * 
 * @param leaf_intf Leaf interface (ptin_intf)
 * @param int_vlan  Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_stacked_leaf_add(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Adding stacked leaf intf# %u [Int.VLAN=%u]...",
           leaf_intf, int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(leaf_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Add ingress xlate entry: (leaf_intf) (Vs',Vc) => (Vr,Vc) */
  rc = ptin_xlate_ingress_add(intIfNum, leaf_out_vlan, 0, int_vlan, leaf_inner_vlan);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error adding intf %u xlate Ingress entry [Leaf Out.VLAN %u + Inn.VLAN %u => Root Int.VLAN %u] (rc=%d)",
            leaf_intf, leaf_out_vlan, leaf_inner_vlan, int_vlan, rc);
    return L7_FAILURE;
  }

  /* Add egress xlate entry: (leaf_intf) (Vr,Vc) => (Vs',Vc) */
  rc = ptin_xlate_egress_add(intIfNum, int_vlan, leaf_inner_vlan, leaf_out_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error adding intf %u xlate Egress entry [Root Int.VLAN %u + Inn.VLAN %u => Leaf Out.VLAN %u] (rc=%d)",
            leaf_intf, int_vlan, leaf_inner_vlan, leaf_out_vlan, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Removes a leaf port (for stacked and unstacked EVCs) 
 * 
 * @param leaf_intf Leaf interface (ptin_intf)
 * @param int_vlan  Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_stacked_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Removing stacked leaf intf# %u [Int.VLAN=%u]...",
           leaf_intf, int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(leaf_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Add ingress xlate entry: (leaf_intf) (Vs',Vc) => (Vr,Vc) */
  rc = ptin_xlate_ingress_delete(intIfNum, leaf_out_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error deleting intf %u xlate Ingress entry [Leaf Out.VLAN %u + Inn.VLAN %u] (rc=%d)",
            leaf_intf, leaf_out_vlan, 0, rc);
    return L7_FAILURE;
  }

  /* Add egress xlate entry: (leaf_intf) (Vr,Vc) => (Vs',Vc) */
  rc = ptin_xlate_egress_delete(intIfNum, int_vlan, leaf_inner_vlan);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error deleting intf %u xlate Egress entry [Root Int.VLAN %u + Inn.VLAN %u (rc=%d)",
            leaf_intf, int_vlan, leaf_inner_vlan, leaf_out_vlan, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/**
 * Adds root configuration for unstacked EVCs 
 *  1. configures leaf->root egress xlate
 *  2. associates leaf Int.VLAN (Vl) to root intf (Pr)
 * 
 * @param root_intf     Root interface # (ptin_intf)
 * @param root_out_vlan Root outer VLAN
 * @param leaf_int_vlan Leaf internal VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_unstacked_root_add(L7_uint root_intf, L7_uint16 root_out_vlan, L7_uint16 leaf_int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Configuring unstacked root intf# %u [Leaf Int.VLAN=%u => Root Out.VLAN=%u]...",
           root_intf, leaf_int_vlan, root_out_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(root_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Interface is invalid: %u", root_intf);
    return L7_FAILURE;
  }

  /* Add egress xlate entry: (root_intf) leaf_int_vlan -> root_out_vlan */
  rc = ptin_xlate_egress_add(intIfNum, leaf_int_vlan, 0, root_out_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error adding intIfNum# %u xlate Egress entry [Leaf Int.VLAN %u => Root Out.VLAN %u] (rc=%d)",
            intIfNum, leaf_int_vlan, root_out_vlan, rc);
    return L7_FAILURE;
  }

  /* Associate leaf internal vlan to the root intf */
  rc = usmDbVlanMemberSet(1, leaf_int_vlan, intIfNum, L7_DOT1Q_FIXED, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error associating leaf Int.VLAN %u to root intIfNum# %u to (rc=%d)", leaf_int_vlan, intIfNum, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Removes root configuration for unstacked EVCs 
 * 
 * @param root_intf     Root interface # (ptin_intf)
 * @param root_out_vlan Root outer VLAN
 * @param leaf_int_vlan Leaf internal VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_unstacked_root_remove(L7_uint root_intf, L7_uint16 root_out_vlan, L7_uint16 leaf_int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Removing unstacked root intf# %u [Leaf Int.VLAN=%u => Root Out.VLAN=%u]...",
           root_intf, leaf_int_vlan, root_out_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(root_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Interface is invalid: %u", root_intf);
    return L7_FAILURE;
  }

  /* Delete intIfNum from int_vlan */
  rc = usmDbVlanMemberSet(1, leaf_int_vlan, intIfNum, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error deleting intIfNum# %u from Int.VLAN %u (rc=%d)", intIfNum, leaf_int_vlan, rc);
    return L7_FAILURE;
  }

  /* Delete egress xlate entry: (root_intf) leaf_int_vlan -> root_out_vlan */
  rc = ptin_xlate_egress_delete(intIfNum, leaf_int_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error deleting intIfNum# %u xlate Egress entry [Leaf Int.VLAN %u => Root Out.VLAN %u] (rc=%d)",
            intIfNum, leaf_int_vlan, root_out_vlan, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Adds leaf configuration for unstacked EVCs 
 *  1. configures ingress xlate (leaf outer vlan => leaf internal vlan)
 *  2. configures egress xlate (root internal vlan => leaf outer vlan)
 *  3. associates leaf_intf to root int.vlan (Vr)
 * 
 * @param leaf_intf     Leaf interface # (ptin_intf)
 * @param leaf_out_vlan Leaf outer VLAN
 * @param leaf_int_vlan Leaf internal VLAN
 * @param root_int_vlan Root internal VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_unstacked_leaf_add(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_int_vlan, L7_uint16 root_int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Configuring unstacked leaf intf# %u [Leaf Out.VLAN=%u => Leaf Int.VLAN=%u; Root Int.VLAN=%u => Leaf Out.VLAN=%u]...",
           leaf_intf, leaf_out_vlan, leaf_int_vlan, root_int_vlan, leaf_out_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(leaf_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Add ingress xlate entry: (leaf_intf)  (leaf outer vlan => leaf internal vlan) */
  rc = ptin_xlate_ingress_add(intIfNum, leaf_out_vlan, 0, leaf_int_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error adding intIfNum# %u xlate Ingress entry [Leaf Out.VLAN %u => Leaf Int.VLAN %u] (rc=%d)",
            intIfNum, leaf_out_vlan, leaf_int_vlan, rc);
    return L7_FAILURE;
  }

  /* Add egress xlate entry: (leaf_intf) (root internal vlan => leaf outer vlan) */
  rc = ptin_xlate_egress_add(intIfNum, root_int_vlan, 0, leaf_out_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error adding intIfNum# %u xlate Egress entry [Root Int.VLAN %u => Leaf Out.VLAN %u] (rc=%d)",
            intIfNum, root_int_vlan, leaf_out_vlan, rc);
    return L7_FAILURE;
  }

  /* Associate root internal vlan to the leaf intf */
  rc = usmDbVlanMemberSet(1, root_int_vlan, intIfNum, L7_DOT1Q_FIXED, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error associating root Int.VLAN %u to leaf intIfNum# %u to (rc=%d)", root_int_vlan, intIfNum, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Removes leaf configuration for unstacked EVCs 
 *  1. deletes ingress xlate (leaf outer vlan => leaf internal vlan)
 *  2. deletes egress xlate (root internal vlan => leaf outer vlan)
 *  3. deletes association of leaf_intf to root int.vlan (Vr)
 * 
 * @param leaf_intf     Leaf interface # (ptin_intf)
 * @param leaf_out_vlan Leaf outer VLAN
 * @param leaf_int_vlan Leaf internal VLAN
 * @param root_int_vlan Root internal VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_unstacked_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_int_vlan, L7_uint16 root_int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Removing unstacked leaf intf# %u [Leaf Out.VLAN=%u => Leaf Int.VLAN=%u; Root Int.VLAN=%u => Leaf Out.VLAN=%u]...",
           leaf_intf, leaf_out_vlan, leaf_int_vlan, root_int_vlan, leaf_out_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(leaf_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Delete intIfNum from int_vlan */
  rc = usmDbVlanMemberSet(1, root_int_vlan, intIfNum, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error deleting intIfNum# %u from Int.VLAN %u (rc=%d)", intIfNum, root_int_vlan, rc);
    return L7_FAILURE;
  }

  /* Deletes ingress xlate entry: (leaf_intf)  (leaf outer vlan => leaf internal vlan) */
  rc = ptin_xlate_ingress_delete(intIfNum, leaf_out_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error deleting intIfNum# %u xlate Ingress entry [Leaf Out.VLAN %u => Leaf Int.VLAN %u] (rc=%d)",
            intIfNum, leaf_out_vlan, leaf_int_vlan, rc);
    return L7_FAILURE;
  }

  /* Deletes egress xlate entry: (leaf_intf) (root internal vlan => leaf outer vlan) */
  rc = ptin_xlate_egress_delete(intIfNum, root_int_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error deleting intIfNum# %u xlate Egress entry [Root Int.VLAN %u => Leaf Out.VLAN %u] (rc=%d)",
            intIfNum, root_int_vlan, leaf_out_vlan, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/**
 * Adds a bridge between a root and leaf interface of a stacked EVC 
 *  1. configures translation entries (egress+ingress) on leaf interface
 *  2. configures a cross-connection between root and leaf interface
 * 
 * @param root_intf     Root ptin interface #
 * @param root_int_vid  Root internal VLAN (same as leaf)
 * @param leaf_intf     Leaf ptin interface #
 * @param leaf_out_vid  Leaf outer VLAN (Vs' -> new outer VLAN on client side)
 * @param leaf_inn_vid  Leaf inner VLAN (Vclient)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_stacked_bridge_add(L7_uint root_intf, L7_uint16 root_int_vid, L7_uint leaf_intf, L7_uint16 leaf_out_vid, L7_uint16 leaf_inn_vid)
{
  L7_uint32 root_intIfNum;
  L7_uint32 leaf_intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Adding stacked bridge [Root: Intf=%u IntVID=%u] <=> [Leaf: Intf=%u NEW Out.VID=%u Inn.VID=%u] ...",
           root_intf, root_int_vid, leaf_intf, leaf_out_vid, leaf_inn_vid);

  /* Get intIfNum of root */
  rc = ptin_intf_port2intIfNum(root_intf, &root_intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Interface is invalid: %u", root_intf);
    return L7_FAILURE;
  }

  /* Get intIfNum of root */
  rc = ptin_intf_port2intIfNum(leaf_intf, &leaf_intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

#if 1
  #if ( !PTIN_BOARD_IS_MATRIX )
  rc = switching_stacked_leaf_add(leaf_intf, leaf_out_vid, leaf_inn_vid, root_int_vid);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error adding translations for leaf interface %u (rc=%d)",leaf_intf, rc);
    return L7_FAILURE;
  }
  #endif
#else
  /* Add ingress xlate entry: (leaf_intf) (Vs',Vc) => (Vr,Vc) */
  rc = ptin_xlate_ingress_add(leaf_intIfNum, leaf_out_vid, 0, root_int_vid, leaf_inn_vid);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error adding intIfNum# %u xlate Ingress entry [Leaf Out.VLAN %u + Inn.VLAN %u => Root Int.VLAN %u] (rc=%d)",
            leaf_intIfNum, leaf_out_vid, leaf_inn_vid, root_int_vid, rc);
    return L7_FAILURE;
  }

  /* Add egress xlate entry: (leaf_intf) (Vr,Vc) => (Vs',Vc) */
  rc = ptin_xlate_egress_add(leaf_intIfNum, root_int_vid, leaf_inn_vid, leaf_out_vid, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error adding intIfNum# %u xlate Egress entry [Root Int.VLAN %u + Inn.VLAN %u => Leaf Out.VLAN %u] (rc=%d)",
            leaf_intIfNum, root_int_vid, leaf_inn_vid, leaf_out_vid, rc);
    return L7_FAILURE;
  }
#endif

  /* Add cross-connection between root and leaf interfaces */
  rc = ptin_crossconnect_add(root_int_vid, leaf_inn_vid, root_intIfNum, leaf_intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error adding Xconnect (OVID=%u, IVID=%u) (intIfNum1=%u, intIfNum2=%u) (rc=%d)",
            root_int_vid, leaf_inn_vid, root_intIfNum, leaf_intIfNum, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Removes a cross-connect between a root and leaf interface of a stacked EVC
 *  1. removes translation entries (egress+ingress) on leaf interface
 *  2. removes a cross-connection between root and leaf interface
  * 
 * @param root_intf     Root ptin interface #
 * @param root_int_vid  Root internal VLAN (same as leaf)
 * @param leaf_intf     Leaf ptin interface #
 * @param leaf_out_vid  Leaf outer VLAN (Vs' -> new outer VLAN on client side)
 * @param leaf_inn_vid  Leaf inner VLAN (Vclient)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_stacked_bridge_remove(L7_uint root_intf, L7_uint16 root_int_vid, L7_uint leaf_intf, L7_uint16 leaf_out_vid, L7_uint16 leaf_inn_vid)
{
  L7_uint32 root_intIfNum;
  L7_uint32 leaf_intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_EVC, "Removing stacked bridge [Root: Intf=%u IntVID=%u] <=> [Leaf: Intf=%u NEW Out.VID=%u Inn.VID=%u] ...",
           root_intf, root_int_vid, leaf_intf, leaf_out_vid, leaf_inn_vid);

  /* Get intIfNum of root */
  rc = ptin_intf_port2intIfNum(root_intf, &root_intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Interface is invalid: %u", root_intf);
    return L7_FAILURE;
  }

  /* Get intIfNum of root */
  rc = ptin_intf_port2intIfNum(leaf_intf, &leaf_intIfNum);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

#if 1
  #if ( !PTIN_BOARD_IS_MATRIX )
  rc = switching_stacked_leaf_remove(leaf_intf, leaf_out_vid, leaf_inn_vid, root_int_vid);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error removing translations for leaf interface %u (rc=%d)",leaf_intf, rc);
    return L7_FAILURE;
  }
  #endif
#else
  /* Deletes ingress xlate entry: (leaf_intf) (Vs',Vc) => (Vr,Vc) */
  rc = ptin_xlate_ingress_delete(leaf_intIfNum, leaf_out_vid, 0 /*leaf_inn_vid*/);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error deleting intIfNum# %u xlate Ingress entry [Leaf Out.VLAN %u + Inn.VLAN %u => Root Int.VLAN %u] (rc=%d)",
            leaf_intIfNum, leaf_out_vid, leaf_inn_vid, root_int_vid, rc);
    return L7_FAILURE;
  }

  /* Deletes egress xlate entry: (leaf_intf) (Vr,Vc) => (Vs',Vc) */
  rc = ptin_xlate_egress_delete(leaf_intIfNum, root_int_vid, leaf_inn_vid);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error deleting intIfNum# %u xlate Egress entry [Root Int.VLAN %u + Inn.VLAN %u => Leaf Out.VLAN %u] (rc=%d)",
            leaf_intIfNum, root_int_vid, leaf_inn_vid, leaf_out_vid, rc);
    return L7_FAILURE;
  }
#endif

  /* Deletes cross-connection between root and leaf interfaces */
  rc = ptin_crossconnect_delete(root_int_vid, leaf_inn_vid);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Error adding Xconnect (OVID=%u, IVID=%u) (intIfNum1=%u, intIfNum2=%u) (rc=%d)",
            root_int_vid, leaf_inn_vid, root_intIfNum, leaf_intIfNum, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/**
 * Creates a VLAN
 * 
 * @param vid 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_vlan_create(L7_uint16 vid)
{
  L7_RC_t rc = L7_SUCCESS;

  rc = usmDbVlanCreate(1, vid);
  if (rc != L7_SUCCESS)
  {
    LOG_CRITICAL(LOG_CTX_PTIN_EVC, "Error creating VLAN %u (rc=%d)", vid, rc);
    return L7_FAILURE;
  }

  rc = usmDbVlanMemberSet(1, vid, L7_ALL_INTERFACES, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    LOG_CRITICAL(LOG_CTX_PTIN_EVC, "Error removing VLAN %u from all interfaces (rc=%d)", vid, rc);
    return L7_FAILURE;
  }

  rc = usmDbVlanTaggedSet(1, vid, L7_ALL_INTERFACES, L7_DOT1Q_TAGGED);
  if (rc != L7_SUCCESS)
  {
    LOG_CRITICAL(LOG_CTX_PTIN_EVC, "Error setting VLAN %u as tagged on all interfaces (rc=%d)", vid, rc);
    return L7_FAILURE;
  }

  /* Wait until all requests are attended */
  while (!dot1qQueueIsEmpty())
    osapiSleepMSec(10);

  return L7_SUCCESS;
}

/**
 * Deletes a VLAN
 * 
 * @param vid 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_vlan_delete(L7_uint16 vid)
{
  L7_RC_t rc = L7_SUCCESS;

  rc = usmDbVlanDelete(1, vid);
  if (rc != L7_SUCCESS)
  {
    LOG_CRITICAL(LOG_CTX_PTIN_EVC, "Error deleting VLAN %u (rc=%d)", vid, rc);
    return L7_FAILURE;
  }

  /* Wait until all requests are attended */
  while (!dot1qQueueIsEmpty())
    osapiSleepMSec(10);

  return L7_SUCCESS;
}

/**
 * Configures Forward VLAN, MAC Learning and Multicast Flooding type 
 * for a particular VLAN 
 * 
 * @param vid           VLAN to be configured
 * @param fwr_vid       Forward VLAN
 * @param mac_learning  L7_TRUE/L7_FALSE
 * @param mc_flood      L7_FILTER_FORWARD_ALL, 
 *                      L7_FILTER_FORWARD_UNREGISTERED,
 *                      L7_FILTER_FILTER_UNREGISTERED
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_vlan_config(L7_uint16 vid, L7_uint16 fwd_vid, L7_BOOL mac_learning, L7_uint8 mc_flood, L7_uint8 cpu_trap)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_FILTER_VLAN_FILTER_MODE_t mcf = 0xff;

  /* Apply/remove cpu vlan */
  rc = ptin_vlan_cpu_set( vid, cpu_trap );
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "VLAN %u: error setting CPU trap state to %u (rc=%d)",vid, cpu_trap, rc);
    return L7_FAILURE;
  }

  /* Set Forward VLAN to int_vlan and set Mac Learning state */
  rc = ptin_crossconnect_vlan_learn(vid, fwd_vid, mac_learning);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "VLAN %u: error setting MAC Learning state to %s (w/ Forward VLAN %u) (rc=%d)",
            vid, mac_learning ? "Enabled":"Disabled", fwd_vid, rc);
    return L7_FAILURE;
  }

  /* Set Multicast Flooding type */
  rc = usmdbFilterVlanFilteringModeSet(1, vid, mc_flood);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "VLAN %u: error setting Multicast Flood type to %s (rc=%d)",
            vid, mc_flood == L7_FILTER_FORWARD_ALL ? "FORWARD_ALL":mc_flood == L7_FILTER_FORWARD_UNREGISTERED ? "FORWARD_UNREGISTERED":"FILTER_UNREGISTERED", rc);
    return L7_FAILURE;
  }
  usmdbFilterVlanFilteringModeGet(1, vid, &mcf);
  LOG_TRACE(LOG_CTX_PTIN_EVC,"success with usmdbFilterVlanFilteringModeSet(1,%u,%u) => usmdbFilterVlanFilteringModeSet(1,%u,&mc_flood): mc_flood=%u",vid,mc_flood,vid,mcf);

  return L7_SUCCESS;
}

/**
 * Verifies if the new EVC parameters are valid
 * 
 * @param evcConf : EVC configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_evc_param_verify(ptin_HwEthMef10Evc_t *evcConf)
{
  L7_uint     i, evc_id;
  L7_int      port;
  ptin_intf_t ptin_intf;

  /* Arguments */
  if (evcConf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Null pointer");
    return L7_FAILURE;
  }

  /* EVC index */
  if (evcConf->index>=PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid EVC index (%u)",evcConf->index);
    return L7_FAILURE;
  }

  /* Number of interfaces */
  if (evcConf->n_intf==0 || evcConf->n_intf>=PTIN_SYSTEM_N_PORTS_AND_LAGS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid number of interfaces (%u)",evcConf->n_intf);
    return L7_FAILURE;
  }

  /* Run all interfaces */
  for (i=0; i<evcConf->n_intf; i++)
  {
    /* Interface id and type */
    if (evcConf->intf[i].intf_type!=PTIN_EVC_INTF_PHYSICAL && evcConf->intf[i].intf_type!=PTIN_EVC_INTF_LOGICAL)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"Interface index %u has an invalid type (%u)",i,evcConf->intf[i].intf_type);
      return L7_FAILURE;
    }
    if ( (evcConf->intf[i].intf_type==PTIN_EVC_INTF_PHYSICAL && evcConf->intf[i].intf_id>=PTIN_SYSTEM_N_PORTS) ||
         (evcConf->intf[i].intf_type==PTIN_EVC_INTF_LOGICAL  && evcConf->intf[i].intf_id>=PTIN_SYSTEM_N_LAGS ) )
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"Interface index %u has an invalid id (%u/%u)",i,evcConf->intf[i].intf_type,evcConf->intf[i].intf_id);
      return L7_FAILURE;
    }
    /* MEF type */
    if (evcConf->intf[i].mef_type!=PTIN_EVC_INTF_ROOT && evcConf->intf[i].mef_type!=PTIN_EVC_INTF_LEAF)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"Interface index %u has an invalid mef type (%u/%u: %u)",i,evcConf->intf[i].intf_type,evcConf->intf[i].intf_id,evcConf->intf[i].mef_type);
      return L7_FAILURE;
    }

    /* If interface is root, or any of the unstacked EVCs,
       check if the vlan is not being used by other EVCs in the same interface */
    if (evcConf->intf[i].mef_type==PTIN_EVC_INTF_ROOT || !(evcConf->flags & PTIN_EVC_MASK_STACKED))
    {
      /* Vlan */
      if (evcConf->intf[i].vid==0 || evcConf->intf[i].vid>=4095)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC,"Interface index %u has an invalid vlan (%u/%u: %u)",i,evcConf->intf[i].intf_type,evcConf->intf[i].intf_id,evcConf->intf[i].vid);
        return L7_FAILURE;
      }

      ptin_intf.intf_type = evcConf->intf[i].intf_type;
      ptin_intf.intf_id   = evcConf->intf[i].intf_id;
      if (ptin_intf_ptintf2port(&ptin_intf,&port)==L7_SUCCESS && port<PTIN_SYSTEM_N_INTERF)
      {
        /* Run all active EVCs except the given one */
        for (evc_id=0; evc_id<PTIN_SYSTEM_N_EVCS; evc_id++)
        {
          /* Skip the referred evc or not used ones */
          if (evc_id==evcConf->index || !evcs[evc_id].in_use)  continue;
          /* Skip evcs which are not using this interface */
          if (!evcs[evc_id].intf[port].in_use)  continue;

          /* Skip leaf interfaces of stacked services */
          if (IS_EVC_INTF_LEAF(evc_id,port) && IS_EVC_STACKED(evc_id))  continue;

          /* If outer vlan matches, we have a conflict */
          if (evcs[evc_id].intf[port].out_vlan == evcConf->intf[i].vid)
          {
            LOG_ERR(LOG_CTX_PTIN_EVC,"Interface index %u, port=%u (%u/%u) of EVC %u has the same vlan %u",
                    i,port,ptin_intf.intf_type,ptin_intf.intf_id,evc_id,evcConf->intf[i].vid);
            return L7_FAILURE;
          }
        }
      }
    }
  }

  return L7_SUCCESS;
}

/**
 * Verify bandwidth profile parameters
 * 
 * @param evc_idx : evc index
 * @param profile : profile data 
 * @param bwPolicer_ptr : Location of policer pointer in evc 
 *                database (address of a ptr to a ptr to
 *                bwPolicer)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_evc_bwProfile_verify(L7_uint evc_idx, ptin_bw_profile_t *profile, void ***bwPolicer_ptr)
{
  nimUSP_t  usp;
  L7_uint32 intIfNum = L7_ALL_INTERFACES;
  L7_int    ptin_port, i_port;
  struct ptin_evc_client_s *pclient;

  LOG_TRACE(LOG_CTX_PTIN_EVC,"Starting bw profile verification");

  /* Profile pointer should have a valid address */
  if (profile==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Profile is a null pointer");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_EVC,"Initial bw profile data:");
  LOG_TRACE(LOG_CTX_PTIN_EVC," evcId       = %u",evc_idx);
  LOG_TRACE(LOG_CTX_PTIN_EVC," srcIntf     = {%d,%d,%d}",profile->ddUsp_src.unit,profile->ddUsp_src.slot,profile->ddUsp_src.port);
  LOG_TRACE(LOG_CTX_PTIN_EVC," dstIntf     = {%d,%d,%d}",profile->ddUsp_dst.unit,profile->ddUsp_dst.slot,profile->ddUsp_dst.port);
  LOG_TRACE(LOG_CTX_PTIN_EVC," OVID_in     = %u",profile->outer_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_EVC," OVID_out    = %u",profile->outer_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_EVC," IVID_in     = %u",profile->inner_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_EVC," {CIR,CBS}   = {%lu,%lu}",profile->meter.cir,profile->meter.cbs);
  LOG_TRACE(LOG_CTX_PTIN_EVC," {EIR,EBS}   = {%lu,%lu}",profile->meter.eir,profile->meter.ebs);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_idx >= PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u is out of range [0..%u]", evc_idx, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (!evcs[evc_idx].in_use)
  {
    LOG_WARNING(LOG_CTX_PTIN_EVC, "EVC# %u is not in use", evc_idx);
    return L7_NOT_EXIST;
  }

  /* If source interface is provided, validate it */
  if (profile->ddUsp_src.unit>=0 && profile->ddUsp_src.slot>=0 && profile->ddUsp_src.port>=0)
  {
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Processing source interface");
    /* Get USP interface */
    usp.unit = profile->ddUsp_src.unit;
    usp.slot = profile->ddUsp_src.slot;
    usp.port = profile->ddUsp_src.port + 1;
    /* Get interface number */
    if (nimGetIntIfNumFromUSP(&usp,&intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"Error getting interface number from USP={%d,%d,%d}",usp.unit,usp.slot,usp.port);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Source interface: intIfNum=%u",intIfNum);

    /* Get ptin_port */
    if (ptin_intf_intIfNum2port(intIfNum,&ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"Error getting ptin_port from intIfNum=%u",intIfNum);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Source interface: ptin_port=%u",ptin_port);

    /* Verify if interface is in use */
    if (!evcs[evc_idx].intf[ptin_port].in_use)
    {
      LOG_WARNING(LOG_CTX_PTIN_EVC,"ptin_port %d is not in use",ptin_port);
      return L7_NOT_EXIST;
    }
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Source interface is present in EVC");

    /* Verify Svlan*/
    if (profile->outer_vlan_in>0 &&
        evcs[evc_idx].intf[ptin_port].out_vlan>0 && evcs[evc_idx].intf[ptin_port].out_vlan<4096)
    {
      if (profile->outer_vlan_in!=evcs[evc_idx].intf[ptin_port].out_vlan)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC,"OVid_in %u does not match to the one in EVC (%u)",profile->outer_vlan_in,evcs[evc_idx].intf[ptin_port].out_vlan);
        return L7_FAILURE;
      }
      LOG_TRACE(LOG_CTX_PTIN_EVC,"Source interface (ptin_port=%u): OVid_in %u verified",ptin_port,profile->outer_vlan_in);
    }

    /* Default outer and inner vlan at egress:
       - the outer vlan is the defined for each interface
       - the inner vlan is the same as the internal inner vlan
       If interface is leaf and EVC is stacked, the outer and inner vlans at egress should not be considered */
    #if ( !PTIN_BOARD_IS_MATRIX )
    profile->outer_vlan_out = 0;
    profile->inner_vlan_out = 0;
    if (IS_EVC_INTF_ROOT(evc_idx,ptin_port) || IS_EVC_UNSTACKED(evc_idx))
    #endif
    {
      profile->outer_vlan_out = evcs[evc_idx].intf[ptin_port].out_vlan;
      profile->inner_vlan_out = profile->inner_vlan_in;
    }

    /* If bwPolicer_ptr is not null, we should provide a pointer to the location where the bwPolicer address will be stored */
    if (bwPolicer_ptr!=L7_NULLPTR)
    {
      /* If inner_vlan is null, use the general policer for the interface */
      if (profile->inner_vlan_in==0)
      {
        *bwPolicer_ptr = &(evcs[evc_idx].intf[ptin_port].bwprofile);
      } /* if (profile->inner_vlan_in==0) */
      /* If valid, find the specified client, and provide the policer location */
      else
      {
        /* Find the specified cvlan in all EVC clients */
        for (i_port=0, pclient=L7_NULLPTR; i_port<PTIN_SYSTEM_N_INTERF && pclient==L7_NULLPTR; i_port++)
        {
          if ( IS_EVC_INTF_ROOT(evc_idx,ptin_port) ||
              (IS_EVC_INTF_LEAF(evc_idx,ptin_port) && i_port==ptin_port))
          {
            ptin_evc_find_client(profile->inner_vlan_in, &(evcs[evc_idx].intf[i_port].clients), (dl_queue_elem_t **) &pclient);
          }
        }
        /* Client not found */
        if (pclient==L7_NULLPTR)
        {
          LOG_WARNING(LOG_CTX_PTIN_EVC,"Client %u not found in EVC %u",profile->inner_vlan_in,evc_idx);
          return L7_NOT_EXIST;
        }
        /* If interface is a leaf... */
        if (IS_EVC_INTF_LEAF(evc_idx,ptin_port))
        {
          /* Compare its outer vlan with the given one */
          if (profile->outer_vlan_in>0 &&
              pclient->out_vlan>0 && pclient->out_vlan<4096)
          {
            if (profile->outer_vlan_in!=pclient->out_vlan)
            {
              LOG_ERR(LOG_CTX_PTIN_EVC,"OVid_in %u does not match to the one in EVC client (%u)",profile->outer_vlan_in,pclient->out_vlan);
              return L7_FAILURE;
            }
            LOG_TRACE(LOG_CTX_PTIN_EVC,"OVid_in %u verified for client %u",ptin_port,profile->outer_vlan_in,profile->inner_vlan_in);
          }
          profile->outer_vlan_out = pclient->out_vlan;
          *bwPolicer_ptr = &(pclient->bwprofile[PTIN_EVC_INTF_LEAF]);
        }
        else
        {
          *bwPolicer_ptr = &(pclient->bwprofile[PTIN_EVC_INTF_ROOT]);
        }
      } /* else (profile->inner_vlan_in==0) */
    } /* if (bwPolicer_ptr!=L7_NULLPTR) */

    /* If svlan is provided, it was already validated... Rewrite it with the internal value */
    profile->outer_vlan_in = evcs[evc_idx].intf[ptin_port].int_vlan;
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Interface (ptin_port=%u): OVid_in  = %u",ptin_port,profile->outer_vlan_in);
  } /* if (profile->ddUsp_src.unit>=0 && profile->ddUsp_src.slot>=0 && profile->ddUsp_src.port>=0) */
  /* If source interface is not provided... */
  else
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Source interface is not speficied");
    return L7_FAILURE;
  } /* else (profile->ddUsp_src.unit>=0 && profile->ddUsp_src.slot>=0 && profile->ddUsp_src.port>=0) */

  /* If destination interface is provided, validate it */
  if (profile->ddUsp_dst.unit>=0 && profile->ddUsp_dst.slot>=0 && profile->ddUsp_dst.port>=0)
  {
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Processing destination interface");
    /* Get USP interface */
    usp.unit = profile->ddUsp_dst.unit;
    usp.slot = profile->ddUsp_dst.slot;
    usp.port = profile->ddUsp_dst.port + 1;

    /* Get interface number */
    if (nimGetIntIfNumFromUSP(&usp,&intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"Error getting interface number from USP={%d,%d,%d}",usp.unit,usp.slot,usp.port);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Destination interface: intIfNum=%u",intIfNum);

    /* Get ptin_port */
    if (ptin_intf_intIfNum2port(intIfNum,&ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"Error getting ptin_port from intIfNum=%u",intIfNum);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Destination interface: ptin_port=%u",ptin_port);

    /* Verify if interface is in use */
    if (!evcs[evc_idx].intf[ptin_port].in_use)
    {
      LOG_WARNING(LOG_CTX_PTIN_EVC,"ptin_port %d is not in use",ptin_port);
      return L7_NOT_EXIST;
    }
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Destination interface is present in EVC");
  } /* if (profile->ddUsp_dst.unit>=0 && profile->ddUsp_dst.slot>=0 && profile->ddUsp_dst.port>=0) */
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Destination interface is not speficied");
  } /* else (profile->ddUsp_dst.unit>=0 && profile->ddUsp_dst.slot>=0 && profile->ddUsp_dst.port>=0) */

  LOG_TRACE(LOG_CTX_PTIN_EVC,"Final bw profile data:");
  LOG_TRACE(LOG_CTX_PTIN_EVC," evcId       = %u",evc_idx);
  LOG_TRACE(LOG_CTX_PTIN_EVC," srcIntf     = {%d,%d,%d}",profile->ddUsp_src.unit,profile->ddUsp_src.slot,profile->ddUsp_src.port);
  LOG_TRACE(LOG_CTX_PTIN_EVC," dstIntf     = {%d,%d,%d}",profile->ddUsp_dst.unit,profile->ddUsp_dst.slot,profile->ddUsp_dst.port);
  LOG_TRACE(LOG_CTX_PTIN_EVC," OVID_in     = %u",profile->outer_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_EVC," OVID_out    = %u",profile->outer_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_EVC," IVID_in     = %u",profile->inner_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_EVC," {CIR,CBS}   = {%lu,%lu}",profile->meter.cir,profile->meter.cbs);
  LOG_TRACE(LOG_CTX_PTIN_EVC," {EIR,EBS}   = {%lu,%lu}",profile->meter.eir,profile->meter.ebs);

  return L7_SUCCESS;
}

/**
 * Verify evcStats profile parameters
 * 
 * @param evc_idx : evc index
 * @param profile : evcStats profile data 
 * @param counters_ptr : Location of evcStats pointer in evc 
 *                database (address of a ptr to a ptr to
 *                evcStats)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_evc_evcStats_verify(L7_uint evc_idx, ptin_evcStats_profile_t *profile, void ***counters_ptr)
{
  nimUSP_t  usp;
  L7_uint32 intIfNum = L7_ALL_INTERFACES;
  L7_int    ptin_port, i_port;
  struct ptin_evc_client_s *pclient;

  LOG_TRACE(LOG_CTX_PTIN_EVC,"Starting evcStats verification");

  /* Profile pointer should have a valid address */
  if (profile==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "Profile is a null pointer");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_EVC,"Initial evcStats profile data:");
  LOG_TRACE(LOG_CTX_PTIN_EVC," evcId     = %u",evc_idx);
  LOG_TRACE(LOG_CTX_PTIN_EVC," ddUsp_src = {%d,%d,%d}",profile->ddUsp_src.unit,profile->ddUsp_src.slot,profile->ddUsp_src.port);
  LOG_TRACE(LOG_CTX_PTIN_EVC," ddUsp_dst = {%d,%d,%d}",profile->ddUsp_dst.unit,profile->ddUsp_dst.slot,profile->ddUsp_dst.port);
  LOG_TRACE(LOG_CTX_PTIN_EVC," OVID_in   = %u",profile->outer_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_EVC," OVID_out  = %u",profile->outer_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_EVC," IVID_in   = %u",profile->inner_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_EVC," Dest_IP   = %u",profile->dst_ip);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_idx >= PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_EVC, "EVC# %u is out of range [0..%u]", evc_idx, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (!evcs[evc_idx].in_use)
  {
    LOG_WARNING(LOG_CTX_PTIN_EVC, "EVC# %u is not in use", evc_idx);
    return L7_NOT_EXIST;
  }

  /* If interface is provided, validate it */
  if (profile->ddUsp_src.unit>=0 && profile->ddUsp_src.slot>=0 && profile->ddUsp_src.port>=0)
  {
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Processing Interface");
    /* Get USP interface */
    usp.unit = profile->ddUsp_src.unit;
    usp.slot = profile->ddUsp_src.slot;
    usp.port = profile->ddUsp_src.port + 1;
    /* Get interface number */
    if (nimGetIntIfNumFromUSP(&usp,&intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"Error getting interface number from USP={%d,%d,%d}",usp.unit,usp.slot,usp.port);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Interface: intIfNum=%u",intIfNum);

    /* Get ptin_port */
    if (ptin_intf_intIfNum2port(intIfNum,&ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_EVC,"Error getting ptin_port from intIfNum=%u",intIfNum);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Interface: ptin_port=%u",ptin_port);

    /* Verify if interface is in use */
    if (!evcs[evc_idx].intf[ptin_port].in_use)
    {
      LOG_WARNING(LOG_CTX_PTIN_EVC,"ptin_port %d is not in use",ptin_port);
      return L7_NOT_EXIST;
    }
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Interface is present in EVC");

    /* Verify Svlan*/
    if (profile->outer_vlan_in>0 &&
        evcs[evc_idx].intf[ptin_port].out_vlan>0 && evcs[evc_idx].intf[ptin_port].out_vlan<4096)
    {
      if (profile->outer_vlan_in!=evcs[evc_idx].intf[ptin_port].out_vlan)
      {
        LOG_ERR(LOG_CTX_PTIN_EVC,"OVid_in %u does not match to the one in EVC (%u)",profile->outer_vlan_in,evcs[evc_idx].intf[ptin_port].out_vlan);
        return L7_FAILURE;
      }
      LOG_TRACE(LOG_CTX_PTIN_EVC,"Interface (ptin_port=%u): OVid_in %u verified",ptin_port,profile->outer_vlan_in);
    }

    /* Default outer and inner vlan at egress:
       - the outer vlan is the defined for each interface
       - the inner vlan is the same as the internal inner vlan
       If interface is leaf and EVC is stacked, the outer and inner vlans at egress should not be considered */
    #if ( !PTIN_BOARD_IS_MATRIX )
    profile->outer_vlan_out = 0;
    profile->inner_vlan_out = 0;
    if (IS_EVC_INTF_ROOT(evc_idx,ptin_port) || IS_EVC_UNSTACKED(evc_idx))
    #endif
    {
      profile->outer_vlan_out = evcs[evc_idx].intf[ptin_port].out_vlan;
      profile->inner_vlan_out = profile->inner_vlan_in;
    }

    /* If bwPolicer_ptr is not null, we should provide a pointer to the location where the bwPolicer address will be stored */
    if (counters_ptr!=L7_NULLPTR)
    {
      /* If inner_vlan is null, use the general policer for the interface */
      if (profile->inner_vlan_in==0)
      {
        *counters_ptr = &(evcs[evc_idx].intf[ptin_port].counter);
      } /* if (profile->inner_vlan_in==0) */
      /* If valid, find the specified client, and provide the policer location */
      else
      {
        /* Find the specified cvlan in all EVC clients */
        for (i_port=0, pclient=L7_NULLPTR; i_port<PTIN_SYSTEM_N_INTERF && pclient==L7_NULLPTR; i_port++)
        {
          if ( IS_EVC_INTF_ROOT(evc_idx,ptin_port) ||
              (IS_EVC_INTF_LEAF(evc_idx,ptin_port) && i_port==ptin_port))
          {
            ptin_evc_find_client(profile->inner_vlan_in, &(evcs[evc_idx].intf[i_port].clients), (dl_queue_elem_t **) &pclient);
          }
        }
        if (pclient==L7_NULLPTR)
        {
          LOG_WARNING(LOG_CTX_PTIN_EVC,"Client %u not found in EVC %u",profile->inner_vlan_in,evc_idx);
          return L7_NOT_EXIST;
        }
        /* If interface is a leaf... */
        if (IS_EVC_INTF_LEAF(evc_idx,ptin_port))
        {
          /* Compare its outer vlan with the given one */
          if (profile->outer_vlan_in>0 &&
              pclient->out_vlan>0 && pclient->out_vlan<4096)
          {
            if (profile->outer_vlan_in!=pclient->out_vlan)
            {
              LOG_ERR(LOG_CTX_PTIN_EVC,"OVid_in %u does not match to the one in EVC client (%u)",profile->outer_vlan_in,pclient->out_vlan);
              return L7_FAILURE;
            }
            LOG_TRACE(LOG_CTX_PTIN_EVC,"OVid_in %u verified for client %u",ptin_port,profile->outer_vlan_in,profile->inner_vlan_in);
          }
          profile->outer_vlan_out = pclient->out_vlan;
          *counters_ptr = &(pclient->counter[PTIN_EVC_INTF_LEAF]);
        }
        else
        {
          *counters_ptr = &(pclient->counter[PTIN_EVC_INTF_ROOT]);
        }
      } /* else (profile->inner_vlan_in==0) */
    } /* if (counters_ptr!=L7_NULLPTR) */

    /* If svlan is provided, it was already validated... Rewrite it with the internal value */
    profile->outer_vlan_in = evcs[evc_idx].intf[ptin_port].int_vlan;
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Interface (ptin_port=%u): OVid_in  = %u",ptin_port,profile->outer_vlan_in);

  } /* if (profile->ddUsp_src.unit>=0 && profile->ddUsp_src.slot>=0 && profile->ddUsp_src.port>=0) */
  /* If interface is not provided... */
  else
  {
    LOG_ERR(LOG_CTX_PTIN_EVC,"Interface is not speficied");
    return L7_FAILURE;
  } /* else (profile->ddUsp_src.unit>=0 && profile->ddUsp_src.slot>=0 && profile->ddUsp_src.port>=0) */

  LOG_TRACE(LOG_CTX_PTIN_EVC,"Final evcStats profile data:");
  LOG_TRACE(LOG_CTX_PTIN_EVC," evcId     = %u",evc_idx);
  LOG_TRACE(LOG_CTX_PTIN_EVC," ddUsp_src = {%d,%d,%d}",profile->ddUsp_src.unit,profile->ddUsp_src.slot,profile->ddUsp_src.port);
  LOG_TRACE(LOG_CTX_PTIN_EVC," ddUsp_dst = {%d,%d,%d}",profile->ddUsp_dst.unit,profile->ddUsp_dst.slot,profile->ddUsp_dst.port);
  LOG_TRACE(LOG_CTX_PTIN_EVC," OVID_in   = %u",profile->outer_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_EVC," OVID_out  = %u",profile->outer_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_EVC," IVID_in   = %u",profile->inner_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_EVC," Dst_IP    = %u",profile->dst_ip);

  return L7_SUCCESS;
}

/* DEBUG Functions ************************************************************/
/**
 * Dumps EVC detailed info 
 * If evc_idx is invalid, all EVCs are dumped 
 * 
 * @param evc_idx 
 */
void ptin_evc_dump(L7_uint evc_idx)
{
  L7_uint start, end;
  L7_uint i, j;
  struct ptin_evc_client_s *pclient;

  /* Dump all ? */
  if (evc_idx >= PTIN_SYSTEM_N_EVCS)
  {
    start = 0;
    end   = PTIN_SYSTEM_N_EVCS - 1;
  }
  else
  {
    start = evc_idx;
    end   = evc_idx;
  }

  for (evc_idx = start; evc_idx <= end; evc_idx++)
  {
    if (!evcs[evc_idx].in_use) {
      if (evc_idx<PTIN_SYSTEM_N_EVCS)
        printf("*** EVC# %02u not in use\n\n", evc_idx);
      continue;
    }

    printf("EVC# %02u\n", evc_idx);

    printf("  Flags     = 0x%08X", evcs[evc_idx].flags);
    if (evcs[evc_idx].flags)
      printf("   ");
    if (evcs[evc_idx].flags & PTIN_EVC_MASK_BUNDLING)
      printf("BUNDLING  ");
    if (evcs[evc_idx].flags & PTIN_EVC_MASK_ALL2ONE)
      printf("ALL2ONE  ");
    if (evcs[evc_idx].flags & PTIN_EVC_MASK_STACKED)
      printf("STACKED  ");
    if (evcs[evc_idx].flags & PTIN_EVC_MASK_MACLEARNING)
      printf("MACLEARNING  ");
    if (evcs[evc_idx].flags & PTIN_EVC_MASK_CPU_TRAPPING)
      printf("CPUTrap  ");
    if (evcs[evc_idx].flags & PTIN_EVC_MASK_DHCP_PROTOCOL)
      printf("DHCPop82  ");
    printf("\n");

    printf("  MC Flood  = %s (%u)\n", evcs[evc_idx].mc_flood == PTIN_EVC_MC_FLOOD_ALL ? "All":evcs[evc_idx].mc_flood == PTIN_EVC_MC_FLOOD_UNKNOWN ? "Unknown":"None",
           evcs[evc_idx].mc_flood);
              
    printf("  Roots     = %2u        Counters = %u\n", evcs[evc_idx].n_roots, evcs[evc_idx].n_counters);
    printf("  Leafs     = %2u        BW Prof. = %u\n", evcs[evc_idx].n_leafs, evcs[evc_idx].n_bwprofiles);

    printf("  Root VLAN = %u\n", evcs[evc_idx].rvlan);

    if (IS_EVC_STACKED(evc_idx))
      printf("  Clients   = %u\n", evcs[evc_idx].n_clients);
              

    for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
    {
      if (!evcs[evc_idx].intf[i].in_use)
        continue;

      if (i<PTIN_SYSTEM_N_PORTS)
        printf("  PHY# %02u\n", i);
      else
        printf("  LAG# %02u\n", i - PTIN_SYSTEM_N_PORTS);

      printf("    MEF Type      = %s\n", evcs[evc_idx].intf[i].type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf");
      printf("    Outer VLAN    = %-5u      Counter  = %s\n", evcs[evc_idx].intf[i].out_vlan, evcs[evc_idx].intf[i].counter != NULL ? "Active":"Disabled");
      printf("    Internal VLAN = %-5u      BW Prof. = %s\n", evcs[evc_idx].intf[i].int_vlan, evcs[evc_idx].intf[i].bwprofile != NULL ? "Active":"Disabled");

      if (IS_EVC_STACKED(evc_idx)) {
        printf("    Clients       = %u\n", evcs[evc_idx].intf[i].clients.n_elems);

        dl_queue_get_head(&evcs[evc_idx].intf[i].clients, (dl_queue_elem_t **) &pclient);
        for (j=0; j<evcs[evc_idx].intf[i].clients.n_elems; j++) {
          printf("      Client# %2u: OVID=%04u IVID=%04u (Counter {%s,%s}; BW Prof. {%s,%s})\n", j, pclient->out_vlan, pclient->inn_vlan,
                 pclient->counter[PTIN_EVC_INTF_ROOT]   != NULL ? "Root ON ":"Root OFF", pclient->counter[PTIN_EVC_INTF_LEAF]   != NULL ? "Leaf ON ":"Leaf OFF",
                 pclient->bwprofile[PTIN_EVC_INTF_ROOT] != NULL ? "Root ON ":"Root OFF", pclient->bwprofile[PTIN_EVC_INTF_LEAF] != NULL ? "Leaf ON ":"Leaf OFF");

          pclient = (struct ptin_evc_client_s *) dl_queue_get_next(&evcs[evc_idx].intf[i].clients, (dl_queue_elem_t *) pclient);
        }
      }
    }

    printf("\n");
  }
}

#if 0
void sizeof_evc(void)
{
  LOG_CRITICAL(LOG_CTX_PTIN_EVC, "sizeof(ptin_evc_client_s) = %d", sizeof(struct ptin_evc_client_s));
  LOG_CRITICAL(LOG_CTX_PTIN_EVC, "sizeof(ptin_evc_intf_s)   = %d", sizeof(struct ptin_evc_intf_s));
  LOG_CRITICAL(LOG_CTX_PTIN_EVC, "sizeof(ptin_evc_s)        = %d", sizeof(struct ptin_evc_s));
  LOG_CRITICAL(LOG_CTX_PTIN_EVC, "sizeof(evcs)              = %d", sizeof(evcs));
  LOG_CRITICAL(LOG_CTX_PTIN_EVC, "sizeof(clients)           = %d", sizeof(clients));
  LOG_CRITICAL(LOG_CTX_PTIN_EVC, "total(evcs+clients)       = %d", sizeof(evcs)+sizeof(clients));
}


void test_bitmaps(void)
{
    #define SIZE    16
    BITMAP(bmp, SIZE);
    int i;

    printf("sizeof(bmp)=%d\n", sizeof(bmp));

    printf("BITMAP_CLEAR_ALL    = %d\n", BITMAP_CLEAR_ALL(bmp));
    printf("BITMAP_IS_ALL_CLEAR = %d\n", BITMAP_IS_ALL_CLEAR(bmp));
    printf("BITMAP_IS_ALL_SET   = %d\n", BITMAP_IS_ALL_SET(bmp));
    printf("BITMAP_IS_SET(8)    = %d\n", BITMAP_IS_SET(bmp, 8));
    printf("BITMAP_SET(8)       = %d\n", BITMAP_SET(bmp, 8));
    printf("BITMAP_IS_SET(8)    = %d\n", BITMAP_IS_SET(bmp, 8));
    printf("BITMAP_IS_CLEAR(8)  = %d\n", BITMAP_IS_CLEAR(bmp, 8));
    printf("BITMAP_SET(7)       = %d\n", BITMAP_SET(bmp, 7));
    printf("BITMAP_IS_SET(7)    = %d\n", BITMAP_IS_SET(bmp, 7));
    printf("BITMAP_IS_CLEAR(7)  = %d\n", BITMAP_IS_CLEAR(bmp, 7));

    printf("setting %d bits...\n", SIZE);
    for (i=0; i<SIZE; i++) {
        BITMAP_SET(bmp, i);
    }
    printf("BITMAP_IS_ALL_SET   = %d\n", BITMAP_IS_ALL_SET(bmp));

    printf("BITMAP_CLEAR_ALL    = %d\n", BITMAP_CLEAR_ALL(bmp));
    printf("BITMAP_IS_ALL_CLEAR = %d\n", BITMAP_IS_ALL_CLEAR(bmp));

    printf("BITMAP_SET_ALL      = %d\n", BITMAP_SET_ALL(bmp));
    printf("BITMAP_IS_ALL_SET   = %d\n", BITMAP_IS_ALL_SET(bmp));
    printf("BITMAP_IS_ALL_CLEAR = %d\n", BITMAP_IS_ALL_CLEAR(bmp));

    printf("clearing %d bits...\n", SIZE);
    for (i=0; i<SIZE; i++) {
        BITMAP_CLEAR(bmp, i);
    }
    printf("BITMAP_IS_ALL_CLEAR = %d\n", BITMAP_IS_ALL_CLEAR(bmp));
    printf("BITMAP_IS_ALL_SET   = %d\n", BITMAP_IS_ALL_SET(bmp));
}
#endif
