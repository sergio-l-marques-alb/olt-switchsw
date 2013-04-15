/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename lldp_cnfgr.h
*
* @purpose Contains prototypes to support the new configurator API
*
* @component
*
* @comments
*
* @create 02/01/2005
*
* @author dfowler
* @end
*
**********************************************************************/

#ifndef INCLUDE_LLDP_CNFGR_H
#define INCLUDE_LLDP_CNFGR_H

#include "l7_cnfgr_api.h"
/*********************************************************************
*
* @purpose  CNFGR System Initialization for 802.1AB component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the component.  This function is re-entrant.
*
* @end
*********************************************************************/
void lldpApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

#endif /* INCLUDE_LLDP_CNFGR_H */
