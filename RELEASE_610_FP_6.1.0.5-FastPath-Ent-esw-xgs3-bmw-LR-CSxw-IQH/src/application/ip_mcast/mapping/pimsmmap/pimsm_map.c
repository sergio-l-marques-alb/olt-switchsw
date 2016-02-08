/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  pimsm_map.c
*
* @purpose   PIM-SM Mapping system infrastructure
*
* @component PIMSM Mapping Layer
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
#include "l7_cnfgr_api.h"
#include "l7_pimsminclude.h"
#include "l7_mcast_api.h"
#include "l7_pimsm_api.h"
#include "nvstoreapi.h"
#include "support_api.h"
#include "pimsm_v6_wrappers.h"
#include "pimsmuictrl.h"
#include "pimsm_cnfgr.h"
#include "pimsm_map.h"

pimsmGblVariables_t pimsmGblVariables_g;
pimsmMapCB_t        *pimsmMapCB;


pimsmDebugCfg_t pimsmDebugCfg;


/*********************************************************************
* @purpose  Save PIM-SM user config file to NVStore
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmSave(void)
{
  L7_uint32     cbIndex = L7_NULL;
  pimsmMapCB_t  *pimsmMapCbPtr = L7_NULLPTR;
  L7_uint32     protoMax = L7_NULL;
 
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  protoMax =  pimsmMapProtocolsMaxGet();
  for (cbIndex = PIMSM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {  
    pimsmMapCbPtr = &pimsmMapCB[cbIndex];     
    if (pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged == L7_TRUE)
    {
      pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_FALSE;
      pimsmMapCbPtr->pPimsmMapCfgData->checksum =
      nvStoreCrc32((L7_char8 *)pimsmMapCbPtr->pPimsmMapCfgData,
                   sizeof(L7_pimsmMapCfg_t) - 
                   sizeof(pimsmMapCbPtr->pPimsmMapCfgData->checksum));

      /* call NVStore save routine */
      if (sysapiCfgFileWrite(L7_FLEX_PIMSM_MAP_COMPONENT_ID, 
                             pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.filename,
                             (L7_char8 *)pimsmMapCbPtr->pPimsmMapCfgData,
                             sizeof(L7_pimsmMapCfg_t)) != L7_SUCCESS)
      {
           PIMSM_MAP_LOG_MSG(pimsmMapCbPtr->familyType,
                             "PIMSM Config-Save Failed; File - %s",
                             pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.filename);
        return L7_FAILURE;
      }
    }
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Build default pimsm config data
*
* @param    pimsmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    ver             @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
static void pimsmCommonBuildDefaultConfigData(pimsmMapCB_t *pimsmMapCbPtr,
                                                            L7_uint32 ver)
{
  L7_uint32             index = L7_NULL;
  pimsmCfgStaticRP_t    *staticRp = L7_NULLPTR;
  pimsmCfgSsmRange_t    *ssmRange = L7_NULLPTR;
  pimsmCfgCandRPdata_t  *candidateRp = L7_NULLPTR;
  pimsmCfgCandBSRdata_t *candidateBsr = L7_NULLPTR;
  pimsmCfgData_t        *cfgData = L7_NULLPTR;
  

  /*--------------------------*/
  /* build config file header */
  /*--------------------------*/

  memset(&pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr, 0, sizeof(L7_fileHdr_t));
  pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.version       = ver;
  pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.componentID   
                                              = L7_FLEX_PIMSM_MAP_COMPONENT_ID;
  pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.type          = L7_CFG_DATA;
  pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.length        
                                              = sizeof(L7_pimsmMapCfg_t);
  pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged   = L7_FALSE;

  /*---------------------------*/
  /* build default config data */
  /*---------------------------*/
  cfgData = &pimsmMapCbPtr->pPimsmMapCfgData->rtr;
  cfgData->pimsmAdminMode = FD_PIMSM_DEFAULT_ADMIN_MODE;
  cfgData->dataThresholdRate = FD_PIMSM_DEFAULT_DATA_THRESHOLD_RATE;
  cfgData->regThresholdRate = FD_PIMSM_DEFAULT_REG_THRESHOLD_RATE;
  strcpy(pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.filename, 
               pimsmMapCbPtr->cfgFilename);

  for (index = 0; index < L7_MAX_STATIC_RP_NUM; index++)
  {
    staticRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[index];
    memset(staticRp, 0 , sizeof(pimsmCfgStaticRP_t));
    inetAddressZeroSet(pimsmMapCbPtr->familyType, &staticRp->rpIpAddr);
    inetAddressZeroSet(pimsmMapCbPtr->familyType, &staticRp->rpGrpAddr);
    inetAddressZeroSet(pimsmMapCbPtr->familyType, &staticRp->rpGrpMask);
  }
  for (index = 0; index < L7_MAX_SSM_RANGE; index++)
  {
    ssmRange = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[index];
    /* Resetting ssm Table */
    memset(ssmRange, 0 , sizeof(pimsmCfgSsmRange_t));
    inetAddressZeroSet(pimsmMapCbPtr->familyType, &ssmRange->groupAddr);
    pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] = L7_TRUE;
  }
  for (index = 0; index < L7_MAX_CAND_RP_NUM; index++)
  {
    candidateRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmCandidateRp[index];
    /* Resetting candidate Rp Table */
    memset(candidateRp, 0 , sizeof(pimsmCandRPdata_t));
    inetAddressZeroSet(pimsmMapCbPtr->familyType, 
      &pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].candRPAddress);
    inetAddressZeroSet(pimsmMapCbPtr->familyType, &candidateRp->grpAddress);
    /*pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].empty = L7_TRUE;*/
     pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].operMode= L7_FALSE;
  }
  /* setting candidate BSR Table  default values */
  candidateBsr = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.candidateBsr;
  memset(candidateBsr,0,sizeof(pimsmCfgCandBSRdata_t));
  inetAddressZeroSet(pimsmMapCbPtr->familyType,
      &pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.candBSRAddress);
  candidateBsr->candBSRPriority = FD_PIMSM_CBSR_PRIORITY;
  candidateBsr->candBSRScope = FD_PIMSM_DEFAULT_INTERFACE_CBSR_SCOPE;
  pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.operMode = L7_FALSE;
  pimsmDefaultHashMaskLenGet(pimsmMapCbPtr->familyType, 
      &candidateBsr->candBSRHashMaskLength);
  /* PIM router interface configuration parameters */
  for (index=1; index < L7_IPMAP_INTF_MAX_COUNT; index++)
  {
    pimsmIntfBuildDefaultConfigData(&(pimsmMapCbPtr->
                                      pPimsmMapCfgData->intf[index]));
  }
  pimsmMapCbPtr->pPimsmMapCfgData->checksum = 0;
  return;
}


/*********************************************************************
* @purpose  Restore PIM-SM user config file to factory defaults
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmRestore(void)
{
  L7_uint32    cbIndex = L7_NULL;
  L7_uint32    protoMax = L7_NULL;
  pimsmMapCB_t *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  protoMax =  pimsmMapProtocolsMaxGet();

  for (cbIndex = PIMSM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {  
     pimsmMapCbPtr = &pimsmMapCB[cbIndex];     
     /*  disable PIM-SM if it is currently enabled  */
     pimsmMapPimsmAdminModeDisable( pimsmMapCbPtr, L7_TRUE);
     pimsmCommonBuildDefaultConfigData(pimsmMapCbPtr, 
         L7_PIMSM_CFG_VER_CURRENT);
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
     "\npimsmBuildDefaultConfigData Done\n");
     if ((pimsmMapPimsmIsOperational(pimsmMapCbPtr->familyType)== L7_TRUE))
     { 
     if (pimsmApplyConfigData(pimsmMapCbPtr) != L7_SUCCESS)
     {
       PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                         "Error applying config data");
       continue;
     }
     pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
     }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if PIM-SM user config data has changed
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimsmHasDataChanged(void)
{
  return (pimsmV4HasDataChanged() || pimsmV6HasDataChanged());
}
void pimsmResetDataChanged(void)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  (void)pimsmMapCtrlBlockGet(L7_AF_INET, &pimsmMapCbPtr);
  pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_FALSE;
  (void)pimsmMapCtrlBlockGet(L7_AF_INET6, &pimsmMapCbPtr);
  pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;

}
/*********************************************************************
* @purpose  Check if PIM-SM V4 user config data has changed
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimsmV4HasDataChanged(void)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  (void)pimsmMapCtrlBlockGet(L7_AF_INET, &pimsmMapCbPtr);

  return (pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged);
}

/*********************************************************************
* @purpose  Build default PIM-SM config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void pimsmBuildDefaultConfigData(L7_uint32 ver)
{
  pimsmMapCB_t      *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (pimsmMapCtrlBlockGet(L7_AF_INET, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Failed to get control block");
    return;
  }
  pimsmCommonBuildDefaultConfigData(pimsmMapCbPtr, ver);
  return;
}

/*********************************************************************
* @purpose  Build default PIM-SM config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void pimsm6BuildDefaultConfigData(L7_uint32 ver)
{
  pimsmMapCB_t      *pimsm6MapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (pimsmMapCtrlBlockGet(L7_AF_INET6, &pimsm6MapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Failed to get control block");
    return;
  }
  pimsmCommonBuildDefaultConfigData(pimsm6MapCbPtr, ver);
  return;
}

/*********************************************************************
* @purpose  Apply PIM-SM config data
*
* @param    pimsmMapCbPtr   @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS       Config data applied
* @returns  L7_FAILURE       Unexpected condition encountered
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmApplyConfigData(pimsmMapCB_t *pimsmMapCbPtr)
{
  L7_uint32        intIfNum, index, cfgIndex, pimsmIfMode;
  pimsmCfgCkt_t   *pCfg = L7_NULLPTR;
  pimsmCfgStaticRP_t *staticRp = L7_NULLPTR;
  pimsmCfgCandRPdata_t  *pCandRp   = L7_NULLPTR;
  pimsmCfgCandBSRdata_t *pCandBsr = L7_NULLPTR;
  pimsmCfgSsmRange_t  *ssmRange = L7_NULLPTR;
  L7_inet_addr_t        crpGroupMask, tmpAddr;
  L7_RC_t rc;

PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmAdminMode == L7_ENABLE)
  {
    pimsmMapPimsmAdminModeEnable(pimsmMapCbPtr, L7_TRUE);
  }
  else if (pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmAdminMode == L7_DISABLE)
  {
    pimsmMapPimsmAdminModeDisable(pimsmMapCbPtr, L7_TRUE);
  }
  /* apply global parms */
  
#if 0  /* Duplicate */
  pimsmMapDataThresholdRateApply(pimsmMapCbPtr,
                               pimsmMapCbPtr->pPimsmMapCfgData->rtr.
                               dataThresholdRate);
#endif
  if (pimsmMapDataThresholdRateApply(pimsmMapCbPtr, 
      pimsmMapCbPtr->pPimsmMapCfgData->rtr.dataThresholdRate) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                      "Failed to set data threshold Rate %d",
                      pimsmMapCbPtr->pPimsmMapCfgData->rtr.dataThresholdRate);
  }

  if (pimsmMapRegisterThresholdRateApply(pimsmMapCbPtr,
        pimsmMapCbPtr->pPimsmMapCfgData->rtr.regThresholdRate) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                      "Failed to set register threshold Rate %d",
                      pimsmMapCbPtr->pPimsmMapCfgData->rtr.regThresholdRate);
  }

  for (index = 0; index < L7_MAX_STATIC_RP_NUM; index++)
  {
    staticRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[index];
    if (inetIsAddressZero(&staticRp->rpIpAddr) != L7_TRUE)
    {
      (void)pimsmMapStaticRPApply(pimsmMapCbPtr, &staticRp->rpIpAddr,
                      &staticRp->rpGrpAddr, &staticRp->rpGrpMask, 
                      staticRp->overRide, index);
    }
  }
   for (index = 0; index < L7_MAX_SSM_RANGE; index++)
 {
    ssmRange = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[index];    

   if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational == L7_FALSE)
       {
         pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] = L7_TRUE;

       }
      else
      {
        if(inetIsAddressZero(&ssmRange->groupAddr)!= L7_TRUE)
        {
          pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] = L7_FALSE;

        }
        else
       {
           pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] = L7_TRUE;
       }
 
    }
}
  
 for (index=0;index<L7_MAX_CAND_RP_NUM;index++)
  {
    pCandRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmCandidateRp[index]; 
   if((pCandRp->intIfNum != 0) && (nimCheckIfNumber(pCandRp->intIfNum) == L7_SUCCESS))
   {
        /* get interface addr*/
        rc = mcastIpMapRtrIntfGlobalIpAddressGet(pimsmMapCbPtr->familyType,
                                                  pCandRp->intIfNum, &tmpAddr);
        if(rc !=L7_SUCCESS)
        {
          PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "mcastIpMapRtrIntfGlobalIpAddressGet() failed! \
                       intIfNum = %d",pCandRp->intIfNum);
          return L7_FAILURE;
        }
        inetMaskLenToMask(pimsmMapCbPtr->familyType, pCandRp->grpPrefixLength,
                        &crpGroupMask);

        if (pimsmMapCandRPApply(pimsmMapCbPtr, pCandRp->intIfNum,
                                        &tmpAddr,
                                        &pCandRp->grpAddress, &crpGroupMask,
                                        L7_ENABLE, index) != L7_SUCCESS)
        {
          PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                      "Failed to apply Candidate RP(Enable/Disable) \
                       for CandRP intfNum(%d) ",pCandRp->intIfNum);
        }
   }
   
 }  
  pCandBsr = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.candidateBsr;
    if((pCandBsr->intIfNum != 0) && (nimCheckIfNumber(pCandBsr->intIfNum) == L7_SUCCESS))
    {
       /* get interface addr*/
        rc =mcastIpMapRtrIntfGlobalIpAddressGet(pimsmMapCbPtr->familyType,
                                                pCandBsr->intIfNum, &tmpAddr);
        if(rc !=L7_SUCCESS)
        {
          PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                    "mcastIpMapRtrIntfIpAddressGet() failed! intIfNum = %d",
                     pCandBsr->intIfNum);
          return L7_FAILURE;
        }
        if (pimsmMapCandBSRApply(pimsmMapCbPtr,
         &tmpAddr, pCandBsr->intIfNum,
                                    pCandBsr->candBSRPriority,
                                    L7_ENABLE,
                                    pCandBsr->candBSRHashMaskLength,
                                    pCandBsr->candBSRScope) != L7_SUCCESS)
        {
          PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                         "Failed to apply BSR (Enable/Disable) \
                          for BSR intfNum(%d) ",
                           pCandBsr->intIfNum);
        }
  }

  /*--------------------------------------*/
  /* configure per-interface PIM-SM parms */
  /*--------------------------------------*/
  if (PIMSM_IS_READY)
  {
    for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
    {
      if (nimIntIfFromConfigIDGet(&(pimsmMapCbPtr->
              pPimsmMapCfgData->intf[cfgIndex].configId),
              &intIfNum) == L7_SUCCESS)
      {
        if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr,
                         intIfNum, &pCfg) == L7_TRUE)
        {
          if (pimsmMapInterfaceModeGet( pimsmMapCbPtr->familyType, intIfNum,
                                        &pimsmIfMode) == L7_SUCCESS)
          {
            if (pimsmMapInterfaceModeApply(pimsmMapCbPtr, intIfNum, 
                                           pimsmIfMode) != L7_SUCCESS)
            {
              PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                                "Failed to configure PIM-SM on intIfNum(%d)",
                                intIfNum);
            }            
          }
        }
      }
    }
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Retrieve a pointer to an empty configuration data for a given
*           interface if found.
*
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    pimsmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    pCfg            @b{(input)} Output pointer location
*                           @b{(output)} Pointer to PIMSM interface 
*                                        config structure
*
* @returns  L7_TRUE     an empty interface config structure was found
* @returns  L7_FALSE    no empty interface config structure entry is available
*
* @comments Does not check for a valid router interface, since that may
*           not get configured until later.  All that is required here
*           is a valid slot.port number to use when referencing the  
*           interface config data structure.
*
* @end
*********************************************************************/
static L7_BOOL pimsmMapIntfConfigEntryGet(L7_uint32 intIfNum, 
                            pimsmMapCB_t *pimsmMapCbPtr, pimsmCfgCkt_t **pCfg)
{
  L7_uint32 i = L7_NULL;
  nimConfigID_t configIdNull;

  if (!(PIMSM_IS_READY))
  {
    *pCfg = L7_NULLPTR;
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "PIMSM not ready");
    return L7_FALSE;
  }

  memset((void *)&configIdNull, 0, sizeof(nimConfigID_t));

  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&(pimsmMapCbPtr->
                pPimsmMapCfgData->intf[i].configId), &configIdNull))
    {
       pimsmMapCbPtr->pPimsmMapCfgMapTbl[intIfNum] = i;
      *pCfg = &pimsmMapCbPtr->pPimsmMapCfgData->intf[i];
      return L7_TRUE;
    }
  }
  PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                    "No empty interface config structure entry is available");
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32       @b{(input)} intIfNum  internal interface number
* @param    pimsmMapCbPtr   @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pimsmIntfCreate(L7_uint32 intIfNum, pimsmMapCB_t *pimsmMapCbPtr)
{
  nimConfigID_t configId;
  pimsmCfgCkt_t *pCfg = L7_NULLPTR;
  L7_RC_t       pimsmRC = L7_SUCCESS;
  L7_uint32     i = L7_NULL;

  if (pimsmIntfIsValid(intIfNum)!= L7_TRUE)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                      "Invalid intIfNum(%d)",intIfNum);
    return(pimsmRC);
  }

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
  {
   PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                    "Failed to get config ID from NIM for intIfNum(%d)",
                     intIfNum);
    return(pimsmRC);
  }

  pCfg = L7_NULLPTR;

  /* scan for any preexisting config records that match this **
  ** configId; if found, update the config mapping table     */
  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT; i++ )
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&pimsmMapCbPtr->
                               pPimsmMapCfgData->intf[i].configId, &configId))
    {
      pimsmMapCbPtr->pPimsmMapCfgMapTbl[intIfNum] = i;
      break;
    }
  }

  /* If an interface configuration entry is not already assigned to **
  ** the interface, assign one                                      */
  /* 
  ** Note: pimsmMapInterfaceIsConfigurable() can return L7_FALSE for two reasons: **
  ** no matching config table entry was found, or the intIfNum is not valid  **
  ** for PIM-SM... the above call to pimsmIntfIsValid() assures that if we   **
  ** get L7_FAILURE it is due to the first reason and we should try to assign  **
  ** a config table entry for this intf.                                     */

  if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr, intIfNum, &pCfg) == L7_FALSE)
  {
    if (pimsmMapIntfConfigEntryGet(intIfNum, pimsmMapCbPtr, &pCfg) == L7_TRUE)
    {
      /* successfully assigned a new config table entry for this intf, **
      ** so initialize the entry to defaults                           */
      /* Update the configuration structure with the config id */
      NIM_CONFIG_ID_COPY(&pCfg->configId, &configId);
      /* since this is a newly assigned configuration entry, initialize **
      ** it to default settings                                         */
	   pimsmIntfBuildDefaultConfigData(pCfg);
      pimsmRC = L7_SUCCESS;
    }
    else
    {
      /* unsuccessful in assigning config table entry for this intf... **
      ** report failure                                                */
      PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                        "Failed to get assign config entry for intIfNum(%d)",
                        intIfNum);
      pimsmRC = L7_FAILURE;
    }
  }
  else
  {
    /* we found an existing config table entry for this intf */
    pimsmRC = L7_SUCCESS;
  }
  return(pimsmRC);
}


/*********************************************************************
* @purpose  Propogate Routing Event notifications to PIM-SM Map task
*
* @param    intIfNum   @b{(input)} Interface number
* @param    event      @b{(input)} Event type
* @param    pData      @b{(input)} Data
* @param    pEventInfo @b{(input)} EventInfo.
*
* @returns  L7_SUCCESS  
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRoutingEventChangeCallBack (L7_uint32 intIfNum, 
                                        L7_uint32 event,
                                        void *pData, 
                                        ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  pimsmMapMsg_t msg;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  if (((event != L7_RTR_ENABLE) &&
       (event != L7_RTR_DISABLE) &&
       (event != L7_RTR_DISABLE_PENDING) &&
       (event != L7_RTR_INTF_ENABLE) &&
       (event != L7_RTR_INTF_DISABLE_PENDING) &&
       (event != L7_RTR_INTF_DISABLE) &&
       (event != L7_RTR_INTF_IPADDR_DELETE) &&
       (event != L7_RTR_INTF_IPADDR_DELETE_PENDING) &&
       (event != L7_RTR_INTF_IPADDR_ADD) &&
       (event != L7_RTR_INTF_SECONDARY_IP_ADDR_ADD) &&
       (event != L7_RTR_INTF_SECONDARY_IP_ADDR_DELETE) &&
       (event != L7_RTR_INTF_CONFIG_CLEAR)) ||
      ((intIfNum != 0) && (pimsmIntfIsValid(intIfNum) != L7_TRUE)))
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                    "Invalid eventType(%d) or intfIfNum(%d)",
                      event,intIfNum);

    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_PIMSM;
    if (pEventInfo != L7_NULLPTR)
    {
      completionData.correlator  = pEventInfo->correlator;
      completionData.handlerId   = pEventInfo->handlerId;
    }
    completionData.async_rc.rc = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);    
    return L7_SUCCESS;
  }

  memset(&msg, 0, sizeof(pimsmMapMsg_t));
    
  msg.msgId = PIMSMMAP_ROUTING_EVENT_MSG;

  msg.u.pimsmMapRoutingEventParms.intIfNum = intIfNum;
  msg.u.pimsmMapRoutingEventParms.event = event;
  msg.u.pimsmMapRoutingEventParms.pData = pData;

  if (pEventInfo != L7_NULLPTR)
  {
    memcpy(&msg.u.pimsmMapRoutingEventParms.eventInfo, 
           pEventInfo, sizeof(ASYNC_EVENT_NOTIFY_INFO_t));
  }

  if (pimsmMessageSend(PIMSM_EVENT_Q, (L7_VOIDPTR)&msg)
                   != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\nMessae Send Failed" 
                    "for eventType:%d.\n", event);
    /* getting one of these for every event when pim disabled. not acceptable. */
    /* PIMSM_MAP_LOG_MSG(L7_AF_INET,
                      "Failed to send message for event(%d)",event); */
    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_PIMSM;
    if (pEventInfo != L7_NULLPTR)
    {
      completionData.correlator  = pEventInfo->correlator;
      completionData.handlerId   = pEventInfo->handlerId;
    }
    completionData.async_rc.rc = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);    
   }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Propogate Routing Event notifications to PIM-SM6 Map task
*
* @param    intIfNum   @b{(input)} Interface number
* @param    event      @b{(input)} Event type
* @param    pData      @b{(input)} Data
* @param    pEventInfo @b{(input)} Event Info.
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsm6MapRoutingEventChangeCallBack(L7_uint32 intIfNum,
                                         L7_uint32 event,
                                         void *pData,
                                         ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  pimsmMapMsg_t msg;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;
  if (((event != RTR6_EVENT_ENABLE) &&
      (event != RTR6_EVENT_DISABLE_PENDING) &&
      (event != RTR6_EVENT_INTF_ADDR_ADD) &&
      (event != RTR6_EVENT_INTF_ADDR_DEL) &&
      (event != RTR6_EVENT_INTF_ENABLE) &&
      (event != RTR6_EVENT_INTF_DISABLE_PENDING))||
      ((intIfNum != 0) && (pimsmIntfIsValid(intIfNum) != L7_TRUE)))
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\nInput parameters are not" 
                    "valid for eventType:%d.\n", event);
    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_PIMSM;
    if (pEventInfo != L7_NULLPTR)
    {
      completionData.correlator  = pEventInfo->correlator;
      completionData.handlerId   = pEventInfo->handlerId;
    }
    completionData.async_rc.rc = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);    
    return L7_SUCCESS;
  }
  memset(&msg, 0, sizeof(pimsmMapMsg_t));

  msg.msgId = PIMSMMAP_RTR6_EVENT_MSG;

  msg.u.pimsmMapRoutingEventParms.intIfNum = intIfNum;
  msg.u.pimsmMapRoutingEventParms.event = event;
  msg.u.pimsmMapRoutingEventParms.pData = pData;

  if (pEventInfo != L7_NULLPTR)
  {
    memcpy(&msg.u.pimsmMapRoutingEventParms.eventInfo, pEventInfo, 
            sizeof(ASYNC_EVENT_NOTIFY_INFO_t));
  }
  if (pimsmMessageSend(PIMSM_EVENT_Q,(L7_VOIDPTR)&msg) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\nMessae Send Failed" 
                    "for eventType:%d.\n", event);
    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_PIMSM;
    if (pEventInfo != L7_NULLPTR)
    {
      completionData.correlator  = pEventInfo->correlator;
      completionData.handlerId   = pEventInfo->handlerId;
    }
    completionData.async_rc.rc = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);    
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This routine is called when a routing event occurs that
*           may change the operational state of the PIMSM stack.
*
* @param    intIfnum   @b{(input)} internal interface number
* @param    event      @b{(input)} state event, as defined in L7_RTR_EVENT_CHANGE_t
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
static void pimsmMapRoutingEventProcess (L7_uint32 intIfNum,
                                  L7_uint32 event,
                                  void *pData,
                                   ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  L7_inet_addr_t   ipAddr;
  pimsmCfgCkt_t    *pCfg = L7_NULLPTR;
  pimsmMapCB_t     *pimsmMapCbPtr = L7_NULLPTR;
  L7_uint32        mode = L7_NULL;
  L7_RC_t          rc = L7_SUCCESS;

  if (pimsmMapCtrlBlockGet(L7_AF_INET, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Failed to get Control Block");
    return;
  }
  inetAddressZeroSet(pimsmMapCbPtr->familyType, &ipAddr);
  if ((intIfNum == 0) ||
      ((intIfNum != 0) && (pimsmIntfIsValid(intIfNum) == L7_TRUE)))
  {
    if ((pimsmMapPimsmAdminModeGet(pimsmMapCbPtr->familyType, 
        &mode)==L7_SUCCESS) && (mode == L7_ENABLE))
    {
      switch (event)
      {
      case L7_RTR_ENABLE:
        /*Enable PIMSM gloablly*/
        rc = pimsmMapPimsmAdminModeEnable(pimsmMapCbPtr, L7_FALSE);
        break;

      case L7_RTR_DISABLE_PENDING:
        /*Disable PIMSM gloablly*/
        rc = pimsmMapPimsmAdminModeDisable(pimsmMapCbPtr, L7_FALSE);
        break;

      case L7_RTR_DISABLE:
        break;

      case L7_RTR_INTF_DISABLE:
        /* do nothing */
        break;

      case L7_RTR_INTF_DISABLE_PENDING:
      if ((pimsmMapInterfaceModeGet(pimsmMapCbPtr->familyType, intIfNum, 
            &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
      {
        if (pimsmMapIntfIsOperational(pimsmMapCbPtr->familyType, intIfNum) == L7_TRUE)
        {
          rc = pimsmMapInterfaceModeApply(pimsmMapCbPtr, intIfNum, L7_DISABLE);
        }
      }
      break;    
  
    case L7_RTR_INTF_ENABLE:
      if ((pimsmMapInterfaceModeGet(pimsmMapCbPtr->familyType, intIfNum, 
            &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
      {
        if (pimsmMapIntfIsOperational(pimsmMapCbPtr->familyType, intIfNum) != L7_TRUE)
        {
          rc = pimsmMapInterfaceModeApply(pimsmMapCbPtr, intIfNum, L7_ENABLE);
        }
      }
  
      break;    

      case L7_RTR_INTF_SECONDARY_IP_ADDR_ADD:
        /* Nothing to do. We cleaned up on the delete pending */
        break;

      case L7_RTR_INTF_SECONDARY_IP_ADDR_DELETE:
        /* Nothing to do. We cleaned up on the delete pending */
        break;

      case L7_RTR_INTF_CONFIG_CLEAR:
        if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr, intIfNum, &pCfg))
        {
          pimsmIntfBuildDefaultConfigData(pCfg);
        }
        break;

      default:
        break;
      }
    }
  }

  {
    ASYNC_EVENT_COMPLETE_INFO_t completionData;

    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_PIMSM;
    completionData.correlator  = pEventInfo->correlator;
    completionData.handlerId   = pEventInfo->handlerId;
    completionData.async_rc.rc = rc;
    if (rc == L7_SUCCESS)
    {
      completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    }
    else
    {
      completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_REGISTRANT_FAILURE;
    }
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
  }

  return;
}

/*********************************************************************
* @purpose  This routine is called when a routing event occurs that
*           may change the operational state of the PIMSM stack.
*
* @param    intIfnum  @b{(input)}  internal interface number
* @param    event     @b{(input)}  state event, as defined in L7_RTR_EVENT_CHANGE_t
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
static void pimsm6MapRoutingEventProcess (L7_uint32 intIfNum,
                                  L7_uint32 event,
                                  void *pData,
                                  ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  L7_inet_addr_t   ipAddr;
  pimsmMapCB_t    *pimsm6MapCbPtr = L7_NULLPTR;
  L7_uint32        mode = L7_NULL;
  L7_RC_t          rc = L7_SUCCESS;

  inetAddressZeroSet(L7_AF_INET6, &ipAddr);
  if (pimsmMapCtrlBlockGet(L7_AF_INET6, &pimsm6MapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Failed to get Control Block");
    return;
  }
  if ((intIfNum == 0) ||
      ((intIfNum != 0) && (pimsmIntfIsValid(intIfNum) == L7_TRUE)))
  {
    if ((pimsmMapPimsmAdminModeGet(pimsm6MapCbPtr->familyType, 
        &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
    {
      switch (event)
      {
      case RTR6_EVENT_ENABLE:
        /*Enable PIMSM gloablly*/
        rc = pimsmMapPimsmAdminModeEnable(pimsm6MapCbPtr, L7_FALSE);
        break;
      case RTR6_EVENT_DISABLE_PENDING:
        /*Disable PIMSM gloablly*/
        rc = pimsmMapPimsmAdminModeDisable(pimsm6MapCbPtr, L7_FALSE);
        break;


      case RTR6_EVENT_DISABLE:
        break;

      case RTR6_EVENT_INTF_ADDR_ADD:
        if (mcastIpMapRtrIntfIpAddressGet(pimsm6MapCbPtr->familyType, 
            intIfNum, 
           &ipAddr) == L7_SUCCESS && inetIsAddressZero(&ipAddr) != L7_TRUE)
        {
          if ((pimsmMapInterfaceModeGet(pimsm6MapCbPtr->familyType, intIfNum, 
              &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
          {
            if (pimsm6MapCbPtr->pPimsmIntfInfo[intIfNum].
                                            pimsmOperational != L7_TRUE)
            { 
              rc = pimsmMapInterfaceModeApply(pimsm6MapCbPtr, intIfNum, 
                                              L7_ENABLE);
            }
          }
        }
        break;

        case RTR6_EVENT_INTF_ADDR_DEL:
            if ((pimsmMapInterfaceModeGet(pimsm6MapCbPtr->familyType, intIfNum, 
                &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
            {
              if (pimsm6MapCbPtr->pPimsmIntfInfo[intIfNum].
                                              pimsmOperational == L7_TRUE)
              { 
                rc = pimsmMapInterfaceModeApply(pimsm6MapCbPtr, intIfNum, 
                                                mode);
              }
            }
          break;

       case RTR6_EVENT_INTF_ENABLE:
          if ((pimsmMapInterfaceModeGet(pimsm6MapCbPtr->familyType, intIfNum, 
                &mode)== L7_SUCCESS) && (mode == L7_ENABLE))
          {
            if (pimsm6MapCbPtr->pPimsmIntfInfo[intIfNum].
                                            pimsmOperational != L7_TRUE)
            {
              rc = pimsmMapInterfaceModeApply(pimsm6MapCbPtr, intIfNum, 
                                              L7_ENABLE);
            }
          }
          break;

        case RTR6_EVENT_INTF_DISABLE_PENDING:
          if ((pimsmMapInterfaceModeGet(pimsm6MapCbPtr->familyType, intIfNum, 
              &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
          {
            if (pimsm6MapCbPtr->pPimsmIntfInfo[intIfNum].
                                            pimsmOperational == L7_TRUE)
            { 
              rc = pimsmMapInterfaceModeApply(pimsm6MapCbPtr, intIfNum, 
                                              L7_DISABLE);
            }
          }
          break;



      default:
        break;
      }
    }
  }
  {
    ASYNC_EVENT_COMPLETE_INFO_t completionData;

    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_PIMSM;
    completionData.correlator  = pEventInfo->correlator;
    completionData.handlerId   = pEventInfo->handlerId;
    completionData.async_rc.rc = rc;
    if (rc == L7_SUCCESS)
    {
      completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    }
    else
    {
      completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_REGISTRANT_FAILURE;
    }
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
  }
  return;
}

/*********************************************************************
* @purpose  Propogate MCAST Event notifications to PIM-SM Map task
*
* @param    intIfNum   @b{(input)} Interface number
* @param    event      @b{(input)} Event type
* @param    pData      @b{(input)} Data
* @param    pEventInfo @b{(input)} EventInfo.
*
* @returns  L7_SUCCESS  
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapMcastEventCallBack(L7_uint32 event, 
                                   void *pData, 
                                   ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  pimsmMapMsg_t msg;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  if ((event != MCAST_RTR_ADMIN_MODE_ENABLED) &&
      (event != MCAST_RTR_ADMIN_MODE_DISABLE_PENDING))
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Invalid event(%d)",event);
    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId     = L7_MRP_PIMSM;
    completionData.correlator      = pEventInfo->correlator;
    completionData.handlerId       = pEventInfo->handlerId;
    completionData.async_rc.rc     = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);    
    return L7_SUCCESS;
  }

  memset(&msg, 0, sizeof(pimsmMapMsg_t));
    
  msg.msgId = PIMSMMAP_MCAST_EVENT_MSG;

  msg.u.pimsmMapMcastEventParms.event = event;
  msg.u.pimsmMapMcastEventParms.pData = pData;
  memcpy(&msg.u.pimsmMapMcastEventParms.eventInfo, pEventInfo, 
         sizeof(ASYNC_EVENT_NOTIFY_INFO_t));

  if (pimsmMessageSend(PIMSM_EVENT_Q,(L7_VOIDPTR)&msg) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\nMessae Send Failed" 
                    "for eventType:%d.\n", event);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process Mcast related Events.
*
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    event           @b{(input)} Event On the Interface
* @param    pimsmMapCbPtr   @b{(input)} Mapping Control Block.
*
* @returns  None.
*
* @comments none    
*
* @end
*********************************************************************/
static void pimsmMapMcastCommonEventProcess(L7_uint32 event,
                                            pimsmMapCB_t *pimsmMapCbPtr)
{
  L7_RC_t                     rc = L7_SUCCESS;

  switch (event)
  {
  case MCAST_RTR_ADMIN_MODE_ENABLED:
    if (pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmAdminMode == L7_ENABLE)
    {
       rc = pimsmMapPimsmAdminModeEnable(pimsmMapCbPtr, L7_FALSE);
       if (rc != L7_SUCCESS)
       {
         PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                           "Failed to enable PIMSM Globally for event %d", 
                            event);
       }
    }
    break;

  case MCAST_RTR_ADMIN_MODE_ENABLE_PENDING:
    /* do nothing here */
    break;

  case MCAST_RTR_ADMIN_MODE_DISABLE_PENDING:
    if (pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmAdminMode == L7_ENABLE)
    {
      rc = pimsmMapPimsmAdminModeDisable(pimsmMapCbPtr, L7_FALSE);
      if (rc != L7_SUCCESS)
      {
         PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                           "Failed to disable PIMSM Globally for event %d", 
                            event);
      }
    }
    break;

  case MCAST_RTR_ADMIN_MODE_DISABLED:
    /* do nothing here */
    break;

  default:
    break;
  }
  return;
}

/*********************************************************************
* @purpose  Propogate MCAST Event notifications to PIM-SM Map task
*
* @param    intIfNum   @b{(input)} Interface number
* @param    event      @b{(input)} Event type
* @param    pData      @b{(input)} Event-specific data
* @param    correlator @b{(input)} Correlator for event
*
* @returns  void
*
* @comments none    
*
* @end
*********************************************************************/
static void pimsmMapMcastEventProcess(L7_uint32 intIfNum, 
                              L7_uint32 event, 
                              void *pData, 
                              ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  L7_uint32                   cbIndex = L7_NULL;
  L7_uint32                   protoMax = L7_NULL;
  pimsmMapCB_t                *pimsmMapCbPtr = L7_NULLPTR;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  protoMax =  pimsmMapProtocolsMaxGet();

  for (cbIndex = PIMSM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {  
    pimsmMapCbPtr = &pimsmMapCB[cbIndex];     
    /* process Mcast Events. */
    pimsmMapMcastCommonEventProcess(event, pimsmMapCbPtr);
  }

  memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
  completionData.componentId     =  L7_MRP_PIMSM;
  completionData.correlator      = pEventInfo->correlator;
  completionData.handlerId       = pEventInfo->handlerId;
  completionData.async_rc.rc     = L7_SUCCESS;
  completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
  /* Return event completion status  */
  asyncEventCompleteTally(&completionData);

  return;
}
/*********************************************************************
* @purpose  Propogate MCAST Admin-scope Event notifications to PIM-SM Map task
*

* @param    event      @b{(input)} Event type
* @param    pData      @b{(input)} Data
* @param    pEventInfo @b{(input)} EventInfo.
*
* @returns  L7_SUCCESS  
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapAdminScopeEventChangeCallback(L7_uint32 eventType, 
                                              L7_uint32 msgLen,
                                              void *eventMsg)
{
  L7_uint32 rtrIfNum, intIfNum;

  intIfNum = ((mcastAdminMsgInfo_t *)eventMsg)->intIfNum;
  if (mcastIpMapIntIfNumToRtrIntf (L7_AF_INET, intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                    "Failed to convert rtrIfNum (%d) to intIfNum for "
                    , rtrIfNum);
    return L7_FAILURE;
  }

  ((mcastAdminMsgInfo_t *)eventMsg)->intIfNum = rtrIfNum;
  if (pimsmMapCommonCallback(L7_AF_INET, eventType, 
                               msgLen, eventMsg) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES,"Failed to post event(%d) to \
                     the PIMSM Map Message Queue ",eventType);
    return L7_FAILURE;
  }
  return L7_SUCCESS;



}
/*********************************************************************
* @purpose  Process protocol-related events
*
* @param    cbHandle    @b{(input)} pimsm CB Handle.
* @param    event       @b{(input)} Event.
* @param    pData       @b{(input)} Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
static void pimsmMapProtoEventChangeProcess(L7_uchar8 familyType,
               L7_uint32 event, void *pData)
{
  if (pimsmEventProcess(familyType, event, pData) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "Failed to process event(%d)",event);

    /* It might be possible that the MRP might be disabled after the MGMD
     * event is enqueued.  So, free the allocated MGMD Event buffers.
     */
    if (event == MCAST_EVENT_MGMD_GROUP_UPDATE)
    {
      /* Free the MCAST MGMD Events Source List Buffers */
      mcastMgmdEventsBufferPoolFree (familyType, (mgmdMrpEventInfo_t *) pData);
    }
    if (event == MCAST_EVENT_PIMSM_CONTROL_PKT_RECV ||
        event == MCAST_EVENT_IPv6_CONTROL_PKT_RECV)
    {
      mcastCtrlPktBufferPoolFree(familyType, ((mcastControlPkt_t *) pData)->payLoad);
    }
    return;
  }
  return;
}

/*********************************************************************
* @purpose  Propogate Interface notifications to PIM-SM Map task
*
* @param    intIfNum   @b{(input)} Interface number
* @param    event      @b{(input)} Event type
* @param    correlator @b{(input)} Correlator for event
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  pimsmMapMsg_t msg;
  NIM_EVENT_COMPLETE_INFO_t status; 
  
  if (((event != L7_CREATE) &&
      (event != L7_DELETE) &&
      (event != L7_ACTIVE) &&
      (event != L7_INACTIVE) &&
      (event != L7_LAG_ACQUIRE) &&
      (event != L7_PROBE_SETUP) &&
      (event != L7_LAG_RELEASE) &&
      (event != L7_PROBE_TEARDOWN))||
      ((intIfNum != 0) && (pimsmIntfIsValid(intIfNum) != L7_TRUE)))
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\nInput parameters are not" 
                    "valid for eventType:%d.\n", event);
    /* Too chatty */
    /* PIMSM_MAP_LOG_MSG(L7_NULL,"Invalid event(%d) or intIfNum(%d)",event,
                      intIfNum); */
    status.intIfNum     = intIfNum;
    status.component    = L7_FLEX_PIMSM_MAP_COMPONENT_ID;
    status.response.rc  = L7_SUCCESS;
    status.response.reason  = NIM_ERR_RC_UNUSED;
    status.event        = event;
    status.correlator   = correlator;                    
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  memset(&msg, 0, sizeof(pimsmMapMsg_t));  
  msg.msgId = PIMSMMAP_INTF_CHANGE_MSG;

  msg.u.pimsmMapIntfChangeParms.intIfNum = intIfNum;
  msg.u.pimsmMapIntfChangeParms.event = event;
  msg.u.pimsmMapIntfChangeParms.correlator = correlator;

  if (pimsmMessageSend(PIMSM_EVENT_Q,(L7_VOIDPTR)&msg) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\nMessae Send Failed" 
                    "for eventType:%d.\n", event);
    status.intIfNum     = intIfNum;
    status.component    = L7_FLEX_PIMSM_MAP_COMPONENT_ID;
    status.response.rc  = L7_SUCCESS;
    status.response.reason  = NIM_ERR_RC_UNUSED;
    status.event        = event;
    status.correlator   = correlator;                    
    nimEventStatusCallback(status);    
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process interface-related NIM Events per protocol.
*
*
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    event           @b{(input)} Event On the Interface
* @param    correlator      @b{(input)} Correlator for event
* @param    pimsmMapCbPtr   @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS or L7_FALIURE
*
*
*
* @end
*********************************************************************/
static void pimsmMapNimEventProcess(L7_uint32 intIfNum, L7_uint32 event,
                  pimsmMapCB_t *pimsmMapCbPtr)
{
  L7_uint32                 mode, linkState;
  pimsmCfgCkt_t             *pCfg = L7_NULLPTR;
  
  L7_uint32                 rtrIfNum = L7_NULL;
  L7_uint32                 currAcquired = L7_NULL;
  L7_uint32                 prevAcquired = L7_NULL;
  if (intIfNum != 0)
  {
    if ( pimsmIntfIsValid(intIfNum) != L7_TRUE)
    {
      /* If PIM-SM is not interested in this interface, inform event 
       * issuer that we have completed processing                    
       */
      return;
    }
  }

  switch (event)
  {
  case L7_UP:
  case L7_DOWN:
    break;

  case L7_CREATE:
    pimsmIntfCreate(intIfNum, pimsmMapCbPtr);
    break;

  case L7_DELETE:
    if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr, intIfNum, &pCfg) == L7_TRUE)
    {
      pimsmIntfBuildDefaultConfigData(pCfg);
      memset((void *)&(pCfg->configId), 0, sizeof(nimConfigID_t));
       pimsmMapCbPtr->pPimsmMapCfgMapTbl[intIfNum] = 0;
    }
    break;

  case L7_ATTACH:
    /* do nothing here*/
    break;

  case L7_DETACH:
    /* do nothing here*/
    break;

  case L7_ACTIVE:
    if ((pimsmMapPimsmAdminModeGet(pimsmMapCbPtr->familyType, 
         &mode)== L7_SUCCESS) && mode == L7_ENABLE)
    {
      if ((pimsmMapInterfaceModeGet(pimsmMapCbPtr->familyType, intIfNum, 
          &mode)== L7_SUCCESS) && (mode == L7_ENABLE))
      {
        if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational 
                                                           != L7_TRUE)
        {
          (void)pimsmMapInterfaceModeApply(pimsmMapCbPtr, intIfNum, L7_ENABLE);
        }
      }
    }
    break;

  case L7_INACTIVE:
    if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational == L7_TRUE)
    {
      (void)pimsmMapInterfaceModeApply(pimsmMapCbPtr, intIfNum, L7_DISABLE);
    }
    break;

  case L7_LAG_ACQUIRE:
    if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                         intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      /* check if previously acquired */
      COMPONENT_ACQ_NONZEROMASK(pimsmMapCbPtr->pPimsmIntfInfo[rtrIfNum].
           acquiredList, prevAcquired);

      /* update aquired list */
      COMPONENT_ACQ_SETMASKBIT(pimsmMapCbPtr->pPimsmIntfInfo[rtrIfNum].
           acquiredList, L7_DOT3AD_COMPONENT_ID);

      /* If the interface was not previously acquired by some component,
       * cause the acquisition to be reflected in the state of the      
       * spanning tree                                                   
       */
      if (prevAcquired == L7_FALSE)
      {
        if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational 
            == L7_TRUE)
        {
          (void)pimsmMapInterfaceModeApply(pimsmMapCbPtr, intIfNum, L7_DISABLE);
        }
      }
    }
    break;

  case L7_PROBE_SETUP:

    if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                         intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      /* check if previously acquired */
      COMPONENT_ACQ_NONZEROMASK(pimsmMapCbPtr->pPimsmIntfInfo[rtrIfNum].
          acquiredList, prevAcquired);

      /* update aquired list */
      COMPONENT_ACQ_SETMASKBIT(pimsmMapCbPtr->pPimsmIntfInfo[rtrIfNum].
          acquiredList, L7_PORT_MIRROR_COMPONENT_ID);

      /* If the interface is not previously acquired by some component,
       * cause the acquisition to be reflected in the state of the     
       * spanning tree.                                                 
       */
      if (prevAcquired == L7_FALSE)
      {
        if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational 
            == L7_TRUE)
        {
          (void)pimsmMapInterfaceModeApply(pimsmMapCbPtr, intIfNum, L7_DISABLE);
        }
      }
    }
    break;

  case L7_LAG_RELEASE:
    if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                         intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      /* update aquired list */
      COMPONENT_ACQ_CLRMASKBIT(pimsmMapCbPtr->pPimsmIntfInfo[rtrIfNum].
          acquiredList, L7_DOT3AD_COMPONENT_ID);

      /* check if currently acquired */
      COMPONENT_ACQ_NONZEROMASK(pimsmMapCbPtr->pPimsmIntfInfo[rtrIfNum].
          acquiredList, currAcquired);

      /* If the interface is not currently acquired by some component, cause 
       * the acquisition to be reflected in the state of the spanning tree   
       */
      if (currAcquired == L7_FALSE)
      {
        if (nimGetIntfLinkState(intIfNum, &linkState) == L7_SUCCESS)
        {
          if (linkState == L7_FORWARDING)
          {
            if ((pimsmMapPimsmAdminModeGet(pimsmMapCbPtr->familyType, 
                &mode)== L7_SUCCESS) && (mode == L7_ENABLE))
            {
              if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr,
                 intIfNum, &pCfg) == L7_TRUE && pCfg->mode == L7_ENABLE)
              {
                if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].
                    pimsmOperational != L7_TRUE)
                {
                  (void)pimsmMapInterfaceModeApply(pimsmMapCbPtr,
                                             intIfNum, L7_ENABLE);
                }
              }
            }
          }
        }
      }
    }
    break;

  case L7_PROBE_TEARDOWN:
    if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                         intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      /* update aquired list */
      COMPONENT_ACQ_CLRMASKBIT(pimsmMapCbPtr->pPimsmIntfInfo[rtrIfNum].
          acquiredList, L7_PORT_MIRROR_COMPONENT_ID);

      /* check if currently acquired */
      COMPONENT_ACQ_NONZEROMASK(pimsmMapCbPtr->pPimsmIntfInfo[rtrIfNum].
          acquiredList, currAcquired);

      /* If the interface is not currently acquired by some component, cause **
      ** the acquisition to be reflected in the state of the spanning tree.  */

      if (currAcquired == L7_FALSE)
      {
        if (nimGetIntfLinkState(intIfNum, &linkState) == L7_SUCCESS)
        {
          if (linkState == L7_FORWARDING)
          {
            if ((pimsmMapPimsmAdminModeGet(pimsmMapCbPtr->familyType,
                &mode)== L7_SUCCESS) && (mode == L7_ENABLE))
            {
              if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr,
                  intIfNum, &pCfg) == L7_TRUE && pCfg->mode == L7_ENABLE)
              {
                if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational
                    != L7_TRUE)
                {
                  (void)pimsmMapInterfaceModeApply(pimsmMapCbPtr,
                                            intIfNum, L7_ENABLE);
                }
              }
            }
          }
        }
      }
    }
    break;            

  default:
    break;
  }
  return;
}


/*********************************************************************
* @purpose  Process interface-related events
*
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    event       @b{(input)} Event On the Interface
* @param    correlator  @b{(input)} Correlator
*
* @returns  void
*
* @end
*********************************************************************/
static void pimsmMapIntfChangeProcess(L7_uint32 intIfNum,
                  L7_uint32 event, NIM_CORRELATOR_t correlator)
{

  L7_uint32    cbIndex = L7_NULL;
  L7_uint32    protoMax = L7_NULL;
  pimsmMapCB_t *pimsmMapCbPtr = L7_NULLPTR;
  NIM_EVENT_COMPLETE_INFO_t status;  
  
  protoMax =  pimsmMapProtocolsMaxGet();
  for (cbIndex = PIMSM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {
    pimsmMapCbPtr = &pimsmMapCB[cbIndex];
    pimsmMapNimEventProcess(intIfNum, event, pimsmMapCbPtr);
  }
  status.intIfNum     = intIfNum;
  status.component    = L7_FLEX_PIMSM_MAP_COMPONENT_ID;
  status.response.rc  = L7_SUCCESS;
  status.response.reason  = NIM_ERR_RC_UNUSED;
  status.event        = event;
  status.correlator   = correlator;                    
  nimEventStatusCallback(status);
  
  return;
}

/*********************************************************************
* @purpose  Build default PIMSM config data for an intf 
*
* @param    pCfg  @b{(input)} pointer to the DVMRP ckt configuration record
*
* @returns  void
*
* @comments none 
*
* @end
*********************************************************************/
void pimsmIntfBuildDefaultConfigData(pimsmCfgCkt_t *pCfg)
{
  pCfg->mode                  = FD_PIMSM_DEFAULT_INTERFACE_MODE;
  pCfg->intfHelloInterval     = FD_PIMSM_DEFAULT_INTERFACE_HELLO_INTERVAL;
  pCfg->intfJoinPruneInterval = FD_PIMSM_DEFAULT_INTERFACE_JOIN_PRUNE_INTERVAL;
  pCfg->drPriority          =   FD_PIMSM_DEFAULT_INTERFACE_DR_PRIORITY;
  pCfg->bsrBorderMode       = FD_PIMSM_DEFAULT_BSR_BORDER;

}


/*********************************************************************
* @purpose  Set the PIM-SM administrative mode
*
* @param    familyType      @b{(input)}     Address Family type
* @param    mode            @b{(input)}     L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Dynamic configurability is supported
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCfgSetEventQueue(pimsmMapCB_t *pimsmMapCbPtr, 
            PIMSMMAP_CFG_SET_EVENT_ID_t event, L7_VOIDPTR pMsg, L7_uint32 msgLen)
{
  pimsmMapMsg_t msg;

  if (pimsmMapCbPtr == L7_NULLPTR)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Invalid Ctrl Block %p",(void *)pimsmMapCbPtr);
    return L7_FAILURE;
  }
  if (event <= 0 && event >= PIMSMMAP_CFG_SET_LAST_EVENT)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Invalid eventType:%d", event);
    return L7_FAILURE;
  }

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_EVENTS, "Received eventType:%d.\n", event);  
  memset(&msg, 0, sizeof(pimsmMapMsg_t));
  msg.msgId = PIMSMMAP_CFG_SET_EVENT_MSG;

  msg.u.pimsmMapCfgSetEventInfo.event = event;
  msg.u.pimsmMapCfgSetEventInfo.pimsmMapCbPtr = pimsmMapCbPtr;

  if ( msgLen != 0 )
  {
     memcpy(&(msg.u.pimsmMapCfgSetEventInfo.u), pMsg, msgLen);
  }
  
  if (pimsmMessageSend(PIMSM_EVENT_Q,(L7_VOIDPTR)&msg) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Failed to send message for event %d", event);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the PIM-SM administrative mode
*
* @param    familyType      @b{(input)}  Address Family type
* @param    mode            @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Dynamic configurability is supported
*
* @end
*********************************************************************/
static void pimsmMapPimsmAdminModeProcess(pimsmMapCB_t *pimsmMapCbPtr,
                                          L7_uint32 mode)
{
  L7_RC_t       rc = L7_FAILURE;
  
  if (mode == L7_ENABLE)
  {
     rc = pimsmMapPimsmAdminModeEnable(pimsmMapCbPtr, L7_TRUE);
  }
  else
  {
      rc = pimsmMapPimsmAdminModeDisable(pimsmMapCbPtr, L7_TRUE);
  }

  if (rc == L7_FAILURE)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Failed to set PIMSM Mode(%d)",mode);
  }
  return;
}


/*********************************************************************
* @purpose  Sets the static RP information
*
* @param    familyType       @b{(input)}    Address Family type
* @param    rpIpAddr         @b{(input)}    Ip address of the RP
* @param    rpGrpAddr        @b{(input)}    Group address supported by the RP
* @param    prefixLen        @b{(input)}    prefix Length.
* @param    conflict         @b{(input)}    over ride.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
static void pimsmMapStaticRPProcess(pimsmMapCB_t *pimsmMapCbPtr, 
         pimsmMapStaticRPdata_t *staticRPdata)
{
  L7_inet_addr_t inetGroupMask;
  if (inetMaskLenToMask(pimsmMapCbPtr->familyType, 
      staticRPdata->prefixLen, &inetGroupMask) != L7_SUCCESS)
  {
     PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                       "Failed to convert MaskLen %d to Mask",
                       staticRPdata->prefixLen);
     return;   
  }
  
  if (staticRPdata->mode == L7_TRUE) 
  {
     pimsmMapStaticRPApply(pimsmMapCbPtr, &staticRPdata->rpAddr, 
           &staticRPdata->grpAddr, 
           &inetGroupMask, staticRPdata->overrideFlag, staticRPdata->index);
  }
  else
  {
     pimsmMapStaticRPRemoveApply(pimsmMapCbPtr, &staticRPdata->rpAddr, 
           &staticRPdata->grpAddr, 
           &inetGroupMask, staticRPdata->index);
  }
}



/*********************************************************************
* @purpose  Sets the static RP information
*
* @param    familyType       @b{(input)}    Address Family type
* @param    rpIpAddr         @b{(input)}    Ip address of the RP
* @param    rpGrpAddr        @b{(input)}    Group address supported by the RP
* @param    prefixLen        @b{(input)}    prefix Length.
* @param    conflict         @b{(input)}    over ride.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
static void pimsmMapCandRPProcess(pimsmMapCB_t *pimsmMapCbPtr, 
         pimsmMapCandRPdata_t *candRPdata)
{
  L7_inet_addr_t inetGroupMask;
  L7_inet_addr_t rpAddr;
  L7_uint32 index = candRPdata->index;

  if (inetMaskLenToMask(pimsmMapCbPtr->familyType, 
      candRPdata->prefixLen, &inetGroupMask) != L7_SUCCESS)
  {
     PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                       "Failed to convert MaskLen %d to Mask",
                       candRPdata->prefixLen);
     return;   
  }
  if (mcastIpMapRtrIntfGlobalIpAddressGet(pimsmMapCbPtr->familyType,
                                    candRPdata->intIfNum, &rpAddr) != L7_SUCCESS)
  {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Failed to get RP address on intfIfNum(%d)",
                        candRPdata->intIfNum);
        inetAddressZeroSet(pimsmMapCbPtr->familyType, 
        &pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].candRPAddress);
        pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].operMode = L7_FALSE;
        /*pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].empty = L7_TRUE;      */
        return;
  }
  pimsmMapCandRPApply(pimsmMapCbPtr, candRPdata->intIfNum,&rpAddr, 
      &candRPdata->grpAddr, &inetGroupMask, candRPdata->mode, 
      candRPdata->index);
}

/*********************************************************************
* @purpose  To set the candidate BSR Address.
*
* @param    familyType       @b{(input)} Address Family type
* @param    cbsrAddr         @b{(input)} bsr Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
static void  pimsmMapCandBSRProcess(pimsmMapCB_t *pimsmMapCbPtr, 
                                    pimsmMapCandBSRdata_t  *candBSRData)
{
  L7_inet_addr_t bsrAddr;
  if (mcastIpMapRtrIntfGlobalIpAddressGet(pimsmMapCbPtr->familyType,
                                    candBSRData->intIfNum, &bsrAddr) != L7_SUCCESS)
  {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Failed to get RP address on intfIfNum(%d)",
                        candBSRData->intIfNum);
     inetAddressZeroSet(pimsmMapCbPtr->familyType, 
        &pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.candBSRAddress);   
     pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.operMode = L7_FALSE;
     return;
  }
   pimsmMapCandBSRApply(pimsmMapCbPtr,&bsrAddr, 
                                candBSRData->intIfNum,
                                candBSRData->bsrPriority, 
                                candBSRData->mode,
                                candBSRData->bsrHashMaskLength,
                                candBSRData->bsrScope);
}

/*********************************************************************
* @purpose  Sets the static RP information
*
* @param    familyType       @b{(input)}    Address Family type
* @param    rpIpAddr         @b{(input)}    Ip address of the RP
* @param    rpGrpAddr        @b{(input)}    Group address supported by the RP
* @param    prefixLen        @b{(input)}    prefix Length.
* @param    conflict         @b{(input)}    over ride.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
static void
pimsmMapSSMRangeProcess (pimsmMapCB_t *pimsmMapCbPtr, 
                         pimsmMapSSMdata_t *ssmInfo)
{
  L7_uchar8 addrFamily = 0;
  L7_uint32 rtrIfNum = 0;
  L7_uint32 intIfNum = 0;

  addrFamily = pimsmMapCbPtr->familyType;

  if (ssmInfo->operMode == L7_ENABLE)
  {
    /* Get the Group Membership information from MGMD */
    for (rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
    {
      if (mcastIpMapRtrIntfToIntIfNum (addrFamily, rtrIfNum, &intIfNum) != L7_SUCCESS)
      {
        PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES,
                         "Failed to convert rtrIfNum (%d) to intIfNum for "
                         "familyType %d", rtrIfNum, addrFamily);
        continue;
      }
      if (pimsmMapIntfIsOperational (addrFamily, intIfNum) != L7_TRUE)
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "PIM-SM is not Operational on rtrIfNum - %d for Family - %d",
                        rtrIfNum, addrFamily);
        continue;
      }

      if (mgmdMapMRPGroupInfoGet (addrFamily, rtrIfNum) != L7_SUCCESS)
      {
        PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Event Post to MGMD Failed "
                         "for rtrIfNum - %d.\n", rtrIfNum);
        continue;
      }
    }
  }
  else if (ssmInfo->operMode == L7_DISABLE)
  {
    /* TBD */
  }
  else
  {
    /* Nothing else to do */
  }

  return;
}

/*********************************************************************
* @purpose  Process protocol-related events
*
* @param    cbHandle    @b{(input)} pimsm CB Handle.
* @param    event       @b{(input)} Event.
* @param    pData       @b{(input)} Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
static void pimsmMapCfgSetEventProcess(pimsmMapCB_t *pimsmMapCbPtr, 
                    L7_uint32 event, pimsmMapCfgSetEventInfo_t *pData)
{
  if (pimsmMapCbPtr == L7_NULLPTR)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Invalid Ctrl Block %p", (void *)pimsmMapCbPtr);
    return ;
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_EVENTS,
                      "Received eventType = %d.\n", event);
  switch(event)
  {
    case PIMSMMAP_ADMIN_MODE_EVENT:
      pimsmMapPimsmAdminModeProcess(pimsmMapCbPtr, pData->u.mode);
      break;

    case PIMSMMAP_SSM_EVENT:
      pimsmMapSSMRangeProcess (pimsmMapCbPtr, &pData->u.ssmData);
      break;

    case PIMSMMAP_STATIC_RP_EVENT:
      pimsmMapStaticRPProcess(pimsmMapCbPtr, &pData->u.staticRPData);
      break;

    case PIMSMMAP_CAND_RP_EVENT:
      pimsmMapCandRPProcess(pimsmMapCbPtr, &pData->u.candRPData);
      break;

    case PIMSMMAP_CAND_BSR_EVENT:
      pimsmMapCandBSRProcess(pimsmMapCbPtr, &pData->u.candBSRData);
      break;

    case PIMSMMAP_SPT_THRESHOLD_EVENT:
      pimsmMapDataThresholdRateApply(pimsmMapCbPtr, 
                                             pData->u.threshold);
      break;

    case PIMSMMAP_REGISTER_THRESHOLD_EVENT:
      pimsmMapRegisterThresholdRateApply(pimsmMapCbPtr, 
                                             pData->u.threshold);      
      break;
      
    case PIMSMMAP_INTF_MODE_EVENT:
      if (pimsmMapInterfaceModeApply(pimsmMapCbPtr,
            pData->u.intfData.intIfNum,
            pData->u.intfData.opaqueData) != L7_SUCCESS)
      {
        PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Interface Mode Apply Failed "
                         "for intIfNum - %d", pData->u.intfData.intIfNum);
        return;
      }
      break;

    case PIMSMMAP_INTF_HELLO_INTV_EVENT:
      pimsmMapInterfaceHelloIntervalApply(pimsmMapCbPtr, 
            pData->u.intfData.intIfNum,
            pData->u.intfData.opaqueData);      
      break;

    case PIMSMMAP_INTF_JP_INTV_EVENT:
      pimsmMapInterfaceJoinPruneIntervalApply(pimsmMapCbPtr,
            pData->u.intfData.intIfNum,
            pData->u.intfData.opaqueData);      
      break;

    case PIMSMMAP_INTF_DR_PRIORITY_EVENT:
      pimsmMapInterfaceDRPriorityApply(pimsmMapCbPtr, 
            pData->u.intfData.intIfNum,
            pData->u.intfData.opaqueData);      
      break;

    case PIMSMMAP_INTF_BSR_BORDER_EVENT:
       break;

    default:
      PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "invalid eventType: %d", event);
      return;
  }
  return;
}

/*********************************************************************
*
* @purpose task to handle all PIM-SM Mapping messages
*
* @param    none 
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void pimsmMapTask()
{
  pimsmMapMsg_t msg;
  pimsmMapCtrlPktAndMfcMsg_t pktAndMfcMsg;
  pimsmMapAppTmrMsg_t appTmrMsg;
  pimsmMapDataPktMsg_t dataMsg;
  L7_RC_t status;
  L7_uint32 QIndex =0;

  pimsmGblVariables_g.pimsmQueue[PIMSM_APP_TIMER_Q].QRecvBuffer =  &appTmrMsg;
  pimsmGblVariables_g.pimsmQueue[PIMSM_EVENT_Q].QRecvBuffer =  &msg;
  pimsmGblVariables_g.pimsmQueue[PIMSM_CTRL_PKT_Q].QRecvBuffer =  &pktAndMfcMsg;
  pimsmGblVariables_g.pimsmQueue[PIMSM_DATA_PKT_Q].QRecvBuffer =  &dataMsg;
  
  osapiTaskInitDone(L7_PIMSM_MAP_TASK_SYNC);

  do
  {
    /* Since we are reading from multiple queues, we cannot wait forever 
     * on the message receive from each queue. Rather than sleep between
     * queue reads, use a semaphore to indicate whether any queue has
     * data. Wait until data is available. */

    if (osapiSemaTake(pimsmGblVariables_g.msgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Failure taking PIMSM message queue semaphore");
      continue;
    }

    for(QIndex =0;QIndex < PIMSM_MAX_Q; QIndex++)
    {
      status = osapiMessageReceive(pimsmGblVariables_g.pimsmQueue[QIndex].QPointer, 
                                   pimsmGblVariables_g.pimsmQueue[QIndex].QRecvBuffer,
                                   pimsmGblVariables_g.pimsmQueue[QIndex].QSize, 
                                   L7_NO_WAIT);
      if(status == L7_SUCCESS)
      {
        break;
      }
    }

    if(status == L7_SUCCESS)
    {
      if (osapiSemaTake (pimsmGblVariables_g.queueCountersSemId, L7_WAIT_FOREVER)
                      == L7_SUCCESS)
      {
        pimsmGblVariables_g.pimsmQueue[QIndex].QCurrentMsgCnt--;
        pimsmGblVariables_g.pimsmQueue[QIndex].QRxSuccess ++;
        osapiSemaGive (pimsmGblVariables_g.queueCountersSemId);  
      }

      switch(QIndex)
      {
        case PIMSM_APP_TIMER_Q:
          pimsmMapProtoEventChangeProcess(appTmrMsg.pimsmMapAppTmrParms.familyType,
                                   appTmrMsg.pimsmMapAppTmrParms.event,L7_NULLPTR);

          break;
        case PIMSM_EVENT_Q:
         {
           switch (msg.msgId)
            {
              case (PIMSMMAP_CNFGR_MSG):
              pimsmCnfgrCommandProcess(&msg.u.cnfgrCmdData);
              break;

              case (PIMSMMAP_INTF_CHANGE_MSG):
              pimsmMapIntfChangeProcess(msg.u.pimsmMapIntfChangeParms.intIfNum, 
                                msg.u.pimsmMapIntfChangeParms.event, 
                                msg.u.pimsmMapIntfChangeParms.correlator);
              break;

              case (PIMSMMAP_ROUTING_EVENT_MSG):
              /* Processing Events from IpMap*/
              pimsmMapRoutingEventProcess(msg.u.pimsmMapRoutingEventParms.intIfNum, 
                                msg.u.pimsmMapRoutingEventParms.event, 
                                msg.u.pimsmMapRoutingEventParms.pData, 
                                &(msg.u.pimsmMapRoutingEventParms.eventInfo));
              break;

              case (PIMSMMAP_RTR6_EVENT_MSG):
              /* Processing Events from Ip6Map*/
              pimsm6MapRoutingEventProcess(msg.u.pimsmMapRoutingEventParms.intIfNum,
                                msg.u.pimsmMapRoutingEventParms.event,
                                msg.u.pimsmMapRoutingEventParms.pData,
                                &(msg.u.pimsmMapRoutingEventParms.eventInfo));
              break;

              case (PIMSMMAP_MCAST_EVENT_MSG):
              pimsmMapMcastEventProcess(msg.u.pimsmMapMcastEventParms.intIfNum, 
                                msg.u.pimsmMapMcastEventParms.event, 
                                msg.u.pimsmMapMcastEventParms.pData, 
                                &(msg.u.pimsmMapMcastEventParms.eventInfo));
              break;

              case (PIMSMMAP_PROTOCOL_EVENT_MSG):
              if (msg.u.pimsmMapProtocolEventParms.event == MCAST_EVENT_MGMD_GROUP_UPDATE)
              {
                pimsmMapCountersUpdate (PIMSM_MAP_NUM_MGMD_EVENTS, PIMSM_MAP_COUNTERS_DECREMENT);
              }
              pimsmMapProtoEventChangeProcess(msg.u.pimsmMapProtocolEventParms.familyType,
                                msg.u.pimsmMapProtocolEventParms.event,
                                (void *)&(msg.u.pimsmMapProtocolEventParms.msgData));
              break;

              case (PIMSMMAP_CFG_SET_EVENT_MSG):
              pimsmMapCfgSetEventProcess(msg.u.pimsmMapCfgSetEventInfo.pimsmMapCbPtr,
                                        msg.u.pimsmMapCfgSetEventInfo.event,
                                        (void *)&(msg.u.pimsmMapCfgSetEventInfo));
              break;

              default:
              PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"invalid message type.\n");
              break;
            }
         }
         break;
        case PIMSM_CTRL_PKT_Q:

          if((pktAndMfcMsg.pimsmMapCtrlPktAndMfcParms.event == MCAST_MFC_NOCACHE_EVENT)||
             (pktAndMfcMsg.pimsmMapCtrlPktAndMfcParms.event == MCAST_MFC_WRONGIF_EVENT)||
             (pktAndMfcMsg.pimsmMapCtrlPktAndMfcParms.event == MCAST_MFC_ENTRY_EXPIRE_EVENT))
          {
            pimsmMapCountersUpdate (PIMSM_MAP_NUM_MFC_EVENTS, PIMSM_MAP_COUNTERS_DECREMENT);
          }

          pimsmMapProtoEventChangeProcess(pktAndMfcMsg.pimsmMapCtrlPktAndMfcParms.familyType,
                                 pktAndMfcMsg.pimsmMapCtrlPktAndMfcParms.event, 
                                 (void *)&(pktAndMfcMsg.pimsmMapCtrlPktAndMfcParms.u));

          break;
        case PIMSM_DATA_PKT_Q:
           switch (dataMsg.msgId)
            {         
              case (PIMSMMAP_PROTOCOL_EVENT_MSG):
                pimsmMapProtoEventChangeProcess(dataMsg.pimsmMapDataPktEventParms.familyType,
                           dataMsg.pimsmMapDataPktEventParms.event,
                           (void *)&(dataMsg.pimsmMapDataPktEventParms.u));
                break;
              default:
                PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"invalid message type %d.\n", 
                        dataMsg.msgId);
                break;
            }         
         break;         
        default:
          PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"invalid Queue index = %d.\n", QIndex);

         break;
      }

    }

    
  } while (1);
}


/*********************************************************************
* @purpose  Wrapper function to send the the message on the appropriate Q
*
* @param    QIndex    @b{(input)} Index to the Queue on which message is to be sent.
* @param    msg        @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
**********************************************************************/

L7_RC_t pimsmMessageSend(PIMSM_QUEUE_ID_t QIndex,void *msg)
{

  if (QIndex >= PIMSM_MAX_Q)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "PIMSM Map wrong queueIndex:%d \n", QIndex);
    return L7_FAILURE;
  }

  if (osapiMessageSend (pimsmGblVariables_g.pimsmQueue[QIndex].QPointer, (L7_VOIDPTR) msg,
                        pimsmGblVariables_g.pimsmQueue[QIndex].QSize, L7_NO_WAIT, L7_MSG_PRIORITY_NORM)
                     != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "PIMSM Map Queue of queueIndex:%d  Failed while \
                      sending the message :.\n", QIndex);
    if (osapiSemaTake (pimsmGblVariables_g.queueCountersSemId, L7_WAIT_FOREVER)
                    == L7_SUCCESS)
    {
      pimsmGblVariables_g.pimsmQueue[QIndex].QSendFailedCnt++;
      osapiSemaGive (pimsmGblVariables_g.queueCountersSemId);
    }
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimsmGblVariables_g.queueCountersSemId, L7_WAIT_FOREVER)
                  == L7_SUCCESS)
  {
    pimsmGblVariables_g.pimsmQueue[QIndex].QCurrentMsgCnt++;
    if(pimsmGblVariables_g.pimsmQueue[QIndex].QCurrentMsgCnt >
                 pimsmGblVariables_g.pimsmQueue[QIndex].QMaxRx)
    {
      pimsmGblVariables_g.pimsmQueue[QIndex].QMaxRx =
                pimsmGblVariables_g.pimsmQueue[QIndex].QCurrentMsgCnt;
    }
    osapiSemaGive (pimsmGblVariables_g.queueCountersSemId);    
  }

  osapiSemaGive (pimsmGblVariables_g.msgQSema);
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Function to delete the Queue
*
* @param    None
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
**********************************************************************/

L7_RC_t pimsmMapQDelete(void)
{
  L7_uint32 count;

  for(count =0; count <PIMSM_MAX_Q;count++)
  {
    if(pimsmGblVariables_g.pimsmQueue[count].QPointer != L7_NULLPTR)
    {
      osapiMsgQueueDelete(pimsmGblVariables_g.pimsmQueue[count].QPointer);
    }
  }
  return L7_SUCCESS;
}


/********************** PIMSM DEBUG TRACE ROUTINES *******************/

/*********************************************************************
 * @purpose  Saves pimsm debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    pimsmDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t pimsmDebugSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Copy the operational states into the config file */
  pimsmDebugCfgUpdate();

  if (pimsmDebugHasDataChanged() == L7_TRUE)
  {
    pimsmDebugCfg.hdr.dataChanged = L7_FALSE;
    pimsmDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&pimsmDebugCfg,
        (L7_uint32)(sizeof(pimsmDebugCfg) - sizeof(pimsmDebugCfg.checkSum)));
    /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_FLEX_PIMSM_MAP_COMPONENT_ID, PIMSM_DEBUG_CFG_FILENAME ,
            (L7_char8 *)&pimsmDebugCfg, (L7_uint32)sizeof(pimsmDebugCfg_t))) == L7_ERROR)
    {
      PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error on call to osapiFsWrite routine on config file %s\n",PIMSM_DEBUG_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
 * @purpose  Restores pimsm debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    pimsmDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t pimsmDebugRestore(void)
{
  L7_RC_t rc;

  pimsmDebugBuildDefaultConfigData(PIMSM_DEBUG_CFG_VER_CURRENT);

  pimsmDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = pimsmApplyDebugConfigData();

  return rc;
}
/*********************************************************************
 * @purpose  Checks if pimsm debug config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL pimsmDebugHasDataChanged(void)
{
  return pimsmDebugCfg.hdr.dataChanged;
}

/*********************************************************************
 * @purpose  Build default pimsm config data
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void pimsmDebugBuildDefaultConfigData(L7_uint32 ver)
{

  /* setup file header */
  pimsmDebugCfg.hdr.version = ver;
  pimsmDebugCfg.hdr.componentID = L7_FLEX_PIMSM_MAP_COMPONENT_ID;
  pimsmDebugCfg.hdr.type = L7_CFG_DATA;
  pimsmDebugCfg.hdr.length = (L7_uint32)sizeof(pimsmDebugCfg);
  strcpy((L7_char8 *)pimsmDebugCfg.hdr.filename, PIMSM_DEBUG_CFG_FILENAME);
  pimsmDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&pimsmDebugCfg.cfg, 0, sizeof(pimsmDebugCfg.cfg));
}

/*********************************************************************
 * @purpose  Apply pimsm debug config data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes    Called after a default config is built
 *
 * @end
 *********************************************************************/
L7_RC_t pimsmApplyDebugConfigData(void)
{
  L7_uchar8 familyIndex = 0;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 flagIndex;

  for (familyIndex = 0; familyIndex < PIMSM_MAP_CB_MAX; 
       familyIndex++)
  {
    pimsmDebugTraceFlags_t *pimsmDebugTraceFlags = 
       &(pimsmDebugCfg.cfg.pimsmDebugTraceFlag[familyIndex]);
    for(flagIndex = 0;  flagIndex < PIMSM_DEBUG_LAST_TRACE ; flagIndex ++)
    { 
       if(((*(pimsmDebugTraceFlags[flagIndex/PIMSM_DEBUG_TRACE_FLAG_BITS_MAX])) & 
          (PIMSM_DEBUG_TRACE_FLAG_VALUE << (flagIndex % PIMSM_DEBUG_TRACE_FLAG_BITS_MAX))) != 0)
       {
         pimsmDebugTraceFlagSet(familyIndex+1, flagIndex, L7_TRUE);
       }
       else
       {
         pimsmDebugTraceFlagSet(familyIndex+1, flagIndex, L7_FALSE);
       }
    }
  }
  return rc;
}




