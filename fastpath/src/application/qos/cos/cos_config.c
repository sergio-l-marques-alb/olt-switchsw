/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   cos_config.c
*
* @purpose    COS component config file and interface state change handling
*
* @component  cos
*
* @comments   none
*
* @create     03/17/2004
*
* @author     gpaussa
*
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "cnfgr.h"
#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "osapi.h"
#include "nvstoreapi.h"
#include "log.h"
#include "dot1q_api.h"
#include "usmdb_unitmgr_api.h"
#include "dot3ad_api.h"
#include "mirror_api.h"
#include "l7_cos_api.h"
#include "cos_cnfgr.h"
#include "cos_config.h"
#include "cos_util.h"

L7_cosCfgData_t   *pCosCfgData_g  = L7_NULLPTR;
L7_uint32         *cosMapTbl_g    = L7_NULLPTR;
cosIntfInfo_t     *pCosIntfInfo_g = L7_NULLPTR;
cosDeregister_t   cosDeregister_g;
PORTEVENT_MASK_t  cosPortEventMask_g;

/* 802.1p table initializer matrix
 *
 * NOTE: Each row represents a different device configuration regarding the
 *       number of COS queues it supports.  Only one row is used to init
 *       the COS component IP Precedence and IP DSCP mapping tables, based
 *       on the L7_MAX_CFG_QUEUES_PER_PORT value as the major index.
 *
 * NOTE: Rows are indexed by (numTrafficClasses-1).
 */
static L7_uchar8 cosMapTableInit[FD_QOS_COS_MAX_QUEUES_PER_INTF]
                                [L7_QOS_COS_MAP_NUM_IPPREC] =
{
  FD_QOS_COS_MAP_TABLE_INIT_1,          /* 1 COS queue supported  */
  FD_QOS_COS_MAP_TABLE_INIT_2,          /* 2 COS queues supported */
  FD_QOS_COS_MAP_TABLE_INIT_3,          /* 3 COS queues supported */
  FD_QOS_COS_MAP_TABLE_INIT_4,          /* 4 COS queues supported */
  FD_QOS_COS_MAP_TABLE_INIT_5,          /* 5 COS queues supported */
  FD_QOS_COS_MAP_TABLE_INIT_6,          /* 6 COS queues supported */
  FD_QOS_COS_MAP_TABLE_INIT_7,          /* 7 COS queues supported */
  FD_QOS_COS_MAP_TABLE_INIT_8           /* 8 COS queues supported */
};

/* local function prototypes */
static L7_BOOL cosConfigIntfMatchesGlobal(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Lookup a specific value in the 802.1p table initializer matrix
*
* @param    numTrafficClasses @b{(input)}  Number of traffic classes supported
* @param    precLevel         @b{(input)}  User precedence level
* @param    *pVal             @b{(output)} Ptr to output value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosMapTableInitLookup(L7_uint32 numTrafficClasses, L7_uint32 precLevel,
                              L7_uint32 *pVal)
{
  if ((numTrafficClasses == 0) ||
      (numTrafficClasses > FD_QOS_COS_MAX_QUEUES_PER_INTF))
    return L7_FAILURE;

  if (precLevel >= L7_QOS_COS_MAP_NUM_IPPREC)
    return L7_FAILURE;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  *pVal = cosMapTableInit[numTrafficClasses-1][precLevel];
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build default COS config data
*
* @param    ver         @b{(input)} Config data version number
*
* @returns  void
*
* @comments Builds the config file contents, but does not apply
*           it to the device.
*
* @comments This function is used during component initialization (phase 3)
*           as well as when restoring defaults (clear config).
*
* @end
*********************************************************************/
void cosBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32               cfgIndex;
  nimConfigID_t           configIdSave[L7_COS_INTF_MAX_COUNT];
  L7_cosCfgGlobalParms_t  *pCfgGlob;
  nimConfigID_t           configIdNull;

  memset(&configIdNull, 0, sizeof(configIdNull));
  memset(configIdSave, 0, sizeof(configIdSave));

  /* save all existing configId values before clearing the entire cfg struct */
  for (cfgIndex = 1; cfgIndex < L7_COS_INTF_MAX_COUNT; cfgIndex++)
  {
    NIM_CONFIG_ID_COPY(&configIdSave[cfgIndex],
                       &pCosCfgData_g->cosIntf[cfgIndex].configId);
  }

  memset(pCosCfgData_g, 0, L7_COS_CFG_DATA_SIZE);

  /* build config file header */
  strcpy((char *)pCosCfgData_g->cfgHdr.filename, L7_COS_CFG_FILENAME);
  pCosCfgData_g->cfgHdr.version       = ver;
  pCosCfgData_g->cfgHdr.componentID   = L7_FLEX_QOS_COS_COMPONENT_ID;
  pCosCfgData_g->cfgHdr.type          = L7_CFG_DATA;
  pCosCfgData_g->cfgHdr.length        = (L7_uint32)L7_COS_CFG_DATA_SIZE;
  pCosCfgData_g->cfgHdr.dataChanged   = L7_FALSE;

  /* build general config data */
  pCosCfgData_g->msgLvl = FD_QOS_COS_MSG_LVL;

  /* build global config defaults */
  pCfgGlob = &pCosCfgData_g->cosGlobal;
  pCfgGlob->cfg.intf.intfShapingRate = FD_QOS_COS_QCFG_INTF_SHAPING_RATE;

  cosDefaultMappingConfigBuild(L7_ALL_INTERFACES, &pCfgGlob->cfg.mapping);
  cosDefaultIntfConfigBuild(&pCfgGlob->cfg.intf);
  cosDefaultQueueConfigAllBuild(&pCfgGlob->cfg.queue[0]);

  /* build interface default config, using previously saved configId values
   *
   * NOTE:  Do not build defaults for interfaces that have not been created
   *        yet (i.e. configId is null).  This will happen later, during
   *        the L7_CREATE interface change callback processing.
   */
  for (cfgIndex = 1; cfgIndex < L7_COS_INTF_MAX_COUNT; cfgIndex++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configIdSave[cfgIndex], &configIdNull) != L7_TRUE)
    {
      cosBuildDefaultIntfConfigData(&configIdSave[cfgIndex],
                                    &pCosCfgData_g->cosIntf[cfgIndex]);
    }
  }
}

/*********************************************************************
* @purpose  Build default COS per-interface config data
*
* @param    pConfigId   @b{(input)}  Ptr to NIM config ID for the interface
* @param    pCfgIntf    @b{(input)}  Ptr to interface config structure
*
* @returns  void
*
* @comments There may be times when the caller wants to preserve the
*           configId by passing (&pCfg->configId) as the configId parm.
*           This is used after clearing out the interface config structure
*           to ensure the configId value is not destroyed.
*
* @end
*********************************************************************/
void cosBuildDefaultIntfConfigData(nimConfigID_t *pConfigId,
                                   L7_cosCfgIntfParms_t *pCfgIntf)
{
  if ((pConfigId == L7_NULLPTR) || (pCfgIntf == L7_NULLPTR))
    return;

  memset(pCfgIntf, 0, sizeof(*pCfgIntf));
  NIM_CONFIG_ID_COPY(&pCfgIntf->configId, pConfigId);

  /* build per-interface config defaults
   *
   * NOTE:  Instead of calling the default build functions here, copy
   *        the current global config data to establish the interface
   *        defaults.  After initialization, changes to global config
   *        are applied to all existing interfaces.  When a new interface
   *        is created, this function will establish the same values
   *        as all other interfaces (at least those that do not have
   *        per-interface config overrides).
   */
  memcpy(&pCfgIntf->cfg, &pCosCfgData_g->cosGlobal.cfg, sizeof(pCfgIntf->cfg));
}

/*********************************************************************
* @purpose  Build default COS mapping table config data
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    pCfgMap     @b{(input)}  Ptr to mapping config data
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cosDefaultMappingConfigBuild(L7_uint32 intIfNum, L7_cosMapCfg_t *pCfgMap)
{
  L7_uint32         i, trafficClass;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (pCfgMap == L7_NULLPTR)
    return;

  memset(pCfgMap, 0, sizeof(*pCfgMap));

  /* build default IP Precedence mapping table */
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPPREC; i++)
  {
    if (cosDefaultMappingIpPrecGet(intIfNum, i, &trafficClass) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
              "%s: Cannot obtain default traffic class mapping for "
              "IP Prec %u, intf %u, %s\n", __FUNCTION__, i, intIfNum, ifName);
      return;
    }
    pCfgMap->ipPrecMapTable[i] = trafficClass;
  }

  /* build default IP DSCP mapping table
   *
   * NOTE:  Uses a fixed 8:1 ratio vs. IP Precedence table init, meaning DSCP
   *        values 00-07 map to same queue as IP Precedence 0, DSCP 08-15 to
   *        same queue as IP Precedence 1, etc.
   */
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPDSCP; i++)
  {
    if (cosDefaultMappingIpDscpGet(intIfNum, i, &trafficClass) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
              "%s: Cannot obtain default traffic class mapping for "
              "IP DSCP %u, intf %u, %s\n", __FUNCTION__, i, intIfNum, ifName);
      return;
    }
    pCfgMap->ipDscpMapTable[i] = trafficClass;
  }

  /* set default interface trust mode */
  pCfgMap->intfTrustMode = FD_QOS_COS_MAP_INTF_TRUST_MODE;
}

/*********************************************************************
* @purpose  Get default traffic class mapping for specified IP precedence value
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    prec        @b{(input)}  IP precedence
* @param    *pVal       @b{(output)} Ptr to traffic class output value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The default class mapping is independent of the number of
*           traffic classes supported in the system.
*
* @end
*********************************************************************/
L7_RC_t cosDefaultMappingIpPrecGet(L7_uint32 intIfNum, L7_uint32 prec,
                                   L7_uint32 *pVal)
{
  if (intIfNum == 0)
    return L7_FAILURE;

  if (prec >= L7_QOS_COS_MAP_NUM_IPPREC)
    return L7_FAILURE;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  /* NOTE:  The same defaults are used for global and per-intf config */
  *pVal = cosMapTableInit[L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT-1][prec];

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get default traffic class mapping for specified IP DSCP value
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    dscp        @b{(input)}  IP DSCP
* @param    *pVal       @b{(output)} Ptr to traffic class output value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The default class mapping is independent of the number of
*           traffic classes supported in the system.
*
* @end
*********************************************************************/
L7_RC_t cosDefaultMappingIpDscpGet(L7_uint32 intIfNum, L7_uint32 dscp,
                                   L7_uint32 *pVal)
{
  if (intIfNum == 0)
    return L7_FAILURE;

  if (dscp >= L7_QOS_COS_MAP_NUM_IPDSCP)
    return L7_FAILURE;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  /* NOTE:  The same defaults are used for global and per-intf config
   *
   * NOTE:  Uses a fixed 8:1 ratio vs. IP Precedence table init, meaning DSCP
   *        values 00-07 map to same queue as IP Precedence 0, DSCP 08-15 to
   *        same queue as IP Precedence 1, etc.
   */
  *pVal = cosMapTableInit[L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT-1]
                         [dscp/FD_QOS_COS_MAP_TABLE_DSCP_MAPPING_RATIO];

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build default COS interface config data
*
* @param    pCfgIf      @b{(input)}  Ptr to interface config data
*
* @returns  void
*
* @comments This is for the interface-level config paramters (as opposed
*           to queue-level), which may be managed globally or per-interface.
*
* @end
*********************************************************************/
void cosDefaultIntfConfigBuild(L7_cosIntfCfg_t *pCfgIntf)
{
  if (pCfgIntf == L7_NULLPTR)
    return;


  memset(pCfgIntf, 0, sizeof(*pCfgIntf));

  /* NOTE:  Don't worry about feature present checking when setting defaults */
  pCfgIntf->intfShapingRate = FD_QOS_COS_QCFG_INTF_SHAPING_RATE;
  pCfgIntf->queueMgmtTypePerIntf = (L7_uchar8)FD_QOS_COS_QCFG_MGMT_TYPE;
  pCfgIntf->wredDecayExponent = FD_QOS_COS_QCFG_WRED_DECAY_EXP;
}

/*********************************************************************
* @purpose  Build default config data for all COS queues
*
* @param    pCfgQ       @b{(input)}  Ptr to queue config data
*
* @returns  void
*
* @comments Builds defaults for all queues, either globally or per-interface.
*
* @end
*********************************************************************/
void cosDefaultQueueConfigAllBuild(L7_cosQueueCfg_t *pCfgQ)
{
  L7_uint32     i;

  for (i = 0; i < L7_COS_INTF_QUEUE_MAX_COUNT; i++, pCfgQ++)
  {
    cosDefaultQueueConfigBuild(pCfgQ, i);
  }
}

/*********************************************************************
* @purpose  Build default COS queue config data for a single queue
*
* @param    pCfgQ       @b{(input)}  Ptr to queue config data
* @param    queueId     @b{(input)}  Queue id
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cosDefaultQueueConfigBuild(L7_cosQueueCfg_t *pCfgQ, L7_uint32 queueId)
{
  L7_uint32     i;
  L7_uchar8     tdropThreshBase;
  L7_uchar8     wredMinThreshBase;
  L7_uchar8     wredMaxThreshBase;

  if (pCfgQ == L7_NULLPTR)
    return;

  if ((queueId < L7_QOS_COS_QUEUE_ID_MIN) ||
      (queueId > L7_QOS_COS_QUEUE_ID_MAX))
    return;

  memset(pCfgQ, 0, sizeof(*pCfgQ));

  pCfgQ->queueId = queueId;

  pCfgQ->minBwPercent = FD_QOS_COS_QCFG_MIN_BANDWIDTH;
  pCfgQ->maxBwPercent = FD_QOS_COS_QCFG_MAX_BANDWIDTH;
  pCfgQ->schedulerType = (L7_uchar8)FD_QOS_COS_QCFG_SCHED_TYPE;
  pCfgQ->wrr_weight    = queueId+1;                                       /* PTin added: QoS */
  pCfgQ->queueMgmtType = (L7_uchar8)FD_QOS_COS_QCFG_MGMT_TYPE;
  pCfgQ->wred_decayExponent = (L7_uchar8) FD_QOS_COS_QCFG_WRED_DECAY_EXP; /* PTin added: QoS */

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_SCHED_STRICT_ONLY_FEATURE_ID) == L7_TRUE)
  {
      pCfgQ->schedulerType = L7_QOS_COS_QUEUE_SCHED_TYPE_STRICT;
  }

  /* init defaults for supported number of drop precedence levels */

  for (i = 0; i < (L7_COS_INTF_DROP_PREC_MAX_COUNT+1); i++)
  {
    switch (i)
    {
    case 0:
      tdropThreshBase   = FD_QOS_COS_QCFG_TDROP_THRESH_Q0_DP1;
      wredMinThreshBase = FD_QOS_COS_QCFG_WRED_MIN_THRESH_Q0_DP1;
      wredMaxThreshBase = FD_QOS_COS_QCFG_WRED_MAX_THRESH_Q0_DP1;
      break;
    case 1:
      tdropThreshBase   = FD_QOS_COS_QCFG_TDROP_THRESH_Q0_DP2;
      wredMinThreshBase = FD_QOS_COS_QCFG_WRED_MIN_THRESH_Q0_DP2;
      wredMaxThreshBase = FD_QOS_COS_QCFG_WRED_MAX_THRESH_Q0_DP2;
      break;
    case 2:
      tdropThreshBase   = FD_QOS_COS_QCFG_TDROP_THRESH_Q0_DP3;
      wredMinThreshBase = FD_QOS_COS_QCFG_WRED_MIN_THRESH_Q0_DP3;
      wredMaxThreshBase = FD_QOS_COS_QCFG_WRED_MAX_THRESH_Q0_DP3;
      break;
    case 3:
      tdropThreshBase   = FD_QOS_COS_QCFG_TDROP_THRESH_Q0_NONTCP;
      wredMinThreshBase = FD_QOS_COS_QCFG_WRED_MIN_THRESH_Q0_NONTCP;
      wredMaxThreshBase = FD_QOS_COS_QCFG_WRED_MAX_THRESH_Q0_NONTCP;
      break;
    default:
      /* set all base values to 0 for unknown drop precedence levels */
      tdropThreshBase = wredMinThreshBase = wredMaxThreshBase = 0;
      break;
    } /* endswitch */

    pCfgQ->dropPrec[i].tdropThresh = tdropThreshBase;
    pCfgQ->dropPrec[i].wredMinThresh = wredMinThreshBase;
    pCfgQ->dropPrec[i].wredMaxThresh = wredMaxThreshBase;
    pCfgQ->dropPrec[i].wredDropProb = FD_QOS_COS_QCFG_WRED_DROP_PROB;

  } /* endfor i */
}

/*********************************************************************
* @purpose  Reset COS per-interface config data
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    pConfigId   @b{(input)}  Ptr to NIM config ID for the interface
* @param    pCfgIntf    @b{(input)}  Ptr to interface config structure
*
* @returns  void
*
* @comments There may be times when the caller wants to preserve the
*           configId by passing it as the pConfigId parm.
*           This is used after clearing out the interface config structure
*           to ensure the configId value is not destroyed.
*
* @comments This function differs from cosBuildDefaultIntfConfigData in
*           that it clears each interface config field to its factory
*           default regardless of the current global config settings.
*           This is needed for certain occasions where the interface
*           config must be applied to known defaults (such as when
*           'detached').
*
* @end
*********************************************************************/
void cosResetDefaultIntfConfigData(L7_uint32 intIfNum,
                                   nimConfigID_t *pConfigId,
                                   L7_cosCfgIntfParms_t *pCfgIntf)
{
  /* Check general intIfNum boundary conditions */
  if ((intIfNum == 0) || (intIfNum >= platIntfMaxCountGet()))
    return;

  if ((pConfigId == L7_NULLPTR) || (pCfgIntf == L7_NULLPTR))
    return;

  memset(pCfgIntf, 0, sizeof(*pCfgIntf));
  NIM_CONFIG_ID_COPY(&pCfgIntf->configId, pConfigId);

  /* build per-interface config defaults */
  cosDefaultMappingConfigBuild(intIfNum, &pCfgIntf->cfg.mapping);
  cosDefaultIntfConfigBuild(&pCfgIntf->cfg.intf);
  cosDefaultQueueConfigAllBuild(&pCfgIntf->cfg.queue[0]);
}

/*********************************************************************
* @purpose  Applies COS component config data
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The saved global config that shadows the interface config
*           parms need not be explicitly applied, since the interface
*           config contains the most recent user configuration, whether
*           it was specified globally or on a per-interface basis.
*
* @end
*********************************************************************/
L7_RC_t cosConfigDataApply(void)
{
  L7_uint32               intIfNum;
  L7_uint32               cfgIndex;
  L7_cosCfgGlobalParms_t  *pCfgGlob;
  L7_cosCfgIntfParms_t    *pCfgIntf;
  nimConfigID_t           configIdNull;
  L7_uint32               i, num;

  if ((pCosInfo_g == L7_NULLPTR) || (pCosCfgData_g == L7_NULLPTR))
    return L7_FAILURE;

  /* Carefully apply the configured msgLvl to the operational msgLvl.
   * In order to facilitate debugging across a 'clear config', only
   * overwrite the operational msgLvl if it is currently set to zero.
   * This allows a current msgLvl setting to remain in effect during
   * a clear config.
   *
   * (This also means the msgLvl must be manually turned off if debugging
   * output is not desired.)
   */
  if (pCosInfo_g->msgLvl == 0)
    pCosInfo_g->msgLvl = pCosCfgData_g->msgLvl;

  /* Get the latest operational number of traffic classes from the
   * dot1p component. Ignore a 0 value (means dot1p not fully set up
   * yet for the interface -- dot1p value will be processed later via
   * outcall mechanism.)
   *
   * NOTE:  These may have been previously indicated by the dot1p component,
   *        but reading the values here to ensure they are updated before
   *        doing all the COS apply work.
   */
  num = dot1dPortNumTrafficClassesGet(L7_ALL_INTERFACES);
  if (num != 0)
    pCosInfo_g->globalNumTrafficClasses = num;
  for (i = 1; i < L7_MAX_INTERFACE_COUNT; i++)
  {
    num = dot1dPortNumTrafficClassesGet(i);
    if (num != 0)
      pCosInfo_g->numTrafficClasses[i] = num;
  }

  /* Make sure global COS mapping tables conform to operational number of
   * traffic classes, reverting to default settings if in error.  The
   * global mapping tables will be used during the interface apply function
   * clean up any interface COS mapping tables that are invalid.
   *
   * NOTE:  It is possible that a previously saved config was based
   *        on a larger number of supported traffic classes. While not
   *        common, such a case could occur when changing between
   *        a stacking and non-stacking build on a given platform, e.g.
   */
  pCfgGlob = &pCosCfgData_g->cosGlobal;
  if (cosMapTableContentIsValid(L7_ALL_INTERFACES, &pCfgGlob->cfg.mapping)
      != L7_TRUE)
  {
    cosDefaultMappingConfigBuild(L7_ALL_INTERFACES, &pCfgGlob->cfg.mapping);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
            "%s: Global COS mapping table config is no longer valid, "
            "using defaults instead.\n", __FUNCTION__);
  }

  /* NOTE:  The only defined COS global data is that which shadows the
   *        interface config, so no need to 'apply' any COS global data
   *        to the device.
   */

  /* no point checking each interface if the component is not in ready state */
  if (COS_IS_READY != L7_TRUE)
    return L7_SUCCESS;

  /* apply configuration for each known interface */

  memset(&configIdNull, 0, sizeof(configIdNull));

  for (cfgIndex = 1; cfgIndex < L7_COS_INTF_MAX_COUNT; cfgIndex++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&pCosCfgData_g->cosIntf[cfgIndex].configId,
                               &configIdNull))
      continue;

    if (nimIntIfFromConfigIDGet(&pCosCfgData_g->cosIntf[cfgIndex].configId,
                                &intIfNum) != L7_SUCCESS)
      continue;

    if (cosIntfIsConfigurable(intIfNum, &pCfgIntf) == L7_TRUE)
    {
      if (cosConfigIntfDataApply(intIfNum, pCfgIntf) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
                "%s: Failed applying config data to interface %s\n",
                __FUNCTION__, ifName);
        return L7_FAILURE;
      }
    }

  } /* endfor cfgIndex */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply COS component interface config data
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    *pCfgIntf   @b{(input)}  Ptr to interface config parms
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes caller provides a ptr to the interface config parms
*           and has checked that the interface is configurable.  This
*           allows the function to be used when applying temporary config
*           data to the interface (e.g. default config when port acquire
*           occurs).
*
* @comments This function is also used when L7_ATTACH or L7_DETACH event
*           received for an interface.
*
* @end
*********************************************************************/
L7_RC_t cosConfigIntfDataApply(L7_uint32 intIfNum, L7_cosCfgIntfParms_t *pCfgIntf)
{
  L7_cosCfgParms_t  *pCfg;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if ((pCfgIntf == L7_NULLPTR) || (pCosInfo_g == L7_NULLPTR))
    return L7_FAILURE;

  /* Check general intIfNum boundary conditions */
  if ((intIfNum == 0) || (intIfNum >= platIntfMaxCountGet()))
    return L7_FAILURE;

  pCfg = &pCfgIntf->cfg;

  /* apply current COS mapping table config */
  if (cosConfigIntfMapTableDataApply(intIfNum, pCfg, L7_FALSE) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
            "%s: Failed applying COS map table config data to interface %s \n",
            __FUNCTION__, ifName);
    return L7_FAILURE;
  }

  /* apply current COS queue config parms */
  if (cosConfigIntfQueueCfgDataApply(intIfNum, pCfg) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
            "%s: Failed applying COS queue config data to interface %s \n",
            __FUNCTION__, ifName);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply COS component interface mapping table config data
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    *pCfg       @b{(input)}  Ptr to COS config parms
* @param    forceDtl    @b{(input)}  Force DTL call even if in trust-dot1p mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes caller provides a ptr to the COS config parms
*           and has checked that the interface is configurable.
*
* @end
*********************************************************************/
L7_RC_t cosConfigIntfMapTableDataApply(L7_uint32 intIfNum, L7_cosCfgParms_t *pCfg,
                                       L7_BOOL forceDtl)
{
  if (pCfg == L7_NULLPTR)
    return L7_FAILURE;

  /* Make sure interface COS mapping tables conform to operational number of
   * traffic classes, reverting to global table default settings if in error.
   *
   * NOTE:  It is possible that a previously saved config was based
   *        on a larger number of supported traffic classes.
   */
  if (cosMapTableContentIsValid(intIfNum, &pCfg->mapping) != L7_TRUE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    cosDefaultMappingConfigBuild(intIfNum, &pCfg->mapping);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
            "%s: Intf %s COS map table config is no longer valid, "
            "using global defaults instead.\n", __FUNCTION__, ifName);
  }

  /* doing an extra check here to expedite the process if the config cannot
   * be applied to the interface (this would have been discovered later in
   * each of the individual apply functions)
   */
  if (cosIntfIsSettable(intIfNum) != L7_TRUE)
    return L7_SUCCESS;

  /* apply current intf trust mode and corresponding mapping table */
  if (cosMapIntfTrustModeApply(intIfNum, pCfg, forceDtl) != L7_SUCCESS)
  {
    COS_PRT(COS_MSGLVL_HI,
            "\nError applying COS intf trust mode on intf %u\n",
            intIfNum);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply COS component interface queue config data
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    *pCfg       @b{(input)}  Ptr to COS config parms
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes caller provides a ptr to the interface config parms
*           and has checked that the interface is configurable.
*
* @end
*********************************************************************/
L7_RC_t cosConfigIntfQueueCfgDataApply(L7_uint32 intIfNum, L7_cosCfgParms_t *pCfg)
{
  L7_cosQueueCfg_t              *pQ;
  L7_cosDropPrecCfg_t           *pDP;
  L7_uint32                     queueId;
  L7_uint32                     i;
  L7_cosQueueSchedParms_t       qParms;
  L7_qosCosDropParmsList_t      dropList;
  L7_BOOL                       wRedSupport;

  if (pCfg == L7_NULLPTR)
    return L7_FAILURE;

  /* doing an extra check here to expedite the process if the config cannot
   * be applied to the interface (this would have been discovered later in
   * each of the individual apply functions, but only after doing all the
   * setup work)
   */
  if (cosIntfIsSettable(intIfNum) != L7_TRUE)
    return L7_SUCCESS;

  wRedSupport = cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID);

  /* apply configuration for each queue */
  for (queueId = 0; queueId < L7_MAX_CFG_QUEUES_PER_PORT; queueId++)
  {
    pQ = &pCfg->queue[queueId];

    /* construct queue config parm lists */
    qParms.minBwList.bandwidth[queueId] = pQ->minBwPercent;
    qParms.maxBwList.bandwidth[queueId] = pQ->maxBwPercent;
    qParms.schedTypeList.schedType[queueId] =
      (L7_QOS_COS_QUEUE_SCHED_TYPE_t)pQ->schedulerType;
    qParms.wrr_weights.queue_weight[queueId] = pQ->wrr_weight;            /* PTin added: QoS */

    dropList.queue[queueId].mgmtType = pQ->queueMgmtType;
    dropList.queue[queueId].wred_decayExponent = pQ->wred_decayExponent;  /* PTin added: QoS */
    /* construct tail drop and WRED parms lists */
    for (i = 0; i < (L7_MAX_CFG_DROP_PREC_LEVELS+1); i++)
    {
      pDP = &pQ->dropPrec[i];

      dropList.queue[queueId].minThreshold[i] = pDP->wredMinThresh;
      dropList.queue[queueId].tailDropMaxThreshold[i] = pDP->tdropThresh;
      dropList.queue[queueId].wredMaxThreshold[i] = pDP->wredMaxThresh;
      dropList.queue[queueId].dropProb[i] = pDP->wredDropProb;
      if (pQ->queueMgmtType == L7_QOS_COS_QUEUE_MGMT_TYPE_WRED)
      {
    if (wRedSupport == L7_FALSE)
        {
      return L7_FAILURE;
    }
      }
    } /* endfor drop-prec */
  } /* endfor queueId */

  /* apply queue config parms for this interface */
  if (cosQueueSchedConfigApply(intIfNum, &qParms) != L7_SUCCESS)
  {
    COS_PRT(COS_MSGLVL_HI,
            "\nError applying COS queue config parms on intf %u\n",
            intIfNum);
    return L7_FAILURE;
  }

  if (cosQueueDropParmsApply(intIfNum, &dropList) != L7_SUCCESS)
  {
    COS_PRT(COS_MSGLVL_HI,
        "\nError applying COS intf config drop parms on intf %u\n",
        intIfNum);
    return L7_FAILURE;
  }
  /* apply interface config parms
   *
   * NOTE:  Do this last so that platforms supporting per-interface queue
   *        management config get this command after the individual
   *        tail drop or WRED commands are issued per queue.
   */
  if (cosQueueIntfConfigApply(intIfNum,
                              pCfg->intf.intfShapingRate,
                              pCfg->intf.queueMgmtTypePerIntf,
                              pCfg->intf.wredDecayExponent)
      != L7_SUCCESS)
  {
    COS_PRT(COS_MSGLVL_HI,
            "\nError applying COS intf config parms on intf %u\n",
            intIfNum);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Unapply COS component interface config data
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    *pCfgIntf   @b{(input)}  Ptr to interface config parms
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes caller provides a ptr to the interface config parms
*           and has checked that the interface is configurable.
*
* @comments This function is used when L7_DETACH event received
*           for an interface.
*
* @end
*********************************************************************/
L7_RC_t cosConfigIntfDataUnapply(L7_uint32 intIfNum, L7_cosCfgIntfParms_t *pCfgIntf)
{
  nimConfigID_t         configId;
  L7_cosCfgIntfParms_t  intfCfg;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  /* Check general intIfNum boundary conditions */
  if ((intIfNum == 0) || (intIfNum >= platIntfMaxCountGet()))
    return L7_FAILURE;

  if (pCfgIntf == L7_NULLPTR)
    return L7_FAILURE;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  /* NOTE:  When an interface is detached, it is important to set the
   *        device back to a known "reset" state regarding the COS
   *        component config parms.  This is somewhat different from
   *        building default config, which gets the latest global config
   *        and applies it to an interface.
   *
   *        Using a local interface config structure here so that the
   *        real config data for this interface is not altered (ignore
   *        the pCfgIntf config that was passed into this function).
   */
  cosResetDefaultIntfConfigData(intIfNum, &configId, &intfCfg);

  /* conditionally apply default COS mapping table config if different
   * from current config
   */
  if (memcmp(&pCfgIntf->cfg.mapping, &intfCfg.cfg.mapping, sizeof(intfCfg.cfg.mapping)) != 0)
  {
    /* if the previous and current trust modes differ, or if they are the same but
     * not trust-dot1p mode, apply the default config data (forcing the DTL call
     * via the flag parm)
     */
    if ((pCfgIntf->cfg.mapping.intfTrustMode != intfCfg.cfg.mapping.intfTrustMode) ||
        (pCfgIntf->cfg.mapping.intfTrustMode != L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P))
    {
      if (cosConfigIntfMapTableDataApply(intIfNum, &intfCfg.cfg, L7_TRUE) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
                "%s: Failed to apply default COS map table config to interface %s \n",
                __FUNCTION__, ifName);
        return L7_FAILURE;
      }
    }
  }

  /* conditionally apply default COS queue config parms if different from
   * current config
   */
  if ( (memcmp(&pCfgIntf->cfg.intf, &intfCfg.cfg.intf, sizeof(intfCfg.cfg.intf)) != 0) ||
       (memcmp(pCfgIntf->cfg.queue, intfCfg.cfg.queue, sizeof(intfCfg.cfg.queue)) != 0) )
  {
    if (cosConfigIntfQueueCfgDataApply(intIfNum, &intfCfg.cfg) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
              "%s: Failed to apply default COS queue config to interface %s \n",
              __FUNCTION__, ifName);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Save COS component user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosSave(void)
{
  if (cosDeregister_g.cosSave == L7_TRUE)
    return L7_SUCCESS;

  /* only save file if something has actually changed */
  if (pCosCfgData_g->cfgHdr.dataChanged == L7_FALSE)
    return L7_SUCCESS;

  /* copy the operational msgLvl to the saved config msgLvl to be used
   * on a subsequent reboot
   */
  pCosCfgData_g->msgLvl = pCosInfo_g->msgLvl;

  pCosCfgData_g->cfgHdr.dataChanged = L7_FALSE;
  pCosCfgData_g->checkSum =
    nvStoreCrc32((L7_uchar8*)pCosCfgData_g,
                 (L7_uint32)(sizeof(L7_cosCfgData_t) - sizeof(pCosCfgData_g->checkSum)));

  if (sysapiCfgFileWrite(L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_CFG_FILENAME,
                        (L7_char8 *)pCosCfgData_g,
                        (L7_uint32)sizeof(L7_cosCfgData_t)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
            "COS: Error writing config file %s\n", L7_COS_CFG_FILENAME);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Restore COS component user configuration to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosRestore(void)
{
  L7_RC_t       rc = L7_SUCCESS;

  if (cosDeregister_g.cosRestore == L7_TRUE)
    return L7_SUCCESS;

  cosBuildDefaultConfigData(pCosCfgData_g->cfgHdr.version);
  pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
  if (cosConfigDataApply() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
            "%s: Failed to restore COS factory defaults\n", __FUNCTION__);
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Check if COS component user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL cosHasDataChanged(void)
{
  if (cosDeregister_g.cosHasDataChanged == L7_TRUE)
    return L7_FALSE;

  return pCosCfgData_g->cfgHdr.dataChanged;
}
void cosResetDataChanged(void)
{
  pCosCfgData_g->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Obtain a pointer to the configuration data for the specified
*           interface
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    **ppCfgIntf @b{(output)} Ptr to COS interface config struct ptr
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @comments The caller can set the pCfgIntf parm to L7_NULLPTR if it does not
*           want the output value from this function.
*
* @end
*********************************************************************/
L7_BOOL cosIntfIsConfigurable(L7_uint32 intIfNum, L7_cosCfgIntfParms_t **ppCfgIntf)
{
  L7_uint32     index;
  nimConfigID_t configId;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (COS_IS_READY != L7_TRUE)
    return L7_FALSE;

  /* Check general intIfNum boundary conditions */
  if ((intIfNum == 0) || (intIfNum >= platIntfMaxCountGet()))
    return L7_FALSE;

  index = cosMapTbl_g[intIfNum];

  if (index == 0)
    return L7_FALSE;

  /* verify that the configId in the config data table entry matches the
   * NIM configId for the specified intIfNum
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &pCosCfgData_g->cosIntf[index].configId)
        == L7_FALSE)
    {
      /* there is either a table mismatch between pCosCfgData_g and cosMapTbl_g,
       * or there is synchronization issue between NIM and components w.r.t.
       * interface creation/deletion
       */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
              "cosIntfIsConfigurable: Error accessing COS config data for "
              "interface %s\n", ifName);
      return L7_FALSE;
    }
  }

  if (ppCfgIntf != L7_NULLPTR)
    *ppCfgIntf = &pCosCfgData_g->cosIntf[index];

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Determine if interface is in a state for config to be applied
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Returns L7_TRUE if the interface state is either
*           L7_ATTACHED, L7_ATTACHING, or L7_DETACHING.  This
*           is a pre-condition for being able to apply config
*           data to the device (i.e. make DTL calls).
*
* @end
*********************************************************************/
L7_BOOL cosIntfIsSettable(L7_uint32 intIfNum)
{
  L7_NIM_QUERY_DATA_t queryData;

  /* do some preliminary checking so NIM doesn't log a msg for improper usage */
  if (COS_IS_READY != L7_TRUE)
    return L7_FALSE;

  /* Check general intIfNum boundary conditions */
  if ((intIfNum == 0) || (intIfNum >= platIntfMaxCountGet()))
    return L7_FALSE;

  memset(&queryData, 0, sizeof(queryData));
  queryData.intIfNum = intIfNum;
  queryData.request = L7_NIM_QRY_RQST_STATE;

  if (nimIntfQuery(&queryData) == L7_SUCCESS)
  {
    if ((queryData.data.state == L7_INTF_ATTACHED) ||
        (queryData.data.state == L7_INTF_ATTACHING) ||
        (queryData.data.state == L7_INTF_DETACHING))
    {
      return L7_TRUE;
    }
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine if interface should have config written to it
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    featureId   @b{(input)}  COS feature id to be checked
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments This is needed to handle the case where only global
*           config is supported for a particular feature.
*           This function determines if the specified interface
*           is the first interface for the management unit,
*           which is the only one to which config should be applied
*           for this case.
*
* @comments Assumes caller supplies a meaningful featureId to be checked.
*           This would typically be one of the "CFG_PER_INTF" features,
*           but any value is permitted here.
*
* @end
*********************************************************************/
L7_BOOL cosIntfIsWriteable(L7_uint32 intIfNum, L7_uint32 featureId)
{
  L7_uint32     firstIntf;

  /* make sure interface is settable (does some intIfNum validation that
   * need not be repeated here)
   */
  if (cosIntfIsSettable(intIfNum) != L7_TRUE)
    return L7_FALSE;

  /* if the specified feature is supported, then assume the config is allowed
   * to be written for this interface
   */
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, featureId) == L7_TRUE)
    return L7_TRUE;

  /* NOTE:  The specified feature is not supported.  This featureId is
   *        intended to be one of the "configurable per interface" IDs,
   *        so it is assumed that only the first interface of the management
   *        unit is allowed to be written to indicate a global config
   *        change to the device.
   */

  if (cosIntfMgmtUnitFirstFind(&firstIntf) == L7_SUCCESS)
  {
    /* only return 'true' if intIfNum matches first interface found
     * on management unit
     */
    if (intIfNum == firstIntf)
      return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine if interface should have config written to it
*           as conditioned by a mode flag
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    globalOnly  @b{(input)}  Checking mode
* @param    featureId   @b{(input)}  COS feature id to be checked
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments If globalOnly is L7_TRUE, handle the case where global-only
*           (not per-intf) config is supported for a particular feature.
*           This function determines if the specified interface
*           is the first interface for the management unit,
*           which is the only one to which config should be applied
*           for this case.
*
* @comments If globalOnly is L7_FALSE, the function only checks if the
*           indicated featureId is supported or not.  This is used by
*           callers trying to determine if config can be written to an
*           interface when the per-intf feature is supported.
*
* @comments Assumes caller supplies a meaningful featureId to be checked.
*           This would typically be one of the "_PER_INTF" feature IDs.
*
* @end
*********************************************************************/
L7_BOOL cosIntfIsWriteableCond(L7_uint32 intIfNum, L7_BOOL globalOnly, L7_uint32 featureId)
{
  L7_BOOL       rc;
  L7_uint32     firstIntf;

  /* make sure interface is settable (does some intIfNum validation that
   * need not be repeated here)
   */
  if (cosIntfIsSettable(intIfNum) != L7_TRUE)
    return L7_FALSE;

  rc = cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, featureId);

  if (globalOnly == L7_FALSE)
  {
    /* this query is for per-intf feature support, so simply return the
     * result of the feature check
     */
    return rc;
  }

  /* reaching this point means the query is for global-only feature support,
   * not per-intf feature support
   */

  if (rc == L7_TRUE)
  {
    /* the specified per-intf feature is supported, so assume the config
     * is NOT allowed to be written globally
     */
    return L7_FALSE;
  }

  /* NOTE:  The specified per-intf feature is not supported.  This featureId is
   *        intended to be one of the "configurable per interface" IDs,
   *        so it is assumed that only the first interface of the management
   *        unit is allowed to be written to indicate a global config
   *        change to the device.
   */

  if (cosIntfMgmtUnitFirstFind(&firstIntf) == L7_SUCCESS)
  {
    /* only return 'true' if intIfNum matches first interface found
     * on management unit
     */
    if (intIfNum == firstIntf)
      return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Find first valid interface on management unit
*
* @param    *pIntIfNum  @b{(output)} Internal interface number
*
* @returns  L7_SUCCESS  First mgmt unit interface found
* @returns  L7_FAILURE  First mgmt unit interface not found
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosIntfMgmtUnitFirstFind(L7_uint32 *pIntIfNum)
{
  L7_uint32     mgmtUnit;
  nimUSP_t      nimUsp;
  L7_uint32     i, maxIntf;

  if (pIntIfNum == L7_NULLPTR)
    return L7_FAILURE;

  *pIntIfNum = 0;

  if (usmDbUnitMgrMgrNumberGet(&mgmtUnit) != L7_SUCCESS)
    return L7_FAILURE;

  maxIntf = platIntfMaxCountGet();

  for (i = 1; i < maxIntf; i++)
  {
    if (nimGetUnitSlotPort(i, &nimUsp) == L7_SUCCESS)
    {
      if (nimUsp.unit == (L7_uchar8)mgmtUnit)
      {
        if (cosMapTbl_g[i] != 0)
        {
          *pIntIfNum = i;
          return L7_SUCCESS;            /* found it */
        }
      }
    }
  } /* endfor */

  return L7_FAILURE;                    /* not found */
}

/*********************************************************************
* @purpose  Obtain a pointer to the first available interface config structure
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    **ppCfgIntf @b{(output)} Ptr to COS interface config struct ptr,
*                                      or L7_NULLPTR if output not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured.
*
* @end
*********************************************************************/
L7_BOOL cosIntfConfigEntryGet(L7_uint32 intIfNum, L7_cosCfgIntfParms_t **ppCfgIntf)
{
  L7_uint32     i;
  nimConfigID_t configIdNull, configId;
  static L7_uint32 nextIndex = 1;

  if ((COS_IS_READY != L7_TRUE) ||
      (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS))
    return L7_FALSE;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  /* Avoid N^2 processing when interfaces created at startup */
  if ((nextIndex < L7_COS_INTF_MAX_COUNT) &&
      (NIM_CONFIG_ID_IS_EQUAL(&pCosCfgData_g->cosIntf[nextIndex].configId, &configIdNull)))
  {
    /* entry corresponding to nextIndex is free so use it, move speculative index to next entry
     * for next time
     */
    i = nextIndex++;
  }
  else
  /* Cached nextIndex is in use. Resort to search from beginning. N^2. */
  {
    for (i = 1; i < L7_COS_INTF_MAX_COUNT; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&pCosCfgData_g->cosIntf[i].configId, &configIdNull))
      {
        /* found a free entry, update the speculative index to next entry for next time */
        nextIndex = i+1;
        break;
      }
    }
  }

  if (i < L7_COS_INTF_MAX_COUNT)
  {
    cosMapTbl_g[intIfNum] = i;
    if (ppCfgIntf != L7_NULLPTR)
      *ppCfgIntf = &pCosCfgData_g->cosIntf[i];
    return L7_TRUE;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Process callback for L7_CREATE interface event
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t         configId;
  L7_uint32             cfgIndex;
  L7_cosCfgIntfParms_t  *pCfgIntf;

  /* only interested in certain interface types for this component
   * (quietly ignoring other types)
   */
  if (cosIntfTypeIsValid(intIfNum) != L7_TRUE)
    return L7_SUCCESS;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  for (cfgIndex = 1; cfgIndex < L7_COS_INTF_MAX_COUNT; cfgIndex++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&pCosCfgData_g->cosIntf[cfgIndex].configId, &configId))
    {
      cosMapTbl_g[intIfNum] = cfgIndex;
      break;
    }
  }

  pCfgIntf = L7_NULLPTR;

  /* if an interface configuration entry is not already assigned to the
   * interface, assign one
   */
  if (cosIntfIsConfigurable(intIfNum, &pCfgIntf) != L7_TRUE)
  {
    if (cosIntfConfigEntryGet(intIfNum, &pCfgIntf) != L7_TRUE)
      return L7_FAILURE;

    /* update the configuration structure with the config id and build
     * the default config
     */
    if (pCfgIntf != L7_NULLPTR)
      cosBuildDefaultIntfConfigData(&configId, pCfgIntf);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process callback for L7_DELETE interface event
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosIntfDelete(L7_uint32 intIfNum)
{
  L7_cosCfgIntfParms_t  *pCfgIntf;

  if (cosIntfIsConfigurable(intIfNum, &pCfgIntf) == L7_TRUE)
  {
    memset(&pCfgIntf->configId, 0, sizeof(nimConfigID_t));
    cosMapTbl_g[intIfNum] = 0;
    pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Callback function to process interface state changes
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    event       @b{(input)}  Interface event
* @param    correlator  @b{(input)}  NIM event correlator, defined by
*                                      L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,
                              NIM_CORRELATOR_t correlator)
{
  L7_RC_t                   rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;
  L7_cosCfgIntfParms_t      *pCfgIntf = L7_NULLPTR;

  status.intIfNum        = intIfNum;
  status.component       = L7_FLEX_QOS_COS_COMPONENT_ID;
  status.event           = event;
  status.correlator      = correlator;
  status.response.rc     = rc;
  status.response.reason = NIM_ERR_RC_UNUSED;

  if (cosDeregister_g.cosIntfChange == L7_TRUE)
  {
    rc = L7_SUCCESS;
  }
  else if (cosIntfTypeIsValid(intIfNum) != L7_TRUE)
  {
    rc = L7_SUCCESS;
  }
  else if (COS_IS_READY == L7_TRUE)
  {
    switch (event)
    {
    case L7_CREATE:
      rc = cosIntfCreate(intIfNum);
      break;

    case L7_ATTACH:
      if (cosIntfIsConfigurable(intIfNum, &pCfgIntf) == L7_TRUE)
        rc = cosConfigIntfDataApply(intIfNum, pCfgIntf);
      break;

    case L7_DETACH:
      if (cosIntfIsConfigurable(intIfNum, &pCfgIntf) == L7_TRUE)
        rc = cosConfigIntfDataUnapply(intIfNum, pCfgIntf);
      break;

    case L7_DELETE:
      rc = cosIntfDelete(intIfNum);
      break;

    case L7_ACTIVE:
      pCosIntfInfo_g[intIfNum].intfIsActive = L7_TRUE;
      break;

    case L7_INACTIVE:
      pCosIntfInfo_g[intIfNum].intfIsActive = L7_FALSE;
      break;

    case L7_LAG_ACQUIRE:
    case L7_PROBE_SETUP:
      /* NOTE:  Per agreement, the HAPI layer is responsible for retaining
       *        all COS interface config data while the port is a
       *        member of a LAG.  The application continues to treat
       *        config changes normally, letting HAPI re-apply it to the
       *        port when it is released from the LAG.  LAG member ports are
       *        controlled by the COS config of the LAG interface itself.
       */
      break;

    case L7_LAG_RELEASE:
    case L7_PROBE_TEARDOWN:
      /* NOTE:  Per agreement, the HAPI layer is responsible for retaining
       *        all COS interface config data while the port is a
       *        member of a LAG.  The application continues to treat
       *        config changes normally, letting HAPI re-apply it to the
       *        port when it is released from the LAG.  LAG member ports are
       *        controlled by the COS config of the LAG interface itself.
       */
      break;

    default:
      /* not interested in this interface event */
      break;

    } /* endswitch */

    if (rc != L7_SUCCESS)
    {
      status.response.reason = NIM_ERR_RC_INTERNAL;
    }
  } /* endelseif COS_IS_READY == L7_TRUE */
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
            "cosIntfChangeCallback called outside permitted phase\n");
    rc = L7_FAILURE;
    status.response.reason = NIM_ERR_RC_OUT_OF_SEQUENCE;
  }

  status.response.rc = rc;
  nimEventStatusCallback(status);

  return rc;
}

/*********************************************************************
* @purpose  Process NIM Startup notifications
*
* @param    NIM_STARTUP_PHASE_t  startupPhase
*
* @notes
*
* @end
*********************************************************************/
void cosStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;
  L7_cosCfgIntfParms_t      *pCfgIntf = L7_NULLPTR;

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    switch (startupPhase)
    {
    case NIM_INTERFACE_CREATE_STARTUP:
      COS_PRT(COS_MSGLVL_LO, "CoS: Create Startup %3u\n", intIfNum);
      (void)cosIntfCreate(intIfNum);
      break;

    case NIM_INTERFACE_ACTIVATE_STARTUP:
      COS_PRT(COS_MSGLVL_LO, "CoS: Activate Startup %3u\n", intIfNum);
      if (cosIntfIsConfigurable(intIfNum, &pCfgIntf) == L7_TRUE)
        rc = cosConfigIntfDataApply(intIfNum, pCfgIntf);
      break;

    default:
      break;
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  switch (startupPhase)
  {
  case NIM_INTERFACE_CREATE_STARTUP:
    /* Register with NIM to receive port CREATE/DELETE events */
    PORTEVENT_SETMASKBIT(cosPortEventMask_g, L7_CREATE);
    PORTEVENT_SETMASKBIT(cosPortEventMask_g, L7_DELETE);
    nimRegisterIntfEvents(L7_FLEX_QOS_COS_COMPONENT_ID, cosPortEventMask_g);
    break;

  case NIM_INTERFACE_ACTIVATE_STARTUP:
    /* Add ATTACH/DETACH/etc. events to our NIM registration */
    PORTEVENT_SETMASKBIT(cosPortEventMask_g, L7_ATTACH);
    PORTEVENT_SETMASKBIT(cosPortEventMask_g, L7_DETACH);
    PORTEVENT_SETMASKBIT(cosPortEventMask_g, L7_ACTIVE);
    PORTEVENT_SETMASKBIT(cosPortEventMask_g, L7_INACTIVE);
    PORTEVENT_SETMASKBIT(cosPortEventMask_g, L7_LAG_ACQUIRE);
    PORTEVENT_SETMASKBIT(cosPortEventMask_g, L7_LAG_RELEASE);
    PORTEVENT_SETMASKBIT(cosPortEventMask_g, L7_PROBE_SETUP);
    PORTEVENT_SETMASKBIT(cosPortEventMask_g, L7_PROBE_TEARDOWN);
    nimRegisterIntfEvents(L7_FLEX_QOS_COS_COMPONENT_ID, cosPortEventMask_g);
    break;

  default:
    break;
  }

  nimStartupEventDone(L7_FLEX_QOS_COS_COMPONENT_ID);

  /* Inform cnfgr that protected port has completed it HW updates */
  if (startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
  {
    cnfgrApiComponentHwUpdateDone(L7_FLEX_QOS_COS_COMPONENT_ID, L7_CNFGR_HW_APPLY_CONFIG);
  }
}

/*********************************************************************
* @purpose  Determines if an interface has been acquired by another
*           component and is therefore unavailable to COS
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_TRUE    interface has been acquired
* @returns  L7_FALSE
*
* @comments The interface is considered not available for COS purposes
*           while currently acquired by any of the following components:
*              - DOT3AD (LAGs)
*              - port mirroring
*
* @end
*********************************************************************/
L7_BOOL cosIsIntfAcquired(L7_uint32 intIfNum)
{
  if ((mirrorIsActiveProbePort(intIfNum)) ||
      (dot3adIsLagActiveMember(intIfNum)))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Display the contents of the COS component config data for
*           the specified interface
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  void
*
* @comments An intIfNum of 0 or L7_ALL_INTERFACES displays the global
*           config data instead of interface config data.
*
* @end
*********************************************************************/
void cosConfigDataShow(L7_uint32 intIfNum)
{
  L7_uint32             msgLvlReqd = COS_MSGLVL_ON; /* always display output */
  L7_cosCfgIntfParms_t  *pCfgIntf;
  nimUSP_t              *pUsp;

  if (intIfNum == 0)
    intIfNum = L7_ALL_INTERFACES;

  /* check for supported interface type */
  if ((intIfNum != L7_ALL_INTERFACES) &&
      (cosIntfTypeIsValid(intIfNum) != L7_TRUE))
  {
    COS_PRT(msgLvlReqd, "\nInvalid interface %u\n\n", intIfNum);
    return;
  }

  /* make sure config data exists */
  if (pCosCfgData_g == L7_NULLPTR)
  {
    COS_PRT(msgLvlReqd, "\nCOS component configuration data does not exist\n\n");
    return;
  }

  /* display header banner */
  if (intIfNum == L7_ALL_INTERFACES)
  {
    COS_PRT(msgLvlReqd, "\nGlobal COS configuration:\n\n");
  }
  else
  {
    COS_PRT(msgLvlReqd, "\nCOS configuration for interface %u\n\n", intIfNum);
    if (cosConfigIntfMatchesGlobal(intIfNum) != L7_TRUE)
    {
      COS_PRT(msgLvlReqd, "  ** intf config differs from global **\n\n");
    }

    if (cosIntfIsConfigurable(intIfNum, &pCfgIntf) == L7_TRUE)
    {
      pUsp = &pCfgIntf->configId.configSpecifier.usp;
      COS_PRT(msgLvlReqd, "  ConfigId info:  intf_type=%u  usp=%u/%u/%u\n\n",
              (L7_uint32)pCfgIntf->configId.type,
              pUsp->unit, pUsp->slot, pUsp->port);
    }
  }

  /* NOTE:  L7_ALL_INTERFACES is used to display global config */
  cosMapTableShow(intIfNum);
  cosQueueConfigShow(intIfNum);
}

/*********************************************************************
* @purpose  Display entire contents of the COS component config data
*
* @param    void
*
* @returns  void
*
* @comments Engineering debug function.
*
* @end
*********************************************************************/
L7_RC_t cosConfigDataShowAll(void)
{
  L7_uint32     msgLvlReqd = COS_MSGLVL_ON;     /* always display output */
  L7_fileHdr_t  *pFileHdr;
  L7_uint32     intIfNum;

  /* make sure config data exists */
  if (pCosCfgData_g == L7_NULLPTR)
  {
    COS_PRT(msgLvlReqd, "\nCOS component configuration data does not exist\n\n");
    return L7_FAILURE;
  }

  /* Config File Header Contents */
  pFileHdr = &pCosCfgData_g->cfgHdr;
  COS_PRT(msgLvlReqd, "\nCOS Config File Header:\n\n");
  COS_PRT(msgLvlReqd,
          "File name......................  %s\n", (char *)pFileHdr->filename);
  COS_PRT(msgLvlReqd,
          "Version........................  %u\n", pFileHdr->version);
  COS_PRT(msgLvlReqd,
          "Component ID...................  %u (%s)\n", pFileHdr->componentID,
          (pFileHdr->componentID == L7_FLEX_QOS_COS_COMPONENT_ID) ? "COS" : "???");
  COS_PRT(msgLvlReqd,
          "Type...........................  %u\n", pFileHdr->type);
  COS_PRT(msgLvlReqd,
          "Length.........................  %u\n", pFileHdr->length);
  COS_PRT(msgLvlReqd,
          "Data Changed flag..............  %u\n", pFileHdr->dataChanged);

  /* display the general config elements */
  COS_PRT(msgLvlReqd, "\nCOS debug message level:  %u (oper)  %u (config)\n\n",
          pCosInfo_g->msgLvl, pCosCfgData_g->msgLvl);

  /* NOTE:  L7_ALL_INTERFACES is used to display global config */
  cosConfigDataShow(L7_ALL_INTERFACES);

  intIfNum = 0;
  while (cosIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* only display information for configurable (i.e. created) interfaces */
    if (cosIntfIsConfigurable(intIfNum, (L7_cosCfgIntfParms_t **)L7_NULLPTR) == L7_TRUE)
      cosConfigDataShow(intIfNum);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Display a list of interfaces whose configuration does not
*           match the global config
*
* @param    void
*
* @returns  void
*
* @end
*********************************************************************/
void cosConfigIntfOverrideShow(void)
{
  L7_uint32     msgLvlReqd = COS_MSGLVL_ON;     /* always display output */
  L7_uint32     maxPerLine = 20;        /* entries output per line */
  L7_uint32     intIfNum, count;

  /* make sure config data exists */
  if (pCosCfgData_g == L7_NULLPTR)
  {
    COS_PRT(msgLvlReqd, "\nCOS component configuration data does not exist\n\n");
    return;
  }

  /* display header banner */
  COS_PRT(msgLvlReqd, "\nThe following interfaces differ from global config:\n\n");

  count = 0;

  intIfNum = 0;
  while (cosIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    if (cosIntfIsConfigurable(intIfNum, (L7_cosCfgIntfParms_t **)L7_NULLPTR)
        == L7_TRUE)
    {
      if (cosConfigIntfMatchesGlobal(intIfNum) != L7_TRUE)
      {
        if ((count % maxPerLine) == 0)
        {
          COS_PRT(msgLvlReqd, "  ");
        }

        COS_PRT(msgLvlReqd, "%3u ", intIfNum);
        count++;
      }
    }
  }

  if (count == 0)
  {
    COS_PRT(msgLvlReqd, "  (none)");
  }
  COS_PRT(msgLvlReqd, "\n\n");
}

/*********************************************************************
* @purpose  Check if interface config matches the global config data
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Internal function.
*
* @end
*********************************************************************/
static L7_BOOL cosConfigIntfMatchesGlobal(L7_uint32 intIfNum)
{
  L7_cosCfgIntfParms_t    *pCfgIntf;

  /* make sure config data exists */
  if (pCosCfgData_g == L7_NULLPTR)
    return L7_FALSE;

  /* get ptr to interface config data */
  if (cosIntfIsConfigurable(intIfNum, &pCfgIntf) != L7_TRUE)
    return L7_FALSE;

  /* compare the common 'cfg' sections of the global and interface configs */
  if (memcmp(&pCfgIntf->cfg, &pCosCfgData_g->cosGlobal.cfg,
             sizeof(L7_cosCfgParms_t)) != 0)
    return L7_FALSE;

  return L7_TRUE;
}


/*============================================================================*/
/*================= START OF CONFIG MIGRATION DEBUG CHANGES ==================*/
/*============================================================================*/

/*********************************************************************
*
* @purpose  Build test COS intf config data
*
* @param    intIfNum    Internal interface number
* @param    *pCfg       Configuration data ptr for this interface
*
* @returns  void
*
* @comments The test config data consists of arbitrary non-default values.
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*
*********************************************************************/
void cosBuildTestIntfConfigData(L7_uint32 intIfNum, L7_cosCfgParms_t *pCfg)
{
  L7_uint32             i, j;
  L7_cosMapCfg_t        *pMap;
  L7_cosIntfCfg_t       *pIntf;
  L7_cosQueueCfg_t      *pQueue;
  L7_cosDropPrecCfg_t   *pDp;
  L7_uchar8             testMapTableInit[FD_QOS_COS_MAX_QUEUES_PER_INTF]
                                        [L7_QOS_COS_MAP_NUM_IPPREC] =
  {
    {1,1,1,1,1,1,1,1},                  /* 1 queue (invalid values) */
    {1,1,1,1,0,0,0,0},                  /* 2 queues */
    {2,2,2,1,1,1,0,0},                  /* 3 queues */
    {3,3,2,2,1,1,0,0},                  /* 4 queues */
    {4,4,3,3,2,2,1,0},                  /* 5 queues */
    {5,5,4,4,3,2,1,0},                  /* 6 queues */
    {6,6,5,4,3,2,1,0},                  /* 7 queues */
    {7,6,5,4,3,2,1,0}                   /* 8 queues */
  };

  /* should only be here for a valid interface */
  if (intIfNum == 0)
    return;

  memset(pCfg, 0, sizeof(L7_cosCfgParms_t));

  /* mapping table config */
  pMap = &pCfg->mapping;
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPPREC; i++)
    pMap->ipPrecMapTable[i] = testMapTableInit[L7_MAX_CFG_QUEUES_PER_PORT-1][i];
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPDSCP; i++)
    pMap->ipDscpMapTable[i] = testMapTableInit[L7_MAX_CFG_QUEUES_PER_PORT-1]
                                              [i/FD_QOS_COS_MAP_TABLE_DSCP_MAPPING_RATIO];
  pMap->intfTrustMode = (L7_uchar8)L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC;

  /* interface config */
  pIntf = &pCfg->intf;
  pIntf->intfShapingRate = 50;
  pIntf->queueMgmtTypePerIntf = L7_QOS_COS_QUEUE_MGMT_TYPE_WRED;
  pIntf->wredDecayExponent = 15;

  /* queue config */
  for (i = 0; i < L7_COS_INTF_QUEUE_MAX_COUNT; i++)
  {
    pQueue = &pCfg->queue[i];
    pQueue->queueId = i;
    pQueue->minBwPercent = 5;
    pQueue->maxBwPercent = 90;
    pQueue->schedulerType = L7_QOS_COS_QUEUE_SCHED_TYPE_STRICT;
    pQueue->wrr_weight    = i+1;                                  /* PTin added: QoS */
    pQueue->queueMgmtType = L7_QOS_COS_QUEUE_MGMT_TYPE_WRED;
    pQueue->wred_decayExponent = 9;                               /* PTin added: QoS */
    /* drop precedence parm config */
    for (j = 0; j < L7_COS_INTF_DROP_PREC_MAX_COUNT+1; j++)
    {
      pDp = &pQueue->dropPrec[j];
      pDp->tdropThresh = 1;
      pDp->wredMinThresh = 2;
      pDp->wredMaxThresh = 3;
      pDp->wredDropProb = 4;
    } /* endfor j */
  } /* endfor i */
}

/*********************************************************************
*
* @purpose  Build test COS config data
*
* @param    void
*
* @returns  void
*
* @comments The test config data consists of arbitrary non-default values.
*
* @end
*
*********************************************************************/
void cosBuildTestConfigData(void)
{
  L7_uint32               cfgIndex;
  nimConfigID_t           configIdSave[L7_COS_INTF_MAX_COUNT];
  L7_cosCfgGlobalParms_t  *pCfgGlob;
  L7_cosCfgIntfParms_t    *pCfgIntf;
  nimConfigID_t           configIdNull;
  L7_uint32               intIfNum;

  memset(&configIdNull, 0, sizeof(configIdNull));
  memset(configIdSave, 0, sizeof(configIdSave));

  /* save all existing configId values before clearing the entire cfg struct */
  for (cfgIndex = 1; cfgIndex < L7_COS_INTF_MAX_COUNT; cfgIndex++)
  {
    NIM_CONFIG_ID_COPY(&configIdSave[cfgIndex],
                       &pCosCfgData_g->cosIntf[cfgIndex].configId);
  }

  memset(pCosCfgData_g, 0, L7_COS_CFG_DATA_SIZE);

  /* build config file header */
  strcpy((char *)pCosCfgData_g->cfgHdr.filename, L7_COS_CFG_FILENAME);
  pCosCfgData_g->cfgHdr.version       = L7_COS_CFG_VER_CURRENT;
  pCosCfgData_g->cfgHdr.componentID   = L7_FLEX_QOS_COS_COMPONENT_ID;
  pCosCfgData_g->cfgHdr.type          = L7_CFG_DATA;
  pCosCfgData_g->cfgHdr.length        = (L7_uint32)L7_COS_CFG_DATA_SIZE;
  pCosCfgData_g->cfgHdr.dataChanged   = L7_FALSE;

  /* build general config data */
  pCosCfgData_g->msgLvl = COS_MSGLVL_OFF;

  /* build global config defaults */
  pCfgGlob = &pCosCfgData_g->cosGlobal;
  cosBuildTestIntfConfigData(L7_ALL_INTERFACES, &pCfgGlob->cfg);

  /* build interface default config, using previously saved configId values
   *
   * NOTE:  Do not build config for interfaces that have not been created
   *        yet (i.e. configId is null).
   */
  for (cfgIndex = 1; cfgIndex < L7_COS_INTF_MAX_COUNT; cfgIndex++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configIdSave[cfgIndex], &configIdNull) != L7_TRUE)
    {
      if (nimIntIfFromConfigIDGet(&configIdSave[cfgIndex], &intIfNum) == L7_SUCCESS)
      {
        pCfgIntf = &pCosCfgData_g->cosIntf[cfgIndex];

        NIM_CONFIG_ID_COPY(&pCfgIntf->configId, &configIdSave[cfgIndex]);

        cosBuildTestIntfConfigData(intIfNum, &pCfgIntf->cfg);
      }
    }
  } /* endfor cfgIndex */

  pCosCfgData_g->checkSum = 0;

  sysapiPrintf("\nBuilt COS test config data\n\n");
}

/*********************************************************************
*
* @purpose  Save test COS config data to NVStore
*
* @param    void
*
* @returns  void
*
* @comments Cannot use normal config save, since the config data contents
*           get overwritten by the operational config.
*
* @end
*
*********************************************************************/
void cosSaveTestConfigData(void)
{
  pCosCfgData_g->cfgHdr.dataChanged = L7_FALSE;
  pCosCfgData_g->checkSum = nvStoreCrc32((L7_uchar8*)pCosCfgData_g,
                                         (L7_uint32)(sizeof(L7_cosCfgData_t) - sizeof(pCosCfgData_g->checkSum)));

  if (sysapiCfgFileWrite(L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_CFG_FILENAME,
                        (L7_char8 *)pCosCfgData_g,
                        (L7_uint32)sizeof(L7_cosCfgData_t)) != L7_SUCCESS)
  {
    sysapiPrintf("\nAttempt to save COS test config data failed!");
    return;
  }

  sysapiPrintf("\nSaved COS test config data to NVStore\n\n");
}

/*********************************************************************
*
* @purpose  Dump test COS config data contents
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
void cosConfigDataTestShow(void)
{
  /* config file header contents */
  sysapiCfgFileHeaderDump(&pCosCfgData_g->cfgHdr);
  sysapiPrintf("\n");

  /* config file parameters */
  cosConfigDataShowAll();

  /* checksum */
  sysapiPrintf("pCosCfgData_g->checkSum : 0x%8.8x\n", pCosCfgData_g->checkSum);

  /* pertinent scaling constants */
  sysapiPrintf("\nScaling Constants\n");
  sysapiPrintf(  "-----------------\n");

  sysapiPrintf("L7_QOS_COS_MAP_NUM_IPPREC.................. %u\n",
               (L7_uint32)L7_QOS_COS_MAP_NUM_IPPREC);

  sysapiPrintf("L7_QOS_COS_MAP_NUM_IPDSCP.................. %u\n",
               (L7_uint32)L7_QOS_COS_MAP_NUM_IPDSCP);

  sysapiPrintf("L7_COS_INTF_MAX_COUNT...................... %u\n",
               (L7_uint32)L7_COS_INTF_MAX_COUNT);

  sysapiPrintf("L7_COS_INTF_QUEUE_MAX_COUNT................ %u\n",
               (L7_uint32)L7_COS_INTF_QUEUE_MAX_COUNT);

  sysapiPrintf("L7_COS_INTF_DROP_PREC_MAX_COUNT............ %u\n",
               (L7_uint32)L7_COS_INTF_DROP_PREC_MAX_COUNT);

  sysapiPrintf("\n");
}

/*============================================================================*/
/*================== END OF CONFIG MIGRATION DEBUG CHANGES ===================*/
/*============================================================================*/

