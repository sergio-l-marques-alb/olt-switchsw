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
* @end
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
#include "dtl_ptin.h"

#define SNOOP_MAC_ADDR_PREFIX_LEN 2
#define SNOOP_MAC_ADDR_SUFFIX_LEN 4

#define SNOOP_LITTLE_ENDIAN 0
#define SNOOP_BIG_ENDIAN 1

typedef enum
{
  SNOOP_PARAM_ADMIN_MODE = 0,
  SNOOP_PARAM_INTF_MODE,
  SNOOP_PARAM_MCAST_RTR_STATUS,
  SNOOP_PARAM_INTF_FAST_LEAVE_ADMIN_MODE,
  SNOOP_PARAM_SRC_ADDR_PROXY,
  SNOOP_PARAM_GROUP_MEMBERSHIP_INTERVAL,
  SNOOP_PARAM_MAX_RESPONSE_TIME,
  SNOOP_PARAM_MCAST_RTR_EXPIRY_TIME
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

  /* Snooping Querier events */
  snoopQuerierModeChange,
  snoopQuerierVlanModeChange,
  snoopQuerierVlanElectionModeChange,
  snoopQuerierAddressChange,
  snoopQuerierVlanAddressChange,
  snoopQuerierVersionChange,
  snoopQuerierNetworkAddressChange

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
  L7_uint32		      snoopQuerierOperVersion; /* version currently active in the network */
  L7_inet_addr_t      querierAddress; /* IP Address of detected other querier */
  L7_uint32		      querierVersion;        /* version of detected other querier */
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


/* AVL Tree Snooping Entry Strucutre */
typedef struct snoopInfoDataKey_s
{
  L7_uchar8  family;
  L7_uchar8  vlanIdMacAddr[L7_FDB_KEY_SIZE]; /* Include two bytes of VLAN ID */
} snoopInfoDataKey_t;

// PTin added
typedef struct _st_ptin_ip_list_t
{
  L7_uint32 channelIPs_bmp;
  L7_uint16 number_of_ips;
} st_ptin_ip_list_t;

typedef struct _st_ptin_client_list_t
{
  L7_uint16 number_of_clients;
  /*L7_uint16 number_of_clients_by_port[L7_SYSTEM_N_INTERF];*/
  L7_uint32 clientVlan[4096/(sizeof(L7_uint32)*8)];
} st_ptin_client_list_t;


typedef struct snoopIpInfoDataKey_s
{
  L7_uint16 vlanId;       /* UNI VLAN */
  L7_uint32 channelIP;    /* Channel IP address */
} snoopIpInfoDataKey_t; 
// PTin end

typedef struct snoopInfoData_s
{
  snoopInfoDataKey_t  snoopInfoDataKey;
  L7_INTF_MASK_t      snoopGrpMemberList;
  L7_INTF_MASK_t      snoopMrtrMemberList;
  L7_sll_t            ll_timerList; /* Linked list of timer data nodes */
  L7_APP_TMR_CTRL_BLK_t   timerCB;       /* Entry App Timer Control Block */
  L7_uint32               timerIdx;     /* Index to timer List */

  st_ptin_ip_list_t ipList;
  L7_BOOL           staticEntry;

  void               *next;
} snoopInfoData_t;


// PTin added
typedef struct snoopIpInfoData_s
{
  snoopIpInfoDataKey_t  snoopIpInfoDataKey;
  st_ptin_client_list_t clientList;
  L7_BOOL               staticEntry;
  void                  *next;
} snoopIpInfoData_t;
// PTin end

 
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

  // PTin added
  // MC channels AVL Tree data
  avlTree_t           snoopIpAvlTree;
  avlTreeTables_t    *snoopIpTreeHeap;
  snoopIpInfoData_t  *snoopIpDataHeap;
  // PTin end 

  /* L3 AVL Tree data */
  avlTree_t           snoopL3AvlTree;
  avlTreeTables_t    *snoopL3TreeHeap;
  snoopL3InfoData_t  *snoopL3DataHeap;

  /* Semaphores */
  void               *snoopMsgQSema;
  void               *snoopAvlTreeSema;           // PTin added

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

  /* Number of snooping instances supported */
  L7_uchar8      maxSnoopInstances;
 
  /* Flag to specify if IPv6 hop by hop option with rtr alert is
     supported supported or not */
  L7_BOOL        ipv6OptionsSupport;

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

  /* Number of snooping queriers supported */
  L7_ushort16    enabledSnoopQuerierVlans;

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
    snoopDot1sIntfChangeParms_t snoopDot1sIntfChangeParms;
    struct {
      L7_uint16 admin;
      L7_uint16 vlanId;
      L7_uint16 CoS_intPrio;
    } mode;
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

// PTin added
extern L7_RC_t igmp_generate_packet_and_send(L7_uint32 vlan, L7_uint8 source_type, L7_uint32 groupIP);

/* End of function prototypes */
#endif /* SNOOPING_H */

