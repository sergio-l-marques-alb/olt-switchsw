/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   nim_cnfgr.h
*
* @purpose    The prototypes of functions that interface to the CNFGR component
*
* @component  NIM
*
* @comments   none
*
* @create     05/07/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/
#ifndef NIM_CNFGR_H
#define NIM_CNFGR_H

#include "nim_data.h"
#include "l7_cnfgr_api.h"

/*********************************************************************
*
* @purpose  CNFGR System Initialization for NIM component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the NIM.  This function is re-entrant.
*           The CNFGR request should be transfered to the NIM 
*           task as quickly as possible to avoid processing in the 
*           Configurator's task. 
*
* @end
*********************************************************************/
void nimApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair.
*
* @param    pCmdData  - @b{(input)}  command to be processed.       
*
* @returns  None
*
* @notes    This function completes synchronously. The return value 
*           is presented to the configurator by calling the cnfgrApiCallback().
*           The following are the possible return codes:   
*           L7_SUCCESS - There were no errors. Response is available.
*           L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_CB_ERR_RC_INVALID_RQST
*           L7_CNFGR_CB_ERR_RC_INVALID_CMD
*           L7_CNFGR_ERR_RC_INVALID_CMD_TYPE
*           L7_CNFGR_ERR_RC_INVALID_PAIR
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
void nimRecvCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData );

/*********************************************************************
* @purpose  phase 1 to Initialize Network Interface Manager component   
*
* @param    void
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t   nimPhaseOneInit();

/*********************************************************************
* @purpose  Remove all resources acquired during Phase 1
*
* @param    void
*
* @returns  void
*
* @notes    nimPhaseTwoFini and nimPhaseThreeFini must be called before
*           this function is called. Failure to keep the sequence will
*           result in memory leaks. 
*       
* @end
*********************************************************************/
void nimPhaseOneFini();

/*********************************************************************
* @purpose  phase 2 to Initialize Network Interface Manager component   
*
* @param    void
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t   nimPhaseTwoInit();

/*********************************************************************
* @purpose  Remove all resources acquired during Phase 2
*
* @param    void
*
* @returns  void
*
* @notes    nimPhaseThreeFini must be called before this is called.  
*           Failure to keep the sequence will result in memory leaks. 
*       
* @end
*********************************************************************/
void nimPhaseTwoFini();

/*********************************************************************
* @purpose  phase 3 to Initialize Network Interface Manager component   
*
* @param    void
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t   nimPhaseThreeInit();

/*********************************************************************
* @purpose  Remove all resources acquired during Phase 3
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
void nimPhaseThreeFini();

/*********************************************************************
* @purpose  Allocate all of the interface related data
*
* @param    none
*
* @returns  L7_RC_t  Returns L7_SUCCESS or L7_ERROR
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t nimIntfDataAlloc();

/*********************************************************************
* @purpose  DeAllocate all of the interface related data
*
* @param    none
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
void nimIntfDataDeAlloc();

/*********************************************************************
* @purpose  Initialize and start Card Manager Task function.
*
* @param    none
*
* @returns  L7_RC_t  Returns L7_SUCCESS or L7_ERROR
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t nimStartTask();

#endif /* NIM_CNFGR_H */
