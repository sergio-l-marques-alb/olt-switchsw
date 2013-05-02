/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cnfgr_msg.h
*
* @purpose    Configuraror component message handler header file
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
#ifndef INCLUDE_CNFGR_MSG_H
#define INCLUDE_CNFGR_MSG_H


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
typedef enum CNFGR_MSG_TYPE {

    CNFGR_MSG_NOOP = 0,
    CNFGR_MSG_COMMAND,
    CNFGR_MSG_CALLBACK,
    CNFGR_MSG_TIMER

} CNFGR_MSG_TYPE_t;

/* Message Block 
 *
*/

typedef struct CNFGR_MSG_DATA {

    CNFGR_MSG_TYPE_t      msgType;
    union {

        CNFGR_CMD_DATA_t    cmdData;
        L7_CNFGR_CB_DATA_t  cbData;
        CNFGR_TIMER_DATA_t  tmrData;

    } u;

} CNFGR_MSG_DATA_t;

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

/*********************************************************************
* @purpose  This function indicates to the message handler that the         
*           current event has been processed and can take the next
*           event. This function is a Message Handler interface
*           function. 
*
* @param    None.  
* 
* @returns  None.        
*
* @notes    None.                                                     
*       
* @end
*********************************************************************/
void cnfgrMsgEventComplete(void);

/*********************************************************************
* @purpose  This function unconditionally terminates the Message Handler.         
*           All Message Handler resources are returned. This function
*           is a Message Handler interface function. 
*
* @param    None.  
* 
* @returns  None.        
*
* @notes    None.                                                     
*       
* @end
*********************************************************************/
void cnfgrMsgFini(void);

/*********************************************************************
* @purpose  This function handles messages to the component configurator.         
*           This function is a Message Handler interface function. 
*
* @param    None.  
* 
* @returns  None.        
*
* @notes    None.                                                     
*       
* @end
*********************************************************************/
void cnfgrMsgHandler(void);

/*********************************************************************
* @purpose  This function initializes the Message Handler. This function 
*           is a Message Handler interface.
*
* @param    None.                                                  
* 
* @returns  L7_SUCCESS - Message Handler initialized succesfully.                
* @returns  L7_ERROR   - Message Handler did not initialized succesfully.    
*
* @notes    None.                                                     
*       
* @end
*********************************************************************/
L7_RC_t cnfgrMsgInitialize(void);

/*********************************************************************
* @purpose  This function sends internal messages to the configurator.  
*           This function is a Message Handler interface.
*
* @param    pMsgData  - @b{(input)}  Message data to send.
* 
* @returns  L7_SUCCESS - Message sent succesfully.                               
* @returns  L7_ERROR   - Message was not sent successfully.    
*
* @notes    None.                                                     
*       
* @end
*********************************************************************/
L7_RC_t cnfgrMsgSend(CNFGR_IN CNFGR_MSG_DATA_t *pMsgData);


#endif /* INCLUDE_CNFGR_MSG_H */
