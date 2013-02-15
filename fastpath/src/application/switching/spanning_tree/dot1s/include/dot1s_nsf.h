
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename    dot1s_nsf.h
* @purpose     802.1s Multiple Spanning Tree, Non stop forwarding support
* @component   dot1s
* @comments    none
* @create      1/23/09
* @author      akulkarn
* @end
*             
**********************************************************************/
#ifndef __DOT1S_NSF_H_
#define __DOT1S_NSF_H_

#include "ckpt_api.h"
#include "dot1s_cnfgr.h"
#include "comm_mask.h"


/* MSG header*/

#define DOT1S_CHECKPOINT_MESSAGE_VER 0
#define DOT1S_RECON_TIMEOUT_MS 12000 
#define DOT1S_PORT_COMMON_CKPT_SIZE 4

/****************************************************8888*/

typedef struct
{
  L7_ushort16 version;
  L7_ushort16 messageLen;

} dot1s_ckpt_header_t;
typedef enum
{
  DOT1S_INST_CKPT_READY = 0,
  DOT1S_INST_CKPT_BEGIN,
  DOT1S_INST_CKPT_INPROGRESS,
} dot1s_inst_ckpt_states_t;


typedef enum
{
  DOT1S_TLV_PORT_INST_CKPT_BEGIN = 1,
  DOT1S_TLV_PORT_INST_CKPT_END,
  DOT1S_TLV_PORT_COMMON = 10,
  DOT1S_TLV_PORT_INST_INFO,
} dot1s_checkpoint_tlvs_t;
/* Check point data structure*/
typedef struct
{
  L7_ushort16 tlvType;
  L7_ushort16 tlvLen;
  L7_uint32   index;
} dot1s_nsf_tlv_header_t;



typedef struct
{
  L7_uchar8   disputed:1;
  /* Need to make this structure 4 bytes
     so it would not cross word boundaries.
  */
  L7_uchar8   pad;
  L7_uchar8   portRole;
  L7_uchar8   portState;
} dot1s_nsf_port_inst_ckpt_t;

typedef struct
{
  L7_uchar8   operEdge:1;
  L7_uchar8   rcvdBpdu:1;
  L7_uchar8   rcvdRSTP:1;
  L7_uchar8   rcvdSTP:1;
  L7_uchar8   rcvdMSTP:1;
  L7_uchar8   sendRSTP:1;
  L7_uchar8   bpduGuardEffect:1;
  L7_uchar8   loopInconsistent:1;
  L7_uchar8   diagnosticDisable:1;
  /* Need to make this structure 4 bytes
     so it would not cross word boundaries.
  */
  L7_ushort16 pad;
} dot1s_nsf_port_common_ckpt_t;


typedef struct
{
  L7_uchar8 numStatesWait;
  L7_uchar8 waitingOnFlush;
}dot1s_inst_recon_data_t;

typedef struct
{
  /* Indicates if this intf is waiting on a PDU */
  L7_BOOL                  pduExpected; 
  /* All the reconciliation for this interface */
  L7_BOOL                  intfReconciled;
  /* reconciliation state of the interface*/
  dot1s_nsf_recon_states_t intReconState;

  dot1s_inst_recon_data_t instInfo[L7_MAX_MULTIPLE_STP_INSTANCES + 1];
} dot1s_intf_recon_data_t;

typedef struct
{
  dot1s_nsf_port_common_ckpt_t common;
  dot1s_nsf_port_inst_ckpt_t instInfo[L7_MAX_MULTIPLE_STP_INSTANCES + 1];
} dot1s_nsf_port_checkpoint_data_t;

extern L7_BOOL dot1sBackupMgrElected;
/*********************************************************************
* @purpose  Reset the check point data to prepare for further checkpoints 
*
* @param    
*
* @returns  L7_SUCCESS
*
* @notes  called during init and after the switchover is complete  
*
* @end
*********************************************************************/
L7_RC_t dot1sCheckPtDataReset();


/*********************************************************************
* @purpose Initialize the Non stop forwarding sub system for dot3ad
*
* @param    None
*
* @returns  L7_SUCCESS   on success
*           L7_FAILURE   otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dot1sNsfInit();


void dot1sNsfFini();

L7_RC_t dot1sNsfCallbacksRegister();
/*********************************************************************
* @purpose  Build the message to transmit to the backup side
*
* @param    checkpoint_data  @b{(input)}   buffer in which the message is to be populated
* @param    max_data_len    @b{(inout)}   Max len allocated for the data
* @param    ret_data_len  @b{(output)}  Actual length copied
* @param    more_data  @b{(output)}  Boolean to indicate if there is data available to checkpoint
*
* @returns  L7_SUCCESS
*
* @notes    caller is responsible for allocating checkpoint_data 
*
* @end
*********************************************************************/
L7_RC_t dot1sBuildCheckpointMessage(L7_uchar8   *checkpoint_data,
                      L7_uint32               max_data_len,
                      L7_uint32              *ret_data_len,
                      L7_BOOL                *more_data);

/*********************************************************************
* @purpose  Callback from the checkpoint service 
*
* @param    callback_type  @b{(input)}   callback type CKPT_NEW_BACKUP_MANAGER,
*                                       CKPT_NEXT_ELEMENT or CKPT_BACKUP_MANAGER_GONE
* @param    checkpoint_data  @b{(input)}   buffer in which the message is to be populated
* @param    max_data_len    @b{(inout)}   Max len allocated for the data
* @param    ret_data_len  @b{(output)}  Actual length copied
* @param    more_data  @b{(output)}  Boolean to indicate if there is data available to checkpoint
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
void dot1sManagerCheckpoint(L7_CKPT_MGR_CALLBACK_t  callback_type,
                      L7_uchar8              *checkpoint_data,
                      L7_uint32               max_data_len,
                      L7_uint32              *ret_data_len,
                      L7_BOOL                *more_data);

/*********************************************************************
* @purpose  Populate the check point datat structures on the backup unit with
*           checkpointed data. 
*
* @param    checkpoint_data  @b{(input)}  checkpointed data
* @param    data_len    @b{(inout)}   size of the data
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void dot1sPopulateCheckpointData(L7_uchar8  *checkpoint_data,
                                  L7_uint32  data_len);

/*********************************************************************
* @purpose  Callback for the backup to handle checkpointed data 
*
* @param    checkpoint_data  @b{(input)}  checkpointed data
* @param    data_len    @b{(inout)}   size of the data
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
void dot1sBackupUnitCheckpoint(L7_CKPT_BACKUP_MGR_CALLBACK_t callback_type,
                                    L7_uchar8                  *checkpoint_data,
                                    L7_uint32                   data_len);

/*********************************************************************
* @purpose  Notify the checkpoint service about availability of data 
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
*
* @returns  L7_SUCCESS
*
* @notes    a. sets the bitmask for the appropriate interface
*           b. copies the data from operational structure to checkpoint strucutres
*
* @end
*********************************************************************/
L7_RC_t dot1sCallCheckpointService(L7_INTF_MASK_t *intfMask, L7_uint32 instIndex);

dot1s_nsf_port_checkpoint_data_t *dot1sIntfCkptDataFind(L7_uint32 intIfNum);


/* APIs Used by rest of the app */
L7_BOOL dot1sIsIntfReconciled(L7_uint32 intIfNum);
L7_RC_t dot1sCkptStateGet(L7_uint32 intIfNum, L7_uint32 instIndex, 
                             L7_uint32 *state);
L7_RC_t dot1sNsfReconEventFunc(L7_uint32 intIfNum, L7_uint32 instIndex,
                               dot1s_recon_intf_events_t reconEvent);
void dot1sCheckGlobalReconciled(L7_BOOL coldRestart);
void dot1sUpdateIntfReconciled(L7_uint32 intIfNum);
L7_RC_t dot1sNsfWarmRestartPrepare();
L7_RC_t dot1sIntfReconcile(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sNsfStateSet(L7_uint32 intIfNum, L7_uint32 instIndex, 
                         L7_uint32 state, L7_BOOL isCkpted);
void dot1sNsfActivateStartupBegin();
void dot1sNsfCheckpointPrepare();
void dot1sReconcileTimerExpiredProcess();

#endif /* __DOT1S_NSF_H_*/
