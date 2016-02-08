/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   voip_cnfgr.h
*
* @purpose    Contains prototypes to support the new configurator API
*
* @component  VOIP
*
* @comments   none
*
* @create     05/08/2007
*
* @author     parora
* @end
*
**********************************************************************/
#ifndef INCLUDE_VOIP_CNFGR_H
#define INCLUDE_VOIP_CNFGR_H

#include "l7_cnfgr_api.h"
         
/****************************************
*
*  VOIP Configurator Phase Data
*
*****************************************/

typedef enum 
{
  VOIP_PHASE_INIT_0 = 0,
  VOIP_PHASE_INIT_1,
  VOIP_PHASE_INIT_2,
  VOIP_PHASE_WMU,
  VOIP_PHASE_INIT_3,
  VOIP_PHASE_EXECUTE,
  VOIP_PHASE_UNCONFIG_1,
  VOIP_PHASE_UNCONFIG_2
} voipCnfgrState_t;

extern voipCnfgrState_t   voipCnfgrState_g;


#define VOIP_IS_READY \
          ( ((voipCnfgrState_g == VOIP_PHASE_INIT_3) ||    \
             (voipCnfgrState_g == VOIP_PHASE_EXECUTE) ||   \
             (voipCnfgrState_g == VOIP_PHASE_UNCONFIG_1))  \
            ? (L7_TRUE) : (L7_FALSE) )


/*********************************************************************
* @purpose  CNFGR System Initialization for VOIP component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                 `           
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the VOIP component.  This function is re-entrant.
*
* @end
*********************************************************************/
void voipApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1
*
* @param    pResponse   @b{(output)} Response (only if L7_SUCCESS)
* @param    pReason     @b{(output)} Reason (only if L7_ERROR)
*
* @returns  L7_SUCCESS  There were no errors (pResponse is available)
* @returns  L7_ERROR    There were errors (pReason code is available)
*
* @comments The following responses are valid:
*             L7_CNFGR_CMD_COMPLETE
*
* @comments The following error reason codes are valid:
*             L7_CNFGR_ERR_RC_FATAL
*             L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread, which MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t voipCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2
*
* @param    pResponse   @b{(output)} Response (only if L7_SUCCESS)
* @param    pReason     @b{(output)} Reason (only if L7_ERROR)
*
* @returns  L7_SUCCESS  There were no errors (pResponse is available)
* @returns  L7_ERROR    There were errors (pReason code is available)
*
* @comments The following responses are valid:
*             L7_CNFGR_CMD_COMPLETE
*
* @comments The following error reason codes are valid:
*             L7_CNFGR_ERR_RC_FATAL
*             L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread, which MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t voipCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason);
/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3
*
* @param    pResponse   @b{(output)} Response (only if L7_SUCCESS)
* @param    pReason     @b{(output)} Reason (only if L7_ERROR)
*
* @returns  L7_SUCCESS  There were no errors (pResponse is available)
* @returns  L7_ERROR    There were errors (pReason code is available)
*
* @comments The following responses are valid:
*             L7_CNFGR_CMD_COMPLETE
*
* @comments The following error reason codes are valid:
*             L7_CNFGR_ERR_RC_FATAL
*             L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread, which MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t voipCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason);
/*********************************************************************
* @purpose  This function undoes voipCnfgrInitPhase1Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void voipCnfgrFiniPhase1Process(void);

/*********************************************************************
* @purpose  This function undoes voipCnfgrInitPhase2Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void voipCnfgrFiniPhase2Process(void);

/*********************************************************************
* @purpose  This function undoes voipCnfgrInitPhase3Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void voipCnfgrFiniPhase3Process(void);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP
*
* @param    pResponse   @b{(output)} Response always command complete
* @param    pReason     @b{(output)} Always 0
*
* @returns  L7_SUCCESS  Always return this value (pResponse is available)
*
* @comments The following responses are valid:
*             L7_CNFGR_CMD_COMPLETE
*
* @comments The following error reason codes are valid:
*             (none used)
*
* @comments This function runs in the configurator's thread, which MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t voipCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
                             L7_CNFGR_ERR_RC_t   *pReason);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse   @b{(output)} Response (only if L7_SUCCESS)
* @param    pReason     @b{(output)} Reason (only if L7_ERROR)
*
* @returns  L7_SUCCESS  There were no errors (pResponse is available)
* @returns  L7_ERROR    There were errors (pReason code is available)
*
* @comments The following responses are valid:
*             L7_CNFGR_CMD_COMPLETE
*
* @comments The following error reason codes are valid:
*             L7_CNFGR_ERR_RC_FATAL
*
* @comments This function runs in the configurator's thread, which MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t voipCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason);


#endif /*INCLUDE_VOIP_CNFGR_H */
