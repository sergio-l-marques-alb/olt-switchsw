/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/cos/cli_show_running_config_cos.c
 *
 * @purpose show running config commands for the cli
 *
 * @component user interface
 *
 * @comments
 *
 * @create  04/16/2004
 *
 * @author  kmans
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "l7_common.h"
#include "cos_exports.h"
#include "cli_web_exports.h"
#ifdef L7_QOS_PACKAGE
#include "usmdb_qos_cos_api.h"
#endif

#include "comm_mask.h"
#ifdef L7_QOS_FLEX_PACKAGE_COS
#include "config_script_api.h"
#include "clicommands_cos.h"

#include "datatypes.h"
#include "usmdb_counters_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "cli_web_user_mgmt.h"
#include "l7_cos_api.h"
#include "cli_show_running_config.h"

/*********************************************************************
* @purpose  To print the running configuration of global cos-queue min-bandwidth
*
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/
L7_RC_t cliRunningConfigGlobalCosQueue(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf2[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tmp[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 queueNum, val;
  L7_uint32 diff = L7_FALSE;
  L7_qosCosQueueBwList_t queueBwList;
  L7_qosCosQueueSchedTypeList_t queueSchedTypeList;
  L7_qosCosQueueMgmtTypeList_t queueMgmtTypeList;

  /* Maximum Bandwidth */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MAX_BW_FEATURE_ID) == L7_TRUE)
  {
    memset(&queueBwList, 0, sizeof(queueBwList));
    if (usmDbQosCosQueueMaxBandwidthListGet(unit, L7_ALL_INTERFACES, &queueBwList) == L7_SUCCESS)
    {
      memset (buf, 0,sizeof(buf));
      for (queueNum = 0 ; queueNum < L7_MAX_CFG_QUEUES_PER_PORT; queueNum++)
      {
        if (usmDbQosCosQueueIdIndexGet(unit, queueNum) == L7_SUCCESS)
        {
          if (queueBwList.bandwidth[queueNum] != FD_QOS_COS_QCFG_MAX_BANDWIDTH || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT )
          {
            diff = L7_TRUE;
          }
          osapiSnprintf(tmp, sizeof(tmp), "%-u ", queueBwList.bandwidth[queueNum]);
          strcat(buf, tmp);
        }
      }
      if (diff)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_CosQueueMaxBandwidth_1, buf );
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }

  diff = L7_FALSE;
  /* Minimum Bandwidth */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MIN_BW_FEATURE_ID) == L7_TRUE)
  {
    memset(&queueBwList, 0, sizeof(queueBwList));
    if (usmDbQosCosQueueMinBandwidthListGet(unit, L7_ALL_INTERFACES, &queueBwList) == L7_SUCCESS)
    {
      memset (buf, 0,sizeof(buf));
      for (queueNum = 0 ; queueNum < L7_MAX_CFG_QUEUES_PER_PORT; queueNum++)
      {
        if (usmDbQosCosQueueIdIndexGet(unit, queueNum) == L7_SUCCESS)
        {
          if (queueBwList.bandwidth[queueNum] != FD_QOS_COS_QCFG_MIN_BANDWIDTH || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT )
          {
            diff = L7_TRUE;
          }
          osapiSnprintf(tmp, sizeof(tmp), "%-u ", queueBwList.bandwidth[queueNum]);
          strcat(buf, tmp);
        }
      }
      if (diff)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_CosQueueMinBandwidth_1, buf );
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }

  diff = L7_FALSE;
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_FALSE)
  {
    /* Management Type */
    diff = L7_FALSE;
    memset(&queueMgmtTypeList, 0, sizeof(queueMgmtTypeList));
    if (usmDbQosCosQueueMgmtTypeListGet(unit, L7_ALL_INTERFACES, &queueMgmtTypeList) == L7_SUCCESS)
    {
      memset (buf, 0,sizeof(buf));
      for (queueNum = 0 ; queueNum < L7_MAX_CFG_QUEUES_PER_PORT; queueNum++)
      {
        if (usmDbQosCosQueueIdIndexGet(unit, queueNum) == L7_SUCCESS)
        {
          if (queueMgmtTypeList.mgmtType[queueNum] != FD_QOS_COS_QCFG_MGMT_TYPE || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT )
          {
            diff = L7_TRUE;
            osapiSnprintf(tmp, sizeof(tmp), "%u ", queueNum);
            strcat(buf, tmp);
          }
        }
      }
      if ((diff) && (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE))
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_CosQueueRandomDetect_1, buf );
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }

  /* Scheduler Type (Strict) */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_SCHED_STRICT_ONLY_FEATURE_ID) == L7_FALSE)
  {
    diff = L7_FALSE;
    memset(&queueSchedTypeList, 0, sizeof(queueSchedTypeList));
    if (usmDbQosCosQueueSchedulerTypeListGet(unit, L7_ALL_INTERFACES, &queueSchedTypeList) == L7_SUCCESS)
    {
      memset (buf, 0,sizeof(buf));
      memset (buf2, 0, sizeof(buf2));
      for (queueNum = 0 ; queueNum < L7_MAX_CFG_QUEUES_PER_PORT; queueNum++)
      {
        if (usmDbQosCosQueueIdIndexGet(unit, queueNum) == L7_SUCCESS)
        {
          if ((queueNum <= (L7_MAX_CFG_QUEUES_PER_PORT - 1) && queueSchedTypeList.schedType[queueNum] != FD_QOS_COS_QCFG_SCHED_TYPE)
              || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT )
          {
            diff = L7_TRUE;
            osapiSnprintf(tmp, sizeof(tmp), "%u ", queueNum);
            if( queueSchedTypeList.schedType[queueNum] != FD_QOS_COS_QCFG_SCHED_TYPE)
            {
              strcat(buf, tmp);
            }
            else
            {
              strcat(buf2, tmp);
            }

          }
        }
      }
      memset (stat, 0,sizeof(stat));
      if (strlen(buf2) != 0)
      {
        sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, pStrInfo_qos_NoCosQueueStrict, buf2 );
        EWSWRITEBUFFER(ewsContext, stat);
      }

      memset (stat, 0,sizeof(stat));
      if (strlen(buf) != 0)
      {
        sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, pStrInfo_qos_CosQueueStrict_1, buf );
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }

  diff = L7_FALSE;
  /* Shaping Rate */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_INTF_SHAPING_FEATURE_ID) == L7_TRUE)
  {
    if (usmDbQosCosQueueIntfShapingRateGet(unit, L7_ALL_INTERFACES, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext, val, FD_QOS_COS_QCFG_INTF_SHAPING_RATE, pStrInfo_qos_TrafficShape_2);
    }
  }

  return L7_SUCCESS;

} /* END OF NORMAL COMMAND */

/*********************************************************************
* @purpose  To print the running configuration of per interface cos-queue min-bandwidth
*
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/
L7_RC_t cliRunningConfigInterfaceCosQueue(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tmp[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf2[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 queueNum, interfaceRate, globalRate;
  L7_uint32 diff = L7_FALSE, diff2 = L7_FALSE;

  L7_qosCosQueueBwList_t interfaceQueueBwMinList, globalQueueBwMinList;
  L7_qosCosQueueBwList_t interfaceQueueBwMaxList, globalQueueBwMaxList;
  L7_qosCosQueueSchedTypeList_t interfaceQueueSchedTypeList, globalQueueSchedTypeList;
  L7_qosCosQueueMgmtTypeList_t interfaceQueueMgmtTypeList, globalQueueMgmtTypeList;

  /* Minimum Bandwidth */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MIN_BW_FEATURE_ID) == L7_TRUE &&
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) == L7_TRUE)
  {
    memset(&interfaceQueueBwMinList, 0, sizeof(interfaceQueueBwMinList));
    memset(&globalQueueBwMinList, 0, sizeof(globalQueueBwMinList));
    if(usmDbQosCosQueueMinBandwidthListGet(unit, interface, &interfaceQueueBwMinList) == L7_SUCCESS)
    {
      if(usmDbQosCosQueueMinBandwidthListGet(unit, L7_ALL_INTERFACES, &globalQueueBwMinList) == L7_SUCCESS)
      {
        memset (buf, 0,sizeof(buf));
        for (queueNum = 0 ; queueNum < L7_MAX_CFG_QUEUES_PER_PORT; queueNum++)
        {
          if (usmDbQosCosQueueIdIndexGet(unit, queueNum) == L7_SUCCESS)
          {
            if (interfaceQueueBwMinList.bandwidth[queueNum] != globalQueueBwMinList.bandwidth[queueNum]
                || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT )
            {
              diff = L7_TRUE;
            }
            osapiSnprintf(tmp, sizeof(tmp), "%-u ", interfaceQueueBwMinList.bandwidth[queueNum]);
            strcat (buf, tmp);
          }
        }
        if (diff)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_CosQueueMinBandwidth_1, buf );
          EWSWRITEBUFFER(ewsContext, stat);
        }
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_SCHED_STRICT_ONLY_FEATURE_ID) == L7_FALSE)
  {
    diff = L7_FALSE;
    /* Scheduler Type (Strict) */
    memset(&interfaceQueueSchedTypeList, 0, sizeof(interfaceQueueSchedTypeList));
    memset(&globalQueueSchedTypeList, 0, sizeof(globalQueueSchedTypeList));
    if (usmDbQosCosQueueSchedulerTypeListGet(unit, interface, &interfaceQueueSchedTypeList) == L7_SUCCESS  &&
        usmDbQosCosQueueSchedulerTypeListGet(unit, L7_ALL_INTERFACES, &globalQueueSchedTypeList) == L7_SUCCESS)
    {
      memset (buf, 0,sizeof(buf));
      for (queueNum = 0 ; queueNum < L7_MAX_CFG_QUEUES_PER_PORT; queueNum++)
      {
        if (usmDbQosCosQueueIdIndexGet(unit, queueNum) == L7_SUCCESS)
        {
          if (interfaceQueueSchedTypeList.schedType[queueNum] != FD_QOS_COS_QCFG_SCHED_TYPE)
          {
            if (interfaceQueueSchedTypeList.schedType[queueNum] != globalQueueSchedTypeList.schedType[queueNum]
                || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT )
            {
              diff = L7_TRUE;
            }
            osapiSnprintf(tmp, sizeof(tmp), "%u ", queueNum);
            strcat (buf, tmp);
          }
        }
      }
      if (diff)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_CosQueueStrict_1, buf );
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }

  diff = L7_FALSE;
  /* Shaping Rate */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_INTF_SHAPING_FEATURE_ID) == L7_TRUE)
  {
    if (usmDbQosCosQueueIntfShapingRateGet(unit, interface, &interfaceRate) == L7_SUCCESS  &&
        usmDbQosCosQueueIntfShapingRateGet(unit, L7_ALL_INTERFACES, &globalRate) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,interfaceRate,globalRate,pStrInfo_qos_TrafficShape_2);
    }
  }

  /* Maximum Bandwidth */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MAX_BW_FEATURE_ID) == L7_TRUE &&
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) == L7_TRUE)
  {
    memset(&interfaceQueueBwMaxList, 0, sizeof(interfaceQueueBwMaxList));
    memset(&globalQueueBwMaxList, 0, sizeof(globalQueueBwMaxList));
    if (usmDbQosCosQueueMaxBandwidthListGet(unit, interface, &interfaceQueueBwMaxList) == L7_SUCCESS &&
        usmDbQosCosQueueMaxBandwidthListGet(unit, L7_ALL_INTERFACES, &globalQueueBwMaxList) == L7_SUCCESS)
    {
      memset (buf, 0,sizeof(buf));
      for (queueNum = 0 ; queueNum < L7_MAX_CFG_QUEUES_PER_PORT; queueNum++)
      {
        if (usmDbQosCosQueueIdIndexGet(unit, queueNum) == L7_SUCCESS)
        {
          if (interfaceQueueBwMaxList.bandwidth[queueNum] != globalQueueBwMaxList.bandwidth[queueNum]
              || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT )
          {
            diff = L7_TRUE;
          }
          osapiSnprintf(tmp, sizeof(tmp), "%-u ", interfaceQueueBwMaxList.bandwidth[queueNum]);
          strcat (buf, tmp);
        }
      }
      if (diff)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_CosQueueMaxBandwidth_1, buf );
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }

  /* Management Type (Random-detect) */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_FALSE &&
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) == L7_TRUE && 
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE) 
  {
    diff = L7_FALSE;
    diff2 = L7_FALSE;
    memset(&interfaceQueueMgmtTypeList, 0, sizeof(interfaceQueueMgmtTypeList));
    memset(&globalQueueMgmtTypeList, 0, sizeof(globalQueueMgmtTypeList));
    if (usmDbQosCosQueueMgmtTypeListGet(unit, interface, &interfaceQueueMgmtTypeList) == L7_SUCCESS  &&
        usmDbQosCosQueueMgmtTypeListGet(unit, L7_ALL_INTERFACES, &globalQueueMgmtTypeList) == L7_SUCCESS)
    {
      buf[0] = '\0';
      buf2[0] = '\0';
      for (queueNum = 0 ; queueNum < L7_MAX_CFG_QUEUES_PER_PORT; queueNum++)
      {
        if (usmDbQosCosQueueIdIndexGet(unit, queueNum) == L7_SUCCESS)
        {
	  if (interfaceQueueMgmtTypeList.mgmtType[queueNum] != globalQueueMgmtTypeList.mgmtType[queueNum])
	  {
	    if (interfaceQueueMgmtTypeList.mgmtType[queueNum] == L7_QOS_COS_QUEUE_MGMT_TYPE_WRED)
	    {
	      diff = L7_TRUE;
	      osapiSnprintf(tmp, sizeof(tmp), "%u ", queueNum);
	      strcat (buf, tmp);
	    }
	    else
	    {
	      diff2 = L7_TRUE;
	      osapiSnprintf(tmp, sizeof(tmp), "%u ", queueNum);
	      strcat (buf2, tmp);
	    }
	  }
	}
      }
      if (diff == L7_TRUE)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_CosQueueRandomDetect_1, buf );
        EWSWRITEBUFFER(ewsContext, stat);
      }
      if (diff2 == L7_TRUE)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_NoCosQueueRandomDetect_1, buf2 );
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration
*
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/
L7_RC_t cliRunningConfigGlobalClassofService(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 precedence, dscp, globalVal, defaultTrafficClass;

  /* ClassofService Trust */

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_TRUST_MODE_FEATURE_ID) == L7_TRUE)
  {
    if (usmDbQosCosMapTrustModeGet (unit, L7_ALL_INTERFACES, &globalVal) == L7_SUCCESS &&
        (globalVal != FD_QOS_COS_MAP_INTF_TRUST_MODE || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      memset(buf, 0, sizeof(buf));
      switch (globalVal)
      {
      case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
        if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID) == L7_TRUE)
        {
          strcpy(buf, pStrErr_qos_CfgCosTrustDot1p);
        }
        break;
      case  L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_FEATURE_ID) == L7_TRUE)
        {
          strcpy(buf, pStrErr_qos_CfgCosTrustIpDscp);
        }
        break;
      case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_FEATURE_ID) == L7_TRUE)
        {
          strcpy(buf, pStrErr_qos_CfgCosTrustIpPrecedence);
        }
        break;
      case  L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED:
        strcpy(buf, pStrErr_qos_CfgCosTrustUntrusted);
        break;
      default:
        break;
      }
      if (strlen(buf) > 0)
      {
        /* if the value to be displayed is the factory default, use the
         * 'no' form of the command instead of the normal form
         *
         * NOTE: we're here because either the global mode differs from factory
         *       default, or we are showing everything including defaults
         */
        if (globalVal == FD_QOS_COS_MAP_INTF_TRUST_MODE)
        {
          sprintfAddBlanks (2, 0, 0, 0, L7_NULLPTR, stat, pStrInfo_qos_NoClassofserviceTrust );
        }
        else
        {
          sprintfAddBlanks (2, 0, 0, 0, L7_NULLPTR, stat, pStrInfo_qos_ClassofserviceTrust, buf );
        }
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }

  /* Ip-Dscp */

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_FEATURE_ID) == L7_TRUE)
  {

    for (dscp = 0; dscp < L7_QOS_COS_MAP_NUM_IPDSCP; dscp++)
    {
      if ((usmDbQosCosMapIpDscpTrafficClassGet (unit, L7_ALL_INTERFACES, dscp, &globalVal) == L7_SUCCESS &&
           usmDbQosCosMapIpDscpDefaultTrafficClassGet (unit, L7_ALL_INTERFACES, dscp, &defaultTrafficClass) == L7_SUCCESS) && (globalVal != defaultTrafficClass || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
      {
        osapiSnprintf(buf, sizeof(buf), "%u %u", dscp, globalVal);
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_ClassofserviceIpDscpMapping, buf );
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }
  /* IP-Precedence */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_FEATURE_ID) == L7_TRUE)
  {
    for (precedence = 0; precedence < L7_QOS_COS_MAP_NUM_IPPREC; precedence++)
    {
      if ((usmDbQosCosMapIpPrecTrafficClassGet (unit, L7_ALL_INTERFACES, precedence, &globalVal) == L7_SUCCESS &&
           usmDbQosCosMapIpPrecDefaultTrafficClassGet (unit, L7_ALL_INTERFACES, precedence, &defaultTrafficClass) == L7_SUCCESS) && ( globalVal != defaultTrafficClass || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
      {
        osapiSnprintf(buf, sizeof(buf), "%u %u", precedence, globalVal);
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_ClassofserviceIpPrecedenceMapping, buf );
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration
*
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/
L7_RC_t cliRunningConfigInterfaceClassofService(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 precedence, dscp, val, globalVal;

  /* ClassofService Trust */
  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_TRUST_MODE_FEATURE_ID) == L7_TRUE) &&
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_TRUST_MODE_PER_INTF_FEATURE_ID) == L7_TRUE))
  {
    if ((usmDbQosCosMapTrustModeGet (unit, interface, &val) == L7_SUCCESS &&
         usmDbQosCosMapTrustModeGet (unit, L7_ALL_INTERFACES, &globalVal) == L7_SUCCESS) &&
        (val != globalVal || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      memset(buf, 0, sizeof(buf));
      switch (val)
      {
      case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
        if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID) == L7_TRUE &&
            usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) == L7_TRUE)
        {
          strcpy(buf, pStrErr_qos_CfgCosTrustDot1p);
        }
        break;
      case  L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_FEATURE_ID) == L7_TRUE)
        {
          strcpy(buf, pStrErr_qos_CfgCosTrustIpDscp);
        }
        break;
      case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_FEATURE_ID) == L7_TRUE)
        {
          strcpy(buf, pStrErr_qos_CfgCosTrustIpPrecedence);
        }
        break;
      case  L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED:
        strcpy(buf, pStrErr_qos_CfgCosTrustUntrusted);
        break;
      default:
        break;
      }
      if (strlen(buf) > 0)
      {
        /* if the value to be displayed is the factory default, use the
         * 'no' form of the command instead of the normal form
         *
         * NOTE: we're here because either the interface mode differs from
         *       global mode, or we are showing everything including defaults
         */
        if (val == FD_QOS_COS_MAP_INTF_TRUST_MODE)
        {
          sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, pStrInfo_qos_NoClassofserviceTrust );
        }
        else
        {
          sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, pStrInfo_qos_ClassofserviceTrust, buf );
        }
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }

  /* Ip-Dscp */
  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_FEATURE_ID) == L7_TRUE) &&
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_PER_INTF_FEATURE_ID) == L7_TRUE))
  {
    for (dscp = 0; dscp < L7_QOS_COS_MAP_NUM_IPDSCP; dscp++)
    {
      if ((usmDbQosCosMapIpDscpTrafficClassGet(unit, interface, dscp, &val) == L7_SUCCESS &&
           usmDbQosCosMapIpDscpTrafficClassGet(unit, L7_ALL_INTERFACES, dscp, &globalVal) == L7_SUCCESS) &&
          (val != globalVal || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
      {
        osapiSnprintf(buf, sizeof(buf), "%u %u", dscp, val);
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_ClassofserviceIpDscpMapping, buf );
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }

  /* Ip-Precedence */
  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_FEATURE_ID) == L7_TRUE) &&
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_PER_INTF_FEATURE_ID) == L7_TRUE))
  {
    for (precedence = 0; precedence < L7_QOS_COS_MAP_NUM_IPPREC; precedence++)
    {
      if ((usmDbQosCosMapIpPrecTrafficClassGet(unit, interface, precedence, &val) == L7_SUCCESS &&
           usmDbQosCosMapIpPrecTrafficClassGet(unit, L7_ALL_INTERFACES, precedence, &globalVal) == L7_SUCCESS)&&
          (val != globalVal || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
      {
        osapiSnprintf(buf, sizeof(buf), "%u %u", precedence, val);
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_ClassofserviceIpPrecedenceMapping, buf );
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration of global cos-queue min-bandwidth
*
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/
L7_RC_t cliRunningConfigGlobalRandomDetect(EwsContext ewsContext, L7_uint32 unit)
{
  return cliRunningConfigInterfaceRandomDetect(ewsContext, unit, L7_ALL_INTERFACES);
}

/*********************************************************************
* @purpose  To print the running configuration of global cos-queue random-detect
*
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
* @param    L7_uint32 interface (intIfNum or L7_ALL_INTERFACES)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/
L7_RC_t cliRunningConfigInterfaceRandomDetect(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface)
{
    L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 tmp[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 thisQueueSpec[L7_CLI_MAX_STRING_LENGTH];
    L7_uint32 queueId, precLevel, val, globalVal, checkQueueId;
    L7_qosCosDropParmsList_t dList, globalDList;
    L7_cosCfgParms_t defCfg;
    L7_BOOL same, diff = L7_FALSE;
    L7_char8 skipThisQueue[L7_MAX_CFG_QUEUES_PER_PORT];

    /* Random Detect */
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_TRUE &&
        usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE)
    {
      if ((usmDbQosCosQueueMgmtTypePerIntfGet(unit, interface, &val) == L7_SUCCESS) &&
          (((val != FD_QOS_COS_QCFG_MGMT_TYPE) || (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))) && 
           ((interface == L7_ALL_INTERFACES) ||
            (usmDbQosCosQueueMgmtTypePerIntfGet(unit, L7_ALL_INTERFACES, &globalVal) != L7_SUCCESS) ||
            (val != globalVal)))
      {
        memset (buf, 0,sizeof(buf));
        switch (val)
        {
        case L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP:
          sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrInfo_qos_NoRandomDetect_1);
          break;
        case L7_QOS_COS_QUEUE_MGMT_TYPE_WRED:
          sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrInfo_qos_RandomDetect_2);
          break;
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

    /* Random Detect Exponential Weighting Constant */
    if ((usmDbQosCosQueueWredDecayExponentGet(unit, interface, &val) == L7_SUCCESS) &&
        ((val != FD_QOS_COS_QCFG_WRED_DECAY_EXP) || (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)) && 
        ((interface == L7_ALL_INTERFACES) || 
        (usmDbQosCosQueueWredDecayExponentGet(unit, L7_ALL_INTERFACES, &globalVal) != L7_SUCCESS) || 
        (val != globalVal)))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_RandomDetectExponentialWeightingConstant_1, val );
      EWSWRITEBUFFER(ewsContext, buf);
    }

    /* Random Detect Queue Parms */
    memset(skipThisQueue, 0, sizeof(skipThisQueue));
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE)
    {
      if ((interface != L7_ALL_INTERFACES) || (usmDbQosCosQueueDefaultConfigGet(unit, interface, &defCfg) == L7_SUCCESS))
      {
          if (usmDbQosCosQueueDropParmsListGet(unit, interface, &dList) == L7_SUCCESS)
          {
            if ((interface == L7_ALL_INTERFACES) || 
                (usmDbQosCosQueueDropParmsListGet(unit, L7_ALL_INTERFACES, &globalDList) == L7_SUCCESS)) 
            {
                for (queueId = 0; queueId < L7_MAX_CFG_QUEUES_PER_PORT; queueId++)
                {
                    if (skipThisQueue[queueId] != 0) 
                    {
                      continue;
                    }
                    if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) && 
			(interface == L7_ALL_INTERFACES))
                    {
                      diff = L7_TRUE;
                    }
                    else
                    {
                      diff = L7_FALSE;
                    }
                    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_MinThresh);
                    for (precLevel = 0; precLevel < (L7_MAX_CFG_DROP_PREC_LEVELS+1); precLevel++)
                    {
                      if (((interface == L7_ALL_INTERFACES) && 
			  (dList.queue[queueId].minThreshold[precLevel] != 
                           defCfg.queue[queueId].dropPrec[precLevel].wredMinThresh)) ||
			  ((interface != L7_ALL_INTERFACES) && 
			   (dList.queue[queueId].minThreshold[precLevel] != 
			    globalDList.queue[queueId].minThreshold[precLevel])))
                      {
                        diff = L7_TRUE;
                      }
                    osapiSnprintf(tmp, sizeof(tmp), "%u ", dList.queue[queueId].minThreshold[precLevel]);
                    strcat(buf, tmp);
                    }

                    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_MAX_THRESH_FEATURE_ID) == L7_TRUE)
                    {
                      strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_MaxThresh_2);
                      for (precLevel = 0; precLevel < (L7_MAX_CFG_DROP_PREC_LEVELS+1); precLevel++)
                      {
                        if (((interface == L7_ALL_INTERFACES) && 
			     (dList.queue[queueId].wredMaxThreshold[precLevel] != 
			      defCfg.queue[queueId].dropPrec[precLevel].wredMaxThresh)) ||
			    ((interface != L7_ALL_INTERFACES) && 
			     (dList.queue[queueId].wredMaxThreshold[precLevel] != 
			      globalDList.queue[queueId].wredMaxThreshold[precLevel])))
                        {
			  diff = L7_TRUE;
                        }
                        osapiSnprintf(tmp, sizeof(tmp), "%u ", dList.queue[queueId].wredMaxThreshold[precLevel]);
                        strcat(buf, tmp);
                      }
                    }
    
                    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_DropProb_1);
                    for (precLevel = 0; precLevel < (L7_MAX_CFG_DROP_PREC_LEVELS+1); precLevel++)
                    {
                      if (((interface == L7_ALL_INTERFACES) && 
			  (dList.queue[queueId].dropProb[precLevel] != 
                           defCfg.queue[queueId].dropPrec[precLevel].wredDropProb)) ||
			  ((interface != L7_ALL_INTERFACES) && 
			   (dList.queue[queueId].dropProb[precLevel] != 
			    globalDList.queue[queueId].dropProb[precLevel])))
                      {
                        diff = L7_TRUE;
                      }
                      osapiSnprintf(tmp, sizeof(tmp), "%u ", dList.queue[queueId].dropProb[precLevel]);
                      strcat(buf, tmp);
                    }
                    if (diff == L7_TRUE)
                    {
                      osapiSnprintf(thisQueueSpec, sizeof(thisQueueSpec), "%u ", queueId);
                      /* Collapse identically-configured queues into one line, to save RPC 
                         calls at bootup time, when config is applied */
                      checkQueueId = queueId + 1;
                      while (checkQueueId < L7_MAX_CFG_QUEUES_PER_PORT) 
                      {
                        same = L7_TRUE;
                        if (skipThisQueue[checkQueueId] == 0)
                        {
                            for (precLevel = 0; precLevel < (L7_MAX_CFG_DROP_PREC_LEVELS+1); precLevel++) 
                            {
                                if ((dList.queue[queueId].minThreshold[precLevel] != 
                                     dList.queue[checkQueueId].minThreshold[precLevel]) || 
                                    (dList.queue[queueId].wredMaxThreshold[precLevel] != 
                                     dList.queue[checkQueueId].wredMaxThreshold[precLevel]) || 
                                    (dList.queue[queueId].dropProb[precLevel] != 
                                     dList.queue[checkQueueId].dropProb[precLevel])) 
                                  {
                                    same = L7_FALSE;
                                    break;
                                  }
                            }
                            if (same == L7_TRUE) 
                            {
			      /* Check to see that this queue's config is
				 still different than the global config. */
			      if (interface != L7_ALL_INTERFACES)
			      {
				for (precLevel = 0; precLevel < (L7_MAX_CFG_DROP_PREC_LEVELS+1); precLevel++)
				{
				  if ((dList.queue[checkQueueId].minThreshold[precLevel] == 
				       globalDList.queue[checkQueueId].minThreshold[precLevel]) && 
				      (dList.queue[checkQueueId].wredMaxThreshold[precLevel] == 
				       globalDList.queue[checkQueueId].wredMaxThreshold[precLevel]) && 
				      (dList.queue[checkQueueId].dropProb[precLevel] == 
				       globalDList.queue[checkQueueId].dropProb[precLevel]))
				  {
				    same = L7_FALSE;
				    break;
				  }
				}
			      }
			      if (same == L7_TRUE)
			      {
				osapiSnprintf(tmp, sizeof(tmp), "%u ", checkQueueId);
				strcat(thisQueueSpec, tmp);
				skipThisQueue[checkQueueId] = 1;
			      }
                            }
                        }
                        checkQueueId++;
                      } /* End collapse-indentically-configured-queues loop */
                      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_RandomDetectQueueParms_2, thisQueueSpec, buf );
                      EWSWRITEBUFFER(ewsContext, stat);
                    } /* End if not default config */
                } /* for each queue */
            } 
          } 
      } 
    } 
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration of global cos-queue tail-drop
*
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/
L7_RC_t cliRunningConfigGlobalTailDrop(EwsContext ewsContext, L7_uint32 unit)
{
  return cliRunningConfigInterfaceTailDrop(ewsContext, unit, L7_ALL_INTERFACES);
}

/*********************************************************************
* @purpose  To print the running configuration of global cos-queue tail-drop
*
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
* @param    L7_uint32 interface (intIfNum or L7_ALL_INTERFACES)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/
L7_RC_t cliRunningConfigInterfaceTailDrop(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface)
{
  L7_char8 finished_string[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 this_parmset[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tmp[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 this_queueset[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 queueId, precLevel, checkQueueId;
  L7_BOOL diff;
  L7_BOOL same;
  L7_qosCosDropParmsList_t dropList, globalDropList;
  L7_cosCfgParms_t defCfg;
  L7_char8 skipThisQueue[L7_MAX_CFG_QUEUES_PER_PORT];

  memset(skipThisQueue, 0, sizeof(skipThisQueue));

  if ((interface != L7_ALL_INTERFACES) || (usmDbQosCosQueueDefaultConfigGet(unit, interface, &defCfg) == L7_SUCCESS))
  {
    if (usmDbQosCosQueueDropParmsListGet(unit, interface, &dropList) == L7_SUCCESS)
    {
      if ((interface == L7_ALL_INTERFACES) || 
          (usmDbQosCosQueueDropParmsListGet(unit, L7_ALL_INTERFACES, &globalDropList) == L7_SUCCESS)) 
      {
          for (queueId = 0; queueId < L7_MAX_CFG_QUEUES_PER_PORT; queueId++)
          {
            this_queueset[0] = '\0';
            this_parmset[0] = '\0';
            diff = L7_FALSE;
            if (skipThisQueue[queueId] != 0) 
            {
              continue;
            }
            if ((interface == L7_ALL_INTERFACES) && (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
            {
              diff = L7_TRUE;
            }
            for (precLevel = 0; precLevel < (L7_MAX_CFG_DROP_PREC_LEVELS+1); precLevel++)
            {	      
              if ((interface == L7_ALL_INTERFACES) && 
		  (dropList.queue[queueId].tailDropMaxThreshold[precLevel] != 
		   defCfg.queue[queueId].dropPrec[precLevel].tdropThresh)) 
              {
		diff = L7_TRUE;
	      }
	      if ((interface != L7_ALL_INTERFACES) && 
		  (dropList.queue[queueId].tailDropMaxThreshold[precLevel] != 
		   globalDropList.queue[queueId].tailDropMaxThreshold[precLevel]))
              {
		diff = L7_TRUE;
	      }
              osapiSnprintf(tmp, sizeof(tmp), "%u ", dropList.queue[queueId].tailDropMaxThreshold[precLevel]);
              strcat(this_parmset, tmp);
            }
            if (diff == L7_TRUE) 
            { /* We will have something to print for this queue. See if any other queues are configured 
                 identically. If so, add them to this command line. This makes config more compact, 
                 and quicker to apply at bootup (potentially fewer RPCs). */
                osapiSnprintf(this_queueset, sizeof(this_queueset), "%u ", queueId);
                checkQueueId = queueId + 1;
                while (checkQueueId < L7_MAX_CFG_QUEUES_PER_PORT) 
                {
                    if (skipThisQueue[checkQueueId] == 0)
                    {
                        same = L7_TRUE;
                        for (precLevel = 0; precLevel < (L7_MAX_CFG_DROP_PREC_LEVELS+1); precLevel++)
                        {
                            if (dropList.queue[queueId].tailDropMaxThreshold[precLevel] != 
                                dropList.queue[checkQueueId].tailDropMaxThreshold[precLevel]) 
                            {
                                same = L7_FALSE;
                                break;
                            }
                        }
                        if (same == L7_TRUE)
                        {
                            skipThisQueue[checkQueueId] = 1;
                            osapiSnprintf(tmp, sizeof(tmp), "%u ", checkQueueId);
                            strcat(this_queueset, tmp);
                        }
                    }
                    checkQueueId++;
                }
            }
            if (diff == L7_TRUE)
            {
                this_queueset[strlen(this_queueset)-1] = '\0';
                sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, finished_string, pStrInfo_qos_TailDropQueueParms_2, 
                                  this_queueset, pStrInfo_qos_Thresh_2, this_parmset );
                EWSWRITEBUFFER(ewsContext, finished_string);
            }
          } /* End for each queue */
      }
    }
  }
  return L7_SUCCESS;
}
#endif /* End if COS package include */
