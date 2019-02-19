/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename diffserv_cluster.h
*
* @purpose DiffServ - support for clustering
*
* @component DiffServ
*
* @comments
*
* @create 02/28/2008
*
* @author gpaussa
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_DIFFSERV_CLUSTER_H
#define INCLUDE_DIFFSERV_CLUSTER_H

#include "datatypes.h"
#include "clustering_commdefs.h"

#define DS_CLUSTER_QUEUE_SIZE   5
#define DS_CLUSTER_MSG_SIZE_MAX  CLUSTER_MSG_RELIABLE_SIZE_MAX

#define DIFFSERV_CLUSTER_DLOG(__fmt__, __args__...) \
  if (dsClusterDebug == L7_TRUE) \
  { \
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, __fmt__, ## __args__); \
  }

/* macro for checking IE definition counts */
#define DS_CLUSTER_IE_DEF_COUNT_CHECK(_var, _val) \
  if ((_var) != (_val)) \
  { \
    dsClusterRxError = L7_TRUE; \
  }

/* custom set/get macro definitions */
#define DS_CLUSTER_PKT_INT32_GET_OR_SET(_set_op, _field, _buffer, _offset) \
  do \
  { \
    L7_uint32   _tmp = (_field); \
    _dsClusterPktInt32GetOrSet((_set_op), &_tmp, (_buffer), (_offset)); \
    (_field) = _tmp; \
  } while (0);

#define DS_CLUSTER_PKT_INT16_GET_OR_SET(_set_op, _field, _buffer, _offset) \
  do \
  { \
    L7_ushort16 _tmp = (_field); \
    _dsClusterPktInt16GetOrSet((_set_op), &_tmp, (_buffer), (_offset)); \
    (_field) = _tmp; \
  } while (0);

#define DS_CLUSTER_PKT_INT8_GET_OR_SET(_set_op, _field, _buffer, _offset) \
  do \
  { \
    L7_uchar8   _tmp = (_field); \
    _dsClusterPktInt8GetOrSet((_set_op), &_tmp, (_buffer), (_offset)); \
    (_field) = _tmp; \
  } while (0);

#define DS_CLUSTER_PKT_STRING_GET_OR_SET(_set_op, _field, _len, _buffer, _offset) \
  do \
  { \
    _dsClusterPktStringGetOrSet((_set_op), (_field), (_len), (_buffer), (_offset)); \
  } while (0);


/* 0x1100-0x11FF reserved for QoS, use 0x1180-0x11FF for diffServ */
typedef enum
{
  DS_CLUSTER_MSG_CONFIG = 0x1180,
  DS_CLUSTER_MSG_LAST   = 0x11FF

} dsClusterMsgType;

typedef enum
{
  DS_CLUSTER_MSG_IE_NONE                      = 0x0000, /* invalid IE - can use as flag */

  DS_CLUSTER_MSG_IE_CONFIG_START              = 0x0001,
    #define DS_CLUSTER_MSG_IE_CONFIG_START_SIZE                 0

  DS_CLUSTER_MSG_IE_GLOBAL_CONFIG             = 0x0002,
    #define DS_CLUSTER_MSG_IE_GLOBAL_CONFIG_SIZE                4

  DS_CLUSTER_MSG_IE_CLASS_CONFIG_START        = 0x0003,
    #define DS_CLUSTER_MSG_IE_CLASS_CONFIG_START_SIZE           2

  DS_CLUSTER_MSG_IE_CLASS_DEF                 = 0x0004,
    #define DS_CLUSTER_MSG_IE_CLASS_DEF_SIZE                    100

  DS_CLUSTER_MSG_IE_CLASS_CONFIG_END          = 0x0005,
    #define DS_CLUSTER_MSG_IE_CLASS_CONFIG_END_SIZE             2

  DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_START   = 0x0006,
    #define DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_START_SIZE      2

  DS_CLUSTER_MSG_IE_CLASS_RULE_DEF            = 0x0007,
    #define DS_CLUSTER_MSG_IE_CLASS_RULE_DEF_SIZE               76

  DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_END     = 0x0008,
    #define DS_CLUSTER_MSG_IE_CLASS_RULE_CONFIG_END_SIZE        2

  DS_CLUSTER_MSG_IE_POLICY_CONFIG_START       = 0x0009,
    #define DS_CLUSTER_MSG_IE_POLICY_CONFIG_START_SIZE          2

  DS_CLUSTER_MSG_IE_POLICY_DEF                = 0x000A,
    #define DS_CLUSTER_MSG_IE_POLICY_DEF_SIZE                   56

  DS_CLUSTER_MSG_IE_POLICY_CONFIG_END         = 0x000B,
    #define DS_CLUSTER_MSG_IE_POLICY_CONFIG_END_SIZE            2

  DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_START  = 0x000C,
    #define DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_START_SIZE     2

  DS_CLUSTER_MSG_IE_POLICY_INST_DEF           = 0x000D,
    #define DS_CLUSTER_MSG_IE_POLICY_INST_DEF_SIZE              32

  DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_END    = 0x000E,
    #define DS_CLUSTER_MSG_IE_POLICY_INST_CONFIG_END_SIZE       2

  DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_START  = 0x000F,
    #define DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_START_SIZE     2

  DS_CLUSTER_MSG_IE_POLICY_ATTR_DEF           = 0x0010,
    #define DS_CLUSTER_MSG_IE_POLICY_ATTR_DEF_SIZE              68

  DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_END    = 0x0011,
    #define DS_CLUSTER_MSG_IE_POLICY_ATTR_CONFIG_END_SIZE       2

  DS_CLUSTER_MSG_IE_CONFIG_END                = 0x0012
    #define DS_CLUSTER_MSG_IE_CONFIG_END_SIZE                   0

} dsClusterMsgIe_t;

typedef enum
{
  DS_CLUSTER_IE_GROUP_NONE = 0,
  DS_CLUSTER_IE_GROUP_START,
  DS_CLUSTER_IE_GROUP_DEF,
  DS_CLUSTER_IE_GROUP_END

} dsClusterIeGroup_t;

typedef enum
{
  DS_CLUSTER_QUEUE_MSG_CONFIG_MSG = 1,
  DS_CLUSTER_QUEUE_MSG_CONFIG_SEND,
  DS_CLUSTER_QUEUE_MSG_CONFIG_RX
} dsClusterQueueMsgId_t;

typedef struct dsClusterMsg_s 
{
  L7_uint32 length;
  L7_uchar8 buffer[DS_CLUSTER_MSG_SIZE_MAX];
} dsClusterMsg_t;

typedef struct dsClusterQueueMsg_s
{
  dsClusterQueueMsgId_t type;
  union
  {
    dsClusterMsg_t    msg;
    clusterConfigCmd  configCmd;
    clusterMemberID_t memberID;
  } content;

} dsClusterQueueMsg_t;

typedef void (*dsClusterTableAccessFn_t)(L7_BOOL           isSet,
                                         L7_diffServCfg_t  *pCfgData,
                                         L7_uint32         entryIndex,
                                         L7_uchar8         *buffer,
                                         L7_uint32         *offset);

typedef L7_RC_t (*dsClusterTableVerifyFn_t)(L7_diffServCfg_t *pCfgData,
                                            L7_uint32        entryIndex);

typedef struct dsClusterTableParms_s
{
  L7_uint32                   startEnt;
  L7_uint32                   maxEnt;
  L7_ushort16                 startType;
  L7_ushort16                 startTypeSize;
  L7_ushort16                 defType;
  L7_ushort16                 defTypeSize;
  L7_ushort16                 endType;
  L7_ushort16                 endTypeSize;
  dsClusterTableAccessFn_t    accessFn;
  dsClusterTableVerifyFn_t    verifyFn;

} dsClusterTableParms_t;


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
L7_RC_t diffServClusterDebugShow(void);

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
L7_RC_t diffServClusterPhase1Init(void);

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
L7_RC_t diffServClusterPhase2Init(void);

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
void diffServClusterTask(void);

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
                                L7_uchar8               *msgBuf);

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
void diffServClusterConfigSendCallback(clusterMemberID_t *memberID);

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
void diffServClusterConfigRxCallback(clusterConfigCmd cmd);

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
L7_RC_t diffServClusterConfigRxIeSeqCheck(L7_ushort16 elemId);

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
void diffServClusterConfigMsgProcess(dsClusterMsg_t *msg);

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
                                         L7_uint32         *offset);

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
                                             L7_uint32         *offset);

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
                                          L7_uint32         *offset);

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
                                              L7_uint32         *offset);

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
                                              L7_uint32         *offset);

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
                                        L7_uint32         *offset);

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
                                       L7_uint32         *offset);

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
L7_RC_t diffServClusterConfigSend(clusterMemberID_t *memberID);

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
                                          L7_uint32         *offset);

/*********************************************************************
* @purpose  Verify received DiffServ class config against
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
L7_RC_t diffServClusterConfigClassVerify(L7_diffServCfg_t *pCfgData,
                                         L7_uint32        entryIndex);

/*********************************************************************
* @purpose  Verify received DiffServ class rule config against system
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
L7_RC_t diffServClusterConfigClassRuleVerify(L7_diffServCfg_t *pCfgData,
                                             L7_uint32        entryIndex);

/*********************************************************************
* @purpose  Verify received DiffServ policy config against system
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
L7_RC_t diffServClusterConfigPolicyVerify(L7_diffServCfg_t *pCfgData,
                                          L7_uint32        entryIndex);

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
                                              L7_uint32        entryIndex);

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
                                              L7_uint32        entryIndex);

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
L7_RC_t diffServClusterConfigServiceIntfRemap(L7_diffServCfg_t *pCfgData);

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
L7_RC_t diffServClusterConfigVerify(L7_diffServCfg_t *pCfgData);

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
void diffServClusterConfigApply(void);

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
void diffServClusterConfigRxCmdProcess(clusterConfigCmd cmd);

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
void diffServClusterDebugSet(L7_BOOL debug);

#endif

