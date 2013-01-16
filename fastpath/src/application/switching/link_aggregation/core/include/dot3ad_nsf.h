#ifndef __DOT3AD_NSF_H__
#define __DOT3AD_NSF_H__

#include "l7_common.h"
#include "dot3ad_db.h"
#include "ckpt_api.h"
#include "sysapi.h"


extern L7_uint32 dot3adNsfDebugFlag;
/* The message Version defined in this release*/
#define DOT3AD_CHECKPOINT_MESSAGE_VER 0
#define SIZEOF_DOT3AD_CHECKPOINT_AGG_DATA_RECORD (sizeof(L7_uint32) + sizeof(L7_ushort16) + sizeof(L7_ushort16) +\
                                                   sizeof(dot3ad_agg_checkpoint_data_t)) 


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
typedef enum dot3adCheckpointTlv_e
{
  DOT3AD_LAG_PORT_ADD_TLV = 1,
  DOT3AD_LAG_PORT_DELETE_TLV,
  DOT3AD_CHECKPOINT_AGG_DATA
}dot3adCheckpointTlvs_t;

typedef struct dot3ad_port_checkpoint_data_s
{
  L7_uint32         actorPortAggId;        
  L7_uint32         actorOperPortKey;       /* intIfNum */

 /*dot3ad_partner_info_t partnerOperData;*/
  L7_enetMacAddr_t  partnerOperSys;
  L7_uchar8         partnerOperPortState;
  L7_uchar8         pad;
  L7_uint32         partnerOperSysPri;
  L7_uint32         partnerOperKey;
  L7_uint32         partnerOperPortNumber;
  L7_uint32         partnerOperPortPri;
} dot3ad_port_checkpoint_data_t;

typedef struct
{
  L7_ushort16 tlvType;
  L7_ushort16 tlvLen;
  L7_uint32   index;
} dot3ad_nsf_tlv_header_t;


#define DOT3AD_LAG_PORT_ADD_TLV_LEN (sizeof(dot3ad_nsf_tlv_header_t) + \
                                   sizeof(dot3ad_port_checkpoint_data_t)) 
#define DOT3AD_LAG_PORT_DELETE_TLV_LEN (sizeof(dot3ad_nsf_tlv_header_t) + \
                                   sizeof(L7_uint32))


typedef struct dot3ad_agg_checkpoint_data_s
{

  /*ports actively participating in aggregation*/
  L7_uint32           aggId;            /* intIfNum of this logical interface */
  L7_uint32           aggActivePortList[L7_MAX_MEMBERS_PER_LAG];
  L7_uint32           activeNumMembers;

} dot3ad_agg_checkpoint_data_t;


typedef struct
{
  L7_ushort16 version;
  L7_ushort16 messageLen;

} dot3ad_ckpt_header_t;

                    /*
typedef struct
{
  L7_ushort16 objectID
  L7_ushort16 key;
  dot3ad_agg_checkpoint_data_t aggData;
}
                      */

extern dot3ad_agg_checkpoint_data_t *dot3adAggCheckPoint;
extern dot3ad_port_checkpoint_data_t *dot3adPortCheckPoint;


/*********************************************************************/
/*********************************************************************/



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
L7_RC_t dot3adNsfInit();

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
void dot3adManagerCheckpoint(L7_CKPT_MGR_CALLBACK_t  callback_type,
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
L7_RC_t dot3adCallCheckpointService(L7_uint32 intIfNum, DOT3AD_NSF_OPCODE_t opcode);

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
void dot3adBackupUnitCheckpoint(L7_CKPT_BACKUP_MGR_CALLBACK_t callback_type,
                                    L7_uchar8                  *checkpoint_data,
                                    L7_uint32                   data_len);
#endif /* __DOT3AD_NSF_H__*/
