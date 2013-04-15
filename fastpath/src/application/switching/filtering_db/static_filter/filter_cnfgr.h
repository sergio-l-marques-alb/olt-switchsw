/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename filter_cnfgr.h
*
* @purpose Contains prototypes to support the new configurator API
*
* @component 
*
* @comments
*
* @create 07/26/2003
*
* @author msmith
* @end
*
**********************************************************************/


#ifndef INCLUDE_FILTER_CNFGR_H
#define INCLUDE_FILTER_CNFGR_H

/*********************************************************************
*
* @purpose  CNFGR System Initialization for filter component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the filter comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void filterApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

#endif /* INCLUDE_FILTER_CNFGR_H */

