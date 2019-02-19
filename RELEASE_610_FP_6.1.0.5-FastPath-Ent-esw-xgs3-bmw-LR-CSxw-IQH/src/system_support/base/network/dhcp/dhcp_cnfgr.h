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

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @returns  L7_SUCCESS - There were no errors.
*
* @end
*********************************************************************/
L7_RC_t dhcpCnfgrUconfigPhase2();
/*********************************************************************
* @purpose  This function Builds the default configuration for the DHCP Client
*
* @returns  L7_SUCCESS - There were no errors.
*
* @end
*********************************************************************/
L7_RC_t dhcpBuildDefaultConfig();

#endif /*DHCP_CNFGR_H_*/
