/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\policy\std_policy_api.c
*
* @purpose Contains definitions to support LVL7 standard policy offerings
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
#include "string.h"
#include "nvstoreapi.h"
#include "nimapi.h"
#include "defaultconfig.h"
#include "log.h"
#include "dtlapi.h"
#include "std_policy.h"
#include "std_policy_api.h"
#include "statsapi.h"
#include "l7_cnfgr_api.h"
#include "platform_config.h"

policyCfgData_t  *policyCfgData = L7_NULLPTR;
extern L7_uint32 *policyMapTbl;


/*********************************************************************
* @purpose  Enable or disable flow control for an interface
*
* @param    intIfNum    intIfNum
*
* @param    mode        admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policyIntfFlowCtrlModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (pCfg->flowControlMode != mode)
  {
     pCfg->flowControlMode = mode;
     policyCfgData->cfgHdr.dataChanged = L7_TRUE;

     return policyIntfFlowCtrlModeApply(intIfNum,mode);
  }
  else
  {
     return L7_SUCCESS;
  }
}


/*********************************************************************
* @purpose  Get flow control for an interface
*
* @param    intIfNum    intIfNum
*
* @param    mode        admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policyIntfFlowCtrlModeGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  *mode = pCfg->flowControlMode;
  return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Sets the Unit's System Flow Control Mode
*
* @param    mode  System Flow Control Mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 policySystemFlowControlModeSet(L7_uint32 mode)
{
  if (policyCfgData->systemFlowControlMode != mode)
  {
     policyCfgData->systemFlowControlMode = mode;
     policyCfgData->cfgHdr.dataChanged = L7_TRUE;

     /* dtl call */
     if (dtlPolicyIntfAllFlowCtrlModeSet(mode) != L7_SUCCESS)
     {
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
               "Failure setting system flow control mode to %d\n", mode);
       return L7_FAILURE;
     }
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Returns the Unit's System Flow Control Mode
*
* @param    void
*
* @returns  mode  System Flow Control Mode (L7_ENABLE or L7_DISABLE)
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemFlowControlModeGet(void)
{
  return(policyCfgData->systemFlowControlMode);
}


/*********************************************************************
* @purpose  Sets the Unit's System Broadcast Storm Mode
*
* @param    mode  System Broadcast Storm Mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemBcastStormModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 cfgIndex;
  L7_uint32 intIfNum;
  nimConfigID_t configIdNull;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  policyCfgData->systemBcastStormMode = mode;
  policyCfgData->cfgHdr.dataChanged = L7_TRUE;


  if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
     for (cfgIndex = 1; cfgIndex < L7_POLICY_INTF_MAX_COUNT; cfgIndex++)
     {
       if (NIM_CONFIG_ID_IS_EQUAL(&policyCfgData->policyIntfCfgData[cfgIndex].configId, &configIdNull))
         continue;
       if (nimIntIfFromConfigIDGet(&(policyCfgData->policyIntfCfgData[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
         continue;
       policyCfgData->policyIntfCfgData[cfgIndex].bcastStormMode = mode;

       if (dtlPolicyIntfBcastCtrlModeSet(intIfNum, mode, 
                                         policyCfgData->policyIntfCfgData[cfgIndex].bcastStormThreshold, 
                                         policyCfgData->policyIntfCfgData[cfgIndex].bcastStormBurstSize /* PTin added: stormcontrol */, 
                                         policyCfgData->policyIntfCfgData[cfgIndex].bcastStormThresholdUnit) != L7_SUCCESS)
         rc = L7_FAILURE;
     }
  }
  else
  {
     if (dtlPolicyIntfAllBcastCtrlModeSet(mode, 
                                          policyCfgData->systemBcastStormThreshold, 
                                          policyCfgData->systemBcastStormBurstSize /* PTin added: stormcontrol */, 
                                          policyCfgData->systemBcastStormThresholdUnit) != L7_SUCCESS)
     {
        rc = L7_FAILURE;
     }
  }

  if ( rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Failure setting system broadcast storm mode to %d\n", mode);
  }

  return(rc);
}


/*********************************************************************
* @purpose  Sets the Unit's System Broadcast Storm Threshold
*
* @param    threshold  System Broadcast Storm Threshold
* @param    burstSize  Burst Size (only for Percentage/Kbps units)
* @param    rate_unit  Threshold rate unit it percent, pps, or kbps
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemBcastStormThresholdSet(L7_uint32 threshold, L7_uint32 burstSize /* PTin added: stormcontrol */, L7_RATE_UNIT_t rate_unit)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 cfgIndex;
  L7_uint32 intIfNum;
  nimConfigID_t configIdNull;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  policyCfgData->systemBcastStormThreshold = threshold;
  policyCfgData->systemBcastStormBurstSize = burstSize;     /* PTin added: stormcontrol */
  policyCfgData->systemBcastStormThresholdUnit = rate_unit;
  policyCfgData->cfgHdr.dataChanged = L7_TRUE;

  if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
     for (cfgIndex = 1; cfgIndex < L7_POLICY_INTF_MAX_COUNT; cfgIndex++)
     {
       if (NIM_CONFIG_ID_IS_EQUAL(&policyCfgData->policyIntfCfgData[cfgIndex].configId, &configIdNull))
         continue;
       if (nimIntIfFromConfigIDGet(&(policyCfgData->policyIntfCfgData[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
         continue;
       policyCfgData->policyIntfCfgData[cfgIndex].bcastStormThreshold = threshold;
       policyCfgData->policyIntfCfgData[cfgIndex].bcastStormBurstSize = burstSize;      /* PTin added: stormcontrol */
       policyCfgData->policyIntfCfgData[cfgIndex].bcastStormThresholdUnit = rate_unit;
     }
  }

  if (dtlPolicyIntfAllBcastCtrlModeSet(policyCfgData->systemBcastStormMode, 
                                       threshold, 
                                       burstSize /* PTin added: stormcontrol */, 
                                       rate_unit) != L7_SUCCESS)
  {
     rc = L7_FAILURE;
  }

  if ( rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Failure setting system broadcast storm threshold to %d\n", threshold);
  }

  return(rc);
}



/*********************************************************************
* @purpose  Sets the Unit's System Multicast Storm Mode
*
* @param    mode  System Multicast Storm Mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemMcastStormModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 cfgIndex;
  L7_uint32 intIfNum;
  nimConfigID_t configIdNull;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  policyCfgData->systemMcastStormMode = mode;
  policyCfgData->cfgHdr.dataChanged = L7_TRUE;

  if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
     for (cfgIndex = 1; cfgIndex < L7_POLICY_INTF_MAX_COUNT; cfgIndex++)
     {
       if (NIM_CONFIG_ID_IS_EQUAL(&policyCfgData->policyIntfCfgData[cfgIndex].configId, &configIdNull))
         continue;
       if (nimIntIfFromConfigIDGet(&(policyCfgData->policyIntfCfgData[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
         continue;
       policyCfgData->policyIntfCfgData[cfgIndex].mcastStormMode = mode;

       if (dtlPolicyIntfMcastCtrlModeSet(intIfNum, mode, 
                                         policyCfgData->policyIntfCfgData[cfgIndex].mcastStormThreshold, 
                                         policyCfgData->policyIntfCfgData[cfgIndex].mcastStormBurstSize /* PTin added: stormcontrol */, 
                                         policyCfgData->policyIntfCfgData[cfgIndex].mcastStormThresholdUnit) != L7_SUCCESS)
         rc = L7_FAILURE;
     }
  }
  else
  {
    if (dtlPolicyIntfAllMcastCtrlModeSet(mode, 
                                         policyCfgData->systemMcastStormThreshold, 
                                         policyCfgData->systemMcastStormBurstSize /* PTin added: stormcontrol */, 
                                         policyCfgData->systemMcastStormThresholdUnit) != L7_SUCCESS)
      rc = L7_FAILURE;
  }

  if ( rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Failure setting system multicast storm mode to %d\n", mode);
  }

  return(rc);
}


/*********************************************************************
* @purpose  Sets the Unit's System Multicast Storm Threshold
*
* @param    threshold  System Multicast Storm Threshold
* @param    burstSize  Burst Size (only for Percentage/Kbps units)
* @param    rate_unit  Threshold rate unit it percent, pps, or kbps
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemMcastStormThresholdSet(L7_uint32 threshold, L7_uint32 burstSize /* PTin added: stormcontrol */, L7_RATE_UNIT_t rate_unit)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 cfgIndex;
  L7_uint32 intIfNum;
  nimConfigID_t configIdNull;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  policyCfgData->systemMcastStormThreshold = threshold;
  policyCfgData->systemMcastStormBurstSize = burstSize;     /* PTin added: stormcontrol */
  policyCfgData->systemMcastStormThresholdUnit = rate_unit;
  policyCfgData->cfgHdr.dataChanged = L7_TRUE;

  if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
     for (cfgIndex = 1; cfgIndex < L7_POLICY_INTF_MAX_COUNT; cfgIndex++)
     {
       if (NIM_CONFIG_ID_IS_EQUAL(&policyCfgData->policyIntfCfgData[cfgIndex].configId, &configIdNull))
         continue;
       if (nimIntIfFromConfigIDGet(&(policyCfgData->policyIntfCfgData[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
         continue;
       policyCfgData->policyIntfCfgData[cfgIndex].mcastStormThreshold = threshold;
       policyCfgData->policyIntfCfgData[cfgIndex].mcastStormBurstSize = burstSize;      /* PTin added: stormcontrol */
       policyCfgData->policyIntfCfgData[cfgIndex].mcastStormThresholdUnit = rate_unit;
     }
  }

  if (dtlPolicyIntfAllMcastCtrlModeSet(policyCfgData->systemMcastStormMode, 
                                       threshold, 
                                       burstSize /* PTin added: stormcontrol */, 
                                       rate_unit) != L7_SUCCESS)
    rc = L7_FAILURE;

  if ( rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Failure setting system multicast storm threshold to %d\n", threshold);
  }

  return(rc);
}



/*********************************************************************
* @purpose  Sets the Unit's System Destination lookup failure Storm Mode
*
* @param    mode  System Broadcast Storm Mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemUcastStormModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 cfgIndex;
  L7_uint32 intIfNum;
  nimConfigID_t configIdNull;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  policyCfgData->systemUcastStormMode = mode;
  policyCfgData->cfgHdr.dataChanged = L7_TRUE;


  if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
     for (cfgIndex = 1; cfgIndex < L7_POLICY_INTF_MAX_COUNT; cfgIndex++)
     {
       if (NIM_CONFIG_ID_IS_EQUAL(&policyCfgData->policyIntfCfgData[cfgIndex].configId, &configIdNull))
         continue;
       if (nimIntIfFromConfigIDGet(&(policyCfgData->policyIntfCfgData[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
         continue;
       policyCfgData->policyIntfCfgData[cfgIndex].ucastStormMode = mode;

       if (dtlPolicyIntfUcastCtrlModeSet(intIfNum, mode, 
                                         policyCfgData->policyIntfCfgData[cfgIndex].ucastStormThreshold, 
                                         policyCfgData->policyIntfCfgData[cfgIndex].ucastStormBurstSize /* PTin added: stormcontrol */, 
                                         policyCfgData->policyIntfCfgData[cfgIndex].ucastStormThresholdUnit) != L7_SUCCESS)
         rc = L7_FAILURE;
     }
  }
  else
  {
    if (dtlPolicyIntfAllUcastCtrlModeSet(mode, 
                                         policyCfgData->systemUcastStormThreshold, 
                                         policyCfgData->systemUcastStormBurstSize /* PTin added: stormcontrol */, 
                                         policyCfgData->systemUcastStormThresholdUnit) != L7_SUCCESS)
      rc = L7_FAILURE;
  }

  if ( rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Failure setting system unicast storm mode to %d\n", mode);
  }

  return(rc);
}


/*********************************************************************
* @purpose  Sets the Unit's System Destination lookup failure Storm Threshold
*
* @param    threshold  System Multicast Storm Threshold
* @param    burstSize  Burst Size (only for Percentage/Kbps units)
* @param    rate_unit  Threshold rate unit it percent, pps, or kbps
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemUcastStormThresholdSet(L7_uint32 threshold, L7_uint32 burstSize /* PTin added: stormcontrol */, L7_RATE_UNIT_t rate_unit)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 cfgIndex;
  L7_uint32 intIfNum;
  nimConfigID_t configIdNull;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  policyCfgData->systemUcastStormThreshold = threshold;
  policyCfgData->systemUcastStormBurstSize = burstSize;     /* PTin added: stormcontrol */
  policyCfgData->systemUcastStormThresholdUnit = rate_unit;
  policyCfgData->cfgHdr.dataChanged = L7_TRUE;


  if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
     for (cfgIndex = 1; cfgIndex < L7_POLICY_INTF_MAX_COUNT; cfgIndex++)
     {
       if (NIM_CONFIG_ID_IS_EQUAL(&policyCfgData->policyIntfCfgData[cfgIndex].configId, &configIdNull))
         continue;
       if (nimIntIfFromConfigIDGet(&(policyCfgData->policyIntfCfgData[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
         continue;
       policyCfgData->policyIntfCfgData[cfgIndex].ucastStormThreshold = threshold;
       policyCfgData->policyIntfCfgData[cfgIndex].ucastStormBurstSize = burstSize;      /* PTin added: stormcontrol */
       policyCfgData->policyIntfCfgData[cfgIndex].ucastStormThresholdUnit = rate_unit;
     }
  }

  if (dtlPolicyIntfAllUcastCtrlModeSet(policyCfgData->systemUcastStormMode, 
                                       threshold, 
                                       burstSize /* PTin added: stormcontrol */, 
                                       rate_unit) != L7_SUCCESS)
    rc = L7_FAILURE;

  if ( rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Failure setting system unicast storm threshold to %d\n", threshold);
  }

  return(rc);
}



/*********************************************************************
* @purpose  Sets an interface's System Broadcast Storm Mode
*
* @param    mode  System Broadcast Storm Mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemBcastStormModeIntfSet(L7_uint32 interface, L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /* Nothing to be done */
  if (pCfg->bcastStormMode == mode)
  {
    PT_LOG_TRACE(LOG_CTX_API, "policySystemBcastStormModeIntfSet: Nothing to be done");
    return L7_SUCCESS;
  }

  pCfg->bcastStormMode = mode;
  policyCfgData->cfgHdr.dataChanged = L7_TRUE;

  if ( dtlPolicyIntfBcastCtrlModeSet(interface, mode, 
                                     pCfg->bcastStormThreshold, 
                                     pCfg->bcastStormBurstSize /* PTin added: stormcontrol */, 
                                     pCfg->bcastStormThresholdUnit) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Failure setting port %d broadcast storm mode to %d\n", interface, mode);
    rc = L7_FAILURE;
  }

  return(rc);
}


/*********************************************************************
* @purpose  Sets an interface's System Broadcast Storm Threshold
*
* @param    threshold  System Broadcast Storm Threshold
* @param    burstSize  Burst Size (only for Percentage/Kbps units)
* @param    rate_unit  Threshold rate unit it percent, pps, or kbps
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemBcastStormThresholdIntfSet(L7_uint32 interface, L7_uint32 threshold, L7_uint32 burstSize /* PTin added: stormcontrol */,
                                                 L7_RATE_UNIT_t rate_unit)
{
  L7_RC_t rc = L7_SUCCESS;
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /* Nothing to be done */
  if (pCfg->bcastStormThreshold     == threshold &&
      pCfg->bcastStormBurstSize     == burstSize &&
      pCfg->bcastStormThresholdUnit == rate_unit)
  {
    PT_LOG_TRACE(LOG_CTX_API, "policySystemBcastStormThresholdIntfSet: Nothing to be done");
    return L7_SUCCESS;
  }

  pCfg->bcastStormThreshold = threshold;
  pCfg->bcastStormBurstSize = burstSize;      /* PTin added: stormcontrol */
  pCfg->bcastStormThresholdUnit = rate_unit;
  policyCfgData->cfgHdr.dataChanged = L7_TRUE;

  if (pCfg->bcastStormMode)
  {
    if ( dtlPolicyIntfBcastCtrlModeSet(interface, pCfg->bcastStormMode, 
                                       threshold, 
                                       burstSize /* PTin added: stormcontrol */, 
                                       rate_unit) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
              "Failure setting port %d broadcast storm threshold to %d\n", interface, threshold);
      rc = L7_FAILURE;
    }
  }

  return(rc);
}



/*********************************************************************
* @purpose  Sets an interface's System Multicast Storm Mode
*
* @param    mode  System Broadcast Storm Mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemMcastStormModeIntfSet(L7_uint32 interface, L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /* Nothing to be done */
  if (pCfg->mcastStormMode == mode)
  {
    PT_LOG_TRACE(LOG_CTX_API, "policySystemMcastStormModeIntfSet: Nothing to be done");
    return L7_SUCCESS;
  }

  pCfg->mcastStormMode = mode;
  policyCfgData->cfgHdr.dataChanged = L7_TRUE;


  if ( dtlPolicyIntfMcastCtrlModeSet(interface, mode, 
                                     pCfg->mcastStormThreshold, 
                                     pCfg->mcastStormBurstSize /* PTin added: stormcontrol */, 
                                     pCfg->mcastStormThresholdUnit) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Failure setting port %d multicast storm mode to %d\n", interface, mode);
    rc = L7_FAILURE;
  }

  return(rc);
}


/*********************************************************************
* @purpose  Sets an interface's System Broadcast Storm Threshold
*
* @param    threshold  System Broadcast Storm Threshold
* @param    burstSize  Burst Size (only for Percentage/Kbps units)
* @param    rate_unit  Threshold rate unit it percent, pps, or kbps
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemMcastStormThresholdIntfSet(L7_uint32 interface, L7_uint32 threshold, L7_uint32 burstSize /* PTin added: stormcontrol */,
                                                 L7_RATE_UNIT_t rate_unit)
{
  L7_RC_t rc = L7_SUCCESS;
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /* Nothing to be done */
  if (pCfg->mcastStormThreshold     == threshold &&
      pCfg->mcastStormBurstSize     == burstSize &&
      pCfg->mcastStormThresholdUnit == rate_unit)
  {
    PT_LOG_TRACE(LOG_CTX_API, "policySystemMcastStormThresholdIntfSet: Nothing to be done");
    return L7_SUCCESS;
  }

  pCfg->mcastStormThreshold = threshold;
  pCfg->mcastStormBurstSize = burstSize;      /* PTin added: stormcontrol */
  pCfg->mcastStormThresholdUnit = rate_unit;
  policyCfgData->cfgHdr.dataChanged = L7_TRUE;

  if (pCfg->mcastStormMode)
  {
    if ( dtlPolicyIntfMcastCtrlModeSet(interface, pCfg->mcastStormMode, 
                                       threshold, 
                                       burstSize /* PTin added: stormcontrol */, 
                                       rate_unit) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
              "Failure setting port %d multicast storm threshold to %d\n", interface, threshold);
      rc = L7_FAILURE;
    }
  }

  return(rc);
}



/*********************************************************************
* @purpose  Sets an interface's System Destination lookup failure Storm Mode
*
* @param    mode  System Broadcast Storm Mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemUcastStormModeIntfSet(L7_uint32 interface, L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /* Nothing to be done */
  if (pCfg->ucastStormMode == mode)
  {
    PT_LOG_TRACE(LOG_CTX_API, "policySystemUcastStormModeIntfSet: Nothing to be done");
    return L7_SUCCESS;
  }

  pCfg->ucastStormMode = mode;
  policyCfgData->cfgHdr.dataChanged = L7_TRUE;

  if ( dtlPolicyIntfUcastCtrlModeSet(interface, mode, 
                                     pCfg->ucastStormThreshold, 
                                     pCfg->ucastStormBurstSize /* PTin added: stormcontrol */, 
                                     pCfg->ucastStormThresholdUnit) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Failure setting port %d unicast storm mode to %d\n", interface, mode);
    rc = L7_FAILURE;
  }

  return(rc);
}


/*********************************************************************
* @purpose  Sets an interface's System Unicast Storm Threshold
*
* @param    threshold  System Unicast Storm Threshold
* @param    burstSize  Burst Size (only for Percentage/Kbps units)
* @param    rate_unit  Threshold rate unit it percent, pps, or kbps
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemUcastStormThresholdIntfSet(L7_uint32 interface, L7_uint32 threshold, L7_uint32 burstSize /* PTin added: stormcontrol */,
                                                 L7_RATE_UNIT_t rate_unit)
{
  L7_RC_t rc = L7_SUCCESS;
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
    return L7_FAILURE;

    /* Nothing to be done */
  if (pCfg->ucastStormThreshold     == threshold &&
      pCfg->ucastStormBurstSize     == burstSize &&
      pCfg->ucastStormThresholdUnit == rate_unit)
  {
    PT_LOG_TRACE(LOG_CTX_API, "policySystemUcastStormThresholdIntfSet: Nothing to be done");
    return L7_SUCCESS;
  }

  pCfg->ucastStormThreshold = threshold;
  pCfg->ucastStormBurstSize = burstSize;      /* PTin added: stormcontrol */
  pCfg->ucastStormThresholdUnit = rate_unit;
  policyCfgData->cfgHdr.dataChanged = L7_TRUE;

  if (pCfg->ucastStormMode)
  {
    if ( dtlPolicyIntfUcastCtrlModeSet(interface, pCfg->ucastStormMode, 
                                       threshold, 
                                       burstSize /* PTin added: stormcontrol */, 
                                       rate_unit) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
              "Failure setting port %d unicast storm threshold to %d\n", interface, threshold);
      rc = L7_FAILURE;
    }
  }

  return(rc);
}



/*********************************************************************
* @purpose  Returns the Unit's System Broadcast Storm Mode
*
* @param    void
*
* @returns  mode  System Broadcast Storm Mode
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemBcastStormModeGet(void)
{
  return(policyCfgData->systemBcastStormMode);
}

/*********************************************************************
* @purpose  Returns the Unit's System Multicast Storm Mode
*
* @param    void
*
* @returns  mode  System Broadcast Storm Mode
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemMcastStormModeGet(void)
{
  return(policyCfgData->systemMcastStormMode);
}

/*********************************************************************
* @purpose  Returns the Unit's System Destination lookup failure Storm Mode
*
* @param    void
*
* @returns  mode  System Broadcast Storm Mode
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemUcastStormModeGet(void)
{
  return(policyCfgData->systemUcastStormMode);
}


/*********************************************************************
* @purpose  Returns the Unit's System Broadcast Storm threshold
*
* @param    *threshold  System Broadcast Storm Threshold
* @param    *burstSize  Burst Size (only for Percentage/Kbps units)
* @param    *rate_unit  Threshold units in PERCENT or PPS
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policySystemBcastStormThresholdGet(L7_uint32 *threshold, L7_uint32 *burstSize /* PTin added: stormcontrol */,
                                             L7_RATE_UNIT_t *rate_unit)
{
  if (threshold != L7_NULLPTR)  *threshold = policyCfgData->systemBcastStormThreshold;
  if (burstSize != L7_NULLPTR)  *burstSize = policyCfgData->systemBcastStormBurstSize;      /* PTin added: stormcontrol */
  if (rate_unit != L7_NULLPTR)  *rate_unit = policyCfgData->systemBcastStormThresholdUnit;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns the Unit's System Multicast Storm threshold
*
* @param    *threshold  System Multicast Storm Threshold
* @param    *burstSize  Burst Size (only for Percentage/Kbps units)
* @param    *rate_unit  Threshold units in PERCENT or PPS
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policySystemMcastStormThresholdGet(L7_uint32 *threshold, L7_uint32 *burstSize /* PTin added: stormcontrol */,
                                             L7_RATE_UNIT_t *rate_unit)
{
  if (threshold != L7_NULLPTR)  *threshold = policyCfgData->systemMcastStormThreshold;
  if (burstSize != L7_NULLPTR)  *burstSize = policyCfgData->systemMcastStormBurstSize;      /* PTin added: stormcontrol */
  if (rate_unit != L7_NULLPTR)  *rate_unit = policyCfgData->systemMcastStormThresholdUnit;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns the Unit's System Destination lookup failure Storm threshold
*
* @param    *threshold  System Unknown Unicast Storm Threshold
* @param    *burstSize  Burst Size (only for Percentage/Kbps units)
* @param    *rate_unit  Threshold units in PERCENT or PPS
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policySystemUcastStormThresholdGet(L7_uint32 *threshold, L7_uint32 *burstSize /* PTin added: stormcontrol */,
                                             L7_RATE_UNIT_t *rate_unit)
{
  if (threshold != L7_NULLPTR)  *threshold = policyCfgData->systemUcastStormThreshold;
  if (burstSize != L7_NULLPTR)  *burstSize = policyCfgData->systemUcastStormBurstSize;      /* PTin added: stormcontrol */
  if (rate_unit != L7_NULLPTR)  *rate_unit = policyCfgData->systemUcastStormThresholdUnit;
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Returns an interface's Broadcast Storm Mode
*
* @param    void
*
* @returns  mode  System Broadcast Storm Mode
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemBcastStormModeIntfGet(L7_uint32 interface)
{
  policyIntfCfgData_t *pCfg;

  /* Calling usmdb function takes care of errors */
  /* Do not dereference pCfg if policyMapIntfIsConfigurable returns L7_FALSE
   * return 0 if that is the case. usmdb does not catch the case of
   * component being in an invalid phase.
   */
  if (policyMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
    return 0;

  return(pCfg->bcastStormMode);
}

/*********************************************************************
* @purpose  Returns an interface's Multicast Storm Mode
*
* @param    void
*
* @returns  mode  System Broadcast Storm Mode
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemMcastStormModeIntfGet(L7_uint32 interface)
{
  policyIntfCfgData_t *pCfg;

  /* Calling usmdb function takes care of errors */
  /* Do not dereference pCfg if policyMapIntfIsConfigurable returns L7_FALSE
   * return 0 if that is the case. usmdb does not catch the case of
   * component being in an invalid phase.
   */
  if (policyMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
    return 0;

  return(pCfg->mcastStormMode);
}

/*********************************************************************
* @purpose  Returns an interface's Destination lookup failure Storm Mode
*
* @param    void
*
* @returns  mode  System Broadcast Storm Mode
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policySystemUcastStormModeIntfGet(L7_uint32 interface)
{
  policyIntfCfgData_t *pCfg;

  /* Calling usmdb function takes care of errors */
  /* Do not dereference pCfg if policyMapIntfIsConfigurable returns L7_FALSE
   * return 0 if that is the case. usmdb does not catch the case of
   * component being in an invalid phase.
   */
  if (policyMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
    return 0;

  return(pCfg->ucastStormMode);
}


/*********************************************************************
* @purpose  Returns an interface's Broadcast Storm threshold
*
* @param    *threshold  Broadcast Storm Threshold
* @param    *burstSize  Burst Size (only for Percentage/Kbps units)
* @param    *rate_unit  Threshold units in PERCENT or PPS
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policySystemBcastStormThresholdIntfGet(L7_uint32 interface, L7_uint32 *threshold, L7_uint32 *burstSize /* PTin added: stormcontrol */,
                                          L7_RATE_UNIT_t *rate_unit)
{
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (threshold != L7_NULLPTR)  *threshold = pCfg->bcastStormThreshold;
  if (burstSize != L7_NULLPTR)  *burstSize = pCfg->bcastStormBurstSize;     /* PTin added: stormcontrol */
  if (rate_unit != L7_NULLPTR)  *rate_unit = pCfg->bcastStormThresholdUnit;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns an interface's Multicast Storm threshold
*
* @param    *threshold  Multicast Storm Threshold
* @param    *burstSize  Burst Size (only for Percentage/Kbps units)
* @param    *rate_unit  Threshold units in PERCENT or PPS
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policySystemMcastStormThresholdIntfGet(L7_uint32 interface, L7_uint32 *threshold, L7_uint32 *burstSize /* PTin added: stormcontrol */,
                                          L7_RATE_UNIT_t *rate_unit)
{
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (threshold != L7_NULLPTR)  *threshold = pCfg->mcastStormThreshold;
  if (burstSize != L7_NULLPTR)  *burstSize = pCfg->mcastStormBurstSize;     /* PTin added: stormcontrol */
  if (rate_unit != L7_NULLPTR)  *rate_unit = pCfg->mcastStormThresholdUnit;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns an interface's Destination lookup failure Storm threshold
*
* @param    *threshold  Unknown Unicast Storm Threshold
* @param    *burstSize  Burst Size (only for Percentage/Kbps units)
* @param    *rate_unit  Threshold units in PERCENT or PPS
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policySystemUcastStormThresholdIntfGet(L7_uint32 interface, L7_uint32 *threshold, L7_uint32 *burstSize /* PTin added: stormcontrol */,
                                          L7_RATE_UNIT_t *rate_unit)
{
  policyIntfCfgData_t *pCfg;

  if (policyMapIntfIsConfigurable(interface, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (threshold != L7_NULLPTR)  *threshold = pCfg->ucastStormThreshold;
  if (burstSize != L7_NULLPTR)  *burstSize = pCfg->ucastStormBurstSize;     /* PTin added: stormcontrol */
  if (rate_unit != L7_NULLPTR)  *rate_unit = pCfg->ucastStormThresholdUnit;

  return L7_SUCCESS;
}


