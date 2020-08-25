/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cnfgr_ctrl.h
*
* @purpose    Configuraror component controller header file
*
* @component  cnfgr
*
* @comments   none
*
* @create     03/17/2003
*
* @author     avasquez
* @end
*
**********************************************************************/
#ifndef INCLUDE_CNFGR_CTRL_H
#define INCLUDE_CNFGR_CTRL_H


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/


/*
***********************************************************************
*                          Data Types 
***********************************************************************
*/


/* Enumerated values for internal event notifications to the configurator */
typedef enum CNFGR_CTLR_EVNT
{
    /* First event ---  range checking    */
    CNFGR_EVNT_FIRST            = 0,

    /* Component Initialization events    */
    CNFGR_EVNT_I_START          = 1,
    CNFGR_EVNT_I_PHASE1_CMPLT,
    CNFGR_EVNT_I_PHASE2_CMPLT,
    CNFGR_EVNT_I_MGMT_UNIT,
    CNFGR_EVNT_I_UNCONFIGURE_CMPLT,

    /* Component execution                */
    CNFGR_EVNT_E_START,

    /* Component Unconfigure events       */
    CNFGR_EVNT_U_START,
    CNFGR_EVNT_U_PHASE1_CMPLT, 

    /* Component Termination event        */
    CNFGR_EVNT_T_START,

    /* Component Suspend event            */
    CNFGR_EVNT_S_START,

    /* Component Resume event             */
    CNFGR_EVNT_R_START,

    /* Last event -- range checking       */
    CNFGR_EVNT_LAST
  
} CNFGR_CTLR_EVNT_t;


/* Event data structure */
typedef struct {

    CNFGR_CTLR_EVNT_t  event;
    L7_CNFGR_DATA_t    data;

} CNFGR_EVNT_DATA_t;


/* Command data Structure */

typedef struct {

    L7_CNFGR_CORRELATOR_t  correlator;
    L7_CNFGR_CB_HANDLE_t   cbHandle;
    L7_CNFGR_CMD_t         command;
    L7_CNFGR_CMD_TYPE_t    type;
    CNFGR_EVNT_DATA_t      evntData;       /* input  */ 

} CNFGR_CMD_DATA_t;

/*
***********************************************************************
*                          Global Data
***********************************************************************
*/



/*
***********************************************************************
*                          CCTlr Interface Function Prototypes
***********************************************************************
*/


/*********************************************************************
* @purpose  This function unconditionally terminates the CCtlr. All         
*           CCtlr resources are returned. This function is a CCTlr
*           interace. 
*
* @param    None.  
* 
* @returns  None.        
*
* @notes    None.                                                     
*       
* @end
*********************************************************************/
void cnfgrCCtlrFini(void);

/*********************************************************************
* @purpose  This function initializes the CCtlr. This function is a 
*           CCtlr interface.
*
* @param    None.                                                  
* 
* @returns  L7_SUCCESS - CCtlr initialized succesfully.                
* @returns  L7_ERROR   - CCtlr did not initialized succesfully.    
*
* @notes    None.                                                     
*       
* @end
*********************************************************************/
L7_RC_t cnfgrCCtlrInitialize(void);

/*********************************************************************
* @purpose  This function process command/events pairs. This function
*           is a CCtlr interface.
*
* @param    pCmdData  - @b{(input)}  Handle of the completed tally. 
*
* @returns  None.              
*
* @notes    None.                                                     
*       
* @end
*********************************************************************/
void cnfgrCCtlrProcessCommand(CNFGR_IN CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  This function is used by Tally to indicate that it has    
*           completed the tally operation. This is a CCtlr interface.
*
* @param    ctHandle  - @b{(input)}  Handle of the completed tally. 
* @param    nextState - @b{(input)}  Value to generate next message.
* @param    nextCid   - @b{(input)}  Value to generate next message.
* @param    pRspList  - @b{(input)}  list of responses received by tally.
*
* @returns  None.              
*
* @notes    None.                                                     
*       
* @end
*********************************************************************/
void cnfgrCCtlrTallyComplete( CNFGR_IN CNFGR_CT_HANDLE_t  ctHandle,
                              CNFGR_IN L7_CNFGR_STATE_t   nextState,
                              CNFGR_IN L7_COMPONENT_IDS_t nextCid,
                              CNFGR_IN CNFGR_RSP_LIST_t  *pRspList
                            );

/*********************************************************************
 * @purpose  Set whether the restart is warm or cold
 *
 * @param    warmRestart  - @b{(input)}  L7_TRUE if restart is warm
 *
 * @returns  None.              
 *
 * @notes    None.                                                     
 *       
 * @end
 *********************************************************************/
void cnfgrCCtlrWarmRestartSet(L7_BOOL warmRestart);


#endif /* INCLUDE_CNFGR_CTRL_H */
