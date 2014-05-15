/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename   arp.c
 *
 * @purpose    Regular ARP object implementation. The code in this file is used
 *             by all modes of
 *
 * @component  ARP Component
 *
 *
 * @comments    All functions in this file are internal to the ARP
 *              component and must not be called without first taking
 *              the ARP lock (via the API).
 *              
 *              None of the functions in this file take the lock.
 *
 * @comments
 * External Routines:
 *
 * ARP_Init              - create ARP object
 * ARP_Delete            - delete ARP object
 * ARP_UpdateTx          - change transmit callback
 * ARP_AddInterface      - adds low interface to the ARP object
 * ARP_DelInterface      - deletes low interface from the ARP object
 * ARP_UpdateNAK         - change NAK callback
 * ARP_AddLocAddr        - add local address peer
 * ARP_DelLocAddr        - delete local address peer
 * ARP_AddDstAddr        - add destination address peer
 * ARP_DelDstAddr        - delete destination address peer
 * ARP_Receive           - data receive routine
 * ARP_GetMACAddr        - get MAC address via IP address
 * ARP_SendARPReq        - send ARP request
 * ARP_SendARPReqNoRxmt  - send ARP request with no timout for retransmits
 * ARP_RegUser           - register user request
 * ARP_UnregUser         - unregister user
 * ARP_AdjustTimeout     - adjust the ARP entry timeout
 * ARP_TrimCache         - trim ARP cache
 * ARP_ClearCache        - clear ARP cache
 * ARP_GetCacheStats     - retrieve various ARP cache statistics
 * ARP_StopEntryTimers   - stop ARP entry timers
 *
 * Static Routines:
 * _emptyData            - empty data receive/transmit routine
 * _emptyARPCallback     - empty positive ARP callback routine
 * _emptyNAKCallback     - empty negative ARP callback routine
 * _emptyEVCallback      - empty event callback routine
 * _deleteAddrEntry      - delete address entry
 * _createAddrEntry      - create address entry
 * _updateARPCache       - update ARP cache
 * _arpAgeTime           - ARP age timeuot callback
 * _arpTimeout           - ARP response timeout callback
 * _stopAllEntryTimers   - forces all ARP entry timers to be stopped
 * _sendARPReq           - send ARP request frame
 * _setARPRes            - set ARP resolution structure
 *
 *
 * @create     10/01/1997
 *
 * @author     Alex Ossinski
 *
 * @end
 *
 * ********************************************************************/
 #ifdef FILEID_IN_MEMORY
  static char __fileid[] = "$Header: lvl7dev\src\system_support\routing\arp\arp.c 1.10.3.7 2002/11/03 19:00:59EST mfiorito Exp  $";
#endif


/* --- standard include files --- */

#include <string.h>
#include <stdlib.h>
#include <stddef.h>

/* --- external object interfaces --- */

#include "xx.ext"
#include "frame.h"
#include "frame.ext"
#include "buffer.ext"
#include "timer.ext"
#include "hl.ext"
#include "ll.ext"

#include "arp.ext"
#include "arp_debug_api.h"
#include "arp_debug.h"
#include "arp.h"
#include "l7_vrrp_api.h"

#include "l7_common.h"
#include "osapi.h"
#include "osapi_support.h"
#include "l7_ipmap_arp_api.h"
#include "ipmap_arp_exten.h"
#include "ipmap_arp.h"
#include "log.h"
#include "rto_api.h"
#include "nimapi.h"
#include "simapi.h"
#include "trapapi.h"
#include "l7utils_api.h"

#define LOCAL_FLAG 0x1

/* check for expected number of supported routing interfaces */
#if (MAX_INTERFACES != (L7_RTR_MAX_RTR_INTERFACES+1))
#error The MAX_INTERFACES value defined in arp.h is wrong!
#endif

/*------static layer data--------------*/
t_ARP *ARP_List;
static byte nullMacAddr[MACADDRLENGTH] = {0,0,0,0,0,0};
static byte bcstMacAddr[MACADDRLENGTH] = {0xff,0xff,0xff,0xff,0xff,0xff};
static L7_uint32 lastPurgeOperationTime = 0;

ulng ARP_SeqNum = ARP_SEQ_NUM_INIT;     /* continuously incrementing sequence number */

/* static routine declaration */
static e_Err _emptyData(t_Handle, t_Handle, word);
static e_Err _emptyNAKCallback(t_Handle userId, t_Handle regId);
t_ADR *_createAddrEntry(t_ARP *p_A, t_IPAddr *ipAddr, t_MACAddr *macAddr,
                        word entryType, word flags);
static t_ADR *_createGratArpAddrEntry(t_ARP *p_A, t_IPAddr *ipAddr, word ifIndex);
static e_Err _deleteAddrEntry(t_ARP *p_A, t_ADR *p_Adr);
static e_Err  _deleteOldestAddrEntry(t_ARP *p_A, Bool allowGw);
static void trapAndLogConflict(word ifIndex, t_IPAddr *ipAddr,
                               t_MACAddr *macAddr,
                               L7_uchar8 vrId,
                               L7_BOOL notifyFlag);
static Bool  detectIPAddrConflicts(t_ARP *p_A, t_IPAddr *ipAddr,
                                   t_MACAddr *macAddr, word ifIndex);
static t_ADDREnt *getLocalAddrEntry(t_ARP *p_A, t_IPAddr *ipAddr,
                                    word ifIndex);
static void  _updateARPCache(t_ARP *p_A, t_IPAddr *ipAddr,
                             t_MACAddr *macAddr, word flags, Bool create);
static void  _stopAllEntryTimers(t_ADR *p_Adr);
static void  _sendARPReq(t_ADR *p_Adr, L7_BOOL rxmt);
static e_Err  _sendGratARP(t_ADR *p_Adr);
static e_Err ARP_ProxyArp(L7_uint32 flags, t_ARPFrame *arpFrame);

/* Timer Expiration Callbacks. These functions run in a different
 * thread context and need to take the ARP component lock
 */
static e_Err _arpAgeTimeExp(t_Handle adrId, t_Handle timer, word flags);
static e_Err _arpTimeoutExp(t_Handle adrId, t_Handle timer, word flags);
static e_Err _gratArpTimeoutExp(t_Handle adrId, t_Handle timer, word flags);

/* ARP global context */
extern ipMapArpCtx_t  ipMapArpCtx_g;

extern void ipMapAddrConflictNotify(L7_uint32 intIfNum,
                                    L7_uint32 ipAddr,
                                    L7_uchar8 *macAddr);

/* PTin Added - Routing support */
extern L7_uint16 ptin_ipdtl0_getdtl0Vid(L7_uint16 dtl0Vid);

/*********************************************************************
 * @purpose             Create ARP object.
 *
 *
 * @param  userId       @b{(input)} user Id
 * @param  lowId        @b{(input)}  layer tranmit Id
 * @param  p_ObjId      @b{(output)}  pointer to put object Id
 *
 * @returns  E_OK      - success
 * @returns  E_BADPARM - wrong parameters or combination of them
 * @returns  E_NOMEMORY - memory allocation failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARP_Init(t_ARPCfg *p_config, t_Handle userId,
               t_Handle lowId, word queueSize, t_Handle *p_ObjId)
{
    t_ARPIf *p_if;
    t_ARP   *p_A = (t_ARP *)XX_Malloc(sizeof(t_ARP));
    if (p_A == NULL)
    {
      return E_NOMEMORY;
    }

    memset(p_A, 0, sizeof(t_ARP));
    p_if = (t_ARPIf *)XX_Malloc(sizeof(t_ARPIf));
    if(!p_if)
    {
        XX_Free(p_A);
        return E_NOMEMORY;
    }

    /* set ARP object configuration parameters */
    p_A->p_if[0] = p_if;
    p_A->userId = userId;
    p_if->lowId = lowId;
    if(p_config != (t_ARPCfg *)NULLP)
        p_A->arpAgeTime = p_config->arpAgeTime;
    if(!p_A->arpAgeTime)
        p_A->arpAgeTime = (word)ARP_DEF_AGETIME;

    if(p_config != (t_ARPCfg *)NULLP)
        p_A->arpRespTime = p_config->arpRespTime;
    if(!p_A->arpRespTime)
        p_A->arpRespTime = (word)ARP_DEF_RESPTIME;

    if(p_config != (t_ARPCfg *)NULLP)
        p_A->retrNmb = p_config->retrNmb;
    if(!p_A->retrNmb)
        p_A->retrNmb = (word)ARP_DEF_RETRNMB;

    if(p_config != (t_ARPCfg *)NULLP)
        p_A->arpCacheSize = p_config->arpCacheSize;
    if(!p_A->arpCacheSize)
        p_A->arpCacheSize = (word)platRtrArpMaxEntriesGet();

    if(p_config != (t_ARPCfg *)NULLP)
        p_A->dynamicRenew = p_config->dynamicRenew;
    if(!p_A->dynamicRenew)
        p_A->dynamicRenew = (word)ARP_DEF_DYNAMICRENEW;

    if(p_config != (t_ARPCfg *)NULLP)
        p_if->f_Transmit = p_config->f_Transmit;
    if(p_if->f_Transmit == (F_ARPTransmit)NULLP)
        p_if->f_Transmit = _emptyData;

    if(p_config != (t_ARPCfg *)NULLP)
        p_A->f_NAK = p_config->f_NAC;
    if(p_A->f_NAK == (F_ARPNAKCallback)NULLP)
        p_A->f_NAK = _emptyNAKCallback;

    if(queueSize != NULLP)
        p_A->queueSize = queueSize;
    else
        p_A->queueSize = (word)ARP_REQUESTS_QUEUE_SIZE;

    p_A->arpCacheId = p_A;    /* use object instance as cache identifier */
    p_A->status = ARP_OBJ_STAT_Valid;
    if(p_ObjId != (t_Handle *)NULLP)
        *p_ObjId = p_A;
    return E_OK;

}




/*********************************************************************
 * @purpose         Delete ARP object.
 *
 *
 * @param p_bjid  @b{(input)}  pointer to clear object Id
 *
 * @param         @b{(output)}
 *
 * @returns E_OK      - success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARP_Delete(t_Handle *p_ObjId)
{
   t_ARP *p_A = NULLP;
   e_Err e;
   t_ADR *p_Adr;
   t_ADDREnt *p_Adre, *tmp;
   word       index;

   if(p_ObjId)
   {
     p_A = (t_ARP *)*p_ObjId;
   }
   ASSERT(p_A);
   ASSERT(p_A->status == ARP_OBJ_STAT_Valid);

   /* delete all address entries from the ARP cache */
   for(e = ipMapArpExtenCacheGetFirst(p_A->arpCacheId, &p_Adr);
       e == E_OK;
       e = ipMapArpExtenCacheGetNext(p_A->arpCacheId, p_Adr, &p_Adr))
   {
     (void)_deleteAddrEntry(p_A, p_Adr);
   }

   /* destroy all local addresses entries if any */
   for(p_Adre = p_A->addrEnt; p_Adre != (t_ADDREnt *)NULLP;)
   {
       tmp = p_Adre;
       p_Adre = p_Adre->next;
       XX_Free(tmp);
   }

   /* destroy all interfaces */
   for(index = 0; index < (word)MAX_INTERFACES; index++)
   {
       if(p_A->p_if[index])
           XX_Free(p_A->p_if[index]);
   }

   p_A->status = ARP_OBJ_STAT_Invalid;
   XX_Free(p_A);
   if(p_ObjId != (t_Handle *)NULLP)
       *p_ObjId = (t_Handle)NULLP;
   return E_OK;
}

/*********************************************************************
 * @purpose       Remove the older entries from the ARP cache
 *
 * @param p_bjid  @b{(input)}  pointer to clear object Id
 *
 * @param         @b{(output)}
 *
 * @returns E_OK    checked entire ARP cache
 *          E_BUSY  stopped without checking entire cache because of time limit
 *
 * @notes         Limit the time spent on this to 100 milli-seconds. Don't want
 *                to hold the ARP lock for a long time. 
 *
 * @end
 * ********************************************************************/
e_Err ARP_PurgeOldEntries(t_Handle *p_ObjId)
{
  t_ARP *p_A = (t_ARP *)*p_ObjId;
  t_ADR         *p_Adr;
  t_ADR *nextAdr;
  L7_uint32     currentTime = osapiUpTimeRaw();
  e_Err e;
  L7_arpQuery_t arpQueryEntry;
  L7_uint32 startTime = osapiTimeMillisecondsGet();

  /* purge all dynamic entries for the specified interface(s) */
  e = ipMapArpExtenCacheGetFirst(p_A->arpCacheId, &p_Adr);
  while ((e == E_OK) && p_Adr)
  {
    e = ipMapArpExtenCacheGetNext(p_A->arpCacheId, p_Adr, &nextAdr);

    /* skip all permanent ARP entries (i.e., local, static) and gateway entries */
    if ((p_Adr->permanent == TRUE) ||
        (p_Adr->entryType == ARP_ENTRY_TYPE_GATEWAY))
    {
      p_Adr = nextAdr;
      continue;
    }

    memcpy((void *)&(arpQueryEntry.ipAddr),
           (void *)p_Adr->ipAddr, sizeof(p_Adr->ipAddr));
    arpQueryEntry.vlanId        = 0; /* ignored now */
    arpQueryEntry.intIfNum      = p_Adr->interfaceNumber;
    arpQueryEntry.arpQueryFlags = 0; /* set in the layers below */
    arpQueryEntry.lastHitTime   = p_Adr->lastHitTime;

    if (ipMapIpArpEntryQuery(&arpQueryEntry) != L7_SUCCESS)
    {
      p_Adr = nextAdr;
      continue;
    }

    p_Adr->lastHitTime   = arpQueryEntry.lastHitTime;

    /* for the unused entries, check if their last-hit-time is more than
     * ARP_PURGE_TIMEOUT seconds. If found, delete such entries from
     * the cache */
    if ((! (arpQueryEntry.arpQueryFlags & L7_ARP_HIT_ANY)) &&
        ((currentTime - p_Adr->lastHitTime) > ARP_PURGE_TIMEOUT))
    {

       if (_deleteAddrEntry(p_A, p_Adr) == E_OK)
       {
         p_A->earlyDeletes++;
       }
    }
    p_Adr = nextAdr;
    if ((osapiTimeMillisecondsGet() - startTime) > ARP_MAX_WINNOW_TIME)
    {
      return E_BUSY;
    }
  } 

  return E_OK;
}

/*********************************************************************
 * @purpose Change low layer transmit function pointer
 *
 *
 * @param  Id      @b{(input)}  object Id
 * @param  f_Tx    @b{(input)}  transmit function pointer or NULLP
 * @param  lowId   @b{(input)}  low layer handle or NULLP
 *
 * @param         @b{(output)}  n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void ARP_UpdateTx(t_Handle Id, F_ARPTransmit f_Tx, t_Handle lowId)
{
    t_ARP *p_A = (t_ARP *)Id;
    ASSERT(p_A);
    ASSERT(p_A->status == ARP_OBJ_STAT_Valid);

    if(f_Tx != (F_ARPTransmit)NULLP)
        p_A->p_if[0]->f_Transmit = f_Tx;
    if(lowId != (t_Handle)NULLP)
        p_A->p_if[0]->lowId = lowId;
}




/*********************************************************************
 * @purpose             Adds layer 2 interface
 *
 *
 * @param  arpId        @b{(input)}   object Id
 * @param  f_Tx         @b{(input)}  transmit function pointer or NULL
 * @param  txId         @b{(input)}  low layer handle or NULLP
 * @param  ifNumber     @b{(input)}  interface index
 *
 *
 *
 * @returns             E_FAILED
 * @returns             E_BADPARM
 * @returns             E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARP_AddInterface(t_Handle arpId, F_ARPTransmit f_Tx, t_Handle txId, t_LIH
 ifNumber)
{
    t_ARP   *p_A = (t_ARP *)arpId;
    t_ARPIf *p_if;

    if(!p_A || p_A->status != ARP_OBJ_STAT_Valid)
        return E_BADPARM;
    if(ifNumber >= (t_LIH)MAX_INTERFACES)
        return E_BADPARM;
    if(p_A->p_if[ifNumber])
        return E_BADPARM;

    p_if = (t_ARPIf *)XX_Malloc(sizeof(t_ARPIf));
    if(!p_if)
        return E_FAILED;
    memset(p_if, 0, sizeof(t_ARPIf));
    p_if->f_Transmit = f_Tx;
    p_if->lowId = txId;
    p_if->number = ifNumber;
    p_if->addrValid = FALSE;
    p_A->p_if[ifNumber] = p_if;
    return E_OK;
}



/*********************************************************************
 * @purpose              Deletes layer 2 interface
 *
 *
 * @param  arpId         @b{(input)}  object Id
 * @param ifNumber       @b{(input)}  interface index
 *
 *
 *
 * @returns              E_BADPARM
 * @returns              E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARP_DelInterface(t_Handle arpId, t_LIH ifNumber)
{
    t_ARP   *p_A = (t_ARP *)arpId;

    if(!p_A || p_A->status != ARP_OBJ_STAT_Valid)
        return E_BADPARM;
    if(ifNumber >= (t_LIH)MAX_INTERFACES)
        return E_BADPARM;
    if(!p_A->p_if[ifNumber])
        return E_OK;
    XX_Free(p_A->p_if[ifNumber]);
    p_A->p_if[ifNumber] = (t_ARPIf *)NULLP;
    return E_OK;
}



/*********************************************************************
 * @purpose          Change NAK callback
 *
 *
 * @param Id         @b{(input)}  object Id
 * @param f_NAC      @b{(input)}  NAK callback or NULLP
 * @param userId     @b{(input)}  user handle or NULLP
 *
 *
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void ARP_UpdateNAK(t_Handle Id, F_ARPNAKCallback f_NAK, t_Handle userId)
{
    t_ARP *p_A = (t_ARP *)Id;
    ASSERT(p_A);
    ASSERT(p_A->status == ARP_OBJ_STAT_Valid);

    if(f_NAK != (F_ARPNAKCallback)NULLP)
        p_A->f_NAK = f_NAK;
    if(userId != (t_Handle)NULLP)
        p_A->userId = userId;
}




/*********************************************************************
 * @purpose          Add local MAC & IP address to the object list and ARP cache
 *
 *
 * @param Id         @b{(input)}  object Id
 * @param ipAddr     @b{(input)}  IP address pointer
 * @param macAddr    @b{(input)}  MAC address pointer
 * @param flags      @b{(input)}  control bit flag
 *
 * @returns      E_OK      - success
 * @returns      E_FAILED
 * @returns      E_NOMEMORY
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARP_AddLocAddr(t_Handle Id, t_IPAddr *ipAddr,
                     t_MACAddr *macAddr, word flags)
{
    t_ARP *p_A = (t_ARP *)Id;
    t_ADR *p_Adr;
    t_ARPIf *p_if;
    L7_uint32 netIpAddr;
    t_ADDREnt *addrEnt = (t_ADDREnt *)XX_Malloc(sizeof(t_ADDREnt ));

    ASSERT(p_A);
    ASSERT(p_A->status == ARP_OBJ_STAT_Valid);

    if (addrEnt == NULL)
    {
      return E_NOMEMORY;
    }

    memset(addrEnt, 0, sizeof(t_ADDREnt));
    p_if = p_A->p_if[flags];
    if (!p_if)
    {
      XX_Free(addrEnt);
      return E_FAILED;
    }
    netIpAddr = osapiHtonl(*(L7_uint32 *)ipAddr);
    addrEnt->next = addrEnt->prev = (t_ADDREnt *)NULLP;
    memcpy(addrEnt->macAddr, macAddr, MACADDRLENGTH);
    memcpy(addrEnt->ipAddr, &netIpAddr, IPADDRLENGTH);
    addrEnt->intfNum = flags;
    XX_AddToDLList(addrEnt, p_A->addrEnt);
    p_A->state = ARP_STATE_ACTIVE;
    p_if->addrValid = TRUE;

    /* also add the local address entry to the ARP cache */
    p_Adr = _createAddrEntry(p_A, (t_IPAddr *)&netIpAddr, macAddr, ARP_ENTRY_TYPE_LOCAL, flags);
    ASSERT(p_Adr);

    return E_OK;
}




/*********************************************************************
 * @purpose        Delete local MAC & IP address from the object list.
 *
 *
 * @param Id        @b{(input)}  object Id
 * @param ipAddr    @b{(input)}  IP address pointer
 * @param flags     @b{(input)}  Control bit flag (interface number)
 *
 * @returns  E_OK      - success
 * @returns  E_FAILED  - failure
 *
 * @notes    ipAddr is in host byte order.
 *
 * @end
 * ********************************************************************/
e_Err ARP_DelLocAddr(t_Handle Id, t_IPAddr *ipAddr, word flags)
{
    t_ARP     *p_A = (t_ARP *)Id;
    t_ADDREnt *addrEnt, *saveAddrEnt = NULL;
    t_ADR     *p_Adr;
    t_ARPIf   *p_if;
    e_Err      e;
    L7_uint32 addrEntIntfNumCount = 0;
    word      addrEntIntfNum = 0;
    L7_uint32 netIpAddr = osapiHtonl(*(L7_uint32 *)ipAddr);


    ASSERT(p_A);
    ASSERT(p_A->status == ARP_OBJ_STAT_Valid);

    for (addrEnt = p_A->addrEnt; addrEnt; addrEnt = addrEnt->next)
    {
        if ((memcmp(addrEnt->ipAddr, &netIpAddr, sizeof(t_IPAddr)) == 0) && 
            (addrEnt->intfNum == flags ))
        {
            addrEntIntfNum = addrEnt->intfNum;
            saveAddrEnt = addrEnt;
            break;
        }
    }

    for (addrEnt = p_A->addrEnt; addrEnt; addrEnt = addrEnt->next)
    {
        if (addrEnt->intfNum == addrEntIntfNum)
        {
            addrEntIntfNumCount++;
        }
    }

    addrEnt = saveAddrEnt;

    if (addrEnt)
    {
        /* mark the interface as no longer valid */

        p_if = p_A->p_if[addrEnt->intfNum];

        if (!p_if)
        {
            return E_FAILED;
        }

        if (addrEntIntfNumCount == 1)
        {
            /*
            Only do the following if there is only one IP address
            associated with this interface
            */

            p_if->addrValid = FALSE;

            /* delete all address entries for this interface from the ARP cache */

            (void)ARP_ClearCache(p_A, addrEnt->intfNum, TRUE);
        }

        /* delete the local address entry from the ARP cache */
        e = ipMapArpExtenCacheFind(p_A->arpCacheId, (byte *)&netIpAddr, flags, &p_Adr);
        ASSERT(e == E_OK);
        ASSERT(p_Adr);
        e = _deleteAddrEntry(p_A, p_Adr);
        ASSERT(e == E_OK);

        XX_DelFromDLList(addrEnt, p_A->addrEnt);
        XX_Free(addrEnt);
    }

    /* set the overall ARP state to disabled when last intf deleted */

    if (!p_A->addrEnt)
    {
        p_A->state = ARP_STATE_DISABLED;

        /* delete any leftover address entries from the ARP cache */

        (void)ARP_ClearCache(p_A, IP_MAP_ARP_INTF_ALL, TRUE);
    }

    return E_OK;
}



/*********************************************************************
 * @purpose         Returns first available source MAC address
 *
 *
 * @param  Id       @b{(input)}  object Id
 * @param  flags    @b{(input)}  Control bit flag (interface number)
 * @param  srcMAC   @b{(input)}  Pointer to store source MAC address
 *
 * @returns  E_OK      - success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARP_GetSrcMAC(t_Handle Id, word flags, t_MACAddr *srcMAC)
{
   t_ARP     *p_A = (t_ARP *)Id;
   t_ADDREnt *p_ent;

   ASSERT(p_A);
   ASSERT(p_A->status == ARP_OBJ_STAT_Valid);

   for(p_ent = p_A->addrEnt; p_ent; p_ent = p_ent->next)
   {
       if(p_ent->intfNum == flags)
           break;
   }
   if(!p_ent)
       return E_FAILED;

   if(srcMAC)
       memcpy(srcMAC, p_ent->macAddr, sizeof(t_MACAddr));
   return E_OK;
}




/*********************************************************************
 * @purpose         Add destination address peer to the object list.
 *
 *
 * @param Id        @b{(input)}  object Id
 * @param ipAddr    @b{(input)}  IP address pointer
 * @param macAddr   @b{(input)}  MAC address pointer
 * @param flags     @b{(input)}  control bitflag
 *
 * @param           @b{(output)}
 *
 * @returns  E_OK      - success
 * @returns  E_BADPARM - entry exist already
 * @returns  E_FAILED  - processing error, or no more room for static entries
 *
 * @notes    ipAddr is in host byte order and converted to network byte order here.
 *
 * @end
 * ********************************************************************/
e_Err ARP_AddDstAddr(t_Handle  Id, t_IPAddr  *ipAddr,
                     t_MACAddr *macAddr, word flags)
{
   t_ARP        *p_A = (t_ARP *)Id;
   t_ADR        *p_Adr;
   e_Err        e;
   L7_uint32    _ipAddr = osapiHtonl(*(L7_uint32 *)ipAddr);
   t_IPAddr     *netIpAddr = (t_IPAddr*) &_ipAddr;

   ASSERT(p_A);
   ASSERT(p_A->status == ARP_OBJ_STAT_Valid);

   /* check if entry already exists */
   if(ipMapArpExtenCacheFind(p_A->arpCacheId, (byte *)netIpAddr, flags, &p_Adr) == E_OK)
   {
     /* stop all entry timers */
     _stopAllEntryTimers(p_Adr);

     /* lvl7_p1272 start */
     /* if an ARP resolution is in progress for this IP address,
      * destroy the pending entry and create a new one to allow
      * normal user callback notifications
      */
     if(memcmp(p_Adr->macAddr, nullMacAddr, sizeof(t_MACAddr)) == 0)
     {
       e_Err e;
       /* delete address entry */
       e = _deleteAddrEntry(p_A, p_Adr);
       if (e != E_OK)
         return E_FAILED;
       p_Adr = (t_ADR *)NULLP;
     }
     else
     {
       /* existing ARP entry with an already-resolved MAC address
        *
        * NOTE:  Want to avoid deleting an existing gateway entry
        *        here in lieu of the static entry so that there is
        *        not a momentary lapse in route resolution
        */

       /* if changing dynamic/gateway to static, remove it from age list */
       if (p_Adr->permanent != TRUE)
       {
         e = ipMapArpExtenEntryStaticConvert(p_A->arpCacheId, p_Adr);
         ASSERT(e == E_OK);
       }

       p_Adr->entryType = ARP_ENTRY_TYPE_STATIC;
       p_Adr->permanent = TRUE;
       p_Adr->timeStamp = 0;
       p_Adr->intfNum = flags;
       /* check if supplied MAC address is different than existing MAC */
       if ((macAddr) &&
           (memcmp(p_Adr->macAddr, macAddr, MACADDRLENGTH) != 0))
       {
         memcpy(p_Adr->macAddr, macAddr, MACADDRLENGTH);

         e = ipMapArpExtenCacheUpdate(p_A->arpCacheId, p_Adr, TRUE);
         ASSERT(e == E_OK);
       }
       return E_OK;
     }
     /* lvl7_p1272 end */
   }

   p_Adr = _createAddrEntry(p_A, netIpAddr, macAddr, ARP_ENTRY_TYPE_STATIC, flags);
   ASSERT(p_Adr);

   return E_OK;
}




/*********************************************************************
 * @purpose         Delete destination address peer from the object list.
 *
 *
 * @param Id        @b{(input)}  object Id
 * @param ipAddr    @b{(input)}  IP address pointer (host byte order)
 * @param flags     @b{(input)}  Control bit flag (interface number)
 *
 * @returns         E_OK      - success
 * @returns         E_BADPARM - entry does not exist
 * @returns         E_FAILED  - unable to delete address
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARP_DelDstAddr(t_Handle Id, t_IPAddr *ipAddr, word flags)
{
   t_ARP *p_A = (t_ARP *)Id;
   t_ADR *p_Adr;
   e_Err e;
   L7_uint32    _ipAddr = osapiHtonl(*(L7_uint32 *)ipAddr);
   t_IPAddr     *netIpAddr = (t_IPAddr*) &_ipAddr;

   ASSERT(ipAddr);
   ASSERT(p_A);
   ASSERT(p_A->status == ARP_OBJ_STAT_Valid);

   /* find entry to delete */
   if(ipMapArpExtenCacheFind(p_A->arpCacheId, (byte *)netIpAddr, flags, 
         &p_Adr) != E_OK)
     return E_BADPARM;

   e = _deleteAddrEntry(p_A, p_Adr);

   return (e == E_OK) ? E_OK : E_FAILED;
}

/*********************************************************************
 * @purpose         Given an ARP request received on a given local interface,
 *                  attempt to respond to the ARP request using proxy ARP.
 *
 * @param flags     @b{(input)}     Control bit flag (interface number)
 * @param arpFrame  @b{(input/output)}   ARP frame pointer. If proxy ARP
 *                                  is successful, the IP and MAC fields
 *                                  are updated as required to turn this into
 *                                  an ARP response.
 *
 * @returns  E_OK   - if we can generate a proxy ARP response
 * @returns  E_FAILED
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err ARP_ProxyArp(L7_uint32 flags, t_ARPFrame *arpFrame)
{
    L7_uint32 destAddr;    /* destination IP in host byte order */
    L7_uint32 srcIPAddr;    /* source IP in host byte order */
    L7_routeEntry_t route;   /* best route to target IP address */
    L7_uint32 nh, tempSrcIP, tempDstIP;
    L7_uint32 destIP;
    char rxIfMacAddr[sizeof(t_MACAddr)];   /* MAC on rx interface */
    char vMacAddr[sizeof(t_MACAddr)];      /* virtual MAC if VRRP active */
    L7_BOOL localProxyArpMode;  /* whether local proxy ARP is enabled on
                                      rx interface */

    /* respond with MAC address on receive interface unless VRRP is active */
    char *srcMac = rxIfMacAddr;

    L7_BOOL vrrpEnabled = L7_FALSE;
    L7_uchar8 vrrpId;
    L7_uint32 rxIntIfNum;

    /* Get internal interface number from flags */
    if (ipMapArpExtenGetIntIfNum(flags, &rxIntIfNum) != E_OK)
    {
        return E_FAILED;
    }

    /* Determine whether proxy ARP is enabled on the receive interface */
    if (!ipMapArpExtenProxyArpEnabled(flags))
    {
        return E_FAILED;
    }

    if (vrrpFuncTable.L7_vrrpAdminModeGet &&
        (vrrpFuncTable.L7_vrrpAdminModeGet(&vrrpEnabled) == L7_SUCCESS) &&
        (vrrpEnabled == L7_TRUE))
    {
        /* If VRRP is configured on the interface where the ARP request
         * was received, then if we have a route, respond with the virtual
         * MAC rather than the physical MAC. Assume the master and backup
         * routers in the same virtual router have reachability to the
         * same destinations (if they don't they probably shouldn't be part
         * of the same virtual router). */
        if (vrrpFuncTable.L7_vrrpIsActive &&
            (vrrpFuncTable.L7_vrrpIsActive(rxIntIfNum, &vrrpId) == L7_TRUE))
        {
            if (vrrpFuncTable.L7_vrrpOperVirtualMacAddrGet &&
                (vrrpFuncTable.L7_vrrpOperVirtualMacAddrGet(vrrpId, rxIntIfNum, vMacAddr)
                                                                          == L7_SUCCESS))
            {
                srcMac = vMacAddr;
            }
        }
    }

    /* Find best route to destination. Only allowed to respond to ARP
     * request if none of the router's next hops to destination is out the
     * interface where the ARP request was received, if local proxy ARP is
     * disabled on the interface.
     */

    /* LocalProxyARP always disabled for unnumbered interface */
    if (ipMapArpExtenIsUnnumbIntf(flags) == L7_TRUE )
    {
        localProxyArpMode = L7_FALSE;
    }
    else
    {
        localProxyArpMode = ipMapArpExtenLocalProxyArpEnabled(flags);
    }

    memcpy(&tempDstIP, arpFrame->dstIP, sizeof(arpFrame->dstIP)); 
    destAddr = osapiNtohl(tempDstIP);
    memcpy(&tempSrcIP, arpFrame->srcIP, sizeof(arpFrame->srcIP)); 
    srcIPAddr = osapiNtohl(tempSrcIP);

    if(destAddr == srcIPAddr)
    {
      /* never respond to a gratuitous ARP */
      return E_FAILED;
    }

    if (rtoBestRouteLookup(destAddr, &route, L7_FALSE) == L7_SUCCESS)
    {
      /* Have a route to the target IP. If local proxy ARP is not enabled,
       * must verify that no next hops are on the interface where the
       * ARP Request was received. */
      if (localProxyArpMode != L7_TRUE)
      {
        for (nh = 0; nh < route.ecmpRoutes.numOfRoutes; nh++)
        {
            if (route.ecmpRoutes.equalCostPath[nh].arpEntry.intIfNum == rxIntIfNum)
            {
               return E_FAILED;
            }
        }
      }

      /* Set up ARP response */
      destIP = osapiHtonl(destAddr);
      nimGetIntfL3MacAddress(rxIntIfNum, L7_NULL, rxIfMacAddr);
      memcpy(arpFrame->dstIP, arpFrame->srcIP, sizeof(t_IPAddr));
      memcpy(arpFrame->dstMAC, arpFrame->srcMAC, sizeof(t_MACAddr));
      memcpy(arpFrame->srcIP, &destIP, 4);
      memcpy(arpFrame->srcMAC, srcMac, sizeof(t_MACAddr));

      return E_OK;
    }

    return E_FAILED;
}

/*********************************************************************
 * @purpose         Receive ARP frame routine.
 *
 *
 * @param Id        @b{(input)}   object Id
 * @param p_Frame   @b{(input)}   ARP frame pointer
 * @param rxIntIfNum  @b{(input)}   rx internal interface number
 * @param flags     @b{(input)}   rx interface (ipMapArpIntf_t.index)
 *
 * @returns  E_OK   - success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARP_Receive(t_Handle Id, void *p_Frame, unsigned int rxIntIfNum, word flags)
{
   t_ARP       *p_A = (t_ARP *)Id;
   t_EthHeader ethHeader;
   t_ARPFrame  arpFrame;
   word        type;
   word        opCode;
   word        len;
   L7_uchar8   vrId;
   Bool        nullSrcIp = FALSE;
   t_IPAddr    nullIP;

   /* TRUE if ARP should respond to ARP Request. */
   Bool respond = FALSE;

   ASSERT(p_A);
   ASSERT(p_A->status == ARP_OBJ_STAT_Valid);
   ASSERT(p_Frame);

   memset(&nullIP, 0, sizeof(nullIP));
   F_SET_OWNER(p_Frame);
   /* check object state */
   if(p_A->state != ARP_STATE_ACTIVE)
   {
       F_Delete(p_Frame);
       return E_OK;
   }
   /* check interface validity */
   if (!p_A->p_if[flags])
   {
       F_Delete(p_Frame);
       return E_OK;
   }
   /* check interface readiness */
   if (p_A->p_if[flags]->addrValid != TRUE)
   {
       F_Delete(p_Frame);
       return E_OK;
   }
   /* copy ARP frame to the array*/
   if(F_Read((void *)&arpFrame, p_Frame, 0, (word)sizeof(arpFrame)) !=
      (word)sizeof(arpFrame))
   {
      F_Delete(p_Frame);
      return E_OK;
   }
   arpDebugPacketRxTrace(&arpFrame);
   /* validate ARP frame */
   if(arpFrame.hwLen != (byte)sizeof(t_MACAddr) ||
      arpFrame.prLen != (byte)sizeof(t_IPAddr))
   {
      /* wrong hardware or protocol address length */
      F_Delete(p_Frame);
      return E_OK;
   }
   L7_NTOHS(&arpFrame.hwType, &type);
   if(type != HW_ETHERNET)
   {
      /* wrong hardware type */
      F_Delete(p_Frame);
      return E_OK;
   }
   L7_NTOHS(&arpFrame.prType, &type);
   if(type != ETHERTYPE_IP)
   {
      /* wrong protocol type */
      F_Delete(p_Frame);
      return E_OK;
   }

   L7_NTOHS(&arpFrame.opCode, &opCode);

   if(memcmp(arpFrame.srcIP, &nullIP, IPADDRLENGTH) == 0)
   {
     nullSrcIp = TRUE;
   }

   if (! nullSrcIp)
   {
     /* If an ARP packet is received from a conflicting host, don't
      * process the packet further */
     if (detectIPAddrConflicts(p_A, &arpFrame.srcIP, &arpFrame.srcMAC, flags) == TRUE)
     {
       F_Delete(p_Frame);
       return E_OK;
     }
   }

   if(opCode == ARP_RESPONSE)
   {
     if (! nullSrcIp)
     {
       p_A->arpRepRecvCt++;               /* increment debug counter */

        /* Handle ARP response. may create new entry.
         * We don't handle ARP response with NULL source IP */
        _updateARPCache(p_A, &arpFrame.srcIP, &arpFrame.srcMAC, flags, TRUE );
     }
     else
     {
       /* Increment error count if we receive ARP Reply with
        * null source IP */
       ipMapArpCtx_g.inSrcDiscard++;
     }
   }
   else if(opCode == ARP_REQUEST)
   {
      t_ADDREnt *addrEnt;
      Bool dstAddrIsLocal = FALSE;
      t_MACAddr tVMAC;
      t_IPAddr dstIPCopy;

      p_A->arpReqRecvCt++;              /* increment debug counter */

      /* Check if the request is targetting a Virtual Router's IP address */
      if(vrrpFuncTable.vrrpMapGetVMac &&
         (vrrpFuncTable.vrrpMapGetVMac((L7_uchar8 *)arpFrame.dstIP, (L7_uchar8 *)tVMAC, &vrId) == L7_SUCCESS))
      {
         if (nullSrcIp)
         {
           /* Handle ARP probes received as per RFC 5227 by responding to those probes
            * to help others detect any conflicts with us. */ 
           memcpy(arpFrame.srcIP, arpFrame.dstIP, sizeof(t_IPAddr));
         }
         /* make copy of the target IP address field */
         memcpy(&dstIPCopy, arpFrame.dstIP, sizeof(t_IPAddr));

         /* Copy the frame's source fields to the dest fields for the reply */
         memcpy(arpFrame.dstIP, arpFrame.srcIP, sizeof(t_IPAddr));
         memcpy(&arpFrame.dstMAC, &arpFrame.srcMAC, sizeof(t_MACAddr));

         /* if target IP is virtual router, use virtual MAC and IP in reply */
         memcpy(&arpFrame.srcMAC, tVMAC, sizeof(t_MACAddr));
         memcpy(arpFrame.srcIP, &dstIPCopy, sizeof(t_IPAddr));
         respond = TRUE;
      }
      else
      {
          for (addrEnt = p_A->addrEnt; (addrEnt != (t_ADDREnt *)NULLP);
                addrEnt = addrEnt->next)
          {
              if (addrEnt->intfNum == flags)
              {
                  /* found ARP address entry for interface where request was received */
                  if (memcmp(arpFrame.dstIP, addrEnt->ipAddr, IPADDRLENGTH) == 0)
                  {
                      if (nullSrcIp)
                      {
                          /* Handle ARP probes received as per RFC 5227 by responding to those probes
                           * to help others detect any conflicts with us. */
                          memcpy(arpFrame.srcIP, arpFrame.dstIP, sizeof(t_IPAddr));
                      }
                      /* Local IP address matched target IP. Send ARP response */
                      memcpy(arpFrame.dstIP, arpFrame.srcIP, sizeof(t_IPAddr));
                      memcpy(arpFrame.dstMAC, arpFrame.srcMAC, sizeof(t_MACAddr));
                      memcpy(arpFrame.srcIP, addrEnt->ipAddr, sizeof(t_IPAddr));
                      memcpy(arpFrame.srcMAC, addrEnt->macAddr, sizeof(t_MACAddr));

                      dstAddrIsLocal = TRUE;
                      respond = TRUE;
                      break;
                  }
              }
          }

         if (!dstAddrIsLocal && !nullSrcIp)
         {
           /* Target IP address not on interface where ARP request 
           * received. Check if ProxyARP is to be done */
           if (ARP_ProxyArp(flags, &arpFrame) == E_OK)
             {
                 respond = TRUE;
             }
         }
      }

      if (respond)
      {
        /* Update ARP cache. If responding to ARP Request, then target is local or 
         * we are proxying. Likely to send traffic to this station, so create an 
         * entry for {srcIP, srcMAC} if one does not yet exist. Note that by this 
         * time src and dest have been swapped in the packet. */
        _updateARPCache(p_A, &arpFrame.dstIP, &arpFrame.dstMAC, flags, TRUE);


        /* Send ARP response */
        opCode = ARP_RESPONSE;
        L7_HTONS(&opCode, &arpFrame.opCode);
  
        /* transmit ARP response frame */
        if (F_Init(p_Frame) != E_OK)
        {
          F_Delete(p_Frame);
          return E_OK;
        }
  
        /* set ethernet header */
        memcpy(&ethHeader.dstMACAddr, &arpFrame.dstMAC, sizeof(t_MACAddr));
        memcpy(&ethHeader.srcMACAddr, &arpFrame.srcMAC, sizeof(t_MACAddr));
        type = ETHERTYPE_ARP;
        L7_HTONS(&type, &ethHeader.type);
        len = F_AddToEnd(p_Frame, (byte *)&ethHeader, (word)sizeof(ethHeader));
        ASSERT(len);
        len = F_AddToEnd(p_Frame, (byte *)&arpFrame, (word)sizeof(arpFrame));
        ASSERT(len);
        if(p_A->p_if[flags]->f_Transmit(p_A->p_if[flags]->lowId, p_Frame, flags) != E_OK)
            F_Delete(p_Frame);
        else
            p_A->arpRepSentCt++;          /* increment debug counter */
        return E_OK;
      }
      else if(!nullSrcIp)
      {
        /* Not responding to the ARP request. Can still update an existing 
         * entry in the cache for {sender IP, senderMAC}, but can't create
         * a new entry. */
        _updateARPCache(p_A, &arpFrame.srcIP, &arpFrame.srcMAC, flags, FALSE);
      }
   }
   
   F_Delete(p_Frame);
   return E_OK;
}


/*********************************************************************
 * @purpose        Look up an IP address in the ARP cache.
 *
 *
 * @param Id           @b{(input)}  object Id
 * @param ipAddr       @b{(input)}  IP address pointer
 * @param flags        @b{(input)}  Control flag
 * @param macAddr      @b{(output)}  request Id (fill out by ARP)
 *
 * @returns  E_OK      - success
 * @returns  E_FAILED  - IP address is not resolved to a MAC address
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARP_GetMACAddr(t_Handle  Id, t_IPAddr  *ipAddr, word flags,
                     t_MACAddr *macAddr)
{
   t_ARP *p_A = (t_ARP *)Id;
   e_Err e;
   t_ADR *p_Adr;

   ASSERT(p_A);
   ASSERT(p_A->status == ARP_OBJ_STAT_Valid);
   ASSERT(ipAddr);
   ASSERT(macAddr);

   /* try to find given address antry */
   e = ipMapArpExtenCacheFind(p_A->arpCacheId, (byte *)ipAddr, flags, &p_Adr);
   if(e != E_OK)
      return e; /* address not found */

   /* See if this is a temporary entry, created because an ARP resolution attempt
    * is in progress. */
   if(memcmp(p_Adr->macAddr, nullMacAddr, MACADDRLENGTH) == 0)
      return E_FAILED;
   memcpy(macAddr, p_Adr->macAddr, MACADDRLENGTH);
   return E_OK;
}

/*********************************************************************
 * @purpose           Address resolution request.
 *
 *
 * @param Id          @b{(input)}  object Id
 * @param ipAddr      @b{(input)}  IP address pointer (fill in by user)
 * @param userId      @b{(input)}  callback user Id or NULLP (fill in by user)
 * @param flags       @b{(input)}  control flags (port number etc.)
 *
 * @returns           E_OK      - success
 * @returns           E_FAILED  - MAC address doesn't exist 
 *
 * @notes   Seems kinda dumb to put an entry on the request list when 
 *          the caller has given a NULL callback. But necessary to free
 *          the callback params.
 *
  * @end
 * ********************************************************************/
e_Err ARP_SendARPReq(t_Handle Id, byte *ipAddr, t_Handle userId, word flags)
{
   t_ARP *p_A = (t_ARP *)Id;
   e_Err  e;
   t_ADR *p_Adr;
   t_RQE *p_Rqe;
   word   frameQueue;
   t_MACAddr *pMacNone = NULLP;

   ASSERT(p_A);
   ASSERT(p_A->status == ARP_OBJ_STAT_Valid);
   ASSERT(ipAddr);

   /* check object state */
   if(p_A->state != ARP_STATE_ACTIVE)
       return E_FAILED;
   /* check interface validity */
   if (!p_A->p_if[flags])
       return E_FAILED;
   /* check interface readiness */
   if (p_A->p_if[flags]->addrValid != TRUE)
       return E_FAILED;

   /* try to find given address entry */
   e = ipMapArpExtenCacheFind(p_A->arpCacheId, (byte *)ipAddr, flags, &p_Adr);
   if(e != E_OK)
   {
     /* No entry. Not currently resolved and resolution is not already in progress. Create
      * a temporary entry with a MAC address of all 0s. */
      p_Adr = _createAddrEntry(p_A, (t_IPAddr *)ipAddr, pMacNone, ARP_ENTRY_TYPE_DYNAMIC, flags);
      /* If cache is full, won't add dynamic entry. */
      if (p_Adr == NULL)
      {
        return E_FAILED;
      }

      /* create request entry */
      p_Rqe = (t_RQE *)XX_Malloc(sizeof(t_RQE ));
      if (p_Rqe == NULL)
      {
        return E_NOMEMORY;
      }
      memset(p_Rqe, 0, sizeof(t_RQE));
      p_Rqe->dstAddrId = p_Adr;
      p_Rqe->userId = userId;
      p_Rqe->l3addr.protocolId = 0;
      memcpy(p_Rqe->l3addr.lay3Addr, ipAddr, sizeof(t_IPAddr));
      XX_AddToDLList(p_Rqe, p_Adr->request);
      
      /* send ARP request */
      _sendARPReq(p_Adr, L7_TRUE);
      return E_OK; /* address resolution in progress */
   }

   /* There is a cache entry already. Because this function is only called from 
    * ipMapArpResolution() and that function only calls this one if the IP address
    * is not already resolved, we know that this entry is a temporary entry created
    * on a previous attempt to resolve this address. Verify that entry's MAC is zero.
    * If not, consider this an error. */
   if(memcmp(p_Adr->macAddr, nullMacAddr, MACADDRLENGTH) != 0)
   {
     L7_uchar8 targetIpAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
     L7_uint32 targetIpAddr = osapiHtonl(*(L7_ulong32*)ipAddr);
     osapiInetNtoa(targetIpAddr, targetIpAddrStr);
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
             "ARP_SendARPReq() unexpectedly found existing ARP cache entry for %s.",
             targetIpAddrStr);
     return E_FAILED;
   }

   /* We are waiting for the ARP response to a request already in progress;
        create a new request entry */
   for(frameQueue = 0, p_Rqe = p_Adr->request; p_Rqe; p_Rqe = p_Rqe->next)
       frameQueue++;
   if(frameQueue >= p_A->queueSize)    /* ARP_REQUESTS_QUEUE_SIZE */
   {
       return E_FAILED;
   }
   p_Rqe = (t_RQE *)XX_Malloc(sizeof(t_RQE ));
   if (p_Rqe == NULL)
   {
     return E_NOMEMORY;
   }
   memset(p_Rqe, 0, sizeof(p_Rqe));
   p_Rqe->dstAddrId = p_Adr;
   p_Rqe->userId = userId;
   p_Rqe->l3addr.protocolId = 0;
   memcpy(p_Rqe->l3addr.lay3Addr, ipAddr, sizeof(t_IPAddr));
   XX_AddToDLList(p_Rqe, p_Adr->request);
   
   return E_OK;
}

/*****************************************************************
* @purpose           Address resolution request.
*
* @param Id          @b{(input)}  ARP object instance
* @param *ipAddr     @b{(input)}  IP address pointer (fill in by user)
* @param *index      @b{(input)}  interface index ARP request is to be sent
*
* @returns           E_OK      - success
* @returns           E_FAILED  - MAC address doesn't exist
*
* @notes
*
* @end
******************************************************************/
e_Err ARP_SendARPReqNoRxmt(t_Handle Id, byte *ipAddr, word index)
{
  t_ARP *p_A = (t_ARP *)Id;
  t_ADR *p_Adr = NULL;
  L7_BOOL rxmt = L7_TRUE;
  L7_BOOL temporary = L7_FALSE;

  ASSERT(p_A);
  ASSERT(p_A->status == ARP_OBJ_STAT_Valid);
  ASSERT(ipAddr);

  /* check object state */
  if(p_A->state != ARP_STATE_ACTIVE)
    return E_FAILED;

  /* try to find given address entry */
  if(ipMapArpExtenCacheFind(p_A->arpCacheId, (byte *)ipAddr, index, &p_Adr) != E_OK)
  {
    /* ipAddr is not in the ARP cache. Create a temporary. Unlike the normal ARP 
     * request process, we don't put this temporary in the ARP cache. */
    p_Adr = XX_Malloc(sizeof(t_ADR));
    if (p_Adr)
    {
      memset(p_Adr, 0, sizeof(t_ADR));
      p_Adr->p_obj = p_A;
      p_Adr->status = ARP_ENT_STAT_Valid;
      memcpy(p_Adr->ipAddr, ipAddr, sizeof(ipAddr));
      p_Adr->intfNum = index;
      p_Adr->entryType = ARP_ENTRY_TYPE_DYNAMIC;
      temporary = L7_TRUE;
    }
  }
  if (p_Adr)
  {
    /* send ARP request */
    _sendARPReq(p_Adr, !rxmt);
    if (temporary)
    {
      XX_Free(p_Adr);
    }
    return E_OK;
  }

  return E_FAILED;
}

/*----------------------------------------------------------------
 * ROUTINE:    ARP_SendGratARP
 *
 * DESCRIPTION:
 *    Send a sequence of gratuitous ARPs.
 *
 * ARGUMENTS:
 *  Id         - ARP object instance
 *  ipAddr     - IP address to be sent as target IP
 *  ifIndex    - index of interface where grat ARP is to be sent
 *
 * RESULTS:
 *   E_OK      - success
 *   E_FAILED  - an error occurred
 *
 *----------------------------------------------------------------*/
e_Err ARP_SendGratARP(t_Handle Id, byte *ipAddr, word ifIndex)
{
   t_ADR *p_Adr;
   t_ARP *p_A = (t_ARP *)Id;

   ASSERT(p_A);
   ASSERT(p_A->status == ARP_OBJ_STAT_Valid);
   ASSERT(ipAddr);

   /* check object state */
   if(p_A->state != ARP_STATE_ACTIVE)
       return E_FAILED;

   /* verify that the interface is valid */
   if (!p_A->p_if[ifIndex])
       return E_FAILED;

   /* verify that the local interface has a valid IP and MAC address. */
   if (p_A->p_if[ifIndex]->addrValid != TRUE)
       return E_FAILED;

   /* Create an ARP address entry, but do not insert it in the ARP cache.
    * This entry is needed to retain information about the ARP request when
    * the ARP response timer expires. The ARP response timer will ensure that
    * we send a sequence of gratuitous ARPs, instead of just one.
    */
    p_Adr = _createGratArpAddrEntry(p_A, (t_IPAddr *)ipAddr, ifIndex);
    if (p_Adr == NULL)
        return E_NOMEMORY;

    /* send ARP request */
    if ( TIMER_StartSec(p_Adr->arpRspTimer, 0, FALSE,
       _gratArpTimeoutExp, (t_Handle)ipMapArpCtx_g.timerExp.taskId) != E_OK)
    {
        /* p_Adr won't be freed in timeout callback, so free here */
        XX_Free(p_Adr);
        return E_FAILED;
    }

   return E_OK;
}

/* ========================================================================== */
/* ==           API EXTENSIONS (PROVIDED BY BROADCOM CORPORATION)          == */
/* ========================================================================== */

/*----------------------------------------------------------------
 *
 *
 * @purpose
 *    Adjusts each ARP entry timeout to the current arpAgeTime value.
 *
 *
 * @param      @b{(input)}  arpId      - object Id
 * @param      @b{(input)}  newAgeime - new value for ARP ageout time (in seconds)
 *
 *
 * @returns    E_OK      - success
 * @returns    E_FAILED  - unsuccess
 *
 * @returns
 *   1. For simplicity, the ARP entry timeout timer is restarted with a
 *      value representing the delta between the remaining timeout amount
 *      and the (new) arpAgeTime setting.  For entries that are considered
 *      expired based on this new value, their timer is restarted with a
 *      value of 1 second to allow normal timeout processing to occur
 *      (almost) immediately.
 *
 *    mainly used whenever the arpAgeTime is changed so that the entries are
 *    aged out based on the new value.  Expired entries are aged out right
 *    away (i.e., within one second).
 *
 *
 *
 * @end
 *----------------------------------------------------------------*/
e_Err ARP_AdjustTimeout(t_Handle arpId, ulng newAgeTime)
{
   t_ARP *p_A = (t_ARP *)arpId;
   t_ADR *p_Adr;
   e_Err  e;
   ulng   entryAgeInSecs, deltaTimeout;

   /* walk through ARP cache, processing all non-permanent entries */
   for(e = ipMapArpExtenCacheGetFirst(p_A->arpCacheId, &p_Adr);
       e == E_OK;
       e = ipMapArpExtenCacheGetNext(p_A->arpCacheId, p_Adr, &p_Adr))
   {
     ASSERT(p_Adr);
     if (p_Adr->permanent == TRUE)
       continue;

     /* calculate difference between newAgeTime value and entry current
      * age (in seconds)
      *
      * if entry age is greater than or equal to newAgeTime, the entry is
      * considered 'expired', so use a new timeout of 1 second to allow
      * normal timeout processing to occur.
      */
     ipMapArpExtenEntryAgeCalc(p_Adr->timeStamp, &entryAgeInSecs);
     if (newAgeTime > entryAgeInSecs)
       deltaTimeout = newAgeTime - entryAgeInSecs;
     else
       deltaTimeout = 1;

     /* this function automatically restarts a running timer... */
     e = TIMER_StartSec(p_Adr->arpAgeTimer, deltaTimeout, FALSE,
                        _arpAgeTimeExp, (t_Handle)ipMapArpCtx_g.timerExp.taskId);
     ASSERT(e == E_OK);
   }

   return E_OK;
}




/*********************************************************************
 * @purpose          Deletes (oldest) entries from the ARP cache in excess of
 *                   the maximumallowed cache size.  Only dynamic
 *                   entries are removed.
 *
 *
 *
 * @param    arpId    @b{(input)}  object Id
 * @param    maxEnt   @b{(input)}  new maximum cache size
 *
 * @returns  E_OK      - success
 * @returns  E_FAILED  - Could not complete ARP trim operation
 *
 * @notes    1. When the table size is reduced, any dynamic entries in excess
 *              ofthe new size are discarded, starting with the oldest first,
 *              then any gateway entries if necessary.  Local and static
 *              entries are never discarded here.
 *
 * @end
 * ********************************************************************/
e_Err ARP_TrimCache(t_Handle arpId, word maxEnt)
{
   t_ARP *p_A = (t_ARP *)arpId;
   L7_arpCacheStats_t   arpStats;

   /* initialize the local count structure */
   if (ARP_GetCacheStats(arpId, &arpStats) != E_OK)
     return E_FAILED;

   /* NOTE:  Must use maxEnt parm value here, not arpStats.cacheMax */
   while((word)arpStats.cacheCurrent > maxEnt)
   {
     if (_deleteOldestAddrEntry(p_A, TRUE) != E_OK)
       return E_FAILED;

     if (ARP_GetCacheStats(arpId, &arpStats) != E_OK)
       return E_FAILED;
   }

   /* change maximum size value AFTER removing the excess entries (so the
    * table full checking code does not inadvertantly assert)
    *
    * NOTE:  Doing this here so it is under p_A->arpMutexLock protection
    */
   if (ipMapArpExtenCacheResize(maxEnt) != E_OK)
     return E_FAILED;

   return E_OK;
}





/*********************************************************************
 * @purpose            Clears the ARP table of dynamic (and gateway) entries for
 *                      the specified interface(s).
 *
 *
 * @param   arpId       @b{(input)}  o bject Id
 * @param   intf        @b{(input)}  interface whose entries should be purged,
 *                                      or 0 for all intfs
 * @param   allowGw     @b{(input)}  denotes whether gateway entries should
 *                                       be cleared as well
 *
 * @returns    E_OK      - succes
 * @returns    E_FAILED  - Could not complete ARP clear cache operation
 *
 * @notes      1. The ARP entries for local router interfaces and static ARP
 *                 entries are not affected by this command.
 *
 *             2. If the allowGw parm is set to TRUE, dynamic ARP entries
 *                labeled  as "gateway" entries are also removed from
 *                the ARP cache.
 *
 *
 * @end
 * ********************************************************************/
e_Err ARP_ClearCache(t_Handle arpId, word intf, Bool allowGw)
{
   t_ARP *p_A = (t_ARP *)arpId;
   t_ADR *p_Adr;
   t_ADR *nextAdr;
   e_Err e;

   /* purge all dynamic entries for the specified interface(s) */
   e = ipMapArpExtenCacheGetFirst(p_A->arpCacheId, &p_Adr);
   while ((e == E_OK) && p_Adr)
   {
     /* get next before deleting current */
     e = ipMapArpExtenCacheGetNext(p_A->arpCacheId, p_Adr, &nextAdr);

     /* skip all permanent ARP entries (i.e., local, static)
      * and skip gateway entries unless asked to delete gateways, too */
     if (!p_Adr->permanent &&
         (allowGw || (p_Adr->entryType != ARP_ENTRY_TYPE_GATEWAY)))
     {
     if ((intf == 0) || (p_Adr->intfNum == intf))
     {
       /* delete the entry (don't quit for individual failures) */
         (void)_deleteAddrEntry(p_A, p_Adr);
       }
     }
     p_Adr = nextAdr;
   } 

   return E_OK;
}




/*********************************************************************
 * @purpose           Retrieves various counts related to the ARP cache
 *
 *
 * @param arpId        @b{(input)}  object Id
 * @param *pStats      @b{(input)}  output location for storing ARP counts
 *
 * @returns  E_OK      - success
 * @returns  E_FAILED  - Could not complete operation
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARP_GetCacheStats(t_Handle arpId, L7_arpCacheStats_t *pStats)
{
   t_ARP *p_A = (t_ARP *)arpId;

   ASSERT(p_A);

   if (ipMapArpExtenCacheStatsGet(pStats) != L7_SUCCESS)
     return E_FAILED;

   return E_OK;
}

/*********************************************************************
 * @purpose               Stops all timers associated with the ARP entry
 *
 *
 * @param   arpId         @b{(input)}  object Id
 * @param  *p_adrEntry    @b{(input)}  pointer to ARP entry
 *
 * @returns   E_OK      - success
 * @returns   E_FAILED  - Could not complete operation
 *
 * @notes     1. This function is typically used in preparation for removing an
 *               ARP entry from the ARP cache.  It is important that the entry
 *               timers be stopped prior to its removal from the cache.
 *
 * @end
 * ********************************************************************/
e_Err ARP_StopEntryTimers(t_Handle arpId, t_Handle p_adrEntry)
{
   t_ARP *p_A = (t_ARP *)arpId;
   t_ADR *p_Adr = (t_ADR *)p_adrEntry;

   ASSERT(p_A);
   ASSERT(p_Adr);

   _stopAllEntryTimers(p_Adr);

   return E_OK;
}


/*********************************************************************
 * @purpose  Empty user  routines
 *
 *
 * @param         @b{(input)}
 * @param         @b{(input)}
 * @param         @b{(input)}
 * @param         @b{(input)}
 *
 * @param         @b{(output)}
 *
 * @returns  E_OK      - success
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err _emptyData(t_Handle Id, t_Handle p_Frame, word flags)
{
   F_Delete(p_Frame);
   return E_OK;
}
static e_Err _emptyNAKCallback(t_Handle userId, t_Handle regId)
{
   return E_OK;
}

/*********************************************************************
 * @purpose             Destroy address entry resources.
 *
 *
 * @param p_Addr        @b{(input)}  address entry pointer
 *
 * @returns  E_OK       - success
 * @returns  E_FAILED   - processing error
 *
 * @notes    
 *
 * @end
 * ********************************************************************/
e_Err ARP_DestroyAddrEntry(t_Handle p_adrEntry)
{
   t_ADR *p_Adr = (t_ADR *)p_adrEntry;
   t_RQE          *p_Rqe;
   t_ARPResInfo    arpRes;

   ASSERT(p_Adr);
   ASSERT(p_Adr->status == ARP_ENT_STAT_Valid);

   /* destroy timers */
   TIMER_Delete(p_Adr->arpAgeTimer);
   TIMER_Delete(p_Adr->arpRspTimer);
   p_Adr->arpAgeTimer = 0;
   p_Adr->arpRspTimer = 0;

   /* if someone was waiting for resolution of this IP address, invoke his callback to
    * say that resolution failed. */
   for(p_Rqe = p_Adr->request; p_Rqe != (t_RQE *)NULLP;)
   {
      t_RQE *tmp = p_Rqe;
      p_Rqe = p_Rqe->next;
      memset(&arpRes, 0, sizeof(t_ARPResInfo));
      arpRes.destAddr = tmp->l3addr;
      arpRes.lanNmb = p_Adr->intfNum;
      (void)ipMapArpResCallbackInvoke(tmp->userId, &arpRes);
      XX_Free(tmp);
   }
   p_Adr->request = NULL;

   p_Adr->status = ARP_ENT_STAT_Invalid;
   p_Adr->seqNum = ARP_SEQ_NUM_INVALID;
   return E_OK;
}

/*********************************************************************
 * @purpose            Delete address entry.
 *
 *
 * @param p_A          @b{(input)}  ARP object instance pointer
 * @param p_Addr       @b{(input)}  address entry pointer
 *
 * @returns  E_OK      - success
 * @returns  E_BADPARM - address entry does not exist
 * @returns  E_FAILED  - processing error
 *
 * @notes    
 *
 * @end
 * ********************************************************************/
static e_Err  _deleteAddrEntry(t_ARP *p_A, t_ADR *p_Adr)
{
   ASSERT(p_A);
   ASSERT(p_A->status == ARP_OBJ_STAT_Valid);
   ASSERT(p_Adr);
   ASSERT(p_Adr->status == ARP_ENT_STAT_Valid);

   return ipMapArpExtenCacheRemove(p_A->arpCacheId, p_Adr);
}

/*********************************************************************
 * @purpose              Delete oldest dynamic address entry.
 *
 *
 * @param p_A            @b{(input)}  ARP object instance pointer
 * @param allowGw        @b{(input)}  indicates whether removal of gateway
 *                                    entries is allowed
 *
 * @returns  E_OK        - success
 * @returns  E_BADPARM   - no removable addr entry exists
 * @returns  E_FAILED    - processing error
 *
 * @notes    Removal of any gateway entry only occurs if there are no dynamic
 *               entries currently in the ARP cache.
 *
 * @end
 * ********************************************************************/
static e_Err  _deleteOldestAddrEntry(t_ARP *p_A, Bool allowGw)
{
   ASSERT(p_A);
   ASSERT(p_A->status == ARP_OBJ_STAT_Valid);

   return ipMapArpExtenCacheOldestRemove(p_A->arpCacheId, allowGw);
}


/*********************************************************************
 * @purpose         Create address entry.
 *
 *
 * @param p_A       @b{(input)}  ARP object instance pointer
 * @param ipAddr    @b{(input)}  IP address pointer
 * @param macAddr   @b{(input)}  MAC address pointer or NULLP
 * @param entryType @b{(input)}  flag that given address entry has no time age
 *                                value
 *
 * @param flags     @b{(input)}  ARP entry control bitflag
 *
 * @param           @b{(output)}   address entry pointer or NULLP
 *
 * @returns         p_adr
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_ADR *_createAddrEntry(t_ARP *p_A, t_IPAddr *ipAddr, t_MACAddr *macAddr,
                        word entryType, word flags)
{
   t_ADR          adr, *p_Adr;
   e_Err           e;
   L7_arpCacheStats_t   arpStats;
   L7_uint32       currentTime;

   ASSERT(ipAddr);
   ASSERT(p_A);
   ASSERT(p_A->status == ARP_OBJ_STAT_Valid);

   /* if a dynamic entry and cache is full, drop the new entry. For other
    * types of ARP entries, see if we can make space. */
   if ((entryType == ARP_ENTRY_TYPE_DYNAMIC) && 
       ipMapArpExtenCacheFullCheck(p_A->arpCacheId))
   {
     return NULL;
   }

   /* create space in the ARP cache if it is currently full */
   while (ipMapArpExtenCacheFullCheck(p_A->arpCacheId) == L7_TRUE)
   {
     e = _deleteOldestAddrEntry(p_A, FALSE);
     if (e != E_OK)
     {
         return (t_ADR *)NULLP;
     }
   }

   if (ARP_GetCacheStats(p_A, &arpStats) != E_OK)
   {
     return (t_ADR *)NULLP;
   }

   /* If the cache size has hit the high threshold, winnow the arp cache
    * to make space for new entries. This is really an attempt to avoid
    * hash collisions in the hardware and therefore should be tied to 
    * the hardware capacity, not the application cache size. But hardware
    * capacity isn't readily available. */
   if(((arpStats.cacheCurrent * 100) / arpStats.cacheMax)
       >= ARP_PURGE_THRESHOLD_HIGH)
   {
     currentTime = osapiUpTimeRaw();
     /* It might happen that the purge operation didn't bring the cache count
      * to below the high threshold b'coz there were very few inactive entries in the cache.
      * In such a case, to ensure that we don't call the cpu-intensive purge operation for
      * every create operation that's called immediately, we space the 
      * consecutive purge operations ARP_PURGE_TIMEOUT/2 seconds apart( reducing frequency
      * in calls to ipMapArpExtenPurgeOldEntries() ).
      *
      * Frequency in reporting the hardware insertion failures is already handled 
      * in the hardware layer, so we need not check for lastPurgeOperationTime
      * when purging ARP cache in such a case */
      
     if((lastPurgeOperationTime == 0) ||
        ((currentTime - lastPurgeOperationTime) > (ARP_PURGE_TIMEOUT/2)))
     {
       L7_uint32 initialCacheSize = arpStats.cacheCurrent;
       L7_uint32 startTime = osapiTimeMillisecondsGet();

       ipMapArpExtenPurgeOldEntries();
       lastPurgeOperationTime = currentTime;

       (void)ARP_GetCacheStats(p_A, &arpStats);
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ARP_MAP_COMPONENT_ID, 
               "Purged old entries. Reduced ARP cache from %u to %u entries in %u msec.",
               initialCacheSize, arpStats.cacheCurrent, osapiTimeMillisecondsGet() - startTime);
     }
   }

   memset(&adr, 0, sizeof(t_ADR));
   memcpy(&adr.ipAddr, ipAddr, IPADDRLENGTH);
   if(macAddr)
      memcpy(&adr.macAddr, macAddr, MACADDRLENGTH);
   adr.intfNum = flags;
   adr.p_obj = p_A;
   adr.entryType = entryType;
   if ((entryType == ARP_ENTRY_TYPE_LOCAL) ||
       (entryType == ARP_ENTRY_TYPE_STATIC))
   {
     adr.permanent = TRUE;
     adr.timeStamp = 0;
   }
   else
   {
     adr.permanent = FALSE;
     adr.timeStamp = TIMER_SysTime();
   }

   adr.status = ARP_ENT_STAT_Valid;

   adr.seqNum = ARP_SeqNum++;
   if (ARP_SeqNum == ARP_SEQ_NUM_INVALID)
     ARP_SeqNum = ARP_SEQ_NUM_INIT;     /* avoid using ambiguous 0 value */

   /* insert entry into ARP cache
    *   -- use returned p_Adr value for subsequent references
    */
   e = ipMapArpExtenCacheInsert(p_A->arpCacheId, &adr, &p_Adr);
   if (e != E_OK)
   {
       return p_Adr;
   }
   ASSERT(p_Adr);
   ASSERT(p_Adr->status == ARP_ENT_STAT_Valid);

   /* create timers */
   e = TIMER_InitSec(1, p_Adr, &p_Adr->arpAgeTimer);
   e |= TIMER_InitSec(1, p_Adr, &p_Adr->arpRspTimer);

   /* check for resource failure(s) */
   if (e != E_OK)
   {
     /* set entry macAddr to null so delete won't cause user notification */
     memcpy(p_Adr->macAddr, nullMacAddr, sizeof(t_MACAddr));
     (void)_deleteAddrEntry(p_A, p_Adr);
     return (t_ADR *)NULLP;
   }

   return p_Adr;
}

/*********************************************************************
 * @purpose         Change the ARP entry's timeout
 *
 * @param p_Adr     @b{(input)}  address entry pointer
 * @param timeout   @b{(input)}  Changed ARP timeout
 *
 * @returns         E_OK
 *                  E_FAILED if an error occured
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ARP_SetTimer(t_ADR *p_Adr, L7_uint32 timeout)
{
  e_Err e = E_FAILED;

  if(p_Adr->arpAgeTimer)
  {
    e = TIMER_StartSec(p_Adr->arpAgeTimer, timeout,
                       FALSE, _arpAgeTimeExp, (t_Handle)ipMapArpCtx_g.timerExp.taskId);
  }
  return e;
}

/*----------------------------------------------------------------
 * ROUTINE:    _createGratArpAddrEntry
 *
 * DESCRIPTION:
 *    Create a temporary ARP addr object describing a gratuitous ARP. This
 *    entry is not inserted in the ARP cache.
 *
 * ARGUMENTS:
 *  p_A     - ARP object instance pointer
 *  ipAddr  - IP address pointer
 *  ifIndex   - index of interface where gratuitous ARP is to be sent
 *
 * RESULTS:
 *   address entry pointer or NULLP
 *
 * NOTES:
 *   1. Assumes caller has already locked the p_A->arpMutexLock semaphore.
 *
 *----------------------------------------------------------------*/
static t_ADR *_createGratArpAddrEntry(t_ARP *p_A, t_IPAddr *ipAddr, word ifIndex)
{
   t_ADR     *p_Adr;
   t_ADDREnt *addrEnt = NULLP;
   e_Err e;
   ulng localIpAddr = osapiHtonl(*(L7_uint32 *)ipAddr);

   ASSERT(ipAddr);
   ASSERT(p_A);
   ASSERT(p_A->status == ARP_OBJ_STAT_Valid);

   p_Adr = (t_ADR*) XX_Malloc(sizeof(t_ADR));
   if (p_Adr == NULL)
       return NULL;

   memset(p_Adr, 0, sizeof(t_ADR));
   memcpy(&p_Adr->ipAddr, &localIpAddr, IPADDRLENGTH);
   p_Adr->intfNum = ifIndex;
   p_Adr->p_obj = p_A;
   p_Adr->entryType = ARP_ENTRY_TYPE_GRAT;
   p_Adr->permanent = FALSE;
   p_Adr->timeStamp = TIMER_SysTime();
   p_Adr->status = ARP_ENT_STAT_Valid;
   p_Adr->seqNum = ARP_SeqNum++;

   if ((addrEnt = getLocalAddrEntry(p_A, (t_IPAddr *)&(p_Adr->ipAddr), p_Adr->intfNum)) != NULLP)
   {
      addrEnt->addrConflictStats.numOfConflictsDetected = 0;
   }

   if (ARP_SeqNum == ARP_SEQ_NUM_INVALID)
     ARP_SeqNum = ARP_SEQ_NUM_INIT;     /* avoid using ambiguous 0 value */

   /* create timers */
   /* The age time is not used. The entry is deleted after a fixed number
    * of transmissions. */
   e = TIMER_InitSec(1, p_Adr, &p_Adr->arpRspTimer);
   if (e != E_OK)
   {
       XX_Free(p_Adr);
       return NULL;
   }

   return p_Adr;
}

/*********************************************************************
 * @purpose           Log and raise an SNMP trap for the address
 *                    conflict detected
 *
 * @param ifIndex     @b{(input)}  ARP interface index number
 * @param ipAddr      @b{(input)}  IP address pointer
 * @param macAddr     @b{(input)}  MAC address pointer or NULLP
 * @param vrId        @b{(input)}  VRRP group Identifier
 * @param notifyFlag  @b{(input)}  Flag to indicate if notification
 *                                 needs to be done
 *
 * @returns   The ipAddr argument is passed in network byte order.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void trapAndLogConflict(word ifIndex, t_IPAddr *ipAddr,
                               t_MACAddr *macAddr,
                               L7_uchar8 vrId,
                               L7_BOOL notifyFlag)
{
   L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
   L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
   L7_uchar8 macAddrStr[20];
   L7_uint32 tmpIp, ip, intIfNum;
   L7_uchar8 *mac = (byte*) macAddr;

   if (l7utilsMacAddrHexToString(mac, sizeof(macAddrStr), macAddrStr) != L7_SUCCESS)
   {
     return;
   }
   memcpy(&tmpIp, ipAddr, sizeof(tmpIp));
   ip = osapiNtohl(tmpIp);
   osapiInetNtoa(ip, ipAddrStr);
   memset(ifName, 0, sizeof(ifName));
   ipMapArpExtenGetIntIfNum(ifIndex, &intIfNum);
   nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
   if (vrId)
   {
       L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_ARP_MAP_COMPONENT_ID,
               "IP address conflict on interface %s with VR %d Master IP address %s."
               " Conflicting host MAC address is %s.", ifName, (int)vrId, ipAddrStr, macAddrStr);
   }
   else
   {
       L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_ARP_MAP_COMPONENT_ID,
               "IP address conflict on interface %s for IP address %s."
               " Conflicting host MAC address is %s.", ifName, ipAddrStr, macAddrStr);
   }

   /* Send trap via SNMP agent */
   trapMgrSwitchIpAddrConflictTrap(ip, mac);

   if(notifyFlag)
   {
       ipMapAddrConflictNotify(intIfNum, ip, mac);
   }

  simIPAddrConflictDetectIPMacSet(ip, mac);
}

/*********************************************************************
 * @purpose           Return pointer to the interface local address
 *                    entry structure based on the ip address and
 *                    interface index arguments
 *
 * @param p_A         @b{(input)}  ARP object in
 * @param ipAddr      @b{(input)}  IP address pointer
 * @param ifIndex     @b{(input)}  ARP interface index number
 *
 * @returns           valid pointer to address object if found
 *                    NULLP if not found
 *
 * @notes  IP address argument is in network byte order
 *
 * @end
 * ********************************************************************/
static t_ADDREnt *getLocalAddrEntry(t_ARP *p_A, t_IPAddr *ipAddr,
                                    word ifIndex)
{
   t_ADDREnt   *addrEnt = NULLP;

   for (addrEnt = p_A->addrEnt; addrEnt != (t_ADDREnt *)NULLP;
        addrEnt = addrEnt->next)
   {
      if ((memcmp(ipAddr, addrEnt->ipAddr, IPADDRLENGTH) == 0) && 
          (addrEnt->intfNum == ifIndex))
      {
        return addrEnt;
      }
   }
   return NULLP;
}

/*********************************************************************
 * @purpose           Detect if IP address conflict exists
 *
 * @param p_A         @b{(input)}  ARP object in
 * @param ipAddr      @b{(input)}  IP address pointer
 * @param macAddr     @b{(input)}  MAC address pointer or NULLP
 * @param ifIndex     @b{(input)}  ARP interface index number
 *
 * @returns           TRUE if conflict is detected
 *                    FALSE if conflict is not detected
 *
 * @notes   IP address argument is in network byte order
 *
 * @end
 * ********************************************************************/
static Bool  detectIPAddrConflicts(t_ARP *p_A, t_IPAddr *ipAddr,
                                   t_MACAddr *macAddr, word ifIndex)
{
   t_ADDREnt   *addrEnt = NULLP;
   L7_uchar8   vrId;
   t_MACAddr   tVMAC;

   if ((addrEnt = getLocalAddrEntry(p_A, ipAddr, ifIndex)) != NULLP)
   {
     /* If ipAddr is local address but different MAC addr, another
      * station may be (mis)configured with our IP address. */
     if (memcmp(macAddr, addrEnt->macAddr, MACADDRLENGTH) != 0)
     {
       /* If address conflict is detected and conflict detect mode is enabled do:
        * (a) LOG a message
        * (b) Raise an SNMP trap
        * (c) Notify registrants of the detected conflict
        */
       trapAndLogConflict(ifIndex, ipAddr, macAddr, 0, L7_TRUE);
       addrEnt->addrConflictStats.numOfConflictsDetected++;
       return TRUE;
     }
   }

   if (vrrpFuncTable.vrrpMapGetVMac &&
      (vrrpFuncTable.vrrpMapGetVMac(*ipAddr, tVMAC, &vrId) == L7_SUCCESS))
   {
      /* Address conflict with one of the VRRP master's addresses
       * (primary or secondary) on this interface */
      if(memcmp(macAddr, tVMAC, MACADDRLENGTH) != 0)
      {
         trapAndLogConflict(ifIndex, ipAddr, macAddr, vrId, L7_FALSE);
         return TRUE;
      }
   }

   return FALSE;
}

/*********************************************************************
 * @purpose           Update ARP cache.
 *
 *
 * @param p_A         @b{(input)}  ARP object in
 * @param ipAddr      @b{(input)}  IP address pointer
   @param macAddr     @b{(input)}  MAC address pointer or NULLP
 * @param flags       @b{(input)}  control flag (interface number ect.
 * @param creat       @b{(input)}  If TRUE, may create a new entry
 *
 * @returns           void
 *
 * @notes             1. Assumes caller has already locked the
 *                       p_A->arpMutexLock semaphore.
 *
 * @end
 * ********************************************************************/
static void  _updateARPCache(t_ARP *p_A, t_IPAddr *ipAddr,
                             t_MACAddr *macAddr, word flags, Bool create)
{
   t_ADR          *p_Adr;
   e_Err           e;
   Bool            sendCallback = FALSE;
   e_L3amEvent     callbackEvent = ARP_EVNT_ModifiedEntry;
   t_ADDREnt       *addrEnt;   
   t_MACAddr       tVMAC;
   L7_uchar8       vrId;

   if((memcmp(macAddr, nullMacAddr, MACADDRLENGTH) == 0) ||
      ((*macAddr[0] & 1) == 1))
   {
      /* Don't add the following invalid addresses into the ARP cache:
       * (a) Address 00:00:00:00:00:00
       * (b) Any multicast address (the first bit in the first byte being 1)
       *     It includes the broadcast address FF:FF:FF:FF:FF:FF
       */
      return;
   }

   /* never put an address entry in the cache if it's already a local addr */
   for(addrEnt = p_A->addrEnt; addrEnt != (t_ADDREnt *)NULLP; addrEnt = addrEnt->next)
   {
      if ((memcmp(ipAddr, addrEnt->ipAddr, IPADDRLENGTH) == 0) && 
          (addrEnt->intfNum == flags))
      {
          return;
      }
   }

   /* Don't update the ARP cache for an IP address used by VRRP
    * as a virtual router address. */
   if (vrrpFuncTable.vrrpMapGetVMac &&
      (vrrpFuncTable.vrrpMapGetVMac(*ipAddr, tVMAC, &vrId) == L7_SUCCESS))
   {
      return; 
   }

   /* try to find address entry */
   if (ipMapArpExtenCacheFind(p_A->arpCacheId, (byte *)ipAddr, flags, &p_Adr) != E_OK) 
   {
     if (create)
     {
       p_Adr = _createAddrEntry(p_A, ipAddr, macAddr, ARP_ENTRY_TYPE_DYNAMIC, flags);
       if (p_Adr == NULL)
         return;    /* cache could be full */
     }
     else
     {
       return;   
     }
   }
   else if (p_Adr->permanent == TRUE)
   {
     /* don't let dynamic ARP info update an existing local or static entry */
   }
   else
   {
      t_RQE *p_Rqe;
      Bool  macHasChanged = FALSE;

      ASSERT(!memcmp(p_Adr->ipAddr, ipAddr, IPADDRLENGTH));

      /* update dynamic ARP address entry */
      if(memcmp(p_Adr->macAddr, macAddr, MACADDRLENGTH))
      {
          sendCallback = TRUE;
          if(!memcmp(p_Adr->macAddr, nullMacAddr, MACADDRLENGTH))
             callbackEvent = ARP_EVNT_NewEntry;

          memcpy(p_Adr->macAddr, macAddr, MACADDRLENGTH);
          macHasChanged = TRUE;
      }

      p_Adr->timeStamp = TIMER_SysTime();

      e = ipMapArpExtenCacheUpdate(p_A->arpCacheId, p_Adr, macHasChanged);
      ASSERT(e == E_OK);

      /* If someone is waiting for resolution of this IP address, invoke his resolution callback. */
      /* set p_Rqe to the end of DLList to send callback in a right order */
      for(p_Rqe = p_Adr->request; ; p_Rqe = p_Rqe->next)
      {
         if(p_Rqe == (t_RQE *)NULLP || p_Rqe->next == (t_RQE *)NULLP)
            break;
      }
      while(p_Rqe != (t_RQE *)NULLP)
      {
         t_RQE *tmp = p_Rqe;
         t_EthHeader header;
         t_ARPResInfo arpRes;

         word type = ETHERTYPE_IP;

         p_Rqe = p_Rqe->prev;

         memset(&header, 0, sizeof(header));
         header.type[0] = (byte)(type >> 8);
         header.type[1] = (byte)type;
         memcpy(&header.dstMACAddr, p_Adr->macAddr, MACADDRLENGTH);
         e = ARP_GetSrcMAC((t_Handle)p_Adr->p_obj, p_Adr->intfNum, &header.srcMACAddr);
         ASSERT(e == E_OK);

         /* send callback */
         arpRes.destAddr = tmp->l3addr;
         arpRes.dllHeaderSize = (byte)sizeof(header);
         arpRes.lanNmb = p_Adr->intfNum;
         memcpy(&arpRes.dllHeader, &header, sizeof(header));
         (void)ipMapArpResCallbackInvoke(tmp->userId, &arpRes);
         XX_Free(tmp);
      }
      p_Adr->request = (t_RQE *)NULLP;
   }
   /* stop timeout timer */
   if (p_Adr->arpRspTimer)
   {
     e = TIMER_Stop(p_Adr->arpRspTimer);
   }
   p_Adr->retrNmb = 0;

   /* restart ARP age timer */
   if(p_Adr->permanent != TRUE)
   {
       e = TIMER_StartSec(p_Adr->arpAgeTimer, TimerFractVarLessBy10(p_A->arpAgeTime), FALSE,
                          _arpAgeTimeExp, (t_Handle)ipMapArpCtx_g.timerExp.taskId);
       ASSERT(e == E_OK);
   }
}

/*********************************************************************
 * @purpose          ARP age timer callback.
 *
 *
 * @param adrId      @b{(input)}   address entry Id
 * @param timer      @b{(input)}  timer handle
 * @param flags      @b{(input)}  timer bit flag
 *
 * @returns          E_OK
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err _arpAgeTimeExp(t_Handle adrId, t_Handle timer, word flags)
{
   t_ADR          *p_Adr = (t_ADR *)adrId;
   t_ARP          *p_A;
   ulng         seqNum;
   ulng ipAddr;              /* IP address of entry aging out. Host byte order. */
   L7_uint32 tmp;
   IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

   /* try to establish ptrs to addr entry and ARP object elements
    * NOTE:  It is possible that addr entry was deleted before timer handler
    *        gets serviced.
    */
   if (!p_Adr || p_Adr->status != ARP_ENT_STAT_Valid)
   {
     IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
     return E_OK;
   }

   seqNum = p_Adr->seqNum;              /* save for comparison after getting lock */
   p_A = p_Adr->p_obj;
   if (!p_A || p_A->status != ARP_OBJ_STAT_Valid)
   {
     IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
     return E_OK;
   }

   /* re-check addr entry ptr after obtaining ARP lock to ensure still valid */
   if ((p_Adr->status != ARP_ENT_STAT_Valid) || (p_Adr->seqNum != seqNum))
   {
     IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
     return E_OK;
   }

   /* Determine if entry should be kept */
   memcpy(&tmp,p_Adr->ipAddr,sizeof(p_Adr->ipAddr));
   ipAddr = (L7_uint32)osapiNtohl(tmp);

   if (ipMapArpEntryKeep(ipAddr, p_Adr->interfaceNumber))
   {
      p_Adr->retrNmb = 0;
      _sendARPReq(p_Adr, L7_TRUE);
   }
   else
   {
      (void)_deleteAddrEntry(p_A, p_Adr);
   }

   IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

   return E_OK;
}

/*********************************************************************
 * @purpose          ARP response timeout callback.
 *
 *
 * @param drId       @b{(input)}  address entry Id
 * @param timer      @b{(input)}  timer handle
 * @param flags      @b{(input)}  timer bit flag
 *
 * @returns          E_OK  - success
 *
 * @notes            This function needs to take the component lock as it
 *                   runs in the ARP timer exp thread context
 *
 * @end
 * ********************************************************************/
static e_Err _arpTimeoutExp(t_Handle adrId, t_Handle timer, word flags)
{
   t_ADR *p_Adr = (t_ADR *)adrId;
   t_ARP *p_A;
   e_Err e;
   ulng  seqNum;

   IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

   /* try to establish ptrs to addr entry and ARP object elements
    * NOTE:  It is possible that addr entry was deleted before timer handler
    *        gets serviced.
    */
   if (!p_Adr || p_Adr->status != ARP_ENT_STAT_Valid)
   {
     IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
     return E_OK;
   }

   seqNum = p_Adr->seqNum;              /* save for comparison after getting lock */
   p_A = p_Adr->p_obj;
   if (!p_A || p_A->status != ARP_OBJ_STAT_Valid)
   {
     IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
     return E_OK;
   }

   /* re-check addr entry ptr after obtaining ARP lock to ensure still valid */
   if ((p_Adr->status != ARP_ENT_STAT_Valid) || (p_Adr->seqNum != seqNum))
   {
     IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
     return E_OK;
   }

   /* if entry has been marked as static, stop trying to resolve it */
   if (p_Adr->permanent == TRUE)
   {
     if (p_Adr->arpRspTimer)
     {
       e = TIMER_Stop(p_Adr->arpRspTimer);
     }
     p_Adr->retrNmb = 0;

     IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
     return E_OK;
   }

   if(p_Adr->retrNmb >= p_A->retrNmb)
   {
      /* delete address entry */
      e = _deleteAddrEntry(p_A, p_Adr);
      ASSERT(e == E_OK);
   }
   else                                 /* LVL7_MOD d08586 */
   {
     p_Adr->retrNmb++;
     _sendARPReq(p_Adr, L7_TRUE);
   }

   IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

   return E_OK;
}




/*********************************************************************
 * ROUTINE:    _gratArpTimeout
 *
 * DESCRIPTION:
 *    Gratuitous ARP response timeout callback. This is simply used
 *    to send a sequence of gratuitous ARPs, so that stations that
 *    miss one gratuitous ARP will have another chance to see it.
 *
 * ARGUMENTS:
 *  adrId   - address entry Id
 *  timer   - timer handle
 *  flags   - timer bit flag
 *
 * RESULTS:
 *   E_OK  - success
 *
 *----------------------------------------------------------------*/
static e_Err _gratArpTimeoutExp(t_Handle adrId, t_Handle timer, word flags)
{
   t_ADR *p_Adr = (t_ADR *)adrId;
   t_ARP *p_A;
   t_ADDREnt *addrEnt = NULLP;
   ulng  seqNum;

   IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

   if (!p_Adr || p_Adr->status != ARP_ENT_STAT_Valid)
   {
     IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
     return E_OK;
   }

   seqNum = p_Adr->seqNum;              /* save for comparison after getting lock */
   p_A = p_Adr->p_obj;
   if (!p_A || p_A->status != ARP_OBJ_STAT_Valid)
   {
     IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
     return E_OK;
   }

   /* re-check addr entry ptr after obtaining ARP lock to ensure still valid */
   if ((p_Adr->status != ARP_ENT_STAT_Valid) || (p_Adr->seqNum != seqNum))
   {
     IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
     return E_OK;
   }

   if ((addrEnt = getLocalAddrEntry(p_A, (t_IPAddr *)&(p_Adr->ipAddr), p_Adr->intfNum)) != NULLP)
   {
      /* If there are any address conflicts reported for this local IP 
       * address, stop sending gratuitous ARP retries for that interface.
       * This is to avoid repeated reporting of conflicts and moreover
       * we are sure that the grat ARP has reached other hosts in the LAN */
      if ((addrEnt->addrConflictStats.numOfConflictsDetected > 0) ||
          (p_Adr->retrNmb > p_A->retrNmb))
      {
         /* delete temporary address entry */
          TIMER_Delete(p_Adr->arpRspTimer);
          XX_Free(p_Adr);
      }
      else
      {
        p_Adr->retrNmb++;
        _sendGratARP(p_Adr);
      }
   }

   IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
   return E_OK;
}

/*----------------------------------------------------------------
 * @purpose       Forces all ARP entry timers to be stopped
 *
 *
 * @param p_Adr   @b{(input)}  address entry pointer
 *
 * @param         @b{(output)}  n/a
 *
 * @returns       E_OK
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void  _stopAllEntryTimers(t_ADR *p_Adr)
{
  e_Err e;

  if (p_Adr->arpRspTimer)
  {
    e = TIMER_Stop(p_Adr->arpRspTimer);
  }
  if (p_Adr->arpAgeTimer)
  {
    e = TIMER_Stop(p_Adr->arpAgeTimer);
  }
}





/*********************************************************************
 * @purpose       Broadcast ARP request frame.
 *
 *
 * @param  p_Adr  @b{(input)}  address entry pointer
 * @param  rxmt @b{(input)}  whether to start the arp resp timer
 *
 *
 * @returns  n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void  _sendARPReq(t_ADR *p_Adr, L7_BOOL rxmt)
{
   void           *p_Frame;
   t_EthHeader    ethHeader;
   t_ARPFrame     arpFrame;
   t_ARP          *p_A;
   word           type, opCode;
   e_Err          e;
   t_ADDREnt      *srcAddrEnt;
   t_ADDREnt      *primaryAddrEnt = NULL;   /* primary IP addr on interface where sending */
   byte           *pDstMac;
   word           len;
   L7_uchar8      vrId;

   ASSERT(p_Adr);
   ASSERT(p_Adr->status == ARP_ENT_STAT_Valid);
   p_A = p_Adr->p_obj;
   ASSERT(p_A);
   ASSERT(p_A->status == ARP_OBJ_STAT_Valid);
   if(!p_A->p_if[p_Adr->intfNum])
      return;

   /* Select src IP address. Try to find one in same subnet as target. */
   for(srcAddrEnt = p_A->addrEnt; srcAddrEnt; srcAddrEnt = srcAddrEnt->next)
   {
     if(srcAddrEnt->intfNum == p_Adr->intfNum)
     {
       /* Check if entry in same subnet as target IP. */
       if (ipMapArpExtenSameSubnet(p_Adr->intfNum, p_Adr->ipAddr, srcAddrEnt->ipAddr))
       {
         break;
       }
       if(ipMapArpExtenIpAddrIsPrimary(srcAddrEnt->intfNum, srcAddrEnt->ipAddr))
       {
         primaryAddrEnt = srcAddrEnt;
       }
     }
   }
  
   if(!srcAddrEnt)
   {  
     /* didn't find match in same subnet */
     if (primaryAddrEnt)
       /* use primary */
       srcAddrEnt = primaryAddrEnt;
     else
       /* no primary either. Punt. */
       return;
   }

   /* create frame */
   p_Frame = F_New((void *)NULLP);
   ASSERT(p_Frame);

#if 1 /* PTin Added - Routing support (add the dtl0 vlan to the ARP request packet) */
   {
     L7_uint32 internalVid = 0;
     L7_uint16 dtl0Vid = 0;
     L7_uint32 intfNum;
     L7_uint16 temp;
     L7_uint16 tpid;

     intfNum = ((ipMapArpIntf_t *)(p_A->p_if[p_Adr->intfNum]->lowId))->intIfNum;

     /* set ethernet header */
     if(memcmp(p_Adr->macAddr, nullMacAddr, MACADDRLENGTH) != 0)
        pDstMac = p_Adr->macAddr;         /* send as unicast */
     else
        pDstMac = bcstMacAddr;            /* send as broadcast */
     memcpy(ethHeader.dstMACAddr, pDstMac, MACADDRLENGTH);
     memcpy(ethHeader.srcMACAddr, srcAddrEnt->macAddr, MACADDRLENGTH);
     len = F_AddToEnd(p_Frame, (byte *)ethHeader.dstMACAddr, (word)MACADDRLENGTH);
     ASSERT(len);
     len = F_AddToEnd(p_Frame, (byte *)ethHeader.srcMACAddr, (word)MACADDRLENGTH);
     ASSERT(len);

     /* Add vlan */
     tpid = 0x8100;
     L7_HTONS(&tpid, &temp);
     len = F_AddToEnd(p_Frame, (byte *)&temp, (word)sizeof(L7_uint16));
     ASSERT(len);
     ipMapVlanRtrIntIfNumToVlanId(intfNum, &internalVid);
     dtl0Vid = ptin_ipdtl0_getdtl0Vid(internalVid);
     L7_HTONS(&dtl0Vid, &temp);
     len = F_AddToEnd(p_Frame, (byte *)&temp, (word)sizeof(L7_uint16));
     ASSERT(len);

     type = ETHERTYPE_ARP;
     L7_HTONS(&type, ethHeader.type);
     len = F_AddToEnd(p_Frame, (byte *)ethHeader.type, (word)sizeof(L7_uint16));
     ASSERT(len);
   }
#else
   /* set ethernet header */
   if(memcmp(p_Adr->macAddr, nullMacAddr, MACADDRLENGTH) != 0)
      pDstMac = p_Adr->macAddr;         /* send as unicast */
   else
      pDstMac = bcstMacAddr;            /* send as broadcast */
   memcpy(ethHeader.dstMACAddr, pDstMac, MACADDRLENGTH);
   memcpy(ethHeader.srcMACAddr, srcAddrEnt->macAddr, MACADDRLENGTH);
   type = ETHERTYPE_ARP;
   L7_HTONS(&type, ethHeader.type);
   len = F_AddToEnd(p_Frame, (byte *)&ethHeader, (word)sizeof(ethHeader));
   ASSERT(len);
#endif

   /* set ARP frame */
   memset(&arpFrame, 0, sizeof(arpFrame));
   type = HW_ETHERNET;
   L7_HTONS(&type, arpFrame.hwType);
   type = ETHERTYPE_IP;
   L7_HTONS(&type, arpFrame.prType);
   arpFrame.hwLen = (byte)MACADDRLENGTH;
   arpFrame.prLen = (byte)IPADDRLENGTH;
   opCode = ARP_REQUEST;
   L7_HTONS(&opCode, arpFrame.opCode);
#if LVL7_ORIGINAL_VRRP1       /* lvl7_VRRP1 start */
   memcpy(arpFrame.srcMAC, srcAddrEnt->macAddr, MACADDRLENGTH);
   memcpy(arpFrame.srcIP, srcAddrEnt->ipAddr, IPADDRLENGTH);
#else
{
   t_MACAddr tVMAC;
   memcpy(arpFrame.srcIP, srcAddrEnt->ipAddr, IPADDRLENGTH);
   /* check if the interface IP matches a Virtual Router's IP */
   if(vrrpFuncTable.vrrpMapGetVMac &&
      (vrrpFuncTable.vrrpMapGetVMac((L7_uchar8 *)arpFrame.srcIP, (L7_uchar8 *)tVMAC, &vrId) == L7_SUCCESS))
      /* if so, need to replace interface MAC with virtual MAC (VMAC) in request */
      memcpy(arpFrame.srcMAC, tVMAC, MACADDRLENGTH);
   else
      memcpy(arpFrame.srcMAC, srcAddrEnt->macAddr, MACADDRLENGTH);
}
#endif  /* lvl7_VRRP1*/
   memcpy(arpFrame.dstIP, p_Adr->ipAddr, IPADDRLENGTH);
   len = F_AddToEnd(p_Frame, (byte *)&arpFrame, (word)sizeof(arpFrame));
   ASSERT(len);

   /* transmit ARP request
    *
    * NOTE:  The called f_Transmit function becomes the owner of the p_Frame
    *        buffer and is responsible for freeing it.
    */
   if (p_A->p_if[p_Adr->intfNum] &&
       p_A->p_if[p_Adr->intfNum]->f_Transmit
           (p_A->p_if[p_Adr->intfNum]->lowId, p_Frame, p_Adr->intfNum) == E_OK)
   {
     p_A->arpReqSentCt++;            /* increment debug counter */
   }

   if(rxmt == L7_TRUE)
   {
     /* start ARP response timeout timer */
     e = TIMER_StartSec(p_Adr->arpRspTimer, p_A->arpRespTime, FALSE,
        _arpTimeoutExp, (t_Handle)ipMapArpCtx_g.timerExp.taskId);
     ASSERT(e == E_OK);
   }
}

/*----------------------------------------------------------------
 * ROUTINE:    _sendGratARP
 *
 * DESCRIPTION:
 *    Broadcast gratuitous ARP.
 *
 * ARGUMENTS:
 *  p_Adr   - address entry pointer
 *
 * RESULTS:
 *   E_OK if ARP sent
 *   E_FAILED if an error occurred
 *
 *----------------------------------------------------------------*/
static e_Err  _sendGratARP(t_ADR *p_Adr)
{
   void           *p_Frame;
   t_EthHeader    ethHeader;
   t_ARPFrame     arpFrame;
   t_ARP          *p_A;
   word           type, opCode;
   e_Err          e;
   t_ADDREnt      *srcAddrEnt;
   word           len;
   word           ifIndex;
   t_MACAddr tVMAC;
   L7_uchar8      vrId;

   ASSERT(p_Adr);
   ASSERT(p_Adr->status == ARP_ENT_STAT_Valid);
   p_A = p_Adr->p_obj;
   ASSERT(p_A);
   ASSERT(p_A->status == ARP_OBJ_STAT_Valid);
   ifIndex = p_Adr->intfNum;
   if(!p_A->p_if[ifIndex])
      return E_FAILED;

   /* find MAC address on the interface where grat ARP to be sent.
    * This will be used as the source MAC address in the ethernet header
    * and as the source MAC in the ARP request. */
   if ((srcAddrEnt = getLocalAddrEntry(p_A, (t_IPAddr *)&(p_Adr->ipAddr), p_Adr->intfNum)) == NULLP)
   {
       return E_FAILED;
   }

   /* Reset the number of conflicts detected for a local IP address
   * on an interface to 0 before sending gratuitous ARP */
   srcAddrEnt->addrConflictStats.numOfConflictsDetected = 0;

   /* create frame */
   p_Frame = F_New((void *)NULLP);
   ASSERT(p_Frame);

   /* set ethernet header */
   memcpy(ethHeader.dstMACAddr, bcstMacAddr, MACADDRLENGTH);  /* L2 broadcast */
   memcpy(ethHeader.srcMACAddr, srcAddrEnt->macAddr, MACADDRLENGTH);
   type = ETHERTYPE_ARP;
   L7_HTONS(&type, ethHeader.type);
   len = F_AddToEnd(p_Frame, (byte *)&ethHeader, (word)sizeof(ethHeader));
   ASSERT(len);

   /* set ARP frame */
   memset(&arpFrame, 0, sizeof(arpFrame));
   type = HW_ETHERNET;
   L7_HTONS(&type, arpFrame.hwType);
   type = ETHERTYPE_IP;
   L7_HTONS(&type, arpFrame.prType);
   arpFrame.hwLen = (byte)MACADDRLENGTH;
   arpFrame.prLen = (byte)IPADDRLENGTH;
   opCode = ARP_REQUEST;
   L7_HTONS(&opCode, arpFrame.opCode);

   /* Use virtual MAC addr, if necessary */
   memcpy(arpFrame.srcIP, srcAddrEnt->ipAddr, IPADDRLENGTH);
   /* check if the interface IP matches a Virtual Router's IP */
   if(vrrpFuncTable.vrrpMapGetVMac &&
      (vrrpFuncTable.vrrpMapGetVMac((L7_uchar8 *)arpFrame.srcIP, (L7_uchar8 *)tVMAC, &vrId) == L7_SUCCESS))
      /* if so, need to replace interface MAC with virtual MAC (VMAC) in request */
      memcpy(arpFrame.srcMAC, tVMAC, MACADDRLENGTH);
   else
      memcpy(arpFrame.srcMAC, srcAddrEnt->macAddr, MACADDRLENGTH);

   memcpy(arpFrame.dstIP, p_Adr->ipAddr, IPADDRLENGTH);
   /* Target hardware address is generally ignored but by convention is
    * set to all 0s in Gratuitous ARP request */
   memset(arpFrame.dstMAC, 0, MACADDRLENGTH);
   len = F_AddToEnd(p_Frame, (byte *)&arpFrame, (word)sizeof(arpFrame));
   ASSERT(len);

   /* transmit ARP request
    *
    * NOTE:  The called f_Transmit function becomes the owner of the p_Frame
    *        buffer and is responsible for freeing it.
    */
   if (p_A->p_if[ifIndex]->f_Transmit
           (p_A->p_if[ifIndex]->lowId, p_Frame, ifIndex) == E_OK)
   {
     p_A->arpReqSentCt++;            /* increment debug counters */
     p_A->gratArpSentCt++;
   }

   /* start ARP response timeout timer */
   e = TIMER_StartSec(p_Adr->arpRspTimer, p_A->arpRespTime, FALSE,
      _gratArpTimeoutExp, (t_Handle)ipMapArpCtx_g.timerExp.taskId);
   if (e != E_OK)
   {
       /* p_Adr won't be freed in timeout callback, so free here */
       XX_Free(p_Adr);
       return e;
   }

   return E_OK;
}

void ARP_ObjectInfoShow(t_Handle p_Obj)
{
  t_ARP *p_A = (t_ARP *)p_Obj;
  t_ADDREnt *addrEnt;
  t_ARPIf   *p_if;
  ulng      i;


  printf("\nVendor ARP Object Contents:\n");
  printf("  status         = 0x%4.4x\n", p_A->status);
  printf("  state          = %u\n", p_A->state);
  printf("  userId         = 0x%8.8lx\n", (ulng)p_A->userId);
  printf("  *addrEnt       = 0x%8.8lx\n", (ulng)p_A->addrEnt);
  printf("  arpAgeTime     = %u\n", p_A->arpAgeTime);
  printf("  arpRespTime    = %u\n", p_A->arpRespTime);
  printf("  retrNmb        = %u\n", p_A->retrNmb);
  printf("  arpCacheSize   = %u\n", p_A->arpCacheSize);
  printf("  dynamicRenew   = %u\n", p_A->dynamicRenew);
  printf("  f_NAK          = 0x%8.8lx\n", (ulng)p_A->f_NAK);
  printf("  *p_if          = 0x%8.8lx\n", (ulng)p_A->p_if);
  printf("  queueSize      = %u\n", p_A->queueSize);
  printf("  ARP Stats:\n");
  printf("    request recv = %lu\n", p_A->arpReqRecvCt);
  printf("    reply recv   = %lu\n", p_A->arpRepRecvCt);
  printf("    request sent = %lu\n", p_A->arpReqSentCt);
  printf("    reply sent   = %lu\n", p_A->arpRepSentCt);
  printf("    grat ARP sent = %lu\n", p_A->gratArpSentCt);
  printf("    early deletes = %lu\n", p_A->earlyDeletes);

  printf("  \nAddress Entries:\n");
  addrEnt = p_A->addrEnt;
  if (addrEnt == (t_ADDREnt *)NULLP)
    printf("    (none)\n");
  else
  {
    while (addrEnt != (t_ADDREnt *)NULLP)
    {
      printf("    intf=%4lu:  macaddr=", (ulng)addrEnt->intfNum);
      for (i = 0; i < MACADDRLENGTH-1; i++)
      {
        printf("%2.2x:", addrEnt->macAddr[i]);
      }
      printf("%2.2x  ", addrEnt->macAddr[i]);
      printf("ipaddr=%u.%u.%u.%u\n",
             addrEnt->ipAddr[0], addrEnt->ipAddr[1],
             addrEnt->ipAddr[2], addrEnt->ipAddr[3]);
      addrEnt = addrEnt->next;
    }
  }

  printf("  \nInterface List:\n");
  for (i = 0; i < (ulng)MAX_INTERFACES; i++)
  {
    p_if = p_A->p_if[i];
    if ((p_if != (t_ARPIf *)NULLP) &&
        (p_if->number != 0))
    {
      printf("    intf=%4lu:  f_Transmit=0x%8.8lx  lowid=0x%8.8lx\n",
             (ulng)p_if->number, (ulng)p_if->f_Transmit, (ulng)p_if->lowId);
    }
  }

  printf("\nARP_SeqNum:  %lu\n", ARP_SeqNum);

  printf("\n");
}

/*******************end of file************************************/
