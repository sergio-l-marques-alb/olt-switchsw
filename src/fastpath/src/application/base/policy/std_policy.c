/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\policy\std_policy.c
*
* @purpose Contains definitions to support the FlowControl and BroadCastStorm
*
* @component
*
* @comments
*
* @create 04/02/2001
*
* @author rjindal
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/



#include "l7_common.h"
#include "osapi.h"
#include "nimapi.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "log.h"
#include "dtlapi.h"
#include "std_policy.h"
#include "std_policy_api.h"
#include "platform_config.h"
#include "l7_product.h"

extern policyCfgData_t  *policyCfgData;

extern policyCnfgrState_t policyCnfgrState;

L7_uint32 *policyMapTbl = L7_NULLPTR;
policyDeregister_t policyDeregister = {L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE};
PORTEVENT_MASK_t policyEventMask_g;


void policyBuildTestIntfConfigData(policyIntfCfgData_t *pCfg, L7_uint32 seed);

/*********************************************************************
* @purpose  Build default policy intf config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void policyBuildDefaultIntfConfigData(nimConfigID_t *configId, policyIntfCfgData_t *pCfg)
{
  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);

  pCfg->bcastStormMode = FD_POLICY_DEFAULT_BCAST_STORM_MODE;
  pCfg->mcastStormMode = FD_POLICY_DEFAULT_MCAST_STORM_MODE;
  pCfg->ucastStormMode = FD_POLICY_DEFAULT_UCAST_STORM_MODE;
  pCfg->bcastStormThreshold = FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD;
  pCfg->bcastStormBurstSize = FD_POLICY_DEFAULT_BCAST_STORM_BURSTSIZE;          /* PTin added: stormcontrol */
  pCfg->bcastStormThresholdUnit = FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD_UNIT;
  pCfg->mcastStormThreshold = FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD;
  pCfg->mcastStormBurstSize = FD_POLICY_DEFAULT_MCAST_STORM_BURSTSIZE;          /* PTin added: stormcontrol */
  pCfg->mcastStormThresholdUnit = FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD_UNIT;
  pCfg->ucastStormThreshold = FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD;
  pCfg->ucastStormBurstSize = FD_POLICY_DEFAULT_UCAST_STORM_BURSTSIZE;          /* PTin added: stormcontrol */
  pCfg->ucastStormThresholdUnit = FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD_UNIT;
  pCfg->flowControlMode = FD_POLICY_DEFAULT_FLOW_CONTROL_MODE;
}


/*********************************************************************
* @purpose  Build default policy config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    Builds Garp, VLAN and port default data which will be
*           applied in dot1qApplyConfigData
*           Also inits the config file header
*
* @end
*********************************************************************/
void policyBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32   cfgIndex;
  nimConfigID_t configId[L7_POLICY_INTF_MAX_COUNT];

  memset(&configId[0], 0, sizeof(nimConfigID_t) * L7_POLICY_INTF_MAX_COUNT);

  for (cfgIndex = 1; cfgIndex < L7_POLICY_INTF_MAX_COUNT; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configId[cfgIndex], &policyCfgData->policyIntfCfgData[cfgIndex].configId);

  memset((char*)policyCfgData, 0, sizeof(policyCfgData_t));

  /* Initialize all interface-related entries in the config file to default values. */
  for (cfgIndex = 1; cfgIndex < L7_POLICY_INTF_MAX_COUNT; cfgIndex++)
    policyBuildDefaultIntfConfigData(&configId[cfgIndex], &policyCfgData->policyIntfCfgData[cfgIndex]);

  policyCfgData->systemBcastStormMode = FD_POLICY_DEFAULT_BCAST_STORM_MODE;
  policyCfgData->systemMcastStormMode = FD_POLICY_DEFAULT_MCAST_STORM_MODE;
  policyCfgData->systemUcastStormMode = FD_POLICY_DEFAULT_UCAST_STORM_MODE;

  policyCfgData->systemFlowControlMode = FD_POLICY_DEFAULT_FLOW_CONTROL_MODE;

  policyCfgData->systemBcastStormThreshold = FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD;
  policyCfgData->systemBcastStormBurstSize = FD_POLICY_DEFAULT_BCAST_STORM_BURSTSIZE;           /* PTin added: stormcontrol */
  policyCfgData->systemBcastStormThresholdUnit = FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD_UNIT;
  policyCfgData->systemMcastStormThreshold = FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD;
  policyCfgData->systemMcastStormBurstSize = FD_POLICY_DEFAULT_MCAST_STORM_BURSTSIZE;           /* PTin added: stormcontrol */
  policyCfgData->systemMcastStormThresholdUnit = FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD_UNIT;
  policyCfgData->systemUcastStormThreshold = FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD;
  policyCfgData->systemUcastStormBurstSize = FD_POLICY_DEFAULT_UCAST_STORM_BURSTSIZE;           /* PTin added: stormcontrol */
  policyCfgData->systemUcastStormThresholdUnit = FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD_UNIT;

  /* Build header */
  strcpy(policyCfgData->cfgHdr.filename, POLICY_CFG_FILENAME);
  policyCfgData->cfgHdr.version = ver;
  policyCfgData->cfgHdr.componentID = L7_POLICY_COMPONENT_ID;
  policyCfgData->cfgHdr.type = L7_CFG_DATA;
  policyCfgData->cfgHdr.length = sizeof(policyCfgData_t);
  policyCfgData->cfgHdr.dataChanged = L7_FALSE;
}


/*********************************************************************
* @purpose  Applies std policy config data
*
* @param    void
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policyApplyConfigData(void)
{
  L7_uint32 intIfNum;
  L7_uint32 cfgIndex;
  nimConfigID_t configIdNull;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  /* Set System storm control modes if per port config not available*/
  if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
     (void)policySystemBcastStormThresholdSet(policyCfgData->systemBcastStormThreshold,
                                              policyCfgData->systemBcastStormBurstSize /* PTin added: stormcontrol */, 
                                              policyCfgData->systemBcastStormThresholdUnit);
  }
  if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
     (void)policySystemMcastStormThresholdSet(policyCfgData->systemMcastStormThreshold,
                                              policyCfgData->systemMcastStormBurstSize /* PTin added: stormcontrol */, 
                                              policyCfgData->systemMcastStormThresholdUnit);
  }
  if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
     (void)policySystemUcastStormThresholdSet(policyCfgData->systemUcastStormThreshold,
                                              policyCfgData->systemUcastStormBurstSize /* PTin added: stormcontrol */, 
                                              policyCfgData->systemUcastStormThresholdUnit);
  }

  /* Set System Flow control mode, if supported */
  if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_FALSE)
  {
     /* Do not use policySystemFlowControlModeSet(), as it cross-checks with system mode */
     if (dtlPolicyIntfAllFlowCtrlModeSet(policyCfgData->systemFlowControlMode) != L7_SUCCESS)
     {
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
               "Failure setting system flow control mode to %d\n", policyCfgData->systemFlowControlMode);
     }
  }

  if (POLICY_IS_READY)
  {
    for (cfgIndex = 1; cfgIndex < L7_POLICY_INTF_MAX_COUNT; cfgIndex++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&policyCfgData->policyIntfCfgData[cfgIndex].configId, &configIdNull))
        continue;
      if (nimIntIfFromConfigIDGet(&(policyCfgData->policyIntfCfgData[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
        continue;
      if (policyApplyIntfConfigData(intIfNum) != L7_SUCCESS)
        return L7_FAILURE;
    }
  }

  /* As of this writing, the error returns are logged in the called routines.
     To allow for future change, and to be consistent with similar routines
      in other application, return L7_RC_t */

  policyCfgData->cfgHdr.dataChanged = L7_FALSE;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Apply the broadcast rate limiting for an interface
*
* @param    intIfNum    intIfNum
*
* @param    mode        admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @param    threshold   threshold value
*
* @param    rate_unit   L7_RATE_UNIT_PERCENT or L7_RATE_UNIT_PPS or L7_RATE_UNIT_KBPS
*
* @returns  L7_RC_t
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policyIntfBcastCtrlModeApply(L7_uint32 intIfNum, L7_uint32 mode, L7_int32 threshold, L7_uint32 burstSize /* PTin added: stormControl */, 
                                     L7_RATE_UNIT_t rate_unit)
{
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (dtlPolicyIntfBcastCtrlModeSet(intIfNum, mode, threshold, burstSize /* PTin added: stormcontrol */, rate_unit) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Failure setting interface %s broadcast storm mode to %d and threshold to %d\n",
            ifName, mode, threshold);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Apply the multicast rate limiting for an interface
*
* @param    intIfNum    intIfNum
*
* @param    mode        admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @param    threshold   threshold value
*
* @param    rate_unit   L7_RATE_UNIT_PERCENT or L7_RATE_UNIT_PPS or L7_RATE_UNIT_KBPS
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policyIntfMcastCtrlModeApply(L7_uint32 intIfNum, L7_uint32 mode, L7_int32 threshold, L7_uint32 burstSize /* PTin added: stormControl */, 
                                     L7_RATE_UNIT_t rate_unit)
{
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (dtlPolicyIntfMcastCtrlModeSet(intIfNum, mode, threshold, burstSize /* PTin added: stormcontrol */, rate_unit) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Failure setting interface %s multicast storm mode to %d and threshold to %d\n",
            ifName, mode, threshold);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Apply the destination lookup failure rate limiting for an interface
*
* @param    intIfNum    intIfNum
*
* @param    mode        admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @param    threshold   threshold value
*
* @param    rate_unit   L7_RATE_UNIT_PERCENT or L7_RATE_UNIT_PPS or L7_RATE_UNIT_KBPS
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policyIntfUcastCtrlModeApply(L7_uint32 intIfNum, L7_uint32 mode, L7_int32 threshold, L7_uint32 burstSize /* PTin added: stormControl */, 
                                     L7_RATE_UNIT_t rate_unit)
{
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (dtlPolicyIntfUcastCtrlModeSet(intIfNum, mode, threshold, burstSize /* PTin added: stormcontrol */, rate_unit) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Failure setting interface %s unicast storm mode to %d and threshold to %d\n",
            ifName, mode, threshold);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Apply flow control for an interface
*
* @param    intIfNum    intIfNum
*
* @param    mode        admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_RC_T
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policyIntfFlowCtrlModeApply(L7_uint32 intIfNum, L7_uint32 mode)
{
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (dtlPolicyIntfFlowCtrlModeSet(intIfNum, mode) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Failure setting interface %s flow control mode to %d\n", ifName, mode);
    return L7_FAILURE;
  }

  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose  To process the Callback for L7_PORT_INSERT
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t policyApplyIntfConfigData(L7_uint32 intIfNum)
{
  policyIntfCfgData_t *pCfg;
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (policyMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_TRUE)
    {
      if (policyIntfFlowCtrlModeApply(intIfNum, pCfg->flowControlMode) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
                "policyApplyIntfConfigData: error returned from policyIntfFlowCtrlModeApply() setting intf=%s to mode=%d\n",
                ifName, pCfg->flowControlMode);
        rc = L7_FAILURE;
      }
    }

    if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (policyIntfBcastCtrlModeApply(intIfNum,
                                        pCfg->bcastStormMode,
                                        pCfg->bcastStormThreshold,
                                        pCfg->bcastStormBurstSize /* PTin added: stgormcontrol */, 
                                        pCfg->bcastStormThresholdUnit)  != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
                 "policyApplyIntfConfigData: error returned from policyIntfBcastCtrlModeApply() setting intf=%s to mode=%d\n",
                 ifName, pCfg->bcastStormMode);
         rc = L7_FAILURE;
       }
    }

    if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (policyIntfMcastCtrlModeApply(intIfNum,
                                        pCfg->mcastStormMode, 
                                        pCfg->mcastStormThreshold, 
                                        pCfg->mcastStormBurstSize /* PTin added: stormcontrol */, 
                                        pCfg->mcastStormThresholdUnit)  != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
                 "policyApplyIntfConfigData: error returned from policyIntfMcastCtrlModeApply() setting intf=%s to mode=%d\n",
                 ifName, pCfg->mcastStormMode);
         rc = L7_FAILURE;
       }
    }

    if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (policyIntfUcastCtrlModeApply(intIfNum,
                                        pCfg->ucastStormMode, 
                                        pCfg->ucastStormThreshold, 
                                        pCfg->ucastStormBurstSize /* PTin added: stormcontrol */, 
                                        pCfg->ucastStormThresholdUnit)  != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
                 "policyApplyIntfConfigData: error returned from policyIntfUcastCtrlModeApply() setting intf=%s to mode=%d\n",
                 ifName, pCfg->ucastStormMode);
         rc = L7_FAILURE;
       }
    }

  }
 /* Create policy stats for interface. */
  (void)policyStatsCreateIntf(intIfNum);

  return(rc);
}

/*********************************************************************
* @purpose  Checks if policy user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL policyHasDataChanged(void)
{
  if (policyDeregister.policyHasDataChanged == L7_TRUE)
    return L7_FALSE;

  return policyCfgData->cfgHdr.dataChanged;
}
void policyResetDataChanged(void)
{
  policyCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Saves policy user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policySave(void)
{
  L7_RC_t rc=L7_SUCCESS;

  if (policyDeregister.policySave == L7_TRUE)
    return L7_SUCCESS;


  if (policyCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    policyCfgData->cfgHdr.dataChanged = L7_FALSE;
    policyCfgData->checkSum = nvStoreCrc32((L7_char8 *)policyCfgData,
                                           sizeof(policyCfgData_t) - sizeof(policyCfgData->checkSum));

    if (sysapiCfgFileWrite(L7_POLICY_COMPONENT_ID,
                           POLICY_CFG_FILENAME,
                           (L7_char8 *)policyCfgData,
                           sizeof(policyCfgData_t)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
              "Error on call to osapiFsWrite routine on config file %s\n",POLICY_CFG_FILENAME);
    }
  }
  return(rc);
}

/*********************************************************************
* @purpose  Restores policy user config file to factore defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policyRestore(void)
{

  if (policyDeregister.policyRestore == L7_TRUE)
    return L7_SUCCESS;

  policyBuildDefaultConfigData(policyCfgData->cfgHdr.version);
  (void)policyApplyConfigData();

  policyCfgData->cfgHdr.dataChanged = L7_TRUE;
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Register all policy counters with the Statistics Manager
*
* @param    None
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t policyStatsCreate()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 i, j, k;
  L7_uint32     listSize = sizeof(stdPolicyMutants) / sizeof(statsParm_entry_t);
  pStatsParm_list_t pTmp1 = stdPolicyMutants;
  pMutling_id_t pTmp2;

  for (i = 1; i < L7_MAX_INTERFACE_COUNT; i++)
  {
    /* Reset pTmp1 for initializing the counters for the next interface */
    pTmp1 = stdPolicyMutants;

    for (j = 0; j < listSize; j++)
    {
      pTmp1->cKey = i;
      if (pTmp1->pMutlingsParmList != L7_NULL)
      {
        pTmp2 = pTmp1->pMutlingsParmList->pMutlingParms;

        for (k = 0; ((pTmp2 != L7_NULL) &&
                     (k < pTmp1->pMutlingsParmList->listSize)); k++)
        {
          pTmp2->cKey = i;
          pTmp2++;
        }
      }
      pTmp1++;
    }

    /* Call the stats manager to create the counters for this interface */
    rc = statsCreate (listSize, stdPolicyMutants);
  }

  return rc;
}

/*********************************************************************
* @purpose  Register all policy counters with the Statistics Manager
*           for a specified interface.
*
* @param    None
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t policyStatsCreateIntf(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 j, k;
  L7_uint32     listSize = sizeof(stdPolicyMutants) / sizeof(statsParm_entry_t);
  pStatsParm_list_t pTmp1 = stdPolicyMutants;
  pMutling_id_t pTmp2;

  /* Reset pTmp1 for initializing the counters for the next interface */
  pTmp1 = stdPolicyMutants;

  for (j = 0; j < listSize; j++)
  {
    pTmp1->cKey = intIfNum;
    if (pTmp1->pMutlingsParmList != L7_NULL)
    {
      pTmp2 = pTmp1->pMutlingsParmList->pMutlingParms;

      for (k = 0; ((pTmp2 != L7_NULL) &&
                   (k < pTmp1->pMutlingsParmList->listSize)); k++)
      {
        pTmp2->cKey = intIfNum;
        pTmp2++;
      }
    }
    pTmp1++;
  }

  /* Call the stats manager to create the counters for this interface */
  rc = statsCreate (listSize, stdPolicyMutants);

  return rc;
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to policy interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL policyMapIntfIsConfigurable(L7_uint32 intIfNum, policyIntfCfgData_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;

  if (!(POLICY_IS_READY))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Error on call to policyMapIntfIsConfigurable routine outside the EXECUTE state\n");
    return L7_FALSE;
  }

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  index = policyMapTbl[intIfNum];

  if (index == 0)
    return L7_FALSE;

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(policyCfgData->policyIntfCfgData[index].configId)) == L7_FALSE)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      /* if we get here, either we have a table management error between policyCfgData and policyMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
              "Error accessing POLICY config data for interface %s in policyMapIntfIsConfigurable.\n", ifName);
      return L7_FALSE;
    }
  }

  *pCfg = &policyCfgData->policyIntfCfgData[index];

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to policy interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL policyMapIntfConfigEntryGet(L7_uint32 intIfNum, policyIntfCfgData_t **pCfg)
{
  L7_uint32 i = L7_POLICY_INTF_MAX_COUNT;
  nimConfigID_t configId;
  nimConfigID_t configIdNull;
  static L7_uint32 nextIndex = 1;

  if ((POLICY_IS_READY != L7_TRUE) ||
      (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS))
  {
    return L7_FALSE;
  }

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  /* Avoid N^2 processing when interfaces created at startup */
  if ((nextIndex < L7_POLICY_INTF_MAX_COUNT) &&
      (NIM_CONFIG_ID_IS_EQUAL(&policyCfgData->policyIntfCfgData[nextIndex].configId, &configIdNull) == L7_TRUE))
  {
    /* entry corresponding to nextIndex is free so use it, move speculative index to next entry
     * for next time
     */
    i = nextIndex++;
  }
  else
  /* Cached nextIndex is in use. Resort to search from beginning. N^2. */
  {
    for (i = 1; i < L7_POLICY_INTF_MAX_COUNT; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&policyCfgData->policyIntfCfgData[i].configId, &configIdNull))
      {
        /* found a free entry, update the speculative index to next entry for next time */
        nextIndex = i+1;
        break;
      }
    }
  }

  if (i < L7_POLICY_INTF_MAX_COUNT)
  {
    policyMapTbl[intIfNum] = i;
    *pCfg = &policyCfgData->policyIntfCfgData[i];
    return L7_TRUE;
  }
  return L7_FALSE;
}


/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t policyIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t configId;
  policyIntfCfgData_t *pCfg;
  L7_uint32 i;

  if (policyIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 1; i < L7_POLICY_INTF_MAX_COUNT; i++ )
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&policyCfgData->policyIntfCfgData[i].configId, &configId))
    {
      policyMapTbl[intIfNum] = i;
      break;
    }
  }

  pCfg = L7_NULL;

  /* If an interface configuration entry is not already assigned to the interface,
     assign one */
  if (policyMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    if(policyMapIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    /* Update the configuration structure with the config id and build the default config */
    if (pCfg != L7_NULL)
      policyBuildDefaultIntfConfigData(&configId,pCfg);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DETACH
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t policyIntfDetach(L7_uint32 intIfNum)
{
  policyIntfCfgData_t *pCfg;
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (policyMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_TRUE)
    {
      if (policyIntfFlowCtrlModeApply(intIfNum, FD_POLICY_DEFAULT_FLOW_CONTROL_MODE) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
                "policyIntfDetach: error returned from policyIntfFlowCtrlModeApply() setting intf=%s to mode=%d\n",
                ifName, FD_POLICY_DEFAULT_FLOW_CONTROL_MODE);
        rc = L7_FAILURE;
      }
    }

    if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (policyIntfBcastCtrlModeApply(intIfNum,
                                        FD_POLICY_DEFAULT_BCAST_STORM_MODE,
                                        FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD,
                                        FD_POLICY_DEFAULT_BCAST_STORM_BURSTSIZE /* PTin added: stormcontrol */, 
                                        FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD_UNIT)  != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
                 "policyIntfDetach: error returned from policyIntfBcastCtrlModeApply() setting intf=%s to mode=%d and threshold %d\n",
                 ifName, FD_POLICY_DEFAULT_BCAST_STORM_MODE, FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD);
         rc = L7_FAILURE;
       }
    }

    if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (policyIntfMcastCtrlModeApply(intIfNum,
                                        FD_POLICY_DEFAULT_MCAST_STORM_MODE, 
                                        FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD, 
                                        FD_POLICY_DEFAULT_MCAST_STORM_BURSTSIZE /* PTin added: stormcontrol */, 
                                        FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD_UNIT)  != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
                 "policyIntfDetach: error returned from policyIntfMcastCtrlModeApply() setting intf=%s to mode=%d and threshold %d\n",
               ifName, FD_POLICY_DEFAULT_MCAST_STORM_MODE, FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD);
         rc = L7_FAILURE;
       }
    }

    if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (policyIntfUcastCtrlModeApply(intIfNum,
                                        FD_POLICY_DEFAULT_UCAST_STORM_MODE, 
                                        FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD, 
                                        FD_POLICY_DEFAULT_UCAST_STORM_BURSTSIZE /* PTin added: stormcontrol */, 
                                        FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD_UNIT)  != L7_SUCCESS)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
                 "policyIntfDetach: error returned from policyIntfUcastCtrlModeApply() setting intIfNum=%s to mode=%d and threshold %d\n",
              ifName, FD_POLICY_DEFAULT_UCAST_STORM_MODE, FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD);
         rc = L7_FAILURE;
       }
    }

  }
  return(rc);
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t policyIntfDelete(L7_uint32 intIfNum)
{
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
    memset((void *)&policyMapTbl[intIfNum], 0, sizeof(L7_uint32));
    policyCfgData->cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process NIM events.
*
* @param    L7_uint32  intIfNum  internal interface number
* @param    L7_uint32  event     event, defined by L7_PORT_EVENTS_t
* @param    L7_uint32  correlator event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t policyIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;

  status.intIfNum     = intIfNum;
  status.component    = L7_POLICY_COMPONENT_ID;
  status.event        = event;
  status.correlator   = correlator;

  if (policyDeregister.policyIntfChangeCallbackFlag == L7_TRUE)
  {
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  if (policyIsValidIntf(intIfNum) != L7_TRUE)
  {
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  if (POLICY_IS_READY)
  {

    switch (event)
    {
      case L7_CREATE:
        rc = policyIntfCreate(intIfNum);
        break;

      case L7_ATTACH: /* for stats creation */
      case L7_UP:     /* for storm-control features needing current link speed */
        rc = policyApplyIntfConfigData(intIfNum);
        break;

      case L7_DETACH:
        rc = policyIntfDetach(intIfNum);
        break;

      case L7_DELETE:
        rc = policyIntfDelete(intIfNum);
        break;

      default:
        rc = L7_SUCCESS;
        break;
    }
  } else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "policyIntfChangeCallback: Received an interface event callback while not in EXECUTE state.\n");
  }

  status.response.rc = rc;
  nimEventStatusCallback(status);
  return rc;
}

/*********************************************************************
*
* @purpose  Determine whether an interface is in the attached state.
*
* @param    intIfNum  @b{(input)}  internal interface number
*
* @returns  L7_TRUE if the interface is attached.
*           L7_FALSE otherwise.
*
* @notes    The interface is considered to be attached if the state is
*           either L7_INTF_ATTACHING or L7_INTF_ATTACHED.
*
* @end
*********************************************************************/
L7_BOOL policyIntfIsAttached(L7_uint32 intIfNum)
{
    L7_NIM_QUERY_DATA_t queryData;

    /* get the interface state */
    queryData.intIfNum = intIfNum;
    queryData.request = L7_NIM_QRY_RQST_STATE;
    if (nimIntfQuery(&queryData) != L7_SUCCESS)
    {
        return L7_FALSE;
    }

    if ((queryData.data.state == L7_INTF_ATTACHING) ||
        (queryData.data.state == L7_INTF_ATTACHED))
        return L7_TRUE;
    else
        return L7_FALSE;
}

/*********************************************************************
* @purpose  Propogate Startup notifications
*
* @param    NIM_STARTUP_PHASE_t  startupPhase
*
* @notes
*
* @end
*********************************************************************/
void policyStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    if (policyIsValidIntf(intIfNum) == L7_TRUE)
    {
      switch (startupPhase)
      {
      case NIM_INTERFACE_CREATE_STARTUP:
        (void)policyIntfCreate(intIfNum);
        break;

      case NIM_INTERFACE_ACTIVATE_STARTUP:
        if (policyIntfIsAttached(intIfNum) == L7_TRUE)
        {
          (void)policyApplyIntfConfigData(intIfNum);
        }
        break;

      default:
        break;
      }
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  switch (startupPhase)
  {
    case NIM_INTERFACE_CREATE_STARTUP:
      /* Register with NIM to receive port CREATE/DELETE events */
      PORTEVENT_SETMASKBIT(policyEventMask_g, L7_CREATE);
      PORTEVENT_SETMASKBIT(policyEventMask_g, L7_DELETE);
      nimRegisterIntfEvents(L7_POLICY_COMPONENT_ID, policyEventMask_g);
      break;

    case NIM_INTERFACE_ACTIVATE_STARTUP:
      PORTEVENT_SETMASKBIT(policyEventMask_g, L7_ATTACH);
      PORTEVENT_SETMASKBIT(policyEventMask_g, L7_DETACH);
      PORTEVENT_SETMASKBIT(policyEventMask_g, L7_UP);
      nimRegisterIntfEvents(L7_POLICY_COMPONENT_ID, policyEventMask_g);
      break;

    default:
      break;
  }

  nimStartupEventDone(L7_POLICY_COMPONENT_ID);

  /* Inform cnfgr that policy has completed it HW updates */
  if (startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
  {
    cnfgrApiComponentHwUpdateDone(L7_POLICY_COMPONENT_ID,
                                  L7_CNFGR_HW_APPLY_CONFIG);
  }
}




/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void policyBuildTestConfigData(void)
{

    L7_uint32   cfgIndex;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/

    /* Initialize all interface-related entries in the config file to default values. */
    for (cfgIndex = 1; cfgIndex < L7_POLICY_INTF_MAX_COUNT; cfgIndex++)
      policyBuildTestIntfConfigData(&policyCfgData->policyIntfCfgData[cfgIndex], cfgIndex);

    policyCfgData->systemBcastStormMode = L7_ENABLE;
    policyCfgData->systemFlowControlMode = L7_ENABLE;


 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   policyCfgData->cfgHdr.dataChanged = L7_TRUE;
   sysapiPrintf("Built test config data\n");


}


/*********************************************************************
* @purpose  Build default policy intf config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void policyBuildTestIntfConfigData(policyIntfCfgData_t *pCfg, L7_uint32 seed)
{

  pCfg->bcastStormMode = L7_ENABLE;
  pCfg->flowControlMode = L7_ENABLE;
}


/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void policyConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;
    nimUSP_t    usp;
    L7_uint32   cfgIndex;
    policyIntfCfgData_t *pCfg;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(policyCfgData->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/


    sysapiPrintf("policyCfgData->systemBcastStormMode   = %d\n",
                 policyCfgData->systemBcastStormMode);
    sysapiPrintf("policyCfgData->systemFlowControlMode   = %d\n",
                 policyCfgData->systemFlowControlMode);

    for (cfgIndex = 1; cfgIndex < L7_POLICY_INTF_MAX_COUNT; cfgIndex++)
    {

        memset((void *)&usp, 0, sizeof(nimUSP_t));
        pCfg = &policyCfgData->policyIntfCfgData[cfgIndex];
        if (nimUspFromConfigIDGet(&(policyCfgData->policyIntfCfgData[cfgIndex].configId), &usp)
                                   != L7_SUCCESS)
        {
          usp.unit = 0;
          usp.slot = 0;
          usp.port = 0;
        }
        sysapiPrintf("Port %d/%d/%d\n", usp.unit, usp.slot, usp.port);

        sysapiPrintf("bcastStormMode        = %d\n", pCfg->bcastStormMode);
        sysapiPrintf("flowControlMode       = %d\n", pCfg->flowControlMode);

        sysapiPrintf("\n");

    }


    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("policyCfgData->checkSum : %u\n", policyCfgData->checkSum);


}



/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/


