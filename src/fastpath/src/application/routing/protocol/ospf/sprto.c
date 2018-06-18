 /********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename  sprto.c
 *
 * @purpose    OSPF Router Object (RTO) module.
 *
 * @component  Routing OSPF Component
 *
 * @comments  ExternalRoutines:
 * RTO_Init
 * RTO_Delete
 * RTO_SetMngId
 * RTO_Config
 * RTO_PurgeExternalLsas
 * RTO_SetCallbacks
 * RTO_AsExternalCfg
 * RTO_TrapControl
 * RTO_GetNextHop
 * RTO_ShowStatitstics
 * RtbPrint
 * RtbEntryPrint
 * RTO_TrapPrint
 * RTO_LsaDbPrint
 * RTO_Cleanup
 * ForMIB:
 * RTO_BindOspfMibObject
 * RTO_GetOspfMibObject
 * InternalRoutines:
 * RTO_Kill
 * RteDelete
 * TimerExpirationRoutines
 * OverflowTimerExp
 *
 *
 * @create     01/27/1999
 *
 * @author     Dan Dovolsky
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\application\protocol\ospf\sprto.c 1.1.3.7 2002/09/11 14:51:24EDT anayar Exp  $";
#endif



/* --- standard include files --- */
#include <stdio.h>
#include "l7_common.h"
#include "osapi.h"
#include "nimapi.h"
#include "simapi.h"

#include "std.h"

#include "mem.ext"

/* --- external object interfaces --- */

/* --- specific include files --- */

#include "spobj.h"
#include "osapi.h"
#include "mempool.h"
#include "os_xxcmn.h"
#include "ospf_debug_api.h"

#ifdef L7_NSF_PACKAGE
#include "ospf_ckpt.h"
#endif

/* --- internal prototypes --- */
e_Err RTO_Kill(void *p_Info);

t_RTO        *RTO_List;       /* RTO object list  */

/* --- internal (static) data  --- */
static char text[200];
#define ADR_PRINT(a) (byte)(a>>24),(byte)(a>>16),(byte)(a>>8),(byte)a
static Bool RteDelete(byte *RteId, ulng rtbId);
e_Err RTO_HandleOverflow(t_RTO *p_RTO);
e_Err OverflowTimerExp( t_Handle Id, t_Handle TimerId, word Flag );
e_Err OspfTickTimerExp( t_Handle Id, t_Handle TimerId, word Flag );
extern void RTB_ComputeCalcDelay(t_RTB *p_RTB, t_RTO *p_RTO);
extern void UpdateRejectRoute(t_RTO *p_RTO, t_AgrEntry *p_AdrRange,
                              e_RtChange rtChange, Bool delayOp);
extern void cleanupAggregateRtes(t_AgrEntry *p_AdrRange);
extern e_Err LsaOrgSumBulk(t_Handle Id);
void ClearLsaList(t_Handle avlh);
static e_Err RTO_CalcForwardingAddr(t_RTO *p_RTO, SP_IPADR *nexthop, SP_IPADR *fwdAddr);

/*********************************************************************
 * @purpose      Initialize an RTO object
 *
 *
 * @param         @b{(input)}  4 bytes of user-info
 * @param         @b{(input)}  OSPF's thread handler
 * @param         @b{(input)}  OSPF routing table process thread id
 * @param         @b{(input)}  Create default (public) Routing Table
 *                             Object
 * @param         @b{(output)}  RTO Object handler returned here
 *
 * @returns       E_OK           success
 * @returns       E_FAILED       couldn't create an object
 * @returns       E_NOMEMORY     not enough memory for resources allocating
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_Init(t_SysLabel *p_OspfSysLabel, t_SysLabel *p_OspfRtbThread, 
               Bool CreateDefaultRTB, t_Handle *p_RTO_Id)
{
   t_RTO   *p_RTO;
   e_Err   e = E_OK;

   /* try allocate from user memory */
   p_RTO = (t_RTO *)XX_Malloc( sizeof(t_RTO) );
   if(p_RTO)
      memset( p_RTO, 0, sizeof(t_RTO) );
   else
      return E_NOMEMORY;

   /* initialize the object */

   XX_AddToDLList(p_RTO, RTO_List);

   p_RTO->Taken = TRUE;
   p_RTO->neighborsToRouter = 0;
   p_RTO->restartStatus = OSPF_GR_NOT_RESTARTING;
   p_RTO->grExitReason = OSPF_GR_EXIT_NONE;

   /* overflow timer handler */
   e |= TIMER_InitSec(1, p_RTO, &p_RTO->OverflowTimer);
   e |= TIMER_InitSec(1, p_RTO, &p_RTO->gracePeriodTimer);

   /* overflow timer handler */
   e |= TIMER_InitSec(1, p_RTO, &p_RTO->LsdbOverloadTimer);

   /* for ASBR allocate local attached AS-external routes Hash List */
   e |= HL_Init(0, 2*sizeof(SP_IPADR), offsetof(t_S_AsExternalCfg, DestNetIpAdr),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_RTO->AsExtRoutesHl);

   /* allocate AS-external LSAs Hash List */
   e |= AVLH_Init(0, sizeof(t_A_DbKey),
               /* key offset in t_A_DbEntry */
               offsetof(t_A_DbEntry,Lsa) + offsetof(t_S_LsaHeader, LsId),
               NULL, NULL, &p_RTO->AsExternalLsaHl);

   /* allocate AS-Opaque LSAs Hash List */
   e |= AVLH_Init(0, sizeof(t_A_DbKey),
               /* key offset in t_A_DbEntry */
               offsetof(t_A_DbEntry,Lsa) + offsetof(t_S_LsaHeader, LsId),
               NULL, NULL, &p_RTO->AsOpaqueLsaHl);

   /* allocate delayed LSAs list */
   e |= HL_Init(FREE_ON_DEL, sizeof(t_DlyLsaKey),
               /* key offset in t_A_DbEntry */
               offsetof(t_DelayedLsa,LsaType),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_RTO->DelayedLsa);


   /* delayed LSA timer init */
   e |= TIMER_InitSec(1, p_RTO, &p_RTO->DlyLsaTimer);

   /* allocate Attached Areas Objects hash list */
   e |= HL_Init(0, sizeof(SP_IPADR),
               /* key offset in t_ARO */
               offsetof(t_ARO,AreaId),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_RTO->AroHl);

   /* allocate routing table binary tree objects hash list */
   e |= HL_Init(0, sizeof(SP_IPADR),
               /* key offset in t_RTB */
               offsetof(t_RTB,VpnCos),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_RTO->RtbHl);

   /* allocate IFO indexed by Layer 2 index hash list */
   e |= HL_Init(OWN_LINK, sizeof(t_LIH),
               /* key offset in t_IFO */
               offsetof(t_IFO,Cfg)+offsetof(t_IFO_Cfg, IfIndex),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_RTO->IfoIndexHl);

#if L7_OSPF_TE
   /* allocate FA indexed by Layer 2 index hash list */
   e |= HL_Init(0, sizeof(t_LIH),
               /* key offset in t_FA */
                offsetof(t_FA, FaIndex),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_RTO->FaIndexHl);

   /* Create the Bitset object used as allocator of instance number */
   /* of TE Lsa sent by this router. Mark the router LSA instance   */
   /* bit (bit number - 0xFFFF) as TRUE (i.e. busy)                 */
   e |= BS_Init(TE_LSA_INST_MAX_NUMBER, BS_IS_ALLOC, &p_RTO->TeLsaInstBS);
   if(e == E_OK)
      e |= BS_SetBit(p_RTO->TeLsaInstBS, TE_LSA_ROUTER_INST, TRUE);
#endif

   /* Copy Configuration */

   if(p_OspfSysLabel == NULL)
      p_RTO->OspfSysLabel.threadHndle = DEFAULT_TIMER_THREAD;
   else
      p_RTO->OspfSysLabel = *p_OspfSysLabel;

   if(p_OspfRtbThread == NULL)
   {
      XX_CreateThread(255, NULL, FALSE, &p_RTO->OspfRtbThread.threadHndle, "OSPF RTB");
      p_RTO->RtbOwnThread = 1;
   }
   else
      p_RTO->OspfRtbThread = *p_OspfRtbThread;

   /* Set Router configuration defaults */

   p_RTO->Cfg.ExtLsdbLimit = -1;
   p_RTO->Cfg.MaximumPaths = DEFAULT_MAX_PATHS;
   p_RTO->TrapControl = 0;       /* disable all traps */
   p_RTO->Cfg.AdminStat = FALSE; /* disable router functionality */
   p_RTO->Cfg.RouterStatus = ROW_CREATE_AND_WAIT;
   p_RTO->Cfg.RFC1583Compatibility = TRUE;
   p_RTO->Cfg.VersionNumber = 2;
   p_RTO->Cfg.DelAreaOnLastIfDel = TRUE;
   p_RTO->Cfg.LsRefreshTime = LSRefreshTime;
   p_RTO->Cfg.MaxLsaRetxEntries = OSPF_MAX_RETX_ENTRIES;
   p_RTO->Cfg.SilenceInterval = SILENCE_INTERVAL_DFLT;
   p_RTO->Cfg.Holdtime = HOLDTIME_INTERVAL_DFLT;
   p_RTO->Cfg.HelperSupport = DEFAULT_HELPER_SUPPORT;
   p_RTO->Cfg.StrictLsaChecking = DEFAULT_STRICT_LSA_CHECKING;
   p_RTO->Cfg.NsfSupport = DEFAULT_NSF_SUPPORT;
   p_RTO->Cfg.RestartInterval = DEFAULT_RESTART_INTERVAL;

   if(CreateDefaultRTB)
   {
      t_RTB *p_RTB;

      e = RTB_Init(p_RTO, OSPF_PUBLIC_VPN_ID, (void *)&p_RTB);
   }

   p_RTO->ospfTraceFlags = 0;

   /* Create aging object */
   e |= AgingTbl_Init(p_RTO, p_RTO->OspfSysLabel.threadHndle, &p_RTO->AgingObj);

   /* Create OSPF tick timer */
   e |= TIMER_InitSec( 1, p_RTO, &p_RTO->TickTimer );

   if(e == E_OK)
   {
      /* pass object ID back to caller */
      *p_RTO_Id = (t_Handle) p_RTO;
   }
   else
      RTO_Delete((t_Handle) p_RTO);

   return e;
}

/*********************************************************************
 * @purpose    Given an AVLH of LSDB entries, remove each entry from the
 *             list and free its memory.
 *
 *
 * @param Id   avlh  @b{(input)}  AVLH containing LSAs to be cleared
 *
 * @returns    void
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void ClearLsaList(t_Handle avlh)
{
  e_Err   e;
  t_A_DbEntry *p_DbEntry = NULL;
  t_A_DbEntry *p_NextDbEntry = NULL;
  t_RTO *p_RTO = (t_RTO*) RTO_List;    /* easier than making a new argument */

  e = AVLH_GetFirst(avlh, (void *)&p_DbEntry);
   while (e == E_OK)
   {
     e = AVLH_GetNext(avlh, (void *)&p_NextDbEntry, p_DbEntry);
     LsaRelease(p_RTO, p_DbEntry, 1);   /* free the lsdb entry */
     p_DbEntry = p_NextDbEntry;
   }
}


/*********************************************************************
 * @purpose          Delete a RTO object.
 *
 *
 * @param Id         @b{(input)}  RTO object Id
 *
 * @returns          always E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_Delete( t_Handle Id)
{
   t_RTO   *p_RTO = (t_RTO *)Id;
   t_RTB   *p_RTB;
   e_Err   e;


   /* Print all VPN routing tables */
   if(p_RTO->TrapControl & GET_TRAP_BITMASK(RTB_ENTRY_INFO))
   {
      e = HL_GetFirst(p_RTO->RtbHl, (void *)&p_RTB);
      while(e == E_OK)
      {
         RtbPrint(p_RTO, p_RTB->VpnCos);

         e = HL_GetNext(p_RTO->RtbHl, (void *)&p_RTB, p_RTB);
      }
   }

   if(p_RTO->AgingObj)
      AgingTbl_Destroy(&p_RTO->AgingObj);

   TIMER_Delete(p_RTO->TickTimer);

   TIMER_Delete(p_RTO->OverflowTimer);
   TIMER_Delete(p_RTO->LsdbOverloadTimer);
   TIMER_Delete(p_RTO->gracePeriodTimer);

   HL_Cleanup(p_RTO->AsExtRoutesHl,1);
   HL_Destroy(&p_RTO->AsExtRoutesHl);

   ClearLsaList(p_RTO->AsExternalLsaHl);
   ClearLsaList(p_RTO->AsOpaqueLsaHl);

   AVLH_Destroy(&p_RTO->AsExternalLsaHl);
   AVLH_Destroy(&p_RTO->AsOpaqueLsaHl);

#if L7_OSPF_TE
   HL_Browse(p_RTO->FaIndexHl, TE_FaCleanup, 0);
   HL_Destroy(&p_RTO->FaIndexHl);
#endif

   /* delete LSA timer init */
   TIMER_Delete(p_RTO->DlyLsaTimer);
   /* Release delayed LSAs list */
   HL_Browse(p_RTO->DelayedLsa, DlyLsaDelete, 0);

   HL_Browse(p_RTO->AroHl, ARO_Delete, 0);
   HL_Destroy(&p_RTO->AroHl);

   HL_Browse(p_RTO->RtbHl, RTB_Delete, 0);
   HL_Destroy(&p_RTO->RtbHl);

   HL_Destroy(&p_RTO->IfoIndexHl);
   HL_Destroy(&p_RTO->DelayedLsa);
   

#if L7_OSPF_TE
   if(p_RTO->TeLsaInstBS)
      BS_Destroy(p_RTO->TeLsaInstBS);
#endif
   p_RTO->Taken = FALSE;
   if(p_RTO->RtbOwnThread)
   {
      t_XXCallInfo *callData;

      /* place request to change routing table in the thread's queue */
      PACKET_INIT(callData, RTO_Kill, 0, 0, 1, (ulng)p_RTO);
      if (XX_Call(p_RTO->OspfRtbThread.threadHndle, callData) != E_OK)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_OSPF_MAP_COMPONENT_ID,
               "Failed to queue RTO delete to own thread.");
      }
   }
   else
   {
      XX_DelFromDLList(p_RTO, RTO_List);
      XX_Free( p_RTO );
   }

   return E_OK;
}



/*********************************************************************
 * @purpose       RTO Kill command unpack function. Serves XX_Call
 *                requests to kill RTO
 *
 *
 * @param p_Info  @b{(input)}  pointer to XX_Call's t_XXCallInfo structure
 *
 * @returns       E_OK - always
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_Kill(void *p_Info)
{
   t_XXCallInfo *callInfo  = (t_XXCallInfo *) p_Info;
   t_RTO        *p_RTO     = (t_RTO *) PACKET_GET(callInfo,0);

   /* received command to kill the thread */
   XX_KillThread(p_RTO->OspfRtbThread.threadHndle);

   XX_DelFromDLList(p_RTO, RTO_List);
   XX_Free( p_RTO );

   return E_OK;
}





/*********************************************************************
 * @purpose      Clean up all router topology and routing databases.
 *
 *
 * @param  Id     @b{(input)}  RTO object Id
 *
 * @returns       always E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_Cleanup(t_Handle Id)
{
   t_RTO   *p_RTO = (t_RTO *)Id;
   t_RTB   *p_RTB;
   t_ARO   *p_ARO;
   t_IFO   *p_IFO;
   t_AckElement *p_Ack;
   t_AgrEntry *p_Entry;
   e_Err   e,er;
   t_RoutingTableEntry *p_Rte;
   t_VpnCos VpnCos = 0;
   p_RTO->IsOverflowed = FALSE;

   if (o2GracefulRestartInProgress(p_RTO))
   {
     o2GracefulRestartExit(p_RTO, OSPF_GR_MANUAL_CLEAR);
   }

   /* Cleanup all AROs */
   e = HL_GetFirst(p_RTO->AroHl,  (void**)&p_ARO);
   while(e == E_OK)
   {
      p_RTO->BundleLsas = TRUE;
      /* Flush all self-originated LSAs for clean all neighbors database */
      AVLH_Browse(p_RTO->AsExternalLsaHl, (BROWSEFUNC)FlushLsa, (ulng)p_ARO);
      AVLH_Browse(p_RTO->AsOpaqueLsaHl, (BROWSEFUNC)FlushLsa, (ulng)p_ARO);
      AVLH_Browse(p_ARO->RouterLsaHl, (BROWSEFUNC)FlushLsa, (ulng)p_ARO);
      AVLH_Browse(p_ARO->NetworkLsaHl, (BROWSEFUNC)FlushLsa, (ulng)p_ARO);
      AVLH_Browse(p_ARO->NetSummaryLsaHl, (BROWSEFUNC)FlushLsa, (ulng)p_ARO);
      AVLH_Browse(p_ARO->AsBoundSummaryLsaHl, (BROWSEFUNC)FlushLsa, (ulng)p_ARO);
      AVLH_Browse(p_ARO->NssaLsaHl, (BROWSEFUNC)FlushLsa, (ulng)p_ARO);
      AVLH_Browse(p_ARO->AreaOpaqueLsaHl, (BROWSEFUNC)FlushLsa, (ulng)p_ARO);

      if (p_RTO->BundleLsas)
        LsUpdatesSend(p_RTO);

      /* Release Area's LSA Database */

      ClearLsaList(p_ARO->RouterLsaHl);
      ClearLsaList(p_ARO->NetworkLsaHl);
      ClearLsaList(p_ARO->NetSummaryLsaHl);
      ClearLsaList(p_ARO->AsBoundSummaryLsaHl);
      ClearLsaList(p_ARO->NssaLsaHl);
      ClearLsaList(p_ARO->AreaOpaqueLsaHl);

      if(HL_FindFirst(p_RTO->RtbHl, (byte *)&VpnCos, (void *)&p_RTB) == E_OK)
      {
         /* Release Area's range database */
         er = HL_GetFirst(p_ARO->AdrRangeHl, (void *)&p_Entry);
         while(er == E_OK)
         {
            /* Remove reject routes of active address ranges from the rto */
            p_Rte = o2RouteFind(p_RTB, p_RTB->RtbNtBt, p_Entry->NetIpAdr, p_Entry->SubnetMask);
            if (p_Rte)
            {
               if(p_RTO->Clbk.p_RoutingTableChanged && (p_Rte->isRejectRoute == TRUE))
               {
                  p_RTO->Clbk.p_RoutingTableChanged(p_RTO->Clbk.f_RtmAsyncEvent[0] ? 
                              (t_Handle)p_RTO->Clbk.f_RtmAsyncEvent : p_RTO->MngId, 
                              p_Rte->DestinationType, RTE_REMOVED, 
                              p_Rte->DestinationId, p_Rte->IpMask, 
                              p_Rte->PathNum, p_Rte->Cost, p_Rte->PathPrm, p_Rte->PathType,
                              p_Rte->isRejectRoute);
                  o2RouteDelete(p_RTB, p_RTB->RtbNtBt, p_Rte->DestinationId, p_Rte->IpMask, TRUE);
               }
            }
            HL_Cleanup(p_Entry->AgrRteHl,0);
            er = HL_GetNext(p_ARO->AdrRangeHl, (void *)&p_Entry, p_Entry);
          }
      }

      /* stop LSA delay timer */
      TIMER_Stop(p_ARO->DlyLsaTimer);

      /* Reset counters */
      p_ARO->AsBdrRtrCount   = 0;
      p_ARO->AreaBdrRtrCount = 0;
      p_ARO->AreaIntRtrCount = 0;
      p_ARO->StubDefLSAOriginated = FALSE;

      /* Release delayed LSAs list */
      HL_Browse(p_ARO->DelayedLsa, DlyLsaDelete, 0);

      /* Cleanup all IFOs */
      er = HL_GetFirst(p_ARO->IfoHl,  (void**)&p_IFO);
      while(er == E_OK)
      {
         /* Close Hello session and kill all NBOs */
         IFO_Down(p_IFO);

         /* Cleanup delayed LSA Ack list */
         while((p_Ack = LL_Get(p_IFO->LsAck)) != NULL)
            XX_Free(p_Ack);

         TIMER_Stop( p_IFO->AckTimer );
         TIMER_Stop( p_IFO->WaitTimer );

         er = HL_GetNext(p_ARO->IfoHl,  (void**)&p_IFO, p_IFO);
      }

      e = HL_GetNext(p_RTO->AroHl,  (void**)&p_ARO, p_ARO);
   }

   /* Release delayed LSAs list */
   HL_Browse(p_RTO->DelayedLsa, DlyLsaDelete, 0);

   /* Cleanup all the AS External and AS Opaque LSAs  */
   ClearLsaList(p_RTO->AsExternalLsaHl);
   ClearLsaList(p_RTO->AsOpaqueLsaHl);

   /* Cleanup all RTBs */
   e = HL_GetFirst(p_RTO->RtbHl,  (void**)&p_RTB);
   while(e == E_OK)
   {
     /* Remove routes from RTO */
      o2RoutesBrowse(p_RTB, p_RTB->RtbRtBt, RteDelete, (ulng)p_RTB);
      o2RoutesBrowse(p_RTB, p_RTB->RtbNtBt, RteDelete, (ulng)p_RTB);

      /* Remove routes from OSPF routing tables */
      o2ClearRoutes(p_RTB, p_RTB->RtbRtBt);
      o2ClearRoutes(p_RTB, p_RTB->RtbNtBt);

      TIMER_Stop( p_RTB->RecalcTimer );

      /* Setup the first start-up silence interval */
      p_RTB->FirstCalc = TRUE;
      if(p_RTO->Cfg.StartUpSilenceInterval)
         p_RTB->CalcDelay = p_RTO->Cfg.StartUpSilenceInterval;
      else
         p_RTB->CalcDelay = ROUTER_DEAD_INTERVAL_DFLT;

      e = HL_GetNext(p_RTO->RtbHl,  (void**)&p_RTB, p_RTB);
   }

   /* Stop LSA aging */
   AgingTbl_StopAndCleanup(p_RTO->AgingObj);

   if(TIMER_Active(p_RTO->TickTimer))
     TIMER_Stop(p_RTO->TickTimer);

#if L7_OSPF_TE
   HL_Browse(p_RTO->FaIndexHl, TE_FaCleanup, 0);
#endif

   return E_OK;
}

/*********************************************************************
 * @purpose      Set a timer to recompute all routing tables.
 *
 * @param  Id     @b{(input)}  RTO object Id
 *
 * @returns       always E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_RecomputeAllRoutes(t_Handle Id)
{
    t_RTO *p_RTO = (t_RTO *)Id;
    t_RTB *p_RTB;
    e_Err e;

    e = HL_GetFirst(p_RTO->RtbHl,  (void**)&p_RTB);
    while (e == E_OK)
    {
        if(!TIMER_Active(p_RTB->RecalcTimer))
        {
            /* Start recalculation timer */
            RTB_ComputeCalcDelay(p_RTB, p_RTO);
            TIMER_StartSec(p_RTB->RecalcTimer, p_RTB->CalcDelay,
                           0, RecalcTimerExp, p_RTO->OspfRtbThread.threadHndle);
        }

        e = HL_GetNext(p_RTO->RtbHl,  (void**)&p_RTB, p_RTB);
    }
    return E_OK;
}

/*********************************************************************
 * @purpose      Sends notification to user about RTB entry removing
 *               and remove entry from BT.
 *
 *
 * @param RteId        @b{(input)}  RTB Entry handler
 * @param rtbId        @b{(input)}  RTB object Id
 *
 * @returns            always E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static Bool RteDelete(byte *RteId, ulng rtbId)
{
   t_RoutingTableEntry *p_Rte = (t_RoutingTableEntry*)RteId;
   t_RTB   *p_RTB = (t_RTB*) rtbId;
   t_RTO   *p_RTO = (t_RTO *)p_RTB->RTO_Id;
   ulng    rteCost = 0;

   if(p_RTO->Clbk.p_RoutingTableChanged)
   {
      rteCost = (((p_Rte->PathType == OSPF_TYPE_2_EXT) ||
                   (p_Rte->PathType == OSPF_NSSA_TYPE_2_EXT)) ?
                   p_Rte-> Type2Cost : p_Rte->Cost);

      p_RTO->Clbk.p_RoutingTableChanged(p_RTO->Clbk.f_RtmAsyncEvent[0] ?
       (t_Handle)p_RTO->Clbk.f_RtmAsyncEvent : p_RTO->MngId,
       p_Rte->DestinationType, RTE_REMOVED,
       p_Rte->DestinationId, p_Rte->IpMask, 
       p_Rte->PathNum, rteCost, p_Rte->PathPrm, p_Rte->PathType, 0);
   }

   return TRUE;
}



/*********************************************************************
 * @purpose             Set RTO's Management object id.
 *
 *
 * @param RTO_Id        @b{(input)}  RTO object Id
 * @param MngObjId      @b{(input)}  Management object id
 *
 * @returns             always E_OK       success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_SetMngId(t_Handle RTO_Id, t_Handle MngObjId)
{
   t_RTO   *p_RTO = (t_RTO *)RTO_Id;

   p_RTO->MngId = MngObjId;

   return E_OK;
}


#if L7_IPMS

/*********************************************************************
 * @purpose            Set RTO's IPMS object id.
 *
 *
 * @param RTO_Id        @b{(input)}  RTO object Id
 * @param IpmsObjId     @b{(input)}  Management object id
 * @param isStub        @b{(input)}  Stub or real object
 * @param               @b{(input)}
 *
 * @returns             always E_OK       success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_SetIpmsParams(t_Handle RTO_Id, t_Handle IpmsObjId, Bool isStub)
{
   t_RTO   *p_RTO = (t_RTO *)RTO_Id;

   p_RTO->ipmsHandle = IpmsObjId;
   p_RTO->isStub = isStub;

   return E_OK;
}
#endif



/*********************************************************************
 * @purpose       Router Object Config get and set, depend on RowStatus
 *                value.
 *
 *
 * @param RTO_Id      @b{(input)} TO object Id
 * @param p_Cfg       @b{(input)} RTO Configuration parameters structure
 *
 * @returns           E_OK       if success
 * @returns           E_FAILED   if failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
 e_Err RTO_Config(t_Handle RTO_Id, t_S_RouterCfg *p_Cfg)
{
   t_RTO   *p_RTO = (t_RTO *)RTO_Id;
   t_ARO   *p_ARO;
   t_IFO   *p_IFO;
   byte     id_state = 0;   /* 0 - not changed, 1 - switched on, 2 - switched off, 3 - changed */
   byte     asbr_state = 0; /* 0 - not changed, 1 - switched on, 2 - switched off */
   e_Err    e, er;
   byte     oldRtrId[4];
   Bool     prevOverflow;
   ulng     prevExtLsdbLimit;
   Bool     prevLsdbOverload = p_RTO->LsdbOverload;
   L7_uint32 prevStubRtrSummaryMetricOveride = p_RTO->Cfg.StubRtrSummaryMetricOveride;
   L7_uint32 prevStubRtrSummaryMetric = p_RTO->Cfg.StubRtrSummaryMetric;

   switch(p_Cfg->RouterStatus)
   {
      case  ROW_ACTIVE:

         /* If first time init, then run the router origination */
         if (!p_RTO->Cfg.AdminStat)
         {
           id_state = 1;
         }

         p_RTO->Cfg.AdminStat = TRUE;
         if (p_RTO->ospfTraceFlags & OSPF_TRACE_NBR_STATE)
         {
           RTO_TraceWrite("Enabling OSPF.");    
         }

         /* Start aging object */
         if(!AgingTbl_Running(p_RTO->AgingObj))
         {
           if(AgingTbl_Start(p_RTO->AgingObj, MaxAge, DEF_AGING_INTERVAL,
                             ospfAgingTick) != E_OK)
              return E_FAILED;
         }

         /* start the OSPF tick timer */
         if(!TIMER_Active(p_RTO->TickTimer))
           TIMER_StartSec(p_RTO->TickTimer, DEF_OSPF_TICK_INTERVAL, 1,
                          OspfTickTimerExp, p_RTO->OspfSysLabel.threadHndle );


      break;
      case  ROW_NOT_IN_SERVICE:
         if (p_RTO->Cfg.AdminStat)
         {
           if (p_RTO->ospfTraceFlags & OSPF_TRACE_NBR_STATE)
           {
             RTO_TraceWrite("Disabling OSPF.");    
           }

           RTO_Cleanup(p_RTO);

           p_RTO->Cfg.AdminStat = FALSE;
           if (p_RTO->Cfg.StubRtrMode != OSPF_STUB_RTR_CFG_ALWAYS)
           {
             p_RTO->LsdbOverload = FALSE;    
           }
         }
         return E_OK;

      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:
      case  ROW_CHANGE:

         if ((p_RTO->Cfg.RFC1583Compatibility != p_Cfg->RFC1583Compatibility) ||
             (p_RTO->Cfg.MaximumPaths != p_Cfg->MaximumPaths))
            id_state = 4;

         A_SET_4B(p_RTO->Cfg.RouterId, oldRtrId);
         if(p_RTO->Cfg.RouterId != p_Cfg->RouterId)
            id_state = 3;

         if(!p_RTO->Cfg.ASBdrRtrStatus && p_Cfg->ASBdrRtrStatus)
            asbr_state = 1;
         else if(p_RTO->Cfg.ASBdrRtrStatus && !p_Cfg->ASBdrRtrStatus)
            asbr_state = 2;

         prevOverflow = p_RTO->IsOverflowed;
         prevExtLsdbLimit = p_RTO->Cfg.ExtLsdbLimit;

         memcpy(&p_RTO->Cfg, p_Cfg, sizeof(t_S_RouterCfg));

          if (p_RTO->Cfg.StubRtrMode == OSPF_STUB_RTR_CFG_ALWAYS)
          {
            EnterLsdbOverload(p_RTO, OSPF_STUB_RTR_CONFIGURED);
          }
          else if (p_RTO->Cfg.StubRtrMode == OSPF_STUB_RTR_CFG_STARTUP)
          {
            if ((osapiUpTimeRaw() - p_RTO->p3InitTime) < p_RTO->Cfg.StubRtrStartupTime)
            {
              EnterLsdbOverload(p_RTO, OSPF_STUB_RTR_STARTUP);
            }
            else
            {
              o2StubRouterModeExit(p_RTO);
            }
          }
          else if (p_RTO->Cfg.StubRtrMode == OSPF_STUB_RTR_CFG_DISABLED)
          {
            if (p_RTO->LsdbOverload &&
                (p_RTO->LsdbOverloadReason != OSPF_STUB_ROUTER_RESOURCE))
            {
              o2StubRouterModeExit(p_RTO);
            }
          }

          /* See if we have to update the metric of summary LSAs in stub router mode. */
          if (p_RTO->LsdbOverload && prevLsdbOverload)
          {
            /* OSPF was in stub router mode before this config change and is 
             * still there. (If OSPF entered stub router mode as a result of this
             * config change, then it reoriginated summary LSAs with the correct metric
             * at that time.) */
            if ((prevStubRtrSummaryMetricOveride != p_RTO->Cfg.StubRtrSummaryMetricOveride) ||
                (prevStubRtrSummaryMetric != p_RTO->Cfg.StubRtrSummaryMetric))
            {
              /* Metric update needed */
              e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
              while (e == E_OK)
              {
                LsaOrgSumBulk(p_ARO);
                e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
              }
            }
          }

         /* If the user has issued a command to decrease the size of the Ext lsdb
         ** check if this has pushed the router into overflow state
         ** Note: If the user is trying to increase the LsdbLimit we don't need to
         ** do anything at this point, the router will automatically come out
         ** of overflow when the OverflowTimer expires.
         */
         if(prevExtLsdbLimit > p_Cfg->ExtLsdbLimit)
         {
           if ((p_RTO->NonDefExtLsaCount >= (ulng)p_RTO->Cfg.ExtLsdbLimit) &&
               (prevOverflow == FALSE))
             RTO_HandleOverflow(p_RTO);
         }

         if(p_Cfg->RouterStatus == ROW_CREATE_AND_WAIT)
            p_RTO->Cfg.AdminStat = FALSE;
         else if(p_Cfg->RouterStatus == ROW_CREATE_AND_GO)
         {
            if(!p_RTO->Cfg.AdminStat)
            {
              id_state = 1;
            }

            p_RTO->Cfg.AdminStat = TRUE;
         }
      break;
      case  ROW_READ:

         memcpy(p_Cfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
         p_Cfg->AreaBdrRtrStatus = routerIsBorder(p_RTO) ? TRUE : FALSE;
         p_Cfg->VersionNumber = OSPF_VERSION;
         p_Cfg->RouterStatus = ROW_ACTIVE;

      break;
      case  ROW_DESTROY:
         RTO_Cleanup(p_RTO);
         RTO_Delete(p_RTO);
      break;
      default:
         return E_FAILED;
   }

   if ((p_RTO->Cfg.AdminStat == TRUE) && (id_state == 1))
   {
      /* OverFlow State will be left when the OSPF has been started */
      p_RTO->IsOverflowed = FALSE;

      e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
      while(e == E_OK)
      {
         LsaReOriginate[S_ROUTER_LSA](p_ARO, 0);

#if L7_OSPF_TE
         {
            t_TeRtr *p_TeRtr;

            /* Flush old Router LSA from TE database */
            if(AVLH_Find(p_ARO->TeRtrHl, oldRtrId,  (void**)&p_TeRtr,0) == E_OK)
            {
               A_SET_2B(MaxAge, p_TeRtr->p_DbEntry->Lsa.LsAge);
               LSA_FLOODING(p_RTO, p_ARO, NULL, p_TeRtr->p_DbEntry, NULL);
               LsaAgedSendTrap(p_RTO, p_ARO, p_TeRtr->p_DbEntry);
            }
         }
         /* Originate new TE Router LSA */
         TE_RouterInfoUpdate((t_Handle)p_RTO, (t_Handle)p_ARO);
#endif
         e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
      }
   }
   
   else if(id_state == 3)
   {
      RTO_Cleanup(p_RTO);

      {
        Bool operStat = FALSE;

        /* Bringing Up all IFOs */
        er = HL_GetFirst(p_RTO->IfoIndexHl,  (void**)&p_IFO);
        while(er == E_OK)
        {
          if (p_IFO->Cfg.Type != IFO_VRTL)
          {
            /* Check the status of the lower layer interface */
            if(p_RTO->Clbk.f_GetLLOperStatus)
              p_RTO->Clbk.f_GetLLOperStatus(p_RTO->MngId, p_IFO->Cfg.IfIndex,
                                            &operStat);
            /* If the lower layer interface is active bring up this IFO */
            if(operStat == TRUE)
              IFO_Up(p_IFO);
          }

          er = HL_GetNext(p_RTO->IfoIndexHl,  (void**)&p_IFO, p_IFO);
          operStat = FALSE;
        }
      }
   } else if (id_state == 4)
       RTO_RecomputeAllRoutes(p_RTO);

   /* If ASBR flag changed, need to reoriginate the router LSA
    * with E bit set. */
   if((id_state == 0) && ((asbr_state == 1) || (asbr_state == 2)))
   {
     e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
     while(e == E_OK)
     {
       LsaReOriginate[S_ROUTER_LSA](p_ARO, 0);
       e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
     }
   }
   return E_OK;
}
/*********************************************************************
 * @purpose       Get the ChecksumSum of Type-11 LSAs.
 *
 * @param RTO_Id      @b{(input)} TO object Id
 * @param opaqueCksumSum  @b{(input)} OpaqueCheckSum
 *
 * @returns           E_OK       if success
 * @returns           E_FAILED   if failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_ASOpaqueCksumSum_Get(t_Handle RTO_Id, L7_uint32 *opaqueCksumSum)
{
  t_RTO   *p_RTO = (t_RTO *)RTO_Id;

  *opaqueCksumSum = p_RTO->OpaqueASLsaCksumSum;

  return E_OK;
}

/*********************************************************************
 * @purpose    Indicates whether OSPF is in stub router advertisement mode.
 *
 * @param RTO_Id      @b{(input)} TO object Id
 * @param isStubRtr   @b{(input)} TRUE if router is in stub router adv mode
 *
 * @returns           E_OK       if success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_IsStubRtr_Get(t_Handle RTO_Id, L7_BOOL *isStubRtr)
{
  t_RTO   *p_RTO = (t_RTO *)RTO_Id;
  *isStubRtr = (L7_BOOL) p_RTO->LsdbOverload;
  return E_OK;
}

/*********************************************************************
 * @purpose    Indicates whether OSPF is in stub router advertisement mode.
 *
 * @param RTO_Id      @b{(input)} TO object Id
 * @param extLsdbOverflow   @b{(input)} TRUE if external LSDB is over limit
 *
 * @returns           E_OK       if success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_ExtLsdbOverflow_Get(t_Handle RTO_Id, L7_BOOL *extLsdbOverflow)
{
  t_RTO   *p_RTO = (t_RTO *)RTO_Id;
  *extLsdbOverflow = (L7_BOOL) p_RTO->IsOverflowed;
  return E_OK;
}

/*********************************************************************
 * @purpose       Router Object Config get and set
 *
 * @param RTO_Id      @b{(input)} TO object Id
 * @param opaqueEnabled  @b{(input)} Opaque Support
 *
 * @returns           E_OK       if success
 * @returns           E_FAILED   if failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_OpaqueCapability_Get(t_Handle RTO_Id, L7_uint32 *opaqueEnabled)
{
  t_RTO   *p_RTO = (t_RTO *)RTO_Id;

  *opaqueEnabled = p_RTO->Cfg.OpaqueCapability;

  return E_OK;
}

/*********************************************************************
 * @purpose       Router Object Config get and set
 *
 * @param RTO_Id      @b{(input)} TO object Id
 * @param opaqueEnabled  @b{(input)} TRUE if opaque LSAs supported
 *
 * @returns           E_OK       if success
 * @returns           E_FAILED   if failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_OpaqueCapability_Set(t_Handle RTO_Id, Bool opaqueEnabled)
{
  t_RTO   *p_RTO = (t_RTO *)RTO_Id;

  p_RTO->Cfg.OpaqueCapability = opaqueEnabled;

  return E_OK;
}

/*********************************************************************
 * @purpose       In the router's external routes hash list, delete the
 *                route from the hash list and reoriginate the
 *                corresponding LSA with max age
 *
 *
 * @param RTO_Id  @b{(input)}  RTO object Id
 *
 * @returns       always E_OK       success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_PurgeExternalLsas(t_Handle RTO_Id)
{
    t_RTO *p_RTO = (t_RTO *)RTO_Id;
    t_S_AsExternalCfg *p_Entry;
    t_S_AsExternalCfg *p_NextEntry;
    e_Err eIter;

    eIter = HL_GetFirst(p_RTO->AsExtRoutesHl, (void *) &p_Entry);
    while (eIter == E_OK) {
        eErr e;

        eIter = HL_GetNext(p_RTO->AsExtRoutesHl, (void*) &p_NextEntry, p_Entry);
        e = HL_DeleteFirst(p_RTO->AsExtRoutesHl,
                           (byte*) &p_Entry->DestNetIpAdr,
                           (void *)&p_Entry);

        if (e == E_OK) {
            /* Reoriginate the LSA with max age */
            p_Entry->PrevEntry = (t_Handle)RTE_REMOVED;
            LsaReOriginate[S_AS_EXTERNAL_LSA](p_RTO, (ulng)p_Entry);

            /* free the AS external route. Note that this does not
             * touch the actual LSA, which will eventually be deleted
             * as a result of aging out.
             */
            XX_Free(p_Entry);
        }
        p_Entry = p_NextEntry;
    }
    return E_OK;
}





/*********************************************************************
 * @purpose             OSPF SW callbacks setup.
 *
 *
 * @param RTO_Id        @b{(input)}  RTO object Id
 * @param p_CallBacks   @b{(input)}  pointer to the callbacks structure
 *
 * @returns             always E_OK       success
 *
 * @notes    Callbacks amy be reassigned on run-time.The only defined
 *           in p_CallBacks parameter callbacks will be replaced,
 *           rest callbacks will be leaved without change
 *
 * @end
 * ********************************************************************/
e_Err RTO_SetCallbacks(t_Handle RTO_Id, t_S_Callbacks *p_CallBacks)
{
   t_RTO   *p_RTO = (t_RTO *)RTO_Id;

   if(p_CallBacks->f_AreaCreateInd)
      p_RTO->Clbk.f_AreaCreateInd = p_CallBacks->f_AreaCreateInd;
   if(p_CallBacks->f_NeighborCreateInd)
      p_RTO->Clbk.f_NeighborCreateInd = p_CallBacks->f_NeighborCreateInd;
   if (p_CallBacks->f_NeighborFullInd)
     p_RTO->Clbk.f_NeighborFullInd = p_CallBacks->f_NeighborFullInd;
   if (p_CallBacks->f_DrElected)
     p_RTO->Clbk.f_DrElected = p_CallBacks->f_DrElected;
   if(p_CallBacks->f_IpForward)
      p_RTO->Clbk.f_IpForward = p_CallBacks->f_IpForward;
   if(p_CallBacks->p_OspfTrap)
      p_RTO->Clbk.p_OspfTrap = p_CallBacks->p_OspfTrap;
   if(p_CallBacks->p_RoutingTableChanged)
      p_RTO->Clbk.p_RoutingTableChanged = p_CallBacks->p_RoutingTableChanged;
   if (p_CallBacks->f_InitialRtoUpdateDone)
     p_RTO->Clbk.f_InitialRtoUpdateDone = p_CallBacks->f_InitialRtoUpdateDone;
   if(p_CallBacks->p_RemoteVpnRtrInd)
      p_RTO->Clbk.p_RemoteVpnRtrInd = p_CallBacks->p_RemoteVpnRtrInd;
   if(p_CallBacks->f_OpaqueReceive)
      p_RTO->Clbk.f_OpaqueReceive = p_CallBacks->f_OpaqueReceive;

   if(p_CallBacks->f_RtmAsyncEvent[0])
   {
      p_RTO->Clbk.f_RtmAsyncEvent[0] = p_CallBacks->f_RtmAsyncEvent[0];
      p_RTO->Clbk.f_RtmAsyncEvent[1] = p_CallBacks->f_RtmAsyncEvent[1];
   }

   if(p_CallBacks->f_GetLLOperStatus)
     p_RTO->Clbk.f_GetLLOperStatus = p_CallBacks->f_GetLLOperStatus;

   return E_OK;
}





/*********************************************************************
 * @purpose             OSPF SW trap enable/disable bit mask setup.
 *
 *
 * @param RTO_Id        @b{(input)}  RTO object Id
 * @param TrapControl   @b{(input)}  trap enable/disable bitmask
 *
 * @returns             always E_OK       success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_TrapControl(t_Handle RTO_Id, t_S_TrapControl TrapControl)
{
   t_RTO   *p_RTO = (t_RTO *)RTO_Id;

   p_RTO->TrapControl = TrapControl;

   return E_OK;
}


/*********************************************************************
 * @purpose             Router's AS-External LSA origination
 *                      (only for AS-boundary routers).
 *
 *
 * @param RTO_Id        @b{(input)}  RTO object Id
 * @param p_AsExtCfg    @b{(input)}  S-External LSA origination structure
 *
 * @returns             E_OK       success
 * @returns             E_FAILED   failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
 e_Err RTO_AsExternalCfg(t_Handle RTO_Id, t_S_AsExternalCfg *p_AsExtCfg)
{
   t_RTO *p_RTO = (t_RTO *) RTO_Id;
   t_S_AsExternalCfg *p_Entry = L7_NULLPTR;
   e_Err e = E_OK;

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_REDIST)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    L7_uchar8 *status;
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 nextHopStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_AsExtCfg->DestNetIpAdr, destStr);
    osapiInetNtoa(p_AsExtCfg->DestNetIpMask, maskStr);
    osapiInetNtoa(p_AsExtCfg->Nexthop, nextHopStr);
    switch(p_AsExtCfg->AsExtStatus)
    {
      case ROW_ACTIVE: status = "ROW_ACTIVE"; break;
      case ROW_NOT_IN_SERVICE: status = "ROW_NOT_IN_SERVICE"; break;
      case ROW_NOT_READY: status = "ROW_NOT_READY"; break;
      case ROW_CREATE_AND_GO: status = "ROW_CREATE_AND_GO"; break;
      case ROW_CREATE_AND_WAIT: status = "ROW_CREATE_AND_WAIT"; break;
      case ROW_DESTROY: status = "ROW_DESTROY"; break;
      case ROW_READ: status = "ROW_READ"; break;
      case ROW_READ_NEXT: status = "ROW_READ_NEXT"; break;
      case ROW_CHANGE: status = "ROW_CHANGE"; break;
      case ROW_MODIFY: status = "ROW_MODIFY"; break;
      default: status = "OTHER"; 
    
    }
    osapiSnprintf(traceBuf, OSPF_MAX_TRACE_STR_LEN,
            "Redistributing %s/%s with type %d metric %lu via next hop %s. Status: %s",
            destStr, maskStr, (p_AsExtCfg->IsMetricType2 ? 2 : 1), 
            p_AsExtCfg->MetricValue, nextHopStr, status);
    RTO_TraceWrite(traceBuf);    
  } 
  if (!p_RTO->Cfg.AdminStat)
  {
    if (p_RTO->ospfTraceFlags & OSPF_TRACE_REDIST)
    {
      char traceBuf[OSPF_MAX_TRACE_STR_LEN];
      sprintf(traceBuf, 
              "RTO disabled globally. Ignoring external route.");
      RTO_TraceWrite(traceBuf);    
    }
    return E_FAILED;
  }

   /* try to find exist entry */
   if((p_AsExtCfg->AsExtStatus != ROW_READ_NEXT) &&
      (p_AsExtCfg->AsExtStatus != ROW_ACTIVE))
      e = HL_FindFirst(p_RTO->AsExtRoutesHl, (byte*) &p_AsExtCfg->DestNetIpAdr,
                       (void *)&p_Entry);

   switch(p_AsExtCfg->AsExtStatus)
   {
      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:

         if(e == E_OK) /* if already exist */
         {
          if (p_RTO->ospfTraceFlags & OSPF_TRACE_REDIST)
          {
            char traceBuf[OSPF_MAX_TRACE_STR_LEN];
            sprintf(traceBuf, 
                    "Error. Already redistributing a route to this destination.");
            RTO_TraceWrite(traceBuf);    
          }
          return E_FAILED;
         }

         if((p_Entry = XX_Malloc(sizeof(t_S_AsExternalCfg))) == NULL)
         {
           L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
           L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
           osapiInetNtoa(p_AsExtCfg->DestNetIpAdr, destStr);
           osapiInetNtoa(p_AsExtCfg->DestNetIpMask, maskStr);
           L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
                   "Memory allocation failure prevented origination of external LSA for %s/%s.",
                   destStr, maskStr);
           return E_NOMEMORY;
         }

         RTO_CalcForwardingAddr(p_RTO, &p_AsExtCfg->Nexthop, 
                                &p_AsExtCfg->ForwardingAdr);
         if (p_RTO->ospfTraceFlags & OSPF_TRACE_REDIST)
         {
           char traceBuf[OSPF_MAX_TRACE_STR_LEN];
           L7_uchar8 fwdAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
           osapiInetNtoa(p_AsExtCfg->ForwardingAdr, fwdAddrStr);
           osapiSnprintf(traceBuf, OSPF_MAX_TRACE_STR_LEN, 
                   "Selected forwarding address %s.", fwdAddrStr);
           RTO_TraceWrite(traceBuf);    
         }

         memcpy(&p_Entry->DestNetIpAdr,
                &p_AsExtCfg->DestNetIpAdr,
                sizeof(t_S_AsExternalCfg) - 2*sizeof(ulng));

         e = HL_Insert(p_RTO->AsExtRoutesHl, (byte*) &p_AsExtCfg->DestNetIpAdr, p_Entry);

         if ((e != E_OK) && (p_RTO->ospfTraceFlags & OSPF_TRACE_REDIST))
         {
           char traceBuf[OSPF_MAX_TRACE_STR_LEN];
           sprintf(traceBuf, 
                   "Error. Failed to insert route into external routes hash list.");
           RTO_TraceWrite(traceBuf);    
         }

         /* If some change occured, then reoriginate the LSA */
         /* sanity check. */
         if (p_RTO->Cfg.ASBdrRtrStatus != TRUE) {
             /* log error */
         }

         /* Originate LSAs (note that the LsId is not known at this point. This is 
          * assigned during origination after appendix E processing
          */
         p_Entry->PrevEntry = (t_Handle)RTE_ADDED;
         if((e == E_OK) && (p_AsExtCfg->AsExtStatus != ROW_CREATE_AND_WAIT))
         {
            if (LsaReOriginate[S_AS_EXTERNAL_LSA](p_RTO, (ulng)p_Entry) != E_OK)
            {
              /* If route not installed in database, remove from external routes list. */
              if (HL_DeleteFirst(p_RTO->AsExtRoutesHl,
                             (byte*) &p_AsExtCfg->DestNetIpAdr, (void *)&p_Entry) == E_OK)
              {
                XX_Free(p_Entry);
              }
              return E_FAILED;
            }

            /* Originate T7 LSA into all attached NSSAs, unless a default route.
             * The default is originated via NssaDefaultLsaOriginate(). */
            if ((p_Entry->DestNetIpAdr != 0) && (p_Entry->DestNetIpMask != 0))
            {
              LsaOrgNssaBulk(RTO_Id, p_Entry);
            }
         }


      break;
      case  ROW_CHANGE:
         if (e == E_OK)
         {
            if (p_RTO->Cfg.ASBdrRtrStatus == FALSE) {
                /* log error */
            }

            RTO_CalcForwardingAddr(p_RTO, &p_AsExtCfg->Nexthop, 
                                   &p_AsExtCfg->ForwardingAdr);

            if (p_RTO->ospfTraceFlags & OSPF_TRACE_REDIST)
            {
              char traceBuf[OSPF_MAX_TRACE_STR_LEN];
              L7_uchar8 fwdAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
              osapiInetNtoa(p_AsExtCfg->ForwardingAdr, fwdAddrStr);
              osapiSnprintf(traceBuf,OSPF_MAX_TRACE_STR_LEN, 
                      "Selected forwarding address %s.", fwdAddrStr);
              RTO_TraceWrite(traceBuf);    
            }

            memcpy(&p_Entry->DestNetIpAdr, &p_AsExtCfg->DestNetIpAdr,
                   sizeof(t_S_AsExternalCfg) - 2*sizeof(ulng));

            p_Entry->PrevEntry = (t_Handle)RTE_ADDED;
            LsaReOriginate[S_AS_EXTERNAL_LSA](p_RTO, (ulng)p_Entry);

            /* Originate T7 LSA into all attached NSSAs, unless a default route.
             * The default is originated via NssaDefaultLsaOriginate(). */
            if ((p_Entry->DestNetIpAdr != 0) && (p_Entry->DestNetIpMask != 0))
            {
              LsaOrgNssaBulk(RTO_Id, p_Entry);
            }
         }
      break;
      case  ROW_READ:
         if(e == E_OK)
            memcpy(&p_AsExtCfg->DestNetIpAdr,
                   &p_Entry->DestNetIpAdr,
                   sizeof(t_S_AsExternalCfg) - 2*sizeof(ulng));

      break;
      case  ROW_READ_NEXT:
         if(p_AsExtCfg->PrevEntry == NULL)
            e = HL_GetFirst(p_RTO->AsExtRoutesHl, (void *)&p_AsExtCfg);
         else
            e = HL_GetNext(p_RTO->AsExtRoutesHl, (void *)&p_AsExtCfg, p_AsExtCfg->PrevEntry);

         if(e == E_OK)
         {
           p_AsExtCfg->PrevEntry = p_AsExtCfg;
         }
      break;
      case  ROW_DESTROY:
         /* We could be purging an external LSA in response to redistribution
          * being disabled. In this case, the router is no longer considered
          * an ASBR, but we still want to allow LSA to be sent. So don't
          * check p_RTO->Cfg.ASBdrRtrStatus.
          */
         e = HL_DeleteFirst(p_RTO->AsExtRoutesHl,
                           (byte*) &p_AsExtCfg->DestNetIpAdr, (void *)&p_Entry);

         if(e == E_OK)
         {
            /* If some change occured, then reoriginate the LSA */
            p_Entry->PrevEntry = (t_Handle)RTE_REMOVED;
            LsaReOriginate[S_AS_EXTERNAL_LSA](p_RTO, (ulng)p_Entry);

            /* Originate T7 LSA into all attached NSSAs, unless a default route.
             * The default is originated via NssaDefaultLsaOriginate(). */
            if ((p_Entry->DestNetIpAdr != 0) && (p_Entry->DestNetIpMask != 0))
            {
              LsaOrgNssaBulk(RTO_Id, p_Entry);
            }
            XX_Free(p_Entry);
         }
         else
         {
           if (p_RTO->ospfTraceFlags & OSPF_TRACE_REDIST)
           {
              char traceBuf[OSPF_MAX_TRACE_STR_LEN];
              sprintf(traceBuf,
                      "Error. Failed to remove route from external routes hash list.");
              RTO_TraceWrite(traceBuf);
           }
         }
      break;
      case ROW_ACTIVE:
         e = HL_GetFirst(p_RTO->AsExtRoutesHl, (void *)&p_Entry);
         while(e == E_OK)
         {
            if(!p_AsExtCfg->DestNetIpAdr || (p_AsExtCfg->DestNetIpAdr == p_Entry->DestNetIpAdr))
            {
               p_Entry->PrevEntry = (t_Handle)RTE_ADDED;
               LsaReOriginate[S_AS_EXTERNAL_LSA](p_RTO, (ulng)p_Entry);

              /* Originate T7 LSA into all attached NSSAs, unless a default route.
               * The default is originated via NssaDefaultLsaOriginate(). */
              if ((p_Entry->DestNetIpAdr != 0) && (p_Entry->DestNetIpMask != 0))
              {
                 LsaOrgNssaBulk(RTO_Id, p_Entry);
              }
            }
            e = HL_GetNext(p_RTO->AsExtRoutesHl, (void *)&p_Entry, p_Entry);
         }
         e = E_OK;
      break;
      default:
         return E_FAILED;
   }

   return e;
}






/*********************************************************************
 * @purpose      This function is used by an NSSA ASBR to originate NSSA
 *               LSAs for all attached NSSAs when a new AS external route
 *               is learned.
 *
 *
 * @param Id      @b{(input)}  Router Object handler
 *
 * @returns       E_OK - if originated successfull,
 * @returns       E_FAILED - if failed in LSA allocation
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void LsaOrgNssaBulk(t_Handle Id, t_S_AsExternalCfg *p_Entry)
{
  t_RTO *p_RTO = (t_RTO *)Id;
  t_ARO *p_ARO;
  e_Err err;

  err = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
  while(err == E_OK)
  {
    /* check if NSSA */
    if((p_ARO->ExternalRoutingCapability == AREA_IMPORT_NSSA) &&
       (p_ARO->NSSARedistribute == TRUE))
      LsaReOriginate[S_NSSA_LSA](p_ARO, (ulng)p_Entry);

    err = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
  }
}

/*--------------------------------------------------------------------
 * ROUTINE:    RTO_HandleOverflow
 *
 * DESCRIPTION:
 *    When the external lsdb limit is exceeded the router enters
 *    overflow state.  This function handles the overflow state and
 *    performs the following:
 *        1. Flush previously originated AS External LSAs
 *        2. Start the overflow timer if relevant
 *        3. Remove AS External & NSSA LSAs from the neighbors'
 *           LsRequest list(s)
 *        4. Send appropriate traps
 *    all attached NSSAs when a new AS external route is learned.
 *
 * ARGUMENTS:
 *    Id - Router Object handler
 *
 * RESULTS:
 *   E_OK - if originated successfull,
 *   E_FAILED - if failed in LSA allocation
 *--------------------------------------------------------------------*/
e_Err RTO_HandleOverflow(t_RTO *p_RTO)
{
  t_LsdbOverflow Overflow;
  t_ARO          *p_ARO;
  e_Err          e, er, err;
  t_IFO          *p_IFO;
  t_NBO          *p_NBO;

  if(!p_RTO)
    return E_FAILED;

  Overflow.RouterId = p_RTO->Cfg.RouterId;
  Overflow.ExtLsdbLimit = p_RTO->Cfg.ExtLsdbLimit;

  p_RTO->IsOverflowed = TRUE;

  /* Find the first area */
  if ((err = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO)) != E_OK)
    return E_FAILED;

  /* Start overflow state exit timer */
  if(p_RTO->Cfg.ExitOverflowInterval)
    TIMER_StartSec(p_RTO->OverflowTimer, p_RTO->Cfg.ExitOverflowInterval, 0,
                   OverflowTimerExp, p_RTO->OspfSysLabel.threadHndle );

  /* Flush all self-originated non-default AS-Ext LSAs */
  AVLH_Browse(p_RTO->AsExternalLsaHl, (BROWSEFUNC)FlushAsLsa, (ulng)p_RTO);

  /* Flush all corresponding self-originated Type-7 LSAs */
  while(err == E_OK)
  {
   /* check if NSSA */
   if((p_ARO->ExternalRoutingCapability == AREA_IMPORT_NSSA) &&
      (p_ARO->NSSARedistribute == TRUE))
     AVLH_Browse(p_ARO->NssaLsaHl, (BROWSEFUNC)FlushLsa, (ulng)p_ARO);

   /* Delete all AS-Ext LSAs from NBO Request lists */
   /* Examine all attached to this Area links (IFOs) */
   e = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
   while(e == E_OK)
   {
     /* Examine all attached to this IFO neighbors */
     er = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
     while(er == E_OK)
     {
        HL_Browse(p_NBO->LsReq, AsExtAndNssaRqFree, 0);

        er = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
     }
     e = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
   }
   err = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
  }

  if(p_RTO->Clbk.p_OspfTrap)
  {
     if(p_RTO->IsOverflowed &&
       (p_RTO->TrapControl & GET_TRAP_BITMASK(LS_DB_OVERFLOW)))
        p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, LS_DB_OVERFLOW, (u_OspfTrap *)&Overflow);

     if(!p_RTO->IsOverflowed &&
       (p_RTO->TrapControl & GET_TRAP_BITMASK(LS_DB_APPROACHING_OVERFLOW)))
        p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, LS_DB_APPROACHING_OVERFLOW, (u_OspfTrap *)&Overflow);
  }

  return E_OK;
}



/*********************************************************************
 * @purpose          Show OSPF Topology Database and Routing table sizes.
 *                   Show Routing table computation time statistics.
 *
 * @returns          E_OK 
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_ShowStatistics(void)
{
   t_RTO   *p_RTO = (t_RTO *) RTO_List;
   t_RTB   *p_RTB;
   t_ARO   *p_ARO;
   t_IFO   *p_IFO;
   e_Err   e, er;
   ulng    num[10];
   Bool    flag = FALSE;
   ulng    RtbSize;
   t_RoutingTableEntry *p_Rte;
   ulng    numLinkOpaque = 0;

   /* Get number of LSAs per type */
   AVLH_GetEntriesNmb(p_RTO->AsExternalLsaHl, &num[0]);     /* AS-external LSAs list */
   AVLH_GetEntriesNmb(p_RTO->AsOpaqueLsaHl, &num[1]);       /* AS-Opaque-LSAs */

  /* Browse all AROs */
   e = HL_GetFirst(p_RTO->AroHl,  (void**)&p_ARO);
   while(e == E_OK)
   {
      /* Get number of LSAs per type */

      AVLH_GetEntriesNmb(p_ARO->RouterLsaHl, &num[2]);         /* Router LSAs DB list */
      AVLH_GetEntriesNmb(p_ARO->NetworkLsaHl, &num[3]);        /* Network LSAs DB list */
      AVLH_GetEntriesNmb(p_ARO->NetSummaryLsaHl, &num[4]);     /* Network Summary LSAs DB list */
      AVLH_GetEntriesNmb(p_ARO->AsBoundSummaryLsaHl, &num[5]); /* AS boundary routers Summary LSAs list */
      num[6] = 0;                                              /* group-membership-LSAs */  
      AVLH_GetEntriesNmb(p_ARO->AreaOpaqueLsaHl, &num[8]);     /* Local-Area-Opaque-LSAs */

      num[7] = 0;
      e = HL_GetFirst(p_ARO->IfoHl, (void**) &p_IFO);
      while (e == E_OK)
      {
        AVLH_GetEntriesNmb(p_IFO->LinkOpaqueLsaHl, &numLinkOpaque);     /* link opaque LSAs */
        num[7] += numLinkOpaque;
        e = HL_GetNext(p_ARO->IfoHl, (void**) &p_IFO, (void*) p_IFO);
      }

      if(!flag)
      {
         flag = TRUE;
         osapiSnprintf(text,200,"OSPF: Area: %d, Database Size:%ld, Lsa Num: AS-Ext %d, AS Opaque %d",
                       (int)p_ARO->AreaId, p_ARO->DataBaseSize, (int)num[0], (int)num[1]);
      }
      else
      {
         sprintf(text,"   Area: %d, Database Size:%ld", (int)p_ARO->AreaId, p_ARO->DataBaseSize);
      }
      XX_Print(text);

      sprintf(text, "   Router %d, Network %d, NetSummary %d, ASBR Summary %d, LSAs Num: Link Opaque %d, Area Opaque %d",
               (int)num[2], (int)num[3], (int)num[4],(int) num[5], (int)num[7], (int)num[8]);
      XX_Print(text);

      e = HL_GetNext(p_RTO->AroHl,  (void**)&p_ARO, p_ARO);
   }

   /* Browse all RTBs */
   e = HL_GetFirst(p_RTO->RtbHl,  (void**)&p_RTB);
   while(e == E_OK)
   {
      RtbSize = 0;
      for (er = o2RouteGetFirst(p_RTB, p_RTB->RtbRtBt, &p_Rte); 
            er == E_OK; 
            er = o2RouteGetNext(p_RTB, p_RTB->RtbRtBt, &p_Rte))
         RtbSize += sizeof(t_RoutingTableEntry) + p_Rte->PathNum*sizeof(t_OspfPathPrm) +
         p_Rte->ErNum*sizeof(t_ErPrm);

      for (er = o2RouteGetFirst(p_RTB, p_RTB->RtbNtBt, &p_Rte); 
           er == E_OK; 
           er = o2RouteGetNext(p_RTB, p_RTB->RtbNtBt, &p_Rte))
      {
        RtbSize += sizeof(t_RoutingTableEntry) + 
                   p_Rte->PathNum * sizeof(t_OspfPathPrm) +
                   p_Rte->ErNum * sizeof(t_ErPrm);
      }

      sprintf(text,"   RTB:%ld, Routers Num:%d, Networks Num:%d, Size:%ld, TotalCompTime:%ld, AsExtCompTime:%ld, SummCompTime:%ld",
              p_RTB->VpnCos, 
              o2RoutingTableCount(p_RTB, p_RTB->RtbRtBt), 
              o2RoutingTableCount(p_RTB, p_RTB->RtbNtBt), RtbSize,
         p_RTB->RtComputeTime,
         p_RTB->AsExtIncComputeTime,
         p_RTB->SumIncComputeTime);
      XX_Print(text);

      sprintf(text, "\n\nNumber of times SPF delayed waiting for RTO update:  %u", p_RTB->spfWaitForRto);
      XX_Print(text);

      e = HL_GetNext(p_RTO->RtbHl,  (void**)&p_RTB, p_RTB);
   }

   return E_OK;
}




/*********************************************************************
 * @purpose             Print OSPF Routing table.
 *
 *
 * @param RTO_Id        @b{(input)}  RTO object Id
 * @param VpnCos        @b{(input)}  VPN and COS
 *
 * @returns             E_OK - RTB found,
 * @returns             E_FAILED - RTB not found
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RtbPrint(t_Handle RTO_Id, t_VpnCos VpnCos)
{
   t_RTO *p_RTO = (t_RTO *) RTO_Id;
   t_RTB *p_RTB;
   t_RoutingTableEntry *p_Rte = NULLP;
   t_RtbEntryInfo info;
   e_Err e;
   Bool ntLookup;
#if L7_OSPF_TE
   word i;
   t_ErPrm *Er;
#endif
   t_OspfPathPrm *path;

   /* Locate the RtbBt handler which belongs to this VPN */
   if(HL_FindFirst(p_RTO->RtbHl, (byte *) &VpnCos, (void *)&p_RTB) != E_OK)
      return E_FAILED;

   RTO_TraceWrite("OSPF: Routing table:");
   sysapiPrintf("\nNetwork routes....\n");

   ntLookup = TRUE;
   e = o2RouteGetFirst(p_RTB, p_RTB->RtbNtBt, &p_Rte);
   if(e != E_OK)
   {
      e = o2RouteGetFirst(p_RTB, p_RTB->RtbRtBt, &p_Rte);
      ntLookup = FALSE;
   }
   while(e == E_OK)
   {
      memset(&info, 0, sizeof(t_RtbEntryInfo));
      memcpy(&info, &p_Rte->DestinationId,
            offsetof(t_RoutingTableEntry,VrtlIfo) -
            offsetof(t_RoutingTableEntry,DestinationId));
      info.Status = RTE_REMOVED;
      info.NextHopNum = p_Rte->PathNum;
      if(p_Rte->PathPrm)
         info.NextIpAdr = p_Rte->PathPrm->IpAdr;
       RtbEntryPrint(&info, 0);

      for(path = p_Rte->PathPrm; path; path = path->next)
      {
#if L7_OSPF_TE
         for(i = 0, Er = path->ErList; Er && (i < 20); Er = Er->next,i++)
         {
            info.RtrChain[i] = A_GET_4B(Er->RouterId);
            info.IfIndex[i] = A_GET_4B(Er->IfIndex);
         }
#endif

         RtbEntryPrint(&info, 2);
      }

      if(ntLookup)
      {
         e = o2RouteGetNext(p_RTB, p_RTB->RtbNtBt, &p_Rte);
         if(e != E_OK)
         {
            ntLookup = FALSE;
            sysapiPrintf("\n\nRouter routes...\n");
            e = o2RouteGetFirst(p_RTB, p_RTB->RtbRtBt, &p_Rte);
         }
      }
      else
      {
         e = o2RouteGetNext(p_RTB, p_RTB->RtbRtBt, &p_Rte);
      }
   }
   return E_OK;
}




/*********************************************************************
 * @purpose            Print OSPF Routing table Entry.
 *
 *
 * @param p_Rte        @b{(input)}  RTB entry info
 *
 * @returns            n/a.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void RtbEntryPrint(t_RtbEntryInfo *p_Rte, byte mode)
{
   static char *aDestType[] = {
         "ILLEGAL ",
         "ROUTER  ",
         "ILLEGAL ",
         "ASBR    ",
         "ILLEGAL ",
         "ABR     ",
         "ILLEGAL ",
         "ABR-ASBR",
         "NETWORK ",
         "ILLEGAL ",
         "ILLEGAL ",
         "ILLEGAL ",
         "ILLEGAL ",
         "ILLEGAL ",
         "ILLEGAL ",
         "ILLEGAL ",
         "STUB_NET",
         "HOST/NET",
         "AS-EXT  "};

   static char *aPathType[] = {
         "INTRA_AREA",
         "INTER_AREA",
         "TYPE_1_EXT",
         "TYPE_2_EXT",
         "ILLEG_TYPE"};

   static char *ChType[] = {
         "NO CHANGE",
         "ADDED    ",
         "REMOVED  ",
         "MODIFIED "};

   SP_IPADR ipadr;
   char *p_type;

   ipadr = p_Rte->NextHopNum ? p_Rte->NextIpAdr : 0;

   if(p_Rte->IpMask == SPF_HOST_ROUTE)
       p_type = aDestType[17];
   else if((p_Rte->PathType == OSPF_TYPE_2_EXT) ||
       (p_Rte->PathType == OSPF_TYPE_1_EXT))
       p_type = aDestType[18];
   else
       p_type = aDestType[p_Rte->DestinationType];

   if(mode == 1)
    osapiSnprintf(text,200,"OSPF: %s-> %s, Id:%d.%d.%d.%d, Mask:%d.%d.%d.%d, Area:%d.%d.%d.%d, Cost:%ld, Cost2:%ld, Path:%s, Opt:%x, NhAdr:%d.%d.%d.%d, NhNum:%d.",
       ChType[p_Rte->Status],
       p_type,
       ADR_PRINT(p_Rte->DestinationId),
       ADR_PRINT(p_Rte->IpMask),
       ADR_PRINT(p_Rte->AreaId),
       p_Rte->Cost,
       p_Rte->Type2Cost,
       aPathType[p_Rte->PathType],
       p_Rte->OptionalCapabilities,
       ADR_PRINT(ipadr),
       p_Rte->NextHopNum);
    else if(mode == 0)
    osapiSnprintf(text,200,"OSPF: Type:%s, Id:%d.%d.%d.%d, Mask:%d.%d.%d.%d, Area:%d.%d.%d.%d, Cost:%ld, Cost2:%ld, Path:%s, Opt:%x, NhAdr:%d.%d.%d.%d, NhNum:%d.",
       p_type,
       ADR_PRINT(p_Rte->DestinationId),
       ADR_PRINT(p_Rte->IpMask),
       ADR_PRINT(p_Rte->AreaId),
       p_Rte->Cost,
       p_Rte->Type2Cost,
       aPathType[p_Rte->PathType],
       p_Rte->OptionalCapabilities,
       ADR_PRINT(ipadr),
       p_Rte->NextHopNum);
   else if(mode == 2)
   {
      int i, len;

      i = 0;
      len = 5;
      memcpy(text,"     ",6);
      while((i < 20) && p_Rte->RtrChain[i] )
      {
         sprintf(&text[len], "[If:%d.%d.%d.%d->R:%d.%d.%d.%d]-> ",
            ADR_PRINT(p_Rte->IfIndex[i]), ADR_PRINT(p_Rte->RtrChain[i]));
         len = strlen(text);
         i ++;
      }
      if(i == 0)
         text[0] = 0;
   }
   XX_Print(text);
}




/*********************************************************************
 * @purpose             Trap Event Info Print
 *
 *
 * @param Id            @b{(input)}  Router Id
 * @param TrapType      @b{(input)}  trap type
 * @param p_Trap        @b{(input)}  pointer to store area user Id
 *
 * @returns             n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void RTO_TrapPrint(t_Handle rto_Id, e_OspfTrapType TrapType, u_OspfTrap *p_Trap)
{
    static char *aTrapType[] = {
            "VIRT_IF_STATE_CHANGE         ",
            "NBR_STATE_CHANGE             ",
            "VIRT_NBR_STATE_CHANGE        ",
            "IF_CONFIG_ERROR              ",
            "VIRT_IF_CONFIG_ERROR         ",
            "IF_AUTH_FAILURE              ",
            "VIRT_IF_AUTH_FAILURE         ",
            "RX_BAD_PACKET                ",
            "VIRT_IF_RX_BAD_PACKET        ",
            "TX_RETRANSMIT                ",
            "VIRT_IF_TX_RETRANSMIT        ",
            "ORIGINATE_LSA                ",
            "MAX_AGE_LSA                  ",
            "LS_DB_OVERFLOW               ",
            "LS_DB_APPROACHING_OVERFLOW   ",
            "IF_STATE_CHANGE              ",
            "PACKET_RECEIVED              ",
            "RTB_ENTRY_INFO               "
    };

    static char *aCnfErrorType[] = {
        "CONFIG_OK                 ",
            "BAD_VERSION               ",
            "AREA_MISMATCH             ",
            "UNKNOWN_NBMA_NBR          ",
            "UNKNOWN_VIRTUAL_NBR       ",
            "AUTH_TYPE_MISMATCH        ",
            "AUTH_FAILURE              ",
            "NET_MASK_MISMATCH         ",
            "HELLO_INTERVAL_MISMATCH   ",
            "DEAD_INTERVAL_MISMATCH    ",
            "OPTION_MISMATCH           ",
            "BAD_PACKET                "
    };

    static char *aPacketType[] = {"ILLEGAL     ", "HELLO       ",
        "DB_DESCR    ", "LS_REQUEST  ", "LS_UPDATE   ", "LS_ACK      ",
        "IGMP_QUERY  ", "IGMP_REPORT1", "ILLEGAL     ","ILLEGAL     ","ILLEGAL     ",
        "IGMP_REPORT2", "IGMP_LEAVE  "};

    static char *aLsdbType[] = {"ILLEGAL_LSA       ", "ROUTER_LSA        ",
        "NETWORK_LSA       ", "IPNET_SUMMARY_LSA ", "ASBR_SUMMARY_LSA  ",
        "AS_EXTERNAL_LSA   ", "GROUP_MEMBER_LSA  ", "ILLEGAL_LSA       ",
        "ILLEGAL_LSA       ", "LINK_OPAQUE_LSA   ", "AREA_OPAQUE_LSA   ",
        "AS_OPAQUE_LSA     "};

    static char *aIfoStates[] = {"DOWN    ", "LOOP    ", "WAIT    ",
        "PTP     ",  "DR      ", "BACKUP  ","DROTHER "};


    static char *aNboStates[] = {"DOWN     ", "ATTEMPT  ", "INIT     ",
        "2WAY     ", "EXSTART  ", "EXCHANGE ", "LOADING  ", "FULL     "};

    static char *IfoEvent[] = {
   "IFO_UP     ",
   "WAIT_TIMER ",
   "BACKUP_SEEN",
   "NGB_CHANGE ",
   "LOOP_IND   ",
   "UNLOOP_IND ",
   "IFO_DOWN   "};


    static char *NboEvent[] = {
   "HLO_RX     ",
   "START      ",
   "2WAY_RX    ",
   "NEG_DONE   ",
   "EXC_DONE   ",
   "BAD_LSREQ  ",
   "LOAD_DONE  ",
   "ADJOK      ",
   "SEQNUM_MISM",
   "1WAY_RX    ",
   "KILL       ",
   "INACT_TIMER",
   "LL_DOWN    "};

   if(TrapType != RTB_ENTRY_INFO)
   {
      osapiSnprintf(text,200,"OSPF:  ========= Router Trap: %s =========",
                    aTrapType[TrapType]);
      XX_Print(text);
   }

   switch(TrapType)
   {
      case VIRT_IF_STATE_CHANGE:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtIfStateChange.RouterId));
        XX_Print(text);
        sprintf(text," VirtIfAreaId: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtIfStateChange.VirtIfAreaId));
        XX_Print(text);
        sprintf(text," VirtIfNeighbor: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtIfStateChange.VirtIfNeighbor));
        XX_Print(text);
        osapiSnprintf(text,200," VirtIfState: %s", aIfoStates[p_Trap->VirtIfStateChange.VirtIfState]);
        XX_Print(text);
        osapiSnprintf(text,200," Event: %s", IfoEvent[p_Trap->VirtIfStateChange.Event]);
        XX_Print(text);

      break;
      case NBR_STATE_CHANGE:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->NbrStateChange.RouterId));
        XX_Print(text);
        sprintf(text," NbrIpAddr: %d.%d.%d.%d", ADR_PRINT(p_Trap->NbrStateChange.NbrIpAddr));
        XX_Print(text);
        sprintf(text," NbrAddressLessIndex: %x", (unsigned int)p_Trap->NbrStateChange.NbrAddressLessIndex);
        XX_Print(text);
        sprintf(text," NbrRtrId: %d.%d.%d.%d", ADR_PRINT(p_Trap->NbrStateChange.NbrRtrId));
        XX_Print(text);
        osapiSnprintf(text,200," NbrState: %s", aNboStates[p_Trap->NbrStateChange.NbrState-1]);
        XX_Print(text);
        osapiSnprintf(text,200," Event: %s", NboEvent[p_Trap->NbrStateChange.Event]);
        XX_Print(text);

      break;
      case VIRT_NBR_STATE_CHANGE:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtNbrStateChange.RouterId));
        XX_Print(text);
        sprintf(text," VirtNbrArea: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtNbrStateChange.VirtNbrArea));
        XX_Print(text);
        sprintf(text," VirtNbrRtrId: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtNbrStateChange.VirtNbrRtrId));
        XX_Print(text);
        osapiSnprintf(text,200," VirtNbrState: %s", aNboStates[p_Trap->VirtNbrStateChange.VirtNbrState-1]);
        XX_Print(text);
        osapiSnprintf(text,200," Event: %s", NboEvent[p_Trap->VirtNbrStateChange.Event]);
        XX_Print(text);

      break;
      case IF_CONFIG_ERROR:
      case IF_AUTH_FAILURE:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->IfConfigError.RouterId));
        XX_Print(text);
        sprintf(text," IfIpAddress: %d.%d.%d.%d", ADR_PRINT(p_Trap->IfConfigError.IfIpAddress));
        XX_Print(text);
        sprintf(text," AddressLessIf: %x", (unsigned int)p_Trap->IfConfigError.AddressLessIf);
        XX_Print(text);
        sprintf(text," PacketSrc: %d.%d.%d.%d", ADR_PRINT(p_Trap->IfConfigError.PacketSrc));
        XX_Print(text);
        osapiSnprintf(text,200," PacketType: %s", aPacketType[p_Trap->IfConfigError.PacketType]);
        XX_Print(text);
        osapiSnprintf(text,200," ErrorType: %s", aCnfErrorType[p_Trap->IfConfigError.ConfigErrorType]);
        XX_Print(text);

      break;
      case VIRT_IF_CONFIG_ERROR:
      case VIRT_IF_AUTH_FAILURE:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtIfConfigError.RouterId));
        XX_Print(text);
        sprintf(text," VirtIfAreaId: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtIfConfigError.VirtIfAreaId));
        XX_Print(text);
        sprintf(text," VirtIfNeighbor: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtIfConfigError.VirtIfNeighbor));
        XX_Print(text);
        osapiSnprintf(text,200," PacketType: %s", aPacketType[p_Trap->VirtIfConfigError.PacketType]);
        XX_Print(text);
        osapiSnprintf(text,200," ErrorType: %s", aCnfErrorType[p_Trap->VirtIfConfigError.ConfigErrorType]);
        XX_Print(text);

      break;
      case RX_BAD_PACKET:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->IfRxBadPacket.RouterId));
        XX_Print(text);
        sprintf(text," IfIpAddress: %d.%d.%d.%d", ADR_PRINT(p_Trap->IfRxBadPacket.IfIpAddress));
        XX_Print(text);
        sprintf(text," AddressLessIf/GroupId: %x", (unsigned int)p_Trap->IfRxBadPacket.AddressLessIf);
        XX_Print(text);
        sprintf(text," PacketSrc: %d.%d.%d.%d", ADR_PRINT(p_Trap->IfRxBadPacket.PacketSrc));
        XX_Print(text);
        osapiSnprintf(text,200," PacketType: %s", aPacketType[p_Trap->IfRxBadPacket.PacketType]);
        XX_Print(text);

      break;
      case VIRT_IF_RX_BAD_PACKET:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtIfRxBadPacket.RouterId));
        XX_Print(text);
        sprintf(text," VirtIfAreaId: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtIfRxBadPacket.VirtIfAreaId));
        XX_Print(text);
        sprintf(text," VirtIfNeighbor: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtIfRxBadPacket.VirtIfNeighbor));
        XX_Print(text);
        osapiSnprintf(text,200," PacketType: %s", aPacketType[p_Trap->VirtIfRxBadPacket.PacketType]);
        XX_Print(text);

      break;
      case TX_RETRANSMIT:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->TxRetransmit.RouterId));
        XX_Print(text);
        sprintf(text," IfIpAddress: %d.%d.%d.%d", ADR_PRINT(p_Trap->TxRetransmit.IfIpAddress));
        XX_Print(text);
        sprintf(text," AddressLessIf: %x", (unsigned int)p_Trap->TxRetransmit.AddressLessIf);
        XX_Print(text);
        sprintf(text," NbrRtrId: %d.%d.%d.%d", ADR_PRINT(p_Trap->TxRetransmit.NbrRtrId));
        XX_Print(text);
        osapiSnprintf(text,200," PacketType: %s", aPacketType[p_Trap->TxRetransmit.PacketType]);
        XX_Print(text);
        osapiSnprintf(text,200," LsdbType: %s", aLsdbType[p_Trap->TxRetransmit.LsdbType]);
        XX_Print(text);
        sprintf(text," LsdbLsid: %d.%d.%d.%d", ADR_PRINT(p_Trap->TxRetransmit.LsdbLsid));
        XX_Print(text);
        sprintf(text," LsdbRouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->TxRetransmit.LsdbRouterId));
        XX_Print(text);
        sprintf(text," LsdbSeqNum: %d.%d.%d.%d", ADR_PRINT(p_Trap->TxRetransmit.LsdbSeqNum));
        XX_Print(text);

      break;
      case VIRT_IF_TX_RETRANSMIT:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtTxRetransmit.RouterId));
        XX_Print(text);
        sprintf(text," VirtIfAreaId: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtTxRetransmit.VirtIfAreaId));
        XX_Print(text);
        sprintf(text," VirtIfNeighbor: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtTxRetransmit.VirtIfNeighbor));
        XX_Print(text);
        osapiSnprintf(text,200," PacketType: %s", aPacketType[p_Trap->VirtTxRetransmit.PacketType]);
        XX_Print(text);
        osapiSnprintf(text,200," LsdbType: %s", aLsdbType[p_Trap->VirtTxRetransmit.LsdbType]);
        XX_Print(text);
        sprintf(text," LsdbLsid: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtTxRetransmit.LsdbLsid));
        XX_Print(text);
        sprintf(text," LsdbRouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtTxRetransmit.LsdbRouterId));
        XX_Print(text);
        sprintf(text," LsdbSeqNum: %d.%d.%d.%d", ADR_PRINT(p_Trap->VirtTxRetransmit.LsdbSeqNum));
        XX_Print(text);

      break;
      case ORIGINATE_LSA:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->OriginateLsa.RouterId));
        XX_Print(text);
        sprintf(text," LsdbAreaId %d.%d.%d.%d", ADR_PRINT(p_Trap->OriginateLsa.LsdbAreaId));
        XX_Print(text);
        osapiSnprintf(text,200," LsdbType: %s", aLsdbType[p_Trap->OriginateLsa.LsdbType]);
        XX_Print(text);
        sprintf(text," LsdbLsid: %d.%d.%d.%d", ADR_PRINT(p_Trap->OriginateLsa.LsdbLsid));
        XX_Print(text);
        sprintf(text," LsdbRouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->OriginateLsa.LsdbRouterId));
        XX_Print(text);
        sprintf(text," LsdbSeqNum: %d.%d.%d.%d", ADR_PRINT(p_Trap->OriginateLsa.LsdbSeqNum));
        XX_Print(text);

      break;
      case MAX_AGE_LSA:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->MaxAgeLsa.RouterId));
        XX_Print(text);
        sprintf(text," LsdbAreaId %d.%d.%d.%d", ADR_PRINT(p_Trap->MaxAgeLsa.LsdbAreaId));
        XX_Print(text);
        osapiSnprintf(text,200," LsdbType: %s", aLsdbType[p_Trap->MaxAgeLsa.LsdbType]);
        XX_Print(text);
        sprintf(text," LsdbLsid: %d.%d.%d.%d", ADR_PRINT(p_Trap->MaxAgeLsa.LsdbLsid));
        XX_Print(text);
        sprintf(text," LsdbRouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->MaxAgeLsa.LsdbRouterId));
        XX_Print(text);
        sprintf(text," LsdbSeqNum: %d.%d.%d.%d", ADR_PRINT(p_Trap->MaxAgeLsa.LsdbSeqNum));
        XX_Print(text);

      break;
      case LS_DB_OVERFLOW:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->LsdbOverflow.RouterId));
        XX_Print(text);
        sprintf(text," ExtLsdbLimit: %ld", p_Trap->LsdbOverflow.ExtLsdbLimit);
        XX_Print(text);

      break;
      case LS_DB_APPROACHING_OVERFLOW:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->LsdbApproachingOverflow.RouterId));
        XX_Print(text);
        sprintf(text," ExtLsdbLimit: %ld", p_Trap->LsdbApproachingOverflow.ExtLsdbLimit);
        XX_Print(text);

      break;
      case IF_STATE_CHANGE:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->IfStateChange.RouterId));
        XX_Print(text);
        sprintf(text," IfIpAddress: %d.%d.%d.%d", ADR_PRINT(p_Trap->IfStateChange.IfIpAddress));
        XX_Print(text);
        sprintf(text," AddressLessIf: %x", (unsigned int)p_Trap->IfStateChange.AddressLessIf);
        XX_Print(text);
        osapiSnprintf(text,200," IfState: %s", aIfoStates[p_Trap->IfStateChange.IfState]);
        XX_Print(text);
        osapiSnprintf(text,200," Event: %s", IfoEvent[p_Trap->IfStateChange.Event]);
        XX_Print(text);

      break;

      case IF_RX_PACKET:

        sprintf(text," RouterId: %d.%d.%d.%d", ADR_PRINT(p_Trap->IfRxPacket.RouterId));
        XX_Print(text);
        sprintf(text," IfIpAddress: %d.%d.%d.%d", ADR_PRINT(p_Trap->IfRxPacket.IfIpAddress));
        XX_Print(text);
        sprintf(text," AddressLessIf/GroupId: %x", (unsigned int)p_Trap->IfRxPacket.AddressLessIf);
        XX_Print(text);
        sprintf(text," PacketSrc: %d.%d.%d.%d", ADR_PRINT(p_Trap->IfRxPacket.PacketSrc));
        XX_Print(text);
        osapiSnprintf(text,200," PacketType: %s", aPacketType[p_Trap->IfRxPacket.PacketType]);
        XX_Print(text);

      break;
      case RTB_ENTRY_INFO:

          RtbEntryPrint(&p_Trap->RtbEntryInfo, 1);
        break;
      default:
         sprintf(text," Uknown Trap Event");
         XX_Print(text);
      break;
   }

   if(TrapType != RTB_ENTRY_INFO)
   {
      sprintf(text,"OSPF:  =============== End Trap =====");
      XX_Print(text);
   }
}


/* --------------------------------------------------------------- */
/* --------------------------------------------------------------- */
/* ---- internal routines ---------------------------------------- */
/* --------------------------------------------------------------- */
/* --------------------------------------------------------------- */

/*********************************************************************
 * @purpose          LSA Retransmit Tick Expiry Handler
 *
 *
 * @param RTO_Id     @b{(input)}  Router Id
 *
 * @returns          E_OK
 *
 * @notes            Checks NBO LsRxmt lists for LSA retransmission.
 *
 *
 * @end
 * ********************************************************************/
e_Err LsaRxmtTickExp(t_RTO * p_RTO)
{
  t_ARO * p_ARO = NULLP;
  t_IFO * p_IFO = NULLP;
  t_NBO * p_NBO = NULLP;
  e_Err e = E_OK;
  L7_uint32 startTime = osapiTimeMillisecondsGet();
  L7_uint32 retxEntries = p_RTO->retxEntries;

  /* browse areas */
  e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
  while(e == E_OK && p_ARO)
  {
    /* browse intefaces */
    e = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
    while(e == E_OK && p_IFO)
    {
      /* browse neighbors */
      e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
      while(e == E_OK && p_NBO)
      {
        /* send LS rxmt for this neighbor */
        NBO_LsRxmtExpire(p_NBO);
        e = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
      }
      /* get next interface */
      e = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
    }
    /* get next area */
    e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
  }

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_RETX)
   {
     char traceBuf[OSPF_MAX_TRACE_STR_LEN];
     L7_uint32 duration = osapiTimeMillisecondsGetOffset(startTime);
     sprintf(traceBuf, "OSPF retransmit list processing took %u ms for %u entries.",
             duration, retxEntries);
     RTO_TraceWrite(traceBuf);
   }

  return e;
}

/*********************************************************************
 * @purpose      Update stats history
 *
 * @returns      void
 *
 * @notes        Updates a circular buffer of LSA stats.
 *
 * @end
 * ********************************************************************/
void o2UpdateStats(t_RTO *p_RTO)
{
  L7_uint32 index = p_RTO->statsIndex;
  L7_uint32 prevIndex = (index == 0) ? O2_STATS_SIZE - 1 : index - 1;
  L7_uint32 rxDelta;;
  L7_uint32 txDelta;

  /* Record current stats */
  p_RTO->lsuRxStats[index] = p_RTO->RxLsUpdate;
  p_RTO->lsuTxStats[index] = p_RTO->TxLsUpdate;

  rxDelta = p_RTO->lsuRxStats[index] - p_RTO->lsuRxStats[prevIndex];
  txDelta = p_RTO->lsuTxStats[index] - p_RTO->lsuTxStats[prevIndex];

  /* Record maximums */
  if (rxDelta > p_RTO->RxLsUpdateMax)
  {
    p_RTO->RxLsUpdateMax = rxDelta;
  }
  if (txDelta > p_RTO->TxLsUpdateMax)
  {
    p_RTO->TxLsUpdateMax = txDelta;
  }

  /* Increment index */
  p_RTO->statsIndex++;
  if (p_RTO->statsIndex == O2_STATS_SIZE)
  {
    p_RTO->statsIndex = 0;
  }
}

/*********************************************************************
 * @purpose      Clear stats high water marks
 *
 * @returns      void
 *
 * @notes        
 *
 * @end
 * ********************************************************************/
void o2PacketStatsHwClear(void)
{
  t_RTO *p_RTO = (t_RTO*) RTO_List;

  p_RTO->RxLsUpdateMax = 0;
  p_RTO->TxLsUpdateMax = 0;
}

/*********************************************************************
 * @purpose      Print stats history
 *
 * @returns      void
 *
 * @notes        Print latest stats first
 *
 * @end
 * ********************************************************************/
void o2PacketStatsShow(void)
{
  t_RTO *p_RTO = (t_RTO*) RTO_List;
  L7_uint32 i;
  L7_uint32 index;
  L7_uint32 prevIndex;
  L7_uint32 offset = p_RTO->statsIndex;
  L7_uint32 deltaRx, deltaTx;

  printf("\n\nSeconds since boot:  %u", osapiUpTimeRaw());

  printf("\n\nNumber of LS Update packets received:  %u", p_RTO->RxLsUpdate);
  printf("\nNumber of LS Update packets transmitted:  %u", p_RTO->TxLsUpdate);

  printf("\n\nMax receive rate (%d second interval):  %u", 
         O2_STATS_INTERVAL, p_RTO->RxLsUpdateMax);
  printf("\nMax transmit rate (%d second interval):  %u", 
         O2_STATS_INTERVAL, p_RTO->TxLsUpdateMax);

  printf("\n\nLS Update packets sent and received in each %d-second interval for last 30 minutes:",
         O2_STATS_INTERVAL);
  printf("\n\n%5s %12s %12s", "Interval", "Received", "Transmitted");

  for (i = 1; i < (O2_STATS_SIZE - 1); i++)
  {
    if (i <= offset)
    {
      index = offset - i;
    }
    else
    {
      index = O2_STATS_SIZE - (i - offset);
    }
    if (index == 0)
      prevIndex = O2_STATS_SIZE - 1;
    else
      prevIndex = index - 1;

    /* Only print non-zero intervals */
    deltaRx = p_RTO->lsuRxStats[index] - p_RTO->lsuRxStats[prevIndex];
    deltaTx = p_RTO->lsuTxStats[index] - p_RTO->lsuTxStats[prevIndex];
    if (deltaRx || deltaTx)
    {
      printf("\n%5u %12u %12u", i, deltaRx, deltaTx);
    }
  }
}

/*----------------------------------------------------------------
 *
 *      Timer Expiration Routines
 *
 *----------------------------------------------------------------*/

/*********************************************************************
 * @purpose          OSPF Tick Timer Expiry Handler
 *
 * @returns          E_OK
 *
 * @notes
 *
 *
 * @end
 * ********************************************************************/
e_Err OspfTickTimerExp( t_Handle Id, t_Handle TimerId, word Flag )
{
  t_RTO *p_RTO = (t_RTO *) Id;

  /* service NBO retx lists */
  LsaRxmtTickExp(p_RTO);

  if (osapiUpTimeRaw() % O2_STATS_INTERVAL == 0)
  {
    o2UpdateStats(p_RTO);
  }

  return E_OK;
}

e_Err OverflowTimerExp( t_Handle Id, t_Handle TimerId, word Flag )
{
   t_RTO *p_RTO = (t_RTO *) Id;
   t_S_AsExternalCfg *p_Entry;
   e_Err e;
   word extRouteNum = 0;

   /* If originating the LSAs is going to put us back in overflow state
   ** then simply restart the overflow timer and remain in overflow state
   */
   if((HL_GetEntriesNmb(p_RTO->AsExtRoutesHl, &extRouteNum) != E_OK) ||
      ((p_RTO->NonDefExtLsaCount + (ulng)extRouteNum) >= (ulng)p_RTO->Cfg.ExtLsdbLimit))
   {
     TIMER_StartSec(p_RTO->OverflowTimer, p_RTO->Cfg.ExitOverflowInterval, 0,
                    OverflowTimerExp, p_RTO->OspfSysLabel.threadHndle );
     return E_OK;
   }


   p_RTO->IsOverflowed = FALSE;

   if(!p_RTO->Cfg.ASBdrRtrStatus) {
       /* redistribution has been turned off since router entered
          overflow state */
      return E_OK;
   }

   /* Reoriginate the non-default AS-Ext LSAs */
   e = HL_GetFirst(p_RTO->AsExtRoutesHl, (void *)&p_Entry);
   while(e == E_OK)
   {
      if(p_Entry->DestNetIpAdr)
      {
         p_Entry->PrevEntry = (t_Handle)RTE_ADDED;
         LsaReOriginate[S_AS_EXTERNAL_LSA](p_RTO, (ulng)p_Entry);
         LsaOrgNssaBulk(p_RTO, p_Entry);
         if(p_RTO->IsOverflowed)
            break;
      }
      e = HL_GetNext(p_RTO->AsExtRoutesHl, (void *)&p_Entry, p_Entry);
   }
   return E_OK;
}

/*********************************************************************
 * @purpose    Handle expiration of stub router startup timer
 *
 * @returns    E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err o2StubRouterTimerExp(t_Handle Id, t_Handle TimerId, word Flag)
{
   t_RTO *p_RTO = (t_RTO *) Id;

   if (p_RTO->LsdbOverload)
   {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID, 
             "OSPFv2 stub router startup time expired.");
   }

   return o2StubRouterModeExit(p_RTO);
}

/*********************************************************************
* @purpose  Clear stub router mode.
*
* @param    RTO_Id - OSPF instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Clear doesn't exit configured stub router mode.
*
* @end
*********************************************************************/
e_Err o2StubRouterClear(t_Handle RTO_Id)
{
  t_RTO *p_RTO = (t_RTO*) RTO_Id;

  if (!p_RTO->LsdbOverload)
    return E_OK;

  if (p_RTO->LsdbOverloadReason == OSPF_STUB_RTR_CONFIGURED)
  {
    /* Cannot clear a configured stub router with clear command. */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID, 
            "Router is configured as a stub router. To exit stub router mode,"
            " use 'no max-metric router-lsa' in OSPF router configuration mode.");
    return E_OK;
  }

  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID, 
          "OSPFv2 stub router mode cleared by user.");

  return o2StubRouterModeExit(p_RTO);
}

/*********************************************************************
* @purpose  Set the OSPF trace flags.
*
* @param    traceFlags     Indicates the tracing desired (e_OSPF_TRACE_FLAGS)
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    For now, sets the trace flags on the default RTO.
*
* @end
*********************************************************************/
e_Err ospfTraceFlagsSet(ulng traceFlags)
{
   t_RTO *p_RTO = RTO_List;
   if (p_RTO)
   {
       p_RTO->ospfTraceFlags = traceFlags;
       return E_OK;
   }
   return E_FAILED;
}

/*********************************************************************
* @purpose  Set all of the OSPF trace flags.
*
* @param    none
*
* @notes    For now, sets all the trace flags on the default RTO.
*
* @end
*********************************************************************/
void ospfTraceFlagsSetAll()
{
   t_RTO *p_RTO = RTO_List;
   if (p_RTO)
   {
       p_RTO->ospfTraceFlags = 0xFFFF;
   }
   return;
}

/*********************************************************************
* @purpose  Set all of the OSPF trace flags.
*
* @param    none
*
* @notes    For now, sets all the trace flags on the default RTO.
*
* @end
*********************************************************************/
void ospfTraceFlagsSetNone()
{
   t_RTO *p_RTO = RTO_List;
   if (p_RTO)
   {
       p_RTO->ospfTraceFlags = 0x0000;
   }
   return;
}


word LsRefreshTimeGet(t_RTO *p_RTO)
{
  return p_RTO->Cfg.LsRefreshTime;
}

/* Debug function to set the LS refresh time. Seconds. */
e_Err LsRefreshTimeSet(word refreshTime)
{
  t_RTO *p_RTO = RTO_List;
  if (p_RTO)
  {
    p_RTO->Cfg.LsRefreshTime = refreshTime;
    return E_OK;
  }
  else
  {
    XX_Print("Failed to get OSPFv2 RTO object.");
  }
  return E_FAILED;
}

ulng RTO_MaxLsaRetxEntriesGet(t_RTO *p_RTO)
{
  return p_RTO->Cfg.MaxLsaRetxEntries;
}

/* Debug function to set the max number of retx entries. */
e_Err ospfDebugMaxRetxEntriesSet(ulng maxRetx)
{
  t_RTO *p_RTO = RTO_List;
  if (p_RTO)
  {
    p_RTO->Cfg.MaxLsaRetxEntries = maxRetx;
    return E_OK;
  }
  else
  {
    XX_Print("Failed to get OSPFv2 RTO object.");
  }
  return E_FAILED;
}

/* Clear the retx entries high water mark */
void ospfDebugRetxHighWaterReset(void)
{
  t_RTO *p_RTO = RTO_List;
  if (p_RTO)
  {
    p_RTO->retxHighWater = 0;
  }
  else
  {
    XX_Print("Failed to get OSPFv2 RTO object.");
  }
}


/*********************************************************************
* @purpose  Print the OSPF trace flags help
*
* @param    void
*
* @returns  E_OK
*
* @notes    
*
* @end
*********************************************************************/
e_Err ospfDebugTraceHelp(ulng traceFlags)
{
  t_RTO *p_RTO = RTO_List;
  L7_uchar8 helpMsg[512];

  XX_Print("0x1,        hello tx/rx");
  XX_Print("0x2,        database description packets");
  XX_Print("0x4,        link state request packets");
  XX_Print("0x8,        link state update packets");
  XX_Print("0x10,       link state ack packets");
  XX_Print("0x20,       interface and neighbor state changes");
  XX_Print("0x40,       shortest path algorithm");
  XX_Print("0x80,       virtual links");
  XX_Print("0x100,      NSSAs");
  XX_Print("0x200,      OSPF packets received");
  XX_Print("0x800,      LSA aging");
  XX_Print("0x1000,     SPF detail");
  XX_Print("0x2000,     router and network LSA preparsing");
  XX_Print("0x4000,     route redistribution");
  XX_Print("0x8000,     LSA origination");
  XX_Print("0x10000,    DR calculation");
  XX_Print("0x20000,    Graceful restart");
  XX_Print("0x40000,    Checkpointing");
  XX_Print("0x80000,    LSA retransmission");
  XX_Print("0x100000,   OSPF routing table changes");

  if (p_RTO)
  {
    sprintf(helpMsg, "\nCurrent trace flags:  %#lx", p_RTO->ospfTraceFlags);
    XX_Print(helpMsg);
  }
  return E_OK;
}

/*********************************************************************
* @purpose  Generate a name for an OSPF interface. Non-virtual interfaces
*           use the NIM name (unit/slot/port). Virtual interfaces use
*           a combination of area ID and remote router ID. This name
*           should be suitable for trace messages and other debug output.
*
* @param    p_IFO   interface whose name is desired
*           ifName  the name of the interface. Buffer should be at least
*                   (L7_NIM_IFNAME_SIZE + 1) characters long.
*
* @returns  E_OK
*
* @notes
*
* @end
*********************************************************************/
e_Err RTO_InterfaceName(t_IFO *p_IFO, char *ifName)
{
    if (p_IFO->Cfg.Type == IFO_VRTL)
    {
        /* virtual interface. Can't use usp. */
        L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 virtNbrStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(p_IFO->Cfg.VirtTransitAreaId, areaIdStr);
        osapiInetNtoa(p_IFO->Cfg.VirtIfNeighbor, virtNbrStr);
        sprintf(ifName, "VirtIf_Area_%s_Nbr_%s", areaIdStr, virtNbrStr);
    }
    else
    {
        L7_uint32 intIfNum = p_IFO->Cfg.IfIndex;
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    }
    return E_OK;
}

/*********************************************************************
* @purpose  Generate an OSPF trace message.
*
* @param    inputStr    A string of debug information to be presented to the user.
 *               Only the first OSPF_MAX_TRACE_STR_LEN characters of
 *               the input string will be printed.
*
* @returns  E_OK
*
* @notes
*
* @end
*********************************************************************/
e_Err RTO_TraceWrite(char *inputStr)
{
    L7_uint32 i, stackUptime, secs, msecs;
    L7_uchar8 debugMsg[512];
    L7_char8 msgQLenStr[10];
    L7_int32 msgQLen = -1;
    t_RTO *p_RTO = RTO_List;
    OS_Thread *threadId;


    if (inputStr == NULL)
        return E_OK;

    stackUptime = simSystemUpTimeMsecGet();
    secs = stackUptime / 1000;
    msecs = stackUptime % 1000;

    /* For now, just print the message with a timestamp. */
    sprintf(debugMsg, "\n\n%d.%03d (", secs, msecs);

    if(p_RTO)
    {
      threadId = (OS_Thread*) p_RTO->OspfRtbThread.threadHndle;
      for(i = 0; i < OSPF_NUM_QUEUES; i++)
      {
        if (osapiMsgQueueGetNumMsgs((void*)threadId->QueueID[i], &msgQLen) != L7_SUCCESS) 
        {
          msgQLen = -1;
        }
        osapiSnprintf(msgQLenStr,10, "%d", msgQLen);

        if(i)
          strncat(debugMsg, ",", OSPF_MAX_TRACE_STR_LEN);

        strncat(debugMsg, msgQLenStr, OSPF_MAX_TRACE_STR_LEN);
      }
    }

    strncat(debugMsg, "): ", OSPF_MAX_TRACE_STR_LEN);

    strncat(debugMsg, inputStr, OSPF_MAX_TRACE_STR_LEN);
    sysapiPrintf(debugMsg);
    
    return E_OK;
}


/*********************************************************************
* @purpose  Convert an OSPF options bit string to a character string
*           suitable for trace messages.
*
* @param    options    OSPF options bit string
* @param    optStr     character string representing options. Point to
*                      a buffer of at least 20 bytes.
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void OspfOptionsStr(byte options, L7_uchar8 *optStr)
{
    *optStr = '\0';
    if (options & OSPF_OPT_O_BIT)
        strncat(optStr, "O ", 2);
    if (options & OSPF_OPT_P_BIT)
        strncat(optStr, "N/P ", 4);
    if (options & OSPF_OPT_MC_BIT)
        strncat(optStr, "MC ", 3);
    if (options & OSPF_OPT_E_BIT)
        strncat(optStr, "E ", 2);
    if (options & OSPF_OPT_Q_BIT)
        strncat(optStr, "Q", 1);
}


#if L7_MIB_OSPF




/*********************************************************************
 * @purpose               Bind OSPF MIB handle to the router object.
 *
 *
 * @param RTO_Id          @b{(input)}  RTO Object handle
 * @param ospfMibHandle   @b{(input)}  the OSPF MIB handle to be bound
 *                                     to the RTO
 *
 * @returns  E_OK       - success
 * @returns  E_BADPARM  - bad parameter(s)
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_BindOspfMibObject (t_Handle RTO_Id, t_Handle ospfMibHandle)
{
   t_RTO   *p_RTO = (t_RTO *)RTO_Id;

   /* Verify the input parameter(s) */
   if (!RTO_Id)
      return E_BADPARM;

   if(p_RTO->Taken != TRUE)
      return E_BADPARM;

   p_RTO->ospfMibHandle = ospfMibHandle;

   return E_OK;
}




/*********************************************************************
 * @purpose         Return OSPF MIB handle bound to the Router object.
 *
 *
 * @param RTO_Id    @b{(input)}  RTO Object handle
 *
 * @returns         OSPF MIB handle - success
 * @returns         NULLP           - failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle RTO_GetOspfMibObject (t_Handle RTO_Id)
{
   t_RTO   *p_RTO = (t_RTO *)RTO_Id;

   /* Verify the input parameter */
   if(!RTO_Id)
      return NULLP;

   if(p_RTO->Taken != TRUE)
      return NULLP;

   return p_RTO->ospfMibHandle;
}

#endif /* L7_MIB_OSPF */

/*********************************************************************
 * @purpose         Determine if the supplied IP address is configured
 *                  on an interface running OSPF.
 *
 * @param RTO_Id    @b{(input)}  RTO Object handle
 * @param addr      @b{(input)}  IP address pointer
 *
 * @returns         TRUE            
 * @returns         FALSE
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool
RTO_AddressIsLocallyConfigured(t_RTO *p_RTO, SP_IPADR *addr)
{

   t_ARO *p_ARO;
   e_Err ea, ei;

   ea = HL_GetFirst(p_RTO->AroHl, (void**)&p_ARO);

   /* Iterate through all attached areas. */
   while (ea == E_OK)
   {
      t_IFO *p_IFO;

      ei = HL_GetFirst(p_ARO->IfoHl, (void **)&p_IFO);

      while (ei == E_OK)
      {
         int j;

         if (p_IFO->Cfg.IpAdr == *addr)
            return TRUE;

         for (j=1; j < IFO_MAX_IP_ADDRS; j++)
         {
            if (p_IFO->Cfg.addrs[j].ipAddr == 0)
               break;
            else if (p_IFO->Cfg.addrs[j].ipAddr == *addr)
               return TRUE;
         }

         ei = HL_GetNext(p_ARO->IfoHl, (void **)&p_IFO, p_IFO);

      }

      ea = HL_GetNext(p_RTO->AroHl, (void**)&p_ARO, p_ARO);

   }

   return FALSE;

}

/*********************************************************************
 * @purpose   Given the next hop of a redistributed route, determine
 *            the forwarding address OSPF should advertise.
 *
 * @param     RTO_Id      @b{(input)}  RTO Object handle
 * @param     ospfStatus  @b{(output)} status data
 *
 * @returns   E_OK
 *
 * @notes     If OSPF has an intra or inter-area route to the next hop,
 *            advertise the next hop as the FA. Otherwise, the FA is set
 *            to 0.
 *
 * @end
 * ********************************************************************/
e_Err RTO_CalcForwardingAddr(t_RTO *p_RTO, SP_IPADR *nextHop, SP_IPADR *fwdAddr)
{
  t_RoutingTableEntry *p_Rte;
  t_RTB *p_RTB;
  t_VpnCos CoS = TOS_NORMAL_SERVICE;

  if (*nextHop &&
      ((HL_FindFirst(p_RTO->RtbHl, (byte*)&CoS, (void*)&p_RTB) == E_OK)))
  {
      /* ensure FA is reachable via intra-AS ospf route */
      p_Rte = o2LongestPrefixMatch(p_RTB, p_RTB->RtbNtBt, *nextHop);
      if (p_Rte)
      {
        if ((p_Rte->PathType == OSPF_INTRA_AREA) ||
            (p_Rte->PathType == OSPF_INTER_AREA)) 
        {
          *fwdAddr = *nextHop;
          return E_OK;
        }
      }
  }

  *fwdAddr = DefaultDestination;
  return E_OK;
}

e_Err RTO_VerifyForwardingAddrs(t_RTO *p_RTO)
{
    t_S_AsExternalCfg *p_Entry;
    e_Err eIter;

    for (eIter = HL_GetFirst(p_RTO->AsExtRoutesHl, (void *) &p_Entry);
         eIter == E_OK;
         eIter = HL_GetNext(p_RTO->AsExtRoutesHl, (void*) &p_Entry, p_Entry))
    {
        SP_IPADR newFwdAddr;

        RTO_CalcForwardingAddr(p_RTO, &p_Entry->Nexthop, &newFwdAddr);

        /* If some change occured, then reoriginate the LSA */
        if (p_Entry->ForwardingAdr != newFwdAddr) 
        {
           p_Entry->PrevEntry = (t_Handle)RTE_ADDED;
           p_Entry->ForwardingAdr = newFwdAddr;
           LsaReOriginate[S_AS_EXTERNAL_LSA](p_RTO, (ulng)p_Entry);

           /* verify all T7s leaked as T5s against new set of T5s */
           LsaOrgNssaBulk(p_RTO, p_Entry);
        }
    }

    return E_OK;
}

/*********************************************************************
 * @purpose         Get global status data.
 *
 * @param RTO_Id      @b{(input)}  RTO Object handle
 * @param ospfStatus  @b{(output)} status data
 *
 * @returns         E_OK
 *                  E_FAILED if input param bad
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_GlobalStatusGet(t_Handle RTO_Id, L7_ospfStatus_t *ospfStatus)
{
  t_RTO *p_RTO = (t_RTO*) RTO_Id;
  t_ARO *p_ARO;
  t_IFO *p_IFO;
  e_Err e, e2;

  if (!ospfStatus)
    return E_FAILED;

  ospfStatus->abr = routerIsBorder(p_RTO);
  /* OSPF mapping layer handles ASBR status. Don't overwrite. */
  ospfStatus->stubRouter = p_RTO->LsdbOverload;
  ospfStatus->stubReason = p_RTO->LsdbOverloadReason;
  if ((p_RTO->LsdbOverloadReason == OSPF_STUB_RTR_STARTUP) &&
      TIMER_Active(p_RTO->LsdbOverloadTimer))
  {
    ospfStatus->stubRemainingTime = TIMER_ToExpireSec(p_RTO->LsdbOverloadTimer);
  }
  ospfStatus->extLsdbOverflow = p_RTO->IsOverflowed;
  ospfStatus->extLsaCount = p_RTO->Cfg.ExternLsaCount;
  ospfStatus->extLsaChecksum = p_RTO->Cfg.ExternLsaCksumSum;
  ospfStatus->asOpaqueLsaCount = p_RTO->Cfg.OpaqueASLsaCount;
  ospfStatus->asOpaqueLsaChecksum = p_RTO->OpaqueASLsaCksumSum;
  ospfStatus->lsasOrig = p_RTO->Cfg.OriginateNewLsas;
  ospfStatus->lsasReceived = p_RTO->Cfg.RxNewLsas;
  ospfStatus->lsaCount = p_RTO->ActiveLsas;
  ospfStatus->lsaCapacity = OSPF_MAX_LSAS;
  ospfStatus->lsaHighWater = p_RTO->lsaHighWater;
  ospfStatus->retxEntries = p_RTO->retxEntries;
  ospfStatus->retxCapacity = RTO_MaxLsaRetxEntriesGet(p_RTO);
  ospfStatus->retxHighWater = p_RTO->retxHighWater;

  /* Count number of active areas of each type */
  ospfStatus->normalAreas = 0;
  ospfStatus->stubAreas = 0;
  ospfStatus->nssaAreas = 0;
  e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
  while (e == E_OK)
  {
    /* Only consider an area active if it is operational and there is at least 
     * one interface that's up. */
    if (p_ARO->OperationState)
    {
        Bool upIfFound = FALSE;
        e2 = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
        while ((e2 == E_OK) && !upIfFound)
        {
            if (p_IFO->Cfg.State != IFO_DOWN)
            {
              /* This area is active. */
              if (p_ARO->ExternalRoutingCapability == AREA_IMPORT_EXTERNAL)
                ospfStatus->normalAreas++;
              else if (p_ARO->ExternalRoutingCapability == AREA_IMPORT_NO_EXTERNAL)
                ospfStatus->stubAreas++;
              else if (p_ARO->ExternalRoutingCapability == AREA_IMPORT_NSSA)
                ospfStatus->nssaAreas++;
              upIfFound = TRUE;
            }
            e2 = HL_GetNext(p_ARO->IfoHl, (void*)&p_IFO, p_IFO);
        }
    }
    e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
  }

#ifdef L7_NSF_PACKAGE
  ospfStatus->restartStatus = p_RTO->restartStatus;
  ospfStatus->restartAge = 0;
  if ((p_RTO->restartStatus != OSPF_GR_NOT_RESTARTING) &&
      TIMER_Active(p_RTO->gracePeriodTimer))
  {
    ospfStatus->restartAge = (L7_uint32) TIMER_ToExpireSec(p_RTO->gracePeriodTimer);
  }
  ospfStatus->grExitReason = p_RTO->grExitReason;
#endif

  return E_OK;
}

/*********************************************************************
 * @purpose         Clear OSPF statistics.
 *
 * @param RTO_Id      @b{(input)}  RTO Object handle
 *
 * @returns         E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTO_CountersClear(t_Handle RTO_Id)
{
  e_Err ea, ei, en;
  t_RTO *p_RTO = (t_RTO*) RTO_Id;
  t_IFO *p_IFO;
  t_ARO *p_ARO;
  t_NBO *p_NBO;

  /* Global stats */
  p_RTO->Cfg.OriginateNewLsas = 0;
  p_RTO->Cfg.RxNewLsas = 0;
  p_RTO->lsaHighWater = 0;
  p_RTO->retxHighWater = 0;

  ea = HL_GetFirst(p_RTO->AroHl, (void**)&p_ARO);
  while (ea == E_OK)
  {
    /* no area stats to clear. don't clear num spf runs. */
    ei = HL_GetFirst(p_ARO->IfoHl, (void**) &p_IFO);
    while (ei == E_OK)
    {
      /* Interface stats */
      memset(&p_IFO->PckStats, 0, sizeof (t_PCK_Stats));
      en = HL_GetFirst(p_IFO->NboHl, (void**) &p_NBO);
      while (en == E_OK)
      {
        p_NBO->NbrEvents = 0;
        en = HL_GetNext(p_IFO->NboHl, (void**)&p_NBO, p_NBO);
      }
      ei = HL_GetNext(p_ARO->IfoHl, (void**)&p_IFO, p_IFO);
    }
    ea = HL_GetNext(p_RTO->AroHl, (void**)&p_ARO, p_ARO);
  }
  return E_OK;
}

/*********************************************************************
 * @purpose         Allocate a retransmit list entry.
 *
 * @param RTO_Id      @b{(input)}  RTO Object handle
 *
 * @returns     pointer to new retx entry, or NULL 
 *
 * @notes   Zeros the retx structure.
 *          Adds to the count of retx entries in use. 
 *
 * @end
 * ********************************************************************/
t_RetxEntry *RTO_RetxEntryAlloc(t_RTO *p_RTO)
{
  t_RetxEntry *p_RetxEntry;

  if ((p_RetxEntry = XX_Malloc(sizeof(t_RetxEntry))) == NULL)
    return NULL;
  memset(p_RetxEntry, 0, sizeof(t_RetxEntry));
  p_RTO->retxEntries++;
  if (p_RTO->retxEntries > p_RTO->retxHighWater)
  {
    p_RTO->retxHighWater = p_RTO->retxEntries;
  }
  return p_RetxEntry;
}

/*********************************************************************
 * @purpose      Check if sufficient resources exist to store and flood
 *               an LSA. 
 *
 * @param RTO_Id         @b{(input)}  RTO Object handle
 * @param replaceFormer  @b{(input)}  TRUE if new LSA replaces an existing
 *                                      LSA in our LSDB
 *
 * @returns  OSPF_LSDB_FULL - LSDB contains OSPF_MAX_LSAS and can't store 
 *                            a new LSA
 *           OSPF_RETX_LIMIT - System has the max number of retx entries 
 *                            already in use. 
 *           OSPF_RESOURCE_AVAIL - Resources exist for new LSA
 *
 * @notes   
 *
 * @end
 * ********************************************************************/
e_OSPF_RES_CHECK RTO_LsaResourceCheck(t_RTO *p_RTO, Bool replaceFormer)
{
  /* See if LSDB is already full. If there is a previous LSA that the new 
   * one replaces, the LSDB isn't really growing and no need to check if 
   * we're full. Need to reissue our router LSAs even if LSDB is full. */
   if (!replaceFormer && (p_RTO->ActiveLsas >= OSPF_MAX_LSAS))
   {
       if (!p_RTO->LsdbOverload)
       {
         L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_OSPF_MAP_COMPONENT_ID,
                 "Discarding LSA because link state database is full."
                 "Link state database contains %u LSAs.", OSPF_MAX_LSAS);
         EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
       }
       return OSPF_LSDB_FULL;
   }

   if (p_RTO->retxEntries >= RTO_MaxLsaRetxEntriesGet(p_RTO))
   {
     /* Don't go into overload. We'll simply drop the LSA and not ACK. 
      * Neighbor will retx to us. Hopefully, we'll be able to accept then. */
     return OSPF_RETX_LIMIT;
   }

   return OSPF_RESOURCE_AVAIL;
}

/*********************************************************************
 * @purpose      Check if sufficient resources exist to store and flood
 *               an LSA. 
 *
 * @param RTO_Id         @b{(input)}  RTO Object handle
 * @param replaceFormer  @b{(input)}  TRUE if new LSA replaces an existing
 *                                      LSA in our LSDB
 *
 * @returns  OSPF_LSDB_FULL - LSDB contains OSPF_MAX_LSAS and can't store 
 *                            a new LSA
 *           OSPF_RETX_LIMIT - System has the max number of retx entries 
 *                            already in use. 
 *           OSPF_RESOURCE_AVAIL - Resources exist for new LSA
 *
 * @notes   
 *
 * @end
 * ********************************************************************/
Bool HasAsFloodingScope(e_S_LScodes lsaType)
{
  return ((lsaType == S_AS_EXTERNAL_LSA) ||
          (lsaType == S_AS_OPAQUE_LSA));
}

/* --- end of file SPRTO.C --- */
