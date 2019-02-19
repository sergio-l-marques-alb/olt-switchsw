/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   mfc_debug.c
*
* @purpose    Implements the Debug support functionality of the Multicast
*             Forwarding Cache (MFC) module
*
* @component  Multicast Forwarding Cache (MFC)
*
* @comments   none
*
* @create     March 24,2006
*
* @author     ddevi.
* @end
*
**********************************************************************/
/**********************************************************************
                  Includes
***********************************************************************/
#include "defaultconfig.h"
#include "datatypes.h"
#include "sysapi.h"
#include "mfc.h"
#include "mfc_debug.h"
#include "mfc_rxtx.h"

L7_uint32 mfcTotalBytesAllocated = 0;

/**********************************************************************
                  Typedefs & Defines
***********************************************************************/
#define MFC_NUM_FLAG_BYTES     ((MFC_DEBUG_FLAG_LAST + 7) / 8)


/******************************************************************
                 Global Declarations
******************************************************************/
static L7_uchar8 debugFlags[MFC_NUM_FLAG_BYTES];
static L7_BOOL   debugEnabled = L7_TRUE;

/*****************************************************************
    Function Definitions
******************************************************************/

/*********************************************************************
*
* @purpose  Enable Debug Tracing for the MFC.
*
* @param    None.
*
* @returns  L7_SUCCESS  if Debug trace was successfully enabled.
* @returns  L7_FAILURE  if there was an error enabling Debug Trace.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcDebugEnable(void)
{
  debugEnabled = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for the MFC.
*
* @param    None.
*
* @returns  L7_SUCCESS  if Debug trace was successfully disabled.
* @returns  L7_FAILURE  if there was an error disabling Debug Trace.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcDebugDisable(void)
{
  debugEnabled = L7_FALSE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing on the entire module of MFC.
*
* @param    None.
*
* @returns  L7_SUCCESS   if Debug trace was successfully enabled.
* @returns  L7_FAILURE   if there was an error enabling Debug Trace.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcDebugAllSet(void)
{
  memset(debugFlags, 0xFF, sizeof(debugFlags));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing on the entire module of MFC.
*
* @param    None.
*
* @returns  L7_SUCCESS if Debug trace was successfully disabled.
* @returns  L7_FAILURE if there was an error disabling Debug Trace.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcDebugAllReset(void)
{
  memset(debugFlags, 0, sizeof(debugFlags));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific flag in MFC.
*
* @param    None.
*
* @returns  L7_SUCCESS   if Debug trace was successfully enabled.
* @returns  L7_FAILURE   if there was an error enabling Debug Trace.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcDebugFlagSet(MFC_DEBUG_FLAGS_t flag)
{
  if (flag >= MFC_DEBUG_FLAG_LAST)
  {
    return L7_FAILURE;
  }
  debugFlags[flag/8] |= (1 << (flag % 8));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific flag in MFC.
*
* @param    None.
*
* @returns  L7_SUCCESS   if Debug trace was successfully disabled.
* @returns  L7_FAILURE   if there was an error disabling Debug Trace.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcDebugFlagReset(MFC_DEBUG_FLAGS_t flag)
{
  if (flag >= MFC_DEBUG_FLAG_LAST)
  {
    return L7_FAILURE;
  }
  debugFlags[flag/8] &= (~(1 << (flag % 8)));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Test if a particular Debug Flag is turned ON in MFC.
*
* @param    None.
*
* @returns  L7_TRUE  if the Debug trace flag is turned ON.
* @returns  L7_FALSE if the Debug trace flag is turned OFF.
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL mfcDebugFlagCheck(MFC_DEBUG_FLAGS_t dbg_type)
{
  if (debugEnabled != L7_TRUE)
  {
    return L7_FALSE;
  }
  if (dbg_type >= MFC_DEBUG_FLAG_LAST)
  {
    return L7_FALSE;
  }
  if ((debugFlags[dbg_type/8] & (1 << (dbg_type % 8))) != 0)
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Shows the current MFC Debug flag status.
*
* @param    None.
*
* @returns  None.
*
* @comments
*
* @end
*
*********************************************************************/
void mfcDebugFlagShow(void)
{
  L7_uint32 i;

  if (debugEnabled == L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," MFC Debugging : Enabled\n");
    for (i = 0;  i < MFC_DEBUG_FLAG_LAST ; i++)
    {
      if (mfcDebugFlagCheck(i) == L7_TRUE)
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"     debugFlag [%d] : Enabled\n", i);
      }
      else
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"     debugFlag [%d] : Disabled\n", i);
      }
    }
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," MFC Debugging : Disabled\n");
  }
}

/*********************************************************************
*
* @purpose  Shows usage of the MFC Debug utility
*
* @param    None.
*
* @returns  None.
*
* @comments
*
* @end
*
*********************************************************************/
void mfcDebugHelp(void)
{
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," Use mfcDebugEnable()/mfcDebugDisable() to Enable/Disable Debug trace in MFC\n");
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," Use mfcDebugFlagSet(flag)/mfcDebugFlagReset(flag) to Enable/Disable specific functionality traces\n");
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," Use mfcDebugFlagAllSet()/mfcDebugFlagAllReset() to Enable/Disable all traces\n");  
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"     Various Debug Trace flags and their definitions are as follows ;\n");
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"         0  -> Trace the complete Receive Data path\n");
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"         1  -> Trace the complete Transmission Data path\n");
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"         2  -> Trace all the EVENT generations and receptions\n");
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"         3  -> Trace all Timer activities\n");
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"         4  -> Trace all failures\n");
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"         5  -> Trace all the APIs invoked\n");

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," Use mfcDebugShow(count) to view mroute entries info \n");
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," Use mfcDebugDetailShow(count) to view mroute entries in tabular form \n");  
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," Use mfcDebugBriefShow to get the brief details of mroute table \n");    

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," Use 'mfcDebugAllEntriesDelete' to Delete all Entries from the MFC and H/W Tables \n");
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," Use 'mfcDebugAllNegativeEntriesDelete' to Delete all Negative Entries from the MFC and H/W Tables \n");

  return;
}

/*********************************************************************
* @purpose  Debug Dump of all the entries in Multicast Forwarding Cache
*
* @param    None
*
* @returns  None
*
* @comments
*
* @end
*
*********************************************************************/
void mfcDebugDetailShow(L7_int32 count)
{
  mfcCacheEntry_t *mfentry = L7_NULLPTR;
  mfcCacheEntry_t tempEntry;
  L7_uchar8 dstDump[40];
  L7_uchar8 srcDump[40];    
  L7_uint32 i, mfcNumCacheEntries;
  L7_BOOL   bOif;
  L7_uint32 now;
  L7_int32 localcount = 0;  

  /* Lock the Table */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to take MFC semaphore");
    return;
  }

  mfcNumCacheEntries = avlTreeCount(&(mfcInfo.mfcAvlTree));

  MFC_DEBUG_PRINTF("\n __________ MFC CACHE TABLE ______________\n");
  MFC_DEBUG_PRINTF("\nMFC IPv4 Mode   : %s", ((mfcInfo.mfcV4Enabled)  ? ("Enabled") : ("Disabled")));
  MFC_DEBUG_PRINTF("\nMFC IPv6 Mode   : %s", ((mfcInfo.mfcV6Enabled)  ? ("Enabled") : ("Disabled")));
  MFC_DEBUG_PRINTF("\nMFC Entry Count : %d", mfcNumCacheEntries );
  MFC_DEBUG_PRINTF("\nH/W Entry Count : %d", mfcCountersValueGet(MFC_NUM_HW_CACHE_ENTRIES));
  MFC_DEBUG_PRINTF("\nNew Entry Count : %d", mfcCountersValueGet(MFC_NUM_NEW_CACHE_ENTIRES));
  MFC_DEBUG_PRINTF("\nPkts Forwarded  : %d", mfcCountersValueGet(MFC_NUM_FORWARDED_PKTS));
  MFC_DEBUG_PRINTF("\n");

  if (mfcNumCacheEntries > 0)
  {
    memset(&tempEntry, 0 , sizeof(mfcCacheEntry_t));
    inetAddressZeroSet(L7_AF_INET, &tempEntry.source);
    inetAddressZeroSet(L7_AF_INET, &tempEntry.group);

    /* initialize the search correctly with right family */
    while ((mfentry =(mfcCacheEntry_t *)avlSearchLVL7(&(mfcInfo.mfcAvlTree),
                                                      (void *)(&tempEntry), AVL_NEXT)) != L7_NULLPTR)
      {

        if((count!= L7_NULL)&&(localcount >= count))
        {
          break;     
        }

        localcount++;

        inetCopy(&(tempEntry.source), &(mfentry->source));
        inetCopy(&(tempEntry.group), &(mfentry->group));
        now = osapiUpTimeRaw(); 
 
        MFC_DEBUG_PRINTF("\n -------------------------------- "); 
        MFC_DEBUG_PRINTF(" \n Source         : ");
        if (inetAddrHtop(&(mfentry->source),srcDump) == L7_SUCCESS)
        {
        MFC_DEBUG_PRINTF(" %s\n",srcDump);
        }
        else
        {
        MFC_DEBUG_PRINTF(" unable to get src addr \n");
        }

        MFC_DEBUG_PRINTF(" Destination    : ");        
        if (inetAddrHtop(&(mfentry->group),dstDump) == L7_SUCCESS)
        {
        MFC_DEBUG_PRINTF(" %s\n",dstDump);
        }
        else
        {
        MFC_DEBUG_PRINTF(" unable to get destn addr \n");
        }

      MFC_DEBUG_PRINTF(" Incoming Iface : %d\n", mfentry->iif);
      MFC_DEBUG_PRINTF(" Outgoing Ifaces: ");
        BITX_IS_EMPTY(&(mfentry->oif), bOif);
        if (bOif != L7_TRUE)
        {
          for (i = 0; i < MAX_INTERFACES; i++)
          {
            if (BITX_TEST(&(mfentry->oif), i))
            {
            MFC_DEBUG_PRINTF("%d ", i);
            }
            else
            {  
            MFC_DEBUG_PRINTF(" ");  
            }
          }
        MFC_DEBUG_PRINTF("\n");
        }
        else
        {
        MFC_DEBUG_PRINTF("None\n");
        } 
      MFC_DEBUG_PRINTF(" mcastProtocol  : %s\n", ((mfentry->mcastProtocol == 1) ? ("DVMRP") :
                                                  ((mfentry->mcastProtocol == 2) ? ("PIM-DM") :
                                                  ((mfentry->mcastProtocol == 3) ? ("PIM-SM") :
                                                  ((mfentry->mcastProtocol == 4) ? ("IGMP-PROXY") : ("Unknown"))))));
      MFC_DEBUG_PRINTF(" WrongIfRateLimitEnable    : %s\n", ((mfcRegisterList[mfentry->mcastProtocol].wrongIfRateLimitEnable == L7_TRUE) ? "Enable" : "Disable"));
      if (mfcRegisterList[mfentry->mcastProtocol].wrongIfRateLimitEnable == L7_TRUE)
      {
        MFC_DEBUG_PRINTF(" WrongIfRateLimitStart     : %s\n", ((mfcRegisterList[mfentry->mcastProtocol].wrongIfRateLimitStarted == L7_TRUE) ? "Started" : "Stopped"));
        MFC_DEBUG_PRINTF(" WrongIfRateLimitThreshold : %d\n", mfcRegisterList[mfentry->mcastProtocol].wrongIfRateLimitCount);
      }
      MFC_DEBUG_PRINTF(" Addedto HW     : %s\n",
                       mfentry->addedToHwTable ? "TRUE":"FALSE");
      MFC_DEBUG_PRINTF(" NewRoute       : %s\n", 
                       mfentry->newRoute ? "TRUE":"FALSE");
      MFC_DEBUG_PRINTF(" Wrong_iifs     : %d\n", mfentry->numWrongIfPkts);
      MFC_DEBUG_PRINTF(" No. of Pkts    : %d\n", mfentry->numForwardedPkts);
      MFC_DEBUG_PRINTF(" MFC Use        : %s\n",
                       mfentry->inUseBySw ? "TRUE":"FALSE");
      /*MFC_DEBUG_PRINTF(" HW Use         : %s\n",
                       mfentry->inUseByHw ? "TRUE":"FALSE");*/
      MFC_DEBUG_PRINTF(" HoldTime       : %d secs\n", mfentry->holdtime);
      MFC_DEBUG_PRINTF(" Expire Time    : %ld secs\n", mfentry->expire - now);
      MFC_DEBUG_PRINTF(" Up Time        : %ld secs\n", now - mfentry->ctime);
      if (mfentry->newRoute == L7_TRUE)
      {
        MFC_DEBUG_PRINTF(" Upcall ETime   : %ld secs\n",
                         mfentry->upcallExpire - now);
      }
      else
      {
        MFC_DEBUG_PRINTF(" Upcall ETime   : %ld secs\n", mfentry->upcallExpire);
      }
      MFC_DEBUG_PRINTF(" No-cache events : %d \n", mfentry->numNoCacheEvents);
      MFC_DEBUG_PRINTF(" WholePkt events : %d \n", mfentry->numWholePktEvents);
      MFC_DEBUG_PRINTF(" Wrong-iif events: %d \n", mfentry->numWrongIfEvents);

      MFC_DEBUG_PRINTF("\n ");
      }
    }
    else
    {
      MFC_DEBUG_PRINTF("\n MFC is empty \n");
    }

  osapiSemaGive(mfcInfo.mfcAvlTree.semId);
  return;
}

/*********************************************************************
* @purpose  Debug Dump of all the entries in Multicast Forwarding Cache
*
* @param    None
*
* @returns  None
*
* @comments
*
* @end
*
*********************************************************************/
void mfcDebugShow(L7_int32 count)
{
  mfcCacheEntry_t *mfentry = L7_NULLPTR;
  mfcCacheEntry_t tempEntry;
  L7_uchar8 grpDump[40];
  L7_uchar8 srcDump[40];
  L7_uchar8 dummyStr[20] = "                    ";
  L7_uint32 i, mfcNumCacheEntries;
  L7_BOOL   bOif;
  L7_int32 localcount = 0, mfcNegativeEntryCount = 0;  
  L7_uchar8 srcStr[20] = "Source Address";
  L7_uchar8 grpStr[20] = "Group Address";
  L7_uchar8 proStr[20] = "Protocol";
  L7_uchar8 hwStr[20] = "AddedToHw";
  L7_uchar8 iifStr[20] = "IIF";
  L7_uchar8 oifStr[20] = "OIF-List";

  /* Lock the Table */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to take MFC semaphore");
    return;
  }

  mfcNumCacheEntries = avlTreeCount(&(mfcInfo.mfcAvlTree));

  MFC_DEBUG_PRINTF("\n __________ MFC CACHE TABLE ______________\n");
  MFC_DEBUG_PRINTF("\nMFC IPv4 Mode   : %s", ((mfcInfo.mfcV4Enabled)  ? ("Enabled") : ("Disabled")));
  MFC_DEBUG_PRINTF("\nMFC IPv6 Mode   : %s", ((mfcInfo.mfcV6Enabled)  ? ("Enabled") : ("Disabled")));
  MFC_DEBUG_PRINTF("\nMFC Entry Count : %d", mfcNumCacheEntries );
  MFC_DEBUG_PRINTF("\nH/W Entry Count : %d", mfcCountersValueGet(MFC_NUM_HW_CACHE_ENTRIES));
  MFC_DEBUG_PRINTF("\nNew Entry Count : %d", mfcCountersValueGet(MFC_NUM_NEW_CACHE_ENTIRES));
  MFC_DEBUG_PRINTF("\nPkts Forwarded  : %d", mfcCountersValueGet(MFC_NUM_FORWARDED_PKTS));
  MFC_DEBUG_PRINTF("\n");

  if (mfcNumCacheEntries <= 0)
  {
    osapiSemaGive(mfcInfo.mfcAvlTree.semId);
    return;
  }

  memset(&tempEntry, 0 , sizeof(mfcCacheEntry_t));
  inetAddressZeroSet(L7_AF_INET, &tempEntry.source);
  inetAddressZeroSet(L7_AF_INET, &tempEntry.group);

  MFC_DEBUG_PRINTF("\n-------------------- -------------------- ---------- --------- --- ------------------\n");
  MFC_DEBUG_PRINTF ("%-20s ", srcStr);
  MFC_DEBUG_PRINTF ("%-20s ", grpStr);
  MFC_DEBUG_PRINTF ("%-10s ", proStr);
  MFC_DEBUG_PRINTF ("%-9s ", hwStr);
  MFC_DEBUG_PRINTF ("%-3s ", iifStr);
  MFC_DEBUG_PRINTF ("%-8s", oifStr);
  MFC_DEBUG_PRINTF("\n-------------------- -------------------- ---------- --------- --- ------------------\n");

  while ((mfentry = (mfcCacheEntry_t*)
                    avlSearchLVL7 (&(mfcInfo.mfcAvlTree), (void *)(&tempEntry),
                                   AVL_NEXT))
                                != L7_NULLPTR)
  {
    if ((count != L7_NULL) && (localcount >= count))
      break;

    localcount++;

    inetCopy(&(tempEntry.source), &(mfentry->source));
    inetCopy(&(tempEntry.group), &(mfentry->group));

    if (inetAddrHtop (&(mfentry->source), srcDump) == L7_SUCCESS)
      MFC_DEBUG_PRINTF ("%-20s ", srcDump);
    else
      MFC_DEBUG_PRINTF ("%-20s ", dummyStr);

    if (inetAddrHtop (&(mfentry->group), grpDump) == L7_SUCCESS)
      MFC_DEBUG_PRINTF ("%-20s ", grpDump);
    else
      MFC_DEBUG_PRINTF ("%-20s ", dummyStr);

    MFC_DEBUG_PRINTF ("%-10s ", ((mfentry->mcastProtocol == 1) ? ("DVMRP") :
                                 ((mfentry->mcastProtocol == 2) ? ("PIM-DM") :
                                  ((mfentry->mcastProtocol == 3) ? ("PIM-SM") :
                                   ((mfentry->mcastProtocol == 4) ? ("MGMD-PROXY"):
                                    ("Unknown"))))));

    MFC_DEBUG_PRINTF ("%-9s ", mfentry->addedToHwTable ? ("TRUE"): ("FALSE"));

    MFC_DEBUG_PRINTF("%-3d ", mfentry->iif);

    BITX_IS_EMPTY(&(mfentry->oif), bOif);
    MFC_DEBUG_PRINTF("[");
    if (bOif != L7_TRUE)
    {
      for (i = 0; i <= MCAST_MAX_INTERFACES; i++)
      {
        if (BITX_TEST (&mfentry->oif, i) != 0)
        {
          MFC_DEBUG_PRINTF("%d ", i);
        }
      }
    }
    else
    {
      MFC_DEBUG_PRINTF("None");
      mfcNegativeEntryCount++;
    }

    MFC_DEBUG_PRINTF("]");
    MFC_DEBUG_PRINTF("\n");
  }
  osapiSemaGive(mfcInfo.mfcAvlTree.semId);
  MFC_DEBUG_PRINTF("\nMFC Negative Entry Count : %d", mfcNegativeEntryCount );  
  return;
}

/*********************************************************************
* @purpose  Debug Dump of all the entries in Multicast Forwarding Cache
*
* @param    None
*
* @returns  None
*
* @comments
*
* @end
*
*********************************************************************/
void mfcDebugNegativeEntriesShow(L7_int32 count)
{
  mfcCacheEntry_t *mfentry = L7_NULLPTR;
  mfcCacheEntry_t tempEntry;
  L7_uchar8 grpDump[40];
  L7_uchar8 srcDump[40];
  L7_uchar8 dummyStr[20] = "                    ";
  L7_uint32 mfcNumCacheEntries;
  L7_BOOL   bOif;
  L7_int32 localcount = 0, mfcNegativeEntryCount = 0;  
  L7_uchar8 srcStr[20] = "Source Address";
  L7_uchar8 grpStr[20] = "Group Address";
  L7_uchar8 proStr[20] = "Protocol";
  L7_uchar8 hwStr[20] = "AddedToHw";
  L7_uchar8 iifStr[20] = "IIF";
  L7_uchar8 oifStr[20] = "OIF-List";

  /* Lock the Table */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to take MFC semaphore");
    return;
  }

  mfcNumCacheEntries = avlTreeCount(&(mfcInfo.mfcAvlTree));

  MFC_DEBUG_PRINTF("\n __________ MFC CACHE TABLE ______________\n");
  MFC_DEBUG_PRINTF("\nMFC IPv4 Mode   : %s", ((mfcInfo.mfcV4Enabled)  ? ("Enabled") : ("Disabled")));
  MFC_DEBUG_PRINTF("\nMFC IPv6 Mode   : %s", ((mfcInfo.mfcV6Enabled)  ? ("Enabled") : ("Disabled")));
  MFC_DEBUG_PRINTF("\nMFC Entry Count : %d", mfcNumCacheEntries );
  MFC_DEBUG_PRINTF("\nH/W Entry Count : %d", mfcCountersValueGet(MFC_NUM_HW_CACHE_ENTRIES));
  MFC_DEBUG_PRINTF("\nNew Entry Count : %d", mfcCountersValueGet(MFC_NUM_NEW_CACHE_ENTIRES));
  MFC_DEBUG_PRINTF("\nPkts Forwarded  : %d", mfcCountersValueGet(MFC_NUM_FORWARDED_PKTS));
  MFC_DEBUG_PRINTF("\n");

  if (mfcNumCacheEntries <= 0)
  {
    osapiSemaGive(mfcInfo.mfcAvlTree.semId);
    return;
  }

  memset(&tempEntry, 0 , sizeof(mfcCacheEntry_t));
  inetAddressZeroSet(L7_AF_INET, &tempEntry.source);
  inetAddressZeroSet(L7_AF_INET, &tempEntry.group);

  MFC_DEBUG_PRINTF("\n-------------------- -------------------- ---------- --------- --- ------------------\n");
  MFC_DEBUG_PRINTF ("%-20s ", srcStr);
  MFC_DEBUG_PRINTF ("%-20s ", grpStr);
  MFC_DEBUG_PRINTF ("%-10s ", proStr);
  MFC_DEBUG_PRINTF ("%-9s ", hwStr);
  MFC_DEBUG_PRINTF ("%-3s ", iifStr);
  MFC_DEBUG_PRINTF ("%-8s", oifStr);
  MFC_DEBUG_PRINTF("\n-------------------- -------------------- ---------- --------- --- ------------------\n");

  while ((mfentry = (mfcCacheEntry_t*)
                    avlSearchLVL7 (&(mfcInfo.mfcAvlTree), (void *)(&tempEntry),
                                   AVL_NEXT))
                                != L7_NULLPTR)
  {
    if ((count != L7_NULL) && (localcount >= count))
      break;

    localcount++;

    inetCopy(&(tempEntry.source), &(mfentry->source));
    inetCopy(&(tempEntry.group), &(mfentry->group));

    BITX_IS_EMPTY(&(mfentry->oif), bOif);
    if (bOif != L7_TRUE)
    {
      continue;
    }

    if (inetAddrHtop (&(mfentry->source), srcDump) == L7_SUCCESS)
      MFC_DEBUG_PRINTF ("%-20s ", srcDump);
    else
      MFC_DEBUG_PRINTF ("%-20s ", dummyStr);

    if (inetAddrHtop (&(mfentry->group), grpDump) == L7_SUCCESS)
      MFC_DEBUG_PRINTF ("%-20s ", grpDump);
    else
      MFC_DEBUG_PRINTF ("%-20s ", dummyStr);

    MFC_DEBUG_PRINTF ("%-10s ", ((mfentry->mcastProtocol == 1) ? ("DVMRP") :
                                 ((mfentry->mcastProtocol == 2) ? ("PIM-DM") :
                                  ((mfentry->mcastProtocol == 3) ? ("PIM-SM") :
                                   ((mfentry->mcastProtocol == 4) ? ("MGMD-PROXY"):
                                    ("Unknown"))))));

    MFC_DEBUG_PRINTF ("%-9s ", mfentry->addedToHwTable ? ("TRUE"): ("FALSE"));

    MFC_DEBUG_PRINTF("%-3d ", mfentry->iif);

    MFC_DEBUG_PRINTF("[");
    MFC_DEBUG_PRINTF("None");
    mfcNegativeEntryCount++;

    MFC_DEBUG_PRINTF("]");
    MFC_DEBUG_PRINTF("\n");
  }

  osapiSemaGive(mfcInfo.mfcAvlTree.semId);
  MFC_DEBUG_PRINTF("\nMFC Negative Entry Count : %d", mfcNegativeEntryCount );  
  return;
}

/*********************************************************************
* @purpose  Brief Debug Dump of all the entries in Multicast Forwarding Cache
*
* @param    None
*
* @returns  None
*
* @comments
*
* @end
*
*********************************************************************/
void mfcDebugBriefShow(void)
{
  L7_uint32 mfcNumCacheEntries;
  mfcCacheEntry_t *mfentry = L7_NULLPTR;
  mfcCacheEntry_t tempEntry;
  L7_BOOL   bOif;
  L7_int32 mfcNegativeEntryCount = 0;  

  /* Lock the Table */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to take MFC semaphore");
    return;
  }

  mfcNumCacheEntries = avlTreeCount(&(mfcInfo.mfcAvlTree));

  MFC_DEBUG_PRINTF("\n __________ MFC CACHE TABLE ______________\n");
  MFC_DEBUG_PRINTF("\nMFC IPv4 Mode   : %s", ((mfcInfo.mfcV4Enabled)  ? ("Enabled") : ("Disabled")));
  MFC_DEBUG_PRINTF("\nMFC IPv6 Mode   : %s", ((mfcInfo.mfcV6Enabled)  ? ("Enabled") : ("Disabled")));
  MFC_DEBUG_PRINTF("\nMFC Entry Count : %d", mfcNumCacheEntries );
  MFC_DEBUG_PRINTF("\nH/W Entry Count : %d", mfcCountersValueGet(MFC_NUM_HW_CACHE_ENTRIES));
  MFC_DEBUG_PRINTF("\nNew Entry Count : %d", mfcCountersValueGet(MFC_NUM_NEW_CACHE_ENTIRES));
  MFC_DEBUG_PRINTF("\nPkts Forwarded  : %d", mfcCountersValueGet(MFC_NUM_FORWARDED_PKTS));
  MFC_DEBUG_PRINTF("\n");

  while ((mfentry = (mfcCacheEntry_t*)
                    avlSearchLVL7 (&(mfcInfo.mfcAvlTree), (void *)(&tempEntry),
                                   AVL_NEXT))
                                != L7_NULLPTR)
  {
    inetCopy(&(tempEntry.source), &(mfentry->source));
    inetCopy(&(tempEntry.group), &(mfentry->group));

    BITX_IS_EMPTY(&(mfentry->oif), bOif);
    if (bOif == L7_TRUE)
    {
      mfcNegativeEntryCount++;
    }
  }

  MFC_DEBUG_PRINTF("\nMFC Negative Entry Count : %d", mfcNegativeEntryCount);

  osapiSemaGive(mfcInfo.mfcAvlTree.semId);
  return;
}

/*********************************************************************
* @purpose  Get the number of entries in multicast forwarding cache
*
* @param    pCount    @b{(output)} Multicast Forwarding Cache Count
*
* @returns  L7_SUCCESS: MFC Count is returned
* @returns  L7_FAILURE:MFC Count is not returned
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t mfcEntryCountGet(L7_uint32 *pCount)
{
  if(pCount == L7_NULLPTR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n invalid input parameters");
    return L7_FAILURE;
  }

  *pCount = 0;

  /* Lock the table */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to take MFC semaphore");
    return L7_FAILURE;
  }
  *pCount = avlTreeCount(&(mfcInfo.mfcAvlTree));
  osapiSemaGive(mfcInfo.mfcAvlTree.semId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Debug Dump of all the entries in Multicast Forwarding Cache
*
* @param    None
*
* @returns  None
*
* @comments
*
* @end
*
*********************************************************************/
void mfcDebugAllEntriesDelete(void)
{
  mfcCacheEntry_t *pEntry = L7_NULLPTR;
  mfcCacheEntry_t entry;
  L7_uint32 mfcNumCacheEntries;
  L7_uchar8 grp[IPV6_DISP_ADDR_LEN];
  L7_uchar8 src[IPV6_DISP_ADDR_LEN];

  /* Lock the Table */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG_PRINTF ("Failed to take MFC semaphore");
    return;
  }

  mfcNumCacheEntries = avlTreeCount(&(mfcInfo.mfcAvlTree));

  if (mfcNumCacheEntries <= 0)
  {
    osapiSemaGive(mfcInfo.mfcAvlTree.semId);
    return;
  }

  memset(&entry, 0, sizeof(mfcCacheEntry_t));
  inetAddressZeroSet(L7_AF_INET, &entry.source);
  inetAddressZeroSet(L7_AF_INET, &entry.group);

  while (L7_NULLPTR !=
         (pEntry = (mfcCacheEntry_t *)avlSearchLVL7(&(mfcInfo.mfcAvlTree), 
                                                    (void *)(&entry), AVL_NEXT)))
  {
    inetCopy(&(entry.source), &(pEntry->source));
    inetCopy(&(entry.group), &(pEntry->group));

    if (pEntry->addedToHwTable == L7_TRUE)
    {
      /* Remove the entry from H/W */
      mfcHwEntryDelete(&(pEntry->source), &(pEntry->group), pEntry->iif,
                       pEntry->mcastProtocol);
      mfcCountersUpdate (MFC_NUM_HW_CACHE_ENTRIES, MFC_COUNTERS_DECREMENT);
    }
  
    /* Remove the entry */
    MFC_DEBUG_PRINTF ("Deleted Entry with S-%s, G-%s, I-%d.\n",
                      inetAddrPrint(&pEntry->source,src),
                      inetAddrPrint(&pEntry->group,grp), pEntry->iif); 
    avlDeleteEntry(&(mfcInfo.mfcAvlTree), (void *)pEntry);
    if (pEntry->newRoute == L7_TRUE)  
    {
       mfcCountersUpdate (MFC_NUM_NEW_CACHE_ENTIRES, MFC_COUNTERS_DECREMENT);
    }      
  }

  osapiSemaGive(mfcInfo.mfcAvlTree.semId);
  return;
}

/*********************************************************************
* @purpose  Debug Dump of all the entries in Multicast Forwarding Cache
*
* @param    None
*
* @returns  None
*
* @comments
*
* @end
*
*********************************************************************/
void mfcDebugAllNegativeEntriesDelete(void)
{
  mfcCacheEntry_t *pEntry = L7_NULLPTR;
  mfcCacheEntry_t entry;
  L7_uint32 mfcNumCacheEntries;
  L7_uchar8 grp[IPV6_DISP_ADDR_LEN];
  L7_uchar8 src[IPV6_DISP_ADDR_LEN];
  L7_BOOL isEmpty = L7_FALSE;

  /* Lock the Table */
  if (osapiSemaTake(mfcInfo.mfcAvlTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MFC_DEBUG_PRINTF ("Failed to take MFC semaphore");
    return;
  }

  mfcNumCacheEntries = avlTreeCount(&(mfcInfo.mfcAvlTree));

  if (mfcNumCacheEntries <= 0)
    return;

  memset(&entry, 0, sizeof(mfcCacheEntry_t));
  inetAddressZeroSet(L7_AF_INET, &entry.source);
  inetAddressZeroSet(L7_AF_INET, &entry.group);

  while (L7_NULLPTR !=
         (pEntry = (mfcCacheEntry_t *)avlSearchLVL7(&(mfcInfo.mfcAvlTree), 
                                                    (void *)(&entry), AVL_NEXT)))
  {

    inetCopy(&(entry.source), &(pEntry->source));
    inetCopy(&(entry.group), &(pEntry->group));

    BITX_IS_EMPTY (&pEntry->oif, isEmpty);
    if (isEmpty == L7_TRUE)
    {
      if (pEntry->addedToHwTable == L7_TRUE)
      {
        /* Remove the entry from H/W */
        mfcHwEntryDelete(&(pEntry->source), &(pEntry->group), pEntry->iif,
                         pEntry->mcastProtocol);
        mfcCountersUpdate (MFC_NUM_HW_CACHE_ENTRIES, MFC_COUNTERS_DECREMENT);
      }

      /* Remove the entry */
      MFC_DEBUG_PRINTF ("Deleted Entry with S-%s, G-%s, I-%d.\n",
                        inetAddrPrint(&pEntry->source,src),
                        inetAddrPrint(&pEntry->group,grp), pEntry->iif); 
      avlDeleteEntry(&(mfcInfo.mfcAvlTree), (void *)pEntry);
    }
  }
  osapiSemaGive(mfcInfo.mfcAvlTree.semId);
  return;
}

/*********************************************************************
* @purpose  Compute the memory allocated by the MFC component
*
* @param    void
*
* @returns  void
*
* @notes 
*
* @end
*********************************************************************/
void mfcDebugMemoryInfoCompute(void)
{
  sysapiPrintf ("\n***** Memory Allocated By MFC Component *****\n");
  sysapiPrintf ("        MFC Memory Info\n");
  sysapiPrintf ("          mfcInfo_t                 - %d\n", sizeof(mfcInfo_t));
  mfcTotalBytesAllocated += sizeof(mfcInfo_t);
  sysapiPrintf ("          mfcRegisterList_t         - %d\n", sizeof(mfcRegisterList_t));
  mfcTotalBytesAllocated += sizeof(mfcRegisterList_t);
  sysapiPrintf ("          AVL Tree Heap             - %d\n", sizeof(avlTreeTables_t) * L7_MULTICAST_FIB_MAX_ENTRIES);
  mfcTotalBytesAllocated += sizeof(avlTreeTables_t) * L7_MULTICAST_FIB_MAX_ENTRIES;
  sysapiPrintf ("          AVL Data Heap             - %d\n", sizeof(mfcCacheEntry_t) * L7_MULTICAST_FIB_MAX_ENTRIES);
  mfcTotalBytesAllocated += sizeof(mfcCacheEntry_t) * L7_MULTICAST_FIB_MAX_ENTRIES;
  sysapiPrintf ("\n");
  sysapiPrintf ("        Total Memory                - %d\n", mfcTotalBytesAllocated);
  sysapiPrintf ("\n");
  sysapiPrintf ("***************************************************\n");

  return;
}

