/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   cos_cnfgr.h
*
* @purpose    Contains prototypes to support the new configurator API
*
* @component  cos
*
* @comments   none
*
* @create     03/12/2004
*
* @author     gpaussa
* @end
*
**********************************************************************/
#ifndef INCLUDE_COS_CNFGR_H
#define INCLUDE_COS_CNFGR_H


/****************************************
*
*  COS Configurator Phase Data
*
*****************************************/

typedef enum 
{
  COS_PHASE_INIT_0 = 0,
  COS_PHASE_INIT_1,
  COS_PHASE_INIT_2,
  COS_PHASE_WMU,
  COS_PHASE_INIT_3,
  COS_PHASE_EXECUTE,
  COS_PHASE_UNCONFIG_1,
  COS_PHASE_UNCONFIG_2
} cosCnfgrState_t;

extern cosCnfgrState_t   cosCnfgrState_g;


#define COS_IS_READY \
          ( ((cosCnfgrState_g == COS_PHASE_INIT_3) ||    \
             (cosCnfgrState_g == COS_PHASE_EXECUTE) ||   \
             (cosCnfgrState_g == COS_PHASE_UNCONFIG_1))  \
            ? (L7_TRUE) : (L7_FALSE) )


/*********************************************************************
*
* @purpose  CNFGR System Initialization for COS component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the COS component.  This function is re-entrant.
*
* @end
*********************************************************************/
void cosApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);


#endif /* INCLUDE_COS_CNFGR_H */
