/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cnfgr_cr.h
*
* @purpose    Configuraror component repository header file
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
#ifndef INCLUDE_CNFGR_CR_H
#define INCLUDE_CNFGR_CR_H


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


typedef void * CNFGR_CR_HANDLE_t;

typedef struct {

    L7_COMPONENT_IDS_t          cid;
    void                       *pLock;
    L7_CNFGR_STATE_t            currentState,
                                nextState,  
                                saveState; 
    L7_CNFGR_CMD_t              cmd;
    L7_CNFGR_RQST_t             rqst;
    L7_CNFGR_ASYNC_RESPONSE_t   aRsp;
    L7_BOOL                     present;
    L7_CNFGR_COMPONENT_MODE_t   mode;
    L7_CNFGR_COMPONENT_STATUS_t status;
    L7_VOIDFUNCPTR_t            pCommandFunction;
    L7_char8                    *mnemonic;
    L7_char8                    *name;
    L7_uint32                   hwApplyPhases;

} CNFGR_CR_COMPONENT_t;

/*
***********************************************************************
*                          Global Data
***********************************************************************
*/



/*
***********************************************************************
*                          Function Prototypes
***********************************************************************
*/

/* Component Accessors */

/*********************************************************************
* @purpose  This function get the first component in CR.                
*           This is a CR interface function.
*
* @param    pCrHandle    - @b{(output)} location of unique identifier.
*                                       NULL if cid not found.     
*                                      
* @param    ppComponent  - @b{(output)} location of for the ComponentData
*                                       NULL if cid not found. 
*
* @returns  L7_SUCCESS   - function completed succesfully. Caller can update
*                          component data.
* @returns  L7_ERROR     - function failed. This COULD be a fatal error.
*
* @notes    Queries can be done to the while the component is locked.                     
*       
* @end
*********************************************************************/
L7_RC_t cnfgrCrComponentFirstTake( CNFGR_OUT CNFGR_CR_HANDLE_t *pCrHandle,
                                   CNFGR_OUT CNFGR_CR_COMPONENT_t **ppComponent );

/*********************************************************************
* @purpose  This function puts the component to the repository.
*           User can get it to update content. This is a CR interface
*           function.
*
* @param    crHandle     - @b{(input)} unique identifier for component
*                                      to allow read acces only.
*
* @returns  L7_SUCCESS   - function completed succesfully. 
* @returns  L7_ERROR     - function failed. This COULD be a fatal error.
*
* @notes    None.                
*       
* @end
*********************************************************************/
L7_RC_t cnfgrCrComponentGive ( CNFGR_IN CNFGR_CR_HANDLE_t crHandle );

/*********************************************************************
* @purpose  This function get the next component in CR.                
*           This is a CR interface function.
*
* @param    pCrHandle    - @b{(output)} location of unique identifier.
*                                       NULL if cid not found.     
*                                      
* @param    ppComponent  - @b{(output)} location of for the ComponentData
*                                       NULL if cid not found. 
*
* @returns  L7_SUCCESS   - function completed succesfully. Caller can update
*                          component data.
* @returns  L7_ERROR     - function failed. This COULD be a fatal error.
*
* @notes    Queries can be done to the while the component is locked.                     
*       
* @end
*********************************************************************/
L7_RC_t cnfgrCrComponentNextTake( CNFGR_INOUT CNFGR_CR_HANDLE_t *pCrHandle,
                                  CNFGR_OUT   CNFGR_CR_COMPONENT_t **ppComponent );

/*********************************************************************
* @purpose  This function gets a component from CR using CID key.       
*           This is a CR interface function.
*
* @param    cid          - @b{(input)}  Search key - component ID. 
*                                       
* @param    pCrHandle    - @b{(output)} location of unique identifier.
*                                       NULL if cid not found.     
*                                      
* @param    ppComponent  - @b{(output)} location of for the ComponentData
*                                       NULL if cid not found. 
*
* @returns  L7_SUCCESS   - function completed succesfully. Caller can update
*                          component data.
* @returns  L7_ERROR     - function failed. This COULD be a fatal error.
*
* @notes    Queries can be done to the while the component is locked.                     
*       
* @end
*********************************************************************/
L7_RC_t cnfgrCrComponentTake ( CNFGR_IN  L7_COMPONENT_IDS_t cid,
                               CNFGR_OUT CNFGR_CR_HANDLE_t *pCrHandle,
                               CNFGR_OUT CNFGR_CR_COMPONENT_t **ppComponent );

/*********************************************************************
* @purpose  This function will attempt to return all CR resources.      
*           This is a CR interface function.
*
* @param    None.                                                   
*                                       
*
* @returns  None.                                                    
*
* @notes    None.                     
*       
* @end
*********************************************************************/
void cnfgrCrFini(void);

/*********************************************************************
* @purpose  This function initializes CR  module. The use of this  
*           function is mandatory. This function is a CR interface.                          
*
* @param    None
*
* @returns  L7_SUCCESS - CR initialized successfully and it is         
*                        ready.
* @returns  L7_ERROR   - CR had problems and did not initialized.
*                        This is a fatal error.
*
*
* @notes    None                                              
*
*       
* @end
*********************************************************************/
L7_RC_t cnfgrCrInitialize(void);

#endif /* INCLUDE_CNFGR_CR_H */
