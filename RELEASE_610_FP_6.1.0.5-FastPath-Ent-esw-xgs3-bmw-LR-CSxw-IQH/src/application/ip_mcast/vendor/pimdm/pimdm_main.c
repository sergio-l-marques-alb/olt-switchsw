/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_main.c
*
* @purpose    
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

#include "l3_comm_structs.h"
#include "l3_mcast_commdefs.h"
#include "l7_socket.h"
#include "buff_api.h"
#include "sysnet_api.h"
#include "l7_mcast_api.h"
#include "l7_mgmd_api.h"
#include "l7_pimdm_api.h"
#include "mfc_api.h"
#include "pimdm_api.h"
#include "pimdm_main.h"
#include "pimdm_debug.h"
#include "pimdm_pkt.h"
#include "pimdm_ctrl.h"
#include "pimdm_mrt.h"
#include "pimdm_util.h"
#include "pimdm_intf.h"
#include "pimdm_mgmd.h"
#include "pimdm_admin_scope.h"
#include "pimdm_map_vend_ctrl.h"
#include "pimdm_map.h"

/*******************************************************************************
**                        Function Declarations                               **
*******************************************************************************/

static L7_RC_t
pimdmGlobalAdminModeEnable (pimdmCB_t *pimdmCB, pimdmMapEvent_t *mapLayerInfo,
                            L7_uint32 adminMode);

static L7_RC_t
pimdmGlobalAdminModeDisable (pimdmCB_t *pimdmCB);

static L7_RC_t
pimdmMFCEventHandler (pimdmCB_t *pimdmCB, L7_uint32 mfcEventType,
                      mfcEntry_t *mfcInfo);

static L7_RC_t
pimdmRTOEventHandler (pimdmCB_t *pimdmCB);

static L7_RC_t
pimdmMGMDEventHandler (pimdmCB_t *pimdmCB, L7_uint32 mgmdEventType,
                       mgmdMrpEventInfo_t *mgmdInfo);

static L7_RC_t
pimdmAdminScopeBoundaryEventHandler (pimdmCB_t *pimdmCB, L7_uint32 asbEventType,
                                     mcastAdminMsgInfo_t *asbInfo);

static L7_RC_t
pimdmStaticMRouteChangeEventHandler (pimdmCB_t *pimdmCB,
                                     L7_uint32 eventType,
                                     mcastRPFInfo_t *rpfInfo);

static L7_RC_t
pimdmTimerEventHandler (pimdmCB_t *pimdmCB);

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/
/* None */

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  Process the PIM-DM Global Admin Mode Enable event
*
* @param    pimdmCB      @b{ (input) } Pointer to the PIM-DM Ctrl Block
*           mapLayerInfo @b{ (input) } Mapping Layer Event Info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmGlobalAdminModeEnable (pimdmCB_t *pimdmCB,
                            pimdmMapEvent_t *mapLayerInfo,
                            L7_uint32 adminMode)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (pimdmCB->operMode == L7_TRUE)
  {
    return L7_SUCCESS;
  }

  /* Enable the PIM-DM Operational Flag.
   * Do this at last in order to populate the Operational data.
   */
  pimdmCB->operMode = L7_TRUE;

  /* Start the PIM-DM Join/Prune Bundle Timer */
    if (pimdmUtilAppTimerSet (pimdmCB, pimdmJPBundleTimerExpiryHandler,
                              (void*) pimdmCB, PIMDM_DEFAULT_JP_BUNDLE_TIME,
                              L7_NULLPTR, L7_NULLPTR,
                              &pimdmCB->pimdmJPBundleTimer,
                              "DM-JPB2")
                           != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Join/Prune Bundle Timer Start Failed");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "PIM-DM Global Admin Mode Enabled");
  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Process the PIM-DM Global Admin Mode Disable event
*
* @param    pimdmCB     @b{ (input) } Pointer to the PIM-DM Ctrl Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments    This API deletes all the PIM-DM Operational data.
*
* @end
*********************************************************************/
static L7_RC_t
pimdmGlobalAdminModeDisable (pimdmCB_t *pimdmCB)
{
  L7_RC_t retVal = L7_SUCCESS;
  L7_uint32 rtrIfNum = 0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (pimdmCB->operMode == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  /* Disable the PIM-DM Operational Flag.
   * Do this first in order to stop anyother incoming events.
   */
  pimdmCB->operMode = L7_FALSE;

  /* For all the Enabled PIM-DM Interfaces, clear the operational
   * data.  This takes care of deleting all the MRT (S,G) Entries
   * and MGMD Group Membership entreis associated with the interface.
   */
  for (rtrIfNum = 1; rtrIfNum < PIMDM_MAX_INTERFACES; rtrIfNum++)
  {
    if (pimdmIntfIsEnabled (pimdmCB, rtrIfNum) != L7_TRUE)
    {
      continue;
    }

    retVal |= pimdmIntfDown (pimdmCB, rtrIfNum);
  }

  /* Stop the PIM-DM Join/Prune Bundle Timer */
  if(pimdmCB->pimdmJPBundleTimer != L7_NULLPTR)
  {
    pimdmUtilAppTimerCancel (pimdmCB, &pimdmCB->pimdmJPBundleTimer);
  }

  /* Act on PIM-DM MRT database to remove any Stale entries */
  pimdmMrtAdminDownAction (pimdmCB);

  PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "PIM-DM Global Admin Mode Disabled");
  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  To set PIMDM Global Admin Mode.
*
* @param    pimdmCbHandle    @b{ (input) } PIM-DM Ctrl Block Handle
* @param    mapLayerInfo     @b{ (input) } Mapping Layer Event Info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments All the SET commands will be posted to the Vendor by the
*           PIM-DM Mapping Layer.
*
* @end
*********************************************************************/
L7_RC_t
pimdmGlobalAdminModeSet (MCAST_CB_HNDL_t pimdmCbHandle,
                         pimdmMapEvent_t *mapLayerInfo)
{
  pimdmCB_t          *pimdmCB = (pimdmCB_t *) pimdmCbHandle;
  L7_RC_t            retVal = L7_FAILURE;
  L7_uint32          adminMode;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB == L7_NULLPTR) || (mapLayerInfo == L7_NULLPTR))
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Input Validations Failed");
    return L7_FAILURE;
  }

  adminMode = mapLayerInfo->msg.adminMode.mode;
  PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "PIMDMMAP_GLBL_ADMINMODE_SET with Mode %d",
               adminMode);

  if (adminMode == L7_ENABLE)
  {
    retVal = pimdmGlobalAdminModeEnable (pimdmCB, mapLayerInfo, adminMode);
  }
  else if (adminMode == L7_DISABLE)
  {
    retVal = pimdmGlobalAdminModeDisable (pimdmCB);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Handler for MFC Component Events
*
* @param    pimdmCB       @b{ (input) } Pointer to the PIM-DM Ctrl Block
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
static L7_RC_t
pimdmMFCEventHandler (pimdmCB_t *pimdmCB,
                      L7_uint32 mfcEventType,
                      mfcEntry_t *mfcInfo)
{
  L7_RC_t retVal = L7_FAILURE;
  L7_inet_addr_t *srcAddr = L7_NULLPTR;
  L7_inet_addr_t *grpAddr = L7_NULLPTR;
  L7_uint32 rtrIfNum = 0;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (mfcInfo == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Invalid MFC Info");
    return L7_FAILURE;
  }

  /* Check if PIM-DM Protocol Instance is Operational.
   */
  if (pimdmCB->operMode == L7_FALSE)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "PIM-DM is Not Operational");
    return L7_FAILURE;
  }

  srcAddr = &(mfcInfo->source);
  grpAddr = &(mfcInfo->group);
  rtrIfNum = mfcInfo->iif;

  /* Check if PIM-DM is Active on the Incoming Interface.
   */
  if (pimdmIntfIsEnabled (pimdmCB, rtrIfNum) != L7_TRUE)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIM-DM is Not Active on Interface - %d",
                 rtrIfNum);
    return L7_FAILURE;
  }

  switch (mfcEventType)
  {
    case MCAST_MFC_NOCACHE_EVENT:
    {
      PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "MCAST_MFC_NOCACHE_EVENT with IIF - %d "
                   "Src - %s Grp - %s", rtrIfNum,
                   inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

      retVal = pimdmMrtMfcNoCacheEventProcess (pimdmCB, srcAddr, grpAddr,
                                               rtrIfNum, mfcInfo->dataTTL);
      if(retVal == L7_SUCCESS)
      {
        pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_DATA_NO_CACHE,
                              PIMDM_STATS_RECEIVED);
      }
      else
      {
        pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_DATA_NO_CACHE,
                              PIMDM_STATS_DROPPED);
      }
      break;
    }

    case MCAST_MFC_WRONGIF_EVENT:
    {
      PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "MCAST_MFC_WRONGIF_EVENT with IIF - %d "
                   "Src - %s Grp - %s", rtrIfNum,
                   inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

      retVal = pimdmMrtMfcWrongIfEventProcess (pimdmCB, srcAddr, grpAddr,
                                               rtrIfNum);
      if(retVal == L7_SUCCESS)
      {
        pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_DATA_WRONG_IF,
                              PIMDM_STATS_RECEIVED);
      }
      else
      {
        pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_DATA_WRONG_IF,
                              PIMDM_STATS_DROPPED);
      }
      break;
    }

    case MCAST_MFC_ENTRY_EXPIRE_EVENT:
    {
      PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "MCAST_MFC_ENTRY_EXPIRE_EVENT with "
                   "IIF - %d Src - %s Grp - %s", rtrIfNum,
                   inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

      retVal = pimdmMrtMfcExpiryEventProcess (pimdmCB, srcAddr, grpAddr);
      break;
    }

    default:
      PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "Invalid MFC Event - %d received",
                   mfcEventType);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Handler for Admin Scope Boundary Events
*
* @param    pimdmCB      @b{ (input) } Pointer to the PIM-DM Ctrl Block
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
static L7_RC_t
pimdmAdminScopeBoundaryEventHandler (pimdmCB_t *pimdmCB,
                       L7_uint32 asbEventType,
                       mcastAdminMsgInfo_t *asbInfo)
{
  L7_RC_t retVal = L7_FAILURE;
  L7_uint32 rtrIfNum =0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (asbInfo == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Admin Scope Boundary Info");
    return L7_FAILURE;
  }

  if(mcastIpMapIntIfNumToRtrIntf(pimdmCB->addrFamily,asbInfo->intIfNum,&rtrIfNum) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to convert intIfNum(%d) to rtrIfNum",asbInfo->intIfNum);
    return L7_FAILURE;
  }

  switch (asbInfo->mode)
  {
    case L7_ENABLE:
    {
      PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "MCAST_EVENT_ADMINSCOPE_BOUNDARY ADD");

      retVal = pimdmAdminScopeBoundarySet (pimdmCB,
                                           &(asbInfo->groupAddress),
                                           &(asbInfo->groupMask),
                                           rtrIfNum);
      break;
    }

    case L7_DISABLE:
    {
      PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "MCAST_EVENT_ADMINSCOPE_BOUNDARY DELETE");

      retVal = pimdmAdminScopeBoundaryReset (pimdmCB,
                                             &(asbInfo->groupAddress),
                                             &(asbInfo->groupMask),
                                             rtrIfNum);
      break;
    }

    default:
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Admin Scope Mode - %d received", asbInfo->mode);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Handler for Admin Scope Boundary Events
*
* @param    pimdmCB      @b{ (input) } Pointer to the PIM-DM Ctrl Block
*           eventType    @b{ (input) } Static MRoute Event ID
*           rpfInfo      @b{ (input) } Static MRoute Event Info 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmStaticMRouteChangeEventHandler (pimdmCB_t *pimdmCB,
                                     L7_uint32 eventType,
                                     mcastRPFInfo_t *rpfInfo)
{
  L7_RC_t retVal = L7_FAILURE;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (rpfInfo == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Static MRoute Info");
    return L7_FAILURE;
  }

  if (pimdmMrtRTOBestRouteChangeProcess (pimdmCB, rpfInfo) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Failed to process Static MRoute Best Route Changes");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Handler for RTO Component Events
*
* @param    pimdmCB        @b{ (input) } Pointer to the PIM-DM Ctrl Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimdmRTOEventHandler (pimdmCB_t *pimdmCB)
{
  L7_RC_t retVal = L7_FAILURE;
 
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (pimdmCB->operMode == L7_FALSE)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIM-DM is Not Operational");
    return L7_FAILURE;
  }

  switch (pimdmCB->addrFamily)
  {
    case L7_AF_INET:
    {
      PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "MCAST_EVENT_RTO4_BEST_ROUTE_CHANGE");
      retVal = pimdmV4BestRoutesGet(pimdmCB); 
      break;
    }

    case L7_AF_INET6:
    {
      PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "MCAST_EVENT_RTO6_BEST_ROUTE_CHANGE");
      retVal = pimdmV6BestRoutesGet(pimdmCB); 
      break;
    }

    default:
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Addr Family - %d received",
                   pimdmCB->addrFamily);
      retVal = L7_FAILURE; 
      break;
    }
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Handler for MGMD Component Events
*
* @param    pimdmCB       @b{ (input) } Pointer to the PIM-DM Ctrl Block
*           mgmdEventType @b{ (input) } MGMD Event ID
*           mgmdInfo      @b{ (input) } MGMD Event Info 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmMGMDEventHandler (pimdmCB_t *pimdmCB,
                       L7_uint32 mgmdEventType,
                       mgmdMrpEventInfo_t *mgmdInfo)
{
  L7_RC_t retVal = L7_FAILURE;

  if (mgmdInfo == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid MGMD Info");
    return L7_FAILURE;
  }

  /* Check If PIM-DM Protocol Instance is Operational &
   * If PIM-DM is Active on the Interface.
   */
  if (pimdmCB->operMode == L7_FALSE)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIM-DM is Not Operational/Active on the "
                 "Interface - %d", mgmdInfo->rtrIfNum);
    return L7_FAILURE;
  }

  switch (mgmdEventType)
  {
    case MCAST_EVENT_MGMD_GROUP_UPDATE:

      PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "MCAST_EVENT_MGMD_GROUP_UPDATE");

      retVal = pimdmMgmdMsgProcess (pimdmCB, mgmdEventType, mgmdInfo);
      break;

    default:
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid MGMD Event - %d received",
                   mgmdEventType);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Handler for PIM-DM Timer Events
*
* @param    pimdmCB     @b{ (input) } Pointer to the PIM-DM Ctrl Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmTimerEventHandler (pimdmCB_t *pimdmCB)
{
  /* PIMDM_TRACE (PIMDM_DEBUG_API, "Entry"); */
  /*PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "MCAST_EVENT_PIMDM_TIMER_EXPIRY");*/

  if (pimdmCB == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  appTimerProcess (pimdmCB->appTimer);

  /* PIMDM_TRACE (PIMDM_DEBUG_API, "Exit"); */
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  This function process PIMDM Events.
*
* @param    familyType     @b{ (input) } address family.
* @param    eventType      @b{ (input) } Event.
* @param    pMsg           @b{ (input) } Message.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  This function recieves all the messages and events and 
*            call the appropriate handler.
*
* @end
*********************************************************************/
L7_RC_t
pimdmEventProcess (L7_uchar8 familyType,
                   mcastEventTypes_t eventType, 
                   void *pMsg)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  L7_RC_t retVal = L7_FAILURE;

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");*/

  /* Get the PIM-DM Control Block from Handle */
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (familyType))
              == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  switch (eventType)
  {
    case MCAST_EVENT_PIMDM_TIMER_EXPIRY:  /* PIM-DM Appn Timer Expiry Event */
    {
      retVal = pimdmTimerEventHandler (pimdmCB);
      break;
    }

    case MCAST_EVENT_PIMDM_CONTROL_PKT_RECV:/* IPv4 Control Pkt Recv Event */
    case MCAST_EVENT_IPv6_CONTROL_PKT_RECV: /* IPv6 Control Pkt Recv Event */
    {
      mcastControlPkt_t *ctrlPkt = (mcastControlPkt_t *)pMsg;

      /* Copy the received packet to a local buffer and Free the MCAST Buffer */
      memset (pimdmCB->pktRxBuf, 0, PIMDM_MAX_PDU);
      memcpy (pimdmCB->pktRxBuf, (L7_uchar8*) ctrlPkt->payLoad, PIMDM_MAX_PDU);
      mcastCtrlPktBufferPoolFree (pimdmCB->addrFamily, ctrlPkt->payLoad);

      pimdmCtrlPktReceive (pimdmCB, ctrlPkt->rtrIfNum, &ctrlPkt->srcAddr,
                                    &ctrlPkt->destAddr, pimdmCB->pktRxBuf,
                                    ctrlPkt->length);
      retVal = L7_SUCCESS;
      break;
    }

    case MCAST_MFC_NOCACHE_EVENT:          /* MFC No Cache Entry Event */
    case MCAST_MFC_WRONGIF_EVENT:          /* MFC Wrong Interface Event */
    case MCAST_MFC_ENTRY_EXPIRE_EVENT:     /* MFC Entry Expiry Event */
    {
      retVal = pimdmMFCEventHandler (pimdmCB, eventType,
                                     (mfcEntry_t*)pMsg);
      break;
    }

    case MCAST_EVENT_RTO_BEST_ROUTE_CHANGE: /* RTO Route Change Event */
    {
      retVal = pimdmRTOEventHandler (pimdmCB);
      break;
    }

    case MCAST_EVENT_MGMD_GROUP_UPDATE:     /* MGMD Group Update Event */
    {
      mgmdMrpEventInfo_t *mgmdMrpGrpInfo = L7_NULLPTR;

      mgmdMrpGrpInfo = (mgmdMrpEventInfo_t *) pMsg;
      retVal = pimdmMGMDEventHandler (pimdmCB, eventType, mgmdMrpGrpInfo);

      /* Free the MCAST MGMD Events Source List Buffers */
      if (retVal == L7_SUCCESS)
      {
        mcastMgmdEventsBufferPoolFree (pimdmCB->addrFamily, mgmdMrpGrpInfo);
      }
      break;
    }

    case MCAST_EVENT_ADMINSCOPE_BOUNDARY:   /* Admin Scope Boundary Event */
    {
      retVal = pimdmAdminScopeBoundaryEventHandler (pimdmCB, eventType,
                                                    (mcastAdminMsgInfo_t*)pMsg);
      break;
    }

    case MCAST_EVENT_STATIC_MROUTE_CHANGE:   /* Static MRoute Change Event */
    {
      retVal = pimdmStaticMRouteChangeEventHandler (pimdmCB, eventType,
                                                    (mcastRPFInfo_t*)pMsg);
      break;
    }

    default:
      PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "Invalid Event - %d received", eventType);

  } /* switch (eventType) */

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");*/
  return retVal;
}

