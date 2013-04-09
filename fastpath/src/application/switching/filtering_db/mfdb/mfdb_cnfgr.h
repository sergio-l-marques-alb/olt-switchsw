/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename mfdb_cnfgr.h
*
* @purpose Contains prototypes to support the new configurator API
*
* @component 
*
* @comments
*
* @create 07/25/2003
*
* @author msmith
* @end
*
**********************************************************************/


#ifndef INCLUDE_MFDB_CNFGR_H
#define INCLUDE_MFDB_CNFGR_H

/*********************************************************************
*
* @purpose  CNFGR System Initialization for mfdb component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the mfdb comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void mfdbApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

#endif /* INCLUDE_MFDB_CNFGR_H */

