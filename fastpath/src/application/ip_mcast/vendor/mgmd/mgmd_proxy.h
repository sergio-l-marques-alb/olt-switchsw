/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  mgmd_proxy.h
*
* @purpose   MGMD-Proxy
*
* @component MGMD (IGMP+MLD)
*
* @comments  none
*
* @create    03/10/2005
*
* @author   ryadagiri
*
* @end
*
**********************************************************************/

#ifndef _L7_IGMP_PROXY_H
#define _L7_IGMP_PROXY_H

#include "mcast_api.h"
#include "mgmd_proxy_api.h"
#include "heap_api.h"
/*********************************************************************
                      CONSTANT MACROS
**********************************************************************/  
/* Other version querier timer = ((RobustnessVar)*(QueryInterval))+ 1/2(Query_Rsp_Interval). *
 * Other version querier timer = (2*125)+10 = 260s.                                      */

#define IGMP_VER2_QUERIER_TIMER     260
#define IGMP_VER1_QUERIER_TIMER     260
#define IGMP_PROXY_GROUP_TIMER      10
#define IGMP_PROXY_INTERFACE_TIMER  10

/* MALI = ((RobustnessVar)*(QueryInterval))+(Query_Rsp_Interval). *
 * MALI = (2*125)+10 = 260s.                                      */
#define IGMP_PROXY_MALI    260     /* ((2*125)+10)Secs */

#define MGMD_PROXY_MRT_MAX_IPV4_ROUTE_ENTRIES   platIpv4McastRoutesMaxGet()
#define MGMD_PROXY_MRT_MAX_IPV6_ROUTE_ENTRIES   platIpv6McastRoutesMaxGet()

#define MGMD_HOST_MAX_GROUPS               (MGMD_MAX_GROUPS + 1)
#define MGMD_HOST_MAX_SRC_RECORDS          (10*MGMD_HOST_MAX_GROUPS)
#define MGMD_MAX_UNSOLICITED_REPORTS       MGMD_HOST_MAX_GROUPS
        /*  PROXY MAX TIMERS = {{grpResponsetmrs for max groups} +
                                {timers for max unsolicited reports} +
                                {V1 version timer} + {V2 version timer} + 
                                {General Response timer} + 2(buffer) }           
            P.S : This is per address family control block.
        */
#define MGMD_PROXY_MAX_TIMERS       (MGMD_MAX_GROUPS +  MGMD_MAX_UNSOLICITED_REPORTS + 5)
#define MGMD_HOST_BUFFPOOL_NAME      "mgmdHostBufferPool"
#define MGMD_INTF_BIT_SIZE           MCAST_BITX_NUM_BITS(MCAST_MAX_INTERFACES+1)
#define MGMD_ADMIN_SCOPE_BUFFPOOL_NAME "mgmdProxyAdmScpPool"

#define MGMD_PROXY_ALLOC(family,size) \
        heapAlloc(mcastMapHeapIdGet(family),size,__FILE__,__LINE__)
#define MGMD_PROXY_FREE(family,pMem) \
        heapFree(mcastMapHeapIdGet(family),pMem)

#define MGMD_PROXY_REPORT_RATE_LIMIT 105 /* Max reports to send out on proxy interface */
                                         /* Due to the possibility of non-availability
                                          * of MBUFs on the recieving router,
                                          * this value is reduced to 105; this means 
                                          * 105 packets will be sent per second on 
                                          * the proxy interface.
                                          */

/*********************************************************************
                      ENUMS
**********************************************************************/                      
typedef enum _mgmd_proxy_timer
{
  L7_MGMD_GRP_RSP_TIMER,
  L7_MGMD_GENERAL_RSP_TIMER,
  L7_MGMD_VERSION1_TIMER,
  L7_MGMD_VERSION2_TIMER,
  L7_MGMD_UNSOLICITED_REPORT_TIMER
} mgmd_proxy_timer;


/*********************************************************************
                      TYPEDEFS
**********************************************************************/                      
typedef L7_uint32 (*router_report_events_t)(mgmd_cb_t *mgmdCB, 
                                            L7_uint32 eventType,L7_uint32 ifNum, 
                                            L7_inet_addr_t group, L7_uint32 numSrcs, 
                                            L7_inet_addr_t *sourceList);

typedef L7_uint32  (*router_timer_events_t)(mgmd_cb_t *mgmdCB, 
                                            L7_uint32 eventType, L7_uint32 ifNum, 
                                            L7_inet_addr_t group, 
                                            L7_inet_addr_t source, L7_uint32 numSrcs, 
                                            L7_inet_addr_t *sourceList);


typedef struct mgmd_proxy_unsolicited_rpt_s
{
  L7_sll_member_t   *next;
  L7_inet_addr_t    group;
  L7_APP_TMR_HNDL_t timer;
  L7_uint32         timer_handle; /* to safeguard timeout functions */  
  L7_uint32         retryCount;
  L7_uint32         reportId;
  L7_inet_addr_t    srcList[MGMD_MAX_QUERY_SOURCES];
  L7_uint32         numSrcs;
  L7_uint32         grpRecType;
  void              *mgmdCB;     /* mgmd control block stored to retrieve info 
                                         while destroying the node itself */
} mgmd_proxy_unsolicited_rpt_t;


struct mgmd_host_info_s
{
  L7_uchar8                 family;
  L7_inet_addr_t            querierIP;          /* IP address of the querier*/
  L7_APP_TMR_HNDL_t         ver1_querier_timer; /* version 1 querier timer */
  L7_APP_TMR_HNDL_t         ver2_querier_timer; /* version 2 querier timer */
  L7_APP_TMR_HNDL_t         gen_rsp_timer;      /* Interface timer to respond to general querier */
  L7_uint32                 rtrIntf;
  L7_sll_t                  ll_unsolicited_reports; /* List with nodes of type "mgmd_proxy_unsolicited_rpt_t */
  L7_uint32                 numOfGroups;
  mgmd_version              hostCompatibilityMode;
  L7_uint32                 proxyIntfStats[MGMD_CTRL_PKT_MAX][MGMD_STATS_MAX];  /* Proxy specific statistics for
                                                              various control packets to be displayed in 
                                                              "show ipv6 mld-proxy interface" */
  router_report_events_t    router_report_event;
  router_timer_events_t     router_timer_event;
  avlTreeTables_t          *mgmdHostMembershipTreeHeap;       /* Data structure reqd. by AVL tree implementation */
  void                     *mgmdHostMembershipDataHeap;       /* Data structure reqd. by AVL tree implementation */
  avlTree_t                 hostMembership;                   /* Groups and source Info */
  avlTreeTables_t          *mgmdProxyCacheTreeHeap;           /* Data structure reqd. by AVL tree implementation */
  void                     *mgmdProxyCacheDataHeap;           /* Data structure reqd. by AVL tree implementation */
  avlTree_t                 mgmdProxyCache;                   /* Forwarding Entries */
  L7_sll_t                  asbList;                          /* AdminScope Boundary List of type mgmdProxyASBNode_t */
  L7_BOOL                   mgmdProxyUpStreamDisable;
  L7_uint32                 unsolicitedReportInterval;
  L7_uint32                 robustness;
  L7_uint32                 configVersion;
  L7_inet_addr_t            reportSrcList1[MGMD_MAX_QUERY_SOURCES];
  L7_inet_addr_t            reportSrcList2[MGMD_MAX_QUERY_SOURCES];
  L7_uint32                 reportRateLimitCnt;
};

typedef struct mgmd_host_group_s
{
  L7_inet_addr_t     group;          /* group address */
  time_t             uptime;         /* time received  */ 
  L7_inet_addr_t     last_reporter;  /* last reporter group address */
  MGMD_FILTER_MODE_T filterMode;     /* Source Filter Mode */
  L7_APP_TMR_HNDL_t  grp_rsp_timer;  /* Timer respond to group queries */
  L7_uint32          grp_rsp_timer_handle; /* to safeguard timeout functions */  
  interface_bitset_t grpPortList;    /* (GPL) Mask of interfaces configured for this Group Address */
  L7_sll_t           sourceRecords;  /* list of source records for each group */
  MGMD_HOST_STATE_T  hostState;
  L7_BOOL            grpSuppressed;
  L7_BOOL            sendReportUponQuery; /* Introduced for Rate-Limiting IGMP
                                           * Group Membership Reports.
                                           * Update to TRUE on receiveing Query.
                                           * Update to FALSE on trasmitting
                                           * Report.
                                           */
  void              *mgmdCB;
  /* AVL TREE requires this as last */  
  void *avlData;
}mgmd_host_group_t;


typedef struct mgmd_host_source_record_s
{
  L7_sll_member_t    *next;
  L7_inet_addr_t     sourceAddress;
  interface_bitset_t srcPortList;     /* (SPL) Mask of interfaces configured for this Source Address */
  time_t             sourceCtime;     /* Time when this source was created */
  L7_int32           sourceInterval;  /* Timeout Interval for this Source */
  L7_BOOL            sendQueryReport; /* Send this Source in the Query Report */
  L7_BOOL            createFlag;      /* create flag to use SPL or GPL */ 
  void               *mgmdCB;         /* mgmd control block stored to retrieve info 
                                         while destroying the node itself */
} mgmd_host_source_record_t;

typedef struct mgmdProxyCacheEntry_s
{
  L7_inet_addr_t srcAddr; /* Index for the MRT Entry*/
  L7_inet_addr_t grpAddr; /* Index for the MRT Entry*/
  L7_uint32      upstrmRtrIfNum; /* Upstream Router interface index */
  L7_uchar8      oifList[MGMD_INTF_BIT_SIZE];
  L7_uint32      entryUpTime;              /* Entry's Create Time */
  /* AVL TREE requires this as last */  
  void *avlData;
}mgmdProxyCacheEntry_t;


typedef struct mgmdProxyASBNode_s
{
  L7_sll_member_t *next;
  L7_inet_addr_t  grpAddr;                       /* Group Address */
  L7_inet_addr_t  grpMask;                       /* Group Mask */
  L7_uchar8       intfMask[MGMD_INTF_BIT_SIZE]; /* Interface Mask */
  L7_uchar8       family;
}mgmdProxyASBNode_t;


/*********************************************************************
                      FUNCTION PROTOTYPES
**********************************************************************/  

/*****************************************************************
* @purpose Function routine provided for MGMD to repay group and
*          source timers
*
* @param    mgmdCB       @b{ (input) }   MGMD Control Block
* @param    eventType    @b{ (input) }   timer event type
* @param    ifNum        @b{ (input) }   Router Interface Number
* @param    group        @b{ (input) }   groups address
* @param    numSrcs      @b{ (input) }   number of sources
* @param    sourceList   @b{ (input) }   list of sources
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_uint32 mgmd_proxy_routerReportEvent(mgmd_cb_t *mgmdCB, 
                                       L7_uint32 eventType, L7_uint32 ifNum, 
                                       L7_inet_addr_t group, L7_uint32 numSrcs, 
                                       L7_inet_addr_t *sourceList);

/*****************************************************************
* @purpose Function routine provided for MGMD to repay group and
*          source timers
*
* @param    mgmdCB       @b{ (input) }   MGMD Control Block
* @param    eventType    @b{ (input) }   timer event type
* @param    ifNum        @b{ (input) }   Router Interface Number
* @param    group        @b{ (input) }   groups address
* @param    group_source @b{ (input) }   source address of the host
* @param    numSrcs      @b{ (input) }   number of sources
* @param    sourceList   @b{ (input) }   list of sources
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_uint32 mgmd_proxy_routerTimerEvents(mgmd_cb_t *mgmdCB, 
                                       L7_uint32 eventType, L7_uint32 ifNum, 
                                       L7_inet_addr_t group,
                                       L7_inet_addr_t group_source, L7_uint32 numSrcs, 
                                       L7_inet_addr_t *sourceList);
/*****************************************************************
* @purpose Initialzes the MGMD Proxy interface 
*
* @param    mgmdCB    @b{ (input) }   MGMD Control Block
* @param    rtrIntf   @b{ (input) }   Router Interface Number
* @param    mode      @b{ (input) }   L7_ENABLE / L7_DISABLE
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   Either enable or disable the proxy inteface 
*
* @end
*********************************************************************/
L7_RC_t mgmd_proxy_interface (mgmd_cb_t   *mgmdCB, L7_uint32    rtrIntf, 
                              L7_uint32    mode,mgmdIntfConfig_t *mgmdIntfInfo);
/*****************************************************************
* @purpose  Handles the v3 queries on the proxy interface 
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    group             @b{ (input) }   Group Address
* @param    source            @b{ (input) }   Source Address
* @param    max_resp_code     @b{ (input) }   MAx Response Code
* @param    v3Query           @b{ (input) }   V3 Query Structure
*
* @returns None
*
* @notes  depending upon the query type it starts the timer either the 
*         group timer for group query and grp src specific query or 
*         interface timer for general queries 
* @end
*********************************************************************/
void mgmd_proxy_v3membership_query_process(mgmd_cb_t *mgmdCB, 
                                           L7_inet_addr_t group, 
                                           L7_inet_addr_t source, 
                                           L7_uchar8 max_resp_code, 
                                           L7_mgmdv3Query_t *v3Query);

/*****************************************************************
* @purpose  Handles the v2 and v1 queries on the proxy interface
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    group             @b{ (input) }   Group Address
* @param    source            @b{ (input) }   Source Address
* @param    max_resp_time     @b{ (input) }   MAx Response Time
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  depending upon the query type it starts the timer either the 
*         group timer for group query or interface timer for general queries
* @end
*********************************************************************/
void mgmd_proxy_membership_query_process (mgmd_cb_t *mgmdCB, 
                                          L7_inet_addr_t group, 
                                          L7_inet_addr_t source, 
                                          L7_uint32 max_resp_time);

/*****************************************************************
* @purpose Handles v1/v2 reports on the proxy interface 
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    group             @b{ (input) }   Group Address
* @param    source            @b{ (input) }   Source Address
*
* @returns None
*
* @notes if the interface is in v1/v2 mode it checks wether it has send the 
*        report previously. if no it send the report accordingly
* @end
*********************************************************************/
void mgmd_proxy_v1v2report_process(mgmd_cb_t *mgmdCB, 
                                   L7_inet_addr_t group, 
                                   L7_inet_addr_t source);

/*****************************************************************
* @purpose Handles  the physical interface up and down 
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    rtrIfNum          @b{ (input) }   Router Interface Number
*
* @returns  None
*
* @notes    
* @end
*********************************************************************/
void mgmd_proxy_intf_down_chk(mgmd_cb_t *mgmdCB, L7_uint32 intIfNum);

/*****************************************************************
* @purpose Debug routine 
*
* @param   family            @b{ (input) }   Family
*
* @returns None
*
* @notes 
* @end
*********************************************************************/
void mgmd_proxy_groups_show(L7_uint32 family);

/*****************************************************************
* @purpose  Debug show routine
*
* @param    family            @b{ (input) }   Family
*
* @returns  None
*
* @notes    None
* @end
*********************************************************************/
void mgmd_proxy_info_show(L7_uint32 family);

/*****************************************************************
* @purpose  Debug routine. It displays the Proxy Membership Table.
*
* @param    family    @b { (input) }    Family
*
* @returns  None
*
* @notes  
*
* @end
*********************************************************************/
void mgmd_proxy_sat_dump(L7_uint32 family);

/*****************************************************************
* @purpose To update MFC entries
*
* @param    mgmdCB       @b{ (input) }   MGMD Control Block
* @param    mfcMsg       @b{ (input) }   MFC Message
*
* @returns  None
*
* @notes  it update the MFC cache table
*
* @end
*********************************************************************/
L7_RC_t mgmd_proxy_call_fn_update (mgmd_cb_t *mgmdCB, mfcEntry_t* mfcMsg);

/********************************************************************
* @purpose  Handles the v1 querier timer expiry and puts the running 
*           version into 3 if and only if v2 querier time is zero 
*
* @param    param     @b { (input) }  MGMD Control Block
*
* @returns  None
*
* @notes
* @end
*********************************************************************/
void mgmd_proxy_v1querier_timeout_event_handler(void *mgmdCB);

/*****************************************************************
* @purpose  Handles v2 querier timer expiry & puts the running version into 3 
*
* @param    param     @b { (input) }  MGMD Control Block
*
* @returns  None
*
* @notes
* @end
*********************************************************************/
void mgmd_proxy_v2querier_timeout_event_handler(void *mgmdCB);

/********************************************************************
* @purpose  Handles all the interface timer expiry for all general queries 
*
* @param    param     @b { (input) }  MGMD Control Block
*
* @returns  None
*
* @notes
* @end
*********************************************************************/
void mgmd_proxy_interface_timer_expiry_handler(void *mgmdCB);

/*****************************************************************
* @purpose  Handles all the group timer expiry events for all
*           group and grp src sprcific queries
*
* @param    param     @b { (input) }  MGMD Host Group
*
* @returns  None
*
* @notes
* @end
*********************************************************************/
void mgmd_proxy_group_response_timer_expiry_handler(void *mgmd_host_group);

/*****************************************************************
* @purpose Handles the unsolicited report timer interval expiry
*
* @param    param     @b { (input) }  Timer Event Info
*
* @returns  None
*
* @notes it sends the group report or group src specific report 
* @end
*********************************************************************/
void mgmd_proxy_unsolicited_timer_expiry_handler(void *param);

/*****************************************************************
* @purpose  Handles Querier Update event from IGMP/MLD. It Updates the 
*           Multi cast forwarding cache. 
*
* @param   mgmdCB       @b { (input) }    MGMD Control Block
* @param   intIfNum     @b { (input) }    Interface Number
* @param   querierFlag  @b { (input) }    Querier Flag
*
* @returns  None
*
* @notes    it update the MFC cache table. Called when an MGMD Interface changes
*           between the following states Querier, Non-Querier states.  
*
* @end
*********************************************************************/
void mgmd_proxy_querier_update_event_send(mgmd_cb_t *mgmdCB, L7_int32 intIfNum, L7_BOOL querierFlag);
/*********************************************************************
*
* @purpose  To Delete the  Timer
*
* @param    mgmdCB  @b{ (input) } MGMD Control Block
*                 srcRec     Source record for which timer needs to be started
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t  mgmdProxyUtilAppTimerHandleDelete(mgmd_cb_t *mgmdCB,L7_APP_TMR_HNDL_t *timer, L7_uint32 *handle);

/*********************************************************************
*
* @purpose  Handles the group mode change
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    rtrIntf           @b{ (input) }   Router Interface Number
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    numSrcs   @b{ (input) }   Number of sources in the list
* @param    sourceList @b{ (input) }   pointer to the source list
* @notes    
*
* @end
*********************************************************************/
void  mgmd_proxy_group_mode_change(mgmd_cb_t *mgmdCB, L7_uint32 rtrIntf, 
                                   mgmd_host_group_t *mgmd_host_group, L7_uint32 numSrcs, 
                                   L7_inet_addr_t *sourceList);


/*****************************************************************
* @purpose  It Updates the Multi cast forwarding cache 
*
* @param    mgmdCB           @b{ (input) }   MGMD Control Block
* @param    srcAddr          @b{ (input) }   Soource Address
* @param    mgmd_host_group  @b{ (input) }   MGMD Host Group
*
* @returns  None
*
* @notes  it update the MFC cache table
*
* @end
*********************************************************************/
void mgmdProxyMRTUpdate(mgmd_cb_t         *mgmdCB, 
                        L7_inet_addr_t    *srcAddr,
                        L7_inet_addr_t    *grpAddr);

/*********************************************************************
*
* @purpose  Initialise the MGMD MRT Tree
*
* @param    MGMDCB @b{ (input) } Pointer PIMDM Control Block
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyCacheTreeInit (mgmd_cb_t *mgmdCb);

/*********************************************************************
*
* @purpose  Deinitialise the MRT Tree
*
* @param    MGMDCB @b{ (input) } Pointer PIMDM Control Block
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyCacheTreeDeinit (mgmd_cb_t *mgmdCb);

/*********************************************************************
*
* @purpose  Search the MRT tree to get the  entry 
*
* @param    MGMDCB @b{ (input) } Pointer PIMDM Control Block
*           srcAddr @b{ (input) } Pointer Source Address
*           grpAddr @b{ (input) } Pointer Group Address
*                            
* @returns  Pointer to the MRT Entry if found else return NULL.
*
* @comments None
*
* @end
*********************************************************************/
mgmdProxyCacheEntry_t* mgmdProxyMRTCacheEntryGet (mgmd_cb_t *mgmdCb,
                                                  L7_inet_addr_t *srcAddr,
                                                  L7_inet_addr_t *grpAddr);

/*********************************************************************
*
* @purpose  Search the MRT tree to get the  next entry 
*
* @param    MGMDCB @b{ (input) } Pointer PIMDM Control Block
*           srcAddr @b{ (input) } Pointer Source Address
*           grpAddr @b{ (input) } Pointer Group Address
*                            
* @returns  Pointer to the MRT Entry if found else return NULL.
*
* @comments None
*
* @end
*********************************************************************/
mgmdProxyCacheEntry_t* mgmdProxyMRTCacheEntryNextGet (mgmd_cb_t *mgmdCb,
                                                      L7_inet_addr_t *srcAddr,
                                                      L7_inet_addr_t *grpAddr);

/*****************************************************************
* @purpose This function gets the source record from the source list 
*
* @param   srcRecList    @b{ (input) }   Source Record List to be searched
* @param   source        @b{ (input) }   Source Address to be searched
* @param   srcRec        @b{ (output) }  Host Source Record (if found)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  None
*
* @end
*********************************************************************/
L7_RC_t mgmd_proxy_src_rec_get (L7_sll_t *srcRecList, L7_inet_addr_t source, 
                                mgmd_host_source_record_t **srcRec);

/*****************************************************************
* @purpose The function searches for the proxy group and returns
*
* @param   mgmdCB           @b{ (input) }   MGMD Control Block
* @param   group            @b{ (input) }   Group Address
* @param   mgmd_host_group  @b{ (input) }   MGMD Host Group
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes 
* @end
*********************************************************************/
L7_RC_t mgmd_proxy_host_group_get(mgmd_cb_t *mgmdCB, L7_inet_addr_t group, 
                                  mgmd_host_group_t **mgmd_host_group);

/*********************************************************************
*
* @purpose  Handler for MFC Component Events
*
* @param    mgmdCB       @b{ (input) }   MGMD Control Block
*           mfcEventType  @b{ (input) } MFC Event ID
*           mfcInfo       @b{ (input) } MFC Event Info 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyMFCEventHandler (mgmd_cb_t *mgmdCB,
                                  L7_uint32 mfcEventType,
                                  mfcEntry_t *mfcInfo);

/*********************************************************************
*
* @purpose  Handler for Admin Scope Boundary Events
*
* @param    mgmdCB       @b{ (input) }   MGMD Control Block
*           asbEventType @b{ (input) } Admin Scope Event ID
*           asbInfo      @b{ (input) } Admin Scope Boundary Event Info 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyAdminScopeBoundaryEventHandler (mgmd_cb_t *mgmdCB,
                                                 L7_uint32 asbEventType,
                                                 mcastAdminMsgInfo_t *asbInfo);
/*********************************************************************
*
* @purpose  Handler for Querier state change Events
*
* @param    mgmdCB       @b{ (input) } MGMD Control Block
*           rtrIfNum     @b{ (input) } Interface on which querier state changed
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t mgmdProxyQuerierStateChangeEventHandler (mgmd_cb_t *mgmdCB);

/*********************************************************************
*
* @purpose  Updates MRT table when group record in proxy database is deleted
*
* @param    mgmdCB     @b{ (input) } MGMD Control Block
*           group      @b{ (input) } group address
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void mgmdProxyMRTGroupRecordDeleteHandler(mgmd_cb_t *mgmdCB, L7_inet_addr_t group);

/*********************************************************************
*
* @purpose  Updates MRT table when group record in proxy database is deleted
*
* @param    mgmdCB     @b{ (input) } MGMD Control Block
* @param    rtIfNum    @b{ (input) } router interface number
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void mgmdProxyMRTIntfDownHandler(mgmd_cb_t *mgmdCB, L7_uint32 rtrIfNum);

/*********************************************************************
*
* @purpose  Purges proxy MRT table 
*
* @param    mgmdCB     @b{ (input) } MGMD Control Block
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void mgmdProxyMRTPurge(mgmd_cb_t *mgmdCB);

#endif
