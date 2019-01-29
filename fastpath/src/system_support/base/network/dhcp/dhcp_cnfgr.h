/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008
*
**********************************************************************
* @filename  dhcp_cnfgr.h
*
* @purpose   dhcp configurator API
*
* @component DHCP client
*
* @comments
*
* @create    1/02/2008
*
* @author    ipopov
*
* @end
*
**********************************************************************/

#ifndef DHCP_CNFGR_H_
#define DHCP_CNFGR_H_

#include "datatypes.h"
#include "l7_cnfgr_api.h"

/**************************************************************************
* @purpose  DHCP Init Phase1 function.
*
* @comments None.
*
* @end
*************************************************************************/
L7_RC_t dhcpCnfgrInitPhase1Process();

/*********************************************************************
* @purpose  This function undoes dhcpCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpCnfgrFiniPhase1Process();

L7_RC_t dhcpCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *response,
                                   L7_CNFGR_ERR_RC_t *reason);
L7_RC_t dhcpCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *response,
                                   L7_CNFGR_ERR_RC_t *reason);
L7_RC_t dhcpCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *response,
                              L7_CNFGR_ERR_RC_t *reason);
L7_RC_t dhcpCnfgrUnconfigPhase1(L7_CNFGR_RESPONSE_t *response,
                                L7_CNFGR_ERR_RC_t *reason);
L7_RC_t dhcpCnfgrUnconfigPhase2(L7_CNFGR_RESPONSE_t *response,
                                L7_CNFGR_ERR_RC_t *reason);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @returns  L7_SUCCESS - There were no errors.
*
* @end
*********************************************************************/
L7_RC_t dhcpCnfgrUnconfigPhase2();

/*********************************************************************
* @purpose  This function Builds the default configuration for the DHCP Client
*
* @returns  L7_SUCCESS - There were no errors.
*
* @end
*********************************************************************/
L7_RC_t dhcpBuildDefaultConfig();

#endif /*DHCP_CNFGR_H_*/
