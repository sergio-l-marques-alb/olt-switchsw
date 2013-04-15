
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  dai_cnfgr.h
*
* @purpose   Dynamic ARP Inspection configurator file
*
* @component Dynamic ARP Inspection
*
* @comments none
*
* @create 09/06/2007
*
* @author Kiran Kumar Kella
*
* @end
*             
**********************************************************************/

#ifndef DAI_CNFGR_H
#define DAI_CNFGR_H

#include "l7_cnfgr_api.h"


#define DAI_IS_READY (((daiCnfgrState == DAI_PHASE_INIT_3) || \
                       (daiCnfgrState == DAI_PHASE_EXECUTE) || \
                       (daiCnfgrState == DAI_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

typedef enum
{
  DAI_PHASE_INIT_0 = 0,
  DAI_PHASE_INIT_1,
  DAI_PHASE_INIT_2,
  DAI_PHASE_WMU,
  DAI_PHASE_INIT_3,
  DAI_PHASE_EXECUTE,
  DAI_PHASE_UNCONFIG_1,
  DAI_PHASE_UNCONFIG_2,
} daiCnfgrState_t;

void daiApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData);  
void daiCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t daiCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t * pResponse,
                                  L7_CNFGR_ERR_RC_t * pReason);                                   
L7_RC_t daiCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t * pResponse,
                                  L7_CNFGR_ERR_RC_t * pReason);                                   
L7_RC_t daiCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t * pResponse,
                                  L7_CNFGR_ERR_RC_t * pReason);                                   
void daiCnfgrFiniPhase1Process();                         
void daiCnfgrFiniPhase2Process();                         
void daiCnfgrFiniPhase3Process();                         
L7_RC_t daiCnfgrNoopProccess(L7_CNFGR_RESPONSE_t * pResponse,
                             L7_CNFGR_ERR_RC_t * pReason);
L7_RC_t daiCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t * pResponse,
                              L7_CNFGR_ERR_RC_t * pReason);


#endif /* DAI_CNFGR_H */

