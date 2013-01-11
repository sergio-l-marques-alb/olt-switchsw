/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename  sprtb.c
 *
 * @purpose   OSPF Routing Table calculation service module.
 *
 * @component OSPF Routing Component
 *
 * @comments   RTB_Init
 * RTB_Delete
 * RTB_Find
 * GetVpn
 * CheckVpn
 * RtrLsaCheckPresense
 * NetLsaCheckPresense
 * RecalcTimerExp
 * RteRelease
 * PathCopy
 * PathAdd
 * PathCompare
 * CheckDestPath
 *
 * Internal Routines:
 *
 * RTB_EntireTableCalc
 * SumIncrementalCalc
 * AsExtIncrementalCalc
 * AsExtCalc
 * AsExtUpdateRte
 * InterAreaCalc
 * TransitAreaExam
 * SpfIntraAreaCalc
 * SpfCheckLink
 * CalculateNextHop
 * UnresolvedRteDelete
 * RteAdded
 * RteChanged
 * RteRemoved
 * ReorgSumLsa
 * RemoteVpnRouterInd
 * FindVrtNgbIpAdr
 * CheckVirtLinkUp
 * RteStatusReset
 * RteUpdate
 * PathCompareAndValidate
 * RteIntraAreaFind
 * RteIntraAreaFindByType
 *
 *
 * @create    Dan Dovolsky
 *
 * @author    02/10/1999
 *
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\application\protocol\ospf\sprtb.c 1.3.3.14 2003/02/20 16:48:47EST anayar Exp  $";
#endif


/* --- standard include files --- */

/* --- external object interfaces --- */

/* --- specific include files --- */

#include "std.h"
#include "stdio.h"
#include "local.h"



#include "spobj.h"
#include "spgrc.h"
#include "osapi.h"
#include "osapi_support.h"
#include "os_xxcmn.h"
#include "frame.h"
#include "buffer.h"

/* --- internal prototypes --- */

/* Path computation performance mesuring base on system clock */
#ifdef WIN32
#include <time.h>
#define GET_SYS_CLOCK clock()
#else
#define GET_SYS_CLOCK TIMER_SysTime()
#endif
/* --- internal (static) data  --- */

static e_Err RTB_EntireTableCalcCb(void *p_Info);
static e_Err SumIncrementalCalc(t_RTB *p_RTB, t_ARO *p_ARO, 
                                t_A_DbEntry *p_DbEntry);
static e_Err AsExtIncrementalCalc(t_RTB *p_RTB, t_A_DbEntry *p_DbEntry);
static e_Err AsExtCalc(t_RTB *p_RTB, t_RoutingTableKey *SingleDestKey);
static e_Err AsExtUpdateRte(t_RTB *p_RTB, t_Handle objId,
                            t_RoutingTableKey *SingleDestKey);
static e_Err AsExtVerifyLsa(t_RTB *p_RTB, t_A_DbEntry *p_DbEntry, 
                            t_RoutingTableEntry **p_PrefRte);
static e_Err AsExtModifyRte(t_RTB *p_RTB, t_A_DbEntry *p_DbEntry, 
                            t_RoutingTableEntry *p_PreferRte);
static e_Err InterAreaCalc(t_RTB *p_RTB, t_ARO *p_ARO, t_RoutingTableKey *rtbkey, Bool ForSingleDest);
static e_Err TransitAreaExam(t_RTB *p_RTB, t_ARO *p_ARO, t_RoutingTableKey *rtbkey, Bool ForSingleDest);
static e_Err SpfIntraAreaCalc(t_RTB *p_RTB, t_ARO *p_ARO);
static e_Err RTB_IntraAreaStage1(t_RTB *p_RTB, t_ARO *p_ARO, t_RoutingTableEntry *p_V);
static e_Err RTB_IntraAreaStage2(t_RTB *p_RTB, t_ARO *p_ARO, t_RoutingTableEntry *p_V);
static e_Err RTB_IntraAreaRouterVertexAdded(t_RTB *p_RTB, t_ARO *p_ARO, 
                                            t_RoutingTableEntry *p_V);
static e_Err RTB_IntraAreaNetworkVertexAdded(t_RTB *p_RTB, t_ARO *p_ARO, 
                                             t_RoutingTableEntry *p_V);

static e_Err AsExtFindPreferableRoute(t_RTB *p_RTB, t_A_DbEntry *p_DbEntry,
                                      t_RoutingTableEntry **p_PrefRte,
                                      SP_IPADR forwardingAddr);

static Bool UnresolvedRteDelete(byte *rte, ulng AroId);
Bool CheckDestPath(t_RoutingTableEntry *p_Rte, t_Handle ifoid);

static void RteAdded(t_RTB *p_RTB, t_RoutingTableEntry *p_Rte);
void RteChanged(t_RTB *p_RTB, t_RoutingTableEntry *p_Old, t_RoutingTableEntry *p_New); 
static void RteRemoved(t_RTB *p_RTB, t_RoutingTableEntry *p_Rte);
static void DetermineActiveRanges(t_RTB *p_RTB);
static void UpdateAdrRange(t_RTB *p_RTB, t_RoutingTableEntry *p_Rte,
                           e_RtChange chType);
e_Err CreateRejectRoute(t_RTB *p_RTB, t_AgrEntry *p_AdrRange);
void UpdateAggregationInfo(t_RTB *p_RTB, t_AgrEntry *p_AdrRange,
                           t_RoutingTableEntry *p_Rte, e_RtChange chType);
Bool UpdateNssaAggregationInfo (t_RTB *p_RTB, t_RoutingTableEntry *p_Rte,
                                       e_RtChange chType);
static void ReorgSumLsa(t_RTO *p_RTO, t_RoutingTableEntry *p_Rte,
                        e_RtChange chType);

static SP_IPADR FindVrtNgbIpAdr(t_RTB *p_RTB, t_A_DbEntry *p_DbEntry);
void CheckVirtLinkUp(t_IFO *p_IFO, t_RTB *p_RTB);
static Bool VlinkStatusUpdate(t_Handle ifo, ulng RtbId);
static e_Err SpfCheckLink(t_RTB *p_RTB, t_ARO *p_ARO, t_RoutingTableEntry *p_V, t_ParseEntry *PrsEntry);
static e_Err CalculateNextHop(t_RTB *p_RTB, t_ARO *p_ARO, t_RoutingTableEntry *p_W,
                              t_RoutingTableEntry *p_V, t_ParseEntry *PrsEntry);
static e_Err CalculateNextHopToStub(t_RTB *p_RTB, t_ARO *p_ARO, t_RoutingTableEntry *p_W,
                                    t_RoutingTableEntry *p_V);
static e_Err CalcNextHopToAdjRouter(t_RTB *p_RTB, t_ARO *p_ARO, 
                                    t_RoutingTableEntry *p_W,
                                    t_RoutingTableEntry *p_V,
                                    t_ParseEntry *PrsEntry);

static e_Err RteUpdate(t_RTB *p_RTB, t_RoutingTableEntry  *p_Rte);
static Bool PathCompareAndValidate(t_RoutingTableEntry  *Rte1, t_RoutingTableEntry *Rte2);
static e_Err RTB_RtoUpdate(void *p_Info);

static e_Err RTB_RtoUpdateRoutes(t_RTB *p_RTB, ulng startTime, t_Handle newTree, t_Handle oldTree);
static e_Err RTB_RtoFlushRoutes(t_RTB *p_RTB, ulng startTime, t_Handle oldTree);
static Bool RTB_RoutesEqual(t_RoutingTableEntry *route1, t_RoutingTableEntry *route2);
static e_Err RTB_RouteReplace(t_RTB *p_RTB, t_RoutingTableEntry *p_RtEntry, 
                              t_RoutingTableEntry *p_V);
void RTB_ComputeCalcDelay(t_RTB *p_RTB, t_RTO *p_RTO);

void RTB_ComputeCalcDelay(t_RTB *p_RTB, t_RTO *p_RTO);
void RTB_ClearCandidateHl(t_ARO *p_ARO);
void RTB_ClearSpt(t_ARO *p_ARO);
e_Err RTB_LsaProcess(t_RTO *p_RTO, t_ARO *p_ARO, t_A_DbEntry *p_DbEntry);

int pruneArea(t_OspfPathPrm *path, SP_IPADR key);
int pruneAdvRtr(t_OspfPathPrm *path, SP_IPADR key);
typedef int (*pruneMatch)(t_OspfPathPrm *, SP_IPADR);
static e_Err PrunePaths(t_RTB *p_RTB, t_RoutingTableEntry *p_Rte, 
                        SP_IPADR key, pruneMatch prune);

static e_Err RtbRteDelete(t_RTB *p_RTB, t_RoutingTableEntry *p_Rte);
static e_Err RteIntraAreaFind(t_ARO *p_ARO, SP_IPADR rtrId, 
                              t_RoutingTableEntry **p_Rt);
e_Err RteIntraAreaFindByType(t_ARO *p_ARO, SP_IPADR rtrId,
                                    e_OspfDestType type,
                                    t_RoutingTableEntry **p_Rte);
static e_Err RTB_SpfStatsUpdate(t_RTB *p_RTB, L7_uint32 spfStart);
static e_Err OspfReasonToString(L7_uint32 reasonMask, L7_uchar8 *reasonStr);

static e_Err RTB_AsbrUpdate(t_RTB *p_RTB, t_ARO *p_ARO, 
                            t_RoutingTableEntry *p_V);
static e_Err AsExtPreferredPathFind(t_RTO *p_RTO, t_RoutingTableEntry *p_New, 
                                    t_RoutingTableEntry *p_Old, 
                                    Bool *NewIsPreferred,
                                    Bool section_16_4_1_only);

extern word InterfaceMetric(t_IFO *p_IFO);

static unsigned char *rtbUpdateStates[RTB_UPD_LAST] = {"Update Done", "Update Start", 
    "NET_PEND", "NET_FLUSH",  "RTR_PEND", "RTR_FLUSH"};

extern e_Err HideSummariesOfBackbone(t_ARO *p_backbone, t_ARO *p_ARO);
extern e_Err DeliverSummariesOfBackbone(t_ARO *p_backbone, t_ARO *p_ARO);

/*********************************************************************
 * @purpose     Initialize an RTB object.
 *
 * @param RTO_Id         @b{(input)}  Parent Router Object handler
 * @param VpnCos         @b{(input)}VPN/COS identifies this RTB object
 * @param p_RTB_Id       @b{(output)}RTB Object handler returned here
 *
 * @returns E_OK           success
 * @returns E_FAILED       couldn't create an object
 * @returns E_NOMEMORY     not enough memory for resources allocating
 *
 * @notes
 *          At least one RTB object mustbe created after RTO object
 *          creation and before IFO objects creation.For each configured
 *          IFO VPN the VPN related RTB object must be created.Besides
 *          that, the additional RTBs may be created with different VPNs.
 *          The separate routing table created and calculated for each RTB's VPN.
 *
 * @end
 * ********************************************************************/
e_Err RTB_Init(t_Handle RTO_Id, t_VpnCos VpnCos, t_Handle *p_RTB_Id)
{
   t_RTO *p_RTO = (t_RTO *)RTO_Id;
   t_RTB *p_RTB;
   e_Err e = E_OK;
   e_Err e2;
   t_VpnCos zero = 0;

   /* try allocate from user memory */
   p_RTB = (t_RTB *)XX_Malloc( sizeof(t_RTB) );
   if(p_RTB)
      memset( p_RTB, 0, sizeof(t_RTB) );
   else
      return E_NOMEMORY;

   p_RTB->Taken = TRUE;

   e2 = o2RoutingTableCreate(p_RTB, OSPF_MAX_ROUTES);
   if (e2 != E_OK)
   {
     if (e2 == E_NOMEMORY)
     {
       L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
               "Failed to allocate memory for OSPF routing table.");
     }
     else
     {
       L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
               "Failure creating OSPF routing table.");
     }
   }

   e |= TIMER_InitSec( 1, p_RTB, &p_RTB->RecalcTimer );

   /* Setup the first start-up silence interval */
   p_RTB->FirstCalc = TRUE;
   if(p_RTO->Cfg.StartUpSilenceInterval)
      p_RTB->CalcDelay = p_RTO->Cfg.StartUpSilenceInterval;
   else
      p_RTB->CalcDelay = ROUTER_DEAD_INTERVAL_DFLT;

   p_RTB->RTO_Id = p_RTO;
   p_RTB->IncExtCalcNum = 0;
   p_RTB->IncSumCalcNum = 0;
   p_RTB->spfWaitForRto = 0;
   
   p_RTB->RtbUpdateState = RTB_UPD_DONE;
   p_RTB->RtePendingUpdate = NULL;
   
   if ((e == E_OK) && (e2 == E_OK))
   {
      HL_SortedInsert(p_RTO->RtbHl, (byte *) &zero, p_RTB);
   
      /* pass object ID back to caller */
      *p_RTB_Id = (t_Handle) p_RTB;     
      return E_OK;
   }

      RTB_Delete((t_Handle) p_RTB, 1);

   return E_FAILED;
}

/*********************************************************************
 * @purpose      Remove all routes from an area's candidate hash list.
 *
 * @param p_ARO  @b{(input)}  Area whose candidate HL is to be cleared.
 *
 * @returns      void
 *
 * @notes        Frees memory for each route in the HL. 
 *
 * @end
 * ********************************************************************/
void RTB_ClearCandidateHl(t_ARO *p_ARO)
{
  e_Err e;
  t_RoutingTableEntry *p_Rte = NULL;
  t_RoutingTableEntry *p_NextRte = NULL;

  e = AVLH_GetFirst(p_ARO->CandidateHl, (void *)&p_Rte);
   while (e == E_OK)
   {  
     e = AVLH_GetNext(p_ARO->CandidateHl, (void *)&p_NextRte, p_Rte);
     AVLH_Delete(p_ARO->CandidateHl, (byte*)&p_Rte->DestinationId, NULL);
     RteRelease(p_Rte, TRUE);   /* free the route */
     p_Rte = p_NextRte;
   }
}

/*********************************************************************
 * @purpose      Remove all routes from an area's shortest path tree.
 *
 * @param p_ARO  @b{(input)}  Area whose SPT is to be cleared.
 *
 * @returns      void
 *
 * @notes        Frees memory for each route in the SPT. 
 *
 * @end
 * ********************************************************************/
void RTB_ClearSpt(t_ARO *p_ARO)
{
  e_Err e;
  t_RoutingTableEntry *p_Rte = NULL;
  t_RoutingTableEntry *p_NextRte = NULL;

  e = AVLH_GetFirst(p_ARO->SpfHl, (void *)&p_Rte);
   while (e == E_OK)
   {  
     e = AVLH_GetNext(p_ARO->SpfHl, (void *)&p_NextRte, p_Rte);
     AVLH_Delete(p_ARO->SpfHl, (byte*)&p_Rte->DestinationId, NULL);
     RteRelease(p_Rte, TRUE);   /* free the route */
     p_Rte = p_NextRte;
   }
}


/*********************************************************************
 * @purpose            Delete a RTB object.
 *
 *
 * @param Id           @b{(input)}  RTB object Id
 * @param flag         @b{(input)}  delete this entry from RTO RtbHl
 *
 * @returns            TRUE if this entry remains in RTO RtbHl,
 * @returns            FALSE otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool RTB_Delete( t_Handle Id, ulng flag)
{
   t_RTB   *p_RTB = (t_RTB *)Id;
   t_VpnCos zero = 0;
      
   p_RTB->Taken = FALSE;
   
   if(flag)
      HL_Delete(((t_RTO*)p_RTB->RTO_Id)->RtbHl, (byte *) &zero, p_RTB);

   o2ClearRoutes(p_RTB, p_RTB->RtbRtBt);
   o2ClearRoutes(p_RTB, p_RTB->RtbRtBtOld);
   o2ClearRoutes(p_RTB, p_RTB->RtbNtBt);
   o2ClearRoutes(p_RTB, p_RTB->RtbNtBtOld);
   o2RoutingTableDelete(p_RTB);

   TIMER_Delete( p_RTB->RecalcTimer );

   p_RTB->RtbUpdateState = RTB_UPD_DONE;
   p_RTB->RtePendingUpdate = NULL;

   XX_Free(p_RTB);

   return ((Bool) flag);
}

/*********************************************************************
 * @purpose  Returns the number of routes in the current network routes tree.      
 *
 * @param    p_RTB  @b{(input)}    routing table
 *                                
 * @returns  number of routes 
 *
 * @notes
 *
 * @end
 * ********************************************************************/
ulng RTB_NetworkRoutesCount(t_Handle Id)
{
  t_RTB *p_RTB = (t_RTB*) Id;
  return o2RoutingTableCount(p_RTB, p_RTB->RtbNtBt);
}


/*********************************************************************
 * @purpose        Process a new LSA when it is added to the LSDB. 
 *                 Update existing routes computed from the LSA's 
 *                 predecessor to point to the new database entry. 
 *                 Run an incremental SPF or schedule a full SPF.
 *
 * @param    p_RTO   @b{(input)}    OSPF instance
 * @param    p_ARO   @b{(input)}    Area where LSA was learned
 * @param    p_DbEntry @b{(input)}  New database entry
 *                                
 * @returns  E_OK 
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RTB_LsaProcess(t_RTO *p_RTO, t_ARO *p_ARO, t_A_DbEntry *p_DbEntry)
{
   e_S_LScodes LsType = p_DbEntry->Lsa.LsType;
   t_S_LinkDscr *p_Link;
   ulng         links;
   t_RTB        *p_RTB;
   e_Err        e;
   t_RoutingTableEntry *p_Rte = NULL;
   
   /* Check if RTO Object is valid */
   if(!p_RTO->Taken)
      return E_OK;

   /* Check if DB entry is valid */
   if(! (L7_BIT_ISSET(p_DbEntry->dbFlags, O2_DB_ENTRY_IS_CUR)))
      return E_OK;

   switch(LsType)
   {
      case S_ROUTER_LSA:

            for (e = HL_GetFirst(p_RTO->RtbHl, (void *)&p_RTB);
                 e == E_OK;
                 e = HL_GetNext(p_RTO->RtbHl, (void *)&p_RTB, p_RTB))
            {
               /* Update p_DbEntry pointer if match router RTE already exist */
           p_Rte = o2RouteFind(p_RTB, p_RTB->RtbRtBt, A_GET_4B(p_DbEntry->Lsa.LsId), SPF_HOST_ROUTE);
           if (p_Rte)
           {
              p_Rte->p_DbEntry = (L7_BIT_ISSET(p_DbEntry->dbFlags, O2_DB_ENTRY_IS_CUR) && 
                                  (A_GET_2B(p_DbEntry->Lsa.LsAge) <= MaxAge)) ?
                                  p_DbEntry:NULL;
           }

               /* Update p_DbEntry pointer if match stub network RTEs already exist */

               /* Get Router LSA's pointer to the first link descriptor */ 
               if((p_Link = (t_S_LinkDscr *) (p_DbEntry->p_Lsa + 4)) == NULL)
               {
                  ASSERT(p_Link != NULL); 
                  break;
               }
               /* get number of links */
               links = A_GET_4B(p_DbEntry->p_Lsa) & 0xFFFF;
               while(links--)
               {
              if (p_Link->Type == S_STUB_NET)
                  {
                 p_Rte = o2RouteFind(p_RTB, p_RTB->RtbNtBt, A_GET_4B(p_Link->LinkId), 
                                     A_GET_4B(p_Link->LinkData));
                 if (p_Rte)
                 {
                   p_Rte->p_DbEntry = (L7_BIT_ISSET(p_DbEntry->dbFlags, O2_DB_ENTRY_IS_CUR) && 
                                       (A_GET_2B(p_DbEntry->Lsa.LsAge) <= MaxAge)) ?
                                        p_DbEntry : NULL;
                 }
                  }
                  p_Link = (t_S_LinkDscr*) (((byte*)p_Link) + 
                     sizeof(t_S_LinkDscr) + p_Link->TosNum*sizeof(t_S_Tos));
               }

               p_RTB->RtbChanged = RTE_NOT_MODIFIED;

               RTB_AddSpfReason(p_RTB, R_ROUTER_LSA);
               if(!TIMER_Active(p_RTB->RecalcTimer))
               {
                  /* Start recalculation timer */
                  RTB_ComputeCalcDelay(p_RTB, p_RTO);
                  TIMER_StartSec(p_RTB->RecalcTimer, p_RTB->CalcDelay,
                                 0, RecalcTimerExp, p_RTO->OspfRtbThread.threadHndle);
               }
            }
            break;
         
         
      case S_NETWORK_LSA:
         
         for (e = HL_GetFirst(p_RTO->RtbHl, (void *)&p_RTB);
                 e == E_OK;
                 e = HL_GetNext(p_RTO->RtbHl, (void *)&p_RTB, p_RTB))
            {
               /* Update p_DbEntry pointer if match RTE already exist */
               p_Rte = o2RouteFind(p_RTB, p_RTB->RtbNtBt, A_GET_4B(p_DbEntry->Lsa.LsId),
                                   A_GET_4B(p_DbEntry->p_Lsa));
               if(p_Rte)
               {
                  p_Rte->p_DbEntry = (L7_BIT_ISSET(p_DbEntry->dbFlags, O2_DB_ENTRY_IS_CUR) && 
                                      (A_GET_2B(p_DbEntry->Lsa.LsAge) <= MaxAge)) ?
                                      p_DbEntry:NULL;
               }

               p_RTB->RtbChanged = RTE_NOT_MODIFIED;

               RTB_AddSpfReason(p_RTB, R_NETWORK_LSA);
               if(!TIMER_Active(p_RTB->RecalcTimer))
               {
                  /* Start recalculation timer */
                  RTB_ComputeCalcDelay(p_RTB, p_RTO);
                  TIMER_StartSec(p_RTB->RecalcTimer, p_RTB->CalcDelay,
                                 0, RecalcTimerExp, p_RTO->OspfRtbThread.threadHndle);
               }
            }
            
      break;      
      case S_IPNET_SUMMARY_LSA :
      case S_ASBR_SUMMARY_LSA  :

        /* Never need to calculate routes from our own summary LSAs. */
        if (p_RTO->Cfg.RouterId == A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter))
          return E_OK;

         
         
        /* If public LSA received all VPN Routing tables should be recalculated */
        for (e = HL_GetFirst(p_RTO->RtbHl, (void *)&p_RTB); 
             e == E_OK;
             e = HL_GetNext(p_RTO->RtbHl, (void *)&p_RTB, p_RTB))
        {
          SumIncrementalCalc(p_RTB, p_ARO, p_DbEntry);
        }
         
      break;
      case S_AS_EXTERNAL_LSA   :
      case S_NSSA_LSA          :

        /* Never need to calculate routes from our own external LSAs. */
        if (p_RTO->Cfg.RouterId == A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter))
          return E_OK;

         for (e = HL_GetFirst(p_RTO->RtbHl, (void *)&p_RTB);
                 e == E_OK;
                 e = HL_GetNext(p_RTO->RtbHl, (void *)&p_RTB, p_RTB))
          {
             p_RTB->RtbChanged = RTE_NOT_MODIFIED;
             
             /* the best route to destination must be recalculated */
             AsExtIncrementalCalc(p_RTB, p_DbEntry);
         }

      break;
      default:
      break;
   }

   return E_OK;
}

/*********************************************************************
 * @purpose        Check presense of Link Id within Router LSA's link list
 *
 * @param p_Lsa     @b{(input)}  body of router LSA
 * @param Id        @b{(input)}  link id
 * @param Mask      @b{(input)}  Link mask
 *
 * @returns         pointer to the link descriptor in Router LSA if found
 * @returns         NULL   if not found
 *
 * @notes           Considers link a match if its LinkId is in the same subnet
 *                  as Id/Mask. Only considers transit links. 
 *
 * @end
 * ********************************************************************/
t_S_LinkDscr *RtrLsaCheckPresense(t_S_RouterLsa *p_Lsa, SP_IPADR Id, SP_IPADR Mask)
{
   t_S_LinkDscr *p_Link;
   word link;

   if(Mask == 0)
      Mask = 0xffffffffL;

   link = A_GET_2B(p_Lsa->LinkNum);
   p_Link = (t_S_LinkDscr *) ((byte*)p_Lsa + sizeof(t_S_RouterLsa));
   while(link--)
   {
     if ((p_Link->Type == S_TRANSIT_NET) &&
         ((A_GET_4B(p_Link->LinkId) & Mask) == (Id & Mask)))
         return p_Link;      
         
      p_Link = (t_S_LinkDscr*) (((byte*)p_Link) + 
         sizeof(t_S_LinkDscr) + p_Link->TosNum*sizeof(t_S_Tos));
   }
   return NULL;   
}


/*********************************************************************
 * @purpose          Check presense of router ID within network
 *                   LSA's routers list
 *
 * @param p_DbEntry  @b{(input)}  network LSA of adjacent network
 * @param Id         @b{(input)}  router ID
 *
 * @returns          TRUE   if found
 * @returns          FALSE  if not found
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool NetLsaCheckPresense(t_A_DbEntry *p_DbEntry, SP_IPADR Id)
{
   byte *p_Lsa = p_DbEntry->p_Lsa;
   L7_uint32 numRouters;

   if (p_DbEntry->Lsa.LsType != S_NETWORK_LSA)
   {
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
             "NetLsaCheckPresense() called with incorrect database entry. LS type is %d",
             p_DbEntry->Lsa.LsType);
     return FALSE;
   }

   /* Figure out number of routers listed in network LSA */
   numRouters = (A_GET_2B(p_DbEntry->Lsa.Length) - LSA_HEADER_LEN - 4) / 4;

   /* go to beginning of attached routers list */
   p_Lsa += 4;

   while(numRouters--)
   {
      if(A_GET_4B(p_Lsa) == Id)
         return TRUE;      
      p_Lsa += 4;
   }
   return FALSE;   
}


/*********************************************************************
 * @purpose          One shot RTB recalculation timer expiration routine
 *
 *
 * @param Id         @b{(input)}  Routing Table object handler
 * @param TimerId    @b{(input)}  unused
 * @param Flag cost  @b{(input)}  unused
 *
 * @returns          Always E_OK.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RecalcTimerExp( t_Handle Id, t_Handle TimerId, word Flag )
{
   t_RTB *p_RTB = (t_RTB*) Id;
   t_RTO *p_RTO = (t_RTO*) p_RTB->RTO_Id;

   /* do not start an SPF until the RTO update from the previous SPF is done */   
   if(p_RTB->RtbUpdateState != RTB_UPD_DONE)
   {
     p_RTB->spfWaitForRto++;
     TIMER_StartSecMQueue(p_RTB->RecalcTimer, RTO_UPDATE_BACKOFF,
                    0, RecalcTimerExp, p_RTO->OspfRtbThread.threadHndle, OSPF_EVENT_QUEUE);
     return E_OK;
   }

   if(p_RTB->FirstCalc)
   {
      p_RTB->FirstCalc = FALSE;
   }

   RTB_EntireTableCalc(p_RTB);

   return E_OK;
}

/*********************************************************************
 * @purpose          Schedule an SPF to run
 *
 * @param p_RTO      @b{(input)}  Routing table object
 *
 * @returns          n/a.
 *
 * @notes            This always goes on the event queue 
 *
 * @end
 * ********************************************************************/
void ScheduleSPF(t_RTB *p_RTB)
{
   t_RTO *p_RTO = (t_RTO*) p_RTB->RTO_Id;
  
   t_XXCallInfo *callData = 0;
   PACKET_INIT_MQUEUE(callData, RTB_EntireTableCalcCb, 0, 0, 
                      OSPF_EVENT_QUEUE, 1, (ulng)p_RTB);
   ASSERT(callData);
   if (XX_Call(p_RTO->OspfRtbThread.threadHndle, callData) != E_OK)
   {
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to schedule SPF.");
     if (!p_RTO->LsdbOverload)
       EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
   }
}

/*********************************************************************
 * @purpose    Free the next hops belonging to a route.
 *
 * @param      p_Rte      @b{(input)}  routing table entry
 * @param      freeRoute  @b{(input)}  TRUE to free memory for the route
 *
 * @returns    void
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void RteRelease(t_RoutingTableEntry *p_Rte, Bool freeRoute)
{
#if L7_OSPF_TE
   t_ErPrm  *Er, *TmpEr;
#endif
   t_OspfPathPrm *Path, *TmpPath;

   Path = p_Rte->PathPrm;
   while(Path != NULL)
   {
#if L7_OSPF_TE
      Er = Path->ErList;
      while(Er != NULL)
      {
         TmpEr = Er->next;
         XX_Free(Er);
         Er = TmpEr;
         p_Rte->ErNum--;
      }
#endif

      TmpPath = Path->next;
      XX_Free(Path);
      Path = TmpPath;
      p_Rte->PathNum--;
   }
   p_Rte->PathPrm = NULL;
   p_Rte->PathNum = 0;
   p_Rte->ErNum = 0;

   if (freeRoute)
   {
      XX_Free(p_Rte);
   }
}




/*********************************************************************
 * @purpose   Deletes routing table entry if it belongs to the specified area
 *            and notifies RTO of the delete.  This procedure used
 *                 in browsing AVL function.
 *
 * @param     rte      @b{(input)}  routing table entry
 * @param     area     @b{(input)}  area object
 *
 * @returns   FALSE - delete route from routing table
 *            TRUE  - leave route in routing table
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool RteOfAreaDeleteAndNotify(byte *rte, ulng area)
{
   t_RoutingTableEntry *p_Rte = (t_RoutingTableEntry *)rte;
   t_ARO               *p_ARO =  (t_ARO *)area;
   t_RTO               *p_RTO = NULLP;
   ulng                rteCost = 0;

   if(!p_ARO)
      return TRUE;

   p_RTO = (t_RTO *)p_ARO->RTO_Id;

   if(p_Rte->AreaId != p_ARO->AreaId)
      return TRUE;


   /* if areaId of the routing table entry is the same as the  */
   /* specified area, then:                                    */
   /*  send notification to the user,                          */ 
   /*  release memory of this entry,                           */
   /*  return FALSE in order to delete the entry from the routing table list */

   /* Optional trap RTB entry information */
   if(p_RTO->Clbk.p_OspfTrap &&
      (p_RTO->TrapControl & GET_TRAP_BITMASK(RTB_ENTRY_INFO)))
   {
      t_RtbEntryInfo info;
#if L7_OSPF_TE
      t_ErPrm *Er;
      word     i;
#endif
   
      memcpy(&info, &p_Rte->DestinationId, 
            offsetof(t_RoutingTableEntry,VrtlIfo) - 
            offsetof(t_RoutingTableEntry,DestinationId));
      info.Status = RTE_REMOVED;
      info.NextHopNum = p_Rte->PathNum;
      if (p_Rte->PathPrm != NULL) {
         info.NextIpAdr = p_Rte->PathPrm->IpAdr; 
#if L7_OSPF_TE
         if(p_Rte->PathPrm->ErList)
         {
            for(i = 0, Er = p_Rte->PathPrm->ErList; Er && (i < 20); Er = Er->next)
            {
               info.RtrChain[i] = A_GET_4B(Er->RouterId);
               info.IfIndex[i] = A_GET_4B(Er->IfIndex);
            }
         }
#endif
      }
      p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, RTB_ENTRY_INFO, (u_OspfTrap*)&info);
   }

   /* if a network or ASBR route has been */
   /* removed, Summary_LSA has to be flushed   */
   if(((p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER) ||
       (p_Rte->DestinationType >= DEST_TYPE_IS_NETWORK)) &&
      ((p_Rte->PathType == OSPF_INTRA_AREA) ||
       (p_Rte->PathType == OSPF_INTER_AREA)))
         ReorgSumLsa(p_RTO, p_Rte, RTE_REMOVED);
   
   /* send notification to the user */
   if(p_RTO->Clbk.p_RoutingTableChanged)
   {
      rteCost = (((p_Rte->PathType == OSPF_TYPE_2_EXT) || 
                 (p_Rte->PathType == OSPF_NSSA_TYPE_2_EXT)) ? 
                 p_Rte->Type2Cost : p_Rte->Cost);

      p_RTO->Clbk.p_RoutingTableChanged(p_RTO->Clbk.f_RtmAsyncEvent[0] ? 
                       (t_Handle)p_RTO->Clbk.f_RtmAsyncEvent : p_RTO->MngId, 
                       p_Rte->DestinationType, RTE_REMOVED, 
                       p_Rte->DestinationId, p_Rte->IpMask, 
                       p_Rte->PathNum, rteCost, p_Rte->PathPrm, p_Rte->PathType,
                       p_Rte->isRejectRoute);
   }

   return FALSE;  /* remove this route from the OSPF routing table */
}


/*********************************************************************
 * @purpose       Deletes RTE entry if it was advertised by a specific
 *                neighbor This procedure used in browsing AVL function.
 *
 * @param rte     @b{(input)}  RTB entry
 * @param area    @b{(input)}  handle of the area object
 *
 * @returns       FALSE - the specified entry is to be removed from the
 *                        AVL data base
 * @returns       TRUE  - do nothing with the specified entry in AVL
 *                        browse function
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool RteOfNeighborDeleteAndNotify(byte *rte, ulng neighbor)
{
   t_RoutingTableEntry *p_Rte = (t_RoutingTableEntry *)rte;
   t_NBO               *p_NBO =  (t_NBO *)neighbor;
   t_RTO               *p_RTO = NULLP;
   ulng                rteCost= 0;

   if(p_Rte->p_DbEntry == NULLP)
      /* we don't need to do anything */
      return TRUE;
   
   if(!p_NBO)
      return TRUE;

   p_RTO = (t_RTO *)p_NBO->RTO_Id;

   /* If this route entry was not advertised by this neighbor then we don't need
      to delete it */
   if(A_GET_4B(p_Rte->p_DbEntry->Lsa.AdvertisingRouter) != p_NBO->RouterId)
     return TRUE;

/*----------------------------------------------------------------------------------*/
/*    If the advertising router is the same as the neighbor that is being deleted,  */
/*    we need to remove this route table entry since an NBO kill event has been     */
/*    generated.                                                                    */
/*----------------------------------------------------------------------------------*/
   
   /* Optional trap RTB entry information */
   if(p_RTO->Clbk.p_OspfTrap &&
      (p_RTO->TrapControl & GET_TRAP_BITMASK(RTB_ENTRY_INFO)))
   {
      t_RtbEntryInfo info;
#if L7_OSPF_TE
      t_ErPrm *Er;
      word     i;
#endif
   
      memcpy(&info, &p_Rte->DestinationId, 
            offsetof(t_RoutingTableEntry,VrtlIfo) - 
            offsetof(t_RoutingTableEntry,DestinationId));
      info.Status = RTE_REMOVED;
      info.NextHopNum = p_Rte->PathNum;
      if (p_Rte->PathPrm != NULL) {
         info.NextIpAdr = p_Rte->PathPrm->IpAdr; 
#if L7_OSPF_TE
         if(p_Rte->PathPrm->ErList)
         {
            for(i = 0, Er = p_Rte->PathPrm->ErList; Er && (i < 20); Er = Er->next)
            {
               info.RtrChain[i] = A_GET_4B(Er->RouterId);
               info.IfIndex[i] = A_GET_4B(Er->IfIndex);
            }
         }
#endif
      }
      p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, RTB_ENTRY_INFO, (u_OspfTrap*)&info);
   }

   /* if a network or ASBR route has been */
   /* removed, Summary_LSA has to be flushed   */
   if(((p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER) ||
       (p_Rte->DestinationType >= DEST_TYPE_IS_NETWORK)) &&
      ((p_Rte->PathType == OSPF_INTRA_AREA) ||
       (p_Rte->PathType == OSPF_INTER_AREA)))
         ReorgSumLsa(p_RTO, p_Rte, RTE_REMOVED);
   
   /* send notification to the user */
   if(p_RTO->Clbk.p_RoutingTableChanged)
   {
      rteCost = (((p_Rte->PathType == OSPF_TYPE_2_EXT) || 
                 (p_Rte->PathType == OSPF_NSSA_TYPE_2_EXT)) ? 
                 p_Rte->Type2Cost : p_Rte->Cost);

      p_RTO->Clbk.p_RoutingTableChanged(p_RTO->Clbk.f_RtmAsyncEvent[0] ? 
                       (t_Handle)p_RTO->Clbk.f_RtmAsyncEvent : p_RTO->MngId, 
                       p_Rte->DestinationType, RTE_REMOVED, 
                       p_Rte->DestinationId, p_Rte->IpMask, 
                       p_Rte->PathNum, rteCost, p_Rte->PathPrm, p_Rte->PathType,
                       p_Rte->isRejectRoute);
   }

   return FALSE;      /* Delete route from OSPF routing table */
}




/*********************************************************************
 * @purpose              Copy paths from source RTE to dest RTE and
 *                       assign the originator router id and forwarding
 *                       address(if any defined)
 *
 *
 * @param p_Rte           @b{(input)}  Destination RTB entry
 * @param p_ForwardRtr    @b{(input)}  Source RTB entry
 * @param OrgAdr          @b{(input)}  Originator Address
 * @param ForwAdr         @b{(input)}  Forwarding Address  - only set if the external LSA
 *                   specified a forwarding address and if that forwarding
 *                   address is on a local subnet. In this case, the forwarding
 *                   address is used as the next hop rather than the next hop
 *                   to the forwarding address.
 *
 * @returns               returns number of copied paths.
 *
 * @notes           Consider a next hop unique if either the outgoing interface
 *                  or the next hop IP address is different. 
 *
 * @end
 * ********************************************************************/
word PathCopy(t_RTO *p_RTO, t_RoutingTableEntry *p_Rte, 
              t_RoutingTableEntry *p_ForwardRtr, 
              SP_IPADR OrgAdr, SP_IPADR ForwAdr)
{
#if L7_OSPF_TE
   t_ErPrm  *SrcEr, *DstEr, *LastEr;
#endif
   t_OspfPathPrm *SrcPath, *DstPath;
   t_OspfPathPrm *prevPath = NULL;
   t_OspfPathPrm *NewPath;
   word num = 0;
   int result = 0;

   /* Only add new path if destination route is not already at the
    * maximum number of paths. */
   if (p_Rte->PathNum >= p_RTO->Cfg.MaximumPaths)
   {
       return 0;
   }

   SrcPath = p_ForwardRtr->PathPrm; 
   DstPath = p_Rte->PathPrm;
   while (SrcPath && (p_Rte->PathNum < p_RTO->Cfg.MaximumPaths))
   {
     if (DstPath)
     {
       result = memcmp(&SrcPath->NextHop, &DstPath->NextHop, 8);
        while (result > 0)
        {
          prevPath = DstPath;
          DstPath = DstPath->next;
          if (DstPath == NULL)
            break;
          result = memcmp(&SrcPath->NextHop, &DstPath->NextHop, 8);
        }
     }

      /* At this point, DstPath points to the first path whose key is > SrcPath, 
       * or NULL, if the SrcPath is greater than all keys in DstPath. prevPath 
       * points to the last DstPath whose key is < SrcPath. */
      /* If result == 0, paths are the same. Don't add duplicate. */
      if (!DstPath || (result < 0))
      {
        /* Create new path for destination route */
        if ((NewPath = XX_Malloc(sizeof(t_OspfPathPrm))) == NULL)
        {
          if (!p_RTO->LsdbOverload)
            EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
          return 0;
        }
        memset(NewPath, 0, sizeof(t_OspfPathPrm));
    
          /* Copy Path parameters */
          if(ForwAdr)
            NewPath->IpAdr = ForwAdr;
        else
            NewPath->IpAdr = SrcPath->IpAdr;    
  
        NewPath->NextHop = SrcPath->NextHop;          
        NewPath->AdvertisingRouter = OrgAdr ? OrgAdr : SrcPath->AdvertisingRouter;
    
#if L7_OSPF_TE
          /* Copy ER parameters */
          for(SrcEr = SrcPath->ErList; SrcEr; SrcEr = SrcEr->next)
          {
             if((DstEr = XX_Malloc(sizeof(t_ErPrm))) == NULL)
                return 0;
             
             DstEr->next = NULL;
             memcpy(DstEr->IfIndex, SrcEr->IfIndex, 4);
             memcpy(DstEr->RouterId, SrcEr->RouterId, 4);
             
             if(!NewPath->ErList)
             {
                /* first entry */
                NewPath->ErList = DstEr; 
             }
             else
             {
                /* find the last entry */
                for(LastEr = NewPath->ErList; LastEr->next; LastEr = LastEr->next);
                
                LastEr->next = DstEr;
             }
             p_Rte->ErNum ++;
          }
#endif
    
          /* Insert new path in sorted order */
           NewPath->next = DstPath;
           if (prevPath)
           {
             prevPath->next = NewPath;
           }
           else
           {
             /* Only path. Set list head. */
             p_Rte->PathPrm = NewPath;
           }
           prevPath = NewPath;
          ASSERT(p_Rte->PathPrm != p_Rte->PathPrm->next);
          p_Rte->PathNum ++;
          num ++;
      }
          
    
      /* If a forwarding address was specified, we only add a single path,
       * with the forwarding address as the next hop. */
      if (ForwAdr == 0)
          SrcPath = SrcPath->next;
      else 
          SrcPath = NULL;
   }

   return num;
}


/* --------------------------------------------------------------- */
/* --------------------------------------------------------------- */
/* ---- internal routines ---------------------------------------- */
/* --------------------------------------------------------------- */
/* --------------------------------------------------------------- */

/*********************************************************************
 * @purpose      A wrapper around RTB_EntireTableCalc() used when an SPF
 *               event is put on OSPF's message queue. 
 *
 *
 * @param p_info     @b{(input)}  Routing Table object handler
 *
 * @returns          E_OK       - calculation passed successful,
 * @returns          E_NOMEMORY - fail in memory allocation
 * @returns          E_FAILED   - critical error in SPF calculation
 *                                algorithm
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err RTB_EntireTableCalcCb(void *p_Info)
{
  t_XXCallInfo *callInfo  = (t_XXCallInfo *) p_Info;
  t_RTB *p_RTB = (t_RTB *) PACKET_GET(callInfo,0);
  if (p_RTB == NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
           "Invalid RTB pointer.");
    return E_FAILED;
  }

  return RTB_EntireTableCalc(p_RTB);
}

/*********************************************************************
 * @purpose          Calculate entire Router's routing table.
 *
 * @param p_info     @b{(input)}  Routing Table object handler
 *
 * @returns          E_OK       - calculation passed successful,
 * @returns          E_NOMEMORY - fail in memory allocation
 * @returns          E_FAILED   - critical error in SPF calculation
 *                                algorithm
 *
 * @notes      If this router is in a graceful restart, only compute
 *             intra-area routes and check vlink status. Need to try to
 *             bring virtual links up, but not going to send routes to
 *             RTO, so there's no reason to compute inter-area or 
 *             external routes.
 *
 * @end
 * ********************************************************************/
e_Err RTB_EntireTableCalc(t_RTB *p_RTB)
{
   t_RTO *p_RTO = (t_RTO *) p_RTB->RTO_Id;
   t_ARO *p_ARO = NULL;
   e_Err e;
   e_Err er = E_OK;
   Bool  IsBorder = FALSE;   
   t_IFO *p_tmpIFO = NULL;
   t_Handle tmp = p_RTB->RtbRtBtOld;
   L7_uint32 spfStart = osapiTimeMillisecondsGet();

   p_RTB->RtComputeTime = GET_SYS_CLOCK;

   /* Check if there is no area on the router, do nothing */
   if(p_RTO->AreasNum == 0)
      return E_OK;

   /* do not start an SPF till route updation from the last SPF is done */   
   if(p_RTB->RtbUpdateState != RTB_UPD_DONE) 
   {
     /* Schedule an event, this gives OSPF a chance to complete the update */
     if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF)
     {
       char traceBuf[OSPF_MAX_TRACE_STR_LEN];
       sprintf(traceBuf, "OSPF RTB_EntireTableCalc pending while RTB_Update is in state %s",
         rtbUpdateStates[p_RTB->RtbUpdateState]);
       RTO_TraceWrite(traceBuf);
     }

     p_RTB->spfWaitForRto++;
     if (!TIMER_Active(p_RTB->RecalcTimer))
     {
       /* Start recalculation timer */
       TIMER_StartSecMQueue(p_RTB->RecalcTimer, RTO_UPDATE_BACKOFF,
                            0, RecalcTimerExp, p_RTO->OspfRtbThread.threadHndle, OSPF_EVENT_QUEUE);
     }
     return E_OK;
   }

   p_RTB->RtbRtBtOld = p_RTB->RtbRtBt;
   p_RTB->RtbRtBt = tmp;

   tmp = p_RTB->RtbNtBtOld;
   p_RTB->RtbNtBtOld = p_RTB->RtbNtBt;
   p_RTB->RtbNtBt = tmp;

   /* Changes to fix defect 7857 */
   IsBorder = routerIsBorder(p_RTO) ? TRUE : FALSE;

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF)
   {
       char traceBuf[OSPF_MAX_TRACE_STR_LEN];
       sprintf(traceBuf, "Recomputing all OSPF routes");
       RTO_TraceWrite(traceBuf);
   }

   p_RTB->Backbone = NULL;
   /* Browse all areas */
   for (e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
        e == E_OK;
        e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO))
   {   
      /* if the area is not operational or there are no active interfaces on 
      ** the area go to the next area 
      */
      if(!p_ARO->OperationState || (HL_GetFirst(p_ARO->IfoHl, (void *)&p_tmpIFO) != E_OK))
      {
          /* Make sure SPT is cleared for this area. VlinkStatusUpdate() uses the
           * SPT when looking for a route to the far end of a vlink. */
          RTB_ClearSpt(p_ARO);
          RTB_ClearCandidateHl(p_ARO);
          p_ARO->TransitCapability = FALSE;
          continue;
      }

      /* Delay the bacbone area calculation to be last */
      if(p_ARO->AreaId == OSPF_BACKBONE_AREA_ID)
      {
         p_RTB->Backbone = p_ARO;
         continue;
      }

   /*  (2)  The intra-area routes are calculated by building the shortest-   */
   /* path tree for each attached area.  In particular, all routing          */
   /* table entries whose Destination  Type is  "area border router" are     */
   /* calculated in this step.  This step is described in two  parts.        */
   /* At first the tree is constructed by only considering those links       */
   /* between  routers  and transit networks.  Then the  stub networks       */
   /* are incorporated into the tree.  During the area's shortest-path       */
   /* tree calculation, the area's TransitCapability is also                 */
   /* calculated for later use in Step 4.                                    */
      if((er = SpfIntraAreaCalc(p_RTB, p_ARO)) != E_OK)
         goto rtb_exit;

      /* for regular router only one area exist */
      if(!IsBorder)
         break;

   }         

   if(p_RTB->Backbone)
   {
      /* Now calculate backbone SPF */
      p_ARO = p_RTB->Backbone;
      if((er = SpfIntraAreaCalc(p_RTB,p_RTB->Backbone)) != E_OK)
         goto rtb_exit;
   }

   if (!o2GracefulRestartInProgress(p_RTO))
   {
   /* Determine the active Area-Aggregation-Ranges.
    * "Active" means that there are one or more reachable (by intra-area paths)
    * networks contained in the address range.
    *
    * We are determining the active ranges at this step so that the inter-area
    * routes calculation that follows this step can ignore summary LSAs whose
    * destination matches an active range. See RFC 2328 para 16.2 (3)
    */
   DetermineActiveRanges(p_RTB);

   if(!IsBorder || p_RTB->Backbone)
   {
      /*  (3)  The inter-area routes are calculated, through examination of */
      /* summary-LSAs.  If the router is  attached to multiple areas        */
      /* (i.e., it is an area border router), only backbone summary-LSAs    */
      /* are examined.                                                      */
      er = InterAreaCalc(p_RTB, p_ARO, 0, 0); 
      if(er != E_OK)
        goto rtb_exit;
      
   }
   /*  (4)  In area  border routers connecting to one or more transit areas */
   /* (i.e, non-backbone areas whose TransitCapability is found to be       */
   /* TRUE), the transit areas' summary-LSAs are examined to see            */
   /* whether  better paths exist using the transit areas than were         */
   /* found in Steps 2-3 above.                                             */

   if(IsBorder)
   {
       for (e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
            e == E_OK;
            e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO))
       {
         if(p_ARO->TransitCapability)
         {
            er = TransitAreaExam(p_RTB, p_ARO, 0, 0);
            if(er != E_OK)
               goto rtb_exit;
         }    

         /* If we happen to be NSSA ABR, we need to re-elect the nssa translator,
          * as the intra-area reachability to other nssa abrs in this area
          * might have changed after spf */
         NssaTranslatorElect(p_ARO, 0);
       }
   }

   /* (5) Routes to external destinations are calculated,   through     */
   /* examination of AS-external-LSAs.  The locations of the AS       */
   /* boundary routers (which originate the AS-external-LSAs)  have    */
   /* been determined   in steps 2-4.                                   */
   AsExtCalc(p_RTB, 0);

   if(IsBorder && p_RTB->Backbone)
   {
      /* Browse all areas */
      for (e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
           e == E_OK;
           e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO))
      {
         if(p_ARO->AreaId == OSPF_BACKBONE_AREA_ID)
            continue;

         if(p_ARO->TransitCapabilityOld != p_ARO->TransitCapability)
         {
            /* There is a change in the transitcapability of this area */
            if(p_ARO->TransitCapability)
            {
               /* Area has become a transit-area
                * Don't suppress the summary lsas into this area */
               DeliverSummariesOfBackbone(p_RTB->Backbone, p_ARO);
            }
            else
            {
               /* Area is no longer a transit-area
                * Condense the summary lsas into this area */
               HideSummariesOfBackbone(p_RTB->Backbone, p_ARO);
            }
         }
      }
   }
   }

rtb_exit:   
   /* Evaluate Virtual link status */
   if(p_RTB->Backbone)
     HL_Browse(((t_ARO *)p_RTB->Backbone)->IfoHl, VlinkStatusUpdate, (ulng)p_RTB);

   /* Schedule an event to check the new constracted routing table 
      against the old one and find all changed  entries */
   {
     t_XXCallInfo *callData = 0;
     PACKET_INIT(callData, RTB_RtoUpdate, 0, 0, 1, (ulng)p_RTB);
     ASSERT(callData);

     p_RTB->RtbUpdateState = RTB_UPD_START;
     if (XX_Call(p_RTO->OspfRtbThread.threadHndle, callData) != E_OK)
     {
       L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
               "Failed to update routing table update after SPF.");
        /* Won't finish RTO update. Forwarding table may be incomplete. Go into overload. */
        if (!p_RTO->LsdbOverload)
          EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
     }
   }

   p_RTB->RtComputeTime = GET_SYS_CLOCK - p_RTB->RtComputeTime;
   p_RTB->AroId = NULL;
   p_RTB->SumType = 0;
   p_RTB->NextCompItem = NULL;
   p_RTB->Backbone = NULL;

   p_RTB->tsSpf = osapiUpTimeRaw();

   RTB_SpfStatsUpdate(p_RTB, spfStart);

   RTO_VerifyForwardingAddrs(p_RTO);

   return er;

}



/*********************************************************************
 * @purpose            Routing table incremental updates upon receiving a
 *                     new summary LSA.
 *
 * @param p_RTB        @b{(input)}  Routing Table object handler
 * @param p_ARO        @b{(input)}  Area where the summary LSA was received
 * @param p_DbEntry    @b{(input)}  LSA's database entry
 *
 * @returns            E_OK - calculation passed successful,
 * @returns            E_NOMEMORY - fail in memory allocation
 * @returns            E_FAILED - critical error in SPF calculation algorithm
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err SumIncrementalCalc(t_RTB *p_RTB, t_ARO *p_ARO, t_A_DbEntry *p_DbEntry)
{
   t_RTO *p_RTO = (t_RTO *) p_RTB->RTO_Id;
   t_S_SummaryLsa *p_Sum;
   t_RoutingTableKey rtbkey;
   t_RoutingTableEntry *p_Rte = NULL;
   t_RoutingTableEntry *p_NewRte = NULL;
   t_AgrEntry *p_AdrRange;
   t_ARO *p_tmpAro;
   e_Err e = E_FAILED;
   Bool to_be_removed = 0;
   Bool identicalRoute = FALSE;     
   Bool IsBorder;
   e_OspfPathType ptype;
   t_RoutingTableEntry *p_backupRte = NULL; 
   t_Handle routingTable;

   /* Delay calculation during starting silence interval */
   if(p_RTB->FirstCalc)
      return E_OK;

   /* If a full SPF is scheduled, don't bother with incremental. */
   if (TIMER_Active(p_RTB->RecalcTimer))
     return E_OK;

   /* If in the midst of a graceful restart, no need to compute inter-area route */
   if (o2GracefulRestartInProgress(p_RTO))
     return E_OK;

   /* If route update from the last SPF is not done, 
    * skip the incremental, schedule a full SPF.
    */   
   if(p_RTB->RtbUpdateState != RTB_UPD_DONE)
   {
     RTB_ComputeCalcDelay(p_RTB, p_RTO);
     TIMER_StartSec(p_RTB->RecalcTimer, p_RTB->CalcDelay,
                    0, RecalcTimerExp, p_RTO->OspfRtbThread.threadHndle);
     return E_OK;
   }
   else
   {
     /* If we get a lot of summary LSAs at once, do a full calculation
      * rather than an incremental for every one. */
     p_RTB->IncSumCalcNum++;
     if (p_RTB->IncSumCalcNum >= OSPF_MAX_SUM_INCREMENTALS)
     {
       if (p_DbEntry->Lsa.LsType == S_IPNET_SUMMARY_LSA)
         RTB_AddSpfReason(p_RTB, R_SUMMARY_LSA);
       else 
         RTB_AddSpfReason(p_RTB, R_ASBR_SUM_LSA);
       RTB_ComputeCalcDelay(p_RTB, p_RTO);
       TIMER_StartSec(p_RTB->RecalcTimer, p_RTB->CalcDelay,
                        0, RecalcTimerExp, p_RTO->OspfRtbThread.threadHndle);
       p_RTB->IncSumCalcNum = 0;
     }
   }

   IsBorder = routerIsBorder(p_RTO) ? TRUE : FALSE;

   if(! (L7_BIT_ISSET(p_DbEntry->dbFlags, O2_DB_ENTRY_IS_CUR)))
      return E_FAILED;

   p_RTB->SumIncComputeTime = GET_SYS_CLOCK;

   p_Sum = (t_S_SummaryLsa *) p_DbEntry->p_Lsa;

   rtbkey.DstAdr = A_GET_4B(p_DbEntry->Lsa.LsId);
   if (p_DbEntry->Lsa.LsType == S_IPNET_SUMMARY_LSA)
   {
     routingTable = p_RTB->RtbNtBt;
     rtbkey.Prefix =  A_GET_4B(p_Sum->NetworkMask);
     if (rtbkey.Prefix)
     {
       rtbkey.DstAdr &= rtbkey.Prefix;
     }
   }
   else if (p_DbEntry->Lsa.LsType == S_ASBR_SUMMARY_LSA)
   {
     routingTable = p_RTB->RtbRtBt;
     rtbkey.Prefix = SPF_HOST_ROUTE;
   }
   else
   {
     /* invalid LSA type */
     return E_FAILED;
   }

   /* Find existing route */
   p_Rte = o2RouteFind(p_RTB, routingTable, rtbkey.DstAdr, rtbkey.Prefix);                     

   /* If there is already an intra-area route to the destination, no need
    * to recompute inter-area routes. */
   if (p_Rte && (p_Rte->PathType == OSPF_INTRA_AREA) &&
       !(p_ARO->TransitCapability && IsBorder))
   {
     return E_OK;
   }

   /* If the collection of destinations described by the Summary LSA equals one of the
    * router's configured area address ranges (see Section 3.5), and the particular
    * area address range is active, then the summary-LSA should be ignored. "Active" means
    * that there are onr or more reachable (by intra-area paths) networks contained in the
    * area range.
    */
   if ((HL_FindFirst(p_ARO->AdrRangeHl, (byte*)&rtbkey.DstAdr, 
                                        (void *)&p_AdrRange) == E_OK) &&
       (p_AdrRange->LsdbType == AGGREGATE_SUMMARY_LINK) &&  
       (p_AdrRange->AggregateStatus == ROW_ACTIVE) && 
       (p_AdrRange->alive) && (routerIsBorder(p_RTO)))
   {
      return E_OK;
   }

   if(p_Rte != NULL)
   {
      /* Make a copy of the next hops in the existing route. */
      if((p_backupRte = XX_Malloc(sizeof(t_RoutingTableEntry))) == NULL)
      {
        if (!p_RTO->LsdbOverload)
          EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
        return E_NOMEMORY;
      }

      memset(p_backupRte, 0, sizeof(t_RoutingTableEntry));
      PathCopy(p_RTO, p_backupRte, p_Rte, 0, 0);
   }

   /* Case 1: Summary LSA received in the backbone area or the router 
    * is not area border router */
   if((p_ARO->AreaId == OSPF_BACKBONE_AREA_ID) || !IsBorder)
   {
      /* First invalidate the matching inter-area entry in RTB if one exists */
      if(p_Rte && (p_Rte->PathType == OSPF_INTER_AREA))
      {
         /* Remove the existing route from the OSPF routing table, but 
          * do not free the route. */
         RtbRteDelete(p_RTB, p_Rte);
         to_be_removed = 1;
      }
      
      /* run the inter-area recalculation for single destination DstAdr */
      InterAreaCalc(p_RTB, p_ARO, &rtbkey, 1);

       /* In addition, if the router is an area border router attached to */
       /* one or more transit areas, the calculation in Section 16.3      */
       /* must be run again for the single destination.                   */

      if(IsBorder)
      {
          for (e = HL_GetFirst(p_RTO->AroHl, (void *)&p_tmpAro);
               e == E_OK;
               e = HL_GetNext(p_RTO->AroHl, (void *)&p_tmpAro, p_tmpAro))
          {
            if(p_tmpAro->TransitCapability)
               TransitAreaExam(p_RTB, p_tmpAro, &rtbkey, 1);       
          }
      }

      /* See if a new route was computed to the destination */  
      p_NewRte = o2RouteFind(p_RTB, routingTable, rtbkey.DstAdr, rtbkey.Prefix);
   }

   /* Case 2: Area is the transit area and the router is area border router */
   else if(p_ARO->TransitCapability && IsBorder)
   {
       /* First, if N's routing table  entry presently   contains one or */
       /* more inter-area paths that utilize the transit area  Area A,   */
       /* these paths   should be removed. If this removes all paths     */
       /* from the routing table entry, the entry should be              */
       /* invalidated.                                                   */
       if(p_Rte && (p_Rte->PathType == OSPF_INTER_AREA))
       {           
         /* If PrunePaths() removes all next hops, then it also removes the
          * route from the OSPF routing table. */
         if (PrunePaths(p_RTB, p_Rte, p_ARO->AreaId, pruneArea) == E_OK) 
         {
           if (p_Rte->PathPrm == NULL) 
           {
             to_be_removed = 1;
           }
         }
       }

     /* Next the calculation in Section 16.3 must                      */
     /* be run again for the single   destination N. If the results of */    
     /* this calculation have caused the cost to N to increase, the    */ 
     /* complete routing table calculation must be rerun starting      */
     /* with the Dijkstra algorithm   specified in Section 16.1.       */
      for (e = HL_GetFirst(p_RTO->AroHl, (void *)&p_tmpAro);
           e == E_OK;
           e = HL_GetNext(p_RTO->AroHl, (void *)&p_tmpAro, p_tmpAro))
      {
         if(p_tmpAro->TransitCapability)
            TransitAreaExam(p_RTB, p_tmpAro, &rtbkey, 1);       
       }
        
       p_NewRte = o2RouteFind(p_RTB, routingTable, rtbkey.DstAdr, rtbkey.Prefix);
       if(!p_NewRte || (p_Rte && (p_NewRte->Cost > p_Rte->Cost)))
       {  
         /* Free previously invalidated RTB entry */
         if (p_Rte && to_be_removed)
         {
            RteRemoved(p_RTB, p_Rte);
         }

         /* Start recalculation timer */
         if (p_DbEntry->Lsa.LsType == S_IPNET_SUMMARY_LSA)
           RTB_AddSpfReason(p_RTB, R_SUMMARY_LSA);
         else 
           RTB_AddSpfReason(p_RTB, R_ASBR_SUM_LSA);
         if(!TIMER_Active(p_RTB->RecalcTimer))
         {
             RTB_ComputeCalcDelay(p_RTB, p_RTO);
             TIMER_StartSec(p_RTB->RecalcTimer, p_RTB->CalcDelay,
                            0, RecalcTimerExp, p_RTO->OspfRtbThread.threadHndle);
         }
        
         p_RTB->SumIncComputeTime = GET_SYS_CLOCK - p_RTB->SumIncComputeTime;

         if(p_backupRte)
         {
            RteRelease(p_backupRte, TRUE);
         }

         return E_OK;         
       } 
   }
   else
   {
      p_RTB->SumIncComputeTime = GET_SYS_CLOCK - p_RTB->SumIncComputeTime;

      if(p_backupRte)
         RteRelease(p_backupRte, TRUE);

      return E_OK;
   }

   ptype = p_Rte ? p_Rte->PathType : 0;

   if (p_NewRte && p_Rte)
   {
       if ((RTB_RoutesEqual(p_Rte, p_NewRte) == FALSE) ||
           (PathCompare(p_NewRte, p_backupRte) == FALSE))
       {
           RteChanged(p_RTB, p_Rte, p_NewRte);
       }
       else
       {
           identicalRoute = TRUE;
       }
       if (to_be_removed == 1)
       {
           /* We removed p_Rte from the routing table already. Now free it. */
           RteRelease(p_Rte, TRUE);
       }
   }
   else if (p_NewRte && !p_Rte)
   {
      RteAdded(p_RTB, p_NewRte);
   }
   else if (!p_NewRte && p_Rte)
   {
      /* Remove the route from the OSPF routing table and from RTO. */
      RtbRteDelete(p_RTB, p_Rte);
      RteRemoved(p_RTB, p_Rte);
   }

   /* Free our temporary copy of the original route. */
   if (p_backupRte)
   {
      RteRelease(p_backupRte, TRUE);
   }

   /* If the results of these calculations have changed the cost/path to */
   /* an AS boundary router (as would be the case for a Type 4           */
   /* summary-LSA) or to any forwarding addresses, all AS-               */
   /* external-LSAs will have to be reexamined by rerunning the          */
   /* calculation in Section 16.4.                                       */
   /* Run SPF whether dest is network or ASBR to catch changes to forwarding
    * addresses and in case an inter-area route was masking an external route. */
   if (identicalRoute == FALSE)
   {
     /* Start recalculation timer to recompute all routes, including external 
      * routes. Calling AsExtCalc() directly won't work because AsExtCalc() by
      * itself does not update RTO. */
     if (p_DbEntry->Lsa.LsType == S_IPNET_SUMMARY_LSA)
       RTB_AddSpfReason(p_RTB, R_SUMMARY_LSA);
     else 
       RTB_AddSpfReason(p_RTB, R_ASBR_SUM_LSA);
     if(!TIMER_Active(p_RTB->RecalcTimer))
     {
         RTB_ComputeCalcDelay(p_RTB, p_RTO);
        TIMER_StartSec(p_RTB->RecalcTimer, p_RTB->CalcDelay,
           0, RecalcTimerExp, p_RTO->OspfRtbThread.threadHndle);
     }
   }
      
   p_RTB->SumIncComputeTime = GET_SYS_CLOCK - p_RTB->SumIncComputeTime;

   return E_OK;
}

/*********************************************************************
 * @purpose          Routing table incremental updates upon receiving a
 *                   new AS external LSA.
 *
 * @param p_RTB      @b{(input)}  Routing Table object handler
 * @param p_DbEntry  @b{(input)} LSA's database entry
 *
 * @returns          E_OK - calculation passed successful,
 * @returns          E_NOMEMORY - fail in memory allocation
 * @returns          E_FAILED - critical error in SPF calculation algorithm
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err AsExtIncrementalCalc(t_RTB *p_RTB, t_A_DbEntry *p_DbEntry)
{
   t_S_AsExternalLsa *p_AsExt;
   t_RoutingTableEntry *oldRoute = NULL;   /* existing route to LSA's destination */
   t_RoutingTableEntry *newRoute = NULL;  /* new route after processing LSA */
   e_Err er = E_OK;
   t_RoutingTableKey rtbkey;
   t_ARO  *p_ARO = NULLP;
   t_RTO  *p_RTO = (t_RTO *)p_RTB->RTO_Id;

   /* Delay calculation during starting silence interval */
   if(p_RTB->FirstCalc)
      return E_OK;

   /* If a full SPF is scheduled, don't bother with incremental. */
   if (TIMER_Active(p_RTB->RecalcTimer))
     return E_OK;

   /* If router is restarting, don't bother computing external route */
   if (o2GracefulRestartInProgress(p_RTO))
     return E_OK;

   /* If route updation from the last SPF is not done, 
    * skip the incremental, schedule a full SPF.
    */   
   if(p_RTB->RtbUpdateState != RTB_UPD_DONE)
   {
     RTB_ComputeCalcDelay(p_RTB, p_RTO);
     TIMER_StartSec(p_RTB->RecalcTimer, p_RTB->CalcDelay,
                    0, RecalcTimerExp, p_RTO->OspfRtbThread.threadHndle);
     return E_OK;
   }

   if(! (L7_BIT_ISSET(p_DbEntry->dbFlags, O2_DB_ENTRY_IS_CUR)))
      return E_FAILED;

   p_RTB->AsExtIncComputeTime = GET_SYS_CLOCK;

   rtbkey.DstAdr = A_GET_4B(p_DbEntry->Lsa.LsId);

   if(p_DbEntry->Lsa.LsType == S_NSSA_LSA)
   {
     p_ARO = (t_ARO *)p_DbEntry->ARO_Id;
   }

     p_AsExt = (t_S_AsExternalLsa *)p_DbEntry->p_Lsa;
     rtbkey.Prefix =  A_GET_4B(p_AsExt->NetworkMask);
   if (rtbkey.Prefix)
   {
      rtbkey.DstAdr &= rtbkey.Prefix;
   }
    
   /* If there is already an intra-area or inter-area route to the     */
   /* destination, no recalculation is necessary (internal routes take */
   /* precedence).                                                     */
   oldRoute = o2RouteFind(p_RTB, p_RTB->RtbNtBt, rtbkey.DstAdr, rtbkey.Prefix);
   if (oldRoute && ((oldRoute->PathType == OSPF_INTRA_AREA) ||
                    (oldRoute->PathType == OSPF_INTER_AREA)))
   {
      er = E_OK;
   }
   else 
   {
     if (oldRoute)
     {
         /* Remove previous route from OSPF routing table. AsExtCalc() will 
          * reconsider all type 5 LSAs to the destination. This route
          * may be associated with an LSA that is no longer in the db.
          * Note that this does not inform RTO of the removal. That 
          * will be done when we know the final result of the incremental
          * calculation. 
          */
        if (oldRoute->DestinationType >= DEST_TYPE_IS_NETWORK)         
           o2RouteDelete(p_RTB, p_RTB->RtbNtBt, rtbkey.DstAdr, rtbkey.Prefix, FALSE);
        else
           o2RouteDelete(p_RTB, p_RTB->RtbRtBt, rtbkey.DstAdr, rtbkey.Prefix, FALSE);
     }

   /* Otherwise, the procedure in Section 16.4 will have to be         */
   /* performed, but only for those AS-external-LSAs whose destination */
   /* is N.                                                            */
 
      er = AsExtCalc(p_RTB, &rtbkey);   
 
      /* Now determine the final result and inform RTO. */ 
      newRoute = o2RouteFind(p_RTB, p_RTB->RtbNtBt, rtbkey.DstAdr, rtbkey.Prefix);
      if(!newRoute)
      {
         if(oldRoute)
         {
            RteRemoved(p_RTB, oldRoute);
         }
      }
      else if (!oldRoute)
      {
         /* Add new ASBR */
         RteAdded(p_RTB, newRoute);
      }
      else
      {
        if (RTB_RoutesEqual(newRoute, oldRoute))
        {
          /* Routes are the same. Retain the old route object because 
           * there may be references to it, like t_AgrEntry.LcRte. */
          o2RadixChange(p_RTB, p_RTB->RtbNtBt, oldRoute);
        }
        else
        {
          /* routes are different. replace old with new. */
          RteChanged(p_RTB, oldRoute, newRoute);
          RteRelease(oldRoute, TRUE);
        }
      }
   }       

   p_RTB->AsExtIncComputeTime = GET_SYS_CLOCK - p_RTB->AsExtIncComputeTime;
   
   return er;
}



/*********************************************************************
 * @purpose          The procedure checks the specified Summary_Lsa according
 *                   to RFC 2328 section 16.4 items (1) - (3).
 *
 * @param p_RTB      @b{(input)}  routing table handle
 * @param p_DbEntry  @b{(input)}  the proccesed Summary-Lsa Db entry
 *
 * @param            @b{(output)} p_BorderRtr   pointer to RT entry of
 *                   the ASBR or forwarding address,
 *                   (selected according to Section 16.4.1)
 *
 * @returns          E_OK  routing table entry is processed OK
 * @returns          E_NOMEMORY - no memory
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err static AsExtVerifyLsa(t_RTB *p_RTB, t_A_DbEntry *p_DbEntry,
                            t_RoutingTableEntry **p_PrefRte)
{
   t_RTO *p_RTO = (t_RTO *) p_RTB->RTO_Id;
   t_S_AsExternalLsa *p_AsExtLsa;
   t_RoutingTableEntry *p_AsBr = NULLP, *p_PreferedRoute = NULLP;
   t_ARO *p_ARO;
   SP_IPADR forwardingAddr = 0;

   p_AsExtLsa = (t_S_AsExternalLsa *)p_DbEntry->p_Lsa;

   /* (1) If the cost specified by the LSA is LSInfinity, or if the */
   /* LSA's LS age is equal to MaxAge, then examine the the next LSA */      
   if((A_GET_3B(p_AsExtLsa->Metric) >= LSInfinity) ||
      (A_GET_2B(p_DbEntry->Lsa.LsAge) == MaxAge))
      return E_FAILED;                           

   /* (2) If the LSA was originated by the calculating router */
   /* itself, examine the next LSA.                           */
   if(A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter) == p_RTO->Cfg.RouterId)
      return E_FAILED;

   /* ignore AS-External-LSAs from other routers with us as the FA */
   forwardingAddr = A_GET_4B(p_AsExtLsa->ForwardingAddress);
   if (forwardingAddr)
     if (RTO_AddressIsLocallyConfigured(p_RTO, &forwardingAddr) == TRUE)
       return E_FAILED;
   
   /* Look up the routing table entries for the ASBR that originated */
   /* the LSA. If the ASBR is unreachable, do nothing with this LSA  */
   p_AsBr = o2RouteFind(p_RTB, p_RTB->RtbRtBt, A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter), 
                        SPF_HOST_ROUTE);
   if ((!p_AsBr) || !(p_AsBr->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER))
   {
      return E_FAILED;
   }

   /* NSSA CHANGE:  (Section 2.5 point (3)) Since the flooding scope of 
   ** a Type-7 LSA is restricted to the originating NSSA, the routing
   ** table entry of its ASBR must be found in the originating NSSA.
   ** If no entries exist for the ASBR (i.e. the ASBR is unreachable
   ** over the transit topology for a Type-5 LSA, or, for a Type-7 LSA, it
   ** is unreachable over the LSA's originating NSSA), do nothing with this
   ** LSA and consider the next in the list 
   */

   /* If this is a Type-7 LSA, verify that the advertising ASBR lies in the NSSA */
   if(p_DbEntry->Lsa.LsType == S_NSSA_LSA)
     if((HL_FindFirst(p_RTO->AroHl, (byte*)&p_AsBr->AreaId, (void *)&p_ARO) != E_OK) || 
        (p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA))
       return E_FAILED;

   /* Find preferable route to the ASBR or to the forwarding address */
   if(AsExtFindPreferableRoute(p_RTB, p_DbEntry, &p_PreferedRoute, forwardingAddr) != E_OK )
      return E_FAILED;
   
   if(p_PrefRte)
      *p_PrefRte = p_PreferedRoute;

   return E_OK;

}



/*********************************************************************
 * @purpose               Modifying the routing table entry as described in
 *                        RFC 2328 section 16.4 item (4) - (6).
 *
 * @param p_RTB           @b{(input)}  routing table handle
 * @param p_DbEntry       @b{(input)}  the proccesed Summary-Lsa Db entry
 * @param p_PreferRte     @b{(input)}  RT entry of the ASBR or
 *                                     forwarding address
 *
 * @returns               E_OK  routing table entry is processed OK
 * @returns               E_NOMEMORY - no memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err AsExtModifyRte(t_RTB *p_RTB, t_A_DbEntry *p_DbEntry,
                            t_RoutingTableEntry *p_PreferRte)
{

   t_RTO *p_RTO = (t_RTO *) p_RTB->RTO_Id;
   t_S_AsExternalLsa   *p_AsExtLsa;
   t_ARO               *p_ARO = NULLP;
   t_RoutingTableEntry *p_Rte;
   t_RoutingTableEntry *existingAsbrRoute;
   Bool existingRouteHasHighestPref;
   Bool newRouteHasHighestPref;
   Bool p_flagAsExt;
   ulng X;    /* the cost to the ASBR or forwarding address for the new T5 LSA */
   ulng Y;    /* the cost specified in the T5 LSA */
   ulng prevCost;
   t_S_AsExternalLsa* lsaForRoute;
   Bool  preferNewRte = FALSE;

   if (!p_PreferRte)
     return E_FAILED;

   /* forwarding address advertised in the LSA for the destination network. */
   SP_IPADR fwdingAddr = 0;

   /* Address to use as next hop to destination. Set to the forwarding
    * address if the forwarding address is on a local subnet. Otherwise,
    * the next hop address is copied from the route to the ASBR/forwarding address */
   SP_IPADR nextHopAddr = 0;
   SP_IPADR rtEntryFwdAddr;

   p_AsExtLsa = (t_S_AsExternalLsa *)p_DbEntry->p_Lsa;

   if(p_DbEntry->Lsa.LsType == S_NSSA_LSA)
     p_ARO     = (t_ARO *)p_DbEntry->ARO_Id;

   fwdingAddr = A_GET_4B(p_AsExtLsa->ForwardingAddress);

   if (p_PreferRte->DirAttachNet)
   {
      /* route is to forwarding address which is on a local subnet.
       * Use the forwarding address as the next hop address. */
       nextHopAddr = fwdingAddr;
   }

   /* (4) Let X be the cost specified by the preferred rte */
   /* and Y the cost specified in the LSA.                 */      
   X = p_PreferRte->Cost;
   Y = A_GET_3B(p_AsExtLsa->Metric);

   /* (5) Look up for the destination described into the AS Ext-LSA */
   p_Rte = o2RouteFind(p_RTB, p_RTB->RtbNtBt, A_GET_4B(p_DbEntry->Lsa.LsId), 
                       A_GET_4B(p_AsExtLsa->NetworkMask));
   if(p_Rte == NULL)
   {
      /* (5) No entry exists for the dest., install the new entry in the RTB */
      if((p_Rte = XX_Malloc(sizeof(t_RoutingTableEntry))) == NULL)
      {
        if (!p_RTO->LsdbOverload)
          EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
        return E_NOMEMORY;
      }
      memset(p_Rte, 0, sizeof(t_RoutingTableEntry));
         
      p_Rte->DestinationType = DEST_TYPE_IS_STUB_NETWORK;
      p_Rte->DestinationId   = A_GET_4B(p_DbEntry->Lsa.LsId);
      p_Rte->IpMask          = A_GET_4B(p_AsExtLsa->NetworkMask);
      if(p_Rte->IpMask)
         p_Rte->DestinationId &= p_Rte->IpMask;
      p_Rte->AreaId          = p_PreferRte->AreaId;
      p_Rte->p_DbEntry       = p_DbEntry;
         
      PathCopy(p_RTO, p_Rte, p_PreferRte, 
               A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter), nextHopAddr);

      /* set the cost value and path type */
      /* NSSA CHANGE: If this is a route learnt via an NSSA ASBR mark the
      ** route accordingly
      */
      if(p_AsExtLsa->Flags & S_AS_EXT_E_FLAG)
      {
         /* type 2 ext */
         p_Rte->PathType = (p_ARO && (p_ARO->ExternalRoutingCapability == AREA_IMPORT_NSSA)) ? 
           OSPF_NSSA_TYPE_2_EXT : OSPF_TYPE_2_EXT;
         p_Rte->Cost = X;
         p_Rte->Type2Cost = Y;
      }
      else
      {
         /* type 1 ext */
         p_Rte->PathType = (p_ARO && (p_ARO->ExternalRoutingCapability == AREA_IMPORT_NSSA)) ? 
           OSPF_NSSA_TYPE_1_EXT : OSPF_TYPE_1_EXT;
         p_Rte->Cost = (X + Y);
      }  
      if (p_DbEntry->Lsa.LsType == S_NSSA_LSA)
      {
        if (p_DbEntry->Lsa.Options & OSPF_OPT_P_BIT)
          p_Rte->pBitSet = TRUE;
        else
          p_Rte->pBitSet = FALSE;
      }
         
      RteUpdate(p_RTB, p_Rte);
      return E_OK;
   }
         
   /* (6) Route for the AS ext dest already exists. Compare the new */
   /* path with the existing path                                   */

   /* (a) Intra-area and inter-area paths are always preferred   */
   if((p_Rte->PathType == OSPF_INTRA_AREA) ||
      (p_Rte->PathType == OSPF_INTER_AREA))
      /* do nothing - the existing path is preferred */ 
      return E_OK;

   /* (b) Type 1 ext paths are always preferred over type 2 ext path.   */ 
   /* When all paths are type 2, the paths with the smallest advertised */
   /* type 2 metric are always preferred. */
   if(((p_Rte->PathType == OSPF_TYPE_1_EXT) ||
       (p_Rte->PathType == OSPF_NSSA_TYPE_1_EXT)) && 
       (p_AsExtLsa->Flags & S_AS_EXT_E_FLAG))
      /* do nothing - the existing path is preferred */ 
      return E_OK;

   if(((p_Rte->PathType == OSPF_TYPE_2_EXT) ||
       (p_Rte->PathType == OSPF_NSSA_TYPE_2_EXT)) &&
         !(p_AsExtLsa->Flags & S_AS_EXT_E_FLAG))
      /* the new path is preferred */
      goto replacePath;

   if(((p_Rte->PathType == OSPF_TYPE_2_EXT) ||
       (p_Rte->PathType == OSPF_NSSA_TYPE_2_EXT)) &&
        (p_AsExtLsa->Flags & S_AS_EXT_E_FLAG))
   {
      if(p_Rte->Type2Cost < Y)
         /* do nothing - the existing path is preferred */ 
         return E_OK;
      else if(p_Rte->Type2Cost > Y)
         /* the new path is better */
         goto replacePath;
   }
   
   /* (c) If the new AS external path is still indistinguishable from   */
   /* the current paths, and RFC1583Compatibility is set to "disabled", */
   /* select the preferred paths based on the intra-AS paths to the     */
   /* ASBR/frwrding addr, as specified in Section 16.4.1.               */

   /* First need to find the route to the ASBR or FA for the existing route */
   if (AsExtVerifyLsa(p_RTB, p_Rte->p_DbEntry, &existingAsbrRoute) != E_OK)
   {
     return E_FAILED;
   }
   if (AsExtPreferredPathFind(p_RTO, p_PreferRte, existingAsbrRoute, 
                               &preferNewRte, TRUE) == E_OK)
   {
      if (preferNewRte == TRUE)
         goto replacePath;
      else
         return E_OK;
   }
   
   /* (d) If the new AS ext path is still indistinguishable from the current */
   /* paths, select the preferred path based on a least cost comparison. */
   if ((p_Rte->PathType == OSPF_TYPE_1_EXT || 
      p_Rte->PathType == OSPF_NSSA_TYPE_1_EXT) &&
     !(p_AsExtLsa->Flags & S_AS_EXT_E_FLAG))
   {
      /* For type 1 ext paths the (X+Y) are compared. */
     if (p_Rte->Cost < (X + Y))            
       return E_OK;
     else if (p_Rte->Cost > (X + Y))
       goto replacePath;
   }

   if (((p_Rte->PathType == OSPF_TYPE_2_EXT) || 
        (p_Rte->PathType == OSPF_NSSA_TYPE_2_EXT)) &&
       (p_AsExtLsa->Flags & S_AS_EXT_E_FLAG))
   {
      /* Type 2 ext paths with equal type 2 metrics are compared by looking 
      * at the distance to the forwarding addresses. */
     if (existingAsbrRoute->Cost < X)            
       return E_OK;
     else if (existingAsbrRoute->Cost > X)  
         goto replacePath;         
   }

   /* (e) If the current LSA is functionally the same as an installed LSA (i.e., same */
   /* destination, cost and non-zero forwarding address) then apply the following     */
   /* priorities in deciding which LSA is preferred:                                  */
   /*       1. A Type-7 LSA with the P-Bit set                                        */
   /*       2. A Type-5 LSA                                                           */
   /*       3. The LSA with the higher router ID                                      */
   lsaForRoute = (t_S_AsExternalLsa *)p_DbEntry->p_Lsa;
   rtEntryFwdAddr = A_GET_4B(lsaForRoute->ForwardingAddress);

   if (fwdingAddr && rtEntryFwdAddr && (fwdingAddr == rtEntryFwdAddr))
   { 
       p_flagAsExt = (p_DbEntry->Lsa.Options & OSPF_OPT_P_BIT) ? TRUE : FALSE;

       existingRouteHasHighestPref = (p_Rte->pBitSet && 
                                      (p_Rte->p_DbEntry->Lsa.LsType == S_NSSA_LSA));
       newRouteHasHighestPref = (p_flagAsExt && (p_DbEntry->Lsa.LsType == S_NSSA_LSA));
       
       if (existingRouteHasHighestPref && !newRouteHasHighestPref)
         return E_OK;
       else if (!existingRouteHasHighestPref && newRouteHasHighestPref)
         goto replacePath;
    
       if((p_Rte->p_DbEntry->Lsa.LsType == S_AS_EXTERNAL_LSA) && 
          (p_DbEntry->Lsa.LsType != S_AS_EXTERNAL_LSA))
         return E_OK;
       else if ((p_Rte->p_DbEntry->Lsa.LsType != S_AS_EXTERNAL_LSA) &&
                (p_DbEntry->Lsa.LsType == S_AS_EXTERNAL_LSA))
         goto replacePath;
    
       if(A_GET_4B(p_Rte->p_DbEntry->Lsa.AdvertisingRouter) > 
          A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter))
         return E_OK;
       else if(A_GET_4B(p_Rte->p_DbEntry->Lsa.AdvertisingRouter) == 
               A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter))
           goto addASExtPath;
       else
           goto replacePath;
   }

   /* Routes still equal preference, so add new path to existing route. */
   goto addASExtPath;


replacePath:
   prevCost = p_Rte->Cost;
   p_Rte->p_DbEntry = p_DbEntry;
   p_Rte->AreaId    = p_PreferRte->AreaId;

   if(p_DbEntry->Lsa.LsType == S_NSSA_LSA)
   {
     p_Rte->PathType  = (p_AsExtLsa->Flags & S_AS_EXT_E_FLAG) ? 
       OSPF_NSSA_TYPE_2_EXT : OSPF_NSSA_TYPE_1_EXT;
     if (p_DbEntry->Lsa.Options & OSPF_OPT_P_BIT)
       p_Rte->pBitSet = TRUE;
     else
       p_Rte->pBitSet = FALSE;
   }
   else
   {
     p_Rte->PathType  = (p_AsExtLsa->Flags & S_AS_EXT_E_FLAG) ? OSPF_TYPE_2_EXT : OSPF_TYPE_1_EXT;
   }

     p_Rte->Cost      = (p_AsExtLsa->Flags & S_AS_EXT_E_FLAG) ? X : X+Y;
     p_Rte->Type2Cost = (p_AsExtLsa->Flags & S_AS_EXT_E_FLAG) ? Y : 0;

   RteRelease(p_Rte, FALSE); /* release recent paths */

addASExtPath:   
   if(p_Rte && (p_Rte->AreaId < p_PreferRte->AreaId))
     goto replacePath;

   if(!PathCopy(p_RTO, p_Rte, p_PreferRte, 
                A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter), nextHopAddr))
      return E_NOMEMORY;
      
   return E_OK;
}

/*********************************************************************
 * @purpose    AS-external routes calculation according to Section 16.4.
 *
 *
 * @param   p_RTB          @b{(input)}  Routing Table object handler
 * @param   SingleDestKey  @b{(input)}  key of a single destination -
 *                                      must be set to 0 when  the
 *                                      procedure is called for all
 *                                      destinations
 *
 * @returns                E_OK - calculation passed successful,
 * @returns                E_NOMEMORY - fail in memory allocation
 * @returns                E_FAILED   - critical error
 *
 *
 * @notes
 *                    This routine is also used to compute routes learnt
 *                    from Type-7 LSAs
 *
 * @end
 * ********************************************************************/
static e_Err AsExtCalc(t_RTB *p_RTB,
                       t_RoutingTableKey *SingleDestKey)
{
   e_Err e;
   t_RTO *p_RTO = (t_RTO *) p_RTB->RTO_Id;
   t_ARO *p_ARO;

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF)
   {
       char traceBuf[OSPF_MAX_TRACE_STR_LEN];
       sprintf(traceBuf, "Computing OSPF external routes ");
       if (SingleDestKey)
       {
           char traceBuf2[OSPF_MAX_TRACE_STR_LEN];
           L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
           L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
           osapiInetNtoa(SingleDestKey->DstAdr, destStr);
           osapiInetNtoa(SingleDestKey->Prefix, maskStr);
           sprintf(traceBuf2, "for destination %s/%s", destStr, maskStr);
           if (strlen(traceBuf) + strlen(traceBuf2) < OSPF_MAX_TRACE_STR_LEN)
           {
               strcat(traceBuf, traceBuf2);
           }
       }
       RTO_TraceWrite(traceBuf);
   }

   if (SingleDestKey)
   {
     p_RTB->IncExtCalcNum++;
   
     /* If we get a lot of external LSAs at once, do a full calculation
      * rather than an incremental for every one. */
     if (p_RTB->IncExtCalcNum >= OSPF_MAX_EXT_INCREMENTALS)
     {
       RTB_AddSpfReason(p_RTB, R_EXT_LSA);
       if (!TIMER_Active(p_RTB->RecalcTimer))
       {
         RTB_ComputeCalcDelay(p_RTB, p_RTO);
         TIMER_StartSec(p_RTB->RecalcTimer, p_RTB->CalcDelay,
                        0, RecalcTimerExp, p_RTO->OspfRtbThread.threadHndle);
       }
       p_RTB->IncExtCalcNum = 0;
      }
   }

   /* First examine all Type-5 LSAs */
   AsExtUpdateRte(p_RTB, p_RTO->AsExternalLsaHl, SingleDestKey);

   /* Now examine Type-7 LSAs in all areas */
   e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
   while(e == E_OK)
   {
      AsExtUpdateRte(p_RTB, p_ARO->NssaLsaHl, SingleDestKey);
      e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
   }

   return E_OK;
}

/*********************************************************************
 * @purpose    AS-external routes calculation according to Section 16.4.
 *
 *
 * @param   p_RTB          @b{(input)}  Routing Table object handler
 * @param   objId          @b{(input)}  Hl of External-LSAs
 * @param   SingleDestKey  @b{(input)}  key of a single destination -
 *                                      must be set to 0 when  the
 *                                      procedure is called for all
 *                                      destinations
 *
 * @returns                E_OK - calculation passed successful,
 * @returns                E_NOMEMORY - fail in memory allocation
 * @returns                E_FAILED   - critical error
 *
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err AsExtUpdateRte(t_RTB *p_RTB, t_Handle objId,
                            t_RoutingTableKey *SingleDestKey)
{
   e_Err e;
   t_A_DbEntry *p_DbEntry;
   t_RTO *p_RTO = (t_RTO*) p_RTB->RTO_Id;

   for (e = AVLH_GetFirst(objId, (void *)&p_DbEntry);
        e == E_OK;
        e = AVLH_GetNext(objId, (void *)&p_DbEntry, p_DbEntry))
   {
      t_S_AsExternalLsa *p_AsExtLsa;
      t_RoutingTableKey rtbkey;
      t_RoutingTableEntry *p_PreferedRte = NULLP;

/*---------------------------------------------------------------*/
/*    Note: These changes were added to fix ANVL OSPF test 24.6  */
/*---------------------------------------------------------------*/
      UpdateAgeOfDbEntry(p_DbEntry);                       
   
      if(SingleDestKey)
      {            
        p_AsExtLsa = (t_S_AsExternalLsa *)p_DbEntry->p_Lsa;

        rtbkey.DstAdr = A_GET_4B(p_DbEntry->Lsa.LsId);
        rtbkey.Prefix = A_GET_4B(p_AsExtLsa->NetworkMask);

         if(rtbkey.Prefix)
            rtbkey.DstAdr &= rtbkey.Prefix;

         /* we check LSAs desribing the specified destination */
         if(memcmp(&rtbkey, SingleDestKey, sizeof(t_RoutingTableKey)))
            continue;
      }

      /* Don't let OSPF compute more network routes than the platform max.
       * This check motivated by need to limit the memory allocated from the
       * routing heap for routing table entries. Since we allow more LSAs 
       * than routes for ECMP, we need to limit number of routes computed. 
       * Of course, RTO may have other routes and so even some of these OSPF
       * routes may fail to get added to RTO. Again, the primary concern here is
       * limiting memory consumption, not enforcing max routes.
       * Only check > max routes. If equal, need to process all db entries 
       * to that dest so we get the right route for that dest. */
      if (RTB_NetworkRoutesCount(p_RTB) > OSPF_MAX_ROUTES)
      {
        /* Enter overload since forwarding table is going to be incomplete. */
        if (!p_RTO->LsdbOverload)
        {
          L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_OSPF_MAP_COMPONENT_ID,
                 "The number of OSPFv2 network routes exceeds the forwarding "
                  "table limit of %d routes.", OSPF_MAX_ROUTES);
          EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
        }
        return E_NO_ROOM;
      }

      if(AsExtVerifyLsa(p_RTB,p_DbEntry,&p_PreferedRte) == E_OK)
         AsExtModifyRte(p_RTB, p_DbEntry, p_PreferedRte);
   }

   return E_OK;
}

/*********************************************************************
 * @purpose             Calculate the inter Area routes on base summary LSAs
 *                      examination.
 *
 *
 * @param p_RTB          @b{(input)}  Routing Table object handler
 * @param p_ARO          @b{(input)}  AREA object Id
 * @param incrKey        @b{(input)}  Destination key to process in
 *                                    case of  ForSingleDest = 1
 * @param ForSingleDest  @b{(input)}  process all LSAs describing paths to
 *                                    the single destination DstAdr
 *
 * @returns              E_OK       - calculation passed successful,
 * @returns              E_NOMEMORY - fail in memory allocation
 * @returns              E_FAILED   - critical error in SPF calculation
 *                                    algorithm
 *
 *
 *  @notes
 *                      In case of border router ARO is backbone
 *                      area (0.0.0.0)
 *
 * @end
 * ********************************************************************/
static e_Err InterAreaCalc(t_RTB *p_RTB, t_ARO *p_ARO, t_RoutingTableKey *incrKey, 
                           Bool ForSingleDest)
{
   t_RTO *p_RTO = (t_RTO *) p_RTB->RTO_Id;
   e_Err e;
   e_S_LScodes sumtype;
   t_A_DbEntry *p_DbEntry;
   t_RoutingTableKey destKey;
   t_S_SummaryLsa *p_Sum;
   t_AgrEntry *p_AdrRange;
   t_RoutingTableEntry *p_Rte, *p_Br;
   ulng iac;
   SP_IPADR advRtr, linkStateId, netMask;
   char traceBuf[OSPF_MAX_TRACE_STR_LEN];
   L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
   L7_uchar8 ipAdrStr[OSAPI_INET_NTOA_BUF_SIZE];
   L7_uchar8 netMaskStr[OSAPI_INET_NTOA_BUF_SIZE];

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF)
   {
       osapiInetNtoa((L7_uint32)p_ARO->AreaId, areaIdStr);
       sprintf(traceBuf, "Computing inter-area routes for area %s ",
               areaIdStr);
       if (ForSingleDest)
       {
           char traceBuf2[OSPF_MAX_TRACE_STR_LEN];
           osapiInetNtoa(incrKey->DstAdr, ipAdrStr);
           osapiInetNtoa(incrKey->Prefix, netMaskStr);
           sprintf(traceBuf2, 
                   "and destination %s/%s", ipAdrStr, netMaskStr);
           if (strlen(traceBuf) + strlen(traceBuf2) < OSPF_MAX_TRACE_STR_LEN)
           {
               strcat(traceBuf, traceBuf2);
           }
       }
       RTO_TraceWrite(traceBuf);
   }

   /* examine all summary LSAs in this area */
   for(sumtype=S_IPNET_SUMMARY_LSA; sumtype<=S_ASBR_SUMMARY_LSA; sumtype++)
   {
      for (e = AVLH_GetFirst(GET_OWNER_HL(p_RTO, p_ARO, NULL, sumtype), (void *)&p_DbEntry);
           (e == E_OK) && (p_DbEntry != NULL);
           e = AVLH_GetNext(GET_OWNER_HL(p_RTO, p_ARO, NULL, sumtype), 
                            (void *)&p_DbEntry, p_DbEntry))
      {
        /* Don't let OSPF compute more network routes than the platform max.
         * This check motivated by need to limit the memory allocated from the
         * routing heap for routing table entries. Since we allow more LSAs 
         * than routes for ECMP, we need to limit number of routes computed. 
         * Of course, RTO may have other routes and so even some of these OSPF
         * routes may fail to get added to RTO. Again, the primary concern here is
         * limiting memory consumption, not enforcing max routes.
         * Only check > max routes. If equal, need to process all db entries 
         * to that dest so we get the right route for that dest. */
        if (RTB_NetworkRoutesCount(p_RTB) > OSPF_MAX_ROUTES)
        {
          /* Enter overload since forwarding table is going to be incomplete. */
          if (!p_RTO->LsdbOverload)
          {
            L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_OSPF_MAP_COMPONENT_ID,
                 "The number of OSPFv2 network routes exceeds the forwarding "
                  "table limit of %d routes.", OSPF_MAX_ROUTES);
            EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
          }
          return E_NO_ROOM;
        }

/*---------------------------------------------------------------*/
/*    Note: These changes were added to fix ANVL OSPF test 24.6  */
/*---------------------------------------------------------------*/
      UpdateAgeOfDbEntry(p_DbEntry);

         p_Sum = (t_S_SummaryLsa *)p_DbEntry->p_Lsa;
         advRtr = A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter);
         linkStateId = A_GET_4B(p_DbEntry->Lsa.LsId);
         
         /* (1) If the cost specified by the LSA is LSInfinity, or if the */
         /*     LSA's LS age is equal to MaxAge, then examine the the next  */
         /*     LSA.                                                        */
         /* (2) If the LSA was originated by the calculating router itself, */
         /*     examine the next LSA.                                      */

         if((A_GET_3B(p_Sum->Metric) >= LSInfinity) ||
            (A_GET_2B(p_DbEntry->Lsa.LsAge) == MaxAge) ||
            (advRtr == p_RTO->Cfg.RouterId) ||
            ((p_DbEntry->Lsa.LsType == S_ASBR_SUMMARY_LSA) &&
            (linkStateId == p_RTO->Cfg.RouterId))) 
         {
            continue;                           
         }

         destKey.DstAdr = linkStateId;
         destKey.Prefix = netMask = A_GET_4B(p_Sum->NetworkMask);

         if(destKey.Prefix) 
         {
            destKey.DstAdr &= destKey.Prefix;
         }

         /* if only single destination check was required and this summary does 
          * not match the destination we are recomputing, go to next summary LSA. */
         if(ForSingleDest) 
         {
            if (memcmp(&destKey, incrKey, sizeof(t_RoutingTableKey))) 
            {
               continue;                           
            }
         }

         /* (3) If it is a Type 3 summary-LSA, and the collection of          */
         /*     destinations described by the summary-LSA equals one of the   */
         /*     router's configured area address ranges (see Section 3.5),    */
         /*     and  the particular area address range is active, then the    */
         /*     summary-LSA   should be ignored.  "Active" means that   there */
         /*     are  one or more reachable (by intra-area paths) networks     */
         /*     contained in the area range.                                  */
        if (p_DbEntry->Lsa.LsType == S_IPNET_SUMMARY_LSA)
        {
          if ((HL_FindFirst(p_ARO->AdrRangeHl, (byte*)&destKey.DstAdr, 
                            (void *)&p_AdrRange) == E_OK) &&
              (p_AdrRange->LsdbType == AGGREGATE_SUMMARY_LINK) &&
              (p_AdrRange->AggregateStatus == ROW_ACTIVE) && 
              (p_AdrRange->alive) && (routerIsBorder(p_RTO)))
            continue;                           
        }
         
         /* Else, call the destination described by the LSA N (for Type     */
         /*     3 summary-LSAs, N's address  is obtained by masking the LSA's   */
         /*     Link State ID with the network/subnet mask contained in the     */
         /*     body of the   LSA), and the area border originating the LSA     */
         /*     BR.   Look up the routing table entry for BR   having Area A as */
         /*     its  associated area.  If no such entry exists for router BR    */
         /*     (i.e., BR is unreachable in  Area A), do nothing with this      */
         /*     LSA  and consider the next in the list.  Else, this LSA         */
         /*     describes an inter-area path to destination N, whose cost is    */
         /*     the  distance to BR plus the cost specified in the LSA. Call    */
         /*     the  cost of  this inter-area   path IAC.                       */
         
         RteIntraAreaFindByType(p_ARO, advRtr,
                                DEST_TYPE_IS_BORDER_ROUTER, &p_Br);

         if (p_Br == NULL) 
         {
            if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
            {
               L7_uchar8 rtrStr[OSAPI_INET_NTOA_BUF_SIZE];
               L7_uchar8 areaStr[OSAPI_INET_NTOA_BUF_SIZE];

               osapiInetNtoa(advRtr, rtrStr),
               osapiInetNtoa(p_ARO->AreaId, areaStr);

               sprintf(traceBuf, 
                       "ABR %s is unreachable in area %s", rtrStr, areaStr);
               RTO_TraceWrite(traceBuf);
            }
            continue;
         } 

         /* (5) Next, look up the routing table entry for the destination N.       */
         /*     (If  N is an  AS boundary router, look up the  "router" routing    */
         /*     table entry   associated with   Area A).  If no   entry exists for */
         /*     N or if the   entry's  path type is "type 1 external" or "type     */
         /*     2 external", then install the inter-area path to N,with       */
         /*     associated area Area A, cost IAC, next hop equal to  the list      */
         /*     of next hops to router BR, and Advertising router equal to BR */

         destKey.DstAdr = linkStateId;
         if (p_DbEntry->Lsa.LsType == S_IPNET_SUMMARY_LSA) 
         {
            destKey.Prefix = netMask;
            if(destKey.Prefix) 
            {
               destKey.DstAdr &= destKey.Prefix;
            }
            p_Rte = o2RouteFind(p_RTB, p_RTB->RtbNtBt, linkStateId, netMask);
         } 
         else 
         {
           p_Rte = o2RouteFind(p_RTB, p_RTB->RtbRtBt, linkStateId, SPF_HOST_ROUTE);
           destKey.Prefix = SPF_HOST_ROUTE;
         }
         
         /* calculate IAC */
         iac = p_Br->Cost + A_GET_3B(p_Sum->Metric);

         /* if there's no route, or if existing route might be less preferred */
         if((p_Rte == NULL) || 
            (p_DbEntry->Lsa.LsType == S_IPNET_SUMMARY_LSA && 
             ((p_Rte->PathType == OSPF_TYPE_1_EXT) ||
             (p_Rte->PathType == OSPF_TYPE_2_EXT))) ||
            (p_DbEntry->Lsa.LsType == S_ASBR_SUMMARY_LSA && 
             ((p_Rte->PathType == OSPF_INTRA_AREA) &&
             (p_Rte->AreaId == 0))))
         {
            if((p_Rte = XX_Malloc(sizeof(t_RoutingTableEntry))) == NULL)
            {
              if (!p_RTO->LsdbOverload)
                EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
              return E_NOMEMORY;
            }
            memset(p_Rte, 0, sizeof(t_RoutingTableEntry));   

            destKey.DstAdr = A_GET_4B(p_DbEntry->Lsa.LsId);

            if (p_DbEntry->Lsa.LsType == S_ASBR_SUMMARY_LSA)
            {
               destKey.Prefix = SPF_HOST_ROUTE;
            } 
            else 
            {
               destKey.Prefix = netMask;
               if(destKey.Prefix) 
               {
                  destKey.DstAdr &= destKey.Prefix;
               }
            }
        
            p_Rte->DestinationId = destKey.DstAdr;
            p_Rte->IpMask = destKey.Prefix;
            p_Rte->AreaId = p_ARO->AreaId;

            p_Rte->PathType = OSPF_INTER_AREA;
            p_Rte->Cost = iac;
            p_Rte->p_DbEntry = p_DbEntry;
            if(L7_BIT_ISSET(p_Br->p_DbEntry->dbFlags, O2_DB_ENTRY_IS_CUR))
            {
               if (p_DbEntry->Lsa.LsType == S_ASBR_SUMMARY_LSA) 
               {
                  p_Rte->OptionalCapabilities = *(p_DbEntry->p_Lsa);
               }
               else
               {
                  p_Rte->OptionalCapabilities = *(p_Br->p_DbEntry->p_Lsa);
               }
            }

            p_Rte->PathNum = 0;
            p_Rte->ErNum = 0;
            p_Rte->PathPrm = NULL;

            /* copy paths from Border router */
            PathCopy(p_RTO, p_Rte, p_Br, p_Br->DestinationId, 0);

            if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL) 
            {
              osapiInetNtoa(destKey.DstAdr, ipAdrStr);
              osapiInetNtoa(destKey.Prefix, netMaskStr);
              sprintf(traceBuf, 
                      "New inter-area route %s/%s metric %ld and %d next hops.", 
                      ipAdrStr, netMaskStr, iac, p_Rte->PathNum);
               RTO_TraceWrite(traceBuf);
            }
            
            if(p_DbEntry->Lsa.LsType == S_IPNET_SUMMARY_LSA) 
            {
               p_Rte->DestinationType = DEST_TYPE_IS_NETWORK;
               RteUpdate(p_RTB, p_Rte);
            }
            else
            {
               p_Rte->DestinationType =  DEST_TYPE_IS_ROUTER | DEST_TYPE_IS_AS_BOUNDARY_ROUTER;
               if(p_Rte->OptionalCapabilities & S_BORDER_FLAG)
               {
                  p_Rte->DestinationType |=  DEST_TYPE_IS_BORDER_ROUTER;
               }

               RTB_AsbrUpdate(p_RTB, p_ARO, p_Rte);
               RteRelease(p_Rte, TRUE);   /* RTB_AsbrUpdate() makes a copy if p_Rte installed*/
            }
         }

         /* (7) Else, the paths present in the routing table are also       */
         /*     inter-area paths.  Install the new path through BR if it is */
         /*     cheaper, overriding the paths in the routing table.         */
         /*     Otherwise, if the new path is the same cost, add it  to the */
         /*     list of paths that appear in the routing table entry.       */
         else if (p_Rte && (p_Rte->PathType == OSPF_INTER_AREA))
         {
            if(iac < p_Rte->Cost)
            {
               p_Rte->Cost = iac;
               p_Rte->p_DbEntry = p_DbEntry;
               p_Rte->AreaId = p_ARO->AreaId;

               p_Rte->PathType = OSPF_INTER_AREA;
               p_Rte->OptionalCapabilities = *(p_Br->p_DbEntry->p_Lsa);

               RteRelease(p_Rte, FALSE); /* release recent paths */
               PathCopy(p_RTO, p_Rte, p_Br, p_Br->DestinationId, 0);

               if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL) 
               {
                  L7_uchar8 nHopStr[OSAPI_INET_NTOA_BUF_SIZE];
                  osapiInetNtoa(destKey.DstAdr, ipAdrStr);
                  osapiInetNtoa(destKey.Prefix, netMaskStr);
                  osapiInetNtoa(p_Rte->PathPrm->IpAdr, nHopStr);
                  sprintf(traceBuf, 
                          "Inter-area route %s/%s with metric %ld via %s has lower cost.", 
                          ipAdrStr, netMaskStr, iac, nHopStr);
                  RTO_TraceWrite(traceBuf);
               }
            }
            else if(iac == p_Rte->Cost)         
            {
              if (PathCompare(p_Rte, p_Br) == FALSE)
              {
                word newPathNum = 0;

                /* next hops to border router different from next hops to dest */
                  p_Rte->p_DbEntry = p_DbEntry;
                  /* add next hops to BR to next hops to dest */
                  newPathNum = PathCopy(p_RTO, p_Rte, p_Br, p_Br->DestinationId, 0);

                  if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL) 
                  {
                     L7_uchar8 nHopStr[OSAPI_INET_NTOA_BUF_SIZE];
                     osapiInetNtoa(destKey.DstAdr, ipAdrStr);
                     osapiInetNtoa(destKey.Prefix, netMaskStr);
                     osapiInetNtoa(p_Rte->PathPrm->IpAdr, nHopStr);
                     sprintf(traceBuf, 
                             "%d paths added to inter-area route %s/%s [%ld]. Route now has %d paths.", 
                             newPathNum, ipAdrStr, netMaskStr, iac, p_Rte->PathNum);
                     RTO_TraceWrite(traceBuf);
                  }
               }
            }
         }
      } /* endwhile each summary */
   } /* endfor sumtype */
   return E_OK;
}


/*********************************************************************
 * @purpose              Examining transit area's summar-LSAs
 *
 *
 * @param p_RTB          @b{(input)}  Routing Table object handler
 * @param p_ARO          @b{(input)}  ARO object Id
 * @param incrKey        @b{(input)}  Destination key to process in
 *                                    case of  ForSingleDest = 1
 * @param ForSingleDest  @b{(input)}  process all LSAs describing paths to
 *                                    the single destination DstAdr
 *
 * @returns  E_OK        success
 * @returns  E_NOMEMORY  failed in memory allocation
 * @returns  E_FAILED    failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err TransitAreaExam(t_RTB *p_RTB, t_ARO *p_ARO, t_RoutingTableKey *incrKey, Bool ForSingleDest)
{

   t_RTO *p_RTO = (t_RTO *) p_RTB->RTO_Id;
   e_Err e;
   e_S_LScodes sumtype;
   t_A_DbEntry *p_DbEntry;
   t_S_SummaryLsa *p_Sum;
   t_RoutingTableKey destKey;
   t_Handle routeTree;
   t_RoutingTableEntry *p_Rte, *p_Br;
   ulng iac;
   SP_IPADR advRtr;
   char traceBuf[OSPF_MAX_TRACE_STR_LEN];
   L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
   L7_uchar8 ipAdrStr[OSAPI_INET_NTOA_BUF_SIZE];
   L7_uchar8 netMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
   L7_uchar8 nHopStr[OSAPI_INET_NTOA_BUF_SIZE];

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF)
   {
       if (p_ARO)
       {
           osapiInetNtoa((L7_uint32)p_ARO->AreaId, areaIdStr);
           sprintf(traceBuf, "Examining transit area %s ", areaIdStr);
           if (ForSingleDest)
           {
               char traceBuf2[OSPF_MAX_TRACE_STR_LEN];
               L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
               L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
               osapiInetNtoa(incrKey->DstAdr, destStr);
               osapiInetNtoa(incrKey->Prefix, maskStr);
               sprintf(traceBuf2, "and destination %s/%s", destStr, maskStr);
               if (strlen(traceBuf) + strlen(traceBuf2) < OSPF_MAX_TRACE_STR_LEN)
               {
                   strcat(traceBuf, traceBuf2);
               }
           }
           RTO_TraceWrite(traceBuf);
       }
   }

   /* examine all summary LSAs in this area */
   for(sumtype=S_IPNET_SUMMARY_LSA; sumtype<=S_ASBR_SUMMARY_LSA; sumtype++)
   {
      for (e = AVLH_GetFirst(GET_OWNER_HL(p_RTO, p_ARO, NULL, sumtype), (void *)&p_DbEntry);
           ((e == E_OK) && (p_DbEntry != NULL));
           e = AVLH_GetNext(GET_OWNER_HL(p_RTO, p_ARO, NULL, sumtype), 
                            (void *)&p_DbEntry, p_DbEntry))
      {

/*---------------------------------------------------------------*/
/*    Note: These changes were added to fix ANVL OSPF test 24.6  */
/*---------------------------------------------------------------*/
      UpdateAgeOfDbEntry(p_DbEntry);

         /* Only transit summary LSA are examined */
         if((p_DbEntry->Lsa.Options & OSPF_OPT_E_BIT) == 0)
         {
            continue;
         }
             
         /* For each summary */      
         p_Sum = (t_S_SummaryLsa *)p_DbEntry->p_Lsa;

   /* (1) If the cost   advertised by the summary-LSA is LSInfinity, or    */
   /*     if the LSA's LS age is equal to MaxAge, then examine the         */
   /*     next LSA.                                                        */
         if((A_GET_3B(p_Sum->Metric) >= LSInfinity) ||
            (A_GET_2B(p_DbEntry->Lsa.LsAge) == MaxAge))
         {
            continue;                           
         }

   /* (2) If the summary-LSA was originated by the calculating router      */
   /*     itself, examine the next LSA.                                    */
         if(A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter) == 
            p_RTO->Cfg.RouterId) 
         {
            continue;                           
         }

   /* (3) Look up the   routing  table entry for   N. (If N is an AS       */
   /*     boundary router, look up the "router" routing table  entry       */
   /*     associated with the backbone area).   If it does not exist, or   */
   /*     if the route type is other than intra-area or inter-area, or     */
   /*     if the area   associated with   the routing table entry is not   */
   /*     the  backbone area, then examine the  next LSA. In other         */
   /*     words, this   calculation only updates backbone intra-area       */
   /*     routes found in Section 16.1 and inter-area routes found in      */
   /*     Section 16.2.                                                    */

         destKey.DstAdr = A_GET_4B(p_DbEntry->Lsa.LsId);

         if (p_DbEntry->Lsa.LsType == S_ASBR_SUMMARY_LSA) 
         {
            destKey.Prefix = 0;
            routeTree = p_RTB->RtbRtBt;
         } 
         else 
         {
            destKey.Prefix = A_GET_4B(p_Sum->NetworkMask);
            if(destKey.Prefix) 
            {
               destKey.DstAdr &= destKey.Prefix;
            }
            routeTree = p_RTB->RtbNtBt;
         }
     
         if(ForSingleDest) 
         {
            if (memcmp(&destKey, incrKey, sizeof(t_RoutingTableKey))) 
            {
              continue;                           
            } 
         }

         p_Rte = o2RouteFind(p_RTB, routeTree, destKey.DstAdr, destKey.Prefix);

         if(p_Rte == NULL) 
         {
            continue;      
         } 
         else if(((p_Rte->AreaId != OSPF_BACKBONE_AREA_ID) &&
             !p_Rte->BackboneReachable) ||
            !p_Rte->p_DbEntry ||
            ((p_Rte->PathType != OSPF_INTRA_AREA) &&
            (p_Rte->PathType != OSPF_INTER_AREA)))
         {
            continue;      
         }

         /* don't update direct routes */
         if (!p_Rte->PathPrm->IpAdr) 
         {
            continue;      
         }

   /* (4) Look up the   routing  table entry for   the advertising   router*/
   /*     BR associated with the Area  A. If it is unreachable, examine    */
   /*     the  next LSA. Otherwise, the cost to destination N is the       */
   /*     sum  of the cost in BR's Area A routing table entry and the      */
   /*     cost advertised in the LSA.  Call this cost IAC.                 */
       
         advRtr = A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter); 
         RteIntraAreaFindByType(p_ARO, advRtr, DEST_TYPE_IS_BORDER_ROUTER, &p_Br);

         if (p_Br == NULL) 
         {
            continue;                           
         }

         /* calculate the cost to destination N */
         iac = p_Br->Cost + A_GET_3B(p_Sum->Metric);

   /* (5) If this cost is less than the cost occurring in N's  routing    */
   /*     table entry, overwrite N's list of next hops with those used    */
   /*     for  BR, and  set N's  routing  table cost to IAC. Else, if IAC */
   /*     is the same   as N's current cost, add BR's list of next hops   */
   /*     to N's list   of next  hops. In any case, the area associated   */
   /*     with N's routing table entry must remain the backbone area,     */
   /*     and  the path type (either intra-area or inter-area) must       */
   /*     also remain   the same.                                         */

         if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF)
         {
            osapiInetNtoa(destKey.DstAdr, ipAdrStr);
            osapiInetNtoa(destKey.Prefix, netMaskStr);
            osapiInetNtoa(p_Br->PathPrm->IpAdr, nHopStr);
         }

         if(iac < p_Rte->Cost)
         {
            p_Rte->Cost = iac;
            p_Rte->p_DbEntry = p_DbEntry;

            RteRelease(p_Rte, FALSE); /* release recent paths */
            PathCopy(p_RTO, p_Rte, p_Br, p_Br->DestinationId, 0);
            if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
            {
               sprintf(traceBuf, 
                       "transit shortcut %s/%s [%ld] via %s (lower cost)", 
                       ipAdrStr, netMaskStr, iac, nHopStr);
               RTO_TraceWrite(traceBuf);
            }
         }
         else if(iac == p_Rte->Cost)         
         {
            byte match_paths = 0;
            t_OspfPathPrm *Path1, *Path2;
            t_LIH ifIndex;
            t_LIH ifIndex_Br = 0;

            for(Path1 = p_Br->PathPrm; Path1; Path1 = Path1->next)
            {
               for(Path2 = p_Rte->PathPrm; Path2; Path2 = Path2->next)
               {
                  ifIndex = (((t_IFO*)Path2->NextHop)->Cfg.Type == IFO_VRTL) ? 
                     ((t_IFO*)Path2->NextHop)->VirtIfIndex : ((t_IFO*)Path2->NextHop)->Cfg.IfIndex;

                  if(((t_IFO*)Path1->NextHop)->Cfg.IfIndex == ifIndex)

                  /* If the next hop is a virtual interface then compare the virtIfIndex with
                  ** the corresponding virtIfIndex of the router entry
                  */
                  ifIndex_Br = (((t_IFO*)Path1->NextHop)->Cfg.Type == IFO_VRTL) ? 
                    ((t_IFO *)Path1->NextHop)->VirtIfIndex : ((t_IFO*)Path1->NextHop)->Cfg.IfIndex;

                  if(ifIndex_Br == ifIndex)

                  {
                     match_paths++;
                     break;
                  }
               }
            }
            
            if(match_paths != p_Br->PathNum)
            {
               p_Rte->p_DbEntry = p_DbEntry;

               /* release recent paths for virtual interface */
               if(p_Rte->VrtlIfo)
                  RteRelease(p_Rte, FALSE); 

               /* add the new path into N' list of paths */
               PathCopy(p_RTO, p_Rte, p_Br, p_Br->DestinationId, 0);
               if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
               {
                  sprintf(traceBuf, 
                          "transit shortcut %s/%s [%ld] via %s (equal cost)", 
                          ipAdrStr, netMaskStr, iac, nHopStr);
                  RTO_TraceWrite(traceBuf);
               }
            }
            else
            {
              PathCompareAndValidate(p_Rte,p_Br);
            }
         }
         else
         {
            if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
            {
               sprintf(traceBuf, 
                       "transit shortcut %s/%s [%ld] via %s (higher cost)", 
                       ipAdrStr, netMaskStr, iac, nHopStr);
                  RTO_TraceWrite(traceBuf);
            }
         }
      } /* endwhile each summary */
   } /* endfor sumtype */

   /* Now any paths calculated before and still have unresolved
      virtual next hops should be discarded */
   o2RoutesBrowse(p_RTB, p_RTB->RtbRtBt, UnresolvedRteDelete, (ulng) p_ARO);      
   o2RoutesBrowse(p_RTB, p_RTB->RtbNtBt, UnresolvedRteDelete, (ulng) p_ARO);      

   return E_OK;
}




/*********************************************************************
 * @purpose              Calculate the Short Path First Tree for an Area.
 *
 *
 * @param  p_RTB         @b{(input)}  Routing Table object handler
 * @param  p_ARO         @b{(input)}  AREA object Id
 *
 * @returns              E_OK       - calculation passed successful,
 * @returns              E_NOMEMORY - fail in memory allocation
 * @returns              E_FAILED   - critical error in SPF calculation
 *                                    algorithm
 *
 * @notes
 *
 * @end
 * ********************************************************************/

/*--------------------------------------------------------------------
 * ROUTINE:    SpfIntraAreaCalc
 *
 * DESCRIPTION:
 *    Calculate the Short Path First Tree for an Area.
 *
**     16.1.  Calculating the shortest-path tree for an area              **
**                                                                        **
**    This calculation yields the set  of intra-area routes associated    **
**    with an  area (called hereafter Area A).   A router calculates the  **
**    shortest-path tree using itself  as the root.[22] The formation     **
**    of the shortest   path tree is done here in two stages.  In the     **
**    first stage, only links between  routers  and transit networks are  **
**    considered.  Using the Dijkstra  algorithm, a tree is formed from   **
**    this subset of the link state database.    In the  second stage,    **
**    leaves are added to the tree by  considering the   links to stub    **
**    networks.                                                           **
**                                                                        **
**    The procedure will be explained  using the graph   terminology that **
**    was introduced in Section 2.  The area's link state database is     **
**    represented as a directed graph.  The graph's vertices are          **
**    routers, transit networks and stub networks.  The first  stage of   **
**    the procedure concerns only the  transit  vertices (routers and     **
**    transit  networks) and their connecting links.                      **
**                                                                        **
**    The first stage   of the procedure (i.e., the Dijkstra algorithm)   **
**    can now  be summarized as follows. At each iteration of the         **
**    algorithm, there is a list of candidate   vertices.  Paths from     **
**    the root to these vertices have  been found, but   not necessarily  **
**    the shortest ones.  However, the paths to the candidate  vertex     **
**    that is  closest  to the root are   guaranteed to be shortest; this **
**    vertex is added   to the shortest-path tree, removed from   the     **
**    candidate list,   and its  adjacent vertices are examined for       **
**    possible addition to/modification of the candidate list.  The       **
**    algorithm then iterates again.   It terminates when the candidate   **
**    list becomes empty.                                                 **
**                                                                        **
**                                                                        **
**    The stub networks are added to the tree   in the procedure's        **
**    second stage.  In this stage, all router vertices are again         **
**    examined.  Those that have been  determined to be unreachable in    **
**    the above first   phase are discarded.  For each reachable router   **
**    vertex (call it   V), the  associated router-LSA is found in the    **
**    link state database.  Each stub  network  link appearing in the     **
**    LSA is then examined and processed.                                 **
 *
 *
 * ARGUMENTS:
 *    p_RTB      Routing Table object handler
 *    p_ARO     AREA object Id
 *
 * RESULTS:
 *    E_OK - calculation passed successful,
 *    E_NOMEMORY - fail in memory allocation
 *    E_FAILED - critical error in SPF calculation algorithm
 *--------------------------------------------------------------------*/
static e_Err SpfIntraAreaCalc(t_RTB *p_RTB, t_ARO *p_ARO)
{
   t_RTO *p_RTO = (t_RTO *) p_RTB->RTO_Id;
   t_RoutingTableEntry *p_V;
   e_Err e;
   t_A_DbKey   DbKey;

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF)
   {
     char traceBuf[OSPF_MAX_TRACE_STR_LEN];
     L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
     osapiInetNtoa((L7_uint32)p_ARO->AreaId, areaIdStr);
     sprintf(traceBuf, "Computing intra-area routes for area %s", areaIdStr);
     RTO_TraceWrite(traceBuf);
   }

   /* (1) Initialize the algorithm's data structures */
   RTB_ClearSpt(p_ARO);
   RTB_ClearCandidateHl(p_ARO);
   p_ARO->TransitCapabilityOld = p_ARO->TransitCapability;
   p_ARO->TransitCapability = FALSE;
   
   /* Initialize the SPF tree to only the root */
   if((p_V = XX_Malloc(sizeof(t_RoutingTableEntry))) == NULL)
   {
     if (!p_RTO->LsdbOverload)
       EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
     return E_NOMEMORY;
   }
   memset(p_V, 0, sizeof(t_RoutingTableEntry));

   p_V->DestinationType = DEST_TYPE_IS_ROUTER;
   p_V->DestinationId = p_RTO->Cfg.RouterId; 
   p_V->IpMask = SPF_HOST_ROUTE;
   p_V->AreaId = p_ARO->AreaId;
   p_V->Cost = 0;
   p_V->PathType = OSPF_INTRA_AREA;
   if(p_ARO->AreaId == OSPF_BACKBONE_AREA_ID)
      p_V->BackboneReachable = TRUE;

   A_SET_4B(p_V->DestinationId, DbKey.LsId);
   memcpy(DbKey.AdvertisingRouter, DbKey.LsId, 4);
   e = E_FAILED;

/*---------------------------------------------------------------*/
/*    Note: These changes were added to fix ANVL OSPF test 24.6  */
/*---------------------------------------------------------------*/
   if((AVLH_Find(p_ARO->RouterLsaHl, (byte*)&DbKey, (void *)&p_V->p_DbEntry,0) == E_OK) &&
      (UpdateAgeOfDbEntry(p_V->p_DbEntry) == E_OK) &&
      (A_GET_2B(p_V->p_DbEntry->Lsa.LsAge) < MaxAge))

   {
      e = E_OK;
   }
   if(e != E_OK)
   {
      RteRelease(p_V, TRUE);
      return E_OK;
   }

   p_V->OptionalCapabilities = *(p_V->p_DbEntry->p_Lsa);

   if(AVLH_Insert(p_ARO->SpfHl, (byte*)&p_V->DestinationId, p_V) != E_OK)
   {
      RteRelease(p_V, TRUE);
      return E_FAILED;
   }

   p_ARO->AsBdrRtrCount = p_ARO->AreaBdrRtrCount  = p_ARO->AreaIntRtrCount = 0;

   if (RTB_IntraAreaStage1(p_RTB, p_ARO, p_V) != E_OK)
       return E_FAILED;
 
   /* delete candidates that were never added to the SPT. They are not needed 
    * in stage 2. */
   RTB_ClearCandidateHl(p_ARO);
                              
   if (RTB_IntraAreaStage2(p_RTB, p_ARO, p_V) != E_OK)
       return E_FAILED;

   /* The number of times that the intra-area  route table  has  been calculated  */
   p_ARO->SpfRuns ++;         

   return E_OK;
}

/*********************************************************************
* @purpose  Stage 1 of the intra-area route calculation.
*
* @param    p_RTB  @b{(input)} OSPF routing table
* @param    p_ARO  @b{(input)} area whose intra-area routes are being computed
* @param    p_V    @b{(input)} vertex where calculation should begin (or resume)
*
* @returns  E_OK
*
* @notes    RFC 2328 section 16.1:   At each iteration of the
*           algorithm, there is a list of candidate vertices.  Paths from
*           the root to these vertices have been found, but not necessarily
*           the shortest ones.  However, the paths to the candidate vertex
*           that is closest to the root are guaranteed to be shortest; this
*           vertex is added to the shortest-path tree, removed from the
*           candidate list, and its adjacent vertices are examined for
*           possible addition to/modification of the candidate list.  The
*           algorithm then iterates again.  It terminates when the candidate
*           list becomes empty.
*
* @end
*********************************************************************/
static e_Err RTB_IntraAreaStage1(t_RTB *p_RTB, t_ARO *p_ARO, t_RoutingTableEntry *p_V)
{
    t_RTO *p_RTO = (t_RTO *) p_RTB->RTO_Id;
    t_RoutingTableEntry *p_W;
    e_Err e;
    t_S_RouterLsa *p_RouterLsa;
    ulng cand_list_num;
    t_ParseEntry *PrsEntry;
    /* Iterate until candidate list is empty */
    while (TRUE)
    {
        /* For router entry check virtual flag */
        if (p_V->DestinationType < DEST_TYPE_IS_NETWORK)
        {
            p_RouterLsa = (t_S_RouterLsa *) p_V->p_DbEntry->p_Lsa;

            /* If bit V of Router LSA is set, then switch on 
                the Area's TransitCapability */
            if (p_RouterLsa->Flags & S_VIRTUAL_FLAG) {
                p_ARO->TransitCapability = TRUE;
        }
        }

        /* (2) Examine the LSA assosiated with vertex V */
        /* For each described link which connect V to W do following: */
        for (PrsEntry = p_V->p_DbEntry->ConnList;PrsEntry;PrsEntry = PrsEntry->next)
        {
            if ((e = SpfCheckLink(p_RTB, p_ARO, p_V, PrsEntry)) != E_OK)
                return e;
        }    

        /* (3) Check if at this step the candidate list becames to be empty */
        AVLH_GetEntriesNmb(p_ARO->CandidateHl, &cand_list_num);
        if (cand_list_num == 0)
            break;   /* exit while(TRUE) loop */

        /* Otherwise, choose the closest vertex */
        /* Should maintain the candidates in a heap in order to quickly find
         * the candidate closest to the SPT. */
        e = AVLH_GetFirst(p_ARO->CandidateHl, (void *)&p_W);         
        p_V = p_W;
        while (e == E_OK && p_W)
        {
            /* Network vertices must be choosen before routers */
            if ((p_W->Cost < p_V->Cost) ||
                ((p_W->Cost == p_V->Cost) && 
                 (p_W->DestinationType & DEST_TYPE_IS_NETWORK)))
                p_V = p_W;

            e = AVLH_GetNext(p_ARO->CandidateHl, (void *)&p_W, p_W);         
        }

        /* Delete the found entry from candidate list */
        AVLH_Delete(p_ARO->CandidateHl, (byte*)&p_V->DestinationId, NULL);

        /* Insert the found entry into spf tree list */
        if (AVLH_Insert(p_ARO->SpfHl, (byte*)&p_V->DestinationId, p_V) != E_OK)
        {
            return E_FAILED;
        }

        if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
        {
            char traceBuf[OSPF_MAX_TRACE_STR_LEN];
            L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
            L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
            L7_uchar8 ipMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
            L7_uchar8 ipNHopStr[OSAPI_INET_NTOA_BUF_SIZE];
     
            osapiInetNtoa((L7_uint32)p_ARO->AreaId, areaIdStr);
            osapiInetNtoa(p_V->DestinationId, ipAddrStr);
            osapiInetNtoa(p_V->IpMask, ipMaskStr);
            osapiInetNtoa(p_V->PathPrm->IpAdr, ipNHopStr);
            sprintf(traceBuf, "Area %15s: intra-area route %s/%s [%ld] via %s ",
                    areaIdStr, ipAddrStr, ipMaskStr, p_V->Cost, ipNHopStr);
            RTO_TraceWrite(traceBuf);
        }

        /* (4) Modifying of routing table */

        if (p_V->DestinationType < DEST_TYPE_IS_NETWORK)
        {
            e = RTB_IntraAreaRouterVertexAdded(p_RTB, p_ARO, p_V);
            if (e != E_OK)
                return e;
        }
        else
        {
            e = RTB_IntraAreaNetworkVertexAdded(p_RTB, p_ARO, p_V);
            if (e != E_OK)
                return e;
        }
    } 

    return E_OK;
}

/*********************************************************************
* @purpose  Stage 2 of the intra-area route calculation.
*
* @param    p_RTB  @b{(input)} OSPF routing table
* @param    p_ARO  @b{(input)} area whose intra-area routes are being computed
* @param    p_V    @b{(input)} vertex where calculation should begin (or resume)
*
* @returns  E_OK
*
* @notes    RFC 2328 section 16.1:   The stub networks are added to the 
*           tree in the procedure's second stage.  In this stage, all router 
*           vertices are again examined.  Those that have been determined to 
*           be unreachable in the above first phase are discarded.  For each 
*           reachable router vertex (call it V), the associated router-LSA is 
*           found in the link state database.  Each stub network link appearing 
*           in the LSA is then examined. For all routing table entries 
*           added/modified in the second stage, the associated area will be set 
*           to Area A and the path type will be set to intra-area.  When the 
*           list of reachable router-LSAs is exhausted, the second stage is 
*           completed.  At this time, all intra-area routes associated with 
*           Area A have been determined.
*
* @end
*********************************************************************/
static e_Err RTB_IntraAreaStage2(t_RTB *p_RTB, t_ARO *p_ARO, t_RoutingTableEntry *p_V)
{
  t_RTO *p_RTO = (t_RTO*) p_RTB->RTO_Id;
  e_Err e;
  t_S_RouterLsa *p_RouterLsa;
  t_S_LinkDscr *p_Link;
  word link;
  t_RoutingTableEntry *p_RtEntry;
  t_RoutingTableKey rtbkey;
  t_VpnCos    VpnCos = 0;

  /* iterate through all nodes in the shortest path tree */
  e = AVLH_GetFirst(p_ARO->SpfHl, (void *)&p_V);
  while (e == E_OK && p_V)
  {
    if (p_V->DestinationType >= DEST_TYPE_IS_NETWORK)
    {
      e = AVLH_GetNext(p_ARO->SpfHl, (void *)&p_V, p_V);
      continue;
    }

    /* destination vertex is a router */
    p_RouterLsa = (t_S_RouterLsa *) p_V->p_DbEntry->p_Lsa;

    link = A_GET_2B(p_RouterLsa->LinkNum);
    if (link == 0)
    {
      /* No links advertised in this router LSA. Next vertex. */
        e = AVLH_GetNext(p_ARO->SpfHl, (void *)&p_V, p_V);
        continue;
    }
    
    /* Browse all links to stub networks */
    for (p_Link = (t_S_LinkDscr *) ((byte*)p_RouterLsa + sizeof(t_S_RouterLsa));
         link > 0;
         link--, 
           p_Link = (t_S_LinkDscr*) (((byte*)p_Link) + 
                                  sizeof(t_S_LinkDscr) + p_Link->TosNum*sizeof(t_S_Tos)))
    {
      if (p_Link->Type != S_STUB_NET)
        continue;
      
      if (p_Link->TosNum)
        VpnCos = A_GET_4B((byte*)(p_Link+1));
      else
        VpnCos = 0;

      /* Try to locate current stub network entry in routing table */
      rtbkey.DstAdr = A_GET_4B(p_Link->LinkId);
      rtbkey.Prefix = A_GET_4B(p_Link->LinkData);
      if (rtbkey.Prefix)
        rtbkey.DstAdr &= rtbkey.Prefix;

      p_RtEntry = o2RouteFind(p_RTB, p_RTB->RtbNtBt, rtbkey.DstAdr, rtbkey.Prefix);
      if (p_RtEntry)
      {
        Bool pathCompare;

        if (p_RtEntry->DirAttachNet)
        {
          p_RtEntry->PathPrm->AdvertisingRouter = 0;
          pathCompare = TRUE;
        }
        else
          pathCompare = PathCompareAndValidate(p_RtEntry, p_V);

        if ((p_RtEntry->DestinationType == DEST_TYPE_IS_STUB_NETWORK) ||
            ((p_RtEntry->DestinationType == DEST_TYPE_IS_NETWORK)))
        {
          /* (2) update the stub network routing table entry */  
          /* Assuming the only routes in the routing table at this 
           * point are intra area routes. Guess it doesn't hurt 
           * to check. */
          if (p_RtEntry->PathType == OSPF_INTRA_AREA)
          {
            /* If this a better route to the destination, replace the current one */
            if ((p_V->Cost + A_GET_2B(p_Link->Metric)) < p_RtEntry->Cost)
            {
              /* replace path */
              p_RtEntry->Cost = p_V->Cost + A_GET_2B(p_Link->Metric);                  
              p_RtEntry->p_DbEntry = p_V->p_DbEntry;   
              p_RtEntry->AreaId = p_V->AreaId;
              p_RtEntry->DestinationType = DEST_TYPE_IS_STUB_NETWORK;
              p_RtEntry->PathType = OSPF_INTRA_AREA;
              RteRelease(p_RtEntry, FALSE); /* release previous next hops */
              CalculateNextHopToStub(p_RTB, p_ARO, p_RtEntry, p_V);
            }
            else if (((p_V->Cost + A_GET_2B(p_Link->Metric)) == p_RtEntry->Cost) &&
                     (p_V->AreaId == p_RtEntry->AreaId)) /* ecmp paths must be in same area */
            {
              /* Copy next hops to this router into the route for the network */  
              CalculateNextHopToStub(p_RTB, p_ARO, p_RtEntry, p_V);

              /* RFC 2328 section 16.1. If this Link State Origin is a 
               * router-LSA whose Link State ID is smaller than V's 
               * Router ID, reset the Link State Origin to V's router-LSA. */
              if (A_GET_4B(p_RtEntry->p_DbEntry->Lsa.LsId) < 
                  A_GET_4B(p_V->p_DbEntry->Lsa.LsId))
              {
                p_RtEntry->p_DbEntry = p_V->p_DbEntry;   
              }
            }
          }
        }
      }
    else
    {  
      /* stub network entry not found */
      /* Add this stub network to the routing table  */
      if ((p_RtEntry = XX_Malloc(sizeof(t_RoutingTableEntry))) == NULL)
      {
        if (!p_RTO->LsdbOverload)
          EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
        return E_NOMEMORY;
      }

      /* copy the AreaId, VpnId, DestinationType, DestinationId, 
         Cost, etc.. from added "V" vertex */
      memcpy(p_RtEntry,p_V,sizeof(t_RoutingTableEntry));   

      p_RtEntry->DestinationId = rtbkey.DstAdr;
      p_RtEntry->IpMask        = rtbkey.Prefix;
      p_RtEntry->PathType = OSPF_INTRA_AREA;
      p_RtEntry->Cost = p_V->Cost + A_GET_2B(p_Link->Metric);
      p_RtEntry->DestinationType = DEST_TYPE_IS_STUB_NETWORK;
      p_RtEntry->PathNum = 0;
      p_RtEntry->ErNum = 0;
      p_RtEntry->PathPrm = NULL;

      if (p_ARO->AreaId == OSPF_BACKBONE_AREA_ID)
        p_RtEntry->BackboneReachable = TRUE;

      /* calculate next hop list */
        if (CalculateNextHopToStub(p_RTB, p_ARO, p_RtEntry, p_V) == E_OK)
      {
        if (RteUpdate(p_RTB, p_RtEntry) != E_OK)
          return E_FAILED;
      }
      else
          RteRelease(p_RtEntry, TRUE);
    }   
    } /* for loop over links in router LSA */
    e = AVLH_GetNext(p_ARO->SpfHl, (void *)&p_V, p_V);
  }   /* end while more vertices in SPT */
  return E_OK;
}

/*********************************************************************
 * @purpose           Dijkstra algorithm service routine.
 *
 *
 * @param p_RTB       @b{(input)}  Routing Table object handler
 * @param p_ARO       @b{(input)}  ARO object Id
 * @param p_V         @b{(input)}  source vertex V being processed
 * @param PrsEntry    @b{(input)}  destination described by Preparsing
 *                                 List Entry
 *
 * @returns           E_OK       success
 * @returns           E_NOMEMORY failed in memory allocation
 * @returns           E_FAILED   failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err SpfCheckLink(t_RTB *p_RTB, t_ARO *p_ARO, t_RoutingTableEntry *p_V, 
                          t_ParseEntry *PrsEntry)
{
   t_RTO *p_RTO = (t_RTO*) p_RTB->RTO_Id;
   t_RoutingTableEntry *p_W;
   t_RoutingTableKey rtbkey;
   t_A_DbEntry *candDbEntry;
   ulng cost;

   /* (b) If the LSA does not exist, or its LS age is equal to MaxAge, or
    * it does not have a link back to vertex V, examine the next link 
    * in V's LSA. Notice that the 2-way connectivity check is omitted.
    * That check is made when the parse entries are created.
    */
   candDbEntry = PrsEntry->p_DbEntry;
   if (candDbEntry == NULL)
       return E_OK;
   if (A_GET_2B(candDbEntry->Lsa.LsAge) >= MaxAge)
   {
       /* candidate LSA has aged out. skip it. */
       return E_OK;
   }

   /* Use 32-bit LSID as vertex ID for network LSAs. Don't mask LSID with
    * subnet mask. Could have multiple routers originate a network LSA for 
    * a given LAN (network partition or transient during DR election). 
    * RFC 2328 16.1. */
   rtbkey.DstAdr = A_GET_4B(PrsEntry->p_DbEntry->Lsa.LsId);
   if(PrsEntry->p_DbEntry->Lsa.LsType == S_NETWORK_LSA)
   {
      rtbkey.Prefix = A_GET_4B(PrsEntry->p_DbEntry->p_Lsa);
   }
   else
      rtbkey.Prefix = SPF_HOST_ROUTE;

   /* (c) If vertex W is already on the shortest-path tree,
		examine	the next link in the LSA. */
   if(AVLH_Find(p_ARO->SpfHl, (byte*)&rtbkey, (void *)&p_W,0) == E_OK)
      return E_OK;

   /* (d) Calculate the LS cost */
   cost = p_V->Cost + PrsEntry->metric;

   /* Find this W in candidate list */
   if(AVLH_Find(p_ARO->CandidateHl, (byte*)&rtbkey, (void *)&p_W,0) == E_OK)
   {
      /* if the calculated cost is greated than exist, goto next link */
      if (cost > p_W->Cost)
         return E_OK;

      /* If less, than set the W'cost to calculated value */
      if(cost < p_W->Cost)
      {
         p_W->Cost = cost;
         p_W->p_DbEntry = PrsEntry->p_DbEntry;
         RteRelease(p_W, FALSE); /* release recent paths */
      }
            
      /* If less or equal, calculate the set of next hops that result 
         from using this advertised link. 
         Add it to the found in candidate list W's next hop list */
      CalculateNextHop(p_RTB, p_ARO, p_W, p_V, PrsEntry);
   }
   else
   {
      /* Add this W to the candidate list */
      if((p_W = XX_Malloc(sizeof(t_RoutingTableEntry))) == NULL)
      {
        if (!p_RTO->LsdbOverload)
          EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
        return E_NOMEMORY;
      }
      memset(p_W, 0, sizeof(t_RoutingTableEntry));

      if(PrsEntry->p_DbEntry->Lsa.LsType == S_NETWORK_LSA)
      {
         p_W->DestinationType = DEST_TYPE_IS_NETWORK;
         p_W->DestinationId = rtbkey.DstAdr;
         p_W->IpMask = rtbkey.Prefix;
      }
      else
      {
         p_W->DestinationId = rtbkey.DstAdr;
         p_W->IpMask        = SPF_HOST_ROUTE; 
         p_W->OptionalCapabilities = *(PrsEntry->p_DbEntry->p_Lsa);
         p_W->DestinationType = DEST_TYPE_IS_ROUTER;
         if(p_W->OptionalCapabilities & S_EXTERNAL_FLAG)      
            p_W->DestinationType |= DEST_TYPE_IS_AS_BOUNDARY_ROUTER;
         if(p_W->OptionalCapabilities & S_BORDER_FLAG)
            p_W->DestinationType |= DEST_TYPE_IS_BORDER_ROUTER;
      }

      p_W->AreaId        = p_ARO->AreaId;
      p_W->Cost          = cost;
      p_W->p_DbEntry     = PrsEntry->p_DbEntry;
      p_W->PathType      = OSPF_INTRA_AREA;
      if(p_ARO->AreaId == OSPF_BACKBONE_AREA_ID)
         p_W->BackboneReachable = TRUE;

      /* And calculate next hop list */
      if(CalculateNextHop(p_RTB, p_ARO, p_W, p_V, PrsEntry) == E_OK)
      {
         if(AVLH_Insert(p_ARO->CandidateHl, (byte*)&p_W->DestinationId, p_W) != E_OK)
         {
            RteRelease(p_W, TRUE);
            return E_FAILED;
         }
      }
      else
         RteRelease(p_W, TRUE);
   }
   return E_OK;
}

/*********************************************************************
 * @purpose           Calculate list of next hops for a stub link
 *                    in stage 2 of SPF
 *
 *
 * @param  p_RTB      @b{(input)}Routing Table object handler
 * @param  p_ARO      @b{(input)}Area object handle
 * @param  p_W        @b{(input)}  destination vertex or route
 * @param  p_V        @b{(input)}  parent vertex. Router that advertised stub.
 *
 * @returns           E_OK - if NextHop successfully found,
 * @returns           E_FAILED - otherwise
 *
 * @notes      separating stub links from CalculateNextHop() to simplify logic.
 *             Removed all TE and VPN stuff.
 *
 * @end
 * ********************************************************************/
static e_Err CalculateNextHopToStub(t_RTB *p_RTB, t_ARO *p_ARO, 
                                    t_RoutingTableEntry *p_W,
                                    t_RoutingTableEntry *p_V)
{
  t_RTO *p_RTO = (t_RTO *) p_RTB->RTO_Id;
  SP_IPADR NextHopAdr = 0;
  t_IFO *p_IFO = NULLP;
  e_Err e;

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 parent[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_W->DestinationId, destStr);
    osapiInetNtoa(p_W->IpMask, maskStr);
    osapiInetNtoa(p_V->DestinationId, parent);
    sprintf(traceBuf, "Calculating next hop to stub %s/%s (%d) via parent %s (%d)",
            destStr, maskStr, p_W->DestinationType, parent, p_V->DestinationType);
    RTO_TraceWrite(traceBuf);
  }

  /* if parent is not root */
  if (p_V->DestinationId != p_RTO->Cfg.RouterId)
  {
    if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
    {
      RTO_TraceWrite("parent not the root. inheriting next hops from parent.");
    }
    /* the dest simply inherits the set of paths from the parent */
    PathCopy(p_RTO, p_W, p_V, 0, 0);
    return E_OK;
  }
    
  if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
  {
    RTO_TraceWrite("parent is the root router");
  }

  /* Stub network must be numbered. Since we are at the root, we
   * originated the router LSA. Our router LSA only adds type 3 links
   * for numbered networks. Find local interface in stub network. 
   * Use local interface address as next hop. */
  e = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
  while (e == E_OK)
  {
    int j;
    ulng combinedMask = p_IFO->Cfg.IpMask & p_W->IpMask;
    
    if ((p_IFO->Cfg.IpAdr & combinedMask) == (p_W->DestinationId & combinedMask))
    {
      NextHopAdr = p_IFO->Cfg.IpAdr;
      break;
    }

			for (j=1; j < IFO_MAX_IP_ADDRS; j++)
			{
				if (p_IFO->Cfg.addrs[j].ipAddr == 0)
				{
					continue;
				}
      combinedMask = p_IFO->Cfg.addrs[j].ipMask & p_W->IpMask;
      if ((p_IFO->Cfg.addrs[j].ipAddr & combinedMask) == 
          (p_W->DestinationId & combinedMask))
				{
        NextHopAdr = p_IFO->Cfg.addrs[j].ipAddr;
        break;
      }
    }

    if (NextHopAdr)
    {
      break;
    }

    e = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
  }

  if (NextHopAdr)
  {
    /* found interface to stub network */
    if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
    {
      char traceBuf[OSPF_MAX_TRACE_STR_LEN];
      L7_uchar8 nhAdrStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(NextHopAdr, nhAdrStr);
      sprintf(traceBuf, "Found interface to stub network. Next hop is %s", 
              nhAdrStr);
      RTO_TraceWrite(traceBuf);
    }
    PathAdd(p_W, p_IFO, NextHopAdr, NextHopAdr);
    return E_OK;
  }

  return E_FAILED;
}

/*********************************************************************
 * @purpose           Calculate list of next hops
 *
 * @param  p_RTB      @b{(input)}  Routing Table object handler
 * @param  p_ARO      @b{(input)}  Area object handle
 * @param  p_W        @b{(input)}  destination vertex or route
 * @param  p_V        @b{(input)}  parent vertex
 * @param  PrsEntry   @b{(input)}  parse entry. Used to identify local interface.
 *
 * @returns           E_OK - if NextHop successfully found,
 * @returns           E_FAILED - otherwise
 *
 * @notes    This function is called when the root router, represented by
 *           p_V, has one or more adjacencies with another router, represented
 *           by p_W, via point-to-point or virtual links. Use the neighbor's
 *           address for each adjacency as a next hop address.
 *           off parse entries. 
 *
 * @end
 * ********************************************************************/
static e_Err CalcNextHopToAdjRouter(t_RTB *p_RTB, t_ARO *p_ARO, 
                                    t_RoutingTableEntry *p_W,
                                    t_RoutingTableEntry *p_V, 
                                    t_ParseEntry *PrsEntry)
{
  t_RTO *p_RTO = (t_RTO*) p_ARO->RTO_Id;
  t_IFO *p_IFO;       /* interface on local router */
  t_IfoHashKey ifoKey;
  t_NBO *p_NBO;       /* neighbor of local router on p_IFO */
  e_Err ei, en;
  SP_IPADR nextHopAddr;
  ulng ifIndex;


  if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    L7_uchar8 ifoIdStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(PrsEntry->ifoId, ifoIdStr);
    osapiInetNtoa(p_W->DestinationId, destStr);
    sprintf(traceBuf, "Calculating next hop to adjacent router %s from interface %s", 
            destStr, ifoIdStr);
    RTO_TraceWrite(traceBuf);
  }

  if (PrsEntry->linkType == S_VIRTUAL_LINK)
  {
    /* find transit area of virtual link to remote router from given interface. */
    ei = HL_GetFirst(p_ARO->IfoHl, (void*) &p_IFO);
    while (ei == E_OK)
    {
      if ((p_IFO->Cfg.IpAdr == PrsEntry->ifoId) &&
          (p_IFO->IfoId == p_W->DestinationId))
        break;
      ei = HL_GetNext(p_ARO->IfoHl, (void*) &p_IFO, p_IFO);
    }
    if (ei == E_OK)
    {
      nextHopAddr = p_IFO->NextHopToVrtNbr;
      ifIndex = p_IFO->Cfg.IpAdr;
    }
    else
    {
      if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
      {
        char traceBuf[OSPF_MAX_TRACE_STR_LEN];
        L7_uchar8 ifoStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 virtNbrStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(PrsEntry->ifoId, ifoStr);
        osapiInetNtoa(p_W->DestinationId, virtNbrStr);
        sprintf(traceBuf, "Failed to find virtual interface with local address %s and "
                "remote neighbor %s.", 
                ifoStr, virtNbrStr);
        RTO_TraceWrite(traceBuf);
      }
      return E_FAILED;
    }
  }
  else if (PrsEntry->linkType == S_POINT_TO_POINT)
  {
    ifoKey.IfAddr = PrsEntry->ifoId;
    ifoKey.aro = NULL;
    ei = HL_FindFirst(p_ARO->IfoHl, (byte*) &ifoKey, (void *)&p_IFO);
    if (ei != E_OK)
    {
      if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
      {
        RTO_TraceWrite("local interface not found");
      }
      return E_FAILED;
    }

    en = HL_GetFirst(p_IFO->NboHl, (void *) &p_NBO);
    if (en != E_OK)
    {
      if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
      {
        RTO_TraceWrite("no neighbor on this interface");
      }
      return E_FAILED;
    }

    if ((p_NBO->RouterId == p_W->DestinationId) && 
        ((p_NBO->State == NBO_FULL) || o2NeighborIsRestarting(p_NBO)))
    {
      nextHopAddr = p_NBO->IpAdr;
      ifIndex = p_IFO->Cfg.IpAdr;
    }
    else
    {
      return E_FAILED;
    }
  }
  else
  {
    if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
    {
      char traceBuf[OSPF_MAX_TRACE_STR_LEN];
      sprintf(traceBuf, "Invalid link type %d in %s", 
              PrsEntry->linkType, __FUNCTION__);
      RTO_TraceWrite(traceBuf);
    }
    return E_FAILED;
  }

  
  if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    L7_uchar8 ifoStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 nextHopAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_IFO->IfoId, ifoStr);
    osapiInetNtoa(nextHopAddr, nextHopAddrStr);
    sprintf(traceBuf, "Selected next hop interface %s and next hop addr %s.", 
            ifoStr, nextHopAddrStr);
    RTO_TraceWrite(traceBuf);
  }
  PathAdd(p_W, p_IFO, nextHopAddr, ifIndex);

  return E_OK;
}

/*********************************************************************
 * @purpose           Calculate list of next hops
 *
 *
 * @param  p_RTB      @b{(input)}  Routing Table object handler
 * @param  p_ARO      @b{(input)}  Area object handle
 * @param  p_W        @b{(input)}  destination vertex or route
 * @param  p_V        @b{(input)}  parent vertex
 * @param  PrsEntry   @b{(input)}  parse entry. When parent is root router vertex,
 *                                 used to identify local interface.
 *
 * @returns           E_OK - if NextHop successfully found,
 * @returns           E_FAILED - otherwise
 *
 * @notes             Called during stage 1 of the intra-area SPF to determine
 *                    the next hop to a candidate vertex.
 *
 * @end
 * ********************************************************************/
static e_Err CalculateNextHop(t_RTB *p_RTB, t_ARO *p_ARO, t_RoutingTableEntry *p_W,
                              t_RoutingTableEntry *p_V, t_ParseEntry *PrsEntry)
{
  t_RTO *p_RTO = (t_RTO *) p_RTB->RTO_Id;
  t_S_RouterLsa *p_Lsa;
  t_S_LinkDscr *p_Link;
  t_OspfPathPrm *path;
  word link;
  SP_IPADR NextHopAdr = 0;
  t_IFO *p_IFO = NULLP;
  e_Err e;
  t_VpnCos VpnCos;
  t_IfoHashKey ifoKey;

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 parent[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 ifoIdStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_W->DestinationId, destStr);
    osapiInetNtoa(p_W->IpMask, maskStr);
    osapiInetNtoa(p_V->DestinationId, parent);
    osapiInetNtoa(PrsEntry->ifoId, ifoIdStr);
    sprintf(traceBuf, "Calculating next hop to %s/%s (%d) via parent %s (%d) for"
            " type %d link %s",
            destStr, maskStr, p_W->DestinationType, parent, p_V->DestinationType,
            PrsEntry->linkType, ifoIdStr);
    RTO_TraceWrite(traceBuf);
  }

  /* if parent is not root */
  if ((p_V->DestinationId != p_RTO->Cfg.RouterId) ||
      (p_V->DestinationType >= DEST_TYPE_IS_NETWORK))
  {
    if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
    {
      RTO_TraceWrite("parent not the root. inheriting next hops from parent.");
    }
    /* the dest simply inherits the set of paths from the parent */
    PathCopy(p_RTO, p_W, p_V, 0, 0);

    if ((p_V->DestinationType & DEST_TYPE_IS_NETWORK) && p_V->DirAttachNet &&
        (p_W->DestinationType < DEST_TYPE_IS_NETWORK))
    {
      /* second case: the parent is the directly attached network
         connected to the dest router */

      if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
      {
        RTO_TraceWrite("parent is directly attached network");
      }

      p_Lsa = (t_S_RouterLsa *) p_W->p_DbEntry->p_Lsa;
      p_Link = (t_S_LinkDscr *) ((byte*)p_Lsa + sizeof(t_S_RouterLsa));
      link = A_GET_2B(p_Lsa->LinkNum);
      while (link--)
      {
        /* for each link pointing back find the IFO interface attached to 
         * the parent root router */
        if (((A_GET_4B(p_Link->LinkId) & p_V->IpMask)== (p_V->DestinationId & p_V->IpMask)) &&
            (p_Link->Type == S_TRANSIT_NET))
        {    
          path = p_W->PathPrm;
          while (path)
          {
            /* Update only paths learned from p_V */
            if (path->NextHop == ((t_OspfPathPrm*)p_V->PathPrm)->NextHop)
            {
              path->IpAdr = (SP_IPADR) A_GET_4B(p_Link->LinkData);
            }
            path = path->next;
          }
        }
        p_Link = (t_S_LinkDscr*) (((byte*)p_Link) + 
                                  sizeof(t_S_LinkDscr) + p_Link->TosNum*sizeof(t_S_Tos));
      }
    }
  }
  else if (p_V->DestinationType < DEST_TYPE_IS_NETWORK)
  {
    /* first case: the parent is the root router vertex */

    if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
    {
      RTO_TraceWrite("parent is root router");
    }

    if (p_W->DestinationType < DEST_TYPE_IS_NETWORK)
    {
      /* Directly adjacent router. No network vertex in between. The routers
       * can be connected by p2p links or virtual links. Could be more than
       * one connection. */
      return CalcNextHopToAdjRouter(p_RTB, p_ARO, p_W, p_V, PrsEntry);
    }

    p_Link = RtrLsaCheckPresense((t_S_RouterLsa*) p_V->p_DbEntry->p_Lsa, 
                                 p_W->DestinationId, p_W->IpMask);
    if (!p_Link)
    {
      if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
      {
        RTO_TraceWrite("Failed to find matching link in root router LSA");
      }
      goto calc_ex;
    }

    if (p_Link->Type != S_TRANSIT_NET)
    {
      char traceBuf[OSPF_MAX_TRACE_STR_LEN];
      L7_uchar8 linkIdStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 linkDataStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(A_GET_4B(p_Link->LinkId), linkIdStr);
      osapiInetNtoa(A_GET_4B(p_Link->LinkData), linkDataStr);
      sprintf(traceBuf, "ERROR:  Processing unexpected link type."
              " Link in root router LSA has link type %d, link ID %s, link data %s",
              p_Link->Type, linkIdStr, linkDataStr);
      RTO_TraceWrite(traceBuf);
    }

    if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
    {
      char traceBuf[OSPF_MAX_TRACE_STR_LEN];
      L7_uchar8 linkIdStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 linkDataStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(A_GET_4B(p_Link->LinkId), linkIdStr);
      osapiInetNtoa(A_GET_4B(p_Link->LinkData), linkDataStr);
      sprintf(traceBuf, "Link in root router LSA has link type %d, link ID %s, link data %s",
              p_Link->Type, linkIdStr, linkDataStr);
      RTO_TraceWrite(traceBuf);
    }

    ifoKey.IfAddr = (SP_IPADR) A_GET_4B(p_Link->LinkData);  /* IP addr on local interface */
    ifoKey.aro = NULL;  
    if (HL_FindFirst(p_ARO->IfoHl, (byte*)&ifoKey, (void *)&p_IFO) != E_OK)
      return E_FAILED;
    
    /* If a broadcast network, identify it as locally attached. We'll use local
     * address as next hop. Don't do this if p2p network. */
    if ((p_IFO->Cfg.Type != IFO_PTP) || (A_GET_4B(p_Link->LinkData) != SPF_HOST_ROUTE))
    {
      if (p_W->DestinationType >= DEST_TYPE_IS_NETWORK)
      {
        if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
        {
          RTO_TraceWrite("setting NextHopAdr to local interface address.");
        }
         p_W->DirAttachNet = 1;
         NextHopAdr = ifoKey.IfAddr;
      }
    }

    /* set the IP address of the next router only for PMP */
    if ((p_W->DestinationType < DEST_TYPE_IS_NETWORK) && 
        (p_IFO->Cfg.Type == IFO_PTM))
    {
      p_Lsa = (t_S_RouterLsa *) p_W->p_DbEntry->p_Lsa;
      p_Link = (t_S_LinkDscr *) ((byte*)p_Lsa + sizeof(t_S_RouterLsa));
      link = A_GET_2B(p_Lsa->LinkNum);
      path = p_V->PathPrm;
      for (;link && path; link--)
      {
        if (p_Link->TosNum)
          VpnCos = A_GET_4B((byte*)(p_Link+1));
        else
          VpnCos = 0;

        /* for each link pointing back */
        if ((A_GET_4B(p_Link->LinkId) == p_V->DestinationId) && 
            (p_Link->Type == S_POINT_TO_POINT) &&
            !CheckDestPath(p_W, path->NextHop))
        {
          /* Add new path */
          PathAdd(p_W, p_IFO, A_GET_4B(p_Link->LinkData), p_IFO->Cfg.IpAdr);
        }
        path = path->next;
        p_Link = (t_S_LinkDscr*) (((byte*)p_Link) + 
                                  sizeof(t_S_LinkDscr) + p_Link->TosNum*sizeof(t_S_Tos));
      }
    }
      /* for other dest types : */
    else if (!CheckDestPath(p_W, p_IFO))
      {
         /* Add new path */
      if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_DETAIL)
      {
        char traceBuf[OSPF_MAX_TRACE_STR_LEN];
        L7_uchar8 nhAdrStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(NextHopAdr, nhAdrStr);
        sprintf(traceBuf, "Adding next hop %s to path.", nhAdrStr);
        RTO_TraceWrite(traceBuf);
      }
         PathAdd(p_W, p_IFO, NextHopAdr, p_IFO->Cfg.IpAdr);
      }
      
   } /* endif first case */  

calc_ex:

   /* Check if NextHop found */
   e = E_FAILED;
  for (path = p_W->PathPrm; path; path = path->next)
   {
    if (path->NextHop &&
         (path->IpAdr || p_W->DirAttachNet || 
         (((t_IFO*)path->NextHop)->Cfg.Type == IFO_PTP) || 
         (((t_IFO*)path->NextHop)->Cfg.Type == IFO_VRTL)))
      {
         e = E_OK;
         break;
      }
   }

   return e;
}



/*********************************************************************
 * @purpose          Routing table entry delete.
 *                   Serves the  browsing HL_Browse functio
 *
 * @param rte        @b{(input)}  routing table entry
 * @param AroId      @b{(input)}  Area object handler id
 *
 * @returns          FALSE if need to delete this entry 
 * @returns          TRUE  otherwise.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static Bool UnresolvedRteDelete(byte *rte, ulng AroId)
{
   t_RoutingTableEntry *p_Rte = (t_RoutingTableEntry *) rte;
   t_ARO *p_ARO = (t_ARO *) AroId;
   
   if((p_Rte->AreaId == p_ARO->AreaId) &&
      (p_Rte->OptionalCapabilities & S_VIRTUAL_FLAG) &&
      (p_Rte->PathNum == 0))
   {
      return FALSE;      /* route will be deleted within o2RoutesBrowse */
   }         
   return TRUE;
}






/*********************************************************************
 * @purpose         Routing table entry changes examination.
 *                  Serves the  browsing HL_Browse function
 *
 * @param p_Rte     @b{(input)}  routing table entry
 * @param RtbId     @b{(input)}  Router table object handler id
 *
 * @returns         FALSE if need to delete this entry by HL_Browse \
 *                  function
 * @returns         TRUE  otherwise.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool CheckDestPath(t_RoutingTableEntry *p_Rte, t_Handle ifoid)
{
   t_OspfPathPrm *path;

   for(path = p_Rte->PathPrm; path; path = path->next)
      if(path->NextHop == ifoid)
         return TRUE;
   
   return FALSE;
}

static void RteAdded(t_RTB *p_RTB, t_RoutingTableEntry *p_Rte)
{
   t_RTO *p_RTO = (t_RTO *) p_RTB->RTO_Id;
   t_ARO *p_ARO = NULL;
   ulng  rteCost= 0;

   p_RTB->RtbChanged = RTE_ADDED;

   if(p_Rte->isRejectRoute == FALSE)
   {
     p_ARO = (t_ARO *) p_Rte->p_DbEntry->ARO_Id;
     /* NSSA Changes */
     if(((p_Rte->PathType == OSPF_NSSA_TYPE_1_EXT) || 
         (p_Rte->PathType == OSPF_NSSA_TYPE_2_EXT)) &&
        (p_Rte->DestinationType >= DEST_TYPE_IS_NETWORK) &&
        (p_ARO->ExternalRoutingCapability == AREA_IMPORT_NSSA))
     {
       if(!UpdateNssaAggregationInfo(p_RTB, p_Rte, RTE_ADDED))
         NssaHandleRouteChange(p_RTB, p_Rte, RTE_ADDED, NULLP);
     }
   }
 
   /* if the router is ABR and network or ASBR route has been added */
   /* (but not a reject route) Summary_LSA has to be originated     */
   if((routerIsBorder(p_RTO)) && 
      ((p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER) ||
       (p_Rte->DestinationType >= DEST_TYPE_IS_NETWORK)) &&
      ((p_Rte->PathType == OSPF_INTRA_AREA) ||
       (p_Rte->PathType == OSPF_INTER_AREA)) &&
      (p_Rte->isRejectRoute == FALSE))
      ReorgSumLsa(p_RTO, p_Rte, RTE_ADDED);
   
   /* Optional trap RTB entry information */
   if(p_RTO->Clbk.p_OspfTrap &&
      (p_RTO->TrapControl & GET_TRAP_BITMASK(RTB_ENTRY_INFO)))
   {
      t_RtbEntryInfo info;
#if L7_OSPF_TE
      t_ErPrm *Er;
      word     i;
#endif
   
      memcpy(&info, &p_Rte->DestinationId, 
            offsetof(t_RoutingTableEntry,VrtlIfo) - 
            offsetof(t_RoutingTableEntry,DestinationId));
      info.Status = RTE_ADDED;
      info.NextHopNum = p_Rte->PathNum;
      if(p_Rte->PathPrm != NULL)
      {
         info.NextIpAdr = p_Rte->PathPrm->IpAdr; 
#if L7_OSPF_TE
         if(p_Rte->PathPrm->ErList)
         {
            for(i = 0, Er = p_Rte->PathPrm->ErList; Er && (i < 20); Er = Er->next)
            {
               info.RtrChain[i] = A_GET_4B(Er->RouterId);
               info.IfIndex[i] = A_GET_4B(Er->IfIndex);
            }
         }
#endif
      }
      p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, RTB_ENTRY_INFO, (u_OspfTrap*)&info);
   }

   /* optionaly inform the user about RTB entry adding */
   if(p_RTO->Clbk.p_RoutingTableChanged)
   {
      rteCost = (((p_Rte->PathType == OSPF_TYPE_2_EXT) || 
                 (p_Rte->PathType == OSPF_NSSA_TYPE_2_EXT)) ? 
                 p_Rte->Type2Cost : p_Rte->Cost);

      if (p_RTO->Clbk.p_RoutingTableChanged(p_RTO->Clbk.f_RtmAsyncEvent[0] ? 
          (t_Handle)p_RTO->Clbk.f_RtmAsyncEvent : p_RTO->MngId, 
          p_Rte->DestinationType,
          RTE_ADDED, p_Rte->DestinationId, p_Rte->IpMask,  
          p_Rte->PathNum, rteCost, p_Rte->PathPrm, p_Rte->PathType,
          p_Rte->isRejectRoute) == E_NO_ROOM)
      {
        if (!p_RTO->LsdbOverload)
        {
          L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_OSPF_MAP_COMPONENT_ID,
                 "Failed to add OSPFv2 route because routing table is full.");
          EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
        }
      }
   }
}

void RteChanged(t_RTB *p_RTB, t_RoutingTableEntry *p_Old, t_RoutingTableEntry *p_New)
{
   t_RTO *p_RTO = (t_RTO *) p_RTB->RTO_Id;
   t_ARO *p_nssaARO = NULL;
   ulng  rteCost= 0;
   e_RtChange actionType;

   /* Optional trap RTB entry information */
   if(p_RTO->Clbk.p_OspfTrap &&
      (p_RTO->TrapControl & GET_TRAP_BITMASK(RTB_ENTRY_INFO)))
   {
      t_RtbEntryInfo info;
#if L7_OSPF_TE
      t_ErPrm *Er;
      word     i;
#endif
   
      memcpy(&info, &p_New->DestinationId, 
            offsetof(t_RoutingTableEntry,VrtlIfo) - 
            offsetof(t_RoutingTableEntry,DestinationId));
      info.Status = RTE_MODIFIED;
      info.NextHopNum = p_New->PathNum;
      if(p_New->PathPrm != NULL)
      {
         info.NextIpAdr = p_New->PathPrm->IpAdr; 
#if L7_OSPF_TE
         if(p_New->PathPrm->ErList)
         {
            for(i = 0, Er = p_New->PathPrm->ErList; Er && (i < 20); Er = Er->next)
            {
               info.RtrChain[i] = A_GET_4B(Er->RouterId);
               info.IfIndex[i] = A_GET_4B(Er->IfIndex);
            }
         }
#endif
      }
      
      p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, RTB_ENTRY_INFO, (u_OspfTrap*)&info);
   }
   
   if(p_New->isRejectRoute == FALSE)
   {
     /* NSSA Changes */
     p_nssaARO = (t_ARO *) p_New->p_DbEntry->ARO_Id;
     if(((p_New->PathType == OSPF_NSSA_TYPE_1_EXT) || 
         (p_New->PathType == OSPF_NSSA_TYPE_2_EXT)) &&
        (p_New->DestinationType >= DEST_TYPE_IS_NETWORK) &&
        (p_nssaARO->ExternalRoutingCapability == AREA_IMPORT_NSSA))
     {
       if(!UpdateNssaAggregationInfo(p_RTB, p_New, RTE_MODIFIED))
         NssaHandleRouteChange(p_RTB, p_New, RTE_MODIFIED, NULLP);
     }
   }

   /* 12.4.3 last paragraph:  Also, if the destination is
    * still reachable, yet can no longer be advertised according
    * to the above procedure (e.g., it is now an inter-area route,
    * when it used to be an intra-area route associated with some
    * non-backbone area; it would thus no longer be advertisable
    * to the backbone), the LSA should also be flushed from the
    * routing domain.
    */
   if((routerIsBorder(p_RTO)) &&
      (p_New->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER) &&
      (p_Old->PathType == OSPF_INTRA_AREA) &&
      (p_New->PathType == OSPF_INTER_AREA))
   {
      ReorgSumLsa(p_RTO, p_New, RTE_REMOVED);
   }
   
   /* if the router is ABR and network or ASBR route has been */
   /* changed, Summary_LSA has to be originated               */
   else if((routerIsBorder(p_RTO)) &&
      ((p_New->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER) ||
       (p_New->DestinationType >= DEST_TYPE_IS_NETWORK)) &&
      ((p_New->PathType == OSPF_INTRA_AREA) ||
       (p_New->PathType == OSPF_INTER_AREA)))
   {
      /* don't originate summary lsa for the reject routes */
      if((p_Old->isRejectRoute == TRUE) &&
         (p_New->isRejectRoute == FALSE))
         ReorgSumLsa(p_RTO, p_New, RTE_ADDED);
      else if((p_Old->isRejectRoute == FALSE) &&
              (p_New->isRejectRoute == TRUE))
         ReorgSumLsa(p_RTO, p_Old, RTE_REMOVED);
      else
         ReorgSumLsa(p_RTO, p_New, RTE_MODIFIED);
   }
   p_RTB->RtbChanged = RTE_MODIFIED;
   
   /* update RTO. If the route type has not changed, modify the route else
    * Do a delete of the old route and an add of the new route, 
    * so that RTO is able to find the old route. If we just give RTO the new
    * route, then changes to the route (namely route type) makes it tough
    * for RTO to find the old route. */
   if (p_RTO->Clbk.p_RoutingTableChanged)
   {
      /* delete if path types dont match */
      if(p_Old->PathType != p_New->PathType)
      {
        rteCost = (((p_Old->PathType == OSPF_TYPE_2_EXT) || 
                   (p_Old->PathType == OSPF_NSSA_TYPE_2_EXT)) ? 
                   p_Old->Type2Cost : p_Old->Cost);

        p_RTO->Clbk.p_RoutingTableChanged((p_RTO->Clbk.f_RtmAsyncEvent[0] ? 
                                           (t_Handle)p_RTO->Clbk.f_RtmAsyncEvent : 
                                           p_RTO->MngId), 
                                          p_Old->DestinationType, RTE_REMOVED, 
                                          p_Old->DestinationId, p_Old->IpMask, 
                                          p_Old->PathNum, rteCost, 
                                          p_Old->PathPrm, p_Old->PathType,
                                          p_Old->isRejectRoute);
        /* if deleted, we need to add */
        actionType = RTE_ADDED; 
      }
      else
      {
        actionType = RTE_MODIFIED;
      }

      /* add or modify new route */
      rteCost = (((p_New->PathType == OSPF_TYPE_2_EXT) || 
                  (p_New->PathType == OSPF_NSSA_TYPE_2_EXT)) ? 
                  p_New->Type2Cost : p_New->Cost);

      p_RTO->Clbk.p_RoutingTableChanged((p_RTO->Clbk.f_RtmAsyncEvent[0] ? 
                                        (t_Handle)p_RTO->Clbk.f_RtmAsyncEvent : 
                                        p_RTO->MngId), 
                                        p_New->DestinationType, actionType, 
                                        p_New->DestinationId, p_New->IpMask, 
                                        p_New->PathNum, rteCost, 
                                        p_New->PathPrm, p_New->PathType,
                                        p_New->isRejectRoute);
   }
}

static void RteRemoved(t_RTB *p_RTB, t_RoutingTableEntry *p_Rte)
{
   t_RTO *p_RTO = (t_RTO *) p_RTB->RTO_Id;
   t_ARO *p_ARO = NULL;
   ulng  rteCost = 0;

   /* if the router is ABR and network or ASBR route has been */
   /* removed, Summary_LSA has to be reoriginated (flushed)   */
   if(((p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER) ||
       (p_Rte->DestinationType >= DEST_TYPE_IS_NETWORK)) &&
      ((p_Rte->PathType == OSPF_INTRA_AREA) ||
       (p_Rte->PathType == OSPF_INTER_AREA)) &&
      (p_Rte->isRejectRoute == FALSE))
         ReorgSumLsa(p_RTO, p_Rte, RTE_REMOVED);

   if(p_Rte->isRejectRoute == FALSE)
   {
     /* Update the address ranges as the Rte is going to be released below */
     if ((HL_FindFirst(p_RTO->AroHl, (byte*) &p_Rte->AreaId, (void *)&p_ARO) == E_OK) && p_ARO)
     {
       if(p_ARO->ExternalRoutingCapability == AREA_IMPORT_NSSA && 
          ((p_Rte->PathType == OSPF_NSSA_TYPE_1_EXT) || 
           (p_Rte->PathType == OSPF_NSSA_TYPE_2_EXT)) &&
          (p_Rte->DestinationType >= DEST_TYPE_IS_NETWORK))
       {
         if(!UpdateNssaAggregationInfo(p_RTB, p_Rte, RTE_REMOVED))
           NssaHandleRouteChange(p_RTB, p_Rte, RTE_REMOVED, NULLP);
       }
     }
   }

   p_RTB->RtbChanged = RTE_REMOVED;

   /* Optional trap RTB entry inforamtion */
   if(p_RTO->Clbk.p_OspfTrap &&
      (p_RTO->TrapControl & GET_TRAP_BITMASK(RTB_ENTRY_INFO)))
   {
      t_RtbEntryInfo info;
#if L7_OSPF_TE
      t_ErPrm *Er;
      word     i;
#endif
   
      memcpy(&info, &p_Rte->DestinationId, 
            offsetof(t_RoutingTableEntry,VrtlIfo) - 
            offsetof(t_RoutingTableEntry,DestinationId));
      info.Status = RTE_REMOVED;
      info.NextHopNum = p_Rte->PathNum;
      if(p_Rte->PathPrm)
      {
         info.NextIpAdr = p_Rte->PathPrm->IpAdr; 
#if L7_OSPF_TE
         if(p_Rte->PathPrm->ErList)
         {
            for(i = 0, Er = p_Rte->PathPrm->ErList; Er && (i < 20); Er = Er->next)
            {
               info.RtrChain[i] = A_GET_4B(Er->RouterId);
               info.IfIndex[i] = A_GET_4B(Er->IfIndex);
            }
         }
#endif
      }
      p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, RTB_ENTRY_INFO, (u_OspfTrap*)&info);
   }

   /* optionaly inform the user about RTB entry deleting */
   if(p_RTO->Clbk.p_RoutingTableChanged)
   {
      rteCost = (((p_Rte->PathType == OSPF_TYPE_2_EXT) || 
                 (p_Rte->PathType == OSPF_NSSA_TYPE_2_EXT)) ? 
                 p_Rte->Type2Cost : p_Rte->Cost);

      p_RTO->Clbk.p_RoutingTableChanged(p_RTO->Clbk.f_RtmAsyncEvent[0] ? 
       (t_Handle)p_RTO->Clbk.f_RtmAsyncEvent : p_RTO->MngId, 
       p_Rte->DestinationType, RTE_REMOVED, 
       p_Rte->DestinationId, p_Rte->IpMask, 
       p_Rte->PathNum, p_Rte->Cost, p_Rte->PathPrm, p_Rte->PathType,
       p_Rte->isRejectRoute);
   }
   
   RteRelease(p_Rte, TRUE);
}

/*--------------------------------------------------------------------
 * ROUTINE:    CheckAggrCostUpdate
 *
 * DESCRIPTION:
 *    Check if the aggregate cost needs to be updated
 * ARGUMENTS:
 *    t_RTO    Route Table Object Handle
 *    rteCost  Cost associated with the newly added/removed/modified route
 *             entry
 *    aggrCost Current cost associated with the aggregation
 *    
 * RESULTS:
 *     TRUE    If update required
 *     FALSE   If update not required
 *--------------------------------------------------------------------*/
Bool CheckAggrCostUpdate(t_RTO *p_RTO, long rteCost, long aggrCost)
{
  if(!p_RTO)
    return FALSE;

  /* The route is going to be removed and the aggregate will be updated
  ** in the RteRemoved path
  */
  if(rteCost >= LSInfinity)
    return FALSE;

  /* We need to use the largest cost of the component networks */
  if(rteCost > aggrCost)
    return TRUE;

  return FALSE;
}

/*********************************************************************
* @purpose  Check the Rte changes and create the reject route if the
*           no of contained routes is not zero, and there is no
*           intra-area route matching the reject route in the rtbntbt
*
* @param    p_RTB       @b{(input)} RTB object
* @param    p_AdrRange  @b{(input)} Address range entry
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
e_Err CreateRejectRoute(t_RTB *p_RTB, t_AgrEntry *p_AdrRange)
{
   t_RTO *p_RTO = (t_RTO*) p_RTB->RTO_Id; 
   t_RoutingTableEntry *p_RtEntry;
   word entriesNmb = 0;

   HL_GetEntriesNmb(p_AdrRange->AgrRteHl, &entriesNmb);
   if(entriesNmb)
   {
      /* add a reject route if contained route entries exist */
      p_RtEntry = o2RouteFind(p_RTB, p_RTB->RtbNtBt, p_AdrRange->NetIpAdr, p_AdrRange->SubnetMask);
      if (!p_RtEntry)
      {
         if ((p_RtEntry = XX_Malloc(sizeof(t_RoutingTableEntry))) == NULL)
         {
           if (!p_RTO->LsdbOverload)
             EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
           return E_NOMEMORY;
         }

         memset(p_RtEntry,0,sizeof(t_RoutingTableEntry));   

         p_RtEntry->DestinationId = p_AdrRange->NetIpAdr;
         p_RtEntry->IpMask        = p_AdrRange->SubnetMask;
         p_RtEntry->PathType = OSPF_INTER_AREA;  /* Section 11.1, RFC 2328 */
         p_RtEntry->Cost = 0;
         p_RtEntry->DestinationType = DEST_TYPE_IS_NETWORK;
         p_RtEntry->PathNum = 0;
         p_RtEntry->PathPrm = NULL;
         p_RtEntry->isRejectRoute = TRUE;
         if (o2RouteAdd(p_RTB, p_RTB->RtbNtBt, p_RtEntry) != E_OK)
         {
            RteRelease(p_RtEntry, TRUE);
            return E_FAILED;
         }
      }
   }

   return E_OK;
}

/*********************************************************************
* @purpose  Determine the active and inactive ranges
*
* @param  p_RTB      @b{(input)}Routing Table object handler
*
* @returns  None
*
* @notes   This function compares the old routing table and new routing
*          table updated with intra-area-calc to find the removed/added/
*          modified routes. And it updates the AgrRteHl of the address
*          ranges configured across all the areas
*
* @end
*********************************************************************/
static void DetermineActiveRanges(t_RTB *p_RTB)
{
   /* Compare the intra-area routes in the old and the new routing tables.
    * Use the comparision to update the ranges
    */
   e_Err e = E_OK, er = E_OK;
   e_Err ea;

   t_RoutingTableEntry *p_Rte, *p_RteOld, *p_RteNew;
   t_Handle oldTree    = p_RTB->RtbNtBtOld;
   t_Handle newTree    = p_RTB->RtbNtBt;
   t_RTO *p_RTO = (t_RTO *)(p_RTB->RTO_Id);
   t_ARO *p_ARO;
   t_ARO *p2_ARO;
   t_AgrEntry *p_AdrRange;

   /* Find the removed routes */
   /* Iterate through all routes in the old routing table. */
   e = o2RouteGetFirst(p_RTB, oldTree, &p_Rte);
   while(e == E_OK)
   {
      /* only consider the intra area paths in the old RT table
       * for removal in the new RT table */
      if(p_Rte->PathType == OSPF_INTRA_AREA)
      {
         /* Look for exact match for this route in the new routing table */
         p_RteNew = o2RouteFind(p_RTB, newTree, p_Rte->DestinationId, p_Rte->IpMask);
         if (p_RteNew == NULL)
         {
            /* This route is not found in new routing table */
            UpdateAdrRange(p_RTB, p_Rte, RTE_REMOVED);
         }
      }
      e = o2RouteGetNext(p_RTB, oldTree, &p_Rte);
   }

   /* Find the modified routes and new routes */
   /* Iterate through all routes in the new routing table. */
   e = o2RouteGetFirst(p_RTB, newTree, &p_Rte);

   while(e == E_OK)
   {
      /* all the routes at this point in the new RT table will be intra */
      if (p_Rte->PathType == OSPF_INTRA_AREA)
      {
         /* Look for exact match for this route in the old routing table */
         p_RteOld = o2RouteFind(p_RTB, oldTree, p_Rte->DestinationId, p_Rte->IpMask);
         if (p_RteOld && (p_RteOld->isRejectRoute == FALSE))
         {
            if (RTB_RoutesEqual(p_Rte, p_RteOld) == FALSE)
            {
               UpdateAdrRange(p_RTB, p_Rte, RTE_MODIFIED);
            }
         }
         else
         {
            UpdateAdrRange(p_RTB, p_Rte, RTE_ADDED);
         }
      }
      e = o2RouteGetNext(p_RTB, newTree, &p_Rte);
   }

   if(routerIsBorder(p_RTO))
   {
      /* add reject routes for the active ranges for all areas into RTB table
       * only if the router is an ABR */
      e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);   
      while(e == E_OK)
      {
        if (!p_ARO->TransitCapability)
        {
         /* Browse all configured ranges */
         er = HL_GetFirst(p_ARO->AdrRangeHl, (void *)&p_AdrRange);
         while(er == E_OK)
         {
            if((p_AdrRange->AggregateStatus == ROW_ACTIVE) &&
               (p_AdrRange->LsdbType == AGGREGATE_SUMMARY_LINK))
            {
                if (p_AdrRange->alive)
                {
               CreateRejectRoute(p_RTB, p_AdrRange);
    
                   ea = HL_GetFirst(p_RTO->AroHl, (void*) &p2_ARO);
                   while (ea == E_OK)
                   {
                     if (p2_ARO != p_ARO)
                     {
                       if (!o2AggrSummaryLsaCostCorrect(p_RTO, p2_ARO, p_AdrRange))
                       {
                         o2ReoriginateAggrSummary(p_RTO, p2_ARO, p_AdrRange);
                       }
                     }
                     ea = HL_GetNext(p_RTO->AroHl, (void*) &p2_ARO, p2_ARO);
                   }
                }
              }
              er = HL_GetNext(p_ARO->AdrRangeHl, (void **)&p_AdrRange,
                              (void*)p_AdrRange);
           }
         }
         e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
      }
   }
}

/*********************************************************************
* @purpose  Update the address range with the contained routing table
*           entry
*
* @param    p_RTB       @b{(input)} Routing table object handle
* @param    p_Rte       @b{(input)} routing table entry
* @param    chType      @b{(input)} route operation type
*
* @returns  None
*
* @notes    For a routing table entry, it determines if the route is
*           contained in any of the address ranges in its associated
*           area. If so, it updates the address ranges attributes
*           like "alive" field, AgrRteHl (list of contained rtes)
*           Only intra-area routes are considered.
*
* @end
*********************************************************************/
static void UpdateAdrRange(t_RTB *p_RTB, t_RoutingTableEntry *p_Rte,
                           e_RtChange chType)
{
   t_ARO         *p_ARO;
   t_RTO         *p_RTO = (t_RTO *)p_RTB->RTO_Id;
   t_AgrEntry    *p_AdrRange;
   e_Err          er = E_FAILED;

   /* Addr aggregation is relevant only for intra-area network routes */
   if((p_Rte->DestinationType < DEST_TYPE_IS_NETWORK) ||
      ((p_Rte->PathType != OSPF_INTRA_AREA) && (chType != RTE_MODIFIED)))
      return;

   /* Get area object handle of the RT entry*/
   if(HL_FindFirst(p_RTO->AroHl,(byte*)&p_Rte->AreaId,
                                  (void **)&p_ARO) != E_OK)
      return;

   /* Browse all configured ranges */
   er = HL_GetFirst(p_ARO->AdrRangeHl, (void *)&p_AdrRange);
   while(er == E_OK)
   {
      /* Check if the destination belongs to aggregation */
      if((p_AdrRange->AggregateStatus == ROW_ACTIVE) &&
         (p_AdrRange->LsdbType == AGGREGATE_SUMMARY_LINK) &&
         ((p_Rte->DestinationId & p_AdrRange->SubnetMask) == 
          (p_AdrRange->NetIpAdr & p_AdrRange->SubnetMask)))
      {
          UpdateAggregationInfo(p_RTB, p_AdrRange, p_Rte, chType);
          return;
      }
      er = HL_GetNext(p_ARO->AdrRangeHl, (void **)&p_AdrRange,
                      (void*)p_AdrRange);
   }
}

/*********************************************************************
* @purpose  Update the address range attributes depending on the 
*           contained routing table entry
*
* @param    p_RTB       @b{(input)} RTB object
* @param    p_AdrRange  @b{(input)} Address range entry
* @param    p_Rte       @b{(input)} routing table entry
* @param    chType      @b{(input)} route operation type
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void UpdateAggregationInfo(t_RTB *p_RTB, t_AgrEntry *p_AdrRange,
                           t_RoutingTableEntry *p_Rte, e_RtChange chType)
{
   t_ARO         *p_ARO;
   t_RTO         *p_RTO = (t_RTO *)p_RTB->RTO_Id;
   t_A_DbEntry   *p_DbEntry = NULLP;
   e_Err          e = E_FAILED;
   word           entriesNmb = 0;

   /* Update the list of RT entries belonging to the aggregation */
   if (chType == RTE_REMOVED)
   {
      e = HL_Delete(p_AdrRange->AgrRteHl,(byte*)&p_Rte->DestinationId,NULL);
   }
   else if (chType == RTE_ADDED)
   {
      HL_Insert(p_AdrRange->AgrRteHl,(byte*)&p_Rte->DestinationId,p_Rte);
   }

   /* If the route entry has been changed we need to account for a situation
   ** where a route's accessibility may have changed from intra-area to inter-area
   ** or vice versa
   */ 
   else if (chType == RTE_MODIFIED)
   {
     if(p_Rte->PathType == OSPF_INTRA_AREA)
     {
       /* Replace route in range hash list */
       if (HL_Change(p_AdrRange->AgrRteHl, (byte*)&p_Rte->DestinationId, p_Rte) == E_NOT_FOUND)
       {
         /* Route not previously in HL, so add it. */
         HL_Insert(p_AdrRange->AgrRteHl, (byte*)&p_Rte->DestinationId, p_Rte);
       }
     }
     else
     {
       /* If the changed route is now accessible only via an inter-area path and it was previously
       ** included in the aggregate route list, delete it as it no longer qualifies as it is no
       ** longer an intra-area path
       */
       HL_Delete(p_AdrRange->AgrRteHl, (byte*)&p_Rte->DestinationId, NULL);
     }
   }

   HL_GetEntriesNmb(p_AdrRange->AgrRteHl, &entriesNmb);
   /* If the aggregation is empty; flush its Summary LSA */
   if(!entriesNmb)
   {
      p_AdrRange->alive = FALSE;
      o2AggregateCostReset(p_AdrRange);
      
      e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);   
      while(e == E_OK)
      {
         if(FindSummaryOrASExtLsa(p_RTO, p_ARO, S_IPNET_SUMMARY_LSA,
               p_AdrRange->NetIpAdr, p_AdrRange->SubnetMask,
               &p_DbEntry, NULLP) == E_OK)
            FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);

         e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
      }
      return;
   }

   /* The aggregation is not empty. It should be advertised. */
   p_AdrRange->alive = TRUE;
      
   if (o2AggregateCostIsSet(p_AdrRange))
   {
     /* Attempt an incremental update of the aggregation cost */
   if(chType == RTE_ADDED)
   {
     if(CheckAggrCostUpdate(p_RTO, p_Rte->Cost, p_AdrRange->LargestCost) == TRUE)
     {
       p_AdrRange->LargestCost = p_AdrRange->AggregateRte.Cost = p_Rte->Cost;
       p_AdrRange->LcRte = p_Rte;
     }
   }
   else if(chType == RTE_REMOVED)
   {
       if ((p_Rte->Cost >= p_AdrRange->LargestCost) &&
           (p_Rte == p_AdrRange->LcRte))
       {
         /* In the remove case, p_Rte is from the old route table. If it is
          * the largest cost route, then update the aggregate cost. Since the 
          * route has been removed, we won't try to generate a T3 
          * LSA for it. So need to find the new largest cost route here. */
         o2ComputeAggregateCost(p_RTO, p_AdrRange);
       }
   }
   else if(chType == RTE_MODIFIED)
   {
       if (CheckAggrCostUpdate(p_RTO, p_Rte->Cost, p_AdrRange->LargestCost) == TRUE)
      {
         p_AdrRange->LargestCost = p_AdrRange->AggregateRte.Cost = p_Rte->Cost;
         p_AdrRange->LcRte = p_Rte;
       }
       else
       {
         /* Just reset aggregate cost. Will be recomputed when T3 LSA for 
          * modified route is generated. */
         o2AggregateCostReset(p_AdrRange);
       }
     }
   }

   return;
}

/*********************************************************************
* @purpose  Reset the largest cost of an aggregate route to "unknown."
*
* @param    p_AdrRange  @b{(input)} Address range entry
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void o2AggregateCostReset(t_AgrEntry *p_AdrRange)
{
  p_AdrRange->LargestCost = O2_AGGR_COST_UNKNOWN;
  p_AdrRange->AggregateRte.Cost = O2_AGGR_COST_UNKNOWN;      
  p_AdrRange->LcRte = NULL;
}

/*********************************************************************
* @purpose  Determine whether the largest cost is set for a given
*           summary range.
*
* @param    p_AdrRange  @b{(input)} Address range entry
*
* @returns  TRUE if largest cost has been set
*
* @notes
*
* @end
*********************************************************************/
Bool o2AggregateCostIsSet(t_AgrEntry *p_AdrRange)
{
  return (p_AdrRange->LargestCost != O2_AGGR_COST_UNKNOWN);
}

/*********************************************************************
* @purpose  Compute the cost to be advertised in the summary LSA for an
*           aggregate route. This is the largest cost from among all
*           contained routes. 
*
* @param    p_AdrRange  @b{(input)} Address range entry
*
* @returns  cost
*
* @notes
*
* @end
*********************************************************************/
long o2ComputeAggregateCost(t_RTO *p_RTO, t_AgrEntry *p_AdrRange)
{
  e_Err e;
  t_RoutingTableEntry *rte;

  o2AggregateCostReset(p_AdrRange);    
  e = HL_GetFirst(p_AdrRange->AgrRteHl, (void**)&rte);
  while (e == E_OK)
  {
    if (((long) (rte->Cost) < LSInfinity) && ((long) rte->Cost > p_AdrRange->LargestCost))
    {
      p_AdrRange->LargestCost = p_AdrRange->AggregateRte.Cost = rte->Cost;
      p_AdrRange->LcRte = rte;
    }
    e = HL_GetNext(p_AdrRange->AgrRteHl, (void**)&rte, rte);
  }
  return p_AdrRange->LargestCost;
}


/*********************************************************************
* @purpose  From among the routes matching an NSSA range, find the 
*           route with the largest cost. Update the range to point
*           to this route and use the largest cost as the cost of 
*           the summary route.
*
* @param    p_AdrRange  @b{(input)} NSSA address range
*
* @returns  E_OK if at least one route in range and LC route found
*           E_FAILED if range not a T7 range or no LC route found
*
* @notes    RFC 3101 section 3.2. If any of these LSAs have a path type of
          2, the range's path type is 2, otherwise it is 1.  If the
          range's path type is 1 its metric is the highest cost amongst
          these LSAs; if the range's path type is 2 its metric is the
          highest Type-2 cost + 1 amongst these LSAs.  

*         The extra 1 is added later when we create the T5 LSA.
*
* @end
*********************************************************************/
e_Err NssaRangeLcRouteFind(t_AgrEntry *p_AdrRange)
{
  e_Err e = E_FAILED;
  t_RoutingTableEntry *rte;
  Bool type2PathExists = FALSE;

  if (p_AdrRange->LsdbType != AGGREGATE_NSSA_EXTERNAL_LINK)
    return E_FAILED;

  p_AdrRange->LargestCost = 0;
  p_AdrRange->AggregateRte.Cost = 0;
  p_AdrRange->LcRte = NULL;

  /* First spin through the contained routes and see if range 
   * route will have a type 1 or 2 metric. */
  e = HL_GetFirst(p_AdrRange->AgrRteHl, (void**)&rte);
  while(e == E_OK)
  {
    if (rte->PathType == OSPF_NSSA_TYPE_2_EXT)
    {
      /* gonna advertise a type 2 metric with the range */
      type2PathExists = TRUE;
      break;
    }
    e = HL_GetNext(p_AdrRange->AgrRteHl, (void**)&rte, rte);
  }

  /* Go through again */
  e = HL_GetFirst(p_AdrRange->AgrRteHl, (void**)&rte);
  while (e == E_OK)
  {
    if ((type2PathExists == TRUE) && (rte->PathType == OSPF_NSSA_TYPE_2_EXT))
    {
      if ((p_AdrRange->LcRte == NULL) || (rte->Type2Cost > p_AdrRange->LargestCost))
      {
        if (rte->Type2Cost < LSInfinity)
        {
          p_AdrRange->LargestCost = rte->Type2Cost;
          p_AdrRange->LcRte = rte;
        }
      }
    }
    else if (type2PathExists == FALSE)
    {
      if ((p_AdrRange->LcRte == NULL) || (rte->Cost > p_AdrRange->LargestCost))
      {
        if (rte->Cost < LSInfinity)
        {
          p_AdrRange->LargestCost = rte->Cost;
          p_AdrRange->LcRte = rte;
        }
      }
    }

    e = HL_GetNext(p_AdrRange->AgrRteHl, (void**)&rte, rte);
  }
  if (p_AdrRange->LcRte == NULL)
  {
    p_AdrRange->LargestCost = -1;
    return E_FAILED;
  }

  if (type2PathExists)
    p_AdrRange->AggregateRte.Type2Cost = p_AdrRange->LargestCost;
  else
    p_AdrRange->AggregateRte.Cost = p_AdrRange->LargestCost;
  return E_OK;
}

/* This routine updates the Type-7 Area Aggregate entry.  If needed a Type-5 external
** LSA will be originated.  A return value of TRUE is returned if a Type-5 LSA was 
** originated/handled (for e.g. flushed)
*/
Bool UpdateNssaAggregationInfo (t_RTB *p_RTB, t_RoutingTableEntry *p_Rte,
                                e_RtChange chType)
{
   t_ARO         *p_ARO;
   t_RTO         *p_RTO;
   t_AgrEntry    *p_AdrRange;
   e_Err          er = E_FAILED;
   word           entriesNmb = 0;
   t_A_DbEntry   *p_DbEntry = NULLP;
   t_RoutingTableEntry *p_AgrRte, *lcRte = NULL;
   Bool           prevLcRouteExists = FALSE;    /* TRUE if matching range already has an LC route */
   e_OspfPathType prevPathType = OSPF_NSSA_TYPE_1_EXT;
   ulng           prevCost = 0, prevType2Cost = 0;


   /* This routine only handles Type-7 Aggregates */
   if(!p_RTB || (p_Rte->DestinationType < DEST_TYPE_IS_NETWORK) ||
      (p_Rte->DestinationId == DefaultDestination) ||
      ((p_Rte->PathType != OSPF_NSSA_TYPE_1_EXT) && (p_Rte->PathType != OSPF_NSSA_TYPE_2_EXT)) ||
      (p_Rte->p_DbEntry->Lsa.LsType != S_NSSA_LSA))
     return FALSE;

   p_RTO = (t_RTO *)p_RTB->RTO_Id;

   /* Get area object handle of the RT entry*/
   if((HL_FindFirst(p_RTO->AroHl,(byte*)&p_Rte->AreaId, (void **)&p_ARO) != E_OK) ||
      (p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA))
     return FALSE;

   /* Browse all configured ranges */
   er = HL_GetFirst(p_ARO->AdrRangeHl, (void *)&p_AdrRange);
   while(er == E_OK)
   {
      /* Check if the destination belongs to a type-7 aggregation */
      if((p_AdrRange->AggregateStatus != ROW_ACTIVE) ||
         (p_AdrRange->LsdbType != AGGREGATE_NSSA_EXTERNAL_LINK) ||
         ((p_Rte->DestinationId & p_AdrRange->SubnetMask) != 
          (p_AdrRange->NetIpAdr & p_AdrRange->SubnetMask)))
        goto next_aggregation;

      /* Update the list of RT entries belonging to the aggregation */
      if(chType == RTE_REMOVED)
      {
        RemoveRouteFromT7Range(p_RTO, p_AdrRange, p_Rte);
      }

      if(chType == RTE_ADDED)
      {
        AddRouteToT7Range(p_RTO, p_AdrRange, p_Rte);
      }

      if(chType == RTE_MODIFIED)
      {
        if(HL_FindFirst(p_AdrRange->AgrRteHl, (byte*)&p_Rte->DestinationId, (void **)&p_AgrRte) == E_OK)
        {
          RemoveRouteFromT7Range(p_RTO, p_AdrRange, p_Rte);
          AddRouteToT7Range(p_RTO, p_AdrRange, p_Rte);
        }
      }

      HL_GetEntriesNmb(p_AdrRange->AgrRteHl, &entriesNmb);

      /* If the aggregation is empty; flush the corresponding External LSA */
      if(!entriesNmb)
      {
        p_AdrRange->alive = FALSE;
        p_AdrRange->LargestCost = p_AdrRange->AggregateRte.Cost = -1;
        p_AdrRange->LcRte = NULL;
        
        if(FindSummaryOrASExtLsa(p_RTO, p_ARO, S_AS_EXTERNAL_LSA,
                                 p_AdrRange->NetIpAdr, p_AdrRange->SubnetMask,
                                 &p_DbEntry, NULLP) == E_OK)
        {
          FlushAsLsa((t_Handle)p_DbEntry, (ulng)p_RTO);
        }
         
        return TRUE;
      }

      /* If we get here the aggregation is not empty and we need to update 
      ** the aggregation cost 
      */
      p_AdrRange->alive = TRUE;

      /* store the previous pathtype/metric values for later comparision */
      lcRte = (t_RoutingTableEntry *)p_AdrRange->LcRte;
      if(lcRte)
      {
        prevLcRouteExists = TRUE;
        prevPathType = lcRte->PathType;
        prevType2Cost = lcRte->Type2Cost;
        prevCost = lcRte->Cost;
      }

      if (NssaRangeLcRouteFind(p_AdrRange) != E_OK)
        return FALSE;
      
      break;

next_aggregation:
      er = HL_GetNext(p_ARO->AdrRangeHl, (void **)&p_AdrRange,
                         (void*)p_AdrRange);
   }

   /* If our translator state is disabled or we did not find a matching aggregation
   ** entry return */
   if((er != E_OK) || (p_ARO->NSSATranslatorState == NSSA_TRANS_STATE_DISABLED))
     return FALSE;

   /* If the LcRte has changed, originate the aggregate summary again */
   /* Change in cost, pathtype, forwarding address */
   lcRte = (t_RoutingTableEntry *)p_AdrRange->LcRte;
   if (lcRte && 
       (!prevLcRouteExists ||
        ((prevPathType != lcRte->PathType) ||
         (prevType2Cost != lcRte->Type2Cost) ||
         (prevCost != lcRte->Cost) || (chType == RTE_MODIFIED))))
   {
     p_AgrRte = lcRte;
     NssaHandleRouteChange(p_RTB, p_AgrRte, RTE_ADDED, p_AdrRange);
   }

   return TRUE;
}

/*********************************************************************
* @purpose  When a inter-area or intra-area route is added, changed, or
*           deleted, consider whether the change affects the T3 summary
*           LSAs this router should originate.
*
* @param    p_RTO  @b{(input)}  OSPF instance
* @param    p_Rte  @b{(input)}  the changed route
* @param    chType @b{(input)}  add/mod/del
*
* @returns  void
*
* @notes  
*
* @end
*********************************************************************/
static void ReorgSumLsa(t_RTO *p_RTO, t_RoutingTableEntry *p_Rte,
                        e_RtChange chType)
{
   t_ARO *p_ARO;
   e_Err e, er;
   t_A_DbKey key;
   t_A_DbEntry *p_DbEntry;

   e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);   
   while(e == E_OK)
   {
      Bool FlushSummary = FALSE;

      /* We should not originate a summary LSA into an area where
       * we learn an intra-area or inter-area route to the same 
       * destination. So regardless of change type, flush a matching
       * summary LSA from the route's area. A default route is the
       * one exception. An ABR never learns an intra-area or 
       * inter-area default route. */
      if ((p_Rte->AreaId == p_ARO->AreaId) && 
          (p_Rte->DestinationId != DefaultDestination))
      {
         FlushSummary = TRUE;
      }

      /* All inter-area routes are associated with the backbone. Don't 
       * originate a summary into the backbone for this network. */
      if((p_ARO->AreaId == OSPF_BACKBONE_AREA_ID) && 
         (p_Rte->PathType == OSPF_INTER_AREA))
      {
         FlushSummary = TRUE;
      }

      /* Route has been removed. Remove associated summary. 
       * Again, default route is an exception. */
      if ((chType == RTE_REMOVED) && 
          (p_Rte->DestinationId != DefaultDestination))
      {
         FlushSummary = TRUE;
      }

      if(!FlushSummary)
      {
         e_S_LScodes LsType;
         
         LsType = (p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER) ?
                                     S_ASBR_SUMMARY_LSA : S_IPNET_SUMMARY_LSA;

         LsaReOriginate[LsType](p_ARO, (ulng)p_Rte);
      }
      else
      {
         /* Flush the entry - if the destination becomes unreachable or it is */
         /* still reachable, but can no longer be advertised (section 12.4.3) */        
         A_SET_4B(p_RTO->Cfg.RouterId, key.AdvertisingRouter);
         A_SET_4B(p_Rte->DestinationId, key.LsId);
         
         er = E_FAILED;
         if(p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER)
         {
            er = AVLH_Find(p_ARO->AsBoundSummaryLsaHl, (byte*) &key, (void *)&p_DbEntry,0);
         }
         else if (p_Rte->DestinationType >= DEST_TYPE_IS_NETWORK)
         {
            er =  FindSummaryOrASExtLsa(p_RTO, p_ARO, S_IPNET_SUMMARY_LSA,
                                       p_Rte->DestinationId, p_Rte->IpMask,
                                       &p_DbEntry, NULLP);
            /* If we are originating a T3 summary for the same prefix into this 
             * area for an active area range in another area, then don't flush
             * the summary. */
            if (o2OriginatingRangeSummary(p_RTO, p_ARO, p_Rte->DestinationId, p_Rte->IpMask))
            {
              /* Don't flush summary LSA from this area */
              er = E_NO_MATCH;
            }
         }
            
         if((er == E_OK) && (A_GET_2B(p_DbEntry->Lsa.LsAge) < MaxAge)) 
         {
           if ((p_Rte->PathType == OSPF_INTRA_AREA) ||
               (p_Rte->PathType == OSPF_INTER_AREA))
             FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);
           else
             FlushAsLsa((t_Handle)p_DbEntry, (ulng)p_RTO);
         }
      }

      e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);   
   }
}

/*********************************************************************
 * @purpose           Find the Virtual Neighbor IP ADR
 *
 *
 * @param p_RTB       @b{(input)}  Routing Table object handler
 * @param p_DbEntry   @b{(input)}  Virtual Neighbor Router LSA
 *
 * @returns           Virtual Neighbor IP ADR or NULL.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static SP_IPADR FindVrtNgbIpAdr(t_RTB *p_RTB, t_A_DbEntry *p_DbEntry)
{
   t_S_LinkDscr *p_Link;
   t_S_RouterLsa *p_Lsa;
   word link;
   t_VpnCos LinkVpnCos;
   ulng best_cost;
   t_RoutingTableKey rtbkey;
   t_RoutingTableEntry *p_Rte;
   SP_IPADR ipmask, ipadr, first_ipadr;
   SP_IPADR candidateAddr;
   t_RTO *p_RTO = (t_RTO*) p_RTB->RTO_Id;

   if(!(L7_BIT_ISSET(p_DbEntry->dbFlags, O2_DB_ENTRY_IS_CUR)))
      return 0;

   best_cost = 0xFFFF;
   p_Lsa = (t_S_RouterLsa *)p_DbEntry->p_Lsa;
   link = A_GET_2B(p_Lsa->LinkNum);
   p_Link = (t_S_LinkDscr *) ((byte*)p_Lsa + sizeof(t_S_RouterLsa));
   ipadr = 0;
   first_ipadr = 0;

   while(link--)
   {
      if ((p_Link->Type != S_VIRTUAL_LINK) && (p_Link->Type != S_STUB_NET))
      {
         if(p_Link->TosNum)
            LinkVpnCos = A_GET_4B((byte*)(p_Link+1));
         else
            LinkVpnCos = 0;
         
         /* Avoid p2p link for unnumbered interface where Link Data is 
          * interface index, not interface address. */
         candidateAddr = A_GET_4B(p_Link->LinkData);
         if (candidateAddr > 0x01000000)
         {
           rtbkey.DstAdr = candidateAddr;
           rtbkey.Prefix = 0;

          if(!first_ipadr)
             first_ipadr = rtbkey.DstAdr;

          /* Find the routing table entry best match to required destination */
          p_Rte = o2LongestPrefixMatch(p_RTB, p_RTB->RtbNtBt, candidateAddr);
          if ((p_Rte && p_Rte->PathNum) &&
              (p_Rte->AreaId == ((t_ARO*)(p_DbEntry->ARO_Id))->AreaId))
          {
             /* Check if found entry has fitted subnet mask */
             ipmask = p_Rte->IpMask ? p_Rte->IpMask : 0xFFFFFFFFL;
             if((rtbkey.DstAdr & ipmask) == (p_Rte->DestinationId & ipmask))
             {
                if(p_Rte->Cost < best_cost)
                {
                   best_cost = p_Rte->Cost;
                   ipadr = rtbkey.DstAdr;
                }
             }
          }
         }
      }
      p_Link = (t_S_LinkDscr*) (((byte*)p_Link) + 
         sizeof(t_S_LinkDscr) + p_Link->TosNum*sizeof(t_S_Tos));
   }

   if(!ipadr && first_ipadr)
      ipadr = first_ipadr;

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_VIRT_LINK)
   {
       char traceBuf[OSPF_MAX_TRACE_STR_LEN];
       L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
       L7_uchar8 vNbrIdStr[OSAPI_INET_NTOA_BUF_SIZE];
       L7_uchar8 vNbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
       t_ARO *transitArea = (t_ARO*) p_DbEntry->ARO_Id;
       t_S_LsaHeader *p_Lsa = (t_S_LsaHeader*) &p_DbEntry->Lsa;
       osapiInetNtoa(ipadr, vNbrAddrStr);
       osapiInetNtoa(transitArea->AreaId, areaIdStr);
       osapiInetNtoa(A_GET_4B(p_Lsa->LsId), vNbrIdStr);
       sprintf(traceBuf, "Selected address %s for virtual neighbor %s in area %s",
               vNbrAddrStr, vNbrIdStr, areaIdStr);
       RTO_TraceWrite(traceBuf);
   }

   return ipadr;
}


void CheckVirtLinkUp(t_IFO *p_IFO, t_RTB *p_RTB)
{
  VlinkStatusUpdate((t_Handle)p_IFO, (ulng)p_RTB);
}

static Bool VlinkStatusUpdate(t_Handle ifo, ulng RtbId)
{
  t_IFO               *p_IFO = (t_IFO *)ifo, *p_nhIFO = NULLP;
  t_RoutingTableEntry *p_Rte = NULLP;
  t_ARO               *p_ARO = NULLP;
  t_RTO               *p_RTO = NULLP;
  t_RTB               *p_RTB = (t_RTB *)RtbId;
  t_S_IfMetric        ifo_metric;
  SP_IPADR            adr = OSPF_BACKBONE_AREA_ID;

  /* This function is only executed for virtual interfaces */
  if (p_IFO->Cfg.Type != IFO_VRTL)
    return TRUE;

  p_RTO = (t_RTO *)p_RTB->RTO_Id;

  if(HL_FindFirst(p_RTO->AroHl, (byte *)&p_IFO->Cfg.VirtTransitAreaId, 
                  (void *)&p_ARO) == E_OK)
  {
    /* Update TransitARO. If transit area got deleted (OSPF global disable or 
     * OSPF disabled on last intf in transit area), pointer may have been set to
     * NULL. */
    p_IFO->TransitARO = p_ARO;

    /* Since we cannot (yet) lookup a 
     * ABR route for a specific area, look up the route on the area's
     * shortest path tree. The SPT hangs around between SPF runs. */
    if (RteIntraAreaFindByType(p_ARO,p_IFO->Cfg.VirtIfNeighbor, 
                               DEST_TYPE_IS_BORDER_ROUTER, &p_Rte) == E_OK)
    {
        if ((p_Rte->AreaId == p_ARO->AreaId) &&
          ((p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER) || 
           (p_Rte->DestinationType & DEST_TYPE_IS_BORDER_ROUTER)))
      {
        if((p_Rte->Cost < 0xffff) && (A_GET_2B(p_Rte->p_DbEntry->Lsa.LsAge) < MaxAge))
        {
            if(HL_FindFirst(p_RTO->AroHl, (byte *)&adr, (void *)&p_ARO) == E_OK)
            {
              p_nhIFO = (t_IFO *)p_Rte->PathPrm->NextHop;

              /* Get cost of the virtual IFO to the RTB entry's cost */
              memset(&ifo_metric,0,sizeof(t_S_IfMetric));
              ifo_metric.MetricStatus = ROW_READ;
              IFO_MetricConfig(p_IFO, &ifo_metric);

              /* If there has been no change to the route return */
              if((p_IFO->Cfg.IpAdr == p_nhIFO->Cfg.IpAdr) && (p_IFO->LowLayerId == p_nhIFO->LowLayerId) &&
                 (ifo_metric.Metric == p_Rte->Cost) && (p_IFO->Cfg.State != IFO_DOWN))
                return TRUE;

              p_IFO->Cfg.IpAdr = p_nhIFO->Cfg.IpAdr;
              p_IFO->Cfg.IpMask = p_nhIFO->Cfg.IpMask;
              p_IFO->VirtIfIndex = p_nhIFO->Cfg.IfIndex;
              p_IFO->NextHopToVrtNbr = p_Rte->PathPrm->IpAdr;
              p_IFO->VrtNgbIpAdr = FindVrtNgbIpAdr(p_RTB, p_Rte->p_DbEntry);
              p_IFO->LowLayerId =  ((t_IFO *)p_Rte->PathPrm->NextHop)->LowLayerId;
              p_Rte->VrtlIfo = p_IFO;

              if(p_IFO->VrtNgbIpAdr)
              {
                 /* Set cost of the virtual IFO to the RTB entry's cost */
                 memset(&ifo_metric,0,sizeof(t_S_IfMetric));
                 ifo_metric.IpAdr = p_IFO->Cfg.IpAdr;             
                 ifo_metric.MetricTOS = TOS_NORMAL_SERVICE;  
                 ifo_metric.Metric = (word)p_Rte->Cost; 
                 ifo_metric.MetricStatus = (p_IFO->Cfg.State == IFO_DOWN) ? ROW_CREATE_AND_GO : ROW_CHANGE;
                 IFO_MetricConfig(p_IFO, &ifo_metric);

                 /* RFC 2328: section 12.4, page 126                                                 
                 ** When the state of one of the router's configured virtual links changes, it may be
                 ** necessary to originate a new router-LSA into the virtual links transit area as   
                 ** well as originated a new router-LSA into the backbone.                           
                 */
                 if(p_IFO->Cfg.State != IFO_DOWN)
                   LsaReOriginate[S_ROUTER_LSA](p_IFO->TransitARO, 0);
                 else
                   IFO_Up(p_IFO);
              }
              return TRUE;
            }
        }
      }
    }
  }

  /* If we reached here either the area no longer exists or the route to the
  ** Virtual Neighbor does not exist, so bring the virtual link down
  */
  if(p_IFO->Cfg.State >= IFO_WAIT)
    IFO_Down(p_IFO);

  return TRUE;
}

/* see RFC 2328 section 16.4.1 */
static e_Err AsExtFindPreferableRoute(t_RTB *p_RTB, t_A_DbEntry *p_DbEntry,
                                      t_RoutingTableEntry **p_PrefRte,
                                      SP_IPADR forwardingAddr)
{
   t_S_AsExternalLsa   *p_AsExtLsa = NULLP;
   t_RoutingTableEntry *p_AsBr = NULLP, *p_FrwrdRte = NULLP;
   t_ARO               *p_FrwrdRteARO, *p_ARO = p_DbEntry->ARO_Id;
   Bool                pFlag;
   t_RTO               *p_RTO = (t_RTO *)p_RTB->RTO_Id;

   p_AsExtLsa = (t_S_AsExternalLsa *)p_DbEntry->p_Lsa;

   /* NSSA CHANGE */
   /* If the destination of a Type-7 default route (destination ID = DefaultDestination)
   ** and one of the following is true, then do nothing with this LSA and consider the 
   ** next in the list:
   **       1. The calculating router is a border router and the LSA has its P-bit clear
   **       2. The calculating router is a border router and is suppressing the import 
   **          of summary routes as Type-3 summary-LSAs.
   */
   pFlag = (p_DbEntry->Lsa.Options & OSPF_OPT_P_BIT) ? TRUE : FALSE;
   if((A_GET_4B(p_DbEntry->Lsa.LsId) == DefaultDestination) &&
      (p_DbEntry->Lsa.LsType == S_NSSA_LSA) && routerIsBorder(p_RTO) && 
      (!pFlag || !p_ARO->ImportSummaries))
     return E_FAILED;

   /* if no forwarding address, find path to ASBR */
   if(forwardingAddr == DefaultDestination)
   {
      p_AsBr = o2RouteFind(p_RTB, p_RTB->RtbRtBt, 
                           A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter), SPF_HOST_ROUTE);
      if (p_AsBr)
      {
         *p_PrefRte = p_AsBr;
         return E_OK;
      }
   }
   else
   {
     /* If the forwarding address is non-zero look up the
      ** forwarding address in the routing table.
      */
      p_FrwrdRte = o2LongestPrefixMatch(p_RTB, p_RTB->RtbNtBt, forwardingAddr);
      if (!p_FrwrdRte)
        return E_FAILED;

      /* For a Type-7 LSA the matching routing table entry must specify an 
      ** intra area path through the LSA's originating NSSA 
      */
      p_FrwrdRteARO = (t_ARO *)p_FrwrdRte->p_DbEntry->ARO_Id;
      if ((p_DbEntry->Lsa.LsType == S_NSSA_LSA) &&
         ((p_FrwrdRte->AreaId == ((t_ARO *)p_DbEntry->ARO_Id)->AreaId) ||
          (p_FrwrdRteARO->ExternalRoutingCapability == AREA_IMPORT_NSSA)))
      {
          *p_PrefRte = p_FrwrdRte;
          return E_OK;
      }
        
      /* For a Type-5 LSA, the matching routing table entry must specify an 
      ** intra-area or inter-area path through a Type-5 capable area.
      */
      if ((p_DbEntry->Lsa.LsType == S_AS_EXTERNAL_LSA) && 
          ((p_FrwrdRte->PathType == OSPF_INTRA_AREA) || 
           (p_FrwrdRte->PathType == OSPF_INTER_AREA)) &&
          (p_FrwrdRteARO->ExternalRoutingCapability == AREA_IMPORT_EXTERNAL))
      {
         *p_PrefRte = p_FrwrdRte;
         return E_OK;
      }
   }

   /* If we get here the above searching and checking failed */
   return E_FAILED;
}


/*********************************************************************
 * @purpose          Add the new path to RTE
 *
 *
 * @param Rte        @b{(input)}  RTB entry
 * @param p_IFO      @b{(input)}  NextHop interface handler
 * @param nextHop    @b{(input)}  next hop address
 * @param linkId     @b{(input)}  Link Id from source RTE to Current RTE
 *
 * @returns          n/a.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void PathAdd(t_RoutingTableEntry  *Rte, t_IFO *p_IFO, SP_IPADR nextHop, ulng linkId)
{
   t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
   t_OspfPathPrm *path, *DstPath, *nextPath, *prevPath;
#if L7_OSPF_TE
   t_ErPrm  *Er;
#endif

   /* Only add new path if route is not already at the
    * maximum number of paths. */
   if (Rte->PathNum >= p_RTO->Cfg.MaximumPaths)
   {
       return;
   }
   
   /* First, check if this path already present in dest route */
   for(DstPath = Rte->PathPrm; DstPath; DstPath = DstPath->next)
   {
       /* Next hop only considered the same if both next hop IP address 
        * and outgoing interface are the same. */
      if ((DstPath->IpAdr == nextHop) && (DstPath->NextHop == p_IFO))
         return;
   }

   /* Add new path */
   if((path = XX_Malloc(sizeof(t_OspfPathPrm))) == NULL)
   {
     if (!p_RTO->LsdbOverload)
       EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
     return;
   }
   memset(path, 0 , sizeof(t_OspfPathPrm));
   
   path->NextHop = p_IFO;
   path->IpAdr = nextHop;  
   
   /* Set he new Router Chain Entry equal to W itself */

#if L7_OSPF_TE
   /* Add ER parameters */
   if((Er = XX_Malloc(sizeof(t_ErPrm))) == NULL)
      return;
   
   A_SET_4B(Rte->DestinationId, Er->RouterId);
   A_SET_4B(linkId, Er->IfIndex);
   Er->next = NULL;
   path->ErList = Er;
#endif
   
   /* Insert next hop in sorted order */
   nextPath = Rte->PathPrm;
   prevPath = NULL;
   while (nextPath && (memcmp(&path->NextHop, &nextPath->NextHop, 8) > 0))
   {
     prevPath = nextPath;
     nextPath = nextPath->next;
   }
   path->next = nextPath;
   if (prevPath)
   {
     prevPath->next = path;
   }
   else
   {
     /* Only path. Set list head. */
   Rte->PathPrm = path;
   }
   
   ASSERT(Rte->PathPrm != Rte->PathPrm->next);
   Rte->PathNum ++;
#if L7_OSPF_TE
   Rte->ErNum ++;
#endif
}


/*********************************************************************
 * @purpose       Compare two RTEs paths
 *
 *
 * @param Rte1    @b{(input)}  1-st RTB entry
 * @param Rte2    @b{(input)}  2-nd RTB entry
 *
 * @returns       TRUE if both have equal path (NextHops).
 * @returns       FALSE otherwise.
 *
 * @notes         Assumes paths are sorted
 *
 * @end
 * ********************************************************************/
Bool PathCompare(t_RoutingTableEntry  *Rte1, t_RoutingTableEntry *Rte2)
{
   t_OspfPathPrm *Path1, *Path2;

   if (Rte1->PathNum != Rte2->PathNum)
      return FALSE;
   
   for (Path1 = Rte1->PathPrm, Path2 = Rte2->PathPrm; 
         Path1 && Path2; 
         Path1 = Path1->next, Path2 = Path2->next)
   {
      if ((Path1->NextHop != Path2->NextHop) || (Path1->IpAdr != Path2->IpAdr))
      {
         return FALSE;
      }
   }

   return TRUE;
}

/*********************************************************************
 * @purpose      Compare two RTEs paths
 *
 *
 * @param Rte1    @b{(input)}  1-st RTB entry
 * @param Rte2    @b{(input)}  2-nd RTB entry
 *
 *
 * @returns       TRUE if both have equal path (NextHops).
 * @returns       FALSE otherwise.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static Bool PathCompareAndValidate(t_RoutingTableEntry  *Rte1, t_RoutingTableEntry *Rte2)
{
   t_OspfPathPrm *Path1, *Path2;
   byte eqPaths = 0;

   for (Path1 = Rte1->PathPrm, Path2 = Rte2->PathPrm; 
         Path1 && Path2; 
         Path1 = Path1->next, Path2 = Path2->next)
      {
         if ((Path1->NextHop == Path2->NextHop) && (Path1->IpAdr == Path2->IpAdr))
         {
        eqPaths++;
            /* mark this intra-area path as valid */
            Path1->AdvertisingRouter = 0;
      }
   }

   if ((Rte1->PathNum != eqPaths) || (Rte2->PathNum != eqPaths))
      return FALSE;
   else
      return TRUE;
}

/*********************************************************************
 * @purpose   Add a route to the OSPF routing table.
 *
 * @param     p_RTB     @b{(input)}  routing table instance
 * @param     p_Rte     @b{(input)}  new route
 *
 * @returns       E_OK   if update successful.  
 *                E_FAILED otherwise.
 *
 * @notes     Checks if the routing table contains an existing route to the
 *            destination. If so, p_Rte replaces the existing route. If there
 *            is not an existing route, p_Rte is inserted.
 *
 * @end
 *
 *--------------------------------------------------------------------*/
static e_Err RteUpdate(t_RTB *p_RTB, t_RoutingTableEntry  *p_Rte)
{
   t_RoutingTableEntry  *p_Old;   /* existing route to same destination*/

   /* If a network route, update the networks routing table. If a router route, 
    * update the router routing table. */
   t_Handle bt = (p_Rte->DestinationType >= DEST_TYPE_IS_NETWORK) ? 
       p_RTB->RtbNtBt : p_RTB->RtbRtBt;

   /* Find existing route to same dest */
   p_Old = o2RouteFind(p_RTB, bt, p_Rte->DestinationId, p_Rte->IpMask);
   if (!p_Old)
      {
      if (o2RouteAdd(p_RTB, bt, p_Rte) != E_OK)
      {
         RteRelease(p_Rte, TRUE);
         return E_FAILED;
      }
   }
   else if (p_Rte != p_Old)
   {
     /* Routing table contains a route to same destination, but not the 
      * same route object. Since the radix key is the same for the old and
      * new route, slip the new route into the old route's place in the tree. */
      if (o2RadixChange(p_RTB, bt, p_Rte) != E_OK)    
      {
         RteRelease(p_Rte, TRUE);
         return E_FAILED;
      }
   }

   return E_OK;
}

/*********************************************************************
 * @purpose         etermines if the specified router is a border router
 *
 *
 * @param p_RTO     @b{(input)}  Router object handle
 *
 * @returns         TRUE  - The router is a border router
 * @returns         FALSE - The router is not a border router
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool routerIsBorder(t_RTO* p_RTO)
{
  e_Err err = E_FAILED;
  e_Err e;
  t_ARO *p_tmpARO = NULL;
  t_IFO *p_tmpIFO = NULL;
  ulng areaNum = 0;
  
  /* Determine if this is a border router.  A router may be classified as a border router
  ** if it has at least two active areas each of which has at least one active interface
  */
  err = HL_GetFirst(p_RTO->AroHl, (void **)&p_tmpARO);
  while(err == E_OK)
  {
    if(areaNum > 1)
     break;

    /* If this is an active area and it has at least one active interface then bump up the
    ** area counter
    */
    if (p_tmpARO->OperationState)
    {
        Bool upIfFound = FALSE;
        e = HL_GetFirst(p_tmpARO->IfoHl, (void **)&p_tmpIFO);
        while ((e == E_OK) && !upIfFound)
        {
            if ((p_tmpIFO->Cfg.State != IFO_DOWN) &&
                (p_tmpIFO->Cfg.Type != IFO_VRTL))
            {
                upIfFound = TRUE;
                areaNum++;
            }
            e = HL_GetNext(p_tmpARO->IfoHl, (void**)&p_tmpIFO, p_tmpIFO);
        }
    }

    err = HL_GetNext(p_RTO->AroHl, (void *)&p_tmpARO, p_tmpARO);
  }

  return ((areaNum > 1) ? TRUE : FALSE);
}

/*********************************************************************
* @purpose  Update RTO with the results with the results of the latest 
*           OSPF routing table calculation.
*
* @param    p_RTB    @b{(input)} routing table just computed
*
* @returns  E_OK  if success
*
* @notes    When this function is called p_RTB->RtbRtBt and p_RTB->RtbNtBt
*           contain the routes just computed and RtbRtBtOld and RtbNtBtOld
*           contain the routes from the previous SPF. We iterate through
*           all routes just computed and ask whether the old routing table
*           had a route to the same destination. If so and the routes are
*           the same, there is no need to report the route to RTO. If the
*           routes are different, report a route change to RTO. If there
*           is no old route to the destination, report a new route to RTO.
*           Matching routes are removed from the old routing table. After
*           all new routes have been examined, the routes left in the old
*           routing table are ones which should be deleted from RTO.
*
*           This process is asychronous and goes through 4 states. 
*           1. Compare old and new network RTBs
*           2. Flush routes from old network RTB
*           3. Compare old and new router RTBs
*           4. Flush routes from old router RTB
*
*           RtePendingUpdate indicates the next RTE to be processed. NULL
*           implies the first RTE. The implemenation functions set 
*           RtePendingUpdate and return E_BUSY if they are unable to complete
*           processing within OSPF_MAX_RTB_UPDATE_MSECS.
*
*           The SPF cannot run till the RTB udpate is done.
*
* @end
*********************************************************************/
static e_Err RTB_RtoUpdate(void *p_Info)
{
    e_Err e = E_OK;
    t_XXCallInfo *callInfo  = (t_XXCallInfo *) p_Info;
    t_RTB *p_RTB = (t_RTB *) PACKET_GET(callInfo,0);
    t_RTO *p_RTO = (t_RTO*) p_RTB->RTO_Id;
    ulng startTime = osapiTimeMillisecondsGet();

    /* enable bundling */
    p_RTO->BundleLsas = TRUE;

    switch(p_RTB->RtbUpdateState)
    {
    case RTB_UPD_START:
      p_RTB->RtbUpdateState   = RTB_UPD_NET_PEND;
      p_RTB->RtePendingUpdate = NULL;
      /* fall through */

    case RTB_UPD_NET_PEND:
      e = RTB_RtoUpdateRoutes(p_RTB, startTime, p_RTB->RtbNtBt, p_RTB->RtbNtBtOld);
      if (e == E_BUSY)
        break;

      p_RTB->RtbUpdateState   = RTB_UPD_NET_FLUSH;
      p_RTB->RtePendingUpdate = NULL;
      /* fall through */

    case RTB_UPD_NET_FLUSH:
      e = RTB_RtoFlushRoutes(p_RTB, startTime, p_RTB->RtbNtBtOld);
      if (e == E_BUSY)
        break;

      p_RTB->RtbUpdateState   = RTB_UPD_RTR_PEND;
      p_RTB->RtePendingUpdate = NULL;
      /* fall through */

    case RTB_UPD_RTR_PEND:
      e = RTB_RtoUpdateRoutes(p_RTB, startTime, p_RTB->RtbRtBt, p_RTB->RtbRtBtOld);
      if (e == E_BUSY)
        break;

      p_RTB->RtbUpdateState   = RTB_UPD_RTR_FLUSH;
      p_RTB->RtePendingUpdate = NULL;
      /* fall through */

    case RTB_UPD_RTR_FLUSH:
      e = RTB_RtoFlushRoutes(p_RTB, startTime, p_RTB->RtbRtBtOld);
      if (e == E_BUSY)
        break;

      p_RTB->RtbUpdateState   = RTB_UPD_DONE;
      p_RTB->RtePendingUpdate = NULL;
      /* fall through */

    default:
      break;
    }

    /* flush pending bundles */
    if (p_RTO->BundleLsas)
      LsUpdatesSend(p_RTO);

    /* not done yet? */
    if (e == E_BUSY)
    {
      /* Schedule an event, this gives OSPF a chance to service its queues */
      p_RTB->spfWaitForRto++;
      t_XXCallInfo *callData = 0;
      PACKET_INIT_MQUEUE(callData, RTB_RtoUpdate, 0, 0, OSPF_EVENT_QUEUE, 1, (ulng)p_RTB);
      ASSERT(callData);
      if (XX_Call(p_RTO->OspfRtbThread.threadHndle, callData) != E_OK)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
               "Failed to complete routing table update.");
        /* Won't finish RTO update. Forwarding table may be incomplete. Go into overload. */
        if (!p_RTO->LsdbOverload)
          EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
      }


      if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF)
      {
        char traceBuf[OSPF_MAX_TRACE_STR_LEN];
        sprintf(traceBuf, "OSPF RTB update pending in state %s",
          rtbUpdateStates[p_RTB->RtbUpdateState]);
        RTO_TraceWrite(traceBuf);
      }
    }
    else
    {
      /* we're done if we get here! */
      if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF)
      {
        RTO_TraceWrite("OSPF RTB update complete.");
      }

      p_RTB->RtbUpdateState   = RTB_UPD_DONE;
      p_RTB->RtePendingUpdate = NULL;

      /* Tell IP MAP after we finish the first RTO update after exiting GR. */
      if (!o2GracefulRestartInProgress(p_RTO) && !p_RTB->FirstRtoUpdateDone)
      {
        p_RTB->FirstRtoUpdateDone = TRUE;

        /* This stuff is logically part of o2GracefulRestartExit(). However, 
         * routes generated from the SPF (like T3 summaries) are not originated
         * until the RTO update. The RTO update is not run immediately after
         * the SPF completes. OSPF puts a message on its own msg queue to 
         * trigger the RTO update. */
        /* Flush all pre-restart LSAs we no longer want to originate */
        o2FlushAllPreRestartLsas(p_RTO);

        if (p_RTO->Clbk.f_InitialRtoUpdateDone)
        {
          (p_RTO->Clbk.f_InitialRtoUpdateDone)(p_RTO);
        }
      }
    }

    return E_OK;
}

/*********************************************************************
* @purpose  Implementation function for RTB_RtoUpdate().
*
* @param    p_RTB    @b{(input)} routing table just computed
* @param    newTree  @b{(input)} AVL tree containing routes just computed
* @param    oldTree  @b{(input)} AVL tree containing routes computed in 
*                                previous routing table calculation.
* @param    startTime @b{(input)} Time at which the update was started
*
* @returns  E_OK  if success
* @returns  E_BUSY  if it is in-progress but ran out of time
*
* @notes    
*           The update process run is limited to OSPF_MAX_RTB_UPDATE_MSECS
*           This allows OSPF to service its queues periodically. Before
*           this was introduced event processing would stop till all
*           changed routes had been originated/flushed. This turns out
*           to be a long time e.g. when scaling to 4000 routes originating
*           to one area took 4ms/LSA on Linux. Thats a total delay of 16secs.
*
*           If we are doing a graceful restart, no need to update RTO and no 
*           need to generate LSAs. 
*
* @end
*********************************************************************/
static e_Err RTB_RtoUpdateRoutes(t_RTB *p_RTB, ulng startTime, t_Handle newTree, t_Handle oldTree)
{
    e_Err e = E_OK;
    t_RTO *p_RTO = (t_RTO*) p_RTB->RTO_Id;
    t_RoutingTableEntry *p_Rte;
    t_RoutingTableEntry *p_RteOld;
    t_RoutingTableEntry *p_RteNext;
    t_RoutingTableKey rtbkey;

    if (o2GracefulRestartInProgress(p_RTO))
    {
      return E_OK;
    }

    if(p_RTB->RtePendingUpdate)
    {
      /* start where we left off */
      rtbkey.DstAdr = ((t_RoutingTableEntry *)p_RTB->RtePendingUpdate)->DestinationId;
      rtbkey.Prefix = ((t_RoutingTableEntry *)p_RTB->RtePendingUpdate)->IpMask;
      p_RteNext = o2RouteFind(p_RTB, newTree, rtbkey.DstAdr, rtbkey.Prefix);
    }
    else
    {
    /* Iterate through all routes in the new routing table. */
      e = o2RouteGetFirst(p_RTB, newTree, (void *)&p_RteNext);
    }
    while(e == E_OK)
    {
        /* Limit amount of time spent doing RTO updates so the OSPF thread 
         * can keep up with other important things (like sending hellos!). Also, 
         * route changes can prompt us to send LSAs (e.g., T3 summaries and translated
         * type 5s. If we reach the global limit for retx list entries, throttle the
         * update process. */
        if (((osapiTimeMillisecondsGet() - startTime) > OSPF_MAX_RTB_UPDATE_MSECS) ||
            (p_RTO->retxEntries >= RTO_MaxLsaRetxEntriesGet(p_RTO)))
        {
          p_RTB->RtePendingUpdate= p_RteNext;
          e = E_BUSY;
          break;
        }

        p_Rte = p_RteNext;
        /* May delete p_Rte below, so get next here. */
        e = o2RouteGetNext(p_RTB, newTree, &p_RteNext);

        /* Look for exact match for this route in the old routing table */
        p_RteOld = o2RouteFind(p_RTB, oldTree, p_Rte->DestinationId, p_Rte->IpMask);
        if (p_RteOld != NULL)
        {
            /* Remove the old route from the old routing table since we have
             * processed the new route to the same destination. */
            o2RouteDelete(p_RTB, oldTree, p_Rte->DestinationId, p_Rte->IpMask, FALSE);
            if (RTB_RoutesEqual(p_Rte, p_RteOld) == FALSE)
            {
                RteChanged(p_RTB, p_RteOld, p_Rte);
                RteRelease(p_RteOld, TRUE);
        }
        else
        {
                /* Routes are the same. Retain the old route object because 
                 * there may be references to it, like t_AgrEntry.LcRte. */
                o2RadixChange(p_RTB, newTree, p_RteOld);
            }           
        }
        else
        {
            /* This is a new route */
            RteAdded(p_RTB, p_Rte);
        }
    }

    /* if we're on a break, dont delete the old rtb just yet */
    return e;
}

/*********************************************************************
* @purpose  Implementation function for Flushing old routes in 
*           RTB_RtoUpdate().
*
* @param    p_RTB    @b{(input)} routing table just computed
* @param    newTree  @b{(input)} AVL tree containing routes just computed
* @param    oldTree  @b{(input)} AVL tree containing routes computed in 
*                                previous routing table calculation.
* @param    startTime @b{(input)} Time at which the update was started
*
* @returns  E_OK  if success
* @returns  E_BUSY  if it is in-progress but ran out of time
*
* @notes    
*           The flushing process run is limited to OSPF_MAX_RTB_UPDATE_MSECS
*           This allows OSPF to service its queues periodically. Before
*           this was introduced event processing would stop till all
*           changed routes had been originated/flushed. This turns out
*           to be a long time e.g. when scaling to 4000 routes originating
*           to one area took 4ms/LSA on Linux. Thats a total delay of 16secs.
*
* @end
*********************************************************************/
static e_Err RTB_RtoFlushRoutes(t_RTB *p_RTB, ulng startTime, t_Handle oldTree)
{
    e_Err e = E_FAILED;
    t_RoutingTableEntry *p_RteOld;
    t_RoutingTableEntry *p_RteToDelete;
    t_RoutingTableKey rtbkey;
    t_RTO *p_RTO = (t_RTO*) p_RTB->RTO_Id;

    /* We have processed all new routes. Any routes left in the 
     * old routing table should be deleted. */
    if(p_RTB->RtePendingUpdate)
    {
      /* start where we left off */
      rtbkey.DstAdr = ((t_RoutingTableEntry *)p_RTB->RtePendingUpdate)->DestinationId;
      rtbkey.Prefix = ((t_RoutingTableEntry *)p_RTB->RtePendingUpdate)->IpMask;
      p_RteOld = o2RouteFind(p_RTB, oldTree, rtbkey.DstAdr, rtbkey.Prefix);
      if (p_RteOld)
      {
        e = E_OK;
      }
    }
    else
    {
      /* Iterate through all routes in the old routing table. */
      e = o2RouteGetFirst(p_RTB, oldTree, &p_RteOld);
    }
    
    while(e == E_OK)
    {
      /* Limit amount of time spent doing RTO updates so the OSPF thread 
       * can keep up with other important things (like sending hellos!). Also, 
       * route changes can prompt us to send LSAs (e.g., T3 summaries and translated
       * type 5s. If we reach the global limit for retx list entries, throttle the
       * update process. */
        if (((osapiTimeMillisecondsGet() - startTime) > OSPF_MAX_RTB_UPDATE_MSECS) ||
            (p_RTO->retxEntries >= RTO_MaxLsaRetxEntriesGet(p_RTO)))
        {
          /* save our position */
          p_RTB->RtePendingUpdate = p_RteOld;
          e = E_BUSY;
          break;
        }

        p_RteToDelete = p_RteOld;
        e = o2RouteGetNext(p_RTB, oldTree, &p_RteOld);
        o2RouteDelete(p_RTB, oldTree, p_RteToDelete->DestinationId, p_RteToDelete->IpMask, FALSE);
        RteRemoved(p_RTB, p_RteToDelete);
    }

    return e;
}

/*********************************************************************
* @purpose  Compare two OSPF routes and determine if they are equal.
*
* @param    route1    @b{(input)} First route to compare
* @param    route2    @b{(input)} Second route to compare
*
* @returns  TRUE  if routes are equal
* @returns  FALSE if routes are different
*
* @notes    Two routes are considered equal if the following attributes are equal:
*             - destination address and mask
*             - destination type
*             - path type
*             - area ID
*             - next hops
*             - type 1 and type 2 cost
*
* @end
*********************************************************************/
static Bool RTB_RoutesEqual(t_RoutingTableEntry *route1, t_RoutingTableEntry *route2)
{
    if (route1->DestinationId != route2->DestinationId)
        return FALSE;
    if (route1->IpMask != route2->IpMask)
        return FALSE;
    if (route1->DestinationType != route2->DestinationType)
        return FALSE;
    if (route1->AreaId != route2->AreaId)
        return FALSE;
    if (route1->PathType != route2->PathType)
        return FALSE;
    if (route1->Cost != route2->Cost)
        return FALSE;
    if ((route1->PathType == OSPF_TYPE_2_EXT) || 
        (route1->PathType == OSPF_NSSA_TYPE_2_EXT))
    {
        if (route1->Type2Cost != route2->Type2Cost)
            return FALSE;
    }

    /* if the p-bit changes in an NSSA route, need to consider it a change. 
     * may affect translation of T7 routes to T5 routes. */
    if ((route1->PathType == OSPF_NSSA_TYPE_1_EXT) || 
        (route1->PathType == OSPF_NSSA_TYPE_2_EXT))
    {
      if (route1->pBitSet != route2->pBitSet)
        return FALSE;
    }

    return PathCompare(route1, route2);
}

/*********************************************************************
* @purpose  Given an existing routing table entry and a routing table
*           entry formed from a vertex just added to the shortest path tree,
*           determine whether the new routing table entry should replace
*           the existing one. 
*
* @param    p_RtEntry    @b{(input)} An existing intra-area route
* @param    p_V    @b{(input)} Route formed from an LSA just added to 
*                              the shortest path tree in the intra-area
*                              route calculation
*
* @returns  TRUE if the current routing table entry should be overwritten.
*
* @notes    This function is a helper function for the intra-area route 
*           calculation. See RFC 2328 16.1.(4).
*
* @end
*********************************************************************/
static Bool RTB_TransitVertexReplace(t_RoutingTableEntry *p_RtEntry, 
                                     t_RoutingTableEntry *p_V)
{
    /* This check should no longer be needed since we no longer keep
     * the previous SPF's routes in the routing table when we run a new
     * SPF, and thus the routing table should only include intra area routes
     * at this point. */
    if (p_RtEntry->PathType != OSPF_INTRA_AREA)
        return TRUE; 
    if (p_RtEntry->p_DbEntry == NULL)
        return TRUE;  /* no longer have an LSA for previous route */
    if (p_V->Cost > p_RtEntry->Cost) 
        return FALSE;   /* current route is shorter, so retain it */
            
    /* new route is just as short as the current route. Overwrite if 
     * current route from LSA with smaller Link State ID than new route. */
    if (A_GET_4B(p_RtEntry->p_DbEntry->Lsa.LsId) <
        A_GET_4B(p_V->p_DbEntry->Lsa.LsId))
        return TRUE;
    return FALSE;
}

/*********************************************************************
* @purpose  For intra-area route calculation, if necessary, modify routing 
*           table as a result of adding a router vertex to the shortest 
*           path tree.
*
* @param    p_RTB    @b{(input)}  Routing table 
* @param    p_ARO    @b{(input)}  Area whose intra-area routes are being
*                                 calculated
* @param    p_V    @b{(input)} Route formed from an LSA just added to 
*                              the shortest path tree in the intra-area
*                              route calculation
*
* @returns  E_OK if operation successful
*           E_NOMEMORY if unable to allocate a new route entry
*           E_FAILED if unable to add new route entry to AVL tree
*
* @notes    See RFC 2328 16.1.(4).  Only routes to ASBRs go into RtbRtBt.
*
* @end
*********************************************************************/
static e_Err RTB_IntraAreaRouterVertexAdded(t_RTB *p_RTB, t_ARO *p_ARO, 
                                            t_RoutingTableEntry *p_V)
{
    if(p_V->DestinationType & DEST_TYPE_IS_BORDER_ROUTER)
    {
        /* The total number of area border routers reachable
           within this area. */
        p_ARO->AreaBdrRtrCount ++;
 
    }
    if(p_V->DestinationType & DEST_TYPE_IS_ROUTER)  
    {
        /* The total number of area internal routers reachable
           within this area. */
        p_ARO->AreaIntRtrCount ++;
    }
    if(p_V->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER)
    {
        p_ARO->AsBdrRtrCount++;

        return RTB_AsbrUpdate(p_RTB, p_ARO, p_V);
    }
    return E_OK;


}

/*********************************************************************
* @purpose  For intra-area route calculation, if necessary, modify routing 
*           table as a result of adding a transit network vertex to the  
*           shortest path tree.
*
* @param    p_RTB    @b{(input)}  Routing table 
* @param    p_ARO    @b{(input)}  Area whose intra-area routes are being
*                                 calculated
* @param    p_V    @b{(input)} Route formed from an LSA just added to 
*                              the shortest path tree in the intra-area
*                              route calculation
*
* @returns  E_OK if operation successful
*           E_NOMEMORY if unable to allocate a new route entry
*           E_FAILED if unable to add new route entry to AVL tree
*
* @notes    See RFC 2328 16.1.(4).
*
* @end
*********************************************************************/
static e_Err RTB_IntraAreaNetworkVertexAdded(t_RTB *p_RTB, t_ARO *p_ARO, 
                                             t_RoutingTableEntry *p_V)
{
    t_RTO *p_RTO = (t_RTO*) p_RTB->RTO_Id;
    t_RoutingTableEntry *p_RtEntry;

    /* try to locate the routing table entry for this network */
    p_RtEntry = o2RouteFind(p_RTB, p_RTB->RtbNtBt, p_V->DestinationId, p_V->IpMask);
    if (p_RtEntry)
    {
        /* So we have two LSAs for the same transit network. This is a
         * transient condition when the DR is being established for a 
         * transit network. The current routing table entry should be 
         * overwritten if and only if the newly found path is shorter and
         * the current entry's link state origin is smaller than
         * the newly added vertex. Note that because this is a transient
         * condition, we don't go to the trouble to combine the next hops
         * in the case that the costs are equal. */
        if (RTB_TransitVertexReplace(p_RtEntry, p_V) == TRUE)
        {
            RTB_RouteReplace(p_RTB, p_RtEntry, p_V);
            p_RtEntry->PathType = OSPF_INTRA_AREA;
            p_RtEntry->DestinationId &= p_RtEntry->IpMask;
        }
    }
    else
    {
        /* no routing table entry for this network */
        /* Allocate the new Routing table entry */
        if ((p_RtEntry = XX_Malloc(sizeof(t_RoutingTableEntry))) == NULL)
        {
          if (!p_RTO->LsdbOverload)
            EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
          return E_NOMEMORY;
        }

        memset(p_RtEntry, 0, sizeof(t_RoutingTableEntry));
        RTB_RouteReplace(p_RTB, p_RtEntry, p_V);
        p_RtEntry->PathType = OSPF_INTRA_AREA;
        p_RtEntry->IpMask = p_V->IpMask;
        p_RtEntry->DestinationId &= p_RtEntry->IpMask;

        if (p_ARO->AreaId == OSPF_BACKBONE_AREA_ID)
            p_RtEntry->BackboneReachable = TRUE;

        /* Insert new route into OSPF routing table */
        if (RteUpdate(p_RTB, p_RtEntry) != E_OK)
            return E_FAILED;
    }
    return E_OK;
}

/*********************************************************************
* @purpose  Replace one route with another by copying the attributes 
*           of the other route into the first route.
*
* @param    p_RTB    @b{(input)}  Routing table containing p_RtEntry
* @param    p_RtEntry    @b{(input)}  route to be replaced
* @param    p_V    @b{(input)} route to be copied
*
* @returns  E_OK
*
* @notes    
*
* @end
*********************************************************************/
static e_Err RTB_RouteReplace(t_RTB *p_RTB, t_RoutingTableEntry *p_RtEntry, 
                              t_RoutingTableEntry *p_V)
{
    t_RTO *p_RTO = (t_RTO *)p_RTB->RTO_Id;

    /* delete old next hops; don't free p_RtEntry */
    RteRelease(p_RtEntry, FALSE); 
                      
    /* copy the AreaId, VpnId, DestinationType, DestinationId, 
       Cost, EqualPath,etc.. from added "V" vertex */   
    memcpy(p_RtEntry, p_V, sizeof(t_RoutingTableEntry));   
                      
    /* copy paths, since memcpy above is shallow. */
    p_RtEntry->PathNum = 0;   
    p_RtEntry->ErNum = 0;
    p_RtEntry->PathPrm = NULL;
    PathCopy(p_RTO, p_RtEntry, p_V, 0, 0);

    return E_OK;
}

/*********************************************************************
* @purpose  Computes the spf calculation delay
*
* @param    p_RTB    @b{(input)}  Routing table containing p_RtEntry
*
* @returns  
*
* @notes
*
* @end
*********************************************************************/
void RTB_ComputeCalcDelay(t_RTB *p_RTB, t_RTO *p_RTO)
{
  ulng elapsedTime, currentTime;    /* seconds since boot */

  currentTime = osapiUpTimeRaw();

  /* Calculate elapsed time in seconds since the previous spf calculation */
  elapsedTime = currentTime - p_RTB->tsSpf;

  /* Check if elapsed time since last spf calculation is less than
   * Hold time. If yes, we need to check for remaining hold time.
   * If the elapsed time is greater than hold time, set the CalcDelay
   * equal to spf delay.
   */
  if(elapsedTime < p_RTO->Cfg.Holdtime)
  {
    /* Check if remaining hold time is less than spf delay, if yes
     * set the CalcDelay equal to spf delay, else set CalcDelay equal
     * to the remaining hold time.
     */
    if (p_RTO->Cfg.Holdtime - elapsedTime < p_RTO->Cfg.SilenceInterval)
      p_RTB->CalcDelay = p_RTO->Cfg.SilenceInterval;
    else
      p_RTB->CalcDelay = p_RTO->Cfg.Holdtime - elapsedTime;
  }
  else
    p_RTB->CalcDelay = p_RTO->Cfg.SilenceInterval;
} 

/*********************************************************************
 * @purpose         Lookup intra-area route for supplied destination
 *                  in the indicated area's SpfHl.
 *
 * @param p_ARO     @b{(input)}  Pointer to Area Object
 * @param rtbkey    @b{(input)}  Address and mask for destination
 * @param p_Rt      @b{(output)} Route table entry if found, NULL if not
 *
 * @returns         E_OK - if originated successfull,
 * @returns         E_FAILED - not reachable
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err
RteIntraAreaFind(t_ARO *p_ARO, SP_IPADR rtrId, t_RoutingTableEntry **p_Rt)
{
   e_Err e;
   t_RoutingTableKey rtbkey;

   ASSERT(p_ARO != NULL);

   rtbkey.DstAdr = rtrId;
   rtbkey.Prefix = SPF_HOST_ROUTE;

   if((e = AVLH_Find(p_ARO->SpfHl, (byte*)&rtbkey, (void *)p_Rt, 0)) != E_OK)
      *p_Rt = NULL; 

   return e;
}

/*********************************************************************
* @purpose  Find intra-area best route to ABR or ASBR.
*
* @param    p_ARO       @b{(input)}  Area in which to search
* @param    rtrId       @b{(input)}  OSPF Router Id of ABR or ASBR
* @param    type        @b{(input)}  DEST_TYPE_IS_BORDER_ROUTER
*
* @param    p_Rte       @b{(output)}  Best route, if found, or NULL
*
* @returns  E_OK
*
* @notes    
*
* @end
*********************************************************************/
e_Err 
RteIntraAreaFindByType(t_ARO *p_ARO, SP_IPADR rtrId, e_OspfDestType type,
                       t_RoutingTableEntry **p_Rte)
{
   e_Err e;

   ASSERT(p_ARO != NULL);

   e = RteIntraAreaFind(p_ARO, rtrId, p_Rte);

   if ((e != E_OK) || !((*p_Rte)->DestinationType & type)) 
   {
      *p_Rte = NULL; 
      e = E_FAILED;
   }
   return e;

}

int 
pruneArea(t_OspfPathPrm *path, SP_IPADR key) 
{
  return (((t_ARO *)((t_IFO *)path->NextHop)->ARO_Id)->AreaId == key);
}

int 
pruneAdvRtr(t_OspfPathPrm *path, SP_IPADR key) 
{
  return (path->AdvertisingRouter == key);
}


/*********************************************************************
 * @purpose       Remove paths using specified area
 *
 * @param paths   @b{(input)}  set of paths in use
 * @param area    @b{(input)}  area to stop using
 *
 * @returns       TRUE if both have equal path (NextHops).
 * @returns       FALSE otherwise.
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err
PrunePaths(t_RTB *p_RTB, t_RoutingTableEntry *p_Rte, SP_IPADR key, 
           pruneMatch prune)
{
#if L7_OSPF_TE
   t_ErPrm  *Er, *TmpEr;
#endif
   t_OspfPathPrm *TmpPath, *currPath, *prevPath;
   e_Err e = E_OK;

   currPath = p_Rte->PathPrm; 
   prevPath = 0;

   while(currPath != NULL)
   {
      if(prune(currPath,key))
      {
#if L7_OSPF_TE
         Er = currPath->ErList;
         while(Er)
         {
            TmpEr = Er->next;
            XX_Free(Er);
            Er = TmpEr;
            p_Rte->ErNum--;
         }
#endif
   
         p_Rte->ErNum = 0;
      
         TmpPath = currPath->next;
         XX_Free(currPath);
         p_Rte->PathNum--;
   
         if(prevPath) 
         {
            prevPath->next = TmpPath;
         }
         else 
         {
            p_Rte->PathPrm = TmpPath;
         }
         currPath = TmpPath;
      
      } 
      else 
      {
         prevPath = currPath;
         currPath = currPath->next;
      }
   }
   
   /* if all paths were deleted */
   if(p_Rte->PathNum == 0)
   {
      /* Remove the entry from the Routing Table Binary Tree */
      RtbRteDelete(p_RTB, p_Rte);
      p_Rte->PathPrm = NULL;
    }
   return e;
}

/*********************************************************************
* @purpose  Wrapper around o2RouteDelete that determines which 
*           routing table contains the route, based on the destination type 
*
* @param    p_RTB    @b{(input)}  Routing table instance
* @param    p_rte    @b{(output)} route to be removed
*
* @notes    Does not free the route, just removes it from the routing table.
*
* @end
*--------------------------------------------------------------------*/
static e_Err
RtbRteDelete(t_RTB *p_RTB, t_RoutingTableEntry *p_Rte)
{
   e_Err e;

   if (p_Rte->DestinationType >= DEST_TYPE_IS_NETWORK)
   {
      e = o2RouteDelete(p_RTB, p_RTB->RtbNtBt, p_Rte->DestinationId, p_Rte->IpMask, FALSE);
   }
   else
   {
      e = o2RouteDelete(p_RTB, p_RTB->RtbRtBt, p_Rte->DestinationId, p_Rte->IpMask, FALSE);
   }
   return e;
}

/*********************************************************************
* @purpose          Updates the Routing table Entry given to 
*                   the external world
*
* @param p_Rte      @b{(input)}  Routing table Entry
* @param p_rtbEntry @b{(output)} external rtb Entry structure
*
* @notes
*
* @end
*--------------------------------------------------------------------*/
e_Err updateRteEntry(t_RoutingTableEntry *p_Rte,
                     L7_RtbEntryInfo_t *p_rtbEntry)
{
  L7_uint32 i = 0;

  t_OspfPathPrm *path = p_Rte->PathPrm;

  p_rtbEntry->destinationIp = (L7_uint32)p_Rte->DestinationId;
  p_rtbEntry->destinationIpMask = (L7_uint32)p_Rte->IpMask;
  p_rtbEntry->areaId = (L7_uint32)p_Rte->AreaId;
  p_rtbEntry->destinationType = (L7_ushort16)p_Rte->DestinationType;
  p_rtbEntry->pathType = (L7_ushort16)p_Rte->PathType;
  p_rtbEntry->cost = (L7_uint32)p_Rte->Cost;

  /* If no path to the next hop exists, return failure */
  p_rtbEntry->no_of_paths = 0;
  if(path != NULL)
  {
    /* Iterate over the paths in the RTB entry and copy
     *      * each path info to the caller. */
    for(path = p_Rte->PathPrm; (path && (i < platRtrRouteMaxEqualCostEntriesGet()));
        path = path->next, i++)
    {
      /* If the IFO handle exists corresponding to next hop */
      if(path->NextHop)
      {
        t_IFO *p_IFO = (t_IFO *)(path->NextHop);

        if(p_IFO->Cfg.Type == IFO_VRTL)
        {
          /* assigning the IP address of the next hop for reaching virtual neighbor */
          p_rtbEntry->path[i].nextHopIpAdr = (L7_uint32)(p_IFO->NextHopToVrtNbr);
          /* assigning physical interface index for virtual IFOs to the nextHopIfIndex */
          p_rtbEntry->path[i].nextHopIfIndex = (L7_uint32)(p_IFO->VirtIfIndex);
        }
        else
        {
          p_rtbEntry->path[i].nextHopIpAdr = (L7_uint32)(path->IpAdr);
          p_rtbEntry->path[i].nextHopIfIndex = (L7_uint32)(p_IFO->Cfg.IfIndex);
        }
      }
    }
    p_rtbEntry->no_of_paths = i;

    return E_OK;
  }

  return E_FAILED;
}

/*********************************************************************
* @purpose         Gives the ABR entry
*
*
* @param RTO_Id            @b{(input)}  RTO Object handle
* @param destinationIp     @b{(input)}  Router Id of the ABR
* @param p_rtbEntry         pointer to routing table entry
*
* @return              E_OK       success
* @return              E_FAILED   failed
*
* @notes
*
* @end
*--------------------------------------------------------------------*/
e_Err RTO_AbrEntryGet(t_Handle RTO_Id,
                      L7_uint32 destinationIp,
                      L7_RtbEntryInfo_t *p_rtbEntry)
{
  e_Err e;

  t_RoutingTableKey    rtbkey;
  t_RoutingTableEntry  *p_Rte = NULL;

  t_RTO *p_RTO = (t_RTO *)RTO_Id;
  t_ARO *p_ARO;

  rtbkey.DstAdr = destinationIp;
  rtbkey.Prefix = 0;

  for (e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
       e == E_OK;
       e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO))
  {
    if(AVLH_Find(p_ARO->SpfHl, (byte *)&rtbkey, (void **)&p_Rte,0) == E_OK)
    {
      if(p_Rte->DestinationType & DEST_TYPE_IS_BORDER_ROUTER)
      {
        return updateRteEntry(p_Rte, p_rtbEntry);
      }
    }
  }
  
  return E_FAILED;
}

/*********************************************************************
* @purpose         Gives the ASBR entry
*
*
* @param RTO_Id            @b{(input)}  RTO Object handle
* @param destinationIp     @b{(input)}  Router Id of the ASBR
* @param p_rtbEntry         pointer to routing table entry
*
* @return              E_OK       success
* @return              E_FAILED   failed
*
* @notes
*
* @end
*--------------------------------------------------------------------*/
e_Err RTO_AsbrEntryGet(t_Handle RTO_Id,
                       L7_uint32 destinationIp,
                       L7_RtbEntryInfo_t *p_rtbEntry)
{
  e_Err e;
  t_RoutingTableEntry  *p_Rte = NULL;
  t_RTO  *p_RTO = (t_RTO *)RTO_Id;
  t_RTB  *p_RTB = NULL;

  for (e = HL_GetFirst(p_RTO->RtbHl, (void *)&p_RTB);
       e == E_OK;
       e = HL_GetNext(p_RTO->RtbHl, (void *)&p_RTB, p_RTB))
  {
    p_Rte = o2RouteFind(p_RTB, p_RTB->RtbRtBt, destinationIp, SPF_HOST_ROUTE);
    if (p_Rte)
    {
      if(p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER)
      {
        return updateRteEntry(p_Rte, p_rtbEntry);
      }
    }
  }

  return E_FAILED;
}

/*********************************************************************
* @purpose         Gives the Next ABR entry
*
*
* @param RTO_Id            @b{(input)}  RTO Object handle
* @param destinationIp     @b{(input)}  Router Id of the ABR
* @param p_rtbEntry         pointer to routing table entry
*
* @return              E_OK       success
* @return              E_FAILED   failed
*
* @notes
*
* @end
*--------------------------------------------------------------------*/
e_Err RTO_AbrEntryNext(t_Handle RTO_Id,
                       L7_uint32 *destinationIp,
                       L7_RtbEntryInfo_t *p_rtbEntry)
{
  e_Err e;
  t_RoutingTableKey    rtbkey;
  t_RoutingTableEntry  *p_Rte = NULL;
  t_RTO *p_RTO = (t_RTO *)RTO_Id;
  t_ARO *p_ARO;
  Bool getFirst = FALSE, abrFound = FALSE;

  rtbkey.DstAdr = *destinationIp;
  rtbkey.Prefix = 0;
  if(rtbkey.DstAdr == 0)
  {
    /* This is request for the first ABR. Get the first area */
    if((e = HL_GetFirst(p_RTO->AroHl, (void **)&p_ARO)) == E_OK)
      getFirst = TRUE;
  }
  else
  {
    /* Get the area of the previous entry and the previous entry */
    if((e = HL_FindFirst(p_RTO->AroHl, (byte *)&p_rtbEntry->areaId,
                         (void **)&p_ARO)) == E_OK)
      e = AVLH_Find(p_ARO->SpfHl, (byte *)&rtbkey, (void **)&p_Rte, 0);
  }
  /* We shall fail if (a) No area exists
   * (b) the area of the previous entry doesn't exist
   * (c) the previous entry doesn't exist */
  if(e != E_OK)
    return E_FAILED;

  for (; e == E_OK; e = HL_GetNext(p_RTO->AroHl, (void **)&p_ARO, p_ARO))
  {
    if(abrFound == TRUE)
      break;
    if(getFirst == TRUE)
    {
      if(AVLH_GetFirst(p_ARO->SpfHl, (void **)&p_Rte) == E_OK)
      {
        if(p_Rte->DestinationType & DEST_TYPE_IS_BORDER_ROUTER)
        {
          abrFound = TRUE;
          break;
        }
      }
      else
      {
        /* no entries in the current SpfHl, Go to the next area */
        continue;
      }
    }
    while((e = AVLH_GetNext(p_ARO->SpfHl, (void **)&p_Rte, p_Rte)) == E_OK)
    {
      if(p_Rte->DestinationType & DEST_TYPE_IS_BORDER_ROUTER)
      {
        abrFound = TRUE;
        break;
      }
    }
    getFirst = L7_TRUE;
  }

  if(abrFound == TRUE)
  {
    *destinationIp = (L7_uint32)p_Rte->DestinationId;
    return updateRteEntry(p_Rte, p_rtbEntry);
  } 

  return E_FAILED;
}

/*********************************************************************
* @purpose         Gives the Next ASBR entry
*
*
* @param RTO_Id            @b{(input)}  RTO Object handle
* @param destinationIp     @b{(input)}  Router Id of the ASBR
* @param p_rtbEntry         pointer to routing table entry
*
* @return              E_OK       success
* @return              E_FAILED   failed
*
* @notes
*
* @end
*--------------------------------------------------------------------*/
e_Err RTO_AsbrEntryNext(t_Handle RTO_Id,
                        L7_uint32 *destinationIp,
                        L7_RtbEntryInfo_t *p_rtbEntry)
{
  e_Err e;

  Bool rtbEntryFound     = FALSE; 
  Bool rtbNextEntryFound = FALSE; 

  t_RoutingTableEntry  *p_Rte = NULL;

  t_RTO  *p_RTO = (t_RTO *)RTO_Id;
  t_RTB  *p_RTB = NULL;

  if (*destinationIp == 0)
  {
    /* This we assume, is the query done for the first Border Router
     * Entry, by the Management component */
    rtbEntryFound = TRUE;
  }
  for (e = HL_GetFirst(p_RTO->RtbHl, (void *)&p_RTB);
       e == E_OK;
       e = HL_GetNext(p_RTO->RtbHl, (void *)&p_RTB, p_RTB))
  {
    if(rtbEntryFound == TRUE)
    {
      /* If rtbEntry is already found, parse the entries in this
       * AVL tree, to find the next entry following the criteria
       * DestinationType == ASBR */

      /* If no entries are in this AVL tree, go to next rtb AVL tree */
      if (o2RouteGetFirst(p_RTB, p_RTB->RtbRtBt, &p_Rte) != E_OK)
        continue;

      if(p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER)
      {
        rtbNextEntryFound = TRUE;
        break;
      }
    }
    else
    {
      /* If rtbEntry is found, get the next entry in this
       * AVL tree, which follows the criteria
       * DestinationType == ASBR */
      p_Rte = o2RouteFind(p_RTB, p_RTB->RtbRtBt, *destinationIp, SPF_HOST_ROUTE);
      if (!p_Rte || (!(p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER)))
      {
          continue;
      }

      rtbEntryFound = TRUE;
    }

    /* Parse the next entries in this AVL tree, and check if they are ASBR */
    while ((o2RouteGetNext(p_RTB, p_RTB->RtbRtBt, &p_Rte)) == E_OK)
    {
      if(p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER)
      {
        rtbNextEntryFound = TRUE;
        break;
      }
    }
  }

  if(rtbNextEntryFound == TRUE)
  {
    *destinationIp = (L7_uint32)p_Rte->DestinationId;
    return updateRteEntry(p_Rte, p_rtbEntry);
  }

  return E_FAILED;
}

/*********************************************************************
* @purpose       Record a reason for an SPF being scheduled
*
*
* @param p_RTB   @b{(input)}  routing table object
* @param reason  @b{(input)}  reason why SPF has been scheduled
*
* @return   E_OK 
*
* @notes
*
* @end
*--------------------------------------------------------------------*/
e_Err RTB_AddSpfReason(t_RTB *p_RTB, e_SpfReason reason)
{
  p_RTB->SpfStats[p_RTB->SpfStatsIndex].SpfReason |= reason;
  return E_OK;
}

/*********************************************************************
* @purpose       Record the time and duration of an SPF
*
*
* @param p_RTB   @b{(input)}  routing table object
* @param reason  @b{(input)}  reason why SPF has been scheduled
*
* @return   E_OK 
*
* @notes
*
* @end
*--------------------------------------------------------------------*/
static e_Err RTB_SpfStatsUpdate(t_RTB *p_RTB, L7_uint32 spfStart)
{
  t_RTO *p_RTO = (t_RTO*) p_RTB->RTO_Id;
  L7_uint32 duration = osapiTimeMillisecondsGet() - spfStart;   /* msec */

  /* Record stats for SPF just completed */
   p_RTB->SpfStats[p_RTB->SpfStatsIndex].SpfTime = spfStart;
   p_RTB->SpfStats[p_RTB->SpfStatsIndex].SpfDuration = duration;

   /* print trace */
   if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF)
   {
     char traceBuf[OSPF_MAX_TRACE_STR_LEN];
     L7_uchar8 reason[L7_SPF_REASON_LEN];
     memset(reason, 0, sizeof(reason));
     OspfReasonToString(p_RTB->SpfStats[p_RTB->SpfStatsIndex].SpfReason, reason);
     sprintf(traceBuf, "OSPF SPF took %u ms (reason %s).",
             duration, reason);
     RTO_TraceWrite(traceBuf);
   }

   /* Zero stats for next SPF */
   p_RTB->SpfStatsIndex++;
   if (p_RTB->SpfStatsIndex == OSPF_SPF_STATS_NUM)
     p_RTB->SpfStatsIndex = 0;
   p_RTB->SpfStats[p_RTB->SpfStatsIndex].SpfTime = 0;
   p_RTB->SpfStats[p_RTB->SpfStatsIndex].SpfDuration = 0;
   p_RTB->SpfStats[p_RTB->SpfStatsIndex].SpfReason = 0;

   return E_OK;
}

/*********************************************************************
* @purpose       Convert an SPF reason bit mask to a readable string.
*
*
* @param reasonMask @b{(input)}  reason bit mask
* @param reasonStr  @b{(input)}  readable string buffer. 
*                                L7_SPF_REASON_LEN characters long.   
*
* @return   E_OK if successful
*
* @notes
*
* @end
*--------------------------------------------------------------------*/
static e_Err OspfReasonToString(L7_uint32 reasonMask, L7_uchar8 *reasonStr)
{
  L7_BOOL comma = L7_FALSE;

  if (reasonStr == NULL)
    return E_FAILED;

  if (reasonMask & R_ROUTER_LSA)
  {
    comma = L7_TRUE;
    sprintf(reasonStr, "R");
  }

  if (reasonMask & R_NETWORK_LSA)
  {
    if (comma)
      strcat(reasonStr, ", N");
    else
    {
      strcat(reasonStr, "N");
      comma = L7_TRUE;
    }
  }

  if (reasonMask & R_SUMMARY_LSA)
  {
    if (comma)
      strcat(reasonStr, ", SN");
    else
    {
      strcat(reasonStr, "SN");
      comma = L7_TRUE;
    }
  }

  if (reasonMask & R_ASBR_SUM_LSA)
  {
    if (comma)
      strcat(reasonStr, ", SA");
    else
    {
      strcat(reasonStr, "SA");
      comma = L7_TRUE;
    }
  }

  if (reasonMask & R_EXT_LSA)
  {
    if (comma)
      strcat(reasonStr, ", X");
    else
    {
      strcat(reasonStr, "X");
      comma = L7_TRUE;
    }
  }
  return E_OK;
}

/*********************************************************************
* @purpose       Report the time, duration, and reason for an SPF
*
*
* @param p_RTB     @b{(input)}  routing table object
* @param maxStats  @b{(input)}  report stats for the last maxStats SPFs
* @param spfStats  @b{(output)} caller allocates this buffer to hold
*                               maxStats structures 
*
* @return   E_OK 
*
* @notes
*
* @end
*--------------------------------------------------------------------*/
unsigned int RTB_SpfStatsReport(t_Handle *RTO_Id, unsigned int maxStats, 
                                L7_OspfSpfStats_t *spfStats)
{
  t_RTO *p_RTO = (t_RTO*) RTO_Id;
  t_RTB *p_RTB; 
  L7_int32 firstIndex;
  L7_uint32 i;          /* index to SPF stats circular buffer */
  L7_uint32 n = 0;      /* number of spfs reported */
  L7_uint32 numToReport = maxStats;

  /* Assume a single routing table per OSPF instance */
  if (HL_GetFirst(p_RTO->RtbHl, (void*) &p_RTB) != E_OK)
    return 0;

  /* Find starting point in circular buffer */
  firstIndex = p_RTB->SpfStatsIndex - maxStats;
  if (firstIndex < 0)
    firstIndex = OSPF_SPF_STATS_NUM + firstIndex;

  /* Find a non-zero entry */
  while (numToReport > 0 && p_RTB->SpfStats[firstIndex].SpfTime == 0)
  {
    firstIndex++;
    if (firstIndex == OSPF_SPF_STATS_NUM)
      firstIndex = 0;
    numToReport--;
  }

  i = firstIndex;
  for (n = 0; n < numToReport; n++)
  {
    /* convert milliseconds to seconds */
    spfStats[n].spfTime = p_RTB->SpfStats[i].SpfTime / 1000;
    spfStats[n].spfDuration = p_RTB->SpfStats[i].SpfDuration;
    OspfReasonToString(p_RTB->SpfStats[i].SpfReason, spfStats[n].reason);
    
    /* goto next SPF */
    i++;
    if (i == OSPF_SPF_STATS_NUM)
    {
      i = 0;
    }
  }
  return n;
}

/*********************************************************************
 * @purpose       See if preferred path to an ASBR has changed.
 *
 * @param p_RTB   @b{(input)}  Pointer to RTB
 * @param p_ARO   @b{(input)}  Pointer to the Area
 * @param p_New   @b{(input)}  new ASBR route
 *
 * @returns E_OK         routing table is updated as required
 *          E_NOMEMORY   unable to allocate memory for new route
 *          E_FAILED     some other failure
 *
 * @notes   If p_New is the best route, p_New is copied and the copy 
 *          is added to the routing table. The caller is always 
 *          responsible to deallocate memory for p_New.
 *
 * @end
 *********************************************************************/
static e_Err RTB_AsbrUpdate(t_RTB *p_RTB, t_ARO *p_ARO, t_RoutingTableEntry *p_New)
{
  t_RoutingTableEntry *p_RtEntry;
  t_RTO *p_RTO = (t_RTO *)p_RTB->RTO_Id;
  e_Err e;

  /* Try to locate the routing table entry for this route */
  /* Apply the preference rules of RFC 2328 16.4.1 */
  p_RtEntry = o2RouteFind(p_RTB, p_RTB->RtbRtBt, p_New->DestinationId, SPF_HOST_ROUTE);
  if (p_RtEntry)
  {
    Bool vIsPreferred;

    e = AsExtPreferredPathFind(p_RTO, p_New, p_RtEntry, &vIsPreferred, FALSE);
    if (e != E_OK)
      return e;

    if (vIsPreferred == TRUE)
    {
      /* Overwrite existing route with new route */
      RTB_RouteReplace(p_RTB, p_RtEntry, p_New);
    }
  }
  else   
  {
    /* no existing route to this ASBR. Allocate a new one. */
    if((p_RtEntry = XX_Malloc(sizeof(t_RoutingTableEntry))) == NULL)
    {
      if (!p_RTO->LsdbOverload)
        EnterLsdbOverload(p_RTO, OSPF_STUB_ROUTER_RESOURCE);
      return E_NOMEMORY;
    }

    memset(p_RtEntry, 0, sizeof(t_RoutingTableEntry));
    /* Copy route, including next hops */
    RTB_RouteReplace(p_RTB, p_RtEntry, p_New);

    /* Add route to routing table */
    if (RteUpdate(p_RTB, p_RtEntry) != E_OK)
    {
      return E_FAILED;
    }
  }
    
  return E_OK;
}

/*********************************************************************
 * @purpose                Calculate prefered path to an ASBR
 *
 * @param p_RTB            @b{(input)}  Pointer to RTO
 * @param p_New            @b{(input)}  Pointer to candidate route to ASBR or FA
 * @param p_Old            @b{(input)}  Pointer to existing route to ASBR or FA 
 *                                      (not external route!)
 * @param section_16_4_1_only @b{(input)} follow 16.4.1 section rules only
 *
 * @param newIsPreferred   @b{(output)} Pointer to Bool result 
 *
 * @returns E_OK     Routes aren't equal preference; newIsPreferred is valid
 *          E_FAILED Routes are equal preference; newIsPreferred isn't valid
 *
 * @notes
 *          The E_FAILED case is only hit when multiple ASBRs 
 *          in the same area are advertising the same prefix and we have
 *          to choose between N > 1 AS-External LSAs for ASBRs with them
 *          same Area ID.
 *
 *          The E_FAILED case is never hit when calculating the best
 *          path to a single ASBR, since you'll never have N > 1 best 
 *          routes to a single ASBR in the same area.
 *
 * @end
 *********************************************************************/
static e_Err AsExtPreferredPathFind(t_RTO *p_RTO, 
                                    t_RoutingTableEntry *p_New, 
                                    t_RoutingTableEntry *p_Old,
                                    Bool *newIsPreferred,
                                    Bool section_16_4_1_only)
{
  e_Err e = E_OK; 

  *newIsPreferred = FALSE;

  /* routes to ASBRs must be intra-area or inter-area */
  if (((p_New->PathType != OSPF_INTRA_AREA) &&
       (p_New->PathType != OSPF_INTER_AREA)) ||
      ((p_Old->PathType != OSPF_INTRA_AREA) &&
       (p_Old->PathType != OSPF_INTER_AREA)))
    return E_FAILED;

  /* avoid goto by using do/while(0) and break */
  do {
    if (!p_RTO->Cfg.RFC1583Compatibility)
    {
      Bool newIsIntraAreaNonBackbone, oldIsIntraAreaNonBackbone;

      newIsIntraAreaNonBackbone = p_New->PathType == OSPF_INTRA_AREA && 
        p_New->AreaId != OSPF_BACKBONE_AREA_ID;
      oldIsIntraAreaNonBackbone = p_Old->PathType == OSPF_INTRA_AREA && 
        p_Old->AreaId != OSPF_BACKBONE_AREA_ID;

      /* if unequal preference by path type */
      if (newIsIntraAreaNonBackbone != oldIsIntraAreaNonBackbone)
      {
        /* new route is intra-area, non-backbone, old isn't; replace */
        if (newIsIntraAreaNonBackbone && !oldIsIntraAreaNonBackbone)
        {
          *newIsPreferred = TRUE;
          break;
        } 
        /* new route isn't intra-area, non-backbone, old is; leave alone */
        else
        {
          break;
        }
      }
      /* else FALLTHROUGH to cost/area comparison for equal pref routes below */
    }

    if(section_16_4_1_only == TRUE)
    {
       if(*newIsPreferred != TRUE)
          e = E_FAILED;

       break;
    }

    /* Additional rules according to 16.4 (3) follow here */

    /* if 1583 enabled, or if 1583 disabled & paths are equal preference */
    if(p_New->Cost < p_Old->Cost)
    {
      *newIsPreferred = TRUE;
      break;
    }
    else if (p_New->Cost == p_Old->Cost)
    {
      if (p_New->AreaId > p_Old->AreaId)
      {
        *newIsPreferred = TRUE;
        break;
      }
      /* paths to ASBRs are of equal preference */
      else if (p_New->AreaId == p_Old->AreaId)
      {
        e = E_FAILED;
        break;
      }
    }
  } while (0);
  return e;
}

/*********************************************************************
 * @purpose    Convert a routing table entry path type to a string.
 *
 * @param      pathType  @b{(input)}  path type
 *
 * @returns    corresponding string
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_uchar8 *o2PathTypeString(e_OspfPathType pathType)
{
  switch (pathType)
  {
    case OSPF_INTRA_AREA: return "INTRA"; 
    case OSPF_INTER_AREA: return "INTER"; 
    case OSPF_TYPE_1_EXT: return "EXT T1"; 
    case OSPF_TYPE_2_EXT: return "EXT T2"; 
    case OSPF_NSSA_TYPE_1_EXT: return "NSSA T1"; 
    case OSPF_NSSA_TYPE_2_EXT: return "NSSA T2"; 
    case OSPF_PATH_TYPE_ILG: return "ILG"; 
    default: return "INV"; 
  }
}


