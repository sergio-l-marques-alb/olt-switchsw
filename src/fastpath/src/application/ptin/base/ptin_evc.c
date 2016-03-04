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
#include "ptin_pppoe.h"

#include "dtlapi.h"
#include "dot3ad_api.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_filter_api.h"
#include "usmdb_mib_vlan_api.h"

#include "ptin_packet.h"
#include "ptin_hal_erps.h"

#include "dtlinclude.h"
#include "usmdb_nim_api.h"

#include <vlan_port.h>

#define PTIN_FLOOD_VLANS_MAX  8

//#define EVC_COUNTERS_REQUIRE_CLEANUP_BEFORE_REMOVAL   1       /* Used for EVC remotion */
#define EVC_BWPROFILES_REQUIRE_CLEANUP_BEFORE_REMOVAL 1       /* Used for EVC remotion */
#define EVC_CLIENTS_REQUIRE_CLEANUP_BEFORE_REMOVAL    1       /* Used for EVC remotion */


/******************************* 
 * Debug procedures
 *******************************/

L7_BOOL ptin_debug_evc = 0;

void ptin_debug_evc_enable(L7_BOOL enable)
{
  ptin_debug_evc = enable;
}


/* EVC Client entry (ONLY APPLICABLE to Stacked EVCs) */
struct ptin_evc_client_s {
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct ptin_evc_client_s *next;
  struct ptin_evc_client_s *prev;

  L7_BOOL    in_use;        /* When set, indicates that this entry is valid */

  L7_uint16  int_ovid;      /* Internal outer vlan */
  L7_uint16  int_ivid;      /* Inner VLAN (Client VLAN) (ignored on root ports) */

  L7_uint16  uni_ovid;      /* S' -> Translated S-VLAN (depends on S+C) */
  L7_uint16  uni_ivid;      /* Inner S' -> Translated C-VLAN */

  L7_uint16  client_vid;   /* Vlan identifying client (usually is the inner vlan) */

  /* GEM ids which will be flooded the ARP packets */
  L7_uint16  flood_vlan[PTIN_FLOOD_VLANS_MAX];
  L7_int     virtual_gport;
  L7_uint32  vport_id;
  L7_uint32  flags;         /* Client/flow flags */

  /* Counters/Profiles per client on stacked EVCs (S+C) */
  void      *counter[2];    /* Pointer to a counter struct entry (Root + Leaf port) */
  void      *bwprofile[2][L7_COS_INTF_QUEUE_MAX_COUNT];  /* Pointer to a BW profile struct entry (Root + Leaf port) */
};  // sizeof=24

/* EVC Interface entry */
struct ptin_evc_intf_s {
  L7_BOOL    in_use;        /* When set, indicates that this entry is valid */

  L7_uint8   type;          /* 0:root; 1:leaf;*/

#ifdef PTIN_ERPS_EVC
  L7_uint8   portState;     /* 0:PORT_BLOCKING; 1:PORT_FLUSHING;*/
#endif

  L7_uint16  out_vlan;      /* Outer VLAN:
                             *  Root: S-VLAN
                             *  Leaf: S' (ONLY applicable to point-to-multipoint services)
                             *        (on p2multipoint services we allow a S->S'
                             *         xlate per leaf port) */
  L7_uint16  inner_vlan;    /* Inner VLAN */

  L7_uint16  int_vlan;      /* Internal VLAN:
                             *  point-to-point - NOT APPLICABLE
                             *  point-to-multipoint - one internal VLAN per interface */

  L7_int32   l3_intf_id;     /* L3 Interface ID. */ 

  L7_uint32  l2_vlan_port_id; /* L2 LIF */

  /* Counters/Profiles per client on unstacked EVCs (counter per leaf port) */
  void      *counter;       /* Pointer to a counter struct entry */
  void      *bwprofile[L7_COS_INTF_QUEUE_MAX_COUNT];     /* Pointer to a BW profile struct entry */

  /* Clients queue */
  dl_queue_t clients;
  /* Queue of used MC probes */
  dl_queue_t queue_probes;
};  // sizeof=20+12=32


/* EVC entry */
struct ptin_evc_s {
  L7_BOOL    in_use;        /* When set, indicates that this entry is valid */
  L7_uint32  extended_id;   /* Extended ID assigned to this EVC */

  L7_uint32  flags;         /* PTIN_EVC_MASK_xxx:
                             *  PTIN_EVC_MASK_BUNDLING        0x00000001
                             *  PTIN_EVC_MASK_ALL2ONE         0x00000002
                             *  PTIN_EVC_MASK_STACKED         0x00000004
                             *  PTIN_EVC_MASK_MACLEARNING     0x00000008
                             *  PTIN_EVC_MASK_CPU_TRAPPING    0x00000010
                             *  PTIN_EVC_MASK_MC_IPTV         0x00000020
                             *  PTIN_EVC_MASK_IPSG_PROTOCOL   0x00000080
                             *  PTIN_EVC_MASK_DHCPV4_PROTOCOL 0x00000100
                             *  PTIN_EVC_MASK_IGMP_PROTOCOL   0x00000200
                             *  PTIN_EVC_MASK_PPPOE_PROTOCOL  0x00000400
                             *  PTIN_EVC_MASK_DHCPV6_PROTOCOL 0x00001000
                             *  PTIN_EVC_MASK_MLD_PROTOCOL    0x00002000

                             *  PTIN_EVC_MASK_P2P             0x00010000
                             *  PTIN_EVC_MASK_QUATTRO         0x00020000 */

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
  L7_uint32  n_clientflows;       /* Number of clients associated with this EVC (ONLY on stacked services) */

  L7_uint32  n_clientflows_ipsg;
  L7_uint32  n_clientflows_igmp;
  L7_uint32  n_clientflows_dhcpv4;
  L7_uint32  n_clientflows_dhcpv6;
  L7_uint32  n_clientflows_pppoe;
  L7_uint32  n_clientflows_mld;

  L7_uint32  n_counters;      /* Number of counters associated with this EVC */

  L7_uint32  n_bwprofiles;  /* Number of BW profiles associated with this EVC */
  L7_uint32  n_probes;      /* Number of probes associated with this EVC */

  L7_uint8   n_roots;       /* Number of roots */
  L7_uint8   n_leafs;       /* Number of leafs */

  /* Information of first root port */
  struct
  {
    L7_uint8   port;        /* Index of first root port */
    L7_uint16  nni_ovid;    /* NNI outer vlan of first root port */
    L7_uint16  nni_ivid;    /* NNI inner vlan of first root port */
  } root_info;

  L7_int     p2p_port1_intf;  /* For P2P services: First port */
  L7_int     p2p_port2_intf;  /* For P2P services: Second port */

  L7_uint16  rvlan;         /* Root VLAN (internal VLAN) 
                             *   p2multipoint : Internal VLAN shared among all interfaces
                             *          to allow downstream flow (flood)
                             *   p2p:   Unique internal VLAN among all interfaces
                             *          Cross-connects are used to forward P2P
                             *          traffic on downstream and upstream
                             */
  //struct ptin_evc_client_s *client_ref[4096];   /* Direct reference to clients information */

  L7_int multicast_group;   /* Multicast group associated to thi service */

  #if PTIN_IGMP_STATS_IN_EVCS
  /* IGMP statistics */
  ptin_IGMP_Statistics_t stats_igmp_intf[PTIN_SYSTEM_N_INTERF];  /* IGMP statistics at interface level */
  #endif

  dl_queue_t *queue_free_vlans;   /* Pointer to queue of free vlans */
};  // sizeof=36+32*36=1188

/* Probes statistics (for channel measurements) */
struct ptin_probe_s {
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct ptin_probe_s *next;
  struct ptin_probe_s *prev;

  L7_BOOL   in_use;       /* Is entry in use? */

  L7_uint32 channel_ip;   /* Channel IP (only for IPv4) */

  void *counter;
};

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

/* EVC entry (for pool queue) */
struct ptin_evc_entry_s {
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct ptin_evc_entry_s *next;
  struct ptin_evc_entry_s *prev;

  L7_uint evc_id;  /* One index of evcs[] array */
};

/* Static array structures: EVCs + Clients + VLANs */
static struct ptin_evc_s        evcs[PTIN_SYSTEM_N_EVCS];       // sizeof=65*1188=77220
static struct ptin_evc_entry_s  evcs_pool[PTIN_SYSTEM_N_EVCS];  /* Array with all the indexes of EVCs to be used in a queue */

static struct ptin_evc_client_s clients[PTIN_SYSTEM_N_CLIENTS]; // sizeof=24*1024=24576

/* Allocated space for all probes */
static struct ptin_probe_s      probes[PTIN_SYSTEM_MAX_COUNTERS];

/* Queues */
static dl_queue_t queue_busy_evcs;    /* Queue of busy EVCs entries */
static dl_queue_t queue_free_evcs;    /* Queue of free EVCs entries */
static dl_queue_t queue_free_clients; /* Clients (busy) queues are mapped on each interface per EVC */
static dl_queue_t queue_free_probes;  /* Queue of free MC probes */

#if (1)   /* EVCid extended feature */
/* Vlan Queues */
static struct ptin_vlan_s       vlans_pool[1<<16];              /* 4096 VLANs */

typedef enum
{
#if (PTIN_BOARD_IS_DNX)
  PTIN_VLAN_TYPE_VSI=0,
  PTIN_VLAN_TYPE_VLAN,
#else
  PTIN_VLAN_TYPE_BITSTREAM=0,
  PTIN_VLAN_TYPE_CPU,
  PTIN_VLAN_TYPE_CPU_BCAST,
  PTIN_VLAN_TYPE_CPU_MCAST,
  PTIN_VLAN_TYPE_QUATTRO,
#endif
  PTIN_VLAN_TYPE_MAX         /* Do not change this constant */
} ptin_evc_type_enum_t;

static dl_queue_t queue_free_vlans[PTIN_VLAN_TYPE_MAX];
#endif

/* List with all the ports/lags used by EVCs */
static L7_uint8 evcs_intfs_in_use[PTIN_SYSTEM_N_INTERF];

/* Reference of evcid using internal vlan as reference */
static L7_uint32 evcId_from_internalVlan[1<<16];

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
/* Keep track of number of QUATTRO P2P evcs */
static L7_uint16 n_quattro_evcs = 0;
static L7_uint16 n_quattro_igmp_evcs = 0;

#define INCREMENT_QUATTRO_INSTANCE(evcId, counter)   { if (IS_EVC_QUATTRO(evcId) && IS_EVC_STACKED(evc_id))  (counter)++; }
#define DECREMENT_QUATTRO_INSTANCE(evcId, counter)   { if (IS_EVC_QUATTRO(evcId) && IS_EVC_STACKED(evc_id) && (counter)>0)  (counter)--; }

#define NO_INSTANCE(evcId, counter)       (!IS_EVC_QUATTRO(evcId) || ((counter) == 0))
#define SINGLE_INSTANCE(evcId, counter)   (!IS_EVC_QUATTRO(evcId) || ((counter) <= 1))
#endif

/* Local Macros */
#define IS_eEVC_IN_USE(a)             (ptin_evc_ext2int((a), L7_NULLPTR) == L7_SUCCESS)

#define IS_EVC_P2P(evc_id)            ((evcs[evc_id].flags & PTIN_EVC_MASK_P2P    ) == PTIN_EVC_MASK_P2P )
#define IS_EVC_P2MP(evc_id)           ((evcs[evc_id].flags & PTIN_EVC_MASK_P2P    ) == 0 )
#define IS_EVC_QUATTRO(evc_id)        ((evcs[evc_id].flags & PTIN_EVC_MASK_QUATTRO) == PTIN_EVC_MASK_QUATTRO )
#define IS_EVC_STD(evc_id)            ((evcs[evc_id].flags & PTIN_EVC_MASK_QUATTRO) == 0 )

#define IS_EVC_STD_P2P(evc_id)        (IS_EVC_STD(evc_id) && IS_EVC_P2P(evc_id))
#define IS_EVC_STD_P2MP(evc_id)       (IS_EVC_STD(evc_id) && IS_EVC_P2MP(evc_id))

#define IS_EVC_IPTV(evc_id)           ((evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV ) == PTIN_EVC_MASK_MC_IPTV)

#define IS_EVC_STACKED(evc_id)        ((evcs[evc_id].flags & PTIN_EVC_MASK_STACKED ) == PTIN_EVC_MASK_STACKED)
#define IS_EVC_UNSTACKED(evc_id)      ((evcs[evc_id].flags & PTIN_EVC_MASK_STACKED ) == 0 )

#define IS_EVC_WITH_CPU_TRAP(evc_id)  ((evcs[evc_id].flags & PTIN_EVC_MASK_CPU_TRAPPING) == PTIN_EVC_MASK_CPU_TRAPPING)

#define IS_EVC_INTF_USED(evc_id,ptin_port) (evcs[evc_id].intf[ptin_port].in_use)
#define IS_EVC_INTF_ROOT(evc_id,ptin_port) (IS_EVC_INTF_USED(evc_id,ptin_port) && evcs[evc_id].intf[ptin_port].type==PTIN_EVC_INTF_ROOT)
#define IS_EVC_INTF_LEAF(evc_id,ptin_port) (IS_EVC_INTF_USED(evc_id,ptin_port) && evcs[evc_id].intf[ptin_port].type==PTIN_EVC_INTF_LEAF)

#define IS_VLAN_VALID(vlanId)         ((vlanId) > 0 && (vlanId) < 4096)

/**********************************************************
 * AVL TREE with Ext EvcId translation into internal EvcId
 **********************************************************/

/* Extended EvcId key */
typedef struct
{
  L7_uint32 ext_evcId;
} ptinExtEvcIdDataKey_t;

/* Extended EvcId Tree node */
typedef struct ptinExtEvcIdInfoData_s
{
  ptinExtEvcIdDataKey_t extEvcIdDataKey;
  L7_uint16             evc_id;           /* Internal evc id */
  L7_uint8              igmp_inst;        /* IGMP instance */
  L7_uint8              dhcp_inst;        /* DHCP instance */
  L7_uint8              pppoe_inst;       /* PPPoE instance */
  void *next;
} ptinExtEvcIdInfoData_t;

typedef struct {
  avlTree_t                extEvcIdAvlTree;
  avlTreeTables_t         *extEvcIdTreeHeap;
  ptinExtEvcIdInfoData_t  *extEvcIdDataHeap;
} ptinExtEvcIdAvlTree_t;

ptinExtEvcIdAvlTree_t extEvcId_avlTree;

/**********************************************************
 * Internal functions
 **********************************************************/

/* DriveShell functions */
L7_RC_t ptin_evc_allclientsflows_remove( L7_uint evc_id );                             /* Used by ptin_evc_delete */
L7_RC_t ptin_evc_intfclientsflows_remove( L7_uint evc_id, L7_uint32 ptin_port);        /* Used by ptin_evc_destroy */
L7_RC_t ptin_evc_client_remove( L7_uint evc_id, L7_uint32 intf_idx, L7_uint cvlan );

void ptin_evc_clean_counters_enable( L7_BOOL enable );
void ptin_evc_clean_profiles_enable( L7_BOOL enable );

L7_RC_t ptin_evc_clean_all( L7_uint evc_id, L7_BOOL force );                                             /* Used by ptin_evc_delete */
L7_RC_t ptin_evc_intf_clean_all( L7_uint evc_id, L7_uint32 intf_idx, L7_BOOL force );  /* Used by ptin_evc_destroy */

L7_RC_t ptin_evc_allintfs_clean( L7_uint evc_id, L7_BOOL force );                                        /* Used by ptin_evc_delete */
L7_RC_t ptin_evc_intf_clean( L7_uint evc_id, L7_uint32 intf_idx, L7_BOOL force );      /* Used by ptin_evc_destroy */

L7_RC_t ptin_evc_allclients_clean( L7_uint evc_id, L7_BOOL force );
L7_RC_t ptin_evc_intfclients_clean( L7_uint evc_id, L7_uint32 intf_idx, L7_BOOL force );
L7_RC_t ptin_evc_client_clean( L7_uint evc_id, L7_uint32 intf_idx, L7_uint cvlan, L7_BOOL force );

#if (PTIN_BOARD_IS_DNX || PTIN_QUATTRO_FLOWS_FEATURE_ENABLED)
static L7_RC_t ptin_evc_flow_config(ptin_HwEthEvcFlow_t *evcFlow);
static L7_RC_t ptin_evc_flow_unconfig(ptin_HwEthEvcFlow_t *evcFlow);
#endif /* PTIN_BOARD_IS_DNX || PTIN_QUATTRO_FLOWS_FEATURE_ENABLED */

/* Local functions prototypes */
static L7_RC_t ptin_evc_pclientFlow_clean( L7_uint evc_id, L7_uint ptin_port, struct ptin_evc_client_s *pclientFlow, L7_BOOL force );

static void    ptin_evc_entry_init(L7_uint evc_id);
static L7_RC_t ptin_evc_entry_allocate(L7_uint32 evc_ext_id, L7_uint *evc_id);
static L7_RC_t ptin_evc_entry_free(L7_uint32 evc_ext_id);

//static L7_RC_t ptin_evc_ext2int(L7_uint32 evc_ext_id, L7_uint32 *evc_id);
static L7_RC_t ptin_evc_extEvcInfo_get(L7_uint32 evc_ext_id, ptinExtEvcIdInfoData_t **infoData);

static void    ptin_evc_vlan_pool_init(void);
#if (1)   /* EVCid extended feature */
static L7_RC_t ptin_evc_freeVlanQueue_allocate(L7_uint16 evc_id, L7_uint32 evc_flags, dl_queue_t **freeVlan_queue);
static L7_RC_t ptin_evc_freeVlanQueue_free(dl_queue_t *freeVlan_queue);
static L7_RC_t ptin_evc_vlan_allocate(L7_uint16 *vlan, dl_queue_t *queue_vlans, L7_uint16 evc_id);
static L7_RC_t ptin_evc_vlan_free(L7_uint16 vlan, dl_queue_t *queue_vlans);
#else
static L7_RC_t ptin_evc_matrix_vlan_allocate(L7_uint16 *vlan, L7_uint16 ext_evc_id, L7_uint32 evc_flags);
static L7_RC_t ptin_evc_matrix_vlan_free(L7_uint16 vlan);
#endif

static L7_RC_t ptin_evc_intf_add(L7_uint evc_id, ptin_HwEthMef10Intf_t *intf_cfg);
static L7_RC_t ptin_evc_intf_remove(L7_uint evc_id, ptin_HwEthMef10Intf_t *intf_cfg);
static L7_RC_t ptin_evc_intf_remove_all(L7_uint evc_id);

static void    ptin_evc_find_client(L7_uint16 inn_vlan, dl_queue_t *queue, dl_queue_elem_t **pelem);
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
static void ptin_evc_find_flow(L7_uint16 uni_ovid, dl_queue_t *queue, dl_queue_elem_t **pelem);
static void ptin_evc_find_flow_fromVPort(L7_uint32 vport_id, dl_queue_t *queue, dl_queue_elem_t **pelem);
#endif

static L7_RC_t switching_intf_add(L7_uint ptin_port, L7_uint16 vlanId);
static L7_RC_t switching_intf_remove(L7_uint ptin_port, L7_uint16 vlanId);

#if (PTIN_BOARD_IS_DNX)
static L7_RC_t switching_lif_add(L7_uint ptin_port, L7_uint16 out_vlan, L7_uint16 inner_vlan, L7_int8 pcp, L7_uint16 etherType,
                                 L7_uint16 vsi, L7_uint32 mcgroup, L7_uint32 *vlan_port_id);
static L7_RC_t switching_lif_remove(L7_uint ptin_port, L7_uint16 vsi, L7_uint32 mcgroup, L7_uint32 vlan_port_id);
#else
static L7_RC_t switching_root_add(L7_uint root_intf, L7_uint16 out_vlan, L7_uint16 inner_vlan, L7_uint16 int_vlan, L7_uint16 new_innerVlan,
                                  L7_BOOL egress_del_ivlan, L7_int force_pcp);
static L7_RC_t switching_root_remove(L7_uint root_intf, L7_uint16 out_vlan, L7_uint16 inner_vlan, L7_uint16 int_vlan);
static L7_RC_t switching_leaf_add(L7_uint leaf_intf, L7_uint16 leaf_int_vlan);
static L7_RC_t switching_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_int_vlan);

static L7_RC_t switching_elan_leaf_add(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan, L7_BOOL egress_del_ivid, L7_int force_pcp);
static L7_RC_t switching_elan_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan);

/* Leaf add/remove for MC evcs (active IPTV flag) */
#if ( !PTIN_BOARD_IS_MATRIX )
static L7_RC_t switching_mcevc_leaf_add(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan);
static L7_RC_t switching_mcevc_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan);
#endif

static L7_RC_t switching_p2p_bridge_add(L7_uint root_intf, L7_uint16 root_int_vid, L7_uint leaf_intf, L7_uint16 leaf_out_vid, L7_uint16 leaf_inn_vid);
static L7_RC_t switching_p2p_bridge_remove(L7_uint root_intf, L7_uint16 root_int_vid, L7_uint leaf_intf, L7_uint16 leaf_out_vid, L7_uint16 leaf_inn_vid);
#endif

static L7_RC_t switching_vlan_create(L7_uint16 vid);
static L7_RC_t switching_vlan_delete(L7_uint16 vid);
static L7_RC_t switching_vlan_config(L7_uint16 vid, L7_uint16 fwd_vid, L7_BOOL mac_learning, L7_uint8 mc_flood, L7_uint8 cpu_trap);

static L7_RC_t ptin_evc_param_verify(ptin_HwEthMef10Evc_t *evcConf);
static L7_RC_t ptin_evc_bwProfile_verify(L7_uint evc_id, ptin_bw_profile_t *profile);
static L7_RC_t ptin_evc_evcStats_verify(L7_uint evc_id, ptin_evcStats_profile_t *profile);

static L7_RC_t ptin_evc_probe_get(L7_uint evc_id, ptin_evcStats_profile_t *profile, ptin_evcStats_counters_t *stats);
static L7_RC_t ptin_evc_probe_add(L7_uint evc_id, ptin_evcStats_profile_t *profile);
static L7_RC_t ptin_evc_probe_delete(L7_uint evc_id, ptin_evcStats_profile_t *profile);


L7_RC_t ptin_evc_update_dhcp (L7_uint16 evc_id, L7_uint32 *flags_ref, L7_BOOL dhcpv4_enabled, L7_BOOL dhcpv6_enabled,
                              L7_BOOL just_remove, L7_BOOL look_to_counters);
L7_RC_t ptin_evc_update_pppoe(L7_uint16 evc_id, L7_uint32 *flags_ref, L7_BOOL pppoe_enabled,
                              L7_BOOL just_remove, L7_BOOL look_to_counters);
L7_RC_t ptin_evc_update_igmp (L7_uint16 evc_id, L7_uint32 *flags_ref, L7_BOOL igmp_enabled,
                              L7_BOOL just_remove, L7_BOOL look_to_counters);

/* Semaphore to access EVC clients */
void *ptin_evc_clients_sem = L7_NULLPTR;


/* EVC manipulation functions *************************************************/
/**
 * Initializes EVCs data structures
 * 
 * @return L7_RC_t L7_SUCCESS
 */
L7_RC_t ptin_evc_init(void)
{
  L7_uint i;

  PT_LOG_INFO(LOG_CTX_EVC, "EVC init started...");

  /* Clear EVCs DB */
  memset(evcs, 0x00, sizeof(evcs));

  /* Initialize free clients queues */
  memset(clients, 0x00, sizeof(clients));
  dl_queue_init(&queue_free_clients);
  for (i=0; i<(sizeof(clients)/sizeof(clients[0])); i++)
    dl_queue_add(&queue_free_clients, (dl_queue_elem_t*)&clients[i]);

  /* Initialize free probes queue */
  memset(probes,0x00,sizeof(probes));
  dl_queue_init(&queue_free_probes);
  for (i=0; i<PTIN_SYSTEM_MAX_COUNTERS; i++)
    dl_queue_add(&queue_free_probes, (dl_queue_elem_t*)&probes[i]);

  /* Reset EVCs structs */
  for (i=0; i<(sizeof(evcs)/sizeof(evcs[0])); i++)
    ptin_evc_entry_init(i);

  /* Initialize EVCs busy queues (empty) */
  dl_queue_init(&queue_busy_evcs);

  /* Initialize EVCs free queue and update evc indexes of the pool elements */
  dl_queue_init(&queue_free_evcs);
  for (i=0; i<PTIN_SYSTEM_N_EVCS; i++) {
    evcs_pool[i].evc_id = i;
    dl_queue_add(&queue_free_evcs, (dl_queue_elem_t*) &evcs_pool[i]);
  }

  /* Reset EVCs ports/lags in use */
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
    evcs_intfs_in_use[i] = 0;

  ptin_evc_vlan_pool_init();

  /* Extended EVCids AVL tree */
  memset(&extEvcId_avlTree, 0x00, sizeof(ptinExtEvcIdAvlTree_t));

  extEvcId_avlTree.extEvcIdTreeHeap = (avlTreeTables_t *) osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_N_EVCS*sizeof(avlTreeTables_t));
  extEvcId_avlTree.extEvcIdDataHeap = (ptinExtEvcIdInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_N_EVCS*sizeof(ptinExtEvcIdInfoData_t));

  if ((extEvcId_avlTree.extEvcIdTreeHeap == L7_NULLPTR) ||
      (extEvcId_avlTree.extEvcIdDataHeap == L7_NULLPTR))
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Error allocating data for ExtEvcID AVL Tree\n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset (&extEvcId_avlTree.extEvcIdAvlTree, 0x00, sizeof(avlTree_t));
  memset ( extEvcId_avlTree.extEvcIdTreeHeap, 0x00, sizeof(avlTreeTables_t)*PTIN_SYSTEM_N_EVCS);
  memset ( extEvcId_avlTree.extEvcIdDataHeap, 0x00, sizeof(ptinExtEvcIdInfoData_t)*PTIN_SYSTEM_N_EVCS);

  // AVL Tree creations - snoopIpAvlTree
  avlCreateAvlTree(&(extEvcId_avlTree.extEvcIdAvlTree),
                   extEvcId_avlTree.extEvcIdTreeHeap,
                   extEvcId_avlTree.extEvcIdDataHeap,
                   PTIN_SYSTEM_N_EVCS,
                   sizeof(ptinExtEvcIdInfoData_t),
                   0x10,
                   sizeof(ptinExtEvcIdDataKey_t));

  /* Create semaphores */
  ptin_evc_clients_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_evc_clients_sem == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Failed to create ptin_evc_clients_sem semaphore!");
    return L7_FAILURE;
  }

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  intf_vp_DB(0, NULL);
#endif
  PT_LOG_INFO(LOG_CTX_EVC, "EVC init OK");

  return L7_SUCCESS;
}


/**
 * Initializes EVCs for each platform
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_startup(void)
{
#if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
  L7_int  i;
  L7_RC_t rc;
  ptin_HwEthMef10Evc_t evcConf;

  /* Create a new EVC */
  memset(&evcConf, 0x00, sizeof(evcConf));
  evcConf.index             = PTIN_EVC_BL2CPU;
  evcConf.flags             = PTIN_EVC_MASK_MACLEARNING;
  evcConf.mc_flood          = PTIN_EVC_MC_FLOOD_ALL;
  evcConf.n_intf            = 5;
  /* Root port */
  evcConf.intf[0].intf_id   = 16;
  evcConf.intf[0].intf_type = 0;
  evcConf.intf[0].mef_type  = PTIN_EVC_INTF_ROOT;
  evcConf.intf[0].vid       = PTIN_VLAN_BL2CPU_EXT;
  /* Leaf ports */
  for (i=1; i<5; i++)
  {
    evcConf.intf[i].intf_id   = (i-1)*2;
    evcConf.intf[i].intf_type = 0;
    evcConf.intf[i].mef_type  = PTIN_EVC_INTF_LEAF;
    evcConf.intf[i].vid       = PTIN_VLAN_BL2CPU_EXT;
  }

  /* Creates EVC for Broadlights management */
  rc = ptin_evc_create(&evcConf);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error creating EVC# %u for Broadlight management purposes", PTIN_EVC_BL2CPU);
    return rc;
  }
#endif

  L7_uint32 intIfNum_vport;

  /* Create intIfNum for Virtual ports */
  if (L7_SUCCESS != vlan_port_intIfNum_create(1, &intIfNum_vport))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error creating intIfNum for virtual ports");
    return L7_FAILURE;
  }
  PT_LOG_NOTICE(LOG_CTX_EVC, "Success creating intIfNum for virtual ports: %u", intIfNum_vport);

  return L7_SUCCESS;
}


/**
 * Determines if a particular EVC is in use
 * 
 * @param evc_ext_id : EVC extended id
 * 
 * @return L7_BOOL L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_evc_is_in_use(L7_uint32 evc_ext_id)
{
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
    return L7_FALSE;

  return IS_eEVC_IN_USE(evc_ext_id);
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
  L7_int evc_id;

  if (intf_idx>=PTIN_SYSTEM_N_INTERF)
    return L7_FALSE;

  /* Run all EVCs */
  for ( evc_id=0; evc_id<PTIN_SYSTEM_N_EVCS; evc_id++ )
  {
    /* Skip not used EVCs */
    if (!evcs[evc_id].in_use)
      continue;

    /* If any EVC is using this interface, return TRUE immediately */
    if (evcs[evc_id].intf[intf_idx].in_use)
    {
      PT_LOG_INFO(LOG_CTX_INTF, "EVC# %u: port %u is already in use", evc_id, intf_idx);
      return L7_TRUE;
    }
  }

  /* At this point no EVC with this interface was found */
  return L7_FALSE;
}

/**
 * Get interface configuration within an EVC
 *  
 * @param evc_ext_id : EVC extended id
 * @param ptin_intf  : PTin interface 
 * @param cfg        : interface configuration 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intfCfg_get(L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, ptin_evc_intfCfg_t *cfg)
{
  L7_uint32 intf_idx;
  L7_uint32 evc_id;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2port(ptin_intf,&intf_idx)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "ptin_intf is invalid");
    return L7_FAILURE;
  }

  /* Extract configuration */
  if (cfg!=L7_NULLPTR)
  {
    cfg->in_use     = evcs[evc_id].intf[intf_idx].in_use;
    cfg->type       = evcs[evc_id].intf[intf_idx].type;
    cfg->out_vlan   = evcs[evc_id].intf[intf_idx].out_vlan;
    cfg->inner_vlan = evcs[evc_id].intf[intf_idx].inner_vlan;
    cfg->int_vlan   = evcs[evc_id].intf[intf_idx].int_vlan;
  }

  return L7_SUCCESS;
}

/**
 * Gets an EVC configuration
 * 
 * @param evcConf Pointer to the output struct (index field is used as input param) 
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_get(ptin_HwEthMef10Evc_t *evcConf)
{
  L7_uint   evc_id;
  L7_uint32 evc_ext_id;
  L7_uint i;

  evc_ext_id = evcConf->index;

  //PT_LOG_INFO(LOG_CTX_EVC, "Reading EVC# %u...", evc_id);

  /* Validate EVC# extended range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* Copy data to the output struct */
  evcConf->flags    = evcs[evc_id].flags;
  evcConf->type     = evcs[evc_id].type;
  evcConf->mc_flood = evcs[evc_id].mc_flood;
  ptin_evc_check_evctype(evc_ext_id, &evcConf->evc_type);
  memset(evcConf->ce_vid_bmp, 0x00, sizeof(evcConf->ce_vid_bmp));

  /* Return number of attached clients */
  evcConf->n_clientflows = evcs[evc_id].n_clientflows;

  evcConf->n_intf = 0;
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
  {
    if (evcs[evc_id].intf[i].in_use)
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
      evcConf->intf[evcConf->n_intf].mef_type   = evcs[evc_id].intf[i].type;
      evcConf->intf[evcConf->n_intf].vid        = evcs[evc_id].intf[i].out_vlan;
      evcConf->intf[evcConf->n_intf].vid_inner  = evcs[evc_id].intf[i].inner_vlan;
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
 * @param int_vlan : Internal vlan
 * @param evcConf      : Pointer to the evc configuration struct
 * 
 * @return L7_RC_t : L7_SUCCESS   - Success extracting evc data
 *                   L7_FAILURE   - Failure reading evc data
 */
L7_RC_t ptin_evc_get_fromIntVlan(L7_uint16 int_vlan, ptin_HwEthMef10Evc_t *evcConf)
{
  L7_uint evc_id;
  L7_RC_t rc = L7_SUCCESS;

#if 0
  /* Validate arguments */
  if (int_vlan >= 4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (int_vlan=%u)",int_vlan);
    return L7_FAILURE;
  }
#endif

  /* Get evc id */
  evc_id = evcId_from_internalVlan[int_vlan];

  /* Check if this internal vlan is in use by any evc */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    return L7_FAILURE;
  }

  /* Check if this evc is in use... if not we have a non-consistent situation */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u (int_vlan=%u) should be in use",evc_id,int_vlan);
    return L7_FAILURE;
  }

  /* Get evc data */
  if (evcConf!=L7_NULLPTR)
  {
    evcConf->index = evcs[evc_id].extended_id;
    rc = ptin_evc_get(evcConf);
    if (rc!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Error getting evc data from int_vlan=%u, evcId=%u",int_vlan,evc_id);
    }
  }

  return rc;
}

/**
 * Gets an EVC configuration from an internal vlan as input 
 * parameter 
 *  
 * @param int_vlan : Internal vlan
 * @param evc_ext_id   : EVC extended id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_get_evcIdfromIntVlan(L7_uint16 int_vlan, L7_uint32 *evc_ext_id)
{
  L7_uint evc_id;

#if 0
  /* Validate arguments */
  if (int_vlan >= 4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (int_vlan=%u)",int_vlan);
    return L7_FAILURE;
  }
#endif

  /* Get evc id */
  evc_id = evcId_from_internalVlan[int_vlan];

  /* Check if this internal vlan is in use by any evc */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    return L7_FAILURE;
  }

  /* Check if this evc is in use... if not we have a non-consistent situation */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u (int_vlan=%u) should be in use",evc_id,int_vlan);
    return L7_FAILURE;
  }

  if (evc_ext_id != L7_NULLPTR)
    *evc_ext_id = evcs[evc_id].extended_id;

  return L7_SUCCESS;
}


/**
 * Gets an EVC configuration from an internal vlan as input 
 * parameter 
 *  
 * @param int_vlan : Internal vlan
 * @param evc_ext_id   : EVC extended id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_get_internal_evcIdfromIntVlan(L7_uint16 int_vlan, L7_uint32 *evc_id)
{
#if 0
  /* Validate arguments */
  if (int_vlan >= 4096 || evc_id == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (int_vlan=%u evc_id=%p)",int_vlan, evc_id);
    return L7_FAILURE;
  }
#endif

  /* Get evc id */
  *evc_id = evcId_from_internalVlan[int_vlan];

  /* Check if this internal vlan is in use by any evc */
  if (*evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get EVC ext id, from NNI vlan
 * 
 * @param nni_ovlan  : NNI OVLAN
 * @param evc_ext_id : EVC extended id list 
 * @param number_of_evcs : Maximum (in) and returned (out) 
 *                         number of EVCs
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_get_evcId_fromNNIvlan(L7_uint16 nni_ovid, L7_uint32 *evc_ext_id, L7_uint *number_of_evcs)
{
  L7_uint evc_id;
  L7_uint evc_count, max_count;

  /* Validate arguments */
  if (nni_ovid == 0 || nni_ovid >= 4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (nni_ovid=%u)",nni_ovid);
    return L7_FAILURE;
  }

  /* Define maximum number of EVCs to search for */
  if (number_of_evcs == L7_NULLPTR)
  {
    max_count = 1;
  }
  else if (*number_of_evcs > PTIN_SYSTEM_N_EVCS)
  {
    max_count = PTIN_SYSTEM_N_EVCS;
  }
  else
  {
    max_count = *number_of_evcs;
  }

  /* Run all EVCs */
  for (evc_id = 0, evc_count = 0; evc_id < PTIN_SYSTEM_N_EVCS && evc_count < max_count; evc_id++)
  {
    /* Skip not used EVCs */
    if (!evcs[evc_id].in_use)
      continue;

    /* check for NNI VLAN */
    if (evcs[evc_id].root_info.nni_ovid == nni_ovid)
    {
      if (evc_ext_id != L7_NULLPTR)
        evc_ext_id[evc_count] = evcs[evc_id].extended_id;
      evc_count++;
    }
  }

  /* Return number of EVCs returned */
  if (number_of_evcs != L7_NULLPTR)
  {
    *number_of_evcs = evc_count;
  }

  /* Check if no EVCs were found */
  if (evc_count == 0)
  {
    return L7_NOT_EXIST; 
  }

  return L7_SUCCESS;
}

/**
 * Get (internal) VLAN list, from NNI vlan
 * 
 * @param nni_ovlan  : NNI OVLAN
 * @param intVid     : VLAN id list 
 * @param number_of_evcs : Maximum (in) and returned (out) 
 *                         number of VLANs
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_get_intVlan_fromNNIvlan(L7_uint16 nni_ovid, L7_uint16 *intVid, L7_uint *number_of_vlans)
{
  L7_uint evc_id;
  L7_uint vlan_count, max_count;

  /* Validate arguments */
  if (nni_ovid == 0 || nni_ovid >= 4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (nni_ovid=%u)",nni_ovid);
    return L7_FAILURE;
  }

  /* Define maximum number of EVCs to search for */
  if (number_of_vlans == L7_NULLPTR)
  {
    max_count = 1;
  }
  else if (*number_of_vlans > 4096)
  {
    max_count = 4096;
  }
  else
  {
    max_count = *number_of_vlans;
  }

  /* Run all EVCs */
  for (evc_id = 0, vlan_count = 0; evc_id < PTIN_SYSTEM_N_EVCS && vlan_count < max_count; evc_id++)
  {
    /* Skip not used EVCs */
    if (!evcs[evc_id].in_use)
      continue;

    /* check for NNI VLAN */
    if (evcs[evc_id].root_info.nni_ovid == nni_ovid)
    {
      if (intVid != L7_NULLPTR)
        intVid[vlan_count] = evcs[evc_id].rvlan;
      vlan_count++;
    }
  }

  /* Return number of EVCs returned */
  if (number_of_vlans != L7_NULLPTR)
  {
    *number_of_vlans = vlan_count;
  }

  /* Check if no EVCs were found */
  if (vlan_count == 0)
  {
    return L7_NOT_EXIST; 
  }

  return L7_SUCCESS;
}

/**
 * Get NNI VLAN from EVC ext id
 * 
 * @param evc_ext_id : EVC extended id 
 * @param nni_ovid   : NNI OVLAN (output)
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_get_NNIvlan_fromEvcId(L7_uint32 evc_ext_id, L7_uint16 *nni_ovid)
{
  L7_uint32 evc_id;

  /* Validate arguments */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (eEVC=%u)", evc_ext_id);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS || evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_FAILURE;
  }
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC %u is not in use", evc_ext_id, evc_id);
    return L7_FAILURE;
  }

  /* Validate VLANs */
  if (evcs[evc_id].root_info.nni_ovid == 0 || evcs[evc_id].root_info.nni_ovid >= 4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC#%u: No valid NNI vlan to return (%u)", evc_ext_id, evcs[evc_id].root_info.nni_ovid);
    return L7_FAILURE;
  }

  /* Return NNI VLAN */
  if (nni_ovid != L7_NULLPTR)
  {
    *nni_ovid = evcs[evc_id].root_info.nni_ovid;
  }

  return L7_SUCCESS;
}

/**
 * Gets the internal vlan for a particular evc and interface
 * 
 * @param evc_ext_id : EVC extended id 
 * @param ptin_intf  : interface
 * @param int_vlan    : Internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intVlan_get(L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, L7_uint16 *internal_vlan)
{
  L7_uint32 ptin_port;
  L7_uint16 int_vlan;
  L7_uint32 evc_id;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid argument");
    return L7_FAILURE;
  }

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* Obtain ptin_port */
  if (ptin_intf_ptintf2port(ptin_intf, &ptin_port)!=L7_SUCCESS || ptin_port>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Cannot convert ptin_intf %u/%u to ptin_port format",
            ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Interface must be active */
  if (!evcs[evc_id].intf[ptin_port].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Port %u (ptin_intf=%u/%u) not active in Evc %u",
            ptin_port, ptin_intf->intf_type,ptin_intf->intf_id, evc_id);
    return L7_FAILURE;
  }

  int_vlan = evcs[evc_id].intf[ptin_port].int_vlan;

#if 0
  /* Validate interval vlan */
  if (int_vlan < PTIN_VLAN_MIN || int_vlan > PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Evc %u, port %u, has an invalid int vlan (%u)",
            evc_id, ptin_port, int_vlan);
    return L7_FAILURE;
  }
#endif

  /* Return internal vlan */
  if (internal_vlan != L7_NULLPTR)
  {
    *internal_vlan = int_vlan;
  }

  return L7_SUCCESS;
}

/**
 * Gets the root vlan (internal) for a particular evc
 * 
 * @param evc_ext_id  : EVC extended id 
 * @param intRootVlan : Internal root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intRootVlan_get(L7_uint32 evc_ext_id, L7_uint16 *intRootVlan)
{
  L7_uint32 evc_id;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* Return root vlan */
  if (intRootVlan != L7_NULLPTR)
    *intRootVlan = evcs[evc_id].rvlan;

  return L7_SUCCESS;
}

/**
 * Gets the root vlan (internal) from the internal vlan
 * 
 * @param int_vlan     : Internal vlan
 * @param intRootVlan : Internal root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intRootVlan_get_fromIntVlan(L7_uint16 int_vlan, L7_uint16 *intRootVlan)
{
  L7_uint32 evc_id;

#if 0
  /* Validate arguments */
  if (int_vlan < PTIN_VLAN_MIN || int_vlan > PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }
#endif

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[int_vlan];
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC", int_vlan);
    return L7_FAILURE;
  }
  /* Is EVC in use? */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u (int_vlan=%u) is not in use", evc_id, int_vlan);
    return L7_NOT_EXIST;
  }

  /* Return root vlan */
  if (intRootVlan != L7_NULLPTR)
    *intRootVlan = evcs[evc_id].rvlan;

  return L7_SUCCESS;
}

/**
 * Gets flag options for a particular evc
 * 
 * @param evc_ext_id: EVC extended id 
 * @param flags     : Flag options
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_flags_get(L7_uint32 evc_ext_id, L7_uint32 *flags, L7_uint32 *mc_flood)
{
  L7_uint32 evc_id;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* Return evc flags */
  if (flags != L7_NULLPTR)    *flags = evcs[evc_id].flags;

  /* Return mc_flood status */
  if (mc_flood != L7_NULLPTR) *mc_flood = evcs[evc_id].mc_flood;

  return L7_SUCCESS;
}

/**
 * Gets flag options for a particular (internal) OVlan
 * 
 * @param int_vlan   : Internal OVlan
 * @param flags     : Flag options 
 * @param mc_flood  : Multicast flood
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_flags_get_fromIntVlan(L7_uint16 intOVlan, L7_uint32 *flags, L7_uint32 *mc_flood)
{
  L7_uint   evc_id;
  L7_uint32 evc_ext_id;

#if 0
  /* Validate arguments */
  if (intOVlan < PTIN_VLAN_MIN || intOVlan > PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }
#endif

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[intOVlan];
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",intOVlan);
    return L7_FAILURE;
  }

  evc_ext_id = evcs[evc_id].extended_id;

  /* Get external vlans */
  if (ptin_evc_flags_get(evc_ext_id, flags, mc_flood)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error getting EVC flags for evc_ext_id=%u, intOVlan=%u", evc_ext_id, intOVlan);
    return L7_FAILURE;
  }

  return L7_SUCCESS;

}

/**
 * Get the outer+inner external vlan for a specific 
 * interface+evc_id+innerVlan. 
 *  
 * @param intIfNum        : FP interface# 
 * @param evc_ext_id      : EVC extended index 
 * @param evc_int_id      : EVC internal index  
 * @param innerVlan       : Inner vlan
 * @param extOVlan        : External outer-vlan 
 * @param extIVlan        : External inner-vlan (01 means that there 
 *                      is no inner vlan)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_extVlans_get(L7_uint32 intIfNum, L7_uint32 evc_ext_id, L7_uint32 evc_int_id, L7_uint16 innerVlan, L7_uint16 *extOVlan, L7_uint16 *extIVlan)
{
  L7_uint32 ptin_port;
  L7_uint16 ovid, ivid;
  struct ptin_evc_client_s *pclientFlow;

  /* Validate arguments */
  if (intIfNum == 0 || ( evc_int_id!=(L7_uint32)-1  && evc_int_id>=PTIN_SYSTEM_N_EVCS))
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /*If the internal evc id is not given. We search for it*/
  if(evc_int_id==(L7_uint32)-1)
  {
    /* Is EVC in use? */
    if (ptin_evc_ext2int(evc_ext_id, &evc_int_id) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
      return L7_NOT_EXIST;
    }
  }

  /* Extract and validate intIfNum */
  if (ptin_intf_intIfNum2port(intIfNum,&ptin_port)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid intIfNum (%u)",intIfNum);
    return L7_FAILURE;
  }
  if (!evcs[evc_int_id].intf[ptin_port].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"IntIfNum=%u/ptin_port=%u is not used in EVC=%u",intIfNum,ptin_port,evc_int_id);
    return L7_FAILURE;
  }

  /* Initialize external outer+inner vlans */
  ovid = evcs[evc_int_id].intf[ptin_port].out_vlan;
  if (evcs[evc_int_id].intf[ptin_port].inner_vlan>0 && evcs[evc_int_id].intf[ptin_port].inner_vlan<4096)
  {
    ivid = evcs[evc_int_id].intf[ptin_port].inner_vlan;
  }
  else
  {
    ivid = innerVlan;
  }

  /* Interface is leaf? */
  if (evcs[evc_int_id].intf[ptin_port].type == PTIN_EVC_INTF_LEAF)
  {
  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    /* Look to clients/flows for Quattro or standard stacked evcs: */
    if (IS_EVC_QUATTRO(evc_int_id))
    {
      /* Find this client vlan in EVC */
      ptin_evc_find_flow(innerVlan, &(evcs[evc_int_id].intf[ptin_port].clients), (dl_queue_elem_t **) &pclientFlow);
      if (pclientFlow==NULL)
      {
        PT_LOG_ERR(LOG_CTX_EVC,"There is no flow with gemId=%u in IntIfNum=%u/ptin_port=%u and EVC=%u",innerVlan,intIfNum,ptin_port,evc_int_id);
        return L7_FAILURE;
      }
      ovid = pclientFlow->uni_ovid;
      ivid = pclientFlow->uni_ivid;
    }
    else
  #endif
    if (IS_EVC_STACKED(evc_int_id))
    {
      /* Find this client vlan in EVC */
      ptin_evc_find_client(innerVlan, &(evcs[evc_int_id].intf[ptin_port].clients), (dl_queue_elem_t **) &pclientFlow);
      if (pclientFlow==NULL)
      {
        PT_LOG_ERR(LOG_CTX_EVC,"There is no client/flow with cvid=%u in IntIfNum=%u/ptin_port=%u and EVC=%u",innerVlan,intIfNum,ptin_port,evc_int_id);
        return L7_FAILURE;
      }
      ovid = pclientFlow->uni_ovid;
      ivid = 0;
    }
  }

  /* Return output values */
  if (extOVlan!=L7_NULLPTR)  *extOVlan = ovid;
  if (extIVlan!=L7_NULLPTR)  *extIVlan = ivid;

  return L7_SUCCESS;
}

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
/**
 * Get the outer+inner external vlan for a specific evc_id+Vport 
 * (only applicable to QUATTRO services). 
 *  
 * @param evc_ext_id      : EVC extended index 
 * @param evc_int_id      : EVC internal index  
 * @param vport_id        : Vport_id 
 * @param port            : Physical port for transmission (out)
 * @param extOVlan        : External outer-vlan (out)
 * @param extIVlan        : External inner-vlan (01 means that there 
 *                      is no inner vlan) (out)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_extVlans_get_fromVPort(L7_uint32 evc_ext_id, L7_uint32 evc_int_id, L7_uint32 vport_id,
                                        L7_uint32 *port, L7_uint16 *extOVlan, L7_uint16 *extIVlan)
{
  L7_uint32 ptin_port;
  L7_uint16 ovid, ivid;
  intf_vp_entry_t intf_vp_entry;
  struct ptin_evc_client_s *pclientFlow;

  /* Validate arguments */
  if (evc_int_id!=(L7_uint32)-1 && evc_int_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* If the internal evc id is not given. We search for it*/
  if (evc_int_id==(L7_uint32)-1)
  {
    /* Is EVC in use? */
    if (ptin_evc_ext2int(evc_ext_id, &evc_int_id) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
      return L7_NOT_EXIST;
    }
  }

  /* Validate internal EVc id */
  if (!evcs[evc_int_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u is not in use", evc_int_id);
    return L7_FAILURE;
  }

  /* This should be a QUATTRO service */
  if (!(evcs[evc_int_id].flags & PTIN_EVC_MASK_QUATTRO))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u is not a QUATTRO service", evc_int_id);
    return L7_FAILURE;
  }

  /* Search for this virtual port */
  memset(&intf_vp_entry, 0x00, sizeof(intf_vp_entry));
  intf_vp_entry.vport_id = vport_id;

  if (intf_vp_DB(3, &intf_vp_entry) == 0)
  {
    /* Validate interface */
    if (ptin_intf_ptintf2port(&intf_vp_entry.pon, &ptin_port) != L7_SUCCESS ||
        ptin_port >= ptin_sys_number_of_ports)
    {
      PT_LOG_ERR(LOG_CTX_DAI, "Error obtaining ptin_port from PON interface (%u/%u)", intf_vp_entry.pon.intf_type, intf_vp_entry.pon.intf_id);
      return L7_FAILURE;
    }
    /* Validate GEM id */
    else if (intf_vp_entry.gem_id == 0 || intf_vp_entry.gem_id >= 4095)
    {
      PT_LOG_ERR(LOG_CTX_DAI, "Invalid GEM id %u", intf_vp_entry.gem_id);
      return L7_FAILURE;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_DAI, "Vport %u not found", vport_id);
    return L7_FAILURE;
  }

  /* Validate ptin_port */
  if (!evcs[evc_int_id].intf[ptin_port].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"ptin_port=%u is not used in EVC=%u",ptin_port,evc_int_id);
    return L7_FAILURE;
  }

  /* Initialize external outer+inner vlans */
  ovid = evcs[evc_int_id].intf[ptin_port].out_vlan;
  if (evcs[evc_int_id].intf[ptin_port].inner_vlan>0 && evcs[evc_int_id].intf[ptin_port].inner_vlan<4096)
  {
    ivid = evcs[evc_int_id].intf[ptin_port].inner_vlan;
  }
  else
  {
    ivid = 0;
  }

  /* Interface is leaf? */
  if (evcs[evc_int_id].intf[ptin_port].type == PTIN_EVC_INTF_LEAF)
  {
    /* Find this client vlan in EVC */
    ptin_evc_find_flow_fromVPort(vport_id, &(evcs[evc_int_id].intf[ptin_port].clients), (dl_queue_elem_t **) &pclientFlow);
    if (pclientFlow==NULL)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"There is no flow with vport_id=%u in ptin_port=%u and EVC=%u",vport_id,ptin_port,evc_int_id);
      return L7_FAILURE;
    }
    ovid = pclientFlow->uni_ovid;
    ivid = pclientFlow->uni_ivid;
  }

  /* Return output values */
  if (port != L7_NULLPTR)     *port = ptin_port;
  if (extOVlan!=L7_NULLPTR)   *extOVlan = ovid;
  if (extIVlan!=L7_NULLPTR)   *extIVlan = ivid;

  return L7_SUCCESS;
}
#endif

#if 0
/**
 * Validate outer vlan
 * 
 * @param intIfNum : Interface
 * @param extOVlan : external outer vlan
 * @param innerVlan: external inner vlan
 * 
 * @return L7_RC_t : L7_SUCCESS if extOVlan is valid 
 *                   L7_NOT_EXIST if extOVlan does not exist
 *                   L7_FAILURE if other error
 */
L7_RC_t ptin_evc_extVlan_validate(L7_uint16 evc_id, ptin_intf_t *ptin_intf, L7_uint16 extOVlan, L7_uint16 innerVlan)
{
  L7_uint32 ptin_port;
  struct ptin_evc_client_s *pclient;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Null pointer ptin_intf");
    return L7_FAILURE;
  }
  if (extOVlan<PTIN_VLAN_MIN || extOVlan>PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid external outer vlan %u",extOVlan);
    return L7_FAILURE;
  }
  /* Validate evc index */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }
  /* EVC must be active */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u should be in use",evc_id);
    return L7_FAILURE;
  }

  /* Get ptin_port */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS || ptin_port>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Cannot convert ptin_intf %u/%u to ptin_port format",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  /* Interface must be active */
  if (!evcs[evc_id].intf[ptin_port].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"ptin_intf=%u/%u (ptin_port=%u) is not used in EVC=%u", ptin_intf->intf_type, ptin_intf->intf_id, ptin_port, evc_id);
    return L7_FAILURE;
  }

  /* If EVC is stacked, and interface is a leaf, search for its clients */
  if (IS_EVC_STACKED(evc_id) && IS_EVC_INTF_LEAF(evc_id,ptin_port))
  {
    /* Validate inner vlan */
    if (innerVlan==0 || innerVlan>=4096)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Invalid inner vlan %u",innerVlan);
      return L7_FAILURE;
    }

    /* Check if client entry already exists */
    ptin_evc_find_client(innerVlan, &evcs[evc_id].intf[ptin_port].clients, (dl_queue_elem_t**) &pclient);

    /* Client not found */
    if (pclient == L7_NULLPTR)
    {
      return L7_NOT_EXIST;
    }

    /* Compare outer vlan: if found, leave cycle */
    if (pclient->out_vlan != extOVlan)
    {
      return L7_NOT_EXIST;
    }
  }
  /* If EVC unstacked, or interface is root, compare the interface outer vlan */
  else
  {
    /* Compare outer vlan: if found, leave cycle */
    if (evcs[evc_id].intf[ptin_port].out_vlan != extOVlan)
    {
      return L7_NOT_EXIST;
    }
  }
    
  /* If we get here, it's because outer vlan is valid */
  return L7_SUCCESS;
}
#endif

/**
 * Get internal vlans, from external vlans and the interface
 * 
 * @author mruas (8/6/2013)
 * 
 * @param intIfNum : Interface
 * @param extOVlan : external outer vlan
 * @param extIVlan : external inner vlan
 * @param intOVlan : internal outer vlan
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_evc_intVlan_get_fromOVlan(ptin_intf_t *ptin_intf, L7_uint16 extOVlan, L7_uint16 extIVlan,
                             L7_uint16 *intOVlan)
{
  L7_uint16 int_vlan;
  L7_uint32 ptin_port, evc_id;
  struct ptin_evc_client_s *pclientFlow;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Null pointer ptin_intf");
    return L7_FAILURE;
  }
  if (extOVlan<PTIN_VLAN_MIN || extOVlan>PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid external outer vlan %u",extOVlan);
    return L7_FAILURE;
  }

  /* Get ptin_port */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS || ptin_port>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Cannot convert ptin_intf %u/%u to ptin_port format",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Run all valid EVCs searching for this outer vlan */
  for (evc_id=0; evc_id<PTIN_SYSTEM_N_EVCS; evc_id++)
  {
    /* Skip not active EVCs */
    if (!evcs[evc_id].in_use)
      continue;

    /* Skip EVCs with this interface not used */
    if (!evcs[evc_id].intf[ptin_port].in_use)
      continue;

    /* Ignore leaf ports of IPTV EVCs */
    if (IS_EVC_IPTV(evc_id) && IS_EVC_INTF_LEAF(evc_id,ptin_port))
    {
      continue;
    }
    /* If EVC is stacked, and interface is a leaf, search for its clients */
    else if ((IS_EVC_QUATTRO(evc_id) || IS_EVC_STACKED(evc_id)) && IS_EVC_INTF_LEAF(evc_id,ptin_port))
    {
      /* Validate inner vlan */
      if (extIVlan==0 || extIVlan>=4096)
        continue;

      /* Check if client entry already exists */
      ptin_evc_find_client(extIVlan, &evcs[evc_id].intf[ptin_port].clients, (dl_queue_elem_t**) &pclientFlow);

      /* Client not found */
      if (pclientFlow == L7_NULLPTR)
        continue;

      /* Compare outer vlan: if found, leave cycle */
      if (pclientFlow->uni_ovid == extOVlan)
        break;
    }
    /* If EVC unstacked, or interface is root, compare the interface outer vlan */
    else
    {
      /* Compare outer vlan: if found, leave cycle */
      if (evcs[evc_id].intf[ptin_port].out_vlan == extOVlan &&
          (evcs[evc_id].intf[ptin_port].inner_vlan == 0 || evcs[evc_id].intf[ptin_port].inner_vlan == extIVlan))
        break;
    }
  }

  /* Check if outer vlan was found */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC not found with outer vlan %u in ptin_intf %u/%u", extOVlan, ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* EVC was found at this point */
  int_vlan = evcs[evc_id].intf[ptin_port].int_vlan;

#if 0
  /* Validate internal vlan */
  if (int_vlan < PTIN_VLAN_MIN || int_vlan > PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid internal vlan %u for EVC %u and ptin_intf %u/%u",
            int_vlan, evc_id, ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
#endif

  /* Return internal vlan */
  if (intOVlan!=L7_NULLPTR)
    *intOVlan = int_vlan;
    
  return L7_SUCCESS;
}

/**
 * Get interface type for a given internal vlan
 * 
 * @param int_vlan  : Internal vlan 
 * @param intIfNum : Interface
 * @param type     : Interface type (output)
 *                    PTIN_EVC_INTF_ROOT=0,
 *                    PTIN_EVC_INTF_LEAF=1,
 *                    PTIN_EVC_INTF_NOTUSED=255
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intf_type_get(L7_uint16 int_vlan, L7_uint32 intIfNum, L7_uint8 *type)
{
  ptin_evc_intfCfg_t intfCfg;
  L7_uint16 evc_id;
  ptin_intf_t ptin_intf;

  /* Validate interface */
  if (ptin_intf_intIfNum2ptintf(intIfNum, &ptin_intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid intIfNum %u", intIfNum);
    return L7_FAILURE;
  }

#if 0
  /* Validate arguments */
  if (int_vlan < PTIN_VLAN_MIN || int_vlan > PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid int_vlan %u", int_vlan);
    return L7_FAILURE;
  }
#endif

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[int_vlan];
  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",int_vlan);
    return L7_FAILURE;
  }

  /* Get MC EVC configuration */
  memset(&intfCfg, 0x00, sizeof(intfCfg));
  if (ptin_evc_intfCfg_get(evcs[evc_id].extended_id, &ptin_intf, &intfCfg) != L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      PT_LOG_ERR(LOG_CTX_EVC,"Error getting evc %u (int_vlan=%u), intf=%u/%u configuration", evc_id, int_vlan, ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }

  /* type pointer must not be null */
  if (type != L7_NULLPTR)
  {
    if (!intfCfg.in_use)
    {
      *type = PTIN_EVC_INTF_NOTUSED;
    }
    else
    {
      *type = intfCfg.type;
    }
  }

  return L7_SUCCESS;
}

/**
 * Check if the given interface is of type ROOT. 
 * 
 * @param int_vlan  : Internal vlan 
 * @param intIfNum : Interface
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_evc_intf_isRoot(L7_uint16 int_vlan, L7_uint32 intIfNum)
{
  L7_uint8 intf_type;

  /* Validate arguments */
  if ( intIfNum == 0 || intIfNum >= L7_MAX_INTERFACE_COUNT ||
      (int_vlan != 0 /*&& (int_vlan < PTIN_VLAN_MIN || int_vlan > PTIN_VLAN_MAX)*/) )
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid arguments: intIfNum=%u int_vlan=%u", intIfNum, int_vlan);
    return L7_FALSE;
  }

  /* If VLAN is null, return general trusted state */
  if (int_vlan == 0)
  {
    return L7_TRUE;
  }

  /* Get interface configuration */
  if (ptin_evc_intf_type_get(int_vlan, intIfNum, &intf_type)!=L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_DHCP, "Error acquiring interface %u/%u type from internalVid %u and intIfNum %u", int_vlan, intIfNum);
    return L7_FALSE;
  }
  if(intf_type == PTIN_EVC_INTF_ROOT)
  {
     return L7_TRUE;
  }

  return L7_FALSE;
}

/**
 * Get the list of interfaces associated to a internal vlan
 * 
 * @param int_vlan  : Internal vlan 
 * @param type     : Interface type 
 *                    PTIN_EVC_INTF_ROOT=0,
 *                    PTIN_EVC_INTF_LEAF=1
 * @param intfList : List of interfaces
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intfType_getList(L7_uint16 int_vlan, L7_uint8 type, NIM_INTF_MASK_t *intfList)
{
  ptin_HwEthMef10Evc_t evcCfg;
  L7_uint intf_idx;
  L7_uint32 intIfNum;
  ptin_intf_t ptin_intf;
  L7_uint16 evc_id;

#if 0
  /* Validate arguments */
  if (int_vlan < PTIN_VLAN_MIN || int_vlan > PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }
#endif

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[int_vlan];
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",int_vlan);
    return L7_FAILURE;
  }

  /* Get MC EVC configuration */
  memset(&evcCfg,0x00,sizeof(ptin_HwEthMef10Evc_t));
  evcCfg.index = evcs[evc_id].extended_id;
  if (ptin_evc_get(&evcCfg)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      PT_LOG_ERR(LOG_CTX_EVC,"Error getting evc %u configuration (int_vlan=%u)",evc_id,int_vlan);
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
  L7_uint   evc_int_id;
  L7_uint32 evc_ext_id;

  /* Validate arguments */
  if (intIfNum==0 /*|| intOVlan<PTIN_VLAN_MIN || intOVlan>PTIN_VLAN_MAX*/)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get evc id and validate it */
  evc_int_id = evcId_from_internalVlan[intOVlan];
  if (evc_int_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",intOVlan);
    return L7_FAILURE;
  }

  evc_ext_id = evcs[evc_int_id].extended_id;

  /* Get external vlans */
  if (ptin_evc_extVlans_get(intIfNum, evc_ext_id, evc_int_id, intIVlan, extOVlan, extIVlan)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error getting external vlans for intIfNum=%u, evc_ext_id=%u, intIVlan=%u",intIfNum,evc_ext_id,intIVlan);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
/**
 * Get the outer+inner external vlan for a specific oVLAN+Vport 
 * (only applicable to QUATTRO services). 
 * 
 * @param intOVlan   : Internal outer-vlan 
 * @param vport_id   : Vport id 
 * @param intIfNum   : Physical port for transmission (out)
 * @param extOVlan   : External outer-vlan 
 * @param extIVlan   : External inner-vlan (01 means that there 
 *                     is no inner vlan)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_extVlans_get_fromIntVlanVPort(L7_uint16 intOVlan, L7_uint32 vport_id,
                                               L7_uint32 *intIfNum, L7_uint16 *extOVlan, L7_uint16 *extIVlan)
{
  L7_uint   evc_int_id;
  L7_uint32 evc_ext_id;
  L7_uint32 ptin_port;

#if 0
  /* Validate arguments */
  if (intOVlan<PTIN_VLAN_MIN || intOVlan>PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }
#endif

  /* Get evc id and validate it */
  evc_int_id = evcId_from_internalVlan[intOVlan];
  if (evc_int_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",intOVlan);
    return L7_FAILURE;
  }

  evc_ext_id = evcs[evc_int_id].extended_id;

  /* Get external vlans */
  if (ptin_evc_extVlans_get_fromVPort(evc_ext_id, evc_int_id, vport_id, &ptin_port, extOVlan, extIVlan)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error getting external vlans for evc_ext_id=%u, vport_id=%u",evc_ext_id,vport_id);
    return L7_FAILURE;
  }

  if (intIfNum != L7_NULLPTR)
  {
    if (ptin_intf_port2intIfNum(ptin_port, intIfNum) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Error converting ptin_port %u to intIfNum format",ptin_port);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}
#endif

/**
 * Return EVC type. 
 * 
 * @param evc_id_ext : extended evc id
 * @param evc_type   : EVC type (output)
 *  
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_check_evctype(L7_uint32 evc_id_ext, L7_uint8 *evc_type)
{
  L7_uint32 evc_id;

  /* Get local evc id */
  if (ptin_evc_ext2int(evc_id_ext, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error getting local evc id");
    return L7_NOT_EXIST;
  }

  /* EVC should be active */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u should be in use",evc_id);
    return L7_FAILURE;
  }

  /* Check if EVC is stacked, and return result */
  if (evc_type != L7_NULLPTR)
  {
    if (evcs[evc_id].flags & PTIN_EVC_MASK_QUATTRO)
    {
      *evc_type = (evcs[evc_id].flags & PTIN_EVC_MASK_STACKED) ? PTIN_EVC_TYPE_QUATTRO_STACKED : PTIN_EVC_TYPE_QUATTRO_UNSTACKED;
    }
    else
    {
      *evc_type = (evcs[evc_id].flags & PTIN_EVC_MASK_P2P) ? PTIN_EVC_TYPE_STD_P2P : PTIN_EVC_TYPE_STD_P2MP;
    }
  }
  
  return L7_SUCCESS;
}

/**
 * Return EVC type from internal vlan. 
 *  
 * @param int_vlan    : Internal outer-vlan 
 * @param evc_type   : evc type (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_check_evctype_fromIntVlan(L7_uint16 int_vlan, L7_uint8 *evc_type)
{
  L7_uint evc_id;

#if 0
  /* Validate arguments */
  if (int_vlan < PTIN_VLAN_MIN || int_vlan > PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }
#endif

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[int_vlan];
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",int_vlan);
    return L7_FAILURE;
  }

  /* EVC should be active */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u should be in use (int_vlan=%u)",evc_id,int_vlan);
    return L7_FAILURE;
  }

  /* Check if EVC is stacked, and return result */
  return ptin_evc_check_evctype(evcs[evc_id].extended_id, evc_type);
}

/**
 * Check if a specific internal VLAN is a QUATTRO service.
 *  
 * @param int_vlan    : Internal outer-vlan 
 * 
 * @return L7_BOOL: L7_TRUE or L7_FALSE
 */
L7_BOOL ptin_evc_is_quattro_fromIntVlan(L7_uint16 int_vlan)
{
  L7_uint8 evc_type;

  /* Get evc type */
  if (ptin_evc_check_evctype_fromIntVlan(int_vlan, &evc_type) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  /* Only return TRUE is is QUATTRO */
  return (evc_type == PTIN_EVC_TYPE_QUATTRO_STACKED || evc_type == PTIN_EVC_TYPE_QUATTRO_UNSTACKED);
}

/**
 * Check if the EVC related to an internal vlan is stacked. 
 *  
 * @param int_vlan    : Internal outer-vlan 
 * @param is_stacked : Is EVC stacked? (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_check_is_stacked_fromIntVlan(L7_uint16 int_vlan, L7_BOOL *is_stacked)
{
  L7_uint evc_id;

#if 0
  /* Validate arguments */
  if (int_vlan < PTIN_VLAN_MIN || int_vlan > PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }
#endif

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[int_vlan];
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",int_vlan);
    return L7_FAILURE;
  }

  /* EVC should be active */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u should be in use (int_vlan=%u)",evc_id,int_vlan);
    return L7_FAILURE;
  }

  /* Check if EVC is stacked, and return result */
  if (is_stacked!=L7_NULLPTR)
  {
    *is_stacked = IS_EVC_STACKED(evc_id);
  }

  return L7_SUCCESS;
}

/**
 * Validate interface and vlan belonging to a valid interface 
 * inside a valid EVC 
 * 
 * @param intIfNum : source interface number
 * @param int_vlan  : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS: Parameters are valid
 *                   L7_FAILURE: Not valid
 */
L7_RC_t ptin_evc_intfVlan_validate(L7_uint32 intIfNum, L7_uint16 int_vlan)
{
  L7_uint   evc_id;
  L7_uint32 ptin_port;

#if 0
  /* Validate arguments */
  if (int_vlan < PTIN_VLAN_MIN || int_vlan > PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }
#endif

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[int_vlan];
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",int_vlan);
    return L7_FAILURE;
  }

  /* EVC should be active */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u should be in use (int_vlan=%u)",evc_id,int_vlan);
    return L7_FAILURE;
  }

  /* Convert intIfNum to ptin_port format */
  if ( ptin_intf_intIfNum2port(intIfNum, &ptin_port) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Not valid intIfNum %u",intIfNum);
    return L7_FAILURE;
  }

  /* Validate ptin_port */
  if ( ptin_port >= PTIN_SYSTEM_N_INTERF )
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid ptin_port %u from intIfNum %u",ptin_port, intIfNum);
    return L7_FAILURE;
  }

  /* Check if port is in use */
  if ( !evcs[evc_id].intf[ptin_port].in_use )
  {
    //PT_LOG_ERR(LOG_CTX_EVC,"ptin_port %u (intIfNum %u) not used in evc %u",ptin_port, intIfNum, evc_id);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Creates or reconfigures an EVC
 * 
 * @param evcConf Pointer to the input struct
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_create(ptin_HwEthMef10Evc_t *evcConf)
{
  L7_uint   i;
  ptin_intf_t ptin_intf;
  L7_uint   evc_id, evc_ext_id;
  L7_int    intf2cfg[PTIN_SYSTEM_N_INTERF]; /* Lookup array to map sequential to indexed intf */
  L7_uint8  evc_type;
  L7_BOOL   is_p2p, is_quattro, is_stacked;
  L7_BOOL   maclearning;
  L7_BOOL   dhcpv4_enabled, dhcpv6_enabled, igmp_enabled, pppoe_enabled, iptv_enabled;
  L7_BOOL   cpu_trap;
  L7_BOOL   error = L7_FALSE, new_evc = L7_FALSE;
  L7_uint   n_roots;
  L7_uint   n_leafs;
  L7_int    root_port1, root_port2, leaf_port1;
  L7_int    p2p_port1, p2p_port2;
  L7_uint16 root_vlan;
  L7_uint   ptin_port;
  L7_uint32 intIfNum;
  L7_int    multicast_group;
  dl_queue_t *freeVlan_queue = L7_NULLPTR;
  ptin_HwEthMef10Intf_t intf_cfg;
  L7_RC_t   rc;

  evc_ext_id = evcConf->index;

  /* Validate extended EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC id %u is out of range [0..%u[", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS);
    return L7_FAILURE;
  }

  if (ptin_evc_ext2int(evc_ext_id, &evc_id) == L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_EVC, "Configuring eEVC# %u (internal index %u)...", evc_ext_id, evc_id);
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_EVC, "Configuring eEVC# %u (new index)...", evc_ext_id);
  }

  /* Validate parameters */
  if (ptin_evc_param_verify(evcConf)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Given EVC has invalid parameters... nothing done!");
    return L7_FAILURE;
  }

  is_p2p         = (evcConf->flags & PTIN_EVC_MASK_P2P            ) == PTIN_EVC_MASK_P2P;
  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  is_quattro     = (evcConf->flags & PTIN_EVC_MASK_QUATTRO        ) == PTIN_EVC_MASK_QUATTRO;
  #else
  is_quattro     = 0;
  evcConf->flags &= ~((L7_uint32) PTIN_EVC_MASK_QUATTRO);
  #endif
  is_stacked     = (evcConf->flags & PTIN_EVC_MASK_STACKED        ) == PTIN_EVC_MASK_STACKED;
  maclearning    = (evcConf->flags & PTIN_EVC_MASK_MACLEARNING    ) == PTIN_EVC_MASK_MACLEARNING;
  dhcpv4_enabled = (evcConf->flags & PTIN_EVC_MASK_DHCPV4_PROTOCOL) == PTIN_EVC_MASK_DHCPV4_PROTOCOL;
  dhcpv6_enabled = (evcConf->flags & PTIN_EVC_MASK_DHCPV6_PROTOCOL) == PTIN_EVC_MASK_DHCPV6_PROTOCOL;
  igmp_enabled   = (evcConf->flags & PTIN_EVC_MASK_IGMP_PROTOCOL  ) == PTIN_EVC_MASK_IGMP_PROTOCOL;
  pppoe_enabled  = (evcConf->flags & PTIN_EVC_MASK_PPPOE_PROTOCOL ) == PTIN_EVC_MASK_PPPOE_PROTOCOL;
  iptv_enabled   = (evcConf->flags & PTIN_EVC_MASK_MC_IPTV        ) == PTIN_EVC_MASK_MC_IPTV;
  cpu_trap       = (evcConf->flags & PTIN_EVC_MASK_CPU_TRAPPING   ) == PTIN_EVC_MASK_CPU_TRAPPING;

  /* Determine EVC type */
  if (evcConf->flags & PTIN_EVC_MASK_QUATTRO)
  {
    evc_type = (evcConf->flags & PTIN_EVC_MASK_STACKED) ? PTIN_EVC_TYPE_QUATTRO_STACKED : PTIN_EVC_TYPE_QUATTRO_UNSTACKED;
  }
  else
  {
    evc_type = (evcConf->flags & PTIN_EVC_MASK_P2P) ? PTIN_EVC_TYPE_STD_P2P : PTIN_EVC_TYPE_STD_P2MP;
  }

  /* Check if this EVC is allowd to be QUATTRO type */
  if (is_quattro)
  {
    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: This is a QUATTRO EVC", evc_ext_id);
    #else
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u: Flows not available for this board", evc_ext_id);
    return L7_FAILURE;
    #endif
  }

  /* Get the number of Roots and Leafs of received msg (for validation purposes) */
  /* Also save the first root and leaf port */
  n_roots = 0;
  n_leafs = 0;
  root_port1 = root_port2 = leaf_port1 = -1;
  for (i=0; i<evcConf->n_intf; i++)
  {
    /* Convert Phy/Lag# into PTin Intf index */
    if (evcConf->intf[i].intf_type == PTIN_EVC_INTF_PHYSICAL)
      ptin_port = evcConf->intf[i].intf_id;
    else
      ptin_port = evcConf->intf[i].intf_id + PTIN_SYSTEM_N_PORTS;

    if (evcConf->intf[i].mef_type == PTIN_EVC_INTF_ROOT)
    {
      n_roots++;
      if (root_port1 < 0)       root_port1 = ptin_port;   /* First root port */
      else if (root_port2 < 0)  root_port2 = ptin_port;   /* Second root port */
    }
    else
    {
      n_leafs++;
      if (leaf_port1 < 0)       leaf_port1 = ptin_port;   /* First leaf port */
    }
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: First root ports = %u,%u   First leaf port = %u", evc_ext_id, root_port1, root_port2, leaf_port1);

  /* Do not accept:
   *   1. leafs in EVC# PTIN_EVC_INBAND */
  if (evc_ext_id == PTIN_EVC_INBAND && n_leafs != 0)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u: combination of roots/leafs is invalid! [roots=%u leafs=%u]",
            evc_ext_id, n_roots, n_leafs);
    return L7_FAILURE;
  }

  /* For EVC# PTIN_EVC_INBAND, it must be point-to-multipoint! */
  if (evc_ext_id == PTIN_EVC_INBAND && evc_type != PTIN_EVC_TYPE_STD_P2MP)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u: inBand EVC cannot be P2P/QUATTRO!", evc_ext_id);
    return L7_FAILURE;
  }

  /* Determine pair of ports for 1:1 EVCs (needed for unstacked EVCs) */
  p2p_port1 = -1;
  p2p_port2 = -1;
  /* Unstacked 1:1 EVCs only accept one root and one leaf, or two root ports */
  if (is_p2p || is_quattro)
  {
    /* With no leafs, use the first two root ports */
    if ( n_roots >= 2 && n_leafs == 0 )
    {
      p2p_port1 = root_port1;
      p2p_port2 = root_port2;
    }
    /* Otherwise, use first root port and first leaf port */
    else if ( n_roots >= 1 && n_leafs >= 1)
    {
      p2p_port1 = root_port1;
      p2p_port2 = leaf_port1;
    }
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: Port1 = %d   Port2 = %d", evc_ext_id, p2p_port1, p2p_port2 );

  /* Check if phy ports are already assigned to LAGs */
  for (i=0; i<evcConf->n_intf; i++)
  {
    if (evcConf->intf[i].intf_type == PTIN_EVC_INTF_PHYSICAL)
    {
      ptin_intf_port2intIfNum(evcConf->intf[i].intf_id, &intIfNum);
      if (dot3adAggGet(intIfNum, &intIfNum) == L7_SUCCESS)
      {
        ptin_intf_intIfNum2port(intIfNum, &ptin_port);
        PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u: port# %u belongs to LAG# %u", evc_ext_id,
                evcConf->intf[i].intf_id, ptin_port - PTIN_SYSTEM_N_PORTS);
        return L7_FAILURE;
      }
    }
  }

  /* New EVC ? */
  if ( !IS_eEVC_IN_USE(evc_ext_id) )
  {
    new_evc = L7_TRUE;

    rc = ptin_evc_entry_allocate(evc_ext_id, &evc_id);
    if (rc != L7_SUCCESS) {
      PT_LOG_CRITIC(LOG_CTX_EVC, "Error allocating an internal EVC!");
      return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: allocated new internal EVC id %u...", evc_ext_id, evc_id);

    /* Allocate queue of free vlans */
    #if (1)   /* EVCid extended feature */
    if (ptin_evc_freeVlanQueue_allocate(evc_id, evcConf->flags, &freeVlan_queue)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error allocating free vlan queue", evc_id);
      ptin_evc_entry_free(evc_ext_id);
      return L7_FAILURE;
    }
    #else
    freeVlan_queue = L7_NULLPTR;
    #endif

    /* EXCEPTION: EVC# PTIN_EVC_INBAND is for inBand management, which means a fixed root VLAN ID */
    if (evc_ext_id == PTIN_EVC_INBAND)
    {
      if (switching_vlan_create(PTIN_VLAN_INBAND) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error creating VLAN ID %u for inBand management purposes", evc_id, PTIN_VLAN_INBAND);
        #if (1)   /* EVCid extended feature */
        ptin_evc_freeVlanQueue_free(freeVlan_queue);
        #endif
        ptin_evc_entry_free(evc_ext_id);
        return L7_FAILURE;
      }
      root_vlan = PTIN_VLAN_INBAND;
    }
    #if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
    /* EXCEPTION: EVC# PTIN_EVC_BL2CPU is for Broadlight management, which means a fixed root VLAN ID */
    else if (evc_ext_id == PTIN_EVC_BL2CPU)
    {
      if (switching_vlan_create(PTIN_VLAN_BL2CPU) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error creating VLAN ID %u for inBand management purposes", evc_id, PTIN_VLAN_BL2CPU);
        #if (1)   /* EVCid extended feature */
        ptin_evc_freeVlanQueue_free(freeVlan_queue);
        #endif
        ptin_evc_entry_free(evc_ext_id);
        return L7_FAILURE;
      }
      root_vlan = PTIN_VLAN_BL2CPU;
    }
    #endif
    else
    {
      #if (1)   /* EVCid extended feature */
      /* Check if there are enough internal VLANs on the pool
       *  P2P:  only one internal VLAN is needed (shared among all the ports)
       *  P2MP: one VLAN is needed per leaf port plus one for all the root ports */
      if ( ((freeVlan_queue->n_elems < 1)) )
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: not enough internal VLANs available", evc_id);
        ptin_evc_freeVlanQueue_free(freeVlan_queue);
        ptin_evc_entry_free(evc_ext_id);
        return L7_FAILURE;
      }

      /* Get a VLAN from the pool to use as Internal Root VLAN */
      if (ptin_evc_vlan_allocate(&root_vlan, freeVlan_queue, evc_id) != L7_SUCCESS)  /* cannot fail! */
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error allocating internal VLAN", evc_id);
        ptin_evc_freeVlanQueue_free(freeVlan_queue);
        ptin_evc_entry_free(evc_ext_id);
        return L7_FAILURE;
      }
      #else
      if (ptin_evc_matrix_vlan_allocate(&root_vlan, evc_ext_id, evcConf->flags) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error getting new internal VLAN", evc_id);
        ptin_evc_entry_free(evc_ext_id);
        return L7_FAILURE;
      }
      #endif
    }

    PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: Enabling cross-connects?", evc_ext_id);

  #if (!PTIN_BOARD_IS_DNX)
    /* For stacked EVCs, we need to enable forwarding mode to OVID(+IVID) */
    ptin_crossconnect_enable(root_vlan, (evc_type==PTIN_EVC_TYPE_STD_P2P), is_stacked);
  #endif

    /* Virtual ports: Create Multicast group */
    multicast_group = -1;

    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    if (is_quattro)
    #else
    if (0)
    #endif
    {
      if (ptin_multicast_group_vlan_create(root_vlan, &multicast_group, 0)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error creating multicast group", evc_id);
        error = L7_TRUE;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Multicast group 0x%x created", evc_id, multicast_group);
      }

      if (!error)
      {
        /* Configure multicast group for the new VLAN/VSI */
      #if (PTIN_BOARD_IS_DNX)
        /* For XGS flooding is active */
        rc = ptin_vlanBridge_flood_set(0, root_vlan, 0, 0, 0);
      #else
        /* For XGS flooding is active */
        rc = ptin_vlanBridge_flood_set(0, root_vlan, multicast_group, multicast_group, multicast_group);
      #endif
        if (rc !=L7_SUCCESS )
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error configuring flooding for VLAN %u", evc_id, root_vlan);
          error = L7_TRUE;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Multicast group %u associated to vlan %u", evc_id, multicast_group, root_vlan);
        }
      }
    }    
    else
    {
      if (iptv_enabled)
      {
        if (ptin_multicast_group_l3_create(&multicast_group)!=L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error creating multicast group", evc_id);
          error = L7_TRUE;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Multicast group 0x%x created", evc_id, multicast_group);
        }
      }
    }

    /* If no error, proceed to configure each interface */
    if (!error)
    {
      /* Update EVC entry (this info will be used on the configuration functions) */
      evcs[evc_id].in_use           = L7_TRUE;
      evcs[evc_id].flags            = evcConf->flags & (~(L7_uint32) PTIN_EVC_MASK_PROTOCOLS);  /* Do not include protocols yet */
      evcs[evc_id].type             = evcConf->type;
      evcs[evc_id].mc_flood         = evcConf->mc_flood;
      evcs[evc_id].rvlan            = root_vlan;         /* ...above created */
      evcs[evc_id].queue_free_vlans = freeVlan_queue;
      evcs[evc_id].p2p_port1_intf   = p2p_port1;
      evcs[evc_id].p2p_port2_intf   = p2p_port2;
      evcs[evc_id].multicast_group  = multicast_group;

      PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: Adding interfaces", evc_ext_id);

      /* Configure each interface */
      for (i=0; i<evcConf->n_intf; i++)
      {
        /* Apply config */
        if (ptin_evc_intf_add(evc_id, &evcConf->intf[i]) != L7_SUCCESS)
        {
          error = L7_TRUE;
          break;
        }
      }

    #if (!PTIN_BOARD_IS_DNX)
      /* For EVCs point-to-point unstacked, create now the crossconnection */
      if (evc_type == PTIN_EVC_TYPE_STD_P2P && !is_stacked)
      {
        PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: Configuring P2P unstacked bridge", evc_ext_id);

        /* Add bridge between root and leaf port (Proot, Vr, Pleaf, Vs', Vc) */
        if (switching_p2p_bridge_add(p2p_port1, evcs[evc_id].intf[p2p_port1].int_vlan,
                                     p2p_port2, evcs[evc_id].intf[p2p_port2].int_vlan,
                                     0 /* No inner vlan */) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error adding single vlanbridge between port %u / vlan %u <=> port %u / vlan %u", evc_id,
                  p2p_port1, evcs[evc_id].intf[p2p_port1].int_vlan,
                  p2p_port2, evcs[evc_id].intf[p2p_port2].int_vlan);
          error = L7_TRUE;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Single vlanbridge added between port %u / vlan %u <=> port %u / vlan %u", evc_id,
                    p2p_port1, evcs[evc_id].intf[p2p_port1].int_vlan,
                    p2p_port2, evcs[evc_id].intf[p2p_port2].int_vlan);
        }
      }
    #endif /* !PTIN_BOARD_IS_DNX */
    }

    /* Successfull creation */
    if (!error)
    {
      #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
      /* Count number of QUATTRO P2P evcs */
      INCREMENT_QUATTRO_INSTANCE(evc_id, n_quattro_evcs);
      /* Update number of IGMP QUATTRO-P2P evcs */
      #endif
    }
  }
  /* EVC is in use: ONLY allow adding or removing ports */
  else
  {
    /* Get the internal index based on the extended one */
    if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u: EVC not active", evc_ext_id);
      return L7_FAILURE;
    }

    root_vlan = evcs[evc_id].rvlan;

    /* Status variables: Do not allow to be changed */
    is_p2p         = (evcs[evc_id].flags & PTIN_EVC_MASK_P2P    ) == PTIN_EVC_MASK_P2P;
    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    is_quattro     = (evcs[evc_id].flags & PTIN_EVC_MASK_QUATTRO) == PTIN_EVC_MASK_QUATTRO;
    #endif
    is_stacked     = (evcs[evc_id].flags & PTIN_EVC_MASK_STACKED) == PTIN_EVC_MASK_STACKED;
    /* The remaining variables allow to be updated */

    /* For unstacked P2P services, don't allow ports change */
    if ( !(is_p2p && !is_stacked) )
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: applying new config...", evc_id);

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
        if ((evcs[evc_id].intf[i].in_use == 0) && (intf2cfg[i] >= 0))
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: adding interface# %u...", evc_id, i);

          if (ptin_evc_intf_add(evc_id, &evcConf->intf[intf2cfg[i]]) != L7_SUCCESS)
          {
            /* Signal error, but try to process the rest of the config */
            error = L7_TRUE;
            continue;
          }
        }
        /* Port removed ? */
        else if ((evcs[evc_id].intf[i].in_use) && (intf2cfg[i] < 0))
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: removing interface# %u...", evc_id, i);

          /* Do not allow port removal if counters or BW profiles are configured */
          {
           unsigned long j;
           for (j=0; j<L7_COS_INTF_QUEUE_MAX_COUNT; j++)
               if (evcs[evc_id].intf[i].bwprofile[j]!= NULL) goto _ptin_evc_create1;
          }
          if ((evcs[evc_id].intf[i].counter  != NULL) ||
              (evcs[evc_id].intf[i].queue_probes.n_elems > 0) ||
              (evcs[evc_id].intf[i].clients.n_elems > 0))
          {
_ptin_evc_create1:
            PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: %s# %u has counter/BW profiles or flows configured! Cannot remove it!",
                    evc_id,
                    i < PTIN_SYSTEM_N_PORTS ? "PHY":"LAG",
                    i < PTIN_SYSTEM_N_PORTS ? i : i - PTIN_SYSTEM_N_PORTS);

            /* Signal error, but try to process the rest of the config */
            error = L7_TRUE;
            continue;
          }

          memset(&intf_cfg, 0x00, sizeof(intf_cfg));
          if (ptin_intf_port2ptintf(i, &ptin_intf) != L7_SUCCESS)
            continue;
          intf_cfg.intf_type = ptin_intf.intf_type;
          intf_cfg.intf_id   = ptin_intf.intf_id;

          if (ptin_evc_intf_remove(evc_id, &intf_cfg) != L7_SUCCESS)
          {
            /* Signal error, but try to process the rest of the config */
            error = L7_TRUE;
            continue;
          }
        }
        /* Port exists, and new configuration also has it: VLAN should be equal! */
        else if ((evcs[evc_id].intf[i].in_use) && (intf2cfg[i] >= 0))
        {
          /* Check outer vlan */
          if (IS_VLAN_VALID(evcConf->intf[intf2cfg[i]].vid) && IS_VLAN_VALID(evcs[evc_id].intf[i].out_vlan))
          {
            if (evcConf->intf[intf2cfg[i]].vid != evcs[evc_id].intf[i].out_vlan)
            {
              PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Outer VLAN of existent port %u do not match (%u vs %u)", evc_id, i, 
                      evcConf->intf[intf2cfg[i]].vid, evcs[evc_id].intf[i].out_vlan);
              error = L7_TRUE;
            }
          }
          else if (IS_VLAN_VALID(evcConf->intf[intf2cfg[i]].vid) != IS_VLAN_VALID(evcs[evc_id].intf[i].out_vlan))
          {
            PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: One of the Outer VLANs of existent port %u is not defined (%u vs %u)", evc_id, i,
                    evcConf->intf[intf2cfg[i]].vid, evcs[evc_id].intf[i].out_vlan);
            error = L7_TRUE;
          }

          /* Check inner vlan */
          if (IS_VLAN_VALID(evcConf->intf[intf2cfg[i]].vid_inner) && IS_VLAN_VALID(evcs[evc_id].intf[i].inner_vlan))
          {
            if (evcConf->intf[intf2cfg[i]].vid_inner != evcs[evc_id].intf[i].inner_vlan)
            {
              PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Inner VLAN of existent port %u do not match (%u vs %u)", evc_id, i,
                      evcConf->intf[intf2cfg[i]].vid_inner, evcs[evc_id].intf[i].inner_vlan);
              error = L7_TRUE;
            }
          }
          else if (IS_VLAN_VALID(evcConf->intf[intf2cfg[i]].vid_inner) != IS_VLAN_VALID(evcs[evc_id].intf[i].inner_vlan))
          {
            PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: One of the Inner VLANs of existent port %u is not defined (%u vs %u)", evc_id, i,
                    evcConf->intf[intf2cfg[i]].vid_inner, evcs[evc_id].intf[i].inner_vlan);
            error = L7_TRUE;
          }
        }
      }
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Unstacked point-to-point EVC... no change allowed", evc_id);
      error = L7_TRUE;
    }
  }

  /* If no error, update some EVC data */
  if (!error)
  {
    ptin_HwEthMef10EvcOptions_t evcOptions;

    memset(&evcOptions, 0x00, sizeof(evcOptions));

    evcOptions.mask        = PTIN_EVC_OPTIONS_MASK_FLAGS | PTIN_EVC_OPTIONS_MASK_MCFLOOD;
    evcOptions.mc_flood    = evcConf->mc_flood;
    evcOptions.flags.value = evcConf->flags;
    evcOptions.flags.mask  = PTIN_EVC_MASK_MACLEARNING | PTIN_EVC_MASK_CPU_TRAPPING |
                             PTIN_EVC_MASK_DHCPV4_PROTOCOL | PTIN_EVC_MASK_IGMP_PROTOCOL | PTIN_EVC_MASK_PPPOE_PROTOCOL | PTIN_EVC_MASK_DHCPV6_PROTOCOL;
    /* Apply options */
    rc = ptin_evc_config(evc_ext_id, &evcOptions);

    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error configuring EVC options", evc_id);
      return L7_FAILURE;
    }
  }

  /* Error occured: Remove configurations if EVC is new */
  if (error && new_evc)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: failed to be created", evc_id);

    #if (!PTIN_BOARD_IS_MATRIX)
    /* Remove DHCP instance */
    if (dhcpv4_enabled || dhcpv6_enabled)
    {
      ptin_dhcp_evc_remove(evc_ext_id);
    }
    /* remove PPPoE trap rule */
    if (pppoe_enabled)
    {
      ptin_pppoe_evc_remove(evc_ext_id);
    }
    #endif

    /* Remove IGMP trap rules */
    #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
    if (igmp_enabled)
    {
      ptin_igmp_evc_configure(evc_ext_id, L7_FALSE,
                              (!iptv_enabled && SINGLE_INSTANCE(evc_id, n_quattro_igmp_evcs)));
    }
    #endif


  #if (!PTIN_BOARD_IS_DNX)
    /* For unstacked P2P EVCs, remove single vlan cross-connection */
    if (evc_type == PTIN_EVC_TYPE_STD_P2P && !is_stacked)
    {
      /* Add bridge between root and leaf port (Proot, Vr, Pleaf, Vs', Vc) */
      switching_p2p_bridge_remove(p2p_port1, evcs[evc_id].intf[p2p_port1].int_vlan,
                                  p2p_port2, evcs[evc_id].intf[p2p_port2].int_vlan,
                                  0 /* No inner vlan */);
    }
  #endif /* !PTIN_BOARD_IS_DNX */
    
    /* Remove all previously configured interfaces */
    ptin_evc_intf_remove_all(evc_id);

    if ( ((iptv_enabled) 
          #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
          || (is_quattro)
          #endif
          ) && evcs[evc_id].multicast_group > 0)
    {
    #if (1 /*!PTIN_BOARD_IS_DNX*/)
      #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
      if (is_quattro)
      {
        /* Virtual ports: Configure multicast group for the vlan */
        if (ptin_vlanBridge_multicast_clear(root_vlan, evcs[evc_id].multicast_group)!=L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error configuring Multicast replication for VLAN %u (mcgroup=0x%x)", evc_id, root_vlan, evcs[evc_id].multicast_group);
          //return L7_FAILURE;
        }
        PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed multicast replication for vlan %u / group %d", evc_id, root_vlan, evcs[evc_id].multicast_group);
      }
      #endif
    #endif

      if (ptin_multicast_group_destroy(evcs[evc_id].multicast_group, 0)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error destroying Multicast group 0x%x", evc_id, evcs[evc_id].multicast_group);
        //return L7_FAILURE;/*Operation still running*/
      }
      evcs[evc_id].multicast_group = -1;
    } 

    if (evc_ext_id == PTIN_EVC_INBAND)
    {
      switching_vlan_delete(PTIN_VLAN_INBAND);
    }
    #if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
    else if (evc_ext_id == PTIN_EVC_BL2CPU)
    {
      switching_vlan_delete(PTIN_VLAN_BL2CPU);
    }
    #endif
    else
    {
    #if (1)   /* EVCid extended feature */
      ptin_evc_vlan_free(root_vlan, freeVlan_queue);
    #else
      ptin_evc_matrix_vlan_free(root_vlan);
    #endif
    }
    
    ptin_evc_entry_free(evc_ext_id);
  }

  /* If error */
  if (error)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u creation failed", evc_ext_id);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u successfully created with internal index %u", evc_ext_id, evc_id);

  return L7_SUCCESS;
}

/**
 * Add a port to an EVC
 *  
 * @param evc_ext_id : EVC index
 * @param evc_intf : Port information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_port_add(L7_uint32 evc_ext_id, ptin_HwEthMef10Intf_t *evc_intf)
{
  L7_uint evc_idx;
  L7_uint ptin_port;
  ptin_intf_t ptin_intf;

  /* Validate arguments */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC %u is out of range [0,%u[", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS);
    return L7_FAILURE;
  }
  if (evc_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Null pointer provided");
    return L7_FAILURE;
  }

  /* Correct input structure: no inner vlan, should have zero value */
  if (evc_intf->vid_inner > 4095)   evc_intf->vid_inner = 0;

  /* Get the internal index based on the extended one */
  if (ptin_evc_ext2int(evc_ext_id, &evc_idx) != L7_SUCCESS)
  {
    PT_LOG_NOTICE(LOG_CTX_EVC, "eEVC %u not existent", evc_ext_id);
    return L7_DEPENDENCY_NOT_MET;
  }

  /* EVC must be active */
  if (!evcs[evc_idx].in_use)
  {
    PT_LOG_NOTICE(LOG_CTX_EVC, "eEVC %u / EVC %u not active", evc_ext_id, evc_idx);
    return L7_DEPENDENCY_NOT_MET;
  }

  /* For unstacked P2P services, don't allow ports change */
  if (IS_EVC_P2P(evc_idx) && !IS_EVC_STACKED(evc_idx))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC %u: Unstacked point-to-point EVC... no change allowed", evc_ext_id, evc_idx);
    return L7_FAILURE;
  }

  /* Validate port */
  ptin_intf.intf_type = evc_intf->intf_type;
  ptin_intf.intf_id   = evc_intf->intf_id;
  if (ptin_intf_ptintf2port(&ptin_intf, &ptin_port) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC %u: Invalid port %u/%u", evc_ext_id, evc_idx, ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }

  /* Check if port is already present */
  if (evcs[evc_idx].intf[ptin_port].in_use)
  {
  #if (!PTIN_BOARD_IS_DNX)
    /* If configurations are different, return an error */
    if (evcs[evc_idx].intf[ptin_port].type      != evc_intf->mef_type
        || evcs[evc_idx].intf[ptin_port].out_vlan  != evc_intf->vid
        || evcs[evc_idx].intf[ptin_port].inner_vlan!= evc_intf->vid_inner )
    {
      PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC %u: Configuration mismatch for port %u/%u", evc_ext_id, evc_idx, ptin_intf.intf_type, ptin_intf.intf_id);
      return L7_FAILURE;
    }
    /* Equal configuration... nothing to be done! */
    return L7_SUCCESS;
  #endif
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u / EVC %u: Adding port %u/%u...", evc_ext_id, evc_idx, ptin_intf.intf_type, ptin_intf.intf_id);

  /* Add port to EVC */
  if (ptin_evc_intf_add(evc_idx, evc_intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC %u: Error adding port %u/%u", evc_ext_id, evc_idx, ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u / EVC %u: Added port %u/%u!", evc_ext_id, evc_idx, ptin_intf.intf_type, ptin_intf.intf_id);

  return L7_SUCCESS;
}

/**
 * Remove a port from an EVC
 *  
 * @param evc_ext_id : EVC index
 * @param evc_intf : Port information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_port_remove(L7_uint32 evc_ext_id, ptin_HwEthMef10Intf_t *evc_intf)
{
  L7_uint evc_idx;
  L7_uint ptin_port;
  ptin_intf_t ptin_intf;
  ptin_HwEthMef10Intf_t intf_cfg;

  /* Validate arguments */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC %u is out of range [0,%u[", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS);
    return L7_FAILURE;
  }
  if (evc_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Null pointer provided");
    return L7_FAILURE;
  }

  /* Correct input structure: no inner vlan, should have zero value */
  if (evc_intf->vid_inner > 4095)   evc_intf->vid_inner = 0;

  /* Get the internal index based on the extended one */
  if (ptin_evc_ext2int(evc_ext_id, &evc_idx) != L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "eEVC %u not existent", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* For unstacked P2P services, don't allow ports change */
  if (IS_EVC_P2P(evc_idx) && !IS_EVC_STACKED(evc_idx))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC %u: Unstacked point-to-point EVC... no change allowed", evc_ext_id, evc_idx);
    return L7_FAILURE;
  }

  /* Validate port */
  ptin_intf.intf_type = evc_intf->intf_type;
  ptin_intf.intf_id   = evc_intf->intf_id;
  if (ptin_intf_ptintf2port(&ptin_intf, &ptin_port) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC %u: Invalid port %u/%u", evc_ext_id, evc_idx, ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }

  /* Check if port is not present */
  if (!evcs[evc_idx].intf[ptin_port].in_use)
  {
    /* Nothing to be done! */
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC %u: port %u/%u don't exist!", evc_ext_id, evc_idx, ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_NOT_EXIST;
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u / EVC %u: Removing port %u/%u...", evc_ext_id, evc_idx, ptin_intf.intf_type, ptin_intf.intf_id);

#if (!PTIN_BOARD_IS_DNX)
  /* Remove associated resources */
  /* Clean service resources */
  if (ptin_evc_intf_clean(evc_idx, ptin_port, L7_TRUE)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error cleaning service profiles and counters!!!", evc_idx);
    return L7_FAILURE;
  }
  /* Remove all clients/flows */
  if (ptin_evc_intfclientsflows_remove(evc_idx, ptin_port)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing clients!!!", evc_idx);
    return L7_FAILURE;
  }

  /* Check if there is allocated resources */
  {
   unsigned long j;
   for (j=0; j<L7_COS_INTF_QUEUE_MAX_COUNT; j++)
       if (evcs[evc_idx].intf[ptin_port].bwprofile[j]!= NULL) goto _ptin_evc_port_remove1;
  }
  if ((evcs[evc_idx].intf[ptin_port].counter   != NULL) ||
      (evcs[evc_idx].intf[ptin_port].queue_probes.n_elems > 0))
  {
_ptin_evc_port_remove1:
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC# %u: Port %u/%u has counter/BW profiles/Probes configured! Cannot remove it!",
            evc_ext_id, evc_idx, ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }
  /* If clients/flows are attched to this port, also cannot remove this port */
  if (evcs[evc_idx].intf[ptin_port].clients.n_elems > 0)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC# %u: Port %u/%u still has clients/flows configured! Cannot remove it!",
            evc_ext_id, evc_idx, ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }
#endif

  memset(&intf_cfg, 0x00, sizeof(intf_cfg));
  intf_cfg.intf_type = ptin_intf.intf_type;
  intf_cfg.intf_id   = ptin_intf.intf_id;
  intf_cfg.vid       = evc_intf->vid;
  intf_cfg.vid_inner = evc_intf->vid_inner;

  /* Remove port */
  if (ptin_evc_intf_remove(evc_idx, &intf_cfg) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC# %u: Cannot remove port %u/%u",
            evc_ext_id, evc_idx, ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }

#if (PTIN_BOARD_IS_DNX)
  if ((evcs[evc_idx].n_roots + evcs[evc_idx].n_leafs) == 0)
#else
  /* If there is only 1 port, EVC will be destroyed */
  #if (PTIN_BOARD_IS_GPON)
  if ((evcs[evc_idx].n_roots + evcs[evc_idx].n_leafs) <= 1 || (evcs[evc_idx].n_leafs) == 0)
  #else
  if ((evcs[evc_idx].n_roots + evcs[evc_idx].n_leafs) <= 1)
  #endif
#endif
  {
    ptin_evc_delete(evc_ext_id);
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u / EVC %u: Removed port %u/%u!", evc_ext_id, evc_idx, ptin_intf.intf_type, ptin_intf.intf_id);

  return L7_SUCCESS;
}

/**
 * EVC options reconfiguration
 * 
 * @param evc_ext_id : EVC extended id
 * @param evcOptions : EVC options
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_config(L7_uint32 evc_ext_id, ptin_HwEthMef10EvcOptions_t *evcOptions)
{
  L7_uint   evc_id;
  L7_uint8  mc_flood;
  L7_BOOL   maclearning, cpu_trap;
  L7_BOOL   dhcpv4_enabled, dhcpv6_enabled, igmp_enabled, pppoe_enabled;
  L7_BOOL   error = L7_FALSE;

  /* Validate arguments */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC %u is out of range [0,%u[", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS);
    return L7_FAILURE;
  }
  if (evcOptions == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Null pointer provided");
    return L7_FAILURE;
  }

  /* Get the internal index based on the extended one */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "eEVC %u not existent", evc_ext_id);
    return L7_DEPENDENCY_NOT_MET;
  }

  /* Check if port is not present */
  if (!evcs[evc_id].in_use)
  {
    /* Nothing to be done! */
    PT_LOG_ERR(LOG_CTX_EVC, "EVC %u not active", evc_id);
    return L7_NOT_EXIST;
  }

  /* Init state vars, with current values */
  mc_flood       =  evcs[evc_id].mc_flood;
  maclearning    = (evcs[evc_id].flags & PTIN_EVC_MASK_MACLEARNING    ) == PTIN_EVC_MASK_MACLEARNING;
  cpu_trap       = (evcs[evc_id].flags & PTIN_EVC_MASK_CPU_TRAPPING   ) == PTIN_EVC_MASK_CPU_TRAPPING;
  dhcpv4_enabled = (evcs[evc_id].flags & PTIN_EVC_MASK_DHCPV4_PROTOCOL) == PTIN_EVC_MASK_DHCPV4_PROTOCOL;
  dhcpv6_enabled = (evcs[evc_id].flags & PTIN_EVC_MASK_DHCPV6_PROTOCOL) == PTIN_EVC_MASK_DHCPV6_PROTOCOL;
  igmp_enabled   = (evcs[evc_id].flags & PTIN_EVC_MASK_IGMP_PROTOCOL  ) == PTIN_EVC_MASK_IGMP_PROTOCOL;
  pppoe_enabled  = (evcs[evc_id].flags & PTIN_EVC_MASK_PPPOE_PROTOCOL ) == PTIN_EVC_MASK_PPPOE_PROTOCOL;

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: New -> MCflood=%u", evc_id, mc_flood);
  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: Old -> maclearning=%u, cpu_trap=%u", evc_id, maclearning, cpu_trap);
  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: Old -> IGMP=%u, DHCPv4=%u, DHCPv6=%u, PPPoE=%u", evc_id, igmp_enabled, dhcpv4_enabled, dhcpv6_enabled, pppoe_enabled);

  /* Change options? */
  if (evcOptions->mask & PTIN_EVC_OPTIONS_MASK_MCFLOOD)
  {
    mc_flood = evcOptions->mc_flood;
    PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: MCflood will be changed", evc_id);
    PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: New -> MCflood=%u", evc_id, mc_flood);
  }
  /* Flags */
  if (evcOptions->mask & PTIN_EVC_OPTIONS_MASK_FLAGS)
  {
    if (evcOptions->flags.mask & PTIN_EVC_MASK_MACLEARNING) 
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: MACLearning will be changed", evc_id);
      maclearning    = (evcOptions->flags.value & PTIN_EVC_MASK_MACLEARNING    ) == PTIN_EVC_MASK_MACLEARNING; 
    }
    if (evcOptions->flags.mask & PTIN_EVC_MASK_CPU_TRAPPING)
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: CPUtrap will be changed", evc_id);
      cpu_trap       = (evcOptions->flags.value & PTIN_EVC_MASK_CPU_TRAPPING   ) == PTIN_EVC_MASK_CPU_TRAPPING;
    }
    if (evcOptions->flags.mask & PTIN_EVC_MASK_DHCPV4_PROTOCOL)
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: DHCPv4 will be changed", evc_id);
      dhcpv4_enabled = (evcOptions->flags.value & PTIN_EVC_MASK_DHCPV4_PROTOCOL) == PTIN_EVC_MASK_DHCPV4_PROTOCOL;
    }
    if (evcOptions->flags.mask & PTIN_EVC_MASK_DHCPV6_PROTOCOL)
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: DHCPv6 will be changed", evc_id);
      dhcpv6_enabled = (evcOptions->flags.value & PTIN_EVC_MASK_DHCPV6_PROTOCOL) == PTIN_EVC_MASK_DHCPV6_PROTOCOL;
    }
    if (evcOptions->flags.mask & PTIN_EVC_MASK_IGMP_PROTOCOL)
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: IGMP will be changed", evc_id);
      igmp_enabled   = (evcOptions->flags.value & PTIN_EVC_MASK_IGMP_PROTOCOL  ) == PTIN_EVC_MASK_IGMP_PROTOCOL;
    }
    if (evcOptions->flags.mask & PTIN_EVC_MASK_PPPOE_PROTOCOL)
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: PPPoE will be changed", evc_id);
      pppoe_enabled  = (evcOptions->flags.value & PTIN_EVC_MASK_PPPOE_PROTOCOL ) == PTIN_EVC_MASK_PPPOE_PROTOCOL;
    }

    PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: New -> maclearning=%u, cpu_trap=%u", evc_id, maclearning, cpu_trap);
    PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: New -> IGMP=%u, DHCPv4=%u, DHCPv6=%u, PPPoE=%u", evc_id, igmp_enabled, dhcpv4_enabled, dhcpv6_enabled, pppoe_enabled);
  }

  /* VLAN configuration */
  if (switching_vlan_config(evcs[evc_id].rvlan, evcs[evc_id].rvlan, maclearning, mc_flood, cpu_trap) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error configuring VLAN %u [FwdVlan=%u MACLearning=%u MCFlood=%u]",
            evc_id, evcs[evc_id].rvlan, evcs[evc_id].rvlan, maclearning, mc_flood);
    return L7_FAILURE;
  }

  /* MAC Learning, Multicast flood and CPU trap */
  evcs[evc_id].mc_flood = mc_flood;
  (cpu_trap   ) ? (evcs[evc_id].flags |= PTIN_EVC_MASK_CPU_TRAPPING) : (evcs[evc_id].flags &= ~((L7_uint32) PTIN_EVC_MASK_CPU_TRAPPING));
  (maclearning) ? (evcs[evc_id].flags |= PTIN_EVC_MASK_MACLEARNING ) : (evcs[evc_id].flags &= ~((L7_uint32) PTIN_EVC_MASK_MACLEARNING ));

  error = L7_FALSE;

  /* Protocol management */
  if (!IS_EVC_QUATTRO(evc_id))
  {
  #if (!PTIN_BOARD_IS_MATRIX)
    PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: Checking instances", evc_ext_id);
    /* DHCP configuration */
    if (ptin_evc_update_dhcp(evc_id, &evcs[evc_id].flags, dhcpv4_enabled, dhcpv6_enabled,
                             L7_FALSE /*Update*/, L7_FALSE /*Do not look to counters*/) != L7_SUCCESS)
    {
      error = L7_TRUE;
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error configuring DHCP", evc_id);
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: DHCP configured", evc_id);
    }

    /* PPPoE configuration */
    if (ptin_evc_update_pppoe(evc_id, &evcs[evc_id].flags, pppoe_enabled,
                              L7_FALSE /*Update*/, L7_FALSE /*Do not look to counters*/) != L7_SUCCESS)
    {
      error = L7_TRUE;
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error configuring PPPoE", evc_id);
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: PPPoE configured", evc_id);
    }
  #endif

    /* IGMP configuration */
  #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
    if (ptin_evc_update_igmp(evc_id, &evcs[evc_id].flags, igmp_enabled,
                             L7_FALSE /*Update*/, L7_FALSE /*Do not look to counters*/) != L7_SUCCESS)
    {
      error = L7_TRUE;
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error configuring IGMP", evc_id);
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: IGMP configured", evc_id);
    }
  #endif
  }

  return L7_SUCCESS;
}

/**
 * Deletes an EVC
 * 
 * @param evc_ext_id
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_delete(L7_uint32 evc_ext_id)
{
  L7_uint evc_id;

  PT_LOG_TRACE(LOG_CTX_EVC, "Deleting eEVC# %u...", evc_ext_id);

  /* Validate eEVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u[", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS);
    return L7_FAILURE;
  }

  /* Convert to internal evc id */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC %u not existent", evc_ext_id);
    return L7_SUCCESS;
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u is mapped to internal id %u", evc_ext_id, evc_id);

  /* If this EVC belongs to an IGMP instance, stop procedure */
  if (ptin_igmp_is_evc_used(evc_ext_id))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: This EVC belongs to an IGMP instance... So it cannot be removed!", evc_id);
    return L7_FAILURE;
  }

  #ifdef EVC_CLIENTS_REQUIRE_CLEANUP_BEFORE_REMOVAL
  /* Check if there are clients pending... */
  if (evcs[evc_id].n_clientflows > 0)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: %u clients are still configured! EVC cannot be removed!",
            evc_id, evcs[evc_id].n_clientflows);
    return L7_FAILURE;
  }
  #endif

  /* For IGMP enabled EVCs */
  if (ptin_igmp_is_evc_used(evc_ext_id))
    ptin_igmp_evc_remove(evc_ext_id);

  #if (!PTIN_BOARD_IS_MATRIX)
  /* For DHCP enabled EVCs */
  if (ptin_dhcp_is_evc_used(evc_ext_id))
  {
    ptin_dhcp_evc_remove(evc_ext_id);
  }
  /* For PPPoE enabled EVCs */
  if (ptin_pppoe_is_evc_used(evc_ext_id))
  {
    ptin_pppoe_evc_remove(evc_ext_id);
  }
  #endif

  /* For IGMP enabled evcs, remove trap rules */
  #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  if (evcs[evc_id].flags & PTIN_EVC_MASK_IGMP_PROTOCOL)
  {
    if (ptin_igmp_evc_configure(evc_ext_id, L7_FALSE,
                                (!(evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV) && SINGLE_INSTANCE(evc_id, n_quattro_igmp_evcs))
                               ) != L7_SUCCESS)
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Error removing IGMP trap rules", evc_id);
    }
    /* Update number of igmp quattro-p2p evcs */
    DECREMENT_QUATTRO_INSTANCE(evc_id, n_quattro_igmp_evcs);
  }
  #endif

  /* Only clean service resources... Clients remotion will do the rest */
  if (ptin_evc_allintfs_clean(evc_id, L7_TRUE)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error cleaning service profiles and counters! EVC cannot be removed!",
            evc_id);
    return L7_FAILURE;
  }
  /* Clean any clients/flows that may exist */
  if (ptin_evc_allclientsflows_remove(evc_id)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing all clients! EVC cannot be removed!",
            evc_id);
    return L7_FAILURE;
  }

#if (!PTIN_BOARD_IS_DNX)
  /* For unstacked 1:1 EVCs, remove single vlan cross-connection */
  if (IS_EVC_STD_P2P(evc_id) && !IS_EVC_STACKED(evc_id))
  {
    L7_int port1 = evcs[evc_id].p2p_port1_intf;
    L7_int port2 = evcs[evc_id].p2p_port2_intf;

    /* Add bridge between root and leaf port (Proot, Vr, Pleaf, Vs', Vc) */
    if (switching_p2p_bridge_remove(port1, evcs[evc_id].intf[port2].int_vlan,
                                    port2, evcs[evc_id].intf[port2].int_vlan,
                                    0 /* No inner vlan */) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing single vlanbridge between port %u / vlan %u <=> port %u / vlan %u", evc_id,
              port1, evcs[evc_id].intf[port1].int_vlan,
              port2, evcs[evc_id].intf[port2].int_vlan);
      return L7_FAILURE;
    }
  }
#endif /* !PTIN_BOARD_IS_DNX */

  /* Remove all configured interfaces */
  if (ptin_evc_intf_remove_all(evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing interfaces config", evc_id);
    return L7_FAILURE;
  }

  /* If there is a MC group allocated, remove it */
  if (evcs[evc_id].multicast_group > 0)
  {
  #if (1 /*!PTIN_BOARD_IS_DNX*/)
    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED      
    if (IS_EVC_QUATTRO(evc_id))
    {
      /* Virtual ports: Configure multicast group for the vlan */
      if (ptin_vlanBridge_multicast_clear(evcs[evc_id].rvlan, evcs[evc_id].multicast_group)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing Multicast replication for VLAN %u (mcgroup=0x%x)", evc_id, evcs[evc_id].rvlan, evcs[evc_id].multicast_group);
      }
    }
    #endif
  #endif

    /*  Destroy Multicast group */
    if (ptin_multicast_group_destroy(evcs[evc_id].multicast_group, 0)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error destroying multicast group 0x%x", evc_id, evcs[evc_id].multicast_group);
    }    
  }
  
  evcs[evc_id].multicast_group = -1;

  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  /* Update number of QUATTRO-P2P evcs */
  DECREMENT_QUATTRO_INSTANCE(evc_id, n_quattro_evcs);
  #endif

  /* If this EVC is for InBand, the allocated VLAN must be deleted directly! */
  if (evc_ext_id == PTIN_EVC_INBAND)
  {
    switching_vlan_delete(PTIN_VLAN_INBAND);
  }
  #if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
  else if (evc_ext_id == PTIN_EVC_BL2CPU)
  {
    switching_vlan_delete(PTIN_VLAN_BL2CPU);
  }
  #endif
  else
  {
  #if (1)   /* EVCid extended feature */
    ptin_evc_vlan_free(evcs[evc_id].rvlan, evcs[evc_id].queue_free_vlans);
  #else
    ptin_evc_matrix_vlan_free(evcs[evc_id].rvlan);
  #endif
  }

  ptin_evc_entry_free(evc_ext_id);

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u successfully removed (internal id %u)", evc_ext_id, evc_id);

  return L7_SUCCESS;
}


/**
 * Destroys an EVC (extended index)
 * 
 * @param evc_ext_id
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_destroy(L7_uint32 evc_ext_id)
{
  L7_uint     intf_idx;
  L7_uint     evc_id;

  PT_LOG_TRACE(LOG_CTX_EVC, "Destroying eEVC# %u...", evc_ext_id);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u[", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS);
    return L7_FAILURE;
  }

  /* Convert to internal evc id */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC %u not existent", evc_ext_id);
    return L7_SUCCESS;
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u is mapped to internal id %u", evc_ext_id, evc_id);

  /* IF this EVC belongs to an IGMP instance, destroy that instance */
  if (ptin_igmp_is_evc_used(evc_ext_id))
    ptin_igmp_evc_remove(evc_ext_id);

  #if (!PTIN_BOARD_IS_MATRIX)
  /* IF this EVC belongs to a DHCP instance, destroy that instance */
  if (ptin_dhcp_is_evc_used(evc_ext_id))
  {
    ptin_dhcp_evc_remove(evc_ext_id);
  }
  /* IF this EVC belongs to a PPPoE instance, destroy that instance */
  if (ptin_pppoe_is_evc_used(evc_ext_id))
  {
    ptin_pppoe_evc_remove(evc_ext_id);
  }
  #endif

  /* For IGMP enabled evcs, remove trap rules */
  #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  if (evcs[evc_id].flags & PTIN_EVC_MASK_IGMP_PROTOCOL)
  {
    if (ptin_igmp_evc_configure(evc_ext_id, L7_FALSE,
                                (!(evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV) && SINGLE_INSTANCE(evc_id, n_quattro_igmp_evcs))
                               ) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing IGMP trap rules", evc_id);
    }
    /* Update number of igmp quattro-p2p evcs */
    DECREMENT_QUATTRO_INSTANCE(evc_id, n_quattro_igmp_evcs);
  }
  #endif

  /* IF this EVC belongs to an DHCP instance, destroy that instance */
  if (evcs[evc_id].flags & PTIN_EVC_MASK_DHCPV4_PROTOCOL)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: DHCP is not cleared!!!", evc_id);
//    TODO !!!
//    ptin_dhcp_instance_destroy(evc_ext_id);
  }

  /* For each interface... */
  for (intf_idx=0; intf_idx<PTIN_SYSTEM_N_INTERF; intf_idx++)
  {
    if (!evcs[evc_id].intf[intf_idx].in_use)
      continue;

    /* Only clean service resources... Clients remotion will do the rest */
    if (ptin_evc_intf_clean(evc_id, intf_idx, L7_TRUE)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error cleaning service profiles and counters!!!", evc_id);
      return L7_FAILURE;
    }

    /* Remove all clients/flows */
    if (ptin_evc_intfclientsflows_remove(evc_id, intf_idx)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing clients!!!", evc_id);
      return L7_FAILURE;
    }
  }

  /* Check if there are clients pending... */
  if (evcs[evc_id].n_clientflows > 0)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: %u clients are still configured! EVC cannot be destroyed!",
            evc_id, evcs[evc_id].n_clientflows);
    return L7_FAILURE;
  }

#if (!PTIN_BOARD_IS_DNX)
  /* For unstacked 1:1 EVCs, remove single vlan cross-connection */
  if (IS_EVC_STD_P2P(evc_id) && !IS_EVC_STACKED(evc_id))
  {
    L7_int port1 = evcs[evc_id].p2p_port1_intf;
    L7_int port2 = evcs[evc_id].p2p_port2_intf;

    /* Add bridge between root and leaf port (Proot, Vr, Pleaf, Vs', Vc) */
    if (switching_p2p_bridge_remove(port1, evcs[evc_id].intf[port2].int_vlan,
                                    port2, evcs[evc_id].intf[port2].int_vlan,
                                    0 /* No inner vlan */) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing single vlanbridge between port %u / vlan %u <=> port %u / vlan %u", evc_id,
              port1, evcs[evc_id].intf[port1].int_vlan,
              port2, evcs[evc_id].intf[port2].int_vlan);
      return L7_FAILURE;
    }
  }
#endif /* !PTIN_BOARD_IS_DNX */

  /* Remove all previously configured interfaces */
  if (ptin_evc_intf_remove_all(evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing interfaces config", evc_id);
    return L7_FAILURE;
  }

  if ( ((evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV)
        #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED                    
        || (IS_EVC_QUATTRO(evc_id)) 
        #endif
       ) &&  evcs[evc_id].multicast_group > 0)
  {
  #if (1 /*!PTIN_BOARD_IS_DNX*/)
    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    if (IS_EVC_QUATTRO(evc_id))
    {
      /* Virtual ports: Configure multicast group for the vlan */
      if (ptin_vlanBridge_multicast_clear(evcs[evc_id].rvlan, evcs[evc_id].multicast_group)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing Multicast replication for VLAN %u (mcgroup=0x%x)", evc_id, evcs[evc_id].rvlan, evcs[evc_id].multicast_group);
        return L7_FAILURE;      
      }
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed multicast replication for vlan %u / group 0x%x", evc_id, evcs[evc_id].rvlan, evcs[evc_id].multicast_group);
    }
    #endif
  #endif

    /* Destroy Multicast group */
    if (ptin_multicast_group_destroy(evcs[evc_id].multicast_group, 0)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error destroying multicast group 0x%x", evc_id, evcs[evc_id].multicast_group);
//    return L7_FAILURE; /*Operation still running*/
    }
  }  
  evcs[evc_id].multicast_group = -1;

  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  /* Update number of QUATTRO-P2P evcs */
  DECREMENT_QUATTRO_INSTANCE(evc_id, n_quattro_evcs);
  #endif

  /* If this EVC is for InBand, the allocated VLAN must be deleted directly! */
  if (evc_ext_id == PTIN_EVC_INBAND)
  {
    switching_vlan_delete(PTIN_VLAN_INBAND);
  }
  #if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
  else if (evc_ext_id == PTIN_EVC_BL2CPU)
  {
    switching_vlan_delete(PTIN_VLAN_BL2CPU);
  }
  #endif
  else
  {
  #if (1)   /* EVCid extended feature */
    ptin_evc_vlan_free(evcs[evc_id].rvlan, evcs[evc_id].queue_free_vlans);
  #else
    ptin_evc_matrix_vlan_free(evcs[evc_id].rvlan);
  #endif
  }

  ptin_evc_entry_free(evc_ext_id);

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u successfully destroyed (internal id %u)", evc_ext_id, evc_id);

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

  /* Start with index 1 because PTIN_EVC_INBAND=0 */
  for (i=1; i<PTIN_SYSTEM_N_EXTENDED_EVCS; i++)
  {
    /* Do not destroy this EVC */
    #if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
    if (i == PTIN_EVC_BL2CPU)
    {
      continue;
    }
    #endif

    if (IS_eEVC_IN_USE(i))
      ptin_evc_destroy(i);
  }

  return L7_SUCCESS;
}


/**
 * Adds a bridge to a stacked EVC between the root and a particular interface
 * 
 * @param evcBridge Bridge info
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_p2p_bridge_add(ptin_HwEthEvcBridge_t *evcBridge)
{
#if (!PTIN_BOARD_IS_DNX)
  L7_uint evc_id, evc_ext_id;
  L7_uint root_intf;
  L7_uint leaf_intf;
  L7_uint i;
  L7_RC_t rc = L7_SUCCESS;
  struct ptin_evc_client_s *pclient;

  evc_ext_id = evcBridge->index;

  PT_LOG_TRACE(LOG_CTX_EVC, "Adding eEVC# %u bridge connection...", evc_ext_id);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_FAILURE;
  }

  /* Check if the EVC is P2P or P2MP */
  if (!IS_EVC_STD(evc_id))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u is QUATTRO?", evc_id);
    return L7_FAILURE;
  }

  /* Check if the EVC is stacked */
  if (!IS_EVC_STACKED(evc_id))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u is not stacked!!!", evc_id);
    return L7_FAILURE;
  }

  /* Determine leaf ptin_intf */
  if (evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL)
    leaf_intf = evcBridge->intf.intf_id;
  else
    leaf_intf = evcBridge->intf.intf_id + PTIN_SYSTEM_N_PORTS;

  /* Validate leaf interface (from received message) */
  if ((leaf_intf >= PTIN_SYSTEM_N_INTERF) ||
      (!evcs[evc_id].intf[leaf_intf].in_use) ||
      (evcs[evc_id].intf[leaf_intf].type != PTIN_EVC_INTF_LEAF))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: %s# %u is invalid", evc_id,
            evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id);
    return L7_FAILURE;
  }

  /* Get root port */
  root_intf = PTIN_SYSTEM_N_INTERF;
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
  {
    if (evcs[evc_id].intf[i].in_use &&
        evcs[evc_id].intf[i].type == PTIN_EVC_INTF_ROOT)
    {
      root_intf = i;
      break;
    }
  }
  if (root_intf >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: No root port was found", evc_id);
    return L7_FAILURE;
  }

  /* Check if client entry already exists */
  ptin_evc_find_client(evcBridge->inn_vlan, &evcs[evc_id].intf[leaf_intf].clients, (dl_queue_elem_t**) &pclient);
  if (pclient != NULL)
  {
    if (ptin_debug_evc)
      PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: %s# %u already have a bridge with Inner VID = %u", evc_id,
                evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id, evcBridge->inn_vlan);
    return L7_SUCCESS;
  }

  /* Check if there is available clients */
  if (queue_free_clients.n_elems == 0)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: No available clients", evc_id);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: adding bridge [Root: Intf=%u IntVID=%u] <=> [Leaf: Intf=%u NEW Out.VID=%u Inn.VID=%u] ...", evc_id,
           root_intf, evcs[evc_id].rvlan, leaf_intf, evcBridge->intf.vid, evcBridge->inn_vlan);

  /* Add translation rules */
  #if ( !PTIN_BOARD_IS_MATRIX )
  /* Remove inner vlan @ egress */
  rc = switching_elan_leaf_add(leaf_intf, evcBridge->intf.vid, evcBridge->inn_vlan, evcs[evc_id].rvlan, L7_TRUE, -1);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding translations for leaf interface %u (rc=%d)",leaf_intf, rc);
    return L7_FAILURE;
  }
  #endif

  /* Only make cross-connections, if EVC is stacked (1:1) */
  if (IS_EVC_STD_P2P(evc_id))
  {
    /* Add bridge between root and leaf port (Proot, Vr, Pleaf, Vs', Vc) */
    rc = switching_p2p_bridge_add(root_intf, evcs[evc_id].rvlan, leaf_intf, evcBridge->intf.vid, evcBridge->inn_vlan);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error adding bridge", evc_id,
              evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id);
      return L7_FAILURE;
    }
  }

  /* SEM CLIENTS UP */
  osapiSemaTake(ptin_evc_clients_sem, L7_WAIT_FOREVER);

  /* Add client to the EVC struct */
  dl_queue_remove_head(&queue_free_clients, (dl_queue_elem_t**) &pclient);  /* get a free client entry */
  pclient->in_use     = L7_TRUE;                                            /* update it */
  pclient->int_ovid   = evcs[evc_id].intf[leaf_intf].int_vlan;
  pclient->int_ivid   = evcBridge->inn_vlan;
  pclient->uni_ovid   = evcBridge->intf.vid;
  pclient->uni_ivid   = 0;
  pclient->client_vid = evcBridge->inn_vlan;
  /* Save protocol enable flags */
  pclient->flags    = evcs[evc_id].flags & (PTIN_EVC_MASK_IGMP_PROTOCOL | PTIN_EVC_MASK_DHCPV4_PROTOCOL | PTIN_EVC_MASK_PPPOE_PROTOCOL);
  /* No vlans to be flooded */
  memset( pclient->flood_vlan, 0x00, sizeof(pclient->flood_vlan));
  for (i=0; i<L7_COS_INTF_QUEUE_MAX_COUNT; i++) {
      pclient->bwprofile[PTIN_EVC_INTF_ROOT][i] = L7_NULLPTR;
      pclient->bwprofile[PTIN_EVC_INTF_LEAF][i] = L7_NULLPTR;
  }
  pclient->counter  [PTIN_EVC_INTF_ROOT] = L7_NULLPTR;
  pclient->counter  [PTIN_EVC_INTF_LEAF] = L7_NULLPTR;
  dl_queue_add_tail(&evcs[evc_id].intf[leaf_intf].clients, (dl_queue_elem_t*) pclient); /* add it to the corresponding interface */
  evcs[evc_id].n_clientflows++;

  /* SEM CLIENTS DOWN */
  osapiSemaGive(ptin_evc_clients_sem);

  /* Update client direct referencing */
  //if (evcBridge->inn_vlan<4096)
  //  evcs[evc_id].client_ref[evcBridge->inn_vlan] = pclient;
  
  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: bridge successfully added", evc_ext_id);
#endif /* !PTIN_BOARD_IS_DNX */

  return L7_SUCCESS;
}

/**
 * Removes a bridge from a stacked EVC between the root and a particular interface 
 *  
 * NOTE: if there is no bridge, L7_SUCCESS is returned anyway
 * 
 * @param evcBridge Bridge info
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_p2p_bridge_remove(ptin_HwEthEvcBridge_t *evcBridge)
{
#if (!PTIN_BOARD_IS_DNX)
  L7_uint evc_id, evc_ext_id;
  L7_uint root_intf;
  L7_uint leaf_intf;
  L7_uint i;
  L7_RC_t rc = L7_SUCCESS;
  struct ptin_evc_client_s *pclient;

  evc_ext_id = evcBridge->index;

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing eEVC# %u bridge connection...", evc_ext_id);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* Check if the EVC is P2P or P2MP */
  if (!IS_EVC_STD(evc_id))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u is QUATTRO?", evc_id);
    return L7_FAILURE;
  }

  /* Check if the EVC is stacked */
  if (!IS_EVC_STACKED(evc_id))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u is not stacked!!!", evc_id);
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
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: %s# %u is invalid", evc_id,
            evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id);
    return L7_FAILURE;
  }
  if ( !evcs[evc_id].intf[leaf_intf].in_use )
  {
    PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: %s# %u is not active in this EVC", evc_id,
            evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id);
    return L7_NOT_EXIST;
  }
  if ( evcs[evc_id].intf[leaf_intf].type != PTIN_EVC_INTF_LEAF )
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: %s# %u is not a leaf interface", evc_id,
            evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id);
    return L7_FAILURE;
  }

  /* Get root port */
  root_intf = PTIN_SYSTEM_N_INTERF;
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
  {
    if (evcs[evc_id].intf[i].in_use &&
        evcs[evc_id].intf[i].type == PTIN_EVC_INTF_ROOT)
    {
      root_intf = i;
      break;
    }
  }
  if (root_intf >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: No root port was found", evc_id);
    return L7_FAILURE;
  }

  /* Get client entry */
  ptin_evc_find_client(evcBridge->inn_vlan, &evcs[evc_id].intf[leaf_intf].clients, (dl_queue_elem_t**) &pclient);
  if (pclient == NULL)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: %s# %u does not have any bridge with Inner VID = %u", evc_id,
                evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id, evcBridge->inn_vlan);
    return L7_NOT_EXIST;
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: deleting bridge [Root: Intf=%u IntVID=%u] <=> [Leaf: Intf=%u NEW Out.VID=%u Inn.VID=%u] ...", evc_id,
           root_intf, evcs[evc_id].rvlan, leaf_intf, pclient->uni_ovid, pclient->int_ivid);

  /* Check if there are counters attached */
  #ifdef EVC_COUNTERS_REQUIRE_CLEANUP_BEFORE_REMOVAL
  if ( pclient->counter[PTIN_EVC_INTF_ROOT]!=NULL || pclient->counter[PTIN_EVC_INTF_LEAF]!=NULL )
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u have counters attached... please, remove them first!", evc_id);
    return L7_FAILURE;
  }
  #endif
  /* Check if there are bw profiles attached */
  #ifdef EVC_BWPROFILES_REQUIRE_CLEANUP_BEFORE_REMOVAL
  for (i=0; i<L7_COS_INTF_QUEUE_MAX_COUNT; i++) {
      if ( pclient->bwprofile[PTIN_EVC_INTF_ROOT][i]!=NULL || pclient->bwprofile[PTIN_EVC_INTF_LEAF][i]!=NULL )
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u have profiles attached... please, remove them first!", evc_id);
        return L7_FAILURE;
      }
  }
  #endif

  /* Remove profiles and counters to this client */
  if ( ptin_evc_pclientFlow_clean(evc_id, leaf_intf, pclient, L7_TRUE) != L7_SUCCESS )
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: can't remove profiles and counters to client!", evc_id);
    return L7_FAILURE;
  }

  #if ( !PTIN_BOARD_IS_MATRIX )
  rc = switching_elan_leaf_remove(leaf_intf, pclient->uni_ovid, pclient->int_ivid, evcs[evc_id].rvlan);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error removing translations for leaf interface %u (rc=%d)",leaf_intf, rc);
    return L7_FAILURE;
  }
  #endif

  /* Only remove cross-connections, if EVC is stacked (1:1) */
  if (IS_EVC_STD_P2P(evc_id))
  {
    /* Delete bridge between root and leaf port (Proot, Vr, Pleaf, Vs', Vc) */
    rc = switching_p2p_bridge_remove(root_intf, evcs[evc_id].rvlan, leaf_intf, pclient->uni_ovid, pclient->int_ivid);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error deleting bridge", evc_id,
              evcBridge->intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcBridge->intf.intf_id);
      return L7_FAILURE;
    }
  }

  /* SEM CLIENTS UP */
  osapiSemaTake(ptin_evc_clients_sem, L7_WAIT_FOREVER);

  /* Delete client from the EVC struct */
  dl_queue_remove(&evcs[evc_id].intf[leaf_intf].clients, (dl_queue_elem_t*) pclient);
  pclient->in_use     = L7_FALSE;
  pclient->int_ovid   = 0;
  pclient->int_ivid   = 0;
  pclient->uni_ovid   = 0;
  pclient->uni_ivid   = 0;
  pclient->client_vid = 0;
  pclient->flags      = 0;
  dl_queue_add_tail(&queue_free_clients, (dl_queue_elem_t*) pclient);
  evcs[evc_id].n_clientflows--;

  /* SEM CLIENTS DOWN */
  osapiSemaGive(ptin_evc_clients_sem);

  /* Update client direct referencing */
  //if (evcBridge->inn_vlan<4096)
  //  evcs[evc_id].client_ref[evcBridge->inn_vlan] = L7_NULLPTR;

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: bridge successfully removed", evc_ext_id);
#endif /* !PTIN_BOARD_IS_DNX */

  return L7_SUCCESS;
}

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED

#define INTF_VP_MAX   PTIN_SYSTEM_N_CLIENTS

#define INVALID_INTF_VP(pentry)     ((pentry)->vport_id == (unsigned long) -1)
#define EMPTY_INTF_VP               INVALID_INTF_VP
#define INVALIDATE_INTF_VP(pentry)  \
  { \
    memset((pentry), 0x00, sizeof(intf_vp_entry_t));  \
    (pentry)->vport_id = (unsigned long) -1;          \
  }

/* For INTF_VP_MAX == 8192, modu is defined as 8209.
   Because M=(intf_vp_modu%INTF_VP_MAX)=17, result was always between 0 and 16...
   something is not right with these operations... */
//#define vportId__2__i(vp, M) ( ((vp)^(vp)<<24) % M)

/* Because vp tends to be between 0 and 8191, the following operation gives us fast searching procedures: */
#define vportId__2__i(vp, M) ((vp)%(INTF_VP_MAX))

//static unsigned char invnibble[16]={0, 8, 4, 0xc, 2, 0xa, 6, 0xe, 1, 9, 5, 0xd, 3, 0xb, 7, 0xf};
//#define vportId__2__i(IfN, M) ( ((IfN) ^ invnibble[IfN&0xf]<<28 ^ invnibble[IfN>>4&0xf]<<24 ^ invnibble[IfN>>8&0xf]<<20) % M)

static intf_vp_entry_t  intf_vp_table[INTF_VP_MAX];
static unsigned long    intf_vp_n = 0;
static unsigned long    intf_vp_modu = INTF_VP_MAX;

static int              intf_vp_policer(intf_vp_entry_t *intf_vp, ptin_bw_meter_t *meter);
static intf_vp_entry_t *intf_vp_get(L7_uint32 vport_id);

int intf_vp_DB(int _0init_1insert_2remove_3find, intf_vp_entry_t *entry)
{
  
  unsigned long i, j, k, _1st_empty;

  switch (_0init_1insert_2remove_3find) {
  default: return 1;
  case 0:
     intf_vp_n=0;
     for (i=0; i<INTF_VP_MAX; i++) INVALIDATE_INTF_VP(&intf_vp_table[i])

     for (intf_vp_modu=INTF_VP_MAX; 1;) {                                     //Just to improve modulus
         for (i=2; i*i<intf_vp_modu; i++) if (0==intf_vp_modu%i) break;
         if (i*i>=intf_vp_modu) break;
         intf_vp_modu++;
     }
     PT_LOG_INFO(LOG_CTX_EVC, "IfN_vp_DB init(%d)\tN=%lu\tmodu=%lu\tL7_MAX_INTERFACE_COUNT=%lu", _0init_1insert_2remove_3find, INTF_VP_MAX, intf_vp_modu, L7_MAX_INTERFACE_COUNT);
     break;
  case 1:
  case 2:
  case 3:
     i=vportId__2__i(entry->vport_id, intf_vp_modu%INTF_VP_MAX);
     for (j=0, k=i, _1st_empty=-1;  j<INTF_VP_MAX;  j++) {
         if (entry->vport_id==intf_vp_table[k].vport_id) {i=k; break;}
         if (_1st_empty>=INTF_VP_MAX && EMPTY_INTF_VP(&intf_vp_table[k])) _1st_empty=k;
         if (++k>=INTF_VP_MAX) k=0;
     }
     PT_LOG_TRACE(LOG_CTX_EVC, "IfN_vp_DB (_0init_1insert_2remove_3find=%d)\ti=%lu j=%lu k=%lu\t_1st_empty=%lu\tn=%lu", _0init_1insert_2remove_3find, i,j,k, _1st_empty, intf_vp_n);
     if (j>=INTF_VP_MAX) {//(entry->vport_id!=intf_vp_table[i].vport_id) {//didn't find it
         if (3==_0init_1insert_2remove_3find) return 2;
         if (2==_0init_1insert_2remove_3find) return 0;

         //1==_0init_1insert_2remove_3find
         if (_1st_empty>=INTF_VP_MAX) return 3; //if (!EMPTY_IfN_VP(&intf_vp_table[i])) return 3; //if (INTF_VP_MAX==intf_vp_n) return 3;       //no empty entries
         intf_vp_n++;
         intf_vp_table[_1st_empty]=*entry;
         memset(&intf_vp_table[_1st_empty].policer, 0x00, sizeof(intf_vp_entry_policer_t));   /* No policer for new entries */
     }
     else {                                             //did find it
         if (3==_0init_1insert_2remove_3find) {*entry=intf_vp_table[i]; return 0;}
         if (2==_0init_1insert_2remove_3find)
         {
           intf_vp_policer(&intf_vp_table[i], L7_NULLPTR);  /* Remove policer */
           INVALIDATE_INTF_VP(&intf_vp_table[i]);           /* Empty entry */
           intf_vp_n--;
           return 0;
         }

         //1==_0init_1insert_2remove_3find
         entry->policer = intf_vp_table[i].policer;   /* Copy policer data to this entry */
         intf_vp_table[i]=*entry;                     //overwrite
     }
     break;
  case 4:
     printf("Dumping configured virtual ports:\n\r");
     for (i=0; i<INTF_VP_MAX; i++) {
         if (EMPTY_INTF_VP(&intf_vp_table[i])) continue;
         printf(" <%4lu> vport_id=%-4lu pon=%u/%-2u gem_id=%-4u\n\r", i, intf_vp_table[i].vport_id, intf_vp_table[i].pon.intf_type, intf_vp_table[i].pon.intf_id, intf_vp_table[i].gem_id);
     }
     printf("Number of virtual ports: %lu\n\r", intf_vp_n);
     break;
  }//switch

  fflush(stdout);

  return 0;
}//IfN_vp_DB

/**
 * Set bandwidth policer for one virtual port
 * 
 * @param intf_vp
 * @param meter 
 * 
 * @return int : 0>Success, -1>Failed
 */
L7_RC_t ptin_evc_vp_policer(L7_uint32 vport_id, ptin_bw_meter_t *meter)
{
  intf_vp_entry_t *intf_vp;

  intf_vp = intf_vp_get(vport_id);

  if (intf_vp == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_L2, "Error getting pointer tp vp entry (vp 0x%x)", vport_id);
    return L7_FAILURE;
  }

  /* Process policer */
  return intf_vp_policer(intf_vp, meter);
}

/**
 * Determine vport from pon port and gem id
 * 
 * @param pon_port
 * @param gem_id 
 * 
 * @return vport_id (output) 
 */
L7_uint32 intf_vp_calc(L7_uint16 pon_port, L7_uint16 gem_id)
{
  /* Search for this virtual port */
  L7_uint32         i;
  ptin_intf_t       ptin_intf;

  /* Validate arguments */
  if (pon_port >= PTIN_SYSTEM_N_PORTS || gem_id == 0 || gem_id >= 4096)
  {
    PT_LOG_ERR(LOG_CTX_L2, "Invalid parameters: pon_port=%u gem_id=%u", pon_port, gem_id);
    return (L7_uint32) -1;
  }

  /* Get USP of PON port */
  if (ptin_intf_port2ptintf(pon_port, &ptin_intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_L2, "Cannot ptin_intf from ptin_port %u", pon_port);
    return (L7_uint32) -1;
  }

  /* Search for given port and gem_id */
  for (i = 0; i < INTF_VP_MAX; i++)
  {
    /* Skip non used entries */
    if (INVALID_INTF_VP(&intf_vp_table[i]))
      continue;

    /* find pon port and vlan */
    if (intf_vp_table[i].pon.intf_type == ptin_intf.intf_type && intf_vp_table[i].pon.intf_id == ptin_intf.intf_id &&
        intf_vp_table[i].gem_id == gem_id)
      break;
  }
  if (i >= INTF_VP_MAX)
  {
    PT_LOG_WARN(LOG_CTX_L2, "Matched entry not found: pon_port=%u gem_id=%u", pon_port, gem_id);
    return (L7_uint32) -1;
  }

  /* Return result */
  return intf_vp_table[i].vport_id;
}

/**
 * Find a particular entry inside virtual port list
 * 
 * @param vport_id 
 * 
 * @return entry pointer
 */
static intf_vp_entry_t *intf_vp_get(L7_uint32 vport_id)
{
  unsigned long i, j, k;

  /* Search for this virtual port */
  i = vportId__2__i(vport_id, intf_vp_modu%INTF_VP_MAX);

  for (j=0, k=i;  j<INTF_VP_MAX;  j++)
  {
      if (vport_id==intf_vp_table[k].vport_id) {i=k; break;}
      if (++k>=INTF_VP_MAX) k=0;
  }

  /* Not found? */
  if (j>=INTF_VP_MAX)
  {
    return L7_NULLPTR;
  }

  return &intf_vp_table[i];
}

/**
 * Set bandwidth policer for one virtual port
 * 
 * @param intf_vp
 * @param meter 
 * 
 * @return int : 0>Success, -1>Failed
 */
static int intf_vp_policer(intf_vp_entry_t *intf_vp, ptin_bw_meter_t *meter)
{
  /* Search for this virtual port */
  L7_uint32         i;
  L7_uint16         vlanId;
  ptin_bw_profile_t profile;
  ptin_bw_meter_t   meter_info;
  L7_int            policer_id = -1;
  L7_uchar8         keyNext[L7_FDB_KEY_SIZE];
  L7_INTF_TYPES_t   intfType;
  dot1dTpFdbData_t  fdbEntry;
  L7_RC_t           rc = L7_SUCCESS;

  /* Validate arguments */
  if (intf_vp == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_L2, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Fill structure for policer */
  memset(&profile, 0x00, sizeof(profile));
  memset(&meter_info, 0x00, sizeof(meter_info));

  /* Set meter profile */
  if (meter != L7_NULLPTR && meter->cir != (L7_uint32)-1)
  {
    policer_id = (intf_vp->policer.in_use) ? intf_vp->policer.policer_id : -1;

    PT_LOG_TRACE(LOG_CTX_EVC,"Creating policer (%d)", policer_id);

    /* Add this policer to all MAC entries */
    meter_info.cir = meter->cir;
    meter_info.eir = meter->eir;
    meter_info.cbs = meter->cbs;
    meter_info.ebs = meter->ebs;

    /* Create/modify policer */
    if (ptin_bwPolicer_create(&meter_info, &policer_id) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_L2, "Error creating new policer id");
      return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_EVC,"Policer %d created", policer_id);

    /* Save meter data */
    intf_vp->policer.meter      = *meter;
    intf_vp->policer.policer_id = policer_id;
    /* New policer */
    intf_vp->policer.in_use     = L7_TRUE;
  }
  else
  {
    /* Check if policer exists */
    if (!intf_vp->policer.in_use)
    {
      PT_LOG_WARN(LOG_CTX_L2, "vport 0x%x already does not have policer", intf_vp->vport_id);
      return L7_SUCCESS;
    }

    PT_LOG_TRACE(LOG_CTX_EVC,"Going to remove policer");

    /* Remove this policer to all MAC entries */
    meter_info.cir = (L7_uint32) -1;
    meter_info.eir = (L7_uint32) -1;
    meter_info.cbs = (L7_uint32) -1;
    meter_info.ebs = (L7_uint32) -1;

    /* Policer id in use */
    policer_id = intf_vp->policer.policer_id;
  }

  PT_LOG_TRACE(LOG_CTX_EVC,"Running L2 table");

  memset( keyNext, 0x00, sizeof(L7_uchar8)*L7_FDB_KEY_SIZE );

  /* Run all MAC entries */
  for (i=0; i<PLAT_MAX_FDB_MAC_ENTRIES && fdbFind(keyNext, L7_MATCH_GETNEXT, &fdbEntry)==L7_SUCCESS; i++)
  {
    memcpy(keyNext, fdbEntry.dot1dTpFdbAddress, L7_FDB_KEY_SIZE);

    /* Get interface type, and only consider virtual ports */
    if (nimGetIntfType(fdbEntry.dot1dTpFdbPort, &intfType) != L7_SUCCESS)
    {
      PT_LOG_TRACE(LOG_CTX_L2, "Cannot get intfType for intIfNum %u", fdbEntry.dot1dTpFdbPort);
      rc = L7_FAILURE;
      continue;
    }
    /* Skip non virtual port entries */
    if (intfType != L7_VLAN_PORT_INTF)
    {
      continue;
    }

    /* Check for virtual port id */
    if (intf_vp->vport_id != fdbEntry.dot1dTpFdbVirtualPort)
    {
      continue;
    }

    vlanId = ((L7_uint16) fdbEntry.dot1dTpFdbAddress[0]<<8) | ((L7_uint16) fdbEntry.dot1dTpFdbAddress[1]);

    /* Apply policer to current MAC address */
    profile.ptin_port           = -1;
    profile.outer_vlan_ingress  = vlanId;
    profile.cos                 = (L7_uint8) -1;
    memcpy(profile.macAddr, &fdbEntry.dot1dTpFdbAddress[L7_FDB_IVL_ID_LEN], sizeof(L7_uint8)*L7_MAC_ADDR_LEN);

    PT_LOG_TRACE(LOG_CTX_L2, "Processing vlan %u, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
              vlanId, profile.macAddr[0], profile.macAddr[1], profile.macAddr[2], profile.macAddr[3], profile.macAddr[4], profile.macAddr[5]);

    /* Apply policer*/
    if (ptin_bwPolicer_set(&profile, &meter_info, policer_id) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_L2, "Error applying profile to vlan %u, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
              vlanId, profile.macAddr[0], profile.macAddr[1], profile.macAddr[2], profile.macAddr[3], profile.macAddr[4], profile.macAddr[5]);
      rc = L7_FAILURE;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_L2, "Success applying profile to vlan %u, MAC=%02x:%02x:%02x:%02x:%02x:%02x (policer_id=%d)",
                vlanId, profile.macAddr[0], profile.macAddr[1], profile.macAddr[2], profile.macAddr[3], profile.macAddr[4], profile.macAddr[5], policer_id);
    }
  }

  /* If success... */
  if (rc == L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_L2, "Success updating policer to virtual port 0x%08x",intf_vp->vport_id);

    /* Remove policer, if it was that intention */
    if (meter == L7_NULLPTR || meter->cir == (L7_uint32)-1)
    {
      PT_LOG_TRACE(LOG_CTX_EVC,"Policer %d", intf_vp->policer.policer_id);

      if (intf_vp->policer.policer_id > 0)
      {
        PT_LOG_TRACE(LOG_CTX_EVC,"Going to destroy policer %d", intf_vp->policer.policer_id);

        /* Destroy policer */
        if (ptin_bwPolicer_destroy(intf_vp->policer.policer_id) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_L2, "Error destroying policer id %d", intf_vp->policer.policer_id);
          return L7_FAILURE;
        }

        PT_LOG_TRACE(LOG_CTX_EVC,"Policer %d removed", intf_vp->policer.policer_id);
      }

      /* Clean data */
      memset(&intf_vp->policer, 0x00, sizeof(intf_vp_entry_policer_t));
      intf_vp->policer.in_use = L7_FALSE;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_L2, "Error updating policer to virtual port 0x%08x",intf_vp->vport_id);
  }

  PT_LOG_TRACE(LOG_CTX_EVC,"Finished");

  return L7_SUCCESS;
}


//#undef INTF_VP_MAX

void dump_intf_vp_db(void)
{
  intf_vp_DB(4,NULL);
}

#endif

/*********************************Multicast Channel Packages Feature*****************************/

/**
 * Macbridge Flow Client Packages Add
 * 
 * @param ecvFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_macbridge_client_packages_add(ptin_evc_macbridge_client_packages_t *ecvFlow)
{
  L7_uint32   evc_id; 
  L7_uint32   evc_ext_id;
  L7_uint32   leaf_port;
  L7_uint32   intIfNum; 

  /* Input Argument validation */
  if ( ecvFlow  == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid arguments [msg:%p noOfMessages:%u]",ecvFlow);    
    return L7_FAILURE;
  }
  
  evc_ext_id = ecvFlow->evc_idx;
  PT_LOG_TRACE(LOG_CTX_EVC, "Adding eEVC# %u flow connection...", evc_ext_id);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_id);
    return L7_FAILURE;
  }

  /* Determine leaf ptin_intf */
  if (ecvFlow->ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL)
    leaf_port = ecvFlow->ptin_intf.intf_id;
  else
    leaf_port = ecvFlow->ptin_intf.intf_id + PTIN_SYSTEM_N_PORTS;

  /* Validate leaf interface (from received message) */
  if ((leaf_port >= PTIN_SYSTEM_N_INTERF) ||
      (!evcs[evc_id].intf[leaf_port].in_use) ||
      (evcs[evc_id].intf[leaf_port].type != PTIN_EVC_INTF_LEAF))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: %s# %u is invalid", evc_id,
            ecvFlow->ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", ecvFlow->ptin_intf.intf_id);
    return L7_FAILURE;
  }

  /* Convert to intIfNum */
  if (ptin_intf_port2intIfNum(leaf_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Cannot get intIfNum from port %u", evc_id, leaf_port);
    return L7_FAILURE;
  }

  /* Only for QUATTRO serices */
  if (!IS_EVC_QUATTRO(evc_id))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Flows are only applied to QUATTRO services", evc_id);
    return L7_FAILURE;
  }

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  L7_uint                   int_ovid;
  ptin_client_id_t          clientId;
  struct ptin_evc_client_s *pflow;
  L7_BOOL                   addOrRemove = L7_FALSE;//Add Packages
   L7_RC_t                  rc = L7_SUCCESS;

  /* Get internal vlan and inner NNI vlan */
  int_ovid = evcs[evc_id].intf[leaf_port].int_vlan;

  /* Check if flow entry already exists */
  ptin_evc_find_flow(ecvFlow->uni_ovid, &evcs[evc_id].intf[leaf_port].clients, (dl_queue_elem_t**) &pflow);

  if (pflow == NULL)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Flow does not exist", evc_id);
    return L7_DEPENDENCY_NOT_MET;
  }

  /* Client id: For IGMP */
  memset(&clientId, 0x00, sizeof(clientId));
  clientId.ptin_intf.intf_type  = ecvFlow->ptin_intf.intf_type;
  clientId.ptin_intf.intf_id    = ecvFlow->ptin_intf.intf_id;
  clientId.outerVlan            = pflow->int_ovid;
  clientId.innerVlan            = pflow->int_ivid;
  clientId.mask                 = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_OUTERVLAN | PTIN_CLIENT_MASK_FIELD_INNERVLAN;

  /* Add client */
  if ( (rc = ptin_igmp_group_client_add(&clientId, pflow->uni_ovid, pflow->uni_ivid, ecvFlow->onuId, 0x00, 0, 0, addOrRemove, ecvFlow->packageBmpList, ecvFlow->noOfPackages) != L7_SUCCESS) )
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error adding client to IGMP instance", evc_id);
    return rc;
  }
  return rc;
#else
  PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u: Flows not available for this board", evc_id);
  return L7_ERROR;
#endif  
}

/**
 * Macbridge Flow Client Packages Remove
 * 
 * @param ecvFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_macbridge_client_packages_remove(ptin_evc_macbridge_client_packages_t *ecvFlow)
{
  L7_uint32   evc_id; 
  L7_uint32   evc_ext_id;
  L7_uint32   leaf_port;
  L7_uint32   intIfNum;
    
  /* Input Argument validation */
  if ( ecvFlow  == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid arguments [msg:%p noOfMessages:%u]",ecvFlow);    
    return L7_FAILURE;
  }

  evc_ext_id = ecvFlow->evc_idx;
  PT_LOG_TRACE(LOG_CTX_EVC, "Adding eEVC# %u flow connection...", evc_ext_id);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_id);
    return L7_NOT_EXIST;
  }

  /* Determine leaf ptin_intf */
  if (ecvFlow->ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL)
    leaf_port = ecvFlow->ptin_intf.intf_id;
  else
    leaf_port = ecvFlow->ptin_intf.intf_id + PTIN_SYSTEM_N_PORTS;

  /* Validate leaf interface (from received message) */
  if ((leaf_port >= PTIN_SYSTEM_N_INTERF) ||
      (!evcs[evc_id].intf[leaf_port].in_use) ||
      (evcs[evc_id].intf[leaf_port].type != PTIN_EVC_INTF_LEAF))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: %s# %u is invalid", evc_id,
            ecvFlow->ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", ecvFlow->ptin_intf.intf_id);
    return L7_FAILURE;
  }

  /* Convert to intIfNum */
  if (ptin_intf_port2intIfNum(leaf_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Cannot get intIfNum from port %u", evc_id, leaf_port);
    return L7_FAILURE;
  }

  /* Only for QUATTRO serices */
  if (!IS_EVC_QUATTRO(evc_id))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Flows are only applied to QUATTRO services", evc_id);
    return L7_FAILURE;
  }

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  L7_uint                   int_ovid;
  ptin_client_id_t          clientId;
  struct ptin_evc_client_s *pflow;
  L7_BOOL                   addOrRemove = L7_TRUE;//Remove Packages
  L7_RC_t                   rc = L7_SUCCESS;

  /* Get internal vlan and inner NNI vlan */
  int_ovid = evcs[evc_id].intf[leaf_port].int_vlan;

  /* Check if flow entry already exists */
  ptin_evc_find_flow(ecvFlow->uni_ovid, &evcs[evc_id].intf[leaf_port].clients, (dl_queue_elem_t**) &pflow);

  if (pflow == NULL)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Flow does not exist", evc_id);
    return L7_DEPENDENCY_NOT_MET;
  }

  /* Client id: For IGMP */
  memset(&clientId, 0x00, sizeof(clientId));
  clientId.ptin_intf.intf_type  = ecvFlow->ptin_intf.intf_type;
  clientId.ptin_intf.intf_id    = ecvFlow->ptin_intf.intf_id;
  clientId.outerVlan            = pflow->int_ovid;
  clientId.innerVlan            = pflow->int_ivid;
  clientId.mask                 = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_OUTERVLAN | PTIN_CLIENT_MASK_FIELD_INNERVLAN;

  /* Add client */
  if ( (rc = ptin_igmp_group_client_add(&clientId, pflow->uni_ovid, pflow->uni_ivid, ecvFlow->onuId, 0x0C, 0, 0, addOrRemove, ecvFlow->packageBmpList, ecvFlow->noOfPackages) != L7_SUCCESS) )
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error adding client to IGMP instance", evc_id);
    return rc;
  }
  return rc;
#else
  PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u: Flows not available for this board", evc_id);
  return L7_ERROR;
#endif  
}

/*****************************End Multicast Channel Packages Feature*****************************/

/**
 * Adds a flow to the EVC
 * 
 * @param evcFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_flow_add(ptin_HwEthEvcFlow_t *evcFlow)
{
  L7_uint   evc_id, evc_ext_id;
  L7_uint   leaf_port;

  evc_ext_id = evcFlow->evc_idx;

  PT_LOG_TRACE(LOG_CTX_EVC, "Adding eEVC# %u flow connection...", evc_ext_id);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_id);
    return L7_DEPENDENCY_NOT_MET;
  }

  /* Determine leaf ptin_intf */
  if (evcFlow->ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL)
    leaf_port = evcFlow->ptin_intf.intf_id;
  else
    leaf_port = evcFlow->ptin_intf.intf_id + PTIN_SYSTEM_N_PORTS;

  /* Validate leaf interface (from received message) */
  if ((leaf_port >= PTIN_SYSTEM_N_INTERF) ||
      (!evcs[evc_id].intf[leaf_port].in_use) ||
      (evcs[evc_id].intf[leaf_port].type != PTIN_EVC_INTF_LEAF))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: %s# %u is invalid", evc_id,
            evcFlow->ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", evcFlow->ptin_intf.intf_id);
    return L7_FAILURE;
  }

  /* Only for QUATTRO serices */
  if (!IS_EVC_QUATTRO(evc_id))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Flows are only applied to QUATTRO services", evc_id);
    return L7_FAILURE;
  }

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  /* Configure flow */
  if (ptin_evc_flow_config(evcFlow) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error configuring flows", evc_id);
    return L7_FAILURE;
  }
#else
  PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u: Flows not available for this board", evc_id);
  return L7_ERROR;
#endif

  return L7_SUCCESS;
}

/**
 * Removes a flow from the EVC
 * 
 * @param evcFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_flow_remove(ptin_HwEthEvcFlow_t *evcFlow)
{
  L7_uint   evc_id, evc_ext_id;
  L7_RC_t   rc = L7_SUCCESS;

  evc_ext_id = evcFlow->evc_idx;

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing eEVC# %u flow connection...", evc_ext_id);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_DEPENDENCY_NOT_MET;
  }

  evcFlow->evc_idx = evc_id;

  /* Only for QUATTRO serices */
  if (!IS_EVC_QUATTRO(evc_id))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Flows are only applied to QUATTRO services", evc_id);
    return L7_FAILURE;
  }

  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  /* Remove flow */
  rc = ptin_evc_flow_unconfig(evcFlow);
  #endif

  return rc;
}

#if (PTIN_BOARD_IS_DNX || PTIN_QUATTRO_FLOWS_FEATURE_ENABLED)
/**
 * Flow configuration
 * 
 * @author mruas (11/13/2015)
 * 
 * @param evcFlow 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t ptin_evc_flow_config(ptin_HwEthEvcFlow_t *evcFlow)
{
  L7_uint32 evc_id, ptin_port, intIfNum;
  L7_int    vport_id, multicast_group;
  L7_uint16 int_ovid;
  L7_BOOL   igmp_enabled, dhcpv4_enabled, dhcpv6_enabled, pppoe_enabled;
  /* Always add client */
  ptin_client_id_t clientId;
  struct ptin_evc_client_s *pflow;
  L7_RC_t   rc;
  
  /* Only physical ports */
  if (evcFlow->ptin_intf.intf_type != PTIN_EVC_INTF_PHYSICAL)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Only physical interfaces are supported (%u/%u)!", evcFlow->ptin_intf.intf_type, evcFlow->ptin_intf.intf_id);
    return L7_FAILURE;
  }
  /* Validate interface */
  if (ptin_intf_ptintf2port(&evcFlow->ptin_intf, &ptin_port) != L7_SUCCESS ||
      ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Cannot get ptin_port or intIfNum from intf %u/%u", evcFlow->ptin_intf.intf_type, evcFlow->ptin_intf.intf_id);
    return L7_FAILURE;
  }
  /* Validate EVC id */
  if (evcFlow->evc_idx >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid EVC id %u", evcFlow->evc_idx);
    return L7_FAILURE;
  }

  /* Internal EVC id */
  evc_id = evcFlow->evc_idx;

  /* EVC is active? */
  if (!evcs[evc_id].in_use) 
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u not in use", evc_id);
    return L7_FAILURE;
  }
  /* Port should be active within EVC */
  if (!evcs[evc_id].intf[ptin_port].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: ptin_port %u not active within current EVC", evc_id, ptin_port);
    return L7_FAILURE;
  }

  /* Get internal vlan and inner NNI vlan */
  int_ovid = evcs[evc_id].intf[ptin_port].int_vlan;

  /* Multicast group */
  multicast_group = evcs[evc_id].multicast_group;

  /* Check if flow entry already exists */
  ptin_evc_find_flow(evcFlow->uni_ovid, &evcs[evc_id].intf[ptin_port].clients, (dl_queue_elem_t**) &pflow);

  /* If flow does it, create it */
  if (pflow == NULL)
  {
    /* Check if there is available flows */
    if (queue_free_clients.n_elems == 0)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: No available flows", evc_id);
      return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Going to create new flow (client %u)", evc_id, evcFlow->int_ivid);

  #if (PTIN_BOARD_IS_DNX)
    rc = switching_lif_add(ptin_port,
                           evcFlow->uni_ovid, evcFlow->uni_ivid,
                           ((evcFlow->pcp & PTIN_INTF_PCP_PROVIDED) ? (evcFlow->pcp & 0x7) : -1), evcFlow->etherType,
                           int_ovid, multicast_group,
                           &vport_id);
  #else
    rc = ptin_virtual_port_add(intIfNum,
                               evcFlow->uni_ovid, evcFlow->uni_ivid,
                               int_ovid, evcFlow->int_ivid,
                               ((evcFlow->pcp & PTIN_INTF_PCP_PROVIDED) ? (evcFlow->pcp & 0x7) : -1), evcFlow->etherType,
                               multicast_group,
                               &vport_id,
                               evcFlow->macLearnMax);
  #endif
    /* Create virtual port */
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error creating virtual port", evc_id);
      return L7_FAILURE;
    }
    else
    {
      intf_vp_entry_t e;

      e.vport_id  = vport_id & 0xffffff;
      e.pon       = evcFlow->ptin_intf;
      e.gem_id    = evcFlow->uni_ovid;
      intf_vp_DB(1, &e);
    }

    /* Add client to the EVC struct */
    dl_queue_remove_head(&queue_free_clients, (dl_queue_elem_t**) &pflow);    /* get a free client entry */
    pflow->in_use     = L7_TRUE;                                              /* update it */
    pflow->int_ovid   = int_ovid;
    pflow->int_ivid   = evcFlow->int_ivid;
    pflow->uni_ovid   = evcFlow->uni_ovid;
    pflow->uni_ivid   = evcFlow->uni_ivid;
    pflow->client_vid = evcFlow->uni_ivid;
    pflow->flags      = 0; //evcFlow->flags;
    pflow->virtual_gport = vport_id;
    pflow->vport_id   = vport_id & 0xffffff;
    dl_queue_add_tail(&evcs[evc_id].intf[ptin_port].clients, (dl_queue_elem_t*) pflow); /* add it to the corresponding interface */
    evcs[evc_id].n_clientflows++;

    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: flow successfully added (vport_id=%lu pon=%u/%u(%lu) gem_id=%u virtual_gport=0x%lx)",
             evc_id,
             vport_id & 0xffffff,
             evcFlow->ptin_intf.intf_type,evcFlow->ptin_intf.intf_id, intIfNum,
             evcFlow->uni_ovid, vport_id);
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: GEM id already exists", evc_id, evcFlow->uni_ovid, ptin_port);
  }

  /* Protocols */
  igmp_enabled    = (evcFlow->flags & PTIN_EVC_MASK_IGMP_PROTOCOL  ) == PTIN_EVC_MASK_IGMP_PROTOCOL;
  dhcpv4_enabled  = (evcFlow->flags & PTIN_EVC_MASK_DHCPV4_PROTOCOL) == PTIN_EVC_MASK_DHCPV4_PROTOCOL;
  dhcpv6_enabled  = (evcFlow->flags & PTIN_EVC_MASK_DHCPV6_PROTOCOL) == PTIN_EVC_MASK_DHCPV6_PROTOCOL;
  pppoe_enabled   = (evcFlow->flags & PTIN_EVC_MASK_PPPOE_PROTOCOL ) == PTIN_EVC_MASK_PPPOE_PROTOCOL;

  /* Client id: For IGMP */
  memset(&clientId, 0x00, sizeof(clientId));
  clientId.ptin_intf.intf_type  = evcFlow->ptin_intf.intf_type;
  clientId.ptin_intf.intf_id    = evcFlow->ptin_intf.intf_id;
  clientId.outerVlan            = pflow->int_ovid;
  clientId.innerVlan            = pflow->int_ivid;
  clientId.mask                 = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_OUTERVLAN | PTIN_CLIENT_MASK_FIELD_INNERVLAN;

  /* Manage IGMP client */
  if (evcFlow->flags & PTIN_EVC_MASK_IGMP_PROTOCOL)
  {
    /* Add client */
    if (ptin_igmp_group_client_add(&clientId, pflow->uni_ovid, pflow->uni_ivid, evcFlow->onuId, evcFlow->mask, evcFlow->maxBandwidth, evcFlow->maxChannels, L7_FALSE, evcFlow->packageBmpList, evcFlow->noOfPackages) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error adding client to IGMP instance", evc_id);
      return L7_FAILURE;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Added client to IGMP instance", evc_id);
    }
  }
  else if (evcs[evc_id].flags & PTIN_EVC_MASK_IGMP_PROTOCOL)
  {
    /* Remove client */
    if (ptin_igmp_group_client_remove(&clientId) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing client from IGMP instance", evc_id);
      //rc = L7_FAILURE;    /* L7_NOT_EXIST is not an error */
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Client removed from IGMP instance", evc_id);
    }
  }

  /* ---------------- IGMP ------------------- */
  /* Only configure IGMP for first time configurations */
  if (ptin_evc_update_igmp(evc_id, &pflow->flags, igmp_enabled,
                           L7_FALSE /*Update*/, L7_TRUE /*Look to counters*/) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error configuring IGMP", evc_id);
    ptin_igmp_group_client_remove(&clientId);
    return L7_FAILURE;
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: IGMP configured", evc_id);
  }

  #if (!PTIN_BOARD_IS_MATRIX)
  /* ---------------- DHCP ------------------- */
  /* Only configure DHCP for first time configurations */
  if (ptin_evc_update_dhcp(evc_id, &pflow->flags, dhcpv4_enabled, dhcpv6_enabled,
                           L7_FALSE /*Update*/, L7_TRUE /*Look to counters*/) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error configuring DHCP", evc_id);
    return L7_FAILURE;
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: DHCP configured", evc_id);
  }

  /* ---------------- PPPoE ------------------- */
  /* PPPoE configuration */
  if (ptin_evc_update_pppoe(evc_id, &pflow->flags, pppoe_enabled,
                            L7_FALSE /*Update*/, L7_TRUE /*Look to counters*/) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error configuring PPPoE", evc_id);
    return L7_FAILURE;
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: PPPoE configured", evc_id);
  }
  #endif

  return L7_SUCCESS;
}

/**
 * Unconfigure an EVC flow
 * 
 * @param evc_id 
 * @param ptin_port 
 * @param uni_ovid : external outer vlan (gem id)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_evc_flow_unconfig(ptin_HwEthEvcFlow_t *evcFlow)
{
  L7_uint32 evc_id, ptin_port, intIfNum;
  L7_uint   client_vlan;
  L7_int    multicast_group;
  ptin_intf_t ptin_intf;
  struct ptin_evc_client_s *pflow;
  L7_uint32 evc_ext_id;
  L7_BOOL   igmp_enabled, dhcpv4_enabled, dhcpv6_enabled, pppoe_enabled;
  L7_RC_t   rc = L7_SUCCESS;

  /* Validate arguments */
  if (evcFlow->evc_idx >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid EVC# %u", evcFlow->evc_idx);
    return L7_FAILURE;
  }
  /* Validate interface */
  if (ptin_intf_typeId2port(evcFlow->ptin_intf.intf_type, evcFlow->ptin_intf.intf_id, &ptin_port) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Invalid intf %u/%u", evcFlow->evc_idx, evcFlow->ptin_intf.intf_type, evcFlow->ptin_intf.intf_id);
    return L7_FAILURE;
  }

  evc_id = evcFlow->evc_idx;

  /* EVC and port should be active */
  if (!evcs[evc_id].in_use || !evcs[evc_id].intf[ptin_port].in_use /*|| !evcs[evc_id].intf[ptin_port].flow[flow_id].in_use*/)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u/port %u not active", evc_id, ptin_port);
    return L7_FAILURE;
  }

  evc_ext_id = evcs[evc_id].extended_id;

  /* Convert to intIfNum */
  if (ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS ||
      ptin_intf_port2ptintf(ptin_port, &ptin_intf)  != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Cannot get intIfNum/ptin_intf from port %u", evc_id, ptin_port);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: ptin_port=%u uni_ovid=%u", evc_id, ptin_port, evcFlow->uni_ovid);

  /* Check if flow entry does not exists: do nothing if don't */
  ptin_evc_find_flow(evcFlow->uni_ovid, &evcs[evc_id].intf[ptin_port].clients, (dl_queue_elem_t**) &pflow);
  if (pflow == NULL)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: Flow not found", evc_id);
    return L7_SUCCESS;
  }

  /* Protocols */
  igmp_enabled    = (pflow->flags & PTIN_EVC_MASK_IGMP_PROTOCOL  ) == PTIN_EVC_MASK_IGMP_PROTOCOL;
  dhcpv4_enabled  = (pflow->flags & PTIN_EVC_MASK_DHCPV4_PROTOCOL) == PTIN_EVC_MASK_DHCPV4_PROTOCOL;
  dhcpv6_enabled  = (pflow->flags & PTIN_EVC_MASK_DHCPV6_PROTOCOL) == PTIN_EVC_MASK_DHCPV6_PROTOCOL;
  pppoe_enabled   = (pflow->flags & PTIN_EVC_MASK_PPPOE_PROTOCOL ) == PTIN_EVC_MASK_PPPOE_PROTOCOL;

  /* IGMP / DHCP / PPPoE instance management */
  if (pflow->flags & PTIN_EVC_MASK_IGMP_PROTOCOL)
  {
    /* Always remove client */
    ptin_client_id_t clientId;

    /* Client id */
    memset(&clientId, 0x00, sizeof(clientId));
    clientId.ptin_intf.intf_type  = ptin_intf.intf_type;
    clientId.ptin_intf.intf_id    = ptin_intf.intf_id;
    clientId.outerVlan            = pflow->int_ovid;
    clientId.innerVlan            = pflow->int_ivid;
    clientId.mask                 = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_OUTERVLAN | PTIN_CLIENT_MASK_FIELD_INNERVLAN;

    /* Add client */
    if (ptin_igmp_group_client_remove(&clientId) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing client from IGMP instance", evc_id);
      //rc = L7_FAILURE;    /* L7_NOT_EXIST is not an error */
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Client removed from IGMP instance", evc_id);
    }
  }

  /* Multicast group */
  multicast_group = evcs[evc_id].multicast_group;

  /* Get client inner vlan */
  client_vlan = pflow->int_ivid;

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Going to remove flow related to client %u", evc_id, client_vlan);

  /* Force removal of counters and profiles */
  ptin_evc_pclientFlow_clean(evc_id, ptin_port, pflow, L7_TRUE);

  /* Remove virtual port */
  {
    intf_vp_entry_t e;

    e.vport_id = pflow->vport_id;
    intf_vp_DB(2, &e);
  }

#if (PTIN_BOARD_IS_DNX)
  rc = switching_lif_remove(ptin_port, evcs[evc_id].rvlan, multicast_group, pflow->virtual_gport);
#else
  rc = ptin_virtual_port_remove(intIfNum, pflow->virtual_gport, multicast_group);
#endif
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing virtual port", evc_id);
    return L7_FAILURE;
  }

  /* Delete client from the EVC struct */
  dl_queue_remove(&evcs[evc_id].intf[ptin_port].clients, (dl_queue_elem_t*) pflow);
  /* Clear data from flow */
  memset(pflow, 0x00, sizeof(struct ptin_evc_client_s));
  dl_queue_add_tail(&queue_free_clients, (dl_queue_elem_t*) pflow);
  /* Update number of flows */
  if (evcs[evc_id].n_clientflows > 0)
    evcs[evc_id].n_clientflows--;

  /* Remove IGMP configurations */
  if (ptin_evc_update_igmp(evc_id, &pflow->flags, igmp_enabled,
                           L7_TRUE /*Remove*/, L7_TRUE /*Look to counters*/) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error deconfiguring IGMP", evc_id);
    rc = L7_FAILURE;
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: IGMP removed", evc_id);
  }

  #if (!PTIN_BOARD_IS_MATRIX)
  /* Remove DHCP configurations */
  if (ptin_evc_update_dhcp(evc_id, &pflow->flags, dhcpv4_enabled, dhcpv6_enabled,
                           L7_TRUE /*Remove*/, L7_TRUE /*Look to counters*/) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error deconfiguring DHCP", evc_id);
    rc = L7_FAILURE;
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: PPPoE removed", evc_id);
  }

  /* Remove DHCP configurations */
  if (ptin_evc_update_pppoe(evc_id, &pflow->flags, pppoe_enabled,
                            L7_TRUE /*Remove*/, L7_TRUE /*Look to counters*/) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error deconfiguring PPPoE", evc_id);
    rc = L7_FAILURE;
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: PPPoE removed", evc_id);
  }
  #endif

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Flow (related to client %u) removed! rc=%d", evc_id, client_vlan, rc);

  return rc;
}
#endif /* PTIN_BOARD_IS_DNX || PTIN_QUATTRO_FLOWS_FEATURE_ENABLED */

/**
 * DHCP configurations
 * 
 * @author mruas (12/5/2014)
 * 
 * @param evc_id        : EVC id
 * @param flags_ref     : Reference flags 
 * @param dhcpv4_enable 
 * @param dhcpv6_enable  
 * @param just_remove   : Remove configurations?
 * @param look_to_counters : Look to DHCP service counters?
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_evc_update_dhcp(L7_uint16 evc_id, L7_uint32 *flags_ref, L7_BOOL dhcpv4_enabled, L7_BOOL dhcpv6_enabled,
                             L7_BOOL just_remove, L7_BOOL look_to_counters)
{
  L7_uint32 evc_ext_id;
  L7_BOOL dhcpv4_apply, dhcpv6_apply;

  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Invalid evc id", evc_id);
    return L7_FAILURE;
  }
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: EVC not active", evc_id);
    return L7_FAILURE;
  }

  evc_ext_id = evcs[evc_id].extended_id;

  dhcpv4_apply = ((*flags_ref & PTIN_EVC_MASK_DHCPV4_PROTOCOL)==PTIN_EVC_MASK_DHCPV4_PROTOCOL) != dhcpv4_enabled;
  dhcpv6_apply = ((*flags_ref & PTIN_EVC_MASK_DHCPV6_PROTOCOL)==PTIN_EVC_MASK_DHCPV6_PROTOCOL) != dhcpv6_enabled;

  /* Remove configurations? */
  if (just_remove)
  {
    /* Remove DHCPv4 trap rules */
    if (dhcpv4_enabled)
    {
      if (!look_to_counters || evcs[evc_id].n_clientflows_dhcpv4 == 1)
      {
        /* Do not disable rule for QUATTRO-stacked services */
        if (!IS_EVC_QUATTRO(evc_id) || !IS_EVC_STACKED(evc_id))
        {
          if (ptin_dhcp_evc_trap_configure(evc_ext_id, L7_DISABLE, L7_AF_INET) != L7_SUCCESS) 
          {
            PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing DHCPv4 trap rule", evc_id);
            return L7_FAILURE;
          }
          else
          {
            PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed trap rules for DHCPv4", evc_id);
            evcs[evc_id].flags &= ~((L7_uint32) PTIN_EVC_MASK_DHCPV4_PROTOCOL);
          }
        }
      }

      /* Update counters */
      if (look_to_counters)
      {
        if (evcs[evc_id].n_clientflows_dhcpv4 > 0)
          evcs[evc_id].n_clientflows_dhcpv4--;
      }
      else
      {
        evcs[evc_id].n_clientflows_dhcpv4 = 0;
      }
    }
    /* Remove DHCPv6 trap rules */
    if (dhcpv6_enabled)
    {
      if (!look_to_counters || evcs[evc_id].n_clientflows_dhcpv6 == 1)
      {
        /* Do not disable rule for QUATTRO-stacked services */
        if (!IS_EVC_QUATTRO(evc_id) || !IS_EVC_STACKED(evc_id))
        {
          if (ptin_dhcp_evc_trap_configure(evc_ext_id, L7_DISABLE, L7_AF_INET6) != L7_SUCCESS)
          {
            PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing DHCPv6 trap rule", evc_id);
            return L7_FAILURE;
          }
          else
          {
            PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed trap rules for DHCPv6", evc_id);
            evcs[evc_id].flags &= ~((L7_uint32) PTIN_EVC_MASK_DHCPV6_PROTOCOL);
          }
        }
      }

      /* Update counters */
      if (look_to_counters)
      {
        if (evcs[evc_id].n_clientflows_dhcpv6 > 0)
          evcs[evc_id].n_clientflows_dhcpv6--;
      }
      else
      {
        evcs[evc_id].n_clientflows_dhcpv6 = 0;
      }
    }
  }
  /* Update configurations */
  else
  {
    /* DHCP: Correct data */
    if (!(evcs[evc_id].flags & PTIN_EVC_MASK_DHCPV4_PROTOCOL)) 
    {
      if (flags_ref != &evcs[evc_id].flags)
        *flags_ref &= ~((L7_uint32) PTIN_EVC_MASK_DHCPV4_PROTOCOL);
      evcs[evc_id].n_clientflows_dhcpv4 = 0;
    }
    if (!(evcs[evc_id].flags & PTIN_EVC_MASK_DHCPV6_PROTOCOL))
    {
      if (flags_ref != &evcs[evc_id].flags)
        *flags_ref &= ~((L7_uint32) PTIN_EVC_MASK_DHCPV6_PROTOCOL);
      evcs[evc_id].n_clientflows_dhcpv6 = 0;
    }

    /* If is the virst DHCPv4 or v6 protocol to configured, add EVC to instance */
    if (!(evcs[evc_id].flags & PTIN_EVC_MASK_DHCPV4_PROTOCOL) &&
        !(evcs[evc_id].flags & PTIN_EVC_MASK_DHCPV6_PROTOCOL) &&
        (dhcpv4_apply || dhcpv6_apply) )
    {
      if (ptin_dhcp_evc_add(evc_ext_id, evcs[evc_id].root_info.nni_ovid) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error adding evc to DHCP instance", evc_id);
        return L7_FAILURE;
      }
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Added evc to DHCP instance with NNI S-VLAN %u", evc_id, evcs[evc_id].root_info.nni_ovid);
    }

    if (dhcpv4_apply)
    {
      /* Configure DHCPv4 trap rule */
      if (dhcpv4_enabled && (!look_to_counters || evcs[evc_id].n_clientflows_dhcpv4 == 0))
      {
        if (ptin_dhcp_evc_trap_configure(evc_ext_id, L7_ENABLE, L7_AF_INET) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error configuring DHCPv4 trap rule", evc_id);
          return L7_FAILURE;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Added DHCPv4 trap rule", evc_id);
          evcs[evc_id].flags |= PTIN_EVC_MASK_DHCPV4_PROTOCOL;
        }
      }
      else if (!dhcpv4_enabled && (!look_to_counters || evcs[evc_id].n_clientflows_dhcpv4 == 1))
      {
        /* Do not disable rule for QUATTRO-stacked services */
        if (!IS_EVC_QUATTRO(evc_id) || !IS_EVC_STACKED(evc_id))
        {
          if (ptin_dhcp_evc_trap_configure(evc_ext_id, L7_DISABLE, L7_AF_INET) != L7_SUCCESS)
          {
            PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error configuring DHCPv4 trap rule", evc_id);
            return L7_FAILURE;
          }
          else
          {
            PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed DHCPv4 trap rule", evc_id);
            evcs[evc_id].flags &= ~((L7_uint32) PTIN_EVC_MASK_DHCPV4_PROTOCOL);
          }
        }
      }

      /* DHCPv4 local data */
      if (flags_ref != &evcs[evc_id].flags)
      {
        if (dhcpv4_enabled && !(*flags_ref & PTIN_EVC_MASK_DHCPV4_PROTOCOL)) 
        {
          *flags_ref |= PTIN_EVC_MASK_DHCPV4_PROTOCOL;
          if (look_to_counters)
          {
            evcs[evc_id].n_clientflows_dhcpv4++;
          }
        }
        else if (!dhcpv4_enabled && (*flags_ref & PTIN_EVC_MASK_DHCPV4_PROTOCOL))
        {
          *flags_ref &= ~((L7_uint32) PTIN_EVC_MASK_DHCPV4_PROTOCOL);
          if (look_to_counters)
          {
            if (evcs[evc_id].n_clientflows_dhcpv4 > 0)
              evcs[evc_id].n_clientflows_dhcpv4--;
          }
        }
      }
    }

    if (dhcpv6_apply)
    {
      /* Configure DHCPv6 trap rule */
      if (dhcpv6_enabled && (!look_to_counters || evcs[evc_id].n_clientflows_dhcpv6 == 0))
      {
        if (ptin_dhcp_evc_trap_configure(evc_ext_id, L7_ENABLE, L7_AF_INET6) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error configuring DHCPv6 trap rule", evc_id);
          return L7_FAILURE;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Added DHCPv6 trap rule", evc_id);
          evcs[evc_id].flags |= PTIN_EVC_MASK_DHCPV6_PROTOCOL;
        }
      }
      else if (!dhcpv6_enabled && (!look_to_counters || evcs[evc_id].n_clientflows_dhcpv6 == 1))
      {
        /* Do not disable rule for QUATTRO-stacked services */
        if (!IS_EVC_QUATTRO(evc_id) || !IS_EVC_STACKED(evc_id))
        {
          if (ptin_dhcp_evc_trap_configure(evc_ext_id, L7_DISABLE, L7_AF_INET6) != L7_SUCCESS)
          {
            PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error configuring DHCPv6 trap rule", evc_id);
            return L7_FAILURE;
          }
          else
          {
            PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed DHCPv6 trap rule", evc_id);
            evcs[evc_id].flags &= ~((L7_uint32) PTIN_EVC_MASK_DHCPV6_PROTOCOL);
          }
        }
      }

      /* DHCPv6 local data */
      if (flags_ref != &evcs[evc_id].flags)
      {
        if (dhcpv6_enabled && !(*flags_ref & PTIN_EVC_MASK_DHCPV6_PROTOCOL)) 
        {
          *flags_ref |= PTIN_EVC_MASK_DHCPV6_PROTOCOL;
          if (look_to_counters)
          {
            evcs[evc_id].n_clientflows_dhcpv6++;
          }
        }
        else if (!dhcpv6_enabled && (*flags_ref & PTIN_EVC_MASK_DHCPV6_PROTOCOL))
        {
          *flags_ref &= ~((L7_uint32) PTIN_EVC_MASK_DHCPV6_PROTOCOL);
          if (look_to_counters)
          {
            if (evcs[evc_id].n_clientflows_dhcpv6 > 0)
              evcs[evc_id].n_clientflows_dhcpv6--;
          }
        }
      }
    }

    /* Clear counters */
    if (!look_to_counters)
    {
      if (dhcpv4_apply)  evcs[evc_id].n_clientflows_dhcpv4 = 0;
      if (dhcpv6_apply)  evcs[evc_id].n_clientflows_dhcpv6 = 0;
    }
  }

  /* Remove DHCP instance? */
  if (!(evcs[evc_id].flags & PTIN_EVC_MASK_DHCPV4_PROTOCOL) &&
      !(evcs[evc_id].flags & PTIN_EVC_MASK_DHCPV6_PROTOCOL) &&
      (just_remove || dhcpv4_apply || dhcpv6_apply) )
  {
    if (ptin_dhcp_evc_remove(evc_ext_id) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing EVC from DHCP instance", evc_id);
      return L7_FAILURE;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: EVC removed from DHCP instance", evc_id);
    }
  }

  return L7_SUCCESS;
}

/**
 * PPPoE configurations
 * 
 * @author mruas (12/5/2014)
 * 
 * @param evc_id 
 * @param flags_ref     : Reference flags  
 * @param pppoe_enable 
 * @param just_remove   : Remove configurations?
 * @param look_to_counters : Look to PPPoE service counters?
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_evc_update_pppoe(L7_uint16 evc_id, L7_uint32 *flags_ref,
                              L7_BOOL pppoe_enabled, L7_BOOL just_remove, L7_BOOL look_to_counters)
{
  L7_uint32 evc_ext_id;
  L7_BOOL   pppoe_apply;

  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Invalid evc id", evc_id);
    return L7_FAILURE;
  }
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: EVC not active", evc_id);
    return L7_FAILURE;
  }

  evc_ext_id = evcs[evc_id].extended_id;

  pppoe_apply = ((*flags_ref & PTIN_EVC_MASK_PPPOE_PROTOCOL)==PTIN_EVC_MASK_PPPOE_PROTOCOL) != pppoe_enabled;

  if (just_remove)
  {
    /* Remove PPPoE trap rules */
    if (pppoe_enabled)
    {
      if (!look_to_counters || evcs[evc_id].n_clientflows_pppoe == 1)
      {
        if (ptin_pppoe_evc_remove(evc_ext_id) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing evc from PPPoE instance", evc_id);
          return L7_FAILURE;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed evc from PPPoE instance", evc_id);
          evcs[evc_id].flags &= ~((L7_uint32) PTIN_EVC_MASK_PPPOE_PROTOCOL);
        }
      }

      /* Update counters */
      if (look_to_counters)
      {
        if (evcs[evc_id].n_clientflows_pppoe > 0)
          evcs[evc_id].n_clientflows_pppoe--;
      }
      else
      {
        evcs[evc_id].n_clientflows_pppoe = 0;
      }
    }
  }
  else
  {
    /* PPPoE: Correct data */
    if (!(evcs[evc_id].flags & PTIN_EVC_MASK_PPPOE_PROTOCOL)) 
    {
      if (flags_ref != &evcs[evc_id].flags)
        *flags_ref &= ~((L7_uint32) PTIN_EVC_MASK_PPPOE_PROTOCOL);
      evcs[evc_id].n_clientflows_pppoe = 0;
    }

    /* Only configure PPPoE for first time configurations */
    if (pppoe_apply)
    {
      if (pppoe_enabled && (!look_to_counters || evcs[evc_id].n_clientflows_pppoe == 0))
      {
        if (ptin_pppoe_evc_add(evc_ext_id, evcs[evc_id].root_info.nni_ovid) != L7_SUCCESS) 
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error adding evc to PPPoE instance", evc_id);
          return L7_FAILURE;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Added evc to PPPoE instance with NNI S-VLAN %u", evc_id, evcs[evc_id].root_info.nni_ovid);

          /* Update PPPoE flags */
          evcs[evc_id].flags |= PTIN_EVC_MASK_PPPOE_PROTOCOL;
        }
      }
      else if (!pppoe_enabled && (!look_to_counters || evcs[evc_id].n_clientflows_pppoe == 1))
      {
        if (ptin_pppoe_evc_remove(evc_ext_id) != L7_SUCCESS) 
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing evc from PPPoE instance", evc_id);
          return L7_FAILURE;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: EVC removed from PPPoE instance", evc_id);

          /* Update PPPoE flags */
          evcs[evc_id].flags &= ~((L7_uint32) PTIN_EVC_MASK_PPPOE_PROTOCOL);
        }
      }

      /* PPPoE local data */
      if (flags_ref != &evcs[evc_id].flags)
      {
        if (pppoe_enabled && !(*flags_ref & PTIN_EVC_MASK_PPPOE_PROTOCOL)) 
        {
          *flags_ref |= PTIN_EVC_MASK_PPPOE_PROTOCOL;
          if (look_to_counters)
          {
            evcs[evc_id].n_clientflows_pppoe++;
          }
        }
        else if (!pppoe_enabled && (*flags_ref & PTIN_EVC_MASK_PPPOE_PROTOCOL))
        {
          *flags_ref &= ~((L7_uint32) PTIN_EVC_MASK_PPPOE_PROTOCOL);
          if (look_to_counters)
          {
            if (evcs[evc_id].n_clientflows_pppoe > 0)
              evcs[evc_id].n_clientflows_pppoe--;
          }
        }
      }
    }

    /* Clear counters */
    if (!look_to_counters)
    {
      if (pppoe_apply)  evcs[evc_id].n_clientflows_pppoe = 0;
    }
  }

  return L7_SUCCESS;
}

/**
 * IGMP configurations
 * 
 * @author mruas (12/5/2014)
 * 
 * @param evc_id 
 * @param flags_ref     : Reference flags  
 * @param igmp_enable 
 * @param just_remove   : Remove configurations?
 * @param look_to_counters : Look to PPPoE service counters?
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_evc_update_igmp(L7_uint16 evc_id, L7_uint32 *flags_ref,
                             L7_BOOL igmp_enabled, L7_BOOL just_remove, L7_BOOL look_to_counters)
{
  L7_uint32 evc_ext_id;
  L7_BOOL   igmp_apply;
  L7_BOOL   iptv_enabled;

  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Invalid evc id", evc_id);
    return L7_FAILURE;
  }
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: EVC not active", evc_id);
    return L7_FAILURE;
  }

  evc_ext_id = evcs[evc_id].extended_id;

  igmp_apply = ((*flags_ref & PTIN_EVC_MASK_IGMP_PROTOCOL)==PTIN_EVC_MASK_IGMP_PROTOCOL) != igmp_enabled;

  iptv_enabled = evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV;

  if (just_remove)
  {
    /* Remove IGMP trap rules */
    if (igmp_enabled)
    {
      if (!look_to_counters || evcs[evc_id].n_clientflows_igmp == 1)
      {
        if (ptin_igmp_evc_configure(evc_ext_id, L7_FALSE,
                                    !iptv_enabled
                                    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
                                    && SINGLE_INSTANCE(evc_id, n_quattro_igmp_evcs)
                                    #endif
                                   ) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing evc from IGMP instance", evc_id);
          return L7_FAILURE;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed evc from IGMP instance", evc_id);
          evcs[evc_id].flags &= ~((L7_uint32) PTIN_EVC_MASK_IGMP_PROTOCOL);
          #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
          DECREMENT_QUATTRO_INSTANCE(evc_id, n_quattro_igmp_evcs);
          #endif
        }
      }

      /* Update counters */
      if (look_to_counters)
      {
        if (evcs[evc_id].n_clientflows_igmp > 0)
          evcs[evc_id].n_clientflows_igmp--;
      }
      else
      {
        evcs[evc_id].n_clientflows_igmp = 0;
      }
    }
  }
  else
  {
    /* IGMP: Correct data */
    if (!(evcs[evc_id].flags & PTIN_EVC_MASK_IGMP_PROTOCOL)) 
    {
      if (flags_ref != &evcs[evc_id].flags)
        *flags_ref &= ~((L7_uint32) PTIN_EVC_MASK_IGMP_PROTOCOL);
      evcs[evc_id].n_clientflows_igmp = 0;
    }

    /* Only configure IGMP for first time configurations */
    if (igmp_apply)
    {
      if (igmp_enabled && (!look_to_counters || evcs[evc_id].n_clientflows_igmp == 0))
      {
        if (ptin_igmp_evc_configure(evc_ext_id, L7_TRUE,
                                    !iptv_enabled 
                                    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
                                    && NO_INSTANCE(evc_id, n_quattro_igmp_evcs)
                                    #endif
                                   ) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error adding evc to IGMP instance", evc_id);
          return L7_FAILURE;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Added evc to IGMP instance with NNI S-VLAN %u", evc_id, evcs[evc_id].root_info.nni_ovid);

          /* Update IGMP flags */
          evcs[evc_id].flags |= PTIN_EVC_MASK_IGMP_PROTOCOL;
          #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
          INCREMENT_QUATTRO_INSTANCE(evc_id, n_quattro_igmp_evcs);
          #endif
        }
      }
      else if (!igmp_enabled && (!look_to_counters || evcs[evc_id].n_clientflows_igmp == 1))
      {
        if (ptin_igmp_evc_configure(evc_ext_id, L7_FALSE,
                                    !iptv_enabled 
                                    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
                                    && SINGLE_INSTANCE(evc_id, n_quattro_igmp_evcs)
                                    #endif
                                   ) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing evc from IGMP instance", evc_id);
          return L7_FAILURE;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: EVC removed from IGMP instance", evc_id);

          /* Update IGMP flags */
          evcs[evc_id].flags &= ~((L7_uint32) PTIN_EVC_MASK_IGMP_PROTOCOL);
          #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
          DECREMENT_QUATTRO_INSTANCE(evc_id, n_quattro_igmp_evcs);
          #endif
        }
      }

      /* IGMP local data */
      if (flags_ref != &evcs[evc_id].flags)
      {
        if (igmp_enabled && !(*flags_ref & PTIN_EVC_MASK_IGMP_PROTOCOL)) 
        {
          *flags_ref |= PTIN_EVC_MASK_IGMP_PROTOCOL;
          if (look_to_counters)
          {
            evcs[evc_id].n_clientflows_igmp++;
          }
        }
        else if (!igmp_enabled && (*flags_ref & PTIN_EVC_MASK_IGMP_PROTOCOL))
        {
          *flags_ref &= ~((L7_uint32) PTIN_EVC_MASK_IGMP_PROTOCOL);
          if (look_to_counters)
          {
            if (evcs[evc_id].n_clientflows_igmp > 0)
              evcs[evc_id].n_clientflows_igmp--;
          }
        }
      }
    }

    /* Clear counters */
    if (!look_to_counters)
    {
      if (igmp_apply)  evcs[evc_id].n_clientflows_igmp = 0;
    }
  }

  return L7_SUCCESS;
}

/**
 * Gets the flooding vlans list
 * 
 * @param intIfNum    : leaf interface
 * @param int_vlan     : internal Vlan
 * @param client_vlan : client vlan to apply this flooding vlan
 * @param outer_vlan  : list of outer vlans
 * @param inner_vlan  : list of inner vlans 
 * @param number_of_vlans : Size of returned lists
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_flood_vlan_get( L7_uint32 intIfNum, L7_uint int_vlan, L7_uint client_vlan,
                                 L7_uint16 *outer_vlan, L7_uint16 *inner_vlan, L7_uint16 *number_of_vlans )
{
  L7_uint16   evc_id;
  L7_uint16   max_vlans;
  L7_int      ptin_port, port, i, index;
  dl_queue_t *queue;
  struct ptin_evc_client_s *pclient;

#if 0
  /* Validate arguments */
  if (int_vlan < PTIN_VLAN_MIN || int_vlan > PTIN_VLAN_MAX)
  {
    if (ptin_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }
#endif

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[int_vlan];
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    if (ptin_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",int_vlan);
    return L7_FAILURE;
  }

  /* EVC should be active */
  if (!evcs[evc_id].in_use)
  {
    if (ptin_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u should be in use (int_vlan=%u)",evc_id,int_vlan);
    return L7_FAILURE;
  }

  /* Check if the EVC is stacked */
  if (IS_EVC_STD(evc_id) && !IS_EVC_STACKED(evc_id))
  {
    if (ptin_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u is not stacked!!!", evc_id);
    return L7_FAILURE;
  }

  /* Determine leaf ptin_port */
  if ( intIfNum !=0 && intIfNum != L7_ALL_INTERFACES )
  {
    /* Convert intIfNum to ptin_port format */
    if ( ptin_intf_intIfNum2port(intIfNum, &ptin_port) != L7_SUCCESS)
    {
      if (ptin_packet_debug_enable)
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: intIfNum %u is invalid", evc_id, intIfNum);
      return L7_FAILURE;
    }
    /* Validate ptin_port */
    if ( ptin_port >= PTIN_SYSTEM_N_INTERF )
    {
      if (ptin_packet_debug_enable)
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: invalid ptin_port %u associated to inIfNum %u", evc_id, ptin_port, intIfNum);
      return L7_FAILURE;
    }
    /* Check if port is in use */
    if ( !evcs[evc_id].intf[ptin_port].in_use )
    {
      if (ptin_packet_debug_enable)
        PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: Port %u is not active in this EVC", evc_id, ptin_port);
      return L7_NOT_EXIST;
    }
    if ( evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF )
    {
      if (ptin_packet_debug_enable)
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Port %u is not a leaf interface", evc_id, ptin_port);
      return L7_FAILURE;
    }
  }
  else
  {
    ptin_port = -1;   /* All ports */
  }

  /* Determine max number of vlans to be read */
  if ( number_of_vlans != L7_NULLPTR && *number_of_vlans != 0 )
  {
    max_vlans = *number_of_vlans;
  }
  else
  {
    max_vlans = PTIN_FLOOD_VLANS_MAX;
  }

  index = 0;

  /* Run all Leaf ports */
  for (port=0; port<PTIN_SYSTEM_N_INTERF && index<max_vlans; port++)
  {
    /* Skip not used ports, or non leafs */
    if ( !evcs[evc_id].intf[ptin_port].in_use ||
         evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF )
      continue;

    /* Only apply to specified port, or all of them if not specified */
    if ( ptin_port == port || ptin_port == -1 )
    {
      /* SEM CLIENTS UP */
      osapiSemaTake(ptin_evc_clients_sem, L7_WAIT_FOREVER);

      /* Clients queue */
      queue = &evcs[evc_id].intf[port].clients;

      pclient = L7_NULLPTR;

      /* Get clients queue head */
      /* If error, pclient continues being null */
      dl_queue_get_head(queue, (dl_queue_elem_t **)&pclient);

      /* Run all clients */
      while (pclient != L7_NULLPTR && index<max_vlans)
      {
        /* Only apply to specified client_vlan, or all of them if not specified (if null) */
        if (client_vlan == 0 || pclient->int_ivid == client_vlan)
        {
          /* Run all vlan elements */
          for ( i=0; i<PTIN_FLOOD_VLANS_MAX && index<max_vlans; i++)
          {
            if ( pclient->flood_vlan[i] < PTIN_VLAN_MIN || pclient->flood_vlan[i] > PTIN_VLAN_MAX )
              continue;

            if ( outer_vlan != L7_NULLPTR )
              outer_vlan[index] = pclient->flood_vlan[i];

            if ( inner_vlan != L7_NULLPTR )
              inner_vlan[index] = 0;

            index++;
          }

          /* No need to search for more clients, if it was provided */
          if ( client_vlan != 0 )  break;
        }
        /* Next client */
        pclient = (struct ptin_evc_client_s *) dl_queue_get_next(queue, (dl_queue_elem_t *)pclient);
      }

      /* SEM CLIENTS DOWN */
      osapiSemaGive(ptin_evc_clients_sem);
    }
  }

  /* Return number of read vlans */
  if ( number_of_vlans != L7_SUCCESS )
  {
    *number_of_vlans = index;
  }

  if (ptin_packet_debug_enable)
    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: List of vlans read from client_vlan %u (%u vlans)", evc_id, client_vlan, index);

  return L7_SUCCESS;
}

/**
 * Adds a flooding vlan
 * 
 * @param evc_ext_id  : EVC extended id
 * @param ptin_intf   : port of which client_vlan belongs
 * @param client_vlan : client vlan to apply this flooding vlan
 * @param outer_vlan  : outer vlan of transmitted  packets
 * @param inner_vlan  : inner vlan of transmitted  packets
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_flood_vlan_add( L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, L7_uint16 client_vlan,
                                 L7_uint16 outer_vlan, L7_uint16 inner_vlan )
{
  L7_uint32 evc_id;
  L7_int ptin_port, port, i;
  dl_queue_t *queue;
  struct ptin_evc_client_s *pclient;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( outer_vlan < PTIN_VLAN_MIN || outer_vlan > PTIN_VLAN_MAX )
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Given outer vlan is not valid (%u)", outer_vlan);
    return L7_FAILURE;
  }

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_FAILURE;
  }

  /* Check if the EVC is stacked */
  if (IS_EVC_STD(evc_id) && !IS_EVC_STACKED(evc_id))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u is not stacked!!!", evc_id);
    return L7_FAILURE;
  }

  /* Determine leaf ptin_port */
  if ( ptin_intf != L7_NULLPTR )
  {
    if (ptin_intf->intf_type == PTIN_EVC_INTF_PHYSICAL)
      ptin_port = ptin_intf->intf_id;
    else
      ptin_port = ptin_intf->intf_id + PTIN_SYSTEM_N_PORTS;

    /* Validate leaf interface (from received message) */
    if ( ptin_port >= PTIN_SYSTEM_N_INTERF )
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: %s# %u is invalid", evc_id,
              ptin_intf->intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", ptin_intf->intf_id);
      return L7_FAILURE;
    }
    if ( !evcs[evc_id].intf[ptin_port].in_use )
    {
      PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: %s# %u is not active in this EVC", evc_id,
              ptin_intf->intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", ptin_intf->intf_id);
      return L7_NOT_EXIST;
    }
    if ( evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF )
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: %s# %u is not a leaf interface", evc_id,
              ptin_intf->intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", ptin_intf->intf_id);
      return L7_FAILURE;
    }
  }
  else
  {
    ptin_port = -1;   /* All ports */
  }

  /* Run all Leaf ports */
  for (port=0; port<PTIN_SYSTEM_N_INTERF; port++)
  {
    /* Skip not used ports, or non leafs */
    if ( !evcs[evc_id].intf[ptin_port].in_use ||
         evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF )
      continue;

    /* Only apply to specified port, or all of them if not specified */
    if ( ptin_port == port || ptin_port == -1 )
    {
      /* SEM CLIENTS UP */
      osapiSemaTake(ptin_evc_clients_sem, L7_WAIT_FOREVER);

      /* Clients queue */
      queue = &evcs[evc_id].intf[port].clients;

      pclient = L7_NULLPTR;

      /* Get clients queue head */
      /* If error, pclient continues being null */
      dl_queue_get_head(queue, (dl_queue_elem_t **)&pclient);

      /* Run all clients */
      while (pclient != L7_NULLPTR)
      {
        /* Only apply to specified client_vlan, or all of them if not specified (if null) */
        if (client_vlan == 0 || pclient->int_ivid == client_vlan)
        {
          /* Only add new flooding vlan, if it is different than the newer one */
          if ( pclient->uni_ovid != outer_vlan )
          {
            /* Check if this vlan already exists */
            for ( i=0; i<PTIN_FLOOD_VLANS_MAX && pclient->flood_vlan[i]!=outer_vlan; i++);

            /* Not found: */
            if ( i >= PTIN_FLOOD_VLANS_MAX )
            {
              /* Search for the first free element */
              for ( i=0; i<PTIN_FLOOD_VLANS_MAX && pclient->flood_vlan[i]!=0; i++);

              if ( i < PTIN_FLOOD_VLANS_MAX)
              {
                pclient->flood_vlan[i] = outer_vlan;
                PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: New outer vlan %u added to cvlan %u (port %u) - index=%u", evc_id, outer_vlan, pclient->int_ivid, port, i);
              }
              else
              {
                PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: No more free elements for cvlan %u (port %u)", evc_id, pclient->int_ivid, port);
                rc = L7_FAILURE;
              }
            }
            /* outer vlan found: */
            else
            {
              PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: Outer vlan %u already exists for cvlan %u (port %u) - index=%u", evc_id, outer_vlan, pclient->int_ivid, port, i);
            }
          }
          /* Provided outer vlan is repeated */
          else
          {
            PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: Ignored outer vlan %u for cvlan %u (port %u)", evc_id, outer_vlan, pclient->int_ivid, port);
          }

          /* No need to search for more clients, if it was provided */
          if ( client_vlan != 0 )  break;
        }
        /* Next client */
        pclient = (struct ptin_evc_client_s *) dl_queue_get_next(queue, (dl_queue_elem_t *)pclient);
      }

      /* SEM CLIENTS DOWN */
      osapiSemaGive(ptin_evc_clients_sem);
    }
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: Added outer vlan %u to client_vlan %u", evc_ext_id, outer_vlan, client_vlan);

  return L7_SUCCESS;
}

/**
 * Removes a flooding vlan
 * 
 * @param evc_ext_id  : EVC extended id
 * @param ptin_intf   : port of which client_vlan belongs
 * @param client_vlan : client vlan to apply this flooding vlan
 * @param outer_vlan  : outer vlan of transmitted packets
 * @param inner_vlan  : inner vlan of transmitted packets
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_flood_vlan_remove( L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, L7_uint16 client_vlan,
                                    L7_uint16 outer_vlan, L7_uint16 inner_vlan )
{
  L7_uint32 evc_id;
  L7_int ptin_port, port, i;
  dl_queue_t *queue;
  struct ptin_evc_client_s *pclient;

  /* Validate arguments */
  if ( outer_vlan < PTIN_VLAN_MIN || outer_vlan > PTIN_VLAN_MAX )
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Given outer vlan is not valid (%u)", outer_vlan);
    return L7_FAILURE;
  }

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* Check if the EVC is stacked */
  if (IS_EVC_STD(evc_id) && !IS_EVC_STACKED(evc_id))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u is not stacked!!!", evc_id);
    return L7_FAILURE;
  }

  /* Determine leaf ptin_port */
  if ( ptin_intf != L7_NULLPTR )
  {
    if (ptin_intf->intf_type == PTIN_EVC_INTF_PHYSICAL)
      ptin_port = ptin_intf->intf_id;
    else
      ptin_port = ptin_intf->intf_id + PTIN_SYSTEM_N_PORTS;

    /* Validate leaf interface (from received message) */
    if ( ptin_port >= PTIN_SYSTEM_N_INTERF )
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: %s# %u is invalid", evc_id,
              ptin_intf->intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", ptin_intf->intf_id);
      return L7_FAILURE;
    }
    if ( !evcs[evc_id].intf[ptin_port].in_use )
    {
      PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: %s# %u is not active in this EVC", evc_id,
              ptin_intf->intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", ptin_intf->intf_id);
      return L7_NOT_EXIST;
    }
    if ( evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF )
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: %s# %u is not a leaf interface", evc_id,
              ptin_intf->intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", ptin_intf->intf_id);
      return L7_FAILURE;
    }
  }
  else
  {
    ptin_port = -1;   /* All ports */
  }

  /* Run all Leaf ports */
  for (port=0; port<PTIN_SYSTEM_N_INTERF; port++)
  {
    /* Skip not used ports, or non leafs */
    if ( !evcs[evc_id].intf[ptin_port].in_use ||
         evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF )
      continue;

    /* Only apply to specified port, or all of them if not specified */
    if ( ptin_port == port || ptin_port == -1 )
    {
      /* SEM CLIENTS UP */
      osapiSemaTake(ptin_evc_clients_sem, L7_WAIT_FOREVER);

      /* Clients queue */
      queue = &evcs[evc_id].intf[port].clients;

      pclient = L7_NULLPTR;

      /* Get clients queue head */
      /* If error, pclient continues being null */
      dl_queue_get_head(queue, (dl_queue_elem_t **)&pclient);

      /* Run all clients */
      while (pclient != L7_NULLPTR)
      {
        /* Only apply to specified client_vlan, or all of them if not specified (if null) */
        if (client_vlan == 0 || pclient->int_ivid == client_vlan)
        {
          /* Search for the outer vlan */
          for ( i=0; i<PTIN_FLOOD_VLANS_MAX && pclient->flood_vlan[i]!=outer_vlan; i++);

          if ( i < PTIN_FLOOD_VLANS_MAX)
          {
            pclient->flood_vlan[i] = 0;
            PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed outer vlan %u from cvlan %u (port %u) - index=%u", evc_id, outer_vlan, pclient->int_ivid, port, i);
          }
          else
          {
            PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: Outer vlan %u not found for cvlan %u (port %u)", evc_id, outer_vlan, pclient->int_ivid, port);
          }

          /* No need to search for more clients, if client was provided */
          if ( client_vlan != 0 )  break;
        }

        /* Next client */
        pclient = (struct ptin_evc_client_s *) dl_queue_get_next(queue, (dl_queue_elem_t *)pclient);
      }

      /* SEM CLIENTS DOWN */
      osapiSemaGive(ptin_evc_clients_sem);
    }
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: Removed outer vlan %u from client_vlan %u", evc_ext_id, outer_vlan, client_vlan);

  return L7_SUCCESS;
}

/**
 * Initializes Storm Control configurations for all vlans
 * 
 * @return L7_RC_t: L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_stormControl_init(void)
{
  ptin_stormControl_t stormControl;

  memset(&stormControl, 0x00, sizeof(ptin_stormControl_t));

  /* Initialize storm control for all EVCs, and all traffic types */
  stormControl.flags =  /*PTIN_STORMCONTROL_MASK_BCAST | PTIN_STORMCONTROL_MASK_MCAST | PTIN_STORMCONTROL_MASK_UCUNK |*/ PTIN_STORMCONTROL_MASK_CPU;

  return ptin_evc_stormControl_reset(&stormControl);
}


/**
 * Get storm control configurations
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_stormControl_get(ptin_stormControl_t *stormControl)
{
  /* Validate arguments */
  if (stormControl == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid argument");
    return L7_FAILURE;
  }

  /* NOT IMPLEMENTED */
  memset(stormControl, 0x00, sizeof(ptin_stormControl_t));

  return L7_SUCCESS;
}

/**
 * Set storm control configurations
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_stormControl_set(L7_BOOL enable, ptin_stormControl_t *stormControl)
{
  ptin_stormControl_t stormControl_out;

  /* Validate arguments */
  if (stormControl == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid argument");
    return L7_FAILURE;
  }

  /* Validate flags */
  if ((stormControl->flags & PTIN_STORMCONTROL_MASK_ALL) == 0)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "No flags provided (0x%04x)", stormControl->flags);
    return L7_SUCCESS;
  }

  /* Rate values: convert to kbps */
  stormControl_out.flags      = stormControl->flags & PTIN_STORMCONTROL_MASK_ALL;
  stormControl_out.bcast_rate = stormControl->bcast_rate / 1000;
  stormControl_out.mcast_rate = stormControl->mcast_rate / 1000;
  stormControl_out.ucunk_rate = stormControl->ucunk_rate / 1000;
  stormControl_out.cpu_rate   = stormControl->cpu_rate   / 1000;

  if (ptin_stormControl_config(enable, &stormControl_out) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error configuring storm control");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_EVC, "Success configuring storm control");

  return L7_SUCCESS;
}

/**
 * Reset storm control configurations
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_stormControl_reset(ptin_stormControl_t *stormControl)
{
  /* Validate arguments */
  if (stormControl == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid argument");
    return L7_FAILURE;
  }

  /* Default Rate values */
  stormControl->bcast_rate = (L7_uint32) RATE_LIMIT_BCAST * 1000;
  stormControl->mcast_rate = (L7_uint32) RATE_LIMIT_MCAST * 1000;
  stormControl->ucunk_rate = (L7_uint32) RATE_LIMIT_UCUNK * 1000;
  stormControl->cpu_rate   = (L7_uint32) RATE_LIMIT_CPU   * 1000;

  PT_LOG_TRACE(LOG_CTX_EVC, "Use default rate limits");

  if (ptin_evc_stormControl_set(L7_ENABLE, stormControl)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error configuring default rate limits");
    return L7_SUCCESS;
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "Success configuring default rate limits");
  return L7_SUCCESS;
}


/**
 * Bandwidth Policers management functions
 */

/**
 * Read data of a bandwidth profile
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : bw profile
 * @param meter      : Policer meter (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
L7_RC_t ptin_evc_bwProfile_get(L7_uint32 evc_ext_id, ptin_bw_profile_t *profile, ptin_bw_meter_t *meter)
{
  L7_uint32 evc_id;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* Verify and update profile data */
  if (ptin_evc_bwProfile_verify(evc_id, profile) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Profile data have conflicts");
    return L7_FAILURE;
  }

  /* Read policer information */
  if ((rc = ptin_bwPolicer_get(profile, meter))!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error reading policer profile");
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Apply a bandwidth profile to an EVC and (optionally) to a 
 * specific client 
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : bw profile 
 * @param meter      : Policer meter 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
L7_RC_t ptin_evc_bwProfile_set(L7_uint32 evc_ext_id, ptin_bw_profile_t *profile, ptin_bw_meter_t *meter)
{
  L7_uint32 evc_id;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  /* For QUATTRO services, apply special profiles */
  if (0 /*IS_EVC_QUATTRO(evc_id)*/)
  {
    L7_uint32 vport_id;

    /* Calculate vport */
    vport_id = intf_vp_calc(profile->ptin_port, profile->outer_vlan_lookup);

    if (vport_id == 0 || vport_id == (L7_uint32)-1)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Invalid vport_id 0x%x", vport_id);
      return L7_FAILURE;
    }

    /* Apply policer */
    if (ptin_evc_vp_policer(vport_id, meter) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Error applying policer to vport_id 0x%x", vport_id);
      return L7_FAILURE;
    }

    return L7_SUCCESS;
  }
#endif

  /* Verify and update profile data */
  if (ptin_evc_bwProfile_verify(evc_id, profile) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Profile data have conflicts");
    return L7_FAILURE;
  }

  /* Apply policer */
  if ((rc = ptin_bwPolicer_set(profile, meter, -1)) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error applying policer");
  }

  PT_LOG_TRACE(LOG_CTX_EVC,"Policer allocated");

  return rc;
}

/**
 * Remove a bandwidth profile to an EVC 
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : bw profile 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
L7_RC_t ptin_evc_bwProfile_delete(L7_uint32 evc_ext_id, ptin_bw_profile_t *profile)
{
  L7_uint32 evc_id;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  /* For QUATTRO services, apply special profiles */
  if (0 /*IS_EVC_QUATTRO(evc_id)*/)
  {
    L7_uint32 vport_id;

    /* Calculate vport */
    vport_id = intf_vp_calc(profile->ptin_port, profile->outer_vlan_lookup);

    if (vport_id == 0 || vport_id == (L7_uint32)-1)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Invalid vport_id 0x%x", vport_id);
      return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_EVC,"pon_port=%u, gem_id=%u: vport_id=0x%x", profile->ptin_port, profile->outer_vlan_lookup, vport_id);

    /* Apply policer */
    if (ptin_evc_vp_policer(vport_id, L7_NULLPTR) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Error removing policer from vport_id 0x%x", vport_id);
      return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_EVC,"Success applying meter for pon_port=%u, gem_id=%u (vport_id=0x%x)",
              profile->ptin_port, profile->outer_vlan_lookup, vport_id);

    return L7_SUCCESS;
  }
#endif

  /* Verify and update profile data */
  rc = ptin_evc_bwProfile_verify(evc_id,profile);
  if ( rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Profile data have conflicts");
    return rc;
  }

  /* Apply policer */
  rc = ptin_bwPolicer_delete(profile);
  if ( rc != L7_SUCCESS )
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error removing policer");
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_EVC,"Policer deallocated");

  return L7_SUCCESS;
}


/**
 * EVC Counters management functions
 */

/**
 * Read statistics of a particular EVC
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : evcStats profile 
 * @param stats      : Statistics data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
L7_RC_t ptin_evc_evcStats_get(L7_uint32 evc_ext_id, ptin_evcStats_profile_t *profile, ptin_evcStats_counters_t *stats)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 evc_id;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* Probe processing */
  if (profile!=L7_NULLPTR && profile->dst_ip!=0)
  {
    return ptin_evc_probe_get(evc_id, profile, stats);
  }

  /* Verify and update profile data */
  if (ptin_evc_evcStats_verify(evc_id,profile)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Profile data have conflicts");
    return L7_FAILURE;
  }

  /* Read policy information */
  if ((rc = ptin_evcStats_get(stats, profile))!=L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_EVC,"Error reading policer");
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Add a rule to make packets counting of a specific EVC
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : evcStats profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
L7_RC_t ptin_evc_evcStats_set(L7_uint32 evc_ext_id, ptin_evcStats_profile_t *profile)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 evc_id;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* Probe processing */
  if (profile!=L7_NULLPTR && profile->dst_ip!=0)
  {
    return ptin_evc_probe_add(evc_id, profile);
  }

  /* Verify and update profile data */
  if (ptin_evc_evcStats_verify(evc_id,profile)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Profile data have conflicts");
    return L7_FAILURE;
  }

  /* Apply policy */
  if ((rc = ptin_evcStats_set(profile))!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error adding evcStats");
  }

  PT_LOG_TRACE(LOG_CTX_EVC,"Counter allocated");

  return rc;
}

/**
 * Delete a rule to make packets counting of a specific EVC
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : evcStats profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
L7_RC_t ptin_evc_evcStats_delete(L7_uint32 evc_ext_id, ptin_evcStats_profile_t *profile)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 evc_id;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* Probe processing */
  if (profile!=L7_NULLPTR && profile->dst_ip!=0)
  {
    return ptin_evc_probe_delete(evc_id, profile);
  }

  /* Verify and update profile data */
  rc = ptin_evc_evcStats_verify(evc_id,profile);
  if ( rc != L7_SUCCESS )
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Profile data have conflicts");
    return rc;
  }

  /* RX counter */
  rc = ptin_evcStats_delete(profile);
  if ( rc != L7_SUCCESS )
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error removing RX evcStats");
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_EVC,"Counter deallocated");

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
 * Remove all clients/flows for a specific evc
 * 
 * @param evc_id : evc index 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_allclientsflows_remove( L7_uint evc_id )
{
  L7_uint     intf_idx;
  L7_RC_t     rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_id].in_use)
    return L7_SUCCESS;

  /* Only stacked services have clients */
  if (IS_EVC_STD(evc_id) && !IS_EVC_STACKED(evc_id))
    return L7_SUCCESS;

  /* Run all interfaces */
  for (intf_idx=0; intf_idx<PTIN_SYSTEM_N_INTERF; intf_idx++)
  {
    if (!evcs[evc_id].intf[intf_idx].in_use)
      continue;

    /* Remove all clients/flows of this interface */
    if (ptin_evc_intfclientsflows_remove(evc_id, intf_idx)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error removing all clients/flows from port %u",evc_id,intf_idx);
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**
 * Remove all clients for a specific evc and interface
 * 
 * @param evc_id   : evc index 
 * @param intf_idx : interface index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intfclientsflows_remove( L7_uint evc_id, L7_uint32 intf_idx)
{
  struct  ptin_evc_client_s *pclientFlow;
  ptin_HwEthEvcBridge_t     bridge;
  ptin_HwEthEvcFlow_t       evcFlow;
  L7_RC_t                   res;
  L7_RC_t                   rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_id].in_use || !evcs[evc_id].intf[intf_idx].in_use)
  {
    PT_LOG_WARN(LOG_CTX_EVC,"EVC %u not active, or port %u not valid!", evc_id, intf_idx);
    return L7_SUCCESS;
  }

  /* Only stacked services have clients */
  if (IS_EVC_STD(evc_id) && !IS_EVC_STACKED(evc_id))
  {
    PT_LOG_WARN(LOG_CTX_EVC,"EVC %u do not allow clients/flows!", evc_id);
    return L7_SUCCESS;
  }

  memset(&bridge, 0x00, sizeof(bridge));
  bridge.index          = evcs[evc_id].extended_id;
  bridge.intf.intf_type = intf_idx < PTIN_SYSTEM_N_PORTS ? PTIN_EVC_INTF_PHYSICAL : PTIN_EVC_INTF_LOGICAL;
  bridge.intf.intf_id   = intf_idx < PTIN_SYSTEM_N_PORTS ? intf_idx : intf_idx - PTIN_SYSTEM_N_PORTS;
  bridge.intf.mef_type  = evcs[evc_id].intf[intf_idx].type;

  memset(&evcFlow, 0x00, sizeof(evcFlow));
  evcFlow.evc_idx = evc_id;
  evcFlow.ptin_intf.intf_type = intf_idx < PTIN_SYSTEM_N_PORTS ? PTIN_EVC_INTF_PHYSICAL : PTIN_EVC_INTF_LOGICAL;
  evcFlow.ptin_intf.intf_id   = intf_idx < PTIN_SYSTEM_N_PORTS ? intf_idx : intf_idx - PTIN_SYSTEM_N_PORTS;
  evcFlow.uni_ovid = pclientFlow->uni_ovid;

  /* Get all clients */
  pclientFlow = L7_NULLPTR;
  while (dl_queue_get_head(&evcs[evc_id].intf[intf_idx].clients, (dl_queue_elem_t **) &pclientFlow)==NOERR &&
         pclientFlow != L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_EVC,"Going to remove uni_ovid %u from port %u", pclientFlow->uni_ovid, intf_idx);

    /* Clean client */
    res = ptin_evc_pclientFlow_clean(evc_id, intf_idx, pclientFlow, L7_TRUE);
    if ( res != L7_SUCCESS )
    {
      PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning client/flow",evc_id);
      rc = L7_FAILURE;
    }

    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    /* For QUATTRO services, we have flows instead of bridges */
    if (IS_EVC_QUATTRO(evc_id))
    {
      /* Clean client */
      res = ptin_evc_flow_unconfig(&evcFlow);
      if ( res != L7_SUCCESS )
      {
        PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error removing flow",evc_id);
        rc = L7_FAILURE;
      }
    }
    else
    #endif
    {
    #if (!PTIN_BOARD_IS_DNX)
      bridge.intf.vid = pclientFlow->uni_ovid;
      bridge.inn_vlan = pclientFlow->int_ivid;

      /* Remove this bridge (client) */
      if (ptin_evc_p2p_bridge_remove(&bridge)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error removing bridge for client of cvlan=%u attached to intf=%u/%u",
                evc_id, bridge.inn_vlan, bridge.intf.intf_type, bridge.intf.intf_id);
        rc = L7_FAILURE;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_EVC,"EVC #%u: Client of cvlan %u attached to intf=%u/%u removed",
                  evc_id, bridge.inn_vlan, bridge.intf.intf_type, bridge.intf.intf_id);
      }
    #endif /* !PTIN_BOARD_IS_DNX */
    }
  }

  return rc;
}

/**
 * Remove client
 * 
 * @param evc_id 
 * @param intf_type 
 * @param intf_id 
 * @param cvlan 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_client_remove( L7_uint evc_id, L7_uint32 intf_idx, L7_uint cvlan )
{
  ptin_intf_t ptin_intf;
  ptin_HwEthEvcBridge_t bridge;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get intf_idx, and validate it */
  if (ptin_intf_port2ptintf(intf_idx, &ptin_intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Port %u not valid!", intf_idx);
    return L7_FAILURE;
  }

  if (!evcs[evc_id].in_use || !evcs[evc_id].intf[intf_idx].in_use)
    return L7_SUCCESS;

  /* Only stacked services have clients */
  if (IS_EVC_STD(evc_id) && !IS_EVC_STACKED(evc_id))
  {
    PT_LOG_TRACE(LOG_CTX_EVC,"This is an unstacked EVC... nothing to do!");
    return L7_SUCCESS;
  }

  /* Clean client */
  if ( ptin_evc_client_clean(evc_id, intf_idx, cvlan, L7_TRUE) != L7_SUCCESS )
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning client of cvlan=%u attached to port %u",evc_id, cvlan, intf_idx);
    return L7_FAILURE;
  }

  /* Build struct to remove bridge */
  bridge.index          = evcs[evc_id].extended_id;
  bridge.intf.intf_type = ptin_intf.intf_type;
  bridge.intf.intf_id   = ptin_intf.intf_id;
  bridge.intf.mef_type  = evcs[evc_id].intf[intf_idx].type;
  bridge.intf.vid       = 0;
  bridge.inn_vlan       = cvlan;

  /* Remove this bridge (client) */
  if (ptin_evc_p2p_bridge_remove(&bridge)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error removing bridge for client of cvlan=%u attached to intf=%u/%u",
            evc_id, bridge.inn_vlan, bridge.intf.intf_type, bridge.intf.intf_id);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_EVC,"EVC #%u: Client of cvlan %u attached to intf=%u/%u removed",
            evc_id, bridge.inn_vlan, bridge.intf.intf_type, bridge.intf.intf_id);

  return L7_SUCCESS;
}

/**
 * This function will clean all counters and profiles associated 
 * to all interfaces and clients of an EVC
 *  
 * @param evc_id : EVC index 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_clean_all( L7_uint evc_id, L7_BOOL force )
{
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_id].in_use)
    return L7_SUCCESS;

  /* Clean all interfaces profiles and counters */
  if (ptin_evc_allintfs_clean(evc_id, force)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning interfaces",evc_id);
    rc = L7_FAILURE;
  }

  /* Clean all clients profiles and counters */
  if (ptin_evc_allclients_clean(evc_id, force)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning all clients",evc_id);
    rc = L7_FAILURE;
  }

  return rc;
}

/**
 * This function will clean all counters and profiles associated 
 * to one interface of an EVC, as well to its clients
 *  
 * @param evc_id : EVC index 
 * @param intf_type 
 * @param intf_id 
 *  
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_intf_clean_all( L7_uint evc_id, L7_uint32 intf_idx, L7_BOOL force )
{
  L7_RC_t     rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_id].in_use)
    return L7_SUCCESS;

  /* Clean all interfaces profiles and counters */
  if (ptin_evc_intf_clean(evc_id, intf_idx, force)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning port %u", evc_id, intf_idx);
    rc = L7_FAILURE;
  }

  /* Clean all clients profiles and counters */
  if (ptin_evc_intfclients_clean(evc_id, intf_idx, force)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning all clients of port=%u", evc_id, intf_idx);
    rc = L7_FAILURE;
  }

  return rc;
}

/**
 * This function will clean all counters and profiles associated 
 * to a specific evc 
 *  
 * @param evc_id : EVC index 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_allintfs_clean( L7_uint evc_id, L7_BOOL force )
{
  L7_uint     intf_idx;
  L7_RC_t     rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_id].in_use)
    return L7_SUCCESS;

  /* Run all interfaces */
  for (intf_idx=0; intf_idx<PTIN_SYSTEM_N_INTERF; intf_idx++)
  {
    if (!evcs[evc_id].intf[intf_idx].in_use)
      continue;

    /* Clean interface */
    if (ptin_evc_intf_clean(evc_id, intf_idx, force)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning profiles and counters of port=%u", evc_id, intf_idx);
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**
 * Clean all clients for a specific evc
 * 
 * @param evc_id : evc index 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_allclients_clean( L7_uint evc_id, L7_BOOL force )
{
  L7_uint     intf_idx;
  L7_RC_t     rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_id].in_use)
    return L7_SUCCESS;

  /* Only stacked services have clients */
  if (IS_EVC_STD(evc_id) && !IS_EVC_STACKED(evc_id))
    return L7_SUCCESS;

  /* Run all interfaces */
  for (intf_idx=0; intf_idx<PTIN_SYSTEM_N_INTERF; intf_idx++)
  {
    if (!evcs[evc_id].intf[intf_idx].in_use)
      continue;

    /* Clean all clients of this interface */
    if (ptin_evc_intfclients_clean(evc_id, intf_idx, force)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning all clients from port %u", evc_id, intf_idx);
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**
 * Clean all clients for a specific evc and interface
 * 
 * @param evc_id : evc index
 * @param intf_idx : interface index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intfclients_clean( L7_uint evc_id, L7_uint32 intf_idx, L7_BOOL force )
{
  struct  ptin_evc_client_s *pclientFlow;
  L7_RC_t     rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_id].in_use || !evcs[evc_id].intf[intf_idx].in_use)
    return L7_SUCCESS;

  /* Only stacked services have clients */
  if (IS_EVC_STD(evc_id) && !IS_EVC_STACKED(evc_id))
    return L7_SUCCESS;

  /* SEM CLIENTS UP */
  osapiSemaTake(ptin_evc_clients_sem, L7_WAIT_FOREVER);

  /* Get all clients */
  pclientFlow = L7_NULLPTR;
  if (dl_queue_get_head(&evcs[evc_id].intf[intf_idx].clients, (dl_queue_elem_t **) &pclientFlow)==NOERR)
  {
    while ( pclientFlow != L7_NULLPTR )
    {
      /* Clean client */
      if (ptin_evc_pclientFlow_clean(evc_id, intf_idx, pclientFlow, force)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning client port=%u,cvlan=%u",
                evc_id, intf_idx, pclientFlow->int_ivid);
        rc = L7_FAILURE;
      }

      /* Get next client */
      pclientFlow = (struct ptin_evc_client_s *) dl_queue_get_next(&evcs[evc_id].intf[intf_idx].clients, (dl_queue_elem_t *) pclientFlow);
    }
  }

  /* SEM CLIENTS DOWN */
  osapiSemaGive(ptin_evc_clients_sem);

  return rc;
}

/**
 * This function will clean all counters and profiles associated 
 * to a specific evc and interface
 *  
 * @param evc_id : EVC index 
 * @param intf_idx : intferface index  
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_intf_clean( L7_uint evc_id, L7_uint32 intf_idx, L7_BOOL force )
{
  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_id].in_use || !evcs[evc_id].intf[intf_idx].in_use)
    return L7_SUCCESS;

  /* Remove counter on this interface */
  if (force /*ptin_clean_force*/ || ptin_clean_counters)
  {
    #if 1
    ptin_evcStats_profile_t stat_profile;

    memset(&stat_profile, 0x00, sizeof(ptin_evcStats_profile_t));
    stat_profile.ptin_port          = intf_idx;
    stat_profile.outer_vlan_lookup  = evcs[evc_id].intf[intf_idx].out_vlan;
    stat_profile.outer_vlan_egress  = evcs[evc_id].intf[intf_idx].out_vlan;
    stat_profile.outer_vlan_ingress = evcs[evc_id].intf[intf_idx].int_vlan;

    PT_LOG_TRACE(LOG_CTX_EVC,"Counters to be deleted:");
    PT_LOG_TRACE(LOG_CTX_EVC," ptin_port = %u",stat_profile.ptin_port);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_in   = %u",stat_profile.outer_vlan_lookup);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_int  = %u",stat_profile.outer_vlan_ingress);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_out  = %u",stat_profile.outer_vlan_egress);
    PT_LOG_TRACE(LOG_CTX_EVC," IVID_in   = %u",stat_profile.inner_vlan_ingress);
    PT_LOG_TRACE(LOG_CTX_EVC," IVID_out  = %u",stat_profile.inner_vlan_egress);
    PT_LOG_TRACE(LOG_CTX_EVC," DIP       = 0x%08x",stat_profile.dst_ip);

    /* Remove all related counters */
    ptin_evcStats_deleteAll(&stat_profile);

    #else
    if (evcs[evc_id].intf[intf_idx].counter != L7_NULL)
    {
      ptin_evcStats_delete(evcs[evc_id].intf[intf_idx].counter);
      evcs[evc_id].intf[intf_idx].counter = L7_NULL;
      if (evcs[evc_id].n_counters>0)  evcs[evc_id].n_counters--;
      PT_LOG_TRACE(LOG_CTX_EVC,"EVC #%u: Counter removed from intf=%u/%u",evc_id,ptin_intf.intf_type,ptin_intf.intf_id);
    }

    /* Remove all probes of this interface, if any */
    ptin_evc_probe_delete_all(evc_id, intf_idx);
    #endif
  }

  /* Remove bw profile on this interface */
  if (force /*ptin_clean_force*/ || ptin_clean_profiles)
  {
    #if 1
    ptin_bw_profile_t       bw_profile;

    memset(&bw_profile, 0x00, sizeof(ptin_bw_profile_t));
    bw_profile.ptin_port          = intf_idx;
    bw_profile.outer_vlan_ingress = evcs[evc_id].intf[intf_idx].int_vlan;
    bw_profile.cos                = (L7_uint8)-1;

    PT_LOG_TRACE(LOG_CTX_EVC,"Policers to be deleted:");
    PT_LOG_TRACE(LOG_CTX_EVC," ptin_port = %u",bw_profile.ptin_port);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_in   = %u",bw_profile.outer_vlan_lookup);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_int  = %u",bw_profile.outer_vlan_ingress);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_out  = %u",bw_profile.outer_vlan_egress);
    PT_LOG_TRACE(LOG_CTX_EVC," IVID_in   = %u",bw_profile.inner_vlan_ingress);
    PT_LOG_TRACE(LOG_CTX_EVC," IVID_out  = %u",bw_profile.inner_vlan_egress);
    PT_LOG_TRACE(LOG_CTX_EVC," COS       = %u",bw_profile.cos);
    PT_LOG_TRACE(LOG_CTX_EVC," MAC       = %02x:%02x:%02x:%02x:%02x:%02x",bw_profile.macAddr[0],bw_profile.macAddr[1],bw_profile.macAddr[2],bw_profile.macAddr[3],bw_profile.macAddr[4],bw_profile.macAddr[5]);

    /* Remove all related policers */
    ptin_bwPolicer_deleteAll(&bw_profile);

    #else
    unsigned long j;

    for (j=0; j<L7_COS_INTF_QUEUE_MAX_COUNT; j++)
    if (evcs[evc_id].intf[intf_idx].bwprofile[j] != L7_NULL)
    {
      ptin_bwPolicer_delete(evcs[evc_id].intf[intf_idx].bwprofile[j]);
      evcs[evc_id].intf[intf_idx].bwprofile[j] = L7_NULL;
      if (evcs[evc_id].n_bwprofiles>0)  evcs[evc_id].n_bwprofiles--;
      PT_LOG_TRACE(LOG_CTX_EVC,"EVC #%u: Profile removed from intf=%u/%u",evc_id,ptin_intf.intf_type,ptin_intf.intf_id);
    }
    #endif
  }

  return L7_SUCCESS;
}

/**
 * Clean client profiles and counters
 * 
 * @param evc_id 
 * @param intf_type 
 * @param intf_id 
 * @param cvlan 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_client_clean( L7_uint evc_id, L7_uint32 intf_idx, L7_uint cvlan, L7_BOOL force )
{
  struct ptin_evc_client_s *pclientFlow;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  if (!evcs[evc_id].in_use || !evcs[evc_id].intf[intf_idx].in_use)
    return L7_SUCCESS;

  /* Only stacked services have clients */
  if (IS_EVC_STD(evc_id) && !IS_EVC_STACKED(evc_id))
  {
    PT_LOG_TRACE(LOG_CTX_EVC,"This is an unstacked EVC... nothing to do!");
    return L7_SUCCESS;
  }

  /* Find provided client */
  ptin_evc_find_client(cvlan, &(evcs[evc_id].intf[intf_idx].clients), (dl_queue_elem_t **) &pclientFlow);

  if (pclientFlow==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Client of cvlan=%u attached to port=%u not found!",evc_id, cvlan, intf_idx);
    return L7_FAILURE;
  }

  /* Clean client */
  if (ptin_evc_pclientFlow_clean(evc_id, intf_idx, pclientFlow, force)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning client of cvlan=%u attached to port=%u!",evc_id, cvlan, intf_idx);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get next client, belonging to an EVC
 * 
 * @param evc_id      : evc index
 * @param ptin_intf   : interface
 * @param clientFlow  : Current client data
 * @param clientFlow_next : Next client data
 * 
 * @return L7_RC_t : 
 *  L7_SUCCESS tells a next client was returned
 *  L7_NO_VALUE tells there is no more clients (ivid_next==0)
 *  L7_NOT_EXIST tells the reference vlan was not found
 *  L7_NOT_SUPPORTED tells this evc does not support clients
 *  L7_FAILURE in case of error
 */
L7_RC_t ptin_evc_client_next( L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, ptin_HwEthEvcFlow_t *clientFlow, ptin_HwEthEvcFlow_t *clientFlow_next)
{
  L7_uint evc_id;
  L7_uint vid_ref;
  L7_uint intf_idx;
  struct ptin_evc_client_s *client_next;
  struct ptin_evc_client_s *pclient;

  /* Validate arguments */
  if (evc_ext_id>=PTIN_SYSTEM_N_EXTENDED_EVCS || ptin_intf==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid evc_ext_id %u", evc_ext_id);
    return L7_FAILURE;
  }

  /* Validate evc_id */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC %u is not active!",evc_id);
    return L7_FAILURE;
  }

  /* Get intf_idx, and validate it */
  if (ptin_intf_ptintf2port(ptin_intf, &intf_idx)!=L7_SUCCESS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Interface %u/%u not valid!",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Check if interface is in use by the evc */
  if (!evcs[evc_id].intf[intf_idx].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Interface %u/%u is not in use by EVC %u!",ptin_intf->intf_type,ptin_intf->intf_id,evc_id);
    return L7_FAILURE;
  }

  /* Reset pclient */
  pclient = L7_NULLPTR;

  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  if (IS_EVC_QUATTRO(evc_id))
  {
    vid_ref = (clientFlow!=L7_NULLPTR) ? clientFlow->uni_ovid : 0;

    /* Find provided client */
    ptin_evc_find_flow(vid_ref, &(evcs[evc_id].intf[intf_idx].clients), (dl_queue_elem_t **) &pclient);
  }
  else
  #endif
  if (IS_EVC_STACKED(evc_id))
  {
    vid_ref = (clientFlow!=L7_NULLPTR) ? clientFlow->int_ivid : 0;

    /* Find provided client */
    ptin_evc_find_client(vid_ref, &(evcs[evc_id].intf[intf_idx].clients), (dl_queue_elem_t **) &pclient);
  }
  else
  {
    return L7_NOT_SUPPORTED;
  }

  /* No client found? */
  if (pclient == L7_NULLPTR)
  {
    return L7_NOT_EXIST;
  }

  /* If current client was not provided, return first client */
  client_next = (clientFlow==L7_NULLPTR || clientFlow->int_ivid==0) ? pclient : pclient->next;

  /* No next client/flow? */
  if (client_next == L7_NULLPTR)
  {
    return L7_NO_VALUE;
  }

  /* Return next client data */
  if (clientFlow_next != L7_NULLPTR)
  {
    clientFlow_next->evc_idx  = evc_id;
    clientFlow_next->flags    = client_next->flags;
    clientFlow_next->int_ivid = client_next->int_ivid;
    clientFlow_next->uni_ovid = client_next->uni_ovid;
    clientFlow_next->uni_ivid = client_next->uni_ivid;
  }

  return L7_SUCCESS;
}

/**
 * Get next client, belonging to a vlan
 * 
 * @param intVid    : internal vlan
 * @param intIfNum   : intIfNum
 * @param clientFlow  : Current client data
 * @param clientFlow_next : Next client data
 * 
 * @return L7_RC_t : 
 *  L7_SUCCESS tells a next client was returned
 *  L7_NO_VALUE tells there is no more clients
 *  L7_NOT_EXIST tells the reference vlan was not found
 *  L7_NOT_SUPPORTED tells this evc does not support clients
 *  L7_FAILURE in case of error
 */
L7_RC_t ptin_evc_vlan_client_next( L7_uint intVid, L7_uint32 intIfNum, ptin_HwEthEvcFlow_t *clientFlow, ptin_HwEthEvcFlow_t *clientFlow_next)
{
  L7_uint     evc_id;
  ptin_intf_t ptin_intf;

#if 0
  /* Validate arguments */
  if (intVid>=4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (intVid=%u)",intVid);
    return L7_FAILURE;
  }
#endif

  /* Get evc id */
  evc_id = evcId_from_internalVlan[intVid];

  /* Check if this internal vlan is in use by any evc */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    return L7_FAILURE;
  }

  /* Validate evc_id */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC %u is not active!",evc_id);
    return L7_FAILURE;
  }

  /* Convert intIfNum to ptin_intf format */
  if (ptin_intf_intIfNum2ptintf(intIfNum, &ptin_intf)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC %u: Error acquiring ptin_intf from intIfNum %u!",evc_id, intIfNum);
    return L7_FAILURE;
  }

  /* Get next client */
  return ptin_evc_client_next(evcs[evc_id].extended_id, &ptin_intf, clientFlow, clientFlow_next);
}


/**
 * Set igmp instance for a particular evc
 * 
 * @param evc_ext_id 
 * @param igmp_inst 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_igmpInst_set(L7_uint32 evc_ext_id, L7_uint8 igmp_inst)
{
  ptinExtEvcIdInfoData_t  *ext_evcId_infoData;

  /* Validate given extended evc id, and get pointer to AVL node */
  if (ptin_evc_extEvcInfo_get(evc_ext_id, &ext_evcId_infoData) != L7_SUCCESS ||
      ext_evcId_infoData == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid ext_evc_id %u", evc_ext_id);
    return L7_FAILURE;
  }

  /* Set igmp instance */
  ext_evcId_infoData->igmp_inst = igmp_inst;

  return L7_SUCCESS;
}

/**
 * Get igmp instance from a particular evc
 * 
 * @param evc_ext_id 
 * @param igmp_inst (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_igmpInst_get(L7_uint32 evc_ext_id, L7_uint8 *igmp_inst)
{
  ptinExtEvcIdInfoData_t  *ext_evcId_infoData;

  /* Validate given extended evc id, and get pointer to AVL node */
  if (ptin_evc_extEvcInfo_get(evc_ext_id, &ext_evcId_infoData) != L7_SUCCESS ||
      ext_evcId_infoData == L7_NULLPTR)
  {
    //PT_LOG_ERR(LOG_CTX_EVC,"Invalid ext_evc_id %u", evc_ext_id);
    return L7_FAILURE;
  }

  /* Validate instance */
  if (ext_evcId_infoData->igmp_inst >= PTIN_SYSTEM_N_IGMP_INSTANCES)
  {
    //PT_LOG_ERR(LOG_CTX_EVC,"Invalid instance (%u) associated to ext_evc_id %u", ext_evcId_infoData->igmp_inst, evc_ext_id);
    return L7_FAILURE;
  }

  if (igmp_inst != L7_NULLPTR)
  {
    *igmp_inst = ext_evcId_infoData->igmp_inst;
  }

  return L7_SUCCESS;
}

/**
 * Set dhcp instance for a particular evc
 * 
 * @param evc_ext_id 
 * @param dhcp_inst 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_dhcpInst_set(L7_uint32 evc_ext_id, L7_uint8 dhcp_inst)
{
  ptinExtEvcIdInfoData_t  *ext_evcId_infoData;

  /* Validate given extended evc id, and get pointer to AVL node */
  if (ptin_evc_extEvcInfo_get(evc_ext_id, &ext_evcId_infoData) != L7_SUCCESS ||
      ext_evcId_infoData == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid ext_evc_id %u", evc_ext_id);
    return L7_FAILURE;
  }

  /* Set dhcp instance */
  ext_evcId_infoData->dhcp_inst = dhcp_inst;

  return L7_SUCCESS;
}

/**
 * Get dhcp instance from a particular evc
 * 
 * @param evc_ext_id 
 * @param dhcp_inst (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_dhcpInst_get(L7_uint32 evc_ext_id, L7_uint8 *dhcp_inst)
{
  ptinExtEvcIdInfoData_t  *ext_evcId_infoData;

  /* Validate given extended evc id, and get pointer to AVL node */
  if (ptin_evc_extEvcInfo_get(evc_ext_id, &ext_evcId_infoData) != L7_SUCCESS ||
      ext_evcId_infoData == L7_NULLPTR)
  {
    //PT_LOG_ERR(LOG_CTX_EVC,"Invalid ext_evc_id %u", evc_ext_id);
    return L7_FAILURE;
  }

  /* Validate instance */
  if (ext_evcId_infoData->dhcp_inst >= PTIN_SYSTEM_N_DHCP_INSTANCES)
  {
    //PT_LOG_ERR(LOG_CTX_EVC,"Invalid instance (%u) associated to ext_evc_id %u", ext_evcId_infoData->dhcp_inst, evc_ext_id);
    return L7_FAILURE;
  }

  if (dhcp_inst != L7_NULLPTR)
  {
    *dhcp_inst = ext_evcId_infoData->dhcp_inst;
  }

  return L7_SUCCESS;
}

/**
 * Set pppoe instance for a particular evc
 * 
 * @param evc_ext_id 
 * @param pppoe_inst 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_pppoeInst_set(L7_uint32 evc_ext_id, L7_uint8 pppoe_inst)
{
  ptinExtEvcIdInfoData_t  *ext_evcId_infoData;

  /* Validate given extended evc id, and get pointer to AVL node */
  if (ptin_evc_extEvcInfo_get(evc_ext_id, &ext_evcId_infoData) != L7_SUCCESS ||
      ext_evcId_infoData == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid ext_evc_id %u", evc_ext_id);
    return L7_FAILURE;
  }

  /* Set pppoe instance */
  ext_evcId_infoData->pppoe_inst = pppoe_inst;

  return L7_SUCCESS;
}

/**
 * Get pppoe instance from a particular evc
 * 
 * @param evc_ext_id 
 * @param pppoe_inst (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_pppoeInst_get(L7_uint32 evc_ext_id, L7_uint8 *pppoe_inst)
{
  ptinExtEvcIdInfoData_t  *ext_evcId_infoData;

  /* Validate given extended evc id, and get pointer to AVL node */
  if (ptin_evc_extEvcInfo_get(evc_ext_id, &ext_evcId_infoData) != L7_SUCCESS ||
      ext_evcId_infoData == L7_NULLPTR)
  {
    //PT_LOG_ERR(LOG_CTX_EVC,"Invalid ext_evc_id %u", evc_ext_id);
    return L7_FAILURE;
  }

  /* Validate instance */
  if (ext_evcId_infoData->pppoe_inst >= PTIN_SYSTEM_N_PPPOE_INSTANCES)
  {
    //PT_LOG_ERR(LOG_CTX_EVC,"Invalid instance (%u) associated to ext_evc_id %u", ext_evcId_infoData->pppoe_inst, evc_ext_id);
    return L7_FAILURE;
  }

  if (pppoe_inst != L7_NULLPTR)
  {
    *pppoe_inst = ext_evcId_infoData->pppoe_inst;
  }

  return L7_SUCCESS;
}


/**
 * Verify if a particular evc is MAC Bridge
 * 
 * @param evc_ext_id 
 * @param is_mac_bridge (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern
L7_RC_t ptin_evc_mac_bridge_check(L7_uint32 evc_ext_id, L7_BOOL *is_mac_bridge)
{
   ptinExtEvcIdInfoData_t  *ext_evcId_infoData;

   /* Validate given extended evc id, and get pointer to AVL node */
  if (ptin_evc_extEvcInfo_get(evc_ext_id, &ext_evcId_infoData) != L7_SUCCESS ||
      ext_evcId_infoData == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid ext_evc_id %u", evc_ext_id);
    return L7_FAILURE;
  }
  (*is_mac_bridge)=IS_EVC_QUATTRO(ext_evcId_infoData->evc_id);

  return L7_SUCCESS;
}

/****************************************************************************** 
 * STATIC FUNCTIONS
 ******************************************************************************/

/**
 * This function will clean all counters and profiles associated 
 * to the provided client 
 *  
 * @param evc_id    : EVC index 
 * @param ptin_port : Client port
 * @param pclient   : client pointer
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_evc_pclientFlow_clean( L7_uint evc_id, L7_uint ptin_port, struct ptin_evc_client_s *pclientFlow, L7_BOOL force )
{
  //L7_uint i;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }
  if (!evcs[evc_id].in_use || pclientFlow==L7_NULLPTR)
    return L7_SUCCESS;

  /* Only stacked services have clients */
  if (IS_EVC_STD(evc_id) && !IS_EVC_STACKED(evc_id))
    return L7_SUCCESS;

  /* Remove counters */
  if (force /*ptin_clean_force*/ || ptin_clean_counters)
  {
    #if 1
    ptin_evcStats_profile_t stat_profile;

    memset(&stat_profile, 0x00, sizeof(ptin_evcStats_profile_t));
    stat_profile.ptin_port          = ptin_port;
    stat_profile.outer_vlan_lookup  = pclientFlow->uni_ovid;
    stat_profile.outer_vlan_egress  = pclientFlow->uni_ovid;
    stat_profile.outer_vlan_ingress = pclientFlow->int_ovid;
    stat_profile.inner_vlan_ingress = pclientFlow->int_ivid;
    stat_profile.inner_vlan_egress  = pclientFlow->uni_ivid;

    PT_LOG_TRACE(LOG_CTX_EVC,"Counters to be deleted:");
    PT_LOG_TRACE(LOG_CTX_EVC," ptin_port = %u",stat_profile.ptin_port);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_in   = %u",stat_profile.outer_vlan_lookup);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_int  = %u",stat_profile.outer_vlan_ingress);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_out  = %u",stat_profile.outer_vlan_egress);
    PT_LOG_TRACE(LOG_CTX_EVC," IVID_in   = %u",stat_profile.inner_vlan_ingress);
    PT_LOG_TRACE(LOG_CTX_EVC," IVID_out  = %u",stat_profile.inner_vlan_egress);
    PT_LOG_TRACE(LOG_CTX_EVC," DIP       = 0x%08x",stat_profile.dst_ip);

    /* Remove all related counters */
    ptin_evcStats_deleteAll(&stat_profile);

    #else
    for (i=0; i<(sizeof(pclientFlow->counter)/sizeof(pclientFlow->counter[0])); i++)
    {
      if (pclientFlow->counter[i] != L7_NULL)
      {
        ptin_evcStats_delete(pclientFlow->counter[i]);
        pclientFlow->counter[i] = L7_NULL;
        if (evcs[evc_id].n_counters>0)  evcs[evc_id].n_counters--;
        PT_LOG_TRACE(LOG_CTX_EVC,"EVC #%u: Counter removed from client of cvlan=%u (outerVlan=%u)", evc_id, pclientFlow->int_ivid, pclientFlow->uni_ovid);
      }
    }
    #endif
  }

  /* Remove BW Profiles */
  if (force /*ptin_clean_force*/ || ptin_clean_profiles)
  {
    #if 1
    ptin_bw_profile_t       bw_profile;

    memset(&bw_profile, 0x00, sizeof(ptin_bw_profile_t));
    bw_profile.ptin_port          = -1;
    bw_profile.outer_vlan_ingress = evcs[evc_id].intf[ptin_port].int_vlan;
    bw_profile.inner_vlan_ingress = pclientFlow->int_ivid;
    bw_profile.cos                = (L7_uint8)-1;

    PT_LOG_TRACE(LOG_CTX_EVC,"Policers to be deleted:");
    PT_LOG_TRACE(LOG_CTX_EVC," ptin_port = %u",bw_profile.ptin_port);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_in   = %u",bw_profile.outer_vlan_lookup);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_int  = %u",bw_profile.outer_vlan_ingress);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_out  = %u",bw_profile.outer_vlan_egress);
    PT_LOG_TRACE(LOG_CTX_EVC," IVID_in   = %u",bw_profile.inner_vlan_ingress);
    PT_LOG_TRACE(LOG_CTX_EVC," IVID_out  = %u",bw_profile.inner_vlan_egress);
    PT_LOG_TRACE(LOG_CTX_EVC," COS       = %u",bw_profile.cos);
    PT_LOG_TRACE(LOG_CTX_EVC," MAC       = %02x:%02x:%02x:%02x:%02x:%02x",bw_profile.macAddr[0],bw_profile.macAddr[1],bw_profile.macAddr[2],bw_profile.macAddr[3],bw_profile.macAddr[4],bw_profile.macAddr[5]);

    /* Remove all related ingress policers */
    ptin_bwPolicer_deleteAll(&bw_profile);

    memset(&bw_profile, 0x00, sizeof(ptin_bw_profile_t));
    bw_profile.ptin_port          = ptin_port;
    bw_profile.outer_vlan_egress  = pclientFlow->uni_ovid;
    bw_profile.cos                = (L7_uint8)-1;

    PT_LOG_TRACE(LOG_CTX_EVC,"Policers to be deleted:");
    PT_LOG_TRACE(LOG_CTX_EVC," ptin_port = %u",bw_profile.ptin_port);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_in   = %u",bw_profile.outer_vlan_lookup);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_int  = %u",bw_profile.outer_vlan_ingress);
    PT_LOG_TRACE(LOG_CTX_EVC," OVID_out  = %u",bw_profile.outer_vlan_egress);
    PT_LOG_TRACE(LOG_CTX_EVC," IVID_in   = %u",bw_profile.inner_vlan_ingress);
    PT_LOG_TRACE(LOG_CTX_EVC," IVID_out  = %u",bw_profile.inner_vlan_egress);
    PT_LOG_TRACE(LOG_CTX_EVC," COS       = %u",bw_profile.cos);
    PT_LOG_TRACE(LOG_CTX_EVC," MAC       = %02x:%02x:%02x:%02x:%02x:%02x",bw_profile.macAddr[0],bw_profile.macAddr[1],bw_profile.macAddr[2],bw_profile.macAddr[3],bw_profile.macAddr[4],bw_profile.macAddr[5]);

    /* Remove all related egress policers */
    ptin_bwPolicer_deleteAll(&bw_profile);

    #else
    for (i=0; i<(sizeof(pclientFlow->bwprofile)/sizeof(pclientFlow->bwprofile[0])); i++) 
    {
     L7_uint j;

     for (j=0; j<L7_COS_INTF_QUEUE_MAX_COUNT; j++) {
          if (pclientFlow->bwprofile[i][j] != L7_NULL)
          {
            ptin_bwPolicer_delete(pclientFlow->bwprofile[i][j]);
            pclientFlow->bwprofile[i][j] = L7_NULL;
            if (evcs[evc_id].n_bwprofiles>0)  evcs[evc_id].n_bwprofiles--;
            PT_LOG_TRACE(LOG_CTX_EVC,"EVC #%u: Profile removed from client of cvlan=%u (outerVlan=%u)", evc_id, pclientFlow->int_ivid, pclientFlow->uni_ovid);
          }
     }
    }
    #endif
  }

  return L7_SUCCESS;
}

/**
 * Allocates an EVC entry from the pool
 * 
 * @author alex (9/18/2013)
 * 
 * @param evc_ext_id EVC extended index (input)
 * @param evc_id     Allocated index (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_evc_entry_allocate(L7_uint32 evc_ext_id, L7_uint *evc_id)
{
  RET_CODE_t rc;
  struct ptin_evc_entry_s *evc_pool_entry;
  ptinExtEvcIdInfoData_t  *ext_evcId_infoData;
  ptinExtEvcIdDataKey_t   ext_evcId_key;

  /* Key to search for */
  memset(&ext_evcId_key, 0x00, sizeof(ptinExtEvcIdDataKey_t));
  ext_evcId_key.ext_evcId = evc_ext_id;

  /* Search for this extended id */
  ext_evcId_infoData = (ptinExtEvcIdInfoData_t *) avlSearchLVL7( &(extEvcId_avlTree.extEvcIdAvlTree), (void *)&ext_evcId_key, AVL_EXACT);

  /* If already in use, return its (internal) evc_id */
  if (ext_evcId_infoData != L7_NULLPTR)
  {
    if (ext_evcId_infoData->evc_id < PTIN_SYSTEM_N_EVCS &&
        evcs[ext_evcId_infoData->evc_id].in_use)
    {
      PT_LOG_WARN(LOG_CTX_EVC, "Extended entry %u is already in use!", evc_ext_id);

      *evc_id = ext_evcId_infoData->evc_id;

      evcs[ext_evcId_infoData->evc_id].extended_id = evc_ext_id;  /* Save the extended id */

      /* Nothing to do */
      return L7_SUCCESS;
    }
    /* This node is not valid... delete it */
    else
    {
      PT_LOG_WARN(LOG_CTX_EVC,"Invalid node in AVL TREE eith ext_evc_id=%u", evc_ext_id);
      if (avlDeleteEntry(&(extEvcId_avlTree.extEvcIdAvlTree), (void *)&ext_evcId_key) == L7_NULLPTR)
      {
        PT_LOG_ERR(LOG_CTX_EVC,"Error removing key ext_evc_id=%u", evc_ext_id);
        return L7_FAILURE;
      }
      PT_LOG_WARN(LOG_CTX_EVC,"Cleaned AVL TREE entry");
    }
  }

  /* check if there is free space in AVL TREE */
  if (extEvcId_avlTree.extEvcIdAvlTree.count >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Np space in AVL TREE to add one more node");
    return L7_FAILURE;
  }

  /* Try to get an entry from the pool of free elements */
  rc = dl_queue_remove_head(&queue_free_evcs, (dl_queue_elem_t **) &evc_pool_entry);
  if (rc != NOERR) {
    PT_LOG_CRITIC(LOG_CTX_EVC, "There are no free EVCs available! rc=%d", rc);
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_EVC, "EVC free pool: %u of %u entries",
            queue_free_evcs.n_elems, PTIN_SYSTEM_N_EVCS);

  /* Add it to the busy queue */
  rc = dl_queue_add_tail(&queue_busy_evcs, (dl_queue_elem_t *) evc_pool_entry);
  if (rc != NOERR) {
    dl_queue_add_head(&queue_free_evcs, (dl_queue_elem_t *) evc_pool_entry);
    PT_LOG_CRITIC(LOG_CTX_EVC, "Error adding EVC to the busy queue! rc=%d", rc);
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_EVC, "EVC busy pool: %u of %u entries",
            queue_busy_evcs.n_elems, PTIN_SYSTEM_N_EVCS);

  /* Allocate new node in AVL Tree */
  if (avlInsertEntry(&(extEvcId_avlTree.extEvcIdAvlTree), (void *)&ext_evcId_key) != L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error allocating a new node");
    dl_queue_remove(&queue_busy_evcs, (dl_queue_elem_t *) evc_pool_entry);
    dl_queue_add_head(&queue_free_evcs, (dl_queue_elem_t *) evc_pool_entry);
    return L7_FAILURE;
  }
  /* Search for the newly created node */
  ext_evcId_infoData = (ptinExtEvcIdInfoData_t *) avlSearchLVL7( &(extEvcId_avlTree.extEvcIdAvlTree), (void *)&ext_evcId_key, AVL_EXACT);

  /* If already in use, return its (internal) evc_id */
  if (ext_evcId_infoData == L7_NULLPTR)
  {
    PT_LOG_CRITIC(LOG_CTX_EVC,"Something is wrong... new created node, is not found (ext_evc_id=%u)!",ext_evcId_key.ext_evcId);
    return L7_FAILURE;
  }
  /* Fill remaining data into AVL node */
  ext_evcId_infoData->evc_id      = evc_pool_entry->evc_id;
  ext_evcId_infoData->igmp_inst   = (L7_uint8) -1;
  ext_evcId_infoData->dhcp_inst   = (L7_uint8) -1;
  ext_evcId_infoData->pppoe_inst  = (L7_uint8) -1;

  /* EVC id to return */
  *evc_id = evc_pool_entry->evc_id;                     /* output var. */

  evcs[evc_pool_entry->evc_id].in_use = L7_TRUE;        /* ...in use */
  evcs[evc_pool_entry->evc_id].extended_id = evc_ext_id;/* Save the extended */

  return L7_SUCCESS;
}

/**
 * Frees EVC pool entry and resets the EVC to the default parameters (empty)
 * 
 * @param evc_ext_id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 */
static L7_RC_t ptin_evc_entry_free(L7_uint32 evc_ext_id)
{
  L7_RC_t rc;
  L7_uint evc_id;
  struct ptin_evc_entry_s *evc_pool_entry;
  ptinExtEvcIdInfoData_t  *ext_evcId_infoData;
  ptinExtEvcIdDataKey_t   ext_evcId_key;

  /* Key to search for */
  memset(&ext_evcId_key, 0x00, sizeof(ptinExtEvcIdDataKey_t));
  ext_evcId_key.ext_evcId = evc_ext_id;

  /* Search for this extended id */
  ext_evcId_infoData = (ptinExtEvcIdInfoData_t *) avlSearchLVL7( &(extEvcId_avlTree.extEvcIdAvlTree), (void *)&ext_evcId_key, AVL_EXACT);

  /* If already in use, return its (internal) evc_id */
  if (ext_evcId_infoData == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Extended evc id %u not found in AVL tree", evc_ext_id);
    return L7_SUCCESS;
  }

  /* Get internal evc id and validate it */
  evc_id = ext_evcId_infoData->evc_id;

  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid evc id %u", evc_id);
    return L7_SUCCESS;
  }

  /* Reset EVC data */
  ptin_evc_entry_init(evc_id);

  /* Get the EVC entry based on it index */
  evc_pool_entry = &evcs_pool[evc_id];

  /* Remove the EVC from the busy queue */
  rc = dl_queue_remove(&queue_busy_evcs, (dl_queue_elem_t *) evc_pool_entry);
  if (rc != NOERR) {
    PT_LOG_CRITIC(LOG_CTX_EVC, "Entry not found! rc=%d", rc);
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_EVC, "EVC busy pool: %u of %u entries",
            queue_busy_evcs.n_elems, PTIN_SYSTEM_N_EVCS);

  /* Add it to the free queue */
  rc = dl_queue_add_tail(&queue_free_evcs, (dl_queue_elem_t *) evc_pool_entry);
  if (rc != NOERR) {
    PT_LOG_CRITIC(LOG_CTX_EVC, "Error adding EVC to the free queue! rc=%d", rc);
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_EVC, "EVC free pool: %u of %u entries",
            queue_free_evcs.n_elems, PTIN_SYSTEM_N_EVCS);

  return L7_SUCCESS;
}


/**
 * Get ExtEvc AVL node 
 * If Related EVC is valid and in use, L7_SUCCESS will be 
 * returned. Otherwise, L7_FAILURE will be returned. 
 * 
 * @param evc_ext_id : extended evc id
 * @param infoData : AVL node pointer (output)
 * 
 * @return L7_RC_t :  L7_SUCCESS (EVC valid) 
 *                    L7_FAILURE (EVC not valid)
 */
static L7_RC_t ptin_evc_extEvcInfo_get(L7_uint32 evc_ext_id, ptinExtEvcIdInfoData_t **infoData)
{
  ptinExtEvcIdInfoData_t  *ext_evcId_infoData;
  ptinExtEvcIdDataKey_t   ext_evcId_key;

  /* Reset infoData pointer */
  if (infoData != L7_NULLPTR)
  {
    *infoData = L7_NULLPTR;
  }

  /* Validate arguments */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid eEVC %u", evc_ext_id);
    return L7_FAILURE;
  }

  /* Key to search for */
  memset(&ext_evcId_key, 0x00, sizeof(ptinExtEvcIdDataKey_t));
  ext_evcId_key.ext_evcId = evc_ext_id;

  /* Search for this extended id */
  ext_evcId_infoData = (ptinExtEvcIdInfoData_t *) avlSearchLVL7( &(extEvcId_avlTree.extEvcIdAvlTree), (void *)&ext_evcId_key, AVL_EXACT);

  /* Return pointer to AVL node */
  if (infoData != L7_NULLPTR)
  {
    *infoData = ext_evcId_infoData;
  }

  /* Don't exist? */
  if (ext_evcId_infoData == L7_NULLPTR)
  {
    //PT_LOG_ERR(LOG_CTX_EVC,"Extended evc id %u not found in AVL tree", evc_ext_id);
    return L7_FAILURE;
  }

  /* Valid EVC id? */
  if (ext_evcId_infoData->evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid evc id %u (from ext_evc_id=%u)", ext_evcId_infoData->evc_id, evc_ext_id);
    return L7_FAILURE;
  }

  /* EVC in use? */
  if (!evcs[ext_evcId_infoData->evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"evc id %u not in use", ext_evcId_infoData->evc_id);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get L3 Intf Id of EVC Port
 * 
 * 
 * @param evc_ext_id 
 * @param intfNum 
 * @param l3_intf_id 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_evc_l3_intf_get(L7_uint32 evc_ext_id, L7_uint32 intfNum, L7_int *l3_intf_id)
{
  L7_uint32   evc_id = (L7_uint32) -1;
   L7_uint32  ptin_port = (L7_uint32) -1;

  /* Validate arguments */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS || intfNum == 0 || intfNum >= PTIN_SYSTEM_N_INTERF || l3_intf_id == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid Parameters [eEVC:%u intfNum:%u l3_intf_id:%p]", evc_ext_id, intfNum, l3_intf_id);
    return L7_FAILURE;
  }

  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS || evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Failed to Obtain Internal Id [evc_ext_id:%u, evc_id:%u]", evc_ext_id, evc_id);
    return L7_FAILURE;
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_EVC, "Internal evcId:%u (internal index %u)...", evc_ext_id, evc_id);
  }

  /* EVC in use? */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"evc id %u not in use", evc_ext_id);
    return L7_FAILURE;
  }

  if (ptin_intf_intIfNum2port(intfNum, &ptin_port) != L7_SUCCESS || ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Failed to Obtain ptin_port:%u from  intfNum:%u", ptin_port, intfNum);
    return L7_FAILURE;
  }

  if (evcs[evc_id].intf[ptin_port].in_use == L7_FALSE)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"ptin port %u (intfNum:%u) is not in use on evc id %u", ptin_port, intfNum, evc_ext_id);
    return L7_FAILURE;
  }

  if (!IS_EVC_IPTV(evc_id)) 
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC Id %u is not IPTV", evc_ext_id);
    return L7_FAILURE;
  }

  if (evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"ptin port %u (intfNum:%u) type:%u different from leaf :%u", ptin_port, intfNum, evcs[evc_id].intf[ptin_port].type, PTIN_EVC_INTF_LEAF);
    return L7_FAILURE;
  }

  *l3_intf_id = evcs[evc_id].intf[ptin_port].l3_intf_id;
  return L7_SUCCESS;
}

/**
 * Get L3 Multicast Group of an EVC 
 * 
 * 
 * @param evc_ext_id 
 * @param multicast_group 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_evc_l3_multicast_group_get(L7_uint32 evc_ext_id, L7_int *multicast_group)
{
  L7_uint32   evc_id = (L7_uint32) -1;

  /* Validate arguments */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS || multicast_group == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid Parameters [eEVC:%u multicast_group:0x%x]", evc_ext_id, multicast_group);
    return L7_FAILURE;
  }

  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS || evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Failed to Obtain Internal Id [evc_ext_id:%u, evc_id:%u]", evc_ext_id, evc_id);
    return L7_FAILURE;
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_EVC, "Internal evcId:%u (internal index %u)...", evc_ext_id, evc_id);
  }

  /* EVC in use? */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"evc id %u not in use", evc_ext_id);
    return L7_FAILURE;
  }

  *multicast_group = evcs[evc_id].multicast_group;
  return L7_SUCCESS;
}


/**
 * Convert EVC extended id into the internal id
 * 
 * @author alex (9/19/2013)
 * 
 * @param evc_ext_id 
 * @param evc_id 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_evc_ext2int(L7_uint32 evc_ext_id, L7_uint32 *evc_id)
{
  ptinExtEvcIdInfoData_t  *ext_evcId_infoData;

  /* Validate given extended evc id, and get pointer to AVL node */
  if (ptin_evc_extEvcInfo_get(evc_ext_id, &ext_evcId_infoData) != L7_SUCCESS ||
      ext_evcId_infoData == L7_NULLPTR)
  {
    //PT_LOG_ERR(LOG_CTX_EVC,"Invalid ext_evc_id %u", evc_ext_id);
    return L7_FAILURE;
  }

  /* Return internal evc id */
  if (evc_id != NULL)
  {
    *evc_id = ext_evcId_infoData->evc_id;
  }

  return L7_SUCCESS;
}

///**
// * Convert EVC id into the extended id
// *
// * @author alex (9/19/2013)
// *
// * @param evc_id
// * @param evc_ext_id
// *
// * @return L7_RC_t
// */
//static L7_RC_t ptin_evc_int2ext(L7_uint32 evc_id, L7_uint32 *evc_ext_id)
//{
//  if (evc_id >= PTIN_SYSTEM_N_EVCS) {
//    PT_LOG_ERR(LOG_CTX_EVC, "Invalid EVC id %u", evc_id);
//    return L7_FAILURE;
//  }
//
//  if (!evcs[evc_id].in_use) {
//    PT_LOG_ERR(LOG_CTX_EVC, "EVC %u not in use", evc_id);
//    return L7_FAILURE;
//  }
//
//  if (evc_ext_id != NULL)
//    *evc_ext_id = evcs[evc_id].extended_id;
//
//  return L7_SUCCESS;
//}

/**
 * Init an EVC entry to the default parameters (empty)
 * 
 * @param evc_id 
 */
static void ptin_evc_entry_init(L7_uint evc_id)
{
  L7_uint i;
  ptinExtEvcIdDataKey_t ext_evcId_key;

  /* Validate evc id */
  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid evc id %u", evc_id);
    return;
  }

  /* Delete AVL node */
  if (evcs[evc_id].in_use && evcs[evc_id].extended_id < PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    memset(&ext_evcId_key, 0x00, sizeof(ptinExtEvcIdDataKey_t));
    ext_evcId_key.ext_evcId = evcs[evc_id].extended_id;

    /* remove entry from AVL tree */
    if (avlDeleteEntry(&(extEvcId_avlTree.extEvcIdAvlTree), (void *)&ext_evcId_key) == L7_NULLPTR)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Error removing node with ext_evc_id=%u / evc_id=%u", ext_evcId_key.ext_evcId, evc_id);
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_EVC,"Removed AVL node with ext_evc_id=%u / evc_id=%u", ext_evcId_key.ext_evcId, evc_id);
    }
  }

  #if (1)   /* EVCid extended feature */
  /* Release free vlan queue */
  ptin_evc_freeVlanQueue_free(evcs[evc_id].queue_free_vlans);
  #endif

  /* Reset EVC entry (set all memory to 0x00) */
  memset(&evcs[evc_id], 0x00, sizeof(evcs[evc_id]));

  /* Initialize free clients queue (per interface) */
  for (i=0; i<(sizeof(evcs[0].intf)/sizeof(evcs[0].intf[0])); i++)
    dl_queue_init(&evcs[evc_id].intf[i].clients);
}


/**
 * Configure an interface of a particular EVC
 * (and updates EVCs struct with the configuration)
 * 
 * @param evc_id   EVC #
 * @param intf_cfg  Pointer to the interface config data
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_evc_intf_add(L7_uint evc_id, ptin_HwEthMef10Intf_t *intf_cfg)
{
  L7_uint32          ptin_port;
  L7_BOOL            is_p2p, is_quattro, is_stacked;
  L7_BOOL            is_root;
  L7_BOOL            mac_learning;
  L7_BOOL            cpu_trap;
  L7_BOOL            iptv_flag;
  L7_uint16          int_vlan;
  L7_uint16          root_vlan;
  L7_uint32          intIfNum;
#if (!PTIN_BOARD_IS_DNX)
  ptin_intf_t        intf;
  ptin_dtl_l3_intf_t l3_intf;
#endif
  L7_RC_t            rc = L7_SUCCESS;

  /* Correct params */
  if (intf_cfg->vid_inner >= 4096)
  {
    intf_cfg->vid_inner = 0;
  }

  is_p2p       = (evcs[evc_id].flags & PTIN_EVC_MASK_P2P        ) == PTIN_EVC_MASK_P2P;
  is_quattro   = (evcs[evc_id].flags & PTIN_EVC_MASK_QUATTRO    ) == PTIN_EVC_MASK_QUATTRO;
  is_stacked   = (evcs[evc_id].flags & PTIN_EVC_MASK_STACKED    ) == PTIN_EVC_MASK_STACKED;
  mac_learning = (evcs[evc_id].flags & PTIN_EVC_MASK_MACLEARNING) == PTIN_EVC_MASK_MACLEARNING;
  cpu_trap     = (evcs[evc_id].flags & PTIN_EVC_MASK_CPU_TRAPPING)== PTIN_EVC_MASK_CPU_TRAPPING;
  iptv_flag    = (evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV)     == PTIN_EVC_MASK_MC_IPTV;
  is_root      =  intf_cfg->mef_type == PTIN_EVC_INTF_ROOT;
  root_vlan    =  evcs[evc_id].rvlan;

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: adding intf %u/%u (MEF %s) ...",
               evc_id,
               intf_cfg->intf_type, intf_cfg->intf_id,
               is_root ? "Root":"Leaf");

  /* Get intIfNum */
  if (ptin_intf_typeId2port(intf_cfg->intf_type, intf_cfg->intf_id, &ptin_port) != L7_SUCCESS ||
      ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error converting intf %u/%u to ptin_port/intIfNum", evc_id, intf_cfg->intf_type, intf_cfg->intf_id);
    return L7_FAILURE;
  }

#if (PTIN_BOARD_IS_DNX)
  ptin_HwEthEvcFlow_t evcFlow;
  L7_BOOL is_new_intf;

  /* VSI */
  int_vlan = evcs[evc_id].rvlan;

  is_new_intf = !evcs[evc_id].intf[ptin_port].in_use;

  if (is_new_intf)
  {
    /* USe regular VLANs */
    if (int_vlan <= 4095)
    {
      if (switching_intf_add(ptin_port, int_vlan) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error adding port %u to VLAN %u", evc_id, ptin_port, int_vlan);
        return L7_FAILURE;
      }
    }

    memset(&evcs[evc_id].intf[ptin_port], 0x00, sizeof(struct ptin_evc_intf_s)); 

    /* Update intf struct (common info) */
    evcs[evc_id].intf[ptin_port].in_use   = L7_TRUE;
    evcs[evc_id].intf[ptin_port].type     = intf_cfg->mef_type;
    evcs[evc_id].intf[ptin_port].int_vlan = int_vlan;

    if (is_root)
    {
      /* Is the first root port? Save its data */
      if (evcs[evc_id].n_roots == 0)
      {
        evcs[evc_id].root_info.port     = ptin_port;
        evcs[evc_id].root_info.nni_ovid = 0;
        evcs[evc_id].root_info.nni_ivid = 0;
      }
      /* One more root port */
      evcs[evc_id].n_roots++;
    }
    else
    {
      evcs[evc_id].n_leafs++;
    }

    evcs_intfs_in_use[ptin_port]++; /* Add this interface to the list of members in use */
  }

  /* Use LIFs? */
  if (int_vlan >= 4096)
  {
    /* Configure flow withing interface */
    memset(&evcFlow, 0x00, sizeof(evcFlow));
    evcFlow.evc_idx = evc_id;
    if (ptin_intf_port2ptintf(ptin_port, &evcFlow.ptin_intf) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error converting port %u to ptintf format", evc_id, ptin_port);
      return L7_FAILURE;
    }
    evcFlow.uni_ovid  = intf_cfg->vid;
    evcFlow.uni_ivid  = ((is_stacked) ? intf_cfg->vid_inner : 0);
    evcFlow.int_ivid  = ((is_stacked) ? intf_cfg->vid_inner : 0);
    evcFlow.pcp       = intf_cfg->pcp;
    evcFlow.etherType = intf_cfg->ethertype;

    rc = ptin_evc_flow_config(&evcFlow);

    if (rc != L7_SUCCESS)
    {
      /* Rollback interface config */
      if (is_new_intf)
      {
        evcs[evc_id].intf[ptin_port].in_use   = L7_FALSE; 
        evcs[evc_id].intf[ptin_port].type     = 0;
        evcs[evc_id].intf[ptin_port].int_vlan = 0;
      }
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error adding interface [ptin_port=%u OVlan=%u IVlan=%u VSI=%u]",
              evc_id, ptin_port, evcFlow.uni_ovid, evcFlow.uni_ivid, int_vlan);
      return L7_FAILURE;
    }
  }

#else /* PTIN_BOARD_IS_DNX */

  if (is_root)
  {
    int_vlan = root_vlan;   /* Vroot is the same for all the root interfaces */

    #if (PTIN_BOARD_IS_GPON)
    rc = switching_root_add(ptin_port, intf_cfg->vid,                   /* Port and outer vlan */
                            ((is_stacked) ? intf_cfg->vid_inner : 0),   /* Inner vlan */
                            int_vlan,                                   /* Internal vlan */
                            0,                                          /* New inner vlan */
                            !is_stacked,                                /* Delete egress vlan? Only for unstacked EVCs */
                            -1);                                        /* Force PCP */
    #else
    rc = switching_root_add(ptin_port, intf_cfg->vid,                   /* Port and outer vlan */
                            ((is_stacked) ? intf_cfg->vid_inner : 0),   /* Inner vlan */
                            int_vlan,                                   /* Internal vlan */
                            0,                                          /* New inner vlan */
                            L7_FALSE,                                   /* Delete egress vlan? */
                            -1);                                        /* Force PCP */
    #endif

    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error adding root interface [ptin_port=%u Vs=%u Vi=%u Vr=%u]",
              evc_id, ptin_port, intf_cfg->vid, intf_cfg->vid_inner, int_vlan);
      return L7_FAILURE;
    }

    /* Is the first root port? Save its data */
    if (evcs[evc_id].n_roots == 0)
    {
      evcs[evc_id].root_info.port     = ptin_port;
      evcs[evc_id].root_info.nni_ovid = intf_cfg->vid;
      evcs[evc_id].root_info.nni_ivid = intf_cfg->vid_inner;
    }
    /* One more root port */
    evcs[evc_id].n_roots++;
  }
  else
  {
    int_vlan = evcs[evc_id].rvlan;     /* Internal VLAN is the same for all interfaces, including leafs */

    if (switching_leaf_add(ptin_port, int_vlan) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error adding leaf [ptin_port=%u Vl=%u]",
              evc_id, ptin_port, int_vlan);

      #if (1)   /* EVCid extended feature */
      ptin_evc_vlan_free(int_vlan, evcs[evc_id].queue_free_vlans);       /* free VLAN */
      #else
      ptin_evc_matrix_vlan_free(int_vlan);
      #endif
      return L7_FAILURE;
    }
    evcs[evc_id].n_leafs++;

    /* Add translations for leaf ports */
    #if ( !PTIN_BOARD_IS_MATRIX )
    if (!is_stacked && !is_quattro)
    #endif
    {
      /* Only configure MC EVC partially if we are not at MX */
      #if ( !PTIN_BOARD_IS_MATRIX )
      if (iptv_flag)
      {
        rc = switching_mcevc_leaf_add(ptin_port, intf_cfg->vid, intf_cfg->vid_inner, int_vlan);
      }
      else
      #endif
      {
        /* Do not remove inner vlan @ egress */
        rc = switching_elan_leaf_add(ptin_port, intf_cfg->vid, 0, int_vlan, L7_FALSE, -1);
      }

      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Error adding translations for leaf interface %u (rc=%d)",ptin_port, rc);
        return L7_FAILURE;
      }
    }
    
    if (iptv_flag)
    {
      L7_uint32 frameMax = 0;
      memset(&l3_intf, 0x00, sizeof(l3_intf));

      l3_intf.vid = int_vlan;

      /* Get the MTU of this interface */
      if (usmDbIfConfigMaxFrameSizeGet(intIfNum, &frameMax) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Failed to get max MTU of intIfNum %u", intIfNum);
        return L7_FAILURE;                
      }
      l3_intf.mtu = frameMax;
      l3_intf.mtu-= (18 /*Bytes for L2 Header*/ + 4  /*Bytes for VLAN Tagging*/);
      if ( l3_intf.mtu <= 0 )
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Invalid MTU %d of intIfNum %u", l3_intf.mtu, intIfNum);
        return L7_FAILURE;                
      }

      /* Get MAC Address of this interface */
      if (nimGetIntfAddress(intIfNum, L7_NULL, l3_intf.mac_addr)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Error getting MAC address of ptin port %u", ptin_port);
        return L7_FAILURE;
      }
      /*Set the Flag of MAC ADDRESS*/
      l3_intf.flags |= PTIN_BCM_L3_ADD_TO_ARL;

      l3_intf.l3_intf_id = PTIN_HAPI_BROAD_INVALID_L3_INTF_ID;
      /*Add L3 Leaf Interface*/
      rc = dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_L3_INTF, DAPI_CMD_SET, sizeof(ptin_dtl_l3_intf_t), &l3_intf);

      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Error adding L3 leaf interface [ptin_port:%u rc:%d]",ptin_port, rc);
        return L7_FAILURE;
      }

      PT_LOG_TRACE(LOG_CTX_EVC, "Added L3 Leaf Interface [ptin_port:%u l3_intf_id:%d]", ptin_port, l3_intf.l3_intf_id);      

      rc = ptin_multicast_l3_egress_port_add(intIfNum, evcs[evc_id].multicast_group, l3_intf.l3_intf_id);

      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Error adding Egress Port to Multicast Group [ptin_port:%u l3_intf_id:%d multicast_group:0x%x rc:%d]",ptin_port, l3_intf.l3_intf_id, evcs[evc_id].multicast_group, rc);
        return L7_FAILURE;
      }

      PT_LOG_TRACE(LOG_CTX_EVC, "Egress Port Added to Multicast Group [ptin_port:%u l3_intf_id:%d multicast_group:0x%x]", ptin_port, l3_intf.l3_intf_id, evcs[evc_id].multicast_group);      
    }
  }

  evcs_intfs_in_use[ptin_port]++; /* Add this interface to the list of members in use */

  /* Update intf struct (common info) */
  evcs[evc_id].intf[ptin_port].in_use   = L7_TRUE;
  evcs[evc_id].intf[ptin_port].type     = intf_cfg->mef_type;
  evcs[evc_id].intf[ptin_port].int_vlan = int_vlan;

  if (iptv_flag)
    evcs[evc_id].intf[ptin_port].l3_intf_id = l3_intf.l3_intf_id;
  else
    evcs[evc_id].intf[ptin_port].l3_intf_id = -1;

  #ifdef PTIN_ERPS_EVC
  evcs[evc_id].intf[ptin_port].portState = PTIN_EVC_PORT_FORWARDING;
  #endif
  #if ( !PTIN_BOARD_IS_MATRIX )
  if (is_stacked && (intf_cfg->mef_type == PTIN_EVC_INTF_LEAF))
  {
    evcs[evc_id].intf[ptin_port].out_vlan   = 0xFFFF;  /* on stacked EVCs, leafs out.vid is defined per client and not per interface */
    evcs[evc_id].intf[ptin_port].inner_vlan = 0;
  }
  else
  #endif
  {
    evcs[evc_id].intf[ptin_port].out_vlan   = intf_cfg->vid;
    evcs[evc_id].intf[ptin_port].inner_vlan = intf_cfg->vid_inner;
  }

  evcs[evc_id].intf[ptin_port].counter   = L7_NULLPTR;
  {
   unsigned long j;

   for (j=0; j<L7_COS_INTF_QUEUE_MAX_COUNT; j++) evcs[evc_id].intf[ptin_port].bwprofile[j] = L7_NULLPTR;
  }

  evcs[evc_id].intf[ptin_port].clients.head    = L7_NULLPTR;
  evcs[evc_id].intf[ptin_port].clients.tail    = L7_NULLPTR;
  evcs[evc_id].intf[ptin_port].clients.n_elems = 0;

  /* Wait until all requests are attended */
  while (!dot1qQueueIsEmpty())
    osapiSleepMSec(10);

  /* If this is a protection port, remove port from the vlan (only at hardware level) */
  if ( is_root &&
       ptin_intf_is_uplinkProtection(ptin_port) &&
      !ptin_intf_is_uplinkProtectionActive(ptin_port))
  {
    ptin_vlan_port_removeFlush(ptin_port, int_vlan);
    PT_LOG_TRACE(LOG_CTX_EVC,"Root intf %u removed from all vlans", ptin_port);
  }

  /* Update snooping configuration */
  if (ptin_intf_port2ptintf(ptin_port,&intf)==L7_SUCCESS)
  {
    if ( ptin_igmp_is_evc_used(evcs[evc_id].extended_id)
    #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
         || evcs[evc_id].flags & PTIN_EVC_MASK_IGMP_PROTOCOL
    #endif
       )
    {
      ptin_igmp_snooping_trap_interface_update(evcs[evc_id].extended_id,&intf,L7_TRUE);
      PT_LOG_TRACE(LOG_CTX_EVC,"IGMP packet trapping updated for interface %u/%u",intf.intf_type,intf.intf_id);
    }
    if (ptin_dhcp_is_evc_used(evcs[evc_id].extended_id))
    {
      ptin_dhcp_snooping_trap_interface_update(evcs[evc_id].extended_id,&intf,L7_TRUE);
      PT_LOG_TRACE(LOG_CTX_EVC,"DHCP packet trapping updated for interface %u/%u",intf.intf_type,intf.intf_id);
    }
    if (ptin_pppoe_is_evc_used(evcs[evc_id].extended_id))
    {
      ptin_pppoe_snooping_trap_interface_update(evcs[evc_id].extended_id,&intf,L7_TRUE);
      PT_LOG_TRACE(LOG_CTX_EVC,"PPPoE packet trapping updated for interface %u/%u",intf.intf_type,intf.intf_id);
    }
  }
#endif /* PTIN_BOARD_IS_DNX */

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: %s# %02u (MEF %s Out.VID=%04hu Int.VID=%04hu VLAN_PORT=0x%x) successfully added",
            evc_id,
            ptin_port < PTIN_SYSTEM_N_PORTS ? "PHY":"LAG",
            ptin_port < PTIN_SYSTEM_N_PORTS ? ptin_port : ptin_port - PTIN_SYSTEM_N_PORTS,
            evcs[evc_id].intf[ptin_port].type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf",
            evcs[evc_id].intf[ptin_port].out_vlan,
            evcs[evc_id].intf[ptin_port].int_vlan,
            evcs[evc_id].intf[ptin_port].l2_vlan_port_id);

  return L7_SUCCESS;
}

/**
 * Removes an interface configuration (translation entries)
 * NOTE: the intf entry is removed from EVC struct 
 * 
 * @param evc_id EVC #
 * @param intf_cfg  Pointer to the interface config data
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_evc_intf_remove(L7_uint evc_id, ptin_HwEthMef10Intf_t *intf_cfg)
{
  L7_uint32 ptin_port, intIfNum;
  L7_BOOL is_p2p, is_quattro, is_stacked, iptv_flag;
  L7_BOOL            is_root;
  L7_uint16          out_vlan;
  L7_uint16          inn_vlan;
  L7_uint16          int_vlan;
#if (!PTIN_BOARD_IS_DNX)
  ptin_intf_t        intf;
  ptin_dtl_l3_intf_t l3_intf;
#endif
  ptin_HwEthEvcFlow_t evcFlow;
  L7_RC_t             rc;

  /* Get intIfNum */
  if (ptin_intf_typeId2port(intf_cfg->intf_type, intf_cfg->intf_id, &ptin_port) != L7_SUCCESS ||
      ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error converting intf %u/%u to ptin_port/intIfNum", evc_id, intf_cfg->intf_type, intf_cfg->intf_id);
    return L7_FAILURE;
  }

  is_p2p     = (evcs[evc_id].flags & PTIN_EVC_MASK_P2P    ) == PTIN_EVC_MASK_P2P;
  is_quattro = (evcs[evc_id].flags & PTIN_EVC_MASK_QUATTRO) == PTIN_EVC_MASK_QUATTRO;
  is_stacked = (evcs[evc_id].flags & PTIN_EVC_MASK_STACKED) == PTIN_EVC_MASK_STACKED;
  iptv_flag  = (evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV) == PTIN_EVC_MASK_MC_IPTV;
  is_root    = evcs[evc_id].intf[ptin_port].type == PTIN_EVC_INTF_ROOT;
  out_vlan   = evcs[evc_id].intf[ptin_port].out_vlan;
  inn_vlan   = evcs[evc_id].intf[ptin_port].inner_vlan;
  int_vlan   = evcs[evc_id].intf[ptin_port].int_vlan;

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: removing intf %u/%u (MEF %s Out.VID=%04u Inn.VID=%04u Int.VID=%04u) ...",
            evc_id,
            intf_cfg->intf_type, intf_cfg->intf_id,
            evcs[evc_id].intf[ptin_port].type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf",
            evcs[evc_id].intf[ptin_port].out_vlan,
            evcs[evc_id].intf[ptin_port].inner_vlan,
            evcs[evc_id].intf[ptin_port].int_vlan);

  memset(&evcFlow, 0x00, sizeof(evcFlow));
  evcFlow.evc_idx   = evc_id;
  evcFlow.ptin_intf.intf_type = intf_cfg->intf_type;
  evcFlow.ptin_intf.intf_id   = intf_cfg->intf_id;
  evcFlow.uni_ovid  = intf_cfg->vid;
  evcFlow.uni_ivid  = intf_cfg->vid_inner;
  evcFlow.pcp       = intf_cfg->pcp;
  evcFlow.etherType = intf_cfg->ethertype;

#if (PTIN_BOARD_IS_DNX)
  int_vlan = evcs[evc_id].rvlan;

  /* USe regular VLANs */
  if (int_vlan <= 4095)
  {
    if (switching_intf_remove(ptin_port, int_vlan) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing port %u from VLAN %u", evc_id, ptin_port, int_vlan);
      return L7_FAILURE;
    }
  }
  else
  {
    /* If VLAN is not provided, remove all flows */
    if (intf_cfg->vid == 0)
    {
      rc = ptin_evc_intfclientsflows_remove(evc_id, ptin_port);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing all flows belonging to port %u", ptin_port);
        return L7_FAILURE;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: all flows belonging to port %u removed", ptin_port);
      }
    }
    else
    {
      rc = ptin_evc_flow_unconfig(&evcFlow);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing interface 0x%x from VSI %u [ptin_port=%u OVlan=%u IVlan=%u]",
                   evc_id,  evcs[evc_id].intf[ptin_port].l2_vlan_port_id, evcs[evc_id].rvlan,
                   ptin_port, evcs[evc_id].intf[ptin_port].out_vlan, evcs[evc_id].intf[ptin_port].inner_vlan);
        return L7_FAILURE;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: interface 0x%x removed from VSI %u [ptin_port=%u OVlan=%u IVlan=%u]",
                     evc_id,  evcs[evc_id].intf[ptin_port].l2_vlan_port_id, evcs[evc_id].rvlan,
                     ptin_port, evcs[evc_id].intf[ptin_port].out_vlan, evcs[evc_id].intf[ptin_port].inner_vlan);
      }
    }

    /* Still have clients within this interface... do nothing */
    if (evcs[evc_id].intf[ptin_port].clients.n_elems > 0)
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: ptin_port %u still have flows... do nothing", evc_id,  ptin_port);
      return L7_SUCCESS;
    }
  }

  /* Update list of ports */
  if (is_root)
  {
    if (evcs[evc_id].n_roots > 0)  evcs[evc_id].n_roots--; 
  }
  else
  {
    if (evcs[evc_id].n_leafs > 0)  evcs[evc_id].n_leafs--; 
  }

#else /* PTIN_BOARD_IS_DNX */

  if (is_root)
  {
    int_vlan = evcs[evc_id].rvlan;

    if (switching_root_remove(ptin_port, out_vlan, ((is_stacked) ? inn_vlan : 0), int_vlan) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing root interface [ptin_port=%u Vs=%u Vi=%u Vr=%u]",
              evc_id, ptin_port, out_vlan, inn_vlan, int_vlan);
      return L7_FAILURE;
    }
    evcs[evc_id].n_roots--;
  }
  else
  {
    if (switching_leaf_remove(ptin_port, int_vlan) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing leaf [ptin_port=%u Vl=%u]",
              evc_id, ptin_port, int_vlan);
      return L7_FAILURE;
    }
    evcs[evc_id].n_leafs--;

    #if ( !PTIN_BOARD_IS_MATRIX )
    if (!is_stacked && !is_quattro)
    #endif
    {
      /* Add translations for leaf ports, only if we are in matrix board */
      L7_RC_t rc;

      /* Only configure MC EVC partially if we are not at MX */
      #if ( !PTIN_BOARD_IS_MATRIX )
      if (iptv_flag)
      {
        rc = switching_mcevc_leaf_remove(ptin_port, out_vlan, inn_vlan, int_vlan);
      }
      else
      #endif
      {
        rc = switching_elan_leaf_remove(ptin_port, out_vlan, 0, int_vlan);
      }

      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Error removing translations for leaf interface %u (rc=%d)",ptin_port, rc);
        return L7_FAILURE;
      }
    }

    if (iptv_flag)
    {
      if (evcs[evc_id].intf[ptin_port].l3_intf_id  != PTIN_HAPI_BROAD_INVALID_L3_INTF_ID)
      {
        /*Initialize Struct*/
        memset(&l3_intf, 0x00, sizeof(l3_intf));

        /*Set the Flag of L3 Id*/
        l3_intf.flags |= PTIN_BCM_L3_WITH_ID;
        /*Copy L3 Intf Id*/
        l3_intf.l3_intf_id = evcs[evc_id].intf[ptin_port].l3_intf_id;

        /*Remove L3 Leaf Interface*/
        rc = dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_L3_INTF, DAPI_CMD_CLEAR, sizeof(ptin_dtl_l3_intf_t), &l3_intf);

        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "Error adding L3 leaf interface [ptin_port:%u rc:%d]",ptin_port, rc);
          return L7_FAILURE;
        }

        PT_LOG_TRACE(LOG_CTX_EVC, "Added L3 Leaf Interface [ptin_port:%u l3_intf_id:%d]", ptin_port, l3_intf.l3_intf_id);      

        rc = ptin_multicast_l3_egress_port_remove(intIfNum, evcs[evc_id].multicast_group, l3_intf.l3_intf_id);

        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "Error adding Egress Port to Multicast Group [ptin_port:%u l3_intf_id:%d multicast_group:0x%x rc:%d]",ptin_port, l3_intf.l3_intf_id, evcs[evc_id].multicast_group, rc);
          return L7_FAILURE;
        }

        PT_LOG_TRACE(LOG_CTX_EVC, "Egress Port Added to Multicast Group [ptin_port:%u l3_intf_id:%d multicast_group:0x%x]", ptin_port, l3_intf.l3_intf_id, evcs[evc_id].multicast_group);    

      }
      else
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Invalid L3 Intf Id:%u", l3_intf.l3_intf_id);
      }
      evcs[evc_id].intf[ptin_port].l3_intf_id = PTIN_HAPI_BROAD_INVALID_L3_INTF_ID;
    }
  }

  /* Update snooping configuration */
  if (ptin_intf_port2ptintf(ptin_port,&intf)==L7_SUCCESS)
  {
    if ( ptin_igmp_is_evc_used(evcs[evc_id].extended_id)
    #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
         || evcs[evc_id].flags & PTIN_EVC_MASK_IGMP_PROTOCOL
    #endif
       )
    {
      ptin_igmp_snooping_trap_interface_update(evcs[evc_id].extended_id,&intf,L7_FALSE);
      PT_LOG_TRACE(LOG_CTX_EVC,"IGMP packet trapping updated for interface %u/%u",intf.intf_type,intf.intf_id);
    }
    if (ptin_dhcp_is_evc_used(evcs[evc_id].extended_id))
    {
      ptin_dhcp_snooping_trap_interface_update(evcs[evc_id].extended_id,&intf,L7_FALSE);
      PT_LOG_TRACE(LOG_CTX_EVC,"DHCP packet trapping updated for interface %u/%u",intf.intf_type,intf.intf_id);
    }
    if (ptin_pppoe_is_evc_used(evcs[evc_id].extended_id))
    {
      ptin_pppoe_snooping_trap_interface_update(evcs[evc_id].extended_id,&intf,L7_FALSE);
      PT_LOG_TRACE(LOG_CTX_EVC,"PPPoE packet trapping updated for interface %u/%u",intf.intf_type,intf.intf_id);
    }
  }
#endif /* PTIN_BOARD_IS_DNX */

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: %s# %02u (MEF %s Out.VID=%04u Inn.VID=%04u Int.VID=%04u VLAN_PORT=0x%x) successfully removed",
            evc_id,
            ptin_port < PTIN_SYSTEM_N_PORTS ? "PHY":"LAG",
            ptin_port < PTIN_SYSTEM_N_PORTS ? ptin_port : ptin_port - PTIN_SYSTEM_N_PORTS,
            evcs[evc_id].intf[ptin_port].type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf",
            evcs[evc_id].intf[ptin_port].out_vlan,
            evcs[evc_id].intf[ptin_port].inner_vlan,
            evcs[evc_id].intf[ptin_port].int_vlan,
            evcs[evc_id].intf[ptin_port].l2_vlan_port_id);

  if (evcs_intfs_in_use[ptin_port] > 0)
    evcs_intfs_in_use[ptin_port]--; /* Remove this interface from the list of members in use */

  /* Clear intf struct (common info) */
  evcs[evc_id].intf[ptin_port].in_use     = L7_FALSE;
  evcs[evc_id].intf[ptin_port].type       = 0;
  evcs[evc_id].intf[ptin_port].out_vlan   = 0;
  evcs[evc_id].intf[ptin_port].inner_vlan = 0;
  evcs[evc_id].intf[ptin_port].int_vlan   = 0;
  evcs[evc_id].intf[ptin_port].l2_vlan_port_id = 0;
  #ifdef PTIN_ERPS_EVC
  evcs[evc_id].intf[ptin_port].portState = PTIN_EVC_PORT_FORWARDING;
  #endif

  return L7_SUCCESS;
}

/**
 * Removes all interfaces from an EVC
 * NOTE: EVC interfaces data is cleared
 * 
 * @param evc_id 
 */
static L7_RC_t ptin_evc_intf_remove_all(L7_uint evc_id)
{
  L7_uint i;
  ptin_intf_t ptin_intf;
  ptin_HwEthMef10Intf_t intf_cfg;
  L7_RC_t rc = L7_SUCCESS;

  memset(&intf_cfg, 0x00, sizeof(intf_cfg));

  /* Remove all configured interfaces */
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
  {
    if (!evcs[evc_id].intf[i].in_use)
      continue;

    if (ptin_intf_port2ptintf(i, &ptin_intf) != L7_SUCCESS)
      continue;
    intf_cfg.intf_type = ptin_intf.intf_type;
    intf_cfg.intf_id   = ptin_intf.intf_id;

    if (ptin_evc_intf_remove(evc_id, &intf_cfg) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: failed to remove intf# %u", evc_id, i);
      rc = L7_FAILURE;
    }
  }

  return rc;
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

  /* SEM CLIENTS UP */
  osapiSemaTake(ptin_evc_clients_sem, L7_WAIT_FOREVER);

  *pelem = NULL;

  dl_queue_get_head(queue, (dl_queue_elem_t **)&pclient);

  while (pclient != NULL)
  {
    /* If inner vlan is null, the first cvlan is returned */
    if (inn_vlan == 0 || pclient->int_ivid == inn_vlan)
    {
      *pelem = (dl_queue_elem_t *) pclient;
      break;
    }
    pclient = (struct ptin_evc_client_s *) dl_queue_get_next(queue, (dl_queue_elem_t *)pclient);
  }

  /* SEM CLIENTS DOWN */
  osapiSemaGive(ptin_evc_clients_sem);
}

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
/**
 * Search flow based on uni outer VID
 * 
 * @param queue 
 * @param pelem 
 */
static void ptin_evc_find_flow(L7_uint16 uni_ovid, dl_queue_t *queue, dl_queue_elem_t **pelem)
{
  struct ptin_evc_client_s *pflow = NULL;

  /* SEM UP */

  *pelem = NULL;

  dl_queue_get_head(queue, (dl_queue_elem_t **)&pflow);

  while (pflow != NULL)
  {
    /* If inner vlan is null, the first cvlan is returned */
    if (uni_ovid == 0 || pflow->uni_ovid == uni_ovid)
    {
      *pelem = (dl_queue_elem_t *) pflow;
      break;
    }
    pflow = (struct ptin_evc_client_s *) dl_queue_get_next(queue, (dl_queue_elem_t *)pflow);
  }

  /* SEM DOWN */
}

/**
 * Search flow based on VPort id
 * 
 * @param queue 
 * @param pelem 
 */
static void ptin_evc_find_flow_fromVPort(L7_uint32 vport_id, dl_queue_t *queue, dl_queue_elem_t **pelem)
{
  struct ptin_evc_client_s *pflow = NULL;

  /* SEM UP */

  *pelem = NULL;

  dl_queue_get_head(queue, (dl_queue_elem_t **)&pflow);

  while (pflow != NULL)
  {
    /* If inner vlan is null, the first cvlan is returned */
    if (vport_id == 0 || pflow->vport_id == (vport_id & 0xffff))
    {
      *pelem = (dl_queue_elem_t *) pflow;
      break;
    }
    pflow = (struct ptin_evc_client_s *) dl_queue_get_next(queue, (dl_queue_elem_t *)pflow);
  }

  /* SEM DOWN */
}
#endif

/* Internal VLANs management functions ****************************************/

/**
 * Initializes the VLANs pool (queue of free internal VLANs)
 */
static void ptin_evc_vlan_pool_init(void)
{
  L7_uint i;
  //L7_uint block;

  memset(queue_free_vlans, 0x00, sizeof(queue_free_vlans));

#if (PTIN_BOARD_IS_DNX)
  dl_queue_init(&queue_free_vlans[PTIN_VLAN_TYPE_VLAN]);
  for (i=2; i<=4093; i++)
  {
    vlans_pool[i].vid = i;
    dl_queue_add(&queue_free_vlans[PTIN_VLAN_TYPE_VLAN], (dl_queue_elem_t*)&vlans_pool[i]);
  }
  PT_LOG_TRACE(LOG_CTX_EVC,"VLAN instances (type=%u): 2 - 4093", PTIN_VLAN_TYPE_VLAN);

  dl_queue_init(&queue_free_vlans[PTIN_VLAN_TYPE_VSI]);
  for (i=4096; i<=65535; i++)
  {
    vlans_pool[i].vid = i;
    dl_queue_add(&queue_free_vlans[PTIN_VLAN_TYPE_VSI], (dl_queue_elem_t*)&vlans_pool[i]);
  }
  PT_LOG_TRACE(LOG_CTX_EVC,"VSI instances (type=%u): 4096 - 65535", PTIN_VLAN_TYPE_VSI);
#else
  /* ELAN vlans */
  /* Unicast services */
  dl_queue_init(&queue_free_vlans[PTIN_VLAN_TYPE_CPU_MCAST]);
  for (i=PTIN_SYSTEM_EVC_MCAST_VLAN_MIN; i<=PTIN_SYSTEM_EVC_MCAST_VLAN_MAX && i<=PTIN_VLAN_MAX; i++)
  {
    vlans_pool[i].vid = i;
    dl_queue_add(&queue_free_vlans[PTIN_VLAN_TYPE_CPU_MCAST], (dl_queue_elem_t*)&vlans_pool[i]);
  }
  PT_LOG_TRACE(LOG_CTX_EVC,"Multicast vlans (type=%u): %u - %u", PTIN_VLAN_TYPE_CPU_MCAST, PTIN_SYSTEM_EVC_MCAST_VLAN_MIN, i-1);

  /* IPTV services */
  dl_queue_init(&queue_free_vlans[PTIN_VLAN_TYPE_CPU_BCAST]);
  for (i=PTIN_SYSTEM_EVC_BCAST_VLAN_MIN; i<=PTIN_SYSTEM_EVC_BCAST_VLAN_MAX && i<=PTIN_VLAN_MAX; i++)
  {
    vlans_pool[i].vid = i;
    dl_queue_add(&queue_free_vlans[PTIN_VLAN_TYPE_CPU_BCAST], (dl_queue_elem_t*)&vlans_pool[i]);
  }
  PT_LOG_TRACE(LOG_CTX_EVC,"Broadcast vlans (type=%u): %u - %u", PTIN_VLAN_TYPE_CPU_BCAST, PTIN_SYSTEM_EVC_BCAST_VLAN_MIN, i-1);

  /* Bitstream with no MAC learning vlans */
  dl_queue_init(&queue_free_vlans[PTIN_VLAN_TYPE_BITSTREAM]);
  for (i=PTIN_SYSTEM_EVC_BITSTR_VLAN_MIN; i<=PTIN_SYSTEM_EVC_BITSTR_VLAN_MAX && i<=PTIN_VLAN_MAX; i++)
  {
    vlans_pool[i].vid = i;
    dl_queue_add(&queue_free_vlans[PTIN_VLAN_TYPE_BITSTREAM], (dl_queue_elem_t*)&vlans_pool[i]);
  }
  PT_LOG_TRACE(LOG_CTX_EVC,"Bitstream vlans (type=%u): %u - %u", PTIN_VLAN_TYPE_BITSTREAM, PTIN_SYSTEM_EVC_BITSTR_VLAN_MIN, i-1);

  /* QUATTRO P2P vlans */
  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  dl_queue_init(&queue_free_vlans[PTIN_VLAN_TYPE_QUATTRO]);
  for (i=PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN; i<=PTIN_SYSTEM_EVC_QUATTRO_VLAN_MAX && i<=PTIN_VLAN_MAX; i++)
  {
    vlans_pool[i].vid = i;
    dl_queue_add(&queue_free_vlans[PTIN_VLAN_TYPE_QUATTRO], (dl_queue_elem_t*)&vlans_pool[i]);
  }
  PT_LOG_TRACE(LOG_CTX_EVC,"QUATTRO vlans (type=%u): %u - %u", PTIN_VLAN_TYPE_QUATTRO, PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN, i-1);
  #endif
#endif

  /* Reset 'evcId reference from internal vlan' array*/
  memset(evcId_from_internalVlan, 0xff, sizeof(evcId_from_internalVlan));
}


/**
 * Allocates a free VLAN queue from the pool
 * 
 * @param evc_id    : EVC index 
 * @param evc_flags : EVC flags
 * @param queue : Pointer to free vlan queue (output)
 * 
 * @return L7_RC_t L7_SUCCESS if success
 * @return L7_RC_t L7_FAILURE if there are no VLANs available
 */
static L7_RC_t ptin_evc_freeVlanQueue_allocate(L7_uint16 evc_id, L7_uint32 evc_flags, dl_queue_t **freeVlan_queue)
{
  //struct ptin_queue_s *fv_queue;

  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid EVC index (%u)",evc_id);
    return L7_FAILURE;
  }

#if (PTIN_BOARD_IS_DNX)
 #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  if ((evc_flags & PTIN_EVC_MASK_QUATTRO))
  {
    *freeVlan_queue = &queue_free_vlans[PTIN_VLAN_TYPE_VSI];
  }
  else
 #endif
  {
    *freeVlan_queue = &queue_free_vlans[PTIN_VLAN_TYPE_VLAN];
  }
#else

  if ((evc_flags & PTIN_EVC_MASK_QUATTRO) &&
      (evc_flags & PTIN_EVC_MASK_STACKED))
  {
    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    *freeVlan_queue = &queue_free_vlans[PTIN_VLAN_TYPE_QUATTRO];
    PT_LOG_TRACE(LOG_CTX_EVC, "QUATTRO Free Vlan Queue selected!");
    #else
    PT_LOG_ERR(LOG_CTX_EVC, "No QUATTRO vlan available!");
    return L7_ERROR;
    #endif
  }
  /* CPU port is on? */
  else if ((evc_flags & PTIN_EVC_MASK_CPU_TRAPPING))
  {
    if (!(evc_flags & PTIN_EVC_MASK_MC_IPTV))
    {
      *freeVlan_queue = &queue_free_vlans[PTIN_VLAN_TYPE_CPU_MCAST];
      PT_LOG_TRACE(LOG_CTX_EVC, "CPU_MCAST Free Vlan Queue selected!");
    }
    /* IPTV EVCs */
    else
    {
      *freeVlan_queue = &queue_free_vlans[PTIN_VLAN_TYPE_CPU_BCAST];
      PT_LOG_TRACE(LOG_CTX_EVC, "CPU_BCAST Free Vlan Queue selected!");
    }
  }
  /* Finally Bitstream services */
  else
  {
    *freeVlan_queue = &queue_free_vlans[PTIN_VLAN_TYPE_BITSTREAM];
    PT_LOG_TRACE(LOG_CTX_EVC, "BITSTREAM Free Vlan Queue selected!");
  }
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
  //L7_uint32 pool_index;

  /* No (free vlan) queue provided */
  if (freeVlan_queue == L7_NULLPTR)
  {
    //PT_LOG_WARN(LOG_CTX_EVC, "Null pointer given as free vlan queue... do nothing!");
    return L7_SUCCESS;
  }

#if (!PTIN_BOARD_IS_DNX)
  L7_uint32 i;

  for(i=0; i<PTIN_VLAN_TYPE_MAX; i++)
  {
    if (freeVlan_queue == &queue_free_vlans[i])
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "Stacked Free Vlan Queue given... do nothing!");
      return L7_SUCCESS;
    }
  }
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
static L7_RC_t ptin_evc_vlan_allocate(L7_uint16 *vlan, dl_queue_t *queue_vlans, L7_uint16 evc_id)
{
  struct ptin_vlan_s *pvlan;

  if (queue_vlans->n_elems == 0)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "There no VLANs available");
    return L7_FAILURE;
  }

  dl_queue_remove_head(queue_vlans, (dl_queue_elem_t**)&pvlan);

  if (switching_vlan_create(pvlan->vid) != L7_SUCCESS)
  {
    dl_queue_add_head(queue_vlans, (dl_queue_elem_t *) pvlan);

    PT_LOG_ERR(LOG_CTX_EVC, "Error creating VLAN %u", pvlan->vid);
    return L7_FAILURE;
  }

  *vlan = pvlan->vid;
  PT_LOG_TRACE(LOG_CTX_EVC, "Allocated Internal VLAN %04u (%u available)", *vlan, queue_vlans->n_elems);

  /* Mark this internal vlan, as being used by the given evc id */
#if 0
  if (pvlan->vid < 4096)
#endif
  {
    if (evc_id < PTIN_SYSTEM_N_EVCS)
    {
      evcId_from_internalVlan[pvlan->vid] = evc_id;
    }
    else
    {
      evcId_from_internalVlan[pvlan->vid] = -1;
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
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting VLAN %u", vlan);
    return L7_FAILURE;
  }

  /* Index directly to the pool array and add the element to the free queue */
  dl_queue_add_head(queue_vlans, (dl_queue_elem_t *) &vlans_pool[vlan]);

  PT_LOG_TRACE(LOG_CTX_EVC, "Freed Internal VLAN %04u (%u available)", vlan, queue_vlans->n_elems);

  /* Free this internal vlan, as not being used by any evc */
#if 0
  if (vlan<4096)
#endif
  {
    evcId_from_internalVlan[vlan] = -1;
  }

  return L7_SUCCESS;
}

/* Switching configuration functions ******************************************/

#ifdef PTIN_ERPS_EVC

/**
 * Configures a root port (unstacked EVCs) 
 *  1. Associates the internal VLAN to the root intf
 * 
 * @param root_intf     Root interface (ptin_intf)
 * @param int_vlan      Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t switching_root_unblock(L7_uint root_intf, L7_uint16 int_vlan)
{
  L7_uint   evc_id;
  L7_uint32 intIfNum;
  L7_uint16 intIfNum_list[2];
  L7_RC_t   rc = L7_SUCCESS;

#if 0
  /* Validate arguments */
  if (int_vlan >= 4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (int_vlan=%u)", int_vlan);
    return L7_FAILURE;
  }
#endif

  /* Get evc id */
  evc_id = evcId_from_internalVlan[int_vlan];

  /* Check if this internal vlan is in use by any evc */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC Not found");
    return L7_FAILURE;
  }

  /* Check if this evc is in use... if not we have a non-consistent situation */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u (int_vlan=%u) should be in use", evc_id, int_vlan);
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_EVC, "Unblocking root intf# %u [Int.VLAN=%u] on EVC#%u...",
           root_intf, int_vlan, evc_id);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(root_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", root_intf);
    return L7_FAILURE;
  }

  intIfNum_list[0] = intIfNum;
  if (dtlDot1qAddPort(0, int_vlan, intIfNum_list, 1, L7_NULLPTR, 0, L7_NULLPTR, 0) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intIfNum# %u from Int.VLAN %u (rc=%d)", intIfNum, int_vlan, rc);
    rc = L7_FAILURE;
  }
  else
  {
    /* Associate root internal vlan to the root intf */
    if (usmDbVlanMemberSet(1, int_vlan, intIfNum, L7_DOT1Q_FIXED, DOT1Q_SWPORT_MODE_NONE) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error associating root Int.VLAN %u to root intIfNum# %u (rc=%d)", int_vlan, intIfNum, rc);
      rc = L7_FAILURE;
    }
  }

  /* Wait until all requests are attended */
//while (!dot1qQueueIsEmpty())
//  osapiSleepUSec(1);

  evcs[evc_id].intf[root_intf].portState = PTIN_EVC_PORT_FORWARDING;

  return rc;
}

/**
 * Removes a root port (unstacked EVCs) 
 * 
 * @param root_intf Root interface (ptin_intf)
 * @param int_vlan  Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t switching_root_block(L7_uint root_intf, L7_uint16 int_vlan)
{
  L7_uint   evc_id;
  L7_uint32 intIfNum;
  L7_uint16 intIfNum_list[2];
  L7_RC_t   rc = L7_SUCCESS;

#if 0
  /* Validate arguments */
  if (int_vlan >= 4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (int_vlan=%u)", int_vlan);
    return L7_FAILURE;
  }
#endif

  /* Get evc id */
  evc_id = evcId_from_internalVlan[int_vlan];

  /* Check if this internal vlan is in use by any evc */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC Not found");
    return L7_FAILURE;
  }

  /* Check if this evc is in use... if not we have a non-consistent situation */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u (int_vlan=%u) should be in use", evc_id, int_vlan);
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_EVC, "Blocking root intf# %u [Int.VLAN=%u] on EVC#%u...",
           root_intf, int_vlan, evc_id);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(root_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", root_intf);
    return L7_FAILURE;
  }

  intIfNum_list[0] = intIfNum;
  if (dtlDot1qDeletePort(0, int_vlan, intIfNum_list, 1, L7_NULLPTR, 0, L7_NULLPTR, 0) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intIfNum# %u from Int.VLAN %u (rc=%d)", intIfNum, int_vlan, rc);
    rc = L7_FAILURE;
  }
  else
  {
    /* Delete intIfNum from int_vlan */
    if (usmDbVlanMemberSet(1, int_vlan, intIfNum, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intIfNum# %u from Int.VLAN %u (rc=%d)", intIfNum, int_vlan, rc);
      rc = L7_FAILURE;
    }
  }

  /* Wait until all requests are attended */
//while (!dot1qQueueIsEmpty())
//  osapiSleepUSec(1);

  evcs[evc_id].intf[root_intf].portState = PTIN_EVC_PORT_BLOCKING;

  return rc;
}

/**
 * Flushes FDB for all int.VLAN associated to this evc_id
 * 
 * @param int_vlan  Root Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t switching_fdbFlushByVlan(L7_uint16 int_vlan)
{
  L7_uint   evc_id;
  L7_RC_t   rc = L7_SUCCESS;

#if 0
  /* Validate arguments */
  if (int_vlan >= 4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (int_vlan=%u)", int_vlan);
    return L7_FAILURE;
  }
#endif

  /* Get evc id */
  evc_id = evcId_from_internalVlan[int_vlan];

  /* Check if this internal vlan is in use by any evc */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC Not found");
    return L7_FAILURE;
  }

  /* Check if this evc is in use... if not we have a non-consistent situation */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u (int_vlan=%u) should be in use", evc_id, int_vlan);
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_EVC, "Flushing EVC#%u", evc_id);

  PT_LOG_DEBUG(LOG_CTX_EVC, "Flushing Root Int.VLAN=%u", int_vlan);

  /* Flush FDB on Root VLAN */
  if (fdbFlushByVlan(int_vlan) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error Flushing Root Int.VLAN %u (rc=%d)", int_vlan, rc);
    rc = L7_FAILURE;
  }

  return rc;
}


/**
 * Returns internal VLAN and the correspondig EVC ID
 * 
 * @return L7_int evc_id on match condition
 */
L7_int switching_erps_internalVlan_get(L7_int initial_evc_id, L7_uint8 erps_ptin_port0, L7_uint8 erps_ptin_port1, L7_uint8 *vid_bmp, L7_uint16 *int_vlan)
{
  L7_uint16 vid;
  L7_int    evc_id;

  PT_LOG_DEBUG(LOG_CTX_EVC,"(initial_evc_id %d, erps_ptin_port0 %u, erps_ptin_port1 %u)", initial_evc_id, erps_ptin_port0, erps_ptin_port1);

  /* Run all EVCs */
  for (evc_id=initial_evc_id; evc_id < PTIN_SYSTEM_N_EVCS; evc_id++)
  {
    /* Skip not used EVCs */
    if (!evcs[evc_id].in_use)
      continue;

    /* If EVC is using this interface, flush Root VLAN immediately and continues to next EVC ID*/
    if ((evcs[evc_id].root_info.port == erps_ptin_port0) || (evcs[evc_id].root_info.port == erps_ptin_port1))
    {
      vid = evcs[evc_id].root_info.nni_ovid;

      if ( (vid < 1<<12) && (vid_bmp[vid/8] & 1<<(vid%8)) ) //ERP protected VID
      {
        *int_vlan = evcs[evc_id].rvlan;
        return evc_id;
      }
    }
  }

  *int_vlan = 0;
  return PTIN_SYSTEM_N_EVCS;
}

#endif


/**
 * Flushes all VLANs' FDB associated to this ptin_port
 * 
 * @param ptin_port  Port to Flush
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t switching_fdbFlushVlanByPort(L7_uint8 ptin_port)
{
  L7_int  evc_id;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_DEBUG(LOG_CTX_EVC,"(ptin_port=%u)", ptin_port);

  /* Run all EVCs */
  for (evc_id=0; evc_id < PTIN_SYSTEM_N_EVCS; evc_id++)
  {
    /* Skip not used EVCs */
    if (!evcs[evc_id].in_use)
      continue;

    /* If EVC is using this interface, flush Root VLAN immediately and continues to next EVC ID*/
    if (evcs[evc_id].intf[ptin_port].in_use)
    {
      /* Flush FDB on Root VLAN */
      PT_LOG_DEBUG(LOG_CTX_EVC,"vlan=%u", evcs[evc_id].rvlan);
      if ((rc = fdbFlushByVlan(evcs[evc_id].rvlan)) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Error Flushing Root Int.VLAN %u (rc=%d)", evcs[evc_id].rvlan, rc);
        rc = L7_FAILURE;
      }
      continue;
    }
  }

  return rc;
}

#if PTIN_IGMP_STATS_IN_EVCS
/**
 * Get a pointer to IGMP stats
 * 
 * @param int_vlan     : internal vlan
 * @param intIfNum    : interface
 * @param stats_intf  : pointer to stats
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE;
 */
L7_RC_t ptin_evc_igmp_stats_get_fromIntVlan(L7_uint16 int_vlan, L7_uint32 intIfNum, ptin_IGMP_Statistics_t **stats_intf)
{
  L7_uint evc_id;
  L7_uint32 ptin_port;

#if 0
  /* Validate Vlan */
  if (int_vlan < PTIN_VLAN_MIN || int_vlan > PTIN_VLAN_MAX)
  {
    return L7_FAILURE;
  }
#endif

  /* Get evc id */
  evc_id = evcId_from_internalVlan[int_vlan];

  /* Check if this evc is valid */
  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    //PT_LOG_ERR(LOG_CTX_EVC,"Invalid ptin_intf or EVC %u Not found", evc_id);
    return L7_FAILURE;
  }

  /* Check if this evc is in use... if not we have a non-consistent situation */
  if (!evcs[evc_id].in_use)
  {
    //PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u should be in use", evc_id);
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_intIfNum2port(intIfNum, &ptin_port)!=L7_SUCCESS || ptin_port>=PTIN_SYSTEM_N_INTERF)
  {
    //PT_LOG_ERR(LOG_CTX_EVC, "intIfNum %u is invalid", intIfNum);
    return L7_FAILURE;
  }

  /* Interface, must be in use */
  if (!evcs[evc_id].intf[ptin_port].in_use)
  {
    //PT_LOG_ERR(LOG_CTX_EVC,"Interface %u of evc %u is not active", ptin_port, evc_id);
    return L7_FAILURE;
  }

  if (stats_intf!=L7_SUCCESS)
    *stats_intf = &evcs[evc_id].stats_igmp_intf[ptin_port];

  return L7_SUCCESS;
}

/**
 * Get a pointer to IGMP stats
 * 
 * @param evc_ext_id  : EVC extended index 
 * @param ptin_intf    : interface
 * @param stats_intf   : stats
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE;
 */
L7_RC_t ptin_evc_igmp_stats_get(L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, ptin_IGMP_Statistics_t *stats_intf)
{
  L7_uint32 ptin_port;
  L7_uint32 evc_id;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2port(ptin_intf, &ptin_port)!=L7_SUCCESS || ptin_port>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "ptin_intf %u/%u is invalid", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Interface, must be in use */
  if (!evcs[evc_id].intf[ptin_port].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Interface %u of evc %u is not active", ptin_port, evc_id);
    return L7_FAILURE;
  }

  if (stats_intf!=L7_SUCCESS)
    memcpy(stats_intf, &evcs[evc_id].stats_igmp_intf[ptin_port], sizeof(ptin_IGMP_Statistics_t));

  return L7_SUCCESS;
}

/**
 * Clear IGMP stats of one interface
 * 
 * @param evc_ext_id  : EVC extended index 
 * @param ptin_intf   : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE;
 */
L7_RC_t ptin_evc_igmp_stats_clear(L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf)
{
  L7_uint32 ptin_port;
  L7_uint32 evc_id;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2port(ptin_intf, &ptin_port)!=L7_SUCCESS || ptin_port>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "ptin_intf %u/%u is invalid", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Interface, must be in use */
  if (!evcs[evc_id].intf[ptin_port].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Interface %u of evc %u is not active", ptin_port, evc_id);
    return L7_FAILURE;
  }

  /* Clear stats */
  memset(&evcs[evc_id].stats_igmp_intf[ptin_port], 0x00, sizeof(ptin_IGMP_Statistics_t));

  return L7_SUCCESS;
}

/**
 * Clear IGMP stats of all interfaces
 * 
 * @param evc_id      : EVC index 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE;
 */
L7_RC_t ptin_evc_igmp_stats_clear_all(L7_uint32 evc_ext_id)
{
  L7_uint32 evc_id;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%u]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_NOT_EXIST;
  }

  /* Clear all stats */
  memset(evcs[evc_id].stats_igmp_intf, 0x00, sizeof(ptin_IGMP_Statistics_t)*PTIN_SYSTEM_N_INTERF);

  return L7_SUCCESS;
}
#endif

/**
 * Interface add operation for DNX devices
 * 
 * @param ptin_port 
 * @param vlanId
 * 
 * @return L7_RC_t 
 */
static L7_RC_t switching_intf_add(L7_uint ptin_port, L7_uint16 vlanId)
{
  L7_uint32 intIfNum;
  L7_RC_t rc;

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(ptin_port, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", ptin_port);
    return L7_FAILURE;
  }

  /* Validate VLAN */
  if (vlanId == 0 || vlanId >= 4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "VLAN %u is not valid!", vlanId);
    return L7_FAILURE;
  }

  /* Associate root internal vlan to the root intf */
  rc = usmDbVlanMemberSet(1, vlanId, intIfNum, L7_DOT1Q_FIXED, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error associating root Int.VLAN %u to root intIfNum# %u (rc=%d)", vlanId, intIfNum, rc);
    return L7_FAILURE;
  }

  /* Configure the internal VLAN on this interface as tagged */
  rc = usmDbVlanTaggedSet(1, vlanId, intIfNum, L7_DOT1Q_TAGGED);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error setting intIfNum# %u internal VLAN %u as tagged (rc=%d)", intIfNum, vlanId, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Interface remove operation for DNX devices
 * 
 * @param ptin_port 
 * @param vlanId
 * 
 * @return L7_RC_t 
 */
static L7_RC_t switching_intf_remove(L7_uint ptin_port, L7_uint16 vlanId)
{
  L7_uint32 intIfNum;
  L7_RC_t rc;

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(ptin_port, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", ptin_port);
    return L7_FAILURE;
  }

  /* Validate VLAN */
  if (vlanId == 0 || vlanId >= 4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "VLAN %u is not valid!", vlanId);
    return L7_FAILURE;
  }

  /* Delete intIfNum from int_vlan */
  rc = usmDbVlanMemberSet(1, vlanId, intIfNum, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intIfNum# %u from Int.VLAN %u (rc=%d)", intIfNum, vlanId, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

#if (PTIN_BOARD_IS_DNX)
/**
 * Interface add operation for DNX devices
 * 
 * @param ptin_port 
 * @param out_vlan 
 * @param inner_vlan 
 * @param vsi 
 * @param vlan_port_id 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t switching_lif_add(L7_uint ptin_port, L7_uint16 out_vlan, L7_uint16 inner_vlan, L7_int8 pcp, L7_uint16 etherType,
                                 L7_uint16 vsi, L7_uint32 mcgroup, L7_uint32 *vlan_port_id)
{
  L7_uint32 intIfNum;
  L7_int virtual_port;
  L7_RC_t rc;

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(ptin_port, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", ptin_port);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "ptin_port %u / intIfNum %u, outVlan %u and innerVlan %u, vsi=%u, mcgroup=%u", ptin_port, intIfNum, out_vlan, inner_vlan, vsi, mcgroup);

  /* Create Logical Interface */
  rc = ptin_virtual_port_add(intIfNum, out_vlan, inner_vlan, out_vlan, inner_vlan, pcp, etherType, mcgroup, &virtual_port, (L7_uint8)-1);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error creating logical interface from intIfNum %u, outVlan %u and innerVlan %u", intIfNum, out_vlan, inner_vlan);
    return L7_FAILURE;
  }

  do
  {
    /* Egress translations (only configure them, if there are VLANs involved) */
    if (out_vlan != 0 || inner_vlan != 0)
    {
      rc = ptin_xlate_dnx_add(virtual_port, out_vlan, inner_vlan, -1, -1, L7_FALSE); 
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Error configuring egress translation for LIF 0x%x: outVlan %u and innerVlan %u", virtual_port, out_vlan, inner_vlan);
        break;
      }
    }

    do
    {
      /* Add this logical interface to VSI */
      rc = ptin_vsi_member_add(vsi, virtual_port);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Error adding logical interface 0x%x to VSI %u", virtual_port, vsi);
        break;
      }

      do
      {
        /* Configure flooding */
        rc = ptin_vlanBridge_flood_set(virtual_port, 0, mcgroup, mcgroup, mcgroup);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "Error configuring flooding for logical interface 0x%x", virtual_port);
          break;
        }
      }
      while (0);

      /* Undo flooding configuration */
      if (rc != L7_SUCCESS)
      {
        (void) ptin_vsi_member_remove(vsi, virtual_port);
        PT_LOG_TRACE(LOG_CTX_EVC, "VSI member addition undone.");
      }
    }
    while (0);

    /* Undo Egress translation */
    if (out_vlan != 0 || inner_vlan != 0)
    {
      if (rc != L7_SUCCESS)
      {
        (void) ptin_xlate_dnx_delete(virtual_port, L7_FALSE);
        PT_LOG_TRACE(LOG_CTX_EVC, "Egress translation undone.");
      }
    }
  }
  while (0);

  /* Undo virtual port creation */
  if (rc != L7_SUCCESS)
  {
    (void) ptin_virtual_port_remove(intIfNum, virtual_port, mcgroup);
    PT_LOG_TRACE(LOG_CTX_EVC, "LIF creation undone.");
    return rc;
  }

  /* Return created LIF */
  if (vlan_port_id != L7_NULLPTR)
  {
    *vlan_port_id = virtual_port;
  }

  return L7_SUCCESS;
}

/**
 * Interface remove operation for DNX devices
 * 
 * @author mruas (10/21/2015)
 * 
 * @param ptin_port 
 * @param vsi 
 * @param vlan_port_id 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t switching_lif_remove(L7_uint ptin_port, L7_uint16 vsi, L7_uint32 mcgroup, L7_uint32 vlan_port_id)
{
  L7_uint32 intIfNum;
  L7_RC_t rc;

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(ptin_port, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", ptin_port);
    return L7_FAILURE;
  }

  /* Configure flooding */
  rc = ptin_vlanBridge_flood_set(vlan_port_id, 0, 0, 0, 0);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deconfiguring flooding for logical interface 0x%x", vlan_port_id);
    return L7_FAILURE;
  }

  /* Add this logical interface to VSI */
  if (ptin_vsi_member_remove(vsi, vlan_port_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error removing logical interface 0x%x from VSI %u", vlan_port_id, vsi);
    return L7_FAILURE;
  }

  /* Egress translations */
  if (ptin_xlate_dnx_delete(vlan_port_id, L7_FALSE) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error removing egress translation for LIF 0x%x", vlan_port_id);
    return L7_FAILURE;
  }

  /* Create Logical Interface */
  if (ptin_virtual_port_remove(intIfNum, vlan_port_id, mcgroup) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error destroying logical interface 0x%x", vlan_port_id);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

#else /* PTIN_BOARD_IS_DNX */

/**
 * Configures a root port (for stacked and unstacked EVCs) 
 *  1. Associates the internal VLAN to the root intf
 *  2. Adds egress and ingress xlate entries (Out.VLAN<=>Int.VLAN)
 * 
 * @param root_intf     Root interface (ptin_intf)
 * @param out_vlan      Outer VLAN
 * @param int_vlan      Inner VLAN 
 * @param force_pcp     Force ingress packets to have this pcp
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_root_add(L7_uint root_intf, L7_uint16 out_vlan, L7_uint16 inner_vlan, L7_uint16 int_vlan, L7_uint16 new_innerVlan,
                                  L7_BOOL egress_del_ivlan, L7_int force_pcp)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Adding root intf# %u [Out.VID=%u Inn.VID=%u Int.VLAN=%u]...",
           root_intf, out_vlan, inner_vlan, int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(root_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", root_intf);
    return L7_FAILURE;
  }

  #ifdef PTIN_ERPS_EVC
  if (ptin_hal_erps_isPortBlocked(root_intf, out_vlan, int_vlan) != L7_TRUE)
  {
  #endif
    /* Associate root internal vlan to the root intf */
    rc = usmDbVlanMemberSet(1, int_vlan, intIfNum, L7_DOT1Q_FIXED, DOT1Q_SWPORT_MODE_NONE);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error associating root Int.VLAN %u to root intIfNum# %u (rc=%d)", int_vlan, intIfNum, rc);
      return L7_FAILURE;
    }
  #ifdef PTIN_ERPS_EVC
  }
  else {
    PT_LOG_TRACE(LOG_CTX_EVC, "Association of root Int.VLAN %u to root intIfNum# %u will be done later by ERPS", int_vlan, intIfNum);
  }
  #endif

  /* Configure the internal VLAN on this interface as tagged */
  rc = usmDbVlanTaggedSet(1, int_vlan, intIfNum, L7_DOT1Q_TAGGED);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error setting intIfNum# %u internal VLAN %u as tagged (rc=%d)", intIfNum, int_vlan, rc);
    return L7_FAILURE;
  }

  /* Add ingress xlate entry: (root_intf) out_vlan -> int_vlan */
  rc = ptin_xlate_ingress_add(intIfNum, out_vlan, inner_vlan, int_vlan, new_innerVlan, force_pcp, -1);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding intIfNum# %u xlate Ingress entry [Out.VLAN %u + Inn.VLAN %u => Int.VLAN] %u (rc=%d)",
            intIfNum, out_vlan, inner_vlan, int_vlan, rc);
    return L7_FAILURE;
  }

  /* Add egress xlate entry: (root_intf) int_vlan -> out_vlan */
  rc = ptin_xlate_egress_add(intIfNum, int_vlan, new_innerVlan, out_vlan, (egress_del_ivlan) ? (L7_uint16)-1 : inner_vlan, -1, -1);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding intIfNum# %u xlate Egress entry [Int.VLAN %u => Out.VLAN %u] (rc=%d)",
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
static L7_RC_t switching_root_remove(L7_uint root_intf, L7_uint16 out_vlan, L7_uint16 inner_vlan, L7_uint16 int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing root intf# %u [Out.VID=%u Inn.VID=%u Int.VLAN=%u]...",
           root_intf, out_vlan, inner_vlan, int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(root_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", root_intf);
    return L7_FAILURE;
  }

  /* Delete egress xlate entry: (root_intf) int_vlan -> out_vlan */
  rc = ptin_xlate_egress_delete(intIfNum, int_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intIfNum# %u xlate Egress entry [Int.VLAN %u => Out.VLAN %u} (rc=%d)",
            intIfNum, int_vlan, out_vlan, rc);
    return L7_FAILURE;
  }

  /* Delete ingress xlate entry: (root_intf) out_vlan -> int_vlan */
  rc = ptin_xlate_ingress_delete(intIfNum, out_vlan, inner_vlan);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intIfNum# %u xlate Ingress entry [Out.VLAN %u+Inn.VLAN %u => Int.VLAN %u] (rc=%d)",
            intIfNum, out_vlan, inner_vlan, int_vlan, rc);
    return L7_FAILURE;
  }

  /* Delete intIfNum from int_vlan */
  rc = usmDbVlanMemberSet(1, int_vlan, intIfNum, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intIfNum# %u from Int.VLAN %u (rc=%d)", intIfNum, int_vlan, rc);
    return L7_FAILURE;
  }

  #ifdef PTIN_ERPS_EVC
  ptin_hal_erps_evcProtectedRemove(root_intf, out_vlan, int_vlan);
  #endif

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

  PT_LOG_TRACE(LOG_CTX_EVC, "Adding leaf intf# %u [Int.VLAN=%u]...",
           leaf_intf, leaf_int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(leaf_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Associate leaf internal vlan to the leaf intf */
  rc = usmDbVlanMemberSet(1, leaf_int_vlan, intIfNum, L7_DOT1Q_FIXED, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error associating leaf Int.VLAN %u to leaf intIfNum# %u (rc=%d)", leaf_int_vlan, intIfNum, rc);
    return L7_FAILURE;
  }

  /* Configure the internal VLAN on this interface as tagged */
  rc = usmDbVlanTaggedSet(1, leaf_int_vlan, intIfNum, L7_DOT1Q_TAGGED);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error setting intIfNum# %u internal VLAN %u as tagged (rc=%d)", intIfNum, leaf_int_vlan, rc);
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

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing leaf intf# %u [Int.VLAN=%u]...",
           leaf_intf, leaf_int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(leaf_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Delete intIfNum from leaf_int_vlan */
  rc = usmDbVlanMemberSet(1, leaf_int_vlan, intIfNum, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intIfNum# %u from Int.VLAN %u (rc=%d)", intIfNum, leaf_int_vlan, rc);
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
static L7_RC_t switching_elan_leaf_add(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan, L7_BOOL egress_del_ivid, L7_int force_pcp)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Adding stacked leaf intf# %u [Int.VLAN=%u]...",
           leaf_intf, int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(leaf_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Add ingress xlate entry: (leaf_intf) (Vs',Vc) => (Vr,Vc) */
  rc = ptin_xlate_ingress_add(intIfNum, leaf_out_vlan, 0, int_vlan, leaf_inner_vlan, -1, -1);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding intf %u xlate Ingress entry [Leaf Out.VLAN %u + Inn.VLAN %u => Root Int.VLAN %u] (rc=%d)",
            leaf_intf, leaf_out_vlan, leaf_inner_vlan, int_vlan, rc);
    return L7_FAILURE;
  }

  /* Add egress xlate entry: (leaf_intf) (Vr,Vc) => (Vs',Vc); innerVlan is to be removed */
  rc = ptin_xlate_egress_add(intIfNum, int_vlan, leaf_inner_vlan, leaf_out_vlan, (egress_del_ivid) ? (L7_uint16)-1 : leaf_inner_vlan, force_pcp, -1);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding intf %u xlate Egress entry [Root Int.VLAN %u + Inn.VLAN %u => Leaf Out.VLAN %u] (rc=%d)",
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
static L7_RC_t switching_elan_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing stacked leaf intf# %u [Int.VLAN=%u]...",
           leaf_intf, int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(leaf_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Add ingress xlate entry: (leaf_intf) (Vs',Vc) => (Vr,Vc) */
  rc = ptin_xlate_ingress_delete(intIfNum, leaf_out_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intf %u xlate Ingress entry [Leaf Out.VLAN %u + Inn.VLAN %u] (rc=%d)",
            leaf_intf, leaf_out_vlan, 0, rc);
    return L7_FAILURE;
  }

  /* Add egress xlate entry: (leaf_intf) (Vr,Vc) => (Vs',Vc) */
  rc = ptin_xlate_egress_delete(intIfNum, int_vlan, leaf_inner_vlan);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intf %u xlate Egress entry [Root Int.VLAN %u + Inn.VLAN %u (rc=%d)",
            leaf_intf, int_vlan, leaf_inner_vlan, leaf_out_vlan, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

#if ( !PTIN_BOARD_IS_MATRIX )
/**
 * Configures a leaf port (only for MC EVCs) 
 * 
 * @param leaf_intf Leaf interface (ptin_intf) 
 * @param leaf_out_vlan : External outer vlan 
 * @param leaf_inner_vlan : External inner vlan 
 * @param int_vlan : Internal VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_mcevc_leaf_add(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Adding stacked leaf intf# %u [Int.VLAN=%u]...",
           leaf_intf, int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(leaf_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Add egress xlate entry: (leaf_intf) (Vr,Vc) => (Vs',Vc); innerVlan is to be added */
  /* Only for Multicast services at PON ports:
     GPON boards require to replace outer tag to vlan=gem_id + prio 0, and add an inner tag with inner_prio = outer_prio */
  rc = ptin_xlate_egress_add(intIfNum, int_vlan, (L7_uint16)-1, leaf_out_vlan, leaf_inner_vlan, -1, -1);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding intf %u xlate Egress entry [Root Int.VLAN %u => Leaf Out.VLAN %u + Leaf Inn.VLAN %u] (rc=%d)",
            leaf_intf, int_vlan, leaf_out_vlan, leaf_inner_vlan, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Removes a leaf port (for MC EVCs) 
 * 
 * @param leaf_intf Leaf interface (ptin_intf) 
 * @param leaf_out_vlan : External outer vlan 
 * @param leaf_inner_vlan : External inner vlan  
 * @param int_vlan : Internal VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_mcevc_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing stacked leaf intf# %u [Int.VLAN=%u]...",
           leaf_intf, int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(leaf_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Add egress xlate entry: (leaf_intf) (Vr,Vc) => (Vs',Vc) */
  rc = ptin_xlate_egress_delete(intIfNum, int_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intf %u xlate Egress entry [Root Int.VLAN %u (rc=%d)",
            leaf_intf, int_vlan, leaf_out_vlan, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
#endif

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
static L7_RC_t switching_p2p_bridge_add(L7_uint root_intf, L7_uint16 root_int_vid, L7_uint leaf_intf, L7_uint16 leaf_out_vid, L7_uint16 leaf_inn_vid)
{
  L7_uint32 root_intIfNum;
  L7_uint32 leaf_intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Adding stacked bridge [Root: Intf=%u IntVID=%u] <=> [Leaf: Intf=%u NEW Out.VID=%u Inn.VID=%u] ...",
           root_intf, root_int_vid, leaf_intf, leaf_out_vid, leaf_inn_vid);

  /* Get intIfNum of root */
  rc = ptin_intf_port2intIfNum(root_intf, &root_intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", root_intf);
    return L7_FAILURE;
  }

  /* Get intIfNum of root */
  rc = ptin_intf_port2intIfNum(leaf_intf, &leaf_intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Add cross-connection between root and leaf interfaces */
  rc = ptin_crossconnect_add(root_int_vid, leaf_inn_vid, root_intIfNum, leaf_intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding Xconnect (OVID=%u, IVID=%u) (intIfNum1=%u, intIfNum2=%u) (rc=%d)",
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
static L7_RC_t switching_p2p_bridge_remove(L7_uint root_intf, L7_uint16 root_int_vid, L7_uint leaf_intf, L7_uint16 leaf_out_vid, L7_uint16 leaf_inn_vid)
{
  L7_uint32 root_intIfNum;
  L7_uint32 leaf_intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing stacked bridge [Root: Intf=%u IntVID=%u] <=> [Leaf: Intf=%u NEW Out.VID=%u Inn.VID=%u] ...",
           root_intf, root_int_vid, leaf_intf, leaf_out_vid, leaf_inn_vid);

  /* Get intIfNum of root */
  rc = ptin_intf_port2intIfNum(root_intf, &root_intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", root_intf);
    return L7_FAILURE;
  }

  /* Get intIfNum of root */
  rc = ptin_intf_port2intIfNum(leaf_intf, &leaf_intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Deletes cross-connection between root and leaf interfaces */
  rc = ptin_crossconnect_delete(root_int_vid, leaf_inn_vid);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding Xconnect (OVID=%u, IVID=%u) (intIfNum1=%u, intIfNum2=%u) (rc=%d)",
            root_int_vid, leaf_inn_vid, root_intIfNum, leaf_intIfNum, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
#endif /* !PTIN_BOARD_IS_DNX */

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

#if (PTIN_BOARD_IS_DNX)
  if (vid >= 4096)
  {
    rc = ptin_vsi_create(vid);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error creating VSI %u (rc=%d)", vid, rc);
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_EVC, "VSI %u created", vid);
  }
  else
#endif
  {
    rc = usmDbVlanCreate(1, vid); 
    if (rc != L7_SUCCESS)
    {
      PT_LOG_CRITIC(LOG_CTX_EVC, "Error creating VLAN %u (rc=%d)", vid, rc);
      return L7_FAILURE;
    }

    rc = usmDbVlanMemberSet(1, vid, L7_ALL_INTERFACES, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_CRITIC(LOG_CTX_EVC, "Error removing VLAN %u from all interfaces (rc=%d)", vid, rc);
      return L7_FAILURE;
    }

    rc = usmDbVlanTaggedSet(1, vid, L7_ALL_INTERFACES, L7_DOT1Q_TAGGED);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_CRITIC(LOG_CTX_EVC, "Error setting VLAN %u as tagged on all interfaces (rc=%d)", vid, rc);
      return L7_FAILURE;
    }

    /* Wait until all requests are attended */
    while (!dot1qQueueIsEmpty())
      osapiSleepMSec(10);

    PT_LOG_TRACE(LOG_CTX_EVC, "VLAN %u created", vid);
  }

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

#if (PTIN_BOARD_IS_DNX)
  if (vid >= 4096)
  {
    rc = ptin_vsi_destroy(vid);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error destroying VSI %u (rc=%d)", vid, rc);
      return L7_FAILURE;
    }
  }
  else
#endif
  {
    rc = usmDbVlanDelete(1, vid); 
    if (rc != L7_SUCCESS)
    {
      PT_LOG_CRITIC(LOG_CTX_EVC, "Error deleting VLAN %u (rc=%d)", vid, rc);
      //return L7_FAILURE;
    }

    /* Wait until all requests are attended */
    while (!dot1qQueueIsEmpty())
      osapiSleepMSec(10);
  }

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

  if (vid >= 1 && vid <= 4095)
  {
    /* Apply/remove cpu vlan */
    rc = ptin_vlan_cpu_set( vid, cpu_trap );
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "VLAN %u: error setting CPU trap state to %u (rc=%d)",vid, cpu_trap, rc);
      return L7_FAILURE;
    }

  #if (!PTIN_BOARD_IS_DNX)
    /* Set Forward VLAN to int_vlan and set Mac Learning state */
    rc = ptin_crossconnect_vlan_learn(vid, fwd_vid, -1, mac_learning);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "VLAN %u: error setting MAC Learning state to %s (w/ Forward VLAN %u) (rc=%d)",
              vid, mac_learning ? "Enabled":"Disabled", fwd_vid, rc);
      return L7_FAILURE;
    }
  #endif

    /* Set Multicast Flooding type */
    rc = usmdbFilterVlanFilteringModeSet(1, vid, mc_flood);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "VLAN %u: error setting Multicast Flood type to %s (rc=%d)",
              vid, mc_flood == L7_FILTER_FORWARD_ALL ? "FORWARD_ALL":mc_flood == L7_FILTER_FORWARD_UNREGISTERED ? "FORWARD_UNREGISTERED":"FILTER_UNREGISTERED", rc);
      return L7_FAILURE;
    }
    usmdbFilterVlanFilteringModeGet(1, vid, &mcf);

    PT_LOG_TRACE(LOG_CTX_EVC,"success with usmdbFilterVlanFilteringModeSet(1,%u,%u) => usmdbFilterVlanFilteringModeSet(1,%u,&mc_flood): mc_flood=%u",vid,mc_flood,vid,mcf);
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_EVC,"VID %u not in the range 1-4095", vid);
  }

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
  L7_uint32   i, evc_id;
  L7_int      port;
  ptin_intf_t ptin_intf;

  /* Arguments */
  if (evcConf==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Null pointer");
    return L7_FAILURE;
  }

  /* EVC index */
  if (evcConf->index >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid eEVC index (%u)",evcConf->index);
    return L7_FAILURE;
  }

#if !(PTIN_QUATTRO_FLOWS_FEATURE_ENABLED)
  if (evcConf->flags & PTIN_EVC_MASK_QUATTRO)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"eEVC %u: QUATTRO EVCs are not allowed for this board!",evcConf->index);
    return L7_FAILURE;
  }
#endif

  /* ARAD: Allow 0 interfaces */
#if 0
  /* Number of interfaces */
  if (evcConf->n_intf==0 || evcConf->n_intf>=PTIN_SYSTEM_N_PORTS_AND_LAGS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid number of interfaces (%u)",evcConf->n_intf);
    return L7_FAILURE;
  }
#endif

  /* Run all interfaces */
  for (i=0; i<evcConf->n_intf; i++)
  {
    /* Interface id and type */
    if (evcConf->intf[i].intf_type!=PTIN_EVC_INTF_PHYSICAL && evcConf->intf[i].intf_type!=PTIN_EVC_INTF_LOGICAL)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Interface index %u has an invalid type (%u)",i,evcConf->intf[i].intf_type);
      return L7_FAILURE;
    }
    if ( (evcConf->intf[i].intf_type==PTIN_EVC_INTF_PHYSICAL && evcConf->intf[i].intf_id >= ptin_sys_number_of_ports) ||
         (evcConf->intf[i].intf_type==PTIN_EVC_INTF_LOGICAL  && evcConf->intf[i].intf_id >= PTIN_SYSTEM_N_LAGS ) )
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Interface index %u has an invalid id (%u/%u)",i,evcConf->intf[i].intf_type,evcConf->intf[i].intf_id);
      return L7_FAILURE;
    }
    /* MEF type */
    if (evcConf->intf[i].mef_type!=PTIN_EVC_INTF_ROOT && evcConf->intf[i].mef_type!=PTIN_EVC_INTF_LEAF)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Interface index %u has an invalid mef type (%u/%u: %u)",i,evcConf->intf[i].intf_type,evcConf->intf[i].intf_id,evcConf->intf[i].mef_type);
      return L7_FAILURE;
    }

    /* Correct values */
    if (evcConf->intf[i].vid_inner >= 4096)
    {
      evcConf->intf[i].vid_inner = 0;
    }

    /* If interface is root, or any of the unstacked EVCs,
       check if the vlan is not being used by other EVCs in the same interface */
    if (evcConf->intf[i].mef_type==PTIN_EVC_INTF_ROOT || 
        (!(evcConf->flags & PTIN_EVC_MASK_STACKED) && !(evcConf->flags & PTIN_EVC_MASK_QUATTRO)))
    {
      /* Vlan */
      if (evcConf->intf[i].vid==0 || evcConf->intf[i].vid>=4095)
      {
        PT_LOG_ERR(LOG_CTX_EVC,"Interface index %u has an invalid vlan (%u/%u: %u)",i,evcConf->intf[i].intf_type,evcConf->intf[i].intf_id,evcConf->intf[i].vid);
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
          if (!evcs[evc_id].in_use || evcs[evc_id].extended_id == evcConf->index)  continue;
          /* Skip evcs which are not using this interface */
          if (!evcs[evc_id].intf[port].in_use)  continue;

          /* Skip leaf interfaces of stacked services */
          if (IS_EVC_INTF_LEAF(evc_id,port) && (IS_EVC_QUATTRO(evc_id) || IS_EVC_STACKED(evc_id)))
            continue;

          /* Skip MC EVCs when comparing the leaf port */
          /* I'm assuming IPTV service already exists */
          if ( ((evcConf->flags & PTIN_EVC_MASK_MC_IPTV) || (evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV))
                && (evcs[evc_id].intf[port].type == PTIN_EVC_INTF_LEAF) )
            continue;

          /* If outer vlan matches, we have a conflict */
          if (evcs[evc_id].intf[port].out_vlan   == evcConf->intf[i].vid &&
              evcs[evc_id].intf[port].inner_vlan == evcConf->intf[i].vid_inner)
          {
            PT_LOG_ERR(LOG_CTX_EVC,"Interface index %u, port=%u (%u/%u) of EVC %u (eEVC %u) has the same vlan %u+%u",
                    i, port, ptin_intf.intf_type, ptin_intf.intf_id, evc_id, evcs[evc_id].extended_id, evcConf->intf[i].vid, evcConf->intf[i].vid_inner);
            return L7_FAILURE;
          }
        }
      }
    }
  }

  return L7_SUCCESS;
}

#define BWPROFILE_INDX(cos) ((cos)>=L7_COS_INTF_QUEUE_MAX_COUNT?0:(cos))

/**
 * Verify bandwidth profile parameters
 * 
 * @param evc_id : evc index
 * @param profile : profile data 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_evc_bwProfile_verify(L7_uint evc_id, ptin_bw_profile_t *profile)
{
  L7_int    ptin_port, i_port;
  struct ptin_evc_client_s *pclientFlow;

  PT_LOG_TRACE(LOG_CTX_EVC,"Starting bw profile verification");

  /* Profile pointer should have a valid address */
  if (profile==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Profile is a null pointer");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_EVC,"Initial bw profile data:");
  PT_LOG_TRACE(LOG_CTX_EVC," evcId       = %u",evc_id);
  PT_LOG_TRACE(LOG_CTX_EVC," ptin_port   = %d",profile->ptin_port);
  PT_LOG_TRACE(LOG_CTX_EVC," OVID_in     = %u",profile->outer_vlan_lookup);
  PT_LOG_TRACE(LOG_CTX_EVC," OVID_int    = %u",profile->outer_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_EVC," OVID_out    = %u",profile->outer_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_EVC," IVID_in     = %u",profile->inner_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_EVC," IVID_out    = %u",profile->inner_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_EVC," COS         = %u",profile->cos);
  PT_LOG_TRACE(LOG_CTX_EVC," MAC         = %02x:%02x:%02x:%02x:%02x:%02x",profile->macAddr[0],profile->macAddr[1],profile->macAddr[2],profile->macAddr[3],profile->macAddr[4],profile->macAddr[5]);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u is out of range [0..%u]", evc_id, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u is not in use", evc_id);
    return L7_NOT_EXIST;
  }

  ptin_port = profile->ptin_port;

  /* If source interface is provided, validate it */
  if (ptin_port >= 0 && ptin_port < PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_TRACE(LOG_CTX_EVC,"Processing source interface: ptin_port=%u", ptin_port);

    /* Verify if interface is in use */
    if (!evcs[evc_id].intf[ptin_port].in_use)
    {
      PT_LOG_WARN(LOG_CTX_EVC,"ptin_port %d is not in use",ptin_port);
      return L7_NOT_EXIST;
    }
    PT_LOG_TRACE(LOG_CTX_EVC,"Source interface is present in EVC");

    /* Verify Svlan*/
    if (profile->outer_vlan_lookup>0 &&
        evcs[evc_id].intf[ptin_port].out_vlan>0 && evcs[evc_id].intf[ptin_port].out_vlan<4096)
    {
      if (profile->outer_vlan_lookup!=evcs[evc_id].intf[ptin_port].out_vlan)
      {
        PT_LOG_ERR(LOG_CTX_EVC,"OVid_in %u does not match to the one in EVC (%u)",profile->outer_vlan_lookup,evcs[evc_id].intf[ptin_port].out_vlan);
        return L7_FAILURE;
      }
      PT_LOG_TRACE(LOG_CTX_EVC,"Source interface (ptin_port=%u): OVid_in %u verified",ptin_port,profile->outer_vlan_lookup);
    }

    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    /* For MAC-Bridge flows */
    if (IS_EVC_QUATTRO(evc_id) && IS_EVC_INTF_LEAF(evc_id, ptin_port) &&
        profile->outer_vlan_egress != 0)
    {
      PT_LOG_TRACE(LOG_CTX_EVC,"Outer vlan id = %u", profile->outer_vlan_egress);

      /* profile->outer_vlan_out is the GEM id related to the flow */
      ptin_evc_find_flow(profile->outer_vlan_egress, &(evcs[evc_id].intf[ptin_port].clients), (dl_queue_elem_t **)&pclientFlow);

      /* Client not found */
      if (pclientFlow==L7_NULLPTR)
      {
        PT_LOG_WARN(LOG_CTX_EVC,"Client %u not found in EVC %u",profile->inner_vlan_ingress,evc_id);
        return L7_NOT_EXIST;
      }
      /* Using egressing outer_vlan with a valid value, will force the selection of EFP module. Otherwise, it will use the IFP */
      /* For QUATTRO services: these VLANs should be initialized */
      profile->outer_vlan_egress = pclientFlow->uni_ovid;  /* Redundant: flow search guarantees they are equal */
      profile->inner_vlan_egress = 0;                      /* There is no inner vlan, after packet leaves the port (leaf port in a stacked service) */

      PT_LOG_TRACE(LOG_CTX_EVC,"Outer vlan id = %u (uni_ovid=%u)", profile->outer_vlan_egress, pclientFlow->uni_ovid);
    }
    else
    #endif
    /* Find the specified client, and provide the policer location */
    if (profile->inner_vlan_ingress != 0)
    {
      /* Find the specified cvlan in all EVC clients */
      for (i_port=0, pclientFlow=L7_NULLPTR; i_port<PTIN_SYSTEM_N_INTERF && pclientFlow==L7_NULLPTR; i_port++)
      {
        if ( IS_EVC_INTF_ROOT(evc_id,ptin_port) ||
            (IS_EVC_INTF_LEAF(evc_id,ptin_port) && i_port==ptin_port))
        {
          ptin_evc_find_client(profile->inner_vlan_ingress, &(evcs[evc_id].intf[i_port].clients), (dl_queue_elem_t **) &pclientFlow);
        }
      }
      /* Client not found */
      if (pclientFlow==L7_NULLPTR)
      {
        PT_LOG_WARN(LOG_CTX_EVC,"Client %u not found in EVC %u",profile->inner_vlan_ingress,evc_id);
        return L7_NOT_EXIST;
      }
      /* If interface is a leaf... */
      if (IS_EVC_INTF_LEAF(evc_id,ptin_port))
      {
        /* Compare its outer vlan with the given one */
        if (profile->outer_vlan_lookup>0 &&
            pclientFlow->uni_ovid>0 && pclientFlow->uni_ovid<4096)
        {
          if (profile->outer_vlan_lookup!=pclientFlow->uni_ovid)
          {
            PT_LOG_ERR(LOG_CTX_EVC,"OVid_in %u does not match to the one in EVC client (%u)", profile->outer_vlan_lookup, pclientFlow->uni_ovid);
            return L7_FAILURE;
          }
          PT_LOG_TRACE(LOG_CTX_EVC,"OVid_in %u verified for client %u",ptin_port,profile->outer_vlan_lookup,profile->inner_vlan_ingress);
        }
        /* Removed: for non QUATTRO services, these vlans should be null */
        //profile->outer_vlan_out = pclientFlow->uni_ovid;
        //profile->inner_vlan_out = 0;                /* There is no inner vlan, after packet leaves the port (leaf port in a stacked service) */
      }

      /* Using egressing outer_vlan with a valid value, will force the selection of EFP module. Otherwise, it will use the IFP */
      profile->outer_vlan_egress = 0;
      profile->inner_vlan_egress = 0;
    }
    else
    {
      /* Using egressing outer_vlan with a valid value, will force the selection of EFP module. Otherwise, it will use the IFP */
      profile->outer_vlan_egress = 0;
      profile->inner_vlan_egress = 0;
    }

    /* If svlan is provided, it was already validated... Rewrite it with the internal value */
    profile->outer_vlan_lookup  = 0;
    profile->outer_vlan_ingress = evcs[evc_id].intf[ptin_port].int_vlan;
    PT_LOG_TRACE(LOG_CTX_EVC,"Interface (ptin_port=%u): OVid_in  = %u",ptin_port,profile->outer_vlan_ingress);
  } /* if (profile->ddUsp_src.unit>=0 && profile->ddUsp_src.slot>=0 && profile->ddUsp_src.port>=0) */
  else
  {
    PT_LOG_ERR(LOG_CTX_EVC,"No Interface speficied");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_EVC,"Final bw profile data:");
  PT_LOG_TRACE(LOG_CTX_EVC," evcId       = %u",evc_id);
  PT_LOG_TRACE(LOG_CTX_EVC," ptin_port   = %u",profile->ptin_port);
  PT_LOG_TRACE(LOG_CTX_EVC," OVID_in     = %u",profile->outer_vlan_lookup);
  PT_LOG_TRACE(LOG_CTX_EVC," OVID_int    = %u",profile->outer_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_EVC," OVID_out    = %u",profile->outer_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_EVC," IVID_in     = %u",profile->inner_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_EVC," IVID_out    = %u",profile->inner_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_EVC," COS         = %u",profile->cos);
  PT_LOG_TRACE(LOG_CTX_EVC," MAC         = %02x:%02x:%02x:%02x:%02x:%02x",profile->macAddr[0],profile->macAddr[1],profile->macAddr[2],profile->macAddr[3],profile->macAddr[4],profile->macAddr[5]);

  return L7_SUCCESS;
}

/**
 * Verify evcStats profile parameters
 * 
 * @param evc_id : evc index
 * @param profile : evcStats profile data 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_evc_evcStats_verify(L7_uint evc_id, ptin_evcStats_profile_t *profile)
{
  L7_int    ptin_port, i_port;
  struct ptin_evc_client_s *pclientFlow;

  PT_LOG_TRACE(LOG_CTX_EVC,"Starting evcStats verification");

  /* Profile pointer should have a valid address */
  if (profile==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Profile is a null pointer");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_EVC,"Initial evcStats profile data:");
  PT_LOG_TRACE(LOG_CTX_EVC," evcId     = %u",evc_id);
  PT_LOG_TRACE(LOG_CTX_EVC," ptin_port = %d",profile->ptin_port);
  PT_LOG_TRACE(LOG_CTX_EVC," OVID_in   = %u",profile->outer_vlan_lookup);
  PT_LOG_TRACE(LOG_CTX_EVC," OVID_int  = %u",profile->outer_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_EVC," OVID_out  = %u",profile->outer_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_EVC," IVID_in   = %u",profile->inner_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_EVC," IVID_out  = %u",profile->inner_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_EVC," Dest_IP   = %u",profile->dst_ip);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u is out of range [0..%u]", evc_id, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u is not in use", evc_id);
    return L7_NOT_EXIST;
  }

  ptin_port = profile->ptin_port;

  /* If interface is provided, validate it */
  if (ptin_port >= 0 && ptin_port < PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_TRACE(LOG_CTX_EVC,"Processing interface: ptin_port=%u",ptin_port);

    /* Verify if interface is in use */
    if (!evcs[evc_id].intf[ptin_port].in_use)
    {
      PT_LOG_WARN(LOG_CTX_EVC,"ptin_port %d is not in use",ptin_port);
      return L7_NOT_EXIST;
    }
    PT_LOG_TRACE(LOG_CTX_EVC,"Interface is present in EVC");

    /* Verify Svlan*/
    if (profile->outer_vlan_lookup>0 &&
        evcs[evc_id].intf[ptin_port].out_vlan>0 && evcs[evc_id].intf[ptin_port].out_vlan<4096)
    {
      if (profile->outer_vlan_lookup!=evcs[evc_id].intf[ptin_port].out_vlan)
      {
        PT_LOG_ERR(LOG_CTX_EVC,"OVid_in %u does not match to the one in EVC (%u)",profile->outer_vlan_lookup,evcs[evc_id].intf[ptin_port].out_vlan);
        return L7_FAILURE;
      }
      PT_LOG_TRACE(LOG_CTX_EVC,"Interface (ptin_port=%u): OVid_in %u verified",ptin_port,profile->outer_vlan_lookup);
    }

    /* Default outer and inner vlan at egress:
       - the outer vlan is the defined for each interface
       - the inner vlan is the same as the internal inner vlan
       If interface is leaf and EVC is stacked, the outer and inner vlans at egress should not be considered */
    #if ( !PTIN_BOARD_IS_MATRIX )
    profile->outer_vlan_egress = 0;
    profile->inner_vlan_egress = 0;
    if (IS_EVC_INTF_ROOT(evc_id,ptin_port) || IS_EVC_STD_P2MP(evc_id))
    #endif
    {
      profile->outer_vlan_egress = evcs[evc_id].intf[ptin_port].out_vlan;
      profile->inner_vlan_egress = profile->inner_vlan_ingress;
    }

    /* If valid, find the specified client, and provide the policer location */
    if (profile->inner_vlan_ingress != 0)
    {
      /* Find the specified cvlan in all EVC clients */
      for (i_port=0, pclientFlow=L7_NULLPTR; i_port<PTIN_SYSTEM_N_INTERF && pclientFlow==L7_NULLPTR; i_port++)
      {
        if ( IS_EVC_INTF_ROOT(evc_id,ptin_port) ||
            (IS_EVC_INTF_LEAF(evc_id,ptin_port) && i_port==ptin_port))
        {
          ptin_evc_find_client(profile->inner_vlan_ingress, &(evcs[evc_id].intf[i_port].clients), (dl_queue_elem_t **) &pclientFlow);
        }
      }
      if (pclientFlow==L7_NULLPTR)
      {
        PT_LOG_WARN(LOG_CTX_EVC,"Client %u not found in EVC %u",profile->inner_vlan_ingress,evc_id);
        return L7_NOT_EXIST;
      }
      /* If interface is a leaf... */
      if (IS_EVC_INTF_LEAF(evc_id,ptin_port))
      {
        /* Compare its outer vlan with the given one */
        if (profile->outer_vlan_lookup>0 &&
            pclientFlow->uni_ovid>0 && pclientFlow->uni_ovid<4096)
        {
          if (profile->outer_vlan_lookup!=pclientFlow->uni_ovid)
          {
            PT_LOG_ERR(LOG_CTX_EVC,"OVid_in %u does not match to the one in EVC client (%u)",profile->outer_vlan_lookup,pclientFlow->uni_ovid);
            return L7_FAILURE;
          }
          PT_LOG_TRACE(LOG_CTX_EVC,"OVid_in %u verified for client %u",ptin_port,profile->outer_vlan_lookup,profile->inner_vlan_ingress);
        }
        profile->outer_vlan_egress = pclientFlow->uni_ovid;
        profile->inner_vlan_egress = 0;                /* No need to consider inner vlan at the egress */
      }
    } /* else (profile->inner_vlan_in==0) */

    /* If svlan is provided, it was already validated... Use internal value, instead of original one */
    profile->outer_vlan_lookup  = 0;
    profile->outer_vlan_ingress = evcs[evc_id].intf[ptin_port].int_vlan;
    PT_LOG_TRACE(LOG_CTX_EVC,"Interface (ptin_port=%u): OVid_in=%u, OVid_int=%u",ptin_port,profile->outer_vlan_lookup,profile->outer_vlan_ingress);

  } /* if (profile->ddUsp_src.unit>=0 && profile->ddUsp_src.slot>=0 && profile->ddUsp_src.port>=0) */
  /* If interface is not provided... */
  else
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Interface is not speficied");
    return L7_FAILURE;
  } /* else (profile->ddUsp_src.unit>=0 && profile->ddUsp_src.slot>=0 && profile->ddUsp_src.port>=0) */

  PT_LOG_TRACE(LOG_CTX_EVC,"Final evcStats profile data:");
  PT_LOG_TRACE(LOG_CTX_EVC," evcId     = %u",evc_id);
  PT_LOG_TRACE(LOG_CTX_EVC," ptin_port = %d",profile->ptin_port);
  PT_LOG_TRACE(LOG_CTX_EVC," OVID_in   = %u",profile->outer_vlan_lookup);
  PT_LOG_TRACE(LOG_CTX_EVC," OVID_int  = %u",profile->outer_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_EVC," OVID_out  = %u",profile->outer_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_EVC," IVID_in   = %u",profile->inner_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_EVC," IVID_out  = %u",profile->inner_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_EVC," Dst_IP    = %u",profile->dst_ip);

  return L7_SUCCESS;
}

/**
 * Read statistics of a particular probe
 * 
 * @param evc_id : EVC index
 * @param profile : evcStats profile 
 * @param stats : Statistics data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
static L7_RC_t ptin_evc_probe_get(L7_uint evc_id, ptin_evcStats_profile_t *profile, ptin_evcStats_counters_t *stats)
{
  L7_int    ptin_port;
  L7_RC_t   rc = L7_SUCCESS;

  /* Validate arguments */
  if (profile==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Null pointer");
    return L7_FAILURE;
  }

  /* Nothing to do if ip is not valid */
  if (profile->dst_ip==0 || profile->dst_ip==(L7_uint32)-1)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid DIP address");
    return L7_FAILURE;
  }

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u is out of range [0..%u]", evc_id, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u is not in use", evc_id);
    return L7_NOT_EXIST;
  }

  ptin_port = profile->ptin_port;

  /* If interface is provided, validate it */
  if (ptin_port >= 0 && ptin_port < PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_TRACE(LOG_CTX_EVC,"Processing interface: ptin_port=%u",ptin_port);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid ptin_port %u",ptin_port);
    return L7_FAILURE;
  }

  /* Verify if interface is in use */
  if (!evcs[evc_id].intf[ptin_port].in_use)
  {
    PT_LOG_WARN(LOG_CTX_EVC,"ptin_port %d is not in use",ptin_port);
    return L7_NOT_EXIST;
  }
  PT_LOG_TRACE(LOG_CTX_EVC,"Interface is present in EVC");

  /* Use internal VLAN */
  profile->outer_vlan_lookup  = 0;
  profile->outer_vlan_ingress = evcs[evc_id].intf[ptin_port].int_vlan;

  /* Read policy information */
  if ((rc = ptin_evcStats_get(stats, profile))!=L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_EVC,"Error reading probe policer");
    return rc;
  }

  /* Success */
  return L7_SUCCESS;
}

/**
 * Get a new probe node to store counter
 * 
 * @param evc_id : EVC index
 * @param profile : Stats profile
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_evc_probe_add(L7_uint evc_id, ptin_evcStats_profile_t *profile)
{
  L7_int    ptin_port;
  L7_RC_t   rc = L7_SUCCESS;

  /* Validate arguments */
  if (profile==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Null pointer");
    return L7_FAILURE;
  }

  /* Nothing to do if ip is not valid */
  if (profile->dst_ip==0 || profile->dst_ip==(L7_uint32)-1)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid DIP address");
    return L7_FAILURE;
  }

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u is out of range [0..%u]", evc_id, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u is not in use", evc_id);
    return L7_NOT_EXIST;
  }

  ptin_port = profile->ptin_port;

  /* If interface is provided, validate it */
  if (ptin_port >= 0 && ptin_port < PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_TRACE(LOG_CTX_EVC,"Processing Interface: ptin_port=%u",ptin_port);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid ptin_port %u",ptin_port);
    return L7_FAILURE;
  }

  /* Verify if interface is in use */
  if (!evcs[evc_id].intf[ptin_port].in_use)
  {
    PT_LOG_WARN(LOG_CTX_EVC,"ptin_port %d is not in use",ptin_port);
    return L7_NOT_EXIST;
  }
  PT_LOG_TRACE(LOG_CTX_EVC,"Interface is present in EVC");

  /* We should have an outer vlan for this interface */
  if (evcs[evc_id].intf[ptin_port].out_vlan==0 ||
      evcs[evc_id].intf[ptin_port].out_vlan>=4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC %u is not unstacked!",evc_id);
    return L7_FAILURE;
  }

#if 0
  /* We should have an outer vlan for this interface */
  if (evcs[evc_id].intf[ptin_port].int_vlan==0 ||
      evcs[evc_id].intf[ptin_port].int_vlan >= 4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC %u: Internal VLAN %u is not valid!",evc_id,evcs[evc_id].intf[ptin_port].int_vlan);
    return L7_FAILURE;
  }
#endif

  /* Use internal VLAN */
  profile->outer_vlan_lookup  = 0;
  profile->outer_vlan_ingress = evcs[evc_id].intf[ptin_port].int_vlan;

  /* Apply policy */
  if ((rc = ptin_evcStats_set(profile))!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error adding probing evcStats");
  }

  PT_LOG_TRACE(LOG_CTX_EVC,"Allocated probe");

  return rc;
}

/**
 * Get a new probe node to store counter
 * 
 * @param evc_id : EVC index
 * @param profile : Stats profile
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_evc_probe_delete(L7_uint evc_id, ptin_evcStats_profile_t *profile)
{
  L7_int    ptin_port;
  L7_RC_t   rc = L7_SUCCESS;

  /* Validate arguments */
  if (profile==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Null pointer");
    return L7_FAILURE;
  }

  /* Nothing to do if ip is not valid */
  if (profile->dst_ip==0 || profile->dst_ip==(L7_uint32)-1)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid DIP address");
    return L7_FAILURE;
  }

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u is out of range [0..%u]", evc_id, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u is not in use", evc_id);
    return L7_NOT_EXIST;
  }

  ptin_port = profile->ptin_port;

  /* If interface is provided, validate it */
  if (ptin_port >= 0 && ptin_port < PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_TRACE(LOG_CTX_EVC,"Processing interface: ptin_port=%u", ptin_port);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }

  /* Verify if interface is in use */
  if (!evcs[evc_id].intf[ptin_port].in_use)
  {
    PT_LOG_WARN(LOG_CTX_EVC,"ptin_port %d is not in use",ptin_port);
    return L7_NOT_EXIST;
  }
  PT_LOG_TRACE(LOG_CTX_EVC,"Interface is present in EVC");

  /* Use internal VLAN */
  profile->outer_vlan_lookup  = 0;
  profile->outer_vlan_ingress = evcs[evc_id].intf[ptin_port].int_vlan;

  /* Apply policy */
  rc = ptin_evcStats_delete(profile);

  if ( rc != L7_SUCCESS )
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error removing probing evcStats");
    return rc;
  }

  return L7_SUCCESS;
}

/* DEBUG Functions ************************************************************/

/**
 * Dumps EVC detailed info 
 * If evc_id is invalid, all EVCs are dumped 
 * 
 * @param evc_id 
 */
void ptin_evc_dump(L7_uint32 evc_ext_id)
{
  L7_uint ext_id, i, j;
  struct ptin_evc_client_s *pclientFlow;
  ptinExtEvcIdDataKey_t   extEvcIdDataKey;
  ptinExtEvcIdInfoData_t *extEvcIdInfoData;
  L7_uint32 evc_id;

  /* Read all evcs */
  if (evc_ext_id == (L7_uint32)-1)
  {
    /* Initial key */
    memset(&extEvcIdDataKey, 0x00, sizeof(ptinExtEvcIdDataKey_t));

    extEvcIdInfoData = (ptinExtEvcIdInfoData_t *) avlSearchLVL7(&(extEvcId_avlTree.extEvcIdAvlTree), (void *)&extEvcIdDataKey, AVL_EXACT);

    /* If not found, search for the next one */
    if (extEvcIdInfoData == L7_NULLPTR)
    {
      extEvcIdInfoData = (ptinExtEvcIdInfoData_t *) avlSearchLVL7(&(extEvcId_avlTree.extEvcIdAvlTree), (void *)&extEvcIdDataKey, AVL_NEXT);

      /* If not found, table is empty */
      if (extEvcIdInfoData == L7_NULLPTR)
      {
        printf("No EVCs configured!\r\n");
        return;
      }
    }
  }
  /* Read only one EVC */
  else
  {
    /* Initial key */
    memset(&extEvcIdDataKey, 0x00, sizeof(ptinExtEvcIdDataKey_t));
    extEvcIdDataKey.ext_evcId = evc_ext_id;

    extEvcIdInfoData = (ptinExtEvcIdInfoData_t *) avlSearchLVL7(&(extEvcId_avlTree.extEvcIdAvlTree), (void *)&extEvcIdDataKey, AVL_EXACT);

    if (extEvcIdInfoData == L7_NULLPTR)
    {
      printf("EVC %u does not exist!\r\n", evc_ext_id);
      return;
    }
  }

  /* Run all AVL nodes */
  do
  {
    /* Prepare next key */
    memcpy(&extEvcIdDataKey, &extEvcIdInfoData->extEvcIdDataKey, sizeof(ptinExtEvcIdDataKey_t));

    ext_id  = extEvcIdInfoData->extEvcIdDataKey.ext_evcId;
    evc_id  = extEvcIdInfoData->evc_id;

    /* Validate EVC */
    if (evc_id >= PTIN_SYSTEM_N_EVCS)
    {
      printf("Strange... Internal EVC id %u is a too high value...\r\n", evc_id);
      continue;
    }
    if (!evcs[evc_id].in_use)
    {
      printf("Strange... eEVC %u is not in use, but is present in AVL tree...\r\n", ext_id);
      continue;
    }

    printf("eEVC# %02u (internal id %u)\n", ext_id, evc_id);

    printf("  Flags     = 0x%08X   ", evcs[evc_id].flags);

    if (evcs[evc_id].flags & PTIN_EVC_MASK_BUNDLING)
      printf("BUNDLING  ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_ALL2ONE)
      printf("ALL2ONE  ");

    if ((evcs[evc_id].flags & PTIN_EVC_MASK_QUATTRO) == PTIN_EVC_MASK_QUATTRO)
      printf("QUATTRO-");
    if ((evcs[evc_id].flags & PTIN_EVC_MASK_P2P) == PTIN_EVC_MASK_P2P)
      printf("P2P     ");
    else
      printf("P2MP    ");

    if (evcs[evc_id].flags & PTIN_EVC_MASK_STACKED)
      printf("STACKED    ");
    else
      printf("UNSTACKED  ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_MACLEARNING)
      printf("MACLEARNING  ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV)
      printf("MC-IPTV  ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_CPU_TRAPPING)
      printf("CPUTrap  ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_IGMP_PROTOCOL)
      printf("IGMP  ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_DHCPV4_PROTOCOL)
      printf("DHCPop82  ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_PPPOE_PROTOCOL)
      printf("PPPoE  ");
    printf("\n");

    printf("  MC Flood  = %s (%u)\n", evcs[evc_id].mc_flood == PTIN_EVC_MC_FLOOD_ALL ? "All":evcs[evc_id].mc_flood == PTIN_EVC_MC_FLOOD_UNKNOWN ? "Unknown":"None",
           evcs[evc_id].mc_flood);
              
    printf("  Roots     = %-2u        Counters = %u\t\tProbes=%u\n", evcs[evc_id].n_roots, evcs[evc_id].n_counters, evcs[evc_id].n_probes);
    printf("  Leafs     = %-2u        BW Prof. = %u\n", evcs[evc_id].n_leafs, evcs[evc_id].n_bwprofiles);

    printf("  Root port1= %-2u\n", evcs[evc_id].root_info.port);
    printf("  Root VLAN = %-4u      NNI VLAN = %u+%u\n", evcs[evc_id].rvlan, evcs[evc_id].root_info.nni_ovid, evcs[evc_id].root_info.nni_ivid);
    printf("  MC Group  = 0x%x\n",   evcs[evc_id].multicast_group);

    /* Only stacked services have clients */
    if (IS_EVC_STACKED(evc_id))
      printf("  ClientFlows = %u\n", evcs[evc_id].n_clientflows);

    for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
    {
      if (!evcs[evc_id].intf[i].in_use)
        continue;

      if (i<PTIN_SYSTEM_N_PORTS)
        printf("  PHY# %02u\n", i);
      else
        printf("  LAG# %02u\n", i - PTIN_SYSTEM_N_PORTS);

      printf("    MEF Type      = %s          ", evcs[evc_id].intf[i].type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf");
      if (IS_EVC_IPTV(evc_id) && evcs[evc_id].intf[i].type == PTIN_EVC_INTF_LEAF)
        printf(" l3IntfId = %u  ", evcs[evc_id].intf[i].l3_intf_id);
      printf("\r\n");
      printf("    Ext. VLAN     = %-5u+%-5u   Counter  = %s\n", evcs[evc_id].intf[i].out_vlan, evcs[evc_id].intf[i].inner_vlan, evcs[evc_id].intf[i].counter != NULL ? "Active":"Disabled");
      printf("    Internal VLAN = %-5u         BW Prof. = %s\n", evcs[evc_id].intf[i].int_vlan, evcs[evc_id].intf[i].bwprofile[0] != NULL ? "Active":"Disabled");
      #ifdef PTIN_ERPS_EVC
      printf("    Port State    = %s\n", evcs[evc_id].intf[i].portState == PTIN_EVC_PORT_BLOCKING ? "Blocking":"Forwarding");
      #endif

      /* Only stacked services have clients */
    #if (!PTIN_BOARD_IS_DNX)
      if (IS_EVC_QUATTRO(evc_id) || IS_EVC_STACKED(evc_id))
    #endif
      {
        printf("    Clients       = %u\n", evcs[evc_id].intf[i].clients.n_elems);

        /* SEM CLIENTS UP */
        osapiSemaTake(ptin_evc_clients_sem, L7_WAIT_FOREVER);

        pclientFlow = L7_NULLPTR;
        dl_queue_get_head(&evcs[evc_id].intf[i].clients, (dl_queue_elem_t **) &pclientFlow);

        for (j=0; j < evcs[evc_id].intf[i].clients.n_elems && pclientFlow != L7_NULLPTR; j++) {
        #if (PTIN_BOARD_IS_DNX)
          printf("      Flow# %-2u:  flags=0x%08x  vid=%4u+%-4u  (gport=0x%08x)\r\n",
                 j, pclientFlow->flags, pclientFlow->uni_ovid, pclientFlow->uni_ivid, pclientFlow->virtual_gport);
        #else
          #if (PTIN_QUATTRO_FLOWS_FEATURE_ENABLED)
          if (IS_EVC_QUATTRO(evc_id))
          {
            printf("      Flow# %-2u: flags=0x%04x int_vid=%4u+%-4u<->uni_vid=%4u+%-4u (gport=0x%04x)\r\n",
                   j, pclientFlow->flags & 0xffff,
                   pclientFlow->int_ovid, pclientFlow->int_ivid, pclientFlow->uni_ovid, pclientFlow->uni_ivid, pclientFlow->virtual_gport & 0xffff);
          }
          else
          #endif
          {
            printf("      Client# %2u: VID=%4u+%-4u\r\n", j, pclientFlow->uni_ovid, pclientFlow->int_ivid);
          }
        #endif

          pclientFlow = (struct ptin_evc_client_s *) dl_queue_get_next(&evcs[evc_id].intf[i].clients, (dl_queue_elem_t *) pclientFlow);
        }

        /* SEM CLIENTS DOWN */
        osapiSemaGive(ptin_evc_clients_sem);
      }
    }
    printf("  Instances:");
    printf("\n   IGMP = ");
    (extEvcIdInfoData->igmp_inst==(L7_uint8)-1) ? printf("---") : printf("%-3u",extEvcIdInfoData->igmp_inst);
    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    if (IS_EVC_QUATTRO(evc_id))
      printf("   #Flows  = %u", evcs[evc_id].n_clientflows_igmp);
    #endif
    printf("\n   DHCP = "); 
    (extEvcIdInfoData->dhcp_inst==(L7_uint8)-1) ? printf("---") : printf("%-3u",extEvcIdInfoData->dhcp_inst);
    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    if (IS_EVC_QUATTRO(evc_id))
      printf("   #FlowsV4= %-4u #FlowsV6= %-4u", evcs[evc_id].n_clientflows_dhcpv4, evcs[evc_id].n_clientflows_dhcpv6);
    #endif
    printf("\n   PPPoE= ");
    (extEvcIdInfoData->pppoe_inst==(L7_uint8)-1) ? printf("---") : printf("%-3u",extEvcIdInfoData->pppoe_inst);
    #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    if (IS_EVC_QUATTRO(evc_id))
      printf("   #Flows  = %u", evcs[evc_id].n_clientflows_pppoe);
    #endif
    printf("\r\n\n");
    fflush(stdout);
  }
  while ( evc_ext_id == (L7_uint32)-1 &&
         (extEvcIdInfoData = (ptinExtEvcIdInfoData_t *) avlSearchLVL7(&(extEvcId_avlTree.extEvcIdAvlTree), (void *)&extEvcIdDataKey, AVL_NEXT)));

  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  printf("Total number of QUATTRO evcs: %u\r\n", n_quattro_evcs);
  printf("Total number of QUATTRO evcs with IGMP active: %u\r\n", n_quattro_igmp_evcs);
  #endif

  fflush(stdout);
}

/**
 * Dumps EVC detailed info 
 * If evc_id is invalid, all EVCs are dumped 
 * 
 * @param evc_id 
 */
void ptin_evc_dump2(L7_int evc_id_ref)
{
  L7_uint i, j;
  struct ptin_evc_client_s *pclientFlow;
  L7_uint32 evc_id;

  /* Run all AVL nodes */
  for (evc_id = 0; evc_id < PTIN_SYSTEM_N_EVCS; evc_id++)
  {
    /* Skip non wantable EVCs */
    if (evc_id_ref >= 0 && evc_id_ref != evc_id)
      continue;

    /* Skip not active EVCs */
    if (!evcs[evc_id].in_use)
      continue;

    printf("eEVC# %02u (internal id %u)\n", evcs[evc_id].extended_id, evc_id);

    printf("  Flags     = 0x%08X", evcs[evc_id].flags);
    if (evcs[evc_id].flags)
      printf("   ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_BUNDLING)
      printf("BUNDLING  ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_ALL2ONE)
      printf("ALL2ONE  ");

    if ((evcs[evc_id].flags & PTIN_EVC_MASK_QUATTRO) == PTIN_EVC_MASK_QUATTRO)
      printf("QUATTRO-"); 
    if ((evcs[evc_id].flags & PTIN_EVC_MASK_P2P) == PTIN_EVC_MASK_P2P)
      printf("P2P     ");
    else
      printf("P2MP    ");

    if (evcs[evc_id].flags & PTIN_EVC_MASK_STACKED)
      printf("STACKED    ");
    else
      printf("UNSTACKED  ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_MACLEARNING)
      printf("MACLEARNING  ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV)
      printf("MC-IPTV  ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_CPU_TRAPPING)
      printf("CPUTrap  ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_IGMP_PROTOCOL)
      printf("IGMP  ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_DHCPV4_PROTOCOL)
      printf("DHCPop82  ");
    if (evcs[evc_id].flags & PTIN_EVC_MASK_PPPOE_PROTOCOL)
      printf("PPPoE  ");
    printf("\n");

    printf("  MC Flood  = %s (%u)\n", evcs[evc_id].mc_flood == PTIN_EVC_MC_FLOOD_ALL ? "All":evcs[evc_id].mc_flood == PTIN_EVC_MC_FLOOD_UNKNOWN ? "Unknown":"None",
           evcs[evc_id].mc_flood);
              
    printf("  Roots     = %-2u        Counters = %u\t\tProbes=%u\n", evcs[evc_id].n_roots, evcs[evc_id].n_counters, evcs[evc_id].n_probes);
    printf("  Leafs     = %-2u        BW Prof. = %u\n", evcs[evc_id].n_leafs, evcs[evc_id].n_bwprofiles);

    printf("  Root port1= %-2u\n", evcs[evc_id].root_info.port);
    printf("  Root VLAN = %-4u      NNI VLAN = %u+%u\n", evcs[evc_id].rvlan, evcs[evc_id].root_info.nni_ovid, evcs[evc_id].root_info.nni_ivid);
    printf("  MC Group  = 0x%x\n",   evcs[evc_id].multicast_group);

    /* Only stacked services have clients */
    if (IS_EVC_STACKED(evc_id))
      printf("  ClientFlows = %u\n", evcs[evc_id].n_clientflows);

    for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
    {
      if (!evcs[evc_id].intf[i].in_use)
        continue;

      if (i<PTIN_SYSTEM_N_PORTS)
        printf("  PHY# %02u\n", i);
      else
        printf("  LAG# %02u\n", i - PTIN_SYSTEM_N_PORTS);

      printf("    MEF Type      = %s          ", evcs[evc_id].intf[i].type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf");
      if (IS_EVC_IPTV(evc_id) && evcs[evc_id].intf[i].type == PTIN_EVC_INTF_LEAF)
        printf(" l3IntfId = %u  ", evcs[evc_id].intf[i].l3_intf_id);
      printf("\r\n");
      printf("    Ext. VLAN     = %-5u+%-5u   Counter  = %s\n", evcs[evc_id].intf[i].out_vlan, evcs[evc_id].intf[i].inner_vlan, evcs[evc_id].intf[i].counter != NULL ? "Active":"Disabled");
      printf("    Internal VLAN = %-5u         BW Prof. = %s\n", evcs[evc_id].intf[i].int_vlan, evcs[evc_id].intf[i].bwprofile[0] != NULL ? "Active":"Disabled");
      #ifdef PTIN_ERPS_EVC
      printf("    Port State    = %s\n", evcs[evc_id].intf[i].portState == PTIN_EVC_PORT_BLOCKING ? "Blocking":"Forwarding");
      #endif

      /* Only stacked services have clients */
    #if (!PTIN_BOARD_IS_DNX)
      if (IS_EVC_QUATTRO(evc_id) || IS_EVC_STACKED(evc_id))
    #endif
      {
        printf("    Flows         = %u\n", evcs[evc_id].intf[i].clients.n_elems);

        /* SEM CLIENTS UP */
        osapiSemaTake(ptin_evc_clients_sem, L7_WAIT_FOREVER);

        dl_queue_get_head(&evcs[evc_id].intf[i].clients, (dl_queue_elem_t **) &pclientFlow);

        for (j=0; j<evcs[evc_id].intf[i].clients.n_elems; j++)
        {
        #if (PTIN_BOARD_IS_DNX)
          printf("      Flow# %-2u:  flags=0x%08x  vid=%4u+%-4u  (gport=0x%08x)\r\n",
                 j, pclientFlow->flags, pclientFlow->uni_ovid, pclientFlow->uni_ivid, pclientFlow->virtual_gport);
        #else
          #if (PTIN_QUATTRO_FLOWS_FEATURE_ENABLED)
          if (IS_EVC_QUATTRO(evc_id))
          {
            printf("      Flow# %-2u: flags=0x%04x int_vid=%4u+%-4u<->uni_vid=%4u+%-4u (gport=0x%04x) (Count {%s,%s}; BWProf {%s,%s})\r\n",
                   j, pclientFlow->flags & 0xffff,
                   pclientFlow->int_ovid, pclientFlow->int_ivid, pclientFlow->uni_ovid, pclientFlow->uni_ivid, pclientFlow->virtual_gport & 0xffff,
                   pclientFlow->counter[PTIN_EVC_INTF_ROOT]   != NULL ? "Root ON ":"Root OFF", pclientFlow->counter[PTIN_EVC_INTF_LEAF]   != NULL ? "Leaf ON ":"Leaf OFF",
                   pclientFlow->bwprofile[PTIN_EVC_INTF_ROOT][0] != NULL ? "Root ON ":"Root OFF", pclientFlow->bwprofile[PTIN_EVC_INTF_LEAF] != NULL ? "Leaf ON ":"Leaf OFF");
          }
          else
          #endif
          {
            printf("      Client# %2u: VID=%4u+%-4u  (Counter {%s,%s}; BWProf {%s,%s})\n", j, pclientFlow->uni_ovid, pclientFlow->int_ivid,
                   pclientFlow->counter[PTIN_EVC_INTF_ROOT]   != NULL ? "Root ON ":"Root OFF", pclientFlow->counter[PTIN_EVC_INTF_LEAF]   != NULL ? "Leaf ON ":"Leaf OFF",
                   pclientFlow->bwprofile[PTIN_EVC_INTF_ROOT][0] != NULL ? "Root ON ":"Root OFF", pclientFlow->bwprofile[PTIN_EVC_INTF_LEAF] != NULL ? "Leaf ON ":"Leaf OFF");
          }
        #endif

          pclientFlow = (struct ptin_evc_client_s *) dl_queue_get_next(&evcs[evc_id].intf[i].clients, (dl_queue_elem_t *) pclientFlow);
        }

        /* SEM CLIENTS DOWN */
        osapiSemaGive(ptin_evc_clients_sem);
      }
    }
    printf("\r\n");
  }

  fflush(stdout);
}

/**
 * Dumps the EVC related to the given internal vlan
 * 
 * @param int_vlan : internal vlan
 */
void ptin_evc_which(L7_uint int_vlan)
{
  L7_uint evc_id;

  printf("Which EVC corresponds to the internal vlan %u ?\r\n",int_vlan);

  if (int_vlan == 0 || int_vlan > 65535)
  {
    printf("Vlan %u is outside range (1-4095)!\r\n",int_vlan);
    return;
  }

  evc_id = evcId_from_internalVlan[int_vlan];

  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    printf("Vlan %u does not correspond to a valid EVC id (%u)\r\n",int_vlan,evc_id);
    return;
  }

  printf("Internal vlan %u => EVC %u\r\n\n",int_vlan,evc_id);

  fflush(stdout);

  /* Dump EVC */
  ptin_evc_dump(evcs[evc_id].extended_id);
}


/**
 * Dumps EVC extended index mapping 
 */
void ptin_evc_map(void)
{
  L7_uint32 evc_ext_id, evc_idx;

  printf("+---------+---------+\n");
  printf("| Ext. ID | Int. ID |\n");
  printf("+---------+---------+\n");

  for (evc_ext_id=0; evc_ext_id < PTIN_SYSTEM_N_EXTENDED_EVCS; evc_ext_id++) {

    /* Convert to internal evc id */
    if (ptin_evc_ext2int(evc_ext_id, &evc_idx) != L7_SUCCESS ||
        !evcs[evc_idx].in_use)
      continue;

    printf("|  %5u  |  %5d  |\n", evc_ext_id, evc_idx);
  }

  printf("+---------+---------+\n");

  fflush(stdout);
}



void sizeof_evc(void)
{
  PT_LOG_CRITIC(LOG_CTX_EVC, "sizeof(ptin_evc_client_s)     = %d", sizeof(struct ptin_evc_client_s));
  PT_LOG_CRITIC(LOG_CTX_EVC, "sizeof(ptin_evc_intf_s)       = %d", sizeof(struct ptin_evc_intf_s));
  PT_LOG_CRITIC(LOG_CTX_EVC, "sizeof(ptin_evc_s)            = %d", sizeof(struct ptin_evc_s));
  PT_LOG_CRITIC(LOG_CTX_EVC, "sizeof(ptin_IGMP_Statistics_t)= %d", sizeof(ptin_IGMP_Statistics_t));
  PT_LOG_CRITIC(LOG_CTX_EVC, "PTIN_SYSTEM_N_INTERF          = %d", PTIN_SYSTEM_N_INTERF);
  PT_LOG_CRITIC(LOG_CTX_EVC, "sizeof(IGMP statistics)       = %d", sizeof(ptin_IGMP_Statistics_t)*PTIN_SYSTEM_N_INTERF);
  PT_LOG_CRITIC(LOG_CTX_EVC, "sizeof(evcs)                  = %d", sizeof(evcs));
  PT_LOG_CRITIC(LOG_CTX_EVC, "sizeof(clients)               = %d", sizeof(clients));
  PT_LOG_CRITIC(LOG_CTX_EVC, "total(evcs+clients)           = %d", sizeof(evcs)+sizeof(clients));
}

