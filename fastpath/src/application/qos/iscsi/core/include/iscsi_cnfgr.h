/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   iscsi_cnfgr.h
*
* @purpose    Contains prototypes to support the new configurator API
*
* @component  iSCSI
*
* @comments   none
*
* @create     04/18/2008
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_ISCSI_CNFGR_H
#define INCLUDE_ISCSI_CNFGR_H

#include "l7_cnfgr_api.h"

/****************************************
*
*  iSCSI Configurator Phase Data
*
*****************************************/

typedef enum 
{
  ISCSI_PHASE_INIT_0 = 0,
  ISCSI_PHASE_INIT_1,
  ISCSI_PHASE_INIT_2,
  ISCSI_PHASE_WMU,
  ISCSI_PHASE_INIT_3,
  ISCSI_PHASE_EXECUTE,
  ISCSI_PHASE_UNCONFIG_1,
  ISCSI_PHASE_UNCONFIG_2
} iscsiCnfgrState_t;

extern iscsiCnfgrState_t   iscsiCnfgrState_g;


#define ISCSI_IS_READY \
          ( ((iscsiCnfgrState_g == ISCSI_PHASE_INIT_3) ||    \
             (iscsiCnfgrState_g == ISCSI_PHASE_EXECUTE) ||   \
             (iscsiCnfgrState_g == ISCSI_PHASE_UNCONFIG_1))  \
            ? (L7_TRUE) : (L7_FALSE) )


/*********************************************************************
* @purpose  CNFGR System Initialization for ISCSI component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                 `           
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the iSCSI component.  This function is re-entrant.
*
* @end
*********************************************************************/
void iscsiApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

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
L7_RC_t iscsiCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t iscsiCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t iscsiCnfgrInitPhase3Process(L7_BOOL              warmRestart,
                                    L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason);

/*********************************************************************
* @purpose  This function undoes iscsiCnfgrInitPhase1Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void iscsiCnfgrFiniPhase1Process(void);

/*********************************************************************
* @purpose  This function undoes iscsiCnfgrInitPhase2Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void iscsiCnfgrFiniPhase2Process(void);

/*********************************************************************
* @purpose  This function undoes iscsiCnfgrInitPhase3Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void iscsiCnfgrFiniPhase3Process(void);

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
L7_RC_t iscsiCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t iscsiCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason);


#endif /*INCLUDE_ISCSI_CNFGR_H */
