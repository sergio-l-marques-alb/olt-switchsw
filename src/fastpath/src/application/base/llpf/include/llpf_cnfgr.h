/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename llpf_cnfgr.h
*
* @purpose Contains prototypes and Data Structures
*          to support the Link Local Protocol Filtering application
*
* @component LLPF
*
* @comments
*
* @create 10/05/2009
*
* @author vijayanand K
* @end
*
**********************************************************************/
#ifndef INCLUDE_LLPF_CNFGR_H
#define INCLUDE_LLPF_CNFGR_H


#include "nimapi.h"
#include "default_cnfgr.h"
#include "l7_cnfgr_api.h"


typedef enum {
  LLPF_PHASE_INIT_0 =0,
  LLPF_PHASE_INIT_1,
  LLPF_PHASE_INIT_2,
  LLPF_PHASE_WMU,
  LLPF_PHASE_INIT_3,
  LLPF_PHASE_EXECUTE,
  LLPF_PHASE_UNCONFIG_1,
  LLPF_PHASE_UNCONFIG_2,
} llpfCnfgrState_t;

/* extern declarations */
extern llpfCnfgrState_t llpfCnfgrState;

#define LLPF_IS_READY (((llpfCnfgrState == LLPF_PHASE_INIT_3) || \
                            (llpfCnfgrState == LLPF_PHASE_EXECUTE) || \
                            (llpfCnfgrState == LLPF_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

/*********************************************************************
*
* @purpose  CNFGR System Initialization for LLPF component
*
* @param    *pCmdData    @b{(input)} Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the LLPF comp.  This function is re-entrant.
*
* @end
*********************************************************************/

extern void llpfApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData);
/*********************************************************************
* C@purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/

extern L7_RC_t llpfCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t * pResponse,
                                 L7_CNFGR_ERR_RC_t * pReason);
/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/

extern L7_RC_t llpfCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t * pResponse,
                                 L7_CNFGR_ERR_RC_t * pReason);
/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/

extern L7_RC_t llpfCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t * pResponse,
                                 L7_CNFGR_ERR_RC_t * pReason);

/*********************************************************************
* @purpose  This function undoes llpfCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/

extern void llpfCnfgrFiniPhase1Process();
/*********************************************************************
* @purpose  This function undoes llpfCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/

extern void llpfCnfgrFiniPhase2Process();
/*********************************************************************
* @purpose  This function undoes llpfCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/

extern void llpfCnfgrFiniPhase3Process();
/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.
*
* @param    pResponse - @b{(output)}  Response always command complete.
*
* @param    pReason   - @b{(output)}  Always 0
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           None.
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
extern L7_RC_t llpfCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/

extern L7_RC_t llpfCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t * pResponse,
                             L7_CNFGR_ERR_RC_t * pReason);

#endif  /* end of INCLUDE_LLPF_H */
