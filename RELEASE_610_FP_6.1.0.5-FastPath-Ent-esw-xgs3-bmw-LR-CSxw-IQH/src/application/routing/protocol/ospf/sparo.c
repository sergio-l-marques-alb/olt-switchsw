/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename   sparo.c
 *
 * @purpose    OSPF Area Object (ARO) module, supports Area Database maitanance
 *
 * @component  Routing OSPF Component
 *
 * @comments   ARO_Init
 * ARO_Delete
 * ARO_Config
 * ARO_StubMetric
 * ARO_AddressRange
 * ARO_HostConfig
 * ARO_LsaDatabase
 * LsaInstall
 * LsaRelease
 * FlushLsa
 * LsaReOriginate
 * LsaCompare
 * LsaAgedSendTrap
 * LsaFlooding
 * AsExtLsaFlooding
 * FletcherCheckSum
 * ARO_LocalAppJoinToGroup
 * ARO_LocalAppLeaveGroup
 * OpaqueGenerate
 * LsaParseDel
 *
 * InternalRoutines:
 *
 * LsaOrgRouter
 * LsaOrgNetwork
 * LsaOrgSummary
 * StubDefaultLsaOriginate
 * LsaOrgAsExternal
 * LsaOrgGroupMember
 * LsaOrgEmpty
 * LsaOrgOpaque
 * LsaOriginate
 * LSA_Alloc
 * LsaSendTrap
 * AgingGroupList
 * McstFrwdLookUp
 * ReOrigUnpack
 * LsaRelUnpack
 * FlushAsLsa
 * AsExtRqFree
 * RtrLsaPreParsing
 * NetLsaPreParsing
 * TeLinkLsaPreParsing
 * TeRtrLsaParseDel
 * TeLinkLsaParseDel
 * FindRteForSummaryLsa
 * HideAllSummaryOfRange
 * DeliverAllSummaryOfRange
 * FlushAggregateSummary
 * OrigAggregateSummary
 *
 * TimerExpirationRoutines
 *
 * LsaDelayedTimerExp
 *
 *
 * @create     12/17/1998
 *
 * @author     Dan Dovolsky
 * @end
 *
 * ********************************************************************/


#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: /home/cvs/PR1003/PR1003/FASTPATH/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/src/application/routing/protocol/ospf/sparo.c,v 1.1 2011/04/18 17:10:29 mruas Exp $";
#endif


/* --- standard include files --- */
#include "std.h"
#include "stdio.h"
#include "local.h"



/* --- external object interfaces --- */

/* --- specific include files --- */

#include "spobj.h"
#include "log.h"
#include "osapi.h"
#include "os_xxcmn.h"
#include "mempool.h"
#include "osapi_support.h"
#include "buffer.h"
#include "frame.h"
#include "ll.h"

#if L7_MIB_OSPF
#include "ospfmib.ext"
#endif

const L7_uint32 LSA_HEADER_LEN = sizeof(t_S_LsaHeader);

/* --- internal prototypes --- */
e_Err AsLsaDelayedTimerExp( t_Handle Id, t_Handle TimerId, word Flag );
e_Err LsaDelayedTimerExp( t_Handle Id, t_Handle TimerId, word Flag );
e_Err TranslatorStabilityTimerExp(t_Handle Id, t_Handle TimerId, word Flag);
e_Err StubDefaultLsaOriginate(t_ARO *p_ARO);
e_Err NssaDefaultLsaOriginate(t_ARO *p_ARO);
static ulng GetMaxLsaLength(t_ARO *p_ARO, e_S_LScodes lsaType);
static void *LSA_Alloc(t_ARO *p_ARO, e_S_LScodes lsaType);
static void LsaSendTrap(t_RTO *p_RTO, t_ARO *p_ARO, t_A_DbEntry *p_DbEntry);

static e_Err LsaOrgRouter(t_Handle Id, ulng prm);
static e_Err LsaOrgNetwork(t_Handle Id, ulng prm);
static e_Err LsaOrgSummary(t_Handle Id, ulng prm);
static e_Err LsaOrgAsExternal(t_Handle Id, ulng prm);
static Bool OrigT5ToNssaDest(t_RTO *p_RTO, t_S_AsExternalCfg *p_AsExt);
static e_Err LsaOrgNssa(t_Handle Id, ulng prm);
static Bool LsaContentDiff (void *p_LsaNew, t_A_DbEntry *p_LsaOld);
static e_Err AroDeleteOnRtbThread (void *p_Info);
static e_Err HideAllSummaryOfRange(t_ARO *p_ARO, t_AgrEntry *p_AggrEntry);
static e_Err HideAllExtSummaryOfRange(t_RTO *p_RTO, t_AgrEntry *p_AggrEntry);
static e_Err DeliverAllSummaryOfRange(t_ARO *p_ARO, t_AgrEntry *p_AggrEntry);
static e_Err DeliverAllExtSummaryOfRange(t_RTO *p_RTO, t_AgrEntry *p_AggrEntry);
static e_Err FlushAggregateSummary(t_ARO *p_ARO, t_AgrEntry *p_AggrEntry);
static e_Err FlushExtAggregateSummary(t_RTO *p_RTO, t_AgrEntry *p_AggrEntry);
static e_Err OrigAggregateSummary(t_ARO *p_ARO, t_AgrEntry *p_AggrEntry);
static e_Err OrigExtAggregateSummary(t_RTO *p_RTO, t_AgrEntry *p_AggrEntry);
static e_Err NssaTranslatorStateSet(t_ARO *p_ARO, e_NSSATranslatorState newState);
static e_Err NssaFlushTranslatedLsas(t_ARO *p_ARO);

static t_ParseEntry *ParseEntryFind(t_A_DbEntry *p_DbEntry, 
                                    t_A_DbEntry *nbrDbEntry,
                                    ulng ifoId, e_LinkTypes linkType);
static e_Err TwoWayConnectivityCheck(t_S_RouterLsa *p_Lsa, SP_IPADR lsid, 
                                     e_LinkTypes linkType);

e_Err LsaOrgSumBulk(t_Handle Id);

static Bool RteShouldBeSummarized (t_ARO *p_ARO,t_RoutingTableEntry  *p_Rte);
static Bool LsaSummaryCondense(t_ARO *p_ARO, t_RoutingTableEntry *p_Rte);
static e_Err GetLsIdForSummOrASExtLsa(t_RTO *p_RTO, t_ARO *p_ARO, e_S_LScodes LsType,
                           SP_IPADR DestAddr, SP_IPADR NetMask,
                           SP_IPADR *p_LsId);
static e_Err LsaOrgEmpty(t_Handle Id, ulng prm);
static e_Err LsaOrgOpaque(t_Handle Id, ulng prm);
static e_Err LsaOriginate(t_RTO *p_RTO, t_ARO *p_ARO, void *p_F, e_S_LScodes LsaType,
                          SP_IPADR LinkStateId, Bool ReOrig, Bool forceOrig, byte Option);

f_Originate LsaReOriginate[S_LAST_LSA_TYPE] = {
   NULL,
   LsaOrgRouter,
   LsaOrgNetwork,
   LsaOrgSummary,
   LsaOrgSummary,
   LsaOrgAsExternal,
   LsaOrgEmpty,
   LsaOrgNssa,
   LsaOrgEmpty,
   LsaOrgOpaque,
   LsaOrgOpaque,
   LsaOrgOpaque
};

extern t_RTO        *RTO_List;       /* RTO object list  */

static void RtrLsaPreParsing(t_ARO *p_ARO, t_A_DbEntry *p_DbEntry, t_A_DbEntry *nbrDbEntry);
static void NetLsaPreParsing(t_ARO *p_ARO, t_A_DbEntry *p_DbEntry, t_A_DbEntry *nbrDbEntry);

#if L7_OSPF_TE
static void TeLinkLsaPreParsing(t_ARO *p_ARO, t_TeRtr *rtr, t_TeLink *p_Link,
                                t_A_DbEntry *nextDb);
static void TeRtrLsaPreParsing(t_ARO *p_ARO, t_TeRtr *p_TeRtr);
static void TeRtrLsaParseDel(t_ARO *p_ARO, t_A_DbEntry *p_DbEntry);
static void TeLinkLsaParseDel(t_ARO *p_ARO, t_TeLink *p_Link);
static e_Err GetTeLinkMetricAndType (t_A_DbEntry *p_DbEntry,
                                     e_LinkTypes *LkType, ulng *TeMetric);
#endif /* L7_OSPF_TE */

/* NSSA functions */
static e_Err NssaLsaShouldTranslate(t_ARO *p_ARO, t_A_DbEntry *p_DbEntry, 
                                    t_AgrEntry **p_AdrRange);
static Bool CheckNssaLsaOrg(t_A_DbEntry *p_DbEntry, t_ARO *p_ARO);
static Bool NssaLsaExactAndOnlyMatch(t_AgrEntry *p_AdrRange, 
                                     t_RoutingTableEntry *p_RteEntry);
static Bool NssaTranslate(byte *rte, ulng RtbId);
static void NssaTranslatorEnable(t_ARO *p_ARO);
extern Bool UpdateNssaAggregationInfo (t_RTB *p_RTB, t_RoutingTableEntry *p_Rte,
                                       e_RtChange chType);
extern void UpdateAggregationInfo(t_RTB *p_RTB, t_AgrEntry *p_AdrRange,
                                  t_RoutingTableEntry *p_Rte, e_RtChange chType);
extern e_Err NssaRangeLcRouteFind(t_AgrEntry *p_AdrRange);
extern void ClearLsaList(t_Handle avlh);
extern void RTB_ClearCandidateHl(t_ARO *p_ARO);
extern void RTB_ClearSpt(t_ARO *p_ARO);
extern e_Err RTB_LsaProcess(t_RTO *p_RTO, t_ARO *p_ARO, t_A_DbEntry *p_DbEntry);
extern void ScheduleSPF(t_RTB *p_RTB);
extern e_Err RteIntraAreaFindByType(t_ARO *p_ARO, SP_IPADR rtrId, e_OspfDestType type,
                                    t_RoutingTableEntry **p_Rte);

/*********************************************************************
 * @purpose   Debug utility to print the current number of LSAs.
 *
 * @param  void
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void ospfDebugLsaCount(void)
{
  t_RTO *p_RTO = (t_RTO*) RTO_List;
  unsigned char buf[255];
  sprintf(buf, "Number of LSAs in LSDB:  %ld (Max = %d)\n", 
          p_RTO->ActiveLsas, OSPF_MAX_LSAS);
  RTO_TraceWrite(buf);
}

/*********************************************************************
 * @purpose   Debug utility to print out a DD.
 *
 * @param  str        string describing context
 * @param  p_DbEntry  DB entry
 *
 * @returns  void
 *
 * @notes  Can be excuted from the command line using (0,dbptr)
 *
 * @end
 * ********************************************************************/
void ospfDebugDdShow(char * str, t_A_DbEntry *p_DbEntry)
{
  char traceBuf[OSPF_MAX_TRACE_STR_LEN];
  L7_uchar8 lsIdStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 advRouterStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_char8 strDefault[] = "Debug";
  t_ARO * p_ARO = (t_ARO*)p_DbEntry->ARO_Id;

  /* set string to default if not specified */
  str = (str) ? str : strDefault;

  osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.LsId), lsIdStr);
  osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter), advRouterStr);
  if (p_ARO)
  {
    osapiInetNtoa(p_ARO->AreaId, areaIdStr);
  }
  else
  {
    strcpy(areaIdStr, "0");
  }
  sprintf(traceBuf, "\r\n%s DB %s LSID: %s, Adv Router: %s, Area %s, InitAge: %d, OrgTime %u, SeqNo: 0x%lx, Opt 0x%x, p_Lsa 0x%x, isCur %s",
          str, lsaTypeNames[p_DbEntry->Lsa.LsType], lsIdStr, advRouterStr, areaIdStr,
          p_DbEntry->InitAge, (unsigned int)p_DbEntry->OrgTime, A_GET_4B(p_DbEntry->Lsa.SeqNum), p_DbEntry->Lsa.Options,
          (unsigned int)p_DbEntry->p_Lsa, (L7_BIT_ISSET(p_DbEntry->dbFlags, IS_CUR) ? "TRUE" : "FALSE") );
  sysapiPrintf(traceBuf);    
}


/*********************************************************************
 * @purpose   Debug utility to print out the aging table.
 *
 * @param  void
 *
 * @returns  void
 *
 * @notes     Dumps the entire table... output can be a quite long!
 *
 * @end
 * ********************************************************************/
void ospfDebugAgingTableShow(L7_uint32 top)
{
  char traceBuf[OSPF_MAX_TRACE_STR_LEN];
  t_A_DbEntry *p_DbEntry = 0;
  t_RTO *p_RTO = RTO_List;
  int i = 0, cnt = 0;
  e_Err e;

  e = AgingTbl_FindNext(p_RTO->AgingObj, i, 0, (void**)&p_DbEntry);
  while(e != E_FAILED)
  {
    if(e == E_OK)
    {
      if(p_DbEntry->AgeIndex != i)
        sprintf(traceBuf, "AgTbl[%d] MISMATCH AgeIndex[%d] ", i, p_DbEntry->AgeIndex);
      else
        sprintf(traceBuf, "AgTbl[%d] ", i);

      ospfDebugDdShow(traceBuf,p_DbEntry);
      e = AgingTbl_FindNext(p_RTO->AgingObj, i, p_DbEntry, (void**)&p_DbEntry);
    }
    else
    {
      /* find first item at next index */
      ++i;
      e = AgingTbl_FindNext(p_RTO->AgingObj, i, 0, (void**)&p_DbEntry);
    }

    cnt++;
    if(top && cnt > top)
      break;
  }
}

/*********************************************************************
 * @purpose           Initialize an ARO object.
 *
 *
 * @param IFO_Id      @b{(input)}  Originator IFO Id
 * @param p_Object    @b{(output)}  the object ID is returned here
 *
 * @returns   E_OK           success
 * @returns   E_FAILED       couldn't create an object
 * @returns   E_NOMEMORY     not enough memory for resources allocating
 *
 * @notes  Note that the interface is not added to the area's list of 
 *         interfaces here. That's done by the caller. So this function
 *         cannot do anything that requires the area to have an interface.
 *         For example, if this is the second area, the router is still not
 *         considered an ABR here, because routerIsBorder() requires the
 *         areas to have at least one interface.
 *
 *         We can't add the interface to the area here because ARO_AddressRange()
 *         calls this function with a phony IFO object. 
 *
 * @end
 * ********************************************************************/
e_Err ARO_Init(t_Handle IFO_Id, t_Handle *p_Object)
{
   t_IFO   *p_IFO = (t_IFO *) IFO_Id;
   t_ARO   *p_ARO, *p_firstAro;
   t_RTO   *p_RTO = (t_RTO*) p_IFO->RTO_Id;
   e_Err   e = E_OK;

   /* try allocate from user memory */
   p_ARO = (t_ARO *)XX_Malloc( sizeof(t_ARO) );
   if(p_ARO)
      memset( p_ARO, 0, sizeof(t_ARO) );
   else
      return E_NOMEMORY;

   /* initialize the object */

   p_ARO->Taken = TRUE;

   /* Copy Configuration */

   p_ARO->RTO_Id = p_IFO->RTO_Id;
   p_ARO->OspfSysLabel = p_RTO->OspfSysLabel;
   p_ARO->AreaId = p_IFO->Cfg.AreaId;
   p_ARO->ExternalRoutingCapability = DEF_EXTERNAL_ROUTING_CAPABILITY;
   p_ARO->ImportSummaries = TRUE;

   /* allocate Address range Hash List */
   e |= HL_Init(KEY_AT_START, 2*sizeof(SP_IPADR), offsetof(t_AgrEntry, NetIpAdr),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_ARO->AdrRangeHl);

   /* allocate StubDefaultCost metrics Hash List */
   e |= HL_Init(0, sizeof(e_TosTypes), offsetof(t_StubMetricEntry, MetricTOS),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_ARO->StubDefaultCostHl);

   /* allocate The Shortest-path tree AVL List */
   e |= AVLH_Init(AVL_KEY_ULNG, sizeof(t_RoutingTableKey),
               /* key offset in t_RoutingTableEntry */
               offsetof(t_RoutingTableEntry,DestinationId),
               NULL, NULL, &p_ARO->SpfHl);

   /* allocate The SPF candidate AVL List */
   e |= AVLH_Init(AVL_KEY_ULNG, sizeof(t_RoutingTableKey),
               /* key offset in t_RoutingTableEntry */
               offsetof(t_RoutingTableEntry,DestinationId),
               NULL, NULL, &p_ARO->CandidateHl);

   /* allocate Associated IFO objects Hash List */
   /* Use IfoId and areaID as hash key so that virtual interfaces, which 
    * belong to the backbone area, have a unique key. */
   e |= HL_Init(OWN_LINK, 2*sizeof(ulng),
               /* key offset of IfoId in IFO */
               offsetof(t_IFO, IfoId),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_ARO->IfoHl);

   /* allocate hosts hash list attached to this area */
   e |= HL_Init(0, sizeof(SP_IPADR), offsetof(t_S_HostRouteCfg, HostIpAdr),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_ARO->HostHl);

   /* Allocate Area's LSA Database */

   /* allocate Router LSAs DB AVLH List */
   e |= AVLH_Init(0, sizeof(t_A_DbKey),
               /* key offset in t_A_DbEntry */
                offsetof(t_A_DbEntry,Lsa) + offsetof(t_S_LsaHeader, LsId),
                NULL, NULL, &p_ARO->RouterLsaHl);

   /* allocate Network LSAs DB AVLH List */
   e |= AVLH_Init(0, sizeof(t_A_DbKey),
               /* key offset in t_A_DbEntry */
               offsetof(t_A_DbEntry,Lsa) + offsetof(t_S_LsaHeader, LsId),
               NULL, NULL, &p_ARO->NetworkLsaHl);

   /* allocate Network Summary LSAs DB AVLH List */
   e |= AVLH_Init(0, sizeof(t_A_DbKey),
               /* key offset in t_A_DbEntry */
               offsetof(t_A_DbEntry,Lsa) + offsetof(t_S_LsaHeader, LsId),
               NULL, NULL, &p_ARO->NetSummaryLsaHl);

   /* allocate AS boundary routers Summary LSAs DB AVLH List */
   e |= AVLH_Init(0, sizeof(t_A_DbKey),
               /* key offset in t_A_DbEntry */
               offsetof(t_A_DbEntry,Lsa) + offsetof(t_S_LsaHeader, LsId),
               NULL, NULL, &p_ARO->AsBoundSummaryLsaHl);

   /* allocate Group Membership LSAs DB AVLH List */
   e |= AVLH_Init(0, sizeof(t_A_DbKey),
               /* key offset in t_A_DbEntry */
               offsetof(t_A_DbEntry,Lsa) + offsetof(t_S_LsaHeader, LsId),
               NULL, NULL, &p_ARO->GroupMembLsaHl);

   /* NSSA Change */
   /* allocate NSSA LSAs DB AVLH List */
   e |= AVLH_Init(0, sizeof(t_A_DbKey),
               /* key offset in t_A_DbEntry */
               offsetof(t_A_DbEntry,Lsa) + offsetof(t_S_LsaHeader, LsId),
               NULL, NULL, &p_ARO->NssaLsaHl);

   /* allocate Link Opaque LSAs DB AVLH List */
   e |= AVLH_Init(0, sizeof(t_A_DbKey),
               /* key offset in t_A_DbEntry */
               offsetof(t_A_DbEntry,Lsa) + offsetof(t_S_LsaHeader, LsId),
               NULL, NULL, &p_ARO->LinkOpaqueLsaHl);

   /* allocate Area Opaque LSAs DB AVLH List */
   e |= AVLH_Init(0, sizeof(t_A_DbKey),
               /* key offset in t_A_DbEntry */
               offsetof(t_A_DbEntry,Lsa) + offsetof(t_S_LsaHeader, LsId),
               NULL, NULL, &p_ARO->AreaOpaqueLsaHl);
#if L7_OSPF_TE
   /* allocate Area Opaque LSAs DB AVLH List for TE Router Opaque LSA with key as Router ID */
   e |= AVLH_Init(AVL_FREE_ON_DEL, 4,
               /* key offset in t_TeLink */
               offsetof(t_TeRtr,RouterId),
               NULL, NULL, &p_ARO->TeRtrHl);
   /* allocate Area Opaque LSAs DB AVLH List for TE Links with key as Router ID+Link ID  */
   e |= AVLH_Init(AVL_FREE_ON_DEL, 8,
               /* key offset in t_TeLink */
               offsetof(t_TeLink,RouterId),
               NULL, NULL, &p_ARO->TeLsaHl);
#endif

   /* allocate delayed LSAs list */
   e |= HL_Init(FREE_ON_DEL, sizeof(t_DlyLsaKey),
               /* key offset in t_A_DbEntry */
               offsetof(t_DelayedLsa,LsaType),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_ARO->DelayedLsa);


   /* delayed LSA timer init */
   e |= TIMER_InitSec( 1, p_ARO, &p_ARO->DlyLsaTimer );
   /* NSSA translator stability interval timer init */
   e |= TIMER_InitSec( 1, p_ARO, &p_ARO->TranslatorStabilityTimer);

   /* Get first ARO */
   p_firstAro = NULL;
   HL_GetFirst(((t_RTO*)p_ARO->RTO_Id)->AroHl, (void**)&p_firstAro);
   /* Insert this ARO entry into RTO's areas hash list */
   e |= HL_Insert(((t_RTO*)p_ARO->RTO_Id)->AroHl, (byte *) &p_ARO->AreaId, p_ARO);

   if(e == E_OK)
   {
      /* advance the AreasNum counter */
      ((t_RTO*)p_ARO->RTO_Id)->AreasNum += 1;

      p_ARO->OperationState = TRUE;

#if L7_MIB_OSPF
      /* Area object has been created and activated successfully; */
      /* create and activate row in the OSPF MIB Area table.      */
      ospfAreaTable_InternalNewActive( ((t_RTO*)p_ARO->RTO_Id)->ospfMibHandle,
                                       p_ARO->AreaId, p_ARO);
#endif

      /* pass object ID back to caller */
      *p_Object = (t_Handle) p_ARO;

      if(p_RTO->Clbk.f_AreaCreateInd)
         p_RTO->Clbk.f_AreaCreateInd(
         p_RTO->MngId, p_ARO, &p_ARO->MngId);


      LsaReOriginate[S_ROUTER_LSA](p_ARO, 0);

      if((p_RTO->AreasNum == 2) && p_firstAro)
      {
        /* If the first area was an NSSA, and we are now
        ** a border router, re-run translator election 
        */
        NssaTranslatorElect(p_firstAro, 0);
      }

      /* If this is a border router and is attached to one or more
      ** NSSA Areas we may need to re-elect the translator
      */
      HL_Browse(p_RTO->AroHl, NssaTranslatorElect, 0);
   }
   else
      ARO_Delete((t_Handle) p_ARO, 1);

   return e;
}




/*********************************************************************
 * @purpose          Delete a ARO object.
 *
 *
 * @param  Id        @b{(input)}  ARO object Id
 * @param  flag      @b{(input)}  delete this entry from RTO AroHl
 *
 * @returns          TRUE if this entry remains in RTO AroHl,
 * @returns          FALSE otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool ARO_Delete(t_Handle ARO_Id, ulng flag)
{
   t_ARO   *p_ARO = (t_ARO *)ARO_Id;
   t_ARO   *p_firstAro, *p_tmpARO = NULLP;
   t_RTO   *p_RTO = NULL;
   e_Err   er;
   t_RTB   *p_RTB = NULLP;
   e_Err   e = E_FAILED, err = E_FAILED;
   t_AgrEntry  *p_Entry;
   t_A_DbEntry *p_DbEntry;
   t_RoutingTableKey rtbKey;
   t_RoutingTableEntry *p_RtEntry;
   t_VpnCos VpnCos = 0;
   SP_IPADR backbone = OSPF_BACKBONE_AREA_ID;
   t_ARO *p_BB;   /* backbone area object */
   t_IFO *p_IFO;

   if(!p_ARO)
      return E_FAILED;
   p_RTO = (t_RTO *) p_ARO->RTO_Id;

   p_ARO->OperationState = FALSE;

   /* Delete this ARO entry from RTO's areas hash list */
   /* If flag is TRUE, ARO_Delete is called from IFO_Delete procedure. */
   /* In order to prevent curruption in routing table calculation code */
   /* ARO_Delete is to be performed on the rtb thread                  */
   if(flag)
   {
      if(!XX_IsThreadCurrent(p_RTO->OspfRtbThread.threadHndle))
      {
         t_XXCallInfo *xxci;
         PACKET_INIT(xxci, AroDeleteOnRtbThread, 0, 0, 2, (ulng)ARO_Id);
         PACKET_PUT (xxci, flag)
         er = XX_Call(p_RTO->OspfRtbThread.threadHndle, xxci);
         ASSERT(er==E_OK);
         return ((Bool)flag);
      }
      HL_Delete(((t_RTO*)p_ARO->RTO_Id)->AroHl, (byte *) &p_ARO->AreaId, p_ARO);
   }

   /* See if any virtual links reference this area as its transit area. */
   if (HL_FindFirst(p_RTO->AroHl, (byte *) &backbone, (void *)&p_BB) == E_OK)
   {
     e = HL_GetFirst(p_BB->IfoHl, (void *)&p_IFO);
     while (e == E_OK)
     {
       if ((p_IFO->Cfg.Type == IFO_VRTL) &&
           (p_IFO->TransitARO == p_ARO))
       {
         p_IFO->TransitARO = NULL;
       } 
       e = HL_GetNext(p_ARO->IfoHl, (void*)&p_IFO, p_IFO);
     }
   }

   er = HL_GetFirst(p_ARO->AdrRangeHl, (void *)&p_Entry);
   while(er == E_OK)
   {
     p_Entry->alive = FALSE;
     p_Entry->LargestCost = p_Entry->AggregateRte.Cost = -1;
      
     /* Flush aggregate summary LSAs as we are in the process of deleting the address range */
     err = HL_GetFirst(p_RTO->AroHl, (void *)&p_tmpARO);   
     while(err == E_OK)
     {
       if(FindSummaryOrASExtLsa(p_RTO, p_tmpARO, S_IPNET_SUMMARY_LSA,
                                p_Entry->NetIpAdr, p_Entry->SubnetMask,
                                &p_DbEntry, NULLP) == E_OK)
         FlushLsa((t_Handle)p_DbEntry, (ulng)p_tmpARO, 0);

       err = HL_GetNext(p_RTO->AroHl, (void *)&p_tmpARO, p_tmpARO);
     }

     if(HL_FindFirst(p_RTO->RtbHl, (byte *)&VpnCos, (void *)&p_RTB) == E_OK)
     {
        /* Remove reject routes of active address ranges from the rto */
        rtbKey.DstAdr = p_Entry->NetIpAdr;
        rtbKey.Prefix = p_Entry->SubnetMask;
        if (rtbKey.Prefix)
           rtbKey.DstAdr &= rtbKey.Prefix;

        if (RTB_Find(p_RTB, (t_RoutingTableKey *)&rtbKey, &p_RtEntry, 0) == E_OK)
        {
           if(p_RTO->Clbk.p_RoutingTableChanged && (p_RtEntry->isRejectRoute == TRUE))
           {
              p_RTO->Clbk.p_RoutingTableChanged(p_RTO->Clbk.f_RtmAsyncEvent[0] ? 
                          (t_Handle)p_RTO->Clbk.f_RtmAsyncEvent : p_RTO->MngId, 
                          p_RtEntry->DestinationType, RTE_REMOVED, 
                          p_RtEntry->DestinationId, p_RtEntry->IpMask, 
                          p_RtEntry->PathNum, p_RtEntry->Cost, p_RtEntry->PathPrm,
                          p_RtEntry->PathType, p_RtEntry->isRejectRoute);
              AVL_Delete(p_RTB->RtbNtBt, (byte *)&p_RtEntry->DestinationId, NULL);
              RteRelease((byte*)p_RtEntry, 0);
           }
        }
     }

     HL_Destroy(&p_Entry->AgrRteHl);
     XX_Free(p_Entry);
     er = HL_GetNext(p_ARO->AdrRangeHl, (void *)&p_Entry, p_Entry);
   }

   HL_Cleanup(p_ARO->HostHl,1);

   HL_Cleanup(p_ARO->StubDefaultCostHl,1);

   /* Release Area's LSA Database */

   ClearLsaList(p_ARO->RouterLsaHl);
   ClearLsaList(p_ARO->NetworkLsaHl);
   ClearLsaList(p_ARO->NetSummaryLsaHl);
   ClearLsaList(p_ARO->AsBoundSummaryLsaHl);
   ClearLsaList(p_ARO->GroupMembLsaHl);
   ClearLsaList(p_ARO->NssaLsaHl);
   ClearLsaList(p_ARO->LinkOpaqueLsaHl);
   ClearLsaList(p_ARO->AreaOpaqueLsaHl);

   /* delete LSA timer init */
   TIMER_Delete(p_ARO->DlyLsaTimer);
   /* delete NSSA Translator Stability Interval timer */
   TIMER_Delete(p_ARO->TranslatorStabilityTimer);
   /* Release delayed LSAs list */
   HL_Browse(p_ARO->DelayedLsa, DlyLsaDelete, 0);


   HL_Browse(p_ARO->IfoHl, IFO_Delete, 0);

   p_RTO->AreasNum -= 1;

   /* if this router is no longer BR, reoriginate
      Router LSA in first ARO without 'B' flag */
   if(((t_RTO*)p_ARO->RTO_Id)->AreasNum == 1)
   {
      if(HL_GetFirst(((t_RTO*)p_ARO->RTO_Id)->AroHl, (void**)&p_firstAro) == E_OK)
         LsaReOriginate[S_ROUTER_LSA](p_firstAro, 0);
   }

   /* Delete routing table entries related to the deleted area */
   e = HL_GetFirst(p_RTO->RtbHl,  (void**)&p_RTB);
   while(e == E_OK)
   {
      AVL_Browse(p_RTB->RtbRtBt, RteOfAreaDeleteAndNotify, (ulng)p_ARO);
      AVL_Browse(p_RTB->RtbNtBt, RteOfAreaDeleteAndNotify, (ulng)p_ARO);

      /* If the last area is being deleted cleanup multicust hash lists */
      /* and stop routing table recalculation timer                     */
      if(p_RTO->AreasNum == 0)
      {
         HL_Cleanup(p_RTB->LocGroupHl,1);
         HL_Cleanup(p_RTB->ForwCacheHl,1);

         TIMER_Stop( p_RTB->RecalcTimer );
      }

      e = HL_GetNext(p_RTO->RtbHl, (void**)&p_RTB, p_RTB);
   }
#if L7_MIB_OSPF
   ospfAreaTable_DeleteInternal( ((t_RTO*)p_ARO->RTO_Id)->ospfMibHandle,
                                 p_ARO->AreaId);
#endif

   RTB_ClearSpt(p_ARO);
   RTB_ClearCandidateHl(p_ARO);
   AVLH_Destroy(&p_ARO->SpfHl);
   AVLH_Destroy(&p_ARO->CandidateHl);

   HL_Destroy(&p_ARO->AdrRangeHl);
   HL_Destroy(&p_ARO->HostHl);
   HL_Destroy(&p_ARO->StubDefaultCostHl);

   AVLH_Destroy(&p_ARO->RouterLsaHl);
   AVLH_Destroy(&p_ARO->NetworkLsaHl);
   AVLH_Destroy(&p_ARO->NetSummaryLsaHl);
   AVLH_Destroy(&p_ARO->AsBoundSummaryLsaHl);
   AVLH_Destroy(&p_ARO->GroupMembLsaHl);
   /* NSSA Change */
   AVLH_Destroy(&p_ARO->NssaLsaHl);
   AVLH_Destroy(&p_ARO->LinkOpaqueLsaHl);
   AVLH_Destroy(&p_ARO->AreaOpaqueLsaHl);

#if L7_OSPF_TE
   AVLH_Destroy(&p_ARO->TeRtrHl);
   AVLH_Destroy(&p_ARO->TeLsaHl);
#endif

   HL_Destroy(&p_ARO->DelayedLsa);
   HL_Destroy(&p_ARO->IfoHl);

   p_ARO->Taken = FALSE;
   XX_Free( p_ARO );

   return ((Bool) flag);
}




/*********************************************************************
 * @purpose            Area Object Config get and set, depend on
 *                     RowStatus field value.
 *
 *
 * @param ARO_Id       @b{(input)}  ARO object Id
 * @param  p_Cfg       @b{(input)}  ARO config parameter structure
 *
 * @returns  E_OK       success
 * @returns  E_FAILED   failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
 e_Err ARO_Config(t_Handle ARO_Id, t_S_AreaCfg *p_Cfg)
  {
   t_ARO *p_ARO   = (t_ARO *) ARO_Id;
   Bool   tesup;
   t_RTO *p_RTO   = NULLP;
   e_AreaImportExternalCfg oldAreaType;
   Bool oldImportSummaries;
   t_A_DbKey dbkey;
   t_A_DbEntry *p_DbEntry;
   e_Err e;

   if(!p_ARO)
      return E_FAILED; /* The Area object does not exist */

   oldAreaType = p_ARO->ExternalRoutingCapability;
   oldImportSummaries = p_ARO->ImportSummaries;
   p_RTO = (t_RTO*)p_ARO->RTO_Id;

   switch(p_Cfg->AreaStatus)
   {
      case  ROW_ACTIVE:
         p_ARO->OperationState = TRUE;
      break;
      case  ROW_NOT_IN_SERVICE:
         p_ARO->OperationState = FALSE;
      break;
      case  ROW_CHANGE:
         p_ARO->ExternalRoutingCapability = p_Cfg->ExternalRoutingCapability;

         /* This function is only called for regular areas and stub areas.
          * NSSA_Config() is called for NSSAs. */
         if (p_Cfg->ExternalRoutingCapability == AREA_IMPORT_NO_EXTERNAL)
         {
           /* stub area */
           p_ARO->ImportSummaries = (p_Cfg->AreaSummary == AREA_SEND_SUMMARY)? TRUE: FALSE;
         }
         else
         {
           p_ARO->ImportSummaries = TRUE;
         }
         
         tesup = p_ARO->TEsupport;
         p_ARO->TEsupport = p_Cfg->TEsupport;

         if(routerIsBorder(p_RTO))
         {
           if (p_ARO->ImportSummaries == FALSE)
           {
             if (oldImportSummaries == TRUE)
             {
               /* Flush all previously originated summary LSAs except for the default. */
               /* LsaFlooding() suppresses summaries if ImportSummaries is FALSE. So
                * temporarily set it to TRUE while we flush. */
               p_ARO->ImportSummaries = TRUE;
               e = AVLH_GetFirst(p_ARO->NetSummaryLsaHl, (void**) &p_DbEntry);
               while (e == E_OK)
               {
                 if ((A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter) == p_RTO->Cfg.RouterId) &&
                     (A_GET_4B(p_DbEntry->Lsa.LsId) != DefaultDestination))
                 {
                   FlushLsa((t_Handle) p_DbEntry, (ulng) p_ARO, 0);
                 }
                 e = AVLH_GetNext(p_ARO->NetSummaryLsaHl, (void**) &p_DbEntry, (void*) p_DbEntry);
               }
               p_ARO->ImportSummaries = FALSE;  /* restore value */
             }
           }
           else  /* ImportSummaries is TRUE */
           {
             if (oldImportSummaries == FALSE)
             {
               /* Only re-originate summaries if changing from "no-import" to "import" */
               LsaOrgSumBulk(p_ARO);
             }
           }

           if ((p_Cfg->ExternalRoutingCapability == AREA_IMPORT_NO_EXTERNAL) &&
               (oldAreaType != AREA_IMPORT_NO_EXTERNAL))
           {
             /* Area changed to a stub area. Originate a default route into 
              * the stub area. */
             StubDefaultLsaOriginate(p_ARO);
           }
           else if ((p_Cfg->ExternalRoutingCapability != AREA_IMPORT_NO_EXTERNAL) &&
                    (oldAreaType == AREA_IMPORT_NO_EXTERNAL))
           {
             /* Area used to be a stub but is no longer. Flush default route. */
             A_SET_4B(p_RTO->Cfg.RouterId, dbkey.AdvertisingRouter);
             A_SET_4B(DefaultDestination, dbkey.LsId);
             if (AVLH_Find(p_ARO->NetSummaryLsaHl, (byte *)&dbkey, (void *)&p_DbEntry, 0) == E_OK)
             {
               FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);
               p_ARO->StubDefLSAOriginated = FALSE;
             }
           }
         }

#if L7_OSPF_TE
         /* If TE support switched on */
         if(!tesup && p_ARO->TEsupport)
            TE_SupportIsStarted(p_ARO);

         /* If TE support switched off */
         if(tesup && !p_ARO->TEsupport)
            TE_SupportIsStopped(p_ARO);
#endif

      break;
      case  ROW_READ:

         p_Cfg->AreaId = p_ARO->AreaId;
         p_Cfg->ExternalRoutingCapability = p_ARO->ExternalRoutingCapability;
         p_Cfg->AreaSummary = (p_ARO->ImportSummaries == TRUE)? AREA_SEND_SUMMARY: AREA_NO_SUMMARY;
         p_Cfg->SpfRuns = p_ARO->SpfRuns;
         p_Cfg->AreaBdrRtrCount = p_ARO->AreaBdrRtrCount;
         p_Cfg->AsBdrRtrCount = p_ARO->AsBdrRtrCount;
         p_Cfg->AreaLsaCount = p_ARO->AreaLsaCount;
         p_Cfg->AreaLsaCksumSum = p_ARO->AreaLsaCksumSum;
         p_Cfg->AreaStatus = p_ARO->OperationState ? ROW_ACTIVE : ROW_NOT_IN_SERVICE;
         p_Cfg->TEsupport = p_ARO->TEsupport;

      break;
      case  ROW_DESTROY:
         ARO_Delete(p_ARO, 1);
      break;
     case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:
      default:
      return E_FAILED;
   }

#if L7_MIB_OSPF
   /* Update the OSPF MIB Area table */
   if (p_Cfg->AreaStatus == ROW_CHANGE)
   {
      p_Cfg->AreaId = p_ARO->AreaId;
      ospfAreaTable_Update(((t_RTO *)p_ARO->RTO_Id)->ospfMibHandle, p_Cfg);
   }
#endif

   return E_OK;
}





/*********************************************************************
 * @purpose            NSSA Area Object Config get and set, depend on
 *                     RowStatus field value.
 *
 *
 * @param  ARO_Id      @b{(input)}  NSSA ARO object Id
 * @param  p_Cfg       @b{(input)}  NSSA config parameter structure
 *
 * @returns  E_OK       success
 * @returns  E_FAILED   failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err NSSA_Config(t_Handle ARO_Id, t_S_NssaCfg *p_Cfg)
{
   t_RTB  *p_RTB;
   t_VpnCos  vpn   = 0;
   t_ARO *p_ARO   = (t_ARO *) ARO_Id;
   t_RTO *p_RTO   = NULLP;
   e_Err  err = E_FAILED;
   t_A_DbKey dbkey;
   t_S_AsExternalCfg *p_Entry;
   t_A_DbEntry *p_DbEntry;

   if(!p_ARO || !p_ARO->OperationState)
      return E_STATE_NOT_APPL;

   p_RTO = (t_RTO*)p_ARO->RTO_Id;

   switch(p_Cfg->NSSAStatus)
   {
   case  ROW_CREATE_AND_GO:
   case  ROW_ACTIVE:
     /* Update NSSA specific configuration parms */
     p_ARO->ExternalRoutingCapability = AREA_IMPORT_NSSA;
     p_ARO->ImportSummaries  = p_Cfg->ImportSummaries;
     p_ARO->DefaultCost = p_Cfg->DefaultCost;
     p_ARO->NSSARedistribute = p_Cfg->NSSARedistribute;
     p_ARO->NSSADefInfoOrig = p_Cfg->NSSADefInfoOrig;
     p_ARO->NSSADefMetric   = p_Cfg->NSSADefMetric;
     p_ARO->NSSADefMetricType  = p_Cfg->NSSADefMetricType;
     p_ARO->NSSATranslatorRole = p_Cfg->NSSATranslatorRole;
     p_ARO->TranslatorStabilityInterval = p_Cfg->TranslatorStabilityInterval;
     p_ARO->OperationState = TRUE;

     /* Originate Type-7 LSAs for external routes that need to be re-distributed
     ** into the NSSA
     */
     if(p_ARO->NSSARedistribute == TRUE)
     {
       /* Iterate through the As External Routes and originate Type-7 LSAs  */
       err = HL_GetFirst(p_RTO->AsExtRoutesHl, (void *)&p_Entry);
       while(err == E_OK)
       {
          if(p_Entry->DestNetIpAdr)
          {
             p_Entry->PrevEntry = (t_Handle)RTE_ADDED;
             LsaReOriginate[S_NSSA_LSA](p_ARO, (ulng)p_Entry);
          }
          err = HL_GetNext(p_RTO->AsExtRoutesHl, (void *)&p_Entry, p_Entry);
       }
     }

     /* Run NSSA Translator Election Algorithm */
     NssaTranslatorElect(ARO_Id, 0);

     /* Originate a default LSA if needed */
     NssaDefaultLsaOriginate(p_ARO);
     break;

   case  ROW_CHANGE:
     /* Check if this is an NSSA */
     if(p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA)
       return E_STATE_NOT_APPL;

     /* If the user has changed the configuration of the translator role, re-run
     ** the translator elect algorithm
     */
     if(p_ARO->NSSATranslatorRole != p_Cfg->NSSATranslatorRole)
     {
       p_ARO->NSSATranslatorRole = p_Cfg->NSSATranslatorRole;
       NssaTranslatorElect(ARO_Id, 0);
     }

     /* Update the stability interval if there is a change */
     if(p_ARO->TranslatorStabilityInterval != p_Cfg->TranslatorStabilityInterval)
       p_ARO->TranslatorStabilityInterval = p_Cfg->TranslatorStabilityInterval;

     A_SET_4B(p_RTO->Cfg.RouterId,  dbkey.AdvertisingRouter);
     A_SET_4B(DefaultDestination, dbkey.LsId);
     if(p_ARO->ImportSummaries != p_Cfg->ImportSummaries)
     {
       p_ARO->ImportSummaries = p_Cfg->ImportSummaries;
       if(p_Cfg->ImportSummaries == TRUE)
       {
         /* Reorginate summary LSAs into the NSSA */
         LsaOrgSumBulk(p_ARO);

         /* If we had previously originated a Type-3 Default LSA, now that ImportSummaries
         ** has been turned on we can flush it and originate a Type-7 default LSA
         ** instead
         */
         if(AVLH_Find(p_ARO->NetSummaryLsaHl, (byte *)&dbkey, (void *)&p_DbEntry, 0) == E_OK)
           FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);

         if(routerIsBorder(p_RTO))
           NssaDefaultLsaOriginate(p_ARO);
       }
       else  /* transition from import to no import summaries */
       {
         /* Flush all previously originated summary LSAs */
         /* temporarily allow summaries while we flush */
         p_ARO->ImportSummaries = TRUE;
         AVLH_Browse(p_ARO->NetSummaryLsaHl, (BROWSEFUNC)FlushLsa, (ulng)p_ARO);
         p_ARO->ImportSummaries = FALSE;

         /* If we had previously originated a Type-7 Default LSA now that ImportSummaries
         ** has been turned off we can flush it and originate a Type-7 default LSA
         ** instead provided that we are an NSSA ABR.  Internal NSSA ASBRs are not impacted 
         ** by this configuration change.  The Type-7 default LSAs originated by NSSA 
         ** internal routers and the no-summary option are mutually exclusive features. 
         */
         if((AVLH_Find(p_ARO->NssaLsaHl, (byte *)&dbkey, (void *)&p_DbEntry, 0) == E_OK) &&
            (routerIsBorder(p_RTO)))
           FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);

         if(routerIsBorder(p_RTO))
           NssaDefaultLsaOriginate(p_ARO);
       }
     }

     if(p_ARO->NSSARedistribute != p_Cfg->NSSARedistribute)
     {
       p_ARO->NSSARedistribute = p_Cfg->NSSARedistribute;
       if(p_Cfg->NSSARedistribute == TRUE)
       {
         /* Iterate through the As External Routes and originate Type-7 LSAs  */
         err = HL_GetFirst(p_RTO->AsExtRoutesHl, (void *)&p_Entry);
         while(err == E_OK)
         {
            if(p_Entry->DestNetIpAdr)
            {
               p_Entry->PrevEntry = (t_Handle)RTE_ADDED;
               LsaReOriginate[S_NSSA_LSA](p_ARO, (ulng)p_Entry);
            }
            err = HL_GetNext(p_RTO->AsExtRoutesHl, (void *)&p_Entry, p_Entry);
         }
       }
       else
         AVLH_Browse(p_ARO->NssaLsaHl, (BROWSEFUNC)FlushLsa, (ulng)p_ARO);
     }

     if((p_ARO->NSSADefInfoOrig != p_Cfg->NSSADefInfoOrig) || 
        (p_ARO->NSSADefMetric != p_Cfg->NSSADefMetric) ||
        (p_ARO->NSSADefMetricType != p_Cfg->NSSADefMetricType) ||
        (p_ARO->DefaultCost != p_Cfg->DefaultCost))
     {
       p_ARO->NSSADefInfoOrig   = p_Cfg->NSSADefInfoOrig;
       p_ARO->NSSADefMetric     = p_Cfg->NSSADefMetric;
       p_ARO->NSSADefMetricType = p_Cfg->NSSADefMetricType;
       p_ARO->DefaultCost = p_Cfg->DefaultCost;

       if(AVLH_Find(p_ARO->NssaLsaHl, (byte *)&dbkey, (void *)&p_DbEntry, 0) == E_OK)
         FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);
       
       if(AVLH_Find(p_ARO->NetSummaryLsaHl, (byte *)&dbkey, (void *)&p_DbEntry, 0) == E_OK)
         FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);

       NssaDefaultLsaOriginate(p_ARO);
     }
     break;

   case  ROW_READ:
     /* Check if this is an NSSA */
     if(p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA)
       return E_STATE_NOT_APPL;

     p_Cfg->AreaId = p_ARO->AreaId;
     p_Cfg->ImportSummaries = p_ARO->ImportSummaries;
     p_Cfg->DefaultCost = p_ARO->DefaultCost;
     p_Cfg->NSSADefInfoOrig = p_ARO->NSSADefInfoOrig;
     p_Cfg->NSSADefMetric   = p_ARO->NSSADefMetric;
     p_Cfg->NSSADefMetricType = p_ARO->NSSADefMetricType;
     p_Cfg->NSSAStatus        = p_ARO->OperationState ? ROW_ACTIVE : ROW_NOT_IN_SERVICE;
     p_Cfg->NSSATranslatorRole  = p_ARO->NSSATranslatorRole;
     p_Cfg->NSSATranslatorState = p_ARO->NSSATranslatorState;
     p_Cfg->TranslatorStabilityInterval = p_ARO->TranslatorStabilityInterval;
     break;

   case  ROW_NOT_IN_SERVICE:
   case  ROW_DESTROY:
     /* Check if this is an NSSA */
     if(p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA)
       return E_STATE_NOT_APPL;

     AVLH_Browse(p_ARO->NssaLsaHl, (BROWSEFUNC)FlushLsa, (ulng)p_ARO);

     A_SET_4B(p_RTO->Cfg.RouterId,  dbkey.AdvertisingRouter);
     A_SET_4B(DefaultDestination, dbkey.LsId);

     /* Flush previously originated default LSAs */
     if(AVLH_Find(p_ARO->NssaLsaHl, (byte *)&dbkey, (void *)&p_DbEntry, 0) == E_OK)
       FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);

     if(AVLH_Find(p_ARO->NetSummaryLsaHl, (byte *)&dbkey, (void *)&p_DbEntry, 0) == E_OK)
       FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);

     /* Disabling the translator state flushes the Type-5 LSAs (translatd Type-7 LSAs)
      * originated by this router into other areas */
     NssaTranslatorStateSet(p_ARO, NSSA_TRANS_STATE_DISABLED);

     /* After the NSSA configuration is removed adjacencies have to be re-established
     ** which will cause external LSAs to be re-flooded into the area
     */

     /* Schedule SPF to be executed immediately, so that the routing table entries for NSSA
      * routes (N1 and N2 routes) don't have dangling references to the NSSA LSAs that
      * were flushed above. Else it might result in access to those flushed LSAs before SPF runs
      * at its own periodicity, when we reconfigure the NSSA area that tries to translate these
      * Type-7 routes to Type-5 lsas */
     if(HL_FindFirst(p_RTO->RtbHl, (byte *)&vpn, (void *)&p_RTB) == E_OK)
     {
       ScheduleSPF(p_RTB);
     }

     break;
   default:
      return E_FAILED;
   }

   return E_OK;
}

/*********************************************************************
 * @purpose  Validate LSA type value.
 *
 * @param    lsaType  @b{(input)}  LSA type value
 *
 * @returns  L7_TRUE if LSA type value is a valid value
 *
 * @notes    e_S_LScodes defines valid values
 *
 * @end
 * ********************************************************************/
L7_BOOL o2LsaTypeValid(byte lsaType)
{
  return ((lsaType > S_ILLEGAL_LSA) && (lsaType < S_LAST_LSA_TYPE));
}

/*********************************************************************
 * @purpose       Get the LSA hash list for a given area and LSA type.
 *
 *
 * @param  ARO_Id        @b{(input)}  ARO object Id
 * @param  p_AreaStats   @b{(input/output)}  Area LSDB stats pointer
 *
 * @returns  E_OK    success
 * @returns  other   failed
 *
 * @notes     For now, assumes a single instance of RTO. If we go to multiple instances, 
 *            need to pass RTO to this function.
 *
 * @end
 * ********************************************************************/
t_Handle GET_OWNER_HL(t_ARO *p_ARO, e_S_LScodes lsaType)
{
  if (!o2LsaTypeValid(lsaType))
    return NULL;

  if (lsaType == S_AS_EXTERNAL_LSA) 
    return RTO_List->AsExternalLsaHl;
  if (lsaType == S_AS_OPAQUE_LSA) 
    return RTO_List->AsOpaqueLsaHl;
  return *(&(p_ARO->RouterLsaHl) + (lsaType-1));
}

/*********************************************************************
 * @purpose              Area LSDB stats report.
 *
 *
 * @param  ARO_Id        @b{(input)}  ARO object Id
 * @param  p_AreaStats   @b{(input/output)}  Area LSDB stats pointer
 *
 * @returns  E_OK    success
 * @returns  other   failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARO_LsaStatsReport(t_Handle ARO_Id, L7_OspfAreaDbStats_t *p_AreaStats)
{
   t_ARO         *p_ARO = (t_ARO *) ARO_Id;

   p_AreaStats->RouterLsaCount      = p_ARO->AreaLsaStats[S_ROUTER_LSA];
   p_AreaStats->NetworkLsaCount     = p_ARO->AreaLsaStats[S_NETWORK_LSA];
   p_AreaStats->NetSumLsaCount      = p_ARO->AreaLsaStats[S_IPNET_SUMMARY_LSA];
   p_AreaStats->RtrSumLsaCount      = p_ARO->AreaLsaStats[S_ASBR_SUMMARY_LSA];
   p_AreaStats->NSSALsaCount        = p_ARO->AreaLsaStats[S_NSSA_LSA];
   p_AreaStats->OpaqueLinkLsaCount  = p_ARO->AreaLsaStats[S_LINK_OPAQUE_LSA];
   p_AreaStats->OpaqueAreaLsaCount  = p_ARO->AreaLsaStats[S_AREA_OPAQUE_LSA];
   p_AreaStats->SelfOrigNSSALsaCount= p_ARO->SelfOrigNSSALsaCount;

   return E_OK;
}

/*********************************************************************
 * @purpose              Stub Area's default metric config set/get .
 *
 *
 * @param  ARO_Id        @b{(input)}  ARO object Id
 * @param  p_StubCfg     @b{(input)}  Stub area metrics config structure
 *
 * @returns  E_OK    success
 * @returns  other   failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARO_StubMetric(t_Handle ARO_Id, t_S_StubAreaEntry *p_StubCfg)
{
   t_ARO         *p_ARO = (t_ARO *) ARO_Id;
   t_StubMetricEntry *p_M;
   e_Err          e;
   word           command;
   Bool           stubABR = FALSE;
   t_A_DbKey dbkey;
   t_A_DbEntry         *p_DbEntry = NULLP;
   t_RTO  *p_RTO;

   /* Verify if the Area object is OK */
   if(!p_ARO || !p_ARO->OperationState)
      return E_STATE_NOT_APPL;

   p_RTO = (t_RTO *)p_ARO->RTO_Id;

   /* Determine is the router is a stub area border router */
   stubABR = ((p_ARO->ExternalRoutingCapability == AREA_IMPORT_NO_EXTERNAL) &&
              (routerIsBorder(p_RTO)));

   /* try to find exist metric entry */
   e = HL_FindFirst(p_ARO->StubDefaultCostHl, (byte *) &p_StubCfg->StubTOS, (void *)&p_M);

   command = p_StubCfg->StubStatus;
   switch(p_StubCfg->StubStatus)
   {
      case  ROW_ACTIVE:
         if(e == E_OK)
            p_M->IsValid = TRUE;
      break;
      case  ROW_NOT_IN_SERVICE:
         if(e == E_OK)
            p_M->IsValid = FALSE;
      break;
      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:

         if(e == E_OK) /* if already exist */
            return E_FAILED;

         if((p_M = XX_Malloc(sizeof(t_StubMetricEntry))) == NULL)
            return E_NOMEMORY;
         memset(p_M, 0, sizeof(t_MetricEntry));

         p_M->MetricTOS = p_StubCfg->StubTOS;
         p_M->Metric = (word) p_StubCfg->StubMetric;
         p_M->StubMetricType = p_StubCfg->StubMetricType;
         p_M->IsValid = TRUE;

         e = HL_Insert(p_ARO->StubDefaultCostHl, (byte *) &p_M->MetricTOS, p_M);

      break;
      case  ROW_CHANGE:
         if(e == E_OK)
         {
            p_M->MetricTOS = p_StubCfg->StubTOS;
            p_M->Metric = (word) p_StubCfg->StubMetric;
            p_M->StubMetricType = p_StubCfg->StubMetricType;
            p_M->IsValid = TRUE;
         }
      break;
      case  ROW_READ:
         if(e == E_OK)
         {
            p_StubCfg->AreaId = p_ARO->AreaId;
            p_StubCfg->StubTOS = p_M->MetricTOS;
            p_StubCfg->StubMetric = p_M->Metric;
            p_StubCfg->StubMetricType = p_M->StubMetricType;
            p_StubCfg->StubStatus = p_M->IsValid ? ROW_ACTIVE : ROW_NOT_IN_SERVICE;
         }
      break;
   case  ROW_DESTROY:
         if(e == E_OK)
         {
         A_SET_4B(p_RTO->Cfg.RouterId,  dbkey.AdvertisingRouter);
         A_SET_4B(DefaultDestination, dbkey.LsId);
         if(AVLH_Find(p_ARO->NetSummaryLsaHl, (byte *)&dbkey,
                      (void *)&p_DbEntry, 0) == E_OK)
         {
           FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);
         }

         e = HL_DeleteFirst(p_ARO->StubDefaultCostHl,
                            (byte *)&p_StubCfg->StubTOS, (void *)&p_M);
         if(e == E_OK)
            XX_Free(p_M);
         }
      break;
      default:
         e = E_FAILED;
      break;
   }

#if L7_MIB_OSPF
   /* Update the OSPF MIB Stub Area (Metric) table */
   if ((e == E_OK) && (command != ROW_READ))
      ospfStubAreaTable_Update(((t_RTO*)p_ARO->RTO_Id)->ospfMibHandle,
                               p_ARO->AreaId, p_StubCfg);
#endif

   /* If any change occured on a stub area border router, then reoriginate the LSA */
   if((e == E_OK) && stubABR && (command != ROW_READ))
   {
      p_ARO->StubDefLSAOriginated = FALSE;
      e = StubDefaultLsaOriginate(p_ARO);
   }

   return e;
}





/*********************************************************************
 * @purpose              Area address range aggregations set/get.
 *
 *
 * @param  ARO_Id        @b{(input)}  ARO object Id
 * @param  p_AdrRange    @b{(input)}  Area's address range config structure
 *
 * @returns  E_OK        success
 * @returns  E_FAILED    failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARO_AddressRange(t_Handle ARO_Id, t_S_AreaAggregateEntry *p_AdrRange)
{
   t_ARO *p_ARO = (t_ARO *)ARO_Id;
   t_RTO *p_RTO;
   t_AgrEntry *p_Entry;
   e_Err       e = E_FAILED;
   word        command;

   if(!p_ARO || !p_ARO->OperationState)
      return E_STATE_NOT_APPL;
   p_RTO = (t_RTO*) p_ARO->RTO_Id;

   /* This check left here commened out as a warning not to re-add it.
    * While a well-intentioned check, the range may be configured before the
    * area is configured as an NSSA. In that case, the range would never
    * get installed. */
#if 0
   if ((p_AdrRange->LsdbType == AGGREGATE_NSSA_EXTERNAL_LINK) &&
       (p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA))
     return E_BADPARM;
#endif

   /* try to find exist entry */
   if(p_AdrRange->AggregateStatus != ROW_READ_NEXT)
   {
      e = HL_FindFirst(p_ARO->AdrRangeHl, (byte *) &p_AdrRange->NetIpAdr,
                       (void *)&p_Entry);
      if ((e == E_OK) && (p_AdrRange->LsdbType != p_Entry->LsdbType))
      {
        /* Yeah, this is really a bug, but not one expected to been seen 
         * in the wild. See comment on AdrRangeHl. */
        L7_uchar8 areaStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 prefixStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(p_ARO->AreaId, areaStr);
        osapiInetNtoa(p_AdrRange->NetIpAdr, prefixStr);
        osapiInetNtoa(p_AdrRange->SubnetMask, maskStr);
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_OSPF_MAP_COMPONENT_ID,
                "A %s range for %s/%s already exists on area %s.",
                (p_Entry->LsdbType == AGGREGATE_SUMMARY_LINK) ? "T3" : "T7", 
                prefixStr, maskStr, areaStr);
        L7_LOG(L7_LOG_SEVERITY_WARNING, L7_OSPF_MAP_COMPONENT_ID,
               "Cannot configure an area range of both types with the same prefix.");
        return E_FAILED;   /* we didn't really find a match */
      }
   }

   command = p_AdrRange->AggregateStatus;
   switch(p_AdrRange->AggregateStatus)
   {
      case  ROW_ACTIVE:
         if(e == E_OK)
         {
            if(p_Entry->AggregateStatus == p_AdrRange->AggregateStatus)
               /* no action is needed */
               return E_OK;

            p_Entry->AggregateStatus = p_AdrRange->AggregateStatus;
            if(p_AdrRange->LsdbType == AGGREGATE_SUMMARY_LINK)
              HideAllSummaryOfRange(p_ARO, p_Entry);
            else
              HideAllExtSummaryOfRange(p_RTO, p_Entry);

         }
         break;
      case  ROW_NOT_IN_SERVICE:
         if(e == E_OK)
         {
            if(p_Entry->AggregateStatus == p_AdrRange->AggregateStatus)
               /* no action is needed */
               return E_OK;

            p_Entry->AggregateStatus = p_AdrRange->AggregateStatus;
            if(p_AdrRange->LsdbType == AGGREGATE_SUMMARY_LINK)
              DeliverAllSummaryOfRange(p_ARO, p_Entry);
            else
              DeliverAllExtSummaryOfRange(p_RTO, p_Entry);
         }
         break;
      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:

         /* if already exist */
         if(e == E_OK)
            return E_IN_MATCH;

         if((p_Entry = XX_Malloc(sizeof(t_AgrEntry))) == NULL)
            return E_NOMEMORY;
         memset(p_Entry, 0, 2*sizeof(ulng));

         p_Entry->NetIpAdr = p_AdrRange->NetIpAdr;
         p_Entry->SubnetMask = p_AdrRange->SubnetMask;
         p_Entry->LsdbType   = p_AdrRange->LsdbType;
         p_Entry->AggregateEffect = p_AdrRange->AggregateEffect;
         p_Entry->LargestCost = -1;
         p_Entry->alive = FALSE;
         p_Entry->LcRte = NULL;

         /* allocate Hash List of destinations (rte's) belonging to the address */
         /* range of the aggregation.                                           */
         if(HL_Init(OWN_LINK, 2*sizeof(SP_IPADR), offsetof(t_RoutingTableEntry, DestinationId),
                      HL_SMALL_TABLE_SIZE, NULL, NULL, &p_Entry->AgrRteHl) != E_OK)
         {
            XX_Free(p_Entry);
            return E_FAILED;
         }

         /* Fill t_RoutingTableEntry struct in order to use it as a parameter  */
         /* of LsaReOriginate procedure                                        */
         memset(&p_Entry->AggregateRte, 0, sizeof(t_RoutingTableEntry));
         p_Entry->AggregateRte.AreaId          = p_ARO->AreaId;
         p_Entry->AggregateRte.DestinationId   = p_AdrRange->NetIpAdr;
         p_Entry->AggregateRte.IpMask          = p_AdrRange->SubnetMask;
         p_Entry->AggregateRte.DestinationType = DEST_TYPE_IS_NETWORK;
         p_Entry->AggregateRte.Cost            = p_Entry->LargestCost;

         /* Set row status */
         if (p_AdrRange->AggregateStatus == ROW_CREATE_AND_GO)
         {
            p_Entry->AggregateStatus = ROW_ACTIVE;

            if(p_AdrRange->LsdbType == AGGREGATE_SUMMARY_LINK)
              HideAllSummaryOfRange(p_ARO, p_Entry);
            else
              HideAllExtSummaryOfRange(p_RTO, p_Entry);
         }
         else
            p_Entry->AggregateStatus = ROW_NOT_IN_SERVICE;

         /* Insert the address range entry to the hash list */
         if(HL_Insert(p_ARO->AdrRangeHl, (byte *) &p_Entry->NetIpAdr, p_Entry) != E_OK)
         {
            XX_Free(p_Entry);
            return E_FAILED;
         }
         e = E_OK;

         break;
      case  ROW_CHANGE:
         if(e == E_OK)
         {
            if(p_Entry->AggregateEffect == p_AdrRange->AggregateEffect)
               /* do nothing */
               return E_OK;

            /* set the new value */
            p_Entry->AggregateEffect = p_AdrRange->AggregateEffect;

            if(p_AdrRange->AggregateEffect == AGGREGATE_DO_NOT_ADVERTISE)
            {
              if(p_AdrRange->LsdbType == AGGREGATE_SUMMARY_LINK)
                FlushAggregateSummary(p_ARO, p_Entry);
              else
                FlushExtAggregateSummary(p_RTO, p_Entry);
            }
            else
            {
              if(p_AdrRange->LsdbType == AGGREGATE_SUMMARY_LINK)
               OrigAggregateSummary(p_ARO, p_Entry);
              else
                OrigExtAggregateSummary(p_RTO, p_Entry);
            }
         }

         break;
      case  ROW_READ:
         if(e == E_OK)
         {
            p_AdrRange->AreaId = p_ARO->AreaId;
            p_AdrRange->LsdbType = p_Entry->LsdbType;
            p_AdrRange->NetIpAdr = p_Entry->NetIpAdr;
            p_AdrRange->SubnetMask = p_Entry->SubnetMask;
            p_AdrRange->AggregateStatus = p_Entry->AggregateStatus;
            p_AdrRange->AggregateEffect = p_Entry->AggregateEffect;
         }

         return e;
         break;
      case  ROW_READ_NEXT:
         if(p_AdrRange->PrevEntry == NULL)
            e = HL_GetFirst(p_ARO->AdrRangeHl, (void *)&p_Entry);
         else
            e = HL_GetNext(p_ARO->AdrRangeHl, (void *)&p_Entry, p_AdrRange->PrevEntry);

         if(e == E_OK)
         {
            p_AdrRange->AreaId = p_ARO->AreaId;
            p_AdrRange->LsdbType = p_Entry->LsdbType;
            p_AdrRange->NetIpAdr = p_Entry->NetIpAdr;
            p_AdrRange->SubnetMask = p_Entry->SubnetMask;
            p_AdrRange->AggregateStatus = p_Entry->AggregateStatus;
            p_AdrRange->AggregateEffect = p_Entry->AggregateEffect;
            p_AdrRange->PrevEntry =  p_Entry;
         }

         return e;
         break;
      case  ROW_DESTROY:

         if(e == E_OK)
         {
            /* Advertise all summary-LSA were suppressed "by the aggregation"*/
            if(p_Entry->AggregateStatus == ROW_ACTIVE)
            {
               p_Entry->AggregateStatus = ROW_NOT_READY; /* temperary status */
               if(p_Entry->LsdbType == AGGREGATE_SUMMARY_LINK)
                  DeliverAllSummaryOfRange(p_ARO, p_Entry);
               else
                  DeliverAllExtSummaryOfRange(p_RTO, p_Entry);
            }

            e = HL_DeleteFirst(p_ARO->AdrRangeHl, (byte *)&p_AdrRange->NetIpAdr,
                                                                (void *)&p_Entry);
            if(e == E_OK)
            {
               HL_Destroy(&p_Entry->AgrRteHl);
               XX_Free(p_Entry);
            }
         }

         break;
      default:
         return E_FAILED;
      break;
   }

#if L7_MIB_OSPF 
   /* Update the OSPF MIB Area Aggregate table */
   if ((e == E_OK) && (command != ROW_READ) && (command != ROW_READ_NEXT))
   {
      ospfAreaAggrTable_Update(((t_RTO*)p_ARO->RTO_Id)->ospfMibHandle,
                               p_ARO->AreaId, p_AdrRange);
   }
#endif

   return e;
}





/*********************************************************************
 * @purpose             Area local reachable hosts config set/get.
 *
 *
 * @param ARO_Id        @b{(input)}  ARO object Id
 * @param p_Hosts       @b{(input)} Area's reachable hosts config
 *                                  structure
 *
 * @returns             E_OK       success
 * @returns             E_FAILED   failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARO_HostConfig(t_Handle ARO_Id, t_S_HostRouteCfg *p_Hosts)
{
   t_ARO *p_ARO = (t_ARO *) ARO_Id;
   t_S_HostRouteCfg *p_Entry;
   e_Err e = E_OK;
   word  command;

   /* Verify if the Area object is OK */
   if(!p_ARO || !p_ARO->OperationState)
      return E_STATE_NOT_APPL;

   /* try to find exist entry */
   if(p_Hosts->HostStatus != ROW_READ_NEXT)
      e = HL_FindFirst(p_ARO->HostHl, (byte *) &p_Hosts->HostIpAdr, (void *)&p_Entry);

   command = p_Hosts->HostStatus;
   switch(p_Hosts->HostStatus)
   {
      case  ROW_ACTIVE:
      case  ROW_NOT_IN_SERVICE:
         p_Entry->HostStatus = p_Hosts->HostStatus;
      break;
      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:

         /* if already exist */
         if((e == E_OK) &&
            !memcmp((byte *) &p_Hosts + 2*sizeof(ulng),
                    (byte *) &p_Entry + 2*sizeof(ulng),
                    sizeof(t_S_HostRouteCfg)) - 2*sizeof(ulng))
            return E_FAILED;

         if((p_Entry = XX_Malloc(sizeof(t_S_HostRouteCfg))) == NULL)
            return E_NOMEMORY;

         memcpy((byte *) p_Entry + 2*sizeof(ulng),
                (byte *) p_Hosts + 2*sizeof(ulng),
                 sizeof(t_S_HostRouteCfg) - 2*sizeof(ulng));

         p_Entry->HostStatus = ROW_ACTIVE;

         e = HL_Insert(p_ARO->HostHl, (byte *) &p_Entry->HostIpAdr, p_Entry);

      break;
      case  ROW_CHANGE:
         if(e == E_OK)
            memcpy((byte *) p_Entry + 2*sizeof(ulng),
                   (byte *) p_Hosts + 2*sizeof(ulng),
                    sizeof(t_S_HostRouteCfg) - 2*sizeof(ulng));
      break;
      case  ROW_READ:
         if(e == E_OK)
         {
            memcpy((byte *) p_Hosts + 2*sizeof(ulng),
                  (byte *) p_Entry + 2*sizeof(ulng),
                   sizeof(t_S_HostRouteCfg) - 2*sizeof(ulng));
            p_Hosts->AreaId = p_ARO->AreaId;
         }
      break;
      case  ROW_READ_NEXT:
         if(p_Hosts->PrevEntry == NULL)
            e = HL_GetFirst(p_ARO->HostHl, (void *)&p_Hosts);
         else
            e = HL_GetNext(p_ARO->HostHl, (void *)&p_Hosts, p_Hosts->PrevEntry);

         p_Hosts->PrevEntry =  p_Hosts;
      break;
      case  ROW_DESTROY:

         e = HL_DeleteFirst(p_ARO->HostHl, (byte *)&p_Hosts->HostIpAdr, (void *)&p_Entry);
         if(e == E_OK)
            XX_Free(p_Entry);

      break;
      default:
         e = E_FAILED;
      break;
   }

#if L7_MIB_OSPF 
   /* Update the OSPF MIB Host table */
   if ((e == E_OK) && (command != ROW_READ) && (command != ROW_READ_NEXT))
   {
      p_Hosts->HostTOS = 0; /* we only support TOS 0 */
      ospfHostTable_Update(((t_RTO*)p_ARO->RTO_Id)->ospfMibHandle, p_Hosts);
   }
#endif

   /* If some change occured, then reoriginate the LSA */
   if((e == E_OK) && (command != ROW_READ) && (command != ROW_READ_NEXT) &&
      (command != ROW_CREATE_AND_WAIT))
      LsaReOriginate[S_ROUTER_LSA](p_ARO, 0);

   return e;
}




/*********************************************************************
 * @purpose             Area's LSA database browsing.
 *
 *
 * @param ARO_Id        @b{(input)}  ARO object Id
 * @param p_Lsa         @b{(input)}  Area's database LSA entry
 *
 * @return              E_OK       success
 * @return              E_FAILED   failed
 *
 * @notes
 *
 * @end
 *--------------------------------------------------------------------*/
e_Err ARO_LsaDatabase(t_Handle ARO_Id, t_S_LsDbEntry  *p_Lsa)
{
   t_ARO *p_ARO = (t_ARO *) ARO_Id;
   t_A_DbEntry *p_Entry;
   e_Err e;

   if(!p_ARO || !p_ARO->OperationState)
      return E_STATE_NOT_APPL;

   if((p_Lsa->LsdbType >= S_LAST_LSA_TYPE) ||
      (p_Lsa->LsdbType == S_ILLEGAL_LSA) ||
      (p_Lsa->LsdbType == S_TMP2_LSA))
      return E_FAILED;

   if(p_Lsa->PrevEntry == NULL)
      e = AVLH_GetFirst(GET_OWNER_HL(p_ARO, p_Lsa->LsdbType), (void *)&p_Entry);
   else
      e = AVLH_GetNext(GET_OWNER_HL(p_ARO, p_Lsa->LsdbType), (void *)&p_Entry, p_Lsa->PrevEntry);

   p_Lsa->PrevEntry = p_Entry;

   if(e != E_OK)
      return e;

   p_Lsa->AreaId = ((p_Lsa->LsdbType == S_AS_EXTERNAL_LSA) || (p_Lsa->LsdbType == S_AS_OPAQUE_LSA))?0:p_ARO->AreaId;
   p_Lsa->LsdbType = p_Entry->Lsa.LsType;
   p_Lsa->LsdbLsid = A_GET_4B(p_Entry->Lsa.LsId);
   p_Lsa->LsdbRouterId  = A_GET_4B(p_Entry->Lsa.AdvertisingRouter);
   p_Lsa->LsdbSequence  = A_GET_4B(p_Entry->Lsa.SeqNum);


/*---------------------------------------------------------------*/
/*    Note: These changes were added to fix ANVL OSPF test 24.6  */
/*---------------------------------------------------------------*/
   UpdateAgeOfDbEntry(p_Entry);

   p_Lsa->LsdbAge  = A_GET_2B(p_Entry->Lsa.LsAge);
   p_Lsa->LsdbChecksum = A_GET_2B(p_Entry->Lsa.CheckSum);
   p_Lsa->LsdbOptions = p_Entry->Lsa.Options;
   /* Note that this does NOT include the LSA header */
   p_Lsa->LsdbAdvertisement = p_Entry->p_Lsa;
   p_Lsa->LsdbAdvertLength  = (word)(A_GET_2B(p_Entry->Lsa.Length));

   return E_OK;
}

/*********************************************************************
 * @purpose         Updates the Lsdb Entry given to external world
 *
 *
 * @param p_Lsa     @b{(input)}  Lsdb Entry
 * @param p_Entry   @b{(input)}  Database Entry
 *
 * @notes
 *
 * @end
 *--------------------------------------------------------------------*/
void updateLsdbEntry(L7_ospfLsdbEntry_t *p_Lsa, t_A_DbEntry *p_Entry)
{
  t_ARO *p_ARO = (t_ARO *)p_Entry->ARO_Id;
  p_Lsa->AreaId = ((p_Lsa->LsdbType == S_AS_EXTERNAL_LSA) || 
                   (p_Lsa->LsdbType == S_AS_OPAQUE_LSA)) 
                   ? 0 : p_ARO->AreaId;

  p_Lsa->LsdbType = p_Entry->Lsa.LsType;
  p_Lsa->LsdbLsid = A_GET_4B(p_Entry->Lsa.LsId);
  p_Lsa->LsdbRouterId  = A_GET_4B(p_Entry->Lsa.AdvertisingRouter);
  p_Lsa->LsdbSequence  = A_GET_4B(p_Entry->Lsa.SeqNum);

  /*---------------------------------------------------------------*/
  /*    Note: These changes were added to fix ANVL OSPF test 24.6  */
  /*---------------------------------------------------------------*/
  UpdateAgeOfDbEntry(p_Entry);

  p_Lsa->LsdbAge  = A_GET_2B(p_Entry->Lsa.LsAge);
  p_Lsa->LsdbChecksum = A_GET_2B(p_Entry->Lsa.CheckSum);
  p_Lsa->LsdbOptions = p_Entry->Lsa.Options;
  p_Lsa->LsdbAdvertLength  = (L7_ushort16)(A_GET_2B(p_Entry->Lsa.Length));
  p_Lsa->rtrLsaFlags = *p_Entry->p_Lsa;

  return;
}

/*********************************************************************
 * @purpose         Gives the External LSA entry to external world
 *
 *
 * @param RTO_Id    @b{(input)}  RTO handle
 * @param p_Lsa     @b{(input)}  LSA Entry
 *
 * @return              E_OK       success
 * @return              E_FAILED   failed
 *
 * @notes
 *
 * @end
 *--------------------------------------------------------------------*/
e_Err ARO_ExtLsdbAdvertisement(t_Handle RTO_Id,
                               L7_ospfLsdbEntry_t *p_Lsa,
                               L7_char8 **p_LsdbAdvertisement)
{
  t_A_DbEntry   *p_Entry = NULL;
  t_A_DbKey      dbKey;
  byte *lsaBody;

  t_RTO         *p_RTO = (t_RTO *)RTO_Id;

  /* Form the Key for the LSA before we find the entry in the area's LSDB */
  A_SET_4B(p_Lsa->LsdbLsid, dbKey.LsId);
  A_SET_4B(p_Lsa->LsdbRouterId, dbKey.AdvertisingRouter);

  if((AVLH_Find(p_RTO->AsExternalLsaHl,
                (byte *)&dbKey,
                (void **)&p_Entry,0)) != E_OK)
  {
    return E_FAILED;
  }

  updateLsdbEntry(p_Lsa, p_Entry);

  /* Make a copy the whole advertisement here,
   * The caller is responsible for de-allocating this copy */
  if(p_LsdbAdvertisement != NULL)
  {
    /* Use osapiMalloc() rather than XX_Malloc() here. UI component will 
     * have to free this memory. Because of linkable object restriction 
     * on what the UI components can include, they can't call XX_Free().
     */
    if((*p_LsdbAdvertisement = osapiMalloc(L7_OSPF_MAP_COMPONENT_ID, 
                                           p_Lsa->LsdbAdvertLength + 1)) == NULL)
    {
      *p_LsdbAdvertisement = NULL;
      return E_NOMEMORY;
    }
    memset(*p_LsdbAdvertisement, 0, p_Lsa->LsdbAdvertLength + 1);
    memcpy(*p_LsdbAdvertisement, &p_Entry->Lsa, LSA_HEADER_LEN);
    lsaBody = ((byte*) *p_LsdbAdvertisement) + LSA_HEADER_LEN;
    memcpy(lsaBody, p_Entry->p_Lsa, p_Lsa->LsdbAdvertLength - LSA_HEADER_LEN);
  }

  return E_OK;
}

/*********************************************************************
 * @purpose         Gives the External LSA entry to external world
 *
 *
 * @param RTO_Id    @b{(input)}  RTO handle
 * @param p_Lsa     @b{(input)}  LSA Entry
 *
 * @return              E_OK       success
 * @return              E_FAILED   failed
 *
 * @notes
 *
 * @end
 *--------------------------------------------------------------------*/
e_Err ARO_ExtLsdbEntry(t_Handle RTO_Id, L7_ospfLsdbEntry_t *p_Lsa)
{
  t_A_DbEntry   *p_Entry = NULL;
  t_A_DbKey      dbKey;

  t_RTO         *p_RTO = (t_RTO *)RTO_Id;

  /* Form the Key for the LSA before we find the entry in the area's LSDB */
  A_SET_4B(p_Lsa->LsdbLsid, dbKey.LsId);
  A_SET_4B(p_Lsa->LsdbRouterId, dbKey.AdvertisingRouter);

  if((AVLH_Find(p_RTO->AsExternalLsaHl,
                (byte *)&dbKey,
                (void **)&p_Entry,0)) != E_OK)
  {
    return E_FAILED;
  }

  updateLsdbEntry(p_Lsa, p_Entry);

  return E_OK;
}

/*********************************************************************
 * @purpose         Gives the LSA entry to external world
 *
 *
 * @param ARO_Id    @b{(input)}  Area Object handle
 * @param p_Lsa     @b{(input)}  LSA Entry
 * @param p_LsdbAdvertisement @b{(output)} LSDB Advertisement
 *
 * @return              E_OK       success
 * @return              E_FAILED   failed
 *
 * @notes
 *
 * @end
 *--------------------------------------------------------------------*/
e_Err ARO_LsdbAdvertisement(t_Handle ARO_Id,
                            L7_ospfLsdbEntry_t *p_Lsa,
                            L7_char8 **p_LsdbAdvertisement)
{
  t_A_DbEntry   *p_Entry = NULL;
  t_A_DbKey      dbKey;

  t_ARO         *p_ARO = (t_ARO *)ARO_Id;

  if (p_ARO && !p_ARO->OperationState)
  {
    return E_FAILED;
  }

  if((p_Lsa->LsdbType >= S_LAST_LSA_TYPE) ||
     (p_Lsa->LsdbType == S_ILLEGAL_LSA) ||
     (p_Lsa->LsdbType == S_TMP2_LSA))
  {
    return E_FAILED;
  }

  /* Form the Key for the LSA before we find the entry in the area's LSDB */
  A_SET_4B(p_Lsa->LsdbLsid, dbKey.LsId);
  A_SET_4B(p_Lsa->LsdbRouterId, dbKey.AdvertisingRouter);

  if((AVLH_Find(GET_OWNER_HL(p_ARO, p_Lsa->LsdbType),
                (byte *)&dbKey,
                (void **)&p_Entry,0)) != E_OK)
  {
    return E_FAILED;
  }

  updateLsdbEntry(p_Lsa, p_Entry);

  /* Make a copy of the whole advertisement here,
   * The caller is responsible for de-allocating this copy */
  if(p_LsdbAdvertisement != NULL)
  {
    /* Use osapiMalloc() rather than XX_Malloc() here. UI component will 
     * have to free this memory. Because of linkable object restriction 
     * on what the UI components can include, they can't call XX_Free().
     */
    if((*p_LsdbAdvertisement = osapiMalloc(L7_OSPF_MAP_COMPONENT_ID,
                                           p_Lsa->LsdbAdvertLength + 1)) == NULL)
    {
      *p_LsdbAdvertisement = NULL;
      return E_NOMEMORY;
    }
    memset(*p_LsdbAdvertisement, 0, p_Lsa->LsdbAdvertLength + 1);
    memcpy(*p_LsdbAdvertisement, &p_Entry->Lsa, LSA_HEADER_LEN);
    memcpy(*p_LsdbAdvertisement + LSA_HEADER_LEN, p_Entry->p_Lsa, 
           p_Lsa->LsdbAdvertLength - LSA_HEADER_LEN);
  }

  return E_OK;
}

/*********************************************************************
 * @purpose         Gives the LSA entry to external world
 *
 *
 * @param ARO_Id    @b{(input)}  Area Object handle
 * @param p_Lsa     @b{(input)}  LSA Entry
 *
 * @return              E_OK       success
 * @return              E_FAILED   failed
 *
 * @notes
 *
 * @end
 *--------------------------------------------------------------------*/
e_Err ARO_LsdbEntry(t_Handle ARO_Id, L7_ospfLsdbEntry_t *p_Lsa)
{
  t_A_DbEntry   *p_Entry = NULL;
  t_A_DbKey      dbKey;

  t_ARO         *p_ARO = (t_ARO *)ARO_Id;

  if(!p_ARO || !p_ARO->OperationState)
  {
    return E_FAILED;
  }

  if((p_Lsa->LsdbType >= S_LAST_LSA_TYPE) ||
     (p_Lsa->LsdbType == S_ILLEGAL_LSA) ||
     (p_Lsa->LsdbType == S_TMP2_LSA))
  {
    return E_FAILED;
  }

  /* Form the Key for the LSA before we find the entry in the area's LSDB */
  A_SET_4B(p_Lsa->LsdbLsid, dbKey.LsId);
  A_SET_4B(p_Lsa->LsdbRouterId, dbKey.AdvertisingRouter);

  if((AVLH_Find(GET_OWNER_HL(p_ARO, p_Lsa->LsdbType),
                (byte *)&dbKey,
                (void **)&p_Entry,0)) != E_OK)
  {
    return E_FAILED;
  }

  updateLsdbEntry(p_Lsa, p_Entry);

  return E_OK;
}
/*********************************************************************
 * @purpose         Updates the Opaque Lsdb Entry given to external world
 *
 *
 * @param p_Lsa     @b{(input)}  Lsdb Entry
 * @param p_Entry   @b{(input)}  Database Entry
 *
 * @notes
 *
 * @end
 *--------------------------------------------------------------------*/
void updateOpaqueLsdbEntry(L7_ospfOpaqueLsdbEntry_t *p_Lsa, t_A_DbEntry *p_Entry)
{
  t_ARO *p_ARO;

  p_ARO = ((p_Lsa->LsdbType == S_AS_EXTERNAL_LSA) ||
      (p_Lsa->LsdbType == S_AS_OPAQUE_LSA))
    ? L7_NULLPTR : (t_ARO *)p_Entry->ARO_Id;

  p_Lsa->AreaId = ((p_Lsa->LsdbType == S_AS_EXTERNAL_LSA) || 
      (p_Lsa->LsdbType == S_AS_OPAQUE_LSA)) 
    ? 0 : p_ARO->AreaId;

  p_Lsa->LsdbType = p_Entry->Lsa.LsType;
  p_Lsa->LsdbLsid = A_GET_4B(p_Entry->Lsa.LsId);
  p_Lsa->LsdbRouterId  = A_GET_4B(p_Entry->Lsa.AdvertisingRouter);
  p_Lsa->LsdbSequence  = A_GET_4B(p_Entry->Lsa.SeqNum);

  UpdateAgeOfDbEntry(p_Entry);

  p_Lsa->LsdbAge  = A_GET_2B(p_Entry->Lsa.LsAge);
  p_Lsa->LsdbChecksum = A_GET_2B(p_Entry->Lsa.CheckSum);
  p_Lsa->LsdbOptions = p_Entry->Lsa.Options;
  p_Lsa->LsdbAdvertLength  = (L7_ushort16)(A_GET_2B(p_Entry->Lsa.Length));

  return;
}

/*********************************************************************
 * @purpose         Gives the LSA entry to external world
 *
 *
 * @param ARO_Id    @b{(input)}  Area Object handle
 * @param p_Lsa     @b{(input)}  LSA Entry
 * @param p_LsdbAdvertisement @b{(output)} LSDB Advertisement
 *
 * @return              E_OK       success
 * @return              E_FAILED   failed
 *
 * @notes
 *
 * @end
 *--------------------------------------------------------------------*/
e_Err ARO_OpaqueAdvertisement(t_Handle ARO_Id,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa,
    L7_char8 **p_LsdbAdvertisement)
{
  t_A_DbEntry   *p_Entry = NULL;
  t_A_DbKey      dbKey;
  t_ARO         *p_ARO = (t_ARO *)ARO_Id;
  t_Handle       OpaqueLsaHl;
  byte *lsaBody;

  if((p_Lsa->LsdbType != S_LINK_OPAQUE_LSA)&&
      (p_Lsa->LsdbType != S_AREA_OPAQUE_LSA)&&
      (p_Lsa->LsdbType != S_AS_OPAQUE_LSA))
  {
    return E_FAILED;
  }

  if(p_Lsa->LsdbType != S_AS_OPAQUE_LSA)
    if(!p_ARO || !p_ARO->OperationState)
    {
      return E_FAILED;
    }

  /* Form the Key for the LSA before we find the entry in the area's LSDB */
  A_SET_4B(p_Lsa->LsdbLsid, dbKey.LsId);
  A_SET_4B(p_Lsa->LsdbRouterId, dbKey.AdvertisingRouter);

  OpaqueLsaHl = (p_Lsa->LsdbType != S_AS_OPAQUE_LSA) ?
    GET_OWNER_HL(p_ARO, p_Lsa->LsdbType):
      ((t_RTO *)ARO_Id)->AsOpaqueLsaHl;
  if((AVLH_Find(OpaqueLsaHl, (byte *)&dbKey,
          (void **)&p_Entry,0)) != E_OK)
  {
    return E_FAILED;
  }

  updateOpaqueLsdbEntry(p_Lsa, p_Entry);

  /* Make a copy of the whole advertisement here,
   * The caller is responsible for de-allocating this copy */
  if(p_LsdbAdvertisement != NULL)
  {
    /* Use osapiMalloc() rather than XX_Malloc() here. UI component will 
     * have to free this memory. Because of linkable object restriction 
     * on what the UI components can include, they can't call XX_Free().
     */
    if((*p_LsdbAdvertisement = osapiMalloc(L7_OSPF_MAP_COMPONENT_ID,
            p_Lsa->LsdbAdvertLength + 1)) == NULL)
    {
      *p_LsdbAdvertisement = NULL;
      return E_NOMEMORY;
    }
    memset(*p_LsdbAdvertisement, 0, p_Lsa->LsdbAdvertLength + 1);
    memcpy(*p_LsdbAdvertisement, &p_Entry->Lsa, LSA_HEADER_LEN);
    lsaBody = ((byte*) *p_LsdbAdvertisement) + LSA_HEADER_LEN;
    memcpy(lsaBody, p_Entry->p_Lsa, p_Lsa->LsdbAdvertLength - LSA_HEADER_LEN);
  }

  return E_OK;
}

/*********************************************************************
 * @purpose         Gives the Next LSA entry to external world
 *
 *
 * @param ARO_Id    @b{(input)}  Area Object handle
 * @param p_Lsa     @b{(input)}  LSA Entry
 *
 * @return              E_OK       success
 * @return              E_FAILED   failed
 *
 * @notes
 *
 * @end
 *--------------------------------------------------------------------*/
e_Err ARO_OpaqueLsdbNextEntry(t_Handle ARO_Id, L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  e_Err          e;
  t_A_DbEntry    *p_Entry = NULL;
  t_A_DbKey      currentKey;
  t_ARO *p_ARO = (t_ARO *) ARO_Id;
  t_Handle       OpaqueLsaHl;

  if((p_Lsa->LsdbType != S_LINK_OPAQUE_LSA)&&
      (p_Lsa->LsdbType != S_AREA_OPAQUE_LSA)&&
      (p_Lsa->LsdbType != S_AS_OPAQUE_LSA))
  {
    return E_FAILED;
  }

  if(p_Lsa->LsdbType != S_AS_OPAQUE_LSA)
    if(!p_ARO || !p_ARO->OperationState)
    {
      return E_FAILED;
    }

  if(((p_Lsa->LsdbLsid == 0) && (p_Lsa->LsdbRouterId == 0)))
  {
    /* In this case, we assume the query is for the first LSA
     * So we set the LsdbType to first LSA type */

    A_SET_4B(p_Lsa->LsdbLsid, currentKey.LsId);
    A_SET_4B(p_Lsa->LsdbRouterId, currentKey.AdvertisingRouter);

    OpaqueLsaHl = (p_Lsa->LsdbType != S_AS_OPAQUE_LSA) ?
      GET_OWNER_HL(p_ARO, p_Lsa->LsdbType):
        ((t_RTO *)ARO_Id)->AsOpaqueLsaHl;
    e = AVLH_FindNext(OpaqueLsaHl, (byte *)&currentKey, (void **)&p_Entry);
  }
  else
  {

    /* Form the Key for the current LSA, 
     * before we find the next entry in the area's LSDB */
    A_SET_4B(p_Lsa->LsdbLsid, currentKey.LsId);
    A_SET_4B(p_Lsa->LsdbRouterId, currentKey.AdvertisingRouter);

    OpaqueLsaHl = (p_Lsa->LsdbType != S_AS_OPAQUE_LSA) ?
      GET_OWNER_HL(p_ARO, p_Lsa->LsdbType):
        ((t_RTO *)ARO_Id)->AsOpaqueLsaHl;
    e = AVLH_FindNext(OpaqueLsaHl, (byte *)&currentKey, (void **)&p_Entry);
  }

  if(e != E_OK)
  {
    return E_FAILED;
  }

  updateOpaqueLsdbEntry(p_Lsa, p_Entry);

  return E_OK;
}

/*********************************************************************
 * @purpose         Gives the LSA entry to external world
 *
 *
 * @param ARO_Id    @b{(input)}  Area Object handle
 * @param p_Lsa     @b{(input)}  LSA Entry
 *
 * @return              E_OK       success
 * @return              E_FAILED   failed
 *
 * @notes
 *
 * @end
 *--------------------------------------------------------------------*/
e_Err ARO_OpaqueLsdbEntry(t_Handle ARO_Id, L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  t_A_DbEntry   *p_Entry = NULL;
  t_A_DbKey      dbKey;
  t_ARO         *p_ARO = (t_ARO *)ARO_Id;
  t_Handle      OpaqueLsaHl;

  if((p_Lsa->LsdbType != S_LINK_OPAQUE_LSA)&&
      (p_Lsa->LsdbType != S_AREA_OPAQUE_LSA)&&
      (p_Lsa->LsdbType != S_AS_OPAQUE_LSA))
  {
    return E_FAILED;
  }

  if(p_Lsa->LsdbType != S_AS_OPAQUE_LSA)
    if(!p_ARO || !p_ARO->OperationState)
    {
      return E_FAILED;
    }

  /* Form the Key for the LSA before we find the entry in the area's LSDB */
  A_SET_4B(p_Lsa->LsdbLsid, dbKey.LsId);
  A_SET_4B(p_Lsa->LsdbRouterId, dbKey.AdvertisingRouter);

  OpaqueLsaHl = (p_Lsa->LsdbType != S_AS_OPAQUE_LSA) ?
    GET_OWNER_HL(p_ARO, p_Lsa->LsdbType):
      ((t_RTO *)ARO_Id)->AsOpaqueLsaHl;

  if((AVLH_Find(OpaqueLsaHl, (byte *)&dbKey,
          (void **)&p_Entry,0)) != E_OK)
  {
    return E_FAILED;
  }

  updateOpaqueLsdbEntry(p_Lsa, p_Entry);

  return E_OK;
}

/*********************************************************************
 * @purpose         Gives the Next External LSA entry to external world
 *
 *
 * @param RTO_Id    @b{(input)}  RTO handle
 * @param p_Lsa     @b{(input)}  LSA Entry
 *
 * @return              E_OK       success
 * @return              E_FAILED   failed
 *
 * @notes
 *
 * @end
 *--------------------------------------------------------------------*/
e_Err ARO_ExtLsdbNextEntry(t_Handle RTO_Id, L7_ospfLsdbEntry_t *p_Lsa)
{
  e_Err          e;
  t_A_DbEntry    *p_Entry = NULL;
  t_A_DbKey      currentKey;


  t_RTO         *p_RTO = (t_RTO *)RTO_Id;

  if((p_Lsa->LsdbType >= S_LAST_LSA_TYPE) ||
     (p_Lsa->LsdbType == S_TMP2_LSA))
  {
    return E_FAILED;
  }

  if(((p_Lsa->LsdbLsid == 0) && (p_Lsa->LsdbRouterId == 0))
                             && (p_Lsa->LsdbType == S_ILLEGAL_LSA))
  {
    /* In this case, we assume the query is for the first external LSA */
    p_Lsa->LsdbType = (L7_uchar8)S_AS_EXTERNAL_LSA;
  }

  /* Form the Key for the current LSA, 
   * before we find the next entry in the area's LSDB */
  A_SET_4B(p_Lsa->LsdbLsid, currentKey.LsId);
  A_SET_4B(p_Lsa->LsdbRouterId, currentKey.AdvertisingRouter);

  e = AVLH_FindNext(p_RTO->AsExternalLsaHl,
                   (byte *)&currentKey, (void **)&p_Entry);
  
  if(e != E_OK)
  {
    return E_FAILED;
  }

  updateLsdbEntry(p_Lsa, p_Entry);

  return E_OK;
}

/*********************************************************************
 * @purpose         Gives the Next LSA entry to external world
 *
 *
 * @param ARO_Id    @b{(input)}  Area Object handle
 * @param p_Lsa     @b{(input)}  LSA Entry
 *
 * @return              E_OK       success
 * @return              E_FAILED   failed
 *
 * @notes
 *
 * @end
 *--------------------------------------------------------------------*/
e_Err ARO_LsdbNextEntry(t_Handle ARO_Id, L7_ospfLsdbEntry_t *p_Lsa)
{
  e_Err          e;
  t_A_DbEntry    *p_Entry = NULL;
  t_A_DbKey      currentKey;


  byte           LsaType, LastType;

  t_ARO *p_ARO = (t_ARO *) ARO_Id;
  t_RTO *p_RTO;

  if(!p_ARO || !p_ARO->OperationState)
  {
    return E_STATE_NOT_APPL;
  }

  p_RTO = (t_RTO *)(p_ARO->RTO_Id);
  
  if(p_RTO->Cfg.OpaqueCapability)
    LastType = S_LAST_LSA_TYPE;
  else
    LastType = S_TMP2_LSA;

  if(p_Lsa->LsdbType >= LastType) 
  {
    return E_FAILED;
  }

  if(((p_Lsa->LsdbLsid == 0) && (p_Lsa->LsdbRouterId == 0))
                             && (p_Lsa->LsdbType == S_ILLEGAL_LSA))
  {
    /* In this case, we assume the query is for the first LSA
     * So we set the LsdbType to first LSA type */
    p_Lsa->LsdbType = S_ROUTER_LSA;
  }

  /* Form the Key for the current LSA, 
   * before we find the next entry in the area's LSDB */
  A_SET_4B(p_Lsa->LsdbLsid, currentKey.LsId);
  A_SET_4B(p_Lsa->LsdbRouterId, currentKey.AdvertisingRouter);

  e = AVLH_FindNext(GET_OWNER_HL(p_ARO, p_Lsa->LsdbType),
                    (byte *)&currentKey, (void **)&p_Entry);

  if(e != E_OK)
  {
    /* If we are at the end of all LSAs in this Area, return failure */
    if(p_Lsa->LsdbType == (LastType))
      return E_FAILED;

    /* Browse other Hash Lists of this Area Database */
    for(LsaType = p_Lsa->LsdbType+1; LsaType < LastType; LsaType++)
    {
      if((LsaType == S_TMP2_LSA) || 
         (LsaType == S_AS_EXTERNAL_LSA) ||
         (LsaType == S_AS_OPAQUE_LSA))
      continue;

      if(AVLH_GetFirst(GET_OWNER_HL(p_ARO, LsaType), (void **)&p_Entry) == E_OK)
        break;
    }
    if(LsaType == LastType)
    {
      return E_FAILED;
    }
  }
  
  updateLsdbEntry(p_Lsa, p_Entry);

  return E_OK;
}

/*********************************************************************
 * @purpose    Enter the LSDB overload state.      
 *
 * @param  p_RTO    @b{(input)}  OSPF instance pointer
 *
 * @returns      E_OK
 *
 * @notes   When the router is not able to store an LSA because the LSDB is full
 *    the router enters an overload state. In the overload state, the router
 *    sets the link cost of non-stub links to LSInfinity so that other routers
 *    do not compute routes through the overloaded router. See RFC 3137. 
 *    For now, the operator must manually disable and re-enable OSPF to exit
 *    the overload state. A future release may add a mechanism to automatically
 *    exit the overload state when space becomes available again in the LSDB. 
 *
 * @end
 * ********************************************************************/
e_Err EnterLsdbOverload(t_Handle RTO_Id)
{
    t_RTO *p_RTO = (t_RTO*) RTO_Id;
    t_ARO *p_ARO = NULL;
    t_IFO *p_IFO = NULL;
    t_NBO *p_NBO = NULL;
    e_Err err;

    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_OSPF_MAP_COMPONENT_ID,
            "OSPFv2 entering overload state."
            " To restore OSPF to full operation, disable and re-enable OSPF "
            "When OSPFv2 runs out of resources, it goes into overload state. "
            "In this state, it originates a router LSA whose link costs discourage"
            " other routers from routing traffic through the overloaded router.");

    p_RTO->LsdbOverload = TRUE;

    /* For each area, reissue the router LSA with LSInfinity for non-stub links. */
    err = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
    while(err == E_OK)
    {
      /* clear NBO LSA request lists for nbr's this area */
      err = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
      while(err == E_OK)
      {
        err = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
        while(err == E_OK)
        {
          HL_Cleanup(p_NBO->LsReq,1);
          err = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
        }
        err = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
      }

        LsaReOriginate[S_ROUTER_LSA](p_ARO, 0);
        err = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
    }
    return E_OK;
}

/*********************************************************************
 * @purpose          Remove a database entry from the 
 *                   retx list of all neighbors in a given area.
 *
 * @param  p_ARO     @b{(input)}  owner Area Object Handler. 
 * @param  p_dbEntry  @b{(input)}   database entry to be removed
 *
 * @returns          E_OK
 *
 * @end
 * ********************************************************************/
e_Err RemoveFromRetxLists(t_ARO *p_ARO, t_A_DbEntry *p_DbEntry)
{
  t_IFO *p_IFO;
  t_NBO *p_NBO;
  t_RetxEntry *p_RetxEntry;
  e_Err e, er;

  /* for each interface.... */
  er = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
  while (er == E_OK)
  {
    /* Examine all neighbors on this interface */
    e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
    while (e == E_OK)
    {
      /* check retx list */
      if (HL_FindFirst(p_NBO->LsReTx, &p_DbEntry->Lsa.LsType, (void *)&p_RetxEntry) == E_OK)
      {
        if ((! (L7_BIT_ISSET(p_RetxEntry->DbEntry->dbFlags, IS_CUR))) ||
            (p_RetxEntry->DbEntry == p_DbEntry) ||
            (LsaCompare(&p_DbEntry->Lsa, &p_RetxEntry->DbEntry->Lsa) == LSA_EQUAL))
        {
          NBO_LsReTxDelete(p_NBO, &p_RetxEntry->LsType, p_RetxEntry);
        }
      }
      e = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
    }
    er = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
  }
  return E_OK;
}

/*********************************************************************
 * @purpose          Install LSA within Area Database.
 *
 * @param  p_RTO     @b{(input)}  OSPF instance
 * @param  p_ARO     @b{(input)}  owner Area Object Handler. NULL for LSAs with 
 *                                AS flooding scope.
 * @param  p_Lsa     @b{(input)}  LSA frame, in a t_Frame object. This
 *                                function always takes ownership of
 *                                this chunk of memory
 * @param  p_former  @b{(input)}  former instance to be deleted
 *
 * @returns          returns created DB entry pointer if success,
 * @returns          or NULL otherwise.
 *
 * @notes           ROBRICE - consider having callers pass an XX_Malloc'd 
 *                       chunk that we can take ownership of, rather than
 *                       a frame object. Avoid copying.
 *
 * @end
 * ********************************************************************/
t_A_DbEntry *LsaInstall(t_RTO *p_RTO, t_ARO *p_ARO, void *p_Lsa, t_A_DbEntry *p_former)
{
   t_A_DbEntry   *p_DbEntry;
   t_LsdbOverflow Overflow;
   Bool PrevOverflow;
   void *lsaBody = NULL;
   ulng lsaBodyLen;
   t_IFO *p_IFO;
   t_NBO *p_NBO;
   e_Err e, er, err;
   t_ARO *p_tmpARO;
   ulng lsdbPercentFull;
   static ulng lastLsdbPercentFull = 0;
   Bool lsaChanged = TRUE;
   byte oldHeader[sizeof(t_S_LsaHeader)];
   t_S_LsaHeader *lsaHeader = (t_S_LsaHeader *)GET_LSA_HDR(p_Lsa);

   if (!o2LsaTypeValid(lsaHeader->LsType))
   {
     F_Delete(p_Lsa);
     return NULL;
   }

   /* Check if content of the new and former LSAs differ or not. If they */
   /* are the same, just update the header of the existing (former) LSA  */
   /* and the area statistics (area checksum and counters)               */
   lsaChanged = (p_former ? LsaContentDiff(p_Lsa, p_former) : TRUE);
   if(!lsaChanged)
   {
      /* keep the former instance LSA header */
      memcpy(&oldHeader, &p_former->Lsa, sizeof(t_S_LsaHeader));

      UpdateStatCounters(p_RTO, p_ARO, p_former, FALSE);

      /* copy new 20-byte LSA header into db entry*/
      memcpy(&p_former->Lsa, GET_LSA_HDR(p_Lsa), sizeof(t_S_LsaHeader));
      p_former->OrgTime = TIMER_SysTime();  /* installation time stamp     */
      p_former->InitAge = (word)A_GET_2B(p_former->Lsa.LsAge); /* init age */

      /* remove old entry and insert new entry into the aging table */
      if((AgingTbl_DeleteEntry(p_RTO->AgingObj, p_former, 
                               p_former->AgeIndex) == E_OK) && 
         (AgingTbl_AddEntry(p_RTO->AgingObj, p_former, 
                p_former->InitAge, &p_former->AgeIndex) == E_OK))
      {
         /* Release temporary allocated LSA frame */
         F_Delete(p_Lsa);
         UpdateStatCounters(p_RTO, p_ARO, p_former, TRUE);
         return p_former;
      }
      else
      {
         /* Restore the header and statistics */
         memcpy(&p_former->Lsa, &oldHeader, sizeof(t_S_LsaHeader));
         UpdateStatCounters(p_RTO, p_ARO, p_former, TRUE);
         if (!p_RTO->LsdbOverload)
           EnterLsdbOverload(p_RTO);
      }
   }

   /* Print a warning at a couple threshold crossings */
   lsdbPercentFull = (ulng) ((100 * (long long) p_RTO->ActiveLsas) / OSPF_MAX_LSAS);
   if (lsdbPercentFull >= 95 && lastLsdbPercentFull < 95)
   {
       L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID, 
               "Warning: OSPF LSDB is 95%% full (%ld LSAs).", p_RTO->ActiveLsas);
   } 
   else if (lsdbPercentFull >= 90 && lastLsdbPercentFull < 90)
   {
       L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID, 
               "Warning: OSPF LSDB is 90%% full (%ld LSAs)."
               " OSPFv2 limits the number of Link State Advertisements"
               " (LSAs) that can be stored in the link state database (LSDB).", 
               p_RTO->ActiveLsas);
   }
   lastLsdbPercentFull = lsdbPercentFull;

   /* Allocate memory for this DB entry if there is no former instance */
   if(!p_former)
   {
     if((p_DbEntry = XX_Malloc(sizeof(t_A_DbEntry))) == NULL)
     {
       F_Delete(p_Lsa);
       if (!p_RTO->LsdbOverload)
         EnterLsdbOverload(p_RTO);
       return NULL;
     }
     memset(p_DbEntry, 0, sizeof(t_A_DbEntry));
   }
   else
   {
     /* We will re-use the former entry by overwriting
     ** previously advertised information with the newly advertised information
     */
     p_DbEntry = p_former;

     if (HasAsFloodingScope(p_former->Lsa.LsType))
     {
       /* Could be on retx list for neighbor in any area. */
       t_ARO *anArea;
       er = HL_GetFirst(p_RTO->AroHl, (void*)&anArea);
       while (er == E_OK)
       {
         RemoveFromRetxLists(anArea, p_DbEntry);
         er = HL_GetNext(p_RTO->AroHl, (void*)&anArea, anArea);
       }
     }
     else
     {
       RemoveFromRetxLists(p_ARO, p_DbEntry);
     }

     /* Cleanup frame & connlist information, this will be re-attached based
     ** on the most recent lsa instance
     */
     LsaRelease(p_RTO, p_former, 2);
   }

   /* copy LSA header to db entry */
   memcpy(&p_DbEntry->Lsa, GET_LSA_HDR(p_Lsa), sizeof(t_S_LsaHeader));

   /* Store the body of the LSA */
   lsaBodyLen = F_GetLength(p_Lsa) - sizeof(t_S_LsaHeader);
   if (lsaBodyLen > 0 )
   {
     lsaBody = XX_MallocChunk(lsaBodyLen);
     if (lsaBody == NULL)
     {
       /* if db entry allocated locally, free it. If a former LSA exists, it's 
        * on an LSA hash list (and possibly one or more neighbor retx lists). Let
        * it age normally and get freed when it ages out. */
       if (!p_former)
         XX_Free(p_DbEntry);
       F_Delete(p_Lsa);
       if (!p_RTO->LsdbOverload)
         EnterLsdbOverload(p_RTO);
       return NULL;
     }
   }
   else
   {
     /* router, network, summary, and external LSAs should all have some contents */
     if (p_DbEntry->Lsa.LsType <= S_NSSA_LSA)
     {
       L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
               "OSPFv2 attempted to install a zero length LSA");
       if (!p_former)
         XX_Free(p_DbEntry);
       F_Delete(p_Lsa);
       return NULL;
     }
   }

   /* copy LSA body. assumes LSA in one contiguous buffer */
   if (lsaBodyLen >0)
   {
     memcpy(lsaBody, GET_LSA(p_Lsa), lsaBodyLen);
   }
   p_DbEntry->p_Lsa = lsaBody; 

   /* And release temporary allocated LSA frame */
   F_Delete(p_Lsa);

   p_DbEntry->ARO_Id = (t_Handle) p_ARO;   /* NULL for AS scope LSAs */
   p_DbEntry->OrgTime = TIMER_SysTime();  /* installation time stamp     */
   p_DbEntry->InitAge = (word)A_GET_2B(p_DbEntry->Lsa.LsAge); /* init age */
   L7_BIT_SET(p_DbEntry->dbFlags, IS_CUR);    /* last installed instance flag */

   /* Insert the new LSA into LSA database if this is a new db entry */
   if(!p_former)
     AVLH_Insert(GET_OWNER_HL(p_ARO, p_DbEntry->Lsa.LsType),
              (byte *) &p_DbEntry->Lsa.LsId, p_DbEntry);

   /* Perform LSA preparsing */
   if(p_DbEntry->Lsa.LsType == S_ROUTER_LSA)
   {
     if (A_GET_2B(p_DbEntry->Lsa.LsAge) < MaxAge)
     {
      RtrLsaPreParsing(p_ARO, p_DbEntry, NULL);
     }
     else
     {
       /* router LSA has aged out. Delete its parse list elements. */
       if (p_DbEntry->ConnList) 
         LsaParseDel(p_ARO, p_DbEntry);
     }
   }
   else if(p_DbEntry->Lsa.LsType == S_NETWORK_LSA)
   {
     if (A_GET_2B(p_DbEntry->Lsa.LsAge) < MaxAge)
     {
       NetLsaPreParsing(p_ARO, p_DbEntry, NULL);
     }
     else
     {
       /* network LSA has aged out. Delete its parse list elements. */
       if (p_DbEntry->ConnList) 
         LsaParseDel(p_ARO, p_DbEntry);
     }
   }
   
   if(p_former)
   {
     /* Remove the LSA from the aging table */
     if(AgingTbl_Running(p_RTO->AgingObj))
     {
       AgingTbl_DeleteEntry(p_RTO->AgingObj,
                      p_former, p_former->AgeIndex);
     }
   }

   /* Insert the LSA to the Aging object */
   if(AgingTbl_Running(p_RTO->AgingObj))
   {
     if(p_DbEntry->InitAge == MaxAge)
     {
       /* place the LSA on the overage list if its at maxage */
       err = AgingTbl_AddEntry(p_RTO->AgingObj, p_DbEntry, 
                            MaxAge+1, &p_DbEntry->AgeIndex );
     }
     else
     {
       err = AgingTbl_AddEntry(p_RTO->AgingObj, p_DbEntry, 
                            p_DbEntry->InitAge, &p_DbEntry->AgeIndex );
     }

     if(err != E_OK)
     {
       L7_LOG(L7_LOG_SEVERITY_ALERT, L7_OSPF_MAP_COMPONENT_ID, 
              "\nFailed to add Lsa to AgingObj in LsaInstall()");

       /* Failure to add to the aging table puts us in a sticky place
        * since unless we delete the entry now, it will be lost forever.
        * However the LSA could be on the RTB, SPT so we really
        * shouldn't delete it like this! Needs to be fixed when
        * garbage collection is added to LsaRelease.
        */
       LsaRelease(p_RTO, p_DbEntry, 1);

       if (!p_RTO->LsdbOverload)
         EnterLsdbOverload(p_RTO);

        return NULLP;
     }
   }

   /* Update routes for this LSA */
   if(A_GET_2B(p_DbEntry->Lsa.LsAge) <= MaxAge)
   {
      if((p_DbEntry->Lsa.LsType <= S_AS_EXTERNAL_LSA) || (p_DbEntry->Lsa.LsType == S_NSSA_LSA))
      {
        RTB_LsaProcess(p_RTO, p_ARO, p_DbEntry);
      }
   }

   /* Update Area' LSA counters and all LS checksums */
   UpdateStatCounters(p_RTO, p_ARO, p_DbEntry, TRUE);

   if (p_DbEntry->Lsa.LsType == S_AS_EXTERNAL_LSA)
   {
      /* Check may be router reached overflow state */
      PrevOverflow = p_RTO->IsOverflowed;
      if((PrevOverflow == FALSE) && (p_RTO->Cfg.ExtLsdbLimit != -1) &&
         (p_RTO->NonDefExtLsaCount >= (ulng)((9*p_RTO->Cfg.ExtLsdbLimit)/10)))
      {
         Overflow.RouterId = p_RTO->Cfg.RouterId;
         Overflow.ExtLsdbLimit = p_RTO->Cfg.ExtLsdbLimit;

         if(p_RTO->NonDefExtLsaCount >= (ulng)p_RTO->Cfg.ExtLsdbLimit)
         {
            p_RTO->IsOverflowed = TRUE;
            /* Start overflow state exit timer */
            if(p_RTO->Cfg.ExitOverflowInterval)
               TIMER_StartSec(p_RTO->OverflowTimer, p_RTO->Cfg.ExitOverflowInterval, 0,
                              OverflowTimerExp, p_RTO->OspfSysLabel.threadHndle );

            /* Flush all self-originated non-default AS-Ext LSAs */
            AVLH_Browse(p_RTO->AsExternalLsaHl, (BROWSEFUNC)FlushAsLsa, (ulng)p_RTO);

            /* Flush all corresponding self-originated Type-7 LSAs */
            err = HL_GetFirst(p_RTO->AroHl, (void *)&p_tmpARO);
            while(err == E_OK)
            {
              /* check if NSSA */
              if((p_tmpARO->ExternalRoutingCapability == AREA_IMPORT_NSSA) && 
                 (p_tmpARO->NSSARedistribute == TRUE))
              {
                AVLH_Browse(p_tmpARO->NssaLsaHl, (BROWSEFUNC)FlushLsa, (ulng)p_tmpARO);
              }

              /* Delete all AS-Ext LSAs from NBO Request lists */
              e = HL_GetFirst(p_tmpARO->IfoHl, (void *)&p_IFO);
              while(e == E_OK)
              {
                 er = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
                 while(er == E_OK)
                 {
                    HL_Browse(p_NBO->LsReq, AsExtAndNssaRqFree, 0);
                    er = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
                 }
                 e = HL_GetNext(p_tmpARO->IfoHl, (void *)&p_IFO, p_IFO);
              }

              err = HL_GetNext(p_RTO->AroHl, (void *)&p_tmpARO, p_tmpARO);
            }
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
      }

      if((p_DbEntry->Lsa.LsType == S_AS_EXTERNAL_LSA) &&
         (A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter) != p_RTO->Cfg.RouterId))
      {
         t_A_DbKey askey;
         t_A_DbEntry *asext;
         t_S_AsExternalLsa *p_AsExt, *p_AsExt1;

         p_AsExt1 = (t_S_AsExternalLsa *) p_DbEntry->p_Lsa;

         /* if two	routers, both reachable
         from one another, originate	functionally equivalent
         AS-external-LSAs (i.e., same destination, cost and
         non-zero forwarding	address), then the LSA
         originated by the router having the	highest	OSPF
         Router ID is used.	The router having the lower OSPF
         Router ID can then flush its LSA.*/

         A_SET_4B(p_RTO->Cfg.RouterId, askey.AdvertisingRouter);
         memcpy(askey.LsId, p_DbEntry->Lsa.LsId, 4);
         if(AVLH_Find(p_RTO->AsExternalLsaHl, (byte*) &askey, (void**) &asext,0) == E_OK)
         {
            p_AsExt = (t_S_AsExternalLsa *) asext->p_Lsa;
            if(A_GET_4B(p_AsExt->ForwardingAddress) &&
               (A_GET_4B(p_AsExt->ForwardingAddress) == A_GET_4B(p_AsExt1->ForwardingAddress)) &&
               (A_GET_4B(&p_AsExt->Flags) == A_GET_4B(&p_AsExt1->Flags)))
            {
               if(p_RTO->Cfg.RouterId < A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter))
               {
                 /* Flush the LSA from this area */
                 FlushLsa((t_Handle)asext, (ulng)p_ARO, 0);
               }
            }
         }
      }
   }

   /* If installing a router LSA for another router in an NSSA, rerun 
    * translator election. */
   if ((p_DbEntry->Lsa.LsType == S_ROUTER_LSA) &&
       (p_ARO->ExternalRoutingCapability == AREA_IMPORT_NSSA) &&
       (p_RTO->Cfg.RouterId != A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter)))
   {
       NssaTranslatorElect(p_ARO, 0);
   }

#if L7_OSPF_TE
   if (p_ARO->TEsupport && (p_DbEntry->Lsa.LsType == S_AREA_OPAQUE_LSA) &&
      (p_DbEntry->Lsa.LsId[0] == TE_OPAQUE_LSA))
   {
      byte     *p_Tlv, *p_Tlv1;
      t_TeLink *p_Link;
      t_TeRtr  *p_TeRtr;

      /* Check if it is the Link type */
      p_Tlv = FindTlvInData(p_DbEntry->p_Lsa,
         (word)(A_GET_2B(p_DbEntry->Lsa.Length)-sizeof(t_S_LsaHeader)), TE_TLV_LINK);
      if(p_Tlv)
      {
         /* Find Link Id sub-TLV */
         p_Tlv1 = FindTlvInData(p_Tlv+4, (word)A_GET_2B(p_Tlv+2), TELK_ID);
         if(p_Tlv1)
         {
            /* Add the new Link entry into TE database */
            if ((p_Link = XX_Malloc(sizeof(t_TeLink))) != NULL)
            {
               memcpy(p_Link->RouterId, p_DbEntry->Lsa.AdvertisingRouter, 4);
               p_Link->p_DbEntry = p_DbEntry;
               memcpy(p_Link->LinkId, p_Tlv1+4, 4);

               if(!A_GET_4B(p_Link->LinkId) ||
                  (AVLH_Insert(p_ARO->TeLsaHl, p_Link->RouterId, p_Link) != E_OK))
                  XX_Free(p_Link);
               else
                  TeLinkLsaPreParsing(p_ARO, NULL, p_Link, NULL);
            }
         }
      }
      else
      {
         p_Tlv = FindTlvInData(p_DbEntry->p_Lsa,
            (word)(A_GET_2B(p_DbEntry->Lsa.Length)-sizeof(t_S_LsaHeader)), TE_TLV_ROUTER);
         if(p_Tlv)
         {
           /* Add the new Router entry into TE database */
            if ((p_TeRtr = XX_Malloc(sizeof(t_TeRtr))) != NULL)
            {
               memcpy(p_TeRtr->RouterId, p_DbEntry->Lsa.AdvertisingRouter, 4);
               p_TeRtr->p_DbEntry = p_DbEntry;

               if(AVLH_Insert(p_ARO->TeRtrHl, p_TeRtr->RouterId, p_TeRtr) != E_OK)
                  XX_Free(p_TeRtr);
               else
                  TeRtrLsaPreParsing(p_ARO, p_TeRtr);
            }
         }
      }
   }
#endif

   if (p_ARO)
   {
     /* LSA header part of db entry */
     p_ARO->DataBaseSize += 
       (sizeof(t_A_DbEntry) + A_GET_2B(p_DbEntry->Lsa.Length) - LSA_HEADER_LEN);
   }

   p_RTO->ActiveLsas++;
   if (p_RTO->ActiveLsas > p_RTO->lsaHighWater)
     p_RTO->lsaHighWater = p_RTO->ActiveLsas;

   return p_DbEntry;
}




/*********************************************************************
 * @purpose      Mark an LSDB entry invalid. Delete its LSA. Optionally
 *               remove the entry from the LSDB and free its memory.
 *
 * @param p_RTO     @b{(input)}  OSPF instance
 * @param db        @b{(input)}  LSA DB entry pointer
 * @param flag      @b{(input)}  Hash list delete flag:
 *                    1 - remove entry from the LSDB and free it
 *                    2 - only delete the entry's LSA and mark the entry invalid
 *
 * @param           @b{(output)}  flag
 *
 * @returns         flag value
 *
 * @notes           Don't call this function from a Browse function. If the
 *                  LSA is freed here, the Browse function uses a pointer to
 *                  the freed memory in its avl_remove(). The LSA lists are
 *                  not initialized to free the objects they contain.
 *
 * @end
 * ********************************************************************/
Bool LsaRelease(t_RTO *p_RTO, t_Handle db, ulng flag)
{
   t_A_DbEntry *p_DbEntry = (t_A_DbEntry *) db;
   t_ARO *p_ARO = (t_ARO *)p_DbEntry->ARO_Id;
   e_Err e = E_OK;

   if(!p_ARO && !HasAsFloodingScope(p_DbEntry->Lsa.LsType))
      /* Area obj doesn't exist. Just return.*/
      return ((Bool)flag);

   if(flag==1)
   {
      e = AVLH_Delete(GET_OWNER_HL(p_DbEntry->ARO_Id, p_DbEntry->Lsa.LsType),
                     (byte *)&p_DbEntry->Lsa.LsId, NULL);

      /* Remove the LSA from the aging table */
      if(AgingTbl_Running(p_RTO->AgingObj))
      {
        e |= AgingTbl_DeleteEntry(p_RTO->AgingObj, 
                       p_DbEntry, p_DbEntry->AgeIndex);
      }
   }


#if L7_OSPF_TE
   /* Delete the Link entry from TE database */
   if ((p_DbEntry->Lsa.LsType == S_AREA_OPAQUE_LSA) &&
      (p_DbEntry->Lsa.LsId[0] == TE_OPAQUE_LSA))
   {
      t_TeLink *p_Link;
      t_TeLink key;
      byte     *p_Tlv;

      /* Check if it is the Link type */
      p_Tlv = FindTlvInData(GET_LSA(p_DbEntry->p_Lsa),
         (word)(A_GET_2B(p_DbEntry->Lsa.Length)-sizeof(t_S_LsaHeader)), TE_TLV_LINK);
      if(p_Tlv)
      {
         /* Find Link Id sub-TLV */
         p_Tlv = FindTlvInData(p_Tlv+4, (word)A_GET_2B(p_Tlv+2), TELK_ID);
         if(p_Tlv)
         {
            memcpy(key.RouterId, p_DbEntry->Lsa.AdvertisingRouter, 4);
            memcpy(key.LinkId, p_Tlv+4, 4);
            if(AVLH_Find(p_ARO->TeLsaHl, key.RouterId, (void **)&p_Link,0) == E_OK)
            {
               TeLinkLsaParseDel(p_ARO, p_Link);
               AVLH_Delete(p_ARO->TeLsaHl, key.RouterId, NULL);
            }
         }
      }
      else
      {
         p_Tlv = FindTlvInData(GET_LSA(p_DbEntry->p_Lsa),
            (word)(A_GET_2B(p_DbEntry->Lsa.Length)-sizeof(t_S_LsaHeader)), TE_TLV_ROUTER);
         if(p_Tlv)
         {
            TeRtrLsaParseDel(p_ARO, p_DbEntry);
            AVLH_Delete(p_ARO->TeRtrHl, p_DbEntry->Lsa.AdvertisingRouter, NULL);
         }
      }
   }
#endif

   ASSERT(e == E_OK);

   /* remove this entry from all neighbors retransmission list */
   L7_BIT_CLR(p_DbEntry->dbFlags, IS_CUR);    /* reset installed instance flag */

   /* LSA header part of db entry */
   if (p_ARO)
   {
     p_ARO->DataBaseSize -= 
       (sizeof(t_A_DbEntry) + A_GET_2B(p_DbEntry->Lsa.Length) - LSA_HEADER_LEN);
   }

   /* Release LSA's frame */
   if (p_DbEntry->p_Lsa)
   {
     XX_Free(p_DbEntry->p_Lsa);
     p_DbEntry->p_Lsa = NULL;
   }

   /* Remove LSA preparsing elements */
   if(p_DbEntry->ConnList &&
      ((p_DbEntry->Lsa.LsType == S_ROUTER_LSA) || (p_DbEntry->Lsa.LsType == S_NETWORK_LSA)))
   {
     LsaParseDel(p_ARO, p_DbEntry);
   }

   UpdateStatCounters(p_RTO, p_ARO, p_DbEntry, FALSE);

   /* Free memory for the LSDB entry */
   if (flag == 1)
     XX_Free(p_DbEntry);

   if (p_RTO->ActiveLsas > 0)
	   p_RTO->ActiveLsas--;

   return ((Bool)flag);
}

/*********************************************************************
 * @purpose         Flush Self-Originated LSA.
 *
 *
 * @param  db       @b{(input)}  LSA DB entry pointer
 * @param aroId     @b{(input)}  ARO handler. NULL for LSAs with AS flooding scope.
 * @param noRtrIdChk @b{(input)} if advertising router id check is not required.
 *
 * @returns         TRUE
 *                  FALSE if function is called in error
 *
 * @notes           Assumes only one OSPF instance because of need to 
 *                  get instance pointer. Could pass p_RTO, but too many
 *                  calls to change for now.
 *
 * @end
 * ********************************************************************/
Bool FlushLsa(t_Handle db, ulng aroId, Bool noRtrIdChk)
{
   t_A_DbEntry *p_DbEntry = (t_A_DbEntry *) db;
   t_ARO       *p_ARO = (t_ARO*) aroId;     /* May be NULL */
   t_DelayedLsa *p_DlyLsa = NULLP;
   t_DlyLsaKey   dlykey;
   t_RTO       *p_RTO = (t_RTO *)RTO_List;
   t_Handle delayList = NULL;

   if ((p_DbEntry->Lsa.LsType != S_AS_EXTERNAL_LSA) &&
       (p_DbEntry->Lsa.LsType != S_AS_OPAQUE_LSA) &&
       (p_ARO == NULL))
   {
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
             "FlushLsa() called with NULL area for LSA without AS flooding scope.");
     return FALSE;
   }

   if(! noRtrIdChk)
   {
     /* Flush only self-originated LSAs */
     if(A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter) != p_RTO->Cfg.RouterId)
       return TRUE;
   }

   /* Stop aging of the Lsa and move it to the overaged list */
   if(AgingTbl_Running(p_RTO->AgingObj))
   {
      AgingTbl_DeleteEntry(p_RTO->AgingObj, p_DbEntry, 
                           p_DbEntry->AgeIndex);
      AgingTbl_AddEntry(p_RTO->AgingObj, p_DbEntry, MaxAge + 1,
                        &p_DbEntry->AgeIndex);
   }

   /* If there is the LSA instance is in the delayed list, delete it */
   dlykey.LsaType = p_DbEntry->Lsa.LsType;
   dlykey.LsId    = A_GET_4B(p_DbEntry->Lsa.LsId);
   if ((p_DbEntry->Lsa.LsType == S_AS_EXTERNAL_LSA) ||
       (p_DbEntry->Lsa.LsType == S_AS_OPAQUE_LSA))
   {
     delayList = p_RTO->DelayedLsa;
   }
   else
   {
     delayList = p_ARO->DelayedLsa;
   }
   if (delayList && 
       (HL_FindFirst(delayList, (byte*)&dlykey, (void *)&p_DlyLsa) == E_OK))
   {
     if (p_DlyLsa && p_DlyLsa->p_F)
     {
       F_Delete(p_DlyLsa->p_F);
     }
     HL_Delete(delayList, (byte*)&dlykey, p_DlyLsa);
   }

   /* Flush the LSA from this area */
   A_SET_2B(MaxAge, p_DbEntry->Lsa.LsAge);
   LSA_FLOODING(p_RTO, p_ARO, NULL, p_DbEntry, NULL);
   LsaAgedSendTrap(p_RTO, p_ARO, p_DbEntry);

   return TRUE;
}

/*--------------------------------------------------------------------
 * ROUTINE:    LsUpdatesSend
 *
 * DESCRIPTION:
 *    Check every OSPF interface to see if an LS Update is waiting to
 *    be sent. In LsaFlooding(), LS Updates may be constructed but not
 *    sent, so that a function that calls LsaFlooding() can bundle
 *    many LSAs into one LS Update. This function is the trigger to 
 *    send those LS Updates. Resets the BundleLsas flag.
 *
 * ARGUMENTS:
 *    p_RTO   OSPF Instance
 *
 * RESULTS:
 *    E_OK - 
 *--------------------------------------------------------------------*/
e_Err LsUpdatesSend(t_RTO *p_RTO)
{
    e_Err err, e;
    t_IFO *p_IFO = NULL;
    t_ARO *p_ARO = NULL;

    err = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
    while(err == E_OK)
    {
      e = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
      while(e == E_OK)
      {
        IFO_SendPendingUpdate(p_IFO);
        e = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
      }
      err = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
    }

    p_RTO->BundleLsas = FALSE;
    return E_OK;
}

/*********************************************************************
 * @purpose             Flooding LSA to neighbors.
 *
 *
 * @param  p_ARO         @b{(input)}  flooding Area Object handler
 * @param p_SourceNBO    @b{(input)}  source NBO the LSA was received
 *                                   from or NULL if it is local.
 * @param  p_DbEntry     @b{(input)}  LSA related Databse entry to flood it
 * @param p_FloodedBack  @b{(input)}  flooded back flag returned here if
 *                                   this is response to received LSA,
 *                                   otherwise NULL
 *
 * @returns             E_OK - flooded succssful,
 * @returns             E_FAILED - flooding failed

 *--------------------------------------------------------------------*/
e_Err LsaFlooding(t_ARO *p_ARO, t_NBO *p_SourceNBO,
                  t_A_DbEntry *p_DbEntry, Bool *p_FloodedBack)
{
   e_Err e,er;
   t_RTO *p_RTO = (t_RTO *) p_ARO->RTO_Id;
   t_IFO *p_IFO;
   t_NBO *p_NBO;
   t_RetxEntry *p_RetxEntry;
   e_DbInstComp cmpres;
   t_LsReqEntry *p_LsReq;
   Bool IsFlooded;
   t_S_LsaHeader *p_LsaHdr;
   void *p_F;
   word numLsas = 0;
   ulng lsaLen = 0, ipPktLen, allocPktLen, maxUpdatePktLen;
   word authLen = 0;

   if(!p_RTO->Cfg.AdminStat || !p_ARO->OperationState)
      return E_FAILED;

   /* update LSA age before processing 
    * if nothing else, its needed to ensure that the correct age is logged.
    */
   UpdateAgeOfDbEntry(p_DbEntry);

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_LS_UPDATE)
   {
       char traceBuf[OSPF_MAX_TRACE_STR_LEN];
       L7_uchar8 lsIdStr[OSAPI_INET_NTOA_BUF_SIZE];
       L7_uchar8 advRouterStr[OSAPI_INET_NTOA_BUF_SIZE];
       L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
       osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.LsId), lsIdStr);
       osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter), advRouterStr);
       osapiInetNtoa(p_ARO->AreaId, areaIdStr);
       sprintf(traceBuf, "Flooding OSPF %s in area %s. LSID: %s, Adv Router: %s, Age: %d, SeqNo: 0x%lx",
               lsaTypeNames[p_DbEntry->Lsa.LsType], areaIdStr, lsIdStr, advRouterStr, 
               A_GET_2B(p_DbEntry->Lsa.LsAge), A_GET_4B(p_DbEntry->Lsa.SeqNum));
       RTO_TraceWrite(traceBuf);    
   } 

   /* Check Area Opaque LSAs scope */
   if((p_DbEntry->Lsa.LsType == S_AREA_OPAQUE_LSA) &&
      (p_DbEntry->ARO_Id != p_ARO))
      return E_OK;

   /* Find all eligible interfaces attached to this Area links (IFOs) */
   e = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
   while(e == E_OK)
   {
      /* Don't flood T5 or AS Opaque LSAs over virtual links or in stubs or NSSAs. */
      if( ((p_IFO->Cfg.Type == IFO_VRTL) ||
          (p_ARO->ExternalRoutingCapability != AREA_IMPORT_EXTERNAL)) &&
          ((p_DbEntry->Lsa.LsType == S_AS_EXTERNAL_LSA) || 
           (p_DbEntry->Lsa.LsType == S_AS_OPAQUE_LSA)))
         goto next_interface;

      /* Enforce no-summary option on stubs and NSSAs */
      if( (p_ARO->ExternalRoutingCapability != AREA_IMPORT_EXTERNAL) &&
          (p_ARO->ImportSummaries == FALSE) &&
          (((p_DbEntry->Lsa.LsType == S_IPNET_SUMMARY_LSA) ||
           (p_DbEntry->Lsa.LsType == S_ASBR_SUMMARY_LSA)) && (A_GET_4B(p_DbEntry->Lsa.LsId)!=0)))
         goto next_interface;

      /* Don't send T4 LSAs into NSSAs. */
      if( (p_ARO->ExternalRoutingCapability == AREA_IMPORT_NSSA) &&
          (p_DbEntry->Lsa.LsType == S_ASBR_SUMMARY_LSA))
         goto next_interface;

      /* Check Link Opaque LSAs scope */
      if((p_DbEntry->Lsa.LsType == S_LINK_OPAQUE_LSA) &&
         (p_DbEntry->IFO_Id != p_IFO))
         goto next_interface;

      /* If flooding disabled */
      if(p_IFO->Cfg.DisableFlooding)
         goto next_interface;

      /* For each eligible IFO: */
      IsFlooded = FALSE;

      maxUpdatePktLen = GetMaxOspfBufLength(p_IFO, S_LS_UPDATE);

      /* (1) Examine all neighbors attached to this interface */
      er = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
      while(er == E_OK)
      {
         /* (a) check NBO state */
         if(p_NBO->State < NBO_EXCHANGE)
            goto  next_neighbor;

         /* Do not flood Group Member Lsas to the Neighbors which
            does not support multicasting */
         if((p_DbEntry->Lsa.LsType == S_GROUP_MEMBER_LSA) &&
            ((p_NBO->Options & OSPF_OPT_MC_BIT) == 0))
            goto  next_neighbor;

         /* Do not flood Opaque Lsas to the Neighbors which
            does not support Opaque feature */
         if((p_DbEntry->Lsa.LsType >= S_LINK_OPAQUE_LSA) &&
            (p_DbEntry->Lsa.LsType <= S_AS_OPAQUE_LSA) &&
            ((p_NBO->Options & OSPF_OPT_O_BIT) == 0))
            goto  next_neighbor;

         /* (b) if NBO state still not FULL and
                there is an same LSA instance in LsReq list */
         if((p_NBO->State < NBO_FULL) &&
            (HL_FindFirst(p_NBO->LsReq, &p_DbEntry->Lsa.LsType, (void *)&p_LsReq) == E_OK))
         {
            cmpres = LsaCompare(&p_DbEntry->Lsa, &p_LsReq->Lsa);

            if(cmpres == LSA_LESS_RECENT)
               goto  next_neighbor;
            else if(cmpres == LSA_EQUAL)
            {
               /* delete the LSA from LsReq list */
               HL_Delete(p_NBO->LsReq, &p_LsReq->Lsa.LsType, p_LsReq);
               goto  next_neighbor;
            }
            else if(cmpres == LSA_MORE_RECENT)
            {
               /* delete the LSA from LsReq list */
               HL_Delete(p_NBO->LsReq, &p_LsReq->Lsa.LsType, p_LsReq);
            }
         }

         /* (c) if the new LSA was recieved from this neighbor */
                if(p_NBO == p_SourceNBO)
                    goto  next_neighbor;

         /* (d) Flood this LSA to this NBO */

         /* If Retx List still has this entry, then delete this entry first */
         if(HL_FindFirst(p_NBO->LsReTx, &p_DbEntry->Lsa.LsType, (void**)&p_RetxEntry) != E_OK)
         {
            /* Allocate the new Retx entry and insert it into Retx list */
            if((p_RetxEntry = RTO_RetxEntryAlloc(p_RTO)) == NULL)
               return E_NOMEMORY;
            p_RetxEntry->DbEntry = p_DbEntry;
            memcpy(&p_RetxEntry->LsType, &p_DbEntry->Lsa.LsType, 9);
            p_RetxEntry->SentTime = TIMER_SysTime();
            NBO_LsReTxInsert(p_NBO, &p_RetxEntry->LsType, p_RetxEntry);
         }
         else
         {
            p_RetxEntry->DbEntry = p_DbEntry;
            if(p_SourceNBO && (p_SourceNBO->IFO_Id == (t_Handle) p_IFO) &&
               (p_SourceNBO->RouterId != p_IFO->DrId) &&
               (p_SourceNBO->RouterId != p_IFO->BackupId) &&
               (p_IFO->BackupId != p_RTO->Cfg.RouterId))
            {
              NBO_LsReTxUpdateSentTime(p_NBO, p_RetxEntry);
            }
         }

         IsFlooded = TRUE;

next_neighbor:
         er = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
      }

      /* (2) if in previous step LSA was not added to any of LsReTx lists */
      if(!IsFlooded)
         goto  next_interface;

      /* (3-4) if LSA was recieved on this interface */
      if(p_SourceNBO && (p_SourceNBO->IFO_Id == (t_Handle) p_IFO))
      {
         /* (3) if LSA was recieved from DR or Backup examine the next inf*/
         if((p_SourceNBO->RouterId == p_IFO->DrId) ||
            (p_SourceNBO->RouterId == p_IFO->BackupId))
            goto  next_interface;

         /* (4) if the interface state is Backup examine the next inf*/
         if(p_IFO->Cfg.State == IFO_BACKUP)
            goto  next_interface;

         /* Update p_FloodedBack flag */
         if(p_FloodedBack)
            *p_FloodedBack = TRUE;
      }

      /* (5) flood the LSA out this interface */

      lsaLen = A_GET_2B(p_DbEntry->Lsa.Length);

      /* 4 is for the # LSAs field in the LS Update. */
      if (lsaLen > (maxUpdatePktLen - (IP_HDR_LEN + OSPF_MD5_AUTH_DATA_LEN + 
                                       sizeof(t_S_PckHeader) + 4)))
      {
          /* Error. Single LSA doesn't fit into an LS Update. */
          return E_FAILED;
      }

      /* Allocate the LS Update packet. If LSAs are to be bundled, get 
       * the pending update on the interface, if one exists. */
      p_F = NULL;
      if (p_RTO->BundleLsas)
      {
          p_F = p_IFO->PendingUpdate;
      }

      if (p_F == NULL) 
      {
          /* no pending LS Update on this interface */
          if ((p_F = IFO_AllocPacket(p_IFO, S_LS_UPDATE,
                                     0, &allocPktLen)) == NULL)
          {
              return E_FAILED;
          }
          /* Initialize # LSAs to 0 */
          A_SET_4B(0, GET_LSA_HDR(p_F));
          B_SetLength(F_GetFirstBuf(p_F), 4);
          F_GetLength(p_F) = 4;
      }

      numLsas = A_GET_4B(GET_LSA_HDR(p_F));

      if (p_IFO->Cfg.AuType == IFO_CRYPT)
        authLen = OSPF_MD5_AUTH_DATA_LEN;
      else
        authLen = 0;

      ipPktLen = (IP_HDR_LEN + authLen + sizeof(t_S_PckHeader) +
                  F_GetLength(p_F) + lsaLen);

      /* When we build UPDATE packets, we should try to avoid fragmentation.
       * So even though we allocate a buffer larger than the IP MTU, we limit
       * the size of updates to the IP MTU, unless a single LSA is larger
       * than the IP MTU. Then we let the stack fragment. */
      if ((numLsas >= 1) && ((ipPktLen >= p_IFO->Cfg.MaxIpMTUsize) ||
                             (ipPktLen >= maxUpdatePktLen)))
      {
          /* If the Update does not have room for the LSA, send the 
           * Update and allocate a new one. */
          IFO_LsUpdateTx(p_IFO, p_F);
          p_F = NULL;
          p_IFO->PendingUpdate = NULL;

          /* Allocate a new LS Update */
          if ((p_F = IFO_AllocPacket(p_IFO, S_LS_UPDATE,
                                     0, &allocPktLen)) == NULL)
          {
              return E_FAILED;
          }
          A_SET_4B(0, GET_LSA_HDR(p_F));       /* Init # LSAs to 0 */
          B_SetLength(F_GetFirstBuf(p_F), 4);
          F_GetLength(p_F) = 4;
      }

      /* Increment number of LSAs in LS Update packet */   
      numLsas = A_GET_4B(GET_LSA_HDR(p_F));
      A_SET_4B(numLsas + 1, GET_LSA_HDR(p_F));

      /* Get pointer to position in LS Update where next LSA is to be written. */
      p_LsaHdr = (t_S_LsaHeader *) (GET_LSA_HDR(p_F) + F_GetLength(p_F));

      /* Copy LSA header into LS Update packet */
      memcpy(p_LsaHdr, &p_DbEntry->Lsa, LSA_HEADER_LEN);

      /* Copy LSA body into LS Update packet */
      if (p_DbEntry->p_Lsa)
      {
        memcpy(p_LsaHdr + 1, p_DbEntry->p_Lsa, lsaLen - LSA_HEADER_LEN);
      }

      /* Copy the age of the lsa from the Lsa entry in
         the p_DbEntry. This is necessary since the calling
         function may not have correctly updated the p_DbEntry
         p_Lsa though it always updates the Lsa in the p_DbEntry */
      memcpy(p_LsaHdr->LsAge, p_DbEntry->Lsa.LsAge, 2*sizeof(byte));


      /* Update packet length */
      B_SetLength(F_GetFirstBuf(p_F), F_GetLength(p_F) + lsaLen);
      F_GetLength(p_F) = F_GetLength(p_F) + lsaLen;

      /* Update LSA age within the packet */
      A_SET_2B((p_IFO->Cfg.InfTransDelay + A_GET_2B(p_DbEntry->Lsa.LsAge)),
         p_LsaHdr->LsAge);
      if(A_GET_2B(p_LsaHdr->LsAge) > MaxAge)
         A_SET_2B(MaxAge, p_LsaHdr->LsAge);

      /* If bundling LSAs, defer transmission so that more LSAs can be added
       * to this LS Update. */
      if (p_RTO->BundleLsas)
      {
          p_IFO->PendingUpdate = p_F;
      }
      else
      {
        IFO_LsUpdateTx(p_IFO, p_F);
        p_F = NULL;
      }

      /* goto next IFO */
next_interface:
      e = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
   }

   /* For aged network and router, LSAs recalculate the routing table. We use
    * our own network and router LSAs in our SPF. When we flush a self-originated
    * network or router LSA, we set its age to 3600 but don't call LsaInstall(),
    * so the SPF trigger there doesn't get hit. So trigger here. Definitely 
    * don't want to trigger an SPF for external LSAs here, since we'd trigger once
    * for each area!  */
   p_LsaHdr = (t_S_LsaHeader*) &p_DbEntry->Lsa;
   if ((A_GET_4B(p_LsaHdr->AdvertisingRouter) == p_RTO->Cfg.RouterId) &&
       ((p_DbEntry->Lsa.LsType == S_ROUTER_LSA) || 
        (p_DbEntry->Lsa.LsType == S_NETWORK_LSA)) &&
       (A_GET_2B(p_DbEntry->Lsa.LsAge) >= MaxAge))
   {
     t_VpnCos VpnCos = OSPF_PUBLIC_VPN_ID;
     t_RTB *p_RTB = NULL;
      /* Remove LSA preparsing elements */
      if (p_DbEntry->ConnList)
         LsaParseDel(p_ARO, p_DbEntry);

      if (HL_FindFirst(p_RTO->RtbHl, (byte *) &VpnCos, (void *)&p_RTB) == E_OK)
      {
        if(!TIMER_Active(p_RTB->RecalcTimer))
          /* Start recalculation timer */
          TIMER_StartSec(p_RTB->RecalcTimer, p_RTB->CalcDelay,
                         0, RecalcTimerExp, p_RTO->OspfRtbThread.threadHndle);
      }
   }
     
#if L7_MOSPF
      if ((A_GET_2B(p_DbEntry->Lsa.LsAge) >= MaxAge) &&
          (p_DbEntry->Lsa.LsType == S_GROUP_MEMBER_LSA))
      {
         t_VpnCos VpnCos = 0;
         t_RTB    *p_RTB;

         /* find the match RTB */
         if(p_DbEntry->Lsa.Options & VPN_OPTION_BIT)
            GetVpn(p_ARO, S_GROUP_MEMBER_LSA, p_DbEntry->p_Lsa, &VpnCos);

         /* clear the Multicast Forwarding Cache */
         if(HL_FindFirst(p_RTO->RtbHl, (byte *) &VpnCos, (void *)&p_RTB) == E_OK)
            McForwCacheDestruct(p_RTB, A_GET_4B(p_DbEntry->Lsa.LsId), 0, 0);
      }
#endif

   return E_OK;
}




/*********************************************************************
 * @purpose       AsExtLsaFlooding
 *
 *
 * @param     p_RTO        @b{(input)} OSPF instance
 * @param     p_SourceNBO  @b{(input)} source NBO the LSA was received from or
 *                            NULL if it is local.
 * @param     p_DbEntry    @b{(input)} database entry for LSA being flooded
 * @param     p_FloodedBack  @b{(input)} flooded back flag returned here if this
 *                             is response to received LSA, otherwise NULL
 *
 * @returns       E_OK - flooded succssful,
 * @returns       E_FAILED - flooding failed
 *
 * @notes
 *
 * @end
 * *******************************************************************/
e_Err AsExtLsaFlooding(t_RTO *p_RTO, t_NBO *p_SourceNBO,
                       t_A_DbEntry *p_DbEntry, Bool *p_FloodedBack)
{
   t_ARO *p_Aro;
   e_Err err;

   if (!p_RTO->Cfg.AdminStat)
      return E_FAILED;

   /* For AS_EXTERNAL_LSA flood it to all exist areas, except stubs & NSSAs */
   err = HL_GetFirst(p_RTO->AroHl, (void *)&p_Aro);
   while(err == E_OK)
   {
     LsaFlooding(p_Aro, p_SourceNBO, p_DbEntry, p_FloodedBack);
     err = HL_GetNext(p_RTO->AroHl, (void *)&p_Aro, p_Aro);
   }
   return E_OK;
}



/*********************************************************************
 * @purpose        Compare two LSAs.
 *
 *
 * @param lsa1     @b{(input)} first LSA's header pointer
 * @param lsa2     @b{(input)}  second LSA's header pointer
 *
 * @returns        LSA_EQUAL
 * @returns        LSA_LESS_RECENT
 * @returns        LSA_MORE_RECENT
 *
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_DbInstComp LsaCompare(t_S_LsaHeader *lsa1, t_S_LsaHeader *lsa2)
{
   long seqnum1, seqnum2;
   word diff;
   byte checksum1[2], checksum2[2];

   seqnum1 = A_GET_4B(lsa1->SeqNum);
   seqnum2 = A_GET_4B(lsa2->SeqNum);

   if(seqnum1 > seqnum2)
      return LSA_MORE_RECENT;
   else if(seqnum1 < seqnum2)
      return LSA_LESS_RECENT;

   /* since the fletcher algorithm cant differentiate between 0x00 and 0xff
    * disregard 0xff while comparing checksums
    */
   A_SET_2B(A_GET_2B(lsa1->CheckSum), checksum1);
   A_SET_2B(A_GET_2B(lsa2->CheckSum), checksum2);

   checksum1[0] = (checksum1[0] == 0xff) ? 0 : checksum1[0];
   checksum1[1] = (checksum1[1] == 0xff) ? 0 : checksum1[1];
   checksum2[0] = (checksum2[0] == 0xff) ? 0 : checksum2[0];
   checksum2[1] = (checksum2[1] == 0xff) ? 0 : checksum2[1];

   if(A_GET_2B(checksum1) > A_GET_2B(checksum2))
      return LSA_MORE_RECENT;
   else if(A_GET_2B(checksum1) < A_GET_2B(checksum2))
      return LSA_LESS_RECENT;

   if((A_GET_2B(lsa1->LsAge) == MaxAge) && (A_GET_2B(lsa2->LsAge) < MaxAge))
      return LSA_MORE_RECENT;
   else if((A_GET_2B(lsa2->LsAge) == MaxAge) && (A_GET_2B(lsa1->LsAge) < MaxAge))
      return LSA_LESS_RECENT;

   diff = (A_GET_2B(lsa1->LsAge) >= A_GET_2B(lsa2->LsAge)) ?
          (A_GET_2B(lsa1->LsAge) - A_GET_2B(lsa2->LsAge)) :
          (A_GET_2B(lsa2->LsAge) - A_GET_2B(lsa1->LsAge));

   if(diff >= MaxAgeDiff)
   {
      if(A_GET_2B(lsa1->LsAge) < A_GET_2B(lsa2->LsAge))
         return LSA_MORE_RECENT;
      else
         return LSA_LESS_RECENT;
   }

   return LSA_EQUAL;
}

/*********************************************************************
 * @purpose           Compare content of two LSAs having the same
 *                    LSA type, LsId and Adv Router.
 *                    Two LSAs may have the same contents when,
 *                    for example, LSA refresh is received
 *
 * @param p_LsaNew     @b{(input)}  new copy of LSA, as a t_Frame
 * @param p_LsaOld     @b{(input)}  old copy of LSA, represented by db entry
 *
 * @returns        TRUE - the content of the LSAs differ
 *                 FALSE - the content of the LSAs are the same
 * @notes
 *
 * @end
 * ********************************************************************/
static Bool LsaContentDiff (void *p_LsaNew, t_A_DbEntry *p_LsaOld)
{
   t_S_LsaHeader *oldLsa, *newLsa;
   word newAge, oldAge;
   word lsaLength;           /* length of LSA, not including LSA header */

   ASSERT(p_LsaNew); /* new LSA cannot be NULLP */

   if(!p_LsaOld)
      return TRUE;

   oldLsa = (t_S_LsaHeader *) &p_LsaOld->Lsa;
   newLsa = (t_S_LsaHeader *)GET_LSA_HDR(p_LsaNew);

   /* Check age - if the ages are different and one of them is MaxAge */
   /* it means that the LSAs are different */
   if((newAge = A_GET_2B(newLsa->LsAge)) > MaxAge)
      newAge = MaxAge;

   if((oldAge = A_GET_2B(oldLsa->LsAge)) > MaxAge)
      oldAge = MaxAge;

   if ((newAge != oldAge) &&
       ((newAge == MaxAge) || (oldAge == MaxAge)))
      return TRUE;

   if (oldLsa->Options != newLsa->Options)
      return TRUE;

   if(memcmp(oldLsa->Length, newLsa->Length, 2) != 0)
      return TRUE;

   lsaLength = A_GET_2B(oldLsa->Length) - LSA_HEADER_LEN;
   
    if (lsaLength == 0)
    {
      return TRUE;
    }

   /* compare the body of the LSAs */
   /* Not sure we are guaranteed that all the LSA data in the frame
    * is in one contiguous buffer. Not catastrophic if not. */
   if(memcmp((p_LsaOld->p_Lsa),
             ((byte *)newLsa + LSA_HEADER_LEN),
             lsaLength) != 0 )
      return TRUE;

   /* if we get here there is no difference in the contents of the LSAs */
   return FALSE;
}

/*********************************************************************
 * @purpose           Send LSA Agged to MaxAge trap event to the upper
 *                    application.
 *
 * @param p_RTO        @b{(input)}  OSPF instance
 * @param p_ARO        @b{(input)}  ARO object id. NULL for T5 LSAs.
 * @param p_DbEntry    @b{(input)}  agged LSA entry
 *
 * @param              @b{(output)}   n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void LsaAgedSendTrap(t_RTO *p_RTO, t_ARO *p_ARO, t_A_DbEntry *p_DbEntry)
{
   t_OriginateLsa Lsa;

   if(!p_RTO->Clbk.p_OspfTrap ||
      !(p_RTO->TrapControl & GET_TRAP_BITMASK(MAX_AGE_LSA)))
      return;

   Lsa.RouterId =p_RTO->Cfg.RouterId; /* The originator of the trap */
   Lsa.LsdbAreaId = HasAsFloodingScope(p_DbEntry->Lsa.LsType) ?
                    OSPF_BACKBONE_AREA_ID : p_ARO->AreaId;  /* 0.0.0.0 for AS Externals */
   Lsa.LsdbType = p_DbEntry->Lsa.LsType;
   Lsa.LsdbLsid = A_GET_4B(p_DbEntry->Lsa.LsId);
   Lsa.LsdbRouterId = A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter);
   Lsa.LsdbSeqNum = A_GET_4B(p_DbEntry->Lsa.SeqNum);

   p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, MAX_AGE_LSA, (u_OspfTrap *)&Lsa);
}

/*********************************************************************
 * @purpose  Compute Fletcher CheckSum for LSA header field (RFC 905,
 *                                                            ANNEX B).
 *
 *
 * @param  buffer    @b{(input)}  begin of data
 * @param  len       @b{(input)}  PDU length
 * @param  offset    @b{(input)}  position of the first octet of the
 *                                checksum parameter
 *
 * @param    c0,c1   @b{(output)}  checksum value
 *
 *
 * @notes
 *           This function computes and sets the checksum (FLETCHER algorithm).
 *           If "offset" is NULL, then the checksum will be calculated but
 *           the checksum bytes will not be set. If "offset" is non-NULL,
 *           the checksum bytes at "offset" will be set such that the checksum
 *           accumulations, when recalculated, will both be zero.
 *
 * @end
 * ********************************************************************/
word FletcherCheckSum(byte *buffer, int len, int offset)
{
   word c0, c1;
   int cs;
   long cl0, cl1;
   byte *hpp, *pls;

   if(offset)
   {
      buffer[offset] = 0;
      buffer[offset+1] = 0;
   }

   pls = buffer + len;
   c0 = c1 = 0;
   hpp = buffer;

   while(hpp < pls)
   {
      if((c0 += *hpp++) > 254)
         c0 -= 255;
      if((c1 += c0) > 254)
         c1 -= 255;
   }

   if(offset)
   {
      long ctmp = len - offset - 1 ;

      cl0 = c0;
      cl1 = c1;

      if((cs =((ctmp * cl0) - cl1) % 255L) < 0)
         cs += 255;

      buffer[offset] = cs;

      if((cs =(cl1 -((ctmp + 1L) * cl0)) % 255L) < 0)
         cs += 255;

      buffer[offset+1] = cs;
   }

   return(c0 | c1);
}

/*********************************************************************
 * @purpose         Generate All types of Opaque LSA
 *
 *
 * @param Id        @b{(input)}  Router/Area/Interface Object Id
 * @param VpnCos    @b{(input)}  9/10/11 Opaque LSA type
 * @param OpqType   @b{(input)}  VPN/COS
 * @param OpqId     @b{(input)}  Opaque User type (128-255)
 * @param  p_Dat    @b{(input)}   Opaque LSA payload pointer
 * @param DataLen   @b{(input)}  payload length
 *
 * @returns E_OK    -if successfull, otherwise
 * @returns E_FAILED
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err OpaqueGenerate(t_Handle Id, byte LsType, t_VpnCos VpnCos, byte OpqType,
                      ulng OpqId, byte *p_Data, word DataLen)
{
   t_ARO         *p_ARO;
   t_RTO         *p_RTO;
   t_IFO         *p_IFO;
   void          *p_F;
   t_A_DbKey      dbkey;
   t_A_DbEntry   *p_DbEntry;
   SP_IPADR      backbone = OSPF_BACKBONE_AREA_ID;
   byte          *data;

   switch(LsType)
   {
      case  S_LINK_OPAQUE_LSA:
         p_IFO = (t_IFO *) Id;
         p_ARO = (t_ARO *) p_IFO->ARO_Id;
         p_RTO = (t_RTO *) p_ARO->RTO_Id;
      break;
      case  S_AREA_OPAQUE_LSA:
         p_ARO = (t_ARO *) Id;
         p_RTO = (t_RTO *) p_ARO->RTO_Id;
      break;
      case  S_AS_OPAQUE_LSA:
         p_RTO = (t_RTO *) Id;

         if(p_RTO->IsOverflowed)
            return E_FAILED;

         /* Select some ARO */
         if(HL_FindFirst(p_RTO->AroHl, (byte *) &backbone, (void *)&p_ARO) != E_OK)
            if(HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO) != E_OK)
               return E_FAILED;

      break;
      default:
         return E_FAILED;
   }

   if(!p_RTO->Cfg.AdminStat || !p_ARO->OperationState)
      return E_FAILED;

   /* Allocate LSA with sizeof(t_S_LsaHeader) offset */
   if((p_F = LSA_Alloc(p_ARO, LsType)) == NULL)
      return E_FAILED;

   data = GET_LSA_HDR(p_F);
   memcpy(data, p_Data, DataLen);
   if(VpnCos)
      A_SET_4B(VpnCos & 0x3FFFFFFFL, data+DataLen);

   /* Set LSA length */
   B_SetLength(F_GetFirstBuf(p_F), DataLen + (VpnCos?4:0));
   F_GetLength(p_F) = (DataLen + (VpnCos?4:0));

   /* Go to the common part of all LSA types origination procedure */
   LsaOriginate(p_RTO, p_ARO, p_F, LsType,
      ((ulng)OpqType << 24) | (OpqId & 0x00FFFFFFL), 0, FALSE,
      (byte)((VpnCos ? VPN_OPTION_BIT : 0) |
      (p_RTO->Cfg.MulticastExtensions ? OSPF_OPT_MC_BIT : 0) |
      OSPF_OPT_O_BIT));

   /* Mark the Link Opaque LSA with IFO */
   if(LsType == S_LINK_OPAQUE_LSA)
   {
      /* Found the matching LSA */
      A_SET_4B(p_RTO->Cfg.RouterId, dbkey.AdvertisingRouter);
      A_SET_4B(((ulng)OpqType << 24) | (OpqId & 0x00FFFFFFL), dbkey.LsId);
      if((AVLH_Find(p_ARO->LinkOpaqueLsaHl, (byte*) &dbkey, (void *)&p_DbEntry,0) == E_OK) &&
         (A_GET_2B(p_DbEntry->Lsa.LsAge) < MaxAge))
      {
         p_DbEntry->IFO_Id = (t_IFO *)Id;
         ((t_IFO*)p_DbEntry->IFO_Id)->Cfg.LinkLsaCount += 1;
         ((t_IFO*)p_DbEntry->IFO_Id)->Cfg.LinkLsaCksum += A_GET_2B(p_DbEntry->Lsa.CheckSum);
      }
   }

   return E_OK;
}




/*********************************************************************
 * @purpose           Finds the Summary or AS_EXT LSA .
 *
 *
 * @param t_RTO        @b{(input)}  
 * @param p_ARO        @b{(input)}  handle of the area object
 * @param DestAddr     @b{(input)}  IP address of the network
 * @param NetMask      @b{(input)}  network mask
 *
 * @param         @b{(output)}  p_foundLsa - in successful case contains
 *                              pointer to the LSA describing the specified
 *                              network,in unsuccessful case contains pointer
 *                              to the LSA desribing other network with the
 *                              same IP address,  yet different net mask.
 *
 * @param         @b{(output)}  p_LsId     - in successful case contains
 *                              pointer to the LSA ID of the found LSA.
 *                              It may be differ from DestAddr,
 *                              because it may have "host" bit in successful
 *                              case contains 0.
 *
 * @returns  TRUE  - LSA for the destination has been found successfully
 * @returns  FALSE - otherwise
 *
 * @notes
 *             Finds the Summary or AS_EXT LSA for the specifyed
 *             network address and mask that was originated by
 *             the local router.according to Appendix E of RFC 2328
 *
 * @end
 * ********************************************************************/
e_Err FindSummaryOrASExtLsa(t_RTO *p_RTO, t_ARO *p_ARO, e_S_LScodes LsType,
                            SP_IPADR DestAddr, SP_IPADR NetMask,
                            t_A_DbEntry **p_foundLsa, SP_IPADR *p_LsId)
{
   t_A_DbEntry *p_DbEntry = NULLP;
   t_A_DbKey key;
   e_Err    err;
   t_Handle lsaHashList = GET_OWNER_HL(p_ARO, LsType);

   if (lsaHashList == NULL)
       /* This can happen during an RTO_Delete(). Simply means the LSA
        * for a route has already been deleted. */
       return E_FAILED;

   if((LsType != S_IPNET_SUMMARY_LSA) && (LsType != S_AS_EXTERNAL_LSA))
      return E_FAILED;

   /* Init the returning parameters */
   if(p_foundLsa)
      *p_foundLsa = NULLP;
   if(p_LsId)
      *p_LsId = 0;

   /* Find the instance of LSA according to the specified destination */
   A_SET_4B(DestAddr, key.LsId);
   A_SET_4B(p_RTO->Cfg.RouterId, key.AdvertisingRouter);
   err = AVLH_Find(lsaHashList, (byte *)&key, (void *)&p_DbEntry,0);

   /* Keep the found entry (it may be "our" LSA or LSA of other network */
   /* with the same network address and different mask).                */
   if(err == E_OK)
   {
      if(p_foundLsa)
         *p_foundLsa = p_DbEntry;
      if(p_LsId)
         *p_LsId = A_GET_4B(p_DbEntry->Lsa.LsId);

      /* Check if the netmasks are equal. Since mask is first field in T3, T4, and T5
       * LSAs, safe to cast this to a summary LSA here. */
      if(NetMask == A_GET_4B(p_DbEntry->p_Lsa))
        return E_OK;
      else
        err = E_NOT_FOUND;
   }

   /* For Summary and AS_EXT Lsa additional checking is needed           */
   /*  - If Db entry with the specified LsId has not been found above    */
   /*    or Db entry has been found but the found netmask is not equal   */
   /*    with NetMask, we need to add "Host" bits to LsId and try again. */
   /*  - Else the found LSA really describes the specified dest.         */

   DestAddr |= ~NetMask;
   A_SET_4B(DestAddr, key.LsId);
   p_DbEntry = NULL;
   err = AVLH_Find(lsaHashList, (byte *)&key, (void *)&p_DbEntry,0);

   if((err == E_OK) && (NetMask == A_GET_4B(p_DbEntry->p_Lsa)))
   {
      if(p_foundLsa)
         *p_foundLsa = p_DbEntry;
      if(p_LsId)
         *p_LsId = A_GET_4B(p_DbEntry->Lsa.LsId);
         return E_OK;
   }

   return E_NOT_FOUND;
}


/* This procedure generates LsId for Summary_Lsa and AS_EXT LSA  */
/* as it is described in Appendix E in RFC 2328.                 */
/* p_ARO is NULL if LsType is S_AS_EXTERNAL_LSA. */
e_Err GetLsIdForSummOrASExtLsa(t_RTO *p_RTO, t_ARO *p_ARO, e_S_LScodes LsType,
                               SP_IPADR DestAddr, SP_IPADR NetMask,
                               SP_IPADR *p_LsId)
{
   t_A_DbEntry *p_DbEntry = NULLP;
   void        *p_NewF;
   void        *p_Buff;
   SP_IPADR     LsId = 0;
   SP_IPADR     found_mask = 0;
   e_Err        er;
   byte         Options;
   L7_uint32    lsaLength;

   Options = (p_RTO->Cfg.MulticastExtensions & INTER_AREA_MULTICAST ?
                                                   OSPF_OPT_MC_BIT : 0) |
              (p_RTO->Cfg.OpaqueCapability ? OSPF_OPT_O_BIT : 0);

   er = FindSummaryOrASExtLsa(p_RTO, p_ARO, LsType, DestAddr, NetMask,
                              &p_DbEntry, &LsId);
   if(er != E_OK)
   {
      if(p_DbEntry && (A_GET_2B(p_DbEntry->Lsa.LsAge) < MaxAge))
      {
         found_mask = A_GET_4B(p_DbEntry->p_Lsa);
         if(NetMask > found_mask)
         {
            /* case 1 - the new one more specific, set the hosts bits */
            LsId = DestAddr | ~NetMask;
         }
         else if(NetMask < found_mask)
         {
            /* case 2  - the old one more specific */
            /* Originate LSA with the new LSId (with the host bits) */
            /* for the found LSA                                    */
            LsId = DestAddr;
            if ((p_Buff = B_NewEx(NULL, NULL, A_GET_2B(p_DbEntry->Lsa.Length))) == NULL)
            {
              L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_OSPF_MAP_COMPONENT_ID, 
                     "\nFailed to allocate buffer in %s()", __FUNCTION__);

              return E_FAILED;
            }

            if((p_NewF = F_NewEx(p_Buff, 0, 0, 0)) == NULL)
            {
              L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_OSPF_MAP_COMPONENT_ID, 
                     "\nFailed to allocate frame in %s()", __FUNCTION__);
              return E_FAILED;
            }

            lsaLength = A_GET_2B(p_DbEntry->Lsa.Length);
            memcpy(GET_LSA_HDR(p_NewF), &p_DbEntry->Lsa, LSA_HEADER_LEN);
            memcpy(GET_LSA(p_NewF), p_DbEntry->p_Lsa, lsaLength - LSA_HEADER_LEN);
            /* LsaOriginate() expects frame to point to LSA body */
            BufIncOffset(F_GetFirstBuf(p_NewF), sizeof(t_S_LsaHeader));
            B_SetLength(F_GetFirstBuf(p_NewF), (lsaLength - sizeof(t_S_LsaHeader)));
            F_GetLength(p_NewF) = (lsaLength - sizeof(t_S_LsaHeader));
               
            LsaOriginate(p_RTO, p_ARO, p_NewF, LsType, DestAddr |~found_mask, 0, FALSE, Options);
         }
      }
      else
         LsId = DestAddr;
   }

   if(p_LsId)
      *p_LsId = LsId;

   return E_OK;
}



/* --------------------------------------------------------------- */
/* --------------------------------------------------------------- */
/* ---- internal routines ---------------------------------------- */
/* --------------------------------------------------------------- */
/* --------------------------------------------------------------- */

/*********************************************************************
 * @purpose   Determine the metric for an interface
 *
 * @param  p_IFO  @b{(input)} interface to be processed
 *
 * @returns link metric. If no metric on the interface is valid, returns 0.
 *
 * @notes  
 *
 * @end
 * ********************************************************************/
word InterfaceMetric(t_IFO *p_IFO)
{
  t_MetricEntry *p_M;
  e_TosTypes Tos = TOS_NORMAL_SERVICE;

  if ((HL_FindFirst(p_IFO->MetricHl, (byte *)&Tos, (void *)&p_M) == E_OK) &&
      p_M->IsValid)
  {
    return p_M->Metric;
  }
  else
  {
    return 0;
  }
}

/*********************************************************************
 * @purpose   Add one link to a router LSA
 *
 * @param  p_LinkDscr  @b{(input)} current location in router LSA
 * @param  linkId      @b{(input)} Link ID
 * @param  linkData    @b{(input)} Link Data
 * @param  linkType    @b{(input)} Link type 
 * @param  metric      @b{(input)} cost of using this router link
 *
 * @returns void
 *
 * @notes  # TOS is always set to 0
 *
 * @end
 * ********************************************************************/
void AddLinkToRtrLsa(t_S_LinkDscr *p_LinkDscr, ulng linkId, ulng linkData, 
                     e_LinkTypes linkType, word metric)
{
  A_SET_4B(linkId, p_LinkDscr->LinkId);
  A_SET_4B(linkData, p_LinkDscr->LinkData);
  p_LinkDscr->Type = (byte) linkType;
  A_SET_2B(metric, p_LinkDscr->Metric);
  p_LinkDscr->TosNum = 0;
}

/*********************************************************************
 * @purpose         Generate router LSA link information for an interface
 *                  in loopback state. 
 *
 * @param  p_IFO       @b{(input)} interface to be processed
 * @param  p_LinkDscr  @b{(input)} handle to current location in packet
 * @param  pktEnd      @b{(input}) First byte beyond end of packet. 
 *                                 Can't write here or beyond.
 * @param  links       @b{(output)} count of link descriptors in packet
 *
 * @returns E_OK 
 * @returns E_NO_ROOM if there was not enough space to add all link descriptors
 *
 * @notes  Adds at most one link descriptor for each IP address on the interface.
 *
 * @end
 * ********************************************************************/
static e_Err AddLoopStateIfoToRtrLsa(t_IFO *p_IFO, t_S_LinkDscr **p_LinkDscr, 
                                     byte *pktEnd, word *links)
{
  int j;

  if ((byte*)(*p_LinkDscr) + sizeof(t_S_LinkDscr) > pktEnd)
  {
    return E_NO_ROOM;
  }

  AddLinkToRtrLsa(*p_LinkDscr, p_IFO->Cfg.IpAdr, p_IFO->Cfg.IpMask, S_STUB_NET, 0);
  (*p_LinkDscr)++;
  (*links)++;

  /* Secondary IP addresses */
  for (j=1; j < IFO_MAX_IP_ADDRS; j++)
  {
    if (p_IFO->Cfg.addrs[j].ipAddr) 
    {
      if ((byte*)(*p_LinkDscr) + sizeof(t_S_LinkDscr) > pktEnd)
      {
        return E_NO_ROOM;
      }
      AddLinkToRtrLsa(*p_LinkDscr, p_IFO->Cfg.addrs[j].ipAddr, 
                      p_IFO->Cfg.addrs[j].ipMask, 
                      S_STUB_NET, 0);
      (*p_LinkDscr)++;
      (*links)++;
    }
  }
  return E_OK;
}

/*********************************************************************
 * @purpose  Generate router LSA link information for a point-to-point interface. 
 *
 * @param  p_IFO       @b{(input)} interface to be processed
 * @param  p_LinkDscr  @b{(input)} handle to current location in packet
 * @param  pktEnd      @b{(input}) First byte beyond end of packet. 
 *                                 Can't write here or beyond.
 * @param  links       @b{(output)} count of link descriptors in packet
 *
 * @returns E_OK 
 * @returns E_NO_ROOM if there was not enough space to add all link descriptors
 *
 * @notes  RFC 2328 section 12.4.1.1.
 *
 * @end
 * ********************************************************************/
static e_Err AddPtpIfoToRtrLsa(t_IFO *p_IFO, t_S_LinkDscr **p_LinkDscr, 
                               byte *pktEnd, word *links)
{
  int j;
  e_Err e;
  t_NBO *p_NBO;
  t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
  word ifoMetric;
  ulng linkData;

  /* if neighbor exists and it is fully adjacent, add a type 1 link. */
  if (((e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO)) == E_OK) &&
      (p_NBO->State == NBO_FULL))
  {
    if ((byte*)(*p_LinkDscr) + sizeof(t_S_LinkDscr) > pktEnd)
    {
      return E_NO_ROOM;
    }
    if (p_RTO->LsdbOverload)
      ifoMetric = OspfMaxLinkCost;
    else
      ifoMetric = InterfaceMetric(p_IFO);
    if (p_IFO->Cfg.IpAdr == 0)
      linkData = p_IFO->Cfg.IfIndex;
    else
      linkData = p_IFO->Cfg.IpAdr;

    AddLinkToRtrLsa(*p_LinkDscr, p_NBO->RouterId, linkData, 
                    S_POINT_TO_POINT, ifoMetric);
    (*p_LinkDscr)++;
    (*links)++;
  }

  /* In addition if IFO state is point to point, add a type 3 link. */
  if (p_IFO->Cfg.State == IFO_PTPST)
  {
    if ((byte*)(*p_LinkDscr) + sizeof(t_S_LinkDscr) > pktEnd)
    {
      return E_NO_ROOM;
    }
      
    /* Prefer option 2: if subnet has been assigned,
     *  - set Link ID to subnet ID
     *  - set Link Data to subnet mask
     *  - set metric to interface cost   */
    if (p_IFO->Cfg.IpAdr & p_IFO->Cfg.IpMask)
    {
      AddLinkToRtrLsa(*p_LinkDscr, (p_IFO->Cfg.IpAdr & p_IFO->Cfg.IpMask), 
                      p_IFO->Cfg.IpMask, S_STUB_NET, InterfaceMetric(p_IFO));
      (*p_LinkDscr)++;
      (*links)++;
    }
    else if ((e == E_OK) && p_IFO->Cfg.IpAdr && p_NBO->IpAdr)
    {
      /* Note that this option never gets used because we require 
       * non-zero address on local interface, and if that condition is 
       * met, then option 2 is used. */
      /* Use option 1 if neighbor's primary IP address is known. Don't 
       * add a link for unnumbered interfaces. Consider the neighbor's address
       * to be "unknown" in this case. Adding a stub link for an unnumbered
       * interface can cause a problem if the neighbor's borrowed address is
       * is from a subnet that this router is attached to. The local router
       * will compute a /32 route to the neighbor's address and misroute 
       * packets to that neighbor's address when they are intended to be sent
       * on a bcast network, not on unnumbered link.  
       * 
       *   - set Link ID to neighbor's IP addr
       *   - set Link Data to all 1s
       *   - set metric to interface cost    */
      AddLinkToRtrLsa(*p_LinkDscr, p_NBO->IpAdr, SPF_HOST_ROUTE,
                      S_STUB_NET, InterfaceMetric(p_IFO));
      (*p_LinkDscr)++;
      (*links)++;
    }

    /* For each secondary IP address, advertise the associated subnet (option 2). 
     * Can't use option 1 for the secondaries because OSPF doesn't know the 
     * neighbor's secondary addresses.  */
    for (j=1; j < IFO_MAX_IP_ADDRS; j++)
    {
      if ((p_IFO->Cfg.addrs[j].ipAddr == 0) ||
          (p_IFO->Cfg.DupNet[j] == 1))
      {
        continue;
      }
      if ((byte*)(*p_LinkDscr) + sizeof(t_S_LinkDscr) > pktEnd)
      {
        return E_NO_ROOM;
      }
      AddLinkToRtrLsa(*p_LinkDscr, (p_IFO->Cfg.addrs[j].ipAddr & p_IFO->Cfg.addrs[j].ipMask),
                      p_IFO->Cfg.addrs[j].ipMask, S_STUB_NET, InterfaceMetric(p_IFO));
      (*p_LinkDscr)++;
      (*links)++;
    }
  } /* end p2p state */
  return E_OK;
}

/*********************************************************************
 * @purpose  Generate router LSA link information for a broadcast or NBMA 
 *           interface. 
 *
 * @param  p_IFO       @b{(input)} interface to be processed
 * @param  p_LinkDscr  @b{(input)} handle to current location in packet
 * @param  pktEnd      @b{(input}) First byte beyond end of packet. 
 *                                 Can't write here or beyond.
 * @param  links       @b{(output)} count of link descriptors in packet
 *
 * @returns E_OK 
 * @returns E_NO_ROOM if there was not enough space to add all link descriptors
 *
 * @notes  RFC 2328 section 12.4.1.2.
 *
 * @end
 * ********************************************************************/
static e_Err AddBrdcIfoToRtrLsa(t_IFO *p_IFO, t_S_LinkDscr **p_LinkDscr, 
                                byte *pktEnd, word *links)
{
  int j;
  e_Err e = E_FAILED;
  t_NBO *p_NBO = NULL;
  t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
  SP_IPADR IpAdr;
  word ifoMetric;
  SP_IPADR router_id = p_RTO->Cfg.RouterId;    /* local router ID */

  /* if IFO not in waiting state */
  if (p_IFO->Cfg.State > IFO_WAIT)
  {
    IpAdr = 0;
    /* find the DR */
    if (p_IFO->DrId == router_id)
    {
      /* This router is DR. Find at least one fully adjacent neighbor. */
      e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
      while (e == E_OK)
      {
        if (p_NBO->State == NBO_FULL)
        {
          IpAdr = p_IFO->Cfg.IpAdr;
          break;
        }
        e = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
      }
    }
    /* else check if the router fully adjacent to DR */
    else if ((HL_FindFirst(p_IFO->NboHl, (byte *)&p_IFO->DrId, (void *)&p_NBO) == E_OK) &&
             (p_NBO->State == NBO_FULL))
    {
      IpAdr = p_NBO->IpAdr;
      e = E_OK;
    }

    if (e == E_OK)
    {
      if ((byte*)(*p_LinkDscr) + sizeof(t_S_LinkDscr) > pktEnd)
      {
        return E_NO_ROOM;
      }
      if (p_RTO->LsdbOverload)
        ifoMetric = OspfMaxLinkCost;
      else
        ifoMetric = InterfaceMetric(p_IFO);
      AddLinkToRtrLsa(*p_LinkDscr, IpAdr, p_IFO->Cfg.IpAdr, S_TRANSIT_NET, ifoMetric);
      (*p_LinkDscr)++;
      (*links)++;

      /* Secondary IP addresses */
      for (j=1; j < IFO_MAX_IP_ADDRS; j++)
      {
        if ((p_IFO->Cfg.addrs[j].ipAddr == 0) || (p_IFO->Cfg.DupNet[j] == 1))
        {
          continue;
        }
        if ((byte*)(*p_LinkDscr) + sizeof(t_S_LinkDscr) > pktEnd)
        {
          return E_NO_ROOM;
        }
        AddLinkToRtrLsa(*p_LinkDscr, p_IFO->Cfg.addrs[j].ipAddr & p_IFO->Cfg.addrs[j].ipMask,
                        p_IFO->Cfg.addrs[j].ipMask, S_STUB_NET, InterfaceMetric(p_IFO));

        /* go to the next Link descriptor */
        (*p_LinkDscr)++;
        (*links)++;
      }
    }
  }

  if (e != E_OK)
  {
    /* if IFO state is waiting */
    if ((byte*)(*p_LinkDscr) + sizeof(t_S_LinkDscr) > pktEnd)
    {
      return E_NO_ROOM;
    }
    AddLinkToRtrLsa(*p_LinkDscr, p_IFO->Cfg.IpAdr & p_IFO->Cfg.IpMask, 
                    p_IFO->Cfg.IpMask, S_STUB_NET, InterfaceMetric(p_IFO));
    (*p_LinkDscr)++;
    (*links)++;

    /* Secondary IP addresses */
    for (j=1; j < IFO_MAX_IP_ADDRS; j++)
    {
      if ((p_IFO->Cfg.addrs[j].ipAddr == 0) ||
          (p_IFO->Cfg.DupNet[j] == 1))
      {
        continue;
      }
      if ((byte*)(*p_LinkDscr) + sizeof(t_S_LinkDscr) > pktEnd)
      {
        return E_NO_ROOM;
      }
      AddLinkToRtrLsa(*p_LinkDscr, p_IFO->Cfg.addrs[j].ipAddr & p_IFO->Cfg.addrs[j].ipMask,
                      p_IFO->Cfg.addrs[j].ipMask, S_STUB_NET, InterfaceMetric(p_IFO));
      (*p_LinkDscr)++;
      (*links)++;
    }
  }
  return E_OK;
}

/*********************************************************************
 * @purpose  Generate router LSA link information for a point-to-multipoint interface. 
 *
 * @param  p_IFO       @b{(input)} interface to be processed
 * @param  p_LinkDscr  @b{(input)} handle to current location in packet
 * @param  pktEnd      @b{(input}) First byte beyond end of packet. 
 *                                 Can't write here or beyond.
 * @param  links       @b{(output)} count of link descriptors in packet
 *
 * @returns E_OK 
 * @returns E_NO_ROOM if there was not enough space to add all link descriptors
 *
 * @notes  RFC 2328 section 12.4.1.4.
 *
 * @end
 * ********************************************************************/
static e_Err AddPtmIfoToRtrLsa(t_IFO *p_IFO, t_S_LinkDscr **p_LinkDscr, 
                               byte *pktEnd, word *links)
{
  e_Err e;
  int j;
  t_NBO *p_NBO = NULL;
  t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
  word ifoMetric;

  if ((byte*)(*p_LinkDscr) + sizeof(t_S_LinkDscr) > pktEnd)
  {
    return E_NO_ROOM;
  }
  AddLinkToRtrLsa(*p_LinkDscr, p_IFO->Cfg.IpAdr, SPF_HOST_ROUTE, S_STUB_NET, 0);
  (*p_LinkDscr)++;
  (*links)++;

  for (j=1; j < IFO_MAX_IP_ADDRS; j++)
  {
    if (p_IFO->Cfg.addrs[j].ipAddr)
    {
      if ((byte*)(*p_LinkDscr) + sizeof(t_S_LinkDscr) > pktEnd)
      {
        return E_NO_ROOM;
      }
      AddLinkToRtrLsa(*p_LinkDscr, p_IFO->Cfg.addrs[j].ipAddr, SPF_HOST_ROUTE, 
                      S_STUB_NET, 0);
      (*p_LinkDscr)++;
      (*links)++;
    }
  }

  /* for each fully adjacent neighbor add the type 1 link */
  e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
  while (e == E_OK)
  {
    if (p_NBO->State == NBO_FULL)
    {
      if ((byte*)(*p_LinkDscr) + sizeof(t_S_LinkDscr) > pktEnd)
      {
        return E_NO_ROOM;
      }
      if (p_RTO->LsdbOverload)
        ifoMetric = OspfMaxLinkCost;
      else
        ifoMetric = InterfaceMetric(p_IFO);
      AddLinkToRtrLsa(*p_LinkDscr, p_NBO->RouterId, p_IFO->Cfg.IpAdr, 
                      S_POINT_TO_POINT, ifoMetric);
      (*p_LinkDscr)++;
      (*links)++;
    }
    e = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
  }
  return E_OK;
}

/*********************************************************************
 * @purpose  Generate router LSA link information for a virtual interface. 
 *
 * @param  p_IFO       @b{(input)} interface to be processed
 * @param  p_LinkDscr  @b{(input)} handle to current location in packet
 * @param  pktEnd      @b{(input}) First byte beyond end of packet. 
 *                                 Can't write here or beyond.
 * @param  links       @b{(output)} count of link descriptors in packet
 *
 * @returns E_OK 
 * @returns E_NO_ROOM if there was not enough space to add all link descriptors
 *
 * @notes  RFC 2328 section 12.4.1.3.
 *
 * @end
 * ********************************************************************/
static e_Err AddVrtlIfoToRtrLsa(t_IFO *p_IFO, t_S_LinkDscr **p_LinkDscr, 
                                byte *pktEnd, word *links)
{
  e_Err e;
  t_NBO *p_NBO = NULL;
  t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
  t_ARO *transitArea = (t_ARO*) p_IFO->TransitARO;
  word ifoMetric;

  if (!transitArea)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    RTO_InterfaceName(p_IFO, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Virtual link %s transit area not set.", ifName);
    return E_OK;
  }

  if (transitArea->ExternalRoutingCapability != AREA_IMPORT_EXTERNAL)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 *areaTypeStr = "Unknown";
    
    RTO_InterfaceName(p_IFO, ifName);
    osapiInetNtoa(transitArea->AreaId, areaIdStr);
    if (transitArea->ExternalRoutingCapability == AREA_IMPORT_NO_EXTERNAL)
      areaTypeStr = "Stub";
    else if (transitArea->ExternalRoutingCapability == AREA_IMPORT_NSSA)
      areaTypeStr = "NSSA";
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Virtual link %s transit area %s is an invalid transit area type (%s).",
            ifName, areaIdStr, areaTypeStr);
    return E_OK;
  }


  /* if virtual neighbor exists and it is fully adjacent */
  if (((e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO)) == E_OK) &&
      (p_NBO->State == NBO_FULL))
  {
    /* Set V-bit for the transit area if some Virtual Interface found */
    transitArea->HasVirtLink = TRUE;

    if ((byte*)(*p_LinkDscr) + sizeof(t_S_LinkDscr) > pktEnd)
    {
      return E_NO_ROOM;
    }
    if (p_RTO->LsdbOverload)
      ifoMetric = OspfMaxLinkCost;
    else
      ifoMetric = InterfaceMetric(p_IFO);
    AddLinkToRtrLsa(*p_LinkDscr, p_NBO->RouterId, p_IFO->Cfg.IpAdr, 
                    S_VIRTUAL_LINK, ifoMetric);

    (*p_LinkDscr)++;
    (*links)++;
  }
  return E_OK;
}

/*********************************************************************
 * @purpose  Generate router LSA host information for an area. 
 *
 * @param  p_ARO       @b{(input)} area to be processed
 * @param  p_LinkDscr  @b{(input)} handle to current location in packet
 * @param  pktEnd      @b{(input}) First byte beyond end of packet. 
 *                                 Can't write here or beyond.
 * @param  links       @b{(output)} count of link descriptors in packet
 *
 * @returns E_OK 
 * @returns E_NO_ROOM if there was not enough space to add all link descriptors
 *
 * @notes  RFC 2328 section 12.4.1.
 *
 *         FASTPATH does not allow configuration of host links.
 *
 * @end
 * ********************************************************************/
static e_Err AddHostsToRtrLsa(t_ARO *p_ARO, t_S_LinkDscr **p_LinkDscr, 
                              byte *pktEnd, word *links)
{
  e_Err e, er;
  t_S_HostRouteCfg *p_Host = NULL;
  t_RTO *p_RTO = (t_RTO*) p_ARO->RTO_Id;
  t_IFO *p_IFO = NULL;

  e = HL_GetFirst(p_ARO->HostHl, (void *)&p_Host);
  while (e == E_OK)
  {
    if (p_Host->HostStatus == ROW_ACTIVE)
    {
      /* check if the interface for this host already exists */
      er = HL_FindFirst(p_RTO->IfoIndexHl, (byte*)&p_Host->IfIndex, (void**)&p_IFO);

      if (er == E_OK)
      {
        if ((byte*)(*p_LinkDscr) + sizeof(t_S_LinkDscr) > pktEnd)
        {
          return E_NO_ROOM;
        }
        AddLinkToRtrLsa(*p_LinkDscr, p_Host->HostIpAdr, SPF_HOST_ROUTE, 
                        S_STUB_NET, p_Host->HostMetric);
        (*p_LinkDscr)++;
        (*links)++;
      }
    }
    e = HL_GetNext(p_ARO->HostHl, (void *)&p_Host, p_Host);
  }
  return E_OK;
}

/*********************************************************************
 * @purpose       Debug virtual links
 *
 *
 * @param void
 *
 * @returns         
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void ospfDebugVlinks(void)
{
  t_RTO *p_RTO = (t_RTO*) RTO_List;
  SP_IPADR backbone = OSPF_BACKBONE_AREA_ID;
  e_Err e;
  t_ARO *p_ARO = NULL;
  t_IFO *p_IFO = NULL;
  
  if (HL_FindFirst(p_RTO->AroHl, (byte *) &backbone, (void *)&p_ARO) != E_OK)
  {
    printf("\nCan't find backbone area.");
    return;
  }

  e = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
  while (e == E_OK)
  {
    if (p_IFO->Cfg.Type == IFO_VRTL)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_uchar8 nbrIdStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 localAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      t_ARO *p_transitARO = (t_ARO*) p_IFO->TransitARO;
      RTO_InterfaceName(p_IFO, ifName);
      printf("\n\nVirtual interface %s...", ifName);
      printf("\nTransit area ID %lu.", (ulng) p_IFO->Cfg.VirtTransitAreaId);
      if (p_transitARO)
        printf("\nTransit area object %lu.", p_transitARO->AreaId);
      else
        printf("\nNot referencing a transit area.");
      osapiInetNtoa(p_IFO->Cfg.VirtIfNeighbor, nbrIdStr);
      printf("\nVirtual nbr router ID:  %s", nbrIdStr);
      osapiInetNtoa(p_IFO->VrtNgbIpAdr, nbrAddrStr);
      printf("\nVirtual nbr address:  %s", nbrAddrStr);
      osapiInetNtoa(p_IFO->Cfg.IpAdr, localAddrStr);
      printf("\nLocal address:  %s", localAddrStr);
    }
    e = HL_GetNext(p_ARO->IfoHl, (void*)&p_IFO, p_IFO);
  }
}

/*********************************************************************
 * @purpose         Originate Router LSA.
 *
 * @param Id        @b{(input)}  Area Object handler
 * @param prm       @b{(input)}  unused
 *
 * @returns         E_OK - if originated successfull,
 * @returns         E_FAILED - if failed in LSA allocation
 *
 * @notes  The length of a router LSA is limited by the size of the buffers we
 *         use to send packets. This size is OSPF_DATA_BUF_SIZE. A buffer of 
 *         this size must be able to contain the full IP packet. If this ends 
 *         up being larger than the IP MTU on some interface, the IP stack will
 *         fragment the IP packet. So the router LSA itself can be no bigger 
 *         than
 *               OSPF_DATA_BUF_SIZE - (IP header + std OSPF header)
 *
 *         If we allow 1500 bytes for the complete IP packet, a router LSA can  
 *         contain up to 119 links. If this limit is exceeded, we log an error 
 *         message and issue the router LSA with as many links as fit. The OSPF 
 *         configuration code attempts to prevent the user from adding too many 
 *         OSPF interfaces in a single area; however, the use of secondary IP 
 *         addresses makes it difficult to prevent the user from configuring 
 *         a router whose router LSA is too big.
 *
 * @end
 * ********************************************************************/
static e_Err LsaOrgRouter(t_Handle Id, ulng prm)
{
  t_ARO         *p_ARO = (t_ARO *) Id, *p_tmpARO;
  t_IFO         *p_IFO;
  t_RTO         *p_RTO = (t_RTO *) p_ARO->RTO_Id;
  t_ARO         *p_TrAro;
  t_S_RouterLsa *p_RouterLsa;
  t_S_LinkDscr  *p_LinkDscr;
  void          *p_F;
  e_Err         e, err;
  word          links = 0;
  SP_IPADR      router_id = p_RTO->Cfg.RouterId;
  Bool          Mcst = FALSE;
  Bool          isBdrRtr = FALSE, isNssaRtr = FALSE;
  byte Option = 0;

  /* Max length router LSA. Does not include common LSA header. Even though
   * this routine does not build an IP header or standard OSPF or LSA headers,
   * we have to leave room for them. Also must leave room for MD5 authentication
   * data in case any interface in the area runs MD5. */

  ulng maxRtrLsaLen = GetMaxLsaLength(p_ARO, S_ROUTER_LSA) - 
                      (IP_HDR_LEN + OSPF_MD5_AUTH_DATA_LEN + sizeof(t_S_PckHeader) +
                       sizeof(t_S_LsUpdate) + sizeof(t_S_LsaHeader));

  /* First byte beyond end of packet. Cannot write to this location or beyond
   * or router LSA will be too big to fit in an LS Update in an IP packet of 
   * size OSPF_MAX_LSA_BUF_LEN. */
  byte *pktEnd = NULL;

  /* TRUE when the LSA is full */
  Bool lsaFull = FALSE;

  if (!p_RTO->Cfg.AdminStat || !p_ARO->OperationState)
    return E_FAILED;

  p_ARO->IsRtrLsaOrg = TRUE;

  if (p_ARO->AreaId == OSPF_BACKBONE_AREA_ID)
  {
    /* Clear all areas "V" flag */
    e = HL_GetFirst(p_RTO->AroHl, (void *)&p_TrAro);
    while (e == E_OK)
    {
      p_TrAro->HasVirtLink = FALSE;
      e = HL_GetNext(p_RTO->AroHl, (void *)&p_TrAro, p_TrAro);
    }
  }

  /* Allocate LSA with sizeof(t_S_LsaHeader) offset */
  if ((p_F = LSA_Alloc(p_ARO, S_ROUTER_LSA)) == NULL)
    return E_FAILED;

  /* Get pointer to RouterLsa part of the LSA */
  p_RouterLsa = (t_S_RouterLsa *) GET_LSA_HDR(p_F);
  p_RouterLsa->Flags = 0;
  p_RouterLsa->Reserved = 0;

  /* Fill Link descriptors */
  p_LinkDscr = (t_S_LinkDscr *) ((byte*)p_RouterLsa + sizeof(t_S_RouterLsa));
  pktEnd = (byte*) p_RouterLsa + maxRtrLsaLen;


  /* Examine all attached to this Area links (IFOs) */
  e = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
  while ((e == E_OK) && !lsaFull)
  {
    if ((p_IFO->Cfg.State == IFO_LOOP) && (p_IFO->Cfg.Type != IFO_PTP))
    {
      if (AddLoopStateIfoToRtrLsa(p_IFO, &p_LinkDscr, pktEnd, &links) == E_NO_ROOM)
      {
        lsaFull = TRUE;
        break;     
      }
    }
    else if (p_IFO->Cfg.State != IFO_DOWN)
    {
      if(p_IFO->Cfg.passiveMode == TRUE)
      {
         if (AddLoopStateIfoToRtrLsa(p_IFO, &p_LinkDscr, pktEnd, &links) == E_NO_ROOM)
         {
           lsaFull = TRUE;
           break;     
         }
      }
      else
      {
         switch (p_IFO->Cfg.Type)
         {
           case IFO_PTP:  /* Point-to-Point */
             if (AddPtpIfoToRtrLsa(p_IFO, &p_LinkDscr, pktEnd, &links) == E_NO_ROOM)
             {
               lsaFull = TRUE;
             }
             break;
     
           case IFO_BRDC: /* Broadcast */
           case IFO_NBMA: /* NBMA */
             if (AddBrdcIfoToRtrLsa(p_IFO, &p_LinkDscr, pktEnd, &links) == E_NO_ROOM)
             {
               lsaFull = TRUE;
             }
             break;
    
           case IFO_PTM:  /* Point-to-Multipoint */
             if (AddPtmIfoToRtrLsa(p_IFO, &p_LinkDscr, pktEnd, &links) == E_NO_ROOM)
             {
               lsaFull = TRUE;
             }
             break;
   
           case IFO_VRTL:  /* Virtual */
             if (AddVrtlIfoToRtrLsa(p_IFO, &p_LinkDscr, pktEnd, &links) == E_NO_ROOM)
             {
               lsaFull = TRUE;
             }
             break;
   
           default:
             break;
         }   /* end switch */
      }     /* end !IFO_DOWN */
    }
    e = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
  }

  if (!lsaFull)
  {
    /* Now examine the hosts list attached to this area */
    if (AddHostsToRtrLsa(p_ARO, &p_LinkDscr, pktEnd, &links) == E_NO_ROOM)
    { 
      lsaFull = TRUE;
    }
  }

  if (lsaFull)
  {
    L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa((L7_uint32)p_ARO->AreaId, areaIdStr);
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_OSPF_MAP_COMPONENT_ID, 
           "Failed to add all link information to area %s router LSA. Router LSA full.",
            areaIdStr);
  }
  
 

    /* Set Link descriptors number */
    A_SET_2B(links,p_RouterLsa->LinkNum);

    isBdrRtr = routerIsBorder(p_RTO);

    /* Fill RouterLsa fields */
    p_RouterLsa->Flags |= ((p_ARO->HasVirtLink ? S_VIRTUAL_FLAG : 0) | 
                           ((isBdrRtr) ? S_BORDER_FLAG : 0) |
                           ((p_RTO->Cfg.ASBdrRtrStatus && 
                             (p_ARO->ExternalRoutingCapability != AREA_IMPORT_NO_EXTERNAL)) ? 
                            S_EXTERNAL_FLAG : 0) |
                           (Mcst ? S_WILDCARD_FLAG : 0));

    /* If an NSSA border router's translator state is enabled, set the Nt bit */
    if (p_ARO->NSSATranslatorState == NSSA_TRANS_STATE_ENABLED)
    {
      p_RouterLsa->Flags |= S_NSSATRANSLATOR_FLAG;
    }

    if (isBdrRtr)
    {
      err = HL_GetFirst(p_RTO->AroHl, (void *)&p_tmpARO);
      while (err == E_OK)
      {
        if (p_tmpARO->ExternalRoutingCapability == AREA_IMPORT_NSSA)
        {
          isNssaRtr = TRUE;
          break;
        }
        err = HL_GetNext(p_RTO->AroHl, (void *)&p_tmpARO, p_tmpARO);
      }

      if (isNssaRtr)
        p_RouterLsa->Flags |= S_EXTERNAL_FLAG;
    }

    /* Set LSA length */
    B_SetLength(F_GetFirstBuf(p_F), ((byte *)p_LinkDscr - (byte *)p_RouterLsa));
    F_GetLength(p_F) = ((byte *)p_LinkDscr - (byte *)p_RouterLsa);

    /* Go to the common part of all LSA types origination procedure */
    Option = (p_RTO->Cfg.MulticastExtensions ? OSPF_OPT_MC_BIT : 0);

    LsaOriginate(p_RTO, p_ARO, p_F, S_ROUTER_LSA, router_id, 0, FALSE, Option);

  return E_OK;
}

/*********************************************************************
 * @purpose          Originate Network LSA.
 *
 *
 * @param Id         @b{(input)}  Area Object handler
 * @param prm        @b{(input)}  IFO handler for particular network
 *
 * @returns          E_OK     - if originated successfull,
 * @returns          E_FAILED - if failed in LSA allocation
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err LsaOrgNetwork(t_Handle Id, ulng prm)
{
   t_ARO         *p_ARO = (t_ARO *) Id;
   t_IFO         *p_IFO = (t_IFO*) prm;
   t_NBO         *p_NBO;
   t_RTO         *p_RTO = (t_RTO *) p_ARO->RTO_Id;
   byte          *p_Lsa = NULLP;
   byte          *p_start = NULLP;
   void          *p_F     = NULLP;
   e_Err         er;
   word          nr = 0;
   t_A_DbKey     key;
   t_A_DbEntry   *former;
   Bool          lsaFull = FALSE;

   /* Max length network LSA. Does not include common LSA header. Even though
    * this routine does not build an IP header or standard OSPF or LSA headers,
    * we have to leave room for them. Also must leave room for MD5 authentication
   * data in case any interface in the area runs MD5. */
   ulng maxNetworkLsaLen = GetMaxLsaLength(p_ARO, S_NETWORK_LSA) - 
                           (IP_HDR_LEN + OSPF_MD5_AUTH_DATA_LEN + sizeof(t_S_PckHeader) +
                            sizeof(t_S_LsUpdate) + sizeof(t_S_LsaHeader));

   if(!p_RTO->Cfg.AdminStat || !p_ARO->OperationState)
      return E_FAILED;

   /* Examine only broadcast and NBMA networks attached to this Area */
   if((p_IFO->Cfg.Type != IFO_BRDC) && (p_IFO->Cfg.Type != IFO_NBMA))
      return E_FAILED;

   p_ARO->IsNtwLsaOrg = TRUE;

   /* Originate Netwrk-LSA only when DR equal to local router */
   if(p_IFO->DrId == p_RTO->Cfg.RouterId)
   {
      /* Allocate LSA with sizeof(t_S_LsaHeader) offset */
      if((p_F = LSA_Alloc(p_ARO, S_NETWORK_LSA)) == NULL)
         return E_FAILED;

      /* Get pointer to NetworkLsa part of the LSA */
      p_start = p_Lsa = GET_LSA_HDR(p_F);

      /* Fill network mask */
      A_SET_4B(p_IFO->Cfg.IpMask, p_Lsa);
      p_Lsa += 4;

      /* Fill local router id */
      A_SET_4B(p_RTO->Cfg.RouterId, p_Lsa);
      p_Lsa += 4;

      /* browse only fully adjacent neighbors */
      er = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
      while(er == E_OK)
      {
         if(p_NBO->State == NBO_FULL)
         {
            if(((p_Lsa + 4) - p_start) > maxNetworkLsaLen)
            {
               lsaFull = TRUE;
               break;
            }
            A_SET_4B(p_NBO->RouterId, p_Lsa);
            p_Lsa += 4;
            nr ++;
         }
         er = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
      }
   }

   if (lsaFull)
   {
      L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa((L7_uint32)p_ARO->AreaId, areaIdStr);
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_OSPF_MAP_COMPONENT_ID, 
              "Failed to add all link information to area %s Network LSA. Network LSA full.",
              areaIdStr);
   }

   /* If the router is DR and at least one FULL adjacent nbr is found */
   /* originate Network-LSA                                           */
   if (nr != 0)
   {
      /* Set LSA length */
      B_SetLength(F_GetFirstBuf(p_F), (p_Lsa - p_start));
      F_GetLength(p_F) = (p_Lsa - p_start);

      /* Go to the common part of all LSA types origination procedure */
      LsaOriginate(p_RTO, p_ARO, p_F, S_NETWORK_LSA, p_IFO->Cfg.IpAdr,0, FALSE,
        (byte)((p_IFO->Cfg.IsVpnSupported ? VPN_OPTION_BIT : 0) |
        ((p_RTO->Cfg.MulticastExtensions &
        (p_IFO->Cfg.MulticastForwarding > IF_MLTCST_BLOCKED)) ? OSPF_OPT_MC_BIT : 0) |
        (p_RTO->Cfg.OpaqueCapability ? OSPF_OPT_O_BIT : 0)));
   }
   else
   {
      /* The router is not DR or it has no FULL adj neigbors.*/
      /* Flush the recent network-LSA out.                   */
      if(p_F)
         F_Delete(p_F);

      A_SET_4B(p_IFO->Cfg.IpAdr, key.LsId);
      A_SET_4B(p_RTO->Cfg.RouterId, key.AdvertisingRouter);
      if((AVLH_Find(p_ARO->NetworkLsaHl,
                   (byte *)&key,(void *)&former,0) == E_OK) &&
         (L7_BIT_ISSET(former->dbFlags, IS_CUR)) && (A_GET_2B(former->Lsa.LsAge) < MaxAge))
      {
         FlushLsa((t_Handle)former, (ulng)p_ARO, 0);
      }

   }

   return E_OK;
}




 /* Browse all routing table entries: first all networks              */
/* (ntlookup = TRUE) then all AS boundary routers (ntlookup = FALSE). */
/* For each entry generate summary LSA                                */
e_Err LsaOrgSumBulk(t_Handle Id)
{
   t_ARO         *p_ARO = (t_ARO *) Id;
   t_RTO         *p_RTO = (t_RTO *) p_ARO->RTO_Id;
   t_RTB         *p_RTB;
   Bool           ntLookup = TRUE;
   t_Handle       routetbl = NULLP;
   e_Err          er       = E_FAILED;
   t_RoutingTableEntry *p_Rte;

   er = HL_GetFirst(p_RTO->RtbHl, (void *)&p_RTB);
   if(er != E_OK)
     return er;

   routetbl = p_RTB->RtbNtBt;

getfirst:
   er = AVL_GetFirst(routetbl, (void *)&p_Rte);
   while(er == E_OK)
   {
      if(ntLookup && (p_Rte->DestinationId == DefaultDestination))
         goto nextEntry;

      /* If we are processing route entries from the router table, skip
       * entries that are not for ASBRs. */
      if (!ntLookup)
          if ((p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER) == 0)
              goto nextEntry;

      LsaOrgSummary((t_Handle)p_ARO, (ulng)p_Rte);

nextEntry:
      er = AVL_GetNext(routetbl, (void *)&p_Rte);
   }

   if(ntLookup)
   {
      ntLookup = FALSE;
      routetbl = p_RTB->RtbRtBt;
      goto getfirst;
   }

   return E_OK;
}

/*********************************************************************
 * @purpose        Originate Network and AS Boundary Summary LSA.
 *
 *
 * @param Id       @b{(input)}  Area Object handler
 * @param prm      @b{(input)}  added/changed RTB entry or 0 for
 *                              browsing all RTB entries.
 *
 * @returns        E_OK
 * @returns        E_FAILED
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err LsaOrgSummary(t_Handle Id, ulng prm)
{
   t_ARO         *p_ARO = (t_ARO *) Id;
   t_RTO         *p_RTO = (t_RTO *) p_ARO->RTO_Id;
   byte          *p_Lsa, *p_start;
   void          *p_F;
   t_RoutingTableEntry *p_Rte;
   SP_IPADR       LsId = 0;
   byte           Options = 0;

   if(!p_RTO->Cfg.AdminStat || !p_ARO->OperationState || !prm)
      return E_FAILED;

   p_Rte = (t_RoutingTableEntry *)prm;
         
   Options = (p_RTO->Cfg.MulticastExtensions & INTER_AREA_MULTICAST ? 
                                                   OSPF_OPT_MC_BIT : 0) |
              (p_RTO->Cfg.OpaqueCapability ? OSPF_OPT_O_BIT : 0);

   if(!RteShouldBeSummarized (p_ARO, p_Rte))
      return E_OK;

   if(p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER)
   {
      /* Allocate LSA with sizeof(t_S_LsaHeader) offset */
      if((p_F = LSA_Alloc(p_ARO, S_ASBR_SUMMARY_LSA)) == NULL)
         return E_FAILED;

      /* Get pointer to SummaryLsa part of the LSA */
      p_start = p_Lsa = GET_LSA_HDR(p_F);    

      /* set network mask to 0 */
      A_SET_4B(0, p_Lsa);
      p_Lsa += 4;
      /* set metric to the route cost */
      A_SET_4B((p_Rte->Cost & 0x00ffffffL), p_Lsa);
      p_Lsa += 4;

      /* Set LSA length */
      B_SetLength(F_GetFirstBuf(p_F), (p_Lsa - p_start));
      F_GetLength(p_F) = (p_Lsa - p_start);

       /* Go to the common part of LSA origination procedure */
      LsaOriginate(p_RTO, p_ARO, p_F, S_ASBR_SUMMARY_LSA, 
                              p_Rte->DestinationId, 0, FALSE, Options);
      
      return E_OK;
   }
   
   /* If we get here the destination type is the network*/

   /* Check if the network destination has to be condensed*/
   /* If not, originate summary-LSA for the destination   */
   if(!LsaSummaryCondense(p_ARO, p_Rte))
   {
      /* Determine which LsId has to be used according to Appendix E*/
      if(GetLsIdForSummOrASExtLsa(p_RTO, p_ARO, S_IPNET_SUMMARY_LSA, p_Rte->DestinationId, 
                                  p_Rte->IpMask, &LsId) != E_OK)
         return E_FAILED;
      
      /* Allocate LSA with sizeof(t_S_LsaHeader) offset */
      if((p_F = LSA_Alloc(p_ARO, S_IPNET_SUMMARY_LSA)) == NULL)
         return E_FAILED;

      /* Get pointer to SummaryLsa part of the LSA */
      p_start = p_Lsa = GET_LSA_HDR(p_F);

      /* set network mask  */
      A_SET_4B(p_Rte->IpMask, p_Lsa);
      p_Lsa += 4;
      /* set metric to the route cost */
      A_SET_4B((p_Rte->Cost & 0x00ffffffL), p_Lsa);
      p_Lsa += 4;
      /* Set LSA length */
      B_SetLength(F_GetFirstBuf(p_F), (p_Lsa - p_start));
      F_GetLength(p_F) = (p_Lsa - p_start);

      /* Go to the common part of all LSA types origination procedure */
      LsaOriginate(p_RTO, p_ARO,p_F,S_IPNET_SUMMARY_LSA,LsId,0, FALSE, Options);

   }/* end "if not condensed" */

   return E_OK;
}

/* Verifyes if Summary LSA is needed to be originated into the area */
/* p_ARO for the destination p_Rte. (According to Section 12.4.3)   */
static Bool RteShouldBeSummarized (t_ARO *p_ARO,t_RoutingTableEntry  *p_Rte)
{
   t_OspfPathPrm *path = NULLP;
   t_RTO         *p_RTO;
   t_ARO *routesArea;

   p_RTO = p_ARO->RTO_Id;

   /* If the destination is a router, only create a summary if the router
    * is an ASBR. */
   if((p_Rte->DestinationType < DEST_TYPE_IS_NETWORK) && 
      ((p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER) == 0))
      return FALSE;

   /* Don't originate summary lsa for a reject route entry */
   if(p_Rte->isRejectRoute == TRUE)
      return FALSE;

   /* Don't send T4 LSA for ASBRs in NSSAs. T7's never get out of NSSA. */
   if (HL_FindFirst(p_RTO->AroHl, (byte*)&p_Rte->AreaId, 
                    (void *)&routesArea) != E_OK)
     return FALSE;
   if ((p_Rte->DestinationType < DEST_TYPE_IS_NETWORK) &&
       (routesArea->ExternalRoutingCapability == AREA_IMPORT_NSSA))
     return FALSE;
         
   if((p_Rte->PathType != OSPF_INTRA_AREA) &&
      (p_Rte->PathType != OSPF_INTER_AREA))
      return FALSE;

   if((p_Rte->AreaId == p_ARO->AreaId) || (p_Rte->Cost >= LSInfinity))
      return FALSE;

   if((p_Rte->PathType == OSPF_INTER_AREA) && 
      (p_ARO->AreaId == OSPF_BACKBONE_AREA_ID))
      return FALSE;

   if(p_Rte->DestinationId && (p_ARO->ImportSummaries != TRUE))
      return FALSE;

   /* check if the area is stub; ASBR summary LSA has not been originated*/
   if((p_Rte->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER) &&
      (p_ARO->ExternalRoutingCapability != AREA_IMPORT_EXTERNAL))
      return FALSE;

   /* if the next hops associated with this set of paths belong to Area A [p_ARO]
    * itself, do not generate a summary LSA.  Note that an aggregate route (a 
    * configured area range) has no next hops and is therefore not subject to
    * this rule. */
   if (p_Rte->PathPrm)
   {
       for(path = p_Rte->PathPrm; path; path = path->next)
       {
         if(path->NextHop &&
           ((t_ARO *)((t_IFO *)path->NextHop)->ARO_Id)->AreaId != p_ARO->AreaId)
            break;
       }
    
       if(!path && p_Rte->DestinationId)
          return FALSE;
   }

   /* If we get here all verifications are OK */
   return TRUE;
}



/*********************************************************************
 * @purpose  Check if an intra-area route matches a T3 area range.
 *
 *
 * @param p_ARO        @b{(input)}   handler of the area where the
 *                                   destination is going to be advertised
 * @param p_Rte        @b{(input)}  routing table entry of the destination
 *
 * @returns            TRUE - the destination is condensed; aggregation
 *                     summary LSA has been originated (if needed)
 * @returns            the destination is not condensed,i.e. it has to
 *                     be advertised into p_ARO area
 *
 * @notes
 *                      If the destination belongs to one of the address
 *                      aggregation ranges configured for the area,summary-LSA
 *                      for this destination is not sent.
 *
 * @end
 * ********************************************************************/
static Bool LsaSummaryCondense(t_ARO *p_ARO, t_RoutingTableEntry *p_Rte)
{
   t_RTO         *p_RTO = (t_RTO *) p_ARO->RTO_Id;
   byte          *p_Lsa, *p_start;
   void          *p_F;
   e_Err          er = E_OK;
   t_AgrEntry    *p_AdrRange;
   t_ARO         *p_AgrAro;
   t_A_DbEntry   *p_AggrDbEntry, *p_DbEntry;
   SP_IPADR       LsId = 0;
   Bool           mustBeCondensed  = FALSE;
   Bool           AggrSumOriginate = FALSE;
   byte           Options = 0;

   Options = (p_RTO->Cfg.MulticastExtensions & INTER_AREA_MULTICAST ?
                                                   OSPF_OPT_MC_BIT : 0) |
              (p_RTO->Cfg.OpaqueCapability ? OSPF_OPT_O_BIT : 0);

   if(p_Rte->PathType != OSPF_INTRA_AREA)
      goto SumCondense_exit;

   if(p_ARO->TransitCapability && (p_Rte->AreaId == OSPF_BACKBONE_AREA_ID))
      goto SumCondense_exit;

   /* Find handle of the p_Rte area (in order to browse its range list) */
   if(HL_FindFirst(p_RTO->AroHl,(byte*)&p_Rte->AreaId,
                                (void *)&p_AgrAro) != E_OK)
   {
      goto SumCondense_exit;
   }
   
   /* browse address range list */
   er = HL_GetFirst(p_AgrAro->AdrRangeHl, (void *)&p_AdrRange);
   while(er == E_OK)
   {
      /* If p_Rte exactly corresponds to the aggregation "rte", it is  */
      /* "aggregation Summary" originating.                            */
      if ((p_AdrRange->AggregateStatus == ROW_ACTIVE) &&
          (p_Rte == &p_AdrRange->AggregateRte))
      {
         if((p_AdrRange->AggregateEffect == AGGREGATE_ADVERTISE) &&
             p_AdrRange->alive)
            mustBeCondensed = FALSE;
         else
            mustBeCondensed = TRUE;
         
         goto SumCondense_exit;
      }
                            
      /* Check if the destination belongs to the address range */
      if((p_AdrRange->AggregateStatus == ROW_ACTIVE) &&
         (p_AdrRange->LsdbType == AGGREGATE_SUMMARY_LINK) &&
         (p_Rte->IpMask >= p_AdrRange->SubnetMask) &&
         ((p_Rte->DestinationId & p_AdrRange->SubnetMask) == 
          (p_AdrRange->NetIpAdr & p_AdrRange->SubnetMask)))
      {
         mustBeCondensed = TRUE;
                           
         /* If summary LSA for this dest is still presented, flash it */
         if(FindSummaryOrASExtLsa(p_RTO, p_ARO, S_IPNET_SUMMARY_LSA, p_Rte->DestinationId, 
                                  p_Rte->IpMask, &p_DbEntry, &LsId) == E_OK)
         {
           if(p_Rte->PathType == OSPF_INTRA_AREA)
            FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);
           else
             FlushAsLsa((t_Handle)p_DbEntry, (ulng)p_RTO);
         }

         /* Check if the "aggregation summary-LSA" has to be orig/reorig */
         if(p_AdrRange->AggregateEffect == AGGREGATE_ADVERTISE) 
         {
            if(FindSummaryOrASExtLsa(p_RTO, p_ARO, S_IPNET_SUMMARY_LSA, 
                          p_AdrRange->NetIpAdr, p_AdrRange->SubnetMask,
                          &p_AggrDbEntry, &LsId) != E_OK)
            {
               /* Lsa for the aggragation does not exist; originate it */
               LsId = 0;
               AggrSumOriginate = TRUE;
            }
            else
            {
               /* LSA for the aggregation exists; check the aggr. cost */
               ulng lsaMetric = A_GET_4B(p_AggrDbEntry->p_Lsa + 4);

               if(lsaMetric != (p_AdrRange->LargestCost & 0x00ffffffL))
               {
                 AggrSumOriginate = TRUE;
               }
            }

          }
          break;
      } /* End of "if the destination belongs to the address aggregation" */

      er = HL_GetNext(p_AgrAro->AdrRangeHl, (void *)&p_AdrRange, p_AdrRange);
   } /* End of browsing address range list */

SumCondense_exit:
   if(mustBeCondensed && AggrSumOriginate)
   {
      ulng cost = p_AdrRange->LargestCost;
      
      /* Determine which LsId has to be used according to Appendix E*/
      if(!LsId && GetLsIdForSummOrASExtLsa(p_RTO, p_ARO, S_IPNET_SUMMARY_LSA,p_AdrRange->NetIpAdr, 
                                           p_AdrRange->SubnetMask, &LsId) != E_OK)
      return E_FAILED;
      
      /* Allocate LSA with sizeof(t_S_LsaHeader) offset */
      if((p_F = LSA_Alloc(p_ARO, S_IPNET_SUMMARY_LSA)) == NULL)
      return E_FAILED;
                             
      /* Get pointer to SummaryLsa part of the LSA */
      p_start = p_Lsa = GET_LSA_HDR(p_F);    
                             
      /* set network mask  */
      A_SET_4B(p_AdrRange->SubnetMask, p_Lsa);
      p_Lsa += 4;
      /* set metric to the route cost */
      A_SET_4B((cost & 0x00ffffffL), p_Lsa);
      p_Lsa += 4;
      /* Set LSA length */
      B_SetLength(F_GetFirstBuf(p_F), (p_Lsa - p_start));
      F_GetLength(p_F) = (p_Lsa - p_start);
                             
      /* Go to the common part of LSA origination */
      LsaOriginate(p_RTO, p_ARO, p_F, S_IPNET_SUMMARY_LSA, LsId, 0, FALSE, Options);
   }
         
   return mustBeCondensed;
}
/*********************************************************************
 * @purpose       Originates summary-LSA of type 3 for default destination
 *                in a stub area .Default summary routes are used in stub
 *                areas instead of flooding a complete set of external routes.
 *
 *
 * @param p_ARO   @b{(input)}  handle of a stub area object that default
 *                             summary LSA is to be advertised into
 *
 * @returns E_OK             - if originated successfull,
 * @returns E_STATE_NOT_APPL - the router is not a stub area's area border router
 * @returns E_NOT_FOUND      - stub default metric is not defined in the data base
 *
 * @notes
 *              Default summary routes are used in stub
 *              areas instead of flooding a complete set of external routes.
 *
 * @end
 * ********************************************************************/
e_Err StubDefaultLsaOriginate(t_ARO *p_ARO)
{
   t_StubMetricEntry *p_M;
   e_Err              e       = E_FAILED;
   ulng               TOS     = 0;  /* normal service */
   Bool               stubABR = FALSE;
   t_RTO             *p_RTO   = NULLP;

   if(!p_ARO)
     return E_FAILED;

   p_RTO = (t_RTO *)p_ARO->RTO_Id;

   /* Determine if the area is a stub one and the router is a stub area */
   /* border router */
   stubABR = ((p_ARO->ExternalRoutingCapability == AREA_IMPORT_NO_EXTERNAL) &&
              (routerIsBorder(p_RTO)));

   if(!stubABR)
      return E_STATE_NOT_APPL;

   /* Check if stub default LSA was already originated, do nothing */
   if(p_ARO->StubDefLSAOriginated)
      return E_OK;

   /* Find stub default metric entry */
   e = HL_FindFirst(p_ARO->StubDefaultCostHl, (byte *)&TOS, (void *)&p_M);

   if(e != E_OK)
      return E_NOT_FOUND;

   if(!p_M->IsValid)
      return E_STATE_NOT_APPL;

   /* Originate summary-LSA with default destination into the area. */

   /* The stub area default destionation (0.0.0.0) is not defined in the */
   /* routing table of the stub area ABR (because the area border router */
   /* "sees" destinatons out of the stub area, so it does not need the   */
   /* AS internal default route)                                         */
   /* Fill t_RoutingTableEntry struct in order to use it as a parameter  */
   /* of LsaReOriginate procedure                                        */

      memset(&p_M->StubMetricRte, 0, sizeof(t_RoutingTableEntry));

      p_M->StubMetricRte.AreaId        = OSPF_BACKBONE_AREA_ID;
      p_M->StubMetricRte.DestinationId = DefaultDestination;
      p_M->StubMetricRte.IpMask        = DefaultDestination;
      p_M->StubMetricRte.DestinationType = DEST_TYPE_IS_NETWORK;
      p_M->StubMetricRte.Cost            = p_M->Metric;

      e = LsaReOriginate[S_IPNET_SUMMARY_LSA](p_ARO, (ulng)(&p_M->StubMetricRte));

      /* Set flag indicating stub default LSA originating */
      p_ARO->StubDefLSAOriginated = (e == E_OK);

      return e;
}



/*********************************************************************
 * @purpose   Originates a default LSA in a NSSA. 
 *
 *            When OSPF's summary routes are not imported, the default LSA
 *            originated by an NSSA border router into the NSSA should be
 *            a Type-3 summary-LSA. When summary routes are imported into
 *            the NSSA, the default LSA originated by an NSSA border router
 *            must NOT be a Type-3 summary-LSA. (from RFC 3101)
 *
 *            Following the industry standard, if "no-summary" is configured,
 *            a type 3 summary LSA default is advertised. If the router is
 *            configured to originate a default route into the NSSA 
 *            (area nssa default-info originate), it originates a T7 LSA default.
 *            Router can originate both simultaneously, in which case other 
 *            routers prefer the inter-area route.
 *
 *            Metric of the T3 summary is from "area default-cost."
 *
 *            A NSSA ABR can generate a default route with or without a default 
 *            route in its own routing table
 *
 * @param     p_ARO  @b{(input)}  handle of a NSSA object that default LSA is
 *                             to be advertised into
 *
 * @returns   E_OK     
 * @notes
 *            
 *
 * @end
 * ********************************************************************/
e_Err NssaDefaultLsaOriginate(t_ARO *p_ARO)
{
   e_Err    e       = E_FAILED;
   Bool     origType3 = FALSE, origType7 = FALSE;
   t_RTO    *p_RTO = (t_RTO *)p_ARO->RTO_Id;
   t_RTB    *p_RTB;
   t_RoutingTableKey   rtbkey;
   t_RoutingTableEntry rte;
   t_S_AsExternalCfg   asExtCfg;

   /* Determine if the area is a NSSA ABR or ASBR */
   if(p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA)
     return E_STATE_NOT_APPL;

   if (HL_GetFirst(p_RTO->RtbHl, (void *)&p_RTB) != E_OK)
     return E_FAILED;

   rtbkey.DstAdr = DefaultDestination;
   rtbkey.Prefix = 0L;

   if(routerIsBorder(p_RTO) && !p_ARO->ImportSummaries)
     origType3 = TRUE;

   if(p_ARO->NSSADefInfoOrig)
     origType7 = TRUE;

   if(!origType3 && !origType7)
     return E_STATE_NOT_APPL;

   /* Originate default LSA into the area. */
   if(origType3)
   {
      memset(&rte, 0, sizeof(t_RoutingTableEntry));
      rte.AreaId        = OSPF_BACKBONE_AREA_ID;
      rte.DestinationId = DefaultDestination;
      rte.IpMask        = DefaultDestination;
      rte.DestinationType = DEST_TYPE_IS_NETWORK;
      rte.PathType        = OSPF_INTRA_AREA;
      rte.Cost = p_ARO->DefaultCost;

      e = LsaReOriginate[S_IPNET_SUMMARY_LSA](p_ARO, (ulng)&rte);
   }

   if (origType7)
   {
     memset(&asExtCfg, 0, sizeof(t_S_AsExternalCfg));
     asExtCfg.DestNetIpAdr = DefaultDestination;
     asExtCfg.DestNetIpMask = DefaultDestination;
     asExtCfg.IsVpnSupported = FALSE;
     asExtCfg.PhyType = OSPF_ETHERNET;
     asExtCfg.IsMetricType2 = 
       (p_ARO->NSSADefMetricType == NSSA_NON_COMPARABLE_COST) ? TRUE : FALSE;
     asExtCfg.MetricValue   = p_ARO->NSSADefMetric;
     asExtCfg.ForwardingAdr = DefaultDestination;
     asExtCfg.AsExtStatus   = ROW_CREATE_AND_GO;
     asExtCfg.PrevEntry     = (t_Handle)RTE_ADDED;

     e = LsaReOriginate[S_NSSA_LSA](p_ARO, (ulng)&asExtCfg);
   }

   return e;
}



/*********************************************************************
 * @purpose         Originate AS External LSA.
 *
 *
 * @param Id        @b{(input)}  Router Object handler
 * @param prm       @b{(input)}  AsExternalCfg, this describes an As
 *                               External Route
 *
 * @returns         E_OK      - if originated successfull,
 * @returns         E_FAILED  - if failed in LSA allocation
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err LsaOrgAsExternal(t_Handle Id, ulng prm)
{
   t_RTO         *p_RTO = (t_RTO *) Id;
   t_S_AsExternalCfg *p_AsExt = (t_S_AsExternalCfg *) prm;
   byte          *p_Lsa;
   byte          *p_start;
   void          *p_F;
   t_A_DbKey     key;
   t_A_DbEntry   *former;

   if(!p_RTO->Cfg.AdminStat || (p_AsExt->DestNetIpAdr && p_RTO->IsOverflowed))
   {
     if (p_RTO->ospfTraceFlags & OSPF_TRACE_REDIST)
     {
       char traceBuf[OSPF_MAX_TRACE_STR_LEN];
       if (!p_RTO->Cfg.AdminStat)
       {
         sprintf(traceBuf, 
                 "External route not originated because OSPF not enabled.");
       }
       else if (p_RTO->IsOverflowed)
       {
         sprintf(traceBuf, 
                 "External route not originated because OSPF in external LSA overflow.");
       }
       RTO_TraceWrite(traceBuf);    
     }
     return E_FAILED;
   }

   /* Locally originated external LSAs are always be stored on the RTO object
    * and considered to be associated with the backbone area. 
    * A copy is made for all appropriate areas
    * during flooding.  */

   switch((e_RtChange)p_AsExt->PrevEntry)
   {
      case  RTE_ADDED:

        /* Determine which LsId has to be used according to Appendix E,
         * the LsId is cached in p_AsExt for later use.
        */
        if(GetLsIdForSummOrASExtLsa(p_RTO, NULL, S_AS_EXTERNAL_LSA, p_AsExt->DestNetIpAdr, 
                                    p_AsExt->DestNetIpMask, &p_AsExt->LsId) != E_OK)
        {
          if (p_RTO->ospfTraceFlags & OSPF_TRACE_REDIST)
           {
             char traceBuf[OSPF_MAX_TRACE_STR_LEN];
             L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
             L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
             osapiInetNtoa(p_AsExt->DestNetIpAdr, destStr);
             osapiInetNtoa(p_AsExt->DestNetIpMask, maskStr);
             sprintf(traceBuf, 
                     "Failed to compute LSID for external LSA for %s/%s.",
                     destStr, maskStr);
             RTO_TraceWrite(traceBuf);    
           }
           return E_FAILED;
        }

         /* Allocate LSA with sizeof(t_S_LsaHeader) offset */
         if((p_F = LSA_Alloc(NULL, S_AS_EXTERNAL_LSA)) == NULL)
         {
           if (p_RTO->ospfTraceFlags & OSPF_TRACE_REDIST)
           {
             char traceBuf[OSPF_MAX_TRACE_STR_LEN];
             sprintf(traceBuf, 
                     "Unable to allocate space for external LSA.");
             RTO_TraceWrite(traceBuf);    
           }
           return E_FAILED;
         }

         /* Get pointer to AsExtLsa part of the LSA */
         p_start = p_Lsa = GET_LSA_HDR(p_F);

         /* Fill network mask */
         A_SET_4B(p_AsExt->DestNetIpMask, p_Lsa);
         p_Lsa += 4;

         /* Fill metrics */
         A_SET_4B(((p_AsExt->IsMetricType2 ? 0x80000000L : 0L) | p_AsExt->MetricValue), p_Lsa);
         p_Lsa += 4;
         A_SET_4B(p_AsExt->ForwardingAdr, p_Lsa);
         p_Lsa += 4;
         A_SET_4B(p_AsExt->ExtRouteTag, p_Lsa);
         p_Lsa += 4;


         /* Set LSA length */
         B_SetLength(F_GetFirstBuf(p_F), (p_Lsa - p_start));
         F_GetLength(p_F) = (p_Lsa - p_start);

         /* Go to the common part of all LSA types origination procedure */
         LsaOriginate(p_RTO, NULL, p_F, S_AS_EXTERNAL_LSA, p_AsExt->LsId,0, FALSE,
            (byte)((p_AsExt->SupportMulticast ? OSPF_OPT_MC_BIT : 0) |
            (p_AsExt->IsVpnSupported ? VPN_OPTION_BIT : 0)));

      break;
      case RTE_REMOVED:

         /* Flush and ageout the external LSA */

         A_SET_4B(p_AsExt->LsId, key.LsId);
         A_SET_4B(p_RTO->Cfg.RouterId, key.AdvertisingRouter);

         /* Try to find the most recent former instance */
         if((AVLH_Find(p_RTO->AsExternalLsaHl, (byte *)&key, (void *)&former,0) == E_OK) &&
            (L7_BIT_ISSET(former->dbFlags, IS_CUR)) && (A_GET_2B(former->Lsa.LsAge) < MaxAge))
         {
           FlushLsa((t_Handle)former, (ulng)NULL, 0);
         }

      break;
      default:
      break;
   }
   return E_OK;
}

/*********************************************************************
* @purpose  Determine whether this router originates a type 5 LSA 
*           for a given destination network. 
*
* @param    p_RTO   @b{(input)}  OSPF instance
* @param    p_AsExt @b{(input)}  External route config object 
*
* @returns  TRUE if the router does originate a T5 LSA to this destination.
*           FALSE otherwise
*
* @notes    This test is complicated by the fact that OSPF creates a
*           T5 LSA for every redistributed route. The T5 is only originated if the router
*           has an interface to at least one area that accepts T5 LSAs. 
*
* @end
*********************************************************************/
static Bool OrigT5ToNssaDest(t_RTO *p_RTO, t_S_AsExternalCfg *p_AsExt)
{
    t_A_DbKey key;
    t_A_DbEntry *p_AsExtDbEntry;

    if (p_AsExt == NULL)
        return FALSE;

    A_SET_4B(p_AsExt->LsId, key.LsId);
    A_SET_4B(p_RTO->Cfg.RouterId, key.AdvertisingRouter);

    if ((AVLH_Find(p_RTO->AsExternalLsaHl, (byte *)&key, 
                   (void *)&p_AsExtDbEntry, 0) == E_OK) &&
        (L7_BIT_ISSET(p_AsExtDbEntry->dbFlags, IS_CUR)) && 
        (A_GET_2B(p_AsExtDbEntry->Lsa.LsAge) < MaxAge))
    {
        /* Router has a T5 LSA to this destination. Check if there
         * is an interface where it can be flooded. */
        t_ARO *p_ARO;
        e_Err err = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
        while (err == E_OK)
        {
            if (p_ARO->OperationState && 
                (p_ARO->ExternalRoutingCapability == AREA_IMPORT_EXTERNAL))
            {
                /* Router has an area that can accept T5 LSAs */
                t_IFO *p_IFO = NULL;
                if ((HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO) == E_OK) &&
                    p_IFO->Cfg.State != IFO_DOWN)
                {
                    /* router has an interface in this area */
                    return TRUE;
                }
            }
            err = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
        }
    }
    return FALSE;
}

/*********************************************************************
 * @purpose         Originate NSSA LSA. (RFC 3101, Section 2.4:
 *                  Originating Type-7 LSAs)
 *
 * @param Id        @b{(input)}  Router Object handler
 * @param prm       @b{(input)}  AS-External route configuration
 *
 * @returns         E_OK - if originated successfull,
 * @returns         E_FAILED - if failed in LSA allocation
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err LsaOrgNssa(t_Handle Id, ulng prm)
{
  t_ARO               *p_ARO = (t_ARO *) Id;
  t_RTO               *p_RTO = (t_RTO *) p_ARO->RTO_Id;
  t_RTB               *p_RTB = NULLP;
  t_IFO               *p_IFO = NULLP;
  t_S_AsExternalLsa   *p_NssaLsa;
  byte                *p_Lsa;
  byte                *p_start;
  void                *p_F;
  t_A_DbEntry         *former;
  e_Err               e, err;
  t_A_DbEntry         *p_DbEntry;
  Bool                isMetric2, pFlag;
  t_RoutingTableKey   rtbkey;
  t_RoutingTableEntry *p_AsBr = NULLP, *p_Rte = NULLP;
  t_A_DbKey           key;
  byte                Option;
  t_S_AsExternalCfg   *p_AsExt = (t_S_AsExternalCfg *) prm;
  SP_IPADR            ForwardingAdr = DefaultDestination;
  t_VpnCos CoS = TOS_NORMAL_SERVICE;

  /* NSSA AS boundary routers may only originate Type-7 LSAs into NSSAs. */
  if(!p_RTO->Cfg.AdminStat || (HL_GetFirst(p_RTO->RtbHl, (void *)&p_RTB) != E_OK) || 
     (p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA))
    return E_FAILED;

  /* Try to find a valid forwarding address: */

  /* Normally the next hop address of an installed AS external route
  ** learned by an NSSA ASBR from an adjacent AS points at one of the
  ** adjacent AS's gateway routers.  If this address belongs to a network
  ** connected to the NSSA ASBR via one of its NSSAs' active interfaces,
  ** then the NSSA ASBR copies this next hop address into the forwarding
  ** address field of the route's Type-7 LSA that is originated into this
  **  NSSA, as is currently done with Type-5 LSAs
  */
  if(p_AsExt->ForwardingAdr != DefaultDestination)
  {
    if(HL_FindFirst(p_RTO->RtbHl, (byte*)&CoS, (void*)&p_RTB) == E_OK)
    {
      rtbkey.DstAdr = p_AsExt->ForwardingAdr;
      rtbkey.Prefix = 0;

      /* If FA is reachable via the NSSA interface in this area,
       * copy the next hop address into forwarding address field,
       * else the forwarding address would be 0.0.0.0 */
      if (RTB_Find(p_RTB, &rtbkey, &p_Rte, 1) == E_OK)
      {
        if (p_Rte->AreaId == p_ARO->AreaId)
        {
          ForwardingAdr = p_AsExt->ForwardingAdr;
        }
      }
    }
  }  

  /* If internal addresses are not available, preference should be given to the
  ** router's active OSPF stub network addresses.  These choices avoid the
  ** possible extra hop that may happen when a transit network's address
  ** is used.
  */
  if((ForwardingAdr == DefaultDestination) && !routerIsBorder(p_RTO))
  {
    p_IFO = (t_IFO*) NSSA_FindForwardingAddress(Id);
    ForwardingAdr = (p_IFO != NULLP)? p_IFO->Cfg.IpAdr : DefaultDestination;
    
    if(ForwardingAdr == DefaultDestination)
      return E_FAILED;
  }

  switch((e_RtChange)p_AsExt->PrevEntry)
  {
  case RTE_ADDED:

    /* If two NSSA routers, both reachable from one another over the NSSA, 
    ** originate functionally equivalent Type-7 LSAs (i.e., same destination, 
    ** cost and non-zero forwarding address), then the router having the 
    ** least preferred LSA should flush its LSA.  (See [OSPF] Section 12.4.4.1.)  
    ** Preference between two Type-7 LSAs is determined by the following tie breaker
    ** rules:
    **    1. An LSA with the P-bit set is preferred over one with the P-bit
    **       clear.
    **    2. If the P-bit settings are the same, the LSA with the higher
    **       router ID is preferred.
    */
    e = AVLH_GetFirst(p_ARO->NssaLsaHl, (void *)&p_DbEntry);
    while(e == E_OK)
    {
      p_NssaLsa = (t_S_AsExternalLsa*)p_DbEntry->p_Lsa;
      isMetric2 = (p_NssaLsa->Flags & S_AS_EXT_E_FLAG) ? TRUE: FALSE;

      /* Functionally equivalent LSAs? */
      if((A_GET_4B(p_DbEntry->Lsa.LsId) == p_AsExt->LsId) &&
         ((isMetric2 == p_AsExt->IsMetricType2) && (A_GET_3B(p_NssaLsa->Metric) == p_AsExt->MetricValue)) &&
         (A_GET_4B(p_NssaLsa->ForwardingAddress) == ForwardingAdr))
      {
        /* Look up the routing table entries for the ASBR that originated
        ** the LSA.
        */
        rtbkey.DstAdr = A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter);
        rtbkey.Prefix = 0L;

        err = AVL_Find(p_RTB->RtbRtBt, FALSE, (byte*)&rtbkey,  (void*)&p_AsBr);
        if((err == E_OK) && 
           (p_AsBr->DestinationType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER))
        {
          pFlag = (p_DbEntry->Lsa.Options & OSPF_OPT_P_BIT) ? TRUE : FALSE;
          if(pFlag && p_RTO->Cfg.RouterId < A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter))
            return E_OK;
        }
      }

      e = AVLH_GetNext(p_ARO->NssaLsaHl,(void *)&p_DbEntry, p_DbEntry);
    }

    /* If we reached here we need to originate the LSA */

    /* Allocate LSA with sizeof(t_S_LsaHeader) offset */
    if((p_F = LSA_Alloc(p_ARO, S_NSSA_LSA)) == NULL)
       return E_FAILED;

    /* Get pointer to AsExtLsa part of the LSA */
    p_start = p_Lsa = GET_LSA_HDR(p_F);

    /* Fill network mask */
    A_SET_4B(p_AsExt->DestNetIpMask, p_Lsa);
    p_Lsa += 4;

    /* Fill metrics */
    A_SET_4B(((p_AsExt->IsMetricType2 ? 0x80000000L : 0L) | p_AsExt->MetricValue), p_Lsa);
    p_Lsa += 4;
    A_SET_4B(ForwardingAdr, p_Lsa);
    p_Lsa += 4;
    A_SET_4B(p_AsExt->ExtRouteTag, p_Lsa);
    p_Lsa += 4;

    /* Set LSA length */
    B_SetLength(F_GetFirstBuf(p_F), (p_Lsa - p_start));
    F_GetLength(p_F) = (p_Lsa - p_start);

    /* Calculate LSA options */

    /* When an NSSA border router originates both a Type-5 LSA and a Type-7
    ** LSAs for the same network, then the P-bit must be clear in the Type-7
    ** LSA so that it isn't translated into a Type-5 LSA by another NSSA
    ** border router.
    */
    pFlag = TRUE;
    if ((OrigT5ToNssaDest(p_RTO, p_AsExt) == TRUE) ||
       ((p_AsExt->LsId == DefaultDestination) && routerIsBorder(p_RTO)))
    {
        pFlag = FALSE;
    }

    Option = (byte)((p_AsExt->SupportMulticast ? OSPF_OPT_MC_BIT : 0) |
                    (p_AsExt->IsVpnSupported ? VPN_OPTION_BIT : 0) |
                    (pFlag ? OSPF_OPT_P_BIT : 0));

    /* Go to the common part of all LSA types origination procedure */
    LsaOriginate(p_RTO, p_ARO, p_F, S_NSSA_LSA, p_AsExt->LsId, 0, FALSE, Option);

    break;
  
  case RTE_REMOVED:
    /* Flush the previously originated LSA since a route delete notification has been
    ** received
    */
    A_SET_4B(p_AsExt->LsId, key.LsId);
    A_SET_4B(p_RTO->Cfg.RouterId, key.AdvertisingRouter);

    /* Try to find the most recent former instance */
    if((AVLH_Find(p_ARO->NssaLsaHl, (byte *)&key, (void *)&former,0) == E_OK) &&
       (L7_BIT_ISSET(former->dbFlags, IS_CUR)) && (A_GET_2B(former->Lsa.LsAge) < MaxAge))
    {
      FlushLsa((t_Handle)former, (ulng)p_ARO, 0);
    }
    break;

  default:
    break;
  }
  return E_OK;
}


/*********************************************************************
 * @purpose         Lsa Origination Empty routine.
 *
 *
 * @param Id        @b{(input)}  Area Object handler
 * @param prm       @b{(input)}  not use
 *
 * @returns         E_OK - always
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err LsaOrgEmpty(t_Handle Id, ulng prm)
{
   return E_OK;
}

/*********************************************************************
 * @purpose         Opaque LSA Reoriginate routine
 *
 *
 * @param Id        @b{(input)}  Area Object handler
 * @param prm       @b{(input)}  p_DbEntry
 *
 * @returns         E_OK - always
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err LsaOrgOpaque(t_Handle Id, ulng prm)
{
  t_A_DbEntry   *p_DbEntry = (t_A_DbEntry *) prm;
  byte *lsa = NULL;
  L7_uint32 lsaLength = A_GET_2B(p_DbEntry->Lsa.Length);

  if (lsaLength < LSA_HEADER_LEN)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "In %s, LSA length %d less than LSA header length %d.",
            __FUNCTION__, lsaLength, LSA_HEADER_LEN);
    return E_FAILED;
  }

  /* Need to concatentate LSA header and body */
  lsa = XX_MallocChunk(lsaLength);
  if (lsa == NULL)
  {
    return E_FAILED;
  }
  memcpy(lsa, &p_DbEntry->Lsa, LSA_HEADER_LEN);
  memcpy(lsa + LSA_HEADER_LEN, p_DbEntry->p_Lsa, lsaLength - LSA_HEADER_LEN);

  OpaqueGenerate((p_DbEntry->Lsa.LsType == S_LINK_OPAQUE_LSA) ? p_DbEntry->IFO_Id: Id,
                 p_DbEntry->Lsa.LsType, 0L,
                 (byte)(A_GET_4B(p_DbEntry->Lsa.LsId) >> 24),
                 A_GET_4B(p_DbEntry->Lsa.LsId) & 0x00FFFFFFL,
                 lsa, (word)lsaLength);
  XX_Free(lsa);

  return E_OK;
}



/*********************************************************************
 * @purpose          Originate LSA, common part of each LSA origination
 *                   procedure.
 *
 * @param p_RTO       @b{(input)}  
 * @param p_ARO       @b{(input)}  area object handler. NULL for LSAs with AS scope
 * @param p_F         @b{(input)}  LSA packet
 * @param LsaType     @b{(input)}  LSA type
 * @param LinkStateId @b{(input)}  LSA I
 * @param ReOrig      @b{(input)}  Delayed LSA origination flag
 * @param forceOrig   @b{(input)}  Force origination, even if LSA contents have not changed
 * @param Option      @b{(input)}  LSA option bitmask
 *
 * @returns           E_OK - if originated successfull,
 * @returns           E_FAILED - if failed in LSA allocation
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err LsaOriginate(t_RTO *p_RTO, t_ARO *p_ARO, void *p_F, e_S_LScodes LsaType,
                          SP_IPADR LinkStateId, Bool ReOrig, Bool forceOrig, byte Option)
{
   t_S_LsaHeader  *p_Hdr;
   void *p_B;
   t_A_DbEntry *p_DbEntry, *p_former;
   t_A_DbKey key;
   SP_IPADR router_id = p_RTO->Cfg.RouterId;
   t_DelayedLsa *p_DlyLsa;
   t_DlyLsaKey   dlykey;
   t_Handle delayList = NULL;
   t_Handle delayTimer = NULL;
   t_Handle threadHandle = p_RTO->OspfSysLabel.threadHndle;
   F_Expired delayTimerCallback;
   Bool onDelayList = FALSE;    /* set to TRUE if LSA has to be delayed to satisfy minLSInterval */

   if (!o2LsaTypeValid(LsaType))
   {
     F_Delete(p_F);
     return E_FAILED;
   }


   if (p_RTO->ospfTraceFlags & OSPF_TRACE_LSA_ORIG)
   {
     char traceBuf[OSPF_MAX_TRACE_STR_LEN];
     L7_uchar8 areaStr[OSAPI_INET_NTOA_BUF_SIZE];
     L7_uchar8 lsidStr[OSAPI_INET_NTOA_BUF_SIZE];
     strcpy(areaStr, "None");
     if (p_ARO)
       osapiInetNtoa(p_ARO->AreaId, areaStr);
     osapiInetNtoa(LinkStateId, lsidStr);
     sprintf(traceBuf, 
             "Originating area %s %s %s. Reorig: %s. Option: %#x",
             areaStr, lsaTypeNames[LsaType], lsidStr, ReOrig ? "TRUE" : "FALSE",
             Option);
     RTO_TraceWrite(traceBuf);    
   }

   /* Find the former instance */
   A_SET_4B(LinkStateId, key.LsId);
   A_SET_4B(router_id, key.AdvertisingRouter);

   /* Try to find the last most recent former instance */
   p_former = NULL;
   AVLH_Find(GET_OWNER_HL(p_ARO, LsaType), (byte *)&key, (void *)&p_former,0);

   /* if the former instance found, then check MinLSInterval
      since last LSA instance was originated */
   dlykey.LsaType = LsaType;
   dlykey.LsId = LinkStateId;
   if ((LsaType == S_AS_EXTERNAL_LSA) ||
       (LsaType == S_AS_OPAQUE_LSA))
   {
     delayList = p_RTO->DelayedLsa;
     delayTimer = p_RTO->DlyLsaTimer;
     delayTimerCallback = AsLsaDelayedTimerExp;
   }
   else
   {
     delayList = p_ARO->DelayedLsa;
     delayTimer = p_ARO->DlyLsaTimer;
     delayTimerCallback = LsaDelayedTimerExp;
   }
   if(!ReOrig && (HL_FindFirst(delayList, (byte*) &dlykey, (void *)&p_DlyLsa) == E_OK))
   {
      F_Delete(p_DlyLsa->p_F);
      HL_Delete(delayList, (byte*) &dlykey, p_DlyLsa);
   }

   /* Enforce MinLSInterval. */
   if(!ReOrig && p_former && (GET_INTERVAL(p_former->OrgTime) < MinLSInterval))
   {
      /* Delay origination of this LSA till MinLSInterval will pass */
      if ((p_DlyLsa = XX_Malloc(sizeof(t_DelayedLsa))) != NULL)
      {
        p_DlyLsa->IsValid = 1;
        p_DlyLsa->ARO_Id = p_ARO;
        /* LsaInstall() will consume original frame */
        p_DlyLsa->p_F = F_Copy(p_F);
        p_DlyLsa->LsaType = LsaType;
        p_DlyLsa->LsId = LinkStateId;
        p_DlyLsa->Option = Option;
        p_DlyLsa->OrgTime = p_former->OrgTime;
  
        HL_Insert(delayList, (byte*) &p_DlyLsa->LsaType, p_DlyLsa);
  
        /* Start one-second polling timer */
        if(!TIMER_Active(delayTimer))
           TIMER_StartSec(delayTimer, MinLSArrival, 0, 
                          delayTimerCallback, threadHandle);
  
        if (p_RTO->ospfTraceFlags & OSPF_TRACE_LSA_ORIG)
        {
          RTO_TraceWrite("Delaying origination.");    
        }
        /* Even if we put on the delay list, still install locally */
        onDelayList = TRUE;
      }
      /* If failed to allocate memory for delay list, better to send LSA early
       * than not at all. So continue on. */
   }

   /* Update LSA length and offset */
   p_B = F_GetFirstBuf(p_F);
   BufDecOffset(p_B, sizeof(t_S_LsaHeader));
   B_SetLength(p_B, (F_GetLength(p_F) + sizeof(t_S_LsaHeader)));
   F_GetLength(p_F) = (F_GetLength(p_F) + sizeof(t_S_LsaHeader));

   /* Get pointer to LSA header */
   p_Hdr = (t_S_LsaHeader *) B_GetData(p_B);

   /* Fill header info */
   A_SET_2B(0, p_Hdr->LsAge);
   if ((LsaType == S_AS_EXTERNAL_LSA) ||
       (LsaType == S_AS_OPAQUE_LSA))
   {
     p_Hdr->Options = Option | OSPF_OPT_E_BIT;
   }
   else
   {
     p_Hdr->Options = Option | (p_ARO->ExternalRoutingCapability == AREA_IMPORT_EXTERNAL ?
                                OSPF_OPT_E_BIT : 0);
   }

   p_Hdr->LsType = LsaType;
   A_SET_4B(router_id, p_Hdr->AdvertisingRouter);

   A_SET_2B(F_GetLength(p_F), p_Hdr->Length);

   /* if the former instance found */
   if(p_former)
   {
     /* RFC 2328 section 12.4:  "These events should cause new
        originations if and only if the contents of the new LSA would be
        different." LsaContentDiff() only checks age and options in the 
        OSPF header; so ok to set seqno, LSID, and checksum later. 
        Of course we have to reoriginate at least once every LSRefreshTime or if we've
        bumped the seqno after receiving a newer self-originated LSA. Caller sets
        forceOrig in those cases. */
     UpdateAgeOfDbEntry (p_former);
     if (!forceOrig && !LsaContentDiff(p_F, p_former))
     {
       if (p_RTO->ospfTraceFlags & OSPF_TRACE_LSA_ORIG)
       {
         RTO_TraceWrite("Suppressing origination of identical LSA.");    
       }
       F_Delete(p_F);
       return E_OK;
     }
     
      /* Set the next seqnum */
      if(A_GET_4B(p_former->Lsa.SeqNum) == MaxSequenceNumber)
      {
        /* Flush the old all-ones-seqnum LSA from this area.
        * Set the rollover flag so that the LSA will is automatically 
        * re-originated via the LSA aging table.
        */
        L7_BIT_SET(p_former->dbFlags, SEQNO_OVER);
        FlushLsa((t_Handle)p_former, (ulng)p_ARO, 0);
        F_Delete(p_F);
        return E_OK;
      }
      else
      {
        A_SET_4B((A_GET_4B(p_former->Lsa.SeqNum) + 1), p_Hdr->SeqNum);
      }
   }
   else
   {
      /* if no valid LSA insalled in LSA DB
         set the Initial LSA Sequence number       */
      A_SET_4B(InitialSequenceNumber, p_Hdr->SeqNum);
   }

   /* Set the LSA's ID */
   A_SET_4B(LinkStateId, p_Hdr->LsId);

   A_SET_2B(0, p_Hdr->CheckSum);
   FletcherCheckSum(((byte*)p_Hdr)+2,
                    A_GET_2B(p_Hdr->Length)-2,
                    offsetof(t_S_LsaHeader, CheckSum) - 2);

   /* Convert checksum 0x00 bytes to 0xff. This ensures that
    * we dont get into a mess while aging out this LSA. 
    * Cisco calculates 0xff for some LSAs that we calc as 0x00.
    * While aging out a self orig LSAs we keep the seqno same while
    * setting the age to 3600. However since the compare logic looks
    * at the checksum value before the age - this leads to an 
    * infinite loop
    */
   p_Hdr->CheckSum[0] = (p_Hdr->CheckSum[0] == 0) ? 0xff : p_Hdr->CheckSum[0];
   p_Hdr->CheckSum[1] = (p_Hdr->CheckSum[1] == 0) ? 0xff : p_Hdr->CheckSum[1];

   /* Install this LSA into Area database */
   p_DbEntry = LsaInstall(p_RTO, p_ARO, p_F, p_former);

   if(!p_DbEntry)
   {
      if (p_RTO->ospfTraceFlags & OSPF_TRACE_LSA_ORIG)
      {
        char traceBuf[OSPF_MAX_TRACE_STR_LEN];
        sprintf(traceBuf, "Failed to install self-originated LSA in LSDB.");
        RTO_TraceWrite(traceBuf);    
      }
      return E_FAILED;
   }

   p_RTO->Cfg.OriginateNewLsas ++;

   if (!onDelayList)
   {
     /* Flood this LSA to appropriate IFOs */
     LSA_FLOODING(p_RTO, p_ARO, NULL, p_DbEntry, NULL);
   }

   LsaSendTrap(p_RTO, p_ARO, p_DbEntry);

   return E_OK;
}

/*********************************************************************
 * @purpose         Returns the maximum allowed LSA size for an LSA
 *                  in an area
 *
 * @param p_ARO     @b{(input)}  Area Object
 * @param lsaType   @b{(input)}  OSPF Lsa Type
 *
 * @returns          returns max lsa length if success or NULL otherwise
 *
 * @notes
 *
 *
 * @end
 * ********************************************************************/
static ulng GetMaxLsaLength(t_ARO *p_ARO, e_S_LScodes lsaType)
{
   e_Err er;
   t_IFO *p_IFO;
   ulng maxLsaLen, minMtuInArea = OSPF_MAX_LSA_BUF_LEN;

   switch(lsaType)
   {
      /* to allow larger router-lsa and network-lsa, we take a
         sufficiently larger buffer */
      case S_ROUTER_LSA:
      case S_NETWORK_LSA:
         maxLsaLen = OSPF_MAX_LSA_BUF_LEN;
      break;

      case S_IPNET_SUMMARY_LSA:
      case S_ASBR_SUMMARY_LSA:
         maxLsaLen = sizeof(t_S_LsaHeader) + sizeof(t_S_SummaryLsa);
      break;

      case S_AS_EXTERNAL_LSA:
      case S_NSSA_LSA:
         maxLsaLen = sizeof(t_S_LsaHeader) + sizeof(t_S_AsExternalLsa);
      break;

      default:
      {
         /* for other LSAs, get the minimum IP MTU of all 
            the interfaces in this area */
        if (!p_ARO)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
                  "No area while trying to get the max LSA length for LSA type %d.",
                  lsaType);
          return 0;
        }

         er = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
         while(er == E_OK)
         {
            if(p_IFO->Cfg.MaxIpMTUsize < minMtuInArea)
               minMtuInArea = p_IFO->Cfg.MaxIpMTUsize;

            er = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
         }
         maxLsaLen = minMtuInArea;
      }
      break;
   }
   return maxLsaLen;
}

/*********************************************************************
 * @purpose      Allocate new LSA and set offset to LSA header
 *
 *
 * @param p_ARO   @b{(input)} Area Object
 * @param lsaType @b{(input)} OSPF Lsa Type
 *
 * @returns     returns packet handler if successed or
 *                              NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void *LSA_Alloc(t_ARO *p_ARO, e_S_LScodes lsaType)
{
   void *p_F, *p_B;
   ulng maxLsaLen = GetMaxLsaLength(p_ARO, lsaType);

   if(! maxLsaLen)
   {
      LOG_MSG("Max allowed OSPF buf len is zero");
      return NULL;
   }

   /* Allocate buffer from the routing heap */
   if((p_B = B_NewEx(0, 0, maxLsaLen)) == NULL)
   {
      LOG_MSG("Unable to allocate buffer for OSPF LSA.");
      return NULL;
   }

   B_SetOffset(p_B, sizeof(t_S_LsaHeader));

   if((p_F = F_NewEx(p_B,0,0,0)) == NULL)
   {
      LOG_MSG("Unable to allocate frame for OSPF LSA.");
      return NULL;
   }

   return p_F;
}

/*********************************************************************
 * @purpose            Send LSA origination trap event to the upper
 *                     application.
 *
 * @param p_RTO        @b{(input)}  OSPF instance
 * @param p_ARO        @b{(input)}  ARO object id
 * @param p_DbEntry    @b{(input)}  new orignated LSA
 *
 * @param              @b{(output)}  n/a.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void LsaSendTrap(t_RTO *p_RTO, t_ARO *p_ARO, t_A_DbEntry *p_DbEntry)
{
   t_OriginateLsa Lsa;

   if(!p_RTO->Clbk.p_OspfTrap ||
      !(p_RTO->TrapControl & GET_TRAP_BITMASK(ORIGINATE_LSA)))
      return;

   Lsa.RouterId = p_RTO->Cfg.RouterId; /* The originator of the trap */
   if (p_ARO)
   {
     Lsa.LsdbAreaId = p_ARO->AreaId;  
   }
   else
   {
     Lsa.LsdbAreaId = 0;      /* 0.0.0.0 for AS Externals */
   }
   Lsa.LsdbType = p_DbEntry->Lsa.LsType;
   Lsa.LsdbLsid = A_GET_4B(p_DbEntry->Lsa.LsId);
   Lsa.LsdbRouterId = A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter);
   Lsa.LsdbSeqNum = A_GET_4B(p_DbEntry->Lsa.SeqNum);

   p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, ORIGINATE_LSA, (u_OspfTrap *)&Lsa);
}

/*********************************************************************
 *
 * @purpose             Refresh LSA
 *
 * @param p_RTO         @b{(input)}  RTO object pointer
 * @param p_ARO         @b{(input)}  Area object pointer
 * @param p_DbEntry     @b{(input)}  LSA data base entry
 *
 * @returns             E_OK - Lsa has been reoriginated successfully
 * @returns             E_FAILED - otherwise
 *    
 * @notes              This procedure is called from AgingTimerExp to 
 *                     reoriginate LSA after the refresh timer is 
 *                     expired or to reoriginate more recent received
 *                     self originated LSAs.
 *
 * @end
 * ********************************************************************/
e_Err LsaRefresh (t_RTO *p_RTO, t_ARO *p_ARO, t_A_DbEntry *p_DbEntry)
{
  void *p_NewF;
  void *p_Buff;
  byte options;
  L7_uint32 lsaLength = A_GET_2B(p_DbEntry->Lsa.Length);

  if ((p_Buff = B_NewEx(NULL, NULL, lsaLength)) == NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_OSPF_MAP_COMPONENT_ID, 
           "\nFailed to allocate buffer in %s()", __FUNCTION__);

    return E_FAILED;
  }

  if ((p_NewF = F_NewEx(p_Buff, 0, 0, 0)) == NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_OSPF_MAP_COMPONENT_ID, 
           "\nFailed to allocate frame in %s()", __FUNCTION__);
    return E_FAILED;
  }

  memcpy(GET_LSA_HDR(p_NewF), &p_DbEntry->Lsa, LSA_HEADER_LEN);
  if (p_DbEntry->p_Lsa)
  {
    memcpy(GET_LSA(p_NewF), p_DbEntry->p_Lsa, lsaLength - LSA_HEADER_LEN);
  }

  /* LsaOriginate() expects frame to point to LSA body */
  BufIncOffset(F_GetFirstBuf(p_NewF), sizeof(t_S_LsaHeader));
  B_SetLength(F_GetFirstBuf(p_NewF), (lsaLength - sizeof(t_S_LsaHeader)));
  F_GetLength(p_NewF) = (lsaLength - sizeof(t_S_LsaHeader));

  /* Just need to increment seqno and flood.  Use 
   * LsaOriginate(); LsaReOriginate[] won't work.
   */
  /* retain options */
  options = p_DbEntry->Lsa.Options;
  /* Force origination, even if LSA contents have not changed. */
  LsaOriginate(p_RTO, p_ARO, p_NewF, p_DbEntry->Lsa.LsType, 
               A_GET_4B(p_DbEntry->Lsa.LsId), FALSE, TRUE, options);

  return E_OK;
}

/*----------------------------------------------------------------
 *
 *      Timer Expiration Routines
 *
 *----------------------------------------------------------------*/

/*--------------------------------------------------------------------
 * @purpose    Given the db entry for a T5 LSA that has reached its 
 *             refresh age, determine whether the LSA should be 
 *             reoriginated. If so, reoriginate.
 *
 * @param p_DbEntry   @b{(input)}  T5 LSA that has reached its refresh age
 *
 * @returns E_OK     
 *          E_FAILED if an error occurred        
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RefreshTranslatedLsa(t_Handle RTO_Id, t_A_DbEntry *p_DbEntry)
{
    t_RTO *p_RTO = (t_RTO *) RTO_Id;
    t_RoutingTableKey rtbkey;
    t_VpnCos VpnCos = OSPF_PUBLIC_VPN_ID;
    t_S_AsExternalLsa *p_AsExtLsa;
    t_RoutingTableEntry *p_Rte = NULL;
    t_RTB *p_RTB = NULL;

    /* Find the routing table entry for this destination */
    if (HL_FindFirst(p_RTO->RtbHl, (byte *)&VpnCos, (void *)&p_RTB) != E_OK)
        return E_FAILED;

    rtbkey.DstAdr = A_GET_4B(p_DbEntry->Lsa.LsId);
    p_AsExtLsa = (t_S_AsExternalLsa *)p_DbEntry->p_Lsa;
    rtbkey.Prefix = A_GET_4B(p_AsExtLsa->NetworkMask);
    if (rtbkey.Prefix)
        rtbkey.DstAdr &= rtbkey.Prefix;

    if (RTB_Find(p_RTB, &rtbkey, &p_Rte, FALSE) != E_OK)
        /* No route to this destination, no don't retranslate */
        return E_OK;

    /* Verify that the route is an NSSA route. */
    if ((p_Rte->DestinationType < DEST_TYPE_IS_NETWORK) ||
        ((p_Rte->PathType != OSPF_NSSA_TYPE_1_EXT) && 
         (p_Rte->PathType != OSPF_NSSA_TYPE_2_EXT)) ||
        (p_Rte->p_DbEntry->Lsa.LsType != S_NSSA_LSA))
        /* Route to this destination not an NSSA route. Don't retranslate. */
        return E_OK;

    if (!UpdateNssaAggregationInfo(p_RTB, p_Rte, RTE_MODIFIED))
       NssaHandleRouteChange(p_RTB, p_Rte, RTE_MODIFIED, NULLP);
    
    return E_OK;
}

/*************************************************************************
 * LSA aging routines 
 * Browse procedures for various aging actions: checksum checking,  
 * Lsa refreshing (if Age is LSRefreshTime), Lsa flushing (if age > MaxAge) 
 *************************************************************************/

/*********************************************************************
 *
 * @purpose             Check LSDB for corruption
 *
 * @param user          @b{(input)}  unused
 * @param value         @b{(input)}  DB entry to check
 * @param param         @b{(input)}  unused
 *
 * @returns             TRUE
 *    
 * @notes              The LSAs at a multiple of "CheckAge" are checked 
 *                     to detect LSDB corruption. Checksum errors are 
 *                     logged, but the LSA is not deleted.
 *
 *                     Because we store the LSA header and LSA body
 *                     separately, we have to compute the checksum
 *                     in a special way here.
 *
 * @end
 * ********************************************************************/
static Bool AgingCheckLsaCheckSum (t_Handle user, byte *value, ulng param)
{
   t_A_DbEntry *p_DbEntry = (t_A_DbEntry *)value;
   byte *buffer;
   L7_uint32 lsaLength = A_GET_2B(p_DbEntry->Lsa.Length);
   word c0, c1;
   byte *hpp, *pls;

   /* start with LSA header (skipping age field) */
   buffer = &p_DbEntry->Lsa.Options;
   pls = buffer + LSA_HEADER_LEN - 2;
   c0 = c1 = 0;
   hpp = buffer;
   while(hpp < pls)
   {
      if((c0 += *hpp++) > 254)
         c0 -= 255;
      if((c1 += c0) > 254)
         c1 -= 255;
   }

   /* Now do LSA body */
   buffer = p_DbEntry->p_Lsa;
   pls = buffer + (lsaLength - LSA_HEADER_LEN);
   hpp = buffer;
   while(hpp < pls)
   {
      if((c0 += *hpp++) > 254)
         c0 -= 255;
      if((c1 += c0) > 254)
         c1 -= 255;
   }
   
   if ((c0 | c1) != 0)
   {
     LOG_MSG("LSA Checksum error detected for LSID %s checksum 0x%x. OSPF Database may be corrupted.", 
             osapiInet_ntoa(A_GET_4B(p_DbEntry->Lsa.LsId)),
             A_GET_2B(p_DbEntry->Lsa.CheckSum));
   }

   return TRUE;
}

/*********************************************************************
 *
 * @purpose             Refresh self originated aged LSAs
 *
 * @param user          @b{(input)}  pointer to RTO object
 * @param value         @b{(input)}  DB entry to refresh
 * @param param         @b{(input)}  unused
 *
 * @returns             TRUE
 *    
 * @notes              
 *
 * @end
 * ********************************************************************/
static Bool AgingRefreshLsa(t_Handle user, byte *value, ulng param)
{
   t_RTO       *p_RTO = (t_RTO*)user;
   t_ARO       *p_ARO = NULLP;
   t_A_DbEntry *p_DbEntry = (t_A_DbEntry *)value;

   if(!p_RTO || !value)
      return FALSE;

   p_ARO = (t_ARO *)p_DbEntry->ARO_Id;

   /* Only self-originated LSA have to be refreshed */
   if(A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter) == p_RTO->Cfg.RouterId)
   {
     UpdateAgeOfDbEntry(p_DbEntry);

     if (p_RTO->ospfTraceFlags & OSPF_TRACE_AGING)
     {
        char traceBuf[OSPF_MAX_TRACE_STR_LEN];
        L7_uchar8 lsIdStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 advRouterStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.LsId), lsIdStr);
        osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter), advRouterStr);
        if (p_ARO)
        {
          osapiInetNtoa(p_ARO->AreaId, areaIdStr);
        }
        else
        {
          strcpy(areaIdStr, "0");
        }
        sprintf(traceBuf, "OSPF refresh %s on area %s. LSID: %s, Adv Router: %s, Age: %d, SeqNo: 0x%lx",
               lsaTypeNames[p_DbEntry->Lsa.LsType], areaIdStr, lsIdStr, 
               advRouterStr, 
               A_GET_2B(p_DbEntry->Lsa.LsAge), A_GET_4B(p_DbEntry->Lsa.SeqNum));
        RTO_TraceWrite(traceBuf);    
     }

     LsaRefresh(p_RTO, p_ARO, p_DbEntry);
   }
	
   return TRUE;

}

/*********************************************************************
 *
 * @purpose             Flush aged LSAs
 *
 * @param user          @b{(input)}  pointer to RTO object
 * @param value         @b{(input)}  DB entry to ageout
 * @param param         @b{(input)}  unused
 *
 * @returns             TRUE
 *    
 * @notes              
 *
 * @end
 * ********************************************************************/
static Bool AgingFlushLsa(t_Handle user, byte *value, ulng param)
{
   t_RTO       *p_RTO = (t_RTO*)user;
   t_ARO       *p_ARO = NULLP;
   t_A_DbEntry *p_DbEntry = (t_A_DbEntry *)value;

   if(!p_RTO || !value)
      return FALSE;
	
   p_ARO = (t_ARO *)p_DbEntry->ARO_Id;

   if(p_RTO->retxEntries >= p_RTO->Cfg.MaxLsaRetxEntries)
   {
     /* Flushing of this lsa is held as the retx entries count has reached system max
      * We postpone the flushing action till the total count comes down */

     L7_BIT_SET(p_DbEntry->dbFlags, FLUSHING_HELD);
     return TRUE;
   }
   else
   {
     /* Ready to flush and delete this overaged lsa */
     L7_BIT_CLR(p_DbEntry->dbFlags, FLUSHING_HELD);
   }

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_AGING)
   {
      char traceBuf[OSPF_MAX_TRACE_STR_LEN];
      L7_uchar8 lsIdStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 advRouterStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.LsId), lsIdStr);
      osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter), advRouterStr);
      osapiInetNtoa(p_ARO->AreaId, areaIdStr);
      sprintf(traceBuf, "OSPF flush %s on area %s. LSID: %s, Adv Router: %s, Age: %d, SeqNo: 0x%lx",
             lsaTypeNames[p_DbEntry->Lsa.LsType], areaIdStr, lsIdStr, 
             advRouterStr, 
             A_GET_2B(p_DbEntry->Lsa.LsAge), A_GET_4B(p_DbEntry->Lsa.SeqNum));
      RTO_TraceWrite(traceBuf);    
   }

   A_SET_2B(MaxAge, p_DbEntry->Lsa.LsAge);
   LSA_FLOODING(p_RTO, p_ARO, NULL, p_DbEntry, NULL);  
   LsaAgedSendTrap(p_RTO, p_ARO, p_DbEntry);

   return TRUE;

}

/*********************************************************************
 *
 * @purpose             Remove over-aged LSAs
 *
 * @param user          @b{(input)}  pointer to RTO object
 * @param value         @b{(input)}  DB entry to remove
 * @param param         @b{(input)}  unused
 *
 * @returns             TRUE
 *    
 * @notes               This function releases the over aged LSA
 *                      if it is not on a retx list and the are does
 *                      not have any neighbors in exchange/loading.
 *
 * @end
 * ********************************************************************/
static Bool AgingRemoveOverAgedLsa(t_Handle user, byte *value, ulng param)
{
  t_RTO       *p_RTO = (t_RTO*)user;
  t_ARO       *p_ARO = NULLP;
  t_A_DbEntry *p_DbEntry = (t_A_DbEntry *)value;
  e_Err        err, er;
  t_IFO       *p_IFO;
  t_NBO       *p_NBO;
  t_RetxEntry *p_RetxEntry;
  Bool         iterateAllAreas = FALSE;
  Bool         foundInRetxList = FALSE;
  Bool         retval = TRUE;
  
  if(!p_RTO || !value)
    return FALSE;
  
  if(!p_DbEntry)
    return FALSE;
  
  if(L7_BIT_ISSET(p_DbEntry->dbFlags, FLUSHING_HELD))
  {
    /* Keep trying to flush this lsa whose flushing was postponed earlier */
    AgingFlushLsa(user, value, param);
 
    if(L7_BIT_ISSET(p_DbEntry->dbFlags, FLUSHING_HELD))
    {
      /* Still condition not met to flush this lsa, return from here */
      return TRUE;
    }
    /* If we reach here, it means we met condition to flush this lsa.
     * Now lets see below if we can removed this overaged lsa */
  }

  if((p_DbEntry->Lsa.LsType != S_AS_EXTERNAL_LSA) &&
    (p_DbEntry->Lsa.LsType != S_AS_OPAQUE_LSA))
  {
    p_ARO = (t_ARO *)p_DbEntry->ARO_Id;
    iterateAllAreas = FALSE;
    er = E_OK;
  }
  else
  {
    /* browse all areas for a t5 and opaque LSAs */
    er = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
    iterateAllAreas = TRUE;
  }

  while(er == E_OK && !foundInRetxList) 
  {
    /* An overaged LSA can be removed from the Area Database if it is no  */
    /* longer contained in any NBO's retx list and none of the NBOs is in */
    /* Exchange or Loading state.                                         */

    if(p_ARO->ExchOrLoadState)
    {
      if (p_RTO->ospfTraceFlags & OSPF_TRACE_AGING)
      {
         char traceBuf[OSPF_MAX_TRACE_STR_LEN];
         sprintf(traceBuf, "Skip deleting aged DB 0x%x LSID 0x%08x, area 0x%08x in Exchange/Load state",
                (L7_uint32)p_DbEntry, (L7_uint32)A_GET_4B(p_DbEntry->Lsa.LsId), (L7_uint32)p_ARO->AreaId);
         RTO_TraceWrite(traceBuf);
      }

     return TRUE;
    }

    /* Examine ReTx of all NBOs attached to this Area links (IFOs) */
    er = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
    while(er == E_OK && !foundInRetxList)
    {
      /* Examine all attached to this IFO neighbors */
      err = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
      while(err == E_OK && !foundInRetxList)
      {
         /* check retx list */
         if(HL_FindFirst(p_NBO->LsReTx, &p_DbEntry->Lsa.LsType,
                                    (void *)&p_RetxEntry) == E_OK)
         {
           foundInRetxList = TRUE;

           if (p_RTO->ospfTraceFlags & OSPF_TRACE_AGING)
           {
              char traceBuf[OSPF_MAX_TRACE_STR_LEN];
              sprintf(traceBuf, "Skip deleting aged DB 0x%x LSID %x, area %x SeqNo: 0x%x, on NBO %x ReTx list",
                     (L7_uint32)p_DbEntry, (L7_uint32)(A_GET_4B(p_DbEntry->Lsa.LsId)), 
                     (L7_uint32)p_ARO->AreaId, (L7_uint32)(A_GET_4B(p_DbEntry->Lsa.SeqNum)), 
                     (L7_uint32)p_NBO->IpAdr);
              RTO_TraceWrite(traceBuf);
           }
         }

         err = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
      }
      er = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
    }

    if(iterateAllAreas == FALSE)
      break;

    er = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
  }

    
  /* Remove the LSA if it hasn't been found in Retx list */
  if (!foundInRetxList)
  {
    
    if(L7_BIT_ISSET(p_DbEntry->dbFlags, SEQNO_OVER))
    {
      if (p_RTO->ospfTraceFlags & OSPF_TRACE_AGING)
      {
         char traceBuf[OSPF_MAX_TRACE_STR_LEN];
         sprintf(traceBuf, "%s: seqno wrap", __FUNCTION__);
         RTO_TraceWrite(traceBuf);    
      }
    
      /* The LSA was flushed because of Max seq num was achived, */
      /* we need to reoriginate such LSA (not to remove it). We  */
      /* set the SewNum to initial-1 as the refresh process will */
      /* increment it                                            */
      L7_BIT_CLR(p_DbEntry->dbFlags, SEQNO_OVER);
      A_SET_4B(InitialSequenceNumber - 1, p_DbEntry->Lsa.SeqNum);
      LsaRefresh(p_RTO, p_ARO, p_DbEntry);
    }
    else
    {
      if (p_RTO->ospfTraceFlags & OSPF_TRACE_AGING)
      {
         char traceBuf[OSPF_MAX_TRACE_STR_LEN];
         sprintf(traceBuf, "Deleting DBE 0x%x type %d on area %x. LSID: %x, Adv Router: %x, Age: %d, SeqNo: 0x%x",
                (L7_uint32)p_DbEntry, (L7_uint32)p_DbEntry->Lsa.LsType, 
                p_ARO ? (L7_uint32)p_ARO->AreaId : 0, (L7_uint32)A_GET_4B(p_DbEntry->Lsa.LsId), 
                (L7_uint32)A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter), 
                (L7_uint32)A_GET_2B(p_DbEntry->Lsa.LsAge), 
                (L7_uint32)A_GET_4B(p_DbEntry->Lsa.SeqNum));
         RTO_TraceWrite(traceBuf);    
      }
    
       /* Release the LSA and the entry from the aging table.
        * Return TRUE to not have the browser delete the entry from the
        * aging table.
        */
       LsaRelease(p_RTO, p_DbEntry, 1);
       retval = TRUE;
    }
  }

  return retval;
}

/*********************************************************************
 *
 * @purpose             OSPF aging tick handler
 *
 * @param userId        @b{(input)}  pointer to RTO object
 * @param AgingObjId    @b{(input)}  pointer to the aging object
 *
 * @returns             TRUE
 *    
 * @notes              
 *
 * @end
 * ********************************************************************/
e_Err ospfAgingTick( t_Handle userId, t_Handle AgingObjId)
{
   t_RTO  *p_RTO = (t_RTO *)userId;
   word    chkTime;

   /* Small verification */
   ASSERT(p_RTO->AgingObj == AgingObjId);
   if(p_RTO->AgingObj != AgingObjId)
      return E_FAILED;
   
   /* Checksum checking */
   for(chkTime = CheckAge;chkTime < MaxAge;chkTime = chkTime + CheckAge)
      AgingTbl_BrowseAge(AgingObjId, AgingCheckLsaCheckSum, chkTime, 0);

   /* turn on bundling */
   p_RTO->BundleLsas = TRUE;
   {
   /* Lsa refreshing of self-originated LSAs */
   AgingTbl_BrowseAge(AgingObjId, AgingRefreshLsa, LsRefreshTimeGet(p_RTO), 0);

   /* Flush all LSAs reached MaxAge */
   AgingTbl_BrowseAge(AgingObjId, AgingFlushLsa, MaxAge, 0);
   }
   /* flush pending bundles */
   if (p_RTO->BundleLsas)
     LsUpdatesSend(p_RTO);

   /* Remove all overaged LSAs */
   AgingTbl_BrowseAge(AgingObjId, AgingRemoveOverAgedLsa, MaxAge+1, 0);

   return E_OK;
}

/* Check an LSA delay list for LSAs that can now be originated. */
static e_Err CheckDelayedLsaList(t_RTO *p_RTO, t_Handle delayList)
{
  t_DelayedLsa *p_DlyLsa = NULLP, *p_NextDly = NULLP;
  t_DelayedLsa tmp_DlyLsa;
  t_DlyLsaKey   dlykey;
  e_Err         err      = E_FAILED;

  /* Check Delayed LSA list */
   err = HL_GetFirst(delayList, (void *)&p_DlyLsa);

   while((err == E_OK) && p_DlyLsa)
   {
      err = HL_GetNext(delayList, (void *)&p_NextDly, p_DlyLsa);
     
      if(GET_INTERVAL(p_DlyLsa->OrgTime) >= MinLSInterval)
      {
         /* Keep the Lsa info of the entry that is being deleted */
         memset(&tmp_DlyLsa, 0, sizeof(t_DelayedLsa));
         memcpy(&tmp_DlyLsa, p_DlyLsa, sizeof(t_DelayedLsa));

         dlykey.LsaType = p_DlyLsa->LsaType;
         dlykey.LsId    = p_DlyLsa->LsId;
         HL_Delete(delayList, (byte*)&dlykey, p_DlyLsa);

         /* force origination even if LSA contents are the same. Don't know if 
          * original call to LsaOriginate() set forceOrig. Don't want to suppress 
          * an LSA we need to send. */
         LsaOriginate(p_RTO, tmp_DlyLsa.ARO_Id, tmp_DlyLsa.p_F, tmp_DlyLsa.LsaType, 
                      tmp_DlyLsa.LsId, 1, TRUE, tmp_DlyLsa.Option);
      }
      
      p_DlyLsa = p_NextDly;
   }
   return E_OK;
}

/* Delay list processing for LSAs with AS flooding scope */
e_Err AsLsaDelayedTimerExp(t_Handle Id, t_Handle TimerId, word Flag)
{
   t_RTO *p_RTO = (t_RTO *) Id;
   word num = 0;
   t_Handle delayList = p_RTO->DelayedLsa;

   CheckDelayedLsaList(p_RTO, delayList);

   /* if list not empty yet, restart the timer */
   HL_GetEntriesNmb(delayList, &num);
   if (num)
      TIMER_StartSec(p_RTO->DlyLsaTimer, MinLSArrival, 0,
                     AsLsaDelayedTimerExp, p_RTO->OspfSysLabel.threadHndle);

   return E_OK;
}

/* Delay list processing for LSAs with less than AS flooding scope */
e_Err LsaDelayedTimerExp( t_Handle Id, t_Handle TimerId, word Flag )
{
   t_ARO *p_ARO = (t_ARO *) Id;
   t_RTO *p_RTO = (t_RTO*) p_ARO->RTO_Id;
   word num = 0;
   t_Handle delayList = p_ARO->DelayedLsa;

   CheckDelayedLsaList(p_RTO, delayList);

   /* if list not empty yet, restart the timer */
   HL_GetEntriesNmb(delayList, &num);
   if (num)
      TIMER_StartSec(p_ARO->DlyLsaTimer, MinLSArrival, 0,
                     LsaDelayedTimerExp, p_ARO->OspfSysLabel.threadHndle);

   return E_OK;
}


Bool DlyLsaDelete(t_Handle Dly, ulng prm)
{
   t_DelayedLsa *p_DlyLsa = (t_DelayedLsa *) Dly;

   p_DlyLsa->IsValid = 0;
   F_Delete(p_DlyLsa->p_F);
   return FALSE;
}

/*********************************************************************
 * @purpose        Flush AS-Ext LSA.
 *
 *
 * @param db       @b{(input)} LSA DB entry pointer
 * @param aroId    @b{(input)} OSPF instance
 *
 * @param          @b{(output)}  returns flag
 *
 * @returns        TRUE
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool FlushAsLsa(t_Handle db, ulng rtoId)
{
   t_A_DbEntry *p_DbEntry = (t_A_DbEntry *) db;
   t_RTO       *p_RTO = (t_RTO*) rtoId;

   /* Skip default LSAs */
   if(A_GET_4B(p_DbEntry->Lsa.LsId) == 0)
      return TRUE;

   /* Flush only self-originated LSAs */
   if(A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter) != p_RTO->Cfg.RouterId)
	   return TRUE;

   /* Flush the LSA */
   FlushLsa((t_Handle)p_DbEntry, (ulng) NULL, 0);

   return TRUE;
}




/*********************************************************************
 * @purpose       Flush out summary-LSAs containing destinations belonging
 *                to the specified address range (address aggregation).
 *                This procedure is called when an aggregation is 
 *                created; therefore all destinations contained in the
 *                range have to be flushed.
 *
 *
 * @param  p_ARO            @b{(input)}  area that area range belongs to 
 * @param  p_AggrEntry      @b{(input)}  area range object
 *
 * @returns                 E_OK success
 * @returns                 E_FAILED otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err HideAllSummaryOfRange(t_ARO *p_ARO, t_AgrEntry *p_AggrEntry)
{
   t_ARO  *tmp_ARO = NULLP;
   t_RTO  *p_RTO;
   t_RTB  *p_RTB;
   t_VpnCos  vpn   = 0;
   e_Err     err   = E_FAILED;
   e_Err     error = E_FAILED;
   t_A_DbKey dbkey;
   t_A_DbEntry         *p_DbEntry = NULLP;
   t_RoutingTableEntry *p_Rte = NULLP;
   Bool    dontFlushThisLsa = FALSE;

   if(!p_ARO || !p_AggrEntry)
      return E_FAILED;

   p_RTO = (t_RTO *)p_ARO->RTO_Id;

   /* the specified summary is not of type summary link do nothing */
   if(p_AggrEntry->LsdbType != AGGREGATE_SUMMARY_LINK)
      return E_OK;

   /* Find public routing table */
   if(HL_FindFirst(p_RTO->RtbHl, (byte *)&vpn, (void *)&p_RTB) != E_OK)
      return E_FAILED;

  /* Go over the routing table and flush summary-LSA, containing destinations*/
  /* that belong to the address aggregation                                  */
   error = AVL_GetFirst(p_RTB->RtbNtBt, (void *)&p_Rte);
   while((error == E_OK) && p_Rte)
   {
      /* only network destination into our area has to be checking */
      if((p_Rte->DestinationType < DEST_TYPE_IS_NETWORK) ||
         (p_Rte->AreaId != p_ARO->AreaId) ||
         (p_Rte->PathType != OSPF_INTRA_AREA) || (p_Rte->Cost >= LSInfinity))
         goto next_dbEntry;

      /* Check if the rt entry belongs to the specified address range */
      /* (aggregation), if not - process the next LSA */
      if((p_Rte->DestinationId  & p_AggrEntry->SubnetMask) !=
         (p_AggrEntry->NetIpAdr & p_AggrEntry->SubnetMask))
         goto next_dbEntry;

      if(p_Rte->isRejectRoute == TRUE)
         goto next_dbEntry;

      /* If the rt entry parameters are exactly the same as of the          */
      /* aggregation, we will not flush it out. We will reoriginate the LSA */
      if((p_Rte->DestinationId == p_AggrEntry->NetIpAdr) &&
         (p_Rte->IpMask        == p_AggrEntry->SubnetMask) &&
         (p_AggrEntry->AggregateEffect == AGGREGATE_ADVERTISE))
         dontFlushThisLsa = TRUE;

      /* If we get here the destination belongs to the specified aggregation*/
      /* Process aggregation information */
      UpdateAggregationInfo(p_RTB, p_AggrEntry, p_Rte, RTE_ADDED);

      /* Flush the LSA out all areas (if the LSA is still installed)*/
      if(dontFlushThisLsa)
         goto next_dbEntry;

      A_SET_4B(p_RTO->Cfg.RouterId,  dbkey.AdvertisingRouter);
      A_SET_4B(p_Rte->DestinationId, dbkey.LsId);
      err = HL_GetFirst(p_RTO->AroHl, (void *)&tmp_ARO);
      while (!err && tmp_ARO)
      {
         if(tmp_ARO != p_ARO)
         {
            /* 12.4.3 : can't suppress intra-area-networks of 
             *          backbone areas in transit areas */
            if(! ((tmp_ARO->TransitCapability) && 
                  (p_Rte->AreaId == OSPF_BACKBONE_AREA_ID)) )
            {
               if(AVLH_Find(tmp_ARO->NetSummaryLsaHl, (byte *)&dbkey,
                                            (void *)&p_DbEntry,0) == E_OK)
                  FlushLsa((t_Handle)p_DbEntry, (ulng)tmp_ARO, 0);
            }
         }
         err = HL_GetNext(p_RTO->AroHl, (void*)&tmp_ARO, (void*)tmp_ARO);
      } /* end of "while" cycle over areas */

next_dbEntry:
      dontFlushThisLsa = FALSE;
      error = AVL_GetNext(p_RTB->RtbNtBt, (void *)&p_Rte);
   } /* end of cycle over routing table */

   /* schedule SPF so that any existing inter-area-routes equal to 
    * this address range will be masked by the reject routes of this 
    * active range */
   ScheduleSPF(p_RTB);

   /* Originate "aggregation" summary-LSA in all areas if it is needed*/
   if(p_AggrEntry->AggregateEffect == AGGREGATE_ADVERTISE)
      OrigAggregateSummary(p_ARO, p_AggrEntry);
   else
      FlushAggregateSummary(p_ARO, p_AggrEntry);

   return E_OK;
}

/*********************************************************************
 * @purpose       Flush out summary-LSAs containing destinations belonging
 *                to the address ranges (address aggregation) of
 *                the backbone.
 *                This procedure is called for an area when it transitions
 *                to a non-transit area from a transit-area.
 *                Earlier for a transit area, all the backbone summary routes
 *                were not suppressed as per 12.4.3 by any area-ranges in the
 *                backbone.
 *                Once it becomes a non-transit area, we need to suppress the
 *                already advertised backbone summary routes into this area.
 *
 * @param  p_backbone   @b{(input)}  backbone area
 * @param  p_ARO        @b{(input)}  area object
 *
 * @returns                 E_OK success
 * @returns                 E_FAILED otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err HideSummariesOfBackbone(t_ARO *p_backbone, t_ARO *p_ARO)
{
   e_Err     er   = E_FAILED;
   e_Err     error = E_FAILED;
   t_A_DbKey dbkey;
   t_A_DbEntry         *p_DbEntry = NULLP;
   t_RoutingTableEntry *p_Rte = NULLP;
   t_AgrEntry *p_AggrEntry;
   Bool    dontFlushThisLsa = FALSE, matchingRteExists = FALSE;
   t_RTO *p_RTO;

   if(!p_ARO || !p_backbone || (p_backbone == p_ARO))
      return E_FAILED;

   p_RTO = (t_RTO *)p_ARO->RTO_Id;

   for (er = HL_GetFirst(p_backbone->AdrRangeHl, (void *)&p_AggrEntry);
        er == E_OK;
        er = HL_GetNext(p_backbone->AdrRangeHl, (void *)&p_AggrEntry, p_AggrEntry))
   {
     if(p_AggrEntry->LsdbType != AGGREGATE_SUMMARY_LINK)
        continue;

     matchingRteExists = FALSE;
     error = HL_GetFirst(p_AggrEntry->AgrRteHl, (void *)&p_Rte);
     while((error == E_OK) && p_Rte)
     {
       dontFlushThisLsa = FALSE;
       if((p_Rte->DestinationId == p_AggrEntry->NetIpAdr) &&
          (p_Rte->IpMask        == p_AggrEntry->SubnetMask) &&
          (p_AggrEntry->AggregateEffect == AGGREGATE_ADVERTISE))
       {
          dontFlushThisLsa = TRUE;
          matchingRteExists = TRUE;
       }

       if(! dontFlushThisLsa)
       {
         A_SET_4B(p_RTO->Cfg.RouterId,  dbkey.AdvertisingRouter);
         A_SET_4B(p_Rte->DestinationId, dbkey.LsId);

         if(AVLH_Find(p_ARO->NetSummaryLsaHl, (byte *)&dbkey,
                      (void *)&p_DbEntry,0) == E_OK)
           FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);
       }
       error = HL_GetNext(p_AggrEntry->AgrRteHl, (void *)&p_Rte, (void *)p_Rte);
     }

     /* Originate "aggregation" summary-LSA */
     if(p_AggrEntry->AggregateEffect == AGGREGATE_ADVERTISE)
     {
       LsaReOriginate[S_IPNET_SUMMARY_LSA](p_ARO, (ulng)&p_AggrEntry->AggregateRte);
     }
     else if(! matchingRteExists)
     {
       /* Set key for "aggregation" summary LSA searching in the data base */
       A_SET_4B(p_RTO->Cfg.RouterId,   dbkey.AdvertisingRouter);
       A_SET_4B(p_AggrEntry->NetIpAdr, dbkey.LsId);

       /* Find the "aggregation" summary-LSA; if found flush it out */
       if(AVLH_Find(p_ARO->NetSummaryLsaHl, (byte *)&dbkey,
                    (void *)&p_DbEntry,0) == E_OK)
         FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);
     }
   }

   return E_OK;
}

/*********************************************************************
 * @purpose  Flush out translated Type-5 LSAs containing destinations
 *           belong to the specified address range (adress aggregation).
 *
 *
 * @param p_RTO         @b{(input)}  OSPF instance
 * @param p_AggrEntry   @b{(input)}  pointer to struct with Address
 *                                   Aggregation parameters
 *
 * @returns             E_OK success
 * @returns             E_FAILED otherwise
 *
 * @notes               This procedure is called when aggregation is beeing
 *                      created/activated, therefore all destination of the
 *                      range have to be advertised now as MAX_Age External-LSA.
 *
 *                      A type-7 Address range contains a list of Type-7
 *                      routes learned from the NSSA.  When the address
 *                      range gets activated, we iterate through the route
 *                      table to identify routes that belong to this address
 *                      range, flush the corresponding individual Type-5 LSAs
 *                      and originate a single Type-5 LSA for the range of
 *                      routes.
 *
 * @end
 * ********************************************************************/
static e_Err HideAllExtSummaryOfRange(t_RTO *p_RTO, t_AgrEntry *p_AggrEntry)
{
   t_RTB  *p_RTB;
   t_VpnCos  vpn   = 0;
   e_Err     error = E_FAILED;
   t_A_DbKey dbkey;
   t_A_DbEntry *p_DbEntry = NULLP;
   t_RoutingTableEntry *p_Rte = NULLP;
   Bool    dontFlushThisLsa = FALSE;

   if(!p_RTO || !p_AggrEntry)
      return E_FAILED;

   /* it is not area border router or the specified summary is not of type nssa
   ** external link do nothing
   */
   if(!routerIsBorder(p_RTO) ||
      (p_AggrEntry->LsdbType != AGGREGATE_NSSA_EXTERNAL_LINK))
     return E_OK;

   /* Find public routing table */
   if(HL_FindFirst(p_RTO->RtbHl, (byte *)&vpn, (void *)&p_RTB) != E_OK)
      return E_FAILED;

  /* Go over the routing table and flush external-LSAs, containing destinations
  ** that belong to the address aggregation 
  */
   error = AVL_GetFirst(p_RTB->RtbNtBt, (void *)&p_Rte);
   while((error == E_OK) && p_Rte)
   {
      /* Consider only External routes */
      if((p_Rte->DestinationType < DEST_TYPE_IS_NETWORK) ||
         ((p_Rte->PathType != OSPF_NSSA_TYPE_1_EXT) && 
          (p_Rte->PathType != OSPF_NSSA_TYPE_2_EXT)) || 
         (p_Rte->Cost >= LSInfinity))
         goto next_dbEntry;

      /* Check if the rt entry belongs to the specified address range */
      /* (aggregation), if not - process the next LSA */
      if((p_Rte->DestinationId  & p_AggrEntry->SubnetMask) !=
         (p_AggrEntry->NetIpAdr & p_AggrEntry->SubnetMask))
         goto next_dbEntry;

      /* If the rt entry parameters are exactly the same as of the          */
      /* aggregation, we will not flush it out. We will reoriginate the LSA */
      if((p_Rte->DestinationId == p_AggrEntry->NetIpAdr) &&
         (p_Rte->IpMask        == p_AggrEntry->SubnetMask) &&
         (p_AggrEntry->AggregateEffect == AGGREGATE_ADVERTISE))
         dontFlushThisLsa = TRUE;

      /* Add the new RTE into aggregated RTE hash list for this range */
      AddRouteToT7Range(p_RTO, p_AggrEntry, p_Rte);

      /* Flush the LSA out all areas (if the LSA is still installed)*/
      if(dontFlushThisLsa)
        goto next_dbEntry;

      A_SET_4B(p_RTO->Cfg.RouterId,  dbkey.AdvertisingRouter);
      A_SET_4B(p_Rte->DestinationId, dbkey.LsId);

      /* Locate the corresponding AS external LSA */
      if(AVLH_Find(p_RTO->AsExternalLsaHl, (byte *)&dbkey, (void *)&p_DbEntry, 0) == E_OK)
      {
        FlushAsLsa((t_Handle)p_DbEntry, (ulng)p_RTO);
      }

next_dbEntry:
      dontFlushThisLsa = FALSE;
      error = AVL_GetNext(p_RTB->RtbNtBt, (void *)&p_Rte);
   } /* end of cycle over routing table */

   /* Now find largest cost route included in range */
   if (NssaRangeLcRouteFind(p_AggrEntry) == E_OK)
   {
     p_AggrEntry->alive = TRUE;
   }

   /* Originate "aggregation" summary-LSA in all areas if it is needed*/
   if(p_AggrEntry->AggregateEffect == AGGREGATE_ADVERTISE)
      OrigExtAggregateSummary(p_RTO, p_AggrEntry);
   else
      FlushExtAggregateSummary(p_RTO, p_AggrEntry);

   return E_OK;

}




/*********************************************************************
 * @purpose          When the user deletes an area range from the router's
 *                   configuration, flush the summary previously advertised
 *                   for the range and readvertise any routes contained in
 *                   the range.
 *
 *
 * @param p_ARO        @b{(input)}  area object where the address
 *                                  aggregation is declared
 * @param p_AggrEntry  @b{(input)}  pointer to Address Aggregation
 *                                  entry being removed
 *
 * @returns            E_OK success
 * @returns            E_FAILED otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err DeliverAllSummaryOfRange(t_ARO *p_ARO, t_AgrEntry *p_AggrEntry)
{
   t_ARO  *tmp_ARO = NULLP;
   t_RTO  *p_RTO;
   t_RTB  *p_RTB;
   e_Err   err     = E_FAILED;
   e_Err   error   = E_FAILED;
   t_RoutingTableEntry *p_rte = NULLP, *p_RtEntry;
   t_RoutingTableEntry *p_nextRte  = NULLP;
   t_RoutingTableKey   rtbKey;
   Bool                 dontFlushAggrSummary  = FALSE;
   t_VpnCos      VpnCos = 0;

   if(!p_ARO || !p_AggrEntry)
      return E_FAILED;

   if(p_AggrEntry->AggregateStatus == ROW_ACTIVE)
      return E_FAILED;

   p_RTO = (t_RTO *)p_ARO->RTO_Id;

   /* the specified summary is not of type summary link do nothing */
   if(p_AggrEntry->LsdbType != AGGREGATE_SUMMARY_LINK)
      return E_OK;

   /* Find public routing table */
   if(HL_FindFirst(p_RTO->RtbHl, (byte *)&VpnCos, (void *)&p_RTB) != E_OK)
      return E_FAILED;

   /* Remove reject routes of active address ranges from the rto */
   rtbKey.DstAdr = p_AggrEntry->NetIpAdr;
   rtbKey.Prefix = p_AggrEntry->SubnetMask;
   if (rtbKey.Prefix)
      rtbKey.DstAdr &= rtbKey.Prefix;

   if (RTB_Find(p_RTB, (t_RoutingTableKey *)&rtbKey, &p_RtEntry, 0) == E_OK)
   {
      if(p_RTO->Clbk.p_RoutingTableChanged && (p_RtEntry->isRejectRoute == TRUE))
      {
         p_RTO->Clbk.p_RoutingTableChanged(p_RTO->Clbk.f_RtmAsyncEvent[0] ? 
                     (t_Handle)p_RTO->Clbk.f_RtmAsyncEvent : p_RTO->MngId, 
                     p_RtEntry->DestinationType, RTE_REMOVED, 
                     p_RtEntry->DestinationId, p_RtEntry->IpMask, 
                     p_RtEntry->PathNum, p_RtEntry->Cost, p_RtEntry->PathPrm, p_RtEntry->PathType,
                     p_RtEntry->isRejectRoute);
         AVL_Delete(p_RTB->RtbNtBt, (byte *)&p_RtEntry->DestinationId, NULL);
         RteRelease((byte*)p_RtEntry, 0);
      }
   }

   /* schedule SPF so that any inter-area-routes equal to this address range
    * which were masked earlier will be added now ( as the reject routes of
    * this active range is deleted ) */
   ScheduleSPF(p_RTB);

   /* First determine if the existing summary lsa needs to be flushed from the
   ** routing domain.  Flush the "aggregation" summary-LSA if there is no
   ** corresponding route entry that exactly matches the lsid and mask
   */
   error = HL_GetFirst(p_AggrEntry->AgrRteHl, (void *)&p_rte);
   while((error == E_OK) && p_rte)
   {
     if((p_rte->DestinationId == p_AggrEntry->NetIpAdr) &&
        (p_rte->IpMask == p_AggrEntry->SubnetMask))
     {
       dontFlushAggrSummary = TRUE;
       break;
     }
     error = HL_GetNext(p_AggrEntry->AgrRteHl, (void *)&p_rte, (void *)p_rte);
   }

   /* Flush the "aggregation" summary-LSA */
   if(!dontFlushAggrSummary)
     FlushAggregateSummary(p_ARO, p_AggrEntry);

   /* Reset variables */
   error = E_FAILED;
   p_rte = NULLP;

   /* Go over Aggregaqtion Rte hash list and originate LSA for each entry */
   error = HL_GetFirst(p_AggrEntry->AgrRteHl, (void *)&p_rte);
   while((error == E_OK) && p_rte)
   {
      err = HL_GetFirst(p_RTO->AroHl, (void *)&tmp_ARO);
      while ((err == E_OK) && tmp_ARO)
      {
         if(tmp_ARO != p_ARO)
         {
            /* 12.4.3 : intra-area-networks of backbone areas
             *          were not suppressed in transit areas
             *          So, no need to re-originate them */
            if(! ((tmp_ARO->TransitCapability) && 
                  (p_rte->AreaId == OSPF_BACKBONE_AREA_ID)) )
            {
               LsaReOriginate[S_IPNET_SUMMARY_LSA](tmp_ARO, (ulng)p_rte);
            }
         }
         err = HL_GetNext(p_RTO->AroHl, (void*)&tmp_ARO, (void*)tmp_ARO);
      }

      error = HL_GetNext(p_AggrEntry->AgrRteHl, (void*)&p_nextRte,(void*)p_rte);
      HL_Delete(p_AggrEntry->AgrRteHl, (byte*) &p_rte->DestinationId, NULL);
      p_rte = p_nextRte;
   }

   /* Reset aggregation cost */
   p_AggrEntry->LargestCost = p_AggrEntry->AggregateRte.Cost = -1;

   p_AggrEntry->LcRte = NULL;
   p_AggrEntry->alive = FALSE;

   return E_OK;
}

/*********************************************************************
 * @purpose       Originate the summary-LSAs contained by the 
 *                the address ranges (address aggregation) of
 *                the backbone.
 *                This procedure is called for an area when it transitions
 *                to a transit area from a non-transit-area.
 *                Earlier for a non-transit area, all the backbone summary routes
 *                were suppressed as per 12.4.3 by any area-ranges in the
 *                backbone.
 *                Once it becomes a transit area, we shouldn't suppress the
 *                contained summary routes into this area.
 *
 * @param  p_backbone   @b{(input)}  backbone area
 * @param  p_ARO        @b{(input)}  area object
 *
 * @returns                 E_OK success
 * @returns                 E_FAILED otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err DeliverSummariesOfBackbone(t_ARO *p_backbone, t_ARO *p_ARO)
{
   e_Err   er      = E_FAILED;
   e_Err   error   = E_FAILED;
   t_RoutingTableEntry *p_rte = NULLP;
   t_AgrEntry *p_AggrEntry;
   t_A_DbKey dbkey;
   t_A_DbEntry         *p_DbEntry = NULLP;
   Bool                 dontFlushAggrSummary  = FALSE;
   t_RTO *p_RTO;

   if(!p_ARO || !p_backbone || (p_backbone == p_ARO))
      return E_FAILED;

   p_RTO = (t_RTO *)p_ARO->RTO_Id;

   for (er = HL_GetFirst(p_backbone->AdrRangeHl, (void *)&p_AggrEntry);
        er == E_OK;
        er = HL_GetNext(p_backbone->AdrRangeHl, (void *)&p_AggrEntry, p_AggrEntry))
   {
     if(p_AggrEntry->LsdbType != AGGREGATE_SUMMARY_LINK)
        continue;

     dontFlushAggrSummary  = FALSE;
     error = HL_GetFirst(p_AggrEntry->AgrRteHl, (void *)&p_rte);
     while((error == E_OK) && p_rte)
     {
       if((p_rte->DestinationId == p_AggrEntry->NetIpAdr) &&
          (p_rte->IpMask == p_AggrEntry->SubnetMask))
         dontFlushAggrSummary = TRUE;

       LsaReOriginate[S_IPNET_SUMMARY_LSA](p_ARO, (ulng)p_rte);

       error = HL_GetNext(p_AggrEntry->AgrRteHl, (void *)&p_rte, (void *)p_rte);
     }

     /* Flush the "aggregation" summary-LSA */
     if((! dontFlushAggrSummary) &&
        (p_AggrEntry->AggregateEffect == AGGREGATE_ADVERTISE))
     {
       /* Set key for "aggregation" summary LSA searching in the data base */
       A_SET_4B(p_RTO->Cfg.RouterId,   dbkey.AdvertisingRouter);
       A_SET_4B(p_AggrEntry->NetIpAdr, dbkey.LsId);
       /* Find the "aggregation" summary-LSA; if found flush it out */
       if(AVLH_Find(p_ARO->NetSummaryLsaHl, (byte *)&dbkey,
                    (void *)&p_DbEntry,0) == E_OK)
         FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);
     }
   }

   return E_OK;
}



/*********************************************************************
 * @purpose            Sends all external-LSA describing destinations
 *                     belongs to the specific address range.
 *
 *
 * @param  *p_RTO       @b{(input)}  OSPF instance
 * @param  *p_AggrEntry @b{(input)} pointer to Address Aggregation entry
 *                                  being removed
 *
 * @returns             E_OK success
 * @returns             E_FAILED otherwise
 *
 * @notes    All these LSAs were suppressed early. This procedure is
 *           called when aggregation is beeing removed/deactivated,
 *           therefore all "hidden" destinations have to be advertised now.
 *
 * @end
 * ********************************************************************/
static e_Err DeliverAllExtSummaryOfRange(t_RTO *p_RTO, t_AgrEntry *p_AggrEntry)
{
   e_Err   error   = E_FAILED;
   t_RoutingTableEntry *p_rte = NULLP;
   t_RoutingTableEntry *p_nextRte  = NULLP;
   Bool                 dontFlushAggrSummary  = FALSE;
   t_S_AsExternalCfg    AsExtCfg;
   t_A_DbEntry         *p_DbEntry;
   t_S_AsExternalLsa   *p_NssaLsa;

   if(!p_RTO || !p_AggrEntry || 
      (p_AggrEntry->LsdbType != AGGREGATE_NSSA_EXTERNAL_LINK))
     return E_FAILED;

   if(p_AggrEntry->AggregateStatus == ROW_ACTIVE)
     return E_FAILED;

   if(!routerIsBorder(p_RTO))
      /* it is not area border router; do nothing */
      return E_OK;

   /* First determine if the existing external lsa needs to be flushed from the
   ** routing domain.  Flush the "aggregation" external-LSA if there is no 
   ** corresponding route entry that exactly matches the lsid and mask 
   */
   error = HL_GetFirst(p_AggrEntry->AgrRteHl, (void *)&p_rte);
   while((error == E_OK) && p_rte)
   {
     if((p_rte->DestinationId == p_AggrEntry->NetIpAdr) &&
        (p_rte->IpMask == p_AggrEntry->SubnetMask))
     {
       dontFlushAggrSummary = TRUE;
       break;
     }

     error = HL_GetNext(p_AggrEntry->AgrRteHl, (void *)&p_rte, (void *)p_rte);
   }

   /* Flush the "aggregation" external-LSA */
   if(!dontFlushAggrSummary)
     FlushExtAggregateSummary(p_RTO, p_AggrEntry);

   /* Reset variables */
   error = E_FAILED;
   p_rte = NULLP;

   /* Go over Aggregation Rte hash list and originate LSA for each entry */
   error = HL_GetFirst(p_AggrEntry->AgrRteHl, (void *)&p_rte);
   while((error == E_OK) && p_rte)
   {
     p_DbEntry = p_rte->p_DbEntry;
     p_NssaLsa = (t_S_AsExternalLsa *)p_DbEntry->p_Lsa;

     bzero((char *)&AsExtCfg, sizeof(t_S_AsExternalCfg));
     AsExtCfg.DestNetIpAdr  = p_rte->DestinationId & p_rte->IpMask;
     AsExtCfg.DestNetIpMask = p_rte->IpMask;
     AsExtCfg.PhyType       = OSPF_ETHERNET;
     AsExtCfg.IsMetricType2 = (p_rte->PathType == OSPF_NSSA_TYPE_2_EXT)? TRUE : FALSE;
     /* Pick up metric from T7 LSA rather than T7 route because we are setting forwarding
      * address. So calculating router will add his path cost to FA to T5 metric. Picking
      * up metric from T7 route would double count path cost from translator to ASBR. */
     AsExtCfg.MetricValue   = A_GET_3B(p_NssaLsa->Metric);
     AsExtCfg.ForwardingAdr = A_GET_4B(p_NssaLsa->ForwardingAddress);  /* forwarding via the local router */
     AsExtCfg.AsExtStatus   = ROW_CREATE_AND_GO;
     AsExtCfg.PrevEntry     = (t_Handle)RTE_ADDED;

     LsaReOriginate[S_AS_EXTERNAL_LSA](p_RTO, (ulng)&AsExtCfg);
     /* Get the next route in the aggregation */
     error = HL_GetNext(p_AggrEntry->AgrRteHl, (void*)&p_nextRte, (void*)p_rte);
     /* Delete this route entry from the aggregation */
     RemoveRouteFromT7Range(p_RTO, p_AggrEntry, p_rte);
     p_rte = p_nextRte;
   }

   /* Reset aggregation cost */
   p_AggrEntry->LargestCost = p_AggrEntry->AggregateRte.Cost = -1;

   p_AggrEntry->LcRte = NULL;
   p_AggrEntry->alive = FALSE;

   return E_OK;
}

/*********************************************************************
 * @purpose              Flushes "aggregate" summary-LSA out all
 *                       the areas.
 *
 *
 * @param p_ARO          @b{(input)}  area object where the address
 *                                     aggregation is declared
 * @param p_AggrEntry    @b{(input)}  pointer to Address Aggregation entry
 *
 * @returns              E_OK success
 * @returns              E_FAILED otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err FlushAggregateSummary(t_ARO *p_ARO, t_AgrEntry *p_AggrEntry)
{
   t_ARO        *tmp_ARO = NULLP;
   t_RTO        *p_RTO;
   e_Err         err     = E_FAILED;
   t_A_DbKey     dbkey;
   t_A_DbEntry  *p_DbEntry = NULLP;

   if(!p_ARO || !p_AggrEntry)
      return E_FAILED;

   p_RTO = (t_RTO *)p_ARO->RTO_Id;

   /* Set key for "aggregation" summary LSA searching in the data base */
   A_SET_4B(p_RTO->Cfg.RouterId,   dbkey.AdvertisingRouter);
   A_SET_4B(p_AggrEntry->NetIpAdr, dbkey.LsId);

   err = HL_GetFirst(p_RTO->AroHl, (void *)&tmp_ARO);
   while ((err == E_OK) && tmp_ARO)
   {
      if(tmp_ARO != p_ARO)
      {
         /* 12.4.3 : can't advertise address ranges of 
          *          backbone areas into transit areas */
         if(! ((tmp_ARO->TransitCapability) && 
               (p_ARO->AreaId == OSPF_BACKBONE_AREA_ID)) )
         {
            /* Find the "aggregation" summary-LSA; if found flush it out */
            if(AVLH_Find(tmp_ARO->NetSummaryLsaHl, (byte *)&dbkey,
                                          (void *)&p_DbEntry,0) == E_OK)
               FlushLsa((t_Handle)p_DbEntry, (ulng)tmp_ARO, 0);
         }
      }

      err = HL_GetNext(p_RTO->AroHl, (void*)&tmp_ARO, (void*)tmp_ARO);
   }

   return E_OK;
}




/*********************************************************************
 * @purpose              Flushes "aggregate" external LSAs out all
 *                       the areas.
 *
 * @param p_RTO          @b{(input)}  OSPF instance
 * @param p_AggrEntry    @b{(input)}  pointer to Address Aggregation
 *                                    entry
 *
 * @returns              E_OK success
 * @returns              E_FAILED otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err FlushExtAggregateSummary(t_RTO *p_RTO, t_AgrEntry *p_AggrEntry)
{
   t_A_DbKey     dbkey;
   t_A_DbEntry  *p_DbEntry = NULLP;

   if(!p_RTO || !p_AggrEntry)
      return E_FAILED;

   if(!routerIsBorder(p_RTO))
      /* it is not area border router; do nothing */
      return E_OK;

   /* Set key for "aggregation" external LSA searching in the data base */
   A_SET_4B(p_RTO->Cfg.RouterId,   dbkey.AdvertisingRouter);
   A_SET_4B(p_AggrEntry->NetIpAdr, dbkey.LsId);

   /* Find the "aggregation" external-LSA; if found flush it out */
   if(AVLH_Find(p_RTO->AsExternalLsaHl, (byte *)&dbkey, (void *)&p_DbEntry,0) == E_OK)
   {
     FlushAsLsa((t_Handle)p_DbEntry, (ulng)p_RTO);
   }

   return E_OK;
}




/*********************************************************************
 * @purpose                Originates "aggregate" summary-LSA to all
 *                         areas other than the area where the area range
 *                         is configured.
 *
 * @param p_ARO            @b{(input)}  area object where the address
 *                                      aggregation is declared
 * @param p_AggrEntry      @b{(input)}  pointer to Address Aggregation
 *                                      entry
 *
 * @returns                E_OK success
 * @returns                E_FAILED otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err OrigAggregateSummary(t_ARO *p_ARO, t_AgrEntry *p_AggrEntry)
{
   t_ARO        *tmp_ARO = NULLP;
   t_RTO        *p_RTO;
   e_Err         err     = E_FAILED;

   if(!p_ARO || !p_AggrEntry)
      return E_FAILED;

   p_RTO = (t_RTO *)p_ARO->RTO_Id;

   if(!routerIsBorder(p_RTO))
      /* it is not area border router, do nothing */
      return E_OK;

   if(p_AggrEntry->AggregateStatus != ROW_ACTIVE)
      return E_OK;

   if(!p_AggrEntry->alive || (p_AggrEntry->AggregateRte.Cost == -1))
      /* Aggregation range is still empty. Do nothing. */
      return E_OK;

   err = HL_GetFirst(p_RTO->AroHl, (void *)&tmp_ARO);
   while ((err == E_OK) && tmp_ARO)
   {
      if(tmp_ARO != p_ARO)
      {
         /* 12.4.3 : can't advertise address ranges of 
          *          backbone areas into transit areas */
         if(! ((tmp_ARO->TransitCapability) && 
               (p_ARO->AreaId == OSPF_BACKBONE_AREA_ID)) )
         {
            LsaReOriginate[S_IPNET_SUMMARY_LSA](tmp_ARO, (ulng)&p_AggrEntry->AggregateRte);
         }
      }
      err = HL_GetNext(p_RTO->AroHl, (void*)&tmp_ARO, (void*)tmp_ARO);
   }

   return E_OK;
}




/*********************************************************************
 * @purpose             Originates "aggregate" external LSA to all the
 *                      areas.
 *
 *
 * @param p_RTO          @b{(input)}  OSPF instance
 * @param p_AggrEntry    @b{(input)}  pointer to Address Aggregation entry
 *
 * @returns              E_OK success
 * @returns              E_FAILED otherwise
 *
 * @notes
 *                 This routine is used to summarize and translate Type-7
 *                 external routes into a single Type-5 advertisement
 *
 * @end
 * ********************************************************************/
static e_Err OrigExtAggregateSummary(t_RTO *p_RTO, t_AgrEntry *p_AggrEntry)
{
   t_S_AsExternalCfg    AsExtCfg;
   t_RoutingTableEntry  *p_Rte;

   if(!p_RTO || !p_AggrEntry ||
      (p_AggrEntry->LsdbType != AGGREGATE_NSSA_EXTERNAL_LINK))
      return E_FAILED;

   if(!routerIsBorder(p_RTO))
      /* it is not area border router; do nothing */
      return E_OK;

   if(p_AggrEntry->AggregateStatus != ROW_ACTIVE)
      return E_OK;

   if(!p_AggrEntry->alive || p_AggrEntry->AggregateRte.Cost == -1)
      /* Aggregation range is still empty. Do nothing. */
      /* "aggr" summary LSA will be sent later.        */
      return E_OK;

   p_Rte = (t_RoutingTableEntry *)p_AggrEntry->LcRte;

   bzero((char *)&AsExtCfg, sizeof(t_S_AsExternalCfg));
   AsExtCfg.DestNetIpAdr  = p_AggrEntry->NetIpAdr & p_AggrEntry->SubnetMask;
   AsExtCfg.DestNetIpMask = p_AggrEntry->SubnetMask;
   AsExtCfg.PhyType       = OSPF_ETHERNET;
   AsExtCfg.IsMetricType2 = (p_Rte->PathType == OSPF_NSSA_TYPE_2_EXT)? TRUE : FALSE;

   /* RFC 3101, Section 3.2(3) :
    * If the range's path type is 1 its metric is the highest cost amongst
    * these LSAs; if the range's path type is 2 its metric is the
    * highest Type-2 cost + 1 amongst these LSAs.
    */
   if(AsExtCfg.IsMetricType2 == TRUE)
   {
     AsExtCfg.MetricValue   = p_Rte->Type2Cost + 1;
   }
   else
   {
     AsExtCfg.MetricValue   = p_Rte->Cost;
   }
   AsExtCfg.ForwardingAdr = DefaultDestination;  /* forwarding via the local router */
   AsExtCfg.AsExtStatus   = ROW_CREATE_AND_GO;
   AsExtCfg.PrevEntry     = (t_Handle)RTE_ADDED;

   /* Flood the aggregate external LSA to all Type-5 capable areas */
   LsaReOriginate[S_AS_EXTERNAL_LSA](p_RTO, (ulng)&AsExtCfg);

   return E_OK;
}


/*********************************************************************
 * @purpose       Flush AS-Ext LSA Requests from NBO LsReq list.
 *
 *
 * @param rq      @b{(input)}  LSA request entry pointer
 * @param tmp     @b{(input)}  unused
 *
 * @returns       TRUE - skip non matching entry
 * @returns       FALSE - delete from the list and memory free
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool AsExtAndNssaRqFree(t_Handle rq, ulng tmp)
{
   t_LsReqEntry *p_LsReqEntry = (t_LsReqEntry *) rq;

   if((p_LsReqEntry->Lsa.LsType != S_AS_EXTERNAL_LSA) && 
      (p_LsReqEntry->Lsa.LsType != S_NSSA_LSA))
      return TRUE;

   return FALSE;
}

/*********************************************************************
 * @purpose  Find a parse entry on a db entry's connection list.
 *
 * @param   p_DbEntry  @b{(input)}  database entry for router or network LSA
 * @param   nbrDbEntry @b{(input)}  database entry for neighbor vertex 
 * @param   ifoId      @b{(input)}  interface to reach neighbor vertex
 * @param   linkType   @b{(input)}  type of link 
 *
 * @returns  parse entry if found. NULL otherwise.
 *
 * @notes   
 *
 * @end
 * ********************************************************************/
static t_ParseEntry *ParseEntryFind(t_A_DbEntry *p_DbEntry, 
                                    t_A_DbEntry *nbrDbEntry,
                                    ulng ifoId, e_LinkTypes linkType)
{
  t_ParseEntry *PrsEntry;

  if (!p_DbEntry || !nbrDbEntry)
    return NULL;

  PrsEntry = (t_ParseEntry*) p_DbEntry->ConnList;
  while (PrsEntry)
  {
    if ((PrsEntry->p_DbEntry == nbrDbEntry) &&
        (PrsEntry->ifoId == ifoId) &&
        (PrsEntry->linkType == linkType))
    {
      return PrsEntry;
    }
    PrsEntry = PrsEntry->next;
  }
  return NULL;
}

/*********************************************************************
 * @purpose  Verify that a given router or network LSA has a link
 *           to a given vertex. 
 *
 * @param   p_Lsa     @b{(input)}   body of adjacent router LSA
 * @param   lsid      @b{(input)}   link state ID of neighbor vertex. May 
 *                                  represent a router or network vertex. 
 * @param   linkType  @b{(input)}   type of link 
 *
 * @returns  E_OK if LSA has a link to the given vertex
 *           E_FAILED if link not found
 *
 * @notes   Only consider links with given type so that we don't mistake
 *          a link to a network vertex for a link to a router vertex, or
 *          vice versa.   
 *
 * @notes  The two way connectivity check only ensures that our neighbor has a 
 *         link back to us. As RFC 2328 Note 23 says, it is not necessary
 *         to find the matching half of the link under consideration. In fact,
 *         with parallel unnumbered p2p links, it is impossible to find the
 *         matching link. Because of this, the two way check is no longer
 *         done during preparsing. Instead it is deferred to the SPF itself.
 *
 *         Also verifies that neighbor database entry is still valid.
 *
 * @end
 * ********************************************************************/
static e_Err TwoWayConnectivityCheck(t_S_RouterLsa *p_Lsa, SP_IPADR lsid, 
                                     e_LinkTypes linkType)
{
  t_S_LinkDscr *p_Link;
  word linkNum;

  linkNum = A_GET_2B(p_Lsa->LinkNum);
  p_Link = (t_S_LinkDscr *) ((byte*)p_Lsa + sizeof(t_S_RouterLsa));
  while (linkNum--)
  {
    if ((p_Link->Type == linkType) &&
        (A_GET_4B(p_Link->LinkId) == lsid))
      return E_OK;

    p_Link = (t_S_LinkDscr*) (((byte*)p_Link) + 
                              sizeof(t_S_LinkDscr) + 
                              p_Link->TosNum * sizeof(t_S_Tos));
  }
  return E_FAILED;   
}

/*********************************************************************
 * @purpose      Parse the links in a router LSA. For each valid link 
 *               that passes two-way connectivity check, add an entry to
 *               the router's connection list. The SPF uses the connection
 *               list when building the shortest path tree.        
 *
 * @param   p_ARO        @b{(input)}  ARO Handler
 * @param   p_DbEntry    @b{(input)}  database entry for a router LSA
 * @param   nbrDbEntry   @b{(input)}  database entry for a vertex adjacent
 *                                    to the router. If NULL, create connection
 *                                    list entries for all neighbors. If not NULL
 *                                    only process links to this neighbor.
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void RtrLsaPreParsing(t_ARO *p_ARO, t_A_DbEntry *p_DbEntry, 
                             t_A_DbEntry *nbrDbEntry)
{
  t_RTO *p_RTO = (t_RTO*) p_ARO->RTO_Id;
   t_S_RouterLsa *p_RouterLsa;
   t_S_LinkDscr *p_Link;
   word link;
   t_A_DbEntry *p_NextDbEntry;
   t_ParseEntry *PrsEntry;
   word metric;
   ulng ifoId = 0;       /* interface identifier for parse entry */
   Bool validLink = FALSE;
   Bool selfOrig = FALSE;   /* TRUE if parsing our own router LSA */

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_PREPARSE)
   {
     char traceBuf[OSPF_MAX_TRACE_STR_LEN];
     L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
     L7_uchar8 rtrIdStr[OSAPI_INET_NTOA_BUF_SIZE];
     osapiInetNtoa(p_ARO->AreaId, areaIdStr);
     osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.LsId), rtrIdStr);
     sprintf(traceBuf, "RtrLsaPreParsing of router LSA %s in area %s", 
             rtrIdStr, areaIdStr);
     if (nbrDbEntry)
     {
       char traceBuf2[OSPF_MAX_TRACE_STR_LEN];
       L7_uchar8 nbrStr[OSAPI_INET_NTOA_BUF_SIZE];
       osapiInetNtoa(A_GET_4B(nbrDbEntry->Lsa.LsId), nbrStr);
       sprintf(traceBuf2, " for connections to neighbor %s.", nbrStr);
       strcat(traceBuf, traceBuf2);
     }
     RTO_TraceWrite(traceBuf);
   }

#if L7_OSPF_TE
   t_TeRtr  *p_TeRtr = NULL;

   if(p_ARO->TEsupport)
   {
      if(AVLH_Find(p_ARO->TeRtrHl, p_DbEntry->Lsa.LsId, (void**)&p_TeRtr,1) == E_OK)
         p_DbEntry->IFO_Id = p_TeRtr->p_DbEntry;
   }
#endif

   /* determine if this is a self-originated router LSA */
   if (p_RTO->Cfg.RouterId == A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter))
     selfOrig = TRUE;

   /* Browse all links of Router LSA */
   p_RouterLsa = (t_S_RouterLsa *) p_DbEntry->p_Lsa;
   link = A_GET_2B(p_RouterLsa->LinkNum);
   p_Link = (t_S_LinkDscr *) ((byte*)p_RouterLsa + sizeof(t_S_RouterLsa));
   while(link)
   {
      metric = 0;
      validLink = FALSE;
      if((p_Link->Type == S_POINT_TO_POINT) || (p_Link->Type == S_VIRTUAL_LINK))
      {
         /* For p2p links get next router LSA */
         if ((AVLH_Find(p_ARO->RouterLsaHl, p_Link->LinkId, (void**) 
                       &p_NextDbEntry, 1) == E_OK) && 
             L7_BIT_ISSET(p_NextDbEntry->dbFlags, IS_CUR) && 
             (A_GET_2B(p_NextDbEntry->Lsa.LsAge) < MaxAge))
         {
           if (!nbrDbEntry || (p_NextDbEntry == nbrDbEntry))
           {
             if (!nbrDbEntry)
             {
               /* Need to preparse neighbor's router LSA for connections to 
                * this router.  */
               RtrLsaPreParsing(p_ARO, p_NextDbEntry, p_DbEntry);
             }
  
             /* Validate the next DbEntry and check backward connectivity */
             if  (TwoWayConnectivityCheck((t_S_RouterLsa*) p_NextDbEntry->p_Lsa, 
                                          A_GET_4B(p_DbEntry->Lsa.LsId), 
                                          (e_LinkTypes) p_Link->Type) == E_OK)
             {
               if ((p_Link->Type == S_VIRTUAL_LINK) || (!selfOrig))
               {
                 metric = A_GET_2B(p_Link->Metric);
                 validLink = TRUE;
               }
               else
               {
                 /* p2p link in self-originated LSA. If in overload state, we 
                  * set link metrics to infinity. But we don't want to use 
                  * that value in our own routing calculations. */
                 t_IFO *p_IFO;
                 t_IfoHashKey ifoKey;
                 ifoKey.IfAddr = (SP_IPADR) A_GET_4B(p_Link->LinkData);  
                 ifoKey.aro = NULL;  
                 if (HL_FindFirst(p_ARO->IfoHl, (byte*)&ifoKey, (void *)&p_IFO) == E_OK)
                 {
                   metric = InterfaceMetric(p_IFO);;
                   validLink = TRUE;
                 }
               }
             }
           }
         }
      }
      else if(p_Link->Type == S_TRANSIT_NET)
      {
         /* For links to transit networks get next network LSA */
         if ((AVLH_Find(p_ARO->NetworkLsaHl, p_Link->LinkId, (void**) 
                        &p_NextDbEntry, 1) == E_OK) &&
              L7_BIT_ISSET(p_NextDbEntry->dbFlags, IS_CUR) && 
              (A_GET_2B(p_NextDbEntry->Lsa.LsAge) < MaxAge))
         {
           if (!nbrDbEntry || (p_NextDbEntry == nbrDbEntry))
           {
             if (!nbrDbEntry)
             {
               /* Need to re-parse the network LSA */
               NetLsaPreParsing(p_ARO, p_NextDbEntry, p_DbEntry);
             }

             /* Validate the next DbEntry and check backward connectivity */
             if (NetLsaCheckPresense(p_NextDbEntry, A_GET_4B(p_DbEntry->Lsa.LsId)))
             {
               if (!selfOrig)
               {
                 metric = A_GET_2B(p_Link->Metric);
                 validLink = TRUE;
               }
               else
               {
                 /* If in overload state, we 
                  * set link metrics to infinity. But we don't want to use 
                  * that value in our own routing calculations. */
                 t_IFO *p_IFO;
                 t_IfoHashKey ifoKey;
                 ifoKey.IfAddr = (SP_IPADR) A_GET_4B(p_Link->LinkData);  
                 ifoKey.aro = NULL;  
                 if (HL_FindFirst(p_ARO->IfoHl, (byte*)&ifoKey, (void *)&p_IFO) == E_OK)
                 {
                   metric = InterfaceMetric(p_IFO);;
                   validLink = TRUE;
                 }
               }
             }
           }
         }
      }

      if (validLink)
      {
        ifoId = A_GET_4B(p_Link->LinkData);
        /* Eliminate duplicates */
        if (ParseEntryFind(p_DbEntry, p_NextDbEntry, ifoId, 
                           (e_LinkTypes) p_Link->Type) == NULL)
        {
          if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_PREPARSE)
          {
            char traceBuf[OSPF_MAX_TRACE_STR_LEN];
            L7_uchar8 ifoIdStr[OSAPI_INET_NTOA_BUF_SIZE];
            L7_uchar8 rtrIdStr[OSAPI_INET_NTOA_BUF_SIZE];
            osapiInetNtoa(ifoId, ifoIdStr);
            osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.LsId), rtrIdStr);
            sprintf(traceBuf, "RtrLsaPreParsing of %s setting ifoId to %s for link type %d", 
                    rtrIdStr, ifoIdStr, p_Link->Type);
            RTO_TraceWrite(traceBuf);
          }
  
           /* Allocate the new preparsing entry and add it into DbEntry Preparsing list */
           if((PrsEntry = XX_Malloc(sizeof(t_ParseEntry))) == NULL)
           {
             /* Without this preparse entry, OSPF won't compute correct routes. 
              * So enter overload state. */
             if (!p_RTO->LsdbOverload)
             {
               EnterLsdbOverload(p_RTO);
             }
             return;
           }
           PrsEntry->next = PrsEntry->prev = NULL;
           PrsEntry->p_DbEntry = p_NextDbEntry;
           PrsEntry->metric = metric;
           PrsEntry->ifoId = ifoId;
           PrsEntry->linkType = (e_LinkTypes) p_Link->Type;
           XX_AddToDLList(PrsEntry, *(t_ParseEntry**)(&p_DbEntry->ConnList));

#if L7_OSPF_TE
         if(p_ARO->TEsupport)
         {
            t_TeLink *p_TeLink, *p_TeNextLink;
            t_TeLink key;
            t_TeRtr  *p_NextTeRtr;

            /* Update connectivity list of the TE link LSA of the router */
            /* described in p_DbEntry - add p_NextDbEntry ptr to the connList*/
            if (!p_TeRtr)
               goto next_link;

            memcpy(key.RouterId, p_DbEntry->Lsa.AdvertisingRouter, 4);
            memcpy(key.LinkId, p_NextDbEntry->Lsa.LsId, 4);
            if(AVLH_Find(p_ARO->TeLsaHl, key.RouterId,
                                        (void **)&p_TeLink,0) != E_OK)
               goto next_link;

            TeLinkLsaPreParsing(p_ARO, p_TeRtr, p_TeLink,p_NextDbEntry);

            /* If the connection is PTP update conn list of the TE link LSA*/
            /* of the remote router (described in p_NextDbEntry); Add ptr to*/
            /* p_DbEntry to connList of the remote rtr TE Link LSA          */
            if(p_NextDbEntry->Lsa.LsType == S_ROUTER_LSA)
            {
               memcpy(key.RouterId, p_NextDbEntry->Lsa.AdvertisingRouter, 4);
               memcpy(key.LinkId, p_DbEntry->Lsa.LsId, 4);
               if((AVLH_Find(p_ARO->TeLsaHl, key.RouterId,
                                     (void **)&p_TeNextLink,0) == E_OK) &&
                  (AVLH_Find(p_ARO->TeRtrHl, p_NextDbEntry->Lsa.LsId,
                                         (void**)&p_NextTeRtr,1) == E_OK))
                  TeLinkLsaPreParsing(p_ARO, p_NextTeRtr, p_TeNextLink,
                                                               p_DbEntry);
               else
                  TeLinkLsaParseDel(p_ARO, p_TeLink);
             }
         }  /* end of "if the area supports Traffic Engineering */
#endif
        }
      } /* End if valid link */

#if L7_OSPF_TE
next_link:
#endif

      p_Link = (t_S_LinkDscr*) (((byte*)p_Link) +
         sizeof(t_S_LinkDscr) + p_Link->TosNum*sizeof(t_S_Tos));
      link--;
   }
}

/*********************************************************************
 * @purpose      Parse the routers listed in a network LSA. For each valid  
 *               router attachement that passes two-way connectivity check, 
 *               add an entry to the network's connection list. The SPF uses 
 *               the connection list when building the shortest path tree.        
 *
 * @param   p_ARO        @b{(input)}  ARO Handler
 * @param   p_DbEntry    @b{(input)}  database entry for a network LSA
 * @param   nbrDbEntry   @b{(input)}  database entry for a router adjacent
 *                                    to the network. If NULL, create connection
 *                                    list entries for all neighbors. If not NULL
 *                                    only process links to this router.
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void NetLsaPreParsing(t_ARO *p_ARO, t_A_DbEntry *p_DbEntry,
                             t_A_DbEntry *nbrDbEntry)
{
   t_RTO *p_RTO = (t_RTO*) p_ARO->RTO_Id; 
   byte *p_Net;
   word link;
   t_A_DbEntry *p_NextDbEntry;
   t_ParseEntry *PrsEntry;

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_PREPARSE)
   {
     char traceBuf[OSPF_MAX_TRACE_STR_LEN];
     L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
     L7_uchar8 netIdStr[OSAPI_INET_NTOA_BUF_SIZE];
     osapiInetNtoa(p_ARO->AreaId, areaIdStr);
     osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.LsId), netIdStr);
     sprintf(traceBuf, "NetLsaPreParsing of network LSA %s in area %s", 
             netIdStr, areaIdStr);
     if (nbrDbEntry)
     {
       char traceBuf2[OSPF_MAX_TRACE_STR_LEN];
       L7_uchar8 nbrStr[OSAPI_INET_NTOA_BUF_SIZE];
       osapiInetNtoa(A_GET_4B(nbrDbEntry->Lsa.LsId), nbrStr);
       sprintf(traceBuf2, " for connections to neighbor %s.", nbrStr);
       strcat(traceBuf, traceBuf2);
     }
     RTO_TraceWrite(traceBuf);
   }

   /* Browse all links of Network LSA */
   p_Net = p_DbEntry->p_Lsa + 4;
   link =  (A_GET_2B(p_DbEntry->Lsa.Length) - sizeof(t_S_LsaHeader) - 4) / 4;
   while(link--)
   {
      if ((AVLH_Find(p_ARO->RouterLsaHl, p_Net, (void**) &p_NextDbEntry, 1) == E_OK) &&
          L7_BIT_ISSET(p_NextDbEntry->dbFlags, IS_CUR) && 
          (A_GET_2B(p_NextDbEntry->Lsa.LsAge) < MaxAge))
      {
        if (!nbrDbEntry || (p_NextDbEntry == nbrDbEntry))
        {
          if (!nbrDbEntry)
          {
            /* re-parse router's links to this network */
            RtrLsaPreParsing(p_ARO, p_NextDbEntry, p_DbEntry);
          }

          /* Validate the next DbEntry and check backward connectivity */
          if ((TwoWayConnectivityCheck((t_S_RouterLsa*) p_NextDbEntry->p_Lsa,  
                                       A_GET_4B(p_DbEntry->Lsa.LsId),
                                       S_TRANSIT_NET) == E_OK)) 
          {
            /* check if this is a duplicate parse entry */
            if (ParseEntryFind(p_DbEntry, p_NextDbEntry, A_GET_4B(p_Net), 0) == NULL)
            {
              if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_PREPARSE)
              {
                char traceBuf[OSPF_MAX_TRACE_STR_LEN];
                L7_uchar8 rtrIdStr[OSAPI_INET_NTOA_BUF_SIZE];
                L7_uchar8 netIdStr[OSAPI_INET_NTOA_BUF_SIZE];
                osapiInetNtoa(A_GET_4B(p_NextDbEntry->Lsa.LsId), rtrIdStr);
                osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.LsId), netIdStr);
                sprintf(traceBuf, "NetLsaPreParsing of %s adding link to router %s", 
                        netIdStr, rtrIdStr);
                RTO_TraceWrite(traceBuf);
              }
  
         /* Allocate the new preparsing entry and add it into DbEntry Preparsing list */
         if((PrsEntry = XX_Malloc(sizeof(t_ParseEntry))) == NULL)
         {
           if (!p_RTO->LsdbOverload)
           {
             EnterLsdbOverload(p_RTO);
           }
           return;
         }
         PrsEntry->next = PrsEntry->prev = NULL;
         PrsEntry->p_DbEntry = p_NextDbEntry;
         PrsEntry->metric = 0;
               PrsEntry->linkType = 0;
               PrsEntry->ifoId = A_GET_4B(p_Net);
               XX_AddToDLList(PrsEntry, *(t_ParseEntry**)(&p_DbEntry->ConnList));

#if L7_OSPF_TE
         /* Process TE Link LSA ConnList entry */
         if(p_ARO->TEsupport && p_NextDbEntry->IFO_Id)
         {
            t_TeLink *p_TeLink;
            t_TeLink key;

            memcpy(key.RouterId, p_NextDbEntry->Lsa.AdvertisingRouter, 4);
            memcpy(key.LinkId, p_DbEntry->Lsa.LsId, 4);
            if(AVLH_Find(p_ARO->TeLsaHl, key.RouterId, (void **)&p_TeLink,0) == E_OK)
            {
               /* Setup the new preparsing entry into TE Link LSA */
               TeLinkLsaPreParsing(p_ARO, NULL, p_TeLink, p_DbEntry);
            }
         }
#endif
            }  /* end not duplicate */
          }   /* end two-way connectivity */
        }
      }    /* end processing neighbor vertex */

      p_Net += 4;
   }    /* end iteration over net lsa links */
}

#if L7_OSPF_TE
static void TeLinkLsaPreParsing(t_ARO *p_ARO, t_TeRtr *rtr, t_TeLink *p_TeLink,
                                t_A_DbEntry *nextDb)
{
   t_TeRtr  *p_TeRtr;
   t_A_DbEntry *p_NextDbEntry;
   e_Err    e = E_FAILED;
   t_TeParseEntry *PrsEntry;
   t_S_LinkDscr *p_TmpLink;
   ulng          metric;
   e_LinkTypes   lktype;
   Bool          entry_exists = FALSE;

   if(!rtr)
   {
      /* Find match TE Router LSA */
      if(AVLH_Find(p_ARO->TeRtrHl, p_TeLink->RouterId,  (void**)&p_TeRtr,0) != E_OK)
         return;
   }
   else
   {
      p_TeRtr = rtr;
      e = E_OK;
   }

   /* Get TE Metric value and link type */
   if(GetTeLinkMetricAndType (p_TeLink->p_DbEntry, &lktype, &metric) != E_OK)
      return;

   if(!nextDb)
   {
      /* Find the "next" router or network LSA ("next" means is located at */
      /* the other end of the link) and check backward connectivity        */
      if(lktype == S_TRANSIT_NET)
      {
         e = AVLH_Find(p_ARO->NetworkLsaHl, p_TeLink->LinkId,
                                                  (void *)&p_NextDbEntry,1);
         /* Check if the found network LSA is "connected" to our rtr LSA. */
         /* If not we don't need to add it to the conn.list of the        */
         /* TE Link LSA. It will be done later when both LSAs (rtr and    */
         /* network) will be presented in the ConnList of each other      */
         if (e == E_OK)
            if(!NetLsaCheckPresense(p_NextDbEntry, A_GET_4B(p_TeLink->RouterId)))
               e = E_NOT_FOUND;
      }

      else
      {
         e = AVLH_Find(p_ARO->RouterLsaHl, p_TeLink->LinkId,
                                           (void *)&p_NextDbEntry,1);
         /* Check if the found "next" router is "connected" to our rtr LSA. */
         /* If not - don't add the "next" rtr LSA to the conn.list of the TE*/
         /* Link LSA. It will be done when both routers will be "connected" */
         /* to each other (i.e. each rtr Id will be presented among link    */
         /* descriptors of the other end rtr LSA )                          */
         if(e == E_OK)
         {
            p_TmpLink = RtrLsaCheckPresense((t_S_RouterLsa*) p_NextDbEntry->p_Lsa,
                                            A_GET_4B(p_TeLink->RouterId),0);
            if( !p_TmpLink ||
               (!(p_TmpLink->Type == S_POINT_TO_POINT) &&
                !(p_TmpLink->Type == S_VIRTUAL_LINK)))
               e = E_NOT_FOUND;
         }
      }
   }
   else
   {
      p_NextDbEntry = nextDb;
      e = E_OK;
   }

   if(e != E_OK)
      return;

   /* Allocate the new preparsing entry if it hasn't been allocated */
   /* and add it to TE Router LSA ConnList                          */
   for(PrsEntry = p_TeRtr->p_DbEntry->ConnList; PrsEntry; PrsEntry = PrsEntry->next)
   {
      if(PrsEntry->Link == p_TeLink)
      {
         entry_exists = TRUE;
         break;
      }
   }

   if (!entry_exists)
   {
      if((PrsEntry = XX_Malloc(sizeof(t_TeParseEntry))) == NULL)
         return;
      PrsEntry->next = PrsEntry->prev = NULL;
      PrsEntry->Link = p_TeLink;
      PrsEntry->metric = metric;
      XX_AddToDLList(PrsEntry, *(t_TeParseEntry**)(&p_TeRtr->p_DbEntry->ConnList));
   }

   /* Setup the preparsing entry into TE Link LSA */
   p_TeLink->p_DbEntry->ConnList = (t_Handle) p_NextDbEntry;

}

static void TeRtrLsaPreParsing(t_ARO *p_ARO, t_TeRtr *p_TeRtr)
{
   t_TeLink       *p_TeLink, key;
   t_A_DbEntry    *p_RtrDbEntry;
   t_S_RouterLsa  *p_RouterLsa = NULLP;
   word            linkNum     = 0;
   t_S_LinkDscr   *p_LinkDscr;
   t_A_DbEntry    *p_NextDbEntry = NULLP;
   e_Err           e           = E_FAILED;

   if(!p_TeRtr)
      return;

   /* Find match Router LSA */
   if (AVLH_Find(p_ARO->RouterLsaHl, p_TeRtr->RouterId,
                                    (void**)&p_RtrDbEntry,1) != E_OK)
      return;
   else
     /* Setup preparsing reference */
     p_RtrDbEntry->IFO_Id = p_TeRtr->p_DbEntry;


   /* Browse all links of Router LSA and found TE Link LSA for each of */
   /* them. Place ptr TE Link LSA to TE Router ConnList by             */
   /* TeLinkLsaPreParsing call.                                        */
   p_RouterLsa = (t_S_RouterLsa *) p_RtrDbEntry->p_Lsa;
   linkNum = A_GET_2B(p_RouterLsa->LinkNum);
   p_LinkDscr = (t_S_LinkDscr *) ((byte*)p_RouterLsa + sizeof(t_S_RouterLsa));
   while(linkNum)
   {
      e = E_FAILED;

      if((p_LinkDscr->Type == S_POINT_TO_POINT) ||
         (p_LinkDscr->Type == S_VIRTUAL_LINK))
         e = AVLH_Find(p_ARO->RouterLsaHl, p_LinkDscr->LinkId,
                                          (void**)&p_NextDbEntry,1);
      else if(p_LinkDscr->Type == S_TRANSIT_NET)
         e = AVLH_Find(p_ARO->NetworkLsaHl, p_LinkDscr->LinkId,
                                            (void**) &p_NextDbEntry,1);
      if(e == E_OK)
      {
         /* Find TE Link Lsa */
         memcpy(key.RouterId, p_RtrDbEntry->Lsa.AdvertisingRouter, 4);
         memcpy(key.LinkId, p_NextDbEntry->Lsa.LsId, 4);
         if(AVLH_Find(p_ARO->TeLsaHl,key.RouterId,(void **)&p_TeLink,0) == E_OK)
            TeLinkLsaPreParsing(p_ARO, p_TeRtr, p_TeLink, p_NextDbEntry);
      }

      p_LinkDscr = (t_S_LinkDscr*) (((byte*)p_LinkDscr) +
         sizeof(t_S_LinkDscr) + p_LinkDscr->TosNum*sizeof(t_S_Tos));
      linkNum--;
   }
}

/* Procedure gets Link type value and TE Metric value from   */
/* TE Link LSA                                               */
static e_Err GetTeLinkMetricAndType (t_A_DbEntry *p_DbEntry,
                                     e_LinkTypes *LkType, ulng *TeMetric)
{
   byte     *p_Tlv, *p_Tlv1,*p_Tlv2;

   /* Find TE Link TLV */
   p_Tlv = FindTlvInData(p_DbEntry->p_Lsa,
         (word)(A_GET_2B(p_DbEntry->Lsa.Length)-sizeof(t_S_LsaHeader)), TE_TLV_LINK);

   if(!p_Tlv)
      return E_FAILED;

   /* Find Link Id sub-TLV */
   p_Tlv1 = FindTlvInData(p_Tlv+4, (word)A_GET_2B(p_Tlv+2), TELK_ID);
   if(!p_Tlv1)
      return E_FAILED;

   p_Tlv1 = FindTlvInData(p_Tlv+4, (word)A_GET_2B(p_Tlv+2), TELK_TYPE);
   p_Tlv2 = FindTlvInData(p_Tlv+4, (word)A_GET_2B(p_Tlv+2), TELK_METRIC);

   if(!p_Tlv1)
      return E_FAILED;

   /* fill the output prameters */
   *LkType   =  *(p_Tlv1+4);
   *TeMetric =  A_GET_4B(p_Tlv2+4);

   return E_OK;

}


static void TeLinkLsaParseDel(t_ARO *p_ARO, t_TeLink *p_Link)
{
   t_TeRtr  *p_TeRtr;
   t_TeParseEntry *PrsEntry, *tmp;

   p_Link->p_DbEntry->ConnList = NULL;

   /* Find match TE Router LSA */
   if(AVLH_Find(p_ARO->TeRtrHl, p_Link->RouterId,  (void**)&p_TeRtr,0) != E_OK)
      return;

   for(PrsEntry = p_TeRtr->p_DbEntry->ConnList; PrsEntry; )
   {
      tmp = PrsEntry->next;
      if(PrsEntry->Link == p_Link)
      {
         XX_DelFromDLList(PrsEntry, p_TeRtr->p_DbEntry->ConnList);
         XX_Free(PrsEntry);
         return;
      }
      PrsEntry = tmp;
   }
}

static void TeRtrLsaParseDel(t_ARO *p_ARO, t_A_DbEntry *p_DbEntry)
{
   t_TeParseEntry *PrsEntry, *tmp;
   t_A_DbEntry *tmpDbEntry;
   e_Err er;

   /* Find match Router LSA */
   er = AVLH_Find(p_ARO->RouterLsaHl, p_DbEntry->Lsa.AdvertisingRouter,  (void**)&tmpDbEntry,1);
   /* Remove preparsing reference from the router LSA */
   if(er == E_OK)
      tmpDbEntry->IFO_Id = NULL;

   /* Free memory allocated for TE RTR LSA ConnList entries */
   for(PrsEntry = p_DbEntry->ConnList; PrsEntry; )
   {
      tmp = PrsEntry->next;
      XX_Free(PrsEntry);
      PrsEntry = tmp;
   }
}

#endif /* L7_OSPF_TE */

void ospfDebugConnListShow(ulng areaId, ulng linkStateId, e_S_LScodes lsaType)
{
  t_RTO *p_RTO = RTO_List;
  t_ARO *p_ARO;
  t_A_DbEntry *p_DbEntry;
  t_ParseEntry *PrsEntry;
  e_Err e;
  L7_uchar8 nbrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 ifoIdStr[OSAPI_INET_NTOA_BUF_SIZE];
  ulng lsid = osapiHtonl(linkStateId);

  if ((lsaType != S_ROUTER_LSA) && (lsaType != S_NETWORK_LSA))
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                   "\nBad LSA type. Use 1 or 2.");
    return;
  }

  if (p_RTO == NULL)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP, 
                   "\nCan't find RTO.");
    return;
  }

  e = HL_FindFirst(p_RTO->AroHl, (byte*)&areaId, (void**)&p_ARO);
  if (e != E_OK || p_ARO == NULL)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  
                   "\nCould not find area %u", areaId);
    return;
  }

  if (lsaType == S_ROUTER_LSA)
    e = AVLH_Find(p_ARO->RouterLsaHl, (byte*) &lsid, (void**) &p_DbEntry, 1);
  else
    e = AVLH_Find(p_ARO->NetworkLsaHl, (byte*) &lsid, (void**) &p_DbEntry, 1);

  if (e != E_OK)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  
                   "\nCould not find database entry");
    return;
  }
  PrsEntry = (t_ParseEntry *)p_DbEntry->ConnList;
  if (PrsEntry)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  
                   "\nLink Type    Neighbor      Local Interface      Metric");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  
                   "\nNo connections");
  }
  while(PrsEntry)
  {
    osapiInetNtoa(A_GET_4B(PrsEntry->p_DbEntry->Lsa.LsId), nbrStr);
    osapiInetNtoa(PrsEntry->ifoId, ifoIdStr);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  
                   "\n%12d %12s %12s %12d",
                   PrsEntry->linkType, nbrStr, ifoIdStr, PrsEntry->metric);

    PrsEntry = PrsEntry->next;
  }
}

void LsaParseDel(t_ARO *p_ARO, t_A_DbEntry *p_DbEntry)
{
  t_RTO *p_RTO = (t_RTO*) p_ARO->RTO_Id;
   t_ParseEntry *PrsEntry = (t_ParseEntry *)p_DbEntry->ConnList;
   t_ParseEntry *PrsEntry1, *tmp;
   t_ParseEntry *PrsEntryNext = NULL;

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_SPF_PREPARSE)
   {
     char traceBuf[OSPF_MAX_TRACE_STR_LEN];
     L7_uchar8 lsidStr[OSAPI_INET_NTOA_BUF_SIZE];
     osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.LsId), lsidStr);
     sprintf(traceBuf, "Clearing connection list for area %lu vertex %s", 
             (ulng) p_ARO->AreaId, lsidStr);
     RTO_TraceWrite(traceBuf);
   }

   while(PrsEntry)
   {
      PrsEntry1 = PrsEntry->p_DbEntry->ConnList;
      while(PrsEntry1)
      {
         PrsEntryNext = PrsEntry1->next;
         if(PrsEntry1->p_DbEntry == p_DbEntry)
         {
#if L7_OSPF_TE
            if(PrsEntry->p_DbEntry->IFO_Id)
            {
               t_TeParseEntry *PrsEntry2;

               /* TE part for TE Router LSA */

               /* Remove preparsing reference from remote TE Link LSA */
               for(PrsEntry2 = ((t_A_DbEntry *)PrsEntry->p_DbEntry->IFO_Id)->ConnList;
                  PrsEntry2; PrsEntry2 = PrsEntry2->next)
                  {
                     if(PrsEntry2->Link->p_DbEntry->ConnList == p_DbEntry)
                        PrsEntry2->Link->p_DbEntry->ConnList = NULL;
                  }
            }
#endif
            XX_DelFromDLList(PrsEntry1, PrsEntry->p_DbEntry->ConnList);
            XX_Free(PrsEntry1);
         }
         PrsEntry1 = PrsEntryNext;
      }

      tmp = PrsEntry->next;
      XX_Free(PrsEntry);
      PrsEntry = tmp;
   }
   p_DbEntry->ConnList = NULL;
}


/* ARO_Delete unpack procedure  */
/* Used when the area object is being deleted from IFO_Delete procedure. */
/* In order to prevent corruption into rtb calculation process which     */
/* may run in other thread, ARO_DElete is to be called from rtb thread   */
static e_Err AroDeleteOnRtbThread (void *p_Info)
{
   t_XXCallInfo *callInfo  = (t_XXCallInfo *) p_Info;
   t_ARO        *p_ARO     = (t_ARO*)PACKET_GET(callInfo,0);
   ulng          flag      = (ulng)PACKET_GET(callInfo,1);
   t_RTO        *p_RTO     = (t_RTO *)p_ARO->RTO_Id;

   /* Check if RTO Object is valid */
   if(!p_RTO->Taken)
      return E_OK;

   ARO_Delete(p_ARO, flag);

   return E_OK;

}

/*********************************************************************
 * @purpose     Updates age of the LSA into the data base
 *
 *
 * @param       p_DbEntry     @b{(input)}  pointer to data base entry
 *
 * @returns     E_FAILED - if data base entry is NULLP
 * @returns     E_OK     - the age was updated
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err UpdateAgeOfDbEntry (t_A_DbEntry *p_DbEntry)
{
   if (!p_DbEntry)
      return E_FAILED;

   /* If the age == MaxAge, it may be premature aging - do nothing */
   if(A_GET_2B(p_DbEntry->Lsa.LsAge) >= MaxAge)
      return E_OK;

    A_SET_2B(p_DbEntry->InitAge + GET_INTERVAL(p_DbEntry->OrgTime),
                     p_DbEntry->Lsa.LsAge);

    /* Lsa cannot be older than the MaxAge */


   /* Check for (MaxAge-1).  This was added to fix ANVL OSPF test 24.6
   ** The test expects us to age out an lsa advertised at age = 3599 
   ** within 2 seconds.  Due to the large granularity of the aging timer
   ** this will not happen, so this change will force entries that are
   ** almost MaxAge to be regarded as MaxAge and not used in the route
   ** table calculations.
   */
   if(A_GET_2B(p_DbEntry->Lsa.LsAge) >= (MaxAge-1))
      A_SET_2B(MaxAge, p_DbEntry->Lsa.LsAge);


    return E_OK;
}

/* NSSA Functions */

/* If an elected translator determines its services are no longer required, it
** continues to perform its translation duties for the additional time
** interval defined by a new area configuration parameter, TranslatorStabilityInterval.  
** This minimizes excessive flushing of translated Type-7 LSAs and provides for a 
** more stable translator transition.  The default value for the TranslatorStabilityInterval
** parameter has been defined as 40 seconds 
*/

/*********************************************************************
 * @purpose     Take actions required on expiration of the NSSA translator
 *              stability timer. This timer fires when the router has been
 *              deposed from being the elected translator.
 *
 *
 * @param       p_Info  @b{(input)}  Contains area ID of NSSA where router
 *                                   is no longer translator.
 *
 * @returns     E_OK   
 *
 * @notes       Sets the translator state to DISABLED. Flushes any
 *              translated T5 LSAs, either individual or aggregates.
 *
 * @end
 * ********************************************************************/
e_Err TranslatorStabilityTimerExp( t_Handle Id, t_Handle TimerId, word Flag )
{
  /* Assume for now only one RTO object */
  t_RTO *p_RTO = RTO_List;
  t_ARO *p_ARO = (t_ARO*) Id;

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_NSSA)
  {
      char traceBuf[OSPF_MAX_TRACE_STR_LEN];
      L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(p_ARO->AreaId, areaIdStr);          
      sprintf(traceBuf, "Translator stability interval expired. "
              "Setting NSSA translator state to DISABLED for area %s.", 
              areaIdStr);
      RTO_TraceWrite(traceBuf);
  }

  NssaTranslatorStateSet(p_ARO, NSSA_TRANS_STATE_DISABLED);

  return E_OK;
}

/*********************************************************************
 * @purpose     Given a type 7 LSA, determine whether it should be 
 *              translated. 
 *
 *
 * @param  p_ARO      @b{(input)}  The NSSA where the T7 LSA originated
 * @param  p_DbEntry  @b{(input)}  database entry for the T7
 * @param  p_AdrRange @b{(input/output)}  handle to a T7 address range. If
 *                        non-NULL, this function assumes this range is the 
 *                        best match range for the T7 LSA and checks whether
 *                        the range is an advertise or no-advertise range. If
 *                        NULL, this function finds the best matching range.
 *
 * @returns     E_OK if the T7 should be translated
 *              E_IN_MATCH if the T7 matches an "advertise" T7 range
 *              E_DISCARD if the T7 is contained in a "no-advertise" T7 range,
 *                        or if the T7 is not eligible to be translated. 
 *              E_BADPARM if the parameters used to call this function are invalid  
 *
 * @notes       A T7 LSA is not eligible to be translated if the p bit is not set,
 *              or if its forwarding address is not set.  
 *
 * @end
 * ********************************************************************/
static e_Err NssaLsaShouldTranslate(t_ARO *p_ARO, t_A_DbEntry *p_DbEntry, 
                                    t_AgrEntry **p_AdrRange)
{
  t_AgrEntry  *p_Adr;
  t_AgrEntry *matchingRange = NULL;
  t_S_AsExternalLsa *p_NssaLsa;
  e_Err er;

  if (!p_ARO || !p_DbEntry || (p_DbEntry->Lsa.LsType != S_NSSA_LSA))
    return E_BADPARM;

  p_NssaLsa = (t_S_AsExternalLsa *)p_DbEntry->p_Lsa;
  if (!(p_DbEntry->Lsa.Options & OSPF_OPT_P_BIT) || 
      (A_GET_4B(p_NssaLsa->ForwardingAddress) == DefaultDestination))
    return E_DISCARD;

  if (*p_AdrRange == NULL)
  {
    /* Browse address range list to find the most specific configured address range 
    ** entry that subsumes this lsa 
    */
    er = HL_GetFirst(p_ARO->AdrRangeHl, (void *)&p_Adr);
    while (er == E_OK)
    {
      /* If NSSA LSA's LSID & mask is subsumed by the T7 range and 
      ** if the advertise status is not set to advertise this aggregation, 
      ** this lsa is not to be included in the translation process.
      */
      if ((p_Adr->AggregateStatus == ROW_ACTIVE) &&
          (p_Adr->LsdbType == AGGREGATE_NSSA_EXTERNAL_LINK) &&
          (A_GET_4B(p_NssaLsa->NetworkMask) >= p_Adr->SubnetMask) &&
          (A_GET_4B(p_DbEntry->Lsa.LsId) & p_Adr->SubnetMask) == 
          (p_Adr->NetIpAdr & p_Adr->SubnetMask))
      {
        if (!matchingRange || (p_Adr->SubnetMask > matchingRange->SubnetMask))
          matchingRange = p_Adr;
      }
      er = HL_GetNext(p_ARO->AdrRangeHl, (void *)&p_Adr, p_Adr);
    } /* End of browsing address range list */
    *p_AdrRange = matchingRange;
  }

  if (*p_AdrRange)
  { 
    if ((*p_AdrRange)->AggregateEffect == AGGREGATE_ADVERTISE)
      return E_IN_MATCH;
    else
      return E_DISCARD;
  }

  /* This LSA is not contained in a T7 range and should be translated. */
  return E_OK;
}


/*********************************************************************
 * @purpose        Used to translate Type-7 into Type-5 LSAs
 *                 Serves the  browsing AVL_Browse function
 *
 *
 * @param rte      @b{(input)}  Routing table entry
 * @param RtbId    @b{(input)}  Router table object handler id
 *
 * @returns        TRUE  Always returns true since we don't want to
 *                 delete the routing table entry, just analyze if it
 *                 needs to be translated.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static Bool NssaTranslate(byte *rte, ulng p_AggrRange)
{
  t_RoutingTableEntry *p_Rte = (t_RoutingTableEntry *) rte;
  t_A_DbEntry         *p_DbEntry = NULLP;
  t_S_AsExternalLsa         *p_NssaLsa = NULLP;
  t_RTO               *p_RTO = NULLP;
  t_S_AsExternalCfg   AsExtCfg;
  t_AgrEntry          *p_AdrRange = NULLP;
  t_ARO               *p_ARO = NULLP;
  e_Err shouldTranslate;

  if(!p_Rte || !p_Rte->p_DbEntry)
    return TRUE;

  p_ARO = p_Rte->p_DbEntry->ARO_Id;
  p_DbEntry = p_Rte->p_DbEntry;
  if(p_AggrRange)
    p_AdrRange = (t_AgrEntry *)p_AggrRange;

  /* If the calculating router is not an NSSA border router translator, then this
  ** translation algorithm should be skipped
  */
  if((!p_ARO) || (p_ARO->NSSATranslatorState == NSSA_TRANS_STATE_DISABLED) ||
     (p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA))
    return TRUE;

  p_RTO = (t_RTO *)p_ARO->RTO_Id;
  /* (1) If the Type-7 LSA has the P-bit clear, or its forwarding
  ** address is set to 0.0.0.0, or the most specific Type-7 address
  ** range that subsumes the LSA's network has DoNotAdvetise status,
  ** then do nothing with this type-7 LSA and consider the next one
  ** in the list.  Otherwise term the LSA as translatable and proceed
  ** with step (2)
  */
  shouldTranslate = NssaLsaShouldTranslate(p_ARO, p_DbEntry, &p_AdrRange);
  if ((shouldTranslate == E_DISCARD) || (shouldTranslate == E_BADPARM))
    return TRUE;

  p_NssaLsa = (t_S_AsExternalLsa *)p_DbEntry->p_Lsa;

  /* (2) check other conditions for translation. */
  if(CheckNssaLsaOrg(p_DbEntry, p_ARO) == TRUE)
  {
    memset((char *)&AsExtCfg, 0, sizeof(t_S_AsExternalCfg));
    /* (3): If this Type-7 address range has the same [address, mask] pair as 
     * the LSA's network and no other translatable Type-7 LSA with a different 
     * network best matches this range then flag the LSA as not contained in 
     * any explicitly configured Type-7 address range. */
    if(p_AdrRange && NssaLsaExactAndOnlyMatch(p_AdrRange, p_Rte) == FALSE)
    {
      /* advertise the range */
      OrigExtAggregateSummary(p_RTO, p_AdrRange);
    }
    else
    {
      /* advertise individual route */
      AsExtCfg.DestNetIpAdr  = p_Rte->DestinationId & p_Rte->IpMask;
      AsExtCfg.DestNetIpMask = p_Rte->IpMask;
      AsExtCfg.ForwardingAdr = A_GET_4B(p_NssaLsa->ForwardingAddress);
      /* Pick up metric from T7 LSA rather than T7 route because we are setting forwarding
      * address. So calculating router will add his path cost to FA to T5 metric. Picking
      * up metric from T7 route would double count path cost from translator to ASBR. */
      AsExtCfg.MetricValue   = A_GET_3B(p_NssaLsa->Metric);
      AsExtCfg.PhyType       = OSPF_ETHERNET;
      AsExtCfg.IsMetricType2 = (p_NssaLsa->Flags & S_AS_EXT_E_FLAG)? TRUE : FALSE;
      AsExtCfg.AsExtStatus   = ROW_CREATE_AND_GO;
      AsExtCfg.PrevEntry     = (t_Handle)RTE_ADDED;

      if(routerIsBorder(p_RTO))
        LsaReOriginate[S_AS_EXTERNAL_LSA](p_RTO, (ulng)&AsExtCfg);
    }
  }

  return TRUE;
}


/* Checks functional equivalence of type-7 & type-5 lsas.  parm 1 = type7
** parm 2 = type-5
*/

/* (2) If the Type-7 LSA is not contained in any explicitly configured
** type-7 address range and the calculating router has the highest 
** router id amongst NSSA translators that have originated a functionally 
** equivalent Type-5 LSA (i.e. same destination, cost and non-zero
** forwarding address) and that are reachable over area 0 & the NSSA     
*/
Bool CheckLsaEquivalent(t_A_DbEntry *p_DbEntry, t_A_DbEntry *p_AsExtEntry)
{
  t_S_AsExternalLsa       *p_NssaLsa;
  t_S_AsExternalLsa *p_AsExtLsa;
  Bool              nssaMetric2, asExtMetric2;
  
  /* Sanity check for null pointers */
  if(!p_DbEntry || !p_AsExtEntry || p_DbEntry->Lsa.LsType != S_NSSA_LSA ||
     p_AsExtEntry->Lsa.LsType != S_AS_EXTERNAL_LSA)
    return FALSE;

  p_NssaLsa = (t_S_AsExternalLsa *)p_DbEntry->p_Lsa;
  p_AsExtLsa = (t_S_AsExternalLsa *)p_AsExtEntry->p_Lsa;

  /* Check if the lsas are to the same destination */
  if((A_GET_4B(p_DbEntry->Lsa.LsId) & A_GET_4B(p_NssaLsa->NetworkMask)) != 
     (A_GET_4B(p_AsExtEntry->Lsa.LsId) & A_GET_4B(p_AsExtLsa->NetworkMask)))
    return FALSE;

  /* Test for zero forwarding address was carried out in NssaLsaVerify.  Check
  ** if the non-zero forwarding addresses are the same
  */
  if(A_GET_4B(p_NssaLsa->ForwardingAddress) != A_GET_4B(p_AsExtLsa->ForwardingAddress))
    return FALSE;

  /* Compare cost to the destination */
  nssaMetric2  = (p_NssaLsa->Flags & S_AS_EXT_E_FLAG)? TRUE : FALSE;
  asExtMetric2 = (p_AsExtLsa->Flags & S_AS_EXT_E_FLAG)? TRUE : FALSE;

  if(nssaMetric2 != asExtMetric2)
    return FALSE;

  if(A_GET_3B(p_NssaLsa->Metric) != A_GET_3B(p_AsExtLsa->Metric))
    return FALSE;

  /* If we reached here these lsas are equivalent */
  return TRUE;
}

/*********************************************************************
 * @purpose        Check if the destination of a T7 route exactly matches
 *                 a T7 address range, and if the T7 route is the only 
 *                 route contained within the range.
 *
 * @param p_AdrRange   @b{(input)}  T7 address range
 * @param p_RteEntry   @b{(input)}  Route matching the given range
 *
 * @returns  TRUE if destiantion is exact match and route is only matching route
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static Bool NssaLsaExactAndOnlyMatch(t_AgrEntry *p_AdrRange, 
                                     t_RoutingTableEntry *p_RteEntry)
{
  t_RoutingTableEntry *p_rte;
  e_Err               err = E_FAILED;
  word numRoutes = 0;

  /* Sanity checks */
  if(!p_AdrRange || !p_RteEntry || !p_RteEntry->p_DbEntry)
    return FALSE;

  /* Check if the network and the netmask of the LSA exactly match
  ** those of the address range
  */
  if((p_RteEntry->DestinationId != p_AdrRange->NetIpAdr) || 
     (p_RteEntry->IpMask != p_AdrRange->SubnetMask))
    return FALSE;

  /* Check if there are any other networks in this address range */
  if ((HL_GetEntriesNmb(p_AdrRange->AgrRteHl, &numRoutes) != E_OK) ||
      (numRoutes != 1))
    return FALSE;

  /* Verify that the one route in the range is the route we're interested in */
  err = HL_GetFirst(p_AdrRange->AgrRteHl, (void *)&p_rte);
  if ((p_RteEntry->DestinationId != p_rte->DestinationId) || 
      (p_RteEntry->IpMask != p_rte->IpMask))
  {
    /* Something's not right. */
    L7_uchar8 rangeAddr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 rangeMask[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 routeAddr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 routeMask[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_AdrRange->NetIpAdr, rangeAddr);
    osapiInetNtoa(p_AdrRange->SubnetMask, rangeMask);
    osapiInetNtoa(p_RteEntry->DestinationId, routeAddr);
    osapiInetNtoa(p_RteEntry->IpMask, routeMask);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
           "Route %s/%s not contained in T7 range %s/%s.",
            routeAddr, routeMask, rangeAddr, rangeMask);
    return FALSE;
  }

  return TRUE;
}

/* Check if the calculating router has the highest router id
** amongst NSSA translators that have originated a functionally 
** equivalent Type-5 LSA (i.e. same destination, cost and non-zero
** forwarding address) and that are reachable over area 0 & the NSSA     
*/
static Bool CheckNssaLsaOrg(t_A_DbEntry *p_DbEntry, t_ARO *p_ARO)
{
  SP_IPADR      DestNetIpAdr;
  t_A_DbEntry   *p_AsExtEntry, *p_NssaEntry;
  t_S_RouterLsa *p_AsbrRtrLsa;
  e_Err         e;
  t_RTO         *p_RTO;
  t_RTB         *p_RTB;
  t_VpnCos      VpnCos = 0;
  t_A_DbKey     askey;
  byte          b_flag = (S_BORDER_FLAG | S_EXTERNAL_FLAG);
  t_S_AsExternalLsa   *p_AsExtLsa;
  t_S_AsExternalLsa   *p_NssaLsa;
  t_RoutingTableKey   rtbkey;
  t_RoutingTableEntry *p_AsBr = NULLP;

  /* Sanity check for null pointers */
  if(!p_DbEntry || p_DbEntry->Lsa.LsType != S_NSSA_LSA || !p_ARO)
    return FALSE;

  p_RTO = (t_RTO *)p_ARO->RTO_Id;
  DestNetIpAdr = A_GET_4B(p_DbEntry->Lsa.LsId);

  /* Find the relevant routing table */
  if(HL_FindFirst(p_RTO->RtbHl, (byte *)&VpnCos, (void *)&p_RTB) != E_OK)
     return FALSE;

  /* If the advertising ASBR does not have a valid database entry, we 
  ** will not translate this LSA
  */
  rtbkey.DstAdr = A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter);
  rtbkey.Prefix = 0L;

  e = AVL_Find(p_RTB->RtbRtBt, FALSE, (byte*)&rtbkey,  (void*)&p_AsBr);
  if((e != E_OK))
    return FALSE;

  /* The default Type-7 LSA originated by an NSSA Border Router is never
  ** translated into a Type-5
  */
  p_AsbrRtrLsa = (t_S_RouterLsa *)p_AsBr->p_DbEntry->p_Lsa;
  if((A_GET_4B(p_DbEntry->Lsa.LsId) == DefaultDestination) && 
     (p_AsBr->p_DbEntry->Lsa.Options & OSPF_OPT_P_BIT) && 
     (p_AsbrRtrLsa->Flags & S_BORDER_FLAG))
    return FALSE;

  /* Iterate through the tree of external lsas to find an equivalent
  ** Type-5 LSA for the Type-7 LSA being processed
  */
  e = AVLH_GetFirst(p_RTO->AsExternalLsaHl, (void *)&p_AsExtEntry);
  while(e == E_OK)
  {
    if((DestNetIpAdr != A_GET_4B(p_AsExtEntry->Lsa.LsId)) ||
       (A_GET_4B(p_AsExtEntry->Lsa.AdvertisingRouter) == p_RTO->Cfg.RouterId))
      goto nextAsExtLsa;

    /* Check if the found Type-5 Lsa is equivalent to the Type-7 Lsa */
    if(CheckLsaEquivalent(p_DbEntry, p_AsExtEntry) == TRUE)
    {
      /* The calculating router has the highest router id amongst the
      ** NSSA translators that have originated functionally equivalent
      ** Type-5 LSAs.  Find and verify the status of the NSSA translator
      ** that originated this equivalent Type-5 LSA.
      */
      rtbkey.DstAdr = A_GET_4B(p_AsExtEntry->Lsa.AdvertisingRouter);
      rtbkey.Prefix = 0L;

      e = AVL_Find(p_RTB->RtbRtBt, FALSE, (byte*)&rtbkey,  (void*)&p_AsBr);
      if((e != E_OK))
        goto nextAsExtLsa;

      p_AsbrRtrLsa = (t_S_RouterLsa *)p_AsBr->p_DbEntry->p_Lsa;
      if(!p_AsbrRtrLsa || !(p_AsBr->p_DbEntry->Lsa.Options & OSPF_OPT_P_BIT) ||
         ((p_AsbrRtrLsa->Flags & b_flag) != b_flag))
        goto nextAsExtLsa;

      if((p_AsBr->PathType == OSPF_INTRA_AREA) && (p_AsBr->BackboneReachable == TRUE) && 
         (p_AsBr->DestinationId > p_RTO->Cfg.RouterId))
         return FALSE;
    }

nextAsExtLsa:
    e = AVLH_GetNext(p_RTO->AsExternalLsaHl,(void *)&p_DbEntry, p_DbEntry);
  }

  /* If we reached here the Lsa needs to be originated provided the following
  ** conditions are met.
  */
  A_SET_4B(p_RTO->Cfg.RouterId, askey.AdvertisingRouter);
  memcpy(askey.LsId, p_DbEntry->Lsa.LsId, 4);

  /* A Type-5 LSA should be generated if there is currently no Type-5 LSA
  ** originating from this router corresponding to the Type-7 LSA's
  ** network
  */
  p_AsExtEntry = NULL;
  if ((AVLH_Find(p_RTO->AsExternalLsaHl, (byte*) &askey, (void**)&p_AsExtEntry, 0) != E_OK) ||
      !p_AsExtEntry)
  {
    return TRUE;
  }

  p_NssaLsa = (t_S_AsExternalLsa *)p_DbEntry->p_Lsa;
  p_AsExtLsa = (t_S_AsExternalLsa *)p_AsExtEntry->p_Lsa;
  
  /* Or there is an existing Type-5 LSA and either it corresponds to a 
  ** local OSPF external source whose path type and metric is less 
  ** preferred (see Section 2.5 step (6), parts (b) and (d))
  */
  if(p_AsExtLsa && p_NssaLsa)
  {
    /* (b) Type 1 ext paths are always preferred over type 2 ext path.   */ 
    /* When all paths are type 2, the paths with the smallest advertised */
    /* type 2 metric are always preferred. */
    if(!(p_NssaLsa->Flags & S_AS_EXT_E_FLAG) && (p_AsExtLsa->Flags & S_AS_EXT_E_FLAG))
      return TRUE;

    if((p_NssaLsa->Flags & S_AS_EXT_E_FLAG) && (p_AsExtLsa->Flags & S_AS_EXT_E_FLAG) &&
       (A_GET_3B(p_NssaLsa->Metric) < A_GET_3B(p_AsExtLsa->Metric)))
      return TRUE;

    /* Or it doesn't and the Type-5 LSA's path type or cost(s) have 
    ** changed (See Section 2.5 step (5))  
    */
    if((p_NssaLsa->Flags & S_AS_EXT_E_FLAG) != (p_AsExtLsa->Flags & S_AS_EXT_E_FLAG) ||
       (A_GET_3B(p_NssaLsa->Metric) != A_GET_3B(p_AsExtLsa->Metric)))
      return TRUE;

    /* Or the forwarding address no longer maps to a translatable Type-7
    ** LSA.
    */
    A_SET_4B(p_RTO->Cfg.RouterId, askey.AdvertisingRouter);
    memcpy(askey.LsId, p_AsExtLsa->ForwardingAddress, 4);
    if((AVLH_Find(p_ARO->NssaLsaHl, (byte*) &askey, (void**)&p_NssaEntry, 0) != E_OK) ||
       !(p_NssaEntry->Lsa.Options & OSPF_OPT_P_BIT))
      return TRUE;
  }

  return FALSE;
}

/*********************************************************************
 * @purpose    Flush T5 LSAs which have been originated as a result
 *             of translating a T7 LSA from a given area.
 *
 *
 * @param  ARO_Id    @b{(input)} Pointer to the NSSA
 *
 * @returns  E_OK
 *
 * @notes    
 *
 * @end
 * ********************************************************************/
static e_Err NssaFlushTranslatedLsas(t_ARO *p_ARO)
{
    t_RTO *p_RTO = (t_RTO*) p_ARO->RTO_Id;
    e_Err e;
    t_A_DbEntry *p_t7Entry;    /* a type 7 LSA */
    t_A_DbEntry *p_t5Entry;    /* a type 5 LSA */
    t_S_AsExternalLsa *p_NssaLsa;
    void *extRoute = NULL;

    /* Iterate through all T7 LSAs associated with this area */
    e = AVLH_GetFirst(p_ARO->NssaLsaHl, (void *)&p_t7Entry);
    while (e == E_OK)
    {
      /* skip T7 LSAs locally originated and LSAs w/o the P bit set */
      if ((p_RTO->Cfg.RouterId != A_GET_4B(p_t7Entry->Lsa.AdvertisingRouter)) &&
          ((p_t7Entry->Lsa.Options & OSPF_OPT_P_BIT) != 0))
      {
          p_NssaLsa = (t_S_AsExternalLsa *)p_t7Entry->p_Lsa;

          /* Find T5 LSA for same destination */
          if (FindSummaryOrASExtLsa(p_RTO, p_ARO, S_AS_EXTERNAL_LSA, 
                                    A_GET_4B(p_t7Entry->Lsa.LsId),
                                    A_GET_4B(p_NssaLsa->NetworkMask), 
                                    &p_t5Entry, NULLP) == E_OK)
          {
              if (p_RTO->Cfg.RouterId == A_GET_4B(p_t5Entry->Lsa.AdvertisingRouter))
              {
                  /* T5 LSA was originated locally */
                  /* Check if T5 LSA generated from a route redistributed locally */
                  SP_IPADR asExtId[2];         
                  asExtId[0] = (SP_IPADR) A_GET_4B(p_t7Entry->Lsa.LsId);
                  asExtId[1] = (SP_IPADR) A_GET_4B(p_NssaLsa->NetworkMask);
                  if (HL_FindFirst(p_RTO->AsExtRoutesHl, (byte*)asExtId, 
                                   &extRoute) != E_OK)
                  {
                      /* Route not redistributed locally. Must have been translated. */
                      FlushLsa((t_Handle)p_t5Entry, (ulng)p_ARO, 0);
                  }
              }
          }
      }
      e = AVLH_GetNext(p_ARO->NssaLsaHl,(void *)&p_t7Entry, p_t7Entry);
    }
    return E_OK;
}

/*********************************************************************
 * @purpose    Sets the NSSA translator state for an area.
 *
 *
 * @param  p_ARO     @b{(input)} Pointer to the NSSA
 * @param  newState  @b{(input)} The new translator state
 *
 * @returns  E_OK
 *
 * @notes    If router transitions from ENABLED to DISABLED or vice versa
 *           reoriginate the router LSA in the NSSA. If the translator
 *           stability interval timer is active, cancel it.
 *
 * @end
 * ********************************************************************/
static e_Err NssaTranslatorStateSet(t_ARO *p_ARO, e_NSSATranslatorState newState)
{
  t_RTO *p_RTO = (t_RTO*) p_ARO->RTO_Id;
    e_Err e;
    e_NSSATranslatorState oldState = p_ARO->NSSATranslatorState;
    t_AgrEntry    *p_AdrRange;
    t_A_DbEntry   *p_DbEntry;

    if (oldState == newState)
        return E_OK;

    /* If the stability timer is active, clear it. */
    if (TIMER_Active(p_ARO->TranslatorStabilityTimer))
    {
        TIMER_Stop(p_ARO->TranslatorStabilityTimer);
    }

    p_ARO->NSSATranslatorState = newState;

    /* If necessary, reoriginate the router LSA with the new Nt flag */
    if (((oldState == NSSA_TRANS_STATE_ENABLED) && 
         (newState != NSSA_TRANS_STATE_ENABLED)) ||
        ((oldState != NSSA_TRANS_STATE_ENABLED) && 
         (newState == NSSA_TRANS_STATE_ENABLED)))
    {
        LsaReOriginate[S_ROUTER_LSA](p_ARO, 0);
    }

    /* If state has transtioned to disabled, flush translated LSAs */
    if (newState == NSSA_TRANS_STATE_DISABLED)
    {
        /* Flush summaries */
        e = HL_GetFirst(p_ARO->AdrRangeHl, (void *)&p_AdrRange);
        while(e == E_OK)
        {
            /* If range is used for NSSA aggregation and range is active, flush it. */
            if ((p_AdrRange->AggregateStatus == ROW_ACTIVE) && 
                (p_AdrRange->LcRte) &&
                (p_AdrRange->LsdbType == AGGREGATE_NSSA_EXTERNAL_LINK))
            {
                if (FindSummaryOrASExtLsa(p_RTO, p_ARO, S_AS_EXTERNAL_LSA, 
                                          p_AdrRange->NetIpAdr,
                                          p_AdrRange->SubnetMask, 
                                          &p_DbEntry, NULLP) == E_OK)
                {
                    FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);
                }
            }
            e = HL_GetNext(p_ARO->AdrRangeHl, (void *)&p_AdrRange, p_AdrRange);
        }
    
        /* Flush LSAs translated directly (not summarized) */
        NssaFlushTranslatedLsas(p_ARO);
    }

    /* If router becomes the translator, translate existing T7 LSAs. */
    if (oldState == NSSA_TRANS_STATE_DISABLED)
    {
        NssaTranslatorEnable(p_ARO);
    }

    return E_OK;
}

/*********************************************************************
 * @purpose    Called when router becomes an NSSA translator for a given
 *             area. Translates T7 LSAs from the NSSA currently in the 
 *             LSDB into T5 LSAs.
 *
 *
 * @param  p_ARO    @b{(input)} Pointer to the NSSA
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void NssaTranslatorEnable(t_ARO *p_ARO)
{
  t_RTO *p_RTO = NULLP;
  t_RTB *p_RTB = NULLP;
  t_VpnCos VpnCos = 0;

  if(!p_ARO || (p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA) ||
     (p_ARO->NSSATranslatorState == NSSA_TRANS_STATE_DISABLED))
    return;

  p_RTO = (t_RTO *)p_ARO->RTO_Id;
  /* Find the relevant routing table */
  if(HL_FindFirst(p_RTO->RtbHl, (byte *)&VpnCos, (void *)&p_RTB) != E_OK)
     return;

  /* Walk the route table and translate the relevant Type-7 into Type-5 LSAs.
  ** Send a nullptr for the address range paramter, if a route is subsumed
  ** by an address range the NssaTranslate function will determine that and
  ** send out an aggregate Type-5 LSA
  */
  p_RTO->BundleLsas = TRUE;
  AVL_Browse(p_RTB->RtbNtBt, NssaTranslate, (ulng)NULLP);
  if (p_RTO->BundleLsas)
    LsUpdatesSend(p_RTO);
}

/*********************************************************************
 * @purpose    Elects the NSSA translator.  This function may be called
 *             by an HL_Browse function.
 *
 *
 * @param  ARO_Id    @b{(input)} Pointer to the NSSA
 * @param  flag      @b{(input)} unused
 *
 * @returns  TRUE: When called by the HL_Browse function the objective
 *           is only to evaluate the NssaTranslatorStatus and not delete
 *           the area entry and hence this function will always return
 *           true.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool NssaTranslatorElect(t_Handle ARO_Id, ulng flag)
{
  e_Err e;
  t_ARO         *p_ARO = (t_ARO *)ARO_Id;
  t_RTO         *p_RTO = (t_RTO *)p_ARO->RTO_Id;
  t_A_DbEntry   *p_DbEntry;
  t_S_RouterLsa *p_RouterLsa;
  SP_IPADR advRtr;
  t_RoutingTableEntry *p_Br;

  /* If this area is not an NSSA, return */
  if (p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA) 
    return TRUE;

  /* If this router is not an ABR, its translator state must be DISABLED */
  if (!routerIsBorder(p_RTO))
  {
      if (p_ARO->NSSATranslatorState != NSSA_TRANS_STATE_DISABLED)
      {
          if (p_RTO->ospfTraceFlags & OSPF_TRACE_NSSA)
          {
              char traceBuf[OSPF_MAX_TRACE_STR_LEN];
              L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
              osapiInetNtoa(p_ARO->AreaId, areaIdStr);          
              sprintf(traceBuf, "No longer ABR. Setting NSSA translator state to"
                      " DISABLED for area %s.", 
                      areaIdStr);
              RTO_TraceWrite(traceBuf);
          }
          NssaTranslatorStateSet(p_ARO, NSSA_TRANS_STATE_DISABLED);
      }
      return TRUE;
  }

  /* If this router has been configured to "always" be a translator, set
  ** state to enabled
  */
  if (p_ARO->NSSATranslatorRole == NSSA_TRANSLATOR_ALWAYS)
  {
    if(p_ARO->NSSATranslatorState != NSSA_TRANS_STATE_ENABLED)
    {
        if (p_RTO->ospfTraceFlags & OSPF_TRACE_NSSA)
        {
            char traceBuf[OSPF_MAX_TRACE_STR_LEN];
            L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
            osapiInetNtoa(p_ARO->AreaId, areaIdStr);          
            sprintf(traceBuf, "Setting NSSA translator state to ENABLED for area %s.", 
                    areaIdStr);
            RTO_TraceWrite(traceBuf);
        }
        NssaTranslatorStateSet(p_ARO, NSSA_TRANS_STATE_ENABLED);
    }
    return TRUE;
  }

  /* If we reached here we are a "candidate" so we need to elect a translator */
  e = AVLH_GetFirst(p_ARO->RouterLsaHl, (void *)&p_DbEntry);
  while(e == E_OK)
  {
    p_RouterLsa = (t_S_RouterLsa *) p_DbEntry->p_Lsa;

    /* Consider only those NSSA ABRs that are reachable over the intra-area-path in the NSSA area */
    advRtr = A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter);

    if((advRtr != p_RTO->Cfg.RouterId) &&
       (RteIntraAreaFindByType(p_ARO, advRtr, DEST_TYPE_IS_BORDER_ROUTER, &p_Br) == E_OK))
    {
      if ((p_RouterLsa->Flags & S_BORDER_FLAG) &&
          ((A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter) > p_RTO->Cfg.RouterId) ||
           (p_RouterLsa->Flags & S_NSSATRANSLATOR_FLAG)))
      {
          if (p_RTO->ospfTraceFlags & OSPF_TRACE_NSSA)
          {
              char traceBuf[OSPF_MAX_TRACE_STR_LEN];
              L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
              L7_uchar8 electedIdStr[OSAPI_INET_NTOA_BUF_SIZE];
              osapiInetNtoa(p_ARO->AreaId, areaIdStr);        
              osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter), electedIdStr);          
              sprintf(traceBuf, "Router %s elected NSSA translator for area %s.", 
                      electedIdStr, areaIdStr);
              RTO_TraceWrite(traceBuf);
          }
          if (p_ARO->NSSATranslatorState == NSSA_TRANS_STATE_ELECTED)
          {
              /* translator state will be changed when stability interval expires */
              TIMER_StartSec(p_ARO->TranslatorStabilityTimer, 
                             p_ARO->TranslatorStabilityInterval, 0,
                             TranslatorStabilityTimerExp, 
                             p_RTO->OspfSysLabel.threadHndle);
          }
          else if (p_ARO->NSSATranslatorState == NSSA_TRANS_STATE_ENABLED)
          {
              NssaTranslatorStateSet(p_ARO, NSSA_TRANS_STATE_DISABLED); 
          }
          return TRUE;
      }
    }
    e = AVLH_GetNext(p_ARO->RouterLsaHl, (void *)&p_DbEntry, p_DbEntry);
  }

  /* No other router was elected, so this router is elected */
  if (p_ARO->NSSATranslatorState != NSSA_TRANS_STATE_ELECTED)
  {
      if (p_RTO->ospfTraceFlags & OSPF_TRACE_NSSA)
      {
          char traceBuf[OSPF_MAX_TRACE_STR_LEN];
          L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
          osapiInetNtoa(p_ARO->AreaId, areaIdStr);          
          sprintf(traceBuf, "Setting NSSA translator state to ELECTED for area %s.", 
                  areaIdStr);
          RTO_TraceWrite(traceBuf);
      }
      NssaTranslatorStateSet(p_ARO, NSSA_TRANS_STATE_ELECTED);
  }

  return TRUE;
}

/*********************************************************************
 * @purpose           Used to translate Type-7 into Type-5 LSAs
 *                    Serves the  browsing AVL_Browse function
 *
 *
 * @param  p_RTB      @b{(input)} Router table object handler id
 * @param  p_Rte      @b{(input)} Routing table entry
 * @param  chType     @b{(input)} Route change type
 * @param  p_AdrRange @b{(input)} Area Range entry if applicable
 *
 * @returns
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void NssaHandleRouteChange(t_RTB *p_RTB, t_RoutingTableEntry *p_Rte,
                           e_RtChange chType, t_AgrEntry *p_AdrRange)
{
  t_ARO *p_ARO;
  t_RTO *p_RTO;
  t_A_DbEntry *p_DbEntry = NULLP;

  /* This routine only handles Type-7 Routes */
  if(!p_RTB || (p_Rte->DestinationType < DEST_TYPE_IS_NETWORK) || 
     ((p_Rte->PathType != OSPF_NSSA_TYPE_1_EXT) && (p_Rte->PathType != OSPF_NSSA_TYPE_2_EXT)) ||
     (p_Rte->p_DbEntry->Lsa.LsType != S_NSSA_LSA))
    return;

  p_RTO = (t_RTO *)p_RTB->RTO_Id;

  /* Get area object handle of the RT entry*/
  if((HL_FindFirst(p_RTO->AroHl,(byte*)&p_Rte->AreaId, (void **)&p_ARO) != E_OK) ||
     (p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA))
    return;

  switch(chType)
  {
  case RTE_REMOVED:
    /* If a previously advertised Type-7 route is no longer reachable then 
    ** flush the corresponding Type-5 external LSA. This router may no longer
    ** be the translator or even an ABR, but if the router translated previously,
    ** it now needs to flush.
    */
    if(FindSummaryOrASExtLsa(p_RTO, p_ARO, S_AS_EXTERNAL_LSA, p_Rte->DestinationId,
                             p_Rte->IpMask, &p_DbEntry, NULLP) == E_OK)
    {
      FlushAsLsa((t_Handle)p_DbEntry, (ulng)p_RTO);
    }
    break;
  case RTE_ADDED:
  case RTE_MODIFIED:
    /* If this is not an area border router or the translator state is disable,
    ** it cannot perform the duties of a translator 
    */
    if (routerIsBorder(p_RTO) && (p_ARO->NSSATranslatorState != NSSA_TRANS_STATE_DISABLED))
    {
        /* If this is a newly added/changed Type-7 route then translate it to
        ** to a corresponding Type-5 external LSA.  
        */
        NssaTranslate((byte*)p_Rte, (ulng)p_AdrRange);
    }
    break;
  default:
    break;
  }
}

t_Handle NSSA_FindForwardingAddress(t_Handle ARO_Id)
{
  t_ARO    *p_ARO = (t_ARO *)ARO_Id;
  t_IFO    *p_IFO = NULLP, *p_tmpIFO = NULLP;
  t_NBO    *p_NBO = NULLP;
  e_Err    er = E_FAILED;

  if(!p_ARO)
    return NULLP;

  /* If internal addresses are not available, preference should be given to the
  ** router's active OSPF stub network addresses.  These choices avoid the
  ** possible extra hop that may happen when a transit network's address
  ** is used.
  */
  er = HL_GetFirst(p_ARO->IfoHl, (void *)&p_tmpIFO);
  while ((er == E_OK) && p_tmpIFO)
  {
    /* Save the first active interface */
    if(!p_IFO && p_tmpIFO->Cfg.State != IFO_DOWN)
      p_IFO = p_tmpIFO;

    if((p_tmpIFO->Cfg.State != IFO_DOWN) && 
       HL_GetFirst(p_tmpIFO->NboHl, (void *)&p_NBO) != E_OK)
    {
      p_IFO = p_tmpIFO;
      break;
    }

    er = HL_GetNext(p_ARO->IfoHl, (void *)&p_tmpIFO, p_tmpIFO);
  }
  return (t_Handle) p_IFO;
}

/*********************************************************************
 * @purpose   Add a T7 route to a T7 address range.
 *
 * @param  p_RTO       @b{(input)} OSPF instance
 * @param  p_AggrEntry @b{(input)} Area Range entry if applicable
 * @param  p_Rte       @b{(input)} Routing table entry
 *
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err AddRouteToT7Range(t_RTO *p_RTO, t_AgrEntry *p_AggrEntry, 
                        t_RoutingTableEntry *p_Rte)
{
  if (p_RTO->ospfTraceFlags & OSPF_TRACE_NSSA)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    L7_uchar8 rangeAddr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 rangeMask[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 routeAddr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 routeMask[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_AggrEntry->NetIpAdr, rangeAddr);
    osapiInetNtoa(p_AggrEntry->SubnetMask, rangeMask);
    osapiInetNtoa(p_Rte->DestinationId, routeAddr);
    osapiInetNtoa(p_Rte->IpMask, routeMask);
    sprintf(traceBuf, "Adding route %s/%s to T7 range %s/%s", 
            routeAddr, routeMask, rangeAddr, rangeMask);
    RTO_TraceWrite(traceBuf);
  }
  HL_Insert(p_AggrEntry->AgrRteHl, (byte*)&p_Rte->DestinationId, p_Rte);
  return E_OK;
}

/*********************************************************************
 * @purpose   Add a T7 route to a T7 address range.
 *
 * @param  p_RTO       @b{(input)} OSPF instance
 * @param  p_AggrEntry @b{(input)} Area Range entry if applicable
 * @param  p_Rte       @b{(input)} Routing table entry
 *
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err RemoveRouteFromT7Range(t_RTO *p_RTO, t_AgrEntry *p_AggrEntry, 
                             t_RoutingTableEntry *p_Rte)
{
  if (p_RTO->ospfTraceFlags & OSPF_TRACE_NSSA)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    L7_uchar8 rangeAddr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 rangeMask[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 routeAddr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 routeMask[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_AggrEntry->NetIpAdr, rangeAddr);
    osapiInetNtoa(p_AggrEntry->SubnetMask, rangeMask);
    osapiInetNtoa(p_Rte->DestinationId, routeAddr);
    osapiInetNtoa(p_Rte->IpMask, routeMask);
    sprintf(traceBuf, "Removing route %s/%s from T7 range %s/%s", 
            routeAddr, routeMask, rangeAddr, rangeMask);
    RTO_TraceWrite(traceBuf);
  }

  HL_Delete(p_AggrEntry->AgrRteHl, (byte*) &p_Rte->DestinationId, NULL);
  return E_OK;
}

/*********************************************************************
 * @purpose     The procedure updates statistics counters and the
 *              area checksum.
 *              It is called when new LSA is installed into the
 *              database or when an LSA is released from the database
 *
 * @param p_RTO            @b{(input)} OSPF instance
 * @param p_ARO            @b{(input)} Handle of the area object. NULL for 
 *                                     LSAs with AS flooding scope.
 * @param p_DbEntry        @b{(input)} pointer to data base entry
 * @param EntryIsAdded     @b{(input)} TRUE when the entry is being installed
 *                                 FALSE when the specified entry is released
 *
 * @returns      E_FAILED - if the input parameters are not correct
 *               E_OK     - everything is OK
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err UpdateStatCounters(t_RTO *p_RTO, t_ARO *p_ARO, t_A_DbEntry *p_DbEntry,
                         Bool EntryIsAdded)
{
   int    sign = EntryIsAdded ? 1: -1;
   t_IFO *p_IFO;

   if (!p_DbEntry)
      return E_FAILED;

   if(A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter) == p_RTO->Cfg.RouterId)
   {
     if(p_DbEntry->Lsa.LsType == S_AS_EXTERNAL_LSA)
       p_RTO->Cfg.SelfOrigExternLsaCount += sign;
     else if (p_ARO && (p_DbEntry->Lsa.LsType == S_NSSA_LSA))
       p_ARO->SelfOrigNSSALsaCount += sign;
   }

   /* Update Area' LSA counters and all LS checksums */
   if((p_DbEntry->Lsa.LsType < S_AS_EXTERNAL_LSA) ||
      (p_DbEntry->Lsa.LsType == S_AREA_OPAQUE_LSA) || 
      (p_DbEntry->Lsa.LsType == S_GROUP_MEMBER_LSA) ||
      (p_DbEntry->Lsa.LsType == S_NSSA_LSA))
   {
     if (!p_ARO)
     {
       L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
               "UpdateStatCounters() area error.");
       return E_FAILED;
     }
      p_ARO->AreaLsaCount += sign;
      p_ARO->AreaLsaCksumSum += sign * A_GET_2B(p_DbEntry->Lsa.CheckSum);
      p_ARO->AreaLsaStats[p_DbEntry->Lsa.LsType] += sign;
   }   
   else if ((p_DbEntry->Lsa.LsType == S_LINK_OPAQUE_LSA) && p_DbEntry->IFO_Id)
   {
     p_IFO = (t_IFO*)p_DbEntry->IFO_Id;
     p_IFO->Cfg.LinkLsaCount += sign;
     p_IFO->Cfg.LinkLsaCksum += sign * A_GET_2B(p_DbEntry->Lsa.CheckSum);
     /* Even though these have interface scope, we still maintain and report
      * a per-area count. checksum of link opaque LSA does not contribute to
      * area checksum sum (same as cisco). */
     p_ARO->AreaLsaStats[S_LINK_OPAQUE_LSA] += sign;
   }
   else if(p_DbEntry->Lsa.LsType == S_AS_OPAQUE_LSA)
   {
     p_RTO->Cfg.OpaqueASLsaCount += sign;
     p_RTO->OpaqueASLsaCksumSum +=  sign * A_GET_2B(p_DbEntry->Lsa.CheckSum);
   }   
   else if(p_DbEntry->Lsa.LsType == S_AS_EXTERNAL_LSA)
   {
      p_RTO->Cfg.ExternLsaCount += sign;
      p_RTO->Cfg.ExternLsaCksumSum += 
                              sign * A_GET_2B(p_DbEntry->Lsa.CheckSum);
      if (A_GET_4B(p_DbEntry->Lsa.LsId) != 0)
     {
       p_RTO->NonDefExtLsaCount += sign;
     }
   }

   return E_OK;
}



/* --- end of file SPARO.C --- */
