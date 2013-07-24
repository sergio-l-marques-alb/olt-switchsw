/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  mgmd.h
*
* @purpose   Defintions for IGMP, MLD
*
* @component MGMD (IGMP+MLD)
*
* @comments  none
*
* @create    03/10/2005
*
* @author
*
* @end
*
**********************************************************************/
#ifndef _IGMP_H
#define _IGMP_H

#include "l7_common.h"
#include "l3_addrdefs.h"

/*********************************************************************
                      UTIL INCLUDES
**********************************************************************/                      
#include "mgmd_debug.h"
#include "intf_bitset.h"
#include "avl_api.h"
#include "l7sll_api.h"
#include "l7apptimer_api.h"
#include "l7handle_api.h"
#include "buff_api.h"
#include "mcast_wrap.h"

/*********************************************************************
                      MCAST COMPONENT INCLUDES
**********************************************************************/                      
#include "mfc_api.h"
#include "l7_ip_api.h"
#include "l7_mgmd_api.h"
#include "l7_mcast_api.h"
#include "mcast_defs.h"
#include "mcast_inet.h"
#include "mcast_api.h"
#include "l3_mcast_commdefs.h"
#include "mgmd_api.h"
#include "heap_api.h"
/*********************************************************************
                      CONSTANT MACROS
**********************************************************************/                      
/* Macros for Numbers used in code. */
#define MGMD_ZERO      0
#define MGMD_ONE       1
#define MGMD_TWO       2
#define MGMD_THREE     3
#define MGMD_FOUR      4
#define MGMD_FAILURE  -1

#define MLD_RESERVED_SCOPE  MGMD_ZERO
#define MLD_INTERFACE_LOCAL_SCOPE  MGMD_ONE
#define MLD_SITE_LOCAL_SCOPE       5


 /*
    MGMD_MAX_TIMERS =  
                     {{timers for maxgroups in mgmd (grptimer + v1hostTimer + v2HostTimer) }  +
                      {timers for max sources in mgmd } +
                      {timers for max mgmd interfaces (query timer + querier timer)} +
                      {MRP timer} + {buffer of 2}}
            P.S : This is per address family control block.
  */
                                           

#define MGMD_MAX_TIMERS     (((MGMD_MAX_GROUPS * 3) + MGMD_MAX_SRC_RECORDS + (2 * MAX_INTERFACES) + 3) + (MGMD_PROXY_MAX_TIMERS))
#define MGMD_DBG_MSG_SIZE   256
#define MGMD_PKT_SIZE_MAX   L7_MULTICAST_MAX_IP_MTU


#define IGMP_MEMBERSHIP_QUERY           0x11    /* membership query         */
#define IGMP_V1_MEMBERSHIP_REPORT       0x12    /* Ver. 1 membership report */
#define IGMP_V2_MEMBERSHIP_REPORT       0x16    /* Ver. 2 membership report */
#define IGMP_V2_LEAVE_GROUP             0x17    /* Leave-group message      */
#define IGMP_V3_MEMBERSHIP_REPORT       0x22    /* Ver. 3 membership report   */

#define MLD_LISTENER_QUERY              130
#define MLD_V1_LISTENER_REPORT          131
#define MLD_LISTENER_DONE               132
#define MLD_V2_LISTENER_REPORT          143

#define MGMD_MAX_GROUPS                 L7_MGMD_GROUPS_MAX_ENTRIES

/* The factor 2 to  cover both group-specific and group-source-specific queries */ 
#define MGMD_MAX_QUERY_REQ_NODES        (2 * MGMD_MAX_GROUPS)
#define MGMD_MAX_TIMER_NODES            (2 * MGMD_MAX_GROUPS) 

#define MGMD_MAX_SRC_RECORDS            (10*MGMD_MAX_GROUPS)
#define MGMD_BUFFPOOL_NAME              "mgmdBufferPool"

#define IGMP_QUERY_INTERVAL                 125
#define IGMP_QUERY_RESPONSE_INTERVAL        10
#define MGMD_MRP_INFORM_TIMEOUT             5

/* Upon Start-up, the first Query will be sent after this delay (in seconds).
 */
#ifdef _L7_OS_LINUX_
#define MGMD_STARTUP_QUERY_DELAY            2 /* Linux takes a little more time
                                               * upon boot-up */
#else
#define MGMD_STARTUP_QUERY_DELAY            1
#endif /* _L7_OS_LINUX_ */

#define IGMP_PKT_MIN_LENGTH       8
#define IGMP_V3_PKT_MIN_LENGTH    12
#define MLD_PKT_MIN_LENGTH        24
#define MLD_V2_PKT_MIN_LENGTH     28
#define IGMPV3_MAX_QUERY_DATA_LEN ((MGMD_MAX_QUERY_SOURCES * sizeof(L7_IP_ADDR_t)) + \
                                    sizeof(L7_mgmdv3Query_t) - \
                                    sizeof(L7_mgmdMsg_t) - sizeof(L7_uint32))

/*#define IGMP_QUERIER 0x40000000   igmp querier */
#define IGMP_QUERIER              31
#define MRT_IGMP_TTL              1
#define AVL_TREE_TYPE             0x10
#define MAX_FLAGS                 32

#define MGMD_NEW_GROUP            0x1
#define MGMD_GROUP_MODE_UPDATED   0x2
#define MGMD_MRP_REQUESTED_GROUP  0x4
#define MGMD_MRP_INFORM_FAILED    0x8

#define MGMD_ALLOC(family,size) \
        heapAlloc(mgmdMapHeapIdGet(family),size,__FILE__,__LINE__)
#define MGMD_FREE(family,pMem) \
        heapFree(mgmdMapHeapIdGet(family),pMem)

/* Considering the max frame size as 1500 bytes following number is arrived.    */
#define MGMD_MAX_GROUP_RECORDS     200

#undef assert
#define assert(exp) \
    ((exp) ? MGMD_ZERO : printf ("MGMD: assertion (%s) failed " \
                                  "at line %d file %s\n", \
                                  #exp, __LINE__, __FILE__))


/*********************************************************************
                      ENUMS
**********************************************************************/                      
typedef enum 
{
  L7_MGMD_GRP_TIMER,
  L7_MGMD_SRC_TIMER,
  L7_MGMD_QUERY_TIMER,
  L7_MGMD_QUERIER_TIMER,
  L7_MGMD_V1HOST_TIMER,
  L7_MGMD_V2HOST_TIMER,
  L7_MGMD_GRP_QUERY_TIMER,
  L7_MGMD_MRP_INFORM_TIMER  
} MGMD_TIMER_t;




/*********************************************************************
                      TYPEDEFS
**********************************************************************/                      
/* IPv6 Packet including IPv6 header for checksum computation of ICMPv6 packet */
typedef struct ipv6pkt_s
{
  L7_in6_addr_t srcAddr;        /* Source Address */
  L7_in6_addr_t dstAddr;        /* Destination Address */
  L7_uint32     dataLen;        /* Upper-Layer Packet Length */
  L7_uchar8     zero[3];        /* Zero's */
  L7_uchar8     nxtHdr;         /* Next Header */
  L7_uchar8     icmpv6pkt[1500];/* ICMPv6 Packet */
}ipv6pkt_t;

typedef struct mgmd_query_req_s
{
  L7_sll_member_t  *next;
  L7_inet_addr_t    group;
  L7_APP_TMR_HNDL_t timer;
  L7_uint32         query_timer_handle; /* to safeguard timeout functions */    
  L7_int32          count;
  L7_inet_addr_t    *qSrcs[MGMD_MAX_QUERY_SOURCES];
  L7_int32          numSrcs;
  void             *mgmdCB;  /* mgmd control block stored to retrieve info 
                                while destroying the node itself */
}mgmd_query_req_t; 

/* MGMD Info Flags bitset structure */
typedef struct _flags_t {
  L7_uchar8  bits[MCAST_BITX_NUM_BITS(MAX_FLAGS)];
} flags_t;

typedef enum
{
  MGMD_STATS_RX,
  MGMD_STATS_TX,
  MGMD_STATS_MAX
}MGMD_STATS_TYPE_t;

typedef enum
{
  MGMD_QUERY_V1,
  MGMD_QUERY_V2,
  MGMD_QUERY_V3,
  MGMD_REPORT_V1,
  MGMD_REPORT_V2,
  MGMD_REPORT_V3,
  MGMD_LEAVE_V2,
  MGMD_REPORT_V3_ISINC,
  MGMD_REPORT_V3_ISEXC,
  MGMD_REPORT_V3_TOINC,
  MGMD_REPORT_V3_TOEXC,
  MGMD_REPORT_V3_ALLOW,
  MGMD_REPORT_V3_BLOCK,
  MGMD_CTRL_PKT_MAX
}MGMD_CTRL_PKT_TYPE_t;

typedef struct mgmd_info_s 
{
  flags_t             flags;              
  L7_uint32           ifIndex;            /* Router interface index */
  L7_APP_TMR_HNDL_t   mgmd_query_timer;
  L7_uint32           mgmd_query_timer_handle; /* to safeguard timeout functions */    
  int                 mgmd_query_count;
  L7_inet_addr_t      mgmd_querierIP;
  L7_uint32           querierQueryInterval;  /* Stores querier's query interval when existing 
                                                router is non-querier */
  L7_APP_TMR_HNDL_t   mgmd_querier_timer;
  L7_uint32           mgmd_querier_timer_handle; /* to safeguard timeout functions */    
  L7_sll_t            ll_query_reqs;      /* Nodes are of the type mgmd_query_req_t */ 
  int                 numOfJoins;         /* The number of times a group membership has been added on this interface*/
  int                 numOfGroups;        /* The current number of groups registered for this interface. */
  int                 Wrongverqueries;    /* The number of queries received whose IGMP/MLD version does not match Interface Version*/
  int                 querierUpTime;      /* time since Querier was last changed.*/
  int                 querierExpiryTime;  /* The amount of time remaining before the Other Querier Present Timer expires.*/
  int                 ver1QuerierTime;    /* The time remaining until the host assumes that there are no IGMPv1 routers present on the interface */
  int                 version1HostTimer;  /* added for mib */
  struct mgmd_cb_s   *mgmdCB;
  mgmdIntfConfig_t    intfConfig;
  L7_uint32           intfStats[MGMD_CTRL_PKT_MAX][MGMD_STATS_MAX];  /* Interface specific statistics for
                                                                        various control packets*/
} mgmd_info_t;

typedef struct mgmd_host_info_s mgmd_host_info_t;

typedef struct mgmd_counters_s
{
  L7_uint32              TotalPacketsReceived;
  L7_uint32              TotalPacketsSent;
  L7_uint32              QueriesReceived;
  L7_uint32              QueriesSent;
  L7_uint32              ReportsReceived;
  L7_uint32              ReportsSent;
  L7_uint32              LeavesReceived;
  L7_uint32              LeavesSent;
  L7_uint32              BadChecksumPackets;
  L7_uint32              MalformedPackets;
  L7_uint32              WrongTypePackets;

}mgmd_counters_t;

typedef struct mgmdGroupInfo_s
{
  L7_uint32           rtrIfNum;
  L7_uint32           mode;
  L7_inet_addr_t      group;
  L7_uint32           numSrcs;
  mrp_source_record_t *sourceList[MGMD_MAX_QUERY_SOURCES];
}mgmdGroupInfo_t;

/* IGMPv3/MLDv2 Group Record */
typedef struct L7_mgmdv3GroupRecord_s
{
  L7_uchar8         mgmdGrpRecType;       /* Group Record type (see below) */
  L7_uchar8         mgmdGrpRecAuxDataLen; /* Aux Data Len */
  L7_ushort16       mgmdGrpRecNumSrcs;    /* Number of source addresses in this group record */
  L7_inet_addr_t    mgmdGrpRecGroupAddr;  /* Group address */
  L7_inet_addr_t    mgmdGrpRecSrcs[MGMD_MAX_QUERY_SOURCES];  
  /* mgmdGrpRecNumSrcs source addresses */
  /* Followed potentially by auxiliary data */
} L7_mgmdv3GroupRecord_t;

/* IGMPv3/MLDv2 Membership Report */
typedef struct L7_mgmdv3Report_s
{
  L7_uchar8              mgmdReportType;          /* Message type (v3 report = 0x22) */
  L7_uchar8              mgmdReportReserved1;     /* Reserved */
  L7_ushort16            mgmdReportChecksum;      /* checksum */
  L7_ushort16            mgmdReportReserved2;     /* Reserved */
  L7_ushort16            mgmdReportNumGroupRecs;  /* Number of group records in this frame */
  L7_mgmdv3GroupRecord_t mgmdReportGroupRecs[MGMD_MAX_GROUP_RECORDS];
  /* mgmdReportNumGroupRecs group records */
} L7_mgmdv3Report_t;

typedef struct mgmd_cb_s
{
  L7_uint32               proto;                             /* MCAST_PROTOV4 or MCAST_PROTOV6 */
  interface_bitset_t      mgmd_querier_if_mask;              /* mask to flg Querier,non-Qurier*/
  mgmd_info_t             mgmd_info[MAX_INTERFACES];
  avlTreeTables_t        *mgmdMembershipTreeHeap;            /* Data structure reqd. by AVL tree implementation */
  void                   *mgmdMembershipDataHeap;            /* Data structure reqd. by AVL tree implementation */
  avlTree_t               membership;                        /* connected groups */
  mgmd_host_info_t       *mgmd_host_info;                    /* IGMP/MLD Proxy global Data Structure*/
  L7_uint32               mgmd_proxy_status;
  L7_uint32               mgmd_proxy_ifIndex;
  L7_uint32               mgmd_proxy_restartCount;
  L7_uint32               sockfd;
  L7_BOOL                 checkRtrAlert;                     /* this flag to be used only on recption of 
                                                                IGMP control packets as this validation 
                                                                is mandatory for MLD packets  */
  L7_BOOL                 checkForTOS;                       /* this flag to be used only on reception of 
                                                                IGMPv3 control packets. */
  L7_inet_addr_t          all_hosts;
  L7_inet_addr_t          all_routers;
  mgmd_counters_t         counters;

  /* Following timer is basically introduced as a backup mechanism to inform the MRP about the groups and sources information 
     which were not informed during the normal packet processing because of the queue failures.This scenario is possible when 
     IGMP packets are pumped at high rate for different set of groups and different set of sources.This causes, at a stretch, lot
     of MCAST_EVENT_MGMD_GROUP_UPDATE events to be posted to MRP which in turn causes the MRP queues to be full and so further 
     coming events won't be posted to MRP.With the kind of architecture right now we have once the event is missed to inform 
     to MRP there is no mechanism to inform it again to MRP and so this timer inroduced.

     This timer expires every 5 seconds and searches for all the group nodes which are all not informed to MRP during the regular 
     packet processing and infomrs them to MRP.In this way, even if the the group udate event is lost because of queue failures
     it will again infomred latter after 5 secs.
   */  
  L7_APP_TMR_HNDL_t      mgmd_mrp_timer;                  
  L7_uint32              mgmd_mrp_timer_handle; /* to safeguard timeout functions */    
  L7_APP_TMR_CTRL_BLK_t  timerHandle;     /* MGMD App Timer Control Block */
  handle_list_t          *handle_list;
  void                   *handleListMem;
  L7_mgmdv3Report_t      v3Report;
  L7_uchar8              pktRxBuf[MGMD_PKT_SIZE_MAX+L7_IP6_HEADER_LEN]; /* Buffer used to Receive PIM-SM Packets */
  L7_uchar8              pktTxBuf[MGMD_PKT_SIZE_MAX]; /* Buffer used to Transmit PIM-SM Packets */
  L7_inet_addr_t         querySrcList[MGMD_MAX_QUERY_SOURCES];   /* Buffer for calculation of source list in Query*/
} mgmd_cb_t; 

typedef struct mgmd_group_timer_data_s 
{
  void *mgmdCb;
  void *groupRec;
}mgmd_group_timer_data_t; 

typedef struct mgmd_timer_event_info_s
{
  L7_uint32                  proto;
  L7_uint32                  intfNum; 
  mgmd_query_req_t*          qreq;
} mgmd_timer_event_info_t;

typedef struct mgmd_group_s 
{
  L7_uint32             rtrIfNum;        /* Search Key for the AVL tree*/ 
  L7_inet_addr_t        group;           /* Search Key for the AVL tree*/
  L7_inet_addr_t        reporter;        /* Search Key for the AVL tree*/    
  time_t                ctime;           /* time received */
  L7_int32              interval;        /* timeout interval since group specific query on
                                          non-querier router sets different timeout value */
  mgmd_version          groupCompatMode; /* Group compatibility mode: v1/v2/v3 */
  MGMD_FILTER_MODE_T    filterMode;      /* Source filter mode: INCLUDE/EXCLUDE */
  L7_uchar8             mrpUpdated;      
  L7_sll_t              sourceRecords;   /* Nodes of Type mgmd_source_record_t */
  L7_APP_TMR_HNDL_t     v1HostTimer;
  L7_int32              v1HostPresentFlag;
  L7_APP_TMR_HNDL_t     v2HostTimer;
  L7_int32              v2HostPresentFlag;
  L7_APP_TMR_HNDL_t     groupTimer;    /* group timeout */  
  L7_uint32             grp_timer_handle; /* to safeguard timeout functions */

  L7_uint32             v2host_timer_handle; /* to safeguard timeout functions */  
  L7_uint32             v1host_timer_handle; /* to safeguard timeout functions */  
  mgmd_group_timer_data_t  timerData;
  mgmd_timer_event_info_t  *grpQueryData;        /* pointer for timer data for grp-specific query*/
  mgmd_timer_event_info_t  *grpSrcQueryData;    /* pointer for timer data for grp-n-src-specific query*/  
  void *                dummy;           /* required by the AVL tree implementation */
} mgmd_group_t;
typedef struct mgmd_source_timer_data_s
{
  void *srcRec;
  void *groupRec;
}mgmd_source_timer_data_t;

typedef struct mgmd_source_record_s
{
  L7_sll_member_t     *next;
  L7_inet_addr_t      sourceAddress;
  time_t              sourceCtime;
  L7_int32            sourceInterval;
  L7_APP_TMR_HNDL_t   sourceTimer;    /* source  timeout */  
  L7_uint32           src_timer_handle; /* to safeguard timeout functions */
  void                *mgmdCB;        /* mgmd control block stored to retrieve info 
                                         while destroying the node itself */
  mgmd_source_timer_data_t timerData;

} mgmd_source_record_t;


/*************************************************
 *************************************************
 The following FOUR structures are from l7_packet.h
 They are being references by IGMP snooping as well.

 Since IGMP is v6 compliant, we need to change all 
 IP addresses to Inet_addr. BUT as IGMP snooping is
 not v6 compliant yet (May 2006), we cannot have
 common structures. Hence these structures are moved
 here locally. Once IGMP snooping becomes v6 compliant,
 these structures can be deleted from here
 *************************************************
 *************************************************/

/* MGMD message */
typedef struct L7_mgmdMsg_s
{
  L7_uchar8         mgmdType;         /* message type */
  L7_uchar8         igmpMaxRespTime;  /* maximum query response time */
  L7_ushort16       mgmdChecksum;     /* checksum */
  L7_ushort16       mldMaxRespTime;   /* maximum query response time */
  L7_ushort16       mgmdReserved;     /* Reserved */
  L7_inet_addr_t    mgmdGroupAddr;    /* group address */
}L7_mgmdMsg_t;

/* IGMPv3/MLDv2 Membership Query */
typedef struct L7_mgmdv3Query_s
{
  L7_mgmdMsg_t    mgmdMsg;          /* contains L7_mgmdMsg_t above */
  L7_uchar8       mgmdResvSflagQRV; /* 4 bits reserved,  1 bit S Flag, 3 bits Querier's Robustness Variable */
  L7_uchar8       mgmdQQIC;         /* Querier's Query Interval */
  L7_ushort16     mgmdQueryNumSrcs; /* Number of source addresses in this frame */
  L7_inet_addr_t  mgmdQuerySrcs[MGMD_MAX_QUERY_SOURCES];    
  /* mgmdQueryNumSrcs source addresses */
} L7_mgmdv3Query_t;

/*********************************************************************
                      FUNCTION PROTOTYPES
**********************************************************************/                      

/*********************************************************************
*
* @purpose To determine if the given router interface is a MGMD 
*          querier
*
* @param   mgmdCB - MGMD Control Block
* @param   rtrIfNum   @b{ (input) } Router Interface Number 
*
* @returns TRUE -  The given interface is an MGMD querier
*          FALSE -  The give interface is NOT an MGMD querier
*
* @notes
*
* @end
*********************************************************************/
extern L7_BOOL mgmd_is_querier (mgmd_cb_t *mgmdCB, L7_uint32 rtrIfNum);

/*********************************************************************
*
* @purpose  Send IGMP / MLD packet to McastMap
*
* @param    mgmdCB -> MGMD Control Block
*           Destination Address
*           Data (Payload)
*           Data Length
*           Interface on which packet is to be sent out
*
* @returns  
*           
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t mgmd_send(mgmd_cb_t *mgmdCB, L7_inet_addr_t dst, L7_uchar8 *data,
                         L7_uint32 datalen, L7_uint32 rtrIntf,
                         L7_inet_addr_t *ipAddr);

/*********************************************************************
 * @purpose   This function is used to send timer events
 *        
 * @param     Timer Control Block
 *
 * @returns   None
 *
 * @notes     None
 * @end
 *********************************************************************/
extern void mgmdTimerExpiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* mgmdEB);

/*********************************************************************
* @purpose  Retrieve group information learned by MGMD
*
* @param    mgmdCB            @b{(input)}   MGMD Control Block
* @param    pRtrIfNum         @b{(input)}   router Interface number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API is used by MRPs to get the group information learned 
*           by MGMD whenever MRP is disabled and enabled at run-time
*
* @end
*********************************************************************/
extern L7_RC_t mgmdMRPGroupInfoGet(mgmd_cb_t *mgmdCB, L7_uint32 *pRtrIfNum);

/*********************************************************************
* @purpose Function to Start the Timer (Both Set and update Timer)
*
* @param   mgmdCB   @b{(input)} Control Block.
*          pFunc     @b{(input)} Function Pointer.
*          pParam    @b{(input)} Function Parameter
*          timeout   @b{(input)} Timeout in seconds
*          tmrHandle     @b{(output)} Timer Handle
*           
*                            
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t mgmdUtilAppTimerSet (mgmd_cb_t *mgmdCB,
                      L7_app_tmr_fn pFunc,
                      void *pParam,
                      L7_APP_TMR_HNDL_t *tmrHandle,
                      L7_uint32 timeOut,
                      MGMD_TIMER_t timerType);


void mgmd_receive(mcastControlPkt_t *message);
void mgmd_group_leave (mgmd_info_t *mgmd_info, L7_inet_addr_t group);
void mgmd_v3_query_form(L7_uint32 family, L7_mgmdv3Query_t *v3Query, L7_uchar8 *buf);
void mgmd_v3membership_query(mgmd_info_t *mgmd_info, L7_inet_addr_t group, 
                                    L7_inet_addr_t source, L7_uchar8 max_resp_code, 
                                    L7_mgmdv3Query_t *v3Query);
void mgmd_membership_query(mgmd_info_t *mgmd_info, L7_inet_addr_t group,
                                  L7_inet_addr_t source, L7_int32 max_resp_time);
void mgmd_v2membership_report(mgmd_info_t*    mgmd_info, 
                                     L7_inet_addr_t  group, 
                                     L7_inet_addr_t  reporter);
L7_RC_t mgmd_v3_report_form(L7_uint32 family, L7_mgmdv3Report_t *v3Report, 
                                   L7_uchar8 *buf);

void mgmd_v3membership_report(mgmd_info_t       *mgmd_info, 
                                     L7_mgmdv3Report_t *v3Report, 
                                     L7_inet_addr_t     reporter);
L7_RC_t mgmdIntfReportStatsUpdate(mgmd_cb_t *mgmdCB,
                               L7_uint32 rtrIfNum,
                               MGMD_GROUP_REPORT_TYPE_t reportType,
                               MGMD_STATS_TYPE_t statsType);
L7_RC_t mgmdIntfStatsUpdate (L7_uchar8 family,
                             L7_uint32 rtrIfNum,
                             MGMD_CTRL_PKT_TYPE_t msgType,
                             MGMD_STATS_TYPE_t statsType);
void mgmdIntfStatsClear(L7_uint32 family, L7_uint32 rtrIfNum);

extern L7_RC_t
mgmd_v3_report_init (mgmd_cb_t *mgmdCB,
                     L7_mgmdv3Report_t *v3Report);

extern L7_int32 mgmd_decode_max_resp_code(L7_uchar8 family, L7_int32 max_resp_code);
extern L7_RC_t
mgmd_v3_query_init (mgmd_cb_t *mgmdCB,
                    L7_mgmdv3Query_t *v3Query);
#endif /* _IGMP_H */
