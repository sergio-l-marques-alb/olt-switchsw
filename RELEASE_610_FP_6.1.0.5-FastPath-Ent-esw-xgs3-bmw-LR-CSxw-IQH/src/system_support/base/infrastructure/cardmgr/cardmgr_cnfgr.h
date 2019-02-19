
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cardmgr_cnfgr.c
*
* @purpose    The functions that interface to the CNFGR component
*
* @component  CARDMGR
*
* @comments   none
*
* @create     065/22/2003
*
* @author     avasquez
* @end
*
**********************************************************************/



/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair.
*
* @param    cmpdu  - @b{(input)}  cardmgr pdu containing command to be processed.       
*
* @returns  None
*
* @notes    This function runs on the cardmgr task. This function completes a
*           configurator asynchronous command.  The return value 
*           is presented to the configurator by calling the cnfgrApiCallback().
*           The following are the possible return codes:   
*           L7_SUCCESS - There were no errors. Response is available.
*           L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_CB_ERR_RC_INVALID_RQST
*           L7_CNFGR_CB_ERR_RC_INVALID_CMD
*           L7_CNFGR_ERR_RC_INVALID_CMD_TYPE
*           L7_CNFGR_ERR_RC_INVALID_PAIR
*           L7_CNFGR_ERR_RC_FATAL
*
*       
* @end
*********************************************************************/
void cmgrCnfgrCmdReceive( cmgr_cmpdu_t *cmpdu );


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
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t cmgrCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason );


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
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t cmgrCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason
                                  );


L7_RC_t cmgrCnfgrExecuteProcess(L7_CNFGR_RESPONSE_t *pResponse, 
                                 L7_CNFGR_ERR_RC_t *pReason
                               );

L7_RC_t cmgrCnfgrUncfgPhase1Proccess(L7_CNFGR_RESPONSE_t *pResponse, 
                                     L7_CNFGR_ERR_RC_t *pReason
                                    );

L7_RC_t cmgrCnfgrUncfgPhase2Proccess(L7_CNFGR_RESPONSE_t *pResponse, 
                                     L7_CNFGR_ERR_RC_t *pReason
                                    );

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
*       
* @end
*********************************************************************/
L7_RC_t cmgrCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason
                             );

/*********************************************************************
* @purpose  Initialize and start Card Manager Task function.
*
* @param    L7_CNFGR_CMD_DATA_t *pCmdData
*
* @returns  L7_RC_t  Returns L7_SUCCESS or L7_ERROR.
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t cmgrStartTask(L7_CNFGR_CMD_DATA_t *pCmdData);

