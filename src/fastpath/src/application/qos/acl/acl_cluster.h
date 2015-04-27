/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename acl_cluster.h
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

#ifndef INCLUDE_ACL_CLUSTER_H
#define INCLUDE_ACL_CLUSTER_H

#include "clustering_commdefs.h"

#define ACL_CLUSTER_QUEUE_SIZE          5
#define ACL_CLUSTER_MSG_SIZE_MAX        CLUSTER_MSG_RELIABLE_SIZE_MAX
#define ACL_CLUSTER_DEFAULT_RULE_NUM    0xffffffffU   /* platform independent */

#define ACL_CLUSTER_DLOG(__fmt__, __args__...) \
  if (aclClusterDebug == L7_TRUE) \
  { \
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_FLEX_QOS_ACL_COMPONENT_ID, __fmt__, ## __args__); \
  }

#define ACL_CLUSTER_CNFGR_FEATURE_CHECK(_id) \
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID, \
                            (_id)) != L7_TRUE) \
  { \
    ACL_CLUSTER_DLOG("%s: feature check failed for %u.\n", __FUNCTION__, (_id)); \
    return L7_FAILURE; \
  }

/* 0x1100-0x11FF reserved for QoS, use 0x1100-0x117F for ACL */
typedef enum {
  ACL_CLUSTER_MSG_CONFIG = 0x1100,

  ACL_CLUSTER_MSG_LAST = 0x117F
} aclClusterMsgType;

typedef enum {
  ACL_CLUSTER_MSG_IE_NONE                     = 0x0000, /* invalid IE - can use as flag */

  ACL_CLUSTER_MSG_IE_CONFIG_START             = 0x0001,
    #define ACL_CLUSTER_MSG_IE_CONFIG_START_SIZE                0

  ACL_CLUSTER_MSG_IE_CONFIG_END               = 0x0002,
    #define ACL_CLUSTER_MSG_IE_CONFIG_END_SIZE                  0

  ACL_CLUSTER_MSG_IE_ACL_DEF_START            = 0x0003,
    #define ACL_CLUSTER_MSG_IE_ACL_DEF_START_SIZE               4

  ACL_CLUSTER_MSG_IE_ACL_DEF_END              = 0x0004,
    #define ACL_CLUSTER_MSG_IE_ACL_DEF_END_SIZE                 4

  ACL_CLUSTER_MSG_IE_ACL_ID                   = 0x0005,
    #define ACL_CLUSTER_MSG_IE_ACL_ID_SIZE                      32

  ACL_CLUSTER_MSG_IE_RULE_GROUP_START         = 0x0006,
    #define ACL_CLUSTER_MSG_IE_RULE_GROUP_START_SIZE            2

  ACL_CLUSTER_MSG_IE_RULE_GROUP_END           = 0x0007,
    #define ACL_CLUSTER_MSG_IE_RULE_GROUP_END_SIZE              2

  ACL_CLUSTER_MSG_IE_RULE_DEF_IP              = 0x0008,
    #define ACL_CLUSTER_MSG_IE_RULE_DEF_IP_SIZE                 96

  ACL_CLUSTER_MSG_IE_RULE_DEF_MAC             = 0x0009
    #define ACL_CLUSTER_MSG_IE_RULE_DEF_MAC_SIZE                80

} aclClusterMsgIEType;

typedef enum {
  ACL_CLUSTER_QUEUE_MSG_CONFIG_MSG,
  ACL_CLUSTER_QUEUE_MSG_CONFIG_SEND,
  ACL_CLUSTER_QUEUE_MSG_CONFIG_RX
} aclClusterQueueMsgType;

typedef struct aclClusterMsg_s 
{
  L7_uint32 length;
  L7_uchar8 buffer[ACL_CLUSTER_MSG_SIZE_MAX];
} aclClusterMsg_t;

typedef struct aclClusterQueueMsg_s
{
  aclClusterQueueMsgType type;
  union {
    aclClusterMsg_t   msg;
    clusterConfigCmd  configCmd;
    clusterMemberID_t memberID;
  } content;
} aclClusterQueueMsg_t;

typedef struct aclClusterNotifyData_s
{
  L7_uint32 currIndex;  /* current aclNum or aclIndex */
  L7_uint32 rxIndex;    /* received aclNum or aclIndex */
  L7_uchar8 oldName[L7_ACL_NAME_LEN_MAX+1]; /* old ACL name (for deletion notifications) */
} aclClusterNotifyData_t;

/* function prototypes */
L7_RC_t aclClusterPhase1Init(void);
L7_RC_t aclClusterPhase2Init(void);
aclRuleParms_t *aclClusterIpAclRuleNodeAlloc(aclStructure_t *pAcl);
aclMacRuleParms_t *aclClusterMacAclRuleNodeAlloc(aclMacStructure_t *pMacAcl);
void aclClusterTask(void);
void aclClusterMsgCallback(L7_ushort16              msgType,
                           clusterMsgDeliveryMethod method,
                           L7_uint32                msgLength,
                           L7_uchar8               *msgBuf);
void aclClusterConfigSendCallback(clusterMemberID_t *memberID);
void aclClusterConfigRxCallback(clusterConfigCmd cmd);
void aclClusterConfigIpAclRuleParse(L7_ACL_TYPE_t    type,
                                    L7_uchar8       *buffer,
                                    L7_uint32       *offset,
                                    aclRuleParms_t  *pRule);
void aclClusterConfigMacAclRuleParse(L7_uchar8          *buffer,
                                     L7_uint32          *offset,
                                     aclMacRuleParms_t  *pRule);
void aclClusterConfigMsgProcess(aclClusterMsg_t *msg);
void aclClusterConfigMacAclRuleWrite(aclMacRuleParms_t *pRule,
                                     L7_uchar8         *buffer,
                                     L7_uint32         *offset);
void aclClusterConfigIpAclRuleWrite(L7_ACL_TYPE_t    type,
                                    aclRuleParms_t  *pRule,
                                    L7_uchar8       *buffer,
                                    L7_uint32       *offset);
L7_RC_t aclClusterConfigIpAclsSend(clusterMemberID_t *memberID,
                                   L7_uchar8         *buffer,
                                   L7_uint32         *offset);
L7_RC_t aclClusterConfigMacAclsSend(clusterMemberID_t *memberID,
                                    L7_uchar8         *buffer,
                                    L7_uint32         *offset);
L7_RC_t aclClusterConfigSend(clusterMemberID_t *memberID);
L7_RC_t aclClusterConfigIpAclsVerify(void);
L7_RC_t aclClusterConfigMacAclsVerify(void);
L7_RC_t aclClusterConfigVerify(void);
void aclClusterConfigIntfVlanUpdate(L7_uint32 notifyIndex);
void aclClusterConfigAclDelete(aclStructure_t *pAcl);
void aclClusterConfigMacAclDelete(aclMacStructure_t *pMacAcl);
void aclClusterNotify(L7_uint32 aclnum, aclEvent_t event, L7_uchar8 *aclOldName);
void aclClusterConfigApply(void);
void aclClusterConfigRxCmdProcess(clusterConfigCmd cmd);
void aclClusterDebugSet(L7_BOOL debug);

#endif

