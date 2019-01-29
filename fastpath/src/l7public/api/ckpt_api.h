/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   ckpt_api.h
*
* @purpose    Component APIs for the Checkpoint Service
*
* @component  ckpt
*
* @comments   
*
* @create     10/13/2008
*
* @author     bradyr
* @end
*
**********************************************************************/

#ifndef CKPT_API_H
#define CKPT_API_H

#include "l7_common.h"
#include "system_exports.h"


typedef enum
{
  CKPT_NEW_BACKUP_MANAGER = 1,
  CKPT_NEXT_ELEMENT,
  CKPT_BACKUP_MANAGER_GONE
} L7_CKPT_MGR_CALLBACK_t;

typedef enum
{
  CKPT_BACKUP_SYNC_DATA = 1,
  CKPT_BACKUP_NEW_MANAGER
} L7_CKPT_BACKUP_MGR_CALLBACK_t;


typedef void (*CkptMgrCallbackFcn)(L7_CKPT_MGR_CALLBACK_t  callback_type,
                                   L7_uchar8              *checkpoint_data,
                                   L7_uint32               max_data_len,
                                   L7_uint32              *ret_data_len,
                                   L7_BOOL                *more_data);

typedef void (*CkptBkupCallbackFcn)(L7_CKPT_BACKUP_MGR_CALLBACK_t callback_type,
                                    L7_uchar8                  *checkpoint_data,
                                    L7_uint32                   data_len);

/********************************************************************
*
* Data types used to construct & decode checkpoint messages.
*
********************************************************************/
typedef void (*CkptIeDataFcn)(void      *ie_data_ptr,
                              void      *dataPtr,
                              L7_uint32  dataLen);

typedef struct
{
  L7_ushort16   ckptReq; /* Flag, IE requires checkpoint */
  L7_ushort16   elemLen;
  CkptIeDataFcn readFcn;
  CkptIeDataFcn writeFcn;
  void         *dataAddr;
} CkptMsgTable_t;

typedef struct
{
  L7_ushort16  ie_id;
  L7_ushort16  ie_length;
  L7_uchar8    ie_payload[4];
} ckptMsgElem_t;

typedef struct
{
  L7_ushort16  message_type;
  L7_ushort16  message_length;
  ckptMsgElem_t ie1;
} CkptMsg_t;

/*********************************************************************
* @purpose  Registers manager's callback function with the checkpoint service
*
* @param    component   Component ID to register
* @param    mgrCallback Function pointer for checkpoint service to call
*                       on the management unit when checkpointing is required
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t ckptMgrCheckpointCallbackRegister(L7_COMPONENT_IDS_t component,
                                          CkptMgrCallbackFcn mgrCallback);

/*********************************************************************
* @purpose  Registers the backup manager's callback function with the 
*           checkpoint service
*
* @param    component  Component ID to register
* @param    bkupCllbck Function pointer for checkpoint service to call
*                      on the backup management unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t ckptBackupMgrCheckpointCallbackRegister(L7_COMPONENT_IDS_t component,
                                                CkptBkupCallbackFcn bkupCllbck);

/*********************************************************************
* @purpose  Make a data syncronization request to the checkpoint service
*
* @param    component   Component ID making the request
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t ckptMgrCheckpointStart(L7_COMPONENT_IDS_t component);

/*********************************************************************
* @purpose  Inform the checkpoint service of new backup manager election
*
* @param    newBackupMgrId   Unit ID of new backup manager
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments A backup manager ID of zero indicates it has been removed.
*
* @end
*********************************************************************/
L7_RC_t ckptBackupMgrElected(L7_uint32 newBackupMgrId);

/*********************************************************************
* @purpose  Query the checkpoint service to determine the initial
*           checkpoint has completed
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @end
*********************************************************************/
L7_BOOL ckptComplete(void);

/*********************************************************************
* @purpose  Query the checkpoint service to determine if the backup
*           manager's checkpoint has completed
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @end
*********************************************************************/
L7_BOOL ckptBackupComplete(void);

/*********************************************************************
* @purpose  Build up a checkpoint message given a message pointer & 
*           a checkpoint message table indicating which elements need
*           to be checkpointed
*
* @param    ckptMsgPtr  Pointer to the checkpoint message buffer
* @param    maxMsgLen   Maximum length of data buffer
* @param    ckptMsgTablePtr  Pointer to checkpont message table
* @param    maxElement  Last element in the checkpoint message table.
* @param    moreData    Pointer to more data flag.
*
* @returns  length of the constructed message
*
* @end
*********************************************************************/
L7_uint32 ckptMsgBuild(CkptMsg_t      *ckptMsgPtr,      L7_uint32 maxMsgLen,
                       CkptMsgTable_t *ckptMsgTablePtr, L7_uint32 maxElement,
                       L7_BOOL        *moreData);

/*********************************************************************
* @purpose  Parse a checkpoint message and store its contents given a
*           message pointer & a checkpoint message table describing the
*           elements & their write functions.
*
* @param    ckptMsgPtr  Pointer to the checkpoint message buffer
* @param    ckptMsgTablePtr  Pointer to checkpont message table
* @param    maxElement  Last element in the checkpoint message table.
*
* @returns  none
*
* @end
*********************************************************************/
void ckptMsgParse(CkptMsg_t      *ckptMsgPtr,
                  CkptMsgTable_t *ckptMsgTablePtr, L7_uint32 maxElement);

/*********************************************************************
* @purpose  Clear the checkpoint statistics
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t ckptStatsClear(void);

/*********************************************************************
* @purpose  Retrieve the checkpoint statistics
*
* @param    ckptStatsPtr   Pointer to checkpoint statistics structure
*
* @returns  L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t ckptStatsGet(CkptStats_t *ckptStatsPtr);

#endif /*CKPT_API_H*/

