/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   mfc.c
*
* @purpose    Implements the Key/Main features of the Multicast
*             Forwarding Cache (MFC) module
*
* @component  Multicast Forwarding Cache (MFC)
*
* @comments   
            The logic of 
            1) Copy pEntry to tempEntry  in protected mode.
            2) operate on  tempEntry &
            3) update pEntry with tempEntry in protected mode
            is to solve LVL700064559.
*
* @create     10-Jan-06
*
* @author     ddevi
* @end
*
**********************************************************************/
/**********************************************************************
                  Includes
***********************************************************************/
#include "osapi.h"
#include "avl_api.h"
#include "log.h"
#include "sysapi.h"
#include "mcast_api.h"
#include "mfc_api.h"
#include "defaultconfig.h"
#include "l3_comm_structs.h"
#include "l3_mcast_defaultconfig.h"
#include "l3_mcast_commdefs.h"
#include "datatypes.h"
#include "sysnet_api.h"
#include "rtiprecv.h"
#include "rtip.h"
#include "mfc.h"
#include "mfc_map.h"
#include "mfc_rxtx.h"
#include "mfc_debug.h"
#include "l7_mcast_api.h"
#include "mcast_rtmbuf.h"
#include "mfc_v6.h"

#ifdef L7_NSF_PACKAGE
#include "mcast_ckpt.h"
#endif



/******************************************************************
                 Global Declarations
******************************************************************/
/* MFC Control Block */
mfcInfo_t             mfcInfo ;

/* MFC Registration List */
mfcRegisterList_t mfcRegisterList[L7_MRP_MAXIMUM];

/*****************************************************************
    Function Definitions
******************************************************************/
/*********************************************************************
*
* @purpose  Retrieve a cache entry from the internal table for the given
*           source and group information.
*
* @param    pSrc         @b{(input)}pointer to source IP address
* @param    pGroup       @b{(input)}pointer to the destination group IP address
* @param    searchMode   @b{(input)}mode of search : AVL_EXACT or AVL_NEXT
*
* @returns  pCacheEntry  pointer to an internal table entry on successful search
* @returns  L7_NULLPTR   if search failed
*
* @comments This function MUST be invoked from within a semapbore locked region.
*
* @end
*
*********************************************************************/
mfcCacheEntry_t *mfcEntryGet(L7_inet_addr_t *pSource, 
                                    L7_inet_addr_t *pGroup,
                                    L7_uint32 searchMode)
{
  mfcCacheEntry_t mfcCacheEntry;

  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry ");
  if ((pSource == L7_NULLPTR) || (pGroup == L7_NULLPTR))
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL source and group for extracting MFC entry"); 
    return L7_NULLPTR;
  }

  /* Retrieve the entry */
  memset(&mfcCacheEntry, 0, sizeof(mfcCacheEntry_t));
  inetCopy(&(mfcCacheEntry.source), pSource);
  inetCopy(&(mfcCacheEntry.group), pGroup);
  return((mfcCacheEntry_t *)avlSearchLVL7(&(mfcInfo.mfcAvlTree),
                                          (void *)(&mfcCacheEntry), searchMode));
}

/*********************************************************************
*
* @purpose  Compare two MFC Cache Entries.
*
* @param    pData1   @b{(input)}pointer to the first entry
* @param    pData2   @b{(input)}pointer to the second entry
* @param    size     @b{(input)}size of the key in each entry (ignored as of now).
*
* @returns  >0, if pData1 is greater than pData2.
* @returns  =0, if pData1 is equal to pData2.
* @returns  <0, if pData1 is less than pData2.
*
* @comments
*
* @end
*
*********************************************************************/
static 
L7_int32 mfcCacheEntryCompare(const void *pData1, const void *pData2, 
                              L7_uint32 size)
{
  mfcCacheEntry_t *pKey1 = (mfcCacheEntry_t *)pData1;
  mfcCacheEntry_t *pKey2 = (mfcCacheEntry_t *)pData2;
  register L7_int32 retVal = 0;

  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry , pData1 = %x , pData2 = %x, size = %d ",
            pData1, pData2, size);

  if ((pKey1 == L7_NULLPTR) || (pKey2 == L7_NULLPTR))
  {
    return 1;
  }
  MFC_DEBUG(MFC_DEBUG_APIS,"\n key1.family = %d , key2.family = %d", 
            pKey1->source.family,  pKey2->source.family);
  retVal = L7_INET_ADDR_COMPARE(&(pKey1->source), &(pKey2->source));
  if (retVal != 0)
  {
    return retVal;
  }
  return(L7_INET_ADDR_COMPARE(&(pKey1->group), &(pKey2->group)));
}

/*********************************************************************
*
* @purpose  Routine to enable Multicast Forwarding
*
* @param    None.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcEnable(L7_uchar8 family)
{
  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry ");

  /* Set the enable flag */
  if (family == L7_AF_INET)
  {
    mfcInfo.mfcV4Enabled = L7_TRUE;
  }
  else if (family == L7_AF_INET6)
  {
    mfcInfo.mfcV6Enabled = L7_TRUE;
  }
  /* TODO : Following three counters should be part seperate control
    block per family */
  /*mfcCountersUpdate (MFC_NUM_NEW_CACHE_ENTIRES, MFC_COUNTERS_RESET);
  mfcCountersUpdate (MFC_NUM_HW_CACHE_ENTRIES, MFC_COUNTERS_RESET);*/
  mfcCountersUpdate (MFC_NUM_FORWARDED_PKTS, MFC_COUNTERS_RESET);  
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Routine to disable Multicast Forwarding
*
* @param    None.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcDisable(L7_uchar8 family)
{
  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry ");

  /* If already disabled, nothing to do */
  if (mfcIsEnabled(family) != L7_TRUE)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," MFC already disabled for family=%d ", family);
    return L7_FAILURE;
  }
   /* Mark the module as disabled */
  /* Set the enable flag */
  if (family == L7_AF_INET)
  {
    mfcInfo.mfcV4Enabled = L7_FALSE;
  }
  else if (family == L7_AF_INET6)
  {
    mfcInfo.mfcV6Enabled = L7_FALSE;
  }
  /*mfcCountersUpdate (MFC_NUM_NEW_CACHE_ENTIRES, MFC_COUNTERS_RESET);*/
  mfcCountersUpdate (MFC_NUM_FORWARDED_PKTS, MFC_COUNTERS_RESET);  

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Handles Interface Mode changes
*
* @param    pIntfChange  @b{(input)}pointer to a structure containing
*                                   the Interface Mode Change information
*
* @returns  None.
*
* @comments
*
* @end
*
*********************************************************************/
static L7_RC_t mfcInterfaceStatusChangeProcess(mfcInterfaceChng_t *pIntfChange)
{
  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry , Interface = %d ", pIntfChange->rtrIfNum);

  /* Given data pointer is valid? */
  if (pIntfChange == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL interface info for status updation");
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n invalid input parameters ");
    return L7_FAILURE;
  }

  /* Update the hardware/IP stack with the mode change */
  if (mfcHwInterfaceStatusChange(pIntfChange) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"Failed to update the interface status to hardware/IP stack"
              "for rtrIfNum = %d", pIntfChange->rtrIfNum);  
    MFC_DEBUG(MFC_DEBUG_FAILURES,
              "\n failed to update the interface status to hardware/IP stack"
              "for rtrIfNum = %d", pIntfChange->rtrIfNum);
    return L7_FAILURE;
  }
  if (pIntfChange->mode == L7_ENABLE)
  {
    /* Do Nothing */
    /* If some action need to be taken, ADD it here */
  }
  else if (pIntfChange->mode == L7_DISABLE)
  {
    /* Do Nothing */
    /* If some action need to be taken, ADD it here */
  }
  else
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," Invalid interface Mode %d",pIntfChange->mode);  
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n Invalid interface Mode %d",
              pIntfChange->mode);
	  return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/***************************************************************
*
* @purpose   Function invoked by the timer to trigger processing of cache entry
*            time-outs.
*
* @param     none
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     This function would be called in the context of the timer.
*            Hence it is imperative that we offload the processing to
*            the MFC task.
*
* @end
*
*******************************************************************/
static L7_RC_t mfcEntryExpireTimerCallback(void)
{
  /* Post an event to Mcast thread itself */
  if (mcastMapProtocolMsgSend(MFC_ENTRY_EXPIRY_TIMER_EVENT,L7_NULLPTR, 0)
      != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"Failed to queue ENTRY_EXPIRE Timer event ");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/***************************************************************
*
* @purpose   Function invoked by the timer to trigger processing of
*            UP-Call time-outs. UP-Call time-outs are those instances where
*            no MRP has resolved a route entry (in response to a NO-CACHE event)
*            for a long time.
*
* @param     none
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     This function would be called in the context of the timer.
*            Hence it is imperative that we offload the processing to
*            the MFC task.
*
* @end
*
*******************************************************************/
static L7_RC_t mfcUpcallExpireTimerCallback(void)
{
  /* Post an event to Mcast itself */
  if (mcastMapProtocolMsgSend(MFC_UPCALL_EXPIRY_TIMER_EVENT,L7_NULLPTR, 0)
      != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"Failed to queue UPCALL_EXPIRE Timer event ");
    return L7_FAILURE;
  }
  return L7_SUCCESS;  
}

/***************************************************************
*
* @purpose  Start MFC Timers.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
*******************************************************************/
static L7_RC_t mfcTimersStart(void)
{
  /* the passed time is in milli seconds */
  osapiTimerAdd((void *)mfcEntryExpireTimerCallback, L7_NULL, L7_NULL,
                MFC_ENTRY_EXPIRE_TIMER_INTERVAL_MSECS, 
                &mfcInfo.mfcTimers.pExpireTimer);
  osapiTimerAdd ((void*)mfcUpcallExpireTimerCallback, L7_NULL, L7_NULL,
                 MFC_UPCALL_EXPIRE_TIMER_INTERVAL_MSECS, 
                 &mfcInfo.mfcTimers.pUpcallsTimer);

  MFC_DEBUG(MFC_DEBUG_TIMERS,"\n MFC Timers started ");
  return L7_SUCCESS;
}

/***************************************************************
*
* @purpose  Stop MFC Timers.
*
* @param    none
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     none
*
* @end
*
*******************************************************************/
static L7_RC_t mfcTimersStop(void)
{
  osapiTimerFree(mfcInfo.mfcTimers.pExpireTimer);
  osapiTimerFree (mfcInfo.mfcTimers.pUpcallsTimer);
  MFC_DEBUG(MFC_DEBUG_TIMERS,"\n MFC Timers stopped ");
  return L7_SUCCESS;
}

/***************************************************************
*
* @purpose   Handler function called to process MFC_TIMER_EXPIRE events
*
* @param     none
*
* @returns   none
*
* @comments  This function cycles through all the cache entries, checks
*            if they are in use ( either by cache or by hardware )and erases
*            them if they are not in use and their expiry time has elapsed.
*
* @end
*
*******************************************************************/
static void mfcEntryExpireTimerHandler(void)
{
  mfcEntry_t        mfcEntryInfo;
  mfcCacheEntry_t   entry, *pEntry = L7_NULLPTR,tempEntry;
  L7_uint32         now;
  L7_BOOL  inUseByHw;

  /* Cycle through all the entries in the cache table and process them */
  memset(&entry, 0, sizeof(mfcCacheEntry_t));
  inetAddressZeroSet(L7_AF_INET, &entry.source);
  inetAddressZeroSet(L7_AF_INET, &entry.group);

  while (1)
  {
    memset(&tempEntry,0,sizeof(mfcCacheEntry_t));
    /* Lock the table entry */
    if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS) 
    {
      MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to take MFC semaphore");
      return;
    }
    if (L7_NULLPTR == (pEntry = (mfcCacheEntry_t *)avlSearchLVL7(&(mfcInfo.mfcAvlTree),(void *)(&entry), AVL_NEXT)) ) 
    {
      osapiSemaGive(mfcInfo.mfcAvlTree.semId);
      break;
    }
    memcpy(&tempEntry,pEntry,sizeof(mfcCacheEntry_t));

    /*Give up the emaphore after copying mfc Entry into a temporary strcuture.This was the semaphore is held for a short period of time*/
    osapiSemaGive(mfcInfo.mfcAvlTree.semId);

    now = osapiUpTimeRaw();

    inetCopy(&(entry.source), &(tempEntry.source));
    inetCopy(&(entry.group), &(tempEntry.group));

    if (tempEntry.newRoute == L7_TRUE)
    {
      continue;
    }
    inUseByHw = mfcHwUseCheck(&(tempEntry.source), &(tempEntry.group),
                                      tempEntry.iif, tempEntry.mcastProtocol);

    if (tempEntry.inUseBySw == L7_TRUE)  /* Used by the MFC */
    {
      tempEntry.inUseBySw = L7_FALSE;
      tempEntry.expire = now + tempEntry.holdtime;
    }
    if (inUseByHw == L7_TRUE) /* Used by HW */
    {
      tempEntry.expire = now + tempEntry.holdtime;
    }
    if (tempEntry.expire > now)  /* Not expired yet */
    {
      if (mfcRegisterList[tempEntry.mcastProtocol].wrongIfRateLimitStarted == L7_TRUE)
      {
         /* Update the Hardware table */
         tempEntry.rpfAction = L7_FALSE;
         mfcHwEntryUpdate(&tempEntry, tempEntry.rpfAction);
         mfcRegisterList[tempEntry.mcastProtocol].wrongIfRateLimitStarted = L7_FALSE;
         MFC_DEBUG(MFC_DEBUG_RATE_LIMIT,"\n wrongIfRateLimit Stop :");
      }
      
      if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS) 
      {
        MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to take MFC semaphore");
        return;
      }
      pEntry = mfcEntryGet(&tempEntry.source, &tempEntry.group, AVL_EXACT);
      if (pEntry != L7_NULLPTR)
      {
        /* CAUTION: Don't memcpy tempEntry into pEntry
            copy only those params which are changed in tempEntry
            memcpy(pEntry,&tempEntry,sizeof(mfcCacheEntry_t));--REMOVED
         */
         pEntry->inUseBySw = tempEntry.inUseBySw;
         pEntry->expire = tempEntry.expire;
         pEntry->rpfAction = tempEntry.rpfAction;         
      }
      /* Give-up the semaphore before calling mfcEntryRemove and the notifications.
         Otherwise we could end-up with semaphore deadlocks */
      osapiSemaGive(mfcInfo.mfcAvlTree.semId);   
      continue;
    }

    /* Entry to be removed; so populate the notification structure */
    memset(&(mfcEntryInfo.oif),0,sizeof(interface_bitset_t));
    inetCopy(&(mfcEntryInfo.group), &(tempEntry.group));
    inetCopy(&(mfcEntryInfo.source), &(tempEntry.source));
    mfcEntryInfo.iif = tempEntry.iif; 
    mfcEntryInfo.mcastProtocol = tempEntry.mcastProtocol;
    mfcEntryInfo.m   = L7_NULLPTR;

    /* Remove the entry and also issue notifications */
    mfcEntryRemove(&mfcEntryInfo);
   
    mfcRegisteredUsersNotify(MCAST_MFC_ENTRY_EXPIRE_EVENT, sizeof(mfcEntry_t),
                        &mfcEntryInfo);

    MFC_DEBUG(MFC_DEBUG_TIMERS,"\n Mroute entry  expired of source :",
              &mfcEntryInfo.source);
    MFC_DEBUG(MFC_DEBUG_TIMERS,"\n Mroute entry  expired of group :",
              &mfcEntryInfo.group);
  }

  /* the passed time is in milli seconds */  
  osapiTimerAdd((void *)mfcEntryExpireTimerCallback, L7_NULL, L7_NULL, 
                MFC_ENTRY_EXPIRE_TIMER_INTERVAL_MSECS, 
                &mfcInfo.mfcTimers.pExpireTimer);  
}

/***************************************************************
*
* @purpose  Handler function called to process MFC_UPCALL_TIMER_EXPIRE events
*
* @param    none
*
* @returns  none
*
* @comments This function cycles through all the cache entries, checks
*           if they are new entries requiring route resolution and erases them
*           if the overlying MRPs have not resolved the forwarding paths for a
*           long time determined by the upcall expiry timer.
*
* @end
*
*******************************************************************/
static void mfcUpcallExpireTimerHandler(void)
{
  mfcCacheEntry_t  entry, *pEntry = L7_NULLPTR;
  L7_uint32        now;
  L7_uchar8        addrBufStr[IPV6_DISP_ADDR_LEN];

  /* Shortcut : If number of new route entries is 0, nothing to process */
  if (mfcCountersValueGet(MFC_NUM_NEW_CACHE_ENTIRES) <= 0)
  {
   MFC_DEBUG(MFC_DEBUG_TIMERS,"\n No newRoutes. Quiting mfcUpcallExpireTimerHandler()");

    osapiTimerAdd((void*)mfcUpcallExpireTimerCallback, L7_NULL, L7_NULL,
                  MFC_UPCALL_EXPIRE_TIMER_INTERVAL_MSECS, 
                  &mfcInfo.mfcTimers.pUpcallsTimer);
    return;
  }

  MFC_DEBUG(MFC_DEBUG_TIMERS,"\n Scanning MFC Table for newRoutes...");

  /* Lock the table */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to take MFC semaphore");
    return;
  }
  memset(&entry, 0, sizeof(mfcCacheEntry_t));
  inetAddressZeroSet(L7_AF_INET, &entry.source);
  inetAddressZeroSet(L7_AF_INET, &entry.group);
  now = osapiUpTimeRaw();

  /* Cycle through all the entries in the cache table and process them */
  while (L7_NULLPTR !=
         (pEntry = (mfcCacheEntry_t *)avlSearchLVL7(&(mfcInfo.mfcAvlTree), 
                                                    (void *)(&entry), AVL_NEXT)))
  {
    inetCopy(&(entry.source), &(pEntry->source));
    inetCopy(&(entry.group), &(pEntry->group));

    if ((pEntry->newRoute == L7_TRUE) && (pEntry->upcallExpire <= now))
    {
      MFC_DEBUG_ADDR(MFC_DEBUG_TIMERS,"\n Mroute entry upcall expired of source :",
                &pEntry->source);
      MFC_DEBUG_ADDR(MFC_DEBUG_TIMERS,"\n Mroute entry upcall expired of group :",
                &pEntry->group);
      /* Remove the entry */
      if (avlDeleteEntry(&(mfcInfo.mfcAvlTree), (void *)pEntry) == L7_NULLPTR)
      {
        MFC_LOG_MSG(0,"MFC Entry (src,grp)=(%s, %s) Delete from avlTree Failed",
                inetAddrPrint(&pEntry->source,addrBufStr),
                inetAddrPrint(&pEntry->group,addrBufStr)); 

      } 
      else 
      {
         mfcCountersUpdate (MFC_NUM_NEW_CACHE_ENTIRES, MFC_COUNTERS_DECREMENT);
      }
    }
  }

  osapiSemaGive(mfcInfo.mfcAvlTree.semId);
  MFC_DEBUG(MFC_DEBUG_TIMERS,"\n Scanning MFC Table completed");  
  /* the passed time is in milli seconds */  
  osapiTimerAdd((void*)mfcUpcallExpireTimerCallback, L7_NULL, L7_NULL,
                MFC_UPCALL_EXPIRE_TIMER_INTERVAL_MSECS,
                &mfcInfo.mfcTimers.pUpcallsTimer);
}

/***************************************************************
*
* @purpose   Function invoked by the timer controlling processing
*            during a warm restart.
*
* @param     none
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     This function would be called in the context of the timer.
*            Hence it is imperative that we offload the processing to
*            the MFC task.
*
* @end
*
*******************************************************************/
static L7_RC_t mfcWarmRestartTimerCallback(void)
{
  /* Post an event to Mcast thread itself */
  if (mcastMapProtocolMsgSend(MFC_WARM_RESTART_TIMER_EVENT, L7_NULLPTR, 0)
      != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"Failed to queue WARM RESTART Timer event ");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/***************************************************************
*
* @purpose  Handler function called to process MFC_WARM_RESTART_TIMER_EVENT events
*
* @param    none
*
* @returns  none
*
* @comments This function triggers the checkpoint subsystem to process
*           data checkpointed from a previous management unit during
*           the warm restart period.
*
* @end
*
*******************************************************************/
static void mfcWarmRestartTimerHandler(void)
{
#ifdef L7_NSF_PACKAGE
  MFC_DEBUG(MFC_DEBUG_TIMERS,"\nEntry...");

  /* check if finished with warm restart processing*/
  if (mfcInfo.mfcTimers.warmRestartRepeatCount < MFC_WARM_RESTART_TIMER_REPEAT_COUNT_LIMIT)
  {
    mfcInfo.mfcTimers.warmRestartRepeatCount++;
    /* call checkpointing code */
    mcastCkptWarmRestartTimerEvent();

    /* the passed time is in milli seconds */  
    osapiTimerAdd((void*)mfcWarmRestartTimerCallback, L7_NULL, L7_NULL,
                  MFC_WARM_RESTART_TIMER_PERIODIC_INTERVAL_MSECS,
                  &mfcInfo.mfcTimers.pWarmRestartTimer);
  }
  else
  {
    osapiTimerFree(mfcInfo.mfcTimers.pWarmRestartTimer);
    mfcInfo.mfcTimers.pWarmRestartTimer = L7_NULL;
    mfcInfo.mfcTimers.warmRestartRepeatCount = 0;
    /* finished with warm restart processing, initiate reconciliation */
    mcastMapWarmRestartPeriodComplete();
  }
  MFC_DEBUG(MFC_DEBUG_TIMERS,"\nCompleted");  
#else
  MFC_DEBUG(MFC_DEBUG_TIMERS,"\nInvoked in non-NSF build.");  
  return;
#endif
}

/***************************************************************
*
* @purpose  Start MFC NSF Timer
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
*******************************************************************/
L7_RC_t mfcWarmRestartTimerStart(void)
{
  /*
   * this can be called more than once as the RTR6_EVENT_STARTUP_DONE and L7_RTR_STARTUP_DONE
   * events are received, only set flag and start timer on the first call
  */
  if (mfcInfo.mfcTimers.pWarmRestartTimer == L7_NULL)
  {
    osapiTimerAdd((void *)mfcWarmRestartTimerCallback, L7_NULL, L7_NULL,
                  MFC_WARM_RESTART_TIMER_INITIAL_DELAY_INTERVAL_MSECS, 
                  &mfcInfo.mfcTimers.pWarmRestartTimer);
  }

  MFC_DEBUG(MFC_DEBUG_TIMERS,"\n MFC Warm Restart Timer started ");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Event processing for the  Multicast Forwarding
*           Cache module (received from mcastMap thread)
*
* @param    eventType   @b{(input)} Event.
* @param    pMsg        @b{(input)} Message.
*
* @returns  L7_SUCCESS, if successful.
* @returns  L7_FAILURE, if operation failed.
*
* @comments This function is never supposed to exit.
*
* @end
*
*********************************************************************/
L7_RC_t mfcEventProcess(mfcEventType_t eventType, void *pMsg)
{
  switch (eventType) /* Process each individual message */
  {
    case MFC_ENTRY_EXPIRY_TIMER_EVENT:
      {
        mfcEntryExpireTimerHandler();
      }
      break;
    case MFC_UPCALL_EXPIRY_TIMER_EVENT:
      {
        mfcUpcallExpireTimerHandler();
      }
      break;
    case MFC_WARM_RESTART_TIMER_EVENT:
      {
        mfcWarmRestartTimerHandler();
      }
      break;
    case MFC_IPV6_DATA_PKT_RECV_EVENT:
      {
        mfcIpv6PktRecv(pMsg);

        /* Deallocate the Packet receiver IPv6 data buffer */
        if (pMsg != L7_NULLPTR)
        {
          mcastV6DataBufferPoolFree(((mcastControlPkt_t*) pMsg)->payLoad);
        }
      }
      break;
    default: 
      {
        MFC_DEBUG(MFC_DEBUG_FAILURES,"Invalid eventtype = %d rceived", eventType);
        MFC_DEBUG(MFC_DEBUG_EVENTS, "\n Invalid eventType type %d.", eventType);
        MFC_DEBUG(MFC_DEBUG_FAILURES, "\n  Invalid eventType type %d.",eventType); 
        return L7_FAILURE;
      }
      /* passthru */
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Initialize the Multicast Forwarding Cache module - Phase 1
*
* @param    None.
*
* @returns  L7_SUCCESS, if successful.
* @returns  L7_FAILURE, if initialization failed.
*
* @comments Allocate all necesasry memory requirements for MFC
*
* @end
*
*********************************************************************/
L7_RC_t mfcCnfgrPhase1Init(void)
{
  MFC_DEBUG(MFC_DEBUG_APIS,"\n  Entry ");

  /* Initialize the module information */
  memset(mfcRegisterList, 0, L7_MRP_MAXIMUM * sizeof(mfcRegisterList_t));
  memset(&mfcInfo, 0, sizeof(mfcInfo_t));


  /* Initialize memory for the multicast forwarding cache */
  mfcInfo.pTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_MFC_COMPONENT_ID,
                                                     sizeof(avlTreeTables_t) * L7_MULTICAST_FIB_MAX_ENTRIES);
  mfcInfo.pDataHeap = (void *)osapiMalloc(L7_MFC_COMPONENT_ID, 
                                          sizeof(mfcCacheEntry_t) * L7_MULTICAST_FIB_MAX_ENTRIES);
  if ((mfcInfo.pTreeHeap == L7_NULLPTR) || (mfcInfo.pDataHeap == L7_NULLPTR))
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to create dataHeap or TreeHeap");
    return L7_FAILURE;
  }

  /* Initialize the Cache Table on an AVL tree */
  memset(&(mfcInfo.mfcAvlTree), 0, sizeof(avlTree_t));
  avlCreateAvlTree(&(mfcInfo.mfcAvlTree),   /* Pointer to the AVL Tree object */
                   mfcInfo.pTreeHeap,       /* Pointer to a AVL Tree Node/Entry heap */
                   mfcInfo.pDataHeap,       /* Pointer to a data heap */
                   L7_MULTICAST_FIB_MAX_ENTRIES,    /* Max. number of entries supported */
                   sizeof(mfcCacheEntry_t),  /* Size of the data portion of each entry */
                   MFC_AVL_TREE_TYPE,        /* Generic value for AVL Tree type */
                   sizeof(L7_inet_addr_t) * 2);       /* Length of the key - <S,G> */

  /* check AVL semaphore ID to estimate if create succeeded */
  if (mfcInfo.mfcAvlTree.semId == L7_NULLPTR)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to create the cache table object\n");
    return L7_FAILURE;
  }
  avlSetAvlTreeComparator(&(mfcInfo.mfcAvlTree), mfcCacheEntryCompare);


  /* Initialize the Data Reception/Transmission Engine */
  if (mfcIpv6SockInit() != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," Failed to initialize the IPv6 Data Reception Engine\n");
    return L7_FAILURE;
  }

  /* Start the MFC Update timers */
  if (mfcTimersStart() != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," Failed to start MFC Timers\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Initialize the Multicast Forwarding Cache module - Phase 2
*
* @param    None.
*
* @returns  L7_SUCCESS, if successful.
* @returns  L7_FAILURE, if initialization failed.
*
* @comments Register callbacks with all necesary components
*
* @end
*
*********************************************************************/
L7_RC_t mfcCnfgrPhase2Init(void)
{
  if (mfcIpv6SysnetPduInterceptRegister() != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"Failed to register to sysnet to intercept IPv6 data pkts");
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Initialize the Multicast Forwarding Cache module - Phase 3
*
* @param    None.
*
* @returns  L7_SUCCESS, if successful.
* @returns  L7_FAILURE, if initialization failed.
*
* @comments Enable Timerss
*
* @end
*
*********************************************************************/
L7_RC_t mfcCnfgrPhase3Init(void)
{
  MFC_DEBUG(MFC_DEBUG_APIS,"\n  Entry ");


  MFC_DEBUG(MFC_DEBUG_APIS,"\n  Multicast Forwarding Cache is up\n");
  mfcCountersUpdate (MFC_NUM_NEW_CACHE_ENTIRES, MFC_COUNTERS_RESET);
  mfcCountersUpdate (MFC_NUM_HW_CACHE_ENTRIES, MFC_COUNTERS_RESET);
  mfcCountersUpdate (MFC_NUM_FORWARDED_PKTS, MFC_COUNTERS_RESET);  
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  DeInitialize the Multicast Forwarding Cache module - Phase 1
*
* @param    None.
*
* @returns  L7_SUCCESS, if successful.
* @returns  L7_FAILURE, if initialization failed.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcCnfgrPhase1InitUndo(void)
{
  if (mfcInfo.pTreeHeap != L7_NULLPTR)
  {
    osapiFree(L7_MFC_COMPONENT_ID, mfcInfo.pTreeHeap);
    mfcInfo.pTreeHeap = L7_NULLPTR;  
  }
  if (mfcInfo.pDataHeap != L7_NULLPTR)
  {
    osapiFree(L7_MFC_COMPONENT_ID, mfcInfo.pDataHeap);
    mfcInfo.pDataHeap = L7_NULLPTR;  
  }
  if (mfcInfo.mfcAvlTree.semId != L7_NULLPTR)
  {
    avlDeleteAvlTree(&(mfcInfo.mfcAvlTree));
  }
  /* check for socket already initialized or not, is done inside the call */
  mfcIpv6SockDeInit();
  
  if (mfcTimersStop() != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," Failed to stop MFC Timers\n");
  }  
  mfcInfo.mfcV4Enabled = L7_FALSE;
  mfcInfo.mfcV6Enabled = L7_FALSE;
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  DeInitialize the Multicast Forwarding Cache module - Phase 2
*
* @param    None.
*
* @returns  L7_SUCCESS, if successful.
* @returns  L7_FAILURE, if initialization failed.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcCnfgrPhase2InitUndo(void)
{
  if (mfcIpv6SysnetPduInterceptDeregister() != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"Failed to deregister to sysnet to intercept IPv6 data pkts\n");
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  DeInitialize the Multicast Forwarding Cache module - Phase 3
*
* @param    None.
*
* @returns  L7_SUCCESS, if successful.
* @returns  L7_FAILURE, if initialization failed.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcCnfgrPhase3InitUndo(void)
{
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Adds a new entry into the Multicast Forwarding Cache.
*
* @param    pData    @b{(input)}pointer to data containing the entry info.
*
* @returns  L7_SUCCESS, if successful.
* @returns  L7_FAILURE, if operation failed.
*
* @comments    Caution : This function assumes that the cache table
*                     semaphore is already taken and that no
*                     simultaneous access to the cache table is
*                     possible. All future callers of this function
*                     MUST ensure the above condition, failing which,
*                     all crazy things might happen!
*
* @end
*
*********************************************************************/
static L7_RC_t mfcEntryAdd(mfcEntry_t *pData)
{
  mfcCacheEntry_t  entry;
  L7_uint32        now;
  L7_uchar8        addrBuffStr[IPV6_DISP_ADDR_LEN];
  static L7_uint32 cpuIntIfNum = 0;

 /* Given data pointer is valid? */
  if (pData == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL mroute info for MFC entry addition");
    return L7_FAILURE;
  }

  if (pData->iif < 1 ||  pData->iif  >= MAX_INTERFACES )
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," invalid input interface number = %d", pData->iif);
    return L7_FAILURE;
  }

  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry , source : ",&pData->source);
  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry , group : ",&pData->group);

  /* Create a temporary cache entry to insert into the cache table */
  now = osapiUpTimeRaw();
  memset(&entry, 0, sizeof(mfcCacheEntry_t));
  inetCopy(&(entry.source), &(pData->source));
  inetCopy(&(entry.group), &(pData->group));
  entry.iif = pData->iif;
  if (pData->m != L7_NULLPTR)
  {
    /* Store the received Physical interface number and pass it on to the DTL 
     * Add "dtlMulticastAddrAdd()" call when updating entries.
     */
    entry.rxPort = pData->m->rtm_pkthdr.rxPort;
  }
  if (entry.rxPort == 0)
  {
    /* If we don't know the port, use the CPU interface number, to 
       guarantee an RPF failure when we do. */
    if (cpuIntIfNum == 0)
    {
      NIM_INTF_MASK_t cpuIntfMask;

      if (nimCpuIntfMaskGet(&cpuIntfMask) == L7_SUCCESS)
      {	
	NIM_INTF_FHMASKBIT(cpuIntfMask, cpuIntIfNum);
      }
    }
    entry.rxPort = cpuIntIfNum;
  }
  BITX_COPY(&(pData->oif),&(entry.oif));
  entry.mcastProtocol = pData->mcastProtocol;
  entry.dataTTL = pData->dataTTL;
  entry.newRoute = L7_TRUE;
  entry.addedToHwTable = L7_FALSE;
  entry.holdtime = L7_MCAST_FORWARDING_ENTRY_LIFETIME;
  entry.ctime = now;
  entry.expire = entry.ctime + entry.holdtime;
  entry.upcallExpire = entry.ctime + L7_MCAST_FORWARDING_UPCALL_LIFETIME;
  /*entry.numNoCacheEvents++;
  entry.numWholePktEvents++;*/

  if (avlInsertEntry(&(mfcInfo.mfcAvlTree), (void *)(&entry)) != L7_NULLPTR)
  {
    /* Error inserting a new entry : maybe duplicate exists in cache */
    MFC_DEBUG(MFC_DEBUG_FAILURES," Failed to insert entry into MFC avlTree for (src, grp) = (%s, %s)",
                inetAddrPrint(&entry.source, addrBuffStr),
                inetAddrPrint(&entry.group, addrBuffStr));
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n Failed to insert entry into MFC avlTree ");
    return L7_FAILURE;
  }

  /* Cache table addition successful. Return */
  mfcCountersUpdate (MFC_NUM_NEW_CACHE_ENTIRES, MFC_COUNTERS_INCREMENT);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Update the Multicast Forwarding Cache entry with resolved outgoing 
*           interface list from MRP.
*
* @param    pData   @b{(input)}pointer to data containing the updated info.
*
* @returns  L7_SUCCESS, if successful.
* @returns  L7_FAILURE, if operation failed.
*
* @comments This  routine runs under MRP context, the resolved mroute entry 
*           is updated both in cache as well in hardware.If the resolved entry
*           has any queued packets, the packets are forwarded based on the
*           updated outgoing interface list.
*
* @end
*
*********************************************************************/
L7_RC_t mfcEntryUpdate(mfcEntry_t *pData)
{
  mfcCacheEntry_t  *pEntry = L7_NULLPTR,tempEntry;
  L7_RC_t           retVal = L7_FAILURE;
  L7_RC_t           hwUpdation = L7_FAILURE;

  /* Given data pointer is valid? */
  if (pData == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL mroute info for MFC entry updation");
    return L7_FAILURE;
  }

  if (pData->iif < 1 ||  pData->iif  >= MAX_INTERFACES )
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," invalid input interface number = %d", pData->iif);
    return L7_FAILURE;
  }

  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry , source : ",&pData->source);
  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry , group : ",&pData->group);
  MFC_DEBUG(MFC_DEBUG_APIS, "\n iif = %d", pData->iif);

  memset(&tempEntry,0,sizeof(mfcCacheEntry_t));

  /* Lock the Table */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to take MFC semaphore");
    return L7_FAILURE;
  }

  /* Retrieve the associated cache entry */
  pEntry = mfcEntryGet(&(pData->source), &(pData->group), AVL_EXACT);

  /* When this mfcEntryUpdate function is called by MRP,if the mfc entry is not there either 
     because the entry itself not got created at all or entry was created but it got deleted 
     because of the mfc timer expiry, then for the corresponding source and group entry should 
     not be created here.Mfc entry creation trigger should always be the first data packet arrival
     only.Creation of entry here if the entry is not found raised lot of problems for PIMSM.
   */
  if (pEntry == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n No Entry So return from here ");  
    osapiSemaGive(mfcInfo.mfcAvlTree.semId);
    return L7_SUCCESS;
  }

  memcpy(&tempEntry,pEntry,sizeof(mfcCacheEntry_t));

  /*Give up the semaphore after copying mfc Entry into a temporary strcuture.This was the semaphore is held for a short period of time*/
  osapiSemaGive(mfcInfo.mfcAvlTree.semId);

  if ((tempEntry.iif == pData->iif) &&
      (memcmp(&tempEntry.oif, &pData->oif,sizeof(interface_bitset_t)) == 0)&&
      (tempEntry.mcastProtocol == pData->mcastProtocol))
  {
    MFC_DEBUG (MFC_DEBUG_EVENTS, "Not allowing Duplicate Entry Addition to the H/W");
    MFC_DEBUG_ADDR (MFC_DEBUG_EVENTS, "Source :%", &pData->source);
    MFC_DEBUG_ADDR (MFC_DEBUG_EVENTS, "Group  : ", &pData->group);
    return L7_SUCCESS;
  }

  /* Below Block is added only for Debugging */
  if (mfcDebugFlagCheck (MFC_DEBUG_MRP_UPDATES) == L7_TRUE)
  {
    L7_uchar8 src[IPV6_DISP_ADDR_LEN];
    L7_uchar8 grp[IPV6_DISP_ADDR_LEN];
    L7_BOOL oifIsEmpty = L7_TRUE;
    L7_uint32 i=0;

    MFC_DEBUG_PRINTF ("\n");
    MFC_DEBUG_PRINTF ("\n----------%s->MFC ... UPDATE----------\n", ((pData->mcastProtocol == 1) ? ("DVMRP") :
                                                  ((pData->mcastProtocol == 2) ? ("PIM-DM") :
                                                  ((pData->mcastProtocol == 3) ? ("PIM-SM") :
                                                  ((pData->mcastProtocol == 4) ? ("IGMP-PROXY") : ("Unknown"))))));
    MFC_DEBUG_PRINTF ("OLD: ");
    MFC_DEBUG_PRINTF ("S- %s, ", inetAddrPrint (&tempEntry.source, src));
    MFC_DEBUG_PRINTF ("G- %s, ", inetAddrPrint (&tempEntry.group, grp));
    MFC_DEBUG_PRINTF ("I- %d, ", tempEntry.iif);
    MFC_DEBUG_PRINTF ("O- ");
    BITX_IS_EMPTY (&tempEntry.oif, oifIsEmpty);
    if (oifIsEmpty == L7_TRUE)
    {
      MFC_DEBUG_PRINTF ("None");
    }
    else
    {
      for (i = 0; i < MCAST_MAX_INTERFACES; i++)
      {
        if (BITX_TEST(&tempEntry.oif, i) != L7_NULL)
          MFC_DEBUG_PRINTF ("%d ", i);
      }
    }
    MFC_DEBUG_PRINTF (".\n");

    MFC_DEBUG_PRINTF ("NEW: ");
    MFC_DEBUG_PRINTF ("S- %s, ", inetAddrPrint (&pData->source, src));
    MFC_DEBUG_PRINTF ("G- %s, ", inetAddrPrint (&pData->group, grp));
    MFC_DEBUG_PRINTF ("I- %d, ", pData->iif);
    MFC_DEBUG_PRINTF ("O- ");
    BITX_IS_EMPTY (&pData->oif, oifIsEmpty);
    if (oifIsEmpty == L7_TRUE)
    {
      MFC_DEBUG_PRINTF ("None");
    }
    else
    {
      for (i = 0; i < MCAST_MAX_INTERFACES; i++)
      {
        if (BITX_TEST(&pData->oif, i) != L7_NULL)
          MFC_DEBUG_PRINTF ("%d ", i);
      }
    }
    MFC_DEBUG_PRINTF (".\n");
  }

  /* If a matching cache entry is found, update the various
     parameters as per the given information */
  tempEntry.iif = pData->iif;
  BITX_COPY(&(pData->oif), &(tempEntry.oif));
  tempEntry.mcastProtocol = pData->mcastProtocol;
  tempEntry.rpfAction = L7_FALSE;

  /*Update the Hardware table entries */
  if (mfcHwEntryUpdate(&tempEntry, tempEntry.rpfAction) == L7_SUCCESS)
  {
    hwUpdation = L7_SUCCESS;
    retVal = L7_SUCCESS;
  }
  else
  {
    MFC_DEBUG_ADDR(MFC_DEBUG_FAILURES,
                   "Failed to update entry into Hardware Tables for :",
                   &tempEntry.source);
    MFC_DEBUG_ADDR(MFC_DEBUG_FAILURES,
                   "Failed to update entry into Hardware Tables for :",
                   &tempEntry.group);
    if (BITX_TEST(&(tempEntry.oif),L7_PIMSM_REGISTER_INTF) != L7_NULL)
    {
      /* s/w don't want this to be added to h/w, just return success*/
      MFC_DEBUG(MFC_DEBUG_EVENTS,"\n oif contains  L7_PIMSM_REGISTER_INTF\n");
      retVal = L7_SUCCESS;
    } 
    else 
    {
      retVal = L7_FAILURE;
    }
  }
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to take MFC semaphore");
    return L7_FAILURE;
  }

  pEntry = mfcEntryGet(&pData->source, &pData->group, AVL_EXACT);
  if (pEntry != L7_NULLPTR)
  {
    if (BITX_TEST(&(tempEntry.oif),L7_PIMSM_REGISTER_INTF) != L7_NULL)
    {
      L7_uint32 ifIndex;

      /* Register Interface is Set.  Check to see if there is any other interface
       * that is Set in the OIF list.
       * If so, Set the newRoute Flag to False and cancel the Upcall Expire Timer.
       */
      for (ifIndex = 1; ifIndex < MCAST_MAX_INTERFACES; ifIndex++)
      {
        if (BITX_TEST (&tempEntry.oif, ifIndex) != L7_NULL)
        {
          if (pEntry->newRoute == L7_TRUE)
          {
            pEntry->newRoute = L7_FALSE;
            pEntry->upcallExpire = 0;
            mfcCountersUpdate (MFC_NUM_NEW_CACHE_ENTIRES, MFC_COUNTERS_DECREMENT);
          }
          break;
        }
      }
    }

    if ((pEntry->addedToHwTable != L7_TRUE) && (hwUpdation == L7_SUCCESS))
    {
      mfcCountersUpdate (MFC_NUM_HW_CACHE_ENTRIES, MFC_COUNTERS_INCREMENT);
      pEntry->addedToHwTable = L7_TRUE;
      if(pEntry->newRoute == L7_TRUE)
      {
        pEntry->newRoute = L7_FALSE;
        pEntry->upcallExpire = 0;
        mfcCountersUpdate (MFC_NUM_NEW_CACHE_ENTIRES, MFC_COUNTERS_DECREMENT);
      }  
    }
   /* CAUTION: Don't memcpy tempEntry into pEntry
       copy only those params which are changed in tempEntry
      memcpy(pEntry,&tempEntry,sizeof(mfcCacheEntry_t));--REMOVED
    */
    pEntry->iif = tempEntry.iif;
    BITX_COPY(&(tempEntry.oif), &(pEntry->oif));
    pEntry->mcastProtocol = tempEntry.mcastProtocol;
    pEntry->rpfAction = tempEntry.rpfAction;         
    
  
  }

  /* Release the semaphore lock */
  osapiSemaGive(mfcInfo.mfcAvlTree.semId);

  return retVal;
}

/*********************************************************************
*
* @purpose  Remove an mroute entry from the Multicast Forwarding Cache.
*
* @param    pData  @b{(input)} pointer to data containing the entry search key
*                              (source,group).
*
* @returns  L7_SUCCESS, if successful.
* @returns  L7_FAILURE, if operation failed.
*
* @comments This routine is intended to be invoked either by protocol owner of 
*           the mroute entry or when the entry times out through expire timeout
*           or upcall timer(in case of a new unresolved entry).
*
* @end
*
*********************************************************************/
L7_RC_t mfcEntryRemove(mfcEntry_t *pData)
{
  mfcCacheEntry_t  *pEntry=L7_NULLPTR;
  L7_uchar8         addrBufStr[IPV6_DISP_ADDR_LEN];
  mfcCacheEntry_t   tempEntry;
  L7_BOOL           addedToHw = L7_FALSE;

  /* Given data pointer is valid? */
  if (pData == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL mroute info for MFC entry deletion");
    return L7_FAILURE;
  }

  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry , source : ",&pData->source);
  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry , group : ",&pData->group);

  
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, "Failed to take MFC semaphore");
    return L7_FAILURE;
  }
  /* Retrieve the associated cache entry */
  pEntry = mfcEntryGet(&(pData->source), &(pData->group), AVL_EXACT);

  /* If entry not found, nothing to remove... just return */
  if (pEntry == L7_NULLPTR)
  {
    osapiSemaGive(mfcInfo.mfcAvlTree.semId);
    MFC_DEBUG(MFC_DEBUG_FAILURES,
              "\n no mfEntry present to remove in  MFC of source :",
              &pData->source);
    MFC_DEBUG(MFC_DEBUG_FAILURES,
              "\n no mfEntry present to remove in  MFC of group :",
              &pData->group);
    return L7_SUCCESS;
  }
  if (pEntry->newRoute == L7_TRUE)  
  {
     mfcCountersUpdate (MFC_NUM_NEW_CACHE_ENTIRES, MFC_COUNTERS_DECREMENT);
  }  
  addedToHw = pEntry->addedToHwTable;
  if (addedToHw == L7_TRUE)
  {
      mfcCountersUpdate (MFC_NUM_HW_CACHE_ENTRIES, MFC_COUNTERS_DECREMENT);
  }
  pData->iif = pEntry->iif;
  pData->mcastProtocol = pEntry->mcastProtocol;
  if (avlDeleteEntry(&(mfcInfo.mfcAvlTree), (void *)pEntry) == L7_NULLPTR)
  {
    /* Error : Entry not found in cache */
    MFC_LOG_MSG(0,"MFC Entry (src,grp)=(%s, %s) Delete from avlTree Failed",
                inetAddrPrint(&pData->source,addrBufStr),
                inetAddrPrint(&pData->group,addrBufStr)); 
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n Failed to delete mroute entry from avlTree");     
  }
  osapiSemaGive(mfcInfo.mfcAvlTree.semId);

  if (addedToHw == L7_TRUE)
  {
    if (mfcHwEntryDelete(&(pData->source), &(pData->group), pData->iif,
                         pData->mcastProtocol) != L7_SUCCESS)
    {
      MFC_LOG_MSG(0,"MFC Entry (src,grp)=(%s, %s) Delete from Hardware Failed",
                  inetAddrPrint(&pData->source,addrBufStr),
                  inetAddrPrint(&pData->group,addrBufStr)); 
      MFC_DEBUG_ADDR(MFC_DEBUG_FAILURES,
                     "Failed to delete entry from Hardware Tables for :",
                     &tempEntry.source);
      MFC_DEBUG_ADDR(MFC_DEBUG_FAILURES,
                     "Failed to delete entry from Hardware Tables for :",
                     &tempEntry.group);   
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Route/Forward the given multicast packet. If a route entry
*           is not present, add a new entry and request the MRPs to
*           resolve the route
*
* @param    pData @b{(input)} pointer to a structure containing the IP address
*                            of the source, destination group address, incoming
*                            router interface and a pointer to the packet itself.
*
* @returns  L7_SUCCESS, if successful.
*           L7_FAILURE, if operation failed.
*
* @comments Forwards the packet if the mroute entry has already resolved, else 
*           adds a new entry and generates NOCACHE event to registered MRPS
*
* @end
*
*********************************************************************/
L7_RC_t mfcMroutePktForward(mfcEntry_t *pData)
{
  mfcCacheEntry_t *pEntry = L7_NULLPTR,tempEntry;
  mfcEntry_t       outEntry;
  L7_RC_t          retVal = L7_FAILURE;
  L7_RC_t          hwUpdation = L7_FAILURE, txStatus = L7_FAILURE;
  L7_uint32        len = 0;
  L7_BOOL           bWrongIf = L7_FALSE;  
  L7_uchar8         src[IPV6_DISP_ADDR_LEN];
  L7_uchar8         grp[IPV6_DISP_ADDR_LEN];
  L7_BOOL           notifyWholePktEvent = L7_FALSE;  

  /* Data is valid? */
  if ((pData == L7_NULLPTR) || (pData->m == L7_NULLPTR))
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL mroute info for forwarding packet");
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n invalid input parameters ");
    return L7_FAILURE;
  }


  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry , source : ",&pData->source);
  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry , group : ",&pData->group);


  /* Lock the Table */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, "Failed to take MFC semaphore");
    return L7_FAILURE;
  }

  /* Retrieve the associated cache entry */
  pEntry = mfcEntryGet(&(pData->source), &(pData->group), AVL_EXACT);

  /* If entry found, try and forward the packet */
  if (pEntry != L7_NULLPTR)
  {
    if (pEntry->newRoute == L7_TRUE)
    {
      /* Attempt delivery of a WHOLE Packet Event only if ...
       * PIMSM is Registered and
       * Register Interface is set in the OIF and
       * Packet arrived on the correct Incoming interface.
       * Deliver the event later after releasing the Semaphore.
       */
      if (mfcRegisterList[L7_MRP_PIMSM].mfcNotifyFunction != L7_NULLPTR)
      {
        if (BITX_TEST(&(pEntry->oif),L7_PIMSM_REGISTER_INTF) != L7_NULL)
        {
          if (pData->iif == pEntry->iif)
          {
            /* 
             * mfcEntry_t passed from check-point data apply function contains no mbuf, these simulated
             * packet arrival events are used to set up the control plane; however there is no valid data
             * to forward via the software forwarding plane
             */
            if (pData->m->rtm_bufhandle != L7_NULLPTR)
            {
              notifyWholePktEvent = L7_TRUE;
            }
          }
        }
      }
      if (notifyWholePktEvent == L7_FALSE)
      {
        /* Unresolved entry, just return, dont fwd. */
        MFC_DEBUG (MFC_DEBUG_FAILURES, "Unresolved entry for (src,grp) = (%s, %s)",
                   inetAddrPrint(&pData->source,src), inetAddrPrint(&pData->group,grp));
        osapiSemaGive(mfcInfo.mfcAvlTree.semId);
        return L7_FAILURE;
      }
    }
    memcpy(&tempEntry,pEntry,sizeof(mfcCacheEntry_t));

    /*Give up the semaphore after copying mfc Entry into a temporary strcuture.This was the semaphore is held for a short period of time*/
    osapiSemaGive(mfcInfo.mfcAvlTree.semId);

    if (notifyWholePktEvent == L7_TRUE)
    {
      if (mfcRegisteredUsersNotify (MCAST_MFC_WHOLEPKT_EVENT,sizeof(mfcEntry_t), pData)
                                 != L7_SUCCESS)
      {
        MFC_DEBUG (MFC_DEBUG_FAILURES, "Failed to Notify Whole Packet Event for (src,grp) = (%s, %s)",
                   inetAddrPrint(&pData->source,src), inetAddrPrint(&pData->group,grp));
        return L7_FAILURE;
      }
      return L7_SUCCESS;
    }

      /* Update the Data TTL information */
      tempEntry.dataTTL = pData->dataTTL;

      /* Put together the forwarding information from the internal cache entry */
      memset(&outEntry, 0, sizeof(mfcEntry_t));
      inetCopy(&(outEntry.source), &(tempEntry.source));
      inetCopy(&(outEntry.group), &(tempEntry.group));
      BITX_COPY(&(tempEntry.oif), &(outEntry.oif));
      outEntry.mcastProtocol = tempEntry.mcastProtocol;

      if (pData->iif != tempEntry.iif)
      {
        /* If the packet is on the wrong interface, send a WRONG_IF notification
           and return with error */
        outEntry.iif = pData->iif;

        /*usersNotify = L7_FALSE;*/
        /* Do Wrong-iif rate-limiting */
        if ((mfcRegisterList[tempEntry.mcastProtocol].wrongIfRateLimitEnable == L7_TRUE) &&
            (((tempEntry.numWrongIfPkts) % (mfcRegisterList[tempEntry.mcastProtocol].wrongIfRateLimitCount)) == 0))
        {
          bWrongIf = L7_TRUE;
          MFC_DEBUG(MFC_DEBUG_EVENTS,"\n pkt arrived on wrong iface of source :",
                    &outEntry.source);
          MFC_DEBUG(MFC_DEBUG_EVENTS,"\n pkt arrived on wrong iface of group :",
                    &outEntry.group);

          if (mfcRegisterList[tempEntry.mcastProtocol].wrongIfRateLimitStarted != L7_TRUE)
          {
            /* Update the Hardware table */
            tempEntry.rpfAction = L7_TRUE;
            mfcHwEntryUpdate(&tempEntry, tempEntry.rpfAction);
            mfcRegisterList[tempEntry.mcastProtocol].wrongIfRateLimitStarted = L7_TRUE;
            MFC_DEBUG(MFC_DEBUG_RATE_LIMIT,"\n wrongIfRateLimit Start :");
          }
        }

        tempEntry.numWrongIfPkts++;
        tempEntry.inUseBySw = L7_TRUE;

        if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
        {
          MFC_DEBUG (MFC_DEBUG_FAILURES, "Failed to take MFC semaphore");
          return L7_FAILURE;
        }
        pEntry = mfcEntryGet(&tempEntry.source, &tempEntry.group, AVL_EXACT);
       /* CAUTION: Don't memcpy tempEntry into pEntry
           copy only those params which are changed in tempEntry
           memcpy(pEntry,&tempEntry,sizeof(mfcCacheEntry_t));--REMOVED

        */
        if (pEntry != L7_NULLPTR)
        {
          pEntry->numWrongIfPkts = tempEntry.numWrongIfPkts;
          pEntry->inUseBySw = tempEntry.inUseBySw;
          pEntry->rpfAction = tempEntry.rpfAction;         
          pEntry->dataTTL = tempEntry.dataTTL;             
        }
        /* It is critical that the semaphore lock be released before invoking
           the notification; failing with deadly semaphore locks could occur */
        osapiSemaGive(mfcInfo.mfcAvlTree.semId);

        if (bWrongIf == L7_TRUE)
        {
            mfcRegisteredUsersNotify(MCAST_MFC_WRONGIF_EVENT, sizeof(mfcEntry_t),
                                   &outEntry);
        }

        return L7_FAILURE;
      }
      else
      {
        /* 
         * mfcEntry_t passed from check-point data apply function contains no mbuf, these simulated
         * packet arrival events are used to set up the control plane; however there is no valid data
         * to forward via the software forwarding plane
         */
        if (pData->m->rtm_bufhandle != L7_NULLPTR)
        {
          /* Attempt to forward the packet */
          SYSAPI_NET_MBUF_GET_DATALENGTH(
                                        (L7_netBufHandle)(pData->m->rtm_bufhandle), len);
          outEntry.iif = tempEntry.iif;
          if (mfcPktTransmit(pData->m, &outEntry) == L7_SUCCESS)
          {
            txStatus = L7_SUCCESS;
            /* Update the stats */
            tempEntry.numForwardedPkts++;
            tempEntry.inUseBySw = L7_TRUE;
            tempEntry.numBytes += len;

            retVal = L7_SUCCESS;
          }
        }

        if ((tempEntry.addedToHwTable != L7_TRUE))
        {
          /*Update the Hardware table entries */
          tempEntry.rpfAction = L7_FALSE;
          if (mfcHwEntryUpdate(&tempEntry, tempEntry.rpfAction) == L7_SUCCESS)
          {
            hwUpdation = L7_SUCCESS;
          }
          else
          {
            MFC_DEBUG_ADDR(MFC_DEBUG_FAILURES,
                           "Failed to update entry into Hardware Tables for :",
                           &tempEntry.source);
            MFC_DEBUG_ADDR(MFC_DEBUG_FAILURES,
                           "Failed to update entry into Hardware Tables for :",
                           &tempEntry.group);
          } 
        }

        /* Attempt delivery of a WHOLE Packet Event */
        if (BITX_TEST(&(tempEntry.oif),L7_PIMSM_REGISTER_INTF) != L7_NULL)
        {
          if (mfcRegisterList[L7_MRP_PIMSM].mfcNotifyFunction != L7_NULLPTR)
          {
            /* 
             * mfcEntry_t passed from check-point data apply function contains no mbuf, these simulated
             * packet arrival events are used to set up the control plane; however there is no valid data
             * to forward via the software forwarding plane
             */
            if (pData->m->rtm_bufhandle != L7_NULLPTR)
            {
               /*pData->mcastProtocol = L7_MRP_PIMSM;*/
              tempEntry.numWholePktEvents++;
               /*[dsatya] : If notify fails , then Possible mbuf leak - fix-it*/
              mfcRegisteredUsersNotify(MCAST_MFC_WHOLEPKT_EVENT, sizeof(mfcEntry_t), pData);
            }
          }
        }
        if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
        {
          MFC_DEBUG (MFC_DEBUG_FAILURES, "Failed to take MFC semaphore");
          return L7_FAILURE;
        }
        pEntry = mfcEntryGet(&pData->source, &pData->group, AVL_EXACT);
        if (pEntry != L7_NULLPTR)
        {
          if ((pEntry->addedToHwTable != L7_TRUE) && (hwUpdation == L7_SUCCESS))
          {
            mfcCountersUpdate (MFC_NUM_HW_CACHE_ENTRIES, MFC_COUNTERS_INCREMENT);
            pEntry->addedToHwTable = L7_TRUE;
          }
         /* CAUTION: Don't memcpy tempEntry into pEntry
             copy only those params which are changed in tempEntry
          memcpy(pEntry,&tempEntry,sizeof(mfcCacheEntry_t)); --REMOVED
          */
          pEntry->numWholePktEvents = tempEntry.numWholePktEvents;
          pEntry->numForwardedPkts = tempEntry.numForwardedPkts;
          pEntry->numBytes = tempEntry.numBytes;
          pEntry->inUseBySw = tempEntry.inUseBySw;
          pEntry->rpfAction = tempEntry.rpfAction;         
          pEntry->dataTTL = tempEntry.dataTTL;              
        }
        if (txStatus == L7_SUCCESS)
        {
          mfcCountersUpdate (MFC_NUM_FORWARDED_PKTS, MFC_COUNTERS_INCREMENT);
        }
        /* Release the cache lock */
        osapiSemaGive(mfcInfo.mfcAvlTree.semId);
        return retVal;
      }
  }
  /* Next : Request insertion of the entry into the cache table */
  if (mfcEntryAdd(pData) != L7_SUCCESS)
  {
    MFC_DEBUG_ADDR(MFC_DEBUG_FAILURES,
                   "Failed to add entry into MFC tree for :",
                   &pData->source);
    MFC_DEBUG_ADDR(MFC_DEBUG_FAILURES,
                   "Failed to add entry into MFC tree for :",
                   &pData->group);  
    osapiSemaGive(mfcInfo.mfcAvlTree.semId);
    return L7_FAILURE;
  }
  /* Next : Notify the registered MRP(s) about the new entry requesting their
            help in resolving the forwarding interface list.
            Note : Release the semaphore before doing the notification as 
                   otherwise we could end up with semaphore deadlocks */
  osapiSemaGive(mfcInfo.mfcAvlTree.semId);
  /*
     Send No-cache and whole-pkt events, 
     the respective entry counters are updated
     below under semaphore protection.
   */
  if (mfcRegisteredUsersNotify(MCAST_MFC_NOCACHE_EVENT, sizeof(mfcEntry_t),
                          pData) != L7_SUCCESS)
  {
    /* If notification fails, remove the recently added entry */
    MFC_DEBUG(MFC_DEBUG_FAILURES," Failed to send NO_CACHE EVENT to MRPs"); 
    mfcEntryRemove(pData);
    return L7_FAILURE;
  }

  /* Attempt delivery of a WHOLE Packet Event */
  if (mfcRegisterList[L7_MRP_PIMSM].mfcNotifyFunction != L7_NULLPTR)
  {
    /*pData->mcastProtocol = L7_MRP_PIMSM;*/
    /* 
     * mfcEntry_t passed from check-point data apply function contains no mbuf, these simulated
     * packet arrival events are used to set up the control plane; however there is no valid data
     * to forward via the software forwarding plane
     */
    if (pData->m->rtm_bufhandle != L7_NULLPTR)
    {
      /*[dsatya] : If notify fails , then Possible mbuf leak - fix-it*/    
      mfcRegisteredUsersNotify(MCAST_MFC_WHOLEPKT_EVENT, sizeof(mfcEntry_t), pData);
    }
  }
  /* Lock the Table */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, "Failed to take MFC semaphore");
    return L7_FAILURE;
  }

  /* Retrieve the associated cache entry */
  pEntry = mfcEntryGet(&(pData->source), &(pData->group), AVL_EXACT);

  /* If entry found, try and forward the packet */
  if (pEntry != L7_NULLPTR)
  {
      pEntry->numNoCacheEvents++;
      if (mfcRegisterList[L7_MRP_PIMSM].mfcNotifyFunction != L7_NULLPTR)
      {
        pEntry->numWholePktEvents++;
      }
  }
  osapiSemaGive(mfcInfo.mfcAvlTree.semId);
  /*[dsatya] : Possible mbuf leak - fix-it*/   
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  API function used by various modules to post messages to the
*           Multicast Forwarding Cache module.
*
* @param    eventType @b{(input)}Event Type to be posted.
* @param    pData     @b{(input)}Pointer to a buffer containing the message.
* @param    msgLen      @b{(input)} Message Length.
*
* @returns  L7_SUCCESS, if successful.
*           L7_FAILURE, if operation failed.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcMessagePost(mfcOperationType_t operType,  void *pData, 
                       L7_uint32 msgLen)
{
  L7_RC_t rc = L7_FAILURE;
  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry, operType = %d", operType);

  if (pData == L7_NULLPTR || msgLen == 0)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL mroute info to operate on MFC");
    return L7_FAILURE;
  }
  /* post message to mcastmap queue */
  switch (operType)
  {
    case MFC_UPDATE_ENTRY:
      if (msgLen != sizeof(mfcEntry_t))
      {
        MFC_DEBUG(MFC_DEBUG_FAILURES," insufficient message info for MFC UPDATE operation");
        return L7_FAILURE;
      }
      rc = mfcEntryUpdate((mfcEntry_t *)pData);
      break;
    case MFC_DELETE_ENTRY:
      if (msgLen != sizeof(mfcEntry_t))
      {
        MFC_DEBUG(MFC_DEBUG_FAILURES," insufficient message info for MFC DELETE operation");
        return L7_FAILURE;
      }
      rc = mfcEntryRemove((mfcEntry_t *)pData);      
      break;
    case MFC_INTERFACE_STATUS_CHNG:
      if (msgLen != sizeof(mfcInterfaceChng_t))
      {
        MFC_DEBUG(MFC_DEBUG_FAILURES," insufficient message info for MFC INTERFACE "
                    " STATUS CHNG operation");      
        return L7_FAILURE;
      }
      rc = mfcInterfaceStatusChangeProcess((mfcInterfaceChng_t *)pData);      
       break;
    default:
      MFC_DEBUG(MFC_DEBUG_FAILURES,"\n Invalid event type %d.\n", operType);
      rc = L7_FAILURE;
      /*passthru*/
  }
  return rc;
}

/*********************************************************************
* @purpose  Register a routine to be called when changes occur within
*           the Multicast Forwarding Cache module that necessitates
*           communication with other modules for the given protocol.
*
* @param    protocolId    @b{(input)}Protocol ID (A identified constant in L7_MRP_TYPE_t )
* @param    *notifyFn     @b{(input)}pointer to a routine to be invoked for MFC
*                                    changes.
*           wrongIfRateLimitEnable @b{(input)} Flag indicating whether
*                                              Enable/Disable
*           wrongIfRateLimitThreshold @b{(input)} Rate Limit Threshold
*                                                 Value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  The registration from MRPs is reentrant, yet the notify list is not 
*            protected as function pointer location is fixed for each 
* @comments  The wrongIfRateLimitEnable has to be Set only by the MRPs which
*            processes the Wrong Interface Events.
*
* @end
*
*********************************************************************/
L7_RC_t mfcCacheEventRegister(L7_MRP_TYPE_t protocolId,
                              mfcNotifyFn_t notifyFn,
                              L7_BOOL wrongIfRateLimitEnable,
                              L7_uint32 wrongIfRateLimitThreshold)
{
  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry , protocol = %d",protocolId);

  if (protocolId >= L7_MRP_MAXIMUM)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,
              " Failed to register, invalid protocol Id= %d", protocolId);
    return L7_FAILURE;
  }

  if (notifyFn == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," Failed to register , NULL function pointer");
    return L7_FAILURE;
  }
  mfcRegisterList[protocolId].mfcNotifyFunction = notifyFn;

  if (L7_TRUE == wrongIfRateLimitEnable)
  {
    mfcRegisterList[protocolId].wrongIfRateLimitEnable = wrongIfRateLimitEnable;
    mfcRegisterList[protocolId].wrongIfRateLimitCount = wrongIfRateLimitThreshold;
    mfcRegisterList[protocolId].wrongIfRateLimitStarted = L7_FALSE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  De-register an already registered notify function from the
*           Multicast Forwarding Cache module for the given protocol.
*
* @param    protocolId   @b{(input)}Protocol ID  (See L7_MRP_TYPE_t )
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
*********************************************************************/
L7_RC_t mfcCacheEventDeregister(L7_MRP_TYPE_t protocolId)
{

  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry , protocol = %d", protocolId);


  if (protocolId >= L7_MRP_MAXIMUM)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,
              "Invalid protocol Id to deregister = %d",protocolId);
    return L7_FAILURE;
  }

  mfcRegisterList[protocolId].mfcNotifyFunction = L7_NULLPTR;
  mfcRegisterList[protocolId].wrongIfRateLimitCount = 0;
  mfcRegisterList[protocolId].wrongIfRateLimitEnable = L7_FALSE;
  mfcRegisterList[protocolId].wrongIfRateLimitStarted = L7_FALSE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send Notification Event to the appropriate protocol for
*           the given Multicast Forwarding Cache entry
*
* @param    eventType      @b{(input)}Event Type to be sent
* @param    msgLen         @b{(input)}Length of the associated message
* @param    pData          @b{(input)}Pointer to the MFC entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t mfcRegisteredUsersNotify(mcastEventTypes_t eventType, L7_uint32 msgLen,
                            mfcEntry_t *pData)
{
  L7_uint32     i;
  L7_RC_t retCode = L7_FAILURE;
  L7_BOOL bProcessed = L7_FALSE;

  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry , eventType = %d, msgLen = %d",
            eventType,  msgLen);

  /* Do not pass on WRONG IF Event to DVMRP and MGMD-Proxy */
  if (eventType == MCAST_MFC_WRONGIF_EVENT)
  {
    L7_MCAST_IANA_PROTO_ID_t currentMcastProtocol = L7_MCAST_IANA_MROUTE_UNASSIGNED;

    if (mcastMapIpCurrentMcastProtocolGet (L7_AF_INET, &currentMcastProtocol)
                                        == L7_SUCCESS)
    {
      if ((currentMcastProtocol == L7_MCAST_IANA_MROUTE_DVMRP) ||
          (currentMcastProtocol == L7_MCAST_IANA_MROUTE_IGMP_PROXY))
      {
        MFC_DEBUG (MFC_DEBUG_EVENTS, "Dropping the Wrong IF Event to DVMRP/Proxy");
        return L7_SUCCESS;
      }
    }
  }

  /* Given Data is valid? */
  if (pData == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL mroute info to notify MRPs");
    return L7_FAILURE;
  }

  /* Send the message to the appropriate handler(s) */
  if (pData->mcastProtocol == 0)
  {
    /* Cycle through all the registered protocols and send the event */
    for (i = 0; i < L7_MRP_MAXIMUM; i++)
    {
      if (mfcRegisterList[i].mfcNotifyFunction != L7_NULLPTR)
      {
        retCode =
         (mfcRegisterList[i].mfcNotifyFunction)((L7_uchar8)(pData->source.family),
                                             eventType, msgLen, pData);
          if (retCode == L7_SUCCESS)
            bProcessed = L7_TRUE;
      }
    }
    /* Return SUCCESS if the event was successfully handled by atleast one handler */
    if (bProcessed == L7_TRUE)
    {
      return L7_SUCCESS;
    }
    else
    {
      MFC_DEBUG(MFC_DEBUG_FAILURES," No MRP handled the event = %d", eventType);
      return L7_FAILURE;
    }
  }
  else
  {
    if (mfcRegisterList[pData->mcastProtocol].mfcNotifyFunction != L7_NULLPTR)
    {
      return (mfcRegisterList[pData->mcastProtocol].mfcNotifyFunction)((L7_uchar8)(pData->source.family),
                                                      eventType, msgLen, pData);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns whether th mroute entry is active(in use) or not
*
* @param    pRtEntry @b{(inout)} Pointer to a route information structure
*                                Input  : The source and group addresses
*                                Output : Various route info associated with the
*                                given <source,group> cache entry
*
* @returns  L7_TRUE , if the mroute entry is in use by cache or h/w
* @returns  L7_FALSE, if not in use
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL mfcIsEntryInUse(mfcEntry_t *pRtEntry)
{
  mfcCacheEntry_t *pEntry = L7_NULLPTR;

  /* Given Data is valid? */
  if (pRtEntry == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL mroute info to find mroute entry is in use or not");
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n invalid input parameters ");
    return L7_FALSE;
  }

  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry, source :", &pRtEntry->source);
  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry, group :", &pRtEntry->group);


  /* Lock the module to avoid re-entrancy problems */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to take MFC semaphore");
    return L7_FALSE;
  }

  /* Retrieve the associated cache entry */
  pEntry = mfcEntryGet(&(pRtEntry->source), &(pRtEntry->group), AVL_EXACT);

  /* If entry not found, return */
  if (pEntry == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n mroute entry doesnot exist of source : ",
              &(pRtEntry->source));   
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n mroute entry doesnot exist of group : ",
              &(pRtEntry->group));  
    osapiSemaGive(mfcInfo.mfcAvlTree.semId);
    return L7_FALSE;
  }

  /* Fill-in the route information */
  pRtEntry->iif           = pEntry->iif;
  pRtEntry->mcastProtocol = pEntry->mcastProtocol;
  pRtEntry->dataTTL       = pEntry->dataTTL;
  BITX_COPY(&(pEntry->oif),&(pRtEntry->oif));

  /* Return successful */
  osapiSemaGive(mfcInfo.mfcAvlTree.semId);
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Retrieve the statistics associated with a given Multicast
*           Forwarding Cache entry
*
* @param    pStats  @b{(input)}  Pointer to a statistics structure
*                                Input  : The source and group addresses
*                                Output : Various statistics associated with the
*                                given <source,group> cache entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcEntryStatsGet(mfcEntryStats_t *pStats)
{
  mfcCacheEntry_t *pEntry = L7_NULLPTR;
  L7_uchar8        addrBufStr[IPV6_DISP_ADDR_LEN];

  /* Given Data is valid? */
  if (pStats == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL mroute info to find obtain stats ");
    return L7_FAILURE;
  }

  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry, source :", &pStats->source);
  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry, group :", &pStats->group);

  /* Lock the module to avoid re-entrancy problems */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, "\n MFC : Failed to take MFC semaphore");
    return L7_FAILURE;
  }

  /* Retrieve the associated cache entry */
  pEntry = mfcEntryGet(&(pStats->source), &(pStats->group), AVL_EXACT);

  /* If entry not found, return */
  if (pEntry == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," Mroute Entry does nor exist for (src,grp) =  (%s, %s)",
                  inetAddrPrint(&pStats->source,addrBufStr),
                  inetAddrPrint(&pStats->group,addrBufStr));   
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n mroute entry doesnot exist of source : ",
              &(pStats->source));   
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n mroute entry doesnot exist of group : ",
              &(pStats->group));
    osapiSemaGive(mfcInfo.mfcAvlTree.semId);
    return L7_FAILURE;
  }

  /* Fill-in the stats information */
  pStats->pktCount  = pEntry->numForwardedPkts;
  pStats->byteCount = pEntry->numBytes;
  pStats->wrongIf   = pEntry->numWrongIfPkts;

  /* Return successful */
  osapiSemaGive(mfcInfo.mfcAvlTree.semId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear all statistics associated with a given Multicast
*           Forwarding Cache entry
*
* @param    pStats    @b{(input)}Pointer to a statistics structure containing the
*                                source and group addresses of the cache entry, All
*                                other elements in the structure are ignored
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcEntryStatsClear(mfcEntryStats_t *pStats)
{
  mfcCacheEntry_t *pEntry = L7_NULLPTR;
  
  /* Given Data is valid? */
  if (pStats == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," NULL mroute info to clear obtain stats ");
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n invalid input parameters");
    return L7_FAILURE;
  }

  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry, source :", &pStats->source);
  MFC_DEBUG_ADDR(MFC_DEBUG_APIS,"\n Entry, group :", &pStats->group);

  /* Lock the module to avoid re-entrancy problems */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, "\n MFC : Failed to take MFC semaphore");
    return L7_FAILURE;
  }

  /* Retrieve the associated cache entry */
  pEntry = mfcEntryGet(&(pStats->source), &(pStats->group), AVL_EXACT);

  /* If entry not found, return */
  if (pEntry == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n mroute entry doesnot exist of source : ",
              &(pStats->source));   
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n mroute entry doesnot exist of group : ",
              &(pStats->group));    
    osapiSemaGive(mfcInfo.mfcAvlTree.semId);    
    return L7_FAILURE;
  }

  /* Clear out the stats information */
  pEntry->numForwardedPkts = pEntry->numBytes = pEntry->numWrongIfPkts = 0;
  pEntry->numNoCacheEvents = pEntry->numWholePktEvents = pEntry->numWrongIfEvents = 0; 
  /* Return successful */
  osapiSemaGive(mfcInfo.mfcAvlTree.semId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if the module has been initialized
*
* @param    None
*
* @returns  L7_TRUE  : Module is initialized
* @returns  L7_FALSE : Module is un-initialized
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL mfcIsEnabled(L7_uchar8 family)
{
  if (family == L7_AF_INET)
  {
    return mfcInfo.mfcV4Enabled;
  }
  else if (family == L7_AF_INET6)
  {
    return mfcInfo.mfcV6Enabled;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Update the MFC Counters
*
* @param    None
*
* @returns  L7_SUCCESS
*           L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t
mfcCountersUpdate (MFC_COUNTERS_TYPE_t counterType,
                   MFC_COUNTERS_ACTION_t counterAction)
{
  switch (counterAction)
  {
    case MFC_COUNTERS_INCREMENT:
      (mfcInfo.mfcCounters[counterType])++;
      break;

    case MFC_COUNTERS_DECREMENT:
      if ((mfcInfo.mfcCounters[counterType]) <= 0)
       {
         MFC_DEBUG (MFC_DEBUG_FAILURES, "MFC: Invalid operation on counterType %d ",counterType);
         return L7_FAILURE;
       }        
      (mfcInfo.mfcCounters[counterType])--;
      break;

    case MFC_COUNTERS_RESET:
      (mfcInfo.mfcCounters[counterType]) = 0;
      break;

    default:
      break;
  }
  /* Return successful */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the MFC Counters Value
*
* @param    None
*
* @returns  counterValue 
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32
mfcCountersValueGet (MFC_COUNTERS_TYPE_t counterType)
{
  L7_uint32 counterValue = 0;

  counterValue = (mfcInfo.mfcCounters[counterType]);

  /* Return successful */
  return counterValue;
}
/***********************************************************************
* @purpose Get Expiry time for a particular multicast entry.
* 
* params  ipMRtSrc    {(input)}  multicast source address.
*         ipMRtGrp    {(input)}  multicast group address.
*         expiryTime  {(output)} expiry time for multicastentry.
*         
* returns L7_SUCCESS : for suceessful get operation.
*         L7_FAILURE : for error in the operation.
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t mfcEntryExpiryTimeGet(L7_inet_addr_t *ipMRtSrc,
                              L7_inet_addr_t *ipMRtGrp,
                              L7_uint32 *expiryTime)
{
  mfcCacheEntry_t *mfentry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 now;

  now = osapiUpTimeRaw();
  mfentry = mfcEntryGet(ipMRtSrc, ipMRtGrp, AVL_EXACT);
  if(mfentry != L7_NULLPTR)
  {
    *expiryTime = (mfentry->expire) - now;
    rc = L7_SUCCESS;
  }
  return rc;
}

