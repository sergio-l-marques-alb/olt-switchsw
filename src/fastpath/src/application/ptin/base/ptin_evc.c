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

#ifndef SNOOPING_API_H
#include "snooping_api.h"
#endif

#include "ptin_l2.h"
#include "ptin_cfg.h"
#include "ptin_packet.h"
#include "ptin_hal_erps.h"

#include "dtlinclude.h"
#include "usmdb_nim_api.h"

#include "ptin_env_api.h"

#include <vlan_port.h>
#if (PTIN_BOARD_IS_STANDALONE)
extern void ptin_flows_fpga_init(void);
#endif

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

/* EVC queue type */
typedef enum{
    PTIN_EVC_QUEUE_DEFAULT  = 0,
    PTIN_EVC_QUEUE_WIRED    = 0,
    PTIN_EVC_QUEUE_WIRELESS = 1,
    PTIN_EVC_QUEUE_MAX,
    PTIN_EVC_QUEUE_PORT = -1,
}ptin_evc_queue_type_t;

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

  /* VLAN actions */
  ptin_vlanXlate_action_enum action_outer_vlan;
  ptin_vlanXlate_action_enum action_inner_vlan;

  L7_uint16  client_vid;   /* Vlan identifying client (usually is the inner vlan) */

  /* GEM ids which will be flooded the ARP packets */
  L7_uint16  flood_vlan[PTIN_FLOOD_VLANS_MAX];
  L7_int     virtual_gport;
  L7_uint32  l2intf_id;
  L7_uint32  flags;         /* Client/flow flags */

  L7_uint8    macLearnMax;  // Maximum number of Learned MAC addresses                           
  L7_uint8    onuId;        // ONU/CPE Identifier
  L7_uint8    mask;
  L7_uint16   maxChannels;  // [mask = 0x01] Maximum number of channels this client can simultaneously watch
  L7_uint64   maxBandwidth; // [mask = 0x02] Maximum bandwidth that this client can simultaneously consume (bit/s)


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
  /* VLAN actions */
  ptin_vlanXlate_action_enum action_outer_vlan;
  ptin_vlanXlate_action_enum action_inner_vlan;

  L7_uint16  int_vlan;      /* Internal VLAN:
                             *  point-to-point - NOT APPLICABLE
                             *  point-to-multipoint - one internal VLAN per interface */

  L7_int32   l3_intf_id;     /* L3 Interface ID. */ 

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

  L7_int   queue_type;    /* 0 - WIRED/DEFAULT    
                           * 1 - WIRELESS */ 

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

  L7_int mcgroup;           /* Multicast group associated to this service (only GPON ports) */

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

/* Vlan Queues */
static struct ptin_vlan_s       vlans_pool[1<<12];              /* 4096 VLANs */

typedef enum
{
  PTIN_VLAN_TYPE_BITSTREAM=0,
  PTIN_VLAN_TYPE_CPU,
  PTIN_VLAN_TYPE_CPU_BCAST,
  PTIN_VLAN_TYPE_CPU_MCAST,
  PTIN_VLAN_TYPE_QUATTRO,
  PTIN_VLAN_TYPE_MAX         /* Do not change this constant */
} ptin_evc_type_enum_t;

static dl_queue_t queue_free_vlans[PTIN_VLAN_TYPE_MAX];

/* List with all the ports/lags used by EVCs */
static L7_uint8 evcs_intfs_in_use[PTIN_SYSTEM_N_INTERF];

/* Reference of evcid using internal vlan as reference */
static L7_uint32 evcId_from_internalVlan[4096];

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
/* Keep track of number of QUATTRO P2P evcs */
static L7_uint16 n_quattro_evcs = 0;
static L7_uint16 n_quattro_igmp_evcs = 0;

#define INCREMENT_QUATTRO_INSTANCE(evcId, counter)   { if (IS_EVC_QUATTRO(evcId) && IS_EVC_STACKED(evc_id))  (counter)++; }
#define DECREMENT_QUATTRO_INSTANCE(evcId, counter)   { if (IS_EVC_QUATTRO(evcId) && IS_EVC_STACKED(evc_id) && (counter)>0)  (counter)--; }

#define NO_INSTANCE(evcId, counter)       (!IS_EVC_QUATTRO(evcId) || ((counter) == 0))
#define SINGLE_INSTANCE(evcId, counter)   (!IS_EVC_QUATTRO(evcId) || ((counter) <= 1))
#else
#define INCREMENT_QUATTRO_INSTANCE(evcId, counter)   
#define DECREMENT_QUATTRO_INSTANCE(evcId, counter)   
#define NO_INSTANCE(evcId, counter)       (L7_TRUE)
#define SINGLE_INSTANCE(evcId, counter)   (L7_TRUE)
#endif

/* Local Macros */
#define IS_eEVC_IN_USE(a)             (ptin_evc_ext2int((a), L7_NULLPTR) == L7_SUCCESS)

#define IS_EVC_P2P(evc_id)            ((evcs[evc_id].flags & PTIN_EVC_MASK_P2P    ) == PTIN_EVC_MASK_P2P )
#define IS_EVC_P2MP(evc_id)           ((evcs[evc_id].flags & PTIN_EVC_MASK_P2P    ) == 0 )
#define IS_EVC_QUATTRO(evc_id)        ((evcs[evc_id].flags & PTIN_EVC_MASK_QUATTRO) == PTIN_EVC_MASK_QUATTRO )
#define IS_EVC_STD(evc_id)            ((evcs[evc_id].flags & PTIN_EVC_MASK_QUATTRO) == 0 )
#define IS_EVC_ETREE(evc_id)          ((evcs[evc_id].flags & PTIN_EVC_MASK_ETREE  ) == PTIN_EVC_MASK_ETREE )

#define IS_EVC_STD_P2P(evc_id)        (IS_EVC_STD(evc_id) && IS_EVC_P2P(evc_id))
#define IS_EVC_STD_P2MP(evc_id)       (IS_EVC_STD(evc_id) && IS_EVC_P2MP(evc_id))
#define IS_EVC_CPU_TRAP(evc_id)       ((evcs[evc_id].flags & PTIN_EVC_MASK_CPU_TRAPPING) == PTIN_EVC_MASK_CPU_TRAPPING)

#define IS_EVC_IPTV(evc_id)           ((evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV ) == PTIN_EVC_MASK_MC_IPTV)
#define IS_EVC_IGMP(evc_id)           ((evcs[evc_id].flags & PTIN_EVC_MASK_IGMP_PROTOCOL ) == PTIN_EVC_MASK_IGMP_PROTOCOL)
#define IS_EVC_BITSTREAM(evc_id)      (IS_EVC_STD_P2P(evc_id) && !IS_EVC_CPU_TRAP(evc_id))

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
  L7_uint16             igmp_inst;        /* IGMP instance */
  L7_uint16             dhcp_inst;        /* DHCP instance */
  L7_uint16             pppoe_inst;       /* PPPoE instance */
  void *next;
} ptinExtEvcIdInfoData_t;

typedef struct {
  avlTree_t                extEvcIdAvlTree;
  avlTreeTables_t         *extEvcIdTreeHeap;
  ptinExtEvcIdInfoData_t  *extEvcIdDataHeap;
} ptinExtEvcIdAvlTree_t;

ptinExtEvcIdAvlTree_t extEvcId_avlTree;


/* DriveShell functions */
L7_RC_t ptin_evc_allclientsflows_remove( L7_uint evc_id );                                                    /* Used by ptin_evc_delete */
L7_RC_t ptin_evc_intfclientsflows_remove( L7_uint evc_id, L7_uint8 intf_type, L7_uint8 intf_id );        /* Used by ptin_evc_destroy */
L7_RC_t ptin_evc_client_remove( L7_uint evc_id, L7_uint8 intf_type, L7_uint8 intf_id, L7_uint cvlan );

L7_RC_t ptin_evc_allclientsflows_dump( L7_uint evc_id );

void ptin_evc_clean_counters_enable( L7_BOOL enable );
void ptin_evc_clean_profiles_enable( L7_BOOL enable );

L7_RC_t ptin_evc_clean_all( L7_uint evc_id, L7_BOOL force );                                             /* Used by ptin_evc_delete */
L7_RC_t ptin_evc_intf_clean_all( L7_uint evc_id, L7_uint8 intf_type, L7_uint8 intf_id, L7_BOOL force );  /* Used by ptin_evc_destroy */

L7_RC_t ptin_evc_allintfs_clean( L7_uint evc_id, L7_BOOL force );                                        /* Used by ptin_evc_delete */
L7_RC_t ptin_evc_intf_clean( L7_uint evc_id, L7_uint8 intf_type, L7_uint8 intf_id, L7_BOOL force );      /* Used by ptin_evc_destroy */

L7_RC_t ptin_evc_allclients_clean( L7_uint evc_id, L7_BOOL force );
L7_RC_t ptin_evc_intfclients_clean( L7_uint evc_id, L7_uint8 intf_type, L7_uint8 intf_id, L7_BOOL force );
L7_RC_t ptin_evc_client_clean( L7_uint evc_id, L7_uint8 intf_type, L7_uint8 intf_id, L7_uint cvlan, L7_BOOL force );

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
//static L7_RC_t ptin_evc_port_flows_clean(L7_uint16 evc_id, L7_uint ptin_port);
static L7_RC_t ptin_evc_flow_unconfig(L7_int evc_id, L7_int ptin_port, L7_int16 uni_ovid);
#endif

#ifdef NGPON2_SUPPORTED
ptinExtNGEvcIdAvlTree_t extNGEvcId_avlTree;
#endif
/* Local functions prototypes */
static L7_RC_t ptin_evc_pclientFlow_clean( L7_uint evc_id, L7_uint ptin_port, struct ptin_evc_client_s *pclientFlow, L7_BOOL force );

static void    ptin_evc_entry_init(L7_uint evc_id);
static L7_RC_t ptin_evc_entry_allocate(L7_uint32 evc_ext_id, L7_uint *evc_id);
static L7_RC_t ptin_evc_entry_free(L7_uint32 evc_ext_id);

//static L7_RC_t ptin_evc_ext2int(L7_uint32 evc_ext_id, L7_uint32 *evc_id);
static L7_RC_t ptin_evc_extEvcInfo_get(L7_uint32 evc_ext_id, ptinExtEvcIdInfoData_t **infoData);

static void    ptin_evc_vlan_pool_init(void);
static L7_RC_t ptin_evc_freeVlanQueue_allocate(L7_uint16 evc_id, L7_uint32 evc_flags, dl_queue_t **freeVlan_queue);
static L7_RC_t ptin_evc_freeVlanQueue_free(dl_queue_t *freeVlan_queue);
static L7_RC_t ptin_evc_vlan_allocate(L7_uint16 *vlan, dl_queue_t *queue_vlans, L7_uint16 evc_id);
static L7_RC_t ptin_evc_vlan_free(L7_uint16 vlan, dl_queue_t *queue_vlans);

static L7_RC_t ptin_evc_intf_add(L7_uint evc_id, ptin_HwEthMef10Intf_t *intf_cfg);
static L7_RC_t ptin_evc_intf_remove(L7_uint evc_id, L7_uint ptin_port);
static L7_RC_t ptin_evc_intf_remove_all(L7_uint evc_id);

static L7_RC_t ptin_evc_etree_intf_add(L7_uint evc_id, L7_uint ptin_port);
static L7_RC_t ptin_evc_etree_intf_remove(L7_uint evc_id, L7_uint ptin_port);
static L7_RC_t ptin_evc_etree_intf_remove_all(L7_uint evc_id);

//static void    ptin_evc_intf_list_get(L7_uint evc_id, L7_uint8 mef_type, L7_uint intf_list[], L7_uint *n_elems);
static void    ptin_evc_find_client(L7_uint16 inn_vlan, dl_queue_t *queue, dl_queue_elem_t **pelem);
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
static void ptin_evc_find_flow(L7_uint16 uni_ovid, dl_queue_t *queue, dl_queue_elem_t **pelem);
static void ptin_evc_find_flow_from_l2intf(L7_uint32 l2intf_id, dl_queue_t *queue, dl_queue_elem_t **pelem);
#endif

static L7_RC_t switching_root_add(ptin_HwEthMef10Intf_t *intf_cfg, L7_uint16 int_vlan, L7_uint16 new_innerVlan,
                                  L7_BOOL egress_del_ivlan, L7_int force_pcp);
static L7_RC_t switching_root_remove(L7_uint root_intf, L7_uint16 out_vlan, L7_uint16 inner_vlan, L7_uint16 int_vlan);
static L7_RC_t switching_leaf_add(L7_uint leaf_intf, L7_uint16 leaf_int_vlan);
static L7_RC_t switching_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_int_vlan, L7_BOOL iptv_flag);

static L7_RC_t switching_elan_leaf_add(ptin_HwEthMef10Intf_t *intf_vlan, L7_uint16 int_vlan, L7_BOOL egress_del_ivid, L7_int force_pcp);
static L7_RC_t switching_elan_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan, L7_BOOL egress_del_ivid);

/* Leaf add/remove for MC evcs (active IPTV flag) */
#if ( !PTIN_BOARD_IS_MATRIX )
static L7_RC_t switching_mcevc_leaf_add(ptin_HwEthMef10Intf_t *intf_vlan, L7_uint16 int_vlan);
static L7_RC_t switching_mcevc_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan);
#endif

static L7_RC_t switching_etree_root_add(L7_uint root_intf, L7_uint16 root_out_vlan, L7_uint16 leaf_int_vlan, L7_BOOL egress_del_ivid);
static L7_RC_t switching_etree_root_remove(L7_uint root_intf, L7_uint16 root_out_vlan, L7_uint16 leaf_int_vlan);

static L7_RC_t switching_etree_leaf_add(L7_uint leaf_intf, L7_uint16 root_int_vlan);
static L7_RC_t switching_etree_leaf_remove(L7_uint leaf_intf, L7_uint16 root_int_vlan);

#if ( !PTIN_BOARD_IS_MATRIX )
static L7_RC_t switching_etree_stacked_leaf_add(ptin_HwEthMef10Intf_t *intf_vlan, L7_uint16 leaf_int_vlan, L7_uint16 root_int_vlan);
static L7_RC_t switching_etree_stacked_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inn_vlan, L7_uint16 leaf_int_vlan, L7_uint16 root_int_vlan);
#endif

static L7_RC_t switching_etree_unstacked_leaf_add(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_int_vlan, L7_uint16 root_int_vlan);
static L7_RC_t switching_etree_unstacked_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_int_vlan, L7_uint16 root_int_vlan);

static L7_RC_t switching_p2p_bridge_add(L7_uint root_intf, L7_uint16 root_int_vid, L7_uint leaf_intf, L7_uint16 leaf_out_vid, L7_uint16 leaf_inn_vid);
static L7_RC_t switching_p2p_bridge_remove(L7_uint root_intf, L7_uint16 root_int_vid, L7_uint leaf_intf, L7_uint16 leaf_out_vid, L7_uint16 leaf_inn_vid);

static L7_RC_t switching_vlan_create(L7_uint16 vid);
static L7_RC_t switching_vlan_delete(L7_uint16 vid);
static L7_RC_t switching_vlan_config(L7_uint16 vid, L7_uint16 fwd_vid, L7_BOOL mac_learning, L7_uint8 mc_flood, L7_uint8 cpu_trap, L7_int queue_type);

static L7_RC_t ptin_evc_param_verify(ptin_HwEthMef10Evc_t *evcConf);

static L7_RC_t ptin_evc_evcStats_verify(L7_uint evc_id, ptin_evcStats_profile_t *profile);

static L7_RC_t ptin_evc_probe_get(L7_uint evc_id, ptin_evcStats_profile_t *profile, ptin_evcStats_counters_t *stats);
static L7_RC_t ptin_evc_probe_add(L7_uint evc_id, ptin_evcStats_profile_t *profile);
static L7_RC_t ptin_evc_probe_delete(L7_uint evc_id, ptin_evcStats_profile_t *profile);

#if (!PTIN_BOARD_IS_MATRIX)
static 
L7_RC_t ptin_evc_update_dhcp (L7_uint16 evc_id, L7_uint32 *flags_ref, L7_BOOL dhcpv4_enabled, L7_BOOL dhcpv6_enabled,
                              L7_BOOL just_remove, L7_BOOL look_to_counters);
static 
L7_RC_t ptin_evc_update_pppoe(L7_uint16 evc_id, L7_uint32 *flags_ref, L7_BOOL pppoe_enabled,
                              L7_BOOL just_remove, L7_BOOL look_to_counters);
#endif

#if defined(IGMPASSOC_MULTI_MC_SUPPORTED) || PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
static 
L7_RC_t ptin_evc_update_igmp (L7_uint16 evc_id, L7_uint32 *flags_ref, L7_BOOL igmp_enabled,
                              L7_BOOL just_remove, L7_BOOL look_to_counters);
#endif

/* Semaphore to access EVC clients */
void *ptin_evc_clients_sem = L7_NULLPTR;

/* Semaphore to handle L3 Interfaces */
static void *__ptin_evc_l3_intf_sem = L7_NULLPTR;

#define L3_INTF_SEM_MAX_WAITING_PERIOD 100 /*100ms seconds*/

static L7_uint32 __vlanId   = (L7_uint32) -1;
static L7_uint32 __intfport = (L7_uint32) -1;

static void __ptin_evc_l3_intf_sem_handle(L7_uint32 vlanId, L7_uint32 ptin_port)
{
  L7_RC_t rc;
  PT_LOG_INFO(LOG_CTX_EVC, "Take Sem:%p vlanId:%u ptin_port:%u", __ptin_evc_l3_intf_sem, vlanId, ptin_port);
  rc = osapiSemaTake(__ptin_evc_l3_intf_sem, L3_INTF_SEM_MAX_WAITING_PERIOD);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Failed to Take Sem:%p vlanId:%u ptin_port:%u", __ptin_evc_l3_intf_sem, vlanId, ptin_port);
  }

  /*Are we on a clean state?*/
  if ( __vlanId == (L7_uint32) -1 && __intfport == (L7_uint32) -1)
  {
    /*Save Parameters*/
    __vlanId   = vlanId;
    __intfport = ptin_port;    
  }
  else
  {
    if ( __vlanId == vlanId && __intfport == ptin_port )
    {
      /*Clean  Internal Parameters*/
      __vlanId = (L7_uint32) -1;    
      __intfport = (L7_uint32) -1; 

      PT_LOG_INFO(LOG_CTX_EVC, "Give Sem %p vlanId:%u ptin_port:%u", __ptin_evc_l3_intf_sem, vlanId, ptin_port);
      /* SEM L3 Intf Down */
      osapiSemaGive(__ptin_evc_l3_intf_sem);
    }
  }
  return;
}

void ptin_evc_l3_intf_sem_give(L7_uint32 vlanId, L7_uint32 ptin_port)
{  
  if ( __vlanId == vlanId && __intfport == ptin_port )
  {
    PT_LOG_INFO(LOG_CTX_EVC, "Give Sem %p vlanId:%u ptin_port:%u", __ptin_evc_l3_intf_sem, vlanId, ptin_port);
    /* SEM L3 Intf Down */
    osapiSemaGive(__ptin_evc_l3_intf_sem);
  }

  return;
}

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


#ifdef NGPON2_SUPPORTED
  /* Extended EVCids AVL tree */
  memset(&extNGEvcId_avlTree, 0x00, sizeof(ptinExtNGEvcIdAvlTree_t));

  extNGEvcId_avlTree.extNGEvcIdTreeHeap = (avlTreeTables_t *) osapiMalloc(L7_PTIN_COMPONENT_ID, 256*sizeof(avlTreeTables_t));
  extNGEvcId_avlTree.extNGEvcIdDataHeap = (ptinExtNGEvcIdInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, 256*sizeof(ptinExtNGEvcIdInfoData_t));

  if ((extNGEvcId_avlTree.extNGEvcIdTreeHeap == L7_NULLPTR) ||
      (extNGEvcId_avlTree.extNGEvcIdDataHeap == L7_NULLPTR))
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Error allocating data for ExtEvcID AVL Tree\n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset (&extNGEvcId_avlTree.extNGEvcIdAvlTree, 0x00, sizeof(avlTree_t));
  memset ( extNGEvcId_avlTree.extNGEvcIdTreeHeap, 0x00, sizeof(avlTreeTables_t)*256);
  memset ( extNGEvcId_avlTree.extNGEvcIdDataHeap, 0x00, sizeof(ptinExtNGEvcIdInfoData_t)*256);

  // AVL Tree creations - snoopIpAvlTree
  avlCreateAvlTree(&(extNGEvcId_avlTree.extNGEvcIdAvlTree),
                   extNGEvcId_avlTree.extNGEvcIdTreeHeap,
                   extNGEvcId_avlTree.extNGEvcIdDataHeap,
                   256,
                   sizeof(ptinExtNGEvcIdInfoData_t),
                   0x10,
                   sizeof(ptinExtNGEvcIdDataKey_t));
  //
#endif
  /* Create semaphores */
  ptin_evc_clients_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_evc_clients_sem == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Failed to create ptin_evc_clients_sem semaphore!");
    return L7_FAILURE;
  }

  __ptin_evc_l3_intf_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (__ptin_evc_l3_intf_sem == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Failed to create ptin_evc_l3_intf_sem semaphore!");
    return L7_FAILURE;
  }
  

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  l2intf_db_init();
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
  L7_uint32 intIfNum_l2intf;

#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
  L7_RC_t rc;
  ptin_HwEthMef10Evc_t evcConf;
#if 0
  ptin_switch_mac_entry l2_entry;
  L7_uint8 smac_aspenA[L7_MAC_ADDR_LEN] = PTIN_ASPEN2CPU_A_SMAC;
  L7_uint8 smac_aspenB[L7_MAC_ADDR_LEN] = PTIN_ASPEN2CPU_B_SMAC;
#endif

#if 0
  /* Disable temporarily port for Aspens */
  rc = nimSetIntfAdminState(PTIN_ASPEN2CPU_A_PORT+1, L7_DISABLE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error disabling temporarily port A");
    return rc;
  }
  rc = nimSetIntfAdminState(PTIN_ASPEN2CPU_B_PORT+1, L7_DISABLE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error disabling temporarily port B");
    return rc;
  }
#endif

  /* 1st EVC for ASPEN A */
  memset(&evcConf, 0x00, sizeof(evcConf));
  evcConf.index         = PTIN_ASPEN2CPU_A_EVC;
  if (1==ptin_env_board_hwver()) {
      evcConf.flags = PTIN_EVC_MASK_MACLEARNING | PTIN_EVC_MASK_CPU_TRAPPING;
  }
  else {
      evcConf.flags = PTIN_EVC_MASK_MACLEARNING;
      //evcConf.flags = PTIN_EVC_MASK_P2P;
  }
  evcConf.mc_flood      = PTIN_EVC_MC_FLOOD_ALL;
  evcConf.internal_vlan = PTIN_ASPEN2CPU_A_VLAN;
  evcConf.n_intf        = 2;
  /* Root port */
  evcConf.intf[0].intf.format = PTIN_INTF_FORMAT_PORT;
  evcConf.intf[0].intf.value.ptin_port = PTIN_PORT_CPU;
  evcConf.intf[0].mef_type    = PTIN_EVC_INTF_ROOT;
  evcConf.intf[0].vid         = PTIN_ASPEN2CPU_A_VLAN;
  evcConf.intf[0].action_outer= PTIN_XLATE_ACTION_NONE;
  evcConf.intf[0].action_inner= PTIN_XLATE_ACTION_NONE;
  /* Leaf ports */
  evcConf.intf[1].intf.format = PTIN_INTF_FORMAT_INTIFNUM;
  evcConf.intf[1].intf.value.intIfNum = PTIN_ASPEN2CPU_A_INTIFNUM;
  evcConf.intf[1].mef_type    = PTIN_EVC_INTF_LEAF;
  evcConf.intf[1].vid         = PTIN_ASPEN2CPU_A_VLAN_EXT;
  evcConf.intf[1].action_outer= PTIN_XLATE_ACTION_REPLACE;
  evcConf.intf[1].action_inner= PTIN_XLATE_ACTION_NONE;

  /* Complete intf data (necessary for ptin_intf_any_format_t types) */
  if (ptin_intf_any_format(&evcConf.intf[0].intf) != L7_SUCCESS ||
      ptin_intf_any_format(&evcConf.intf[1].intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error with ptin_intf_any_format");
    return L7_FAILURE;
  }

  /* Creates EVC for Broadlights management */
  rc = ptin_evc_create(&evcConf);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error creating EVC# %lu for ASPEN management purposes", PTIN_ASPEN2CPU_A_EVC);
    return rc;
  }
  PT_LOG_INFO(LOG_CTX_API, "EVC# %lu created for ASPEN management purposes", PTIN_ASPEN2CPU_A_EVC);

  /* 2nd EVC for ASPEN B */
  memset(&evcConf, 0x00, sizeof(evcConf));
  evcConf.index         = PTIN_ASPEN2CPU_B_EVC;
  if (1==ptin_env_board_hwver()) {
      evcConf.flags = PTIN_EVC_MASK_MACLEARNING | PTIN_EVC_MASK_CPU_TRAPPING;
  }
  else {
      evcConf.flags = PTIN_EVC_MASK_MACLEARNING;
      //evcConf.flags = PTIN_EVC_MASK_P2P;
  }
  evcConf.mc_flood      = PTIN_EVC_MC_FLOOD_ALL;
  evcConf.internal_vlan = PTIN_ASPEN2CPU_B_VLAN;
  evcConf.n_intf        = 2;
  /* Root port */
  evcConf.intf[0].intf.format = PTIN_INTF_FORMAT_PORT;
  evcConf.intf[0].intf.value.ptin_port = PTIN_PORT_CPU;
  evcConf.intf[0].mef_type    = PTIN_EVC_INTF_ROOT;
  evcConf.intf[0].vid         = PTIN_ASPEN2CPU_B_VLAN;
  evcConf.intf[0].action_outer= PTIN_XLATE_ACTION_NONE;
  evcConf.intf[0].action_inner= PTIN_XLATE_ACTION_NONE;
  /* Leaf ports */
  evcConf.intf[1].intf.format = PTIN_INTF_FORMAT_INTIFNUM;
  evcConf.intf[1].intf.value.intIfNum = PTIN_ASPEN2CPU_B_INTIFNUM;
  evcConf.intf[1].mef_type    = PTIN_EVC_INTF_LEAF;
  evcConf.intf[1].vid         = PTIN_ASPEN2CPU_B_VLAN_EXT;
  evcConf.intf[1].action_outer= PTIN_XLATE_ACTION_REPLACE;
  evcConf.intf[1].action_inner= PTIN_XLATE_ACTION_NONE;

  /* Complete intf data (necessary for ptin_intf_any_format_t types) */
  if (ptin_intf_any_format(&evcConf.intf[0].intf) != L7_SUCCESS ||
      ptin_intf_any_format(&evcConf.intf[1].intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error with ptin_intf_any_format");
    return L7_FAILURE;
  }

  /* Creates EVC for Broadlights management */
  rc = ptin_evc_create(&evcConf);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error creating EVC# %lu for ASPEN management purposes", PTIN_ASPEN2CPU_B_EVC);
    return rc;
  }
  PT_LOG_INFO(LOG_CTX_API, "EVC# %lu created for ASPEN management purposes", PTIN_ASPEN2CPU_B_EVC);

#if 0
  /* Add static MACs for ASPENs */
  memset(&l2_entry, 0x00, sizeof(ptin_switch_mac_entry));
  memcpy(l2_entry.addr, smac_aspenA, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  l2_entry.evcId          = (L7_uint32)-1;
  l2_entry.vlanId         = PTIN_ASPEN2CPU_A_VLAN;
  l2_entry.intf.intf_type = PTIN_EVC_INTF_PHYSICAL;
  l2_entry.intf.intf_id   = PTIN_ASPEN2CPU_A_PORT;
  l2_entry.static_entry   = L7_TRUE;

  rc = ptin_l2_mac_table_entry_add(&l2_entry);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error adding static MAC address for ASPEN A");
    return rc;
  }

  memset(&l2_entry, 0x00, sizeof(ptin_switch_mac_entry));
  memcpy(l2_entry.addr, smac_aspenB, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  l2_entry.evcId          = (L7_uint32)-1;
  l2_entry.vlanId         = PTIN_ASPEN2CPU_B_VLAN;
  l2_entry.intf.intf_type = PTIN_EVC_INTF_PHYSICAL;
  l2_entry.intf.intf_id   = PTIN_ASPEN2CPU_B_PORT;
  l2_entry.static_entry   = L7_TRUE;

  rc = ptin_l2_mac_table_entry_add(&l2_entry);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error adding static MAC address for ASPEN A");
    return rc;
  }
#endif

  /* Configure inband */
  rc = ptin_cfg_tc16sxg_aspen_packets(L7_ENABLE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error configuring interception packets for ASPEN Inband management");
    return rc;
  }
  PT_LOG_INFO(LOG_CTX_API, "Interception packets for ASPEN Inband management ready!");

  rc = ptin_cfg_tc16sxg_aspen_bridge_set();
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error configuring bridge for ASPEN Inband management");
    return rc;
  }

#if 0
  /* Reenable port for Aspens */
  rc = nimSetIntfAdminState(PTIN_ASPEN2CPU_A_PORT+1, L7_ENABLE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error reenabling port A");
    return rc;
  }
  rc = nimSetIntfAdminState(PTIN_ASPEN2CPU_B_PORT+1, L7_ENABLE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error reenabling port B");
    return rc;
  }
#endif

  PT_LOG_INFO(LOG_CTX_API, "Bridge for ASPEN Inband management ready!");
#elif (PTIN_BOARD == PTIN_BOARD_OLT1T0)
  L7_int  i;
  L7_RC_t rc;
  ptin_HwEthMef10Evc_t evcConf;

  /* Create a new EVC */
  memset(&evcConf, 0x00, sizeof(evcConf));
  evcConf.index         = PTIN_EVC_BL2CPU;
  evcConf.flags         = PTIN_EVC_MASK_MACLEARNING;
  evcConf.mc_flood      = PTIN_EVC_MC_FLOOD_ALL;
  evcConf.internal_vlan = PTIN_VLAN_BL2CPU;
  evcConf.n_intf        = 5;
  /* Root port */
  evcConf.intf[0].intf.format = PTIN_INTF_FORMAT_PORT;
  evcConf.intf[0].intf.value.ptin_port = PTIN_PORT_CPU;
  evcConf.intf[0].mef_type    = PTIN_EVC_INTF_ROOT;
  evcConf.intf[0].vid         = PTIN_VLAN_BL2CPU_EXT;
  evcConf.intf[0].action_outer= PTIN_XLATE_ACTION_REPLACE;
  evcConf.intf[0].action_inner= PTIN_XLATE_ACTION_NONE;
  /* Leaf ports */
  for (i=1; i<5; i++)
  {
    evcConf.intf[i].intf.format = PTIN_INTF_FORMAT_PORT;
    evcConf.intf[i].intf.value.ptin_port = (i-1)*2;
    evcConf.intf[i].mef_type    = PTIN_EVC_INTF_LEAF;
    evcConf.intf[i].vid         = PTIN_VLAN_BL2CPU_EXT;
    evcConf.intf[i].action_outer= PTIN_XLATE_ACTION_REPLACE;
    evcConf.intf[i].action_inner= PTIN_XLATE_ACTION_NONE;
  }

  /* Creates EVC for Broadlights management */
  rc = ptin_evc_create(&evcConf);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error creating EVC# %lu for Broadlight management purposes", PTIN_EVC_BL2CPU);
    return rc;
  }
#endif
#if (PTIN_BOARD_IS_STANDALONE)
  /* Only configure special EVCs, for CPU-FPGA-Ports connectivity, if OLT1T0-AC equipment */
#if (PTIN_BOARD == PTIN_BOARD_OLT1T0F)
  if (1)
#else
  if (KERNEL_NODE_IS("OLT1T0-AC")) 
#endif
  {
    ptin_flows_fpga_init();
  }
#endif

  PT_LOG_INFO(LOG_CTX_API, "Standard EVCs configured for OLT1T0 equipment");

  /* Create intIfNum for Virtual ports */
  if (L7_SUCCESS != vlan_port_intIfNum_create(1, &intIfNum_l2intf))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error creating intIfNum for virtual ports");
    return L7_FAILURE;
  }
  PT_LOG_NOTICE(LOG_CTX_EVC, "Success creating intIfNum for virtual ports: %u", intIfNum_l2intf);

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
 * Determines if a particular Port/LAG is being used on EVC Id
 *  
 * @param evc_ext_id  
 * @param ptin_port 
 * 
 * @return L7_BOOL L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_evc_is_intf_in_use_on_evc(L7_uint32 evc_ext_id, L7_uint ptin_port)
{  
  L7_int evc_id;

  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
    return L7_FALSE;

  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
    return L7_FALSE;

  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
     PT_LOG_ERR(LOG_CTX_EVC, "evc_ext_id:0x%x is invalid", evc_ext_id);
     return L7_FALSE;
  }

  if (evcs[evc_id].in_use == L7_FALSE)
    return L7_FALSE;

  return (evcs[evc_id].intf[ptin_port].in_use);    
}

/**
 * Determines if a particular Port/LAG is Leaf on EVC Id
 *  
 * @param evc_ext_id  
 * @param ptin_port 
 * 
 * @return L7_BOOL L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_evc_is_intf_leaf(L7_uint32 evc_ext_id, L7_uint ptin_port)
{  
  L7_int evc_id;
  L7_RC_t   rc;

  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
    return L7_FALSE;

  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
    return L7_FALSE;

  rc = ptin_evc_ext2int(evc_ext_id, &evc_id);
  if ( rc != L7_SUCCESS)
  {
     if (rc != L7_NOT_EXIST)
     {
       PT_LOG_ERR(LOG_CTX_EVC, "evc_ext_id:0x%x is invalid", evc_ext_id);
     }
     else
     {
       PT_LOG_TRACE(LOG_CTX_EVC, "Evc does not exist evc_ext_id:0x%x", evc_ext_id);
     }

     return L7_FALSE;
  }

  if (evcs[evc_id].in_use == L7_FALSE)
    return L7_FALSE;

  if (evcs[evc_id].intf[ptin_port].in_use == L7_FALSE)
    return L7_FALSE;

  if (evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF)
    return L7_FALSE;

  return L7_TRUE;
}


/**
 * Determines if a particular Port/LAG is in use
 *  
 * @param evc_ext_id  
 * @param ptin_port 
 * 
 * @return L7_BOOL L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_evc_is_port_in_use(L7_uint32 evc_id, L7_uint ptin_port)
{  
  if (evc_id >= PTIN_SYSTEM_N_EVCS)
    return L7_FALSE;

  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
    return L7_FALSE;

  if (evcs[evc_id].in_use == L7_FALSE)
    return L7_FALSE;

  if (evcs[evc_id].intf[ptin_port].in_use == L7_FALSE)
    return L7_FALSE;

  return L7_TRUE;
}
/**
 * Get port type on EVC Id
 *  
 * @param evc_ext_id  
 * @param ptin_port 
 * @param portType  
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_port_type_get(L7_uint32 evc_ext_id, L7_uint ptin_port, L7_uint8 *portType)
{  
  L7_int evc_id;

  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS || ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (evc_ext_id=0x%x ptin_port:%u)", evc_ext_id, ptin_port);
    return L7_FAILURE;
  }
  
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
     PT_LOG_ERR(LOG_CTX_EVC, "evc_ext_id:0x%x is invalid", evc_ext_id);
     return L7_FAILURE;
  }

  if (evcs[evc_id].in_use == L7_FALSE)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "evc_ext_id:0x%x is not in use (evc_id:%u)", evc_id, evc_ext_id);
    return L7_FAILURE;
  }

  if (evcs[evc_id].intf[ptin_port].in_use == L7_FALSE)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "ptin_port:%u is not in use on evc_ext_id:0x%x  (evc_id:%u)", ptin_port, evc_ext_id, evc_id);
    return L7_FAILURE;
  }

  if (evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF && evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_ROOT)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid portType:%u of ptin_port %u on evc_ext_id:0x%x  (evc_id:%u)", evcs[evc_id].intf[ptin_port].type, ptin_port, evc_ext_id, evc_id);
    return L7_FAILURE;
  }

  if (ptin_debug_evc)
    PT_LOG_TRACE(LOG_CTX_EVC, "portType:%u of ptin_port:%u on evc_ext_id:0x%x  (evc_id:%u)", evcs[evc_id].intf[ptin_port].type, ptin_port, evc_ext_id, evc_id);
  *portType = evcs[evc_id].intf[ptin_port].type;

  return L7_SUCCESS;
}

/**
 * Get port type on Intenal VLAN Id
 *  
 * @param internalVlan  
 * @param ptin_port 
 * @param portType  
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_BOOL ptin_evc_internal_vlan_port_type_get(L7_uint32 internalVlan, L7_uint32 ptin_port, L7_uint8 *portType)
{
  L7_uint32   evc_id;

  /* Validate arguments */
  if (internalVlan>=4096 || ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (intVlan=%u ptin_port:%u)",internalVlan, ptin_port);
    return L7_FAILURE;
  }

  /* Get evc id */
  evc_id = evcId_from_internalVlan[internalVlan];

  /* Check if this internal vlan is in use by any evc */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Failed to obtain evc_id from internalVlan:%u", internalVlan);
    return L7_FAILURE;
  } 
    
  if (evcs[evc_id].in_use == L7_FALSE)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "evc_id:%u is not in use (internalVlan:%u)", evc_id, internalVlan);
    return L7_FAILURE;
  }

  if (evcs[evc_id].intf[ptin_port].in_use == L7_FALSE)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "ptin_port %u is not in use on internalVlan:%u  (evc_id:%u)", ptin_port, internalVlan, evc_id);
    return L7_FAILURE;
  }

  if (evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF && evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_ROOT)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid portType:%u of ptin_port:%u on internalVlan:%u  (evc_id:%u)", evcs[evc_id].intf[ptin_port].type, ptin_port, internalVlan, evc_id);
    return L7_FAILURE;
  }

  if (ptin_debug_evc)
    PT_LOG_TRACE(LOG_CTX_EVC, "portType:%u of ptin_port:%u on internalVlan:%u  (evc_id:%u)", evcs[evc_id].intf[ptin_port].type, ptin_port, internalVlan, evc_id);
  *portType = evcs[evc_id].intf[ptin_port].type;

  return L7_SUCCESS;
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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
  //memset(evcConf->ce_vid_bmp, 0x00, sizeof(evcConf->ce_vid_bmp));

  /* Return number of attached clients */
  evcConf->n_clientflows = evcs[evc_id].n_clientflows;

  evcConf->n_intf = 0;
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
  {
    if (evcs[evc_id].intf[i].in_use)
    {
      /* PTin port */
      evcConf->intf[evcConf->n_intf].intf.format = PTIN_INTF_FORMAT_PORT;
      evcConf->intf[evcConf->n_intf].intf.value.ptin_port = i;
      /* Any format */
      if (ptin_intf_any_format(&evcConf->intf[evcConf->n_intf].intf) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Error converting ptin_port %u to any format", i);
        return L7_FAILURE;
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
 * @param internalVlan : Internal vlan
 * @param evcConf      : Pointer to the evc configuration struct
 * 
 * @return L7_RC_t : L7_SUCCESS   - Success extracting evc data
 *                   L7_FAILURE   - Failure reading evc data
 */
L7_RC_t ptin_evc_get_fromIntVlan(L7_uint16 internalVlan, ptin_HwEthMef10Evc_t *evcConf)
{
  L7_uint evc_id;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if (internalVlan>=4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (intVlan=%u)",internalVlan);
    return L7_FAILURE;
  }

  /* Get evc id */
  evc_id = evcId_from_internalVlan[internalVlan];

  /* Check if this internal vlan is in use by any evc */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    return L7_FAILURE;
  }

  /* Check if this evc is in use... if not we have a non-consistent situation */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u (intVlan=%u) should be in use",evc_id,internalVlan);
    return L7_FAILURE;
  }

  /* Get evc data */
  if (evcConf!=L7_NULLPTR)
  {
    evcConf->index = evcs[evc_id].extended_id;
    rc = ptin_evc_get(evcConf);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Error getting evc data from internalVlan=%u, evcId=%u",internalVlan,evc_id);
    }
  }

  return rc;
}

/**
 * Gets an EVC configuration from an internal vlan as input 
 * parameter 
 *  
 * @param internalVlan : Internal vlan
 * @param evc_ext_id   : EVC extended id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_get_evcIdfromIntVlan(L7_uint16 internalVlan, L7_uint32 *evc_ext_id)
{
  L7_uint evc_id;

  /* Validate arguments */
  if (internalVlan>=4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (intVlan=%u)",internalVlan);
    return L7_FAILURE;
  }

  /* Get evc id */
  evc_id = evcId_from_internalVlan[internalVlan];

  /* Check if this internal vlan is in use by any evc */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    return L7_FAILURE;
  }

  /* Check if this evc is in use... if not we have a non-consistent situation */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u (intVlan=%u) should be in use",evc_id,internalVlan);
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
 * @param internalVlan : Internal vlan
 * @param evc_ext_id   : EVC extended id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_get_internal_evcIdfromIntVlan(L7_uint16 internalVlan, L7_uint32 *evc_id)
{
  
  /* Validate arguments */
  if (internalVlan>=4096 || evc_id == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (intVlan=%u evc_id=%p)",internalVlan, evc_id);
    return L7_FAILURE;
  }

  /* Get evc id */
  *evc_id = evcId_from_internalVlan[internalVlan];

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
    PT_LOG_TRACE(LOG_CTX_EVC,"Invalid arguments (nni_ovid=%u)",nni_ovid);
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
 * Get (internal) VLAN, from NNI outer and NNI Inner vlan
 * 
 * @param evc_id    : Internal EVC id
 * @param intVid    : Internal VLAN id 
 * @param nni_ivid  : client nni_ivid 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST
 */
L7_RC_t ptin_evc_getCVlan_fromIntVlan(L7_uint32 evc_id, L7_uint16 intVlan, L7_uint16 *nni_ivid)
{
    int i;
    struct ptin_evc_client_s *pclientFlow;

    if (!evcs[evc_id].in_use)
    {
      PT_LOG_TRACE(LOG_CTX_EVC,"Invalid arguments (evc_id=%u)",evc_id);
      return L7_FAILURE;
    }
    /* Validate arguments */
    if (intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX)
    {
      PT_LOG_TRACE(LOG_CTX_EVC,"Invalid arguments (intVlan=%u)",intVlan);
      return L7_FAILURE;
    }

    if (!IS_EVC_STACKED(evc_id))
    {
      PT_LOG_TRACE(LOG_CTX_EVC,"Invalid arguments. Only valid for stacked services");
      return L7_FAILURE;
    }

    for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
    {
      if (!evcs[evc_id].intf[i].in_use)
        continue;

      if(evcs[evc_id].intf[i].clients.n_elems > 1)
      {
          PT_LOG_TRACE(LOG_CTX_EVC,"Not supported. Only 1 client");
          return L7_FAILURE;
      }

      if (evcs[evc_id].intf[i].type == PTIN_EVC_INTF_ROOT)
      {
          if (evcs[evc_id].intf[i].int_vlan) 
          {
              *nni_ivid = evcs[evc_id].intf[i].inner_vlan;
              PT_LOG_TRACE(LOG_CTX_EVC," (nni_ivid=%u)",*nni_ivid);
              return L7_SUCCESS;
          }
      }
      else
      {
        /* SEM CLIENTS UP */
        osapiSemaTake(ptin_evc_clients_sem, L7_WAIT_FOREVER);

        pclientFlow = L7_NULLPTR;
        dl_queue_get_head(&evcs[evc_id].intf[i].clients, (dl_queue_elem_t **) &pclientFlow);

        if ( pclientFlow != L7_NULLPTR ) 
        {
            *nni_ivid = pclientFlow->int_ivid;
            /* SEM CLIENTS DOWN */
            osapiSemaGive(ptin_evc_clients_sem);
            return L7_SUCCESS;
        }

        /* SEM CLIENTS DOWN */
        osapiSemaGive(ptin_evc_clients_sem);

      }
    }

    return L7_SUCCESS;
}


/**
 * Get (internal) VLAN, from NNI outer and NNI Inner vlan
 * 
 * @param nni_ovid  : NNI OVLAN
 * @param nni_ivid  : NNI OVLAN
 * @param intVid    : Internal VLAN id 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST
 */
L7_RC_t ptin_evc_get_intVlan_fromNNI_vlans(L7_uint16 nni_ovid, L7_uint16 nni_ivid, L7_uint16 *intVid)
{
    L7_uint evc_id;

    /* Validate arguments */
    if (nni_ovid == 0 || nni_ovid >= 4096)
    {
        PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (nni_ovid=%u)",nni_ovid);
        return L7_FAILURE;
    }
    if (nni_ivid == 0 || nni_ivid >= 4096)
    {
        PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (nni_ivid=%u)",nni_ivid);
        return L7_FAILURE;
    }

    /* Run all EVCs */
    for (evc_id = 0; evc_id < PTIN_SYSTEM_N_EVCS; evc_id++)
    {
        /* Skip not used EVCs */
        if (!evcs[evc_id].in_use)
        {
            continue;
        }

        /* check for NNI VLAN */
        if ((evcs[evc_id].root_info.nni_ovid == nni_ovid) &&
            (evcs[evc_id].root_info.nni_ivid == nni_ivid))
        {
            if (intVid != L7_NULLPTR)
            {
                *intVid = evcs[evc_id].rvlan;        
                return L7_SUCCESS;
            }
        }
    }

    return L7_NOT_EXIST; 
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
 * @param intVlan    : Internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intVlan_get(L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, L7_uint16 *intVlan)
{
  L7_uint32 ptin_port;
  L7_uint16 internal_vlan;
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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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

  internal_vlan = evcs[evc_id].intf[ptin_port].int_vlan;

  /* Validate interval vlan */
  if (internal_vlan<PTIN_VLAN_MIN || internal_vlan>PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Evc %u, port %u, has an invalid int vlan (%u)",
            evc_id, ptin_port, internal_vlan);
    return L7_FAILURE;
  }

  /* Return internal vlan */
  if (intVlan!=L7_NULLPTR)
  {
    *intVlan = internal_vlan;
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
  L7_RC_t   rc;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  rc = ptin_evc_ext2int(evc_ext_id, &evc_id);
  if ( rc != L7_SUCCESS)
  {
    if ( rc != L7_NOT_EXIST)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    }
    return L7_NOT_EXIST;
  }

  /* Return root vlan */
  if (intRootVlan != L7_NULLPTR) /*Do Not Remove this validation!*/
    *intRootVlan = evcs[evc_id].rvlan;

  return L7_SUCCESS;
}

/**
 * Gets the root vlan (internal) from the internal vlan
 * 
 * @param intVlan     : Internal vlan
 * @param intRootVlan : Internal root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intRootVlan_get_fromIntVlan(L7_uint16 intVlan, L7_uint16 *intRootVlan)
{
  L7_uint32 evc_id;

  /* Validate arguments */
  if (intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[intVlan];
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC", intVlan);
    return L7_FAILURE;
  }
  /* Is EVC in use? */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u (intVlan=%u) is not in use", evc_id, intVlan);
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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
 * @param intVlan   : Internal OVlan
 * @param flags     : Flag options 
 * @param mc_flood  : Multicast flood
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_flags_get_fromIntVlan(L7_uint16 intOVlan, L7_uint32 *flags, L7_uint32 *mc_flood)
{
  L7_uint   evc_id;
  L7_uint32 evc_ext_id;

  /* Validate arguments */
  if (intOVlan < PTIN_VLAN_MIN || intOVlan > PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

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
    PT_LOG_ERR(LOG_CTX_EVC,"Error getting EVC flags for evc_ext_id=0x%x, intOVlan=%u", evc_ext_id, intOVlan);
    return L7_FAILURE;
  }

  return L7_SUCCESS;

}

/**
 * Get the outer+inner external vlan for a specific 
 * interface+evc_id+innerVlan. 
 *  
 * @param ptin_port       : FP interface# 
 * @param evc_ext_id      : EVC extended index 
 * @param evc_int_id      : EVC internal index  
 * @param innerVlan       : Inner vlan
 * @param extOVlan        : External outer-vlan 
 * @param extIVlan        : External inner-vlan (01 means that there 
 *                      is no inner vlan)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_extVlans_get(L7_uint32 ptin_port, L7_uint32 evc_ext_id, L7_uint32 evc_int_id, L7_uint16 innerVlan, L7_uint16 *extOVlan, L7_uint16 *extIVlan)
{
  L7_uint16 ovid, ivid;
  struct ptin_evc_client_s *pclientFlow;

  /* Validate ptin_port */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }
  
  /* Validate arguments */
  if (evc_int_id!=(L7_uint32)-1  && evc_int_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
        PT_LOG_ERR(LOG_CTX_EVC,"There is no flow with gemId=%u in ptin_port=%u and EVC=%u",innerVlan,ptin_port,evc_int_id);
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
        PT_LOG_ERR(LOG_CTX_EVC,"There is no client/flow with cvid=%u in ptin_port=%u and EVC=%u",innerVlan,ptin_port,evc_int_id);
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
 * Get the outer+inner external vlan for a specific 
 * evc_id+l2intf (only applicable to QUATTRO services). 
 *  
 * @param evc_ext_id      : EVC extended index 
 * @param evc_int_id      : EVC internal index  
 * @param l2intf_id       : l2intf_id 
 * @param port            : Physical port for transmission (out)
 * @param extOVlan        : External outer-vlan (out)
 * @param extIVlan        : External inner-vlan (01 means that there 
 *                      is no inner vlan) (out)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_extVlans_get_from_l2intf(L7_uint32 evc_ext_id, L7_uint32 evc_int_id, L7_uint32 l2intf_id,
                                          L7_uint32 *port, L7_uint16 *extOVlan, L7_uint16 *extIVlan)
{
  L7_uint32 ptin_port;
  L7_uint16 ovid, ivid;
  l2intf_entry_t l2intf_entry;
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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
  memset(&l2intf_entry, 0x00, sizeof(l2intf_entry));

  if (l2intf_db_data_get(l2intf_id, &l2intf_entry) == L7_SUCCESS)
  {
    /* Validate interface */
    if (ptin_intf_ptintf2port(&l2intf_entry.pon, &ptin_port) != L7_SUCCESS ||
        ptin_port >= ptin_sys_number_of_ports)
    {
      PT_LOG_ERR(LOG_CTX_DAI, "Error obtaining ptin_port from PON interface (%u/%u)", l2intf_entry.pon.intf_type, l2intf_entry.pon.intf_id);
      return L7_FAILURE;
    }
    /* Validate GEM id */
    else if (l2intf_entry.gem_id == 0 || l2intf_entry.gem_id >= 4095)
    {
      PT_LOG_ERR(LOG_CTX_DAI, "Invalid GEM id %u", l2intf_entry.gem_id);
      return L7_FAILURE;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_DAI, "l2intf_id %u not found", l2intf_id);
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
    ptin_evc_find_flow_from_l2intf(l2intf_id, &(evcs[evc_int_id].intf[ptin_port].clients), (dl_queue_elem_t **) &pclientFlow);
    if (pclientFlow==NULL)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"There is no flow with l2intf_id=%u in ptin_port=%u and EVC=%u",l2intf_id,ptin_port,evc_int_id);
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
 * @param ptin_intf: Interface
 * @param extOVlan : external outer vlan
 * @param extIVlan : external inner vlan
 * @param intOVlan : internal outer vlan
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_evc_intVlan_get_fromOVlan(ptin_intf_t *ptin_intf, L7_uint16 extOVlan, L7_uint16 extIVlan,
                                       L7_uint16 *intOVlan)
{
  L7_uint16 internal_vlan;
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
      continue;
    
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
  internal_vlan = evcs[evc_id].intf[ptin_port].int_vlan;

  /* Validate internal vlan */
  if (internal_vlan<PTIN_VLAN_MIN || internal_vlan>PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid internal vlan %u for EVC %u and ptin_intf %u/%u",
            internal_vlan, evc_id, ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Return internal vlan */
  if (intOVlan!=L7_NULLPTR)
    *intOVlan = internal_vlan;
    
  return L7_SUCCESS;
}

/**
 * Get interface type for a given internal vlan
 * 
 * @param intVlan  : Internal vlan 
 * @param ptin_port: Interface
 * @param type     : Interface type (output)
 *                    PTIN_EVC_INTF_ROOT=0,
 *                    PTIN_EVC_INTF_LEAF=1,
 *                    PTIN_EVC_INTF_NOTUSED=255
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intf_type_get(L7_uint16 intVlan, L7_uint32 ptin_port, L7_uint8 *type)
{
  ptin_evc_intfCfg_t intfCfg;
  L7_uint16 evc_id;
  ptin_intf_t ptin_intf;

  /* Convert from ptin_port to ptin_intf */
  if (ptin_intf_port2ptintf(ptin_port, &ptin_intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }

  /* Validate arguments */
  if (intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid intVlan %u", intVlan);
    return L7_FAILURE;
  }

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[intVlan];
  if (evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",intVlan);
    return L7_FAILURE;
  }

  /* Get MC EVC configuration */
  memset(&intfCfg, 0x00, sizeof(intfCfg));

  PT_LOG_TRACE(LOG_CTX_EVC,"ptin_evc_intfCfg_get evc %u (intVlan=%u), intf=%u/%u configuration", evc_id, intVlan, ptin_intf.intf_type, ptin_intf.intf_id);

  if (ptin_evc_intfCfg_get(evcs[evc_id].extended_id, &ptin_intf, &intfCfg) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error getting evc %u (intVlan=%u), intf=%u/%u configuration", evc_id, intVlan, ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }

  /* type pointer must not be null */
  if (type != L7_NULLPTR)
  {
    if (ptin_debug_dhcp_snooping)
    {
      PT_LOG_TRACE(LOG_CTX_EVC,"intfCfg.in_use=%u intfCfg.type=%u",intfCfg.in_use, intfCfg.type);
    }

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
 * @param intVlan  : Internal vlan 
 * @param ptin_port: Interface
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_evc_intf_isRoot(L7_uint16 intVlan, L7_uint32 ptin_port)
{
  L7_uint8 intf_type;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      (intVlan != 0 && (intVlan < PTIN_VLAN_MIN || intVlan > PTIN_VLAN_MAX)) )
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid arguments: ptin_port=%u intVlan=%u", ptin_port, intVlan);
    return L7_FALSE;
  }

  /* If VLAN is null, return general trusted state */
  if (intVlan == 0)
  {
    PT_LOG_WARN(LOG_CTX_DHCP, "intVlan=0 return true");
    return L7_TRUE;
  }

  /* Get interface configuration */
  if (ptin_evc_intf_type_get(intVlan, ptin_port, &intf_type)!=L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_DHCP, "Error acquiring interface type from internalVid %u and ptin_port %u", intVlan, ptin_port);
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
 * @param intVlan  : Internal vlan 
 * @param type     : Interface type 
 *                    PTIN_EVC_INTF_ROOT=0,
 *                    PTIN_EVC_INTF_LEAF=1
 * @param intfList : List of interfaces
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_intfType_getList(L7_uint16 intVlan, L7_uint8 type, ptin_port_bmp_t *intfList)
{
  ptin_HwEthMef10Evc_t evcCfg;
  L7_uint intf_idx;
  L7_uint32 ptin_port;
  L7_uint16 evc_id;

  /* Validate arguments */
  if (intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[intVlan];
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",intVlan);
    return L7_FAILURE;
  }

  /* Get MC EVC configuration */
  memset(&evcCfg,0x00,sizeof(ptin_HwEthMef10Evc_t));
  evcCfg.index = evcs[evc_id].extended_id;
  if (ptin_evc_get(&evcCfg)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      PT_LOG_ERR(LOG_CTX_EVC,"Error getting evc %u configuration (intVlan=%u)",evc_id,intVlan);
    return L7_FAILURE;
  }

  /* interface list pointer must not be null */
  if (intfList==L7_NULLPTR)
  {
    return L7_SUCCESS;
  }

  /* clear interface list */
  memset(intfList,0x00,sizeof(ptin_port_bmp_t));

  /* Run all interfaces */
  for (intf_idx=0; intf_idx<evcCfg.n_intf; intf_idx++)
  {
    if (evcCfg.intf[intf_idx].mef_type==type)
    {
      ptin_port = evcCfg.intf[intf_idx].intf.value.ptin_port;

      PTINPORT_BITMAP_SET(*intfList, ptin_port);
    }
  }

  return L7_SUCCESS;
}

/**
 * Get the outer+inner external vlan for a specific 
 * interface+outer+inner internal vlan. 
 *  
 * @param ptin_port  : FP interface#
 * @param intOVlan   : Internal outer-vlan 
 * @param intIVlan   : Internal inner-vlan (0 means that there 
 *                     is no inner vlan)
 * @param extOVlan   : External outer-vlan 
 * @param extIVlan   : External inner-vlan (01 means that there 
 *                     is no inner vlan)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_extVlans_get_fromIntVlan(L7_uint32 ptin_port, L7_uint16 intOVlan, L7_uint16 intIVlan, L7_uint16 *extOVlan, L7_uint16 *extIVlan)
{
  L7_uint   evc_int_id;
  L7_uint32 evc_ext_id;

  /* Validate arguments */
  if (intOVlan<PTIN_VLAN_MIN || intOVlan>PTIN_VLAN_MAX)
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
  if (ptin_evc_extVlans_get(ptin_port, evc_ext_id, evc_int_id, intIVlan, extOVlan, extIVlan)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error getting external vlans for ptin_port=%u, evc_ext_id=0x%x, intIVlan=%u",ptin_port,evc_ext_id,intIVlan);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
/**
 * Get the outer+inner external vlan for a specific 
 * oVLAN+l2intf_id (only applicable to QUATTRO services). 
 * 
 * @param intOVlan   : Internal outer-vlan 
 * @param l2intf_id  : l2intf_id
 * @param ptin_port  : Physical port for transmission (out)
 * @param extOVlan   : External outer-vlan 
 * @param extIVlan   : External inner-vlan (01 means that there 
 *                     is no inner vlan)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_extVlans_get_from_IntVlan_l2intf(L7_uint16 intOVlan, L7_uint32 l2intf_id,
                                                  L7_uint32 *ptin_port, L7_uint16 *extOVlan, L7_uint16 *extIVlan)
{
  L7_uint   evc_int_id;
  L7_uint32 evc_ext_id;

  /* Validate arguments */
  if (intOVlan<PTIN_VLAN_MIN || intOVlan>PTIN_VLAN_MAX)
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
  if (ptin_evc_extVlans_get_from_l2intf(evc_ext_id, evc_int_id, l2intf_id, ptin_port, extOVlan, extIVlan)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error getting external vlans for evc_ext_id=0x%x, l2intf_id=%u",evc_ext_id,l2intf_id);
    return L7_FAILURE;
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
 * @param intVlan    : Internal outer-vlan 
 * @param evc_type   : evc type (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_check_evctype_fromIntVlan(L7_uint16 intVlan, L7_uint8 *evc_type)
{
  L7_uint evc_id;

  /* Validate arguments */
  if (intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[intVlan];
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",intVlan);
    return L7_FAILURE;
  }

  /* EVC should be active */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u should be in use (intVlan=%u)",evc_id,intVlan);
    return L7_FAILURE;
  }

  /* Check if EVC is stacked, and return result */
  return ptin_evc_check_evctype(evcs[evc_id].extended_id, evc_type);
}

/**
 * Check if a specific internal VLAN is a QUATTRO service.
 *  
 * @param intVlan    : Internal outer-vlan 
 * 
 * @return L7_BOOL: L7_TRUE or L7_FALSE
 */
L7_BOOL ptin_evc_is_quattro_fromIntVlan(L7_uint16 intVlan)
{
  L7_uint8 evc_type;

  /* Get evc type */
  if (ptin_evc_check_evctype_fromIntVlan(intVlan, &evc_type) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  /* Only return TRUE is is QUATTRO */
  return (evc_type == PTIN_EVC_TYPE_QUATTRO_STACKED || evc_type == PTIN_EVC_TYPE_QUATTRO_UNSTACKED);
}

/**
 * Check if the EVC related to an internal vlan is stacked. 
 *  
 * @param intVlan    : Internal outer-vlan 
 * @param is_stacked : Is EVC stacked? (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_check_is_stacked_fromIntVlan(L7_uint16 intVlan, L7_BOOL *is_stacked)
{
  L7_uint evc_id;

  /* Validate arguments */
  if (intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[intVlan];
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",intVlan);
    return L7_FAILURE;
  }

  /* EVC should be active */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u should be in use (intVlan=%u)",evc_id,intVlan);
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
 * @param ptin_port: source interface number
 * @param intVlan  : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS: Parameters are valid
 *                   L7_FAILURE: Not valid
 */
L7_RC_t ptin_evc_intfVlan_validate(L7_uint32 ptin_port, L7_uint16 intVlan)
{
  L7_uint   evc_id;

  /* Validate arguments */
  if (intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[intVlan];
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",intVlan);
    return L7_FAILURE;
  }

  /* EVC should be active */
  if (!evcs[evc_id].in_use)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u should be in use (intVlan=%u)",evc_id,intVlan);
    return L7_FAILURE;
  }

  /* Validate ptin_port */
  if ( ptin_port >= PTIN_SYSTEM_N_INTERF )
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }

  /* Check if port is in use */
  if ( !evcs[evc_id].intf[ptin_port].in_use )
  {
    //PT_LOG_ERR(LOG_CTX_EVC,"ptin_port %u not used in evc %u",ptin_port, evc_id);
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
  L7_uint   evc_id, evc_ext_id;
  L7_int    intf2cfg[PTIN_SYSTEM_N_INTERF]; /* Lookup array to map sequential to indexed intf */
  L7_uint8  evc_type;
  L7_BOOL   is_p2p, is_quattro, is_stacked;
  L7_BOOL   maclearning;
  L7_BOOL   dhcpv4_enabled, dhcpv6_enabled, igmp_enabled, pppoe_enabled, iptv_enabled;
  L7_BOOL   cpu_trap;
  L7_BOOL   new_evc = L7_FALSE;
  L7_uint   n_roots;
  L7_uint   n_leafs;
  L7_int    root_port1, root_port2, leaf_port1;
  L7_int    p2p_port1, p2p_port2;
  L7_uint16 root_vlan;
  L7_uint   ptin_port;
  L7_uint32 intIfNum;
  L7_int    multicast_group = -1;
  dl_queue_t *freeVlan_queue = L7_NULLPTR;
  L7_RC_t   rc, error = L7_SUCCESS;
  ptin_evc_queue_type_t  queue_type= PTIN_EVC_QUEUE_PORT;
#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
  L7_BOOL is_logged= L7_FALSE;
#endif

  evc_ext_id = evcConf->index;

  /* Validate extended EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC id %u is out of range [0..%lu[", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS);
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


  /* Expand interface formats */
  for (i=0; i<evcConf->n_intf; i++)
  {
    /* Convert interface to all formats */
    if (ptin_intf_any_format(&evcConf->intf[i].intf) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Invalid interfaces");
      return L7_FAILURE;
    }
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
    ptin_port = evcConf->intf[i].intf.value.ptin_port;

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

#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
      /* MAC bridge service have a specific fp rule to direct traffic to wired or wireless*/
      if (iptv_enabled || evc_type == PTIN_EVC_TYPE_STD_P2P)
      {
         /* GPON ports use wireless queue*/
         if (ptin_port < PTIN_SYSTEM_N_PONS_PHYSICAL && queue_type != PTIN_EVC_QUEUE_WIRELESS)
         {
           queue_type = PTIN_EVC_QUEUE_WIRED;
           if (!is_logged)
           {
             PT_LOG_INFO(LOG_CTX_EVC, "eEVC# %u queue type is %s", evc_ext_id, "WIRED");
             is_logged = L7_TRUE;
           }
         }
         /* XGS ports use wired queue*/
         else if (queue_type != PTIN_EVC_QUEUE_WIRED)
         {
           queue_type = PTIN_EVC_QUEUE_WIRELESS;
           if (!is_logged)
           {
             PT_LOG_INFO(LOG_CTX_EVC, "eEVC# %u queue type is %s", evc_ext_id, "WIRELESS");
             is_logged = L7_TRUE;
           }
         }
         else
         {
           PT_LOG_ERR(LOG_CTX_EVC, "On EVC is not supported mix of XGS and GPON ports! (ptin_port = %u)", ptin_port);
           return L7_FAILURE;
         }
      }
#endif
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
    else
    {
      PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u: Unstacked P2P EVCs must only two ports", evc_ext_id);
      return L7_FAILURE;
    }
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: Port1 = %d   Port2 = %d", evc_ext_id, p2p_port1, p2p_port2 );

  /* Check if phy ports are already assigned to LAGs */
  for (i=0; i<evcConf->n_intf; i++)
  {
    if (evcConf->intf[i].intf.port_type == PTIN_EVC_INTF_PHYSICAL)
    {
      if (dot3adAggGet(evcConf->intf[i].intf.value.intIfNum, &intIfNum) == L7_SUCCESS)
      {
        ptin_intf_intIfNum2port(intIfNum, 0/*Vlan*/, &ptin_port); /* FIXME TC16SXG */
        PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u: port# %u belongs to LAG# %u", evc_ext_id,
                   evcConf->intf[i].intf.value.ptin_port, ptin_port - PTIN_SYSTEM_N_PORTS);
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
      return rc;
    }

    PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: allocated new internal EVC id %u...", evc_ext_id, evc_id);

    /* Allocate queue of free vlans */
    if (evcConf->internal_vlan < PTIN_VLAN_MIN || evcConf->internal_vlan > PTIN_VLAN_MAX)
    {
      if (ptin_evc_freeVlanQueue_allocate(evc_id, evcConf->flags, &freeVlan_queue) != L7_SUCCESS) 
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error allocating free vlan queue", evc_id);
        ptin_evc_entry_free(evc_ext_id);
        return L7_FAILURE;
      }
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Internal VLAN %u provided", evc_id, evcConf->internal_vlan);
      /* Only allow a specific range */
      if (evcConf->internal_vlan < PTIN_RESERVED_VLAN_MIN || evcConf->internal_vlan > PTIN_RESERVED_VLAN_MAX)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Provided internal VLAN %u should be in the range %u-%u",
                   evc_id, evcConf->internal_vlan, PTIN_RESERVED_VLAN_MIN, PTIN_RESERVED_VLAN_MAX);
        return L7_FAILURE;
      }
      /* Assume root vlan */
      root_vlan = evcConf->internal_vlan;
      /* No queue to be used */
      freeVlan_queue = L7_NULLPTR;
    }
    
    /* Check if there are enough internal VLANs on the pool
     *  P2P:  only one internal VLAN is needed (shared among all the ports)
     *  P2MP: one VLAN is needed per leaf port plus one for all the root ports */
    if (freeVlan_queue != L7_NULLPTR)
    {
      if ( (!(evcConf->flags & PTIN_EVC_MASK_ETREE) && (freeVlan_queue->n_elems < 1)) ||
           ( (evcConf->flags & PTIN_EVC_MASK_ETREE) && (freeVlan_queue->n_elems < (n_leafs + 1))) )
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: not enough internal VLANs available", evc_id);
        ptin_evc_freeVlanQueue_free(freeVlan_queue);
        ptin_evc_entry_free(evc_ext_id);
        return L7_FAILURE;
      }
    }
    else
    {
      /* Assume given VLAN as root VLAN */
      root_vlan = evcConf->internal_vlan;
    }

    rc = ptin_evc_vlan_allocate(&root_vlan, freeVlan_queue, evc_id); 
    /* Get a VLAN from the pool to use as Internal Root VLAN */
    if ( rc != L7_SUCCESS)  /* cannot fail! */
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error allocating internal VLAN", evc_id);
      ptin_evc_freeVlanQueue_free(freeVlan_queue);
      ptin_evc_entry_free(evc_ext_id);
      return rc;
    }

    PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: Enabling cross-connects?", evc_ext_id);

    /* For stacked EVCs, we need to enable forwarding mode to OVID(+IVID) */
    ptin_crossconnect_enable(root_vlan, (evc_type==PTIN_EVC_TYPE_STD_P2P && !cpu_trap) /* Bitstream services */, is_stacked);

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    if (is_quattro)          
    {
      /* Create Multicast group for L2intf's */
      rc = ptin_multicast_group_vlan_create(&multicast_group);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error creating multicast group", evc_id);
        error = rc;
      }
      else
      {
        PT_LOG_INFO(LOG_CTX_EVC, "EVC# %u: Multicast group %u created", evc_id, multicast_group);
      }

      if (error == L7_SUCCESS)
      {
        /* Virtual ports: Configure multicast group for the new leaf vlan */
        rc = ptin_vlanBridge_multicast_set(root_vlan, multicast_group);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error configuring Multicast replication for VLAN %u", evc_id, root_vlan);
          error = rc;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Multicast group %u associated to vlan %u", evc_id, multicast_group, root_vlan);
        }
      }
    }    
    else
#endif
    {
      #if 0
      if (iptv_enabled)
      {
        if (ptin_multicast_group_l3_create(&multicast_group)!=L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error creating multicast group", evc_id);
          error = L7_TRUE;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Multicast group 0x%08x created", evc_id, multicast_group);
        }
      }
      #endif
    }

    /* If no error, proceed to configure each interface */
    if (error == L7_SUCCESS)
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
      evcs[evc_id].mcgroup          = multicast_group;
      evcs[evc_id].queue_type       = queue_type;   

      PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: Adding interfaces", evc_ext_id);

      /* Configure each interface */
      for (i=0; i<evcConf->n_intf; i++)
      {
        ptin_port = evcConf->intf[i].intf.value.ptin_port;

        /* Apply config */
        rc = ptin_evc_intf_add(evc_id, &evcConf->intf[i]);
        if (rc != L7_SUCCESS)
        {
          error = rc;
          break;
        }

        /* On Unstacked EVCs, a "bridge" needs to be established between each leaf and all root interfaces */
        if ((evcConf->flags & PTIN_EVC_MASK_ETREE))
        {
          rc = ptin_evc_etree_intf_add(evc_id, ptin_port);
          if (rc != L7_SUCCESS)
          {
            error = rc;
            break;
          }
        }
      }

      /* For EVCs point-to-point unstacked, create now the crossconnection */
      if ((evc_type==PTIN_EVC_TYPE_STD_P2P && !cpu_trap) /* Bitstream */ && !is_stacked)
      {
        PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: Configuring P2P unstacked bridge", evc_ext_id);

        /* Add bridge between root and leaf port (Proot, Vr, Pleaf, Vs', Vc) */
        rc = switching_p2p_bridge_add(p2p_port1, evcs[evc_id].intf[p2p_port1].int_vlan,
                                      p2p_port2, evcs[evc_id].intf[p2p_port2].int_vlan,
                                      0 /* No inner vlan */);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error adding single vlanbridge between port %u / vlan %u <=> port %u / vlan %u", evc_id,
                  p2p_port1, evcs[evc_id].intf[p2p_port1].int_vlan,
                  p2p_port2, evcs[evc_id].intf[p2p_port2].int_vlan);
          error = rc;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Single vlanbridge added between port %u / vlan %u <=> port %u / vlan %u", evc_id,
                    p2p_port1, evcs[evc_id].intf[p2p_port1].int_vlan,
                    p2p_port2, evcs[evc_id].intf[p2p_port2].int_vlan);
        }
      }
    }

    /* Successfull creation */
    if (error == L7_SUCCESS)
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
        intf2cfg[evcConf->intf[i].intf.value.ptin_port] = i;
      }

      /* Check if ports are added or removed */
      error = L7_SUCCESS;
      for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
      {
        /* Port added ? */
        if ((evcs[evc_id].intf[i].in_use == 0) && (intf2cfg[i] >= 0))
        {
          PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: adding interface# %u...", evc_id, i);

          rc = ptin_evc_intf_add(evc_id, &evcConf->intf[intf2cfg[i]]);
          if (rc != L7_SUCCESS)
          {
            /* Signal error, but try to process the rest of the config */
            error = rc;
            continue;
          }

          /* NOTE: in unstacked EVCs, a bridge needs to be added between each leaf
           * and all the root interfaces */
          if ((evcConf->flags & PTIN_EVC_MASK_ETREE))
          {
            rc = ptin_evc_etree_intf_add(evc_id, i);
            if (rc != L7_SUCCESS)
            {
              /* Signal error, but try to process the rest of the config */
              error = rc;
              continue;
            }
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
            {
              if (evcs[evc_id].intf[i].bwprofile[j]!= NULL)
              {
                goto _ptin_evc_create1;
              }
            }
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
            error = L7_FAILURE;
            continue;
          }

          /* If it is an unstacked EVC, we need to remove the bridge before removing the interface */
          if ((evcConf->flags & PTIN_EVC_MASK_ETREE))
          {
            rc = ptin_evc_etree_intf_remove(evc_id, i);
            if (rc != L7_SUCCESS)
            {
              /* Signal error, but try to process the rest of the config */
              error = rc;
              continue;
            }
          }

          rc = ptin_evc_intf_remove(evc_id, i);
          if (rc != L7_SUCCESS)
          {
            /* Signal error, but try to process the rest of the config */
            error = rc;
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
              error = L7_FAILURE;
            }
          }
          else if (IS_VLAN_VALID(evcConf->intf[intf2cfg[i]].vid) != IS_VLAN_VALID(evcs[evc_id].intf[i].out_vlan))
          {
            PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: One of the Outer VLANs of existent port %u is not defined (%u vs %u)", evc_id, i,
                    evcConf->intf[intf2cfg[i]].vid, evcs[evc_id].intf[i].out_vlan);
            error = L7_FAILURE;
          }

          /* Check inner vlan */
          if (IS_VLAN_VALID(evcConf->intf[intf2cfg[i]].vid_inner) && IS_VLAN_VALID(evcs[evc_id].intf[i].inner_vlan))
          {
            if (evcConf->intf[intf2cfg[i]].vid_inner != evcs[evc_id].intf[i].inner_vlan)
            {
              PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Inner VLAN of existent port %u do not match (%u vs %u)", evc_id, i,
                      evcConf->intf[intf2cfg[i]].vid_inner, evcs[evc_id].intf[i].inner_vlan);
              error = L7_FAILURE;
            }
          }
          else if (IS_VLAN_VALID(evcConf->intf[intf2cfg[i]].vid_inner) != IS_VLAN_VALID(evcs[evc_id].intf[i].inner_vlan))
          {
            PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: One of the Inner VLANs of existent port %u is not defined (%u vs %u)", evc_id, i,
                    evcConf->intf[intf2cfg[i]].vid_inner, evcs[evc_id].intf[i].inner_vlan);
            error = L7_FAILURE;
          }
        }
      }
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Unstacked point-to-point EVC... no change allowed", evc_id);
      error = L7_FAILURE;
    }
  }

  /* If no error, update some EVC data */
  if (error == L7_SUCCESS)
  {
    ptin_HwEthMef10EvcOptions_t evcOptions;

    memset(&evcOptions, 0x00, sizeof(evcOptions));

    evcOptions.mask        = PTIN_EVC_OPTIONS_MASK_FLAGS | PTIN_EVC_OPTIONS_MASK_MCFLOOD;
    evcOptions.mc_flood    = evcConf->mc_flood;
    evcOptions.flags.value = evcConf->flags;
    evcOptions.flags.mask  = PTIN_EVC_MASK_MACLEARNING | PTIN_EVC_MASK_CPU_TRAPPING |
                             PTIN_EVC_MASK_DHCPV4_PROTOCOL | PTIN_EVC_MASK_IGMP_PROTOCOL | PTIN_EVC_MASK_PPPOE_PROTOCOL | PTIN_EVC_MASK_DHCPV6_PROTOCOL;
    /* Apply options */
    rc = ptin_evc_config(evc_ext_id, &evcOptions, queue_type);

    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error configuring EVC options", evc_id);
      return rc;
    }
  }

  /* Error occured: Remove configurations if EVC is new */
  if (error != L7_SUCCESS && new_evc)
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

    /* If EVC is P2MP, remove specific translations */
    if ((evcConf->flags & PTIN_EVC_MASK_ETREE))
    {
      ptin_evc_etree_intf_remove_all(evc_id);
    }
    /* For unstacked P2P EVCs, remove single vlan cross-connection */
    else if (evc_type==PTIN_EVC_TYPE_STD_P2P && !cpu_trap /* Bitstream */ && !is_stacked)
    {
      /* Add bridge between root and leaf port (Proot, Vr, Pleaf, Vs', Vc) */
      switching_p2p_bridge_remove(p2p_port1, evcs[evc_id].intf[p2p_port1].int_vlan,
                                  p2p_port2, evcs[evc_id].intf[p2p_port2].int_vlan,
                                  0 /* No inner vlan */);
    }
    
    /* Remove all previously configured interfaces */
    ptin_evc_intf_remove_all(evc_id);

    if ( ((iptv_enabled) 
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
          || (is_quattro)
#endif
          ) && evcs[evc_id].mcgroup > 0)
    {
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
      if (is_quattro)
      {
        /* Virtual ports: Configure multicast group for the vlan */
        if (ptin_vlanBridge_multicast_clear(root_vlan, evcs[evc_id].mcgroup)!=L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error configuring Multicast replication for VLAN %u (mcgroup=%u)", evc_id, root_vlan, evcs[evc_id].mcgroup);
          //return L7_FAILURE;
        }
        PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed multicast replication for vlan %u / group %d", evc_id, root_vlan, evcs[evc_id].mcgroup);
      }
#endif
      /*  Destroy Multicast group */
      if (ptin_multicast_group_destroy(evcs[evc_id].mcgroup)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error destroying Multicast group %u", evc_id, evcs[evc_id].mcgroup);
        //return L7_FAILURE;/*Operation still running*/
      }
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed multicast mc_group %d", evc_id, evcs[evc_id].mcgroup);
    }
    evcs[evc_id].mcgroup = -1; 

    ptin_evc_vlan_free(root_vlan, freeVlan_queue);
    ptin_evc_entry_free(evc_ext_id);
  }

  /* If error */
  if (error != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u creation failed: error=%d", evc_ext_id, error);
    return error;
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
  L7_RC_t rc;

  /* Validate arguments */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC %u is out of range [0,%lu[", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS);
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

  /* Expand port formats */
  if (ptin_intf_any_format(&evc_intf->intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid interface %u", evc_intf->intf.value.ptin_port);
    return L7_FAILURE;
  }
  /* ptin_port */
  ptin_port = evc_intf->intf.value.ptin_port;

  /* Check if port is already present */
  if (evcs[evc_idx].intf[ptin_port].in_use)
  {
    /* If configurations are different, return an error */
    if (evcs[evc_idx].intf[ptin_port].type      != evc_intf->mef_type ||
        evcs[evc_idx].intf[ptin_port].out_vlan  != evc_intf->vid      ||
        evcs[evc_idx].intf[ptin_port].inner_vlan!= evc_intf->vid_inner )
    {
      PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC %u: Configuration mismatch for port %u", evc_ext_id, evc_idx, ptin_port);
      return L7_FAILURE;
    }
    /* Equal configuration... nothing to be done! */
    return L7_SUCCESS;
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u / EVC %u: Adding port %u...", evc_ext_id, evc_idx, ptin_port);

  /* Add port to EVC */
  rc = ptin_evc_intf_add(evc_idx, evc_intf);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC %u: Error adding port %u", evc_ext_id, evc_idx, ptin_port);
    return rc;
  }

  /* NOTE: in unstacked EVCs, a bridge needs to be added between each leaf
   * and all the root interfaces */
  if (IS_EVC_ETREE(evc_idx))
  {
    rc = ptin_evc_etree_intf_add(evc_idx, ptin_port);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC %u: Error adding port %u for multipoint EVC", evc_ext_id, evc_idx, ptin_port);
      return rc;
    }
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u / EVC %u: Added port %u!", evc_ext_id, evc_idx, ptin_port);

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
  L7_uint     evc_idx;
  L7_uint     ptin_port;
  L7_RC_t     rc;

  /* Validate arguments */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC %u is out of range [0,%lu[", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS);
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
    return L7_DEPENDENCY_NOT_MET;
  }

  /* For unstacked P2P services, don't allow ports change */
  if (IS_EVC_P2P(evc_idx) && !IS_EVC_STACKED(evc_idx))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC %u: Unstacked point-to-point EVC... no change allowed", evc_ext_id, evc_idx);
    return L7_FAILURE;
  }

  /* Expand port formats */
  if (ptin_intf_any_format(&evc_intf->intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid interface %u", evc_intf->intf.value.ptin_port);
    return L7_SUCCESS; // Do not return error to the manager
  }
  /* ptin_port */
  ptin_port = evc_intf->intf.value.ptin_port;

  /* Check if port is not present */
  if (!evcs[evc_idx].intf[ptin_port].in_use)
  {
    /* Nothing to be done! */
    return L7_SUCCESS;
  }
  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u / EVC %u: Removing port %u...", evc_ext_id, evc_idx, ptin_port);

  /* Remove associated resources */
  /* Clean service resources */
  rc = ptin_evc_intf_clean(evc_idx, evc_intf->intf.value.ptin_intf.intf_type, evc_intf->intf.value.ptin_intf.intf_id, L7_TRUE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error cleaning service profiles and counters!!!", evc_idx);
    return rc;
  }

  /* Only stacked services have clients */
  if (IS_EVC_STD(evc_idx) && !IS_EVC_STACKED(evc_idx))
  {
    #if ( !PTIN_BOARD_IS_MATRIX )
    /* IGMP management */
    if (IS_EVC_IGMP(evc_idx))
    {
      ptin_client_id_t clientId;

      /* Client id */
      memset(&clientId, 0x00, sizeof(clientId));
      clientId.ptin_intf.intf_type  = evc_intf->intf.value.ptin_intf.intf_type;
      clientId.ptin_intf.intf_id    = evc_intf->intf.value.ptin_intf.intf_id;
      clientId.ptin_port            = ptintf2port(clientId.ptin_intf.intf_type,
                                                  clientId.ptin_intf.intf_id); 
      clientId.outerVlan            = evcs[evc_idx].intf[ptin_port].int_vlan;
      #if 0
      clientId.innerVlan            = evcs[evc_idx].intf[ptin_port].inner_vlan;
      #else /*Modified Client Remove Api to Suppor Removing Unicast Unstacked Services*/
      clientId.innerVlan            = (L7_uint16) -1;
      #endif
      clientId.mask                 = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_OUTERVLAN | PTIN_CLIENT_MASK_FIELD_INNERVLAN;    

      /* Remove client */
      if ( (rc = ptin_igmp_api_client_remove(&clientId)) != L7_SUCCESS)
      {
        /*This is not an error if this routine is invoked after a reset defaults message*/
        if (rc == L7_NOT_EXIST)
        {
          /* L7_NOT_EXIST is not an error */
          PT_LOG_NOTICE(LOG_CTX_EVC, "EVC# %u: Client does not exist on IGMP instance (intf_type:%u/intf_id:%u outerVlan:%u/innerVlan:%u)", 
                  evc_ext_id, clientId.ptin_intf.intf_type, clientId.ptin_intf.intf_id, clientId.outerVlan, clientId.innerVlan);       
        }
        else
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing client from IGMP instance (intf_type:%u/intf_id:%u outerVlan:%u/innerVlan:%u) (rc:%u)", 
                 evc_ext_id, clientId.ptin_intf.intf_type, clientId.ptin_intf.intf_id, clientId.outerVlan, clientId.innerVlan, rc);
          //rc = L7_FAILURE;    
        }
        rc = L7_SUCCESS;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Client removed from IGMP instance (intf_type:%u/intf_id:%u outerVlan:%u/innerVlan:%u)", 
                  evc_ext_id, clientId.ptin_intf.intf_type, clientId.ptin_intf.intf_id, clientId.outerVlan, clientId.innerVlan);    
      }
    }
    #endif
  }
  else
  {
    /* Remove all clients/flows */
    rc = ptin_evc_intfclientsflows_remove(evc_idx, evc_intf->intf.value.ptin_intf.intf_type, evc_intf->intf.value.ptin_intf.intf_id);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing clients!!!", evc_idx);
      return rc;
    }
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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC# %u: Port %u has counter/BW profiles/Probes configured! Cannot remove it!",
            evc_ext_id, evc_idx, ptin_port);
    return L7_FAILURE;
  }
  /* If clients/flows are attched to this port, also cannot remove this port */
  if (evcs[evc_idx].intf[ptin_port].clients.n_elems > 0)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC# %u: Port %u still has clients/flows configured! Cannot remove it!",
            evc_ext_id, evc_idx, ptin_port);
    return L7_FAILURE;
  }

  /* If it is an unstacked EVC, we need to remove the bridge before removing the interface */
  if (IS_EVC_ETREE(evc_idx))
  {
    rc = ptin_evc_etree_intf_remove(evc_idx, ptin_port);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC# %u: Cannot remove multipoint port %u",
              evc_ext_id, evc_idx, ptin_port);
      return rc;
    }
  }
  /* Remove port */
  rc = ptin_evc_intf_remove(evc_idx, ptin_port);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u / EVC# %u: Cannot remove port %u",
            evc_ext_id, evc_idx, ptin_port);
    return rc;
  }

  /* If there is only 1 port, EVC will be destroyed, except with is root ports on Linecards
     of non P2MP (unicast and bitsteam unstacked).
     On SF doens't make sense to have a EVC without root ports */
#if (PTIN_BOARD_IS_GPON)
  if (((evcs[evc_idx].n_roots + evcs[evc_idx].n_leafs) < 1) || 
      ((evcs[evc_idx].n_leafs == 0) && (IS_EVC_STD_P2MP(evc_idx) == 0)))
#else
  if ((evcs[evc_idx].n_roots + evcs[evc_idx].n_leafs) <= 1)
#endif
  {
    ptin_evc_destroy(evc_ext_id);
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u / EVC %u: Removed port %u!", evc_ext_id, evc_idx, ptin_port);

  return L7_SUCCESS;
}

/**
 * EVC options reconfiguration
 * 
 * @param evc_ext_id : EVC extended id
 * @param evcOptions : EVC options
 * @param queue_type : queue type 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_config(L7_uint32 evc_ext_id, ptin_HwEthMef10EvcOptions_t *evcOptions, L7_int queue_type)
{
  L7_uint   evc_id;
  L7_uint8  mc_flood;
  L7_BOOL   maclearning, cpu_trap;
  L7_BOOL   dhcpv4_enabled, dhcpv6_enabled, igmp_enabled, pppoe_enabled;
  L7_RC_t   rc, error;

  error = rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC %u is out of range [0,%lu[", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS);
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
  if (switching_vlan_config(evcs[evc_id].rvlan, evcs[evc_id].rvlan, maclearning, mc_flood, cpu_trap, queue_type) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error configuring VLAN %u [FwdVlan=%u MACLearning=%u MCFlood=%u]",
            evc_id, evcs[evc_id].rvlan, evcs[evc_id].rvlan, maclearning, mc_flood);
    return L7_FAILURE;
  }

  /* MAC Learning, Multicast flood and CPU trap */
  evcs[evc_id].mc_flood = mc_flood;
  (cpu_trap   ) ? (evcs[evc_id].flags |= PTIN_EVC_MASK_CPU_TRAPPING) : (evcs[evc_id].flags &= ~((L7_uint32) PTIN_EVC_MASK_CPU_TRAPPING));
  (maclearning) ? (evcs[evc_id].flags |= PTIN_EVC_MASK_MACLEARNING ) : (evcs[evc_id].flags &= ~((L7_uint32) PTIN_EVC_MASK_MACLEARNING ));

  error = L7_SUCCESS;

  /* Protocol management */
  if (!IS_EVC_QUATTRO(evc_id))
  {
  #if (!PTIN_BOARD_IS_MATRIX)
    PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: Checking instances", evc_ext_id);
    /* DHCP configuration */
    rc = ptin_evc_update_dhcp(evc_id, &evcs[evc_id].flags, dhcpv4_enabled, dhcpv6_enabled,
                              L7_FALSE /*Update*/, L7_FALSE /*Do not look to counters*/);
    if (rc != L7_SUCCESS)
    {
      error = rc;
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error configuring DHCP", evc_id);
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: DHCP configured", evc_id);
    }

    /* PPPoE configuration */
    rc = ptin_evc_update_pppoe(evc_id, &evcs[evc_id].flags, pppoe_enabled,
                               L7_FALSE /*Update*/, L7_FALSE /*Do not look to counters*/);
    if (rc != L7_SUCCESS)
    {
      error = rc;
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error configuring PPPoE", evc_id);
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: PPPoE configured", evc_id);
    }
  #endif

    /* IGMP configuration */
  #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
    rc = ptin_evc_update_igmp(evc_id, &evcs[evc_id].flags, igmp_enabled,
                              L7_FALSE /*Update*/, L7_FALSE /*Do not look to counters*/);
    if (rc != L7_SUCCESS)
    {
      error = rc;
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error configuring IGMP", evc_id);
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: IGMP configured", evc_id);
    }
  #endif
  }

  return error;
}

/**
 * Remove QoS-Service maps related to a NNI/Internal VLAN
 * 
 * @param nni_vlan 
 * @param int_vlan 
 */
static void ptin_evc_qos_vlan_clear(L7_uint16 nni_vlan, L7_uint16 int_vlan)
{
  ptin_qos_vlan_t qos_apply;
  L7_BOOL clear_qos_vlan_map = L7_FALSE;

  memset(&qos_apply, 0x00, sizeof(ptin_qos_vlan_t));
  qos_apply.leaf_side  = -1;  /* All sides */
  qos_apply.trust_mode = 0;   /* Ignore */

  /* NNI VLAN is valid, use it, instead of the internal VLAN */
  if (nni_vlan >= 1 && nni_vlan <= 4095)
  {
    qos_apply.nni_vlan = nni_vlan;

    /* Check if this NNI-VLAN still exists */
    if (ptin_evc_get_evcId_fromNNIvlan(nni_vlan, L7_NULLPTR, L7_NULLPTR) == L7_NOT_EXIST)
    {
      /* If not, clear map */
      clear_qos_vlan_map = L7_TRUE;
      PT_LOG_DEBUG(LOG_CTX_MSG, "Going to unconfigure QoS for NNI-Vlan %u", nni_vlan);
    }
    else
    {
      /* Otherwise, do not remove it */
      PT_LOG_DEBUG(LOG_CTX_MSG, "There is still instances using NNI-VLAN %u", nni_vlan);
    }
  }
  else
  {
    /* Always clear map for non quattro-stacked services */
    clear_qos_vlan_map = L7_TRUE;

    qos_apply.int_vlan = int_vlan;
    PT_LOG_DEBUG(LOG_CTX_MSG, "Going to unconfigure QoS for intVlan %u", int_vlan);
  }

  /* If map is to be removed, remove it */
  if (clear_qos_vlan_map)
  {
    if (ptin_qos_vlan_clear(&qos_apply) != L7_SUCCESS) 
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error deconfiguring QoS for intVlan %u / NNI-VLAN %u", int_vlan, nni_vlan);
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Success deconfiguring QoS for intVlan %u / NNI-VLAN %u", int_vlan, nni_vlan);
    }
  }
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
  L7_uint   evc_id, i;
  L7_uint16 int_vlan, nni_vlan;
  L7_RC_t   rc;

  PT_LOG_TRACE(LOG_CTX_EVC, "Deleting eEVC# %u...", evc_ext_id);

  /* Validate eEVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu[", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS);
    return L7_FAILURE;
  }

  /* Convert to internal evc id */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC %u not existent", evc_ext_id);
    return L7_SUCCESS;
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u is mapped to internal id %u", evc_ext_id, evc_id);

  /* Save Internal and NNI VLANs */
  int_vlan = evcs[evc_id].rvlan;
  nni_vlan = (IS_EVC_QUATTRO(evc_id) && IS_EVC_STACKED(evc_id)) ? evcs[evc_id].root_info.nni_ovid : 0;

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
  {
    ptin_igmp_evc_remove(evc_ext_id);
  }

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
  if (IS_EVC_IGMP(evc_id))
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
  rc = ptin_evc_allintfs_clean(evc_id, L7_TRUE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error cleaning service profiles and counters! EVC cannot be removed!",
            evc_id);
    return rc;
  }
  /* Clean any clients/flows that may exist */
  rc = ptin_evc_allclientsflows_remove(evc_id);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing all clients! EVC cannot be removed!",
            evc_id);
    return rc;
  }

  /* Remove bridges on N:1 EVCs */
  if (IS_EVC_ETREE(evc_id))
  {
    rc = ptin_evc_etree_intf_remove_all(evc_id);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing (unstacked) bridges config", evc_id);
      return rc;
    }
  }
  /* For unstacked 1:1 EVCs, remove single vlan cross-connection */
  else if (IS_EVC_BITSTREAM(evc_id) && !IS_EVC_STACKED(evc_id))
  {
    L7_int port1 = evcs[evc_id].p2p_port1_intf;
    L7_int port2 = evcs[evc_id].p2p_port2_intf;

    /* Add bridge between root and leaf port (Proot, Vr, Pleaf, Vs', Vc) */
    rc = switching_p2p_bridge_remove(port1, evcs[evc_id].intf[port2].int_vlan,
                                     port2, evcs[evc_id].intf[port2].int_vlan,
                                     0 /* No inner vlan */);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing single vlanbridge between port %u / vlan %u <=> port %u / vlan %u", evc_id,
              port1, evcs[evc_id].intf[port1].int_vlan,
              port2, evcs[evc_id].intf[port2].int_vlan);
      return rc;
    }
  }
#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
  else if (IS_EVC_BITSTREAM(evc_id) && IS_EVC_STACKED(evc_id)) 
  {
    rc = ptin_pause_frames_drop_cancel(int_vlan, FALSE);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error removing rule to cancel drop of pause frames on VLAN %d rc %d",
              int_vlan, rc);
      return rc;
    }
    
    PT_LOG_INFO(LOG_CTX_EVC, "Frame pause frame drop cancel removed: rc=%d", rc);
  }
#endif 
  /* Remove all configured interfaces */
  rc = ptin_evc_intf_remove_all(evc_id);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing interfaces config", evc_id);
    return rc;
  }

  if ( ((evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV)
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED                    
                    || (IS_EVC_QUATTRO(evc_id)) 
#endif
     ) && evcs[evc_id].mcgroup > 0)
  { 
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    if (IS_EVC_QUATTRO(evc_id))
    {
      /* Virtual ports: Configure multicast group for the vlan */
      rc = ptin_vlanBridge_multicast_clear(evcs[evc_id].rvlan, evcs[evc_id].mcgroup);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing Multicast replication for VLAN %u (mc_group=0x%08x)", evc_id, evcs[evc_id].rvlan, evcs[evc_id].mcgroup);
        //return rc;
      }
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed multicast replication for vlan %u / mc_group 0x%08x", evc_id, evcs[evc_id].rvlan, evcs[evc_id].mcgroup);
    }       
#endif
    /*  Destroy Multicast group */
    rc = ptin_multicast_group_destroy(evcs[evc_id].mcgroup);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error destroying mc_group 0x%08x", evc_id, evcs[evc_id].mcgroup);
      //return rc;/*Operation still running*/
    }
    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed mc_group 0x%08x", evc_id, evcs[evc_id].mcgroup);

    /* Remove all configured interfaces */
    for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
    {
      if (!evcs[evc_id].intf[i].in_use)
      continue;

      ptin_igmp_snooping_channel_reset(evcs[evc_id].rvlan, i);

      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Succefully channel snooping reset 0x%08x", evc_id, evcs[evc_id].mcgroup);
    }

  }  
  evcs[evc_id].mcgroup = -1;

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  /* Update number of QUATTRO-P2P evcs */
  DECREMENT_QUATTRO_INSTANCE(evc_id, n_quattro_evcs);
#endif

  ptin_evc_vlan_free(evcs[evc_id].rvlan, evcs[evc_id].queue_free_vlans);

  ptin_evc_entry_free(evc_ext_id);

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u successfully removed (internal id %u)", evc_ext_id, evc_id);

  /* Clean QoS-VLAN entries */
  ptin_evc_qos_vlan_clear(nni_vlan, int_vlan);  

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
  L7_uint     evc_id, selected_evc_id;
  ptin_intf_t ptin_intf;
  L7_uint16   int_vlan, nni_vlan;
  RC_t        rc;

  PT_LOG_TRACE(LOG_CTX_EVC, "Destroying eEVC# %u...", evc_ext_id);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (evc_ext_id != (L7_uint32)-1 && evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu[", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS);
    return L7_FAILURE;
  }

  selected_evc_id = (L7_uint) -1;

  /* GEt internal EVC id */
  if (evc_ext_id != (L7_uint32)-1)
  {
    /* Convert to internal evc id */
    if (ptin_evc_ext2int(evc_ext_id, &selected_evc_id) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "eEVC %u not existent", evc_ext_id);
      return L7_SUCCESS;
    }
    PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u is mapped to internal id %u", evc_ext_id, selected_evc_id);
  }

  /* Run all (internal) EVCs */
  for (evc_id = 0; evc_id < PTIN_SYSTEM_N_EVCS; evc_id++)
  {
    /* Skip non selected EVCs */
    if (selected_evc_id != (L7_uint)-1 && selected_evc_id != evc_id)
      continue;
    
    /* Skip Not active EVCs */
    if (!evcs[evc_id].in_use)
      continue;

    /* If a destroy all is requested, skip reserved EVCs */
    if ((selected_evc_id == (L7_uint)-1) &&
        (evcs[evc_id].extended_id == PTIN_EVC_INBAND || evcs[evc_id].extended_id >= PTIN_SYSTEM_EXT_EVCS_MGMT))
    {
      continue;
    }
    
    evc_ext_id = evcs[evc_id].extended_id;

    /* Save Internal and NNI VLANs */
    int_vlan = evcs[evc_id].rvlan;
    nni_vlan = (IS_EVC_QUATTRO(evc_id) && IS_EVC_STACKED(evc_id)) ? evcs[evc_id].root_info.nni_ovid : 0;

    /* IF this EVC belongs to an IGMP instance, destroy that instance */
    if (ptin_igmp_is_evc_used(evc_ext_id))
    {
      ptin_igmp_evc_remove(evc_ext_id);
    }

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
      rc = ptin_igmp_evc_configure(evc_ext_id, L7_FALSE,
                                   (!(evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV) && SINGLE_INSTANCE(evc_id, n_quattro_igmp_evcs)));
      if (rc != L7_SUCCESS)
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

      if (ptin_intf_port2ptintf(intf_idx, &ptin_intf)!=L7_SUCCESS)
        continue;

      /* Only clean service resources... Clients remotion will do the rest */
      rc = ptin_evc_intf_clean(evc_id, ptin_intf.intf_type, ptin_intf.intf_id, L7_TRUE);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error cleaning service profiles and counters!!!", evc_id);
        return rc;
      }

      if (IS_EVC_STD(evc_id) && !IS_EVC_STACKED(evc_id))
        continue;

      /* Remove all clients/flows */
      rc = ptin_evc_intfclientsflows_remove(evc_id, ptin_intf.intf_type, ptin_intf.intf_id);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing clients!!!", evc_id);
        return rc;
      }
    }

    /* Check if there are clients pending... */
    if (evcs[evc_id].n_clientflows > 0)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: %u clients are still configured! EVC cannot be destroyed!",
              evc_id, evcs[evc_id].n_clientflows);
      return L7_FAILURE;
    }

    /* Remove bridges on N:1 EVCs */
    if (IS_EVC_ETREE(evc_id))
    {
      rc = ptin_evc_etree_intf_remove_all(evc_id);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing (unstacked) bridges config", evc_id);
        return rc;
      }
    }
    /* For unstacked 1:1 EVCs, remove single vlan cross-connection */
    else if (IS_EVC_BITSTREAM(evc_id) && !IS_EVC_STACKED(evc_id))
    {
      L7_int port1 = evcs[evc_id].p2p_port1_intf;
      L7_int port2 = evcs[evc_id].p2p_port2_intf;

      /* Add bridge between root and leaf port (Proot, Vr, Pleaf, Vs', Vc) */
      rc = switching_p2p_bridge_remove(port1, evcs[evc_id].intf[port2].int_vlan,
                                       port2, evcs[evc_id].intf[port2].int_vlan,
                                       0 /* No inner vlan */);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing single vlanbridge between port %u / vlan %u <=> port %u / vlan %u", evc_id,
                port1, evcs[evc_id].intf[port1].int_vlan,
                port2, evcs[evc_id].intf[port2].int_vlan);
        return rc;
      }
    }

    /* Remove all previously configured interfaces */
    rc = ptin_evc_intf_remove_all(evc_id);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing interfaces config", evc_id);
      return rc;
    }

    if ( ((evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV)
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED                    
          || (IS_EVC_QUATTRO(evc_id)) 
#endif
         ) &&  evcs[evc_id].mcgroup > 0)
    {
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
      if (IS_EVC_QUATTRO(evc_id))
      {
        /* Virtual ports: Configure multicast group for the vlan */
        rc = ptin_vlanBridge_multicast_clear(evcs[evc_id].rvlan, evcs[evc_id].mcgroup);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing Multicast replication for VLAN %u (mc_group=0x%08x)", evc_id, evcs[evc_id].rvlan, evcs[evc_id].mcgroup);
          //return rc;
        }
        PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed multicast replication for vlan %u / mc_group 0x%08x", evc_id, evcs[evc_id].rvlan, evcs[evc_id].mcgroup);
      }      
#endif

      /* Destroy Multicast group */
      rc = ptin_multicast_group_destroy(evcs[evc_id].mcgroup);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error destroying mc_group 0x%08x", evc_id, evcs[evc_id].mcgroup);
        //return rc;/*Operation still running*/
      }
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Removed mc_group 0x%08x", evc_id, evcs[evc_id].mcgroup);
    }  
    evcs[evc_id].mcgroup = -1;

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    /* Update number of QUATTRO-P2P evcs */
    DECREMENT_QUATTRO_INSTANCE(evc_id, n_quattro_evcs);
#endif

    ptin_evc_vlan_free(evcs[evc_id].rvlan, evcs[evc_id].queue_free_vlans);
    ptin_evc_entry_free(evc_ext_id);

    PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u successfully destroyed (internal id %u)", evc_ext_id, evc_id);

    /* Clean QoS-VLAN entries */
    ptin_evc_qos_vlan_clear(nni_vlan, int_vlan);  
  }

  return L7_SUCCESS;
}


/**
 * Destroys all EVCs (except INBAND!)
 * 
 * @return L7_RC_t L7_SUCCESS
 */
L7_RC_t ptin_evc_destroy_all(void)
{
  /* Destroy all active EVCs, except reserved ones */
  ptin_evc_destroy((L7_uint32) -1);

  return L7_SUCCESS;
}


/**
 * Replicate bridges from one port to other port 
 * 
 * @author Rui Fernandes: rui-f-fernandes@telecom.pt 
 * @return L7_RC_t L7_SUCCESS
 */
L7_RC_t ptin_evc_p2p_bridge_replicate(L7_uint32 evc_ext_id, L7_uint32 ptin_port, L7_uint32 ptin_port_ngpon2, ptin_HwEthMef10Intf_t *intf)
{
  L7_uint32 evc_id;
  ptin_HwEthEvcBridge_t evcBridge;
  struct ptin_evc_client_s *pclientFlow;
  int j;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_FAILURE;
  }

  evcBridge.index = evc_ext_id;
  
  /* Validate leaf interface (from received message) */
  if ((ptin_port >= PTIN_SYSTEM_N_INTERF) ||
      (!evcs[evc_id].intf[ptin_port].in_use) ||
      (evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF))
  {
    return L7_FAILURE;
  }

  dl_queue_get_head(&evcs[evc_id].intf[ptin_port_ngpon2].clients, (dl_queue_elem_t **) &pclientFlow);

  for (j=0; j < evcs[evc_id].intf[ptin_port_ngpon2].clients.n_elems && pclientFlow != L7_NULLPTR; j++) 
  {

    evcBridge.inn_vlan = pclientFlow->client_vid;

    evcBridge.intf.action_inner = PTIN_XLATE_ACTION_ADD;
    evcBridge.intf.action_outer = PTIN_XLATE_ACTION_REPLACE;
    evcBridge.intf.evcId        = intf->evcId;
    evcBridge.intf.mef_type     = PTIN_EVC_INTF_LEAF;
    evcBridge.intf.vid          = pclientFlow->uni_ovid; 
    evcBridge.intf.vid_inner    = pclientFlow->int_ivid;
    evcBridge.intf.intf.value.ptin_intf.intf_id        = ptin_port;
    evcBridge.intf.intf.value.ptin_intf.intf_type      = PTIN_EVC_INTF_PHYSICAL;
    evcBridge.intf.intf.format  = PTIN_INTF_FORMAT_TYPEID;

    ptin_evc_p2p_bridge_add(&evcBridge);
   
    pclientFlow = (struct ptin_evc_client_s *) dl_queue_get_next(&evcs[evc_id].intf[ptin_port_ngpon2].clients, (dl_queue_elem_t *) pclientFlow);
  }
  if(evcs[evc_id].n_clientflows == 0)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "No client flow to replicate ");
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
  L7_uint evc_id, evc_ext_id;
  L7_uint root_intf;
  L7_uint leaf_intf;
  L7_uint i;
  ptin_HwEthMef10Intf_t intf_vlan;
  L7_RC_t rc = L7_SUCCESS;
  struct ptin_evc_client_s *pclient;

  evc_ext_id = evcBridge->index;

  PT_LOG_TRACE(LOG_CTX_EVC, "Adding eEVC# %u bridge connection...", evc_ext_id);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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

  /* Expand port formats */
  if (ptin_intf_any_format(&evcBridge->intf.intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid interface %u", evcBridge->intf.intf.value.ptin_port);
    return L7_FAILURE;
  }
  /* Determine leaf ptin_intf */
  leaf_intf = evcBridge->intf.intf.value.ptin_port;

  /* Validate leaf interface (from received message) */
  if ((leaf_intf >= PTIN_SYSTEM_N_INTERF) ||
      (!evcs[evc_id].intf[leaf_intf].in_use) ||
      (evcs[evc_id].intf[leaf_intf].type != PTIN_EVC_INTF_LEAF))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: port %u is invalid", evc_id, evcBridge->intf.intf.value.ptin_port);
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

  if (ptin_intf_portGem2virtualVid(leaf_intf, evcBridge->intf.vid, &evcBridge->intf.vid)!= L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error obtaining the virtual VID from GEM VID %u",
               evcBridge->intf.vid);
    return L7_FAILURE;
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "  New Client.OVlan = %u", evcBridge->intf.vid);
  }

  /* Check if client entry already exists */
  ptin_evc_find_client(evcBridge->inn_vlan, &evcs[evc_id].intf[leaf_intf].clients, (dl_queue_elem_t**) &pclient);
  if (pclient != NULL)
  {
    if (ptin_debug_evc)
      PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: port %u already have a bridge with Inner VID = %u", evc_id,
                  evcBridge->intf.intf.value.ptin_port, evcBridge->inn_vlan);
    return L7_SUCCESS;
  }

  /* Check if there is available clients */
  if (queue_free_clients.n_elems == 0)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: No available clients", evc_id);
    return L7_NO_RESOURCES;
  }

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: adding bridge [Root: Intf=%u IntVID=%u] <=> [Leaf: Intf=%u NEW Out.VID=%u Inn.VID=%u] ...", evc_id,
               root_intf, evcs[evc_id].rvlan, leaf_intf, evcBridge->intf.vid, evcBridge->inn_vlan);

  memset(&intf_vlan, 0x00, sizeof(intf_vlan));
  intf_vlan.intf        = evcBridge->intf.intf;
  intf_vlan.mef_type    = PTIN_EVC_INTF_LEAF;
  intf_vlan.vid         = evcBridge->intf.vid;
  intf_vlan.vid_inner   = evcBridge->inn_vlan;
  intf_vlan.action_outer= PTIN_XLATE_ACTION_REPLACE;  /* Ingress/egress: swap outer VLAN */
  intf_vlan.action_inner= PTIN_XLATE_ACTION_ADD;      /* Ingress: add VLAN / Egress: remove VLAN */

  /* Add translation rules */
  #if ( !PTIN_BOARD_IS_MATRIX )
  /* Also for P2MP this routine will be executed, as long as it is stacked */
  /* For these ones the internal vlan will be the interface internal one */
  if (IS_EVC_ETREE(evc_id))
  {
    rc = switching_etree_stacked_leaf_add(&intf_vlan, evcs[evc_id].intf[leaf_intf].int_vlan, evcs[evc_id].rvlan);
  }
  else
  {
    /* Remove inner vlan @ egress */

    rc = switching_elan_leaf_add(&intf_vlan, evcs[evc_id].rvlan, L7_TRUE, -1);
  }
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding translations for leaf interface %u (rc=%d)",leaf_intf, rc);
    return rc;
  }
  #endif

  /* Only make cross-connections, if EVC is stacked (1:1) */
  if (IS_EVC_BITSTREAM(evc_id))
  {
    /* Add bridge between root and leaf port (Proot, Vr, Pleaf, Vs', Vc) */
    rc = switching_p2p_bridge_add(root_intf, evcs[evc_id].rvlan, leaf_intf, evcBridge->intf.vid, evcBridge->inn_vlan);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error adding bridge", evc_id);
      return rc;
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
#if ( PTIN_BOARD == PTIN_BOARD_TC16SXG )
  /* On TC16SXG P2P have different xlate operations,
      due to the port virtualization and ASPEN limitations*/
  pclient->uni_ivid   = evcBridge->inn_vlan;
#else
  pclient->uni_ivid   = 0;
#endif
  pclient->action_outer_vlan = intf_vlan.action_outer;
  pclient->action_inner_vlan = intf_vlan.action_inner;
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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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

  /* Expand port formats */
  if (ptin_intf_any_format(&evcBridge->intf.intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid interface %u", evcBridge->intf.intf.value.ptin_port);
    return L7_FAILURE;
  }
  /* Determine leaf ptin_intf */
  leaf_intf = evcBridge->intf.intf.value.ptin_port;

  if ( !evcs[evc_id].intf[leaf_intf].in_use )
  {
    PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: port %u is not active in this EVC", evc_id, evcBridge->intf.intf.value.ptin_port);
    return L7_NOT_EXIST;
  }
  if ( evcs[evc_id].intf[leaf_intf].type != PTIN_EVC_INTF_LEAF )
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: port %u is not a leaf interface", evc_id, evcBridge->intf.intf.value.ptin_port);
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
    PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: port %u does not have any bridge with Inner VID = %u", evc_id,
                evcBridge->intf.intf.value.ptin_port, evcBridge->inn_vlan);
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
  rc = ptin_evc_pclientFlow_clean(evc_id, leaf_intf, pclient, L7_TRUE);
  if (rc != L7_SUCCESS )
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: can't remove profiles and counters to client!", evc_id);
    return rc;
  }

  #if ( !PTIN_BOARD_IS_MATRIX )
  /* Remove translations */
  if (IS_EVC_ETREE(evc_id))
  {
    rc = switching_etree_stacked_leaf_remove(leaf_intf, pclient->uni_ovid, pclient->int_ivid, evcs[evc_id].intf[leaf_intf].int_vlan, evcs[evc_id].rvlan);
  }
  else
  {
    rc = switching_elan_leaf_remove(leaf_intf, pclient->uni_ovid, pclient->int_ivid, evcs[evc_id].rvlan, L7_TRUE /*Delete inner vlan at egress*/);
  }
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error removing translations for leaf interface %u (rc=%d)",leaf_intf, rc);
    return rc;
  }
  #endif

  /* Only remove cross-connections, if EVC is stacked (1:1) */
  if (IS_EVC_BITSTREAM(evc_id))
  {
    /* Delete bridge between root and leaf port (Proot, Vr, Pleaf, Vs', Vc) */
    rc = switching_p2p_bridge_remove(root_intf, evcs[evc_id].rvlan, leaf_intf, pclient->uni_ovid, pclient->int_ivid);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error deleting bridge", evc_id);
      return rc;
    }
  }

  #if ( !PTIN_BOARD_IS_MATRIX )
  /* IGMP management */
  if (evcs[evc_id].flags & PTIN_EVC_MASK_IGMP_PROTOCOL)
  {
    ptin_intf_t      ptin_intf;
    ptin_client_id_t clientId;
    
    /* Convert to ptin_intf */
    if (ptin_intf_port2ptintf(leaf_intf, &ptin_intf)  != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Cannot get ptin_intf from port %u", evc_id, leaf_intf);
      return L7_FAILURE;
    }

    /* Client id */
    memset(&clientId, 0x00, sizeof(clientId));
    clientId.ptin_intf.intf_type  = ptin_intf.intf_type;
    clientId.ptin_intf.intf_id    = ptin_intf.intf_id;
    clientId.ptin_port            = ptintf2port(clientId.ptin_intf.intf_type,
                                                clientId.ptin_intf.intf_id); 
    clientId.outerVlan            = pclient->int_ovid;
    clientId.innerVlan            = pclient->int_ivid;
    clientId.mask                 = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_OUTERVLAN | PTIN_CLIENT_MASK_FIELD_INNERVLAN;    

    /* Remove client */
    if ( (rc = ptin_igmp_api_client_remove(&clientId)) != L7_SUCCESS)
    {
      /*This is not an error if this routine is invoked after a reset defaults message*/
      if (rc == L7_NOT_EXIST)
      {
        /* L7_NOT_EXIST is not an error */
       PT_LOG_NOTICE(LOG_CTX_EVC, "EVC# %u: Client does not exist on IGMP instance (intf_type:%u/intf_id:%u outerVlan:%u/innerVlan:%u)", 
                  evc_ext_id, clientId.ptin_intf.intf_type, clientId.ptin_intf.intf_id, clientId.outerVlan, clientId.innerVlan);       
      }
      else
      {
         PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing client from IGMP instance (intf_type:%u/intf_id:%u outerVlan:%u/innerVlan:%u) (rc:%u)", 
                 evc_ext_id, clientId.ptin_intf.intf_type, clientId.ptin_intf.intf_id, clientId.outerVlan, clientId.innerVlan, rc);
        //rc = L7_FAILURE;    
      }
      rc = L7_SUCCESS;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Client removed from IGMP instance (intf_type:%u/intf_id:%u outerVlan:%u/innerVlan:%u)", 
                  evc_ext_id, clientId.ptin_intf.intf_type, clientId.ptin_intf.intf_id, clientId.outerVlan, clientId.innerVlan);    
    }
  }
  #endif


  /* SEM CLIENTS UP */
  osapiSemaTake(ptin_evc_clients_sem, L7_WAIT_FOREVER);

  /* Delete client from the EVC struct */
  dl_queue_remove(&evcs[evc_id].intf[leaf_intf].clients, (dl_queue_elem_t*) pclient);
  memset(pclient, 0x00, sizeof(struct ptin_evc_client_s));
  dl_queue_add_tail(&queue_free_clients, (dl_queue_elem_t*) pclient);
  evcs[evc_id].n_clientflows--;

  /* SEM CLIENTS DOWN */
  osapiSemaGive(ptin_evc_clients_sem);

  /* Update client direct referencing */
  //if (evcBridge->inn_vlan<4096)
  //  evcs[evc_id].client_ref[evcBridge->inn_vlan] = L7_NULLPTR;

  PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: bridge successfully removed", evc_ext_id);

  return L7_SUCCESS;
}


#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED

#define L2INTF_DB_ENTRY_IS_ACTIVE(pentry) ((pentry)->l2intf_id < L2INTF_ID_MAX)

#define CLEAR_L2INTF_DB_ENTRY(pentry)  \
  { \
    memset((pentry), 0x00, sizeof(l2intf_entry_t));  \
    (pentry)->l2intf_id = (unsigned long) -1;          \
  }

static l2intf_entry_t l2intf_db[L2INTF_ID_MAX];
static unsigned long  l2intf_number = 0;

static L7_RC_t l2intf_policer_set(l2intf_entry_t *l2intf, ptin_bw_meter_t *meter);
static L7_RC_t ptin_evc_l2intf_policer(L7_uint32 l2intf_id, ptin_bw_meter_t *meter);

/* Queue management functions */
static void l2intf_pool_init(void);
static L7_RC_t l2intf_pool_pop(L7_int ptin_port, L7_uint32 *l2intf_id, l2intf_entry_t **db_entry);
static L7_RC_t l2intf_pool_push(L7_uint32 l2intf_id);

/**
 * Initialize L2intf Database
 * 
 * @author mruas (30/12/20)
 */
void l2intf_db_init(void)
{
  unsigned long i;

  l2intf_number=0;

  for (i = 0; i < L2INTF_ID_MAX; i++)
  {
    CLEAR_L2INTF_DB_ENTRY(&l2intf_db[i]);
  }

  /* Initialize free L2intf queues */
  l2intf_pool_init();
}

/**
 * Determine l2intf from pon port and gem id
 * 
 * @param pon_port
 * @param gem_id 
 * 
 * @return l2intf_id (output) 
 */
L7_uint32 l2intf_db_search(L7_uint16 pon_port, L7_uint16 gem_id)
{
  /* Search for this virtual port */
  L7_uint32   i;
  ptin_intf_t ptin_intf;

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
  for (i = 0; i < L2INTF_ID_MAX; i++)
  {
    /* Skip non used entries */
    if (!L2INTF_DB_ENTRY_IS_ACTIVE(&l2intf_db[i]))
    {
      continue;
    }

    /* find pon port and vlan */
    if (l2intf_db[i].pon.intf_type == ptin_intf.intf_type &&
        l2intf_db[i].pon.intf_id == ptin_intf.intf_id &&
        l2intf_db[i].gem_id == gem_id)
    {
      break;
    }
  }

  /* Not found? */
  if (i >= L2INTF_ID_MAX)
  {
    PT_LOG_WARN(LOG_CTX_L2, "Matched entry not found: pon_port=%u gem_id=%u", pon_port, gem_id);
    return (L7_uint32) -1;
  }

  /* Return result */
  return l2intf_db[i].l2intf_id;
}

/**
 * Insert a new entry in L2intf Database
 * 
 * @author mruas (30/12/20)
 * 
 * @param entry 
 * 
 * @return L7_RC_t : L7_SUCCESS, L7_TABLE_IS_FULL
 */
L7_RC_t l2intf_db_alloc(l2intf_entry_t *entry)
{
  L7_uint32 l2intf_id;
  L7_uint32 ptin_port;
  l2intf_entry_t *db_entry;

  /* If L2intf id is provided, check if it is already present at DB */
  if (entry->l2intf_id < L2INTF_ID_MAX)
  {
    l2intf_id = entry->l2intf_id;
    db_entry  = &l2intf_db[l2intf_id];

    /* Is DB entry active? */
    if (L2INTF_DB_ENTRY_IS_ACTIVE(db_entry))
    {
      /* Check for inconsistencies */
      if (db_entry->l2intf_id != l2intf_id)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Inconsistency error: [L2intf %u] Invalid l2intf_id (%lu) stored at database",
                   l2intf_id, db_entry->l2intf_id);
        return L7_FAILURE;
      }

      /* Update data */
      entry->policer = db_entry->policer;   /* Copy policer data to this entry */
      *db_entry      = *entry;              //overwrite DB entry

      PT_LOG_INFO(LOG_CTX_EVC, "L2intf %u: database updated", l2intf_id);

      return L7_SUCCESS;
    }
  }
  
  /* Get ptin_port */
  if (ptin_intf_typeId2port(entry->pon.intf_type, entry->pon.intf_id, &ptin_port) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error with ptin_intf %u/%u", entry->pon.intf_type, entry->pon.intf_id);
    return L7_FAILURE;
  }
  
  /* Extract a new id for this L2intf */
  if (l2intf_pool_pop(ptin_port, &l2intf_id, &db_entry) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error extracting a new L2intf id");
    return L7_FAILURE;
  }
  
  /* Check for inconsistencies */
  if (db_entry == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Inconsistency error: Null db_entry pointer");
    return L7_FAILURE;
  }
  /* Check for inconsistencies */
  if (l2intf_id >= L2INTF_ID_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Inconsistency error: Invalid l2intf %u extracted from the free queue", l2intf_id);
    return L7_FAILURE;
  }
  /* Check for inconsistencies */
  if (L2INTF_DB_ENTRY_IS_ACTIVE(db_entry))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Inconsistency error: l2intf %u is already in use at database", l2intf_id);
    return L7_FAILURE;
  }
  
  /* Update l2intf_id field at input struct
     This will mark this entry as valid */
  entry->l2intf_id = l2intf_id;

  /* Update database with new entry */
  *db_entry = *entry;
  memset(&(db_entry->policer), 0x00, sizeof(l2intf_entry_policer_t));   /* No policer for new entries */
  l2intf_number++;

  PT_LOG_INFO(LOG_CTX_EVC, "L2intf %u: New entry allocated", l2intf_id);

  return L7_SUCCESS;
}

/**
 * Remove an entry from the L2intf Database
 * 
 * @author mruas (30/12/20)
 * 
 * @param l2intf_id (in)
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
L7_RC_t l2intf_db_free(L7_uint32 l2intf_id)
{
  l2intf_entry_t *db_entry;

  /* If L2intf id is provided, check if it is already present at DB */
  if (l2intf_id >= L2INTF_ID_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid input L2intf id %u", l2intf_id);
    return L7_FAILURE;
  }

  /* DB entry */
  db_entry = &l2intf_db[l2intf_id];

  /* Check if this L2intf id is already inactive */
  if (!L2INTF_DB_ENTRY_IS_ACTIVE(db_entry))
  {
    PT_LOG_WARN(LOG_CTX_EVC, "L2intf id %u is already inactive", l2intf_id);
    return L7_SUCCESS;
  }

  /* Check for inconsistencies */
  if (db_entry->l2intf_id != l2intf_id)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Inconsistency error: [L2intf %u] Invalid l2intf_id (%lu) stored at database",
               l2intf_id, l2intf_db[l2intf_id].l2intf_id);
    return L7_FAILURE;
  }

  /* Mark this L2intf id as free */
  if (l2intf_pool_push(l2intf_id) != L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_EVC, "Error freeing L2intf id %u", l2intf_id);
    return L7_FAILURE;
  }

  /* Remove policer */
  (void) l2intf_policer_set(db_entry, L7_NULLPTR);
  /* Empty entry at database */
  CLEAR_L2INTF_DB_ENTRY(db_entry);
  if (l2intf_number > 0)
  {
    l2intf_number--;
  }
  
  PT_LOG_INFO(LOG_CTX_EVC, "L2intf %u: Entry freed", l2intf_id);

  return L7_SUCCESS;
}

/**
 * Search for a specific entry in L2intf Database
 * 
 * @author mruas (30/12/20)
 *  
 * @param l2intf_id (in)  
 * @param entry (out)
 * 
 * @return L7_RC_t : L7_SUCCESS, L7_FAILURE
 */
L7_RC_t l2intf_db_data_get(L7_uint32 l2intf_id, l2intf_entry_t *entry)
{
  l2intf_entry_t *db_entry;

  /* If L2intf id is provided, check if it is already present at DB */
  if (l2intf_id >= L2INTF_ID_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid input L2intf id %u", l2intf_id);
    return L7_FAILURE;
  }

  db_entry = &l2intf_db[l2intf_id];

  /* Check if this L2intf id is already inactive */
  if (!L2INTF_DB_ENTRY_IS_ACTIVE(db_entry))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "L2intf id %u is inactive", l2intf_id);
    return L7_FAILURE;
  }

  /* Check for inconsistencies */
  if (db_entry->l2intf_id != l2intf_id)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Inconsistency error: [L2intf %u] Invalid l2intf_id (%lu) stored at database",
               l2intf_id, db_entry->l2intf_id);
    return L7_FAILURE;
  }

  //did find it
  if (entry != L7_NULLPTR)
  {
    *entry = *db_entry;
  }

  return L7_SUCCESS;
}

/**
 * Find a particular entry inside virtual port list
 * 
 * @param l2intf_id 
 * 
 * @return entry pointer
 */
static l2intf_entry_t *l2intf_db_ptr_get(L7_uint32 l2intf_id)
{
  l2intf_entry_t *db_entry;

  /* If L2intf id is provided, check if it is already present at DB */
  if (l2intf_id >= L2INTF_ID_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid input L2intf id %u", l2intf_id);
    return L7_NULLPTR;
  }

  db_entry = &l2intf_db[l2intf_id];

  /* Check if this L2intf id is already inactive */
  if (!L2INTF_DB_ENTRY_IS_ACTIVE(db_entry))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "L2intf id %u is inactive", l2intf_id);
    return L7_NULLPTR;
  }

  /* Check for inconsistencies */
  if (db_entry->l2intf_id != l2intf_id)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Inconsistency error: [L2intf %u] Invalid l2intf_id (%lu) stored at database",
               l2intf_id, db_entry->l2intf_id);
    return L7_NULLPTR;
  }
  
  return &l2intf_db[l2intf_id];
}


/* The following constants will allow L2intf range separation, in order to allow
   specific QoS configurations for each range.
   For TC16SXG board, 2 ranges will be defined with the first applied to GPON ports,
   and the second for XGSPON ports */
#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
 #define L2INTF_QUEUE_SELECT_BY_PORT(ptin_port)      (PTIN_PORT_IS_PON_GPON_TYPE(ptin_port) ? 0 : 1)
 #define L2INTF_QUEUE_SELECT_BY_L2INTF_ID(l2intf_id) (((l2intf_id) < (L2INTF_ID_MAX_PER_QUEUE)) ? 0 : 1)
#else
 #define L2INTF_QUEUE_SELECT_BY_PORT(ptin_port)      0
 #define L2INTF_QUEUE_SELECT_BY_L2INTF_ID(l2intf_id) 0
#endif

/* Pool of L2intf id to be used for the free L2intfs queue */
typedef struct l2intf_id_pool_entry_s {
    /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct ptin_evc_entry_s *next;
  struct ptin_evc_entry_s *prev;

  L7_uint32 l2intf_id;
  l2intf_entry_t *db_entry;

} l2intf_pool_entry_t;

static l2intf_pool_entry_t l2intf_pool[L2INTF_ID_MAX];
static dl_queue_t          queue_free_l2intf[L2INTF_QUEUES_NUMBER];

/**
 * Initialize free L2intf id queues
 * 
 * @author mruas (30/12/20)
 */
static void l2intf_pool_init(void)
{
  int i, j, l2intf_id;

  /* Initialize l2intf_id pool */
  for (l2intf_id = 0; l2intf_id < L2INTF_ID_MAX; l2intf_id++)
  {
    /* Fill data at pool entry */
    l2intf_pool[l2intf_id].l2intf_id = l2intf_id;
    l2intf_pool[l2intf_id].db_entry  = &l2intf_db[l2intf_id];
  }

  /* Initialize free L2intf queues */
  for (i = 0; i < L2INTF_QUEUES_NUMBER; i++)
  {
    dl_queue_init(&queue_free_l2intf[i]);

    for (j = 0; j < L2INTF_ID_MAX_PER_QUEUE; j++)
    {
      /* Caulate l2intf id */
      l2intf_id = (L2INTF_ID_MAX_PER_QUEUE * i) + j;

      /* Skip l2intf null */
      if (l2intf_id == 0)
      {
        continue;
      }

      if (l2intf_id >= L2INTF_ID_MAX)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Invalid L2intf %u for i=%d, j=%d (l2intf_id_max_per_pool=%d)",
                   l2intf_id, i, j, L2INTF_ID_MAX_PER_QUEUE);
        return;
      }
      
      /* add pool entry to queue */
      dl_queue_add(&queue_free_l2intf[i], (dl_queue_elem_t*) &l2intf_pool[l2intf_id]);
    }
  }
}

/**
 * Extract a new L2intf id from the free queue, according to the 
 * provided ptin_port 
 * 
 * @author mruas (30/12/20)
 * 
 * @param ptin_port (in)
 * @param l2intf_id (out) 
 * @param db_entry (out) : Pointer to DB entry
 * 
 * @return L7_RC_t 
 */
static L7_RC_t l2intf_pool_pop(L7_int ptin_port, L7_uint32 *l2intf_id, l2intf_entry_t **db_entry)
{
  int queue_sel;
  l2intf_pool_entry_t *pool_entry;
  int rc;

  /* Validate ptin_port */
  if (ptin_port >= PTIN_SYSTEM_N_PONS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid ptin_port %u: Only PON ports allowed", ptin_port);
    return L7_FAILURE;
  }

  /* Select queue */
  queue_sel = L2INTF_QUEUE_SELECT_BY_PORT(ptin_port);

  /* Check if there are entries to be used */
  if (queue_free_l2intf[queue_sel].n_elems == 0)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Free L2intf queue(%d) is empty", queue_sel);
    return L7_FAILURE;
  }

  /* Try to get an entry from the pool of free elements */
  rc = dl_queue_remove_head(&queue_free_l2intf[queue_sel], (dl_queue_elem_t **) &pool_entry);
  if (rc != NOERR) {
    PT_LOG_ERR(LOG_CTX_EVC, "There are no free L2intfs available at queue(%d)! rc=%d", queue_sel, rc);
    return L7_FAILURE;
  }

  /* Check for inconsistencies */
  if (pool_entry->l2intf_id >= L2INTF_ID_MAX)
  {
    dl_queue_add_head(&queue_free_l2intf[queue_sel], (dl_queue_elem_t *) pool_entry);
    PT_LOG_ERR(LOG_CTX_EVC, "Inconsistency error: Invalid l2intf %u extracted", pool_entry->l2intf_id);
    return L7_FAILURE;
  }

  /* Return l2intf_id */
  if (l2intf_id != L7_NULLPTR)
  {
    *l2intf_id = pool_entry->l2intf_id;
  }
  if (db_entry != L7_NULLPTR)
  {
    *db_entry = pool_entry->db_entry;
  }
  
  return L7_SUCCESS;
}

/**
 * Push an L2intf id to the free queue)
 * 
 * @author mruas (30/12/20)
 * 
 * @param l2intf_id (in) 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t l2intf_pool_push(L7_uint32 l2intf_id)
{
  int queue_sel;
  l2intf_pool_entry_t *pool_entry;
  int rc;

  /* Validate l2intf */
  if (l2intf_id >= L2INTF_ID_MAX)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid l2intf %u", l2intf_id);
    return L7_FAILURE;
  }

  /* Extract pool entry */
  pool_entry = &l2intf_pool[l2intf_id];

  /* Check for inconsistencies */
  if (pool_entry->l2intf_id != l2intf_id)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Inconsistency error: [L2intf %u] Invalid l2intf_id (%u) stored at the pool entry",
               l2intf_id, pool_entry->l2intf_id);
    return L7_FAILURE;
  }

  /* Check for inconsistencies */
  if (pool_entry->prev != NULL || pool_entry->next != NULL)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Inconsistency error: L2intf %u is already part of a queue", l2intf_id);
    return L7_FAILURE;
  }
  
  /* Select queue */
  queue_sel = L2INTF_QUEUE_SELECT_BY_L2INTF_ID(l2intf_id);

  /* Push this entry to the a queue */
  rc = dl_queue_add_head(&queue_free_l2intf[queue_sel], (dl_queue_elem_t *) pool_entry);
  if (rc != NOERR) {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding L2intf to queue(%d)! rc=%d", queue_sel, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Set bandwidth policer for one virtual port
 * 
 * @param l2intf
 * @param meter 
 * 
 * @return int : 0>Success, -1>Failed
 */
static L7_RC_t l2intf_policer_set(l2intf_entry_t *l2intf, ptin_bw_meter_t *meter)
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
  if (l2intf == L7_NULLPTR)
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
    policer_id = (l2intf->policer.in_use) ? l2intf->policer.policer_id : -1;

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
    l2intf->policer.meter      = *meter;
    l2intf->policer.policer_id = policer_id;
    /* New policer */
    l2intf->policer.in_use     = L7_TRUE;
  }
  else
  {
    /* Check if policer exists */
    if (!l2intf->policer.in_use)
    {
      PT_LOG_WARN(LOG_CTX_L2, "l2intf_id 0x%lx already does not have policer", l2intf->l2intf_id);
      return L7_SUCCESS;
    }

    PT_LOG_TRACE(LOG_CTX_EVC,"Going to remove policer");

    /* Remove this policer to all MAC entries */
    meter_info.cir = (L7_uint32) -1;
    meter_info.eir = (L7_uint32) -1;
    meter_info.cbs = (L7_uint32) -1;
    meter_info.ebs = (L7_uint32) -1;

    /* Policer id in use */
    policer_id = l2intf->policer.policer_id;
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
    if (l2intf->l2intf_id != fdbEntry.dot1dTpFdbL2intf)
    {
      continue;
    }

    vlanId = ((L7_uint16) fdbEntry.dot1dTpFdbAddress[0]<<8) | ((L7_uint16) fdbEntry.dot1dTpFdbAddress[1]);

    /* Apply policer to current MAC address */
    profile.ptin_port           = -1;
    profile.outer_vlan_ingress  = vlanId;
    profile.cos                 = (L7_uint8) -1;
    memcpy(profile.
    macAddr, &fdbEntry.dot1dTpFdbAddress[L7_FDB_IVL_ID_LEN], sizeof(L7_uint8)*L7_MAC_ADDR_LEN);

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
    PT_LOG_TRACE(LOG_CTX_L2, "Success updating policer to virtual port 0x%08lx",l2intf->l2intf_id);

    /* Remove policer, if it was that intention */
    if (meter == L7_NULLPTR || meter->cir == (L7_uint32)-1)
    {
      PT_LOG_TRACE(LOG_CTX_EVC,"Policer %d", l2intf->policer.policer_id);

      if (l2intf->policer.policer_id > 0)
      {
        PT_LOG_TRACE(LOG_CTX_EVC,"Going to destroy policer %d", l2intf->policer.policer_id);

        /* Destroy policer */
        if (ptin_bwPolicer_destroy(l2intf->policer.policer_id) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_L2, "Error destroying policer id %d", l2intf->policer.policer_id);
          return L7_FAILURE;
        }

        PT_LOG_TRACE(LOG_CTX_EVC,"Policer %d removed", l2intf->policer.policer_id);
      }

      /* Clean data */
      memset(&l2intf->policer, 0x00, sizeof(l2intf_entry_policer_t));
      l2intf->policer.in_use = L7_FALSE;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_L2, "Error updating policer to virtual port 0x%08lx",l2intf->l2intf_id);
  }

  PT_LOG_TRACE(LOG_CTX_EVC,"Finished");

  return L7_SUCCESS;
}

/**
 * Set bandwidth policer for one virtual port
 * 
 * @param l2intf
 * @param meter 
 * 
 * @return int : 0>Success, -1>Failed
 */
static L7_RC_t ptin_evc_l2intf_policer(L7_uint32 l2intf_id, ptin_bw_meter_t *meter)
{
  l2intf_entry_t *l2intf_entry;

  l2intf_entry = l2intf_db_ptr_get(l2intf_id);

  if (l2intf_entry == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_L2, "Error getting pointer tp vp entry (vp 0x%x)", l2intf_id);
    return L7_FAILURE;
  }

  /* Process policer */
  return l2intf_policer_set(l2intf_entry, meter);
}

/**
 * Dump the L2intf Database
 * 
 * @author mruas (30/12/20)
 */
void l2intf_db_dump(void)
{
  unsigned long i;

  printf("Dumping configured virtual ports:\n\r");

  for (i=0; i<L2INTF_ID_MAX; i++)
  {
      if (l2intf_db[i].l2intf_id >= L2INTF_ID_MAX)
      {
        continue;
      }
      printf(" <%4lu> l2intf_id=%-4lu pon=%u/%-2u gem_id=%-4u\n\r", i,
             l2intf_db[i].l2intf_id, l2intf_db[i].pon.intf_type,
             l2intf_db[i].pon.intf_id, l2intf_db[i].gem_id);
  }

  printf("Number of virtual ports: %lu\n\r", l2intf_number);
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

  /* Input Argument validation */
  if ( ecvFlow  == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid arguments [msg:%p]",ecvFlow);    
    return L7_FAILURE;
  }
  
  evc_ext_id = ecvFlow->evc_idx;
  PT_LOG_TRACE(LOG_CTX_EVC, "Adding eEVC# %u flow connection...", evc_ext_id);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_id);
    return L7_SUCCESS;
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

  /* Only for QUATTRO serices */
  if (!IS_EVC_QUATTRO(evc_id))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Flows are only applied to QUATTRO services", evc_id);
    return L7_FAILURE;
  }

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  {
    L7_uint                   int_ovid;
    ptin_client_id_t          clientId;
    struct ptin_evc_client_s *pflow;
    L7_BOOL                   addOrRemove = L7_FALSE;//Add Packages
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
    clientId.ptin_port            = ptintf2port(clientId.ptin_intf.intf_type,
                                                clientId.ptin_intf.intf_id); 
    clientId.outerVlan            = pflow->int_ovid;
    clientId.innerVlan            = pflow->int_ivid;
    clientId.mask                 = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_OUTERVLAN | PTIN_CLIENT_MASK_FIELD_INNERVLAN;

    /* Add client */
    if ( (rc = ptin_igmp_api_client_add(&clientId, pflow->uni_ovid, pflow->uni_ivid, ecvFlow->onuId, 0x00, 0, 0, addOrRemove, ecvFlow->packageBmpList, ecvFlow->noOfPackages) != L7_SUCCESS) )
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error adding client to IGMP instance", evc_id);
      return rc;
    }
    return rc;
  }
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
    
  /* Input Argument validation */
  if ( ecvFlow  == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid arguments [msg:%p]",ecvFlow);    
    return L7_FAILURE;
  }

  evc_ext_id = ecvFlow->evc_idx;
  PT_LOG_TRACE(LOG_CTX_EVC, "Adding eEVC# %u flow connection...", evc_ext_id);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
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

  /* Only for QUATTRO serices */
  if (!IS_EVC_QUATTRO(evc_id))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Flows are only applied to QUATTRO services", evc_id);
    return L7_FAILURE;
  }

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  {
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
    clientId.ptin_port            = ptintf2port(clientId.ptin_intf.intf_type,
                                                clientId.ptin_intf.intf_id); 
    clientId.outerVlan            = pflow->int_ovid;
    clientId.innerVlan            = pflow->int_ivid;
    clientId.mask                 = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_OUTERVLAN | PTIN_CLIENT_MASK_FIELD_INNERVLAN;

    /* Add client */
    if ( (rc = ptin_igmp_api_client_add(&clientId, pflow->uni_ovid, pflow->uni_ivid, ecvFlow->onuId, 0x0C, 0, 0, addOrRemove, ecvFlow->packageBmpList, ecvFlow->noOfPackages) != L7_SUCCESS) )
    {
      PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: IGMP Client Not Found", evc_id);
      return SUCCESS;
    }
    return rc;
  }
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
L7_RC_t ptin_evc_flow_remove_port(L7_uint32 ptin_port, L7_uint32 evc_ext_id)
{
  L7_uint32 evc_id;
  dl_queue_t *queue;
  struct ptin_evc_client_s *pclientFlow = NULL;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_id);
    return L7_FAILURE;
  }

  queue = &evcs[evc_id].intf[ptin_port].clients;
  dl_queue_get_head( queue, (dl_queue_elem_t **)&pclientFlow);

  while (pclientFlow != NULL)
  {

   ptin_HwEthEvcFlow_t evcFlow;
  
   evcFlow.evc_idx  = evc_ext_id;               // EVC Id [1..PTIN_SYSTEM_N_EVCS]
   evcFlow.flags    = pclientFlow->flags;        // Protocol flags
   evcFlow.int_ivid = pclientFlow->int_ivid;    // C-VLAN tagged in the upstream flows (inside the switch)
   /* Determine leaf ptin_intf */
   evcFlow.ptin_intf.intf_id   = ptin_port;
   evcFlow.ptin_intf.intf_type = PTIN_EVC_INTF_PHYSICAL;
   /* Client interface (root is already known by the EVC) */
   evcFlow.uni_ovid     = pclientFlow->uni_ovid;     // GEM id
   evcFlow.uni_ivid     = pclientFlow->uni_ivid;     // UNI cvlan

   evcFlow.macLearnMax  = pclientFlow->macLearnMax; // pclientFlow-;  // Maximum number of Learned MAC addresses                           
   evcFlow.onuId        = pclientFlow->onuId; //pclientFlow->onuId;        // ONU/CPE Identifier
   evcFlow.mask         = pclientFlow->mask; //pclientFlow->mask;
   evcFlow.maxChannels  = pclientFlow->maxChannels; //pclientFlow-> maxChannels;  // [mask = 0x01] Maximum number of channels this client can simultaneously watch
   evcFlow.maxBandwidth = pclientFlow->maxBandwidth;//pclientFlow->bwprofile; // [mask = 0x02] Maximum bandwidth that this client can simultaneously consume (bit/s)
   //L7_uint32   packageBmpList[(PTIN_SYSTEM_IGMP_MAXPACKAGES-1)/(sizeof(L7_uint32)*8)+1];  //[mask=0x04]  Package Bitmap List   
   evcFlow.noOfPackages = 0; //[mask=0x08]  Number of Packages

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
   ptin_evc_flow_unconfig(evc_id, ptin_port, pclientFlow->uni_ovid);
#endif

   pclientFlow = (struct ptin_evc_client_s *) dl_queue_get_next(queue, (dl_queue_elem_t *)pclientFlow);
  }

  return L7_SUCCESS;

}
/**
 * Adds a flow to the EVC
 * 
 * @param evcFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_flow_replicate(L7_uint32 ptin_port, L7_uint32 evc_ext_id, L7_uint32 leaf_port)
{ 
  L7_uint32  evc_id;
  dl_queue_t *queue;
  struct ptin_evc_client_s *pclientFlow = NULL;

 /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_id);
    return L7_FAILURE;
  }

  /* Validate leaf interface (from received message) */
  if ((leaf_port >= PTIN_SYSTEM_N_INTERF) ||
      (!evcs[evc_id].intf[leaf_port].in_use) ||
      (evcs[evc_id].intf[leaf_port].type != PTIN_EVC_INTF_LEAF))
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_id);
    return L7_FAILURE;
  }

  queue = &evcs[evc_id].intf[leaf_port].clients;
  dl_queue_get_head( queue, (dl_queue_elem_t **)&pclientFlow);

  while (pclientFlow != NULL)
  {

   ptin_HwEthEvcFlow_t evcFlow;
  
   evcFlow.evc_idx  = evc_ext_id;               // EVC Id [1..PTIN_SYSTEM_N_EVCS]
   evcFlow.flags    = pclientFlow->flags;        // Protocol flags
   evcFlow.int_ivid = pclientFlow->int_ivid;    // C-VLAN tagged in the upstream flows (inside the switch)
   /* Determine leaf ptin_intf */
   evcFlow.ptin_intf.intf_id   = ptin_port;
   evcFlow.ptin_intf.intf_type = PTIN_EVC_INTF_PHYSICAL;
   /* Client interface (root is already known by the EVC) */
   evcFlow.uni_ovid     = pclientFlow->uni_ovid;     // GEM id
   evcFlow.uni_ivid     = pclientFlow->uni_ivid;     // UNI cvlan

   evcFlow.macLearnMax  = pclientFlow->macLearnMax; // pclientFlow-;  // Maximum number of Learned MAC addresses                           
   evcFlow.onuId        = pclientFlow->onuId; //pclientFlow->onuId;        // ONU/CPE Identifier
   evcFlow.mask         = pclientFlow->mask; //pclientFlow->mask;
   evcFlow.maxChannels  = pclientFlow->maxChannels; //pclientFlow-> maxChannels;  // [mask = 0x01] Maximum number of channels this client can simultaneously watch
   evcFlow.maxBandwidth = pclientFlow->maxBandwidth;//pclientFlow->bwprofile; // [mask = 0x02] Maximum bandwidth that this client can simultaneously consume (bit/s)

   //L7_uint32   packageBmpList[(PTIN_SYSTEM_IGMP_MAXPACKAGES-1)/(sizeof(L7_uint32)*8)+1];  //[mask=0x04]  Package Bitmap List   
   // evcFlow.noOfPackages = 0; //[mask=0x08]  Number of Packages

   ptin_evc_flow_add(&evcFlow);

   pclientFlow = (struct ptin_evc_client_s *) dl_queue_get_next(queue, (dl_queue_elem_t *)pclientFlow);
  }

  return L7_SUCCESS;
}

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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_FAILURE;
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
  {
    L7_uint int_ovid;
    L7_int  l2intf_id, multicast_group;
    L7_BOOL igmp_enabled, dhcpv4_enabled, dhcpv6_enabled, pppoe_enabled;
    /* Always add client */
    ptin_client_id_t clientId;

    struct ptin_evc_client_s *pflow;

    /* Get internal vlan and inner NNI vlan */
    int_ovid = evcs[evc_id].intf[leaf_port].int_vlan;

    /* Multicast group */
    multicast_group = evcs[evc_id].mcgroup;

    /* Check if flow entry already exists */
    ptin_evc_find_flow(evcFlow->uni_ovid, &evcs[evc_id].intf[leaf_port].clients, (dl_queue_elem_t**) &pflow);

    /* If flow does it, create it */
    if (pflow == NULL)
    {
      l2intf_entry_t l2intf_entry;

      /* Check if there is available flows */
      if (queue_free_clients.n_elems == 0)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: No available flows", evc_id);
        return L7_NO_RESOURCES;
      }

      /* Get a new L2intf id */
      memset(&l2intf_entry, 0x00, sizeof(l2intf_entry_t));
      l2intf_entry.l2intf_id = (unsigned long)-1;
      l2intf_entry.pon       = evcFlow->ptin_intf;
      l2intf_entry.gem_id    = evcFlow->uni_ovid;

      if (l2intf_db_alloc(&l2intf_entry) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error adding L2intf to DB", evc_id);
        return L7_FAILURE;
      }
      /* After the insertion, the L2intf_id field is updated */
      l2intf_id = l2intf_entry.l2intf_id;

      /* Create virtual port */
      if (ptin_l2intf_create(leaf_port,
                                evcFlow->uni_ovid, evcFlow->uni_ivid,
                                int_ovid, evcFlow->int_ivid,
                                multicast_group,
                                &l2intf_id,
                                evcFlow->macLearnMax) != L7_SUCCESS)
      {
        (void) l2intf_db_free(l2intf_id);
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error creating virtual port", evc_id);
        return L7_FAILURE;
      }

      /* Add client to the EVC struct */
      dl_queue_remove_head(&queue_free_clients, (dl_queue_elem_t**) &pflow);    /* get a free client entry */
      pflow->in_use            = L7_TRUE;                                              /* update it */
      pflow->int_ovid          = int_ovid;
      pflow->int_ivid          = evcFlow->int_ivid;
      pflow->uni_ovid          = evcFlow->uni_ovid;
      pflow->uni_ivid          = evcFlow->uni_ivid;
      pflow->action_outer_vlan = PTIN_XLATE_ACTION_REPLACE;
      pflow->action_inner_vlan = PTIN_XLATE_ACTION_NONE;
      pflow->client_vid        = evcFlow->uni_ivid;
      pflow->flags             = 0; //evcFlow->flags;    /* Initial value: no flags exist */
      pflow->virtual_gport     = l2intf_id;
      pflow->l2intf_id         = l2intf_id & 0xffffff;
      pflow->macLearnMax       = evcFlow->macLearnMax;
      pflow->onuId             = evcFlow->onuId;
      pflow->mask              = evcFlow->mask;
      pflow->maxBandwidth      = evcFlow->maxBandwidth;
      pflow->maxChannels       = evcFlow->maxChannels;

      dl_queue_add_tail(&evcs[evc_id].intf[leaf_port].clients, (dl_queue_elem_t*) pflow); /* add it to the corresponding interface */
      evcs[evc_id].n_clientflows++;

      PT_LOG_TRACE(LOG_CTX_EVC, "eEVC# %u: flow successfully added (l2intf_id=%u\tpon=%u/%u\tgem_id=%u)",
                   evc_ext_id,
                   l2intf_id & 0xffffff,
                   evcFlow->ptin_intf.intf_type,evcFlow->ptin_intf.intf_id,
                   evcFlow->uni_ovid);
    }
    else
    {
      PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: GEM id %u already exists (ptin_port:%u)", evc_id, evcFlow->uni_ovid, leaf_port);
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
    clientId.ptin_port            = ptintf2port(clientId.ptin_intf.intf_type,
                                                clientId.ptin_intf.intf_id); 
    clientId.outerVlan            = pflow->int_ovid;
    clientId.innerVlan            = pflow->int_ivid;
    clientId.mask                 = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_OUTERVLAN | PTIN_CLIENT_MASK_FIELD_INNERVLAN;

    /* Manage IGMP client */
    if (evcFlow->flags & PTIN_EVC_MASK_IGMP_PROTOCOL)
    {
      /* Add client */
      if (ptin_igmp_api_client_add(&clientId, pflow->uni_ovid, pflow->uni_ivid, evcFlow->onuId, evcFlow->mask, evcFlow->maxBandwidth, evcFlow->maxChannels, L7_FALSE, evcFlow->packageBmpList, evcFlow->noOfPackages) != L7_SUCCESS)
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
      if (ptin_igmp_api_client_remove(&clientId) != L7_SUCCESS)
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
      ptin_igmp_api_client_remove(&clientId);
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
  L7_uint   leaf_port;
  L7_RC_t   rc = L7_SUCCESS;

  evc_ext_id = evcFlow->evc_idx;

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing eEVC# %u flow connection...", evc_ext_id);

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Is EVC in use? */
  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is not in use", evc_ext_id);
    return L7_SUCCESS;
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
  /* Remove flow */
  rc = ptin_evc_flow_unconfig(evc_id, leaf_port, evcFlow->uni_ovid);
#endif

  return rc;
}

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
/**
 * Unconfigure an EVC flow
 * 
 * @param evc_id 
 * @param ptin_port 
 * @param uni_ovid : external outer vlan (gem id)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_evc_flow_unconfig(L7_int evc_id, L7_int ptin_port, L7_int16 uni_ovid)
{
  L7_uint client_vlan;
  L7_int multicast_group;
  ptin_intf_t ptin_intf;
  struct ptin_evc_client_s *pflow;
  L7_uint32 evc_ext_id;
  L7_BOOL   igmp_enabled, dhcpv4_enabled, dhcpv6_enabled, pppoe_enabled;
  L7_RC_t   rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_id >= PTIN_SYSTEM_N_EVCS || ptin_port >= PTIN_SYSTEM_N_INTERF /*|| flow_id >= PTIN_SYSTEM_N_FLOWS_MAX*/)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid EVC# %u, ptin_port %u", evc_id, ptin_port);
    return L7_FAILURE;
  }

  /* EVC and port should be active */
  if (!evcs[evc_id].in_use || !evcs[evc_id].intf[ptin_port].in_use /*|| !evcs[evc_id].intf[ptin_port].flow[flow_id].in_use*/)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u/port %u not active", evc_id, ptin_port);
    return L7_FAILURE;
  }

  evc_ext_id = evcs[evc_id].extended_id;

  /* Convert to ptin_intf */
  if (ptin_intf_port2ptintf(ptin_port, &ptin_intf)  != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Cannot get ptin_intf from port %u", evc_id, ptin_port);
    return L7_FAILURE;
  }

  /* Check if flow entry does not exists: do nothing if don't */
  ptin_evc_find_flow(uni_ovid, &evcs[evc_id].intf[ptin_port].clients, (dl_queue_elem_t**) &pflow);
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
    clientId.ptin_port            = ptintf2port(clientId.ptin_intf.intf_type,
                                                clientId.ptin_intf.intf_id); 
    clientId.outerVlan            = pflow->int_ovid;
    clientId.innerVlan            = pflow->int_ivid;
    clientId.mask                 = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_OUTERVLAN | PTIN_CLIENT_MASK_FIELD_INNERVLAN;

    /* Remove client */
    if ( (rc = ptin_igmp_api_client_remove(&clientId)) != L7_SUCCESS)
    {
      /*This is not an error if this routine is invoked after a reset defaults message*/
      if (rc == L7_NOT_EXIST)
      {
        /* L7_NOT_EXIST is not an error */
       PT_LOG_NOTICE(LOG_CTX_EVC, "EVC# %u: Client does not exist on IGMP instance (intf_type:%u/intf_id:%u outerVlan:%u/innerVlan:%u)", 
                  evc_ext_id, clientId.ptin_intf.intf_type, clientId.ptin_intf.intf_id, clientId.outerVlan, clientId.innerVlan);       
      }
      else
      {
         PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing client from IGMP instance (intf_type:%u/intf_id:%u outerVlan:%u/innerVlan:%u) (rc:%u)", 
                 evc_ext_id, clientId.ptin_intf.intf_type, clientId.ptin_intf.intf_id, clientId.outerVlan, clientId.innerVlan, rc);
        //rc = L7_FAILURE;    
      }
      rc = L7_SUCCESS;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Client removed from IGMP instance (intf_type:%u/intf_id:%u outerVlan:%u/innerVlan:%u)", 
                  evc_ext_id, clientId.ptin_intf.intf_type, clientId.ptin_intf.intf_id, clientId.outerVlan, clientId.innerVlan);          
    }
  }

  /* Multicast group */
  multicast_group = evcs[evc_id].mcgroup;

  /* Get client inner vlan */
  client_vlan = pflow->int_ivid;

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: Going to remove flow related to client %u", evc_id, client_vlan);

  /* Force removal of counters and profiles */
  ptin_evc_pclientFlow_clean(evc_id, ptin_port, pflow, L7_TRUE);

  /* Remove virtual port */
  if (ptin_l2intf_remove(ptin_port, pflow->virtual_gport, multicast_group) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error removing virtual port", evc_id);
    return L7_FAILURE;
  }

  if (l2intf_db_free(pflow->l2intf_id) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: Error freeing l2intf id %u", evc_id, pflow->l2intf_id);
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
#endif

#if (!PTIN_BOARD_IS_MATRIX)
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
static 
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
static 
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
#endif

#if defined(IGMPASSOC_MULTI_MC_SUPPORTED) || PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
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
static 
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

  /* Check if IGMP configuration should be done */
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
#endif

/**
 * Gets the flooding vlans list
 * 
 * @param ptin_port   : leaf interface
 * @param intVlan     : internal Vlan
 * @param client_vlan : client vlan to apply this flooding vlan
 * @param outer_vlan  : list of outer vlans
 * @param inner_vlan  : list of inner vlans 
 * @param number_of_vlans : Size of returned lists
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evc_flood_vlan_get( L7_uint32 ptin_port, L7_uint intVlan, L7_uint client_vlan,
                                 L7_uint16 *outer_vlan, L7_uint16 *inner_vlan, L7_uint16 *number_of_vlans )
{
  L7_uint16   evc_id;
  L7_uint16   max_vlans;
  L7_int      port, i, index;
  dl_queue_t *queue;
  struct ptin_evc_client_s *pclient;

  /* Validate arguments */
  if (intVlan<PTIN_VLAN_MIN || intVlan>PTIN_VLAN_MAX)
  {
    if (ptin_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get evc id and validate it */
  evc_id = evcId_from_internalVlan[intVlan];
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    if (ptin_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_EVC,"Internal Outer vlan (%u) is not used in any EVC",intVlan);
    return L7_FAILURE;
  }

  /* EVC should be active */
  if (!evcs[evc_id].in_use)
  {
    if (ptin_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u should be in use (intVlan=%u)",evc_id,intVlan);
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
  if ( ptin_port != PTIN_PORT_ALL )
  {
    /* Validate ptin_port */
    if ( ptin_port >= PTIN_SYSTEM_N_INTERF )
    {
      if (ptin_packet_debug_enable)
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: invalid ptin_port %u", evc_id, ptin_port);
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
    ptin_port = PTIN_PORT_ALL;   /* All ports */
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
    /* Only apply to specified port, or all of them if not specified */
    if ( ptin_port < PTIN_SYSTEM_N_INTERF && ptin_port != port )
      continue;

    /* Skip not used ports, or non leafs */
    if ( !evcs[evc_id].intf[ptin_port].in_use ||
         evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF )
      continue;

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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
    ptin_port = PTIN_PORT_ALL;   /* All ports */
  }

  /* Run all Leaf ports */
  for (port=0; port<PTIN_SYSTEM_N_INTERF; port++)
  {
    /* Only apply to specified port, or all of them if not specified */
    if ( ptin_port < PTIN_SYSTEM_N_INTERF && ptin_port != port )
      continue;

    /* Skip not used ports, or non leafs */
    if ( !evcs[evc_id].intf[ptin_port].in_use ||
         evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF )
      continue;

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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
    ptin_port = PTIN_PORT_ALL;   /* All ports */
  }

  /* Run all Leaf ports */
  for (port=0; port<PTIN_SYSTEM_N_INTERF; port++)
  {
    /* Only apply to specified port, or all of them if not specified */
    if ( ptin_port < PTIN_SYSTEM_N_INTERF && ptin_port != port )
      continue;

    /* Skip not used ports, or non leafs */
    if ( !evcs[evc_id].intf[ptin_port].in_use ||
         evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF )
      continue;

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
  stormControl->cpu_rate   = (L7_uint32) RATE_LIMIT_CPU_TRAFFIC * 1000;

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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
    L7_uint32 l2intf_id;

    /* Calculate l2intf */
    l2intf_id = l2intf_db_search(profile->ptin_port, profile->outer_vlan_lookup);

    if (l2intf_id == 0 || l2intf_id == (L7_uint32)-1)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Invalid l2intf_id 0x%x", l2intf_id);
      return L7_FAILURE;
    }

    /* Apply policer */
    if (ptin_evc_l2intf_policer(l2intf_id, meter) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Error applying policer to l2intf_id 0x%x", l2intf_id);
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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
    L7_uint32 l2intf_id;

    /* Calculate l2intf_id */
    l2intf_id = l2intf_db_search(profile->ptin_port, profile->outer_vlan_lookup);

    if (l2intf_id == 0 || l2intf_id == (L7_uint32)-1)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Invalid l2intf_id 0x%x", l2intf_id);
      return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_EVC,"pon_port=%u, gem_id=%u: l2intf_id=0x%x", profile->ptin_port, profile->outer_vlan_lookup, l2intf_id);

    /* Apply policer */
    if (ptin_evc_l2intf_policer(l2intf_id, L7_NULLPTR) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Error removing policer from l2intf_id 0x%x", l2intf_id);
      return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_EVC,"Success applying meter for pon_port=%u, gem_id=%u (l2intf_id=0x%x)",
              profile->ptin_port, profile->outer_vlan_lookup, l2intf_id);

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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
 * Dump all clients/flows for a specific evc
 * 
 * @param evc_id : evc index 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_allclientsflows_dump( L7_uint evc_id )
{
    L7_uint     intf_idx;
    L7_RC_t     rc = L7_SUCCESS;
    struct  ptin_evc_client_s *pclientFlow;

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

        /* Get all clients */
        pclientFlow = L7_NULLPTR;

        if( dl_queue_get_head(&evcs[evc_id].intf[intf_idx].clients, (dl_queue_elem_t **) &pclientFlow) != NOERR)
            continue;

        while ( pclientFlow != L7_NULLPTR)
        {   
            printf("Evc_idx           %u \n", evc_id);
            printf("intf_idx          %u \n", intf_idx);
            printf("Flow                 \n");
            printf("in_use            %u \n", pclientFlow->in_use);
            printf("int_ovid          %u \n", pclientFlow->int_ovid);
            printf("int_ivid          %u \n", pclientFlow->int_ivid);
            printf("uni_ovid          %u \n", pclientFlow->uni_ovid);
            printf("uni_ivid          %u \n", pclientFlow->uni_ivid);
            printf("action_outer_vlan %u \n", pclientFlow->action_outer_vlan);
            printf("action_inner_vlan %u \n", pclientFlow->action_inner_vlan);
            printf("client_vid        %u \n", pclientFlow->client_vid);
            printf("flood_vlan        %u %u %u %u %u %u %u %u\n", 
                         pclientFlow->flood_vlan[0],pclientFlow->flood_vlan[1],pclientFlow->flood_vlan[2],
                         pclientFlow->flood_vlan[3],pclientFlow->flood_vlan[4],pclientFlow->flood_vlan[5],
                         pclientFlow->flood_vlan[6],pclientFlow->flood_vlan[7]);
            printf("virtual_gport     %u \n", pclientFlow->virtual_gport);
            printf("l2intf_id         %u \n", pclientFlow->l2intf_id);
            printf("flags             %u \n", pclientFlow->flags);
            printf("macLearnMax       %u \n", pclientFlow->macLearnMax);
            printf("onuId             %u \n", pclientFlow->onuId);
            printf("mask              %u \n", pclientFlow->mask);
            printf("maxChannels       %u \n", pclientFlow->maxChannels);

            pclientFlow = (struct ptin_evc_client_s *) dl_queue_get_next(&evcs[evc_id].intf[intf_idx].clients, (dl_queue_elem_t *) pclientFlow);
        }

    }

    return rc;
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
  ptin_intf_t ptin_intf;
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

    /* Only stacked services have clients */
    if (ptin_intf_port2ptintf(intf_idx,&ptin_intf)!=L7_SUCCESS)
      continue;

    /* Remove all clients/flows of this interface */
    if (ptin_evc_intfclientsflows_remove(evc_id, ptin_intf.intf_type, ptin_intf.intf_id)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error removing all clients/flows from intf=%u/%u",evc_id,ptin_intf.intf_type,ptin_intf.intf_id);
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
L7_RC_t ptin_evc_intfclientsflows_remove( L7_uint evc_id, L7_uint8 intf_type, L7_uint8 intf_id )
{
  L7_uint     intf_idx;
  ptin_intf_t ptin_intf;
  struct  ptin_evc_client_s *pclientFlow;
  ptin_HwEthEvcBridge_t     bridge;
  L7_RC_t                   res;
  L7_RC_t                   rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get intf_idx, and validate it */
  ptin_intf.intf_type = intf_type;
  ptin_intf.intf_id   = intf_id;
  if (ptin_intf_ptintf2port(&ptin_intf, &intf_idx)!=L7_SUCCESS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Interface %u/%u not valid!",intf_type,intf_id);
    return L7_FAILURE;
  }

  if (!evcs[evc_id].in_use || !evcs[evc_id].intf[intf_idx].in_use)
  {
    PT_LOG_WARN(LOG_CTX_EVC,"EVC %u not active, or interface %u/%u not valid!", evc_id, intf_type, intf_id);
    return L7_SUCCESS;
  }

  /* Only stacked services have clients */
  if (IS_EVC_STD(evc_id) && !IS_EVC_STACKED(evc_id))
  {
    PT_LOG_WARN(LOG_CTX_EVC,"EVC %u do not allow clients/flows!", evc_id);
    return L7_SUCCESS;
  }

  bridge.index            = evcs[evc_id].extended_id;
  bridge.intf.intf.format = PTIN_INTF_FORMAT_TYPEID;
  bridge.intf.intf.value.ptin_intf.intf_type = ptin_intf.intf_type;
  bridge.intf.intf.value.ptin_intf.intf_id   = ptin_intf.intf_id;
  bridge.intf.mef_type    = evcs[evc_id].intf[intf_idx].type;

  /* Get all clients */
  pclientFlow = L7_NULLPTR;
  while (/* dl_queue_remove_head */dl_queue_get_head(&evcs[evc_id].intf[intf_idx].clients, (dl_queue_elem_t **) &pclientFlow)==NOERR &&
         pclientFlow != L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_EVC,"Going to remove uni_ovid %u from intf %u/%u", pclientFlow->uni_ovid, intf_type, intf_id);

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
      res = ptin_evc_flow_unconfig(evc_id, intf_idx, pclientFlow->uni_ovid);
      if ( res != L7_SUCCESS )
      {
        PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error removing flow",evc_id);
        rc = L7_FAILURE;
      }
    }
    else
#endif
    {
      bridge.intf.vid = pclientFlow->uni_ovid;
      bridge.inn_vlan = pclientFlow->int_ivid;

      /* Remove this bridge (client) */
      if (ptin_evc_p2p_bridge_remove(&bridge)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error removing bridge for client of cvlan=%u attached to intf=%u/%u",
                evc_id, bridge.inn_vlan, bridge.intf.intf.value.ptin_intf.intf_type, bridge.intf.intf.value.ptin_intf.intf_id);
        rc = L7_FAILURE;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_EVC,"EVC #%u: Client of cvlan %u attached to intf=%u/%u removed",
                  evc_id, bridge.inn_vlan, bridge.intf.intf.value.ptin_intf.intf_type, bridge.intf.intf.value.ptin_intf.intf_id);
      }
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
L7_RC_t ptin_evc_client_remove( L7_uint evc_id, L7_uint8 intf_type, L7_uint8 intf_id, L7_uint cvlan )
{
  L7_uint     intf_idx;
  ptin_intf_t ptin_intf;
  ptin_HwEthEvcBridge_t bridge;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get intf_idx, and validate it */
  ptin_intf.intf_type = intf_type;
  ptin_intf.intf_id   = intf_id;
  if (ptin_intf_ptintf2port(&ptin_intf, &intf_idx)!=L7_SUCCESS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Interface %u/%u not valid!",intf_type,intf_id);
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
  if ( ptin_evc_client_clean(evc_id, ptin_intf.intf_type, ptin_intf.intf_id, cvlan, L7_TRUE) != L7_SUCCESS )
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning client of cvlan=%u attached to intf=%u/%u!",evc_id, cvlan, ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }

  /* Build struct to remove bridge */
  bridge.index            = evcs[evc_id].extended_id;
  bridge.intf.intf.format = PTIN_INTF_FORMAT_TYPEID;
  bridge.intf.intf.value.ptin_intf.intf_type = ptin_intf.intf_type;
  bridge.intf.intf.value.ptin_intf.intf_id   = ptin_intf.intf_id;
  bridge.intf.mef_type    = evcs[evc_id].intf[intf_idx].type;
  bridge.intf.vid         = 0;
  bridge.inn_vlan         = cvlan;

  /* Remove this bridge (client) */
  if (ptin_evc_p2p_bridge_remove(&bridge)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error removing bridge for client of cvlan=%u attached to intf=%u/%u",
            evc_id, bridge.inn_vlan, bridge.intf.intf.value.ptin_intf.intf_type, bridge.intf.intf.value.ptin_intf.intf_id);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_EVC,"EVC #%u: Client of cvlan %u attached to intf=%u/%u removed",
            evc_id, bridge.inn_vlan, bridge.intf.intf.value.ptin_intf.intf_type, bridge.intf.intf.value.ptin_intf.intf_id);

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
L7_RC_t ptin_evc_intf_clean_all( L7_uint evc_id, L7_uint8 intf_type, L7_uint8 intf_id, L7_BOOL force )
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
  if (ptin_evc_intf_clean(evc_id,intf_type,intf_id, force)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning interface %u/%u",evc_id,intf_type,intf_id);
    rc = L7_FAILURE;
  }

  /* Clean all clients profiles and counters */
  if (ptin_evc_intfclients_clean(evc_id,intf_type,intf_id, force)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning all clients of intf=%u/%u",evc_id,intf_type,intf_id);
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
  ptin_intf_t ptin_intf;
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

    if (ptin_intf_port2ptintf(intf_idx, &ptin_intf)!=L7_SUCCESS)
      continue;

    /* Clean interface */
    if (ptin_evc_intf_clean(evc_id, ptin_intf.intf_type, ptin_intf.intf_id, force)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning profiles and counters of intf=%u/%u",evc_id,ptin_intf.intf_type,ptin_intf.intf_id);
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
  ptin_intf_t ptin_intf;
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

    if (ptin_intf_port2ptintf(intf_idx,&ptin_intf)!=L7_SUCCESS)
      continue;

    /* Clean all clients of this interface */
    if (ptin_evc_intfclients_clean(evc_id, ptin_intf.intf_type, ptin_intf.intf_id, force)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning all clients from intf=%u/%u",evc_id,ptin_intf.intf_type,ptin_intf.intf_id);
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
L7_RC_t ptin_evc_intfclients_clean( L7_uint evc_id, L7_uint8 intf_type, L7_uint8 intf_id, L7_BOOL force )
{
  L7_uint     intf_idx;
  ptin_intf_t ptin_intf;
  struct  ptin_evc_client_s *pclientFlow;
  L7_RC_t     rc = L7_SUCCESS;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get intf_idx, and validate it */
  ptin_intf.intf_type = intf_type;
  ptin_intf.intf_id   = intf_id;
  if (ptin_intf_ptintf2port(&ptin_intf, &intf_idx)!=L7_SUCCESS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Interface %u/%u not valid!",intf_type,intf_id);
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
        PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning client intf=%u/%u,cvlan=%u",
                evc_id, ptin_intf.intf_type, ptin_intf.intf_id, pclientFlow->int_ivid);
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
L7_RC_t ptin_evc_intf_clean( L7_uint evc_id, L7_uint8 intf_type, L7_uint8 intf_id, L7_BOOL force )
{
  L7_uint     intf_idx;
  ptin_intf_t ptin_intf;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get intf_idx, and validate it */
  ptin_intf.intf_type = intf_type;
  ptin_intf.intf_id   = intf_id;
  if (ptin_intf_ptintf2port(&ptin_intf, &intf_idx)!=L7_SUCCESS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Interface %u/%u not valid!",intf_type,intf_id);
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
L7_RC_t ptin_evc_client_clean( L7_uint evc_id, L7_uint8 intf_type, L7_uint8 intf_id, L7_uint cvlan, L7_BOOL force )
{
  L7_uint     intf_idx;
  ptin_intf_t ptin_intf;
  struct ptin_evc_client_s *pclientFlow;

  /* Validate arguments */
  if (evc_id>=PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get intf_idx, and validate it */
  ptin_intf.intf_type = intf_type;
  ptin_intf.intf_id   = intf_id;
  if (ptin_intf_ptintf2port(&ptin_intf, &intf_idx)!=L7_SUCCESS || intf_idx>=PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Interface %u/%u not valid!",intf_type,intf_id);
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
    PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Client of cvlan=%u attached to intf=%u/%u not found!",evc_id, cvlan, ptin_intf.intf_type,ptin_intf.intf_id);
    return L7_FAILURE;
  }

  /* Clean client */
  if (ptin_evc_pclientFlow_clean(evc_id, intf_idx, pclientFlow, force)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC #%u: Error cleaning client of cvlan=%u attached to intf=%u/%u!",evc_id, cvlan, ptin_intf.intf_type,ptin_intf.intf_id);
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
    PT_LOG_DEBUG(LOG_CTX_EVC,"EVC unstacked Not Supported");
    return L7_NOT_SUPPORTED;
  }

  /* No client found? */
  if (pclient == L7_NULLPTR)
  {
    PT_LOG_DEBUG(LOG_CTX_EVC, "No client found");
    return L7_NOT_EXIST;
  }

  /* If current client was not provided, return first client */
  client_next = (clientFlow==L7_NULLPTR || clientFlow->int_ivid==0) ? pclient : pclient->next;

  /* No next client/flow? */
  if (client_next == L7_NULLPTR)
  {
    PT_LOG_DEBUG(LOG_CTX_EVC,"No next client/flow?");
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
    clientFlow_next->onuId    = client_next->onuId;
  }

  return L7_SUCCESS;
}

/**
 * Get next client, belonging to a vlan
 * 
 * @param intVid    : internal vlan
 * @param ptin_port : Port
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
L7_RC_t ptin_evc_vlan_client_next( L7_uint intVid, L7_uint32 ptin_port, ptin_HwEthEvcFlow_t *clientFlow, ptin_HwEthEvcFlow_t *clientFlow_next)
{
  L7_uint     evc_id;
  ptin_intf_t ptin_intf;

  /* Validate arguments */
  if (intVid>=4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (intVid=%u)",intVid);
    return L7_FAILURE;
  }

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

  /* Convert to ptin_intf format */
  if (ptin_intf_port2ptintf(ptin_port, &ptin_intf)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC %u: Error acquiring ptin_intf from ptin_port %u!",evc_id, ptin_port);
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
L7_RC_t ptin_evc_igmpInst_set(L7_uint32 evc_ext_id, L7_uint igmp_inst)
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
L7_RC_t ptin_evc_igmpInst_get(L7_uint32 evc_ext_id, L7_uint *igmp_inst)
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
L7_RC_t ptin_evc_dhcpInst_set(L7_uint32 evc_ext_id, L7_uint dhcp_inst)
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
L7_RC_t ptin_evc_dhcpInst_get(L7_uint32 evc_ext_id, L7_uint *dhcp_inst)
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
L7_RC_t ptin_evc_pppoeInst_set(L7_uint32 evc_ext_id, L7_uint pppoe_inst)
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
L7_RC_t ptin_evc_pppoeInst_get(L7_uint32 evc_ext_id, L7_uint *pppoe_inst)
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
    return L7_NO_RESOURCES;
  }

  /* Try to get an entry from the pool of free elements */
  rc = dl_queue_remove_head(&queue_free_evcs, (dl_queue_elem_t **) &evc_pool_entry);
  if (rc != NOERR) {
    PT_LOG_CRITIC(LOG_CTX_EVC, "There are no free EVCs available! rc=%d", rc);
    return L7_NO_RESOURCES;
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

#ifdef NGPON2_SUPPORTED
/**
 * Allocates an EVC entry from the pool
 * 
 * @author Rui Fernandes: rui-f-fernandes@telecom.pt  
 * 
 * @param evc_ext_id EVC extended index (input)
 * @param evc_id     Allocated index (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_offline_entry_remove(L7_uint32 evc_ext_id)
{
  ptinExtNGEvcIdInfoData_t  *ext_evcId_infoData;
  ptinExtNGEvcIdDataKey_t   ext_evcId_key;

  /* Key to search for */
  memset(&ext_evcId_key, 0x00, sizeof(ptinExtNGEvcIdDataKey_t));
  ext_evcId_key.ext_evcId = evc_ext_id;

  /* Search for this extended id */
  ext_evcId_infoData = (ptinExtNGEvcIdInfoData_t *) avlSearchLVL7( &(extNGEvcId_avlTree.extNGEvcIdAvlTree), (void *)&ext_evcId_key, AVL_EXACT);

  /* If already in use, removed from the AVL */
  if (ext_evcId_infoData != L7_NULLPTR)
  {  
    avlDeleteEntry(&(extNGEvcId_avlTree.extNGEvcIdAvlTree), (void *)&ext_evcId_key);
    PT_LOG_TRACE(LOG_CTX_EVC,"Offline EVC removed (ext_evc_id=%u)!",ext_evcId_key.ext_evcId);
  }
  
  return L7_SUCCESS;
}


/**
 * Removes an EVC entry from the offline pool
 * 
 * @author Rui Fernandes: rui-f-fernandes@telecom.pt  
 * 
 * @param evc_ext_id EVC extended index (input)
 * @param evc_id     Allocated index (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evc_offline_entry_add(ptin_HwEthMef10Evc_t *EvcConf)
{
  ptinExtNGEvcIdInfoData_t  *ext_evcId_infoData;
  ptinExtNGEvcIdDataKey_t   ext_evcId_key;

  /* Key to search for */
  memset(&ext_evcId_key, 0x00, sizeof(ptinExtNGEvcIdDataKey_t));
  ext_evcId_key.ext_evcId = EvcConf->index;

  /* Search for this extended id */
  ext_evcId_infoData = (ptinExtNGEvcIdInfoData_t *) avlSearchLVL7( &(extNGEvcId_avlTree.extNGEvcIdAvlTree), (void *)&ext_evcId_key, AVL_EXACT);
  /* If already in use, return its (internal) evc_id */
  if (ext_evcId_infoData != L7_NULLPTR)
  {
    if (ext_evcId_infoData->extNGEvcIdDataKey.ext_evcId > PTIN_SYSTEM_N_EXTENDED_EVCS)
    {

      PT_LOG_WARN(LOG_CTX_EVC,"Invalid node in AVL TREE eith ext_evc_id = %u", ext_evcId_infoData->extNGEvcIdDataKey.ext_evcId );
      /* Nothing to do */
      return L7_SUCCESS;
    }
    /* This node is not valid... delete it */
    else
    {
      if (avlDeleteEntry(&(extNGEvcId_avlTree.extNGEvcIdAvlTree), (void *)&ext_evcId_key) == L7_NULLPTR)
      {
        return L7_FAILURE;
      }
      PT_LOG_WARN(LOG_CTX_EVC,"Cleaned AVL TREE entry");
    }
  }
  /* check if there is free space in AVL TREE */
  if (extNGEvcId_avlTree.extNGEvcIdAvlTree.count > MAX_NETWORK_SERVICES)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"No space in AVL TREE to add one more node");
    return L7_FAILURE;
  }

  /* Allocate new node in AVL Tree */
  if (avlInsertEntry(&(extNGEvcId_avlTree.extNGEvcIdAvlTree), (void *)&ext_evcId_key) != L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"No space in AVL TREE to add one more node");
    return L7_FAILURE;
  }

  /* Search for the newly created node */
  ext_evcId_infoData = (ptinExtNGEvcIdInfoData_t *) avlSearchLVL7( &(extNGEvcId_avlTree.extNGEvcIdAvlTree), (void *)&ext_evcId_key, AVL_EXACT);

  /* If already in use, return its (internal) evc_id */
  if (ext_evcId_infoData == L7_NULLPTR)
  {
    PT_LOG_CRITIC(LOG_CTX_EVC,"Something is wrong... new created node, is not found (ext_evc_id=%u)!",ext_evcId_key.ext_evcId);
    return L7_FAILURE;
  }
  /* Fill remaining data into AVL node */

  ext_evcId_infoData->evcNgpon2.evc_type         = EvcConf->evc_type;
  ext_evcId_infoData->evcNgpon2.flags            = EvcConf->flags;
  ext_evcId_infoData->evcNgpon2.index            = EvcConf->index;
  ext_evcId_infoData->evcNgpon2.internal_vlan    = EvcConf->internal_vlan;
  ext_evcId_infoData->evcNgpon2.n_intf           = EvcConf->n_intf;
  ext_evcId_infoData->evcNgpon2.n_clientflows    = EvcConf->n_clientflows;
  ext_evcId_infoData->evcNgpon2.mc_flood         = EvcConf->mc_flood;

  int i;

  for (i=0; i < EvcConf->n_intf ;i++)
  {
    memcpy(&ext_evcId_infoData->evcNgpon2.intf[i], &EvcConf->intf[i], sizeof(EvcConf->intf[i]));
  }

  return L7_SUCCESS;
}

#endif
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
 *                    L7_NOT_EXIST(EVC do not exist)  
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
    return L7_NOT_EXIST;
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

  PT_LOG_TRACE(LOG_CTX_EVC, "internal EVC id = %u", ext_evcId_infoData->evc_id);

  return L7_SUCCESS;
}

/**
 * Get L3 Intf Id of EVC Port
 *  
 * @author melo (01/06/2015) 
 * 
 * @param evc_ext_id 
 * @param ptin_port 
 * @param l3_intf_id 
 * 
 * @return L7_RC_t 
 *  
 * @notes Disabled the creation of L3 egress ports on  Multicast
 *        Services!!!
 */
L7_RC_t ptin_evc_l3_intf_get(L7_uint32 evc_ext_id, L7_uint32 ptin_port, L7_int *l3_intf_id)
{
  L7_uint32 evc_id = (L7_uint32) -1;

  /* Validate arguments */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS || ptin_port >= PTIN_SYSTEM_N_INTERF || l3_intf_id == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid Parameters [eEVC:%u ptin_port:%u l3_intf_id:%p]", evc_ext_id, ptin_port, l3_intf_id);
    return L7_FAILURE;
  }

  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS || evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Failed to Obtain Internal Id [evc_ext_id:0x%x, evc_id:%u]", evc_ext_id, evc_id);
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

  if (evcs[evc_id].intf[ptin_port].in_use == L7_FALSE)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"ptin port %u is not in use on evc id %u", ptin_port, evc_ext_id);
    return L7_FAILURE;
  }

  if (!IS_EVC_IPTV(evc_id)) 
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC Id %u is not IPTV", evc_ext_id);
    return L7_FAILURE;
  }

  if (evcs[evc_id].intf[ptin_port].type != PTIN_EVC_INTF_LEAF)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"ptin port %u type:%u different from leaf :%u", ptin_port, evcs[evc_id].intf[ptin_port].type, PTIN_EVC_INTF_LEAF);
    return L7_FAILURE;
  }

  *l3_intf_id = evcs[evc_id].intf[ptin_port].l3_intf_id;
  return L7_SUCCESS;
}

/**
 * Get Multicast Replication Table of an EVC 
 *  
 * @author melo (01/06/2015) 
 * 
 * @param evc_ext_id 
 * @param multicast_group 
 * 
 * @return L7_RC_t 
 *  
 * @notes Disabled the creation of Multicast Replication tables 
 *        on Multicast Services!!!
 */
L7_RC_t ptin_evc_l3_multicast_group_get(L7_uint32 evc_ext_id, L7_int *multicast_group)
{
  L7_uint32   evc_id = (L7_uint32) -1;

  /* Validate arguments */
  if (evc_ext_id >= PTIN_SYSTEM_N_EXTENDED_EVCS || multicast_group == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Invalid Parameters [eEVC:%u multicast_group:%p]", evc_ext_id, multicast_group);
    return L7_FAILURE;
  }

  if (ptin_evc_ext2int(evc_ext_id, &evc_id) != L7_SUCCESS || evc_id >= PTIN_SYSTEM_N_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Failed to Obtain Internal Id [evc_ext_id:0x%x, evc_id:%u]", evc_ext_id, evc_id);
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

  *multicast_group = evcs[evc_id].mcgroup;
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
  L7_RC_t                  rc;

  /* Validate given extended evc id, and get pointer to AVL node */
  rc = ptin_evc_extEvcInfo_get(evc_ext_id, &ext_evcId_infoData);  
  if  (rc != L7_SUCCESS)
  {
    return rc;
  }

  if ( ext_evcId_infoData == L7_NULLPTR )
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

  /* Release free vlan queue */
  ptin_evc_freeVlanQueue_free(evcs[evc_id].queue_free_vlans);

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
  L7_BOOL            is_p2p, is_quattro, is_stacked;
  L7_BOOL            is_root;
  L7_BOOL            mac_learning;
  L7_BOOL            cpu_trap;
  L7_BOOL            iptv_flag;
  L7_uint16          int_vlan;
  L7_uint16          root_vlan;
  ptin_intf_t        intf;
  L7_uint32          ptin_port, intIfNum;
  ptin_HwEthMef10Intf_t intf_vlan;
  #if defined IGMP_SMART_MC_EVC_SUPPORTED
  ptin_dtl_l3_intf_t l3_intf;
  #endif
  L7_RC_t            rc = L7_SUCCESS;
  L7_int             queue_type;
  queue_type = evcs[evc_id].queue_type;

  /* Correct params */
  if (intf_cfg->vid_inner >= 4096)
  {
    intf_cfg->vid_inner = 0;
  }

  /* Convert Phy/Lag# into PTin Intf index */

  ptin_port = intf_cfg->intf.value.ptin_port;
  intIfNum  = intf_cfg->intf.value.intIfNum;

  is_p2p       = (evcs[evc_id].flags & PTIN_EVC_MASK_P2P        ) == PTIN_EVC_MASK_P2P;
  is_quattro   = (evcs[evc_id].flags & PTIN_EVC_MASK_QUATTRO    ) == PTIN_EVC_MASK_QUATTRO;
  is_stacked   = (evcs[evc_id].flags & PTIN_EVC_MASK_STACKED    ) == PTIN_EVC_MASK_STACKED;
  mac_learning = (evcs[evc_id].flags & PTIN_EVC_MASK_MACLEARNING) == PTIN_EVC_MASK_MACLEARNING;
  cpu_trap     = (evcs[evc_id].flags & PTIN_EVC_MASK_CPU_TRAPPING)== PTIN_EVC_MASK_CPU_TRAPPING;
  iptv_flag    = (evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV)     == PTIN_EVC_MASK_MC_IPTV;
  is_root      =  intf_cfg->mef_type == PTIN_EVC_INTF_ROOT;
  root_vlan    =  evcs[evc_id].rvlan;

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: adding %s# %02u (MEF %s) with VLANs %u + %u...",
            evc_id,
            ptin_port < PTIN_SYSTEM_N_PORTS ? "PHY":"LAG",
            ptin_port < PTIN_SYSTEM_N_PORTS ? ptin_port : ptin_port - PTIN_SYSTEM_N_PORTS,
            is_root ? "Root":"Leaf",
            intf_cfg->vid, intf_cfg->vid_inner);

  /* Data to be used to add interface */
  intf_vlan = *intf_cfg;
  /* Unstacked services do not make use of inner VLAN */
  if (!is_stacked)
  {
    intf_vlan.vid_inner = 0;
  }

  if (is_root)
  {
    L7_uint8 egress_del_ivlan = L7_FALSE;

    int_vlan = root_vlan;   /* Vroot is the same for all the root interfaces */

  #if (PTIN_BOARD_IS_GPON)
    /* For uplink interfaces, do not delete inner vlan */
    if (intf_vlan.intf.value.ptin_port >= PTIN_SYSTEM_N_PONS
    #if (PTIN_BOARD_IS_STANDALONE)
        && (intf_vlan.intf.value.ptin_port < PTIN_SYSTEM_N_CLIENT_PORTS ||   /* Physical ports */
            intf_vlan.intf.value.ptin_port >= PTIN_SYSTEM_N_PORTS)            /* LAGs */
    #endif
       )
    {
#if (PTIN_BOARD == PTIN_BOARD_TC16SXG) 
      /* The Aspen does not add c-uni tag on
         Unicast unstacked service, removing the inner vlan
         on this situations removed paylod VLAN's*/
        if ((evcs[evc_id].flags == PTIN_EVC_MASK_MACLEARNING) && 
            !is_stacked) 
        {
            egress_del_ivlan = 0;
        }
        else
        {
            egress_del_ivlan = !is_stacked;
        }
#else
      egress_del_ivlan = !is_stacked;
#endif
    }
    /* For Unstacked MAC-Bridge services - @downstream direction - a new TAG will be added as new outer VLAN */
    /* This way, payload data goes after the second tag, and will be preserved to the ONT */
    if (is_quattro)
    {
      if (!is_stacked)
      {
        /* Push new Outer VLAN is not supported for Trident3x3 */
#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
        intf_vlan.action_outer = PTIN_XLATE_ACTION_REPLACE;
#else
        intf_vlan.action_outer = PTIN_XLATE_ACTION_ADD;
#endif
        intf_vlan.action_inner = PTIN_XLATE_ACTION_NONE;
      }
      else
      {
        intf_vlan.action_inner = PTIN_XLATE_ACTION_REPLACE;
      }
    }
  #endif
    
    rc = switching_root_add(&intf_vlan,                                 /* Input data */
                            int_vlan,                                   /* Internal vlan */
                            0,                                          /* New inner vlan */
                            egress_del_ivlan,                           /* Delete egress vlan? Only for unstacked EVCs */
                            -1);                                        /* Force PCP */

    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error adding root interface [ptin_port=%u Vs=%u Vi=%u Vr=%u]",
              evc_id, ptin_port, intf_cfg->vid, intf_cfg->vid_inner, int_vlan);
      return rc;
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
    if (!IS_EVC_ETREE(evc_id))
    {
      int_vlan = evcs[evc_id].rvlan;     /* Internal VLAN is the same for all interfaces, including leafs */
    }
    else
    {
    #if (!PTIN_BOARD_IS_MATRIX)
      ptin_evc_vlan_allocate(&int_vlan, evcs[evc_id].queue_free_vlans, evc_id); /* One VLAN for each unstacked leaf */
    #else
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: E-trees are not allowed for Matrix board", evc_id);
      return L7_FAILURE;
    #endif
    }

    rc = switching_leaf_add(ptin_port, int_vlan);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error adding leaf [ptin_port=%u Vl=%u]",
              evc_id, ptin_port, int_vlan);
      ptin_evc_vlan_free(int_vlan, evcs[evc_id].queue_free_vlans);       /* free VLAN */
      return rc;
    }
    evcs[evc_id].n_leafs++;

    /* Data to be used to add interface */
    intf_vlan = *intf_cfg;

    /* Add translations for leaf ports */
    #if ( PTIN_BOARD_IS_MATRIX )
    if (is_p2p || !IS_EVC_ETREE(evc_id))
    #else
    if (!IS_EVC_ETREE(evc_id) && !is_stacked && !is_quattro && (intf_vlan.vid >= 1 && intf_vlan.vid <= 4095))
    #endif
    {
      /* Only configure MC EVC partially if we are not at MX */
      #if ( !PTIN_BOARD_IS_MATRIX )
      if (iptv_flag)
      {
        rc = switching_mcevc_leaf_add(&intf_vlan, int_vlan);
      }
      else
      #endif
      {
        /* Leaf interfaces of unstacked/quattro services do not use inner VLAN */
        //intf_vlan.vid_inner = 0;

        /* Do not remove inner vlan @ egress */
        rc = switching_elan_leaf_add(&intf_vlan, int_vlan, L7_FALSE, -1);
      }

      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Error adding translations for leaf interface %u (rc=%d)",ptin_port, rc);
        return rc;
      }
    }
        
    #if defined IGMP_SMART_MC_EVC_SUPPORTED
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
        return rc;
      }
      PT_LOG_TRACE(LOG_CTX_EVC, "Added L3 Leaf Interface [ptin_port:%u l3_intf_id:%d]", ptin_port, l3_intf.l3_intf_id);      
            
      #if 0 /* IGMP_SMART_MC_EVC_SUPPORTED / SFR ?? */
      rc = ptin_multicast_l3_egress_port_add(intIfNum, evcs[evc_id].multicast_group, l3_intf.l3_intf_id);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Error adding Egress Port to Multicast Group [ptin_port:%u l3_intf_id:%d mc_group:0x%x rc:%d]",ptin_port, l3_intf.l3_intf_id, evcs[evc_id].multicast_group, rc);
        return L7_FAILURE;
      }
      PT_LOG_TRACE(LOG_CTX_EVC, "Egress Port Added to Multicast Group [ptin_port:%u l3_intf_id:%d mc_group:0x%x]", ptin_port, l3_intf.l3_intf_id, evcs[evc_id].multicast_group);            
      #endif
    }
    #endif
  }

  /* Vlan mode configuration: Only for E-TREEs configuration */
  if (IS_EVC_ETREE(evc_id))
  {
      rc = switching_vlan_config(int_vlan, root_vlan, mac_learning, evcs[evc_id].mc_flood, cpu_trap, queue_type);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error configuring VLAN %u [FwdVlan=%u MACLearning=%u MCFlood=%u]",
              evc_id, int_vlan, root_vlan, mac_learning, evcs[evc_id].mc_flood);
      return rc;
    }
  }

  evcs_intfs_in_use[ptin_port]++; /* Add this interface to the list of members in use */

  /* Update intf struct (common info) */
  evcs[evc_id].intf[ptin_port].in_use   = L7_TRUE;
  evcs[evc_id].intf[ptin_port].type     = intf_cfg->mef_type;
  evcs[evc_id].intf[ptin_port].int_vlan = int_vlan;

  evcs[evc_id].intf[ptin_port].l3_intf_id = -1;

  #if defined IGMP_SMART_MC_EVC_SUPPORTED
  if (iptv_flag)
  {
    evcs[evc_id].intf[ptin_port].l3_intf_id = l3_intf.l3_intf_id;
  }
  #endif

  #ifdef PTIN_ERPS_EVC
  evcs[evc_id].intf[ptin_port].portState = PTIN_EVC_PORT_FORWARDING;
  #endif
  #if ( !PTIN_BOARD_IS_MATRIX )
  PT_LOG_TRACE(LOG_CTX_EVC, "...");
  if (is_stacked && (intf_cfg->mef_type == PTIN_EVC_INTF_LEAF))
  {
    PT_LOG_TRACE(LOG_CTX_EVC, "vid %u -> 0xFFFF, vid_inner %u -> 0", intf_cfg->vid, intf_cfg->vid_inner);
    evcs[evc_id].intf[ptin_port].out_vlan   = 0xFFFF;  /* on stacked EVCs, leafs out.vid is defined per client and not per interface */
    evcs[evc_id].intf[ptin_port].inner_vlan = 0;
  }
  else
  #endif
  {
    PT_LOG_TRACE(LOG_CTX_EVC, "VLANs %u + %u (%u + %u)", intf_vlan.vid, intf_vlan.vid_inner, intf_cfg->vid, intf_cfg->vid_inner);
    evcs[evc_id].intf[ptin_port].out_vlan   = intf_cfg->vid;
    evcs[evc_id].intf[ptin_port].inner_vlan = intf_cfg->vid_inner;
  }

  evcs[evc_id].intf[ptin_port].action_outer_vlan = (IS_VLAN_VALID(intf_cfg->vid))       ? intf_cfg->action_outer : PTIN_XLATE_ACTION_MAX;
  evcs[evc_id].intf[ptin_port].action_inner_vlan = (IS_VLAN_VALID(intf_cfg->vid_inner)) ? intf_cfg->action_inner : PTIN_XLATE_ACTION_MAX;

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

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: %s# %02u (MEF %s Out.VID=%04hu Int.VID=%04hu) successfully added",
            evc_id,
            ptin_port < PTIN_SYSTEM_N_PORTS ? "PHY":"LAG",
            ptin_port < PTIN_SYSTEM_N_PORTS ? ptin_port : ptin_port - PTIN_SYSTEM_N_PORTS,
            evcs[evc_id].intf[ptin_port].type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf",
            evcs[evc_id].intf[ptin_port].out_vlan,
            evcs[evc_id].intf[ptin_port].int_vlan);

  return L7_SUCCESS;
}

/**
 * Removes an interface configuration (translation entries)
 * NOTE: the intf entry is removed from EVC struct 
 * 
 * @param evc_id EVC #
 * @param ptin_port Interface #
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_evc_intf_remove(L7_uint evc_id, L7_uint ptin_port)
{
  L7_BOOL is_p2p, is_quattro, is_stacked, iptv_flag;
  L7_BOOL            is_root, cpu_trap;
  L7_uint16          out_vlan;
  L7_uint16          inn_vlan;
  L7_uint16          int_vlan;
  ptin_intf_t        intf;
  L7_uint32          intIfNum;
  #if defined IGMP_SMART_MC_EVC_SUPPORTED
  ptin_dtl_l3_intf_t l3_intf;
  #endif
  L7_RC_t            rc;
  L7_uint8           remove_from_hw = TRUE;

  is_p2p     = (evcs[evc_id].flags & PTIN_EVC_MASK_P2P    ) == PTIN_EVC_MASK_P2P;
  is_quattro = (evcs[evc_id].flags & PTIN_EVC_MASK_QUATTRO) == PTIN_EVC_MASK_QUATTRO;
  is_stacked = (evcs[evc_id].flags & PTIN_EVC_MASK_STACKED) == PTIN_EVC_MASK_STACKED;
  iptv_flag  = (evcs[evc_id].flags & PTIN_EVC_MASK_MC_IPTV) == PTIN_EVC_MASK_MC_IPTV;
  cpu_trap   = (evcs[evc_id].flags & PTIN_EVC_MASK_CPU_TRAPPING) == PTIN_EVC_MASK_CPU_TRAPPING;
  is_root    = evcs[evc_id].intf[ptin_port].type == PTIN_EVC_INTF_ROOT;
  out_vlan   = evcs[evc_id].intf[ptin_port].out_vlan;
  inn_vlan   = evcs[evc_id].intf[ptin_port].inner_vlan;
  int_vlan   = evcs[evc_id].intf[ptin_port].int_vlan;

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: removing %s# %02u (MEF %s Out.VID=%04u Inn.VID=%04u Int.VID=%04u) ...",
            evc_id,
            ptin_port < PTIN_SYSTEM_N_PORTS ? "PHY":"LAG",
            ptin_port < PTIN_SYSTEM_N_PORTS ? ptin_port : ptin_port - PTIN_SYSTEM_N_PORTS,
            evcs[evc_id].intf[ptin_port].type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf",
            evcs[evc_id].intf[ptin_port].out_vlan,
            evcs[evc_id].intf[ptin_port].inner_vlan,
            evcs[evc_id].intf[ptin_port].int_vlan);

  /* Get intIfNum */
  if (ptin_intf_port2intIfNum(ptin_port, &intIfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error converting port %u to intIfNum", evc_id, ptin_port);
    return L7_FAILURE;
  }

  if (is_root)
  {
    int_vlan = evcs[evc_id].rvlan;

    rc = switching_root_remove(ptin_port, out_vlan, ((is_stacked) ? inn_vlan : 0), int_vlan);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing root interface [ptin_port=%u Vs=%u Vi=%u Vr=%u]",
              evc_id, ptin_port, out_vlan, inn_vlan, int_vlan);
      return rc;
    }
    evcs[evc_id].n_roots--;
  }
  else
  {
#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
    L7_uint32 aux_port;

    /* check with the other virtual port is configured on EVC.
       If yes, do not remove the correspont physical port from HW*/
    if (ptin_port >= PTIN_SYSTEM_N_PONS_PHYSICAL)
    {
      aux_port = ptin_port - PTIN_SYSTEM_N_PONS_PHYSICAL;
    }
    else
    {
      aux_port = ptin_port + PTIN_SYSTEM_N_PONS_PHYSICAL;
    }

    if (aux_port >= 0 &&
        aux_port < PTIN_SYSTEM_N_INTERF)
    {
      remove_from_hw = !(evcs[evc_id].intf[aux_port].in_use);
    }
    else
    {
      PT_LOG_WARN(LOG_CTX_EVC, "EVC# %u: aux_port %d out of range ", evc_id, aux_port);
      remove_from_hw = TRUE;
    }
#endif

    PT_LOG_INFO(LOG_CTX_EVC, "EVC# %u: ptin_port %d configurations %s removed from HW ",
                evc_id, ptin_port, remove_from_hw ? "can be" : "can't be");

    if (iptv_flag)
    {
      rc = ptin_igmp_mgmd_port_remove(evcs[evc_id].extended_id, ptin_port);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing Mgmd Port [extended_id=%u ptin_port=%u]",
                   evc_id, evcs[evc_id].extended_id, ptin_port);
  //    return rc;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_EVC, "Removed Mgmd Port [extended_id:%u ptin_port=%u]", evcs[evc_id].extended_id, ptin_port);
      }
    }

    if (remove_from_hw)
    {
      rc = switching_leaf_remove(ptin_port, int_vlan, iptv_flag);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing leaf [ptin_port=%u Vl=%u]",
                evc_id, ptin_port, int_vlan);
        return rc;
      }
    }
    evcs[evc_id].n_leafs--;

    #if ( PTIN_BOARD_IS_MATRIX )
    if (is_p2p || !IS_EVC_ETREE(evc_id))
    #else
    if (!IS_EVC_ETREE(evc_id) && !is_stacked && !is_quattro && (out_vlan >= 1 && out_vlan <= 4095))
    #endif
    {
      /* Add translations for leaf ports, only if we are in matrix board */

      /* Only configure MC EVC partially if we are not at MX */
      #if ( !PTIN_BOARD_IS_MATRIX )
      if (iptv_flag && remove_from_hw)
      {
        rc = switching_mcevc_leaf_remove(ptin_port, out_vlan, inn_vlan, int_vlan);
      }
      else
      #endif
      {
        rc = switching_elan_leaf_remove(ptin_port, out_vlan, inn_vlan, int_vlan, L7_FALSE /*Don't touch the inner vlan at egress*/);
      }

      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Error removing translations for leaf interface %u (rc=%d)",ptin_port, rc);
        return rc;
      }
    }

    /* Remove vlan */
    if (IS_EVC_ETREE(evc_id))
    {
      ptin_evc_vlan_free(int_vlan, evcs[evc_id].queue_free_vlans); /* free VLAN */
    }
    
    #if defined IGMP_SMART_MC_EVC_SUPPORTED
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
        
        #if 0 /* IGMP_SMART_MC_EVC_SUPPORTED / SFR ?? */
        rc = ptin_multicast_l3_egress_port_remove(intIfNum, evcs[evc_id].multicast_group, l3_intf.l3_intf_id);

        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "Error removing Egress Port from Multicast Group [ptin_port:%u l3_intf_id:%d mc_group:0x%x rc:%d]",ptin_port, l3_intf.l3_intf_id, evcs[evc_id].multicast_group, rc);
          return L7_FAILURE;
        }
        PT_LOG_TRACE(LOG_CTX_EVC, "Egress Port Removed from Multicast Group [ptin_port:%u l3_intf_id:%d mc_group:0x%x]", ptin_port, l3_intf.l3_intf_id, evcs[evc_id].multicast_group);    
        #endif

        /*Remove L3 Leaf Interface*/
        rc = dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_L3_INTF, DAPI_CMD_CLEAR, sizeof(ptin_dtl_l3_intf_t), &l3_intf);

        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_EVC, "Error removing L3 leaf interface [ptin_port:%u rc:%d]",ptin_port, rc);
          return rc;
        }
        PT_LOG_TRACE(LOG_CTX_EVC, "Removed L3 Leaf Interface [ptin_port:%u l3_intf_id:%d]", ptin_port, l3_intf.l3_intf_id);      
      }
      else
      {
        PT_LOG_ERR(LOG_CTX_EVC, "Invalid L3 Intf Id:%u", l3_intf.l3_intf_id);
      }
      evcs[evc_id].intf[ptin_port].l3_intf_id = PTIN_HAPI_BROAD_INVALID_L3_INTF_ID;
    }
    #endif
  }

  /* Update snooping configuration */
  if (ptin_intf_port2ptintf(ptin_port,&intf)==L7_SUCCESS)
  {
    if ( ptin_igmp_is_evc_used(evcs[evc_id].extended_id)
    #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
         || IS_EVC_IGMP(evc_id)
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

  PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: %s# %02u (MEF %s Out.VID=%04u Inn.VID=%04u Int.VID=%04u) successfully removed",
            evc_id,
            ptin_port < PTIN_SYSTEM_N_PORTS ? "PHY":"LAG",
            ptin_port < PTIN_SYSTEM_N_PORTS ? ptin_port : ptin_port - PTIN_SYSTEM_N_PORTS,
            evcs[evc_id].intf[ptin_port].type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf",
            evcs[evc_id].intf[ptin_port].out_vlan,
            evcs[evc_id].intf[ptin_port].inner_vlan,
            evcs[evc_id].intf[ptin_port].int_vlan);

  evcs_intfs_in_use[ptin_port]--; /* Remove this interface from the list of members in use */

  /* Clear intf struct (common info) */
  evcs[evc_id].intf[ptin_port].in_use     = L7_FALSE;
  evcs[evc_id].intf[ptin_port].type       = 0;
  evcs[evc_id].intf[ptin_port].out_vlan   = 0;
  evcs[evc_id].intf[ptin_port].inner_vlan = 0;
  evcs[evc_id].intf[ptin_port].int_vlan   = 0;
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
  L7_RC_t rc = L7_SUCCESS;

  /* Remove all configured interfaces */
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
  {
    if (!evcs[evc_id].intf[i].in_use)
      continue;

    if (ptin_evc_intf_remove(evc_id, i) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: failed to remove intf# %u", evc_id, i);
      rc = L7_FAILURE;
    }
  }

  return rc;
}


/**
 * Adds translation entries to the leaf interface and also to the root ports 
 * in order to create an E-Tree
 * 
 * @param evc_id    EVC #
 * @param leaf_intf  Leaf interface #
 * @param roots_intf Root interfaces list
 * @param n_roots    Nr of root intf present in the roots_intf array
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_evc_etree_intf_add(L7_uint evc_id, L7_uint ptin_port)
{
  L7_uint   l, r;
  L7_uint   intf_list[PTIN_SYSTEM_N_INTERF];
  L7_uint   n_intf;
  L7_BOOL   egress_del_ivid;
  L7_RC_t   rc = L7_SUCCESS;

  #if (PTIN_BOARD_IS_GPON)
  egress_del_ivid = ((evcs[evc_id].flags & PTIN_EVC_MASK_STACKED) != PTIN_EVC_MASK_STACKED);
  #else
  egress_del_ivid = L7_FALSE;
  #endif

  /* Root intf ? */
  if (evcs[evc_id].intf[ptin_port].type == PTIN_EVC_INTF_ROOT)
  {
    /* Get all leaf interfaces... */
    ptin_evc_intf_list_get(evc_id, PTIN_EVC_INTF_LEAF, intf_list, &n_intf);

    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: configuring %u leafs on root intf# %02u (Out.VID=%04u) ...",
              evc_id,
              n_intf,
              ptin_port,
              evcs[evc_id].intf[ptin_port].out_vlan);

    /* Add all leaf xlate entries on the root port */
    for (l=0; l<n_intf; l++)
    {
      rc = switching_etree_root_add(ptin_port,                                    /* Root intf */
                                    evcs[evc_id].intf[ptin_port].out_vlan,       /* Vs */
                                    evcs[evc_id].intf[intf_list[l]].int_vlan,    /* Vl */
                                    egress_del_ivid );

      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error adding to root a bridge leaf entry [root_intf=%u Vs=%u Vl=%u]",
                evc_id, ptin_port, evcs[evc_id].intf[ptin_port].out_vlan, evcs[evc_id].intf[intf_list[l]].int_vlan);
        break;
      }
    }
  }
  /* Leaf... */
  else
  {
    /* Get all root interfaces... */
    ptin_evc_intf_list_get(evc_id, PTIN_EVC_INTF_ROOT, intf_list, &n_intf);

    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: configuring leaf intf# %u (Int.VID=%04u) on %u root interfaces...",
              evc_id,
              ptin_port,
              evcs[evc_id].intf[ptin_port].int_vlan,
              n_intf);

    /* Add leaf xlate entry on all root ports */
    for (r=0; r<n_intf; r++)
    {
      rc = switching_etree_root_add(intf_list[r],                              /* Root intf */
                                    evcs[evc_id].intf[intf_list[r]].out_vlan,    /* Vs */
                                    evcs[evc_id].intf[ptin_port].int_vlan,       /* Vl */
                                    egress_del_ivid );

      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error adding to root a bridge leaf entry [root_intf=%u Vs=%u Vl=%u]",
                evc_id, intf_list[r], evcs[evc_id].intf[intf_list[r]].out_vlan, evcs[evc_id].intf[ptin_port].int_vlan);
        break;
      }
    }

    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: configuring leaf intf# %02u (Out.VID=%04u Int.VID=%04u) on root Int.VLAN=%04u",
              evc_id,
              ptin_port,
              evcs[evc_id].intf[ptin_port].out_vlan,
              evcs[evc_id].intf[ptin_port].int_vlan,
              evcs[evc_id].rvlan);

    /* Finally add the bridge leaf entry */
    rc = switching_etree_leaf_add(ptin_port,                                      /* Leaf intf */
                                  evcs[evc_id].rvlan);                           /* Vr */
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error adding leaf entry [leaf_intf=%u Vr=%u]",
              evc_id, ptin_port, evcs[evc_id].rvlan);
    }
    /* Only for unstacked services */
    if ( !(evcs[evc_id].flags & PTIN_EVC_MASK_STACKED) )
    {
      rc = switching_etree_unstacked_leaf_add(ptin_port,                                      /* Leaf intf */
                                              evcs[evc_id].intf[ptin_port].out_vlan,         /* Vs' */
                                              evcs[evc_id].intf[ptin_port].int_vlan,         /* Vl */
                                              evcs[evc_id].rvlan);                           /* Vr */
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error adding unstacked leaf entry [leaf_intf=%u Vs'=%u Vl=%u Vr=%u]",
                evc_id, ptin_port, evcs[evc_id].intf[ptin_port].out_vlan,
                evcs[evc_id].intf[ptin_port].int_vlan, evcs[evc_id].rvlan);
      }
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
 * @param evc_id   EVC #
 * @param ptin_port Interface #
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_evc_etree_intf_remove(L7_uint evc_id, L7_uint ptin_port)
{
  L7_uint   l, r;
  L7_uint   intf_list[PTIN_SYSTEM_N_INTERF];
  L7_uint   n_intf;
  L7_RC_t   rc = L7_SUCCESS;

  /* Root intf ? */
  if (evcs[evc_id].intf[ptin_port].type == PTIN_EVC_INTF_ROOT)
  {
    /* Get all leaf interfaces... */
    ptin_evc_intf_list_get(evc_id, PTIN_EVC_INTF_LEAF, intf_list, &n_intf);

    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: removing %u leafs of root intf# %02u (Out.VID=%04u) ...",
              evc_id,
              n_intf,
              ptin_port,
              evcs[evc_id].intf[ptin_port].out_vlan);

    /* Remove all leaf xlate entries on the root port */
    for (l=0; l<n_intf; l++)
    {
      rc = switching_etree_root_remove(ptin_port,                                   /* Root intf */
                                       evcs[evc_id].intf[ptin_port].out_vlan,      /* Vs */
                                       evcs[evc_id].intf[intf_list[l]].int_vlan);  /* Vl */

      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing from root a bridge leaf entry [root_intf=%u Vs=%u Vl=%u]",
                evc_id, ptin_port, evcs[evc_id].intf[ptin_port].out_vlan, evcs[evc_id].intf[intf_list[l]].int_vlan);
        break;
      }
    }
  }
  /* Leaf... */
  else
  {
    /* Get all root interfaces... */
    ptin_evc_intf_list_get(evc_id, PTIN_EVC_INTF_ROOT, intf_list, &n_intf);

    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: removing leaf intf# %u (Int.VID=%04u) of %u root interfaces...",
              evc_id,
              ptin_port,
              evcs[evc_id].intf[ptin_port].int_vlan,
              n_intf);

    /* Remove leaf xlate entry on all root ports */
    for (r=0; r<n_intf; r++)
    {
      rc = switching_etree_root_remove(intf_list[r],                                /* Root intf */
                                       evcs[evc_id].intf[intf_list[r]].out_vlan,   /* Vs */
                                       evcs[evc_id].intf[ptin_port].int_vlan);     /* Vl */

      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing from root a bridge leaf entry [root_intf=%u Vs=%u Vl=%u]",
                evc_id, intf_list[r], evcs[evc_id].intf[intf_list[r]].out_vlan, evcs[evc_id].intf[ptin_port].int_vlan);
        break;
      }
    }

    PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: removing leaf intf# %02u (Out.VID=%04u Int.VID=%04u) of root Int.VLAN=%04u",
              evc_id,
              ptin_port,
              evcs[evc_id].intf[ptin_port].out_vlan,
              evcs[evc_id].intf[ptin_port].int_vlan,
              evcs[evc_id].rvlan);

    /* Finally remove the bridge leaf entry */
    if ( !(evcs[evc_id].flags & PTIN_EVC_MASK_STACKED) )
    {
      rc = switching_etree_unstacked_leaf_remove( ptin_port,                                     /* Leaf intf */
                                                  evcs[evc_id].intf[ptin_port].out_vlan,        /* Vs' */
                                                  evcs[evc_id].intf[ptin_port].int_vlan,        /* Vl */
                                                  evcs[evc_id].rvlan);                          /* Vr */
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing unstacked leaf entry [leaf_intf=%u Vs'=%u Vl=%u Vr=%u]",
                evc_id, ptin_port, evcs[evc_id].intf[ptin_port].out_vlan,
                evcs[evc_id].intf[ptin_port].int_vlan, evcs[evc_id].rvlan);
      }
    }

    rc = switching_etree_leaf_remove(ptin_port,                                     /* Leaf intf */
                                     evcs[evc_id].rvlan);                          /* Vr */

    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: error removing leaf entry [leaf_intf=%u Vr=%u]",
              evc_id, ptin_port, evcs[evc_id].rvlan);
    }
  }

  return rc;
}

/**
 * Removes all bridges interfaces from an EVC (unstacked) 
 * NOTE: EVC interfaces data is not cleared
 * 
 * @param evc_id 
 */
static L7_RC_t ptin_evc_etree_intf_remove_all(L7_uint evc_id)
{
  L7_uint i;
  L7_RC_t rc = L7_SUCCESS;

  /* Remove bridges (unstacked EVCs) */
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
  {
    if (!evcs[evc_id].intf[i].in_use)
      continue;

    if (ptin_evc_etree_intf_remove(evc_id, i) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "EVC# %u: failed to remove intf# %u bridge config", evc_id, i);
      rc = L7_FAILURE;
    }
  }

  return rc;
}


/**
 * Gets the list of root/leaf interfaces of an EVC
 * 
 * @param evc_id 
 * @param mef_type  PTIN_EVC_INTF_ROOT / PTIN_EVC_INTF_LEAF 
 * @param intf_list (output) Array with the root/leaf list
 * @param n_elems   (output) Nr of elems of list array
 */
void ptin_evc_intf_list_get(L7_uint evc_id, L7_uint8 mef_type, L7_uint intf_list[], L7_uint *n_elems)
{
  L7_uint i;

  *n_elems = 0;
  for (i=0; i<PTIN_SYSTEM_N_INTERF; i++)
  {
    if ((evcs[evc_id].intf[i].in_use) &&
        (evcs[evc_id].intf[i].type == mef_type))
    {
      intf_list[*n_elems] = i;
      (*n_elems)++;
    }
  }

//PT_LOG_TRACE(LOG_CTX_EVC, "EVC# %u: %u %s", evc_id,
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
 * Search flow based on l2intf_id
 * 
 * @param queue 
 * @param pelem 
 */
static void ptin_evc_find_flow_from_l2intf(L7_uint32 l2intf_id, dl_queue_t *queue, dl_queue_elem_t **pelem)
{
  struct ptin_evc_client_s *pflow = NULL;

  /* SEM UP */

  *pelem = NULL;

  dl_queue_get_head(queue, (dl_queue_elem_t **)&pflow);

  while (pflow != NULL)
  {
    /* If inner vlan is null, the first cvlan is returned */
    if (l2intf_id == 0 || pflow->l2intf_id == (l2intf_id & 0xffff))
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

  /* ELAN vlans */
#if (PTIN_BOARD != PTIN_BOARD_TC16SXG)
  dl_queue_init(&queue_free_vlans[PTIN_VLAN_TYPE_CPU_MCAST]);

  /* Unicast services */
  for (i=PTIN_SYSTEM_EVC_MCAST_VLAN_MIN; i<=PTIN_SYSTEM_EVC_MCAST_VLAN_MAX && i<=PTIN_VLAN_MAX; i++)
  {
    vlans_pool[i].vid = i;
    dl_queue_add(&queue_free_vlans[PTIN_VLAN_TYPE_CPU_MCAST], (dl_queue_elem_t *)&vlans_pool[i]);  
  }
  PT_LOG_INFO(LOG_CTX_EVC,"Multicast vlans (type=%u): %u - %u", PTIN_VLAN_TYPE_CPU_MCAST, PTIN_SYSTEM_EVC_MCAST_VLAN_MIN, i-1);

  /* IPTV services */
  dl_queue_init(&queue_free_vlans[PTIN_VLAN_TYPE_CPU_BCAST]);
  for (i=PTIN_SYSTEM_EVC_BCAST_VLAN_MIN; i<=PTIN_SYSTEM_EVC_BCAST_VLAN_MAX && i<=PTIN_VLAN_MAX; i++)
  {
    vlans_pool[i].vid = i;
    dl_queue_add(&queue_free_vlans[PTIN_VLAN_TYPE_CPU_BCAST], (dl_queue_elem_t*)&vlans_pool[i]);
  }
  PT_LOG_INFO(LOG_CTX_EVC,"Broadcast vlans (type=%u): %u - %u", PTIN_VLAN_TYPE_CPU_BCAST, PTIN_SYSTEM_EVC_BCAST_VLAN_MIN, i-1);
#else
  /* Both Unicast and IPTV services */
  dl_queue_init(&queue_free_vlans[PTIN_VLAN_TYPE_CPU]);

  /* Unicast services */
  for (i=PTIN_VLAN_MIN; i<=PTIN_SYSTEM_EVC_CPU_VLAN_MAX && i<=PTIN_VLAN_MAX; i++)
  {
    vlans_pool[i].vid = i;
    if((i == PTIN_ASPEN2CPU_A_VLAN)||
       (i == PTIN_ASPEN2CPU_B_VLAN))
    {
        continue;
    }

    dl_queue_add(&queue_free_vlans[PTIN_VLAN_TYPE_CPU], (dl_queue_elem_t*)&vlans_pool[i]);
  }

  PT_LOG_INFO(LOG_CTX_EVC,"CPU vlans (type=%u): %u - %u", PTIN_VLAN_TYPE_CPU, PTIN_VLAN_MIN, i-1);
#endif

  /* Bitstream with no MAC learning vlans */
  dl_queue_init(&queue_free_vlans[PTIN_VLAN_TYPE_BITSTREAM]);
  for (i=PTIN_SYSTEM_EVC_BITSTR_VLAN_MIN; i<=PTIN_SYSTEM_EVC_BITSTR_VLAN_MAX && i<=PTIN_VLAN_MAX; i++)
  {
    /* Skip reserved VLANs */
    if (i >= PTIN_RESERVED_VLAN_MIN && i <= PTIN_RESERVED_VLAN_MAX)
    {
      continue;
    }
    
    vlans_pool[i].vid = i;
    dl_queue_add(&queue_free_vlans[PTIN_VLAN_TYPE_BITSTREAM], (dl_queue_elem_t*)&vlans_pool[i]);
  }
  PT_LOG_INFO(LOG_CTX_EVC,"Bitstream vlans (type=%u): %u - %u", PTIN_VLAN_TYPE_BITSTREAM, PTIN_SYSTEM_EVC_BITSTR_VLAN_MIN, i-1);

  /* QUATTRO P2P vlans */
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  dl_queue_init(&queue_free_vlans[PTIN_VLAN_TYPE_QUATTRO]);
  for (i=PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN; i<=PTIN_SYSTEM_EVC_QUATTRO_VLAN_MAX && i<=PTIN_VLAN_MAX; i++)
  {
    vlans_pool[i].vid = i;
    dl_queue_add(&queue_free_vlans[PTIN_VLAN_TYPE_QUATTRO], (dl_queue_elem_t*)&vlans_pool[i]);
  }
  PT_LOG_INFO(LOG_CTX_EVC,"QUATTRO vlans (type=%u): %u - %u", PTIN_VLAN_TYPE_QUATTRO, PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN, i-1);
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
  else if ((evc_flags & PTIN_EVC_MASK_CPU_TRAPPING) || (evc_flags & PTIN_EVC_MASK_MC_IPTV))
  {
/* on TC16SXG the services are configured in a different manner due to MPM ports.
   As a Example Unicast Stacked use one VLAN per client. On that boards was choose to have
   the same pool for all the EVC with CPU trap*/
#if (PTIN_BOARD != PTIN_BOARD_TC16SXG)
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
#else
      *freeVlan_queue = &queue_free_vlans[PTIN_VLAN_TYPE_CPU];
      PT_LOG_TRACE(LOG_CTX_EVC, "CPUFree Vlan Queue selected!");
#endif
  }
  /* Finally Bitstream services */
  else
  {
    *freeVlan_queue = &queue_free_vlans[PTIN_VLAN_TYPE_BITSTREAM];
    PT_LOG_TRACE(LOG_CTX_EVC, "BITSTREAM Free Vlan Queue selected!");
  }

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
  L7_uint32 i;

  /* No (free vlan) queue provided */
  if (freeVlan_queue == L7_NULLPTR)
  {
    //PT_LOG_WARN(LOG_CTX_EVC, "Null pointer given as free vlan queue... do nothing!");
    return L7_SUCCESS;
  }

  for(i=0; i<PTIN_VLAN_TYPE_MAX; i++)
  {
    if (freeVlan_queue == &queue_free_vlans[i])
    {
      PT_LOG_TRACE(LOG_CTX_EVC, "Stacked Free Vlan Queue given... do nothing!");
      return L7_SUCCESS;
    }
  }

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
  L7_uint16 vid;
  struct ptin_vlan_s *pvlan = L7_NULLPTR;

  /* queue provided? */
  if (queue_vlans != L7_NULLPTR)
  {
    if (queue_vlans->n_elems == 0)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "There no VLANs available");
      return L7_NO_RESOURCES;
    }

    dl_queue_remove_head(queue_vlans, (dl_queue_elem_t**)&pvlan);

    vid = pvlan->vid;
  }
  /* VLAN provided? */
  else if (vlan != L7_NULLPTR)
  {
    vid = *vlan;

    if (vid < PTIN_VLAN_MIN || vid > PTIN_VLAN_MAX)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "VID %u not valid", vid);
      return L7_FAILURE;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_EVC, "No inputs provided");
    return L7_FAILURE;
  }

  /* Validate VLAN */

  if (switching_vlan_create(vid) != L7_SUCCESS)
  {
    if (queue_vlans != L7_NULLPTR)
    {
      dl_queue_add_head(queue_vlans, (dl_queue_elem_t *) pvlan);
    }
    PT_LOG_ERR(LOG_CTX_EVC, "Error creating VLAN %u", vid);
    return L7_FAILURE;
  }

  if (vlan != L7_NULLPTR)  *vlan = vid;

  PT_LOG_TRACE(LOG_CTX_EVC, "Allocated Internal VLAN %04u", vid);

  /* Mark this internal vlan, as being used by the given evc id */
  if (vid < 4096)
  {
    if (evc_id < PTIN_SYSTEM_N_EVCS)
    {
      evcId_from_internalVlan[vid] = evc_id;
    }
    else
    {
      evcId_from_internalVlan[vid] = -1;
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

  if (queue_vlans != L7_NULLPTR)
  {
    /* Index directly to the pool array and add the element to the free queue */
    dl_queue_add_head(queue_vlans, (dl_queue_elem_t *) &vlans_pool[vlan]);
    PT_LOG_TRACE(LOG_CTX_EVC, "Freed Internal VLAN %04u (%u available)", vlan, queue_vlans->n_elems);
  }

  /* Free this internal vlan, as not being used by any evc */
  if (vlan<4096)
  {
    evcId_from_internalVlan[vlan] = -1;
  }

  return L7_SUCCESS;
}


/* Switching configuration functions ******************************************/

/**
 * Configures a root port (for stacked and unstacked EVCs) 
 *  1. Associates the internal VLAN to the root intf
 *  2. Adds egress and ingress xlate entries (Out.VLAN<=>Int.VLAN)
 * 
 * @param intf_cfg      Port and external VLANs 
 * @param int_vlan      Inner VLAN 
 * @param force_pcp     Force ingress packets to have this pcp
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_root_add(ptin_HwEthMef10Intf_t *intf_cfg, L7_uint16 int_vlan, L7_uint16 new_innerVlan,
                                  L7_BOOL egress_del_ivlan, L7_int force_pcp)
{
  L7_uint32 root_intf, intIfNum;
  ptin_HwEthMef10Intf_t intf_vlan_set;
  L7_RC_t   rc = L7_SUCCESS;

  /* Get ptin_port & intIfNum */
  root_intf = intf_cfg->intf.value.ptin_port;
  intIfNum  = intf_cfg->intf.value.intIfNum;

  PT_LOG_TRACE(LOG_CTX_EVC, "Adding root intf# %u [Out.VID=%u Inn.VID=%u Int.VLAN=%u]...",
               root_intf, intf_cfg->vid, intf_cfg->vid_inner, int_vlan);

  #ifdef PTIN_ERPS_EVC
  if (ptin_hal_erps_isPortBlocked(root_intf, intf_cfg->vid, int_vlan) != L7_TRUE)
  {
  #endif
    /* Associate root internal vlan to the root intf */
    rc = usmDbVlanMemberSet(1, int_vlan, intIfNum, L7_DOT1Q_FIXED, DOT1Q_SWPORT_MODE_NONE);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error associating root Int.VLAN %u to root intIfNum# %u (rc=%d)", int_vlan, intIfNum, rc);
      goto exit;
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
    goto exit;
  }

  /* Add ingress xlate entry: (root_intf) out_vlan -> int_vlan */
  if (intf_cfg->vid >= 1 && intf_cfg->vid <= 4095)
  {
    intf_vlan_set = *intf_cfg;
    intf_vlan_set.intf          = intf_cfg->intf;
    intf_vlan_set.mef_type      = intf_cfg->mef_type;
    intf_vlan_set.vid           = intf_cfg->vid;
    intf_vlan_set.vid_inner     = intf_cfg->vid_inner;
    intf_vlan_set.action_outer  = intf_cfg->action_outer;
    intf_vlan_set.action_inner  = intf_cfg->action_inner;

    rc = ptin_xlate_ingress_add(&intf_vlan_set, int_vlan, new_innerVlan, force_pcp, -1); 
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error adding intIfNum# %u xlate Ingress entry [Out.VLAN %u + Inn.VLAN %u => Int.VLAN] %u (rc=%d)",
                 intIfNum, intf_cfg->vid, intf_cfg->vid_inner, int_vlan, rc);
      goto exit;
    }
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_EVC, "No ingress translation defined for root_intf %u, int_vlan %u", root_intf, int_vlan);
  }

  /* Add egress xlate entry: (root_intf) int_vlan -> out_vlan */
  if (intf_cfg->vid >= 1)
  {
    intf_vlan_set = *intf_cfg;
    intf_vlan_set.intf          = intf_cfg->intf;
    intf_vlan_set.mef_type      = intf_cfg->mef_type;
    intf_vlan_set.vid           = int_vlan;
    intf_vlan_set.vid_inner     = new_innerVlan;
    intf_vlan_set.action_outer  = PTIN_XLATE_ACTION_REPLACE;
    intf_vlan_set.action_inner  = (egress_del_ivlan) ? PTIN_XLATE_ACTION_DELETE : intf_cfg->action_inner;

    rc = ptin_xlate_egress_add(&intf_vlan_set,
                               (intf_cfg->vid > 4095) ? (L7_uint16)-1 : intf_cfg->vid,      /* Pop, or swap to out_vlan */
                               (egress_del_ivlan) ? (L7_uint16)-1 : intf_cfg->vid_inner,   /* Pop, or swap to inner_vlan */
                                -1, -1);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error adding intIfNum# %u xlate Egress entry [Int.VLAN %u => Out.VLAN %u] (rc=%d)",
              intIfNum, int_vlan, intf_cfg->vid, rc);
      goto exit;
    }
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_EVC, "No egress translation defined for root_intf %u, int_vlan %u", root_intf, int_vlan);
  }

  return L7_SUCCESS;

exit:
  (void) switching_root_remove(intf_cfg->intf.value.ptin_port, intf_cfg->vid, intf_cfg->vid_inner, int_vlan);
  return rc;
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
  if (out_vlan >= 1)
  {
    rc = ptin_xlate_egress_delete(root_intf, int_vlan, 0);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error deleting ptin_port# %u xlate Egress entry [Int.VLAN %u => Out.VLAN %u} (rc=%d)",
                 root_intf, int_vlan, out_vlan, rc);
      return rc;
    }
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_EVC, "No need to remove egress translations (root_intf %u)", root_intf);
  }

  /* Delete ingress xlate entry: (root_intf) out_vlan -> int_vlan */
  if (out_vlan >= 1 && out_vlan <= 4095)
  {
    rc = ptin_xlate_ingress_delete(root_intf, out_vlan, inner_vlan); 
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error deleting ptin_port# %u xlate Ingress entry [Out.VLAN %u+Inn.VLAN %u => Int.VLAN %u] (rc=%d)",
                 root_intf, out_vlan, inner_vlan, int_vlan, rc);
      return rc;
    }
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_EVC, "No need to remove ingress translations (root_intf %u)", root_intf);
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
  L7_uint16 pvid, newOVlan, newIVlan;
  L7_uint32 tag_mode;
  L7_RC_t   rc = L7_SUCCESS;

  /* Validate arguments */
  if (int_vlan>=4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (intVlan=%u)", int_vlan);
    return L7_FAILURE;
  }

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
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u (intVlan=%u) should be in use", evc_id, int_vlan);
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
    /* Determine tag mode, according to default VLAN */
    if (ptin_xlate_PVID_get(intIfNum, &pvid) == L7_SUCCESS &&
        ptin_xlate_egress_get(root_intf, int_vlan, PTIN_XLATE_NOT_DEFINED, &newOVlan, &newIVlan) == L7_SUCCESS &&
        newOVlan == pvid)
    {
      tag_mode = L7_DOT1Q_UNTAGGED;
    }
    else
    {
      tag_mode = L7_DOT1Q_TAGGED;
    }
    /* Configure the internal VLAN on this interface as tagged */
    if (usmDbVlanTaggedSet(1, int_vlan, intIfNum, tag_mode) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error setting intIfNum# %u internal VLAN %u tag mode (%u)", intIfNum, int_vlan, tag_mode);
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

  /* Validate arguments */
  if (int_vlan>=4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (intVlan=%u)", int_vlan);
    return L7_FAILURE;
  }

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
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u (intVlan=%u) should be in use", evc_id, int_vlan);
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

  /* Validate arguments */
  if (int_vlan>=4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (intVlan=%u)", int_vlan);
    return L7_FAILURE;
  }

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
    PT_LOG_ERR(LOG_CTX_EVC,"Non-consistent situation: evc %u (intVlan=%u) should be in use", evc_id, int_vlan);
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
L7_int switching_erps_internalVlan_get(L7_int initial_evc_id, L7_uint8 erps_ptin_port0, L7_uint8 erps_ptin_port1, L7_uint8 *vid_bmp, L7_uint16 *internalVlan)
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
        *internalVlan = evcs[evc_id].rvlan;
        return evc_id;
      }
    }
  }

  *internalVlan = 0;
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
 * @param intVlan     : internal vlan
 * @param ptin_port   : interface
 * @param stats_intf  : pointer to stats
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE;
 */
L7_RC_t ptin_evc_igmp_stats_get_fromIntVlan(L7_uint16 intVlan, L7_uint32 ptin_port, ptin_IGMP_Statistics_t **stats_intf)
{
  L7_uint evc_id;

  /* Validate ptin_port */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    return L7_FAILURE;
  }
  
  /* Validate Vlan */
  if (intVlan < PTIN_VLAN_MIN || intVlan > PTIN_VLAN_MAX)
  {
    return L7_FAILURE;
  }

  /* Get evc id */
  evc_id = evcId_from_internalVlan[intVlan];

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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
    PT_LOG_ERR(LOG_CTX_EVC, "eEVC# %u is out of range [0..%lu]", evc_ext_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
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
    return rc;
  }

  /* Configure the internal VLAN on this interface as tagged */
  rc = usmDbVlanTaggedSet(1, leaf_int_vlan, intIfNum, L7_DOT1Q_TAGGED);
  if (rc != L7_SUCCESS)
  {
    (void)usmDbVlanMemberSet(1, leaf_int_vlan, intIfNum, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE);  
    PT_LOG_ERR(LOG_CTX_EVC, "Error setting intIfNum# %u internal VLAN %u as tagged (rc=%d)", intIfNum, leaf_int_vlan, rc);
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Removes a leaf port (for stacked and unstacked EVCs) 
 * 
 * @param  leaf_intf Leaf interface (ptin_intf)
 * @param  int_vlan  Inner VLAN 
 * @param  iptv_flag IPTV Flag 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 * @notes  This only applies when the iptv flag is on: before 
 *         returning from this function we need ensure that the
 *         Snooping module finishes the removal of all L3 Snoop
 *         entries associated with this leaf_int_vlan/intIfNum.
 *         This is taken care by the __ptin_evc_l3_intf_sem
 *         semaphore.
 */
static L7_RC_t switching_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_int_vlan, L7_BOOL iptv_flag)
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
  
  /*Please see the notes for further info*/
  if (iptv_flag)
  {    
    __ptin_evc_l3_intf_sem_handle(leaf_int_vlan, leaf_intf);   
  }

  /* Delete intIfNum from leaf_int_vlan */
  rc = usmDbVlanMemberSet(1, leaf_int_vlan, intIfNum, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE);  
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intIfNum# %u from Int.VLAN %u (rc=%d)", intIfNum, leaf_int_vlan, rc);
    return L7_FAILURE;
  }

  /*Please see the notes for further info*/
  if (iptv_flag)
  {
    ptin_evc_l3_intf_sem_give(leaf_int_vlan, leaf_intf);   
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
static L7_RC_t switching_elan_leaf_add(ptin_HwEthMef10Intf_t *intf_vlan,
                                       L7_uint16 int_vlan, L7_BOOL egress_del_ivid, L7_int force_pcp)
{
  L7_uint32 leaf_intf;
  ptin_HwEthMef10Intf_t intf_vlan_set;
  L7_RC_t   rc = L7_SUCCESS;

  /* Get ptin_port & intIfNum */
  leaf_intf = intf_vlan->intf.value.ptin_port;

  PT_LOG_TRACE(LOG_CTX_EVC, "Adding stacked leaf intf# %u [Int.VLAN=%u]...", leaf_intf, int_vlan);

  /* Add ingress xlate entry: (leaf_intf) (Vs',Vc) => (Vr,Vc) */
  if (intf_vlan->vid >= 1 && intf_vlan->vid <= 4095)
  {
    intf_vlan_set = *intf_vlan;
    intf_vlan_set.intf          = intf_vlan->intf;
    intf_vlan_set.mef_type      = intf_vlan->mef_type;
    intf_vlan_set.vid           = intf_vlan->vid;
    intf_vlan_set.vid_inner     = intf_vlan->vid_inner;
    intf_vlan_set.action_outer  = intf_vlan->action_outer;
    intf_vlan_set.action_inner  = intf_vlan->action_inner;

    rc = ptin_xlate_ingress_add(&intf_vlan_set, int_vlan, intf_vlan->vid_inner, -1, -1); 
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error adding intf %u xlate Ingress entry [Leaf Out.VLAN %u + Inn.VLAN %u => Root Int.VLAN %u] (rc=%d)",
              leaf_intf, intf_vlan->vid, intf_vlan->vid_inner, int_vlan, rc);
      goto exit;
    }
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_EVC, "No ingress translations defined for leaf_intf %u, int_vlan %u", leaf_intf, int_vlan);
  }

  /* Add egress xlate entry: (leaf_intf) (Vr,Vc) => (Vs',Vc); innerVlan is to be removed */
  if (intf_vlan->vid >= 1)
  {
    intf_vlan_set = *intf_vlan;
    intf_vlan_set.intf          = intf_vlan->intf;
    intf_vlan_set.mef_type      = intf_vlan->mef_type;
    intf_vlan_set.vid           = int_vlan;
    intf_vlan_set.vid_inner     = intf_vlan->vid_inner;
    intf_vlan_set.action_outer  = (intf_vlan->action_outer == PTIN_XLATE_ACTION_ADD) ? PTIN_XLATE_ACTION_DELETE : intf_vlan->action_outer;
    if (egress_del_ivid)
    {
      intf_vlan_set.action_inner = PTIN_XLATE_ACTION_DELETE;
    }
    else
    {
      /* Inverted logic: comparison is related to ingress, but the applied action is related to egress */
#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
      /* On TC16SXG P2P have different xlate operations,
      due to the port virtualization and ASPEN limitations*/
      intf_vlan_set.action_inner = (intf_vlan->action_inner == PTIN_XLATE_ACTION_ADD) ? PTIN_XLATE_ACTION_REPLACE : intf_vlan->action_inner;
#else
      intf_vlan_set.action_inner = (intf_vlan->action_inner == PTIN_XLATE_ACTION_ADD) ? PTIN_XLATE_ACTION_DELETE : intf_vlan->action_inner;
#endif
    }

    rc = ptin_xlate_egress_add(&intf_vlan_set,
                               (intf_vlan->vid > 4095) ? (L7_uint16)-1 : intf_vlan->vid,  /* Pop, or swap to leaf_out_vlan */
                               (egress_del_ivid) ? (L7_uint16)-1 : intf_vlan->vid_inner,  /* Pop, or swap to leaf_inner_vlan */
                               force_pcp, -1); 
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error adding intf %u xlate Egress entry [Root Int.VLAN %u + Inn.VLAN %u => Leaf Out.VLAN %u] (rc=%d)",
              leaf_intf, int_vlan, intf_vlan->vid_inner, intf_vlan->vid, rc);
      goto exit;
    }
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_EVC, "No egress translations defined for leaf_intf %u, int_vlan %u", leaf_intf, int_vlan);
  }

  return L7_SUCCESS;

exit:
  (void) switching_elan_leaf_remove(leaf_intf, intf_vlan->vid, intf_vlan->vid_inner, int_vlan, egress_del_ivid);
  return rc;
}

/**
 * Removes a leaf port (for stacked and unstacked EVCs) 
 * 
 * @param leaf_intf Leaf interface (ptin_intf) 
 * @param leaf_out_vlan : Inner VLAN 
 * @param leaf_inner_vlan : Inner VLAN 
 * @param int_vlan  Inner : VLAN 
 * @param egress_del_ivid : Applicable where the inner vlan 
 *                        should be deleted/added
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_elan_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inner_vlan, L7_uint16 int_vlan, L7_BOOL egress_del_ivid)
{
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing stacked leaf intf# %u [Int.VLAN=%u]...",
           leaf_intf, int_vlan);

  /* Remove ingress xlate entry: (leaf_intf) (Vs',Vc) => (Vr,Vc) */
  if (leaf_out_vlan >= 1 && leaf_out_vlan <= 4095)
  {
    rc = ptin_xlate_ingress_delete(leaf_intf, leaf_out_vlan, (egress_del_ivid) ? 0 : leaf_inner_vlan);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intf %u xlate Ingress entry [Leaf Out.VLAN %u + Inn.VLAN %u] (rc=%d)",
                 leaf_intf, leaf_out_vlan, 0, rc);
      return rc;
    }
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_EVC, "No need to remove ingress translations (leaf_intf %u)", leaf_intf);
  }

  /* Remove egress xlate entry: (leaf_intf) (Vr,Vc) => (Vs',Vc) */
  if (leaf_out_vlan >= 1)
  {
    rc = ptin_xlate_egress_delete(leaf_intf, int_vlan, leaf_inner_vlan);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intf %u xlate Egress entry [Root Int.VLAN %u + Inn.VLAN %u (rc=%d)",
                 leaf_intf, int_vlan, leaf_inner_vlan, rc);
      return rc;
    }
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_EVC, "No need to remove egress translations (leaf_intf %u)", leaf_intf);
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
static L7_RC_t switching_mcevc_leaf_add(ptin_HwEthMef10Intf_t *intf_vlan, L7_uint16 int_vlan)
{
  L7_uint32 leaf_intf;
  ptin_HwEthMef10Intf_t intf_vlan_set;
  L7_RC_t   rc = L7_SUCCESS;

  /* Get ptin_port & intIfNum */
  leaf_intf = intf_vlan->intf.value.ptin_port;

  PT_LOG_TRACE(LOG_CTX_EVC, "Adding stacked leaf intf# %u [Int.VLAN=%u]...", leaf_intf, int_vlan);

  /* Add egress xlate entry: (leaf_intf) (Vr,Vc) => (Vs',Vc); innerVlan is to be added */
  /* Only for Multicast services at PON ports:
     GPON boards require to replace outer tag to vlan=gem_id + prio 0, and add an inner tag with inner_prio = outer_prio */
  if (intf_vlan->vid >= 1)
  {
    intf_vlan_set = *intf_vlan;
    intf_vlan_set.intf          = intf_vlan->intf;
    intf_vlan_set.mef_type      = intf_vlan->mef_type;
    intf_vlan_set.vid           = int_vlan;
    intf_vlan_set.vid_inner     = intf_vlan->vid_inner;
    intf_vlan_set.action_outer  = (intf_vlan->action_outer == PTIN_XLATE_ACTION_ADD) ? PTIN_XLATE_ACTION_DELETE : intf_vlan->action_outer;
    intf_vlan_set.action_inner  = PTIN_XLATE_ACTION_ADD;  /* Always add an inner VLAN */

    rc = ptin_xlate_egress_add(&intf_vlan_set,
                               (intf_vlan->vid > 4095) ? (L7_uint16)-1 : intf_vlan->vid,  /* Pop, or swap to leaf_out_vlan */
                               intf_vlan->vid_inner,
                               -1, -1);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error adding intf %u xlate Egress entry [Root Int.VLAN %u => Leaf Out.VLAN %u + Leaf Inn.VLAN %u] (rc=%d)",
              leaf_intf, int_vlan, intf_vlan->vid, intf_vlan->vid_inner, rc);
      return rc;
    }
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_EVC, "No egress translations defined for leaf_intf %u, int_vlan %u", leaf_intf, int_vlan);
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
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing stacked leaf intf# %u [Int.VLAN=%u]...",
           leaf_intf, int_vlan);

  /* Add egress xlate entry: (leaf_intf) (Vr,Vc) => (Vs',Vc) */
  if (leaf_out_vlan >= 1)
  {
    rc = ptin_xlate_egress_delete(leaf_intf, int_vlan, 0); 
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intf %u xlate Egress entry [Root Int.VLAN %u (rc=%d)",
                 leaf_intf, int_vlan, rc);
      return rc;
    }
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_EVC, "No need to remove egress translations (leaf_intf %u)", leaf_intf);
  }

  return L7_SUCCESS;
}
#endif


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
static L7_RC_t switching_etree_root_add(L7_uint root_intf, L7_uint16 root_out_vlan, L7_uint16 leaf_int_vlan, L7_BOOL egress_del_ivid)
{
  ptin_HwEthMef10Intf_t intf_vlan_set;
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Configuring unstacked root intf# %u [Leaf Int.VLAN=%u => Root Out.VLAN=%u]...",
               root_intf, leaf_int_vlan, root_out_vlan);

  memset(&intf_vlan_set, 0x00, sizeof(intf_vlan_set));
  intf_vlan_set.intf.format = PTIN_INTF_FORMAT_PORT;
  intf_vlan_set.intf.value.ptin_port = root_intf;
  if (ptin_intf_any_format(&intf_vlan_set.intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", root_intf);
    return L7_FAILURE;
  }
  intf_vlan_set.mef_type = PTIN_EVC_INTF_ROOT; 

  PT_LOG_TRACE(LOG_CTX_EVC, "Adding intIfNum# %u xlate Egress entry [Leaf Int.VLAN %u => Root Out.VLAN %u] (egress_del_ivid=%u)",
               intf_vlan_set.intf.value.intIfNum, leaf_int_vlan, root_out_vlan, egress_del_ivid);

  /* Add egress xlate entry: (root_intf) leaf_int_vlan -> root_out_vlan; innerVlan is to be removed if EVC is unstacked */
  intf_vlan_set.vid           = leaf_int_vlan;
  intf_vlan_set.vid_inner     = 0;
  intf_vlan_set.action_outer  = PTIN_XLATE_ACTION_REPLACE;
  intf_vlan_set.action_inner  = (egress_del_ivid) ? PTIN_XLATE_ACTION_DELETE : PTIN_XLATE_ACTION_NONE;

  rc = ptin_xlate_egress_add(&intf_vlan_set, root_out_vlan, ((egress_del_ivid) ? (L7_uint16) -1 : 0), -1, -1);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding intIfNum# %u xlate Egress entry [Leaf Int.VLAN %u => Root Out.VLAN %u] (rc=%d)",
               intf_vlan_set.intf.value.intIfNum, leaf_int_vlan, root_out_vlan, rc);
    goto exit;
  }

  /* Associate leaf internal vlan to the root intf */
  rc = usmDbVlanMemberSet(1, leaf_int_vlan, intf_vlan_set.intf.value.intIfNum, L7_DOT1Q_FIXED, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error associating leaf Int.VLAN %u to root intIfNum# %u to (rc=%d)",
               leaf_int_vlan, intf_vlan_set.intf.value.intIfNum, rc);
    goto exit;
  }

  return L7_SUCCESS;

exit:
  (void)switching_etree_root_remove(root_intf, root_out_vlan, leaf_int_vlan);
  return rc;
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
static L7_RC_t switching_etree_root_remove(L7_uint root_intf, L7_uint16 root_out_vlan, L7_uint16 leaf_int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing unstacked root intf# %u [Leaf Int.VLAN=%u => Root Out.VLAN=%u]...",
           root_intf, leaf_int_vlan, root_out_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(root_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", root_intf);
    return L7_FAILURE;
  }

  /* Delete intIfNum from int_vlan */
  rc = usmDbVlanMemberSet(1, leaf_int_vlan, intIfNum, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intIfNum# %u from Int.VLAN %u (rc=%d)", intIfNum, leaf_int_vlan, rc);
    return L7_FAILURE;
  }

  /* Delete egress xlate entry: (root_intf) leaf_int_vlan -> root_out_vlan */
  rc = ptin_xlate_egress_delete(root_intf, leaf_int_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting ptin_port# %u xlate Egress entry [Leaf Int.VLAN %u => Root Out.VLAN %u] (rc=%d)",
               root_intf, leaf_int_vlan, root_out_vlan, rc);
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Adds leaf configuration for EVCs 
 * 
 * @param leaf_intf     Leaf interface # (ptin_intf)
 * @param root_int_vlan Root internal VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_etree_leaf_add(L7_uint leaf_intf, L7_uint16 root_int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Configuring leaf intf# %u [Root Int.VLAN=%u]",leaf_intf, root_int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(leaf_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Associate root internal vlan to the leaf intf */
  rc = usmDbVlanMemberSet(1, root_int_vlan, intIfNum, L7_DOT1Q_FIXED, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error associating root Int.VLAN %u to leaf intIfNum# %u to (rc=%d)", root_int_vlan, intIfNum, rc);
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
static L7_RC_t switching_etree_unstacked_leaf_add(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_int_vlan, L7_uint16 root_int_vlan)
{
  ptin_HwEthMef10Intf_t intf_vlan_set;
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Configuring unstacked leaf intf# %u [Leaf Out.VLAN=%u => Leaf Int.VLAN=%u; Root Int.VLAN=%u => Leaf Out.VLAN=%u]...",
           leaf_intf, leaf_out_vlan, leaf_int_vlan, root_int_vlan, leaf_out_vlan);

  memset(&intf_vlan_set, 0x00, sizeof(intf_vlan_set));
  intf_vlan_set.intf.format = PTIN_INTF_FORMAT_PORT;
  intf_vlan_set.intf.value.ptin_port = leaf_intf;
  if (ptin_intf_any_format(&intf_vlan_set.intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }
  intf_vlan_set.mef_type      = PTIN_EVC_INTF_LEAF; 

  /* Add ingress xlate entry: (leaf_intf)  (leaf outer vlan => leaf internal vlan) */
  intf_vlan_set.vid           = leaf_out_vlan;
  intf_vlan_set.vid_inner     = 0;
  intf_vlan_set.action_outer  = PTIN_XLATE_ACTION_REPLACE;
  intf_vlan_set.action_inner  = PTIN_XLATE_ACTION_NONE;

  rc = ptin_xlate_ingress_add(&intf_vlan_set, leaf_int_vlan, 0, -1, -1);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding intIfNum# %u xlate Ingress entry [Leaf Out.VLAN %u => Leaf Int.VLAN %u] (rc=%d)",
               intf_vlan_set.intf.value.intIfNum, leaf_out_vlan, leaf_int_vlan, rc);
    goto exit;
  }

  /* Add egress xlate entry: (leaf_intf) (root internal vlan => leaf outer vlan) */
  intf_vlan_set.vid           = root_int_vlan;
  intf_vlan_set.vid_inner     = 0;
  intf_vlan_set.action_outer  = PTIN_XLATE_ACTION_REPLACE;
  intf_vlan_set.action_inner  = PTIN_XLATE_ACTION_NONE;

  rc = ptin_xlate_egress_add(&intf_vlan_set, leaf_out_vlan, 0, -1, -1);
  if (rc != L7_SUCCESS)
  {
    (void)ptin_xlate_ingress_delete(intf_vlan_set.intf.value.ptin_port, leaf_out_vlan, 0);
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding ptin_port# %u xlate Egress entry [Root Int.VLAN %u => Leaf Out.VLAN %u] (rc=%d)",
               intf_vlan_set.intf.value.ptin_port, root_int_vlan, leaf_out_vlan, rc);
    goto exit;
  }

  return L7_SUCCESS;

exit:
  (void)switching_etree_unstacked_leaf_remove(leaf_intf, leaf_out_vlan, leaf_int_vlan, root_int_vlan);
  return rc;
}

/**
 * Adds leaf configuration for stacked EVCs 
 *  1. configures ingress xlate (leaf outer vlan => leaf internal vlan)
 *  2. configures egress xlate (root internal vlan => leaf outer vlan)
 *  3. associates leaf_intf to root int.vlan (Vr)
 * 
 * @param leaf_intf     Leaf interface # (ptin_intf)
 * @param leaf_out_vlan Leaf outer VLAN 
 * @param leaf_inn_vlan Leaf inner VLAN 
 * @param leaf_int_vlan Leaf internal VLAN
 * @param root_int_vlan Root internal VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
#if ( !PTIN_BOARD_IS_MATRIX )
static L7_RC_t switching_etree_stacked_leaf_add(ptin_HwEthMef10Intf_t *intf_vlan, L7_uint16 leaf_int_vlan, L7_uint16 root_int_vlan)
{
  ptin_HwEthMef10Intf_t intf_vlan_set;
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Configuring unstacked leaf intf# %u [Leaf Out.VLAN=%u => Leaf Int.VLAN=%u + Inn.VLAN=%u; Root Int.VLAN=%u + Inn.VLAN=%u => Leaf Out.VLAN=%u]...",
               intf_vlan->intf.value.ptin_port, intf_vlan->vid, leaf_int_vlan, intf_vlan->vid_inner, root_int_vlan, intf_vlan->vid_inner, intf_vlan->vid);

  /* Add ingress xlate entry: (leaf_intf)  (leaf outer vlan => leaf internal vlan) */
  intf_vlan_set = *intf_vlan;
  intf_vlan_set.intf          = intf_vlan->intf;
  intf_vlan_set.mef_type      = intf_vlan->mef_type;
  intf_vlan_set.vid           = intf_vlan->vid;
  intf_vlan_set.vid_inner     = 0;
  intf_vlan_set.action_outer  = intf_vlan->action_outer;
  intf_vlan_set.action_inner  = intf_vlan->action_inner;

  rc = ptin_xlate_ingress_add(&intf_vlan_set, leaf_int_vlan, intf_vlan->vid_inner, -1, -1);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding intIfNum# %u xlate Ingress entry [Leaf Out.VLAN %u => Leaf Int.VLAN %u + Inn.VLAN %u] (rc=%d)",
               intf_vlan->intf.value.intIfNum, intf_vlan->vid, leaf_int_vlan, intf_vlan->vid_inner, rc);
    goto exit;
  }

  /* Add egress xlate entry: (leaf_intf) (root internal vlan+leaf_inn_vlan => leaf outer vlan / inner vlan is to be removed) */
  intf_vlan_set = *intf_vlan;
  intf_vlan_set.intf          = intf_vlan->intf;
  intf_vlan_set.mef_type      = intf_vlan->mef_type;
  intf_vlan_set.vid           = root_int_vlan;
  intf_vlan_set.vid_inner     = intf_vlan->vid_inner;
  intf_vlan_set.action_outer  = (intf_vlan->action_outer == PTIN_XLATE_ACTION_ADD) ? PTIN_XLATE_ACTION_DELETE : intf_vlan->action_outer;
  intf_vlan_set.action_inner  = (intf_vlan->action_inner == PTIN_XLATE_ACTION_ADD) ? PTIN_XLATE_ACTION_DELETE : intf_vlan->action_inner;

  rc = ptin_xlate_egress_add(&intf_vlan_set, intf_vlan->vid, (L7_uint16)-1, -1, -1);
  if (rc != L7_SUCCESS)
  {
    (void)ptin_xlate_ingress_delete(intf_vlan_set.intf.value.ptin_port, intf_vlan->vid, 0);
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding ptin_port# %u xlate Egress entry [Root Int.VLAN %u + Inn.VLAN %u => Leaf Out.VLAN %u] (rc=%d)",
               intf_vlan->intf.value.ptin_port, root_int_vlan, intf_vlan->vid_inner, intf_vlan->vid, rc);
    goto exit;
  }

  return L7_SUCCESS;

exit:
  (void)switching_etree_stacked_leaf_remove(intf_vlan->intf.value.ptin_port,
                                            intf_vlan->vid, intf_vlan->vid_inner,
                                            leaf_int_vlan, root_int_vlan);
  return rc;
}
#endif

/**
 * Removes leaf configuration for EVCs 
 * 
 * @param leaf_intf     Leaf interface # (ptin_intf)
 * @param root_int_vlan Root internal VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t switching_etree_leaf_remove(L7_uint leaf_intf, L7_uint16 root_int_vlan)
{
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing leaf intf# %u [Root Int.VLAN=%u]", leaf_intf, root_int_vlan);

  /* Get intIfNum of ptin interface */
  rc = ptin_intf_port2intIfNum(leaf_intf, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Interface is invalid: %u", leaf_intf);
    return L7_FAILURE;
  }

  /* Delete intIfNum from int_vlan */
  rc = usmDbVlanMemberSet(1, root_int_vlan, intIfNum, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting intIfNum# %u from Int.VLAN %u (rc=%d)", intIfNum, root_int_vlan, rc);
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
static L7_RC_t switching_etree_unstacked_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_int_vlan, L7_uint16 root_int_vlan)
{
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing unstacked leaf intf# %u [Leaf Out.VLAN=%u => Leaf Int.VLAN=%u; Root Int.VLAN=%u => Leaf Out.VLAN=%u]...",
           leaf_intf, leaf_out_vlan, leaf_int_vlan, root_int_vlan, leaf_out_vlan);

  /* Deletes ingress xlate entry: (leaf_intf)  (leaf outer vlan => leaf internal vlan) */
  rc = ptin_xlate_ingress_delete(leaf_intf, leaf_out_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting ptin_port# %u xlate Ingress entry [Leaf Out.VLAN %u => Leaf Int.VLAN %u] (rc=%d)",
               leaf_intf, leaf_out_vlan, leaf_int_vlan, rc);
    return rc;
  }

  /* Deletes egress xlate entry: (leaf_intf) (root internal vlan => leaf outer vlan) */
  rc = ptin_xlate_egress_delete(leaf_intf, root_int_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting ptin_port# %u xlate Egress entry [Root Int.VLAN %u => Leaf Out.VLAN %u] (rc=%d)",
               leaf_intf, root_int_vlan, leaf_out_vlan, rc);
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Removes leaf configuration for stacked EVCs 
 *  1. deletes ingress xlate (leaf outer vlan => leaf internal vlan)
 *  2. deletes egress xlate (root internal vlan => leaf outer vlan)
 *  3. deletes association of leaf_intf to root int.vlan (Vr)
 * 
 * @param leaf_intf     Leaf interface # (ptin_intf)
 * @param leaf_out_vlan Leaf outer VLAN 
 * @param leaf_inn_vlan Leaf inner VLAN 
 * @param leaf_int_vlan Leaf internal VLAN
 * @param root_int_vlan Root internal VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
#if ( !PTIN_BOARD_IS_MATRIX )
static L7_RC_t switching_etree_stacked_leaf_remove(L7_uint leaf_intf, L7_uint16 leaf_out_vlan, L7_uint16 leaf_inn_vlan, L7_uint16 leaf_int_vlan, L7_uint16 root_int_vlan)
{
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing stacked leaf intf# %u [Leaf Out.VLAN=%u => Leaf Int.VLAN=%u + Inn.VLAN=%u; Root Int.VLAN=%u + Inn.VLAN=%u => Leaf Out.VLAN=%u]...",
           leaf_intf, leaf_out_vlan, leaf_int_vlan, leaf_inn_vlan, root_int_vlan, leaf_inn_vlan, leaf_out_vlan);

  /* Deletes ingress xlate entry: (leaf_intf)  (leaf outer vlan => leaf internal vlan) */
  rc = ptin_xlate_ingress_delete(leaf_intf, leaf_out_vlan, 0);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting ptin_port# %u xlate Ingress entry [Leaf Out.VLAN %u => Leaf Int.VLAN %u] (rc=%d)",
               leaf_intf, leaf_out_vlan, leaf_int_vlan, rc);
    return rc;
  }

  /* Deletes egress xlate entry: (leaf_intf) (root internal vlan => leaf outer vlan) */
  rc = ptin_xlate_egress_delete(leaf_intf, root_int_vlan, leaf_inn_vlan);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error deleting ptin_port# %u xlate Egress entry [Root Int.VLAN %u => Leaf Out.VLAN %u] (rc=%d)",
               leaf_intf, root_int_vlan, leaf_out_vlan, rc);
    return rc;
  }

  return L7_SUCCESS;
}
#endif

/**
 * Adds a bridge between a root and leaf interface of a stacked EVC 
 *  1. configures translation entries (egress+ingress) on leaf interface
 *  2. configures a cross-connection between root and leaf interface
 *  3. on TC16SXG configure a drop cancel rule for Pause frames
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
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Adding stacked bridge [Root: Intf=%u IntVID=%u] <=> [Leaf: Intf=%u NEW Out.VID=%u Inn.VID=%u] ...",
               root_intf, root_int_vid, leaf_intf, leaf_out_vid, leaf_inn_vid);

  /* Add cross-connection between root and leaf interfaces */
  rc = ptin_crossconnect_add(root_int_vid, leaf_inn_vid, root_intf, leaf_intf);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding Xconnect (OVID=%u, IVID=%u) (ptin_port1=%u, ptin_port2=%u) (rc=%d)",
            root_int_vid, leaf_inn_vid, root_intf, leaf_intf, rc);
    return rc;
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
  L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_EVC, "Removing stacked bridge [Root: Intf=%u IntVID=%u] <=> [Leaf: Intf=%u NEW Out.VID=%u Inn.VID=%u] ...",
           root_intf, root_int_vid, leaf_intf, leaf_out_vid, leaf_inn_vid);

  /* Deletes cross-connection between root and leaf interfaces */
  rc = ptin_crossconnect_delete(root_int_vid, leaf_inn_vid);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "Error adding Xconnect (OVID=%u, IVID=%u) (rc=%d)",
            root_int_vid, leaf_inn_vid, rc);
    return rc;
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
    PT_LOG_CRITIC(LOG_CTX_EVC, "Error deleting VLAN %u (rc=%d)", vid, rc);
    //return L7_FAILURE;
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
static L7_RC_t switching_vlan_config(L7_uint16 vid, L7_uint16 fwd_vid, L7_BOOL mac_learning, L7_uint8 mc_flood, L7_uint8 cpu_trap, L7_int queue_type)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_FILTER_VLAN_FILTER_MODE_t mcf = 0xff;

  /* Apply/remove cpu vlan */
  rc = ptin_vlan_cpu_set( vid, cpu_trap );
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "VLAN %u: error setting CPU trap state to %u (rc=%d)",vid, cpu_trap, rc);
    return L7_FAILURE;
  }

  /* Set Forward VLAN to int_vlan and set Mac Learning state */
  rc = ptin_xconnect_vlan_properties(vid, fwd_vid, -1, mac_learning, queue_type);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC, "VLAN %u: error setting MAC Learning state to %s (w/ Forward VLAN %u) (rc=%d)",
            vid, mac_learning ? "Enabled":"Disabled", fwd_vid, rc);
    return L7_FAILURE;
  }

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

#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
  /* Number of interfaces, FIX for TC16SXG */
  if (evcConf->n_intf==0)
  {
    PT_LOG_WARN(LOG_CTX_EVC,"Number of interfaces (%u)",evcConf->n_intf);
    return L7_SUCCESS;
  }
#endif

  /* Number of interfaces */
  if (evcConf->n_intf==0 || evcConf->n_intf>=PTIN_SYSTEM_MAX_N_PORTS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid number of interfaces (%u)",evcConf->n_intf);
    return L7_FAILURE;
  }

  /* Run all interfaces */
  for (i=0; i<evcConf->n_intf; i++)
  {
    /* MEF type */
    if (evcConf->intf[i].mef_type!=PTIN_EVC_INTF_ROOT && evcConf->intf[i].mef_type!=PTIN_EVC_INTF_LEAF)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Interface index %u has an invalid mef type (port %u: %u)", i, evcConf->intf[i].intf.value.ptin_port, evcConf->intf[i].mef_type);
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
      #if 0
      if (evcConf->intf[i].vid==0 || evcConf->intf[i].vid>=4095)
      {
        PT_LOG_ERR(LOG_CTX_EVC,"Interface index %u has an invalid vlan (%u/%u: %u)",i,evcConf->intf[i].intf_type,evcConf->intf[i].intf_id,evcConf->intf[i].vid);
        return L7_FAILURE;
      }
      #endif

      port = evcConf->intf[i].intf.value.ptin_port;

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
        if ((evcConf->intf[i].vid > 0 && evcs[evc_id].intf[port].out_vlan   == evcConf->intf[i].vid) &&
            (evcConf->intf[i].vid_inner > 0 && evcs[evc_id].intf[port].inner_vlan == evcConf->intf[i].vid_inner))
        {
          PT_LOG_ERR(LOG_CTX_EVC,"Interface index %u, port=%u of EVC %u (eEVC %u) has the same vlan %u+%u",
                  i, port, evc_id, evcs[evc_id].extended_id, evcConf->intf[i].vid, evcConf->intf[i].vid_inner);
          return L7_FAILURE;
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
L7_RC_t ptin_evc_bwProfile_verify(L7_uint evc_id, ptin_bw_profile_t *profile)
{
  L7_int   ptin_port, i_port,num_ports=0,port_count=0;
  struct ptin_evc_client_s *pclientFlow;
  L7_uint32 list_port[64],i=0;

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

  if (profile->ptin_port_bmp == 0)
  {
    list_port[0] = profile->ptin_port;
    num_ports = 1;
  }
  else
  {   
    while(i<64)
    {    
      if ( ((profile->ptin_port_bmp >>i) & 0x1) == 1)
      {
       list_port[num_ports] = i;
       num_ports++;
      }
      i++; //equal to ptin_port
    }
  }

  while(num_ports -1 >= port_count)
  {
     ptin_port = list_port[port_count];

     PT_LOG_TRACE(LOG_CTX_EVC,"Interface (port_count=%u):" ,port_count);
     PT_LOG_TRACE(LOG_CTX_EVC,"Interface (ptin_port=%u):" ,ptin_port);

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
        profile->vport             = pclientFlow->virtual_gport;
        profile->queue_type        = evcs[evc_id].queue_type;

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
              PT_LOG_ERR(LOG_CTX_EVC,"OVid_in %u does not match to the one in EVC client (%u)",
                         profile->outer_vlan_lookup, pclientFlow->uni_ovid);
              return L7_FAILURE;
            }
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
      port_count++;

    } /* if (profile->ddUsp_src.unit>=0 && profile->ddUsp_src.slot>=0 && profile->ddUsp_src.port>=0) */
    else
    {
      PT_LOG_ERR(LOG_CTX_EVC,"No Interface speficied");
      return L7_FAILURE;
    }
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
    if (IS_EVC_INTF_ROOT(evc_id,ptin_port) || IS_EVC_STD_P2MP(evc_id) || IS_EVC_STD_P2P(evc_id))
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

  /* We should have an outer vlan for this interface */
  if (evcs[evc_id].intf[ptin_port].int_vlan==0 ||
      evcs[evc_id].intf[ptin_port].int_vlan>=4096)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"EVC %u: Internal VLAN %u is not valid!",evc_id,evcs[evc_id].intf[ptin_port].int_vlan);
    return L7_FAILURE;
  }

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

    printf("eEVC# 0x%x (internal id %u)\n", ext_id, evc_id);

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
    printf("  MC Group  = 0x%x\n",   evcs[evc_id].mcgroup);
    if (evcs[evc_id].queue_type == PTIN_EVC_QUEUE_PORT)
    {
      printf("  Queue type= %s\n", "PORT");
    }
    else
    {
      printf("  Queue type= %s\n", evcs[evc_id].queue_type == PTIN_EVC_QUEUE_WIRED ? "WIRED" : "WIRELESS");
    }

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

      printf("    MEF Type      = %s               ", evcs[evc_id].intf[i].type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf");
      if (IS_EVC_IPTV(evc_id) && evcs[evc_id].intf[i].type == PTIN_EVC_INTF_LEAF)
        printf("l3IntfId = %u  ", evcs[evc_id].intf[i].l3_intf_id);
      printf("\r\n");
      printf("    Ext. VLAN     = ");
      if (IS_VLAN_VALID(evcs[evc_id].intf[i].out_vlan))
        printf("%c:%-5u", ptin_vlanxlate_action_getchar(evcs[evc_id].intf[i].action_outer_vlan), evcs[evc_id].intf[i].out_vlan);
      else
        printf("undef  ");
      if (IS_VLAN_VALID(evcs[evc_id].intf[i].inner_vlan))
        printf("+ %c:%-5u", ptin_vlanxlate_action_getchar(evcs[evc_id].intf[i].action_inner_vlan), evcs[evc_id].intf[i].inner_vlan);
      else
        printf("         ");
      printf("   Counter  = %s\n", evcs[evc_id].intf[i].counter != NULL ? "Active":"Disabled");

      printf("    Internal VLAN = %-5u           ", evcs[evc_id].intf[i].int_vlan);
      printf("   BW Prof. = %s\n", evcs[evc_id].intf[i].counter != NULL ? "Active":"Disabled");
      #ifdef PTIN_ERPS_EVC
      printf("    Port State    = %s\n", evcs[evc_id].intf[i].portState == PTIN_EVC_PORT_BLOCKING ? "Blocking":"Forwarding");
      #endif

      /* Only stacked services have clients */
      if (IS_EVC_QUATTRO(evc_id) || IS_EVC_STACKED(evc_id))
      {
        printf("    Clients       = %u\n", evcs[evc_id].intf[i].clients.n_elems);

        /* SEM CLIENTS UP */
        osapiSemaTake(ptin_evc_clients_sem, L7_WAIT_FOREVER);

        pclientFlow = L7_NULLPTR;
        dl_queue_get_head(&evcs[evc_id].intf[i].clients, (dl_queue_elem_t **) &pclientFlow);

        for (j=0; j < evcs[evc_id].intf[i].clients.n_elems && pclientFlow != L7_NULLPTR; j++) {
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
          if (IS_EVC_QUATTRO(evc_id))
          {
            printf("      Flow# %-2u: flags=0x%04x int_vid=%4u+%-4u<->uni_vid=%4u+%-4u [%c+%c] (gport=0x%04x)\r\n", j,
                   pclientFlow->flags & 0xffff, pclientFlow->int_ovid, pclientFlow->int_ivid,
                   pclientFlow->uni_ovid, pclientFlow->uni_ivid,
                   ptin_vlanxlate_action_getchar(pclientFlow->action_outer_vlan), ptin_vlanxlate_action_getchar(pclientFlow->action_inner_vlan),
                   pclientFlow->virtual_gport & 0xffff);
          }
          else
#endif
          {
            printf("      Client# %2u: VID=%4u+%-4u [%c+%c]\r\n", j,
                   pclientFlow->uni_ovid, pclientFlow->int_ivid,
                   ptin_vlanxlate_action_getchar(pclientFlow->action_outer_vlan), ptin_vlanxlate_action_getchar(pclientFlow->action_inner_vlan));
          }

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
    printf("  MC Group  = 0x%x\n",   evcs[evc_id].mcgroup);
    if (evcs[evc_id].queue_type == PTIN_EVC_QUEUE_PORT)
    {
      printf("  Queue type= %s\n", "PORT");
    }
    else
    {
      printf("  Queue type= %s\n", evcs[evc_id].queue_type == PTIN_EVC_QUEUE_WIRED ? "WIRED" : "WIRELESS");
    }

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
      if (IS_EVC_QUATTRO(evc_id) || IS_EVC_STACKED(evc_id))
      {
        printf("    Clients       = %u\n", evcs[evc_id].intf[i].clients.n_elems);

        /* SEM CLIENTS UP */
        osapiSemaTake(ptin_evc_clients_sem, L7_WAIT_FOREVER);

        dl_queue_get_head(&evcs[evc_id].intf[i].clients, (dl_queue_elem_t **) &pclientFlow);

        for (j=0; j<evcs[evc_id].intf[i].clients.n_elems; j++) {
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
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

  if (int_vlan==0 || int_vlan>4095)
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

  printf("+---------+------------+\n");
  printf("| Int. ID |   Ext. ID  |\n");
  printf("+---------+------------+\n");

  for (evc_idx = 0; evc_idx < PTIN_SYSTEM_N_EVCS; evc_idx++)
  {
    if (!evcs[evc_idx].in_use)  continue;
    
    /* Convert to internal evc id */
    evc_ext_id = evcs[evc_idx].extended_id;

    printf("|  %5u  | 0x%08x |\n", evc_idx, evc_ext_id);
  }

  printf("+---------+---------+\n");

  fflush(stdout);
}



void sizeof_evc(void)
{
  PT_LOG_CRITIC(LOG_CTX_EVC, "sizeof(ptin_evc_client_s)     = %zu", sizeof(struct ptin_evc_client_s));
  PT_LOG_CRITIC(LOG_CTX_EVC, "sizeof(ptin_evc_intf_s)       = %zu", sizeof(struct ptin_evc_intf_s));
  PT_LOG_CRITIC(LOG_CTX_EVC, "sizeof(ptin_evc_s)            = %zu", sizeof(struct ptin_evc_s));
  PT_LOG_CRITIC(LOG_CTX_EVC, "sizeof(ptin_IGMP_Statistics_t)= %zu", sizeof(ptin_IGMP_Statistics_t));
  PT_LOG_CRITIC(LOG_CTX_EVC, "PTIN_SYSTEM_N_INTERF          = %u" , PTIN_SYSTEM_N_INTERF);
  PT_LOG_CRITIC(LOG_CTX_EVC, "sizeof(IGMP statistics)       = %zu", sizeof(ptin_IGMP_Statistics_t)*PTIN_SYSTEM_N_INTERF);
  PT_LOG_CRITIC(LOG_CTX_EVC, "sizeof(evcs)                  = %zu", sizeof(evcs));
  PT_LOG_CRITIC(LOG_CTX_EVC, "sizeof(clients)               = %zu", sizeof(clients));
  PT_LOG_CRITIC(LOG_CTX_EVC, "total(evcs+clients)           = %zu", sizeof(evcs)+sizeof(clients));
}

L7_RC_t test_evc_create(L7_uint evc_id, L7_uint16 int_vlan,
                        L7_uint8 port1, L7_uint16 ovid1_val, L7_uint8 ovid1_op, 
                        L7_uint8 port2, L7_uint16 ovid2_val, L7_uint8 ovid2_op)
{
  
  L7_RC_t rc;
  ptin_HwEthMef10Evc_t evcConf;

  /* Create a new EVC */
  memset(&evcConf, 0x00, sizeof(evcConf));
  evcConf.index         = evc_id;
  evcConf.flags         = PTIN_EVC_MASK_MACLEARNING;
  evcConf.mc_flood      = PTIN_EVC_MC_FLOOD_ALL;
  evcConf.internal_vlan = int_vlan;
  evcConf.n_intf        = 2;
  /* Root port 1 */
  evcConf.intf[0].intf.format = PTIN_INTF_FORMAT_PORT;
  evcConf.intf[0].intf.value.ptin_port = port1;
  evcConf.intf[0].mef_type    = PTIN_EVC_INTF_ROOT;
  evcConf.intf[0].vid         = ovid1_val;
  evcConf.intf[0].action_outer= ovid1_op;
  evcConf.intf[0].action_inner= PTIN_XLATE_ACTION_NONE;
  /* Leaf port 2 */
  evcConf.intf[1].intf.format = PTIN_INTF_FORMAT_PORT;
  evcConf.intf[1].intf.value.ptin_port = port2;
  evcConf.intf[1].mef_type    = PTIN_EVC_INTF_LEAF;
  evcConf.intf[1].vid         = ovid2_val;
  evcConf.intf[1].action_outer= ovid2_op;
  evcConf.intf[1].action_inner= PTIN_XLATE_ACTION_NONE;

  /* Creates EVC for Broadlights management */
  rc = ptin_evc_create(&evcConf);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error creating EVC# %u for testing purposes", evc_id);
    return rc;
  }

  return L7_SUCCESS;
}

#ifdef NGPON2_SUPPORTED
void  remove_all_offlineEvc()
{
  ptinExtNGEvcIdInfoData_t  *ext_evcId_infoData;
  ptinExtNGEvcIdDataKey_t    ext_evcId_key;

  memset(&ext_evcId_key, 0x00, sizeof(ptinExtNGEvcIdDataKey_t));
  PT_LOG_ERR(LOG_CTX_MSG, "Error removing EVC# flow");
  /* Search for this extended id */

  while ( ( ext_evcId_infoData = (ptinExtNGEvcIdInfoData_t *)
            avlSearchLVL7(&extNGEvcId_avlTree.extNGEvcIdAvlTree, (void *)&ext_evcId_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&ext_evcId_key, &ext_evcId_infoData->extNGEvcIdDataKey , sizeof(ptinExtNGEvcIdDataKey_t));

    PT_LOG_TRACE(LOG_CTX_MSG, "Removing EVC# %u flow", ext_evcId_infoData->evcNgpon2.index);
    avlDeleteEntry(&(extNGEvcId_avlTree.extNGEvcIdAvlTree), (void *)&ext_evcId_key);
  }

}


void  dump_all_offlineEvc()
{
  ptinExtNGEvcIdInfoData_t  *ext_evcId_infoData;
  ptinExtNGEvcIdDataKey_t    ext_evcId_key;

  L7_uint32 i;

  memset(&ext_evcId_key, 0x00, sizeof(ptinExtNGEvcIdDataKey_t));
  PT_LOG_ERR(LOG_CTX_MSG, "Error removing EVC# flow");
  /* Search for this extended id */

  while ( ( ext_evcId_infoData = (ptinExtNGEvcIdInfoData_t *)
            avlSearchLVL7(&extNGEvcId_avlTree.extNGEvcIdAvlTree, (void *)&ext_evcId_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&ext_evcId_key, &ext_evcId_infoData->extNGEvcIdDataKey , sizeof(ptinExtNGEvcIdDataKey_t));

    printf("EVC# %u \n", ext_evcId_infoData->evcNgpon2.index);
    printf("Number of interfaces %u \n", ext_evcId_infoData->evcNgpon2.n_intf);
    printf("Flags %u \n", ext_evcId_infoData->evcNgpon2.flags);

    for (i=0 ; i<ext_evcId_infoData->evcNgpon2.n_intf; i++)
    {
      if (ext_evcId_infoData->evcNgpon2.intf[i].intf.value.ptin_intf.intf_type == PTIN_EVC_INTF_NGPON2)
      {
         printf("Intf type %s, ", "NGPON");
      }
      else
      {
        printf("Intf type %s, ",   ext_evcId_infoData->evcNgpon2.intf[i].intf.value.ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY" : "LAG" );
      }
      printf("Intf id %d \n",   ext_evcId_infoData->evcNgpon2.intf[i].intf.value.ptin_intf.intf_id);
      printf("MEF  type %s \n",   ext_evcId_infoData->evcNgpon2.intf[i].mef_type == PTIN_EVC_INTF_ROOT ? "Root" : "Leaf");
      printf("Intf format %d \n \n",   ext_evcId_infoData->evcNgpon2.intf[i].intf.format);
    }

    printf("\n");
  }

}
#endif

