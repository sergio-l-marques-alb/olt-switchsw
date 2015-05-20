/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_mrt.c
*
* @purpose    PIM-DM MRT (S,G) Tree Database Maintenance Routines
*
* @component  PIM-DM
*
* @comments   none
*
* @create
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "osapi.h"
#include "l3_addrdefs.h"
#include "l7apptimer_api.h"
#include "heap_api.h"
#include "pimdm_main.h"
#include "pimdm_debug.h"
#include "pimdm_mrt.h"
#include "mfc_api.h"
#include "pimdm_upstrm_fsm.h"
#include "pimdm_dnstrm_fsm.h"
#include "pimdm_strfr_fsm.h"
#include "pimdm_asrt_fsm.h"
#include "pimdm_intf.h"
#include "pimdm_util.h"
#include "pimdm_mgmd.h"
#include "pimdm_ctrl.h"
#include "pimdm_admin_scope.h"

/*******************************************************************************
**                        Function Declarations                               **
*******************************************************************************/
static L7_int32
pimdmMrtEntryCompare (const void* pData1,
                      const void* pData2,
                      size_t size);
static L7_RC_t
pimdmMrtMfcEntryUpdate (pimdmCB_t *pimdmCB,
                        L7_inet_addr_t *srcAddr,
                        L7_inet_addr_t *grpAddr,
                        L7_uint32 rtrIfNum,
                        interface_bitset_t *oList);
static L7_RC_t
pimdmMrtMfcEntryDelete (pimdmCB_t *pimdmCB,
                        L7_inet_addr_t *srcAddr,
                        L7_inet_addr_t *grpAddr,
                        L7_uint32 rtrIfNum);
static L7_RC_t
pimdmMrtOifInterfaceSet (pimdmMrtEntry_t *mrtEntry,
                         L7_uint32 rtrIfNum,
                         L7_BOOL flag);
static L7_RC_t
pimdmMrtOifListComputeAndApply (pimdmCB_t *pimdmCB,
                                pimdmMrtEntry_t *mrtEntry);
static L7_RC_t
pimdmMrtRPFChangeProcess (pimdmMrtEntry_t *mrtEntry,
                          mcastRPFInfo_t *bestRoute);
static void
pimdmMrtEntryStateRfrTimersStart (pimdmMrtEntry_t *mrtEntry,
                                  L7_uint32 ttl);
static L7_RC_t
pimdmMrtEntryDelete (pimdmCB_t *pimdmCB,
                     pimdmMrtEntry_t *mrtEntry);
static void
pimdmMrtEntryExpiryTimerHandler (void *handle);

static void
pimdmMrtUpstrmIntfDownProcess (pimdmMrtEntry_t *mrtEntry,
                               L7_uint32 rtrIfNum);
static void
pimdmMrtDownstrmIntfDownProcess (pimdmMrtEntry_t *mrtEntry,
                                 L7_uint32 rtrIfNum);
static L7_RC_t
pimdmMrtEntryStatesReset (pimdmMrtEntry_t *mrtEntry,
                          L7_BOOL forceFree);

static L7_RC_t
pimdmMrtDownstrmNeighborDownProcess (pimdmMrtEntry_t *mrtEntry,
                                     L7_uint32 rtrIfNum,
                                     L7_inet_addr_t *nbrAddr);

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose   Compare function for the MRT SC tree
*
* @param     pData1 @b{ (input) } Pointer to MRT Entry
*            pData2 @b{ (input) } Pointer to MRT Entry
*            size   @b{ (input) } Size for the comparision
*
* @returns   > 0  if pData1 > pData2
*            = 0 if pData1 == pData2
*            < 0 if pData1 < pData2
*
* @comments  None
*
* @end
*********************************************************************/
static L7_int32
pimdmMrtEntryCompare (const void* pData1,
                      const void* pData2,
                      size_t size)
{
  pimdmMrtEntry_t *pKey1 = (pimdmMrtEntry_t *) pData1;
  pimdmMrtEntry_t *pKey2 = (pimdmMrtEntry_t *) pData2;
  register L7_int32 retVal = 0;

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");*/

  if ((pKey1 == L7_NULLPTR) || (pKey2 == L7_NULLPTR))
  {
    return 1;
  }

  if ((retVal = L7_INET_ADDR_COMPARE (&(pKey1->grpAddr),
                                      &(pKey2->grpAddr))) != 0)
  {
    return retVal;
  }

  retVal = L7_INET_ADDR_COMPARE (&(pKey1->srcAddr), &(pKey2->srcAddr));

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");*/
  return retVal;
}

/*********************************************************************
*
* @purpose  Update the MFC Entry
*
* @param    pimdmCB  @b{ (input) } Pointer PIMDM Control Block
*           srcAddr  @b{ (input) } Pointer Source Address
*           grpAddr  @b{ (input) } Pointer Group Address
*           rtrIfNum @b{ (input) } Incoming Interface
*           oList    @b{ (input) } Outgoing Interface List.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
static L7_RC_t
pimdmMrtMfcEntryUpdate (pimdmCB_t *pimdmCB,
                        L7_inet_addr_t *srcAddr,
                        L7_inet_addr_t *grpAddr,
                        L7_uint32 rtrIfNum,
                        interface_bitset_t *oList)
{
  mfcEntry_t mfcEntry;
  L7_RC_t retVal = L7_FAILURE;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uint32 index = 0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic Validations */
  if (L7_NULLPTR == pimdmCB)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid PIM-DM Control Block");
    return L7_FAILURE;
  }

  if (rtrIfNum == 0)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Trying to Update Stale Entry for "
                 "(%s, %s) with rtrIfNum - %d", inetAddrPrint(srcAddr,src),
                 inetAddrPrint(grpAddr,grp), rtrIfNum);
    return L7_SUCCESS;
  }

  memset (&mfcEntry,0,sizeof (mfcEntry));
  inetAddressZeroSet(pimdmCB->addrFamily,&(mfcEntry.source));
  inetAddressZeroSet(pimdmCB->addrFamily,&(mfcEntry.group));

  inetCopy (&mfcEntry.group,grpAddr);
  inetCopy (&mfcEntry.source,srcAddr);
  mfcEntry.iif = rtrIfNum;
  BITX_COPY (oList, &(mfcEntry.oif));
  mfcEntry.mcastProtocol = L7_MRP_PIMDM;

  if (pimdmDebugFlagCheck (PIMDM_DEBUG_MFC) == L7_TRUE)
  {
    PIMDM_DEBUG_PRINTF ("\nUpdating MFC ...");
    PIMDM_DEBUG_PRINTF ("S-%s, G-%s, IIF-%d, OIF List  - ",
                inetAddrPrint(srcAddr, src), inetAddrPrint(grpAddr, grp),
                rtrIfNum);
    for (index = 1; index <= PIMDM_INTF_BIT_SIZE; index++)
    {
      if (BITX_TEST (oList, index) != 0)
      {
        PIMDM_DEBUG_PRINTF ("%02d ", index);
      }
    }
    PIMDM_DEBUG_PRINTF ("\n");
  }

  retVal = mfcMessagePost (MFC_UPDATE_ENTRY, (void *) &mfcEntry,
                           sizeof(mfcEntry_t));

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Delete the MFC Entry
*
* @param    pimdmCB  @b{ (input) } Pointer PIMDM Control Block
*           srcAddr  @b{ (input) } Pointer Source Address
*           grpAddr  @b{ (input) } Pointer Group Address
*           rtrIfNum @b{ (input) } Incoming Interface
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
static L7_RC_t
pimdmMrtMfcEntryDelete (pimdmCB_t *pimdmCB,
                        L7_inet_addr_t *srcAddr,
                        L7_inet_addr_t *grpAddr,
                        L7_uint32 rtrIfNum)
{
  mfcEntry_t mfcEntry;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_RC_t retVal = L7_FAILURE;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (rtrIfNum == 0)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Trying to Delete Stale Entry for "
                 "(%s, %s) with rtrIfNum - %d", inetAddrPrint(srcAddr,src),
                 inetAddrPrint(grpAddr,grp), rtrIfNum);
    return L7_SUCCESS;
  }

  memset (&mfcEntry,0,sizeof (mfcEntry));
  inetAddressZeroSet(pimdmCB->addrFamily,&(mfcEntry.source));
  inetAddressZeroSet(pimdmCB->addrFamily,&(mfcEntry.group));

  inetCopy (&mfcEntry.group,grpAddr);
  inetCopy (&mfcEntry.source,srcAddr);
  mfcEntry.iif = rtrIfNum;
  mfcEntry.mcastProtocol = L7_MRP_PIMDM;

  if (pimdmDebugFlagCheck (PIMDM_DEBUG_MFC) == L7_TRUE)
  {
    PIMDM_DEBUG_PRINTF ("\nDeleting from MFC ...");
    PIMDM_DEBUG_PRINTF ("S-%s, G-%s, IIF-%d",
                inetAddrPrint(srcAddr, src), inetAddrPrint(grpAddr, grp),
                rtrIfNum);
    PIMDM_DEBUG_PRINTF ("\n");
  }

  retVal = mfcMessagePost (MFC_DELETE_ENTRY, (void *) &mfcEntry,
                           sizeof(mfcEntry_t));

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/******************************************************************************
* @purpose  Reset/Set the OIF Interface List of a MRT Entry
*
* @param    mrtEntry  @b{ (input) } Pointer PIMDM MRT Entry
*           rtrIfNum  @b{ (input) } Interface that has to be updated
*           flag      @b{ (input) } TRUE to set, FALSE to reset
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None
*
* @end
******************************************************************************/
static L7_RC_t
pimdmMrtOifInterfaceSet (pimdmMrtEntry_t *mrtEntry,
                         L7_uint32 rtrIfNum,
                         L7_BOOL flag)
{
  L7_BOOL isNULL = L7_FALSE;
  pimdmUpstrmEventInfo_t upstrmFsmEventInfo;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  BITX_IS_EMPTY (&mrtEntry->oifList,isNULL);

  if (flag == L7_FALSE)
  {
    BITX_RESET (&mrtEntry->oifList,rtrIfNum);
    if (isNULL == L7_FALSE)
    {
      BITX_IS_EMPTY (&mrtEntry->oifList,isNULL);
      if (isNULL == L7_TRUE)
      {
        /*************** UPSTREAM INTERFACE FSM EVENTS *********************/

        /* Prepare the Upstream Event Information.
         */
        upstrmFsmEventInfo.rtrIfNum = mrtEntry->upstrmRtrIfNum;

        /* EVENT - OIF(S,G) is NULL.
         */
        pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_OIF_NULL, mrtEntry,
                               &upstrmFsmEventInfo);
      }
    }
  }
  else
  {
    BITX_SET (&mrtEntry->oifList,rtrIfNum);
    if (isNULL == L7_TRUE)
    {
      BITX_IS_EMPTY (&mrtEntry->oifList,isNULL);
      if (isNULL == L7_FALSE)
      {
        /*************** UPSTREAM INTERFACE FSM EVENTS *********************/

        /* Prepare the Upstream Event Information.
         */
        upstrmFsmEventInfo.rtrIfNum = mrtEntry->upstrmRtrIfNum;

        /* EVENT - OIF(S,G) is not NULL.
         */
        pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_OIF_NOT_NULL, mrtEntry,
                               &upstrmFsmEventInfo);
      }
    }
  }

  pimdmMrtOifListCompute (mrtEntry->pimdmCB, &mrtEntry->srcAddr,
                          &mrtEntry->grpAddr, PIMDM_MRT_CHANGE_MAX_STATES);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Compute the Outgoing Interface List and update the MFC
*
* @param    pimdmCB  @b{ (input) } Pointer PIMDM Control Block
*           mrtEntry @b{ (input) } Pointer PIMDM MRT Entry
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments The logic for this calculation is mentioned in RFC 3973 in
*           section 4.1.3
*
* @end
*********************************************************************/
static L7_RC_t
pimdmMrtOifListComputeAndApply (pimdmCB_t *pimdmCB,
                                pimdmMrtEntry_t *mrtEntry)
{
  interface_bitset_t oList;
  interface_bitset_t tempList;
  pimdmUpstrmEventInfo_t upstrmFsmEventInfo;
  L7_BOOL entryoListIsNull = L7_FALSE;
  L7_BOOL oListIsNull = L7_FALSE;
  L7_RC_t retVal = L7_SUCCESS;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  BITX_RESET_ALL (&oList);
  BITX_RESET_ALL (&tempList);

  BITX_COPY (&pimdmCB->nbrBitmap, &oList);  /* pim Neighbors */
  BITX_SUBTRACT (&oList, &mrtEntry->downstrmPruneMask, &oList); /* Prune(S,G) */
  BITX_SUBTRACT (&mrtEntry->pimIncludeStarG, &mrtEntry->pimExcludeSG, &tempList); /* include(*,g) - exclude(s,g) */
  BITX_ADD (&oList, &tempList, &oList);
  BITX_ADD (&oList, &mrtEntry->pimIncludeSG, &oList);
  BITX_SUBTRACT (&oList, &mrtEntry->lostAssertMask, &oList); /* lost_assert(S,G) */
  BITX_SUBTRACT (&oList, &mrtEntry->boundaryGMask, &oList);  /* Boundary for G section 4.1.3 */

  /* Reset the Upstream RPF index from the outgoing interface list */
  BITX_RESET (&oList, mrtEntry->upstrmRtrIfNum);

  /* Calculate the olist and apply it to the MFC if there is any change */
  if (BITX_COMPARE (&oList, &mrtEntry->oifList) != 0)
  {
    /* update the oif list in the entry */
    BITX_IS_EMPTY (&oList, oListIsNull);
    BITX_IS_EMPTY (&mrtEntry->oifList, entryoListIsNull);

    /*************** UPSTREAM FSM EVENTS *********************/

    /* Prepare the Upstream Event Information.
     */
    memset (&upstrmFsmEventInfo,0,sizeof (pimdmUpstrmEventInfo_t));
    upstrmFsmEventInfo.rtrIfNum = mrtEntry->upstrmRtrIfNum;

    if ((oListIsNull == L7_TRUE) && (entryoListIsNull == L7_FALSE))
    {
      /* EVENT: olist(S,G)->NULL.
       */
      pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_OIF_NULL, mrtEntry,
                             &upstrmFsmEventInfo);
    }
    else if ((oListIsNull == L7_FALSE) && (entryoListIsNull == L7_TRUE))
    {
      /* EVENT: olist(S,G)->non-NULL.
       */
      pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_OIF_NOT_NULL, mrtEntry,
                             &upstrmFsmEventInfo);
    }
    BITX_COPY (&oList, &mrtEntry->oifList);
  }

  retVal = pimdmMrtMfcEntryUpdate (pimdmCB, &mrtEntry->srcAddr,
                                   &mrtEntry->grpAddr,
                                   mrtEntry->upstrmRtrIfNum, &oList);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Apply the RPF changes to the MRT Entry
*
* @param    mrtEntry    @b{ (input) } Pointer MRT Entry
*           bestRoute   @b{ (input) } Pointer to the RTO best route
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
static L7_RC_t
pimdmMrtRPFChangeProcess (pimdmMrtEntry_t *mrtEntry,
                          mcastRPFInfo_t *bestRoute)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  L7_BOOL isDirectlyConnected = L7_FALSE,bIsNull = L7_FALSE;
  pimdmAssertEventInfo_t asrtFsmEventInfo;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 nxt[PIMDM_MAX_DBG_ADDR_SIZE];
  mcastRPFInfo_t newBestRoute;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (mrtEntry == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid MRT Entry - NULL");
    return L7_FAILURE;
  }

  if ((pimdmCB = mrtEntry->pimdmCB) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid PIMDM CB - NULL");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_RTO, "RPF Change Event [%d] received for rpfAddr - %s "
               "with rpfNexthop - %s, rpfIfIndex - %d, rpfMetric - %d, rpfPref - %d",
               bestRoute->status, inetAddrPrint (&bestRoute->rpfRouteAddress, src),
               inetAddrPrint (&bestRoute->rpfNextHop, nxt),
               bestRoute->rpfIfIndex,
               bestRoute->rpfRouteMetric, bestRoute->rpfRouteMetricPref);

  memset(&asrtFsmEventInfo,0,sizeof(pimdmAssertEventInfo_t));
  asrtFsmEventInfo.rtrIfNum = bestRoute->rpfIfIndex;
  asrtFsmEventInfo.interval = PIMDM_DEFAULT_ASSERT_TIMEOUT;
  inetAddressZeroSet(pimdmCB->addrFamily,&(asrtFsmEventInfo.assertAddr));

  if (bestRoute->status == RTO_DELETE_ROUTE)
  {
    /* Looks like the Upstream Route is deleted.  Delete the MRT Entry.
     */
    PIMDM_TRACE (PIMDM_DEBUG_RTO, "RTO Route Delete Event for Src - %s "
                 "with Nexthop - %s", inetAddrPrint (&mrtEntry->srcAddr, src),
                 inetAddrPrint (&mrtEntry->rpfInfo.rpfNextHop, nxt));

    /* PIMDM doesn't act on Default routes when a Default Route Add Event is
     * received. So, When a Route Delete event for a Source Address is received,
     * check for the existence of a Default route. If present, update the new
     * RPF Nexthop and proceed with processing.
     */
    if(mcastRPFInfoGet(&mrtEntry->srcAddr, &newBestRoute) != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "RPF Info Get Failed for Src - %s",
                   inetAddrPrint (&mrtEntry->srcAddr, src));
      if (pimdmMrtEntryRemove (pimdmCB, mrtEntry) != L7_SUCCESS)
      {
        PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "MRT Entry Delete Failed for Src - %s, "
                     "Grp - %s", inetAddrPrint (&mrtEntry->srcAddr, src),
                     inetAddrPrint (&mrtEntry->grpAddr, nxt));
        return L7_FAILURE;
      }

      return L7_SUCCESS;
    }

    bestRoute = &newBestRoute;

    PIMDM_TRACE (PIMDM_DEBUG_RTO, "Upon Route Delete, New RPF Change Event [%d] "
                 "received for rpfAddr - %s with rpfNexthop - %s, "
                 "rpfIfIndex - %d, rpfMetric - %d, rpfPref - %d",
                 bestRoute->status, inetAddrPrint (&bestRoute->rpfRouteAddress, src),
                 inetAddrPrint (&bestRoute->rpfNextHop, nxt),
                 bestRoute->rpfIfIndex,
                 bestRoute->rpfRouteMetric, bestRoute->rpfRouteMetricPref);
  }

  /* First check the metric & preference */
  if ((L7_INET_IS_ADDR_EQUAL (&mrtEntry->rpfInfo.rpfNextHop,&bestRoute->rpfNextHop)) &&
                              (mrtEntry->upstrmRtrIfNum == bestRoute->rpfIfIndex))
  {
    PIMDM_TRACE (PIMDM_DEBUG_RTO, "No Change in NextHop and Upstream Index");
    /* check the assert metric and preference */
    if ((mrtEntry->rpfInfo.rpfRouteMetric != bestRoute->rpfRouteMetric) ||
        (mrtEntry->rpfInfo.rpfRouteMetricPref != bestRoute->rpfRouteMetricPref))
    {
      PIMDM_TRACE (PIMDM_DEBUG_RTO, "Change in Metric or MetricPref");

      /* calculate the assert winner */
      if (pimdmMrtAsrtMetricCompare(mrtEntry->rpfInfo.rpfRouteMetricPref,
                                    mrtEntry->rpfInfo.rpfRouteMetric,
                                    &mrtEntry->rpfInfo.rpfNextHop,
                                    bestRoute->rpfRouteMetricPref,
                                    bestRoute->rpfRouteMetric,
                                    &bestRoute->rpfNextHop) == L7_TRUE)
      {
        /*************** ASSERT FSM EVENTS *********************/
        /* EVENT - Receive Inferior Assert from Winner.
         */
        pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_INF_ASRT_ARVL_FROM_WNR, mrtEntry,
                               &asrtFsmEventInfo);
      }

      /* update the metrics and preference */
      mrtEntry->rpfInfo.rpfRouteMetric = bestRoute->rpfRouteMetric;
      mrtEntry->rpfInfo.rpfRouteMetricPref = bestRoute->rpfRouteMetricPref;
      mrtEntry->rpfInfo.rpfRouteProto = bestRoute->rpfRouteProtocol;

      /*Copy the RTO best route information into upstream assert info
        structure */
      inetCopy (&mrtEntry->upstrmNbrInfo.assertWnrAddr, &bestRoute->rpfNextHop);
      mrtEntry->upstrmNbrInfo.assertWnrAsrtMetric = bestRoute->rpfRouteMetric;
      mrtEntry->upstrmNbrInfo.assertWnrAsrtPref = bestRoute->rpfRouteMetricPref;
    }
  }
  else
  {
    if (mrtEntry->upstrmRtrIfNum != bestRoute->rpfIfIndex)
    {
      pimdmDnstrmEventInfo_t dnstrmFsmEventInfo;
      pimdmUpstrmEventInfo_t upstrmFsmEventInfo;
      pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;

      PIMDM_TRACE (PIMDM_DEBUG_RTO, "Change in Upstream Index ... "
                   "bestRoute IIF - %d, mrtEntry upstrmIIF - %d",
                   bestRoute->rpfIfIndex,mrtEntry->upstrmRtrIfNum);

      /* Trigger the RPF inteface change to Downstream FSM -
       * Means downstream interface is being upstream interface */
      mrtEntry->rpfInfo.rpfRtrIfNum = bestRoute->rpfIfIndex;

      /* Update couldAssert Flag */
      if (mrtEntry->downstrmStateInfo[bestRoute->rpfIfIndex] == L7_NULLPTR)
      {
        PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Downstrm Intf - %d",
                     bestRoute->rpfIfIndex);
        return L7_FAILURE;
      }

      asrtIntfInfo =
        &mrtEntry->downstrmStateInfo[bestRoute->rpfIfIndex]->downstrmAssertInfo;
      asrtIntfInfo->couldAssert = L7_FALSE;

      /*************** ASSERT FSM EVENTS *********************/

      /* Prepare the Assert Event Information.
       */
      memset(&asrtFsmEventInfo,0,sizeof(pimdmAssertEventInfo_t));
      inetAddressZeroSet(pimdmCB->addrFamily,&(asrtFsmEventInfo.assertAddr));
      asrtFsmEventInfo.rtrIfNum =  bestRoute->rpfIfIndex;
      asrtFsmEventInfo.interval = PIMDM_DEFAULT_ASSERT_TIMEOUT;
      asrtFsmEventInfo.assertMetric = PIMDM_ASSERT_METRIC_INFINITY;
      asrtFsmEventInfo.assertPref = PIMDM_ASSERT_METRIC_INFINITY;

      /* EVENT - couldAssert is FALSE.
       */
      pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_COULD_ASRT_FALSE, mrtEntry,
                             &asrtFsmEventInfo);

      /* couldAssert is also changed for upstream interface */
      if (mrtEntry->upstrmRtrIfNum != 0)
      {
        memset(&asrtFsmEventInfo,0,sizeof(pimdmAssertEventInfo_t));
        inetAddressZeroSet(pimdmCB->addrFamily,&(asrtFsmEventInfo.assertAddr));
        asrtFsmEventInfo.rtrIfNum = mrtEntry->upstrmRtrIfNum;
        asrtFsmEventInfo.interval = PIMDM_DEFAULT_ASSERT_TIMEOUT;

        if (mrtEntry->downstrmStateInfo[mrtEntry->upstrmRtrIfNum] == L7_NULLPTR)
        {
          PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Downstrm Intf - %d",
                       mrtEntry->upstrmRtrIfNum);
          return L7_FAILURE;
        }

        asrtIntfInfo =
          &mrtEntry->downstrmStateInfo[mrtEntry->upstrmRtrIfNum]->downstrmAssertInfo;
        asrtIntfInfo->couldAssert = L7_TRUE;
        asrtIntfInfo->assertWnrAsrtMetric =PIMDM_ASSERT_METRIC_INFINITY;
        asrtIntfInfo->assertWnrAsrtPref = PIMDM_ASSERT_METRIC_INFINITY;

        PIMDM_TRACE(PIMDM_DEBUG_RTO, "Sending Could Assert TRUE event on ifNum %d",
                    mrtEntry->upstrmRtrIfNum);

        /* EVENT - couldAssert is TRUE.
         */
        pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_COULD_ASRT_TRUE, mrtEntry,
                               &asrtFsmEventInfo);

        /*************** UPSTREAM FSM EVENTS *********************/

        /* Prepare the Upstream Event Information.
         */
        upstrmFsmEventInfo.rtrIfNum = mrtEntry->upstrmRtrIfNum;

        /* Event: PIMDM_UPSTRM_EVT_RPF_INTF_CHANGE.
         */
        pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_RPF_INTF_CHANGE, mrtEntry,
                               &upstrmFsmEventInfo);
      }

      mrtEntry->upstrmRtrIfNum =  bestRoute->rpfIfIndex;
      mrtEntry->rpfInfo.rpfRouteMetric = bestRoute->rpfRouteMetric;
      mrtEntry->rpfInfo.rpfRouteMetricPref = bestRoute->rpfRouteMetricPref;
      mrtEntry->rpfInfo.rpfRouteProto = bestRoute->rpfRouteProtocol;
      mrtEntry->rpfInfo.rpfRtrIfNum = bestRoute->rpfIfIndex;

      /* RPF(S) becomes I event for the Downstream FSM */
      dnstrmFsmEventInfo.rtrIfNum = bestRoute->rpfIfIndex;
      pimdmDnstrmFsmExecute (PIMDM_DNSTRM_EVT_RPF_IF_CHNG, mrtEntry,
                             &dnstrmFsmEventInfo);

      /*update the nexthop */
      inetCopy(&mrtEntry->rpfInfo.rpfNextHop,&bestRoute->rpfNextHop);

      BITX_RESET_ALL (&mrtEntry->boundaryGMask);
      pimdmAdminScopeIntfBitSetGet (pimdmCB, &mrtEntry->grpAddr,
                                    mrtEntry->upstrmRtrIfNum,
                                    &mrtEntry->boundaryGMask);

      pimdmMrtOifListComputeAndApply (pimdmCB, mrtEntry);

      /*Copy the RTO best route information into upstream assert info
        structure */
      inetCopy(&mrtEntry->upstrmNbrInfo.assertWnrAddr, &bestRoute->rpfNextHop);
      mrtEntry->upstrmNbrInfo.assertWnrAsrtMetric = bestRoute->rpfRouteMetric;
      mrtEntry->upstrmNbrInfo.assertWnrAsrtPref = bestRoute->rpfRouteMetricPref;

      /*************** UPSTREAM FSM EVENTS *********************/

      /* Prepare the Upstream Event Information.
       */
      upstrmFsmEventInfo.rtrIfNum = mrtEntry->upstrmRtrIfNum;

      BITX_IS_EMPTY(&mrtEntry->oifList,bIsNull);
      if(bIsNull != L7_TRUE)
      {
        /* EVENT: RPF’(S) Changes AND olist(S,G) != NULL.
         */
        pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_RPF_CHNG_OIF_NOT_NULL,
                               mrtEntry, &upstrmFsmEventInfo);
      }
      else
      {
        /* EVENT: RPF’(S) Changes AND olist(S,G) == NULL.
         */
        pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_RPF_CHNG_OIF_NULL,
                               mrtEntry, &upstrmFsmEventInfo);
      }
    }
    else if (!(L7_INET_IS_ADDR_EQUAL(&mrtEntry->rpfInfo.rpfNextHop,&bestRoute->rpfNextHop)))
    {
      L7_BOOL                 isNULL;
      pimdmUpstrmEventInfo_t  upstrmFsmEventInfo;
      L7_uchar8               src[PIMDM_MAX_DBG_ADDR_SIZE];

      PIMDM_TRACE (PIMDM_DEBUG_RTO, "Change in rpfNextHop ... "
                   "bestRoute rpfNexthop - %s, mrtEntry upstrmAddr - %s",
                   inetAddrPrint(&bestRoute->rpfNextHop,src),
                   inetAddrPrint(&bestRoute->rpfNextHop,nxt));

      if(mcastIpMapIsDirectlyConnected (&mrtEntry->srcAddr,
                                        &bestRoute->rpfIfIndex) != L7_TRUE)
      {
        /*************** UPSTREAM FSM EVENTS *********************/

        /* Prepare the Upstream Event Information.
         */
        upstrmFsmEventInfo.rtrIfNum = mrtEntry->upstrmRtrIfNum;
        BITX_IS_EMPTY(&mrtEntry->oifList,isNULL);

        if (isNULL == L7_TRUE)
        {
        /* EVENT: RPF’(S) Changes AND olist(S,G) == NULL.
         */
          pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_RPF_CHNG_OIF_NULL,mrtEntry,
                                 &upstrmFsmEventInfo);
        }
        else
        {
          /* EVENT: RPF’(S) Changes AND olist(S,G) != NULL.
           */
          pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_RPF_CHNG_OIF_NOT_NULL,
                                 mrtEntry,&upstrmFsmEventInfo);
        }
      }

      /* Update the nexthop */
      inetCopy(&mrtEntry->rpfInfo.rpfNextHop,&bestRoute->rpfNextHop);

      /* Copy the RTO best route information into upstream assert info */
      inetCopy(&mrtEntry->upstrmNbrInfo.assertWnrAddr,
               &bestRoute->rpfNextHop);
      mrtEntry->upstrmNbrInfo.assertWnrAsrtMetric = bestRoute->rpfRouteMetric;
      mrtEntry->upstrmNbrInfo.assertWnrAsrtPref = bestRoute->rpfRouteMetricPref;
    }
  }

  /*Check if the source is directly connected now for State refresh message */
  isDirectlyConnected = inetIsDirectlyConnected (&mrtEntry->srcAddr,
                                                 bestRoute->rpfIfIndex);
  if (mrtEntry->sourceDirectlyConnected != isDirectlyConnected)
  {
    if (isDirectlyConnected == L7_TRUE)
    {
      pimdmUpstrmEventInfo_t upstrmFsmEventInfo;

      PIMDM_TRACE (PIMDM_DEBUG_RTO, "Source is Directly Connected");

      /*************** UPSTREAM INTERFACE FSM EVENTS *********************/

      /* Prepare the Upstream Event Information.
       */
      upstrmFsmEventInfo.rtrIfNum = bestRoute->rpfIfIndex;

      /* EVENT - S becomes directly connected.
       */
      pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_SRC_DIR_CONN, mrtEntry,
                             &upstrmFsmEventInfo);
    }
    else
    {
      PIMDM_TRACE (PIMDM_DEBUG_RTO, "Source is Not Directly Connected");

      /*************** STATE REFRESH FSM EVENTS *********************/

      /* EVENT - Source not directly connected.
       */
      pimdmStateRfrFsmExecute (PIMDM_STATE_RFR_EVT_SRC_NOT_DIR_CONN, mrtEntry,
                               L7_NULLPTR);
    }
    mrtEntry->sourceDirectlyConnected = isDirectlyConnected;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Delete the MRT entry from the tree if  found
*
* @param    pimdmCB     @b{ (input) } Pointer PIMDM Control Block
*           mrtEntry    @b{ (input) } Pointer MRT Entry
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
static L7_RC_t
pimdmMrtEntryDelete (pimdmCB_t *pimdmCB,
                     pimdmMrtEntry_t *mrtEntry)
{
  handle_list_t *handleList = L7_NULLPTR;
  L7_RC_t retVal = L7_SUCCESS;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic Validations */
  if ((mrtEntry == L7_NULLPTR) || (pimdmCB == L7_NULLPTR))
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid input parameters");
    return L7_FAILURE;
  }

  handleList = pimdmCB->handleList;

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  /* Delete the Upstream Handle List Param */
  if (mrtEntry->mrtEntryUpstrmTmrHndlParam != L7_NULL)
  {
    handleListNodeDelete (handleList,&mrtEntry->mrtEntryUpstrmTmrHndlParam);
  }
  /* Delete the Entry Expiry Timer Handle List Param */
  if (mrtEntry->mrtEntryExpiryTimerHandle != L7_NULL)
  {
    handleListNodeDelete (handleList, &mrtEntry->mrtEntryExpiryTimerHandle);
  }

  /* Cancel the Entry expiry timer */
  if (mrtEntry->mrtEntryExpiryTimer != L7_NULLPTR)
  {
    pimdmUtilAppTimerCancel (pimdmCB, &(mrtEntry->mrtEntryExpiryTimer));
  }

  /* Reset the Entry's States */
  pimdmMrtEntryStatesReset (mrtEntry, L7_TRUE);

  /* Delete an entry to the PIM-DM MRT (S,G) AVL Tree.
   * Returns non-NULL if entry Delete is Success.
   */
  if (avlDeleteEntry (&pimdmCB->mrtSGTree, (void*) mrtEntry) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                "Entry Remove from PIM-DM MRT Tree for Src - %s, Grp - %s Failed",
                 inetAddrPrint (&(mrtEntry->srcAddr), src),
               inetAddrPrint (&(mrtEntry->grpAddr), grp));
    retVal = L7_FAILURE;
  }

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Add the MRT entry into the tree if not found
*
* @param    pimdmCB  @b{ (input) } Pointer PIMDM Control Block
*           srcAddr  @b{ (input) } Pointer Source Address
*           grpAddr  @b{ (input) } Pointer Group Address
*           rtrIfNum @b{ (input) } Router Interface Number
*
* @returns  Pointer to the MRT Entry if found else return NULL.
*
* @comments None
*
* @end
*********************************************************************/
pimdmMrtEntry_t*
pimdmMrtEntryAdd (pimdmCB_t *pimdmCB,
                  L7_inet_addr_t *srcAddr,
                  L7_inet_addr_t *grpAddr,
                  L7_uint32 rtrIfNum)
{
  pimdmMrtEntry_t *mrtEntry =L7_NULLPTR;
  pimdmMrtEntry_t tempMrtEntry;
  L7_uint32 tempRtrIfNum;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if ((srcAddr == L7_NULLPTR) || (grpAddr == L7_NULLPTR) ||
      (pimdmCB == L7_NULLPTR))
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid input parameters");
    return L7_NULLPTR;
  }

  /* Update the srcAddr and grpAddr information */
  memset (&tempMrtEntry, 0, sizeof (pimdmMrtEntry_t));
  inetCopy (&(tempMrtEntry.srcAddr), srcAddr);
  inetCopy (&(tempMrtEntry.grpAddr), grpAddr);

  /* Add the entry to the PIM-DM MRT (S,G) AVL Tree.
   * Returns NULL if entry Add is Success.
   */
  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_NULLPTR;
  }

  mrtEntry = avlInsertEntry (&pimdmCB->mrtSGTree, (void*)&tempMrtEntry);

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  if ((mrtEntry == L7_NULLPTR) || (mrtEntry != &tempMrtEntry))
  {
    mcastRPFInfo_t bestRoute;

    PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "MRT Entry Added for Src - %s, Grp - %s",
                 inetAddrPrint(srcAddr,src),inetAddrPrint(grpAddr,grp));

    mrtEntry = pimdmMrtEntryGet(pimdmCB,srcAddr,grpAddr);

    if(mrtEntry == L7_NULLPTR)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "MRT Entry Get Failed for Src - %s, Grp - %s",
                   inetAddrPrint(srcAddr,src),inetAddrPrint(grpAddr,grp));
      return L7_NULLPTR;
    }

    /* assign the values */
    mrtEntry->pimdmCB = pimdmCB;

    /* Update RPF Details & Source is directly connected */
    if(mcastRPFInfoGet(srcAddr, &bestRoute) != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "RPF Info Get Failed for Src - %s",
                   inetAddrPrint (srcAddr, src));
      pimdmMrtEntryDelete (pimdmCB, mrtEntry);
      return L7_NULLPTR;
    }

    /* Ensure PIM-DM is enabled on the RPF Interface */
    if (pimdmIntfIsEnabled (pimdmCB, bestRoute.rpfIfIndex) != L7_TRUE)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIM-DM is Not Active on RPF Intf - %d",
                   bestRoute.rpfIfIndex);
      pimdmMrtEntryDelete (pimdmCB, mrtEntry);
      return L7_NULLPTR;
    }

    /* Create the Upstream Timer Handle Param */
    if (mrtEntry->mrtEntryUpstrmTmrHndlParam == L7_NULL)
    {
      mrtEntry->mrtEntryUpstrmTmrHndlParam =
                    handleListNodeStore (pimdmCB->handleList, (void*) mrtEntry);
    }

    /* Create the Extry Expiry Timer Handle Param */
    if (mrtEntry->mrtEntryExpiryTimerHandle == L7_NULL)
    {
      mrtEntry->mrtEntryExpiryTimerHandle =
                    handleListNodeStore (pimdmCB->handleList, (void*) mrtEntry);
    }

    /* Start the MRT Entry Expiry Timer */
    osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                   "MRT Entry Expiry Timer Set Failed for Src - %s Grp - %s ",
                   inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

    osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                   "MRT Entry Expiry Timer Set Success for Src - %s Grp - %s ",
                   inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

    if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                              pimdmMrtEntryExpiryTimerHandler,
                              (void*) mrtEntry->mrtEntryExpiryTimerHandle,
                              PIMDM_MRT_ENTRY_TIMER_TIMEOUT,
                              errMsgBuf, sucMsgBuf,
                              &mrtEntry->mrtEntryExpiryTimer,
                              "DM-ET")
                           != L7_SUCCESS)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Failed to set MRT Entry expiry timer");
      pimdmMrtEntryDelete(pimdmCB,mrtEntry);
      return L7_NULLPTR;
    }

    PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "Updating RPF Info with Metric %d, Pref - %d, "
                 "on Upstrm Index is %d", bestRoute.rpfRouteMetric,
                 bestRoute.rpfRouteMetricPref, bestRoute.rpfIfIndex);

    /* Update the RPF Info */
    inetCopy(&mrtEntry->rpfInfo.rpfNextHop,&bestRoute.rpfNextHop);
    mrtEntry->rpfInfo.rpfRouteMetric = bestRoute.rpfRouteMetric;
    mrtEntry->rpfInfo.rpfRouteMetricPref = bestRoute.rpfRouteMetricPref;
    mrtEntry->rpfInfo.rpfRtrIfNum = bestRoute.rpfIfIndex;
    mrtEntry->rpfInfo.rpfRouteProto = bestRoute.rpfRouteProtocol;
    mrtEntry->upstrmRtrIfNum = bestRoute.rpfIfIndex;

    /* Update the Upstream Neighbor Info */
    inetCopy(&(mrtEntry->upstrmNbrInfo.assertWnrAddr),&bestRoute.rpfNextHop);
    mrtEntry->upstrmNbrInfo.assertWnrAsrtMetric = bestRoute.rpfRouteMetric;
    mrtEntry->upstrmNbrInfo.assertWnrAsrtPref = bestRoute.rpfRouteMetricPref;

    pimdmMrtRPFChangeProcess (mrtEntry, &bestRoute);

    /* Initialize the Upstream Interface State Info */
    mrtEntry->upstrmGraftPruneStateInfo.grfPrnState = PIMDM_UPSTRM_STATE_FORWD;

    /* Initialize the Downstream Interface State Info */
    for(tempRtrIfNum = 1; tempRtrIfNum < PIMDM_MAX_INTERFACES; tempRtrIfNum++)
    {
      if (pimdmIntfIsEnabled (pimdmCB, tempRtrIfNum) != L7_TRUE)
      {
        continue;
      }
      if (mrtEntry->downstrmStateInfo[tempRtrIfNum] != L7_NULLPTR)
      {
        continue;
      }

      if (pimdmMrtDnstrmIntfInit (mrtEntry, tempRtrIfNum) != L7_SUCCESS)
      {
        PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Dnstrm Intf-%d Creation Failed "
                     "for Src-%s, Grp-%s", tempRtrIfNum,
                     inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
        pimdmMrtEntryDelete (pimdmCB, mrtEntry);
        return L7_NULLPTR;
      }
    }
  }
  else
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "MRT Entry Insertion Failure for Src - %s, Grp - %s",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    return L7_NULLPTR;
  }

  /* Update the Create Time */
  mrtEntry->entryUpTime = osapiUpTimeRaw();

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return mrtEntry;
}

/*********************************************************************
*
* @purpose  Initialise the MRT Tree
*
* @param    pimdmCB @b{ (input) } Pointer PIMDM Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtSGTreeInit (pimdmCB_t *pimdmCB)
{
  L7_uint32 mrtTableSize = 0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic Validations */
  if (pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid Control Block");
    return L7_FAILURE;
  }

  if (pimdmCB->addrFamily == L7_AF_INET)
  {
    mrtTableSize = PIMDM_MRT_MAX_IPV4_ROUTE_ENTRIES;
  }
  else if (pimdmCB->addrFamily == L7_AF_INET6)
  {
    mrtTableSize = PIMDM_MRT_MAX_IPV6_ROUTE_ENTRIES;
  }
  else
  {
    return L7_FAILURE;
  }

  /* Allocate the MRT AVL Tree Heap */
  if ((pimdmCB->mrtTreeHeap = (avlTreeTables_t*)PIMDM_ALLOC (pimdmCB->addrFamily,
                      (mrtTableSize * sizeof (avlTreeTables_t))))
             == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIMDM MRT Tree Heap Allocation Failed");
    return L7_FAILURE;
  }

  /* Allocate the MRT AVL Data Heap */
  if ((pimdmCB->mrtDataHeap = (pimdmMrtEntry_t*) PIMDM_ALLOC (pimdmCB->addrFamily,
                      (mrtTableSize * sizeof (pimdmMrtEntry_t))))
             == L7_NULLPTR)
  {
    PIMDM_FREE (pimdmCB->addrFamily, (void*) pimdmCB->mrtTreeHeap);
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIMDM MRT Tree Data Heap Allocation Failed");
    return L7_FAILURE;
  }

  /* Create the MRT (S,G) AVL Tree */
  avlCreateAvlTree (&pimdmCB->mrtSGTree, pimdmCB->mrtTreeHeap,
                    pimdmCB->mrtDataHeap, mrtTableSize,
                    (L7_uint32) (sizeof (pimdmMrtEntry_t)),
                    PIMDM_MRT_TREE_TYPE, sizeof (L7_inet_addr_t) * 2);

  avlSetAvlTreeComparator (&pimdmCB->mrtSGTree, pimdmMrtEntryCompare);

  PIMDM_TRACE (PIMDM_DEBUG_INIT, "MRT (S,G) Tree Creation Successful");

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Deinitialise the MRT Tree
*
* @param    pimdmCB @b{ (input) } Pointer PIMDM Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtSGTreeDeInit (pimdmCB_t *pimdmCB)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic Validations */
  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid Control Block");
    return L7_FAILURE;
  }

  /* Destroy the MRT (S,G) Tree */
  if (avlDeleteAvlTree (&pimdmCB->mrtSGTree) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"MRT (S,G) Tree Destroy Failed");
    return L7_FAILURE;
  }

  PIMDM_FREE (pimdmCB->addrFamily, (void*) pimdmCB->mrtTreeHeap);
  pimdmCB->mrtTreeHeap = L7_NULLPTR;

  PIMDM_FREE (pimdmCB->addrFamily, (void*) pimdmCB->mrtDataHeap);
  pimdmCB->mrtDataHeap = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_INIT, "MRT (S,G) Tree Destroy Successful");

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Search the MRT tree to get the  entry
*
* @param    pimdmCB @b{ (input) } Pointer PIMDM Control Block
*           srcAddr @b{ (input) } Pointer Source Address
*           grpAddr @b{ (input) } Pointer Group Address
*
* @returns  Pointer to the MRT Entry if found else return NULL.
*
* @comments None
*
* @end
*********************************************************************/
pimdmMrtEntry_t*
pimdmMrtEntryGet (pimdmCB_t *pimdmCB,
                  L7_inet_addr_t *srcAddr,
                  L7_inet_addr_t *grpAddr)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  pimdmMrtEntry_t tempMrtEntry;

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");*/

  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid Control Block");
    return mrtEntry;
  }

  /* Update the srcAddr and grpAddr information */
  memset (&tempMrtEntry, 0, sizeof (pimdmMrtEntry_t));
  inetAddressZeroSet(pimdmCB->addrFamily,&(tempMrtEntry.grpAddr));
  inetAddressZeroSet(pimdmCB->addrFamily,&(tempMrtEntry.srcAddr));

  if (srcAddr != L7_NULLPTR)
  {
    inetCopy (&(tempMrtEntry.srcAddr), srcAddr);
  }
  if (grpAddr != L7_NULLPTR)
  {
    inetCopy (&(tempMrtEntry.grpAddr), grpAddr);
  }

  /* Lookup for the matching srcAddr and grpAddr entry in the Tree */
  mrtEntry = (pimdmMrtEntry_t*)
              avlSearchLVL7 (&pimdmCB->mrtSGTree, (void*) (&tempMrtEntry),
              AVL_EXACT);

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");*/
  return mrtEntry;
}

/*********************************************************************
*
* @purpose  Search the MRT tree to get next  entry
*
* @param    pimdmCB @b{ (input) } Pointer PIMDM Control Block
*           srcAddr @b{ (input) } Pointer Source Address
*           grpAddr @b{ (input) } Pointer Group Address
*
* @returns  Pointer to the MRT Entry if found else return NULL.
*
* @comments None
*
* @end
*********************************************************************/
pimdmMrtEntry_t*
pimdmMrtEntryNextGet (pimdmCB_t *pimdmCB,
                      L7_inet_addr_t *srcAddr,
                      L7_inet_addr_t *grpAddr)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  pimdmMrtEntry_t tempMrtEntry;

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");*/

  if (pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid Control Block");
    return mrtEntry;
  }
  /* Update the srcAddr and grpAddr information */
  memset (&tempMrtEntry, 0, sizeof (pimdmMrtEntry_t));
  inetAddressZeroSet(pimdmCB->addrFamily, &tempMrtEntry.srcAddr);
  inetAddressZeroSet(pimdmCB->addrFamily, &tempMrtEntry.grpAddr);

  if (srcAddr != L7_NULLPTR)
  {
    inetCopy (&(tempMrtEntry.srcAddr), srcAddr);
  }

  if (grpAddr != L7_NULLPTR)
  {
    inetCopy (&(tempMrtEntry.grpAddr), grpAddr);
  }

  /* Lookup for the matching srcAddr and grpAddr entry in the Tree */
  mrtEntry = (pimdmMrtEntry_t*)
              avlSearchLVL7 (&pimdmCB->mrtSGTree, (void*) (&tempMrtEntry),
              AVL_NEXT);

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");*/
  return mrtEntry;
}

/*********************************************************************
*
* @purpose  Delete the MRT entry from the tree if  found and also removes
*           it from the MFC.
*
* @param    pimdmCB     @b{ (input) } Pointer PIMDM Control Block
*           mrtEntry    @b{ (input) } Pointer MRT Entry
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtEntryRemove (pimdmCB_t *pimdmCB,
                     pimdmMrtEntry_t *mrtEntry)
{
  L7_RC_t retVal = L7_FAILURE;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB == L7_NULLPTR) || (mrtEntry ==L7_NULLPTR))
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid input parameters");
    return retVal;
  }

  pimdmMrtMfcEntryDelete (pimdmCB,&mrtEntry->srcAddr,
                           &mrtEntry->grpAddr,mrtEntry->upstrmRtrIfNum);

  retVal = pimdmMrtEntryDelete(pimdmCB,mrtEntry);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Compute the Outgoing Interface List and update the MFC
*
* @param    pimdmCB @b{ (input) } Pointer PIMDM Control Block
*           srcAddr @b{ (input) } Pointer Source Address
*           grpAddr @b{ (input) } Pointer Group Address
*           type    @b{ (input) } Type of MRT Change that lead to OIF
*                                 List compute.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments The logic for this calculation is mentioned in RFC 3973 in
*           section 4.1.3
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtOifListCompute (pimdmCB_t *pimdmCB,
                       L7_inet_addr_t *srcAddr,
                       L7_inet_addr_t *grpAddr,
                       PIMDM_MRT_CHANGE_TYPE_t type)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_RC_t retVal = L7_SUCCESS;
  L7_BOOL change = L7_FALSE;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");*/

  /* Validations */
  if (pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid input parameters");
    return L7_FAILURE;
  }

  if ((srcAddr != L7_NULLPTR) && (grpAddr != L7_NULLPTR))
  {
    /* Apply for a specific entry */
    if ((mrtEntry = pimdmMrtEntryGet (pimdmCB,srcAddr,grpAddr)) == L7_NULLPTR)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "MRT Find Failed for Src - %s, Grp - %s\n",
                   inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
      return L7_FAILURE;
    }

    if(type == PIMDM_MRT_CHANGE_MAX_STATES)
    {
      /* Update the (S,G) with MGMD, AdminScope Changes */
      pimdmMgmdSGInclCompute (pimdmCB, srcAddr, grpAddr,
                              &mrtEntry->pimIncludeSG);
      pimdmMgmdSGExclCompute (pimdmCB, srcAddr, grpAddr,
                              &mrtEntry->pimExcludeSG);
      pimdmMgmdStarGInclCompute (pimdmCB, grpAddr,
                                 &mrtEntry->pimIncludeStarG);
      pimdmAdminScopeIntfBitSetGet (pimdmCB,grpAddr,mrtEntry->upstrmRtrIfNum,
                                    &mrtEntry->boundaryGMask);
      change = L7_TRUE;
    }

    if(type == PIMDM_MRT_CHANGE_LOCAL_RCV)
    {
      /* Update the (S,G) with MGMD Changes */
      change |= pimdmMgmdSGInclCompute (pimdmCB, srcAddr, grpAddr,
                                        &mrtEntry->pimIncludeSG);
      change |= pimdmMgmdSGExclCompute (pimdmCB, srcAddr, grpAddr,
                                        &mrtEntry->pimExcludeSG);
    }

    if(change == L7_TRUE)
    {
      retVal = pimdmMrtOifListComputeAndApply (pimdmCB,mrtEntry);
    }
  }
  else if ((srcAddr == L7_NULLPTR) && (grpAddr != L7_NULLPTR))
  {
    /* Browse through Tree and apply for entires for this grpAddr */
    mrtEntry = pimdmMrtEntryNextGet(pimdmCB,L7_NULLPTR,L7_NULLPTR);
    while (mrtEntry != L7_NULLPTR)
    {
      if (L7_INET_ADDR_COMPARE(grpAddr,&mrtEntry->grpAddr) == 0)
      {
        if(type == PIMDM_MRT_CHANGE_LOCAL_RCV)
        {
          /* update the *,G MGMD list */
          change |= pimdmMgmdStarGInclCompute (pimdmCB, grpAddr,
                                               &mrtEntry->pimIncludeStarG);
        }
        if(change == L7_TRUE)
        {
          if ((retVal = pimdmMrtOifListComputeAndApply (pimdmCB,mrtEntry))
                     != L7_SUCCESS)
          {
            break;
          }
        }
      }

      mrtEntry = pimdmMrtEntryNextGet (pimdmCB, &mrtEntry->srcAddr,
                                       &mrtEntry->grpAddr);
    }
  }
  else
  {
    /* Browse through Tree and apply for all entries */
    mrtEntry = pimdmMrtEntryNextGet(pimdmCB,L7_NULLPTR,L7_NULLPTR);
    while (mrtEntry != L7_NULLPTR)
    {
      if(type == PIMDM_MRT_CHANGE_NBR_LIST)
      {
        if ((retVal = pimdmMrtOifListComputeAndApply (pimdmCB,mrtEntry))
                   != L7_SUCCESS)
        {
          break;
        }
      }
      mrtEntry = pimdmMrtEntryNextGet (pimdmCB, &mrtEntry->srcAddr,
                                       &mrtEntry->grpAddr);
    }
  }

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");*/
  return retVal;
}

/*********************************************************************
*
* @purpose  Perform the Interface Down notification
*
* @param    pimdmCB    @b{ (input) } Pointer PIMDM Control Block
*           rtrIfNum   @b{ (input) } Index of the Interface that has
*                                    gone down
*           forceFree  @b{ (input) } Free the Dnstrm Intf Memory Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
static L7_RC_t
pimdmMrtEntryStatesReset (pimdmMrtEntry_t *mrtEntry,
                          L7_BOOL forceFree)
{
  pimdmCB_t *pimdmCB = mrtEntry->pimdmCB;
  L7_uint32 rtrIfNum = 0;
  pimdmGraftPruneState_t *grfPrnState = L7_NULLPTR;
  pimdmOrigState_t *stRfrState = L7_NULLPTR;
  pimdmAssertInfo_t *upstrmNbrInfo = L7_NULLPTR;
  pimdmRPFInfo_t *rpfInfo = L7_NULLPTR;
  L7_uint32 upstrmRtrIfNum = 0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  upstrmRtrIfNum = mrtEntry->upstrmRtrIfNum;

  /* Reset the Downstream Interface States */
  for (rtrIfNum = 1; rtrIfNum < PIMDM_MAX_INTERFACES; rtrIfNum++)
  {
    /* Skip the upstream interface */
    if (mrtEntry->upstrmRtrIfNum == rtrIfNum)
    {
      continue;
    }

    pimdmMrtDnstrmIntfDeInit (mrtEntry, rtrIfNum, forceFree);
  }

  /* Reset the Upstream Interface Information */
  mrtEntry->upstrmRtrIfNum = 0;
  mrtEntry->sourceDirectlyConnected = L7_FALSE;

  /* Reset the RPF Information */
  rpfInfo = &mrtEntry->rpfInfo;
  rpfInfo->rpfRtrIfNum = 0;
  rpfInfo->rpfRouteMetric = 0;
  rpfInfo->rpfRouteMetricPref = ROUTE_PREF_RESERVED;
  rpfInfo->rpfMaskLen = 0;
  rpfInfo->rpfRouteProto = RTO_RESERVED;
  inetAddressZeroSet (pimdmCB->addrFamily, &rpfInfo->rpfNextHop);

  /* Reset the Upstream Neighbor Information */
  upstrmNbrInfo = &mrtEntry->upstrmNbrInfo;
  upstrmNbrInfo->assertWnrAsrtMetric = PIMDM_ASSERT_METRIC_INFINITY;
  upstrmNbrInfo->assertWnrAsrtPref = PIMDM_ASSERT_METRIC_INFINITY;
  inetAddressZeroSet (pimdmCB->addrFamily, &upstrmNbrInfo->assertWnrAddr);
  upstrmNbrInfo->amIAssertWinner = L7_FALSE;
  upstrmNbrInfo->couldAssert = L7_FALSE;

  if (forceFree == L7_TRUE)
  {
    pimdmMrtDnstrmIntfDeInit (mrtEntry, upstrmRtrIfNum, forceFree);
  }

  /* Reset the Upstream Interface Graft/Prune State */
  grfPrnState = &mrtEntry->upstrmGraftPruneStateInfo;
  grfPrnState->grfPrnState = PIMDM_UPSTRM_STATE_FORWD;
  if (grfPrnState->grftRetryTimer != L7_NULLPTR)
  {
    pimdmUtilAppTimerCancel (pimdmCB, &(grfPrnState->grftRetryTimer));
  }
  if (grfPrnState->overrideTimer != L7_NULLPTR)
  {
    pimdmUtilAppTimerCancel (pimdmCB, &(grfPrnState->overrideTimer));
  }
  if (grfPrnState->prnLmtTimer != L7_NULLPTR)
  {
    pimdmUtilAppTimerCancel (pimdmCB, &(grfPrnState->prnLmtTimer));
  }

  /* Reset the State Refresh State */
  stRfrState = &mrtEntry->origStateInfo;
  stRfrState->origState = PIMDM_STATE_RFR_STATE_NO_ORIG;
  if (stRfrState->stateRfrTimer != L7_NULLPTR)
  {
    pimdmUtilAppTimerCancel (pimdmCB, &(stRfrState->stateRfrTimer));
  }
  if (stRfrState->srcActiveTimer != L7_NULLPTR)
  {
    pimdmUtilAppTimerCancel (pimdmCB, &(stRfrState->srcActiveTimer));
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Perform the Interface Down notification
*
* @param    pimdmCB    @b{ (input) } Pointer PIMDM Control Block
*           rtrIfNum   @b{ (input) } Index of the Interface that has
*                                    gone down
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtIntfDownUpdate (pimdmCB_t *pimdmCB,
                        L7_uint32  rtrIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* clean up all the entries of the interface */
  mrtEntry = pimdmMrtEntryNextGet (pimdmCB, L7_NULLPTR, L7_NULLPTR);

  while (mrtEntry != L7_NULLPTR)
  {
    /* Check if it is the Upstream interface */
    if(mrtEntry->upstrmRtrIfNum == rtrIfNum)
    {
      PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "Upstream Iface %d is DOWN for (%s, %s)",
                   rtrIfNum, inetAddrPrint(&mrtEntry->srcAddr, src),
                   inetAddrPrint(&mrtEntry->grpAddr, grp));

      pimdmMrtUpstrmIntfDownProcess (mrtEntry, rtrIfNum);
    }
    else
    {
      PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "Downstream Iface %d is DOWN for (%s, %s)",
                   rtrIfNum, inetAddrPrint(&mrtEntry->srcAddr, src),
                   inetAddrPrint(&mrtEntry->grpAddr, grp));

      pimdmMrtDownstrmIntfDownProcess (mrtEntry, rtrIfNum);
    }

    mrtEntry = pimdmMrtEntryNextGet (pimdmCB, &mrtEntry->srcAddr,
                                     &mrtEntry->grpAddr);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return rc;
}

/*********************************************************************
*
* @purpose  Perform the Interface Down notification
*
* @param    pimdmCB    @b{ (input) } Pointer PIMDM Control Block
*           rtrIfNum   @b{ (input) } Index of the Interface that has
*                                    gone down
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtAdminDownAction (pimdmCB_t *pimdmCB)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  pimdmMrtEntry_t *tempMrtEntry = L7_NULLPTR;
  mfcEntry_t mfcEntry;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* When an Upstream interface is Down, We retain the entries with default
   * RPF Information.  So, During Global Admin Down, search for all such
   * entries and delete them.
   */
  mrtEntry = pimdmMrtEntryNextGet (pimdmCB, L7_NULLPTR, L7_NULLPTR);

  while (mrtEntry != L7_NULLPTR)
  {
    if (mrtEntry->upstrmRtrIfNum == 0)
    {
      tempMrtEntry = mrtEntry;
      mrtEntry = pimdmMrtEntryNextGet (pimdmCB, &tempMrtEntry->srcAddr,
                                       &tempMrtEntry->grpAddr);
      memset (&mfcEntry,0,sizeof(mfcEntry_t));
      inetCopy (&mfcEntry.source,&tempMrtEntry->srcAddr);
      inetCopy (&mfcEntry.group,&tempMrtEntry->grpAddr);
      if (mfcIsEntryInUse (&mfcEntry) == L7_TRUE)
      {
        /* Entry is Active in MFC.  Delete the entry from MFC database */
        pimdmMrtMfcEntryDelete (pimdmCB, &tempMrtEntry->srcAddr,
                                &tempMrtEntry->grpAddr, mfcEntry.iif);
      }

      /* Delete the entry both from PIMDM database */
      pimdmMrtEntryDelete (pimdmCB, tempMrtEntry);
    }
    else
    {
      mrtEntry = pimdmMrtEntryNextGet (pimdmCB, &mrtEntry->srcAddr,
                                       &mrtEntry->grpAddr);
    }
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose   Send a PIM-DM Control Message from the MRT Sub-component
*
* @param    mrtEntry   - @b{ (input) } Pointer MRT Entry
*           type       - @b{ (input) } Type of the packet to be send
*           pimdmCB    - @b{ (input) } Pointer to the MRT (S,G) Entry
*           pimPktType - @b{ (input) } Type of the PIM-DM Packet to be sent
*           upstrmNbrAddr - @b{ (input) } Upstream Neighbor Address
*                                         This argument contains the Originator
*                                         Address in case of a State Refresh
*                                         Message
*           rtrIfNum   - @b{ (input) } Index of the Router Interface on which to
*                                      send the Packet.
*                                      This argument contains the RPF'(S) interface
*                                      index in case of State Refresh Msg Originator
*                                      and received Interface Index in case of State
*                                      Refresh Msg Forwarder.
*           holdTime   - @b{ (input) } Holdtime.
*                                      This argument contains the State Refresh
*                                      Interval in case of State Refresh Msg.
*           stRfrTTL   - @b{ (input) } TTL of a State Refresh Msg
*           msgForwdFlag - @b{ (input) } State Refresh Message Forward Type
*           upstrmPruneIndFlag - @b{ (input) } Prune indicator Flag
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments This routine should be invoked when a Control Packet needs to
*           be sent fromt the MRT Sub-component.
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtEntryControlPacketSend (pimdmMrtEntry_t *mrtEntry,
                                PIMDM_CTRL_PKT_TYPE_t pimPktType,
                                L7_inet_addr_t *upstrmNbrAddr,
                                L7_inet_addr_t *destAddr,
                                L7_uint32 rtrIfNum,
                                L7_uint32 holdTime,
                                L7_uint32 stRfrTTL,
                                PIMDM_STRFR_MSG_FORWD_TYPE_t msgForwdFlag,
                                L7_BOOL upstrmPruneIndFlag)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  L7_RC_t retVal = L7_SUCCESS;
  L7_inet_addr_t *srcAddr = L7_NULLPTR;
  L7_inet_addr_t *grpAddr = L7_NULLPTR;
  L7_uint32 ifIndex = 0;
  L7_uint32 stRfrInterval = 0;
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic Validations */
  if ((mrtEntry == L7_NULLPTR) || (pimPktType >= PIMDM_CTRL_PKT_MAX) ||
      (destAddr == L7_NULLPTR) || (rtrIfNum < 1) ||
      (rtrIfNum >= PIMDM_MAX_INTERFACES))
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid input parameters");
    return L7_FAILURE;
  }

  pimdmCB = mrtEntry->pimdmCB;
  srcAddr = &(mrtEntry->srcAddr);
  grpAddr = &(mrtEntry->grpAddr);

  if ((pimPktType != PIMDM_CTRL_PKT_ASSERT) &&
      (pimPktType != PIMDM_CTRL_PKT_STATE_REFRESH))
  {
    if(upstrmNbrAddr == L7_NULLPTR)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Upstream Neighbor Address is NULL");
      return L7_FAILURE;
    }
  }

  switch (pimPktType)
  {
    case PIMDM_CTRL_PKT_ASSERT:
      retVal = pimdmAssertMsgSend (pimdmCB, &(pimdmCB->allRoutersAddr),
                                   rtrIfNum, srcAddr, grpAddr,
                                   mrtEntry->rpfInfo.rpfRouteMetricPref,
                                   mrtEntry->rpfInfo.rpfRouteMetric);
      break;

    case PIMDM_CTRL_PKT_JOIN:
      retVal = pimdmBundleJPGMsgSend (pimdmCB,&(pimdmCB->allRoutersAddr),
                                           rtrIfNum, srcAddr, grpAddr,
                                           upstrmNbrAddr, holdTime,
                                           PIMDM_CTRL_PKT_JOIN);
      break;

    case PIMDM_CTRL_PKT_PRUNE:
      retVal = pimdmBundleJPGMsgSend (pimdmCB,&(pimdmCB->allRoutersAddr),
                                           rtrIfNum, srcAddr, grpAddr,
                                           upstrmNbrAddr, holdTime,
                                           PIMDM_CTRL_PKT_PRUNE);
      break;

    case PIMDM_CTRL_PKT_GRAFT:
      retVal = pimdmBundleJPGMsgSend (pimdmCB, destAddr, rtrIfNum,
                                           srcAddr, grpAddr, upstrmNbrAddr,
                                           holdTime, PIMDM_CTRL_PKT_GRAFT);
      break;

    case PIMDM_CTRL_PKT_GRAFT_ACK:
      retVal = pimdmBundleJPGMsgSend (pimdmCB, destAddr, rtrIfNum,
                                      srcAddr, grpAddr,
                                      upstrmNbrAddr, holdTime,
                                      PIMDM_CTRL_PKT_GRAFT_ACK);
      break;

    case PIMDM_CTRL_PKT_STATE_REFRESH:

      /* Loop through all the Downstream Interfaces and Send the Message.
       */
      for (ifIndex = 1; ifIndex < PIMDM_MAX_INTERFACES; ifIndex++)
      {
        pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
        L7_BOOL pruneIndFlag = L7_FALSE;
        L7_BOOL pruneNowFlag = L7_FALSE;
        L7_BOOL asrtOverrideFlag = L7_FALSE;
        L7_inet_addr_t intfIpAddr;
        L7_inet_addr_t *pIntfIpAddr = &intfIpAddr;
        L7_uint32 stRfrTTLThreshold = 0;
        pimdmAssertEventInfo_t asrtFsmEventInfo;
        pimdmStRfrMsg_t *stRfrMsg = L7_NULLPTR;

        if (pimdmIntfIsEnabled (pimdmCB, ifIndex) != L7_TRUE)
        {
          /*PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Intf - %d is not Enabled", ifIndex);*/
          continue;
        }

        /* If this interface is the Incoming/RPF interface, Skip it.
         */
        if (rtrIfNum == ifIndex) /* NOTE: For StRfr Msg, rtrIfNum represents the RPF'(S) interface */
        {
          /*PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Intf - %d is Upstream", ifIndex);*/
          continue;
        }

        /* Forward the State Refresh Message on all the Downstream
         * interfaces which has atleast one neighbor present.
         */
        if ((BITX_TEST (&pimdmCB->nbrBitmap, ifIndex)) == 0)
        {
          /* There are no neighbors on this interface */
          /*PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Intf - %d has No Neighbors", ifIndex);*/
          continue;
        }

        /* If the TTL in the SRM is Zero (OR) If the TTL is less than the TTL
         * threshold, then the interface is out of TTL, skip it.
         */
        if (pimdmUtilIntfTTLThresholdGet (pimdmCB, ifIndex, &stRfrTTLThreshold)
                                       != L7_SUCCESS)
        {
          PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                       "State Refresh TTL Threshold Get Failure "
                       "for Interface %d", rtrIfNum);
          continue;
        }

        if (stRfrTTL < stRfrTTLThreshold)
        {
          PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                       "State Refresh TTL Validation Failed for Interface %d",
                       rtrIfNum);
          continue;
        }

        /* If this interface is Scope Boundary, Skip it.
         */
        if (BITX_TEST (&mrtEntry->boundaryGMask, ifIndex) != 0)
        {
          PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Group - %s is AdminScoped on Intf - %d",
                      inetAddrPrint(grpAddr,grp), ifIndex);
          continue;
        }

        /* Let the Assert Winner do the State Refresh.
         */
        if (BITX_TEST (&mrtEntry->lostAssertMask, ifIndex) != 0)
        {
          /*PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Intf - %d is Assert Loser", ifIndex);*/
          continue;
        }

        if ((dnstrmIntfInfo = mrtEntry->downstrmStateInfo[ifIndex]) == L7_NULLPTR)
        {
          PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Dnstrm Intf - %d is NULL", ifIndex);
          continue;
        }

        /* If this Interface is Pruned, Set the Prune Indicator Bit.
         */
        if (BITX_TEST (&mrtEntry->downstrmPruneMask, ifIndex) != 0)
        {
          pruneIndFlag = L7_TRUE;

          /* If this Interface is State Refresh Capable, Set the PT (S,G) to the
           * largest active holdtime read from a Prune Message accepted on this
           * interface.
           */
          if (pimdmIntfIsStateRefreshCapable (pimdmCB, ifIndex) == L7_TRUE)
          {
            pimdmDnstrmEventInfo_t dnstrmFsmEventInfo;

            /*************** DOWNSTREAM INTERFACE FSM EVENTS ******************/

            /* Prepare the Downstream Event Information.
             */
            dnstrmFsmEventInfo.rtrIfNum = ifIndex;
            if (upstrmPruneIndFlag == L7_TRUE)
            {
              dnstrmFsmEventInfo.holdTime = 2 * holdTime;
            }
            else
            {
              dnstrmFsmEventInfo.holdTime = dnstrmIntfInfo->maxPruneHoldTime;
            }

            /* EVENT - Send State Refresh (S,G) out I.
             */
            pimdmDnstrmFsmExecute (PIMDM_DNSTRM_EVT_SEND_ST_RFR, mrtEntry,
                                   &dnstrmFsmEventInfo);
          }
        }

        /* Set the AT(S,G,I) to three times the State Refresh Interval.
         * Interval = PIMDM_DEFAULT_REFRESH_INTERVAL - in case of Originator.
         * Interval = 'interval' read from the SRM   - in case of Forwarder.
         */

        /*************** ASSERT FSM EVENTS *********************/
        /* Prepare the Assert Event Information.
         */
        asrtFsmEventInfo.rtrIfNum = ifIndex;

        if (msgForwdFlag == STRFR_MSG_ORIGINATOR)
        {
          asrtFsmEventInfo.interval = (3 * PIMDM_DEFAULT_REFRESH_INTERVAL);
        }
        else /* Forwarder */
        {
          asrtFsmEventInfo.interval = (3 * holdTime); /* NOTE: Pkt's 'interval' field is passed in holdTime argument */
        }

        /* EVENT - Send State Refresh.
         */
        pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_SEND_STATE_RFR, mrtEntry,
                               &asrtFsmEventInfo);

        /* Check the Assert Override Bit.
         */
        if ((dnstrmIntfInfo->downstrmAssertInfo.assertState)
                                                   == PIMDM_ASSERT_STATE_NOINFO)
        {
          asrtOverrideFlag = L7_TRUE;
        }

        inetAddressZeroSet (pimdmCB->addrFamily, pIntfIpAddr);
        if (msgForwdFlag == STRFR_MSG_ORIGINATOR)
        {
          if (((mrtEntry->origStateInfo.msgSentCount) % 3) == 0)
          {
            pruneNowFlag = L7_TRUE;
          }

          stRfrInterval = PIMDM_DEFAULT_REFRESH_INTERVAL;

          if (pimdmIntfIpAddressGet (pimdmCB, ifIndex, pIntfIpAddr) != L7_SUCCESS)
          {
            return L7_FAILURE;
          }
        }
        else
        {
          if (pimdmIntfStateRefreshIntervalGet (pimdmCB, ifIndex, &stRfrInterval)
                                             != L7_SUCCESS)
          {
            continue;
          }

          inetCopy (pIntfIpAddr, upstrmNbrAddr);  /* NOTE: upstrmNbrAddr represents the
                                                  * Originator Address in case of Forwarder.
                                                  */
        }

        /* RFC 3973 - Section 4.3.4.
         * Store the State Refresh Message that is being relayed on this interface.
         * This will be useful when the Gen-ID of the Downstream interface's
         * Neighbor is changed.
         */
        stRfrMsg = &dnstrmIntfInfo->stRfrMsg;
        inetCopy (&stRfrMsg->grpAddr, grpAddr);
        inetCopy (&stRfrMsg->srcAddr, srcAddr);
        inetCopy (&stRfrMsg->origAddr, pIntfIpAddr);
        stRfrMsg->metricPref = mrtEntry->rpfInfo.rpfRouteMetricPref;
        stRfrMsg->metric = mrtEntry->rpfInfo.rpfRouteMetric;
        stRfrMsg->maskLen = mrtEntry->rpfInfo.rpfMaskLen;
        stRfrMsg->stRfrTTL = stRfrTTL;
        if (pruneIndFlag == L7_TRUE)
        {
          PIMDM_BIT_SET (stRfrMsg->reserved,
                         PIMDM_STRFR_PRUNE_INDICATOR_BIT);  /* Prune Indicator Bit */
        }
        if (asrtOverrideFlag == L7_TRUE)
        {
          PIMDM_BIT_SET (stRfrMsg->reserved,
                         PIMDM_STRFR_ASRT_OVERRIDE_BIT);  /* Assert Override Bit */
        }
        if (pruneNowFlag == L7_TRUE)
        {
          PIMDM_BIT_SET (stRfrMsg->reserved,
                         PIMDM_STRFR_PRUNE_NOW_BIT);  /* Prune Now Bit */
        }
        stRfrMsg->interval = stRfrInterval;

        /* Send the State Refresh Message on this Interface.
         */
        if ((retVal = pimdmStateRefreshMsgSend (pimdmCB, srcAddr, grpAddr, pIntfIpAddr,
                                           ifIndex, stRfrTTL, stRfrInterval,
                                           mrtEntry->rpfInfo.rpfRouteMetricPref,
                                           mrtEntry->rpfInfo.rpfRouteMetric,
                                           mrtEntry->rpfInfo.rpfMaskLen,
                                           pruneIndFlag, asrtOverrideFlag,
                                           pruneNowFlag))
                                        == L7_SUCCESS)
        {
          if (msgForwdFlag == STRFR_MSG_ORIGINATOR)
          {
            mrtEntry->origStateInfo.msgSentCount++;
          }
        }
      } /* End for */

      break;

    default:
      break;
  } /* End switch */

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Evaluate the RPF Neighbor Address
*
* @param    mrtEntry @b{ (input) } Pointer MRT Entry
*
* @returns  RPF Neighbor address
*
* @comments Reference RFC 3973 section 4.1.3 - page 10
*
* @end
*********************************************************************/
L7_inet_addr_t*
pimdmMrtRpfNbrEval (pimdmMrtEntry_t *mrtEntry)
{
  L7_inet_addr_t* rpfNbrAddr = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((mrtEntry->upstrmNbrInfo.amIAssertWinner == L7_FALSE) &&
      (inetIsAddressZero (&mrtEntry->upstrmNbrInfo.assertWnrAddr) != L7_TRUE))
  {
    rpfNbrAddr = &mrtEntry->upstrmNbrInfo.assertWnrAddr;
  }
  else
  {
    rpfNbrAddr = &mrtEntry->rpfInfo.rpfNextHop;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return rpfNbrAddr;
}

/*********************************************************************
*
* @purpose  Evaluate the Downstream Prune Mask
*
* @param    mrtEntry @b{ (input) } Pointer MRT Entry
*
* @returns  None
*
* @comments This needs to be called when there is change in the prune state
*           in the downstream interface info for the MRT entry.
*
* @end
*********************************************************************/
void
pimdmMrtDnstrmPruneMaskEval (pimdmMrtEntry_t *mrtEntry)
{
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  interface_bitset_t tempMask;
  L7_uint32 rtrIfNum = 0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  BITX_RESET_ALL (&tempMask);

  for (rtrIfNum = 1; rtrIfNum < PIMDM_MAX_INTERFACES; rtrIfNum++)
  {
    if ((dnstrmIntfInfo = mrtEntry->downstrmStateInfo[rtrIfNum]) == L7_NULLPTR)
    {
      continue;
    }
    if (dnstrmIntfInfo->pruneState == PIMDM_DNSTRM_STATE_PRUNED)
    {
      BITX_SET(&tempMask,rtrIfNum);
    }
  }

  if (BITX_COMPARE (&tempMask, &mrtEntry->downstrmPruneMask) != 0)
  {
    BITX_COPY (&tempMask, &mrtEntry->downstrmPruneMask);

    /* Evaluate the oif list as the prune mask is changed */
    pimdmMrtOifListComputeAndApply (mrtEntry->pimdmCB, mrtEntry);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return;
}

/*********************************************************************
*
* @purpose  Perform the Assert Metric Comparision
*
* @param    localPrefremce   @b{ (input) } Local Preference
*           localMetric      @b{ (input) } Local Metic
*           localIp          @b{ (input) } Local IP Address
*           remotePreference @b{ (input) } remote Preference
*           remoteMetric     @b{ (input) } remote Metic
*           remoteIp         @b{ (input) } remote IP Address
*
* @returns  TRUE if the local win, otherwise FALSE
*
* @comments RFC 3973 section 4.6.1 page 30
*
* @end
*********************************************************************/
L7_BOOL
pimdmMrtAsrtMetricCompare (L7_uint32 localPreference,
                           L7_uint32 localMetric,
                           L7_inet_addr_t *localIp,
                           L7_uint32 remotePreference,
                           L7_uint32 remoteMetric,
                           L7_inet_addr_t *remoteIp)
{

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (remotePreference > localPreference)
  {
    return L7_TRUE;
  }
  if (remotePreference < localPreference)
  {
    return L7_FALSE;
  }
  if (remoteMetric > localMetric)
  {
    return L7_TRUE;
  }
  if (remoteMetric < localMetric)
  {
    return L7_FALSE;
  }
  if (L7_INET_ADDR_COMPARE(localIp,remoteIp) >= 0)
  {
    return L7_TRUE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Evaluate the Downstream Assert Mask
*
* @param    mrtEntry @b{ (input) } Pointer MRT Entry
*           rtrIfNum @b{ (input) } Interface Number
*           bFlag    @b{ (input) } Flag to indicate whether to calculate oifList
*
* @returns  None
*
* @comments This needs to be called when there is change in the Assert state
*           in the downstream interface info for the MRT entry.
*
* @end
*********************************************************************/
void
pimdmMrtDnstrmAssertMaskEval (pimdmMrtEntry_t *mrtEntry,
                              L7_uint32 rtrIfNum,
                              L7_BOOL bFlag)
{
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;
  interface_bitset_t tempMask;
  L7_uint32 tempRtrIfNum = 0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  BITX_RESET_ALL (&tempMask);

  if(rtrIfNum == 0)
  {
    for(tempRtrIfNum = 1; tempRtrIfNum < PIMDM_MAX_INTERFACES; tempRtrIfNum++)
    {
      if (mrtEntry->downstrmStateInfo[tempRtrIfNum] == L7_NULLPTR)
      {
        continue;
      }

      asrtIntfInfo = &(mrtEntry->downstrmStateInfo[tempRtrIfNum]->downstrmAssertInfo);

      if ((asrtIntfInfo->assertState == PIMDM_ASSERT_STATE_ASSERT_LOSER) ||
          (bFlag == L7_TRUE))
      {
        if(asrtIntfInfo->assertTimer != L7_NULLPTR)
        {
          BITX_SET(&tempMask, tempRtrIfNum);
        }
        else
        {
          BITX_RESET(&tempMask, tempRtrIfNum);
        }
      }
    }
  }
  else
  {
    if (mrtEntry->downstrmStateInfo[rtrIfNum] != L7_NULLPTR)
    {
      asrtIntfInfo = &(mrtEntry->downstrmStateInfo[rtrIfNum]->downstrmAssertInfo);

      BITX_COPY (&mrtEntry->lostAssertMask, &tempMask);
      if ((asrtIntfInfo->assertState == PIMDM_ASSERT_STATE_ASSERT_LOSER) ||
          (bFlag == L7_TRUE))
      {
        if (asrtIntfInfo->assertTimer != L7_NULLPTR)
        {
          BITX_SET(&tempMask,rtrIfNum);
        }
        else
        {
          BITX_RESET(&tempMask,rtrIfNum);
        }
      }
    }
  }

  if (BITX_COMPARE (&tempMask, &mrtEntry->lostAssertMask) != 0)
  {
    BITX_COPY (&tempMask, &mrtEntry->lostAssertMask);

    /* Evaluate the oif list as the assert mask is changed */
    pimdmMrtOifListComputeAndApply (mrtEntry->pimdmCB, mrtEntry);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return;
}

/*********************************************************************
*
* @purpose  Process No Cache Event
*
* @param    pimdmCB  @b{ (input) } Pointer PIMDM Control Block
*           srcAddr  @b{ (input) } Pointer to Source address
*           grpAddr  @b{ (input) } Pointer Group address
*           rtrIfNum @b{ (input) } Interface Number
*           ttl      @b{ (input) } Time to Live.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments This needs to be called when PIM-DM received a no cache event
*           from the MFC
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtMfcNoCacheEventProcess (pimdmCB_t *pimdmCB,
                                L7_inet_addr_t *srcAddr,
                                L7_inet_addr_t *grpAddr,
                                L7_uint32 rtrIfNum,
                                L7_uint32 ttl)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_uint32 tempRtrIfNum = 0;
  L7_BOOL  flag = L7_FALSE;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  pimdmUpstrmEventInfo_t upstrmFsmEventInfo;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Find the MRT entry for the S,G tuple */
  mrtEntry = pimdmMrtEntryGet(pimdmCB,srcAddr,grpAddr);

  if(mrtEntry != L7_NULLPTR)
  {
    /* update the MFC with the entry if the incoming interface is correct */
    if(mrtEntry->upstrmRtrIfNum == rtrIfNum)
    {
      if(mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer != L7_NULLPTR)
      {
        /* This case will be hit only if the mfc entry is deleted and the
         * mrt entry has a NULL oif list.
         */
        pimdmUtilAppTimerCancel (mrtEntry->pimdmCB,
                                 &(mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer));
        mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer = L7_NULLPTR;

        /*************** UPSTREAM FSM EVENTS *********************/

        /* Prepare the Upstream Event Information.
         */
        upstrmFsmEventInfo.rtrIfNum = rtrIfNum;

        /* EVENT: Data packet arrives on RPF_Interface(S) AND olist(S,G) == NULL AND
         * PLT(S,G) not running.
         */
        pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_DATA_ARVL_OIF_NULL_PLT_NOTRUN,
                               mrtEntry, &upstrmFsmEventInfo);
      }

      /* Invoke the State Refresh FSM Timers */
      pimdmMrtEntryStateRfrTimersStart (mrtEntry, ttl);

      /* Update the existing State in MFC */
      pimdmMrtMfcEntryUpdate (pimdmCB,&mrtEntry->srcAddr, &mrtEntry->grpAddr,
                              mrtEntry->upstrmRtrIfNum, &mrtEntry->oifList);
      return L7_SUCCESS;
    }
  }

  /* Do the RPF Check */
  if(mcastRPFInterfaceGet(srcAddr, &tempRtrIfNum) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Error getting the RPF interface info for Src - %s",
                  inetAddrPrint (srcAddr, src));
    return L7_FAILURE;
  }
  /* src-ip-addr is Local addr */
  if(tempRtrIfNum == 0)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "SourceIP  %s local address",
                  inetAddrPrint (srcAddr, src));
    return L7_FAILURE;
  }

#if 0
  if (avlTreeCount (&pimdmCB->mrtSGTree) >= PIMDM_MRT_MAX_ROUTE_ENTRIES)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE,L7_FLEX_PIMDM_MAP_COMPONENT_ID, "PIMDM MRT Table Max Limit - [%d] Reached; "
             "Cannot accomodate any further routes. PIMDM Multicast Route table (S,G) has reached maximum "
             "capacity and cannnot accommodate new registrations anymore.", PIMDM_MRT_MAX_ROUTE_ENTRIES);
    return L7_FAILURE;
  }
#endif

  PIMDM_TRACE (PIMDM_DEBUG_EVENTS,
               "Creating/Updating MRT (S,G) Entry for Src - %s, Grp - %s",
               inetAddrPrint (srcAddr, src), inetAddrPrint (grpAddr, grp));

  /* If not found, create the entry */
  if((mrtEntry = pimdmMrtEntryAdd(pimdmCB,srcAddr,grpAddr,tempRtrIfNum))== L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Unable to create the MRT entry for Src - %s, Grp - %s",
                 inetAddrPrint (srcAddr, src), inetAddrPrint (grpAddr, grp));
    return L7_FAILURE;
  }

  /* calcualte the OIF list */
  if (pimdmMrtOifListCompute (pimdmCB, srcAddr, grpAddr,
                              PIMDM_MRT_CHANGE_MAX_STATES)
                           != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Failed to Compute OIF List for Src - %s, Grp - %s",
                 inetAddrPrint (&(mrtEntry->srcAddr), src),
                 inetAddrPrint (&(mrtEntry->grpAddr), grp));
    return L7_FAILURE;
  }

  BITX_IS_EMPTY (&mrtEntry->oifList, flag);
  if(flag == L7_TRUE)
  {
    if(mrtEntry->sourceDirectlyConnected != L7_TRUE)
    {
      /* check is PLT timer is not running */
      if(mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer == 0)
      {
        /*************** UPSTREAM FSM EVENTS *********************/

        /* Prepare the Upstream Event Information.
         */
        upstrmFsmEventInfo.rtrIfNum = tempRtrIfNum;

        /* EVENT: Data packet arrives on RPF_Interface(S) AND olist(S,G) == NULL AND
         * PLT(S,G) not running.
         */
        pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_DATA_ARVL_OIF_NULL_PLT_NOTRUN,
                               mrtEntry, &upstrmFsmEventInfo);
      }
    }
  }

  /* Invoke the State Refresh FSM Timers */
  pimdmMrtEntryStateRfrTimersStart (mrtEntry, ttl);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Process MFC's MRT Entry Expiry Event
*
* @param    pimdmCB @b{ (input) } Pointer PIMDM Control Block
*           srcAddr @b{ (input) } Pointer Source address
*           grpAddr @b{ (input) } Pointer group address
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments This needs to be called when PIM-DM received a MFC Expiry
*           event arrives from the MFC
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtMfcExpiryEventProcess (pimdmCB_t *pimdmCB,
                               L7_inet_addr_t *srcAddr,
                               L7_inet_addr_t *grpAddr)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Lookup the entry */
  if((mrtEntry = pimdmMrtEntryGet (pimdmCB,srcAddr,grpAddr)) == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
             "MRT Entry Get Failed for Src - %s, Grp - %s",
             inetAddrPrint (srcAddr, src), inetAddrPrint (grpAddr, grp));
    return L7_FAILURE;
  }

  pimdmStateRfrFsmExecute (PIMDM_STATE_RFR_EVT_SRC_NOT_DIR_CONN, mrtEntry,
                           L7_NULLPTR);

  pimdmMrtEntryDelete (pimdmCB, mrtEntry);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose   Process Wrong Interface Event
*
* @param     pimdmCB  @b{ (input) } Pointer PIMDM Control Block
*            srcAddr  @b{ (input) } Pointer Source address
*            grpAddr  @b{ (input) } Pointer group address
*            rtrIfNum @b{ (input) } Interface Number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments This needs to be called when PIM-DM received a wrong interface
*           event comes from the MFC
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtMfcWrongIfEventProcess (pimdmCB_t *pimdmCB,
                                L7_inet_addr_t *srcAddr,
                                L7_inet_addr_t *grpAddr,
                                L7_uint32 rtrIfNum)
{
  L7_RC_t retVal = L7_SUCCESS;
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  pimdmAssertEventInfo_t  asrtFsmEventInfo;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Lookup the entry */
  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB,srcAddr,grpAddr)) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "MRT Entry Get Failed for Src - %s, Grp - %s",
                 inetAddrPrint (srcAddr, src), inetAddrPrint (grpAddr, grp));
    return L7_FAILURE;
  }

  /* check if the pimdm interface is enabled on this interface */
  if (pimdmIntfIsEnabled (pimdmCB, rtrIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  if (BITX_TEST (&mrtEntry->oifList, rtrIfNum) != 0)
  {
    /*************** ASSERT FSM EVENTS *********************/

    /* Prepare the Assert Event Information.
     */
    asrtFsmEventInfo.rtrIfNum = rtrIfNum;
    asrtFsmEventInfo.interval = PIMDM_DEFAULT_ASSERT_TIMEOUT;
    asrtFsmEventInfo.type = PIMDM_CTRL_PKT_ASSERT;

    /* EVENT - Data packet arrives on Downstream interface for (S,G).
     */
    retVal = pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_DATA_ARVL_DNSTRM_IF,
                                    mrtEntry, &asrtFsmEventInfo);
  }

#if 0 /* PIMDM_TO_REMOVE_LATER */
  /* NOTE: The following piece of code is added to re-transmit the
   * Prune message by the Assert Loser if the one sent earlier is
   * missed in the network (not received by the reciever).
   * If you are Assert Loser and you get a Wrong Interface event,
   * Send a Prune message to the Assert Winner.
   */
  if (mrtEntry->downstrmStateInfo[rtrIfNum] == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Downstrm Intf - %d", rtrIfNum);
    return L7_FAILURE;
  }
  asrtIntfInfo = &(mrtEntry->downstrmStateInfo[rtrIfNum]->downstrmAssertInfo);
  if ((asrtIntfInfo->assertState == PIMDM_ASSERT_STATE_ASSERT_LOSER) &&
      (asrtIntfInfo->assertTimer != L7_NULLPTR))
  {
    if(mrtEntry->mrtEntryWrongiifExpiryTimer == L7_NULLPTR )
    {
      if (pimdmMrtEntryControlPacketSend (mrtEntry, PIMDM_CTRL_PKT_PRUNE,
                   &asrtIntfInfo->assertWnrAddr, &mrtEntry->pimdmCB->allRoutersAddr,
                   rtrIfNum, PIMDM_DEFAULT_ASSERT_TIMEOUT, 0, 0)
                != L7_SUCCESS)
      {
        PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Re-transmission of Prune Message on "
                   "rtrIfNum - %d to Assert Winner - %s Failed", rtrIfNum,
                   inetAddrPrint(&asrtIntfInfo->assertWnrAddr, src));
      }

      /*Start the MRT Wrong Iif Entry Expiry Timer */
      osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "Wrong iif MRT Entry Expiry Timer Set Failed for Src - %s Grp - %s ",
                 inetAddrPrint(&mrtEntry->srcAddr,src),
                 inetAddrPrint(&mrtEntry->grpAddr,grp));

      osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "Wrong iif MRT Entry Expiry Timer Set Success for Src - %s Grp - %s ",
                 inetAddrPrint(&mrtEntry->srcAddr,src),
                 inetAddrPrint(&mrtEntry->grpAddr,grp));

      if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                                pimdmMrtEntryWrongIifTimerHandler,
                                (void*) mrtEntry->mrtEntryWrongiifTimerHandle,
                                20, errMsgBuf, sucMsgBuf,
                                &mrtEntry->mrtEntryWrongiifExpiryTimer)
                             != L7_SUCCESS)

      {
        PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to set Wrong iif MRT Entry expiry timer");
        return L7_FAILURE;
      }

      PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "Re-transmitted Prune Message on "
                 "rtrIfNum - %d to Assert Winner - %s", rtrIfNum,
                 inetAddrPrint (&asrtIntfInfo->assertWnrAddr, src));
    }

    mrtEntry->mrtWrongIifPruneCnt++;

  }
#endif /* PIMDM_TO_REMOVE_LATER */

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/******************************************************************************
* @purpose  RTO best-route change event handler
*
* @param    pimdmCB   @b{ (input) } Pointer PIMDM Control Block.
*           routeInfo @b{ (input) } Pointer to the Route Info.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
******************************************************************************/
L7_RC_t
pimdmMrtRTOBestRouteChangeProcess (pimdmCB_t *pimdmCB,
                                   mcastRPFInfo_t *routeInfo)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_inet_addr_t srcAddr;
  L7_inet_addr_t grpAddr;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 nxt[PIMDM_MAX_DBG_ADDR_SIZE];
  mcastRPFInfo_t newBestRoute;
  L7_BOOL isDefaultRoute = L7_FALSE;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB == L7_NULLPTR) || (routeInfo == L7_NULLPTR))
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid Input Parameters");
    return L7_FAILURE;
  }

  if ((inetIsAddressZero (&routeInfo->rpfRouteAddress) == L7_TRUE) &&
      (routeInfo->prefixLength == 0))
  {
    isDefaultRoute = L7_TRUE;
  }

  /* Loop for all the sources that match */
  mrtEntry = pimdmMrtEntryNextGet(pimdmCB,L7_NULLPTR,L7_NULLPTR);
  while (mrtEntry != L7_NULLPTR)
  {
    inetCopy (&srcAddr, &mrtEntry->srcAddr);
    inetCopy (&grpAddr, &mrtEntry->grpAddr);

    if (inetAddrCompareAddrWithMask(&mrtEntry->srcAddr,
                                    routeInfo->prefixLength,
                                    &routeInfo->rpfRouteAddress,
                                    routeInfo->prefixLength)
                                 == 0)
    {
      /* Check if the Notification is for a Default Route.
       * If so, Get the Next Best RPF info.
       */
      if (isDefaultRoute == L7_TRUE)
      {
        if(mcastRPFInfoGet(&mrtEntry->srcAddr, &newBestRoute) == L7_SUCCESS)
        {
          routeInfo = &newBestRoute;

          PIMDM_TRACE (PIMDM_DEBUG_RTO, "Upon Default Route Update, New RPF Change Event [%d] "
                       "received for rpfAddr - %s with rpfNexthop - %s, "
                       "rpfIfIndex - %d, rpfMetric - %d, rpfPref - %d",
                       routeInfo->status, inetAddrPrint (&routeInfo->rpfRouteAddress, src),
                       inetAddrPrint (&routeInfo->rpfNextHop, nxt),
                       routeInfo->rpfIfIndex,
                       routeInfo->rpfRouteMetric, routeInfo->rpfRouteMetricPref);
        }
      }

      pimdmMrtRPFChangeProcess (mrtEntry,routeInfo);

    }
    mrtEntry = pimdmMrtEntryNextGet (pimdmCB, &srcAddr, &grpAddr);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Update the Largest active Holdtime read from a Prune
*           message received on this interface
*
* @param    mrtEntry    @b{ (input) } Pointer to the MRT (S,G) Entry
*           rtrIfNum    @b{ (input) } Router Interface Index
*           holdTime    @b{ (input) } Holdtime
*
* @returns  L7_SUCCESS, if success.
* @returns  L7_FAILURE, if failure.
*
* @comments This needs to be called when a Prune message is received
*           on an Interface.
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtMaxPruneHoldTimeUpdate (pimdmMrtEntry_t *mrtEntry,
                                L7_uint32 rtrIfNum,
                                L7_uint32 holdTime)
{
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((dnstrmIntfInfo = mrtEntry->downstrmStateInfo[rtrIfNum]) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Downstrm Intf - %d", rtrIfNum);
    return L7_FAILURE;
  }

  if (holdTime > dnstrmIntfInfo->maxPruneHoldTime)
  {
    dnstrmIntfInfo->maxPruneHoldTime = holdTime;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Act on the MRT (S,G) database entries when NLT Expires
*
* @param    pimdmCB  @b{ (input) } Pointer to the PIM-DM Control Block
*           nbrAddr  @b{ (input) } Pointer to the Neighbor Address
*           rtrIfNum @b{ (input) } Index of the Router Interface
*           nbrCnt   @b{ (input) } Neighbor Count on the input interface
*
* @returns  L7_SUCCESS, if success.
* @returns  L7_FAILURE, if failure.
*
* @comments This needs to be called when a PIM-DM Neighbor's Liveness
*           Timer (NLT) Expires.
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtNLTExpiryProcess (pimdmCB_t *pimdmCB,
                          L7_inet_addr_t *nbrAddr,
                          L7_uint32 rtrIfNum,
                          L7_uint32 nbrCnt)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR,*mrtNextEntry = L7_NULLPTR;
  L7_inet_addr_t *rpfDashAddr = L7_NULLPTR;
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;
  pimdmAssertEventInfo_t asrtFsmEventInfo;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic Validations.
   */
  if ((pimdmCB == L7_NULLPTR) || (nbrAddr == L7_NULLPTR) ||
      (rtrIfNum >= PIMDM_MAX_INTERFACES))
  {
    return L7_FAILURE;
  }

  /* Scan through the MRT (S,G) database.
   * Get all the (S,G) Entries for which this neighbor is the Assert Winner.
   * Cancel the AT for that (S,G,I) Entry.
   */
  mrtEntry = pimdmMrtEntryNextGet (pimdmCB, L7_NULLPTR, L7_NULLPTR);
  while (mrtEntry != L7_NULLPTR)
  {
    rpfDashAddr = &(mrtEntry->upstrmNbrInfo.assertWnrAddr);

    if(mrtEntry->upstrmRtrIfNum == rtrIfNum)
    {
      if(L7_INET_ADDR_COMPARE(rpfDashAddr,nbrAddr) == 0)
      {
        if(nbrCnt == 1)
        {
          pimdmMrtUpstrmIntfDownProcess (mrtEntry,rtrIfNum);

          /*Remove (S,G) entry as upstream neighbor timed out */
          mrtNextEntry = pimdmMrtEntryNextGet (pimdmCB, &(mrtEntry->srcAddr),
                                     &(mrtEntry->grpAddr));
          mrtEntry = mrtNextEntry;
          continue;
        }
        else
        {
          /*Copy RPF(S) to RPF'(S)*/
          inetCopy(rpfDashAddr,&(mrtEntry->rpfInfo.rpfNextHop));
        }
      }
    }
    else
    {
      if ((dnstrmIntfInfo = mrtEntry->downstrmStateInfo[rtrIfNum]) == L7_NULLPTR)
      {
        return L7_FAILURE;
      }

      asrtIntfInfo = &dnstrmIntfInfo->downstrmAssertInfo;
      if (L7_INET_ADDR_COMPARE (&(asrtIntfInfo->assertWnrAddr), nbrAddr) == 0)
      {
        memset (&asrtFsmEventInfo, 0, sizeof (pimdmAssertEventInfo_t));
        inetAddressZeroSet(pimdmCB->addrFamily,&(asrtFsmEventInfo.assertAddr));
        asrtFsmEventInfo.rtrIfNum = rtrIfNum;

        /*************** ASSERT FSM EVENTS *********************/

        /* EVENT - Winner's NLT(N,I) Expires.
         */
        pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_ASRT_WNR_NLT_EXPIRY, mrtEntry,
                               &asrtFsmEventInfo);
      }
      if (nbrCnt == 1)
      {
        pimdmMrtDownstrmNeighborDownProcess (mrtEntry,rtrIfNum, nbrAddr);
      }
      /* Check for any Upstream State Transitions and re-calculate the OIF List */
      pimdmMrtOifInterfaceSet (mrtEntry, rtrIfNum, L7_FALSE);

    }

    mrtEntry = pimdmMrtEntryNextGet (pimdmCB, &(mrtEntry->srcAddr),
                                     &(mrtEntry->grpAddr));
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Process an Assert Message received on the Upstream
*           Interface
*
* @param    mrtEntry    @b{ (input) } Pointer to the MRT (S,G) Entry
*           asrtNbrAddr @b{ (input) } Pointer to the Neighbor Address
*           rtrIfNum    @b{ (input) } Index of the Router Interface
*           asrtMetricPref @b{ (input) } Assert Metric Preference
*           asrtMetric  @b{ (input) } Assert Metric
*
* @returns  L7_SUCCESS, if success.
* @returns  L7_FAILURE, if failure.
*
* @comments This needs to be called when an Assert Message is received
*           on an Upstream Interface.
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtUpstreamAssertProcess (pimdmMrtEntry_t *mrtEntry,
                               L7_inet_addr_t *asrtNbrAddr,
                               L7_uint32 rtrIfNum,
                               L7_uint32 asrtMetricPref,
                               L7_uint32 asrtMetric)
{
  pimdmAssertInfo_t *upstrmNbrInfo = L7_NULLPTR;
  pimdmUpstrmEventInfo_t upstrmFsmEventInfo;
  L7_BOOL isNull = L7_FALSE;
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = L7_NULLPTR;
  L7_inet_addr_t *grpAddr = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  upstrmNbrInfo = &(mrtEntry->upstrmNbrInfo);
  srcAddr = &(mrtEntry->srcAddr);
  grpAddr = &(mrtEntry->grpAddr);

  /* Compare the Local Assert Info with that of the received to decide if there
   * is a change in our Upstream Interface.
   */
  if (pimdmMrtAsrtMetricCompare (upstrmNbrInfo->assertWnrAsrtPref,
                                 upstrmNbrInfo->assertWnrAsrtMetric,
                                 &(upstrmNbrInfo->assertWnrAddr),
                                 asrtMetricPref, asrtMetric, asrtNbrAddr)
                              == L7_TRUE)
  {
    PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "Existing RPF'(S) is still the Winner.\n"
                 "LocalPref-%d, RemotePref-%d... LocalMetric-%d, RemoteMetric-%d, \n"
                 "LocalAddr-%s, RemoteAddr-%s", upstrmNbrInfo->assertWnrAsrtPref,
                 asrtMetricPref, upstrmNbrInfo->assertWnrAsrtMetric, asrtMetric,
                 inetAddrPrint(&(upstrmNbrInfo->assertWnrAddr),src),
                 inetAddrPrint(asrtNbrAddr,grp));
    return L7_SUCCESS;  /* Existing RPF'(S) is still the Winner */
  }

  /* Assert sender is the Winner.  Change our Upstream to RPF'(S).
   */
  inetCopy (&(upstrmNbrInfo->assertWnrAddr), asrtNbrAddr);
  upstrmNbrInfo->assertWnrAsrtPref = asrtMetricPref;
  upstrmNbrInfo->assertWnrAsrtMetric = asrtMetric;

  /*************** UPSTREAM FSM EVENTS *********************/

  /* Prepare the Upstream Event Information.
   */
  upstrmFsmEventInfo.rtrIfNum = rtrIfNum;

  BITX_IS_EMPTY (&mrtEntry->oifList, isNull);

  if (isNull == L7_FALSE)
  {
    /* EVENT - RPF'(S) Changes AND olist(S,G) != NULL.
     */
    pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_RPF_CHNG_OIF_NOT_NULL, mrtEntry,
                           &upstrmFsmEventInfo);
  }
  else
  {
    /* EVENT - RPF'(S) Changes AND olist(S,G) == NULL.
     */
    pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_RPF_CHNG_OIF_NULL, mrtEntry,
                           &upstrmFsmEventInfo);
  }

  /* Set the Upstream Assert Timer */
  osapiSnprintf (errMsgBuf, PIMDM_MAX_DBG_ADDR_SIZE,
                 "Upstream AT (S,G) Set Failed for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp), rtrIfNum);
  osapiSnprintf (sucMsgBuf, PIMDM_MAX_DBG_ADDR_SIZE,
                 "Upstream AT (S,G,I) Set Success for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp), rtrIfNum);

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmUpstreamAssertTimerExpiryHandler,
                            (void*) mrtEntry->mrtEntryUpstrmTmrHndlParam,
                            PIMDM_DEFAULT_ASSERT_TIMEOUT, errMsgBuf, sucMsgBuf,
                            &upstrmNbrInfo->assertTimer,
                            "DM-AT")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Upstream AT (S,G) Set Failed for Src - %s Grp - %s Intf - %d ",
                inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp), rtrIfNum);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To display the specified MRT Entry info in brief
*
* @param    mrtEntry @b{ (input) } Pointer to the MRT (S,G) Entry
*
* @returns  L7_SUCCESS, if success.
* @returns  L7_FAILURE, if failure.
*
* @comments This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
static void
pimdmMrtEntryBriefShow (pimdmMrtEntry_t *mrtEntry)
{
  L7_uchar8 buf[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 index = 0;

  if (mrtEntry == L7_NULLPTR)
  {
    return;
  }

  PIMDM_DEBUG_PRINTF ("-------------------------------\n");
  PIMDM_DEBUG_PRINTF ("Source    - %s.\n", inetAddrPrint (&(mrtEntry->srcAddr), buf));
  PIMDM_DEBUG_PRINTF ("Group     - %s.\n", inetAddrPrint (&(mrtEntry->grpAddr), buf));
  PIMDM_DEBUG_PRINTF ("RPF(S)    - %s.\n", inetAddrPrint (&(mrtEntry->rpfInfo.rpfNextHop), buf));
  PIMDM_DEBUG_PRINTF ("RPF'(S)   - %s.\n", inetAddrPrint (&(mrtEntry->upstrmNbrInfo.assertWnrAddr), buf));
  PIMDM_DEBUG_PRINTF ("IIF Index - %d.\n", mrtEntry->upstrmRtrIfNum);
  PIMDM_DEBUG_PRINTF ("OIF List  - ");

  for (index = 1; index <= PIMDM_INTF_BIT_SIZE; index++)
  {
    if (BITX_TEST (&mrtEntry->oifList, index) != 0)
    {
      PIMDM_DEBUG_PRINTF (" %02d ", index);
    }
  }

  PIMDM_DEBUG_PRINTF ("\n");

  return;
}

/*********************************************************************
*
* @purpose  To display the specified MRT Entry info in detail
*
* @param    mrtEntry @b{ (input) } Pointer to the MRT (S,G) Entry
*
* @returns  L7_SUCCESS, if success.
* @returns  L7_FAILURE, if failure.
*
* @comments This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
static void
pimdmMrtEntryDetailShow (pimdmMrtEntry_t *mrtEntry)
{
  L7_uchar8 buf[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 index = 0;
  L7_uint32 timeLeft = 0;
  L7_APP_TMR_CTRL_BLK_t *appTimer = L7_NULLPTR;
  L7_timespec timeVal;
  L7_uint32 entryExpiryTime = 0;

  if (mrtEntry == L7_NULLPTR)
  {
    return;
  }

  appTimer = mrtEntry->pimdmCB->appTimer;

  PIMDM_DEBUG_PRINTF ("-------------------------------\n");
  PIMDM_DEBUG_PRINTF ("Source     - %s.\n", inetAddrPrint (&(mrtEntry->srcAddr), buf));
  PIMDM_DEBUG_PRINTF ("Group      - %s.\n", inetAddrPrint (&(mrtEntry->grpAddr), buf));
  PIMDM_DEBUG_PRINTF ("IIF Index  - %d.\n", mrtEntry->upstrmRtrIfNum);

  PIMDM_DEBUG_PRINTF ("RPF(S)     - %s.\n", inetAddrPrint (&(mrtEntry->rpfInfo.rpfNextHop), buf));
  PIMDM_DEBUG_PRINTF ("RPF'(S)    - %s.\n", inetAddrPrint (&(mrtEntry->upstrmNbrInfo.assertWnrAddr), buf));
  PIMDM_DEBUG_PRINTF ("Metric     - %d.\n", mrtEntry->rpfInfo.rpfRouteMetric);
  PIMDM_DEBUG_PRINTF ("Preference - %d.\n", mrtEntry->rpfInfo.rpfRouteMetricPref);

  PIMDM_DEBUG_PRINTF ("ExpireTime - ");
  appTimerTimeLeftGet (appTimer, mrtEntry->mrtEntryExpiryTimer, &entryExpiryTime);
  osapiConvertRawUpTime(entryExpiryTime, &timeVal);
  PIMDM_DEBUG_PRINTF ("%2d:%2d:%2d.\n", timeVal.hours, timeVal.minutes, timeVal.seconds);

  PIMDM_DEBUG_PRINTF ("OIF LISTS .....\n");
  PIMDM_DEBUG_PRINTF ("    pim_nbrs             - ");
  for (index = 1; index <= PIMDM_INTF_BIT_SIZE; index++)
  {
    if (BITX_TEST (&mrtEntry->pimdmCB->nbrBitmap, index) != 0)
    {
      PIMDM_DEBUG_PRINTF (" %02d ", index);
    }
  }
  PIMDM_DEBUG_PRINTF ("\n");
  PIMDM_DEBUG_PRINTF ("    prunes(S,G)          - ");
  for (index = 1; index <= PIMDM_INTF_BIT_SIZE; index++)
  {
    if (BITX_TEST (&mrtEntry->downstrmPruneMask, index) != 0)
    {
      PIMDM_DEBUG_PRINTF (" %02d ", index);
    }
  }
  PIMDM_DEBUG_PRINTF ("\n");
  PIMDM_DEBUG_PRINTF ("    pim_include(*,G)     - ");
  for (index = 1; index <= PIMDM_INTF_BIT_SIZE; index++)
  {
    if (BITX_TEST (&mrtEntry->pimIncludeStarG, index) != 0)
    {
      PIMDM_DEBUG_PRINTF (" %02d ", index);
    }
  }
  PIMDM_DEBUG_PRINTF ("\n");
  PIMDM_DEBUG_PRINTF ("    pim_include(S,G)     - ");
  for (index = 1; index <= PIMDM_INTF_BIT_SIZE; index++)
  {
    if (BITX_TEST (&mrtEntry->pimIncludeSG, index) != 0)
    {
      PIMDM_DEBUG_PRINTF (" %02d ", index);
    }
  }
  PIMDM_DEBUG_PRINTF ("\n");
  PIMDM_DEBUG_PRINTF ("    pim_exclude(S,G)     - ");
  for (index = 1; index <= PIMDM_INTF_BIT_SIZE; index++)
  {
    if (BITX_TEST (&mrtEntry->pimExcludeSG, index) != 0)
    {
      PIMDM_DEBUG_PRINTF (" %02d ", index);
    }
  }
  PIMDM_DEBUG_PRINTF ("\n");
  PIMDM_DEBUG_PRINTF ("    lost_assert(S,G)     - ");
  for (index = 1; index <= PIMDM_INTF_BIT_SIZE; index++)
  {
    if (BITX_TEST (&mrtEntry->lostAssertMask, index) != 0)
    {
      PIMDM_DEBUG_PRINTF (" %02d ", index);
    }
  }
  PIMDM_DEBUG_PRINTF ("\n");
  PIMDM_DEBUG_PRINTF ("    boundary(G)          - ");
  for (index = 1; index <= PIMDM_INTF_BIT_SIZE; index++)
  {
    if (BITX_TEST (&mrtEntry->boundaryGMask, index) != 0)
    {
      PIMDM_DEBUG_PRINTF (" %02d ", index);
    }
  }
  PIMDM_DEBUG_PRINTF ("\n");
  PIMDM_DEBUG_PRINTF ("    immediate_olist(S,G) - ");
  for (index = 1; index <= PIMDM_INTF_BIT_SIZE; index++)
  {
    if (BITX_TEST (&mrtEntry->immediateOifList, index) != 0)
    {
      PIMDM_DEBUG_PRINTF (" %02d ", index);
    }
  }
  PIMDM_DEBUG_PRINTF ("\n");
  PIMDM_DEBUG_PRINTF ("    olist(S,G)           - ");
  for (index = 1; index <= PIMDM_INTF_BIT_SIZE; index++)
  {
    if (BITX_TEST (&mrtEntry->oifList, index) != 0)
    {
      PIMDM_DEBUG_PRINTF (" %02d ", index);
    }
  }
  PIMDM_DEBUG_PRINTF ("\n");

  PIMDM_DEBUG_PRINTF ("\n");
#define PIMDM_DBG_PRINT_IS_TIMER_RUNNING(timerHandle) \
  ((timerHandle != L7_NULLPTR) ? ("RUNNING") : ("NOT RUNNING"))

  PIMDM_DEBUG_PRINTF ("UPSTREAM STATE INFO ...\n");
  {
    pimdmGraftPruneState_t *grfPrnState = &(mrtEntry->upstrmGraftPruneStateInfo);
    pimdmAssertInfo_t *uptrmNbrInfo = &(mrtEntry->upstrmNbrInfo);

#define PIMDM_DBG_PRINT_GRF_STATE(grfState) \
  ((grfState->grfPrnState == PIMDM_UPSTRM_STATE_FORWD) ? ("Forward") : \
   ((grfState->grfPrnState == PIMDM_UPSTRM_STATE_PRUNE) ? ("Pruned") : ("AckPending")))

    PIMDM_DEBUG_PRINTF ("   Upstream Nbr :\n");
    PIMDM_DEBUG_PRINTF ("      Neighbor Addr - %s.\n", inetAddrPrint (&(uptrmNbrInfo->assertWnrAddr), buf));
    PIMDM_DEBUG_PRINTF ("      Metric        - %d.\n", uptrmNbrInfo->assertWnrAsrtMetric);
    PIMDM_DEBUG_PRINTF ("      Metric Pref   - %d.\n", uptrmNbrInfo->assertWnrAsrtPref);

    PIMDM_DEBUG_PRINTF ("   Graft/Prune State :\n");
    PIMDM_DEBUG_PRINTF ("      State    - %s.\n", PIMDM_DBG_PRINT_GRF_STATE(grfPrnState));
    timeLeft = 0;
    appTimerTimeLeftGet (appTimer, grfPrnState->grftRetryTimer, &timeLeft);
    PIMDM_DEBUG_PRINTF ("      GRT(S,G) - %s, TimeLeft - %d sec.\n",
                        PIMDM_DBG_PRINT_IS_TIMER_RUNNING(grfPrnState->grftRetryTimer), timeLeft);
    timeLeft = 0;
    appTimerTimeLeftGet (appTimer, grfPrnState->overrideTimer, &timeLeft);
    PIMDM_DEBUG_PRINTF ("      OT(S,G)  - %s, TimeLeft - %d sec.\n",
                        PIMDM_DBG_PRINT_IS_TIMER_RUNNING(grfPrnState->overrideTimer), timeLeft);
    timeLeft = 0;
    appTimerTimeLeftGet (appTimer, grfPrnState->prnLmtTimer, &timeLeft);
    PIMDM_DEBUG_PRINTF ("      PLT(S,G) - %s, TimeLeft - %d sec.\n",
                        PIMDM_DBG_PRINT_IS_TIMER_RUNNING(grfPrnState->prnLmtTimer), timeLeft);
  }

  PIMDM_DEBUG_PRINTF ("ORIGINATOR STATE INFO ...\n");
  {
    pimdmOrigState_t *origState = &(mrtEntry->origStateInfo);

#define PIMDM_DBG_PRINT_ORG_STATE(orgState) \
  ((orgState->origState == PIMDM_STATE_RFR_STATE_NO_ORIG) ? ("Not Originator") : ("Originator"))

    PIMDM_DEBUG_PRINTF ("   Originator State :\n");
    PIMDM_DEBUG_PRINTF ("      State    - %s.\n", PIMDM_DBG_PRINT_ORG_STATE(origState));
    timeLeft = 0;
    appTimerTimeLeftGet (appTimer, origState->stateRfrTimer, &timeLeft);
    PIMDM_DEBUG_PRINTF ("      SRT(S,G) - %s, TimeLeft - %d sec.\n",
                        PIMDM_DBG_PRINT_IS_TIMER_RUNNING(origState->stateRfrTimer), timeLeft);
    timeLeft = 0;
    appTimerTimeLeftGet (appTimer, origState->srcActiveTimer, &timeLeft);
    PIMDM_DEBUG_PRINTF ("      SAT(S,G) - %s, TimeLeft - %d sec.\n",
                        PIMDM_DBG_PRINT_IS_TIMER_RUNNING(origState->srcActiveTimer), timeLeft);
  }

  PIMDM_DEBUG_PRINTF ("DOWNSTREAM STATE INFO ...\n");
  for (index = 1; index < PIMDM_MAX_INTERFACES; index++)
  {
    pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
    pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;

    if (pimdmIntfIsEnabled (mrtEntry->pimdmCB, index) != L7_TRUE)
    {
      continue;
    }
    if (index == mrtEntry->upstrmRtrIfNum)
    {
      continue;
    }

    if ((dnstrmIntfInfo = mrtEntry->downstrmStateInfo[index]) == L7_NULLPTR)
    {
      continue;
    }

#define PIMDM_DBG_PRINT_PRN_STATE(prnState) \
  ((prnState->pruneState == PIMDM_DNSTRM_STATE_NOINFO) ? ("NoInfo") : \
   ((prnState->pruneState == PIMDM_DNSTRM_STATE_PRUNE_PENDING) ? ("PrunePending") : ("Pruned")))

    PIMDM_DEBUG_PRINTF ("  Interface %d:\n", index);
    PIMDM_DEBUG_PRINTF ("     PRUNE State :\n");
    PIMDM_DEBUG_PRINTF ("        State    - %s.\n", PIMDM_DBG_PRINT_PRN_STATE(dnstrmIntfInfo));
    timeLeft = 0;
    appTimerTimeLeftGet (appTimer, dnstrmIntfInfo->pruneTimer, &timeLeft);
    PIMDM_DEBUG_PRINTF ("        PT(S,G)  - %s, TimeLeft - %d sec.\n",
                        PIMDM_DBG_PRINT_IS_TIMER_RUNNING(dnstrmIntfInfo->pruneTimer), timeLeft);
    timeLeft = 0;
    appTimerTimeLeftGet (appTimer, dnstrmIntfInfo->prunePndTimer, &timeLeft);
    PIMDM_DEBUG_PRINTF ("        PPT(S,G) - %s, TimeLeft - %d sec.\n",
                        PIMDM_DBG_PRINT_IS_TIMER_RUNNING(dnstrmIntfInfo->prunePndTimer), timeLeft);

    asrtIntfInfo = &(dnstrmIntfInfo->downstrmAssertInfo);

#define PIMDM_DBG_PRINT_AST_STATE(astState) \
  ((astState->assertState == PIMDM_ASSERT_STATE_NOINFO) ? ("NoInfo") : \
   ((astState->assertState == PIMDM_ASSERT_STATE_ASSERT_WINNER) ? ("Winner") : ("Loser")))

    PIMDM_DEBUG_PRINTF ("     ASSERT State :\n");
    PIMDM_DEBUG_PRINTF ("        State    - %s.\n", PIMDM_DBG_PRINT_AST_STATE(asrtIntfInfo));
    timeLeft = 0;
    appTimerTimeLeftGet (appTimer, asrtIntfInfo->assertTimer, &timeLeft);
    PIMDM_DEBUG_PRINTF ("        AT(S,G)  - %s, TimeLeft - %d sec.\n",
                        PIMDM_DBG_PRINT_IS_TIMER_RUNNING(asrtIntfInfo->assertTimer), timeLeft);
    PIMDM_DEBUG_PRINTF ("        Am I Assert Winner - %d.\n", asrtIntfInfo->amIAssertWinner);
    PIMDM_DEBUG_PRINTF ("        Assert Winner Addr - %s.\n", inetAddrPrint (&(asrtIntfInfo->assertWnrAddr), buf));
    PIMDM_DEBUG_PRINTF ("        Assert Metric      - %d.\n", asrtIntfInfo->assertWnrAsrtMetric);
    PIMDM_DEBUG_PRINTF ("        Assert Metric Pref - %d.\n", asrtIntfInfo->assertWnrAsrtPref);
    PIMDM_DEBUG_PRINTF ("        Could Assert Flag  - %d.\n", asrtIntfInfo->couldAssert);

#if 0
#define PIMDM_DBG_PRINT_LCL_STATE(lclState) \
  ((lclState == PIMDM_LOCAL_MEMBSHIP_STATE_NOINFO) ? ("NoInfo") : ("Include"))

    PIMDM_DEBUG_PRINTF ("     LOCAL MEMBERSHIP INFO :\n");
    PIMDM_DEBUG_PRINTF ("        State    - %s.\n", PIMDM_DBG_PRINT_LCL_STATE(dnstrmIntfInfo->localMembshipState));
#endif
  }

  return;
}

/*********************************************************************
*
* @purpose  To display the entries MRT Table in detail
*
* @param    pimdmCB  @b{ (input) } Pointer to the PIM-DM Control Block
*           inBrief  @b{ (input) } Flag to indicate Display in Detail or Brief
*
* @returns  L7_SUCCESS, if success.
* @returns  L7_FAILURE, if failure.
*
* @comments This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmMrtTableInfoShow (pimdmCB_t *pimdmCB, L7_BOOL inBrief,L7_uint32 count)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_uint32 localcount=L7_NULL;

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_DEBUG_PRINTF ("Failed to Acquire PIM-DM MRT Semaphore");
    return;
  }

  mrtEntry = pimdmMrtEntryNextGet (pimdmCB, L7_NULLPTR, L7_NULLPTR);
  while (mrtEntry != L7_NULLPTR)
  {
    if((count != L7_NULL) &&(localcount >= count))
    {
      break;
    }

    localcount++;
    if (inBrief == L7_TRUE)
    {
      pimdmMrtEntryBriefShow (mrtEntry);
    }
    else
    {
      pimdmMrtEntryDetailShow (mrtEntry);
    }

    mrtEntry = pimdmMrtEntryNextGet (pimdmCB, &(mrtEntry->srcAddr),
                                     &(mrtEntry->grpAddr));
  }

  osapiSemaGive (pimdmCB->mrtSGTree.semId);
  return;
}

/*********************************************************************
*
* @purpose  To display the entries MRT Table for a Src and Grp combination
*
* @param    pimdmCB  @b{ (input) } Pointer to the PIM-DM Control Block
*           srcAddr  @b{ (input) } Source Address
*           grpAddr  @b{ (input) } Group Address
*           inBrief  @b{ (input) } Flag to indicate Display in Detail or Brief
*
* @returns  L7_SUCCESS, if success.
* @returns  L7_FAILURE, if failure.
*
* @comments This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmMrtTableSrcGrpInfoShow (pimdmCB_t *pimdmCB,
                             L7_inet_addr_t *srcAddr,
                             L7_inet_addr_t *grpAddr,
                             L7_BOOL inBrief)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_DEBUG_PRINTF ("Failed to Acquire PIM-DM MRT Semaphore");
    return;
  }

  mrtEntry = pimdmMrtEntryNextGet (pimdmCB, L7_NULLPTR, L7_NULLPTR);
  while (mrtEntry != L7_NULLPTR)
  {
    if ((L7_INET_ADDR_COMPARE(&mrtEntry->srcAddr,srcAddr) == 0) &&
        (L7_INET_ADDR_COMPARE(&mrtEntry->grpAddr, grpAddr) == 0))
    {
      if (inBrief == L7_TRUE)
      {
        pimdmMrtEntryBriefShow (mrtEntry);
      }
      else
      {
        pimdmMrtEntryDetailShow (mrtEntry);
      }

      osapiSemaGive (pimdmCB->mrtSGTree.semId);
      return;
    }

    mrtEntry = pimdmMrtEntryNextGet (pimdmCB, &(mrtEntry->srcAddr),
                                     &(mrtEntry->grpAddr));
  }

  osapiSemaGive (pimdmCB->mrtSGTree.semId);
  return;
}

/*********************************************************************
*
* @purpose  Scans the MRT entries and computes the oifList when a neighbor
*           is added to the downstream interface index
*
* @param    pimdmCB     @b{ (input) } Pointer to the PIM-DM Control Block
*           rtrIfNum    @b{ (input) } Router Interface Number
*
* @returns  L7_SUCCESS, if success.
* @returns  L7_FAILURE, if failure.
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimdmNbrAddMrtOifListCompute(pimdmCB_t *pimdmCB,L7_uint32 rtrIfNum)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;

  if(pimdmCB == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  mrtEntry = pimdmMrtEntryNextGet (pimdmCB, L7_NULLPTR, L7_NULLPTR);
  while (mrtEntry != L7_NULLPTR)
  {
    if(BITX_TEST(&mrtEntry->downstrmPruneMask,rtrIfNum) != 0)
    {
      BITX_RESET(&mrtEntry->downstrmPruneMask,rtrIfNum);
      pimdmMrtOifListComputeAndApply(pimdmCB,mrtEntry);
    }

    mrtEntry = pimdmMrtEntryNextGet (pimdmCB, &(mrtEntry->srcAddr),
                                     &(mrtEntry->grpAddr));
  }

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Test whether  mrt entry from the (S,G) entry table can be deleted
*
* @param    pimdmCb    @b{(input)} PIM DM Control Block
*
* @returns
* @returns
*
* @comments
*
* @end
*********************************************************************/
static L7_BOOL pimdmMrtEntryCanRemove(pimdmMrtEntry_t *mrtEntry)
{

  L7_uint32 index;
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;
  pimdmGraftPruneState_t *grfPrnState = &(mrtEntry->upstrmGraftPruneStateInfo);
  pimdmAssertInfo_t *uptrmNbrInfo = &(mrtEntry->upstrmNbrInfo);
  pimdmOrigState_t *origState = &(mrtEntry->origStateInfo);
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  for(index = 1; index < PIMDM_MAX_INTERFACES; index++)
  {
     if ((dnstrmIntfInfo = mrtEntry->downstrmStateInfo[index]) == L7_NULLPTR)
     {
          continue;
     }

     if((dnstrmIntfInfo->pruneState != PIMDM_DNSTRM_STATE_NOINFO) ||
        (dnstrmIntfInfo->prunePndTimer != L7_NULLPTR) ||
        (dnstrmIntfInfo->pruneTimer != L7_NULLPTR))
     {
       return L7_FALSE;
     }
     asrtIntfInfo = &dnstrmIntfInfo->downstrmAssertInfo;
     if((asrtIntfInfo->assertState !=
           PIMDM_ASSERT_STATE_NOINFO) ||
        (asrtIntfInfo->assertTimer != L7_NULLPTR))
     {
       return L7_FALSE;
     }
  }

  if((grfPrnState->grfPrnState !=
        PIMDM_UPSTRM_STATE_FORWD) ||
     (grfPrnState->grftRetryTimer != L7_NULLPTR)||
     (grfPrnState->overrideTimer != L7_NULLPTR)||
     (grfPrnState->prnLmtTimer != L7_NULLPTR)||
     (uptrmNbrInfo->assertState !=
        PIMDM_ASSERT_STATE_NOINFO) ||
     (uptrmNbrInfo->assertTimer != L7_NULLPTR)||
     (origState->origState !=
        PIMDM_STATE_RFR_STATE_NO_ORIG) ||
     (origState->srcActiveTimer != L7_NULLPTR)||
     (origState->stateRfrTimer != L7_NULLPTR))
  {
    return L7_FALSE;
  }
  PIMDM_TRACE(PIMDM_DEBUG_API,
                 "pimdmMrtEntryCanRemove()In-Active entry: Src - %s Grp - %s ",
                 inetAddrPrint(&mrtEntry->srcAddr,src),
                 inetAddrPrint(&mrtEntry->grpAddr,grp));

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Try to Delete a mrt entry from the (S,G) entry table
*
* @param    pimdmCb    @b{(input)} PIM DM Control Block

*
* @returns
* @returns
*
* @comments
*
* @end
*********************************************************************/
static L7_BOOL pimdmMrtEntryTryRemove(pimdmMrtEntry_t *mrtEntry)
{
  L7_BOOL retVal;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  retVal = pimdmMrtEntryCanRemove(mrtEntry);
  if(retVal == L7_TRUE)
  {
    PIMDM_TRACE(PIMDM_DEBUG_API,
                 "pimdmMrtEntryTryRemove(): Src - %s Grp - %s ",
                 inetAddrPrint(&mrtEntry->srcAddr,src),
                 inetAddrPrint(&mrtEntry->grpAddr,grp));
    pimdmMrtEntryRemove(mrtEntry->pimdmCB, mrtEntry);
  }
  return retVal;
}

/*********************************************************************
*
* @purpose  MRT Entry expiry timer Handler
*
* @param    handle  @b{ (input) } Handle to get the MRT Entry.
*
* @returns  None
*
* @comments
*
* @end
*********************************************************************/
static void pimdmMrtEntryExpiryTimerHandler (void *handle)
{
  pimdmMrtEntry_t *mrtEntry =L7_NULLPTR;
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  mfcEntry_t mfcEntry;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  mrtEntry  = (pimdmMrtEntry_t*)handleListNodeRetrieve((L7_uint32)handle);
  if (mrtEntry == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
               "Failed to retrive the MRT entry from the given handle");
    return;
  }

  if (mrtEntry->mrtEntryExpiryTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMDM_MAP_COMPONENT_ID,
            "PIMDM mrtEntryExpiryTimer is NULL, But Still Expired");
    return;
  }
  mrtEntry->mrtEntryExpiryTimer = L7_NULLPTR;

  /* Check with MFC to see if the data has stopped.
   */
  memset (&mfcEntry, 0, sizeof (mfcEntry_t));
  inetCopy (&mfcEntry.source, &mrtEntry->srcAddr);
  inetCopy (&mfcEntry.group, &mrtEntry->grpAddr);

  if (mfcIsEntryInUse (&mfcEntry) != L7_TRUE)
  {
    if (pimdmMrtEntryTryRemove (mrtEntry) == L7_TRUE)
    {
      return;
    }
  }

  /*Start the MRT Entry Expiry Timer */
  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "MRT Entry Expiry Timer Set Failed for Src - %s Grp - %s ",
                 inetAddrPrint(&mrtEntry->srcAddr,src),
                 inetAddrPrint(&mrtEntry->grpAddr,grp));

  osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "MRT Entry Expiry Timer Set Success for Src - %s Grp - %s ",
                 inetAddrPrint(&mrtEntry->srcAddr,src),
                 inetAddrPrint(&mrtEntry->grpAddr,grp));

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmMrtEntryExpiryTimerHandler,
                            (void*) mrtEntry->mrtEntryExpiryTimerHandle,
                            PIMDM_MRT_ENTRY_TIMER_TIMEOUT,
                            errMsgBuf, sucMsgBuf,
                            &mrtEntry->mrtEntryExpiryTimer,
                            "DM-ET2")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to set MRT Entry expiry timer");
    return;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return;
}

/*********************************************************************
*
* @purpose  Compute the Outgoing Interface List and update the MFC
*
* @param    pimdmCB         @b{ (input) }   PIM-DM  Control Block
*           grpAddr         @b{ (input) }   Pointer Group Address
*           grpMask         @b{ (input) }   Pointer Group Mask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtEntryAdminScopeUpdate (pimdmCB_t *pimdmCB,
                               L7_inet_addr_t *grpAddr,
                               L7_inet_addr_t *grpMask)
{
  pimdmMrtEntry_t     *mrtEntry = L7_NULLPTR;
  L7_uchar8            maskLen;
  inetMaskToMaskLen(grpMask,&maskLen);
  mrtEntry = pimdmMrtEntryNextGet (pimdmCB, L7_NULLPTR, L7_NULLPTR);

  while(mrtEntry != L7_NULLPTR)
  {
    if(inetAddrCompareAddrWithMask(&mrtEntry->grpAddr,maskLen,grpAddr,maskLen)
                                == L7_SUCCESS)
    {
      /* Calculate the outgoing list */
      pimdmAdminScopeIntfBitSetGet(pimdmCB,&mrtEntry->grpAddr,
                                   mrtEntry->upstrmRtrIfNum,
                                   &mrtEntry->boundaryGMask);
      pimdmMrtOifListComputeAndApply(pimdmCB,mrtEntry);
    }
    mrtEntry =
        pimdmMrtEntryNextGet(pimdmCB,&mrtEntry->srcAddr,&mrtEntry->grpAddr);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Start the State Refresh Timers if Source is Directly
*           connected.
*
* @param    mrtEntry @b{ (input) } Pointer to the MRT (S,G) Entry
*           ttl      @b{ (input) } Time to Live.
*
* @returns  L7_SUCCESS, if success.
* @returns  L7_FAILURE, if failure.
*
* @comments This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
static void
pimdmMrtEntryStateRfrTimersStart (pimdmMrtEntry_t *mrtEntry, L7_uint32 ttl)
{
  pimdmStrfrEventInfo_t strfrFsmEventInfo;

  if (mrtEntry->sourceDirectlyConnected != L7_TRUE)
  {
    return;
  }

  memset(&strfrFsmEventInfo,0,sizeof(pimdmStrfrEventInfo_t));
  strfrFsmEventInfo.ttl = ttl;

  /* srcAddr is directly connected and data arrived */
  pimdmStateRfrFsmExecute (PIMDM_STATE_RFR_EVT_DATA_ARVL_SRC_DIR_CONN,
                           mrtEntry, &strfrFsmEventInfo);

  /* Initialize the msgSentCount */
  if (mrtEntry->origStateInfo.msgSentCount == 0)
  {
    mrtEntry->origStateInfo.msgSentCount = 1;
  }

  return;
}

/*********************************************************************
*
* @purpose  API to reset the upstream info of MRT if the interface goes down.
*
* @param    mrtEntry @b{ (input) } Pointer to the MRT (S,G) Entry
*           rtrIfNum @b{ (input) } rtrIfNum
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
static void
pimdmMrtUpstrmIntfDownProcess (pimdmMrtEntry_t *mrtEntry, L7_uint32 rtrIfNum)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Upstream is Down.
   * Retain the entry in the PIM-DM Database with everything reset, so that
   * When the new Upstream is discovered, the details will be updated or
   * else the entry will be deleted once the KeepAlive timer expires.
   * Delete the entry from the MFC.
   */
  PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "Reset RPF Info to defaults");

  pimdmMrtMfcEntryDelete (mrtEntry->pimdmCB, &mrtEntry->srcAddr,
                          &mrtEntry->grpAddr, mrtEntry->upstrmRtrIfNum);

  pimdmMrtEntryStatesReset (mrtEntry, L7_FALSE);

  /* Clear the Memory associated with this interface */
  pimdmMrtDnstrmIntfDeInit (mrtEntry, rtrIfNum, L7_TRUE);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return;
}

/*********************************************************************
*
* @purpose  API to reset the donwstream info of MRT if the interface goes down
*
* @param    mrtEntry @b{ (input) } Pointer to the MRT (S,G) Entry
*           rtrIfNum      @b{ (input) } rtrIfNum.
*
* @returns  L7_SUCCESS, if success.
* @returns  L7_FAILURE, if failure.
*
* @comments
*
* @end
*********************************************************************/
static void
pimdmMrtDownstrmIntfDownProcess (pimdmMrtEntry_t *mrtEntry, L7_uint32 rtrIfNum)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (mrtEntry == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "mrtEntry is not a valid pointer");
    return;
  }

  pimdmMrtDnstrmIntfDeInit (mrtEntry, rtrIfNum, L7_TRUE);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return;
}

/*********************************************************************
*
* @purpose  API to reset the donwstream info of MRT if the interface goes down
*
* @param    mrtEntry @b{ (input) } Pointer to the MRT (S,G) Entry
*           rtrIfNum      @b{ (input) } rtrIfNum.
*
* @returns  L7_SUCCESS, if success.
* @returns  L7_FAILURE, if failure.
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmMrtDownstrmNeighborDownProcess (pimdmMrtEntry_t *mrtEntry,
                                     L7_uint32 rtrIfNum,
                                     L7_inet_addr_t *nbrAddr)
{
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;
  pimdmCB_t *pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (mrtEntry == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "mrtEntry is not a valid pointer");
    return L7_FAILURE;
  }

  pimdmCB = mrtEntry->pimdmCB;

  if ((dnstrmIntfInfo = mrtEntry->downstrmStateInfo[rtrIfNum]) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Clear the Downstream Interface Prune State Info for this interface */
  dnstrmIntfInfo->pruneState = PIMDM_DNSTRM_STATE_NOINFO;
  dnstrmIntfInfo->maxPruneHoldTime = PIMDM_PRUNE_HOLD_TIME;
  if (dnstrmIntfInfo->prunePndTimer != L7_NULLPTR)
  {
    pimdmUtilAppTimerCancel (pimdmCB, &(dnstrmIntfInfo->prunePndTimer));
  }
  if (dnstrmIntfInfo->pruneTimer != L7_NULLPTR)
  {
    pimdmUtilAppTimerCancel (pimdmCB, &(dnstrmIntfInfo->pruneTimer));
  }

  asrtIntfInfo = &dnstrmIntfInfo->downstrmAssertInfo;

  /* Clear the Downstream Interface Assert State Info for this interface */
  asrtIntfInfo->couldAssert = L7_TRUE;
  asrtIntfInfo->assertWnrAsrtMetric = PIMDM_ASSERT_METRIC_INFINITY;
  asrtIntfInfo->assertWnrAsrtPref = PIMDM_ASSERT_METRIC_INFINITY;
  inetAddressZeroSet (pimdmCB->addrFamily, &asrtIntfInfo->assertWnrAddr);
  if (asrtIntfInfo->assertTimer != L7_NULLPTR)
  {
    pimdmUtilAppTimerCancel (mrtEntry->pimdmCB, &(asrtIntfInfo->assertTimer));
  }

  /* Reset the interface from all the interface bitsets */
  BITX_RESET(&mrtEntry->immediateOifList,rtrIfNum);
  BITX_RESET(&mrtEntry->downstrmPruneMask,rtrIfNum);
  BITX_RESET(&mrtEntry->lostAssertMask,rtrIfNum);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Perform the Interface Up notification
*
* @param    pimdmCB    @b{ (input) } Pointer PIMDM Control Block
*           rtrIfNum   @b{ (input) } Index of the Interface that has
*                                    come up
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtIntfUpUpdate (pimdmCB_t *pimdmCB,
                      L7_uint32  rtrIfNum)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  mrtEntry = pimdmMrtEntryNextGet(pimdmCB,L7_NULLPTR,L7_NULLPTR);

  while (mrtEntry != L7_NULLPTR)
  {
/*    if (mrtEntry->upstrmRtrIfNum != rtrIfNum)*/
    {
      if (pimdmMrtDnstrmIntfInit (mrtEntry, rtrIfNum) != L7_SUCCESS)
      {
        PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Dnstrm Intf-%d Creation Failed",
                     rtrIfNum);
        return L7_FAILURE;
      }
    }

    mrtEntry = pimdmMrtEntryNextGet (pimdmCB, &mrtEntry->srcAddr,
                                     &mrtEntry->grpAddr);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Initialize the Downstream Interface Information
*
* @param    mrtEntry @b{ (input) } Pointer to the MRT (S,G) Entry
*           rtrIfNum @b{ (input) } rtrIfNum
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtDnstrmIntfInit (pimdmMrtEntry_t *mrtEntry,
                        L7_uint32 rtrIfNum)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (mrtEntry->downstrmStateInfo[rtrIfNum] != L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Entry is active");
    return L7_SUCCESS;
  }

  pimdmCB = mrtEntry->pimdmCB;

  if ((dnstrmIntfInfo = PIMDM_ALLOC (pimdmCB->addrFamily,
                                     sizeof (pimdmDownstrmIntfInfo_t)))
                                  == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Heap Alloc for Dnstrm Intf-%d Failed",
                 rtrIfNum);
    return L7_FAILURE;
  }

  mrtEntry->downstrmStateInfo[rtrIfNum] = dnstrmIntfInfo;

  /* Initialize Downstream Prune State Details */
  dnstrmIntfInfo->mrtEntry = mrtEntry;
  dnstrmIntfInfo->rtrIfNum = rtrIfNum;
  dnstrmIntfInfo->pruneState = PIMDM_DNSTRM_STATE_NOINFO;
  dnstrmIntfInfo->maxPruneHoldTime = PIMDM_PRUNE_HOLD_TIME;

  /* Initialize Downstream Assert State Details */
  asrtIntfInfo = &dnstrmIntfInfo->downstrmAssertInfo;

  asrtIntfInfo->assertState = PIMDM_ASSERT_STATE_NOINFO;
  asrtIntfInfo->couldAssert = L7_TRUE;
  asrtIntfInfo->assertWnrAsrtMetric = PIMDM_ASSERT_METRIC_INFINITY;
  asrtIntfInfo->assertWnrAsrtPref = PIMDM_ASSERT_METRIC_INFINITY;
  inetAddressZeroSet (pimdmCB->addrFamily, &asrtIntfInfo->assertWnrAddr);

  /* Create the Downstream Handle */
  if (dnstrmIntfInfo->mrtEntryDnstrmTmrHndlParam == L7_NULL)
  {
    dnstrmIntfInfo->mrtEntryDnstrmTmrHndlParam =
              handleListNodeStore (pimdmCB->handleList, (void*) dnstrmIntfInfo);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  De-Initialize the Downstream Interface Information
*
* @param    mrtEntry  @b{ (input) } Pointer to the MRT (S,G) Entry
*           rtrIfNum  @b{ (input) } rtrIfNum
*           forceFree @b{ (input) } Free the Dnstrm Intf Memory Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtDnstrmIntfDeInit (pimdmMrtEntry_t *mrtEntry,
                          L7_uint32 rtrIfNum,
                          L7_BOOL forceFree)
{
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmAssertEventInfo_t asrtFsmEventInfo;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (mrtEntry == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "mrtEntry is not a valid pointer");
    return L7_FAILURE;
  }

  pimdmCB = mrtEntry->pimdmCB;

  if ((dnstrmIntfInfo = mrtEntry->downstrmStateInfo[rtrIfNum]) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Clear the Downstream Interface Prune State Info for this interface */
  dnstrmIntfInfo->pruneState = PIMDM_DNSTRM_STATE_NOINFO;
  dnstrmIntfInfo->maxPruneHoldTime = PIMDM_PRUNE_HOLD_TIME;
  if (dnstrmIntfInfo->prunePndTimer != L7_NULLPTR)
  {
    pimdmUtilAppTimerCancel (pimdmCB, &(dnstrmIntfInfo->prunePndTimer));
  }
  if (dnstrmIntfInfo->pruneTimer != L7_NULLPTR)
  {
    pimdmUtilAppTimerCancel (pimdmCB, &(dnstrmIntfInfo->pruneTimer));
  }

  /*************** ASSERT FSM EVENTS *********************/

  /* Prepare the Assert Event Information.
   */
  asrtFsmEventInfo.rtrIfNum = rtrIfNum;
  asrtFsmEventInfo.assertMetric = PIMDM_ASSERT_METRIC_INFINITY;
  asrtFsmEventInfo.assertPref = PIMDM_ASSERT_METRIC_INFINITY;

  /* EVENT - couldAssert is FALSE.
   */
  pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_COULD_ASRT_FALSE, mrtEntry,
                         &asrtFsmEventInfo);

  /* Clear the Downstream Interface Assert State Info for this interface */
  asrtIntfInfo = &dnstrmIntfInfo->downstrmAssertInfo;
  asrtIntfInfo->couldAssert = L7_TRUE;
  asrtIntfInfo->assertWnrAsrtMetric = PIMDM_ASSERT_METRIC_INFINITY;
  asrtIntfInfo->assertWnrAsrtPref = PIMDM_ASSERT_METRIC_INFINITY;
  inetAddressZeroSet (pimdmCB->addrFamily, &asrtIntfInfo->assertWnrAddr);
  if (asrtIntfInfo->assertTimer != L7_NULLPTR)
  {
    pimdmUtilAppTimerCancel (mrtEntry->pimdmCB, &(asrtIntfInfo->assertTimer));
  }

  /* Reset the interface from all the interface bitsets */
  BITX_RESET(&mrtEntry->immediateOifList,rtrIfNum);
  BITX_RESET(&mrtEntry->downstrmPruneMask,rtrIfNum);
  BITX_RESET(&mrtEntry->lostAssertMask,rtrIfNum);

  /* Clear the Group Membership details */
  /* NOTE: It looks like there is no need to reset these as the OIF List
   * compute takes care of these.
  BITX_RESET(&mrtEntry->pimIncludeStarG,rtrIfNum);
  BITX_RESET(&mrtEntry->pimIncludeSG,rtrIfNum);
  BITX_RESET(&mrtEntry->pimExcludeSG,rtrIfNum);
   */

  /* Check for any Upstream State Transitions and re-calculate the OIF List */
  pimdmMrtOifInterfaceSet (mrtEntry, rtrIfNum, L7_FALSE);

  /* Release the Heap Memory */
  if (forceFree == L7_TRUE)
  {
    dnstrmIntfInfo->mrtEntry = L7_NULLPTR;
    PIMDM_FREE (pimdmCB->addrFamily, (void*) dnstrmIntfInfo);
    mrtEntry->downstrmStateInfo[rtrIfNum] = L7_NULLPTR;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Handle the scenario where in the Neighbor's Gen ID is
            changed
*
* @param    pimdmCB    @b{ (input) } Pointer PIMDM Control Block
*           rtrIfNum   @b{ (input) } Neighbor's Router Interface Number
*           nbrAddr    @b{ (input) } Neighbor's Address
*
* @returns  L7_SUCCESS, if success.
* @returns  L7_FAILURE, if failure.
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmMrtNbrGenIDChangeProcess (pimdmCB_t *pimdmCB,
                               L7_uint32 rtrIfNum,
                               L7_inet_addr_t *nbrAddr)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  mrtEntry = pimdmMrtEntryNextGet (pimdmCB, L7_NULLPTR, L7_NULLPTR);
  while (mrtEntry != L7_NULLPTR)
  {
    /* If Neighbor is Upstream ... */
    if ((L7_INET_ADDR_COMPARE (&mrtEntry->upstrmNbrInfo.assertWnrAddr, nbrAddr)
                           == 0) &&
        (mrtEntry->upstrmRtrIfNum == rtrIfNum))
    {
      pimdmUpstrmEventInfo_t upstrmFsmEventInfo;

      /* If the Upstream Interface is Pruned (PLT Running), Send a Prune and
       * Reset the PLT.
       */
      if (mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer != L7_NULLPTR)
      {
        /*************** UPSTREAM FSM EVENTS *********************/

        /* Prepare the Upstream Event Information.
         */
        upstrmFsmEventInfo.rtrIfNum = mrtEntry->upstrmRtrIfNum;

        /* Event: PIMDM_UPSTRM_EVT_STRFR_ARVL_PI_FALSE_PLT_RUNNING.
         */
        pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_STRFR_ARVL_PI_FALSE_PLT_RUNNING,
                               mrtEntry, &upstrmFsmEventInfo);
      }
    }
    /* If Neighbor is Downstream ... */
    else
    {
      pimdmStrfrEventInfo_t strfrFsmEventInfo;

     /*************** STATE REFRESH FSM EVENTS *********************/

     /* Prepare the State Refresh Event Information.
      */
     strfrFsmEventInfo.rtrIfNum = rtrIfNum;

     /* EVENT - Neighbor Generation ID changed.
      */
     pimdmStateRfrFsmExecute (PIMDM_STATE_RFR_EVT_NBR_GENID_CHANGED, mrtEntry,
                              &strfrFsmEventInfo);
    }

    mrtEntry = pimdmMrtEntryNextGet (pimdmCB, &(mrtEntry->srcAddr),
                                     &(mrtEntry->grpAddr));
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

