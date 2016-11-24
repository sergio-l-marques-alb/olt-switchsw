/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename  spnbo.c
 *
 * @purpose    OSPF Neighboring Object (NBO) FSM module.
 *
 * @component  Routing OSPF Component
 *
 * @comments    NBO_Init
 * NBO_Delete
 * NBO_Config
 * DbDescrReceived
 * LsReqReceived
 * LsUpdateReceived
 * LsAckReceived
 *
 * State machine routines
 *
 * DoNboTransition
 *
 * Internal Routines:
 *
 * NbEr
 * Nbo0-Nbo11
 *
 * SendDirectAck
 * SendDelayedAck
 * CheckAdj
 * SendDd
 * RetxUnpack
 *
 * Timer Expiration Routines
 *
 * InactivityTimerExp
 * DdRxmtTimerExp
 * RqRxmtTimerExp
 * LsRxmtTimerExp
 * SlaveLastDdTimerExp
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
static char __fileid[] = "$Header: /home/cvs/PR1003/PR1003/FASTPATH/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/src/application/routing/protocol/ospf/spnbo.c,v 1.1 2011/04/18 17:10:29 mruas Exp $";
#endif

#include <stdio.h>


/* --- standard include files --- */

#include "std.h"
#include "local.h"



/* --- external object interfaces --- */

/* --- specific include files --- */
#include "l7_common.h"
#include "log.h"
#include "spobj.h"
#include "osapi.h"
#include "osapi_support.h"
#include "os_xxcmn.h"
#include "nimapi.h"
#include "frame.h"
#include "buffer.h"
#include "ll.h"

#if L7_MIB_OSPF
#include "ospfmib.ext"
#endif
/* --- internal prototypes --- */

static Bool CheckAdj(t_NBO *p_NBO);
static void SendDd(t_NBO *p_NBO, Bool IsInit);
static void SendDirectAck(t_NBO *p_NBO, t_S_LsaHeader *p_LsaHdr);
static void SendDelayedAck(t_NBO *p_NBO, t_S_LsaHeader *p_LsaHdr);
static void ClearLsReTx(t_Handle NBO_Id);
e_Err DdRxmtTimerExp( t_Handle Id, t_Handle TimerId, word Flag );
e_Err RqRxmtTimerExp( t_Handle Id, t_Handle TimerId, word Flag );
e_Err SlaveLastDdTimerExp( t_Handle Id, t_Handle TimerId, word Flag );
void ospfDebugDdShow(char * str, t_A_DbEntry *p_DbEntry);
static Bool SelfOrigLsaReceived (t_RTO *p_RTO, t_S_LsaHeader *p_LsaHdr, t_IFO *p_IFO);

unsigned char *nbrStateNames[NUMB_OF_NBO_STATES] = {"DOWN", "ATTEMPT", "INIT", "2-WAY",
    "EX START", "EXCHANGE", "LOADING", "FULL"};

unsigned char *nbrEventNames[NUMB_OF_NBO_EVENTS] = {"NBO_HLO_RX", "NBO_START", "NBO_2WAY_RX",
	   "NBO_NEG_DONE", "NBO_EXC_DONE", "NBO_BAD_LSREQ", "NBO_LOAD_DONE", "NBO_ADJOK",
	      "NBO_SEQNUM_MISM", "NBO_1WAY_RX", "NBO_KILL", "NBO_INACT_TIMER", "NBO_LL_DOWN"};

unsigned char *lsaTypeNames[S_LAST_LSA_TYPE] = {"ILLEGAL LSA TYPE", "Router LSA", "Network LSA",
    "Summary LSA", "ASBR Sum LSA", "AS EXT LSA", "GROUP MEMB LSA", "NSSA LSA", "UNK", 
    "Link Opaque LSA", "Area Opaque LSA", "AS Opaque LSA"};


/*----------------------------------------------------------------
 *
 *             NBO SM Object Transition table
 *
 *----------------------------------------------------------------*/

static void NbEr(t_NBO *p_NBO, void *p_Prm);
static void Nbo0(t_NBO *p_NBO, void *p_Prm);
static void Nbo1(t_NBO *p_NBO, void *p_Prm);
static void Nbo2(t_NBO *p_NBO, void *p_Prm);
static void Nbo3(t_NBO *p_NBO, void *p_Prm);
static void Nbo4(t_NBO *p_NBO, void *p_Prm);
static void Nbo5(t_NBO *p_NBO, void *p_Prm);
static void Nbo6(t_NBO *p_NBO, void *p_Prm);
static void Nbo7(t_NBO *p_NBO, void *p_Prm);
static void Nbo8(t_NBO *p_NBO, void *p_Prm);
static void Nbo9(t_NBO *p_NBO, void *p_Prm);
static void Nbo10(t_NBO *p_NBO, void *p_Prm);
static void Nbo11(t_NBO *p_NBO, void *p_Prm);

typedef void (*f_Nbo_tr)(t_NBO *, void *);

static f_Nbo_tr Nbo_TT[NUMB_OF_NBO_EVENTS][NUMB_OF_NBO_STATES] = {
/*           DOWN,   ATMPT,  INIT,   2WAY,  START,   EXCH,   LOAD,   FULL */
/* HLO_RX */{Nbo1,   Nbo1,   Nbo9,   Nbo9,   Nbo9,   Nbo9,   Nbo9,   Nbo9},
/* START  */{Nbo5,   NbEr,   NbEr,   NbEr,   NbEr,   NbEr,   NbEr,   NbEr},
/* 2WAY_RX*/{NbEr,   NbEr,   Nbo2,   Nbo0,   Nbo0,   Nbo0,   Nbo0,   Nbo0},
/* NG_DONE*/{NbEr,   NbEr,   NbEr,   NbEr,   Nbo3,   NbEr,   NbEr,   NbEr},
/* EX_DONE*/{NbEr,   NbEr,   NbEr,   NbEr,   NbEr,   Nbo4,   NbEr,   NbEr},
/* BADLSRQ*/{NbEr,   NbEr,   NbEr,   NbEr,   NbEr,   Nbo7,   Nbo7,   Nbo7},
/* LD_DONE*/{NbEr,   NbEr,   NbEr,   NbEr,   NbEr,   NbEr,   Nbo10,  NbEr},
/* ADJOK  */{NbEr,   NbEr,   NbEr,   Nbo2,   Nbo6,   Nbo6,   Nbo6,   Nbo6},
/* SQNM_MS*/{NbEr,   NbEr,   NbEr,   NbEr,   NbEr,   Nbo7,   Nbo7,   Nbo7},
/* 1WAY_RX*/{NbEr,   NbEr,   Nbo0,   Nbo11,  Nbo11,  Nbo11,  Nbo11,  Nbo11},
/* KILL   */{Nbo8,   Nbo8,   Nbo8,   Nbo8,   Nbo8,   Nbo8,   Nbo8,   Nbo8},
/* INAC_TM*/{Nbo8,   Nbo8,   Nbo8,   Nbo8,   Nbo8,   Nbo8,   Nbo8,   Nbo8},
/* LL_DOWN*/{Nbo8,   Nbo8,   Nbo8,   Nbo8,   Nbo8,   Nbo8,   Nbo8,   Nbo8}
}; /* end of table */

/*********************************************************************
 * @purpose              Initialize an NBO object.
 *
 *
 * @param   IFO_Id       @b{(input)}  Corresponded IFO object
 * @param   RouterId     @b{(input)}  Neighboring Router Id
 * @param   SrcIpAdr     @b{(input)}  Neighboring IP Address
 * @param   p_Cfg        @b{(input)}  timer values and other configs
 * @param   p_Object     @b{(output)}  the object ID is returned here
 *
 * @returns   E_OK           success
 * @returns   E_FAILED       couldn't create an object
 * @returns   E_NOMEMORY     not enough memory for resources allocating
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err NBO_Init(t_Handle IFO_Id, SP_IPADR RouterId, SP_IPADR SrcIpAdr,
                t_IFO_Cfg *p_Cfg, t_Handle *p_Object)
{
   t_NBO   *p_NBO;
   t_IFO   *p_IFO = (t_IFO *)IFO_Id;
   e_Err   e = E_OK;
   t_RTO   *p_RTO = ((t_RTO *)p_IFO->RTO_Id);

   /* Hello's would be ignored if the system or the interface have
    * reached maximum neighbors */
   if(p_RTO->neighborsToRouter == OSPF_MAX_NBRS)
   {
      p_IFO->PckStats.HellosIgnored++;
      return E_NOMEMORY;
   }

   if(p_IFO->neighborsOnIface == OSPF_MAX_NBRS_ON_IFACE)
   {
      p_IFO->PckStats.HellosIgnored++;
      return E_NOMEMORY;
   }

   /* Neigbhors can't be created on passive interfaces */
   if(p_IFO->Cfg.passiveMode == TRUE)
      return E_FAILED;

   /* try allocate from user memory */
   p_NBO = (t_NBO *)XX_Malloc( sizeof(t_NBO) );
   if(p_NBO)
      memset( p_NBO, 0, sizeof(t_NBO) );
   else
      return E_NOMEMORY;

   p_RTO->neighborsToRouter++;
   p_IFO->neighborsOnIface++;

   /* initialize the object */

   p_NBO->Taken = TRUE;

   /* Copy Configuration */
   p_NBO->RxmtInterval = p_Cfg->RxmtInterval;
   p_NBO->OspfSysLabel = ((t_RTO *)((t_IFO *) IFO_Id)->RTO_Id)->OspfSysLabel;
   p_NBO->RouterId = RouterId;
   p_NBO->IpAdr = SrcIpAdr;
   p_NBO->IFO_Id = IFO_Id;

   /* Set corresponded RTO Id and ARO Id */
   p_NBO->RTO_Id = ((t_IFO *)IFO_Id)->RTO_Id;
   p_NBO->ARO_Id = ((t_IFO *)IFO_Id)->ARO_Id;

   p_NBO->DdSeqNum = TICKS_TO_SECONDS(TIMER_SysTime()); /* first assignment */
  
   p_NBO->Nbo_TT = Nbo_TT; /* pointer to Neighboring SM transition table */
   p_NBO->State = NBO_DOWN;    /* Current state   */
   p_NBO->NbrHelloSuppressed = FALSE;
  
   /* Inactivity timer handler */
   e |= TIMER_InitSec( 1, p_NBO, &p_NBO->InactivityTimer );
   /* DdRxmtTimer timer handler */
   e |= TIMER_InitSec( 1, p_NBO, &p_NBO->DdRxmtTimer );
   /* RqRxmtTimer timer handler */
   e |= TIMER_InitSec( 1, p_NBO, &p_NBO->RqRxmtTimer );
   /* SlaveLastDdTimer timer handler */
   e |= TIMER_InitSec( 1, p_NBO, &p_NBO->SlaveLastDdTimer );
    
   /* allocate Neighbor Link State retransmission Hash List */
   e |= HL_Init(FREE_ON_DEL|KEY_AT_START, 9, 
               /* key offset in t_RetxEntry */
                offsetof(t_RetxEntry, LsType),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_NBO->LsReTx);

   p_NBO->LsReTxHead = NULL;
   p_NBO->LsReTxTail = NULL;

   /* allocate Neighbor Data Base Summary Hash List */
   e |= HL_Init(OWN_LINK|KEY_AT_START,9, 
               /* key offset in t_A_DbEntry */
               offsetof(t_A_DbEntry,Lsa) + offsetof(t_S_LsaHeader, LsType),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_NBO->DbSum);

   /* allocate Neighbor Link State request list */
   e |= HL_Init(FREE_ON_DEL|KEY_AT_START,9, 
               /* key offset in t_A_DbEntry */
               offsetof(t_LsReqEntry,Lsa) + offsetof(t_S_LsaHeader, LsType),
                HL_SMALL_TABLE_SIZE, NULL, NULL, &p_NBO->LsReq);
   
   e |= HL_Insert(((t_IFO*)IFO_Id)->NboHl, (byte*) &p_NBO->RouterId, p_NBO);
   
   if(e == E_OK)
   {
      /* pass object ID back to caller */
      *p_Object = (t_Handle) p_NBO;     
      p_NBO->OperationState = TRUE;
      
      
      /* Neighbor Object creation indication callback */
      if(((t_RTO*)p_NBO->RTO_Id)->Clbk.f_NeighborCreateInd)
         ((t_RTO*)p_NBO->RTO_Id)->Clbk.f_NeighborCreateInd(
         ((t_ARO*)p_NBO->ARO_Id)->MngId, p_NBO, &p_NBO->MngId);
   }
   else
      NBO_Delete((t_Handle) p_NBO, 1);

#if L7_MIB_OSPF
   /* If the neighbor object has been created successfully,  */
   /* create and activate a new row in the OSPF MIB Neighbor */
   /* table. NBMA neighbor is created only by SNMP request.  */
   if (e == E_OK)
   {
      if (((t_IFO *)IFO_Id)->Cfg.Type == IFO_VRTL)
      {
         ospfVirtNbrTable_NewInternal(((t_RTO*)p_NBO->RTO_Id)->ospfMibHandle,
                              ((t_IFO *)IFO_Id)->Cfg.VirtTransitAreaId,
                              p_NBO->RouterId, p_NBO);
      }
      else
      {
        ospfNbrTable_InternalNewAndActive(((t_RTO*)p_NBO->RTO_Id)->ospfMibHandle,
                                          p_NBO->IpAdr, ((t_IFO *)IFO_Id)->Cfg.IfIndex, p_NBO);
      }
   }
#endif
      
   return e;
}

/*********************************************************************
 * @purpose              Clear the LS retxt list and HL
 *
 *
 * @param   NBO_Id       @b{(input)}  Neighbor object
 *
 * @returns   E_OK           success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void ClearLsReTx(t_Handle NBO_Id)
{
  t_NBO *p_NBO = (t_NBO*) NBO_Id;
  t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;
  word numEntries;

  ASSERT(p_NBO);

  HL_GetEntriesNmb(p_NBO->LsReTx, &numEntries);
  HL_Cleanup(p_NBO->LsReTx, 1);
  p_RTO->retxEntries -= numEntries;
  p_NBO->LsReTxHead = 0;
  p_NBO->LsReTxTail = 0;
}

/*********************************************************************
 * @purpose              Insert a ReTx entry to the retx list and HL
 *
 *
 * @param   NBO_Id       @b{(input)}  Neighbor object
 * @param   key          @b{(input)}  pointer to HL key 
 * @param   entry        @b{(input)}  pointer to ReTx entry
 *
 * @returns          E_OK        success
 * @returns          E_BADPARAM  wrong parameters (from HL)
 *
 * @notes 
 *          The retx HL is organized by LsType, LsId. The retx list is
 *          organized by sent time. Entries are inserted at the tail.
 *
 *          If the key parameter is NULL, the entry is inserted in the 
 *          list but not in the HL.
 *
 * @end
 * ********************************************************************/
e_Err NBO_LsReTxInsert(t_Handle NBO_Id, byte *key, t_Handle entry)
{
  t_RetxEntry *p_RetxEntry = (t_RetxEntry *) entry;
  t_NBO       *p_NBO       = (t_NBO *)NBO_Id;
  e_Err       rc           = E_OK;

  ASSERT(p_RetxEntry);  /* must have entry to insert */

  if(key != NULL)
    rc = HL_Insert(p_NBO->LsReTx, key, entry);

  if(rc == E_OK)
  {
    if(p_NBO->LsReTxTail)
    {
      
      ASSERT(p_NBO->LsReTxHead != 0); /* have tail, must have head */
      ASSERT(((t_RetxEntry *)p_NBO->LsReTxTail)->nextSent == 0); /* tail must be last */

      p_RetxEntry->prevSent = p_NBO->LsReTxTail;
      p_RetxEntry->nextSent = 0;

      ((t_RetxEntry *)p_NBO->LsReTxTail)->nextSent = p_RetxEntry;
      p_NBO->LsReTxTail = p_RetxEntry; /* reset tail ptr */
    }
    else
    {
      p_NBO->LsReTxHead = p_RetxEntry;
      p_NBO->LsReTxTail = p_RetxEntry;
      p_RetxEntry->nextSent = 0;
      p_RetxEntry->prevSent = 0;
    }
  }

  return rc;
}

/*********************************************************************
 * @purpose              Delete a ReTx entry from the retx list and HL
 *
 *
 * @param   NBO_Id       @b{(input)}  Neighbor object
 * @param   key          @b{(input)}  pointer to HL key 
 * @param   entry        @b{(input)}  pointer to ReTx entry
 *
 * @returns          E_OK        success
 * @returns          E_BADPARAM  wrong parameters (from HL)
 *
 * @notes 
 *          The retx HL is organized by LsType, LsId. The retx list is
 *          organized by sent time. 
 *
 *          If the entry parameter is NULL, the function attempts to 
 *          find the entry in the HL using the key (if provided).
 *
 *          If the key parameter is NULL, the entry is deleted from the 
 *          list but not the HL.
 *
 * @end
 * ********************************************************************/
e_Err NBO_LsReTxDelete(t_Handle NBO_Id, byte *key, t_Handle entry)
{
  t_RetxEntry *p_Prev, *p_Next;
  t_RetxEntry *p_RetxEntry = (t_RetxEntry *) entry;
  t_NBO       *p_NBO       = (t_NBO *)NBO_Id;
  t_RTO       *p_RTO = (t_RTO*) p_NBO->RTO_Id;
  e_Err       rc           = E_OK;

  /* try to find the retx entry if not specified */
  if(key != NULL && p_RetxEntry == NULL)
  {
    if(HL_FindFirst(p_NBO->LsReTx, key, (void**)&p_RetxEntry) != E_OK)
      p_RetxEntry = NULL;
  }

  /* if found, remove from link list */
  if(p_RetxEntry)
  {
    p_Next = p_RetxEntry->nextSent;
    p_Prev = p_RetxEntry->prevSent;

    if(p_Prev)
      p_Prev->nextSent = p_Next;

    if(p_Next)
      p_Next->prevSent = p_Prev;

    if(p_NBO->LsReTxHead == p_RetxEntry)
      p_NBO->LsReTxHead = p_Next;

    if(p_NBO->LsReTxTail == p_RetxEntry)
      p_NBO->LsReTxTail = p_Prev;

    p_RetxEntry->nextSent = NULL;
    p_RetxEntry->prevSent = NULL;
  }

  /* attempt to delete from the HL */
  if(key != NULL)
  {
    rc = HL_Delete(p_NBO->LsReTx, key, entry);
    if (rc == E_OK)
    {
      p_RTO->retxEntries--;
    }
  }

  return rc;
}

/*********************************************************************
 * @purpose              Update the sent time and position of a Retx Entry
 *
 *
 * @param   NBO_Id       @b{(input)}  Neighbor object
 * @param   entry        @b{(input)}  pointer to ReTx entry
 *
 * @returns          E_OK        success
 * @returns          E_BADPARAM  wrong parameters (from HL)
 *
 * @notes 
 *          The retx HL is organized by LsType, LsId. The retx list is
 *          organized by sent time. Update the sent time and move the
 *          entry to the tail of the list.
 *
 * @end
 * ********************************************************************/
e_Err NBO_LsReTxUpdateSentTime(t_Handle NBO_Id, t_Handle entry)
{
  t_NBO       *p_NBO       = (t_NBO *)NBO_Id;
  t_RetxEntry *p_RetxEntry = (t_RetxEntry *) entry;

  /* update sent time */
  p_RetxEntry->SentTime = TIMER_SysTime();

  /* move to tail of the list */
  NBO_LsReTxDelete(p_NBO, NULL, p_RetxEntry);
  NBO_LsReTxInsert(p_NBO, NULL, p_RetxEntry);

  return E_OK;
}

/*********************************************************************
 * @purpose           Delete a NBO object.
 *
 *
 * @param Id           @b{(input)}  NBO object Id
 * @param flag         @b{(input)}  delete this entry from IFO's NboHl flag
 *
 * @returns            TRUE if this entry remains in IFO's NboHl,
 * @returns            FALSE otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool NBO_Delete( t_Handle Id, ulng flag)
{
   t_NBO   *p_NBO = (t_NBO *)Id;
   t_ARO   *p_ARO = (t_ARO *) p_NBO->ARO_Id;
   t_RTO   *p_RTO = (t_RTO*) p_NBO->RTO_Id;
   t_IFO   *p_IFO = (t_IFO *) p_NBO->IFO_Id;
   word numRetx = 0;

   p_NBO->OperationState = FALSE;

   /* Update ARO's ExchOrLoadState counter */
   if (p_ARO && p_NBO->ExchOrLoadState)
   {
      p_ARO->ExchOrLoadState -= 1;
      p_NBO->ExchOrLoadState = FALSE;
   } 

   if(flag)
      HL_Delete(((t_IFO*)p_NBO->IFO_Id)->NboHl, (byte*) &p_NBO->RouterId, p_NBO);

#if L7_MIB_OSPF
   {
      if (((t_IFO*)p_NBO->IFO_Id)->Cfg.Type == IFO_VRTL)
      {
         /* This is a virtual Neighbor. Update the MIB VirtNbr Table. */
         ospfVirtNbrTable_Delete(((t_RTO*)p_NBO->RTO_Id)->ospfMibHandle,
                                 ((t_IFO *)p_NBO->IFO_Id)->Cfg.VirtTransitAreaId,
                                 p_NBO->RouterId);
      }
      else
      {
        /* Update the MIB Neighbor Table. */
        ospfNbrTable_DeleteInternal( ((t_RTO*)p_NBO->RTO_Id)->ospfMibHandle,
                                     p_NBO->IpAdr, 
                                     ((t_IFO*)p_NBO->IFO_Id)->Cfg.IfIndex);
      }
   }      
#endif

   HL_GetEntriesNmb(p_NBO->LsReTx, &numRetx);
   HL_Destroy(&p_NBO->LsReTx); /* Link State retransmission list */
   p_RTO->retxEntries -= numRetx;
   p_NBO->LsReTxHead = NULL;
   p_NBO->LsReTxTail = NULL;

   HL_Destroy(&p_NBO->DbSum);  /* Data Base Summary list */
   HL_Destroy(&p_NBO->LsReq);  /* Link State request list */

   TIMER_Delete( p_NBO->InactivityTimer );
   TIMER_Delete( p_NBO->DdRxmtTimer );
   TIMER_Delete( p_NBO->RqRxmtTimer );
   TIMER_Delete( p_NBO->SlaveLastDdTimer );
                         
   if(p_NBO->p_LastDd)
   {
      F_Delete(p_NBO->p_LastDd);
      p_NBO->p_LastDd = 0;
   }

   p_NBO->Taken = FALSE;
   XX_Free( p_NBO );

   p_RTO->neighborsToRouter--;
   p_IFO->neighborsOnIface--;

   return ((Bool) flag);
}




/*********************************************************************
 * @purpose             Get Neighbor object information.
 *
 *
 * @param Id            @b{(input)}  NBO object Id
 * @param p_Ngb         @b{(input)}  pointer to Neighbor Info to copy
 *
 * @returns             E_OK       success
 * @returns             E_FAILED   bad parameters
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err NBO_Config(t_Handle Id, t_S_NeighborCfg *p_Ngb)
{
   t_NBO   *p_NBO = (t_NBO *)Id;
   t_IFO   *p_IFO;
   word    num;

   if(!p_NBO || !p_NBO->Taken || !p_NBO->OperationState)
      return E_FAILED;

   p_IFO = (t_IFO *) p_NBO->IFO_Id;

   switch(p_Ngb->NbmaNbrStatus)   
   {
      case  ROW_ACTIVE:
         p_NBO->OperationState = TRUE;
      break;   
      case  ROW_NOT_IN_SERVICE:
         p_NBO->OperationState = FALSE;
      break;   
      case  ROW_CHANGE:
         p_NBO->RouterPriority = p_Ngb->NbrPriority;
      break;
      case  ROW_READ:         
         p_Ngb->IpAdr = p_NBO->IpAdr;
         p_Ngb->IfIndex = p_IFO->Cfg.IfIndex;
         p_Ngb->NbrRtrId = p_NBO->RouterId;   
         p_Ngb->NbrOptions = p_NBO->Options;
         p_Ngb->NbrPriority = p_NBO->RouterPriority;  
         p_Ngb->NbrState = p_NBO->State + 1;
         p_Ngb->NbrArea = ((t_ARO *)(p_NBO->ARO_Id))->AreaId;
         p_Ngb->NbrEvents = p_NBO->NbrEvents;
         HL_GetEntriesNmb(p_NBO->LsReTx, &num);
         p_Ngb->NbrLsRetransQLen = (ulng) num;
         p_Ngb->NbmaNbrPermanence = 
            ((((t_IFO*)p_NBO->IFO_Id)->Cfg.Type == IFO_NBMA) ||
             (((t_IFO*)p_NBO->IFO_Id)->Cfg.Type == IFO_VRTL)) ? 1 : 2;
         p_Ngb->NbrHelloSuppressed = p_NBO->NbrHelloSuppressed;
         p_Ngb->NbmaNbrStatus = p_NBO->OperationState ? ROW_ACTIVE : 
                                                        ROW_NOT_IN_SERVICE;
         p_Ngb->NbrDeadTimerRemaining = TIMER_ToExpireMilli(p_NBO->InactivityTimer);
         /* seconds neighbor has been up */
         p_Ngb->NbrUptime = TICKS_TO_SECONDS(TIMER_SysTime() - p_NBO->Uptime);   
      break;
      case  ROW_DESTROY:
         NBO_Delete(p_NBO, 1);
      break;
      default:
         return E_FAILED;      
      break;
   }      
   return E_OK;
}

/*********************************************************************
 * @purpose     Determine if router has an adjacency in Exchange or 
 *              Loading state for a given neighbor in a given area. 
 *
 *
 * @param p_ARP    @b{(input)}  area 
 * @param nbrId    @b{(input)}  neighbor router ID
 *
 * @returns     L7_TRUE or L7_FALSE
 *
 * @notes
 *
 * @end
 * ********************************************************************/
L7_BOOL AdjInExchOrLoad(t_ARO *p_ARO, L7_uint32 nbrId)
{
  e_Err e;
  t_IFO *p_IFO;
  t_NBO *p_NBO;

  e = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
  while(e == E_OK && p_IFO)
  {
    /* browse neighbors */
    e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
    while(e == E_OK && p_NBO)
    {
      if ((p_NBO->RouterId == nbrId) &&
          ((p_NBO->State == NBO_EXCHANGE) || (p_NBO->State == NBO_LOADING)))
      {
        return L7_TRUE;
      }

      e = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
    }
    /* get next interface */
    e = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
  }
  return L7_FALSE;
}

/*********************************************************************
 * @purpose        Processing of receiving Database Description packet.
 *
 *
 * @param p_NBO    @b{(input)}  NBO object Id
 * @param p_Hdr    @b{(input)}  packet's header pointer
 *
 * @returns        n/a.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void DbDescrReceived(t_NBO *p_NBO, t_S_PckHeader *p_Hdr)
{
   t_S_DbDscr *p_DbDscr;   
   t_RTO *p_RTO = (t_RTO *) p_NBO->RTO_Id;
   t_IFO *p_IFO = (t_IFO*) p_NBO->IFO_Id;
   t_ARO *p_ARO = (t_ARO *) p_NBO->ARO_Id;
   SP_IPADR LocRouterId = p_RTO->Cfg.RouterId;
   Bool IsDuplicate = FALSE;
   word entries_num, lsrqs;
   t_S_LsaHeader *p_LsaHdr;
   t_A_DbEntry *p_DbEntry;
   t_LsReqEntry *p_LsReqEntry;
   e_Err er;
   static L7_uint32 mtuMismatch = 0;
   /* Number of DDs dropped for MTU mismatch between each log msg. */
   L7_uint32 mtuMismatchCount = 10;    
   static L7_uint32 fatNeighbor = 0;

   if(!p_NBO->OperationState)
      return;
      
   /* Get pointer to the DD payload */
   p_DbDscr =  (t_S_DbDscr *) GET_PAYLOAD_PTR(p_Hdr);

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_DD)
   {
       char traceBuf[OSPF_MAX_TRACE_STR_LEN];
       L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
       L7_uchar8 srcRouterIdStr[OSAPI_INET_NTOA_BUF_SIZE];
       L7_uchar8 flagStr[10];
       L7_uchar8 optStr[20];
       flagStr[0] = '\0';
       if (p_DbDscr->Flags & S_INIT_FLAG)
           strncat(flagStr, "I/", 2);
       if (p_DbDscr->Flags & S_MORE_FLAG)
           strncat(flagStr, "M/", 2);
       if (p_DbDscr->Flags & S_MASTER_FLAG)
           strncat(flagStr, "MS", 2);    
       OspfOptionsStr(p_DbDscr->Options, optStr);
       RTO_InterfaceName(p_IFO, ifName);
       osapiInetNtoa((L7_uint32) p_NBO->RouterId, srcRouterIdStr);
       sprintf(traceBuf, "OSPF DD received on interface %s from router %s. MTU: %d Options: %s Flags: %s SeqNo: %lu Length: %d ",
               ifName, srcRouterIdStr, A_GET_2B(p_DbDscr->InterfaceMtu), optStr, flagStr, 
               A_GET_4B(p_DbDscr->SeqNum), A_GET_2B(p_Hdr->Length));
       RTO_TraceWrite(traceBuf);
   }

  if (p_IFO->Cfg.MtuIgnore == FALSE)
   {
    L7_uint32 ddMtu = A_GET_2B(p_DbDscr->InterfaceMtu);
    if (ddMtu > (p_IFO->Cfg.MaxIpMTUsize))
    {
      /* Quick attempt to limit the number of these in the log. */
      if ((mtuMismatch % mtuMismatchCount) == 0)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        RTO_InterfaceName(p_IFO, ifName);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
                "Dropping OSPFv2 DD packet received on interface %s. DD MTU is %u. Local MTU is %u."
                " OSPFv2 ignored a Database Description packet whose MTU is greater than the IP MTU"
                " on the interface where the DD was received.",
                ifName, ddMtu, p_IFO->Cfg.MaxIpMTUsize);
      }
      p_NBO->NbrEvents += 1;
      mtuMismatch++;
         return; /* reject the packet */
      }
   }
   
   if((p_DbDscr->Options == p_NBO->LastRxDd.Options) &&
      (p_DbDscr->Flags   == p_NBO->LastRxDd.Flags)   &&
      (A_GET_4B(p_DbDscr->SeqNum) == A_GET_4B(p_NBO->LastRxDd.SeqNum)))
   {
      IsDuplicate = TRUE;   
      p_NBO->dupDDs++;
   }
   else
   {
     p_NBO->dupDDs = 0;
   }

   switch(p_NBO->State)
   {
      case NBO_DOWN:
      case NBO_ATTEMPT:
      case NBO_2WAY:
      return;   /* reject the packet */
      case NBO_INIT:

         DoNboTransition(p_NBO, NBO_2WAY_RX, 0);
   
         if(p_NBO->State != NBO_EXSTART)
            return; /* reject the packet */
            
      case NBO_EXSTART:

         /* Check limit for number of neighbors in exchange or loading */
         if (p_ARO->ExchOrLoadState >= OSPF_MAX_NBRS_EXCH_LOAD)
         {
           return;
         }
      
         /* If an adjacency with same router is already in Exchange or Load, 
          * let that adjacency get to FULL before starting db exchange on 
          * another adjacency. Trying to avoid sending same LSAs to the 
          * neighbor multiple times. Duplicate LSAs trigger directed ACKs
          * with one ACK per packet. Want to avoid that. */
         if (AdjInExchOrLoad(p_ARO, p_NBO->RouterId))
         {
           if (p_RTO->ospfTraceFlags & OSPF_TRACE_DD)
           {
               char traceBuf[OSPF_MAX_TRACE_STR_LEN];
               L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
               RTO_InterfaceName(p_IFO, ifName);
               sprintf(traceBuf, "Dropping DD on %s because adj to nbr already in exch or load. ",
                       ifName);
               RTO_TraceWrite(traceBuf);
           }

           return;
         }

         if ((p_DbDscr->Flags == (S_INIT_FLAG | S_MORE_FLAG | S_MASTER_FLAG)) &&
             (A_GET_2B(p_Hdr->Length) == (sizeof(t_S_PckHeader) + sizeof(t_S_DbDscr))))
         {
             /* Received an initial DD. If neighbor's router ID is larger than 
              * the local router ID, consider the neighbor the master.
              * Otherwise, ignore the DD. */
             if (p_NBO->RouterId > LocRouterId)
             {
                 p_NBO->IsMaster = FALSE;
                 p_NBO->DdSeqNum = A_GET_4B(p_DbDscr->SeqNum);
             }  
             else
             {
                 /* ignore the DD. This case is expected. Neighbor should send
                  * another DD agreeing to be slave. */
                 return;
             }
         }
         else if(((p_DbDscr->Flags & (S_INIT_FLAG | S_MASTER_FLAG)) == 0) &&
                 (A_GET_4B(p_DbDscr->SeqNum) == p_NBO->DdSeqNum) &&
                 (p_NBO->RouterId < LocRouterId))
         {
             /* Neighbor has agreed to be slave */
             p_NBO->IsMaster = TRUE;
         }
         else
         {
             L7_uchar8 srcRouterIdStr[OSAPI_INET_NTOA_BUF_SIZE];
             L7_uchar8 flagStr[10];
             flagStr[0] = '\0';
             if (p_DbDscr->Flags & S_INIT_FLAG)
                 strncat(flagStr, "I/", 2);
             if (p_DbDscr->Flags & S_MORE_FLAG)
                 strncat(flagStr, "M/", 2);
             if (p_DbDscr->Flags & S_MASTER_FLAG)
                 strncat(flagStr, "MS", 2);
             osapiInetNtoa((L7_uint32)p_NBO->RouterId, srcRouterIdStr);
             L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_OSPF_MAP_COMPONENT_ID,
                     "Dropping DD packet from neighbor %s. flags %s; DD Length %d",
                     srcRouterIdStr, flagStr, A_GET_2B(p_Hdr->Length));
             return; /* reject the packet */
         }

         p_NBO->Options =  p_DbDscr->Options;
            
         DoNboTransition(p_NBO, NBO_NEG_DONE, 0);
         
      break;
      case NBO_EXCHANGE:
         if (IsDuplicate)
         {
            if(!p_NBO->IsMaster)
            {
               /* retransmit the last sent DD packet */
               if(p_NBO->p_LastDd)
               {
                 void *ddClone = F_Copy(p_NBO->p_LastDd);
                 if (ddClone)
                   IFO_SendPacket(p_NBO->IFO_Id, ddClone, S_DB_DESCR, p_NBO, TRUE);   
               }
               else
               {
                 L7_uchar8 srcRouterIdStr[OSAPI_INET_NTOA_BUF_SIZE];
                 osapiInetNtoa((L7_uint32)p_NBO->RouterId, srcRouterIdStr);
                 L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_OSPF_MAP_COMPONENT_ID, 
                         "Unable to retransmit last DD to neighbor %s after receiving "
                         "duplicate DD in EXCHANGE state.", srcRouterIdStr);
               }
            }
            else
            {
              L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_OSPF_MAP_COMPONENT_ID, 
                      "Master discarding duplicate DD in EXCHANGE state."
                      " The local router is Master for database exchange and received "
                      "a duplicate database description packet while in Exchange state."
                      "  The router ignores the DD.");
            }
            return; /* reject the packet */
         }
         
         /* Check the bad seqnum conditions */
         if(((p_DbDscr->Flags & S_MASTER_FLAG) == p_NBO->IsMaster) ||
            (p_DbDscr->Flags & S_INIT_FLAG) ||
            (p_DbDscr->Options != p_NBO->Options) ||
            (p_NBO->IsMaster && (A_GET_4B(p_DbDscr->SeqNum) != p_NBO->DdSeqNum)) ||
            (!p_NBO->IsMaster && (A_GET_4B(p_DbDscr->SeqNum) != (p_NBO->DdSeqNum+1))))
         {
            DoNboTransition(p_NBO, NBO_SEQNUM_MISM, 0);
            return;
         }   
         
      break;
      case NBO_LOADING:
      case NBO_FULL:
         if ((p_DbDscr->Flags & S_INIT_FLAG) ||
             (p_DbDscr->Options != p_NBO->Options))
         {
             L7_uchar8 *stateStr = "OTHER";
             if (p_NBO->State == NBO_LOADING)
                 stateStr = "LOADING";
             else if (p_NBO->State == NBO_FULL)
                 stateStr = "FULL";
             L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_OSPF_MAP_COMPONENT_ID, 
                    "Discarding invalid DD in state %s."
                     " OSPF generates this message when an adjacency is"
                     " in LOADING or FULL state and receives a database"
                     " description packet with the INIT flag set or whose"
                     " options do not match the options previously recorded"
                     " for the neighbor.", stateStr);
             DoNboTransition(p_NBO, NBO_SEQNUM_MISM, 0);
             return;
         }
         if (p_NBO->IsMaster && IsDuplicate)
         {
             /* discard DD */
             L7_uchar8 *stateStr = "OTHER";
             if (p_NBO->State == NBO_LOADING)
                 stateStr = "LOADING";
             else if (p_NBO->State == NBO_FULL)
                 stateStr = "FULL";
             L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_OSPF_MAP_COMPONENT_ID, 
                    "Master discarding duplicate DD in state %s."
                     " The local router is Master for database exchange"
                     " and received a duplicate database description "
                     "packet while in LOADING or FULL state.  The router"
                     " ignores the DD.", stateStr);
             return;
         }
         if (!p_NBO->IsMaster && IsDuplicate)
         {
             /* retransmit the last sent DD packet */
             if (p_NBO->p_LastDd)
             {
               void *ddClone = F_Copy(p_NBO->p_LastDd);
               if (ddClone)
                 IFO_SendPacket(p_NBO->IFO_Id, ddClone, S_DB_DESCR, p_NBO, TRUE);   
             }
             else
             {
                 /* RouterDeadInterval has already expired and last DD 
                  * has already been freed. Do SeqNumberMismatch. See
                  * RFC 2328 section 10.8. */
                 DoNboTransition(p_NBO, NBO_SEQNUM_MISM, 0);
             }
             return;
         }

         /* Reject any other DD received in this state */
         L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_OSPF_MAP_COMPONENT_ID, 
                 "Discarding invalid DD");
         DoNboTransition(p_NBO, NBO_SEQNUM_MISM, 0);
         return;
          
      default:
      return; /* reject the packet */
   }      

   memcpy(&p_NBO->LastRxDd, p_DbDscr, sizeof(t_S_DbDscr));

   /* Check attached LSA header list */
   p_LsaHdr = (t_S_LsaHeader *)((byte*)p_DbDscr + sizeof(t_S_DbDscr));
   entries_num = (A_GET_2B(p_Hdr->Length)-sizeof(t_S_PckHeader)-sizeof(t_S_DbDscr)) / 
                     sizeof(t_S_LsaHeader);
   while(entries_num--)
   {
      if((p_LsaHdr->LsType < S_ROUTER_LSA) ||
         (p_LsaHdr->LsType >= S_LAST_LSA_TYPE) ||
         (p_LsaHdr->LsType == S_TMP2_LSA) ||
         ((p_LsaHdr->LsType == S_AS_EXTERNAL_LSA) &&
          ((p_NBO->Options & OSPF_OPT_E_BIT) == 0)))
      {
         DoNboTransition(p_NBO, NBO_SEQNUM_MISM, 0);
         return;
      }
      
      /* Find in corresponded ARO Database the match entry */
      er = AVLH_Find(GET_OWNER_HL((t_ARO *)p_NBO->ARO_Id, p_LsaHdr->LsType), 
                      (byte*)&p_LsaHdr->LsId, (void *)&p_DbEntry,0);

      /* If found - update LSA age in the data base */
      if(er == E_OK)
         UpdateAgeOfDbEntry (p_DbEntry);

      if((er != E_OK) || ((er == E_OK) && 
         (LsaCompare(p_LsaHdr, &p_DbEntry->Lsa) == LSA_MORE_RECENT)))      
      {
         /* Service the overflowed state: if this is a new LSA and it's an as-ext and not default,
            don't request it */
         if(p_RTO->IsOverflowed && (p_LsaHdr->LsType == S_AS_EXTERNAL_LSA) &&
            (er != E_OK) && (A_GET_4B(p_LsaHdr->LsId) != 0))
         {
            p_LsaHdr ++;
            continue;
         }

         /* only if didn't add yet */
         if(HL_FindFirst(p_NBO->LsReq, &p_LsaHdr->LsType, (void *)&p_LsReqEntry) != E_OK)
         {
           word reqListEntries = 0;

           /* Limit the number of request list entries */
           HL_GetEntriesNmb(p_NBO->LsReq, &reqListEntries);
           if (reqListEntries >= OSPF_MAX_LSAS)
           {
             L7_uchar8 srcRouterIdStr[OSAPI_INET_NTOA_BUF_SIZE];
             L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

             /* Don't print this over and over again for same neighbor */
             if (p_NBO->RouterId != fatNeighbor)
             {
               fatNeighbor = p_NBO->RouterId;
               RTO_InterfaceName(p_IFO, ifName);
               osapiInetNtoa((L7_uint32)p_NBO->RouterId, srcRouterIdStr);
               L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
                       "Neighbor %s on interface %s offers too many LSAs. Bouncing adjacency.",
                       srcRouterIdStr, ifName);
             }
             DoNboTransition(p_NBO, NBO_SEQNUM_MISM, 0);
             return;
           }

            p_LsReqEntry = XX_Malloc(sizeof(t_LsReqEntry));
            if (p_LsReqEntry == NULL)
            {
              /* Rather than enter overload, let's refuse to form adjacency. */
              L7_uchar8 srcRouterIdStr[OSAPI_INET_NTOA_BUF_SIZE];
              L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
              RTO_InterfaceName(p_IFO, ifName);
              osapiInetNtoa((L7_uint32)p_NBO->RouterId, srcRouterIdStr);
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
                      "Failed to allocate memory for request list entry while establishing "
                      "adjacency with router %s on interface %s. Bouncing adjacency.",
                      srcRouterIdStr, ifName);
              DoNboTransition(p_NBO, NBO_SEQNUM_MISM, 0);
              return;
            }

            /* Add to LS Request list */
            memcpy(&p_LsReqEntry->Lsa, p_LsaHdr, sizeof(t_S_LsaHeader));
            HL_Insert(p_NBO->LsReq, &p_LsReqEntry->Lsa.LsType, p_LsReqEntry);
         }
      }         
         
      p_LsaHdr ++;
   }                     
   
    /* if LS Request list is not empty, then 
       send the LS Request packet and 
       Start LS Request Retransmission timer */
   HL_GetEntriesNmb(p_NBO->LsReq, &lsrqs);
   if(lsrqs && !TIMER_Active(p_NBO->RqRxmtTimer))
      RqRxmtTimerExp(p_NBO,0,0);
   
   if(!p_NBO->IsMaster)
   {
      p_NBO->DdSeqNum = A_GET_4B(p_DbDscr->SeqNum);
      
      SendDd(p_NBO, 0);
   }

   if(!p_NBO->p_OutDdEntry && (p_DbDscr->Flags & S_MORE_FLAG) == 0)
   {
      DoNboTransition(p_NBO, NBO_EXC_DONE, 0);
   }         
   else if(p_NBO->IsMaster)
   {
      p_NBO->DdSeqNum += 1;

      /* Start DD Retransmission timer */
      TIMER_StartSec(p_NBO->DdRxmtTimer, TimerFractVar(p_NBO->RxmtInterval),
                     0, DdRxmtTimerExp, p_NBO->OspfSysLabel.threadHndle );

      SendDd(p_NBO, 0);
   }
}



/*********************************************************************
 * @purpose          Processing of receiving Link State Request packet.
 *
 *
 * @param  p_NBO     @b{(input)}  NBO object Id
 * @param  p_Hdr     @b{(input)}  packet's header pointer
 *
 * @returns          n/a.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void LsReqReceived(t_NBO *p_NBO, t_S_PckHeader *p_Hdr)
{
   t_S_LsReq *p_LsReq;
   t_A_DbEntry *p_DbEntry;
   word entries_num, size, num;
   void *p_LsUpdate;
   t_S_LsaHeader *p_LsaHdr;
   t_RetxEntry *p_TmpRetx;
   ulng allocPktLen, ipPktLen;
   L7_uint32 lsaLength;
   word authLen = 0;
   t_IFO *p_IFO = (t_IFO*) p_NBO->IFO_Id;
   t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;

   if(!p_NBO->OperationState || (p_NBO->State < NBO_EXCHANGE))
      return;
      
   /* Get pointer to the packet payload */
   p_LsReq = (t_S_LsReq *) GET_PAYLOAD_PTR(p_Hdr);

   /* Check attached LSAs list */
   size = 4;
   entries_num = (A_GET_2B(p_Hdr->Length) - sizeof(t_S_PckHeader)) / 
                     sizeof(t_S_LsReq);
   if(!entries_num)
      return;

   /* Allocate the LS Update packet */
   if((p_LsUpdate = IFO_AllocPacket(p_NBO->IFO_Id, S_LS_UPDATE,
                                    0, &allocPktLen)) == NULL)
      return;

   p_LsaHdr = (t_S_LsaHeader *) (GET_LSA_HDR(p_LsUpdate) + 4);    
   num = 0;
   if (p_IFO->Cfg.AuType == IFO_CRYPT)
   {
     authLen = OSPF_MD5_AUTH_DATA_LEN;
   }

   while(entries_num--)
   {
      /* Find in corresponded ARO Database the match entry */
      if (o2LsaTypeValid(p_LsReq->LsType[3]) &&
          AVLH_Find(GET_OWNER_HL((t_ARO *)p_NBO->ARO_Id, p_LsReq->LsType[3]), 
                    (byte*)&p_LsReq->LsId, (void *)&p_DbEntry,0) == E_OK)      
      {
         ipPktLen = IP_HDR_LEN + authLen + sizeof(t_S_PckHeader) + size +
                    A_GET_2B(p_DbEntry->Lsa.Length);

         if ((num == 0) && (ipPktLen > allocPktLen))
         {
           /* Single LSA is too big for the LSU buffer we have allocated. 
            * Skip this LSA. */
           L7_uchar8 lsIdStr[OSAPI_INET_NTOA_BUF_SIZE];
           L7_uchar8 advRtrStr[OSAPI_INET_NTOA_BUF_SIZE];

           osapiInetNtoa(A_GET_4B(p_LsReq->AdvertisingRouter), advRtrStr);
           osapiInetNtoa(A_GET_4B(p_LsReq->LsId), lsIdStr);
           L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID,
                   "%s with LSID %s from adv router %s too large for LS Update buffer.", 
                   lsaTypeNames[p_LsReq->LsType[3]], lsIdStr, advRtrStr);
           p_LsReq++;
           continue;
         }
         /* When we build UPDATE packets, we should try to avoid fragmentation.
          * So even though we allocate a buffer larger than the IP MTU, we limit
          * the size of updates to the IP MTU, unless a single LSA is larger
          * than the IP MTU. Then we let the stack fragment. */

         if((num >= 1) && ((ipPktLen >= p_IFO->Cfg.MaxIpMTUsize) ||
                           (ipPktLen >= allocPktLen)))
         {
            /* Update packet length */
            B_SetLength(F_GetFirstBuf(p_LsUpdate), size);
            F_GetLength(p_LsUpdate) = size;
            /* Set number of LSAs in LS Update packet */
            A_SET_4B(num, GET_LSA_HDR(p_LsUpdate));

            /* Transmit the LS Update packet */
            IFO_SendPacket(p_NBO->IFO_Id, p_LsUpdate, S_LS_UPDATE, p_NBO, FALSE);   

            /* Allocate the LS Update packet */
            if((p_LsUpdate = IFO_AllocPacket(p_NBO->IFO_Id, S_LS_UPDATE,
                                             0, &allocPktLen)) == NULL)
            {
               return;
            }

            p_LsaHdr = (t_S_LsaHeader *) (GET_LSA_HDR(p_LsUpdate) + 4);    
            size = 4;
            num = 0;
         }
      
         lsaLength = A_GET_2B(p_DbEntry->Lsa.Length);
         ASSERT(lsaLength);
         ASSERT(L7_BIT_ISSET(p_DbEntry->dbFlags, IS_CUR));

         /* Add to LS Update packet */
         memcpy(p_LsaHdr, &p_DbEntry->Lsa, LSA_HEADER_LEN);                  /* header */
         memcpy(p_LsaHdr + 1, p_DbEntry->p_Lsa, lsaLength - LSA_HEADER_LEN); /* body */
            
         
         size += lsaLength;
         num += 1;
         
         if(HL_FindFirst(p_NBO->LsReTx, &p_DbEntry->Lsa.LsType, (void**)&p_TmpRetx) == E_OK)
         {
           NBO_LsReTxUpdateSentTime(p_NBO, p_TmpRetx);
         }

         /* Update LSA age in the data base and then within the packet */
         UpdateAgeOfDbEntry (p_DbEntry);

         A_SET_2B((((t_IFO*)p_NBO->IFO_Id)->Cfg.InfTransDelay + 
                  A_GET_2B(p_DbEntry->Lsa.LsAge)), p_LsaHdr->LsAge);

         if(A_GET_2B(p_LsaHdr->LsAge) > MaxAge)
            A_SET_2B(MaxAge, p_LsaHdr->LsAge);

         p_LsaHdr = (t_S_LsaHeader *)((byte*)p_LsaHdr + lsaLength);
      }         
      else
      {
        if ((p_RTO->ospfTraceFlags & OSPF_TRACE_LS_REQ) &&
            o2LsaTypeValid(p_LsReq->LsType[3]))    /* don't risk bad ptr from lsaTypeNames[] */
        {
          char traceBuf[OSPF_MAX_TRACE_STR_LEN];
          L7_uchar8 nbrId[OSAPI_INET_NTOA_BUF_SIZE];
          L7_uchar8 lsidStr[OSAPI_INET_NTOA_BUF_SIZE];
          L7_uchar8 advRtrStr[OSAPI_INET_NTOA_BUF_SIZE];

          osapiInetNtoa(p_NBO->RouterId, nbrId);
          osapiInetNtoa(A_GET_4B(p_LsReq->LsId), lsidStr);
          osapiInetNtoa(A_GET_4B(p_LsReq->AdvertisingRouter), advRtrStr);
          snprintf(traceBuf, OSPF_MAX_TRACE_STR_LEN,
                   "LS Request from nbr %s for %s with lsid %s adv router %s. "
                   "LSA not found.",
                   nbrId, lsaTypeNames[p_LsReq->LsType[3]], lsidStr, advRtrStr);
          RTO_TraceWrite(traceBuf);    
        } 
         F_Delete(p_LsUpdate);
         DoNboTransition(p_NBO, NBO_BAD_LSREQ, 0);
         return;
      }
                  
      p_LsReq ++;
   }                     

   /* Update packet length */
   B_SetLength(F_GetFirstBuf(p_LsUpdate), size);
   F_GetLength(p_LsUpdate) = size;
   /* Set number of LSAs in LS Update packet */
   A_SET_4B(num, GET_LSA_HDR(p_LsUpdate));

   /* Transmit the LS Update packet */
   IFO_SendPacket(p_NBO->IFO_Id, p_LsUpdate, S_LS_UPDATE, p_NBO, FALSE);   
}

/*********************************************************************
 *
 * @purpose             Update the Checksum field in the Lsa
 *
 * @param p_DbEntry     @b{(input)}  DB entry
 *
 * @returns             void
 *
 * @notes              Because we store the LSA header and LSA body
 *                     separately, we have to compute the checksum
 *                     in a special way here.
 *
 * @end
 * ********************************************************************/
static void RecalcLsaChecksum (t_A_DbEntry *p_DbEntry)
{
   L7_uint32    lsaLength   = A_GET_2B(p_DbEntry->Lsa.Length);
   L7_uint32    offset      = offsetof(t_S_LsaHeader, CheckSum);
   word         c0, c1;
   long         cl0, cl1, ctmp;
   byte         *hpp, *pls;
   int          cs;

   if(! p_DbEntry)
     return;

   A_SET_2B(0, p_DbEntry->Lsa.CheckSum);

   /* start with LSA header (skipping age field) */
   hpp = &p_DbEntry->Lsa.Options;
   pls = hpp + LSA_HEADER_LEN - 2;
   c0 = c1 = 0;
   while(hpp < pls)
   {
      if((c0 += *hpp++) > 254)
         c0 -= 255;
      if((c1 += c0) > 254)
         c1 -= 255;
   }

   /* Now do LSA body */
   hpp = p_DbEntry->p_Lsa;
   pls = hpp + (lsaLength - LSA_HEADER_LEN);
   while(hpp < pls)
   {
      if((c0 += *hpp++) > 254)
         c0 -= 255;
      if((c1 += c0) > 254)
         c1 -= 255;
   }

   ctmp = lsaLength - offset - 1;

   cl0 = c0;
   cl1 = c1;

   if((cs =((ctmp * cl0) - cl1) % 255L) < 0)
      cs += 255;

   p_DbEntry->Lsa.CheckSum[0] = cs;

   if((cs =(cl1 -((ctmp + 1L) * cl0)) % 255L) < 0)
      cs += 255;

   p_DbEntry->Lsa.CheckSum[1] = cs;
}

/*********************************************************************
 * @purpose          Processing of receiving Link State Update packet.
 *
 *
 * @param p_NBO      @b{(input)}  NBO object Id
 * @param p_Hdr      @b{(input)}  packet's header pointer
 *
 * @returns          n/a.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void LsUpdateReceived(t_NBO *p_NBO, t_S_PckHeader *p_Hdr)
{
   t_ARO *p_ARO = (t_ARO *)p_NBO->ARO_Id;
   t_IFO *p_IFO = (t_IFO *)p_NBO->IFO_Id;
   t_RTO *p_RTO = (t_RTO *)p_ARO->RTO_Id;
   t_S_LsaHeader *p_LsaHdr, *p_TmpLsaHdr;
   t_RetxEntry   *p_RetxEntry;
   t_A_DbEntry   *p_DbEntry;
   t_A_DbEntry   *p_TmpEntry;
   t_LsReqEntry  *p_LsReq;
   e_Err e;
   word  acknum;
   ulng  lsanum;
   void  *p_F;
   void  *p_B;
   Bool  FloodedBack;
   Bool  selfOriginatedLsa = FALSE;
   e_DbInstComp compres;
   SP_IPADR router_id = p_RTO->Cfg.RouterId;
   byte lastLsType = S_LAST_LSA_TYPE;
   ulng allocPktLen = 0;
   L7_uint32 lsaLength;
   t_IFO *p_VirtIFO = NULLP;
   e_OSPF_RES_CHECK resCheck;
   byte *pktEnd;    /* One byte beyond end of last LSA */

   if(!p_NBO->OperationState)
      return;
      
   /* Drop the packet if the NBO state less than Exchange */
   if(p_NBO->State < NBO_EXCHANGE)
      return;

   /* Get number of contained LSAs */
   lsanum  = A_GET_4B(GET_PAYLOAD_PTR(p_Hdr));
   acknum = 0;
   
   /* If Update contains multiple LSAs, set bundling flag so that LsaFlooding()
    * will bundle as many LSAs as will fit in an Update. */
   if (lsanum > 1)
   {
       p_RTO->BundleLsas = TRUE;
   }
   
   /* Get pointer to the LsUpdate payload */
   p_LsaHdr =  (t_S_LsaHeader *)(((byte *) GET_PAYLOAD_PTR(p_Hdr)) + 4);

   /* Note that packet may contain authentication data after last LSA */
   pktEnd = ((byte *)p_Hdr) + A_GET_2B(p_Hdr->Length);

   if (p_RTO->Cfg.OpaqueCapability == FALSE)
   {
       /* drop opaque LSAs. */
       lastLsType = S_TMP2_LSA;
   }

   /* Browse all LSAs contained in LS Update packet */
   while (lsanum-- && (((byte*)p_LsaHdr) < pktEnd))
   {
      /* (0) Make sure LSA has a legal length. If not, we can't process any
       * further because we don't know how to find the next LSA. */
     L7_uint32 lsaLen = A_GET_2B(p_LsaHdr->Length);
     byte *lsaEnd = ((byte*) p_LsaHdr) + lsaLen;
     if ((lsaLen < sizeof(t_S_LsaHeader)) || (lsaEnd > pktEnd))
     {
       if (p_RTO->BundleLsas)
       {
         LsUpdatesSend(p_RTO);
       }
       return;
     }

     /* if a single LSA is larger than we can flood in an Ls Update, given
      * the buffer we allocated for Ls Updates, ignore the LSA. */
     if (lsaLen > OSPF_MAX_LSA_LEN)
     {
       goto next_lsa;
     }

      /* (1) Validate the LSA's checksum */
      if(FletcherCheckSum((byte *)p_LsaHdr+2, A_GET_2B(p_LsaHdr->Length)-2, 0) != 0)
      {
        L7_uchar8 lsIdStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(A_GET_4B(p_LsaHdr->LsId), lsIdStr);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID,
                "LSA Checksum error in LsUpdate, dropping LSID %s checksum 0x%x."
                " OSPFv2 ignored a received link state advertisement (LSA) whose "
                "checksum was incorrect.", 
                lsIdStr, A_GET_2B(p_LsaHdr->CheckSum));
        goto next_lsa;
      }

      if (p_RTO->ospfTraceFlags & OSPF_TRACE_LS_UPDATE)
       {
           char traceBuf[OSPF_MAX_TRACE_STR_LEN];
           L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
           L7_uchar8 lsIdStr[OSAPI_INET_NTOA_BUF_SIZE];
           L7_uchar8 advRouterStr[OSAPI_INET_NTOA_BUF_SIZE];
           L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
           osapiInetNtoa((L7_uint32)p_NBO->IpAdr, nbrAddrStr);
           osapiInetNtoa(A_GET_4B(p_LsaHdr->LsId), lsIdStr);
           osapiInetNtoa(A_GET_4B(p_LsaHdr->AdvertisingRouter), advRouterStr);
           osapiInetNtoa(p_ARO->AreaId, areaIdStr);
           sprintf(traceBuf, "OSPF %s received via area %s from neighbor at %s. LSID: %s, Adv Router: %s, Age: %d, SeqNo: 0x%lx",
                   lsaTypeNames[p_LsaHdr->LsType], areaIdStr, nbrAddrStr, lsIdStr, 
                   advRouterStr, 
                   A_GET_2B(p_LsaHdr->LsAge), A_GET_4B(p_LsaHdr->SeqNum));
           RTO_TraceWrite(traceBuf);    
       } 

      /* (2) Check the LSA's type */
      if((p_LsaHdr->LsType < S_ROUTER_LSA) || 
         (p_LsaHdr->LsType == S_TMP2_LSA) ||
         (p_LsaHdr->LsType >= lastLsType))
      {
          /* This should not be necessary. If we do not advertise the O option,
           * neighbor should not send us any opaque LSAs during db exchange. If 
           * he does anyway (and Ixia does), we need to remove the LSA from the
           * request list or else the adj will never go past LOADING. */
          if((p_NBO->State < NBO_FULL) &&
            (HL_FindFirst(p_NBO->LsReq, &p_LsaHdr->LsType, (void *)&p_LsReq) == E_OK))
          {
              HL_Delete(p_NBO->LsReq, &p_LsReq->Lsa.LsType, p_LsReq);
          }
          SendDelayedAck(p_NBO, p_LsaHdr);
          goto next_lsa;
      }

      /* (3) Check the stub/NSSA area condition for AS-EXT LSAs */
      if ((p_ARO->ExternalRoutingCapability != AREA_IMPORT_EXTERNAL) &&
         (((p_LsaHdr->LsType == S_AS_EXTERNAL_LSA) && (A_GET_4B(p_LsaHdr->LsId) != 0)) || 
            (p_LsaHdr->LsType == S_AS_OPAQUE_LSA)))
         goto next_lsa;

      p_DbEntry = NULL;

      /* (4) If LSA's age is MaxAge and no instance of this LSA found in DB */
      if(((e = AVLH_Find(GET_OWNER_HL(p_ARO, p_LsaHdr->LsType), 
            p_LsaHdr->LsId, (void *)&p_DbEntry,0)) != E_OK) &&
         (A_GET_2B(p_LsaHdr->LsAge) == MaxAge)    &&
          (p_ARO->ExchOrLoadState == 0))
      {
         /* send direct acknoweldge to this LSA */      
         SendDirectAck(p_NBO, p_LsaHdr);
         
         goto next_lsa;      
      }

      /* If instance of the received LSA found in DB, update LSA age */
      /* in the data base before comparing                           */
      if(e == E_OK)
      {
      /*--------------------------------------------------------------------*/
      /* When a router's Router ID has changed, any network-LSAs that were  */
      /* originated with the router's previous Router ID must be flushed.   */
      /* RFC 2328, s12.4.2 p134 Network-LSAs                                */
      /*--------------------------------------------------------------------*/
        if(A_GET_4B(p_LsaHdr->AdvertisingRouter) == A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter))
          UpdateAgeOfDbEntry (p_DbEntry);
        else
        {
          e = E_NOT_FOUND;
          p_DbEntry = NULL;
        }
      }

      
      /* (5) if there is no database copy or the receive LSA is more recent */
      if((e != E_OK) || 
         ((compres = LsaCompare(p_LsaHdr, &p_DbEntry->Lsa)) == LSA_MORE_RECENT))
      {
         selfOriginatedLsa = SelfOrigLsaReceived(p_RTO, p_LsaHdr, p_IFO);
         /* No need to enforce MinLSArrival for self-originated LSAs, since
          * self originated LSAs are never installed by flooding. */
         if (!selfOriginatedLsa && p_DbEntry && p_DbEntry->RcvTime &&
            (GET_INTERVAL(p_DbEntry->RcvTime) < MinLSArrival))
         {
            goto next_lsa;   
         }

         FloodedBack = FALSE;

         if (selfOriginatedLsa)
         {
            if (p_DbEntry == NULL) 
            {
               /* The received LSA is self-orig, but should no longer be originated. 
                * If we wanted to originate this LSA, we would already have it installed 
                * in our LSDB. Need to create db entry in order to flush it. */
               resCheck = RTO_LsaResourceCheck(p_RTO, p_DbEntry ? TRUE : FALSE);
               if (resCheck == OSPF_LSDB_FULL)
               {
                 /* LSDB is full. ACK anyway so that neighbor doesn't send it again. 
                  * We'll reestablish adjacency when we come out of overload and get the
                  * LSA at that point. Process rest of LSAs in LS Update so that they 
                  * can be ACK'd. */
                   if ((p_IFO->BackupId != router_id) ||
                       ((p_IFO->BackupId == router_id) &&
                        (p_NBO->RouterId == p_IFO->DrId))) 
                   {
                       SendDelayedAck(p_NBO, p_LsaHdr);
                   }
                   goto next_lsa;   
               }
               else if (resCheck == OSPF_RETX_LIMIT)
               {
                 /* Have exceeded the max number of retx entries. Don't ACK LSA. We want 
                  * neighbor to retransmit. Hopefully we'll have received some ACKs by 
                  * then and deleted some retx entries. No need to process the rest of 
                  * the LSAs in this LS UPDATE. */
                 if (p_RTO->BundleLsas)
                 {
                   LsUpdatesSend(p_RTO);
                 }
                 return;
               }
               /* Should have sufficient resources to store and flood this LSA */

               /* Allocate new LSA, copy LSA's content and install it */
               if((p_B = B_NewEx(NULL, NULL, (word)A_GET_2B(p_LsaHdr->Length))) == NULL)
               {
                  LOG_MSG("Unable to allocate buffer for received OSPF LSA");
                  if (p_RTO->BundleLsas)
                  {
                      LsUpdatesSend(p_RTO);
                  }
                  return;
               }
               if((p_F = F_NewEx(p_B,0,0,0)) == NULL)
               {
                  LOG_MSG("Unable to allocate frame for received OSPF update");
                  if (p_RTO->BundleLsas)
                  {
                      LsUpdatesSend(p_RTO);
                  }
                  return;
               }
               F_AddToEnd(p_F,(byte*)p_LsaHdr,(word)A_GET_2B(p_LsaHdr->Length));         
               p_DbEntry = LsaInstall(p_RTO, p_ARO, p_F, NULLP);
               if(!p_DbEntry)
               {
                  if (p_RTO->BundleLsas)
                  {
                      LsUpdatesSend(p_RTO);
                  }
                  return;
               }

               FlushLsa((t_Handle)p_DbEntry, (ulng)p_ARO, 1);

               goto next_lsa;  
            }
            else
            {
               /* We are still originating this LSA */
               /* Set former instance's seqnum to the received LS sequence number. 
                * Reoriginating the LSA will increment the sequence number one more. */
               A_SET_4B(A_GET_4B(p_LsaHdr->SeqNum), p_DbEntry->Lsa.SeqNum);

               /* Recalculate the checksum of the entry in the db, now that the
                * sequence number is changed */
               UpdateStatCounters(p_RTO, p_ARO, p_DbEntry, FALSE);
               RecalcLsaChecksum(p_DbEntry);
               UpdateStatCounters(p_RTO, p_ARO, p_DbEntry, TRUE);
               if (p_RTO->ospfTraceFlags & OSPF_TRACE_LSA_ORIG)
               {
                  char traceBuf[OSPF_MAX_TRACE_STR_LEN];
                  L7_uchar8 lsIdStr[OSAPI_INET_NTOA_BUF_SIZE];
                  L7_uchar8 advRouterStr[OSAPI_INET_NTOA_BUF_SIZE];
                  L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
                  osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.LsId), lsIdStr);
                  osapiInetNtoa(A_GET_4B(p_DbEntry->Lsa.AdvertisingRouter), advRouterStr);
                  osapiInetNtoa(p_ARO->AreaId, areaIdStr);
                  sprintf(traceBuf, "v2 Recalc checksum of %s on area %s. LSID: %s, Adv Router: %s, Age: %d, SeqNo: 0x%lx: Chcksum: %x",
                         lsaTypeNames[p_DbEntry->Lsa.LsType], areaIdStr, lsIdStr, advRouterStr,
                         A_GET_2B(p_DbEntry->Lsa.LsAge), A_GET_4B(p_DbEntry->Lsa.SeqNum), A_GET_2B(p_DbEntry->Lsa.CheckSum));
                 RTO_TraceWrite(traceBuf);
               }

               /* If doing db exchange with neighbor, remove LSA from neighbor's request list. */
               if (((p_NBO->State == NBO_EXCHANGE) || (p_NBO->State == NBO_LOADING)) &&
                   (HL_FindFirst(p_NBO->LsReq, &p_DbEntry->Lsa.LsType, (void *)&p_LsReq) == E_OK))
               {
                 HL_Delete(p_NBO->LsReq, &p_LsReq->Lsa.LsType, p_LsReq);
               }

               /* LSA origination will get delayed when we re-originate in less 
                * than MinLSInterval. Since the seqno is not bumped till we originate, 
                * avoid uneccesary retx's by removing from the retx list.
               */
               NBO_LsReTxDelete(p_NBO, &p_DbEntry->Lsa.LsType, NULL);
               LsaRefresh(p_RTO, p_ARO, p_DbEntry);
               goto next_lsa;       
            }
         } /* end of processing of self-originated LSA*/             

         /* (c-d) Install this LSA into Area database 
                  and remove former instance */

         /* Check may be router reached overflow state */
         if(p_RTO->IsOverflowed && (e != E_OK) &&
            (((p_LsaHdr->LsType == S_AS_EXTERNAL_LSA) && (A_GET_4B(p_LsaHdr->LsId) != 0) &&
              (p_RTO->NonDefExtLsaCount == (ulng)p_RTO->Cfg.ExtLsdbLimit)) ||
            (p_LsaHdr->LsType == S_AS_OPAQUE_LSA)))
            goto next_lsa;

         resCheck = RTO_LsaResourceCheck(p_RTO, p_DbEntry ? TRUE : FALSE);
         if (resCheck == OSPF_LSDB_FULL)
         {
           /* LSDB is full. ACK anyway so that neighbor doesn't send it again. 
            * We'll reestablish adjacency when we come out of overload and get the
            * LSA at that point. Process rest of LSAs in LS Update so that they 
            * can be ACK'd. */
             if ((p_IFO->BackupId != router_id) ||
                 ((p_IFO->BackupId == router_id) &&
                  (p_NBO->RouterId == p_IFO->DrId))) 
             {
                 SendDelayedAck(p_NBO, p_LsaHdr);
             }
             goto next_lsa;   
         }
         else if (resCheck == OSPF_RETX_LIMIT)
         {
           /* Have exceeded the max number of retx entries. Don't ACK LSA. We want 
            * neighbor to retransmit. Hopefully we'll have received some ACKs by 
            * then and deleted some retx entries. No need to process the rest of 
            * the LSAs in this LS UPDATE. */
           if (p_RTO->BundleLsas)
           {
             LsUpdatesSend(p_RTO);
           }
           return;
         }
         /* Should have sufficient resources to store and flood this LSA */

         /* Allocate temporary frame for new LSA */
         if((p_B = B_NewEx(NULL, NULL, (word)A_GET_2B(p_LsaHdr->Length))) == NULL)
         {
            LOG_MSG("Unable to allocate buffer for received OSPF LSA");
            if (p_RTO->BundleLsas)
            {
               LsUpdatesSend(p_RTO);
            }
            return;
         }
         if((p_F = F_NewEx(p_B,0,0,0)) == NULL)
         {
            LOG_MSG("Unable to allocate frame for received OSPF update");
            if (p_RTO->BundleLsas)
            {
               LsUpdatesSend(p_RTO);
            }
            return;
         }
         F_AddToEnd(p_F, (byte*)p_LsaHdr, (word)A_GET_2B(p_LsaHdr->Length));

         p_TmpEntry = LsaInstall(p_RTO, p_ARO, p_F, p_DbEntry);
         if(!p_TmpEntry)
         {
             /* LSA not installed. Probably a memory allocation failure. 
              * Probably went into stub router mode. ACK anyway to prevent 
              * needless retransmissions. Process
              * rest of LSAs in LS Update so that they can be ACK'd. */
             if ((p_IFO->BackupId != router_id) ||
                 ((p_IFO->BackupId == router_id) &&
                  (p_NBO->RouterId == p_IFO->DrId))) 
             {
                 SendDelayedAck(p_NBO, p_LsaHdr);
             }
             goto next_lsa;   
         }
         else
         {
             if ((A_GET_2B(p_TmpEntry->Lsa.LsAge) >= MaxAge) &&
                 (p_TmpEntry->Lsa.LsType == S_ROUTER_LSA))
             {   
                 ulng remoteRouterId = A_GET_4B(p_Hdr->RouterId);
                 /* Check if a virtual neighbor has been aged out */
                 p_VirtIFO = (t_IFO*) IFO_VirtLinkFind(p_ARO, remoteRouterId);
                 if (p_VirtIFO)
                 {
                     if (p_VirtIFO->Cfg.State != IFO_DOWN)
                     {
                         IFO_Down(p_VirtIFO);
                     }
                 }
             }
           
           p_DbEntry = p_TmpEntry;
         }


         p_DbEntry->RcvTime = TIMER_SysTime();

         p_RTO->Cfg.RxNewLsas ++;
   
         /* For Link Opaque LSA update the source IFO_Id field */
         if(p_DbEntry->Lsa.LsType == S_LINK_OPAQUE_LSA)
         {
            p_DbEntry->IFO_Id = p_IFO;
            ((t_IFO*)p_DbEntry->IFO_Id)->Cfg.LinkLsaCount += 1;
            ((t_IFO*)p_DbEntry->IFO_Id)->Cfg.LinkLsaCksum += A_GET_2B(p_DbEntry->Lsa.CheckSum);
            p_ARO->AreaLsaStats[p_DbEntry->Lsa.LsType] += 1;
         }

         /* Opaque LSA receiving notification */
         if (p_RTO->Clbk.f_OpaqueReceive &&
            (p_DbEntry->Lsa.LsType >= S_LINK_OPAQUE_LSA) &&
            (p_DbEntry->Lsa.LsType <= S_AS_OPAQUE_LSA))
         {
           /* Currently no callback registered, since no application processes
            * opaque LSAs. Note that payload pointer is beyond the LSA header. 
            * If application needs the LSA header, need to modify. */
            p_RTO->Clbk.f_OpaqueReceive(
               /* MngId       */  p_RTO->MngId, 
               /* VPN/COS     */  0L,
               /* SrcRouterId */  A_GET_4B(p_LsaHdr->AdvertisingRouter),
               /* Ls Type     */  p_DbEntry->Lsa.LsType,
               /* Opaque Type */  (byte)(A_GET_4B(p_DbEntry->Lsa.LsId) >> 24),
               /* Opaque Id   */  A_GET_4B(p_DbEntry->Lsa.LsId) & 0x00FFFFFFL,
               /* Payload ptr */  GET_LSA(p_DbEntry->p_Lsa),
               /* Payload len */  (word)(A_GET_2B(p_DbEntry->Lsa.Length) - LSA_HEADER_LEN));
         }
           
         /* (b) Flood this LSA to appropriate IFOs */ 
         LSA_FLOODING(p_RTO, p_ARO, p_NBO, p_DbEntry, &FloodedBack);
         
         /* (e) Possibly send delayed acknowledgment of receipt of LSA */
         /* for definition of Possibly, see 13.5) table 19 rows 1 and 2. */
         if(!FloodedBack && ((p_IFO->BackupId != router_id) ||
                             ((p_IFO->BackupId == router_id) && 
                              (p_NBO->RouterId == p_IFO->DrId))))
         {
           SendDelayedAck(p_NBO, p_LsaHdr);
         }

         goto next_lsa;      
      }
      
      /* (6) if there is an LSA instance on Ls Req List */
      if(HL_FindFirst(p_NBO->LsReq, &p_LsaHdr->LsType, (void *)&p_LsReq) == E_OK)
      {
         DoNboTransition(p_NBO, NBO_BAD_LSREQ, 0);
         if (p_RTO->BundleLsas)
         {
             LsUpdatesSend(p_RTO);
         }
         return;      
      }      
      
      /* (7) if the received LSA is the same instance as the DB copy */
      if(compres == LSA_EQUAL)
      {
         /* (a) if "implied acknowledgment" */
         if(NBO_LsReTxDelete(p_NBO, &p_DbEntry->Lsa.LsType, NULL) == E_OK)
         {
            if((p_IFO->BackupId == router_id) &&
               (p_NBO->RouterId == p_IFO->DrId))
               SendDelayedAck(p_NBO, p_LsaHdr);
            
            acknum++;
         }     
         else
            SendDirectAck(p_NBO, p_LsaHdr);

         goto next_lsa;      
      }

      /* (8) if the data copy is more recent */
      if(compres == LSA_LESS_RECENT)
      {

         if((A_GET_2B(p_DbEntry->Lsa.LsAge) ==  MaxAge) &&
            (A_GET_4B(p_DbEntry->Lsa.SeqNum) ==  MaxSequenceNumber))
         goto next_lsa; /* discard this LSA */

         if(HL_FindFirst(p_NBO->LsReTx, &p_DbEntry->Lsa.LsType, (void**)&p_RetxEntry) != E_OK)
            p_RetxEntry = NULL;

         /* If this instance has not been sent within the last MinLSArrival */
         if((!p_RetxEntry && 
             (GET_INTERVAL(p_DbEntry->SentTime) >= MinLSArrival))  || 
            (p_RetxEntry && 
             (GET_INTERVAL(p_RetxEntry->SentTime) >= MinLSArrival)))
         {
            /* Allocate the LS Update packet */
            if((p_F = IFO_AllocPacket(p_NBO->IFO_Id, S_LS_UPDATE,
                                      0, &allocPktLen)) == NULL)
            {
                if (p_RTO->BundleLsas)
                {
                    LsUpdatesSend(p_RTO);
                }
                return;
            }

            /* Set number of LSAs in LS Update packet */
            A_SET_4B(1, GET_LSA_HDR(p_F));

            p_TmpLsaHdr = (t_S_LsaHeader *) (GET_LSA_HDR(p_F) + 4);    

            /* Copy the LSA into LS Update packet */
            lsaLength = A_GET_2B(p_DbEntry->Lsa.Length);
            memcpy(p_TmpLsaHdr, &p_DbEntry->Lsa, LSA_HEADER_LEN);
            memcpy(p_TmpLsaHdr + 1 , p_DbEntry->p_Lsa, lsaLength - LSA_HEADER_LEN);

            /* Update LSA age within the packet */
            A_SET_2B((p_IFO->Cfg.InfTransDelay + A_GET_2B(p_TmpLsaHdr->LsAge)),
                     p_TmpLsaHdr->LsAge);
            if(A_GET_2B(p_TmpLsaHdr->LsAge) > MaxAge)
               A_SET_2B(MaxAge, p_TmpLsaHdr->LsAge);

            if(p_RetxEntry)
            {
               NBO_LsReTxUpdateSentTime(p_NBO, p_RetxEntry);
               p_RetxEntry->DbEntry = p_DbEntry;
            }
            /* Transmit at most 1 per MinLSArrival LS Update packet 
             * directly to the neighbor, maintain time in lsa for this case */
            p_DbEntry->SentTime = TIMER_SysTime();  

            /* Update packet length */
            B_SetLength(F_GetFirstBuf(p_F), (lsaLength + 4));
            F_GetLength(p_F) = (lsaLength + 4);

            /* Transmit the LS Update packet directly to the neighbor */
            IFO_SendPacket(p_IFO, p_F, S_LS_UPDATE, p_NBO, TRUE);   
         }
      }
next_lsa:      

      /* get next LSA pointer */
      p_LsaHdr = (t_S_LsaHeader *) (((byte *)p_LsaHdr) + 
                                    A_GET_2B(p_LsaHdr->Length));
   }

   if (p_RTO->BundleLsas)
   {
       LsUpdatesSend(p_RTO);
   }
}




/*********************************************************************
 * @purpose           Processing of receiving Link State Ack packet.
 *
 *
 * @param p_NBO      @b{(input)}  NBO object Id
 * @param p_Hdr      @b{(input)}  packet's header pointer
 *
 * @returns          n/a.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void LsAckReceived(t_NBO *p_NBO, t_S_PckHeader *p_Hdr)
{
  t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;
   t_S_LsaHeader *p_LsaHdr;
   t_RetxEntry   *p_RetxEntry;
   word entries_num, acknum;
   e_DbInstComp res;
   
   if(!p_NBO->OperationState || (p_NBO->State < NBO_EXCHANGE))
      return;
   
   /* Get pointer to the LS Ack payload */
   p_LsaHdr = (t_S_LsaHeader *) GET_PAYLOAD_PTR(p_Hdr);
   
   entries_num = (A_GET_2B(p_Hdr->Length) - sizeof(t_S_PckHeader))/ 
      sizeof(t_S_LsaHeader);
   acknum = 0;

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_LS_ACK)
   {
       char traceBuf[OSPF_MAX_TRACE_STR_LEN];
       L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
       L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
       t_ARO *p_ARO = (t_ARO*) p_NBO->ARO_Id;
       osapiInetNtoa((L7_uint32)p_NBO->IpAdr, nbrAddrStr);
       osapiInetNtoa(p_ARO->AreaId, areaIdStr);
       sprintf(traceBuf, "LS ACK received via area %s from neighbor at %s with %u entries.",
               areaIdStr, nbrAddrStr, entries_num);
       RTO_TraceWrite(traceBuf);    
   } 
   
   /* Browse all LSA headers in LsAck packets */
   while(entries_num--)
   {
      res = LSA_EQUAL;
      if(HL_FindFirst(p_NBO->LsReTx, &p_LsaHdr->LsType, (void **)&p_RetxEntry) == E_OK)
      {
         if((! (L7_BIT_ISSET(p_RetxEntry->DbEntry->dbFlags, IS_CUR))) ||
            ((res = LsaCompare(p_LsaHdr, &p_RetxEntry->DbEntry->Lsa)) == LSA_EQUAL))
         {
            /* delete LSA from NBO retx lists */
            NBO_LsReTxDelete(p_NBO, &p_RetxEntry->LsType, p_RetxEntry);
            acknum ++;

            /* reset the retx sent timer to force pending retx's
             * section 13.6: Another packet of retransmissions can be 
             * sent whenever some of the LSAs are acknowledged
             */
            p_NBO->LsReTxSentTime = TIMER_SysTime() - ((((t_IFO *)p_NBO->IFO_Id)->Cfg.RxmtInterval) + 1);
         }
         else
         {
            p_NBO->NbrEvents += 1; /* Unrecognized LsAck received. */
         }
      }
      
      p_LsaHdr ++;
   }
   if (p_RTO->ospfTraceFlags & OSPF_TRACE_LS_ACK)
   {
       char traceBuf[OSPF_MAX_TRACE_STR_LEN];
       sprintf(traceBuf, "Processed %u ACKs.", acknum);
       RTO_TraceWrite(traceBuf);    
   } 
}


/*----------------------------------------------------------------
 *
 *      Neighboring FSM Transition routines
 *
 *----------------------------------------------------------------*/

/*********************************************************************
 * @purpose           Do Neighboring State machine transition according
 *                    to Event and NBO SM state.
 *
 *
 * @param    p_NBO    @b{(input)}  NBO Object handler
 * @param    Event    @b{(input)}  NBO SM event
 *
 * @returns           n/a.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void DoNboTransition(t_NBO *p_NBO, e_NBO_Events Event, void *p_Prm)
{
  t_RTO *p_RTO = (t_RTO *) p_NBO->RTO_Id;
  t_IFO *p_IFO = (t_IFO *) p_NBO->IFO_Id;
  t_ARO *p_ARO = (t_ARO *) p_NBO->ARO_Id;
  word PreviousState = p_NBO->State;         /* adj state before event */
  word nbrState = p_NBO->State;              /* adj state after event */
  SP_IPADR nboAddr   = p_NBO->IpAdr;         /* neighbor's IP address */
  SP_IPADR nbrRouterId = p_NBO->RouterId;    /* neighbor's router ID */
  SP_IPADR router_id = p_RTO->Cfg.RouterId;  /* local router ID */
  p_ARO->IsRtrLsaOrg = FALSE;
  p_ARO->IsNtwLsaOrg = FALSE;

  /* Set p_Prm before call state machine procedure */
  if ((Event == NBO_INACT_TIMER) || (Event == NBO_LL_DOWN ) || 
      (Event == NBO_KILL))
    p_Prm = &p_NBO;

  /* Invoke neighbor state machine */
  ((f_Nbo_tr *)(p_NBO->Nbo_TT))[Event * NUMB_OF_NBO_STATES + p_NBO->State](p_NBO, p_Prm);

  /* State transition could have deleted neighbor object. So look it up again. */
  p_NBO = NULL;
  if (HL_FindFirst(p_IFO->NboHl, (byte*)&nbrRouterId, (void**) &p_NBO) != E_OK)
  {
    /* Neighbor was deleted */
    if (p_RTO->ospfTraceFlags & OSPF_TRACE_NBR_STATE)
    {
      char traceBuf[OSPF_MAX_TRACE_STR_LEN];
      L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      osapiInetNtoa((L7_uint32)nboAddr, nbrAddrStr);
      RTO_InterfaceName(p_IFO, ifName);
      sprintf(traceBuf, "OSPF neighbor %s on %s got event %s in state %s, and was deleted!", 
              nbrAddrStr, ifName, nbrEventNames[Event], 
              nbrStateNames[PreviousState]);
      RTO_TraceWrite(traceBuf);
    }

    nbrState = NBO_DOWN;

    if (PreviousState >= NBO_INIT)
      DoHelloTransition(p_IFO, IFO_NGB_CHANGE);

    if (PreviousState == NBO_FULL)
    {
      if (!p_ARO->IsRtrLsaOrg)
        /* Reoriginate the router LSA */
        LsaReOriginate[S_ROUTER_LSA](p_ARO, 0);


      /*----------------------------------------------------------------------------------*/
      /* Note: This code was changed for defect# 3289 (ANVL OSPF 27.20).  When a virtual  */
      /* neighbor goes down we need to re-originate the router lsa BOTH in the backbone   */
      /* & transit areas.                                                                 */
      /*----------------------------------------------------------------------------------*/
      /* if it is the Virtual neighbor then reoriginate the router LSA in transit area */
      if ((p_IFO->Cfg.Type == IFO_VRTL) && p_IFO->TransitARO)
        LsaReOriginate[S_ROUTER_LSA](p_IFO->TransitARO, 0);


      /* If local router is DR for this network */
      if (!p_ARO->IsNtwLsaOrg && (p_IFO->DrId == router_id))
        /* Reoriginate the network LSA */
        LsaReOriginate[S_NETWORK_LSA](p_ARO, (ulng)p_IFO);
    }
  }
  else
  {
    /* Neighbor not deleted */
    nbrState = p_NBO->State;

    if (nbrState != PreviousState)
    {
      if (p_RTO->ospfTraceFlags & OSPF_TRACE_NBR_STATE)
      {
        if (PreviousState != p_NBO->State)
        {
          char traceBuf[OSPF_MAX_TRACE_STR_LEN];
          L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          osapiInetNtoa((L7_uint32)nboAddr, nbrAddrStr);
          RTO_InterfaceName(p_IFO, ifName);
          sprintf(traceBuf, "OSPF neighbor %s on %s moved from state %s to state %s, on event %s", 
                  nbrAddrStr, ifName, nbrStateNames[PreviousState], 
                  nbrStateNames[nbrState],
                  nbrEventNames[Event]);
          RTO_TraceWrite(traceBuf);
        }
      } 

      p_NBO->NbrEvents += 1;  
      p_ARO->IsRtrLsaOrg = FALSE;
      p_ARO->IsNtwLsaOrg = FALSE;

      /* If bidirectional connection established or loss
         run NGB CHANGE IFO event */
      if (((PreviousState < NBO_2WAY) && (nbrState >= NBO_2WAY)) ||
          ((PreviousState >= NBO_2WAY) && (nbrState < NBO_2WAY)))
      {
        DoHelloTransition(p_IFO, IFO_NGB_CHANGE);
      }

      /* if the NBO changed it's state to/from FULL */
      if ((nbrState == NBO_FULL) || (PreviousState == NBO_FULL))
      {
        /* Reoriginate the router LSA */             
        if (!p_ARO->IsRtrLsaOrg)
          LsaReOriginate[S_ROUTER_LSA](p_ARO, 0);

        /* If local router is DR for this network */
        if (!p_ARO->IsNtwLsaOrg && (p_IFO->DrId == router_id))
          /* Reoriginate the network LSA */
          LsaReOriginate[S_NETWORK_LSA](p_ARO, (ulng)p_IFO);

        /* if it is the Virtual neighbor then reoriginate the router LSA in transit area */
        if ((p_IFO->Cfg.Type == IFO_VRTL) && p_IFO->TransitARO)
          LsaReOriginate[S_ROUTER_LSA](p_IFO->TransitARO, 0);

#if L7_OSPF_TE
        /* Reoriginate TE Link LSA */
        if (p_ARO->TEsupport)
          TE_LinkInfoUpdate(p_IFO, p_IFO->TeCosMask, p_IFO->TePdr, 1);
#endif
      }
    }
  }

  if (nbrState != PreviousState)
  {
    /* generate OSPF traps events callback */       
    t_NbrStateChange NgbChg;                        
    t_VirtNbrStateChange VirtNgbChg;   

    if (p_RTO->Clbk.p_OspfTrap)
    {
      if ((p_IFO->Cfg.Type == IFO_VRTL) &&          
          (p_RTO->TrapControl & GET_TRAP_BITMASK(VIRT_NBR_STATE_CHANGE)))
      {
        VirtNgbChg.RouterId = router_id;     
        VirtNgbChg.VirtNbrArea = p_IFO->Cfg.VirtTransitAreaId;
        VirtNgbChg.VirtNbrRtrId = nbrRouterId; 
        VirtNgbChg.VirtNbrState = nbrState + 1;
        VirtNgbChg.Event = Event;

        p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, VIRT_NBR_STATE_CHANGE, (u_OspfTrap*)&VirtNgbChg);
      }
      else if (p_RTO->TrapControl & GET_TRAP_BITMASK(NBR_STATE_CHANGE))
      {
        NgbChg.RouterId = router_id;        
        NgbChg.NbrIpAddr = nboAddr;          
        NgbChg.NbrAddressLessIndex = p_IFO->Cfg.IfIndex; 
        NgbChg.NbrRtrId = nbrRouterId;        
        NgbChg.NbrState = nbrState + 1;       
        NgbChg.Event = Event;
        p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, NBR_STATE_CHANGE, (u_OspfTrap*)&NgbChg);
      }
    }
  }
}

/* --------------------------------------------------------------- */
/* --------------------------------------------------------------- */
/* ---- internal routines ---------------------------------------- */
/* --------------------------------------------------------------- */
/* --------------------------------------------------------------- */

static void NbEr(t_NBO *p_NBO, void *p_Prm)
{

}
static void Nbo0(t_NBO *p_NBO, void *p_Prm)
{
}

/* State: NBO_DOWN - Event: NBO_HLO_RX. */
/* New state NBO_INIT                       */ 
static void Nbo1(t_NBO *p_NBO, void *p_Prm)
{
   t_S_Hello *p_Hlo = (t_S_Hello *) p_Prm;
   SP_IPADR  LocRouterId = ((t_RTO*)p_NBO->RTO_Id)->Cfg.RouterId;
   t_IFO     *p_IFO = (t_IFO *) p_NBO->IFO_Id;
   t_NBO     *p_TmpNbo;
   e_Err     e;

   p_NBO->State = NBO_INIT;
   
   /* Update NBO's ExchOrLoadState flag and ARO's ExchOrLoadState counter */
   if(p_NBO->ExchOrLoadState)
   {
      ((t_ARO *) p_NBO->ARO_Id)->ExchOrLoadState -= 1;
      p_NBO->ExchOrLoadState = FALSE;
   }
   
   p_NBO->RouterPriority = p_Hlo->RouterPriority;
   p_NBO->InactivityInterval = A_GET_4B(p_Hlo->RouterDeadInterval);
   p_NBO->Options = p_Hlo->Options;

   p_NBO->DrId = 0;
   p_NBO->BackupId = 0;
   p_NBO->DrIpAddr = 0;
   p_NBO->BackupIpAddr = 0;

   if(p_IFO->Cfg.IpAdr == A_GET_4B(p_Hlo->DesignatedRouter))
   {
      p_NBO->DrId     = LocRouterId;
      p_NBO->DrIpAddr = p_IFO->Cfg.IpAdr;
   }

   if(p_IFO->Cfg.IpAdr == A_GET_4B(p_Hlo->BackupRouter))
   {
      p_NBO->BackupId     = LocRouterId;
      p_NBO->BackupIpAddr = p_IFO->Cfg.IpAdr;
   }
   
   if(!p_NBO->DrId || 
      (!p_NBO->BackupId && A_GET_4B(p_Hlo->BackupRouter)))
   {
      e = HL_GetFirst(p_IFO->NboHl, (void *)&p_TmpNbo);
      while(e == E_OK)
      {
         if(p_TmpNbo->IpAdr == A_GET_4B(p_Hlo->DesignatedRouter))
         {
            p_NBO->DrId     = p_TmpNbo->RouterId;
            p_NBO->DrIpAddr = p_TmpNbo->IpAdr;
         }

         if(p_TmpNbo->IpAdr == A_GET_4B(p_Hlo->BackupRouter))
         {
            p_NBO->BackupId     = p_TmpNbo->RouterId;
            p_NBO->BackupIpAddr = p_TmpNbo->IpAdr;
         }

         if(p_NBO->DrId && p_NBO->BackupId)
            break;      

         e = HL_GetNext(p_IFO->NboHl, (void *)&p_TmpNbo, p_TmpNbo);
      }
   }

   if(A_GET_4B(p_Hlo->BackupRouter) == 0)
      p_NBO->BackupId = p_NBO->BackupIpAddr = 0;

   /* Restart Neighboring timer */
   TIMER_StartSec(p_NBO->InactivityTimer, p_NBO->InactivityInterval,
                  0, InactivityTimerExp, p_NBO->OspfSysLabel.threadHndle );


/*--------------------------------------------------------------------------*/
/* Note: We do NOT need to force a hello immediately when transitioning     */
/* from Neighbor state down on a HelloReceived event as per rfc 2328. This  */
/* change was made to fix defect LVL00007606, ANVL OSPF test 15.12          */
/*--------------------------------------------------------------------------*/
   /* Force immediatlly 2-WAY Hello instead waiting HelloInterval */

}

/* State: NBO_INIT - event: 2WAY_RX. State NBO_2WAY Event: NBO_ADJOK. */
/* New state: NBO_EXSTART or NBO_2WAY                                 */
static void Nbo2(t_NBO *p_NBO, void *p_Prm)
{
   /* Check if adjacency should be established */
   if(CheckAdj(p_NBO))
   {
      p_NBO->State = NBO_EXSTART;      
   
      /* Update NBO's ExchOrLoadState flag and ARO's ExchOrLoadState counter */
      if(p_NBO->ExchOrLoadState)
      {
         ((t_ARO *) p_NBO->ARO_Id)->ExchOrLoadState -= 1;
         p_NBO->ExchOrLoadState = FALSE;
      }

      p_NBO->DdSeqNum += 1;
      p_NBO->IsMaster = 1;

      /* Start DD Retransmission timer */
      TIMER_StartSec(p_NBO->DdRxmtTimer, TimerFractVar(p_NBO->RxmtInterval),
                     0, DdRxmtTimerExp, p_NBO->OspfSysLabel.threadHndle );
                     
      /* Sent first DD packet */                     
      SendDd(p_NBO, 1);
   }
   else
   {
      p_NBO->State = NBO_2WAY;      
   
      /* Update NBO's ExchOrLoadState flag and ARO's ExchOrLoadState counter */
      if(p_NBO->ExchOrLoadState)
      {
         ((t_ARO *) p_NBO->ARO_Id)->ExchOrLoadState -= 1;
         p_NBO->ExchOrLoadState = FALSE;
      }
   }      
}

/* State: NBO_EXSTART - event: NBO_NEG_DONE */
/* New state: NBO_EXCHANGE                  */
static void Nbo3(t_NBO *p_NBO, void *p_Prm)
{
   e_Err e;
   t_A_DbEntry *p_DbEntry;
   t_RetxEntry *p_RetxEntry;
   byte LsaType, LastType;
   t_RTO *p_RTO = (t_RTO *) p_NBO->RTO_Id;
   t_IFO *p_IFO = (t_IFO *) p_NBO->IFO_Id;
   t_ARO *p_ARO = (t_ARO *) p_NBO->ARO_Id;
   
   p_NBO->State = NBO_EXCHANGE;      

   /* Stop DD Retransmission timer */
   TIMER_Stop(p_NBO->DdRxmtTimer);

   /* Update NBO's ExchOrLoadState flag and ARO's ExchOrLoadState counter */
   if(!p_NBO->ExchOrLoadState)
   {
      ((t_ARO *) p_NBO->ARO_Id)->ExchOrLoadState += 1;
      p_NBO->ExchOrLoadState = TRUE;
   }

   /* Browse content of this Area Database and add match LSAs into Db summary list */
   
   if(p_RTO->Cfg.OpaqueCapability)
      LastType = S_LAST_LSA_TYPE;
   else
      LastType = S_TMP2_LSA;

   for(LsaType = S_ROUTER_LSA; LsaType < LastType; LsaType++)
   {
      if(LsaType == S_TMP2_LSA)
         continue;

      /* Do not include Group Member Lsas into DbSum List for Neighbors which
         does not support multicasting */
      if((LsaType == S_GROUP_MEMBER_LSA) && ((p_NBO->Options & OSPF_OPT_MC_BIT) == 0))
         continue;

      /* Do not include Opaque Lsas into DbSum List for Neighbors which
         does not support Opaque option */
      if((LsaType >= S_LINK_OPAQUE_LSA) && (LsaType <= S_AS_OPAQUE_LSA)  &&
          ((p_NBO->Options & OSPF_OPT_O_BIT) == 0))
         continue;

      e = AVLH_GetFirst(GET_OWNER_HL(p_NBO->ARO_Id, LsaType), (void *)&p_DbEntry);
      while(e == E_OK)
      {
         if(!(
             (((p_IFO->Cfg.Type == IFO_VRTL) || 
             (p_ARO->ExternalRoutingCapability != AREA_IMPORT_EXTERNAL)) &&
             ((LsaType == S_AS_EXTERNAL_LSA)||(LsaType == S_AS_OPAQUE_LSA)))
                                          ||
             /* don't import T4 summaries into NSSAs */
             ((p_ARO->ExternalRoutingCapability == AREA_IMPORT_NSSA) &&
              (LsaType == S_ASBR_SUMMARY_LSA))
                                          ||
            ((p_ARO->ExternalRoutingCapability != AREA_IMPORT_EXTERNAL) &&
            ((LsaType == S_IPNET_SUMMARY_LSA) || (LsaType == S_ASBR_SUMMARY_LSA)) &&
             (p_ARO->ImportSummaries == FALSE) && (A_GET_4B(p_DbEntry->Lsa.LsId)!=0)) 
                                          ||
            ((LsaType == S_LINK_OPAQUE_LSA) && (p_NBO->IFO_Id != p_DbEntry->IFO_Id))))
         {
            if(A_GET_2B(p_DbEntry->Lsa.LsAge) < MaxAge)
               HL_Insert(p_NBO->DbSum, &p_DbEntry->Lsa.LsType, p_DbEntry);
            else       
            {
               /* If Retx List still has this entry, then delete this entry first */
               if(HL_FindFirst(p_NBO->LsReTx, &p_DbEntry->Lsa.LsType, (void**)&p_RetxEntry) != E_OK)
               {            
                  /* Allocate the new Retx entry and insert it into Retx list */
                  if ((p_RetxEntry = RTO_RetxEntryAlloc(p_RTO)) == NULL)
                  {
                    /* Can't sync db with neighbor. Kill the neighbor. Will naturally retry. */
                    Nbo8(p_NBO, &p_NBO);
                    return;
                  }
                  p_RetxEntry->DbEntry = p_DbEntry;
                  memcpy(&p_RetxEntry->LsType, &p_DbEntry->Lsa.LsType, 9);
                  p_RetxEntry->SentTime = TIMER_SysTime();
                  NBO_LsReTxInsert(p_NBO, &p_RetxEntry->LsType, p_RetxEntry);
               }
               else
               {
                  p_RetxEntry->DbEntry = p_DbEntry;
               }
            }
         }
         e = AVLH_GetNext(GET_OWNER_HL(p_NBO->ARO_Id, LsaType), (void *)&p_DbEntry, p_DbEntry);
      }
   }

   /* Set first outstanding DD list entry */
   if(!p_NBO->p_OutDdEntry)
      HL_GetFirst(p_NBO->DbSum, &p_NBO->p_OutDdEntry);
}

/* State: NBO_EXCHANGE, event: NBO_EXC_DONE  */
/* New state: NBO_LOADING  or NBO_FULL       */
static void Nbo4(t_NBO *p_NBO, void *p_Prm)
{
   word lsrqs;

   /* Stop DD Retransmission timer */
   TIMER_Stop(p_NBO->DdRxmtTimer);

   /* Free last DD packet copy */
   if(p_NBO->IsMaster)
   {
      if(p_NBO->p_LastDd && F_GetLength(p_NBO->p_LastDd))
      {
         F_Delete(p_NBO->p_LastDd);
         p_NBO->p_LastDd = NULL;
      }

      if(p_NBO->InitDDSent)
      {
         A_SET_4B(A_GET_4B(p_NBO->LastRxDd.SeqNum)+1, p_NBO->LastRxDd.SeqNum);
         p_NBO->DdSeqNum += 1;
         SendDd(p_NBO, 0);
      }
   }
   else
     /* Start Last DD interval for slave */
     TIMER_StartSec(p_NBO->SlaveLastDdTimer, p_NBO->InactivityInterval,
                   0, SlaveLastDdTimerExp, p_NBO->OspfSysLabel.threadHndle );

   HL_GetEntriesNmb(p_NBO->LsReq, &lsrqs);
   if(lsrqs)
   {
      p_NBO->State = NBO_LOADING;   
   
      /* Update NBO's ExchOrLoadState flag and ARO's ExchOrLoadState counter */
      if(!p_NBO->ExchOrLoadState)
      {
         ((t_ARO *) p_NBO->ARO_Id)->ExchOrLoadState += 1;
         p_NBO->ExchOrLoadState = TRUE;
      }

      /* if LS Request list is not empty, then 
         send the LS Request packet and 
         Start LS Request Retransmission timer */
      if(!TIMER_Active(p_NBO->RqRxmtTimer))
         RqRxmtTimerExp(p_NBO,0,0);
   }
   else
   {
      p_NBO->State = NBO_FULL;   
      p_NBO->Uptime = TIMER_SysTime();

      /* Update NBO's ExchOrLoadState flag and ARO's ExchOrLoadState counter */
      if(p_NBO->ExchOrLoadState)
      {
         ((t_ARO *) p_NBO->ARO_Id)->ExchOrLoadState -= 1;
         p_NBO->ExchOrLoadState = FALSE;
      }
      HL_Cleanup(p_NBO->DbSum,0);
   }
}

/* State: NBO_DOWN  , event:  NBO_START  */
/* New state:  NBO_ATTEMPT               */
static void Nbo5(t_NBO *p_NBO, void *p_Prm)
{
   p_NBO->State = NBO_ATTEMPT;

   /* Update NBO's ExchOrLoadState flag and ARO's ExchOrLoadState counter */
   if(p_NBO->ExchOrLoadState)
   {
      ((t_ARO *) p_NBO->ARO_Id)->ExchOrLoadState -= 1;
      p_NBO->ExchOrLoadState = FALSE;
   }
   
   /* Start Neighboring timer */
   TIMER_StartSec(p_NBO->InactivityTimer, p_NBO->InactivityInterval,
                  0, InactivityTimerExp, p_NBO->OspfSysLabel.threadHndle );

   IFO_SendHello(p_NBO->IFO_Id, 0, 0);
}

/* State: >= NBO_EXSTART, event: NBO_ADJOK      */
/* New state:  NBO_2WAY or the same as a state  */
static void Nbo6(t_NBO *p_NBO, void *p_Prm)
{
   if(!CheckAdj(p_NBO))
   {
      p_NBO->State = NBO_2WAY;      

      if (TIMER_Active(p_NBO->SlaveLastDdTimer))
      {
        TIMER_Stop(p_NBO->SlaveLastDdTimer);
      }

      /* Stop DD Retransmission timer */
      TIMER_Stop(p_NBO->DdRxmtTimer);

      /* Update NBO's ExchOrLoadState flag and ARO's ExchOrLoadState counter */
      if(p_NBO->ExchOrLoadState)
      {
         ((t_ARO *) p_NBO->ARO_Id)->ExchOrLoadState -= 1;
         p_NBO->ExchOrLoadState = FALSE;
      }
   
      ClearLsReTx(p_NBO);
      HL_Cleanup(p_NBO->DbSum,0);
      HL_Cleanup(p_NBO->LsReq,1);
      p_NBO->p_OutDdEntry = 0;
   }
}

/* State:  NBO_EXCHANGE, _LOADING, _FULL,  */
/* event: NBO_BAD_LSREQ or NBO_SEQNUM_MISM */
/* New state:  NBO_EXSTART                 */
static void Nbo7(t_NBO *p_NBO, void *p_Prm)
{
   p_NBO->State = NBO_EXSTART;      
 
   /* Update NBO's ExchOrLoadState flag and ARO's ExchOrLoadState counter */
   if(p_NBO->ExchOrLoadState)
   {
      ((t_ARO *) p_NBO->ARO_Id)->ExchOrLoadState -= 1;
      p_NBO->ExchOrLoadState = FALSE;
   }

   ClearLsReTx(p_NBO);
   HL_Cleanup(p_NBO->DbSum,0);
   HL_Cleanup(p_NBO->LsReq,1);
 
   p_NBO->DdSeqNum += 1;
   p_NBO->IsMaster = 1;
   p_NBO->p_OutDdEntry = 0;

   if (TIMER_Active(p_NBO->SlaveLastDdTimer))
   {
     TIMER_Stop(p_NBO->SlaveLastDdTimer);
   }

   /* Start DD Retransmission timer */
   TIMER_StartSec(p_NBO->DdRxmtTimer, TimerFractVar(p_NBO->RxmtInterval),
                  0, DdRxmtTimerExp, p_NBO->OspfSysLabel.threadHndle );
                  
   /* Send first DD packet */                     
   SendDd(p_NBO, 1);
}

/* State: any state , event:  NBO_KILL, NBO_INACT_TIMER or NBO_LL_DOWN  */
/* New state:   NBO_DOWN                                                */
/* p_Prm contains pointer to p_NBO. If NBO is deleted */
/* p_NBO is set to NULLP.                             */
static void Nbo8(t_NBO *p_NBO, void *p_Prm)
{
   t_NBO **p_tmp = (t_NBO**)p_Prm;
   if(((t_IFO *) p_NBO->IFO_Id)->Cfg.Type != IFO_NBMA)
   {
      NBO_Delete(p_NBO, 1);
      *p_tmp = NULLP;
   }
   else
   {
      p_NBO->State = NBO_DOWN;
 
      /* Update NBO's ExchOrLoadState flag and ARO's ExchOrLoadState counter */
      if(p_NBO->ExchOrLoadState)
      {
         ((t_ARO *) p_NBO->ARO_Id)->ExchOrLoadState -= 1;
         p_NBO->ExchOrLoadState = FALSE;
      }

      ClearLsReTx(p_NBO);
      HL_Cleanup(p_NBO->DbSum,0);
      HL_Cleanup(p_NBO->LsReq,1);
      p_NBO->p_OutDdEntry = 0;
            
      /* Stop DD Retransmission timer */
      TIMER_Stop(p_NBO->DdRxmtTimer);

      TIMER_Stop(p_NBO->InactivityTimer);

      if (TIMER_Active(p_NBO->SlaveLastDdTimer))
      {
        TIMER_Stop(p_NBO->SlaveLastDdTimer);
      }
   }      
}

/* State:  >= NBO_INIT  , event: NBO_HLO_RX  */
/* New state:  no state change               */
static void Nbo9(t_NBO *p_NBO, void *p_Prm)
{
   t_S_Hello *p_Hlo = (t_S_Hello *) p_Prm;
   SP_IPADR  LocRouterId = ((t_RTO*)p_NBO->RTO_Id)->Cfg.RouterId;
   t_IFO   *p_IFO = (t_IFO *) p_NBO->IFO_Id;
   t_NBO *p_TmpNbo;
   e_Err e;

   p_NBO->RouterPriority = p_Hlo->RouterPriority;
   p_NBO->InactivityInterval =  A_GET_4B(p_Hlo->RouterDeadInterval);


   p_NBO->DrId = 0;
   p_NBO->BackupId = 0;

   if(p_IFO->Cfg.IpAdr == A_GET_4B(p_Hlo->DesignatedRouter))
   {
      p_NBO->DrId     = LocRouterId;
      p_NBO->DrIpAddr = p_IFO->Cfg.IpAdr;
   }

   if(p_IFO->Cfg.IpAdr == A_GET_4B(p_Hlo->BackupRouter))
   {
      p_NBO->BackupId     = LocRouterId;
      p_NBO->BackupIpAddr = p_IFO->Cfg.IpAdr;
   }
   
   if(!p_NBO->DrId || 
      (!p_NBO->BackupId && A_GET_4B(p_Hlo->BackupRouter)))
   {
      e = HL_GetFirst(p_IFO->NboHl, (void *)&p_TmpNbo);
      while(e == E_OK)
      {
         if(p_TmpNbo->IpAdr == A_GET_4B(p_Hlo->DesignatedRouter))
         {
            p_NBO->DrId     = p_TmpNbo->RouterId;
            p_NBO->DrIpAddr = p_TmpNbo->IpAdr;
         }

         if(p_TmpNbo->IpAdr == A_GET_4B(p_Hlo->BackupRouter))
         {
            p_NBO->BackupId     = p_TmpNbo->RouterId;
            p_NBO->BackupIpAddr = p_TmpNbo->IpAdr;
         }

         if(p_NBO->DrId && p_NBO->BackupId)
            break;      

         e = HL_GetNext(p_IFO->NboHl, (void *)&p_TmpNbo, p_TmpNbo);
      }
   }

   if(A_GET_4B(p_Hlo->BackupRouter) == 0)
      p_NBO->BackupId = p_NBO->BackupIpAddr = 0;

   /* Restart Neighboring timer */
   TIMER_StartSec(p_NBO->InactivityTimer, p_NBO->InactivityInterval,
                  0, InactivityTimerExp, p_NBO->OspfSysLabel.threadHndle );
}

/* State: NBO_LOADING, event: NBO_LOAD_DONE   */
/* New state:  NBO_FULL                       */
static void Nbo10(t_NBO *p_NBO, void *p_Prm)
{

   p_NBO->State = NBO_FULL;   
   p_NBO->Uptime = TIMER_SysTime();
 
   /* Update NBO's ExchOrLoadState flag and ARO's ExchOrLoadState counter */
   if(p_NBO->ExchOrLoadState)
   {
      ((t_ARO *) p_NBO->ARO_Id)->ExchOrLoadState -= 1;
      p_NBO->ExchOrLoadState = FALSE;
   }
   HL_Cleanup(p_NBO->DbSum,0);
}

/* State: > NBO_2WAY  , event: NBO_1WAY_RX  */
/* New state:  NBO_INIT                     */
static void Nbo11(t_NBO *p_NBO, void *p_Prm)
{
   p_NBO->State = NBO_INIT;   

   /* Update NBO's ExchOrLoadState flag and ARO's ExchOrLoadState counter */
   if(p_NBO->ExchOrLoadState)
   {
      ((t_ARO *) p_NBO->ARO_Id)->ExchOrLoadState -= 1;
      p_NBO->ExchOrLoadState = FALSE;
   }
   
   if (TIMER_Active(p_NBO->SlaveLastDdTimer))
   {
     TIMER_Stop(p_NBO->SlaveLastDdTimer);
   }
            
   /* Stop DD Retransmission timer */
   TIMER_Stop(p_NBO->DdRxmtTimer);

   ClearLsReTx(p_NBO);
   HL_Cleanup(p_NBO->DbSum,0);
   HL_Cleanup(p_NBO->LsReq,1);
   p_NBO->p_OutDdEntry = 0;
}


/*********************************************************************
 * @purpose          Send direct Link State Ack packet.
 *
 *
 * @param p_NBO      @b{(input)}  NBO object Id
 * @param p_Hdr      @b{(input)}  LSA header pointer
 *
 * @returns          n/a.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void SendDirectAck(t_NBO *p_NBO, t_S_LsaHeader *p_Hdr)
{
   void *p_F;
   ulng allocPktLen;

   if(!p_NBO->OperationState)
      return;
   
   /* Allocate LS ACK */
   if((p_F = IFO_AllocPacket(p_NBO->IFO_Id, S_LS_ACK, 0, &allocPktLen)) == NULL)
      return;

   /* copy current LSA's header content into LS ACK packet */
   F_AddToEnd(p_F, (byte*)p_Hdr, sizeof(t_S_LsaHeader));

   /* Transmit the LS ACK packet */
   IFO_SendPacket(p_NBO->IFO_Id, p_F, S_LS_ACK, p_NBO, FALSE);   
}



/*********************************************************************
 * @purpose         Send delayed Link State Ack packet.
 *
 *
 * @param p_NBO      @b{(input)}  NBO object Id
 * @param p_Hdrb     @b{(input)}  LSA header pointer
 *
 * @returns          n/a.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void SendDelayedAck(t_NBO *p_NBO, t_S_LsaHeader *p_Hdr)
{
   t_AckElement  *p_Ack, *p_El;
   t_IFO *p_IFO = (t_IFO*) p_NBO->IFO_Id;
   Bool served = FALSE;
   
   if(!p_NBO->OperationState)
      return;

   /* Allocate Ack list element */
   if((p_Ack = XX_Malloc(sizeof(t_AckElement))) == NULL)
      return;
   memset(p_Ack, 0, sizeof(t_AckElement));
   
   /* copy LSA header info */
   memcpy(&p_Ack->LsaHdr, p_Hdr, sizeof(t_S_LsaHeader));    
   p_Ack->NBO_Id = p_NBO;
   
   /* Put into IFO's LsAck list grouped on base NBO relation */               
   LL_Rewind(((t_IFO*)p_NBO->IFO_Id)->LsAck);
   while((p_El = LL_Read(((t_IFO*)p_NBO->IFO_Id)->LsAck)) != NULL)
   {
      if(p_El->NBO_Id == p_Ack->NBO_Id)
      {
         /* if found match, insert after the found element */
         LL_Insert(((t_IFO*)p_NBO->IFO_Id)->LsAck, p_El, p_Ack);
         served = TRUE;
         break;
      }
      LL_Inc(((t_IFO*)p_NBO->IFO_Id)->LsAck);
   }
   
   /* if not found match, add to the end of the list */
   if(!served)
      LL_Put(((t_IFO*)p_NBO->IFO_Id)->LsAck, p_Ack);         

   /* start Ack timer if was not active */
   if(!TIMER_Active(p_IFO->AckTimer))
      TIMER_StartSecMQueue(p_IFO->AckTimer, p_IFO->Cfg.AckInterval, 0,
       ((p_IFO->Cfg.Type == IFO_PTM) ||
        (p_IFO->Cfg.Type == IFO_VRTL)||
        (p_IFO->Cfg.Type == IFO_NBMA)) ? 
        UnicastAckTimerExp : BroadcastAckTimerExp,
        ((t_RTO*)p_IFO->RTO_Id)->OspfSysLabel.threadHndle,
        OSPF_ACK_QUEUE );
}


/*********************************************************************
 * @purpose          Check neighbors' adjancency state.
 *
 *
 * @param p_NBO      @b{(input)}  NBO object Id
 *
 * @returns          TRUE - is adjanced,
 * @returns          FALSE - otherwise.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static Bool CheckAdj(t_NBO *p_NBO)
{
   t_IFO *p_IFO = (t_IFO *) p_NBO->IFO_Id;
   t_RTO *p_RTO = (t_RTO *) p_NBO->RTO_Id;
   e_IFO_Types type = p_IFO->Cfg.Type;
   SP_IPADR LocRouterId = p_RTO->Cfg.RouterId;
   Bool AdjOk;
      
   /* Check if adjacency should be established */
   if((type == IFO_PTP) ||
      (type == IFO_PTM) ||
      (type == IFO_VRTL) ||
      (p_IFO->DrId == LocRouterId) ||
      (p_IFO->BackupId == LocRouterId) ||
      (p_IFO->DrId == p_NBO->RouterId) ||
      (p_IFO->BackupId == p_NBO->RouterId))
      AdjOk = TRUE;
   else
      AdjOk = FALSE;

   return AdjOk;                  
}




/*********************************************************************
 * @purpose              Send Database Description packet.
 *
 *
 * @param p_NBO          @b{(input)}  NBO object Id
 * @param IsInit         @b{(input)}  Sent first DD packet flag
 *
 * @returns   n/a.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void SendDd(t_NBO *p_NBO, Bool IsInit)
{
   t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;
   t_S_DbDscr DbDscr;
   t_S_DbDscr *p_DbDscr;
   t_A_DbEntry *p_DbEntry;
   e_Err e=E_FAILED;
   void *p_F;
   ulng allocPktLen;
   word authLen = 0;

   /* IP MTU on interface to neighbor */
   t_IFO *ifcToNeighbor = (t_IFO*)p_NBO->IFO_Id;

   /* This is the MTU to be advertised to the neighbor. */
   word ipMtu = ifcToNeighbor->Cfg.MaxIpMTUsize;    

   if(!p_NBO->OperationState)
      return;
   
   /* Reallocate the new Database Descriptor packet */
   if(p_NBO->p_LastDd)
   {
      F_Delete(p_NBO->p_LastDd);
      p_NBO->p_LastDd = 0;
   }
   
   if((p_F = IFO_AllocPacket(ifcToNeighbor, S_DB_DESCR, 0, &allocPktLen)) == NULL)
      return;
      
   /* Fill DD header */
   A_SET_2B(((ifcToNeighbor->Cfg.Type != IFO_VRTL) ? ipMtu : 0), 
            DbDscr.InterfaceMtu);

   DbDscr.Options = ((t_ARO *)p_NBO->ARO_Id)->ExternalRoutingCapability == 
                     AREA_IMPORT_EXTERNAL ? OSPF_OPT_E_BIT : 0;
   DbDscr.Options |= ((t_RTO *)p_NBO->RTO_Id)->Cfg.MulticastExtensions ? OSPF_OPT_MC_BIT : 0;
   DbDscr.Options |= ((t_RTO *)p_NBO->RTO_Id)->Cfg.OpaqueCapability ? OSPF_OPT_O_BIT : 0;
                     
   if(IsInit)                     
      DbDscr.Flags = S_INIT_FLAG | S_MORE_FLAG | S_MASTER_FLAG;
   else   
      DbDscr.Flags = p_NBO->IsMaster ? S_MASTER_FLAG : 0;

   A_SET_4B(p_NBO->DdSeqNum, DbDscr.SeqNum);

   /* Add DD header to the packet */
   F_AddToEnd(p_F, (byte*) &DbDscr, sizeof(t_S_DbDscr));
   
   if(!IsInit)
   {
      /* Add top of list of LSA DB summary to the packet,
         starting from the last marked DD list entry */
      if(p_NBO->p_OutDdEntry)
      {
         p_DbEntry = p_NBO->p_OutDdEntry;
         e = E_OK;
      }
      else
         e = E_FAILED;

      if (ifcToNeighbor->Cfg.AuType == IFO_CRYPT)
      {
        authLen = OSPF_MD5_AUTH_DATA_LEN;
      }


      while(e == E_OK) 
      {
          /* Frame includes the 8 byte DD header and all LSA headers previously
           * added. Need to leave space for the common OSPF header and for the
           * IP header. */
         if ((IP_HDR_LEN + authLen + sizeof(t_S_PckHeader) +
              F_GetLength(p_F) + sizeof(t_S_LsaHeader)) > allocPktLen)
         {
            /* Set MORE flag */
            p_DbDscr = (t_S_DbDscr *) GET_LSA_HDR(p_F);
            p_DbDscr->Flags |= S_MORE_FLAG;

            /* set next outstanding DD list entry */
            p_NBO->p_OutDdEntry = p_DbEntry; 

            break;
         }

         UpdateAgeOfDbEntry (p_DbEntry);
         if (A_GET_2B(p_DbEntry->Lsa.LsAge) < MaxAge)
         {
           F_AddToEnd(p_F, (byte*) &p_DbEntry->Lsa, sizeof(t_S_LsaHeader));
         }
         
         e = HL_GetNext(p_NBO->DbSum, (void *)&p_DbEntry, p_DbEntry);
      }
   }
   
   /* Save the last DD packet for retransmission */
   p_NBO->p_LastDd = F_Copy(p_F);

   if(e != E_OK)
      p_NBO->p_OutDdEntry = 0; /* DD sending has completed */

   p_NBO->InitDDSent = IsInit;

   if (p_RTO->ospfTraceFlags & OSPF_TRACE_DD)
   {
       char traceBuf[OSPF_MAX_TRACE_STR_LEN];
       L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
       L7_uchar8 destRouterIdStr[OSAPI_INET_NTOA_BUF_SIZE];
       L7_uchar8 flagStr[10];
       L7_uchar8 optStr[20];
       flagStr[0] = '\0';
       p_DbDscr = (t_S_DbDscr *) GET_LSA_HDR(p_F);
       if (p_DbDscr->Flags & S_INIT_FLAG)
           strncat(flagStr, "I/", 2);
       if (p_DbDscr->Flags & S_MORE_FLAG)
           strncat(flagStr, "M/", 2);
       if (p_DbDscr->Flags & S_MASTER_FLAG)
           strncat(flagStr, "MS", 2);    
       OspfOptionsStr(p_DbDscr->Options, optStr);
       RTO_InterfaceName(ifcToNeighbor, ifName);
       osapiInetNtoa((L7_uint32) p_NBO->RouterId, destRouterIdStr);
       sprintf(traceBuf, "OSPF DD sent on interface %s to router %s. MTU: %d Options: %s Flags: %s SeqNo: %lu Length: %d ",
               ifName, destRouterIdStr, A_GET_2B(p_DbDscr->InterfaceMtu), optStr, flagStr, 
               A_GET_4B(p_DbDscr->SeqNum), sizeof(t_S_PckHeader) + F_GetLength(p_F));
       RTO_TraceWrite(traceBuf);
   }

   /* Transmit the Database Description packet */
   IFO_SendPacket(p_NBO->IFO_Id, p_F, S_DB_DESCR, p_NBO, FALSE);   
}


/*********************************************************************
 * @purpose    Determines if an LSA is self originated.
 *
 * @param            @b{(input)}  RTO object pointer
 * @param            @b{(input)}  pointer to the LSA header
 * @param      @b{(input)}  interface where the LSA was received
 *
 * @returns     TRUE  - the LSA is self-originated 
 * @returns     FALSE - the LSA is not self-originated 
 *
 * @notes    see RFC 2328 sections 13.4 and 14.1 for definition of self-originated.
 *           The definition attempts to detect network LSAs a router originated 
 *           before its router ID changed. It's really not worth the hassle. If 
 *           these former network LSAs are left to age normally, then until they
 *           age no router will have an adjacency with that network and the network
 *           LSAs won't be used in anyone's SPF. IETF recognized this in OSPFv3 and
 *           changed the definition of self-originated to simply check the 
 *           Advertising Router field. But I suppose to stay compliant with 2328, 
 *           we need to follow the rules. 
 *
 * @end
 * ********************************************************************/
static Bool SelfOrigLsaReceived(t_RTO *p_RTO, t_S_LsaHeader *p_LsaHdr, t_IFO *p_IFO)
{
  t_ARO *p_ARO;
  t_IFO *p_IfoTmp = NULL;
  t_IfoHashKey ifoKey;

  if (!p_RTO || !p_LsaHdr || !p_IFO)
    return FALSE;

  if (p_LsaHdr->LsType == S_NETWORK_LSA)
  {
    /* See if the LSID matches a local interface address on an interface in the 
     * area where the LSA was received. */
    ifoKey.IfAddr = (SP_IPADR) A_GET_4B(p_LsaHdr->LsId);
    ifoKey.aro = NULL;
    p_ARO = (t_ARO*) p_IFO->ARO_Id;
    if (HL_FindFirst(p_ARO->IfoHl, (byte*)&ifoKey, (void**) &p_IfoTmp) == E_OK)
    {
      return TRUE;
    }
  }

  return(A_GET_4B(p_LsaHdr->AdvertisingRouter) == p_RTO->Cfg.RouterId);
}

/*----------------------------------------------------------------
 *
 *      Timer Expiration Routines
 *
 *----------------------------------------------------------------*/

e_Err InactivityTimerExp( t_Handle Id, t_Handle TimerId, word Flag )
{
   t_NBO *p_NBO = (t_NBO *) Id;
   
   if(!p_NBO->OperationState)
      return E_OK;

   DoNboTransition(p_NBO, NBO_INACT_TIMER, 0);

   return E_OK;
}

e_Err DdRxmtTimerExp( t_Handle Id, t_Handle TimerId, word Flag )
{
   t_NBO *p_NBO = (t_NBO *) Id;
   t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;
   void *ddClone = NULL;
   
   if(!p_NBO->OperationState)
      return E_OK;

   if(p_NBO->IsMaster)
   {
      /* Restart DD Retransmission timer */
      TIMER_StartSec(p_NBO->DdRxmtTimer, TimerFractVar(p_NBO->RxmtInterval),
         0, DdRxmtTimerExp, p_NBO->OspfSysLabel.threadHndle );
   }

   /* Retransmit the last sent Database Description packet */
   if(p_NBO->p_LastDd)
   {
     if (p_RTO->ospfTraceFlags & OSPF_TRACE_DD)
     {
         t_S_DbDscr *p_DbDscr;
         char traceBuf[OSPF_MAX_TRACE_STR_LEN];
         L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
         L7_uchar8 destRouterIdStr[OSAPI_INET_NTOA_BUF_SIZE];
         L7_uchar8 flagStr[10];
         L7_uchar8 optStr[20];
         flagStr[0] = '\0';
         p_DbDscr = (t_S_DbDscr *) GET_LSA_HDR(p_NBO->p_LastDd);
         if (p_DbDscr->Flags & S_INIT_FLAG)
             strncat(flagStr, "I/", 2);
         if (p_DbDscr->Flags & S_MORE_FLAG)
             strncat(flagStr, "M/", 2);
         if (p_DbDscr->Flags & S_MASTER_FLAG)
             strncat(flagStr, "MS", 2);    
         OspfOptionsStr(p_DbDscr->Options, optStr);
         RTO_InterfaceName((t_IFO*)p_NBO->IFO_Id, ifName);
         osapiInetNtoa((L7_uint32) p_NBO->RouterId, destRouterIdStr);
         sprintf(traceBuf, "Retransmitting OSPF DD on interface %s to router %s."
                 " MTU: %d Options: %s Flags: %s SeqNo: %lu Length: %d ",
                 ifName, destRouterIdStr, A_GET_2B(p_DbDscr->InterfaceMtu), 
                 optStr, flagStr, A_GET_4B(p_DbDscr->SeqNum), 
                 sizeof(t_S_PckHeader) + F_GetLength(p_NBO->p_LastDd));
         RTO_TraceWrite(traceBuf);
     }

     ddClone = F_Copy(p_NBO->p_LastDd);
     IFO_SendPacket(p_NBO->IFO_Id, ddClone, S_DB_DESCR, p_NBO, FALSE);  
   }
   return E_OK;
}

static byte emptystr[3] = {0,0,0};


e_Err RqRxmtTimerExp( t_Handle Id, t_Handle TimerId, word Flag )
{
   t_NBO *p_NBO = (t_NBO *) Id;
   t_IFO *p_IFO = (t_IFO*) p_NBO->IFO_Id;
   word lsrqs;
   void *p_F;
   t_LsReqEntry *p_LsReq, *p_NextLsReq=NULLP;
   e_Err e;
   Bool overflow = 0;
   ulng allocPktLen;
   word authLen = 0;

   if(!p_NBO->OperationState)
      return E_OK;

   if (p_IFO->Cfg.AuType == IFO_CRYPT)
     authLen = OSPF_MD5_AUTH_DATA_LEN;
   
   HL_GetEntriesNmb(p_NBO->LsReq, &lsrqs);
   if(lsrqs)
   {
      do /* while overflow */
      {
         /* Allocate the new LS Req packet */
         if((p_F = IFO_AllocPacket(p_NBO->IFO_Id, S_LS_REQUEST,
                                   0, &allocPktLen)) == NULL)
            return E_FAILED;

         if(overflow)
         {
            /* If the packet overflow occured, 
               then start from the saved LS Req */
            p_LsReq = p_NextLsReq;
            overflow = 0;
            e = E_OK;
         }
         else
            e = HL_GetFirst(p_NBO->LsReq, (void *)&p_LsReq);

         while(e == E_OK)
         {
            if((IP_HDR_LEN + authLen + sizeof(t_S_PckHeader) +
               F_GetLength(p_F) + sizeof(t_S_LsReq)) > allocPktLen)
            {
               overflow = 1;
               p_NextLsReq = p_LsReq;
               break;
            }

            F_AddToEnd(p_F, emptystr, 3);
            F_AddToEnd(p_F, &p_LsReq->Lsa.LsType, 9);
         
            e = HL_GetNext(p_NBO->LsReq, (void *)&p_LsReq, p_LsReq);
         }

         /* Transmit the LS Request packet */
         IFO_SendPacket(p_NBO->IFO_Id, p_F, S_LS_REQUEST, p_NBO, TRUE);   
      
      } while(overflow);

      /* Start LS Request Retransmission timer */
      TIMER_StartSec(p_NBO->RqRxmtTimer, TimerFractVar(p_NBO->RxmtInterval),
                     0, RqRxmtTimerExp, p_NBO->OspfSysLabel.threadHndle );
   }
   else
      DoNboTransition(p_NBO, NBO_LOAD_DONE, 0);

   return E_OK;
}

void ospfDebugNboRetx(t_NBO *p_NBO)
{
  L7_uint32 i = 0;
  t_RetxEntry * retx = 0, * tail = 0;
  char traceBuf[OSPF_MAX_TRACE_STR_LEN];
  char tempBuf[OSPF_MAX_TRACE_STR_LEN];
  ulng lastSent = 0;

  tempBuf[0] = 0;

  if (!p_NBO)
  {
    printf("\nMust supply the memory address of a neighbor.");
    printf("\nUse ospfDebugNBOPrint() to get a list of neighbors.");
    return;
  }

  sprintf(traceBuf, "NBO 0x%x ipaddr %s area 0x%x", 
    (L7_uint32)p_NBO, osapiInet_ntoa((L7_uint32)p_NBO->IpAdr), 
    (L7_uint32)((t_ARO*)(p_NBO->ARO_Id))->AreaId);

  if(p_NBO->LsReTxHead)
  {
    if(!p_NBO->LsReTxTail)
      strcat(traceBuf, " head but no tail!");
  }
  else
  {
    if(p_NBO->LsReTxTail)
      strcat(traceBuf, " but no head!");
  }

  retx = p_NBO->LsReTxHead;

  if(retx)
    lastSent = retx->SentTime;

  while(retx)
  {
    if(!retx->nextSent)
      tail = retx;

    if(retx->SentTime < lastSent)
    {
      sysapiPrintf("\nretx list sort order corrupted last sent %d, curr %d", 
                   lastSent, retx->SentTime);
    }
    else
    {
      lastSent = retx->SentTime;
    }

    retx = retx->nextSent;
    i++;
  }

  if(tail != p_NBO->LsReTxTail)
    strcat(traceBuf, " corrupted!");

  if(!p_NBO->LsReTxHead && !p_NBO->LsReTxTail)
    sprintf(tempBuf, " LsReTx (%u) no head or tail",i);

  if(p_NBO->LsReTxHead && p_NBO->LsReTxTail)
    sprintf(tempBuf, " LsReTx (%u) head %x, tail %x", 
               i, 
               (L7_uint32)A_GET_4B(((t_RetxEntry *)p_NBO->LsReTxHead)->LsId),
               (L7_uint32)A_GET_4B(((t_RetxEntry *)p_NBO->LsReTxTail)->LsId)
               );

  strcat(traceBuf, tempBuf);

  RTO_TraceWrite(traceBuf);
}


void ospfDebugNBOPrint()
{
  t_ARO * p_ARO = NULLP;
  t_IFO * p_IFO = NULLP;
  t_NBO * p_NBO = NULLP;
  t_RTO *p_RTO = RTO_List;
  e_Err e = E_OK;

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
        ospfDebugNboRetx(p_NBO);

        e = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
      }
      /* get next interface */
      e = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
    }
    /* get next area */
    e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
  }
}

/*********************************************************************
 * @purpose  debug routine to list number of LSA retx per neighbor
 *
 * @param            void
 *
 * @returns          E_OK
 *
 * @notes            
 *
 * @end
 * ********************************************************************/
e_Err ospfDebugLsaRxmtCount(void)
{
  t_RTO *p_RTO = (t_RTO*) RTO_List;
  t_ARO * p_ARO = NULLP;
  t_IFO * p_IFO = NULLP;
  t_NBO * p_NBO = NULLP;
  e_Err e = E_OK;
  L7_uchar8 buf[512];

  e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
  while(e == E_OK && p_ARO)
  {
    e = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
    while(e == E_OK && p_IFO)
    {
      e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
      while(e == E_OK && p_NBO)
      {
        if (p_NBO->LsReTxCount)
        {
          L7_uchar8 nbrIdStr[OSAPI_INET_NTOA_BUF_SIZE];
          L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          RTO_InterfaceName(p_IFO, ifName);
          osapiInetNtoa((L7_uint32) p_NBO->RouterId, nbrIdStr);
          osapiInetNtoa((L7_uint32)p_NBO->IpAdr, nbrAddrStr);
          sprintf(buf, "\nNum LSA retx to %s at %s on %s: %lu.",
                  nbrIdStr, nbrAddrStr, ifName, p_NBO->LsReTxCount);
          XX_Print(buf);
        }
        e = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
      }
      /* get next interface */
      e = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
    }
    /* get next area */
    e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO);
  }

  return e;
}

e_Err NBO_LsRxmtExpire(t_Handle NBO_Id)
{
   t_NBO * p_NBO = (t_NBO *) NBO_Id;
   t_IFO *p_IFO = (t_IFO*) p_NBO->IFO_Id;
   t_RTO *p_RTO = (t_RTO *) p_NBO->RTO_Id;
   t_A_DbEntry *p_DbEntry;
   t_S_LsaHeader *p_LsaHdr;
   void  *p_F;
   word  size, num;
   t_TxRetransmit LsRetx;
   t_VirtTxRetransmit VirtLsRetx;
   t_RetxEntry *p_RetxEntry, *p_tempRetxEntry;
   ulng allocPktLen, ipPktLen;
   L7_uint32 lsaLength;
   word authLen = 0;

   if(!p_NBO->OperationState)
      return E_OK;
   
   if((p_NBO->State < NBO_EXCHANGE) || p_NBO->LsReTxHead == NULLP)
      return E_OK;
   
   /* enforce min interval between two consecutive rxmts */
   if(GET_INTERVAL(p_NBO->LsReTxSentTime) < p_IFO->Cfg.RxmtInterval)
      return E_OK;
      
   /* Allocate the LSA Update packet */
   if((p_F = IFO_AllocPacket(p_NBO->IFO_Id, S_LS_UPDATE,
                             0, &allocPktLen)) == NULL)
   {
      return E_FAILED;
   }

   /* save current data position */
   p_LsaHdr = (t_S_LsaHeader *) (GET_LSA_HDR(p_F) + 4);    
   size = 4;
   num = 0;
   if (p_IFO->Cfg.AuType == IFO_CRYPT)
   {
     authLen = OSPF_MD5_AUTH_DATA_LEN;
   }

   /* Browse the NBO's LSA retransmission sent linked list */
   p_RetxEntry = p_NBO->LsReTxHead;
   while(p_RetxEntry)      
   {
      p_DbEntry = p_RetxEntry->DbEntry;
      if(! (L7_BIT_ISSET(p_DbEntry->dbFlags, IS_CUR)))
      {
         p_tempRetxEntry = p_RetxEntry;
         p_RetxEntry = p_RetxEntry->nextSent;
         NBO_LsReTxDelete(p_NBO, &p_tempRetxEntry->LsType, p_tempRetxEntry);
         continue;
      }
      
      if((GET_INTERVAL(p_RetxEntry->SentTime) >= ((t_IFO *)p_NBO->IFO_Id)->Cfg.RxmtInterval))
      {
         /* restrict to one packet full */
         lsaLength = A_GET_2B(p_DbEntry->Lsa.Length);
         ipPktLen = IP_HDR_LEN + authLen + sizeof(t_S_PckHeader) + size + lsaLength;

         /* When we build UPDATE packets, we should try to avoid fragmentation.
          * So even though we allocate a buffer larger than the IP MTU, we limit
          * the size of updates to the IP MTU, unless a single LSA is larger
          * than the IP MTU. Then we let the stack fragment. */
         if ((num >= 1) && ((ipPktLen >= p_IFO->Cfg.MaxIpMTUsize) ||
                            (ipPktLen >= allocPktLen)))
         {
           break;         
         }

         /* Update age of the dbEntry and copy the LSA to LS Update packet*/
         UpdateAgeOfDbEntry (p_DbEntry);

         memcpy(p_LsaHdr, &p_DbEntry->Lsa, LSA_HEADER_LEN);
         memcpy(p_LsaHdr + 1, p_DbEntry->p_Lsa, lsaLength - LSA_HEADER_LEN);
         size += lsaLength;

         /* Update LSA age within the packet */
         A_SET_2B((((t_IFO*)p_NBO->IFO_Id)->Cfg.InfTransDelay + 
                  A_GET_2B(p_DbEntry->Lsa.LsAge)), p_LsaHdr->LsAge);
         if(A_GET_2B(p_LsaHdr->LsAge) > MaxAge)
            A_SET_2B(MaxAge, p_LsaHdr->LsAge);

         num += 1;
         
         /* Send trap   IF_TX_RETRANSMIT */
         if(((t_RTO *)p_NBO->RTO_Id)->Clbk.p_OspfTrap)
         {
            if((p_IFO->Cfg.Type == IFO_VRTL) &&
               (p_RTO->TrapControl & GET_TRAP_BITMASK(VIRT_IF_TX_RETRANSMIT)))
            {
               VirtLsRetx.RouterId =p_RTO->Cfg.RouterId;
               VirtLsRetx.VirtIfAreaId = p_IFO->Cfg.VirtTransitAreaId;  
               VirtLsRetx.VirtIfNeighbor = p_NBO->RouterId;
               VirtLsRetx.PacketType = S_LS_UPDATE;    
               VirtLsRetx.LsdbType = p_LsaHdr->LsType;      
               VirtLsRetx.LsdbLsid = A_GET_4B(p_LsaHdr->LsId);      
               VirtLsRetx.LsdbRouterId = A_GET_4B(p_LsaHdr->AdvertisingRouter);  
               VirtLsRetx.LsdbSeqNum = A_GET_4B(p_LsaHdr->SeqNum);      

               p_RTO->Clbk.p_OspfTrap(p_RTO->MngId,
                     VIRT_IF_TX_RETRANSMIT, (u_OspfTrap *) &VirtLsRetx);
            }
            else if(p_RTO->TrapControl & GET_TRAP_BITMASK(TX_RETRANSMIT))
            {
               LsRetx.RouterId = p_RTO->Cfg.RouterId;
               LsRetx.IfIpAddress = p_IFO->Cfg.IpAdr;  
               LsRetx.AddressLessIf = p_IFO->Cfg.IfIndex;
               LsRetx.NbrRtrId = p_NBO->RouterId;
               LsRetx.PacketType = S_LS_UPDATE;   
               LsRetx.LsdbType = p_LsaHdr->LsType;     
               LsRetx.LsdbLsid = A_GET_4B(p_LsaHdr->LsId);     
               LsRetx.LsdbRouterId = A_GET_4B(p_LsaHdr->AdvertisingRouter); 
               LsRetx.LsdbSeqNum = A_GET_4B(p_LsaHdr->SeqNum);      

               p_RTO->Clbk.p_OspfTrap(p_RTO->MngId, 
                     TX_RETRANSMIT, (u_OspfTrap *) &LsRetx);
            }
         }      

         /* update data position */
         p_LsaHdr = (t_S_LsaHeader *) ((byte*)p_LsaHdr + lsaLength);    

         /* move this entry to the back of the list and get 
          * the next entry. We use the Ins/Del fns with a 
          * NULL key. This ensures the LsReTx HL is not affected 
          * by this operation.
          */
         p_tempRetxEntry = p_RetxEntry;
         p_RetxEntry = p_RetxEntry->nextSent;
         NBO_LsReTxUpdateSentTime(p_NBO, p_tempRetxEntry);
      }
      else
      {
        /* LSAs are ordered by sent time */
        break;
      }

   }

   if(num)
   {
      /* Update packet length */
      B_SetLength(F_GetFirstBuf(p_F), size);
      F_GetLength(p_F) = size;
      
      /* Set number of LSAs in LS Update packet */
      A_SET_4B(num, GET_LSA_HDR(p_F));
      
      /* Retransmit the LS Update packet */
      IFO_SendPacket(p_NBO->IFO_Id, p_F, S_LS_UPDATE, p_NBO, TRUE);   

      /* save retx packet sent time */
      p_NBO->LsReTxSentTime = TIMER_SysTime();
      p_NBO->LsReTxCount += num;

   }
   else
   {
      F_Delete(p_F);
   }

   return E_OK;
}

e_Err SlaveLastDdTimerExp( t_Handle Id, t_Handle TimerId, word Flag )
{
   t_NBO *p_NBO = (t_NBO *) Id;

   if(p_NBO->p_LastDd && F_GetLength(p_NBO->p_LastDd))
   {
      F_Delete(p_NBO->p_LastDd);
      p_NBO->p_LastDd = NULL;
   }

   return E_OK;
}

/*********************************************************************
* @purpose  Clear OSPF neighbors.
*
* @param    RTO_Id - OSPF instance
* @param    IFO_Id - If non-zero, restrict clear to neighbors on this interface.
* @param    routerId   If non-zero, restrict clear to neighbors with this router ID. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
e_Err RTO_NeighborClear(t_Handle RTO_Id, t_Handle IFO_Id, ulng routerId)
{
  e_Err ea, ei, en;
  t_RTO *p_RTO = (t_RTO*) RTO_Id;
  t_IFO *nbrIfo = (t_IFO*) IFO_Id;
  t_IFO *p_IFO;
  t_ARO *p_ARO;
  t_NBO *p_NBO;
  t_NBO *nextNbr = NULL;

  ea = HL_GetFirst(p_RTO->AroHl, (void**)&p_ARO);
  while (ea == E_OK)
  {
    ei = HL_GetFirst(p_ARO->IfoHl, (void**) &p_IFO);
    while (ei == E_OK)
    {
      if (!nbrIfo || (nbrIfo == p_IFO))
      {
        en = HL_GetFirst(p_IFO->NboHl, (void**) &p_NBO);
        while (en == E_OK)
        {
          en = HL_GetNext(p_IFO->NboHl, (void**)&nextNbr, p_NBO);
          if ((routerId == 0) || (routerId == p_NBO->RouterId))
          {
            DoNboTransition(p_NBO, NBO_KILL, 0);
          }
          p_NBO = nextNbr;
        }
      }
      ei = HL_GetNext(p_ARO->IfoHl, (void**)&p_IFO, p_IFO);
    }
    ea = HL_GetNext(p_RTO->AroHl, (void**)&p_ARO, p_ARO);
  }
  return E_OK;
}


/* --- end of file SPNBO.C --- */

