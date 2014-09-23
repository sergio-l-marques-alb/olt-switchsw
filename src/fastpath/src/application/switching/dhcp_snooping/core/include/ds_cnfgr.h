/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  ds_cnfgr.h
*
* @purpose   DHCP Snooping configurator file
*
* @component DHCP Snooping
*
* @comments none
*
* @create 3/16/2007
*
* @author Rob Rice (rrice)
*
* @end
*             
**********************************************************************/

#ifndef DHCP_SNOOPING_CNFGR_H
#define DHCP_SNOOPING_CNFGR_H

#include "l7_cnfgr_api.h"


#define DHCP_SNOOP_IS_READY (((dsCnfgrState == DHCP_SNOOP_PHASE_INIT_3) || \
                              (dsCnfgrState == DHCP_SNOOP_PHASE_EXECUTE) || \
                              (dsCnfgrState == DHCP_SNOOP_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

typedef enum
{
  DHCP_SNOOP_PHASE_INIT_0 = 0,
  DHCP_SNOOP_PHASE_INIT_1,
  DHCP_SNOOP_PHASE_INIT_2,
  DHCP_SNOOP_PHASE_WMU,
  DHCP_SNOOP_PHASE_INIT_3,
  DHCP_SNOOP_PHASE_EXECUTE,
  DHCP_SNOOP_PHASE_UNCONFIG_1,
  DHCP_SNOOP_PHASE_UNCONFIG_2,
} dsCnfgrState_t;



void dsApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData);  
void dsCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t dsCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t * pResponse,
                                 L7_CNFGR_ERR_RC_t * pReason);                                   
L7_RC_t dsCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t * pResponse,
                                 L7_CNFGR_ERR_RC_t * pReason);                                   
L7_RC_t dsCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t * pResponse,
                                 L7_CNFGR_ERR_RC_t * pReason);                                   
void dsCnfgrFiniPhase1Process();                         
void dsCnfgrFiniPhase2Process();                         
void dsCnfgrFiniPhase3Process(); 
L7_RC_t dsCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t * pResponse,
                             L7_CNFGR_ERR_RC_t * pReason);
#endif /* DHCP_SNOOPING_CNFGR_H */
