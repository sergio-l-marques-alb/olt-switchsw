/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_ctrl.c
*
* @purpose    PIM-DM Control Packet Handling/Processing Routines
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
#include "l3_addrdefs.h"
#include "l3_mcast_commdefs.h"
#include "l7_mcast_api.h"
#include "l7_pimdm_api.h"
#include "pim_defs.h"
#include "mcast_wrap.h"
#include "pimdm_defs.h"
#include "pimdm_main.h"
#include "pimdm_debug.h"
#include "pimdm_pkt.h"
#include "pimdm_intf.h"
#include "pimdm_mrt.h"
#include "pimdm_asrt_fsm.h"
#include "pimdm_upstrm_fsm.h"
#include "pimdm_dnstrm_fsm.h"
#include "pimdm_ctrl.h"
#include "pimdm_util.h"
#include "pimdm_map_vend_ctrl.h"
#include "pimdm_map_debug.h"

/*******************************************************************************
**                      Defines & Local Typedefs                              **
*******************************************************************************/
/* PIM Hello Packet structure
 *
 * This structure should not be used to interpret the values as byte
 * alignment issues may result in incorrect values. This is provided
 * so that it could be used to size-up the memory required to assemble
 * a Hello Message
 */
typedef struct pimdmHelloPkt_s
{
  L7_uchar8   dummy1;   /* pimVerType */
  L7_uchar8   dummy2;   /* reserved */
  L7_ushort16 dummy3;   /* chksum */
  L7_ushort16 dummy4;   /* holdTimeOption */
  L7_ushort16 dummy5;   /* holdTimeOptionLen */
  L7_ushort16 dummy6;   /* holdTime */
  L7_ushort16 dummy7;   /* lanPruneDelayOption */
  L7_ushort16 dummy8;   /* lanPruneDelayOptionLen */
  L7_ushort16 dummy9;   /* lanPruneDelay */
  L7_ushort16 dummy10;  /* overrideInterval */
  L7_ushort16 dummy11;  /* genIdOption */
  L7_ushort16 dummy12;  /* genIdOptionLen */
  L7_uint32   dummy13;  /* genId */
  L7_ushort16 dummy14;  /* stateRefreshOption */
  L7_ushort16 dummy15;  /* stateRefreshOptionLen */
  L7_uchar8   dummy16;  /* stateRefreshVer */
  L7_uchar8   dummy17;  /* stateRefreshInterval */
  L7_ushort16 dummy18;  /* stateRefreshReserved */
} pimdmHelloPkt_t;

/*******************************************************************************
**                        Function Declarations                               **
*******************************************************************************/
static L7_RC_t
pimdmHeaderValidate (pimdmCB_t *pimdmCB, L7_uchar8* pimdmMsg, L7_uint32 msgLen,
                     L7_uint32 rtrIfNum, L7_inet_addr_t *ipSrcAddr,
                     PIM_PDU_TYPE_t *pimPduType);

static L7_RC_t
pimdmHeaderFrame (PIM_PDU_TYPE_t pimPduType, pimdmHdr_t *pimdmHdr);

static L7_RC_t
pimdmCtrlPktSend (pimdmCB_t *pimdmCB, L7_uchar8 *pimdmMsg, L7_uint32 msgLen,
                  L7_inet_addr_t *destAddr, L7_uint32 rtrIfNum,
                  PIMDM_CTRL_PKT_TYPE_t pimdmPktType);

static L7_RC_t
pimdmAssertMsgProcess (pimdmCB_t *pimdmCB, L7_inet_addr_t *srcAddr,
                       L7_inet_addr_t *grpAddr, L7_inet_addr_t *nbrAddr,
                       L7_uint32 rtrIfNum, L7_uint32 asrtMetricPref,
                       L7_uint32 asrtMetric);

static L7_RC_t
pimdmAssertMsgReceive (pimdmCB_t *pimdmCB, L7_uchar8 *pimdmMsg, L7_uint32 msgLen,
                       L7_uint32 rtrIfNum, L7_inet_addr_t *nbrAddr);

static L7_RC_t
pimdmStateRefreshMsgProcess (pimdmCB_t *pimdmCB, pimdmStRfrMsg_t *stRfrMsg,
                             L7_uint32 rtrIfNum, L7_inet_addr_t *nbrAddr);

static L7_RC_t
pimdmStateRefreshMsgReceive (pimdmCB_t *pimdmCB, L7_uchar8 *pimdmMsg,
                             L7_uint32 msgLen, L7_uint32 rtrIfNum,
                             L7_inet_addr_t *nbrAddr);

static L7_RC_t
pimdmPruneMsgProcess (pimdmCB_t *pimdmCB, L7_inet_addr_t *srcAddr,
                      L7_inet_addr_t *grpAddr, L7_inet_addr_t *nbrAddr,
                      L7_inet_addr_t *upsmNbrAddr, L7_uint32 rtrIfNum,
                      L7_uint32 jpgHoldTime);

static L7_RC_t
pimdmGraftAckMsgProcess (pimdmCB_t *pimdmCB, L7_inet_addr_t *srcAddr,
                         L7_inet_addr_t *grpAddr, L7_inet_addr_t *nbrAddr,
                         L7_inet_addr_t *upsmNbrAddr, L7_uint32 rtrIfNum,
                         L7_uint32 jpgHoldTime);

static L7_RC_t
pimdmGraftMsgProcess (pimdmCB_t *pimdmCB, L7_inet_addr_t *srcAddr,
                      L7_inet_addr_t *grpAddr, L7_inet_addr_t *nbrAddr,
                      L7_inet_addr_t *upsmNbrAddr, L7_uint32 rtrIfNum,
                      L7_uint32 jpgHoldTime);

static L7_RC_t
pimdmJoinMsgProcess (pimdmCB_t *pimdmCB, L7_inet_addr_t *srcAddr,
                     L7_inet_addr_t *grpAddr, L7_inet_addr_t *nbrAddr,
                     L7_inet_addr_t *upsmNbrAddr, L7_uint32 rtrIfNum,
                     L7_uint32 jpgHoldTime);

static L7_RC_t
pimdmJoinPruneGraftMsgReceive (pimdmCB_t *pimdmCB, L7_uchar8 *jpgMsg,
                          L7_uint32 msgLen, L7_uint32 rtrIfNum,
                          L7_inet_addr_t *ipSrcAddr, PIM_PDU_TYPE_t pimPduType);

static L7_RC_t
pimdmHelloMsgReceive (pimdmCB_t *pimdmCB, L7_uchar8 *pimdmMsg, L7_uint32 msgLen,
                      L7_uint32 rtrIfNum, L7_inet_addr_t *ipSrcAddr);

static L7_RC_t
pimdmCtrlPktProcess (pimdmCB_t *pimdmCB, L7_uchar8 *pimdmMsg, L7_uint32 msgLen,
                     L7_uint32 rtrIfNum, L7_inet_addr_t *ipSrcAddr,
                     PIM_PDU_TYPE_t pimPduType);

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/
/* To print PIM-DM Control Packet Types in Trace messages */
L7_char8* gPimdmPktType[] =
{
  "Hello",
  "Join",
  "Prune",
  "Assert",
  "Graft",
  "Graft-Ack",
  "State-Refresh",
  "Data - NoCache",
  "Data - WrongIf",
};

/* To print PIM-DM PDU Types in Trace messages */
L7_char8* gPimdmPduType[] =
{
  "Hello",
  "Register",
  "Register-Stop",
  "Join-Prune",
  "BootStrap",
  "Assert",
  "Graft",
  "Graft-Ack",
  "Cand-RP-Advt",
  "State-Refresh",
};


/******************************************************************************
*
* @purpose  Send PIM  join/prune pkt immediately
*
* @param    pimdmCb       @b{(input)}  control block
* @param    pJPData          @b{(input)}  join/prune data to sendout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
L7_RC_t pimdmJoinPruneImdtSend(pimdmCB_t *pimdmCB,
                               pimdmSendJoinPruneData_t * pJPData)
{
  L7_uchar8 *pData;
  L7_uchar8 *pDataStart;
  L7_uint32 datalen = 0;
  L7_inet_addr_t *pNbrAddr = L7_NULLPTR;
  L7_uchar8 addr1[PIMDM_MAX_DBG_ADDR_SIZE];
  PIM_PDU_TYPE_t pimPduType;

  if((pimdmCB == (pimdmCB_t * )L7_NULLPTR) ||
     (pJPData == (pimdmSendJoinPruneData_t * )L7_NULLPTR))
  {
    PIMDM_TRACE( PIMDM_DEBUG_FAILURE, "Invalid Input");
    PIMDM_TRACE( PIMDM_DEBUG_API, "pimdmCB  = %p", pimdmCB);
    PIMDM_TRACE( PIMDM_DEBUG_API, "pJPData  = %p", pJPData);
    return L7_FAILURE;
  }

  PIMDM_TRACE( PIMDM_DEBUG_API, "destAddr : ",
                   inetAddrPrint(pJPData->destAddr,addr1));
  PIMDM_TRACE( PIMDM_DEBUG_API, "neighborAddr : ",
                   inetAddrPrint(pJPData->pNbrAddr,addr1));
  PIMDM_TRACE( PIMDM_DEBUG_API, "holdtime = %d",
              pJPData->holdtime);
  PIMDM_TRACE( PIMDM_DEBUG_API, "srcAddr :",
                   inetAddrPrint(pJPData->pSrcAddr,addr1));
  PIMDM_TRACE( PIMDM_DEBUG_API, "srcAddrMaskLen = %d",
              pJPData->srcAddrMaskLen);
  PIMDM_TRACE( PIMDM_DEBUG_API, "grpAddr :",
                   inetAddrPrint(pJPData->pGrpAddr,addr1));
  PIMDM_TRACE( PIMDM_DEBUG_API, "grpAddrMaskLen = %d",
              pJPData->grpAddrMaskLen);
  PIMDM_TRACE( PIMDM_DEBUG_API, "addrFlags = 0x%x",
              pJPData->addrFlags);
  PIMDM_TRACE( PIMDM_DEBUG_API, "rtrIfNum = %d",
              pJPData->rtrIfNum);
  PIMDM_TRACE( PIMDM_DEBUG_API, "pktType = %d",
              pJPData->pktType);

  if(pJPData->rtrIfNum == 0)
  {
    PIMDM_TRACE( PIMDM_DEBUG_FAILURE,
      "Error! rtrIfNum is zero");
    return(L7_FAILURE);
  }


  if ((pJPData->pktType == PIMDM_CTRL_PKT_JOIN) ||
      (pJPData->pktType == PIMDM_CTRL_PKT_PRUNE))
  {
    pimPduType = PIM_PDU_JOIN_PRUNE;
  }
  else if (pJPData->pktType == PIMDM_CTRL_PKT_GRAFT)
  {
    pimPduType = PIM_PDU_GRAFT;
  }
  else if (pJPData->pktType == PIMDM_CTRL_PKT_GRAFT_ACK)
  {
    pimPduType = PIM_PDU_GRAFT_ACK;
  }
  else
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"PIM Failed to get the pimpduType");
    return L7_FAILURE;
  }

  pDataStart = pimdmCB->pktTxBuf;
  memset(pDataStart , 0 , PIMDM_PKT_SIZE_MAX);

  pData = pDataStart;
  /* Prepare the PIM packet */
  MCAST_PUT_BYTE(((PIM_PROTOCOL_VERSION << 4) | (pimPduType & 0x0f)), pData);
  MCAST_PUT_BYTE(0, pData);
  MCAST_PUT_SHORT(0, pData);

  /*Copy the family, type, and neighbor address */
  pNbrAddr = pJPData->pNbrAddr;
  PIM_PUT_EUADDR_INET(pNbrAddr, pData);
  MCAST_PUT_BYTE(0, pData);          /* Reserved */
  MCAST_PUT_BYTE(1, pData);          /* numgroups */
  MCAST_PUT_SHORT(pJPData->holdtime, pData);

  PIM_PUT_EGADDR_INET(pJPData->pGrpAddr, pJPData->grpAddrMaskLen, 0, pData);

  if ((pJPData->pktType == PIMDM_CTRL_PKT_JOIN) ||
      (pJPData->pktType == PIMDM_CTRL_PKT_GRAFT) ||
      (pJPData->pktType == PIMDM_CTRL_PKT_GRAFT_ACK))
  {
    MCAST_PUT_SHORT(1, pData);/* Join Addr num*/
    MCAST_PUT_SHORT(0, pData);/* Prune Addr num*/
  }
  else if (pJPData->pktType == PIMDM_CTRL_PKT_PRUNE)
  {
    MCAST_PUT_SHORT(0, pData);/* Join Addr num*/
    MCAST_PUT_SHORT(1, pData);/* Prune Addr num */
  }
  else
  {
    PIMDM_TRACE( PIMDM_DEBUG_FAILURE,
                "Error: pktType - %d is not set correctly", pJPData->pktType);
    return(L7_FAILURE);
  }

  PIM_PUT_ESADDR_INET(pJPData->pSrcAddr,
                      pJPData->srcAddrMaskLen,
                      pJPData->addrFlags, pData);
  datalen = pData - pDataStart;

  if (pimdmCtrlPktSend (pimdmCB, pDataStart, datalen, pJPData->destAddr,
                        pJPData->rtrIfNum, pJPData->pktType)
                     != L7_SUCCESS)
  {
    PIMDM_TRACE( PIMDM_DEBUG_FAILURE, "pimdmPacketSend() failed! ");
    return L7_FAILURE;
  }

  pimdmIntfStatsUpdate (pimdmCB, pJPData->rtrIfNum,
                        pJPData->pktType, PIMDM_STATS_SENT);

  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  allocate buffers to build join/prune message
*
* @param    pimdmCb     @b{(input)}  control block
* @param    pJPMsgBuff  @b{(output)} buffer to build join/prune message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t
pimdmJPWorkingBuffGet(pimdmCB_t *pimdmCB, pimdmJPMsgStoreBuff_t *pJPMsgBuff)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  memset (pJPMsgBuff, 0, sizeof(pimdmJPMsgStoreBuff_t));

  /* Allocate J/P Message Buffer */
  if ((pJPMsgBuff->pJPMsg =
                   PIMDM_ALLOC (pimdmCB->addrFamily, PIMDM_MAX_JP_MSG_SIZE))
                             == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Unable to allocate buffer from jpMsgBuffer");
    pJPMsgBuff->pJPMsg = L7_NULLPTR;
    return L7_FAILURE;
  }

  /* Allocate Join List Buffer */
  if ((pJPMsgBuff->pJoinList =
                   PIMDM_ALLOC (pimdmCB->addrFamily,
                                ((pimJoinPruneMaxAddrsInListGet(pimdmCB->addrFamily)) * sizeof(pim_encod_src_addr_t))))
                              == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Unable to allocate buffer from jpJoinListBuffer");
    PIMDM_FREE (pimdmCB->addrFamily, (void*) pJPMsgBuff->pJPMsg);
    return L7_FAILURE;
  }

  /* Allocate Prune List Buffer */
  if ((pJPMsgBuff->pPruneList =
                   PIMDM_ALLOC (pimdmCB->addrFamily,
                                ((pimJoinPruneMaxAddrsInListGet(pimdmCB->addrFamily)) * sizeof(pim_encod_src_addr_t))))
                              == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Unable to allocate buffer from jpPruneListBuffer");
    PIMDM_FREE (pimdmCB->addrFamily, (void*) pJPMsgBuff->pJPMsg);
    PIMDM_FREE (pimdmCB->addrFamily, (void*) pJPMsgBuff->pJoinList);
    return L7_FAILURE;
  }

  inetAddressZeroSet (pimdmCB->addrFamily, &pJPMsgBuff->currGrpAddr);
  inetAddressZeroSet (pimdmCB->addrFamily, &pJPMsgBuff->destAddr);

  pJPMsgBuff->buildJPMsgUse = L7_TRUE;
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose de-allocate buffers used to build join/prune message
*
* @param    pimdmCb     @b{(input)}  control block
* @param    pNbrEntry   @b{(input)}  neighbor entry
*
* @returns  void
*
* @comments     none
*
* @end
*
******************************************************************************/
void
pimdmJPWorkingBuffReturn (pimdmCB_t *pimdmCB,
                          pimdmNeighbor_t *pNbrEntry)
{
  pimdmJPMsgStoreBuff_t *pJPMsgBuff = &pNbrEntry->pimdmBuildJPMsg;

  if( pJPMsgBuff->buildJPMsgUse != L7_TRUE)
  {
   return;
  }
  if (pJPMsgBuff->pJPMsg != L7_NULLPTR)
  {
   PIMDM_FREE (pimdmCB->addrFamily, (void*) pJPMsgBuff->pJPMsg);
  }
  if (pJPMsgBuff->pJoinList != L7_NULLPTR)
  {
    PIMDM_FREE (pimdmCB->addrFamily, (void*) pJPMsgBuff->pJoinList);
  }
  if (pJPMsgBuff->pPruneList != L7_NULLPTR)
  {
   PIMDM_FREE (pimdmCB->addrFamily, (void*) pJPMsgBuff->pPruneList);
  }

  memset(pJPMsgBuff, 0, sizeof(pimdmJPMsgStoreBuff_t));

  inetAddressZeroSet (pimdmCB->addrFamily, &pJPMsgBuff->currGrpAddr);
  inetAddressZeroSet (pimdmCB->addrFamily, &pJPMsgBuff->destAddr);

  pJPMsgBuff->buildJPMsgUse = L7_FALSE;

  return;
}
/******************************************************************************
*
* @purpose  Send PIM  join/prune pkt
*
* @param    pimdmCb     @b{(input)}  control block
* @param    pNbrEntry   @b{(input)}  neighbor entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimdmJPMsgSend(pimdmCB_t *pimdmCB,
                              pimdmNeighbor_t *pNbrEntry)
{

  L7_uchar8 *pDataStart = L7_NULLPTR;
  L7_uchar8 *pData = L7_NULLPTR;
  L7_uint32 datalen;
  L7_uint32 rtrIfNum;
  PIM_PDU_TYPE_t pimPduType;

  if ((pNbrEntry->pimdmBuildJPMsg.pktType == PIMDM_CTRL_PKT_JOIN) ||
      (pNbrEntry->pimdmBuildJPMsg.pktType == PIMDM_CTRL_PKT_PRUNE))
  {
    pimPduType = PIM_PDU_JOIN_PRUNE;
  }
  else if (pNbrEntry->pimdmBuildJPMsg.pktType == PIMDM_CTRL_PKT_GRAFT)
  {
    pimPduType = PIM_PDU_GRAFT;
  }
  else if (pNbrEntry->pimdmBuildJPMsg.pktType == PIMDM_CTRL_PKT_GRAFT_ACK)
  {
    pimPduType = PIM_PDU_GRAFT_ACK;
  }
  else
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"PIM Failed to get the pimpduType");
    pimdmJPWorkingBuffReturn(pimdmCB, pNbrEntry);
    return L7_FAILURE;
  }

  pDataStart = pimdmCB->pktTxBuf;
  memset(pDataStart , 0 , PIMDM_PKT_SIZE_MAX);
  pData = pDataStart;
  /* Prepare the PIM packet */
  MCAST_PUT_BYTE(((PIM_PROTOCOL_VERSION << 4) | (pimPduType & 0x0f)), pData);
  MCAST_PUT_BYTE(0, pData);
  MCAST_PUT_SHORT(0, pData);

  rtrIfNum = pNbrEntry->rtrIfNum;
  datalen = pNbrEntry->pimdmBuildJPMsg.jpMsgSize;
  memcpy(pData, pNbrEntry->pimdmBuildJPMsg.pJPMsg, datalen);
  PIMDM_TRACE( PIMDM_DEBUG_API,
                "pimdmBuildJPMsg.pJPMsg Length = %d", datalen);

  if (pimdmCtrlPktSend (pimdmCB, pDataStart, datalen + PIM_HEARDER_SIZE,
                        &pNbrEntry->pimdmBuildJPMsg.destAddr,
                        rtrIfNum, pNbrEntry->pimdmBuildJPMsg.pktType)
                     != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "pimdmCtrlPktSend() failed! ");
    pimdmJPWorkingBuffReturn(pimdmCB, pNbrEntry);
    return L7_FAILURE;
  }

  pimdmIntfStatsUpdate (pimdmCB, rtrIfNum,
                        pNbrEntry->pimdmBuildJPMsg.pktType, PIMDM_STATS_SENT);

  pimdmJPWorkingBuffReturn(pimdmCB, pNbrEntry);
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose   Pack all fields in join/prune message
*
* @param    pimdmCb     @b{(input)}  control block
* @param    pNbrEntry   @b{(input)}  neighbor entry
*
* @returns  void
*
* @comments     none
*
* @end
*
* TODO: XXX: Currently, the (*,*,RP) stuff goes at the end of the
* Join/Prune message. However, this particular implementation of PIM
* processes the Join/Prune messages faster if (*,*,RP) is at the beginning.
* Modify some of the functions below such that the
* outgoing messages place (*,*,RP) at the beginning, not at the end.
*
******************************************************************************/
static void
pimdmJoinPruneMsgPack (pimdmCB_t *pimdmCB,
                       pimdmNeighbor_t *pNbrEntry)
{
  pimdmJPMsgStoreBuff_t *pJPMsgBuff = L7_NULLPTR;
  L7_uchar8 *pData;
  L7_uchar8 addr1[PIMDM_MAX_DBG_ADDR_SIZE];

  pJPMsgBuff = &pNbrEntry->pimdmBuildJPMsg;
  if(pJPMsgBuff->buildJPMsgUse != L7_TRUE)
  {
    PIMDM_TRACE( PIMDM_DEBUG_API,
                "buildJPMsgUse not in use");
    return;
  }
  if(inetIsAddressZero(&pJPMsgBuff->currGrpAddr) == L7_TRUE)
  {
    PIMDM_TRACE( PIMDM_DEBUG_API,
                "currGrpAddr is not set");
    return;
  }

  PIMDM_TRACE( PIMDM_DEBUG_API,
                "pJPMsgBuff->pJPMsg = %p pJPMsgBuff->jpMsgSize = %d",
                pJPMsgBuff->pJPMsg, pJPMsgBuff->jpMsgSize);
  PIMDM_TRACE( PIMDM_DEBUG_API,
                "pJPMsgBuff->currGrpAddr = %s",
                inetAddrPrint(&pJPMsgBuff->currGrpAddr,addr1));
  PIMDM_TRACE( PIMDM_DEBUG_API,
                "pJPMsgBuff->currGrpMaskLen = %d",
                pJPMsgBuff->currGrpMaskLen);
  PIMDM_TRACE( PIMDM_DEBUG_API,
                "pJPMsgBuff->joinListSize = %d",
                pJPMsgBuff->joinListSize);
  PIMDM_TRACE( PIMDM_DEBUG_API,
                "pJPMsgBuff->numJoinAddrs = %d",
                pJPMsgBuff->numJoinAddrs);
  PIMDM_TRACE( PIMDM_DEBUG_API,
                "pJPMsgBuff->pruneListSize = %d",
                pJPMsgBuff->pruneListSize);
  PIMDM_TRACE( PIMDM_DEBUG_API,
                "pJPMsgBuff->numPruneAddrs = %d",
                pJPMsgBuff->numPruneAddrs);
  PIMDM_TRACE( PIMDM_DEBUG_API,
                "*pJPMsgBuff->pNumGrps = %d",
                *pJPMsgBuff->pNumGrps);


  pData = pJPMsgBuff->pJPMsg + pJPMsgBuff->jpMsgSize;
  PIM_PUT_EGADDR_INET(&pJPMsgBuff->currGrpAddr,
   pJPMsgBuff->currGrpMaskLen, 0, pData);
  MCAST_PUT_SHORT(pJPMsgBuff->numJoinAddrs, pData);
  MCAST_PUT_SHORT(pJPMsgBuff->numPruneAddrs, pData);
  memcpy(pData, pJPMsgBuff->pJoinList, pJPMsgBuff->joinListSize);
  pData += pJPMsgBuff->joinListSize;
  memcpy(pData, pJPMsgBuff->pPruneList, pJPMsgBuff->pruneListSize);
  pData += pJPMsgBuff->pruneListSize;
  pJPMsgBuff->jpMsgSize = (pData - pJPMsgBuff->pJPMsg);
  inetAddressZeroSet(pimdmCB ->addrFamily, &pJPMsgBuff->currGrpAddr);
  pJPMsgBuff->currGrpMaskLen = 0;
  pJPMsgBuff->joinListSize = 0;
  pJPMsgBuff->numJoinAddrs = 0;
  pJPMsgBuff->pruneListSize = 0;
  pJPMsgBuff->numPruneAddrs = 0;
  (*pJPMsgBuff->pNumGrps)++;
}

/******************************************************************************
*
* @purpose  Pack and send join/prune message
*
* @param    pimdmCb     @b{(input)}  control block
* @param    pNbrEntry   @b{(input)}  neighbor entry
*
* @returns  void
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t
pimdmJoinPruneMsgPackAndSend(pimdmCB_t *pimdmCB,
         pimdmNeighbor_t *pNbrEntry)
{
  if((pNbrEntry == (pimdmNeighbor_t *)L7_NULLPTR)||
     pNbrEntry->pimdmBuildJPMsg.buildJPMsgUse != L7_TRUE)
  {
    /* PIMDM_TRACE( PIMDM_DEBUG_FAILURE, "pNbrEntry is L7_NULLPTR or buildJPMsgUse is FALSE"); */
    return L7_FAILURE;
  }

  pimdmJoinPruneMsgPack(pimdmCB, pNbrEntry);

  if(pimdmJPMsgSend(pimdmCB, pNbrEntry)!= L7_SUCCESS)
  {
    /* PIMDM_TRACE( PIMDM_DEBUG_FAILURE, "pimdmJPMsgSend() Failed"); */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose   Prepare to send PIM join/prune pkt
*
* @param    pimdmCb     @b{(input)}  control block
* @param    sgSendJoinPruneData               @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     Use This API if bundling join/prune is required.
            The Logic is very implementation specific.

*
* @end
*
******************************************************************************/
static L7_RC_t
pimdmJoinPruneBundleSend (pimdmCB_t *pimdmCB,
                          pimdmNeighbor_t *pNbrEntry,
                          pimdmSendJoinPruneData_t *pJPData)
{
  pimdmJPMsgStoreBuff_t *pJPMsgBuff = L7_NULLPTR;
  L7_uchar8 *pData = L7_NULLPTR;
  L7_uint32 maxJPListSize = 0;
  L7_uint32 joinPruneMsgSize = 0;
  L7_ushort16 holdtime = pJPData->holdtime;
  L7_inet_addr_t *pGrpAddr = pJPData->pGrpAddr;
  L7_uchar8 grpAddrMaskLen = pJPData->grpAddrMaskLen;
  L7_inet_addr_t *pSrcAddr = pJPData->pSrcAddr;
  L7_uchar8 srcAddrMaskLen = pJPData->srcAddrMaskLen;
  L7_ushort16 addrFlags = pJPData->addrFlags;
  PIMDM_CTRL_PKT_TYPE_t pktType = pJPData->pktType;
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  PIMDM_TRACE (PIMDM_DEBUG_API, "nbrAddr    - %s", inetAddrPrint(pJPData->pNbrAddr,nbr));
  PIMDM_TRACE (PIMDM_DEBUG_API, "holdtime   - %d", holdtime);
  PIMDM_TRACE (PIMDM_DEBUG_API, "srcAddr    - %s", inetAddrPrint(pJPData->pSrcAddr,nbr));
  PIMDM_TRACE (PIMDM_DEBUG_API, "srcMaskLen - %d", srcAddrMaskLen);
  PIMDM_TRACE (PIMDM_DEBUG_API, "grpAddr    - %s", inetAddrPrint(pJPData->pGrpAddr,nbr));
  PIMDM_TRACE (PIMDM_DEBUG_API, "grpMaskLen - %d", grpAddrMaskLen);
  PIMDM_TRACE (PIMDM_DEBUG_API, "addrFlags  - 0x%x", pJPData->addrFlags);
  PIMDM_TRACE( PIMDM_DEBUG_API, "rtrIfNum   - %d", pJPData->rtrIfNum);
  PIMDM_TRACE( PIMDM_DEBUG_API, "pktType    - %d", pJPData->pktType);

  pJPMsgBuff = &pNbrEntry->pimdmBuildJPMsg;

  if (pimJoinPruneMsgLenGet (pimdmCB->addrFamily, &joinPruneMsgSize)
                          != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "pimJoinPruneMsgLenGet() Failed");
    return L7_FAILURE;
  }

  maxJPListSize = (pimJoinPruneMaxAddrsInListGet(pimdmCB->addrFamily)) * joinPruneMsgSize;
  PIMDM_TRACE (PIMDM_DEBUG_API, "maxJPListSize - %d", maxJPListSize);

  if (pJPMsgBuff->buildJPMsgUse == L7_TRUE)
  {
    PIMDM_TRACE (PIMDM_DEBUG_API, "jpMsgSize - %d", pJPMsgBuff->jpMsgSize);
    PIMDM_TRACE (PIMDM_DEBUG_API, "joinListSize - %d", pJPMsgBuff->joinListSize);
    PIMDM_TRACE (PIMDM_DEBUG_API, "pruneListSize - %d", pJPMsgBuff->pruneListSize);
    PIMDM_TRACE (PIMDM_DEBUG_API, "JP Msg Buff - %p", pJPMsgBuff);
    PIMDM_TRACE (PIMDM_DEBUG_API, "JP Msg Buff Build Msg - %p", pJPMsgBuff->pJPMsg);
    PIMDM_TRACE (PIMDM_DEBUG_API, "grpAddr in JP MsgBuff - %s", inetAddrPrint(&pJPMsgBuff->currGrpAddr,nbr));
    PIMDM_TRACE (PIMDM_DEBUG_API, "holdtime in JP msgBuff - %d", pJPMsgBuff->holdtime);
    PIMDM_TRACE (PIMDM_DEBUG_API, "GrpMaskLen in JP msgBuff - %d", pJPMsgBuff->currGrpMaskLen);
    if (((pJPMsgBuff->jpMsgSize + pJPMsgBuff->joinListSize +
          pJPMsgBuff->pruneListSize + 24 + 20) >=
          (PIMDM_MAX_JP_MSG_SIZE - joinPruneMsgSize)) ||
         (pJPMsgBuff->joinListSize >= (maxJPListSize-1)) ||
         (pJPMsgBuff->pruneListSize >= (maxJPListSize-1)) ||
         (pJPMsgBuff->pktType != pktType))
    {
      if (pimdmJoinPruneMsgPackAndSend (pimdmCB, pNbrEntry) != L7_SUCCESS)
      {
        PIMDM_TRACE (PIMDM_DEBUG_API, "pimdmJoinPruneMsgPackAndSend() Failed");
        return L7_FAILURE;
      }
    }

    if ((MCAST_INET_IS_ADDR_EQUAL (&pJPMsgBuff->currGrpAddr, pGrpAddr) == L7_FALSE) ||
         (pJPMsgBuff->currGrpMaskLen != grpAddrMaskLen) ||
         (pJPMsgBuff->holdtime != holdtime))
    {
      pimdmJoinPruneMsgPack(pimdmCB, pNbrEntry);
    }
  }

  if (pJPMsgBuff->buildJPMsgUse != L7_TRUE)
  {
    if (pimdmJPWorkingBuffGet (pimdmCB, &pNbrEntry->pimdmBuildJPMsg) != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Unable to allocate buffers for JPMsg");
      return L7_FAILURE;
    }

    inetCopy (&pJPMsgBuff->destAddr, pJPData->destAddr);
    pJPMsgBuff->pktType = pJPData->pktType;

    pData = pJPMsgBuff->pJPMsg;

    /*
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |        Upstream Neighbor Address (Encoded-Unicast format)     |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |  Reserved     | Num groups    |          Holdtime             |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    PIM_PUT_EUADDR_INET (&pNbrEntry->addr, pData); /* Upstream Neighbor Addr */
    MCAST_PUT_BYTE (0, pData);                     /* Reserved */
    pJPMsgBuff->pNumGrps = pData++;                /* The pointer for numgroups */
    *(pJPMsgBuff->pNumGrps) = 0;                   /* Zero groups - This will be updated later */
    MCAST_PUT_SHORT (holdtime, pData);             /* Holdtime */
    pJPMsgBuff->holdtime = holdtime;
    pJPMsgBuff->jpMsgSize = pData - pJPMsgBuff->pJPMsg;
  }

  inetCopy(&pJPMsgBuff->currGrpAddr, pGrpAddr);
  pJPMsgBuff->currGrpMaskLen = grpAddrMaskLen;

  switch(pktType)
  {
    case PIMDM_CTRL_PKT_JOIN:
    case PIMDM_CTRL_PKT_GRAFT:
    case PIMDM_CTRL_PKT_GRAFT_ACK:
         pData = pJPMsgBuff->pJoinList + pJPMsgBuff->joinListSize;
         break;

    case PIMDM_CTRL_PKT_PRUNE:
         pData = pJPMsgBuff->pPruneList + pJPMsgBuff->pruneListSize;
         break;

    default:
         return L7_FAILURE;
  }

  PIM_PUT_ESADDR_INET (pSrcAddr, srcAddrMaskLen, addrFlags, pData);

  switch(pktType)
  {
    case PIMDM_CTRL_PKT_JOIN:
    case PIMDM_CTRL_PKT_GRAFT:
    case PIMDM_CTRL_PKT_GRAFT_ACK:
         pJPMsgBuff->joinListSize = pData - pJPMsgBuff->pJoinList;
         pJPMsgBuff->numJoinAddrs++;
         break;

    case PIMDM_CTRL_PKT_PRUNE:
         pJPMsgBuff->pruneListSize = pData - pJPMsgBuff->pPruneList;
         pJPMsgBuff->numPruneAddrs++;
         break;

    default:
      return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Prepare to send PIM join/prune pkt
*
* @param    pimdmCb     @b{(input)}  control block
* @param    pJPData          @b{(input)}  join/prune data to sendout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
L7_RC_t
pimdmJoinPruneSend (pimdmCB_t *pimdmCB,
                    pimdmSendJoinPruneData_t *pJPData)
{
  pimdmNeighbor_t *pNbrEntry = L7_NULLPTR;
  pimdmInterface_t *pIntfEntry = L7_NULLPTR;
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB == L7_NULLPTR) || (pJPData == L7_NULLPTR))
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Input pimdmCB - %p, pJPData - %p",
                 pimdmCB, pJPData);
    return L7_FAILURE;
  }

  if (pJPData->rtrIfNum == 0)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Error! rtrIfNum is zero");
    return L7_FAILURE;
  }

  if (pimdmIntfEntryGet (pimdmCB, pJPData->rtrIfNum, &pIntfEntry) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Get Intf Entry");
    return L7_FAILURE;
  }

  if (pJPData->pruneEchoFlag == L7_TRUE)
  {
    pimdmJoinPruneImdtSend (pimdmCB, pJPData);
  }
  else
  {
    PIMDM_TRACE (PIMDM_DEBUG_API, "Upstrm Nbr Addr - %s",
                 inetAddrPrint (pJPData->pNbrAddr,nbr));

    if (pimdmNeighborFind (pimdmCB, pIntfEntry, pJPData->pNbrAddr, &pNbrEntry)
                        != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Nbr Entry not Found");
      return L7_FAILURE;
    }

    if (pimdmJoinPruneBundleSend (pimdmCB, pNbrEntry, pJPData) != L7_SUCCESS)
    {
      PIMDM_TRACE( PIMDM_DEBUG_FAILURE, "J/P/G Bundling Failed");
      return L7_FAILURE;
    }
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}
/******************************************************************************
*
* @purpose  Prepare to send PIM join/prune pkt
*
* @param    pimdmCb     @b{(input)}  control block
* @param    pJPData          @b{(input)}  join/prune data to sendout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
L7_RC_t
pimdmBundleJPGMsgSend (pimdmCB_t *pimdmCB,
                       L7_inet_addr_t *destnAddr,
                       L7_uint32 rtrIfNum,
                       L7_inet_addr_t *srcAddr,
                       L7_inet_addr_t *grpAddr,
                       L7_inet_addr_t *upstrmNbrAddr,
                       L7_uint32 jpgHoldtime,
                       PIMDM_CTRL_PKT_TYPE_t pimdmPktType)
{
  pimdmSendJoinPruneData_t jpData;
  L7_uchar8 maskLen = 0;
  L7_inet_addr_t myAddress;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  memset(&jpData, 0, sizeof(pimdmSendJoinPruneData_t));
  jpData.rtrIfNum = rtrIfNum;
  jpData.holdtime = jpgHoldtime;
  jpData.pNbrAddr = upstrmNbrAddr;
  jpData.destAddr = destnAddr;
  jpData.addrFlags = 0;

  jpData.pSrcAddr = srcAddr;
  pimSrcMaskLenGet (pimdmCB->addrFamily, &maskLen);
  jpData.srcAddrMaskLen = maskLen;

  jpData.pGrpAddr = grpAddr;
  pimGrpMaskLenGet (pimdmCB->addrFamily, &maskLen);
  jpData.grpAddrMaskLen = maskLen;

  jpData.pktType = pimdmPktType;

  if (pimdmPktType == PIMDM_CTRL_PKT_PRUNE)
  {
    /* Set the Prune Echo Flag if the Upstream Neighbor Address is our
     * Local Address.
     */
    if(pimdmIntfIpAddressGet (pimdmCB, rtrIfNum, &myAddress) != L7_SUCCESS)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Failed to get Ip address on rtrIfNum %d",
                  rtrIfNum);
      return L7_FAILURE;
    }
    if (L7_INET_ADDR_COMPARE (upstrmNbrAddr, &myAddress) == 0)
    {
      jpData.pruneEchoFlag = L7_TRUE;
    }
  }

  if (pimdmJoinPruneSend (pimdmCB, &jpData) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to send JPG Message on rtrIfNum %d",
                 rtrIfNum);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose
*
* @param        pParam  @b{(input)} timer handle
*
* @returns   void
*
* @comments
*
* @end
******************************************************************************/
void pimdmJPBundleTimerExpiryHandler(void *pParam)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmNeighbor_t *pCurrentNbrEntry = L7_NULLPTR;
  pimdmIntfInfo_t *pIntfInfo = L7_NULLPTR;
  L7_uint32       i;
  L7_inet_addr_t  zeroInet;

  /* PIMDM_TRACE (PIMDM_DEBUG_API, "Entry"); */

  if ((pimdmCB = (pimdmCB_t*)(pParam)) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid input parameter");
    return;
  }
  if (pimdmCB->pimdmJPBundleTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMDM_MAP_COMPONENT_ID,
            "PIMDM pimdmJPBundleTimer is NULL, But Still Expired");
    return;
  }
  pimdmCB->pimdmJPBundleTimer = L7_NULLPTR;

  if ((pIntfInfo = pimdmCB->intfInfo) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "pimdmCB->intfInfo is NULL");
    return;
  }

  inetAddressZeroSet (pimdmCB->addrFamily, &zeroInet);
  for (i = 0; i < PIMDM_MAX_NEIGHBORS; i++)
  {
    pCurrentNbrEntry = &(pIntfInfo->nbrs[i]);

    if (L7_INET_ADDR_COMPARE (&zeroInet, &(pCurrentNbrEntry->addr)) == 0)
    {
      continue;
    }

    if (pimdmJoinPruneMsgPackAndSend (pimdmCB, pCurrentNbrEntry)!= L7_SUCCESS)
    {
      /* PIMDM_TRACE( PIMDM_DEBUG_FAILURE, "pimdmJoinPruneMsgPackAndSend() Failed"); */
    }
  }

  if (pimdmUtilAppTimerSet (pimdmCB, pimdmJPBundleTimerExpiryHandler,
                            (void*)pimdmCB,
                            PIMDM_DEFAULT_JP_BUNDLE_TIME, L7_NULLPTR, L7_NULLPTR,
                            &(pimdmCB->pimdmJPBundleTimer),
                            "PD-JPB")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "pimdmJPBundleTimer appTimerAdd Failed");
    return;
  }

  /* PIMDM_TRACE (PIMDM_DEBUG_API, "Exit"); */
  return;
}

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  Validate the PIM-DM Header fields
*
* @param    *pimdmCB    - @b{(input)} Pointer to the PIM-DM Control Block
*           pimdmMsg    - @b{(input)} Received PIM-DM Message
*           msgLen      - @b{(input)} Length of the received PIM-DM Message
*           rtrIfNum    - @b{(input)} Received Packet Router Interface Index
*           ipSrcAddr   - @b{(input)} Received Packet Source Address
*           *pimPduType - @b{(output)} PIM Control Packet Type
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmHeaderValidate (pimdmCB_t *pimdmCB,
                     L7_uchar8* pimdmMsg,
                     L7_uint32 msgLen,
                     L7_uint32 rtrIfNum,
                     L7_inet_addr_t *ipSrcAddr,
                     PIM_PDU_TYPE_t *pimPduType)
{
  pimdmHdr_t* pimdmHdr = L7_NULLPTR;
  L7_uint32 pimVersion = 0;
  PIM_PDU_TYPE_t pduType = PIM_PDU_MAX;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  pimdmHdr = (pimdmHdr_t*) pimdmMsg;

  /* Validate PIM Version.
   * Only Version 2 is supported.
   */
  if ((pimVersion = ((pimdmHdr->ver_type) >> 4)) != PIM_PROTOCOL_VERSION)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Ctrl Pkt received with Unsupported PIM"
             "Version - %d From - %s on Interface %d with Length %d",
             pimVersion, inetAddrPrint(ipSrcAddr,src), rtrIfNum, msgLen);
    return L7_FAILURE;
  }

  /* Validate PIM Pkt Type.
   */
  if ((pduType = ((pimdmHdr->ver_type) & 0x0F)) >= PIM_PDU_MAX)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Ctrl Pkt received with Unsupported PIM"
             "Pkt Type - %d From - %s on Interface %d with Length %d",
             pduType, inetAddrPrint(ipSrcAddr,src), rtrIfNum, msgLen);
    return L7_FAILURE;
  }

  /* NOTE: Ignore the Reserved field.
   */

  /* Validate PIM Header Checksum.
   * NOTE: For IPv6, Kernel will validate the Checksum.
   */
  if (pimdmCB->addrFamily == L7_AF_INET)
  {
    if (inetChecksum ((L7_short16*) pimdmMsg, msgLen))
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Ctrl Pkt received with Bad Checksum"
               "Pkt Type - %d From - %s on Interface %d with Length %d",
               pduType, inetAddrPrint(ipSrcAddr,src), rtrIfNum, msgLen);
      return L7_FAILURE;
    }
  }

  *pimPduType = pduType;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Prepare the PIM-DM Header fields
*
* @param    pimPduType  - @b{(input)} Type of PIM-DM PDU
*           pimdmHdr    - @b{(output)} PIM-DM Filled-in Header
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmHeaderFrame (PIM_PDU_TYPE_t pimPduType,
                  pimdmHdr_t *pimdmHdr)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmHdr == L7_NULLPTR) || (pimPduType >= PIM_PDU_MAX))
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid Pim Header or PimPduType");
    return L7_FAILURE;
  }

  memset (pimdmHdr, 0, sizeof (pimdmHdr_t));

  /* Update PIM Version and PDU Type.
   * Only Version-2 is Supported.
   */
  pimdmHdr->ver_type = ((PIM_PROTOCOL_VERSION << 4) | (pimPduType & 0x0F));
  pimdmHdr->reserved = 0;
  pimdmHdr->chkSum = 0;  /* This would be updated in pimdmCtrlPktSend() */

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Send a PIM-DM Control Packet out on the Socket
*
* @param    pimdmCB    - @b{(input)} Pointer to the PIM-DM Control Block
*           pimdmMsg   - @b{(input)} PIM-DM Control Packet Type to be Sent
*           msgLen     - @b{(input)} Length of the PIM-DM Control Packet
*           destAddr   - @b{(input)} Destination Address of the Packet
*           rtrIfNum   - @b{(input)} Packet's Outgoing Router Interface Index
*           pimdmPktType - @b{(input)} PIM-DM Control Packet Type
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmCtrlPktSend (pimdmCB_t *pimdmCB,
                  L7_uchar8 *pimdmMsg,
                  L7_uint32 msgLen,
                  L7_inet_addr_t *destAddr,
                  L7_uint32 rtrIfNum,
                  PIMDM_CTRL_PKT_TYPE_t pimdmPktType)
{
  L7_ushort16 checkSum = 0;
  L7_uchar8 dst[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  mcastSendPkt_t mcastSendPkt;
  PIMDM_TRACE_LVL_t trcLvl;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Compute & Update the buffer Checksum in the Message.
   * NOTE: For IPv6, Kernel will update the Checksum.
   */
  if (pimdmCB->addrFamily == L7_AF_INET)
  {
    checkSum = inetChecksum ((L7_ushort16*) pimdmMsg, msgLen);
    ((pimdmHdr_t*) pimdmMsg)->chkSum = checkSum;
  }

  /* Initialize the mcastSendPkt_t structure.
   */
  memset (&mcastSendPkt, 0, sizeof (mcastSendPkt_t));

  /* Fill in the respective fields of mcastSendPkt.
   */
  mcastSendPkt.sockFd = pimdmCB->sockFD;
  inetAddressZeroSet (pimdmCB->addrFamily, &(mcastSendPkt.srcAddr));
  inetAddressZeroSet (pimdmCB->addrFamily, &(mcastSendPkt.destAddr));

  if (pimdmIntfIpAddressGet (pimdmCB , rtrIfNum, &(mcastSendPkt.srcAddr))
      != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "IP Addr Get Failed for rtrIfNum  - %d", rtrIfNum);
    return L7_FAILURE;
  }

  /* NOTE: For IPv6, the Source Address will be filled by the Socket.
   */
  inetCopy (&(mcastSendPkt.destAddr), destAddr);
  mcastSendPkt.protoType = PIM_IP_PROTO_NUM;
  mcastSendPkt.ttl = PIMDM_DEFAULT_CTRL_PKT_TTL;
  mcastSendPkt.rtrIfNum = rtrIfNum;
  mcastSendPkt.payLoad = pimdmMsg;
  mcastSendPkt.length = msgLen;
  mcastSendPkt.flags = 0;
  mcastSendPkt.port = 0;
  mcastSendPkt.family = pimdmCB->addrFamily;
  mcastSendPkt.rtrAlert = L7_FALSE;

  /* Handover the Packet to Socket for Transmission.
   */
  if (mcastMapPacketSend (&mcastSendPkt) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"PIM-DM Packet [%s] Send to %s on"
             "Interface %d with Length %d Failed",
             gPimdmPktType[pimdmPktType], inetAddrPrint(destAddr,dst),
             rtrIfNum, msgLen);
    return L7_FAILURE;
  }

  pimdmDebugPacketRxTxTrace(pimdmCB->addrFamily, L7_FALSE,
                            mcastSendPkt.rtrIfNum, &mcastSendPkt.srcAddr,
                            &mcastSendPkt.destAddr, mcastSendPkt.payLoad,
                            mcastSendPkt.length);

  /* Filter out the Trace messages for Hello Packets, if required. */
  if (pimdmPktType == PIMDM_CTRL_PKT_HELLO)
  {
    trcLvl = PIMDM_DEBUG_HELLO_PKT | PIMDM_DEBUG_CTRL_PKT;
  }
  else
  {
    trcLvl = PIMDM_DEBUG_CTRL_PKT;
  }

  PIMDM_TRACE (trcLvl, "Sent PIM-DM [%s] Packet..\n"
      "    From....... %s\n"
      "    To......... %s\n"
      "    Interface.. %d\n"
      "    Length..... %d\n",
      gPimdmPktType[pimdmPktType], inetAddrPrint(&(mcastSendPkt.srcAddr),src),
      inetAddrPrint(destAddr,dst), rtrIfNum, msgLen);
  pimdmDebugByteDump (pimdmMsg, msgLen, trcLvl);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Send the PIM-DM Assert Control Message
*
* @param    mrtEntry       - @b{(input)} Pointer to the MRT (S,G) entry
*           rtrIfNum       - @b{(input)} Index of the outgoing Router Interface
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmAssertMsgSend (pimdmCB_t *pimdmCB,
                    L7_inet_addr_t *destnAddr,
                    L7_uint32 rtrIfNum,
                    L7_inet_addr_t *srcAddr,
                    L7_inet_addr_t *grpAddr,
                    L7_uint32 asrtMetricPref,
                    L7_uint32 asrtMetric)
{
  PIMDM_CTRL_PKT_TYPE_t pimdmPktType = PIMDM_CTRL_PKT_ASSERT;
  L7_uchar8 *asrtBuf = pimdmCB->pktTxBuf;
  L7_uint32 asrtBufLen = 0;
  L7_uint32 maskLen = 0;
  L7_uchar8 dst[PIMDM_MAX_DBG_ADDR_SIZE];
  pimdmHdr_t pimdmHdr;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  memset (asrtBuf, 0, PIMDM_MAX_PDU);

  /** Prepare and Copy the PIM-DM Header **/
  pimdmHeaderFrame (PIM_PDU_ASSERT, &pimdmHdr);
  memcpy (asrtBuf, (L7_uchar8*) &pimdmHdr, sizeof (pimdmHdr_t));
  asrtBuf += sizeof (pimdmHdr_t);

  /** Prepare the Assert Message **/

  /* Fill the Group Address in Encoded Group Format */
  maskLen = ((pimdmCB->addrFamily == L7_AF_INET) ? (32) : (128));
  PIM_PUT_EGADDR_INET (grpAddr, maskLen, 0, asrtBuf);

  /* Fill the Source Address in Encoded Unicast Format */
  PIM_PUT_EUADDR_INET (srcAddr, asrtBuf);

  /* Fill the Assert Preference */
  MCAST_PUT_LONG (asrtMetricPref, asrtBuf);

  /* Fill the Assert Metric */
  MCAST_PUT_LONG (asrtMetric, asrtBuf);

  /* Calculate the PIM-DM Assert Message Length */
  asrtBufLen = asrtBuf - pimdmCB->pktTxBuf;

  /* Send the PIM-DM Assert Message */
  if (pimdmCtrlPktSend (pimdmCB, pimdmCB->pktTxBuf, asrtBufLen, destnAddr,
        rtrIfNum, pimdmPktType)
      != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "PIM-DM Assert Packet Send to %s on Interface %d with \
                 Length %d Failed", inetAddrPrint(destnAddr,dst),
                 rtrIfNum, asrtBufLen);
    return L7_FAILURE;
  }

  pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, pimdmPktType, PIMDM_STATS_SENT);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Process the received Assert Control Message
*
* @param    pimdmCB    - @b{(input)} Pointer to the PIM-DM Control Block
*           srcAddr    - @b{(input)} Address of the Multicast Data Source
*           grpAddr    - @b{(input)} Address of the Multicast Group
*           nbrAddr    - @b{(input)} Received Packet Source Address
*           rtrIfNum   - @b{(input)} Received Packet Router Interface Index
*           asrtMetricPref - @b{(input)} Assert Metric Preference of the Sender
*           asrtMetric - @b{(input)} Assert Metric of the Sender
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmAssertMsgProcess (pimdmCB_t *pimdmCB,
                       L7_inet_addr_t *srcAddr,
                       L7_inet_addr_t *grpAddr,
                       L7_inet_addr_t *nbrAddr,
                       L7_uint32 rtrIfNum,
                       L7_uint32 asrtMetricPref,
                       L7_uint32 asrtMetric)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_BOOL isSenderMetricPreferred = L7_TRUE;
  L7_BOOL isSenderAsrtWnr = L7_FALSE;
  pimdmAssertEventInfo_t asrtFsmEventInfo;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t ipAddress;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Get the (S,G) MRT Entry */
  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr)) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "MRT Lookup Failed for Src - %s, Group - %s",
                  inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    return L7_FAILURE;
  }

  /* Verify if we received this Packet on our Upstream interface.
   */
  if (mrtEntry->upstrmRtrIfNum == rtrIfNum)  /* Assert on Upstream Interface */
  {
    PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT,
                 "Assert Received on Upstream Interface - %d for Src - %s, Group - %s",
                 rtrIfNum, inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

    if (pimdmMrtUpstreamAssertProcess (mrtEntry, nbrAddr, rtrIfNum,
                                       asrtMetricPref, asrtMetric)
                                    != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                   "Upstream Assert Processing Failed for Src - %s, Group - %s",
                   inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
      return L7_FAILURE;
    }
  }
  else /* Assert on Downstream Interface */
  {
    pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;

    if (mrtEntry->downstrmStateInfo[rtrIfNum] == L7_NULLPTR)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Downstrm Intf - %d", rtrIfNum);
      return L7_FAILURE;
    }

    asrtIntfInfo = &mrtEntry->downstrmStateInfo[rtrIfNum]->downstrmAssertInfo;

    /*************** ASSERT FSM EVENTS *********************/

    /* Prepare the Assert Event Information.
     */
    asrtFsmEventInfo.rtrIfNum = rtrIfNum;
    asrtFsmEventInfo.assertMetric = asrtMetric;
    asrtFsmEventInfo.assertPref = asrtMetricPref;
    asrtFsmEventInfo.interval = PIMDM_DEFAULT_ASSERT_TIMEOUT;
    asrtFsmEventInfo.type = PIMDM_CTRL_PKT_ASSERT;
    inetCopy (&(asrtFsmEventInfo.assertAddr), nbrAddr);

   /* Check if the Sender of this Message is the Assert Winner.
    */
    if (L7_INET_ADDR_COMPARE (nbrAddr, &(asrtIntfInfo->assertWnrAddr)) == 0)
    {
      isSenderAsrtWnr = L7_TRUE;
    }

    /* Check if the Sender is Inferior/Preferred to us.
     */
    inetAddressZeroSet(pimdmCB->addrFamily,&ipAddress);
    if(pimdmIntfIpAddressGet(pimdmCB,rtrIfNum,&ipAddress) != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                   "Failed to get Ip Address on RtrIndex %d",rtrIfNum);
      return L7_FAILURE;
    }

    if(pimdmMrtAsrtMetricCompare(mrtEntry->rpfInfo.rpfRouteMetricPref,
                                 mrtEntry->rpfInfo.rpfRouteMetric,
                                 &ipAddress,asrtMetricPref,asrtMetric,nbrAddr)
                                  == L7_TRUE )
    {
       isSenderMetricPreferred = L7_FALSE;
    }

    /* EVENT - Receive Inferior (Assert OR State Refresh) from Assert Winner.
     */
    if ((isSenderAsrtWnr == L7_TRUE)  && (isSenderMetricPreferred != L7_TRUE))
    {
      pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_INF_ASRT_ARVL_FROM_WNR, mrtEntry,
                             &asrtFsmEventInfo);
    }

    /* EVENT - Receive Inferior (Assert OR State Refresh) from non-Assert Winner
     *         AND CouldAssert == TRUE.
     */
    if ((isSenderAsrtWnr == L7_FALSE) && (isSenderMetricPreferred != L7_TRUE) &&
        (asrtIntfInfo->couldAssert == L7_TRUE))
    {
      pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_INF_ASRT_ARVL_FROM_LSR_COULD_ASRT_TRUE,
                             mrtEntry, &asrtFsmEventInfo);
    }

    /* EVENT - Receive Preferred Assert OR State Refresh.
     */
    if (isSenderMetricPreferred == L7_TRUE)
    {
      pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_PRF_ASRT_ARVL, mrtEntry,
                             &asrtFsmEventInfo);
    }
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Process the received Assert Message.
*
* @param    pimdmCB    - @b{(input)} Pointer to the PIM-DM Control Block
*           pimdmMsg   - @b{(input)} Received PIM-DM Assert Message
*           msgLen     - @b{(input)} Length of the received PIM-DM Message
*           rtrIfNum   - @b{(input)} Received Packet Router Interface Index
*           nbrAddr    - @b{(input)} Received Packet Source Address
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmAssertMsgReceive (pimdmCB_t *pimdmCB,
                    L7_uchar8 *pimdmMsg,
                    L7_uint32 msgLen,
                    L7_uint32 rtrIfNum,
                    L7_inet_addr_t *nbrAddr)
{
  L7_uchar8 *pimdmMsgEnd = pimdmMsg + msgLen;
  pim_encod_grp_addr_t encGrpAddr; /* Encoded Group Format */
  pim_encod_uni_addr_t encSrcAddr; /* Encoded Unicast Format */
  L7_uint32 asrtMetricPref;
  L7_uint32 asrtMetric;
  L7_inet_addr_t tempGrpAddr;
  L7_inet_addr_t *grpAddr = &tempGrpAddr;
  L7_inet_addr_t tempSrcAddr;
  L7_inet_addr_t *srcAddr = &tempSrcAddr;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* NOTE: pimdmMsg points after PIM Header */

  /* Read the Group Address in Encoded Group Format */
  memset (&encGrpAddr, 0, sizeof (pim_encod_grp_addr_t));
  PIM_GET_EGADDR_INET (&encGrpAddr, pimdmMsg);
  inetAddressSet (encGrpAddr.addr_family, &(encGrpAddr.addr), grpAddr);

  /* Read the Source Address in Encoded Unicast Format */
  memset (&encSrcAddr, 0, sizeof (pim_encod_uni_addr_t));
  PIM_GET_EUADDR_INET (&encSrcAddr, pimdmMsg);
  inetAddressSet (encSrcAddr.addr_family, &(encSrcAddr.addr), srcAddr);

  /* Read the Preference and Metric from the Assert Message */
  /* Assert Preference */
  MCAST_GET_LONG (asrtMetricPref, pimdmMsg);

  /* Assert Metric */
  MCAST_GET_LONG (asrtMetric, pimdmMsg);

  /* Validate Packet length */
  if (pimdmMsg > pimdmMsgEnd)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Assert Msg Length is not valid", msgLen);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "Assert Message received From %s on "
               "Interface - %d for Source - %s & Group - %s with MetricPref - %d"
               ", Metric - %d", inetAddrPrint(nbrAddr,nbr), rtrIfNum,
               inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
               asrtMetricPref, asrtMetric);

  /* Process the Received Assert Message.
   */
  if (pimdmAssertMsgProcess (pimdmCB, srcAddr, grpAddr, nbrAddr, rtrIfNum,
                             asrtMetricPref, asrtMetric)
                          != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Assert Msg Processing Failed");
    pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_ASSERT,
                          PIMDM_STATS_DROPPED);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "Assert Message Processing Successful "
               "From %s on Interface - %d for Source - %s & Group - %s",
               inetAddrPrint(nbrAddr,nbr), rtrIfNum,
               inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

  pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_ASSERT,
                        PIMDM_STATS_RECEIVED);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Send the PIM-DM State Refresh Control Message
*
* @param    pimdmCB    - @b{(input)} Pointer to the PIM-DM Control Block
*           srcAddr    - @b{(input)} Address of the Multicast Source
*           grpAddr    - @b{(input)} Address of the Multicast Group
*           originatorAddr    - @b{(input)} Address of the Message Originator
*           rtrIfNum          - @b{(input)} Index of the Router Interface on
*                                           which RPF'(S)/S is connected
*           stRfrTTL          - @b{(input)} TTL of the State Refresh Message
*           stRfrInterval     - @b{(input)} Interval of the State Refresh Message
*           rtMetricPref      - @b{(input)} Preference of the RPF'(S) Route
*           rtMetric          - @b{(input)} Metric of the RPF'(S) Route
*           rtMaskLen         - @b{(input)} Mask Length of the RPF'(S) Route
*           pruneIndFlag      - @b{(input)} Prune Indicator Flag
*           asrtOverrideFlag  - @b{(input)} Assert Override Flag
*           pruneNowFlag      - @b{(input)} Prune Now Flag
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmStateRefreshMsgSend (pimdmCB_t *pimdmCB,
                          L7_inet_addr_t *srcAddr,
                          L7_inet_addr_t *grpAddr,
                          L7_inet_addr_t *originatorAddr,
                          L7_uint32 rtrIfNum,
                          L7_uint32 stRfrTTL,
                          L7_uint32 stRfrInterval,
                          L7_uint32 rtMetricPref,
                          L7_uint32 rtMetric,
                          L7_uint32 rtMaskLen,
                          L7_BOOL pruneIndFlag,
                          L7_BOOL asrtOverrideFlag,
                          L7_BOOL pruneNowFlag)
{
  PIMDM_CTRL_PKT_TYPE_t pimdmPktType = PIMDM_CTRL_PKT_STATE_REFRESH;
  L7_inet_addr_t *destAddr = L7_NULLPTR;
  L7_uchar8 *stRfrBuf = pimdmCB->pktTxBuf;
  L7_uint32 stRfrBufLen = 0;
  L7_uint32 maskLen = 0;
  L7_uchar8 reserved = 0;
  L7_uchar8 dst[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  pimdmHdr_t pimdmHdr;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic Validations */
  if ((pimdmCB == L7_NULLPTR) || (srcAddr == L7_NULLPTR) ||
      (grpAddr == L7_NULLPTR) || (originatorAddr == L7_NULLPTR) ||
      (stRfrTTL <= 0))
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Invalid Ctrl Block or SrcAddr or grpAddr or \
                 OrginatorAddr or StRfrTTL");
    return L7_FAILURE;
  }

  maskLen = ((pimdmCB->addrFamily == L7_AF_INET) ? (32) : (128));
  destAddr = &(pimdmCB->allRoutersAddr);

  /* Frame the State Refresh Message and Transmit on this interface.
   */
  memset (stRfrBuf, 0, PIMDM_MAX_PDU);

  /** Prepare and Copy the PIM-DM Header **/
  pimdmHeaderFrame (PIM_PDU_STATE_REFRESH, &pimdmHdr);
  memcpy (stRfrBuf, (L7_uchar8*) &pimdmHdr, sizeof (pimdmHdr_t));
  stRfrBuf += sizeof (pimdmHdr_t);

  /* Fill in the Group Address - Encoded Group Format */
  PIM_PUT_EGADDR_INET (grpAddr, maskLen, 0, stRfrBuf);

  /* Source Address - Encoded Unicast Format */
  PIM_PUT_EUADDR_INET (srcAddr, stRfrBuf);

  /* Originator Address - Encoded Unicast Format */
  PIM_PUT_EUADDR_INET (originatorAddr, stRfrBuf);

  /* Metric Preference */
  MCAST_PUT_LONG (rtMetricPref, stRfrBuf);

  /* Metric */
  MCAST_PUT_LONG (rtMetric, stRfrBuf);

  /* Mask Len */
  MCAST_PUT_BYTE (rtMaskLen, stRfrBuf);

  /* TTL */
  MCAST_PUT_BYTE (stRfrTTL, stRfrBuf);

  /* Reserved */
  if (pruneIndFlag == L7_TRUE )
  {
    PIMDM_BIT_SET (reserved, PIMDM_STRFR_PRUNE_INDICATOR_BIT);  /* Prune Indicator Bit */
  }

  if (asrtOverrideFlag == L7_TRUE)
  {
    PIMDM_BIT_SET (reserved, PIMDM_STRFR_ASRT_OVERRIDE_BIT);  /* Assert Override Bit */
  }

  if (pruneNowFlag == L7_TRUE)
  {
    PIMDM_BIT_SET (reserved, PIMDM_STRFR_PRUNE_NOW_BIT);  /* Prune Now Bit */
  }

  MCAST_PUT_BYTE (reserved, stRfrBuf);

  /* State Refresh Interval */
  MCAST_PUT_BYTE (PIMDM_DEFAULT_REFRESH_INTERVAL, stRfrBuf);

  /* Calculate the PIM-DM State Refresh Message Length */
  stRfrBufLen = stRfrBuf - pimdmCB->pktTxBuf;

  /* Transmit the State Refresh Message on this interface */
  if (pimdmCtrlPktSend (pimdmCB, pimdmCB->pktTxBuf, stRfrBufLen, destAddr,
                        rtrIfNum, pimdmPktType)
                     != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "PIM-DM State Refresh Packet Send to %s on Interface %d with "
                 "Length %d Failed", inetAddrPrint(destAddr,dst),
                 rtrIfNum, stRfrBufLen);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "PIM-DM Packet [%s] Sent to %s on "
               "Interface %d for Group - %s Source - %s with Length %d",
               gPimdmPktType[pimdmPktType], inetAddrPrint(destAddr,dst),
               rtrIfNum, inetAddrPrint(grpAddr,grp),
               inetAddrPrint(srcAddr,src), stRfrBufLen);

  pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, pimdmPktType, PIMDM_STATS_SENT);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Process the received State Refresh Message.
*
* @param    pimdmCB    - @b{(input)} Pointer to the PIM-DM Control Block
*           stRfrMsg   - @b{(input)} State Refresh Message Block
*           rtrIfNum   - @b{(input)} Received Packet Router Interface Index
*           nbrAddr    - @b{(input)} Received Packet Source Address
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmStateRefreshMsgProcess (pimdmCB_t *pimdmCB,
                             pimdmStRfrMsg_t *stRfrMsg,
                             L7_uint32 rtrIfNum,
                             L7_inet_addr_t *nbrAddr)
{
  pimdmMrtEntry_t *mrtEntry= L7_NULLPTR;
  L7_inet_addr_t tempRpfSAddr;
  L7_inet_addr_t *rpfDashSAddr = &tempRpfSAddr;
  pimdmUpstrmEventInfo_t upstrmFsmEventInfo;
  pimdmAssertEventInfo_t asrtFsmEventInfo;
  L7_BOOL pruneIndicator = L7_FALSE;
  L7_BOOL isSenderPreferred = L7_FALSE;
  L7_inet_addr_t *srcAddr = L7_NULLPTR;
  L7_inet_addr_t *grpAddr = L7_NULLPTR;
  L7_uint32 timeGap = 0;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 dst[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 rpf[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_BOOL upstrmPruneIndFlag = L7_FALSE;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Storing values Just for TRACE purpose.
   */
  srcAddr = &(stRfrMsg->srcAddr);
  grpAddr = &(stRfrMsg->grpAddr);

  /* Get the (S,G) MRT Entry */
  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr)) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "MRT Lookup Failed for Src - %s, Group - %s",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

    /* RFC 3973 - Section 4.3.4 Implementation */
    /* Create the MRT Entry */
    if ((mrtEntry = pimdmMrtEntryAdd (pimdmCB, srcAddr, grpAddr, rtrIfNum))
                                   == L7_NULLPTR)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                   "Unable to create the MRT entry for Src - %s, Grp - %s",
                   inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
      return L7_FAILURE;
    }
    if (PIMDM_BIT_TST (stRfrMsg->reserved, PIMDM_STRFR_PRUNE_INDICATOR_BIT))
    {
      upstrmPruneIndFlag = L7_TRUE;
      mrtEntry->upstrmGraftPruneStateInfo.grfPrnState = PIMDM_UPSTRM_STATE_PRUNE;
    }
  }

  /* Do not process the State Refresh Message when the Upstream Interface
   * is Down.
   */
  if (mrtEntry->upstrmRtrIfNum == 0)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "State Refresh Msg is not processed as "
                 "the Upstream interface - %d is Down", mrtEntry->upstrmRtrIfNum);
    return L7_FAILURE;
  }

  /* Verify if the message has come from the RPF'(S).
   */
  inetCopy (rpfDashSAddr, &(mrtEntry->upstrmNbrInfo.assertWnrAddr));
  if (L7_INET_ADDR_COMPARE (rpfDashSAddr, nbrAddr) != 0)
  {
    if(pimdmCB->addrFamily == L7_AF_INET6)
    {
      if(inetIsDirectlyConnected (nbrAddr, rtrIfNum) != L7_TRUE)
      {
        PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                    "State Refresh Msg received on Interface - %d "
                    "for Src - %s, Group - %s hasn't come from RPF'(S) - %s",
                    rtrIfNum, inetAddrPrint(srcAddr,src),
                    inetAddrPrint(grpAddr,grp),
                    inetAddrPrint(rpfDashSAddr,rpf));
        return L7_FAILURE;
      }
    }
  }

  /* Check the Prune Indicator Bit.
   */
  if (PIMDM_BIT_TST (stRfrMsg->reserved, PIMDM_STRFR_PRUNE_INDICATOR_BIT))
  {
    pruneIndicator = L7_TRUE;
  }

  if ((L7_INET_ADDR_COMPARE (nbrAddr, rpfDashSAddr) == 0) &&
      (mrtEntry->upstrmRtrIfNum == rtrIfNum))
  {
    /* SRM Received on the Upstream Interface */

    /*************** UPSTREAM FSM EVENTS *********************/
    /* Prepare the Upstream Event Information.
     */
    upstrmFsmEventInfo.rtrIfNum = rtrIfNum;

    /* EVENT - State Refresh (S,G) received from RPF'(S) AND
     *         Prune Indicator == 1.
     */
    if (pruneIndicator == L7_TRUE)
    {
      pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_STRFR_ARVL_PI_TRUE, mrtEntry,
                             &upstrmFsmEventInfo);
    }

    /* EVENT - State Refresh (S,G) received from RPF'(S) AND
     *         Prune Indicator == 0 AND
     *         PLT (S,G) not running.
     */
    if ((pruneIndicator == L7_FALSE) &&
        (mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer == L7_NULLPTR))
    {
      pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_STRFR_ARVL_PI_FALSE_PLT_NR,
                             mrtEntry, &upstrmFsmEventInfo);
    }

    /* NOTE: The below Upstream FSM Event is not required anymore as the
     * Bundle Send mechanism is implemented and minimizes the packet loss.
     * Will remove it shortly after observing more scenario's.
     */
#if 0
    /* NEW_EVENT - State Refresh (S,G) received from RPF'(S) AND
     *             Prune Indicator == 0 AND
     *             Upstream GraftPrune State is PRUNED.
     * This is to stabilize the situation wherein the Prune Sent upstream
     * is lost in the network.
     */
    else if ((pruneIndicator == L7_FALSE ) &&
             (mrtEntry->upstrmGraftPruneStateInfo.grfPrnState ==
                                                      PIMDM_UPSTRM_STATE_PRUNE))
    {
      /* This is a hack.When we receive a State Refresh Msg with Prune Indicator
       * as FALSE,PLT is running and state is Pruned, then check if entry is
       * present in Hardware.If yes,send a prune to upstream and remain in
       * Prune state,reset the PLT.
       */
       pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_STRFR_ARVL_PI_FALSE_PLT_RUNNING,
                              mrtEntry, &upstrmFsmEventInfo);
    }
#endif

    /* Update the State Refresh received time for RateLimit.
     * Give the cushion of ~2 seconds as the timers may not return accurate
     * values.
     */
    if(mrtEntry->origStateInfo.timeSinceLastSRMsg != 0)
    {
      timeGap = ((osapiUpTimeRaw()) - (mrtEntry->origStateInfo.timeSinceLastSRMsg));
      if (timeGap < (PIMDM_DEFAULT_REFRESH_INTERVAL -2))
      {
        PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                     "State Refresh Rate Limit is TRUE for Src - %s, Group - %s "
                     "Time Difference - %d Sec\n", inetAddrPrint(srcAddr,src),
                     inetAddrPrint(grpAddr,grp), timeGap);
        return L7_FAILURE;
      }
    }
    mrtEntry->origStateInfo.timeSinceLastSRMsg = osapiUpTimeRaw ();

    if(stRfrMsg->stRfrTTL ==0)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                  "TTL field in state refresh message is 0");
      return L7_FAILURE;
    }

    /* Forward the received State Refresh message on all the connected
     * Downstream Interfaces.
     */
    if (pimdmMrtEntryControlPacketSend (mrtEntry, PIMDM_CTRL_PKT_STATE_REFRESH,
                          &(stRfrMsg->origAddr), &(pimdmCB->allRoutersAddr),
                          rtrIfNum, stRfrMsg->interval, (stRfrMsg->stRfrTTL - 1),
                          STRFR_MSG_FORWARDER, upstrmPruneIndFlag)
                       != L7_SUCCESS)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                  "PIM-DM State Refresh Packet Send to %s Failed",
                  inetAddrPrint(&(pimdmCB->allRoutersAddr),dst));
      return L7_FAILURE;
    }
  }
  else
  {
    /* SRM Received on the Downstream Interface */

    /*************** ASSERT FSM EVENTS *********************/

    /* Check if the Sender is Inferior/Preferred to us.
     */
    pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;
    L7_inet_addr_t myAddr;

    if (mrtEntry->downstrmStateInfo[rtrIfNum] == L7_NULLPTR)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Downstrm Intf - %d", rtrIfNum);
      return L7_FAILURE;
    }

    asrtIntfInfo = &(mrtEntry->downstrmStateInfo[rtrIfNum]->downstrmAssertInfo);

    /* Prepare the Assert Event Information.
     */
    memset (&asrtFsmEventInfo, 0, sizeof (pimdmAssertEventInfo_t));
    asrtFsmEventInfo.rtrIfNum = rtrIfNum;
    asrtFsmEventInfo.type = PIMDM_CTRL_PKT_STATE_REFRESH;
    asrtFsmEventInfo.interval = PIMDM_DEFAULT_ASSERT_TIMEOUT;
    inetAddressZeroSet (pimdmCB->addrFamily, &(asrtFsmEventInfo.assertAddr));

    if (pimdmIntfIpAddressGet (pimdmCB, rtrIfNum, &myAddr) != L7_SUCCESS)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                  "Failed to Get the Address for Interface %d", rtrIfNum);
      return L7_FAILURE;
    }

    if (pimdmMrtAsrtMetricCompare (mrtEntry->rpfInfo.rpfRouteMetricPref,
                                   mrtEntry->rpfInfo.rpfRouteMetric,
                                   &(myAddr),
                                   stRfrMsg->metricPref,
                                   stRfrMsg->metric,
                                   nbrAddr)
                                != L7_TRUE)
    {
      isSenderPreferred = L7_TRUE;
    }

    if (isSenderPreferred == L7_FALSE)
    {
      /* EVENT - Receive Inferior (Assert OR State Refresh) from Assert Winner.
       */
      if (L7_INET_ADDR_COMPARE (&(asrtIntfInfo->assertWnrAddr), nbrAddr) == 0)
      {
        pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_INF_ASRT_ARVL_FROM_WNR, mrtEntry,
                               &asrtFsmEventInfo);
      }
      /* EVENT - Receive Inferior (Assert OR State Refresh) from non-Assert Winner
       *         AND CouldAssert == TRUE.
       */
      else if (asrtIntfInfo->couldAssert == L7_TRUE)
      {
        pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_INF_ASRT_ARVL_FROM_LSR_COULD_ASRT_TRUE,
                               mrtEntry, &asrtFsmEventInfo);
      }
    }
    else
    {
      /* EVENT - Receive Preferred Assert OR State Refresh.
       */
      asrtFsmEventInfo.assertPref = stRfrMsg->metricPref;
      asrtFsmEventInfo.assertMetric = stRfrMsg->metric;
      inetCopy (&(asrtFsmEventInfo.assertAddr), nbrAddr);

      if (asrtIntfInfo->couldAssert == L7_TRUE)
      {
        if (asrtIntfInfo->assertState == PIMDM_ASSERT_STATE_ASSERT_WINNER)
        {
          asrtFsmEventInfo.interval = (3 * PIMDM_DEFAULT_REFRESH_INTERVAL);
        }
        else
        {
          asrtFsmEventInfo.interval = (3 * (stRfrMsg->interval));
        }

        pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_PRF_ASRT_ARVL, mrtEntry,
                               &asrtFsmEventInfo);
      }
    }
  }

  /* Compute the OIF list */
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

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Process the received State Refresh Message.
*
* @param    pimdmCB    - @b{(input)} Pointer to the PIM-DM Control Block
*           pimdmMsg   - @b{(input)} Received PIM-DM State Refresh Message
*           msgLen     - @b{(input)} Length of the received PIM-DM Message
*           rtrIfNum   - @b{(input)} Received Packet Router Interface Index
*           nbrAddr    - @b{(input)} Received Packet Source Address
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmStateRefreshMsgReceive (pimdmCB_t *pimdmCB,
                          L7_uchar8 *pimdmMsg,
                          L7_uint32 msgLen,
                          L7_uint32 rtrIfNum,
                          L7_inet_addr_t *nbrAddr)
{
  L7_uchar8* pimdmMsgEnd = pimdmMsg + msgLen;
  pimdmStRfrMsg_t stRfrMsg;
  pim_encod_grp_addr_t encGrpAddr; /* Encoded Group Format */
  pim_encod_uni_addr_t encSrcAddr; /* Encoded Unicast Format */
  pim_encod_uni_addr_t encOrigAddr; /* Encoded Unicast Format */
  L7_inet_addr_t *srcAddr = L7_NULLPTR;
  L7_inet_addr_t *grpAddr = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* NOTE: pimdmMsg points after PIM Header */

  /* Read the Group, Source and Originator Address from the StRfr Message */
  /* Group Address in Encoded Group Format */
  memset (&encGrpAddr, 0, sizeof (pim_encod_grp_addr_t));
  PIM_GET_EGADDR_INET (&encGrpAddr, pimdmMsg);
  inetAddressSet (encGrpAddr.addr_family, &(encGrpAddr.addr), &(stRfrMsg.grpAddr));

  /* Source Address in Encoded Unicast Format */
  memset (&encSrcAddr, 0, sizeof (pim_encod_uni_addr_t));
  PIM_GET_EUADDR_INET (&encSrcAddr, pimdmMsg);
  inetAddressSet (encSrcAddr.addr_family, &(encSrcAddr.addr), &(stRfrMsg.srcAddr));

  /* Originator Address in Encoded Unicast Format */
  memset (&encOrigAddr, 0, sizeof (pim_encod_uni_addr_t));
  PIM_GET_EUADDR_INET (&encOrigAddr, pimdmMsg);
  inetAddressSet (encOrigAddr.addr_family, &(encOrigAddr.addr), &(stRfrMsg.origAddr));

  /* Storing just for TRACE Purpose.
   */
  srcAddr = &(stRfrMsg.srcAddr);
  grpAddr = &(stRfrMsg.grpAddr);

  /* Read the Preference and Metric and other values from the StRfr Message.
   */
  /* Metric Preference */
  MCAST_GET_LONG (stRfrMsg.metricPref, pimdmMsg);

  /* Metric */
  MCAST_GET_LONG (stRfrMsg.metric, pimdmMsg);

  /* Masklen */
  MCAST_GET_BYTE (stRfrMsg.maskLen, pimdmMsg);

  /* TTL */
  MCAST_GET_BYTE (stRfrMsg.stRfrTTL, pimdmMsg);

  /* Reserved */
  MCAST_GET_BYTE (stRfrMsg.reserved, pimdmMsg);

  /* Interval */
  MCAST_GET_BYTE (stRfrMsg.interval, pimdmMsg);

  /* Validate Packet length */
  if (pimdmMsg > pimdmMsgEnd)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "State Refresh Msg Length %d is not valid",msgLen);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "State Refresh Message received From %s on "
               "Interface - %d for Source - %s & Group - %s",
               inetAddrPrint(nbrAddr,nbr), rtrIfNum,
               inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

  /* Process the Received State Refresh Message.
   */
  if (pimdmStateRefreshMsgProcess (pimdmCB, &stRfrMsg, rtrIfNum, nbrAddr)
                                != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"State Refresh Msg Processing Failed");
    pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_STATE_REFRESH,
                          PIMDM_STATS_DROPPED);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "State Refresh Message Processing Successful "
               "From %s on Interface - %d for Source - %s & Group - %s",
               inetAddrPrint(nbrAddr,nbr), rtrIfNum,
               inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

  pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_STATE_REFRESH,
                        PIMDM_STATS_RECEIVED);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Send a Hello Control Packet
*
* @param    pimdmCB        @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router interface to send the packet on
* param     pNbrInfo       @b{(input)} Pointer to the Neighbor information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmHelloMsgSend (pimdmCB_t *pimdmCB,
                   L7_uint32 rtrIfNum,
                   pimdmIntfNbrInfo_t *pNbrInfo)
{
  pimdmHelloPkt_t helloPkt;
  L7_uchar8 *pMsg = L7_NULLPTR;
  L7_uint32 msgLen;

  /* Basic sanity checks */
  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid PIM-DM Control Block [NULL]");
    return L7_FAILURE;
  }
  if(pNbrInfo == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid Neighbor Information [NULL]");
    return L7_FAILURE;
  }

  /* Form a Hello Packet */
  pMsg = (L7_uchar8 *)(&helloPkt);
  memset(&helloPkt, 0, sizeof(pimdmHelloPkt_t));

  /* Compose the PIM Header */
  MCAST_PUT_BYTE(PIMDM_HELLO_PKT_VER_TYPE, pMsg);
  MCAST_PUT_BYTE(0, pMsg);
  MCAST_PUT_NETSHORT(0, pMsg);  /* The send routine will compute the Checksum */

  /* Compose the HoldTime option */
  MCAST_PUT_NETSHORT(PIMDM_HELLO_OPTION_HOLDTIME, pMsg);
  MCAST_PUT_NETSHORT(PIMDM_HELLO_OPTIONLEN_HOLDTIME, pMsg);
  MCAST_PUT_NETSHORT(pNbrInfo->holdTime, pMsg);

  /* Compose the LAN Prune Delay option */
   if(pNbrInfo->bValidLanPruneDelay == L7_TRUE)
   {
     MCAST_PUT_NETSHORT(PIMDM_HELLO_OPTION_LANPRUNEDELAY, pMsg);
     MCAST_PUT_NETSHORT(PIMDM_HELLO_OPTIONLEN_LANPRUNEDELAY, pMsg);
     MCAST_PUT_NETSHORT(pNbrInfo->lanPruneDelay, pMsg);
     MCAST_PUT_NETSHORT(pNbrInfo->overrideInterval, pMsg);
   }
  /* Compose the Generation ID option */
  if(pNbrInfo->bValidGenId == L7_TRUE)
  {
    MCAST_PUT_NETSHORT(PIMDM_HELLO_OPTION_GENID, pMsg);
    MCAST_PUT_NETSHORT(PIMDM_HELLO_OPTIONLEN_GENID, pMsg);
    MCAST_PUT_NETLONG(pNbrInfo->genId, pMsg);
  }

  /* Compose the State Refresh option */
  if(pNbrInfo->bStateRefreshCapable == L7_TRUE)
  {
    MCAST_PUT_NETSHORT(PIMDM_HELLO_OPTION_STATEREFRESH, pMsg);
    MCAST_PUT_NETSHORT(PIMDM_HELLO_OPTIONLEN_STATEREFRESH, pMsg);
    MCAST_PUT_BYTE(PIMDM_STATEREFRESH_VERSION, pMsg);
    MCAST_PUT_BYTE(pNbrInfo->stateRefreshInterval, pMsg);
    MCAST_PUT_NETSHORT(0,pMsg);
  }

  /* Compute and verify if the message len is within the allocated limits */
  msgLen = pMsg - (L7_uchar8 *)(&helloPkt);
  if(msgLen > sizeof(pimdmHelloPkt_t))
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Message length [%d] of composed message is greater \
                 than allotted [%d]",
                 msgLen, sizeof(pimdmHelloPkt_t));
    return L7_FAILURE;
  }

  /* Send the Hello Packet */
  if (pimdmCtrlPktSend (pimdmCB, (L7_uchar8 *)(&helloPkt), msgLen,
                        &(pimdmCB->allRoutersAddr), rtrIfNum,
                        PIMDM_CTRL_PKT_HELLO)
                     != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"PIM-DM Packet [%s] Sent on "
                 "Interface %d with Length %d Failed",
                 gPimdmPktType[PIMDM_CTRL_PKT_HELLO],
                 rtrIfNum, msgLen);
    return L7_FAILURE;
  }

  /* Update Statistics */
  pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_HELLO,
                        PIMDM_STATS_SENT);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Process the received Prune Control Message
*
* @param    pimdmCB    - @b{(input)} Pointer to the PIM-DM Control Block
*           srcAddr    - @b{(input)} Address of the Multicast Data Source
*           grpAddr    - @b{(input)} Address of the Multicast Group
*           nbrAddr    - @b{(input)} Received Packet Source Address
*           upsmNbrAddr- @b{(input)} Address of the Upstream Neighbor to
*                                whom this message is addressed to
*           rtrIfNum   - @b{(input)} Received Packet Router Interface Index
*           jpgHoldTime- @b{(input)} Holdtime specified in the Message
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmPruneMsgProcess (pimdmCB_t *pimdmCB,
                      L7_inet_addr_t *srcAddr,
                      L7_inet_addr_t *grpAddr,
                      L7_inet_addr_t *nbrAddr,
                      L7_inet_addr_t *upsmNbrAddr,
                      L7_uint32 rtrIfNum,
                      L7_uint32 jpgHoldTime)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 rpf[PIMDM_MAX_DBG_ADDR_SIZE];
  interface_bitset_t tempList;
  interface_bitset_t lclMembList;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "Prune Message received From %s on "
               "Interface - %d for Src - %s, Group - %s Addressed to %s with "
               "Holdtime - %d", inetAddrPrint(nbrAddr,nbr), rtrIfNum,
               inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
               inetAddrPrint(upsmNbrAddr,rpf), jpgHoldTime);

  /* Get the (S,G) MRT Entry */
  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr)) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "MRT Lookup Failed for Src - %s, Group - %s",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    return L7_FAILURE;
  }

  /* Update the Max Prune Hold Time.
   */
  pimdmMrtMaxPruneHoldTimeUpdate (mrtEntry, rtrIfNum, jpgHoldTime);

  /* Verify if we received this Packet on our Upstream interface.
   */
  if (mrtEntry->upstrmRtrIfNum == rtrIfNum)  /* Prune on Upstream Interface */
  {
    pimdmUpstrmEventInfo_t upstrmFsmEventInfo;

    /* Validate if the Source is Directly Connected.
     */
    if (inetIsDirectlyConnected (srcAddr, rtrIfNum) == L7_TRUE)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                  "Prune Message received on Interface - %d "
                  "for Src - %s, Group - %s and Source is "
                  "Directly Connected - %s",
                   rtrIfNum, inetAddrPrint(srcAddr,src),
                   inetAddrPrint(grpAddr,grp), inetAddrPrint(srcAddr,src));
      return L7_FAILURE;
    }

    /*************** UPSTREAM FSM EVENTS *********************/

    /* Prepare the Upstream Event Information.
     */
    upstrmFsmEventInfo.rtrIfNum = rtrIfNum;

    /* EVENT - See Prune (S,G).
     */
    pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_PRUNE_ARVL, mrtEntry,
                           &upstrmFsmEventInfo);
  }
  else /* Prune on Downstream Interface */
  {
    pimdmDnstrmEventInfo_t dnstrmFsmEventInfo;
    pimdmAssertEventInfo_t asrtFsmEventInfo;

    /* Validate if the Upstream Neighbor Address is our Interface Address.
     */
    if (inetIsLocalAddress (upsmNbrAddr, rtrIfNum) != L7_TRUE)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                  "Prune Message received on Interface - %d "
                   "for Src - %s, Group - %s is not addressed to Myself",
                   rtrIfNum, inetAddrPrint(srcAddr,src),
                   inetAddrPrint(grpAddr,grp));
      return L7_FAILURE;
    }

    /*************** DOWNSTREAM INTERFACE FSM EVENTS *********************/

    /* Prepare the Downstream Event Information.
     */
    dnstrmFsmEventInfo.rtrIfNum = rtrIfNum;
    dnstrmFsmEventInfo.holdTime = jpgHoldTime;
    inetCopy (&(dnstrmFsmEventInfo.nbrAddr), nbrAddr);

    /* Start the PPT(S,G,I) only when you do not have any local receivers.
     */
    BITX_RESET_ALL (&lclMembList);
    BITX_RESET_ALL (&tempList);

    BITX_COPY (&mrtEntry->pimIncludeStarG, &lclMembList);
    BITX_SUBTRACT (&mrtEntry->pimIncludeStarG, &mrtEntry->pimExcludeSG, &tempList); /* include(*,g) - exclude(s,g) */
    BITX_ADD (&lclMembList, &tempList, &lclMembList);
    BITX_ADD (&lclMembList, &mrtEntry->pimIncludeSG, &lclMembList);
    if ((BITX_TEST (&lclMembList, rtrIfNum)) == 0)
    {
      /* EVENT - Receive Prune (S,G).
       */
      pimdmDnstrmFsmExecute (PIMDM_DNSTRM_EVT_PRN_ARVL, mrtEntry,
                             &dnstrmFsmEventInfo);
    }

    /*************** ASSERT FSM EVENTS *********************/

    /* Prepare the Assert Event Information.
     */
    asrtFsmEventInfo.rtrIfNum = rtrIfNum;
    inetCopy (&(asrtFsmEventInfo.assertAddr), nbrAddr);
    asrtFsmEventInfo.interval = PIMDM_DEFAULT_ASSERT_TIMEOUT;

    /* EVENT - Receive Prune (S,G), Join (S,G) or Graft (S,G).
     */
    pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_PRN_JOIN_GRAFT_ARVL, mrtEntry,
                           &asrtFsmEventInfo);
  }

#if 0
  pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_PRUNE,
                        PIMDM_STATS_RECEIVED);
#endif

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Process the received Graft-Ack Control Message
*
* @param    pimdmCB    - @b{(input)} Pointer to the PIM-DM Control Block
*           srcAddr    - @b{(input)} Address of the Multicast Data Source
*           grpAddr    - @b{(input)} Address of the Multicast Group
*           nbrAddr    - @b{(input)} Received Packet Source Address
*           upsmNbrAddr- @b{(input)} Address of the Upstream Neighbor to
*                                whom this message is addressed to
*           rtrIfNum   - @b{(input)} Received Packet Router Interface Index
*           jpgHoldTime- @b{(input)} Holdtime specified in the Message
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmGraftAckMsgProcess (pimdmCB_t *pimdmCB,
                         L7_inet_addr_t *srcAddr,
                         L7_inet_addr_t *grpAddr,
                         L7_inet_addr_t *nbrAddr,
                         L7_inet_addr_t *upsmNbrAddr,
                         L7_uint32 rtrIfNum,
                         L7_uint32 jpgHoldTime)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  pimdmUpstrmEventInfo_t upstrmFsmEventInfo;
  L7_inet_addr_t tempRpfSAddr;
  L7_inet_addr_t *rpfSAddr = &tempRpfSAddr;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 rpf[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "Graft-Ack Message received From %s on "
               "Interface - %d for Src - %s, Group - %s Addressed to %s with "
               "Holdtime - %d", inetAddrPrint(nbrAddr,nbr), rtrIfNum,
               inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
               inetAddrPrint(upsmNbrAddr,rpf), jpgHoldTime);

  /* Get the (S,G) MRT Entry */
  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr)) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "MRT Lookup Failed for Src - %s, Group - %s",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    return L7_FAILURE;
  }

  /* Verify if the message has come from the RPF'(S).
   */
  inetCopy (rpfSAddr, &(mrtEntry->rpfInfo.rpfNextHop));
  if (L7_INET_ADDR_COMPARE (rpfSAddr, nbrAddr) != 0)
  {
    if(inetIsDirectlyConnected (rpfSAddr, rtrIfNum) != L7_TRUE)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                  "Graft-Ack Message received on Interface - %d "
                  "for Src - %s, Group - %s hasn't come from RPF'(S) - %s",
                  rtrIfNum, inetAddrPrint(srcAddr,src),
                  inetAddrPrint(grpAddr,grp), inetAddrPrint(rpfSAddr,rpf));
      return L7_FAILURE;
    }
  }

  /* Verify if we received this Packet on one our Downstream interfaces.
   */
  if (mrtEntry->upstrmRtrIfNum != rtrIfNum)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Graft-Ack Message received on the Downstrm Interface - %d for "
                 "Src - %s, Group - %s", mrtEntry->upstrmRtrIfNum,
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    return L7_FAILURE;
  }

  /*************** UPSTREAM FSM EVENTS *********************/

  /* Prepare the Upstream Event Information.
   */
  upstrmFsmEventInfo.rtrIfNum = rtrIfNum;

  /* EVENT - Receive GraftAck (S,G) from RPF'(S).
   */
  pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_GRFT_ACK_ARVL, mrtEntry,
                         &upstrmFsmEventInfo);

#if 0
  pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_GRAFT_ACK,
                        PIMDM_STATS_RECEIVED);
#endif

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Process the received Graft Control Message
*
* @param    pimdmCB    - @b{(input)} Pointer to the PIM-DM Control Block
*           srcAddr    - @b{(input)} Address of the Multicast Data Source
*           grpAddr    - @b{(input)} Address of the Multicast Group
*           nbrAddr    - @b{(input)} Received Packet Source Address
*           upsmNbrAddr- @b{(input)} Address of the Upstream Neighbor to
*                                whom this message is addressed to
*           rtrIfNum   - @b{(input)} Received Packet Router Interface Index
*           jpgHoldTime- @b{(input)} Holdtime specified in the Message
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmGraftMsgProcess (pimdmCB_t *pimdmCB,
                      L7_inet_addr_t *srcAddr,
                      L7_inet_addr_t *grpAddr,
                      L7_inet_addr_t *nbrAddr,
                      L7_inet_addr_t *upsmNbrAddr,
                      L7_uint32 rtrIfNum,
                      L7_uint32 jpgHoldTime)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  pimdmDnstrmEventInfo_t dnstrmFsmEventInfo;
  pimdmAssertEventInfo_t asrtFsmEventInfo;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 rpf[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "Graft Message received From %s on "
               "Interface - %d for Src - %s, Group - %s Addressed to %s with "
               "Holdtime - %d", inetAddrPrint(nbrAddr,nbr), rtrIfNum,
               inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
               inetAddrPrint(upsmNbrAddr,rpf), jpgHoldTime);

  /* Validate if the Upstream Neighbor Address is our Interface Address.
   */
  if (inetIsLocalAddress (upsmNbrAddr, rtrIfNum) != L7_TRUE)
  {
    PIMDM_TRACE(PIMDM_DEBUG_CTRL_PKT,"Graft Message received on Interface - %d "
                 "for Src - %s, Group - %s is not addressed to Myself",
                 rtrIfNum, inetAddrPrint(srcAddr,src),
                 inetAddrPrint(grpAddr,grp));
    return L7_FAILURE;
  }

  /* Get the (S,G) MRT Entry */
  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr)) == L7_NULLPTR)
  {
    L7_BOOL  oifIsEmpty = L7_FALSE;

    PIMDM_TRACE(PIMDM_DEBUG_CTRL_PKT,
                "MRT Lookup Failed for Src - %s, Group - %s",
                inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

    /* If the entry is timed out and you receive a Graft from your
     * downstream interface, Do the following:
     * 1. Create the MRT Entry.
     * 2. Compute the OIF List.
     * 3. If OIF is nonNull, send a Graft Upstream.
     */
    /* Create the MRT Entry */
    if ((mrtEntry = pimdmMrtEntryAdd (pimdmCB, srcAddr, grpAddr, rtrIfNum))
                                   == L7_NULLPTR)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                   "Unable to create the MRT entry for Src - %s, Grp - %s",
                   inetAddrPrint (&(mrtEntry->srcAddr), src),
                   inetAddrPrint (&(mrtEntry->grpAddr), grp));
      return L7_FAILURE;
    }

    /* Compute the OIF list */
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

    /* Check if NULL, else Send a Graft upstream */
    BITX_IS_EMPTY (&mrtEntry->oifList, oifIsEmpty);
    if (oifIsEmpty != L7_TRUE)
    {
      pimdmDnstrmEventInfo_t dnstrmFsmEventInfo;
      pimdmUpstrmEventInfo_t upstrmFsmEventInfo;

      /*************** DOWNSTREAM INTERFACE FSM EVENTS *********************/

      /* Prepare the Downstream Event Information.
       */
      dnstrmFsmEventInfo.rtrIfNum = rtrIfNum;
      dnstrmFsmEventInfo.holdTime = jpgHoldTime;
      inetCopy (&(dnstrmFsmEventInfo.nbrAddr), nbrAddr);

      /* EVENT - Receive Graft (S,G).
       */
      pimdmDnstrmFsmExecute (PIMDM_DNSTRM_EVT_GRAFT_ARVL, mrtEntry,
                             &dnstrmFsmEventInfo);

      /*************** UPSTREAM INTERFACE FSM EVENTS *********************/

      /* Prepare the Upstream Event Information.
       */
      upstrmFsmEventInfo.rtrIfNum = mrtEntry->upstrmRtrIfNum;

      /* EVENT - RPF'(S) Changes and OIF(S,G) not NULL.
       */
      pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_RPF_CHNG_OIF_NOT_NULL,
                             mrtEntry, &upstrmFsmEventInfo);
    }

    return L7_SUCCESS;
  }

  /* Verify if we received this Packet on our Upstream interface.
   */
  if (mrtEntry->upstrmRtrIfNum == rtrIfNum)
  {
    PIMDM_TRACE(PIMDM_DEBUG_CTRL_PKT,"Graft Message received on the Upstream "
                "Interface - %d for Src - %s, Group - %s",
                mrtEntry->upstrmRtrIfNum, inetAddrPrint(srcAddr,src),
                inetAddrPrint(grpAddr,grp));
    return L7_FAILURE;
  }

  /*************** DOWNSTREAM INTERFACE FSM EVENTS *********************/

  /* Prepare the Downstream Event Information.
   */
  dnstrmFsmEventInfo.rtrIfNum = rtrIfNum;
  dnstrmFsmEventInfo.holdTime = jpgHoldTime;
  inetCopy (&(dnstrmFsmEventInfo.nbrAddr), nbrAddr);

  /* EVENT - Receive Graft (S,G).
   */
  pimdmDnstrmFsmExecute (PIMDM_DNSTRM_EVT_GRAFT_ARVL, mrtEntry,
                         &dnstrmFsmEventInfo);

  /*************** ASSERT FSM EVENTS *********************/

  /* Prepare the Assert Event Information.
   */
  asrtFsmEventInfo.rtrIfNum = rtrIfNum;
  asrtFsmEventInfo.type = PIMDM_CTRL_PKT_GRAFT;
  inetCopy (&(asrtFsmEventInfo.assertAddr), nbrAddr);
  asrtFsmEventInfo.interval = PIMDM_DEFAULT_ASSERT_TIMEOUT;

  /* EVENT - Receive Prune (S,G), Join (S,G) or Graft (S,G).
   */
  pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_PRN_JOIN_GRAFT_ARVL, mrtEntry,
                         &asrtFsmEventInfo);

#if 0
  pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_GRAFT,
                        PIMDM_STATS_RECEIVED);
#endif

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Process the received Join Control Message
*
* @param    pimdmCB    - @b{(input)} Pointer to the PIM-DM Control Block
*           srcAddr    - @b{(input)} Address of the Multicast Data Source
*           grpAddr    - @b{(input)} Address of the Multicast Group
*           nbrAddr    - @b{(input)} Received Packet Source Address
*           upsmNbrAddr- @b{(input)} Address of the Upstream Neighbor to
*                                whom this message is addressed to
*           rtrIfNum   - @b{(input)} Received Packet Router Interface Index
*           jpgHoldTime- @b{(input)} Holdtime specified in the Message
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmJoinMsgProcess (pimdmCB_t *pimdmCB,
                     L7_inet_addr_t *srcAddr,
                     L7_inet_addr_t *grpAddr,
                     L7_inet_addr_t *nbrAddr,
                     L7_inet_addr_t *upsmNbrAddr,
                     L7_uint32 rtrIfNum,
                     L7_uint32 jpgHoldTime)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 rpf[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "Join Message received From %s on "
               "Interface - %d for Src - %s, Group - %s Addressed to %s with "
               "Holdtime - %d", inetAddrPrint(nbrAddr,nbr), rtrIfNum,
               inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
               inetAddrPrint(upsmNbrAddr,rpf), jpgHoldTime);

  /* Get the (S,G) MRT Entry */
  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr)) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "MRT Lookup Failed for Src - %s, Group - %s",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    return L7_FAILURE;
  }

  /* Validate if the Upstream Neighbor Address is our Interface Address.
   */
#ifdef PIMDM_TBD /* NOTE: This validation will not work for IPv6.
                  * So, added the correct validation below. Commenting
                  * it for the time being and will remove it later.
                  */
  if(pimdmIntfIpAddressGet(pimdmCB,rtrIfNum,&myAddress) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
               "Failed to get Ip address on rtrIfNum %d",rtrIfNum);
    return L7_FAILURE;
  }
  if (L7_INET_ADDR_COMPARE (&myAddress, upsmNbrAddr) != 0)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Join Message received on Interface - %d"
                 "for Src - %s, Group - %s is not addressed to RPF'(S) - %s",
                 rtrIfNum, inetAddrPrint(srcAddr,src),
                 inetAddrPrint(grpAddr,grp), inetAddrPrint(&myAddress,rpf));
    return L7_FAILURE;
  }
#endif /* PIMDM_TBD */
  if (inetIsLocalAddress (upsmNbrAddr, rtrIfNum) != L7_TRUE)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Join Message received on Interface - %d "
                 "for Src - %s, Group - %s with Upstream Nbr - %s "
                 "is not addressed to Myself", rtrIfNum,
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 inetAddrPrint(upsmNbrAddr, rpf));
    return L7_FAILURE;
  }

  /* Verify if we received this Packet on our Upstream interface.
   */
  if (mrtEntry->upstrmRtrIfNum == rtrIfNum)  /* Join on Upstream Interface */
  {
    pimdmUpstrmEventInfo_t upstrmFsmEventInfo;

    /*************** UPSTREAM FSM EVENTS *********************/

    /* Prepare the Upstream Event Information.
     */
    upstrmFsmEventInfo.rtrIfNum = rtrIfNum;

    /* EVENT - See Join (S,G) to RPF'(S).
     */
    pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_JOIN_ARVL_RPF_SRC, mrtEntry,
                           &upstrmFsmEventInfo);
  }
  else /* Join on Downstream Interface */
  {
    pimdmDnstrmEventInfo_t dnstrmFsmEventInfo;
    pimdmAssertEventInfo_t asrtFsmEventInfo;

    /*************** DOWNSTREAM INTERFACE FSM EVENTS *********************/

    /* Prepare the Downstream Event Information.
     */
    dnstrmFsmEventInfo.rtrIfNum = rtrIfNum;
    dnstrmFsmEventInfo.holdTime = jpgHoldTime;
    inetCopy (&(dnstrmFsmEventInfo.nbrAddr), nbrAddr);

    /* EVENT - Receive Join (S,G).
     */
    pimdmDnstrmFsmExecute (PIMDM_DNSTRM_EVT_JOIN_ARVL, mrtEntry,
                           &dnstrmFsmEventInfo);

    /*************** ASSERT FSM EVENTS *********************/

    /* Prepare the Assert Event Information.
     */
    asrtFsmEventInfo.rtrIfNum = rtrIfNum;
    inetCopy (&(asrtFsmEventInfo.assertAddr), nbrAddr);
    asrtFsmEventInfo.interval = PIMDM_DEFAULT_ASSERT_TIMEOUT;

    /* EVENT - Receive Prune (S,G), Join (S,G) or Graft (S,G).
     */
    pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_PRN_JOIN_GRAFT_ARVL, mrtEntry,
                           &asrtFsmEventInfo);
  }

#if 0
  pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_JOIN,
                        PIMDM_STATS_RECEIVED);
#endif

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Process the received Join/Prune/Graft/GraftAck Control
*           packet and dispatch it to the respective packet handlers.
*
* @param    pimdmCB    - @b{(input)} Pointer to the PIM-DM Control Block
*           jpgMsg     - @b{(input)} Received PIM-DM Join/Prune/Graft Message
*           msgLen     - @b{(input)} Length of the received PIM-DM Message
*           rtrIfNum   - @b{(input)} Received Packet Router Interface Index
*           nbrAddr    - @b{(input)} Received Packet Source Address
*           pimPduType - @b{(input)} PIM-DM Control Packet Type
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmJoinPruneGraftMsgReceive (pimdmCB_t *pimdmCB,
                            L7_uchar8 *jpgMsg,
                            L7_uint32 msgLen,
                            L7_uint32 rtrIfNum,
                            L7_inet_addr_t *nbrAddr,
                            PIM_PDU_TYPE_t pimPduType)
{
  L7_uchar8* pimdmMsgEnd = jpgMsg + msgLen;
  pim_encod_uni_addr_t encUpsmNbrAddr; /* Encoded Unicast Format */
  L7_inet_addr_t tempUpsmNbrAddr;
  L7_inet_addr_t *upsmNbrAddr = &tempUpsmNbrAddr;
  L7_inet_addr_t tempGrpAddr;
  L7_inet_addr_t *grpAddr = &tempGrpAddr;
  L7_inet_addr_t tempSrcAddr;
  L7_inet_addr_t *srcAddr = &tempSrcAddr;
  L7_RC_t retVal = L7_SUCCESS;
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uint32 numJoins;
  L7_uint32 numPrunes;
  L7_uint32 numGroups;
  L7_uchar8 reserved;
  L7_ushort16 jpgHoldTime;
  PIMDM_CTRL_PKT_TYPE_t pimdmPktType;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Retrieve and Validate the Neighbor info from whom this packet
   * is received.
   */
  if (pimdmIntfIsNeighborValid (pimdmCB, rtrIfNum, nbrAddr) != L7_TRUE)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid Neighbor - %s on Interface %d",
                 inetAddrPrint(nbrAddr,nbr), rtrIfNum);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "Join/Prune/Graft Message Received from %s "
               "on rtrIfNum - %d", inetAddrPrint(nbrAddr,nbr), rtrIfNum);

  /* NOTE: jpgMsg points after PIM Header */

  /* Read the Upstream Neighbor Address in Encoded Unicast Format */
  memset (&encUpsmNbrAddr, 0, sizeof (pim_encod_uni_addr_t));
  PIM_GET_EUADDR_INET (&encUpsmNbrAddr, jpgMsg);
  inetAddressSet (encUpsmNbrAddr.addr_family, &(encUpsmNbrAddr.addr), upsmNbrAddr);

  /* Read the reserved field from the Packet */
  MCAST_GET_BYTE (reserved, jpgMsg);
  if (reserved != 0)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Reserved field in the PIM-DM Join/Prune Message is not Zero");
  }

  /* Read the numGroups field from the Packet */
  MCAST_GET_BYTE (numGroups, jpgMsg);
  PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "numGroups - %d", numGroups);

  /* Read the holdtime field from the Packet */
  MCAST_GET_SHORT (jpgHoldTime, jpgMsg);

  if (pimPduType == PIM_PDU_JOIN_PRUNE)
  {
    pimdmPktType = PIMDM_CTRL_PKT_JOIN;
  }
  else if (pimPduType == PIM_PDU_GRAFT)
  {
    pimdmPktType = PIMDM_CTRL_PKT_GRAFT;
  }
  else if (pimPduType == PIM_PDU_GRAFT_ACK)
  {
    pimdmPktType = PIMDM_CTRL_PKT_GRAFT_ACK;
  }
  else
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid PIM PDU Type - %d", pimPduType);
    return L7_FAILURE;
  }

  pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, pimdmPktType, PIMDM_STATS_RECEIVED);

  /* Start processing the Groups */
  while (numGroups--)
  {
    pim_encod_grp_addr_t encGrpAddr; /* Encoded Group Format */

    memset (&encGrpAddr, 0, sizeof (pim_encod_grp_addr_t));
    inetAddressZeroSet (pimdmCB->addrFamily, grpAddr);

    /* Read the Group Address in Encoded Group Format */
    memset (&encGrpAddr, 0, sizeof (pim_encod_grp_addr_t));
    PIM_GET_EGADDR_INET (&encGrpAddr, jpgMsg);
    inetAddressSet (encGrpAddr.addr_family, &(encGrpAddr.addr), grpAddr);

    /* Validate the Group Address */
    if (inetIsInMulticast (grpAddr) != L7_TRUE)
      continue;

    /* Read the Number of Joined Sources from the Packet */
    MCAST_GET_SHORT (numJoins, jpgMsg);
    PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "numJoins - %d", numJoins);

    /* Read the Number of Pruned Sources from the Packet */
    MCAST_GET_SHORT (numPrunes, jpgMsg);
    PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "numPrunes - %d", numPrunes);

    /* Start processing the Joins */
    while (numJoins--)
    {
      pim_encod_src_addr_t encSrcAddr; /* Encoded Source Format */

      memset (&encSrcAddr, 0, sizeof (pim_encod_src_addr_t));
      inetAddressZeroSet (pimdmCB->addrFamily, srcAddr);

      /* Read the Joined Source Address in Encoded Source Format */
      memset (&encSrcAddr, 0, sizeof (pim_encod_src_addr_t));
      PIM_GET_ESADDR_INET (&encSrcAddr, jpgMsg);
      inetAddressSet (encSrcAddr.addr_family, &(encSrcAddr.addr), srcAddr);

      /* Start processing the Join, Graft and Graft-Ack Messages */
      switch (pimdmPktType)
      {
        case PIMDM_CTRL_PKT_JOIN:
          if (pimdmJoinMsgProcess (pimdmCB, srcAddr, grpAddr, nbrAddr,
                                   upsmNbrAddr, rtrIfNum, jpgHoldTime)
                                != L7_SUCCESS)
          {
            PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Join Msg Processing Failed "
                         "From %s on Interface %d with S-%s, G-%s",
                         inetAddrPrint(nbrAddr,nbr), rtrIfNum,
                         inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
            retVal = L7_FAILURE;
            continue;
          }

          break;

        case PIMDM_CTRL_PKT_GRAFT:
          if (pimdmGraftMsgProcess (pimdmCB, srcAddr, grpAddr, nbrAddr,
                                    upsmNbrAddr, rtrIfNum, jpgHoldTime)
                                 != L7_SUCCESS)
          {
            PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Graft Msg Processing Failed "
                         "From %s on Interface %d with S-%s, G-%s",
                         inetAddrPrint(nbrAddr,nbr), rtrIfNum,
                         inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
            retVal = L7_FAILURE;
            continue;
          }

          break;

        case PIMDM_CTRL_PKT_GRAFT_ACK:

          if (pimdmGraftAckMsgProcess (pimdmCB, srcAddr, grpAddr, nbrAddr,
                                       upsmNbrAddr, rtrIfNum, jpgHoldTime)
                                    != L7_SUCCESS)
          {
            PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Graft-Ack Msg Processing Failed"
                         "From %s on Interface %d with S-%s, G-%s",
                         inetAddrPrint(nbrAddr,nbr), rtrIfNum,
                         inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
            retVal = L7_FAILURE;
            continue;
          }

          break;

        default:
            PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid combination of J/P/G "
                         "From %s on Interface %d, PacketType - %d with S-%s, G-%s",
                         inetAddrPrint(nbrAddr,nbr), rtrIfNum, pimdmPktType,
                         inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
            retVal = L7_FAILURE;
            continue;
      }
    } /* while (numJoins--) */

    /* Start processing the Prunes */
    while (numPrunes--)
    {
      pim_encod_src_addr_t encSrcAddr; /* Encoded Source Format */

      memset (&encSrcAddr, 0, sizeof (pim_encod_src_addr_t));
      inetAddressZeroSet (pimdmCB->addrFamily, srcAddr);

      /* Read the Pruned Source Address in Encoded Source Format */
      memset (&encSrcAddr, 0, sizeof (pim_encod_src_addr_t));
      PIM_GET_ESADDR_INET (&encSrcAddr, jpgMsg);
      inetAddressSet (encSrcAddr.addr_family, &(encSrcAddr.addr), srcAddr);

      if ((pimPduType == PIM_PDU_GRAFT) ||
          (pimPduType == PIM_PDU_GRAFT_ACK))
      {
        PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid combination of J/P/G "
                     "From %s on Interface %d, PacketType - %d with S-%s, G-%s",
                     inetAddrPrint(nbrAddr,nbr), rtrIfNum, pimdmPktType,
                     inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
        retVal = L7_FAILURE;
        continue;
      }

      if (pimdmPruneMsgProcess (pimdmCB, srcAddr, grpAddr, nbrAddr,
                                upsmNbrAddr, rtrIfNum, jpgHoldTime)
                          != L7_SUCCESS)
      {
        PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Prune Msg Processing Failed "
                     "From %s on Interface %d with S-%s, G-%s",
                     inetAddrPrint(nbrAddr,nbr), rtrIfNum,
                     inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
        retVal = L7_FAILURE;
        continue;
      }
    } /* while (numPrunes--) */
  } /* while (numGroups--) */

  if (jpgMsg != pimdmMsgEnd)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Packet Length Mismatch "
                 "From %s on Interface %d with Remaining Length %d",
                 inetAddrPrint(nbrAddr,nbr), rtrIfNum, (pimdmMsgEnd-jpgMsg));
    retVal = L7_FAILURE;
  }

  if (retVal != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "J/P/G Message Processig Failed "
                 "From %s on Interface %d", inetAddrPrint(nbrAddr,nbr), rtrIfNum);
    return retVal;
  }

  PIMDM_TRACE (PIMDM_DEBUG_CTRL_PKT, "J/P/G Message Processig Success "
               "From %s on Interface %d", inetAddrPrint(nbrAddr,nbr), rtrIfNum);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Process a Hello Control Packet
*
* @param    pimdmCB        @b{(input)} Pointer to the PIM-DM Control Block
* @param    pimdmMsg       @b{(input)} Pointer to the PIM-DM Hello message
* @param    msgLen         @b{(input)} PIM-DM Hello message length
* @param    rtrIfNum       @b{(input)} Router interface the message was received on
* @param    srcAddr        @b{(input)} Pointer to IP Address of the message originator
*
* @returns  L7_SUCCESS     Hello message successfully processed
* @returns  L7_FAILURE     Error processing the Hello message
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmHelloMsgReceive (pimdmCB_t *pimdmCB,
                      L7_uchar8 *pimdmMsg,
                      L7_uint32 msgLen,
                      L7_uint32 rtrIfNum,
                      L7_inet_addr_t *ipSrcAddr)
{
  L7_uchar8          *pMsgEnd = L7_NULLPTR;
  L7_BOOL            bValidHelloHoldTime = L7_FALSE;
  L7_uchar8          src[PIMDM_MAX_DBG_ADDR_SIZE];
  pimdmIntfNbrInfo_t nbrInfo;
  L7_uint32          option;
  L7_uint32          optionLen;
  L7_uchar8          tmpByte;
  L7_ushort16        tmpShort;
  L7_uint32          i;
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic validations */
  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Control Block");
    return L7_FAILURE;
  }
  if(pimdmMsg == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid pimdmMsg");
    return L7_FAILURE;
  }
  if(ipSrcAddr == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid SrcIp Address");
    return L7_FAILURE;
  }
  if(pimdmIntfIsEnabled(pimdmCB, rtrIfNum) != L7_TRUE)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Hello Msg received on Disabled PIM-DM "
        "Interface - %d", rtrIfNum);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_HELLO_PKT, "Hello Message received From %s on "
               "Interface - %d with Length %d",
               inetAddrPrint(ipSrcAddr,nbr), rtrIfNum, msgLen);


  /* Process the packet and extract the options */
  memset(&nbrInfo, 0, sizeof(pimdmIntfNbrInfo_t));
  nbrInfo.pNbrAddr = ipSrcAddr;
  pMsgEnd = pimdmMsg + msgLen;
  while (pimdmMsg < pMsgEnd)
  {
    MCAST_GET_SHORT (option, pimdmMsg);
    MCAST_GET_SHORT (optionLen, pimdmMsg);
    switch(option)
    {
      case PIMDM_HELLO_OPTION_HOLDTIME:
        if(optionLen != PIMDM_HELLO_OPTIONLEN_HOLDTIME)
        {
          PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                      "Invalid Hello HoldTime Option Len %d",optionLen);
          return L7_FAILURE;
        }
        MCAST_GET_SHORT (nbrInfo.holdTime, pimdmMsg);
        bValidHelloHoldTime = L7_TRUE;
        break;
      case PIMDM_HELLO_OPTION_LANPRUNEDELAY:
        if(optionLen != PIMDM_HELLO_OPTIONLEN_LANPRUNEDELAY)
        {
          PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                     "Invalid Lan Prune Delay Option Len %d",optionLen);
          return L7_FAILURE;
        }
        MCAST_GET_BYTE (tmpByte, pimdmMsg);
        MCAST_GET_BYTE (nbrInfo.lanPruneDelay, pimdmMsg);
        nbrInfo.lanPruneDelay += ((tmpByte & 0x7F) << 8);
        MCAST_GET_SHORT (nbrInfo.overrideInterval, pimdmMsg);
        nbrInfo.bValidLanPruneDelay = L7_TRUE;
        break;
      case PIMDM_HELLO_OPTION_GENID:
        if(optionLen != PIMDM_HELLO_OPTIONLEN_GENID)
        {
          PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                      "Invalid Hello HoldTime Option Len %d",optionLen);
          return L7_FAILURE;
        }
        MCAST_GET_LONG (nbrInfo.genId, pimdmMsg);
        nbrInfo.bValidGenId = L7_TRUE;
        break;
      case PIMDM_HELLO_OPTION_STATEREFRESH:
        if(optionLen != PIMDM_HELLO_OPTIONLEN_STATEREFRESH)
        {
          PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                      "Invalid StateRefresh Option Len %d",optionLen);
          return L7_FAILURE;
        }

        MCAST_GET_BYTE (tmpByte, pimdmMsg);
        if(tmpByte != PIMDM_STATEREFRESH_VERSION)
        {
          PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                      "Invalid State Refresh Message Version");
          return L7_FAILURE;
        }
        MCAST_GET_BYTE (nbrInfo.stateRefreshInterval, pimdmMsg);
        MCAST_GET_SHORT (tmpShort, pimdmMsg);
        if (tmpShort != 0)
        {
          PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                     "Reserved field is non-zero in State Refresh Capable "
                     "Hello Option");
        }
        nbrInfo.bStateRefreshCapable = L7_TRUE;
        break;
      default:
        for(i = 0; i < optionLen; i++)
        {
          MCAST_GET_BYTE (tmpByte, pimdmMsg);
          PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                      "Unsupported PIM-DM Hello Option with Value - %d.",
                      tmpByte);
        }

    }
  };   /* End while */
  if(pimdmMsg != pMsgEnd)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Msg Length does not match the options processed");
    return L7_FAILURE; /* Message Length does not match the options processed */
  }

  /* Check for MUST present options */
  if(bValidHelloHoldTime != L7_TRUE)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Hello holdtime option is not set in the received packet");
    return L7_FAILURE;
  }

  /* Process the hello packet options */
  if (pimdmIntfHelloMsgProcess (pimdmCB, rtrIfNum, &nbrInfo) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Hello Msg Processing Failed");
    pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_HELLO,
                          PIMDM_STATS_DROPPED);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_HELLO_PKT, "Hello Message Processing Successful "
               "From %s on Interface - %d", inetAddrPrint (ipSrcAddr, src),
               rtrIfNum);

  pimdmIntfStatsUpdate (pimdmCB, rtrIfNum, PIMDM_CTRL_PKT_HELLO,
                        PIMDM_STATS_RECEIVED);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Process the received Control packet and dispatch it to
*           the respective packet handlers.
*
* @param    pimdmCB    - @b{(input)} Pointer to the PIM-DM Control Block
*           pimdmMsg   - @b{(input)} Received PIM-DM Message
*           msgLen     - @b{(input)} Length of the received PIM-DM Message
*           rtrIfNum   - @b{(input)} Received Packet Router Interface Index
*           ipSrcAddr  - @b{(input)} Received Packet Source Address
*           pimPduType - @b{(input)} PIM-DM Control Packet Type
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmCtrlPktProcess (pimdmCB_t *pimdmCB,
                     L7_uchar8 *pimdmMsg,
                     L7_uint32 msgLen,
                     L7_uint32 rtrIfNum,
                     L7_inet_addr_t *ipSrcAddr,
                     PIM_PDU_TYPE_t pimPduType)
{
  L7_RC_t retVal = L7_FAILURE;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  switch (pimPduType)
  {
    case PIM_PDU_HELLO:
      retVal = pimdmHelloMsgReceive (pimdmCB, pimdmMsg, msgLen, rtrIfNum,
                                     ipSrcAddr);
         break;

    case PIM_PDU_JOIN_PRUNE:
    case PIM_PDU_GRAFT:
    case PIM_PDU_GRAFT_ACK:
      retVal = pimdmJoinPruneGraftMsgReceive (pimdmCB, pimdmMsg, msgLen,
                                             rtrIfNum, ipSrcAddr, pimPduType);
      break;

    case PIM_PDU_ASSERT:
      retVal = pimdmAssertMsgReceive (pimdmCB, pimdmMsg, msgLen, rtrIfNum,
                                      ipSrcAddr);
      break;

    case PIM_PDU_STATE_REFRESH:
      retVal = pimdmStateRefreshMsgReceive (pimdmCB, pimdmMsg, msgLen,
                                            rtrIfNum, ipSrcAddr);
      break;

    default:
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "UNKNOWN PIM-DM Ctrl Pkt received\n"
               "Pkt Type - %d From - %s on Interface %d with Length %d",
               pimPduType, inetAddrPrint(ipSrcAddr,src), rtrIfNum, msgLen);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Receive and Validate the PIM-DM Control Packet
*
* @param    *pimdmCB       - @b{(input)} Pointer to the PIM-DM Control Block
*           *mcastCtrlPkt  - @b{(input)} Pointer to the received Control Packet
*                                    Message
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmCtrlPktReceive (pimdmCB_t *pimdmCB,
                     L7_uint32 rtrIfNum,
                     L7_inet_addr_t *srcAddr,
                     L7_inet_addr_t *destAddr,
                     L7_uchar8 *recvdBuff,
                     L7_uint32 recvdBuffLen)
{
  PIM_PDU_TYPE_t pimPduType = PIM_PDU_MAX;
  L7_uchar8 *pimdmMsg = L7_NULLPTR;
  L7_uint32 msgLen = 0;
  L7_inet_addr_t ipSrcAddr;
  L7_inet_addr_t ipDestAddr;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 dst[PIMDM_MAX_DBG_ADDR_SIZE];
  PIMDM_TRACE_LVL_t trcLvl;

  pimdmDebugPacketRxTxTrace(pimdmCB->addrFamily, L7_TRUE, rtrIfNum, srcAddr,
                            destAddr, recvdBuff, recvdBuffLen);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic Validations */
  if (pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid Control Block");
    return L7_FAILURE;
  }
  if (pimdmCB->operMode == L7_FALSE)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"PIM-DM is not operational");
    return L7_FAILURE;
  }

  /* Update the received buffer information */
  if ((pimdmMsg = (L7_uchar8*) recvdBuff) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Ctrl Pkt Payload is Empty");
    return L7_FAILURE;
  }

  /* Update the received buffer length */
  msgLen = recvdBuffLen;
  if ((msgLen <= 0) ||
      (msgLen > PIMDM_MAX_PDU))
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Ctrl Pkt Payload Length is out of bounds");
    return L7_FAILURE;
  }

  /* Update the Source address and Destination address */
  inetCopy (&ipSrcAddr, srcAddr);
  inetCopy (&ipDestAddr, destAddr);

  /* Packet received on disabled PIM-DM interface */
  if (pimdmIntfIsEnabled (pimdmCB, rtrIfNum) == L7_FALSE)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Ctrl Pkt received on Disabled PIM-DM "
                 "Interface - %d", rtrIfNum);
    return L7_FAILURE;
  }

  /* Verify if the ipSrcAddr is directly connected */
  if (inetIsDirectlyConnected (&ipSrcAddr, rtrIfNum) != L7_TRUE)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Ctrl Pkt received from %s which is not Directly Connected"
                "Interface - %d",inetAddrPrint(&ipSrcAddr,src),rtrIfNum);

    return L7_FAILURE;
  }

  /* Validate the PIM Header */
  if (pimdmHeaderValidate (pimdmCB, pimdmMsg, msgLen, rtrIfNum, &ipSrcAddr,
                           &pimPduType)
                        != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"PIM Header Validation Failed");
    return L7_FAILURE;
  }

  if((pimPduType != PIM_PDU_GRAFT) && (pimPduType != PIM_PDU_GRAFT_ACK ))
  {
    if(inetIsAllPimRouterAddress(&ipDestAddr) != L7_TRUE)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                 "Invalid destination address sent in PIM messages");
      return L7_FAILURE;
    }
  }

  /* Filter out the Trace messages for PIM Hello PDUs, if required. */
  if (pimPduType == PIM_PDU_HELLO)
  {
    trcLvl = PIMDM_DEBUG_HELLO_PKT | PIMDM_DEBUG_CTRL_PKT;
  }
  else
  {
    trcLvl = PIMDM_DEBUG_CTRL_PKT;
  }

  PIMDM_TRACE (trcLvl, "Received PIM-DM [%s] Packet..\n"
                       "    From....... %s\n"
                       "    To......... %s\n"
                       "    Interface.. %d\n"
                       "    Length..... %d\n",
                       gPimdmPduType[pimPduType], inetAddrPrint(&ipSrcAddr,src),
                       inetAddrPrint(&ipDestAddr,dst), rtrIfNum, msgLen);
  pimdmDebugByteDump (pimdmMsg, msgLen, trcLvl);

  /* Update the Received message to point next to Header */
  pimdmMsg += sizeof (pimdmHdr_t);
  msgLen -= sizeof (pimdmHdr_t);

  /* Process the received PIM-DM Control packet */
  if (pimdmCtrlPktProcess (pimdmCB, pimdmMsg, msgLen, rtrIfNum, &ipSrcAddr,
                           pimPduType)
                        != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"PIM Ctrl Pkt Processing Failed "
        "From - %s, Interface - %d", inetAddrPrint(&ipSrcAddr,src),
         rtrIfNum);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

