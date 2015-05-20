/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   cos_util.c
*
* @purpose    COS component utility functions
*
* @component  cos
*
* @comments   none
*
* @create     03/12/2004
*
* @author     gpaussa
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "l7_flex_packages.h"
#include "registry.h"
#include "nimapi.h"
#include "cnfgr.h"
#include "usmdb_util_api.h"
#include "dot1q_api.h"
#include "dtl_qos_cos.h"
#include "cos_util.h"
#include "log.h"

cosInfo_t       *pCosInfo_g = L7_NULLPTR;

/* display strings for debug messages (matches enum defs) */
char *cosMapIntfModeStr[] =
{
  "---", "untrusted", "trust 802.1p","trust ip prec", "trust ip dscp"
};
char *cosQueueSchedTypeStr[] =
{
  "---", "sp", "wrr"
};
char *cosQueueMgmtTypeStr[] =
{
  "rsvd", "tail", "wred", "unch"
};

/* COS feature list display strings */
char *cosFeatureString[L7_COS_FEATURE_ID_TOTAL] =
{
  "",                                           /* not used */
  "LAG Intf Support................. %s\n",

  "IP Precedence Mapping............ %s\n",
  "IP Precedence Mapping per Intf... %s\n",
  "IP DSCP Mapping.................. %s\n",
  "IP DSCP Mapping per Intf......... %s\n",
  "Trust Mode....................... %s\n",
  "Trust Mode per Intf.............. %s\n",

  "Queue Config per intf............ %s\n",
  "Queue Drop Cfg per intf.......... %s\n",
  "Intf Shaping..................... %s\n",
  "Min Bandwidth.................... %s\n",
  "Max Bandwidth.................... %s\n",
  "Sched Strict Only................ %s\n",
  "Queue Mgmt per Intf Only......... %s\n",
  "Tail Drop Threshold Config....... %s\n",
  "WRED Supported................... %s\n",
  "WRED Max Threshold Config........ %s\n",
  "WRED Decay Exp System-wide Only.. %s\n"
};


/*********************************************************************
* @purpose  Find COS config pointer for specified interface (or global)
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    **ppCfg     @b{(output)} Ptr to COS config parms ptr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosCfgPtrFind(L7_uint32 intIfNum, L7_cosCfgParms_t **ppCfg)
{
  L7_cosCfgIntfParms_t  *pCfgIntf;

  if (cosIntfIsValid(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  if ((ppCfg == L7_NULLPTR) || (pCosCfgData_g == L7_NULLPTR))
    return L7_FAILURE;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    *ppCfg = &pCosCfgData_g->cosGlobal.cfg;
  }
  else
  {
    if (cosIntfIsConfigurable(intIfNum, &pCfgIntf) != L7_TRUE)
      return L7_FAILURE;

    *ppCfg = &pCfgIntf->cfg;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine if the specified interface is valid for COS config
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL cosIntfIsValid(L7_uint32 intIfNum)
{
  /* treat global mode L7_ALL_INTERFACES as valid value */
  if (intIfNum != L7_ALL_INTERFACES)
  {
    if (cosIntfIndexGet(intIfNum) != L7_SUCCESS)
      return L7_FALSE;
  }

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Determine interface type validity for use with COS component
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments At present, only physical interfaces are supported.
*
* @end
*********************************************************************/
L7_BOOL cosIntfTypeIsValid(L7_uint32 intIfNum)
{
  L7_BOOL       rc = L7_FALSE;
  L7_INTF_TYPES_t sysIntfType;

  /* NIM complains about an intIfNum of 0, so check for this first */
  if ((intIfNum > 0) &&
      (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS))
  {
    /* NOTE:  COS only supports physical and LAG interfaces.  The application
     *        code relies on the device driver to manage the configuration
     *        for ports as they are acquired by and released from a LAG.
     */
    switch (sysIntfType)
    {
    case L7_PHYSICAL_INTF:
      rc = L7_TRUE;
      break;

    case L7_LAG_INTF:
      /* check the feature support to ensure LAG interfaces are allowed */
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                                L7_COS_LAG_INTF_SUPPORT_FEATURE_ID) == L7_TRUE)
      {
        rc = L7_TRUE;
      }
      break;

    default:
      break;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Verify specified queue config interface index is valid
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
L7_RC_t cosIntfIndexGet(L7_uint32 intIfNum)
{
  /* the global L7_ALL_INTERFACES value is not valid in context of this API */
  if (intIfNum == L7_ALL_INTERFACES)
    return L7_FAILURE;

  if (cosIntfTypeIsValid(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine next sequential queue config interface index
*
* @param    intIfNum    @b{(input)}  Internal interface number to begin search
* @param    *pNext      @b{(output)} Ptr to next internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosIntfIndexGetNext(L7_uint32 intIfNum, L7_uint32 *pNext)
{
  L7_RC_t       rc;
  L7_BOOL       lagSupported;

  if (pNext == L7_NULLPTR)
    return L7_FAILURE;

  /* check if the LAG intf feature is supported */
  lagSupported = cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                                       L7_COS_LAG_INTF_SUPPORT_FEATURE_ID);

  /* first check for the next physical intf, then check for the next LAG intf
   * (if feature is supported) when no more physical intfs are found
   */
  rc = nimNextValidIntfNumberByType(L7_PHYSICAL_INTF, intIfNum, pNext);

  if ((rc != L7_SUCCESS) && (lagSupported == L7_TRUE))
  {
    rc = nimNextValidIntfNumberByType(L7_LAG_INTF, intIfNum, pNext);
  }

  /* normalize return codes from NIM */
  if (rc != L7_SUCCESS)
    rc = L7_FAILURE;

  return rc;
}

/*********************************************************************
* @purpose  Get COS debug message level
*
* @param    void
*
* @returns  message level value
*
* @comments Messages are defined according to interest level.  Higher
*           values generally display more debug messages.
*
* @comments This function returns the operational msgLvl value.
*
* @end
*********************************************************************/
L7_uint32 cosMsgLvlGet(void)
{
  return pCosInfo_g->msgLvl;
}

/*********************************************************************
* @purpose  Set COS debug message level
*
* @param    msgLvl      @b{(input)} Message level (0 = off, >0 = on)
*
* @returns  void
*
* @comments Messages are defined according to interest level.  Higher
*           values generally display more debug messages.
*
* @comments This function sets the operational msgLvl value.
*
* @end
*********************************************************************/
void cosMsgLvlSet(L7_uint32 msgLvl)
{
  /* NOTE:  The pCosCfgData_g->msgLvl value is set during cosSave() processing. */
  pCosInfo_g->msgLvl = msgLvl;
  pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
}

/*************************************************************************
* @purpose  Apply the assigned traffic class (queue) for this IP precedence
*           on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    prec        @b{(input)}  IP precedence
* @param    val         @b{(input)}  Traffic class value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosMapIpPrecTrafficClassApply(L7_uint32 intIfNum, L7_uint32 prec,
                                      L7_uint32 val)
{
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (dtlQosCosMapIpPrecTrafficClassSet(intIfNum, prec, val) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
            "COS mapping: Unable to apply IP Precedence %u to traffic class %u "
            "on intf %s\n", prec, val, ifName);
    return L7_FAILURE;
  }

  /* display debug message according to COS msgLvl setting */
  COS_PRT(COS_MSGLVL_MED, "\nCOS IP Prec %u mapped to queue %u on intf %u\n",
          prec, val, intIfNum);

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Apply the assigned traffic class (queue) for this IP DSCP
*           on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    dscp        @b{(input)}  IP DSCP
* @param    val         @b{(input)}  Traffic class value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosMapIpDscpTrafficClassApply(L7_uint32 intIfNum, L7_uint32 dscp,
                                      L7_uint32 val)
{
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (dtlQosCosMapIpDscpTrafficClassSet(intIfNum, dscp, val) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
            "COS mapping: Unable to apply IP DSCP %u to traffic class %u "
            "on intf %s\n", dscp, val, ifName);
    return L7_FAILURE;
  }

  /* display debug message according to COS msgLvl setting */
  COS_PRT(COS_MSGLVL_MED, "\nCOS IP DSCP %u mapped to queue %u on intf %u\n",
          dscp, val, intIfNum);

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Apply the COS trust mode setting for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    *pCfg       @b{(input)}  COS config parms ptr
* @param    forceDtl    @b{(input)}  Force DTL call even if in trust-dot1p mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosMapIntfTrustModeApply(L7_uint32 intIfNum,
                                 L7_cosCfgParms_t *pCfg,
                                 L7_BOOL forceDtl)
{
  L7_uint32                   msgLvlReqd = COS_MSGLVL_MED;
  L7_QOS_COS_MAP_INTF_MODE_t  mode;
  DTL_QOS_COS_MAP_TABLE_t     mapTable;
  L7_uint32                   i, tc;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  mode = (L7_QOS_COS_MAP_INTF_MODE_t)pCfg->mapping.intfTrustMode;

  switch (mode)
  {
  case L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED:
  case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
  case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
    break;

  case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
    /* in most cases, don't call HAPI when in 'trust dot1p' mode, since the
     * dot1q component takes care of it
     *
     * however, there are certain times when the COS component must signal
     * HAPI due to certain mode changes, as indicated by the forceDtl flag
     * (e.g. trust mode apply, interface detach)
     */
    if (forceDtl == L7_FALSE)
      return L7_SUCCESS;
    break;

  default:
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
            "COS mapping: Invalid trust mode value (%u) on intf %s\n",
            mode, ifName);
    return L7_FAILURE;
    /*PASSTHRU*/
  }

  /* simply return with success if intf state not ready for DTL call */
  if (cosIntfIsWriteable(intIfNum, L7_COS_MAP_TRUST_MODE_PER_INTF_FEATURE_ID) != L7_TRUE)
    return L7_SUCCESS;

  /* All mapping tables are built here, although which ones are active
   * depend on the trust mode.  Note that the dot1p table is used in every
   * mode, although it maps all priorities to the port default traffic class
   * for modes other than 'trust dot1p'.
   */

  memset(&mapTable, 0, sizeof(mapTable));

  /* if 'trust dot1p' mode, gather the dot1p map info from the dot1p
   * component, otherwise use the operational port default traffic class
   * for this interface (was previously obtained from dot1p component)
   */
  tc = pCosInfo_g->portDefaultTrafficClass[intIfNum]; /* port default traffic class */
  for (i = 0; i < (L7_DOT1P_MAX_PRIORITY+1); i++)
  {
    if (mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
    {
      if (dot1dPortTrafficClassGet(intIfNum, i, &tc) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
                "COS mapping: Unable to obtain dot1p mapping table for intf %s\n",
                ifName);
        return L7_FAILURE;
      }
    }
    mapTable.dot1pTrafficClass[i] = (L7_uchar8)tc;
  } /* endfor */

  /* build the IP precedence map table */
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPPREC; i++)
    mapTable.ipPrecTrafficClass[i] = pCfg->mapping.ipPrecMapTable[i];

  /* build the IP DSCP map table */
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPDSCP; i++)
    mapTable.ipDscpTrafficClass[i] = pCfg->mapping.ipDscpMapTable[i];

  if (dtlQosCosMapIntfTrustModeSet(intIfNum, mode, &mapTable) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
            "COS mapping: Unable to apply trust mode \'%s\' on intf %s\n",
            cosMapIntfModeStr[mode], ifName);
    return L7_FAILURE;
  }

  /* display debug message according to COS msgLvl setting */
  COS_PRT(msgLvlReqd, "\nCOS intf mode set to \'%s\' on intf %u, %s\n\n",
          cosMapIntfModeStr[mode], intIfNum, ifName);
  switch (mode)
  {
  case L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED:
    cosMapPortDefaultPriorityTableShow(intIfNum, msgLvlReqd);
    break;

  case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
    cosMapDot1pTableShow(intIfNum, msgLvlReqd);
    break;

  case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
    cosMapIpPrecTableShow(intIfNum, msgLvlReqd);
    cosMapPortDefaultPriorityTableShow(intIfNum, msgLvlReqd);
    break;

  case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
    cosMapIpDscpTableShow(intIfNum, msgLvlReqd);
    cosMapPortDefaultPriorityTableShow(intIfNum, msgLvlReqd);
    break;

  default:
    break;
  }

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Apply the COS interface parameters for this interface
*
* @param    intIfNum        @b{(input)}  Internal interface number
* @param    intfShapingRate @b{(input)}  Interface shaping rate
* @param    qMgmtTypeIntf   @b{(input)}  Queue mgmt type (per-interface)
* @param    wredDecayExp    @b{(input)}  WRED decay exponent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueIntfConfigApply(L7_uint32 intIfNum,
                                L7_uint32 intfShapingRate,
                                L7_QOS_COS_QUEUE_MGMT_TYPE_t qMgmtTypeIntf,
                                L7_uint32 wredDecayExp)
{
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  /* simply return with success if intf state not ready for DTL call */
  if (cosIntfIsWriteable(intIfNum, L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) != L7_TRUE)
    return L7_SUCCESS;

  if (dtlQosCosIntfConfigSet(intIfNum, intfShapingRate,
                             qMgmtTypeIntf, wredDecayExp) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
            "COS queueing: Unable to apply COS intf config on interface %s\n",
            ifName);
    return L7_FAILURE;
  }

  /* display debug message according to COS msgLvl setting */
  COS_PRT(COS_MSGLVL_MED, "\nCOS intf config applied on intf %u, %s\n", intIfNum, ifName);
  if (cosMsgLvlGet() >= COS_MSGLVL_LO)
  {
    COS_PRT(COS_MSGLVL_LO, "\n  intfShapingRate:  %u%", intfShapingRate);
    COS_PRT(COS_MSGLVL_LO, "\n    qMgmtTypeIntf:  %u (%s)",
            (L7_uint32)qMgmtTypeIntf, cosQueueMgmtTypeStr[qMgmtTypeIntf]);
    COS_PRT(COS_MSGLVL_LO, "\n     wredDecayExp:  %u", wredDecayExp);
    COS_PRT(COS_MSGLVL_LO, "\n");
  }

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the COS interface parameters for this interface
*
* @param    intIfNum        @b{(input)}  Internal interface number
* @param    intfShapingRate @b{(input)}  Interface shaping rate in kbps
* @param    intfShapingBurstSize @b{(input)}  Interface shaping burst size in kbits
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosIntfShapingStatusGet(L7_uint32 intIfNum,
                                L7_uint32 *intfShapingRate,
                                L7_uint32 *intfShapingBurstSize)
{
  return(dtlQosCosIntfStatusGet(intIfNum, intfShapingRate, intfShapingBurstSize));
}

/*************************************************************************
* @purpose  Apply the COS queue scheduler parameters for this interface
*
* @param    intIfNum        @b{(input)}  Internal interface number
* @param    *pQParms        @b{(input)}  Ptr to queue sched parameters
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Each of the 'list' style parameters contains a separate
*           value for each supported COS queue.
*
* @end
*********************************************************************/
L7_RC_t cosQueueSchedConfigApply(L7_uint32 intIfNum,
                                 L7_cosQueueSchedParms_t *pQParms)
{
  L7_uint32     i;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  /* simply return with success if intf state not ready for DTL call */
  if (cosIntfIsWriteable(intIfNum, L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) != L7_TRUE)
    return L7_SUCCESS;

  if (dtlQosCosQueueSchedConfigSet(intIfNum,
                                   &pQParms->minBwList,
                                   &pQParms->maxBwList,
                                   &pQParms->schedTypeList,
                                   &pQParms->wrr_weights) != L7_SUCCESS)      /* PTin modified: QoS */
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
            "COS queueing: Unable to apply COS scheduler config on intf %s\n",
            ifName);
    return L7_FAILURE;
  }

  /* display debug message according to COS msgLvl setting */
  COS_PRT(COS_MSGLVL_MED, "\nCOS scheduler config applied on intf %u, %s\n", intIfNum, ifName);
  if (cosMsgLvlGet() >= COS_MSGLVL_LO)
  {
    COS_PRT(COS_MSGLVL_LO, "\n  minBw:  ");
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
      COS_PRT(COS_MSGLVL_LO, "%4u ", pQParms->minBwList.bandwidth[i]);
    COS_PRT(COS_MSGLVL_LO, "\n  maxBw:  ");
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
      COS_PRT(COS_MSGLVL_LO, "%4u ", pQParms->maxBwList.bandwidth[i]);
    COS_PRT(COS_MSGLVL_LO, "\n  sched:  ");
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
      COS_PRT(COS_MSGLVL_LO, "%4.3s ", cosQueueSchedTypeStr[pQParms->schedTypeList.schedType[i]]);
    COS_PRT(COS_MSGLVL_LO, "\n");
  }

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Apply the queue drop config parms list for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    *pVal       @b{(input)}  Ptr to drop parms list
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The 'list' parameter contains a separate value for each
*           supported COS queue and drop precedence level.
*
* @end
*********************************************************************/
L7_RC_t cosQueueDropParmsApply(L7_uint32 intIfNum,
                               L7_qosCosDropParmsList_t *pVal)
{
  L7_uint32     cosIndex,i;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  /* simply return with success if intf state not ready for DTL call */
  if (cosIntfIsWriteable(intIfNum, L7_COS_QUEUE_DROP_CFG_PER_INTF_FEATURE_ID) != L7_TRUE)
    return L7_SUCCESS;

  /* display debug message according to COS msgLvl setting */
  COS_PRT(COS_MSGLVL_MED, "\nCOS drop parms applied on intf %u\n",
          intIfNum);
  if (cosMsgLvlGet() >= COS_MSGLVL_LO)
  {
    for(cosIndex=0; cosIndex<L7_MAX_CFG_QUEUES_PER_PORT; cosIndex++) {
        if (pVal->queue[cosIndex].mgmtType == L7_QOS_COS_QUEUE_MGMT_TYPE_UNCHANGED)
        {
            continue;
        }
        COS_PRT(COS_MSGLVL_LO, "Queue %d, mgmt type %d: \n      minThresh:  ", cosIndex,
                pVal->queue[cosIndex].mgmtType);
        for (i = 0; i < L7_MAX_CFG_DROP_PREC_LEVELS+1; i++)
          COS_PRT(COS_MSGLVL_LO, "%2u ", pVal->queue[cosIndex].minThreshold[i]);
        COS_PRT(COS_MSGLVL_LO, "\n      maxThresh:  ");
        for (i = 0; i < L7_MAX_CFG_DROP_PREC_LEVELS+1; i++)
          COS_PRT(COS_MSGLVL_LO, "%2u ", pVal->queue[cosIndex].wredMaxThreshold[i]);
        COS_PRT(COS_MSGLVL_LO, "\n  dropProb:  ");
        for (i = 0; i < L7_MAX_CFG_DROP_PREC_LEVELS+1; i++)
          COS_PRT(COS_MSGLVL_LO, "%2u ", pVal->queue[cosIndex].dropProb[i]);
        COS_PRT(COS_MSGLVL_LO, "\n      TDMaxThresh:  ");
        for (i = 0; i < L7_MAX_CFG_DROP_PREC_LEVELS+1; i++)
          COS_PRT(COS_MSGLVL_LO, "%2u ", pVal->queue[cosIndex].tailDropMaxThreshold[i]);
        COS_PRT(COS_MSGLVL_LO, "\n");
    }
  }

  if (dtlQosCosQueueDropConfigSet(intIfNum, pVal) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_COS_COMPONENT_ID,
            "COS queueing: Unable to apply COS drop parms on intf %s\n",
            ifName);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Validate WRED / taildrop config parms
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    *pVal       @b{(input)}  Ptr to drop parms list
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueDropParmsValidate(L7_qosCosDropParmsList_t *pVal)
{
  L7_uint32                 queueIndex, precIndex;
  L7_BOOL                   maxThreshSupp;

  if (pVal == L7_NULLPTR)
        return L7_FAILURE;

  maxThreshSupp = cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                            L7_COS_QUEUE_WRED_MAX_THRESH_FEATURE_ID);

  /* check proposed value list */
  for(queueIndex = 0; queueIndex < L7_MAX_CFG_QUEUES_PER_PORT; queueIndex++)
  {
      for (precIndex = 0; precIndex < (L7_MAX_CFG_DROP_PREC_LEVELS+1); precIndex++)
      {
        /* If we're not changing anything on this queue, no need to validate.
           Otherwise, check both WRED and taildrop (might be setting either) */
        if (pVal->queue[queueIndex].mgmtType == L7_QOS_COS_QUEUE_MGMT_TYPE_UNCHANGED)
        {
            continue;
        }
        if (((pVal->queue[queueIndex].minThreshold[precIndex]+1) < (L7_QOS_COS_QUEUE_WRED_MIN_THRESH_MIN+1)) ||
            (pVal->queue[queueIndex].minThreshold[precIndex] > L7_QOS_COS_QUEUE_WRED_MIN_THRESH_MAX))
            return L7_FAILURE;

        if (((pVal->queue[queueIndex].dropProb[precIndex]+1) < (L7_QOS_COS_QUEUE_WRED_DROP_PROB_MIN+1)) ||
            (pVal->queue[queueIndex].dropProb[precIndex] > L7_QOS_COS_QUEUE_WRED_DROP_PROB_MAX))
            return L7_FAILURE;

        if (maxThreshSupp == L7_TRUE)
        {
            /* only check maxThreshold values if supported (otherwise not used) */
            if (((pVal->queue[queueIndex].wredMaxThreshold[precIndex]+1) < (L7_QOS_COS_QUEUE_WRED_MAX_THRESH_MIN+1)) ||
                (pVal->queue[queueIndex].wredMaxThreshold[precIndex] > L7_QOS_COS_QUEUE_WRED_MAX_THRESH_MAX))
                return L7_FAILURE;

            /* do not allow max thresh to be set less than min thresh */
            if (pVal->queue[queueIndex].wredMaxThreshold[precIndex] <
                pVal->queue[queueIndex].minThreshold[precIndex])
                return L7_FAILURE;
        }
        if (((pVal->queue[queueIndex].tailDropMaxThreshold[precIndex]+1) < (L7_QOS_COS_QUEUE_TDROP_THRESH_MIN+1)) ||
            (pVal->queue[queueIndex].tailDropMaxThreshold[precIndex] > L7_QOS_COS_QUEUE_TDROP_THRESH_MAX))
        {
            return L7_FAILURE;
        }
        /* PTin added: QoS */
        if (((pVal->queue[queueIndex].wred_decayExponent+1) < (L7_QOS_COS_INTF_WRED_DECAY_EXP_MIN+1)) ||
            (pVal->queue[queueIndex].wred_decayExponent > L7_QOS_COS_INTF_WRED_DECAY_EXP_MAX))
        {
            return L7_FAILURE;
        }
      }
  }
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  See if anything changed between saved config and what's to
*           be applied
*
* @param    *pVal       @b{(input)}  Ptr to drop parms list
* @param    *pCfg       @b{(input)}  Ptr to COS config structure
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL cosQueueDropParmsDiffer(L7_qosCosDropParmsList_t *pVal, L7_cosCfgParms_t *pCfg)
{
    L7_BOOL                   maxThreshSupp;
    L7_cosDropPrecCfg_t       *pDP;
    L7_uint32                 queueIndex, precIndex;

    maxThreshSupp = cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_WRED_MAX_THRESH_FEATURE_ID);

    for (queueIndex = 0; queueIndex < L7_MAX_CFG_QUEUES_PER_PORT; queueIndex++)
    {
        if (pVal->queue[queueIndex].mgmtType == L7_QOS_COS_QUEUE_MGMT_TYPE_UNCHANGED)
        {
            continue;
        }
        if (pCfg->queue[queueIndex].queueMgmtType != pVal->queue[queueIndex].mgmtType)
        {
            return L7_TRUE;
        }
        /* PTin added: QoS */
        if (pCfg->queue[queueIndex].wred_decayExponent != pVal->queue[queueIndex].wred_decayExponent)
        {
            return L7_TRUE;
        }
        for (precIndex = 0; precIndex < (L7_MAX_CFG_DROP_PREC_LEVELS+1); precIndex++)
        {
            pDP = &pCfg->queue[queueIndex].dropPrec[precIndex];
            if ((pVal->queue[queueIndex].minThreshold[precIndex] != pDP->wredMinThresh) ||
                (pVal->queue[queueIndex].dropProb[precIndex] != pDP->wredDropProb) ||
                (pVal->queue[queueIndex].tailDropMaxThreshold[precIndex] != pDP->tdropThresh) ||
                ((maxThreshSupp == L7_TRUE) &&
                 (pVal->queue[queueIndex].wredMaxThreshold[precIndex] != pDP->wredMaxThresh)))
            {
                return L7_TRUE;
            }
        } /* End of per-precedence loop */
    } /* End of per-queue loop */
    return L7_FALSE;
}

/*************************************************************************
* @purpose  Update saved config with current drop parms
*
* @param    *pVal       @b{(input)}  Ptr to drop parms list
* @param    *pCfg       @b{(input)}  Ptr to COS config structure
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cosQueueDropConfigUpdate(L7_qosCosDropParmsList_t *pVal, L7_cosCfgParms_t *pCfg)
{
    L7_cosDropPrecCfg_t       *pDP;
    L7_uint32                 queueIndex, precIndex;

    for (queueIndex = 0; queueIndex < L7_MAX_CFG_QUEUES_PER_PORT; queueIndex++)
    {
        if (pVal->queue[queueIndex].mgmtType == L7_QOS_COS_QUEUE_MGMT_TYPE_UNCHANGED)
        {
            continue;
        }
        pCfg->queue[queueIndex].queueMgmtType = pVal->queue[queueIndex].mgmtType;

        /* PTin added: QoS */
        pCfg->queue[queueIndex].wred_decayExponent = pVal->queue[queueIndex].wred_decayExponent;

        for (precIndex = 0; precIndex < (L7_MAX_CFG_DROP_PREC_LEVELS+1); precIndex++)
        {
          pDP = &pCfg->queue[queueIndex].dropPrec[precIndex];
          pDP->wredMinThresh = pVal->queue[queueIndex].minThreshold[precIndex];
          pDP->wredMaxThresh = pVal->queue[queueIndex].wredMaxThreshold[precIndex];
          pDP->wredDropProb = pVal->queue[queueIndex].dropProb[precIndex];
          pDP->tdropThresh = pVal->queue[queueIndex].tailDropMaxThreshold[precIndex];
        }
    }
}

/*********************************************************************
* @purpose  Display a list of debug commands available for COS
*
* @param    void
*
* @returns  void
*
* @end
*********************************************************************/
void cosDebugHelp(void)
{
  L7_uint32     msgLvlReqd = COS_MSGLVL_ON;     /* always display output */

  /* display header banner */
  COS_PRT(msgLvlReqd, "\nThe following debug \'show\' commands are available:\n\n");
  COS_PRT(msgLvlReqd, "  - cosConfigDataShow(0|intIfNum)\n");
  COS_PRT(msgLvlReqd, "  - cosConfigDataShowAll\n");
  COS_PRT(msgLvlReqd, "  - cosConfigIntfOverrideShow\n");
  COS_PRT(msgLvlReqd, "  - cosFeatureShow\n");
  COS_PRT(msgLvlReqd, "  - cosInfoShow\n");
  COS_PRT(msgLvlReqd, "\n\n");

  COS_PRT(msgLvlReqd, "The L7_ALL_INTERFACES value is: %u\n\n", L7_ALL_INTERFACES);

}

/*********************************************************************
* @purpose  Show the COS feature support list for this platform
*
* @param    none
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void cosFeatureShow(void)
{
#define COS_PLATFORM_NAME_LEN      128
  L7_uint32     msgLvlReqd = COS_MSGLVL_ON;     /* always display output */
  L7_uint32     compId = L7_FLEX_QOS_COS_COMPONENT_ID;
  char          platName[COS_PLATFORM_NAME_LEN+1];
  L7_uint32     i;
  char          *pYes = "Yes", *pNo = "No";

  if (sysapiRegistryGet(NPD_TYPE_STRING, STR_ENTRY, platName) != L7_SUCCESS)
    strcpy(platName, "Unknown");

  if (cnfgrFlexIsFeaturePresent(compId, L7_COS_FEATURE_SUPPORTED)
      == L7_FALSE)
  {
    COS_PRT(msgLvlReqd, "\nCOS not supported on platform: %s\n\n", platName);
    return;
  }

  COS_PRT(msgLvlReqd,
          "\nCOS FEATURE SUPPORT LIST (platform: %s)\n\n", platName);

  for (i = 1; i < L7_COS_FEATURE_ID_TOTAL; i++)
  {
    COS_PRT(msgLvlReqd, cosFeatureString[i],
            (cnfgrFlexIsFeaturePresent(compId, i) == L7_TRUE) ? pYes : pNo);
  }

  COS_PRT(msgLvlReqd, "\n");
}

/*********************************************************************
* @purpose  Show the COS operational info structure contents
*
* @param    none
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void cosInfoShow(void)
{
  L7_uint32     msgLvlReqd = COS_MSGLVL_ON;     /* always display output */
  L7_uint32     i;

  if (pCosInfo_g == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\npCosInfo_g is null\n\n");
    return;
  }

  COS_PRT(msgLvlReqd, "\nCOS operational info structure contents:\n\n");

  COS_PRT(msgLvlReqd, "  msgLvl=%u\n", pCosInfo_g->msgLvl);

  COS_PRT(msgLvlReqd, "  numTrafficClasses:\n");
  COS_PRT(msgLvlReqd, "    global:  %1u\n",
          (L7_uint32)pCosInfo_g->globalNumTrafficClasses);

  /* begin loop at 0 so first line formatting comes out right */
  for (i = 0; i < L7_MAX_INTERFACE_COUNT; i++)
  {
    if ((i % 16) == 0)
    {
      COS_PRT(msgLvlReqd, "\n    %3u:  ", i);
    }
    COS_PRT(msgLvlReqd, "%1u ", (L7_uint32)pCosInfo_g->numTrafficClasses[i]);
  }
  COS_PRT(msgLvlReqd, "\n\n");

  COS_PRT(msgLvlReqd, "  portDefaultPriority(portDefaultTrafficClass):\n");
  COS_PRT(msgLvlReqd, "    global:  %1u(Q%1u)\n",
          (L7_uint32)pCosInfo_g->globalPortDefaultPriority,
          (L7_uint32)pCosInfo_g->globalPortDefaultTrafficClass);

  /* begin loop at 0 so first line formatting comes out right */
  for (i = 0; i < L7_MAX_INTERFACE_COUNT; i++)
  {
    if ((i % 8) == 0)
    {
      COS_PRT(msgLvlReqd, "\n    %3u:  ", i);
    }
    COS_PRT(msgLvlReqd, "%1u(Q%1u)  ",
            (L7_uint32)pCosInfo_g->portDefaultPriority[i],
            (L7_uint32)pCosInfo_g->portDefaultTrafficClass[i]);
  }
  COS_PRT(msgLvlReqd, "\n\n");
}

