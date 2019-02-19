/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  mgmd_proxy.c
*
* @purpose   To support IGMP-Proxy and MLD-Proxy
*
* @component MGMD (IGMP+MLD)
*
* @comments  none
*
* @create    03/10/2005
*
* @author   ryadagiri
* @end
*
**********************************************************************/

#include "l7_mgmdmap_include.h"
#include "mcast_defs.h"
#include "mgmd.h"
#include "mgmd_v6.h"
#include "mgmd_proxy.h"
#include "mgmd_proxy_debug.h"

/**********************************************************************
                  Globals
**********************************************************************/



/* MGMD Proxy Local/static prototypes */

static void mgmd_proxy_aggregation_table_init (mgmd_cb_t *mgmdCB);

static L7_RC_t mgmd_proxy_source_record_destroy(L7_sll_member_t *srcRec);

static L7_int32 mgmd_proxy_source_record_compare(void *p, void *q, L7_uint32 key);

static L7_RC_t mgmd_proxy_group_create(mgmd_cb_t *mgmdCB, L7_inet_addr_t group, 
                                       mgmd_host_group_t **mgmd_new_host_group);

static L7_RC_t mgmd_proxy_initial_state_build (mgmd_cb_t     *mgmdCB, 
                                               L7_uint32      rtrIfNum, 
                                               mgmd_group_t  *mgmd_group);

static L7_RC_t mgmd_proxy_include_include_process(mgmd_cb_t *mgmdCB, 
                                                  mgmd_group_t *mgmd_group,
                                                  mgmd_host_group_t *mgmd_host_group,
                                                  L7_uint32  ifnum);

static L7_RC_t mgmd_proxy_include_exclude_process(mgmd_cb_t *mgmdCB, 
                                                  mgmd_group_t *mgmd_group,
                                                  mgmd_host_group_t *mgmd_host_group,
                                                  L7_uint32  ifnum);

static L7_RC_t mgmd_proxy_exclude_include_process(mgmd_cb_t *mgmdCB, 
                                                  mgmd_group_t *mgmd_group,
                                                  mgmd_host_group_t *mgmd_host_group,
                                                  L7_uint32  ifnum);

static L7_RC_t mgmd_proxy_exclude_exclude_process(mgmd_cb_t *mgmdCB,
                                                  mgmd_group_t *mgmd_group,
                                                  mgmd_host_group_t *mgmd_host_group,
                                                  L7_uint32  ifnum);

static L7_RC_t mgmd_proxy_util_new_src_record_create (mgmd_cb_t *mgmdCB, L7_inet_addr_t sourceAddress, 
                                                      mgmd_host_source_record_t **hostSrcRec);

static L7_BOOL mgmd_proxy_src_rec_timeout(mgmd_host_source_record_t *hostSrcRec);

static void mgmd_proxy_IncludeAdd (mgmd_cb_t *mgmdCB, L7_uint32 ifNum, 
                                   mgmd_host_group_t *mgmd_host_group,
                                   L7_uint32 srcsCount, L7_inet_addr_t * srcList);

static void mgmd_proxy_IncludeDelete (mgmd_cb_t *mgmdCB, L7_uint32 ifNum, 
                                      mgmd_host_group_t *mgmd_host_group,
                                      L7_uint32 srcsCount, L7_inet_addr_t * srcList);

void mgmd_proxy_ExcludeAdd (mgmd_cb_t *mgmdCB, L7_uint32 ifNum, 
                            mgmd_host_group_t *mgmd_host_group,
                            L7_uint32 srcsCount, L7_inet_addr_t * srcList);

static void mgmd_proxy_ExcludeDelete (mgmd_cb_t *mgmdCB, L7_uint32 ifNum, 
                                      mgmd_host_group_t *mgmd_host_group,
                                      L7_uint32 srcsCount, L7_inet_addr_t *srcList);

void mgmd_proxy_IncludeToExclude (mgmd_cb_t *mgmdCB, L7_uint32 ifNum,
                                  mgmd_host_group_t *mgmd_host_group,
                                  L7_uint32 srcsCount1, L7_inet_addr_t *srcList1,
                                  L7_uint32 srcsCount2, L7_inet_addr_t *srcList2);

static void mgmd_proxy_group_record_delete (mgmd_cb_t *mgmdCB, L7_uint32 ifNum,
                                            mgmd_host_group_t *mgmd_host_group,
                                            L7_uint32 numSrcs, L7_inet_addr_t *srcAddrList);

static void mgmd_proxy_src_tmrs_grp_rec_update (mgmd_host_group_t *mgmd_host_group,
                                                L7_uint32          numSrcs, 
                                                L7_inet_addr_t    *sourceList,
                                                L7_uint32          timeout); 


static void mgmd_proxy_EXCLUDE_XplusB_YminusB (mgmd_cb_t *mgmdCB,
                                               mgmd_host_group_t *mgmd_host_group,
                                               L7_uint32 numSrcs,
                                               L7_inet_addr_t *sourceList);


void mgmd_proxy_EXCLUDE_AxB_BminusA (mgmd_cb_t *mgmdCB, 
                                     mgmd_host_group_t *mgmd_host_group,
                                     L7_uint32 numSrcs, 
                                     L7_inet_addr_t *sourceList);
static void mgmd_proxy_EXCLUDE_XplusBminusY_Y(mgmd_cb_t *mgmdCB, 
                                              mgmd_host_group_t *mgmd_host_group,
                                              L7_uint32 numSrcs, 
                                              L7_inet_addr_t *sourceList,
                                              L7_uint32 rtrIfNum);

void mgmd_proxy_EXCLUDE_BminusY_YxB (mgmd_cb_t *mgmdCB,
                                     mgmd_host_group_t *mgmd_host_group,
                                     L7_uint32 numSrcs, 
                                     L7_inet_addr_t *sourceList);

static void mgmd_proxy_EXCLUDE_BminusYplusYminusB_YxB (mgmd_cb_t *mgmdCB,
                                                       mgmd_host_group_t *mgmd_host_group,
                                                       L7_uint32  numSrcs, 
                                                       L7_inet_addr_t *sourceList);


static L7_RC_t mgmd_proxy_grp_report_send(mgmd_cb_t *mgmdCB,L7_inet_addr_t group , 
                                           L7_ushort16 numSrcs, L7_inet_addr_t **sourceSet, 
                                           L7_uchar8 grpRecType,mgmd_host_group_t *mgmd_host_group);

static L7_RC_t mgmd_proxy_report_send(mgmd_cb_t *mgmdCB);

static void mgmd_proxy_group_delete (mgmd_cb_t *mgmdCB,mgmd_host_group_t *mgmd_host_group);

static void mgmd_proxy_grp_rec_delete (mgmd_cb_t *mgmdCB, 
                                       mgmd_host_group_t *mgmd_host_group);

static void mgmd_proxy_src_rec_add (mgmd_cb_t *mgmdCB, 
                                    mgmd_host_group_t *mgmd_host_group, 
                                    mgmd_host_source_record_t *srcRec);

static void mgmd_proxy_src_rec_delete (mgmd_cb_t *mgmdCB,
                                       mgmd_host_group_t *mgmd_host_group, 
                                       mgmd_host_source_record_t *srcRec);

static void mgmd_proxy_SPL_portno_delete (mgmd_cb_t *mgmdCB,
                                          mgmd_host_group_t *mgmd_host_group,
                                          mgmd_host_source_record_t *srcRec, 
                                          L7_uint32 ifNum);

static void mgmd_proxy_SPL_portno_add (mgmd_cb_t *mgmdCB, 
                                       mgmd_host_group_t *mgmd_host_group,
                                       mgmd_host_source_record_t *srcRec, 
                                       L7_uint32 ifNum);

static void mgmd_proxy_GPL_portno_delete (mgmd_cb_t *mgmdCB,
                                          mgmd_host_group_t *mgmd_host_group, 
                                          L7_uint32 ifNum);

static void mgmd_proxy_GPL_portno_add (mgmd_cb_t *mgmdCB,
                                       mgmd_host_group_t *mgmd_host_group, 
                                       L7_uint32 ifNum);

static void mgmd_proxy_GPL_to_SPL_copy(mgmd_cb_t *mgmdCB,
                                       mgmd_host_group_t  *mgmd_host_group,
                                       mgmd_host_source_record_t *srcRec);

static L7_BOOL mgmd_proxy_portlist_status(interface_bitset_t *bitset);

static void mgmd_proxy_allow_send(mgmd_cb_t *mgmdCB, 
                                  mgmd_host_group_t         *mgmd_host_group,
                                  mgmd_host_source_record_t *srcRec);

static void mgmd_proxy_block_send(mgmd_cb_t *mgmdCB, 
                                  mgmd_host_group_t         *mgmd_host_group,
                                  mgmd_host_source_record_t *srcRec);

static void mgmd_proxy_to_incl_send(mgmd_cb_t *mgmdCB, 
                                    mgmd_host_group_t *mgmd_host_group);

static void mgmd_proxy_to_excl_send(mgmd_cb_t *mgmdCB, 
                                    mgmd_host_group_t *mgmd_host_group);

static void mgmd_proxy_unsolicited_timer_schedule (mgmd_cb_t *mgmdCB, 
                                                   L7_inet_addr_t  group,
                                                   L7_uint32          numSrcs,
                                                   L7_inet_addr_t   **RptSrcList,
                                                   L7_uint32          grpRecType);

static L7_BOOL mgmd_proxy_bitset_compare(interface_bitset_t *mask1, 
                                         interface_bitset_t *mask2);

static void mgmd_proxy_bitset_copy(interface_bitset_t *dest, 
                                   interface_bitset_t *src);

static L7_uint32 mgmd_proxy_get_bitset_count(interface_bitset_t *interface_mask);

static L7_int32 mgmd_proxy_selected_delay_calculate(L7_int32 max_resp_time);

static void mgmd_proxy_ExcludeAdd1(mgmd_cb_t *mgmdCB, L7_uint32 ifNum, 
                                   mgmd_host_group_t *mgmd_host_group,
                                   L7_uint32 srcsCount, L7_inet_addr_t * srcList);

static void mgmd_proxy_ExcludeAdd2 (mgmd_cb_t *mgmdCB, L7_uint32 ifNum,
                                    mgmd_host_group_t *mgmd_host_group,
                                    L7_uint32 YminusBsrcsCount, 
                                    L7_inet_addr_t *YminusBsrcList,
                                    L7_uint32 BminusYsrcsCount, 
                                    L7_inet_addr_t *BminusYsrcList);

static void mgmd_proxy_includeToExclude_new (mgmd_cb_t *mgmdCB, L7_uint32 ifNum,
                                             mgmd_host_group_t *mgmd_host_group,
                                             L7_uint32 srcCount, 
                                             L7_inet_addr_t *srcList);

static void mgmd_proxy_src_timer_expire_handler(mgmd_cb_t *mgmdCB, 
                                                L7_uint32 rtrIfNum,
                                                L7_inet_addr_t group, 
                                                L7_uint32 numSrcs, 
                                                L7_inet_addr_t *sourceList);

static void mgmd_proxy_ExcludeSrcDelete (mgmd_cb_t *mgmdCB, L7_uint32 rtIfNum, 
                                      mgmd_host_group_t *mgmd_host_group,
                                      L7_uint32 srcsCount, L7_inet_addr_t *srcList);

static L7_RC_t mgmdProxyUtilAppTimerSet (mgmd_cb_t *mgmdCB,
                      L7_app_tmr_fn pFunc,
                      void *pParam,
                      L7_APP_TMR_HNDL_t *tmrHandle,
                      L7_uint32 timeOut,
                      MGMD_TIMER_t timerType);
static 
L7_RC_t mgmdProxyUtilAppTimerLower (mgmd_cb_t *mgmdCB,
                      L7_app_tmr_fn pFunc,
                      void *pParam,
                      L7_APP_TMR_HNDL_t *tmrHandle,
                      L7_uint32 lowerTimeOut,
                      MGMD_TIMER_t timerType);
static L7_RC_t  mgmdProxyUtilAppTimerDelete(mgmd_cb_t *mgmdCB,
                                            L7_APP_TMR_HNDL_t *timer);
static L7_RC_t mgmd_proxy_interface_version_set(mgmd_cb_t *mgmdCB,
                                         L7_uint32 rtrIfNum, L7_uint32 version);

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
L7_RC_t mgmd_proxy_interface (mgmd_cb_t *mgmdCB, L7_uint32 rtrIntf, L7_uint32 mode,
                              mgmdIntfConfig_t *mgmdIntfInfo)           
{
  mgmd_host_info_t    *mgmd_host_info;
  L7_RC_t             rc = L7_FAILURE;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  if (mgmdCB == L7_NULL)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Control Block pointer is NULL.......\n");
    return(L7_FAILURE);
  }

  if (mgmdCB->mgmd_host_info == L7_NULL)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Memory Not allocated for Aggregation table.\n");
    return(L7_FAILURE);         
  }

  if (mode == L7_ENABLE)
  {
    mgmd_host_info = mgmdCB->mgmd_host_info;        
    mgmdCB->mgmd_proxy_status = L7_ENABLE;
    mgmdCB->mgmd_proxy_restartCount++;
    mgmd_host_info->rtrIntf = rtrIntf;
    mgmd_host_info->hostCompatibilityMode = mgmdIntfInfo->version;
    mgmd_host_info->configVersion = mgmdIntfInfo->version;
    mgmd_host_info->robustness = mgmdIntfInfo->robustness;
    mgmd_host_info->unsolicitedReportInterval = mgmdIntfInfo->unsolicitedReportInterval;

    /* the querier of this Proxy host is set to zero untill the querier 
       is learnt fron the network. */
    inetAddressZeroSet(mgmdCB->proto, &mgmdCB->mgmd_host_info->querierIP);
    /* Set mcast data reception for all the routing interfaces */
    if (mgmdCB->proto == L7_AF_INET6)
    {
      mgmdProxyV6InterfacesMcastModeSet(mode);
    }

    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Started general Response timer ");
    if (mgmdProxyUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmdCB, 
                                 &mgmd_host_info->gen_rsp_timer, 
                                 IGMP_PROXY_INTERFACE_TIMER,
                                 L7_MGMD_GENERAL_RSP_TIMER) != L7_SUCCESS)
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not start the gen_rsp_timer .\n");     
      return L7_FAILURE;
    }

    if (mgmdCB->proto == L7_AF_INET)
    {
      mcastMapMRPAdminScopeInfoGet(mgmdCB->proto, rtrIntf);
    }

    mgmd_proxy_aggregation_table_init(mgmdCB);
    rc = mgmd_proxy_report_send(mgmdCB);
    return rc;
  }
  else
  { /* (mode == L7_DISABLE) */
    if (mgmdCB->mgmd_proxy_ifIndex == rtrIntf)
    {
      mgmd_host_info = mgmdCB->mgmd_host_info;
    }
    else
    {
      return L7_FAILURE;
    }

    /* Delete the timers */
    if (mgmdProxyUtilAppTimerDelete(mgmdCB, &mgmd_host_info->ver2_querier_timer) != L7_SUCCESS)
    {
       MGMD_PROXY_DEBUG(MGMD_DEBUG_FAILURE,"Could not delete the ver2_querier_timer.\n");          
    }

    if (mgmdProxyUtilAppTimerDelete(mgmdCB, &mgmd_host_info->ver1_querier_timer) != L7_SUCCESS)
    {
       MGMD_PROXY_DEBUG(MGMD_DEBUG_FAILURE,"Could not delete the ver1_querier_timer.\n");          
    }
    if (mgmdProxyUtilAppTimerDelete(mgmdCB, &mgmd_host_info->gen_rsp_timer) != L7_SUCCESS)
    {
       MGMD_PROXY_DEBUG(MGMD_DEBUG_FAILURE,"Could not delete the general response timer.\n");          
    }

    mgmd_proxy_intf_down_chk(mgmdCB, rtrIntf);

    /* Purge the SLL for unsolicited_reports */
    if (SLLPurge(L7_FLEX_MGMD_MAP_COMPONENT_ID, &(mgmd_host_info->ll_unsolicited_reports)) != L7_SUCCESS)
    {
        MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "Unsolicited Report SLL not Purged.\n");
    }

    mgmdCB->mgmd_proxy_ifIndex = 0;
    mgmdCB->mgmd_proxy_status = L7_DISABLE;     
    inetAddressZeroSet(mgmdCB->proto, &mgmdCB->mgmd_host_info->querierIP);
    mgmd_host_info->rtrIntf = 0;
    mgmd_host_info->hostCompatibilityMode = L7_MGMD_VERSION_3;

    if (mgmdCB->proto == L7_AF_INET6)
    {
      mgmdProxyV6InterfacesMcastModeSet(mode);
    }
    return L7_SUCCESS;
  }

  return(L7_FAILURE);
}


/*****************************************************************
* @purpose Initializes the aggregation table 
*
* @param    mgmdCB    @b{ (input) }   MGMD Control Block
*
* @returns  NONE
*
* @notes  It builds the aggregation table according to the availability of 
*         the members in each router membership table 
*
* @end
*********************************************************************/
static void mgmd_proxy_aggregation_table_init( mgmd_cb_t *mgmdCB)
{

  mgmd_group_t *mgmd_group = L7_NULL;
  mgmd_group_t  dummyGroup;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n Entered ");

  memset(&dummyGroup, 0, sizeof(mgmd_group_t));

/* Get first entry in the tree */
  mgmd_group = (mgmd_group_t *)avlSearchLVL7(&(mgmdCB->membership), (void *)(&dummyGroup), AVL_NEXT);
  while (mgmd_group)
  {
    if (mgmd_proxy_initial_state_build(mgmdCB, mgmd_group->rtrIfNum, mgmd_group) != L7_SUCCESS)
    {
       MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Initial State building Error.\n");
      return;
    }

    /* Get next entry in the tree */
    mgmd_group = (mgmd_group_t *)avlSearchLVL7(&(mgmdCB->membership), (void *)(mgmd_group), AVL_NEXT);
  }
}


/*****************************************************************
* @purpose  Creates a New Group-Record
*
* @param    mgmdCB                @b{ (input) }   MGMD Control Block
* @param    group                 @b{ (input) }   Group Address
* @param    mgmd_new_host_group   @b{ (output) }  New MGMD Host Group
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  
* @end
*********************************************************************/
static L7_RC_t mgmd_proxy_group_create(mgmd_cb_t *mgmdCB, L7_inet_addr_t group, 
                                       mgmd_host_group_t **mgmd_new_host_group)
{
  mgmd_host_group_t          mgmd_host_group;
  mgmd_host_info_t          *mgmd_host_info;
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  mgmd_host_info = mgmdCB->mgmd_host_info;
  /*inetCopy(&mgmd_host_group.group, &group);*/

  /*inetCopy(&mgmd_host_group.group, &group);*/
  memset(&mgmd_host_group, 0, sizeof(mgmd_host_group_t));
  mgmd_host_group.group = group;
  mgmd_host_group.filterMode = MGMD_FILTER_MODE_INCLUDE;
  mgmd_host_group.grpSuppressed = L7_FALSE;
  mgmd_host_group.mgmdCB = mgmdCB;

  /* Create linked list of source records for this group */
  if (SLLCreate(L7_FLEX_MGMD_MAP_COMPONENT_ID, L7_SLL_ASCEND_ORDER, L7_NULL, mgmd_proxy_source_record_compare,
                mgmd_proxy_source_record_destroy, &(mgmd_host_group.sourceRecords)) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "mgmd_proxy_group_create: SLL creation Error\n");
    return L7_FAILURE;
  }

  inetInAddressAnyInit(mgmdCB->proto, &(mgmd_host_group.last_reporter));

  mgmd_host_group.uptime = osapiUpTimeRaw();
  mgmd_host_group.hostState = MGMD_HOST_DELAY_MEMBER;

  if (osapiSemaTake(mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Could not take the Semaphore.\n");
    return L7_FAILURE;
  }
  /* add it to the host info membership table */
  if (avlInsertEntry(&(mgmd_host_info->hostMembership), &mgmd_host_group))
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "mgmd_proxy_group_create: Error inserting into Membership Table\n");

    mgmd_proxy_group_delete(mgmdCB, &mgmd_host_group);
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }

  if (osapiSemaGive(mgmd_host_info->hostMembership.semId) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Could not give the Semaphore.\n");
    return L7_FAILURE;
  }


  mgmd_host_info->numOfGroups++;

  memset(&mgmd_host_group.grpPortList, 0, sizeof(interface_bitset_t));

  if ((*mgmd_new_host_group = (mgmd_host_group_t *)avlSearchLVL7(&(mgmd_host_info->hostMembership), 
                                                                 &mgmd_host_group, AVL_EXACT)) == L7_NULL)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "mgmd_proxy_group_create: Error creating new HOST group\n");
    mgmd_proxy_group_delete(mgmdCB, &mgmd_host_group);
    return L7_FAILURE;
  }

  if (((*mgmd_new_host_group)->grp_rsp_timer_handle =
       handleListNodeStore(mgmdCB->handle_list, *mgmd_new_host_group)) == L7_NULL)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not store the handle node to store the timer data.\n");                    
    mgmd_proxy_group_delete(mgmdCB, *mgmd_new_host_group);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/*****************************************************************
* @purpose  To build the initial aggregation table  
*
* @param    mgmdCB       @b{ (input) }   MGMD Control Block
* @param    rtrIfNum     @b{ (input) }   Router Interface Number
* @param    mgmd_group   @b{ (output) }  MGMD Group
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   it call each individual function depending upon the 
*          mgmd_group compatiability mode
*
* @end
*********************************************************************/
static L7_RC_t mgmd_proxy_initial_state_build(mgmd_cb_t *mgmdCB, L7_uint32 rtrIfNum, 
                                              mgmd_group_t *mgmd_group)
{
  mgmd_host_group_t *mgmd_host_group;
  mgmd_host_info_t  *mgmd_host_info;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n Entered ");

  mgmd_host_info = mgmdCB->mgmd_host_info;

  mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7(&(mgmd_host_info->hostMembership),
                                                       &(mgmd_group->group), AVL_EXACT);

  if (mgmd_host_group == L7_NULL)
  {
    /* Proxy Group info not found - so create it */
    mgmd_proxy_group_create(mgmdCB, mgmd_group->group, &mgmd_host_group);

    if (!mgmd_host_group)
    {
       MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "mgmd_proxy_initial_state_build: Error getting memory for MGMD \
              host group record\n");
      return L7_FAILURE;
    }
  }

  if (mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
  {
    if (mgmd_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
      mgmd_proxy_include_include_process(mgmdCB, mgmd_group, mgmd_host_group,
                                         rtrIfNum);
    else
      mgmd_proxy_include_exclude_process(mgmdCB, mgmd_group, mgmd_host_group,
                                         rtrIfNum);
  }
  else if (mgmd_host_group->filterMode == MGMD_FILTER_MODE_EXCLUDE)
  {
    if (mgmd_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
      mgmd_proxy_exclude_include_process(mgmdCB, mgmd_group, mgmd_host_group,
                                         rtrIfNum);
    else
      mgmd_proxy_exclude_exclude_process(mgmdCB, mgmd_group, mgmd_host_group,
                                         rtrIfNum);
  }

  return L7_SUCCESS;
}


/*****************************************************************
* @purpose  it adds the entry into the mgmd_host_info membership table 
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    mgmd_group        @b{ (input) }   MGMD Group
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    rtrIfNum          @b{ (input) }   Router Interface Number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  
*          PROXY STATE INCLUDE(A)   ROUTER ITERFACE IS_IN(B),
*          NEW STATE INCLUDE(A+B)
*
* @end
**********************************************************************/
static L7_RC_t mgmd_proxy_include_include_process(mgmd_cb_t *mgmdCB, mgmd_group_t * mgmd_group,
                                                  mgmd_host_group_t * mgmd_host_group,
                                                  L7_uint32  rtrIfNum)
{
  mgmd_source_record_t      *srcRec = L7_NULL;
  L7_inet_addr_t            *srcList = mgmdCB->mgmd_host_info->reportSrcList1;
  L7_uint32                  srcsCount = 0;
  L7_RC_t                    retVal = L7_FAILURE;

  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Entry");

  for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL; 
       srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec))
  {
    srcList[srcsCount] = srcRec->sourceAddress;
    srcsCount++;                  
  }/*End-of-for*/

  retVal = mgmd_proxy_routerReportEvent (mgmdCB, MGMD_FILTER_MODE_INCLUDE, rtrIfNum,
                                         mgmd_group->group, srcsCount, srcList);

  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Exit");
  return retVal;
}


/*****************************************************************
* @purpose  it adds the entry into the mgmd_host_info membership table
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    mgmd_group        @b{ (input) }   MGMD Group
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    rtrIfNum          @b{ (input) }   Router Interface Number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes
*            PROXY STATE INCLUDE(A) ROUTER ITERFACE EX(B,C) 
*            NEW STATE EXCLUDE A=(C-A)
*
* @end
*********************************************************************/
static L7_RC_t mgmd_proxy_include_exclude_process(mgmd_cb_t * mgmdCB, 
                                                  mgmd_group_t * mgmd_group,
                                                  mgmd_host_group_t * mgmd_host_group,
                                                  L7_uint32  rtrIfNum)
{
  mgmd_source_record_t      *srcRec = L7_NULL;
  L7_inet_addr_t            *srcList = mgmdCB->mgmd_host_info->reportSrcList1;
  L7_uint32                  srcsCount = 0;
  L7_RC_t                    retVal = L7_FAILURE;

  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Entry");

  for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL; 
       srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec))
  {
    srcList[srcsCount] = srcRec->sourceAddress;
    srcsCount++;                  
  }/*End-of-for*/

  retVal = mgmd_proxy_routerReportEvent (mgmdCB, MGMD_FILTER_MODE_EXCLUDE, rtrIfNum,
                                         mgmd_group->group, srcsCount, srcList);

  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Exit");
  return retVal;
}


/*****************************************************************
* @purpose  it adds the entry into the mgmd_host_info membership table
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    mgmd_group        @b{ (input) }   MGMD Group
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    rtrIfNum          @b{ (input) }   Router Interface Number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes
*         PROXY STATE EXCLUDE(A) ROUTER ITERFACE IS_IN(B) 
*              NEW STATE EXCLUDE A = (A-B)
* @end
**********************************************************************/
static L7_RC_t mgmd_proxy_exclude_include_process(mgmd_cb_t *mgmdCB, mgmd_group_t * mgmd_group,
                                                  mgmd_host_group_t * mgmd_host_group,
                                                  L7_uint32  rtrIfNum)
{
  mgmd_source_record_t  *srcRec = L7_NULL;
  L7_inet_addr_t        *srcList = mgmdCB->mgmd_host_info->reportSrcList1;
  L7_uint32              srcsCount = 0;
  L7_RC_t                retVal = L7_FAILURE;

  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Entry");

  for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL; 
       srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec))
  {
    srcList[srcsCount] = srcRec->sourceAddress;
    srcsCount++;                  
  }/*End-of-for*/

  retVal = mgmd_proxy_routerReportEvent (mgmdCB, MGMD_FILTER_MODE_INCLUDE, rtrIfNum,
                                         mgmd_group->group, srcsCount, srcList);

  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Exit");
  return retVal;
}


/*****************************************************************
* @purpose  it adds the entry into the mgmd_host_info membership table
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    mgmd_group        @b{ (input) }   MGMD Group
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    rtrIfNum          @b{ (input) }   Router Interface Number
**
* @returns L7_SUCCESS
* @returns L7_FAILURE
**
* @notes
*           
*           PROXY STATE EXCLUDE(A) ROUTER ITERFACE IS_IN(B) 
*                 NEW STATE EXCLUDE A = (A*B)
* @end
**********************************************************************/
static L7_RC_t mgmd_proxy_exclude_exclude_process(mgmd_cb_t *mgmdCB, mgmd_group_t * mgmd_group,
                                                  mgmd_host_group_t * mgmd_host_group,
                                                  L7_uint32  rtrIfNum)
{
  mgmd_source_record_t      *srcRec = L7_NULL;
  L7_inet_addr_t            *srcList = mgmdCB->mgmd_host_info->reportSrcList1;
  L7_uint32                  srcsCount = 0;
  L7_RC_t                    retVal = L7_FAILURE;

  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Entry");

  for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(mgmd_group->sourceRecords)); srcRec != L7_NULL;
       srcRec = (mgmd_source_record_t *)SLLNextGet(&(mgmd_group->sourceRecords), (L7_sll_member_t *)srcRec))
  {
    srcList[srcsCount] = srcRec->sourceAddress;
    srcsCount++;                  
  }/*End-of-for*/

  retVal = mgmd_proxy_routerReportEvent (mgmdCB, MGMD_FILTER_MODE_EXCLUDE, rtrIfNum,
                                         mgmd_group->group, srcsCount, srcList);

  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Exit");
  return retVal;
}


/*****************************************************************
* @purpose  Creates a New Source-Record
*
* @param    mgmdCB                @b{ (input) }   MGMD Control Block
* @param    sourceAddress         @b{ (input) }   Source Address
* @param    hostSrcRec            @b{ (output) }  New Host Source Record
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  
* @end
*********************************************************************/
static L7_RC_t mgmd_proxy_util_new_src_record_create (mgmd_cb_t *mgmdCB, L7_inet_addr_t sourceAddress, 
                                                      mgmd_host_source_record_t **hostSrcRec)
{
  mgmd_host_info_t          *mgmd_host_info;
  mgmd_host_source_record_t *newHostSrcRec = L7_NULLPTR;
  mgmd_host_info = mgmdCB->mgmd_host_info;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  if ((newHostSrcRec = (mgmd_host_source_record_t *)MGMD_PROXY_ALLOC(mgmdCB->proto,
                                                                     sizeof(mgmd_host_source_record_t)))
      == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "mgmd_proxy_util_new_src_record_create: Error getting memory \
              for source record\n");
    return L7_FAILURE;
  }

  memset(newHostSrcRec, 0 , sizeof(mgmd_host_source_record_t));
  /* Initialize the SourceRecord fields. */
  newHostSrcRec->sourceAddress = sourceAddress;
  newHostSrcRec->mgmdCB = mgmdCB;
  newHostSrcRec->sourceCtime = osapiUpTimeRaw();
  newHostSrcRec->sourceInterval = IGMP_PROXY_MALI;
  newHostSrcRec->sendQueryReport = L7_FALSE;
  newHostSrcRec->createFlag = L7_TRUE;

  *hostSrcRec = newHostSrcRec;
  return L7_SUCCESS;
}


/*****************************************************************
* @purpose  Check if the given Source-Record has timedout or not.
*
* @param   hostSrcRec   @b{ (input) }   pointer to hostSrcRec
* @param
*
* @returns L7_TRUE     if the src record expires src interval
* @returns L7_FALSE
*
* @notes  
* @end
*********************************************************************/
static L7_BOOL mgmd_proxy_src_rec_timeout(mgmd_host_source_record_t *hostSrcRec)
{
  L7_uint32 now;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  if (hostSrcRec)
  {
    now = osapiUpTimeRaw();

    if (now - hostSrcRec->sourceCtime >= hostSrcRec->sourceInterval)
    {
      return L7_TRUE;
    }
  }

  return L7_FALSE;
}


/**********************************************************************
* @purpose  This is the IncludeAdd() that updates the SPL and GPL  
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    ifNum             @b{ (input) }   Interface Number
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    srcsCount         @b{ (input) }   Source Count
* @param    srcList           @b{ (input) }   Source List
*
* @returns  None
*
* @notes  
* @end
*********************************************************************/
static void mgmd_proxy_IncludeAdd (mgmd_cb_t *mgmdCB, L7_uint32 ifNum, 
                                   mgmd_host_group_t *mgmd_host_group,
                                   L7_uint32 srcsCount, L7_inet_addr_t *srcList)
{
  mgmd_host_source_record_t  *srcRec = L7_NULL, srcRecSrchKey;
  L7_uint32                   index;
  mgmd_host_source_record_t  *prev = L7_NULL;
  mgmd_host_info_t           *mgmd_host_info = mgmdCB->mgmd_host_info;  

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered, ifNum = %d ",ifNum);

  /* a. If (SourceList != NULL) ..*/
  if (srcList != L7_NULL)
  {
    for (index=0; index < srcsCount; index++)
    {
      memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
      inetCopy(&srcRecSrchKey.sourceAddress, &srcList[index]);
      srcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)&srcRecSrchKey);

      /* a.i. For all the sources present in SrcList, create the src-records 
       * if not present and copy GPL to SPL. */ 
      if (srcRec == L7_NULL)
      {
        mgmd_proxy_util_new_src_record_create(mgmdCB, srcList[index], &srcRec);
        if (srcRec == L7_NULL)
        {
          return;
        }
        mgmd_proxy_src_rec_add (mgmdCB, mgmd_host_group, srcRec);
        /* ...Copy GPL to SPL. */
        mgmd_proxy_GPL_to_SPL_copy(mgmdCB, mgmd_host_group, srcRec);
      }

      /* a.ii. Add PortNo to the SPL of all source records in SrcList. */
      mgmd_proxy_SPL_portno_add(mgmdCB, mgmd_host_group, srcRec, ifNum);
    }/*End-of-FOR-LOOP*/

  }/*End-of-If(SourceList != NULL).*/


  /* b. If P is found in GPL....*/
  if (BITX_TEST(&mgmd_host_group->grpPortList, ifNum))
  {
    /* b.i. Delete P from GPL. */      
    mgmd_proxy_GPL_portno_delete (mgmdCB, mgmd_host_group, ifNum);


    /* b.ii. Delete P from the SPL of all the source records
             except the sources present in SL. */
    /* For all the sources present in SrcList. */
    for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
        srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)srcRec))
    {
      for (index=0; index < srcsCount; index++)
      {
        if (L7_INET_IS_ADDR_EQUAL(&(srcRec->sourceAddress), &srcList[index]))
        {
          /* The Source-Record is present in SL. */
          break;
        }
      }/*End-of-FOR-LOOP*/

      if (index == srcsCount)
      {
        /* Delete P from SPL of current SrcRec. */
        mgmd_proxy_SPL_portno_delete (mgmdCB, mgmd_host_group, 
                                      srcRec, ifNum);
      }
    }/*End-of-for*/
  }/*End-of-If(BITX_TEST*/

  /* c. If (GPL == NULL), delete all the source records with NULL SPL. */
  if (mgmd_proxy_portlist_status(&mgmd_host_group->grpPortList) == L7_FALSE)
  {
    /* So here (GPL == NULL). */
    /* Delete all the Source Records with NULL SPL. */
    for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
        srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)prev))
    {
      /* Check if the source record is with NULL SPL. */
      if (mgmd_proxy_portlist_status(&srcRec->srcPortList) == L7_FALSE)
      {
        /* Delete the SrcRec. */
        mgmd_proxy_src_rec_delete (mgmdCB, mgmd_host_group, srcRec);
        continue;
      }
      prev = srcRec;
    }/*End-of-for*/


    /* e. If((GPL == NULL) && (src-Rec set == NULL), delete group record(G). */
    if (mgmd_proxy_portlist_status(&mgmd_host_group->grpPortList) == L7_FALSE &&
        mgmd_host_group->sourceRecords.sllNumElements == 0)
    {
      /* source record set == NULL. So delete group record. */
      mgmd_proxy_grp_rec_delete (mgmdCB, mgmd_host_group);
      mgmd_host_info->numOfGroups--;

    }
  }/*End-of-If */
  else
  {  /* d. If (GPL != NULL), delete all source records with (SPL== GPL). */
    prev = L7_NULL;
    for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
        srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)prev))
    {
      /* Check if the source record's SPL == GPL. */
      if (mgmd_proxy_bitset_compare(&mgmd_host_group->grpPortList, 
                                    &srcRec->srcPortList) == L7_TRUE)
      {
        /* Delete the SrcRec. */
        mgmd_proxy_src_rec_delete(mgmdCB, mgmd_host_group, srcRec);
        continue;    
      }
      prev = srcRec;
    }/*End-of-for*/
  }/*End-of-Else-Block*/

}


/**********************************************************************
* @purpose  This is the IncludeDelete() that updates the SPL and GPL  
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    ifNum             @b{ (input) }   Interface Number
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    srcsCount         @b{ (input) }   Source Count
* @param    srcList           @b{ (input) }   Source List
*
* @returns  None
*
* @notes  
* @end
*********************************************************************/
static void mgmd_proxy_IncludeDelete (mgmd_cb_t *mgmdCB, L7_uint32 ifNum, 
                                      mgmd_host_group_t *mgmd_host_group,
                                      L7_uint32 srcsCount, L7_inet_addr_t * srcList)
{
  mgmd_host_source_record_t *srcRec = L7_NULL, srcRecSrchKey;
  L7_uint32                  index;
  mgmd_host_source_record_t *prev = L7_NULL;
  mgmd_host_info_t          *mgmd_host_info = mgmdCB->mgmd_host_info;  

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered, ifNum = %d ",ifNum);

  /*NOTE: Is group record present,.. this check is done in parent function.*/

  /* i. If (SourceList != NULL), for all the sources present in SL, ..*/
  if (srcList != L7_NULL)
  {
    /* 1. Delete P from the SPL of source record if it is present. 
     * 2. Do not create the source record if it is not present. */
    for (index=0; index < srcsCount; index++)
    {
      memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
      inetCopy(&srcRecSrchKey.sourceAddress, &srcList[index]);
      srcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)&srcRecSrchKey);

      /* Delete P from the SPL of source record if it is present. */
      if (srcRec != L7_NULL)
      {
        mgmd_proxy_SPL_portno_delete(mgmdCB, mgmd_host_group,srcRec,ifNum);
      }
    }/*End-of-FOR-LOOP*/
  }/*End-of-If(SourceList != NULL).*/
  else
  {  /* (srcList == NULL) */
    /* ii. If (SL == NULL), delete P from SPL of all the source *
     * records present for group record (G).                    */
    for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
        srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)srcRec))
    {
      /* Delete P from the SPL of the source record. */
      mgmd_proxy_SPL_portno_delete (mgmdCB,mgmd_host_group, srcRec, ifNum);
    }/*End-of-for*/
  }/*End-of-Else-Block*/


  /* iii. If (GPL == NULL), delete all the source records with NULL SPL. */
  if (mgmd_proxy_portlist_status(&mgmd_host_group->grpPortList) == L7_FALSE)
  {
    /* So here (GPL == NULL). */
    /* Delete all the Source Records with NULL SPL. */
    for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
        srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)prev))
    {
      /* Delete all the source records with NULL SPL. */
      if (mgmd_proxy_portlist_status(&srcRec->srcPortList) == L7_FALSE)
      {
        /* Delete the SrcRec. */
        mgmd_proxy_src_rec_delete (mgmdCB, mgmd_host_group, srcRec);
        continue;       
      }
      prev = srcRec;
    }/*End-of-for*/


    /* v. If((GPL == NULL) && (src-Rec set == NULL), delete group record(G). */
    if ((mgmd_proxy_portlist_status(&mgmd_host_group->grpPortList) == L7_FALSE) &&
        (mgmd_host_group->sourceRecords.sllNumElements == 0))
    {
      /* source record set == NULL. So delete group record. */
      mgmd_proxy_grp_rec_delete (mgmdCB, mgmd_host_group); 
      mgmd_host_info->numOfGroups--;
    }
  }/*End-of-If*/
  else
  {  /* iv. If (GPL != NULL), delete all source records with (SPL== GPL). */
    for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
        srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)prev))
    {
      /* Check if (SPL==GPL). */
      if (mgmd_proxy_bitset_compare(&mgmd_host_group->grpPortList, 
                                    &srcRec->srcPortList) == L7_TRUE)
      {
        /* Delete the SrcRec. */
        mgmd_proxy_src_rec_delete (mgmdCB, mgmd_host_group, srcRec);
        continue;
      }
      prev = srcRec;
    }/*End-of-for*/
  }/*End-of-Else-Block*/

}


/**********************************************************************
* @purpose  This is the ExcludeAdd() that updates the SPL and GPL  
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    ifNum             @b{ (input) }   Interface Number
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    srcsCount         @b{ (input) }   Source Count
* @param    srcList           @b{ (input) }   Source List
*
* @returns  None
*
* @notes  
* @end
*********************************************************************/
void mgmd_proxy_ExcludeAdd (mgmd_cb_t *mgmdCB, L7_uint32 ifNum, 
                            mgmd_host_group_t *mgmd_host_group,
                            L7_uint32 srcsCount, L7_inet_addr_t *srcList)
{
  mgmd_host_source_record_t *srcRec = L7_NULL;
  mgmd_host_source_record_t *prev = L7_NULL;
  L7_uint32                  index;
  L7_BOOL                    flag = L7_FALSE;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered, ifNum = %d ",ifNum);

  /* Add PortNo to GPL of group record G. */
  mgmd_proxy_GPL_portno_add (mgmdCB, mgmd_host_group, ifNum);

  if (mgmd_host_group->sourceRecords.sllNumElements == 0)
  {
    return;
  }

  /* For all the sources present in apiSrcList. */
  for (index=0; index < srcsCount; index++)
  {
    flag = L7_FALSE;

    for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
        srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)prev))
    {
      if (L7_INET_IS_ADDR_EQUAL(&(srcRec->sourceAddress), &srcList[index]))
      {
        /* If the source record is present, add the port ifNum to SPL. */
        mgmd_proxy_SPL_portno_add (mgmdCB, mgmd_host_group, srcRec, ifNum);
        flag = L7_TRUE;
      }

      /* Delete all the source records with (SPL == GPL). */
      if (mgmd_proxy_bitset_compare(&srcRec->srcPortList,
                                    &mgmd_host_group->grpPortList))
      {
        /* Delete the SrcRec. */
        mgmd_proxy_src_rec_delete (mgmdCB, mgmd_host_group, srcRec);
        continue;
        flag = L7_TRUE;
      }
      prev = srcRec;
      if (flag == L7_TRUE)
      {
        break;                                 
      }
    }/*End-of-for*/
  }/*End-of-FOR-LOOP*/

}


/**********************************************************************
* @purpose  This is the ExcludeDelete() that updates the SPL and GPL  
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    ifNum             @b{ (input) }   Interface Number
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    srcsCount         @b{ (input) }   Source Count
* @param    srcList           @b{ (input) }   Source List
*
* @returns  None
*
* @notes  
* @end
*********************************************************************/
static void mgmd_proxy_ExcludeDelete (mgmd_cb_t *mgmdCB, L7_uint32 ifNum, 
                                      mgmd_host_group_t *mgmd_host_group,
                                      L7_uint32 srcsCount, L7_inet_addr_t *srcList)
{
  mgmd_host_source_record_t *srcRec = L7_NULL,srcRecSrchKey;
  L7_uint32                  index;
  L7_BOOL                    flag = L7_FALSE;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered, ifNum = %d ",ifNum);

  /* a. If group record is present,..*/
  if (mgmd_host_group == L7_NULL)
  {
    return;
  }

  /* b. If (SourceList != NULL), for all the sources present in SL, ..*/
  if (srcList != L7_NULL)
  {
    for (index=0; index < srcsCount; index++)
    {
      memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
      inetCopy(&srcRecSrchKey.sourceAddress, &srcList[index]);
      srcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)&srcRecSrchKey);

      /* ii. If the source record is not present, create the source  *
       *     record and copy all ports of GPL to SPL except port P.. */
      if (srcRec == L7_NULL)
      {
        mgmd_proxy_util_new_src_record_create(mgmdCB, srcList[index], &srcRec);
        if (srcRec == L7_NULL)
        {
           MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not create the source Node \n");
          return;
        }

        mgmd_proxy_src_rec_add (mgmdCB, mgmd_host_group, srcRec);
      }

      if (srcRec->createFlag == L7_TRUE)
      {
        /* ...Copy GPL to SPL except port P. */
        flag = (BITX_TEST(&srcRec->srcPortList, ifNum)) ? L7_TRUE : L7_FALSE;               
        mgmd_proxy_GPL_to_SPL_copy(mgmdCB, mgmd_host_group,srcRec);
        if (flag == L7_TRUE)
        {
          mgmd_proxy_SPL_portno_add(mgmdCB,mgmd_host_group,
                                    srcRec,ifNum);
        }
        else
        {
          mgmd_proxy_SPL_portno_delete(mgmdCB,mgmd_host_group,
                                       srcRec,ifNum);
        }
      }
      else
      {
        /* i. If the source record is present, delete P from SPL. */
        mgmd_proxy_SPL_portno_delete(mgmdCB, mgmd_host_group,
                                     srcRec, ifNum);
      }

    }/*End-of-FOR-LOOP*/
  }/*End-of-Main-IF-Block*/

  /* c. If (P is not present in GPL), add P to the SPL of all the *
   *    source records present except the source records present  *
   *    for the sources in SL.                                    */
  if (!BITX_TEST(&mgmd_host_group->grpPortList, ifNum))
  {
    for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
        srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)srcRec))
    {
      for (index=0; index < srcsCount; index++)
      {
        if (L7_INET_IS_ADDR_EQUAL(&(srcRec->sourceAddress), &srcList[index]))
        {
          /* Source record is present. */
          break;
        }
      }

      if (index == srcsCount)
      {
        /* Add P to the SPL. */
        MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS,"\n src = ", &srcRec->sourceAddress);
        mgmd_proxy_SPL_portno_add(mgmdCB, mgmd_host_group, srcRec, ifNum);
      }
    } /*End-of-for*/
  }/*End-of-if(!BITX_TEST.*/

  /* d. Add P to GPL of group record G. */
  mgmd_proxy_GPL_portno_add(mgmdCB, mgmd_host_group, ifNum);

  /* e. Delete all source records with (SPL == GPL). */
  /* NOTE: Deleted this step-(e) as found invalid logically. TODO - Review. */

  /* f. If((GPL == NULL) && (src-rec set == NULL), delete group record (G). */
  if ((mgmd_proxy_portlist_status(&mgmd_host_group->grpPortList) == L7_FALSE) &&
      (mgmd_host_group->sourceRecords.sllNumElements == 0))
  {
    /* source record set == NULL. So delete group record. */
    mgmd_proxy_grp_rec_delete (mgmdCB, mgmd_host_group);
    mgmdCB->mgmd_host_info->numOfGroups--;
  }
}


/**********************************************************************
* @purpose  This is the IncludeToExclude() that updates the SPL and GPL  
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    ifNum             @b{ (input) }   Interface Number
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    srcsCount1        @b{ (input) }   Source Count 1
* @param    srcList1          @b{ (input) }   Source List  1
* @param    srcsCount2        @b{ (input) }   Source Count 2
* @param    srcList2          @b{ (input) }   Source List  2
*
* @returns  None
*
* @notes  
* @end
*********************************************************************/
void mgmd_proxy_IncludeToExclude (mgmd_cb_t *mgmdCB, L7_uint32 ifNum,
                                  mgmd_host_group_t *mgmd_host_group,
                                  L7_uint32 srcsCount1, L7_inet_addr_t *srcList1,
                                  L7_uint32 srcsCount2, L7_inet_addr_t *srcList2)
{

  mgmd_host_source_record_t *srcRec = L7_NULL;
  L7_uint32                  index,j;
  L7_inet_addr_t            *apiSrcList = mgmdCB->mgmd_host_info->reportSrcList1;
  L7_uint32                  apiSrcsCount = 0;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered, ifNum = %d ",ifNum);

  if ((srcList1 == L7_NULLPTR) || (srcList2 == L7_NULLPTR))
  {
    return;
  }

  /* a. If ((group record (G) exists) && (SL1 != NULL) && (GPL == NULL)),
        then for all the sources present in SL1, 
           If ((count(SPL) == 1)&&(port ==  P)), delete the source record. */
  if ((srcList1 != L7_NULL) && (srcsCount1 != 0) &&
      (mgmd_proxy_portlist_status(&mgmd_host_group->grpPortList) == L7_FALSE))
  {
    for (index=0; index < srcsCount1; index++)
    {
      mgmd_host_source_record_t srcRecord;
      srcRecord.sourceAddress = srcList1[index];
      if ((srcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords),(L7_sll_member_t *)&srcRecord)) != L7_NULL)
      {
        /* If ((count(SPL) == 1)&&(port ==  P)), delete source record. */
        if ((mgmd_proxy_get_bitset_count(&srcRec->srcPortList) == 1) &&
            (BITX_TEST(&srcRec->srcPortList, ifNum)))
        {
          mgmd_proxy_src_rec_delete (mgmdCB, mgmd_host_group, srcRec);
        }
      }
    }
  }/*End-of-MAin-If-Block*/

  /* b. Call the API - ExcludeDelete (P, G, (SL2-SL1)). */
  /* b.i. Generate (SL2-SL1). */
  for (index=0; index < srcsCount2; index++)
  {
    for (j=0; j < srcsCount1; j++)
    {
      if (L7_INET_IS_ADDR_EQUAL(&(srcList2[index]), &srcList1[j]))
      {
        break;
      }
    }/*End-of-Inner-For-Loop.*/

    if (j==srcsCount1)
    {
      inetCopy(&apiSrcList[apiSrcsCount], &srcList2[index]);
      apiSrcsCount++;
    }
  }/*End-of-Outer-For-Loop.*/

  /* b.ii. Call the API - ExcludeDelete(). */
  mgmd_proxy_ExcludeDelete (mgmdCB, ifNum, mgmd_host_group, 
                            apiSrcsCount, apiSrcList);
}


/**********************************************************************
* @purpose  This is the DeleteGroupRecord() that updates the SPL and GPL  
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    rtrIntf           @b{ (input) }   Router Interface Number
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
*
* @returns  None
*
* @notes  
* @end
*********************************************************************/
static void mgmd_proxy_group_record_delete (mgmd_cb_t *mgmdCB, L7_uint32 rtrIntf, 
                                            mgmd_host_group_t *mgmd_host_group,
                                            L7_uint32 numSrcs, L7_inet_addr_t *srcAddrList)
{

  mgmd_host_source_record_t *srcRec = L7_NULL;
  mgmd_host_source_record_t *prev = L7_NULL;
  L7_uint32 index;
  L7_BOOL   srcPresent = L7_FALSE;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered, rtrIfNum = %d", rtrIntf);

  if (mgmd_host_group == L7_NULL)
  {
    return;
  }

  /* If group record G is present, then ...*/
  /* ii. Delete P from GPL. */
  /* NOTE:This step is done before to send BLOCK(s) state change Report*/
  mgmd_proxy_GPL_portno_delete (mgmdCB, mgmd_host_group, rtrIntf);
  MGMD_PROXY_DEBUG_BITS(MGMD_PROXY_DEBUG_APIS," GPL after deletion =",
                        &(mgmd_host_group->grpPortList));

  /* i. Delete P from the SPL of all the source records present 
        in the group record. */
  for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
      srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)prev))
  {
    if (srcAddrList != L7_NULLPTR)
    {
      for (index = 0; index < numSrcs; index++)
      {
        MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS," Given src = ",&srcAddrList[index]);
        MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS," Existing src = ",&(srcRec->sourceAddress));
        if (L7_INET_IS_ADDR_EQUAL(&(srcRec->sourceAddress), &srcAddrList[index]))
        {
          srcPresent = L7_TRUE;
          break;
        }
      }
    }
    if (srcPresent == L7_TRUE)
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Found src = ");
      prev = srcRec;
      continue;
    }
    /* If the source record is present, delete P from SPL. */
    mgmd_proxy_SPL_portno_delete (mgmdCB, mgmd_host_group, srcRec, rtrIntf);
    MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS," SPL for src =  after deletion ",
                          &srcRec->sourceAddress);
    MGMD_PROXY_DEBUG_BITS(MGMD_PROXY_DEBUG_APIS, " SPL for above src after deletion ", 
                          &srcRec->srcPortList);

    /* If (GPL == NULL) and (SPL of this SrcRec is NUll) then delete this SrcRec. */
    if (mgmd_proxy_portlist_status(&mgmd_host_group->grpPortList) == L7_FALSE &&
        mgmd_proxy_portlist_status(&srcRec->srcPortList) == L7_FALSE)
    {
      /* Delete the SrcRec. */
      mgmd_proxy_src_rec_delete (mgmdCB, mgmd_host_group, srcRec);
      continue;
    }
    prev = srcRec;
  }

  /* iii. If((GPL == NULL) && (source record set == NULL), 
                delete the group record (G). */
  if (mgmd_proxy_portlist_status(&mgmd_host_group->grpPortList) == L7_FALSE &&
      mgmd_host_group->sourceRecords.sllNumElements == 0)
  {
    mgmd_proxy_grp_rec_delete (mgmdCB, mgmd_host_group);
    mgmdCB->mgmd_host_info->numOfGroups--;
  }
}


/*********************************************************************   
* @purpose  It resets the specified SrcTimers of specified group
*           with the given timeout value 
*
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    numSrcs           @b{ (input) }   Source Count
* @param    srcList           @b{ (input) }   Source List
* @param    timeout           @b{ (input) }   Timeout Value
*
* @returns  None
*
* @notes  
* @end
*********************************************************************/
static void mgmd_proxy_src_tmrs_grp_rec_update (mgmd_host_group_t *mgmd_host_group,
                                                L7_uint32          numSrcs, 
                                                L7_inet_addr_t    *sourceList,
                                                L7_uint32          timeout)
{

  mgmd_host_source_record_t *srcRec = L7_NULL;
  L7_ushort16                    index;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  for (index=0; index < numSrcs; index++)
  {
    mgmd_host_source_record_t srcRecord;
    srcRecord.sourceAddress = sourceList[index];
    if ((srcRec = (mgmd_host_source_record_t *) SLLFind(&(mgmd_host_group->sourceRecords),(L7_sll_member_t *)&srcRecord)) != L7_NULL)
    {
      srcRec->sourceCtime = osapiUpTimeRaw();
      srcRec->sourceInterval = timeout;
    }
  }
}

/*****************************************************************
* @purpose This function does the EXCLUDE(X+B,Y-B) 
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    numSrcs           @b{ (input) }   Source Count
* @param    srcList           @b{ (input) }   Source List
*
* @returns None
*
* @notes EXCLUDE(X,Y) -> EXCLUDE(X+B,Y-B); 
* @end
*********************************************************************/
static void mgmd_proxy_EXCLUDE_XplusB_YminusB (mgmd_cb_t *mgmdCB, 
                                               mgmd_host_group_t *mgmd_host_group,
                                               L7_uint32 numSrcs,
                                               L7_inet_addr_t *sourceList)
{

  mgmd_host_source_record_t *srcRec = L7_NULL;
  L7_uint32                  index = 0;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered ");

  /* Proxy State = Excl(X,Y),  Src = (B),  New State = Excl(X+B,Y-B) */

  for (index = 0; index < numSrcs; index++)
  {
    /* Check to see if this source is already in the list */
    mgmd_host_source_record_t srcRecord;
    srcRecord.sourceAddress = sourceList[index];
    if ((srcRec = (mgmd_host_source_record_t *) SLLFind(&(mgmd_host_group->sourceRecords),(L7_sll_member_t *)&srcRecord)) != L7_NULL)
    {
      if (mgmd_proxy_src_rec_timeout(srcRec) == L7_TRUE)
      {
        /* Matching srcRec belongs to Y. So move to X with updated timeout.*/
        srcRec->sourceCtime = osapiUpTimeRaw();
        srcRec->sourceInterval = IGMP_PROXY_MALI;
      }
    }
    else
    {
      mgmd_proxy_util_new_src_record_create (mgmdCB, sourceList[index], &srcRec);
      if (srcRec == L7_NULL)
      {
        /* Return as Memory allocation failed. */
         MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not create the source Node \n");                 
        return;
      }
      mgmd_proxy_src_rec_add (mgmdCB, mgmd_host_group, srcRec);
    }
  }/*End-of-FOR-LOOP*/

}


/*****************************************************************
* @purpose  This function does the EXCLUDE(A*B,B-A)
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    numSrcs           @b{ (input) }   Source Count
* @param    srcList           @b{ (input) }   Source List
*
* @returns None
*
* @notes   INCLUDE(A) -> EXCLUDE(A*B,B-A)
* @end
*********************************************************************/
void mgmd_proxy_EXCLUDE_AxB_BminusA (mgmd_cb_t *mgmdCB, 
                                     mgmd_host_group_t *mgmd_host_group,
                                     L7_uint32 numSrcs, 
                                     L7_inet_addr_t *sourceList)
{

  mgmd_host_source_record_t *hostSrcRec = L7_NULL;
  mgmd_host_source_record_t *newHostSrcRec = L7_NULL;
  L7_uint32                  index = 0;
  L7_sll_t                   temp_src_records = (mgmd_host_group->sourceRecords);

 /*TODO : Re-write the API logic without SLLCreate and SLLDestroy*/
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  /* Proxy State = Incl(A),  Src = (B),  New State = Excl(A*B,B-A) */

  if (SLLCreate(L7_FLEX_MGMD_MAP_COMPONENT_ID, L7_SLL_ASCEND_ORDER, L7_NULL, mgmd_proxy_source_record_compare,
                mgmd_proxy_source_record_destroy, &(mgmd_host_group->sourceRecords)) != L7_SUCCESS)
  {
     MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "SLL creation Error .  \n");
    return;
  }

  for (index = 0; index < numSrcs; index++)
  {
    mgmd_host_source_record_t srcRecord;
    srcRecord.sourceAddress = sourceList[index];
    if ((hostSrcRec =  (mgmd_host_source_record_t *)SLLFind(&(temp_src_records),(L7_sll_member_t *)&srcRecord)) != L7_NULL)
    {
      mgmd_proxy_util_new_src_record_create(mgmdCB, sourceList[index], &newHostSrcRec);
      if (newHostSrcRec == L7_NULL)
      {
        /* Return as Memory allocation failed. */
         MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not create the source Node \n");                 
        if (SLLDestroy(L7_FLEX_MGMD_MAP_COMPONENT_ID, &(temp_src_records)) != L7_SUCCESS)
        {
            MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "SLL not Destroyed.\n");
        }
        return;
      }

      mgmd_proxy_src_rec_add (mgmdCB, mgmd_host_group, newHostSrcRec);
    }
    else
    {
      /*This Src is not in the List, so Add it to List with SrcTimeout=0.*/
      mgmd_proxy_util_new_src_record_create (mgmdCB, sourceList[index], &hostSrcRec);
      if (hostSrcRec == L7_NULL)
      {
        /* Return as Memory allocation failed. */
         MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not create the source Node \n");                 
        if (SLLDestroy(L7_FLEX_MGMD_MAP_COMPONENT_ID, &(temp_src_records)) != L7_SUCCESS)
        {
            MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "SLL not Destroyed.\n");
        }
        return;
      }
      /* Set Source-Timeout == 0. */
      hostSrcRec->sourceInterval = 0;
      hostSrcRec->createFlag = L7_FALSE;
      mgmd_proxy_src_rec_add (mgmdCB, mgmd_host_group, hostSrcRec);
    }
  }/*End-of-FOR-LOOP*/

  if (SLLDestroy(L7_FLEX_MGMD_MAP_COMPONENT_ID, &(temp_src_records)) != L7_SUCCESS)
  {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "SLL not Destroyed.\n");
    return ;
  }
}


/*****************************************************************
* @purpose   This function does the EXCLUDE(X+(B-Y),Y)
*
* @param   igmp_host_group, numSrcs, sourceList
* @param
*
* @returns None
* @returns None
*
* @notes   EXCLUDE(X,Y) + BLOCK(B) -> EXCLUDE(X+(B-Y),Y)
* @end
*********************************************************************/
static void mgmd_proxy_EXCLUDE_XplusBminusY_Y(mgmd_cb_t *mgmdCB, 
                                              mgmd_host_group_t *mgmd_host_group,
                                              L7_uint32 numSrcs, 
                                              L7_inet_addr_t *sourceList,
                                              L7_uint32 rtrIfNum)
{
  mgmd_host_source_record_t *hostSrcRec = L7_NULL;
  mgmd_host_source_record_t *newHostSrcRec = L7_NULL;
  L7_uint32                  index = 0;

  for (index = 0; index < numSrcs; index++)
  {
    mgmd_host_source_record_t srcRecord;
    srcRecord.sourceAddress = sourceList[index];
    if ((hostSrcRec =  (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords),
                                                            (L7_sll_member_t *)&srcRecord)) == L7_NULLPTR)
    {
      mgmd_proxy_util_new_src_record_create(mgmdCB, sourceList[index], &newHostSrcRec);
      if (newHostSrcRec == L7_NULL)
      {
        /* Return as Memory allocation failed. */
         MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not create the source Node \n");    
         return;
      }

      mgmd_proxy_src_rec_add (mgmdCB, mgmd_host_group, newHostSrcRec);
      mgmd_proxy_GPL_to_SPL_copy(mgmdCB, mgmd_host_group, newHostSrcRec);
    }
  }
}



/*****************************************************************
* @purpose This function does the EXCLUDE(B-Y,Y*B)
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    numSrcs           @b{ (input) }   Source Count
* @param    srcList           @b{ (input) }   Source List
*
* @returns None
*
* @notes  EXCLUDE(X,Y) + TO_EX(B) or IS_EX(B) -> EXCLUDE(B-Y,Y*B)
* @end
*********************************************************************/
void mgmd_proxy_EXCLUDE_BminusY_YxB (mgmd_cb_t *mgmdCB, 
                                     mgmd_host_group_t *mgmd_host_group,
                                     L7_uint32 numSrcs, 
                                     L7_inet_addr_t *sourceList)
{

  mgmd_host_source_record_t *srcRec = L7_NULL;
  mgmd_host_source_record_t *newSrcRec = L7_NULL;
  L7_uint32                  index = 0;
  L7_sll_t                   temp_src_records = mgmd_host_group->sourceRecords;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  /* Proxy State = Exclu(X, Y),  Src = (B),  New State = Excl(B-Y,Y*B) */

  if (SLLCreate(L7_FLEX_MGMD_MAP_COMPONENT_ID, L7_SLL_ASCEND_ORDER,L7_NULL, mgmd_proxy_source_record_compare,
                mgmd_proxy_source_record_destroy, &(mgmd_host_group->sourceRecords)) != L7_SUCCESS)
  {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "SLL creation Error .  \n");
    return;
  }

  for (index = 0; index < numSrcs; index++)
  {
    mgmd_host_source_record_t srcRecord;
    srcRecord.sourceAddress = sourceList[index];
    if ((srcRec = (mgmd_host_source_record_t *)SLLFind(&(temp_src_records),(L7_sll_member_t *)&srcRecord)) != L7_NULL)
    {
      if (mgmd_proxy_src_rec_timeout(srcRec) == L7_TRUE)
      {
        /* The matching hostSrcRec belongs set Y. */
        /* This Src belongs to (Y*B), so add with Timeout = 0. */
        mgmd_proxy_util_new_src_record_create (mgmdCB, sourceList[index], &newSrcRec);
        if (newSrcRec == L7_NULL)
        {
           MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not create the source Node \n");
          /* Return as Memory allocation failed. */
          if (SLLDestroy(L7_FLEX_MGMD_MAP_COMPONENT_ID, &(temp_src_records)) != L7_SUCCESS)
          {
              MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "SLL Not destroyed.  \n");
          }
          return;
        }
        /* Set Source-Timeout = 0. */
        newSrcRec->sourceInterval = 0;
        mgmd_proxy_src_rec_add (mgmdCB, mgmd_host_group, newSrcRec);
        break;
      }
    }
    else
    {
      mgmd_proxy_util_new_src_record_create (mgmdCB, sourceList[index], &srcRec);
      if (srcRec == L7_NULL)
      {
         MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not create the source Node \n");
        /* Return as Memory allocation failed. */
        if (SLLDestroy(L7_FLEX_MGMD_MAP_COMPONENT_ID, &(temp_src_records)) != L7_SUCCESS)
        {
            MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "SLL Not destroyed.  \n");
        }
        return;
      }
      mgmd_proxy_src_rec_add (mgmdCB, mgmd_host_group, srcRec);
    }

  }/*End-of-FOR-LOOP*/

  if (SLLDestroy(L7_FLEX_MGMD_MAP_COMPONENT_ID, &(temp_src_records)) != L7_SUCCESS)
  {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "SLL Not destroyed.  \n");
    return;                    
  }

}


/*****************************************************************
* @purpose This function does the EXCLUDE((B-Y)+(Y-B),Y*B)
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    numSrcs           @b{ (input) }   Source Count
* @param    srcList           @b{ (input) }   Source List
*
* @returns None
*
* @notes  EXCLUDE(X,Y) + TO_EX(B) or IS_EX(B) -> EXCLUDE(X+(B-Y)+(Y-B),Y*B)
* @end
*********************************************************************/
static void mgmd_proxy_EXCLUDE_BminusYplusYminusB_YxB (mgmd_cb_t *mgmdCB, 
                                                       mgmd_host_group_t *mgmd_host_group,
                                                       L7_uint32  numSrcs, 
                                                       L7_inet_addr_t *sourceList)
{

  mgmd_host_source_record_t *srcRec = L7_NULL, srcRecSrchKey;
  L7_uint32                  index = 0;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered ");

  /* Proxy State = Exclu(X, Y),  Src = (B),  New State = Excl(B-Y,Y*B) */
  for (index = 0; index < numSrcs; index++)
  {
    /* Check to see if this source is already in the list */
    memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
    inetCopy(&srcRecSrchKey.sourceAddress,&sourceList[index]);
    srcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), 
                                                  (L7_sll_member_t *)&srcRecSrchKey);

    if (srcRec == L7_NULL)
    {
      /*This Src is not in List, so Add it to List with SrcTimeout=MALI. */
      mgmd_proxy_util_new_src_record_create (mgmdCB, sourceList[index], &srcRec);
      if (srcRec == L7_NULL)
      {
        /* Return as Memory allocation failed. */
         MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not create the source Node \n");                
        return;
      }
      mgmd_proxy_src_rec_add (mgmdCB, mgmd_host_group, srcRec);
    }
  }/*End-of-FOR-LOOP*/

  /* Move Y srcs not present in B to include mode */
  for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
      srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)srcRec))
  {
    if (mgmd_proxy_src_rec_timeout(srcRec) == L7_TRUE)
    {
      for (index = 0; index < numSrcs; index++)
      {
        if (L7_INET_IS_ADDR_EQUAL(&(srcRec->sourceAddress), &sourceList[index]))
        {
          break;
        }
      }
      if (index == numSrcs)
      {
        /* move it to include mode */
        srcRec->sourceCtime = osapiUpTimeRaw();
        srcRec->sourceInterval = IGMP_PROXY_MALI;
      }
    }
  }
  return;   
}


/*****************************************************************
* @purpose  it handles the router report event call backs 
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    eventType         @b{ (input) }   Event Type
* @param    ifNum             @b{ (input) }   Interface Number
* @param    group             @b{ (input) }   Group Address
* @param    numSrcs           @b{ (input) }   Source Count
* @param    srcList           @b{ (input) }   Source List
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  
* @end
********************************************************************/
L7_uint32 mgmd_proxy_routerReportEvent(mgmd_cb_t *mgmdCB, 
                                              L7_uint32 eventType, L7_uint32 ifNum, 
                                              L7_inet_addr_t group, L7_uint32 numSrcs, 
                                              L7_inet_addr_t *sourceList)
{
  mgmd_host_group_t        *mgmd_host_group = L7_NULL;
  L7_inet_addr_t           *apiSrcList;
  L7_uint32                 apiSrcsCount = 0;
  mgmd_host_info_t  *mgmd_host_info;
  mgmd_host_info = mgmdCB->mgmd_host_info;
 
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered, event = %d ", eventType);

  apiSrcList = mgmd_host_info->reportSrcList1;

  mgmd_proxy_host_group_get(mgmdCB, group, &mgmd_host_group);
  if (mgmd_host_group == L7_NULL)
  {
    /* Proxy Group info not found - so create it */
    mgmd_proxy_group_create(mgmdCB, group, &mgmd_host_group);
    if (mgmd_host_group != L7_NULL)
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  numSrcs = %d, event = %d , version = %d",
                       numSrcs, eventType, mgmd_host_info->hostCompatibilityMode);

      if (((numSrcs == 0) && ((eventType == L7_IGMP_MODE_IS_EXCLUDE) ||
                              (eventType == L7_IGMP_CHANGE_TO_EXCLUDE_MODE)) &&
           (mgmd_host_info->hostCompatibilityMode != L7_MGMD_VERSION_3)) ||
          ((mgmd_host_info->hostCompatibilityMode != L7_MGMD_VERSION_3) && (numSrcs !=0)))
      {
        /* Send the EXCL(NULL) report upstream */
        if (mgmd_proxy_grp_report_send(mgmdCB,mgmd_host_group->group, 0, 
                                       L7_NULL, L7_IGMP_MODE_IS_EXCLUDE, mgmd_host_group)
                != L7_SUCCESS)
        {
          MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"Failed to send initial report out");
          return L7_FAILURE;
        }
        /* Schedule the Unsolicited timer for resending this report. */ 
        mgmd_proxy_unsolicited_timer_schedule(mgmdCB,mgmd_host_group->group, 0, L7_NULL,
                                              L7_IGMP_MODE_IS_EXCLUDE);
      }
    }
    else
    {
      return L7_FAILURE;
    }
  }
  
  switch (eventType)
  {
  case L7_IGMP_MODE_IS_INCLUDE:            
    {
      if (mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
      {
        /* 1. INCLUDE (A+B) and IncludeAdd (B) */ 
        mgmd_proxy_IncludeAdd (mgmdCB,ifNum,mgmd_host_group,numSrcs,sourceList);        

        /* 2. (B) = MALI */
        mgmd_proxy_src_tmrs_grp_rec_update (mgmd_host_group, numSrcs,
                                            sourceList, IGMP_PROXY_MALI);
      }
        else
        { /* MGMD_FILTER_MODE_EXCLUDE */
        /* 1. EXCLUDE (X+B, Y-B) */
        mgmd_proxy_EXCLUDE_XplusB_YminusB (mgmdCB, mgmd_host_group, numSrcs,
                                         sourceList);
        /* 2. (B) = MALI */
        mgmd_proxy_src_tmrs_grp_rec_update (mgmd_host_group, numSrcs,
                                            sourceList, IGMP_PROXY_MALI);
        /* 3. ExcludeAdd (B) */ 
        mgmd_proxy_ExcludeAdd1(mgmdCB, ifNum, mgmd_host_group, 
                               numSrcs, sourceList);        
      }
    }
    break;

  case L7_IGMP_MODE_IS_EXCLUDE:
    {
  
      if (mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
      {
        mgmd_proxy_includeToExclude_new(mgmdCB, ifNum, mgmd_host_group,
                                        numSrcs,sourceList);

        /* 6. If (GPL == NULL) Send TO_EXCL(S) State change Report Upstream. */ 
          /*  if (mgmd_proxy_portlist_status(&mgmd_host_group->grpPortList)==0)
        {
          mgmd_proxy_to_excl_send(mgmdCB, mgmd_host_group);
            } */
          mgmd_proxy_to_excl_send(mgmdCB, mgmd_host_group);

        }
        else
        { /* MGMD_FILTER_MODE_EXCLUDE */
        mgmd_host_source_record_t *hostSrcRec = L7_NULL;
        L7_uint32             index = 0;
        L7_inet_addr_t       *B_YsrcList =  mgmd_host_info->reportSrcList2;
        L7_int32              B_YsrcCount = 0;

        /* Proxy State = Excl(X,Y), Report = IS_EX(B), 
           New State = Excl(B-Y, Y*B). */           
        

        /* 0. Generate (B-(X+Y)) ..to be used in Step 6. */
        for (index = 0; index < numSrcs; index++)
        {
          /* Check to see if this source is already in the list */
            mgmd_host_source_record_t srcRecSrchKey;
            memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
            inetCopy(&srcRecSrchKey.sourceAddress, &sourceList[index]);
            hostSrcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), 
                                                              (L7_sll_member_t *)&srcRecSrchKey);

          if (hostSrcRec == L7_NULL)
          {
            B_YsrcList[B_YsrcCount] = sourceList[index];
            B_YsrcCount++;
          }
        }/*End-of-FOR-LOOP*/

         /* 0. Generate ( (x+Y)-B) for Step-7. */
          for (hostSrcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); hostSrcRec != L7_NULL; 
            hostSrcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)hostSrcRec))
        {
          /* Check to see if this source is already in list(B). */
          for (index = 0; index < numSrcs; index++)
          {
            if (L7_INET_IS_ADDR_EQUAL(&(hostSrcRec->sourceAddress), &sourceList[index]))
            {
              break;             
            }
          }/*End-of-FOR-LOOP*/

          if (index == numSrcs)
          {
            apiSrcList[apiSrcsCount] = hostSrcRec->sourceAddress;
            apiSrcsCount++;                  
          }
        }/*End-of-for*/


        /* 1. EXCLUDE ((B-Y)+(Y-B), Y*B). */
        /* 2. (B-X-Y) = MALI.     */
        /* 3. Delete (X-B).       */
        mgmd_proxy_EXCLUDE_BminusYplusYminusB_YxB (mgmdCB, mgmd_host_group, 
                                                   numSrcs, sourceList);

        /* 6. ExcludeAdd2 (B-Y, Y-B). */
        mgmd_proxy_ExcludeAdd2(mgmdCB, ifNum, mgmd_host_group, 
                               apiSrcsCount, apiSrcList,
                               B_YsrcCount, B_YsrcList);
      }   
    }
    break;

  case L7_IGMP_CHANGE_TO_INCLUDE_MODE:
    {
      if (mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
      {
        /* 1. INCLUDE (A+B) */

        /* 2. IncludeAdd (B) */ 
        mgmd_proxy_IncludeAdd (mgmdCB, ifNum, mgmd_host_group, 
                               numSrcs, sourceList);
        /* 3. (B) = MALI */
        mgmd_proxy_src_tmrs_grp_rec_update (mgmd_host_group, numSrcs,
                                            sourceList, IGMP_PROXY_MALI);
                
      } 
        else
        { /* MGMD_FILTER_MODE_EXCLUDE */
        /* 1. EXCLUDE (X+B, Y-B) */
        mgmd_proxy_EXCLUDE_XplusB_YminusB (mgmdCB, mgmd_host_group, numSrcs,
                                           sourceList);
        /* 2. (B) = MALI */
        mgmd_proxy_src_tmrs_grp_rec_update (mgmd_host_group, numSrcs,
                                            sourceList, IGMP_PROXY_MALI);
        /* 3. ExcludeAdd (B) */ 
        mgmd_proxy_ExcludeAdd1(mgmdCB, ifNum, mgmd_host_group, 
                               numSrcs, sourceList);        
      }
    }
    break;

  case L7_IGMP_CHANGE_TO_EXCLUDE_MODE:
    {
      if (mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
      {
        mgmd_proxy_includeToExclude_new(mgmdCB, ifNum,mgmd_host_group,
                                        numSrcs, sourceList);

        mgmd_proxy_to_excl_send(mgmdCB, mgmd_host_group);
               
      } 
        else
        { /* MGMD_FILTER_MODE_EXCLUDE */
          mgmd_host_source_record_t *hostSrcRec = L7_NULL, srcRecSrchKey;
        L7_uint32              timeout = 0; 
        L7_int32              index = 0;
        L7_inet_addr_t       *B_XplusYsrcList = mgmd_host_info->reportSrcList2;
        L7_int32              B_XplusYsrcCount = 0;
        mgmd_group_t        *mgmd_group, searchKey;

        /* Proxy State = Excl(X,Y), Report = TO_EX(B),  *
         * New State = Excl(B-Y, Y*B).                  */

        /* 0. Generate (B-(X+Y)) ..to be used in Step 6. */
        for (index = 0; index < numSrcs; index++)
        {
          /* Check to see if this source is already in the list */
            memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
            inetCopy(&srcRecSrchKey.sourceAddress, &sourceList[index]);
            hostSrcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), 
                                                              (L7_sll_member_t *)&srcRecSrchKey);

          if (hostSrcRec == L7_NULL)
          {
            B_XplusYsrcList[B_XplusYsrcCount] = sourceList[index];
            B_XplusYsrcCount++;
          }
        }/*End-of-FOR-LOOP*/

         /* 0. Generate ( (x+Y)-B) for Step-7. */
          for (hostSrcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); hostSrcRec != L7_NULL; 
            hostSrcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)hostSrcRec))
        {   
             /* Check to see if this source is already in list(B). */
            for (index = 0; index < numSrcs; index++)
            {
              if (L7_INET_IS_ADDR_EQUAL(&(hostSrcRec->sourceAddress), 
                                        &sourceList[index]))
              {
                break;             
              }
            }/*End-of-FOR-LOOP*/

            if (index == numSrcs)
            {
              apiSrcList[apiSrcsCount] = hostSrcRec->sourceAddress;
              apiSrcsCount++;                  
            }
        }/*End-of-for*/

        /* 1. EXCLUDE (X+(B-Y)+(Y-B), Y*B). */
        mgmd_proxy_EXCLUDE_BminusYplusYminusB_YxB (mgmdCB, mgmd_host_group, 
                                                   numSrcs, sourceList);
        
        /* 4.c. Now update the Filter-Timer timeout to the new set. 
           NOTE: (B-X-Y) == (B-(X+Y)). And below we pass B_XplusYsrcList. */

        searchKey.rtrIfNum = ifNum;
        if (inetCopy(&searchKey.group, &group) != L7_SUCCESS)
        {
          MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, " InetCopy Failed. \n");  
          return L7_FAILURE;
        }
        if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
        {
           MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "MGMD : Failed to get group info fron IGMP database \n");
          return L7_FAILURE;
        }
        if (appTimerTimeLeftGet(mgmdCB->timerHandle,
                                mgmd_group->groupTimer, 
                                &timeout) != L7_SUCCESS)
        {
           MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, " Unable to get left over time in group filter timer ");
        }

        mgmd_proxy_src_tmrs_grp_rec_update (mgmd_host_group, B_XplusYsrcCount,
                                            B_XplusYsrcList, timeout);

        /* 7. ExcludeAdd2 (B-Y, Y-B). */
        mgmd_proxy_ExcludeAdd2 (mgmdCB, ifNum, mgmd_host_group, 
                                apiSrcsCount,apiSrcList,
                                B_XplusYsrcCount, B_XplusYsrcList);
      }
    }
    break;

  case L7_IGMP_ALLOW_NEW_SOURCES:
    {
      if (mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
      {
        /* 1. INCLUDE (A+B) */
      
        /* 2. IncludeAdd (B) */ 
        mgmd_proxy_IncludeAdd (mgmdCB, ifNum, mgmd_host_group, 
                               numSrcs, sourceList);        
        /* 3. (B) = MALI */
        mgmd_proxy_src_tmrs_grp_rec_update (mgmd_host_group, numSrcs,
                                            sourceList, IGMP_PROXY_MALI);
                
      } 
        else
        { /* MGMD_FILTER_MODE_EXCLUDE */

        /* 1. EXCLUDE (X+B, Y-B) */
        mgmd_proxy_EXCLUDE_XplusB_YminusB (mgmdCB, mgmd_host_group, numSrcs,
                                           sourceList);
        /* 2. (B) = MALI */
        mgmd_proxy_src_tmrs_grp_rec_update (mgmd_host_group, numSrcs,
                                            sourceList, IGMP_PROXY_MALI);
        /* 3. ExcludeAdd (B) */ 
        mgmd_proxy_ExcludeAdd1(mgmdCB, ifNum, mgmd_host_group, 
                               numSrcs, sourceList);        
      }
    }
    break;

  case L7_IGMP_BLOCK_OLD_SOURCES:
    {
      if (mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
      {
        /* Proxy State = Incl(A), Report = IS_IN(B), New State = Incl(A+B) */

        /* 1. INCLUDE(A). No changes to be done in Src Records of this Group.*/
      } 
      else
      { /* MGMD_FILTER_MODE_EXCLUDE */
        mgmd_host_source_record_t *hostSrcRec = L7_NULL, srcRecSrchKey;
        L7_inet_addr_t      *B_YsrcList = apiSrcList; /* this already has reportSrcList1*/
        L7_int32             B_YsrcCount = 0;
        L7_int32             index = 0;
        L7_int32             timeout = 0;
        L7_inet_addr_t      *B_XplusYsrcList = mgmd_host_info->reportSrcList2;
        L7_int32             B_XplusYsrcCount = 0;        
        mgmd_group_t        *mgmd_group, searchKey;

        /* Proxy State = Excl(X,Y), Report = BLOCK(B),
           New State   = Excl(X+(B-Y), Y). */
        
        /* 0. Generate (B-(X+Y)) ..to be used in Step 2. */
        for (index = 0; index < numSrcs; index++)
        {
          /* Check to see if this source is already in the list */
          memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
          inetCopy(&srcRecSrchKey.sourceAddress, &sourceList[index]);
          hostSrcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), 
                                                            (L7_sll_member_t *)&srcRecSrchKey);

          if (hostSrcRec == L7_NULL)
          {
            B_XplusYsrcList[B_XplusYsrcCount] = sourceList[index];
            B_XplusYsrcCount++;
          }
        } /*End-of-FOR-LOOP*/


        /* 0. Generate (B-Y) ..to be used in Step 1. */
        for (index = 0; index < numSrcs; index++)
        {
          /* Check to see if this source is already in the list */
          memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
          inetCopy(&srcRecSrchKey.sourceAddress, &sourceList[index]);
          hostSrcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), 
                                                            (L7_sll_member_t *)&srcRecSrchKey);

          if (hostSrcRec != L7_NULLPTR)
          {
            if (mgmd_proxy_src_rec_timeout(hostSrcRec) != L7_TRUE)
               hostSrcRec = L7_NULLPTR;
          }
          if (hostSrcRec == L7_NULLPTR)
          {
            B_YsrcList[B_YsrcCount] = sourceList[index];
            B_YsrcCount++;
          }
        } /*End-of-FOR-LOOP*/

        /* 1. Call the ExcludeDelete(B-Y) */
        mgmd_proxy_EXCLUDE_XplusBminusY_Y(mgmdCB, mgmd_host_group,
                                 B_YsrcCount, B_YsrcList, ifNum);
  
        /* 2. (B-X-Y) = Filter-Timer. [For all new srcs added from B, set src timer to Filter-Timer.] */
        /* 2.a. First Calculate Remaining time for Filter-Timer of this group. */
        /* 2.b. Now update the Filter-Timer timeout to the new set.       *
         * NOTE: (B-X-Y) == (B-(X+Y)). And below we pass B_XplusYsrcList. */

          searchKey.rtrIfNum = ifNum;
          if (inetCopy(&searchKey.group, &group) != L7_SUCCESS)
          {
            MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, " InetCopy Failed. \n");  
            return L7_FAILURE;
          }
          if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
          {
             MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "MGMD : Failed to get group info fron IGMP database \n");
            return L7_FAILURE;
          }
          if (appTimerTimeLeftGet(mgmdCB->timerHandle,
                                  mgmd_group->groupTimer, 
                                  &timeout) != L7_SUCCESS)
          {
             MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, " Unable to get left over time in group filter timer ");
          }

          mgmd_proxy_src_tmrs_grp_rec_update(mgmd_host_group, B_XplusYsrcCount, 
                                           B_XplusYsrcList, timeout);
      }
    }
    break;

  default:  break;
  }/*End-of-Switch*/

  return L7_SUCCESS;
}


/*****************************************************************
* @purpose  Handles the router timer events call backs
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    eventType         @b{ (input) }   Event Type
* @param    ifNum             @b{ (input) }   Interface Number
* @param    flag              @b{ (input) }   Flag
* @param    group             @b{ (input) }   Group Address
* @param    group_source      @b{ (input) }   Source Address

*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  It may be a source timer expiry or group timer expiry
* @end
*********************************************************************/
L7_uint32 mgmd_proxy_routerTimerEvents(mgmd_cb_t *mgmdCB, 
                                              L7_uint32 eventType, L7_uint32 ifNum,L7_inet_addr_t group, L7_inet_addr_t group_source, L7_uint32 numSrcs, 
                                              L7_inet_addr_t *sourceList)
{
  mgmd_host_group_t *mgmd_host_group = L7_NULLPTR;
  mgmd_host_info_t  *mgmd_host_info;

  mgmd_host_info = mgmdCB->mgmd_host_info;
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered, event = %d ", eventType);
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered, srcList = %d ", sourceList);

  switch (eventType)
  {
    case L7_IGMP_PROXY_GRP_TIMER_EXPIRY:
      {
        mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7 (&(mgmd_host_info->hostMembership), &group,
                                                              AVL_EXACT);
        if (mgmd_host_group != L7_NULL)
        {
          mgmd_proxy_group_record_delete(mgmdCB, ifNum, mgmd_host_group, numSrcs, sourceList);
          mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7 (&(mgmd_host_info->hostMembership), &group,
                                                                AVL_EXACT);
          {
            MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," grp Record is not yet deleted ");
            mgmd_proxy_group_mode_change(mgmdCB, ifNum, mgmd_host_group,numSrcs,sourceList);  
          }
        }
      }
      break;
    case L7_IGMP_PROXY_SRC_TIMER_EXPIRY:
      {
        mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7 (&(mgmd_host_info->hostMembership), &group,
                                                              AVL_EXACT);
        mgmd_proxy_src_timer_expire_handler(mgmdCB, ifNum, group, 1, &group_source);
        if (mgmd_host_group != L7_NULLPTR)
        {

          MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n Found group ");
          if (mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
          {
            mgmd_proxy_IncludeDelete(mgmdCB, ifNum, mgmd_host_group, 1, &group_source); 
          }
          else
          {
            mgmd_proxy_ExcludeSrcDelete (mgmdCB, ifNum, mgmd_host_group, 1, &group_source);
          } 

        }
      }
      break;
    default:
      break;
  }/*End-of-Switch*/

  return 0;
}


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
void mgmd_proxy_v3membership_query_process (mgmd_cb_t *mgmdCB, L7_inet_addr_t group, 
                                            L7_inet_addr_t source, L7_uchar8 max_resp_code,
                                            L7_mgmdv3Query_t *v3Query)
{
  mgmd_host_group_t         *mgmd_host_group = L7_NULL;
  mgmd_host_source_record_t *srcRec = L7_NULL;
  L7_int32                   result, numSrcs, i;
  L7_inet_addr_t             srcs;
  L7_int32                   selectedDelay = 0, grpTime = 0;
  mgmd_host_info_t          *mgmd_host_info = mgmdCB->mgmd_host_info;
  mgmd_host_group_t          dummyGroup;
  L7_BOOL                    grpTimerSet = L7_FALSE, srcTmrFlag = L7_FALSE;
  L7_uint32 max_resp_time;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  if (mgmd_host_info == L7_NULLPTR)
    return;

  /* Do compatibility check. */
  if (mgmd_host_info->hostCompatibilityMode != L7_MGMD_VERSION_3)
  {
    return;
  }
  MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_QUERY, " Existing Querier = ",
                        &mgmd_host_info->querierIP);

  if (inetIsAddressZero(&mgmd_host_info->querierIP) == L7_TRUE)
  {
    inetCopy(&mgmd_host_info->querierIP, &source);
  }
  else
  {
    result = L7_INET_ADDR_COMPARE(&mgmd_host_info->querierIP, &source);
  
    if (result > 0)
    {
      /* This querier has lower ip address than the current */
      mgmd_host_info->querierIP = source;
    }
    else if (result < 0)
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Rx'ed invalid query ");
      return;
    }
  }

  numSrcs = v3Query->mgmdQueryNumSrcs;

  /* Calculate the Selected delay */
  max_resp_time = mgmd_decode_max_resp_code(mgmdCB->proto, max_resp_code);  
  selectedDelay = mgmd_proxy_selected_delay_calculate((L7_int32) max_resp_time);

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_QUERY," selected delay = %d ", selectedDelay);
  if (!inetIsInAddressAny(&group))
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_QUERY, " Rx'ed grp-specific (and src-specific ) query ");
    mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7 (&(mgmd_host_info->hostMembership), &group,
                                                          AVL_EXACT);
    if (mgmd_host_group == L7_NULL)
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_FAILURE," Failed to find group for which grp-query is rx'ed");
      return;
    }

    /* grpTimerSet flag is set to know whether to schedule a response for the query.
       when the group-specific query is rx'ed, timer is set (or) 
       when there is sources in query which need to be responded, timer flag is set */   
    if (mgmd_host_group->grp_rsp_timer == L7_NULLPTR)
    {
      if (numSrcs != 0)
      {
        for (i = 0; i < numSrcs; i++)
        {
          mgmd_host_source_record_t srcRecSrchKey;

          srcs = v3Query->mgmdQuerySrcs[i];

          memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
          inetCopy(&srcRecSrchKey.sourceAddress, &srcs);

          if ((srcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)&srcRecSrchKey)))
          {
            srcTmrFlag = mgmd_proxy_src_rec_timeout(srcRec);
            if ((mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE) ||
                ((mgmd_host_group->filterMode == MGMD_FILTER_MODE_EXCLUDE) && srcTmrFlag == L7_FALSE))
            {
              srcRec->sendQueryReport = L7_TRUE;
              grpTimerSet = L7_TRUE;
            }
          }
        }
      }
      else
      {
        grpTimerSet = L7_TRUE;
      }
      /* RFC 3376 section 5.2 and statement 3 */
      if (grpTimerSet == L7_TRUE)
      {
        grpTime = selectedDelay;
        if (selectedDelay > IGMP_PROXY_GROUP_TIMER)
        {
          grpTime = IGMP_PROXY_GROUP_TIMER;
        }
        if (mgmdProxyUtilAppTimerSet(mgmdCB, L7_NULL, 
                                     (void*)mgmd_host_group->grp_rsp_timer_handle,
                                     &mgmd_host_group->grp_rsp_timer,
                                     grpTime, 
                                     L7_MGMD_GRP_RSP_TIMER) != L7_SUCCESS)
        {
          MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not start the Group Response timer  \n");
          return;
        }
      }
    }
    else
    {
      /* RFC 3376 section 5.2 and statement 4 */
      if (numSrcs == 0)
      {
        /* clear the source list for the group */
        for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
            srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)srcRec))
        {
          srcRec->sendQueryReport = L7_FALSE;
        }
      }
      else
      {
        /* RFC 3376 section 5.2 and statement 5 */
        /* add the new sources to the send report list */
        for (i = 0; i < numSrcs; i++)
        {
          mgmd_host_source_record_t srcRecSrchKey;

          srcs = v3Query->mgmdQuerySrcs[i];

          memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
          inetCopy(&srcRecSrchKey.sourceAddress, &srcs);

          if ((srcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)&srcRecSrchKey)))
          {
            srcRec->sendQueryReport = L7_TRUE;
          }
        } /*End-of-FOR-Loop*/
      }
      if (mgmdProxyUtilAppTimerLower(mgmdCB, L7_NULL, (void *)mgmd_host_group->grp_rsp_timer_handle,
                                     &mgmd_host_group->grp_rsp_timer,selectedDelay, 
                                     L7_MGMD_GRP_RSP_TIMER) != L7_SUCCESS)
      {
        MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not update the Group Response timer with delay(%d)\n",
                         selectedDelay);
        return;
      }
    } /* end of else part of grp rsp timer */
  }
  else
  {
    /* RFC 3376 section 5.2 and statement 1 & 2 */
    /* Recieved V3 General Query */
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_QUERY, " Rx'ed general query ");
    if (mgmdProxyUtilAppTimerLower(mgmdCB, L7_NULL, (void *)mgmdCB,
                                   &mgmd_host_info->gen_rsp_timer,selectedDelay, 
                                   L7_MGMD_GENERAL_RSP_TIMER) != L7_SUCCESS)
    {
      /* Statement 2 */
      if (selectedDelay > IGMP_PROXY_INTERFACE_TIMER)
      {
        selectedDelay = IGMP_PROXY_INTERFACE_TIMER;
      }
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_QUERY, "Starting the interface timer ");
      if (mgmdProxyUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmdCB, 
                                 &mgmd_host_info->gen_rsp_timer, 
                                 selectedDelay,
                                 L7_MGMD_GENERAL_RSP_TIMER) != L7_SUCCESS)
      {
         MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not start the General Response timer  \n");
        return;      
      }
    }
  }

  /* Rate limiting reports send on proxy interface*/
  memset (&dummyGroup, 0, sizeof(mgmd_host_group_t));
  mgmd_host_group = (mgmd_host_group_t*)
                    avlSearchLVL7 (&(mgmd_host_info->hostMembership),
                    (void*) (&dummyGroup), AVL_NEXT);
  while (mgmd_host_group != L7_NULLPTR)
  {
    mgmd_host_group->sendReportUponQuery = L7_TRUE;
    mgmd_host_group = (mgmd_host_group_t*)
                      avlSearchLVL7 (&(mgmd_host_info->hostMembership),
                      (void*)(mgmd_host_group), AVL_NEXT);
  }

  return;
}


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
void mgmd_proxy_membership_query_process(mgmd_cb_t *mgmdCB, L7_inet_addr_t group, 
                                         L7_inet_addr_t source,
                                         L7_uint32 max_resp_time)
{

  L7_int32                   result;
  L7_int32                   selectedDelay;
  mgmd_host_group_t         *mgmd_host_group = L7_NULL;
  mgmd_host_info_t          *mgmd_host_info = mgmdCB->mgmd_host_info;
  mgmd_host_group_t         *temp_mgmd_host_group = L7_NULL;
  mgmd_host_source_record_t *hostSrcRec = L7_NULL;
  mgmd_host_group_t          dummyGroup;
  L7_uint32                  mgmdHostVersion;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"  Entered ");

  mgmdHostVersion = mgmdCB->mgmd_host_info->configVersion;

  if (mgmd_host_info== L7_NULLPTR)
    return;

  MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_QUERY, " Existing Querier = ",
                        &mgmd_host_info->querierIP);
  if (inetIsAddressZero(&mgmd_host_info->querierIP) == L7_TRUE)
  {
    inetCopy(&mgmd_host_info->querierIP, &source);
  }
  else
  {
    result = L7_INET_ADDR_COMPARE(&mgmd_host_info->querierIP, &source);

    if (result > 0)
    {
      /* This querier has lower ip address than the current */
      mgmd_host_info->querierIP = source;
    }
    else if (result < 0)
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Rx'ed invalid query ");
      return;
    }
  }

  MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_QUERY," Group in Query = ", &group);

  mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7 (&(mgmd_host_info->hostMembership), &group,
                                                        AVL_EXACT);

  if ((mgmdCB->proto == L7_AF_INET) && (max_resp_time == 0))
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_QUERY," Rx'ed V1 query ");
    /* Update the Stats. */

    /* If Default-Host-Version is not IGMPv1, update GroupCompatabilityMode.*/
    if (mgmdHostVersion  > L7_MGMD_VERSION_1)
    {
      if (mgmd_host_info->hostCompatibilityMode != L7_MGMD_VERSION_1)
      {
        mgmd_host_info->hostCompatibilityMode = L7_MGMD_VERSION_1;

        /* RFC-3376 Sec 7.2.1[Whenever a host changes its compatibility mode,
         * it cancels all its pending response and retransmission timers]. */
        /* 1. Clear all Retransmissions */
        SLLPurge(L7_FLEX_MGMD_MAP_COMPONENT_ID, &(mgmd_host_info->ll_unsolicited_reports));

        /* 2. Clear all Pending Responses. */
        memset(&dummyGroup, 0, sizeof(mgmd_host_group_t));

        /* Get first entry in the tree */
        temp_mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7(&(mgmd_host_info->hostMembership), (void *)(&dummyGroup), AVL_NEXT);
        while (temp_mgmd_host_group)
        {
          for (hostSrcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(temp_mgmd_host_group->sourceRecords)); hostSrcRec != L7_NULL; 
              hostSrcRec = (mgmd_host_source_record_t *)SLLNextGet(&(temp_mgmd_host_group->sourceRecords), (L7_sll_member_t *)hostSrcRec))
          {
            hostSrcRec->sendQueryReport = L7_FALSE;
          }/*End-of-for*/

          /* Delete Group response timer , if running */
          if (mgmdProxyUtilAppTimerDelete(mgmdCB, &temp_mgmd_host_group->grp_rsp_timer) != L7_SUCCESS)
          {
            MGMD_PROXY_DEBUG_ADDR(MGMD_DEBUG_FAILURE,"Could not delete the group response timer for grp =\n",
                             &temp_mgmd_host_group->group);          
          }
          /* Get next entry in the tree */
          temp_mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7(&(mgmd_host_info->hostMembership), (void *)(temp_mgmd_host_group), AVL_NEXT);
        }/*End-of-While*/

        /* Delete interface timer , if running */
        if (mgmdProxyUtilAppTimerDelete(mgmdCB, &mgmd_host_info->gen_rsp_timer) != L7_SUCCESS)
        {
          MGMD_PROXY_DEBUG(MGMD_DEBUG_FAILURE,"Could not delete the general response timer.\n");          
        }

      }
      /* set V1 querier present timer to predefined interval. */
      if (mgmd_host_info->ver1_querier_timer != L7_NULLPTR)
      {
        if (appTimerUpdate (mgmdCB->timerHandle, &mgmd_host_info->ver1_querier_timer,
                            mgmd_proxy_v1querier_timeout_event_handler, 
                            mgmdCB, IGMP_VER1_QUERIER_TIMER,
                            "MGMD-Proxy V1 Querier Timer")
                         != L7_SUCCESS)
        {
          MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_FAILURE,
                            "ver1_querier_timer: appTimerUpdate for %d Seconds "
                            "Failed", IGMP_VER1_QUERIER_TIMER);
          return;
        }
      }
      else
      {
        if (mgmdProxyUtilAppTimerSet(mgmdCB, L7_NULL, (void *)mgmdCB,
                                     &mgmd_host_info->ver1_querier_timer,
                                     IGMP_VER1_QUERIER_TIMER, 
                                     L7_MGMD_VERSION1_TIMER) != L7_SUCCESS)
        {
          MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not start the ver1_querier_timer .\n");          
          return;
        }
      }

      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Started general Response timer ");
      /* Calculate the Selected delay */
      selectedDelay = mgmd_proxy_selected_delay_calculate(IGMP_PROXY_INTERFACE_TIMER);

      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_QUERY," Selected Delay = %d ",selectedDelay);
      /* Start the interface timer to respond V1 General query */
      mgmdProxyUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmdCB, 
                               &mgmd_host_info->gen_rsp_timer, 
                               selectedDelay,
                               L7_MGMD_GENERAL_RSP_TIMER);
    }
    else
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_QUERY," Starting interface timer ");
      /* The host has already changed its comatability mode to v1, so  */
      /* Update Interface timer if query is rx'ed again with timer already running */
      selectedDelay = mgmd_proxy_selected_delay_calculate((L7_int32) IGMP_PROXY_INTERFACE_TIMER);
      if (mgmdProxyUtilAppTimerLower(mgmdCB, L7_NULL, (void *)mgmdCB,
                                   &mgmd_host_info->gen_rsp_timer,selectedDelay, 
                                   L7_MGMD_GENERAL_RSP_TIMER) != L7_SUCCESS)
      {
        MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_QUERY," Gen RespTimer = %p",mgmd_host_info->gen_rsp_timer);
        mgmdProxyUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmdCB, 
                                 &mgmd_host_info->gen_rsp_timer, 
                                 IGMP_PROXY_INTERFACE_TIMER,
                                 L7_MGMD_GENERAL_RSP_TIMER);
      }
    }
  }
  else
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_QUERY," Rx'ed V2 query ");
    /* Do compatibility check. */
    if (mgmdHostVersion == L7_MGMD_VERSION_1)
    {
      /* Drop it as we dont support compatibility when received report is higher version (here v2) than
         supported or configured version ( here v1)*/
      return;
    }
    /* if v1 querier present timer is running then ignore, then the v3 host 
       has already changed its compatability mode to v1 and hence there is no
       point in maintaining v2 timer or processing v2 query.  */
    /* set V2 querier present timer to a predefined interval, */

    if (mgmd_host_info->ver1_querier_timer == L7_NULLPTR)
    {
      /* If Default-Host-Version is IGMPv3, update GroupCompatabilityMode.*/
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_QUERY," Rx'ed V2 query ");
      selectedDelay = mgmd_proxy_selected_delay_calculate(max_resp_time);
      if (mgmdHostVersion == L7_MGMD_VERSION_3)
      {
        if (mgmd_host_info->hostCompatibilityMode != L7_MGMD_VERSION_2)
        {
          mgmd_host_info->hostCompatibilityMode = L7_MGMD_VERSION_2;
          /* RFC-3376 Sec 7.2.1[Whenever a host changes its compatibility mode,
           * it cancels all its pending response and retransmission timers]. */
          /* 1. Clear all Retransmissions */
          SLLPurge(L7_FLEX_MGMD_MAP_COMPONENT_ID, &(mgmdCB->mgmd_host_info->ll_unsolicited_reports));

          /* 2. Clear all Pending Responses. */
          memset(&dummyGroup, 0, sizeof(mgmd_host_group_t));

          /* Get first entry in the tree */
          temp_mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7(&(mgmd_host_info->hostMembership), (void *)(&dummyGroup), AVL_NEXT);
          while (temp_mgmd_host_group)
          {
            for (hostSrcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(temp_mgmd_host_group->sourceRecords)); hostSrcRec != L7_NULL; 
                hostSrcRec = (mgmd_host_source_record_t *)SLLNextGet(&(temp_mgmd_host_group->sourceRecords), (L7_sll_member_t *)hostSrcRec))
            {
              hostSrcRec->sendQueryReport = L7_FALSE;
            }/*End-of-for*/

            /* Delete Group response timer , if running */
           if (mgmdProxyUtilAppTimerDelete(mgmdCB, &temp_mgmd_host_group->grp_rsp_timer) != L7_SUCCESS)
           {
            MGMD_PROXY_DEBUG_ADDR(MGMD_DEBUG_FAILURE,"Could not delete the group response timer for grp =\n",
                             &temp_mgmd_host_group->group);          
           }
            /* Get next entry in the tree */
            temp_mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7(&(mgmd_host_info->hostMembership), (void *)(temp_mgmd_host_group), AVL_NEXT);
          }/*End-of-While*/
        }
      }

      if (mgmd_host_group != L7_NULL)
      {
        /* reset the group response timer */
        if (mgmdProxyUtilAppTimerLower(mgmdCB, L7_NULL, (void *)mgmd_host_group->grp_rsp_timer_handle,
                                       &mgmd_host_group->grp_rsp_timer,selectedDelay, 
                                       L7_MGMD_GRP_RSP_TIMER) != L7_SUCCESS)
        {
          if (mgmdProxyUtilAppTimerSet(mgmdCB, L7_NULL, 
                                     (void*)mgmd_host_group->grp_rsp_timer_handle,
                                     &mgmd_host_group->grp_rsp_timer,
                                     selectedDelay, 
                                     L7_MGMD_GRP_RSP_TIMER) != L7_SUCCESS)
          {
            MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not start the Group Response timer  \n");
            return;
          }
        }
      }
      if (inetIsAddressZero(&group) == L7_TRUE)
      {
        MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Started general Response timer ");
        if (selectedDelay > IGMP_PROXY_INTERFACE_TIMER)
        {
          selectedDelay = IGMP_PROXY_INTERFACE_TIMER;
        }
        if (mgmdProxyUtilAppTimerSet(mgmdCB, L7_NULL, (void*)mgmdCB, 
                                     &mgmd_host_info->gen_rsp_timer, 
                                     selectedDelay,
                                     L7_MGMD_GENERAL_RSP_TIMER) != L7_SUCCESS)
        {
          MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not start the gen_rsp_timer .\n");         
          return;
        }
      }
    }
    /* Do not update v2_querier timer  when the configured version is v2 
       If configured Host version is V3 , then a v2_querier_timer has to be 
       started/updated when v2 query is received. If configured host version 
       is v1, then v2 query is not accepted since higher version, dropped at 
       mgmd itself. But if configured version is v3 and hostCompatability version 
       has changed to v1/v3, then v2 timer is started/updated when the v2 query is received. */
    if (mgmdHostVersion > L7_MGMD_VERSION_2 )
    {
      /* set V2 querier present timer to predefined interval. */
      if (mgmd_host_info->ver2_querier_timer != L7_NULLPTR)
      {
        if (appTimerUpdate (mgmdCB->timerHandle, &mgmd_host_info->ver2_querier_timer,
                            mgmd_proxy_v2querier_timeout_event_handler, 
                            mgmdCB, IGMP_VER2_QUERIER_TIMER,
                            "MGMD-Proxy V2 Querier Timer")
                         != L7_SUCCESS)
        {
          MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_FAILURE,
                            "ver2_querier_timer: appTimerUpdate for %d Seconds "
                            "Failed", IGMP_VER2_QUERIER_TIMER);
          return;
        }
      }
      else
      {
        if (mgmdProxyUtilAppTimerSet(mgmdCB, L7_NULL, (void *)mgmdCB,
                                     &mgmd_host_info->ver2_querier_timer,
                                     IGMP_VER2_QUERIER_TIMER, 
                                     L7_MGMD_VERSION2_TIMER) != L7_SUCCESS)
        {
          MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "Could not start the ver2_querier_timer .\n");     
          return;
        }
      }
    }
  }

  /* Rate limiting reports send on proxy interface*/
  memset (&dummyGroup, 0, sizeof(mgmd_host_group_t));
  mgmd_host_group = (mgmd_host_group_t*)
                    avlSearchLVL7 (&(mgmd_host_info->hostMembership),
                    (void*) (&dummyGroup), AVL_NEXT);
  while (mgmd_host_group != L7_NULLPTR)
  {
    mgmd_host_group->sendReportUponQuery = L7_TRUE;
    mgmd_host_group = (mgmd_host_group_t*)
                      avlSearchLVL7 (&(mgmd_host_info->hostMembership),
                      (void*)(mgmd_host_group), AVL_NEXT);
  }

  return;
}

/*****************************************************************
* @purpose Sends a v3/v2/v1 report for the specified group 
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    group             @b{ (input) }   Group Address
* @param    numSrcs           @b{ (input) }   Number of Sources
* @param    sourceSet         @b{ (input) }   Source Address List
* @param    grpRecType        @b{ (input) }   Group Record Type
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  depending upon the arguments it send a grp report or group & 
*         src specific report 
* @end
*********************************************************************/
static L7_RC_t mgmd_proxy_grp_report_send(mgmd_cb_t *mgmdCB, L7_inet_addr_t group, 
                                           L7_ushort16 numSrcs, L7_inet_addr_t **sourceSet, 
                                           L7_uchar8 grpRecType, mgmd_host_group_t *mgmd_host_group)
{
  L7_uchar8            *dataStart = mgmdCB->pktTxBuf;
  L7_uchar8            *data = dataStart;
  L7_uint32             dataLen;
  L7_ushort16           checksum = MGMD_ZERO, maxRespTime = MGMD_ZERO;
  L7_uint8              zero = MGMD_ZERO;
  mgmd_host_info_t     *mgmd_host_info;
  L7_inet_addr_t        destAddr, ipAddr, srcAddr;
  L7_uint8              msgType;
  L7_RC_t               rc = L7_FAILURE;
  L7_uchar8             src[IPV6_DISP_ADDR_LEN];
  
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered ");

  if((mgmdCB == L7_NULLPTR)||
      (mgmdCB->mgmd_host_info == L7_NULLPTR) ||
      (mgmd_host_group == L7_NULLPTR))
  {
     MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, 
            "Invalid input params  \n");
    return L7_FAILURE;
  }    
  inetAddressZeroSet(mgmdCB->proto, &destAddr);
  memset(dataStart, 0 , sizeof(MGMD_PKT_SIZE_MAX));
  mgmd_host_info = mgmdCB->mgmd_host_info;
  
  dataLen = (mgmdCB->proto == L7_AF_INET) ? IGMP_PKT_MIN_LENGTH : MLD_PKT_MIN_LENGTH;

  if (mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_1)
  {
    if (grpRecType == L7_IGMP_BLOCK_OLD_SOURCES)
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," The group leaving in V1 mode ");
      return L7_SUCCESS;
    }
    msgType = L7_IGMP_V1_MEMBERSHIP_REPORT;

    MCAST_PUT_BYTE(msgType, data);
    MCAST_PUT_BYTE(zero, data);
    MCAST_PUT_SHORT(checksum, data);
    MCAST_PUT_ADDR_INET(mgmdCB->proto, &group, data);
    inetCopy(&destAddr, &mgmdCB->all_routers); 
  }
  else if (mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_2)
  {
    if (grpRecType == L7_IGMP_BLOCK_OLD_SOURCES)
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," The group leaving in V2 mode ");
      return L7_SUCCESS;
    }
    if ((grpRecType == L7_IGMP_CHANGE_TO_INCLUDE_MODE) && (numSrcs == 0))
    {
      msgType = (mgmdCB->proto == L7_AF_INET) ? L7_IGMP_V2_LEAVE_GROUP :
                                                L7_MLD_V1_MEMBERSHIP_DONE;
    }
    else
    {
      msgType = (mgmdCB->proto == L7_AF_INET) ? L7_IGMP_V2_MEMBERSHIP_REPORT :
                                               L7_MLD_V1_MEMBERSHIP_REPORT;

    }
    MCAST_PUT_BYTE(msgType, data); 
    MCAST_PUT_BYTE(zero, data);
    MCAST_PUT_SHORT(checksum, data);
    if (mgmdCB->proto == L7_AF_INET6)
    {
      MCAST_PUT_SHORT(maxRespTime, data);
      MCAST_PUT_SHORT(zero, data);
    }
    MCAST_PUT_ADDR_INET(mgmdCB->proto, &group, data);
    inetCopy(&destAddr, &mgmdCB->all_routers); 
  }
  else
  {
    /* Version3 Report -> We send only ONE Group Record per report */
    L7_ushort16          numGroupRecs = MGMD_ONE;
    L7_uint32            i;

    msgType = (mgmdCB->proto == L7_AF_INET) ? L7_IGMP_V3_MEMBERSHIP_REPORT :
                                              L7_MLD_V2_MEMBERSHIP_REPORT;

    MCAST_PUT_BYTE(msgType, data);
    MCAST_PUT_BYTE(zero, data);
    MCAST_PUT_SHORT(checksum, data);
    MCAST_PUT_SHORT(zero, data);
    MCAST_PUT_SHORT(numGroupRecs, data);


    if (grpRecType == 0)
    {
      if (mgmd_host_group != L7_NULLPTR)
      {
        grpRecType = (L7_uchar8) mgmd_host_group->filterMode;
      }
    }
    mgmdIntfReportStatsUpdate(mgmdCB,mgmd_host_info->rtrIntf,
                              grpRecType, MGMD_STATS_TX); 
   
    MCAST_PUT_BYTE(grpRecType, data);
    MCAST_PUT_BYTE(zero, data);
    MCAST_PUT_SHORT(numSrcs, data);
    MCAST_PUT_ADDR_INET(mgmdCB->proto, &group, data);

    /* Check for overflow */
    if (numSrcs > MGMD_MAX_QUERY_SOURCES)
      numSrcs = MGMD_MAX_QUERY_SOURCES;

    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," \n  numSrc = %d", numSrcs);
    for (i = 0; i < numSrcs; i++)
    {
      inetCopy(&srcAddr, (sourceSet[i]));
      MCAST_PUT_ADDR_INET(mgmdCB->proto, &srcAddr, data);
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"i - %d, srcAddr - %s.\n", i, inetAddrPrint(sourceSet[i],src));
    }
    dataLen = data - dataStart; /*IGMP_V3_PKT_MIN_LENGTH + sizeof(L7_inet_addr_t) + numSrcs*sizeof(L7_in_addr_t); */
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "\n dataLen = %d", dataLen);    
    inetIgmpv3RouterAddressInit(mgmdCB->proto, &destAddr) ; 
    MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS, "\n destAddr = ", &destAddr);    
  }

  inetAddressZeroSet(mgmdCB->proto, &ipAddr);
  rc = (mgmd_send (mgmdCB, destAddr, dataStart, dataLen, mgmd_host_info->rtrIntf, &ipAddr));
  if (rc == L7_SUCCESS)
  {
    if (mgmd_host_group != L7_NULLPTR)
    {
      inetCopy(&mgmd_host_group->last_reporter, &ipAddr);
    }
  }
  return rc;
}


/*****************************************************************
* @purpose Sends a v3/v2/v1 report for general query 
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  
* @end
*********************************************************************/
static L7_RC_t
mgmd_proxy_report_send (mgmd_cb_t *mgmdCB)
{
  mgmd_host_info_t  *mgmd_host_info = L7_NULLPTR;
  mgmd_host_group_t *mgmd_host_group = L7_NULLPTR;
  mgmd_host_group_t  dummyGroup;
  L7_uchar8 grpRecType = 0;
  L7_inet_addr_t *sourceSet[MGMD_MAX_QUERY_SOURCES];
  L7_ushort16 numSrcs = 0;
  L7_uchar8 grp[IPV6_DISP_ADDR_LEN];

  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Entry");

  if ((mgmd_host_info = mgmdCB->mgmd_host_info) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Find the Group Record Type to send */
  switch (mgmd_host_info->hostCompatibilityMode)
  {
    case L7_MGMD_VERSION_1:
      grpRecType = L7_IGMP_V1_MEMBERSHIP_REPORT;
      break;

    case L7_MGMD_VERSION_2:
      grpRecType = L7_IGMP_V2_MEMBERSHIP_REPORT;
      break;

    case L7_MGMD_VERSION_3:
      grpRecType = L7_IGMP_V3_MEMBERSHIP_REPORT;
      break;

    default:
      return L7_FAILURE;
  }

  /* Traverse the Host Membership Tree */
  memset(&dummyGroup, 0, sizeof(mgmd_host_group_t));
  mgmd_host_group = (mgmd_host_group_t*) avlSearchLVL7 (&mgmd_host_info->hostMembership,
                                         (void*) &dummyGroup, AVL_NEXT);
  while (mgmd_host_group != L7_NULLPTR)
  {
    if (mgmd_host_group->sendReportUponQuery == L7_TRUE)
    {
      mgmd_host_info->reportRateLimitCnt++;
    }
    else
    {
      /* Get next entry in the tree */
      mgmd_host_group = (mgmd_host_group_t*)
                        avlSearchLVL7 (&mgmd_host_info->hostMembership,
                                       (void*) mgmd_host_group, AVL_NEXT);
      continue;
    }

    if ((mgmd_host_info->reportRateLimitCnt) % (MGMD_PROXY_REPORT_RATE_LIMIT) == 0)
    {
      if (mgmdProxyUtilAppTimerSet (mgmdCB, L7_NULL, (void*)mgmdCB,
                                    &mgmd_host_info->gen_rsp_timer, 1,
                                    L7_MGMD_GENERAL_RSP_TIMER)
                                 != L7_SUCCESS)
      {
        MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_FAILURE, "Failed to Start GenRspTimer");
      }
      else
      {
        MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_FAILURE, "GenRspTimer Started for 1 Sec");
        return L7_SUCCESS;
      }
    }

    mgmd_host_group->hostState = MGMD_HOST_IDLE_MEMBER;

    if ((grpRecType == L7_IGMP_V1_MEMBERSHIP_REPORT) ||
        (grpRecType == L7_IGMP_V2_MEMBERSHIP_REPORT))
    {
      if (mgmd_host_group->grpSuppressed != L7_TRUE)
      {
        MGMD_DEBUG (MGMD_DEBUG_REPORTS, "Preparing to Send Report Message (%x); "
                    "Group - %s, numSrcs-%d", grpRecType,
                    inetAddrPrint(&mgmd_host_group->group,grp), numSrcs);
        if (mgmd_proxy_grp_report_send (mgmdCB, mgmd_host_group->group, numSrcs,
                                        L7_NULLPTR, L7_IGMP_MODE_IS_INCLUDE,
                                        mgmd_host_group)
                                     != L7_SUCCESS)
        {
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Report Message (%x) Send Failed",
                      grpRecType);
        }
      }
      mgmd_host_group->grpSuppressed = L7_FALSE;
    }
    else /* L7_IGMP_V3_MEMBERSHIP_REPORT */
    {
      mgmd_host_source_record_t *srcRec = L7_NULLPTR;
      L7_BOOL srcTmrFlag = L7_FALSE;
      L7_uint32 query_count = 0;
      L7_uint32 srcIndex = 0;

      for (query_count = 0; query_count < MGMD_MAX_QUERY_SOURCES; query_count++)
      {
        sourceSet[query_count] = L7_NULLPTR;
      }

      srcRec = L7_NULLPTR;
      numSrcs = 0;
      for (srcRec = (mgmd_host_source_record_t*) SLLFirstGet (&(mgmd_host_group->sourceRecords));
           srcRec != L7_NULLPTR; 
           srcRec = (mgmd_host_source_record_t*) SLLNextGet(&(mgmd_host_group->sourceRecords),
                                                            (L7_sll_member_t *)srcRec))
      {
        srcTmrFlag = mgmd_proxy_src_rec_timeout(srcRec);
        if ((mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE) ||
            ((mgmd_host_group->filterMode == MGMD_FILTER_MODE_EXCLUDE) && srcTmrFlag == L7_TRUE))
        {
          sourceSet[srcIndex] = &(srcRec->sourceAddress);
          srcIndex++;
          numSrcs++;
        }
      }

      MGMD_DEBUG (MGMD_DEBUG_REPORTS, "Preparing to Send Report Message (%x); "
                  "Group - %s, numSrcs-%d", grpRecType,
                  inetAddrPrint(&mgmd_host_group->group,grp), numSrcs);
      if (mgmd_proxy_grp_report_send (mgmdCB, mgmd_host_group->group, numSrcs,
                                      sourceSet, 0, mgmd_host_group)
                                   != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Report Message (%x) Send Failed",
                    grpRecType);
      }
    }

    mgmd_host_group->sendReportUponQuery = L7_FALSE;

    /* Get next entry in the tree */
    mgmd_host_group = (mgmd_host_group_t*) avlSearchLVL7 (&mgmd_host_info->hostMembership,
                                           (void*) mgmd_host_group, AVL_NEXT);
  }

  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}


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
void mgmd_proxy_v1v2report_process(mgmd_cb_t *mgmdCB, L7_inet_addr_t group, 
                                   L7_inet_addr_t source)
{
  mgmd_host_info_t   *mgmd_host_info = L7_NULL;
  mgmd_host_group_t  *mgmd_host_group = L7_NULL;
  L7_uint32 timeLeft = 0;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered ");

  mgmd_host_info = mgmdCB->mgmd_host_info;

  mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7 (&(mgmd_host_info->hostMembership), &group,
                                                        AVL_EXACT);


  if (mgmd_host_group == L7_NULL)
    return;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," HostCompatability = %d ",
                   mgmd_host_info->hostCompatibilityMode);

  if (mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_1)
  {
    MGMD_DEBUG(MGMD_PROXY_DEBUG_REPORTS, "V1 Report received.\n");
    if (appTimerTimeLeftGet (mgmdCB->timerHandle, &mgmd_host_info->gen_rsp_timer, &timeLeft) != L7_SUCCESS)
    {
      MGMD_DEBUG(MGMD_PROXY_DEBUG_FAILURE, "appTimerTimeLeftGet Failed for genRspTimer.\n");
    }
    MGMD_DEBUG(MGMD_PROXY_DEBUG_TIMERS, "timeLeft for genRspTimer - %d secs.\n", timeLeft);
    if (timeLeft > 0)
    {
      mgmd_host_group->grpSuppressed = L7_TRUE;
      MGMD_DEBUG(MGMD_PROXY_DEBUG_QUERY, "grpSuppressed Flag Set.\n");
    }
    if (mgmdProxyUtilAppTimerDelete(mgmdCB,&mgmd_host_group->grp_rsp_timer) != L7_SUCCESS)
    {
       MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not Delete the grp rsp timer node   \n");        
      return;
    }
    mgmd_host_group->hostState = MGMD_HOST_IDLE_MEMBER;
    mgmd_host_group->grpSuppressed = L7_TRUE;
  }
  else if (mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_2)
  {
    MGMD_DEBUG(MGMD_PROXY_DEBUG_REPORTS, "V2 Report received.\n");
    if (appTimerTimeLeftGet (mgmdCB->timerHandle, &mgmd_host_info->gen_rsp_timer, &timeLeft) != L7_SUCCESS)
    {
      MGMD_DEBUG(MGMD_PROXY_DEBUG_FAILURE, "appTimerTimeLeftGet Failed for genRspTimer.\n");
    }
    MGMD_DEBUG(MGMD_PROXY_DEBUG_TIMERS, "timeLeft for genRspTimer - %d secs.\n", timeLeft);
    if (timeLeft > 0)
    {
      mgmd_host_group->grpSuppressed = L7_TRUE;
      MGMD_DEBUG(MGMD_PROXY_DEBUG_QUERY, "grpSuppressed Flag Set.\n");
    }
    if (mgmdProxyUtilAppTimerDelete(mgmdCB,&mgmd_host_group->grp_rsp_timer) != L7_SUCCESS)
    {
       MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not Delete the grp rsp timer node   \n");        
      return;
    }
    mgmd_host_group->hostState = MGMD_HOST_IDLE_MEMBER;
    mgmd_host_group->grpSuppressed = L7_TRUE;
  }

  /* update the last reporter */
  inetCopy(&mgmd_host_group->last_reporter, &source);

}

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
void mgmd_proxy_intf_down_chk(mgmd_cb_t *mgmdCB, L7_uint32 rtrIfNum)
{
  mgmd_host_group_t *group_next;
  mgmd_host_group_t  dummyGroup;
  mgmd_host_group_t *mgmd_host_group = L7_NULL;
  mgmd_host_info_t  *mgmd_host_info = mgmdCB->mgmd_host_info;

  if (mgmd_host_info == L7_NULLPTR)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_FAILURE, " Host info is null ");
    return ;
  }
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered, rtrIfNum = %d, in-proxy = %d",
                   rtrIfNum , mgmd_host_info->rtrIntf);

  if (rtrIfNum != mgmd_host_info->rtrIntf)
  {
    /* When an igmp interface is down , the proxy table and MRT table 
       needs to be updated accordingly. All the group records registered 
       by the disabled interface in proxy table needs  to be updated,
       (if needed, the group record will be deleted) */
    /* Firstly, delete all MRT interfaces whose incoming interface is equal to
       disabled interface.
       Secondly, update the group state and SAT in proxy and then the MRT
       table acordingly.*/
    mgmd_group_t dummyGroup, *groupInfo;

    memset(&dummyGroup, 0, sizeof(mgmd_group_t));

    /* Parse through all the group entries learnt from disabled interface */

    while (L7_NULLPTR != (groupInfo = (mgmd_group_t *)avlSearchLVL7(&(mgmdCB->membership),
                                                                    (void *)(&dummyGroup), AVL_NEXT)))
    {
      memcpy(&dummyGroup, groupInfo, sizeof(mgmd_group_t));

      if (groupInfo->rtrIfNum != rtrIfNum)
      {
        continue;
      }
      mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7(&(mgmd_host_info->hostMembership),
                                                           (void *)(&groupInfo->group), AVL_EXACT);

      /* Updates the GPL n SPL n MRT Table for proxy*/
      if (mgmd_host_group != L7_NULLPTR)
      {
        mgmd_proxy_group_record_delete(mgmdCB, rtrIfNum, mgmd_host_group, L7_NULL, L7_NULLPTR);
      }
    }
    mgmdProxyMRTIntfDownHandler(mgmdCB, rtrIfNum);
    return;
  }
  else
  {
    /* when igmp-proxy interface is down , all the proxy dB 
       and MRT table needs to be cleaned */

    /* LOCK Proxy membership-table. */
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," disabling Proxy iface "); 
    if (osapiSemaTake(mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "Could not take the Semaphore.\n");
      return;
    }

    memset(&dummyGroup, 0, sizeof(mgmd_host_group_t));

    /* Get first entry in the tree */
    group_next = (mgmd_host_group_t *)avlSearchLVL7(&(mgmd_host_info->hostMembership),
                                                    (void *)(&dummyGroup), AVL_NEXT);

    while (group_next)
    {
      mgmd_host_group = group_next;
      /* Get the next entry */
      group_next =  (mgmd_host_group_t *)avlSearchLVL7 (&(mgmd_host_info->hostMembership),
                                                        group_next, AVL_NEXT);
      MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS," Found grp = ", &mgmd_host_group->group);
      if (mgmd_host_group)
      {
        MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Deleting grp ");
        if ((mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_2) ||
            (mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_3))
        {
          mgmd_proxy_grp_report_send(mgmdCB, mgmd_host_group->group, L7_NULL,
                                       L7_NULLPTR,L7_IGMP_CHANGE_TO_INCLUDE_MODE,mgmd_host_group);
        }
        if (mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_3 )
        {
          mgmd_proxy_unsolicited_timer_schedule(mgmdCB,mgmd_host_group->group, L7_NULL,
                                                L7_NULLPTR, L7_IGMP_CHANGE_TO_INCLUDE_MODE);
        }
        /* Delete the intfnum from the group info */
        mgmd_proxy_group_delete(mgmdCB, mgmd_host_group);
        avlDeleteEntry(&(mgmd_host_info->hostMembership), mgmd_host_group);
      }
    }/*End-of-While*/

    /* UNLOCK Proxy membership-table. */
    if (osapiSemaGive(mgmd_host_info->hostMembership.semId) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "Could not give the semaphore.\n");
      return;
    }
    mgmd_host_info->numOfGroups = avlTreeCount(&(mgmd_host_info->hostMembership));
    /* Delete all MRT table info */
    mgmdProxyMRTPurge(mgmdCB);
  }/*End-of-Else-Block*/
}

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
                                  mgmd_host_group_t **mgmd_host_group)
{
  mgmd_host_info_t  *mgmd_host_info;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (mgmd_host_info == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if ((*mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7 (&(mgmd_host_info->hostMembership), &group, 
                                                              AVL_EXACT)) == L7_NULL)
  {
    return L7_FAILURE;
  }
  else
  {
    return L7_SUCCESS;
  }
}

/*****************************************************************
* @purpose The function deletes a group record
*
* @param   mgmd_host_group  @b{ (input) }   MGMD Host Group
*
* @returns None
* @returns None
*
* @notes 
* @end
*********************************************************************/
static void mgmd_proxy_group_delete(mgmd_cb_t *mgmdCB, mgmd_host_group_t *mgmd_host_group)
{
 
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  if (mgmdProxyUtilAppTimerHandleDelete(mgmdCB,&mgmd_host_group->grp_rsp_timer,&mgmd_host_group->grp_rsp_timer_handle) != L7_SUCCESS)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not Delete the grp rsp timer node   \n");        
    return;
  }
  if (SLLDestroy(L7_FLEX_MGMD_MAP_COMPONENT_ID, &(mgmd_host_group->sourceRecords)) != L7_SUCCESS)
  {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "SLL not Destroyed.\n");
    return;
  }
}


/*****************************************************************
* @purpose The function checks the equality of the two given sources
*
* @param   p     @b{ (input) }   Pointer to data1
* @param   q     @b{ (input) }   Pointer to data2
* @param   key   @b{ (input) }   Key - Unused
*
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes 
* @end
*********************************************************************/
static L7_int32 mgmd_proxy_source_record_compare(void *p, void *q, L7_uint32 key)
{
  return(L7_INET_ADDR_COMPARE(&(((mgmd_host_source_record_t *)p)->sourceAddress), &(((mgmd_host_source_record_t *)q)->sourceAddress)));
}


/*****************************************************************
* @purpose The function deletes a source record
*
* @param   srcRec    @b{ (input) }   Source Record to be deleted
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes 
* @end
*********************************************************************/
static L7_RC_t mgmd_proxy_source_record_destroy(L7_sll_member_t *srcRec)
{
  mgmd_cb_t *mgmdCB = L7_NULLPTR;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  if (srcRec != L7_NULLPTR)
  {
    if ((mgmdCB = ((mgmd_host_source_record_t *)srcRec)->mgmdCB) != L7_NULLPTR)
    {
      MGMD_PROXY_FREE(mgmdCB->proto, (srcRec));  
    }
  }
  return L7_SUCCESS;
}



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
                                mgmd_host_source_record_t **srcRec)
{
  mgmd_host_source_record_t srcRecSrchKey;

  if (srcRecList == L7_NULL)
  {
    return L7_FAILURE;
  }

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  /* Search the required source in srcRecList. */
  memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
  inetCopy(&srcRecSrchKey.sourceAddress,&source);

  *srcRec = (mgmd_host_source_record_t *)SLLFind(srcRecList, (L7_sll_member_t *)&srcRecSrchKey);

  return L7_SUCCESS;
}


/*****************************************************************
* @purpose This function deletes the group record from the host info
*
* @param   mgmdCB            @b{ (input) }   MGMD Control Block
* @param   mgmd_host_group   @b{ (input) }   MGMD Host Group to be deleted
*
* @returns none
*
* @end
*********************************************************************/
static void mgmd_proxy_grp_rec_delete (mgmd_cb_t *mgmdCB, 
                                       mgmd_host_group_t *mgmd_host_group)
{
  L7_inet_addr_t  ipAddr;
  mgmd_host_info_t  *mgmd_host_info = mgmdCB->mgmd_host_info;

  if (mgmd_host_info == L7_NULLPTR)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_FAILURE, " Host info is null ");
    return ;
  }

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered, host mode = %d ", 
                   mgmd_host_info->hostCompatibilityMode);

  /* Send Leave message Upstream. */
  switch (mgmd_host_info->hostCompatibilityMode)
  {
    case L7_MGMD_VERSION_2:
      {
        if (mcastIpMapIpAddressGet(mgmdCB->proto, mgmd_host_info->rtrIntf, &ipAddr) != L7_SUCCESS)
        {
            MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "Could not Map the Rtr Inter face to interface number \n");
          break;
        }

        if (L7_INET_IS_ADDR_EQUAL(&(mgmd_host_group->last_reporter), &ipAddr))
        {
          MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," I am  the last reporter ");
          /* Send leave as INCLUDE(NULL)  */
          mgmd_proxy_grp_report_send(mgmdCB, mgmd_host_group->group, L7_NULL,
                                     L7_NULLPTR,L7_IGMP_CHANGE_TO_INCLUDE_MODE,mgmd_host_group);
        }

      }          
     break;
    case L7_MGMD_VERSION_3:
      {
        /* Send the LEAVE report upstream */
        MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS," Last reporter = ",&(mgmd_host_group->last_reporter));
        if (mgmd_proxy_grp_report_send(mgmdCB, mgmd_host_group->group, 
                                       L7_NULL, L7_NULLPTR,L7_IGMP_CHANGE_TO_INCLUDE_MODE,
                                       mgmd_host_group) ==L7_FAILURE)
        {
           MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Message Not Sent \n");
        }
      }
      mgmd_proxy_unsolicited_timer_schedule(mgmdCB,mgmd_host_group->group, L7_NULL,
                                              L7_NULLPTR, L7_IGMP_CHANGE_TO_INCLUDE_MODE);
      break;
    case L7_MGMD_VERSION_1:

    default: 
      break;
  }/*End-of-Switch*/

  MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS,
                        " Updating MRT table n deleting grp entry for grp =",
                        &mgmd_host_group->group);
  /* Update Proxy MRT table when group record is deleted */
  mgmdProxyMRTGroupRecordDeleteHandler(mgmdCB, mgmd_host_group->group);
  mgmdProxyUtilAppTimerHandleDelete(mgmdCB, &mgmd_host_group->grp_rsp_timer, &mgmd_host_group->grp_rsp_timer_handle);
  if (osapiSemaTake(mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Could not take the Semaphore.\n");
    return;
  }
  avlDeleteEntry(&(mgmd_host_info->hostMembership), mgmd_host_group);
  osapiSemaGive(mgmd_host_info->hostMembership.semId);
} 


/*****************************************************************
* @purpose This function adds the source record to the host info
*
* @param   mgmdCB            @b{ (input) }   MGMD Control Block
* @param   mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param   srcRec            @b{ (input) }   Source Record
*
* @returns none
*
* @notes  None
*
* @end
*********************************************************************/
static void mgmd_proxy_src_rec_add (mgmd_cb_t *mgmdCB, 
                                    mgmd_host_group_t *mgmd_host_group, 
                                    mgmd_host_source_record_t *srcRec)
{
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  if (SLLAdd (&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)srcRec) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "Could not add the Source Node  to source records List\n");
    return;
  }

  /* Call the cache update function to reflect the changes in the 
     SPL */
  mgmdProxyMRTUpdate(mgmdCB, &srcRec->sourceAddress, &mgmd_host_group->group);
} 


/*****************************************************************
* @purpose This function deletes the source record from the group Record
*
* @param   mgmdCB            @b{ (input) }   MGMD Control Block
* @param   mgmd_host_group   @b{ (input) }   MGMD Host Group pointer
* @param   srcRec            @b{ (input) }   Source Record to be deleted
*
* @returns none
*
* @notes   None
*
* @end
*********************************************************************/
static void mgmd_proxy_src_rec_delete (mgmd_cb_t *mgmdCB,
                                       mgmd_host_group_t *mgmd_host_group, 
                                       mgmd_host_source_record_t *srcRec)
{
  MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS,"Entered, src = ", &srcRec->sourceAddress);

  if (SLLDelete(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)srcRec) != L7_SUCCESS)
  {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE,   " Could not Delete the Source node \n");
    return;
  }

  /* Call the cache update function to reflect the changes in the 
     SPL */
  mgmdProxyMRTUpdate(mgmdCB, &srcRec->sourceAddress, &mgmd_host_group->group);
} 


/*****************************************************************
* @purpose This function deletes the port from the SPL
*
* @param   mgmdCB            @b{ (input) }   MGMD Control Block
* @param   mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param   srcRec            @b{ (input) }   Source Record
* @param   rtrIntf           @b{ (input) }   The interface number to be deleted from the SPL
*
* @returns none

* @notes  None
*
* @end
*********************************************************************/
static void mgmd_proxy_SPL_portno_delete (mgmd_cb_t *mgmdCB,
                                          mgmd_host_group_t *mgmd_host_group,
                                          mgmd_host_source_record_t *srcRec, 
                                          L7_uint32 rtrIntf)
{
  L7_BOOL flag;
  
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered, rtrIfNum = %d", rtrIntf);

  if (BITX_TEST(&srcRec->srcPortList, rtrIntf))
  {
    flag = mgmd_proxy_portlist_status(&srcRec->srcPortList);
    BITX_RESET(&srcRec->srcPortList, rtrIntf);
    if (flag != mgmd_proxy_portlist_status(&srcRec->srcPortList))
    {
      if (flag != L7_FALSE)
      {
        /* SPL - NON NULL to NULL */
        mgmd_proxy_block_send(mgmdCB, mgmd_host_group,srcRec);
      }
    }

    /* Call the cache update function to reflect the changes in the SPL */
    mgmdProxyMRTUpdate(mgmdCB, &srcRec->sourceAddress, &mgmd_host_group->group);
  }/*End-of-IF-Block*/

} 


/*****************************************************************
* @purpose This function adds the port to the SPL 
*
* @param   mgmdCB            @b{ (input) }   MGMD Control Block
* @param   mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param   srcRec            @b{ (input) }   Source Record
* @param   rtrIntf           @b{ (input) }   The interface number to be added to the SPL
*
* @returns none
*
* @notes  None
*
* @end
*********************************************************************/
static void mgmd_proxy_SPL_portno_add (mgmd_cb_t *mgmdCB,
                                       mgmd_host_group_t *mgmd_host_group,
                                       mgmd_host_source_record_t *srcRec, 
                                       L7_uint32 rtrIntf)
{
  L7_BOOL flag;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  if (!BITX_TEST(&srcRec->srcPortList, rtrIntf))
  {
    flag = mgmd_proxy_portlist_status(&srcRec->srcPortList);
    BITX_SET(&srcRec->srcPortList, rtrIntf);
    if (flag != mgmd_proxy_portlist_status(&srcRec->srcPortList))
    {
      if (flag == L7_FALSE)
      {
        if ( ((mgmd_host_group->filterMode == MGMD_FILTER_MODE_EXCLUDE) &&
              (srcRec->createFlag == L7_FALSE) &&
              (mgmd_proxy_src_rec_timeout(srcRec) == L7_FALSE)) 
            || ((mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE) &&
                (srcRec->createFlag == L7_TRUE) ))
        {
          /* SPL -- NULL to NON NULL */
          MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n Allow send ");
          mgmd_proxy_allow_send(mgmdCB, mgmd_host_group, srcRec);
        }
      }
      else
      {
        /* SPL - NON NULL to NULL */
        MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n Block send ");
        mgmd_proxy_block_send(mgmdCB, mgmd_host_group,srcRec);
      }
    }

    /* Set the Create Flag of the srcRec. */
    srcRec->createFlag = L7_FALSE;

    /* Call the cache update function to reflect the changes in the SPL */
    mgmdProxyMRTUpdate(mgmdCB, &srcRec->sourceAddress, &mgmd_host_group->group);

  }/*End-of-Main-If-Block*/
} 

/*****************************************************************
* @purpose This function deletes the Port from the GPL
*
* @param   mgmdCB            @b{ (input) }   MGMD Control Block
* @param   mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param   rtrIntf           @b{ (input) }   The interface number to be added to the SPL
*
* @returns none
*
* @notes  None
*
* @end
*********************************************************************/
static void mgmd_proxy_GPL_portno_delete (mgmd_cb_t *mgmdCB, 
                                          mgmd_host_group_t *mgmd_host_group, 
                                          L7_uint32 rtrIntf)
{
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered, port = %d ", rtrIntf);

  if (BITX_TEST(&mgmd_host_group->grpPortList, rtrIntf))
  {
    BITX_RESET(&mgmd_host_group->grpPortList, rtrIntf);
    /* Call the cache update function to reflect the changes in the SPL */
    mgmdProxyMRTUpdate(mgmdCB, L7_NULL, &mgmd_host_group->group);
  }/*End-of-If-Block*/
} 


/*****************************************************************
* @purpose This function Adds the Port to the GPL
*
* @param   mgmdCB            @b{ (input) }   MGMD Control Block
* @param   mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param   rtrIntf           @b{ (input) }   The interface number to be added to the SPL
*
* @returns none
*
*
* @notes  None
*
* @end
*********************************************************************/
static void mgmd_proxy_GPL_portno_add (mgmd_cb_t *mgmdCB,
                                       mgmd_host_group_t *mgmd_host_group, 
                                       L7_uint32 rtrIntf)
{
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  if (!BITX_TEST(&mgmd_host_group->grpPortList, rtrIntf))
  {
    BITX_SET(&mgmd_host_group->grpPortList, rtrIntf);
    /* Call the cache update function to reflect the changes in the SPL */
    mgmdProxyMRTUpdate(mgmdCB, L7_NULL, &mgmd_host_group->group);
  }/*End-of-If-Block*/
} 

/*****************************************************************
* @purpose This function copies the GPL list to the SPL
*
* @param   mgmdCB            @b{ (input) }   MGMD Control Block
* @param   mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param   srcrec            @b{ (input) }   Source Record
*
* @returns none
*
* @returns  None
*
* @notes  None
*
* @end
*********************************************************************/
static void mgmd_proxy_GPL_to_SPL_copy(mgmd_cb_t *mgmdCB, 
                                       mgmd_host_group_t *mgmd_host_group,
                                       mgmd_host_source_record_t *srcRec)
{
  L7_BOOL flag,flag1;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  /* 1. Set the Create Flag of the srcRec. */
/*    srcRec->createFlag = L7_TRUE; */

  /* 2. If (GPL != SPL), Then Perform the Copy GPL to SPL function . */
  if (mgmd_proxy_bitset_compare(&srcRec->srcPortList, 
                                &mgmd_host_group->grpPortList) == L7_FALSE)
  {
    flag = mgmd_proxy_portlist_status(&srcRec->srcPortList);
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  SPL Flag = %d  ",flag);
    flag1 = mgmd_proxy_portlist_status(&mgmd_host_group->grpPortList);
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  GPL Flag = %d  ",flag1);

    mgmd_proxy_bitset_copy(&srcRec->srcPortList, &mgmd_host_group->grpPortList);
    if (flag != mgmd_proxy_portlist_status(&srcRec->srcPortList))
    {
      if (flag == L7_FALSE)
      {
        /* SPL -- NULL to NON NULL */
        if ( ((mgmd_host_group->filterMode == MGMD_FILTER_MODE_EXCLUDE) &&
              (srcRec->createFlag == L7_FALSE) &&
              (mgmd_proxy_src_rec_timeout(srcRec) == L7_FALSE)) 
            || ((mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE) &&
                (srcRec->createFlag == L7_TRUE) ))
        {
          mgmd_proxy_allow_send(mgmdCB, mgmd_host_group, srcRec);
        }
        
      }
      else
      {
        /* SPL - NON NULL to NULL */
        mgmd_proxy_block_send(mgmdCB, mgmd_host_group,srcRec);
      }
    }

    /* Set the Create Flag of the srcRec. */
    srcRec->createFlag = L7_FALSE;

    /* Call the cache update function to reflect the changes in the SPL */
    mgmdProxyMRTUpdate(mgmdCB, &srcRec->sourceAddress, &mgmd_host_group->group);
  }/*End-of-Main-If-Block*/
  /*
  else
  {
    srcRec->createFlag = L7_FALSE;
  }       */

} 


/*****************************************************************
* @purpose This function gets the status of the portlist
*
* @param   bitset   @b{ (input) }  The interface bitset pointer.
*
* @returns L7_TRUE   if the SPL is not NULL 
* @returns L7_FALSE  if the SPL is NULL 
*
* @notes  
*
* @end
*********************************************************************/
static L7_BOOL mgmd_proxy_portlist_status(interface_bitset_t *bitset)
{
  interface_bitset_t          nullPortList;
  memset(&nullPortList, 0, sizeof (interface_bitset_t));

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered ");


  if (mgmd_proxy_bitset_compare(&nullPortList, bitset) == L7_TRUE)
  {
    return L7_FALSE;
  }
  else
  {
    return L7_TRUE;
  }
}


/*****************************************************************
* @purpose This function send ALLOW report due to addition of new sources.
*
* @param   mgmdCB            @b{ (input) }   MGMD Control Block
* @param   mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param   srcrec            @b{ (input) }   Source Record
*
* @returns none
*
* @notes  
*
* @end
*********************************************************************/
static void mgmd_proxy_allow_send (mgmd_cb_t                 *mgmdCB,
                                   mgmd_host_group_t         *mgmd_host_group,
                                   mgmd_host_source_record_t *srcRec)
{
  L7_BOOL       flag;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered ");
  
  flag = mgmd_proxy_portlist_status(&mgmd_host_group->grpPortList);

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  GPL Flag = %d",flag);
  MGMD_PROXY_DEBUG_BITS(MGMD_PROXY_DEBUG_APIS,"GPL = ",&mgmd_host_group->grpPortList);
  /* Check for the Filter mode and the GPL list */
  if ((mgmd_host_group->filterMode == MGMD_FILTER_MODE_EXCLUDE && flag!=0) ||
      (mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE && flag==0))
  {
    /* Update the Last Reporter */

    if (mgmdCB->mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_3)
    {
      /* Send the ALLOW(S) report upstream */
      L7_inet_addr_t *pSrcAddr = &srcRec->sourceAddress;
      if (mgmd_proxy_grp_report_send(mgmdCB, mgmd_host_group->group, MGMD_ONE, 
                                     &pSrcAddr, L7_IGMP_ALLOW_NEW_SOURCES,
                                     mgmd_host_group) ==L7_FAILURE)
      {
        MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Message Not Sent \n");
        return;        
      }
      /* Schedule the Unsolicited timer for resending this report. */
      mgmd_proxy_unsolicited_timer_schedule(mgmdCB, mgmd_host_group->group, 1, 
                                            &pSrcAddr,
                                            L7_IGMP_ALLOW_NEW_SOURCES);
    }
  }
}

/*****************************************************************
* @purpose This function send BLOCK report due to deletion of sources.
*
* @param   mgmdCB            @b{ (input) }   MGMD Control Block
* @param   mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param   srcrec            @b{ (input) }   Source Record
*
* @returns none
*
*
* @notes  
*
* @end
*********************************************************************/
static void mgmd_proxy_block_send(mgmd_cb_t                 *mgmdCB,
                                  mgmd_host_group_t         *mgmd_host_group,
                                  mgmd_host_source_record_t *srcRec)
{
  L7_BOOL       flag;
  
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered ");

  flag = mgmd_proxy_portlist_status(&mgmd_host_group->grpPortList);

  /* Check for the Filter mode and the GPL list */
  if ((mgmd_host_group->filterMode == MGMD_FILTER_MODE_EXCLUDE && flag!=0) ||
      (mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE && flag==0))
  {

    if (mgmdCB->mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_3)
    {
      L7_inet_addr_t *pSrcAddr = &srcRec->sourceAddress;
      /* Send the BLOCK(S) report upstream */
      if (mgmd_proxy_grp_report_send(mgmdCB, mgmd_host_group->group, MGMD_ONE, 
                                     &pSrcAddr, L7_IGMP_BLOCK_OLD_SOURCES,
                                     mgmd_host_group) ==L7_FAILURE)
      {
         MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Message Not Sent \n");
        return;        
      }

      /* Schedule the Unsolicited timer for resending this report. */
      mgmd_proxy_unsolicited_timer_schedule(mgmdCB, mgmd_host_group->group, 1, 
                                            &pSrcAddr,
                                            L7_IGMP_BLOCK_OLD_SOURCES);
    }
  }
}


/*****************************************************************
* @purpose This function send TO_IN report due to change of filter mode
*
* @param   mgmdCB            @b{ (input) }   MGMD Control Block
* @param   mgmd_host_group   @b{ (input) }   MGMD Host Group
*
* @returns none
*
*
* @notes  
*
* @end
*********************************************************************/
static void mgmd_proxy_to_incl_send(mgmd_cb_t         *mgmdCB,
                                    mgmd_host_group_t *mgmd_host_group)
{
  L7_inet_addr_t            *newSrcList[MGMD_MAX_QUERY_SOURCES];
  L7_uint32                  newSrcsCount = 0;
  mgmd_host_source_record_t *hostSrcRec;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered ");

  for (hostSrcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); hostSrcRec != L7_NULL; 
      hostSrcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)hostSrcRec))
  {
    MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS," Found source = ", &hostSrcRec->sourceAddress);
    if (mgmd_proxy_portlist_status(&hostSrcRec->srcPortList) != L7_FALSE)
    {
      newSrcList[newSrcsCount] = &(hostSrcRec->sourceAddress);
      newSrcsCount++;                  
    }
  }/*End-of-for*/

  if (mgmdCB->mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_3)
  {
    /* Send the TO_INCL(S) report upstream */
    if (mgmd_proxy_grp_report_send(mgmdCB, mgmd_host_group->group, 
                                   newSrcsCount, newSrcList,L7_IGMP_CHANGE_TO_INCLUDE_MODE,
                                   mgmd_host_group) == L7_FAILURE)
    {
       MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Message Not Sent \n");
      return;     
    }
    /* Schedule the Unsolicited timer for resending this report. */
    mgmd_proxy_unsolicited_timer_schedule(mgmdCB, mgmd_host_group->group, newSrcsCount,
                                          newSrcList,L7_IGMP_CHANGE_TO_INCLUDE_MODE);
  }
}


/*****************************************************************
* @purpose This function send TO_EX report due to change of filter mode
*
* @param   mgmdCB            @b{ (input) }   MGMD Control Block
* @param   mgmd_host_group   @b{ (input) }   MGMD Host Group
*
* @returns none
*
*
* @notes  
*
* @end
*********************************************************************/
static void mgmd_proxy_to_excl_send(mgmd_cb_t *mgmdCB,
                                    mgmd_host_group_t *mgmd_host_group)
{
  L7_inet_addr_t            *newSrcList[MGMD_MAX_QUERY_SOURCES];
  L7_uint32                  newSrcsCount = 0;
  mgmd_host_source_record_t *srcRec;
  
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  /* Create set A =Set of all Sources with (SPL==NULL). */
  for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
      srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)srcRec))
  {
    if (mgmd_proxy_portlist_status(&srcRec->srcPortList) == L7_FALSE)
    {
      newSrcList[newSrcsCount] = &(srcRec->sourceAddress);
      newSrcsCount++;                  
    }
  }/*End-of-for*/

  if (newSrcsCount)
  {
    if (mgmdCB->mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_3)
    {
      /* Send the TO_EX(A) report upstream */
      if (mgmd_proxy_grp_report_send(mgmdCB, mgmd_host_group->group, 
                                     newSrcsCount, newSrcList,L7_IGMP_CHANGE_TO_EXCLUDE_MODE,
                                     mgmd_host_group) ==L7_FAILURE)
      {
         MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Message Not Sent \n");
        return;        
      }
      /* Schedule the Unsolicited timer for resending this report. */
      mgmd_proxy_unsolicited_timer_schedule(mgmdCB, mgmd_host_group->group, newSrcsCount,
                                            newSrcList,L7_IGMP_CHANGE_TO_EXCLUDE_MODE);
    }
  }
}


/*****************************************************************
* @purpose This function schedules a unsolicit report timer
*
* @param   mgmdCB            @b{ (input) }   MGMD Control Block
* @param   mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param   numSrcs           @b{ (input) }   Number of sources
* @param   RptSrcList        @b{ (input) }   Report source list
*
* @returns none
*
* @notes  
*
* @end
*********************************************************************/
static void mgmd_proxy_unsolicited_timer_schedule(mgmd_cb_t *mgmdCB,
                                                  L7_inet_addr_t group,
                                                  L7_uint32          numSrcs,
                                                  L7_inet_addr_t   **RptSrcList,
                                                  L7_uint32          grpRecType)
{
  static L7_uint32               unsolicitedRptCount = 0; /* Counter used for 
                                 assigning unique IDs to unsolicited Reports.*/
  mgmd_proxy_unsolicited_rpt_t  *rreq = L7_NULLPTR;
  L7_uint32                      index;
  L7_uint32                      robustnessvar, unsolicitedReportInterval;
  mgmd_host_info_t  *mgmd_host_info = mgmdCB->mgmd_host_info;

  if (mgmd_host_info == L7_NULLPTR)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_FAILURE, " Host info is null ");
    return ;
  }

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  robustnessvar = mgmd_host_info->robustness;
  unsolicitedReportInterval = mgmd_host_info->unsolicitedReportInterval;

  /* This timer shouldn't be scheduled if RobustnessVariable value is 0.*/
  if (robustnessvar <= MGMD_ONE)
  {
    return;
  }

  if ((rreq = (mgmd_proxy_unsolicited_rpt_t *) MGMD_PROXY_ALLOC(mgmdCB->proto,
                                                                sizeof(mgmd_proxy_unsolicited_rpt_t)))
      == L7_NULLPTR)
  {
     MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "mgmd_proxy_unsolicited_timer_schedule: Error getting memory \
                for source record\n");
    return;
  }

  memset(rreq, 0, sizeof(mgmd_proxy_unsolicited_rpt_t));
  rreq->reportId = (++unsolicitedRptCount);

  /* group prefix has to be preserved until all reports are sent */
  rreq->group = group;
  rreq->mgmdCB = mgmdCB;
  rreq->retryCount = robustnessvar-1;
  rreq->grpRecType = grpRecType;
  memset(rreq->srcList, 0, sizeof(rreq->srcList));
  rreq->numSrcs = 0;

  for (index = 0; index < numSrcs; index++)
  {
    inetCopy(&rreq->srcList[index], RptSrcList[index]);
    rreq->numSrcs++;
  }

  if (SLLAdd(&(mgmd_host_info->ll_unsolicited_reports), (L7_sll_member_t *)rreq) != L7_SUCCESS)
  {
     MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not add the unsolicited report Node to the report List\n");
    MGMD_PROXY_FREE(mgmdCB->proto, (rreq));       
    return;
  }

  if ((rreq->timer_handle  = handleListNodeStore(mgmdCB->handle_list,rreq)) == 0)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE,   "Could not get the handle node to store the timer data.\n");                    
    MGMD_PROXY_FREE(mgmdCB->proto, (rreq));       
    return;
  }
  if (mgmdProxyUtilAppTimerSet(mgmdCB, L7_NULL, 
                               (void*)rreq->timer_handle,
                               &rreq->timer,
                               unsolicitedReportInterval, 
                               L7_MGMD_UNSOLICITED_REPORT_TIMER) != L7_SUCCESS)
  {
     MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not start the unsolicited report timer  \n");
    MGMD_PROXY_FREE(mgmdCB->proto, (rreq));       
    return;
  }
}


/*****************************************************************
* @purpose  Checks equality of two given PortLists.
*
* @param   mask1           @b{ (input) }   Bit Mask 1
* @param   mask2           @b{ (input) }   Bit Mask 2
*
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes  
* @end
*********************************************************************/
static L7_BOOL mgmd_proxy_bitset_compare(interface_bitset_t *mask1, 
                                         interface_bitset_t *mask2) 
{
  L7_ushort16 index = 0;
  L7_uint32 maxBits = MCAST_BITX_NUM_BITS(MAX_INTERFACES);


  for (index=0; index<maxBits; index++)
    if (mask1->bits[index] != mask2->bits[index])
      break;

  if (index == maxBits)
    return(L7_TRUE);
  else
    return(L7_FALSE);


}


/*****************************************************************
* @purpose  Copies the portlist to the dest from src
*
* @param   dest         @b{ (output) }  Dest Bit Mask
* @param   src          @b{ (input) }   Src Bit Mask
*
* @returns none
*
* @notes  
* @end
*********************************************************************/
static void mgmd_proxy_bitset_copy(interface_bitset_t *dest, 
                                   interface_bitset_t *src)
{
  BITX_COPY(src, dest);
  return;
}


/***************************************************************
* @purpose  Gets the count of the bitset
*
* @param    interface_mask  @b{ (input) } bitset to get the count from
*
* @returns  the count.
*
* @notes    this function is optimized to return the count as soon
*           as the count is more than 1.
* @end
*********************************************************************/
L7_uint32 mgmd_proxy_get_bitset_count(interface_bitset_t *interface_mask)
{
  L7_uint32 count =0;     
  L7_uint32 index =0;
  L7_uint32 maxBits = MCAST_BITX_NUM_BITS(MAX_INTERFACES);

  for (index=0; index<maxBits; index++)
  {
    if (BITX_TEST (interface_mask, index))
    {
      count++;
      if (count > 1)
        break;
    }
  }   

  return(count);
}


/*****************************************************************
* @purpose  calculates the selected delay from the max response time
*
* @param    max_resp_time @b{ (input) } the maximum response time
*
* @returns  the selected delay for a
   response is randomly selected in the range (0, [Max Resp Time]) where
   Max Resp Time is derived from Max Resp Code in the received Query
   message.
* @notes  
*

* @end
*********************************************************************/

static L7_int32 mgmd_proxy_selected_delay_calculate (L7_int32 max_resp_time)
{
  L7_int32 selectedDelay = MGMD_ZERO;
  extern L7_double64 L7_Random(void);
  L7_double64 randval;
  randval =  L7_Random();
  selectedDelay= max_resp_time *randval;
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_REPORTS,
    "\n selectedDelay %d  randval %e",selectedDelay, randval);
  /* report suppression override*/
  if (selectedDelay == 0)
    selectedDelay = 1;
 
  return (selectedDelay);
}


/**********************************************************************
* @purpose  This is the ExcludeAdd() that updates the SPL and GPL  
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    ifNum             @b{ (input) }   Interface Number
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    srcsCount         @b{ (input) }   Source Count
* @param    srcList           @b{ (input) }   Source List
*
* @returns  None
*
* @notes  
* @end
*********************************************************************/
static void mgmd_proxy_ExcludeAdd1(mgmd_cb_t *mgmdCB, L7_uint32 ifNum, 
                                   mgmd_host_group_t *mgmd_host_group,
                                   L7_uint32 srcsCount, L7_inet_addr_t *srcList)
{
  mgmd_host_source_record_t *srcRec = L7_NULL;
  L7_uint32                  index;
  L7_uchar8                  addrBuff[IPV6_DISP_ADDR_LEN];

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  if (mgmd_host_group->sourceRecords.sllNumElements == 0)
  {
    return;
  }

  MGMD_PROXY_DEBUG_BITS(MGMD_PROXY_DEBUG_APIS," GPL = ",&mgmd_host_group->grpPortList);
  
  /* For all the sources present in apiSrcList. */
  for (index=0; index < srcsCount; index++)
  {

    mgmd_host_source_record_t srcRecord;
    memset(&srcRecord, 0, sizeof(mgmd_host_source_record_t));
    srcRecord.sourceAddress = srcList[index]; 

    if ((srcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)&srcRecord)))
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  SRC = %s ",
                       inetAddrPrint(&srcList[index],addrBuff));
      MGMD_PROXY_DEBUG_BITS(MGMD_PROXY_DEBUG_APIS," SPL = ",&srcRec->srcPortList);

      if (srcRec->createFlag == L7_FALSE)
      {
        /*If source record was already present, then add port ifNum to SPL.*/
        mgmd_proxy_SPL_portno_add (mgmdCB,mgmd_host_group, srcRec, ifNum);
      }
      else
      { /* Source Record was added newly in calling routine. */
        /* ...Copy GPL to SPL. */
        mgmd_proxy_GPL_to_SPL_copy(mgmdCB, mgmd_host_group, srcRec);

        /* If the source record is present, add the port ifNum to SPL. */
        mgmd_proxy_SPL_portno_add (mgmdCB, mgmd_host_group, srcRec, ifNum);
      }

    }
  }/*End-of-FOR-LOOP*/

}


/**********************************************************************
* @purpose  This is the ExcludeAdd() that updates the SPL and GPL  
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    ifNum             @b{ (input) }   Interface Number
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    YminusBsrcsCount  @b{ (input) }   Source Count
* @param    YminusBsrcList    @b{ (input) }   Source List
* @param    BminusYsrcsCount  @b{ (input) }   Source Count
* @param    BminusYsrcList    @b{ (input) }   Source List
*
* @returns  None
*
* @notes  
* @end
*********************************************************************/
static void mgmd_proxy_ExcludeAdd2 (mgmd_cb_t *mgmdCB,
                                    L7_uint32 ifNum,
                                    mgmd_host_group_t *mgmd_host_group,
                                    L7_uint32 YminusBsrcsCount, 
                                    L7_inet_addr_t *YminusBsrcList,
                                    L7_uint32 BminusYsrcsCount, 
                                    L7_inet_addr_t *BminusYsrcList)
{
  mgmd_host_source_record_t *srcRec = L7_NULL, srcRecSrchKey;
  L7_uint32                  index;
  
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered ");

  /* For all the sources present in YminusBsrcList. */
  for (index=0; index < YminusBsrcsCount; index++)
  {
    memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
    inetCopy(&srcRecSrchKey.sourceAddress, &YminusBsrcList[index]);
    MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS," Given addr : ", &srcRecSrchKey.sourceAddress);
    if ((srcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)&srcRecSrchKey)))
    {
      /* Add PortNo to SPL. */
      MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS," Port added for src: ", &srcRec->sourceAddress);
      mgmd_proxy_SPL_portno_add (mgmdCB, mgmd_host_group, srcRec, ifNum);
    }
  }/*End-of-FOR-LOOP*/

  /* 2. For all the sources present in BminusYsrcList. */
  for (index=0; index < BminusYsrcsCount; index++)
  {
    memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
    inetCopy(&srcRecSrchKey.sourceAddress, &BminusYsrcList[index]);  
    if ((srcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)&srcRecSrchKey)))
    {
      /* Copy GPL to SPL. */
      mgmd_proxy_GPL_to_SPL_copy(mgmdCB, mgmd_host_group, srcRec);
    }
  }/*End-of-FOR-LOOP*/

  /* 3. Add PortNo to GPL of group record G. */
  mgmd_proxy_GPL_portno_add (mgmdCB, mgmd_host_group, ifNum);

}


/**********************************************************************
* @purpose  This is the mgmd_proxy_includeToExclude1() that updates the SPL and GPL  
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    ifNum             @b{ (input) }   Interface Number
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    srcsCount         @b{ (input) }   Source Count
* @param    srcList           @b{ (input) }   Source List
*
* @returns  None
*
* @notes     Include(A) + IS_EXCLUDE(B) 
* @end
*********************************************************************/
static void mgmd_proxy_includeToExclude_new (mgmd_cb_t *mgmdCB,
                                             L7_uint32 ifNum,
                                             mgmd_host_group_t *mgmd_host_group,
                                             L7_uint32 srcCount, 
                                             L7_inet_addr_t *srcList)
{
  mgmd_host_source_record_t *newSrcRec = L7_NULL, *srcRec = L7_NULL;
  L7_uint32                  index;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered ");

 if ((srcCount == 0) && (mgmdCB->mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_3))
 {
   /* rx'ed V2 join downstream");
   so send a IS_EXC(NULL) report upstream */

   MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Rx'ed v2 join dnstream ");
   mgmd_proxy_grp_report_send(mgmdCB,
                              mgmd_host_group->group, L7_NULL, L7_NULLPTR, 
                              L7_IGMP_CHANGE_TO_EXCLUDE_MODE,mgmd_host_group);

   mgmd_proxy_unsolicited_timer_schedule(mgmdCB,mgmd_host_group->group, L7_NULL, L7_NULLPTR,
                                         L7_IGMP_CHANGE_TO_EXCLUDE_MODE);
 }


  /* 1. for all B - A  copy GPL to SPL */
  for (index=0; index < srcCount; index++)
  {
    mgmd_host_source_record_t srcRecord;
    memset(&srcRecord, 0, sizeof(mgmd_host_source_record_t));
    srcRecord.sourceAddress = srcList[index];

    if ((srcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)&srcRecord)))
    {
      continue; 
    }
    if (srcRec == L7_NULL)
    {
      /* add the src record and copy the GPL to SPL */
      mgmd_proxy_util_new_src_record_create (mgmdCB, srcList[index], &newSrcRec);
      if (newSrcRec == L7_NULL)
      {
        /* Return as Memory allocation failed. */
         MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not create the source Node \n");                
        return;
      }
      /* B-A = 0 timeout */
      newSrcRec->sourceInterval = 0;
      newSrcRec->createFlag = L7_FALSE;
      mgmd_proxy_src_rec_add (mgmdCB, mgmd_host_group, newSrcRec);
      mgmd_proxy_GPL_to_SPL_copy(mgmdCB, mgmd_host_group,newSrcRec);

    }
  }/*End-of-FOR-LOOP*/


  /* 2. For all A-B Add port to SPL */
  for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
      srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)srcRec))
  {
    for (index=0; index < srcCount; index++)
    {
      if (L7_INET_IS_ADDR_EQUAL(&(srcRec->sourceAddress), &srcList[index]))
      {
        break;
      }
    }
    if (index == srcCount)
    {
      /* A - B */  /* add port to SPL */
      mgmd_proxy_SPL_portno_add(mgmdCB, mgmd_host_group,srcRec,ifNum);
    }
    #ifdef DEFECT_FIX /* fix for bug 43717 */
    else
    {
      /* A * B */   /* Remove port from SPL */
      mgmd_proxy_SPL_portno_delete(mgmdCB, mgmd_host_group,srcRec,ifNum);
    }
    #endif
  }
  /* 3. Add PortNo to GPL of group record G. */
  mgmd_proxy_GPL_portno_add (mgmdCB, mgmd_host_group, ifNum);

  /*4. Change filter to exclude */
  mgmd_host_group->filterMode = MGMD_FILTER_MODE_EXCLUDE;

}


/********************************************************************
* @purpose  Invoked when SRC timer expiry event from downstream MGMD router. 
*
* @param    param     @b { (input) }  MGMD Control Block
*
* @returns  None
*
* @notes   No source timer is maintained at proxy level. 
*
* @end
*********************************************************************/
static void mgmd_proxy_src_timer_expire_handler(mgmd_cb_t *mgmdCB, 
                                                L7_uint32 rtrIfNum,
                                                L7_inet_addr_t group, 
                                                L7_uint32 numSrcs, 
                                                L7_inet_addr_t *sourceList)
{
  mgmd_host_group_t *mgmd_host_group = L7_NULLPTR;
  mgmd_host_source_record_t *srcRec = L7_NULLPTR, srcRecSrchKey, *newHostSrcRec = L7_NULLPTR;
  L7_uint32 index;
  L7_uchar8 addrBuff[IPV6_DISP_ADDR_LEN];

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");
  mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7 (&(mgmdCB->mgmd_host_info->hostMembership), &group,
                                                        AVL_EXACT);
  if (mgmd_host_group == L7_NULL)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Failed to find group entry ");
    return;  
  }

  for (index = 0; index < numSrcs; index++)
  {
    /* INC(X-S) */
    if (mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE)
    {
      memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
      inetCopy(&srcRecSrchKey.sourceAddress,&sourceList[index]);
      if ((srcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords),
                                                         (L7_sll_member_t *)&srcRecSrchKey))
          != L7_NULLPTR)
      {
        mgmd_proxy_SPL_portno_delete(mgmdCB, mgmd_host_group,
                                     srcRec, rtrIfNum);

        if (mgmd_proxy_portlist_status(&srcRec->srcPortList) == L7_FALSE)
        {
          mgmd_proxy_src_rec_delete(mgmdCB,  mgmd_host_group, srcRec);
        }
      }
    }
    else
    {
     /* EXC(X-S, Y+S)  */
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  EXCLUDE part ");
      memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
      inetCopy(&srcRecSrchKey.sourceAddress,&sourceList[index]);
      if ((srcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords),
                                                         (L7_sll_member_t *)&srcRecSrchKey))
          != L7_NULLPTR)
      {
        MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS,"\n  Found Src = %s ",
                              &sourceList[index]);
        MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"  Deleted port = %d from ", rtrIfNum);
        MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," SPL =",&srcRec->srcPortList);

        if (mgmd_proxy_src_rec_timeout(srcRec) == L7_FALSE)
        {
          mgmd_proxy_SPL_portno_delete(mgmdCB, mgmd_host_group,
                                       srcRec, rtrIfNum);
          if (mgmd_proxy_portlist_status(&srcRec->srcPortList) == L7_FALSE)
          {
            srcRec->sourceCtime = 0;
            srcRec->sourceInterval = 0;
          }
        }
      }
      if (srcRec == L7_NULLPTR)
      {
        /* (Y+S) */
        MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  NOT Found Src = %s ",
                         inetAddrPrint(&sourceList[index], addrBuff));

        mgmd_proxy_util_new_src_record_create(mgmdCB, sourceList[index], &newHostSrcRec);
        if (newHostSrcRec == L7_NULL)
        {
          /* Return as Memory allocation failed. */
           MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not create the source Node \n");  
           return;
        }
        /* Set Source-Timeout == 0. */
        mgmd_proxy_src_rec_add (mgmdCB, mgmd_host_group, newHostSrcRec);
        /* Update the SPL of this new exclude src to GPL such that any  
           downstream listeners, other than interface from which BLOCK is 
           received, waiting for this source could also listen. */
        mgmd_proxy_GPL_to_SPL_copy(mgmdCB, mgmd_host_group, newHostSrcRec);
        mgmd_proxy_SPL_portno_delete(mgmdCB,mgmd_host_group ,newHostSrcRec,rtrIfNum);
        if (mgmd_proxy_portlist_status(&newHostSrcRec->srcPortList) == L7_FALSE)
        {
          newHostSrcRec->sourceCtime = 0;
          newHostSrcRec->sourceInterval = 0;
        }
      }
    }
  }
}


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
void mgmd_proxy_v1querier_timeout_event_handler(void *param)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)param;
  L7_uint32 left = 0;
  L7_uint32 mgmdHostVersion;
  mgmd_host_info_t  *mgmd_host_info = L7_NULLPTR;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"  Entered ");
  if (mgmdCB == L7_NULLPTR)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not get control block ");
    return;
  }
  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (mgmd_host_info == L7_NULLPTR)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_FAILURE, " Host info is null ");
    return ;
  }

  mgmd_host_info->ver1_querier_timer = L7_NULLPTR;

  mgmdHostVersion = mgmdCB->mgmd_host_info->configVersion;

  if (mgmd_host_info->ver2_querier_timer != L7_NULLPTR)
  {
    if (appTimerTimeLeftGet(mgmdCB->timerHandle,mgmd_host_info->ver2_querier_timer, &left) != L7_SUCCESS)
    {
       MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not get the left out time \n");
      return;
    }
  }

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," mgmdHostVersion = %d , compatability mode = %d, leftovertime = %d",
                   mgmdHostVersion, mgmd_host_info->hostCompatibilityMode, left);

  switch (mgmdHostVersion)
  {
    case L7_MGMD_VERSION_1:
      mgmd_host_info->hostCompatibilityMode = L7_MGMD_VERSION_1; 
      break;
    case L7_MGMD_VERSION_2:
      mgmd_host_info->hostCompatibilityMode = L7_MGMD_VERSION_2; 
      break;
    case L7_MGMD_VERSION_3:
      if (left != 0)
        mgmd_host_info->hostCompatibilityMode = L7_MGMD_VERSION_2;
      else
        mgmd_host_info->hostCompatibilityMode = L7_MGMD_VERSION_3;
      break;
    default:
      break;
  }/*End-of-Switch*/
}


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
void mgmd_proxy_v2querier_timeout_event_handler(void *param)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)param;
  mgmd_host_info_t  *mgmd_host_info = L7_NULLPTR;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");

  if (mgmdCB == L7_NULLPTR)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not get control block ");
    return;
  }
  if (mgmdCB->mgmd_host_info == L7_NULLPTR)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not get Proxy control block ");
    return;
  }
  mgmd_host_info = mgmdCB->mgmd_host_info;
  mgmd_host_info->ver2_querier_timer = L7_NULLPTR;


  if (mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_1)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Version 1 is already running ");
    return;
  }
  mgmd_host_info->hostCompatibilityMode = L7_MGMD_VERSION_3;
}


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
void mgmd_proxy_interface_timer_expiry_handler(void *param)
{

  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)param;

  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Entry");

  if (mgmdCB == L7_NULLPTR)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not get control block ");
    return;
  }

  mgmdCB->mgmd_host_info->gen_rsp_timer = L7_NULLPTR;
  mgmd_proxy_report_send(mgmdCB);

  MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_APIS, "Exit");
}


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
void mgmd_proxy_group_response_timer_expiry_handler(void *param)
{
  mgmd_cb_t                 *mgmdCB = L7_NULLPTR;
  mgmd_host_group_t         *mgmd_host_group = L7_NULLPTR;
  mgmd_host_source_record_t *hostSrcRec = L7_NULL;
  L7_uint32                  i = 0, numSrcs = 0, grpRecType = 0;
  L7_BOOL                    grpSrcReport = L7_FALSE, srcTmrFlag;
  L7_inet_addr_t            *sourceSet[MGMD_MAX_QUERY_SOURCES];
  mgmd_host_info_t          *mgmd_host_info; 
  L7_int32      handle = (L7_int32)param; 

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered , handle = %d", handle);

  mgmd_host_group = (mgmd_host_group_t *)handleListNodeRetrieve(handle); 
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," mgmd_host_group = %p ",mgmd_host_group);

  if (mgmd_host_group == L7_NULLPTR)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_FAILURE," Failed to retrive node from handle list");
    return;
  }
  mgmdCB = mgmd_host_group->mgmdCB;

  if (mgmdCB == L7_NULLPTR)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_FAILURE," Failed to get mgmd control block from group node");
    return;
  }

  if (mgmdCB->mgmd_host_info == L7_NULL)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_FAILURE," Failed to get mgmd proxy control block ");
    return;
  }


  mgmd_host_info = mgmdCB->mgmd_host_info;

  mgmd_host_group->hostState = MGMD_HOST_IDLE_MEMBER;

  mgmd_host_group->grp_rsp_timer = L7_NULLPTR;

  memset(sourceSet,0, sizeof(sourceSet));

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," numSrcs in GrpNode = %d ", 
                   SLLNumMembersGet(&(mgmd_host_group->sourceRecords)));

  for (hostSrcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); hostSrcRec != L7_NULL; 
      hostSrcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)hostSrcRec))
  {
    if (hostSrcRec->sendQueryReport == L7_TRUE)
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  grpSrcReport = %d ", grpSrcReport);
      if (grpSrcReport == L7_FALSE)
      {
        memset(sourceSet,0, sizeof(sourceSet));
        i = 0;
        numSrcs = 0;
        grpSrcReport = L7_TRUE;
      }
      MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_REPORTS,"Adding src for GrpSrcQ ",&hostSrcRec->sourceAddress);
      sourceSet[i] = &hostSrcRec->sourceAddress;
      numSrcs++;
      i++;
      grpRecType = L7_IGMP_MODE_IS_INCLUDE;
    }
    if (grpSrcReport == L7_FALSE)
    {
      srcTmrFlag = mgmd_proxy_src_rec_timeout(hostSrcRec);
      MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_REPORTS,"Adding src for GrpQ ",&hostSrcRec->sourceAddress);
      if ((mgmd_host_group->filterMode == MGMD_FILTER_MODE_INCLUDE) ||
          ((mgmd_host_group->filterMode == MGMD_FILTER_MODE_EXCLUDE) && srcTmrFlag == L7_TRUE))
      {
        sourceSet[i] = &(hostSrcRec->sourceAddress);
        numSrcs++;
        i++;
      }
      grpRecType = mgmd_host_group->filterMode;
    }
    /* Reset the query repond flag as the source is checked for 
       adding to response report or not by now */
    hostSrcRec->sendQueryReport = L7_FALSE;
  }
  if (mgmd_proxy_grp_report_send(mgmdCB, mgmd_host_group->group, 
                                 numSrcs, sourceSet,
                                 grpRecType,mgmd_host_group) == L7_FAILURE)
  {
     MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "V3 report Not Sent \n");
    return;        
  }
}

/*********************************************************************
* @purpose Handles the unsolicited report timer interval expiry
*
* @param    param     @b { (input) }  Timer Event Info
*
* @returns  None
*
* @notes it sends the group report or group src specific report 
* @end
*********************************************************************/
void mgmd_proxy_unsolicited_timer_expiry_handler(void *param)
{
  mgmd_cb_t                    *mgmdCB = L7_NULLPTR;
  L7_uint32                     reportIndex, index;
  mgmd_host_group_t            *mgmd_host_group = L7_NULL, mgmd_host_group_srchKey;
  mgmd_proxy_unsolicited_rpt_t *rreq = L7_NULL;
  mgmd_host_info_t             *mgmd_host_info;
  L7_uint32                     unsolicitedReportInterval;
  L7_int32      handle = (L7_int32)param; 
  L7_inet_addr_t              *srcList[MGMD_MAX_QUERY_SOURCES];

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered ");

  rreq = (mgmd_proxy_unsolicited_rpt_t *)handleListNodeRetrieve(handle);   

  if (rreq == L7_NULLPTR)
  {
    return;
  }

  mgmdCB = rreq->mgmdCB;
  reportIndex = rreq->reportId;

  if (mgmdCB->mgmd_host_info == L7_NULL)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Proxy interface is not routing interface ");
    return;
  }

  mgmd_host_info = mgmdCB->mgmd_host_info;
  unsolicitedReportInterval = mgmd_host_info->unsolicitedReportInterval;

  rreq->timer = L7_NULLPTR;

  rreq = L7_NULL;
  /* Get the node from the Unsolicited Reports Linked List. */
  for (rreq = (mgmd_proxy_unsolicited_rpt_t *)SLLFirstGet(&(mgmdCB->mgmd_host_info->ll_unsolicited_reports)); rreq != L7_NULL; 
      rreq = (mgmd_proxy_unsolicited_rpt_t *)SLLNextGet(&(mgmdCB->mgmd_host_info->ll_unsolicited_reports), (L7_sll_member_t *)rreq))
  {
    if (rreq->reportId == reportIndex)
    {
      break;
    }
  }

  if (rreq == L7_NULL)
  {
     MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "\n Could not find report req ");
    return;
  }

  memset(&mgmd_host_group_srchKey, 0, sizeof(mgmd_host_group_t));
  mgmd_host_group_srchKey.group = rreq->group;

  if ((mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7 (&(mgmd_host_info->hostMembership), 
                                                             &mgmd_host_group_srchKey, AVL_EXACT)) != L7_NULLPTR)
  {
    /* The required Group no more exists.*/
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Found the group");
  }
  /* Group-specific report */
  for (index = 0; index < rreq->numSrcs; index++)
  {
    srcList[index] = &(rreq->srcList[index]);
  }
  if (mgmd_proxy_grp_report_send(mgmdCB, rreq->group,rreq->numSrcs,
                                 srcList,rreq->grpRecType,
                                 mgmd_host_group) ==L7_FAILURE)
  {
     MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Message Not Sent \n");
    /*return; */   /*fallback to cleanup ll_unsolicited_reports*/
  }

  rreq->retryCount--;
  if (rreq->retryCount == 0)
  {
    if (SLLDelete(&(mgmdCB->mgmd_host_info->ll_unsolicited_reports), (L7_sll_member_t *)rreq) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE,   " Could not Delete the Unsolicited report node \n");
      return ;
    }
  }
  else
  {
    if (mgmdProxyUtilAppTimerSet(mgmdCB, L7_NULL, 
                                 (void*)rreq->timer_handle,
                                 &rreq->timer,
                                 unsolicitedReportInterval, 
                                 L7_MGMD_UNSOLICITED_REPORT_TIMER) != L7_SUCCESS)
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not start the unsolicited report timer  \n");
      return;
    }
  }
}/*End-of-Function.*/   


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
void mgmd_proxy_querier_update_event_send (mgmd_cb_t  *mgmdCB, 
                                           L7_int32    intIfNum, 
                                           L7_BOOL     querierFlag)
{ 
  mgmdProxyQuerierStateChangeEventHandler(mgmdCB);
}

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

L7_RC_t  mgmdProxyUtilAppTimerHandleDelete(mgmd_cb_t *mgmdCB,L7_APP_TMR_HNDL_t *timer, L7_uint32 *handle)
{
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered ");
  if (timer != L7_NULL && *timer != L7_NULLPTR)
  {
    if (appTimerDelete(mgmdCB->timerHandle, *timer ) != L7_SUCCESS)
    {
       MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not Delete the timer.\n");
    }
    *timer=L7_NULLPTR;    
  }                                                                    
  if (handle != L7_NULL && *handle != L7_NULL)
  {
    handleListNodeDelete(mgmdCB->handle_list,handle); 
  }
  return L7_SUCCESS;
}
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
void mgmd_proxy_group_mode_change(mgmd_cb_t *mgmdCB, L7_uint32 rtrIntf, 
                                  mgmd_host_group_t *mgmd_host_group, L7_uint32 numSrcs, 
                                  L7_inet_addr_t *sourceList)
{
  mgmd_host_source_record_t *srcRec = L7_NULL;
  mgmd_host_source_record_t *prev = L7_NULL;
  L7_uint32 index;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered ");

  if (mgmd_host_group == L7_NULL)
  {
    return;
  }

  /* If group record G is present, then ...*/
  /* ii. Delete P from GPL. */
  mgmd_proxy_GPL_portno_delete (mgmdCB, mgmd_host_group, rtrIntf);

  /* i. Delete P from the SPL of the source records except from the sources present in the source list */
  for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
      srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)srcRec))
  {
    for (index=0; index < numSrcs; index++)
    {
      if (L7_INET_IS_ADDR_EQUAL(&(srcRec->sourceAddress), &sourceList[index]))
      {
        break;
      }
    }
    if (index == numSrcs)
    {
      /* Remove port from SPL */
      mgmd_proxy_SPL_portno_delete(mgmdCB, mgmd_host_group,srcRec,rtrIntf);
    }
  }

  if (mgmd_proxy_portlist_status(&mgmd_host_group->grpPortList) == L7_FALSE)
  {
    for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
        srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)srcRec))
    {
      /* Check if the current Src has timed-out. */
      if (mgmd_proxy_src_rec_timeout(srcRec) == L7_TRUE)
      {
        /* Then, delete the src. */
        mgmd_proxy_src_rec_delete (mgmdCB, mgmd_host_group, srcRec);
        if (prev == L7_NULLPTR)
          continue;
        srcRec= prev;
      }
      prev = srcRec;
    }  /*End-of-for*/

    if (mgmd_host_group->filterMode == MGMD_FILTER_MODE_EXCLUDE)
    {
      mgmd_host_group->filterMode = MGMD_FILTER_MODE_INCLUDE;
      mgmd_proxy_to_incl_send(mgmdCB, mgmd_host_group);
    }

  }
}
/*********************************************************************
* @purpose  Handles the interface version change
*
* @param    mgmdCB            @b{ (input) }  MGMD Control Block
* @param    rtrIfnum          @b{ (input) }  router interface number
* @param    version           @b{ (input) }  interface verion
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t mgmd_proxy_interface_version_set(mgmd_cb_t *mgmdCB,
                                         L7_uint32 rtrIfNum, L7_uint32 version)
{
  mgmd_host_info_t       *mgmd_host_info;
  mgmd_host_group_t         *temp_mgmd_host_group = L7_NULL;
  mgmd_host_group_t          dummyGroup;
  mgmd_host_source_record_t *hostSrcRec = L7_NULL;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered , ver = %d",version );

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (mgmdCB->mgmd_proxy_status != L7_ENABLE)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Proxy is not enabled");
    return L7_FAILURE;
  }
  if (mgmd_host_info->rtrIntf != rtrIfNum)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Configured iface is not the proxy interface");
    return L7_FAILURE;
  }
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Rx'ed version = V%d , existing version = V%d",
                   version, mgmd_host_info->hostCompatibilityMode);

  mgmd_host_info->configVersion = version;

  switch(version)
  {
    case L7_MGMD_VERSION_3:
      {
        if (mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_1)
        {
          if (mgmd_host_info->ver1_querier_timer == L7_NULLPTR)
          {
            /* This means, the V1 querier timer has not started, and hence the 
               host comapatability mode has been changed from configuration only,
               so we can override with the new version configured */
            mgmd_host_info->hostCompatibilityMode = L7_MGMD_VERSION_3;
          }
        }
        if (mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_2)
        {
          if (mgmd_host_info->ver2_querier_timer == L7_NULLPTR)
          {
            /* This means, the V2 querier timer has not started, and hence the 
               host comapatability mode has been changed from configuration only,
               so we can override with the new version configured */
            mgmd_host_info->hostCompatibilityMode = L7_MGMD_VERSION_3;
          }
        }
      }
      break;
    case L7_MGMD_VERSION_2:
      {
        if (mgmd_host_info->hostCompatibilityMode > L7_MGMD_VERSION_2)
        {
          MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Degrading version from V%d to V2",
                                                   mgmd_host_info->hostCompatibilityMode);
          mgmd_host_info->hostCompatibilityMode = L7_MGMD_VERSION_2;

          /* delete the version 2 timer */
          if (mgmdProxyUtilAppTimerDelete(mgmdCB, &mgmd_host_info->ver2_querier_timer) != L7_SUCCESS)
          {
            MGMD_PROXY_DEBUG(MGMD_DEBUG_FAILURE,"Could not delete the ver2_querier_timer.\n");          
          }
          if (mgmdProxyUtilAppTimerDelete(mgmdCB, &mgmd_host_info->gen_rsp_timer) != L7_SUCCESS)
          {
            MGMD_PROXY_DEBUG(MGMD_DEBUG_FAILURE,"Could not delete the general response timer.\n");          
          }
          /* Get first entry in the tree */
          memset(&dummyGroup, 0, sizeof(mgmd_host_group_t));
          temp_mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7(&(mgmd_host_info->hostMembership), (void *)(&dummyGroup), AVL_NEXT);
          while (temp_mgmd_host_group)
          {
            for (hostSrcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(temp_mgmd_host_group->sourceRecords)); hostSrcRec != L7_NULL; 
                hostSrcRec = (mgmd_host_source_record_t *)SLLNextGet(&(temp_mgmd_host_group->sourceRecords), (L7_sll_member_t *)hostSrcRec))
            {
              hostSrcRec->sendQueryReport = L7_FALSE;
            }/*End-of-for*/

            /* Delete Group response timer , if running */

            if (mgmdProxyUtilAppTimerDelete(mgmdCB, &temp_mgmd_host_group->grp_rsp_timer) != L7_SUCCESS)
            {
              MGMD_PROXY_DEBUG(MGMD_DEBUG_FAILURE,"Could not delete the general response timer.\n");          
            }
            /* Get next entry in the tree */
            temp_mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7(&(mgmd_host_info->hostMembership), (void *)(temp_mgmd_host_group), AVL_NEXT);
          }/*End-of-While*/
        }
        else if (mgmd_host_info->hostCompatibilityMode == L7_MGMD_VERSION_1)
        {
          MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Already V1 running");
          if (mgmd_host_info->ver1_querier_timer == L7_NULLPTR)
          {
            /* This means, the V1 querier timer has not started, and hence the 
               host comapatability mode has been changed from configuration only,
               so we can override with the new version configured */
            mgmd_host_info->hostCompatibilityMode = L7_MGMD_VERSION_2;          
          }
        }
        /* delete the version 2 timer */
        if (mgmdProxyUtilAppTimerDelete(mgmdCB, &mgmd_host_info->ver2_querier_timer) != L7_SUCCESS)
        {
          MGMD_PROXY_DEBUG(MGMD_DEBUG_FAILURE,"Could not delete the version2 querier timer.\n");          
        }

      }
      break;
    case L7_MGMD_VERSION_1:
      {
        /* delete the version 2 timer */
        if (mgmdProxyUtilAppTimerDelete(mgmdCB, &mgmd_host_info->ver2_querier_timer) != L7_SUCCESS)
        {
          MGMD_PROXY_DEBUG(MGMD_DEBUG_FAILURE,"Could not delete the version2 querier timer.\n");          
        }
        /* delete the version 1 timer */
        if (mgmdProxyUtilAppTimerDelete(mgmdCB, &mgmd_host_info->ver1_querier_timer) != L7_SUCCESS)
        {
          MGMD_PROXY_DEBUG(MGMD_DEBUG_FAILURE,"Could not delete the version1 querier timer.\n");          
        }

        if (mgmd_host_info->hostCompatibilityMode > L7_MGMD_VERSION_1)
        {
          MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Degrading version from V%d to V1",
                                                   mgmd_host_info->hostCompatibilityMode);
                                                   
          mgmd_host_info->hostCompatibilityMode = L7_MGMD_VERSION_1;
          if (mgmdProxyUtilAppTimerDelete(mgmdCB, &mgmd_host_info->gen_rsp_timer) != L7_SUCCESS)
          {
            MGMD_PROXY_DEBUG(MGMD_DEBUG_FAILURE,"Could not delete the general response timer.\n");          
          }
          /* Get first entry in the tree */
          memset(&dummyGroup, 0, sizeof(mgmd_host_group_t));
          temp_mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7(&(mgmd_host_info->hostMembership), (void *)(&dummyGroup), AVL_NEXT);
          while (temp_mgmd_host_group)
          {
            for (hostSrcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(temp_mgmd_host_group->sourceRecords)); hostSrcRec != L7_NULL; 
                hostSrcRec = (mgmd_host_source_record_t *)SLLNextGet(&(temp_mgmd_host_group->sourceRecords), (L7_sll_member_t *)hostSrcRec))
            {
              hostSrcRec->sendQueryReport = L7_FALSE;
            }/*End-of-for*/

            /* Delete Group response timer , if running */
            if (mgmdProxyUtilAppTimerDelete(mgmdCB, &temp_mgmd_host_group->grp_rsp_timer) != L7_SUCCESS)
            {
              MGMD_PROXY_DEBUG_ADDR(MGMD_DEBUG_FAILURE,"Could not delete the group response timer for grp=\n",
                                    &temp_mgmd_host_group->group);
            }
            /* Get next entry in the tree */
            temp_mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7(&(mgmd_host_info->hostMembership), (void *)(temp_mgmd_host_group), AVL_NEXT);
          }/*End-of-While*/
        }
      }
      break;
    default:
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Unknown version set");
      return L7_FAILURE;
  }
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Successful exit ");
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  This is the ExcludeSrcDelete() that updates the SPL and GPL  
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    ifNum             @b{ (input) }   Interface Number
* @param    mgmd_host_group   @b{ (input) }   MGMD Host Group
* @param    srcsCount         @b{ (input) }   Source Count
* @param    srcList           @b{ (input) }   Source List
*
* @returns  None
*
* @notes  
* @end
*********************************************************************/
static void mgmd_proxy_ExcludeSrcDelete (mgmd_cb_t *mgmdCB, L7_uint32 rtrIfNum, 
                                      mgmd_host_group_t *mgmd_host_group,
                                      L7_uint32 srcsCount, L7_inet_addr_t *srcList)
{
  mgmd_host_source_record_t *srcRec = L7_NULL,*delSrcRec = L7_NULLPTR,srcRecSrchKey;
  L7_uint32                  index;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"\n  Entered, ifNum = %d ",rtrIfNum);

  /* a. If group record is present,..*/
  if (mgmd_host_group == L7_NULL)
  {
    return;
  }

  /* b. If (SourceList != NULL), for all the sources present in SL,
        delete the port from SPL of the src expired ..*/
  if (srcList != L7_NULL)
  {
    for (index=0; index < srcsCount; index++)
    {
      memset(&srcRecSrchKey, 0, sizeof(mgmd_host_source_record_t));
      inetCopy(&srcRecSrchKey.sourceAddress, &srcList[index]);
      srcRec = (mgmd_host_source_record_t *)SLLFind(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)&srcRecSrchKey);

      /* ii. If the source record is present, create the source  *
       *     record and copy all ports of GPL to SPL except port P.. */
      if (srcRec != L7_NULL)
      {
        mgmd_proxy_SPL_portno_delete(mgmdCB, mgmd_host_group,
                                     srcRec, rtrIfNum);

        MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS,"  Deleted port for Src", &srcRec->sourceAddress);
        MGMD_PROXY_DEBUG_BITS(MGMD_PROXY_DEBUG_APIS," SPL =",&srcRec->srcPortList);
        if (mgmd_proxy_portlist_status(&srcRec->srcPortList) == L7_FALSE)
        {
          MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS,
                           " Src is com-letely moved to exclude state for src =",&srcRec->sourceAddress);
          srcRec->sourceCtime = 0; 
          srcRec->sourceInterval = 0; 
        }
      }
    }/*End-of-FOR-LOOP*/
  }/*End-of-Main-IF-Block*/

  MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS," For Grp = , ", &mgmd_host_group->group);
  MGMD_PROXY_DEBUG_BITS(MGMD_PROXY_DEBUG_APIS," GPL = ", &mgmd_host_group->grpPortList);

  /* c. For all srcs whose SPL == GPL, delete them. */
  delSrcRec = L7_NULLPTR;
  srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords));
  while(srcRec != L7_NULLPTR) 
  {
    delSrcRec = srcRec;
    srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords),
                                                     (L7_sll_member_t *)srcRec);
    /* Check if the source record's SPL == GPL. */
    MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS," For Src= , ", &delSrcRec->sourceAddress);
    MGMD_PROXY_DEBUG_BITS(MGMD_PROXY_DEBUG_APIS," SPL = ", &delSrcRec->srcPortList);
    if (mgmd_proxy_bitset_compare(&mgmd_host_group->grpPortList, 
                                  &delSrcRec->srcPortList) == L7_TRUE)
    {
      /* Delete the SrcRec. */
      mgmd_proxy_src_rec_delete(mgmdCB, mgmd_host_group, delSrcRec);

    }
  }/*End-of-for*/

  /* d. If((GPL == NULL) && (source record set == NULL), 
                delete the group record (G). */
  if ((mgmd_proxy_portlist_status(&mgmd_host_group->grpPortList) == L7_FALSE) &&
      (mgmd_host_group->sourceRecords.sllNumElements == 0))
  {
    mgmd_proxy_grp_rec_delete (mgmdCB, mgmd_host_group);
    mgmdCB->mgmd_host_info->numOfGroups--;
  }
}
/**********************************************************************
* @purpose  To enable/disable multicast data reception in stack for the 
*           specified interface  
*
* @param    family            @b{ (input) }   MGMD address family
* @param    rtrIfNum          @b{ (input) }   Router interface Number
* @param    mode              @b{ (input) }   L7_ENABLE/ L7_DISABLE
*
* @returns  None
*
* @notes    This function is specific to IPv6 only.
* @end
*********************************************************************/
L7_RC_t mgmd_proxy_MfcIntfEventQueue(L7_uchar8 family, L7_uint32 rtrIfNum, L7_uint32 mode)
{
  mfcInterfaceChng_t ifChange;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"Enter, rtIfNum = %d, mode = %d ", 
                   rtrIfNum, mode);

  memset(&ifChange, 0, sizeof(mfcInterfaceChng_t));
  ifChange.family = family;
  ifChange.rtrIfNum = rtrIfNum;
  ifChange.mode = mode;
  if(mfcMessagePost(MFC_INTERFACE_STATUS_CHNG, &ifChange,
                    sizeof(mfcInterfaceChng_t)) != L7_SUCCESS)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_FAILURE,
                "MFC_INTERFACE_MODE_CHG_EVENT Failed.");
  }
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"Exit ");
  return L7_SUCCESS;
}


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
static 
L7_RC_t mgmdProxyUtilAppTimerSet (mgmd_cb_t *mgmdCB,
                      L7_app_tmr_fn pFunc,
                      void *pParam,
                      L7_APP_TMR_HNDL_t *tmrHandle,
                      L7_uint32 timeOut,
                      MGMD_TIMER_t timerType)
{
  L7_char8 timerName[APPTIMER_STR_LEN];
  
  switch(timerType)
  {
  case L7_MGMD_GRP_RSP_TIMER:
       pFunc = mgmd_proxy_group_response_timer_expiry_handler;
       osapiStrncpySafe(timerName,"MP-GRSP", APPTIMER_STR_LEN);              
       break;
  case L7_MGMD_GENERAL_RSP_TIMER:
       pFunc = mgmd_proxy_interface_timer_expiry_handler;
       osapiStrncpySafe(timerName,"MP-RSP", APPTIMER_STR_LEN);              
       break;
  case L7_MGMD_UNSOLICITED_REPORT_TIMER:
       pFunc = mgmd_proxy_unsolicited_timer_expiry_handler;
       osapiStrncpySafe(timerName,"MP-URT", APPTIMER_STR_LEN);              
       break;
  case L7_MGMD_VERSION1_TIMER:
       pFunc = mgmd_proxy_v1querier_timeout_event_handler;
       osapiStrncpySafe(timerName,"MP-V1T", APPTIMER_STR_LEN);              
       break;
  case L7_MGMD_VERSION2_TIMER:
       pFunc = mgmd_proxy_v2querier_timeout_event_handler;
       osapiStrncpySafe(timerName,"MP-V2T", APPTIMER_STR_LEN);                     
       break;
  default:
       MGMD_DEBUG(MGMD_DEBUG_TIMERS, 
                  " Timer type(%d) has no pre-defined callback function", timerType);
       break;
  }

  if(*tmrHandle != NULL)
  {
    if (appTimerUpdate (mgmdCB->timerHandle,tmrHandle,pFunc,pParam,timeOut,
                        timerName)
                        != L7_SUCCESS)
    {
      *tmrHandle = 0;
      MGMD_DEBUG(MGMD_DEBUG_TIMERS, "Failed to update timer(%d) for timeout = %d", 
                 timerType, timeOut);
      return L7_FAILURE;
    }
  }
  else
  {
    if ((*tmrHandle = appTimerAdd (mgmdCB->timerHandle, pFunc, (void*) pParam,
                                   timeOut, timerName)) == L7_NULLPTR)
    {
      MGMD_DEBUG(MGMD_DEBUG_TIMERS, "Failed to start timer(%d) for timeout = %d",
                 timerType, timeOut);
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
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
static
L7_RC_t mgmdProxyUtilAppTimerLower (mgmd_cb_t *mgmdCB,
                      L7_app_tmr_fn pFunc,
                      void *pParam,
                      L7_APP_TMR_HNDL_t *tmrHandle,
                      L7_uint32 lowerTimeOut,
                      MGMD_TIMER_t timerType)
{
  L7_uint32 timeLeft = 0;
  L7_char8 timerName[80];
  
  switch(timerType)
  {
  case L7_MGMD_GRP_RSP_TIMER:
       pFunc = mgmd_proxy_group_response_timer_expiry_handler;
       osapiStrncpySafe(timerName,"MGMD-Proxy Group RSP Timer2", 80);              
       break;
  case L7_MGMD_GENERAL_RSP_TIMER:
       pFunc = mgmd_proxy_interface_timer_expiry_handler;
       osapiStrncpySafe(timerName,"MGMD-Proxy General RSP Timer2", 80);              
       break;
  case L7_MGMD_UNSOLICITED_REPORT_TIMER:
       pFunc = mgmd_proxy_unsolicited_timer_expiry_handler;
       osapiStrncpySafe(timerName,"MGMD-Proxy Unsolicited Report Timer2", 80);              
       break;
  case L7_MGMD_VERSION1_TIMER:
       pFunc = mgmd_proxy_v1querier_timeout_event_handler;
       osapiStrncpySafe(timerName,"MGMD-Proxy V1 Timer2", 80);              
       break;
  case L7_MGMD_VERSION2_TIMER:
       pFunc = mgmd_proxy_v2querier_timeout_event_handler;
       osapiStrncpySafe(timerName,"MGMD-Proxy V2 Timer2", 80);                     
       break;  
  default:
       MGMD_DEBUG(MGMD_DEBUG_TIMERS, 
                  " Timer type(%d) has no pre-defined callback function", timerType);
       break;
  }
  if(*tmrHandle != NULL)
  {
    if (appTimerTimeLeftGet(mgmdCB->timerHandle,tmrHandle, &timeLeft)
                        != L7_SUCCESS)
    {
      MGMD_DEBUG(MGMD_DEBUG_TIMERS, "Failed to get leftover time for timer(%d)", 
                 timerType);
      return L7_FAILURE;
    }
    if (lowerTimeOut < timeLeft)
    {
      if (appTimerUpdate(mgmdCB->timerHandle, tmrHandle,pFunc,pParam,lowerTimeOut,
                        timerName)
                        != L7_SUCCESS)
        
      {
        MGMD_DEBUG(MGMD_DEBUG_TIMERS, "Failed to update timer(%d) for timeout = %d",
                   timerType, lowerTimeOut);
        return L7_FAILURE;
      }
    }
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  To Delete the  AppTimer
*
* @param    mgmdCB          @b{ (input) } MGMD Control Block
*           timerHandle     @b{ (input) } time handle to be deleted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t  mgmdProxyUtilAppTimerDelete(mgmd_cb_t *mgmdCB,L7_APP_TMR_HNDL_t *timer)
{
  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered, timer = %d", timer);

  if (timer != L7_NULL && *timer != L7_NULLPTR)
  {
    if (appTimerDelete(mgmdCB->timerHandle, *timer ) != L7_SUCCESS)
    {
      MGMD_DEBUG(MGMD_DEBUG_TIMERS, " Could not Delete the timer.\n");
      /*return L7_FAILURE;*/
    }
    *timer=L7_NULLPTR;        
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Processes all configuration events
*
* @param    mgmd_map_events_params_t @b{ (input) } mapping event info
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyProtocolConfigSet(mgmdMapEventParams_t *eventParams)
{
  mgmd_cb_t *mgmdCB = L7_NULLPTR;
  L7_RC_t    rc = L7_FAILURE;
  L7_uint32 eventInfo;
  L7_uint32 rtrIfNum;
  MGMD_UI_EVENT_TYPE_t eventType;

  if (eventParams == L7_NULLPTR)
  {
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "Invalid eventParams \n");
    return L7_FAILURE;
  }
  eventInfo = eventParams->eventInfo;
  rtrIfNum = eventParams->rtrIfNum;
  eventType = eventParams->eventType;

  MGMD_DEBUG(MGMD_DEBUG_EVENTS, " Family = %d , eventType = %d", eventParams->familyType,
             eventParams->eventType);

  MGMD_DEBUG(MGMD_DEBUG_EVENTS, " rtrIfNum = %d , intfType = %d", eventParams->rtrIfNum,
             eventParams->intfType);

  MGMD_DEBUG(MGMD_DEBUG_EVENTS, "eventInfo = %d", eventParams->eventInfo);
  if (eventParams->mgmdIntfInfo != L7_NULLPTR)
  {
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : robustness = %d", 
               eventParams->mgmdIntfInfo->robustness);
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : queryIntvl = %d", 
               eventParams->mgmdIntfInfo->queryInterval);
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : queryMaxRespTime = %d", 
               eventParams->mgmdIntfInfo->queryResponseInterval);
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : startupIntvl = %d", 
               eventParams->mgmdIntfInfo->startupQueryInterval);
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : startupCount = %d", 
               eventParams->mgmdIntfInfo->startupQueryCount); 
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : lastMemQIntvl = %d", 
               eventParams->mgmdIntfInfo->lastMemQueryInterval); 
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "IntfConfigData : lastMemQCount = %d", 
               eventParams->mgmdIntfInfo->lastMemQueryCount); 
  }


  if (mgmdMapProtocolCtrlBlockGet(eventParams->familyType, 
                                  (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    MGMD_DEBUG(MGMD_DEBUG_EVENTS, "Invalid Control Block\r\n");
    return L7_FAILURE;
  }
  switch(eventType)
  {
  case MGMD_VERSION_SET:
       rc = mgmd_proxy_interface_version_set(mgmdCB, rtrIfNum, eventInfo);
       break;
  case MGMD_UNSOLICITED_REPORT_INTERVAL_SET:
       mgmdCB->mgmd_host_info->unsolicitedReportInterval = eventInfo;
       rc = L7_SUCCESS;
       break;
  case MGMD_ROBUSTNESS_SET:
       mgmdCB->mgmd_host_info->robustness = eventInfo;
       rc = L7_SUCCESS;
       break;
  default:
       MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_FAILURE, " Invalid event = %d", eventType);
       break;
  }
  return rc;
}
