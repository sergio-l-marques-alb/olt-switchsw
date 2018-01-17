/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmApi.c
*
* @purpose Contains all APIs called from other components.
*
* @component pimsm
*
* @comments
*
* @create 01/01/2006
*
* @author dsatyanarayana
* @end
*
**********************************************************************/

#include "pimsmdefs.h"
#include "pimsmmacros.h"
#include "osapi_support.h"
#include "rto_api.h"
#include "rtmbuf.h"
#include "rtiprecv.h"
#include "l3_mcast_commdefs.h"
#include "pimsmsgrpttree.h"
#include "pimsmsgtree.h"
#include "pimsmstargtree.h"
#include "pimsmstarstarrptree.h"
#include "pimsmtimer.h"
#include "pimsmbsr.h"
#include "pimsmcontrol.h"
#include "pimsmintf.h"
#include "pimsmneighbor.h"
#include "l7apptimer_api.h"
#include "pimsmmrt.h"
#include "mcast_rtmbuf.h"
#include "pimsm_vend_ctrl.h"
/******************************************************************************
*
* @purpose  callback function provided to external components by pimsm
*
* @param  pimsmCbHndle  @b{(input)} pimsm CB Handle
* @param  pMfcEntry     @b{(inout)} Mfc entry.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
L7_RC_t pimsmWholePacketEventQueue(L7_uchar8 familyType,
                                          mfcEntry_t *pMfcEntry)
{
  struct rtmbuf    *mbuf;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_DEBUG,"Entry");
  mbuf = pMfcEntry->m; /* store original buf pointer */
  pMfcEntry->m = rtm_dup(mbuf);/* copy new  buf pointer */
  if (pMfcEntry->m == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,
                "rtm_dup failed for MCAST_MFC_WHOLEPKT_EVENT event");
    /*restore original buf pointer */
    pMfcEntry->m = mbuf;
    return L7_FAILURE;
  }
  /* post event to PIMSM QUEUE */
  if (pimsmMapMessageQueueSend(familyType,
                               MCAST_MFC_WHOLEPKT_EVENT,
                               sizeof(mfcEntry_t),
                               (L7_VOIDPTR)pMfcEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_INFO,"PIMSM message Queue Send  failed");
    if ((pMfcEntry->m->rtm_bufhandle) != L7_NULLPTR)
    {
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(pMfcEntry->m)->rtm_bufhandle);
    }
    rtm_freem(pMfcEntry->m);
    /*restore original buf pointer */
    pMfcEntry->m = mbuf;
    return L7_FAILURE;
  }
  /*restore original buf pointer */
  pMfcEntry->m = mbuf;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION, PIMSM_TRACE_DEBUG,"Exit");
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  MIB : interface entry Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
*           rtrIfNum    @b{(input)} router interface number
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmInterfaceEntryGet(L7_VOIDPTR handle, L7_uint32 rtrIfNum)
{
  pimsmCB_t *pimsmCb;
  L7_RC_t rc;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if (rc  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_INFO,
                "intfEntry is NULL for rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  MIB : interface entry NextGet Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
*           outRtrIfNum @b{(input/output)} router interface number
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmInterfaceEntryNextGet(L7_VOIDPTR handle, L7_uint32 *outRtrIfNum)
{
  pimsmCB_t *pimsmCb;
  L7_uint32 i, rtrIfNumStart = 0;
  L7_BOOL found = L7_FALSE;
  L7_RC_t rc;
  L7_uint32 rtrIfNum;
  pimsmInterfaceEntry_t  *pIntfEntry = L7_NULLPTR;


  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  rtrIfNum = *outRtrIfNum;
  if (rtrIfNum != 0)
  {
    rtrIfNumStart = rtrIfNum;
  }
  for (i = rtrIfNumStart+1; i < MCAST_MAX_INTERFACES; i++)
  {
    rc = pimsmIntfEntryGet(pimsmCb, i, &pIntfEntry);
    if (rc  == L7_SUCCESS)
    {
      *outRtrIfNum = i;
      found = L7_TRUE;
      break;
    }
  }
  if (found == L7_TRUE)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/******************************************************************************
* @purpose interface neighbor count Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
*           rtrIfNum    @b{(input)} router interface number
*           nbrCount    @b{(output)} count
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmInterfaceNeighborCountGet(L7_VOIDPTR handle, L7_uint32 rtrIfNum,
                                       L7_uint32 *nbrCount)
{
  pimsmCB_t *pimsmCb;
  L7_RC_t rc;
  pimsmInterfaceEntry_t *pIntfEntry =L7_NULLPTR;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if (rc  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,
                "intfEntry is NULL for rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;
  }

  *nbrCount = pIntfEntry->pimsmNbrCount;
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  MIB: interface entry Object Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
*           rtrIfNum    @b{(input)} router interface number
*           objId       @b{(input)} pimsm object id
*           output      @b{(output)} object value
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmInterfaceObjectGet(L7_VOIDPTR handle, L7_uint32 rtrIfNum,
                                pimsmInterfaceObjectId_t objId,
                                L7_uint32  *output)
{
  pimsmCB_t *pimsmCb;
  L7_RC_t rc;
  pimsmInterfaceEntry_t *pIntfEntry =L7_NULLPTR;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if (rc  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,
                "intfEntry is NULL for rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;
  }

  switch (objId)
  {
    case I_pimsmInterfaceIfIndex:/* already got it above in params*/
    case I_pimsmInterfaceIPVersion:/* already got it above in params*/
      break;
    case I_pimsmInterfaceAddressType:
    case I_pimsmInterfaceAddress:
    case I_pimsmInterfaceNetMaskLength:
    case I_pimsmInterfaceHelloInterval:
    case I_pimsmInterfaceJoinPruneInterval:
    case I_pimsmInterfaceBSRBorder:
      /* take these params from mapping layer */
      return L7_FAILURE;
    case I_pimsmInterfaceDR:
      inetCopy((L7_inet_addr_t*)output, &pIntfEntry->pimsmInterfaceDR);
      break;
    case I_pimsmInterfaceDRPriority:
      *output = pIntfEntry->pimsmInterfaceDRPriority;
      break;
    case I_pimsmInterfaceHelloHoldtime:
      *output = pIntfEntry->pimsmInterfaceHelloHoldtime;
      break;
    case I_pimsmInterfaceJoinPruneHoldtime:
      *output = pIntfEntry->pimsmInterfaceJoinPruneHoldtime;
      break;
    case I_pimsmInterfaceGenerationIDValue:
      *output = pIntfEntry->pimsmInterfaceGenerationIDValue;
      break;
    case I_pimsmInterfaceTrigHelloInterval:
    case I_pimsmInterfaceDFElectionRobustness:
    case I_pimsmInterfaceUseLanPruneDelay:
    case I_pimsmInterfacePropagationDelay:
    case I_pimsmInterfaceOverrideInterval:
    case I_pimsmInterfaceUseGenerationID:
    case I_pimsmInterfaceUseDRPriority:
    case I_pimsmInterfaceLanDelayEnabled:
    case I_pimsmInterfaceEffectPropagDelay:
    case I_pimsmInterfaceEffectOverrideIvl:
    case I_pimsmInterfaceSuppressionEnabled:
    case I_pimsmInterfaceBidirCapable:
    case I_pimsmInterfaceDRPriorityEnabled:
      return L7_NOT_SUPPORTED;
    default :
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  MIB: neighbor entry Get Routine
*
*
* @param    handle      @b{(input)} pimsm CB Handle
*           rtrIfNum    @b{(input)} router interface number
* @param    pNbrAddr    @b{(input)} neighbor IP Addr
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmNeighborEntryGet(L7_VOIDPTR handle, L7_uint32 rtrIfNum,
                              L7_inet_addr_t *pNbrAddr)
{
  pimsmCB_t *pimsmCb;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  rc = pimsmIntfNbrEntryGet(pimsmCb, rtrIfNum, pNbrAddr, L7_NULLPTR);
  if (rc  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmIntfNbrEntryGet() failed");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  MIB: neighbor entry NextGet Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
*           outRtrIfNum @b{(input/output)} router interface number
* @param    pNbrAddr    @b{(input/output)} neighbor IP Addr
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmNeighborEntryNextGet(L7_VOIDPTR handle, L7_uint32 *outRtrIfNum,
                                  L7_inet_addr_t *pNbrAddr)
{
  pimsmCB_t *pimsmCb;
  L7_RC_t rc, retVal = L7_FAILURE;
  pimsmInterfaceEntry_t *pIntfEntry =L7_NULLPTR;
  pimsmNeighborEntry_t  *pCurrNbrEntry;
  pimsmNeighborEntry_t  *pNextNbrEntry;
  L7_uint32 rtrIfNum;
  L7_uint32 i, rtrIfNumStart = 0;
  L7_BOOL found = L7_FALSE;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  rtrIfNum = *outRtrIfNum;
  if ( rtrIfNum == 0 && inetIsAddressZero(pNbrAddr) == L7_TRUE)
  {
      rtrIfNumStart = 0;
  }
  else
  {
    rtrIfNumStart = rtrIfNum;
  }

  while (1)
  {
     for (i = rtrIfNumStart; i < MCAST_MAX_INTERFACES; i++)
     {
       rc = pimsmIntfEntryGet(pimsmCb, i, &pIntfEntry);
       if (rc  == L7_SUCCESS)
       {
         rtrIfNum = i;
         found = L7_TRUE;
         break;
       }
     }
     if ( i >= MCAST_MAX_INTERFACES )
     {
        break;
     }
      if (found == L7_TRUE)
      {
           /* Lock the Table */
         if(osapiSemaTake(pimsmCb->pimsmNbrLock, L7_WAIT_FOREVER)
            != L7_SUCCESS)
         {
           PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "osapiSemaTake() failed");
           return L7_FAILURE;
         }
         if (inetIsAddressZero(pNbrAddr) != L7_TRUE)
         {
           rc = pimsmNeighborFind(pimsmCb,pIntfEntry, pNbrAddr, &pCurrNbrEntry);
           if (rc  == L7_SUCCESS)
           {
             rc = pimsmNeighborNextGet(pimsmCb,pIntfEntry, pCurrNbrEntry, &pNextNbrEntry);
             if (rc  == L7_SUCCESS)
             {
               inetCopy(pNbrAddr, &pNextNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);
               *outRtrIfNum = rtrIfNum;
               retVal = L7_SUCCESS;
               if(osapiSemaGive(pimsmCb->pimsmNbrLock) != L7_SUCCESS)
               {
                 PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "osapiSemaGive() failed ");
                 return L7_FAILURE;
               }
               return retVal;
             }
           }
         }
         else
         {
           rc = pimsmNeighborGetFirst(pimsmCb,pIntfEntry, &pCurrNbrEntry);
           if (rc  == L7_SUCCESS)
           {
             inetCopy(pNbrAddr, &pCurrNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);
             *outRtrIfNum = rtrIfNum;
             retVal = L7_SUCCESS;
             if(osapiSemaGive(pimsmCb->pimsmNbrLock) != L7_SUCCESS)
             {
               PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "osapiSemaGive() failed ");
               return L7_FAILURE;
             }
             return retVal;
           }
         }
         if(osapiSemaGive(pimsmCb->pimsmNbrLock) != L7_SUCCESS)
         {
           PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "osapiSemaGive() failed ");
           return L7_FAILURE;
         }
      }
      rtrIfNumStart++;
      inetAddressZeroSet(pimsmCb->family, pNbrAddr);
  }
  return retVal;
}
/******************************************************************************
* @purpose   MIB: neighbor entry object Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
*           rtrIfNum    @b{(input)} router interface number
* @param    pNbrAddr    @b{(input)} neighbor IP Addr
*           objId       @b{(input)} pimsm object id
*           output      @b{(output)} object value
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmNeighborObjectGet(L7_VOIDPTR handle, L7_uint32 rtrIfNum,
                               L7_inet_addr_t *pNbrAddr,
                               pimsmNeighborObjectId_t objId,
                               L7_uint32  *output)
{
  pimsmCB_t *pimsmCb;
  L7_RC_t rc = L7_SUCCESS;
  pimsmNeighborEntry_t  *pCurrNbrEntry =L7_NULLPTR;
  L7_uint32 timeLeft = 0;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  if(osapiSemaTake(pimsmCb->pimsmNbrLock, L7_WAIT_FOREVER)
     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "osapiSemaTake() failed");
    return L7_FAILURE;
  }
  rc = pimsmIntfNbrEntryGet(pimsmCb, rtrIfNum, pNbrAddr, &pCurrNbrEntry);
  if (rc  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmNbrEntryGet() failed");
    if(osapiSemaGive(pimsmCb->pimsmNbrLock) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "osapiSemaGive() failed ");
    }
    return L7_FAILURE;
  }

  switch (objId)
  {
    case I_pimsmNeighborIfIndex:  /* already got it above in params*/
    case I_pimsmNeighborAddressType: /* already got it above in params*/
    case I_pimsmNeighborAddress: /* already got it above in params*/
      break;
    case I_pimsmNeighborUpTime:
      *output = (osapiUpTimeRaw() -
                 pCurrNbrEntry->pimsmNeighborCreateTime);
      break;
    case I_pimsmNeighborExpiryTime:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pCurrNbrEntry->pimsmNeighborExpiryTimer, &timeLeft);
      *output =  timeLeft;
      break;
    case I_pimsmNeighborDRPriority:
      *output = pCurrNbrEntry->pimsmNeighborDRPriority;
      break;
    case I_pimsmNeighborGenerationIDValue:
      *output = pCurrNbrEntry->pimsmNeighborGenerationIDValue;
      break;
    case I_pimsmNeighborDRPriorityPresent:
      *output = L7_TRUE;
      break;
    case I_pimsmNeighborLanPruneDelayPresent:
    case I_pimsmNeighborPropagationDelay:
    case I_pimsmNeighborOverrideInterval:
    case I_pimsmNeighborTBit:
    case I_pimsmNeighborGenerationIDPresent:
    case I_pimsmNeighborBidirCapable:
      rc = L7_NOT_SUPPORTED;
      break;
    default :
      rc = L7_FAILURE;
      break;
  }
  if(osapiSemaGive(pimsmCb->pimsmNbrLock) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "osapiSemaGive() failed ");
    return L7_FAILURE;
  }
  return rc;
}
/******************************************************************************
* @purpose  MIB: (*,G) entry Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmStarGEntryGet(L7_VOIDPTR handle,
                           L7_inet_addr_t *pGrpAddr)
{
  pimsmCB_t *pimsmCb;
  pimsmStarGNode_t *pStarGNode =L7_NULLPTR;
  L7_RC_t rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,"Entry");
  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc  = pimsmStarGFind(pimsmCb, pGrpAddr,&pStarGNode);
  if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    return L7_FAILURE;
  }
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," failed pimsmStarGFind()");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Group Address :", pGrpAddr);
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,"Exit");
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  MIB: (*,G) entry NextGet Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmStarGEntryNextGet(L7_VOIDPTR handle,
                               L7_inet_addr_t *pGrpAddr)
{
  pimsmCB_t *pimsmCb;
  pimsmStarGNode_t *pStarGNode;
  pimsmStarGNode_t starGNodeTmp;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  memset (&starGNodeTmp, 0, sizeof (pimsmStarGNode_t));

  inetCopy(&starGNodeTmp.pimsmStarGEntry.pimsmStarGGrpAddress, pGrpAddr);
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc = pimsmStarGEntryFind(pimsmCb, &starGNodeTmp, AVL_NEXT, &pStarGNode);
  if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    return L7_FAILURE;
  }
  if (rc  == L7_SUCCESS)
  {
    inetCopy(pGrpAddr,
             &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/******************************************************************************
* @purpose MIB: (S,G) entry Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
* @param    pSrcAddr    @b{(input)} Source IP Addr

*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmSGEntryGet(L7_VOIDPTR handle,
                        L7_inet_addr_t *pGrpAddr,
                        L7_inet_addr_t *pSrcAddr)
{
  pimsmCB_t *pimsmCb;
  pimsmSGNode_t *pSGNode= L7_NULLPTR;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc = pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode);

  if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore ");
    return L7_FAILURE;
  }
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," failed pimsmSGFind() ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Grp Address :", pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Src Address :", pSrcAddr);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  MIB: (S,G) entry NextGet Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
* @param    pSrcAddr    @b{(input)} Source IP Addr

*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmSGEntryNextGet(L7_VOIDPTR handle,
                            L7_inet_addr_t *pGrpAddr,
                            L7_inet_addr_t *pSrcAddr)
{
  pimsmCB_t *pimsmCb;
  pimsmSGNode_t *pSGNode;
  pimsmSGNode_t sgNodeTmp;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  memset (&sgNodeTmp, 0, sizeof (pimsmSGNode_t));

  inetCopy(&sgNodeTmp.pimsmSGEntry.pimsmSGGrpAddress, pGrpAddr);
  inetCopy(&sgNodeTmp.pimsmSGEntry.pimsmSGSrcAddress, pSrcAddr);
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc = pimsmSGEntryFind(pimsmCb, &sgNodeTmp, AVL_NEXT, &pSGNode);

  if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore ");
    return L7_FAILURE;
  }

  if (rc  == L7_SUCCESS)
  {
    inetCopy(pGrpAddr,
           &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
    inetCopy(pSrcAddr,
           &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/******************************************************************************
* @purpose  MIB: (S,G,rpt) entry Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
* @param    pSrcAddr    @b{(input)} Source IP Addr

*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmSGRptEntryGet(L7_VOIDPTR handle,
                           L7_inet_addr_t *pGrpAddr,
                           L7_inet_addr_t *pSrcAddr)
{
  pimsmCB_t *pimsmCb;
  pimsmSGRptNode_t *pSGRptNode = L7_NULLPTR;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
    /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGRptTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc = pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode);
  if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    return L7_FAILURE;
  }
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," failed pimsmSGRptFind()");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Group Address :", pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Src Address :", pSrcAddr);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  MIB: (S,G,rpt) entry NextGet Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
* @param    pSrcAddr    @b{(input)} Source IP Addr
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmSGRptEntryNextGet(L7_VOIDPTR handle,
                               L7_inet_addr_t *pGrpAddr,
                               L7_inet_addr_t *pSrcAddr)
{

  pimsmCB_t *pimsmCb;
  pimsmSGRptNode_t *pSGRptNode;
  pimsmSGRptNode_t sgRptNodeTmp;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  memset (&sgRptNodeTmp, 0, sizeof (pimsmSGRptNode_t));

  inetCopy(&sgRptNodeTmp.pimsmSGRptEntry.pimsmSGRptGrpAddress, pGrpAddr);
  inetCopy(&sgRptNodeTmp.pimsmSGRptEntry.pimsmSGRptSrcAddress, pSrcAddr);
    /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGRptTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc = pimsmSGRptEntryFind(pimsmCb, &sgRptNodeTmp, AVL_NEXT, &pSGRptNode);
  if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    return L7_FAILURE;
  }
  if (rc  == L7_SUCCESS)
  {
    inetCopy(pGrpAddr,
           &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress);
    inetCopy(pSrcAddr,
           &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress);
    return L7_SUCCESS;
  }
  return L7_FAILURE;

}


/******************************************************************************
* @purpose  MIB: (*,G,I) entry Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
*           rtrIfNum    @b{(input)} router interface number
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmStarGIEntryGet(L7_VOIDPTR handle,
                            L7_inet_addr_t *pGrpAddr,
                            L7_uint32 rtrIfNum)
{
  pimsmCB_t *pimsmCb;
  pimsmStarGNode_t *pStarGNode;
  pimsmStarGIEntry_t   *pStarGIEntry;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc  = pimsmStarGFind(pimsmCb, pGrpAddr,&pStarGNode);
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," failed pimsmStarGFind()");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Group Address :", pGrpAddr);
    if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    }
    return L7_FAILURE;
  }
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if ( pStarGIEntry == L7_NULLPTR )
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,
                "pStarGIEntry is Null. rtrIfNum=%d ", rtrIfNum);
    if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    }
    return L7_FAILURE;
  }
  if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  MIB: (*,G,I) entry OIF NextGet Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pStarGNode  @b{(input)} (*,G) Node
*           outRtrIfNum @b{(input/output)} router interface number
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmStarGEntryOIFNextGet(pimsmCB_t *pimsmCb,
                                         pimsmStarGNode_t *pStarGNode,
                                         L7_uint32 *outRtrIfNum)
{
  L7_uint32 i, rtrIfNumStart;
  L7_uint32 rtrIfNum;
  interface_bitset_t oif;

  pimsmStarGImdtOlist(pimsmCb, pStarGNode, &oif);
  rtrIfNum = *outRtrIfNum;
  if (rtrIfNum == 0)
  {
      rtrIfNumStart = 1;
  }
  else
  {
     rtrIfNumStart = rtrIfNum + 1;
   }
  for (i = rtrIfNumStart; i < MCAST_MAX_INTERFACES; i++)
  {
    if (BITX_TEST(&oif, i) != L7_NULL)
    {
      *outRtrIfNum = i;
      return L7_SUCCESS;
    }
  }
  *outRtrIfNum = 0;
  return L7_FAILURE;
}

/******************************************************************************
* @purpose  MIB: (*,G,I) entry NextGet Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input/output)} Group IP Addr
*           outRtrIfNum @b{(input/output)} router interface number
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmStarGIEntryNextGet(L7_VOIDPTR handle,
                                L7_inet_addr_t *pGrpAddr,
                                L7_uint32 *outRtrIfNum)
{
  pimsmCB_t *pimsmCb;
  pimsmStarGNode_t *pStarGNode =L7_NULLPTR;
  pimsmStarGNode_t StarGNodeTemp;
  L7_inet_addr_t grpAddr;
  L7_uint32 rtrIfNum = *outRtrIfNum;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  if (inetIsAddressZero(pGrpAddr) == L7_TRUE && *outRtrIfNum == 1)
  {
    /* g =0 && iif = 1*/
    rc = pimsmStarGFirstGet(pimsmCb, &pStarGNode);
    if (rc == L7_SUCCESS)
    {
      inetCopy(&grpAddr,
               &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
      rtrIfNum = 0;
    }
  } else
  {
    rc = pimsmStarGFind(pimsmCb, pGrpAddr,&pStarGNode);
    if (rc != L7_SUCCESS)
    {
      if (pStarGNode == L7_NULLPTR)
      {
        memset (&StarGNodeTemp, 0, sizeof(pimsmStarGNode_t));
        inetCopy(&StarGNodeTemp.pimsmStarGEntry.pimsmStarGGrpAddress,
                 pGrpAddr);
        rc = pimsmStarGNextGet(pimsmCb, &StarGNodeTemp,
                               &pStarGNode);
      }
      else
      {
        rc = pimsmStarGNextGet(pimsmCb, pStarGNode,
                               &pStarGNode);
      }

      if (rc  == L7_SUCCESS)
      {
        inetCopy(&grpAddr,
                 &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
        rtrIfNum = 0; 
      }
    }
  }
  while (rc == L7_SUCCESS)
  {
    if (pimsmStarGEntryOIFNextGet(pimsmCb, pStarGNode, &rtrIfNum)
        != L7_SUCCESS)
    {
      rc = pimsmStarGNextGet(pimsmCb, pStarGNode,
                             &pStarGNode);
      if (rc == L7_SUCCESS)
      {
        inetCopy(pGrpAddr,
                 &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
        rtrIfNum = 0;
        if (pimsmStarGEntryOIFNextGet(pimsmCb, pStarGNode, &rtrIfNum) == L7_SUCCESS)
        {
          *outRtrIfNum = rtrIfNum;
          if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
            return L7_FAILURE;
          }
          return L7_SUCCESS;
        }
      }
    } else
    {
      inetCopy(pGrpAddr, &grpAddr);
      *outRtrIfNum = rtrIfNum;
      if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
        return L7_FAILURE;
      }
      return L7_SUCCESS;
    }
  }
  if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    return L7_FAILURE;
  }
  return L7_FAILURE;
}


/******************************************************************************
* @purpose  MIB: (S,G,I) entry Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
* @param    pSrcAddr    @b{(input)} Source IP Addr
*           rtrIfNum    @b{(input)} router interface number
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmSGIEntryGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr,
                         L7_uint32 rtrIfNum)
{
  pimsmCB_t *pimsmCb;
  pimsmSGNode_t *pSGNode =L7_NULLPTR;
  pimsmSGIEntry_t    *pSGIEntry =L7_NULLPTR;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc = pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode);

  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," failed pimsmSGFind() ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Grp Address :", pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Src Address :", pSrcAddr);
    if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore ");
    }
    return L7_FAILURE;
  }
  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if ( pSGIEntry == L7_NULLPTR )
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,
                "pSGIEntry is Null. rtrIfNum=%d ", rtrIfNum);
    if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore ");
    }
    return L7_FAILURE;
  }
  if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore ");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  MIB: (S,G,I) entry OIF NextGet Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pSGNode     @b{(input)} (S,G) Node
*           outRtrIfNum @b{(input/output)} router interface number
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmSGEntryOIFNextGet(pimsmCB_t *pimsmCb,
                                      pimsmSGNode_t *pSGNode,
                                      L7_uint32 *outRtrIfNum)
{
  L7_uint32 i, rtrIfNumStart = 1;
  L7_uint32 rtrIfNum;
  interface_bitset_t oif;

  pimsmSGInhrtdOlist(pimsmCb, pSGNode, &oif);
  rtrIfNum = *outRtrIfNum;
  if (rtrIfNum == 0)
  {
      rtrIfNumStart = 1;
  }
  else
  {
     rtrIfNumStart = rtrIfNum + 1;
   }
  for (i = rtrIfNumStart; i < MCAST_MAX_INTERFACES; i++)
  {
    if (BITX_TEST(&oif, i) != L7_NULL)
    {
      *outRtrIfNum = i;
      return L7_SUCCESS;
    }
  }
  *outRtrIfNum = 0;
  return L7_FAILURE;

}

/******************************************************************************
* @purpose  MIB: (S,G,I) entry NextGet Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input/output)} Group IP Addr
* @param    pSrcAddr    @b{(input/output)} Source IP Addr
*           outRtrIfNum @b{(input/output)} router interface number
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmSGIEntryNextGet(L7_VOIDPTR handle,
                             L7_inet_addr_t *pGrpAddr,
                             L7_inet_addr_t *pSrcAddr,
                             L7_uint32 *outRtrIfNum)
{
  pimsmCB_t *pimsmCb;
  pimsmSGNode_t *pSGNode =L7_NULLPTR;
  pimsmSGNode_t SGNodeTemp;
  L7_uint32 rtrIfNum = *outRtrIfNum;
  L7_inet_addr_t grpAddr, srcAddr;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  if (inetIsAddressZero(pGrpAddr) == L7_TRUE &&
      inetIsAddressZero(pSrcAddr) == L7_TRUE  &&
      *outRtrIfNum == 1)
  {
    /* g =0 && s = 0 iif = 1*/
    rc = pimsmSGFirstGet(pimsmCb, &pSGNode);
    if (rc == L7_SUCCESS)
    {
      inetCopy(&grpAddr,
               &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
      inetCopy(&srcAddr,
               &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
      rtrIfNum = 0;
    }
  } else
  {
    rc = pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode);
    if (rc != L7_SUCCESS)
    {
      if (pSGNode == L7_NULLPTR)
      {
        memset (&SGNodeTemp, 0, sizeof(pimsmSGNode_t));
        inetCopy(&SGNodeTemp.pimsmSGEntry.pimsmSGGrpAddress,
                 pGrpAddr);
        inetCopy(&SGNodeTemp.pimsmSGEntry.pimsmSGSrcAddress,
                 pSrcAddr);
        rc = pimsmSGNextGet(pimsmCb, &SGNodeTemp,
                            &pSGNode);
      }
      else
      {
        rc = pimsmSGNextGet(pimsmCb, pSGNode,
                            &pSGNode);
      }
      if (rc  == L7_SUCCESS)
      {
        inetCopy(&grpAddr,
                 &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
        inetCopy(&srcAddr,
                 &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
        rtrIfNum = 0;
      }
    }
  }
  while (rc == L7_SUCCESS)
  {
    if (pimsmSGEntryOIFNextGet(pimsmCb, pSGNode, &rtrIfNum)
        != L7_SUCCESS)
    {
      rc = pimsmSGNextGet(pimsmCb, pSGNode,
                          &pSGNode);
      if (rc == L7_SUCCESS)
      {
        inetCopy(&grpAddr,
                 &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
        inetCopy(&srcAddr,
                 &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
        rtrIfNum = 0;
        if (pimsmSGEntryOIFNextGet(pimsmCb, pSGNode, &rtrIfNum) == L7_SUCCESS)
        {
          inetCopy(pGrpAddr, &grpAddr);
          inetCopy(pSrcAddr, &srcAddr);
          *outRtrIfNum = rtrIfNum;
          if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore ");
            return L7_FAILURE;
          }
          return L7_SUCCESS;
        }
      }
    } else
    {
      inetCopy(pGrpAddr, &grpAddr);
      inetCopy(pSrcAddr, &srcAddr);
      *outRtrIfNum = rtrIfNum;
      if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore ");
        return L7_FAILURE;
      }
      return L7_SUCCESS;
    }
  }
  if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore ");
    return L7_FAILURE;
  }
  return L7_FAILURE;
}

/******************************************************************************
* @purpose  MIB: (S,G,rpt,I) entry Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
* @param    pSrcAddr    @b{(input)} Source IP Addr
*           rtrIfNum    @b{(input)} router interface number
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmSGRptIEntryGet(L7_VOIDPTR handle,
                            L7_inet_addr_t *pGrpAddr,
                            L7_inet_addr_t *pSrcAddr,
                            L7_uint32 rtrIfNum)
{
  pimsmCB_t *pimsmCb;
  pimsmSGRptNode_t *pSGRptNode;
  pimsmSGRptIEntry_t   *pSGRptIEntry;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
    /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGRptTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc = pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode);

  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," failed pimsmSGRptFind()");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Group Address :", pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Src Address :", pSrcAddr);
    if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    }
    return L7_FAILURE;
  }
  pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[rtrIfNum];
  if ( pSGRptIEntry == L7_NULLPTR )
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,
                "pSGRptIEntry is Null. rtrIfNum=%d ", rtrIfNum);
    if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    }
    return L7_FAILURE;
  }
  if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  MIB: (S,G,rpt,I) entry OIF NextGet Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pSGRptNode  @b{(input)} (S,G,rpt) Node
*           outRtrIfNum @b{(input/output)} router interface number
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmSGRptEntryOIFNextGet(pimsmCB_t *pimsmCb,
                                         pimsmSGRptNode_t *pSGRptNode,
                                         L7_uint32 *outRtrIfNum)
{
  L7_uint32 i, rtrIfNumStart = 1;
  L7_uint32 rtrIfNum;
  interface_bitset_t oif;

  pimsmSGRptInhrtdOlist(pimsmCb, pSGRptNode, &oif);
  rtrIfNum = *outRtrIfNum;

  if (rtrIfNum == 0)
  {
      rtrIfNumStart = 1;
  }
  else
  {
     rtrIfNumStart = rtrIfNum + 1;
   }
  for (i = rtrIfNumStart; i < MCAST_MAX_INTERFACES; i++)
  {
    if (BITX_TEST(&oif, i) != L7_NULL)
    {
      *outRtrIfNum = i;
      return L7_SUCCESS;
    }
  }
  *outRtrIfNum = 0;
  return L7_FAILURE;
}
/******************************************************************************
* @purpose  MIB: (S,G,rpt,I) entry NextGet Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input/output)} Group IP Addr
* @param    pSrcAddr    @b{(input/output)} Source IP Addr
*           outRtrIfNum @b{(input/output)} router interface number
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmSGRptIEntryNextGet(L7_VOIDPTR handle,
                                L7_inet_addr_t *pGrpAddr,
                                L7_inet_addr_t *pSrcAddr,
                                L7_uint32 *outRtrIfNum)
{
  pimsmCB_t *pimsmCb;
  pimsmSGRptNode_t *pSGRptNode =L7_NULLPTR;
  pimsmSGRptNode_t SGRptNodeTemp;
  L7_uint32 rtrIfNum = *outRtrIfNum;
  L7_inet_addr_t grpAddr, srcAddr;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
    /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGRptTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  if (inetIsAddressZero(pGrpAddr) == L7_TRUE &&
      inetIsAddressZero(pSrcAddr) == L7_TRUE  &&
      *outRtrIfNum == 1)
  {
    /* g =0 && s = 0 && iif = 1*/
    rc = pimsmSGRptFirstGet(pimsmCb, &pSGRptNode);
    if (rc == L7_SUCCESS)
    {
      inetCopy(&grpAddr,
               &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress);
      inetCopy(&srcAddr,
               &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress);
      rtrIfNum = 0;
    }
  } else
  {
    rc = pimsmSGRptFind(pimsmCb, pGrpAddr,pSrcAddr, &pSGRptNode);
    if (rc != L7_SUCCESS)
    {
      if (pSGRptNode == L7_NULLPTR)
      {
        memset (&SGRptNodeTemp, 0, sizeof(pimsmSGRptNode_t));
        inetCopy(&SGRptNodeTemp.pimsmSGRptEntry.pimsmSGRptGrpAddress,
                 pGrpAddr);
        inetCopy(&SGRptNodeTemp.pimsmSGRptEntry.pimsmSGRptSrcAddress,
                 pSrcAddr);
        rc = pimsmSGRptNextGet(pimsmCb, &SGRptNodeTemp,
                               &pSGRptNode);
      }
      else
      {
        rc = pimsmSGRptNextGet(pimsmCb, pSGRptNode,
                               &pSGRptNode);
      }
      if (rc  == L7_SUCCESS)
      {
        inetCopy(&grpAddr,
                 &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress);
        inetCopy(&srcAddr,
                 &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress);
        rtrIfNum = 0;
      }
    }
  }
  while (rc == L7_SUCCESS)
  {
    if (pimsmSGRptEntryOIFNextGet(pimsmCb, pSGRptNode, &rtrIfNum)
        != L7_SUCCESS)
    {
      rc = pimsmSGRptNextGet(pimsmCb, pSGRptNode,
                             &pSGRptNode);
      if (rc == L7_SUCCESS)
      {
        inetCopy(&grpAddr,
                 &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress);
        inetCopy(&srcAddr,
                 &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress);
        rtrIfNum = 0;
        if (pimsmSGRptEntryOIFNextGet(pimsmCb, pSGRptNode, &rtrIfNum) == L7_SUCCESS)
        {
          inetCopy(pGrpAddr, &grpAddr);
          inetCopy(pSrcAddr, &srcAddr);
          *outRtrIfNum = rtrIfNum;
          if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
          {
            PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
            return L7_FAILURE;
          }
          return L7_SUCCESS;
        }
      }
    } else
    {
      inetCopy(pGrpAddr, &grpAddr);
      inetCopy(pSrcAddr, &srcAddr);
      *outRtrIfNum = rtrIfNum;
      if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
        return L7_FAILURE;
      }
      return L7_SUCCESS;
    }
  }
  if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    return L7_FAILURE;
  }
  return L7_FAILURE;
}
/******************************************************************************
* @purpose  MIB: (S,G) entry Object Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
* @param    pSrcAddr    @b{(input)} Source IP Addr
*           objId       @b{(input)} pimsm object id
*           output      @b{(output)} object value
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmSGObjectGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr,
                         pimsmSGObjectId_t objId,
                         L7_uint32  *output)
{
  pimsmCB_t *pimsmCb;
  pimsmSGNode_t *pSGNode;
  pimsmSGEntry_t   *pSGEntry;
  L7_uint32 timeLeft = 0;
  L7_RC_t rc, retVal = L7_SUCCESS;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "pimsmCb is NULL");
    return L7_FAILURE;
  }
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGTree.semId, L7_WAIT_FOREVER)
   != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc = pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode);

  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," failed pimsmSGFind() ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Grp Address :", pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Src Address :", pSrcAddr);
    if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore ");
    }
    return L7_FAILURE;
  }

  pSGEntry = &pSGNode->pimsmSGEntry;
  switch (objId)
  {
    case I_pimsmSGAddressType: /* it is a part of index*/
    case I_pimsmSGGrpAddress: /* it is a part of index*/
    case I_pimsmSGSrcAddress: /* it is a part of index*/
      break;
    case I_pimsmSGUpTime:
      *output = (osapiUpTimeRaw() - pSGEntry->pimsmSGCreateTime);
      break;
    case I_pimsmSGpimMode:
      *output = pSGEntry->pimsmSGPimMode;
      break;
    case I_pimsmSGUpstreamJoinState:
      *output = pSGEntry->pimsmSGUpstreamJoinState + 1;
      break;
    case I_pimsmSGUpstreamJoinTimer:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pSGEntry->pimsmSGUpstreamJoinTimer, &timeLeft);
      *output = timeLeft;
      break;
    case I_pimsmSGUpstreamNeighbor:
      inetCopy((L7_inet_addr_t*)output,
               &pSGEntry->pimsmSGUpstreamNeighbor);
      break;
    case I_pimsmSGRPFIfIndex:
      *output = pSGEntry->pimsmSGRPFIfIndex;
      break;
    case I_pimsmSGRPFNextHop:
      inetCopy((L7_inet_addr_t*)output,
               &pSGEntry->pimsmSGRPFNextHop);
      break;
    case I_pimsmSGRPFRouteProtocol:
      *output = pSGEntry->pimsmSGRPFRouteProtocol;
      break;
    case I_pimsmSGRPFRouteAddress:
      inetCopy((L7_inet_addr_t*)output,
               &pSGEntry->pimsmSGRPFRouteAddress);
      break;
    case I_pimsmSGRPFRoutePrefixLength:
      *output = pSGEntry->pimsmSGRPFRoutePrefixLength;
      break;
    case I_pimsmSGRPFRouteMetricPref:
      *output = pSGEntry->pimsmSGRPFRouteMetricPref;
      break;
    case I_pimsmSGRPFRouteMetric:
      *output = pSGEntry->pimsmSGRPFRouteMetric;
      break;
    case I_pimsmSGSPTBit:
      *output = pSGEntry->pimsmSGSPTBit;
      break;
    case I_pimsmSGKeepaliveTimer:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pSGEntry->pimsmSGKeepaliveTimer, &timeLeft);
      *output = timeLeft;
      break;
    case I_pimsmSGDRRegisterState:
      *output = pSGEntry->pimsmSGDRRegisterState + 1;
      break;
    case I_pimsmSGDRRegisterStopTimer:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pSGEntry->pimsmSGDRRegisterStopTimer, &timeLeft);
      *output = timeLeft;
      break;
    case I_pimsmSGRPRegisterPMBRAddress:
      inetCopy((L7_inet_addr_t*)output,
               &pSGEntry->pimsmSGRPRegisterPMBRAddress);
      break;
    default :
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Invalid (S,G) object request");
      retVal = L7_FAILURE;
      break;
  }
  if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore ");
    return L7_FAILURE;
  }
  return retVal;
}
/******************************************************************************
* @purpose  MIB: (S,G,I) entry Object Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
* @param    pSrcAddr    @b{(input)} Source IP Addr
*           rtrIfNum    @b{(input)} router interface number
*           objId       @b{(input)} pimsm object id
*           output      @b{(output)} object value
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmSGIObjectGet(L7_VOIDPTR handle,
                          L7_inet_addr_t *pGrpAddr,
                          L7_inet_addr_t *pSrcAddr,
                          L7_uint32 rtrIfNum,
                          pimsmSGIObjectId_t objId,
                          L7_uint32  *output)
{
  pimsmCB_t *pimsmCb;
  pimsmSGNode_t *pSGNode;
  pimsmSGIEntry_t    *pSGIEntry;
  L7_uint32 timeLeft = 0;
  L7_RC_t rc, retVal = L7_SUCCESS;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc = pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode);


  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," failed pimsmSGFind() ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Grp Address :", pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Src Address :", pSrcAddr);
    if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore ");
    }
    return L7_FAILURE;
  }
  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if ( pSGIEntry == L7_NULLPTR )
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,
                "pSGIEntry is Null. rtrIfNum=%d ", rtrIfNum);
    if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore ");
    }
    return L7_FAILURE;
  }

  switch (objId)
  {
    case I_pimsmSGIIfIndex: /* it is a part of index*/
      break;
    case I_pimsmSGIUpTime:
      *output = (osapiUpTimeRaw() - pSGIEntry->pimsmSGICreateTime);
      break;
    case I_pimsmSGILocalMembership:
      *output = pSGIEntry->pimsmSGILocalMembership;
      break;
    case I_pimsmSGIJoinPruneState:
      *output = pSGIEntry->pimsmSGIJoinPruneState + 1;
      break;
    case I_pimsmSGIPrunePendingTimer:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pSGIEntry->pimsmSGIPrunePendingTimer, &timeLeft);
      *output = timeLeft;
      break;
    case I_pimsmSGIJoinExpiryTimer:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pSGIEntry->pimsmSGIJoinExpiryTimer, &timeLeft);
      *output = timeLeft;
      break;
    case I_pimsmSGIAssertState:
      *output = pSGIEntry->pimsmSGIAssertState + 1;
      break;
    case I_pimsmSGIAssertTimer:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pSGIEntry->pimsmSGIAssertTimer, &timeLeft);
      *output = timeLeft;
      break;
    case I_pimsmSGIAssertWinnerAddress:
      inetCopy((L7_inet_addr_t*)output,
               &pSGIEntry->pimsmSGIAssertWinnerAddress);
      break;
    case I_pimsmSGIAssertWinnerMetricPref:
      *output = pSGIEntry->pimsmSGIAssertWinnerMetricPref;
      break;
    case I_pimsmSGIAssertWinnerMetric:
      *output = pSGIEntry->pimsmSGIAssertWinnerMetric;
      break;
    default :
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Invalid (S,G) object request");
      retVal = L7_FAILURE;
      break;
  }
  if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore ");
    return L7_FAILURE;
  }
  return retVal;
}
/******************************************************************************
* @purpose  MIB: (*,G) entry Object Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
*           objId       @b{(input)} pimsm object id
*           output      @b{(output)} object value
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmStarGObjectGet(L7_VOIDPTR handle,
                            L7_inet_addr_t *pGrpAddr,
                            pimsmStarGObjectId_t objId,
                            L7_uint32 * output)
{
  pimsmCB_t *pimsmCb;
  pimsmStarGNode_t *pStarGNode;
  pimsmStarGEntry_t    *pStarGEntry;
  L7_uint32 timeLeft = 0;
  L7_RC_t rc, retVal = L7_SUCCESS;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc  = pimsmStarGFind(pimsmCb, pGrpAddr,&pStarGNode);
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," failed pimsmStarGFind()");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Group Address :", pGrpAddr);
    if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    }
    return L7_FAILURE;
  }
  pStarGEntry = &pStarGNode->pimsmStarGEntry;
  switch (objId)
  {
    case I_pimsmStarGAddressType:/* it is a part of index*/
    case I_pimsmStarGGrpAddress:/* it is a part of index*/
      break;
    case I_pimsmStarGUpTime:
      *output = (osapiUpTimeRaw() - pStarGEntry->pimsmStarGCreateTime);
      break;
    case I_pimsmStarGpimMode:
      *output = pStarGEntry->pimsmStarGPimMode;
      break;
    case I_pimsmStarGRPAddress:
      inetCopy((L7_inet_addr_t*)output,
               &pStarGEntry->pimsmStarGRPAddress);
      break;
    case I_pimsmStarGRPOrigin:
      *output = pStarGEntry->pimsmStarGRPOrigin;
      break;
    case I_pimsmStarGRPIsLocal:
      *output = pStarGEntry->pimsmStarGRPIsLocal;
      break;
    case I_pimsmStarGUpstreamJoinState:
      *output = pStarGEntry->pimsmStarGUpstreamJoinState + 1;
      break;
    case I_pimsmStarGUpstreamJoinTimer:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pStarGEntry->pimsmStarGUpstreamJoinTimer, &timeLeft);
      *output = timeLeft;
      break;
    case I_pimsmStarGUpstreamNeighbor:
      inetCopy((L7_inet_addr_t*)output,
               &pStarGEntry->pimsmStarGUpstreamNeighbor);
      break;
    case I_pimsmStarGRPFIfIndex:
      *output = pStarGEntry->pimsmStarGRPFIfIndex;
      break;
    case I_pimsmStarGRPFNextHop:
      inetCopy((L7_inet_addr_t*)output,
               &pStarGEntry->pimsmStarGRPFNextHop);
      break;
    case I_pimsmStarGRPFRouteProtocol:
      *output = pStarGEntry->pimsmStarGRPFRouteProtocol;
      break;
    case I_pimsmStarGRPFRouteAddress:
      inetCopy((L7_inet_addr_t*)output,
               &pStarGEntry->pimsmStarGRPFRouteAddress);
      break;
    case I_pimsmStarGRPFRoutePrefixLength:
      *output = pStarGEntry->pimsmStarGRPFRoutePrefixLength;
      break;
    case I_pimsmStarGRPFRouteMetricPref:
      *output = pStarGEntry->pimsmStarGRPFRouteMetricPref;
      break;
    case I_pimsmStarGRPFRouteMetric:
      *output = pStarGEntry->pimsmStarGRPFRouteMetric;
      break;
    default :
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Invalid (S,G) object request");
      retVal = L7_FAILURE;
      break;
  }
  if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    return L7_FAILURE;
  }
  return retVal;
}
/******************************************************************************
* @purpose  MIB: (*,G,I) entry Object Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
*           rtrIfNum    @b{(input)} router interface number
*           objId       @b{(input)} pimsm object id
*           output      @b{(output)} object value
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmStarGIObjectGet(L7_VOIDPTR handle,
                             L7_inet_addr_t *pGrpAddr,
                             L7_uint32 rtrIfNum,
                             pimsmStarGIObjectId_t objId,
                             L7_uint32 *output)
{
  pimsmCB_t *pimsmCb;
  pimsmStarGNode_t *pStarGNode;
  pimsmStarGIEntry_t   *pStarGIEntry;
  L7_uint32 timeLeft = 0;
  L7_RC_t rc, retVal = L7_SUCCESS;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc  = pimsmStarGFind(pimsmCb, pGrpAddr,&pStarGNode);

  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," failed pimsmStarGFind()");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Group Address :", pGrpAddr);
    if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    }
    return L7_FAILURE;
  }
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if ( pStarGIEntry == L7_NULLPTR )
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,
                "pStarGIEntry is Null. rtrIfNum=%d ", rtrIfNum);
    if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    }
    return L7_FAILURE;
  }
  switch (objId)
  {
    case I_pimsmStarGIIfIndex: /* it is a part of index*/
      break;
    case I_pimsmStarGIUpTime:
      *output = (osapiUpTimeRaw() -
                 pStarGIEntry->pimsmStarGICreateTime);
      break;
    case I_pimsmStarGILocalMembership:
      *output = pStarGIEntry->pimsmStarGILocalMembership;
      break;
    case I_pimsmStarGIJoinPruneState:
      *output = pStarGIEntry->pimsmStarGIJoinPruneState + 1;
      break;
    case I_pimsmStarGIPrunePendingTimer:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pStarGIEntry->pimsmStarGIPrunePendingTimer,
                          &timeLeft);
      *output = timeLeft;
      break;
    case I_pimsmStarGIJoinExpiryTimer:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pStarGIEntry->pimsmStarGIJoinExpiryTimer,
                          &timeLeft);
      *output = timeLeft;
      break;
    case I_pimsmStarGIAssertState:
      *output = pStarGIEntry->pimsmStarGIAssertState + 1;
      break;
    case I_pimsmStarGIAssertTimer:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pStarGIEntry->pimsmStarGIAssertTimer,
                          &timeLeft);
      *output = timeLeft;
      break;
    case I_pimsmStarGIAssertWinnerAddress:
      inetCopy((L7_inet_addr_t*)output,
               &pStarGIEntry->pimsmStarGIAssertWinnerAddress);
      break;
    case I_pimsmStarGIAssertWinnerMetricPref:
      *output = pStarGIEntry->pimsmStarGIAssertWinnerMetricPref;
      break;
    case I_pimsmStarGIAssertWinnerMetric:
      *output = pStarGIEntry->pimsmStarGIAssertWinnerMetric;
      break;
    default :
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Invalid (S,G) object request");
      retVal = L7_FAILURE;
      break;
  }
  if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    return L7_FAILURE;
  }
  return retVal;
}
/******************************************************************************
* @purpose  MIB: (S,G,rpt) entry Object Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
* @param    pSrcAddr    @b{(input)} Source IP Addr
*           objId       @b{(input)} pimsm object id
*           output      @b{(output)} object value
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmSGRptObjectGet(L7_VOIDPTR handle,
                            L7_inet_addr_t *pGrpAddr,
                            L7_inet_addr_t *pSrcAddr,
                            pimsmSGRptObjectId_t objId,
                            L7_uint32  *output)
{
  pimsmCB_t *pimsmCb;
  pimsmSGRptNode_t *pSGRptNode;
  pimsmSGRptEntry_t    *pSGRptEntry;
  L7_uint32 timeLeft = 0;
  L7_RC_t rc;
  L7_RC_t retVal = L7_SUCCESS;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGRptTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc = pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode);

  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," failed pimsmSGRptFind()");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Group Address :", pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Src Address :", pSrcAddr);
    if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    }
    return L7_FAILURE;
  }

  pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;
  switch (objId)
  {
    case I_pimsmSGRptSrcAddress:/* it is a part of index*/
      break;
    case I_pimsmSGRptUpTime:
      *output = (osapiUpTimeRaw() -
                 pSGRptEntry->pimsmSGRptCreateTime);
      break;
    case I_pimsmSGRptUpstreamPruneState:
      *output = pSGRptEntry->pimsmSGRptUpstreamPruneState + 1;
      break;
    case I_pimsmSGRptUpstreamOverrideTimer:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pSGRptEntry->pimsmSGRptUpstreamOverrideTimer,
                          &timeLeft);
      *output = timeLeft;
      break;
    default :
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Invalid (S,G) object request");
      retVal = L7_FAILURE;
      break;
  }
  if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    return L7_FAILURE;
  }
  return retVal;
}
/******************************************************************************
* @purpose  MIB: (S,G,rpt,I) entry Object Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
* @param    pSrcAddr    @b{(input)} Source IP Addr
*           rtrIfNum    @b{(input)} router interface number
*           objId       @b{(input)} pimsm object id
*           output      @b{(output)} object value
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmSGRptIObjectGet(L7_VOIDPTR handle,
                             L7_inet_addr_t *pGrpAddr,
                             L7_inet_addr_t *pSrcAddr,
                             L7_uint32 rtrIfNum,
                             pimsmSGRptIObjectId_t objId,
                             L7_uint32  *output)
{
  pimsmCB_t *pimsmCb;
  pimsmSGRptNode_t *pSGRptNode;
  pimsmSGRptIEntry_t   *pSGRptIEntry;
  L7_uint32 timeLeft = 0;
  L7_RC_t rc, retVal = L7_SUCCESS;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
    /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGRptTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc = pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode);
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," failed pimsmSGRptFind()");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Group Address :", pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Src Address :", pSrcAddr);
    if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    }
    return L7_FAILURE;
  }
  pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[rtrIfNum];
  if ( pSGRptIEntry == L7_NULLPTR )
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,
                "pSGRptIEntry is Null. rtrIfNum=%d ", rtrIfNum);
    if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    }
    return L7_FAILURE;
  }

  switch (objId)
  {
    case I_pimsmSGRptIIfIndex:/* it is a part of index*/
      break;
    case I_pimsmSGRptIUpTime:
      *output = (osapiUpTimeRaw() -
                 pSGRptIEntry->pimsmSGRptICreateTime);
      break;
    case I_pimsmSGRptILocalMembership:
      *output = pSGRptIEntry->pimsmSGRptILocalMembership;
      break;
    case I_pimsmSGRptIJoinPruneState:
      *output = pSGRptIEntry->pimsmSGRptIJoinPruneState + 1;
      break;
    case I_pimsmSGRptIPrunePendingTimer:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pSGRptIEntry->pimsmSGRptIPrunePendingTimer,
                          &timeLeft);
      *output = timeLeft;
      break;
    case I_pimsmSGRptIPruneExpiryTimer:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pSGRptIEntry->pimsmSGRptIPruneExpiryTimer,
                          &timeLeft);
      *output = timeLeft;
      break;
    default :
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Invalid (S,G) object request");
      retVal = L7_FAILURE;
      break;
  }
  if(osapiSemaGive(pimsmCb->pimsmSGRptTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    return L7_FAILURE;
  }
  return retVal;
}

/*
 * Enable below code if RFC2934 MRoute table need to be supported.
 * Rightnow it is decided not to be supported for PIMSM
 */
#ifdef PIMSM_TBD

/******************************************************************************
* @purpose
*
* @param    handle      @b{(input)} pimsm CB Handle
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static void pimsmSGToMRoute(pimsmCB_t *pimsmCb, pimsmSGNode_t *pSGNode,
                            pimsmMRouteEntry_t *pMRouteEntry)
{
  L7_uint32 now;
  pimsmSGEntry_t   *pSGEntry;
  pSGEntry = &pSGNode->pimsmSGEntry;
  memset(pMRouteEntry,0 ,sizeof(pimsmMRouteEntry_t));
  inetCopy(&pMRouteEntry->pimsmMRouteGroup,
           &pSGEntry->pimsmSGGrpAddress);
  inetCopy(&pMRouteEntry->pimsmMRouteSource,
           &pSGEntry->pimsmSGSrcAddress);
  now = osapiUpTimeRaw();
  pMRouteEntry->pimsmMRouteUpTime = now - pSGEntry->pimsmSGCreateTime;
  pMRouteEntry->pimsmMRouteType = pSGEntry->pimsmSGPimMode;
  pimsmRpAddressGet(pimsmCb, &pSGEntry->pimsmSGGrpAddress,
                    &pMRouteEntry->pimsmMRouteRPAddress);
  pMRouteEntry->pimsmMRouteRPFIfIndex= pSGEntry->pimsmSGRPFIfIndex;
  inetCopy(&pMRouteEntry->pimsmMRouteRPFNeighbor,
           &pSGEntry->pimsmSGRPFNextHop);
  if (pSGEntry->pimsmSGSPTBit == L7_TRUE)
  {
    pMRouteEntry->pimsmMRouteFlags = PIMSM_MROUTE_SPT;
  }

/* Following params are undefined in (S,G) Entry at present.
   Hopefully there might be added at later stage.
    pMRouteEntry->pimsmMRouteExpiryTime;
    pMRouteEntry->pimsmMRouteUpstreamAssertTimer;
   pMRouteEntry->pimsmMRouteAssertMetric;
   pMRouteEntry->pimsmMRouteAssertMetricPref;
   pMRouteEntry->pimsmMRouteAssertRPTBit;
    pMRouteEntry->pimsmMRouteSourceMask; --- unused here.

 */
}
/******************************************************************************
* @purpose
*
* @param    handle      @b{(input)} pimsm CB Handle
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static void pimsmSGRptToMRoute(pimsmCB_t *pimsmCb, pimsmSGRptNode_t *pSGRptNode,
                               pimsmMRouteEntry_t *pMRouteEntry)
{
  pimsmSGRptEntry_t    *pSGRptEntry;
  pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;
  memset(pMRouteEntry,0 ,sizeof(pimsmMRouteEntry_t));
  inetCopy(&pMRouteEntry->pimsmMRouteGroup,
           &pSGRptEntry->pimsmSGRptGrpAddress);
  inetCopy(&pMRouteEntry->pimsmMRouteSource,
           &pSGRptEntry->pimsmSGRptSrcAddress);
  pMRouteEntry->pimsmMRouteFlags = PIMSM_MROUTE_RPT;
}
/******************************************************************************
* @purpose
*
* @param    handle      @b{(input)} pimsm CB Handle
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static void pimsmStarGToMRoute(pimsmCB_t *pimsmCb,
                               pimsmStarGNode_t *pStarGNode,
                               pimsmMRouteEntry_t *pMRouteEntry)
{
  L7_uint32 now;
  pimsmStarGEntry_t    *pStarGEntry;
  pStarGEntry = &pStarGNode->pimsmStarGEntry;
  memset(pMRouteEntry,0 ,sizeof(pimsmMRouteEntry_t));
  inetCopy(&pMRouteEntry->pimsmMRouteGroup,
           &pStarGEntry->pimsmStarGGrpAddress);
  now = osapiUpTimeRaw();
  pMRouteEntry->pimsmMRouteUpTime = now - pStarGEntry->pimsmStarGCreateTime;
  pMRouteEntry->pimsmMRouteType = pStarGEntry->pimsmStarGPimMode;
  inetCopy(&pMRouteEntry->pimsmMRouteRPAddress,
           &pStarGEntry->pimsmStarGRPAddress);
  pMRouteEntry->pimsmMRouteRPFIfIndex= pStarGEntry->pimsmStarGRPFIfIndex;
  inetCopy(&pMRouteEntry->pimsmMRouteRPFNeighbor,
           &pStarGEntry->pimsmStarGRPFNextHop);
  pMRouteEntry->pimsmMRouteFlags = PIMSM_MROUTE_RPT;

/* Following params are undefined in (*,G) Entry at present.
   Hopefully there might be added at later stage.
    pMRouteEntry->pimsmMRouteExpiryTime;
    pMRouteEntry->pimsmMRouteUpstreamAssertTimer;
   pMRouteEntry->pimsmMRouteAssertMetric;
   pMRouteEntry->pimsmMRouteAssertMetricPref;
   pMRouteEntry->pimsmMRouteAssertRPTBit;
    pMRouteEntry->pimsmMRouteSourceMask; --- unused here.

 */
}

/******************************************************************************
* @purpose
*
* @param    handle      @b{(input)} pimsm CB Handle
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmMRouteEntryFind(pimsmCB_t *pimsmCb,
                                    L7_inet_addr_t *pGrpAddr,
                                    L7_inet_addr_t *pSrcAddr,
                                    pimsmMRouteEntry_t *pMRouteEntry)
{

  pimsmSGNode_t *pSGNode;
  pimsmSGRptNode_t *pSGRptNode;
  pimsmStarGNode_t *pStarGNode;
  pimsmSGNode_t *pSGNodeNext;
  pimsmSGRptNode_t *pSGRptNodeNext;
  pimsmStarGNode_t *pStarGNodeNext;
  L7_BOOL found = L7_FALSE;
  L7_BOOL endLoop = L7_FALSE;

  while (L7_TRUE)
  {

    if ((inetIsAddressZero(pSrcAddr) == L7_TRUE) &&
        (inetIsAddressZero(pGrpAddr) == L7_TRUE))
    {
      if (pimsmStarGFirstGet( pimsmCb, &pStarGNode )== L7_SUCCESS)
      {
        /*  (*,G) entry */
        if (pMRouteEntry != L7_NULLPTR)
        {
          pimsmStarGToMRoute(pimsmCb,
                             pStarGNode, pMRouteEntry);
        }
        inetCopy(pGrpAddr,
                 &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
        inetAddressZeroSet(pimsmCb->family, pSrcAddr);
        found = L7_TRUE;
        break;
      }
      /* (No (*,G) entry, so search
      (S,G) or (S,G,rpt) entry */
      if (pimsmSGFirstGet(pimsmCb, &pSGNode)
          == L7_SUCCESS)
      {
        if (pMRouteEntry != L7_NULLPTR)
        {
          pimsmSGToMRoute(pimsmCb,
                          pSGNode, pMRouteEntry);
        }
        inetCopy(pGrpAddr,
                 &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
        inetCopy(pSrcAddr,
                 &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
        found = L7_TRUE;
        break;
      }
      if (pimsmSGRptFirstGet(pimsmCb, &pSGRptNode)
          == L7_SUCCESS)
      {
        if (pMRouteEntry != L7_NULLPTR)
        {
          pimsmSGRptToMRoute(pimsmCb,
                             pSGRptNode, pMRouteEntry);
        }
        inetCopy(pGrpAddr,
                 &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress);
        inetCopy(pSrcAddr,
                 &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress);
        found = L7_TRUE;
        break;
      }
    }
    else
    {
      /* iterate through (S,G) routing info */
      if (inetIsAddressZero(pSrcAddr) == L7_TRUE)
      {
        if (pimsmSGFirstGet(pimsmCb, &pSGNode)
            == L7_SUCCESS)
        {
          if (pMRouteEntry != L7_NULLPTR)
          {
            pimsmSGToMRoute(pimsmCb,
                            pSGNode, pMRouteEntry);
          }
          inetCopy(pGrpAddr,
                   &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
          inetCopy(pSrcAddr,
                   &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
          found = L7_TRUE;
          break;
        }
        if (pimsmSGRptFirstGet(pimsmCb, &pSGRptNode)
            == L7_SUCCESS)
        {
          if (pMRouteEntry != L7_NULLPTR)
          {
            pimsmSGRptToMRoute(pimsmCb,
                               pSGRptNode, pMRouteEntry);
          }
          inetCopy(pGrpAddr,
                   &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress);
          inetCopy(pSrcAddr,
                   &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress);
          found = L7_TRUE;
          break;
        }
        if (endLoop == L7_TRUE)
        {
          found = L7_FALSE;
          break;
        }
      }
      else
      {
        if (pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode)
            == L7_SUCCESS)
        {
          if (pimsmSGNextGet(pimsmCb, pSGNode,
                             &pSGNodeNext) == L7_SUCCESS)
          {
            if (pMRouteEntry != L7_NULLPTR)
            {
              pimsmSGToMRoute(pimsmCb,
                              pSGNode, pMRouteEntry);
            }
            inetCopy(pGrpAddr,
                     &pSGNodeNext->pimsmSGEntry.pimsmSGGrpAddress);
            inetCopy(pSrcAddr,
                     &pSGNodeNext->pimsmSGEntry.pimsmSGSrcAddress);
            found = L7_TRUE;
            break;
          }
        }
        if (pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode)
            == L7_SUCCESS)
        {
          if (pimsmSGRptNextGet(pimsmCb, pSGRptNode,
                                &pSGRptNodeNext) == L7_SUCCESS)
          {
            if (pMRouteEntry != L7_NULLPTR)
            {
              pimsmSGRptToMRoute(pimsmCb,
                                 pSGRptNode, pMRouteEntry);
            }
            inetCopy(pGrpAddr,
                     &pSGRptNodeNext->pimsmSGRptEntry.pimsmSGRptGrpAddress);
            inetCopy(pSrcAddr,
                     &pSGRptNodeNext->pimsmSGRptEntry.pimsmSGRptSrcAddress);
            found = L7_TRUE;
            break;
          }
          else
          {
            inetAddressZeroSet(pimsmCb->family, pSrcAddr);
            if (endLoop == L7_TRUE)
            {
              found = L7_FALSE;
              break;
            }
          }
        }
      }
    }
    /* exhausted all (S,G) routing entries for this group */
    if (pimsmStarGFind(pimsmCb, pGrpAddr, &pStarGNode)
        == L7_SUCCESS)
    {
      if (pimsmStarGNextGet(pimsmCb, pStarGNode,
                            &pStarGNodeNext) == L7_SUCCESS)
      {
        if (pMRouteEntry != L7_NULLPTR)
        {
          pimsmStarGToMRoute(pimsmCb,
                             pStarGNode, pMRouteEntry);
        }
        inetCopy(pGrpAddr,
                 &pStarGNodeNext->pimsmStarGEntry.pimsmStarGGrpAddress);
        inetAddressZeroSet(pimsmCb->family, pSrcAddr);
        found = L7_TRUE;
        break;
      }
      else
      {
        endLoop = L7_TRUE;
        continue;
      }
    }
    break;
  }
  if ( found == L7_TRUE)
  {
    return L7_SUCCESS;
  }
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,
                   " failed --- Group Addr = ", pGrpAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," Src Addr = ", pSrcAddr);
  inetAddressZeroSet(pimsmCb->family, pGrpAddr);
  inetAddressZeroSet(pimsmCb->family, pSrcAddr);
  return L7_FAILURE;
}


/******************************************************************************
* @purpose
*
* @param    handle      @b{(input)} pimsm CB Handle
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmMRouteEntryOIFNextGet(L7_VOIDPTR handle,
                                   L7_inet_addr_t *pGrpAddr,
                                   L7_inet_addr_t *pSrcAddr,
                                   L7_uint32 *outIntIfNum)
{
  pimsmCB_t *pimsmCb;

  pimsmSGNode_t *pSGNode;
  pimsmSGRptNode_t *pSGRptNode;
  pimsmStarGNode_t *pStarGNode;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  if (inetIsAddressZero(pSrcAddr) != L7_TRUE)
  {
    if (pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode)
        == L7_SUCCESS)
    {
      return pimsmSGEntryOIFNextGet( pimsmCb, pSGNode, outIntIfNum);
    }
    if (pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode)
        == L7_SUCCESS)
    {
      return pimsmSGRptEntryOIFNextGet( pimsmCb, pSGRptNode, outIntIfNum);
    }
  }
  else
  {
    if (pimsmStarGFind(pimsmCb, pGrpAddr, &pStarGNode)
        == L7_SUCCESS)
    {
      return pimsmStarGEntryOIFNextGet( pimsmCb, pStarGNode, outIntIfNum);
    }
  }


  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,
                   " failed pimsmMRouteEntryOIFFind() Group Addr = ", pGrpAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," Src Addr = ", pSrcAddr);
  return L7_FAILURE;
}

/******************************************************************************
* @purpose
*
* @param    handle      @b{(input)} pimsm CB Handle
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*          NOTE: pSrcAddrMask is ignored
* @end
******************************************************************************/
L7_RC_t pimsmMRouteEntryGet(L7_VOIDPTR handle,
                            L7_inet_addr_t *pGrpAddr,
                            L7_inet_addr_t *pSrcAddr,
                            L7_inet_addr_t *pSrcAddrMask)
{
  pimsmCB_t *pimsmCb;
  L7_inet_addr_t lgrpAddr;
  L7_inet_addr_t lsrcAddr;
  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  inetCopy(&lgrpAddr, pGrpAddr);
  inetCopy(&lsrcAddr, pSrcAddr);
  if (pimsmMRouteEntryFind(pimsmCb, &lgrpAddr, &lsrcAddr,
                           L7_NULLPTR) != L7_SUCCESS)
  {
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,
                     " failed pimsmMRouteEntryFind() Group Addr = ", pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," Src Addr = ", pSrcAddr);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose
*
* @param    handle      @b{(input)} pimsm CB Handle
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*       NOTE: pSrcAddrMask is ignored
* @end
******************************************************************************/
L7_RC_t pimsmMRouteEntryObjectGet(L7_VOIDPTR handle,
                                  L7_inet_addr_t *pGrpAddr,
                                  L7_inet_addr_t *pSrcAddr,
                                  L7_inet_addr_t *pSrcAddrMask,
                                  pimsmMRouteObjectId_t objId,
                                  L7_uint32  *output)
{
  pimsmCB_t *pimsmCb;
  pimsmMRouteEntry_t  mrouteEntry;
  L7_inet_addr_t lgrpAddr;
  L7_inet_addr_t lsrcAddr;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  inetCopy(&lgrpAddr, pGrpAddr);
  inetCopy(&lsrcAddr, pSrcAddr);

  if (pimsmMRouteEntryFind(pimsmCb, &lgrpAddr, &lsrcAddr,
                           &mrouteEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,
                     " failed pimsmMRouteEntryFind() Group Addr = ", pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," Src Addr = ", pSrcAddr);
    return L7_FAILURE;
  }

  switch (objId)
  {
    case I_pimsmMRouteGroup:/* it is a part of index*/
    case I_pimsmMRouteSource:/* it is a part of index*/
    case I_pimsmMRouteSourceMask:/* it is a part of index*/
    case I_pimsmMRouteUpTime:
      *output = mrouteEntry.pimsmMRouteUpTime;
      break;
    case I_pimsmMRouteExpiryTime:
      *output = mrouteEntry.pimsmMRouteExpiryTime;
      break;
    case I_pimsmMRouteType:
      *output = mrouteEntry.pimsmMRouteType;
      break;
    case I_pimsmMRouteRPAddress:
      inetCopy((L7_inet_addr_t*)output,
               &mrouteEntry.pimsmMRouteRPAddress);
      break;
    case I_pimsmMRouteRPFIfIndex:
      *output = mrouteEntry.pimsmMRouteRPFIfIndex;
      break;
    case I_pimsmMRouteRPFNeighbor:
      inetCopy((L7_inet_addr_t*)output,
               &mrouteEntry.pimsmMRouteRPFNeighbor);
      break;
    case I_pimsmMRouteUpstreamAssertTimer:
      *output = mrouteEntry.pimsmMRouteUpstreamAssertTimer;
      break;
    case I_pimsmMRouteAssertMetric:
      *output = mrouteEntry.pimsmMRouteAssertMetric;
      break;
    case I_pimsmMRouteAssertMetricPref:
      *output = mrouteEntry.pimsmMRouteAssertMetricPref;
      break;
    case I_pimsmMRouteAssertRPTBit:
      *output = mrouteEntry.pimsmMRouteAssertRPTBit;
      break;
    case I_pimsmMRouteFlags:
      *output = mrouteEntry.pimsmMRouteFlags;
      break;
    default :
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose
*
* @param    handle      @b{(input)} pimsm CB Handle
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*       pSrcAddrMask is ignored
* @end
******************************************************************************/
L7_RC_t pimsmMRouteEntryNextGet(L7_VOIDPTR handle,
                                L7_inet_addr_t *pGrpAddr,
                                L7_inet_addr_t *pSrcAddr,
                                L7_inet_addr_t *pSrcAddrMask)
{
  pimsmCB_t *pimsmCb;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  if (pimsmMRouteEntryFind(pimsmCb, pGrpAddr, pSrcAddr,
                           L7_NULLPTR) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,
                " failed pimsmMRouteEntryNextFind()");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Grp Address :", pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Src Address :", pSrcAddr);
    inetAddressZeroSet(pimsmCb->family, pGrpAddr);
    inetAddressZeroSet(pimsmCb->family, pSrcAddr);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
#endif
/******************************************************************************
* @purpose  MIB: RP-Group Mapping entry Get Routine
*
* @param    handle             @b{(input)} pimsm CB Handle
* @param    origin             @b{(input)} origin type
* @param    pGrpAddrIn         @b{(input)} Group IP Addr
* @param    grpPrefixLength    @b{(input)} Group prefix ength
* @param    pRpAddrIn          @b{(input)} RP IP Addr
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpGrpMappingEntryGet(L7_VOIDPTR handle,
                                  L7_uchar8 origin,
                                  L7_inet_addr_t *pGrpAddrIn,
                                  L7_uchar8 grpPrefixLength,
                                  L7_inet_addr_t *pRpAddrIn)
{
  pimsmCB_t *pimsmCb;
  L7_inet_addr_t  maskAddr,grpAddr, rpAddr;
  pimsmRpGrpNode_t *pRpGrpNode = L7_NULLPTR;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  inetCopy(&grpAddr, pGrpAddrIn);
  inetCopy(&rpAddr, pRpAddrIn);
  inetMaskLenToMask(pimsmCb->family,(L7_uchar8)grpPrefixLength,&maskAddr);

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,"grp = ", &grpAddr);

  if (pimsmRpGrpNodeFind(pimsmCb,grpAddr,maskAddr,rpAddr,origin,&pRpGrpNode) == L7_FAILURE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,"pimsmRpSetFind() failed");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  MIB: RP-Group Mapping entry NextGet Routine
*
* @param    handle           @b{(input)} pimsm CB Handle
* @param    pOrigin          @b{(input/output)} origin type
* @param    pGrpAddrIn       @b{(input/output)} Group IP Addr
* @param    pGrpPrefixLength @b{(input/output)} Group prefix ength
* @param    pRpAddrIn        @b{(input/output)} RP IP Addr
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpGrpMappingEntryNextGet(L7_VOIDPTR handle,
                                      L7_uchar8 *pOrigin,
                                      L7_inet_addr_t *pGrpAddrIn,
                                      L7_uchar8 *pGrpPrefixLength,
                                      L7_inet_addr_t *pRpAddrIn)
{
  pimsmCB_t *pimsmCb;
  L7_inet_addr_t  maskAddr, rpAddr,grpAddr;
  pimsmRpSetNode_t  *pRpSetNode = L7_NULLPTR;
  pimsmRpGrpNode_t  *pRpGrpNode = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  inetCopy(&grpAddr, pGrpAddrIn);
  inetMaskLenToMask(pimsmCb->family,(L7_uchar8)*pGrpPrefixLength,&maskAddr);

  inetCopy(&rpAddr, pRpAddrIn);
  do
  {
    pimsmRpSetNodeFind(pimsmCb,grpAddr,maskAddr,&pRpSetNode);
   if (pRpSetNode == L7_NULLPTR)
    {
      pimsmRpSetNodeNextFind(pimsmCb,grpAddr,maskAddr,&pRpSetNode);
      if (pRpSetNode == L7_NULLPTR)
      {
         (*pOrigin)++;
       inetAddressZeroSet(pimsmCb->family, &grpAddr);
       continue;
      }
   }
    do
      {
      pimsmRpGrpNodeNextFind(pimsmCb,pRpSetNode,rpAddr,*pOrigin,&pRpGrpNode);
        if (pRpGrpNode == L7_NULLPTR)
        {
      inetCopy(&grpAddr, &pRpSetNode->pimsmGrpAddr);
      inetMaskLenToMask(pimsmCb->family,pRpSetNode->pimsmGrpMaskLen,&maskAddr);
          pimsmRpSetNodeNextFind(pimsmCb,grpAddr,maskAddr,&pRpSetNode);
        if (pRpSetNode == L7_NULLPTR)
        {
         break;
      }
      inetAddressZeroSet(pimsmCb->family, &rpAddr);
      pimsmRpGrpNodeNextFind(pimsmCb,pRpSetNode, rpAddr,*pOrigin,&pRpGrpNode);
      }
     if (pRpGrpNode != L7_NULLPTR)
      {
        inetCopy(pGrpAddrIn, &pRpSetNode->pimsmGrpAddr);
        inetCopy(pRpAddrIn, &pRpGrpNode->pimsmRpAddr);
        *pOrigin = (L7_uchar8)pRpGrpNode->pimsmOrigin;
        *pGrpPrefixLength = pRpSetNode->pimsmGrpMaskLen;
       rc = L7_SUCCESS;
      break;
     }
     inetCopy(&grpAddr, &pRpSetNode->pimsmGrpAddr);
      inetMaskLenToMask(pimsmCb->family,pRpSetNode->pimsmGrpMaskLen,&maskAddr);
      pimsmRpSetNodeNextFind(pimsmCb,grpAddr,maskAddr,&pRpSetNode);

     inetAddressZeroSet(pimsmCb->family, &rpAddr);
      }while (pRpSetNode != L7_NULLPTR && rc == L7_FAILURE);

   if (pRpGrpNode != L7_NULLPTR)
     break;
   inetAddressZeroSet(pimsmCb->family, &grpAddr);
   inetAddressZeroSet(pimsmCb->family, &rpAddr);
   (*pOrigin)++;
  }while (*pOrigin <= PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED_OVERRIDE &&
            rc == L7_FAILURE);

  return rc;
}
/******************************************************************************
* @purpose  MIB: RP-Group Mapping entry object Get Routine
*
* @param    handle             @b{(input)} pimsm CB Handle
* @param    origin             @b{(input)} origin type
* @param    pGrpAddrIn         @b{(input)} Group IP Addr
* @param    grpPrefixLength    @b{(input)} Group prefix ength
* @param    pRpAddrIn          @b{(input)} RP IP Addr
*           objId              @b{(input)} pimsm object id
*           output             @b{(output)} object value
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmRpGrpMappingEntryObjectGet(L7_VOIDPTR handle,
                                        L7_uchar8 origin,
                                        L7_inet_addr_t *pGrpAddrIn,
                                        L7_uchar8 grpPrefixLength,
                                        L7_inet_addr_t *pRpAddrIn,
                                        pimsmGroupMappingObjectId_t objId,
                                        L7_uint32  *output)
{
  pimsmCB_t *pimsmCb;
  pimsmRpGrpNode_t *pRpGrpNode = L7_NULLPTR;
  L7_inet_addr_t  maskAddr,rpAddr, grpAddr;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  inetCopy(&grpAddr, pGrpAddrIn);
  inetCopy(&rpAddr, pRpAddrIn);
  inetMaskLenToMask(pimsmCb->family,grpPrefixLength,&maskAddr);

  if (pimsmRpGrpNodeFind(pimsmCb,grpAddr,maskAddr,rpAddr,origin,&pRpGrpNode) == L7_FAILURE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,"pimsmRpSetFind() failed");
    return L7_FAILURE;
  }

  switch (objId)
  {
    case I_pimsmGroupMappingOrigin:/* it is a part of index*/
    case I_pimsmGroupMappingAddressType:/* it is a part of index*/
    case I_pimsmGroupMappingGrpAddress:/* it is a part of index*/
    case I_pimsmGroupMappingGrpPrefixLength:/* it is a part of index*/
    case I_pimsmGroupMappingRPAddress:/* it is a part of index*/
      break;
    case I_pimsmGroupMappingpimMode:
      *output =5; /* it is others as per the mib */
      break;
    case I_pimsmGroupMappingPrecedence:
      *output = 0; /* Not supported */
      break;
    case I_pimsmGroupMappingActive:
      *output = 1; /* Always active */
      break;
    default :
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  MIB: BSR RP-set entry Get Routine
*
* @param    handle             @b{(input)} pimsm CB Handle
* @param    origin             @b{(input)} origin type
* @param    pGrpAddrIn         @b{(input)} Group IP Addr
* @param    grpPrefixLength    @b{(input)} Group prefix ength
* @param    pRpAddrIn          @b{(input)} RP IP Addr
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmBsrRPSetEntryGet(L7_VOIDPTR handle,
                              L7_uchar8 origin,
                              L7_inet_addr_t *pGrpAddrIn,
                              L7_uchar8 grpPrefixLength,
                              L7_inet_addr_t *pRpAddrIn)
{
  pimsmCB_t *pimsmCb;
  pimsmRpGrpNode_t *pRpGrpNode = L7_NULLPTR;
  L7_inet_addr_t  maskAddr,rpAddr, grpAddr;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  inetCopy(&grpAddr, pGrpAddrIn);
  inetCopy(&rpAddr, pRpAddrIn);
  inetMaskLenToMask(pimsmCb->family,(L7_uchar8)grpPrefixLength,&maskAddr);
  if (pimsmRpGrpNodeFind(pimsmCb, grpAddr, maskAddr, rpAddr,origin,&pRpGrpNode) == L7_FAILURE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,"pimsmRpSetFind() failed");
    return L7_FAILURE;
  }
  return L7_SUCCESS;

}
/******************************************************************************
* @purpose  MIB: BSR RP-set entry NextGet Routine
*
* @param    handle           @b{(input)} pimsm CB Handle
* @param    pOrigin          @b{(input/output)} origin type
* @param    pGrpAddrIn       @b{(input/output)} Group IP Addr
* @param    pGrpPrefixLength @b{(input/output)} Group prefix ength
* @param    pRpAddrIn        @b{(input/output)} RP IP Addr
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmBsrRPSetEntryNextGet(L7_VOIDPTR handle,
                                  L7_uchar8 *pOrigin,
                                  L7_inet_addr_t *pGrpAddrIn,
                                  L7_uchar8 *pGrpPrefixLength,
                                  L7_inet_addr_t *pRpAddrIn)
{
  pimsmCB_t *pimsmCb;
  L7_inet_addr_t  maskAddr,grpAddr, rpAddr;
  pimsmRpSetNode_t  *pRpSetNode = L7_NULLPTR;
  pimsmRpGrpNode_t  *pRpGrpNode = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;


  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  inetCopy(&rpAddr, pRpAddrIn);
  inetCopy(&grpAddr, pGrpAddrIn);
  inetMaskLenToMask(pimsmCb->family,(L7_uchar8)*pGrpPrefixLength,&maskAddr);

  do
  {
    pimsmRpSetNodeFind(pimsmCb,grpAddr,maskAddr,&pRpSetNode);
    if (pRpSetNode == L7_NULLPTR)
    {
      pimsmRpSetNodeNextFind(pimsmCb,grpAddr,maskAddr,&pRpSetNode);
      if (pRpSetNode == L7_NULLPTR)
      {
        return L7_FAILURE;
      }
      else
      {
        inetCopy(&grpAddr, &pRpSetNode->pimsmGrpAddr);
        inetMaskLenToMask(pimsmCb->family,pRpSetNode->pimsmGrpMaskLen,&maskAddr);
      }
    }

    if (*pOrigin == 0)
    {
      *pOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR;
    }
    pimsmRpGrpNodeNextFind(pimsmCb,pRpSetNode, rpAddr,*pOrigin, &pRpGrpNode);
    if (pRpGrpNode == L7_NULLPTR)
    {
      pimsmRpSetNodeNextFind(pimsmCb,grpAddr,maskAddr,&pRpSetNode);
      if (pRpSetNode == L7_NULLPTR)
      {
        break;
      }
      inetAddressZeroSet(pimsmCb->family, &rpAddr);
      pimsmRpGrpNodeNextFind(pimsmCb,pRpSetNode, rpAddr,*pOrigin, &pRpGrpNode);
    }

    if (pRpGrpNode != L7_NULLPTR)
    {
      inetCopy(pGrpAddrIn, &pRpSetNode->pimsmGrpAddr);
      inetCopy(pRpAddrIn, &pRpGrpNode->pimsmRpAddr);
      *pOrigin = pRpGrpNode->pimsmOrigin;
      *pGrpPrefixLength = pRpSetNode->pimsmGrpMaskLen;
      if (pRpGrpNode->pimsmOrigin == PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR)
      {
        rc = L7_SUCCESS;
        break;
      }
      if (*pOrigin > PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR)
        return L7_FAILURE;
      *pOrigin = PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR;
      inetAddressZeroSet(pimsmCb->family, &rpAddr);
      inetAddressZeroSet(pimsmCb->family, &grpAddr);
      inetAddressZeroSet(pimsmCb->family, &maskAddr);
      continue;
    }
    inetAddressZeroSet(pimsmCb->family, &rpAddr);
    inetCopy(&grpAddr, &pRpSetNode->pimsmGrpAddr);
    inetMaskLenToMask(pimsmCb->family,pRpSetNode->pimsmGrpMaskLen,&maskAddr);
  }while (rc == L7_FAILURE);

  return rc;
}
/******************************************************************************
* @purpose  MIB: BSR RP-set entry object Get Routine
*
* @param    handle             @b{(input)} pimsm CB Handle
* @param    origin             @b{(input)} origin type
* @param    pGrpAddrIn         @b{(input)} Group IP Addr
* @param    grpPrefixLength    @b{(input)} Group prefix ength
* @param    pRpAddrIn          @b{(input)} RP IP Addr
*           objId              @b{(input)} pimsm object id
*           output             @b{(output)} object value
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmBsrRPSetEntryObjectGet(L7_VOIDPTR handle,
                                    L7_uchar8 origin,
                                    L7_inet_addr_t *pGrpAddrIn,
                                    L7_uchar8 grpPrefixLength,
                                    L7_inet_addr_t *pRpAddrIn,
                                    pimsmBsrRPSetObjectId_t objId,
                                    L7_uint32  *output)
{
  pimsmCB_t *pimsmCb;
  L7_inet_addr_t  maskAddr,grpAddr,rpAddr;
  pimsmRpGrpNode_t   *pRpGrpNode;
  pimsmAddrList_t *pGrpAddrList = L7_NULLPTR;
  L7_uint32     timeLeft;
  L7_RC_t       rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  if(origin != PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"Origin Type is not BSR");
    return L7_FAILURE;
  }
  inetCopy(&grpAddr, pGrpAddrIn);
  inetCopy(&rpAddr, pRpAddrIn);
  inetMaskLenToMask(pimsmCb->family,(L7_uchar8)grpPrefixLength,&maskAddr);
  if (pimsmRpGrpNodeFind(pimsmCb, grpAddr,maskAddr, rpAddr,origin,&pRpGrpNode) == L7_FAILURE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO,"pimsmRpSetFind() failed");
    return L7_FAILURE;
  }

  switch (objId)
  {
    case I_bsrRPSetPriority:
      *output =pRpGrpNode->pimsmRpPriority;
      break;
    case I_bsrRPSetHoldtime:
      *output =pRpGrpNode->pimsmRpHoldTime;
      break;
    case I_bsrRPSetExpiryTime:
      rc = pimsmRpGrpListGrpNodeFind(pimsmCb,pRpGrpNode,
                grpAddr, grpPrefixLength,origin, &pGrpAddrList);
     if (rc != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR," Unable to get Rp-Group node");
      return L7_FAILURE;
     }
      appTimerTimeLeftGet(pimsmCb->timerCb,
            pGrpAddrList->pimsmRpGrpExpiryTimer, &timeLeft);
      *output = timeLeft;
      break;
    default:
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  MIB: Elected BSR entry Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmBsrElectedBSREntryGet(L7_VOIDPTR handle)
{
  pimsmBsrPerScopeZone_t *pBsrNode = L7_NULLPTR;
  pimsmCB_t *pimsmCb;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  pimsmBsrElectedBsrGet(pimsmCb, &pBsrNode);

  if ( pBsrNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"electedBsrEntry is not set");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  MIB: Elected BSR entry NextGet Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmBsrElectedBSREntryNextGet(L7_VOIDPTR handle)
{
  pimsmBsrPerScopeZone_t *pBsrNode = L7_NULLPTR;
  pimsmCB_t *pimsmCb;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  pimsmBsrElectedBsrNextGet(pimsmCb, &pBsrNode);

  if ( pBsrNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"electedBsrEntry is not set");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  MIB: Elected BSR entry Object Get Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
*           objId       @b{(input)} pimsm object id
*           output      @b{(output)} object value
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmBsrElectedBSRObjectGet(L7_VOIDPTR handle,
                                    pimsmBsrElectedBSRObjectId_t objId,
                                    L7_uint32  *output)
{
  pimsmCB_t *pimsmCb;
  pimsmBsrPerScopeZone_t *pBsrNode = L7_NULLPTR;
  L7_uint32 timeLeft = 0;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  pimsmBsrElectedBsrGet(pimsmCb, &pBsrNode);

  if ( pBsrNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"electedBsrEntry is not set");
    return L7_FAILURE;
  }
  switch (objId)
  {
    case I_bsrElectedBSRAddressType:/* it is a part of index*/
      break;
    case I_bsrElectedBSRAddress:
      inetCopy((L7_inet_addr_t*)output, &pBsrNode->zone.cbsr.pimsmElectedBSRAddress);
      break;
    case I_bsrElectedBSRPriority:
      *output = pBsrNode->zone.cbsr.pimsmElectedBSRPriority ;
      break;
    case I_bsrElectedBSRHashMaskLength:
      *output = pBsrNode->pimsmBSRHashMasklen;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_NORMAL, " MaskLen:%d",
                                      pBsrNode->pimsmBSRHashMasklen);
      break;
    case I_bsrElectedBSRExpiryTime:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pBsrNode->pimsmBSRTimer, &timeLeft);
      *output = timeLeft;
      break;
    case I_bsrElectedBSRCRPAdvTimer:
      appTimerTimeLeftGet(pimsmCb->timerCb,
                          pimsmCb->pimsmRpInfo->pimsmCandRpInfo.pimsmCandRpAdvTimer, &timeLeft);
      *output = timeLeft;
      break;
    default:
      return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_NORMAL, "Exit.");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gives RP Hash table for give group address.
*
* @param    handle             @b{(input)} pimsm CB Handle
* @param    *pGrpAddr          @b{(input)} Group Address.
* @param    *pRpAddr           @b{(output)} RP Address.
* @param    *pOrigin           @b{(output)} Origin
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmRpHashGet(L7_VOIDPTR handle,
                       L7_inet_addr_t *pGrpAddr,
                       L7_inet_addr_t *pRpAddr,
                       L7_uchar8 *pOrigin)
{
  pimsmRpGrpNode_t *pRpGrpNode = L7_NULLPTR;
  pimsmCB_t           *pimsmCb = L7_NULLPTR;


  PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_NORMAL, "pimsmRpHashGet Entered.");
  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }

  pimsmRpGrpMappingPreferredGet(pimsmCb,*pGrpAddr, &pRpGrpNode);

  if (pRpGrpNode != L7_NULLPTR)
  {
    inetCopy(pRpAddr,&pRpGrpNode->pimsmRpAddr);
    *pOrigin =  pRpGrpNode->pimsmOrigin;
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_NORMAL, "pimsmRpHashGet Exit with success.");
    return L7_SUCCESS;
  }
  inetAddressZeroSet(pimsmCb->family, pRpAddr);
  *pOrigin = -1;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_NORMAL, "pimsmRpHashGet Exit with failure.");
  return L7_FAILURE;
}
/*********************************************************************
* @purpose Process RTO Timer Event
*
* @param    param      @b{(input)} pimsm CB Handle
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
void pimsmRTOTimerExpiresHandler (void* param)
{
  pimsmCB_t* pimsmCb = (pimsmCB_t *)param;

  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return;
  }

  /* Acquire the RTO Timer Semaphore */
  if (osapiSemaTake (pimsmCb->rtoTimerSemId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,
                "PIMSM RTO Timer Semaphore Take Failed");
    return;
  }

  if (pimsmCb->rtoTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM rtoTimer is NULL, But Still Expired");

    /* Release the RTO Timer Semaphore */
    osapiSemaGive (pimsmCb->rtoTimerSemId);
    return;
  }
  pimsmCb->rtoTimer = L7_NULLPTR;

  /* Release the RTO Timer Semaphore */
  osapiSemaGive (pimsmCb->rtoTimerSemId);

  if (pimsmMapMessageQueueSend(pimsmCb->family,
                               MCAST_EVENT_RTO_BEST_ROUTE_CHANGE,
                               0,
                               L7_NULLPTR) != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_INFO,
                 "PIMSM: RTO Event Post Failed.\n");
  }
  return;
}

/*********************************************************************
* @purpose  To Start RTo Timer
*
* @param    pimsmCbHandle      @b{(input)} pimsm CB Handle
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/

void pimsmRtoStartTimer (MCAST_CB_HNDL_t pimsmCbHandle)
{
  pimsmCB_t        *pimsmCb = (pimsmCB_t *) pimsmCbHandle;

  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,
         "PIMSM CB is null");
    return;
  }

  /* Acquire the RTO Timer Semaphore */
  if (osapiSemaTake (pimsmCb->rtoTimerSemId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_UI_INTERACTION, PIMSM_TRACE_ERROR,
                "PIMSM RTO Timer Semaphore Take Failed");
    return;
  }

  /* Add the RTO Timer */
  if (pimsmCb->rtoTimer == L7_NULLPTR)
  {
    if (pimsmUtilAppTimerSet (pimsmCb, pimsmRTOTimerExpiresHandler, (void*) pimsmCb,
                              L7_PIM_BEST_ROUTE_DELAY, &(pimsmCb->rtoTimer),
                              "SM-RTO") != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "RTO Timer Add Failed");

      /* Release the RTO Timer Semaphore */
      osapiSemaGive (pimsmCb->rtoTimerSemId);
      return;
    }
  }

  /* Release the RTO Timer Semaphore */
  osapiSemaGive (pimsmCb->rtoTimerSemId);
  return;
}

/******************************************************************************
* @purpose      Wrapper to get Default Hash Mask Len
*
* @param        family   @b{(input)}
* @param        pMaskLen  @b{(output)} mask length
*
* @returns       L7_SUCCESS or L7_FAILURE
*
* @comments
* @end
******************************************************************************/
L7_RC_t pimsmDefaultHashMaskLenGet(L7_uchar8 family,L7_uchar8 *pMaskLen)
{
  PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_MAX, "Enter");
  if(L7_NULLPTR == pMaskLen)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,
                "Invalid input parameter");
    return L7_FAILURE;
  }
  if(family == L7_AF_INET)
  {
    *pMaskLen = L7_PIMSM_CBSR_HASH_MASK_LENGTH;
  }
  else if(family == L7_AF_INET6)
  {
    *pMaskLen = L7_PIMSM6_CBSR_HASH_MASK_LENGTH;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_MAX, "Exit");
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  (*,G) entry OIF GET Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
*           pOif        @b{(output)} oif-list
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmStarGEntryOIFGet(L7_VOIDPTR handle,
                                         L7_inet_addr_t *pGrpAddr,
                                         interface_bitset_t *pOif)
{
  pimsmCB_t *pimsmCb;
  pimsmStarGNode_t *pStarGNode =L7_NULLPTR;
  interface_bitset_t scopedOifList;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
   PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Group Address :", pGrpAddr);

  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmStarGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc  = pimsmStarGFind(pimsmCb, pGrpAddr,&pStarGNode);
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," failed pimsmStarGFind()");
    if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    }
    return L7_FAILURE;
  }
  rc = pimsmStarGImdtOlist(pimsmCb, pStarGNode, pOif);
  if (pimsmCb->family == L7_AF_INET)
  {
   BITX_RESET_ALL(&scopedOifList);
   if (mcastMapGroupScopedInterfacesGet(pGrpAddr,
                                        (L7_uchar8 *)&(scopedOifList)) == L7_SUCCESS)
   {
     if (BITX_TEST(&scopedOifList,pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex))
     {
       BITX_RESET_ALL(pOif);
     }
     else
     {
       BITX_MASK_AND(pOif, &scopedOifList, pOif);
     }
   }
  }
  BITX_RESET(pOif, pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex);
  if(osapiSemaGive(pimsmCb->pimsmStarGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    return L7_FAILURE;
  }
  return rc;
}



/******************************************************************************
* @purpose  (S,G) entry OIF GET Routine
*
* @param    handle      @b{(input)} pimsm CB Handle
* @param    pGrpAddr    @b{(input)} Group IP Addr
* @param    pSrcAddr    @b{(input)} Source IP Addr
*           pOif        @b{(output)} oif-list
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmSGEntryOIFGet(L7_VOIDPTR handle,
                                         L7_inet_addr_t *pGrpAddr,
                                         L7_inet_addr_t *pSrcAddr,
                                         interface_bitset_t *pOif)
{
  pimsmCB_t *pimsmCb;
  pimsmSGNode_t *pSGNode =L7_NULLPTR;
  interface_bitset_t scopedOifList;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Group Address :", pGrpAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_UI_INTERACTION,   PIMSM_TRACE_INFO, "Src Address :", pSrcAddr);
  /* Lock the Table */
  if(osapiSemaTake(pimsmCb->pimsmSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to take semaphore");
    return L7_FAILURE;
  }
  rc  = pimsmSGFind(pimsmCb, pGrpAddr,pSrcAddr, &pSGNode);
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_INFO," failed pimsmSGFind()");
    if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    }
    return L7_FAILURE;
  }
  rc = pimsmSGInhrtdOlist(pimsmCb, pSGNode, pOif);

  if (pimsmCb->family == L7_AF_INET)
  {
   BITX_RESET_ALL(&scopedOifList);
   if (mcastMapGroupScopedInterfacesGet(pGrpAddr,
                                        (L7_uchar8 *)&(scopedOifList)) == L7_SUCCESS)
   {
     if (BITX_TEST(&scopedOifList,pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex))
     {
       BITX_RESET_ALL(pOif);
     }
     else
     {
       BITX_MASK_AND(pOif, &scopedOifList, pOif);
     }
   }
  }
  BITX_RESET(pOif, pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex);
  if(osapiSemaGive(pimsmCb->pimsmSGTree.semId) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR, "Failed to give semaphore");
    return L7_FAILURE;
  }
  return rc;
}
/******************************************************************************
* @purpose  To get the PIMSM MRT table entry count
*
* @param    handle           @b{(input)} pimsm CB Handle
* @param    entryCount       @b{(input)} number of MRT entries
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmIpMrouteEntryCountGet(L7_VOIDPTR handle, L7_uint32 *entryCount)
{
  pimsmCB_t *pimsmCb;
  pimsmSGNode_t *pSGNode =L7_NULLPTR;
  pimsmStarGNode_t *pStarGNode = L7_NULLPTR;
  L7_RC_t rc;

  pimsmCb = (pimsmCB_t *)handle;
  if (pimsmCb == L7_NULLPTR || entryCount == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_UI_INTERACTION,  PIMSM_TRACE_ERROR,"pimsmCb is NULL");
    return L7_FAILURE;
  }
  *entryCount = 0;
  rc = pimsmStarGFirstGet(pimsmCb, &pStarGNode);
  while (rc == L7_SUCCESS)
  {
    if ((pStarGNode->flags & PIMSM_ADDED_TO_MFC) != L7_NULL)
    {
      (*entryCount)++;
    }
    rc = pimsmStarGNextGet(pimsmCb, pStarGNode, &pStarGNode);
  }
  rc = pimsmSGFirstGet(pimsmCb,&pSGNode);
  while (rc == L7_SUCCESS)
  {
    if ((pSGNode->flags & PIMSM_ADDED_TO_MFC) != L7_NULL)
    {
      (*entryCount)++;
    }
    rc = pimsmSGNextGet(pimsmCb, pSGNode, &pSGNode);
  }
  return L7_SUCCESS;
}

