/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_common.h
*
* @purpose Contains structures,constants,macros, for dvmrp protocol
*
* @component
*
* @comments
*
* @create 07/03/2006
*
* @author Prakash/Shashidhar
* @end
*
**********************************************************************/
#ifndef _DVMRP_COMMON_H
#define _DVMRP_COMMON_H

#include <stdio.h> 
#include <assert.h> 
#include "l3_addrdefs.h" 
#include "osapi_support.h" 
#include "l3_mcast_commdefs.h" 
#include "intf_bitset.h" 
#include "mcast_defs.h" 
#include "l3_mcast_defaultconfig.h"
#include "l7_mcast_api.h" 
#include "mfc_api.h" 
#include "l7apptimer_api.h" 
#include "l7_dvmrp_api.h"
#include "l7_mgmd_api.h"
#include "l7_ip_api.h"
#include "l7_socket.h"
#include "avl_api.h"
#include "radix_api.h"
#include "comm_mask.h"
#include "dvmrp_logging.h"
#include "l7sll_api.h"
#include "l7handle_api.h"
#include "heap_api.h"
#include "dvmrp_api.h"
#include "mcast_api.h"
#include "mcast_inet.h"

typedef enum
{
  DVMRP_EVENT_MFC_ADD_MEMBER = 100,
  DVMRP_EVENT_MFC_DEL_MEMBER,
  MCAST_EVENT_MGMD_QUERIER,
  MCAST_EVENT_MGMD_NO_QUERIER,
  DVMRP_EVENT_ADMIN_SCOPE_ADD,
  DVMRP_EVENT_ADMIN_SCOPE_DELETE,
}dvmrpEventsTypes_t;

#define DVMRP_ALLOC(family,size) \
        heapAlloc(mcastMapHeapIdGet(family),size,__FILE__,__LINE__)
#define DVMRP_FREE(family,pMem) \
        heapFree(mcastMapHeapIdGet(family),pMem)

#define DVMRP_DEFAULT_NETMASK    32
#define DVMRP_ALL_ROUTERS    "224.0.0.4"
#define MAX_STRING_LENGTH 100
#define L7_DVMRP_PROTO            12
#define DVMRP_PROBE      1       /* for finding neighbors    */
#define DVMRP_REPORT     2       /* for reporting some or all routes  */

/* sent by mapper, asking for a list of this router's neighbors. */
#define DVMRP_ASK_NEIGHBORS     3 
#define DVMRP_ASK_NEIGHBORS2    5  /* as above, want new format reply */
#define DVMRP_NEIGHBORS2        6
#define DVMRP_PRUNE             7       /* prune message */
#define DVMRP_GRAFT             8       /* graft message */
#define DVMRP_GRAFT_ACK         9       /* graft acknowledgement */
#define DVMRP_VIF_UP           0x08    /* kernel state of interface */
#define DVMRP_VIF_LEAF         0x80    /* Neighbor reports that it is a leaf */
/*
 * Limit on length of route data
 */
#define DVMRP_IGMP_MINLEN       8                                            
#define DVMRP_MAX_IP_PACKET_LEN       1220

#define DVMRP_MAX_IP_HEADER_LEN       60
#define DVMRP_MAX_DVMRP_DATA_LEN \
          ( DVMRP_MAX_IP_PACKET_LEN - DVMRP_MAX_IP_HEADER_LEN - DVMRP_IGMP_MINLEN )
#define DVMRP_ROUTE_EXPIRE_TIME       200  /* time to mark route invalid  */
#define DVMRP_NEIGHBOR_EXPIRE_TIME     30      /* time to consider neighbor gone   */
#define DVMRP_TIMEOUT_INTERVAL          140 /* Changed from 200 to 120 since route expiry is 140 -Kishore */
#define DVMRP_GARBAGE_INTERVAL          200 /* changed from 140 to 200 since it needs to wait upto 3 report intervals*/
#define DVMRP_NEIGHBOR_EXPIRE_TIME      30
#define DVMRP_CACHE_UPDATE_INTERVAL 10
#define DVMRP_CACHE_EXPIRE_TIME (DVMRP_ROUTE_EXPIRE_TIME - DVMRP_CACHE_UPDATE_INTERVAL)
#define DVMRP_UNREACHABLE       32      /* "infinity" metric, must be <= 64 */  
#define DVMRP_AVERAGE_PRUNE_LIFETIME  7200    /* average lifetime of prunes sent  */
#define DVMRP_FULL_HOST_MASK 0xffffffff
#define DVMRP_MAJOR_VERSION          3
#define DVMRP_MAX_DBG_ADDR_SIZE         IPV6_DISP_ADDR_LEN

#define DVMRP_GROUP_INCLUDE           1
#define DVMRP_RT_CHANGE  0x01
#define DVMRP_RT_DELETE  0x02
#define DVMRP_RT_HOLD    0x10
#define DVMRP_NEIGHBOR_DELETE 0x01

#define DVMRP_MAX_NEIGHBORS    L7_DVMRP_MAX_NBR

#define DVMRP_CACHE_DVMRP_GRAFT 0x10
#define DVMRP_CACHE_NEGATIVE 0x02
#define DVMRP_CACHE_DELETE 0x01

/*#define DVMRP_TIMER_EXPIRY_EVENT 5*/
#define DVMRP_UPDATE_INTERVAL           60
#define DVMRP_PRUNE_TIMEOUT_INTERVAL 10

#define DVMRP_CAPABILITY_FLAGS       0x0e
#define DVMRP_MINOR_VERSION          255
#define DVMRP_METRIC_INFINITY       32
#define DVMRP_STRING_SIZE   25
#define DVMRP_ENTRY_NOT_USED    1
#define DVMRP_ENTRY_USED 2
#define DVMRP_ROUTE_ENTRY_NOT_USED    0
#define DVMRP_ROUTE_ENTRY_USED 1

#define DVMRP_MULTICAST 2
#define DVMRP_MAX_ADMIN_SCOPE_ENTRIES L7_MCAST_MAX_ADMINSCOPE_ENTRIES
#define DVMRP_ADMIN_SCOPE_BUFFPOOL_NAME "dvmrpAdmScpPool"
#define DVMRP_UNUSED_PARAM(arg) ((void) arg)
#define DVMRP_INVALID_INDEX   -1
#define DVMRP_PRUNE_MIN_LENGTH   12
#define DVMRP_PRUNE_MAX_LENGTH   16
#define DVMRP_GRAFT_MIN_LENGTH   8
#define DVMRP_GRAFT_MAX_LENGTH   12


#define DVMRP_MAX_TIMERS (MAX_INTERFACES +DVMRP_MAX_NEIGHBORS +2*DVMRP_MRT_MAX_IPV4_ROUTE_ENTRIES +3)
/* Max unicast route table size within dvmrp*/
#define DVMRP_L3_ROUTE_TBL_SIZE_TOTAL L7_DVMRP_MAX_L3_TABLE_SIZE
#define BIT_TEST(f, b)  ((f) & (b))
#define BIT_SET(f, b)   ((f) |= b)
#define BIT_RESET(f, b) ((f) &= ~(b))

/* DVMRP interface state */
typedef enum
{
  DVMRP_MAP_INTF_STATE_NONE = 0,   /* not created */
  DVMRP_MAP_INTF_STATE_DOWN,       /* not operational from DVMRP perspective */
  DVMRP_MAP_INTF_STATE_UP          /* usable for DVMRP */
} dvmrpMapIntfState_t;

typedef enum
{
  DVMRP_UPDATE_TIMER =1,
  DVMRP_AGING_TIMER,
  DVMRP_FLASH_TIMER,
  DVMRP_PRUNE_EXPIRATION_TIMER,
  DVMRP_GRAFT_TIMER,
  DVMRP_PRUNE_RETRANSMISSION_TIMER,
  DVMRP_NEIGHBOUR_EXPIRY_TIMER,
  DVMRP_PROBE_TIMER
}L7_DVMRP_TIMER_IDS;

typedef struct _L7_dvmrp_inet_addr_s
{
  L7_uchar8 maskLength;
  L7_inet_addr_t addr;
}L7_dvmrp_inet_addr_t;

typedef struct _neighbor_bitset_t
{
  L7_uchar8 bits[(DVMRP_MAX_NEIGHBORS+MCAST_BITX_NBITS-1)/MCAST_BITX_NBITS];
} neighbor_bitset_t;

typedef struct _dvmrp_addr_t 
{
  L7_sll_member_t *next;
  L7_dvmrp_inet_addr_t inetAddr;       /* interface addresess */
} dvmrp_addr_t;


typedef struct _dvmrp_interface_t 
{
  L7_int32 index;
  dvmrp_addr_t primary;
  L7_ulong32 flags;                    /* interface flags (from the ioctl) */    
  neighbor_bitset_t neighbor_mask;
  L7_ulong32 genid;                    /*  -- for V3 -10 has genid per intf */
  L7_int32 metric_in;
  L7_int32 metric_out;
  L7_int32 threshold;
  L7_ulong32 badPkts;                  /*  -- MIB support */
  L7_ulong32 badRts;                   /*  -- MIB support */
  L7_ulong32 sentRts;                  /*  -- MIB support */
  L7_ulong32 badProbePkts;             /*  -- stats */  
  L7_ulong32 badReportPkts;            /*  -- stats */    
  L7_ulong32 badPrunePkts;             /*  -- stats */  
  L7_ulong32 badGraftPkts;             /*  -- stats */    
  L7_ulong32 badGraftAckPkts;          /*  -- stats */      
  L7_ulong32 badNbr2Pkts;              /*  -- stats */        
  L7_ulong32 probePktsRecievedCount;   /*  -- stats */        
  L7_ulong32 reportPktsRecievedCount;  /*  -- stats */        
  L7_ulong32 prunePktsRecievedCount;   /*  -- stats */        
  L7_ulong32 graftPktsRecievedCount;   /*  -- stats */        
  L7_ulong32 graftAckPktsRecievedCount;/*  -- stats */        
  L7_ulong32 nbr2PktsRecievedCount;    /*  -- stats */          
  L7_ulong32 probePktsSentCount;       /*  -- stats */        
  L7_ulong32 reportPktsSentCount;      /*  -- stats */        
  L7_ulong32 prunePktsSentCount;       /*  -- stats */        
  L7_ulong32 graftPktsSentCount;       /*  -- stats */        
  L7_ulong32 graftAckPktsSentCount;    /*  -- stats */        

  L7_sll_t ll_neighbors;
  L7_int32 dlist_in;    /* list num for input filtering */
  L7_int32 dlist_out;   /* list num for output filtering */
  L7_int32 nbr_count;
  L7_APP_TMR_HNDL_t probe; /* neighbor probe */
  void *global; 
  L7_int32 igmp_querier;    

  L7_uint32 probePeriodicTimerHandle;
  dvmrp_timer_event_t timerBlock;
} dvmrp_interface_t;

typedef struct _dvmrp_neighbor_t
{
  L7_sll_member_t *next;
  L7_dvmrp_inet_addr_t nbrAddr;
  dvmrp_interface_t *interface;
  L7_uint32 ctime, utime;
  L7_ulong32 status;  
  L7_ulong32 genid;
  L7_ulong32 badPkts;         /*  -- MIB support */
  L7_ulong32 badRts;          /*  -- MIB support */
  L7_ulong32 flags;
  L7_ulong32 versionSupport;
  L7_int32 index;
  L7_APP_TMR_HNDL_t timeout;/* neighbor timeout */
  L7_int32 state;

  L7_uint32 nbrTimeoutHandle;
} dvmrp_neighbor_t;

typedef struct _desg_fwd_t
{
  L7_ulong32 status;  
  L7_int32 metric;
  dvmrp_neighbor_t *fwd_nbr;
} desg_fwd_t;

typedef struct _dvmrp_route_t
{
  L7_int32 proto;
  L7_ulong32 flags;
  L7_ulong32 entryStatus;
  L7_dvmrp_inet_addr_t networkAddr;
  /* this may be redundant but used for direct if */
  dvmrp_interface_t *interface;
  dvmrp_neighbor_t *neighbor;
  neighbor_bitset_t children;
  neighbor_bitset_t dominants;
  neighbor_bitset_t dependents;
  desg_fwd_t desg_fwd[MAX_INTERFACES];
  L7_int32 metric;
  L7_int32 received_metric;
  L7_uint32 ctime;               /* time created */
  L7_uint32 utime;               /* time updated */
  L7_uint32 dtime;               /* time started deletion process */
} dvmrp_route_t;



typedef struct dvmrp_cache_entry_s 
{
  L7_inet_addr_t source;                             /* Source address*/
  L7_inet_addr_t group;                              /* Group address */
  dvmrp_interface_t *parent;            /* incoming interface */
  L7_uchar8 children[L7_INTF_INDICES];             /* direct members */
  L7_uchar8  routers[L7_INTF_INDICES];               /* neighbor routers  */ 
  L7_sll_t ll_prunes;            /* Prunes received for this cache entry*/
  L7_APP_TMR_HNDL_t       graft_timer; /* graft time out */
  void  *graft_sent;
  L7_ulong32 flags;
  dvmrp_route_t *data;     /* pointer to route entry (dvmrp only?) */  
  L7_int32 holdtime;
  L7_uint32 expire;  
  L7_int32 count;
  L7_uchar8      pruneSent[L7_INTF_INDICES]; /* prunes sent upstream - bit mask */
  L7_uchar8     pruneRcvd[L7_INTF_INDICES];   /* prunes received - bit mask */
  L7_uint32 lifetime;                               /*  prune retransmission*/
  L7_uint32 firstPrune;                           /* prune retransmission*/
  L7_uint32 lastPrune;                            /* prune retransmission*/
  L7_uint32 prune_retry_interval;          /* prune retransmission*/
  L7_APP_TMR_HNDL_t prune_retry_timer; /* Prune retry timer  */
  L7_APP_TMR_HNDL_t cacheRemove_timer; /* cacheRemove timer  */

  L7_uint32  cacheRemoveHandle;
  L7_uint32  pruneRetransmitimerHandle;
  L7_uint32  graftTimeoutHandle;
  /* AVL TREE requires this as last */  
  void *avlData;
} dvmrp_cache_entry_t;

typedef struct dvmrp_src_grp_entry_s 
{
  L7_inet_addr_t source;              /* Group Address   */
  L7_inet_addr_t group;             /*  Source Address */  
  L7_uint32 iif;                            /*  Incoming interface */
  L7_uchar8  oif[L7_INTF_INDICES];   /* out going interface list   */
} dvmrp_src_grp_entry_t;


typedef struct dvmrp_Group_entry_s 
{
  L7_inet_addr_t group;              /* Group address */
  L7_inet_addr_t source;             /* Source address*/
  interface_bitset_t includeStarG;   /* (*,G) Include Bitmask - Only V3 */
  interface_bitset_t includeSG;      /* (S,G) Include Bitmask - Only V3 */
  interface_bitset_t excludeSG;      /* (S,G) Exclude Bitmask - Only V3 */
  L7_uchar8  grpIntfBitMask[L7_INTF_INDICES];     /* Group Intf Bit Mask - Only V2 */
  L7_uchar8  modeIntfBitMask[L7_INTF_INDICES];    /* Mode Intf Bit Mask - Only V2 */
  L7_uchar8  srcIntfBitMask[L7_INTF_INDICES];     /* Sourc Intf Bit Mask - Only V2 */
  L7_uint32 numSrcs;                 /* For IGMP V3 */
  /* AVL TREE requires this as last */  
  void *avlData; 
} dvmrp_Group_entry_t;

typedef struct dvmrpTreeKey_s
{
  L7_uchar8 reserved[4];
  L7_inet_addr_t addr;
} dvmrpTreeKey_t;

typedef struct dvmrpRouteData_s
{
  /* Must be the first element in the structure */
  struct l7_radix_node nodes[2]; 
  /* ipaddr (key) and mask must follow */
  dvmrpTreeKey_t      network;  /* Only the significant portion of the 
                                   network is stored in this field.
                               */
  dvmrpTreeKey_t      netmask;  /* Destination network mask. */

  dvmrp_route_t  dvmrpRouteInfo;  /* route info  */
  void *next;   /* Need this for radix */
} dvmrpRouteData_t;


typedef struct _dvmrp_report_t
{
  L7_dvmrp_inet_addr_t srcNetwork;
  L7_int32 metric;
  dvmrp_neighbor_t *neighbor;
} dvmrp_report_t;

/*********************************************************************
*               DVMRP AdminScope Database Node
*********************************************************************/
typedef struct dvmrpASBNode_s
{
  L7_sll_member_t *next;
  L7_inet_addr_t  grpAddr;                       /* Group Address */
  L7_inet_addr_t  grpMask;
  L7_uchar8       intfMask[L7_INTF_INDICES]; /* Interface Mask */
}dvmrpASBNode_t;


typedef struct _dvmrp_t 
{
  L7_int32 proto;          /* PROTO_DVMRP */
  L7_uchar8     family;  
  L7_uint32 sockFd;          /* PROTO_DVMRP */

  L7_uchar8  interface_mask[L7_INTF_INDICES];  /* mask of interfaces configed for */

  L7_APP_TMR_CTRL_BLK_t   timerHandle;
  L7_APP_TMR_HNDL_t       timer; /* timer used for sending update */
  L7_APP_TMR_HNDL_t       age;  /* aging routes */
  L7_APP_TMR_HNDL_t       flash; /* flash update */
  L7_APP_TMR_HNDL_t       expire; /* prune expiration */
  dvmrp_interface_t dvmrp_interfaces[MAX_INTERFACES];
  dvmrp_neighbor_t index2neighbor[DVMRP_MAX_NEIGHBORS];
  dvmrp_report_t  report_entries[L7_L3_ROUTE_TBL_SIZE_TOTAL];

  L7_dvmrp_inet_addr_t all_routers;
  L7_int32 flash_update_waiting;
  L7_int32 changed;
  L7_ulong32 versionSupport;
  /******** AdminScope Boundary Database Specific parameters ***************/
  L7_sll_t           dvmrpasbList;  /* AdminScope Boundary List of type pimdmASBNode_t */

  avlTreeTables_t   *dvmrpCacheTableTreeHeap;   /* space for tree nodes */
  dvmrp_cache_entry_t *dvmrpCacheDataHeap;   /* space for data */
  avlTree_t dvmrpCacheTableTree;

  avlTreeTables_t   *dvmrpGroupTableTreeHeap;   /* space for tree nodes */
  dvmrp_Group_entry_t *dvmrpGroupDataHeap;   /* space for data */
  avlTree_t dvmrpGroupTableTree;

  radixTree_t      dvmrpRouteTreeData;
  dvmrpRouteData_t *dvmrpRouteDataHeap; 
  L7_uchar8        *dvmrpRouteTreeHeap;
  L7_uint32 radix_entries;

  osapiRWLock_t    dvmrpRwLock;  /* read-write lock protects all DVMRP data. */
  L7_BOOL          dvmrpOperFlag; /* DVMRP Current operational flag */
  handle_list_t    *handle_list;     /* create the handle list */

  L7_uint32  updateTimerHandle;
  L7_uint32  routesTimeoutHandle;
  L7_uint32  pruneExpireHandle;
  L7_uint32  flashrUpdateTimerHandle;
} dvmrp_t;

typedef struct _dvmrp_prune_t 
{
  L7_sll_member_t *next;
  dvmrp_neighbor_t *neighbor;
  L7_int32 lifetime;
  L7_uint32 received;
  L7_uint32 expire;
} dvmrp_prune_t;
typedef struct _dvmrp_graft_t 
{
  dvmrp_neighbor_t *neighbor;
  L7_int32 holdtime;
  L7_uint32 received;
  L7_uint32 expire;
  dvmrp_cache_entry_t *entry;
  L7_ulong32 flags;
} dvmrp_graft_t;

extern dvmrp_t *dvmrpCB;
#endif
