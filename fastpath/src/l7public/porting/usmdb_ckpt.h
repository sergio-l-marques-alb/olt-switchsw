/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
**********************************************************************
* @filename usmdb_ckpt.h
*
* @purpose USMDB declarations for checkpoint service
*
* @component checkpoint service
*
* @comments none
*
* @create 2/5/2009
*
* @author rrice
*
* @end
*             
**********************************************************************/

#ifndef USMDB_CKPT_H
#define USMDB_CKPT_H



/*********************************************************************
* @purpose  Determine whether the system is ready for a warm restart
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    L7_TRUE if all components that checkpoint have completed an
*           initial full checkpoint since the current backup manager was
*           elected.
*
* @end
*********************************************************************/
L7_BOOL usmDbCkptWarmRestartReady(void);

/*********************************************************************
* @purpose  Retrieve the checkpoint statistics
*
* @param    ckptStats   Checkpoint statistics structure
*
* @returns  L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t usmDbCkptStatsGet(CkptStats_t *ckptStats);

/*********************************************************************
* @purpose  Clear the checkpoint statistics
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t usmDbCkptStatsClear(void);


#endif
