/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename pml_cnfgr.h
*
* @purpose Contains prototypes to support the new configurator API
*
* @component
*
* @comments
*
* @create 05/21/2004
*
* @author colinw
* @end
*
**********************************************************************/

#ifndef INCLUDE_PML_CNFGR_H
#define INCLUDE_PML_CNFGR_H

#include "l7_cnfgr_api.h"
/*********************************************************************
*
* @purpose  CNFGR System Initialization for PML component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the pml comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void pmlApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);


#endif /* INCLUDE_PML_CNFGR_H */
