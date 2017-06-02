/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename std_policy_cnfgr.h
*
* @purpose Contains prototypes to support the new configurator API
*
* @component 
*
* @comments
*
* @create 06/09/2003
*
* @author markl
* @end
*
**********************************************************************/


#ifndef INCLUDE_STD_POLICY_CNFGR_H
#define INCLUDE_STD_POLICY_CNFGR_H

/*********************************************************************
*
* @purpose  CNFGR System Initialization for Policy component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the policy comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void policyApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)

#endif /* INCLUDE_STD_POLICY_CNFGR_H */
