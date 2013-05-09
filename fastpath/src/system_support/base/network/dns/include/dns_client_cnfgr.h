/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dns_client_cnfgr.h
*
* @purpose Contains prototypes to support the new configurator API
*
* @component
*
* @comments
*
* @create 02/28/2005
*
* @author dfowler
* @end
*
**********************************************************************/

#ifndef INCLUDE_DNS_CLIENT_CNFGR_H
#define INCLUDE_DNS_CLIENT_CNFGR_H

#include "l7_cnfgr_api.h"
/*********************************************************************
*
* @purpose  CNFGR System Initialization for DNS client component
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
void dnsApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);


#endif /* INCLUDE_DNS_CLIENT_CNFGR_H */
