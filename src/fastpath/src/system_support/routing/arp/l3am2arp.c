/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename   l3am2arp.c
 *
 * @purpose     Layer 3 Arp Manager object interface to ARP object
 *
 * @component   ARP Component
 *
 * @comments    External Routines:
 *
 *              L3AM_Init
                L3AM_Lay3ARPResolution
 *
 * @create      11/01/1999
 *
 * @author     Alex Osinsky
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\system_support\routing\arp\l3am2arp.c 1.1.3.1 2002/02/13 23:03:25EST ssaraswatula Development  $";
#endif



#include <string.h>

#include "std.h"
#include "local.h"
#include "arp.h"

/* --- external object interfaces --- */

#include "xx.ext"
#include "frame.ext"


/* --- specific include files --- */

#include "l3am.ext"


/* --- Internal (static) function prototypes --- */




/*********************************************************************
 * @purpose               L3AM_Init
 *
 *
 * @param  name           @b{(input)} pointer to ASCII string with the L3AM
 *                                   object's name needed for tracing purposes (may be
 *                                                            NULLP)
 * @param  name           @b{(input)} L3AM object flags
 * @param  flags          @b{(input)}  maximum length of queue of packets with
 *                                      pending layer 3 ARP resolution
 * @param  ul2eQueueLen   @b{(input)} thread Id / processor Id L3AM object
 *                                     belongs to(may be NULLP)
 *
 * @param  sysLabel       @b{(input)} pointer to return handle of successfully
 *                                        created L3AM object
 *
 * @returns  E_OK         - success
 * @returns  not E_OK     -  request failed, look at std.h for definitions of
 *                             all possible causes of failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err L3AM_Init(IN char         *name,
                IN word          flags,
                IN word          ul2eQueueLen,
                IN t_SysLabel   *sysLabel,
                IN t_L3AMConfig *l3AMConfig,
                IN t_Handle     *p_l3amHandle)
{
    t_ARPCfg arpCfg;
    t_ARPCfg *p_Prm = NULLP;

    if(l3AMConfig)
    {
      memset(&arpCfg, 0 ,sizeof(t_ARPCfg));
      arpCfg.arpAgeTime = l3AMConfig->arpAgeTime;
      arpCfg.arpRespTime = l3AMConfig->arpRespTime;
      arpCfg.arpCacheSize = l3AMConfig->arpCacheSize;
      arpCfg.retrNmb = l3AMConfig->retrNmb;
      p_Prm = &arpCfg;
    }
    return ARP_Init(p_Prm, (t_Handle)NULLP, (t_Handle)NULLP, ul2eQueueLen,
 p_l3amHandle);
}




/*********************************************************************
 * @purpose    L3AM_Lay3ARPResolution
 *
 *
 * @param   l3amHandle      @b{(input)} handle returned by the L3AM_Init,
 * @param   userId          @b{(input)} user Id to be passed to the
 *                                         arpResIndCallback
 * @param   reqId           @b{(input)} reques t Id to be passed to the
 *                                         arpResIndCallback
 * @param   rpResInfo       @b{(input)}  RP Resolution request /response
 *
 * @param  rpResIndCallback @b{(input)} callback to the requestor on ARP
 *                                             Resolution response
 *
 * @returns   E_OK          - success
 * @returns   E_BUSY        - request was successfully initiated and pending,the
 *                              callback will be called when it is finished
 * @returns   not E_OK      - request failed, look at std.h for definitions of
 *                              all possible causes of failure
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err L3AM_Lay3ARPResolution (IN t_Handle   l3amHandle,
                              IN t_Handle   userId,
                              IN t_Handle   reqId,
                              IN OUT t_ARPResInfo *arpResInfo,
                              IN L3AM_ARP_RES_IND arpResIndCallback)
{
    F_ARPNAKCallback  fNakNull = NULLP;
    t_MACAddr   dstMac;
    t_MACAddr   srcMac;
    t_EthHeader header;
    e_Err       e;
    word        type = ETHERTYPE_IP;

    if(ARP_GetMACAddr(l3amHandle, (t_IPAddr *)arpResInfo->destAddr.lay3Addr,
        arpResInfo->lanNmb, &dstMac) == E_OK)
    {
        memset(&header, 0, sizeof(t_EthHeader));
        header.type[0] = (byte)(type >> 8);
        header.type[1] = (byte)type;
        memcpy(&header.dstMACAddr, dstMac, sizeof(t_MACAddr));
        if(ARP_GetSrcMAC(l3amHandle, (word)arpResInfo->lanNmb, &srcMac) == E_OK)
        {
           memcpy(&header.srcMACAddr, srcMac, sizeof(t_MACAddr));
           arpResInfo->dllHeaderSize = (byte)sizeof(t_EthHeader);
           memcpy(arpResInfo->dllHeader, &header, sizeof(t_EthHeader));
           return E_OK;
        }
    }
    e =  ARP_SendARPReq(l3amHandle, arpResInfo->destAddr.lay3Addr, reqId, userId, 
                        arpResIndCallback, fNakNull, (word)arpResInfo->lanNmb);
    if(e == E_OK)
        return E_BUSY;
    return e;
}

/*----------------------------------------------------------------
 * ROUTINE:    L3AM_GratArpSend
 *
 * DESCRIPTION:
 *   Triggers the transmission of a gratuitous ARP for a local IP address.
 *
 * ARGUMENTS:
 *  l3amHandle        - ARP instance
 *  arpResInfo    - gratuitous ARP request 
 *
 * RESULTS:
 *  E_OK          - success
 *  not E_OK      - request failed
 *----------------------------------------------------------------*/
e_Err L3AM_GratArpSend(IN t_Handle l3amHandle,
                       IN t_ARPResInfo *arpResInfo)
{
    e_Err e;
    t_ARP       *p_A = (t_ARP *)l3amHandle;

    e = ARP_SendGratARP(p_A, arpResInfo->destAddr.lay3Addr, 
                        (word)arpResInfo->lanNmb);
    return e;
}

/* The following are simple wrappers used to protect the called ARP function from 
 * undesired effects attributed to multi-threading
 */




/*********************************************************************
 * @purpose              Adds layer 2 interface to L3AM object
 *
 *
 * @param   mHandle      @b{(input)}  handle of L3AM object returned by
 *                                     L3AM_Init,
 * @param f_L2Transmit   @b{(input)}  layer 2 transmit function pointer,
 * @param l2Handle       @b{(input)}  handle of layer 2 object,
 * @param ifNumber       @b{(input)}  logical layer 2 interface number
 *
 *  @param l3amHandle    @b{(input)}  handle of L3AM object returned by
 *                                      L3AM_Init,
 *
 * @returns  E_OK        - success
 * @returns  not E_OK    - request failed, look at std.h for definitions of
 *                           all  possible causes of failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err L3AM_AddInterface( IN t_Handle         l3amHandle,
                         IN L3AM_L2_TRANSMIT f_L2Transmit, 
                         IN t_Handle         l2Handle,
                         IN t_LIH            ifNumber)
{
  e_Err         e;

  e = ARP_AddInterface(l3amHandle, (F_ARPTransmit)f_L2Transmit, l2Handle, ifNumber);
  return e;
}




/*********************************************************************
 * @purpose           Deletess an interface from the L3AM object
 *
 *
 * @param    l3amHandle      @b{(input)}  handle of L3AM object returned by
 L3AM_Init,
 * @param    ifNumber        @b{(input)}  logical layer 2 interface number
 *
 * @returns  E_OK          - success
 * @returns  not E_OK      - request failed, look at std.h for definitions of
 *                           allpossible causes of failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err L3AM_DelInterface( IN t_Handle         l3amHandle,
                         IN t_LIH            ifNumber)
{
  e_Err         e;

  e = ARP_DelInterface(l3amHandle, ifNumber);
  return e;
}



/*********************************************************************
 * @purpose   Deallocates all resources allocated for the
 *            L3AM object and destroy the object.
 *
 *
 * @param p_l3amHandle         @b{(output)}  pointer to the L3AM object handle
 *                                            returned by the L3AM_Init zeroed
 on success
 *
 * @returns  E_OK          - success
 * @returns  not E_OK      - request failed, look at std.h for definitions of
 *                           all possible causes of failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err L3AM_Destroy (IN OUT t_Handle   *p_l3amHandle)
{
  e_Err         e;

  e = ARP_Delete(p_l3amHandle);
  return e;
}


/*********************************************************************
 * @purpose    Associates local layer 3 address - layer 2 address binding with
 *              L3AM object
 *
 *
 * @param l3amHandle   @b{(input)} handle of L3AM object returned by L3AM_Init,
 * @param p_l3Addr     @b{(input)} pointer to layer 3 address,
 * @param p_l2Addr     @b{(input)} pointer to layer 2 address
 * @param flags        @b{(input)} control bit flag (e.g., interface number)
 *
 * @returns       E_OK          - success
 * @returns       not E_OK      - request failed, look at std.h for definitions
 *                                 of all possible causes of failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err L3AM_AddLocalAddress ( IN t_Handle l3amHandle,  IN t_Lay3Addr *p_l3Addr,
 IN byte *p_l2Addr,
                             IN word flags)
{
  e_Err         e;

  e = ARP_AddLocAddr(l3amHandle, (t_IPAddr *)p_l3Addr->lay3Addr, (t_MACAddr *)p_l2Addr, flags);
  return e;
}




/*********************************************************************
 * @purpose     Disassociates local layer 3 address - layer 2 address binding
 *                from L3AM object
 *
 * @param l3amHandle     @b{(input)}  handle of L3AM object returned by
 *                                    L3AM_Init,
 * @param p_l3Addr       @b{(input)}  pointer to layer 3 address
 * @param p_l2Addr       @b{(input)}  pointer to layer 2 address
 * @param flags          @b{(input)}  Control bit flag (interface number)
 *
 * @returns   E_OK          - success
 * @returns   not E_OK      - request failed, look at std.h for definitions of
 *                            all possible causes of failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err L3AM_DelLocalAddress ( IN t_Handle l3amHandle,  IN t_Lay3Addr *p_l3Addr,
 IN byte *p_l2Addr, word flags)
{
  e_Err         e;

  e = ARP_DelLocAddr(l3amHandle, (t_IPAddr *)p_l3Addr->lay3Addr, flags);
  return e;
}



/*********************************************************************
 * @purpose      Adds static ARP Table entry. 
 *
 *
 * @param l3amHandle          @b{(input)}   handle of L3AM object returned by
 *                                          L3AM_Init,
 * @param statARPInfo         @b{(input)}  static AT entry information,
 *
 * @returns   E_OK          - success
 * @returns   not E_OK      - request failed, look at std.h for definitions of
 *                            all possible causes of failure
 *
 * @notes
 *           These entries are of the highest
 *           priority and are looked up before dynamic ARP Table entries
 *           created by the ARP engine.
 *           These entries are not aging out and are removed only via
 *           L3AM_DeleteStaticATEntry
 * 
 *
 * @end
 * ********************************************************************/
e_Err L3AM_AddStaticATEntry ( IN t_Handle l3amHandle,  IN t_ARPStatInfo
 *statARPInfo)
{
  e_Err         e;

  e = ARP_AddDstAddr(l3amHandle, (t_IPAddr *)statARPInfo->l3Addr.lay3Addr,
                     (t_MACAddr *)statARPInfo->l2Addr, (word)statARPInfo->lanNmb);
  return e;
}




/*********************************************************************
 * @purpose               Deletes static ARP Table entry.
 *
 *
 * @param    mHandle      @b{(input)}  handle of L3AM object returned by
 *                                     L3AM_Init,
 * @param    statARPInfo  @b{(input)}  static AT entry information,
 *
 * @returns   E_OK          - success
 * @returns   not E_OK      - request failed, look at std.h for definitions of
 *                            all  possible causes of failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
 e_Err L3AM_DeleteStaticATEntry( IN t_Handle l3amHandle,  IN t_ARPStatInfo
 *statARPInfo)
{
  e_Err         e;

  e = ARP_DelDstAddr(l3amHandle, (t_IPAddr *)statARPInfo->l3Addr.lay3Addr, statARPInfo->lanNmb);
  return e;
}


/*********************************************************************
 * @purpose       Registers user application to be notified about ARP Table
 *                 asynchronous events
 *
 *
 * @param l3amHandle         @b{(input)}   handle of L3AM object returned by
 *                                         L3AM_Init
 * @param name               @b{(input)}   pointer to ASCII string with the
 *                                         user's name,needed for tracing
 *                                         purposes (may be NULLP)
 * @param userId             @b{(input)}   user ID to be passed to the
 *                                         eventCallback
 * @param eventCallback      @b{(input)}   callback to be called on ARP Table
 *                                         events
 * @param p_userHandle       @b{(output)}   pointer to return the handle
 *                                         associated with the user
 *
 * @returns  E_OK          - success
 * @returns  not E_OK      - request failed, look at std.h for definitions of al
 *                           l  possible causes of failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err L3AM_RegisterUser (IN t_Handle l3amHandle,  IN char  *name,  IN t_Handle
 userId,
                         IN L3AM_ASYNC_EVENT_CALLBACK eventCallback,
                         OUT t_Handle *p_userHandle)
{
  e_Err         e;

  e = ARP_RegUser(l3amHandle, userId, (F_ARPEventCallback)eventCallback, p_userHandle);
  return e;
}



/*********************************************************************
 * @purpose            Unregisters user application to be notified about
 *                     ARP Table asynchronous events
 *
 *
 * @param l3amHandle          @b{(input)}    handle of L3AM object returned by
 *                                           L3AM_Init
 * @param p_userHandle        @b{(output)}  pointer to a handle assigned to the
 *                                           user via L3AM_RegisterUser,
 *                                           zeroed on success
 *
 * @returns  E_OK          -  success
 * @returns   not E_OK     - request failed, look at std.h for definitions of
 *                            all possible causes of failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err L3AM_UnregisterUser(IN t_Handle l3amHandle, IN OUT t_Handle *p_userHandle)
{
  e_Err         e;

  e = ARP_UnregUser(p_userHandle);
  return e;
}


/*********************************************************************
 * @purpose              Processes a received ARP packet, either request or
 *                        response, and updates the ARP cache with the
 *                        sender's addressing information.
 *
 *
 * @param  l3amHandle        @b{(input)}  handle of L3AM object returned by
 *                                        L3AM_Init,
 * @param  p_arpPacket       @b{(input)}  pointer to start of packet ARP header
 * @param  flags             @b{(input)}  control flags (e.g., interface number)
 *
 * @returns  E_OK          - success
 *
 * @returns  not E_OK      - request failed, look at std.h for definitions of
 all
 *
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err L3AMX_ReceiveArpPacket ( IN t_Handle l3amHandle, IN void * p_arpPacket, IN
 word flags)
{
  e_Err         e;

  e = ARP_Receive(l3amHandle, p_arpPacket, flags);
  return e;
}



/*********************************************************************
 * @purpose              Adjusts each ARP entry timeout to the current
 *                       arpAgeTime value.
 *
 *
 * @param l3amHandle         @b{(input)}  handle of L3AM object returned by
 *                                        L3AM_Init,
 * @param newAgeTime         @b{(input)} new value for ARP ageout time (in
 *                                        seconds)
 *
 * @returns  E_OK          - success
 * @returns  not E_OK      - request failed, look at std.h for definitions of
 *                           all possible causes of failure
 *
 * @notes
 *                       This is mainly used whenever
 *                       the arpAgeTime is changed so that the entries are
 *                       aged out based on the new value.  Entries older
 *                       than the new arpAgeTime are
 *                       aged out right away (i.e., within one second).
 *
 * @end
 * ********************************************************************/
e_Err L3AMX_AdjustArpTimeout ( IN t_Handle l3amHandle, IN ulng newAgeTime)
{
  e_Err         e;

  e = ARP_AdjustTimeout(l3amHandle, newAgeTime);
  return e;
}




/*********************************************************************
 * @purpose           Removes entries from the ARP cache until the current
 *                    ARP size is within the specified maxEnt value.
 *
 *
 * @param l3amHandle     @b{(input)}  handle of L3AM object returned by
 *                                        L3AM_Init,
 * @param maxEnt         @b{(input)}  new maximum ARP cache size
 *
 * @returns  E_OK          - success
 * @returns  not E_OK      - request failed, look at std.h for definitions of
 *                           all possible causes of failure
 *
 * @notes          
 *                    The oldest dynamic entries are removed.
 *                    This allows the ARP cache size to be changed
 *                    dynamically while continuing proper ARP cache
 *                    management operation.
 *
 *
 * @end
 * ********************************************************************/
e_Err L3AMX_TrimArpCache ( IN t_Handle l3amHandle, IN word maxEnt)
{
  e_Err         e;

  e = ARP_TrimCache(l3amHandle, maxEnt);
  return e;
}




/*********************************************************************
 * @purpose          Clears the ARP table of dynamic (and optionally gateway)
 *                   entries for the specified interface(s).
 *                   Does not affect local or static ARP entries.
 *
 *
 * @param   13mHandle @b{(input)}  handle of L3AM object returned by
 *                                    L3AM_Init,
 * @param   intf      @b{(input)}  interface whose entries should be purged, or
 *                                    0 for all intfs
 * @param   allowGw    @b{(input)}  denotes whether gateway entries should be
 *                                    cleared as well
 * @param              @b{(input)}  succesnterface whose entries should be
 purged, or
 *                                    0 for all intfs
 *
 * @returns   E_OK         - succesnterface whose entries should be purged, or
 *                            0 for all intfs
 * @returns  not E_OK      -  request failed, look at std.h for definitions of
 *                            allpossible causes of failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err L3AMX_ClearArpCache ( IN t_Handle l3amHandle, IN word intf, IN Bool
 allowGw)
{
  e_Err         e;

  e = ARP_ClearCache(l3amHandle, intf, allowGw);
  return e;
}




/*********************************************************************
 * @purpose               Retrieves various counts associated with the ARP
 *                        cache.
 *
 *
 * @param l3amHandle      @b{(input)}  handle of L3AM object returned by
 *                                        L3AM_Init
 * @param *pStats         @b{(input)}  pointer to structure to return ARP cache
 *                                        counts
 *
 * @returns  E_OK          - success
 * @returns   not E_OK     - request failed, look at std.h for definitions of
 *                           all possible causes of failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err L3AMX_GetCacheStats ( IN t_Handle l3amHandle,
                             OUT L7_arpCacheStats_t *pStats)
{
  e_Err         e;

  e = ARP_GetCacheStats(l3amHandle, pStats);
  return e;
}




/*********************************************************************
 * @purpose                  Stops all timers associated with this ARP entry.
 *
 *
 * @param l3amHandle         @b{(input)}  handle of L3AM object returned by
 *                                        L3AM_Init,
 * @param  p_adrEntry        @b{(input)} pointer to ARP entry
 *
 * @returns  E_OK          - success
 * @returns  not E_OK      - request failed, look at std.h for definitions of
 *                           all possible causes of failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err L3AMX_StopArpEntryTimers ( IN t_Handle l3amHandle,  IN t_Handle
 p_adrEntry)
{
  e_Err         e;

  e = ARP_StopEntryTimers(l3amHandle, p_adrEntry);
  return e;
}




/*********************************************************************
 * @purpose              Deletes a single entry from the ARP cache, as
 *                      specified by its IP address.Only works for dynamic
 *                      ARP entries.
 *
 *
 * @param l3amHandle         @b{(input)}  handle of L3AM object returned by
 *                                        L3AM_Init,
 * @param p_l3Addr           @b{(input)}  pointer to layer 3 address
 * @param flags              @b{(input)}  Control bit flag (interface number)
 *
 * @returns  E_OK          - success
 * @returns  not E_OK      - request failed, look at std.h for definitions of
 *                           all possible causes of failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err L3AMX_DeleteArpEntry ( IN t_Handle l3amHandle,  IN t_Lay3Addr *p_l3Addr, IN word flags)
{
  e_Err         e;

  e = ARP_DelDstAddr(l3amHandle, (t_IPAddr *)p_l3Addr->lay3Addr, flags);
  return e;
}


/**********************end of file********************/
