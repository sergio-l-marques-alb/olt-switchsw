/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   mgmd_proxy_debug.c
*
* @purpose    Implements the Debug support functionality of the Multicast 
*             Group Membership Discovery (MGMD_PROXY)Proxy module
*
* @component  Multicast Group Membership Discovery  Proxy(MGMD_Proxy)
*
* @comments   
*
* @create     Dec 29,2006
*
* @author     ddevi
* @end
*
**********************************************************************/
/**********************************************************************
                  Includes
***********************************************************************/
#include <string.h>
#include "log.h"
#include "datatypes.h"
#include "sysapi.h"
#include "intf_bitset.h"
#include "l7_mgmdmap_include.h"
#include "mgmd.h"
#include "mgmd_proxy.h"
#include "mgmd_proxy_debug.h"

/**********************************************************************
                  Typedefs & Defines
***********************************************************************/
#define MGMD_PROXY_NUM_FLAG_BYTES     ((MGMD_PROXY_DEBUG_FLAG_LAST + 7) / 8)


/******************************************************************
                 Global Declarations
******************************************************************/
static L7_uchar8 debugFlags[MGMD_PROXY_NUM_FLAG_BYTES];
static L7_BOOL   debugEnabled = L7_FALSE;

/*****************************************************************
    Function Definitions
******************************************************************/

/*********************************************************************
*
* @purpose  Enable Debug Tracing for the MGMD_PROXY.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyDebugEnable(void)
{
  debugEnabled = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for the MGMD_PROXY.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyDebugDisable(void)
{
  debugEnabled = L7_FALSE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of MGMD_PROXY.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyDebugAllSet(void)
{
  memset(debugFlags, 0xFF, sizeof(debugFlags));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of MGMD_PROXY.
*
* @param    None.
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyDebugAllReset(void)
{
  memset(debugFlags, 0, sizeof(debugFlags));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in MGMD_PROXY.
*
* @param    flag    @b{(input)}   Flags to be Set
*
* @returns  L7_SUCCESS - if Debug trace was successfully enabled.
*           L7_FAILURE - if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyDebugFlagSet(MGMD_PROXY_DEBUG_FLAGS_t flag)
{
  if(flag >= MGMD_PROXY_DEBUG_FLAG_LAST)
    return L7_FAILURE;
  debugFlags[flag/8] |= (1 << (flag % 8));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in MGMD_PROXY.
*
* @param    flag    @b{(input)}   Flags to be Cleared
*
* @returns  L7_SUCCESS - if Debug trace was successfully disabled.
*           L7_FAILURE - if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyDebugFlagReset(MGMD_PROXY_DEBUG_FLAGS_t flag)
{
  if(flag >= MGMD_PROXY_DEBUG_FLAG_LAST)
    return L7_FAILURE;
  debugFlags[flag/8] &= (~(1 << (flag % 8)));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in MGMD_PROXY.
*
* @param    dbg_type  @b{(input)}   Flag to be Checked
*
* @returns  L7_TRUE - if the Debug trace flag is turned ON.
*           L7_FALSE - if the Debug trace flag is turned OFF.
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL mgmdProxyDebugFlagCheck(MGMD_PROXY_DEBUG_FLAGS_t dbg_type)
{
  if(debugEnabled != L7_TRUE)
    return L7_FALSE;
  if(dbg_type >= MGMD_PROXY_DEBUG_FLAG_LAST)
    return L7_FALSE;
  if((debugFlags[dbg_type/8] & (1 << (dbg_type % 8))) != 0)
    return L7_TRUE;
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Shows the current MGMD_PROXY Debug flag status.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
void mgmdProxyDebugFlagShow(void)
{
  L7_uint32 i;

  if(L7_TRUE == debugEnabled)
  {
    sysapiPrintf(" MGMD_PROXY Debugging : Enabled\n");
    for(i = 0;  i < MGMD_PROXY_DEBUG_FLAG_LAST ; i ++)
    {
      if(L7_TRUE == mgmdProxyDebugFlagCheck(i))
        sysapiPrintf("     debugFlag [%d] : %s\n", i , "Enabled");
      else
        sysapiPrintf("     debugFlag [%d] : %s\n", i , "Disabled");
    }
  }
  else
  {
    sysapiPrintf (" MGMD_PROXY Debugging : Disabled\n");
  }
}

/*********************************************************************
*
* @purpose  Shows usage of the MGMD_PROXY Debug utility
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
void mgmdProxyDebugHelp(void)
{
  sysapiPrintf(" Use mgmdProxyDebugEnable()/mgmdProxyDebugDisable() to Enable/Disable Debug trace in MGMD_PROXY\n");
  sysapiPrintf(" Use mgmdProxyDebugFlagSet(flag)/mgmdProxyDebugFlagReset(flag) to Enable/Disable specific functionality traces\n");
  sysapiPrintf(" Use mgmdProxyDebugFlagAllSet()/mgmdProxyDebugFlagAllReset() to Enable/Disable all traces\n");
  sysapiPrintf("     Various Debug Trace flags and their definitions are as follows ;\n");
  sysapiPrintf("         0  -> Trace the complete Receive Data path\n");
  sysapiPrintf("         1  -> Trace the complete Transmission Data path\n");
  sysapiPrintf("         2  -> Trace all the EVENT generations and receptions\n");
  sysapiPrintf("         3  -> Trace all Timer activities\n");
  sysapiPrintf("         4  -> Trace all failures\n");
  sysapiPrintf("         5  -> Trace all the APIs invoked\n");
sysapiPrintf(" mgmdProxyDebugProxyInfoShow(family) \n");
  sysapiPrintf(" mgmdProxyDebugGroupsShow(family, count) \n");
  sysapiPrintf(" mgmdProxyDebugSATShow(family =1 or 2) \n");
  sysapiPrintf(" mgmdProxyDebugMRTTableShow(family, count) \n");
  sysapiPrintf(" mgmdProxyDebugAdminScopeTableShow(family, count) \n");

}
/*****************************************************************
* @purpose  Debug routine. It displays interface bit set values.
*
* @param    interface_mask  @b{ (input) } bitset to display values for
*
* @returns none
*
* @notes  
*
* @end
*********************************************************************/
static void mgmdProxyDebugBitsetShow (interface_bitset_t *interface_mask)
{
  L7_uint32 count = 0;     
  L7_uint32 index = 0;

  for (index=0; index < MAX_INTERFACES; index++)
  {
    if (BITX_TEST (interface_mask, index))
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, " %u", index);
      count++;
      if ((count%10) == 0)
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n");
      }
    }
  }/*End-of-For-Loop*/   
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
static L7_uint32 mgmdProxyDebugBitsetCountGet(interface_bitset_t *interface_mask)
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
* @purpose Debug show routine to display global proxy info
*
* @param   family            @b{ (input) }   Family
*
* @returns None
*
* @notes   None
* @end
*********************************************************************/
void mgmdProxyDebugProxyInfoShow(L7_uint32 family)
{
  L7_uint32           now;
  mgmd_host_info_t   *mgmd_host_info;
  L7_uint32           timeLeft;
  mgmd_cb_t          *mgmdCB = L7_NULLPTR;
  L7_uchar8           addrStr[IPV6_DISP_ADDR_LEN];

  if (mgmdMapProtocolCtrlBlockGet(family, 
                                  (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed to access control block\n");
    return;
  }
  mgmd_host_info = mgmdCB->mgmd_host_info;
  now = osapiUpTimeRaw();

  if (mgmd_host_info == L7_NULL)
    return;

  inetAddrHtop(&(mgmd_host_info->querierIP), addrStr);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "querier IP = %s\n", addrStr);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Configured version = %d\n", 
                mgmd_host_info->configVersion);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Robustness = %d\n", 
                mgmd_host_info->robustness);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Unsolicited Report interval = %d\n", 
                mgmd_host_info->unsolicitedReportInterval);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "numOfGrps = %d \nrestartcount = %d \n", 
                mgmd_host_info->numOfGroups, mgmdCB->mgmd_proxy_restartCount);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Host Compatability version = %d ",
                mgmd_host_info->hostCompatibilityMode);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "reportRateLimitCnt = %d\n", 
        mgmd_host_info->reportRateLimitCnt);
  timeLeft = 0;
  if (mgmd_host_info->gen_rsp_timer != L7_NULLPTR)
  {
    if (appTimerTimeLeftGet(mgmdCB->timerHandle, mgmd_host_info->gen_rsp_timer, &timeLeft) != L7_SUCCESS)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Could not get the left out time on General Response Timer");
      return;
    }
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\ninterface_timer = %d\n",timeLeft);

  timeLeft = 0;
  if (mgmd_host_info->ver1_querier_timer != L7_NULLPTR)
  {
    if (appTimerTimeLeftGet(mgmdCB->timerHandle, mgmd_host_info->ver1_querier_timer, &timeLeft) != L7_SUCCESS)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Could not get the left out time on Version 1 Timer");
      return;
    }
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "V1 version tmr = %d\n",timeLeft);

  timeLeft = 0;
  if (mgmd_host_info->ver2_querier_timer != L7_NULLPTR)
  {
    if (appTimerTimeLeftGet(mgmdCB->timerHandle, mgmd_host_info->ver2_querier_timer, &timeLeft) != L7_SUCCESS)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Could not get the left out time on Version 2 Timer");
      return;
    }
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "V2 version tmr = %d\n",timeLeft);

}

/*****************************************************************
* @purpose Debug routine to display groups registered in MGMG-Proxy 
*
* @param   family            @b{ (input) } Family type
* @param   count             @b{ (input) } number of entries to be displayed
*
* @returns None
*
* @notes 
* @end
*********************************************************************/
void mgmdProxyDebugGroupsShow(L7_uint32 family, L7_uint32 count)
{
  L7_uint32                    now, count1 = 0;
  mgmd_host_info_t            *mgmd_host_info = L7_NULL;
  mgmd_host_group_t           *mgmd_host_group = L7_NULL;
  mgmd_host_source_record_t   *srcRec = L7_NULL;
  mgmd_cb_t                   *mgmdCB = L7_NULLPTR;
  mgmd_host_group_t            dummyGroup;
  L7_uint32                    timeLeft;
  L7_uchar8                    addrStr[IPV6_DISP_ADDR_LEN];

  now = osapiUpTimeRaw();
  if (mgmdMapProtocolCtrlBlockGet(family, 
                                  (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed to access control block\n");
    return;
  }
  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (mgmd_host_info == L7_NULL)
    return;

  memset(&dummyGroup, 0, sizeof(mgmd_host_group_t));

  /* Get first entry in the tree */
  mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7(&(mgmd_host_info->hostMembership), (void *)(&dummyGroup), AVL_NEXT);

  while ((mgmd_host_group != L7_NULLPTR) && ((count != L7_NULL) && (count1++ < count)))
  {
    inetAddrHtop(&(mgmd_host_group->group), addrStr);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "group addr:        %s\n", addrStr);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "mode = %d\n", mgmd_host_group->filterMode);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "sourcecount = %d  ",mgmd_host_group->sourceRecords.sllNumElements);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "state = %d \n",mgmd_host_group->hostState);
    if ((mgmd_host_group->sourceRecords.sllNumElements) != 0)
    {
      for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(mgmd_host_group->sourceRecords)); srcRec != L7_NULL; 
          srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(mgmd_host_group->sourceRecords), (L7_sll_member_t *)srcRec))
      {
        inetAddrHtop(&(srcRec->sourceAddress), addrStr);
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\tsourceAddress = %s\n", addrStr);
      }
    }
    if (mgmd_host_group->grp_rsp_timer != L7_NULLPTR)
    {
      if (appTimerTimeLeftGet(mgmdCB->timerHandle, mgmd_host_group->grp_rsp_timer, &timeLeft) != L7_SUCCESS)
      {
         MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not get the left out time \n");
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Could not get the left out time on Group Response Timer");
        return;
      }
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "grp_timer = %d  ",timeLeft);
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n");
    /* Get next entry in the tree */
    mgmd_host_group = (mgmd_host_group_t *)avlSearchLVL7(&(mgmd_host_info->hostMembership), (void *)(mgmd_host_group), AVL_NEXT);
  }
}


/*****************************************************************
* @purpose  Debug routine. It displays the Proxy Membership Table.
*
* @param   family    @b{ (input) } Family type
* @param   count     @b{ (input) } number of entries to be displayed
*
* @returns  None
*
* @notes  
*
* @end
*********************************************************************/
void mgmdProxyDebugSATShow(L7_uint32 family, L7_uint32 count)
{
  mgmd_host_group_t         *group_next = L7_NULL;
  mgmd_host_source_record_t *srcRec = L7_NULL;
  L7_uint32                  timeNow, count1 = 0;
  mgmd_cb_t                 *mgmdCB = L7_NULLPTR;
  mgmd_host_group_t          dummyGroup;
  L7_uint32                  timeLeft = 0;
  L7_uchar8                  addrStr[IPV6_DISP_ADDR_LEN];

  timeNow = osapiUpTimeRaw();
  if (mgmdMapProtocolCtrlBlockGet(family, 
                                  (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed to access control block\n");
    return;
  }
  if ((mgmdCB == L7_NULL) || (mgmdCB->mgmd_host_info == L7_NULL))
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n ERROR: Proxy_Info_Pointer NULL. \n");
    return;
  }

  /* LOCK to Proxy membership-table. */
  if (osapiSemaTake(mgmdCB->mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Could not take the Semaphore.\n");
    return;
  }

  memset(&dummyGroup, 0, sizeof(mgmd_host_group_t));

  /* Get first entry in the tree */
  group_next = (mgmd_host_group_t *)avlSearchLVL7(&(mgmdCB->mgmd_host_info->hostMembership), (void *)(&dummyGroup), AVL_NEXT);
  while ((group_next != L7_NULLPTR) && ((count != L7_NULL) && (count1++ < count))) 
  {
    inetAddrHtop(&(group_next->group), addrStr);
    /* group address */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n\n Group Addr: %s",addrStr);  
    /* time received */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n UpTime: %ld", group_next->uptime);      
    /*group address*/
    inetAddrHtop(&(group_next->last_reporter), addrStr);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Last-Reporter: %s", addrStr);

    /* Source Filter Mode */
    switch (group_next->filterMode)
    {
      case MGMD_FILTER_MODE_INCLUDE: SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Filter_Mode: INCLUDE ");
        break;
      case MGMD_FILTER_MODE_EXCLUDE: SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Filter_Mode: Exclude ");
        break;
      default: SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Filter_Mode: %d (Invalid) ",group_next->filterMode);
    }/*End-of-Switch*/
    /* Timer respond to group queries */
    if ( group_next->grp_rsp_timer != L7_NULLPTR)
    {
      if (appTimerTimeLeftGet(mgmdCB->timerHandle, group_next->grp_rsp_timer, &timeLeft) != L7_SUCCESS)
      {
        MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, "Could not get the left out time \n");
      }
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Grp-Rsp-Timer: %d ", timeLeft);

    /* Host State */
    switch (group_next->hostState)
    {
      case MGMD_HOST_NON_MEMBER: SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Host State: NON_MEMBER ");
        break;
      case MGMD_HOST_DELAY_MEMBER: SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Host State: DELAY_MEMBER ");
        break;
      case MGMD_HOST_IDLE_MEMBER: SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Host State: IDLE_MEMBER ");
        break; 
      default: SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Host State: %d (Invalid) ",group_next->hostState);
    }/*End-of-Switch*/
    /* Count of (GPL) Mask of interfaces configured for this Group Address */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, " GPL Count: %d ", 
                  mgmdProxyDebugBitsetCountGet(&group_next->grpPortList));
    /* (GPL) Mask of interfaces configured for this Group Address */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Group Port List: < ");
    mgmdProxyDebugBitsetShow (&group_next->grpPortList);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, " > ");

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n SourceCount = %d",group_next->sourceRecords.sllNumElements);
    /* list of source records for each group */
    for (srcRec = (mgmd_host_source_record_t *)SLLFirstGet(&(group_next->sourceRecords)); srcRec != L7_NULL; 
        srcRec = (mgmd_host_source_record_t *)SLLNextGet(&(group_next->sourceRecords), (L7_sll_member_t *)srcRec))
    {
      /* source Address */
      inetAddrHtop(&(srcRec->sourceAddress), addrStr);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n  Source Addr: %s",addrStr);
      /* Create Flag */
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Create Flag: %s", 
                    ((srcRec->createFlag == L7_TRUE) ? "TRUE":"FALSE"));
      /* time received */
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n  UpTime: %ld", srcRec->sourceCtime);        
      if (srcRec->sourceInterval != 0)
      {
        /* timeout Interval */
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n  Source Interval: %d ", srcRec->sourceInterval);    
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n  Expire time    : %ld ", 
                      srcRec->sourceCtime + srcRec->sourceInterval - timeNow);
      }
      /*Count of (SPL)Mask of interfaces configured for this Source Address*/
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, " SPL Count: %d ", 
                    mgmdProxyDebugBitsetCountGet(&srcRec->srcPortList));
      /* (SPL) Mask of interfaces configured for this Source Address */
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n  Source Port List: < ");
      mgmdProxyDebugBitsetShow (&srcRec->srcPortList);  
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, " > ");
      /* Send this Source in the Query Report */
      if (srcRec->sendQueryReport == L7_TRUE)
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n  SendQueryReport: TRUE \n");
      }
      else
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n  SendQueryReport: FALSE \n");
      }
    }  /*End-of-for*/

    /* Get next entry in the tree */
    group_next = (mgmd_host_group_t *)avlSearchLVL7(&(mgmdCB->mgmd_host_info->hostMembership), (void *)(group_next), AVL_NEXT);
  }/*End-of-While*/

  /* UNLOCK Proxy membership-table. */
  if (osapiSemaGive(mgmdCB->mgmd_host_info->hostMembership.semId) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Could not give the semaphore.\n");
    return;
  }

}
/*********************************************************************
*
* @purpose  Dumps MGMD Proxy MRT Table
*
* @param   family       @b{ (input) } IP family for ctrl block
* @param   count        @b{ (input) } number of entries to be displayed
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t mgmdProxyDebugMRTTableShow(L7_uchar8 family, L7_uint32 count)
{
  mgmdProxyCacheEntry_t  *mrtEntry;
  L7_uchar8              addrBuf[IPV6_DISP_ADDR_LEN];
  mgmd_cb_t              *mgmdCB;
  L7_uint32              count1 = 0;
  L7_uint32              entryCount = 0;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  if (mgmdMapProtocolCtrlBlockGet(family, 
                                  (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    MGMD_PRINTF ("Failed to access control block\n");
    return L7_FAILURE;
  }

  if (mgmdCB->mgmd_host_info == L7_NULLPTR)
  {
    MGMD_PRINTF ("Failed to access MGMD-Proxy Control Block\n");
    return L7_FAILURE;
  }

  if ((entryCount = avlTreeCount (&(mgmdCB->mgmd_host_info->mgmdProxyCache))) == 0)
  {
    MGMD_PRINTF ("MGMD-Proxy MRT Table is Empty\n");
    return L7_FAILURE;
  }

  MGMD_PRINTF (" \n **************** PROXY MRT TABLE *********************\n");
  MGMD_PRINTF (" \n Entry Count : %d\n", entryCount);
  MGMD_PRINTF (" \n Source    Group    Incoming-If    Outgoing-If    Up-Time \n"); 
  MGMD_PRINTF (" ------------------------------------------------------------------\n");
  mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,L7_NULLPTR,L7_NULLPTR);
  while ((mrtEntry != L7_NULLPTR) && ((count != L7_NULL) && (count1++ < count)))
  {
    MGMD_PRINTF("  %s ", inetAddrPrint(&mrtEntry->srcAddr, addrBuf));
    MGMD_PRINTF("  %s ", inetAddrPrint(&mrtEntry->grpAddr, addrBuf));
    MGMD_PRINTF("  %d            ", mrtEntry->upstrmRtrIfNum);
    MCAST_BITX_DEBUG_PRINT(mrtEntry->oifList);
    MGMD_PRINTF("       %d \n", (osapiUpTimeRaw()) - (mrtEntry->entryUpTime));


    mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,&mrtEntry->srcAddr,&mrtEntry->grpAddr);
  }
  return L7_SUCCESS;

}
/*********************************************************************
*
* @purpose  Dumps MGMD Proxy Admin scope boundary Table
*
* @param   family       @b{ (input) } IP family for ctrl block
* @param   count        @b{ (input) } number of entries to be displayed
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t mgmdProxyDebugAdminScopeTableShow(L7_uchar8 family, L7_uint32 count)
{
  mgmdProxyASBNode_t  *adminScopeEntry = L7_NULLPTR;
  L7_uchar8           addrBuf[IPV6_DISP_ADDR_LEN];
  mgmd_cb_t           *mgmdCB;
  L7_uint32            count1 = 0;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  if (mgmdMapProtocolCtrlBlockGet(family, 
                                  (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    MGMD_PRINTF ("Failed to access control block\n");
    return L7_FAILURE;
  }


  MGMD_PRINTF (" \n **************** PROXY MRT TABLE *********************\n");
  MGMD_PRINTF (" \n  Group-Addr  Group-Mask   BufferPool-Id  Interface-Mask \n"); 
  MGMD_PRINTF (" ------------------------------------------------------------------\n"); 
  for (adminScopeEntry = (mgmdProxyASBNode_t *)SLLFirstGet(&(mgmdCB->mgmd_host_info->asbList));
       ((adminScopeEntry != L7_NULLPTR) && (count1 < count));
       adminScopeEntry = (mgmdProxyASBNode_t *)SLLNextGet(&(mgmdCB->mgmd_host_info->asbList),
                                                          (L7_sll_member_t *)adminScopeEntry))
  {
    MGMD_PRINTF("\n  %s ", inetAddrPrint(&adminScopeEntry->grpAddr, addrBuf));
    MGMD_PRINTF("  %s ", inetAddrPrint(&adminScopeEntry->grpMask, addrBuf));
    MGMD_PRINTF("  %d            ", adminScopeEntry->family);
    MCAST_BITX_DEBUG_PRINT(adminScopeEntry->intfMask);
  }
  return L7_SUCCESS;

}






