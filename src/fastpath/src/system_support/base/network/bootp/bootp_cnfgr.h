/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008
*
**********************************************************************
* @filename  bootp_cnfgr.h
*
* @purpose   bootp configurator API
*
* @component BOOTP client
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

#ifndef BOOTP_CNFGR_H_
#define BOOTP_CNFGR_H_

#include "datatypes.h"

/**************************************************************************
* @purpose  DHCP Init Phase1 function.
*
* @comments None.
*
* @end
*************************************************************************/
L7_RC_t bootpCnfgrInitPhase1Process();

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
void bootpCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @returns  L7_SUCCESS - There were no errors.
*
* @end
*********************************************************************/
L7_RC_t bootpCnfgrUconfigPhase2();

#endif /*BOOTP_CNFGR_H_*/
