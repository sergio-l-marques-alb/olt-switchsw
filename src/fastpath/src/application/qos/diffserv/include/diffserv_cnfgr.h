/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   diffserv_cnfgr.h
*
* @purpose    Contains prototypes to support the new configurator API
*
* @component  DiffServ
*
* @comments   none
*
* @create     06/22/2003
*
* @author     gpaussa
* @end
*
**********************************************************************/
#ifndef INCLUDE_DIFFSERV_CNFGR_H
#define INCLUDE_DIFFSERV_CNFGR_H

/****************************************
*
*  DiffServ Configurator Phase Data
*
*****************************************/

typedef enum 
{
  DIFFSERV_PHASE_INIT_0 = 0,
  DIFFSERV_PHASE_INIT_1,
  DIFFSERV_PHASE_INIT_2,
  DIFFSERV_PHASE_WMU,
  DIFFSERV_PHASE_INIT_3,
  DIFFSERV_PHASE_EXECUTE,
  DIFFSERV_PHASE_UNCONFIG_1,
  DIFFSERV_PHASE_UNCONFIG_2
} diffServCnfgrState_t;

extern diffServCnfgrState_t   diffServCnfgrState_g;


#define DIFFSERV_IS_READY \
        ( ((diffServCnfgrState_g == DIFFSERV_PHASE_INIT_3) ||   \
           (diffServCnfgrState_g == DIFFSERV_PHASE_EXECUTE) ||  \
           (diffServCnfgrState_g == DIFFSERV_PHASE_UNCONFIG_1)) \
          ? L7_TRUE : L7_FALSE                                  \
        )


/*********************************************************************
*
* @purpose  CNFGR System Initialization for Diffserv component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the diffserv comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void diffServApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

#endif /* INCLUDE_DIFFSERV_CNFGR_H */
