/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   mgmd_debug.c
*
* @purpose    Implements the Debug support functionality of the Multicast 
*             Group Membership Discovery (MGMD) module
*
* @component  Multicast Group Membership Discovery (MGMD)
*
* @comments   Derived from MFC component
*
* @create     April 25,2006
*
* @author     Kamlesh Agrawal
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
#include "l7_common.h"
#include "l3_addrdefs.h"
#include "l7_mgmdmap_include.h"
#include "mgmd.h"
#include "mgmd_proxy.h"
#include "mgmd_debug.h"

/**********************************************************************
                  Typedefs & Defines
***********************************************************************/
#define MGMD_NUM_FLAG_BYTES     ((MGMD_DEBUG_FLAG_LAST + 7) / 8)


/******************************************************************
                 Global Declarations
******************************************************************/
static L7_uchar8 debugFlags[MGMD_NUM_FLAG_BYTES];
static L7_BOOL   debugEnabled = L7_FALSE;

/*****************************************************************
    Function Definitions
******************************************************************/

/*********************************************************************
*
* @purpose  Enable Debug Tracing for the MGMD.
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
L7_RC_t mgmdDebugEnable(void)
{
  debugEnabled = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for the MGMD.
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
L7_RC_t mgmdDebugDisable(void)
{
  debugEnabled = L7_FALSE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of MGMD.
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
L7_RC_t mgmdDebugAllSet(void)
{
  memset(debugFlags, 0xFF, sizeof(debugFlags));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of MGMD.
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
L7_RC_t mgmdDebugAllReset(void)
{
  memset(debugFlags, 0, sizeof(debugFlags));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in MGMD.
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
L7_RC_t mgmdDebugFlagSet(MGMD_DEBUG_FLAGS_t flag)
{
  if(flag >= MGMD_DEBUG_FLAG_LAST)
    return L7_FAILURE;
  debugFlags[flag/8] |= (1 << (flag % 8));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in MGMD.
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
L7_RC_t mgmdDebugFlagReset(MGMD_DEBUG_FLAGS_t flag)
{
  if(flag >= MGMD_DEBUG_FLAG_LAST)
    return L7_FAILURE;
  debugFlags[flag/8] &= (~(1 << (flag % 8)));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in MGMD.
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
L7_BOOL mgmdDebugFlagCheck(MGMD_DEBUG_FLAGS_t dbg_type)
{
  if(debugEnabled != L7_TRUE)
    return L7_FALSE;
  if(dbg_type >= MGMD_DEBUG_FLAG_LAST)
    return L7_FALSE;
  if((debugFlags[dbg_type/8] & (1 << (dbg_type % 8))) != 0)
    return L7_TRUE;
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Shows the current MGMD Debug flag status.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
void mgmdDebugFlagShow(void)
{
  L7_uint32 i;

  if(L7_TRUE == debugEnabled)
  {
    sysapiPrintf(" MGMD Debugging : Enabled\n");
    for(i = 0;  i < MGMD_DEBUG_FLAG_LAST ; i ++)
    {
      if(L7_TRUE == mgmdDebugFlagCheck(i))
        sysapiPrintf("     debugFlag [%d] : %s\n", i , "Enabled");
      else
        sysapiPrintf("     debugFlag [%d] : %s\n", i , "Disabled");
    }
  }
  else
  {
    sysapiPrintf (" MGMD Debugging : Disabled\n");
  }
}

/*********************************************************************
*
* @purpose  Shows usage of the MGMD Debug utility
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
void mgmdDebugHelp(void)
{
  sysapiPrintf(" Use mgmdDebugEnable()/mgmdDebugDisable() to Enable/Disable Debug trace in MGMD\n");
  sysapiPrintf(" Use mgmdDebugFlagSet(flag)/mgmdDebugFlagReset(flag) to Enable/Disable specific functionality traces\n");
  sysapiPrintf(" Use mgmdDebugFlagAllSet()/mgmdDebugFlagAllReset() to Enable/Disable all traces\n");  
  sysapiPrintf("     Various Debug Trace flags and their definitions are as follows ;\n");
  sysapiPrintf("         0  -> Trace the complete Receive Data path\n");
  sysapiPrintf("         1  -> Trace the complete Transmission Data path\n");
  sysapiPrintf("         2  -> Trace all the EVENT generations and receptions\n");
  sysapiPrintf("         3  -> Trace all Timer activities\n");
  sysapiPrintf("         4  -> Trace all failures\n");
  sysapiPrintf("         5  -> Trace all the APIs invoked\n");
  sysapiPrintf(" mgmdDebugCBShow(family =1 or 2) \n");
  sysapiPrintf(" mgmdDebugGroupsShow(family, rtrIfNum) \n");
  sysapiPrintf(" mgmdDebugIntfShow(family, rtrIfNum) \n");

}

/*********************************************************************
* @purpose  Debug Routine to display the contents of MGMD CB
* @param    param  @b{ (input) } Timer Event Info Block
*
* @param    addrFamily @b{(input)} Address Family Identifier
*
* @returns  none
*
* @notes    This routine should be used only for Debugging purpose
*
* @end
*********************************************************************/
void mgmdDebugCBShow(L7_uchar8 family)
{
  mgmd_cb_t      *mgmdCB = L7_NULLPTR;
  L7_uint32       timeLeft = 0;
  L7_uchar8      src[IPV6_DISP_ADDR_LEN];

  if ((family != L7_AF_INET) && (family != L7_AF_INET6))
  {
    sysapiPrintf ("Incorrect family parameter\n");
    return;
  }

  if (mgmdMapProtocolCtrlBlockGet (family, (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    sysapiPrintf ("MGMD:Failed to access control block\n");
    return;
  }

  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n            *************** CB INFO ****************\n\n");
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Raw Info...\n");
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "mgmdCB - %p.\n", mgmdCB);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "proto - %d.\n", mgmdCB->proto);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "mgmd_querier_if_mask - %p.\n", mgmdCB->mgmd_querier_if_mask);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "mgmd_info - %p.\n", mgmdCB->mgmd_info);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "mgmdMembershipTreeHeap - %p.\n", mgmdCB->mgmdMembershipTreeHeap);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "mgmdMembershipDataHeap - %p.\n", mgmdCB->mgmdMembershipDataHeap);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "membership.type - %x.\n", (L7_uint32) mgmdCB->membership.type);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "mgmd_host_info - %p.\n", mgmdCB->mgmd_host_info);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "mgmd_proxy_status - %d.\n", mgmdCB->mgmd_proxy_status);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "mgmd_proxy_ifIndex - %d.\n", mgmdCB->mgmd_proxy_ifIndex);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "mgmd_proxy_restartCount - %d.\n", mgmdCB->mgmd_proxy_restartCount);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "sockfd - %d.\n", mgmdCB->sockfd);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "checkRtrAlert - %d.\n", mgmdCB->checkRtrAlert);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "check IP TOS - %d.\n", mgmdCB->checkForTOS);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "all_hosts - %s.\n", inetAddrPrint(&mgmdCB->all_hosts,src));
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "all_routers - %s.\n", inetAddrPrint(&mgmdCB->all_routers,src));
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "mgmd_mrp_timer - %p.\n", mgmdCB->mgmd_mrp_timer);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "mgmd_mrp_timer_handle - %d.\n", mgmdCB->mgmd_mrp_timer_handle);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "timerHandle - %p.\n", mgmdCB->timerHandle);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "handle_list - %p.\n", mgmdCB->handle_list);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "handleListMem - %p.\n", mgmdCB->handleListMem);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "querySrcList - %p.\n", mgmdCB->querySrcList);

  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Detailed Info...\n");
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n  IGMP Querier Interfaces :");
  MCAST_BITX_DEBUG_PRINT(mgmdCB->mgmd_querier_if_mask.bits);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n SockFd      = %d", mgmdCB->sockfd);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n chkRtrAlert = %d", mgmdCB->checkRtrAlert);
  if (mgmdCB->mgmd_mrp_timer != 0)
  {
    if (appTimerTimeLeftGet(mgmdCB->timerHandle,
                            mgmdCB->mgmd_mrp_timer, &timeLeft) != L7_SUCCESS)
    {
      SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Failed to get left over time of query timer");
      timeLeft = 0;
    }
  }
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n MRP Expire Time :   %ld\n",timeLeft);

  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n ----  PROXY --- \n");
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Status        = %d", mgmdCB->mgmd_proxy_status);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Proxy Index   = %d", mgmdCB->mgmd_proxy_ifIndex);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n RestartCnt    = %d", mgmdCB->mgmd_proxy_restartCount);

  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n ----  HANDLE LIST --- \n");
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n MAX HANDLES   = %d", MGMD_MAX_TIMERS); 
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n HANDLE LIST   = %x", mgmdCB->handle_list); 
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n HANDLE LIST SIZE = %d", mgmdCB->handle_list->size);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n CURRENT HANDLE LIST SIZE(mgmd) = %d", mgmdCB->handle_list->numAllocated);
  SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n            *************** END ****************\n");
}


/*********************************************************************
* @purpose  
* @param    param  @b{ (input) } Timer Event Info Block
*
* @param    addrFamily @b{(input)} Address Family Identifier
*
* @returns  none
*
* @notes    This routine should be used only for Debugging purpose
*
* @end
*********************************************************************/
void mgmdDebugTimersShow(L7_uchar8 family)
{
  mgmd_cb_t      *mgmdCB = L7_NULLPTR;

  if (family > L7_AF_INET6)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Incorrect family parameter\n");
    return;
  }

  if (mgmdMapProtocolCtrlBlockGet (family,
                                   (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Failed to access control block\n");
    return;
  }
  appTimerDebugShow(mgmdCB->timerHandle);
}
/*********************************************************************
* @purpose  DevShell Debug Command to show the groups
*
* @param    unit    @b{ (input) } Unit
* @param    slot    @b{ (input) } Slot
* @param    port    @b{ (input) } Port
* @param    family  @b{ (input) } Family (IPv4, IPv6)
*
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void mgmdDebugGroupsShow(L7_uint32 family,L7_uint32 rtrIfNum, L7_uint32 count)
{
  mgmd_info_t          *mgmdIntfGroupInfo;
  mgmd_group_t         *groupInfo;
  time_t                timeNow;
  mgmd_source_record_t *srcRec;
  mgmd_cb_t            *mgmdCB = L7_NULLPTR;
  mgmd_group_t          dummyGroup;
  L7_uint32             timeLeft;
  L7_uchar8             addrStr[32];
  L7_uint32 localcount=L7_NULL;  

  if (family > L7_AF_INET6)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Incorrect family parameter\n");
    return;
  }

  if (mgmdMapProtocolCtrlBlockGet (family,
                                   (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Failed to access control block\n");
    return;
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Total groups: %d\n\n", avlTreeCount(&(mgmdCB->membership)));

  timeNow = osapiUpTimeRaw();




  mgmdIntfGroupInfo = (mgmd_info_t*)&(mgmdCB->mgmd_info[rtrIfNum]);
  if (mgmdIntfGroupInfo == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Couldn't get intf group info from interface ptr, %d\n", rtrIfNum);
    return;
  }



  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "IGMP Group info for USPrtrIfNum: %d\n",rtrIfNum);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Number of groups: AVLTreeCount=%d, numOfGroups=%d\n\n",
                avlTreeCount(&(mgmdCB->membership)), mgmdIntfGroupInfo->numOfGroups);

  memset(&dummyGroup, 0, sizeof(mgmd_group_t));

  /* Get first entry in the tree */
  groupInfo = (mgmd_group_t *)avlSearchLVL7(&(mgmdCB->membership), (void *)(&dummyGroup), AVL_NEXT);

  while (groupInfo)
  {

    if((count != L7_NULL)&&(localcount >= count))
    {
      break;
    }

    localcount++;
    /* Group info */
    if (groupInfo->rtrIfNum != rtrIfNum)
    {
      groupInfo = (mgmd_group_t *)avlSearchLVL7(&(mgmdCB->membership), (void *)(groupInfo), AVL_NEXT);
      continue;
    }
    inetAddrHtop(&(groupInfo->group), addrStr);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "group addr:        %s\n", addrStr);
    inetAddrHtop(&(groupInfo->reporter), addrStr);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "last reporter:     %s\n", addrStr);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "ctime:             %ld\n", groupInfo->ctime);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "group interval:    %d\n", groupInfo->interval);
    if (groupInfo->groupTimer != 0)
    {
      if (appTimerTimeLeftGet(mgmdCB->timerHandle,
                              groupInfo->groupTimer, &timeLeft) != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_APIS, "Failed to get left over time of group timer");
        timeLeft = 0;
      }
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "  expire time:     %ld\n",timeLeft);
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "group compat mode: %d\n", groupInfo->groupCompatMode);

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "filter mode:       %d", groupInfo->filterMode);
    if (groupInfo->filterMode == MGMD_FILTER_MODE_INCLUDE)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, " (Include)\n");
    }
    else if (groupInfo->filterMode == MGMD_FILTER_MODE_EXCLUDE)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, " (Exclude)\n");
    }
    else
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, " (None)\n");
    }

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "v2 host present:   %d\n", groupInfo->v2HostPresentFlag);
    if (appTimerTimeLeftGet(mgmdCB->timerHandle, groupInfo->v2HostTimer, &timeLeft) == L7_SUCCESS && 
        groupInfo->v2HostPresentFlag == 1)
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "v2 expire time:    %d\n", timeLeft);

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "v1 host present:   %d\n", groupInfo->v1HostPresentFlag);
    if (appTimerTimeLeftGet(mgmdCB->timerHandle, groupInfo->v1HostTimer, &timeLeft) == L7_SUCCESS &&
        groupInfo->v1HostPresentFlag == 1)
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "v1 expire time:    %d\n", timeLeft);

    /* Source info */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Source Info:\n");
    for (srcRec = (mgmd_source_record_t *)SLLFirstGet(&(groupInfo->sourceRecords)); srcRec != L7_NULL;
        srcRec = (mgmd_source_record_t *)SLLNextGet(&(groupInfo->sourceRecords), (L7_sll_member_t *)srcRec))
    {
      inetAddrHtop(&(srcRec->sourceAddress), addrStr);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "  source addr: %s", addrStr);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "  ctime:%08ld", srcRec->sourceCtime);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "  interval:%4d", srcRec->sourceInterval);
      if (srcRec->sourceTimer != 0)
      {
        if (appTimerTimeLeftGet(mgmdCB->timerHandle,
                                srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
        {
          MGMD_DEBUG (MGMD_DEBUG_APIS, "Failed to get left over time of group timer");
          timeLeft = 0;
        }

        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "  expire time:%4ld", timeLeft);
      }
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n");
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n");

    /* Get the next entry in the AVL tree*/
    groupInfo = (mgmd_group_t *)avlSearchLVL7(&(mgmdCB->membership), (void *)(groupInfo), AVL_NEXT);
  }/*End-of-While*/
}



/*********************************************************************
* @purpose  DevShell Debug Command to show the interface info
*
* @param    family    @b{ (input) } Family (IPv4, IPv6)
* @param    rtrIfNum  @b{ (input) } Router interface number
*
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void mgmdDebugIntfShow(L7_uint32 family, L7_uint32 rtrIfNum)
{
  mgmd_info_t          *mgmdIntfGroupInfo;
  time_t                timeNow;
  mgmd_cb_t            *mgmdCB = L7_NULLPTR;
  L7_uint32             timeLeft;
  L7_uchar8             addrBuff[IPV6_DISP_ADDR_LEN];

  if (family > L7_AF_INET6)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Incorrect family parameter\n");
    return;
  }
  if ((rtrIfNum == 0) || (rtrIfNum >= MCAST_MAX_INTERFACES))
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Incorrect rtrIfNum\n");
    return;
  }

  if (mgmdMapProtocolCtrlBlockGet (family,
                                   (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Couldn't get Control Block for family = %d", family);
    return;
  }
  timeNow = osapiUpTimeRaw();


  mgmdIntfGroupInfo = (mgmd_info_t*)&(mgmdCB->mgmd_info[rtrIfNum]);
  if (mgmdIntfGroupInfo == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Couldn't get intf group info for interface ptr, USP: %d\n", rtrIfNum);
    return;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n *********** INTERFACE INFO ************** \n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n ---------- CONFIG INFO ----------- \n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n Version          : %d",
                mgmdIntfGroupInfo->intfConfig.version)
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n Robustness       : %d",
                mgmdIntfGroupInfo->intfConfig.robustness)
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n QueryInvl        : %d",
                mgmdIntfGroupInfo->intfConfig.queryInterval)
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n QueryMaxRespTime : %d",
                mgmdIntfGroupInfo->intfConfig.queryResponseInterval)
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n StartupQIntvl    : %d",
                mgmdIntfGroupInfo->intfConfig.startupQueryInterval)
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n StartupQcount    : %d",
                mgmdIntfGroupInfo->intfConfig.startupQueryCount)
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n LastMemQIntvl    : %d",
                mgmdIntfGroupInfo->intfConfig.lastMemQueryInterval)
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n LastMemQCount    : %d",
                mgmdIntfGroupInfo->intfConfig.lastMemQueryCount)

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n ---------------------------- \n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n ---------- DYNAMIC INFO ----------- \n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Querier IP : %s", 
                inetAddrPrint(&mgmdIntfGroupInfo->mgmd_querierIP,addrBuff));

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n I M QUERIER : %s",
                MCAST_BITX_TEST (mgmdIntfGroupInfo->flags.bits, IGMP_QUERIER) ? "TRUE" : "FALSE");
  
  if (mgmdIntfGroupInfo->mgmd_query_timer != 0)
  {
    if (appTimerTimeLeftGet(mgmdCB->timerHandle,
                            mgmdIntfGroupInfo->mgmd_query_timer, &timeLeft) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Failed to get left over time of query timer");
      timeLeft = 0;
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Query Expire Time :   %ld\n",timeLeft);
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Querier Query Interval : %d",
                mgmdIntfGroupInfo->querierQueryInterval );

  if (mgmdIntfGroupInfo->mgmd_querier_timer != 0)
  {
    if (appTimerTimeLeftGet(mgmdCB->timerHandle,
                            mgmdIntfGroupInfo->mgmd_querier_timer, &timeLeft) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Failed to get left over time of querier timer");
      timeLeft = 0;
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "\n Querier Expire Time :   %ld\n",timeLeft);
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n No. of Joins : %d",
                mgmdIntfGroupInfo->numOfJoins); 
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n No. of Groups : %d",
                mgmdIntfGroupInfo->numOfGroups); 
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n Wrong Version queries : %d",
                mgmdIntfGroupInfo->Wrongverqueries); 
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n ---------------------------- \n");
}


/*********************************************************************
* @purpose  DevShell Debug Command to show the interface statistics
*
* @param    family      @b{ (input) } Family (IPv4, IPv6)
* @param    rtrIfNum    @b{ (input) } router interface number
*
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void mgmdDebugIntfStatsShow(L7_uint32 family, L7_uint32 rtrIfNum)
{
  mgmd_cb_t *mgmdCB;
  L7_uint32 intfStats[MGMD_CTRL_PKT_MAX][MGMD_STATS_MAX];

  if (mgmdMapProtocolCtrlBlockGet (family,
                                   (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Couldn't get Control Block for family = %d", family);
    return;
  }
  if ((rtrIfNum == 0) || (rtrIfNum >= MCAST_MAX_INTERFACES))
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Incorrect rtrIfNum\n");
    return;
  }
  if (mgmdCB->mgmd_proxy_ifIndex == rtrIfNum)
  {
    memcpy(intfStats, (mgmdCB->mgmd_host_info->proxyIntfStats), 
           sizeof(intfStats));
  }
  else
  {

    memcpy(intfStats, (mgmdCB->mgmd_info[rtrIfNum].intfStats), 
           sizeof(intfStats));
  }

  if (family == L7_AF_INET)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n-------------------------------------------------------------------------");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n  Intf  Stat  v1Query   v2Query    v3Query   v1Report  v2Report v2Leave  v3Report");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n--------------------------------------------------------------------------");
  
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n %-5d :: RX :: %-8d  %-8d  %-8d  %-8d  %-8d  %-8d %-8d ", rtrIfNum,
                  intfStats[MGMD_QUERY_V1][MGMD_STATS_RX], intfStats[MGMD_QUERY_V2][MGMD_STATS_RX], intfStats[MGMD_QUERY_V3][MGMD_STATS_RX], intfStats[MGMD_REPORT_V1][MGMD_STATS_RX],
                  intfStats[MGMD_REPORT_V2][MGMD_STATS_RX], intfStats[MGMD_LEAVE_V2][MGMD_STATS_RX], intfStats[MGMD_REPORT_V3][MGMD_STATS_RX]);
  
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n       :: TX :: %-8d  %-8d  %-8d  %-8d  %-8d  %-8d ",
                  intfStats[MGMD_QUERY_V1][MGMD_STATS_TX], intfStats[MGMD_QUERY_V2][MGMD_STATS_TX], intfStats[MGMD_QUERY_V3][MGMD_STATS_TX], intfStats[MGMD_REPORT_V1][MGMD_STATS_TX],
                  intfStats[MGMD_REPORT_V2][MGMD_STATS_TX], intfStats[MGMD_LEAVE_V2][MGMD_STATS_TX], intfStats[MGMD_REPORT_V3][MGMD_STATS_TX]);
  
  }
  else if (family == L7_AF_INET6)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n-------------------------------------------------------------------------");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n  Intf  Stat  v1Query   v2Query   v1Report  v1Leave   v2Report ");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n--------------------------------------------------------------------------");
  
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n %-5d :: RX :: %-8d  %-8d  %-8d  %-8d  %-8d ",
                  intfStats[MGMD_QUERY_V2][MGMD_STATS_RX], intfStats[MGMD_QUERY_V3][MGMD_STATS_RX], rtrIfNum,
                  intfStats[MGMD_REPORT_V2][MGMD_STATS_RX], intfStats[MGMD_LEAVE_V2][MGMD_STATS_RX], intfStats[MGMD_REPORT_V3][MGMD_STATS_RX]);
  
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n       :: TX :: %-8d  %-8d  %-8d  %-8d  %-8d ",
                  intfStats[MGMD_QUERY_V2][MGMD_STATS_TX], intfStats[MGMD_QUERY_V3][MGMD_STATS_TX], 
                  intfStats[MGMD_REPORT_V2][MGMD_STATS_TX], intfStats[MGMD_LEAVE_V2][MGMD_STATS_TX], intfStats[MGMD_REPORT_V3][MGMD_STATS_TX]);
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n-------------------------------------------------------------");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n      isInc    isExc     toInc     toExc     Allow    Block");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n--------------------------------------------------------------");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n RX :: %-8d  %-8d  %-8d  %-8d  %-8d  %-8d ",
                intfStats[MGMD_REPORT_V3_ISINC][MGMD_STATS_RX], intfStats[MGMD_REPORT_V3_ISEXC][MGMD_STATS_RX], intfStats[MGMD_REPORT_V3_TOINC][MGMD_STATS_RX], 
                intfStats[MGMD_REPORT_V3_TOEXC][MGMD_STATS_RX], intfStats[MGMD_REPORT_V3_ALLOW][MGMD_STATS_RX], intfStats[MGMD_REPORT_V3_BLOCK][MGMD_STATS_RX]);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP,"\n TX :: %-8d  %-8d  %-8d  %-8d  %-8d  %-8d ",
               intfStats[MGMD_REPORT_V3_ISINC][MGMD_STATS_TX], intfStats[MGMD_REPORT_V3_ISEXC][MGMD_STATS_TX], intfStats[MGMD_REPORT_V3_TOINC][MGMD_STATS_TX], 
               intfStats[MGMD_REPORT_V3_TOEXC][MGMD_STATS_TX], intfStats[MGMD_REPORT_V3_ALLOW][MGMD_STATS_TX], intfStats[MGMD_REPORT_V3_BLOCK][MGMD_STATS_TX]);
}


/**********************************************************************
* @purpose  Debug Routine to enable IGMPv3 to validate 'IP Precedence of
*           received IGMPV3 packets and discard if not set'
*
* @param    addrFamily @b{(input)} Address Family Identifier
* @param    ipTOSCheck @b{(input)} L7_TRUE to enable checking TOS
*                                  L7_FALSE to accept IGMPv3 even without
*                                           IP Precdence equal to 0xC0.
*
* @returns  none
*
* @notes    This validation works only for IGMPv3
*
* @end
*********************************************************************/
L7_RC_t mgmdDebugIPPrecedenceCheckSet(L7_uint32 family, L7_BOOL ipTOSCheck)
{
  mgmd_cb_t *mgmdCB;

  if (family != L7_AF_INET)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Error! The setting is applicable only for IGMP (family -1).");
    return L7_FAILURE;
  }
  if (mgmdMapProtocolCtrlBlockGet (family,
                                   (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IGMPMAP, "Couldn't get Control Block for family = %d", family);
    return L7_FAILURE;
  }
  mgmdCB->checkForTOS = ipTOSCheck;
  return L7_SUCCESS;
}

