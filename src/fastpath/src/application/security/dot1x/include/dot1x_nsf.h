/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_ih.h
*
* @purpose   dot1x Interface Handler
*
* @component dot1x
*
* @comments  none
*
* @create    01/31/2003
*
* @author    mfiorito
*
* @end
*             
**********************************************************************/


#ifndef __DOT1X_NSF_H__
#define __DOT1X_NSF_H__

#include "l7_common.h"
#include "ckpt_api.h"
#include "sysapi.h"
#include "user_mgr_api.h"
#include "dot1x_exports.h"
#include "dot1x_db.h"


/* The message Version defined in this release*/
#define DOT1X_CHECKPOINT_MESSAGE_VER 0


/* MSG header*/
/*
   The message looks like
   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
   |   Message Version (2)         |     Message Length (2)        |
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


 DOT3AD_LAG_PORT_ADD_TLV                           
                    
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |   TLV Type (2)            |    TLV Length(2)                 |
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        Element Key (4)                       |
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        Agg port ID (4)                       |
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        Actor oper key (4)                    |
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        Partner oper sys (6)                  |
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |   oper sys (contd)        |partnerOperPortState (1) | PAD(1) |
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        partnerOperSysPri (4)                 |
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        partnerOperKey (4)                    |
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        partnerOperPortNumber (4)             |
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        partnerOperPortPri (4)                |
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


DOT3AD_LAG_PORT_DELETE_TLV
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |   TLV Type (2)            |    TLV Length(2)                 |
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        Element Key (4)                       |
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        Agg port ID (4)                       |
   |+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/


/* This type define is used both internally by the check point code and in the check point message*/
typedef enum dot1xCheckpointTlv_e
{
  DOT1X_TLV_NONE = 0,
  DOT1X_LOG_PORT_ADD_TLV,
  DOT1X_LOG_PORT_DELETE_TLV,
  DOT1X_LOG_PORT_MODIFY_TLV,
  DOT1X_PHY_PORT_TLV
}dot1xCheckpointTlvs_t;

typedef enum dot1xCheckpointRecord_e
{
  DOT1X_CKPT_RECORD_NEW =1,
  DOT1X_CKPT_RECORD_UPDATE,
  DOT1X_CKPT_RECORD_DELETED,
  DOT1X_CKPT_RECORD_CKPTED
} dot1xCheckpointRecord_t;

typedef struct dot1xLPortCheckPointData_s
{
  L7_uint32   logicalPortNumber; 
  L7_uchar8    currentIdL;       /* ID of current auth session (0-255) */
  L7_DOT1X_PORT_STATUS_t  logicalPortStatus;      /* Current authorization state of the port */
  L7_uint32 serverStateLen;
  L7_uchar8 serverState[DOT1X_SERVER_STATE_LEN];
  L7_uchar8 serverClass[DOT1X_SERVER_CLASS_LEN];
  L7_uint32 serverClassLen;
  L7_uint32 sessionTimeout;
  L7_enetMacAddr_t suppMacAddr;   /* MAC address of Supplicant */ 

  /* user Details */
  L7_uchar8 dot1xUserName[DOT1X_USER_NAME_LEN];
  L7_uint32 dot1xUserNameLength;
  L7_uchar8 dot1xChallenge[DOT1X_CHALLENGE_LEN];
  L7_uint32 dot1xChallengelen;

  L7_uint32 terminationAction;
  L7_uint32 vlanId;   /* Vlan Id of Supplicant */ 
  L7_uint32 vlanAssigned; /* stores the assigned vlan */
  L7_uchar8 filterName[DOT1X_FILTER_NAME_LEN]; 
  L7_uint32 unauthVlan; /* unauthenticated vlan */
  L7_uint32 guestVlanId;    /*Guest Vlan Id on the interface */
  L7_uint32 voiceVlanId;    /* To enable Voip devices on the interface */
  L7_BOOL   isMABClient;    /* TRUE if client was authenticated using MAB */
  L7_uint32 defaultVlanId;         /* Monitor Mode Default VLANID */
  L7_BOOL   isMonitorModeClient;    /* TRUE if client was authenticated using Monitor Mode */
  L7_uint32 blockVlanId;    /* blocked l2 entry vlan id */
  dot1xCheckpointRecord_t    recordType;

  void *reserved; /* AVL TREE requires this as last */

} dot1xLPortCheckPointData_t;


typedef struct dot1xPhyPortCheckPointData_s
{
  L7_uint32      authCount;       /* number of authorized clients */  
  L7_BOOL        portMacAddrAdded; /* flag to keep track of if Client Mac address was added to the Mac DB for a port-based port */

}dot1xPhyPortCheckPointData_t;

typedef struct
{
  L7_ushort16 tlvType;
  L7_ushort16 tlvLen;
  L7_uint32   index;
  void *tlvData;
} dot1x_nsf_tlv_header_t;


typedef struct
{
  L7_ushort16 version;
  L7_ushort16 messageLen;

} dot1x_ckpt_header_t;

/* Add the size of index to the data */
#define DOT1X_LOG_PORT_ADD_TLV_LEN (sizeof(dot1xLPortCheckPointData_t) + sizeof(L7_uint32))
#define DOT1X_LOG_PORT_DELETE_TLV_LEN sizeof(L7_uint32)
#define DOT1X_PHY_PORT_TLV_LEN (sizeof(dot1xPhyPortCheckPointData_t) + sizeof(L7_uint32))
#define DOT1X_TLV_HEADER_LEN (2*sizeof(L7_ushort16))

/*********************************************************************/
/*********************************************************************/



/*********************************************************************
* @purpose Initialize the Non stop forwarding sub system for dot1x
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
L7_RC_t dot1xNsfInit();

/*********************************************************************
* @purpose Release the resources acquired for the NSF subsytem of dot1x
*
* @param    None
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void dot1xNsfFini();

/*********************************************************************
* @purpose Initialize the Non stop forwarding sub system for dot1s
*
* @param    None
*
* @returns  L7_SUCCESS   on success
*           L7_NO_MEM    if memory not sufficient
*           L7_FAILURE   otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dot1xNsfCallbacksRegister();

/*********************************************************************
* @purpose  Clean stale check pointed data (if any) 
*
* @param    
*
* @returns  L7_SUCCESS
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t dot3adCheckPtDataClean();

/*********************************************************************
* @purpose  Reset the check pointed data. 
*
* @param    
*
* @returns  L7_SUCCESS
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t dot3adNsfCheckPtDataReset();

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
void dot1xManagerCheckpoint(L7_CKPT_MGR_CALLBACK_t  callback_type,
                        L7_uchar8              *checkpoint_data,
                        L7_uint32               max_data_len,
                        L7_uint32              *ret_data_len,
                        L7_BOOL                *more_data);

/*********************************************************************
* @purpose  Notify the checkpoint service about availability of data 
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    opcode  @b{(input)}  checkpointed data mode of type DOT3AD_NSF_OPCODE_t 
*                                type of values DOT3AD_NSF_OPCODE_ADD, 
*                                DOT3AD_NSF_OPCODE_MODIFY or DOT3AD_NSF_OPCODE_DELETE
* @returns  L7_SUCCESS
*
* @notes    a. sets the bitmask for the appropriate interface
*           b. copies the data from operational structure to checkpoint strucutres
*
* @end
*********************************************************************/
L7_RC_t dot1xCallCheckpointService(L7_uint32 intIfNum, 
                                   dot1xCheckpointTlvs_t recordType);

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
void dot1xBackupUnitCheckpoint(L7_CKPT_BACKUP_MGR_CALLBACK_t callback_type,
                                    L7_uchar8                  *checkpoint_data,
                                    L7_uint32                   data_len);

/*********************************************************************
* @purpose  Restart timers in case of a warm restart
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_SUPPORTED
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot1xNsfTimersRestart(void);

/*********************************************************************
* @purpose  Get the next logical interface from the checkpoint tree
*
* @param    L7_uint32                  lIntIfNum      @b{(input)} logical intf
* @param    dot1xLPortCheckPointData_t pDot1xCkptNode @b{(output)} pointer to node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dot1xCkptLogicalPortNextGet(L7_uint32 lIntIfNum,
                                    dot1xLPortCheckPointData_t *pDot1xCkptNode);

/*********************************************************************
* @purpose  To Get a Logical Port Info Node
*
* @param    lIntIfNum  @b{(input)} The logical internal interface number
*
* @returns  Logical Internal Interface node
*
* @comments none
*
* @end
*********************************************************************/
dot1xLPortCheckPointData_t* dot1xCkptLogicalPortInfoGet(L7_uint32 lIntIfNum);

L7_uchar8 *dot1xNsfDebugTlvStringGet(dot1xCheckpointTlvs_t tlvType);
L7_uchar8 *dot1xNsfDebugCkptTypeStringGet(dot1xCheckpointRecord_t ckptType);
#endif /* __DOT1X_NSF_H__*/
