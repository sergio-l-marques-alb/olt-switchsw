/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmmrt.c
*
* @purpose  MTR table implementation specifices.
*
* @component 
*
* @comments 
*
* @create 01/01/2006
*
* @author  dsatyanarayana
* @end
*
******************************************************************************/
#include "pimsmdefs.h"
#include "pimsmsgtree.h"
#include "pimsmsgrpttree.h"
#include "pimsmstargtree.h"
#include "pimsmstarstarrptree.h"
#include "pimsmmrt.h"

/******************************************************************************
* @purpose  
*
* @param    
*
* @returns  
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmMRTFind(pimsmCB_t *pimsmCb,
                     L7_inet_addr_t *pGrpAddr,
                     L7_inet_addr_t *pSrcAddr,
                     pimsmLongestMatch_t *pMRTNode)
{

  pimsmSGNode_t *pSGNode;
  pimsmSGRptNode_t *pSGRptNode;
  pimsmStarGNode_t *pStarGNode;
  pimsmStarStarRpNode_t *pStarStarRpNode;
  L7_RC_t  rc;
  L7_inet_addr_t rpAddr;

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_INFO, "Group Address :", pGrpAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_INFO, "Src Address :", pSrcAddr);

  if(pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO," failed pimsmSGFind().");
    if(pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode) 
       != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO," failed pimsmSGRptFind()");
      if(pimsmStarGFind(pimsmCb, pGrpAddr, &pStarGNode) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO," failed pimsmStarGFind()");
        rc = pimsmRpAddressGet(pimsmCb, pGrpAddr, &rpAddr); 
        if(rc != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR, 
                      "pimsmRpAddressGet failed ");
          PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_INFO, "Grp Address :", pGrpAddr);
          return L7_FAILURE;
        }
        if(pimsmStarStarRpFind(pimsmCb, &rpAddr, &pStarStarRpNode) 
           != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO," failed pimsmStarStarRpFind()");
          return L7_FAILURE;
        }
        else
        {
          /* (*,*,RP) found */
          pMRTNode->entryType = PIMSM_ENTRY_TYPE_STAR_STAR_RP;
          pMRTNode->mrtNode = (void*)pStarStarRpNode;
          PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO," (*,*,RP) found");
          return L7_SUCCESS;
        }
      }
      else
      {
        /* (*,G found */
        pMRTNode->entryType = PIMSM_ENTRY_TYPE_STAR_G;
        pMRTNode->mrtNode = (void*)pStarGNode;
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO," (*,G) found");
        return L7_SUCCESS;
      }
    }
    else
    {
      /* (S,G,rpt) found */
      pMRTNode->entryType = PIMSM_ENTRY_TYPE_S_G_RPT;
      pMRTNode->mrtNode = (void*)pSGRptNode;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO," (S,G,rpt) found");
      return L7_SUCCESS;
    } 
  }
  else
  {
    /* (S,G) found */
    pMRTNode->entryType = PIMSM_ENTRY_TYPE_S_G;
    pMRTNode->mrtNode = (void*)pSGNode;
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO," (S,G) found");
    return L7_SUCCESS;
  } 
}

void pimsmMRTNodeCleanup(pimsmCB_t * pimsmCb)
{
  pimsmSGNodeCleanup(pimsmCb);
  pimsmSGRptNodeCleanup(pimsmCb);
  pimsmStarGNodeCleanup(pimsmCb);
  pimsmStarStarRpNodeCleanup(pimsmCb);
}
