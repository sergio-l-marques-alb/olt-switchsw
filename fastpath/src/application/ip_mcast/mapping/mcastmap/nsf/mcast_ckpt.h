/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
**********************************************************************
*
* @filename   mcast_ckpt.h
*
* @purpose    MCAST internal function prototypes for checkpoint functions
*
* @component  MCAST
*
* @comments   none
*
* @create     7/8/2009
*
* @end
*
**********************************************************************/


#ifndef INCLUDE_MCAST_CKPT_H
#define INCLUDE_MCAST_CKPT_H

#include "datatypes.h"

#define MCAST_CKPT_TABLE_SIZE     L7_MULTICAST_FIB_MAX_ENTRIES
#define MCAST_CKPT_AVL_TREE_TYPE  0x10

#define MCAST_CKPT_MESSAGE_TYPE_1 0x0001


#define MCAST_CKPT_FLAG_SYNC_PENDING   0x01
#define MCAST_CKPT_FLAG_DELETE_PENDING 0x02

typedef L7_uint32 mcastCkptFlags_t;

typedef enum
{
  MCAST_CKPT_TYPE_MFC_ENTRY = 1
} MCAST_CKPT_TYPE_t;

typedef enum
{
  MCAST_CKPT_ACTION_ADD = 1,
  MCAST_CKPT_ACTION_DELETE 
} MCAST_CKPT_ACTION_t;

typedef struct
{
  void               *mcastCkptPendingSema;
  void               *mcastCkptDbSema;

  L7_BOOL             mcastCkptBackupManagerElected;

  avlTree_t           mcastCkptAvlTree;
  L7_uint32           mcastCkptAvlTreeMaxEntries;
} mcastCkptInfo_t;

typedef struct
{
  L7_inet_addr_t       source;
  L7_inet_addr_t       group;
  L7_uint32            intIfNum;
  L7_uint32            rxPort;
  mcastCkptFlags_t     ckptFlags;
} mcastCkptRecord_t;

/*********************************************************************
* @purpose  Init MCAST checkpoint code.
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t mcastCkptInit();

/*********************************************************************
* @purpose  Fini MCAST checkpoint code.
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t mcastCkptFini();

/*********************************************************************
* @purpose  Indicates if a backup manager has been elected.
*
* @param 
*
* @returns L7_BOOL
*
* @notes  
*
* @end
*********************************************************************/
L7_BOOL mcastCkptBackupManagerIsElected();

/*********************************************************************
* @purpose  If a checkpoint is not already pending, start a checkpoint.
*
* @param    forceCheckpoint - If L7_TRUE, then force a checkpoint even
*                             if there is already a checkpoint pending.
*
* @returns L7_RC_t
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t mcastCkptStart(L7_BOOL forceCheckpoint);

/*********************************************************************
* @purpose  Take the semaphore for the MCAST ckpt DB
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void mcastCkptDbSemaTake();

/*********************************************************************
* @purpose  Give the semaphore for the MCAST ckpt DB
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void mcastCkptDbSemaGive();

/*********************************************************************
* @purpose  Register callback functions for both management and backup roles.
*
* @param  void
*
* @returns  L7_SUCCESS if callback registrations are successful
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t mcastCkptCallbacksRegister(void);

/*********************************************************************
* @purpose  Clear all checkpointed data.
*
* @param    void
*
* @returns  void
*
* @notes    Can be called when a new backup unit is reported. Can also
*           happen if we do a cold restart and need to throw away any
*           data that happened to be checkpointed.
*
* @end
*********************************************************************/
void mcastCkptFlush(L7_BOOL purge);

/*********************************************************************
* @purpose  Rebuild all checkpointed data.
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void mcastCkptRebuild();

/*********************************************************************
* @purpose  Indicate that a MCAST checkpoint is now pending.
*
* @param  void
*
* @returns  L7_TRUE if a MCAST checkpoint is already pending.
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL mcastCkptIsPending();

/*********************************************************************
* @purpose  Indicate that a MCAST checkpoint is not pending.
*
* @param 
*
* @returns 
*
* @notes  
*
* @end
*********************************************************************/
void mcastCkptIsNotPending();

/*********************************************************************
* @purpose Use the checkpoint service to sync mcast connection info.
*
* @param   L7_BOOL          checkpointImmediately 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mcastCkptEntrySync(L7_inet_addr_t *source,
                           L7_inet_addr_t *group,
                           L7_uint32       iif,
                           L7_uint32       rxPort,
                           L7_BOOL         checkpointImmediately);

/*********************************************************************
* @purpose Use the checkpoint service to sync mcast connection info.
*
* @param   L7_BOOL          checkpointImmediately 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mcastCkptEntryDelete(L7_inet_addr_t *source,
                             L7_inet_addr_t *group);


void mcastCkptWarmRestartTimerEvent(void);


#endif   /* INCLUDE_MCAST_CKPT_H */
