/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   cnfgr_hw_tally.h
*
* @purpose    Configuraror component hardware update tally header file
*
* @component  cnfgr
*
* @comments   none
*
* @create     11/21/2008
*
* @author     rrice
* @end
*
**********************************************************************/
#ifndef INCLUDE_CNFGR_HW_TALLY_H
#define INCLUDE_CNFGR_HW_TALLY_H


/*********************************************************************
* @purpose  Allocate resources needed to tally hardware update complete reports. 
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    Don't once at boot
*
*
* @end
*********************************************************************/
L7_RC_t cnfgrHwUpdateTallyInitialize(void);

/*********************************************************************
* @purpose  Initialize the pending mask for each hw reconciliation phase.
*           This has to be done whenever the system goes through p3 init. 
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    
*
*
* @end
*********************************************************************/
L7_RC_t cnfgrHwUpdatePendingMasksInit(void);

/*********************************************************************
* @purpose  Free resources.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    
*
*
* @end
*********************************************************************/
L7_RC_t cnfgrHwTallyFini(void);

#endif




