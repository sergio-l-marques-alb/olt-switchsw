/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename igmp_snooping.h
*
* @purpose   Operational Information
*
* @component Snooping
*
* @comments none
*
* @create 08-Dec-2006
*
* @author drajendra
*   
* @end
* 
* Modified on 2013/04/14
*      Author: Márcio Melo (marcio-d-melo@ptinovacao.pt) 
*  Notes:
*
**********************************************************************/
#ifndef SNOOPING_H
#define SNOOPING_H

#include "l7handle_api.h"
#include "l7apptimer_api.h"
#include "l7sll_api.h"
#include "avl_api.h"
#include "dot1q_api.h"

#include "snooping_exports.h"
#include "snooping_cfg.h"
#include "snooping_cnfgr.h"
#include "snooping_querier.h"

/* PTin added: IGMP Snooping */
#if 1
#include "ptin_globaldefs.h"

#endif

#define SNOOP_MAC_ADDR_PREFIX_LEN 2
#define SNOOP_MAC_ADDR_SUFFIX_LEN 4

#define SNOOP_LITTLE_ENDIAN 0
#define SNOOP_BIG_ENDIAN 1

/* PTin added: IGMPv3 snooping */
#if SNOOP_PTIN_IGMPv3_GLOBAL

#if SNOOP_PTIN_IGMPv3_PROXY

#define SNOOP_IGMPv3_MAX_SOURCE_PER_REPORT (1500-8-24-8-8)/4 /*363=(MTU-802.1Q-IPPayload-IGMPv3_Payload-IGMPv3_Group_Record_Payload)/IPv4Size : Sources per Per Report*/

#define SNOOP_IGMPv3_MAX_GROUP_RECORD_PER_REPORT 64 //((MTU-802.1Q-IPPayload-IGMPv3_Payload)/[(GroupRecordPayload+GroupAddr+SourceAddr)/8]=1460/12=121.66 Bytes*/

#define SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM 0

#define SNOOP_PTIN_MAX_ROBUSTNESS_VARIABLE 7 /*This value must serve two purposes: RV configured on the Management and the RV that came from the network*/

#define SNOOP_PTIN_GROUP_AND_SOURCE_SPECIFC_QUERY_SUPPORT 0 /*Currently we do not support sending Group and Source Specific Queries*/

#define SNOOP_PTIN_LW_IGMPv3_MLDv2_MODE 0 /*To reduce the complexity of the IGMPv3 and MLDV2 we us the LW-IGMPv3/LW-MLDv2 (RFC 5790) */

/*This values are not defined in RFC3376, altought it makes sense to have them in order to easily identify the type of packet to be sent*/
#define L7_IGMP_MEMBERSHIP_GROUP_SPECIFIC_QUERY             0x10
#define L7_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY 0x9
/*End definition*/
#endif

typedef enum
{
  PTIN_SNOOP_FILTERMODE_INCLUDE=0,
  PTIN_SNOOP_FILTERMODE_EXCLUDE
} snoop_ptin_filtermode_t;

typedef enum
{
  PTIN_SNOOP_SOURCESTATE_INACTIVE = 0,
  PTIN_SNOOP_SOURCESTATE_ACTIVE,
  PTIN_SNOOP_SOURCESTATE_TOREMOVE
} snoop_ptin_sourcestate_t;
#endif

typedef enum
{
  SNOOP_PARAM_ADMIN_MODE = 0,
  SNOOP_PARAM_INTF_MODE,
  SNOOP_PARAM_MCAST_RTR_STATUS,
  SNOOP_PARAM_INTF_FAST_LEAVE_ADMIN_MODE,
  SNOOP_PARAM_SRC_ADDR_PROXY,
  SNOOP_PARAM_GROUP_MEMBERSHIP_INTERVAL,
  SNOOP_PARAM_MAX_RESPONSE_TIME,
  SNOOP_PARAM_MCAST_RTR_EXPIRY_TIME,
  SNOOP_PARAM_IGMP_NETWORK_VERSION,
  SNOOP_PARAM_IGMP_CLIENT_VERSION
} snoop_params_t;

typedef enum
{
  SNOOP_VLAN_SEND = 0,
  SNOOP_VLAN_INTF_SEND
} snoop_send_type_t;

typedef enum
{
  SNOOP_STATIC = 0,
  SNOOP_DYNAMIC
} snoop_requestor_t;

/* Snooping Timers */
typedef enum
{
  SNOOP_GROUP_MEMBERSHIP = 0,
  SNOOP_MAX_RESPONSE,
  SNOOP_MCAST_ROUTER_ATTACHED,
  SNOOP_STATIC_GROUP_MEMBERSHIP,
  SNOOP_STATIC_MCAST_ROUTER_ATTACHED

} snoop_interface_type_t;


/* Snooping Event Message IDs */
typedef enum
{
  snoopMsgTimerTick = 1,
  snoopPDUMsgRecv,
  snoopCnfgr,
  snoopIntfChange,
  snoopNimStartup,
  snoopIntfModeChange,
  snoopIntfMrouterModeChange,
  snoopVlanMrouterModeChange,
  snoopAdminModeChange,
  snoopVlanModeChange,
  snoopVlanChange,
  snoopFlush,
  snoopDot1sIntfChange,
  snoopL3McastAdd,
  snoopL3McastDelete,
  snoopL3McastModeChange,

  /* Snooping Querier events */
  snoopQuerierModeChange,
  snoopQuerierVlanModeChange,
  snoopQuerierVlanElectionModeChange,
  snoopQuerierAddressChange,
  snoopQuerierVlanAddressChange,
  snoopQuerierVersionChange,
  snoopQuerierNetworkAddressChange,
  snoopQuerierRtrAddressChange,

  snoopHandleNewBackupManager,
  snoopActivateStartupDone

}snoopMgmtMessages_t;

/* Dynamically learnt querier information */
typedef struct snoopQuerierTimerData_s
{
  /* Querier Expiry Timer */
  L7_APP_TMR_HNDL_t      querierExpiryTimer;
  L7_uint32              querierExpiryTimerHandle;

  /* Query Interval Timer */
  L7_APP_TMR_HNDL_t      queryIntervalTimer;
  L7_uint32              queryIntervalTimerHandle;
} snoopQuerierTimerData_t;

typedef struct snoopQuerierInfo_s
{
  snoopQuerierState_t snoopQuerierOperState; /* Snooping querier state */
  /* True Querier information. When the snooping querier is in
     Disabled/INIT/Non-Querier mode then the following info corresponds to detected
     true querier info
   */
  L7_uint32           snoopQuerierOperVersion; /* version currently active in the network */
  L7_inet_addr_t      querierAddress; /* IP Address of detected other querier */
  L7_uint32           querierVersion;        /* version of detected other querier */
  L7_ushort16         maxResponseCode; /* Maximum Response Code */
  L7_uchar8           sFlagQRV;        /* Router side suppression Flag, Querier Robustness Variable*/
  L7_uchar8           qqic;            /* Querier Querier Interval Code */
  snoopQuerierTimerData_t snoopQuerierTimerData;
} snoopQuerierInfo_t;

/* Snooping Statistics */
typedef struct snoopStats_s
{
  L7_uint32 controlFramesProcessed;
} snoopStats_t;

/* This is a per-interface operational structure which presently has only one field.
   More fields can be added to this as and when required */
typedef struct snoopIntfAcqInfo_s
{
  AcquiredMask   acquiredList;     /* Mask of components "acquiring"
                                      an interface */
} snoopIntfAcqInfo_t;


typedef struct snoopOperData_s
{
  L7_ushort16         vlanId;
  L7_INTF_MASK_t      snoopIntfMode;       /* Snooping Interface operational mode:  1 = enabled, 0 = disabled */
  L7_INTF_MASK_t      mcastRtrAttached;    /* MC router attached?  1 = true, 0 = false */
  snoopQuerierInfo_t  snoopQuerierInfo;
  void               *cbHandle;
  void               *next;
} snoopOperData_t;

/* Group Membership Timer Node Structure */
struct snoopInfoData_s;

typedef struct snoopGrpTimerData_s
{
  L7_sll_member_t          *next;
  struct snoopInfoData_s   *snoopEntry; /* To reference vidmac and snoopMemberList
                                    */
  L7_uint32                 intIfNum;
  L7_uchar8                 timerType;
  L7_APP_TMR_HNDL_t         grpTimer;
  L7_uint32                 grpTimerHandle;
} snoopGrpTimerData_t;

/* Mrouter Timer Node Structure */
typedef struct snoopMrtrTimerDataKey_s
{
#ifndef L7_PRODUCT_SMARTPATH
  L7_uint32           vlanId;
  L7_uint32           intIfNum;
#else
  L7_ushort16           vlanId;
  L7_ushort16           intIfNum;
#endif
} snoopMrtrTimerDataKey_t;

typedef struct snoopMrtrTimerData_s
{
  snoopMrtrTimerDataKey_t  snoopMrtrTimerDataKey;
  void                    *cbHandle;
  L7_APP_TMR_HNDL_t        mrtrTimer;
  L7_uint32                mrtrTimerHandle;
  void                    *next;
} snoopMrtrTimerData_t;

/* PTin added: IGMP snooping */
#if 1
/* IP channel information */
typedef struct
{
  L7_BOOL   active;             /* Is channel active? */
  L7_uint32 ipAddr;             /* Channel IP address */
  L7_uint8  number_of_ports;    /* Number of interfaces being used */
  L7_uint32 intIfNum_mask[PTIN_SYSTEM_MAXINTERFACES_PER_GROUP/(sizeof(L7_uint32)*8)+1];  /* List of ports, this IP is being used */
  L7_uint16 number_of_clients;  /* Number of clients using this channel */
  L7_uint32 clients_list[PTIN_SYSTEM_IGMP_MAXCLIENTS/(sizeof(L7_uint32)*8)+1]; /* List of (index) clients */
  L7_uint16 intf_number_of_clients[PTIN_SYSTEM_MAXINTERFACES_PER_GROUP];
} ptinSnoopChannelInfo_t;

/* Port information */
typedef struct
{
  L7_BOOL active;               /* Is this port being used? */
  L7_uint16 number_of_channels; /* Number of channels used in this port */
  L7_uint16 number_of_clients;  /* Number of clients in this port */
} ptinSnoopPortInfo_t;

/* Global informations */
typedef struct
{
  L7_uint8  number_of_ports;    /* Total number of ports being used */
  L7_uint16 number_of_channels; /* Total number of channels */
  L7_uint16 number_of_clients;  /* Total number of clients */
} ptinSnoopGlobalInfo_t;
#endif

/* AVL Tree Snooping Entry Strucutre */
typedef struct snoopInfoDataKey_s
{
  L7_uchar8  family;
  L7_uchar8  vlanIdMacAddr[L7_FDB_KEY_SIZE]; /* Include two bytes of VLAN ID */

} snoopInfoDataKey_t;

/* PTin added: IGMP snooping */
#if 1
#define SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY  32
#endif
typedef struct snoopInfoData_s
{
  snoopInfoDataKey_t      snoopInfoDataKey;
  L7_INTF_MASK_t          snoopGrpMemberList;
  L7_INTF_MASK_t          snoopMrtrMemberList;
  L7_sll_t                ll_timerList; /* Linked list of timer data nodes */
  L7_APP_TMR_CTRL_BLK_t   timerCB;       /* Entry App Timer Control Block */
  L7_uint32               timerIdx;     /* Index to timer List */

  /* PTin added: IGMP snooping */
  #if 1
  ptinSnoopGlobalInfo_t   global;
  ptinSnoopChannelInfo_t  channel_list[SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY]; /* IP address: 4 fixed bits + 5 variable bits + 23 fixed bits for this group: 2^5=32 */
  ptinSnoopPortInfo_t     port_list[PTIN_SYSTEM_MAXINTERFACES_PER_GROUP];

  L7_BOOL staticGroup;
  #endif

  void               *next;
} snoopInfoData_t;

/* PTin DFF - AVL Tree Snooping L3 Entry Structure */
#if 1
typedef struct snoopPTinL3InfoData_s snoopPTinL3InfoData_t;


typedef struct
{
  L7_sll_member_t   *next;

  L7_uint16             interfaceIdx;
  snoopPTinL3InfoData_t *groupData;

  L7_uchar8         timerType;
  L7_APP_TMR_HNDL_t timer;
  L7_uint32         timerHandle;
} snoopPTinL3Querytimer_t;

typedef struct
{
  L7_sll_member_t       *next;

  L7_uint16             sourceIdx;
  L7_uint16             interfaceIdx;
  snoopPTinL3InfoData_t *groupData;

  L7_uint8              isRunning;

  L7_uchar8             timerType;
  L7_APP_TMR_HNDL_t     timer;
  L7_uint32             timerHandle;
} snoopPTinL3Sourcetimer_t;

typedef struct
{
  L7_sll_member_t       *next;

  L7_uint16             interfaceIdx;
  snoopPTinL3InfoData_t *groupData;

  L7_uchar8             timerType;
  L7_APP_TMR_HNDL_t     timer;
  L7_uint32             timerHandle;
} snoopPTinL3Grouptimer_t;

typedef struct
{
  L7_uint32                clients[PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE];
  L7_inet_addr_t           sourceAddr;
//L7_uint32                clientfiltermode[PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE];//snoop_ptin_filtermode_t
  snoopPTinL3Sourcetimer_t sourceTimer;
  L7_uint8                 status;
  L7_uint16                numberOfClients;
  L7_BOOL                  isStatic;
} snoopPTinL3Source_t;

typedef struct
{
  L7_uint8                filtermode;//snoop_ptin_filtermode_t
  snoopPTinL3Source_t     sources[PTIN_SYSTEM_IGMP_MAXSOURCES_PER_GROUP];
  snoopPTinL3Grouptimer_t groupTimer;
  L7_uint8                numberOfSources;
  L7_uint8                active;
  L7_BOOL                 isStatic;
  L7_uint32               clients[PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE];
//L7_uint32               clientfiltermode[PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE];//snoop_ptin_filtermode_t
  L7_uint16               numberOfClients;
} snoopPTinL3Interface_t;

typedef struct snoopPTinL3InfoDataKey_s
{
  L7_inet_addr_t mcastGroupAddr;
  L7_uint32      vlanId;
} snoopPTinL3InfoDataKey_t;

typedef struct snoopPTinL3InfoData_s
{
  snoopPTinL3InfoDataKey_t snoopPTinL3InfoDataKey;
  snoopPTinL3Interface_t   interfaces[PTIN_SYSTEM_MAXINTERFACES_PER_GROUP]; // Iface id 0 is used to store Proxy information.

  snoopInfoData_t          *L2MC;

  void                     *next;
} snoopPTinL3InfoData_t;

/************************************************************************************************************/
/*Structures required for MGMD Proxy*/

typedef enum
{
  SNOOP_PTIN_IGMPv3_MLDv2=0,//Default version
  SNOOP_PTIN_IGMPv2_MLDv1,
  SNOOP_PTIN_IGMPv1 //Here for historical reasons
}SnoopPtinProxyMRouterVersion_t;

typedef enum
{
  SNOOP_PTIN_MEMBERSHIP_REPORT=0,
  SNOOP_PTIN_GENERAL_QUERY,
  SNOOP_PTIN_GROUP_SPECIFIC_QUERY,
  SNOOP_PTIN_GROUP_AND_SOURCE_SPECIFIC_QUERY,
  SNOOP_PTIN_MEMBERSHIP_QUERY
}SnoopPtinProxy_Query_t;


typedef struct  snoopPTinProxyGroup_s snoopPTinProxyGroup_t;

typedef struct snoopPTinProxyTimer_s
{
  L7_sll_member_t                   *next;

  void                              *groupData;//It will be either a interfacePtr or a groupPtr

  L7_uint32                          noOfRecords;

  L7_uint8                           robustnessVariable;

  L7_uint8                           reportType;
  
//L7_uint32                          selectedDelay;

  L7_BOOL                            isFirstTransmission;

  L7_BOOL                            isRunning;

  L7_BOOL                            isInterface;

  L7_uchar8                          timerType;
  L7_APP_TMR_HNDL_t                  timer;
  L7_uint32                          timerHandle;
  
} snoopPTinProxyTimer_t;

typedef struct snoopPTinProxySourceKey_s
{  
#if 0
  L7_inet_addr_t                     groupAddr;//IPv4(v6) Multicast Group Address
#else
  snoopPTinProxyGroup_t*             groupPtr;//Group Record Ptr
#endif
  L7_inet_addr_t                     sourceAddr;
} snoopPTinProxySourceKey_t;

typedef struct snoopPTinProxySource_s snoopPTinProxySource_t;

typedef struct snoopPTinProxySource_s
{
  
  snoopPTinProxySourceKey_t          key;
//snoopPTinProxyGroup_t*             groupPtr;//Group Record Ptr

  L7_uint8                           retransmissions;
  L7_uint8                           robustnessVariable;

//snoopPTinProxyTimer_t              timer;
  
  snoopPTinProxySource_t            *nextSource;
  snoopPTinProxySource_t            *previousSource;

  snoopInfoData_t          *L2MC;

  void                     *next;

}snoopPTinProxySource_t;

typedef struct  snoopPTinProxyInterface_s snoopPTinProxyInterface_t;

typedef struct snoopPTinProxyGroupKey_s
{
  L7_uint32                               vlanId;
  L7_inet_addr_t                          groupAddr;//IPv4(v6) Multicast Group Address
  L7_uint8                                recordType;//MGMD_GROUP_REPORT_TYPE_t  
} snoopPTinProxyGroupKey_t;

typedef struct snoopPTinProxyGroup_s
{
  snoopPTinProxyGroupKey_t                key;
  L7_uint8                                retransmissions;
  L7_uint8                                robustnessVariable;
  L7_uint16                               numberOfSources;//Number of active Sources
  snoopPTinProxySource_t                 *source;//List Containing the Sources of this Group  
  snoopPTinProxyInterface_t              *interfacePtr;//Interface 

  L7_uint8                                recordType;//MGMD_GROUP_REPORT_TYPE_t  

  snoopPTinProxyTimer_t                  timer;   
  
  snoopPTinProxyGroup_t            *nextGroupRecord;
  snoopPTinProxyGroup_t            *previousGroupRecord;

  snoopInfoData_t          *L2MC;

  void                     *next;
}snoopPTinProxyGroup_t;

typedef struct snoopPTinProxyInterfaceKey_s
{  
  L7_uint32                               vlanId;
} snoopPTinProxyInterfaceKey_t;


typedef struct snoopPTinProxyInterface_s
{
  snoopPTinProxyInterfaceKey_t    key;

  L7_uint8                                retransmissions;
  L7_uint8                                robustnessVariable;

  L7_uint32                               numberOfGroupRecords;//Number of active Groups
  
  snoopPTinProxyGroup_t                  *groupRecord;

  snoopPTinProxyTimer_t                   timer;   

//void                                   *next;

  snoopInfoData_t          *L2MC;

  void                     *next;
}snoopPTinProxyInterface_t;


/*End Structures required for MGMD Proxy*/
/************************************************************************************************************/

#endif

/* PTin DFF: IGMPv3 Query Queue */
#if 1
typedef struct snoopPTinQueryData_s
{
  L7_uint32         queuePos;
  L7_uint32         retransmissions;
  L7_BOOL           sFlag;
  L7_uint16         vlanId;
  L7_inet_addr_t    groupAddr;
  L7_inet_addr_t    sourceList[PTIN_SYSTEM_IGMP_MAXSOURCES_PER_GROUP];
  L7_uint8          sourcesCnt;
  osapiTimerDescr_t *queryTimer;
}snoopPTinQueryData_t;
#endif

/* AVL Tree Snooping L3 Entry Strucutre */
typedef struct snoopL3InfoDataKey_s
{
  L7_uchar8      macAddrSuffix[SNOOP_MAC_ADDR_SUFFIX_LEN];
  L7_inet_addr_t mcastGroupAddr;
  L7_inet_addr_t mcastSrcAddr;
} snoopL3InfoDataKey_t;

typedef struct snoopL3InfoData_s
{
  snoopL3InfoDataKey_t  snoopL3InfoDataKey;
  L7_int32              srcIntfVlan; /* L7_NULL means port based rtr intf */
  L7_VLAN_MASK_t        outVlanRtrIntfMask; /* Routing VLANs List */

  void               *next;
} snoopL3InfoData_t;

typedef struct snoopAppTimer_s
{
  L7_APP_TMR_CTRL_BLK_t  timerCB;
  snoopInfoData_t        *pSnoopEntry; /* Ptr to group entry */

} snoopAppTimer_t;


typedef struct snoopCkptInfoData_s
{
  snoopInfoDataKey_t  snoopInfoDataKey;
  L7_INTF_MASK_t      snoopGrpMemberList;
  L7_INTF_MASK_t      snoopMrtrMemberList;

  void               *next;
} snoopCkptInfoData_t;

typedef struct snoopCkptOperData_s
{
  L7_ushort16         vlanId;
  snoopQuerierInfo_t  snoopQuerierInfo;
  L7_INTF_MASK_t      mcastRtrAttached;

  void               *next;
} snoopCkptOperData_t;

/******************************************************************************
  Control Objects Definitions
 *****************************************************************************/
/* Snooping Execution block */
typedef struct snoop_eb_s
{
  snoopIntfAcqInfo_t *snoopIntfInfo;    /* component acquire list */
  L7_uint32          *snoopIntfMapTbl;  /* interface map table */
  L7_VLAN_MASK_t      routingVlanVidMask; /* Routing VLANs List */

  /* Timer Data */
  L7_APP_TMR_CTRL_BLK_t   timerCB;       /* MGMD App Timer Control Block */
  snoopAppTimer_t        *snoopEntryTimerCBList;/* Timers per group entry */
  L7_int32               *snoopEntryTimerFreeList;/* Timers per group entry */
  L7_uint32               snoopEntryTimerFreeIdx;/* Timers per group entry */
  handle_list_t          *handle_list;
  void                   *handleListMemHndl;

  /* AVL Tree data */
  avlTree_t           snoopAvlTree;
  avlTreeTables_t    *snoopTreeHeap;
  snoopInfoData_t    *snoopDataHeap;

  /* L3 AVL Tree data */
  avlTree_t           snoopL3AvlTree;
  avlTreeTables_t    *snoopL3TreeHeap;
  snoopL3InfoData_t  *snoopL3DataHeap;

  /* Checkpoint/Backup AVL Tree data */
  avlTree_t              snoopCkptAvlTree;
  avlTreeTables_t       *snoopCkptTreeHeap;
  snoopCkptInfoData_t   *snoopCkptDataHeap;

/* PTin added: IGMPv3 snooping */
#if SNOOP_PTIN_IGMPv3_GLOBAL

#if SNOOP_PTIN_IGMPv3_ROUTER
  /* L3 PTin AVL Tree data */
  avlTree_t               snoopPTinL3AvlTree;
  avlTreeTables_t        *snoopPTinL3TreeHeap;
  snoopPTinL3InfoData_t  *snoopPTinL3DataHeap;
#endif

#if SNOOP_PTIN_IGMPv3_PROXY
  /* PTin Proxy  Source AVL Tree data */
  avlTree_t                  snoopPTinProxySourceAvlTree;
  avlTreeTables_t           *snoopPTinProxySourceTreeHeap;
  snoopPTinProxySource_t    *snoopPTinProxySourceDataHeap;

   /* PTin Proxy  Group AVL Tree data */
  avlTree_t                  snoopPTinProxyGroupAvlTree;
  avlTreeTables_t           *snoopPTinProxyGroupTreeHeap;
  snoopPTinProxyGroup_t     *snoopPTinProxyGroupDataHeap;

  /* PTin Proxy  Group AVL Tree data */
  avlTree_t                  snoopPTinProxyInterfaceAvlTree;
  avlTreeTables_t           *snoopPTinProxyInterfaceTreeHeap;
  snoopPTinProxyInterface_t *snoopPTinProxyInterfaceDataHeap;
#endif

#endif

  /* Checkpoint/Backup L3 AVL Tree data */
  avlTree_t           snoopCkptL3AvlTree;
  avlTreeTables_t    *snoopCkptL3TreeHeap;
  snoopL3InfoData_t  *snoopCkptL3DataHeap;

  /* Semaphores */
  void               *snoopMsgQSema;
/* PTin added: IGMPv3 snooping */
#if 1
  void               *snoopPTinQueryQSema;
#endif

/* PTin added: IGMPv3 Query queue */
#if 1
  snoopPTinQueryData_t  snoopPTinQueryQueue[PTIN_SYSTEM_QUERY_QUEUE_MAX_SIZE]; //Query Buffer (behaves as a FIFO queue)
  L7_uint32             snoopPTinQueryQueueFreeList[PTIN_SYSTEM_QUERY_QUEUE_MAX_SIZE]; //List of free positions in Query Buffer
  L7_uint32             snoopPTinQueryQueueFreeListPopIdx; //Index of the first element in Query Queue to be removed
  L7_uint32             snoopPTinQueryQueueFreeListPushIdx; //Index of the first free position in Query Queue
  L7_BOOL               snoopPTinQueryQueueFreeListFull; //Index of the first free position in Query Queue
  L7_BOOL               snoopPTinQueryQueueFreeListEmpty; //Index of the first free position in Query Queue
#endif

  /* Internal Event Message Queue */
  void               *snoopQueue;
  void               *snoopTimerQueue;
  void               *snoopMLDQueue;
  void               *snoopIGMPQueue;

  /* Snoop Task */
  L7_uint32           snoopTaskId;

  /* Buffer Pools */
  L7_uint32           snoopSmallBufferPoolId;
  L7_uint32           snoopMedBufferPoolId;
  L7_uint32           snoopLargeBufferPoolId;
  L7_uint32           snoopTimerBufferPoolId;
  L7_uint32           appTimerBufferPoolId;

  /* Component Configurator state */
  snoopCnfgrState_t snoopCnfgrState;

  /* L7_TRUE after NIM create startup is done */
  L7_BOOL createStartupDone;

  /* Bit mask of NIM events that Snooping is registered to receive */
  PORTEVENT_MASK_t nimEventMask;

  /* Number of snooping instances supported */
  L7_uchar8      maxSnoopInstances;

  /* Flag to specify if IPv6 hop by hop option with rtr alert is
     supported supported or not */
  L7_BOOL        ipv6OptionsSupport;

  /* L7_TRUE if last restart was a warm restart */
  L7_BOOL        snoopWarmRestart;

  /* L7_TRUE after NIM activate startup is done */
  L7_BOOL        activateStartupDone;

  /* L7_TRUE when the checkpoint service has notified Snooping that a
   * backup unit has been elected. Snooping checkpoints all checkpoint
   * data when there is a backup. */
  L7_BOOL        snoopBackupElected;

  /*
  Queue used for holding checkpoint requests while waiting for
  checkpoint service to process the data
  */
  void           *snoopCkptReqQ;

} snoop_eb_t;

/* Snooping Control block */
typedef struct snoop_cb_s
{
  L7_uchar8           cbIndex;/* Assigned at runtime 0 or 1 */
  L7_uchar8           family; /* L7_AF_INET => IGMP, L7_AF_INET6 => MLD, ... */
  snoopCfgData_t     *snoopCfgData;   /* Mgmt configured instance */
  snoopStats_t        counters;       /* Statistics collector */
  snoop_eb_t         *snoopExec;      /* Pointer to Execution block */

  /* Debug info */
  snoopDebugCfg_t    *snoopDebugCfg;   /* Persistent Debug Data */
  L7_BOOL snoopDebugPacketTraceTxFlag; /* Volatile Debug Data */
  L7_BOOL snoopDebugPacketTraceRxFlag; /* Volatile Debug Data */

  /* Timer Data */
  /* AVL Tree data */
  avlTree_t                snoopMrtrTimerAvlTree;
  avlTreeTables_t         *snoopMrtrTimerTreeHeap;
  snoopMrtrTimerData_t    *snoopMrtrTimerDataHeap;

  /* Operational Data AVL Tree data */
  avlTree_t                snoopOperDataAvlTree;
  avlTreeTables_t         *snoopOperDataTreeHeap;
  snoopOperData_t         *snoopOperDataDataHeap;

  /* Checkpoint/Backup Data AVL Tree data */
  avlTree_t                snoopCkptOperDataAvlTree;
  avlTreeTables_t         *snoopCkptOperDataTreeHeap;
  snoopCkptOperData_t     *snoopCkptOperDataDataHeap;

  /* Number of snooping queriers supported */
  L7_ushort16    enabledSnoopQuerierVlans;

  /*
  Queue used for holding checkpoint requests while waiting for
  checkpoint service to process the data
  */
  void           *snoopCkptReqQ;

} snoop_cb_t;

typedef struct snoopIntfChangeParms_s
{
  L7_uint32 event;
  NIM_CORRELATOR_t correlator;
} snoopIntfChangeParms_t;

typedef struct snoopDot1sIntfChangeParms_s
{
  L7_uint32 event;
  L7_uint32 mstID;
} snoopDot1sIntfChangeParms_t;

typedef struct snoopNimStartup_s
{
  NIM_STARTUP_PHASE_t startupPhase;
} snoopNimStartup_t;

typedef struct snoopL3McastChangeParms_s
{
  L7_inet_addr_t mcastGroupAddr;
  L7_inet_addr_t mcastSrcAddr;
  L7_int32       srcIntfVlan; /* -1 if it is port based rtr interface */
  L7_VLAN_MASK_t outVlanRtrIntfMask;
} snoopL3McastChangeParms_t;

typedef struct snoopTimerParams_s
{
  L7_uint32 timerCBHandle;
} snoopTimerParams_t;
#define SNOOP_TIMER_MSG_SIZE         sizeof(snoopTimerParams_t)

/* Snoop Event Message format */
typedef struct snoopMgmtMsg_s
{
  L7_uint32        msgId;    /* Of type snoopMgmtMessages_t */
  L7_uint32        intIfNum;
  L7_uint32        vlanId;
  L7_uint32        vlanEvent;
  union
  {
    L7_CNFGR_CMD_DATA_t  CmdData;
    snoopIntfChangeParms_t snoopIntfChangeParms;
    snoopNimStartup_t snoopNimStartup;
    snoopDot1sIntfChangeParms_t snoopDot1sIntfChangeParms;
    L7_uint32           mode;
    snoopL3McastChangeParms_t mcastChangeParams;
    snoopTimerParams_t  timerParams;
    dot1qNotifyData_t   vlanData;
  } u;
  snoop_cb_t *cbHandle;
} snoopMgmtMsg_t;
#define SNOOP_MSG_SIZE         sizeof(snoopMgmtMsg_t)

/* Snoop PDU Message format */
typedef struct snoopPDU_Msg_s
{
  L7_uint32        msgId;    /* Of type snoopMgmtMessages_t */
  L7_uint32        intIfNum; /*Interface on which PDU was received */
  L7_uint32        vlanId;   /*VLAN on which PDU was received */
  L7_uint32        innerVlanId;   /*Inner VLAN if present */
  L7_uint32        client_idx;    /* Client index */          /* PTin added: IGMP snooping */
  snoop_cb_t      *cbHandle; /* Pointer to control block */
  L7_uchar8       *snoopBuffer; /* Pointer to the received PDU */
  L7_uint32        dataLength;  /* Length of received PDU */
  L7_uint32        snoopBufferPoolId; /* Bufferpool used to make a local copy */
} snoopPDU_Msg_t;
#define SNOOP_PDU_MSG_SIZE     sizeof(snoopPDU_Msg_t)

/* Start of Function Prototype */
L7_uint32   maxSnoopInstancesGet(void);
snoop_cb_t *snoopCBGet(L7_uchar8 family);
snoop_cb_t *snoopCBFirstGet(void);
snoop_eb_t *snoopEBGet(void);
/* End of function prototypes */

/* PTin added: IGMP snoopping */
#if 1
/**
 * Generate an IGMP packet from scratch, and send it to router 
 * interfaces 
 * 
 * @param vlan    : Internal Vlan
 * @param type    : IGMP packet type
 * @param groupIP : IP channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t igmp_generate_packet_and_send(L7_uint32 vlan, L7_uint8 type, L7_inet_addr_t *channel);

L7_uint8 snoopRecordType2IGMPStatField(L7_uint8 recordType,L7_uint8 fieldType);
#endif

#endif /* SNOOPING_H */

