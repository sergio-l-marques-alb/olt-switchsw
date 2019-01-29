/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  pppoe_cnfgr.h
*
* @purpose   PPPoE configurator file
*
* @component PPPoE
*
* @comments none
*
* @create 9/05/2013
*
* @author Daniel Figueira
*
* @end
*             
**********************************************************************/

#ifndef PPPOE_CNFGR_H
#define PPPOE_CNFGR_H

#include "l7_cnfgr_api.h"


#define PPPOE_IS_READY (((pppoeCnfgrState == PPPOE_PHASE_INIT_3) || \
                        (pppoeCnfgrState == PPPOE_PHASE_EXECUTE) || \
                        (pppoeCnfgrState == PPPOE_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

typedef enum
{
  PPPOE_PHASE_INIT_0 = 0,
  PPPOE_PHASE_INIT_1,
  PPPOE_PHASE_INIT_2,
  PPPOE_PHASE_WMU,
  PPPOE_PHASE_INIT_3,
  PPPOE_PHASE_EXECUTE,
  PPPOE_PHASE_UNCONFIG_1,
  PPPOE_PHASE_UNCONFIG_2,
} pppoeCnfgrState_t;



void pppoeApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData);  
void pppoeCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t pppoeCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t * pResponse,
                                    L7_CNFGR_ERR_RC_t * pReason);                                   
L7_RC_t pppoeCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t * pResponse,
                                    L7_CNFGR_ERR_RC_t * pReason);                                   
L7_RC_t pppoeCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t * pResponse,
                                    L7_CNFGR_ERR_RC_t * pReason);                                   
void pppoeCnfgrFiniPhase1Process();                         
void pppoeCnfgrFiniPhase2Process();                         
void pppoeCnfgrFiniPhase3Process(); 
L7_RC_t pppoeCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t * pResponse,
                                L7_CNFGR_ERR_RC_t * pReason);
#endif /* PPPOE_CNFGR_H */
