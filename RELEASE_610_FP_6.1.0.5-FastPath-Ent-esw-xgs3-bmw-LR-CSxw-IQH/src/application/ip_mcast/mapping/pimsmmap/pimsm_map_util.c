/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  pimsm_map_util.c
*
* @purpose   PIM-SM Utility Functions
*
* @component PIM-SM Mapping Layer
*
* @comments  none
*
* @create    03/11/2002
*
* @author    gkiran/dsatyanarayana
*
* @end
*
**********************************************************************/

#include "l7_pimsminclude.h"
#include "mcast_wrap.h"
#include "l7_pimsm_api.h"
#include "l7_mcast_api.h"
#include "pimsm_map.h"
#include "pimsm_vend_exten.h"
#include "pimsm_map_util.h"

extern pimsmCfgDefs_t pimsmConstsDefs[PIMSM_MAP_CB_MAX];
extern pimsmGblVariables_t pimsmGblVariables_g;

/*---------------------------------------------------------------------
 *                  SUPPORT FUNCTIONS  -  GLOBAL CONFIG
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Enable the PIM-SM Routing Function
*
* @param    pimsmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    pimsmDoInit     @b{(input)} Flag to check whether PIM-SM
*                                       Memory can be Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Admin mode is enabled and the Mapping layer Init function
*           is called so that vendor stack can be initialized and
*           protocol made operational if all configurability conditions
*           are satisfied
*
* @end
*********************************************************************/
L7_RC_t pimsmMapPimsmAdminModeEnable(pimsmMapCB_t *pimsmMapCbPtr,
                                     L7_BOOL pimsmDoInit)
{
  L7_uint32 intIfNum,   index, pimsmIfMode, rtrIfNum;
  pimsmCfgStaticRP_t    *pStaticRp = L7_NULLPTR;
  L7_inet_addr_t        crpGroupMask;
  pimsmCfgSsmRange_t  *ssmRange = L7_NULLPTR;

  L7_uchar8 grpAddr[IPV6_DISP_ADDR_LEN];
  L7_uchar8 rpAddr[IPV6_DISP_ADDR_LEN];

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  inetAddressZeroSet(pimsmMapCbPtr->familyType, &crpGroupMask);

  if (pimsmMapExtenPimsmAdminModeSet(pimsmMapCbPtr, 
                                     L7_ENABLE, pimsmDoInit) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                    "Failed to enable PIMSM Globally");
    return L7_FAILURE;
  }

  /* apply global parms */
  if (pimsmMapDataThresholdRateApply(pimsmMapCbPtr, 
      pimsmMapCbPtr->pPimsmMapCfgData->rtr.dataThresholdRate) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                    "Failed to apply data threshold rate %d",
                    pimsmMapCbPtr->pPimsmMapCfgData->rtr.dataThresholdRate);
  }

  if (pimsmMapRegisterThresholdRateApply(pimsmMapCbPtr,
      pimsmMapCbPtr->pPimsmMapCfgData->rtr.regThresholdRate) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                    "Failed to apply register threshold rate %d",
                    pimsmMapCbPtr->pPimsmMapCfgData->rtr.regThresholdRate);
  }

  for (index = 0; index < L7_MAX_SSM_RANGE; index++)
  {
   ssmRange = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[index];

   if(inetIsAddressZero(&ssmRange->groupAddr)!= L7_TRUE)
   {
          pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] = L7_FALSE;
        
   }
   else
   {
           pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] = L7_TRUE;
   }

  }
  for (rtrIfNum = 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
  {
    if (mcastIpMapRtrIntfToIntIfNum(pimsmMapCbPtr->familyType, 
                                    rtrIfNum, &intIfNum) != L7_SUCCESS)
    {
      continue;
    }

    if (pimsmMapInterfaceModeGet(pimsmMapCbPtr->familyType, intIfNum, 
                      &pimsmIfMode )== L7_SUCCESS && (pimsmIfMode == L7_ENABLE))
    {
      if (pimsmMapInterfaceModeApply(pimsmMapCbPtr,
                                     intIfNum, pimsmIfMode) != L7_SUCCESS)
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Failed to enable PIM-SM  on \
                        intIfNum(%d)",intIfNum);
      }
    }
  }


  for (index = 0; index < L7_MAX_STATIC_RP_NUM; index++)
  {
    pStaticRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[index];
    if (inetIsAddressZero(&pStaticRp->rpIpAddr) != L7_TRUE)
    {
      if (pimsmMapStaticRPApply(pimsmMapCbPtr, 
                                &pStaticRp->rpIpAddr, 
                                &pStaticRp->rpGrpAddr,
                                &pStaticRp->rpGrpMask, 
                                pStaticRp->overRide, index) != L7_SUCCESS)
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Failed to apply static RP information for RP %s,"
                        "Grp %s",inetAddrPrint(&pStaticRp->rpIpAddr,rpAddr),
                        inetAddrPrint(&pStaticRp->rpGrpAddr,grpAddr));
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Disable the PIM-SM Routing Function
*
* @param    pimsmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    pimsmDoInit     @b{(input)} Flag to check whether PIM-SM
*                                       Memory can be De-Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapPimsmAdminModeDisable(pimsmMapCB_t *pimsmMapCbPtr,
                                      L7_BOOL pimsmDoInit)
{
  L7_uint32            intIfNum = L7_NULL, index = L7_NULL;
  pimsmCfgStaticRP_t   *pStaticRp = L7_NULLPTR;
  pimsmCfgCandRPdata_t *pCandRp   = L7_NULLPTR;
  pimsmCfgCandBSRdata_t *pCandBsr = L7_NULLPTR;
  pimsmOperCandRPdata_t *pOperCandRp = L7_NULLPTR;
  pimsmOperCandBSRdata_t *pOperCandBsr = L7_NULLPTR;
  L7_inet_addr_t       crpGroupMask;
  L7_uchar8 grpAddr[IPV6_DISP_ADDR_LEN];
  L7_uchar8 rpAddr[IPV6_DISP_ADDR_LEN];

  
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  inetAddressZeroSet(pimsmMapCbPtr->familyType, &crpGroupMask);

  if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational == L7_TRUE)
  {

    for (index = 0; index < L7_MAX_STATIC_RP_NUM; index++)
    {
      pStaticRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[index];
      if (pimsmMapCbPtr->pPimsmInfo->pimsmStaticRPOper[index] == L7_TRUE)
      {
        if(pimsmMapStaticRPRemoveApply(pimsmMapCbPtr, 
                                        &pStaticRp->rpIpAddr,
                                        &pStaticRp->rpGrpAddr, 
                                        &pStaticRp->rpGrpMask,
                                        index) != L7_SUCCESS)
        {
          PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Failed to remove static RP information for RP %s,\
                      Grp %s ",inetAddrPrint(&pStaticRp->rpIpAddr,rpAddr),
                      inetAddrPrint(&pStaticRp->rpGrpAddr,grpAddr));
        }
      }
    }

    for (index = 0; index < L7_MAX_CAND_RP_NUM; index++)
    {
      pCandRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmCandidateRp[index];
      pOperCandRp = &pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index];
      if (pOperCandRp->operMode == L7_TRUE)
      {
        inetMaskLenToMask(pimsmMapCbPtr->familyType, pCandRp->grpPrefixLength, 
                        &crpGroupMask);
        if (pimsmMapCandRPApply(pimsmMapCbPtr, 
                                    pCandRp->intIfNum,
                                     &pOperCandRp->candRPAddress, 
                                     &pCandRp->grpAddress, &crpGroupMask, 
                                     L7_DISABLE, index) != L7_SUCCESS)
        {
          PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Failed to remove Candidate RP intIfNum (%d),\
                      Grp %s ",intIfNum,
                      inetAddrPrint(&pCandRp->grpAddress,grpAddr));
        }
      }
    }
    pCandBsr = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.candidateBsr; 
    pOperCandBsr = &pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper;
    if (pOperCandBsr->operMode == L7_TRUE)
    {
    if (pimsmMapCandBSRApply(pimsmMapCbPtr, 
                                    &pOperCandBsr->candBSRAddress, 
                                    pCandBsr->intIfNum,
                                    pCandBsr->candBSRPriority, 
                                    L7_DISABLE, 
                                    pCandBsr->candBSRHashMaskLength, 
                                    pCandBsr->candBSRScope) != L7_SUCCESS)
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                         "Failed to apply BSR (Enable/Disable) \
                          for intIfNum (%d) ",
                          intIfNum);
      }
    }
#if 0   
      for (index = 0; index < L7_MAX_SSM_RANGE; index++)
     {
         pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] = L7_TRUE;
     }       
#endif      
    /* disable all interfaces that are currently operational */
    for (intIfNum = 0; intIfNum < L7_MAX_INTERFACE_COUNT; intIfNum++)
    {
      if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational == L7_TRUE)
      {
        if (pimsmMapInterfaceModeApply(pimsmMapCbPtr, intIfNum, 
                                       L7_DISABLE) != L7_SUCCESS)
        {
          PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                            "Failed to disable PIM-SM  on \
                             intIfNum(%d)",intIfNum);
        }
      }
    }
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "PIMSM protocol is not operational");
  }

  /*The following function has to be called even if the protocol is not 
    operationally enabled for the reason that the heap memory would have 
    been allocated even if the protcol is not operatiopnally enabled,and 
    so that memory has to be freed up.
   */
  
    if (pimsmMapExtenPimsmAdminModeSet(pimsmMapCbPtr, 
                                       L7_DISABLE, pimsmDoInit) != L7_SUCCESS)
    {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                          "Failed to disable PIMSM Globally");
        return L7_FAILURE;
    }
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply the static RP information
*
* @param    pimsmMapCbPtr @b{(input)}       Mapping Control Block.
* @param    index         @b{(input)}       Index into the config structure 
*                                           for this static RP
* @param    rpIpAddr      @b{(input)}       Ip address of the RP
* @param    rpGrpAddr     @b{(input)}       Group address supported by the RP
* @param    rpGrpMask     @b{(input)}       Group mask for the group address
* @param    conflict      @b{(input)}       conflict
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPApply(pimsmMapCB_t *pimsmMapCbPtr,
                        L7_inet_addr_t *rpIpAddr, L7_inet_addr_t *rpGrpAddr, 
                        L7_inet_addr_t *rpGrpMask, L7_BOOL conflict, L7_uint32 index)
{
  L7_uchar8 grpAddr[IPV6_DISP_ADDR_LEN];
  L7_uchar8 rpAddr[IPV6_DISP_ADDR_LEN];

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  /* exten function called only if PIM-SM is operational */
  if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational == L7_TRUE)
  {
    /* 
     * Lots of checking todo. 
     * Check if an intf has same ip address but is not enabled for pimsm.
     * etc. 
     * Check if this rp ip address is same as one of the rtr intf
     * ip address. If yes check if crppref is set                 
     */
    if (pimsmMapExtenStaticRPSet(pimsmMapCbPtr, rpIpAddr, 
                                  rpGrpAddr, rpGrpMask, conflict) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Failed to apply static RP information for RP %s,\
                        Grp %s ",inetAddrPrint(rpIpAddr,rpAddr),
                        inetAddrPrint(rpGrpAddr,grpAddr));
       return L7_FAILURE;
    }
    pimsmMapCbPtr->pPimsmInfo->pimsmStaticRPOper[index]= L7_TRUE;
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, 
        "\n PIM-SM not operational: config paramter stored but not applied."); 
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "PIMSM protocol is not operational.\
                       Config paramter stored but not applied");
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Remove the static RP information
*
* @param    pimsmMapCbPtr @b{(input)}       Mapping Control Block.
* @param    index         @b{(input)}       Index into the config structure 
*                                           for this static RP
* @param    rpIpAddr      @b{(input)}       Ip address of the RP
* @param    rpGrpAddr     @b{(input)}       Group address supported by the RP
* @param    rpGrpMask     @b{(input)}       Group mask for the group address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPRemoveApply(pimsmMapCB_t *pimsmMapCbPtr, 
        L7_inet_addr_t *rpIpAddr, 
        L7_inet_addr_t *rpGrpAddr, L7_inet_addr_t *rpGrpMask, L7_uint32 index)
{
  L7_uchar8 grpAddr[IPV6_DISP_ADDR_LEN];
  L7_uchar8 rpAddr[IPV6_DISP_ADDR_LEN];
  
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  /* Lots of checking todo. */
  /* Check if an intf has same ip address but is not enabled for pimsm. */
  /* etc. */
  if (pimsmMapExtenStaticRPRemove(pimsmMapCbPtr, rpIpAddr, 
                                   rpGrpAddr, rpGrpMask) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Failed to remove static RP information for RP %s,\
                      Grp %s ",inetAddrPrint(rpIpAddr,rpAddr),
                      inetAddrPrint(rpGrpAddr,grpAddr));
    return L7_FAILURE;
  }
  pimsmMapCbPtr->pPimsmInfo->pimsmStaticRPOper[index]= L7_FALSE;
  return L7_SUCCESS;
}


/*---------------------------------------------------------------------
 *                 SUPPORT FUNCTIONS  -  INTERFACE CONFIG
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Configure PIM-SM router interfaces
*
* @param    pimsmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    intIfNum        @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapInterfaceConfigure(pimsmMapCB_t *pimsmMapCbPtr,
                                    L7_uint32 intIfNum)
{
  pimsmCfgCkt_t     *pCfg = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr, intIfNum, &pCfg) == L7_TRUE)
  {
      if  ((pimsmMapInterfaceHelloIntervalApply(pimsmMapCbPtr,
            intIfNum, pCfg->intfHelloInterval) == L7_FAILURE) || 
           (pimsmMapInterfaceJoinPruneIntervalApply(pimsmMapCbPtr,
            intIfNum, pCfg->intfJoinPruneInterval) == L7_FAILURE)||
            (pimsmMapInterfaceDRPriorityApply(pimsmMapCbPtr,
            intIfNum, pCfg->drPriority) == L7_FAILURE))
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                          "Failed to configure PIMSM on intIfNum(%d)",
                          intIfNum);
        return L7_FAILURE;
      }
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS,
      "Interface Configuration done\n");
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Apply the RP candidate (ENABLE/DISABLE)
*
* @param    pimsmMapCbPtr  @b{(input)}  Mapping Control Block.
* @param    rpAddr         @b{(input)}  candidate RP address.
* @param    rpGrpAddr      @b{(input)}  Group address supported by the RP
* @param    rpGrpMask      @b{(input)}  Group mask for the group address
* @param    mode           @b{(input)}  mode (ENABLE/DISABLE).
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandRPApply(pimsmMapCB_t *pimsmMapCbPtr, 
            L7_uint32 intIfNum,L7_inet_addr_t *rpAddr, L7_inet_addr_t *rpGrpAddr, 
             L7_inet_addr_t *rpGrpMask, L7_uint32 mode, L7_uint32 index)
{
  L7_uchar8 grpAddr[IPV6_DISP_ADDR_LEN];
  L7_uchar8 rpAddrBuf[IPV6_DISP_ADDR_LEN];
  
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if(pimsmMapCbPtr->pimsmIntfInfo[intIfNum].pimsmOperational == L7_FALSE)
  {  
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "PIMSM not operational on interface %d",intIfNum);
     memset(&pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index],0,
        sizeof(pimsmOperCandRPdata_t));
     pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].operMode = L7_FALSE;
     inetAddressZeroSet(pimsmMapCbPtr->familyType, 
      &pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].candRPAddress);

     return L7_SUCCESS;
  }
  /* exten function called only if PIM-SM is operational */
  if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational == L7_TRUE)
  {
    if (pimsmMapExtenInterfaceCandRPSet(pimsmMapCbPtr, intIfNum, 
                   rpAddr, rpGrpAddr, rpGrpMask, mode) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Failed to apply Candidate RP information for RP %s,\
                      Grp %s ",inetAddrPrint(rpAddr,rpAddrBuf),
                      inetAddrPrint(rpGrpAddr,grpAddr));
     inetAddressZeroSet(pimsmMapCbPtr->familyType, 
      &pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].candRPAddress);
      pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].operMode = L7_FALSE;
      return L7_FAILURE;
    }
    if (mode == L7_ENABLE)
    {
     inetCopy(&pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].candRPAddress,
         rpAddr);
     pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].operMode = L7_TRUE;
    } 
    else if (mode == L7_DISABLE)
    {
     inetAddressZeroSet(pimsmMapCbPtr->familyType, 
      &pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].candRPAddress);
      pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].operMode = L7_FALSE;
    }
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n PIM-SM not operational:"
    "config paramter stored but not applied\n");
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "PIMSM protocol is not operational.\
                       Config paramter stored but not applied");
     memset(&pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index],0,
        sizeof(pimsmOperCandRPdata_t));
     pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].operMode = L7_FALSE;
     inetAddressZeroSet(pimsmMapCbPtr->familyType, 
      &pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].candRPAddress);
  }
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply the BSR candidate (ENABLE/DISABLE)
*
* @param    pimsmMapCbPtr @b{(input)}  Mapping Control Block.
* @param    BsrAddr       @b{(input)}  BSR Address
* @param    priority      @b{(input)}  Priority
* @param    mode          @b{(input)}  mode (ENABLE/DISABLE).
* @param    length        @b{(input)}  maskLen
* @param    scope         @b{(input)}  scope Value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRApply(pimsmMapCB_t *pimsmMapCbPtr, 
                           L7_inet_addr_t *bsrAddr, L7_uint32 intIfNum, L7_uint32 priority, 
                           L7_uint32 mode, L7_uint32 length, L7_uint32 scope)
{
  L7_uchar8 bsrAddrBuf[IPV6_DISP_ADDR_LEN];
  
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if(pimsmMapCbPtr->pimsmIntfInfo[intIfNum].pimsmOperational == L7_FALSE)
  {  
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "PIMSM not operational on interface %d",intIfNum);

     inetAddressZeroSet(pimsmMapCbPtr->familyType, 
        &pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.candBSRAddress);   
     pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.operMode = L7_FALSE;
     return L7_SUCCESS;
  }
  

  if (pimsmMapExtenInterfaceCandBSRSet(pimsmMapCbPtr, intIfNum, 
            bsrAddr, priority, mode, length, scope) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Failed to apply BSR (Enable/Disable) \
                       for BSR Address %s ",
                       inetAddrPrint(bsrAddr,bsrAddrBuf));
     inetAddressZeroSet(pimsmMapCbPtr->familyType, 
        &pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.candBSRAddress);   
     pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.operMode = L7_FALSE;
     return L7_FAILURE;
  }
  if (mode == L7_ENABLE)
  {
        /* storing in Control Block */
     inetCopy(&pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.candBSRAddress,
        bsrAddr);
     pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.operMode = L7_TRUE;
  } 
  else if (mode == L7_DISABLE)
  {
   inetAddressZeroSet(pimsmMapCbPtr->familyType, 
      &pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.candBSRAddress);   
   pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.operMode = L7_FALSE;
  }  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine if the interface is valid for PIM-SM configuration
*           and optionally output a pointer to the configuration structure
*           entry for this interface
*
* @param    pimsmMapCbPtr @b{(input)} Mapping Control Block.
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    pCfg          @b{(input)} Output pointer to PIM-SM interface config
*                                     structure  or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the PIM-SM component has been
*           started (regardless of whether the PIM-SM protocol is enabled
*           or not).
*
* @comments The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL pimsmMapInterfaceIsConfigurable(pimsmMapCB_t *pimsmMapCbPtr,
                     L7_uint32 intIfNum, pimsmCfgCkt_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  index =  pimsmMapCbPtr->pPimsmMapCfgMapTbl[intIfNum];

  if (index == 0)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Invalid Index %d",index);
    return L7_FALSE;
  }

  /* verify that the configId in the config data table entry               
   * matches the configId that NIM maps to the intIfNum we are considering 
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(pimsmMapCbPtr->
            pPimsmMapCfgData->intf[index].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between    
       * pPimsmMapCfgData and pPimsmMapCfgMapTbl or there is synchronization  
       * issue between NIM and components w.r.t. interface creation/deletion
       */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Error accessing PIMSM config data for intIfNum (%d)",
                        intIfNum);
      return L7_FALSE;
    }
  }

  if (pCfg != L7_NULLPTR)
  {
    *pCfg = &(pimsmMapCbPtr->pPimsmMapCfgData->intf[index]);
  }

  return L7_TRUE;
}
/*********************************************************************
* @purpose  Set the PIMSM admin mode for the specified interface
*
* @param    pimsmMapCbPtr @b{(input)} Mapping Control Block.
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    mode          @b{(input)} Administrative mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes:
*           1. PIMSM is enabled
*           2. Interface has been created and configured for PIMSM at
*              the PIMSM vendor layer
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapInterfaceParamsApply(pimsmMapCB_t *pimsmMapCbPtr,
                           L7_uint32 intIfNum, L7_uint32 mode)
{
  pimsmCfgCandBSRdata_t *candBsr = L7_NULLPTR;
  pimsmCfgCandRPdata_t  *pCandRp   = L7_NULLPTR;
  L7_RC_t rc;
  L7_inet_addr_t tmpAddr,crpGroupMask;
  L7_uint32   index;

  candBsr = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.candidateBsr;

  if (intIfNum == candBsr->intIfNum)
    {
      rc =mcastIpMapRtrIntfGlobalIpAddressGet(pimsmMapCbPtr->familyType,
      intIfNum, &tmpAddr);
      if(rc == L7_SUCCESS)
      {
        if (pimsmMapCandBSRApply(pimsmMapCbPtr,
         &tmpAddr, intIfNum, candBsr->candBSRPriority, mode,
          candBsr->candBSRHashMaskLength,candBsr->candBSRScope) != L7_SUCCESS)
        {
          PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                         "Failed to apply BSR (Enable/Disable) \
                          for BSR intfNum(%d) ",
                           intIfNum);
        }
      }   
      else
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                    "mcastIpMapRtrIntfGlobalIpAddressGet() failed! intIfNum = %d",intIfNum);
       /* return L7_FAILURE;*/
      }
    }
    
    for (index=0;index<L7_MAX_CAND_RP_NUM;index++)
    {
       pCandRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmCandidateRp[index];
      if( intIfNum == pCandRp->intIfNum)
      {
        rc = mcastIpMapRtrIntfGlobalIpAddressGet(pimsmMapCbPtr->familyType,
       intIfNum, &tmpAddr);
       if(rc !=L7_SUCCESS)
       {
         PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                     "mcastIpMapRtrIntfGlobalIpAddressGet() failed! intIfNum = %d",intIfNum);
         /*return L7_FAILURE; */
         continue;
       }
       inetMaskLenToMask(pimsmMapCbPtr->familyType, pCandRp->grpPrefixLength,
                       &crpGroupMask);
  
       if (pimsmMapCandRPApply(pimsmMapCbPtr, intIfNum,
                                       &tmpAddr,
                                       &pCandRp->grpAddress, &crpGroupMask,
                                       mode, index) != L7_SUCCESS)
       {
         PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                     "Failed to apply Candidate RP(Enable/Disable) \
                      for CandRP intfNum(%d) ",intIfNum);
       }
     }
    }

    return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the PIMSM admin mode for the specified interface
*
* @param    pimsmMapCbPtr @b{(input)} Mapping Control Block.
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    mode          @b{(input)} Administrative mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes:
*           1. PIMSM is enabled
*           2. Interface has been created and configured for PIMSM at
*              the PIMSM vendor layer
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceModeApply(pimsmMapCB_t *pimsmMapCbPtr,
                           L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_inet_addr_t  ipAddr;
  L7_RC_t rc;
  L7_uint32 mcastMode;
  L7_inet_addr_t allRoutersAddr;     
  L7_uchar8 addr[IPV6_DISP_ADDR_LEN];

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (mode == L7_DISABLE)
  {
    inetAllPimRouterAddressInit (pimsmMapCbPtr->familyType, &(allRoutersAddr));
    if(mcastLocalMulticastAddrUpdate(pimsmMapCbPtr->familyType,intIfNum,
                        &allRoutersAddr,L7_FALSE) != L7_SUCCESS)
    {
      PIMSM_MAP_LOG_MSG (pimsmMapCbPtr->familyType,"PIM All Routers Address - %s "
                         "Delete from the DTL Mcast List Failed for intIfNum - %d",
                         inetAddrPrint(&allRoutersAddr,addr), intIfNum);
    }
      

    if (mcastRtrIntfMcastFwdModeSet(pimsmMapCbPtr->familyType, intIfNum,
                                    L7_DISABLE) != L7_SUCCESS)
    {
      PIMSM_MAP_LOG_MSG (pimsmMapCbPtr->familyType, "Mcast Forwarding Mode "
                         "Disable Failed for intIfNum - %d", intIfNum);
    }

    pimsmMapInterfaceParamsApply(pimsmMapCbPtr, intIfNum, mode);
    rc = pimsmMapExtenInterfaceModeSet(pimsmMapCbPtr, intIfNum, mode);
    return  rc;
  }
  else if (mode == L7_ENABLE)
  {
    mcastMapMcastAdminModeGet(&mcastMode);
    
    if(mcastMode == L7_DISABLE)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Global Multicast Mode is Disabled");
      return L7_FAILURE;
    }

    /* If no IP address has been assigned yet, just return a success.  
     * The enable will be completed when we get notified of an IP address add.
     */
    inetAddressReset(&ipAddr);
    if ((mcastIpMapRtrIntfIpAddressGet (pimsmMapCbPtr->familyType, intIfNum, 
                                        &ipAddr) != L7_SUCCESS) ||
        (inetIsAddressZero (&ipAddr) == L7_TRUE))
    {
      return L7_SUCCESS;
    }

    inetAllPimRouterAddressInit (pimsmMapCbPtr->familyType, &(allRoutersAddr));
    if(mcastLocalMulticastAddrUpdate(pimsmMapCbPtr->familyType,intIfNum,
                        &allRoutersAddr,L7_TRUE) != L7_SUCCESS)
    {
      PIMSM_MAP_LOG_MSG (pimsmMapCbPtr->familyType,"PIM All Routers Address - %s "
                         "Add to the DTL Mcast List Failed for intIfNum - %d",
                         inetAddrPrint(&allRoutersAddr,addr), intIfNum);
    }

    if (mcastRtrIntfMcastFwdModeSet(pimsmMapCbPtr->familyType, intIfNum,
                                    L7_ENABLE) != L7_SUCCESS)
    {
      PIMSM_MAP_LOG_MSG (pimsmMapCbPtr->familyType, "Mcast Forwarding Mode "
                         "Enable Failed for intIfNum - %d", intIfNum);
      return L7_FAILURE;
    }

    if (pimsmMapExtenInterfaceModeSet(pimsmMapCbPtr, intIfNum, mode)
                                   != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Failed to Enable PIM-SM on Interface - %d", intIfNum);
      return L7_FAILURE;
    }

    /* apply config data to the operational interface */
    if (pimsmMapInterfaceConfigure(pimsmMapCbPtr, intIfNum) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Failed to configure PIMSM params on intfIfNum(%d)",
                      intIfNum);
    }
    pimsmMapInterfaceParamsApply(pimsmMapCbPtr, intIfNum, mode);
   }

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                    "Invalid mode (%d)",mode);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the DR Priority for the specified interface
*
* @param    pimsmMapCbPtr     @b{(input)} Mapping Control Block.
* @param    intIfNum          @b{(input)} Internal Interface Number
* @param    priority          @b{(input)} Designated Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceDRPriorityApply(pimsmMapCB_t *pimsmMapCbPtr,
                             L7_uint32 intIfNum, L7_uint32 priority)
{

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  /* exten function called only if PIM-SM is operational on this interface */
  if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational == L7_TRUE)
  {
     if (pimsmMapExtenInterfaceDRPrioritySet(pimsmMapCbPtr, intIfNum, priority)
                                                                != L7_SUCCESS)
     {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                         "Failed to set DR priority(%d) on intIfNum(%d)",
                         priority,intIfNum);
       return L7_FAILURE;
     }
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n:PIM-SM not:"
    "operational config paramter stored but not applied\n");
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "PIMSM not operational on intIfNum(%d).\
                       Config paramter stored but not applied",intIfNum); 
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the hello interval for an interface
*
* @param    pimsmMapCbPtr        Mapping Control Block.
* @param    intIfNum             Interface num.
* @param    helloIntvl           Hello Interval in secs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t pimsmMapInterfaceHelloIntervalApply(pimsmMapCB_t *pimsmMapCbPtr, 
                                            L7_uint32 intIfNum, L7_uint32 helloIntvl)
{
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered\n", __FUNCTION__, __LINE__);
  /* exten function called only if PIM-SM is operational on this interface */
  if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational == L7_TRUE)
  {
     if (pimsmMapExtenInterfaceHelloIntervalSet(pimsmMapCbPtr,intIfNum, helloIntvl)
                                                                != L7_SUCCESS)
     {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                         "Failed to set Hello Interval(%d) on intIfNum(%d)",
                         helloIntvl,intIfNum);
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n %s, %d"
       "pimsmMapExtenInterfaceHelloIntervalSet Failed.\n", 
       __FUNCTION__, __LINE__);
       return L7_FAILURE;
     }
  }
  else 
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n %s, %d :PIM-SM not:"
    "operational config paramter stored but not applied\n", 
    __FUNCTION__, __LINE__);
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "PIMSM not operational on intIfNum(%d).\
                       Config paramter stored but not applied",intIfNum); 
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the join prune interval for an interface
*
* @param    pimsmMapCbPtr        Mapping Control Block.
* @param    intIfNum             Interface num.
* @param    joinPruneIntvl       Join-prune Interval in secs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t pimsmMapInterfaceJoinPruneIntervalApply(pimsmMapCB_t *pimsmMapCbPtr, 
                                            L7_uint32 intIfNum, L7_uint32 joinPruneIntvl)
{
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered\n", __FUNCTION__, __LINE__);
  
  /* exten function called only if PIM-SM is operational on this interface */
  if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational == L7_TRUE)
  {
     if (pimsmMapExtenInterfaceJoinPruneIntervalSet(pimsmMapCbPtr,intIfNum, joinPruneIntvl)
                                                                != L7_SUCCESS)
     {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                         "Failed to set Join Prune Interval(%d) \
                          on intIfNum(%d)",
                         joinPruneIntvl,intIfNum);
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n %s, %d"
       "pimsmMapExtenInterfaceJoinPruneIntervalSet Failed.\n", 
       __FUNCTION__, __LINE__);
       return L7_FAILURE;
     }
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n %s, %d :PIM-SM not:"
    "operational config paramter stored but not applied\n", 
    __FUNCTION__, __LINE__);
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "PIMSM not operational on intIfNum(%d).\
                       Config paramter stored but not applied",intIfNum); 
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the data threshold rate for switching to SPT
*
* @param    pimsmMapCbPtr     @b{(input)}   Mapping Control Block.
* @param    dataThresholdRate @b{(input)}   Data Threshold Rate in kbits/sec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done before this function is invoked
*
* @end
*********************************************************************/
L7_RC_t pimsmMapDataThresholdRateApply(pimsmMapCB_t *pimsmMapCbPtr, 
                                               L7_uint32 dataThresholdRate)
{
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  /* exten function called only if PIM-SM is operational */
  if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational == L7_TRUE)
  {
    if (pimsmMapExtenDataThresholdRateSet(pimsmMapCbPtr, 
                                      dataThresholdRate) != L7_SUCCESS)
    {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                          "Failed to set Data Threshold(%d)",
                          dataThresholdRate);
        return L7_FAILURE;
    }
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n :PIM-SM not:"
    "operational config paramter stored but not applied\n"); 
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "PIMSM not operational.\
                       Config paramter stored but not applied"); 
  }   
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the data threshold rate for switching to SPT
*
* @param    pimsmMapCbPtr    @b{(input)}    Mapping Control Block.
* @param    regThresholdRate @b{(input)}    Data Threshold Rate in kbits/sec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done before this function is invoked
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRegisterThresholdRateApply(pimsmMapCB_t *pimsmMapCbPtr, 
                                               L7_uint32 regThresholdRate)
{
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* exten function called only if PIM-SM is operational */
  if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational == L7_TRUE)
  {
   if (pimsmMapExtenRegisterThresholdRateSet(pimsmMapCbPtr, 
                                     regThresholdRate) != L7_SUCCESS)
   {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                          "Failed to set Register Threshold(%d)",
                          regThresholdRate);
       return L7_FAILURE;
   }
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n PIM-SM not:"
    "operational config paramter stored but not applied\n"); 
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "PIMSM not operational.\
                       Config paramter stored but not applied"); 
  }   
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update the PIMSM-MAP Counters
*
* @param    None
*
* @returns  L7_SUCCESS
*           L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t
pimsmMapCountersUpdate (PIMSM_MAP_COUNTERS_TYPE_t counterType,
                        PIMSM_MAP_COUNTERS_ACTION_t counterAction)
{
  /* Lock the Table */
  if (osapiSemaTake (pimsmGblVariables_g.eventCountersSemId, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "Failed to take PIMSM-MAP Counters Semaphore.\n");
    return L7_FAILURE;
  }

  switch (counterAction)
  {
    case PIMSM_MAP_COUNTERS_INCREMENT:
      (pimsmGblVariables_g.eventCounters[counterType])++;
      break;

    case PIMSM_MAP_COUNTERS_DECREMENT:
      if ((pimsmGblVariables_g.eventCounters[counterType]) <= 0)
       {
         PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "PIMSM: Invalid operation on counterType %d ",counterType);
         return L7_FAILURE;
       }
      (pimsmGblVariables_g.eventCounters[counterType])--;
      break;

    case PIMSM_MAP_COUNTERS_RESET:
      (pimsmGblVariables_g.eventCounters[counterType]) = 0;
      break;

    default:
      break;
  }

  /* Return successful */
  osapiSemaGive (pimsmGblVariables_g.eventCountersSemId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the PIMSM-MAP Counters Value
*
* @param    None
*
* @returns  counterValue 
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32
pimsmMapCountersValueGet (PIMSM_MAP_COUNTERS_TYPE_t counterType)
{
  L7_uint32 counterValue = 0;

  /* Lock the Table */
  if (osapiSemaTake (pimsmGblVariables_g.eventCountersSemId, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "Failed to take PIMSM-MAP Counters Semaphore.\n");
    return counterValue;
  }

  counterValue = (pimsmGblVariables_g.eventCounters[counterType]);

  /* Return successful */
  osapiSemaGive(pimsmGblVariables_g.eventCountersSemId);
  return counterValue;
}

