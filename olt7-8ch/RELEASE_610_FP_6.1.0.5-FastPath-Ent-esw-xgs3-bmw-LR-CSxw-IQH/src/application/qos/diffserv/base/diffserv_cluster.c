/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  diffserv_cluster.c
*
* @purpose   Diffserv support for clustering
*
* @component QoS - Diffserv
*
* @comments
*
* @create    02/28/2008
*
* @author    gpaussa
*
* @end
*
**********************************************************************/
#include <string.h>
#include "commdefs.h"
#include "osapi.h"
#include "osapi_support.h"
#include "log.h"
#include "nvstoreapi.h"
#include "l7utils_api.h"
#include "comm_mask.h"
#include "avl_api.h"
#include "clustering_commdefs.h"
#include "clustering_api.h"
#include "l7_diffserv_api.h"
#include "diffserv_config.h"
#include "diffserv_prvtmib.h"
#include "diffserv_util.h"
#include "diffserv_cluster.h"
#include "usmdb_mib_diffserv_private_api.h"

extern char *dsmibPrvtTableIdStr[];
extern L7_uchar8  policyAttrIpDscpValSupported[];

static L7_diffServCfg_t *dsClusterTxCfg   = L7_NULLPTR;
static L7_diffServCfg_t *dsClusterRxCfg   = L7_NULLPTR;
static L7_diffServCfg_t *dsClusterTmpCfg  = L7_NULLPTR;

static L7_BOOL          dsClusterRxCfgMsgStarted  = L7_FALSE;               /* received Config Start IE msg, but not Config End */
static dsClusterMsgIe_t dsClusterRxCfgIeExpected  = DS_CLUSTER_MSG_IE_NONE; /* next expected IE in a defined sequence */

/* copy of data received from cluster member switch */
static L7_BOOL  dsClusterRxInProg   = L7_FALSE; /* indicates Rx operation currently in progress */
static L7_BOOL  dsClusterRxError    = L7_FALSE; /* indicates unrecoverable error receiving, drop remaining messages */

static L7_int32 dsClusterTaskId     = L7_ERROR;
static void     *dsClusterQueue     = L7_NULLPTR;
static L7_BOOL  dsClusterDebug      = L7_FALSE;

static L7_COMPONENT_IDS_t dsCompId  = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;

static dsClusterTableParms_t dsClusterTableParms[DSMIB_TABLE_ID_TOTAL] = 
{
  { /* none (placeholder) */
    0, 0, 0, 0, 0, 0, 0, 0, L7_NULLPTR, L7_NULLPTR
  },
  { /* class table */
    L7_DIFFSERV_CLASS_HDR_START, 
    L7_DIFFSERV_CLASS_HDR_MAX,
    DS_CLUSTER_MSG_IE_CLASS_CONFIG_START, 
    DS_CLUSTER_MSG_IE_CLASS_CONFIG_START_SIZE,
    DS_CLUSTER_MSG_IE_CLASS_DEF, 
    DS_CLUSTER_MSG_IE_CLASS_DEF_SIZE,
    DS_CLUSTER_MSG_IE_CLASS_CONFIG_END, 
    DS_CLUSTER_MSG_IE_CLASS_CONFIG_END_SIZE,
    diffServClusterConfigClassMsgAccess,
    diffServClusterConfigClassVerify
  },
  { /* class rule table */
    L7_DIFFSERV_CLASS_RULE_START, 
    L7_DIFFSERV_CLASS_RULE_MAX,
    DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_START, 
    DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_START_SIZE,
    DS_CLUSTER_MSG_IE_CLASS_RULE_DEF, 
    DS_CLUSTER_MSG_IE_CLASS_RULE_DEF_SIZE,
    DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_END, 
    DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_END_SIZE,
    diffServClusterConfigClassRuleMsgAccess,
    diffServClusterConfigClassRuleVerify
  },
  { /* policy table */
    L7_DIFFSERV_POLICY_HDR_START, 
    L7_DIFFSERV_POLICY_HDR_MAX,
    DS_CLUSTER_MSG_IE_POLICY_CONFIG_START, 
    DS_CLUSTER_MSG_IE_POLICY_CONFIG_START_SIZE,
    DS_CLUSTER_MSG_IE_POLICY_DEF, 
    DS_CLUSTER_MSG_IE_POLICY_DEF_SIZE,
    DS_CLUSTER_MSG_IE_POLICY_CONFIG_END, 
    DS_CLUSTER_MSG_IE_POLICY_CONFIG_END_SIZE,
    diffServClusterConfigPolicyMsgAccess,
    diffServClusterConfigPolicyVerify
  },
  { /* policy inst table */
    L7_DIFFSERV_POLICY_INST_START, 
    L7_DIFFSERV_POLICY_INST_MAX,
    DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_START, 
    DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_START_SIZE,
    DS_CLUSTER_MSG_IE_POLICY_INST_DEF, 
    DS_CLUSTER_MSG_IE_POLICY_INST_DEF_SIZE,
    DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_END, 
    DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_END_SIZE,
    diffServClusterConfigPolicyInstMsgAccess,
    diffServClusterConfigPolicyInstVerify
  },
  { /* policy attr table */
    L7_DIFFSERV_POLICY_ATTR_START, 
    L7_DIFFSERV_POLICY_ATTR_MAX,
    DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_START, 
    DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_START_SIZE,
    DS_CLUSTER_MSG_IE_POLICY_ATTR_DEF, 
    DS_CLUSTER_MSG_IE_POLICY_ATTR_DEF_SIZE,
    DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_END, 
    DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_END_SIZE,
    diffServClusterConfigPolicyAttrMsgAccess,
    diffServClusterConfigPolicyAttrVerify
  },
  { /* policy perf in (not used here) */
    0, 0, 0, 0, 0, 0, 0, 0, L7_NULLPTR, L7_NULLPTR
  },
  { /* policy perf out (not used here) */
    0, 0, 0, 0, 0, 0, 0, 0, L7_NULLPTR, L7_NULLPTR
  },
  { /* service (not used here) */
    0, 0, 0, 0, 0, 0, 0, 0, L7_NULLPTR, L7_NULLPTR
  }
};

/* internal functions */

static void _dsClusterPktInt32GetOrSet(L7_BOOL isSet, L7_uint32 *value,
                                       L7_uchar8 *buffer, L7_uint32 *offset)
{
  if (L7_TRUE == isSet)
  {
    CLUSTER_PKT_INT32_SET(*value, buffer, *offset);
  }
  else
  {
    CLUSTER_PKT_INT32_GET(*value, buffer, *offset);
  }
}

static void _dsClusterPktInt16GetOrSet(L7_BOOL isSet, L7_ushort16 *value,
                                       L7_uchar8 *buffer, L7_uint32 *offset)
{
  if (L7_TRUE == isSet)
  {
    CLUSTER_PKT_INT16_SET(*value, buffer, *offset);
  }
  else
  {
    CLUSTER_PKT_INT16_GET(*value, buffer, *offset);
  }
}

static void _dsClusterPktInt8GetOrSet(L7_BOOL isSet, L7_uchar8 *value,
                                      L7_uchar8 *buffer, L7_uint32 *offset)
{
  if (L7_TRUE == isSet)
  {
    CLUSTER_PKT_INT8_SET(*value, buffer, *offset);
  }
  else
  {
    CLUSTER_PKT_INT8_GET(*value, buffer, *offset);
  }
}

static void _dsClusterPktStringGetOrSet(L7_BOOL isSet, L7_uchar8 *str, L7_uint32 len,
                                        L7_uchar8 *buffer, L7_uint32 *offset)
{
  if (L7_TRUE == isSet)
  {
    CLUSTER_PKT_STRING_SET(str, len, buffer, *offset);
  }
  else
  {
    CLUSTER_PKT_STRING_GET(str, len, buffer, *offset);
  }
}

static void _dsClusterCfgStructInit(L7_diffServCfg_t *pCfg, L7_char8 *pDescrStr)
{
  if (L7_NULLPTR != pCfg)
  {
    memset(pCfg, 0, sizeof(*pCfg));

    /* fill in file hdr fields */
    if (L7_NULLPTR != pDescrStr)
    {
      snprintf(pCfg->cfgHdr.filename, sizeof(pCfg->cfgHdr.filename), "%s", pDescrStr);
    }
    pCfg->cfgHdr.version          = L7_DIFFSERV_CFG_VER_CURRENT;
    pCfg->cfgHdr.componentID      = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    pCfg->cfgHdr.type             = L7_CFG_DATA;
    pCfg->cfgHdr.length           = (L7_uint32)L7_DIFFSERV_CFG_DATA_SIZE;
    pCfg->cfgHdr.dataChanged      = L7_FALSE;
    pCfg->cfgHdr.fileHdrVersion   = L7_FILE_HDR_VER_CURRENT;
  }
}

static L7_BOOL _dsClusterConfigEntryInUseGet(dsmibTableId_t    tableID,
                                             L7_diffServCfg_t  *pCfg,
                                             L7_uint32         index)
{
  L7_BOOL inUse = L7_FALSE;

  switch (tableID)
  {
    case DSMIB_TABLE_ID_CLASS:
      inUse = pCfg->classHdr[index].inUse;
      break;

    case DSMIB_TABLE_ID_CLASS_RULE:
      inUse = (0 == pCfg->classRule[index].hdrIndexRef) ? L7_FALSE : L7_TRUE;
      break;

    case DSMIB_TABLE_ID_POLICY:
      inUse = pCfg->policyHdr[index].inUse;
      break;

    case DSMIB_TABLE_ID_POLICY_INST:
      inUse = (0 == pCfg->policyInst[index].hdrIndexRef) ? L7_FALSE : L7_TRUE;
      break;

    case DSMIB_TABLE_ID_POLICY_ATTR:
      inUse = (0 == pCfg->policyAttr[index].instIndexRef) ? L7_FALSE : L7_TRUE;
      break;

    default:
      /* table ID not suppoorted here */
      break;

  } /* endswitch */

  return inUse;
}

static L7_ushort16 _dsClusterIeDefinedSizeGet(L7_ushort16 elemId)
{
  L7_ushort16       elemSize = 0;

  switch (elemId)
  {
    case DS_CLUSTER_MSG_IE_CONFIG_START:
      elemSize = DS_CLUSTER_MSG_IE_CONFIG_START_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_GLOBAL_CONFIG:
      elemSize = DS_CLUSTER_MSG_IE_GLOBAL_CONFIG_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_CLASS_CONFIG_START:
      elemSize = DS_CLUSTER_MSG_IE_CLASS_CONFIG_START_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_CLASS_DEF:
      elemSize = DS_CLUSTER_MSG_IE_CLASS_DEF_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_CLASS_CONFIG_END:
      elemSize = DS_CLUSTER_MSG_IE_CLASS_CONFIG_END_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_START:
      elemSize = DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_START_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_CLASS_RULE_DEF:
      elemSize = DS_CLUSTER_MSG_IE_CLASS_RULE_DEF_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_END:
      elemSize = DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_END_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_POLICY_CONFIG_START:
      elemSize = DS_CLUSTER_MSG_IE_POLICY_CONFIG_START_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_POLICY_DEF:
      elemSize = DS_CLUSTER_MSG_IE_POLICY_DEF_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_POLICY_CONFIG_END:
      elemSize = DS_CLUSTER_MSG_IE_POLICY_CONFIG_END_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_START:
      elemSize = DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_START_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_POLICY_INST_DEF:
      elemSize = DS_CLUSTER_MSG_IE_POLICY_INST_DEF_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_END:
      elemSize = DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_END_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_START:
      elemSize = DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_START_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_POLICY_ATTR_DEF:
      elemSize = DS_CLUSTER_MSG_IE_POLICY_ATTR_DEF_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_END:
      elemSize = DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_END_SIZE;
      break;

    case DS_CLUSTER_MSG_IE_CONFIG_END:
      elemSize = DS_CLUSTER_MSG_IE_CONFIG_END_SIZE;
      break;

    default:
      /* invalid element ID */
      break;

  } /* endswitch */

  return elemSize;
}

static dsClusterIeGroup_t _dsClusterIeGroupTypeGet(L7_ushort16 elemId)
{
  dsClusterIeGroup_t    groupType = DS_CLUSTER_IE_GROUP_NONE;

  switch (elemId)
  {
    case DS_CLUSTER_MSG_IE_CLASS_CONFIG_START:
    case DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_START:
    case DS_CLUSTER_MSG_IE_POLICY_CONFIG_START:
    case DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_START:
    case DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_START:
      groupType = DS_CLUSTER_IE_GROUP_START;
      break;

    case DS_CLUSTER_MSG_IE_CLASS_DEF:
    case DS_CLUSTER_MSG_IE_CLASS_RULE_DEF:
    case DS_CLUSTER_MSG_IE_POLICY_DEF:
    case DS_CLUSTER_MSG_IE_POLICY_INST_DEF:
    case DS_CLUSTER_MSG_IE_POLICY_ATTR_DEF:
      groupType = DS_CLUSTER_IE_GROUP_DEF;
      break;

    case DS_CLUSTER_MSG_IE_CLASS_CONFIG_END:
    case DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_END:
    case DS_CLUSTER_MSG_IE_POLICY_CONFIG_END:
    case DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_END:
    case DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_END:
      groupType = DS_CLUSTER_IE_GROUP_END;
      break;

    case DS_CLUSTER_MSG_IE_CONFIG_START:
    case DS_CLUSTER_MSG_IE_GLOBAL_CONFIG:
    case DS_CLUSTER_MSG_IE_CONFIG_END:
    default:
      groupType = DS_CLUSTER_IE_GROUP_NONE;
      break;

  } /* endswitch */

  return groupType;
}

static dsmibTableId_t _dsClusterIeTableIdGet(L7_ushort16 elemId)
{
  dsmibTableId_t        tableID = DSMIB_TABLE_ID_NONE;

  switch (elemId)
  {
    case DS_CLUSTER_MSG_IE_CLASS_CONFIG_START:
    case DS_CLUSTER_MSG_IE_CLASS_DEF:
    case DS_CLUSTER_MSG_IE_CLASS_CONFIG_END:
      tableID = DSMIB_TABLE_ID_CLASS;
      break;

    case DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_START:
    case DS_CLUSTER_MSG_IE_CLASS_RULE_DEF:
    case DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_END:
      tableID = DSMIB_TABLE_ID_CLASS_RULE;
      break;

    case DS_CLUSTER_MSG_IE_POLICY_CONFIG_START:
    case DS_CLUSTER_MSG_IE_POLICY_DEF:
    case DS_CLUSTER_MSG_IE_POLICY_CONFIG_END:
      tableID = DSMIB_TABLE_ID_POLICY;
      break;

    case DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_START:
    case DS_CLUSTER_MSG_IE_POLICY_INST_DEF:
    case DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_END:
      tableID = DSMIB_TABLE_ID_POLICY_INST;
      break;

    case DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_START:
    case DS_CLUSTER_MSG_IE_POLICY_ATTR_DEF:
    case DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_END:
      tableID = DSMIB_TABLE_ID_POLICY_ATTR;
      break;

    default:
      break;

  } /* endswitch */

  return tableID;
}

dsmibPolicyAttrType_t _dsClusterPoliceActToEntryType(dsmibPoliceAct_t policeAct)
{
  dsmibPolicyAttrType_t       entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;

  switch (policeAct)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2;    
      break;
      
    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
    default:
      /* nothing specific to translate to */
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
      break;

  } /* endswitch*/

  return entryType;
}

/*********************************************************************
* @purpose  Show DiffServ cluster internal variables for support/debug
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes 
*           
* @end
*********************************************************************/
L7_RC_t diffServClusterDebugShow(void)
{
  sysapiPrintf("\nDiffServ cluster support data:\n");
  sysapiPrintf("  dsClusterTxCfg           = %p\n",      dsClusterTxCfg);
  sysapiPrintf("  dsClusterRxCfg           = %p\n",      dsClusterRxCfg);
  sysapiPrintf("  dsClusterTmpCfg          = %p\n",      dsClusterTmpCfg);
  sysapiPrintf("  dsClusterRxCfgMsgStarted = %d\n",      dsClusterRxCfgMsgStarted);
  sysapiPrintf("  dsClusterRxCfgIeExpected = 0x%4.4x\n", dsClusterRxCfgIeExpected);
  sysapiPrintf("  dsClusterRxInProg        = %d\n",      dsClusterRxInProg);
  sysapiPrintf("  dsClusterRxError         = %d\n",      dsClusterRxError);
  sysapiPrintf("  dsClusterTaskId          = 0x%x\n",    dsClusterTaskId);
  sysapiPrintf("  dsClusterQueue           = 0x%x\n",    (L7_uint32)dsClusterQueue);
  sysapiPrintf("  dsClusterDebug           = %d\n",      dsClusterDebug);
  sysapiPrintf("\n");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Perform all phase 1 initialization for clustering support
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes 
*           
* @end
*********************************************************************/
L7_RC_t diffServClusterPhase1Init(void)
{
  L7_uint32         dataSize;

  /* allocate memory to send and receive DiffServ configuration.
   *
   * NOTE: Must be able to receive a copy of the entire config in order
   *       to verify it before it is applied.  Must be able to abort the
   *       receive processing without affecting the current operational
   *       or saved config.
   */
  dataSize = sizeof(L7_diffServCfg_t);
  dsClusterTxCfg  = osapiMalloc(dsCompId, dataSize);
  dsClusterRxCfg  = osapiMalloc(dsCompId, dataSize);
  dsClusterTmpCfg = osapiMalloc(dsCompId, dataSize);
  if ((L7_NULLPTR == dsClusterTxCfg) ||
      (L7_NULLPTR == dsClusterRxCfg) ||
      (L7_NULLPTR == dsClusterTmpCfg))
  {
    LOG_MSG("%s: Unable to allocate memory for Tx, Rx and Tmp config structs (%u bytes each).\n",
            __FUNCTION__, dataSize);
    return L7_FAILURE;
  }
  memset(dsClusterTxCfg, 0, dataSize);
  memset(dsClusterRxCfg, 0, dataSize);
  memset(dsClusterTmpCfg, 0, dataSize);

  dsClusterQueue = (void *)osapiMsgQueueCreate("dsClusterQueue", 
                                               DS_CLUSTER_QUEUE_SIZE,
                                               DS_CLUSTER_QUEUE_SIZE * sizeof(dsClusterQueueMsg_t));
  if (L7_NULLPTR == dsClusterQueue)
  {
    LOG_MSG("%s: osapiMsgQueueCreate failed.\n", __FUNCTION__);
    return L7_FAILURE;
  }

  dsClusterTaskId = osapiTaskCreate("dsClusterTask", (void *)diffServClusterTask,
                                    0, 0, 
                                    L7_DEFAULT_STACK_SIZE,
                                    L7_DEFAULT_TASK_PRIORITY,
                                    L7_DEFAULT_TASK_SLICE);

  if ((L7_ERROR == dsClusterTaskId) ||
      (osapiWaitForTaskInit(L7_QOS_DIFFSERV_CLUSTER_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS))
  {
    LOG_MSG("%s: osapiTaskCreate failed.\n", __FUNCTION__);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Perform all phase 2 initialization for clustering support
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes 
*           
* @end
*********************************************************************/
L7_RC_t diffServClusterPhase2Init(void)
{
  if (clusterMsgRegister(DS_CLUSTER_MSG_CONFIG, 
                         CLUSTER_MSG_DELIVERY_RELIABLE,
                         diffServClusterMsgCallback) != L7_SUCCESS)
  {
    LOG_MSG("%s: clusterMsgRegister failed.\n", __FUNCTION__);
    return L7_FAILURE;
  }

  if (clusterConfigRegister(CLUSTER_CFG_ID(QOS_DIFFSERV),
                            diffServClusterConfigSendCallback,
                            diffServClusterConfigRxCallback) != L7_SUCCESS)
  {
    LOG_MSG("%s: clusterConfigRegister failed.\n", __FUNCTION__);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Task to wait on cluster messages and events
*
* @param    void
*
* @returns  void
*
* @notes    Clustering requires processing messages and commands in 
*           a separate task context, a component must not block the
*           clustering task. 
*           
* @end
*********************************************************************/
void diffServClusterTask(void)
{
  dsClusterQueueMsg_t queueMsg;
  L7_RC_t             rc;

  osapiTaskInitDone(L7_QOS_DIFFSERV_CLUSTER_TASK_SYNC);

  while (L7_TRUE)
  {
    if (osapiMessageReceive(dsClusterQueue, 
                            (void *)&queueMsg, sizeof(dsClusterQueueMsg_t),
                            L7_WAIT_FOREVER) == L7_SUCCESS)
    {
      switch (queueMsg.type)
      {
        case DS_CLUSTER_QUEUE_MSG_CONFIG_MSG:
          if (L7_FALSE == dsClusterRxError)
          {
            diffServClusterConfigMsgProcess(&queueMsg.content.msg);
          }
          break;
        case DS_CLUSTER_QUEUE_MSG_CONFIG_SEND:
          rc = diffServClusterConfigSend(&queueMsg.content.memberID);
          if (clusterConfigSendDone(CLUSTER_CFG_ID(QOS_DIFFSERV), rc) != L7_SUCCESS)
          {
            LOG_MSG("%s: clusterConfigSendDone failed\n", __FUNCTION__);
          }
          break;
        case DS_CLUSTER_QUEUE_MSG_CONFIG_RX:
          diffServClusterConfigRxCmdProcess(queueMsg.content.configCmd);
          break;
        default:
          LOG_MSG("%s: received invalid message, queueMsg.type=%u\n", 
                  __FUNCTION__, queueMsg.type);
          break;
      }
    }
  }
}

/*********************************************************************
* @purpose  Callback to handle messages received through clustering
*
* @param    L7_ushort16              msgType   @b{(input)} message type
* @param    clusterMsgDeliveryMethod method    @b{(input)} delivery method
* @param    L7_uint32                msgLength @b{(input)} message length
* @param    L7_uchar8               *msgBuf    @b{(input)} contents of message
*
* @returns  void
*
* @notes    This function should NOT introduce significant amounts 
*           of latency (e.g. by blocking), since it will be called
*           in the context of the clustering task.  It should
*           copy the message and signal another task to
*           deal with the message's reception.
* @end
*********************************************************************/
void diffServClusterMsgCallback(L7_ushort16              msgType,
                                clusterMsgDeliveryMethod method,
                                L7_uint32                msgLength,
                                L7_uchar8               *msgBuf)
{
  dsClusterQueueMsg_t queueMsg;

  DIFFSERV_CLUSTER_DLOG("%s: msgType=0x%4.4X\n", __FUNCTION__, msgType);

  if (msgLength > sizeof(queueMsg.content.msg.buffer))
  {
    LOG_MSG("%s: Received invalid cluster message length, msgLength=%u\n", 
            __FUNCTION__, msgLength);
    return;
  }

  memset(&queueMsg, 0, sizeof(dsClusterQueueMsg_t));
  switch (msgType)
  {
    case DS_CLUSTER_MSG_CONFIG:
      queueMsg.type = DS_CLUSTER_QUEUE_MSG_CONFIG_MSG;
      queueMsg.content.msg.length = msgLength;
      memcpy(queueMsg.content.msg.buffer, msgBuf, msgLength);
      break;
    default:
      LOG_MSG("%s: Received invalid cluster message type, msgType=0x%4.4x\n", 
              __FUNCTION__, msgType);
      return;
  }

  /* send message to process on diffServClusterTask */
  if (osapiMessageSend(dsClusterQueue, (void *)&queueMsg, 
                       sizeof(dsClusterQueueMsg_t), 
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("%s: osapiMessageSend failed, queueMsg.type=%u\n", 
            __FUNCTION__, queueMsg.type);
  }

  return;
}

/*********************************************************************
* @purpose  Callback to send configuration information to a cluster member
*
* @param    clusterMemberID_t *memberID @b{(input)} member to receive config
*
* @returns  void
*
* @notes     
*           
* @end
*********************************************************************/
void diffServClusterConfigSendCallback(clusterMemberID_t *memberID)
{
  dsClusterQueueMsg_t queueMsg;

  DIFFSERV_CLUSTER_DLOG("ENTERING %s\n", __FUNCTION__);

  if (L7_NULLPTR == memberID)
  {
    LOG_MSG("%s: received NULL cluster member ID\n", __FUNCTION__);
    return;
  }

  memset(&queueMsg, 0, sizeof(dsClusterQueueMsg_t));
  queueMsg.type = DS_CLUSTER_QUEUE_MSG_CONFIG_SEND;
  memcpy(&queueMsg.content.memberID, memberID, sizeof(clusterMemberID_t));

  /* send message to process on diffServClusterTask */
  if (osapiMessageSend(dsClusterQueue, (void *)&queueMsg, 
                       sizeof(dsClusterQueueMsg_t), 
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("%s: osapiMessageSend failed, queueMsg.type=%u\n", 
            __FUNCTION__, queueMsg.type);
  }

  return;
}

/*********************************************************************
* @purpose  Callback to handle received configuration events
*
* @param    clusterConfigCmd cmd @b{(input)} configuration command
*
* @returns  void
*
* @notes     
*           
* @end
*********************************************************************/
void diffServClusterConfigRxCallback(clusterConfigCmd cmd)
{
  dsClusterQueueMsg_t queueMsg;

  DIFFSERV_CLUSTER_DLOG("%s: cmd=%u\n", __FUNCTION__, cmd);

  memset(&queueMsg, 0, sizeof(dsClusterQueueMsg_t));
  queueMsg.type = DS_CLUSTER_QUEUE_MSG_CONFIG_RX;
  queueMsg.content.configCmd = cmd;

  /* send message to process on diffServClusterTask */
  if (osapiMessageSend(dsClusterQueue, (void *)&queueMsg, 
                       sizeof(dsClusterQueueMsg_t), 
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("%s: osapiMessageSend failed, queueMsg.type=%u\n", 
            __FUNCTION__, queueMsg.type);
  }

  return;
}

/*********************************************************************
* @purpose  Function to process cluster config message
*
* @param    L7_ushort16    elemId @b{(input)} received element ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t diffServClusterConfigRxIeSeqCheck(L7_ushort16 elemId)
{
  dsClusterMsgIe_t  currIe = DS_CLUSTER_MSG_IE_NONE;
  dsClusterMsgIe_t  nextIe = dsClusterRxCfgIeExpected;

  /* check this IE versus what is expected */
  switch (elemId)
  {
    /* independent or config start IEs expect IE type of 'none' */
    case DS_CLUSTER_MSG_IE_CONFIG_START:
    case DS_CLUSTER_MSG_IE_GLOBAL_CONFIG:
    case DS_CLUSTER_MSG_IE_CLASS_CONFIG_START:
    case DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_START:
    case DS_CLUSTER_MSG_IE_POLICY_CONFIG_START:
    case DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_START:
    case DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_START:
    case DS_CLUSTER_MSG_IE_CONFIG_END:
      currIe = DS_CLUSTER_MSG_IE_NONE;
      break;

    /* def IEs expect to see their own IE */
    case DS_CLUSTER_MSG_IE_CLASS_DEF:
    case DS_CLUSTER_MSG_IE_CLASS_RULE_DEF:
    case DS_CLUSTER_MSG_IE_POLICY_DEF:
    case DS_CLUSTER_MSG_IE_POLICY_INST_DEF:
    case DS_CLUSTER_MSG_IE_POLICY_ATTR_DEF:
      currIe = elemId;
      break;

    /* config end IEs expect IE to be their corresponding def IE */
    case DS_CLUSTER_MSG_IE_CLASS_CONFIG_END:
      currIe = DS_CLUSTER_MSG_IE_CLASS_DEF;
      break;
    case DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_END:
      currIe = DS_CLUSTER_MSG_IE_CLASS_RULE_DEF;
      break;
    case DS_CLUSTER_MSG_IE_POLICY_CONFIG_END:
      currIe = DS_CLUSTER_MSG_IE_POLICY_DEF;
      break;
    case DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_END:
      currIe = DS_CLUSTER_MSG_IE_POLICY_INST_DEF;
      break;
    case DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_END:
      currIe = DS_CLUSTER_MSG_IE_POLICY_ATTR_DEF;
      break;

    default:
      return L7_FAILURE;
      /*PASSTHRU*/

  } /* endswitch */

  /* check if we got the expected IE */
  if (currIe != dsClusterRxCfgIeExpected)
  {
    return L7_FAILURE;
  }

  /* determine next expected IE */
  switch (elemId)
  {
    /* config start IEs expect next IE to be their corresponding def IE */
    case DS_CLUSTER_MSG_IE_CLASS_CONFIG_START:
      nextIe = DS_CLUSTER_MSG_IE_CLASS_DEF;
      break;
    case DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_START:
      nextIe = DS_CLUSTER_MSG_IE_CLASS_RULE_DEF;
      break;
    case DS_CLUSTER_MSG_IE_POLICY_CONFIG_START:
      nextIe = DS_CLUSTER_MSG_IE_POLICY_DEF;
      break;
    case DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_START:
      nextIe = DS_CLUSTER_MSG_IE_POLICY_INST_DEF;
      break;
    case DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_START:
      nextIe = DS_CLUSTER_MSG_IE_POLICY_ATTR_DEF;
      break;

    /* def IEs expect to see their own IE */
    case DS_CLUSTER_MSG_IE_CLASS_DEF:
    case DS_CLUSTER_MSG_IE_CLASS_RULE_DEF:
    case DS_CLUSTER_MSG_IE_POLICY_DEF:
    case DS_CLUSTER_MSG_IE_POLICY_INST_DEF:
    case DS_CLUSTER_MSG_IE_POLICY_ATTR_DEF:
      nextIe = elemId;
      break;

    /* independent or config end IEs expect next IE of 'none' */
    case DS_CLUSTER_MSG_IE_CONFIG_START:
    case DS_CLUSTER_MSG_IE_GLOBAL_CONFIG:
    case DS_CLUSTER_MSG_IE_CLASS_CONFIG_END:
    case DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_END:
    case DS_CLUSTER_MSG_IE_POLICY_CONFIG_END:
    case DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_END:
    case DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_END:
    case DS_CLUSTER_MSG_IE_CONFIG_END:
      nextIe = DS_CLUSTER_MSG_IE_NONE;
      break;

    default:
      return L7_FAILURE;
      /*PASSTHRU*/

  } /* endswitch */

  /* set IE to expect next */
  dsClusterRxCfgIeExpected = nextIe;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Function to process cluster config message
*
* @param    dsClusterMsg_t *msg   @b{(input)} received message
*
* @returns  void
*
* @notes  
*
* @end
*********************************************************************/
void diffServClusterConfigMsgProcess(dsClusterMsg_t *msg)
{
  static L7_ushort16          startDefs = 0;
  static L7_ushort16          remainDefs = 0;

  clusterMsgElementHdr_t      elem;
  L7_ushort16                 elemSize;
  L7_uchar8                   *buffer = msg->buffer;
  L7_uint32                   offset = 0;
  dsClusterIeGroup_t          ieGroupType;
  L7_uint32                   index;
  L7_ushort16                 endDefs;
  dsmibTableId_t              tableID;
  dsClusterTableParms_t       *pTable = L7_NULLPTR;

  DIFFSERV_CLUSTER_DLOG("ENTERING %s: message length=%u\n", __FUNCTION__, msg->length);

  while (offset < msg->length)
  { 
    CLUSTER_PKT_ELEMENT_HDR_GET(elem, msg->buffer, offset);

    /*DIFFSERV_CLUSTER_DLOG("%s: received elem ID 0x%4.4x (len=%u)\n",
                          __FUNCTION__, elem.elementId, elem.elementLen);*/

    /* check for proper IE sequence order */
    if (diffServClusterConfigRxIeSeqCheck(elem.elementId) != L7_SUCCESS)
    {
      LOG_MSG("%s: improper msg IE sequence detected: expected 0x%4.4x, got 0x%4.4x\n",
              __FUNCTION__, dsClusterRxCfgIeExpected, elem.elementId);
      /* quit processing this and further messages */
      dsClusterRxError = L7_TRUE;
      return;
    }

    do
    {
      /* NOTE: The essence of this function is to process IEs comprising the 
       *       DiffServ cluster config.  Some of these IEs are independent, but 
       *       most belong to a group of IEs -- start, zero or more def(s), end.
       *       The processing of each of these group types is handled the same
       *       for each DiffServ config table, so the first switch stmt handles
       *       any unique processing, while the second switch stmt takes care
       *       of the commonality.
       */

      /* identify this IE as a particular group type (will take action on this below) */
      ieGroupType = _dsClusterIeGroupTypeGet(elem.elementId);

      /* make sure received IE size matches its defined size */
      elemSize = _dsClusterIeDefinedSizeGet(elem.elementId);
      CLUSTER_PKT_ELEMENT_SIZE_CHECK(dsCompId, dsClusterDebug, elem, elemSize, offset);

      /* get the table ID and entry pointer for this elem type (if applicable) */
      tableID = _dsClusterIeTableIdGet(elem.elementId);
      if (DSMIB_TABLE_ID_NONE != tableID)
      {
        pTable = &dsClusterTableParms[tableID];
      }

      switch (elem.elementId)
      {
        case DS_CLUSTER_MSG_IE_CONFIG_START:
          offset += DS_CLUSTER_MSG_IE_CONFIG_START_SIZE;  /* nothing to process in buffer*/
          dsClusterRxCfgMsgStarted = L7_TRUE;
          break;

        case DS_CLUSTER_MSG_IE_GLOBAL_CONFIG:
          CLUSTER_PKT_INT32_GET(dsClusterRxCfg->adminMode, buffer, offset);
          break;

        case DS_CLUSTER_MSG_IE_CLASS_CONFIG_START:
        case DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_START:
        case DS_CLUSTER_MSG_IE_POLICY_CONFIG_START:
        case DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_START:
        case DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_START:
          /* handled below */
          break;

        case DS_CLUSTER_MSG_IE_CLASS_DEF:
        case DS_CLUSTER_MSG_IE_CLASS_RULE_DEF:
        case DS_CLUSTER_MSG_IE_POLICY_DEF:
        case DS_CLUSTER_MSG_IE_POLICY_INST_DEF:
        case DS_CLUSTER_MSG_IE_POLICY_ATTR_DEF:
          /* handled below */
          break;

        case DS_CLUSTER_MSG_IE_CLASS_CONFIG_END:
        case DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_END:
        case DS_CLUSTER_MSG_IE_POLICY_CONFIG_END:
        case DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_END:
        case DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_END:
          /* handled below */
          break;

        case DS_CLUSTER_MSG_IE_CONFIG_END:
          offset += DS_CLUSTER_MSG_IE_CONFIG_END_SIZE;    /* nothing to process in buffer */
          dsClusterRxCfgMsgStarted = L7_FALSE;
          /* display the received config struct based on DiffServ msgLvl setting */
          if (pDiffServInfo_g->diffServMsgLvl >= DIFFSERV_MSGLVL_LO)
          {
            DIFFSERV_CLUSTER_DLOG("%s: DiffServ Cluster Config RX data\n", __FUNCTION__); 
            dsmibConfigDataShow(dsClusterRxCfg);
          }
          break;

        default:
          DIFFSERV_CLUSTER_DLOG("%s: received unknown IE 0x%4.4x (len=%u)\n",
                                 __FUNCTION__, elem.elementId, elem.elementLen);
          offset += elem.elementLen;
          break;

      } /* endswitch */

      /* process IE depending on whether start, def, or end of a group */
      switch (ieGroupType)
      {
        case DS_CLUSTER_IE_GROUP_START:
          CLUSTER_PKT_INT16_GET(startDefs, buffer, offset);
          DS_CLUSTER_IE_DEF_COUNT_CHECK(remainDefs, 0);
          if ((L7_NULLPTR != pTable) &&
              (startDefs >= pTable->maxEnt))
          {
            DIFFSERV_CLUSTER_DLOG("%s: IE 0x%4.4x group start shows %u entries, "
                                  "but max supported is %u\n",
                                   __FUNCTION__, elem.elementId, startDefs, 
                                   pTable->maxEnt);
            dsClusterRxError = L7_TRUE;
            break;
          }
          remainDefs = startDefs;
          break;

        case DS_CLUSTER_IE_GROUP_DEF:
          index = startDefs - remainDefs + 1;   /* all cfg table indexes are 1-based */
          if ((L7_NULLPTR != pTable) &&
              (L7_NULLPTR != pTable->accessFn))
          {
            pTable->accessFn(L7_FALSE, dsClusterRxCfg, index, buffer, &offset);
          }
          remainDefs--;
          break;

        case DS_CLUSTER_IE_GROUP_END:
          CLUSTER_PKT_INT16_GET(endDefs, buffer, offset);
          DS_CLUSTER_IE_DEF_COUNT_CHECK(remainDefs, 0);
          DS_CLUSTER_IE_DEF_COUNT_CHECK(startDefs, endDefs);
          startDefs = 0;
          break;

        case DS_CLUSTER_IE_GROUP_NONE:
        default:
          break;

      } /* endswitch */

    } while (0);

    if (L7_TRUE == dsClusterRxError)
    {
      /* quit processing this and further messages */
      DIFFSERV_CLUSTER_DLOG("%s: config receive error occurred\n", __FUNCTION__);
      break;
    }

  } /* endwhile */

  DIFFSERV_CLUSTER_DLOG("LEAVING %s: rxError=%u\n", __FUNCTION__, dsClusterRxError);
}

/*********************************************************************
* @purpose  Sets/Gets DiffServ class config data to/from message buffer
*
* @param    L7_BOOL           isSet       @b{(input)} L7_TRUE (set) or L7_FALSE (get)
* @param    L7_diffServCfg_t  *pCfgData   @b{(input)} operational config data
* @param    L7_uint32         entryIndex  @b{(input)} config table array index
* @param    L7_uchar8         *buffer     @b{(inout)} message buffer
* @param    L7_uint32         *offset     @b{(inout)} current offset in buffer
*
* @returns  void
*
* @comments Works with a cached copy of current DiffServ operational config data.
*
* @comments Assumes caller already checked for sufficient room in msg buffer. 
*
* @end
*********************************************************************/
void diffServClusterConfigClassMsgAccess(L7_BOOL           isSet,
                                         L7_diffServCfg_t  *pCfgData,
                                         L7_uint32         entryIndex,
                                         L7_uchar8         *buffer,
                                         L7_uint32         *offset)
{
  L7_diffServClassHdr_t       *pEntry = &pCfgData->classHdr[entryIndex];
  L7_uchar8                   rsvd[2] = { 0 };
  L7_uint32                   aclNum;
  L7_ACL_TYPE_t               aclType;
  L7_uchar8                   aclName[L7_ACL_NAME_LEN_MAX+1];

  aclNum = 0;
  aclType = L7_ACL_TYPE_NONE;
  memset(aclName, 0, sizeof(aclName));

  /* Need to send the ACL name instead of the ACL index number in the 
   * config message, since named ACL index numbers are not necessarily
   * the same on the sending and receiving systems.
   *
   * An L7_ACL_TYPE_NONE represents an aclNum of 0.  In this case,
   * the aclName is sent as an empty string ("") and is ignored by the
   * receiver.
   */
  if (L7_TRUE == isSet)
  {
    if (0 == pEntry->mib.aclNum)
    {
      /* ignoring rc here, since failure to get name causes empty string to be sent
       * (this condition is checked by receiver)
       */
      (void)diffServAclCommonNameStringGet(pEntry->mib.aclNum, &aclType, aclName);
    }
  }

  /* 00-03: In-use flag */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->inUse, buffer, offset);
  /* 04-07: Config array index of first class rule */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->chainIndex, buffer, offset);

  /** Start MIB row definition **/

  /* 08-11: Table row index (class index) */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->mib.index, buffer, offset);
  /* 12-43: Class name (includes terminating null character) */
  DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, pEntry->mib.name, sizeof(pEntry->mib.name), buffer, offset);
  /* 44-47: Class type */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->mib.type, buffer, offset);
  /* 48-51: Layer 3 protocol */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->mib.l3Protocol, buffer, offset);
  /* 52-55: ACL type (for class type ACL) */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, aclType, buffer, offset);
  /* 56-87: ACL name (for class type ACL) -- null terminated string */
  DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, aclName, sizeof(aclName), buffer, offset);
  /* 88-91: Next free class rule index */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->mib.ruleIndexNextFree, buffer, offset);
  /* 92   : Storage type */
  DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pEntry->mib.storageType, buffer, offset);
  /* 93   : Row status */
  DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pEntry->mib.rowStatus, buffer, offset);

  /** End MIB row definition **/

  /* 94-95: (Reserved = 0) */
  DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, rsvd, sizeof(rsvd), buffer, offset);
  /* 96-99: Row invalid object flags */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->rowInvalidFlags, buffer, offset);

  /* Translate ACL type and name received from config message to its 
   * ACL index number on the receiving system.  
   *
   * An L7_ACL_TYPE_NONE represents an aclNum of 0.  In this case,
   * the aclName is sent as an empty string ("") and is ignored by the
   * receiver.
   *
   * NOTE: If the name-to-index translation is unsuccessful, just set the 
   *       mib.aclNum value to 0 and keep going.  The config validation checking
   *       will detect an error when it sees a class type 'acl' with an 
   *       aclNum of 0.
   */
  if (L7_FALSE == isSet)
  {
    if (L7_ACL_TYPE_NONE != aclType)
    {
      if (diffServAclCommonNameToIndex(aclType, aclName, &aclNum) != L7_SUCCESS)
      {
        aclNum = 0;
      }
    }
    pEntry->mib.aclNum = aclNum;
  }
}

/*********************************************************************
* @purpose  Sets/Gets DiffServ class rule config data to/from message buffer
*
* @param    L7_BOOL           isSet       @b{(input)} L7_TRUE (set) or L7_FALSE (get)
* @param    L7_diffServCfg_t  *pCfgData   @b{(input)} operational config data
* @param    L7_uint32         entryIndex  @b{(input)} config table array index
* @param    L7_uchar8         *buffer     @b{(inout)} message buffer
* @param    L7_uint32         *offset     @b{(inout)} current offset in buffer
*
* @returns  void
*
* @comments Works with a cached copy of current DiffServ operational config data.
*
* @comments Assumes caller already checked for sufficient room in msg buffer. 
*
* @end
*********************************************************************/
void diffServClusterConfigClassRuleMsgAccess(L7_BOOL           isSet,
                                             L7_diffServCfg_t  *pCfgData,
                                             L7_uint32         entryIndex,
                                             L7_uchar8         *buffer,
                                             L7_uint32         *offset)
{
  L7_diffServClassRule_t      *pEntry = &pCfgData->classRule[entryIndex];
  L7_uchar8                   rsvd[1] = { 0 };
  dsmibClassRuleData_t        *pMatch;
  dsmibClassRuleType_t        entryType;

  /* 00-03: Config array index of parent class */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->hdrIndexRef, buffer, offset);
  /* 04-07: Config array index of next sequential class rule */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->chainIndex, buffer, offset);

  /** Start MIB row definition **/

  /* 08-11: Table row index (class rule index) */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->mib.index, buffer, offset);
  /* 12-15: Class rule entry type */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->mib.entryType, buffer, offset);

  /* set up an entry type variable for type-specific field processing */
  entryType = pEntry->mib.entryType;

  /* set ptr to mib.match fields for convenience */
  pMatch = &pEntry->mib.match;

  /* 16-19: General value (per entryType) */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->cos, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->cos2, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:
      DS_CLUSTER_PKT_INT16_GET_OR_SET(isSet, pMatch->etype.keyid, buffer, offset);
      DS_CLUSTER_PKT_INT16_GET_OR_SET(isSet, pMatch->etype.value, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->matchEvery, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->ipv6FlowLabel, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->ipDscp, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->ipPrecedence, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pMatch->ipTos.bits, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pMatch->ipTos.mask, buffer, offset);
      DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, pMatch->ipTos.rsvd1, 
                                       sizeof(pMatch->ipTos.rsvd1),
                                       buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->protocolNum, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->refClassIndex, buffer, offset);
      break;
    default:
      *offset += sizeof(L7_uint32);
      break;
  }
  /* 20-27: IPv4 address (per entryType) */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->dstIp.addr, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->dstIp.mask, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->srcIp.addr, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->srcIp.mask, buffer, offset);
      break;
    default:
      *offset += (2 * sizeof(L7_uint32));
      break;
  }
  /* 28-47: IPv6 address (per entryType) */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
      DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, pMatch->dstIpv6.ip6prefix.in6Addr.in6.addr8, IPV6_ADDRESS_LEN, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->dstIpv6.ip6prefix.in6PrefixLen, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
      DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, pMatch->srcIpv6.ip6prefix.in6Addr.in6.addr8, IPV6_ADDRESS_LEN, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->srcIpv6.ip6prefix.in6PrefixLen, buffer, offset);
      break;
    default:
      *offset += (IPV6_ADDRESS_LEN + sizeof(L7_uint32));
      break;
  }
  /* 48-55: Numeric range (per entryType) */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->dstL4Port.start, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->dstL4Port.end, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->srcL4Port.start, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->srcL4Port.end, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->vlanId.start, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->vlanId.end, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->vlanId2.start, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pMatch->vlanId2.end, buffer, offset);
      break;
    default:
      *offset += (2 * sizeof(L7_uint32));
      break;
  }
  /* 52-63: MAC address (per entryType) */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
      DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, pMatch->dstMac.addr, L7_MAC_ADDR_LEN, buffer, offset);
      DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, pMatch->dstMac.mask, L7_MAC_ADDR_LEN, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
      DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, pMatch->srcMac.addr, L7_MAC_ADDR_LEN, buffer, offset);
      DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, pMatch->srcMac.mask, L7_MAC_ADDR_LEN, buffer, offset);
      break;
    default:
      *offset += (2 * L7_MAC_ADDR_LEN);
      break;
  }
  /* 64   : Exclude flag */
  DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pEntry->mib.excludeFlag, buffer, offset);
  /* 65   : Storage type */
  DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pEntry->mib.storageType, buffer, offset);
  /* 66   : Row status */
  DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pEntry->mib.rowStatus, buffer, offset);

  /** End MIB row definition **/

  /* 67   : (Reserved = 0) */
  DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, rsvd, sizeof(rsvd), buffer, offset);
  /* 68-71: Row invalid object flags */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->rowInvalidFlags, buffer, offset);
  /* 72-75: ACL rule identifier origin */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->arid, buffer, offset);
}

/*********************************************************************
* @purpose  Sets/Gets DiffServ policy config data to/from message buffer
*
* @param    L7_BOOL           isSet       @b{(input)} L7_TRUE (set) or L7_FALSE (get)
* @param    L7_diffServCfg_t  *pCfgData   @b{(input)} operational config data
* @param    L7_uint32         entryIndex  @b{(input)} config table array index
* @param    L7_uchar8         *buffer     @b{(inout)} message buffer
* @param    L7_uint32         *offset     @b{(inout)} current offset in buffer
*
* @returns  void
*
* @comments Works with a cached copy of current DiffServ operational config data.
*
* @comments Assumes caller already checked for sufficient room in msg buffer. 
*
* @end
*********************************************************************/
void diffServClusterConfigPolicyMsgAccess(L7_BOOL           isSet,
                                          L7_diffServCfg_t  *pCfgData,
                                          L7_uint32         entryIndex,
                                          L7_uchar8         *buffer,
                                          L7_uint32         *offset)
{
  L7_diffServPolicyHdr_t      *pEntry = &pCfgData->policyHdr[entryIndex];
  L7_uchar8                   rsvd[1] = { 0 };

  /* 00-03: In-use flag */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->inUse, buffer, offset);
  /* 04-07: Config array index of first policy instance */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->chainIndex, buffer, offset);

  /** Start MIB row definition **/

  /* 08-11: Table row index (policy index) */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->mib.index, buffer, offset);
  /* 12-43: Policy name (includes terminating null character) */
  DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, pEntry->mib.name, sizeof(pEntry->mib.name), buffer, offset);
  /* 44-47: Next free policy instance index */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->mib.instIndexNextFree, buffer, offset);
  /* 49   : Policy type */
  DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pEntry->mib.type, buffer, offset);
  /* 49   : Storage type */
  DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pEntry->mib.storageType, buffer, offset);
  /* 50   : Row status */
  DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pEntry->mib.rowStatus, buffer, offset);

  /** End MIB row definition **/

  /* 51   : (Reserved = 0) */
  DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, rsvd, sizeof(rsvd), buffer, offset);
  /* 52-55: Row invalid object flags */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->rowInvalidFlags, buffer, offset);
}

/*********************************************************************
* @purpose  Sets/Gets DiffServ policy instance config data to/from message buffer
*
* @param    L7_BOOL           isSet       @b{(input)} L7_TRUE (set) or L7_FALSE (get)
* @param    L7_diffServCfg_t  *pCfgData   @b{(input)} operational config data
* @param    L7_uint32         entryIndex  @b{(input)} config table array index
* @param    L7_uchar8         *buffer     @b{(inout)} message buffer
* @param    L7_uint32         *offset     @b{(inout)} current offset in buffer
*
* @returns  void
*
* @comments Works with a cached copy of current DiffServ operational config data.
*
* @comments Assumes caller already checked for sufficient room in msg buffer. 
*
* @end
*********************************************************************/
void diffServClusterConfigPolicyInstMsgAccess(L7_BOOL           isSet,
                                              L7_diffServCfg_t  *pCfgData,
                                              L7_uint32         entryIndex,
                                              L7_uchar8         *buffer,
                                              L7_uint32         *offset)
{
  L7_diffServPolicyInst_t     *pEntry = &pCfgData->policyInst[entryIndex];
  L7_uchar8                   rsvd[2] = { 0 };

  /* 00-03: Config array index of parent policy */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->hdrIndexRef, buffer, offset);
  /* 04-07: Config array index of first policy attribute */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->attrChainIndex, buffer, offset);
  /* 08-11: Config array index of next sequential policy instance */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->chainIndex, buffer, offset);

  /** Start MIB row definition **/

  /* 12-15: Table row index (policy instance index) */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->mib.index, buffer, offset);
  /* 16-19: Class index */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->mib.classIndex, buffer, offset);
  /* 20-23: Next free policy attribute index */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->mib.attrIndexNextFree, buffer, offset);
  /* 24   : Storage type */
  DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pEntry->mib.storageType, buffer, offset);
  /* 25   : Row status */
  DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pEntry->mib.rowStatus, buffer, offset);

  /** End MIB row definition **/

  /* 26-27: (Reserved = 0) */
  DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, rsvd, sizeof(rsvd), buffer, offset);
  /* 28-31: Row invalid object flags */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->rowInvalidFlags, buffer, offset);
}

/*********************************************************************
* @purpose  Sets/Gets DiffServ policy attribute config data to/from message buffer
*
* @param    L7_BOOL           isSet       @b{(input)} L7_TRUE (set) or L7_FALSE (get)
* @param    L7_diffServCfg_t  *pCfgData   @b{(input)} operational config data
* @param    L7_uint32         entryIndex  @b{(input)} config table array index
* @param    L7_uchar8         *buffer     @b{(inout)} message buffer
* @param    L7_uint32         *offset     @b{(inout)} current offset in buffer
*
* @returns  void
*
* @comments Works with a cached copy of current DiffServ operational config data.
*
* @comments Assumes caller already checked for sufficient room in msg buffer. 
*
* @end
*********************************************************************/
void diffServClusterConfigPolicyAttrMsgAccess(L7_BOOL           isSet,
                                              L7_diffServCfg_t  *pCfgData,
                                              L7_uint32         entryIndex,
                                              L7_uchar8         *buffer,
                                              L7_uint32         *offset)
{
  L7_diffServPolicyAttr_t     *pEntry = &pCfgData->policyAttr[entryIndex];
  dsmibPolicyAttrType_t       entryType;
  dsmibPolicyAttrData_t       *pStmt;
  L7_uchar8                   rsvd[2] = { 0 };

  /* 00-03: Config array index of parent policy */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->instIndexRef, buffer, offset);
  /* 04-07: Config array index of next sequential policy attribute */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->chainIndex, buffer, offset);

  /** Start MIB row definition **/

  /* NOTE: Some of the field accesses that follow are contingent upon the entryType.
   *       If data is not defined for a particular entryType, its location in the 
   *       message buffer is accounted for, but is bypassed.
   */

  pStmt = &pEntry->mib.stmt;            /* set ptr to mib.stmt fields for convenience */

  /* 08-11: Table row index (policy attribute index) */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->mib.index, buffer, offset);
  /* 12-15: Policy attribute entry type */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->mib.entryType, buffer, offset);

  /* set up an entry type variable for type-specific field processing */
  entryType = pEntry->mib.entryType;

  /* 16   : Queue Id */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->assignQueue.qid, buffer, offset);
      break;
    default:
      *offset += sizeof(L7_uchar8);
      break;
  }
  /* 17   : Boolean flag */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->drop, buffer, offset);
      break;
    default:
      *offset += sizeof(L7_uchar8);
      break;
  }
  /* 18   : Mark value */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->markCos.val, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->markCos2.val, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->markCosAsCos2.val, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->markIpDscp.val, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->markIpPrecedence.val, buffer, offset);
      break;
    default:
      *offset += sizeof(L7_uchar8);
      break;
  }
  /* 19   : (Reserved = 0) */
  DS_CLUSTER_PKT_STRING_GET_OR_SET(isSet, rsvd, sizeof(rsvd[0]), buffer, offset);
  /* 20-27: NIM config Id */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->mirror.configId.type, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->mirror.configId.configSpecifier.usp.unit, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->mirror.configId.configSpecifier.usp.slot, buffer, offset);
      DS_CLUSTER_PKT_INT16_GET_OR_SET(isSet, pStmt->mirror.configId.configSpecifier.usp.port, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->redirect.configId.type, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->redirect.configId.configSpecifier.usp.unit, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->redirect.configId.configSpecifier.usp.slot, buffer, offset);
      DS_CLUSTER_PKT_INT16_GET_OR_SET(isSet, pStmt->redirect.configId.configSpecifier.usp.port, buffer, offset);
      break;
    default:
      *offset += (sizeof(L7_uint32) + (2 * sizeof(L7_uchar8)) + sizeof(L7_ushort16));
      break;
  }
  /* 28-31: Policing C-rate
   * 32-35: Policing C-burst
   */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeSimple.crate, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeSimple.cburst, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeSingleRate.crate, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeSingleRate.cburst, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeTwoRate.crate, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeTwoRate.cburst, buffer, offset);
      break;
    default:
      *offset += (2 * sizeof(L7_uint32));
      break;
  }
  /* 36-39: Policing E-rate/P-rate
   * 40-43: Policing E-burst/P-burst
   */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeSingleRate.crate, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeSingleRate.eburst, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeTwoRate.prate, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeTwoRate.pburst, buffer, offset);
      break;
    default:
      *offset += (2 * sizeof(L7_uint32));
      break;
  }
  /* 44   : Policing C-action
   * 45   : Policing C-mark
   * 46   : Policing E-action
   * 47   : Policing E-mark
   * 48   : Policing N-action
   * 49   : Policing N-mark
   */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSimple.conformAct, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSimple.conformVal, buffer, offset);
      *offset += (2 * sizeof(L7_uchar8));       /* E-action, E-mark not used */
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSimple.nonconformAct, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSimple.nonconformVal, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSingleRate.conformAct, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSingleRate.conformVal, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSingleRate.exceedAct, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSingleRate.exceedVal, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSingleRate.nonconformAct, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSingleRate.nonconformVal, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeTwoRate.conformAct, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeTwoRate.conformVal, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeTwoRate.exceedAct, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeTwoRate.exceedVal, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeTwoRate.nonconformAct, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeTwoRate.nonconformVal, buffer, offset);
      break;
    default:
      *offset += (6 * sizeof(L7_uchar8));
      break;
  }
  /* 50   : Policing C-color mode
   * 51   : Policing C-color value
   * 52-55: Policing C-color index
   */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSimple.colorConformMode, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSimple.colorConformVal, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeSimple.colorConformIndex, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSingleRate.colorConformMode, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSingleRate.colorConformVal, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeSingleRate.colorConformIndex, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeTwoRate.colorConformMode, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeTwoRate.colorConformVal, buffer, offset);
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeTwoRate.colorConformIndex, buffer, offset);
      break;
    default:
      *offset += ((2 * sizeof(L7_uchar8)) + sizeof(L7_uint32));
      break;
  }
  /* 56-59: Policing E-color index
   * 60   : Policing E-color mode
   * 61   : Policing E-color value
   */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeSingleRate.colorExceedIndex, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSingleRate.colorExceedMode, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeSingleRate.colorExceedVal, buffer, offset);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pStmt->policeTwoRate.colorExceedIndex, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeTwoRate.colorExceedMode, buffer, offset);
      DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pStmt->policeTwoRate.colorExceedVal, buffer, offset);
      break;
    default:
      *offset += (sizeof(L7_uint32) + (2 * sizeof(L7_uchar8)));
      break;
  }
  /* 62   : Storage type */
  DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pEntry->mib.storageType, buffer, offset);
  /* 63   : Row status */
  DS_CLUSTER_PKT_INT8_GET_OR_SET(isSet, pEntry->mib.rowStatus, buffer, offset);

  /** End MIB row definition **/

  /* 64-67: Row invalid object flags */
  DS_CLUSTER_PKT_INT32_GET_OR_SET(isSet, pEntry->rowInvalidFlags, buffer, offset);
}

/*********************************************************************
* @purpose  Function to send DiffServ global config to cluster member
*
* @param    L7_diffServCfg_t  *pCfgData  @b{(input)} operational config data
* @param    clusterMemberID_t *memberID  @b{(input)} cluster member
* @param    L7_uchar8         *buffer @b{(input/output)} message buffer
* @param    L7_uint32         *offset @b{(input/output)} current offset in buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Works with a cached copy of current DiffServ operational config data.
*
* @end
*********************************************************************/
L7_RC_t diffServClusterConfigGlobalSend(L7_diffServCfg_t  *pCfg,
                                        clusterMemberID_t *memberID,
                                        L7_uchar8         *buffer,
                                        L7_uint32         *offset)
{
  L7_uint32         dataSize;

  dataSize = (1 * sizeof(clusterMsgElementHdr_t)) + 
             DS_CLUSTER_MSG_IE_GLOBAL_CONFIG_SIZE;
  if (diffServClusterConfigMsgSendCheck(dataSize, memberID, buffer, offset) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  CLUSTER_PKT_ELEMENT_HDR_SET(DS_CLUSTER_MSG_IE_GLOBAL_CONFIG,
                              DS_CLUSTER_MSG_IE_GLOBAL_CONFIG_SIZE, 
                              buffer, *offset);
  CLUSTER_PKT_INT32_SET(pCfg->adminMode, buffer, *offset);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Function to send DiffServ table config to cluster member
*
* @param    dsmibTableId_t    tableID     @b{(input)} DiffServ table identifier    
* @param    L7_diffServCfg_t  *pCfg       @b{(input)} operational config data
* @param    clusterMemberID_t *memberID   @b{(input)} cluster member
* @param    L7_uchar8         *buffer     @b{(input/output)} message buffer
* @param    L7_uint32         *offset     @b{(input/output)} current offset in buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is a generic function that handles building the common parts
*           of the DiffServ table-based configuration in the cluster config msg.
*           Only certain table IDs are supported.
*
* @comments Works with a cached copy of current DiffServ operational config data.
*
* @end
*********************************************************************/
L7_RC_t diffServClusterConfigTableSend(dsmibTableId_t    tableID,
                                       L7_diffServCfg_t  *pCfg,
                                       clusterMemberID_t *memberID,
                                       L7_uchar8         *buffer,
                                       L7_uint32         *offset)
{
  L7_uint32                   i, dataSize;
  L7_uint32                   startEnt, maxEnt;
  L7_ushort16                 defType, defSize;
  L7_ushort16                 numEnt = 0;
  L7_uint32                   configStartOffset;
  dsClusterTableParms_t       *pTable;

  if ((tableID < DSMIB_TABLE_ID_CLASS) ||
      (tableID > DSMIB_TABLE_ID_POLICY_ATTR))
  {
    return L7_FAILURE;
  }

  pTable = &dsClusterTableParms[tableID];
  if (L7_NULLPTR == pTable->accessFn)
  {
    return L7_FAILURE;
  }

  startEnt = pTable->startEnt; 
  maxEnt   = pTable->maxEnt;

  defType = pTable->defType;
  defSize = pTable->defTypeSize;

  /* Config Start IE */
  dataSize = sizeof(clusterMsgElementHdr_t) + pTable->startTypeSize;
  if (diffServClusterConfigMsgSendCheck(dataSize, memberID, buffer, offset) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  CLUSTER_PKT_ELEMENT_HDR_SET(pTable->startType, pTable->startTypeSize,
                              buffer, *offset);
  /* save current offset to go back and fill in actual number of entries later */
  configStartOffset = *offset;
  CLUSTER_PKT_INT16_SET(numEnt, buffer, *offset);   /* sets num entries to 0 for now */


  /* iterate through each configured entry */
  dataSize = sizeof(clusterMsgElementHdr_t) + defSize;
  for (i = startEnt; i < maxEnt; i++)
  {
    /* array entry not in use means end of the configured list */
    if (_dsClusterConfigEntryInUseGet(tableID, pCfg, i) != L7_TRUE)
      break;

    if (diffServClusterConfigMsgSendCheck(dataSize, memberID, buffer, offset) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    CLUSTER_PKT_ELEMENT_HDR_SET(defType, defSize, buffer, *offset);

    /* invoke table-specific access function to send msg def IE for this entry */
    pTable->accessFn(L7_TRUE, pCfg, i, buffer, offset);

  } /* endfor i */

  numEnt = i - startEnt;

  /* fill in config start number of entries using saved buffer offset*/
  CLUSTER_PKT_INT16_SET(numEnt, buffer, configStartOffset);

  /* Config End IE */
  dataSize = sizeof(clusterMsgElementHdr_t) + pTable->endTypeSize;
  if (diffServClusterConfigMsgSendCheck(dataSize, memberID, buffer, offset) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  CLUSTER_PKT_ELEMENT_HDR_SET(pTable->endType, pTable->endTypeSize, buffer, *offset);
  CLUSTER_PKT_INT16_SET(numEnt, buffer, *offset);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Function to send configuration to cluster member
*
* @param    clusterMemberID_t *memberID  @b{(input)} cluster member
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t diffServClusterConfigSend(clusterMemberID_t *memberID)
{
  L7_RC_t               rc = L7_SUCCESS;
  L7_char8              peerMacStr[L7_MAC_ADDR_STRING_LEN+1];
  L7_uchar8             buffer[DS_CLUSTER_MSG_SIZE_MAX];
  L7_uint32             offset = 0;
  dsmibTableId_t        tableID;

  if (l7utilsMacAddrHexToString(memberID->addr, 
                                sizeof(peerMacStr), peerMacStr) != L7_SUCCESS)
  {
    LOG_MSG("%s: Received invalid memberID.\n", __FUNCTION__);
    return L7_FAILURE;
  }

  DIFFSERV_CLUSTER_DLOG("%s: member ID=%s\n", __FUNCTION__, peerMacStr); 

  /* init cluster tx struct */
  if (L7_NULLPTR == dsClusterTxCfg)
  {
    LOG_MSG("%s: Error - DiffServ cluster TX config struct not allocated.\n", __FUNCTION__);
    return L7_FAILURE;
  }
  _dsClusterCfgStructInit(dsClusterTxCfg, "dsClusterTxCfg");

  /* read all of the DiffServ operational config into the tx struct */
  do
  {
    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);
    if (diffServBuildUserConfigData(L7_DIFFSERV_CFG_VER_CURRENT, dsClusterTxCfg) != L7_SUCCESS)
    {
      LOG_MSG("%s: Error extracting DiffServ operational config data.\n", __FUNCTION__);
      rc = L7_FAILURE;
      break;
    }
    dsClusterTxCfg->checkSum = 0;
    dsClusterTxCfg->checkSum = nvStoreCrc32((L7_uchar8 *)dsClusterTxCfg, L7_DIFFSERV_CFG_DATA_SIZE);
    DIFFSERV_SEMA_GIVE(dsmibIndexSemId);
  
    /* init the msg buffer */
    memset(buffer, 0, sizeof(buffer));
    offset = 0;
  
    CLUSTER_PKT_ELEMENT_HDR_SET(DS_CLUSTER_MSG_IE_CONFIG_START, 
                                DS_CLUSTER_MSG_IE_CONFIG_START_SIZE,
                                buffer, offset);
  
    if (diffServClusterConfigGlobalSend(dsClusterTxCfg, memberID, buffer, &offset) != L7_SUCCESS)
    {
      LOG_MSG("%s: Error sending DiffServ global config to peer switch %s.\n",
              __FUNCTION__, peerMacStr);
      rc = L7_FAILURE;
      break;
    }
  
    for (tableID = DSMIB_TABLE_ID_CLASS; tableID <= DSMIB_TABLE_ID_POLICY_ATTR; tableID++)
    {
      if (diffServClusterConfigTableSend(tableID, dsClusterTxCfg, memberID, buffer, &offset) != L7_SUCCESS)
      {
        LOG_MSG("%s: Error sending DiffServ %s table config to peer switch %s.\n",
                __FUNCTION__, dsmibPrvtTableIdStr[tableID], peerMacStr);
        rc = L7_FAILURE;
        break;
      }
    }

    /* display the sent config struct based on DiffServ msgLvl setting */
    if (pDiffServInfo_g->diffServMsgLvl >= DIFFSERV_MSGLVL_LO)
    {
      dsmibConfigDataShow(dsClusterTxCfg);
    }

  } while (0);

  if (L7_SUCCESS != rc)
  {
    return L7_FAILURE;
  }

  if (diffServClusterConfigMsgSendCheck(sizeof(clusterMsgElementHdr_t) + 
                                        DS_CLUSTER_MSG_IE_CONFIG_END_SIZE,
                                        memberID, buffer, &offset) != L7_SUCCESS)
  {
    LOG_MSG("%s: Error sending DiffServ config to peer switch.\n", __FUNCTION__);
    return L7_FAILURE;
  }

  CLUSTER_PKT_ELEMENT_HDR_SET(DS_CLUSTER_MSG_IE_CONFIG_END, 
                              DS_CLUSTER_MSG_IE_CONFIG_END_SIZE,
                              buffer, offset);

  rc = clusterMsgSend(memberID, CLUSTER_MSG_DELIVERY_RELIABLE, DS_CLUSTER_MSG_CONFIG,
                      offset, buffer);

  DIFFSERV_CLUSTER_DLOG("%s: DiffServ Cluster Config data (len=%u) sent to peer switch %s, rc=%d\n",
                        __FUNCTION__, offset, peerMacStr, rc); 

  return rc;
}

/*********************************************************************
* @purpose  Check if room in message buffer to add IE, send and clear 
*           buffer when max message size is reached.
*
* @param    L7_uint32          size      @b{(input)} IE size to write
* @param    clusterMemberID_t *memberID  @b{(input)} cluster member
* @param    L7_uchar8         *buffer    @b{(inout)} message buffer
* @param    L7_uint32         *offset    @b{(inout)} offset in buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes 
*           
* @end
*********************************************************************/
L7_RC_t diffServClusterConfigMsgSendCheck(L7_uint32          size,
                                          clusterMemberID_t *memberID,
                                          L7_uchar8         *buffer,
                                          L7_uint32         *offset)
{
  if (((*offset) + size) >= DS_CLUSTER_MSG_SIZE_MAX)
  {
    if (clusterMsgSend(memberID, CLUSTER_MSG_DELIVERY_RELIABLE,
                       DS_CLUSTER_MSG_CONFIG,
                       (*offset), buffer) != L7_SUCCESS)
    {
      DIFFSERV_CLUSTER_DLOG("%s: clusterMsgSend() call failed\n", __FUNCTION__);
      return L7_FAILURE;
    }
    memset(buffer, 0, DS_CLUSTER_MSG_SIZE_MAX);
    (*offset) = 0;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verify received DiffServ class config against system
*           defines and features
*
* @param    L7_diffServCfg_t  *pCfgData   @{(input)} received DiffServ config data
* @param    L7_uint32         entryIndex  @b{(input)} config table array index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This assumes data has already been validated by the sender,
*           so we can skip basic checks, such as range checking.  However,
*           this system may not have the same prod/plat defines or features 
*           enabled, so need to fail if we cannot apply entire configuration.
*
* @end
*********************************************************************/
L7_RC_t diffServClusterConfigClassVerify(L7_diffServCfg_t *pCfgData,
                                         L7_uint32        entryIndex)
{
  L7_diffServClassHdr_t       *pEntry = &pCfgData->classHdr[entryIndex];
  L7_uint32                   i, num = 0, lim = L7_DIFFSERV_RULE_PER_CLASS_LIM;

  /* make sure current number of rules per class is within supported limit
   *
   * NOTE: For accuracy, search the hdrIndex field in the Class Rule table
   *       and count the number of occurrences of this class entryIndex.
   */
  for (i = L7_DIFFSERV_CLASS_RULE_START; i < L7_DIFFSERV_CLASS_RULE_MAX; i++)
  {
    if (pCfgData->classRule[i].hdrIndexRef == entryIndex)
    {
      num++;
    }
  }
  if (num > lim)
  {
    DIFFSERV_CLUSTER_DLOG("%s: too many rules (%u) for class \'%s\' - max=%u\n",
                          __FUNCTION__, num, pEntry->mib.name, lim);
    return L7_FAILURE;
  }

  /* check feature support */
  if (dsmibClassFeaturePresentCheck(pEntry->mib.type) != L7_SUCCESS)
  {
    DIFFSERV_CLUSTER_DLOG("%s: class type \'%s\' not supported\n",
                          __FUNCTION__, dsmibClassTypeStr[pEntry->mib.type]);
    return L7_FAILURE;
  }
  if (dsmibClassProtoFeaturePresentCheck(pEntry->mib.l3Protocol) != L7_SUCCESS)
  {
    DIFFSERV_CLUSTER_DLOG("%s: class layer3 protocol \'%s\' not supported\n",
                          __FUNCTION__, dsmibClassL3ProtoStr[pEntry->mib.l3Protocol]);
    return L7_FAILURE;
  }

  /* for class type 'acl', check if aclType and aclNum are valid
   *
   * NOTE: Can only check this general case, since DiffServ does not
   *       attempt to keep current with the latest ACL definition.  It
   *       is possible to delete an ACL after it was used to create a 
   *       DiffServ class from it, so cannot check whether this ACL
   *       currently exists.
   */
  if (L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL == pEntry->mib.type)
  {
    if ((L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_NONE == pEntry->mib.aclType) ||
        (0 == pEntry->mib.aclNum))
    {
      DIFFSERV_CLUSTER_DLOG("%s: class type \'acl\' aclType or aclNum invalid\n",
                            __FUNCTION__);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verify received DiffServ class rule config against
*           system defines and features
*
* @param    L7_diffServCfg_t  *pCfgData   @{(input)} received DiffServ config data
* @param    L7_uint32         entryIndex  @b{(input)} config table array index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This assumes data has already been validated by the sender,
*           so we can skip basic checks, such as range checking.  However,
*           this system may not have the same prod/plat defines or features 
*           enabled, so need to fail if we cannot apply entire configuration.
*
* @end
*********************************************************************/
L7_RC_t diffServClusterConfigClassRuleVerify(L7_diffServCfg_t *pCfgData,
                                             L7_uint32        entryIndex)
{
  L7_diffServClassRule_t      *pEntry = &pCfgData->classRule[entryIndex];
  L7_uchar8                   nullMac[L7_MAC_ADDR_LEN] = { 0 };
  dsmibClassRuleType_t        entryType;
  dsmibClassRuleData_t        *pMatch;
  L7_uint32                   range1, range2;
  L7_BOOL                     isMasked;

  entryType = pEntry->mib.entryType;
  pMatch = &pEntry->mib.match;

  /* check entry type feature support */
  if (dsmibClassRuleFeaturePresentCheck(entryType) != L7_SUCCESS)
  {
    DIFFSERV_CLUSTER_DLOG("%s: rule type \'%s\' not supported\n",
                          __FUNCTION__, dsmibClassRuleTypeStr[entryType]);
    return L7_FAILURE;
  }

  /* check exclude flag feature support */
  if (cnfgrIsFeaturePresent(dsCompId, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID)
      != L7_TRUE)
  {
    if (L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE == pEntry->mib.excludeFlag)
    {
      DIFFSERV_CLUSTER_DLOG("%s: exclude flag not supported\n", __FUNCTION__);
      return L7_FAILURE;
    }
  }

  /* check masking feature support */
  if (cnfgrIsFeaturePresent(dsCompId, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_MASKING_FEATURE_ID)
      != L7_TRUE)
  {
    isMasked = L7_FALSE;
    switch (entryType)
    {
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
        isMasked = (0 != pEntry->mib.match.dstIp.mask)
                    ? L7_TRUE : L7_FALSE;
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
        isMasked = (0 != pEntry->mib.match.dstIpv6.ip6prefix.in6PrefixLen)
                    ? L7_TRUE : L7_FALSE;
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
        isMasked = (0 != memcmp(pEntry->mib.match.dstMac.mask, nullMac, sizeof(nullMac)))
                    ? L7_TRUE : L7_FALSE;
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
        isMasked = (0 != pEntry->mib.match.srcIp.mask)
                    ? L7_TRUE : L7_FALSE;
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
        isMasked = (0 != pEntry->mib.match.srcIpv6.ip6prefix.in6PrefixLen)
                    ? L7_TRUE : L7_FALSE;
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
        isMasked = (0 != memcmp(pEntry->mib.match.srcMac.mask, nullMac, sizeof(nullMac)))
                    ? L7_TRUE : L7_FALSE;
        break;

      default:
        break;

    } /* endswitch */
    if (L7_TRUE == isMasked)
    {
      DIFFSERV_CLUSTER_DLOG("%s: class rule type \'%s\' masking not supported\n",
                            __FUNCTION__, dsmibClassRuleTypeStr[entryType]);
      return L7_FAILURE;
    }

  } /* endif masking feature check */

  /* check range feature support */
  if (cnfgrIsFeaturePresent(dsCompId, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID)
      != L7_TRUE)
  {
    range1 = range2 = 0;
    switch (entryType)
    {
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
        range1 = pMatch->dstL4Port.start;
        range2 = pMatch->dstL4Port.end;
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
        range1 = pMatch->srcL4Port.start;
        range2 = pMatch->srcL4Port.end;
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID:
        range1 = pMatch->vlanId.start;
        range2 = pMatch->vlanId.end;
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2:
        range1 = pMatch->vlanId2.start;
        range2 = pMatch->vlanId2.end;
        break;

      default:
        break;

    } /* endswitch */
    if (range1 != range2)
    {
      DIFFSERV_CLUSTER_DLOG("%s: class rule type \'%s\' range not supported\n",
                            __FUNCTION__, dsmibClassRuleTypeStr[entryType]);
      return L7_FAILURE;
    }
  } /* endif range feature check */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verify received DiffServ policy config against
*           system defines and features
*
* @param    L7_diffServCfg_t  *pCfgData   @{(input)} received DiffServ config data
* @param    L7_uint32         entryIndex  @b{(input)} config table array index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This assumes data has already been validated by the sender,
*           so we can skip basic checks, such as range checking.  However,
*           this system may not have the same prod/plat defines or features 
*           enabled, so need to fail if we cannot apply entire configuration.
*
* @end
*********************************************************************/
L7_RC_t diffServClusterConfigPolicyVerify(L7_diffServCfg_t *pCfgData,
                                          L7_uint32        entryIndex)
{
  L7_diffServPolicyHdr_t      *pEntry = &pCfgData->policyHdr[entryIndex];
  L7_diffServPolicyInst_t     *pInstEntry;
  L7_diffServClassHdr_t       *pClassEntry;
  L7_diffServClassRule_t      *pRuleEntry;
  L7_uint32                   instIndex;
  L7_uint32                   classIndex, refClassIndex, classRuleIndex;
  dsmibClassL3Protocol_t      savedL3Protocol, classL3Protocol;
  L7_uint32                   i, num = 0, lim = L7_DIFFSERV_INST_PER_POLICY_LIM;

  /* make sure current number of instances per policy is within supported limit
   *
   * NOTE: For accuracy, search the hdrIndex field in the Policy Inst table
   *       and count the number of occurrences of this policy entryIndex.
   */
  for (i = L7_DIFFSERV_POLICY_INST_START; i < L7_DIFFSERV_POLICY_INST_MAX; i++)
  {
    if (pCfgData->policyInst[i].hdrIndexRef == entryIndex)
    {
      num++;
    }
  }
  if (num > lim)
  {
    DIFFSERV_CLUSTER_DLOG("%s: too many instances (%u) for policy \'%s\' - max=%u\n",
                          __FUNCTION__, num, pEntry->mib.name, lim);
    return L7_FAILURE;
  }

  /* check policy type feature support
   *
   * NOTE: There is no specific feature ID for the policy type, so check the
   *       service interface direction support instead, since this is how the 
   *       user interface controls whether to allow certain policy types to be
   *       created.
   */
  if ((cnfgrIsFeaturePresent(dsCompId, L7_DIFFSERV_SERVICE_IN_SLOTPORT_FEATURE_ID)
       != L7_TRUE) &&
      (cnfgrIsFeaturePresent(dsCompId, L7_DIFFSERV_SERVICE_IN_ALLPORTS_FEATURE_ID)
       != L7_TRUE))
  {
    if (L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN == pEntry->mib.type)
    {
      DIFFSERV_CLUSTER_DLOG("%s: policy type \'%s\' not supported\n",
                            __FUNCTION__, dsmibPolicyTypeStr[pEntry->mib.type]);
      return L7_FAILURE;
    }
  }
  if ((cnfgrIsFeaturePresent(dsCompId, L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID)
       != L7_TRUE) &&
      (cnfgrIsFeaturePresent(dsCompId, L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID)
       != L7_TRUE))
  {
    if (L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT == pEntry->mib.type)
    {
      DIFFSERV_CLUSTER_DLOG("%s: policy type \'%s\' not supported\n",
                            __FUNCTION__, dsmibPolicyTypeStr[pEntry->mib.type]);
      return L7_FAILURE;
    }
  }

  /* check feature that restricts classes used for outbound policy instances
   * to those containing only IP DSCP or IP Precedence match rules
   */
  if (L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT == pEntry->mib.type)
  {
    if (cnfgrIsFeaturePresent(dsCompId,
                              L7_DIFFSERV_POLICY_OUT_CLASS_UNRESTRICTED_FEATURE_ID)
        != L7_TRUE)
    {
      instIndex = pEntry->chainIndex;           /* first instance for this policy */

      while (0 != instIndex)
      {
        pInstEntry = &pCfgData->policyInst[instIndex];
  
        classIndex = pInstEntry->mib.classIndex;
        refClassIndex = classIndex;             /* start with current class instance */
  
        /* walk cascaded class reference chain to evaluate all match conditions for
         * this policy instance
         *
         * each class can have at most one refClass rule, so can process referenced
         * class after all rules within current class (i.e. recursion not needed)
         */
        while (0 != refClassIndex)
        {
          pClassEntry = &pCfgData->classHdr[refClassIndex];
          classRuleIndex = pClassEntry->chainIndex;

          /* this becomes nonzero again only if current class contains another ref class */
          refClassIndex = 0;

          while (0 != classRuleIndex)
          {
            pRuleEntry = &pCfgData->classRule[classRuleIndex];

            if (L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS
                == pRuleEntry->mib.entryType)
            {
              refClassIndex = pRuleEntry->mib.match.refClassIndex;
            }
            else
            {
              /* only IP DSCP or IP Precedence rule entry type allowed */
              if ((L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP
                   != pRuleEntry->mib.entryType) &&
                  (L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE
                   != pRuleEntry->mib.entryType))
              {
                /* found unsupported class rule match condition used in outbound policy */
                DIFFSERV_CLUSTER_DLOG("%s: class rule type \'%s\' not valid "
                                      "for outbound policy \'%s\'\n",
                                      __FUNCTION__,
                                      dsmibClassRuleTypeStr[pRuleEntry->mib.entryType],
                                      pEntry->mib.name);
                return L7_FAILURE;
              }
            }

            classRuleIndex = pRuleEntry->chainIndex;

          } /* endwhile classRuleIndex */

        } /* endwhile refClassIndex */
  
        instIndex = pInstEntry->chainIndex;     /* next instance for this policy */
  
      } /* endwhile instIndex */
    } /* endif feature present check */
  } /* endif policy type out */

  /* check mixed ipv4/ipv6 policy instance feature support */
  if (cnfgrIsFeaturePresent(dsCompId,
                            L7_DIFFSERV_POLICY_MIXED_IPV6_FEATURE_ID)
      != L7_TRUE)
  {
    savedL3Protocol = L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_NONE;    /* init */

    instIndex = pEntry->chainIndex;           /* first instance for this policy */

    while (0 != instIndex)
    {
      pInstEntry = &pCfgData->policyInst[instIndex];

      classIndex = pInstEntry->mib.classIndex;
      pClassEntry = &pCfgData->classHdr[classIndex];
      classL3Protocol = pClassEntry->mib.l3Protocol;

      /* l3 protocol of all instance classes must be the same (ignoring those
       * whose class mib.l3Protocol field is not set)
       *
       * no need to scan refClass chain, since all classes within a chain
       * are guaranteed to have the same l3Protocol value
       */
      if (L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_NONE != classL3Protocol)
      {
        if (L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_NONE == savedL3Protocol)
        {
          savedL3Protocol = classL3Protocol;    /* cache first valid value */
        }
        else
        {
          if (classL3Protocol != savedL3Protocol)
          {
            /* inconsistent l3Protocol among classes comprising this policy definition */
            DIFFSERV_CLUSTER_DLOG("%s: inconsistent l3Protocol among classes "
                                  "comprising policy \'%s\'\n",
                                  __FUNCTION__, pEntry->mib.name);
            return L7_FAILURE;
          }
        }
      }

      instIndex = pInstEntry->chainIndex;     /* next instance for this policy */

    } /* endwhile instIndex */

  } /* endif feature present check */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verify received DiffServ policy instance config against
*           system defines and features
*
* @param    L7_diffServCfg_t  *pCfgData   @{(input)} received DiffServ config data
* @param    L7_uint32         entryIndex  @b{(input)} config table array index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This assumes data has already been validated by the sender,
*           so we can skip basic checks, such as range checking.  However,
*           this system may not have the same prod/plat defines or features 
*           enabled, so need to fail if we cannot apply entire configuration.
*
* @end
*********************************************************************/
L7_RC_t diffServClusterConfigPolicyInstVerify(L7_diffServCfg_t *pCfgData,
                                              L7_uint32        entryIndex)
{
  L7_diffServPolicyInst_t     *pEntry = &pCfgData->policyInst[entryIndex];
  L7_uint32                   i, num = 0, lim = L7_DIFFSERV_ATTR_PER_INST_LIM;

  /* make sure current number of attributes per policy instance is within
   * supported limit
   *
   * NOTE: For accuracy, search the instIndexRef field in the Policy Attr table
   *       and count the number of occurrences of this policy instance 
   *       entryIndex.
   */
  for (i = L7_DIFFSERV_POLICY_ATTR_START; i < L7_DIFFSERV_POLICY_ATTR_MAX; i++)
  {
    if (pCfgData->policyAttr[i].instIndexRef == entryIndex)
    {
      num++;
    }
  }
  if (num > lim)
  {
    DIFFSERV_CLUSTER_DLOG("%s: too many attributes (%u) for instance %u of policy \'%s\' - max=%u\n",
                          __FUNCTION__, num, entryIndex, 
                          pCfgData->policyHdr[pEntry->hdrIndexRef].mib.name,
                          lim);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verify received DiffServ policy attribute config against
*           system defines and features
*
* @param    L7_diffServCfg_t  *pCfgData   @{(input)} received DiffServ config data
* @param    L7_uint32         entryIndex  @b{(input)} config table array index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This assumes data has already been validated by the sender,
*           so we can skip basic checks, such as range checking.  However,
*           this system may not have the same prod/plat defines or features 
*           enabled, so need to fail if we cannot apply entire configuration.
*
* @end
*********************************************************************/
L7_RC_t diffServClusterConfigPolicyAttrVerify(L7_diffServCfg_t *pCfgData,
                                              L7_uint32        entryIndex)
{
  L7_diffServPolicyAttr_t     *pEntry = &pCfgData->policyAttr[entryIndex];
  dsmibPolicyAttrType_t       entryType;
  dsmibPolicyAttrData_t       *pStmt;
  dsmibPolicyAttrType_t       type1, type2, type3;
  L7_uchar8                   markVal1, markVal2, markVal3;
  dsmibPoliceColor_t          colorMode1, colorMode2;
  nimConfigID_t               *pConfigId;
  L7_uint32                   intIfNum;

  entryType = pEntry->mib.entryType;
  pStmt = &pEntry->mib.stmt;

  /* check entry type feature support */
  if (L7_SUCCESS != dsmibPolicyAttrFeaturePresentCheck(entryType))
  {
    DIFFSERV_CLUSTER_DLOG("%s: policy attribute type \'%s\' not supported\n",
                          __FUNCTION__, dsmibPolicyAttrTypeStr[entryType]);
    return L7_FAILURE;
  }

  /* check assign queue ID within supported range */
  if (L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE == entryType)
  {
    if ((pStmt->assignQueue.qid < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MIN) ||
        (pStmt->assignQueue.qid > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MAX))
    {
      DIFFSERV_CLUSTER_DLOG("%s: assign queue ID (%u) out of range\n",
                            __FUNCTION__, pStmt->assignQueue.qid);
      return L7_FAILURE;
    }
  }

  /* check IP DSCP mark value */
  if (L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL == entryType)
  {
    if (L7_TRUE != policyAttrIpDscpValSupported[pStmt->markIpDscp.val])
    {
      DIFFSERV_CLUSTER_DLOG("%s: IP DSCP mark value (%u) not supported\n",
                            __FUNCTION__, pStmt->assignQueue.qid);
      return L7_FAILURE;
    }
  }

  /* check policing actions and any mark IP DSCP values
   *
   * convert police action to its comparable entry type and use it to
   * check the feature support
   */

  type1 = type2 = type3 = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
  markVal1 = markVal2 = markVal3 = 0;
  colorMode1 = colorMode2 = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_NONE;

  if (L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE == entryType)
  {
    type1 = _dsClusterPoliceActToEntryType(pStmt->policeSimple.conformAct);
    type3 = _dsClusterPoliceActToEntryType(pStmt->policeSimple.nonconformAct);
    if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP == type1)
    {
      markVal1 = pStmt->policeSimple.conformVal;
    }
    if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP == type3)
    {
      markVal3 = pStmt->policeSimple.nonconformVal;
    }
    colorMode1 = pStmt->policeSimple.colorConformMode;
  }
  else if (L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE == entryType)
  {
    type1 = _dsClusterPoliceActToEntryType(pStmt->policeSingleRate.conformAct);
    type2 = _dsClusterPoliceActToEntryType(pStmt->policeSingleRate.exceedAct);
    type3 = _dsClusterPoliceActToEntryType(pStmt->policeSingleRate.nonconformAct);
    if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP == type1)
    {
      markVal1 = pStmt->policeSingleRate.conformVal;
    }
    if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP == type2)
    {
      markVal2 = pStmt->policeSingleRate.exceedVal;
    }
    if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP == type3)
    {
      markVal3 = pStmt->policeSingleRate.nonconformVal;
    }
    colorMode1 = pStmt->policeSingleRate.colorConformMode;
    colorMode2 = pStmt->policeSingleRate.colorExceedMode;
  }
  else if (L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE == entryType)
  {
    type1 = _dsClusterPoliceActToEntryType(pStmt->policeTwoRate.conformAct);
    type2 = _dsClusterPoliceActToEntryType(pStmt->policeTwoRate.exceedAct);
    type3 = _dsClusterPoliceActToEntryType(pStmt->policeTwoRate.nonconformAct);
    if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP == type1)
    {
      markVal1 = pStmt->policeTwoRate.conformVal;
    }
    if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP == type2)
    {
      markVal2 = pStmt->policeTwoRate.exceedVal;
    }
    if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP == type3)
    {
      markVal3 = pStmt->policeTwoRate.nonconformVal;
    }
    colorMode1 = pStmt->policeTwoRate.colorConformMode;
    colorMode2 = pStmt->policeTwoRate.colorExceedMode;
  }
  /* check police action
   * (default value of 'none' is accepted if DiffServ is supported)
   */
  if ((L7_SUCCESS != dsmibPolicyAttrFeaturePresentCheck(type1)) ||
      (L7_SUCCESS != dsmibPolicyAttrFeaturePresentCheck(type2)) ||
      (L7_SUCCESS != dsmibPolicyAttrFeaturePresentCheck(type3)))
  {
    DIFFSERV_CLUSTER_DLOG("%s: police action not supported\n",
                          __FUNCTION__);
    return L7_FAILURE;
  }
  /* check police IP DSCP mark value
   * (default mark value of 0 is always considered supported) 
   */
  if ((L7_TRUE != policyAttrIpDscpValSupported[markVal1]) ||
      (L7_TRUE != policyAttrIpDscpValSupported[markVal2]) ||
      (L7_TRUE != policyAttrIpDscpValSupported[markVal3]))
  {
    DIFFSERV_CLUSTER_DLOG("%s: police IP DSCP mark value not supported\n",
                          __FUNCTION__);
    return L7_FAILURE;
  }
  /* check policing color-aware support */
  if (L7_TRUE != cnfgrIsFeaturePresent(dsCompId, 
                                       L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID))
  {
    if ((L7_TRUE == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_MODE_IS_AWARE(colorMode1)) ||
        (L7_TRUE == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_MODE_IS_AWARE(colorMode2)))
    {
      DIFFSERV_CLUSTER_DLOG("%s: police color aware mode not supported\n",
                            __FUNCTION__);
      return L7_FAILURE;
    }
  }

  /* check for valid mirror/redirect intf (u/s/p) */
  pConfigId = L7_NULLPTR;
  if (L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR == entryType)
  {
    pConfigId = &pStmt->mirror.configId;
  }
  else if (L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT == entryType)
  {
    pConfigId = &pStmt->redirect.configId;
  }
  /* get intIfNum and check for valid DiffServ interface  */
  if (L7_NULLPTR != pConfigId)
  {
    if ((L7_SUCCESS != nimIntIfFromConfigIDGet(pConfigId, &intIfNum)) ||
        (L7_TRUE != diffServIsValidIntf(intIfNum)))
    {
      DIFFSERV_CLUSTER_DLOG("%s: mirror/redirect DiffServ interface not valid\n",
                            __FUNCTION__);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Remap DiffServ service intf config from old to new policy indexes
*
* @param    L7_diffServCfg_t  *pCfgData   @{(input)} received DiffServ config data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The service intf table is NOT part of the cluster config
*           passed from sender to receiver.
* @end
*********************************************************************/
L7_RC_t diffServClusterConfigServiceIntfRemap(L7_diffServCfg_t *pCfgData)
{
  L7_diffServIntfCfg_t        *pEntry;
  L7_diffServPolicyHdr_t      *pPolicyOld, *pPolicyNew;
  L7_diffServService_t        *pService;
  dsmibServiceIfDir_t         dir;
  L7_uint32                   i, j;
  L7_uint32                   policyIndex;
  L7_uint32                   policyIndexXlate[L7_DIFFSERV_POLICY_HDR_MAX];

  /* gather current system operational config into temp config struct */
  if (diffServBuildUserConfigData(L7_DIFFSERV_CFG_VER_CURRENT, dsClusterTmpCfg)
      != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* copy all temp config entries into received config struct */
  for (i = 0; i < L7_DIFFSERV_INTF_MAX_COUNT; i++)
  {
    memcpy(&pCfgData->diffServIntfCfg[i], &dsClusterTmpCfg->diffServIntfCfg[i],
           sizeof(L7_diffServIntfCfg_t));
  }

  /* build a temporary policy index translation table */
  memset(policyIndexXlate, 0, sizeof(policyIndexXlate));

  for (i = L7_DIFFSERV_POLICY_HDR_START; i < L7_DIFFSERV_POLICY_HDR_MAX; i++)
  {
    pPolicyOld = &dsClusterTmpCfg->policyHdr[i];

    if (L7_TRUE != pPolicyOld->inUse)
    {
      break;
    }

    for (j = L7_DIFFSERV_POLICY_HDR_START; j < L7_DIFFSERV_POLICY_HDR_MAX; j++)
    {
      pPolicyNew = &pCfgData->policyHdr[j];

      if (L7_TRUE != pPolicyNew->inUse)
      {
        break;
      }

      if (0 == strncmp(pPolicyOld->mib.name, pPolicyNew->mib.name, 
                       sizeof(pPolicyOld->mib.name)))
      {
        /* found policy name match -- set new policy index in xlate table */
        policyIndexXlate[pPolicyOld->mib.index] = pPolicyNew->mib.index;
        break;
      }

    } /* endfor j*/

  } /* endfor i */

  /* adjust the policy index in each service {intf,direction} entry, as needed */
  for (i = 0; i < L7_DIFFSERV_INTF_MAX_COUNT; i++)
  {
    pEntry = &pCfgData->diffServIntfCfg[i];

    for (dir = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
         dir <= L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
         dir++)
    {

      if (L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN == dir)
      {
        pService = &pEntry->serviceIn;
      }
      else
      {
        pService = &pEntry->serviceOut;
      }

      if (L7_TRUE == pService->inUse)
      {
        policyIndex = pService->mib.policyIndex;
        if (0 != policyIndex)
        {
          if (0 != policyIndexXlate[policyIndex])
          {
            if (policyIndex != policyIndexXlate[policyIndex])
            {
              DIFFSERV_CLUSTER_DLOG("%s: Remapping service intf %u, dir %u from policy index %u to %u\n",
                                    __FUNCTION__, pService->mib.index, pService->mib.direction,
                                    pService->mib.policyIndex, policyIndexXlate[policyIndex]);
              pService->mib.policyIndex = policyIndexXlate[policyIndex];
            }
          }
          else
          {
            /* this policy is no longer available -- remove from intf config */
            LOG_MSG("%s: Obsolete policy index %u removed from service intf %u, dir \'%s\'.\n",
                    __FUNCTION__, policyIndex, pService->mib.index,
                    (L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN == dir) ? "in" : "out");
            pService->mib.policyIndex = 0;
          }
        }
      } /* endif inUse */

    } /* endfor dir */

  } /* endfor i */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verify received configuration against system defines and features
*
* @param    L7_diffServCfg_t  *pCfgData @{(input)} received DiffServ config data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This assumes data has already been validated by the sender,
*           so we can skip basic checks, such as range checking.  However,
*           this system may not have the same prod/plat defines or features 
*           enabled, so need to fail if we cannot apply entire configuration.
*
* @end
*********************************************************************/
L7_RC_t diffServClusterConfigVerify(L7_diffServCfg_t *pCfgData)
{
  L7_RC_t                     rc = L7_SUCCESS;
  dsmibTableId_t              tableID;
  L7_uint32                   i, startEnt, maxEnt;
  dsClusterTableParms_t       *pTable;

  DIFFSERV_CLUSTER_DLOG("ENTERING %s\n", __FUNCTION__);

  /* iterate through each table ID */
  for (tableID = DSMIB_TABLE_ID_CLASS; tableID <= DSMIB_TABLE_ID_POLICY_ATTR; tableID++)
  {
    pTable = &dsClusterTableParms[tableID];

    startEnt = pTable->startEnt; 
    maxEnt   = pTable->maxEnt;

    /* if validation fn ptr is null, assume there is nothing to check */
    if (L7_NULLPTR == pTable->verifyFn)
    {
      continue;
    }

    for (i = startEnt; i < maxEnt; i++)
    {
      /* array entry not in use means end of the configured list */
      if (_dsClusterConfigEntryInUseGet(tableID, pCfgData, i) != L7_TRUE)
        break;

      /* invoke table-specific validation function to check applicability of 
       * config on this system
       */
      rc = pTable->verifyFn(pCfgData, i);
      if (L7_SUCCESS != rc)
      {
        LOG_MSG("%s: Failed verification check for received DiffServ %s table config\n",
                __FUNCTION__, dsmibPrvtTableIdStr[tableID]);
        break;
      }

    } /* endfor i */

    /* check for errors */
    if (L7_SUCCESS != rc)
    {
      break;
    }

  } /* endfor tableID */

  /* create new service intf config from the existing config by mapping
   * policy indexes from old to new via policy name lookup
   *
   * NOTE: Any policy index translation failures cause a log msg to be
   *       generated and result in the removal of the non-existent policy
   *       from the service interface, but does not result in a config failure.
   */
  if (diffServClusterConfigServiceIntfRemap(pCfgData) != L7_SUCCESS)
  {
    LOG_MSG("%s: Error mapping DiffServ service interface config.\n",
            __FUNCTION__, dsmibPrvtTableIdStr[tableID]);
    rc = L7_FAILURE;
  }

  DIFFSERV_CLUSTER_DLOG("LEAVING %s (rc = %d)\n", __FUNCTION__, rc);

  return rc;
}

/*********************************************************************
* @purpose  Function to apply entire received configuration
*
* @param    void
*
* @returns  void
*
* @notes    This function is not allowed to fail. There is no way to 
*           abort once the configuration has been verified successfully.
*
* @end
*********************************************************************/
void diffServClusterConfigApply(void)
{
  L7_BOOL         needDefault = L7_FALSE;
  L7_char8        *pMsg = L7_NULLPTR;

  DIFFSERV_CLUSTER_DLOG("ENTERING %s\n", __FUNCTION__);

  /* save current system operational config into temp config struct
   * for a possible restore scenario
   */
  if (diffServBuildUserConfigData(L7_DIFFSERV_CFG_VER_CURRENT, dsClusterTmpCfg)
      != L7_SUCCESS)
  {
    needDefault = L7_TRUE;
  }

  /* apply received config (with modified service intf info) to switch */
  if (diffServReplace(dsClusterRxCfg) != L7_SUCCESS)
  {
    LOG_MSG("%s: Unexpected error applying received DiffServ config to switch.\n", 
            __FUNCTION__);

    /* try restoring config to what it was */
    if ((needDefault == L7_FALSE) &&
        (diffServReplace(dsClusterTmpCfg) == L7_SUCCESS))
    {
      pMsg = "Previously existing DiffServ config successfully restored.";
    }
    else
    {
      /* restore using default config */
      if (diffServRestore() == L7_SUCCESS)
      {
        pMsg = "DiffServ config has been reset to factory defaults.";
      }
      else
      {
        pMsg = "Error setting DiffServ config to factory defaults. Please reset device.";
      }
    }

    /* log additional information regarding the outcome */
    if (pMsg != L7_NULLPTR)
    {
      LOG_MSG("%s: %s\n", __FUNCTION__, pMsg);
    }
  }

  DIFFSERV_CLUSTER_DLOG("LEAVING %s\n", __FUNCTION__);

  return;
}

/*********************************************************************
* @purpose  Function to process configuration receive commands
*
* @param    clusterConfigCmd  cmd  @b{(input)} cluster member
*
* @returns  void
*
* @notes  
*
* @end
*********************************************************************/
void diffServClusterConfigRxCmdProcess(clusterConfigCmd cmd)
{
  L7_RC_t rc = L7_FAILURE;

  switch (cmd)
  {
    case CLUSTER_CONFIG_CMD_START_TRANSFER:
    case CLUSTER_CONFIG_CMD_ABORT:
      if (CLUSTER_CONFIG_CMD_START_TRANSFER == cmd)
      {
        DIFFSERV_CLUSTER_DLOG("%s: CLUSTER START received\n", __FUNCTION__);
      }
      else
      {
        DIFFSERV_CLUSTER_DLOG("%s: CLUSTER ABORT received\n", __FUNCTION__);
      }
      /* initialize temporary structure to receive entire configuration from peer */
      _dsClusterCfgStructInit(dsClusterRxCfg, "dsClusterRxCfg");
      /* initialize variables used for debugging */
      dsClusterRxCfgMsgStarted = L7_FALSE;
      dsClusterRxCfgIeExpected = DS_CLUSTER_MSG_IE_NONE;
      dsClusterRxError = L7_FALSE;
      dsClusterRxInProg = (CLUSTER_CONFIG_CMD_START_TRANSFER == cmd) ? L7_TRUE : L7_FALSE;
      break;

    case CLUSTER_CONFIG_CMD_END_TRANSFER:
      DIFFSERV_CLUSTER_DLOG("%s: CLUSTER END received (rxError=%u)\n",
                            __FUNCTION__, dsClusterRxError);
      if (L7_TRUE == dsClusterRxError)
      {
        rc = L7_FAILURE;
      } 
      else
      {
        rc = diffServClusterConfigVerify(dsClusterRxCfg);
        if (L7_SUCCESS != rc)
        {
          DIFFSERV_CLUSTER_DLOG("%s: DiffServ received config verification failed.\n", __FUNCTION__);
          LOG_MSG("%s: DiffServ received config verification failed.\n", __FUNCTION__);
        }
      }
      if (clusterConfigRxDone(CLUSTER_CFG_ID(QOS_DIFFSERV), rc) != L7_SUCCESS)
      {
        LOG_MSG("%s: clusterConfigRxDone failed.\n", __FUNCTION__);
      }
      if (L7_SUCCESS != rc)
      {
        dsClusterRxInProg = L7_FALSE;
      }
      DIFFSERV_CLUSTER_DLOG("%s: CLUSTER END completed (rxError=%u, rc=%u)\n",
                            __FUNCTION__, dsClusterRxError, rc);
      break;

    case CLUSTER_CONFIG_CMD_APPLY:
      DIFFSERV_CLUSTER_DLOG("%s: CLUSTER APPLY received\n", __FUNCTION__);
      diffServClusterConfigApply();
      clusterConfigRxApplyDone(CLUSTER_CFG_ID(QOS_DIFFSERV));
      dsClusterRxInProg = L7_FALSE;
      DIFFSERV_CLUSTER_DLOG("%s: CLUSTER APPLY completed\n", __FUNCTION__);
      break;

    default:
      DIFFSERV_CLUSTER_DLOG("%s: CLUSTER <UNKNOWN> received (cmd=%u)\n",
                            __FUNCTION__, cmd);
      LOG_MSG("%s: Received unknown clusterConfigCmd=%u\n", __FUNCTION__, cmd);
      break;
  }

  return;
}

/*********************************************************************
* @purpose  Indicates if DiffServ cluster config receive operation currently in progress
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*
* @end
*********************************************************************/
L7_BOOL diffServClusterIsRxInProgress(void)
{
  return dsClusterRxInProg;
}

/*********************************************************************
* @purpose  Translate a config push policy name into its associated Policy Table index
*
* @param    stringPtr     @b{(input)}  Policy name string pointer
* @param    pPolicyIndex  @b{(output)} Pointer to policy index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Searches the received config push data for a matching
*           policy name.
*
* @end
*********************************************************************/
L7_RC_t diffServClusterPolicyNameToIndex(L7_uchar8 *stringPtr, 
                                         L7_uint32 *pPolicyIndex)
{
  dsmibTableId_t              tableID = DSMIB_TABLE_ID_POLICY;
  L7_diffServCfg_t            *pCfgData = dsClusterRxCfg;
  L7_uint32                   len;
  L7_uint32                   i, startEnt, maxEnt;
  dsClusterTableParms_t       *pTable;
  L7_diffServPolicyHdr_t      *pEntry;

  /* check inputs */
  if ((stringPtr == L7_NULLPTR) ||
      (pPolicyIndex == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  /* check the length of the policy name string */
  len = (L7_uint32)strlen((char *)stringPtr);
  if ((len == 0) || (len > L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX))
  {
    return L7_FAILURE;
  }

  /* config push RX data only valid while push receive operation still in progress
   * (i.e. prior to the conclusion of the data apply)
   */
  if (diffServClusterIsRxInProgress() != L7_TRUE)
  {
    return L7_FAILURE;
  }

  pTable = &dsClusterTableParms[tableID];

  startEnt = pTable->startEnt; 
  maxEnt   = pTable->maxEnt;

  for (i = startEnt; i < maxEnt; i++)
  {
    pEntry = &pCfgData->policyHdr[i];

    /* array entry not in use means end of the configured list */
    if (pEntry->inUse != L7_TRUE)
      break;

    /* compare against name in existing row */
    if (strcmp((char *)stringPtr, (char *)pEntry->mib.name) == 0)
    {
      *pPolicyIndex = pEntry->mib.index;
      return L7_SUCCESS;
    }

  } /* endfor i */

  *pPolicyIndex = 0;
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Function to enable/disable debug via devshell
*
* @param    L7_BOOL debug  @b{(input)} L7_TRUE/L7_FALSE
*
* @returns  void
*
* @notes  
*
* @end
*********************************************************************/
void diffServClusterDebugSet(L7_BOOL debug)
{
  dsClusterDebug = debug;
  return;
}
