/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename acl_cluster.c
*
* @purpose Access Control Lists - support for clustering
*
* @component Access Control List
*
* @comments
*
* @create 02/08/2008
*
* @author dfowler
*
* @end
*
**********************************************************************/
#include <string.h>
#include "commdefs.h"
#include "osapi.h"
#include "osapi_support.h"
#include "log.h"
#include "l7utils_api.h"
#include "comm_mask.h"
#include "avl_api.h"
#include "acl.h"
#include "acl_api.h"
#include "acl_cluster.h"
#include "clustering_commdefs.h"
#include "clustering_api.h"

/* global operational structures */
extern L7_uint32         acl_curr_entries_g;    /* total current list entries */
extern L7_uint32         aclRuleNodeCount;
extern avlTree_t        *pAclTree;
extern avlTree_t        *pAclMacTree;
extern osapiRWLock_t     aclRwLock;
extern aclCfgFileData_t *aclCfgFileData;

/* used to track route notifies on config apply */
static aclClusterNotifyData_t *pAclNotifyList = L7_NULLPTR;

/* copy of data received from cluster member switch */
static aclStructure_t    *pAclRxList = L7_NULLPTR;
static aclMacStructure_t *pAclMacRxList = L7_NULLPTR;
static L7_ushort16        aclRxCount = 0;
static L7_ushort16        aclMacRxCount = 0;
static L7_uint32          aclRxRuleCount = 0;    /* total rules received to verify against L7_ACL_MAX_RULES */
static L7_BOOL            aclRxInProg = L7_FALSE; /* indicates Rx operation currently in progress */
static L7_BOOL            aclRxError = L7_FALSE; /* indicates unrecoverable error receiving, drop remaining messages */


static L7_int32 aclClusterTaskId = L7_ERROR;
static void    *aclClusterQueue  = L7_NULLPTR;
static L7_BOOL  aclClusterDebug  = L7_FALSE;

/*********************************************************************
* @purpose  Check if room in message buffer to add IE, send and clear 
*           buffer when max message size is reached.
*
* @param    L7_uint32          size      @b{(input)} IE size to write
* @param    clusterMemberID_t *memberID  @b{(input)} cluster member
* @param    L7_uchar8         *buffer    @b{(input/output)} message buffer
* @param    L7_uint32         *offset    @b{(input/output)} offset in buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes 
*           
* @end
*********************************************************************/
static L7_RC_t aclClusterConfigMsgSendCheck(L7_uint32          size,
                                            clusterMemberID_t *memberID,
                                            L7_uchar8         *buffer,
                                            L7_uint32         *offset)
{
  if (((*offset) + size) >= ACL_CLUSTER_MSG_SIZE_MAX)
  {
    if (clusterMsgSend(memberID, CLUSTER_MSG_DELIVERY_RELIABLE,
                       ACL_CLUSTER_MSG_CONFIG,
                       (*offset), buffer) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    memset(buffer, 0, ACL_CLUSTER_MSG_SIZE_MAX);
    (*offset) = 0;
  }
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
L7_RC_t aclClusterPhase1Init(void)
{
  /* allocate memory to receive ACLs, must receive in a copy
     to verify received configuration before apply, must be able to abort */
  pAclRxList = (aclStructure_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                             (sizeof(aclStructure_t)*L7_ACL_MAX_LISTS));
  if (pAclRxList == L7_NULLPTR)
  {
    LOG_MSG("%s: osapiMalloc failed.\n", __FUNCTION__);
    return L7_FAILURE;
  }
  aclRxCount = 0;

  pAclMacRxList = (aclMacStructure_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                   (sizeof(aclMacStructure_t)*L7_ACL_MAX_LISTS));
  if (pAclMacRxList == L7_NULLPTR)
  {
    LOG_MSG("%s: osapiMalloc failed.\n", __FUNCTION__);
    return L7_FAILURE;
  }
  aclMacRxCount = 0;

  /* worst case we need to track deleting max ACLs and adding max ACLs */
  pAclNotifyList = (aclClusterNotifyData_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                         (sizeof(aclClusterNotifyData_t)*(L7_ACL_MAX_LISTS*2)));
  if (pAclNotifyList == L7_NULLPTR)
  {
    LOG_MSG("%s: osapiMalloc failed.\n", __FUNCTION__);
    return L7_FAILURE;
  }

  aclClusterQueue = (void *)osapiMsgQueueCreate("aclClusterQueue", 
                                                ACL_CLUSTER_QUEUE_SIZE,
                                                ACL_CLUSTER_QUEUE_SIZE * sizeof(aclClusterQueueMsg_t));
  if (aclClusterQueue == L7_NULLPTR)
  {
    LOG_MSG("%s: osapiMsgQueueCreate failed.\n", __FUNCTION__);
    return L7_FAILURE;
  }

  aclClusterTaskId = osapiTaskCreate("aclClusterTask", (void *)aclClusterTask,
                                     0, 0, 
                                     L7_DEFAULT_STACK_SIZE,
                                     L7_DEFAULT_TASK_PRIORITY,
                                     L7_DEFAULT_TASK_SLICE);

  if ((aclClusterTaskId == L7_ERROR) ||
      (osapiWaitForTaskInit(L7_QOS_ACL_CLUSTER_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS))
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
L7_RC_t aclClusterPhase2Init(void)
{
  if (clusterMsgRegister(ACL_CLUSTER_MSG_CONFIG, 
                         CLUSTER_MSG_DELIVERY_RELIABLE,
                         aclClusterMsgCallback) != L7_SUCCESS)
  {
    LOG_MSG("%s: clusterMsgRegister failed.\n", __FUNCTION__);
    return L7_FAILURE;
  }

  if (clusterConfigRegister(CLUSTER_CFG_ID(QOS_ACL),
                            aclClusterConfigSendCallback,
                            aclClusterConfigRxCallback) != L7_SUCCESS)
  {
    LOG_MSG("%s: clusterConfigRegister failed.\n", __FUNCTION__);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Allocate memory and add new rule to IPv4/IPv6 ACL
*
* @param    aclStructure_t *pAcl @b{(input)} pointer to ACL data
*
* @returns  aclRuleParms_t *
*
* @notes    We should never allocate memory outside of phase 1, but 
*           since the rest of the component dynamically allocates/frees 
*           memory to manage the ACL rules, we have to do the same,
*           our aclStructure_t is added directly to the global aclTree.
*           
* @end
*********************************************************************/
aclRuleParms_t *aclClusterIpAclRuleNodeAlloc(aclStructure_t *pAcl)
{
  aclRuleParms_t *pRule = (aclRuleParms_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID, 
                                                        sizeof(aclRuleParms_t));

  /* add rule to end of list, can assume sending switch has already ordered by ruleNum */
  if (pRule != L7_NULLPTR)
  {
    pRule->next = L7_NULLPTR;
    pRule->last = L7_TRUE;
    if (pAcl->head == L7_NULLPTR)
    {
      pAcl->head = pRule;
    }
    else
    {
      aclRuleParms_t *prev = pAcl->head;
      while (prev->next != L7_NULLPTR)
      {
        prev = prev->next;
      }
      prev->next = pRule;
      prev->last = L7_FALSE;
    }
  }
  return pRule;
}

/*********************************************************************
* @purpose  Allocate memory and add new rule to MAC ACL
*
* @param    aclMacStructure_t *pMacAcl @b{(input)} pointer to ACL data
*
* @returns  aclMacRuleParms_t *
*
* @notes    We should never allocate memory outside of phase 1, but 
*           since the rest of the component dynamically allocates/frees 
*           memory to manage the ACL rules, we have to do the same,
*           our aclStructure_t is added directly to the global aclMacTree.
*           
* @end
*********************************************************************/
aclMacRuleParms_t *aclClusterMacAclRuleNodeAlloc(aclMacStructure_t *pMacAcl)
{
  aclMacRuleParms_t *pRule = (aclMacRuleParms_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID, 
                                                              sizeof(aclMacRuleParms_t));

  /* add rule to end of list, can assume sending switch has already ordered by ruleNum */
  if (pRule != L7_NULLPTR)
  {
    pRule->next = L7_NULLPTR;
    pRule->last = L7_TRUE;
    if (pMacAcl->head == L7_NULLPTR)
    {
      pMacAcl->head = pRule;
    }
    else
    {
      aclMacRuleParms_t *prev = pMacAcl->head;
      while (prev->next != L7_NULLPTR)
      {
        prev = prev->next;
      }
      prev->next = pRule;
      prev->last = L7_FALSE;
    }
  }
  return pRule;
}

/*********************************************************************
* @purpose  Task to wait on cluster messages and events
*
* @param    void
*
* @returns  void
*
* @notes    clustering requires processing messages and commands in 
*           a separate task context, a component must not block the
*           clustering task. 
*           
* @end
*********************************************************************/
void aclClusterTask(void)
{
  aclClusterQueueMsg_t queueMsg;
  L7_RC_t              rc;

  osapiTaskInitDone(L7_QOS_ACL_CLUSTER_TASK_SYNC);

  while (L7_TRUE)
  {
    if (osapiMessageReceive(aclClusterQueue, 
                            (void *)&queueMsg, sizeof(aclClusterQueueMsg_t),
                            L7_WAIT_FOREVER) == L7_SUCCESS)
    {
      switch (queueMsg.type)
      {
        case ACL_CLUSTER_QUEUE_MSG_CONFIG_MSG:
          if (aclRxError == L7_FALSE)
          {
            aclClusterConfigMsgProcess(&queueMsg.content.msg);
          }
          break;
        case ACL_CLUSTER_QUEUE_MSG_CONFIG_SEND:
          rc = aclClusterConfigSend(&queueMsg.content.memberID);
          if (clusterConfigSendDone(CLUSTER_CFG_ID(QOS_ACL), rc) != L7_SUCCESS)
          {
            LOG_MSG("%s: clusterConfigSendDone failed\n", __FUNCTION__);
          }
          break;
        case ACL_CLUSTER_QUEUE_MSG_CONFIG_RX:
          aclClusterConfigRxCmdProcess(queueMsg.content.configCmd);
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
* @param    L7_uchar8                msgBuf    @b{(input)} contents of message
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
void aclClusterMsgCallback(L7_ushort16              msgType,
                           clusterMsgDeliveryMethod method,
                           L7_uint32                msgLength,
                           L7_uchar8               *msgBuf)
{
  aclClusterQueueMsg_t queueMsg;

  ACL_CLUSTER_DLOG("%s: msgType=0x%4.4X\n", __FUNCTION__, msgType);

  if (msgLength > sizeof(queueMsg.content.msg.buffer))
  {
    LOG_MSG("%s: Received invalid cluster message length, msgLength=%u\n", 
            __FUNCTION__, msgLength);
    return;
  }

  memset(&queueMsg, 0, sizeof(aclClusterQueueMsg_t));
  switch (msgType)
  {
    case ACL_CLUSTER_MSG_CONFIG:
      queueMsg.type = ACL_CLUSTER_QUEUE_MSG_CONFIG_MSG;
      queueMsg.content.msg.length = msgLength;
      memcpy(queueMsg.content.msg.buffer, msgBuf, msgLength);
      break;
    default:
      LOG_MSG("%s: Received invalid cluster message type, msgType=0x%4.4X\n", 
              __FUNCTION__, msgType);
      return;
  }

  /* send message to process on aclClusterTask */
  if (osapiMessageSend(aclClusterQueue, (void *)&queueMsg, 
                       sizeof(aclClusterQueueMsg_t), 
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
void aclClusterConfigSendCallback(clusterMemberID_t *memberID)
{
  aclClusterQueueMsg_t queueMsg;

  ACL_CLUSTER_DLOG("ENTERING %s\n", __FUNCTION__);

  if (memberID == L7_NULLPTR)
  {
    LOG_MSG("%s: received NULL cluster member ID\n", __FUNCTION__);
    return;
  }

  memset(&queueMsg, 0, sizeof(aclClusterQueueMsg_t));
  queueMsg.type = ACL_CLUSTER_QUEUE_MSG_CONFIG_SEND;
  memcpy(&queueMsg.content.memberID, memberID, sizeof(clusterMemberID_t));

  /* send message to process on aclClusterTask */
  if (osapiMessageSend(aclClusterQueue, (void *)&queueMsg, 
                       sizeof(aclClusterQueueMsg_t), 
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
void aclClusterConfigRxCallback(clusterConfigCmd cmd)
{
  aclClusterQueueMsg_t queueMsg;

  ACL_CLUSTER_DLOG("%s: cmd=%u\n", __FUNCTION__, cmd);

  memset(&queueMsg, 0, sizeof(aclClusterQueueMsg_t));
  queueMsg.type = ACL_CLUSTER_QUEUE_MSG_CONFIG_RX;
  queueMsg.content.configCmd = cmd;

  /* send message to process on aclClusterTask */
  if (osapiMessageSend(aclClusterQueue, (void *)&queueMsg, 
                       sizeof(aclClusterQueueMsg_t), 
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("%s: osapiMessageSend failed, queueMsg.type=%u\n", 
            __FUNCTION__, queueMsg.type);
  }

  return;
}

/*********************************************************************
* @purpose  Function to read IPv4/IPv6 rules to operational structure
*
* @param    L7_ACL_TYPE_t    type   @b{(input)} IPv4 or IPv6
* @param    L7_uchar8       *buffer @b{(input)} pointer to rule config
* @param    L7_uint32       *offset @b{(input/output)} current offset in buffer
* @param    aclRuleParms_t  *pRule  @b{(input/output)} pointer to rule data
*
* @returns  void
*
* @notes  
*
* @end
*********************************************************************/
void aclClusterConfigIpAclRuleParse(L7_ACL_TYPE_t    type,
                                    L7_uchar8       *buffer,
                                    L7_uint32       *offset,
                                    aclRuleParms_t  *pRule)
{
  /* Bytes 00-03 Config Mask */
  CLUSTER_PKT_INT32_GET(pRule->configMask, buffer, *offset);
  /* Bytes 04-07 Rule index */
  CLUSTER_PKT_INT32_GET(pRule->ruleNum, buffer, *offset);
  /* Bytes 08-11 Rule action */
  CLUSTER_PKT_INT32_GET(pRule->action, buffer, *offset);
  /* Bytes 12-15 Assign queue Id */
  CLUSTER_PKT_INT32_GET(pRule->assignQueueId, buffer, *offset);

  /* Bytes 16-23 Redirect config Id*/
  CLUSTER_PKT_INT32_GET(pRule->redirectConfigId.type, buffer, *offset);
  CLUSTER_PKT_INT8_GET(pRule->redirectConfigId.configSpecifier.usp.unit, buffer, *offset);
  CLUSTER_PKT_INT8_GET(pRule->redirectConfigId.configSpecifier.usp.slot, buffer, *offset);
  CLUSTER_PKT_INT16_GET(pRule->redirectConfigId.configSpecifier.usp.port, buffer, *offset);

  /* Bytes 24-31 Mirror config Id*/
  CLUSTER_PKT_INT32_GET(pRule->mirrorConfigId.type, buffer, *offset);
  CLUSTER_PKT_INT8_GET(pRule->mirrorConfigId.configSpecifier.usp.unit, buffer, *offset);
  CLUSTER_PKT_INT8_GET(pRule->mirrorConfigId.configSpecifier.usp.slot, buffer, *offset);
  CLUSTER_PKT_INT16_GET(pRule->mirrorConfigId.configSpecifier.usp.port, buffer, *offset);

  /* Byte 32 - Logging flag */
  CLUSTER_PKT_INT8_GET(pRule->logging, buffer, *offset);
  /* Byte 33 - Match every flag */
  CLUSTER_PKT_INT8_GET(pRule->every, buffer, *offset);
  /* Byte 34 - Protocol number */
  CLUSTER_PKT_INT8_GET(pRule->protocol, buffer, *offset);
  /* Byte 35 - Protocol mask */
  CLUSTER_PKT_INT8_GET(pRule->protmask, buffer, *offset);

  /* Bytes 36-55 Src Ipv4/Ipv6 */
  if (type == L7_ACL_TYPE_IP)
  {
    CLUSTER_PKT_INT32_GET(pRule->srcIp.v4.addr, buffer, *offset);
    CLUSTER_PKT_INT32_GET(pRule->srcIp.v4.mask, buffer, *offset);
    *offset += (3 * sizeof(L7_uint32)); /* reserved */
  } else
  {
    CLUSTER_PKT_STRING_GET(&pRule->srcIp.v6.in6Addr, sizeof(L7_in6_addr_t), 
                           buffer, *offset);
    CLUSTER_PKT_INT32_GET(pRule->srcIp.v6.in6PrefixLen, buffer, *offset);
  }

  /* Bytes 56-57 Src L4 port number */
  CLUSTER_PKT_INT16_GET(pRule->srcPort, buffer, *offset);
  /* Bytes 58-59 Src L4 port start */
  CLUSTER_PKT_INT16_GET(pRule->srcStartPort, buffer, *offset);
  /* Bytes 60-61 Src L4 port end */
  CLUSTER_PKT_INT16_GET(pRule->srcEndPort, buffer, *offset);
  *offset += sizeof(L7_ushort16);  /* reserved */

  /* Bytes 64-83 Dst Ipv4/Ipv6 */
  if (type == L7_ACL_TYPE_IP)
  {
    CLUSTER_PKT_INT32_GET(pRule->dstIp.v4.addr, buffer, *offset);
    CLUSTER_PKT_INT32_GET(pRule->dstIp.v4.mask, buffer, *offset);
    *offset += (3 * sizeof(L7_uint32)); /* reserved */
  } else
  {
    CLUSTER_PKT_STRING_GET(&pRule->dstIp.v6.in6Addr, sizeof(L7_in6_addr_t), 
                           buffer, *offset);
    CLUSTER_PKT_INT32_GET(pRule->dstIp.v6.in6PrefixLen, buffer, *offset);
  }

  /* Bytes 84-85 Dst L4 port number */
  CLUSTER_PKT_INT16_GET(pRule->dstPort, buffer, *offset);
  /* Bytes 86-87 Dst L4 port start */
  CLUSTER_PKT_INT16_GET(pRule->dstStartPort, buffer, *offset);
  /* Bytes 88-89 Dst L4 port end */
  CLUSTER_PKT_INT16_GET(pRule->dstEndPort, buffer, *offset);

  /* Byte 90 ToS value */
  CLUSTER_PKT_INT8_GET(pRule->tosbyte, buffer, *offset);
  /* Byte 91 ToS mask */
  CLUSTER_PKT_INT8_GET(pRule->tosmask, buffer, *offset);

  /* Bytes 92-95 Ipv6 flow label */
  CLUSTER_PKT_INT32_GET(pRule->flowlbl, buffer, *offset);

  return;
}

/*********************************************************************
* @purpose  Function to read MAC rules to operational structure
*
* @param    L7_uchar8         *buffer @b{(input)} pointer to rule config
* @param    L7_uint32         *offset @b{(input/output)} current offset in buffer
* @param    aclMacRuleParms_t *pRule  @b{(input/output)} pointer to rule data
*
* @returns  void
*
* @notes  
*
* @end
*********************************************************************/
void aclClusterConfigMacAclRuleParse(L7_uchar8          *buffer,
                                     L7_uint32          *offset,
                                     aclMacRuleParms_t  *pRule)
{
  /* Bytes 00-03 Config Mask */
  CLUSTER_PKT_INT32_GET(pRule->configMask, buffer, *offset);
  /* Bytes 04-07 Rule index */
  CLUSTER_PKT_INT32_GET(pRule->ruleNum, buffer, *offset);
  /* Bytes 08-11 Rule action */
  CLUSTER_PKT_INT32_GET(pRule->action, buffer, *offset);
  /* Bytes 12-15 Assign queue Id */
  CLUSTER_PKT_INT32_GET(pRule->assignQueueId, buffer, *offset);

  /* Bytes 16-23 Redirect config Id*/
  CLUSTER_PKT_INT32_GET(pRule->redirectConfigId.type, buffer, *offset);
  CLUSTER_PKT_INT8_GET(pRule->redirectConfigId.configSpecifier.usp.unit, buffer, *offset);
  CLUSTER_PKT_INT8_GET(pRule->redirectConfigId.configSpecifier.usp.slot, buffer, *offset);
  CLUSTER_PKT_INT16_GET(pRule->redirectConfigId.configSpecifier.usp.port, buffer, *offset);

  /* Bytes 24-31 Mirror config Id*/
  CLUSTER_PKT_INT32_GET(pRule->mirrorConfigId.type, buffer, *offset);
  CLUSTER_PKT_INT8_GET(pRule->mirrorConfigId.configSpecifier.usp.unit, buffer, *offset);
  CLUSTER_PKT_INT8_GET(pRule->mirrorConfigId.configSpecifier.usp.slot, buffer, *offset);
  CLUSTER_PKT_INT16_GET(pRule->mirrorConfigId.configSpecifier.usp.port, buffer, *offset);

  /* Byte 32 - Logging flag */
  CLUSTER_PKT_INT8_GET(pRule->logging, buffer, *offset);
  /* Byte 33 - Match every flag */
  CLUSTER_PKT_INT8_GET(pRule->every, buffer, *offset);

  /* Byte 34 - CoS priority */
  CLUSTER_PKT_INT8_GET(pRule->cos, buffer, *offset);
  /* Byte 35 - secondary CoS priority */
  CLUSTER_PKT_INT8_GET(pRule->cos2, buffer, *offset);

  /* Bytes 36-43 Dst MAC address */
  CLUSTER_PKT_MAC_ADDR_GET(pRule->dstMac, buffer, *offset);
  /* Bytes 44-51 Dst MAC mask */
  CLUSTER_PKT_MAC_ADDR_GET(pRule->dstMacMask, buffer, *offset);

  /* Bytes 52-53 Ethertype key Id */
  CLUSTER_PKT_INT16_GET(pRule->etypeKeyId, buffer, *offset);
  /* Bytes 54-55 Ethertype custom value */
  CLUSTER_PKT_INT16_GET(pRule->etypeValue, buffer, *offset);

  /* Bytes 56-61 Src MAC address */
  CLUSTER_PKT_MAC_ADDR_GET(pRule->srcMac, buffer, *offset);
  /* Bytes 62-67 Src MAC mask */
  CLUSTER_PKT_MAC_ADDR_GET(pRule->srcMacMask, buffer, *offset);

  /* Bytes 68-69 VLAN Id (single) */
  CLUSTER_PKT_INT16_GET(pRule->vlanId, buffer, *offset);
  /* Bytes 70-71 VLAN Id range start */
  CLUSTER_PKT_INT16_GET(pRule->vlanIdStart, buffer, *offset);
  /* Bytes 72-73 VLAN Id range end */
  CLUSTER_PKT_INT16_GET(pRule->vlanIdEnd, buffer, *offset);

  /* Bytes 74-75 Secondary VLAN Id (single) */
  CLUSTER_PKT_INT16_GET(pRule->vlanId2, buffer, *offset);
  /* Bytes 76-77 Secondary VLAN Id range start */
  CLUSTER_PKT_INT16_GET(pRule->vlanId2Start, buffer, *offset);
  /* Bytes 78-79 Secondary VLAN Id range end */
  CLUSTER_PKT_INT16_GET(pRule->vlanId2End, buffer, *offset);

  return;
}

/*********************************************************************
* @purpose  Function to process cluster config message
*
* @param    aclClusterMsg_t *msg   @b{(input)} received message
*
* @returns  void
*
* @notes  
*
* @end
*********************************************************************/
void aclClusterConfigMsgProcess(aclClusterMsg_t *msg)
{
  static L7_uint32 currAclIndex = 0;
  static aclStructure_t *pAcl = L7_NULLPTR;
  static aclMacStructure_t *pMacAcl = L7_NULLPTR;
  static L7_ushort16 ruleCount = 0;

  clusterMsgElementHdr_t element;
  L7_uchar8 *buffer = msg->buffer;
  L7_uint32 offset = 0;

  ACL_CLUSTER_DLOG("ENTERING %s: message length=%u\n", __FUNCTION__, msg->length); 

  while (offset < msg->length)
  { 
    CLUSTER_PKT_ELEMENT_HDR_GET(element, msg->buffer, offset);
    switch (element.elementId)
    {
      case ACL_CLUSTER_MSG_IE_CONFIG_START:
        CLUSTER_PKT_ELEMENT_SIZE_CHECK(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                       aclClusterDebug, element, 
                                       ACL_CLUSTER_MSG_IE_CONFIG_START_SIZE, 
                                       offset);
        offset += ACL_CLUSTER_MSG_IE_CONFIG_START_SIZE;
        currAclIndex = 0;
        ruleCount = 0;
        pAcl = L7_NULLPTR;
        pMacAcl = L7_NULLPTR;
        break;
      case ACL_CLUSTER_MSG_IE_CONFIG_END:
        CLUSTER_PKT_ELEMENT_SIZE_CHECK(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                       aclClusterDebug, element, 
                                       ACL_CLUSTER_MSG_IE_CONFIG_END_SIZE, 
                                       offset);
        offset += ACL_CLUSTER_MSG_IE_CONFIG_END_SIZE;
        if (currAclIndex != 0) /* did not finish receiving an ACL */
        {
          aclRxError = L7_TRUE;
        }
        break;
      case ACL_CLUSTER_MSG_IE_ACL_DEF_START:
        CLUSTER_PKT_ELEMENT_SIZE_CHECK(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                       aclClusterDebug, element, 
                                       ACL_CLUSTER_MSG_IE_ACL_DEF_START_SIZE, 
                                       offset);
        if (currAclIndex == 0)
        {
          CLUSTER_PKT_INT32_GET(currAclIndex, buffer, offset);
        }
        else
        {
          aclRxError = L7_TRUE;
        }
        break;
      case ACL_CLUSTER_MSG_IE_ACL_DEF_END:
        {
          L7_uint32 aclIndex;
          CLUSTER_PKT_ELEMENT_SIZE_CHECK(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                         aclClusterDebug, element, 
                                         ACL_CLUSTER_MSG_IE_ACL_DEF_END_SIZE, 
                                         offset);
          CLUSTER_PKT_INT32_GET(aclIndex, buffer, offset);
          if (aclIndex == currAclIndex)
          {
            /* done receiving current ACL */
            currAclIndex = 0;
            pAcl = L7_NULLPTR;
            pMacAcl = L7_NULLPTR;
          } 
          else
          {
            LOG_MSG("%s: invalid ACL configuration, received wrong index in ACL_CLUSTER_MSG_IE_ACL_DEF_END IE.\n", 
                    __FUNCTION__);
            aclRxError = L7_TRUE;
          }
        }
        break;
      case ACL_CLUSTER_MSG_IE_ACL_ID:
        {
          L7_ACL_TYPE_t aclType;
          L7_uchar8     aclName[L7_ACL_NAME_LEN_MAX+1];
          CLUSTER_PKT_ELEMENT_SIZE_CHECK(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                         aclClusterDebug, element, 
                                         ACL_CLUSTER_MSG_IE_ACL_ID_SIZE, 
                                         offset);
          CLUSTER_PKT_INT8_GET(aclType, buffer, offset);
          CLUSTER_PKT_STRING_GET(aclName, L7_ACL_NAME_LEN_MAX, buffer, offset);
          aclName[L7_ACL_NAME_LEN_MAX] = '\0';
          aclRxError = L7_TRUE;
          if (aclImpTypeFromIndexGet(currAclIndex) == aclType)
          {
            switch (aclType)
            {
              case L7_ACL_TYPE_MAC:
                if (aclMacRxCount < L7_ACL_MAX_LISTS)
                {
                  aclMacRxCount++;
                  pMacAcl = &pAclMacRxList[aclMacRxCount-1];
                  pMacAcl->aclIndex = currAclIndex;
                  memcpy(pMacAcl->aclName, aclName, sizeof(pMacAcl->aclName));
                  aclRxError = L7_FALSE;
                }
                break;
              case L7_ACL_TYPE_IP:
              case L7_ACL_TYPE_IPV6:
                if (aclRxCount < L7_ACL_MAX_LISTS)
                {
                  aclRxCount++;
                  pAcl = &pAclRxList[aclRxCount-1];
                  pAcl->aclNum = currAclIndex;
                  memcpy(pAcl->aclName, aclName, sizeof(pAcl->aclName));
                  aclRxError = L7_FALSE;
                }
                break;
              default:
                break;
            }
          }
        }
        break;
      case ACL_CLUSTER_MSG_IE_RULE_GROUP_START:
        CLUSTER_PKT_ELEMENT_SIZE_CHECK(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                       aclClusterDebug, element, 
                                       ACL_CLUSTER_MSG_IE_RULE_GROUP_START_SIZE, 
                                       offset);
        CLUSTER_PKT_INT16_GET(ruleCount, buffer, offset);
        aclRxRuleCount += ruleCount;
        if ((ruleCount > L7_ACL_MAX_RULES_PER_LIST) ||  /* check max rules per list */
            (aclRxRuleCount > L7_ACL_MAX_RULES) ||      /* check max total rules */
            (currAclIndex == 0) || /* no current ACL */
            ((pMacAcl != L7_NULLPTR) && (pMacAcl->head != L7_NULLPTR)) ||  /* already received rule(s) */
            ((pAcl != L7_NULLPTR) && (pAcl->head != L7_NULLPTR)))
        {
          LOG_MSG("%s: invalid configuration for ACL index=%u, max rules per list is %u.\n", 
                  __FUNCTION__, currAclIndex, L7_ACL_MAX_RULES_PER_LIST);
          aclRxError = L7_TRUE;
        }
        break;
      case ACL_CLUSTER_MSG_IE_RULE_GROUP_END:
        CLUSTER_PKT_ELEMENT_SIZE_CHECK(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                       aclClusterDebug, element, 
                                       ACL_CLUSTER_MSG_IE_RULE_GROUP_END_SIZE, 
                                       offset);
        CLUSTER_PKT_INT16_GET(ruleCount, buffer, offset);
        /* check that we received the expected number of rules */
        if ((currAclIndex == 0) ||
            ((pMacAcl != L7_NULLPTR) && (pMacAcl->ruleCount != ruleCount)) ||
            ((pAcl != L7_NULLPTR) && (pAcl->ruleCount != ruleCount)))
        {
          LOG_MSG("%s: invalid ACL configuration, invalid rule count for ACL index=%u, received:%u, expected:%u.\n", 
                  __FUNCTION__, currAclIndex, 
                  (pMacAcl != L7_NULLPTR) ? pMacAcl->ruleCount : ((pAcl != L7_NULLPTR) ? pAcl->ruleCount : 0),
                  ruleCount);
          aclRxError = L7_TRUE;
        }
        ruleCount = 0;  /* do not receive any more rules for this ACL */
        break;
      case ACL_CLUSTER_MSG_IE_RULE_DEF_IP:
        CLUSTER_PKT_ELEMENT_SIZE_CHECK(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                       aclClusterDebug, element, 
                                       ACL_CLUSTER_MSG_IE_RULE_DEF_IP_SIZE, 
                                       offset);
        aclRxError = L7_TRUE;
        if ((pAcl != L7_NULLPTR) && (pAcl->ruleCount <= ruleCount))
        {
          aclRuleParms_t *pRule = aclClusterIpAclRuleNodeAlloc(pAcl);
          if (pRule != L7_NULLPTR)
          {
            aclClusterConfigIpAclRuleParse(aclImpTypeFromIndexGet(pAcl->aclNum), buffer, &offset, pRule);
            if (pRule->ruleNum == ACL_CLUSTER_DEFAULT_RULE_NUM)
            {
              pRule->ruleNum = L7_ACL_DEFAULT_RULE_NUM;
            }
            else
            {
              pAcl->ruleCount++;
            }
            aclRxError = L7_FALSE;
          }
        }
        break;
      case ACL_CLUSTER_MSG_IE_RULE_DEF_MAC:
        CLUSTER_PKT_ELEMENT_SIZE_CHECK(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                       aclClusterDebug, element, 
                                       ACL_CLUSTER_MSG_IE_RULE_DEF_MAC_SIZE, 
                                       offset);
        aclRxError = L7_TRUE;
        if ((pMacAcl != L7_NULLPTR) && (pMacAcl->ruleCount <= ruleCount))
        {
          aclMacRuleParms_t *pMacRule = aclClusterMacAclRuleNodeAlloc(pMacAcl);
          if (pMacRule != L7_NULLPTR)
          {
            aclClusterConfigMacAclRuleParse(buffer, &offset, pMacRule);
            if (pMacRule->ruleNum == ACL_CLUSTER_DEFAULT_RULE_NUM)
            {
              pMacRule->ruleNum = L7_ACL_DEFAULT_RULE_NUM;
            }
            else
            {
              pMacAcl->ruleCount++;
            }
            aclRxError = L7_FALSE;
          }
        }
        break;
      default:
        ACL_CLUSTER_DLOG("%s: received unknown IE 0x%x.\n", __FUNCTION__, element.elementId);
        offset += element.elementLen;
        break;
    }
    if (aclRxError == L7_TRUE)
    {
      /* quit processing this and further messages */
      LOG_MSG("%s: invalid ACL configuration, error receiving IE:%u.\n", 
              __FUNCTION__, element.elementId);
      break;
    }
  } /* endwhile */

  ACL_CLUSTER_DLOG("LEAVING %s: rxError=%u\n", __FUNCTION__, aclRxError); 

  return;
}

/*********************************************************************
* @purpose  Function to write MAC Layer 2 rules to config message
*
* @param    aclMacRuleParms_t *pRule  @b{(input)} pointer to rule data
* @param    L7_uchar8         *buffer @b{(input/output)} message buffer
* @param    L7_uint32         *offset @b{(input/output)} current offset in buffer
*
* @returns  void
*
* @notes  
*
* @end
*********************************************************************/
void aclClusterConfigMacAclRuleWrite(aclMacRuleParms_t *pRule,
                                     L7_uchar8         *buffer,
                                     L7_uint32         *offset)
{
  L7_uint32    ruleNum;

  /* Bytes 00-03 Config Mask */
  CLUSTER_PKT_INT32_SET(pRule->configMask, buffer, *offset);
  /* Bytes 04-07 Rule index */
  ruleNum = pRule->ruleNum;
  if (L7_ACL_DEFAULT_RULE_NUM == ruleNum)
  {
    ruleNum = ACL_CLUSTER_DEFAULT_RULE_NUM;
  }
  CLUSTER_PKT_INT32_SET(ruleNum, buffer, *offset);
  /* Bytes 08-11 Rule action */
  CLUSTER_PKT_INT32_SET(pRule->action, buffer, *offset);
  /* Bytes 12-15 Assign queue Id */
  CLUSTER_PKT_INT32_SET(pRule->assignQueueId, buffer, *offset);

  /* Bytes 16-23 Redirect config Id*/
  CLUSTER_PKT_INT32_SET(pRule->redirectConfigId.type, buffer, *offset);
  CLUSTER_PKT_INT8_SET(pRule->redirectConfigId.configSpecifier.usp.unit, buffer, *offset);
  CLUSTER_PKT_INT8_SET(pRule->redirectConfigId.configSpecifier.usp.slot, buffer, *offset);
  CLUSTER_PKT_INT16_SET(pRule->redirectConfigId.configSpecifier.usp.port, buffer, *offset);

  /* Bytes 24-31 Mirror config Id*/
  CLUSTER_PKT_INT32_SET(pRule->mirrorConfigId.type, buffer, *offset);
  CLUSTER_PKT_INT8_SET(pRule->mirrorConfigId.configSpecifier.usp.unit, buffer, *offset);
  CLUSTER_PKT_INT8_SET(pRule->mirrorConfigId.configSpecifier.usp.slot, buffer, *offset);
  CLUSTER_PKT_INT16_SET(pRule->mirrorConfigId.configSpecifier.usp.port, buffer, *offset);

  /* Byte 32 - Logging flag */
  CLUSTER_PKT_INT8_SET(pRule->logging, buffer, *offset);
  /* Byte 33 - Match every flag */
  CLUSTER_PKT_INT8_SET(pRule->every, buffer, *offset);

  /* Byte 34 - CoS priority */
  CLUSTER_PKT_INT8_SET(pRule->cos, buffer, *offset);
  /* Byte 35 - secondary CoS priority */
  CLUSTER_PKT_INT8_SET(pRule->cos2, buffer, *offset);

  /* Bytes 36-43 Dst MAC address */
  CLUSTER_PKT_MAC_ADDR_SET(pRule->dstMac, buffer, *offset);
  /* Bytes 44-51 Dst MAC mask */
  CLUSTER_PKT_MAC_ADDR_SET(pRule->dstMacMask, buffer, *offset);

  /* Bytes 52-53 Ethertype key Id */
  CLUSTER_PKT_INT16_SET(pRule->etypeKeyId, buffer, *offset);
  /* Bytes 54-55 Ethertype custom value */
  CLUSTER_PKT_INT16_SET(pRule->etypeValue, buffer, *offset);

  /* Bytes 56-61 Src MAC address */
  CLUSTER_PKT_MAC_ADDR_SET(pRule->srcMac, buffer, *offset);
  /* Bytes 62-67 Src MAC mask */
  CLUSTER_PKT_MAC_ADDR_SET(pRule->srcMacMask, buffer, *offset);

  /* Bytes 68-69 VLAN Id (single) */
  CLUSTER_PKT_INT16_SET(pRule->vlanId, buffer, *offset);
  /* Bytes 70-71 VLAN Id range start */
  CLUSTER_PKT_INT16_SET(pRule->vlanIdStart, buffer, *offset);
  /* Bytes 72-73 VLAN Id range end */
  CLUSTER_PKT_INT16_SET(pRule->vlanIdEnd, buffer, *offset);

  /* Bytes 74-75 Secondary VLAN Id (single) */
  CLUSTER_PKT_INT16_SET(pRule->vlanId2, buffer, *offset);
  /* Bytes 76-77 Secondary VLAN Id range start */
  CLUSTER_PKT_INT16_SET(pRule->vlanId2Start, buffer, *offset);
  /* Bytes 78-79 Secondary VLAN Id range end */
  CLUSTER_PKT_INT16_SET(pRule->vlanId2End, buffer, *offset);

  return;
}

/*********************************************************************
* @purpose  Function to write IPv4/IPv6 rules to config message
*
* @param    L7_ACL_TYPE_t    type   @b{(input)} IPv4 or IPv6
* @param    aclRuleParms_t  *pRule  @b{(input)} pointer to rule data
* @param    L7_uchar8       *buffer @b{(input/output)} message buffer
* @param    L7_uint32       *offset @b{(input/output)} current offset in buffer
*
* @returns  void
*
* @notes  
*
* @end
*********************************************************************/
void aclClusterConfigIpAclRuleWrite(L7_ACL_TYPE_t    type,
                                    aclRuleParms_t  *pRule,
                                    L7_uchar8       *buffer,
                                    L7_uint32       *offset)
{
  L7_uint32    ruleNum;

  /* Bytes 00-03 Config Mask */
  CLUSTER_PKT_INT32_SET(pRule->configMask, buffer, *offset);
  /* Bytes 04-07 Rule index */
  ruleNum = pRule->ruleNum;
  if (L7_ACL_DEFAULT_RULE_NUM == ruleNum)
  {
    ruleNum = ACL_CLUSTER_DEFAULT_RULE_NUM;
  }
  CLUSTER_PKT_INT32_SET(ruleNum, buffer, *offset);
  /* Bytes 08-11 Rule action */
  CLUSTER_PKT_INT32_SET(pRule->action, buffer, *offset);
  /* Bytes 12-15 Assign queue Id */
  CLUSTER_PKT_INT32_SET(pRule->assignQueueId, buffer, *offset);

  /* Bytes 16-23 Redirect config Id*/
  CLUSTER_PKT_INT32_SET(pRule->redirectConfigId.type, buffer, *offset);
  CLUSTER_PKT_INT8_SET(pRule->redirectConfigId.configSpecifier.usp.unit, buffer, *offset);
  CLUSTER_PKT_INT8_SET(pRule->redirectConfigId.configSpecifier.usp.slot, buffer, *offset);
  CLUSTER_PKT_INT16_SET(pRule->redirectConfigId.configSpecifier.usp.port, buffer, *offset);

  /* Bytes 24-31 Mirror config Id*/
  CLUSTER_PKT_INT32_SET(pRule->mirrorConfigId.type, buffer, *offset);
  CLUSTER_PKT_INT8_SET(pRule->mirrorConfigId.configSpecifier.usp.unit, buffer, *offset);
  CLUSTER_PKT_INT8_SET(pRule->mirrorConfigId.configSpecifier.usp.slot, buffer, *offset);
  CLUSTER_PKT_INT16_SET(pRule->mirrorConfigId.configSpecifier.usp.port, buffer, *offset);

  /* Byte 32 - Logging flag */
  CLUSTER_PKT_INT8_SET(pRule->logging, buffer, *offset);
  /* Byte 33 - Match every flag */
  CLUSTER_PKT_INT8_SET(pRule->every, buffer, *offset);
  /* Byte 34 - Protocol number */
  CLUSTER_PKT_INT8_SET(pRule->protocol, buffer, *offset);
  /* Byte 35 - Protocol mask */
  CLUSTER_PKT_INT8_SET(pRule->protmask, buffer, *offset);

  /* Bytes 36-55 Src Ipv4/Ipv6 */
  if (type == L7_ACL_TYPE_IP)
  {
    CLUSTER_PKT_INT32_SET(pRule->srcIp.v4.addr, buffer, *offset);
    CLUSTER_PKT_INT32_SET(pRule->srcIp.v4.mask, buffer, *offset);
    (*offset) += (3 * sizeof(L7_uint32)); /* reserved */
  } else
  {
    CLUSTER_PKT_STRING_SET(&pRule->srcIp.v6.in6Addr, sizeof(L7_in6_addr_t), 
                           buffer, *offset);
    CLUSTER_PKT_INT32_SET(pRule->srcIp.v6.in6PrefixLen, buffer, *offset);
  }

  /* Bytes 56-57 Src L4 port number */
  CLUSTER_PKT_INT16_SET(pRule->srcPort, buffer, *offset);
  /* Bytes 58-59 Src L4 port start */
  CLUSTER_PKT_INT16_SET(pRule->srcStartPort, buffer, *offset);
  /* Bytes 60-61 Src L4 port end */
  CLUSTER_PKT_INT16_SET(pRule->srcEndPort, buffer, *offset);
  /* Bytes 62-63 reserved */
  (*offset) += sizeof(L7_ushort16);

  /* Bytes 64-83 Dst Ipv4/Ipv6 */
  if (type == L7_ACL_TYPE_IP)
  {
    CLUSTER_PKT_INT32_SET(pRule->dstIp.v4.addr, buffer, *offset);
    CLUSTER_PKT_INT32_SET(pRule->dstIp.v4.mask, buffer, *offset);
    (*offset) += (3 * sizeof(L7_uint32)); /* reserved */
  } else
  {
    CLUSTER_PKT_STRING_SET(&pRule->dstIp.v6.in6Addr, sizeof(L7_in6_addr_t), 
                           buffer, *offset);
    CLUSTER_PKT_INT32_SET(pRule->dstIp.v6.in6PrefixLen, buffer, *offset);
  }

  /* Bytes 84-85 Dst L4 port number */
  CLUSTER_PKT_INT16_SET(pRule->dstPort, buffer, *offset);
  /* Bytes 86-87 Dst L4 port start */
  CLUSTER_PKT_INT16_SET(pRule->dstStartPort, buffer, *offset);
  /* Bytes 88-89 Dst L4 port end */
  CLUSTER_PKT_INT16_SET(pRule->dstEndPort, buffer, *offset);

  /* Byte 90 ToS value */
  CLUSTER_PKT_INT8_SET(pRule->tosbyte, buffer, *offset);
  /* Byte 91 ToS mask */
  CLUSTER_PKT_INT8_SET(pRule->tosmask, buffer, *offset);

  /* Bytes 92-95 Ipv6 flow label */
  CLUSTER_PKT_INT32_SET(pRule->flowlbl, buffer, *offset);

  return;
}

/*********************************************************************
* @purpose  Function to send IPv4/IPv6 ACLs to cluster member
*
* @param    clusterMemberID_t *memberID  @b{(input)} cluster member
* @param    L7_uchar8         *buffer @b{(input/output)} message buffer
* @param    L7_uint32         *offset @b{(input/output)} current offset in buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t aclClusterConfigIpAclsSend(clusterMemberID_t *memberID,
                                   L7_uchar8         *buffer,
                                   L7_uint32         *offset)
{
  aclStructure_t  *pAcl = L7_NULLPTR;
  aclRuleParms_t  *pRule = L7_NULLPTR;
  L7_uint32        aclNum = 0;
  L7_ACL_TYPE_t    aclType;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  while ((pAcl = avlSearchLVL7(pAclTree, &aclNum, AVL_NEXT)) != L7_NULLPTR)
  {
    if (aclClusterConfigMsgSendCheck((3 * sizeof(clusterMsgElementHdr_t)) +
                                     ACL_CLUSTER_MSG_IE_ACL_DEF_START_SIZE +
                                     ACL_CLUSTER_MSG_IE_ACL_ID_SIZE +
                                     ACL_CLUSTER_MSG_IE_RULE_GROUP_START_SIZE,
                                     memberID, buffer, offset) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    CLUSTER_PKT_ELEMENT_HDR_SET(ACL_CLUSTER_MSG_IE_ACL_DEF_START, 
                                ACL_CLUSTER_MSG_IE_ACL_DEF_START_SIZE, 
                                buffer, *offset);
    CLUSTER_PKT_INT32_SET(pAcl->aclNum, buffer, *offset);

    /* ACL Identifier = aclType & aclName */
    CLUSTER_PKT_ELEMENT_HDR_SET(ACL_CLUSTER_MSG_IE_ACL_ID, 
                                ACL_CLUSTER_MSG_IE_ACL_ID_SIZE,
                                buffer, *offset);
    aclType = aclImpTypeFromIndexGet(pAcl->aclNum);
    CLUSTER_PKT_INT8_SET(aclType, buffer, *offset);
    CLUSTER_PKT_STRING_SET(pAcl->aclName, L7_ACL_NAME_LEN_MAX, buffer, *offset);

    CLUSTER_PKT_ELEMENT_HDR_SET(ACL_CLUSTER_MSG_IE_RULE_GROUP_START,
                                ACL_CLUSTER_MSG_IE_RULE_GROUP_START_SIZE, 
                                buffer, *offset);
    CLUSTER_PKT_INT16_SET(pAcl->ruleCount, buffer, *offset);

    pRule = pAcl->head;
    while (pRule != L7_NULLPTR)
    {
      if (aclClusterConfigMsgSendCheck(sizeof(clusterMsgElementHdr_t) +
                                       ACL_CLUSTER_MSG_IE_RULE_DEF_IP_SIZE,
                                       memberID, buffer, offset) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      CLUSTER_PKT_ELEMENT_HDR_SET(ACL_CLUSTER_MSG_IE_RULE_DEF_IP,
                                  ACL_CLUSTER_MSG_IE_RULE_DEF_IP_SIZE,
                                  buffer, *offset);
      aclClusterConfigIpAclRuleWrite(aclType, pRule, buffer, offset);
      pRule = pRule->next;
    }

    if (aclClusterConfigMsgSendCheck((2 * sizeof(clusterMsgElementHdr_t)) +
                                     ACL_CLUSTER_MSG_IE_RULE_GROUP_END_SIZE +
                                     ACL_CLUSTER_MSG_IE_ACL_DEF_END_SIZE,
                                     memberID, buffer, offset) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    CLUSTER_PKT_ELEMENT_HDR_SET(ACL_CLUSTER_MSG_IE_RULE_GROUP_END,
                                ACL_CLUSTER_MSG_IE_RULE_GROUP_END_SIZE, 
                                buffer, *offset);
    CLUSTER_PKT_INT16_SET(pAcl->ruleCount, buffer, *offset);

    CLUSTER_PKT_ELEMENT_HDR_SET(ACL_CLUSTER_MSG_IE_ACL_DEF_END, 
                                ACL_CLUSTER_MSG_IE_ACL_DEF_END_SIZE, 
                                buffer, *offset);
    CLUSTER_PKT_INT32_SET(pAcl->aclNum, buffer, *offset);

    /* update search key */
    aclNum = pAcl->aclNum;
  }
  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Function to send MAC ACLs to cluster member
*
* @param    clusterMemberID_t *memberID  @b{(input)} cluster member
* @param    L7_uchar8         *buffer @b{(input/output)} message buffer
* @param    L7_uint32         *offset @b{(input/output)} current offset in buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t aclClusterConfigMacAclsSend(clusterMemberID_t *memberID,
                                    L7_uchar8         *buffer,
                                    L7_uint32         *offset)
{
  aclMacStructure_t  *pAcl = L7_NULLPTR;
  aclMacRuleParms_t  *pRule = L7_NULLPTR;
  L7_uint32           aclIndex = 0;
  
  /* TBD - check with Greg/Jeff R, in some functions only tree semaphore is used */
  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  while ((pAcl = avlSearchLVL7(pAclMacTree, &aclIndex, AVL_NEXT)) != L7_NULLPTR)
  {
    if (aclClusterConfigMsgSendCheck((3 * sizeof(clusterMsgElementHdr_t)) +
                                     ACL_CLUSTER_MSG_IE_ACL_DEF_START_SIZE +
                                     ACL_CLUSTER_MSG_IE_ACL_ID_SIZE +
                                     ACL_CLUSTER_MSG_IE_RULE_GROUP_START_SIZE,
                                     memberID, buffer, offset) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    CLUSTER_PKT_ELEMENT_HDR_SET(ACL_CLUSTER_MSG_IE_ACL_DEF_START, 
                                ACL_CLUSTER_MSG_IE_ACL_DEF_START_SIZE, 
                                buffer, *offset);
    CLUSTER_PKT_INT32_SET(pAcl->aclIndex, buffer, *offset);

    /* ACL Identifier = aclType & aclName */
    CLUSTER_PKT_ELEMENT_HDR_SET(ACL_CLUSTER_MSG_IE_ACL_ID, 
                                ACL_CLUSTER_MSG_IE_ACL_ID_SIZE,
                                buffer, *offset);
    CLUSTER_PKT_INT8_SET(aclImpTypeFromIndexGet(pAcl->aclIndex), buffer, *offset);
    CLUSTER_PKT_STRING_SET(pAcl->aclName, L7_ACL_NAME_LEN_MAX, buffer, *offset);

    CLUSTER_PKT_ELEMENT_HDR_SET(ACL_CLUSTER_MSG_IE_RULE_GROUP_START,
                                ACL_CLUSTER_MSG_IE_RULE_GROUP_START_SIZE, 
                                buffer, *offset);
    CLUSTER_PKT_INT16_SET(pAcl->ruleCount, buffer, *offset);

    pRule = pAcl->head;
    while (pRule != L7_NULLPTR)
    {
      if (aclClusterConfigMsgSendCheck(sizeof(clusterMsgElementHdr_t) +
                                       ACL_CLUSTER_MSG_IE_RULE_DEF_MAC_SIZE,
                                       memberID, buffer, offset) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      CLUSTER_PKT_ELEMENT_HDR_SET(ACL_CLUSTER_MSG_IE_RULE_DEF_MAC,
                                  ACL_CLUSTER_MSG_IE_RULE_DEF_MAC_SIZE,
                                  buffer, *offset);
      aclClusterConfigMacAclRuleWrite(pRule, buffer, offset);
      pRule = pRule->next;
    }

    if (aclClusterConfigMsgSendCheck((2 * sizeof(clusterMsgElementHdr_t)) +
                                     ACL_CLUSTER_MSG_IE_RULE_GROUP_END_SIZE +
                                     ACL_CLUSTER_MSG_IE_ACL_DEF_END_SIZE,
                                     memberID, buffer, offset) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    CLUSTER_PKT_ELEMENT_HDR_SET(ACL_CLUSTER_MSG_IE_RULE_GROUP_END,
                                ACL_CLUSTER_MSG_IE_RULE_GROUP_END_SIZE, 
                                buffer, *offset);
    CLUSTER_PKT_INT16_SET(pAcl->ruleCount, buffer, *offset);

    CLUSTER_PKT_ELEMENT_HDR_SET(ACL_CLUSTER_MSG_IE_ACL_DEF_END, 
                                ACL_CLUSTER_MSG_IE_ACL_DEF_END_SIZE, 
                                buffer, *offset);
    CLUSTER_PKT_INT32_SET(pAcl->aclIndex, buffer, *offset);

    /* update search key */
    aclIndex = pAcl->aclIndex;
  }
  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

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
L7_RC_t aclClusterConfigSend(clusterMemberID_t *memberID)
{
  L7_uchar8 buffer[ACL_CLUSTER_MSG_SIZE_MAX];
  L7_uint32 offset = 0;

  if (l7utilsMacAddrHexToString(memberID->addr, 
                                sizeof(buffer), buffer) != L7_SUCCESS)
  {
    LOG_MSG("%s: received invalid memberID.\n", __FUNCTION__);
    return L7_FAILURE;
  }

  ACL_CLUSTER_DLOG("%s: member ID=%s\n", __FUNCTION__, buffer); 

  memset(buffer, 0, sizeof(buffer));
  offset = 0;

  CLUSTER_PKT_ELEMENT_HDR_SET(ACL_CLUSTER_MSG_IE_CONFIG_START, 
                              ACL_CLUSTER_MSG_IE_CONFIG_START_SIZE,
                              buffer, offset);

  if ((aclClusterConfigIpAclsSend(memberID, buffer, &offset) != L7_SUCCESS) ||
      (aclClusterConfigMacAclsSend(memberID, buffer, &offset) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }

  if (aclClusterConfigMsgSendCheck(sizeof(clusterMsgElementHdr_t) + 
                                   ACL_CLUSTER_MSG_IE_CONFIG_END_SIZE,
                                   memberID, buffer, &offset) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  CLUSTER_PKT_ELEMENT_HDR_SET(ACL_CLUSTER_MSG_IE_CONFIG_END, 
                              ACL_CLUSTER_MSG_IE_CONFIG_END_SIZE,
                              buffer, offset);

  return clusterMsgSend(memberID,
                        CLUSTER_MSG_DELIVERY_RELIABLE,
                        ACL_CLUSTER_MSG_CONFIG,
                        offset, buffer);
}

/*********************************************************************
* @purpose  Verify received IPv4/IPv6 ACL configuration against features
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  This assumes data has already been validated by the sender,
*         so we can skip basic checks, such as range checking, but
*         this system not have the same prod/plat defines or features 
*         enabled, need to fail if we cannot apply entire configuration.
*         The max ACL & rule limits are validated during receive.
*
* @end
*********************************************************************/
L7_RC_t aclClusterConfigIpAclsVerify(void)
{
  L7_uint32 i = 0;

  for (i=0; i < aclRxCount; i++)
  {
    aclStructure_t *pAcl = &pAclRxList[i];
    aclRuleParms_t *pRule = pAcl->head;
    L7_ACL_TYPE_t   aclType = aclImpTypeFromIndexGet(pAcl->aclNum);

    if (aclType == L7_ACL_TYPE_IPV6)
    {
      ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_TYPE_IPV6_FEATURE_ID);
    }

    while (pRule != L7_NULLPTR)
    {
      L7_uint32 intIfNum;
      if (((pRule->configMask) & (1 << ACL_ASSIGN_QUEUEID)) != 0)
      {
        ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_ASSIGN_QUEUE_FEATURE_ID);
        /* check assign queue ID within supported range */
        if ((pRule->assignQueueId < L7_ACL_ASSIGN_QUEUE_ID_MIN) ||
            (pRule->assignQueueId > L7_ACL_ASSIGN_QUEUE_ID_MAX))
        {
          ACL_CLUSTER_DLOG("%s: assign queue ID:%u out of range, index=%u, rule=%u.\n",
                           __FUNCTION__, pRule->assignQueueId, pAcl->aclNum, pRule->ruleNum);
          return L7_FAILURE;
        }
      }
      if (((pRule->configMask) & (1 << ACL_REDIRECT_INTF)) != 0)
      {
        ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_REDIRECT_FEATURE_ID);
        if ((nimIntIfFromConfigIDGet(&pRule->redirectConfigId, &intIfNum) != L7_SUCCESS) ||
            (aclIsValidIntf(intIfNum) != L7_TRUE))
        {
          ACL_CLUSTER_DLOG("%s: redirect interface not valid, index=%u, rule=%u.\n", 
                           __FUNCTION__, pAcl->aclNum, pRule->ruleNum);
          return L7_FAILURE;
        }
      }
      if (((pRule->configMask) & (1 << ACL_MIRROR_INTF)) != 0)
      {
        ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_MIRROR_FEATURE_ID);
        if ((nimIntIfFromConfigIDGet(&pRule->mirrorConfigId, &intIfNum) != L7_SUCCESS) ||
            (aclIsValidIntf(intIfNum) != L7_TRUE))
        {
          ACL_CLUSTER_DLOG("%s: mirror interface not valid, index=%u, rule=%u.\n", 
                           __FUNCTION__, pAcl->aclNum, pRule->ruleNum);
          return L7_FAILURE;
        }
      }
      if (((pRule->configMask) & (1 << ACL_LOGGING)) != 0)
      {
        if (aclImpLoggingIsAllowed(pRule->action) != L7_TRUE)  /* action-specific feature check */
        {
          return L7_FAILURE;
        }
      }
      if (((pRule->configMask) & (1 << ACL_EVERY)) != 0)
      {
        if (pRule->every == L7_TRUE)
        {
          ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_EVERY_FEATURE_ID);
        }
      }
      else
      {
        if (((pRule->configMask) & (1 << ACL_DSTIP)) != 0)
        {
          if (aclType == L7_ACL_TYPE_IP)
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID);
            if (pRule->dstIp.v4.mask != (L7_uint32)L7_IP_SUBMASK_EXACT_MATCH_MASK)
            {
              ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID);
            }
          }
        }
        if (((pRule->configMask) & (1 << ACL_DSTIPV6)) != 0)
        {
          if (aclType == L7_ACL_TYPE_IPV6)
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID);
          }
        }
        if (((pRule->configMask) & (1 << ACL_DSTPORT)) != 0)
        {
          if (aclType == L7_ACL_TYPE_IPV6)
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_FEATURE_ID);
          }
          else
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_DSTL4PORT_FEATURE_ID);
          }
        }
        if (((pRule->configMask) & (1 << ACL_DSTSTARTPORT)) != 0)
        {
          if (aclType == L7_ACL_TYPE_IPV6)
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_RANGE_FEATURE_ID);
          }
          else
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_DSTL4PORT_RANGE_FEATURE_ID);
          }
        }
        if (((pRule->configMask) & (1 << ACL_FLOWLBLV6)) != 0)
        {
          ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_IPV6_FLOWLBL_FEATURE_ID);
        }
        if (((pRule->configMask) & (1 << ACL_IPDSCP)) != 0)
        {
          if (aclType == L7_ACL_TYPE_IPV6)
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_IPV6_IPDSCP_FEATURE_ID);
          }
          else
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_IPDSCP_FEATURE_ID);
          }
        }
        if (((pRule->configMask) & (1 << ACL_IPPREC)) != 0)
        {
          if (aclType == L7_ACL_TYPE_IP)
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_IPPRECEDENCE_FEATURE_ID);
          }
        }
        if (((pRule->configMask) & (1 << ACL_IPTOS)) != 0)
        {
          if (aclType == L7_ACL_TYPE_IP)
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_IPTOS_FEATURE_ID);
          }
        }
        if (((pRule->configMask) & (1 << ACL_PROTOCOL)) != 0)
        {
          if (aclType == L7_ACL_TYPE_IPV6)
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_IPV6_PROTOCOL_FEATURE_ID);
          }
          else
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_PROTOCOL_FEATURE_ID);
          }
        }
        if (((pRule->configMask) & (1 << ACL_SRCIP)) != 0)
        {
          if (aclType == L7_ACL_TYPE_IP)
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID);
            if (pRule->srcIp.v4.mask != (L7_uint32)L7_IP_SUBMASK_EXACT_MATCH_MASK)
            {
              ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID);
            }
          }
        }
        if (((pRule->configMask) & (1 << ACL_SRCIPV6)) != 0)
        {
          if (aclType == L7_ACL_TYPE_IPV6)
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_IPV6_SRCIP_FEATURE_ID);
          }
        }
        if (((pRule->configMask) & (1 << ACL_SRCPORT)) != 0)
        {
          if (aclType == L7_ACL_TYPE_IPV6)
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_FEATURE_ID);
          }
          else
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_SRCL4PORT_FEATURE_ID);
          }
        }
        if (((pRule->configMask) & (1 << ACL_SRCSTARTPORT)) != 0)
        {
          if (aclType == L7_ACL_TYPE_IPV6)
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_RANGE_FEATURE_ID);
          }
          else
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_SRCL4PORT_RANGE_FEATURE_ID);
          }
        }
      }
      pRule = pRule->next;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verify received MAC ACL configuration against features
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  This assumes data has already been validated by the sender,
*         so we can skip basic checks, such as range checking, but
*         this system not have the same prod/plat defines or features 
*         enabled, need to fail if we cannot apply entire configuration.
*         The max ACL & rule limits are validated during receive.
*
* @end
*********************************************************************/
L7_RC_t aclClusterConfigMacAclsVerify(void)
{
  L7_uint32 i = 0;
  L7_uchar8 hostMacMask[L7_MAC_ADDR_LEN] = {0xff,0xff,0xff,0xff,0xff,0xff};

  for (i=0; i < aclMacRxCount; i++)
  {
    aclMacStructure_t *pAcl = &pAclMacRxList[i];
    aclMacRuleParms_t *pRule = pAcl->head;

    ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_TYPE_MAC_FEATURE_ID);

    while (pRule != L7_NULLPTR)
    {
      L7_uint32 intIfNum;
      if (((pRule->configMask) & (1 << ACL_MAC_ASSIGN_QUEUEID)) != 0)
      {
        ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_ASSIGN_QUEUE_FEATURE_ID);
        /* check assign queue ID within supported range */
        if ((pRule->assignQueueId < L7_ACL_ASSIGN_QUEUE_ID_MIN) ||
            (pRule->assignQueueId > L7_ACL_ASSIGN_QUEUE_ID_MAX))
        {
          ACL_CLUSTER_DLOG("%s: assign queue ID:%u out of range, index=%u, rule=%u.\n",
                           __FUNCTION__, pRule->assignQueueId, pAcl->aclIndex, pRule->ruleNum);
          return L7_FAILURE;
        }
      }
      if (((pRule->configMask) & (1 << ACL_MAC_REDIRECT_INTF)) != 0)
      {
        ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_REDIRECT_FEATURE_ID);
        if ((nimIntIfFromConfigIDGet(&pRule->redirectConfigId, &intIfNum) != L7_SUCCESS) ||
            (aclIsValidIntf(intIfNum) != L7_TRUE))
        {
          ACL_CLUSTER_DLOG("%s: redirect interface not valid, index=%u, rule=%u.\n", 
                           __FUNCTION__, pAcl->aclIndex, pRule->ruleNum);
          return L7_FAILURE;
        }
      }
      if (((pRule->configMask) & (1 << ACL_MAC_MIRROR_INTF)) != 0)
      {
        ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_MIRROR_FEATURE_ID);
        if ((nimIntIfFromConfigIDGet(&pRule->mirrorConfigId, &intIfNum) != L7_SUCCESS) ||
            (aclIsValidIntf(intIfNum) != L7_TRUE))
        {
          ACL_CLUSTER_DLOG("%s: mirror interface not valid, index=%u, rule=%u.\n", 
                           __FUNCTION__, pAcl->aclIndex, pRule->ruleNum);
          return L7_FAILURE;
        }
      }
      if (((pRule->configMask) & (1 << ACL_MAC_LOGGING)) != 0)
      {
        if (aclImpLoggingIsAllowed(pRule->action) != L7_TRUE)  /* action-specific feature check */
        {
          return L7_FAILURE;
        }
      }
      if (((pRule->configMask) & (1 << ACL_MAC_EVERY)) != 0)
      {
        if (pRule->every == L7_TRUE)
        {
          ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_EVERY_FEATURE_ID);
        }
      }
      else
      {
        if (((pRule->configMask) & (1 << ACL_MAC_COS)) != 0)
        {
          ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_COS_FEATURE_ID);
        }
        if (((pRule->configMask) & (1 << ACL_MAC_COS2)) != 0)
        {
          ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_COS2_FEATURE_ID);
        }
        if (((pRule->configMask) & (1 << ACL_MAC_DSTMAC)) != 0)
        {
          ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_DSTMAC_FEATURE_ID);
          if (memcmp(pRule->dstMacMask, hostMacMask, sizeof(hostMacMask)) != 0)
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID);
          }
        }
        if (((pRule->configMask) & (1 << ACL_MAC_ETYPE_KEYID)) != 0)
        {
          ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_ETYPE_FEATURE_ID);
        }
        if (((pRule->configMask) & (1 << ACL_MAC_SRCMAC)) != 0)
        {
          ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_SRCMAC_FEATURE_ID);
          if (memcmp(pRule->srcMacMask, hostMacMask, sizeof(hostMacMask)) != 0)
          {
            ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_SRCMAC_MASK_FEATURE_ID);
          }        
        }
        if (((pRule->configMask) & (1 << ACL_MAC_VLANID)) != 0)
        {
          ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_VLANID_FEATURE_ID);
        }
        if (((pRule->configMask) & (1 << ACL_MAC_VLANID_START)) != 0)
        {
          ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_VLANID_RANGE_FEATURE_ID);
        }
        if (((pRule->configMask) & (1 << ACL_MAC_VLANID2)) != 0)
        {
          ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID);
        }
        if (((pRule->configMask) & (1 << ACL_MAC_VLANID2_START)) != 0)
        {
          ACL_CLUSTER_CNFGR_FEATURE_CHECK(L7_ACL_RULE_MATCH_VLANID2_RANGE_FEATURE_ID);
        }
      }
      pRule = pRule->next;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verify received configuration against system defines and features
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  This assumes data has already been validated by the sender,
*         so we can skip basic checks, such as range checking, but
*         this system not have the same prod/plat defines or features 
*         enabled, need to fail if we cannot apply entire configuration.
*         The max ACL & rule limits are validated during receive.
*
* @end
*********************************************************************/
L7_RC_t aclClusterConfigVerify(void)
{
  L7_RC_t rc = L7_SUCCESS;

  ACL_CLUSTER_DLOG("ENTERING %s\n", __FUNCTION__);

  if ((aclClusterConfigIpAclsVerify() != L7_SUCCESS) ||
      (aclClusterConfigMacAclsVerify() != L7_SUCCESS))
  {
    rc = L7_FAILURE;
  }

  ACL_CLUSTER_DLOG("LEAVING %s (rc = %d)\n", __FUNCTION__, rc);

  return rc;
}

/*********************************************************************
* @purpose  Free memory for the entire IPv4/IPv6 ACL rule list
*
* @returns  void
*
* @notes  
*
* @end
*********************************************************************/
static void aclClusterConfigAclRulesFree(aclStructure_t *pAcl)
{
  aclRuleParms_t *curr = pAcl->head;
  while (curr != L7_NULLPTR)
  {
    aclRuleParms_t *next = curr->next;
    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, curr);
    curr = next;
  }
}

/*********************************************************************
* @purpose  Free memory for the entire MAC ACL rule list
*
* @returns  void
*
* @notes  
*
* @end
*********************************************************************/
static void aclClusterConfigMacAclRulesFree(aclMacStructure_t *pMacAcl)
{
  aclMacRuleParms_t *curr = pMacAcl->head;
  while (curr != L7_NULLPTR)
  {
    aclMacRuleParms_t *next = curr->next;
    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, curr);
    curr = next;
  }
}

/*********************************************************************
* @purpose  Free memory for all the received rules
*
* @returns  void
*
* @notes  In normal operation the rules are inserted directly into
*         the AVL tree, but we need a method to free allocated rules
*         on a receive error or an abort command.
*
* @end
*********************************************************************/
static void aclClusterConfigRulesFree()
{
  L7_uint32 i;

  for (i=0; i < aclRxCount; i++)
  {
    aclClusterConfigAclRulesFree(&pAclRxList[i]);
  }
  for (i=0; i < aclMacRxCount; i++)
  {
    aclClusterConfigMacAclRulesFree(&pAclMacRxList[i]);
  }
}

/*********************************************************************
* @purpose  Update intf/vlan configuration based on received ACLs.
*
* @param    L7_uint32 notifyIndex  @b{(input)} number of notify entries
*
* @returns  void
*
* @notes    This removes deleted ACLs from intf/vlan configuration,
*           and modifies intf/vlan configuration for ACL index changes.
*           intf/vlan updates are saved and done once per intf/vlan,
*           instead of per ACL list.
*
* @end
*********************************************************************/
void aclClusterConfigIntfVlanUpdate(L7_uint32 notifyIndex)
{
  L7_INTF_MASK_t intfMask[ACL_INTF_DIR_MAX];
  L7_VLAN_MASK_t vlanMask[ACL_INTF_DIR_MAX];
  L7_uint32 dir, i, j, k;
  L7_uint32 intIfNum;
  nimConfigID_t configIdNull;
  L7_BOOL inUse; /* used to track if intf/vlan & dir has ACLs */

  memset(&configIdNull, 0, sizeof(nimConfigID_t));
  memset(&intfMask, 0, sizeof(intfMask));
  memset(&vlanMask, 0, sizeof(vlanMask));

  /* first remove all deleted ACLs from intf/vlan configuration, 
     do this before we get an ACL list for each intf/vlan since
     removing moves location of entries in the ACL list */
  for (i=0; i < notifyIndex; i++)
  {
    if ((pAclNotifyList[i].rxIndex == 0) && (pAclNotifyList[i].currIndex != 0))
    {
      L7_ACL_TYPE_t aclType = aclImpTypeFromIndexGet(pAclNotifyList[i].currIndex);
      for (dir=0; dir < ACL_INTF_DIR_MAX; dir++) 
      {
        L7_ACL_ASSIGNED_INTF_LIST_t intfList;
        L7_ACL_ASSIGNED_VLAN_LIST_t vlanList;
        if (((aclType == L7_ACL_TYPE_MAC) && 
             (aclMacImpAssignedIntfDirListGet(pAclNotifyList[i].currIndex,
                                              dir, &intfList) == L7_SUCCESS)) ||
            (aclImpAssignedIntfDirListGet(pAclNotifyList[i].currIndex,
                                          dir, &intfList) == L7_SUCCESS))
        {
          for (j=0; j < intfList.count; j++)
          {
            /* mark interface for update */
            L7_INTF_SETMASKBIT(intfMask[dir], intfList.intIfNum[j]);
            /* remove from cfg, does not make any driver calls */
            if (aclIntfDirListRemove(intfList.intIfNum[j], dir, 
                                     aclType, pAclNotifyList[i].currIndex) != L7_SUCCESS)
            {
              LOG_MSG("%s: aclIntfDirListRemove failed for intIfNum=%u, dir=%u, acl=%u\n", 
                      __FUNCTION__, intfList.intIfNum[j], dir, pAclNotifyList[i].currIndex);
            }
            else
            {
              LOG_MSG("%s: removed acl:%u from intIfNum:%u, dir:%u, ACL does not exist in received configuration.\n",
                      __FUNCTION__, pAclNotifyList[i].currIndex, intfList.intIfNum[j], dir);
            }
          }
        }
        if (((aclType == L7_ACL_TYPE_MAC) && 
             (aclMacImpAssignedVlanDirListGet(pAclNotifyList[i].currIndex,
                                              dir, &vlanList) == L7_SUCCESS)) ||
            (aclImpAssignedVlanDirListGet(pAclNotifyList[i].currIndex, 
                                          dir, &vlanList) == L7_SUCCESS))
        {
          for (j = 0; j < vlanList.count; j++)
          {
            /* mark vlan for update */
            L7_VLAN_SETMASKBIT(vlanMask[dir], vlanList.vlanNum[j]);
            /* remove from cfg, does not make any driver calls */
            if (aclVlanDirListRemove(vlanList.vlanNum[j], dir, 
                                     aclType, pAclNotifyList[i].currIndex) != L7_SUCCESS)
            {
              LOG_MSG("%s: aclVlanDirListRemove failed for vlanNum=%u, dir=%u, acl=%u\n", 
                      __FUNCTION__, vlanList.vlanNum[j], dir, pAclNotifyList[i].currIndex);
            } 
            else
            {
              LOG_MSG("%s: removed acl:%u from vlan:%u, dir:%u, ACL does not exist in received configuration.\n",
                      __FUNCTION__, pAclNotifyList[i].currIndex, vlanList.vlanNum[j], dir);
            }
          }
        }    
      }
    }
  }

  /* it seems best to loop through interfaces once, 
      and loop through notify/acl lists multiple times,
      this does the rest of the work for index and driver updates */
  for (i = 1; i < L7_ACL_INTF_MAX_COUNT; i++)
  {
    aclIntfCfgData_t *pIntfCfg = L7_NULLPTR;
    if ((NIM_CONFIG_ID_IS_EQUAL(&aclCfgFileData->cfgParms.aclIntfCfgData[i].configId, 
                                &configIdNull)) ||
        (nimIntIfFromConfigIDGet(&aclCfgFileData->cfgParms.aclIntfCfgData[i].configId, 
                                 &intIfNum) != L7_SUCCESS) ||
        (aclMapIntfIsConfigurable(intIfNum, &pIntfCfg) != L7_TRUE) ||
        (pIntfCfg == L7_NULLPTR))
    {
      continue;
    }
    for (dir=0; dir < ACL_INTF_DIR_MAX; dir++)
    {
      aclIntfList_t *pIntfList = &pIntfCfg->intf_dir_Tb[dir];
      L7_uint32 intfDirRuleCount = 0;
      inUse = L7_FALSE;
      for (j = 0; j < L7_ACL_MAX_LISTS_PER_INTF_DIR; j++)
      {
        if (pIntfList->assignList[j].inUse != L7_TRUE)
        {
          break;
        }
        for (k=0; k < notifyIndex; k++)
        {
          if ((pAclNotifyList[k].rxIndex != 0) && 
              (pAclNotifyList[k].currIndex != 0) &&
              (pIntfList->assignList[j].id.aclId == pAclNotifyList[k].currIndex))
          {
            L7_uint32 ruleCount;
            /* mark interface for update */
            L7_INTF_SETMASKBIT(intfMask[dir], intIfNum);
            /* check if rule count exceeds L7_ACL_MAX_RULES_PER_MULTILIST,
               use rxIndex, AVL trees are already updated with new index */
            if (aclRuleCountGet(pIntfList->assignList[j].id.aclType, pAclNotifyList[k].rxIndex, 
                                &ruleCount) != L7_SUCCESS)
            {
              LOG_MSG("%s: aclRuleCountGet failed for aclId:%u, aclType:%u\n", 
                      __FUNCTION__, 
                      pIntfList->assignList[j].id.aclId, pIntfList->assignList[j].id.aclType);
              ruleCount = 0;
            }
            intfDirRuleCount += ruleCount;
            if (intfDirRuleCount > L7_ACL_MAX_RULES_PER_MULTILIST)
            {
              /* remove from intf configuration */
              if (aclIntfDirListRemove(intIfNum, dir, 
                                       pIntfList->assignList[j].id.aclType, 
                                       pIntfList->assignList[j].id.aclId) == L7_SUCCESS)
              {
                LOG_MSG("%s: removed aclId:%u, aclType:%u from intIfNum:%u, dir:%u, L7_ACL_MAX_RULES_PER_MULTILIST exceeded.\n",
                        __FUNCTION__, pIntfList->assignList[j].id.aclId, pIntfList->assignList[j].id.aclType, intIfNum, dir);
              }
              else
              {
                LOG_MSG("%s: failed to remove aclId:%u, aclType:%u from intIfNum:%u, dir:%u, L7_ACL_MAX_RULES_PER_MULTILIST exceeded.\n",
                        __FUNCTION__, pIntfList->assignList[j].id.aclId, pIntfList->assignList[j].id.aclType, intIfNum, dir);
              }
            }
            else
            {
              inUse = L7_TRUE;
              if (pAclNotifyList[k].rxIndex != pAclNotifyList[k].currIndex)
              {
                /* update in place to maintain sequence number */
                pIntfList->assignList[j].id.aclId = pAclNotifyList[k].rxIndex;
              }
            }
          }
        }
      }
      /* intf may have been marked on delete loop or above */
      if (L7_INTF_ISMASKBITSET(intfMask[dir], intIfNum))
      {
        ACL_CLUSTER_DLOG("%s: calling aclIntfUpdate with intIfNum=%u, dir=%u, ACL_INTF_REMOVE.\n", 
                         __FUNCTION__, intIfNum, dir);
        (void)aclIntfUpdate(intIfNum, dir, ACL_INTF_REMOVE);
        if (inUse == L7_TRUE)
        {
          ACL_CLUSTER_DLOG("%s: calling aclIntfUpdate with intIfNum=%u, dir=%u, ACL_INTF_ADD.\n", 
                           __FUNCTION__, intIfNum, dir);
          if (aclIntfUpdate(intIfNum, dir, ACL_INTF_ADD) != L7_SUCCESS)
          {
            LOG_MSG("%s: unable to assign ACL rules to intIfNum:%u, dir:%u.\n",
                    __FUNCTION__, intIfNum, dir);
          }
        }
      }
    }
  }

  for (i = 1; i <= L7_ACL_VLAN_MAX_COUNT; i++)
  {
    aclVlanCfgData_t *pVlanCfg = L7_NULLPTR;
    L7_BOOL   vlanInUse = L7_FALSE; /* ACLs assigned in either dir */
    L7_uint32 vlanId;
    if (aclCfgFileData->cfgParms.aclVlanCfgData[i].inUse != L7_TRUE)
    {
      continue;
    }
    vlanId = aclCfgFileData->cfgParms.aclVlanCfgData[i].vlanId;
    if ((aclMapVlanIsConfigurable(vlanId, &pVlanCfg) != L7_TRUE) ||
        (pVlanCfg == L7_NULLPTR))
    {
      continue;
    }
    for (dir=0; dir < ACL_INTF_DIR_MAX; dir++)
    {
      aclVlanList_t *pVlanList = &pVlanCfg->vlan_dir_Tb[dir];
      L7_uint32 vlanDirRuleCount = 0;
      inUse = L7_FALSE;
      for (j = 0; j < L7_ACL_MAX_LISTS_PER_VLAN_DIR; j++)
      {
        if (pVlanList->assignList[j].inUse != L7_TRUE)
        {
          break;
        }
        for (k=0; k < notifyIndex; k++)
        {
          if ((pAclNotifyList[k].rxIndex != 0) &&
              (pAclNotifyList[k].currIndex != 0) &&
              (pVlanList->assignList[j].id.aclId == pAclNotifyList[k].currIndex))
          {
            L7_uint32 ruleCount;
            /* mark vlan for update */
            L7_VLAN_SETMASKBIT(vlanMask[dir], vlanId);
            /* check if rule count exceeds L7_ACL_MAX_RULES_PER_MULTILIST,
               use rxIndex, AVL trees are already updated with new index */
            if (aclRuleCountGet(pVlanList->assignList[j].id.aclType, pAclNotifyList[k].rxIndex, 
                                &ruleCount) != L7_SUCCESS)
            {
              LOG_MSG("%s: aclRuleCountGet failed for aclId:%u, aclType:%u\n", 
                      __FUNCTION__, 
                      pVlanList->assignList[j].id.aclId, pVlanList->assignList[j].id.aclType);
              ruleCount = 0;
            }
            vlanDirRuleCount += ruleCount;
            if (vlanDirRuleCount > L7_ACL_MAX_RULES_PER_MULTILIST)
            {
              /* remove from vlan configuration */
              if (aclVlanDirListRemove(vlanId, dir, 
                                       pVlanList->assignList[j].id.aclType, 
                                       pVlanList->assignList[j].id.aclId) == L7_SUCCESS)
              {
                LOG_MSG("%s: removed aclId:%u, aclType:%u from vlanId:%u, dir:%u, L7_ACL_MAX_RULES_PER_MULTILIST exceeded.\n",
                        __FUNCTION__, pVlanList->assignList[j].id.aclId, pVlanList->assignList[j].id.aclType, vlanId, dir);
              }
              else
              {
                LOG_MSG("%s: failed to remove aclId:%u, aclType:%u from vlanId:%u, dir:%u, L7_ACL_MAX_RULES_PER_MULTILIST exceeded.\n",
                        __FUNCTION__, pVlanList->assignList[j].id.aclId, pVlanList->assignList[j].id.aclType, vlanId, dir);
              }
            }
            else
            {
              inUse = L7_TRUE;
              if (pAclNotifyList[k].rxIndex != pAclNotifyList[k].currIndex)
              {
                /* update in place to maintain sequence number */
                pVlanList->assignList[j].id.aclId = pAclNotifyList[k].rxIndex;
              }
            }
          }
        }
      }
      /* vlan may have been marked on delete loop or above */
      if (L7_VLAN_ISMASKBITSET(vlanMask[dir], vlanId))
      { 
        ACL_CLUSTER_DLOG("%s: calling aclVlanUpdate with vlanId=%u, dir=%u, ACL_VLAN_REMOVE.\n", 
                         __FUNCTION__, vlanId, dir);
        (void)aclVlanUpdate(vlanId, dir, ACL_VLAN_REMOVE);
        if (inUse == L7_TRUE)
        {
          vlanInUse = L7_TRUE;
          ACL_CLUSTER_DLOG("%s: calling aclVlanUpdate with vlanId=%u, dir=%u, ACL_VLAN_ADD.\n", 
                           __FUNCTION__, vlanId, dir);
          if (aclVlanUpdate(vlanId, dir, ACL_VLAN_ADD) != L7_SUCCESS)
          {
            LOG_MSG("%s: unable to assign ACL rules to vlanId:%u, dir:%u.\n",
                    __FUNCTION__, vlanId, dir);
          }
        }
      }
    }
    if (vlanInUse == L7_FALSE)
    {
      aclMapVlanConfigEntryRelease(i);
    }
  }

  return;
}

/*********************************************************************
* @purpose  Free memory for rules and delete current IP ACL from tree.
*
* @param    aclStructure_t *pAcl  @b{(input)} pointer to IPv4/IPv6 ACL
*
* @returns  void
*
* @notes 
*
* @end
*********************************************************************/
void aclClusterConfigAclDelete(aclStructure_t *pAcl)
{
  aclClusterConfigAclRulesFree(pAcl);

  if (ACL_SEMA_TAKE(pAclTree->semId, 
                    L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return;
  }

  if (avlDeleteEntry(pAclTree, pAcl) == L7_NULLPTR)
  {
    LOG_MSG("%s: avlDeleteEntry failed for aclNum=%u\n", 
            __FUNCTION__, pAcl->aclNum);
  }

  (void)ACL_SEMA_GIVE(pAclTree->semId);

  return;
}

/*********************************************************************
* @purpose  Free memory for rules and delete current MAC ACL from tree.
*
* @param    aclMacStructure_t *pMacAcl  @b{(input)} pointer to MAC ACL
*
* @returns  void
*
* @notes 
*
* @end
*********************************************************************/
void aclClusterConfigMacAclDelete(aclMacStructure_t *pMacAcl)
{
  /* free memory for all rules */
  aclClusterConfigMacAclRulesFree(pMacAcl);

  if (ACL_SEMA_TAKE(pAclMacTree->semId, 
                    L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return;
  }

  if (avlDeleteEntry(pAclMacTree, pMacAcl) == L7_NULLPTR)
  {
    LOG_MSG("%s: avlDeleteEntry failed for aclIndex=%u\n", 
            __FUNCTION__, pMacAcl->aclIndex);
  }

  (void)ACL_SEMA_GIVE(pAclMacTree->semId);

  return;
}

/*********************************************************************
*
* @purpose  Notify registered users of a change to an ACL.
*
* @param    aclnum      @b{(input)} ACL associated with the event
* @param    event       @b{(input)} Event indication
* @param    aclOldName  @b{(input)} Previous ACL name, or L7_NULLPTR
*
* @returns  void
*
* @comments It is important that this function only use a read lock.
*           The callback functions could lead to a path that takes a   
*           write lock which would cause a deadlock condition. 
*
* @comments The aclOldName parm is used for a rename or delete event
*           only, and is set to L7_NULLPTR otherwise.
*
* @comments If the ACL is being named for the first time, the aclOldName
*           parameter is non-null and points to an empty string "".
*
* @comments This function is common for all ACL types.
*
* @end
*
*********************************************************************/
void aclClusterNotify(L7_uint32 aclnum, aclEvent_t event, L7_uchar8 *aclOldName)
{
  L7_ACL_TYPE_t   aclType;

  aclType = aclImpTypeFromIndexGet(aclnum);

  if (aclType == L7_ACL_TYPE_NONE)
    return;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return;

  aclImpNotify(event, aclType, aclnum, aclOldName);

  (void)ACL_READ_LOCK_GIVE(aclRwLock);
}

/*********************************************************************
* @purpose  Function to apply entire received configuration
*
* @param    void
*
* @returns  void
*
* @notes  This function cannot fail, there is no way to abort once
*         we have verified the configuration.
*
* @end
*********************************************************************/
void aclClusterConfigApply(void)
{
  L7_uint32 notifyIndex = 0;
  aclStructure_t *pAcl = L7_NULLPTR;
  aclMacStructure_t *pMacAcl = L7_NULLPTR;
  L7_uint32 index = 0; /* aclNum or aclIndex */
  L7_uint32 i;

  memset(pAclNotifyList, 0, (sizeof(aclClusterNotifyData_t)*(L7_ACL_MAX_LISTS*2)));

  /* get the config API semaphore for entire apply */
  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return;
  }

  ACL_CLUSTER_DLOG("%s: applying %u IPv4/IPv6 ACL(s) and %u MAC ACL(s)\n", 
                   __FUNCTION__, aclRxCount, aclMacRxCount);

  /* pAclNotifyList saves all ACL index changes in order to do app notifies,
     and to fix intf/vlan configuration when a current ACL is in the received 
     configuration with a different index.
       rxIndex == currIndex indicates a modify to current ACL,
       rxIndex != currIndex indicates a delete for curr, create for rx, modify intf/vlan
       rxIndex != 0 && currIndex ==0, indicates a create for rx ACL 
       rxIndex == 0 && currIndex != 0 indicates a delete for current ACL */
  
  /* loop through rx list, save indexes as above and delete matching entries,
     then we are left with only deletes in the AVL tree. */
  for (i = 0; i < aclRxCount; i++)
  {
    index = pAclRxList[i].aclNum;
    pAclNotifyList[notifyIndex].rxIndex = index;
    pAclNotifyList[notifyIndex].currIndex = 0;
    if ((pAcl = avlSearchLVL7(pAclTree, &index, 
                              AVL_EXACT)) != L7_NULLPTR)
    {
      if (strncmp(pAclRxList[i].aclName, pAcl->aclName, L7_ACL_NAME_LEN_MAX) == 0)
      {
        /* TBD - could compare all rules to avoid notify and intf tear down/build up if no change */
        ACL_CLUSTER_DLOG("%s: found match on aclNum=%u and aclName=%s.\n", 
                         __FUNCTION__, index, pAcl->aclName);
        pAclNotifyList[notifyIndex].currIndex = index;
        aclClusterConfigAclDelete(pAcl);
        notifyIndex++;
        continue;
      }
      /* if name does not match, just leave current entry in tree, 
           it will either match another rx ACL on name or is a delete */
    }

    /* search for received ACL by name, 
       if found, save current index for intf/vlan updates */
    if (aclImpNameToIndexGet(pAclRxList[i].aclName, &index) == L7_SUCCESS)
    {
      ACL_CLUSTER_DLOG("%s: found match on aclName=%s, current aclNum=%u, received aclNum=%u.\n", 
                       __FUNCTION__, pAclRxList[i].aclName, index, pAclNotifyList[notifyIndex].rxIndex);
      pAclNotifyList[notifyIndex].currIndex = index;
      if ((pAcl = avlSearchLVL7(pAclTree, &index, 
                                AVL_EXACT)) != L7_NULLPTR)
      {
        osapiStrncpy(pAclNotifyList[notifyIndex].oldName, pAcl->aclName, L7_ACL_NAME_LEN_MAX);
        pAclNotifyList[notifyIndex].oldName[L7_ACL_NAME_LEN_MAX] = '\0';
        aclClusterConfigAclDelete(pAcl);
      }
    }
    notifyIndex++;
  }

  /* remaining entries are a delete */
  index = 0;
  while ((pAcl = avlSearchLVL7(pAclTree, &index, 
                               AVL_NEXT)) != L7_NULLPTR)
  {
    ACL_CLUSTER_DLOG("%s: deleting current aclNum=%u and aclName=%s.\n", 
                     __FUNCTION__, pAcl->aclNum, pAcl->aclName);
    pAclNotifyList[notifyIndex].rxIndex = 0;
    pAclNotifyList[notifyIndex].currIndex = pAcl->aclNum;
    osapiStrncpy(pAclNotifyList[notifyIndex].oldName, pAcl->aclName, L7_ACL_NAME_LEN_MAX);
    pAclNotifyList[notifyIndex].oldName[L7_ACL_NAME_LEN_MAX] = '\0';
    notifyIndex++;
    aclClusterConfigAclDelete(pAcl);
    /* aclClusterConfigAclDelete removes from avlTree, do not update search index */
  }

  /* do the same for MAC ACLs */
  for (i = 0; i < aclMacRxCount; i++)
  {
    index = pAclMacRxList[i].aclIndex;
    pAclNotifyList[notifyIndex].rxIndex = index;
    pAclNotifyList[notifyIndex].currIndex = 0;
    if ((pMacAcl = avlSearchLVL7(pAclMacTree, &index, 
                                 AVL_EXACT)) != L7_NULLPTR)
    {
      if (strncmp(pAclMacRxList[i].aclName, pMacAcl->aclName, L7_ACL_NAME_LEN_MAX) == 0)
      {
        /* TBD - could compare all rules to avoid notify and intf tear down/build up if no change */
        ACL_CLUSTER_DLOG("%s: found match on aclIndex=%u and aclName=%s.\n", 
                         __FUNCTION__, index, pMacAcl->aclName);
        pAclNotifyList[notifyIndex].currIndex = index;
        aclClusterConfigMacAclDelete(pMacAcl);
        notifyIndex++;
        continue;
      }
      /* if name does not match, just leave current entry in tree, 
           it will either match another rx ACL on name or is a delete */
    }

    /* search for received ACL by name, 
       if found, save current index for intf/vlan updates */
    if (aclMacImpNameToIndexGet(pAclMacRxList[i].aclName, &index) == L7_SUCCESS)
    {
      ACL_CLUSTER_DLOG("%s: found match on aclName=%s, current aclIndex=%u, received aclIndex=%u.\n", 
                       __FUNCTION__, pAclMacRxList[i].aclName, index, pAclNotifyList[notifyIndex].rxIndex);
      pAclNotifyList[notifyIndex].currIndex = index;
      if ((pMacAcl = avlSearchLVL7(pAclMacTree, &index, 
                                   AVL_EXACT)) != L7_NULLPTR)
      {
        osapiStrncpy(pAclNotifyList[notifyIndex].oldName, pMacAcl->aclName, L7_ACL_NAME_LEN_MAX);
        pAclNotifyList[notifyIndex].oldName[L7_ACL_NAME_LEN_MAX] = '\0';
        aclClusterConfigMacAclDelete(pMacAcl);
      }
    }
    notifyIndex++;
  }

  index = 0;
  while ((pMacAcl = avlSearchLVL7(pAclMacTree, &index, 
                                  AVL_NEXT)) != L7_NULLPTR)
  {
    ACL_CLUSTER_DLOG("%s: deleting current aclIndex=%u and aclName=%s.\n", 
                     __FUNCTION__, pMacAcl->aclIndex, pMacAcl->aclName);
    pAclNotifyList[notifyIndex].rxIndex = 0;
    pAclNotifyList[notifyIndex].currIndex = pMacAcl->aclIndex;
    osapiStrncpy(pAclNotifyList[notifyIndex].oldName, pMacAcl->aclName, L7_ACL_NAME_LEN_MAX);
    pAclNotifyList[notifyIndex].oldName[L7_ACL_NAME_LEN_MAX] = '\0';
    notifyIndex++;
    aclClusterConfigMacAclDelete(pMacAcl);
    /* aclClusterConfigAclDelete removes from avlTree, do not update search index */
  }

  /* reinit all global counters and index structures kept in sync with AVL trees */
  acl_curr_entries_g = 0;
  aclRuleNodeCount = 0;
  aclImpNamedIndexInUseInit(L7_ACL_TYPE_IP);
  aclImpNamedIndexInUseInit(L7_ACL_TYPE_IPV6);
  aclMacImpIndexInUseInit();

  /* insert all received ACLs and rules into tree */
  for (i=0; i < aclRxCount; i++)
  {
    if (avlInsertEntry(pAclTree, &pAclRxList[i]) == L7_NULLPTR)
    {
      L7_ACL_TYPE_t aclType = aclImpTypeFromIndexGet(pAclRxList[i].aclNum);
      acl_curr_entries_g++;
      aclRuleNodeCount += (pAclRxList[i].ruleCount + 1);  /* ruleCount does not include implicit deny all final rule */
      aclImpNamedIndexInUseUpdate(aclType, pAclRxList[i].aclNum, L7_TRUE);
      aclImpNamedIndexNextUpdate(aclType);
    }
    else
    {
      /* free memory for rules */
      aclClusterConfigAclRulesFree(&pAclRxList[i]);
      LOG_MSG("%s: unexpected error, failed to add aclNum=%u\n", 
              __FUNCTION__, pAclRxList[i].aclNum);
    }
  }

  for (i=0; i < aclMacRxCount; i++)
  {
    if (avlInsertEntry(pAclMacTree, &pAclMacRxList[i]) == L7_NULLPTR)
    {
      acl_curr_entries_g++;
      aclRuleNodeCount += (pAclMacRxList[i].ruleCount + 1);  /* ruleCount does not include implicit deny all final rule */
      aclMacImpIndexInUseUpdate(pAclMacRxList[i].aclIndex, L7_TRUE);
      aclMacImpIndexNextUpdate();
    }
    else
    {
      /* free memory for rules */
      aclClusterConfigMacAclRulesFree(&pAclMacRxList[i]);
      LOG_MSG("%s: unexpected error, failed to add aclIndex=%u\n", 
              __FUNCTION__, pAclMacRxList[i].aclIndex);
    }
  }

  /* update intf/vlan configuration based on any aclNum/aclIndex changes */
  /* perform all driver calls once per intf/vlan, instead of per list  */
  aclClusterConfigIntfVlanUpdate(notifyIndex);

  /* mark configuration changed */
  aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  /* release semaphore */
  (void)ACL_WRITE_LOCK_GIVE(aclRwLock);

  /* do notifies after semaphore release, 
       perform all currIndex delete notifies first, the way
       we construct the list we may have a create on an rx 
       ACL located before a delete on a curr ACL, with the same index */
  for (i=0; i < notifyIndex; i++)
  {
    if ((pAclNotifyList[i].currIndex != pAclNotifyList[i].rxIndex) &&
        (pAclNotifyList[i].currIndex != 0))
    {
      ACL_CLUSTER_DLOG("%s: sending ACL_EVENT_DELETE notify for index=%u, name=%s.\n", 
                       __FUNCTION__, pAclNotifyList[i].currIndex, pAclNotifyList[i].oldName); 
      aclClusterNotify(pAclNotifyList[i].currIndex, ACL_EVENT_DELETE, pAclNotifyList[i].oldName);
    }
  }
  for (i=0; i < notifyIndex; i++)
  {
    if (pAclNotifyList[i].currIndex == pAclNotifyList[i].rxIndex)
    {
      ACL_CLUSTER_DLOG("%s: sending ACL_EVENT_MODIFY notify for index=%u.\n", 
                       __FUNCTION__, pAclNotifyList[i].rxIndex); 
      aclClusterNotify(pAclNotifyList[i].rxIndex, ACL_EVENT_MODIFY, L7_NULLPTR);
    }
    else if (pAclNotifyList[i].rxIndex != 0)
    {
      ACL_CLUSTER_DLOG("%s: sending ACL_EVENT_CREATE notify for index=%u.\n", 
                       __FUNCTION__, pAclNotifyList[i].rxIndex); 
      aclClusterNotify(pAclNotifyList[i].rxIndex, ACL_EVENT_CREATE, L7_NULLPTR);
    }
  }
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
void aclClusterConfigRxCmdProcess(clusterConfigCmd cmd)
{
  L7_RC_t rc = L7_FAILURE;

  switch (cmd)
  {
    case CLUSTER_CONFIG_CMD_ABORT:
      /* need to free all memory for received rules */
      aclClusterConfigRulesFree();
      /* pass thru */
    case CLUSTER_CONFIG_CMD_START_TRANSFER:
      if (CLUSTER_CONFIG_CMD_START_TRANSFER == cmd)
      {
        ACL_CLUSTER_DLOG("%s: CLUSTER START received\n", __FUNCTION__);
      }
      else
      {
        ACL_CLUSTER_DLOG("%s: CLUSTER ABORT received\n", __FUNCTION__);
      }
      /* initialize temporary structures to receive entire configuration */
      memset(pAclRxList, 0, sizeof(aclStructure_t)*L7_ACL_MAX_LISTS);
      memset(pAclMacRxList, 0, sizeof(aclMacStructure_t)*L7_ACL_MAX_LISTS);
      aclRxCount = 0;
      aclMacRxCount = 0;
      aclRxRuleCount = 0;
      aclRxError = L7_FALSE;
      aclRxInProg = (CLUSTER_CONFIG_CMD_START_TRANSFER == cmd) ? L7_TRUE : L7_FALSE;
      break;
    case CLUSTER_CONFIG_CMD_END_TRANSFER:
      ACL_CLUSTER_DLOG("%s: CLUSTER END received (rxError=%u)\n", __FUNCTION__, aclRxError);
      if (aclRxError == L7_TRUE)
      {
        rc = L7_FAILURE;
      } 
      else
      {
        rc = aclClusterConfigVerify();
        if (rc != L7_SUCCESS)
        {
          ACL_CLUSTER_DLOG("%s: ACL received config verification failed.\n", __FUNCTION__);
          LOG_MSG("%s: ACL received config verification failed.\n", __FUNCTION__);
        }
      }
      if (clusterConfigRxDone(CLUSTER_CFG_ID(QOS_ACL), rc) != L7_SUCCESS)
      {
        LOG_MSG("%s: clusterConfigRxDone failed.\n", __FUNCTION__);
      }
      if (L7_SUCCESS != rc)
      {
        aclRxInProg = L7_FALSE;
      }
      ACL_CLUSTER_DLOG("%s: CLUSTER END completed (rxError=%u, rc=%u)\n", __FUNCTION__, aclRxError, rc);
      break;
    case CLUSTER_CONFIG_CMD_APPLY:
      ACL_CLUSTER_DLOG("%s: CLUSTER APPLY received\n", __FUNCTION__);
      aclClusterConfigApply();
      clusterConfigRxApplyDone(CLUSTER_CFG_ID(QOS_ACL));
      aclRxInProg = L7_FALSE;
      ACL_CLUSTER_DLOG("%s: CLUSTER APPLY completed\n", __FUNCTION__);
      break;
    default:
      ACL_CLUSTER_DLOG("%s: CLUSTER <UNKNOWN> received (cmd=%u)\n", __FUNCTION__, cmd);
      LOG_MSG("%s: Received unknown clusterConfigCmd=%u\n", __FUNCTION__, cmd);
      break;
  }

  return;
}

/*********************************************************************
* @purpose  Indicates if ACL cluster config receive operation currently in progress
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
L7_BOOL aclClusterIsRxInProgress(void)
{
  return aclRxInProg;
}

/*********************************************************************
*
* @purpose  Checks if config push aclnum exists
*
* @param    L7_uint32  aclnum   the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list was found
* @returns  L7_FAILURE, if the access list does not exist 
*
* @comments Searches the received config push data for a matching 
*           ACL number.
*
* @end
*
*********************************************************************/
L7_RC_t aclClusterNumCheckValid(L7_uint32 aclnum)
{
  L7_RC_t        rc = L7_FAILURE;
  L7_uint32      i;
  aclStructure_t *pAcl;

  if (aclClusterIsRxInProgress() != L7_TRUE)
  {
    return L7_FAILURE;
  }

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  for (i = 0; i < aclRxCount; i++)
  {
    pAcl = &pAclRxList[i];

    if (pAcl->aclNum == aclnum)
    {
      rc = L7_SUCCESS;
      break;
    }

  } /* endfor i */

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To get the number of a config push named access list, given its name.
*
* @param    *name       @b{(input)} access list name
* @param    *aclnum     @b{(output)} access list identifying number
*                                            
* @returns  L7_SUCCESS  named access list number is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list name does not exist
*
* @comments Searches the received config push data for a matching 
*           ACL number.
*
* @end
*
*********************************************************************/
L7_RC_t aclClusterNameToIndex(L7_uchar8 *name, L7_uint32 *aclnum)
{
  L7_RC_t        rc = L7_ERROR;
  L7_uint32      i;
  aclStructure_t *pAcl;

  /* check inputs */
  if ((name == L7_NULLPTR) || 
      (strlen(name) > L7_ACL_NAME_LEN_MAX))
  {
    return L7_FAILURE;
  }

  if (aclnum == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (aclClusterIsRxInProgress() != L7_TRUE)
  {
    return L7_FAILURE;
  }

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  for (i = 0; i < aclRxCount; i++)
  {
    pAcl = &pAclRxList[i];

    if (strcmp((char *)name, (char *)pAcl->aclName) == 0)
    {
      *aclnum = pAcl->aclNum;
      rc = L7_SUCCESS;
      break;
    }

  } /* endfor i */

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To get the index of a config push MAC access list, given its name.
*
* @param    *name       @b{(input)} access list name
* @param    *aclIndex   @b{(output)} access list index
*                                            
* @returns  L7_SUCCESS  MAC access list index is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list name does not exist
*
* @comments Searches the received config push data for a matching 
*           ACL number.
*
* @end
*
*********************************************************************/
L7_RC_t aclClusterMacNameToIndex(L7_uchar8 *name, L7_uint32 *aclIndex)
{
  L7_RC_t           rc = L7_ERROR;
  L7_uint32         i;
  aclMacStructure_t *pAcl;

  /* check inputs */
  if ((name == L7_NULLPTR) || 
      (strlen(name) > L7_ACL_NAME_LEN_MAX))
  {
    return L7_FAILURE;
  }

  if (aclIndex == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (aclClusterIsRxInProgress() != L7_TRUE)
  {
    return L7_FAILURE;
  }

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  for (i = 0; i < aclMacRxCount; i++)
  {
    pAcl = &pAclMacRxList[i];

    if (strcmp((char *)name, (char *)pAcl->aclName) == 0)
    {
      *aclIndex = pAcl->aclIndex;
      rc = L7_SUCCESS;
      break;
    }

  } /* endfor i */

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
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
void aclClusterDebugSet(L7_BOOL debug)
{
  aclClusterDebug = debug;
  return;
}
