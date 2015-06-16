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
#include "ptin_fpga_api.h"
#include "ptin_debug.h"
#include "dtl_l3_mcast_api.h"

#define IGMP_INVALID_ENTRY    0xFF

/******************************* 
 * FEATURES
 *******************************/

/* Comment the follwing line, if you don't want to use client timers */
#define CLIENT_TIMERS_SUPPORTED

#define PTIN_MGMD_MC_SERVICE_ID_IN_USE 

#define PTIN_CLIENT_IS_MASKBITSET(array,idx) ((array[(idx)/(sizeof(L7_uint8)*8)] >> ((idx)%(sizeof(L7_uint8)*8))) & 1)
#define PTIN_CLIENT_IS_MASKBYTESET(array, idx)  (array[((idx)/(8*sizeof(uint8)))] == 0 ? 0 : 1)  
#define PTIN_CLIENT_SET_MASKBIT(array,idx)   { array[(idx)/(sizeof(L7_uint8)*8)] |=   (L7_uint32) 1 << ((idx)%(sizeof(L7_uint8)*8)) ; }
#define PTIN_CLIENT_UNSET_MASKBIT(array,idx) { array[(idx)/(sizeof(L7_uint8)*8)] &= ~((L7_uint32) 1 << ((idx)%(sizeof(L7_uint8)*8))); }

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

/******************************* 
 * QUEUES
 *******************************/

struct ptinIgmpClientInfoData_s;

/* Client indexes pool */
typedef struct ptinIgmpClientId_s
{
  struct ptinIgmpClientId_s *next;
  struct ptinIgmpClientId_s *prev;

  L7_BOOL   inUse;
  L7_uint16 clientId;
} ptinIgmpClientId_t;

/* Client info pool */
typedef struct ptinIgmpClientDevice_s
{
  struct ptinIgmpClientDevice_s *next;
  struct ptinIgmpClientDevice_s *prev;

  struct ptinIgmpClientInfoData_s *client;
} ptinIgmpDeviceClient_t;

/* Client Device Indentifier Pool */
struct ptinIgmpClientId_s pool_device_client_id[PTIN_IGMP_INTFPORT_MAX][PTIN_IGMP_CLIENTIDX_MAX];

/* Client Group Identifier Pool */
struct ptinIgmpClientId_s pool_group_client_id[PTIN_IGMP_INTFPORT_MAX][PTIN_IGMP_CLIENTIDX_MAX];

/* Queue of free Device Client Identifiers */
static dl_queue_t queue_free_device_client_id[PTIN_IGMP_INTFPORT_MAX];

/* Queue of free Group Client Identifiers */
static dl_queue_t queue_free_group_client_id[PTIN_IGMP_INTFPORT_MAX];

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
  ptin_client_id_t  key;
  L7_BOOL           in_use;
  void             *next;
} ptinIgmpClientGroupsSnapshotInfoData_t;

typedef struct
{
  avlTree_t                               avlTree;
  avlTreeTables_t                        *treeHeap;
  ptinIgmpClientGroupsSnapshotInfoData_t *dataHeap;
} ptinIgmpClientGroupsSnapshotAvlTree_t;

#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
typedef struct
{
  L7_uint8                  mask;                 /*mask = 0x00 Disable | mask !=0x00 Enable*/
  L7_uint16                 maxAllowedChannels;   /* [mask 0x01] Maximum number of channels allowed for this client */
  L7_uint32                 maxAllowedBandwidth;  /* [mask 0x02] Maximum bandwidth allowed for this client (kbit/s)*/
  L7_uint16                 allocatedChannels;     /* Number of channels currently allocated  for this client */
  L7_uint32                 allocatedBandwidth;    /* Bandwidth currently allocated for this client (kbit/s) */
} ptinIgmpAdmissionControl_t;
#endif

/* Client Groups */
typedef struct ptinIgmpClientGroupInfoData_s
{
  ptinIgmpClientDataKey_t     igmpClientDataKey;
  L7_uint16                   uni_ovid;               /* Ext. OVID to be used for packet transmission */
  L7_uint16                   uni_ivid;               /* Ext. IVID to be used for packet transmission */
  L7_uint8                    ptin_port;              /* Port */
  L7_uint32                   client_bmp_list[PTIN_IGMP_CLIENT_BITMAP_SIZE];  /* Clients (children) bitmap (only for one interface) */
  dl_queue_t                  queue_clientDevices;
  ptin_IGMP_Statistics_t      stats_client;
  L7_uint8                    onuId;
  L7_uint8                    groupClientId;
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
  ptinIgmpAdmissionControl_t  admissionControl;   
#endif
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  L7_uint32                   package_bmp_list[PTIN_IGMP_PACKAGE_BITMAP_SIZE];  /* package bitmap list */
  L7_uint8                    number_of_packages;
#endif
  void *next;
} ptinIgmpGroupClientInfoData_t;

typedef struct
{
  avlTree_t                     igmpClientsAvlTree;
  avlTreeTables_t               *igmpClientsTreeHeap;
  ptinIgmpGroupClientInfoData_t *igmpClientsDataHeap;
} ptinIgmpGroupClientAvlTree_t;

typedef struct
{
  L7_uint16                        number_of_clients;                                  /* Total number of clients */
  ptinIgmpGroupClientInfoData_t   *group_client[PTIN_IGMP_INTFPORT_MAX][PTIN_IGMP_CLIENTIDX_MAX];
  ptinIgmpGroupClientAvlTree_t     avlTree;
} ptinIgmpClientGroups_t;


/* Child clients (devices) */
typedef struct ptinIgmpClientInfoData_s
{
  ptinIgmpClientDataKey_t   igmpClientDataKey;
  L7_uint8                  ptin_port;              /* Port */
  L7_uint16                 deviceClientId;
  L7_uint16                 uni_ovid;               /* Ext. OVID to be used for packet transmission */
  L7_uint16                 uni_ivid;               /* Ext. IVID to be used for packet transmission */
  L7_BOOL                   isDynamic;
  L7_uint16                 number_of_mc_services;  /* A client may be part of several MC services: this will benused for AVL tree management */
  struct ptinIgmpClientGroupInfoData_s *pClientGroup;    /* Pointer to ClientGroup (null for client groups avl tree) */
  void *next;
} ptinIgmpClientInfoData_t;

typedef struct
{
  avlTree_t                  igmpClientsAvlTree;
  avlTreeTables_t           *igmpClientsTreeHeap;
  ptinIgmpClientInfoData_t  *igmpClientsDataHeap;
} ptinIgmpClientsAvlTree_t;

typedef struct
{
  L7_uint16                      number_of_clients;                                  /* Total number of clients */
  L7_uint16                      number_of_clients_per_intf[PTIN_SYSTEM_N_INTERF];   /* Number of clients per interface for one IGMP instance */

  ptinIgmpDeviceClient_t         client_devices[PTIN_IGMP_INTFPORT_MAX][PTIN_IGMP_CLIENTIDX_MAX];

  dl_queue_t                     queue_free_clientDevices[PTIN_IGMP_INTFPORT_MAX]; /* Queue with free (device) clients */

  ptinIgmpClientsAvlTree_t       avlTree;
  L7_APP_TMR_CTRL_BLK_t          timerCB;       /* Entry App Timer Control Block */

  L7_sll_t                       ll_timerList;  /* Linked list of timer data nodes */
  L7_uint32                      ctrlBlkBufferPoolId;
  L7_uint32                      appTimerBufferPoolId;
  handle_list_t                 *appTimer_handle_list;
  void                          *appTimer_handleListMemHndl;
} ptinIgmpClients_unified_t;

/******************************* 
 * GLOBAL STRUCTS
 *******************************/

/* Snapshot list of client groups, required for the GROUP_CLIENTS API */
ptinIgmpClientGroupsSnapshotAvlTree_t igmpSnapshotClientGroups;

/* Client Groups (to be added manually) */
ptinIgmpClientGroups_t igmpGroupClients;

/* Unified list with all clients (to be added dynamically) */
ptinIgmpClients_unified_t igmpDeviceClients;

/******************************* 
 * MULTI MULTICAST FEATURE
 *******************************/

/** Service association AVL Tree */
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED

/* Optional */
  #define IGMPASSOC_CHANNEL_UC_EVC_ISOLATION  0
  #define IGMPASSOC_CHANNEL_SOURCE_SUPPORTED  1

/* IGMP Channel */
typedef struct
{
  L7_uint32       evc_mc;

  L7_inet_addr_t  channel_group;

#if (IGMPASSOC_CHANNEL_SOURCE_SUPPORTED)
  L7_inet_addr_t  channel_source;
#endif

#if IGMPASSOC_CHANNEL_UC_EVC_ISOLATION
  L7_uint32 evc_uc;
#endif
} ptinIgmpChannelDataKey_t;

typedef enum
{
  PTIN_IGMP_ASSOC_DYNAMIC_ENTRY = 1,
  PTIN_IGMP_ASSOC_STATIC_ENTRY,  
  PTIN_IGMP_ASSOC_DYNAMIC_AND_STATIC_ENTRY
} ptinIgmpAssocEntryType_t;

typedef struct
{
  ptinIgmpChannelDataKey_t  channelDataKey;        
  L7_uint8                  entryType; //ptinIgmpAssocEntryType_t
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
  L7_uint32                 channelBandwidth;  // kbps 
#endif  
  dl_queue_t                queuePackage;    /* Pool of Package Identifiers*/
  L7_uint32                 groupClientBmpPerPort[PTIN_SYSTEM_N_UPLINK_INTERF][PTIN_IGMP_CLIENT_BITMAP_SIZE];    
  L7_uint8                  noOfGroupClientsPerPort[PTIN_SYSTEM_N_UPLINK_INTERF];  
  L7_uint32                 portBmp[PTIN_SYSTEM_N_UPLINK_INTERF/(sizeof(L7_uint32)*8)+1];
  L7_uint8                  noOfPorts;
  void                     *next; /*AVL Tree Element*/
} ptinIgmpChannelInfoData_t;

typedef struct
{  
  L7_uint32                  default_evc_mc;
  L7_uint8                   default_evc_mc_is_in_use;
  L7_uint32                  default_bandwidth;

  avlTree_t                  channelAvlTree;
  avlTreeTables_t           *channelTreeHeap;
  ptinIgmpChannelInfoData_t *channelDataHeap;
} ptinIgmpChannelAvlTree_t;

/* List of all IGMP associations */
static ptinIgmpChannelAvlTree_t channelDB;

/* Prototypes */
static L7_RC_t ptin_igmp_channel_get( L7_uint32 evc_mc,
                                      L7_inet_addr_t *channel_group,
                                      L7_inet_addr_t *channel_source,
                                      ptinIgmpChannelInfoData_t **avlEntry );

static L7_RC_t ptin_igmp_channel_to_netmask( L7_inet_addr_t *channel_in, L7_uint16 channel_mask,
                                             L7_inet_addr_t *channel_out, L7_uint32 *number_of_channels);
static L7_RC_t ptin_igmp_channel_add( ptinIgmpChannelInfoData_t *node );
static L7_RC_t ptin_igmp_channel_remove( ptinIgmpChannelDataKey_t *avl_key );
static L7_RC_t ptin_igmp_channel_remove_multicast_service ( L7_uint32 evc_uc, L7_uint32 evc_mc );
static L7_RC_t ptin_igmp_channel_remove_all ( void );

static ptinIgmpGroupClientInfoData_t* deviceClientId2groupClientPtr(L7_uint32 ptinPort, L7_uint32 clientId);
static RC_t ptin_igmp_multicast_channel_service_get(L7_uint32 ptinPort, L7_uint32 clientId, L7_inet_addr_t *groupAddr, L7_inet_addr_t *sourceAddr, L7_uint32 *serviceId);

static RC_t ptin_igmp_max_mask_size(L7_uchar8 family, L7_uchar8 *maxMasklen);

static RC_t ptin_igmp_max_mask_size(L7_uchar8 family, L7_uchar8 *maxMasklen)
{
  if (maxMasklen == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Null Pointer");
    return L7_FAILURE;
  }
  if (family == L7_AF_INET)
  {
    *maxMasklen = 32;
    return L7_SUCCESS;
  }
  else if (family == L7_AF_INET6)
  {
    *maxMasklen = 128;
    return L7_SUCCESS;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid Family Type:%u", family);    
    return L7_FAILURE;
  }
}

/************IGMP Admission Control Feature****************************************************/ 
  #if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT

typedef struct
{
  ptinIgmpAdmissionControl_t   admissionControl;         
} ptinIgmpAdmissionControlPort_t;

typedef struct
{
  ptinIgmpAdmissionControl_t   admissionControl;         
} igmpMulticastAdmissionControl_t;

typedef struct
{
  L7_uint8       inUse;
  L7_uint32      evc_mc;
  L7_inet_addr_t channel_group;
#if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
  L7_inet_addr_t  channel_source;
#endif
  L7_uint32      channelBandwidth;         
} ptinIgmpChannelBandwidthCache_t;

static ptinIgmpChannelBandwidthCache_t ptinIgmpChannelBandwidthCache;

static L7_uint8 ptinIgmpAdmissionControlMulticastInternalServiceId[PTIN_SYSTEM_N_EXTENDED_EVCS] ;

static L7_uint32 ptinIgmpAdmissionControlMulticastExternalServiceId[PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID];

static igmpMulticastAdmissionControl_t igmpMulticastAdmissionControl[PTIN_SYSTEM_N_UPLINK_INTERF][PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF][PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID];

static ptinIgmpAdmissionControlPort_t igmpPortAdmissionControl[PTIN_SYSTEM_N_UPLINK_INTERF];

  #endif
/*******************End IGMP Admission Control Feature***********************************************/

/***************Multicast Channel Package Feature****************************************************/ 

/* Package Pool Entry*/
typedef struct
{
  L7_uint8       packageId;
  L7_uint8       inUse;
  L7_uint32      groupClientBmpPerPort[PTIN_SYSTEM_N_UPLINK_INTERF][PTIN_IGMP_CLIENT_BITMAP_SIZE];  /* groupClient Bitmap*/
  L7_uint32      portBmp[PTIN_IGMP_PORT_BITMAP_SIZE];
  L7_uint32      noOfGroupClientsPerPort[PTIN_SYSTEM_N_UPLINK_INTERF];  /* Number of groupClients per port*/
  L7_uint32      noOfPorts;  /* Number of ports*/
  dl_queue_t     queueChannel;  
} ptinIgmpMulticastPackage_t;

/* Multicast Service Identifier*/
typedef struct
{
  L7_uint8       inUse;
  L7_uint32      serviceId; 
} ptinIgmpMulticastServiceId_t;

/* Number of Multicast Services*/
typedef struct
{  
  L7_uint32      noOfMulticastServices; 
} ptinIgmpNoOfMulticastServices_t;

/* Package Identifier Pool Entry */
struct packagePoolEntryId_s
{
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct    packagePoolEntryId_s *next;
  struct    packagePoolEntryId_s *prev;

  L7_uint32 entryId; 
};

/* Package Identifier Pool Entry */
struct packagePoolEntry_s
{
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct    packagePoolEntry_s *next;
  struct    packagePoolEntry_s *prev;

  L7_uint32 packageEntryId; /*Index on the Pool*/
  L7_uint32 packageId; 

};

/* Channel Identifier Pool Entry */
struct channelPoolEntryId_s
{
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct    channelPoolEntryId_s *next;
  struct    channelPoolEntryId_s *prev;

  L7_uint32 entryId; 
};

/* Channel Pool Entry */
struct channelPoolEntry_s
{
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct channelPoolEntry_s      *next;
  struct channelPoolEntry_s      *prev;

  L7_uint32                      channelEntryId;      /*Index on the Pool*/
  ptinIgmpChannelInfoData_t     *channelAvlTreeEntry;
};

static struct channelPoolEntryId_s         channelIdPoolEntry[PTIN_SYSTEM_IGMP_MAXPACKAGES_CHANNEL_ENTRIES]; /*Channel Identifiers Pool*/
static dl_queue_t                          queueFreeChannelId;    /* Queue of Free Channel Identifiers*/

static struct channelPoolEntry_s           channelPoolEntry[PTIN_SYSTEM_IGMP_MAXPACKAGES_CHANNEL_ENTRIES]; /*Channel Data Pool*/

static struct packagePoolEntryId_s         packageIdPoolEntry[PTIN_SYSTEM_IGMP_MAXPACKAGES_CHANNEL_ENTRIES]; /*Package Identifiers Pool*/
static dl_queue_t                          queueFreePackageId; /* Queue of Free Package Identifiers*/

static struct packagePoolEntry_s           packagePoolEntry[PTIN_SYSTEM_IGMP_MAXPACKAGES_CHANNEL_ENTRIES]; /*Package Data Pool*/


static L7_uint8                            noOfMulticastPackages;
static ptinIgmpMulticastPackage_t          multicastPackage[PTIN_SYSTEM_IGMP_MAXPACKAGES]; /*Multicast Packages*/

static ptinIgmpNoOfMulticastServices_t     multicastServices[PTIN_SYSTEM_N_UPLINK_INTERF][PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF];
static ptinIgmpMulticastServiceId_t        multicastServiceId[PTIN_SYSTEM_N_UPLINK_INTERF][PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF][PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID];

static void ptin_igmp_multicast_service_reset(void);

static RC_t   queue_channel_entry_add(L7_uint32 packageId, ptinIgmpChannelInfoData_t *channelAvlTreeEntry);
static RC_t   queue_channel_entry_remove(L7_uint32 packageId, ptinIgmpChannelInfoData_t *channelAvlTreeEntry);
static RC_t   queue_channel_entry_find(L7_uint32 packageId, ptinIgmpChannelInfoData_t *channelAvlTreeEntry, struct channelPoolEntry_s **channelEntry);
static struct channelPoolEntry_s* queue_channel_entry_get_next(L7_uint32 packageId, struct channelPoolEntry_s *channelEntry);

static RC_t   queue_package_entry_add(L7_uint32 packageId, ptinIgmpChannelInfoData_t *channelAvlTreeEntry);
static RC_t   queue_package_entry_remove(L7_uint32 packageId, ptinIgmpChannelInfoData_t *channelAvlTreeEntry);
static RC_t   queue_package_entry_find(L7_uint32 packageId, ptinIgmpChannelInfoData_t *channelAvlTreeEntry, struct packagePoolEntry_s  **packageEntry );
static struct packagePoolEntry_s* queue_package_entry_get_next(ptinIgmpChannelInfoData_t *channelAvlTreeEntry, struct packagePoolEntry_s  *packageEntry);

static RC_t ptin_igmp_multicast_client_package_add(L7_uint32 packageId, ptinIgmpGroupClientInfoData_t *groupClient);
static RC_t ptin_igmp_multicast_client_package_remove(L7_uint32 packageId, ptinIgmpGroupClientInfoData_t *groupClient);
//static RC_t ptin_igmp_multicast_client_package_get(L7_uint32 packageId, ptinIgmpGroupClientInfoData_t *groupClient, L7_BOOL *isBitSet);

static RC_t ptin_igmp_multicast_client_packages_add(L7_uint32 *packagePtr, L7_uint32 noOfPackages, ptinIgmpGroupClientInfoData_t *groupClient);
static RC_t ptin_igmp_multicast_client_packages_remove(L7_uint32 *packagePtr, L7_uint32 noOfPackages, ptinIgmpGroupClientInfoData_t *groupClient);
static RC_t ptin_igmp_multicast_client_packages_remove_all(ptinIgmpGroupClientInfoData_t *groupClient);

/**
 * @purpose Add Multicast Channel to a Package
 * 
 * @param packageId 
 * @param serviceId 
 * @param groupAddr 
 * @param sourceAddr 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
static RC_t ptin_igmp_multicast_package_channel_add(L7_uint32 packageId, L7_uint32 serviceId, L7_inet_addr_t *groupAddr, L7_inet_addr_t *sourceAddr);

/**
 * @purpose Remove Multicast Channel to a Package
 * 
 * @param packageId 
 * @param serviceId 
 * @param groupAddr 
 * @param sourceAddr 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
static RC_t ptin_igmp_multicast_package_channel_remove(L7_uint32 packageId, L7_uint32 serviceId, L7_inet_addr_t *groupAddr, L7_inet_addr_t *sourceAddr);

/**
 * @purpose Verify if a given channel exists in another package with a different multicast service
 * 
 * @param packageId               : [in]  Package Identifier
 * @param groupClient             : [in]  Group Client Pointer 
 * @param channelAvlTreeEntry     : [in]  Channel AVL Tree Pointer
 *  
 * @return RC_t                   
 *
 * @notes  
 *  
 */
static RC_t ptin_igmp_client_channel_conflict_validation(L7_uint32 packageId, ptinIgmpGroupClientInfoData_t *groupClient, ptinIgmpChannelInfoData_t  *channelAvlTreeEntry);

/**
 * @purpose Verify if this client can be or not removed from this channel: i.e. this client has another package with this channel 
 * 
 * @param packageId               : [in]  Package Identifier
 * @param groupClient             : [in]  Group Client Pointer 
 * @param channelAvlTreeEntry     : [in]  Channel AVL Tree Pointer
 *  
 * @return RC_t                   
 *
 * @notes  
 *  
 */
static RC_t ptin_igmp_client_channel_dependency_validation(L7_uint32 packageId, ptinIgmpGroupClientInfoData_t *groupClient, ptinIgmpChannelInfoData_t  *channelAvlTreeEntry);

/**
 * @purpose Verify if this channel can be added to this package: i.e. this channel already exists in this package, but it belongs to a different multicast service
 * 
 * @param packageId               : [in]  Package Identifier * 
 * @param channelAvlTreeEntry     : [in]  Channel AVL Tree Pointer
 *  
 * @return RC_t                   
 *
 * @notes  
 *  
 */
static RC_t ptin_igmp_package_channel_conflict_validation(L7_uint32 packageId, ptinIgmpChannelInfoData_t  *channelAvlTreeEntry);

/**
 * @purpose Add a given client to a Multicast Channel  
 *  
 * @param  packageId
 * @param *groupClient
 * @param *channelAvlTreeEntry 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
static RC_t ptin_igmp_multicast_channel_client_add(L7_uint32 packageId, ptinIgmpGroupClientInfoData_t *groupClient, ptinIgmpChannelInfoData_t *channelAvlTreeEntry);

/**
 * @purpose Remove a given client from a Multicast Channel  
 *  
 * @param  packageId
 * @param *groupClient
 * @param *channelAvlTreeEntry 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
static RC_t ptin_igmp_multicast_channel_client_remove(L7_uint32 packageId, ptinIgmpGroupClientInfoData_t *groupClient, ptinIgmpChannelInfoData_t *channelAvlTreeEntry);

/************End Multicast Channel Package Feature****************************************************/ 


#endif/*IGMPASSOC_MULTI_MC_SUPPORTED*/

/**
 * Pop new Group client identifier 
 */
static L7_uint16 ptin_igmp_group_client_identifier_pop(L7_uint ptin_port);

/**
 * Push existing device client identifier 
 */
static void ptin_igmp_group_client_identifier_push(L7_uint ptin_port, L7_uint16 client_id);

/******************************* 
 * IGMP INSTANCES STRUCTS
 *******************************/

/* IGMP Instance config struct
 * IMPORTANT:
 *   1. only ONE root is allowed per instance
 *   2. only ONE vlan is allowed for all client interfaces
 *   3. the unicast VLAN cannot be used on multiple IGMP instances */ 
typedef struct
{
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

/* MGMD Query Instance Array  */
#if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
mgmdQueryInstances_t  mgmdQueryInstances[PTIN_SYSTEM_N_EVCS];
L7_uint32             mgmdNumberOfQueryInstances=0;
#endif

/* Configuration structures */
ptin_IgmpProxyCfg_t igmpProxyCfg;

/* Arrays to map the number of IGMP instances that use each interface */
L7_uint8 igmpClientsIntf[PTIN_SYSTEM_N_INTERF];
L7_uint8 igmpRoutersIntf[PTIN_SYSTEM_N_INTERF];

/* Global IGMP statistics at interface level */
static ptin_IGMP_Statistics_t global_stats_intf[PTIN_SYSTEM_N_INTERF];

/* Lookup tables (invalid entry: 0xFF - IGMP_INVALID_ENTRY)
 * IMPORTANT:
 *   These indexed VLANs are the ones after translation (internal VLANs).
 *   The main reason is that there might be multiple IGMP instances that use
 *   the same VLANs, but on different interfaces. Only the interval VLAN will
 *   differenciate them. */ 
L7_uint8 igmpInst_fromRouterVlan[4096];  /* Lookup table to get IGMP instance index based on Router (root) VLAN */
L7_uint8 igmpInst_fromUCVlan[4096];      /* Lookup table to get IGMP instance index based on Unicast (clients uplink) VLAN */

#define QUATTRO_IGMP_TRAP_PREACTIVE     0   /* To always have this rule active, set 1 */
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
/* Global number of QUATTRO P2P flows */
static L7_uint32 igmp_quattro_stacked_evcs = 0;
#endif


/* Local functions prototypes */
static L7_RC_t ptin_igmp_group_client_find(ptin_client_id_t *client_ref, ptinIgmpGroupClientInfoData_t **client_info);

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
static L7_RC_t ptin_igmp_evc_trap_configure(L7_uint32 evc_idx, L7_BOOL enable);
#endif
static L7_RC_t ptin_igmp_instance_find_agg(L7_uint16 nni_ovlan, L7_uint *igmp_idx);

static L7_RC_t ptin_igmp_querier_configure(L7_uint igmp_idx, L7_BOOL enable);
static L7_RC_t ptin_igmp_evc_querier_configure(L7_uint32 evc_idx, L7_BOOL enable);
L7_RC_t ptin_igmp_mgmd_whitelist_add(L7_uint16 serviceId, L7_uint32 groupAddr, L7_uint8 groupMaskLen, L7_uint32 sourceAddr, L7_uint8 sourceMaskLen, L7_uint64 bw);
L7_RC_t ptin_igmp_mgmd_whitelist_remove(L7_uint16 serviceId, L7_uint32 groupAddr, L7_uint8 groupMaskLen, L7_uint32 sourceAddr, L7_uint8 sourceMaskLen, L7_uint64 bw);
L7_RC_t ptin_igmp_mgmd_whitelist_clean(void);
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

/************IGMP Admission Control Feature****************************************************/ 
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT

static void ptin_igmp_channel_bandwidth_cache_set(ptinIgmpChannelInfoData_t* ptinIgmpPairInfoData);

static void ptin_igmp_channel_bandwidth_cache_unset(ptinIgmpChannelDataKey_t* ptinIgmpPairDataKey);

static ptinIgmpChannelBandwidthCache_t* ptin_igmp_channel_bandwidth_cache_get(void);

static void ptin_igmp_admission_control_reset_allocation(void);

#endif
/*******************End IGMP Admission Control Feature***********************************************/

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
static L7_uint16 ptin_igmp_device_client_identifier_pop(L7_uint ptin_port, ptinIgmpGroupClientInfoData_t* clientGroup);
/* Get new client index */
static void ptin_igmp_device_client_identifier_push(L7_uint ptin_port, L7_uint16 client_idx);
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

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
/**
 * Initializes Package Feature
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_igmp_multicast_package_init(void)
{
  L7_uint32 iterator;
  L7_RC_t   rc;

  /*Initialize Multicast Packages*/
  {
    noOfMulticastPackages = 0;

    memset(&multicastPackage, 0x00, sizeof(multicastPackage));
  }

  /*Initialize Channel Pool*/
  {
    /*Initialize Free Channel Pool*/
    memset(&channelPoolEntry, 0x00, sizeof(channelPoolEntry));

    for (iterator = 0; iterator < PTIN_SYSTEM_IGMP_MAXPACKAGES; iterator++)
    {
      /* Init Channel Queue*/
      if ( (rc = dl_queue_init(&multicastPackage[iterator].queueChannel)) != L7_SUCCESS )
      {
        LOG_FATAL(LOG_CTX_PTIN_IGMP,"Failed to Initialize Queue on packageId:%u", iterator );
        return rc;
      }
    }

    /* Init Free Channel Queue*/
    dl_queue_init(&queueFreeChannelId);

    /*Init Free Channel Identifier Pool*/    
    memset(channelIdPoolEntry, 0x00, sizeof(channelIdPoolEntry));

    for ( iterator = 0; iterator <PTIN_SYSTEM_IGMP_MAXPACKAGES_CHANNEL_ENTRIES; iterator++)
    {
      channelIdPoolEntry[iterator].entryId = iterator;

      if ( ( rc = dl_queue_add(&queueFreeChannelId, (dl_queue_elem_t*) &channelIdPoolEntry[iterator]) ) != L7_SUCCESS)
      {
        LOG_FATAL(LOG_CTX_PTIN_IGMP,"Failed to Add Queue on packageId:%u", iterator );
        return rc;
      }
    }
  }

  /*Initialize Package Pool*/
  {
    /*Initialize Free Package Pool*/
    memset(&packagePoolEntry, 0x00, sizeof(packagePoolEntry));

    /*This operation is been performed each time a channel is being added in the Avl Tree*/
//  dl_queue_init(&avl_infoData->queuePackage);

    /* Init Package Queue*/
    if ( ( rc = dl_queue_init(&queueFreePackageId) ) != L7_SUCCESS)
    {
      LOG_FATAL(LOG_CTX_PTIN_IGMP,"Failed to Initialize Queue of packageId");
      return rc;
    }

    /*Initialize Free Package Identifier Pool*/
    memset(&packageIdPoolEntry, 0x00, sizeof(packageIdPoolEntry));

    for ( iterator = 0; iterator <PTIN_SYSTEM_IGMP_MAXPACKAGES_CHANNEL_ENTRIES; iterator++)
    {
      packageIdPoolEntry[iterator].entryId = iterator;

      if ( L7_SUCCESS != ( rc = dl_queue_add(&queueFreePackageId, (dl_queue_elem_t*) &packageIdPoolEntry[iterator]) ) )
      {
        LOG_FATAL(LOG_CTX_PTIN_IGMP,"Failed to Add Element to Queue: packageId:%u", iterator);
        return rc;
      }
    }
  }
  return L7_SUCCESS;
}
#endif

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

  /*Igmp Proxy Default Configuration*/
  ptin_igmp_proxy_defaultcfg_load();

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

  if (L7_SUCCESS != ptin_igmp_clientGroupSnapshot_clean())
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to clear clientGroupSnapshot avlTree");
    return L7_FAILURE;
  }

  /* Client group */
  memset(&igmpGroupClients, 0x00, sizeof(igmpGroupClients));

#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT

  /* Channel Bandwidth Cache*/
  memset(&ptinIgmpChannelBandwidthCache, 0x00, sizeof(ptinIgmpChannelBandwidthCache));

  /* Port Admission Control*/
  memset(&igmpPortAdmissionControl, 0x00, sizeof(igmpPortAdmissionControl));

  /* Multicast Admission Control*/
  memset(&igmpMulticastAdmissionControl, 0x00, sizeof(igmpPortAdmissionControl));  


  L7_uint8 internalServiceId;
  for (internalServiceId = 0; internalServiceId < PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID; internalServiceId++)
  {
    ptinIgmpAdmissionControlMulticastExternalServiceId[internalServiceId] = (L7_uint32) -1;
  }

  L7_uint32 externalServiceId;
  for (externalServiceId = 0; externalServiceId < PTIN_SYSTEM_N_EXTENDED_EVCS; externalServiceId++)
  {
    ptinIgmpAdmissionControlMulticastInternalServiceId[externalServiceId] = (L7_uint8) -1;
  }
#endif

  igmpGroupClients.avlTree.igmpClientsTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_IGMP_MAXONUS * sizeof(avlTreeTables_t)); 
  igmpGroupClients.avlTree.igmpClientsDataHeap = (ptinIgmpGroupClientInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_IGMP_MAXONUS * sizeof(ptinIgmpGroupClientInfoData_t)); 

  if ((igmpGroupClients.avlTree.igmpClientsTreeHeap == L7_NULLPTR) ||
      (igmpGroupClients.avlTree.igmpClientsDataHeap == L7_NULLPTR))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error allocating data for IGMP AVL Trees\n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset (&igmpGroupClients.avlTree.igmpClientsAvlTree, 0x00, sizeof(avlTree_t));
  memset ( igmpGroupClients.avlTree.igmpClientsTreeHeap, 0x00, sizeof(avlTreeTables_t)*PTIN_SYSTEM_IGMP_MAXONUS);
  memset ( igmpGroupClients.avlTree.igmpClientsDataHeap, 0x00, sizeof(ptinIgmpGroupClientInfoData_t)*PTIN_SYSTEM_IGMP_MAXONUS);

  // AVL Tree creations - snoopIpAvlTree
  avlCreateAvlTree(&(igmpGroupClients.avlTree.igmpClientsAvlTree),
                   igmpGroupClients.avlTree.igmpClientsTreeHeap,
                   igmpGroupClients.avlTree.igmpClientsDataHeap,
                   PTIN_SYSTEM_IGMP_MAXONUS, 
                   sizeof(ptinIgmpGroupClientInfoData_t),
                   0x10,
                   sizeof(ptinIgmpClientDataKey_t));

  /* No of client groups */
  igmpGroupClients.number_of_clients = 0;

  /* INDIVIUAL CLIENTS INITIALIZATION */
  /* Reset unified list of clients */
  memset(&igmpDeviceClients, 0x00, sizeof(igmpDeviceClients));

  igmpDeviceClients.avlTree.igmpClientsTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_IGMP_MAXCLIENTS * sizeof(avlTreeTables_t)); 
  igmpDeviceClients.avlTree.igmpClientsDataHeap = (ptinIgmpClientInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_IGMP_MAXCLIENTS * sizeof(ptinIgmpClientInfoData_t)); 

  if ((igmpDeviceClients.avlTree.igmpClientsTreeHeap == L7_NULLPTR) ||
      (igmpDeviceClients.avlTree.igmpClientsDataHeap == L7_NULLPTR))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error allocating data for IGMP AVL Trees\n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset (&igmpDeviceClients.avlTree.igmpClientsAvlTree, 0x00, sizeof(avlTree_t));
  memset ( igmpDeviceClients.avlTree.igmpClientsTreeHeap, 0x00, sizeof(avlTreeTables_t)*PTIN_SYSTEM_IGMP_MAXCLIENTS);
  memset ( igmpDeviceClients.avlTree.igmpClientsDataHeap, 0x00, sizeof(ptinIgmpClientInfoData_t)*PTIN_SYSTEM_IGMP_MAXCLIENTS);

  // AVL Tree creations - snoopIpAvlTree
  avlCreateAvlTree(&(igmpDeviceClients.avlTree.igmpClientsAvlTree),
                   igmpDeviceClients.avlTree.igmpClientsTreeHeap,
                   igmpDeviceClients.avlTree.igmpClientsDataHeap,
                   PTIN_SYSTEM_IGMP_MAXCLIENTS, 
                   sizeof(ptinIgmpClientInfoData_t),
                   0x10,
                   sizeof(ptinIgmpClientDataKey_t));

  /* No clients */
  igmpDeviceClients.number_of_clients = 0;
  memset(igmpDeviceClients.number_of_clients_per_intf,0x00,sizeof(igmpDeviceClients.number_of_clients_per_intf));
  /* Initialize clients list */
  memset(&igmpDeviceClients.client_devices, 0x00, sizeof(igmpDeviceClients.client_devices));

  /* Client Device Identifier Pool */
  memset(pool_device_client_id, 0x00, sizeof(pool_device_client_id));

  /* Client Group Identifier Pool */
  memset(pool_group_client_id, 0x00, sizeof(pool_group_client_id));

  for (port=0; port<PTIN_IGMP_INTFPORT_MAX; port++)
  {
    /*Device Clients*/
    dl_queue_init(&queue_free_device_client_id[port]);

    /*Group Clients*/
    dl_queue_init(&queue_free_group_client_id[port]);

    for (i = 0; i < PTIN_IGMP_CLIENTIDX_MAX; i++)
    {
      /*Device Clients*/
      {
        pool_device_client_id[port][i].clientId =  i;
        pool_device_client_id[port][i].inUse     = L7_FALSE;
        dl_queue_add_tail(&queue_free_device_client_id[port], (dl_queue_elem_t *) &pool_device_client_id[port][i]);
      }

      /*Group Clients*/
      {
        pool_group_client_id[port][i].clientId =  i;
        pool_group_client_id[port][i].inUse     = L7_FALSE;
        dl_queue_add_tail(&queue_free_group_client_id[port], (dl_queue_elem_t *) &pool_group_client_id[port][i]);
      }
    }

    dl_queue_init(&igmpDeviceClients.queue_free_clientDevices[port]);

    for (i=0; i<PTIN_IGMP_CLIENTIDX_MAX; i++)
    {
      igmpDeviceClients.client_devices[port][i].client = L7_NULLPTR;   /* Pointer to client structure */
      dl_queue_add_tail(&igmpDeviceClients.queue_free_clientDevices[port], (dl_queue_elem_t *) &igmpDeviceClients.client_devices[port][i]);
    }
  }

  /* IGMP associaations */
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED

  memset(&channelDB, 0x00, sizeof(channelDB));

  channelDB.channelTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_IGMP_CHANNELS_MAX * sizeof(avlTreeTables_t)); 
  channelDB.channelDataHeap = (ptinIgmpChannelInfoData_t *)osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_IGMP_CHANNELS_MAX * sizeof(ptinIgmpChannelInfoData_t)); 

  if ((channelDB.channelTreeHeap == L7_NULLPTR) ||
      (channelDB.channelDataHeap == L7_NULLPTR))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error allocating data for IGMP Pairing AVL Trees\n");
    return L7_FAILURE;
  }

  /* Initialize the storage for all IGMP associations */
  memset (&channelDB.channelAvlTree , 0x00, sizeof(avlTree_t));
  memset ( channelDB.channelTreeHeap, 0x00, sizeof(avlTreeTables_t)*PTIN_IGMP_CHANNELS_MAX);
  memset ( channelDB.channelDataHeap, 0x00, sizeof(ptinIgmpChannelInfoData_t)*PTIN_IGMP_CHANNELS_MAX);

  // AVL Tree creations - snoopIpAvlTree
  avlCreateAvlTree(&(channelDB.channelAvlTree),
                   channelDB.channelTreeHeap,
                   channelDB.channelDataHeap,
                   PTIN_IGMP_CHANNELS_MAX, 
                   sizeof(ptinIgmpChannelInfoData_t),
                   0x10,
                   sizeof(ptinIgmpChannelDataKey_t));

  LOG_INFO(LOG_CTX_PTIN_IGMP,"Going to initialize Multicast Packages.");
  if (ptin_igmp_multicast_package_init() != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to Initialize Multicast Packages!");
    return L7_FAILURE;
  }
  LOG_INFO(LOG_CTX_PTIN_IGMP,"Initialized Multicast Packages.");
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

  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpInstances)             = %u", sizeof(igmpInstances));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpClientGroups)          = %u", sizeof(igmpGroupClients));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpClientGroups)*         = %u",
           sizeof(avlTree_t) + sizeof(avlTreeTables_t)*PTIN_SYSTEM_IGMP_MAXONUS + sizeof(ptinIgmpGroupClientInfoData_t)*PTIN_SYSTEM_IGMP_MAXONUS);
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpSnapshotClientGroups)  = %u", sizeof(igmpSnapshotClientGroups));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpSnapshotClientGroups)* = %u", 
           sizeof(avlTree_t) + sizeof(avlTreeTables_t)*PTIN_SYSTEM_IGMP_MAXONUS + sizeof(ptinIgmpClientGroupsSnapshotInfoData_t)*PTIN_SYSTEM_IGMP_MAXONUS);
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpClients_unified)       = %u", sizeof(igmpDeviceClients));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpClients_unified)*      = %u",
           sizeof(avlTree_t) + sizeof(avlTreeTables_t)*PTIN_SYSTEM_IGMP_MAXCLIENTS + sizeof(ptinIgmpClientInfoData_t)*PTIN_SYSTEM_IGMP_MAXCLIENTS);
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpPairDB)                = %u", sizeof(channelDB));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpPairDB)*               = %u",
           sizeof(avlTree_t) + sizeof(avlTreeTables_t)*PTIN_IGMP_CHANNELS_MAX + sizeof(ptinIgmpChannelInfoData_t)*PTIN_IGMP_CHANNELS_MAX);
#endif
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(global_stats_intf)         = %u", sizeof(global_stats_intf));

  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpProxyCfg   )         = %u", sizeof(igmpProxyCfg));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpClientsIntf)         = %u", sizeof(igmpClientsIntf));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpRoutersIntf)         = %u", sizeof(igmpRoutersIntf));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpInst_fromRouterVlan) = %u", sizeof(igmpInst_fromRouterVlan));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpInst_fromUCVlan)     = %u", sizeof(igmpInst_fromUCVlan));

#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(ptinIgmpChannelBandwidthCache) = %u", sizeof(ptinIgmpChannelBandwidthCache));
#if 0
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(clientGroupLookUpTable)        = %u", sizeof(clientGroupLookUpTable));
#endif
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpPortAdmissionControl)      = %u", sizeof(igmpPortAdmissionControl));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(igmpMulticastAdmissionControl) = %u", sizeof(igmpMulticastAdmissionControl));
#endif

  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(pool_device_client_id)      = %u", sizeof(pool_device_client_id));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(queue_free_device_clientIdx)= %u", sizeof(queue_free_device_client_id));

  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(pool_group_client_id)      = %u", sizeof(pool_group_client_id));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(queue_free_group_clientIdx)= %u", sizeof(queue_free_group_client_id));

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(channelIdPoolEntry)      = %u", sizeof(channelIdPoolEntry));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(channelPoolEntry)      = %u", sizeof(channelPoolEntry));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(packageIdPoolEntry)      = %u", sizeof(packageIdPoolEntry));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(packagePoolEntry)      = %u", sizeof(packagePoolEntry));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(multicastServices)      = %u", sizeof(multicastServices));
  LOG_INFO(LOG_CTX_PTIN_IGMP,"sizeof(multicastServiceId)      = %u", sizeof(multicastServiceId));
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
  igmpDeviceClients.number_of_clients = 0;
  memset(igmpDeviceClients.number_of_clients_per_intf,0x00,sizeof(igmpDeviceClients.number_of_clients_per_intf));
  memset(&igmpDeviceClients.client_devices, 0x00, sizeof(igmpDeviceClients.client_devices));

  /* Clean pools */
  memset(pool_device_client_id, 0x00, sizeof(pool_device_client_id));

  /* Clean pools */
  memset(pool_group_client_id, 0x00, sizeof(pool_group_client_id));

  // AVL Tree creations - snoopIpAvlTree
  avlPurgeAvlTree(&(igmpDeviceClients.avlTree.igmpClientsAvlTree), PTIN_SYSTEM_IGMP_MAXCLIENTS);

  osapiFree(L7_PTIN_COMPONENT_ID, igmpDeviceClients.avlTree.igmpClientsTreeHeap); 
  osapiFree(L7_PTIN_COMPONENT_ID, igmpDeviceClients.avlTree.igmpClientsDataHeap); 

  /* Reset structure of unified list of clients */
  memset(&igmpDeviceClients, 0x00, sizeof(igmpDeviceClients));

  /* CLIENT GROUPS CLEANUP */

  // AVL Tree creations - snoopIpAvlTree
  avlPurgeAvlTree(&(igmpGroupClients.avlTree.igmpClientsAvlTree),PTIN_SYSTEM_IGMP_MAXONUS);

  osapiFree(L7_PTIN_COMPONENT_ID, igmpGroupClients.avlTree.igmpClientsTreeHeap);
  osapiFree(L7_PTIN_COMPONENT_ID, igmpGroupClients.avlTree.igmpClientsDataHeap);

  /* Reset structure of unified list of clients */
  memset(&igmpGroupClients, 0x00, sizeof(igmpGroupClients));

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
 * Load IGMP proxy default configuraion parameters
 * 
 * @return RC_t SUCCESS/FAILURE
 */
L7_RC_t ptin_igmp_proxy_defaultcfg_load(void)
{
  ptin_IgmpProxyCfg_t igmpProxy;
  L7_RC_t             rc;

  igmpProxy.mask                                   = 0xFFFF;
  igmpProxy.admin                                  = 0;  
  igmpProxy.networkVersion                         = PTIN_IGMP_DEFAULT_VERSION;
  igmpProxy.clientVersion                          = PTIN_IGMP_DEFAULT_VERSION;
  igmpProxy.ipv4_addr.s_addr                       = PTIN_IGMP_DEFAULT_IPV4;
  igmpProxy.igmp_cos                               = PTIN_IGMP_DEFAULT_COS;
  igmpProxy.fast_leave                             = PTIN_IGMP_DEFAULT_FASTLEAVEMODE;

  igmpProxy.querier.mask                           = 0xFFFF;
  igmpProxy.querier.flags                          = 0;
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
  igmpProxy.host.robustness                        = PTIN_IGMP_DEFAULT_ROBUSTNESS;
  igmpProxy.host.unsolicited_report_interval       = PTIN_IGMP_DEFAULT_UNSOLICITEDREPORTINTERVAL;
  igmpProxy.host.older_querier_present_timeout     = PTIN_IGMP_DEFAULT_OLDERQUERIERPRESENTTIMEOUT;
  igmpProxy.host.max_records_per_report            = PTIN_IGMP_DEFAULT_MAX_RECORDS_PER_REPORT;

  igmpProxy.bandwidthControl                       = PTIN_IGMP_DEFAULT_BANDWIDTHCONTROL_MODE;
  igmpProxy.channelsControl                        = PTIN_IGMP_DEFAULT_CHANNELSCONTROL_MODE;
#if PTIN_BOARD_IS_MATRIX
  igmpProxy.whiteList                              = L7_DISABLE;
#else
  igmpProxy.whiteList                              = L7_ENABLE;
#endif

  /* Apply default config */
  rc = ptin_igmp_proxy_config_set__snooping_old(&igmpProxy);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "IGMP default config failed to be load");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

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

  /*Validate Mask & Flags*/
  {
    if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_RV && !(igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_RV))
    {
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "  Querier Robustness Mask Not Set: 0x%08X", igmpProxy->querier.mask);
      igmpProxy->querier.mask |=PTIN_IGMP_QUERIER_MASK_RV;
      igmpProxy->querier.robustness = igmpProxy->host.robustness;
    }

    if ( (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_QI ||
          igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_QRI ||
          igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_RV))
    {
      if (!(igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_GMI))
      {
        LOG_NOTICE(LOG_CTX_PTIN_IGMP, "  Querier Auto GMI Flag Not Set: 0x%08X", igmpProxy->querier.flags);
        igmpProxy->querier.flags |= PTIN_IGMP_QUERIER_MASK_AUTO_GMI;
      }

      if (!(igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_OQPI))
      {
        LOG_NOTICE(LOG_CTX_PTIN_IGMP, "  Querier Auto OQPI Flag Not Set: 0x%08X", igmpProxy->querier.flags);
        igmpProxy->querier.flags |= PTIN_IGMP_QUERIER_MASK_AUTO_OQPI;
      }

      if (!(igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_OHPT))
      {
        LOG_NOTICE(LOG_CTX_PTIN_IGMP, "  Querier Auto OHPT Flag Not Set: 0x%08X", igmpProxy->querier.flags);
        igmpProxy->querier.flags |= PTIN_IGMP_QUERIER_MASK_AUTO_OHPT;
      }

      if (!(igmpProxy->host.flags & PTIN_IGMP_HOST_MASK_OQPT))
      {
        LOG_NOTICE(LOG_CTX_PTIN_IGMP, "  Host Auto OQPT Flag Not Set: 0x%08X", igmpProxy->host.flags);
        igmpProxy->querier.flags |= PTIN_IGMP_HOST_MASK_OQPT;
      }
    }

    if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_SQI &&
        igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_SQI)
    {
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "  Querier Auto SQI Mask Set: 0x%08X", igmpProxy->querier.flags); 
      igmpProxy->querier.flags &= ~PTIN_IGMP_QUERIER_MASK_AUTO_SQI;      
    }

    if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_SQC &&
        igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_SQC)
    {
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "  Querier Auto SQC Mask Set: 0x%08X", igmpProxy->querier.flags);    
      igmpProxy->querier.flags &= ~PTIN_IGMP_QUERIER_MASK_AUTO_SQC;
    }

    if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_LMQC &&
        igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_LMQC)
    {
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "  Querier Auto LMQC Mask Set: 0x%08X", igmpProxy->querier.flags);    
      igmpProxy->querier.flags &= ~PTIN_IGMP_QUERIER_MASK_AUTO_LMQC;
    }

    if ( (igmpProxy->querier.mask || igmpProxy->querier.flags) && !(igmpProxy->mask & PTIN_IGMP_PROXY_MASK_QUERIER))
    {
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "  Querier Mask Not Set: 0x%08X", igmpProxy->mask);
      igmpProxy->mask |= PTIN_IGMP_PROXY_MASK_QUERIER;
    }

    if ( (igmpProxy->host.mask || igmpProxy->host.flags) && !(igmpProxy->mask & PTIN_IGMP_PROXY_MASK_HOST))
    {
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "  Host Mask Not Set: 0x%08X", igmpProxy->mask);
      igmpProxy->mask |= PTIN_IGMP_PROXY_MASK_HOST;
    }
  }
  /*End Mask & Flag Validation*/

  /* *******************
   * IGMP general config
   * *******************/
  /* Output admin state, but only apply changes in the end... */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_ADMIN
      && igmpProxyCfg.admin != igmpProxy->admin)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "  Admin:                                   %s", igmpProxy->admin == 0 ? "OFF":"ON");
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

  /* Bandwidth Control mode */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_BANDWIDTHCONTROL
      && igmpProxyCfg.bandwidthControl != igmpProxy->bandwidthControl)
  {
    igmpProxyCfg.bandwidthControl = igmpProxy->bandwidthControl;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "  Bandwidth Control mode:                         %s", igmpProxyCfg.bandwidthControl != 0 ? "ON":"OFF");
  }

  /* Channels Control mode */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_CHANNELSCONTROL
      && igmpProxyCfg.channelsControl != igmpProxy->channelsControl)
  {
    igmpProxyCfg.channelsControl = igmpProxy->channelsControl;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "  Channels Control mode:                         %s", igmpProxyCfg.channelsControl != 0 ? "ON":"OFF");
  }

  /* *******************
   * IGMP Querier config
   * *******************/
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "  Querier config:");
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_QUERIER)
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
    if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_OHPT)
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
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_HOST)
  {
    /* Host Robustness */
    if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_RV
        && igmpProxyCfg.host.robustness != igmpProxy->host.robustness)
    {
      igmpProxyCfg.host.robustness = igmpProxy->host.robustness;
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "    Host Robustness:                            %u", igmpProxyCfg.host.robustness);
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
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
    if (igmpProxy->admin == L7_DISABLE)
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Resetting Admission Control Allocation Parameters");
      //Reset Allocation Values of Admisssion Control Parameters
      ptin_igmp_admission_control_reset_allocation();    
    }
#endif

#if PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
    if ( igmpProxy->admin == L7_ENABLE )
    {
      /*Enable L3 IPMC Table*/
      if (dtlRouterMulticastForwardModeSet(L7_ENABLE, L7_AF_INET)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to enable L3 IPMC Table!");
        return L7_FAILURE;
      }
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Enabled L3 IPMC Table");
    }
    else
    {
      /*Disable L3 IPMC Table*/
      if (dtlRouterMulticastForwardModeSet(L7_DISABLE, L7_AF_INET)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to disable L3 IPMC Table!");
        return L7_FAILURE;
      }
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Disabled L3 IPMC Table");
    }
#endif

    igmpProxyCfg.admin = igmpProxy->admin;
  }

  osapiSemaGive(igmp_sem);

  return L7_SUCCESS;
}

void ptin_igmp_proxy_config_dump__snooping_old(void)
{
  ptin_IgmpProxyCfg_t igmpProxy;

  ptin_igmp_proxy_config_get__snooping_old(&igmpProxy);

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "IGMP Proxy (mask=0x%08X)", igmpProxy.mask);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, " Admin #                          = %u",          igmpProxy.admin);  
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, " Network Version                  = %u",          igmpProxy.networkVersion);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, " Client Version                   = %u",          igmpProxy.clientVersion);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, " IP Addr                          = %u.%u.%u.%u", (igmpProxy.ipv4_addr.s_addr>>24)&0xFF, (igmpProxy.ipv4_addr.s_addr>>16)&0xFF, (igmpProxy.ipv4_addr.s_addr>>8)&0xFF, igmpProxy.ipv4_addr.s_addr&0xFF);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, " COS                              = %u",          igmpProxy.igmp_cos);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, " FastLeave                        = %s",          igmpProxy.fast_leave ? "ON" : "OFF");
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, " Querier (mask=0x%08X)", igmpProxy.querier.mask);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Flags                          = 0x%04X",      igmpProxy.querier.flags);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Robustness                     = %u",          igmpProxy.querier.robustness);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Query Interval                 = %u (s)",          igmpProxy.querier.query_interval);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Query Response Interval        = %u (ds)",          igmpProxy.querier.query_response_interval);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Group Membership Interval      = %u (s)",          igmpProxy.querier.group_membership_interval);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Other Querier Present Interval = %u (s)",          igmpProxy.querier.other_querier_present_interval);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Startup Query Interval         = %u (s)",          igmpProxy.querier.startup_query_interval);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Startup Query Count            = %u",          igmpProxy.querier.startup_query_count);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Last Member Query Interval     = %u (ds)",          igmpProxy.querier.last_member_query_interval);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Last Member Query Count        = %u",          igmpProxy.querier.last_member_query_count);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Older Host Present Timeout     = %u (s)",          igmpProxy.querier.older_host_present_timeout);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, " Host (mask=0x%08X)", igmpProxy.host.mask);         
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Flags                          = 0x%02X",      igmpProxy.host.flags);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Robustness                     = %u",          igmpProxy.host.robustness);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Unsolicited Report Interval    = %u (s)",          igmpProxy.host.unsolicited_report_interval);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Older Querier Present Timeout  = %u (s)",          igmpProxy.host.older_querier_present_timeout);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "   Max Group Records per Packet   = %u",          igmpProxy.host.max_records_per_report);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, " Bandwidth Control                = %s",          igmpProxy.bandwidthControl ? "ON" : "OFF");
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, " Channels Control                 = %s",          igmpProxy.channelsControl ? "ON" : "OFF");
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, " White-List                       = %s",          igmpProxy.whiteList  ? "ON" : "OFF");
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
  if (0 == ctrlResMsg.res)
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

    oldIgmpConfig.bandwidthControl                       = igmpProxy->bandwidthControl;
    oldIgmpConfig.channelsControl                        = igmpProxy->channelsControl;

    ptin_igmp_proxy_config_set__snooping_old(&oldIgmpConfig);
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error configuring Igmp Proxy rc:%u", ctrlResMsg.res);
  }

  /* Configure global trapping */
  if (ptin_igmp_enable(igmpProxy->admin) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error configuring VLANs trapping (enable=%u)", igmpProxy->admin);
    return L7_FAILURE;
  }

  return ctrlResMsg.res;
}


void ptin_igmp_proxy_admission_control_set(L7_uint8 admissionControl)
{
  igmpProxyCfg.bandwidthControl = igmpProxyCfg.channelsControl = (admissionControl & L7_TRUE);
};  

void ptin_igmp_proxy_bandwidth_control_set(L7_uint8 bandwidthControl)
{
  igmpProxyCfg.bandwidthControl = (bandwidthControl & L7_TRUE);
};

void ptin_igmp_proxy_channels_control_set(L7_uint8 channelsControl)
{
  igmpProxyCfg.channelsControl = (channelsControl & L7_TRUE);
};

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
  if (sizeof(PTIN_MGMD_CTRL_MGMD_CONFIG_t) != ctrlResMsg.dataLength)
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
  LOG_INFO(LOG_CTX_PTIN_IGMP,"Multicast queriers reset:");

#if PTIN_SNOOP_USE_MGMD
  if (ptin_igmp_generalquerier_reset() != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to reset MGMD General Queriers");
    return L7_FAILURE;
  }
#else
  L7_uint32 admin;

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
#endif

  LOG_INFO(LOG_CTX_PTIN_IGMP,"Multicast queriers reenabled!");

  return L7_SUCCESS;
}

/**
 * Set Global enable for IGMP packet trapping
 * 
 * @param enable : L7_ENABLE/L7_DISABLE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_enable(L7_BOOL enable)
{
#if 0
  /* Global trap enable */
  if (usmDbSnoopAdminModeSet(1, enable, L7_AF_INET) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP,"Error setting IGMP global enable to %u", enable);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_DHCP,"Success setting IGMP global enable to %u", enable);
#endif

#if (PTIN_QUATTRO_FLOWS_FEATURE_ENABLED && QUATTRO_IGMP_TRAP_PREACTIVE)
  /* Configure packet trapping for this VLAN  */
  if (usmDbSnoopVlanModeSet(1, PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN, enable, L7_AF_INET) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring packet trapping for QUATTRO VLANs (enable=%u)", enable);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Packet trapping for QUATTRO VLANs configured (enable=%u)", enable);
#endif

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
    if (ptin_debug_igmp_snooping)
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
    return L7_TABLE_IS_FULL;
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
  if ((rc=ptin_igmp_assoc_clean_all())!=L7_SUCCESS)
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
 * Reset IGMP Module to Default
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_default_reset(void)
{
  L7_RC_t rc, rc_global = L7_SUCCESS;

  //Disable Snoop Module
  {
    L7_uint32 adminMode = L7_DISABLE;
    rc = usmDbSnoopAdminModeGet(0, &adminMode, L7_AF_INET);
    if ( rc != L7_SUCCESS )
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to obtain Snoop AdminMode (rc:%u)", rc);
      rc_global = rc;
    }

    if (adminMode == L7_ENABLE)
    {
      rc = usmDbSnoopAdminModeSet(0, L7_DISABLE, L7_AF_INET);
      if ( rc != L7_SUCCESS )
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to disable Snoop Module (rc:%u)", rc);
        rc_global = rc;
      }
    }
  }
  
  //Proxy Defaults
  {
    rc = ptin_igmp_proxy_defaultcfg_load();  
    if ( rc != L7_SUCCESS )
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error load proxy default (rc:%u)", rc);
      rc_global = rc;
    }
  }

  //Reset MGMD Lib
  {
    ptin_igmp_mgmd_resetdefaults();
    if ( rc != L7_SUCCESS )
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error on performing MGMD Lib reset (rc:%u)", rc);
      rc_global = rc;
    }
  }

  /* Remove all instances */
  {
    L7_uint igmp_idx;
    for (igmp_idx=0; igmp_idx<PTIN_SYSTEM_N_IGMP_INSTANCES; igmp_idx++)
    {
      /*Move to the Next Instance*/   
      if (!igmpInstances[igmp_idx].inUse)
        continue;

      rc = ptin_igmp_instance_delete(igmp_idx);
      if ( rc !=L7_SUCCESS )
      {
        if (rc_global == L7_SUCCESS)
          rc_global = rc;
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error clearing igmp_idx=%u (rc:%u)",igmp_idx, rc);
      }
    }
  }

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  /* Remove Multicast associations */
  {
    rc = ptin_igmp_assoc_clean_all();
    if ( rc != L7_SUCCESS )
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error clearing igmp associations (rc:%u)", rc);
      rc_global = rc;
    }
  }

#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
  //Reset Admission Control Feature  
  {
    ptin_igmp_admission_control_reset_all();    
  }  
#endif

   //Reset Package Feature
  {
    rc = ptin_igmp_multicast_package_init();
    if ( rc != L7_SUCCESS )
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error clearing multicast package (rc:%u)", rc);
      rc_global = rc;
    }
  }

  //Reset Multicast Service
  {
    ptin_igmp_multicast_service_reset();
  }
#endif

  //Remove All Group Clients 
  //Remove Device Clients Attached to Each Group Client
  //Remove Timer Attached to Device Client
  {
    rc=ptin_igmp_all_clients_flush();
    if ( rc != L7_SUCCESS )
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error clearing igmp clients (rc:%u)", rc);
      rc_global = rc;
    }
  }

  return rc_global;
}

/**
 * Reset MGMD
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_mgmd_resetdefaults(void)
{
  PTIN_MGMD_EVENT_t               reqMsg        = {0};
  PTIN_MGMD_EVENT_t               resMsg        = {0};
  PTIN_MGMD_CTRL_RESET_DEFAULTS_t mgmdConfigMsg = {0}; 

  mgmdConfigMsg.family = 0; // Reset both IGMP and MLD
  ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_RESET_DEFAULTS, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdConfigMsg, sizeof(PTIN_MGMD_CTRL_RESET_DEFAULTS_t));
  ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);

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
  L7_uint  igmp_idx;
  L7_uint8 evc_type;
  L7_BOOL  new_instance = L7_FALSE;

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
  if (evc_type != PTIN_EVC_TYPE_QUATTRO_STACKED)
  {
    nni_ovlan = 0;
  }

  /* Is EVC associated to an IGMP instance? */
  if ( ptin_igmp_instance_find(evc_idx, 0 /*Not used*/, L7_NULLPTR)==L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"There is already an instance with mcEvcId=%u",evc_idx);
    return L7_SUCCESS;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to add evc %u",evc_idx);

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
  if (evc_type != PTIN_EVC_TYPE_QUATTRO_STACKED || igmp_quattro_stacked_evcs == 0)
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
  if (evc_type == PTIN_EVC_TYPE_QUATTRO_STACKED)
  {
    igmp_quattro_stacked_evcs++;
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
  L7_uint   igmp_idx;
  L7_uint8  evc_type;
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
  if (evc_type != PTIN_EVC_TYPE_QUATTRO_STACKED || igmp_quattro_stacked_evcs <= 1)
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
  if (evc_type == PTIN_EVC_TYPE_QUATTRO_STACKED)
  {
    if (igmp_quattro_stacked_evcs>0)  igmp_quattro_stacked_evcs--;
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
 * @param evc_idx      : evc id
 * @param client       : client identification parameters 
 * @param uni_ovid     : External Outer vlan 
 * @param uni_ivid     : External Inner vlan 
 * @param OnuId        : ONU Identifier 
 * @param mask         : To set the admission control parameters
 * @param maxBandwidth : [mask 0x01] Maximum allowed bandwidth 
 *                     for this client. Use (L7_uint64)-1 to
 *                     disable.
 * @param maxChannels  : [mask 0x02] Maximum number of channels 
 *                     for this client. Use (L7_uint64)-1 to
 *                     disable.
 * @param addOrRemove  : Add/Remove Packages
 * @param packagePtr   : Package Bitmap Pointer 
 * @param noOfPackages : Number of Packages
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_client_add(L7_uint32 evc_idx, const ptin_client_id_t *client_id, L7_uint16 uni_ovid, L7_uint16 uni_ivid, L7_uint8 onuId, L7_uint8 mask, L7_uint64 maxBandwidth, L7_uint16 maxChannels, L7_BOOL addOrRemove, L7_uint32 *packagePtr, L7_uint32 noOfPackages)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;
  ptin_client_id_t client;

  memcpy(&client, client_id, sizeof(ptin_client_id_t));

  /* Validate, and rearrange, client info */
  if (ptin_igmp_clientId_convert(evc_idx, &client)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client id");
    return L7_FAILURE;
  }

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
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Ext vlans for ptin_intf %u/%u, cvlan %u: uni_ovid=%u, uni_ivid=%u",
                  client.ptin_intf.intf_type,client.ptin_intf.intf_id, client.innerVlan, uni_ovid, uni_ivid);
      }
      else
      {
        uni_ovid = uni_ivid = 0;
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot get ext vlans for ptin_intf %u/%u, cvlan %u",
                client.ptin_intf.intf_type,client.ptin_intf.intf_id, client.innerVlan);
      }
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid ptin_intf %u/%u", client.ptin_intf.intf_type, client.ptin_intf.intf_id);
    }
  }

  /* Create new static client */
  rc = ptin_igmp_group_client_add(&client, uni_ovid, uni_ivid, onuId, mask, maxBandwidth, maxChannels, addOrRemove, packagePtr, noOfPackages);

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
L7_RC_t ptin_igmp_client_delete(L7_uint32 evc_idx, const ptin_client_id_t *client_id)
{
  L7_RC_t rc;
  ptin_client_id_t client;

  memcpy(&client, client_id, sizeof(ptin_client_id_t));

  /* Validate, and rearrange, client info */
  /* If error, client does not exist: return success */
  if (ptin_igmp_clientId_convert(evc_idx, &client)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client id");
    return L7_NOT_EXIST;
  }

  /* Remove client */
  rc = ptin_igmp_group_client_remove(&client);

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
  if ( ptin_igmp_group_client_clean()!=L7_SUCCESS)
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

  return(L7_RC_t)ctrlResMsg.res;
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

L7_RC_t ptin_igmp_channelList_get(L7_uint32 McastEvcId, const ptin_client_id_t *client_id,
                                  L7_uint16 channel_index, L7_uint16 *max_number_of_channels, ptin_igmpChannelInfo_t *channel_list,
                                  L7_uint16 *total_channels)
{
  L7_uint32         entryId = 0;
  L7_uint32         numberOfChannels = 0;   
  ptin_client_id_t  client;

  /* Validate arguments */
  if (channel_list == L7_NULLPTR || max_number_of_channels == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Null parameters");
    return L7_FAILURE;
  }

  /* Validate client */
  if (client_id == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client pointer");
    return L7_FAILURE;
  }

  memcpy(&client, client_id, sizeof(ptin_client_id_t));

  /* Validate and rearrange clientId info */
  if (client.mask != 0)
  {
    if (ptin_igmp_clientId_convert(McastEvcId, &client)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client id");
      return L7_FAILURE;
    }
  }

  /*MGMD Lib is expecting the channel_index to be equal to the last index sent. 
  However, Fastpath is expecting the index to be the first channel to be sent*/
  if (channel_index != 0)
  {
    --channel_index;
  }

  if (client.mask == 0)
  {
    uint32 channelCopied;    

    do
    {
      PTIN_MGMD_EVENT_t                      reqMsg        = {0};
      PTIN_MGMD_EVENT_t                      resMsg        = {0};
      PTIN_MGMD_EVENT_CTRL_t                 ctrlResMsg    = {0};
      PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t  mgmdGroupsMsg = {0}; 
      PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t mgmdGroupsRes = {0};

      channelCopied = 0;  

      mgmdGroupsMsg.serviceId = McastEvcId;   
      mgmdGroupsMsg.portId = PTIN_MGMD_ROOT_PORT;
      mgmdGroupsMsg.clientId = PTIN_MGMD_MANAGEMENT_CLIENT_ID;

      if (numberOfChannels==0)
      {
        mgmdGroupsMsg.entryId   = (channel_index==0)?(PTIN_MGMD_CTRL_ACTIVEGROUPS_FIRST_ENTRY):(channel_index);
      }
      else
      {
        mgmdGroupsMsg.entryId   = entryId;
      }

      ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_GROUPS_GET, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdGroupsMsg, sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t));
      ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
      ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);

      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code: %08X",      ctrlResMsg.msgCode);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id  : %08X",      ctrlResMsg.msgId);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Res     : %u",        ctrlResMsg.res);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Length  : %u (%.1f)", ctrlResMsg.dataLength, ((double)ctrlResMsg.dataLength)/sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));


      if (0 == ctrlResMsg.dataLength%sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t))
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Active groups (Service:%u)", McastEvcId);
        while ((ctrlResMsg.dataLength > 0) && (numberOfChannels < *max_number_of_channels))
        {
          memcpy(&mgmdGroupsRes, ctrlResMsg.data + channelCopied*sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t), sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));

          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  Entry [%u]", mgmdGroupsRes.entryId);
          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Type:           %s",   mgmdGroupsRes.groupType==PTIN_MGMD_CTRL_GROUPTYPE_DYNAMIC? ("Dynamic"):("Static"));
          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Filter-Mode:    %s",   mgmdGroupsRes.filterMode==PTIN_MGMD_CTRL_FILTERMODE_INCLUDE? ("Include"):("Exclude"));
          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Group Timer:    %u",   mgmdGroupsRes.groupTimer);
          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Groups Address: %08X", mgmdGroupsRes.groupIP);
          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Source Timer:   %u",   mgmdGroupsRes.sourceTimer);
          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Source Address: %08X", mgmdGroupsRes.sourceIP);

          inetAddressSet(L7_AF_INET, &mgmdGroupsRes.groupIP, &channel_list[numberOfChannels].groupAddr);
          inetAddressSet(L7_AF_INET, &mgmdGroupsRes.sourceIP, &channel_list[numberOfChannels].sourceAddr);
          channel_list[numberOfChannels].static_type = mgmdGroupsRes.groupType;

          ctrlResMsg.dataLength -= sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t);
          ++channelCopied;
          ++numberOfChannels;
        }
      }
      else
      {
        if (ctrlResMsg.res==NOT_EXIST)
          break;
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid response size from MGMD [size:%u]. Expecting Multiple of :%u", ctrlResMsg.dataLength, sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));
        return L7_FAILURE;
      }      
      entryId = mgmdGroupsRes.entryId;

      LOG_TRACE(LOG_CTX_PTIN_IGMP, "channelCopied=%u channelCopied=%u sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t)=%u", channelCopied, numberOfChannels,sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));

    } while ( numberOfChannels<*max_number_of_channels && channelCopied == PTIN_MGMD_EVENT_CTRL_DATA_SIZE_MAX / sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));
    *max_number_of_channels = numberOfChannels;
  }
  else
  {
    ptinIgmpGroupClientInfoData_t *clientGroup;   
    L7_uint32                      intIfNum;
    L7_uint32                      globalGroupCountperMsg = 0;
    L7_BOOL                        isFirstDevice=L7_TRUE;

    /* Find client */
    if (ptin_igmp_group_client_find(&client, &clientGroup)!=L7_SUCCESS)
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
      return L7_FAILURE;
    }


    /*Take Semaphore*/
    osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

    /* Validate interface */
    if (ptin_intf_ptintf2intIfNum(&client.ptin_intf, &intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting port %u/%u to intIfNum",client.ptin_intf.intf_type,client.ptin_intf.intf_id);

      /*Give Semaphore*/
      osapiSemaGive(ptin_igmp_clients_sem);
      return L7_FAILURE;
    }

    L7_uint16 noOfClients = igmp_clientDevice_get_devices_number(clientGroup);
    L7_uint16 clientId;

    if (noOfClients>0)
    {
      L7_uint16 noOfClientsFound = 0;
      L7_uint16 clientIdAux = 0;

      L7_uint32 channelCopied;
      L7_uint32 groupCountperMsg = 0;

      for (clientId = 0; clientId<PTIN_IGMP_CLIENTIDX_MAX; ++clientId)
      {

        if (isFirstDevice == L7_FALSE && numberOfChannels >= PTIN_MGMD_EVENT_CTRL_DATA_SIZE_MAX / sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t))
        {
          break;
        }

        if (IS_BITMAP_WORD_SET(clientGroup->client_bmp_list, clientId, UINT32_BITSIZE) == L7_FALSE)
        {
          //Next Position on the Array of Clients. -1 since the for adds 1 unit.
          clientId += UINT32_BITSIZE - 1;
          continue;
        }

        if (IS_BITMAP_BIT_SET(clientGroup->client_bmp_list, clientId, UINT32_BITSIZE))
        {
          do
          {
            PTIN_MGMD_EVENT_t                      reqMsg        = {0};
            PTIN_MGMD_EVENT_t                      resMsg        = {0};
            PTIN_MGMD_EVENT_CTRL_t                 ctrlResMsg    = {0};
            PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t  mgmdGroupsMsg = {0}; 
            PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t mgmdGroupsRes = {0};

            mgmdGroupsMsg.serviceId = McastEvcId;
            mgmdGroupsMsg.portId    = intIfNum;
            mgmdGroupsMsg.clientId  = clientId;

            if (globalGroupCountperMsg == 0 || clientIdAux != clientId)
            {
              mgmdGroupsMsg.entryId   = (channel_index==0)?(PTIN_MGMD_CTRL_ACTIVEGROUPS_FIRST_ENTRY):(channel_index);
            }
            else
            {
              mgmdGroupsMsg.entryId   = entryId;
            }

            channelCopied = 0;
            groupCountperMsg = 0;

            ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_CLIENT_GROUPS_GET, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdGroupsMsg, sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t));
            ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
            ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);

            LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
            LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code: %08X",      ctrlResMsg.msgCode);
            LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id  : %08X",      ctrlResMsg.msgId);
            LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Res     : %u",        ctrlResMsg.res);
            LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Length  : %u (%u)", ctrlResMsg.dataLength, (ctrlResMsg.dataLength)/sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));    

            if (0 != ctrlResMsg.dataLength%sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t))
            {
              if (ctrlResMsg.res==NOT_EXIST)
                continue;

              LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid response size from MGMD [size:%u]. Expecting multiple of %u", ctrlResMsg.dataLength, sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));
              /*Give Semaphore*/
              osapiSemaGive(ptin_igmp_clients_sem);
              return L7_FAILURE;
            }

            L7_uint16   iterator;
            L7_uint16   existingEntry=L7_FALSE;

            LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Active groups (Service:%u)", McastEvcId);
            while ((ctrlResMsg.dataLength > 0) && (numberOfChannels < *max_number_of_channels))
            {
              memcpy(&mgmdGroupsRes, ctrlResMsg.data + groupCountperMsg*sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t), sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));         

              LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  Entry [%u]", mgmdGroupsRes.entryId);
              LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Type:           %s",   mgmdGroupsRes.groupType==PTIN_MGMD_CTRL_GROUPTYPE_DYNAMIC? ("Dynamic"):("Static"));
              LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Filter-Mode:    %s",   mgmdGroupsRes.filterMode==PTIN_MGMD_CTRL_FILTERMODE_INCLUDE? ("Include"):("Exclude"));
              LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Group Timer:    %u",   mgmdGroupsRes.groupTimer);
              LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Groups Address: %08X", mgmdGroupsRes.groupIP);
              LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Source Timer:   %u",   mgmdGroupsRes.sourceTimer);
              LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    Source Address: %08X", mgmdGroupsRes.sourceIP);

              //This procedure is required, since the client id on mgmd identifies devices (e.g. set-top boxes), while on the manager identifies ONUs    
              //Import Note: The number of Groups of all devices should be bellow the size of buffer (i.e. 292 Groups) with the Manager and the Upper Layers (WebTi, CLI, Agora-NG). If this is not the case we should modify this function to support a bitmap of clients!
              if (isFirstDevice==L7_FALSE)
              {
                for (iterator=0;iterator<numberOfChannels;iterator++)
                {
                  //Verify if we have already read this entry before
                  if (channel_list[iterator].groupAddr.addr.ipv4.s_addr == mgmdGroupsRes.groupIP && 
                      channel_list[iterator].sourceAddr.addr.ipv4.s_addr == mgmdGroupsRes.sourceIP)
                  {
                    existingEntry=L7_TRUE;
                    break;
                  }
                }
              }

              if (existingEntry==L7_FALSE)
              {//Add entry
                inetAddressSet(L7_AF_INET, &mgmdGroupsRes.groupIP, &channel_list[numberOfChannels].groupAddr);
                inetAddressSet(L7_AF_INET, &mgmdGroupsRes.sourceIP, &channel_list[numberOfChannels].sourceAddr);
                channel_list[numberOfChannels].static_type = mgmdGroupsRes.groupType;

                ++channelCopied;
                ++numberOfChannels;
              }
              else
              {//Restore value            
                existingEntry=FALSE;                                 
              }

              ++groupCountperMsg;         
              ctrlResMsg.dataLength -= sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t);
            }        

            globalGroupCountperMsg+=groupCountperMsg;
            entryId = mgmdGroupsRes.entryId;

            LOG_TRACE(LOG_CTX_PTIN_IGMP, "channelCopied=%u numberOfChannels=%u sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t)=%u", channelCopied, numberOfChannels, sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));

          }while (isFirstDevice == TRUE && numberOfChannels<*max_number_of_channels  &&   groupCountperMsg == PTIN_MGMD_EVENT_CTRL_DATA_SIZE_MAX / sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));    

          if (isFirstDevice == TRUE)
          {
            isFirstDevice = L7_FALSE;
          }

          if ( ++noOfClientsFound >= noOfClients)
          {
            LOG_TRACE(LOG_CTX_PTIN_IGMP, "noOfClientsFound:%u >= noOfClients:%u", noOfClientsFound, noOfClients);
            break;
          }

          clientIdAux = clientId;                    
        }

        if ( numberOfChannels >= *max_number_of_channels )
        {
          break;
        }
      }
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "This groupClient has zero devices currently attached!");
    }

    *max_number_of_channels = numberOfChannels;

    /*Give Semaphore*/
    osapiSemaGive(ptin_igmp_clients_sem);
  }

  return L7_SUCCESS;
}

/**
 * Get list of clients, watching a specific channel 
 * 
 * @param McastEvcId         : (in) Multicast EVC id
 * @param groupAddr          : (in) Group Address
 * @param sourceAddr         : (in) Source Address
 * @param client_index       : (in) First client index
 * @param number_of_clients  : (in) Maximum number of clients 
 *                             (out) Number of clients 
 * @param client_list        : (out) Clients array 
 * @param evc_id             : (out) Extended EVC Id 
 * @param total_clients      : (out) Total number of clients 
 *  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
//static L7_uint16 clientList_size=0;
//static ptin_client_id_t clientList[PTIN_SYSTEM_IGMP_MAXONUS];

L7_RC_t ptin_igmp_clientList_get(L7_uint32 McastEvcId, L7_in_addr_t *groupAddr, L7_in_addr_t *sourceAddr,
                                 L7_uint16 client_index, L7_uint16 *number_of_clients, ptin_client_id_t *client_list,L7_uint32 *extendedEvcId, 
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
  L7_uint32                              totalInvalidClients = 0; 

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
  if (L7_SUCCESS != ptin_igmp_clientGroupSnapshot_clean())
  {
    *number_of_clients=0;
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to clean the current clientGroup snapshot");
    return L7_FAILURE;
  }

  /* If the entry index is 0, request the client list from MGMD. Otherwise, read from our local snapshot */
  if (0 == client_index)
  {
    L7_uint32 maxResponseEntries = PTIN_MGMD_EVENT_CTRL_DATA_SIZE_MAX/sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t);
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Max Entries:%u", maxResponseEntries);
    L7_uint32 pageClientCount = 0; 

    /*Take Semaphore*/
    osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

    do
    {
      //Save current page and total client context
      totalClientCount += pageClientCount;
      pageClientCount   = 0; 

      mgmdGroupsMsg.serviceId = McastEvcId;
      mgmdGroupsMsg.groupIP   = groupAddr->s_addr;
      mgmdGroupsMsg.sourceIP  = sourceAddr->s_addr;
      mgmdGroupsMsg.entryId   = (totalClientCount==0)?(PTIN_MGMD_CTRL_GROUPCLIENTS_FIRST_ENTRY):(totalClientCount);
      ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_GROUP_CLIENTS_GET, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdGroupsMsg, sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_REQUEST_t));
      ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
      ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code            : %08X",      ctrlResMsg.msgCode);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id              : %08X",      ctrlResMsg.msgId);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Res                 : %u",        ctrlResMsg.res);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Length              : %u", ctrlResMsg.dataLength);
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  Entry Length             : %u", sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t), ctrlResMsg.dataLength/sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t));
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  Number Of Entries Length : %u", ctrlResMsg.dataLength/sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t));

      if (0 != ctrlResMsg.dataLength%sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t))
      {
        if (ctrlResMsg.res==NOT_EXIST)
          break;
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid dataLength:%u received. Expecting multiple of :%u", ctrlResMsg.dataLength, sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t));
        /*Give Semaphore*/
        osapiSemaGive(ptin_igmp_clients_sem);

        return L7_FAILURE;
      }

      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Active groups (Service:%u GroupAddr:%08X)", McastEvcId, groupAddr->s_addr);

      PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t *mgmdGroupsRes = (PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t*) ctrlResMsg.data;      
      while (ctrlResMsg.dataLength > 0 && mgmdGroupsRes != L7_NULLPTR)
      {
        ptin_client_id_t               newClientEntry;    
        ptinIgmpClientInfoData_t      *client;
        ptinIgmpGroupClientInfoData_t *clientGroup;
        L7_uint32                      ptinPort;

        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  EntryId [%u]",   mgmdGroupsRes->entryId);
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    PortId:   %u", mgmdGroupsRes->portId);
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "    ClientId: %u", mgmdGroupsRes->clientId);

        if (ptin_intf_intIfNum2port(mgmdGroupsRes->portId,&ptinPort) != L7_SUCCESS)
        {
          *number_of_clients=0;
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to convert intIfNum [%u] to ptinPort",mgmdGroupsRes->portId);
          /*Give Semaphore*/
          osapiSemaGive(ptin_igmp_clients_sem);

          return L7_FAILURE;
        }
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Converted   intIfNum [%u] to ptinPort [%u]", mgmdGroupsRes->portId,ptinPort);

        /* Save entry in the clientGroup snapshot avlTree */
        if (L7_NULLPTR == (client = igmpDeviceClients.client_devices[ptinPort][mgmdGroupsRes->clientId].client))
        {
          /*Decrement Data Length*/
          ctrlResMsg.dataLength -= sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t);
          /*Increment the Number of Consumed Pages*/
          ++totalClientCount;
          /*Increment the Total Number of Clients Found*/
          ++pageClientCount;
          /*Increment the Total Invalid Clients Found*/
          ++totalInvalidClients;
          /*Move the Pointer to the Next Client*/
          LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid client returned from MGMD clientId:%u", mgmdGroupsRes->clientId);
          mgmdGroupsRes++;                  
          continue;
        }
        if (L7_NULLPTR == (clientGroup = client->pClientGroup))
        {
          *number_of_clients=0;
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client returned from MGMD clientId:%u", mgmdGroupsRes->clientId);

          /*Give Semaphore*/
          osapiSemaGive(ptin_igmp_clients_sem);

          return L7_FAILURE;
        }
#if (MC_CLIENT_INTERF_SUPPORTED)
        if (ptin_intf_port2ptintf(clientGroup->ptin_port, &newClientEntry.ptin_intf)!=L7_SUCCESS)
        {
          *number_of_clients=0;
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to Convert intfNum[%u]", clientGroup->ptin_port);

          /*Give Semaphore*/
          osapiSemaGive(ptin_igmp_clients_sem);

          return L7_FAILURE;
        }
        newClientEntry.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Port: %u", clientGroup->ptin_port);
#endif
#if (MC_CLIENT_OUTERVLAN_SUPPORTED)
        newClientEntry.outerVlan = clientGroup->igmpClientDataKey.outerVlan;
        newClientEntry.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "oVlan: %u", newClientEntry.outerVlan);
#endif
#if (MC_CLIENT_INNERVLAN_SUPPORTED)
        newClientEntry.innerVlan = clientGroup->igmpClientDataKey.innerVlan;
        newClientEntry.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "iVlan: %u", newClientEntry.innerVlan);
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
        if (L7_SUCCESS != ptin_igmp_clientGroupSnapshot_add(&newClientEntry))
        {
          *number_of_clients=0;
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to add this clientIdx[%u] port portId[%u] to the clientGroupSnapshot avlTree", mgmdGroupsRes->clientId, mgmdGroupsRes->portId);

          /*Give Semaphore*/
          osapiSemaGive(ptin_igmp_clients_sem);

          return L7_FAILURE;
        }

        /*Decrement Data Length*/
        ctrlResMsg.dataLength -= sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t);
        /*Increment the Number of Consumed Pages*/
        ++totalClientCount;
        /*Increment the Total Number of Clients Found*/
        ++pageClientCount;
        /*Move the Pointer to the Next Client*/
        mgmdGroupsRes++;        
      }
    } while (pageClientCount == maxResponseEntries); //While the number of clients returned equals the max number of clients per page

    /*Give Semaphore*/
    osapiSemaGive(ptin_igmp_clients_sem);
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
    if (avl_infoData->in_use != L7_TRUE)
    {
      continue;
    }

    /* If we still haven't reached the desired clientID, continue */
    if (currentClientId < client_index)
    {
      ++currentClientId;
      continue;
    }

    /*MAC Bridge Services Support*/
    if (L7_SUCCESS != ptin_evc_get_evcIdfromIntVlan(tempKey.outerVlan,&extendedEvcId[clientBufferIdx]))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to get external EVC Id :%u",tempKey.outerVlan);
      extendedEvcId[clientBufferIdx]=(L7_uint32)-1;
    }
    /*End MAC Bridge Services Support*/

    /* Copy client contents */
    if (L7_SUCCESS != ptin_igmp_clientId_restore(&tempKey))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to convert client[mask:%02X intf:%u/%u oVlan:%u iVlan:%u]", avl_infoData->key.mask, avl_infoData->key.ptin_intf.intf_type, avl_infoData->key.ptin_intf.intf_id,
              avl_infoData->key.outerVlan, avl_infoData->key.innerVlan);
      return L7_FAILURE;
    }
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "      Idx:   %u",      clientBufferIdx);
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "        Mask:  0x%02X",  tempKey.mask);
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "        Intf:  %u/%u", tempKey.ptin_intf.intf_type, tempKey.ptin_intf.intf_id);
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "        oVlan: %u",    tempKey.outerVlan);
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "        iVlan: %u",    tempKey.innerVlan);
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "        eEVCId:%u",    extendedEvcId[clientBufferIdx]);
    memcpy(&client_list[clientBufferIdx], &tempKey, sizeof(tempKey));

    /* Increase the ID of the read clientGroup */
    ++clientBufferIdx;
    ++currentClientId;
  }

  *total_clients     = totalClientCount-totalInvalidClients;
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
L7_RC_t ptin_igmp_static_channel_remove(PTIN_MGMD_CTRL_STATICGROUP_t* channel)
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
    clientInfo = igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;

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
  if ( ivid != 0 && ptin_evc_flags_get_fromIntVlan(intOVlan, &flags, L7_NULLPTR) == L7_SUCCESS &&
       !(flags & PTIN_EVC_MASK_QUATTRO) &&
       !(flags & PTIN_EVC_MASK_STACKED))
  {
    ivid = 0;
  }

  /* For packets sent to root ports, belonging to unstacked EVCs, remove inner vlan */
  if ( ivid != 0 && ptin_evc_flags_get_fromIntVlan(intOVlan, &flags, L7_NULLPTR) == L7_SUCCESS)
  {
    if ( ( (flags & PTIN_EVC_MASK_QUATTRO) && !(flags & PTIN_EVC_MASK_STACKED) && (ptin_igmp_rootIntfVlan_validate(intIfNum, intOVlan)==L7_SUCCESS) ) ||
         (!(flags & PTIN_EVC_MASK_QUATTRO) && !(flags & PTIN_EVC_MASK_STACKED)) )
    {
      ivid = 0;
    }
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
  L7_uint  next;
  L7_uint8 evc_type;
  L7_RC_t  rc;

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
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,
                "Client not found {mask=0x%02x,"
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
    return L7_NOT_EXIST;
  }

  /* Update client index in data cell */
  client_idx = clientInfo->deviceClientId;

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

#if PTIN_SNOOP_USE_MGMD
  /*Refresh the Timer*/
#if 0  /*This routine is not refreshing the timers!*/
  ptin_igmp_client_timer_update(intIfNum, client_idx);
#else
  ptin_igmp_client_timer_start(intIfNum, client_idx);
#endif

#endif

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
  clientInfo = igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;

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
 * Start the timer for this client
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
 * (Re)start the timer for this client
 *  
 * @param intIfNum    : Interface Number
 * @param client_idx  : client index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @notes Not working properly! 
 */
L7_RC_t ptin_igmp_client_timer_update(L7_uint32 intIfNum, L7_uint32 client_idx)
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
  rc = ptin_igmp_timer_update(ptin_port, client_idx);

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
 * @param client       : client group identification parameters 
 * @param intVid       : Internal vlan
 * @param uni_ovid     : External Outer vlan 
 * @param uni_ivid     : External Inner vlan 
 * @param onuId        : ONU/CPE Id
 * @param mask         : Applies only to the Multicast Admission
 *                     Control Parameters
 * @param maxBandwidth : [Mask = 0x01] Maximum allowed bandwidth
 *                     for this client. Use (L7_uint64)-1 to
 *                     disable.
 * @param maxChannels  : [Mask = 0x02] Maximum number of 
 *                     channels for this client. Use
 *                     (L7_uint64)-1 to disable.
 * @param addOrRemove  : Add/Remove Packages
 * @param packagePtr   : Package Bitmap Pointer 
 * @param noOfPackages : Number of Packages 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_group_client_add(ptin_client_id_t *client, L7_uint16 uni_ovid, L7_uint16 uni_ivid, L7_uint8 onuId, L7_uint8 mask, L7_uint64 maxAllowedBandwidth, L7_uint16 maxAllowedChannels, L7_BOOL addOrRemove, L7_uint32 *packagePtr, L7_uint32 noOfPackages)
{
  ptinIgmpClientDataKey_t        avl_key;
  ptinIgmpGroupClientAvlTree_t  *avl_tree;
  ptinIgmpGroupClientInfoData_t *avl_infoData;
  L7_uint32                      ptin_port;
  L7_uint32                      group_client_id;
  L7_BOOL                        newEntry = L7_FALSE;

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
  avl_tree = &igmpGroupClients.avlTree;
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
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
    /*We do not support addding new group clients when handling multicast packages!*/
    if (noOfPackages != 0)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot find key for this package{"
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
#endif

    /*Allocate Group Client Identifier*/
    if ( (group_client_id = ptin_igmp_group_client_identifier_pop(ptin_port)) >= PTIN_IGMP_CLIENTIDX_MAX )
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to Obtain Group Client Identifier (groupClientId:%u) {"              
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
              ,avl_infoData->groupClientId
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
    if ((avl_infoData=(ptinIgmpGroupClientInfoData_t *) avlSearchLVL7(&(avl_tree->igmpClientsAvlTree),(void *)&avl_key, AVL_EXACT))==L7_NULLPTR)
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
    igmpGroupClients.number_of_clients++;

#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
    memset(&avl_infoData->admissionControl,0x00,sizeof(avl_infoData->admissionControl));
#endif

    /*Save Group Client Id*/
    avl_infoData->groupClientId = group_client_id;

    /*Add Group Client Pointer*/
    igmpGroupClients.group_client[ptin_port][group_client_id] = avl_infoData;

    /*Onu Id*/
    avl_infoData->onuId = onuId;

    /*Multicast Package Feature*/
    {
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
      /*Multicast Package Initialization*/
      memset(&avl_infoData->package_bmp_list,0x00,sizeof(avl_infoData->package_bmp_list));    

      avl_infoData->number_of_packages = 0;
#endif
    }
    /*Set Flag to True*/
    newEntry = L7_TRUE;
  }
  /* ClientGroup already present */
  else
  {
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
    if (noOfPackages == 0)
#endif
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

#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
  if ( mask <= PTIN_IGMP_ADMISSION_CONTROL_MASK_VALID )
  {
    /* Save IGMP client bandwidth/channel restrictions */
    if ( (mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS)
    {
      if (maxAllowedChannels == PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS_DISABLE)  /*Disable this Parameter*/
      {
        avl_infoData->admissionControl.maxAllowedChannels = 0;         
        avl_infoData->admissionControl.allocatedChannels  = 0;         
        avl_infoData->admissionControl.mask &= ~PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS;
        if (ptin_debug_igmp_snooping)
        {
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Disabling Max Channels Control [mask:0x%x allocatedChannels:%u maxAllowedChannels:%u]",avl_infoData->admissionControl.mask, avl_infoData->admissionControl.allocatedChannels, avl_infoData->admissionControl.maxAllowedChannels);
        }
      }
      else
      {
        avl_infoData->admissionControl.maxAllowedChannels = maxAllowedChannels;    
        avl_infoData->admissionControl.mask |= PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS;

        if (newEntry == L7_TRUE)
        {
          avl_infoData->admissionControl.allocatedChannels  = 0;         
        }

        if (ptin_debug_igmp_snooping)
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Max Channels Control [mask:0x%x allocatedChannels:%u maxAllowedChannels:%u]",avl_infoData->admissionControl.mask, avl_infoData->admissionControl.allocatedChannels, avl_infoData->admissionControl.maxAllowedChannels);
        }
      }    
    }

    if ( (mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH)
    {
      if (maxAllowedBandwidth == PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_BPS_DISABLE) /*Disable this Parameter*/
      {
        avl_infoData->admissionControl.maxAllowedBandwidth = 0;
        avl_infoData->admissionControl.allocatedBandwidth  = 0;
        avl_infoData->admissionControl.mask &= ~PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH;

        if (ptin_debug_igmp_snooping)
        {
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Disabling Max Channels Control [mask:0x%x allocatedBandwidth:%u maxAllowedBandwidth:%u]",avl_infoData->admissionControl.mask, avl_infoData->admissionControl.allocatedBandwidth, avl_infoData->admissionControl.maxAllowedBandwidth);
        }
      }
      else
      {
        avl_infoData->admissionControl.maxAllowedBandwidth = maxAllowedBandwidth / 1000; /*Convert from bps to kbps*/
        avl_infoData->admissionControl.mask |= PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH;

        if (newEntry == L7_TRUE)
        {
          avl_infoData->admissionControl.allocatedBandwidth  = 0;
        }

        if (ptin_debug_igmp_snooping)
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Max Bandwidth Control [mask:0x%x allocatedBandwidth:%u maxAllowedBandwidth:%u]",avl_infoData->admissionControl.mask, avl_infoData->admissionControl.allocatedBandwidth, avl_infoData->admissionControl.maxAllowedBandwidth);
        }
      }
    }
  }
#endif

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  if (noOfPackages > 0)
  {
    if ( addOrRemove == L7_FALSE)
    {
      /*Multicast Client Packages  Add*/
      if ( L7_SUCCESS != ptin_igmp_multicast_client_packages_add(packagePtr, noOfPackages, avl_infoData))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to Add Packages {"
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
    }
    else
    {
      /*Multicast Client Packages  Remove*/
      if ( L7_SUCCESS != ptin_igmp_multicast_client_packages_remove(packagePtr, noOfPackages, avl_infoData))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to Remove Packages {"
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
    }
  }
#endif  

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
static L7_RC_t ptin_igmp_device_client_clean(ptinIgmpGroupClientInfoData_t *avl_infoData_clientGroup);

/**
 * Remove a Multicast client group
 * 
 * @param client      : client group identification parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_group_client_remove(ptin_client_id_t *client)
{
  ptinIgmpClientDataKey_t   avl_key;
  ptinIgmpGroupClientAvlTree_t *avl_tree;
  ptinIgmpGroupClientInfoData_t *avl_infoData;
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

  avl_tree = &igmpGroupClients.avlTree;
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
  if ((avl_infoData=(ptinIgmpGroupClientInfoData_t *) avlSearchLVL7( &(avl_tree->igmpClientsAvlTree), (void *)&avl_key, AVL_EXACT))==L7_NULLPTR)
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
  if (ptin_igmp_device_client_clean(avl_infoData) != L7_SUCCESS)
  {
    osapiSemaGive(ptin_igmp_clients_sem);
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not remove child clients!");
    return L7_FAILURE;
  }

  /*Release Group Client Identifier*/
  ptin_igmp_group_client_identifier_push(ptin_port, avl_infoData->groupClientId);

  /*Remove Group Client Pointer*/
  igmpGroupClients.group_client[ptin_port][avl_infoData->groupClientId] = L7_NULLPTR;

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  if ( L7_SUCCESS != ptin_igmp_multicast_client_packages_remove_all(avl_infoData))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to Remove Packages {"
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
#endif  

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
  if (igmpGroupClients.number_of_clients>0)
    igmpGroupClients.number_of_clients--;

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
L7_RC_t ptin_igmp_group_client_clean(void)
{
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpGroupClientAvlTree_t *avl_tree;
  ptinIgmpGroupClientInfoData_t *avl_infoData;
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* AVL tree refrence */
  avl_tree = &igmpGroupClients.avlTree;

  /* Get all clients */
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  while ( (avl_infoData=(ptinIgmpGroupClientInfoData_t *) avlSearchLVL7(&(avl_tree->igmpClientsAvlTree), &avl_key, L7_MATCH_GETNEXT))!=L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_infoData->igmpClientDataKey, sizeof(ptinIgmpClientDataKey_t));

    /* Remove all child clients, belonging to this client group */
    if (ptin_igmp_device_client_clean(avl_infoData) != L7_SUCCESS)
    {
      osapiSemaGive(ptin_igmp_clients_sem);
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not remove child clients!");
      return L7_FAILURE;
    }

    /*Release Group Client Identifier*/
    ptin_igmp_group_client_identifier_push(avl_infoData->ptin_port, avl_infoData->groupClientId);

    /*Remove Group Client Pointer*/
    igmpGroupClients.group_client[avl_infoData->ptin_port][avl_infoData->groupClientId] = L7_NULLPTR;

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
      if (igmpGroupClients.number_of_clients>0)
        igmpGroupClients.number_of_clients--;

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
    igmpGroupClients.number_of_clients = 0;
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
            intIfNum, intVlan, innerVlan);

  /* Add client */
  rc = ptin_igmp_new_client(&client, uni_ovid, uni_ivid, L7_TRUE, client_idx_ret);

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error adding dynamic client: intIfNum %u, intVlan %u, innerVlan %u", intIfNum, intVlan, innerVlan);
    return rc;
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"New client created: intIfNum %u, intVlan %u, innerVlan %u, client_idx_ret:%u",
            intIfNum, intVlan, innerVlan, *client_idx_ret);
  }

  #if PTIN_SNOOP_USE_MGMD
  /*Start the Timer*/
  rc = ptin_igmp_client_timer_start(intIfNum, *client_idx_ret);
  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error starting client timer: intIfNum %u, intVlan %u, innerVlan %u client_idx_ret:%u", intIfNum, intVlan, innerVlan, *client_idx_ret);
    return rc;
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Started client timer: intIfNum %u, intVlan %u, innerVlan %u, client_idx_ret:%u",
            intIfNum, intVlan, innerVlan, *client_idx_ret);
  }
  #endif

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
  clientInfo = igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;

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
L7_RC_t ptin_igmp_get_port_type(L7_uint32 intIfNum, L7_uint16 intVlan, L7_uint32 *port_type)
{
  //st_IgmpInstCfg_t *igmpInst;
  ptin_intf_t ptin_intf;
  ptin_evc_intfCfg_t intf_cfg;
  L7_uint32 evc_id;

  /* Get ptin_intf format for the interface number */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting ptin_intf from intIfNum %u",intIfNum);
    return L7_FAILURE;
  }


  if (ptin_evc_get_evcIdfromIntVlan(intVlan, &evc_id)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"No EVC id associated to intVlan %u",intVlan);
    return L7_FAILURE;
  }

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

  if (port_type != L7_NULLPTR)
  {
    if (intf_cfg.type == PTIN_EVC_INTF_LEAF)
    {
      *port_type = PTIN_EVC_INTF_LEAF;
    }
    else
    {
      if (intf_cfg.type == PTIN_EVC_INTF_ROOT)
      {
        *port_type = PTIN_EVC_INTF_ROOT;
      }
      else
      {
        return L7_FAILURE;
      }
    }
  }
  return L7_FAILURE;
}

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
/************IGMP Look Up Table Feature****************************************************/ 

static ptinIgmpGroupClientInfoData_t* deviceClientId2groupClientPtr(L7_uint32 ptin_port, L7_uint32 clientId)
{
  /*Input Arguments Validation*/
  if (ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF || clientId >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid Input Arguments: ptin_port:%u clientId:%u",ptin_port, clientId);
    return L7_NULLPTR;
  }

  return igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][clientId].client->pClientGroup;
}

static ptinIgmpGroupClientInfoData_t* groupClientId2groupClientPtr(L7_uint32 ptin_port, L7_uint32 clientId)
{
  /*Input Arguments Validation*/
  if (ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF || clientId >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid Input Arguments: ptin_port:%u clientId:%u",ptin_port, clientId);
    return L7_NULLPTR;
  }

  return igmpGroupClients.group_client[PTIN_IGMP_CLIENT_PORT(ptin_port)][clientId];

}
/************End IGMP Look Up Table Feature****************************************************/ 

/**
 * Get the MC root vlan associated to the internal vlan
 *  
 * @param intVlan       : Internal VLAN 
 * @param intIfNum      : Interface Number isLeafPort 
 * @param isLeafPort    : Port Type is Leaf
 * @param clientId      : Client Identifier
 * @param groupAddr     : Channel Group address 
 * @param sourceAddr    : Channel Source address 
 * @param mcastEvcId    : Multicast EVC Id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_mcast_evc_id_get(L7_uint16 intVlan, L7_uint32 intIfNum, L7_BOOL isLeafPort, L7_uint32 clientId, L7_inet_addr_t *groupAddr, L7_inet_addr_t *sourceAddr, L7_uint32 *mcastEvcId)
{
  L7_uint32         ptinPort;
  st_IgmpInstCfg_t *igmpInst;

  /*Input Parameters Validation*/  
  if ( intIfNum == 0 || intIfNum >= L7_MAX_INTERFACE_COUNT || (isLeafPort == L7_TRUE && clientId >= PTIN_IGMP_CLIENTIDX_MAX) || (isLeafPort == L7_FALSE && (intVlan <= PTIN_VLAN_MIN || intVlan>=PTIN_VLAN_MAX))
       ||  groupAddr == L7_NULLPTR || sourceAddr == L7_NULLPTR || mcastEvcId == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [isLeafPort:%u intVlan:%u intIfNum:%u client_idx:%u groupAddr:%p sourceAddr:%p mcastEvcId:%p]", isLeafPort, intVlan, intIfNum, clientId, groupAddr, sourceAddr, mcastEvcId);    
    return L7_FAILURE;
  }

  /* IGMP instance, from internal vlan */
  if (isLeafPort == L7_FALSE && (L7_uint16)-1 != intVlan && ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)==L7_SUCCESS)
  {
    /* This vlan is related to an EVC belonging to an IGMP instance: use its evc id */
    *mcastEvcId = igmpInst->McastEvcId;
  }
  else
  {
    if ( ptin_intf_intIfNum2port(intIfNum, &ptinPort) != L7_SUCCESS )
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to obtain ptin_port from intIfNum:%u", intIfNum);
      return L7_FAILURE;
    }

    if ( ptin_igmp_multicast_channel_service_get(ptinPort, clientId, groupAddr, sourceAddr, mcastEvcId) != L7_SUCCESS )
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to Get Multicast Service [ptin_port:%u client_idx:%u]", ptinPort, clientId);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/**
 * Get the MC root vlan associated to the internal vlan
 *  
 * @param intVlan       : Internal VLAN 
 * @param intIfNum      : Interface Number isLeafPort 
 * @param isLeafPort    : Port Type is Leaf
 * @param clientId      : Client Identifier
 * @param groupAddr     : Channel Group address 
 * @param sourceAddr    : Channel Source address 
 * @param mcastRootVlan : Multicast root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_McastRootVlan_get(L7_uint16 intVlan, L7_uint32 intIfNum, L7_BOOL isLeafPort, L7_uint32 clientId, L7_inet_addr_t *groupAddr, L7_inet_addr_t *sourceAddr, L7_uint16 *mcastRootVlan)
{
  L7_uint32         mcastEvcId;
  L7_uint32         ptinPort;
  L7_uint16         intRootVlan;
  st_IgmpInstCfg_t *igmpInst;

  /*Input Parameters Validation*/  
  if ( intIfNum == 0 || intIfNum >= L7_MAX_INTERFACE_COUNT || (isLeafPort == L7_TRUE && clientId >= PTIN_IGMP_CLIENTIDX_MAX) || (isLeafPort == L7_FALSE && (intVlan <= PTIN_VLAN_MIN || intVlan>=PTIN_VLAN_MAX))
       ||  groupAddr == L7_NULLPTR || sourceAddr == L7_NULLPTR || mcastRootVlan == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [isLeafPort:%u intVlan:%u intIfNum:%u client_idx:%u groupAddr:%p sourceAddr:%p McastRootVlan:%p]", isLeafPort, intVlan, intIfNum, clientId, groupAddr, sourceAddr, mcastRootVlan);    
    return L7_FAILURE;
  }

  /* IGMP instance, from internal vlan */
  if (isLeafPort == L7_FALSE && (L7_uint16)-1 != intVlan && ptin_igmp_inst_get_fromIntVlan(intVlan,&igmpInst,L7_NULLPTR)==L7_SUCCESS)
  {
    /* This vlan is related to an EVC belonging to an IGMP instance: use its evc id */
    mcastEvcId = igmpInst->McastEvcId;
  }
  else
  {
    if ( ptin_intf_intIfNum2port(intIfNum, &ptinPort) != L7_SUCCESS )
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to obtain ptin_port from intIfNum:%u", intIfNum);
      return L7_FAILURE;
    }

    if ( ptin_igmp_multicast_channel_service_get(ptinPort, clientId, groupAddr, sourceAddr, &mcastEvcId) != L7_SUCCESS )
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to Get Multicast Service [ptin_port:%u client_idx:%u]", ptinPort, clientId);
      return L7_FAILURE;
    }
  }

  /* Get Multicast root vlan */
  if ( ptin_evc_intRootVlan_get(mcastEvcId, &intRootVlan) != L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting McastRootVlan from MCEvcId=%u", mcastEvcId);      
    return L7_FAILURE;
  }

  /* Return Multicast root vlan */
  *mcastRootVlan = intRootVlan;  
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
 * @param intVlan        : Internal vlan
 * @param intfList       : List of interfaces 
 * @param noOfInterfaces : Number of interfaces 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_rootIntfs_getList(L7_uint16 intVlan, L7_INTF_MASK_t *intfList, L7_uint32 *noOfInterfaces)
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
  if (intfList==L7_NULLPTR || noOfInterfaces==L7_NULLPTR)
  {
    return L7_SUCCESS;
  }

  /* clear interface list */
  memset(intfList,0x00,sizeof(L7_INTF_MASK_t));

  *noOfInterfaces = 0;

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
        (*noOfInterfaces)++;
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
L7_RC_t ptin_igmp_clientIntfs_getList(L7_uint16 intVlan, L7_INTF_MASK_t *intfList, L7_uint32 *noOfInterfaces)
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
  if (intfList==L7_NULLPTR || noOfInterfaces == L7_NULLPTR)
  {
    return L7_SUCCESS;
  }

  /* clear interface list */
  memset(intfList,0x00,sizeof(L7_INTF_MASK_t));

  *noOfInterfaces = 0;

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
      if (igmpDeviceClients.number_of_clients_per_intf[ptin_port] > 0)
#else
      if (!(evcCfg.flags & PTIN_EVC_MASK_STACKED) || evcCfg.n_clientflows > 0)
#endif
      {
        if (ptin_debug_igmp_snooping)
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Port bitmap set intIfNum:%u",intIfNum);
        L7_INTF_SETMASKBIT(*intfList,intIfNum);
        (*noOfInterfaces)++;
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
  if (ptin_evc_extVlans_get(intIfNum,igmpInst->McastEvcId,(L7_uint32)-1,intIVlan,extMcastVlan,extIVlan)!=L7_SUCCESS)
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
  if (ptin_evc_extVlans_get(intIfNum,igmpInst->UcastEvcId,(L7_uint32)-1,intIVlan,extUcastVlan,extIVlan)!=L7_SUCCESS)
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
  if (bufferPoolInit(PTIN_SYSTEM_IGMP_MAXCLIENTS,
                     sizeof(timerNode_t) /*L7_APP_TMR_NODE_SIZE*/,
                     "PTin_IGMP_CtrlBlk_Timer_Bufs",
                     &bufferPoolId) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to allocate memory for IGMP Control Block timer buffers");
    return L7_FAILURE;
  }
  igmpDeviceClients.ctrlBlkBufferPoolId = bufferPoolId;

  /* Timers buffer pool */
  if (bufferPoolInit(PTIN_SYSTEM_IGMP_MAXCLIENTS,
                     sizeof(igmpTimerData_t),
                     "PTin_IGMP_Timer_Bufs",
                     &bufferPoolId) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to allocate memory for IGMP client timer buffers");
    return L7_FAILURE;
  }
  igmpDeviceClients.appTimerBufferPoolId = bufferPoolId;
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Allocated buffer pools");

  /* Create SLL list for each IGMP instance */
  if (SLLCreate(L7_PTIN_COMPONENT_ID, L7_SLL_NO_ORDER,
                sizeof(L7_uint32)*2, igmp_timer_dataCmp, igmp_timer_dataDestroy,
                &(igmpDeviceClients.ll_timerList)) != L7_SUCCESS)
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
  if (handleListInit(L7_PTIN_COMPONENT_ID, PTIN_SYSTEM_IGMP_MAXCLIENTS,
                     &handle_list, handleListMemHndl) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Unable to create timer handle list");
    return L7_FAILURE;
  }
  igmpDeviceClients.appTimer_handleListMemHndl = handleListMemHndl;
  igmpDeviceClients.appTimer_handle_list = handle_list;
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Handle list created");

  /* Initialize timer control blocks */
  timerCB = appTimerInit(L7_PTIN_COMPONENT_ID, igmp_timerExpiryHdlr,
                         (void *) 0, L7_APP_TMR_1SEC,
                         igmpDeviceClients.ctrlBlkBufferPoolId);
  if (timerCB  == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR,"snoopEntry App Timer Initialization Failed.");
    return L7_FAILURE;
  }
  igmpDeviceClients.timerCB = timerCB;
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
  if (igmpDeviceClients.timerCB != (L7_APP_TMR_CTRL_BLK_t) NULL)
  {
    appTimerDeInit(igmpDeviceClients.timerCB);
    igmpDeviceClients.timerCB = (L7_APP_TMR_CTRL_BLK_t) NULL;
  }

  /* Remove timers handle list for this IGMP instance  */
  handleListDeinit(L7_PTIN_COMPONENT_ID, igmpDeviceClients.appTimer_handle_list);

  /* Free memory for the Handle List */
  if (igmpDeviceClients.appTimer_handleListMemHndl != L7_NULLPTR)
  {
    osapiFree(L7_PTIN_COMPONENT_ID, igmpDeviceClients.appTimer_handleListMemHndl);
    igmpDeviceClients.appTimer_handleListMemHndl = L7_NULLPTR;
  }

  /* Destroy SLL list for each IGMP instance */
  SLLDestroy(L7_PTIN_COMPONENT_ID, &igmpDeviceClients.ll_timerList);

  /* Buffer pool termination */
  if (igmpDeviceClients.appTimerBufferPoolId != 0)
  {
    bufferPoolTerminate(igmpDeviceClients.appTimerBufferPoolId);
    igmpDeviceClients.appTimerBufferPoolId = 0;
  }
  if (igmpDeviceClients.ctrlBlkBufferPoolId != 0)
  {
    bufferPoolTerminate(igmpDeviceClients.ctrlBlkBufferPoolId);
    igmpDeviceClients.ctrlBlkBufferPoolId = 0;
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

  if ( (pTimerData = (igmpTimerData_t *)SLLFind(&igmpDeviceClients.ll_timerList, (void *)&timerData)) != L7_NULLPTR )
  {
    timer_exists = L7_TRUE;
  }

  if ( timer_exists )
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Already exists a timer running for ptin_port=%u client_idx=%u", ptin_port, client_idx);

#if 1
    if (pTimerData->timer != L7_NULL)
    {
      if (appTimerDelete(igmpDeviceClients.timerCB, (void *) pTimerData->timer) != L7_SUCCESS)
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
      handleListNodeDelete(igmpDeviceClients.appTimer_handle_list, &pTimerData->timerHandle);
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
    /* Buffer pool allocation for pTimerData*/
    if (bufferPoolAllocate(igmpDeviceClients.appTimerBufferPoolId, (L7_uchar8 **) &pTimerData) != L7_SUCCESS)
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
  if ((pTimerData->timerHandle = handleListNodeStore(igmpDeviceClients.appTimer_handle_list, pTimerData)) == 0)
  {
    /* Free the previously allocated bufferpool */
    bufferPoolFree(igmpDeviceClients.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
    osapiSemaGive(ptin_igmp_timers_sem);
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not get the handle node to store the timer data.");
    return L7_FAILURE;
  }
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Stored node in handle list (ptin_port=%u client_idx=%u)", ptin_port, client_idx);

  timeout = (igmpProxyCfg.querier.group_membership_interval*3)/2;

  /* Add a new timer */
  pTimerData->timer = appTimerAdd( igmpDeviceClients.timerCB, igmp_timer_expiry,
                                   (void *) pTimerData->timerHandle, timeout,
                                   "PTIN_TIMER");
  if (pTimerData->timer == NULL)
  {
    /* Free the previously allocated bufferpool */
    handleListNodeDelete(igmpDeviceClients.appTimer_handle_list, &pTimerData->timerHandle);
    pTimerData->timerHandle = 0;
    bufferPoolFree(igmpDeviceClients.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
    osapiSemaGive(ptin_igmp_timers_sem);
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not Start the Client timer.");
    return L7_FAILURE;
  }

  if ( !timer_exists )
  {
    /* Add timer to SLL */
    if (SLLAdd(&igmpDeviceClients.ll_timerList, (L7_sll_member_t *)pTimerData) != L7_SUCCESS)
    {
      /* Free the previously allocated bufferpool */
//    LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not add new timer data node");
      if (appTimerDelete( igmpDeviceClients.timerCB, pTimerData->timer) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to delete timer");
      }
      pTimerData->timer = L7_NULLPTR;
      handleListNodeDelete(igmpDeviceClients.appTimer_handle_list, &pTimerData->timerHandle);
      pTimerData->timerHandle = 0;
      bufferPoolFree(igmpDeviceClients.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
      osapiSemaGive(ptin_igmp_timers_sem);
      return L7_FAILURE;
    }
  }

  osapiSemaGive(ptin_igmp_timers_sem);

  if (ptin_debug_igmp_snooping)
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Timer Started: ptin_port=%u client_idx=%u timerCB=%p timer=%p timerHandle=%p timeout=%u (s)", ptin_port, client_idx, igmpDeviceClients.timerCB, pTimerData->timer, pTimerData->timerHandle, timeout);

  return L7_SUCCESS;
}

/**
 * Restart a timer for an existent client
 * 
 * @param ptin_port  : Interface port
 * @param client_idx : client index
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE 
 *  
 * @notes Not working properly! 
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
  if ((pTimerData = (igmpTimerData_t *)SLLFind(&igmpDeviceClients.ll_timerList, (void *)&timerData)) == L7_NULLPTR)
  {
    osapiSemaGive(ptin_igmp_timers_sem);
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Client timer not found: ptin_port=%u client_idx=%u",  ptin_port, client_idx);
    return L7_FAILURE;
  }

  if (pTimerData->timer == L7_NULL)
  {
    osapiSemaGive(ptin_igmp_timers_sem);
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Client timer not running: ptin_port=%u client_idx=%u timerCB=%p timer=%p timerHandle:%p", ptin_port, client_idx, igmpDeviceClients.timerCB, pTimerData->timer, pTimerData->timerHandle);
    return L7_FAILURE;
  }

  timeout = (igmpProxyCfg.querier.group_membership_interval*3)/2;

  if (appTimerUpdate(igmpDeviceClients.timerCB, pTimerData->timer,
                     L7_NULLPTR, L7_NULLPTR, timeout,
                     "PTIN_TIMER") != L7_SUCCESS)
  {
    osapiSemaGive(ptin_igmp_timers_sem);
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to update client timer: ptin_port=%u client_idx=%u timerCB=%p timer=%p timerHandle=%p timeout=%u (s)", ptin_port, client_idx, igmpDeviceClients.timerCB, pTimerData->timer, pTimerData->timerHandle, timeout);
    return L7_FAILURE;
  }

  osapiSemaGive(ptin_igmp_timers_sem);

  if (ptin_debug_igmp_snooping)
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Timer Updated: ptin_port=%u client_idx=%u timerCB=%p timer=%p timerHandle=%p timeout=%u (s)", ptin_port, client_idx, igmpDeviceClients.timerCB, pTimerData->timer, pTimerData->timerHandle, timeout);

  return L7_SUCCESS;
}

/**
 * Timer Tiemout for an existent client
 * 
 * @param ptin_port  : Interface port
 * @param client_idx : client index
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_uint32 ptin_igmp_timer_timeout_get(L7_uint32 ptin_port, L7_uint32 client_idx)
{
  L7_uint32 time_left = 0;
  igmpTimerData_t timerData, *pTimerData;


  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to get the timer timeout (ptin_port=%u client_idx=%u)", ptin_port, client_idx);

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
  if ((pTimerData = (igmpTimerData_t *)SLLFind(&igmpDeviceClients.ll_timerList, (void *)&timerData)) == L7_NULLPTR)
  {
    osapiSemaGive(ptin_igmp_timers_sem);
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Client timer not found: ptin_port=%u client_idx=%u",  ptin_port, client_idx);
    return L7_FAILURE;
  }

  if (pTimerData->timer == L7_NULL)
  {
    osapiSemaGive(ptin_igmp_timers_sem);
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Client timer not running: ptin_port=%u client_idx=%u timerCB=%p timer=%p timerHandle=%p", ptin_port, client_idx, igmpDeviceClients.timerCB, pTimerData->timer, pTimerData->timerHandle);
    return L7_FAILURE;
  }

  appTimerTimeLeftGet(igmpDeviceClients.timerCB, pTimerData->timer, &time_left);

  if (ptin_debug_igmp_snooping)
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Client Timer TimeOut: ptin_port=%u client_idx=%u timerCB=%p timer=%p timerHandle=%p time_left=%u (s))", ptin_port, client_idx, igmpDeviceClients.timerCB, pTimerData->timer, pTimerData->timerHandle, time_left);

  osapiSemaGive(ptin_igmp_timers_sem);

  return time_left;
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
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to stop a timer (ptin_port=%u client_idx=%u )", ptin_port, client_idx);

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
  if (SLLDelete(&igmpDeviceClients.ll_timerList, (L7_sll_member_t *)&timerData) != L7_SUCCESS)
  {
    osapiSemaGive(ptin_igmp_timers_sem);
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to delete timer node: ptin_port=%u client_idx=%u timerCB=%p timer=%p timerHandle=%p", ptin_port, client_idx, igmpDeviceClients.timerCB, timerData.timer, timerData.timerHandle);
    return L7_FAILURE;
  }

  osapiSemaGive(ptin_igmp_timers_sem);

  if (ptin_debug_igmp_snooping)
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Timer stopped successfully for ptin_port=%u client_idx=%u timerCB=%p timer=%p timerHandle=%p", ptin_port, client_idx, igmpDeviceClients.timerCB, timerData.timer, timerData.timerHandle);

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
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Expiration event ocurred for timerHandle %p!",timerHandle);

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
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Expiration event ocurred for ptin_port=%u client_idx=%u timerCB=%p timer=%p timerHandle=%p", ptin_port, client_idx, igmpDeviceClients.timerCB, pTimerData->timer, pTimerData->timerHandle);

#if 0
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
#endif
  osapiSemaGive(ptin_igmp_timers_sem);

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removing client (ptin_port=%u client_idx=%u)", ptin_port, client_idx);

  /* Remove client (only if it is dynamic. For static ones, only is removed from snooping entries) */
  if (ptin_igmp_rm_clientIdx(ptin_port, client_idx, L7_FALSE, L7_TRUE)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed removing client (ptin_port=%u client_idx=%u)!", ptin_port, client_idx);
  }
  else
  {
    if (ptin_debug_igmp_snooping)
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Client removed (ptin_port=%u client_idx=%u)", ptin_port, client_idx);
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

      appTimerProcess( igmpDeviceClients.timerCB );
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
    if (appTimerDelete(igmpDeviceClients.timerCB, pTimerData->timer)!=L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Cannot delete timer (ptin_port=%u client_idx=%u)", pTimerData->ptin_port, pTimerData->client_idx);
    }
    pTimerData->timer = (L7_APP_TMR_HNDL_t) NULL;

    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer deleted for ptin_port=%u client_idx=%u", pTimerData->ptin_port, pTimerData->client_idx);

    /* Delete the handle we had created */
    handleListNodeDelete(igmpDeviceClients.appTimer_handle_list, &pTimerData->timerHandle);
    pTimerData->timerHandle = 0;

    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Timer handle removed for ptin_port=%u client_idx=%u", pTimerData->ptin_port, pTimerData->client_idx);
  }
  else
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Timer not running for ptin_port=%u client_idx=%u", pTimerData->ptin_port, pTimerData->client_idx);
  }

  bufferPoolFree(igmpDeviceClients.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
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
  return ptin_igmp_channel_remove_all();
}

/**
 * Get the association of a particular dst/src channel.
 *  
 * @param evc_uc : MC EVC index 
 * @param evc_uc : UC EVC index
 * @param channel_group   : Group address
 * @param channel_source  : Source address
 * @param evc_mc : MC EVC pair (out)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_igmp_channel_get( L7_uint32 evc_mc,
                                      L7_inet_addr_t *channel_group,
                                      L7_inet_addr_t *channel_source,
                                      ptinIgmpChannelInfoData_t **avlEntry )
{
  ptinIgmpChannelDataKey_t   avl_key;
  ptinIgmpChannelInfoData_t *avl_infoData;
  char                       groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                       sourceAddrStr[IPV6_DISP_ADDR_LEN]={};   

  inetAddrPrint(channel_group, groupAddrStr);
  inetAddrPrint(channel_source, sourceAddrStr);
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Channel Get [evc_mc:%u groupAddr:%s sourceAddr:%s]", evc_mc, groupAddrStr, sourceAddrStr);

  /* Prepare key */
  memset( &avl_key, 0x00, sizeof(ptinIgmpChannelDataKey_t) );

  avl_key.evc_mc = evc_mc;

#if ( IGMPASSOC_CHANNEL_UC_EVC_ISOLATION )
  avl_key.evc_uc = evc_uc;
#endif

  memcpy(&avl_key.channel_group, channel_group, sizeof(L7_inet_addr_t));

#if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
  memcpy(&avl_key.channel_source, channel_source, sizeof(L7_inet_addr_t));
#endif

  
  ptin_timer_start(77,"ptin_igmp_channel_get");
  #if 0//Future Use
  /* Lock Semaphore */
  if (osapiSemaTake(channelDB.channelAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    ptin_timer_stop(77);
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to take channelDB semaphore");      
    return L7_FAILURE;
  }
  #endif
  /* Search for this key */
  avl_infoData = (ptinIgmpChannelInfoData_t *) avlSearchLVL7( &(channelDB.channelAvlTree), (void *)&avl_key, AVL_EXACT);
  #if 0//Future Use
  /* Give Semaphore */
  osapiSemaGive(channelDB.channelAvlTree.semId);
  #endif
  ptin_timer_stop(77);
  

  if ( avl_infoData == L7_NULLPTR)
  {
   
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"Channel Does Not Exist [evc_mc:%u groupAddr:%s sourceAddr:%s]", evc_mc, groupAddrStr, sourceAddrStr);      
    return L7_NOT_EXIST;      
  }
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
  else
  {
    ptin_timer_stop(77);
    if (ptin_igmp_proxy_bandwidth_control_get())
    {
      //Cache this Group and it's channelBandwidth
      ptin_igmp_channel_bandwidth_cache_set(avl_infoData);
    }
  }
#else
  ptin_timer_stop(77);
#endif

  /* Return AVL Tree Entry */
  if ( avlEntry != L7_NULLPTR )
  {
    (*avlEntry) = avl_infoData;
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
L7_RC_t ptin_igmp_channel_list_get( L7_uint32 evc_uc, L7_uint32 evc_mc,
                                    igmpAssoc_entry_t *channel_list,
                                    L7_uint16 *channels_number )
{
  ptinIgmpChannelDataKey_t avl_key;
  ptinIgmpChannelInfoData_t *avl_info;
  L7_uint16 channel_i, channels_max = PTIN_IGMP_CHANNELS_MAX;

  /* Validate arguments */
  if ( channels_number == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Define maximum number of channels to be read */
  if ( *channels_number > 0 && *channels_number < PTIN_IGMP_CHANNELS_MAX )
  {
    channels_max = *channels_number;
  }

  /* Run all cells in AVL tree */
  memset(&avl_key, 0x00, sizeof(ptinIgmpChannelDataKey_t));

  channel_i = 0;
  while ( channel_i < channels_max &&
          (avl_info=(ptinIgmpChannelInfoData_t *) avlSearchLVL7(&channelDB.channelAvlTree, (void *)&avl_key, AVL_NEXT)) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->channelDataKey, sizeof(ptinIgmpChannelDataKey_t));

    /* Verify uc evc */
#if ( IGMPASSOC_CHANNEL_UC_EVC_ISOLATION )
    if ( evc_uc != 0 && evc_uc != (L7_uint16)-1 &&
         evc_uc != avl_info->channelDataKey.evc_uc )
    {
      continue;
    }
#endif

    /* Verify MC evc */
    if ( evc_mc != 0 && evc_mc != (L7_uint16)-1 &&
         avl_info->channelDataKey.evc_mc != evc_mc )
    {
      continue;
    }

    /* Clear data */
    memset(&channel_list[channel_i], 0x00, sizeof(igmpAssoc_entry_t));

    /* UC evc */
#if ( IGMPASSOC_CHANNEL_UC_EVC_ISOLATION )
    channel_list[channel_i].evc_uc = avl_info->channelDataKey.evc_uc;
#endif
    /* Group address */
    memcpy( &channel_list[channel_i].groupAddr, &avl_info->channelDataKey.channel_group, sizeof(L7_inet_addr_t) );
    /* Source address */
#if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
    memcpy( &channel_list[channel_i].sourceAddr, &avl_info->channelDataKey.channel_source, sizeof(L7_inet_addr_t) );
#endif
    /* MC evc */
    channel_list[channel_i].evc_mc = avl_info->channelDataKey.evc_mc;
    /* Is static? */
    channel_list[channel_i].is_static = avl_info->entryType & L7_TRUE;

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
 * @param channelBandwidth: Bandwidth required for this channel
 *                  
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t igmp_assoc_channel_add( L7_uint32 evc_uc, L7_uint32 evc_mc,
                                L7_inet_addr_t *channel_group , L7_uint16 channel_grpMask,
                                L7_inet_addr_t *channel_source, L7_uint16 channel_srcMask,
                                L7_BOOL is_static, L7_uint64 channelBandwidth)
{
  L7_inet_addr_t             group; 
  L7_inet_addr_t             source;  
  L7_int32                   i, 
  n_groups=0, 
  n_sources=0;
  ptinIgmpChannelInfoData_t  avl_node;  
  L7_RC_t                    rc;

  /* Validate multicast service */
  if ( evc_mc >= PTIN_SYSTEM_N_EXTENDED_EVCS )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid MC evc (%u)", evc_mc);
    return L7_FAILURE;
  }

  //Default MC Service Gateway
  if (inetIsAddressZero(channel_group)==L7_TRUE && channel_grpMask==0)
  {
    channelDB.default_evc_mc_is_in_use = L7_TRUE;
    channelDB.default_evc_mc = evc_mc;   

#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
    channelDB.default_bandwidth = channelBandwidth/1000; /*Convert from bps to kbps*/      
#endif

    LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Added a Default MC Service Gateway [evc_mc:%u]", channelDB.default_evc_mc );
    return L7_SUCCESS;
  }

  /* Validate and prepare channel group Address*/
  if (ptin_igmp_channel_to_netmask( channel_group, channel_grpMask, &group, &n_groups)!=L7_SUCCESS || n_groups == 0 || n_groups > PTIN_IGMP_CHANNELS_MAX )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error preparing groupAddr n_groups:%u", n_groups);
    return L7_FAILURE;    
  }  
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to add group 0x%08x (%u addresses)", group.addr.ipv4.s_addr, n_groups);


  /* Prepare source channel */
  if (L7_SUCCESS != ptin_igmp_channel_to_netmask( channel_source, channel_srcMask, &source, &n_sources) || n_sources == 0 || ( (n_sources * n_groups) > PTIN_IGMP_CHANNELS_MAX))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error preparing sourceAddr n_sources:%u", n_sources);
    return L7_FAILURE;    
  }
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to add source 0x%08x (%u addresses)", source.addr.ipv4.s_addr, n_sources);

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Maximum addresses to be added: %u", n_groups*n_sources);

  /* Prepare key */
  memset( &avl_node, 0x00, sizeof(ptinIgmpChannelInfoData_t));

  avl_node.channelDataKey.evc_mc = evc_mc;

#if ( IGMPASSOC_CHANNEL_UC_EVC_ISOLATION )
  avl_node.channelDataKey.evc_uc = evc_uc;
#endif

  memcpy(&avl_node.channelDataKey.channel_group, &group, sizeof(avl_node.channelDataKey.channel_group));  

  if (is_static == L7_TRUE)
  {
    avl_node.entryType = PTIN_IGMP_ASSOC_STATIC_ENTRY;
  }
  else
  {
    avl_node.entryType = PTIN_IGMP_ASSOC_DYNAMIC_ENTRY;
  }

#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
  avl_node.channelBandwidth = channelBandwidth/1000; /*Convert from bps to kbps*/      
#endif

  /* Add channels */
  i = 0;
  rc = L7_SUCCESS;

  /* Run all group addresses */
  while ((rc==L7_SUCCESS && i<n_groups) || (rc!=L7_SUCCESS && i>=0))
  {
#if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
    L7_int32        j = 0;
    memcpy(&avl_node.channelDataKey.channel_source, &source, sizeof(avl_node.channelDataKey.channel_source));
    
    /* Run all source addresses */
    while ((rc==L7_SUCCESS && j<n_sources) || (rc!=L7_SUCCESS && j>=0))
#endif
    {
      /* In case of success, continue adding nodes into avl tree */
      if (rc == L7_SUCCESS)
      {
        if ( (rc = ptin_igmp_channel_add( &avl_node )) != L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Error inserting group channel 0x%08x, source=0x%08x for evc_mc=%u",
                  avl_node.channelDataKey.channel_group.addr.ipv4.s_addr, avl_node.channelDataKey.channel_source.addr.ipv4.s_addr, avl_node.channelDataKey.evc_mc);
        }
        else
        {
          rc = ptin_igmp_channel_get (avl_node.channelDataKey.evc_mc, &avl_node.channelDataKey.channel_group, &avl_node.channelDataKey.channel_source,  L7_NULLPTR);
          if ( rc != L7_SUCCESS )
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP,"Channel Does Not Exist group 0x%08x, source 0x%08x", avl_node.channelDataKey.channel_group.addr.ipv4.s_addr, avl_node.channelDataKey.channel_source.addr.ipv4.s_addr);                    
            return rc;      
          }
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Added group 0x%08x, source 0x%08x, evc_mc=%u", avl_node.channelDataKey.channel_group.addr.ipv4.s_addr, avl_node.channelDataKey.channel_source.addr.ipv4.s_addr, avl_node.channelDataKey.evc_mc);
        }
      }
      /* If one error ocurred, remove previously added nodes */
      else
      {
        if (ptin_igmp_channel_remove( &avl_node.channelDataKey ) != L7_SUCCESS)
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
      if (avl_node.channelDataKey.channel_source.family != L7_AF_INET6)
      {
        if (rc==L7_SUCCESS)
        {
          avl_node.channelDataKey.channel_source.addr.ipv4.s_addr++;  j++;
        }
        else
        {
          avl_node.channelDataKey.channel_source.addr.ipv4.s_addr--;  j--;
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
        avl_node.channelDataKey.channel_group.addr.ipv4.s_addr++;   i++;
      }
      else
      {
        avl_node.channelDataKey.channel_group.addr.ipv4.s_addr--;   i--;
      }
    }
    else
      break;
  }

#if 1 
  if ( rc == L7_SUCCESS )
  {
    //Only IPv4 is supported!
    if (channel_group->family!=L7_AF_INET || channel_source->family!=L7_AF_INET)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"IPv6 not supported for MGMD [UC_EVC=%u MC_EVC]",evc_uc,evc_mc);
      return L7_FAILURE;                       
    }
    if (L7_SUCCESS != ptin_igmp_mgmd_whitelist_add(evc_mc,channel_group->addr.ipv4.s_addr,channel_grpMask,channel_source->addr.ipv4.s_addr, channel_srcMask,channelBandwidth))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to create requested entry in the whitelist");
      return L7_FAILURE;
    }
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
L7_RC_t igmp_assoc_channel_remove( L7_uint32 evc_mc, L7_uint32 evc_uc, 
                                   L7_inet_addr_t *channel_group, L7_uint16 channel_grpMask,
                                   L7_inet_addr_t *channel_source, L7_uint16 channel_srcMask, L7_uint8 isStatic)
{
  L7_inet_addr_t              group;
  L7_inet_addr_t              source;  
  L7_inet_addr_t              sourceAux;
  L7_uint32                   i; 
  L7_uint32                   n_groups      = 1;
  L7_uint32                   j; 
  L7_uint32                   n_sources     = 1;  
  ptinIgmpChannelInfoData_t  *avlEntry;  
  L7_RC_t                     rc            = L7_SUCCESS;

   /* Validate multicast service */
  if ( evc_mc >= PTIN_SYSTEM_N_EXTENDED_EVCS )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid MC evc (%u)", evc_mc);
    return L7_FAILURE;
  }

  //Default MC Service Gateway
  if (inetIsAddressZero(channel_group)==L7_TRUE && channel_grpMask==0)
  {
    channelDB.default_evc_mc_is_in_use = L7_FALSE;

    LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Removed Default MC Service Gateway [evc_mc:%u]", evc_mc);
    return L7_SUCCESS;
  }

  /* Validate and prepare channel group Address*/
  if (ptin_igmp_channel_to_netmask( channel_group, channel_grpMask, &group, &n_groups)!=L7_SUCCESS || n_groups == 0 || n_groups > PTIN_IGMP_CHANNELS_MAX )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error preparing groupAddr n_groups:%u", n_groups);
    return L7_FAILURE;    
  }  
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to remove group 0x%08x (%u addresses)", group.addr.ipv4.s_addr, n_groups);

  /* Prepare source channel */
  if (L7_SUCCESS != ptin_igmp_channel_to_netmask( channel_source, channel_srcMask, &source, &n_sources) || n_sources == 0 || ( (n_sources * n_groups) > PTIN_IGMP_CHANNELS_MAX))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error preparing sourceAddr n_sources:%u", n_sources);
    return L7_FAILURE;    
  }
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to remove source 0x%08x (%u addresses)", source.addr.ipv4.s_addr, n_sources);

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Maximum addresses to be remove: %u", n_groups*n_sources);

   /* Save Source Address */
  memcpy(&sourceAux, &source, sizeof(sourceAux));

  /* Remove channels */
  for (i=0; i<n_groups; i++)
  {    
    if (i > 0)
    {
      memcpy(&source, &sourceAux, sizeof(source));
    }

    for (j=0; j<n_sources; j++)
    {
      /* Find associated MC service */
      if ( (rc =ptin_igmp_channel_get( evc_mc, &group, &source, &avlEntry )) != L7_SUCCESS )
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Channel Does Not Exist: evc_mc:%u group:%u source:%u",evc_mc, group.addr.ipv4.s_addr,  source.addr.ipv4.s_addr);        
      }
      else
      {
        switch (avlEntry->entryType)
        {
        case PTIN_IGMP_ASSOC_DYNAMIC_ENTRY:
          {
            if (isStatic == L7_TRUE)
            {
              rc = L7_DEPENDENCY_NOT_MET;              
            }
            break;
          }
        case PTIN_IGMP_ASSOC_STATIC_ENTRY:
          {
            if (isStatic != L7_TRUE)
            {
              rc = L7_DEPENDENCY_NOT_MET;              
            }
            break;
          }
        case PTIN_IGMP_ASSOC_DYNAMIC_AND_STATIC_ENTRY:
          {
            if (isStatic == L7_TRUE)
            {
              avlEntry->entryType = PTIN_IGMP_ASSOC_DYNAMIC_ENTRY;
            }
            else
            {
              avlEntry->entryType = PTIN_IGMP_ASSOC_STATIC_ENTRY;
            }
            rc = L7_DEPENDENCY_NOT_MET;
            break;
          }
        default:
          {
            LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid whitelist entryType:%u",avlEntry->entryType);  
            break;
          }
        }
      }

      /* Remove node from avl tree */
      if (rc == L7_SUCCESS)
      {
        #if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT  
        ptin_igmp_channel_bandwidth_cache_unset(&avlEntry->channelDataKey);        
        #endif

        if (ptin_igmp_channel_remove( &avlEntry->channelDataKey ) != L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing group channel 0x%08x, source=0x%08x,  evc_mc=%u",
                 group.addr.ipv4.s_addr, source.addr.ipv4.s_addr, evc_mc);
          return L7_FAILURE;
        }
        else
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removed group 0x%08x, source 0x%08x, evc_mc %u", group.addr.ipv4.s_addr,  source.addr.ipv4.s_addr, evc_mc);
        }
      }

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

#if 1
  if ( rc == L7_SUCCESS )
  {
    //Only IPv4 is supported!
    if (channel_group->family !=L7_AF_INET || channel_source->family!=L7_AF_INET)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"IPv6 not supported for MGMD [UC_EVC=%u MC_EVC=%u]", evc_uc, evc_mc);
      return FAILURE;                       
    }
    ptin_igmp_mgmd_whitelist_remove(evc_mc,channel_group->addr.ipv4.s_addr,channel_grpMask,channel_source->addr.ipv4.s_addr,channel_srcMask, 0);   
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
  L7_RC_t rc;

  /* Validate multicast service */
  if ( evc_mc >= PTIN_SYSTEM_N_EXTENDED_EVCS )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid MC eEVC (%u)", evc_mc);
    return L7_FAILURE;
  }

  /* Clear entries */
  if ( (rc = ptin_igmp_channel_remove_multicast_service(evc_uc, evc_mc)) !=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error (rc:%u) removing all channels to MC evc %u!", rc, evc_mc);
    return rc;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removed all channels to MC evc %u!", evc_mc);

  return L7_SUCCESS;
}

/**
 * Remove all associations
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_igmp_assoc_clean_all(void)
{
  L7_RC_t rc = L7_SUCCESS;

  rc = ptin_igmp_channel_remove_all();
  if ( rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error (rc:%u): failed to remove all channels!", rc);
    return rc;
  }

  /*Trigger the Removal on MGMD Lib*/
  rc = ptin_igmp_mgmd_whitelist_clean();
  if ( rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error (rc:%u): failed to remove all channels from MGMD Lib!", rc);
    return rc;
  }

  return rc;
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
static L7_RC_t ptin_igmp_channel_to_netmask( L7_inet_addr_t *channel_in, L7_uint16 channel_mask,
                                             L7_inet_addr_t *channel_out, L7_uint32 *number_of_channels)
{
  L7_uchar8 maxMasklen;

  /*Validate Input Parameters*/
  if (channel_in == L7_NULLPTR || channel_out == L7_NULLPTR || number_of_channels == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid Input Arguments: channel_in:%p channel_out:%p number_of_channels:%p");
    return L7_FAILURE;
  }

  if ( L7_SUCCESS != ptin_igmp_max_mask_size(channel_in->family, &maxMasklen) )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to obtain Max Mask Size for family:%u", channel_in->family);
    return FAILURE;
  }

  /*Validate Mask Size*/
  if (channel_mask > maxMasklen)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid maskSize:%u > maxMaskSize:%u for family:%u", channel_mask, maxMasklen, channel_in->family);
    return FAILURE;
  }

  inetMaskLenToMask(channel_in->family, channel_mask, channel_out);
  inetAddressAnd(channel_in, channel_out, channel_out);

  if (channel_mask == 0)
    *number_of_channels = 1;
  else
  {
    *number_of_channels = (1 << (maxMasklen-channel_mask));

    if (*number_of_channels > PTIN_IGMP_CHANNELS_MAX)
    {      
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Max Addresses [%u]  > PTIN_MGMD_MAX_WHITELIST [%u]",*number_of_channels, PTIN_IGMP_CHANNELS_MAX);
      *number_of_channels = 0;
      return FAILURE;
    } 
  }  
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Bit mask %u, number of Addresses %u", channel_mask, *number_of_channels);
  return L7_SUCCESS;
}

/**
 * Insert a node to the IGMPpair tree
 * 
 * @param node : node information to be added
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE;
 */
static L7_RC_t ptin_igmp_channel_add( ptinIgmpChannelInfoData_t *node )
{
  ptinIgmpChannelInfoData_t        *avl_infoData;

  /* Check if this key already exists */
  if ((avl_infoData=(ptinIgmpChannelInfoData_t *) avlSearchLVL7( &(channelDB.channelAvlTree), (void *)&node->channelDataKey, AVL_EXACT)) != L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Group channel 0x%08x already exists",
                 node->channelDataKey.channel_group.addr.ipv4.s_addr);

    if (avl_infoData->entryType != node->entryType)
    {
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
      avl_infoData->channelBandwidth = node->channelBandwidth;
#endif

      avl_infoData->entryType |= node->entryType;
      return L7_SUCCESS;
    }
    else
    {
#if 0
      return L7_REQUEST_DENIED;
#else
      return L7_SUCCESS;
#endif
    }
  }

  

  /* Check if there is enough room for one more channels */
  if (channelDB.channelAvlTree.count >= PTIN_IGMP_CHANNELS_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"No more free entries! (number_of_entries:%u >= IGMPASSOC_CHANNELS_MAX:%u)", channelDB.channelAvlTree.count, PTIN_IGMP_CHANNELS_MAX);
    return L7_FAILURE;
  }

  /* Add key */
  if (avlInsertEntry(&(channelDB.channelAvlTree), (void *)&node->channelDataKey) != L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error inserting group channel 0x%08x",
            node->channelDataKey.channel_group.addr.ipv4.s_addr);

    return L7_FAILURE;
  }

  /* Search for inserted key */
  if ((avl_infoData=(ptinIgmpChannelInfoData_t *) avlSearchLVL7(&(channelDB.channelAvlTree),(void *)&node->channelDataKey, AVL_EXACT)) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Group channel 0x%08x was added, but does not exist",
            node->channelDataKey.channel_group.addr.ipv4.s_addr);

    return L7_FAILURE;
  }

  /* Fill with remaining data */  
  avl_infoData->entryType = node->entryType;

  /*Channel Package Feature*/  
  {
    /*Initialize Group Client Bitmap*/
    memset( &avl_infoData->groupClientBmpPerPort, 0x00, sizeof(avl_infoData->groupClientBmpPerPort));

    /*Initialize number of group Clients*/
    memset( &avl_infoData->noOfGroupClientsPerPort, 0x00, sizeof(avl_infoData->noOfGroupClientsPerPort));

    /*Initialize number of group Clients*/
    memset( &avl_infoData->portBmp, 0x00, sizeof(avl_infoData->portBmp));

    avl_infoData->noOfPorts = 0;
  }

#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
  avl_infoData->channelBandwidth = node->channelBandwidth;
#endif

  /*Initialize Queue*/
  if ( L7_SUCCESS != dl_queue_init(&avl_infoData->queuePackage) )
  {
    LOG_FATAL(LOG_CTX_PTIN_IGMP,"Failed to Initialize Package Queue");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Remove a node to the IGMPpair tree
 * 
 * @param key : key information to be removed
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE;
 */
static L7_RC_t ptin_igmp_channel_remove( ptinIgmpChannelDataKey_t *avl_key )
{
  ptinIgmpChannelInfoData_t          *avl_infoData;  

  /* Check if this key does not exists */
  if ((avl_infoData=(ptinIgmpChannelInfoData_t *) avlSearchLVL7( &(channelDB.channelAvlTree), (void *) avl_key, AVL_EXACT)) == L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Group channel 0x%08x does not exist",
                avl_key->channel_group.addr.ipv4.s_addr);
    return L7_SUCCESS;
  }

  /*Check if this channel has packages attached*/
  if (avl_infoData->queuePackage.n_elems != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot remove Group channel 0x%08x, since it has packages currently attached:%u!",
            avl_key->channel_group.addr.ipv4.s_addr, avl_infoData->queuePackage.n_elems);
    return L7_FAILURE;
  }

  /*Check if this channel has packages attached*/
  if (avl_infoData->noOfPorts != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot remove Group channel 0x%08x, since it has ports currently attached:%u!",
            avl_key->channel_group.addr.ipv4.s_addr, avl_infoData->noOfPorts);
    return L7_FAILURE;
  }

  /* Remove key */
  if (avlDeleteEntry(&(channelDB.channelAvlTree), (void *) avl_key) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing group channel 0x%08x",
            avl_key->channel_group.addr.ipv4.s_addr);
    return L7_FAILURE;
  }
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
static L7_RC_t ptin_igmp_channel_remove_multicast_service( L7_uint32 evc_uc, L7_uint32 evc_mc )
{
  ptinIgmpChannelDataKey_t      avl_key;
  ptinIgmpChannelInfoData_t    *avl_info;
  struct packagePoolEntry_s    *packageEntry = L7_NULLPTR;
  char                          groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                          sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
  L7_RC_t                       rc = L7_SUCCESS;

#if 0
  /*Before removing any channel entry we need to validate if we have any package or ports attached to it*/
  {

    /* Run all cells in AVL tree */
    memset(&avl_key,0x00,sizeof(ptinIgmpChannelDataKey_t));

    while ( ( avl_info = (ptinIgmpChannelInfoData_t *)
              avlSearchLVL7(&channelDB.channelAvlTree, (void *)&avl_key, AVL_NEXT)
            ) != L7_NULLPTR )
    {
      /* Prepare next key */
      memcpy(&avl_key, &avl_info->channelDataKey, sizeof(ptinIgmpChannelDataKey_t));

      /* Check if this node will be removed */
      if (avl_info->channelDataKey.evc_mc == evc_mc
  #if IGMPASSOC_CHANNEL_UC_EVC_ISOLATION
          && avl_info->channelDataKey.evc_uc == evc_uc
  #endif
         )
      {
        if (avl_info->queuePackage.n_elems != 0 || avl_info->noOfPorts != 0)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot remove this channel (EVC_MC=%u groupAddr:0x%08x sourceAddr:0x%08x): noOfPackages:%u noPorts:%u",
                avl_key.evc_mc, avl_key.channel_group.addr.ipv4.s_addr, avl_key.channel_source.addr.ipv4.s_addr, avl_info->queuePackage.n_elems, avl_info->noOfPorts);
          return L7_FAILURE;        
        }
      }
    }
  }
#endif

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinIgmpChannelDataKey_t));

  while ( ( avl_info = (ptinIgmpChannelInfoData_t *)
            avlSearchLVL7(&channelDB.channelAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->channelDataKey, sizeof(ptinIgmpChannelDataKey_t));

    /* Check if this node will be removed */
    if (avl_info->channelDataKey.evc_mc == evc_mc
#if IGMPASSOC_CHANNEL_UC_EVC_ISOLATION
        && avl_info->channelDataKey.evc_uc == evc_uc
#endif
       )
    {
      /*If Packages Exist. Remove Them First*/
      if (avl_info->queuePackage.n_elems != 0 || avl_info->noOfPorts != 0)
      {
        packageEntry = L7_NULLPTR;    
        while ( L7_NULLPTR != (packageEntry = queue_package_entry_get_next(avl_info, packageEntry)) && 
            (packageEntry->packageId < PTIN_SYSTEM_IGMP_MAXPACKAGES) )
        {        
          if ( (rc = ptin_igmp_multicast_package_channel_remove(packageEntry->packageId, avl_info->channelDataKey.evc_mc, &avl_info->channelDataKey.channel_group, &avl_info->channelDataKey.channel_source)) != L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing channel from multicast package [packageId:%u serviceId:%u groupAddr:%s sourceAddr:%s]",
                    packageEntry->packageId, avl_info->channelDataKey.evc_mc, inetAddrPrint(&avl_info->channelDataKey.channel_group, groupAddrStr), inetAddrPrint(&avl_info->channelDataKey.channel_source, sourceAddrStr));
            return rc;
          }
        }

        /*Validate If No Dependency Exist*/
        if (avl_info->queuePackage.n_elems != 0 || avl_info->noOfPorts != 0)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot remove this channel (EVC_MC=%u groupAddr:0x%08x sourceAddr:0x%08x): noOfPackages:%u noPorts:%u",
                avl_key.evc_mc, avl_key.channel_group.addr.ipv4.s_addr, avl_key.channel_source.addr.ipv4.s_addr, avl_info->queuePackage.n_elems, avl_info->noOfPorts);
          return L7_FAILURE;        
        }
      }      

      /* Remove key */
      if ( avlDeleteEntry(&(channelDB.channelAvlTree), (void *)&avl_key) == L7_NULLPTR )
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing channel (groupAddr:0x%08x sourceAddr:0x%08x EVC_MC=%u)",
                avl_key.channel_group.addr.ipv4.s_addr, avl_key.channel_source.addr.ipv4.s_addr, avl_key.evc_mc);
        rc = L7_FAILURE;
      }
      else
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removed channel (groupAddr:0x%08x sourceAddr:0x%08x EVC_MC=%u)",
                  avl_key.channel_group.addr.ipv4.s_addr, avl_key.channel_source.addr.ipv4.s_addr, avl_key.evc_mc);
      }
    }
  }

  if (channelDB.default_evc_mc_is_in_use == L7_TRUE && channelDB.default_evc_mc == evc_mc)
  {
    channelDB.default_bandwidth = 0;

    channelDB.default_evc_mc = 0;

    channelDB.default_evc_mc_is_in_use = L7_FALSE;
  }

  return rc;
}

/**
 * Remove all nodes from the IGMPpair tree. 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE;
 */
static L7_RC_t ptin_igmp_channel_remove_all( void )
{
#if 0
  /*Before removing any channel entry we need to validate if we have any package or ports attached to it*/
  {
    ptinIgmpChannelDataKey_t avl_key;
    ptinIgmpChannelInfoData_t *avl_info;
   
    /* Run all cells in AVL tree */
    memset(&avl_key,0x00,sizeof(ptinIgmpChannelDataKey_t));

    while ( ( avl_info = (ptinIgmpChannelInfoData_t *)
              avlSearchLVL7(&channelDB.channelAvlTree, (void *)&avl_key, AVL_NEXT)
            ) != L7_NULLPTR )
    {
      if (avl_info->queuePackage.n_elems != 0 || avl_info->noOfPorts != 0)
      {    
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot remove this channel (EVC_MC=%u groupAddr:0x%08x sourceAddr:0x%08x): noOfPackages:%u noPorts:%u",
              avl_key.evc_mc, avl_key.channel_group.addr.ipv4.s_addr, avl_key.channel_source.addr.ipv4.s_addr, avl_info->queuePackage.n_elems, avl_info->noOfPorts);
        return L7_DEPENDENCY_NOT_MET;        
      }
    }
  }
#endif


  /* Purge all AVL tree, but the root node */
  avlPurgeAvlTree( &channelDB.channelAvlTree, PTIN_IGMP_CHANNELS_MAX );

  channelDB.default_bandwidth = 0;

  channelDB.default_evc_mc = 0;

  channelDB.default_evc_mc_is_in_use = L7_FALSE;

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
    if (ptin_igmp_evc_trap_configure(evc_idx, enable) != L7_SUCCESS)
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
    ptin_igmp_evc_trap_configure(evc_idx, !enable);
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
    LOG_NOTICE(LOG_CTX_PTIN_IGMP,"IGMP instance %u is not active", igmp_idx);
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
static L7_RC_t ptin_igmp_device_client_clean(ptinIgmpGroupClientInfoData_t *clientGroup)
{
  L7_uint ptin_port, client_idx;
  ptinIgmpDeviceClient_t *client_device;

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
    if (client_device->client == L7_NULLPTR || client_device->client->deviceClientId >= PTIN_IGMP_CLIENTIDX_MAX)
      continue;

    /* Client index */
    ptin_port  = client_device->client->ptin_port;
    client_idx = client_device->client->deviceClientId;

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
static L7_RC_t ptin_igmp_group_client_find(ptin_client_id_t *client_ref, ptinIgmpGroupClientInfoData_t **client_info)
{
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpGroupClientAvlTree_t  *avl_tree;
  ptinIgmpGroupClientInfoData_t *clientInfo;
#if (MC_CLIENT_INTERF_SUPPORTED)
  L7_uint32 ptin_port;
#endif

  /*Input Arguments Validation*/
  if (client_ref == L7_NULLPTR || client_info == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid Input Arguments client_ref:[%p] client_info[%p]", client_ref, client_info);
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

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* Key to search for */
  avl_tree = &igmpGroupClients.avlTree;
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
  *client_info = clientInfo;

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
                                    L7_BOOL isDynamic, L7_uint *device_client_id_ret)
{
  L7_uint device_client_id;
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
  avl_tree = &igmpDeviceClients.avlTree;
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
    ptinIgmpGroupClientAvlTree_t *avl_tree_group;
    ptinIgmpGroupClientInfoData_t *clientGroup = L7_NULLPTR;

    avl_tree_group = &igmpGroupClients.avlTree;

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
    if (igmpDeviceClients.queue_free_clientDevices[PTIN_IGMP_CLIENT_PORT(ptin_port)].n_elems == 0)
    {
      osapiSemaGive(ptin_igmp_clients_sem);
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"No more free clients available!");
      return L7_FAILURE;
    }

    /* Get new client index */
    if ((device_client_id=ptin_igmp_device_client_identifier_pop(ptin_port, clientGroup)) >= PTIN_IGMP_CLIENTIDX_MAX)
    {
      osapiSemaGive(ptin_igmp_clients_sem);
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot get new client index %u for ptin_port %u",device_client_id, ptin_port);
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

      ptin_igmp_device_client_identifier_push(ptin_port, device_client_id);
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

      ptin_igmp_device_client_identifier_push(ptin_port, device_client_id);
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
    avl_infoData->deviceClientId = device_client_id;

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
    igmp_clientIndex_mark(ptin_port, device_client_id, avl_infoData);
  }
  else
  {
    if (ptin_debug_igmp_snooping)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"This key {"
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
                ,avl_infoData->deviceClientId);
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
  if (device_client_id_ret!=L7_NULLPTR)
  {
    *device_client_id_ret = avl_infoData->deviceClientId;
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
  ptinIgmpGroupClientInfoData_t *clientGroup = L7_NULLPTR;
  L7_uint   ptin_port;
  L7_uint32 intIfNum;
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* AVL tree refrence */
  avl_tree = &igmpDeviceClients.avlTree;

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
        (avl_infoData->pClientGroup==L7_NULLPTR 
#if !PTIN_SNOOP_USE_MGMD      
         || avl_infoData->pClientGroup->stats_client.active_groups>0
#endif
        ))
      continue;


    /* Save client index */
    ptin_port  = avl_infoData->ptin_port;
    client_idx = avl_infoData->deviceClientId;

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
#if PTIN_SNOOP_USE_MGMD
      L7_uint32                   intIfNum;   

      /*Remove this Client From MGMD*/
      if (ptin_intf_port2intIfNum(ptin_port, &intIfNum) == L7_SUCCESS)
      {
        ptin_igmp_mgmd_client_remove(intIfNum, client_idx);
      }
#endif

      if (clientGroup != L7_NULLPTR)
      {
        /* Remove device from client group */
        igmp_clientDevice_remove(clientGroup, avl_infoData);
      }

      /* Remove client from unified list of clients */
      igmp_clientIndex_unmark(ptin_port, client_idx);

      /* Free client index */
      ptin_igmp_device_client_identifier_push(ptin_port, client_idx);

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
  ptinIgmpGroupClientInfoData_t *clientGroup = L7_SUCCESS;
  L7_RC_t rc;

  if (ptin_port >= PTIN_SYSTEM_N_INTERF || client_idx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    //if (ptin_debug_igmp_snooping)
    //  LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get pointer to client structure in AVL tree */
  clientInfo = igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;

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
       (clientInfo->pClientGroup!=L7_NULLPTR 
#if !PTIN_SNOOP_USE_MGMD      
        && clientInfo->pClientGroup->stats_client.active_groups==0
#endif
       ) )
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
      avl_tree = &igmpDeviceClients.avlTree;

      /* Save client group reference */
      clientGroup = clientInfo->pClientGroup;

      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to unmark ptin_port=%u client_idx=%u", ptin_port, client_idx);

      /* Remove device from client group */
      if (clientGroup != L7_NULLPTR)
      {
        igmp_clientDevice_remove(clientGroup, clientInfo);
      }

#if PTIN_SNOOP_USE_MGMD
      L7_uint32                   intIfNum;   

      /*Remove this Client From MGMD*/
      if (ptin_intf_port2intIfNum(ptin_port, &intIfNum) == L7_SUCCESS)
      {
        ptin_igmp_mgmd_client_remove(intIfNum, client_idx);
      }
#endif

      /* Remove client from unified list of clients */
      igmp_clientIndex_unmark(ptin_port, client_idx);

      /* Free client index */
      ptin_igmp_device_client_identifier_push(ptin_port, client_idx);

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
    }
  }

#ifndef PTIN_SNOOP_USE_MGMD
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
#endif

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
  L7_uint16 mc_vlan;

  enable &= 1;

  /* Get root vlan for MC evc, and add it for packet trapping */
  if (ptin_evc_intRootVlan_get(evc_idx_mc, &mc_vlan) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Can't get MC root vlan for evc id %u", evc_idx_mc);
    return L7_FAILURE;
  }

#if (PTIN_QUATTRO_FLOWS_FEATURE_ENABLED && QUATTRO_IGMP_TRAP_PREACTIVE)
  if (!PTIN_VLAN_IS_QUATTRO(mc_vlan))
#endif
  {
    /* Configure packet trapping for MC VLAN  */
    if (usmDbSnoopVlanModeSet(1, mc_vlan, enable, L7_AF_INET) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring vlan %u for packet trapping (enable=%u)", mc_vlan, enable);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success configuring vlan %u for packet trapping (enable=%u)", mc_vlan, enable);
  }

#if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
  L7_uint16 uc_vlan;

  /* Unicast VLAN */
  if (ptin_evc_intRootVlan_get(evc_idx_uc, &uc_vlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Can't get UC root vlan for evc id %u", evc_idx_uc);
    return L7_FAILURE;
  }

  /* Configure packet trapping for UC VLAN  */
#if (PTIN_QUATTRO_FLOWS_FEATURE_ENABLED && QUATTRO_IGMP_TRAP_PREACTIVE)
  if (!PTIN_VLAN_IS_QUATTRO(mc_vlan))
#endif
  {
    if (usmDbSnoopVlanModeSet(1, uc_vlan, enable, L7_AF_INET) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring vlan %u for packet trapping (enable=%u)", uc_vlan, enable);
      usmDbSnoopVlanModeSet(1, mc_vlan, !enable, L7_AF_INET);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success configuring vlan %u for packet trapping (enable=%u)", uc_vlan, enable);
  }
#endif

  return L7_SUCCESS;
}

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
/**
 * Configure an IGMP vlan trapping rule (most essentally for the
 * UC services) 
 * 
 * @param evc_idx   : evc index
 * @param enable    : enable flag 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_igmp_evc_trap_configure(L7_uint32 evc_idx, L7_BOOL enable)
{
  L7_uint16 vlan;

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

  /* Configure root vlan (stacked and unstacked services) */
  if (ptin_evc_intRootVlan_get(evc_idx, &vlan)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Can't get root vlan for evc id %u",evc_idx);
    return L7_FAILURE;
  }

  /* Configure packet trapping for this VLAN  */
#if (PTIN_QUATTRO_FLOWS_FEATURE_ENABLED && QUATTRO_IGMP_TRAP_PREACTIVE)
  if (!PTIN_VLAN_IS_QUATTRO(vlan))
#endif
  {
    if (usmDbSnoopVlanModeSet(1, vlan, enable, L7_AF_INET) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring vlan %u for packet trapping (enable=%u)", vlan, enable);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success configuring vlan %u for packet trapping (enable=%u)", vlan, enable);
  }

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
mgmdQueryInstances_t* ptin_mgmd_query_instances_get(L7_uint32 *mgmdNumberOfQueryInstancesPtr)
{
  (*mgmdNumberOfQueryInstancesPtr) =mgmdNumberOfQueryInstances;

  return mgmdQueryInstances;
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
  if (ctrlResMsg.res==L7_SUCCESS)
  {
    L7_uint16 iterator;
    for (iterator=0; iterator<PTIN_SYSTEM_N_EVCS; iterator++)
    {
      if (enable==L7_TRUE)
      {
        if (mgmdQueryInstances[iterator].inUse==L7_FALSE)
        {
          mgmdQueryInstances[iterator].UcastEvcId=evc_idx;
          mgmdQueryInstances[iterator].inUse=L7_TRUE;
          ++mgmdNumberOfQueryInstances;
          break;
        }
      }
      else
      {
        if (mgmdQueryInstances[iterator].inUse==L7_TRUE && mgmdQueryInstances[iterator].UcastEvcId==evc_idx)
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

/**
 * Removes all groups related to this port Id
 * 
 * @param intfnum : Multicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_mgmd_port_remove(L7_uint32 intIfNum)
{
  PTIN_MGMD_EVENT_t               reqMsg        = {0};
  PTIN_MGMD_EVENT_t               resMsg        = {0};
  PTIN_MGMD_EVENT_CTRL_t          ctrlResMsg    = {0};
  PTIN_MGMD_CTRL_PORT_REMOVE_t mgmdConfigMsg = {0}; 

  mgmdConfigMsg.portId = intIfNum;
  ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_PORT_REMOVE, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdConfigMsg, sizeof(PTIN_MGMD_CTRL_PORT_REMOVE_t));
  ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
  ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Res     : %u",   ctrlResMsg.res);

  return ctrlResMsg.res;
}

/**
 * Removes all groups related to this client Id
 * 
 * @param intfnum : Multicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_mgmd_client_remove(L7_uint32 intIfNum, L7_uint32 clientId)
{
  PTIN_MGMD_EVENT_t               reqMsg        = {0};
  PTIN_MGMD_EVENT_t               resMsg        = {0};
  PTIN_MGMD_EVENT_CTRL_t          ctrlResMsg    = {0};
  PTIN_MGMD_CTRL_CLIENT_REMOVE_t mgmdConfigMsg = {0}; 

  mgmdConfigMsg.portId   = intIfNum;
  mgmdConfigMsg.clientId = clientId;

  ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_CLIENT_REMOVE, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdConfigMsg, sizeof(PTIN_MGMD_CTRL_CLIENT_REMOVE_t));
  ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
  ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Res     : %u",   ctrlResMsg.res);

  return ctrlResMsg.res;
}

L7_RC_t ptin_igmp_mgmd_whitelist_add(L7_uint16 serviceId, L7_uint32 groupAddr, L7_uint8 groupMaskLen, L7_uint32 sourceAddr, L7_uint8 sourceMaskLen, L7_uint64 bw)
{
  PTIN_MGMD_EVENT_t                 reqMsg        = {0};
  PTIN_MGMD_EVENT_t                 resMsg        = {0};
  PTIN_MGMD_EVENT_CTRL_t            ctrlResMsg    = {0};
  PTIN_MGMD_CTRL_WHITELIST_CONFIG_t mgmdConfigMsg = {0}; 

  mgmdConfigMsg.serviceId    = serviceId;
  mgmdConfigMsg.groupIp      = groupAddr;
  mgmdConfigMsg.groupMaskLen = groupMaskLen;
  mgmdConfigMsg.sourceIp     = sourceAddr;
  mgmdConfigMsg.sourceMaskLen= sourceMaskLen;
  mgmdConfigMsg.bw           = bw;

  ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_WHITELIST_ADD, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdConfigMsg, sizeof(PTIN_MGMD_CTRL_WHITELIST_CONFIG_t));
  ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
  ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Res     : %u",   ctrlResMsg.res);

  return ctrlResMsg.res;
}

L7_RC_t ptin_igmp_mgmd_whitelist_remove(L7_uint16 serviceId, L7_uint32 groupAddr, L7_uint8 groupMaskLen, L7_uint32 sourceAddr, L7_uint8 sourceMaskLen, L7_uint64 bw)
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
  mgmdConfigMsg.bw            = bw;

  ptin_mgmd_event_ctrl_create(&reqMsg, PTIN_MGMD_EVENT_CTRL_WHITELIST_REMOVE, rand(), 0, ptinMgmdTxQueueId, (void*)&mgmdConfigMsg, sizeof(PTIN_MGMD_CTRL_WHITELIST_CONFIG_t));
  ptin_mgmd_sendCtrlEvent(&reqMsg, &resMsg);
  ptin_mgmd_event_ctrl_parse(&resMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Response");
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "  CTRL Res     : %u",   ctrlResMsg.res);

  return ctrlResMsg.res;
}

L7_RC_t ptin_igmp_mgmd_whitelist_clean(void)
{
  PTIN_MGMD_EVENT_t txMsg   = {0};
  uint32            params[PTIN_MGMD_EVENT_DEBUG_PARAM_MAX]  = {0}; 

  ptin_mgmd_event_debug_create(&txMsg, PTIN_MGMD_EVENT_DEBUG_WHITELIST_CLEAN, (void*)&params, 0);
  if (SUCCESS != ptin_mgmd_eventQueue_tx(&txMsg))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to sent event");
    return L7_FAILURE;
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
  avl_tree = &igmpDeviceClients.avlTree;
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
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Key {"
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

  memset(statistics, 0x00, sizeof(PTIN_MGMD_CTRL_STATS_RESPONSE_t));

  /*Added to Include the Stats from FP*/
  {
    ptin_IGMP_Statistics_t *stat_port_g = L7_NULLPTR;
    L7_uint32 ptin_port;

    if ( ptin_intf_ptintf2port(ptin_intf,&ptin_port) != SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting port %u/%u to ptin_port",ptin_intf->intf_type,ptin_intf->intf_id);
      return L7_FAILURE;
    }

    /* Global interface statistics at interface level */
    stat_port_g = &global_stats_intf[ptin_port];

    statistics->v2.joinRx                          = stat_port_g->joins_received_success +
                                                     stat_port_g->joins_received_dropped;

    statistics->v2.joinInvalidRx                   = stat_port_g->joins_received_invalid; 

    statistics->v2.leaveRx                         = stat_port_g->leaves_received +
                                                     stat_port_g->leaves_received_dropped;

    statistics->v2.leaveInvalidRx                  = stat_port_g->leaves_received_invalid;  

    statistics->v3.membershipReportRx              = stat_port_g->igmpv3.membership_report_valid_rx +
                                                     stat_port_g->igmpv3.membership_report_dropped_rx;

    statistics->v3.membershipReportInvalidRx       = stat_port_g->igmpv3.membership_report_invalid_rx;      

    statistics->query.generalQueryRx               = stat_port_g->igmpquery.general_query_valid_rx +
                                                     stat_port_g->igmpquery.general_query_dropped_rx +
                                                     stat_port_g->igmpquery.generic_query_invalid_rx;

    statistics->igmpInvalidRx                      = stat_port_g->igmpquery.generic_query_invalid_rx + 
                                                     stat_port_g->joins_received_invalid + 
                                                     stat_port_g->leaves_received_invalid +                                                       
                                                     stat_port_g->igmpv3.membership_report_invalid_rx + 
                                                     stat_port_g->igmpquery.generic_query_invalid_rx +
                                                     stat_port_g->igmp_received_invalid;                                                    

    statistics->igmpDroppedRx                      = stat_port_g->joins_received_dropped + 
                                                     stat_port_g->leaves_received_dropped + 
                                                     stat_port_g->igmpv3.membership_report_dropped_rx + 
                                                     stat_port_g->igmpquery.general_query_dropped_rx +
                                                     stat_port_g->igmp_dropped;

    statistics->igmpValidRx                        = stat_port_g->joins_received_success +
                                                     stat_port_g->leaves_received +                                                        
                                                     stat_port_g->igmpv3.membership_report_valid_rx +
                                                     stat_port_g->igmpquery.general_query_valid_rx +
                                                     stat_port_g->igmp_received_valid;                                                

    statistics->igmpTotalRx                        = statistics->igmpInvalidRx + 
                                                     statistics->igmpDroppedRx + 
                                                     statistics->igmpValidRx;      
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
  if (SUCCESS == ctrlResMsg.res)
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
  if (SUCCESS == ctrlResMsg.res)
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
L7_RC_t ptin_igmp_stat_client_get(L7_uint32 evc_idx, const ptin_client_id_t *client_id, PTIN_MGMD_CTRL_STATS_RESPONSE_t *statistics)
{
  ptin_client_id_t                client;
  ptinIgmpGroupClientInfoData_t  *clientGroup;
  PTIN_MGMD_EVENT_t               reqMsg          = {0};
  PTIN_MGMD_EVENT_t               resMsg          = {0};
  PTIN_MGMD_EVENT_CTRL_t          ctrlResMsg      = {0};
  PTIN_MGMD_CTRL_STATS_REQUEST_t  mgmdStatsReqMsg = {0};
  PTIN_MGMD_CTRL_STATS_RESPONSE_t mgmdStatsResMsg = {0};
  L7_uint32                       clientId;
  L7_uint32                       intIfNum;

  memset(statistics, 0x00, sizeof(PTIN_MGMD_CTRL_STATS_RESPONSE_t));

  memcpy(&client, client_id, sizeof(ptin_client_id_t));

  /* Validate and rearrange clientId info */
  if (ptin_igmp_clientId_convert(evc_idx, &client) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client id");
    return L7_FAILURE;
  }

  /* Get client */
  if (ptin_igmp_group_client_find(&client, &clientGroup) != L7_SUCCESS)
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
    return L7_FAILURE;
  }

  /*Take Semaphore*/
  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(&client.ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting port %u/%u to intIfNum",client.ptin_intf.intf_type,client.ptin_intf.intf_id);
    /*Give Semaphore*/
    osapiSemaGive(ptin_igmp_clients_sem);
    return L7_FAILURE;
  }

  /*Added to Include the Stats from FP*/
  {
    statistics->v2.joinRx                          = clientGroup->stats_client.joins_received_success +
                                                     clientGroup->stats_client.joins_received_dropped;

    statistics->v2.joinInvalidRx                   = clientGroup->stats_client.joins_received_invalid; 

    statistics->v2.leaveRx                         = clientGroup->stats_client.leaves_received +
                                                     clientGroup->stats_client.leaves_received_dropped;

    statistics->v2.leaveInvalidRx                  = clientGroup->stats_client.leaves_received_invalid;  

    statistics->v3.membershipReportRx              = clientGroup->stats_client.igmpv3.membership_report_valid_rx +
                                                     clientGroup->stats_client.igmpv3.membership_report_dropped_rx;

    statistics->v3.membershipReportInvalidRx       = clientGroup->stats_client.igmpv3.membership_report_invalid_rx;      

    statistics->query.generalQueryRx               = clientGroup->stats_client.igmpquery.general_query_valid_rx +
                                                     clientGroup->stats_client.igmpquery.general_query_dropped_rx +
                                                     clientGroup->stats_client.igmpquery.generic_query_invalid_rx;

    statistics->igmpInvalidRx                      = clientGroup->stats_client.igmpquery.generic_query_invalid_rx + 
                                                     clientGroup->stats_client.joins_received_invalid + 
                                                     clientGroup->stats_client.leaves_received_invalid +                                                       
                                                     clientGroup->stats_client.igmpv3.membership_report_invalid_rx + 
                                                     clientGroup->stats_client.igmpquery.generic_query_invalid_rx +
                                                     clientGroup->stats_client.igmp_received_invalid;                                                    

    statistics->igmpDroppedRx                      = clientGroup->stats_client.joins_received_dropped + 
                                                     clientGroup->stats_client.leaves_received_dropped + 
                                                     clientGroup->stats_client.igmpv3.membership_report_dropped_rx + 
                                                     clientGroup->stats_client.igmpquery.general_query_dropped_rx +
                                                     clientGroup->stats_client.igmp_dropped;

    statistics->igmpValidRx                        = clientGroup->stats_client.joins_received_success +
                                                     clientGroup->stats_client.leaves_received +                                                      
                                                     clientGroup->stats_client.igmpv3.membership_report_valid_rx +
                                                     clientGroup->stats_client.igmpquery.general_query_valid_rx +
                                                     clientGroup->stats_client.igmp_received_valid;              

    statistics->igmpTotalRx                        = statistics->igmpInvalidRx + 
                                                     statistics->igmpDroppedRx + 
                                                     statistics->igmpValidRx;             
  }                                              

  L7_uint16 noOfClients=igmp_clientDevice_get_devices_number(clientGroup);
  if (noOfClients>0)
  {
    L7_uint16 noOfClientsFound=0;
    for (clientId = 0; clientId < PTIN_IGMP_CLIENTIDX_MAX; ++clientId)
    {
      /*Check if this position on the Client Array is Empty*/
      if (IS_BITMAP_WORD_SET(clientGroup->client_bmp_list, clientId, UINT32_BITSIZE) == L7_FALSE)
      {
        //Next Position on the Array of Clients. -1 since the for adds 1 unit.
        clientId += UINT32_BITSIZE - 1;
        continue;
      }

      if (IS_BITMAP_BIT_SET(clientGroup->client_bmp_list, clientId, UINT32_BITSIZE))
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

        if (L7_SUCCESS != ctrlResMsg.res)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Error reading clientId[%u] statistics rc:%u", clientId, ctrlResMsg.res);
          /*Give Semaphore*/
          osapiSemaGive(ptin_igmp_clients_sem);
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
        statistics->v2.joinRx                          += mgmdStatsResMsg.v2.joinRx;   
        statistics->v2.joinInvalidRx                   += mgmdStatsResMsg.v2.joinInvalidRx;    
        statistics->v2.leaveTx                         += mgmdStatsResMsg.v2.leaveTx;              
        statistics->v2.leaveRx                         += mgmdStatsResMsg.v2.leaveRx;    
        statistics->v2.leaveInvalidRx                  += mgmdStatsResMsg.v2.leaveInvalidRx;    

        statistics->v3.membershipReportTx              += mgmdStatsResMsg.v3.membershipReportTx; 
        statistics->v3.membershipReportRx              += mgmdStatsResMsg.v3.membershipReportRx;
        statistics->v3.membershipReportInvalidRx       += mgmdStatsResMsg.v3.membershipReportInvalidRx;          
        statistics->v3.groupRecords.allowTx            += mgmdStatsResMsg.v3.groupRecords.allowTx;
        statistics->v3.groupRecords.allowRx       += mgmdStatsResMsg.v3.groupRecords.allowRx;
        statistics->v3.groupRecords.allowInvalidRx     += mgmdStatsResMsg.v3.groupRecords.allowInvalidRx;
        statistics->v3.groupRecords.blockTx            += mgmdStatsResMsg.v3.groupRecords.blockTx;
        statistics->v3.groupRecords.blockRx       += mgmdStatsResMsg.v3.groupRecords.blockRx;
        statistics->v3.groupRecords.blockInvalidRx     += mgmdStatsResMsg.v3.groupRecords.blockInvalidRx;
        statistics->v3.groupRecords.isIncludeTx        += mgmdStatsResMsg.v3.groupRecords.isIncludeTx;
        statistics->v3.groupRecords.isIncludeRx   += mgmdStatsResMsg.v3.groupRecords.isIncludeRx;
        statistics->v3.groupRecords.isIncludeInvalidRx += mgmdStatsResMsg.v3.groupRecords.isIncludeInvalidRx;
        statistics->v3.groupRecords.isExcludeTx        += mgmdStatsResMsg.v3.groupRecords.isExcludeTx;
        statistics->v3.groupRecords.isExcludeRx   += mgmdStatsResMsg.v3.groupRecords.isExcludeRx;
        statistics->v3.groupRecords.isExcludeInvalidRx += mgmdStatsResMsg.v3.groupRecords.isExcludeInvalidRx;
        statistics->v3.groupRecords.toIncludeTx        += mgmdStatsResMsg.v3.groupRecords.toIncludeTx;
        statistics->v3.groupRecords.toIncludeRx   += mgmdStatsResMsg.v3.groupRecords.toIncludeRx;
        statistics->v3.groupRecords.toIncludeInvalidRx += mgmdStatsResMsg.v3.groupRecords.toIncludeInvalidRx;
        statistics->v3.groupRecords.toExcludeTx        += mgmdStatsResMsg.v3.groupRecords.toExcludeTx;
        statistics->v3.groupRecords.toExcludeRx   += mgmdStatsResMsg.v3.groupRecords.toExcludeRx;
        statistics->v3.groupRecords.toExcludeInvalidRx += mgmdStatsResMsg.v3.groupRecords.toExcludeInvalidRx;                                  

        statistics->query.generalQueryTx               += mgmdStatsResMsg.query.generalQueryTx;     
        statistics->query.generalQueryRx          += mgmdStatsResMsg.query.generalQueryRx;
        statistics->query.groupQueryTx                 += mgmdStatsResMsg.query.groupQueryTx;       
        statistics->query.groupQueryRx            += mgmdStatsResMsg.query.groupQueryRx;  
        statistics->query.sourceQueryTx                += mgmdStatsResMsg.query.sourceQueryTx;      
        statistics->query.sourceQueryRx           += mgmdStatsResMsg.query.sourceQueryRx; 

        if (noOfClientsFound++>=noOfClients)
          break;
      }
    }
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "This groupClient has zero devices currently attached!");
  }
  /*Give Semaphore*/
  osapiSemaGive(ptin_igmp_clients_sem);

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
L7_RC_t ptin_igmp_stat_client_clear(L7_uint32 evc_idx, const ptin_client_id_t *client_id)
{
  ptin_client_id_t client;
  ptinIgmpGroupClientInfoData_t  *clientGroup;
  PTIN_MGMD_EVENT_t              reqMsg          = {0};
  PTIN_MGMD_EVENT_t              resMsg          = {0};
  PTIN_MGMD_EVENT_CTRL_t         ctrlResMsg      = {0};
  PTIN_MGMD_CTRL_STATS_REQUEST_t mgmdStatsReqMsg = {0};
  L7_uint32                      clientId;
  L7_uint32                      intIfNum;

  memcpy(&client, client_id, sizeof(ptin_client_id_t));

  /* Validate and rearrange clientId info */
  if (ptin_igmp_clientId_convert(evc_idx, &client) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid client id");
    return L7_FAILURE;
  }

  /* Find client */
  if (ptin_igmp_group_client_find(&client, &clientGroup) != L7_SUCCESS)
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
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(&client.ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting port %u/%u to intIfNum",client.ptin_intf.intf_type,client.ptin_intf.intf_id);
    return L7_FAILURE;
  }

  for (clientId=0; clientId<PTIN_IGMP_CLIENTIDX_MAX; ++clientId)
  {
    /*Check if this position on the Client Array is Empty*/
    if (IS_BITMAP_WORD_SET(clientGroup->client_bmp_list, clientId, UINT32_BITSIZE) == L7_FALSE)
    {
      //Next Position on the Array of Clients. -1 since the for adds 1 unit.
      clientId += UINT32_BITSIZE - 1;
      continue;
    }

    if (IS_BITMAP_BIT_SET(clientGroup->client_bmp_list, clientId, UINT32_BITSIZE))
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

      if (L7_SUCCESS != ctrlResMsg.res)
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
    client = igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;

    if (client != L7_NULLPTR && client->pClientGroup != L7_NULLPTR)
    {
      /* Statistics at client level: point to clientGroup data */
      stat_client = &client->pClientGroup->stats_client;
    }
  }

  switch (field)
  {
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

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_INVALID:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->joins_received_invalid;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->joins_received_invalid;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->joins_received_invalid;
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_DROPPED:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->joins_received_dropped;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->joins_received_dropped;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->joins_received_dropped;
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

  case SNOOP_STAT_FIELD_LEAVES_RECEIVED_INVALID:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->leaves_received_invalid;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->leaves_received_invalid;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->leaves_received_invalid;
    break;

  case SNOOP_STAT_FIELD_LEAVES_RECEIVED_DROPPED:
    if (stat_port_g!=L7_NULLPTR)  statPortG=stat_port_g->leaves_received_dropped;
    if (stat_port  !=L7_NULLPTR)  statPort=stat_port->leaves_received_dropped;
    if (stat_client!=L7_NULLPTR)  statClient=stat_client->leaves_received_dropped;
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
    client = igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;   
    if (client != L7_NULLPTR && client->pClientGroup != L7_NULLPTR)
    {
      /* Statistics at client level */
      stat_client = &client->pClientGroup->stats_client;
    }
  }

  switch (field)
  {
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

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_INVALID:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_received_invalid = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_received_invalid = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_received_invalid = 0;
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_DROPPED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_received_dropped = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_received_dropped = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_received_dropped = 0;
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

  case SNOOP_STAT_FIELD_LEAVES_RECEIVED_INVALID:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->leaves_received_invalid = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->leaves_received_invalid = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->leaves_received_invalid = 0;
    break;

  case SNOOP_STAT_FIELD_LEAVES_RECEIVED_DROPPED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->leaves_received_dropped = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->leaves_received_dropped = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->leaves_received_dropped = 0;
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
      client = igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;
      if (client != L7_NULLPTR && client->pClientGroup != L7_NULLPTR)
      {
        /* Statistics at client level */
        stat_client = &client->pClientGroup->stats_client;
      }
    }
  }

  osapiSemaTake(ptin_igmp_stats_sem, L7_WAIT_FOREVER);

  switch (field)
  {
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

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_INVALID:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_received_invalid++;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_received_invalid++;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_received_invalid++;
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_DROPPED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_received_dropped++;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_received_dropped++;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_received_dropped++;
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

  case SNOOP_STAT_FIELD_LEAVES_RECEIVED_INVALID:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->leaves_received_invalid++;
    if (stat_port  !=L7_NULLPTR)  stat_port->leaves_received_invalid++;
    if (stat_client!=L7_NULLPTR)  stat_client->leaves_received_invalid++;
    break;

  case SNOOP_STAT_FIELD_LEAVES_RECEIVED_DROPPED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->leaves_received_dropped++;
    if (stat_port  !=L7_NULLPTR)  stat_port->leaves_received_dropped++;
    if (stat_client!=L7_NULLPTR)  stat_client->leaves_received_dropped++;
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
    client = igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client;
    if (client != L7_NULLPTR && client->pClientGroup != L7_NULLPTR)
    {
      /* Statistics at client level */
      stat_client = &client->pClientGroup->stats_client;
    }
  }

  osapiSemaTake(ptin_igmp_stats_sem, L7_WAIT_FOREVER);

  switch (field)
  {
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


/**
 * Open/close ports on the switch for the requested channel 
 *  
 * @param admin      : Admin (L7_ENABLE; L7_DISABLE)
 * @param serviceId  : Service ID
 * @param portId     : Port ID (intfNum)
 * @param groupAddr  : Group IP Address
 * @param sourceAddr : Source IP Address
 * @param isStatic   : Dynamic or static port (0-dynamic; 
 *                  1-static)
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_mgmd_port_sync(L7_uint8 admin, L7_uint32 serviceId, L7_uint32 portId, L7_uint32 ipv4GroupAddr, L7_uint32 ipv4SourceAddr, L7_uint8 isStatic)
{
  L7_inet_addr_t groupAddr;
  L7_inet_addr_t sourceAddr;
  L7_BOOL        isProtection = L7_TRUE;
  L7_RC_t        rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Received request to sync port");

  inetAddressSet(L7_AF_INET, &ipv4GroupAddr, &groupAddr);
  inetAddressSet(L7_AF_INET, &ipv4SourceAddr, &sourceAddr);


#if PTIN_BOARD_IS_MATRIX
  L7_uint32 slotId;
  L7_uint32 lagId;

  /* 
   * PortId is a slot in the matrix context. We need to convert it first, but only if this is the active matrix.
   * The backup matrix only receives sync requests from the active matrix. Hence, the ports are already converted.
   */
  if (ptin_fpga_mx_is_matrixactive() == 1)
  {
    slotId = portId;
    if (L7_SUCCESS != ptin_intf_slot2lagIdx(slotId, &lagId))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to get lag index from slot ID [slotId:%u]", slotId);
      return L7_FAILURE;
    }
    if (L7_SUCCESS != ptin_intf_lag2intIfNum(lagId, &portId))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to get intfnum from lag index [lagId:%u]", lagId);
      return L7_FAILURE;
    }
  }
#endif

  if (admin == L7_ENABLE)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Going to open port [intfNum:%u]", portId);    
    rc = snoopPortOpen(serviceId, portId, &groupAddr, &sourceAddr, isStatic, isProtection);    
    return rc;
  }
  else if (admin == L7_DISABLE)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Going to close port [intfNum:%u]", portId); 
    
    rc = snoopPortClose(serviceId, portId, &groupAddr, &sourceAddr, isProtection);    

    return rc;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unknown admin value %u", admin);
    return L7_FAILURE;
  }

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
    return(struct ptinIgmpClientDevice_s *) clientGroup->queue_clientDevices.head;

  /* Otherwise, return next value */
  return(struct ptinIgmpClientDevice_s *) pelem->next;
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
  clientIdx = clientInfo->deviceClientId;
  if (clientIdx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    return L7_NULLPTR;
  }

  /* Check if the provided client already exists */
  if (igmp_clientDevice_find(clientGroup, clientInfo) != L7_NULLPTR)
  {
    return L7_NULLPTR;
  }

  /* Set clientIdx in the client bitmap */
  BITMAP_BIT_SET(clientGroup->client_bmp_list, clientIdx, UINT32_BITSIZE);

  /* Client port */
  ptin_port = clientGroup->ptin_port;
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    return L7_NULLPTR;
  }

  /* Add client to the EVC struct */
  dl_queue_remove_head(&igmpDeviceClients.queue_free_clientDevices[PTIN_IGMP_CLIENT_PORT(ptin_port)], (dl_queue_elem_t**) &clientDevice);
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
  clientIdx = clientInfo->deviceClientId;
  if (clientIdx >= PTIN_IGMP_CLIENTIDX_MAX)
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
  BITMAP_BIT_CLR(clientGroup->client_bmp_list, clientIdx, UINT32_BITSIZE);

  /* Remove node from client devices queue */
  dl_queue_remove(&clientGroup->queue_clientDevices, (dl_queue_elem_t*) clientDevice);
  clientDevice->client = L7_NULLPTR;
  dl_queue_add_tail(&igmpDeviceClients.queue_free_clientDevices[PTIN_IGMP_CLIENT_PORT(ptin_port)], (dl_queue_elem_t*) clientDevice);

  return L7_SUCCESS;
}


/**
 * Pop new device client index 
 */
static L7_uint16 ptin_igmp_device_client_identifier_pop(L7_uint ptin_port, ptinIgmpGroupClientInfoData_t* clientGroup)
{
  struct ptinIgmpClientId_s *pClientIdx;

  /* Validate port */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    return(L7_uint16)-1;
  }

  /* Find a free client index */
  do
  {
    /* Extract one index */
    pClientIdx = L7_NULLPTR;

    dl_queue_remove_head(&queue_free_device_client_id[PTIN_IGMP_CLIENT_PORT(ptin_port)], (dl_queue_elem_t **) &pClientIdx);

    /* Check if there is free indexes */
    if (pClientIdx == L7_NULLPTR)
    {
      /* Not found: return -1 */
      return(L7_uint16)-1;
    }
  } while (pClientIdx->inUse);

  /* Mark index as being used */
  pClientIdx->inUse = L7_TRUE;

  /* Return client index */
  return pClientIdx->clientId;
}

/**
 * Pop new Group client identifier 
 */
static L7_uint16 ptin_igmp_group_client_identifier_pop(L7_uint ptin_port)
{
  struct ptinIgmpClientId_s *pClientIdx;

  /* Validate port */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    return(L7_uint16)-1;
  }

  /* Find a free client index */
  do
  {
    /* Extract one index */
    pClientIdx = L7_NULLPTR;

    dl_queue_remove_head(&queue_free_group_client_id[PTIN_IGMP_CLIENT_PORT(ptin_port)], (dl_queue_elem_t **) &pClientIdx);

    /* Check if there is free indexes */
    if (pClientIdx == L7_NULLPTR)
    {
      /* Not found: return -1 */
      return(L7_uint16)-1;
    }
  } while (pClientIdx->inUse);

  /* Mark index as being used */
  pClientIdx->inUse = L7_TRUE;

  /* Return client index */
  return pClientIdx->clientId;
}

/**
 * Push existing device client identifier 
 */
static void ptin_igmp_device_client_identifier_push(L7_uint ptin_port, L7_uint16 client_idx)
{
  struct ptinIgmpClientId_s *pClientIdx;


  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      client_idx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    return;
  }

  /* Check if free indexes pool is already full */
  if (queue_free_device_client_id[PTIN_IGMP_CLIENT_PORT(ptin_port)].n_elems >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    return;
  }

  /* Node to be added */
  pClientIdx = &pool_device_client_id[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx];

  /* Just to be sure it has the right value */
  pClientIdx->clientId = client_idx;

  /* Check if this entry is already free */
  if (!pClientIdx->inUse)
    return;

  /* Add index to free indexes queue */
  if (dl_queue_add_head(&queue_free_device_client_id[PTIN_IGMP_CLIENT_PORT(ptin_port)], (dl_queue_elem_t *) pClientIdx) != NOERR)
  {
    return;
  }

  /* Update in_use flag */
  pClientIdx->inUse = L7_FALSE;

}

/**
 * Push existing group client identifier 
 */
static void ptin_igmp_group_client_identifier_push(L7_uint ptin_port, L7_uint16 client_idx)
{
  struct ptinIgmpClientId_s *pClientIdx;


  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      client_idx >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    return;
  }

  /* Check if free indexes pool is already full */
  if (queue_free_group_client_id[PTIN_IGMP_CLIENT_PORT(ptin_port)].n_elems >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    return;
  }

  /* Node to be added */
  pClientIdx = &pool_group_client_id[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx];

  /* Just to be sure it has the right value */
  pClientIdx->clientId = client_idx;

  /* Check if this entry is already free */
  if (!pClientIdx->inUse)
    return;

  /* Add index to free indexes queue */
  if (dl_queue_add_head(&queue_free_group_client_id[PTIN_IGMP_CLIENT_PORT(ptin_port)], (dl_queue_elem_t *) pClientIdx) != NOERR)
  {
    return;
  }

  /* Update in_use flag */
  pClientIdx->inUse = L7_FALSE;
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
  return(igmpClients_unified.client_devices[client_idx].client!=L7_NULLPTR);
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
  if (igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client == L7_NULLPTR)
  {
    if (igmpDeviceClients.number_of_clients < PTIN_IGMP_CLIENTIDX_MAX)
      igmpDeviceClients.number_of_clients++;

    if (ptin_port<PTIN_SYSTEM_N_INTERF)
    {
      if (igmpDeviceClients.number_of_clients_per_intf[ptin_port] < PTIN_IGMP_CLIENTIDX_MAX )
        igmpDeviceClients.number_of_clients_per_intf[ptin_port]++;
    }
  }

  igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client = infoData;
}

/**
 * Get outer vlan from Client Id
 */
L7_RC_t igmp_intVlan_from_clientId_get(L7_uint ptin_port, L7_uint client_idx, L7_uint16 *outerVlan)
{
  if (igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client == L7_NULLPTR)
  {
    *outerVlan = (L7_uint16) -1;
    return L7_FAILURE;
  }
  *outerVlan=igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client->igmpClientDataKey.outerVlan;
  return L7_SUCCESS;
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
  if (igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client != L7_NULLPTR)
  {
    if (igmpDeviceClients.number_of_clients>0)
    {
      igmpDeviceClients.number_of_clients--;
    }

    if (ptin_port<PTIN_SYSTEM_N_INTERF)
    {
      if (igmpDeviceClients.number_of_clients_per_intf[ptin_port] > 0 )
      {
        igmpDeviceClients.number_of_clients_per_intf[ptin_port]--;
      }
    }
  }

  igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client = L7_NULLPTR;
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
    if (!igmpInstances[i].inUse)
    {
      printf("*** Igmp instance %02u not in use\r\n", i);
      continue;
    }

    printf("IGMP instance %02u   ", i);
    printf("\n");
    printf("   MC evcId = %4u   NNI VLAN = %u\r\n",igmpInstances[i].McastEvcId, igmpInstances[i].nni_ovid);
    printf("   UC evcId = %4u     #evc's = %u\r\n",igmpInstances[i].UcastEvcId, igmpInstances[i].n_evcs);
  }
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  printf("\r\nTotal number of QUATTRO-STACKED evcs: %u\r\n", igmp_quattro_stacked_evcs);
#endif
  printf("Clients are not associated to IGMP instances any more!!!\r\n");

  osapiSemaGive(ptin_igmp_clients_sem);

  fflush(stdout);
}

/**
 * Dumps EVC detailed info 
 * If evc_idx is invalid, all EVCs are dumped 
 * 
 * @param evc_idx 
 */
void ptin_igmp_group_clients_dump(void)
{
  L7_uint32                      i_client;
  L7_uint32                      child_clients;
  ptinIgmpClientDataKey_t        avl_key;
  ptinIgmpGroupClientInfoData_t *clientGroup;
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED  
  L7_int32                       packageIdIterator;
#endif

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  printf("List of Group clients (%u clients):\n",igmpGroupClients.number_of_clients);

  i_client = 0;

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  while ( ( clientGroup = (ptinIgmpGroupClientInfoData_t *)
            avlSearchLVL7(&igmpGroupClients.avlTree.igmpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &clientGroup->igmpClientDataKey, sizeof(ptinIgmpClientDataKey_t));

    /* Count number of child clients */
    child_clients = igmp_clientDevice_get_devices_number(clientGroup);

    printf(
#if (MC_CLIENT_INTERF_SUPPORTED)
          "ptin_port=%-2u "
#endif
          "groupClientId=%-2u (OnuId=%u) (#devices=%u) "
#if (MC_CLIENT_OUTERVLAN_SUPPORTED)
          "svlan=%-4u (intVlan=%-4u) "
#endif
#if (MC_CLIENT_INNERVLAN_SUPPORTED)
          "cvlan=%-4u "
#endif
#if (MC_CLIENT_IPADDR_SUPPORTED)
          "IP=%03u.%03u.%03u.%03u "
#endif
#if 0//(MC_CLIENT_MACADDR_SUPPORTED)
          "MAC=%02x:%02x:%02x:%02x:%02x:%02x "
#endif           
          "uni_vid=%4u+%-4u "
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
          "mask=0x%02x maxChannels=%hu  maxBw=%u channels=%hu  Bw=%u noOfPackages:%u "
#endif
          ,          
#if (MC_CLIENT_INTERF_SUPPORTED)
          clientGroup->igmpClientDataKey.ptin_port,
#endif
          clientGroup->groupClientId, clientGroup->onuId, child_clients,
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
#if 0 //(MC_CLIENT_MACADDR_SUPPORTED)
          clientGroup->igmpClientDataKey.macAddr[0],
          clientGroup->igmpClientDataKey.macAddr[1],
          clientGroup->igmpClientDataKey.macAddr[2],
          clientGroup->igmpClientDataKey.macAddr[3],
          clientGroup->igmpClientDataKey.macAddr[4],
          clientGroup->igmpClientDataKey.macAddr[5],
#endif           
          clientGroup->uni_ovid, clientGroup->uni_ivid           
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
          ,clientGroup->admissionControl.mask, 
          clientGroup->admissionControl.maxAllowedChannels,
          clientGroup->admissionControl.maxAllowedBandwidth,
          clientGroup->admissionControl.allocatedChannels,
          clientGroup->admissionControl.allocatedBandwidth
#endif
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
          ,clientGroup->number_of_packages
#endif
          );  
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED  
    if ( clientGroup->number_of_packages != 0 )
    {
      printf("packageBmp: 0x");
      for (packageIdIterator = PTIN_IGMP_PACKAGE_BITMAP_SIZE -1; packageIdIterator>=0; --packageIdIterator)
      {
        printf("%08X", clientGroup->package_bmp_list[packageIdIterator]);
      }
    }
#endif
    printf("\r\n");   

    i_client++;
  }

  printf("\nNoOfGroupClients:%u\n", i_client);
  osapiSemaGive(ptin_igmp_clients_sem);

  fflush(stdout);
}

/************IGMP Admission Control Feature****************************************************/ 
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT

/**
 * Reset All Admission Control  Values of GroupClients
 *  
 *  
 */
void ptin_igmp_admission_control_group_clients_reset_all(void)
{
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpGroupClientInfoData_t *clientGroup;

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  while ( ( clientGroup = (ptinIgmpGroupClientInfoData_t *)
            avlSearchLVL7(&igmpGroupClients.avlTree.igmpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &clientGroup->igmpClientDataKey, sizeof(ptinIgmpClientDataKey_t));

    clientGroup->admissionControl.mask = 
    clientGroup->admissionControl.maxAllowedChannels =
    clientGroup->admissionControl.maxAllowedBandwidth =
    clientGroup->admissionControl.allocatedChannels =
    clientGroup->admissionControl.allocatedBandwidth = 0;
  }
  osapiSemaGive(ptin_igmp_clients_sem);
}

/**
 * Reset Allocation  Admission Control GroupClients 
 *  
 *  
 */
void ptin_igmp_admission_control_group_clients_reset_allocation(void)
{
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpGroupClientInfoData_t *clientGroup;

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  while ( ( clientGroup = (ptinIgmpGroupClientInfoData_t *)
            avlSearchLVL7(&igmpGroupClients.avlTree.igmpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &clientGroup->igmpClientDataKey, sizeof(ptinIgmpClientDataKey_t));

    clientGroup->admissionControl.allocatedChannels =
    clientGroup->admissionControl.allocatedBandwidth = 0;
  }

  osapiSemaGive(ptin_igmp_clients_sem);
}

/**
 * @purpose Verify if this device client has any other device on
 *          the same group client
 * 
 * @param  ptin_port 
 * @param  clientId  
 * @param *clientBmpPtr 
 *  
 * @return L7_RC_t           : L7_SUCCESS/L7_FAILURE  
 *  
 */
extern L7_RC_t ptin_igmp_admission_control_verify_the_presence_of_other_clients(L7_uint32 ptin_port, L7_uint32 clientId, L7_uchar8 *clientBmpPtr)
{
  ptinIgmpDeviceClient_t        *client_device = L7_NULLPTR;
  ptinIgmpGroupClientInfoData_t *ptinIgmpClientGroupInfoData;

  /*Input Arguments Validation*/
  if (ptin_port >= PTIN_SYSTEM_N_INTERF || clientId >= PTIN_IGMP_CLIENTIDX_MAX || clientBmpPtr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid Input Arguments: ptin_port:%u clientId:%u clientBmpPtr:%p",ptin_port, clientId, clientBmpPtr);
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Arguments [ptin_port:%u clientId:%u]",ptin_port, clientId);

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);  
  if ( (ptinIgmpClientGroupInfoData = deviceClientId2groupClientPtr(ptin_port, clientId)) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to find any valid clientGroup [ptin_port:%u clientId:%u]",ptin_port, clientId);    
    osapiSemaGive(ptin_igmp_clients_sem);
    return L7_FAILURE;
  }

  while ( (client_device = igmp_clientDevice_next(ptinIgmpClientGroupInfoData, client_device)) != L7_NULLPTR)
  {
    if ( clientId == client_device->client->deviceClientId)
    {
      continue;
    }

    if ( PTIN_CLIENT_IS_MASKBITSET(clientBmpPtr, client_device->client->deviceClientId) == L7_TRUE)
    {
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "This groupclient has more than one device watching this stream [ptin_port:%u clientId:%u clientId2:%u]",ptin_port, clientId, client_device->client->deviceClientId);
      osapiSemaGive(ptin_igmp_clients_sem);
      return L7_ALREADY_CONFIGURED;
    }
  }

  osapiSemaGive(ptin_igmp_clients_sem);
  return L7_SUCCESS;
}

  #if PTIN_BOARD_IS_ACTIVETH
/**
 * @purpose Verify if this group client has any other group 
 *          clients on the same port
 * 
 * @param  ptin_port 
 * @param  clientId  
 * @param *clientBmpPtr 
 *  
 * @return L7_RC_t           : L7_SUCCESS/L7_FAILURE  
 *  
 */
extern L7_RC_t ptin_igmp_admission_control_verify_the_presence_of_other_groupclients(L7_uint32 ptin_port, L7_uint32 clientId, L7_uchar8 *clientBmpPtr)
{
  ptinIgmpGroupClientInfoData_t *ptinIgmpClientGroupInfoData1;
  ptinIgmpGroupClientInfoData_t *ptinIgmpClientGroupInfoData2;
  L7_uint16                      iterator;

  /*Input Arguments Validation*/
  if (ptin_port >= PTIN_SYSTEM_N_INTERF || clientId >= PTIN_IGMP_CLIENTIDX_MAX || clientBmpPtr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid Input Arguments: ptin_port:%u clientId:%u clientBmpPtr:%p",ptin_port, clientId, clientBmpPtr);
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Arguments [ptin_port:%u clientId:%u]",ptin_port, clientId);

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);  
  if ( (ptinIgmpClientGroupInfoData1 = deviceClientId2groupClientPtr(ptin_port, clientId)) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to find any valid clientGroup [ptin_port:%u clientId:%u]",ptin_port, clientId);    
    osapiSemaGive(ptin_igmp_clients_sem);
    return L7_FAILURE;
  }

  for (iterator = 0; iterator < PTIN_IGMP_CLIENTIDX_MAX; iterator++)
  {
    //Move forward 8 bits if this byte is 0 (no clients)
    if (! (PTIN_CLIENT_IS_MASKBYTESET(clientBmpPtr, iterator)))
    {
      clientId += PTIN_MGMD_CLIENT_MASK_UNIT -1; //Less one, because of the For cycle that increments also 1 unit.
      continue;
    }

    if (iterator == clientId)
      continue;

    if ( PTIN_CLIENT_IS_MASKBITSET(clientBmpPtr, iterator) == L7_TRUE)
    {
      if ( (ptinIgmpClientGroupInfoData2 = deviceClientId2groupClientPtr(ptin_port, iterator)) == L7_NULLPTR)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to find any valid clientGroup [ptin_port:%u clientId:%u]",ptin_port, iterator);    
        osapiSemaGive(ptin_igmp_clients_sem);
        return L7_FAILURE;
      }

      /*Is it this groupclient?*/
      if (ptinIgmpClientGroupInfoData1 == ptinIgmpClientGroupInfoData2)
        continue;

      /*Is it a group groupclient from another ONU/TA48GE_Port?*/
      if (ptinIgmpClientGroupInfoData1->onuId != ptinIgmpClientGroupInfoData2->onuId)
        continue;

      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "This ONU/TA48GE Port has more than one groupclient watching this stream [ptin_port:%u clientId:%u clientId2:%u]",ptin_port, clientId, iterator);
      osapiSemaGive(ptin_igmp_clients_sem);
      return L7_ALREADY_CONFIGURED;
    }
  }

  osapiSemaGive(ptin_igmp_clients_sem);
  return L7_SUCCESS;
}
  #endif

static void ptin_igmp_channel_bandwidth_cache_set(ptinIgmpChannelInfoData_t* ptinIgmpPairInfoData)
{
  /*Input Arguments Validation*/
  if (ptinIgmpPairInfoData == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid Input Arguments: ptinIgmpPairInfoData:%p",ptinIgmpPairInfoData);
    return;
  }

  ptinIgmpChannelBandwidthCache.inUse = L7_TRUE;
  ptinIgmpChannelBandwidthCache.channelBandwidth = ptinIgmpPairInfoData->channelBandwidth;

  ptinIgmpChannelBandwidthCache.evc_mc = ptinIgmpPairInfoData->channelDataKey.evc_mc;
  memcpy(&ptinIgmpChannelBandwidthCache.channel_group, &ptinIgmpPairInfoData->channelDataKey.channel_group, sizeof(L7_inet_addr_t));   
#if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
  memcpy(&ptinIgmpChannelBandwidthCache.channel_source, &ptinIgmpPairInfoData->channelDataKey.channel_source, sizeof(L7_inet_addr_t));   
#endif 

  if (ptin_debug_igmp_snooping)
  {
    char  groupAddrStr[IPV6_DISP_ADDR_LEN]={};
    char  sourceAddrStr[IPV6_DISP_ADDR_LEN]={};   
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Cache Channel Bandwidth [inUse:%s evc_mc:%u groupAddr:%s sourceAddr:%s bandwidth:%u kbps]",ptinIgmpChannelBandwidthCache.inUse?"Yes":"No", 
              ptinIgmpChannelBandwidthCache.evc_mc, inetAddrPrint(&ptinIgmpChannelBandwidthCache.channel_group,groupAddrStr), 
              inetAddrPrint(&ptinIgmpChannelBandwidthCache.channel_source,sourceAddrStr), ptinIgmpChannelBandwidthCache.channelBandwidth); 
  }
}

static void ptin_igmp_channel_bandwidth_cache_unset(ptinIgmpChannelDataKey_t* ptinIgmpPairDataKey)
{
  /*Input Arguments Validation*/
  if (ptinIgmpPairDataKey == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid Input Arguments: ptinIgmpPairDataKey:%p",ptinIgmpPairDataKey);
    return;
  }

  if ( ptinIgmpChannelBandwidthCache.inUse == L7_TRUE && ptinIgmpPairDataKey->evc_mc == ptinIgmpChannelBandwidthCache.evc_mc && L7_INET_ADDR_COMPARE(&ptinIgmpPairDataKey->channel_group, &ptinIgmpChannelBandwidthCache.channel_group) == 0
#if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
       && L7_INET_ADDR_COMPARE(&ptinIgmpPairDataKey->channel_source, &ptinIgmpChannelBandwidthCache.channel_source) == 0
#endif
     )
  {
    if (ptin_debug_igmp_snooping)
    {
      char  groupAddrStr[IPV6_DISP_ADDR_LEN]={};
      char  sourceAddrStr[IPV6_DISP_ADDR_LEN]={};   
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Uncache Channel Bandwidth [inUse:%s evc_mc:%u groupAddr:%s sourceAddr:%s bandwidth:%u kbps]",ptinIgmpChannelBandwidthCache.inUse?"Yes":"No",
                ptinIgmpChannelBandwidthCache.evc_mc, inetAddrPrint(&ptinIgmpChannelBandwidthCache.channel_group,groupAddrStr), 
                inetAddrPrint(&ptinIgmpChannelBandwidthCache.channel_source,sourceAddrStr), ptinIgmpChannelBandwidthCache.channelBandwidth); 
    }

    ptinIgmpChannelBandwidthCache.inUse = L7_FALSE;
  }
}

static ptinIgmpChannelBandwidthCache_t* ptin_igmp_channel_bandwidth_cache_get(void)
{
  if (ptin_debug_igmp_snooping)
  {
    char  groupAddrStr[IPV6_DISP_ADDR_LEN]={};
    char  sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Cached Channel Bandwidth [inUse:%s evc_mc:%u groupAddr:%s sourceAddr:%s bandwidth:%u kbps]",ptinIgmpChannelBandwidthCache.inUse?"Yes":"No",
              ptinIgmpChannelBandwidthCache.evc_mc, inetAddrPrint(&ptinIgmpChannelBandwidthCache.channel_group,groupAddrStr), 
              inetAddrPrint(&ptinIgmpChannelBandwidthCache.channel_source,sourceAddrStr), ptinIgmpChannelBandwidthCache.channelBandwidth); 
  }
  return(&ptinIgmpChannelBandwidthCache);  
}

/**
 * @purpose Set the Port Admission Control Parameters
 * 
 * @param ptin_port 
 * @param mask 
 * @param maxAllowedChannels 
 * @param maxAllowedBandwidth  
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_admission_control_port_set(L7_uint32 ptin_port, L7_uint8 mask, L7_uint16 maxAllowedChannels, L7_uint64 maxAllowedBandwidth)  
{
  /*Input Parameters Validation*/
  if (ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u mask:0x%x maxAllowedChannels:%u maxAllowedBandwidth:%ull]",ptin_port, mask, maxAllowedChannels, maxAllowedBandwidth);    
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input arguments [ptin_port:%u mask:0x%x maxAllowedChannels:%u maxAllowedBandwidth:%ull]",ptin_port, mask, maxAllowedChannels, maxAllowedBandwidth);

  if ( (mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS)
  {
    if (maxAllowedChannels == PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS_DISABLE) /*Disable this Parameter*/
    {
      igmpPortAdmissionControl[ptin_port].admissionControl.maxAllowedChannels = 0;
      igmpPortAdmissionControl[ptin_port].admissionControl.allocatedChannels = 0;
      igmpPortAdmissionControl[ptin_port].admissionControl.mask &= PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH;
    }
    else
    {
      igmpPortAdmissionControl[ptin_port].admissionControl.maxAllowedChannels = maxAllowedChannels;    
      igmpPortAdmissionControl[ptin_port].admissionControl.mask |= PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS;
    }
  }

  if ( (mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH)
  {
    if (maxAllowedBandwidth == PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_BPS_DISABLE) /*Disable this Parameter*/
    {
      igmpPortAdmissionControl[ptin_port].admissionControl.maxAllowedBandwidth = 0;
      igmpPortAdmissionControl[ptin_port].admissionControl.allocatedBandwidth = 0;
      igmpPortAdmissionControl[ptin_port].admissionControl.mask &= PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS;
    }
    else
    {
      igmpPortAdmissionControl[ptin_port].admissionControl.maxAllowedBandwidth = maxAllowedBandwidth / 1000; /*Convert from bps to kbps*/
      igmpPortAdmissionControl[ptin_port].admissionControl.mask |= PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH;
    }
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output arguments [ptin_port:%u mask:0x%x maxAllowedChannels:%u maxAllowedBandwidth:%ull]",ptin_port, igmpPortAdmissionControl[ptin_port].admissionControl.mask, igmpPortAdmissionControl[ptin_port].admissionControl.maxAllowedChannels, igmpPortAdmissionControl[ptin_port].admissionControl.maxAllowedBandwidth);

  return L7_SUCCESS;
}

/**
 * @purpose Reset Admission Control Port Allocation Parameters
 * 
 * @notes none 
 *  
 */
void ptin_igmp_admission_control_port_reset_allocation(void)  
{
  L7_uint32 ptin_port;

  for (ptin_port = 0; ptin_port < PTIN_SYSTEM_N_UPLINK_INTERF; ptin_port++)
  {
    igmpPortAdmissionControl[ptin_port].admissionControl.allocatedChannels =
    igmpPortAdmissionControl[ptin_port].admissionControl.allocatedBandwidth = 0;
  }
}

/**
 * @purpose Reset All Admission Control Port  
 *          Parameters
 * 
 * @notes none 
 *  
 */
void ptin_igmp_admission_control_port_reset_all(void)  
{
  memset(&igmpPortAdmissionControl, 0x00, sizeof(igmpPortAdmissionControl));  
}

/**
 * @purpose Dump the Port Admission Control Parameters
 * 
 * @notes none 
 *  
 */
void ptin_igmp_admission_control_port_dump_active(void)  
{
  L7_uint32 ptin_port;

  for (ptin_port = 0; ptin_port < PTIN_SYSTEM_N_UPLINK_INTERF; ptin_port++)
  {
    if (igmpPortAdmissionControl[ptin_port].admissionControl.mask != 0x00)
      printf("ptin_port:%u mask:0x%02x maxChannels:%hu maxBandwidth:%u (kbps) channels:%u bandwidth:%u (kbps)\n",
             ptin_port,
             igmpPortAdmissionControl[ptin_port].admissionControl.mask,
             igmpPortAdmissionControl[ptin_port].admissionControl.maxAllowedChannels,
             igmpPortAdmissionControl[ptin_port].admissionControl.maxAllowedBandwidth,
             igmpPortAdmissionControl[ptin_port].admissionControl.allocatedChannels,
             igmpPortAdmissionControl[ptin_port].admissionControl.allocatedBandwidth);
  }  

  /*It is Required for OLT1T0 and OLT1T1*/
  fflush(stdout);
}

/**
 * @purpose Dump the Port Admission Control Parameters
 * 
 * @notes none 
 *  
 */
void ptin_igmp_admission_control_port_dump(void)  
{
  L7_uint32 ptin_port;

  for (ptin_port = 0; ptin_port < PTIN_SYSTEM_N_UPLINK_INTERF; ptin_port++)
  {
    printf("ptin_port:%u mask:0x%02x maxChannels:%hu maxBandwidth:%u (kbps) channels:%u bandwidth:%u (kbps)\n",
           ptin_port,
           igmpPortAdmissionControl[ptin_port].admissionControl.mask,
           igmpPortAdmissionControl[ptin_port].admissionControl.maxAllowedChannels,
           igmpPortAdmissionControl[ptin_port].admissionControl.maxAllowedBandwidth,
           igmpPortAdmissionControl[ptin_port].admissionControl.allocatedChannels,
           igmpPortAdmissionControl[ptin_port].admissionControl.allocatedBandwidth);
  }  

  /*It is Required for OLT1T0 and OLT1T1*/
  fflush(stdout);
}

/**
 * @purpose Get the Port Admission Control Parameters
 * 
 * @param ptin_port 
 *  
 * @return ptinIgmpAdmissionControl_t
 *
 * @notes none 
 *  
 */
static ptinIgmpAdmissionControlPort_t* ptin_igmp_admission_control_port_get(L7_uint32 ptin_port)
{
  return(&igmpPortAdmissionControl[ptin_port]);
}

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/*To convert the External Service Id to an internal Service Id*/
static L7_uint8 ptin_igmp_admission_control_multicast_internal_id_set(L7_uint32 externalServiceId)
{
  L7_uint8 iterator;
  L7_uint8 firstFree = (L7_uint8) -1;

  if (externalServiceId == ((L7_uint32) - 1) || externalServiceId >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [externalServiceId:%u]", externalServiceId);    
    return((L7_uint8) -1);
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input arguments [externalServiceId:%u]", externalServiceId);

  for (iterator = 0; iterator<PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID; iterator++)
  {
    if (ptinIgmpAdmissionControlMulticastExternalServiceId[iterator] == externalServiceId)
    {
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output arguments [internalServiceId:%u]", iterator);
      return(iterator);
    }
    if ( (firstFree == (L7_uint8) -1) && ptinIgmpAdmissionControlMulticastExternalServiceId[iterator] == (L7_uint32) -1 )
    {
      firstFree = iterator;      
    }
  }

  if (firstFree != (L7_uint8) -1)
  {
    ptinIgmpAdmissionControlMulticastExternalServiceId[firstFree] = externalServiceId;
    ptinIgmpAdmissionControlMulticastInternalServiceId[externalServiceId] = firstFree;    
  }
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output arguments [internalServiceId:%u]", firstFree);

  return firstFree;
}

void ptin_igmp_admission_control_multicast_internal_id_unset(L7_uint32 externalServiceId)
{
  L7_uint8 iterator;

  if (externalServiceId == ((L7_uint32) - 1) || externalServiceId >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [externalServiceId:%u]", externalServiceId);    
    return;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input arguments [externalServiceId:%u]", externalServiceId);

  for (iterator = 0; iterator<PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID; iterator++)
  {
    if (ptinIgmpAdmissionControlMulticastExternalServiceId[iterator] == externalServiceId)
    {
      ptinIgmpAdmissionControlMulticastExternalServiceId[iterator] = (L7_uint32) -1;
      ptinIgmpAdmissionControlMulticastInternalServiceId[externalServiceId] = (L7_uint8) -1;    

      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output arguments [internalServiceId:%u]", iterator);
      return;          
    }
  }

  if (ptin_debug_igmp_snooping)
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Failed to find internal service id [externalServiceId:%u]", externalServiceId);

  return;
}

static L7_uint8 ptin_igmp_admission_control_multicast_internal_id_get(L7_uint32 externalServiceId)
{
  if (externalServiceId == ((L7_uint32) - 1) || externalServiceId >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [externalServiceId:%u]", externalServiceId);    
    return((L7_uint8) -1);
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input arguments [externalServiceId:%u]", externalServiceId);

  if ( ptinIgmpAdmissionControlMulticastInternalServiceId[externalServiceId] != (L7_uint8) -1)
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output arguments [internalServiceId:%u]", ptinIgmpAdmissionControlMulticastInternalServiceId[externalServiceId]);
    return ptinIgmpAdmissionControlMulticastInternalServiceId[externalServiceId];
  }
  else
  {
    return ptin_igmp_admission_control_multicast_internal_id_set(externalServiceId);
  }
}

void ptin_igmp_admission_multicast_external_service_id_dump(void)
{
  L7_uint8 internalServiceId;

  for (internalServiceId = 0; internalServiceId < PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID; internalServiceId++)
  {
    if (ptinIgmpAdmissionControlMulticastExternalServiceId[internalServiceId] != (L7_uint32) -1)
      printf("internalServiceId:%u serviceId:%u\n",internalServiceId, ptinIgmpAdmissionControlMulticastExternalServiceId[internalServiceId]);
  }

  /*It is Required for OLT1T0 and OLT1T1*/
  fflush(stdout);
}

void ptin_igmp_admission_multicast_external_service_id_dump_all(void)
{
  L7_uint8 internalServiceId;

  for (internalServiceId = 0; internalServiceId < PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID; internalServiceId++)
  {
    printf("internalServiceId:%u serviceId:%u\n",internalServiceId, ptinIgmpAdmissionControlMulticastExternalServiceId[internalServiceId]);
  }

  /*It is Required for OLT1T0 and OLT1T1*/
  fflush(stdout);
}

void ptin_igmp_admission_multicast_internal_service_id_dump(void)
{
  L7_uint32 externalServiceId;

  for (externalServiceId = 0; externalServiceId < PTIN_SYSTEM_N_EXTENDED_EVCS; externalServiceId++)
  {
    if (ptinIgmpAdmissionControlMulticastInternalServiceId[externalServiceId] != (L7_uint8) -1)
      printf("externalServiceId:%u internalServiceId:%u\n",externalServiceId, ptinIgmpAdmissionControlMulticastInternalServiceId[externalServiceId]);
  }

  /*It is Required for OLT1T0 and OLT1T1*/
  fflush(stdout);
}

void ptin_igmp_admission_multicast_internal_service_id_dump_all(void)
{
  L7_uint32 externalServiceId;

  for (externalServiceId = 0; externalServiceId < PTIN_SYSTEM_N_EXTENDED_EVCS; externalServiceId++)
  {
    printf("externalServiceId:%u internalServiceId:%u\n",externalServiceId, ptinIgmpAdmissionControlMulticastInternalServiceId[externalServiceId]);
  }

  /*It is Required for OLT1T0 and OLT1T1*/
  fflush(stdout);
}
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

/**
 * @purpose Set the Multicast Admission Control Parameters
 * 
 * @param igmpAdmissionControl 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_admission_control_multicast_service_set(ptin_igmp_admission_control_t *igmpAdmissionControl)  
{
  L7_uint8 internalServiceId = (L7_uint8) -1;

  /*Input Parameters Validation*/
  if (igmpAdmissionControl == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments igmpAdmissionControl:%p", igmpAdmissionControl);
    return L7_FAILURE;
  }

  if (igmpAdmissionControl->ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF || igmpAdmissionControl->onuId > PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF || igmpAdmissionControl->mask > PTIN_IGMP_ADMISSION_CONTROL_MASK_VALID ||
      ((L7_uint8) -1) == (internalServiceId = ptin_igmp_admission_control_multicast_internal_id_get(igmpAdmissionControl->serviceId)) || internalServiceId >= PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u onuId:%u serviceId:%u internalServiceId:%u mask:0x%x maxAllowedChannels:%u maxAllowedBandwidth:%ull]",igmpAdmissionControl->ptin_port, igmpAdmissionControl->onuId, igmpAdmissionControl->serviceId, internalServiceId, igmpAdmissionControl->mask, igmpAdmissionControl->maxAllowedChannels, igmpAdmissionControl->maxAllowedBandwidth);    
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Arguments [ptin_port:%u onuId:%u serviceId:%u internalServiceId:%u mask:0x%x maxAllowedChannels:%u maxAllowedBandwidth:%ull]",igmpAdmissionControl->ptin_port, igmpAdmissionControl->onuId, igmpAdmissionControl->serviceId, internalServiceId, igmpAdmissionControl->mask, igmpAdmissionControl->maxAllowedChannels, igmpAdmissionControl->maxAllowedBandwidth);

  if ( (igmpAdmissionControl->mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS)
  {
    if (igmpAdmissionControl->maxAllowedChannels == PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS_DISABLE) /*Disable this Parameter*/
    {
      igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.maxAllowedChannels = 0;
      igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.allocatedChannels = 0;     
      igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.mask &= PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH;      
    }
    else
    {
      igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.maxAllowedChannels = igmpAdmissionControl->maxAllowedChannels;    
      igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.mask |= PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS;
    }
  }

  if ( (igmpAdmissionControl->mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH)
  {
    if (igmpAdmissionControl->maxAllowedBandwidth == PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_BPS_DISABLE) /*Disable this Parameter*/
    {
      igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.maxAllowedBandwidth = 0;
      igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.allocatedBandwidth = 0;
      igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.mask &= PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS;       
    }
    else
    {
      igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.maxAllowedBandwidth = igmpAdmissionControl->maxAllowedBandwidth / 1000; /*Convert from bps to kbps*/
      igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.mask |= PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH;
    }
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Arguments [mask:0x%x maxAllowedChannels:%du maxAllowedBandwidth:%u allocatedChannels:%u allocatedBandwidth:%u]",
              igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.mask,
              igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.maxAllowedChannels,
              igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.maxAllowedBandwidth,
              igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.allocatedChannels,
              igmpMulticastAdmissionControl[igmpAdmissionControl->ptin_port][igmpAdmissionControl->onuId][internalServiceId].admissionControl.allocatedBandwidth );
  return L7_SUCCESS;
}

/**
 * @purpose Get the Multicast Service  Admission Control 
 *          Parameters
 * 
 * @param ptin_port 
 * @param serviceId 
 * @param onuId 
 *  
 * @return ptinIgmpAdmissionControl_t
 *
 * @notes none 
 *  
 */
static igmpMulticastAdmissionControl_t* ptin_igmp_admission_control_multicast_service_get(L7_uint32 ptin_port, L7_uint32 serviceId, L7_uint8 onuId)
{
  L7_uint8 internalServiceId = (L7_uint8) -1;

  if (ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF || onuId >= PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF || serviceId >= PTIN_SYSTEM_N_EXTENDED_EVCS ||
      ((L7_uint8) -1) == (internalServiceId = ptin_igmp_admission_control_multicast_internal_id_get(serviceId)) || internalServiceId >= PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u onuId:%u serviceId:%u internalServiceId:%u]", ptin_port, onuId, serviceId, internalServiceId);    
    return L7_NULLPTR;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input arguments [ptin_port:%u onuId:%u serviceId:%u internalServiceId:%u]", ptin_port, onuId, serviceId, internalServiceId);

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Arguments [mask:0x%x maxAllowedChannels:%u maxAllowedBandwidth:%u (kbps) allocatedChannels:%u allocatedBandwidth:%u (kbps)]",
              igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.mask,
              igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.maxAllowedChannels,
              igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.maxAllowedBandwidth,
              igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.allocatedChannels,
              igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.allocatedBandwidth );

  return(&(igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId]));
}

/**
 * @purpose Reset All the Admission Control Parameters of 
 *          Multicast Service Parameters
 * 
 * @notes none 
 *  
 */
void ptin_igmp_admission_control_multicast_service_reset_all(void)  
{
  memset(igmpMulticastAdmissionControl, 0x00, sizeof(igmpMulticastAdmissionControl));
}

/**
 * @purpose Reset the Allocation Admission Control Parameters of
 *          Multicast Service
 *          Parameters
 * 
 * @notes none 
 *  
 */
void ptin_igmp_admission_control_multicast_service_reset_allocation(void)  
{
  L7_uint32 ptin_port;
  L7_uint32 onuId;
  L7_uint8 internalServiceId;

  for (ptin_port = 0; ptin_port < PTIN_SYSTEM_N_UPLINK_INTERF; ptin_port++)
  {
    for (onuId = 0; onuId < PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF; onuId++)
    {
      for (internalServiceId = 0; internalServiceId < PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID; internalServiceId++)
      {
        igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.allocatedChannels =
        igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.allocatedBandwidth = 0;
      }
    }
  }  
}

/**
 * @purpose Dump the Active Multicast Service Admission Control 
 *          Parameters
 * 
 * @notes none 
 *  
 */
void ptin_igmp_admission_control_multicast_service_dump_active(void)  
{
  L7_uint32 ptin_port;
  L7_uint32 onuId;
  L7_uint8 internalServiceId;

  for (ptin_port = 0; ptin_port < PTIN_SYSTEM_N_UPLINK_INTERF; ptin_port++)
  {
    for (onuId = 0; onuId < PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF; onuId++)
    {
      for (internalServiceId = 0; internalServiceId < PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID; internalServiceId++)
      {
        if ( ptinIgmpAdmissionControlMulticastExternalServiceId[internalServiceId] != (L7_uint32) -1 && igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.mask != 0x00)
        {
          printf("ptin_port:%u onuId:%u serviceId:%u (internalServiceId:%u) mask:0x%02x maxChannels:%hu maxBandwidth:%u (kbps) channels:%u bandwidth:%u (kbps)\n",
                 ptin_port, onuId, ptinIgmpAdmissionControlMulticastExternalServiceId[internalServiceId], internalServiceId,
                 igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.mask,
                 igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.maxAllowedChannels,
                 igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.maxAllowedBandwidth,
                 igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.allocatedChannels,
                 igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.allocatedBandwidth);
        }
      }
    }
  }

  /*It is Required for OLT1T0 and OLT1T1*/
  fflush(stdout);  
}

/**
 * @purpose Dump the Active Multicast Service Admission Control 
 *          Parameters
 * 
 * @notes none 
 *  
 */
void ptin_igmp_admission_control_multicast_service_dump(void)  
{
  L7_uint32 ptin_port;
  L7_uint32 onuId;
  L7_uint8 internalServiceId;

  for (ptin_port = 0; ptin_port < PTIN_SYSTEM_N_UPLINK_INTERF; ptin_port++)
  {
    for (onuId = 0; onuId < PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF; onuId++)
    {
      for (internalServiceId = 0; internalServiceId < PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID; internalServiceId++)
      {
        if ( ptinIgmpAdmissionControlMulticastExternalServiceId[internalServiceId] != (L7_uint32) -1 )
        {
          printf("ptin_port:%u onuId:%u serviceId:%u (internalServiceId:%u) mask:0x%02x maxChannels:%hu maxBandwidth:%u (kbps) channels:%u bandwidth:%u (kbps)\n",
                 ptin_port, onuId, ptinIgmpAdmissionControlMulticastExternalServiceId[internalServiceId], internalServiceId,
                 igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.mask,
                 igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.maxAllowedChannels,
                 igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.maxAllowedBandwidth,
                 igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.allocatedChannels,
                 igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.allocatedBandwidth);
        }
      }
    }
  } 

  /*It is Required for OLT1T0 and OLT1T1*/
  fflush(stdout); 
}

/**
 * @purpose Dump All the Multicast Service Admission Control 
 *          Parameters
 * 
 * @notes none 
 *  
 */
void ptin_igmp_admission_control_multicast_service_dump_all(void)  
{
  L7_uint32 ptin_port;
  L7_uint32 onuId;
  L7_uint8 internalServiceId;

  for (ptin_port = 0; ptin_port < PTIN_SYSTEM_N_UPLINK_INTERF; ptin_port++)
  {
    for (onuId = 0; onuId < PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF; onuId++)
    {
      for (internalServiceId = 0; internalServiceId < PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID; internalServiceId++)
      {
        printf("ptin_port:%u onuId:%u serviceId:%u (internalServiceId:%u) mask:0x%02x maxChannels:%hu maxdBandwidth:%u (kbps) channels:%u bandwidth:%u (kbps)\n",
               ptin_port, onuId, ptinIgmpAdmissionControlMulticastExternalServiceId[internalServiceId], internalServiceId,
               igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.mask,
               igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.maxAllowedChannels,
               igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.maxAllowedBandwidth,
               igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.allocatedChannels,
               igmpMulticastAdmissionControl[ptin_port][onuId][internalServiceId].admissionControl.allocatedBandwidth);
      }
    }
  }  
}

/**
 * @purpose Get the bandwidth requested by a given 
 * channel
 *  
 * @param evc_mc:   
 * @param channel_group:  
 * @param channel_source:  
 * @param channelBandwidth: 
 *  
 * @return SUCCESS/FAILURE 
 *
 * @notes Channel bandwidth is given in kbps. If the channel 
 *        does not exist or a null pointer is given, FAILURE is
 *        returned.
 *  
 */
L7_RC_t ptin_igmp_channel_bandwidth_get(L7_uint32 evc_mc, L7_inet_addr_t* channel_group, L7_inet_addr_t* channel_source, L7_uint32 *channel_bandwidth)
{
  ptinIgmpChannelInfoData_t*            ptinIgmpPairInfoData = L7_NULLPTR;
  ptinIgmpChannelBandwidthCache_t*      ptinIgmpChannelBandwidthCachePtr;  

//We currently do not support any of this modes
#if ( IGMPASSOC_CHANNEL_UC_EVC_ISOLATION )
#error "Parameter Currently Not Supported!"
#endif

  /*  Argument validation */
  if (channel_group == L7_NULLPTR || channel_source == L7_NULLPTR || channel_bandwidth == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid input arguments group:%p channel_source:%p channel_bandwidth:%p", channel_group, channel_source, channel_bandwidth);
    return L7_FAILURE;
  }

  ptinIgmpChannelBandwidthCachePtr = ptin_igmp_channel_bandwidth_cache_get();

  if (ptinIgmpChannelBandwidthCachePtr->inUse == L7_TRUE)
  {
    if (evc_mc == ptinIgmpChannelBandwidthCachePtr->evc_mc && L7_INET_ADDR_COMPARE(channel_group, &ptinIgmpChannelBandwidthCachePtr->channel_group) == 0  
#if ( IGMPASSOC_CHANNEL_SOURCE_SUPPORTED )
        && L7_INET_ADDR_COMPARE(channel_source, &ptinIgmpChannelBandwidthCachePtr->channel_source) == 0 
#endif
       )
    {
      *channel_bandwidth = ptinIgmpChannelBandwidthCachePtr->channelBandwidth;
      return L7_SUCCESS;
    }
  }

  /* Check if this key does exist */
  if ( ptin_igmp_channel_get(evc_mc, channel_group, channel_source, &ptinIgmpPairInfoData) != L7_SUCCESS ||  ptinIgmpPairInfoData == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Group 0x%08x / Source does 0x%08x not exist.",
            channel_group->addr.ipv4.s_addr, channel_source->addr.ipv4.s_addr);   
    return L7_FAILURE;
  }

  /*Set Channel Bandwidth Cache*/
  ptin_igmp_channel_bandwidth_cache_set(ptinIgmpPairInfoData);

  *channel_bandwidth = ptinIgmpPairInfoData->channelBandwidth;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Group 0x%08x / Source 0x%08x requires  %u kbit/s of available bandwidth", channel_group->addr.ipv4.s_addr, channel_source->addr.ipv4.s_addr, *channel_bandwidth);

  return L7_SUCCESS;
}

/**
 * @purpose Check Multicast Resources
 * 
 * @param admissionControlPtr 
 * @param channelBandwidth 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
static RC_t ptin_igmp_admission_control_available(ptinIgmpAdmissionControl_t* admissionControlPtr, L7_uint32 channelBandwidth)
{
  L7_uint8                         globalBandwidthControl = ptin_igmp_proxy_bandwidth_control_get();
  L7_uint8                         globalChannelsControl = ptin_igmp_proxy_channels_control_get();
  L7_uint8                         globalAdmissionControl = ptin_igmp_proxy_admission_control_get();
  L7_uint8                         globalAdmissionControlMask = (globalBandwidthControl << 1) | globalChannelsControl;

  if (admissionControlPtr == L7_NULLPTR || channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_KBPS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid Input Parameters:[admissionControlPtr:%p channelBandwidth:%u]", admissionControlPtr, channelBandwidth);
    return L7_FAILURE;
  }

  if ( globalAdmissionControl == L7_FALSE )
  {
    //Admission Control Disabled
    if (ptin_debug_igmp_snooping)
      LOG_INFO(LOG_CTX_PTIN_IGMP, "Global Admission Control Feature is Disabled");
    return L7_SUCCESS;
  }

  switch (globalAdmissionControlMask)
  {
  case (PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS | PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH):          
    switch (admissionControlPtr->mask)
    {
    case (PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS | PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH):
      if ( (admissionControlPtr->allocatedChannels + 1 > admissionControlPtr->maxAllowedChannels) ||
           admissionControlPtr->allocatedBandwidth + channelBandwidth > admissionControlPtr->maxAllowedBandwidth)
      {
        //Not Enough Resources
        if (ptin_debug_igmp_snooping)
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Not have enough resources to perform this operation"
                     "[allocatedChannels:%u maxChannels:%u | allocatedBandwidth:%u maxAllowedBandwidth:%u]",                     
                     admissionControlPtr->allocatedChannels, admissionControlPtr->maxAllowedChannels,
                     admissionControlPtr->allocatedBandwidth, admissionControlPtr->maxAllowedBandwidth);
        return L7_FAILURE;
      }

      //Enough Resources
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Enough resources to perform this operation"
                  "[allocatedChannels:%u maxChannels:%u | allocatedBandwidth:%u maxAllowedBandwidth:%u]",
                  admissionControlPtr->allocatedChannels, admissionControlPtr->maxAllowedChannels,
                  admissionControlPtr->allocatedBandwidth, admissionControlPtr->maxAllowedBandwidth);

//          ++admissionControlPtr->allocatedChannels;
//          admissionControlPtr->allocatedBandwidth += channelBandwidth;

      return L7_SUCCESS;

      break;
    case PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS:           
      if (admissionControlPtr->allocatedChannels + 1 > admissionControlPtr->maxAllowedChannels)
      {
        //Not Enough Resources
        if (ptin_debug_igmp_snooping)
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Not have enough resources to perform this operation [allocatedChannels:%u maxChannels:%u]",                
                     admissionControlPtr->allocatedChannels, admissionControlPtr->maxAllowedChannels);
        return L7_FAILURE;
      }

      //Enough Resources
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Enough resources to perform this operation"
                  "[allocatedChannels:%u maxChannels:%u]",            
                  admissionControlPtr->allocatedChannels, admissionControlPtr->maxAllowedChannels);

//          ++admissionControlPtr->allocatedChannels;

      return L7_SUCCESS;

      break;
    case PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH:
      if (admissionControlPtr->allocatedBandwidth + channelBandwidth > admissionControlPtr->maxAllowedBandwidth)
      {
        //Not Enough Resources
        if (ptin_debug_igmp_snooping)
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Not enough resources to perform this operation [allocatedBandwidth:%u maxAllowedBandwidth:%u]",                     
                     admissionControlPtr->allocatedBandwidth, admissionControlPtr->maxAllowedBandwidth);
        return L7_FAILURE;
      }

      //Enough Resources
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Enough resources to perform this operation"
                  "[allocatedBandwidth:%u maxAllowedBandwidth:%u]",                    
                  admissionControlPtr->allocatedBandwidth, admissionControlPtr->maxAllowedBandwidth);

//          admissionControlPtr->allocatedBandwidth += channelBandwidth;

      return L7_SUCCESS;

      break;
    default:
      //Admission Control is Disabled for this Client
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "The admission control feature is disabled");
      return L7_SUCCESS;              
    }
    break;                    
  case PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS: 
    if ( (admissionControlPtr->mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS)
    {
      if (admissionControlPtr->allocatedChannels + 1 > admissionControlPtr->maxAllowedChannels)
      {
        //Not Enough Resources
        if (ptin_debug_igmp_snooping)
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Not have enough resources to perform this operation [allocatedChannels:%u maxChannels:%u]",                     
                     admissionControlPtr->allocatedChannels, admissionControlPtr->maxAllowedChannels);
        return L7_FAILURE;
      }

      //Enough Resources
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Enough resources to perform this operation"
                  "[allocatedChannels:%u maxChannels:%u]",                  
                  admissionControlPtr->allocatedChannels, admissionControlPtr->maxAllowedChannels);

//          ++admissionControlPtr->allocatedChannels;
    }
    else
    {
      //Admission Control is Disabled for this Client
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "The admission control feature is disabled");
    }
    return L7_SUCCESS; 
    break;          
  case PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH:  
    if ( (admissionControlPtr->mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH)
    {
      if (admissionControlPtr->allocatedBandwidth + channelBandwidth > admissionControlPtr->maxAllowedBandwidth)
      {
        //Not Enough Resources
        if (ptin_debug_igmp_snooping)
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Not have enough resources to perform this operation [allocatedBandwidth:%u maxAllowedBandwidth:%u]",                     
                     admissionControlPtr->allocatedBandwidth, admissionControlPtr->maxAllowedBandwidth);
        return L7_FAILURE;
      }

      //Enough Resources
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Enough resources to perform this operation"
                  "[allocatedBandwidth:%u maxAllowedBandwidth:%u]",                                         
                  admissionControlPtr->allocatedBandwidth, admissionControlPtr->maxAllowedBandwidth);

//          admissionControlPtr->allocatedBandwidth += channelBandwidth;
    }
    else
    {
      //Admission Control is Disabled for this Client
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "The admission control feature is disabled");
    }

    return L7_SUCCESS;                

    break;          
  default:
    {
      //Admission Control is Disabled for this Client
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "The admission control feature is disabled");
      return L7_SUCCESS;     
    }
  }

  //Not Enough Resources
  LOG_ERR(LOG_CTX_PTIN_IGMP, "Ooops Something went...wrong!");
  return L7_FAILURE;   
}

/**
 * @purpose Allocate Multicast Resources
 * 
 * @param admissionControlPtr 
 * @param channelBandwidth 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
static RC_t ptin_igmp_admission_control_allocate(ptinIgmpAdmissionControl_t* admissionControlPtr, L7_uint32 channelBandwidth)
{
  L7_uint8                         globalBandwidthControl = ptin_igmp_proxy_bandwidth_control_get();
  L7_uint8                         globalChannelsControl = ptin_igmp_proxy_channels_control_get();
  L7_uint8                         globalAdmissionControl = ptin_igmp_proxy_admission_control_get();
  L7_uint8                         globalAdmissionControlMask = (globalBandwidthControl << 1) | globalChannelsControl;

  if (admissionControlPtr == L7_NULLPTR || channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_KBPS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid Input Parameters:[admissionControlPtr:%p channelBandwidth:%u]", admissionControlPtr, channelBandwidth);
    return L7_FAILURE;
  }

  if ( globalAdmissionControl == L7_FALSE )
  {
    //Admission Control Disabled
    if (ptin_debug_igmp_snooping)
      LOG_INFO(LOG_CTX_PTIN_IGMP, "Global Admission Control Feature is Disabled");
    return L7_SUCCESS;
  }

  switch (globalAdmissionControlMask)
  {
  case (PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS | PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH):          
    switch (admissionControlPtr->mask)
    {
    case (PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS | PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH):
      if ( (admissionControlPtr->allocatedChannels + 1 > admissionControlPtr->maxAllowedChannels) ||
           admissionControlPtr->allocatedBandwidth + channelBandwidth > admissionControlPtr->maxAllowedBandwidth)
      {
        //Not Enough Resources
        if (ptin_debug_igmp_snooping)
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Not have enough resources to perform this operation"
                     "[allocatedChannels:%u maxChannels:%u | allocatedBandwidth:%u maxAllowedBandwidth:%u]",                     
                     admissionControlPtr->allocatedChannels, admissionControlPtr->maxAllowedChannels,
                     admissionControlPtr->allocatedBandwidth, admissionControlPtr->maxAllowedBandwidth);
        return L7_FAILURE;
      }

      //Enough Resources
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Enough resources to perform this operation"
                  "[allocatedChannels:%u maxChannels:%u | allocatedBandwidth:%u maxAllowedBandwidth:%u]",
                  admissionControlPtr->allocatedChannels, admissionControlPtr->maxAllowedChannels,
                  admissionControlPtr->allocatedBandwidth, admissionControlPtr->maxAllowedBandwidth);

      ++admissionControlPtr->allocatedChannels;
      admissionControlPtr->allocatedBandwidth += channelBandwidth;

      return L7_SUCCESS;

      break;
    case PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS:           
      if (admissionControlPtr->allocatedChannels + 1 > admissionControlPtr->maxAllowedChannels)
      {
        //Not Enough Resources
        if (ptin_debug_igmp_snooping)
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Not have enough resources to perform this operation [allocatedChannels:%u maxChannels:%u]",                     
                     admissionControlPtr->allocatedChannels, admissionControlPtr->maxAllowedChannels);
        return L7_FAILURE;
      }

      //Enough Resources
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Enough resources to perform this operation"
                  "[allocatedChannels:%u maxChannels:%u]",                  
                  admissionControlPtr->allocatedChannels, admissionControlPtr->maxAllowedChannels);

      ++admissionControlPtr->allocatedChannels;

      return L7_SUCCESS;

      break;
    case PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH:
      if (admissionControlPtr->allocatedBandwidth + channelBandwidth > admissionControlPtr->maxAllowedBandwidth)
      {
        //Not Enough Resources
        if (ptin_debug_igmp_snooping)
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Not enough resources to perform this operation [allocatedBandwidth:%u maxAllowedBandwidth:%u]",                     
                     admissionControlPtr->allocatedBandwidth, admissionControlPtr->maxAllowedBandwidth);
        return L7_FAILURE;
      }

      //Enough Resources
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Enough resources to perform this operation"
                  "[allocatedBandwidth:%u maxAllowedBandwidth:%u]",                    
                  admissionControlPtr->allocatedBandwidth, admissionControlPtr->maxAllowedBandwidth);

      admissionControlPtr->allocatedBandwidth += channelBandwidth;

      return L7_SUCCESS;

      break;
    default:
      //Admission Control is Disabled for this Client
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "The admission control feature is disabled");
      return L7_SUCCESS;              
    }
    break;                    
  case PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS: 
    if ( (admissionControlPtr->mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS)
    {
      if (admissionControlPtr->allocatedChannels + 1 > admissionControlPtr->maxAllowedChannels)
      {
        //Not Enough Resources
        if (ptin_debug_igmp_snooping)
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Not have enough resources to perform this operation [allocatedChannels:%u maxChannels:%u]",    
                     admissionControlPtr->allocatedChannels, admissionControlPtr->maxAllowedChannels);
        return L7_FAILURE;
      }

      //Enough Resources
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Enough resources to perform this operation"
                  "[allocatedChannels:%u maxChannels:%u]",                  
                  admissionControlPtr->allocatedChannels, admissionControlPtr->maxAllowedChannels);

      ++admissionControlPtr->allocatedChannels;
    }
    else
    {
      //Admission Control is Disabled for this Client
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "The admission control feature is disabled");
    }
    return L7_SUCCESS; 
    break;          
  case PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH:  
    if ( (admissionControlPtr->mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH)
    {
      if (admissionControlPtr->allocatedBandwidth + channelBandwidth > admissionControlPtr->maxAllowedBandwidth)
      {
        //Not Enough Resources
        if (ptin_debug_igmp_snooping)
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Not have enough resources to perform this operation [allocatedBandwidth:%u maxAllowedBandwidth:%u]",                     
                     admissionControlPtr->allocatedBandwidth, admissionControlPtr->maxAllowedBandwidth);
        return L7_FAILURE;
      }

      //Enough Resources
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Enough resources to perform this operation"
                  "[allocatedBandwidth:%u maxAllowedBandwidth:%u]",                                         
                  admissionControlPtr->allocatedBandwidth, admissionControlPtr->maxAllowedBandwidth);

      admissionControlPtr->allocatedBandwidth += channelBandwidth;
    }
    else
    {
      //Admission Control is Disabled for this Client
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "The admission control feature is disabled");
    }

    return L7_SUCCESS;                

    break;          
  default:
    {
      //Admission Control is Disabled for this Client
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "The admission control feature is disabled");
      return L7_SUCCESS;     
    }
  }

  //Not Enough Resources
  LOG_ERR(LOG_CTX_PTIN_IGMP, "Ooops Something went...wrong!");
  return L7_FAILURE;   
}

/**
 * @purpose Release Multicast Resources
 * 
 * @param admissionControlPtr 
 * @param channelBandwidth 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
static RC_t ptin_igmp_admission_control_release(ptinIgmpAdmissionControl_t* admissionControlPtr, L7_uint32 channelBandwidth)
{
  L7_uint8                         globalBandwidthControl = ptin_igmp_proxy_bandwidth_control_get();
  L7_uint8                         globalChannelsControl = ptin_igmp_proxy_channels_control_get();
  L7_uint8                         globalAdmissionControl = ptin_igmp_proxy_admission_control_get();
  L7_uint8                         globalAdmissionControlMask = (globalBandwidthControl << 1) | globalChannelsControl;

  if (admissionControlPtr == L7_NULLPTR || channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_KBPS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid Input Parameters:[admissionControlPtr:%p channelBandwidth:%u]", admissionControlPtr, channelBandwidth);
    return L7_FAILURE;
  }

  if ( globalAdmissionControl == L7_FALSE )
  {
    //Admission Control Disabled
    if (ptin_debug_igmp_snooping)
      LOG_INFO(LOG_CTX_PTIN_IGMP, "Global Admission Control Feature is Disabled");
    return L7_SUCCESS;
  }

  switch (globalAdmissionControlMask)
  {
  case (PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS | PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH):          
    switch (admissionControlPtr->mask)
    {
    case (PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS | PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH):
      if (admissionControlPtr->allocatedChannels > 0)
      {
        --admissionControlPtr->allocatedChannels;
      }
      else
      {
        LOG_NOTICE(LOG_CTX_PTIN_IGMP, "[allocatedChannels:%u maxChannels:%u]",                     
                   admissionControlPtr->allocatedChannels, admissionControlPtr->maxAllowedChannels);
      }

      if (admissionControlPtr->allocatedBandwidth - channelBandwidth <= admissionControlPtr->allocatedBandwidth)
      {
        admissionControlPtr->allocatedBandwidth -= channelBandwidth;
      }
      else
      {
        if (ptin_debug_igmp_snooping)
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "[allocatedBandwidth:%u maxAllowedBandwidth:%u]",                     
                     admissionControlPtr->allocatedBandwidth, admissionControlPtr->maxAllowedBandwidth);
        admissionControlPtr->allocatedBandwidth = 0;
      }      
      return L7_SUCCESS;
    case PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS:           
      if (admissionControlPtr->allocatedChannels > 0)
      {
        --admissionControlPtr->allocatedChannels;
      }
      return L7_SUCCESS;      
    case PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH:
      if (admissionControlPtr->allocatedBandwidth - channelBandwidth <= admissionControlPtr->allocatedBandwidth)
      {
        admissionControlPtr->allocatedBandwidth -= channelBandwidth;
      }
      else
      {
        admissionControlPtr->allocatedBandwidth = 0;
      }      
      return L7_SUCCESS;      
    default:
      //Admission Control is Disabled for this Client
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "The admission control feature is disabled");
      return L7_SUCCESS;              
    }
    break;                    
  case PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS: 
    if ( (admissionControlPtr->mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS)
    {
      if (admissionControlPtr->allocatedChannels > 0)
      {
        --admissionControlPtr->allocatedChannels;
      }
      else
      {
        LOG_NOTICE(LOG_CTX_PTIN_IGMP, "[allocatedChannels:%u maxChannels:%u]",                     
                   admissionControlPtr->allocatedChannels, admissionControlPtr->maxAllowedChannels);
      }
    }
    else
    {
      //Admission Control is Disabled for this Client
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "The admission control feature is disabled");
    }
    return L7_SUCCESS; 
    break;          
  case PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH:  
    if ( (admissionControlPtr->mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH)
    {
      if (admissionControlPtr->allocatedBandwidth - channelBandwidth <= admissionControlPtr->allocatedBandwidth)
      {
        admissionControlPtr->allocatedBandwidth -= channelBandwidth;
      }
      else
      {
        if (ptin_debug_igmp_snooping)
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "[allocatedBandwidth:%u maxAllowedBandwidth:%u]",                     
                     admissionControlPtr->allocatedBandwidth, admissionControlPtr->maxAllowedBandwidth);
        admissionControlPtr->allocatedBandwidth = 0;
      }      
    }
    else
    {
      //Admission Control is Disabled for this Client
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "The admission control feature is disabled");
    }

    return L7_SUCCESS;                

    break;          
  default:
    {
      //Admission Control is Disabled for this Client
      if (ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "The admission control feature is disabled");
      return L7_SUCCESS;     
    }
  }

  //Not Enough Resources
  LOG_ERR(LOG_CTX_PTIN_IGMP, "Ooops Something went...wrong!");
  return L7_FAILURE;   
}

/**
 * @purpose Convert Client Id to an ONU Id
 * 
 * @param ptin_port 
 * @param clientId 
 *  
 * @return onuId
 *
 * @notes none 
 *  
 */
L7_uint8 ptin_igmp_client_id_to_onu_id(L7_uint32 ptin_port, L7_uint32 clientId)
{
  ptinIgmpGroupClientInfoData_t*   ptinIgmpClientGroupInfoData;

  /* Argument validation */
  if (ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF ||  clientId >= PTIN_IGMP_CLIENTIDX_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u clientId:%u ]",ptin_port, clientId);    
    return((L7_uint8) -1);
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input arguments [ptin_port:%u clientId:%u]",
              ptin_port, clientId);

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);  
  if ( (ptinIgmpClientGroupInfoData = deviceClientId2groupClientPtr(ptin_port, clientId)) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to find any valid clientGroup [ptin_port:%u clientId:%u]",ptin_port, clientId);    
    osapiSemaGive(ptin_igmp_clients_sem);
    return((L7_uint8) -1);
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output arguments [onuId:%u]",
              ptinIgmpClientGroupInfoData->onuId);

  osapiSemaGive(ptin_igmp_clients_sem);
  return(ptinIgmpClientGroupInfoData->onuId);
}

/**
 * @purpose Verifies if a Service Multicast has 
 * available resources for a new multicast channels 
 * 
 * @param ptin_port 
 * @param clientId 
 * @param serviceId 
 * @param channelBandwidth 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_multicast_service_resources_available(L7_uint32 ptin_port, L7_uint32 clientId, L7_uint32 serviceId, L7_uint32 channelBandwidth)
{
  igmpMulticastAdmissionControl_t* ptinIgmpAdmissionControlPtr;
  L7_uint8                         onuId = (L7_uint8) -1;
  L7_uint8                         globalAdmissionControl = ptin_igmp_proxy_admission_control_get();
  RC_t                             rc;

  /* Argument validation */
  if (ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF ||  clientId >= PTIN_IGMP_CLIENTIDX_MAX || 
      serviceId>=PTIN_SYSTEM_N_EXTENDED_EVCS ||  channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_KBPS ||
      (onuId = ptin_igmp_client_id_to_onu_id(ptin_port,clientId)) >= PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u clientId:%u (onuId:%u) serviceId:%u channelBandwidth:%u kbps]",
            ptin_port, clientId, onuId, serviceId, channelBandwidth);    
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input arguments [ptin_port:%u clientId:%u (onuId:%u) serviceId:%u channelBandwidth:%u kbps]",
              ptin_port, clientId, onuId, serviceId, channelBandwidth);

  if ( globalAdmissionControl == L7_FALSE )
  {
    //Admission Control Disabled
    if (ptin_debug_igmp_snooping)
      LOG_INFO(LOG_CTX_PTIN_IGMP, "Global Admission Control Feature is Disabled");
    return L7_SUCCESS;
  }

  if (L7_NULLPTR == (ptinIgmpAdmissionControlPtr = ptin_igmp_admission_control_multicast_service_get(ptin_port, serviceId, onuId)))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid Parameters: ptinIgmpAdmissionControlPtr:%p",ptinIgmpAdmissionControlPtr);
    return L7_FAILURE;
  }

  if (L7_SUCCESS != (rc = ptin_igmp_admission_control_available(&ptinIgmpAdmissionControlPtr->admissionControl, channelBandwidth)))
  {
    //Not Enough Resources    
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Multicast Service without Resources [ptin_port:%u clientId:%u (onuId:%u) serviceId:%u channelBandwidth:%u kbps]",
                 ptin_port, clientId, onuId, serviceId, channelBandwidth);
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [ptin_port:%u clientId:%u (onuId:%u) serviceId:%u channelBandwidth:%u (kbps) mask:%u maxAllowedChannels:%u maxAllowedBandwidth:%u allocatedChannels:%u allocatedBandwidth:%u]",
              ptin_port, clientId, onuId, serviceId, channelBandwidth, 
              ptinIgmpAdmissionControlPtr->admissionControl.mask, ptinIgmpAdmissionControlPtr->admissionControl.maxAllowedChannels, ptinIgmpAdmissionControlPtr->admissionControl.maxAllowedBandwidth,
              ptinIgmpAdmissionControlPtr->admissionControl.allocatedChannels, ptinIgmpAdmissionControlPtr->admissionControl.allocatedBandwidth);

  return rc;
}


/**
 * @purpose Allocate Multicast Service Resources for a new 
 *          multicast channel
 * 
 * @param ptin_port 
 * @param clientId 
 * @param serviceId 
 * @param channelBandwidth 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_multicast_service_resources_allocate(L7_uint32 ptin_port, L7_uint32 clientId, L7_uint32 serviceId, L7_uint32 channelBandwidth)
{
  igmpMulticastAdmissionControl_t* ptinIgmpAdmissionControlPtr;
  L7_uint8                         onuId = (L7_uint8) -1;
  L7_uint8                         globalAdmissionControl = ptin_igmp_proxy_admission_control_get();
  RC_t                             rc;

  /* Argument validation */
  if (ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF ||  clientId >= PTIN_IGMP_CLIENTIDX_MAX || 
      serviceId>=PTIN_SYSTEM_N_EXTENDED_EVCS ||  channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_KBPS ||
      (onuId = ptin_igmp_client_id_to_onu_id(ptin_port,clientId)) >= PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u clientId:%u (onuId:%u) serviceId:%u channelBandwidth:%u kbps]",
            ptin_port, clientId, onuId, serviceId, channelBandwidth);    
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input arguments [ptin_port:%u clientId:%u (onuId:%u) serviceId:%u channelBandwidth:%u kbps]",
              ptin_port, clientId, onuId, serviceId, channelBandwidth);


  if ( globalAdmissionControl == L7_FALSE )
  {
    //Admission Control Disabled
    if (ptin_debug_igmp_snooping)
      LOG_INFO(LOG_CTX_PTIN_IGMP, "Global Admission Control Feature is Disabled");
    return L7_SUCCESS;
  }

  if (L7_NULLPTR == (ptinIgmpAdmissionControlPtr = ptin_igmp_admission_control_multicast_service_get(ptin_port, serviceId, onuId)))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid Parameters: ptinIgmpAdmissionControlPtr:%p",ptinIgmpAdmissionControlPtr);
    return L7_FAILURE;
  }

  if (L7_SUCCESS != (rc = ptin_igmp_admission_control_allocate(&ptinIgmpAdmissionControlPtr->admissionControl, channelBandwidth)))
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Failed to Allocate Multicast Service Resources [ptin_port:%u clientId:%u (onuId:%u) serviceId:%u channelBandwidth:%u kbps]",
                 ptin_port, clientId, onuId, serviceId, channelBandwidth);
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [ptin_port:%u clientId:%u (onuId:%u) serviceId:%u channelBandwidth:%u (kbps) mask:%u maxAllowedChannels:%u maxAllowedBandwidth:%u allocatedChannels:%u allocatedBandwidth:%u]",
              ptin_port, clientId, onuId, serviceId, channelBandwidth, 
              ptinIgmpAdmissionControlPtr->admissionControl.mask, ptinIgmpAdmissionControlPtr->admissionControl.maxAllowedChannels, ptinIgmpAdmissionControlPtr->admissionControl.maxAllowedBandwidth,
              ptinIgmpAdmissionControlPtr->admissionControl.allocatedChannels, ptinIgmpAdmissionControlPtr->admissionControl.allocatedBandwidth);

  return rc;  
}

/**
 * @purpose Release Multicast Service Resources
 * 
 * @param ptin_port 
 * @param clientId 
 * @param serviceId 
 * @param channelBandwidth 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_multicast_service_resources_release(L7_uint32 ptin_port, L7_uint32 clientId, L7_uint32 serviceId, L7_uint32 channelBandwidth)
{
  igmpMulticastAdmissionControl_t* ptinIgmpAdmissionControlPtr;
  L7_uint8                         onuId = (L7_uint8) -1;
  L7_uint8                         globalAdmissionControl = ptin_igmp_proxy_admission_control_get();
  RC_t                             rc;

  /* Argument validation */
  if (ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF ||  clientId >= PTIN_IGMP_CLIENTIDX_MAX || 
      serviceId>=PTIN_SYSTEM_N_EXTENDED_EVCS ||  channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_KBPS ||
      (onuId = ptin_igmp_client_id_to_onu_id(ptin_port,clientId)) >= PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u clientId:%u (onuId:%u) serviceId:%u channelBandwidth:%u kbps]",
            ptin_port, clientId, onuId, serviceId, channelBandwidth);    
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input arguments [ptin_port:%u clientId:%u (onuId:%u) serviceId:%u channelBandwidth:%u kbps]",
              ptin_port, clientId, onuId, serviceId, channelBandwidth);

  if ( globalAdmissionControl == L7_FALSE )
  {
    //Admission Control Disabled
    if (ptin_debug_igmp_snooping)
      LOG_INFO(LOG_CTX_PTIN_IGMP, "Global Admission Control Feature is Disabled");
    return L7_SUCCESS;
  }

  if (L7_NULLPTR == (ptinIgmpAdmissionControlPtr = ptin_igmp_admission_control_multicast_service_get(ptin_port, serviceId, onuId)))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid Parameters: ptinIgmpAdmissionControlPtr:%p",ptinIgmpAdmissionControlPtr);
    return L7_FAILURE;
  }

  if (L7_SUCCESS != (rc = ptin_igmp_admission_control_release(&ptinIgmpAdmissionControlPtr->admissionControl, channelBandwidth)))
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Failed to Release Multicast Service Resources [ptin_port:%u clientId:%u (onuId:%u) serviceId:%u channelBandwidth:%u kbps]",
                 ptin_port, clientId, onuId, serviceId, channelBandwidth);
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [ptin_port:%u clientId:%u (onuId:%u) serviceId:%u channelBandwidth:%u (kbps) mask:%u maxAllowedChannels:%u maxAllowedBandwidth:%u allocatedChannels:%u allocatedBandwidth:%u]",
              ptin_port, clientId, onuId, serviceId, channelBandwidth, 
              ptinIgmpAdmissionControlPtr->admissionControl.mask, ptinIgmpAdmissionControlPtr->admissionControl.maxAllowedChannels, ptinIgmpAdmissionControlPtr->admissionControl.maxAllowedBandwidth,
              ptinIgmpAdmissionControlPtr->admissionControl.allocatedChannels, ptinIgmpAdmissionControlPtr->admissionControl.allocatedBandwidth);

  return rc;  
}

/**
 * @purpose Port Resources Available?
 * 
 * @param ptin_port 
 * @param channelBandwidth 
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_port_resources_available(L7_uint32 ptin_port, L7_uint32 channelBandwidth)
{
  ptinIgmpAdmissionControlPort_t* ptinIgmpAdmissionControlPtr = L7_NULLPTR;
  L7_uint8                        globalAdmissionControl = ptin_igmp_proxy_admission_control_get();
  RC_t                            rc;

  /* Argument validation */
  if (ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF || channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_KBPS || L7_NULLPTR == (ptinIgmpAdmissionControlPtr = ptin_igmp_admission_control_port_get(ptin_port)) )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u channelBandwidth:%u kbps ptinIgmpAdmissionControlPtr:%p]",ptin_port, channelBandwidth, ptinIgmpAdmissionControlPtr);    
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [ptin_port:%u channelBandwidth:%u kbps]",ptin_port, channelBandwidth);

  if ( globalAdmissionControl == L7_FALSE )
  {
    //Admission Control Disabled
    if (ptin_debug_igmp_snooping)
      LOG_INFO(LOG_CTX_PTIN_IGMP, "Global Admission Control Feature is Disabled");
    return L7_SUCCESS;
  }

  if (L7_SUCCESS != (rc = ptin_igmp_admission_control_available(&ptinIgmpAdmissionControlPtr->admissionControl, channelBandwidth)))
  {
    //Not Enough Resources    
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Port without Resources [ptin_port:%u channelBandwidth:%u kbps]",ptin_port, channelBandwidth);
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [ptin_port:%u channelBandwidth:%u kbps mask:%u maxAllowedChannels:%u maxAllowedBandwidth:%u allocatedChannels:%u allocatedBandwidth:%u]",
              ptin_port, channelBandwidth,
              ptinIgmpAdmissionControlPtr->admissionControl.mask, ptinIgmpAdmissionControlPtr->admissionControl.maxAllowedChannels, ptinIgmpAdmissionControlPtr->admissionControl.maxAllowedBandwidth,
              ptinIgmpAdmissionControlPtr->admissionControl.allocatedChannels, ptinIgmpAdmissionControlPtr->admissionControl.allocatedBandwidth);

  return rc;  
}

/**
 * @purpose Allocate Multicast Resources for a given port
 * 
 * @param ptin_port 
 * @param channelBandwidth 
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_port_resources_allocate(L7_uint32 ptin_port, L7_uint32 channelBandwidth)
{
  ptinIgmpAdmissionControlPort_t* ptinIgmpAdmissionControlPtr = L7_NULLPTR;
  L7_uint8                        globalAdmissionControl = ptin_igmp_proxy_admission_control_get();
  RC_t                            rc;

  /* Argument validation */
  if (ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF || channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_KBPS || L7_NULLPTR == (ptinIgmpAdmissionControlPtr = ptin_igmp_admission_control_port_get(ptin_port)) )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u channelBandwidth:%u kbps ptinIgmpAdmissionControlPtr:%p]",ptin_port, channelBandwidth, ptinIgmpAdmissionControlPtr);    
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [ptin_port:%u channelBandwidth:%u kbps]",ptin_port, channelBandwidth);

  if ( globalAdmissionControl == L7_FALSE )
  {
    //Admission Control Disabled
    if (ptin_debug_igmp_snooping)
      LOG_INFO(LOG_CTX_PTIN_IGMP, "Global Admission Control Feature is Disabled");
    return L7_SUCCESS;
  }

  if (L7_SUCCESS != (rc = ptin_igmp_admission_control_allocate(&ptinIgmpAdmissionControlPtr->admissionControl, channelBandwidth)))
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Failed to Allocate Port Resources [ptin_port:%u channelBandwidth:%u kbps]",ptin_port, channelBandwidth);
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [ptin_port:%u channelBandwidth:%u kbps mask:%u maxAllowedChannels:%u maxAllowedBandwidth:%u allocatedChannels:%u allocatedBandwidth:%u]",
              ptin_port, channelBandwidth,
              ptinIgmpAdmissionControlPtr->admissionControl.mask, ptinIgmpAdmissionControlPtr->admissionControl.maxAllowedChannels, ptinIgmpAdmissionControlPtr->admissionControl.maxAllowedBandwidth,
              ptinIgmpAdmissionControlPtr->admissionControl.allocatedChannels, ptinIgmpAdmissionControlPtr->admissionControl.allocatedBandwidth);
  return rc;  
}

/**
 * @purpose Release Multicast Resources for a given port
 * 
 * @param ptin_port 
 * @param channelBandwidth 
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_port_resources_release(L7_uint32 ptin_port, L7_uint32 channelBandwidth)
{
  ptinIgmpAdmissionControlPort_t* ptinIgmpAdmissionControlPtr = L7_NULLPTR;
  L7_uint8                        globalAdmissionControl = ptin_igmp_proxy_admission_control_get();
  RC_t                            rc;

  /* Argument validation */
  if (ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF || channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_KBPS || L7_NULLPTR == (ptinIgmpAdmissionControlPtr = ptin_igmp_admission_control_port_get(ptin_port)) )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u channelBandwidth:%u kbps ptinIgmpAdmissionControlPtr:%p]",ptin_port, channelBandwidth, ptinIgmpAdmissionControlPtr);    
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [ptin_port:%u channelBandwidth:%u kbps]",ptin_port, channelBandwidth);

  if ( globalAdmissionControl == L7_FALSE )
  {
    //Admission Control Disabled
    if (ptin_debug_igmp_snooping)
      LOG_INFO(LOG_CTX_PTIN_IGMP, "Global Admission Control Feature is Disabled");
    return L7_SUCCESS;
  }

  if (L7_SUCCESS != (rc = ptin_igmp_admission_control_release(&ptinIgmpAdmissionControlPtr->admissionControl, channelBandwidth)))
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Failed to Release Port Resources [ptin_port:%u channelBandwidth:%u kbps]",ptin_port, channelBandwidth);
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [ptin_port:%u channelBandwidth:%u kbps mask:%u maxAllowedChannels:%u maxAllowedBandwidth:%u allocatedChannels:%u allocatedBandwidth:%u]",
              ptin_port, channelBandwidth,
              ptinIgmpAdmissionControlPtr->admissionControl.mask, ptinIgmpAdmissionControlPtr->admissionControl.maxAllowedChannels, ptinIgmpAdmissionControlPtr->admissionControl.maxAllowedBandwidth,
              ptinIgmpAdmissionControlPtr->admissionControl.allocatedChannels, ptinIgmpAdmissionControlPtr->admissionControl.allocatedBandwidth);
  return rc;  
}

L7_uint8 ptin_igmp_proxy_admission_control_get(void){return (igmpProxyCfg.bandwidthControl | igmpProxyCfg.channelsControl);}

L7_uint8 ptin_igmp_proxy_bandwidth_control_get(void){return igmpProxyCfg.bandwidthControl;}

L7_uint8 ptin_igmp_proxy_channels_control_get(void){return igmpProxyCfg.channelsControl;}


/**
 * @purpose Verifies if a given client Id has available 
 * resources for a new multicast channels 
 * 
 * @param ptin_port 
 * @param clientId 
 * @param channelBandwidth 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_client_resources_available(L7_uint32 ptin_port, L7_uint32 clientId, L7_uint32 channelBandwidth)
{
  ptinIgmpGroupClientInfoData_t*  ptinIgmpClientGroupInfoData;
  L7_uint8                        globalAdmissionControl = ptin_igmp_proxy_admission_control_get();
  L7_RC_t                         rc;


  //We currently do not support any of this modes
#if (!MC_CLIENT_INTERF_SUPPORTED)
#error "This Mode Is Currently Not Supported!"
#endif

  /* Argument validation */
  if ( ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF || clientId >= PTIN_IGMP_CLIENTIDX_MAX || channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_KBPS )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u clientId:%u channelBandwidth:%u kbps]",ptin_port, clientId, channelBandwidth);    
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [ptin_port:%u clientId:%u channelBandwidth:%u kbps]",ptin_port, clientId, channelBandwidth);

  if ( globalAdmissionControl == L7_FALSE )
  {
    //Admission Control Disabled
    if (ptin_debug_igmp_snooping)
      LOG_INFO(LOG_CTX_PTIN_IGMP, "Global Admission Control Feature is Disabled");
    return L7_SUCCESS;
  }

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);  
  if ( (ptinIgmpClientGroupInfoData = deviceClientId2groupClientPtr(ptin_port, clientId)) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to find any valid clientGroup [ptin_port:%u clientId:%u]",ptin_port, clientId);    
    osapiSemaGive(ptin_igmp_clients_sem);
    return L7_FAILURE;
  }

  if (L7_SUCCESS != (rc = ptin_igmp_admission_control_available(&ptinIgmpClientGroupInfoData->admissionControl, channelBandwidth)))
  {
    //Not Enough Resources
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Client without resources [ptin_port:%u clientId:%u channelBandwidth:%u kbps]",ptin_port, clientId, channelBandwidth);
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [ptin_port:%u clientId:%u channelBandwidth:%u kbps mask:%u maxAllowedChannels:%u maxAllowedBandwidth:%u allocatedChannels:%u allocatedBandwidth:%u]",
              ptin_port, clientId, channelBandwidth,
              ptinIgmpClientGroupInfoData->admissionControl.mask, ptinIgmpClientGroupInfoData->admissionControl.maxAllowedChannels, ptinIgmpClientGroupInfoData->admissionControl.maxAllowedBandwidth,
              ptinIgmpClientGroupInfoData->admissionControl.allocatedChannels, ptinIgmpClientGroupInfoData->admissionControl.allocatedBandwidth);

  osapiSemaGive(ptin_igmp_clients_sem);
  return rc;
}

/**
 * @purpose Allocate Multicast Resources for a given client Id
 * 
 * @param ptin_port 
 * @param clientId 
 * @param channelBandwidth 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_client_resources_allocate(L7_uint32 ptin_port, L7_uint32 clientId, L7_uint32 channelBandwidth)
{
  ptinIgmpGroupClientInfoData_t*  ptinIgmpClientGroupInfoData;
  L7_uint8                        globalAdmissionControl = ptin_igmp_proxy_admission_control_get();
  L7_RC_t                         rc;


  //We currently do not support any of this modes
#if (!MC_CLIENT_INTERF_SUPPORTED)
#error "This Mode Is Currently Not Supported!"
#endif

  /* Argument validation */
  if ( ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF || clientId >= PTIN_IGMP_CLIENTIDX_MAX || channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_KBPS )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u clientId:%u channelBandwidth:%u kbps]",ptin_port, clientId, channelBandwidth);    
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [ptin_port:%u clientId:%u channelBandwidth:%u kbps]",ptin_port, clientId, channelBandwidth);

  if ( globalAdmissionControl == L7_FALSE )
  {
    //Admission Control Disabled
    if (ptin_debug_igmp_snooping)
      LOG_INFO(LOG_CTX_PTIN_IGMP, "Global Admission Control Feature is Disabled");
    return L7_SUCCESS;
  }

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);  
  if ( (ptinIgmpClientGroupInfoData = deviceClientId2groupClientPtr(ptin_port, clientId)) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to find any valid clientGroup [ptin_port:%u clientId:%u]",ptin_port, clientId);    
    osapiSemaGive(ptin_igmp_clients_sem);
    return L7_FAILURE;
  }

  if (L7_SUCCESS != (rc = ptin_igmp_admission_control_allocate(&ptinIgmpClientGroupInfoData->admissionControl, channelBandwidth)))
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Failed to Allocate Client Resources [ptin_port:%u clientId:%u channelBandwidth:%u kbps]",ptin_port, clientId, channelBandwidth);
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [ptin_port:%u clientId:%u channelBandwidth:%u kbps mask:%u maxAllowedChannels:%u maxAllowedBandwidth:%u allocatedChannels:%u allocatedBandwidth:%u]",
              ptin_port, clientId, channelBandwidth,
              ptinIgmpClientGroupInfoData->admissionControl.mask, ptinIgmpClientGroupInfoData->admissionControl.maxAllowedChannels, ptinIgmpClientGroupInfoData->admissionControl.maxAllowedBandwidth,
              ptinIgmpClientGroupInfoData->admissionControl.allocatedChannels, ptinIgmpClientGroupInfoData->admissionControl.allocatedBandwidth);

  osapiSemaGive(ptin_igmp_clients_sem);
  return rc;
}

/**
 * @purpose Release Multicast Resources for a given client Id
 * 
 * @param ptin_port 
 * @param clientId 
 * @param channelBandwidth 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_client_resources_release(L7_uint32 ptin_port, L7_uint32 clientId, L7_uint32 channelBandwidth)
{
  ptinIgmpGroupClientInfoData_t*  ptinIgmpClientGroupInfoData;
  L7_uint8                        globalAdmissionControl = ptin_igmp_proxy_admission_control_get();
  L7_RC_t                         rc;


  //We currently do not support any of this modes
#if (!MC_CLIENT_INTERF_SUPPORTED)
#error "This Mode Is Currently Not Supported!"
#endif

  /* Argument validation */
  if ( ptin_port >= PTIN_SYSTEM_N_UPLINK_INTERF || clientId >= PTIN_IGMP_CLIENTIDX_MAX || channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_KBPS )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u clientId:%u channelBandwidth:%u kbps]",ptin_port, clientId, channelBandwidth);    
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [ptin_port:%u clientId:%u channelBandwidth:%u kbps]",ptin_port, clientId, channelBandwidth);

  if ( globalAdmissionControl == L7_FALSE )
  {
    //Admission Control Disabled
    if (ptin_debug_igmp_snooping)
      LOG_INFO(LOG_CTX_PTIN_IGMP, "Global Admission Control Feature is Disabled");
    return L7_SUCCESS;
  }

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);  
  if ( (ptinIgmpClientGroupInfoData = deviceClientId2groupClientPtr(ptin_port, clientId)) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to find any valid clientGroup [ptin_port:%u clientId:%u]",ptin_port, clientId);    
    osapiSemaGive(ptin_igmp_clients_sem);
    return L7_FAILURE;
  }

  if (L7_SUCCESS != (rc = ptin_igmp_admission_control_release(&ptinIgmpClientGroupInfoData->admissionControl, channelBandwidth)))
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Failed to Release Client Resources[ptin_port:%u clientId:%u channelBandwidth:%u kbps]",ptin_port, clientId, channelBandwidth);
  }

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [ptin_port:%u clientId:%u channelBandwidth:%u kbps mask:%u maxAllowedChannels:%u maxAllowedBandwidth:%u allocatedChannels:%u allocatedBandwidth:%u]",
              ptin_port, clientId, channelBandwidth,
              ptinIgmpClientGroupInfoData->admissionControl.mask, ptinIgmpClientGroupInfoData->admissionControl.maxAllowedChannels, ptinIgmpClientGroupInfoData->admissionControl.maxAllowedBandwidth,
              ptinIgmpClientGroupInfoData->admissionControl.allocatedChannels, ptinIgmpClientGroupInfoData->admissionControl.allocatedBandwidth);

  osapiSemaGive(ptin_igmp_clients_sem);
  return rc;
}

void ptin_igmp_admission_control_reset_allocation(void)
{
  ptin_igmp_admission_control_group_clients_reset_allocation();

  ptin_igmp_admission_control_port_reset_allocation();

  ptin_igmp_admission_control_multicast_service_reset_allocation();
}

void ptin_igmp_admission_control_reset_all(void)
{
  ptin_igmp_admission_control_group_clients_reset_all();

  ptin_igmp_admission_control_port_reset_all();

  ptin_igmp_admission_control_multicast_service_reset_all();
}

#endif
/**********End IGMP Admission Control Feature****************************************************/ 

/**
 * Get IGMP Client Bitmap
 *  
 * @param extendedEvcId       : Extended EVC Id
 * @param intIfNum            : intIfNum
 * @param clientBmpPtr        : Client Bitmap Pointer
 * 
 * @return  L7_RC_t           : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_igmp_clients_bmp_get(L7_uint32 extendedEvcId, L7_uint32 intIfNum, L7_uchar8 *clientBmpPtr, L7_uint32 *noOfClients)
{
  L7_uint32 i_client;
  L7_uint32 clientIntIfNum;
  L7_uint32 clientExtendedEvcId;
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientInfoData_t *avl_info;

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  if (clientBmpPtr == L7_NULLPTR || noOfClients == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid input parameters: [clientBmpPtrList=%p  noOfClients=%p]",clientBmpPtr,noOfClients);
    return L7_FAILURE;
  }

  if (ptin_debug_igmp_snooping)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"List of clients (%u clients):",igmpDeviceClients.number_of_clients);
  }

  i_client = 0;
  *noOfClients=0;

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  while ( ( avl_info = (ptinIgmpClientInfoData_t *)
            avlSearchLVL7(&igmpDeviceClients.avlTree.igmpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->igmpClientDataKey, sizeof(ptinIgmpClientDataKey_t));

    if (ptin_debug_igmp_snooping)
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"      Client#%u: "
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
                ": port=%-2u/index=%-3u  uni_vid=%4u+%-4u [%s] "
#if !PTIN_SNOOP_USE_MGMD 
                "#channels=%u"
#endif
                " ",
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
                avl_info->deviceClientId,
                avl_info->uni_ovid, avl_info->uni_ivid,
                ((avl_info->isDynamic) ? "dynamic" : "static ")
#if !PTIN_SNOOP_USE_MGMD  
                ,(avl_info->pClientGroup != L7_NULLPTR) ? avl_info->pClientGroup->stats_client.active_groups : 0
#endif
               );
    }


#if (MC_CLIENT_INTERF_SUPPORTED)
    if (ptin_intf_port2intIfNum(avl_info->igmpClientDataKey.ptin_port,&clientIntIfNum)!=SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to convert port2intIfNum :%u",avl_info->igmpClientDataKey.ptin_port);
      continue;
    }
    if (clientIntIfNum==intIfNum)
#endif
    {
#if (MC_CLIENT_OUTERVLAN_SUPPORTED)
      if (L7_SUCCESS != ptin_evc_get_evcIdfromIntVlan(avl_info->igmpClientDataKey.outerVlan,&clientExtendedEvcId))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to get external EVC Id :%u",avl_info->igmpClientDataKey.outerVlan);
        clientExtendedEvcId=avl_info->igmpClientDataKey.outerVlan;
//      continue;
      }
      if (clientExtendedEvcId==extendedEvcId)
#endif
      {
        PTIN_CLIENT_SET_MASKBIT(clientBmpPtr, avl_info->deviceClientId);     
        (*noOfClients)++;
        if (ptin_debug_igmp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "Client Found [ServiceId:%u PortId:%u ClientId:%u]",clientExtendedEvcId,clientIntIfNum,avl_info->deviceClientId);
      }
    }

    i_client++;
  }
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Done!");
  osapiSemaGive(ptin_igmp_clients_sem);

  return SUCCESS;
}

/**
 * Get the id of the first client of each groupclient and fill 
 * the bitmap with them
 * 
 * @param  intIfNum 
 * @param *clientBmpPtr 
 * @param *noOfClients 
 *  
 * @return L7_RC_t           : L7_SUCCESS/L7_FAILURE  
 *  
 * @notes The noOfClients will be used to encode the clientId
 */
L7_RC_t ptin_igmp_groupclients_bmp_get(L7_uint32 extendedEvcId, L7_uint32 intIfNum, L7_uchar8 *clientBmpPtr, L7_uint32 *noOfClients)
{
  L7_uint32                       ptin_port;  
  L7_uint32                       client_idx;
  L7_uint32                       clientExtendedEvcId;
  ptinIgmpGroupClientInfoData_t  *clientGroup;
  ptinIgmpDeviceClient_t         *client_device;          

  if (intIfNum==0 || intIfNum >= L7_MAX_INTERFACE_COUNT)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "intIfNum# %u is out of range [1..%u]", intIfNum, L7_MAX_INTERFACE_COUNT);
    return L7_FAILURE;
  }

  if (extendedEvcId >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "extendedEvcId# %u is out of range [0..%u]", extendedEvcId, PTIN_SYSTEM_N_EXTENDED_EVCS);
    return L7_FAILURE;
  }

  if (clientBmpPtr == L7_NULLPTR || noOfClients == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid Input Parameters: clientBmpPtr=%p noOfClients=%p",clientBmpPtr, noOfClients);
  }

  *noOfClients = 0;

  if ( L7_SUCCESS != ptin_intf_intIfNum2port(intIfNum, &ptin_port))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to convert intIfNum:%u",intIfNum);    
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  for ( client_idx = 0; client_idx < PTIN_IGMP_CLIENTIDX_MAX; client_idx++)
  {
    if ( PTIN_CLIENT_IS_MASKBITSET(clientBmpPtr, client_idx) == L7_TRUE)
    {
      continue;
    }

    /*Is this device client or group client invalid?*/
    if (igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client == L7_NULLPTR || 
        igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client->pClientGroup == L7_NULLPTR)
    {
      continue;
    }

    clientGroup   = igmpDeviceClients.client_devices[PTIN_IGMP_CLIENT_PORT(ptin_port)][client_idx].client->pClientGroup;
    client_device = L7_NULLPTR;          

    if ( (client_device=igmp_clientDevice_next(clientGroup, client_device)) != L7_NULLPTR && client_device->client != L7_NULLPTR)
    {
      if (  PTIN_CLIENT_IS_MASKBITSET(clientBmpPtr, client_device->client->deviceClientId) == L7_TRUE )
      {
        continue;
      }

#if (MC_CLIENT_OUTERVLAN_SUPPORTED)     
      if (L7_SUCCESS != ptin_evc_get_evcIdfromIntVlan(clientGroup->igmpClientDataKey.outerVlan,&clientExtendedEvcId))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to get external EVC Id for outerVlan:%u", clientGroup->igmpClientDataKey.outerVlan);        
        continue;
      }
      if (clientExtendedEvcId != extendedEvcId)
      {
        continue;
      }
#endif

      PTIN_CLIENT_SET_MASKBIT(clientBmpPtr, client_device->client->deviceClientId);
      (*noOfClients)++; 

      if (ptin_debug_igmp_snooping)
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Client Found [extendedEvcId:%u ptin_port:%u clientId:%u]",extendedEvcId, ptin_port, client_device->client->deviceClientId);
      }
    }
  }

  if (ptin_debug_igmp_snooping)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Number of Clients found [extendedEvcId:%u ptin_port:%u noOfClients:%u]",extendedEvcId, ptin_port, *noOfClients);
  }

#if 0
  L7_uint                        i_client = 0;            
  L7_uint                        child_clients;
  ptinIgmpClientDataKey_t        avl_key;
  ptinIgmpClientGroupInfoData_t *clientGroup;
  ptinIgmpClientDevice_t        *client_device;
  L7_uint32                      clientExtendedEvcId;

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"List of Group clients (%u clients):\n",igmpClientGroups.number_of_clients);

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  while ( ( clientGroup = (ptinIgmpClientGroupInfoData_t *)
            avlSearchLVL7(&igmpClientGroups.avlTree.igmpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &clientGroup->igmpClientDataKey, sizeof(ptinIgmpClientDataKey_t));

    client_device = L7_NULLPTR;
    if ( (child_clients=igmp_clientDevice_get_devices_number(clientGroup))!=0 &&
         (client_device=igmp_clientDevice_next(clientGroup, client_device)) != L7_NULLPTR)
    {
#if (MC_CLIENT_INTERF_SUPPORTED)
      L7_uint32 clientIntIfNum;
      if (ptin_intf_port2intIfNum(clientGroup->igmpClientDataKey.ptin_port,&clientIntIfNum)!=SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to convert port2intIfNum :%u",clientGroup->igmpClientDataKey.ptin_port);
        continue;
      }
      if (clientIntIfNum==intIfNum)
#endif
      {
#if (MC_CLIENT_OUTERVLAN_SUPPORTED)
        if (L7_SUCCESS != ptin_evc_get_evcIdfromIntVlan(clientGroup->igmpClientDataKey.outerVlan,&clientExtendedEvcId))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to get external EVC Id for outerVlan:%u", clientGroup->igmpClientDataKey.outerVlan);        
          continue;
        }
        if (clientExtendedEvcId==extendedEvcId)
#endif
        {
          PTIN_CLIENT_SET_MASKBIT(clientBmpPtr, client_device->client->client_index);     
          (*noOfClients)++;
          if (ptin_debug_igmp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_IGMP, "Client Found [ServiceId:%u PortId:%u ClientId:%u]",clientExtendedEvcId, clientIntIfNum,client_device->client->client_index);
        }
      }
    }

    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"      Client#%u: "
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
                ": port=%-2u uni_vid=%4u+%-4u (#devices=%u)",
                i_client++,
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
  }
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Done!");
#endif
  osapiSemaGive(ptin_igmp_clients_sem);

  return L7_SUCCESS;
}


/**
 * Dumps EVC detailed info 
 * If evc_idx is invalid, all EVCs are dumped 
 * 
 * @param evc_idx 
 */
void ptin_igmp_device_clients_dump(void)
{
  L7_uint i_client;
  ptinIgmpClientDataKey_t avl_key;
  ptinIgmpClientInfoData_t *avl_info;

  osapiSemaTake(ptin_igmp_clients_sem, L7_WAIT_FOREVER);

  printf("List of clients (%u clients):\n",igmpDeviceClients.number_of_clients);

  i_client = 0;

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinIgmpClientDataKey_t));
  while ( ( avl_info = (ptinIgmpClientInfoData_t *)
            avlSearchLVL7(&igmpDeviceClients.avlTree.igmpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->igmpClientDataKey, sizeof(ptinIgmpClientDataKey_t));

    printf("ptin_port_port=%u / clientId=%u (OnuId:%u) "           
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
           " uni_vid=%4u+%-4u [%s] "
#if !PTIN_SNOOP_USE_MGMD 
           "#channels=%u"
#endif
           "\r\n",
           avl_info->ptin_port, avl_info->deviceClientId, (avl_info->pClientGroup != L7_NULLPTR) ? avl_info->pClientGroup->onuId : 0,           
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
           avl_info->uni_ovid, avl_info->uni_ivid,
           ((avl_info->isDynamic) ? "dynamic" : "static ")
#if !PTIN_SNOOP_USE_MGMD 
           ,(avl_info->pClientGroup != L7_NULLPTR) ? avl_info->pClientGroup->stats_client.active_groups : 0
#endif
          );

    i_client++;
  }

  printf("\nNoOfClients:%u\n", i_client);
  osapiSemaGive(ptin_igmp_clients_sem);

  fflush(stdout);
}

/**
 * Dumps all IGMP channels 
 */
void ptin_igmp_channels_dump(L7_int evc_mc, L7_int evc_uc, L7_uint8 flag_port)
{
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  ptinIgmpChannelDataKey_t   avl_key;
  ptinIgmpChannelInfoData_t *avl_info;
  L7_uint16                  n_entries;
  char                       groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                       sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
  L7_int16                   portIdIterator;
  L7_int16                   groupClientIterator;
  struct packagePoolEntry_s* packageEntry = L7_NULLPTR;
  L7_int16                   packageIdAux;

  /* Hello */
  if ( evc_mc <= 0 )
  {
    printf("Printing all IGMP channel entries (%u total):\r\n",channelDB.channelAvlTree.count);
  }
  else
  {
    printf("Printing only IGMP channel entries related to EVC_MC %u:\r\n",evc_mc);
  }

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinIgmpChannelDataKey_t));

  n_entries = 0;

  while ( ( avl_info = (ptinIgmpChannelInfoData_t *)
            avlSearchLVL7(&channelDB.channelAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->channelDataKey, sizeof(ptinIgmpChannelDataKey_t));

    /* Skip entry, if MC evc is provided and does not match to this entry */
    if ( evc_mc > 0 && avl_info->channelDataKey.evc_mc != evc_mc )
    {
      continue;
    }
#if (IGMPASSOC_CHANNEL_UC_EVC_ISOLATION)
    if ( evc_uc > 0 && avl_info->channelDataKey.evc_uc != evc_uc )
    {
      continue;
    }
#endif

    printf(
#if (IGMPASSOC_CHANNEL_UC_EVC_ISOLATION)
          "EVC_UC=%-3u "
#endif
          "EVC_MC=%-3u "
          "groupAddr=%s "
#if (IGMPASSOC_CHANNEL_SOURCE_SUPPORTED)
          "srcIPAddr=%s "
#endif
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
          "bw:%u (kbps)"
#endif
          "(entryType=%-1u) "
          "noOfPorts:%u noOfPackages:%u "
#if (IGMPASSOC_CHANNEL_UC_EVC_ISOLATION)
          , avl_info->channelDataKey.evc_uc
#endif
          , avl_info->channelDataKey.evc_mc
          ,inetAddrPrint(&avl_info->channelDataKey.channel_group, groupAddrStr)            
#if (IGMPASSOC_CHANNEL_SOURCE_SUPPORTED)
          ,inetAddrPrint(&avl_info->channelDataKey.channel_source, sourceAddrStr)            
#endif
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
          ,avl_info->channelBandwidth
#endif
          , avl_info->entryType
          , avl_info->noOfPorts
          , avl_info->queuePackage.n_elems                    
          );          
    while ( L7_NULLPTR != (packageEntry = queue_package_entry_get_next(avl_info, packageEntry)) && 
            (packageIdAux = packageEntry->packageId) < PTIN_SYSTEM_IGMP_MAXPACKAGES )
    {
      printf("packageId:%u ", packageIdAux);
    }
    if (flag_port)
    {
      for (portIdIterator = 0; portIdIterator<PTIN_SYSTEM_N_UPLINK_INTERF; portIdIterator++)
      {
        if (avl_info->noOfGroupClientsPerPort[portIdIterator] == 0)
          continue;
        printf(" portId:%u noOfGroupClients:%u groupClientBmp: 0x", portIdIterator, avl_info->noOfGroupClientsPerPort[portIdIterator]);
        for ( groupClientIterator = PTIN_IGMP_CLIENT_BITMAP_SIZE-1; groupClientIterator>=0; --groupClientIterator )
        {
          printf("%08X", avl_info->groupClientBmpPerPort[portIdIterator][groupClientIterator]);
        }            
      }
    }
    printf("\n");             
    n_entries++;
  }

  printf("Done! %u entries displayed.\r\n",n_entries);

#else
  printf("IGMP Multi-MC not supported on this version\r\n");
#endif

  fflush(stdout);
}

/**
 * Dumps IGMP proxy configuration
 */
void ptin_igmp_proxy_dump(void)
{
  printf("IGMP Proxy global:\r\n");
  printf(" Mask                     = 0x%04x\r\n", igmpProxyCfg.mask);
  printf(" Admin                    = %u\r\n",     igmpProxyCfg.admin);
  printf(" Network Version          = %u\r\n",     igmpProxyCfg.networkVersion);
  printf(" Client  Version          = %u\r\n",     igmpProxyCfg.clientVersion);
  printf(" IPv4 Address             = %03u.%03u.%03u.%03u\r\n",
         (igmpProxyCfg.ipv4_addr.s_addr>>24) & 0xff, (igmpProxyCfg.ipv4_addr.s_addr>>16) & 0xff, (igmpProxyCfg.ipv4_addr.s_addr>>8) & 0xff, (igmpProxyCfg.ipv4_addr.s_addr) & 0xff);
  printf(" COS                      = %u\r\n",     igmpProxyCfg.igmp_cos);
  printf(" Fast Leave?              = %u\r\n",     igmpProxyCfg.fast_leave);
  printf(" Bandwidth Control?       = %u\r\n",     igmpProxyCfg.bandwidthControl);
  printf(" Channels Control?        = %u\r\n",     igmpProxyCfg.channelsControl);

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

  fflush(stdout);
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

  fflush(stdout);
}


/**********************************Multicast Group Packages*********************************************************/
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED

/**
 * @purpose Add Multicast Package
 * 
 * @param packageId 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_multicast_package_add(L7_uint32 packageId)
{
  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u]",packageId);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u]",
              packageId);

  /*Creating the Multicast Package for the First Time*/
  if (multicastPackage[packageId].inUse == L7_FALSE)
  {
    /*Increment the Number of Multicast Packages*/
    noOfMulticastPackages++;

    multicastPackage[packageId].inUse = L7_TRUE;
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Multicast Package Created [packageId:%u noOfMulticastPackages:%u]",
              packageId, noOfMulticastPackages);
    return L7_SUCCESS;    
  }
  else /*This Multicast Package Already Exists*/
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Multicast Package Already Created [packageId:%u noOfMulticastPackages:%u]",
                 packageId, noOfMulticastPackages);
#if 0//Do Not Signal this to the caller
    return L7_ALREADY_CONFIGURED;
#else
    return L7_SUCCESS;    
#endif
  }
}

/**
 * @purpose Remove Multicast Package
 * 
 * @param packageId 
 * @param forceRemoval  
 *  
 * @return RC_t
 *
 * @notes To maintain consistency we do not permit the removal
 *        of packages if clients or channels exist. This
 *        validation can be circumvent by setting to TRUE the
 *        the forceRemoval flag
 *  
 */
RC_t ptin_igmp_multicast_package_remove(L7_uint32 packageId, L7_BOOL forceRemoval)
{
#if 0
  struct channelPoolEntry_s *channelEntry = L7_NULLPTR;
  ptinIgmpChannelInfoData_t *channelAvlTreeEntry;
  L7_uint32                  groupClientId;
  L7_uint32                  ptinPort;
#endif

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u]",packageId);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u]",
              packageId);

  if ( multicastPackage[packageId].inUse == L7_FALSE )
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Package does not exist [packageId:%u]",packageId);
#if 0//Do Not Signal this to the caller
    return L7_NOT_EXIST;
#else
    return L7_SUCCESS;    
#endif
  }

  if (forceRemoval == L7_FALSE)
  {
    if ( multicastPackage[packageId].queueChannel.n_elems != 0 )
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Package contains channels [noOfChannels:%u]", multicastPackage[packageId].queueChannel.n_elems);    
      return L7_DEPENDENCY_NOT_MET;
    }

    if ( multicastPackage[packageId].noOfPorts != 0 )
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Package contain clients on ports [noOfPorts:%u]", multicastPackage[packageId].noOfPorts);    
      return L7_DEPENDENCY_NOT_MET;
    }
  }
  else
  {
#if 0//Not Validated Yet
    while ( L7_NULLPTR != (channelEntry = queue_channel_entry_find_next(packageId, channelEntry)) && 
            L7_NULLPTR != (channelAvlTreeEntry = channelEntry->channelAvlTreeEntry))
    {
      /*We need to remove all clients associated with this multicast package*/
      for (ptinPort = 0; ptinPort <= PTIN_SYSTEM_N_UPLINK_INTERF; ptinPort++)
      {
        for (groupClientId = 0; groupClientId <= PTIN_IGMP_CLIENTIDX_MAX; groupClientId++)
        {
          //Move forward 32 bits if this byte is 0 (no packages)
          if (IS_BITMAP_WORD_SET(channelAvlTreeEntry->groupClientBmpPerPort, groupClientId, UINT32_BITSIZE) == L7_FALSE)
          {
            packageId += UINT32_BITSIZE -1; //Less one, because of the For cycle that increments also 1 unit.
            continue;
          }

          if (IS_BITMAP_BIT_SET(channelAvlTreeEntry->groupClientBmpPerPort, groupClientId, UINT32_BITSIZE))
          {

            if ( L7_SUCCESS != ptin_igmp_multicast_channel_client_remove(ptinPort, groupClientId, 
                                                                         channelAvlTreeEntry->channelDataKey.evc_mc, &channelAvlTreeEntry->channelDataKey.channel_group, &channelAvlTreeEntry->channelDataKey.channel_source))
            {
              LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to remove client from channel [ptinPort:%u clientId:%u serviceId:%u groupAddr:%s sourceAddr:%s]",
                      ptinPort, groupClientId, channelAvlTreeEntry->channelDataKey.evc_mc, 
                      inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_group, groupAddrStr), inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_source, sourceAddrStr));    
              return L7_FAILURE;
            }
          }
        }
      }

      /*We need to remove channel entries associated with this multicast package*/
    }
#endif      
  }

  multicastPackage[packageId].inUse = L7_FALSE;

  if ( noOfMulticastPackages>0 )
  {
    /*Decrement the Number of Multicast Packages*/
    noOfMulticastPackages--;
  }

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Multicast Package Removed [packageId:%u noOfMulticastPackages:%u]",
            packageId, noOfMulticastPackages);

  return L7_SUCCESS;        
}

/**
 * @purpose Multicast Packages Dump
 * 
 * @notes none 
 *  
 */
void ptin_igmp_multicast_package_dump(L7_uint32 packageId)
{
  L7_uint32                   packageIdIterator;
  L7_int8                     portIdIterator;
  L7_int16                    groupClientIterator;
  char                        groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                        sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
  struct channelPoolEntry_s  *channelEntry = L7_NULLPTR;
  ptinIgmpChannelInfoData_t  *channelAvlTreeEntry;

  if ( packageId != (L7_uint32) -1 )
  {
    printf("packageId:%u inUse:%s\n", packageId, multicastPackage[packageId].inUse?"Yes":"No");      
    printf("noOfPorts:%u\n", multicastPackage[packageId].noOfPorts);
    printf("portBmp: 0x");      
    for (portIdIterator = PTIN_IGMP_PORT_BITMAP_SIZE-1; portIdIterator>=0; --portIdIterator)
    {
      printf("%08X", multicastPackage[packageId].portBmp[portIdIterator]);
    }
    printf("\n");             
    for (portIdIterator = 0; portIdIterator<PTIN_SYSTEM_N_UPLINK_INTERF; portIdIterator++)
    {
      printf("portId:%u noOfGroupClients:%u groupClientBmp: 0x", portIdIterator, multicastPackage[packageId].noOfGroupClientsPerPort[portIdIterator]);
      for ( groupClientIterator = PTIN_IGMP_CLIENT_BITMAP_SIZE-1; groupClientIterator>=0; --groupClientIterator )
      {
        printf("%08X", multicastPackage[packageId].groupClientBmpPerPort[portIdIterator][groupClientIterator]);
      }
      printf("\n");             
    }
    printf("noOfChannels:%u\n", multicastPackage[packageId].queueChannel.n_elems);            
    while ( L7_NULLPTR != (channelEntry = queue_channel_entry_get_next(packageId, channelEntry)) && 
            L7_NULLPTR != (channelAvlTreeEntry = channelEntry->channelAvlTreeEntry))
    {
      printf("serviceId:%u groupAddr:%s sourceAddr:%s\n", channelAvlTreeEntry->channelDataKey.evc_mc, inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_group, groupAddrStr), inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_source, sourceAddrStr));
    }
  }
  else
  {
    printf("Multicast Packages [noOfPackages:%u]\n", noOfMulticastPackages);
    for (packageIdIterator = 0; packageIdIterator < PTIN_SYSTEM_IGMP_MAXPACKAGES; packageIdIterator++)
    {
      if ( 1 )
      {
        printf("packageId:%u inUse:%s\n", packageIdIterator, multicastPackage[packageIdIterator].inUse?"Yes":"No");           
        printf("noOfPorts:%u\n", multicastPackage[packageIdIterator].noOfPorts);
        printf("portBmp: 0x");
        for (portIdIterator = PTIN_IGMP_PORT_BITMAP_SIZE -1 ; portIdIterator>=0; --portIdIterator)
        {
          printf("%08X", multicastPackage[packageIdIterator].portBmp[portIdIterator]);
        }
        printf("\n");             
        printf("noOfChannels:%u\n", multicastPackage[packageIdIterator].queueChannel.n_elems);                    
      }
    }  
  }
}

/**
 * @purpose Multicast Packages Dump Active
 * 
 * @notes none 
 *  
 */
void ptin_igmp_multicast_packages_dump_active(void)
{
  L7_uint32                   packageIdIterator;
  L7_int8                     portIdIterator;  

  printf("Multicast Packages [noOfPackages:%u]\n", noOfMulticastPackages);
  for (packageIdIterator = 0; packageIdIterator < PTIN_SYSTEM_IGMP_MAXPACKAGES; packageIdIterator++)
  {
    if ( multicastPackage[packageIdIterator].inUse == L7_TRUE )
    {
      printf("packageId:%u\n", packageIdIterator);      
      printf("noOfPorts:%u\n", multicastPackage[packageIdIterator].noOfPorts);
      printf("portBmp: 0x");
      for (portIdIterator = PTIN_IGMP_PORT_BITMAP_SIZE -1; portIdIterator>=0; --portIdIterator)
      {
        printf("%08X", multicastPackage[packageIdIterator].portBmp[portIdIterator]);
      }
      printf("\n");           
      printf("noOfChannels:%u\n", multicastPackage[packageIdIterator].queueChannel.n_elems);                  
    }
  }  
}

/**
 * @purpose Multicast Packages Dump Active
 * 
 * @notes none 
 *  
 */
void ptin_igmp_multicast_packages_dump_all(void)
{
  L7_uint32                   packageIdIterator;
  L7_int8                     portIdIterator;  

  printf("Multicast Packages [noOfPackages:%u]\n", noOfMulticastPackages);
  for (packageIdIterator = 0; packageIdIterator < PTIN_SYSTEM_IGMP_MAXPACKAGES; packageIdIterator++)
  {
    if ( 1 )
    {
      printf("packageId:%u inUse:%s\n", packageIdIterator, multicastPackage[packageIdIterator].inUse?"Yes":"No");      
      printf("noOfPorts:%u\n", multicastPackage[packageIdIterator].noOfPorts);
      printf("portBmp: 0x");
      for (portIdIterator = PTIN_IGMP_PORT_BITMAP_SIZE -1; portIdIterator>=0; --portIdIterator)
      {
        printf("%08X", multicastPackage[packageIdIterator].portBmp[portIdIterator]);
      }
      printf("\n");           
      printf("noOfChannels:%u\n", multicastPackage[packageIdIterator].queueChannel.n_elems);                  
    }
  }  
}

/**
 * @purpose Multicast Package Channels Dump
 * 
 * @notes none 
 *  
 */
void ptin_igmp_multicast_package_clients_dump(L7_uint32 packageId)
{
  L7_int8                    portIdIterator;
  L7_int16                   groupClientIterator;

  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES)
  {
    printf("Invalid Package Id [packageId:%u > max_packages:%u\n", packageId, PTIN_SYSTEM_IGMP_MAXPACKAGES);      
    return;
  }

  printf("packageId:%u inUse:%s\n", packageId, multicastPackage[packageId].inUse?"Yes":"No");      
  printf("noOfPorts:%u\n", multicastPackage[packageId].noOfPorts);
  printf("portBmp: 0x");
  for (portIdIterator =PTIN_IGMP_PORT_BITMAP_SIZE -1; portIdIterator>=0; --portIdIterator)
  {
    printf("%08X", multicastPackage[packageId].portBmp[portIdIterator]);
  }
  printf("\n");   
  for (portIdIterator = 0; portIdIterator<PTIN_SYSTEM_N_UPLINK_INTERF; portIdIterator++)
  {
    printf("portId:%u noOfGroupClients:%u groupClientBmp: 0x", portIdIterator, multicastPackage[packageId].noOfGroupClientsPerPort[portIdIterator]);
    for ( groupClientIterator = PTIN_IGMP_CLIENT_BITMAP_SIZE -1; groupClientIterator>=0; --groupClientIterator )
    {
      printf("%08X", multicastPackage[packageId].groupClientBmpPerPort[portIdIterator][groupClientIterator]);
    }
    printf("\n");             
  }
}

/**
 * @purpose Multicast Package Channels Dump
 * 
 * @notes none 
 *  
 */
void ptin_igmp_multicast_package_channels_dump(L7_uint32 packageId)
{
  char                        groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                        sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
  struct channelPoolEntry_s  *channelEntry = L7_NULLPTR;
  ptinIgmpChannelInfoData_t  *channelAvlTreeEntry;

  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES)
  {
    printf("Invalid Package Id [packageId:%u > max_packages:%u\n", packageId, PTIN_SYSTEM_IGMP_MAXPACKAGES);      
    return;
  }

  printf("packageId:%u inUse:%s\n", packageId, multicastPackage[packageId].inUse?"Yes":"No");            
  printf("noOfChannels:%u\n", multicastPackage[packageId].queueChannel.n_elems);            
  while ( L7_NULLPTR != (channelEntry = queue_channel_entry_get_next(packageId, channelEntry)) && 
          L7_NULLPTR != (channelAvlTreeEntry = channelEntry->channelAvlTreeEntry))
  {
    printf(" serviceId:%u groupAddr:%s sourceAddr:%s\n", channelAvlTreeEntry->channelDataKey.evc_mc, inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_group, groupAddrStr), inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_source, sourceAddrStr));
  }  
}


/**
 * @purpose Add Multicast IPv4 Channels to a Package
 * 
 * @param packageId 
 * @param serviceId 
 * @param inGroupAddr 
 * @param groupMask  
 * @param inSourceAddr 
 * @param sourceMask  
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_debug_multicast_package_channels_add(L7_uint32 packageId, L7_uint32 serviceId, L7_uint32 inGroupAddr, L7_uint32 groupMask, L7_uint32 inSourceAddr, L7_uint32 sourceMask)
{
  L7_inet_addr_t groupAddr;
  L7_inet_addr_t sourceAddr;

  inetAddressSet(L7_AF_INET, &inGroupAddr, &groupAddr);
  inetAddressSet(L7_AF_INET, &inSourceAddr, &sourceAddr);

  return(ptin_igmp_multicast_package_channels_add(packageId,serviceId,&groupAddr,groupMask,&sourceAddr,sourceMask));
}

/**
 * @purpose Add Multicast Channels to a Package
 * 
 * @param packageId 
 * @param serviceId 
 * @param groupAddr 
 * @param groupMask  
 * @param sourceAddr 
 * @param sourceMask  
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_multicast_package_channels_add(L7_uint32 packageId, L7_uint32 serviceId, L7_inet_addr_t *groupAddr, L7_uint32 groupMask, L7_inet_addr_t *sourceAddr, L7_uint32 sourceMask)
{
  ptinIgmpChannelInfoData_t      *channelAvlTreeEntry = L7_NULLPTR;
  char                           groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                           sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
  L7_inet_addr_t                 groupAddrBase;
  L7_inet_addr_t                 groupAddrBaseIterator;
  L7_inet_addr_t                 sourceAddrBase;
  L7_inet_addr_t                 sourceAddrBaseIterator;
  L7_uint32                      noOfGroups = 0;
  L7_uint32                      noOfSources = 0;
  L7_int32                       groupIterator = 0;
  L7_int32                       sourceIterator = 0;
  RC_t                           rc = L7_SUCCESS;

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || serviceId >= PTIN_SYSTEM_N_EXTENDED_EVCS ||  groupAddr == L7_NULLPTR || sourceAddr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u serviceId:%u groupNetAddr:%p sourceNetAddr:%p]",packageId, serviceId, groupAddr, sourceAddr);    
    return L7_FAILURE;
  }

  /*Validate Family Address. We only support IPV4!*/
  if (groupAddr->family != L7_AF_INET || sourceAddr->family != L7_AF_INET)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Family not supported groupNetAddr->family:%u sourceNetAddr->family:%u", groupAddr->family, sourceAddr->family);
    return L7_NOT_SUPPORTED;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u serviceId:%u groupNetAddr:%s groupMask:%u sourceNetAddr:%s sourceMask:%u]",
              packageId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), groupMask, inetAddrPrint(sourceAddr, sourceAddrStr), sourceMask);

  /*Creating the Multicast Package for the First Time*/
  if (multicastPackage[packageId].inUse == L7_FALSE)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Multicast Package Not Created [packageId:%u serviceId:%u groupNetAddr:%s groupMask:%u sourceNetAddr:%s sourceMask:%u]",
                packageId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), groupMask, inetAddrPrint(sourceAddr, sourceAddrStr), sourceMask);
    return L7_NOT_EXIST;

  }
  else /*This Multicast Package Already Exists*/
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Multicast Package Does Exist [packageId:%u serviceId:%u groupNetAddr:%s groupMask:%u sourceNetAddr:%s sourceMask:%u",
              packageId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), groupMask, inetAddrPrint(sourceAddr, sourceAddrStr), sourceMask);
  }

  /* Validate and prepare channel group Address*/
  if ( (rc = ptin_igmp_channel_to_netmask( groupAddr, groupMask, &groupAddrBase, &noOfGroups)) !=L7_SUCCESS || noOfGroups == 0 || noOfGroups > PTIN_IGMP_CHANNELS_MAX )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to obtain groupAddr:%s from groupMask:%u rc:%u!", inetAddrPrint(groupAddr, groupAddrStr), groupMask, rc);
    return L7_FAILURE;    
  }  
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to add groupAddr:%s (%u addresses)", inetAddrPrint(&groupAddrBase, groupAddrStr), noOfGroups);

  /* Prepare source channel */
  if (L7_SUCCESS != (rc = ptin_igmp_channel_to_netmask( sourceAddr, sourceMask, &sourceAddrBase, &noOfSources)) || noOfSources == 0 || ( (noOfSources * noOfGroups) > PTIN_IGMP_CHANNELS_MAX))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to obtain sourceAddr:%s from sourceMask:%u rc:%u!", inetAddrPrint(sourceAddr, sourceAddrStr), sourceMask, rc);
    return L7_FAILURE;    
  }
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to add sourceAddr:%s (%u addresses)", inetAddrPrint(&sourceAddrBase, sourceAddrStr), noOfSources);

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Maximum addresses to be added: %u", noOfGroups*noOfSources);
  

  memcpy(&groupAddrBaseIterator, &groupAddrBase, sizeof(groupAddrBaseIterator));

  /*Verify If We can Add this Channels to this Package*/
  for (groupIterator = 0; groupIterator<noOfGroups; groupIterator++, groupAddrBaseIterator.addr.ipv4.s_addr++)
  {
    memcpy(&sourceAddrBaseIterator, &sourceAddrBase, sizeof(sourceAddrBaseIterator));
    for (sourceIterator = 0; sourceIterator<noOfSources; sourceIterator++, sourceAddrBaseIterator.addr.ipv4.s_addr++)
    {
      /* Find Channel Entry */
      rc = ptin_igmp_channel_get (serviceId, &groupAddrBaseIterator, &sourceAddrBaseIterator,  &channelAvlTreeEntry );
      if ( rc == L7_NOT_EXIST )
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Channel Does Not Exist [serviceId:%u groupAddr:%s sourceAddr:%s]",
                    serviceId, inetAddrPrint(&groupAddrBaseIterator, groupAddrStr), inetAddrPrint(&sourceAddrBase, sourceAddrStr));
        return L7_NOT_EXIST;      
      }
      else if (rc != L7_SUCCESS || channelAvlTreeEntry == L7_NULLPTR )
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Search Channel Entry [packageId:%u serviceId:%u groupAddr:%s sourceAddr:%p channelEntry:%p]", packageId, serviceId, inetAddrPrint(&groupAddrBaseIterator, groupAddrStr), inetAddrPrint(&sourceAddrBaseIterator, sourceAddrStr), channelAvlTreeEntry);    
        return rc;
      }

      if ( L7_SUCCESS != (rc = ptin_igmp_package_channel_conflict_validation(packageId, channelAvlTreeEntry) ) )
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Channel Conflict Found [packageId:%u serviceId:%u groupAddr:%s sourceAddr:%s]",packageId, serviceId, inetAddrPrint(&groupAddrBaseIterator, groupAddrStr), inetAddrPrint(&sourceAddrBaseIterator, sourceAddrStr));    
        return rc;
      }
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to add groupAddr:%s (%u addresses)", inetAddrPrint(&sourceAddrBase, sourceAddrStr), noOfSources);

  groupIterator = 0;
  memcpy(&groupAddrBaseIterator, &groupAddrBase, sizeof(groupAddrBaseIterator));

  /* Run all group addresses */
  while ((rc==L7_SUCCESS && groupIterator<noOfGroups) || ( rc!= L7_SUCCESS && groupIterator>=0))
  {
    sourceIterator = 0; 
    memcpy(&sourceAddrBaseIterator, &sourceAddrBase, sizeof(sourceAddrBaseIterator));
    /* Run all source addresses */
    while ( ( rc == L7_SUCCESS && sourceIterator<noOfSources ) || ( rc != L7_SUCCESS && sourceIterator>=0 ) )
    {
      /* In case of success, continue adding nodes into avl tree */
      if (rc == L7_SUCCESS)
      {

        if ( ptin_igmp_multicast_package_channel_add(packageId, serviceId, &groupAddrBaseIterator, &sourceAddrBaseIterator) != L7_SUCCESS )
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Error adding channel to multicast package [packageId:%u serviceId:%u groupAddr:%s groupMask:%u sourceAddr:%s sourceMask:%u]",
                  packageId, serviceId, inetAddrPrint(&groupAddrBaseIterator, groupAddrStr), groupMask, inetAddrPrint(&sourceAddrBaseIterator, sourceAddrStr), sourceMask);
          rc = L7_FAILURE;
        }
        else
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Added channel to multicast package [packageId:%u serviceId:%u groupAddr:%s groupMask:%u sourceAddr:%s sourceMask:%u]",
                    packageId, serviceId, inetAddrPrint(&groupAddrBaseIterator, groupAddrStr), groupMask, inetAddrPrint(&sourceAddrBaseIterator, sourceAddrStr), sourceMask);
        }
      }
      /* If one error ocurred, remove previously added nodes */
      else
      {
        if (ptin_igmp_multicast_package_channel_remove(packageId, serviceId, &groupAddrBaseIterator, &sourceAddrBaseIterator) != L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing channel from multicast package [packageId:%u serviceId:%u groupAddr:%s groupMask:%u sourceAddr:%s sourceMask:%u]",
                  packageId, serviceId, inetAddrPrint(&groupAddrBaseIterator, groupAddrStr), groupMask, inetAddrPrint(&sourceAddrBaseIterator, sourceAddrStr), sourceMask);
        }
        else
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removed channel from multicast package [packageId:%u serviceId:%u groupAddr:%s groupMask:%u sourceAddr:%s sourceMask:%u]",
                    packageId, serviceId, inetAddrPrint(&groupAddrBaseIterator, groupAddrStr), groupMask, inetAddrPrint(&sourceAddrBaseIterator, sourceAddrStr), sourceMask);
        }
      }

      /* Next source ip address */
      if (sourceAddrBaseIterator.family != L7_AF_INET6)
      {
        if ( rc == L7_SUCCESS )
        {
          sourceAddrBaseIterator.addr.ipv4.s_addr++;  sourceIterator++;
        }
        else
        {
          sourceAddrBaseIterator.addr.ipv4.s_addr--;  sourceIterator--;
        }
      }
      else
        break;      
    }
    /* Next group address */
    if (groupAddrBaseIterator.family != L7_AF_INET6)
    {
      if ( rc == L7_SUCCESS )
      {
        groupAddrBaseIterator.addr.ipv4.s_addr++;   groupIterator++;
      }
      else
      {
        groupAddrBaseIterator.addr.ipv4.s_addr--;   groupIterator--;
      }
    }
    else
      break;
  }

  return rc;;
}

/**
 * @purpose Remove Multicast IPv4 Channels to a Package
 * 
 * @param packageId 
 * @param serviceId 
 * @param inGroupAddr 
 * @param groupMask  
 * @param inSourceAddr 
 * @param sourceMask  
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_debug_multicast_package_channels_remove(L7_uint32 packageId, L7_uint32 serviceId, L7_uint32 inGroupAddr, L7_uint32 groupMask, L7_uint32 inSourceAddr, L7_uint32 sourceMask)
{
  L7_inet_addr_t groupAddr;
  L7_inet_addr_t sourceAddr;

  inetAddressSet(L7_AF_INET, &inGroupAddr, &groupAddr);
  inetAddressSet(L7_AF_INET, &inSourceAddr, &sourceAddr);

  return(ptin_igmp_multicast_package_channels_remove(packageId,serviceId,&groupAddr,groupMask,&sourceAddr,sourceMask));
}

/**
 * @purpose Remove Multicast Channels from a Package
 * 
 * @param packageId 
 * @param serviceId 
 * @param groupAddr 
 * @param groupMask  
 * @param sourceAddr 
 * @param sourceMask  
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_multicast_package_channels_remove(L7_uint32 packageId, L7_uint32 serviceId, L7_inet_addr_t *groupAddr, L7_uint32 groupMask, L7_inet_addr_t *sourceAddr, L7_uint32 sourceMask)
{
  char                           groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                           sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
  L7_inet_addr_t                 groupAddrBase;
  L7_inet_addr_t                 sourceAddrBase;
  L7_inet_addr_t                 sourceAddrBaseIterator;
  L7_uint32                      noOfGroups = 0;
  L7_uint32                      noOfSources = 0;
  L7_uint32                      groupIterator = 0;
  L7_uint32                      sourceIterator = 0;
  RC_t                           rc = L7_SUCCESS;

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || serviceId >= PTIN_SYSTEM_N_EXTENDED_EVCS ||  groupAddr == L7_NULLPTR || sourceAddr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u serviceId:%u groupNetAddr:%p sourceNetAddr:%p]",packageId, serviceId, groupAddr, sourceAddr);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u serviceId:%u groupNetAddr:%s groupMask:%u sourceNetAddr:%s sourceMask:%u",
              packageId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), groupMask, inetAddrPrint(sourceAddr, sourceAddrStr), sourceMask);

  /*Creating the Multicast Package for the First Time*/
  if (multicastPackage[packageId].inUse == L7_FALSE)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Multicast Package Does Not Exists [packageId:%u serviceId:%u groupNetAddr:%s groupMask:%u sourceNetAddr:%s sourceMask:%u",
                 packageId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), groupMask, inetAddrPrint(sourceAddr, sourceAddrStr), sourceMask);
    return L7_NOT_EXIST;

  }
  else /*This Multicast Package Already Exists*/
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Multicast Package Does Exist [packageId:%u serviceId:%u groupNetAddr:%s groupMask:%u sourceNetAddr:%s sourceMask:%u",
              packageId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), groupMask, inetAddrPrint(sourceAddr, sourceAddrStr), sourceMask);
  }

  if (groupMask == 0)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "No Channels to Remove [packageId:%u serviceId:%u groupAddr:%s groupMask:%u sourceAddr:%s sourceMask:%u",
                 packageId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), groupMask, inetAddrPrint(sourceAddr, sourceAddrStr), sourceMask);
    return L7_SUCCESS;
  }

 /* Validate and prepare channel group Address*/
  if ( (rc = ptin_igmp_channel_to_netmask( groupAddr, groupMask, &groupAddrBase, &noOfGroups)) !=L7_SUCCESS || noOfGroups == 0 || noOfGroups > PTIN_IGMP_CHANNELS_MAX )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to obtain groupAddr:%s from groupMask:%u rc:%u!", inetAddrPrint(groupAddr, groupAddrStr), groupMask, rc);
    return L7_FAILURE;    
  }  
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to remove groupAddr:%s (%u addresses)", inetAddrPrint(&groupAddrBase, groupAddrStr), noOfGroups);

  /* Prepare source channel */
  if (L7_SUCCESS != (rc = ptin_igmp_channel_to_netmask( sourceAddr, sourceMask, &sourceAddrBase, &noOfSources)) || noOfSources == 0 || ( (noOfSources * noOfGroups) > PTIN_IGMP_CHANNELS_MAX))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to obtain sourceAddr:%s from sourceMask:%u rc:%u!", inetAddrPrint(sourceAddr, sourceAddrStr), sourceMask, rc);
    return L7_FAILURE;    
  }
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to remove sourceAddr:%s (%u addresses)", inetAddrPrint(&sourceAddrBase, sourceAddrStr), noOfSources);

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Maximum addresses to be removed: %u", noOfGroups*noOfSources);

  /* Remove channels */
  for ( groupIterator = 0; groupIterator < noOfGroups; groupIterator++ )
  {
    memcpy(&sourceAddrBaseIterator, &sourceAddrBase, sizeof(sourceAddrBaseIterator));

    for ( sourceIterator = 0; sourceIterator < noOfSources; sourceIterator++ )
    {
      /* Remove node from avl tree */
      if (rc == L7_SUCCESS)
      {
        if ( (rc = ptin_igmp_multicast_package_channel_remove(packageId, serviceId, &groupAddrBase, &sourceAddrBaseIterator)) != L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing channel from multicast package [packageId:%u serviceId:%u groupAddr:%s groupMask:%u sourceAddr:%s sourceMask:%u]",
                  packageId, serviceId, inetAddrPrint(&groupAddrBase, groupAddrStr), groupMask, inetAddrPrint(&sourceAddrBaseIterator, sourceAddrStr), sourceMask);
          return rc;
        }
        else
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removed channel from multicast package [packageId:%u serviceId:%u groupAddr:%s groupMask:%u sourceAddr:%s sourceMask:%u]",
                    packageId, serviceId, inetAddrPrint(&groupAddrBase, groupAddrStr), groupMask, inetAddrPrint(&sourceAddrBaseIterator, sourceAddrStr), sourceMask);
        }
      }

      /* Next source ip address */
      if (sourceAddrBaseIterator.family != L7_AF_INET6)
        sourceAddrBaseIterator.addr.ipv4.s_addr++;
      else
        break;
    }
    /* Next group address */
    if (groupAddrBase.family != L7_AF_INET6)
      groupAddrBase.addr.ipv4.s_addr++;
    else
      break;
  }
  return rc;
}

/**
 * @purpose Add Multicast Channel to a Package
 * 
 * @param packageId 
 * @param serviceId 
 * @param groupAddr 
 * @param sourceAddr 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
static RC_t ptin_igmp_multicast_package_channel_add(L7_uint32 packageId, L7_uint32 serviceId, L7_inet_addr_t *groupAddr, L7_inet_addr_t *sourceAddr)
{
  ptinIgmpChannelInfoData_t     *channelAvlTreeEntry = L7_NULLPTR;  
  char                           groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                           sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
  L7_uint32                      portIterator;
  L7_uint32                      noOfPortsFound = 0;
  L7_uint32                      groupClientIterator;
  L7_uint32                      noOfgroupClientsFound;
  ptinIgmpGroupClientInfoData_t *groupClientPtr;
  RC_t                           rc;

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || serviceId >= PTIN_SYSTEM_N_EXTENDED_EVCS ||  groupAddr == L7_NULLPTR || sourceAddr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u serviceId:%u groupAddr:%p sourceAddr:%p]",packageId, serviceId, groupAddr, sourceAddr);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u serviceId:%u groupAddr:%s sourceAddr:%s]",
              packageId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr));

  if (multicastPackage[packageId].inUse == L7_FALSE)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Multicast Package Does Not Exist [packageId:%u]", packageId);
    return L7_NOT_EXIST;   
  }

  /* Find Channel Entry */
  rc = ptin_igmp_channel_get (serviceId, groupAddr, sourceAddr,  &channelAvlTreeEntry );
  if ( rc == L7_NOT_EXIST )
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Channel Does Not Exist [serviceId:%u groupAddr:%s sourceAddr:%s]",
                serviceId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr));
    return L7_NOT_EXIST;      
  }
  else if (rc != L7_SUCCESS || channelAvlTreeEntry == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Search Channel Entry [packageId:%u serviceId:%u groupAddr:%s sourceAddr:%p channelEntry:%p]", packageId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr), channelAvlTreeEntry);    
    return rc;
  }

  if ( L7_SUCCESS != (rc = queue_channel_entry_add(packageId, channelAvlTreeEntry) ) )
  {
    if (L7_ALREADY_CONFIGURED != rc)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Add Channel to Queue [packageId:%u serviceId:%u groupAddr:%s sourceAddr:%s]",packageId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr));    
      return rc;
    }
    else
    {
      //Channel Already Added to Package
      return L7_SUCCESS;
    }
  }

  if ( multicastPackage[packageId].noOfPorts != 0 )
  {
    for ( portIterator = 0; portIterator < PTIN_SYSTEM_N_UPLINK_INTERF && noOfPortsFound < multicastPackage[packageId].noOfPorts; portIterator++)
    {
      /*Check if this position on the Client Array is Empty*/
      if (IS_BITMAP_WORD_SET(multicastPackage[packageId].portBmp, portIterator, UINT32_BITSIZE) == L7_FALSE)
      {
        //Next Position on the Array of Ports. -1 since the for adds 1 unit.
        portIterator += UINT32_BITSIZE - 1;
        continue;
      }

      if (IS_BITMAP_BIT_SET(multicastPackage[packageId].portBmp, portIterator, UINT32_BITSIZE) == L7_FALSE)
      {
        //Move to the Next Port Id
        continue;
      }

      //Increment the Number of Ports Found
      noOfPortsFound++;

      //Reset the No Of Group Clients Found
      noOfgroupClientsFound = 0;

      for ( groupClientIterator = 0; 
          groupClientIterator < PTIN_IGMP_CLIENTIDX_MAX && noOfgroupClientsFound < multicastPackage[packageId].noOfGroupClientsPerPort[portIterator]; 
          groupClientIterator++)
      {
        /*Check if this position on the Client Array is Empty*/
        if (IS_BITMAP_WORD_SET(multicastPackage[packageId].groupClientBmpPerPort[portIterator], groupClientIterator, UINT32_BITSIZE) == L7_FALSE)
        {
          //Next Position on the Array of Clients. -1 since the for adds 1 unit.
          groupClientIterator += UINT32_BITSIZE - 1;
          continue;
        }

        if (IS_BITMAP_BIT_SET(multicastPackage[packageId].groupClientBmpPerPort[portIterator], groupClientIterator, UINT32_BITSIZE) == L7_FALSE)
        {
          //Move to the Next Group Client Id
          continue;
        }

        //Increment the Number of Group Clients
        noOfgroupClientsFound++;

        if ( (groupClientPtr = groupClientId2groupClientPtr(portIterator, groupClientIterator)) == L7_NULLPTR )
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to get groupClientPtr [ptinPort:%u clientId:%u serviceId:%u groupAddr:%s sourceAddr:%s]",
                  portIterator, groupClientIterator, serviceId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr));
          return L7_FAILURE;
        }

        if ( (rc = ptin_igmp_multicast_channel_client_add(packageId, groupClientPtr, channelAvlTreeEntry)) != L7_SUCCESS )
        {
          //Error Already Logged
          return rc;
        }
      }
    }
  }
  return L7_SUCCESS;
}

/**
 * @purpose Remove Multicast Channel from a Package
 * 
 * @param packageId 
 * @param serviceId 
 * @param groupAddr 
 * @param sourceAddr 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
static RC_t ptin_igmp_multicast_package_channel_remove(L7_uint32 packageId, L7_uint32 serviceId, L7_inet_addr_t *groupAddr, L7_inet_addr_t *sourceAddr)
{
  ptinIgmpChannelInfoData_t           *channelAvlTreeEntry;
  char                                 groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                                 sourceAddrStr[IPV6_DISP_ADDR_LEN]={}; 
  RC_t                                 rc;

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || serviceId >= PTIN_SYSTEM_N_EXTENDED_EVCS ||  groupAddr == L7_NULLPTR || sourceAddr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u serviceId:%u groupAddr:%p sourceAddr:%p]",packageId, serviceId, groupAddr, sourceAddr);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u serviceId:%u groupAddr:%s sourceAddr:%s]",
              packageId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr));

  if (multicastPackage[packageId].inUse == L7_FALSE)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Multicast Package Does Not Exist [packageId:%u]", packageId);
#if 0//Do not return this rc to caller
    return L7_NOT_EXIST;
#else
    return L7_SUCCESS;
#endif     
  }

  /* Find Channel Entry */
  rc = ptin_igmp_channel_get (serviceId, groupAddr, sourceAddr,  &channelAvlTreeEntry );
  if ( rc == L7_NOT_EXIST )
  {
    /*Exit Here No More Internal Identifers Left and Entry Not Found*/
    if (ptin_debug_igmp_snooping)
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Entry Does Not Exist [packageId:%u  serviceId:%u groupAddr:%s sourceAddr:%s]",
                packageId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr));
#if 0//Do not return this rc to caller
    return L7_NOT_EXIST;
#else
    return L7_SUCCESS;
#endif   
  }
  else if (rc != L7_SUCCESS || channelAvlTreeEntry == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Search Channel Entry [packageId:%u serviceId:%u groupAddr:%s sourceAddr:%p channelEntry:%p]", packageId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr), channelAvlTreeEntry);    
    return L7_FAILURE;
  }

  if ( multicastPackage[packageId].noOfPorts != 0 )
  {
    L7_uint32                            portIterator;
    L7_uint32                            noOfPortsFound = 0;
    L7_uint32                            groupClientIterator;
    L7_uint32                            noOfgroupClientsFound;
    ptinIgmpGroupClientInfoData_t       *groupClientPtr;

    for ( portIterator = 0; portIterator < PTIN_SYSTEM_N_UPLINK_INTERF && noOfPortsFound < multicastPackage[packageId].noOfPorts; portIterator++)
    {
      /*Check if this position on the Client Array is Empty*/
      if (IS_BITMAP_WORD_SET(multicastPackage[packageId].portBmp, portIterator, UINT32_BITSIZE) == L7_FALSE)
      {
        //Next Position on the Array of Ports. -1 since the for adds 1 unit.
        portIterator += UINT32_BITSIZE - 1;
        continue;
      }

      if (IS_BITMAP_BIT_SET(multicastPackage[packageId].portBmp, portIterator, UINT32_BITSIZE) == L7_FALSE)
      {
        //Move to the Next Port Id
        continue;
      }

      //Increment the Number of Ports Found
      noOfPortsFound++;

      //Reset the No Of Group Clients Found
      noOfgroupClientsFound = 0;

      for ( groupClientIterator = 0; 
          groupClientIterator < PTIN_IGMP_CLIENTIDX_MAX && noOfgroupClientsFound < multicastPackage[packageId].noOfGroupClientsPerPort[portIterator]; 
          groupClientIterator++)
      {
        /*Check if this position on the Client Array is Empty*/
        if (IS_BITMAP_WORD_SET(multicastPackage[packageId].groupClientBmpPerPort[portIterator], groupClientIterator, UINT32_BITSIZE) == L7_FALSE)
        {
          //Next Position on the Array of Clients. -1 since the for adds 1 unit.
          groupClientIterator += UINT32_BITSIZE - 1;
          continue;
        }

        if (IS_BITMAP_BIT_SET(multicastPackage[packageId].groupClientBmpPerPort[portIterator], groupClientIterator, UINT32_BITSIZE) == L7_FALSE)
        {
          //Move to the Next Group Client Id
          continue;
        }

        //Increment the Number of Group Clients
        noOfgroupClientsFound++;

        if ( (groupClientPtr = groupClientId2groupClientPtr(portIterator, groupClientIterator)) == L7_NULLPTR )
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to get groupClientPtr [ptinPort:%u clientId:%u serviceId:%u groupAddr:%s sourceAddr:%s]",
                  portIterator, groupClientIterator, serviceId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr));
          return L7_FAILURE;
        }

        if ( (rc = ptin_igmp_multicast_channel_client_remove(packageId, groupClientPtr, channelAvlTreeEntry)) != L7_SUCCESS )
        {
          //Error Already Logged
          return rc;
        }
      }
    }
  }

  if ( L7_SUCCESS != (rc = queue_channel_entry_remove(packageId, channelAvlTreeEntry) ) )
  {
    if ( L7_NOT_EXIST != rc)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Remove Channel from Queue [packageId:%u serviceId:%u groupAddr:%s sourceAddr:%s]",packageId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr));    
      return rc;
    }
    else
    {
      //Channel Entry Already Remove
      return L7_SUCCESS;
    }
  }

  return L7_SUCCESS;
}

/***************************************Queue Channel Routines*********************************************/

static struct channelPoolEntry_s* queue_channel_entry_get_next(L7_uint32 packageId, struct channelPoolEntry_s *channelEntry)
{
  ;

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u]", packageId);    
    return L7_NULLPTR;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u channelEntry:%p]", packageId, channelEntry);

  if (multicastPackage[packageId].queueChannel.n_elems == 0)
  {
    return L7_NULLPTR;
  }
  else
  {
    if ( channelEntry == L7_NULLPTR )
    {
      return( (struct channelPoolEntry_s*) multicastPackage[packageId].queueChannel.head);
    }
    else
    {
      return(channelEntry->next);
    }
  }
}

static RC_t queue_channel_entry_find(L7_uint32 packageId, ptinIgmpChannelInfoData_t *channelAvlTreeEntry, struct channelPoolEntry_s **channelEntry)
{
  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || channelAvlTreeEntry == L7_NULLPTR  || channelEntry ==  L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u channelAvlTreeEntry:%p channelEntry:%u]", packageId, channelAvlTreeEntry, channelEntry);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u channelAvlTreeEntry:%p channelEntry:%u]",
              packageId, channelAvlTreeEntry, channelEntry);

  if (multicastPackage[packageId].queueChannel.n_elems != 0)
  {
    *channelEntry = (struct channelPoolEntry_s*) multicastPackage[packageId].queueChannel.head;
    while ( *channelEntry != L7_NULLPTR)
    {
      if ( (*channelEntry)->channelAvlTreeEntry == channelAvlTreeEntry )
      {
        /*Output Parameters*/
        if (ptin_debug_igmp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [channelEntryId:%u]",
                    (*channelEntry)->channelEntryId);

        return L7_SUCCESS;
      }
      *channelEntry = (*channelEntry)->next;
    }
  }

  return L7_NOT_EXIST;
}

static RC_t queue_channel_entry_add(L7_uint32 packageId, ptinIgmpChannelInfoData_t *channelAvlTreeEntry)
{
  struct channelPoolEntryId_s   *channelEntryId = L7_NULLPTR;
  struct channelPoolEntry_s     *channelEntry;
  RC_t                           rc;

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || channelAvlTreeEntry == L7_NULLPTR  )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u channelAvlTreeEntry:%p]",packageId, channelAvlTreeEntry);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u channelAvlTreeEntry:%p]",
              packageId, channelAvlTreeEntry);

  /*Attach this package on this channel*/
  if ( L7_SUCCESS != (rc = queue_package_entry_add(packageId, channelAvlTreeEntry ) ) )
  {
    if (L7_ALREADY_CONFIGURED != rc)
    {
      //Error Already Logged
      return rc;
    }
    else
    {
      //Package Already Added To Channel
      return L7_SUCCESS;
    }
  }

  /* Check if queue has free elements */
  if (queueFreeChannelId.n_elems == 0)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Queue is empty!");
    return L7_NO_MEMORY;
  }

#if 0 //This verification is not required, if we perform the attach of this package before.
  rc = queue_channel_entry_find(packageId, channelAvlTreeEntry, &channelEntry);
  if (rc == L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Channel Entry Already Added");
    return L7_ALREADY_CONFIGURED;    
  }
  else
  {
    if (rc != L7_NOT_EXIST)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Find Channel Entry [packageId:%u channelAvlTreeEntry:%p]",packageId, channelAvlTreeEntry);    
      return L7_FAILURE;
    }
  }
#endif

  /* Pop Channel Pool Entry Id*/
  rc = dl_queue_remove_head(&queueFreeChannelId, (dl_queue_elem_t **) &channelEntryId);
  if (rc != NOERR || channelEntryId == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error removing element from free quueue: rc=%u channelEntryId:%p!", rc, channelEntryId);
    return rc;
  }

  /*Get Channel Pool Entry*/
  channelEntry = &channelPoolEntry[channelEntryId->entryId];

  /*Save Channel Avl Tree Entry*/
  channelEntry->channelAvlTreeEntry = channelAvlTreeEntry;  

  /*Save Channel Entry Id*/
  channelEntry->channelEntryId = channelEntryId->entryId;  

  /*Push Channel Entry*/
  rc = dl_queue_add_tail(&multicastPackage[packageId].queueChannel, (dl_queue_elem_t *) channelEntry);
  if (rc != L7_SUCCESS)
  {
    memset(channelEntryId, 0x00, sizeof(*channelEntryId));
    dl_queue_add_head(&queueFreeChannelId, (dl_queue_elem_t *) channelEntryId);
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error adding element to queue rc=%u!", rc);
    return rc;
  }

  /*Output Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [channelEntryId:%u]",
              channelEntry->channelEntryId);

  return L7_SUCCESS;
}

static RC_t queue_channel_entry_remove(L7_uint32 packageId, ptinIgmpChannelInfoData_t *channelAvlTreeEntry)
{
  struct channelPoolEntryId_s   *channelEntryId;
  struct channelPoolEntry_s     *channelEntry;    
  RC_t                           rc;

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || channelAvlTreeEntry == L7_NULLPTR  )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u channelAvlTreeEntry:%p]",packageId, channelAvlTreeEntry);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u channelAvlTreeEntry:%p]",
              packageId, channelAvlTreeEntry);

  /* Check if queue has free elements */
  if (queueFreeChannelId.n_elems >= PTIN_SYSTEM_IGMP_MAXPACKAGES_CHANNEL_ENTRIES)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"All elements already removed (Free Queue is full: n_elems:%u)!", queueFreeChannelId.n_elems);
    return L7_NOT_EXIST;    
  }

  /*Detach this package from this channel*/
  if ( L7_SUCCESS != (rc = queue_package_entry_remove(packageId, channelAvlTreeEntry) ) )
  {
    if ( L7_NOT_EXIST != rc)
    {
      //Error Already Logged
      return rc;
    }
    else
    {
      //Package Entry Already Remove
      return L7_SUCCESS;
    }
  }

  rc = queue_channel_entry_find(packageId, channelAvlTreeEntry, &channelEntry);
  if (rc != L7_SUCCESS)
  {
    if (rc == L7_NOT_EXIST)
    {
      if (ptin_debug_igmp_snooping)
        LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Channel Entry Does Not Exist [packageId:%u channelAvlTreeEntry:%p]",packageId, channelAvlTreeEntry);
      return L7_NOT_EXIST;      
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to find element in queue: rc=%u!", rc);
      return rc;
    }
  }

  /* Pop Package Entry */
  rc = dl_queue_remove(&multicastPackage[packageId].queueChannel, (dl_queue_elem_t *) channelEntry);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error removing element from queue: rc=%u!", rc);
    return rc;
  }

  /*Get Channel Entry Id*/
  channelEntryId = &channelIdPoolEntry[channelEntry->channelEntryId];

  /* Push Package Entry Id*/
  rc = dl_queue_add_tail(&queueFreeChannelId, (dl_queue_elem_t *) channelEntryId);
  if (rc != L7_SUCCESS)
  {
    dl_queue_add_head(&multicastPackage[packageId].queueChannel, (dl_queue_elem_t *) channelEntry);
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error adding element to free queue: rc=%u!", rc);
    return rc;
  }

  /* Clear Package Entry */
  memset(channelEntry, 0x00, sizeof(*channelEntry));

  /*Output Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [channelEntryId:%u]",
              channelEntry->channelEntryId);

  return L7_SUCCESS;
}
/****************************************End Queue Channel Routines***********************************/


/***************************************Queue Package Routines*********************************************/
static struct packagePoolEntry_s* queue_package_entry_get_next(ptinIgmpChannelInfoData_t *channelAvlTreeEntry, struct packagePoolEntry_s  *packageEntry)
{
  ;

  /* Input Argument validation */
  if ( channelAvlTreeEntry == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [channelAvlTreeEntry:%p]", channelAvlTreeEntry);    
    return L7_NULLPTR;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [channelAvlTreeEntry:%p packageEntry:%p]", channelAvlTreeEntry, packageEntry);

  if (channelAvlTreeEntry->queuePackage.n_elems == 0)
  {
    return L7_NULLPTR;
  }
  else
  {
    if ( packageEntry == L7_NULLPTR )
    {
      return( (struct packagePoolEntry_s*) channelAvlTreeEntry->queuePackage.head);
    }
    else
    {
      return(packageEntry->next);
    }
  }
}

static RC_t queue_package_entry_find(L7_uint32 packageId, ptinIgmpChannelInfoData_t *channelAvlTreeEntry, struct packagePoolEntry_s  **packageEntry )
{
  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || channelAvlTreeEntry == L7_NULLPTR  || packageEntry ==  L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u channelAvlTreeEntry:%p channelEntry:%u]", packageId, channelAvlTreeEntry, packageEntry);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u channelAvlTreeEntry:%p channelEntry:%u]",
              packageId, channelAvlTreeEntry, packageEntry);

  if (channelAvlTreeEntry->queuePackage.n_elems != 0)
  {
    *packageEntry = (struct packagePoolEntry_s*) channelAvlTreeEntry->queuePackage.head;
    while ( *packageEntry != L7_NULLPTR)
    {
      if ( (*packageEntry)->packageId == packageId )
      {
        /*Output Parameters*/
        if (ptin_debug_igmp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [packageEntryId:%u]",
                    (*packageEntry)->packageEntryId);

        return L7_SUCCESS;

      }
      *packageEntry = (*packageEntry)->next;
    }
  }
  return L7_NOT_EXIST;
}

static RC_t queue_package_entry_add(L7_uint32 packageId, ptinIgmpChannelInfoData_t *channelAvlTreeEntry)
{
  struct packagePoolEntryId_s   *packageEntryId;  
  struct packagePoolEntry_s     *packageEntry;  
  RC_t                           rc;

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || channelAvlTreeEntry == L7_NULLPTR  )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u channelAvlTreeEntry:%p]",packageId, channelAvlTreeEntry);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u channelAvlTreeEntry:%p]",
              packageId, channelAvlTreeEntry);

  rc = queue_package_entry_find(packageId, channelAvlTreeEntry, &packageEntry);
  if (rc == L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Package Entry Already Added");
    return L7_ALREADY_CONFIGURED;    
  }
  else
  {
    if (rc != L7_NOT_EXIST)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Find Package Entry [packageId:%u channelAvlTreeEntry:%p]",packageId, channelAvlTreeEntry);    
      return L7_FAILURE;
    }
  }

  /* Check if queue has free elements */
  if (queueFreePackageId.n_elems == 0)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Queue is empty!");
    return L7_NO_MEMORY;
  }

  /* Pop Package Pool Entry Id*/
  rc = dl_queue_remove_head(&queueFreePackageId, (dl_queue_elem_t **) &packageEntryId);
  if (rc != NOERR || packageEntryId == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error removing element from free quueue: rc=%u packageEntryId:%p!", rc, packageEntryId);
    return rc;
  }

  /*Get Package Pool Entry*/
  packageEntry = &packagePoolEntry[packageEntryId->entryId];

  /*Save Package Id*/
  packageEntry->packageId = packageId;

  /*Save Package Entry Id*/
  packageEntry->packageEntryId = packageEntryId->entryId;

  /*Push Package Entry*/
  rc = dl_queue_add_tail(&channelAvlTreeEntry->queuePackage, (dl_queue_elem_t *) packageEntry);
  if (rc != L7_SUCCESS)
  {
    memset(packageEntryId, 0x00, sizeof(*packageEntryId));
    /*Push Package Id Entry*/
    dl_queue_add_head(&queueFreePackageId, (dl_queue_elem_t *) packageEntryId);
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error adding element to queue rc=%u!", rc);
    return rc;
  }

  /*Output Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [packageEntryId:%u]",
              packageEntryId->entryId);

  return L7_SUCCESS;
}

static RC_t queue_package_entry_remove(L7_uint32 packageId, ptinIgmpChannelInfoData_t *channelAvlTreeEntry)
{
  struct packagePoolEntryId_s   *packageEntryId;  
  struct packagePoolEntry_s     *packageEntry;    
  RC_t                           rc;

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || channelAvlTreeEntry == L7_NULLPTR  )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u channelAvlTreeEntry:%p]",packageId, channelAvlTreeEntry);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u channelAvlTreeEntry:%p]",
              packageId, channelAvlTreeEntry);

  /* Check if queue has free elements */
  if (queueFreePackageId.n_elems >= PTIN_SYSTEM_IGMP_MAXPACKAGES_CHANNEL_ENTRIES)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"All elements already removed (Free Queue is full: n_elems:%u)!", queueFreePackageId.n_elems);
    return L7_NOT_EXIST;    
  }

  rc = queue_package_entry_find(packageId, channelAvlTreeEntry, &packageEntry);  
  if (rc != L7_SUCCESS)
  {
    if (rc == L7_NOT_EXIST)
    {
      if (ptin_debug_igmp_snooping)
        LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Package Entry Does Not Exist [packageId:%u channelAvlTreeEntry:%p]",packageId, channelAvlTreeEntry);
      return L7_NOT_EXIST;      
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to find element in queue: rc=%u!", rc);
      return rc;
    }
  }

  /* Pop Package Entry */
  rc = dl_queue_remove(&channelAvlTreeEntry->queuePackage, (dl_queue_elem_t *) packageEntry);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error removing element from queue: rc=%u!", rc);
    return rc;
  }

  /*Get Package Entry Id*/
  packageEntryId = &packageIdPoolEntry[packageEntry->packageEntryId];

  /* Push Package Entry Id*/
  rc = dl_queue_add_tail(&queueFreePackageId, (dl_queue_elem_t *) packageEntryId);
  if (rc != L7_SUCCESS)
  {
    dl_queue_add_head(&channelAvlTreeEntry->queuePackage, (dl_queue_elem_t *) packageEntry);
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error adding element to queue: rc=%u", rc);
    return rc;
  }

  /* Clear Package Entry */
  memset(packageEntry, 0x00, sizeof(*packageEntry));

  return L7_SUCCESS;
}

/***************************************End Queue Package Routines*********************************************/

/**
 * @purpose Reset Multicast Service
 * 
 *
 * @notes none 
 *  
 */
void ptin_igmp_multicast_service_reset(void)  
{
  memset(&multicastServices, 0x00, sizeof(multicastServices));

  memset(&multicastServiceId, 0x00, sizeof(multicastServiceId));
}

/**
 * @purpose Dump Multicast Service
 * 
 *
 * @notes none 
 *  
 */
void ptin_igmp_multicast_service_dump(L7_uint32 ptinPort, L7_uint32 onuId, L7_uint32 serviceId)  
{
  L7_uint8 internalServiceId;

  /* Input Argument validation */
  if ( ptinPort >= PTIN_SYSTEM_N_UPLINK_INTERF || onuId >= PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF || serviceId >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u onuId:%u serviceId:%u]",ptinPort, onuId, serviceId);    
    return;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [ptinPort:%u onuId:%u serviceId:%u]",
              ptinPort, onuId, serviceId);

  if ( (internalServiceId = ptin_igmp_admission_control_multicast_internal_id_get(serviceId)) >= PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Obtain Internal Multicast Identifier [ptinPort:%u onuId:%u serviceId:%u internalServiceId:%u]",ptinPort, onuId, serviceId, internalServiceId);    
    return;
  }

  printf("ptinPort:%u onuId:%u serviceId:%u internalServiceId:%u noOfMulticastServices:%u\n",
         ptinPort, onuId, serviceId, internalServiceId, multicastServices[ptinPort][onuId].noOfMulticastServices);

  fflush(stdout);  

  return;
}

/**
 * @purpose Dump Active Multicast Service
 * 
 *
 * @notes none 
 *  
 */
void ptin_igmp_multicast_service_dump_active(void)  
{
  L7_uint8 internalServiceId;
  L7_uint32 ptinPort;
  L7_uint32 onuId;

  for (ptinPort = 0; ptinPort<PTIN_SYSTEM_N_UPLINK_INTERF; ptinPort++)
  {
    for (onuId = 0; onuId<PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF; onuId++)
    {
      if (!multicastServices[ptinPort][onuId].noOfMulticastServices)
        continue;

      printf("ptinPort:%u onuId:%u noOfMulticastServices:%u\n",
               ptinPort, onuId, multicastServices[ptinPort][onuId].noOfMulticastServices);

      for (internalServiceId = 0; internalServiceId<PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID; internalServiceId++)
      {
        if (!multicastServiceId[ptinPort][onuId][internalServiceId].inUse)
          continue;
        printf("  internalServiceId:%u serviceId:%u \n",
               internalServiceId, multicastServiceId[ptinPort][onuId][internalServiceId].serviceId);
      }
    }
  }

  fflush(stdout);  

  return;
}

/**
 * @purpose Dump All Multicast Service
 * 
 *
 * @notes none 
 *  
 */
void ptin_igmp_multicast_service_dump_all(void)  
{
  L7_uint8 internalServiceId;
  L7_uint32 ptinPort;
  L7_uint32 onuId;

  for (ptinPort = 0; ptinPort<PTIN_SYSTEM_N_UPLINK_INTERF; ptinPort++)
  {
    for (onuId = 0; onuId<PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF; onuId++)
    {
      printf("ptinPort:%u onuId:%u noOfMulticastServices:%u\n",
               ptinPort, onuId, multicastServices[ptinPort][onuId].noOfMulticastServices);
      for (internalServiceId = 0; internalServiceId<PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID; internalServiceId++)
      {        
        printf("  internalServiceId:%u inUse:%u serviceId:%u\n",
               internalServiceId, multicastServiceId[ptinPort][onuId][internalServiceId].inUse, multicastServiceId[ptinPort][onuId][internalServiceId].serviceId);
      }
    }
  }

  fflush(stdout);  

  return;
}

/**
 * @purpose Add Multicast Service Identifier
 * 
 * @param ptinPort 
 * @param onuId 
 * @param serviceId  
 *  
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_multicast_service_add(L7_uint32 ptinPort, L7_uint32 onuId, L7_uint32 serviceId)  
{
  L7_uint8 internalServiceId;

  /* Input Argument validation */
  if ( ptinPort >= PTIN_SYSTEM_N_UPLINK_INTERF || onuId >= PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF || serviceId >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptin_port:%u onuId:%u serviceId:%u]",ptinPort, onuId, serviceId);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [ptinPort:%u onuId:%u serviceId:%u]",
              ptinPort, onuId, serviceId);

  if ( (internalServiceId = ptin_igmp_admission_control_multicast_internal_id_get(serviceId)) >= PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Obtain Internal Multicast Identifier [ptinPort:%u onuId:%u serviceId:%u internalServiceId:%u]",ptinPort, onuId, serviceId, internalServiceId);    
    return L7_FAILURE;
  }

  if ( multicastServiceId[ptinPort][onuId][internalServiceId].inUse == L7_TRUE )
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Multicast Service Already Added [ptinPort:%u onuId:%u serviceId:%u internalServiceId:%u noOfMulticastServices:%u]",
                 ptinPort, onuId, serviceId, internalServiceId, multicastServices[ptinPort][onuId].noOfMulticastServices);
#if 0//Do Not Signal this to the Caller
    return L7_ALREADY_CONFIGURED;
#else
    return L7_SUCCESS;
#endif
  }

  multicastServiceId[ptinPort][onuId][internalServiceId].inUse = L7_TRUE;

  multicastServiceId[ptinPort][onuId][internalServiceId].serviceId = serviceId;

  multicastServices[ptinPort][onuId].noOfMulticastServices++;

  /*Output Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [ptinPort:%u onuId:%u serviceId:%u internalServiceId:%u noOfMulticastServices:%u]",
              ptinPort, onuId, serviceId, internalServiceId, multicastServices[ptinPort][onuId].noOfMulticastServices);

  return L7_SUCCESS;
}

/**
 * @purpose Remove Multicast Service Identifier
 * 
 * @param ptinPort 
 * @param onuId 
 * @param serviceId  
 *  
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_multicast_service_remove(L7_uint32 ptinPort, L7_uint32 onuId, L7_uint32 serviceId)  
{
  L7_uint8 internalServiceId;

  /* Input Argument validation */
  if ( ptinPort >= PTIN_SYSTEM_N_UPLINK_INTERF || onuId >= PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF || serviceId >= PTIN_SYSTEM_N_EXTENDED_EVCS )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptinPort:%u onuId:%u serviceId:%u]",ptinPort, onuId, serviceId);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [ptinPort:%u onuId:%u serviceId:%u]",
              ptinPort, onuId, serviceId);

  if ( (internalServiceId = ptinIgmpAdmissionControlMulticastInternalServiceId[serviceId]) == (L7_uint8) -1 ||  multicastServiceId[ptinPort][onuId][internalServiceId].inUse == L7_FALSE )
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Multicast Service Does Not Exist [ptinPort:%u onuId:%u serviceId:%u internalServiceId:%u noOfMulticastServices:%u]",
                 ptinPort, onuId, serviceId, internalServiceId, multicastServices[ptinPort][onuId].noOfMulticastServices);
#if 0//Do Not Signal this to the Caller
    return L7_NOT_EXIST;
#else
    return L7_SUCCESS;
#endif
  }

  multicastServiceId[ptinPort][onuId][internalServiceId].inUse = L7_FALSE;

  ptin_igmp_admission_control_multicast_internal_id_unset(serviceId);

  if ( multicastServices[ptinPort][onuId].noOfMulticastServices > 0 )
    multicastServices[ptinPort][onuId].noOfMulticastServices--;

  /*Output Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [ptinPort:%u onuId:%u serviceId:%u internalServiceId:%u noOfMulticastServices:%u]",
              ptinPort, onuId, serviceId, internalServiceId, multicastServices[ptinPort][onuId].noOfMulticastServices);

  return L7_SUCCESS;
}

  #if 0
/**
 * @purpose Get Next Multicast Service Identifier
 * 
 * @param ptinPort 
 * @param onuId 
 * @param *serviceId  
 *  
 *  
 * @return RC_t
 *
 * @notes First entry is obtained with  serviceId = (L7_uint32) 
 *        -1
 *  
 */
RC_t ptin_igmp_multicast_service_get_next(L7_uint32 ptinPort, L7_uint32 onuId, L7_uint32 *serviceId)  
{
  L7_uint8 internalServiceId = (L7_uint8) -1;

  /* Input Argument validation */
  if ( ptinPort >= PTIN_SYSTEM_N_UPLINK_INTERF || onuId >= PTIN_SYSTEM_IGMP_MAXONUS_PER_INTF || serviceId == L7_NULLPTR )
  {
    if (serviceId == L7_NULLPTR)
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptinPort:%u onuId:%u serviceId:%p]",ptinPort, onuId, serviceId);
    else
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptinPort:%u onuId:%u]",ptinPort, onuId);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [ptinPort:%u onuId:%u serviceId:%u]",
              ptinPort, onuId, serviceId);

  if (*serviceId != (L7_uint32) -1)
  {
    if ( (internalServiceId = ptin_igmp_admission_control_multicast_internal_id_get(*serviceId)) >= PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Obtain Internal Multicast Identifier [ptin_port:%u onuId:%u serviceId:%u internalServiceId:%u]",ptinPort, onuId, serviceId, internalServiceId);    
      return L7_FAILURE;
    }

    while (++internalServiceId < PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID)
    {
      if ( multicastServiceId[ptinPort][onuId][internalServiceId].inUse == L7_FALSE )
        continue;

      *serviceId = multicastServiceId[ptinPort][onuId][internalServiceId].serviceId;

      /*Output Parameters*/
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [ptinPort:%u onuId:%u serviceId:%u (internalServiceId:%u) ]",
                  ptinPort, onuId, *serviceId, internalServiceId);

      return L7_SUCCESS;
    }
  }
  else
  {
    if ( multicastServices[ptinPort][onuId].noOfMulticastServices != 1)
    {
      internalServiceId = 0;
      while (++internalServiceId < PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID)
      {
        if ( multicastServiceId[ptinPort][onuId][internalServiceId].inUse == L7_FALSE )
          continue;

        *serviceId = multicastServiceId[ptinPort][onuId][internalServiceId].serviceId;

        /*Output Parameters*/
        if (ptin_debug_igmp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [ptinPort:%u onuId:%u serviceId:%u (internalServiceId:%u) ]",
                    ptinPort, onuId, *serviceId, internalServiceId);

        return L7_SUCCESS;
      }
    }
  }

  /*Output Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "No Active/More Multicast Services Exist [ptinPort:%u onuId:%u serviceId:%u (internalServiceId:%u) ]",
              ptinPort, onuId, *serviceId, internalServiceId);

  return L7_NO_VALUE;
}
  #endif

/**
 * @purpose Add a given client to a Multicast Channel Package 
 * 
 * @param ptinPort 
 * @param deviceClientId  Device Client Identifier 
 * @param groupAddr 
 * @param sourceAddr 
 * @param *serviceId :[Out] Multicast Service Identifier 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_multicast_channel_service_get(L7_uint32 ptinPort, L7_uint32 deviceClientId, L7_inet_addr_t *groupAddr, L7_inet_addr_t *sourceAddr, L7_uint32 *serviceId)
{
  char                            groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                            sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
  ptinIgmpChannelInfoData_t      *channelEntry;  
  L7_uint8                        internalServiceId = 0;
  L7_uint8                        onuId;
  L7_uint16                       groupClientId;
  L7_uint16                       noOfServicesFound = 0;
  L7_uint32                       serviceIdAux;
  ptinIgmpGroupClientInfoData_t  *groupClient = L7_NULLPTR;
  L7_uint8                        useDefaultMCService = L7_FALSE;
  RC_t                            rc;

  /* Input Argument validation */
  if ( ptinPort >= PTIN_SYSTEM_N_UPLINK_INTERF || deviceClientId >= PTIN_IGMP_CLIENTIDX_MAX ||  groupAddr == L7_NULLPTR || sourceAddr == L7_NULLPTR || serviceId == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptinPort:%u deviceClientId:%u groupAddr:%p sourceAddr:%p serviceId:%p]",ptinPort, deviceClientId, groupAddr, sourceAddr, serviceId);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [ptinPort:%u deviceClientId:%u groupAddr:%s sourceAddr:%s serviceId:%p]",
              ptinPort, deviceClientId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr), serviceId);

  if ( (groupClient = deviceClientId2groupClientPtr(ptinPort, deviceClientId)) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Obtain groupClient [ptinPort:%u deviceClientId:%u serviceId:%u groupAddr:%p sourceAddr:%p serviceId:%p groupClient:%p]",ptinPort, deviceClientId, groupAddr, sourceAddr, serviceId, groupClient);    
    return L7_FAILURE;
  }

  /*Save the ONU Id*/
  onuId = groupClient->onuId;

  /*Save the Group Client Id*/
  groupClientId = groupClient->groupClientId;

  if (multicastServices[ptinPort][onuId].noOfMulticastServices == 0)
  {
    /*Output Parameters*/  
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "No Multicast Service Configured [ptinPort:%u deviceClientId:%u onuId:%u groupAddr:%s sourceAddr:%s noOfMulticastServices:%u]",
                 ptinPort, deviceClientId, onuId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr), multicastServices[ptinPort][onuId].noOfMulticastServices);
    return L7_NOT_EXIST;
  }

  while (noOfServicesFound <  multicastServices[ptinPort][onuId].noOfMulticastServices )
  {
    if ( multicastServiceId[ptinPort][onuId][internalServiceId].inUse == L7_FALSE )
    {
      if (++internalServiceId >= PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID)
      {
        /*Exit Here No More Internal Identifers Left*/
        if (ptin_debug_igmp_snooping)
          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Entry Does Not Exist [ptinPort:%u deviceClientId:%u onuId:%u groupAddr:%s sourceAddr:%s internalServiceId:%u noOfMulticastServices:%u]",
                    ptinPort, deviceClientId, onuId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr), internalServiceId, multicastServices[ptinPort][onuId].noOfMulticastServices);
        return L7_NOT_EXIST;        
      }

      /*Next Multicast Service*/
      continue;
    }
//  else /*inUse= L7_TRUE*/


    /*Increase the Number of Multicast Services Found*/
    noOfServicesFound++;

    /*Save the Service Id*/
    serviceIdAux = multicastServiceId[ptinPort][onuId][internalServiceId].serviceId;


    if (useDefaultMCService == L7_FALSE && channelDB.default_evc_mc_is_in_use == L7_TRUE)
    {
      if ( channelDB.default_evc_mc == serviceIdAux)
      {
        /*Set Default Multicast Flag*/
        useDefaultMCService = L7_TRUE;
      }
    }

    /* Find Channel Entry */
    rc = ptin_igmp_channel_get (serviceIdAux, groupAddr, sourceAddr,  &channelEntry );

    if ( rc == L7_NOT_EXIST )
    {
      if (++internalServiceId >= PTIN_IGMP_MAX_MULTICAST_INTERNAL_SERVICE_ID)
      {
        /*Exit Here No More Internal Identifers Left and Entry Not Found*/
        if (ptin_debug_igmp_snooping)
          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Entry Does Not Exist [ptinPort:%u deviceClientId:%u onuId:%u groupAddr:%s sourceAddr:%s serviceId:%u internalServiceId:%u noOfMulticastServices:%u]",
                    ptinPort, deviceClientId, onuId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr), serviceIdAux, internalServiceId, multicastServices[ptinPort][onuId].noOfMulticastServices);
        return L7_NOT_EXIST;      
      }

      /*Next Multicast Service*/      
      continue;      
    }
    else if (rc != L7_SUCCESS || channelEntry == L7_NULLPTR )
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Search Channel Entry [ptinPort:%u deviceClientId:%u onuId:%u serviceId:%u groupAddr:%p sourceAddr:%p serviceId:%u internalServiceId:%u noOfMulticastServices:%u]", 
              ptinPort, deviceClientId, onuId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr), serviceIdAux, internalServiceId, multicastServices[ptinPort][onuId].noOfMulticastServices);    
      return L7_FAILURE;
    }

    /* Is clientId Not Set in the client bitmap */
    if (IS_BITMAP_BIT_SET(channelEntry->groupClientBmpPerPort[ptinPort], groupClientId, UINT32_BITSIZE) == L7_FALSE)
    {
      /*Next Multicast Service*/
      ++internalServiceId;  
      continue;
    }

    *serviceId = serviceIdAux;
    /*Exit Here Multicast Service Found*/
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Entry Does Exist [ptinPort:%u deviceClientId:%u groupClientId:%u onuId:%u groupAddr:%s sourceAddr:%s serviceId:%u (internalServiceId:%u)]",
                ptinPort, deviceClientId, groupClientId, onuId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr), *serviceId, internalServiceId);

    return L7_SUCCESS;    
  }

  if (useDefaultMCService == L7_TRUE)
  {
    *serviceId = channelDB.default_evc_mc;

    /*Exit Here Multicast Service Found*/
    if (ptin_debug_igmp_snooping)
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Entry Does Exist Going to Use Default MC Service [ptinPort:%u deviceClientId:%u groupClientId:%u onuId:%u groupAddr:%s sourceAddr:%s serviceId:%u]",
                ptinPort, deviceClientId, groupClientId, onuId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr), *serviceId);

    return L7_SUCCESS;

  }

  /*Output Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Entry Does Not Exist [ptinPort:%u deviceClientId:%u groupClientId:%u onuId:%u groupAddr:%s sourceAddr:%s internalServiceId:%u noOfMulticastServices:%u]",
              ptinPort, deviceClientId, groupClientId, onuId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr), internalServiceId, multicastServices[ptinPort][onuId].noOfMulticastServices);

  return L7_NOT_EXIST;  
}

/**
 * @purpose Add a given client to a Multicast IPv4 Channel  
 *  
 * @param packageId  
 * @param ptinPort 
 * @param groupClientId  Client Identifier
 * @param serviceId Multicast Service Identifier
 * @param groupAddr 
 * @param sourceAddr  
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_debug_multicast_channel_client_add(L7_uint32 packageId, L7_uint32 ptinPort, L7_uint32 groupClientId, L7_uint32 serviceId, L7_uint32 inGroupAddr, L7_uint32 inSourceAddr)
{
  L7_inet_addr_t                 groupAddr;
  L7_inet_addr_t                 sourceAddr;
  ptinIgmpChannelInfoData_t     *channelEntry = L7_NULLPTR;
  ptinIgmpGroupClientInfoData_t *groupClientPtr;
  char                           groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                           sourceAddrStr[IPV6_DISP_ADDR_LEN]={};

  inetAddressSet(L7_AF_INET, &inGroupAddr, &groupAddr);
  inetAddressSet(L7_AF_INET, &inSourceAddr, &sourceAddr);

  /* Find Channel Entry */
  if ( ptin_igmp_channel_get( serviceId, &groupAddr, &sourceAddr,  &channelEntry) != L7_SUCCESS || channelEntry == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to get channel [ptinPort:%u clientId:%u serviceId:%u groupAddr:%s sourceAddr:%s]",
            ptinPort, groupClientId, serviceId, inetAddrPrint(&groupAddr, groupAddrStr), inetAddrPrint(&sourceAddr, sourceAddrStr));
    return L7_FAILURE;
  }

  if ( (groupClientPtr = groupClientId2groupClientPtr(ptinPort, groupClientId)) == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to get groupClientPtr [ptinPort:%u clientId:%u serviceId:%u groupAddr:%s sourceAddr:%s]",
            ptinPort, groupClientId, serviceId, inetAddrPrint(&groupAddr, groupAddrStr), inetAddrPrint(&sourceAddr, sourceAddrStr));
    return L7_FAILURE;
  }

  return(ptin_igmp_multicast_channel_client_add(packageId, groupClientPtr, channelEntry));
}

/**
 * @purpose Add a given client to a Multicast Channel  
 *  
 * @param  packageId
 * @param *groupClient
 * @param *channelAvlTreeEntry 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_multicast_channel_client_add(L7_uint32 packageId, ptinIgmpGroupClientInfoData_t *groupClient, ptinIgmpChannelInfoData_t *channelAvlTreeEntry)
{
  char                            groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                            sourceAddrStr[IPV6_DISP_ADDR_LEN]={};

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || groupClient == L7_NULLPTR ||  channelAvlTreeEntry == L7_NULLPTR  )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u groupClientPtr:%p channelEntry:%p]",packageId, groupClient, channelAvlTreeEntry);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u ptinPort:%u groupClientId:%u serviceId:%u groupAddr:%s sourceAddr:%s]", packageId,
              groupClient->igmpClientDataKey.ptin_port, groupClient->groupClientId, channelAvlTreeEntry->channelDataKey.evc_mc, 
              inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_group, groupAddrStr), inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_source, sourceAddrStr));

  if (IS_BITMAP_BIT_SET(channelAvlTreeEntry->groupClientBmpPerPort[groupClient->igmpClientDataKey.ptin_port], groupClient->groupClientId, UINT32_BITSIZE) == L7_TRUE)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "This groupClient was already added to this channel [ptin_port:%u groupClientId:%u serviceId:%u groupAddr:%s sourceAddr:%s]", groupClient->igmpClientDataKey.ptin_port, groupClient->groupClientId, 
                 channelAvlTreeEntry->channelDataKey.evc_mc, inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_group, groupAddrStr), inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_source, sourceAddrStr));
#if 0//DO Not Signal this to the Caller
    return L7_ALREADY_CONFIGURED;    
#else
    return L7_SUCCESS;    
#endif    
  }

  if ( L7_SUCCESS != ptin_igmp_client_channel_conflict_validation(packageId, groupClient, channelAvlTreeEntry) )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Channel Conflict Found [ptinPort:%u clientId:%u serviceId:%u groupAddr:%s sourceAddr:%s]",
            groupClient->igmpClientDataKey.ptin_port, groupClient->groupClientId, channelAvlTreeEntry->channelDataKey.evc_mc, 
            inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_group, groupAddrStr), inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_source, sourceAddrStr));    
    return L7_FAILURE;
  }

  /* Set clientId in the client bitmap */
  BITMAP_BIT_SET(channelAvlTreeEntry->groupClientBmpPerPort[groupClient->igmpClientDataKey.ptin_port], groupClient->groupClientId, UINT32_BITSIZE);

  /*Increment the Number of Group Clients*/
  if (channelAvlTreeEntry->noOfGroupClientsPerPort[groupClient->igmpClientDataKey.ptin_port]++ == 0)
  {
    if (IS_BITMAP_BIT_SET(channelAvlTreeEntry->portBmp, groupClient->igmpClientDataKey.ptin_port, UINT32_BITSIZE) == L7_FALSE)
    {
      /* Set portId in the port bitmap */
      BITMAP_BIT_SET(channelAvlTreeEntry->portBmp, groupClient->igmpClientDataKey.ptin_port, UINT32_BITSIZE);

      /*Increment the Number of Used Ports*/
      channelAvlTreeEntry->noOfPorts++;    
    }
  }


  return L7_SUCCESS;
}

/**
 * @purpose Remove a given client from a Multicast IPv4 Channel 
 *  
 * @param packageId 
 * @param ptinPort 
 * @param groupClientId  Client Identifier
 * @param serviceId Multicast Service Identifier
 * @param groupAddr 
 * @param sourceAddr  
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_debug_multicast_channel_client_remove(L7_uint32 packageId, L7_uint32 ptinPort, L7_uint32 groupClientId, L7_uint32 serviceId, L7_uint32 inGroupAddr, L7_uint32 inSourceAddr)
{
  L7_inet_addr_t                 groupAddr;
  L7_inet_addr_t                 sourceAddr;
  ptinIgmpChannelInfoData_t     *channelEntry = L7_NULLPTR;
  ptinIgmpGroupClientInfoData_t *groupClientPtr;
  char                           groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                           sourceAddrStr[IPV6_DISP_ADDR_LEN]={};

  inetAddressSet(L7_AF_INET, &inGroupAddr, &groupAddr);
  inetAddressSet(L7_AF_INET, &inSourceAddr, &sourceAddr);

  /* Find Channel Entry */
  if ( ptin_igmp_channel_get( serviceId, &groupAddr, &sourceAddr,  &channelEntry) != L7_SUCCESS || channelEntry == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to get channel [ptinPort:%u clientId:%u serviceId:%u groupAddr:%s sourceAddr:%s]",
            ptinPort, groupClientId, serviceId, inetAddrPrint(&groupAddr, groupAddrStr), inetAddrPrint(&sourceAddr, sourceAddrStr));
    return L7_FAILURE;
  }

  if ( (groupClientPtr = groupClientId2groupClientPtr(ptinPort, groupClientId)) == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to get groupClientPtr [ptinPort:%u clientId:%u serviceId:%u groupAddr:%s sourceAddr:%s]",
            ptinPort, groupClientId, serviceId, inetAddrPrint(&groupAddr, groupAddrStr), inetAddrPrint(&sourceAddr, sourceAddrStr));
    return L7_FAILURE;
  }

  return(ptin_igmp_multicast_channel_client_remove(packageId, groupClientPtr, channelEntry));
}

/**
 * @purpose Remove a given client from Multicast Channel 
 * 
 * @param ptinPort 
 * @param groupClientId  Client Identifier
 * @param serviceId Multicast Service Identifier
 * @param groupAddr 
 * @param sourceAddr  
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_multicast_channel_client_remove(L7_uint32 packageId, ptinIgmpGroupClientInfoData_t *groupClient, ptinIgmpChannelInfoData_t *channelAvlTreeEntry)
{
  char                            groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                            sourceAddrStr[IPV6_DISP_ADDR_LEN]={};

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || groupClient == L7_NULLPTR ||  channelAvlTreeEntry == L7_NULLPTR  )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u groupClientPtr:%p channelEntry:%p]",packageId, groupClient, channelAvlTreeEntry);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u ptinPort:%u groupClientId:%u serviceId:%u groupAddr:%s sourceAddr:%s]", packageId,
              groupClient->igmpClientDataKey.ptin_port, groupClient->groupClientId, channelAvlTreeEntry->channelDataKey.evc_mc, 
              inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_group, groupAddrStr), inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_source, sourceAddrStr));


  if ( IS_BITMAP_BIT_SET(channelAvlTreeEntry->groupClientBmpPerPort[groupClient->igmpClientDataKey.ptin_port], groupClient->groupClientId, UINT32_BITSIZE) == L7_FALSE )
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "This groupClient does not exist on this channel [ptin_port:%u groupClientId:%u serviceId:%u groupAddr:%s sourceAddr:%s]", groupClient->igmpClientDataKey.ptin_port, groupClient->groupClientId, 
                channelAvlTreeEntry->channelDataKey.evc_mc, inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_group, groupAddrStr), inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_source, sourceAddrStr));    
#if 0//DO Not Signal this to the Caller
    return L7_NOT_EXIST;    
#else
    return L7_SUCCESS;    
#endif    
  }

  if ( L7_SUCCESS != ptin_igmp_client_channel_dependency_validation(packageId, groupClient, channelAvlTreeEntry) )
  {
    //Error Already Logged
#if 0//DO Not Signal this to the Caller
    return L7_DEPENDENCY_NOT_MET;    
#else
    return L7_SUCCESS;    
#endif    
  }

  /* Clear clientId in the client bitmap */
  BITMAP_BIT_CLR(channelAvlTreeEntry->groupClientBmpPerPort[groupClient->igmpClientDataKey.ptin_port], groupClient->groupClientId, UINT32_BITSIZE);

  /*Decrement the Number of Group Clients*/
  if (channelAvlTreeEntry->noOfGroupClientsPerPort[groupClient->igmpClientDataKey.ptin_port] > 0)
  {
    if (--channelAvlTreeEntry->noOfGroupClientsPerPort[groupClient->igmpClientDataKey.ptin_port] == 0)
    {
      if (IS_BITMAP_BIT_SET(channelAvlTreeEntry->portBmp, groupClient->igmpClientDataKey.ptin_port, UINT32_BITSIZE) == L7_TRUE)
      {
        /* Clear portId in the port bitmap */
        BITMAP_BIT_CLR(channelAvlTreeEntry->portBmp, groupClient->igmpClientDataKey.ptin_port, UINT32_BITSIZE);

        if (channelAvlTreeEntry->noOfPorts > 0)
        {
          /*Decrement the Number of Used Ports*/
          --channelAvlTreeEntry->noOfPorts;    
        }
      }
    }
  }

  return L7_SUCCESS;
}

/**
 * @purpose Remove a given client from Multicast Channel 
 * 
 * @param ptinPort      :Port Identifier using PTIN 
 *                      representation
 * @param groupClientId :Group Client Identifier
 * @param serviceId     :Multicast Service Identifier
 * @param groupAddr     :Group Address
 * @param sourceAddr    :Source Address
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_multicast_channel_client_get(L7_uint32 ptinPort, L7_uint32 groupClientId, L7_uint32 serviceId, L7_inet_addr_t *groupAddr, L7_inet_addr_t *sourceAddr, L7_BOOL *isClientSet)
{
  char                            groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                            sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
  ptinIgmpChannelInfoData_t      *channelEntry;

  /* Input Argument validation */
  if ( ptinPort >= PTIN_SYSTEM_N_UPLINK_INTERF || groupClientId >= PTIN_IGMP_CLIENTIDX_MAX ||  groupAddr == L7_NULLPTR || sourceAddr == L7_NULLPTR || isClientSet == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [ptinPort:%u clientId:%u serviceId:%u groupAddr:%p sourceAddr:%p isClientSet:%p]",ptinPort, groupClientId, serviceId, groupAddr, sourceAddr, isClientSet);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [ptinPort:%u clientId:%u serviceId:%u groupAddr:%s sourceAddr:%s isClientSet:%p]",
              ptinPort, groupClientId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr), isClientSet);

  /* Find Channel Entry */
  if ( ptin_igmp_channel_get( serviceId, groupAddr, sourceAddr,  &channelEntry) != L7_SUCCESS || channelEntry == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to get channel [ptinPort:%u clientId:%u serviceId:%u groupAddr:%s sourceAddr:%s isClientSet:%p]",
            ptinPort, groupClientId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr), isClientSet);
    return L7_FAILURE;
  }

  /* Get clientId in the client bitmap */
  *isClientSet = IS_BITMAP_BIT_SET(channelEntry->groupClientBmpPerPort[ptinPort], groupClientId, UINT32_BITSIZE);

  /*Output Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Output Parameters [ptinPort:%u clientId:%u serviceId:%u groupAddr:%s sourceAddr:%s isBitSet:%s]",
              ptinPort, groupClientId, serviceId, inetAddrPrint(groupAddr, groupAddrStr), inetAddrPrint(sourceAddr, sourceAddrStr), *isClientSet?"Yes":"No");

  return L7_SUCCESS;
}

/**
 * @purpose Add a Bitmap of Packages to a Given Group Client
 * 
 * @param packagePtr  :Pointer to a Bitmap of Packages
 * @param groupClient :Group Client Pointer
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
static RC_t ptin_igmp_multicast_client_packages_add(L7_uint32 *packagePtr, L7_uint32 noOfPackages, ptinIgmpGroupClientInfoData_t *groupClient)
{
  L7_uint32 packageId;
  L7_uint32 noOfPackagesFound = 0;
  RC_t      rc = L7_SUCCESS;

  /* Input Argument validation */
  if ( packagePtr == L7_NULLPTR || groupClient == L7_NULLPTR  )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packagePtr:%p groupClient:%p]",packagePtr, groupClient);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packagePtr:%p groupClient:%p]",
              packagePtr, groupClient);

  for (packageId = 0; packageId<= PTIN_SYSTEM_IGMP_MAXPACKAGES; packageId++)
  {
    //Move forward 32 bits if this byte is 0 (no packages)
    if (IS_BITMAP_WORD_SET(packagePtr, packageId, UINT32_BITSIZE) == L7_FALSE)
    {
      packageId += UINT32_BITSIZE -1; //Less one, because of the For cycle that increments also 1 unit.
      continue;
    }

    if (IS_BITMAP_BIT_SET(packagePtr, packageId, UINT32_BITSIZE))
    {
      if ( L7_SUCCESS != (rc = ptin_igmp_multicast_client_package_add(packageId, groupClient)) )
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to add package [packageId:%p ptin_port:%u groupClientId:%u]",packagePtr, groupClient->igmpClientDataKey.ptin_port, groupClient->groupClientId);    
        return rc;
      }

      if (++noOfPackagesFound >= noOfPackages)
      {
        break;
      }
    }
  }
  return rc;
}

/**
 * @purpose Remove a Bitmap of Packages from a Group Client
 * 
 * @param packagePtr   : Pointer to a Bitmap of Packages
 * @param noOfPackages : Number of Packages
 * @param groupClient  : Group Client Pointer
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
static RC_t ptin_igmp_multicast_client_packages_remove(L7_uint32 *packagePtr, L7_uint32 noOfPackages, ptinIgmpGroupClientInfoData_t *groupClient)
{
  L7_uint32 packageId;
  L7_uint32 noOfPackagesFound = 0;
  RC_t      rc = L7_SUCCESS;

  /* Input Argument validation */
  if ( packagePtr == L7_NULLPTR || groupClient == L7_NULLPTR  )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packagePtr:%p groupClient:%p]",packagePtr, groupClient);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packagePtr:%p groupClient:%p]",
              packagePtr, groupClient);

  if ( groupClient->number_of_packages != 0 )
  {
    for (packageId = 0; packageId<= PTIN_SYSTEM_IGMP_MAXPACKAGES; packageId++)
    {
      //Move forward 32 bits if this byt is 0 (no packages)
      if (IS_BITMAP_WORD_SET(packagePtr, packageId, UINT32_BITSIZE) == L7_FALSE)
      {
        packageId += UINT32_BITSIZE -1; //Less one, because of the For cycle that increments also 1 unit.
        continue;
      }

      if (IS_BITMAP_BIT_SET(packagePtr, packageId, UINT32_BITSIZE))
      {
        if ( L7_SUCCESS != (rc = ptin_igmp_multicast_client_package_remove(packageId, groupClient)) )
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to add package [packageId:%p ptin_port:%u groupClientId:%u]",packagePtr, groupClient->igmpClientDataKey.ptin_port, groupClient->groupClientId);    
          return rc;
        }

        if (++noOfPackagesFound >= noOfPackages)
        {
          break;
        }
      }
    }
  }
  else
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "This client does not have packages associated [packageId:%p ptin_port:%u groupClientId:%u]",packagePtr, groupClient->igmpClientDataKey.ptin_port, groupClient->groupClientId);    
    return rc;
  }
  return rc;
}

/**
 * @purpose Remove all packages from a given group client
 * 
 * @param groupClient : Group Client Pointer
   
 * @return RC_t
 *
 * @notes none 
 *  
 */
static RC_t ptin_igmp_multicast_client_packages_remove_all(ptinIgmpGroupClientInfoData_t *groupClient)
{
  /* Input Argument validation */
  if ( groupClient == L7_NULLPTR  )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [groupClient:%p]", groupClient);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [groupClient:%p]",
              groupClient);

  if (groupClient->number_of_packages != 0)
  {
    /*Remove All Packages of this Client*/
    ptin_igmp_multicast_client_packages_remove(groupClient->package_bmp_list, groupClient->number_of_packages, groupClient);
  }

  return L7_SUCCESS; 
}


/**
 * @purpose Add a package to a given Group Client Identifier
 * 
 * @param packageId      : Package Identifier 
 * @param ptinPort       : Ptin Port Id 
 * @param groupClientId  : Group Client Id 
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_debug_multicast_client_package_add(L7_uint32 packageId, L7_uint32 ptinPort, L7_uint32 groupClientId)
{
  return(ptin_igmp_multicast_client_package_add(packageId, igmpGroupClients.group_client[ptinPort][groupClientId]));
}

/**
 * @purpose Add a package to a given Group Client
 * 
 * @param packageId    : Package Identifier
 * @param groupClient  : Group Client Pointer
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
static RC_t ptin_igmp_multicast_client_package_add(L7_uint32 packageId, ptinIgmpGroupClientInfoData_t *groupClient)
{
  ptinIgmpChannelInfoData_t      *channelAvlTreeEntry;
  struct channelPoolEntry_s      *channelEntry = L7_NULLPTR;
  RC_t                            rc;

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || groupClient == L7_NULLPTR  )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u groupClient:%p]",packageId, groupClient);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u groupClient:%p]",
              packageId, groupClient);

  /*Let us Check if this Package is Configured*/
  if (multicastPackage[packageId].inUse == L7_FALSE )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "This package does not exist [packageId:%u]", packageId);    
    return L7_DEPENDENCY_NOT_MET;
  }

  /* Is this PackageId not set in the Bitmap*/
  if ( IS_BITMAP_BIT_SET(groupClient->package_bmp_list, packageId, UINT32_BITSIZE) == L7_TRUE )
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "This package was already added to this groupClient [packageId:%u ptin_port:%u groupClientId:%u]", packageId, groupClient->igmpClientDataKey.ptin_port, groupClient->groupClientId);
#if 0//DO Not Signal this to the Caller
    return L7_ALREADY_CONFIGURED;    
#else
    return L7_SUCCESS;    
#endif
  }

  while ( L7_NULLPTR != (channelEntry = queue_channel_entry_get_next(packageId, channelEntry)) && 
          L7_NULLPTR != (channelAvlTreeEntry = channelEntry->channelAvlTreeEntry))
  {
    if ( (rc = ptin_igmp_multicast_channel_client_add(packageId, groupClient, channelAvlTreeEntry)) != L7_SUCCESS )
    {
      //Error Already Logged
      return rc;
    }
  }

  /* Set packageId in the package bitmap */
  BITMAP_BIT_SET(groupClient->package_bmp_list, packageId, UINT32_BITSIZE);

  /*Increment the Number of Packages*/
  groupClient->number_of_packages++;

  /*Package Client Bit Map Manipulation*/
  {
    if ( IS_BITMAP_BIT_SET(multicastPackage[packageId].groupClientBmpPerPort[groupClient->igmpClientDataKey.ptin_port], groupClient->groupClientId, UINT32_BITSIZE) == L7_FALSE )
    {
      /* Set groupClientId bitmap for this package*/
      BITMAP_BIT_SET(multicastPackage[packageId].groupClientBmpPerPort[groupClient->igmpClientDataKey.ptin_port], groupClient->groupClientId, UINT32_BITSIZE);

      /*Increment the Number of Group Clients per Port for this package*/
      if ( multicastPackage[packageId].noOfGroupClientsPerPort[groupClient->igmpClientDataKey.ptin_port]++ == 0 &&
           IS_BITMAP_BIT_SET(multicastPackage[packageId].portBmp, groupClient->igmpClientDataKey.ptin_port, UINT32_BITSIZE) == L7_FALSE )
      {
        /* Set groupClientId in the group client bitmap */
        BITMAP_BIT_SET(multicastPackage[packageId].portBmp, groupClient->igmpClientDataKey.ptin_port, UINT32_BITSIZE);

        /*Increment the Number of Ports*/
        multicastPackage[packageId].noOfPorts++;
      }
    }
  }
  return L7_SUCCESS;
}

/**
 * @purpose Remove a package from a given Group Client 
 *          Identifier
 * 
 * @param packageId      : Package Identifier 
 * @param ptinPort       : Ptin Port Id
 * @param groupClientId  : Group Client Id
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_debug_multicast_client_package_remove(L7_uint32 packageId, L7_uint32 ptinPort, L7_uint32 groupClientId)
{
  return(ptin_igmp_multicast_client_package_remove(packageId, igmpGroupClients.group_client[ptinPort][groupClientId]));
}

/**
 * @purpose Remove a package from a given
 *          given Group Client
 * 
 * @param packageId    : Package Identifier
 * @param groupClient  : Group Client Pointer
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
static RC_t ptin_igmp_multicast_client_package_remove(L7_uint32 packageId, ptinIgmpGroupClientInfoData_t *groupClient)
{
  ptinIgmpChannelInfoData_t      *channelAvlTreeEntry = L7_NULLPTR;
  struct channelPoolEntry_s      *channelEntry = L7_NULLPTR;
  RC_t                            rc;

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || groupClient == L7_NULLPTR  )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u groupClient:%p]",packageId, groupClient);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u groupClient:%p]",
              packageId, groupClient);

  /*Let us Check if this Package is Configured*/
  if (multicastPackage[packageId].inUse == L7_FALSE )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "This package does not exist [packageId:%u]", packageId);    
    return L7_DEPENDENCY_NOT_MET;
  }

  /* Is this PackageId set in the Bitmap*/
  if ( IS_BITMAP_BIT_SET(groupClient->package_bmp_list, packageId, UINT32_BITSIZE) == L7_FALSE )
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "This package does not belong to this groupClient [packageId:%u ptin_port:%u groupClientId:%u]", packageId, groupClient->igmpClientDataKey.ptin_port, groupClient->groupClientId);
#if 0//DO Not Signal this to the Caller
    return L7_NOT_EXIST;    
#else
    return L7_SUCCESS;    
#endif
  }

  while ( L7_NULLPTR != (channelEntry = queue_channel_entry_get_next(packageId, channelEntry)) && 
          L7_NULLPTR != (channelAvlTreeEntry = channelEntry->channelAvlTreeEntry))
  {
    if ( (rc = ptin_igmp_multicast_channel_client_remove(packageId, groupClient, channelAvlTreeEntry)) != L7_SUCCESS )
    {
      //Error Already Logged
      return rc;
    }
  }

  /* Clear packageId in the package bitmap */
  BITMAP_BIT_CLR(groupClient->package_bmp_list, packageId, UINT32_BITSIZE);

  /*Decrement the Number of Packages*/
  if (groupClient->number_of_packages > 0)
  {
    groupClient->number_of_packages--;
  }
  else
  {
    /*We Should Log a Warning*/
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"The number of packages in this channel is already zero [packageId:%u ptinPort:%u clientId:%u]",
                packageId, groupClient->igmpClientDataKey.ptin_port, groupClient->groupClientId);    

  }

  if ( IS_BITMAP_BIT_SET(multicastPackage[packageId].groupClientBmpPerPort[groupClient->igmpClientDataKey.ptin_port], groupClient->groupClientId, UINT32_BITSIZE) == L7_TRUE )
  {
    /* Unset groupClientId in the group client bitmap */
    BITMAP_BIT_CLR(multicastPackage[packageId].groupClientBmpPerPort[groupClient->igmpClientDataKey.ptin_port], groupClient->groupClientId, UINT32_BITSIZE);

    /*Decrement the Number of Group Clients*/
    if (multicastPackage[packageId].noOfGroupClientsPerPort[groupClient->igmpClientDataKey.ptin_port]>0)
      multicastPackage[packageId].noOfGroupClientsPerPort[groupClient->igmpClientDataKey.ptin_port]--;


    if ( multicastPackage[packageId].noOfGroupClientsPerPort[groupClient->igmpClientDataKey.ptin_port] == 0 && 
         IS_BITMAP_BIT_SET(multicastPackage[packageId].portBmp, groupClient->igmpClientDataKey.ptin_port, UINT32_BITSIZE) == L7_TRUE )
    {
      /* Clear the groupClientId bit for this package */
      BITMAP_BIT_CLR(multicastPackage[packageId].portBmp, groupClient->igmpClientDataKey.ptin_port, UINT32_BITSIZE);

      /*Decrement the Number of Ports*/
      if (multicastPackage[packageId].noOfPorts > 0)
        multicastPackage[packageId].noOfPorts--;
    }
  }
  return L7_SUCCESS;
}

/**
 * @purpose Verify if a given package is added from a given 
 *          Group Client
 * 
 * @param packageId    : [in]  Package Identifier
 * @param groupClient  : [in]  Group Client Pointer 
 * @param isBitSet     : [out] Is Package Set
 *  
 * @return RC_t
 *
 * @notes none 
 *  
 */
RC_t ptin_igmp_multicast_client_package_get(L7_uint32 packageId, ptinIgmpGroupClientInfoData_t *groupClient, L7_BOOL *isBitSet)
{
  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || groupClient == L7_NULLPTR  || isBitSet == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u groupClient:%p isBitSet:%p]",packageId, groupClient);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u groupClient:%p isBitSet:%p]",
              packageId, groupClient);

  /*Let us Check if this Package is Configured*/
  if (multicastPackage[packageId].inUse == L7_FALSE )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "This package does not exist [packageId:%u]", packageId);    
    return L7_NOT_EXIST;
  }

  *isBitSet = IS_BITMAP_BIT_SET(groupClient->package_bmp_list, packageId, UINT32_BITSIZE);

  return L7_SUCCESS;
}


/**
 * @purpose Verify if a given channel exists in another package with a different multicast service
 * 
 * @param packageId               : [in]  Package Identifier
 * @param groupClient             : [in]  Group Client Pointer 
 * @param channelAvlTreeEntry     : [in]  Channel AVL Tree Pointer
 *  
 * @return RC_t                   
 *
 * @notes  
 *  
 */
static RC_t ptin_igmp_client_channel_conflict_validation(L7_uint32 packageId, ptinIgmpGroupClientInfoData_t *groupClient, ptinIgmpChannelInfoData_t  *channelAvlTreeEntry)
{
  L7_uint32 packageIdIterator;
  L7_uint32 noOfPackagesFound = 0;

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || groupClient == L7_NULLPTR  || channelAvlTreeEntry == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u groupClient:%p channelAvlTreeEntry:%p]",packageId, groupClient, channelAvlTreeEntry);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u groupClient:%p channelAvlTreeEntry:%p]",
              packageId, groupClient, channelAvlTreeEntry);

  if ( groupClient->number_of_packages == 0 )
  {
    /*No packages Associated to this client*/
    return L7_SUCCESS;
  }

  for (packageIdIterator = 0; packageIdIterator < PTIN_SYSTEM_IGMP_MAXPACKAGES; packageIdIterator++)
  {

    //Move forward 32 bits if this byte is 0 (no packages)
    if (IS_BITMAP_WORD_SET(groupClient->package_bmp_list, packageIdIterator, UINT32_BITSIZE) == L7_FALSE)
    {
      packageId += UINT32_BITSIZE -1; //Less one, because of the For cycle that increments also 1 unit.
      continue;
    }

    if ( IS_BITMAP_BIT_SET(groupClient->package_bmp_list, packageIdIterator, UINT32_BITSIZE) == L7_FALSE)
    {
      /*Move to Next Package*/
      continue;
    }

    /*Increment the Number of Packages Found*/
    ++noOfPackagesFound;

    if (packageIdIterator == packageId)
    {
      if ( noOfPackagesFound >= groupClient->number_of_packages )
      {
        /*Conflicts Not Found!*/
        return L7_SUCCESS;
      }
      /*Move to Next Package*/
      continue;
    }

    if ( multicastPackage[packageIdIterator].queueChannel.n_elems == 0 )
    {
      if ( noOfPackagesFound >= groupClient->number_of_packages )
      {
        /*Conflicts Not Found!*/
        return L7_SUCCESS;
      }
    }

    /*Conflict Found*/
    if (L7_SUCCESS != ptin_igmp_package_channel_conflict_validation(packageIdIterator, channelAvlTreeEntry))
    {
      return L7_FAILURE;
    }

    if ( noOfPackagesFound >= groupClient->number_of_packages )
    {
      /*Conflicts Not Found!*/
      return L7_SUCCESS;
    }
  }

  return L7_SUCCESS;
}

/**
 * @purpose Verify if this client can be or not removed from this channel: i.e. this client has another package with this channel 
 * 
 * @param packageId               : [in]  Package Identifier
 * @param groupClient             : [in]  Group Client Pointer 
 * @param channelAvlTreeEntry     : [in]  Channel AVL Tree Pointer
 *  
 * @return RC_t                   
 *
 * @notes  
 *  
 */
static RC_t ptin_igmp_client_channel_dependency_validation(L7_uint32 packageId, ptinIgmpGroupClientInfoData_t *groupClient, ptinIgmpChannelInfoData_t  *channelAvlTreeEntry)
{
  struct packagePoolEntry_s    *packageEntry = L7_NULLPTR;
  L7_uint32                     packageIdAux;
  char                          groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                          sourceAddrStr[IPV6_DISP_ADDR_LEN]={};

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || groupClient == L7_NULLPTR  || channelAvlTreeEntry == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u groupClient:%p channelAvlTreeEntry:%p]",packageId, groupClient, channelAvlTreeEntry);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u groupClient:%p channelAvlTreeEntry:%p]",
              packageId, groupClient, channelAvlTreeEntry);

  if ( groupClient->number_of_packages <= 1 )
  {
    /*No more packages Associated to this client*/
    return L7_SUCCESS;
  }

  if (channelAvlTreeEntry->queuePackage.n_elems <= 1 )
  {
    /*No more packages associated to this channel*/
    return L7_SUCCESS;
  }

  while ( L7_NULLPTR != (packageEntry = queue_package_entry_get_next(channelAvlTreeEntry, packageEntry)) && 
          (packageIdAux = packageEntry->packageId) < PTIN_SYSTEM_IGMP_MAXPACKAGES)
  {
    if ( packageIdAux ==  packageId )
    {
      /*Move to next Channel*/
      continue;
    }

    if ( IS_BITMAP_BIT_SET(groupClient->package_bmp_list, packageIdAux, UINT32_BITSIZE) == L7_FALSE )
    {
      /*Move to Next Package*/
      continue;
    }

    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Dependency not met: Client set within two packages for this channel [ptinPort:%u clientId:%u serviceId:%u groupAddr:%s sourceAddr:%s][packageId1:%u packageId2:%u]",
                groupClient->igmpClientDataKey.ptin_port, groupClient->groupClientId, channelAvlTreeEntry->channelDataKey.evc_mc, 
                inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_group, groupAddrStr), inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_source, sourceAddrStr),
                packageId, packageIdAux);    
    return L7_DEPENDENCY_NOT_MET;

  }
  return L7_SUCCESS;
}

/**
 * @purpose Verify if this channel can be added to this package: i.e. this channel already exists in this package, but it belongs to a different multicast service
 * 
 * @param packageId               : [in]  Package Identifier * 
 * @param channelAvlTreeEntry     : [in]  Channel AVL Tree Pointer
 *  
 * @return RC_t                   
 *
 * @notes  
 *  
 */
static RC_t ptin_igmp_package_channel_conflict_validation(L7_uint32 packageId, ptinIgmpChannelInfoData_t  *channelAvlTreeEntry)
{
  struct channelPoolEntry_s *channelEntry = L7_NULLPTR;
  ptinIgmpChannelInfoData_t *channelAvlTreeEntryAux;  
  struct packagePoolEntry_s *packageEntry;  
  char                       groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                       sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
  RC_t                       rc;

  /* Input Argument validation */
  if ( packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES || channelAvlTreeEntry == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments [packageId:%u channelAvlTreeEntry:%p]",packageId, channelAvlTreeEntry);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Parameters [packageId:%u channelAvlTreeEntry:%p]",
              packageId, channelAvlTreeEntry);

  rc = queue_package_entry_find(packageId, channelAvlTreeEntry, &packageEntry);
  if (rc == L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Package Entry Already Added");
    return rc;    
  }
  else
  {
    if (rc != L7_NOT_EXIST)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Find Package Entry [packageId:%u channelAvlTreeEntry:%p]",packageId, channelAvlTreeEntry);    
      return L7_FAILURE;
    }
  }

  while ( L7_NULLPTR != (channelEntry = queue_channel_entry_get_next(packageId, channelEntry)) && 
          L7_NULLPTR != (channelAvlTreeEntryAux = channelEntry->channelAvlTreeEntry) )
  {
    if ( channelAvlTreeEntryAux->channelDataKey.evc_mc ==  channelAvlTreeEntryAux->channelDataKey.evc_mc)
    {
      /*Move to next Channel*/
      continue;
    }

    if ( (L7_INET_ADDR_COMPARE(&channelAvlTreeEntryAux->channelDataKey.channel_group, 
                               &channelAvlTreeEntryAux->channelDataKey.channel_group) == 0) &&
         (L7_INET_ADDR_COMPARE(&channelAvlTreeEntryAux->channelDataKey.channel_source, 
                               &channelAvlTreeEntryAux->channelDataKey.channel_source) == 0) )
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Conflict Found: Channel configured within two services [packageId:%u serviceId:%u groupAddr:%s sourceAddr:%s][serviceIdNew:]",
              packageId, channelAvlTreeEntry->channelDataKey.evc_mc, 
              inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_group, groupAddrStr), inetAddrPrint(&channelAvlTreeEntry->channelDataKey.channel_source, sourceAddrStr),
              channelAvlTreeEntryAux->channelDataKey.evc_mc);    
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

#endif//IGMPASSOC_MULTI_MC_SUPPORTED
/********************************End Multicast Group Packages*********************************************************/


