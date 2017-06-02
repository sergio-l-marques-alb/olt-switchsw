/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename   sparo.c
 *
 * @purpose    OSPF Area Object (ARO) module, supports  Area Database maitanance
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
 *
 *
 * Internal Routines:
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
 * Timer Expiration Routines
 *
 * AgingTimerExp
 * LsaDelayedTimerExp
 *
 *
 * @create     12/17/1998
 *
 * @author     Dan Dovolsky
 *
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\application\protocol\ospf\spifo.c 1.3.2.8 2003/02/18 12:52:10EST anayar Exp  $";
#endif

#include <stdio.h>

/* --- standard include files --- */
#include "std.h"
#include "local.h"



/* --- external object interfaces --- */

#if L7_MIB_OSPF
#include "ospfmib.ext"
#endif


/* --- specific include files --- */

#include "spobj.h"
#include "spgrc.h"

#include "osapi.h"
#include "os_xxcmn.h"
#include "log.h"
#include "nimapi.h"
#include "simapi.h"
#include "buffer.h"
#include "frame.h"
#include "ll.h"
#include "ospf_debug_api.h"

#ifdef L7_NSF_PACKAGE
#include "ospf_ckpt.h"
#endif

extern void MdGetDigest(byte *data, int length, byte *digest);
extern void CheckVirtLinkUp(t_IFO *p_IFO, t_RTB *p_RTB);
extern e_Err LsaOrgSumBulk(t_Handle Id);
extern void RTB_ComputeCalcDelay(t_RTB *p_RTB, t_RTO *p_RTO);

/* --- internal prototypes --- */
e_Err HelloTimerExp( t_Handle Id, t_Handle TimerId, word Flag );
e_Err WaitTimerExp( t_Handle Id, t_Handle TimerId, word Flag );
e_Err UnicastAckTimerExp( t_Handle Id, t_Handle TimerId, word Flag );
e_Err BroadcastAckTimerExp( t_Handle Id, t_Handle TimerId, word Flag );

static e_TrapConfigErrors HelloReceived(t_IFO *p_IFO, t_S_PckHeader *p_Hdr,
                          SP_IPADR router, SP_IPADR SrcAdr);
static void InformNbrsAboutIfoDown(t_IFO *p_IFO);

static void AuthPckCoding(t_IFO *p_IFO, void *p_Packet);
static e_Err AuthPckDecoding(t_IFO *p_IFO, t_NBO *p_NBO, void *p_Packet);

static void NssaHandleIFOUpEvent(t_IFO *p_IFO);
static void NssaHandleIFODownEvent(t_IFO *p_IFO);

static L7_uint32 DrCalculateDr(t_IFO *p_IFO);
static L7_uint32 DrCalculateBackup(t_IFO *p_IFO);
static e_Err DrNbmaStart(t_IFO *p_IFO);
static L7_uint32 DrInterfaceAddress(t_IFO *p_IFO, L7_uint32 routerId);

/*----------------------------------------------------------------
 *
 *             Hello SM Object Transition table
 *
 *----------------------------------------------------------------*/

static void IfEr(t_IFO *p_IFO);
static void Ifo0(t_IFO *p_IFO);
static void Ifo1(t_IFO *p_IFO);
static void Ifo2(t_IFO *p_IFO);
static void Ifo3(t_IFO *p_IFO);
static void Ifo4(t_IFO *p_IFO);
static void Ifo5(t_IFO *p_IFO);

typedef void (*f_Hello_tr)(t_IFO *);

static f_Hello_tr Hello_TT[NUMB_OF_IFO_EVENTS][NUMB_OF_IFO_STATES] = {
/*           DOWN,   LOOP,   WAIT,   PTPST,  DR,    BACKUP,   DROT */
/* UP     */{Ifo0,   IfEr,   IfEr,   IfEr,   IfEr,   IfEr,   IfEr},
/* WAIT TM    */{IfEr,   IfEr,   Ifo1,   IfEr,   IfEr,   IfEr,   IfEr},
/* BK SEEN    */{IfEr,   IfEr,   Ifo1,   IfEr,   IfEr,   IfEr,   IfEr},
/* NGB CHN    */{IfEr,   IfEr,   IfEr,   IfEr,   Ifo1,   Ifo1,   Ifo1},
/* LOOP   */{IfEr,   Ifo2,   Ifo2,   Ifo2,   Ifo2,   Ifo2,   Ifo2},
/* UNLOOP */{IfEr,   Ifo3,   IfEr,   IfEr,   IfEr,   IfEr,   IfEr},
/* DOWN       */{IfEr,   Ifo4,   Ifo4,   Ifo4,   Ifo4,   Ifo4,   Ifo4},
/* PASSIVE    */{IfEr,   IfEr,   Ifo5,   Ifo5,   Ifo5,   Ifo5,   Ifo5},
/* NON_PASSIVE*/{IfEr,   IfEr,   Ifo0,   Ifo0,   Ifo0,   Ifo0,   Ifo0},
}; /* end of table */

unsigned char *ospfPckTypeNames[] = {"ILLEGAL", "HELLO",
    "DD", "LS REQ", "LS UPDATE", "LS ACK"};
unsigned char *ifoStateNames[NUMB_OF_IFO_STATES] = {"IFO_DOWN", "IFO_LOOP", "IFO_WAIT", "IFO_PTPST",
   "IFO_DR", "IFO_BACKUP", "IFO_DROTHER"};
unsigned char *ifoEventNames[NUMB_OF_IFO_EVENTS] = {"IFO_INTERFACE_UP", "IFO_WAIT_TIMER", "IFO_BACKUP_SEEN",
   "IFO_NGB_CHANGE", "IFO_LOOP_IND", "IFO_UNLOOP_IND", "IFO_INTERFACE_DOWN"};

/*********************************************************************
 * @purpose                Initialize an IFO object.
 *
 * @param RTO_Id           @b{(input)}  parent RTO object handler
 * @param LowLayerId       @b{(input)}  low-layer  object handler
 * @param p_IfoId          @b{(output)}  IFO Object handler retuned here
 *
 * @returns   E_OK           success
 * @returns   E_FAILED       couldn't create an object
 * @returns   E_NOMEMORY     not enough memory for resources allocating
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err IFO_Init(t_Handle RTO_Id, t_Handle LowLayerId,
               t_Handle *p_IfoId)
{
   t_IFO   *p_IFO;
   e_Err   e = E_OK;

   /* try allocate from user memory */
   p_IFO = (t_IFO *)XX_Malloc( sizeof(t_IFO) );
   if(p_IFO)
      memset( p_IFO, 0, sizeof(t_IFO) );
   else
      return E_NOMEMORY;

   /* initialize the object */

   p_IFO->Taken = TRUE;
   p_IFO->neighborsOnIface = 0;
   p_IFO->grSuppressHellos = L7_FALSE;

   p_IFO->RTO_Id = RTO_Id;
   p_IFO->LowLayerId = LowLayerId;

   /* allocate Neighbor Objects Hash List */
   e = HL_Init(OWN_LINK, sizeof(SP_IPADR),
               /* key offset of RouterId in NBO */
               offsetof(t_NBO, RouterId),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_IFO->NboHl);

   /* allocate Metrics Hash List */
   e = HL_Init(0, sizeof(e_TosTypes), offsetof(t_MetricEntry, MetricTOS),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_IFO->MetricHl);

   /* allocate NBMA Hash List */
   e = HL_Init(0, sizeof(SP_IPADR), offsetof(t_S_NbmaRtrCfg, RouterId),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_IFO->NbmaHl);

   /* allocate Authentication Keys Hash List */
   e = HL_Init(0, 1, offsetof(t_OspfAuthKey, KeyId),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_IFO->AuthKeyHl);

   /* allocate Link Opaque LSAs DB AVLH List */
   e |= AVLH_Init(0, sizeof(t_A_DbKey),
                  /* key offset in t_A_DbEntry */
                  offsetof(t_A_DbEntry,Lsa) + offsetof(t_S_LsaHeader, LsId),
                  NULL, NULL, &p_IFO->LinkOpaqueLsaHl);

   /* Init Link State Acknowledgment list */
   p_IFO->LsAck = LL_New(0);

   /* Hello packets resend timer handler */
   e |= TIMER_InitSec( 1, p_IFO, &p_IFO->HelloTimer );
   /* Stop wait timer */
   e |= TIMER_InitSec( 1, p_IFO, &p_IFO->WaitTimer );
   /* Init Ls Ack  repeatitive timer */
   e |= TIMER_InitSec( 1, p_IFO, &p_IFO->AckTimer );
   /* Timer to stop suppressing hellos on unplanned restart */
   e |= TIMER_InitSec(1, p_IFO, &p_IFO->grSuppressHelloTimer );


#if L7_OSPF_TE
   {
      ulng bitSetIndex = 0;
      /* Get instance number for TE Link Lsa and set this number */
      /* as busy (i.e. set the appropriate bit to TRUE)          */
      e |= BS_GetFirst(((t_RTO *)p_IFO->RTO_Id)->TeLsaInstBS,
                        FALSE, &bitSetIndex);
      if (e == E_OK)
      {
         p_IFO->TeLsaInst = (word)bitSetIndex;  /* we use only 16 low bits */
         BS_SetBit(((t_RTO *)p_IFO->RTO_Id)->TeLsaInstBS, bitSetIndex, TRUE);
      }
   }
#endif

   p_IFO->Hello_TT = Hello_TT; /* pointer to Hello SM transition table */

   p_IFO->Cfg.IfStatus = ROW_NOT_IN_SERVICE;
   p_IFO->Cfg.passiveMode = FALSE;

   if(e == E_OK)
   {
      /* pass object ID back to caller */
      *p_IfoId = (t_Handle) p_IFO;
   }
   else
      IFO_Delete((t_Handle) p_IFO, 1);

   return e;
}




/*********************************************************************
 * @purpose            Delete an IFO object.
 *
 *
 * @param   Id         @b{(input)}  IFO object Id
 * @param   flag       @b{(input)}  delete this entry from ARO's IfoHl flag
 *
 * @returns            TRUE if this entry remains in ARO's IfoHl
 * @returns            FALSE otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool IFO_Delete( t_Handle Id, ulng flag)
{
   t_IFO   *p_IFO = (t_IFO *)Id;
   t_RTO   *p_RTO = (t_RTO *) p_IFO->RTO_Id;
   t_RTB   *p_RTB;
   t_IFO   *p_tmpIfo;
   t_AckElement *p_Ack;
   t_ARO *p_ARO = (t_ARO*) p_IFO->ARO_Id;
   t_VpnCos zero = 0;


   /* Remove this entry from corresponded Area Object */
   if (flag && p_ARO)
   {
       HL_Delete(p_ARO->IfoHl, (byte*)&p_IFO->IfoId, p_IFO);
   }

   HL_Delete(p_RTO->IfoIndexHl, (byte*)&p_IFO->Cfg.IfIndex, p_IFO);

   ClearLsaList(p_IFO->LinkOpaqueLsaHl);
   AVLH_Destroy(&p_IFO->LinkOpaqueLsaHl);

  /* Destroy all NBOs */
  HL_Browse(p_IFO->NboHl, NBO_Delete, 0);
  HL_Destroy(&p_IFO->NboHl);

  HL_Cleanup(p_IFO->MetricHl,1);
  HL_Cleanup(p_IFO->NbmaHl,1);
  HL_Cleanup(p_IFO->AuthKeyHl,1);

  HL_Destroy(&p_IFO->MetricHl);
  HL_Destroy(&p_IFO->NbmaHl);
  HL_Destroy(&p_IFO->AuthKeyHl);

  while ((p_Ack = LL_Get(p_IFO->LsAck)) != NULL)
    XX_Free(p_Ack);
  LL_Delete(p_IFO->LsAck);

  TIMER_Delete( p_IFO->AckTimer );
  TIMER_Delete( p_IFO->HelloTimer );
  TIMER_Delete( p_IFO->WaitTimer );
  TIMER_Delete(p_IFO->grSuppressHelloTimer);

   /* if no more Interfaces in this Area */
   if (p_ARO && flag)
      if (HL_GetFirst(p_ARO->IfoHl, (void *)&p_tmpIfo) != E_OK)
      {
          if (p_RTO->Cfg.DelAreaOnLastIfDel)
            ARO_Delete(p_ARO, 1);

         /* If the deleted area is not the last one of the router */
         /* Recalculate the appropriate routing table             */
         if(p_RTO->AreasNum != 0)
         {
            if(HL_FindFirst(p_RTO->RtbHl, (byte *) &zero, (void *)&p_RTB) == E_OK)
            {
               /* Start recalculation timer */
               if(!TIMER_Active(p_RTB->RecalcTimer))
               {
                 RTB_ComputeCalcDelay(p_RTB, p_RTO);
                 TIMER_StartSec(p_RTB->RecalcTimer, p_RTB->CalcDelay, 0, RecalcTimerExp,
                                p_RTO->OspfRtbThread.threadHndle);
               }
            }
         }
      }

      if (p_IFO->PendingUpdate)
      {
        F_Delete(p_IFO->PendingUpdate);
      }

#if L7_OSPF_TE
   /* Free the interface TE Link Lsa instance number   */
   /* (i.e. set the appropriate bit to FALSE)          */
   if (((t_RTO *)p_IFO->RTO_Id)->TeLsaInstBS)
      BS_SetBit(((t_RTO *)p_IFO->RTO_Id)->TeLsaInstBS,
                (ulng)p_IFO->TeLsaInst, FALSE);
#endif
#if L7_MIB_OSPF
   if (p_IFO->Cfg.Type == IFO_VRTL)
   {
      /* remove the row from the MIB table */
      ospfVirtIfTable_DeleteInternal(((t_RTO *)p_IFO->RTO_Id)->ospfMibHandle,
                                     p_IFO->Cfg.VirtTransitAreaId,
                                     p_IFO->Cfg.VirtIfNeighbor);
   }
   else
   {
      ospfIfTable_DeleteInternal(((t_RTO *)p_IFO->RTO_Id)->ospfMibHandle,
                                 p_IFO->Cfg.IpAdr, p_IFO->Cfg.IfIndex);
   }
#endif

   /* Zero DR and BDR as indication to checkpoint code that interface is going away */
   if (p_RTO->Clbk.f_DrElected)
   {
     p_IFO->DrId = 0;
     p_IFO->BackupId = 0;
     (p_RTO->Clbk.f_DrElected)(p_IFO);
   }

   p_IFO->Taken = FALSE;
   XX_Free( p_IFO );

   return ((Bool) flag);
}

/*********************************************************************
 * @purpose    Take actions required when router becomes an ABR as a
 *             result of a change in the state of an interface.
 *
 * @param p_IFO   @b{(input)}  interface whose state has changed
 *
 * @returns   E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err IFO_BecomeAbr(t_IFO *p_IFO)
{
    t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
    t_ARO *p_ARO = (t_ARO*) p_IFO->ARO_Id;
    t_ARO *otherArea;
    e_Err e;

    /* Iterate through all attached areas. */
    e = HL_GetFirst(p_RTO->AroHl, (void**)&otherArea);
    while (e == E_OK)
    {
        /* If area is an NSSA, see if translator state should change. */
        if (otherArea->ExternalRoutingCapability == AREA_IMPORT_NSSA)
        {
            NssaTranslatorElect(otherArea, 0);
        }
        if (otherArea != p_ARO)
        {
           /* need to reoriginate router LSA in all other areas to notify
            * of the change in ABR status. */
            LsaReOriginate[S_ROUTER_LSA](otherArea, 0);
        }

        LsaOrgSumBulk(otherArea);

        /* If area is a stub, originate a default route. */
        if (otherArea->ExternalRoutingCapability == AREA_IMPORT_NO_EXTERNAL)
        {
          StubDefaultLsaOriginate(otherArea);
        }
        else if (otherArea->ExternalRoutingCapability == AREA_IMPORT_NSSA)
        {
          NssaDefaultLsaOriginate(otherArea);
        }

        e = HL_GetNext(p_RTO->AroHl, (void**)&otherArea, otherArea);
    }
    return E_OK;
}

/*********************************************************************
 * @purpose    Take actions required when router loses its ABR status as a
 *             result of a change in the state of an interface.
 *
 * @param p_IFO   @b{(input)}  interface whose state has changed
 *
 * @returns   E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err IFO_NoLongerAbr(t_IFO *p_IFO)
{
    t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
    t_ARO *p_ARO = (t_ARO*) p_IFO->ARO_Id;
    t_ARO *otherArea;
    e_Err e;
    t_A_DbKey dbkey;
    t_A_DbEntry *p_DbEntry;

   e = HL_GetFirst(p_RTO->AroHl, (void**)&otherArea);
    while (e == E_OK)
    {
        /* If area is an NSSA, see if translator state should change. */
        if (otherArea->ExternalRoutingCapability == AREA_IMPORT_NSSA)
        {
            NssaTranslatorElect(otherArea, 0);
        }

        if (otherArea != p_ARO)
        {
          /* need to reoriginate router LSA in all other areas to notify
           * of the change in ABR status. */
            LsaReOriginate[S_ROUTER_LSA](otherArea, 0);

            if ((otherArea->ExternalRoutingCapability == AREA_IMPORT_NO_EXTERNAL) ||
                (otherArea->ExternalRoutingCapability == AREA_IMPORT_NSSA))
            {
              /* If still attached to a stub area or NSSA, flush the T3 default route
               * previously advertised into the area. */
              A_SET_4B(p_RTO->Cfg.RouterId, dbkey.AdvertisingRouter);
              A_SET_4B(DefaultDestination, dbkey.LsId);
              if (AVLH_Find(otherArea->NetSummaryLsaHl, (byte *)&dbkey,
                            (void *)&p_DbEntry, 0) == E_OK)
              {
                FlushLsa((t_Handle)p_DbEntry, (ulng)otherArea, 0);
                otherArea->StubDefLSAOriginated = FALSE;
              }
            }
            if (otherArea->ExternalRoutingCapability == AREA_IMPORT_NSSA)
            {
              /* May have previously originated a T7 default into the NSSA. If
               * so, flush it. */
              A_SET_4B(p_RTO->Cfg.RouterId, dbkey.AdvertisingRouter);
              A_SET_4B(DefaultDestination, dbkey.LsId);
              if (AVLH_Find(otherArea->NssaLsaHl, (byte *)&dbkey,
                             (void *)&p_DbEntry, 0) == E_OK)
              {
                FlushLsa((t_Handle)p_DbEntry, (ulng)otherArea, 0);
              }
            }
        }
        e = HL_GetNext(p_RTO->AroHl, (void**)&otherArea, otherArea);
    }
    return E_OK;
}

/*********************************************************************
 * @purpose      Config IFO object get/set depend on IfStatus field
 *               value.
 *
 *
 * @param IFO_Id        @b{(input)}  IFO object Id
 * @param p_Cfg         @b{(input)}  IFO configuration (see spcfg.h)
 *
 * @returns   E_OK           config successfuly setup or read
 * @returns   E_FAILED       couldn't setup or read
 * @returns   E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err IFO_Config(t_Handle IFO_Id, t_IFO_Cfg *p_Cfg)
{
   t_IFO *p_IFO = (t_IFO *) IFO_Id;
   t_RTO *p_RTO;
   t_ARO *bbArea;    /* the backbone area */
   t_VpnCos zero = 0;

   /* If this is a virtual interface, the area object for the transit area. */
   t_ARO *transitArea;

   e_Err e, er;
   SP_IPADR backbone = OSPF_BACKBONE_AREA_ID;
   t_RTB *p_RTB;
   word  State;
   ulng  areaId;
   word numIfInArea;  /* number of interfaces in this interface's area */

   /* previous hello interval on this interface */
   word oldHelloInt;
   Bool oldPassiveMode;

   if (!p_IFO)
     return E_FAILED;

   p_RTO = (t_RTO *) p_IFO->RTO_Id;
   if (!p_RTO)
     return E_FAILED;

   switch(p_Cfg->IfStatus)
   {
      case  ROW_ACTIVE:
         p_IFO->Cfg.IfStatus = (p_IFO->Cfg.AdminStat) ?
                                 ROW_ACTIVE : ROW_NOT_IN_SERVICE;
         return E_OK;
      case  ROW_NOT_IN_SERVICE:
         p_IFO->Cfg.IfStatus = ROW_NOT_IN_SERVICE;
         return E_OK;
      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:
         memcpy(&p_IFO->Cfg, p_Cfg, sizeof(t_IFO_Cfg));

         p_IFO->Cfg.State = IFO_DOWN;    /* Current state */

         /* If this is a virtual IFO then use the virtual neighbor's router ID
          * as the IfoId.
         */
         if(p_IFO->Cfg.Type == IFO_VRTL)
           p_IFO->IfoId = p_IFO->Cfg.VirtIfNeighbor;
         else
           p_IFO->IfoId = p_IFO->Cfg.IpAdr ? p_IFO->Cfg.IpAdr : p_IFO->Cfg.IfIndex;

         /* set default IP MTU size if not defined by user */
         if(!p_IFO->Cfg.MaxIpMTUsize)
            p_IFO->Cfg.MaxIpMTUsize = OSPF_MAX_IP_MTU;

         p_IFO->Cfg.IfStatus = p_IFO->Cfg.AdminStat ?
            ((p_Cfg->IfStatus == ROW_CREATE_AND_GO) ?
               ROW_ACTIVE : ROW_NOT_READY): ROW_NOT_IN_SERVICE;

#if L7_OSPF_TE
         p_IFO->TeCosMask = 1;
         memset(p_IFO->TePdr, 0, 32*sizeof(float32));
         p_IFO->TePdr[0][0] = p_IFO->Cfg.TeMaxReservBandWidth;
#endif

#if L7_MIB_OSPF
         /* If the interface object has been created successfully, */
         /* create and activate row in MIB OSPF Interface table.   */

         if (p_IFO->Cfg.Type == IFO_VRTL)
         {
            ospfVirtIfTable_InternalNewAndActive(
                        ((t_RTO *)p_IFO->RTO_Id)->ospfMibHandle,
                        p_IFO->Cfg.VirtTransitAreaId,
                        p_IFO->Cfg.VirtIfNeighbor, (t_Handle)p_IFO);
         }
         else
         {
            ospfIfTable_InternalNewAndActive(
                         ((t_RTO *)p_IFO->RTO_Id)->ospfMibHandle,
                         p_IFO->Cfg.IpAdr, p_IFO->Cfg.IfIndex,
                         (t_Handle)p_IFO);
         }
#endif

      break;
      case  ROW_CHANGE:
         State = p_IFO->Cfg.State;
         areaId = p_IFO->Cfg.AreaId;
         oldHelloInt = p_IFO->Cfg.HelloInterval;
         oldPassiveMode = p_IFO->Cfg.passiveMode;
         memcpy(&p_IFO->Cfg, p_Cfg, sizeof(t_IFO_Cfg));
         p_IFO->Cfg.State = State;
         p_IFO->Cfg.AreaId = areaId;

         /* If this is a virtual IFO then use the virtual neighbor's router ID
          * as the IfoId.
         */
         if(p_IFO->Cfg.Type == IFO_VRTL)
           p_IFO->IfoId = p_IFO->Cfg.VirtIfNeighbor;
         else
           p_IFO->IfoId = p_IFO->Cfg.IpAdr ? p_IFO->Cfg.IpAdr :
                                           p_IFO->Cfg.IfIndex;

         p_IFO->Cfg.IfStatus = (p_IFO->Cfg.AdminStat) ?
                                 ROW_ACTIVE : ROW_NOT_IN_SERVICE;

         /* Check the DR/BDR status */
         DoHelloTransition(p_IFO, IFO_NGB_CHANGE);

       /* If the passive mode of an interface changes */
       if (oldPassiveMode != p_IFO->Cfg.passiveMode)
       {
         if(p_IFO->Cfg.passiveMode == TRUE)
         {
            /* If the interface mode is passive, that interface shall not
             * participate in sending hellos (hence no adjacencies over this i/f) */
            DoHelloTransition(p_IFO, IFO_INTERFACE_PASSIVE);
         }
         else
         {
            /* If the interface mode is non-passive, that interface shall be
             * available to form adjacencies */
            DoHelloTransition(p_IFO, IFO_INTERFACE_NON_PASSIVE);
         }
       }
       /* If hello interval has changed and we are sending hellos on this
        * interface, restart the hello timer with the new hello interval. */
       if ((p_IFO->Cfg.State != IFO_DOWN) && (p_IFO->Cfg.Type != IFO_LOOPBACK) &&
           (oldHelloInt != p_IFO->Cfg.HelloInterval) && (p_IFO->Cfg.passiveMode != TRUE))
       {
         /* Since we may lose a hello by restarting the timer, go ahead and
          * send one now. */
         IFO_SendHello(p_IFO, 0, 0);
         TIMER_StartSecMQueue(p_IFO->HelloTimer, p_IFO->Cfg.HelloInterval,
                              TRUE, HelloTimerExp, p_RTO->OspfSysLabel.threadHndle,
                              OSPF_HELLO_QUEUE );
       }

         /* This may be a bit heavy-handed. Motivated by the need to reissue the
          * router LSA when the set of secondary IP addresses changes on the
          * interface. Some other interface config changes may not require a
          * new router LSA. Logic could be added to be more selective about when
          * to reoriginate. */
         LsaReOriginate[S_ROUTER_LSA](p_IFO->ARO_Id, 0);

#if L7_MIB_OSPF
        if (p_IFO->Cfg.Type == IFO_VRTL)
        {
          p_Cfg->VirtIfNeighbor = p_IFO->Cfg.VirtIfNeighbor;
          p_Cfg->VirtTransitAreaId = p_IFO->Cfg.VirtTransitAreaId;
          ospfVirtIfTable_Update(((t_RTO *)p_IFO->RTO_Id)->ospfMibHandle, p_Cfg);
        }
        else
        {
          p_Cfg->IpAdr = p_IFO->Cfg.IpAdr;
          p_Cfg->IfIndex = p_IFO->Cfg.IfIndex;
          ospfIfTable_Update(((t_RTO *)p_IFO->RTO_Id)->ospfMibHandle, p_Cfg);
        }
#endif


         return E_OK;
      case  ROW_READ:
         memcpy(p_Cfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
         p_Cfg->State += 1; /* convert state in reporting to user */
         return E_OK;
      case  ROW_DESTROY:
         return IFO_Delete(IFO_Id, 1);
      default:
         return E_BADPARM;
   }

   /* Serve the virtual IFO configuration */
   if(p_IFO->Cfg.Type == IFO_VRTL)
   {
      if(p_IFO->Cfg.VirtTransitAreaId == OSPF_BACKBONE_AREA_ID)
         return E_BADPARM;

      /* Find non-stub transit area */
      er = HL_FindFirst(p_RTO->AroHl, (byte*)&p_IFO->Cfg.VirtTransitAreaId,
                        (void *)&transitArea);
      if (er == E_OK)
      {
        p_IFO->TransitARO = transitArea;
      }
      else
      {
        /* If the transit area is not found create it */
        p_IFO->Cfg.AreaId = p_IFO->Cfg.VirtTransitAreaId;
        if ((er = ARO_Init(p_IFO, (void *)&transitArea)) != E_OK)
        {
          p_IFO->Cfg.IfStatus = ROW_NOT_IN_SERVICE;
          return er;
        }
        p_IFO->TransitARO = transitArea;
      }

      /* Virtual link can't transit a stub area */
      if(((t_ARO *)p_IFO->TransitARO)->ExternalRoutingCapability != AREA_IMPORT_EXTERNAL)
      {
         p_IFO->TransitARO = NULL;
         return E_BADPARM;
      }

      p_IFO->Cfg.AreaId = OSPF_BACKBONE_AREA_ID;

      /* If backbone area not found, create the new one */
      if(HL_FindFirst(p_RTO->AroHl, (byte*)&backbone, (void *)&bbArea) != E_OK)
      {
         if((er = ARO_Init(p_IFO, (void *)&bbArea)) != E_OK)
         {
            p_IFO->Cfg.IfStatus = ROW_NOT_IN_SERVICE;
            return er;
         }
      }

      /* virtual interface belongs to the backbone */
      p_IFO->ARO_Id = bbArea;
      /* Assigning transit area id where IfoId + TransitAreadId are used as key
       * in IfoHl in backbone area */
      p_IFO->TransitAreaId = p_IFO->Cfg.VirtTransitAreaId;

      /* Add this virtual IFO into backbone ARO Ifos list */
      if((e = HL_SortedInsert(bbArea->IfoHl, (byte*)&p_IFO->IfoId, p_IFO)) != E_OK)
      {
         p_IFO->Cfg.IfStatus = ROW_NOT_IN_SERVICE;
         return e;
      }

      /* Add this IFO into RTO Ifos by ifIndex list */
      if ((e = HL_Insert(p_RTO->IfoIndexHl, (byte*)&p_IFO->Cfg.IfIndex, p_IFO)) != E_OK)
      {
         HL_Delete(bbArea->IfoHl, (byte*)&p_IFO->IfoId, p_IFO);
         p_IFO->Cfg.IfStatus = ROW_NOT_IN_SERVICE;
         return e;
      }

      /* Check if the virtual neighbor entry already exists in the RTB */
      if(HL_FindFirst(p_RTO->RtbHl, (byte *)&zero, (void *)&p_RTB) == E_OK)
        CheckVirtLinkUp(p_IFO, p_RTB);

      /* If this virtual interface is the first interface in the backbone area, and if
       * the router is an ABR, generate summary LSAs for the area. RFC 2328
       * section 12.4, item 7. */
      if ((HL_GetEntriesNmb(bbArea->IfoHl, &numIfInArea) == E_OK) &&
          (numIfInArea == 1))
      {
        if (routerIsBorder(p_RTO))
        {
          LsaOrgSumBulk(bbArea);
        }
      }

      return E_OK;
   }

   /* if after create and wait/go commands the IFO status
      is active or not ready, then find the corresponded ARO and
      add the IFO into ARO list */
   if(p_IFO->Cfg.IfStatus != ROW_NOT_IN_SERVICE)
   {
       t_ARO *p_ARO;    /* interface's area */
      /* Find ARO according to AreaId */
      if(HL_FindFirst(p_RTO->AroHl, (byte*)&p_IFO->Cfg.AreaId, (void *)&p_ARO) != E_OK)
      {
        /* If not found, create the new one */
        if((e = ARO_Init(p_IFO, (void *)&p_ARO)) != E_OK)
        {
            p_IFO->Cfg.IfStatus = ROW_NOT_IN_SERVICE;
            return e;
        }
      }

      p_IFO->ARO_Id = p_ARO;

      if(HL_FindFirst(p_ARO->IfoHl, (byte*)&p_IFO->IfoId, NULL) == E_OK)
         return E_BADPARM;

      /* Add this IFO into ARO Ifos list */
      if((e = HL_SortedInsert(p_ARO->IfoHl, (byte*)&p_IFO->IfoId, p_IFO)) != E_OK)
      {
         p_IFO->Cfg.IfStatus = ROW_NOT_IN_SERVICE;
         return e;
      }


      /* Add this IFO into RTO Ifos by ifIndex list */
      if((e = HL_Insert(p_RTO->IfoIndexHl, (byte*)&p_IFO->Cfg.IfIndex, p_IFO)) != E_OK)
      {
         HL_Delete(p_ARO->IfoHl, (byte*)&p_IFO->IfoId, p_IFO);
         p_IFO->Cfg.IfStatus = ROW_NOT_IN_SERVICE;
         return e;
      }


      /* if the RouterId hasn't assigned yet, set it right now */
      if(!p_RTO->Cfg.RouterId)
         p_RTO->Cfg.RouterId = p_IFO->Cfg.IpAdr;


      if(HL_FindFirst(p_RTO->RtbHl, (byte *) &zero, (void *)&p_RTB) != E_OK)
      {
         /* If not found then delete this IFO */
         IFO_Delete((t_Handle) p_IFO, 1);
         return E_FAILED;
      }

      /* If this interface is the first interface in its area, and if
       * the router is an ABR, generate summary LSAs for the area. RFC 2328
       * section 12.4, item 7. */
      if ((HL_GetEntriesNmb(p_ARO->IfoHl, &numIfInArea) == E_OK) &&
          (numIfInArea == 1))
      {
          if (routerIsBorder(p_RTO))
          {
              LsaOrgSumBulk(p_ARO);
          }
      }
   }
   return E_OK;
}




/*********************************************************************
 * @purpose      IFO object metric get/set depend on MetricStatus
 *               field value.
 *
 *
 * @param IFO_Id     @b{(input)}  IFO object Id
 * @param p_Metric   @b{(input)}  IFO Metric configuration (see spcfg.h)
 *
 * @returns   E_OK           config successfuly setup or read
 * @returns   E_FAILED       couldn't setup or read
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err IFO_MetricConfig(t_Handle IFO_Id, t_S_IfMetric *p_Metric)
{
   t_IFO *p_IFO = (t_IFO *) IFO_Id;
   t_MetricEntry *p_M;
   e_Err e=E_OK;
   word  command;

   /* try to find exist metric entry */
   if(p_Metric->MetricStatus != ROW_READ_NEXT)
      e = HL_FindFirst(p_IFO->MetricHl, (byte*)&p_Metric->MetricTOS, (void *)&p_M);

   command = p_Metric->MetricStatus;
   switch(p_Metric->MetricStatus)
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

         if((p_M = XX_Malloc(sizeof(t_MetricEntry))) == NULL)
            return E_NOMEMORY;
         memset(p_M, 0, sizeof(t_MetricEntry));

         p_M->MetricTOS = p_Metric->MetricTOS;
         p_M->Metric = p_Metric->Metric;
         p_M->IsValid = TRUE;
         p_IFO->CfgMetric = p_M->Metric;

         e = HL_Insert(p_IFO->MetricHl, (byte*)&p_M->MetricTOS, p_M);

      break;
      case  ROW_CHANGE:
         if(e == E_OK)
         {
            p_M->MetricTOS = p_Metric->MetricTOS;
            p_M->Metric = p_Metric->Metric;
            p_M->IsValid = TRUE;
            p_IFO->CfgMetric = p_M->Metric;
         }
      break;
      case  ROW_READ:
         if(e == E_OK)
         {
            p_Metric->IpAdr = p_IFO->Cfg.IpAdr;
            p_Metric->IfIndex = p_IFO->Cfg.IfIndex;
            p_Metric->MetricTOS = p_M->MetricTOS;
            p_Metric->Metric = p_M->Metric;
            p_Metric->MetricStatus = p_M->IsValid ? ROW_ACTIVE : ROW_NOT_IN_SERVICE;
         }
      break;
      case  ROW_READ_NEXT:
         if(p_Metric->PrevEntry == NULL)
            e = HL_GetFirst(p_IFO->MetricHl, (void *)&p_M);
         else
            e = HL_GetNext(p_IFO->MetricHl, (void *)&p_M, p_Metric->PrevEntry);

         if(e == E_OK)
         {
            memcpy(p_Metric, p_M, sizeof(t_MetricEntry));
            p_Metric->PrevEntry =  p_M;
         }
      break;
      case  ROW_DESTROY:

         e = HL_DeleteFirst(p_IFO->MetricHl, (byte*)&p_Metric->MetricTOS, (void *)&p_M);
         if(e == E_OK)
            XX_Free(p_M);

      break;
      default:
         e = E_FAILED;
      break;
   }

#if L7_MIB_OSPF
   /* Update the OSPF MIB Interface Metric table */
   if ((e == E_OK) && (p_IFO->Cfg.Type != IFO_VRTL) && (command != ROW_READ)
       && (command != ROW_READ_NEXT))
   {
      p_Metric->IfIndex = p_IFO->Cfg.IfIndex;
      p_Metric->IpAdr   = p_IFO->Cfg.IpAdr;
      ospfIfMetricTable_Update( ((t_RTO *)p_IFO->RTO_Id)->ospfMibHandle,
                                p_Metric);
   }
#endif

   /* If some change occured, then reoriginate the LSA */
   if((e == E_OK) && (command != ROW_READ) && (command != ROW_READ_NEXT) &&
      (command != ROW_CREATE_AND_WAIT))
         LsaReOriginate[S_ROUTER_LSA](p_IFO->ARO_Id, 0);

   return e;
}





/*********************************************************************
 * @purpose             NBMA type IFO object's NBMA routers list
 *                      entry get/set.
 *
 *
 * @param IFO_Id        @b{(input)}  IFO object Id
 * @param p_NbmaCfg     @b{(input)}  IFO NBMA configuration (see spcfg.h
 *
 * @returns  E_OK           config successfuly setup or read
 * @returns  E_FAILED       couldn't setup or read
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err IFO_NbmaConfig(t_Handle IFO_Id, t_S_IfNbmaCfg *p_NbmaCfg)
{
   t_IFO *p_IFO = (t_IFO *) IFO_Id;
   t_S_NbmaRtrCfg *p_Nbma;
   t_Handle nbo;
   e_Err e=E_OK;

   /* try to find exist NBMA entry */
   if(p_NbmaCfg->NbmaStatus != ROW_READ_NEXT)
      e = HL_FindFirst(p_IFO->NbmaHl, (byte*)&p_NbmaCfg->RouterId, (void *)&p_Nbma);

   switch(p_NbmaCfg->NbmaStatus)
   {
      case  ROW_ACTIVE:
         if(e == E_OK)
            p_Nbma->IsValid = TRUE;
      break;
      case  ROW_NOT_IN_SERVICE:
         if(e == E_OK)
            p_Nbma->IsValid = FALSE;
      break;
      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:

         if(e == E_OK) /* if already exist */
            return E_FAILED;

         if((p_Nbma = XX_Malloc(sizeof(t_S_NbmaRtrCfg))) == NULL)
            return E_NOMEMORY;
         memset(p_Nbma, 0, sizeof(t_S_NbmaRtrCfg));

         p_Nbma->RouterId = p_NbmaCfg->RouterId;
         p_Nbma->IpAdr    = p_NbmaCfg->IpAdr;
         p_Nbma->CanBeDr  = p_NbmaCfg->CanBeDr;
         p_Nbma->IsValid  = TRUE;

         e = HL_Insert(p_IFO->NbmaHl, (byte*)&p_Nbma->RouterId, p_Nbma);

      break;
      case  ROW_CHANGE:
         if(e == E_OK)
         {
            p_Nbma->RouterId = p_NbmaCfg->RouterId;
            p_Nbma->IpAdr    = p_NbmaCfg->IpAdr;
            p_Nbma->CanBeDr  = p_NbmaCfg->CanBeDr;
            p_Nbma->IsValid  = TRUE;
         }
      break;
      case  ROW_READ:
         if(e == E_OK)
         {
            p_NbmaCfg->RouterId   = p_Nbma->RouterId;
            p_NbmaCfg->IpAdr      = p_Nbma->IpAdr;
            p_NbmaCfg->CanBeDr    = p_Nbma->CanBeDr;
            p_NbmaCfg->NbmaStatus = p_Nbma->IsValid ? ROW_ACTIVE : ROW_NOT_IN_SERVICE;
         }
      break;
      case  ROW_READ_NEXT:
         if(p_NbmaCfg->PrevEntry == NULL)
            e = HL_GetFirst(p_IFO->NbmaHl, (void *)&p_Nbma);
         else
            e = HL_GetNext(p_IFO->NbmaHl, (void *)&p_Nbma, p_NbmaCfg->PrevEntry);

         if(e == E_OK)
         {
            memcpy(p_NbmaCfg, p_Nbma, sizeof(t_S_NbmaRtrCfg));
            p_NbmaCfg->PrevEntry =  p_Nbma;
         }
      break;
      case  ROW_DESTROY:

         e = HL_DeleteFirst(p_IFO->NbmaHl, (byte*)&p_NbmaCfg->RouterId, (void *)&p_Nbma);
         if(e == E_OK)
            XX_Free(p_Nbma);

         if(HL_FindFirst(p_IFO->NboHl, (byte *)&p_NbmaCfg->RouterId, (void *)&nbo) == E_OK)
            NBO_Delete(nbo, 1);

      break;
      default:
      break;
   }
   return e;
}



/*********************************************************************
 * @purpose                IFO object's Authentication Keys get/set.
 *
 *
 * @param IFO_Id           @b{(input)}  IFO object Id
 * @param p_Cfg            @b{(input)}  IFO Auth Keys configuration
 *                                      (see spcfg.h)
 *
 * @returns  E_OK           config successfuly setup or read
 * @returns  E_FAILED       couldn't setup or read
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err IFO_AuthKeyConfig(t_Handle IFO_Id, t_OspfAuthKey *p_Cfg)
{
   t_IFO *p_IFO = (t_IFO *) IFO_Id;
   t_OspfAuthKey *p_Key = NULL;
   e_Err e=E_OK;

   /* try to find exist NBMA entry */
   if(p_Cfg->KeyStatus != ROW_READ_NEXT)
      e = HL_FindFirst(p_IFO->AuthKeyHl, &p_Cfg->KeyId, (void *)&p_Key);

   switch(p_Cfg->KeyStatus)
   {
      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:

         if(e == E_OK) /* if already exist */
            return E_FAILED;

         if((p_Key = XX_Malloc(sizeof(t_OspfAuthKey))) == NULL)
            return E_NOMEMORY;

         memcpy(p_Key, p_Cfg, sizeof(t_OspfAuthKey));

         e = HL_Insert(p_IFO->AuthKeyHl, &p_Cfg->KeyId, p_Key);

      break;
      case  ROW_CHANGE:
         if(e == E_OK)
         {
            memcpy((byte *)p_Key + 2*sizeof(ulng),
                   (byte *)p_Cfg + 2*sizeof(ulng),
                   sizeof(t_OspfAuthKey) - 2*sizeof(ulng));
         }
      break;
      case  ROW_READ:
         if(e == E_OK)
         {
            memcpy((byte *)p_Cfg + 2*sizeof(ulng),
                   (byte *)p_Key + 2*sizeof(ulng),
                   sizeof(t_OspfAuthKey) - 2*sizeof(ulng));
         }
      break;
      case  ROW_READ_NEXT:
         if(p_Cfg->PrevEntry == NULL)
            e = HL_GetFirst(p_IFO->AuthKeyHl, (void *)&p_Key);
         else
            e = HL_GetNext(p_IFO->AuthKeyHl, (void *)&p_Key, p_Cfg->PrevEntry);

         if(e == E_OK)
         {
            memcpy(p_Cfg, p_Key, sizeof(t_OspfAuthKey));
            p_Cfg->PrevEntry =  p_Key;
         }
      break;
      case  ROW_DESTROY:

         e = HL_DeleteFirst(p_IFO->AuthKeyHl, &p_Cfg->KeyId, (void *)&p_Key);
         if(e == E_OK)
            XX_Free(p_Key);

      break;
      default:
      break;
   }
   return e;
}

/*********************************************************************
 * @purpose         Returns the maximum allowed OSPF buffer size
 *                  for a packet sent on this interface
 *
 * @param p_IFO     @b{(input)}  interface object
 * @param pktType   @b{(input)}  OSPF packet type
 *
 * @returns          returns Pkt length if success or NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
ulng GetMaxOspfBufLength(t_IFO *p_IFO, e_S_PckType pktType)
{
   ulng maxBuf, ipMtu;

   if(!p_IFO)
      return 0;

   ipMtu = p_IFO->Cfg.MaxIpMTUsize;

   switch(pktType)
   {
      case S_HELLO:
         maxBuf = OSPF_MAX_HELLO_PKT_SIZE;
         break;
      case S_DB_DESCR:
      case S_LS_REQUEST:
      case S_LS_ACK:
         /* If IP MTU is < 128, assign 128. If IP MTU is > 8K, assign 8K.
            Else assign IP MTU */
         if(ipMtu <= OSPF_MIN_BUF_SIZE)
         {
            maxBuf = OSPF_MIN_BUF_SIZE;
         }
         else if(ipMtu >= OSPF_MAX_BUF_SIZE)
         {
            maxBuf = OSPF_MAX_BUF_SIZE;
         }
         else
         {
            maxBuf = ipMtu;
         }
         break;
      case S_LS_UPDATE:
         /* To allow larger Router-LSAs and Network-LSAs, set to 8K */
         maxBuf = OSPF_MAX_BUF_SIZE;
         break;
      default:
         maxBuf = 0;
         break;
   }
   return maxBuf;
}

/*********************************************************************
 * @purpose         Allocate new packet and set offset to (MAC + IP + OSPF)
 *                  headers length + required offset.
 *
 *
 * @param p_IFO       @b{(input)}  Interface Object
 * @param pktType     @b{(input)}  OSPF packet type
 * @param offset     @b{(input)}  required packet header offset
 * @param allocPktLen @b{(output)} length of the packet allocated
 *
 * @returns          returns packet handler if successed or NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void *IFO_AllocPacket(t_IFO *p_IFO, e_S_PckType pktType,
                      word offset, ulng *allocPktLen)
{
   void *p_F = NULL;
   void *p_B;
   ulng maxBufLen = GetMaxOspfBufLength(p_IFO, pktType);

   if(allocPktLen == NULL)
       return NULL;

   *allocPktLen = 0;
   if(! maxBufLen)
   {
       L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
       nimGetIntfName(p_IFO->Cfg.IfIndex, L7_SYSNAME, ifName);

       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
               "Max allowed OSPF pkt len on intf %d, %s is zero", p_IFO->Cfg.IfIndex, ifName);
       return NULL;
   }

   p_B = B_NewEx(0, 0, maxBufLen);
   if (p_B == NULL)
   {
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
               "Unable to allocate buffer for OSPF packet in IFO_AllocPacket().");
       return NULL;
   }

   if(p_B)
   {
      B_SetOffset(p_B, (IP_HDR_LEN + sizeof(t_S_PckHeader) + offset));
      p_F = F_NewEx(p_B,0,0,0);
      if (p_F == NULL)
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
                  "Unable to allocate buffer for OSPF packet in IFO_AllocPacket().");
   }
   *allocPktLen = maxBufLen;

   return p_F;
}

/*********************************************************************
 * @purpose                  Lower layer Link Up indication.
 *
 *
 * @param Id                 @b{(input)}  Hello object Id
 *
 * @returns   E_OK           passed OK
 * @returns   E_FAILED       wrong IFO handler used in parameter
 *
* @notes  An UP event is generated in these cases:
 *
 *           - when OSPF becomes administratively enabled on the interface
 *           - when an interface with OSPF enabled comes up
 *           - when the area type changes (stub, nssa, normal)
 *           - the area ID on the interface changes
 *           - when the interface is released by a LAG or port mirror
 *
 *         For a virtual link, the UP event is generated when the
 *         router computes an intra-area route across the transit
 *         area to the virtual neighbor.
 *
 * @end
 * ********************************************************************/
e_Err IFO_Up(t_Handle Id)
{
   t_IFO *p_IFO = (t_IFO *) Id;
   t_RTO *p_RTO = (t_RTO *) p_IFO->RTO_Id;

   if(!p_IFO->Taken || !p_RTO)
       return E_FAILED;

   if(p_IFO->Cfg.State != IFO_DOWN)
     return E_OK;

   /* If in an unplanned restart and we have not yet sent grace LSAs on
    * this interface, then suppress tx and rx of hellos on this interface. */
   if ((p_RTO->restartStatus == OSPF_GR_UNPLANNED_RESTART) && !p_IFO->graceLsaSent)
   {
     /* This has to be done before the IFO state transition to ensure
      * OSPF doesn't send a hello before getting the grace LSAs out. */
     p_IFO->grSuppressHellos = L7_TRUE;
     TIMER_StartSecMQueue(p_IFO->grSuppressHelloTimer, 3 * p_IFO->Cfg.RxmtInterval,
                          FALSE, o2SuppressHelloTimerExp, p_RTO->OspfSysLabel.threadHndle,
                          OSPF_EVENT_QUEUE);
   }

   if(p_RTO->Cfg.AdminStat && (p_IFO->Cfg.IfStatus == ROW_ACTIVE))
   {
      DoHelloTransition(p_IFO, IFO_INTERFACE_UP);
   }

   if (p_RTO->restartStatus == OSPF_GR_UNPLANNED_RESTART)
   {
     /* Now send grace LSAs */
     o2GrIfoUp(p_IFO);
   }

   NssaHandleIFOUpEvent(p_IFO);

   return E_OK;
}




/*********************************************************************
 * @purpose          Lower layer Link Down indication.
 *
 *
 * @param Id         @b{(input)}  Hello object Id
 *
 * @returns          E_OK             passed OK
 * @returns          E_FAILED       wrong IFO handler used in parameter
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err  IFO_Down(t_Handle Id)
{
   t_IFO *p_IFO = (t_IFO *) Id;
   t_RTO *p_RTO = (t_RTO *) p_IFO->RTO_Id;
   Bool  adminStat;
   e_A_RowStatus ifoStat;
   byte priority;

   if(!p_IFO->Taken || !p_RTO)
      return E_FAILED;

   if(p_IFO->Cfg.State == IFO_DOWN)
     return E_OK;

   /* Delete all neighbors */
   DoHelloTransition(p_IFO, IFO_INTERFACE_DOWN);

   /* Save current admin states */
   adminStat = p_RTO->Cfg.AdminStat;
   ifoStat   = p_IFO->Cfg.IfStatus;
   priority  = p_IFO->Cfg.RouterPriority;

   /* Temporary reset priority for neighbors faster DR calculation */
   p_IFO->Cfg.RouterPriority = 0;

   if(!p_RTO->Cfg.AdminStat || (p_IFO->Cfg.IfStatus != ROW_ACTIVE))
   {
      /* Temporary set the admin state to active */
      p_RTO->Cfg.AdminStat = TRUE;
      p_IFO->Cfg.IfStatus = ROW_ACTIVE;
   }

   /* Restore admin states */
   p_RTO->Cfg.AdminStat      = adminStat;
   p_IFO->Cfg.IfStatus       = ifoStat;
   p_IFO->Cfg.RouterPriority = priority;

   NssaHandleIFODownEvent(p_IFO);

   return E_OK;
}


/*********************************************************************
 * @purpose         Lower layer Link Loopback/Unloopback indication.
 *
 *
 * @param Id        @b{(input)}  Hello object Id
 * @param IsOn      @b{(input)}  Loopback On(1)/Off(0)
 *
 * @returns   E_OK           passed OK
 * @returns   E_FAILED       wrong IFO handler used in parameter
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err  IFO_Loop(t_Handle Id, Bool IsOn)
{
   t_IFO *p_IFO = (t_IFO *) Id;
   t_RTO     *p_RTO = (t_RTO *) p_IFO->RTO_Id;

   if(!p_IFO->Taken || !p_RTO)
       return E_FAILED;

   if(IsOn || (p_RTO->Cfg.AdminStat && (p_IFO->Cfg.IfStatus == ROW_ACTIVE)))
   {
      DoHelloTransition(p_IFO, (IsOn ? IFO_LOOP_IND : IFO_UNLOOP_IND));
   }
   return E_OK;
}

/*********************************************************************
 * @purpose    Find the virtual link that transits a given area and terminates
 *             at a given router. Assumes there can be at most a single virtual
 *             link across an area to a given router.
 *
 * @param transitArea     @b{(input)}  the transit area to which the virtual link belongs
 * @param remoteRouterId  @b{(input)}  the router ID of the remote end of the virtual
 *                                     link, in host byte order.
 *
 * @returns   pointer to the virtual link. NULL if not found.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle IFO_VirtLinkFind(t_Handle p_ARO, ulng remoteRouterId)
{
    e_Err er;
    t_IFO *p_IFO;
    t_ARO *transitArea = (t_ARO*) p_ARO;
    t_ARO *bbArea;
    t_RTO *p_RTO = (t_RTO*) transitArea->RTO_Id;
    SP_IPADR backbone = OSPF_BACKBONE_AREA_ID;

    /* Virtual links are stored as interfaces on the backbone area.
     * Get the backbone area. */
    if(HL_FindFirst(p_RTO->AroHl, (byte*)&backbone, (void *)&bbArea) != E_OK)
    {
        return NULL;
    }

    er = HL_GetFirst(bbArea->IfoHl, (void **)&p_IFO);
    while (er == E_OK)
    {
        if ((p_IFO->Cfg.Type == IFO_VRTL) &&
            (p_IFO->Cfg.VirtIfNeighbor == remoteRouterId) &&
            (p_IFO->Cfg.VirtTransitAreaId == transitArea->AreaId))
        {
            return p_IFO;
        }
        er = HL_GetNext(bbArea->IfoHl, (void **)&p_IFO, p_IFO);
    }

    return NULL;
}

/*********************************************************************
 * @purpose      Received OSPF packet processing.
 *
 *
 * @param Id           @b{(input)}  IFO object Id
 * @param p_Packet     @b{(input)}  pointer to the received Packet (starts from OSPF header)
 * @param SrcAdr       @b{(input)}  Source Address from IP header
 * @param DstAdr       @b{(input)}  Destination Address from IP header
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err  IFO_Received(t_Handle Id, void *p_Packet, SP_IPADR SrcAdr, SP_IPADR  DstAdr)
{
   t_IFO     *p_IFO = (t_IFO *) Id;
   t_ARO     *p_ARO = (t_ARO *)p_IFO->ARO_Id;
   t_RTO     *p_RTO = (t_RTO *) p_IFO->RTO_Id;
   t_NBO     *p_NBO = NULLP;
   t_S_PckHeader *p_Hdr;
   SP_IPADR  router;
   t_IfConfigError CfgErr;
   t_VirtIfConfigError VirtCfgErr;
   e_TrapConfigErrors errtype;
   e_OspfTrapType  traptype;
   t_IfRxBadPacket IfRxPacket;

   errtype = CONFIG_OK;

      p_Hdr = (t_S_PckHeader *)GET_LSA_HDR(p_Packet);

   if(!p_RTO->Cfg.AdminStat ||
      (p_IFO->Cfg.IfStatus != ROW_ACTIVE) ||
      (p_IFO->Cfg.State == IFO_DOWN) || (p_IFO->Cfg.passiveMode == TRUE))
      goto mDiscard;

   if(p_IFO->Cfg.State == IFO_LOOP)
      return IFO_SendPacket(p_IFO, p_Packet, p_Hdr->Type, 0, 0);

   /* check if this packet must be routed to the Virtual IFO */

   if((p_IFO->Cfg.Type != IFO_VRTL) &&
      (p_ARO->AreaId != OSPF_BACKBONE_AREA_ID) &&
      (A_GET_4B(p_Hdr->AreaId) == OSPF_BACKBONE_AREA_ID))
   {
       /* Find the virtual link on which the packet was received. */
       ulng remoteRouterId = A_GET_4B(p_Hdr->RouterId);
       t_IFO *virtIfo = (t_IFO*) IFO_VirtLinkFind(p_ARO, remoteRouterId);
       if (virtIfo)
           return IFO_Received(virtIfo, p_Packet, SrcAdr, DstAdr);
       else
       {
           p_IFO->PckStats.BadVirtualLink++;
           goto mDiscard;
       }
   }

   /* Check destination address. Virtual links can be asymmetric, so
    * the interface we send on may not be where we receive. So skip
    * this check for virtual links. */
   if (p_IFO->Cfg.Type != IFO_VRTL)
   {
       if((DstAdr != AllDRouters) && (DstAdr != AllSpfRouters) &&
          (DstAdr != p_IFO->Cfg.IpAdr))
       {
          p_IFO->PckStats.BadDstAdr += 1;
          goto mDiscard;
       }
   }

   /* Check source address */
   if(SrcAdr == p_IFO->Cfg.IpAdr)
   {
     /* Don't let a reflection of our own packet generate a trap */
     if((DstAdr == AllDRouters) || (DstAdr == AllSpfRouters))
     {
        p_IFO->PckStats.DiscardPackets += 1;
        F_Delete(p_Packet);
        return E_OK;
     }
     else
       goto mDiscard;

   }

   /* Check the OSPF packet header content */

   /* Check version */
   if(p_Hdr->Version != OSPF_VERSION)
   {
      p_IFO->PckStats.BadVersion += 1;
      errtype = BAD_VERSION;
      goto mDiscard;
   }

   /* Check Area Id */
   if(A_GET_4B(p_Hdr->AreaId) == p_ARO->AreaId)
   {
      /* Check if the source router attached to the same network */
      if((p_IFO->Cfg.Type != IFO_PTP) && (p_IFO->Cfg.Type != IFO_VRTL) &&
         ((SrcAdr & p_IFO->Cfg.IpMask) != (p_IFO->Cfg.IpAdr & p_IFO->Cfg.IpMask)))
      {
         p_IFO->PckStats.BadNetwork += 1;
         errtype = NET_MASK_MISMATCH;
         goto mDiscard;
      }
   }
   else
   {
      p_IFO->PckStats.BadArea += 1;
      errtype = AREA_MISMATCH;
      goto mDiscard;
   }

   if((DstAdr == AllDRouters) &&
      ((p_IFO->Cfg.State != IFO_DR) &&
       (p_IFO->Cfg.State != IFO_BACKUP)))
   {
      goto mLegalDisc;
   }

   /* Check Authentication */
   if(A_GET_2B(p_Hdr->AuType) != p_IFO->Cfg.AuType)
   {
      p_IFO->PckStats.BadAuType += 1;
      errtype = AUTH_TYPE_MISMATCH;
      goto mDiscard;
   }

   /* Find Neighbor */
   router = A_GET_4B(p_Hdr->RouterId);
   if (p_Hdr->Type != S_HELLO)
   {
      if (HL_FindFirst(p_IFO->NboHl, (byte *)&router, (void *)&p_NBO) != E_OK)
      {
          p_IFO->PckStats.BadNeighbor += 1;
          errtype = UNKNOWN_NBMA_NBR;
          goto mDiscard;
      }
      else
      {
          /* If packet received on interface other than p2p or virtual,
           * verify that source IP address is what we previously recorded
           * for this neighbor. */
          if ((p_IFO->Cfg.Type != IFO_PTP) && (p_IFO->Cfg.Type != IFO_VRTL))
          {
              if (p_NBO->IpAdr != SrcAdr)
              {
                  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
                  L7_uchar8 srcRouterIdStr[OSAPI_INET_NTOA_BUF_SIZE];
                  L7_uchar8 srcAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
                  L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
                  RTO_InterfaceName(p_IFO, ifName);
                  osapiInetNtoa((L7_uint32)router, srcRouterIdStr);
                  osapiInetNtoa((L7_uint32)SrcAdr, srcAddrStr);
                  osapiInetNtoa((L7_uint32)p_NBO->IpAdr, nbrAddrStr);
                  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID,
                          "Dropping %s on interface %s from router %s at IP address %s. "
                          "Neighbor address is %s."
                          " When OSPFv2 receives a packet, it finds the neighbor on the "
                          "ingress interface whose router ID matches the packet. If the "
                          "source IP address of the packet is different from the source "
                          "IP address of packets previously received from this neighbor, "
                          "the packet is dropped.",
                          ospfPckTypeNames[p_Hdr->Type], ifName, srcRouterIdStr,
                          srcAddrStr, nbrAddrStr);
                  p_IFO->PckStats.BadNeighbor += 1;
                  errtype = NBR_IPADDR_MISMATCH;
                  goto mDiscard;
              }
          }
      }
   }

   if(AuthPckDecoding(p_IFO, p_NBO, p_Packet) != E_OK)
   {
      p_IFO->PckStats.BadAuthentication += 1;
      errtype = AUTH_FAILURE;
      goto mDiscard;
   }

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_RX)
   {
     char traceBuf[OSPF_MAX_TRACE_STR_LEN];
     L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
     L7_uchar8 srcRouterIdStr[OSAPI_INET_NTOA_BUF_SIZE];
     RTO_InterfaceName(p_IFO, ifName);
     osapiInetNtoa((L7_uint32)router, srcRouterIdStr);
     sprintf(traceBuf, "Received OSPF %s on interface %s from %s",
             ospfPckTypeNames[p_Hdr->Type], ifName, srcRouterIdStr);
     RTO_TraceWrite(traceBuf);
   }

   if (ospfMapDebugPacketTraceFlagGet())
   {
       L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
       RTO_InterfaceName(p_IFO, ifName);
       ospfMapDebugPacketRxTrace(ifName,SrcAdr,DstAdr,(L7_char8 *)p_Hdr);
   }


   /* Call to farther packet processing procedure */
   switch(p_Hdr->Type)
   {
      case  S_HELLO:
         p_IFO->PckStats.RxHellos += 1;
         errtype = HelloReceived(p_IFO, p_Hdr, router, SrcAdr);
         if(errtype != CONFIG_OK)
            goto mDiscard;
      break;
      case  S_DB_DESCR:
         p_IFO->PckStats.RxDbDescr += 1;
         DbDescrReceived(p_NBO, p_Hdr);
      break;
      case  S_LS_REQUEST:
         p_IFO->PckStats.RxLsReq += 1;
         LsReqReceived(p_NBO, p_Hdr);
      break;
      case  S_LS_UPDATE:
         p_IFO->PckStats.RxLsUpdate += 1;
         p_RTO->RxLsUpdate++;
         LsUpdateReceived(p_NBO, p_Hdr);
      break;
      case  S_LS_ACK:
         p_IFO->PckStats.RxLsAck += 1;
         LsAckReceived(p_NBO, p_Hdr);
        break;
      default:
         p_IFO->PckStats.BadPckType += 1;
         errtype = BAD_PACKET;
         goto mDiscard;
      break;
   }

mLegalDisc:

   p_IFO->PckStats.RxPackets += 1;

   if(p_RTO->Clbk.p_OspfTrap &&
      (p_RTO->TrapControl & GET_TRAP_BITMASK(IF_RX_PACKET)))
   {
      IfRxPacket.RouterId = p_RTO->Cfg.RouterId;
      IfRxPacket.IfIpAddress = p_IFO->Cfg.IpAdr;
      IfRxPacket.AddressLessIf = p_IFO->Cfg.IfIndex;
      IfRxPacket.PacketSrc = SrcAdr;
      IfRxPacket.PacketType = p_Hdr->Type > S_LAST_PACKET_TYPE ? S_ILLEGAL : p_Hdr->Type;

      p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, IF_RX_PACKET, (u_OspfTrap*)&IfRxPacket);
   }

   /* trap above uses p_Hdr pointer into p_Packet, so delete p_Packet after trap. */
   F_Delete(p_Packet);

   return E_OK;

mDiscard:
   /* Discard the frame */
   p_IFO->PckStats.DiscardPackets += 1;
   p_IFO->Cfg.EventsCounter += 1;
   F_Delete(p_Packet);

   if(p_RTO->Clbk.p_OspfTrap)
   {
      if(p_IFO->Cfg.Type == IFO_VRTL)
      {
         if((errtype == AUTH_TYPE_MISMATCH) || (errtype == AUTH_FAILURE))
            traptype = VIRT_IF_AUTH_FAILURE;
         else if(errtype == BAD_PACKET)
            traptype = VIRT_IF_RX_BAD_PACKET;
         else
            traptype = VIRT_IF_CONFIG_ERROR;

         if(p_RTO->TrapControl & GET_TRAP_BITMASK(traptype))
         {
            VirtCfgErr.RouterId = p_RTO->Cfg.RouterId;
            VirtCfgErr.VirtIfAreaId = p_IFO->Cfg.VirtTransitAreaId;
            VirtCfgErr.VirtIfNeighbor = p_IFO->Cfg.VirtIfNeighbor;
            VirtCfgErr.ConfigErrorType = errtype > UKNOWN_ERROR ? UKNOWN_ERROR : errtype;
            VirtCfgErr.PacketType = p_Hdr->Type > S_LAST_PACKET_TYPE ? S_ILLEGAL : p_Hdr->Type;

            p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, traptype, (u_OspfTrap*)&VirtCfgErr);
         }
      }
      else
      {
         if((errtype == AUTH_TYPE_MISMATCH) || (errtype == AUTH_FAILURE))
            traptype = IF_AUTH_FAILURE;
         else if(errtype == BAD_PACKET)
            traptype = RX_BAD_PACKET;
         else
            traptype = IF_CONFIG_ERROR;

         if(p_RTO->TrapControl & GET_TRAP_BITMASK(traptype))
         {
            CfgErr.RouterId = p_RTO->Cfg.RouterId;
            CfgErr.IfIpAddress = p_IFO->Cfg.IpAdr;
            CfgErr.AddressLessIf = p_IFO->Cfg.IfIndex;
            CfgErr.PacketSrc = SrcAdr;
            CfgErr.ConfigErrorType = errtype > UKNOWN_ERROR ? UKNOWN_ERROR : errtype;
            CfgErr.PacketType = p_Hdr->Type > S_LAST_PACKET_TYPE ? S_ILLEGAL : p_Hdr->Type;

            p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, traptype, (u_OspfTrap*)&CfgErr);
         }
      }
   }
   return E_OK;
}


e_Err IFO_SendPacket(t_Handle Id, void *p_Packet,
                     e_S_PckType Type, t_Handle NboId, Bool IsRetx)
{
   t_IFO     *p_IFO = (t_IFO *) Id;
   t_RTO     *p_RTO = (t_RTO *) p_IFO->RTO_Id;
   t_NBO     *p_NBO = (t_NBO *) NboId;
   t_S_PckHeader *p_Hdr;
   void      *p_buf;
   word      len;
   SP_IPADR  DstAdr;
   SP_IPADR  LocRouterId = p_RTO->Cfg.RouterId;
   t_LIH     ifIndex = p_IFO->Cfg.IfIndex;

   if(!p_RTO->Cfg.AdminStat || (p_IFO->Cfg.IfStatus != ROW_ACTIVE))
   {
      F_Delete(p_Packet);
      return E_FAILED;
   }

   /* Compute the header length */
   len = sizeof(t_S_PckHeader);
   /* get the packet header pointer */
   p_buf = F_GetFirstBuf(p_Packet);
   /* Decrement buffer offset to the OSPF header length */
   BufDecOffset(p_buf, len);
   F_GetLength(p_Packet) += len;
   /* Get pointer to the header */
   p_Hdr = (t_S_PckHeader *)B_GetData(p_buf);

   /* Format the header */
   p_Hdr->Version = OSPF_VERSION;
    p_Hdr->Type = Type;
    A_SET_2B(F_GetLength(p_Packet), p_Hdr->Length);
   A_SET_4B(p_RTO->Cfg.RouterId, p_Hdr->RouterId);
   A_SET_4B(p_IFO->Cfg.AreaId, p_Hdr->AreaId);
   AuthPckCoding(p_IFO, p_Packet);

   /* Set the Destination IP Address */
   switch(p_IFO->Cfg.Type)
   {
      case IFO_PTP:
         DstAdr = AllSpfRouters;
         break;

      case IFO_BRDC:
      {
         switch (Type)
         {
            case S_HELLO:
               DstAdr = AllSpfRouters;
               break;

            case S_LS_UPDATE:
            case S_LS_ACK:
               if(!p_NBO)
               {
                  if((p_IFO->DrId == LocRouterId) ||
                     (p_IFO->BackupId == LocRouterId))
                     DstAdr = AllSpfRouters;
                  else
                     DstAdr = AllDRouters;
               }
               else
                  DstAdr = p_NBO->IpAdr;
               break;

            default:   /* Database Descr or LS_Request packet */
               ASSERT(p_NBO);
               if (!p_NBO)
                  return E_FAILED;
               DstAdr = p_NBO->IpAdr;
         }
         break;
      }

      case IFO_VRTL:
         DstAdr = p_IFO->VrtNgbIpAdr;
         ifIndex = p_IFO->VirtIfIndex;
         break;

      default:    /* Point to multipoint or NBMA link types*/
         ASSERT(p_NBO);
         if (!p_NBO)
            return E_FAILED;
         DstAdr = p_NBO->IpAdr;
   }

   p_IFO->PckStats.TxPackets += 1;
   if (Type == S_LS_UPDATE)
   {
     p_RTO->TxLsUpdate++;
   }

   if (ospfMapDebugPacketTraceFlagGet())
   {
       L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
       RTO_InterfaceName(p_IFO, ifName);
       ospfMapDebugPacketTxTrace(ifName,LocRouterId,DstAdr,(L7_char8 *)p_Hdr);
   }

   switch(Type)
   {
   case  S_HELLO:
      p_IFO->PckStats.TxHellos++;
      break;
   case  S_DB_DESCR:
      p_IFO->PckStats.TxDbDescr++;
      break;
   case  S_LS_REQUEST:
      p_IFO->PckStats.TxLsReq++;
      break;
   case  S_LS_UPDATE:
      p_IFO->PckStats.TxLsUpdate++;
      break;
   case  S_LS_ACK:
      p_IFO->PckStats.TxLsAck++;
      break;
   default:
      break;
   }

   return p_RTO->Clbk.f_IpForward(p_IFO->LowLayerId, p_Packet, DstAdr, ifIndex);
}

/*--------------------------------------------------------------------
 * ROUTINE:    IFO_LsUpdateTx
 *
 * DESCRIPTION:
 *    Transmit an LS UPDATE on a given interface. If the interface is
 *    not broadcast or p2p, send UPDATE separately to each neighbor.
 *
 * ARGUMENTS:
 *    IfoId     IFO object pointer
 *    p_F       Frame containing LS Update packet, not including common
 *              OSPF header. The frame is deleted after it is transmitted
 *              and should not be referenced again after calling this
 *              function.
 *
 * RESULTS:
 *    E_OK
 *--------------------------------------------------------------------*/
e_Err IFO_LsUpdateTx(t_Handle IfoId, void *p_F)
{
  t_IFO *p_IFO = (t_IFO*) IfoId;
  t_NBO *p_NBO = NULL;
  e_Err er;

  if ((p_IFO->Cfg.Type == IFO_PTP) || (p_IFO->Cfg.Type == IFO_BRDC))
  {
    /* Transmit the LS Update packet as broadcast on this IFO */
    IFO_SendPacket(p_IFO, p_F, S_LS_UPDATE, NULL, FALSE);
  }
  else
  {
    /* Transmit the LS Update packet directly to each neighbor */
    er = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
    while (er == E_OK)
    {
      if (p_NBO->State >= NBO_EXCHANGE)
      {
        void *frame = F_Copy(p_F);
        if (frame)
          IFO_SendPacket(p_IFO, frame, S_LS_UPDATE, p_NBO, FALSE);
      }
      er = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
    }
    F_Delete(p_F);
  }
  return E_OK;
}

/*--------------------------------------------------------------------
 * ROUTINE:    IFO_SendPendingUpdate
 *
 * DESCRIPTION:
 *    Send the LS Update pending on a specific interface.
 *
 * ARGUMENTS:
 *    IfoId     IFO object pointer
 *
 * RESULTS:
 *    E_OK
 *--------------------------------------------------------------------*/
e_Err IFO_SendPendingUpdate(t_Handle IfoId)
{
  t_IFO *p_IFO = (t_IFO*) IfoId;
  void *p_F = p_IFO->PendingUpdate;

  if (p_F == NULL)
    return E_OK;

  IFO_LsUpdateTx(p_IFO, p_F);
  p_IFO->PendingUpdate = NULL;
  return E_OK;
}


/*********************************************************************
 * @purpose          Send the HELLO packet.
 *
 *
 * @param Id         @b{(input)}  IFO object Id
 * @param NboId      @b{(input)}  in case of NBMA replay this is source Neighbor
 * @param IsReplay   @b{(input)}  Hello sending in replay flag
 *
 * @returns          E_OK/E_FAILED
 *
 * @notes   All neighbors must be included in a single hello. Splitting
 *          neighbors across multiple hello packets would cause neighbors
 *          to bounce the adjacency on a 1-way received event.
 *
 * @end
 * ********************************************************************/
e_Err IFO_SendHello(t_Handle Id, t_Handle NboId, Bool IsReplay)
{
   t_IFO     *p_IFO = (t_IFO *) Id;
   t_RTO     *p_RTO = (t_RTO *) p_IFO->RTO_Id;
   SP_IPADR  router_id = p_RTO->Cfg.RouterId;
   t_S_Hello *p_Hlo;
   t_NBO     *p_NBO=NULLP;
   byte      *p_Ngb;
   void      *p_F;
   e_Err     e = E_OK, er = E_OK;
   word      ngb_num;
   ulng      allocPktLen;
   word      authLen = 0;

   if(!p_RTO->Cfg.AdminStat || (p_IFO->Cfg.IfStatus != ROW_ACTIVE))
      return E_FAILED;

   if (p_IFO->grSuppressHellos)
     return E_OK;

  ngb_num = 0;
  p_F = IFO_AllocPacket(p_IFO, S_HELLO, 0, &allocPktLen);
  if(!p_F)
     return E_FAILED;

  if (p_IFO->Cfg.AuType == IFO_CRYPT)
    authLen = OSPF_MD5_AUTH_DATA_LEN;

  /* Get pointer to Hello part of the packet */
  p_Hlo = (t_S_Hello *) GET_LSA_HDR(p_F);

  /* Fill HELLO packet fields */
  if (p_IFO->Cfg.Type == IFO_VRTL)
    memset(p_Hlo->NetworkMask, 0, 4);   /* RFC 2328 section 9.5 */
  else
    A_SET_4B(p_IFO->Cfg.IpMask,p_Hlo->NetworkMask);  /* IpMask is 0 for unnumbered links */
  A_SET_2B(p_IFO->Cfg.HelloInterval,p_Hlo->HelloInterval);

  /* O-bit is set only in DD packets */
  p_Hlo->Options = (((t_ARO *)p_IFO->ARO_Id)->ExternalRoutingCapability ==
                    AREA_IMPORT_EXTERNAL) ? OSPF_OPT_E_BIT : 0;
  p_Hlo->Options |= (((t_ARO *)p_IFO->ARO_Id)->ExternalRoutingCapability ==
                    AREA_IMPORT_NSSA) ? OSPF_OPT_N_BIT : 0;
  p_Hlo->RouterPriority = p_IFO->Cfg.RouterPriority;
  A_SET_4B(p_IFO->Cfg.RouterDeadInterval,p_Hlo->RouterDeadInterval);
  A_SET_4B(p_IFO->Cfg.DrIpAddr, p_Hlo->DesignatedRouter);
  A_SET_4B(p_IFO->Cfg.BackupIpAddr, p_Hlo->BackupRouter);
  p_Ngb = (byte*)p_Hlo + sizeof(t_S_Hello); /* points to start of the neighbors list */

  /* form the list of Neighbors */
  e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
  while(e == E_OK)
  {
     /* check if adding another neighbor would make hello exceed OSPF buffer len */
     if (((p_Ngb - (byte*)p_Hlo) + sizeof(SP_IPADR)) >
         (word)(allocPktLen - (sizeof(t_S_PckHeader) + IP_HDR_LEN + authLen)))
     {
        /* Log problem. Send hello with partial neighbor list. */
         L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
         RTO_InterfaceName(p_IFO, ifName);
         L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
                 "On interface %s cannot include all neighbors in Hello.",
                 ifName);
        break;
     }

     A_SET_4B(p_NBO->RouterId, p_Ngb);
     p_Ngb += 4;
     ngb_num += 1;

     e = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
  }

  /* Set packet length */
  B_SetLength(F_GetFirstBuf(p_F), (sizeof(t_S_Hello) + ngb_num*4));
  F_GetLength(p_F) = (sizeof(t_S_Hello) + ngb_num*4);

  switch(p_IFO->Cfg.Type)
  {
     /* Point-to-Multipoint or Virtual */
     case IFO_PTM:
        /* Send to each neighbor individual HELLO copy */
        e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
        while(e == E_OK)
        {
           er = IFO_SendPacket(p_IFO, F_Copy(p_F), S_HELLO, p_NBO, 0);

           if(er != E_OK)
              return er;

           p_IFO->TimeStamp = TICKS_TO_SECONDS(TIMER_SysTime());

           p_NBO->NbrHelloSuppressed = FALSE;

           e = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
        }
        F_Delete(p_F);
     break;
     case IFO_NBMA:

        if(p_IFO->Cfg.State >= IFO_WAIT)
        {
           if(IsReplay)
           {
              IFO_SendPacket(p_IFO, p_F, S_HELLO, NboId, 0);
           }
           else
           {
              Bool LocalIsEligible;
              Bool LocalIsDrOrBackup;
              Bool NeighborIsEligible;
              Bool NeighborIsDrOrBackup;

              LocalIsEligible = (p_IFO->Cfg.RouterPriority != 0) ? TRUE : FALSE;
              LocalIsDrOrBackup = ((p_IFO->DrId == router_id) ||
                                   (p_IFO->BackupId == router_id)) ?
                                   TRUE : FALSE;

              e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
              while(e == E_OK)
              {
                 NeighborIsEligible = p_NBO->RouterPriority ? TRUE : FALSE;
                 NeighborIsDrOrBackup = ((p_IFO->DrId == p_NBO->RouterId) ||
                                         (p_IFO->BackupId == p_NBO->RouterId)) ?
                                         TRUE : FALSE;
                 p_NBO->NbrHelloSuppressed = FALSE;

                 if(LocalIsDrOrBackup ||
                    (LocalIsEligible && NeighborIsEligible) ||
                    (!LocalIsEligible && NeighborIsDrOrBackup))
                    {
                       /* If neighbor is in DOWN state,
                          reduce HELLO sending speed */
                       if((p_NBO->State != NBO_DOWN) ||
                          (++p_NBO->HloIntCount == (ulng) (p_IFO->Cfg.NbmaPollInterval /
                                                           p_IFO->Cfg.HelloInterval)))
                       {
                          er = IFO_SendPacket(p_IFO, F_Copy(p_F), S_HELLO, p_NBO, 0);

                          p_NBO->HloIntCount = 0;
                          if(er != E_OK)
                             return er;

                          p_IFO->TimeStamp = TICKS_TO_SECONDS(TIMER_SysTime());
                       }
                       else
                          p_NBO->NbrHelloSuppressed = TRUE;
                    }
                    else
                       p_NBO->NbrHelloSuppressed = TRUE;

                 e = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
              }
              F_Delete(p_F);
           }
        }
        else
           F_Delete(p_F);
     break;

     /* point-to-point or broadcast or virtual */
     case IFO_VRTL:
     case IFO_PTP:
     case IFO_BRDC:
        er = IFO_SendPacket(p_IFO, p_F, S_HELLO, 0, 0);

        if (p_RTO->ospfTraceFlags & OSPF_TRACE_HELLO)
        {
           char traceBuf[OSPF_MAX_TRACE_STR_LEN];
           char ifName[L7_NIM_IFNAME_SIZE + 1];
           RTO_InterfaceName(p_IFO, ifName);
           sprintf(traceBuf, "OSPF Hello sent on interface %s.", ifName);
           RTO_TraceWrite(traceBuf);
        }

        p_IFO->TimeStamp = TICKS_TO_SECONDS(TIMER_SysTime());
     break;

     default:
        F_Delete(p_F);
        er = E_FAILED;
     break;
  }

  if(er != E_OK)
  {
    p_IFO->Cfg.EventsCounter += 1;
    return er;
  }
   return er;
}





/*********************************************************************
 * @purpose            Get IFO's Packet Statistics.
 *
 *
 * @param Id           @b{(input)}  IFO object Id
 * @param p_St         @b{(input)}  Statistics pointer returned here
 *
 * @returns            E_OK/E_FAILED
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err IFO_GetPckStatistic(t_Handle IFO_Id, t_PCK_Stats **p_St)
{
   t_IFO     *p_IFO = (t_IFO *) IFO_Id;

   *p_St = &p_IFO->PckStats;

   return E_OK;

}


/*********************************************************************
 * @purpose          Compute IP standard 16-bit complement checksum.
 *
 *
 * @param p_Buf      @b{(input)}   begin of data
 * @param length     @b{(input)}  length of data
 *
 * @returns          E_OK - success, otherwise E_FAILED
 *
 * @notes
 *
 * @end
 * ********************************************************************/
word SPF_ComputeIpCheckSum(void *p_Buf, word length)
{
  byte  *buff = (byte *) p_Buf;
  ulng  sum;

  for(sum = 0; length > 1; length = length - 2)
  {
    sum += (*buff<<8) + *(buff+1);
    buff+=2;
  }

  /* If the length was even, it is now zero, and we are done */
  /* If the length was odd, it is now  */
  if(length == 1)
     sum += *buff;

  /* Add the lower and upper 16 bits of the accumulated sum */
  sum = (sum >> 16) + (sum & 0xffff);

  /* If the sum becomes greater than 16 bits, add the carry bits back again */
  sum += (sum >> 16);

  return ~(word)sum;
}

/*********************************************************************
 * @purpose                Received HELLO processing.
 *
 *
 * @param p_IFO            @b{(input)}  IFO object Id
 * @param p_Hdr            @b{(input)}  pointer to the start of a received Packet
 * @param router           @b{(input)}  source router id
 *
 * @returns   e_TrapConfigErrors Config errors (see spcfg.h)
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_TrapConfigErrors HelloReceived(t_IFO *p_IFO, t_S_PckHeader *p_Hdr,
                                        SP_IPADR router, SP_IPADR SrcAdr)
{
   t_S_Hello *p_Hlo;
   t_NBO     *p_NBO;
   t_RTO     *p_RTO = (t_RTO *) p_IFO->RTO_Id;
   Bool      e_flag;
   byte      *p_Ngb;
   word      ngbnum;
   byte      PrevRouterPriority;
   SP_IPADR  PrevDrIpAddr;
   SP_IPADR  PrevBackupIpAddr;
   Bool      HelloSMSched = FALSE;
   e_Err     er = E_FAILED;
   ulng      now;

   if (p_IFO->grSuppressHellos)
     return E_OK;

   /* Get pointer to the HELLO payload */
   p_Hlo =  (t_S_Hello *) GET_PAYLOAD_PTR(p_Hdr);

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_HELLO)
   {
       char traceBuf[OSPF_MAX_TRACE_STR_LEN];
       L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
       L7_uchar8 srcRouterIdStr[OSAPI_INET_NTOA_BUF_SIZE];
       L7_uchar8 srcAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
       RTO_InterfaceName(p_IFO, ifName);
       osapiInetNtoa((L7_uint32)router, srcRouterIdStr);
       osapiInetNtoa((L7_uint32)SrcAdr, srcAddrStr);
       sprintf(traceBuf, "OSPF Hello received on interface %s from %s at %s.",
               ifName, srcRouterIdStr, srcAddrStr);
       RTO_TraceWrite(traceBuf);
   }

   /* Check Network mask if IFO is not PTP or VRTL */
   if((((p_IFO->Cfg.Type != IFO_PTP) && (p_IFO->Cfg.Type != IFO_VRTL)) &&
        (A_GET_4B(p_Hlo->NetworkMask) != p_IFO->Cfg.IpMask)))
      return NET_MASK_MISMATCH;

   /* Check HelloInterval */
   if(A_GET_2B(p_Hlo->HelloInterval) != p_IFO->Cfg.HelloInterval)
      return HELLO_INTERVAL_MISMATCH;

   /* Check RouterDeadInterval */
   if(A_GET_4B(p_Hlo->RouterDeadInterval) != p_IFO->Cfg.RouterDeadInterval)
      return DEAD_INTERVAL_MISMATCH;

   /* NSSA CHANGE: Check E-option flag (Changed condition) */
   e_flag = (p_Hlo->Options & OSPF_OPT_E_BIT) ? TRUE : FALSE;
   if(e_flag != (((t_ARO *)p_IFO->ARO_Id)->ExternalRoutingCapability ==
                  AREA_IMPORT_EXTERNAL))
      return OPTION_MISMATCH;

   /* NSSA CHANGE: Check N-option flag */
   e_flag = (p_Hlo->Options & OSPF_OPT_N_BIT) ? TRUE: FALSE;
   if(e_flag != (((t_ARO *)p_IFO->ARO_Id)->ExternalRoutingCapability ==
                  AREA_IMPORT_NSSA))
      return OPTION_MISMATCH;

   /* try to find match neighbor */
   if(HL_FindFirst(p_IFO->NboHl, (byte *)&router, (void *)&p_NBO) != E_OK)
   {
      /* Check if any other neighbor has the same IP Addres */
      er = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
      while(er == E_OK)
      {
         if((p_NBO->IpAdr == SrcAdr) && (p_NBO->RouterId != router))
             return NBR_IPADDR_MISMATCH;

         er = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
      }

      /* This is the first Hello from this neighbor, so create it */
      if(NBO_Init(p_IFO, router, SrcAdr,
            &p_IFO->Cfg, (t_Handle *) &p_NBO) != E_OK)
         return CONFIG_OK;

      /* Save previous values of priority, DrId and BackupId */
      PrevDrIpAddr       = A_GET_4B(p_Hlo->DesignatedRouter);
      PrevBackupIpAddr   = A_GET_4B(p_Hlo->BackupRouter);
      PrevRouterPriority = p_Hlo->RouterPriority;
   }
   else
   {
       /* Already have a neighbor with this router ID. */
       /* If rx interface is not p2p or virtual, then verify that
        * source IP address matches that of the neighbor. */
     if (p_NBO->IpAdr != SrcAdr)
     {
       if ((p_IFO->Cfg.Type != IFO_PTP) && (p_IFO->Cfg.Type != IFO_VRTL))
       {
         L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
         L7_uchar8 srcRouterIdStr[OSAPI_INET_NTOA_BUF_SIZE];
         L7_uchar8 srcAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
         L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
         RTO_InterfaceName(p_IFO, ifName);
         osapiInetNtoa((L7_uint32)router, srcRouterIdStr);
         osapiInetNtoa((L7_uint32)SrcAdr, srcAddrStr);
         osapiInetNtoa((L7_uint32)p_NBO->IpAdr, nbrAddrStr);
         L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID,
                 "Dropping hello on interface %s for router %s from IP address %s. "
                 "Neighbor address is %s."
                 " When OSPFv2 receives a hello, it compares the source IP address "
                 "of the hello to the IP address of previous hellos from that neighbor."
                 " If the source IP address is different, OSPF ignores the hello.",
                 ifName, srcRouterIdStr, srcAddrStr, nbrAddrStr);
         return NBR_IPADDR_MISMATCH;
       }
       else  /* p2p or virtual neighbor */
       {
#if L7_MIB_OSPF
         /* update MIB with new neighbor address. */
         if (p_IFO->Cfg.Type == IFO_PTP)
         {
           ospfNbrTable_DeleteInternal(p_RTO->ospfMibHandle,
                                       p_NBO->IpAdr,
                                       p_IFO->Cfg.IfIndex);

           ospfNbrTable_InternalNewAndActive(p_RTO->ospfMibHandle,
                                             SrcAdr, p_IFO->Cfg.IfIndex, p_NBO);
         }
#endif
           p_NBO->IpAdr = SrcAdr;
       }
     }

      /* Save previous values of priority, DrId and BackupId */
      PrevDrIpAddr       = p_NBO->DrIpAddr;
      PrevBackupIpAddr   = p_NBO->BackupIpAddr;
      PrevRouterPriority = p_NBO->RouterPriority;

      if (p_NBO->State == NBO_FULL)
      {
        /* Arbitrarily consider previous hello late if this hello arrived
         * less than half the hello interval later. */
        now = osapiUpTimeRaw();
        if ((now - p_NBO->lastHelloTime) < (p_IFO->Cfg.HelloInterval / 2))
          p_NBO->lateHellos++;
        p_NBO->lastHelloTime = now;
        p_NBO->hellosRx++;
      }
   }

   DoNboTransition(p_NBO, NBO_HLO_RX, p_Hlo);

   p_Ngb = (byte*)p_Hlo + sizeof(t_S_Hello); /* points to start of the neighbors list */
   /* Compute number of neighbors in the HELLO's list */
   ngbnum = (A_GET_2B(p_Hdr->Length)-sizeof(t_S_PckHeader)-sizeof(t_S_Hello))/4;

   /* examine the list of Neighbors */
   while(ngbnum)
   {
      if(A_GET_4B(p_Ngb) == p_RTO->Cfg.RouterId)
      {
         /* Router itself found in the neighbor list */
         DoNboTransition(p_NBO, NBO_2WAY_RX, p_Hlo);
         break;
      }

      p_Ngb += 4;
      ngbnum--;
   }

   if(!ngbnum)
   {
     /* The local Router not found in the neighbor list */
     DoNboTransition(p_NBO, NBO_1WAY_RX, p_Hlo);
     return CONFIG_OK;
   }

   if (o2GracefulRestartInProgress(p_RTO))
   {
     /* This neighbor is restarting */
#ifdef L7_NSF_PACKAGE
     /* If neighbor reports same DR and BDR as before restart, reelect them. */
     L7_uint32 preRestartDrIpAddr;
     L7_uint32 preRestartDrId;
     L7_uint32 preRestartBdrIpAddr;
     L7_uint32 preRestartBdrId;
     L7_uint32 intIfNum = p_IFO->Cfg.IfIndex;
     if (o2CkptInterfaceDrGet(intIfNum, &preRestartDrId, &preRestartDrIpAddr,
                              &preRestartBdrId, &preRestartBdrIpAddr) == L7_SUCCESS)
     {
       if (preRestartDrId || preRestartBdrId)
       {
         if ((A_GET_4B(p_Hlo->DesignatedRouter) == preRestartDrIpAddr) &&
             (A_GET_4B(p_Hlo->BackupRouter) == preRestartBdrIpAddr))
         {
           DoHelloTransition(p_IFO, IFO_BACKUP_SEEN);
           HelloSMSched = TRUE;
         }
       }
     }
#endif

     /* If in graceful restart and neighbor lists this router as DR,
      * reelect self as DR on this link */
     if ((A_GET_4B(p_Hlo->DesignatedRouter) == p_RTO->Cfg.RouterId) &&
         (p_IFO->Cfg.State == IFO_WAIT))
     {
       DoHelloTransition(p_IFO, IFO_BACKUP_SEEN);
       HelloSMSched = TRUE;
     }
   }

   /* Check Neighbor change event conditions */
   if(PrevRouterPriority != p_NBO->RouterPriority)
   {
      DoHelloTransition(p_IFO, IFO_NGB_CHANGE);
      HelloSMSched = TRUE;
   }

   if((A_GET_4B(p_Hlo->DesignatedRouter) == p_NBO->IpAdr) &&
      (A_GET_4B(p_Hlo->BackupRouter) == 0) &&
      (p_IFO->Cfg.State == IFO_WAIT))
   {
      DoHelloTransition(p_IFO, IFO_BACKUP_SEEN);
      HelloSMSched = TRUE;
   }
   else if(((A_GET_4B(p_Hlo->DesignatedRouter) == p_NBO->IpAdr) &&
           (PrevDrIpAddr != p_NBO->IpAdr)) ||
           ((A_GET_4B(p_Hlo->DesignatedRouter) != p_NBO->IpAdr) &&
           (PrevDrIpAddr == p_NBO->IpAdr)))
   {
      DoHelloTransition(p_IFO, IFO_NGB_CHANGE);
      HelloSMSched = TRUE;
   }
   if((A_GET_4B(p_Hlo->BackupRouter) == p_NBO->IpAdr) &&
      (p_IFO->Cfg.State == IFO_WAIT))
   {
      DoHelloTransition(p_IFO, IFO_BACKUP_SEEN);
      HelloSMSched = TRUE;
   }
   else if(((A_GET_4B(p_Hlo->BackupRouter) == p_NBO->IpAdr) &&
           (PrevBackupIpAddr != p_NBO->IpAdr)) ||
           ((A_GET_4B(p_Hlo->BackupRouter) != p_NBO->IpAdr) &&
           (PrevBackupIpAddr == p_NBO->IpAdr)))
   {
      DoHelloTransition(p_IFO, IFO_NGB_CHANGE);
      HelloSMSched = TRUE;
   }

   /* If the neighbor itself has not become DR/BRR and has not stopped */
   /* to be DR/BDR, but the neighbor's view of DR/BDR identity has     */
   /* been changed, we have to check adjacency of this neighbor        */
   if(!HelloSMSched &&
      ((PrevDrIpAddr != A_GET_4B(p_Hlo->DesignatedRouter)) ||
       (PrevBackupIpAddr != A_GET_4B(p_Hlo->BackupRouter))))
      DoNboTransition(p_NBO, NBO_ADJOK, 0);


   /* If IFO has NBMA type, then may be HELLO in replay should be sent */
   if((p_IFO->Cfg.Type == IFO_NBMA) &&
      (p_IFO->Cfg.RouterPriority == 0) &&
      (p_NBO->RouterPriority != 0) &&
      (p_IFO->DrId != p_NBO->RouterId) &&
      (p_IFO->BackupId != p_NBO->RouterId))
      IFO_SendHello(p_IFO, p_NBO, TRUE);

   return CONFIG_OK;
}


/*----------------------------------------------------------------
 *
 *      Hello FSM Transition routines
 *
 *----------------------------------------------------------------*/




/*********************************************************************
 * @purpose          Do Hello State machine transition according to
 *                   Event and Hello SM state.
 *
 *
 * @param p_IFO      @b{(input)}  IFO Object handler
 * @param Event      @b{(input)}  Hello SM event
 *
 * @returns          n/a.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void DoHelloTransition(t_IFO *p_IFO, e_IFO_Events Event)
{
#if L7_OSPF_TE
   t_ARO *p_ARO = (t_ARO *) p_IFO->ARO_Id;
#endif

   t_RTO *p_RTO = (t_RTO *) p_IFO->RTO_Id;

   word PreviousState = p_IFO->Cfg.State;

   if(((t_ARO*)p_IFO->ARO_Id) == NULLP)
   {
       /* ARO_Id should always be set before we come here.
        * If not set, LOG a msg and return */
       char ifName[L7_NIM_IFNAME_SIZE + 1];
       RTO_InterfaceName(p_IFO, ifName);
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
               "DoHelloTransition: ARO_Id not set yet on Intf %s, Event %s\n", ifName, ifoEventNames[Event]);

       return;
   }

   ((t_ARO*)p_IFO->ARO_Id)->IsRtrLsaOrg = FALSE;

    ((f_Hello_tr *)(p_IFO->Hello_TT))
      [Event * NUMB_OF_IFO_STATES + p_IFO->Cfg.State](p_IFO);

    if (p_RTO->ospfTraceFlags & OSPF_TRACE_NBR_STATE)
    {
      if(PreviousState != p_IFO->Cfg.State)
      {
        char traceBuf[OSPF_MAX_TRACE_STR_LEN];
        char ifName[L7_NIM_IFNAME_SIZE + 1];
        RTO_InterfaceName(p_IFO, ifName);
        sprintf(traceBuf, "OSPF interface %s moved from state %s to state %s, on event %s",
                ifName, ifoStateNames[PreviousState],
                ifoStateNames[p_IFO->Cfg.State],
                ifoEventNames[Event]);
        RTO_TraceWrite(traceBuf);
      }
    }

    if(p_IFO->Cfg.State != PreviousState)
    {
       t_IfStateChange IfChg;
       t_VirtIfStateChange VirtIfChg;

       p_IFO->Cfg.EventsCounter += 1;

       if(!((t_ARO*)p_IFO->ARO_Id)->IsRtrLsaOrg)
         /* Reoriginate the router LSA */
         LsaReOriginate[S_ROUTER_LSA](p_IFO->ARO_Id, 0);

       /* generate OSPF traps events callback */
       if(p_RTO->Clbk.p_OspfTrap)
       {
          if((p_IFO->Cfg.Type == IFO_VRTL) &&
             (p_RTO->TrapControl & GET_TRAP_BITMASK(VIRT_IF_STATE_CHANGE)))
          {
            VirtIfChg.RouterId = p_RTO->Cfg.RouterId;
            VirtIfChg.VirtIfAreaId = p_IFO->Cfg.VirtTransitAreaId;
            VirtIfChg.VirtIfNeighbor = p_IFO->Cfg.VirtIfNeighbor;
            VirtIfChg.VirtIfState = p_IFO->Cfg.State;
              VirtIfChg.Event = Event;

            p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, VIRT_IF_STATE_CHANGE, (u_OspfTrap*)&VirtIfChg);
          }
          else if(p_RTO->TrapControl & GET_TRAP_BITMASK(IF_STATE_CHANGE))
          {
             IfChg.RouterId = p_RTO->Cfg.RouterId;
             IfChg.IfIpAddress = p_IFO->Cfg.IpAdr;
             IfChg.AddressLessIf = p_IFO->Cfg.IfIndex;
             IfChg.IfState = p_IFO->Cfg.State;
               IfChg.Event = Event;

             p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, IF_STATE_CHANGE, (u_OspfTrap*)&IfChg);
          }
       }
#if L7_OSPF_TE
       if(p_ARO->TEsupport)
       {
          /* If the new state is IFO_DOWN flush the appropriate   */
          /* TE link LSA out, otherwhise sent updated TE link LSA */
          if(p_IFO->Cfg.State == IFO_DOWN)
          {
            t_TeLink key;
            t_TeLink *p_Link;

            A_SET_4B(p_RTO->Cfg.RouterId, key.RouterId);
            A_SET_4B(p_IFO->TeLinkId, key.LinkId);

            if(AVLH_Find(p_ARO->TeLsaHl,key.RouterId,(void**)&p_Link,0)==E_OK)
               FlushLsa(p_Link->p_DbEntry, (ulng)p_ARO, 0);
          }
          else
            TE_LinkInfoUpdate(p_IFO, p_IFO->TeCosMask, p_IFO->TePdr, 1);
       }
#endif
    }
}


static void IfEr(t_IFO *p_IFO)
{
}

static void Ifo0(t_IFO *p_IFO)
{
   e_Err e;
   t_NBO *p_NBO;
   t_S_NbmaRtrCfg *p_NbmaRtr;
   t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
   Bool newAbrStatus;
   Bool oldAbrStatus = routerIsBorder(p_RTO);

   if((p_IFO->Cfg.Type != IFO_LOOPBACK) && (p_IFO->Cfg.passiveMode != TRUE))
   {
   /* Start Hello timer */
     /* Don't start the Hello timer, if the interface is a Loopback or
      * a passive interface */
   TIMER_StartSecMQueue(p_IFO->HelloTimer, p_IFO->Cfg.HelloInterval,
                  TRUE, HelloTimerExp, ((t_RTO*)p_IFO->RTO_Id)->OspfSysLabel.threadHndle,
                  OSPF_HELLO_QUEUE );
   }

   /* Reset DR and Backup for initial time */
   p_IFO->Cfg.DrIpAddr = 0;
   p_IFO->Cfg.BackupIpAddr = 0;
   p_IFO->DrId = 0;
   p_IFO->BackupId = 0;

   switch(p_IFO->Cfg.Type)
   {
      case IFO_PTM:
         p_IFO->Cfg.State = IFO_PTPST;
         e = HL_GetFirst(p_IFO->NbmaHl, (void *)&p_NbmaRtr);
         while(e == E_OK)
         {
            if(p_NbmaRtr->IsValid)
            {
               /* Create configurated PTM neigbors */
               if(NBO_Init(p_IFO, p_NbmaRtr->RouterId,
                  p_NbmaRtr->IpAdr,
                  &p_IFO->Cfg, (t_Handle *) &p_NBO) != E_OK)
                  break;

               p_NBO->InactivityInterval = p_IFO->Cfg.RouterDeadInterval;

            }
            e = HL_GetNext(p_IFO->NbmaHl, (void *)&p_NbmaRtr, p_NbmaRtr);
         }
      break;
      case IFO_VRTL:
      case IFO_PTP:
         p_IFO->Cfg.State = IFO_PTPST;
      break;
      case IFO_LOOPBACK:
         p_IFO->Cfg.State = IFO_LOOP;
      break;
      case IFO_NBMA:
      case IFO_BRDC:
         if(!p_IFO->Cfg.RouterPriority)
            p_IFO->Cfg.State = IFO_DROTHER;
         else
         {
            p_IFO->Cfg.State = IFO_WAIT;
            /* Start Waiting timer */
            TIMER_StartSec(p_IFO->WaitTimer,
                           TimerFractVar(p_IFO->Cfg.RouterDeadInterval),
                           0, WaitTimerExp, ((t_RTO*)p_IFO->RTO_Id)->OspfSysLabel.threadHndle );
         }

         /* process NBMA interface type case */
         if(p_IFO->Cfg.Type == IFO_NBMA)
         {
            e = HL_GetFirst(p_IFO->NbmaHl, (void *)&p_NbmaRtr);
            while(e == E_OK)
            {
               if(p_NbmaRtr->IsValid)
               {
                  /* Create configurated NBMA neigbors */
                  if(NBO_Init(p_IFO, p_NbmaRtr->RouterId,
                     p_NbmaRtr->IpAdr,
                     &p_IFO->Cfg, (t_Handle *) &p_NBO) != E_OK)
                     break;

                  /* Set initial Inactivity Interval till first Hello
                     will be received */
                  p_NBO->InactivityInterval = p_IFO->Cfg.RouterDeadInterval;
                  p_NBO->RouterPriority = p_NbmaRtr->CanBeDr;

                  if(p_NbmaRtr->CanBeDr)
                  {
                     /* Run START event */
                     DoNboTransition(p_NBO, NBO_START, 0);
                  }
               }
               e = HL_GetNext(p_IFO->NbmaHl, (void *)&p_NbmaRtr, p_NbmaRtr);
            }
         }
      break;
      default:
      break;
   }

   /* reinitialization of link may have cleared former DRs */
   if (p_RTO->Clbk.f_DrElected)
   {
     (p_RTO->Clbk.f_DrElected)(p_IFO);
   }

   newAbrStatus = routerIsBorder(p_RTO);
   if (!oldAbrStatus && newAbrStatus)
   {
       /* router has become an ABR */
       IFO_BecomeAbr(p_IFO);
   }

   if((p_IFO->Cfg.Type != IFO_LOOPBACK) && (p_IFO->Cfg.passiveMode != TRUE))
   {
   /* Send first Hello immediately */
      /* Don't send any packets on the loopback interface, or a passive interface */
   IFO_SendHello(p_IFO, 0, 0);
}
}

static void Ifo1(t_IFO *p_IFO)
{
   DrCalculate(p_IFO);
}

static void Ifo2(t_IFO *p_IFO)
{
   p_IFO->Cfg.State = IFO_LOOP;

   if((p_IFO->Cfg.Type != IFO_LOOPBACK) && (p_IFO->Cfg.passiveMode != TRUE))
   {
     /* Since Hello Timer is not started on Loopback interface */
     TIMER_Stop(p_IFO->HelloTimer);
   }

   /* Reset DR and Backup */
   p_IFO->Cfg.DrIpAddr = 0;
   p_IFO->Cfg.BackupIpAddr = 0;
   p_IFO->DrId = 0;
   p_IFO->BackupId = 0;

   InformNbrsAboutIfoDown(p_IFO);
}

static void Ifo3(t_IFO *p_IFO)
{
}

static void Ifo4(t_IFO *p_IFO)
{
    t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
    t_ARO *p_ARO = (t_ARO*) p_IFO->ARO_Id;
    Bool oldAbrStatus = routerIsBorder(p_RTO);
    Bool newAbrStatus;

   p_IFO->Cfg.State = IFO_DOWN;

   if((p_IFO->Cfg.Type != IFO_LOOPBACK) && (p_IFO->Cfg.passiveMode != TRUE))
   {
     /* Since Hello Timer is not started on Loopback interface or passive interface */
     TIMER_Stop(p_IFO->HelloTimer);
   }

   if (TIMER_Active(p_IFO->grSuppressHelloTimer))
   {
      TIMER_Stop(p_IFO->grSuppressHelloTimer);
   }

   /* flush and delete link opaque LSAs */
   AVLH_Browse(p_IFO->LinkOpaqueLsaHl, (BROWSEFUNC)FlushLsa, (ulng)p_ARO);
   ClearLsaList(p_IFO->LinkOpaqueLsaHl);

   /* Reset DR and Backup */
   p_IFO->Cfg.DrIpAddr = 0;
   p_IFO->Cfg.BackupIpAddr = 0;
   p_IFO->DrId = 0;
   p_IFO->BackupId = 0;
   if (p_RTO->Clbk.f_DrElected)
   {
     (p_RTO->Clbk.f_DrElected)(p_IFO);
   }

   newAbrStatus = routerIsBorder(p_RTO);
   if (oldAbrStatus && !newAbrStatus)
       IFO_NoLongerAbr(p_IFO);

   InformNbrsAboutIfoDown(p_IFO);

   if(p_IFO->Cfg.Type != IFO_LOOPBACK)
   {
     /* Send one-way hello to tell neighbors we're gone. */
     IFO_SendHello(p_IFO, NULL, FALSE);
   }
}

static void Ifo5(t_IFO *p_IFO)
{
   TIMER_Stop(p_IFO->HelloTimer);

   InformNbrsAboutIfoDown(p_IFO);

   /* Send one-way hello to tell neighbors we're gone. */
   IFO_SendHello(p_IFO, NULL, FALSE);

   /* Recalculate the states */
   Ifo0(p_IFO);
}

/*********************************************************************
 * @purpose          Determine the best candidate to be designated router.
 *                   (as it is described in RFC 2328 paragraph 9.4 step 3)
 *
 * @param p_IFO      @b{(input)}  IFO object Id
 *
 * @returns          the router ID of the candidate DR. If no routers have
 *                   declared themselves DR, return 0.
 *
 * @notes     Implementation function for DrCalculate()
 *
 * @end
 * ********************************************************************/
static L7_uint32 DrCalculateDr(t_IFO *p_IFO)
{
  t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
  t_NBO *p_NBO = NULL;
  L7_uint32 drId = 0;      /* router ID of best candidate router */
  L7_uint32 drPri = 0;     /* priority of best candidate */
  e_Err er;

  /* This router is considered to be on the list of candidates. 9.4 para 2. */
  if (p_IFO->Cfg.RouterPriority && (p_IFO->DrId == p_RTO->Cfg.RouterId))
  {
    /* This router is eligible and is declaring itself to be DR. */
    drId = p_RTO->Cfg.RouterId;
    drPri = p_IFO->Cfg.RouterPriority;
  }

  /* Check list of neighbors */
  er = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
  while (er == E_OK)
  {
    /* If in helpful neighbor mode with this neighbor and we consider
     * this neighbor to be DR, retain that router as DR. */
    if ((o2NeighborIsRestarting(p_NBO)) && (p_IFO->DrId == p_NBO->RouterId))
    {
      if ((p_RTO->ospfTraceFlags & OSPF_TRACE_DR_CALC) ||
          (p_RTO->ospfTraceFlags & OSPF_TRACE_GR))
      {
        char traceBuf[OSPF_MAX_TRACE_STR_LEN];
        char drIdStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(p_IFO->DrId, drIdStr);
        osapiSnprintf(traceBuf, OSPF_MAX_TRACE_STR_LEN,
                      "Current DR %s is in graceful restart. Retaining DR.",
                      drIdStr);
        RTO_TraceWrite(traceBuf);
      }
      return p_IFO->DrId;
    }

   /* If in graceful restart and neighbor lists this router as DR,
    * reelect self as DR on this link */
   if (o2GracefulRestartInProgress(p_RTO) &&
       (p_IFO->Cfg.State == IFO_WAIT) &&
       (p_NBO->DrId == p_RTO->Cfg.RouterId))
   {
     if ((p_RTO->ospfTraceFlags & OSPF_TRACE_DR_CALC) ||
         (p_RTO->ospfTraceFlags & OSPF_TRACE_GR))
     {
        char traceBuf[OSPF_MAX_TRACE_STR_LEN];
        char nbrIdStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(p_NBO->RouterId, nbrIdStr);
        osapiSnprintf(traceBuf, OSPF_MAX_TRACE_STR_LEN,
                      "In graceful restart. Neighbor %s lists this router as DR. "
                      "Electing self as DR.",
                      nbrIdStr);
        RTO_TraceWrite(traceBuf);
     }
     return p_RTO->Cfg.RouterId;
   }

    /* Only consider neighbors with non-zero priority, in 2-way state
     * or better, and declaring themselves to be DR. */
    if (p_NBO->RouterPriority &&
        (p_NBO->State >= NBO_2WAY) &&
        (p_NBO->DrId == p_NBO->RouterId))
    {
      /* This router is eligible */
      if ((p_NBO->RouterPriority > drPri) ||
          ((p_NBO->RouterPriority == drPri) && (p_NBO->RouterId > drId)))
      {
        drId = p_NBO->RouterId;
        drPri = p_NBO->RouterPriority;
      }
    }
    er = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
  }

  return drId;
}

/*********************************************************************
 * @purpose          Determine the best candidate to be backup designated router.
 *                   (as it is described in RFC 2328 paragraph 9.4 step 2)
 *
 * @param p_IFO      @b{(input)}  IFO object Id
 *
 * @returns          the router ID of the candidate backup
 *
 * @notes      Implementation function for DrCalculate()
 *
 * @end
 * ********************************************************************/
static L7_uint32 DrCalculateBackup(t_IFO *p_IFO)
{
  t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
  t_NBO *p_NBO = NULL;
  L7_uint32 backupId = 0;      /* router ID of best backup router */
  L7_uint32 backupPri = 0;     /* priority of best backup */
  L7_uint32 hiPriRtr = 0;      /* router ID of eligible router with highest priority */
  L7_uint32 hiPri = 0;         /* priority of hiPriRtr */
  e_Err er;

  /* This router is considered to be on the list. 9.4 para 2. */
  if (p_IFO->Cfg.RouterPriority && (p_IFO->DrId != p_RTO->Cfg.RouterId))
  {
    /* This router is eligible to be backup (priority it not 0)
     * and is not declaring itself to be DR. */
    if (p_IFO->BackupId == p_RTO->Cfg.RouterId)
    {
      /* this router is declaring itself to be backup */
      backupId = p_RTO->Cfg.RouterId;
      backupPri = p_IFO->Cfg.RouterPriority;
    }
    else
    {
      hiPriRtr = p_RTO->Cfg.RouterId;
      hiPri = p_IFO->Cfg.RouterPriority;
    }
  }

  /* Check list of neighbors */
  er = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
  while (er == E_OK)
  {
    /* Check only neighbor eligible to be BDR, i.e., those with non-zero
     * priority, in 2-way or better, and not declaring themselves to be DR */
    if (p_NBO->RouterPriority &&
        (p_NBO->State >= NBO_2WAY) && (p_NBO->DrId != p_NBO->RouterId))
    {
      /* router is eligible */
      if (p_NBO->BackupId == p_NBO->RouterId)
      {
        /* this neighbor declaring itself backup */
        if ((p_NBO->RouterPriority > backupPri) ||
            ((p_NBO->RouterPriority == backupPri) && (p_NBO->RouterId > backupId)))
        {
          backupId = p_NBO->RouterId;
          backupPri = p_NBO->RouterPriority;
        }
      }
      else if (!backupId) /* if no routers have declared as a backup */
      {
        if ((p_NBO->RouterPriority > hiPri) ||
            ((p_NBO->RouterPriority == hiPri) && (p_NBO->RouterId > hiPriRtr)))
        {
          hiPriRtr = p_NBO->RouterId;
          hiPri = p_NBO->RouterPriority;
        }
      }
    }
    er = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
  }

  /* if no routers have declared as a Backup,
  choose the router having highest priority */
  if (!backupId)
  {
    backupId = hiPriRtr;
  }
  return backupId;
}

/*********************************************************************
 * @purpose  If this router becomes DR or BDR on NBMA, start sending
 *           hello packets to neighbors not eligible to become DR.
 *
 * @param     p_IFO  @b{(input)}  interface
 *
 * @returns   E_OK
 *
 * @notes     RFC 2328 section 9.4 step 6
 *
 * @end
 * ********************************************************************/
static e_Err DrNbmaStart(t_IFO *p_IFO)
{
  e_Err er;
  t_NBO *p_NBO = NULL;

  er = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
  while (er == E_OK)
  {
    /* Check only not eligible to be DR neighbors */
    if (!p_NBO->RouterPriority)
    {
      DoNboTransition(p_NBO, NBO_START, 0);
    }
    er = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
  }
  return E_OK;
}

/*********************************************************************
 * @purpose  Get the interface address for a DR or backup.
 *
 * @param     p_IFO     @b{(input)}  interface
 * @param     routerId  @b{(input)}  router ID of DR or BDR
 *
 * @returns   Interface IP address
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static L7_uint32 DrInterfaceAddress(t_IFO *p_IFO, L7_uint32 routerId)
{
  t_NBO *p_NBO = NULL;
  t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;

  if (routerId == p_RTO->Cfg.RouterId)
  {
    return p_IFO->Cfg.IpAdr;
  }
  if (routerId &&
      (HL_FindFirst(p_IFO->NboHl, (byte *)&routerId, (void *)&p_NBO) == E_OK))
  {
    return p_NBO->IpAdr;
  }
  return 0;
}

/*********************************************************************
 * @purpose  Take actions required when DR or BDR changes.
 *
 * @param     p_IFO     @b{(input)}  interface
 * @param     NewDr     @b{(input)}  router ID of new DR
 * @param     NewBackup @b{(input)}  router ID of new backup
 * @param     OldDr     @b{(input)}  router ID of former DR
 * @param     OldBackup @b{(input)}  router ID of former backup
 *
 * @returns   E_OK
 *
 * @notes     RFC 2328 section 9.4, steps 5 - 7
 *
 * @end
 * ********************************************************************/
static e_Err DrUpdate(t_IFO *p_IFO, L7_uint32 NewDr, L7_uint32 NewBackup,
                      L7_uint32 OldDr, L7_uint32 OldBackup)
{
  t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
  t_NBO *p_NBO = NULL;
  t_ARO *p_ARO = (t_ARO*) p_IFO->ARO_Id;
  L7_uint32 LocRouterId = p_RTO->Cfg.RouterId;
  e_Err er;
  t_A_DbEntry *p_DbEntry;
  t_A_DbKey DbKey;

  /* set interface address of elected DR and BACKUP */
  p_IFO->Cfg.DrIpAddr = DrInterfaceAddress(p_IFO, NewDr);
  p_IFO->Cfg.BackupIpAddr = DrInterfaceAddress(p_IFO, NewBackup);

  /* step 5 - set interface state */
  if (NewDr == LocRouterId)
    p_IFO->Cfg.State = IFO_DR;
  else if (NewBackup == LocRouterId)
    p_IFO->Cfg.State = IFO_BACKUP;
  else
    p_IFO->Cfg.State = IFO_DROTHER;

  if (p_RTO->ospfTraceFlags & OSPF_TRACE_DR_CALC)
  {
    char traceBuf[OSPF_MAX_TRACE_STR_LEN];
    osapiSnprintf(traceBuf, OSPF_MAX_TRACE_STR_LEN,
                  "New interface state is %s.",
                  ifoStateNames[p_IFO->Cfg.State]);
    RTO_TraceWrite(traceBuf);
  }

  /* step 6 - if IFO is NBMA and this router became B(DR), invoke start event*/
  if ((p_IFO->Cfg.Type == IFO_NBMA) &&
      ((NewDr == LocRouterId) || (NewBackup == LocRouterId)))
  {
    DrNbmaStart(p_IFO);
  }

  /* step 7 - if Dr or Backup changed, may be AdjOK event should be invoked */
  if ((NewDr != OldDr) || (NewBackup != OldBackup))
  {
    LsaReOriginate[S_ROUTER_LSA](p_ARO, 0);

    /* Check list of neighbors */
    er = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
    while (er == E_OK)
    {
      /* Check only neighbors in at least 2-Way state */
      if (p_NBO->State >= NBO_2WAY)
      {
        e_NBO_States oldNbrState = p_NBO->State;
        DoNboTransition(p_NBO, NBO_ADJOK, 0);
        if (p_RTO->ospfTraceFlags & OSPF_TRACE_DR_CALC)
        {
          char traceBuf[OSPF_MAX_TRACE_STR_LEN];
          char ifName[L7_NIM_IFNAME_SIZE + 1];
          char nbrName[OSAPI_INET_NTOA_BUF_SIZE];
          RTO_InterfaceName(p_IFO, ifName);
          if (p_NBO->State != oldNbrState)
          {
            osapiInetNtoa(p_NBO->IpAdr, nbrName);
            osapiSnprintf(traceBuf, OSPF_MAX_TRACE_STR_LEN,
                          "Neighbor %s moved from %s to %s.",
                          nbrName, nbrStateNames[oldNbrState],
                          nbrStateNames[p_NBO->State]);
            RTO_TraceWrite(traceBuf);
          }
        }
      }
      er = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
    }

    /* If local router becomes DR */
    if ((NewDr == LocRouterId) && (OldDr != LocRouterId))
    {
      /* Reoriginate the network LSA */
      LsaReOriginate[S_NETWORK_LSA](p_ARO, (ulng)p_IFO);
    }
    else if (NewDr != LocRouterId)
    {
      /* If local router no longer DR, flush network LSA */
      A_SET_4B(p_IFO->Cfg.IpAdr, DbKey.LsId);
      A_SET_4B(LocRouterId, DbKey.AdvertisingRouter);

      if (AVLH_Find(p_ARO->NetworkLsaHl, (byte *)&DbKey,
                    (void *)&p_DbEntry, 0) == E_OK)
        FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);
    }
    if (p_RTO->Clbk.f_DrElected)
    {
      (p_RTO->Clbk.f_DrElected)(p_IFO);
    }
  }
  return E_OK;
}

/*********************************************************************
 * @purpose          Calculate the Designated and Backup Router
 *                   (as it is described in RFC 2328 paragraph 9.4)
 *
 *
 * @param p_IFO      @b{(input)}  IFO object Id
 *
 * @returns          n/a
 *
 * @notes      Should only be called through the interface state transition
 *             table. Not static for debugging purposes only.
 *
 * @end
 * ********************************************************************/
void DrCalculate(t_IFO *p_IFO)
{
   SP_IPADR OldDr;      /* designated router ID when DrCalculate() begins */
   SP_IPADR OldBackup;  /* Backup router ID when DrCalculate() begins */
   t_RTO *p_RTO = (t_RTO *) p_IFO->RTO_Id;
   SP_IPADR LocRouterId;

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_DR_CALC)
   {
     char traceBuf[OSPF_MAX_TRACE_STR_LEN];
     char ifName[L7_NIM_IFNAME_SIZE + 1];
     RTO_InterfaceName(p_IFO, ifName);
     osapiSnprintf(traceBuf, OSPF_MAX_TRACE_STR_LEN,
                   "Calculating DR on OSPFv2 interface %s in state %s.",
                   ifName, ifoStateNames[p_IFO->Cfg.State]);
     RTO_TraceWrite(traceBuf);
   }

   LocRouterId = p_RTO->Cfg.RouterId;

   /* step 1 - save current DR and BACKUP */
   OldDr = p_IFO->DrId;
   OldBackup = p_IFO->BackupId;

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_DR_CALC)
   {
     char traceBuf[OSPF_MAX_TRACE_STR_LEN];
     L7_uchar8 drid[OSAPI_INET_NTOA_BUF_SIZE];
     L7_uchar8 bdrid[OSAPI_INET_NTOA_BUF_SIZE];
     osapiInetNtoa(OldDr, drid);
     osapiInetNtoa(OldBackup, bdrid);
     osapiSnprintf(traceBuf, OSPF_MAX_TRACE_STR_LEN,
                   "Current DR is %s. Current BDR is %s.", drid, bdrid);
     RTO_TraceWrite(traceBuf);
   }

   p_IFO->BackupId = DrCalculateBackup(p_IFO);    /* step 2 */

   p_IFO->DrId = DrCalculateDr(p_IFO);            /* step 3 */
   if (p_IFO->DrId == 0)
   {
     p_IFO->DrId = p_IFO->BackupId;
   }

   if (((p_IFO->DrId == LocRouterId) && (OldDr != LocRouterId))        ||     /* step 4 */
       ((p_IFO->BackupId == LocRouterId) && (OldBackup != LocRouterId)) ||
       ((p_IFO->DrId != LocRouterId) && (OldDr == LocRouterId))         ||
       ((p_IFO->BackupId != LocRouterId) && (OldBackup == LocRouterId)))
   {
     p_IFO->BackupId = DrCalculateBackup(p_IFO);    /* repeat step 2 */

     p_IFO->DrId = DrCalculateDr(p_IFO);            /* repeat step 3 */
     if (p_IFO->DrId == 0)
     {
       p_IFO->DrId = p_IFO->BackupId;
     }
   }

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_DR_CALC)
   {
     char traceBuf[OSPF_MAX_TRACE_STR_LEN];
     L7_uchar8 drid[OSAPI_INET_NTOA_BUF_SIZE];
     L7_uchar8 bdrid[OSAPI_INET_NTOA_BUF_SIZE];
     osapiInetNtoa(p_IFO->DrId, drid);
     osapiInetNtoa(p_IFO->BackupId, bdrid);
     osapiSnprintf(traceBuf, OSPF_MAX_TRACE_STR_LEN,
                   "New DR ID is %s. New BDR ID is %s.", drid, bdrid);
     RTO_TraceWrite(traceBuf);
   }

   DrUpdate(p_IFO, p_IFO->DrId, p_IFO->BackupId, OldDr, OldBackup);
}

/*********************************************************************
 * @purpose          Informs neighbors of the specified interface
 *                   that the interface is down
 *
 *
 * @param p_IFO      @b{(input)}  IFO object Id
 *
 * @returns           n/a
 *
 * @notes            Terminate helpful neighbor relationship with any
 *                   neighbor on the interface that has gone down.
 *
 * @end
 * ********************************************************************/
static void InformNbrsAboutIfoDown(t_IFO *p_IFO)
{
   t_NBO *p_NBO     = NULLP;
   t_NBO *p_NextNBO = NULLP;
   e_Err  e         = E_FAILED;

   e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);

   while (e == E_OK)
   {
      e = HL_GetNext(p_IFO->NboHl, (void *)&p_NextNBO, p_NBO);
      if (o2NeighborIsRestarting(p_NBO))
      {
        o2HelperModeExit(p_NBO, OSPF_HELPER_LINK_DOWN);
      }
      DoNboTransition(p_NBO, NBO_KILL, 0);
      p_NBO = p_NextNBO;
   }
}


/*********************************************************************
 * @purpose               Performs outgouing packet authentication coding.
 *
 *
 * @param  p_IFO          @b{(input)}  IFO object Id
 * @param  p_Packet       @b{(input)}  packet frame
 *
 * @returns                n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void AuthPckCoding(t_IFO *p_IFO, void *p_Packet)
{
   t_S_PckHeader *p_Hdr;
   void *p_buf;
   word len;
   word chksum;
   byte digest[16];
   t_OspfAuthKey *p_Key = NULLP, *p_Skey = NULLP;
   e_Err  e;
   ulng  curtime;

   len = F_GetLength(p_Packet);
   p_buf = F_GetFirstBuf(p_Packet);
   /* Get pointer to the start of the packet */
   p_Hdr = (t_S_PckHeader *)B_GetData(p_buf);

   /* Set the AuType and AuKey to the Packet header */
   A_SET_2B(p_IFO->Cfg.AuType,p_Hdr->AuType);

   switch(p_IFO->Cfg.AuType)
   {
      case IFO_NULL_AUTH:  /* Null Authentication */
      case IFO_PASSWORD:   /* Simple Password */


         if(len & 1)
         {
            /* Pad the packet to align 16-bit word */
            *(((byte *)p_Hdr) + len) = 0;
            len += 1;
            B_SetLength(p_buf, len);
            F_GetLength(p_Packet) += 1;
         }

         memset(&p_Hdr->AuKey[0], 0, 8);
         A_SET_2B(0,p_Hdr->CheckSum);
         chksum = SPF_ComputeIpCheckSum(p_Hdr, len);
         A_SET_2B(chksum,p_Hdr->CheckSum);
         memcpy(&p_Hdr->AuKey[0], &p_IFO->Cfg.AuKey[0], 8);
      break;
      case IFO_CRYPT:      /* Cryptographic Authentication */

         /* Find the key */
         curtime = simSystemUpTimeGet();
         e = HL_GetFirst(p_IFO->AuthKeyHl, (void *)&p_Skey);
           p_Key = p_Skey;
         while(e == E_OK)
         {
            if((p_Skey->KeyStartGenerate <= curtime) &&
               ((p_Skey->KeyStopGenerate == 0) || (p_Skey->KeyStopGenerate > curtime)) &&
               (p_Skey->KeyStartGenerate > p_Key->KeyStartGenerate))
               p_Key = p_Skey;

            e = HL_GetNext(p_IFO->AuthKeyHl, (void *)&p_Skey, p_Skey);
         }

         if(!p_Key)
            return;

         A_SET_2B(0, p_Hdr->CheckSum);

         /* Formate the AuthKey field */
         A_SET_2B(0, &p_Hdr->AuKey[0]);
         p_Hdr->AuKey[2] = p_Key->KeyId;
         p_Hdr->AuKey[3] = OSPF_MD5_AUTH_DATA_LEN;
         A_SET_4B(curtime, &p_Hdr->AuKey[4]);

         /* Calculate the message digest */
         memcpy(((byte *)p_Hdr) + len, p_Key->KeyValue, OSPF_MD5_AUTH_DATA_LEN);
         MdGetDigest((byte*)p_Hdr, len + OSPF_MD5_AUTH_DATA_LEN, digest);
         memcpy(((byte *)p_Hdr) + len, digest, OSPF_MD5_AUTH_DATA_LEN);

         /* Update the Packet length */
         B_SetLength(p_buf, len + OSPF_MD5_AUTH_DATA_LEN);
         F_GetLength(p_Packet) += OSPF_MD5_AUTH_DATA_LEN;

      break;
      default:
      return;
   }
}




/*********************************************************************
 * @purpose               Performs incomming packet authentication
 *                        decoding.
 *
 *
 * @param p_IFO           @b{(input)}  IFO object Id
 * @param p_NBO           @b{(input)}  Neighbor object Id
 * @param p_Packet        @b{(input)}  packet frame
 *
 * @returns               E_OK - success, otherwise E_FAILED
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err AuthPckDecoding(t_IFO *p_IFO, t_NBO *p_NBO, void *p_Packet)
{
   t_S_PckHeader *p_Hdr;
   void *p_buf;
   word len, croppedLen;
   word chksum;
   ulng curtime;
   byte digest[16];
   byte rx_digest[16];
   t_OspfAuthKey *p_Key;
   byte key[8];

   len = F_GetLength(p_Packet);
   p_buf = F_GetFirstBuf(p_Packet);
   /* Get pointer to the start of the packet */
   p_Hdr = (t_S_PckHeader *)B_GetData(p_buf);

   switch(p_IFO->Cfg.AuType)
   {
      case IFO_NULL_AUTH:  /* Null Authentication */
      case IFO_PASSWORD:   /* Simple Password */

         if(len & 1)
         {
            /* Pad the packet to align 16-bit word */
            *(((byte *)p_Hdr) + len) = 0;
            len += 1;
            B_SetLength(p_buf, len);
            F_GetLength(p_Packet) += 1;
         }

         /* save the key */
         memcpy(key, &p_Hdr->AuKey[0], 8);
         memset(&p_Hdr->AuKey[0], 0, 8);
         chksum = A_GET_2B(p_Hdr->CheckSum);
         A_SET_2B(0, p_Hdr->CheckSum);
         if(SPF_ComputeIpCheckSum(p_Hdr, len) != chksum)
            return E_FAILED;
         else if(p_IFO->Cfg.AuType == IFO_PASSWORD)
         {
            if(memcmp(key, &p_IFO->Cfg.AuKey[0], 8))
               return E_FAILED;
         }
         return E_OK;
      break;
      case IFO_CRYPT:      /* Cryptographic Authentication */

         curtime = simSystemUpTimeGet();

         /* Find the key */
         if((HL_FindFirst(p_IFO->AuthKeyHl, &p_Hdr->AuKey[2], (void *)&p_Key) != E_OK) ||
             (p_Key->KeyStartAccept > curtime) ||
             ((p_Key->KeyStopAccept != 0) && (p_Key->KeyStopAccept <= curtime)))
            return E_FAILED;

         /* Check the seqnum */
         if(p_NBO && (A_GET_4B(&p_Hdr->AuKey[4]) < p_NBO->LastRxCryptSeqNum))
           return E_FAILED;

         /* crop any garbage at the end of the packet */
         croppedLen = A_GET_2B(p_Hdr->Length) + 16;
         if(len > croppedLen)
         {
           /* Update the Packet length */
           B_SetLength(p_buf, croppedLen);
           F_GetLength(p_Packet) -= (len-croppedLen);
           len = croppedLen;
         }

         /* Calculate the received message digest */
         memcpy(rx_digest,((byte *)p_Hdr) + len - 16, 16);
         memcpy(((byte *)p_Hdr) + len - 16, p_Key->KeyValue, 16);
         MdGetDigest((byte*)p_Hdr, len, digest);
         if(memcmp(rx_digest, digest, 16))
            return E_FAILED;
         else
         {
           if(p_NBO)
             p_NBO->LastRxCryptSeqNum = A_GET_4B(&p_Hdr->AuKey[4]);

           /* Update the Packet length */
           B_SetLength(p_buf, len-16);
           F_GetLength(p_Packet) -= 16;
           return E_OK;
         }

      break;
      default:
      break;
   }

   return E_FAILED;
}


/*********************************************************************
 * @purpose    When an interface comes up, do NSSA stuff. If the interface
 *             up event makes the router an ABR, check the NSSA translator
 *             state. If the interface is in an NSSA, and the router did not
 *             previously have an interface in that NSSA, originate
 *             T7 LSAs into the NSSA.
 *
 *
 * @param p_IFO  @b{(input)} Interface that just got UP event.
 *
 * @returns   void
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void NssaHandleIFOUpEvent(t_IFO *p_IFO)
{
  t_RTO *p_RTO = (t_RTO *)p_IFO->RTO_Id;
  t_ARO *p_ARO = (t_ARO *)p_IFO->ARO_Id;
  t_IFO *p_tmpIFO = NULLP;
  e_Err e = E_FAILED;
  SP_IPADR ForwardingAdr = DefaultDestination;
  t_S_AsExternalCfg *p_Entry = NULLP;
  t_S_AsExternalCfg asExtCfg;

  if (!p_ARO)
      return;

  if (routerIsBorder(p_RTO))
  {
      /* might have just become a border router when this interface came up.
       * Have to check translator state in each attached NSSA. */
      t_ARO *attachedArea;     /* an area attached to this router */
      e_Err e = HL_GetFirst(p_RTO->AroHl, (void**)&attachedArea);
      while (e == E_OK)
      {
          /* If area is an NSSA, see if translator state should change. */
          if (attachedArea->ExternalRoutingCapability == AREA_IMPORT_NSSA)
          {
              NssaTranslatorElect(attachedArea, 0);
          }
          e = HL_GetNext(p_RTO->AroHl, (void**)&attachedArea, attachedArea);
      }
  }

  /* Proceed only if the interface belongs to a NSSA */
  if((p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA) ||
     (p_ARO->NSSARedistribute != TRUE))
    return;

  /* Check to see if there is another active interface on this NSSA */
  e = HL_GetFirst(p_ARO->IfoHl, (void *)&p_tmpIFO);
  while(e == E_OK)
  {
    if(p_tmpIFO && (p_tmpIFO->Cfg.State != IFO_DOWN) &&
       (p_tmpIFO->Cfg.IpAdr != p_IFO->Cfg.IpAdr))
      return;

    e = HL_GetNext(p_ARO->IfoHl, (void *)&p_tmpIFO, p_tmpIFO);
  }

  /* If we reached here this is the first active interface for this NSSA */
  ForwardingAdr = p_IFO->Cfg.IpAdr;

  /* Walk the list of external routes and originate corresponding NSSA LSAs
  ** for them
  */
  e = HL_GetFirst(p_RTO->AsExtRoutesHl, (void *)&p_Entry);
  while(e == E_OK)
  {
    /* Default route handled by NssaDefaultLsaOriginate() */
    if ((p_Entry->DestNetIpAdr != 0) && (p_Entry->DestNetIpMask != 0))
    {
      bzero((char *)&asExtCfg, sizeof(t_S_AsExternalCfg));
      memcpy((char *)&asExtCfg, (char *)p_Entry, sizeof(t_S_AsExternalCfg));
      asExtCfg.AsExtStatus = ROW_CREATE_AND_GO;
      asExtCfg.PrevEntry = (t_Handle)RTE_ADDED;
      asExtCfg.ForwardingAdr = ForwardingAdr;

      LsaReOriginate[S_NSSA_LSA](p_ARO, (ulng)&asExtCfg);
    }

    e = HL_GetNext(p_RTO->AsExtRoutesHl, (void *)&p_Entry, p_Entry);
  }
}

/*********************************************************************
 * @purpose    When an interface goes down, do NSSA stuff. If the interface
 *             down event makes the router no longer an ABR, check the NSSA
 *             translator state for each attached NSSA. If the interface is
 *             in an NSSA, and the router previously used the interface
 *             as the forwarding address for T7 LSAs sent into that NSSA,
 *             look for an alternate forwarding address. If no other forwarding
 *             address is available, flush the T7s.
 *
 *
 * @param p_IFO  @b{(input)} Interface that just got DOWN event.
 *
 * @returns   void
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void NssaHandleIFODownEvent(t_IFO *p_IFO)
{
  t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
  t_ARO *p_ARO = (t_ARO *)p_IFO->ARO_Id;
  t_IFO *p_tmpIFO = NULLP;
  SP_IPADR ForwardingAdr = DefaultDestination;
  e_Err  e = E_FAILED;
  t_A_DbEntry *p_DbEntry = NULLP;
  t_S_AsExternalLsa *p_NssaLsa = NULLP;
  t_S_AsExternalCfg asExtCfg;

  if (!p_ARO)
      return;

  if (!routerIsBorder(p_RTO))
  {
      /* might have just lost border router status when this interface
       * went down. Have to check translator state in each attached NSSA. */
      t_ARO *attachedArea;     /* an area attached to this router */
      e_Err e = HL_GetFirst(p_RTO->AroHl, (void**)&attachedArea);
      while (e == E_OK)
      {
          /* If area is an NSSA, see if translator state should change. */
          if (attachedArea->ExternalRoutingCapability == AREA_IMPORT_NSSA)
          {
              NssaTranslatorElect(attachedArea, 0);
          }
          e = HL_GetNext(p_RTO->AroHl, (void**)&attachedArea, attachedArea);
      }
  }

  if ((p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA))
    return;

  /* Try to find a new valid forwarding address */
  p_tmpIFO = (t_IFO*) NSSA_FindForwardingAddress((t_Handle)p_ARO);
  ForwardingAdr = (p_tmpIFO != NULLP)? p_tmpIFO->Cfg.IpAdr : DefaultDestination;

  /* Update all NSSA LSAs with this interface as their forwarding address
  ** to have a new valid forwarding address
  */
  e = AVLH_GetFirst(p_ARO->NssaLsaHl, (void *)&p_DbEntry);
  while(e == E_OK)
  {
    if(ForwardingAdr == DefaultDestination)
      FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 0);
    else
    {
      p_NssaLsa = (t_S_AsExternalLsa *) p_DbEntry->p_Lsa;
      if(A_GET_4B(p_NssaLsa->ForwardingAddress) == p_IFO->Cfg.IpAdr)
      {
        bzero((char *)&asExtCfg, sizeof(t_S_AsExternalCfg));
        asExtCfg.AsExtStatus = ROW_CREATE_AND_GO;
        asExtCfg.DestNetIpAdr  = A_GET_4B(p_DbEntry->Lsa.LsId) & A_GET_4B(p_NssaLsa->NetworkMask);
        asExtCfg.DestNetIpMask = A_GET_4B(p_NssaLsa->NetworkMask);
        asExtCfg.ExtRouteTag = A_GET_4B(p_NssaLsa->ExternalRouteTag);
        asExtCfg.ForwardingAdr = ForwardingAdr;
        asExtCfg.IsMetricType2 = (p_NssaLsa->Flags & S_AS_EXT_E_FLAG) ? TRUE: FALSE;
        asExtCfg.IsVpnSupported = FALSE;
        asExtCfg.MetricValue = A_GET_3B(p_NssaLsa->Metric);
        asExtCfg.PhyType = OSPF_ETHERNET;
        asExtCfg.PrevEntry = (t_Handle)RTE_ADDED;

        LsaReOriginate[S_NSSA_LSA](p_ARO, (ulng)&asExtCfg);
      }
    }

    e = AVLH_GetNext(p_ARO->NssaLsaHl, (void *)&p_DbEntry, p_DbEntry);
  }
}

/*----------------------------------------------------------------
 *
 *      Timer Expiration Routines
 *
 *----------------------------------------------------------------*/


e_Err HelloTimerExp( t_Handle Id, t_Handle TimerId, word Flag )
{
   t_IFO *p_IFO = (t_IFO *) Id;

   /* Send HELLO */
   IFO_SendHello(p_IFO, 0, 0);

   return E_OK;
}


e_Err WaitTimerExp( t_Handle Id, t_Handle TimerId, word Flag )
{
   t_IFO *p_IFO = (t_IFO *) Id;

   DoHelloTransition(p_IFO, IFO_WAIT_TIMER);

   return E_OK;
}


e_Err UnicastAckTimerExp( t_Handle Id, t_Handle TimerId, word Flag )
{
   t_IFO *p_IFO = (t_IFO *) Id;
   t_NBO *prevNbo = NULL;
   t_NBO *p_NBO;
   t_AckElement *p_Ack;
   void *p_F = NULL;
   ulng allocPktLen = 0;
   word authLen = 0;

   if((p_IFO->Cfg.State <= IFO_WAIT) && (LL_HowMany(p_IFO->LsAck) == 0))
      return E_OK;

   if (p_IFO->Cfg.AuType == IFO_CRYPT)
   {
     authLen = OSPF_MD5_AUTH_DATA_LEN;
   }

   while((p_Ack = LL_Get(p_IFO->LsAck)) != NULL)
   {
      p_NBO = (t_NBO *) p_Ack->NBO_Id;
      if(p_NBO->State < NBO_EXCHANGE)
      {
         XX_Free(p_Ack);
         continue;
      }

      /* if it is a new group of LS ACK to other neighbor */
      if((p_NBO != prevNbo) ||
         ((IP_HDR_LEN + authLen + sizeof(t_S_PckHeader) +
           F_GetLength(p_F) + sizeof(t_S_LsaHeader)) > allocPktLen))
      {
         /* Send LsAck only for whole Acks group to the new neighbor */
         if(p_F)
         {
            /* Transmit the LS ACK packet */
            if(F_GetLength(p_F))
               IFO_SendPacket(p_IFO, p_F, S_LS_ACK, p_NBO, FALSE);
            else
               F_Delete(p_F);
         }

         /* Allocate the new LS ACK */
         if((p_F = IFO_AllocPacket(p_IFO, S_LS_ACK, 0, &allocPktLen)) == NULL)
         {
            XX_Free(p_Ack);
            return E_FAILED;
         }

         prevNbo = p_NBO;
      }

      /* copy this element info into LS ACK packet */
      F_AddToEnd(p_F, (byte*)&p_Ack->LsaHdr, sizeof(t_S_LsaHeader));
      XX_Free(p_Ack);
   }
   /* If some frame leaved for trasmission, tx now */
   if(p_F)
   {
      /* Transmit the LS ACK packet */
      if(F_GetLength(p_F))
         IFO_SendPacket(p_IFO, p_F, S_LS_ACK, prevNbo, FALSE);
      else
         F_Delete(p_F);
   }

   return E_OK;
}


e_Err BroadcastAckTimerExp( t_Handle Id, t_Handle TimerId, word Flag )
{
   t_IFO *p_IFO = (t_IFO *) Id;
   t_AckElement *p_Ack;
   void *p_F;
   ulng allocPktLen;
   word authLen = 0;

   if (p_IFO->Cfg.AuType == IFO_CRYPT)
   {
     authLen = OSPF_MD5_AUTH_DATA_LEN;
   }

   if((p_IFO->Cfg.State > IFO_WAIT) && (LL_HowMany(p_IFO->LsAck) > 0))
   {
      /* Allocate LS ACK */
      if((p_F = IFO_AllocPacket(p_IFO, S_LS_ACK, 0, &allocPktLen)) == NULL)
         return E_FAILED;

      while((p_Ack = LL_Get(p_IFO->LsAck)) != NULL)
      {
         if((IP_HDR_LEN + authLen + sizeof(t_S_PckHeader) +
             sizeof(t_S_LsaHeader) + F_GetLength(p_F)) > allocPktLen)
         {
            /* Transmit the LS ACK packet */
            IFO_SendPacket(p_IFO, p_F, S_LS_ACK, NULL, FALSE);

            /* Allocate the new LS ACK */
            if((p_F = IFO_AllocPacket(p_IFO, S_LS_ACK, 0, &allocPktLen)) == NULL)
            {
               XX_Free(p_Ack);
               return E_FAILED;
            }
         }

         /* copy this element info into LS ACK packet */
         F_AddToEnd(p_F, (byte*)&p_Ack->LsaHdr, sizeof(t_S_LsaHeader));

         XX_Free(p_Ack);
      }

      /* Transmit the LS ACK packet */
      if(F_GetLength(p_F))
         IFO_SendPacket(p_IFO, p_F, S_LS_ACK, NULL, FALSE);
      else
         F_Delete(p_F);
   }
   return E_OK;
}



/* --- end of file SPIFO.C --- */

