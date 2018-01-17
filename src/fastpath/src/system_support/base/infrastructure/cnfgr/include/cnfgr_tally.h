/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cnfgr_tally.h
*
* @purpose    Configuraror component callback tally header file
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
#ifndef INCLUDE_CNFGR_TALLY_H
#define INCLUDE_CNFGR_TALLY_H


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

/* Tally handle -  
 *
*/

/* The response list resides in a handle.  It is
 * been used to communicate to the CCtrl correlator
 * received and status
 */
typedef struct CNFGR_RSP_LIST_ENTRY
{
    L7_COMPONENT_IDS_t        cid;
    L7_CNFGR_ASYNC_RESPONSE_t aRsp;

} CNFGR_RSP_LIST_ENTRY_t;

typedef struct CNFGR_RSP_LIST
{
    L7_uint32                 size;
    L7_uint32                 maxSize;
    L7_CNFGR_CORRELATOR_t     correlator;
    L7_CNFGR_CB_HANDLE_t      cbHandle;
    L7_CNFGR_ASYNC_RESPONSE_t aRsp;
    CNFGR_RSP_LIST_ENTRY_t   *pRlEntry;

} CNFGR_RSP_LIST_t;

typedef L7_uint32 CNFGR_CT_HANDLE_t;

/* Timer Data -
 *
*/
typedef struct CNFGR_TIMER_DATA
{
    L7_uint32   ctHandle;
    L7_int32    signature;

} CNFGR_TIMER_DATA_t;

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
* @purpose  This function adds an entry to the handle list and creates 
*           a correlator. This function is a Tally interface.
*
* @param    ctHandle     - @b{(input)}  The handle for the group of
*                                       completion callbacks -correlators.
*
* @param    cid          - @b{(input)}  The component identifier.
*       
* @param    pCorrellator - @b{(output)} The correlator
*
* @returns  L7_SUCCESS - Tally created and added a correlator to the         
*                        handle list.
* @returns  L7_ERROR   - Tally failed to create or add a correlator
*                        to the handle list. This COULD be a fatal error.
*
*
* @notes    The CCtlr MUST call this function to obtain a correlator
*           before issues the assynchronous request.
*  
* @notes    If the asynchronous call fails, the CCtlr MUST delete this
*           correlator or close the handle per error handling policies.
*
*       
* @end
*********************************************************************/
L7_RC_t cnfgrTallyAdd(
                      CNFGR_IN  CNFGR_CT_HANDLE_t      ctHandle, 
                      CNFGR_IN  L7_COMPONENT_IDS_t     cid,
                      CNFGR_OUT L7_CNFGR_CORRELATOR_t *pCorrelator
                    );

/*********************************************************************
* @purpose  This function tells Tally that CCtlr has completed a  
*           bath of asynchronous request and to issue a completion  
*           message to CCtlr when all completion callback associated 
*           with the handle are received. This function is a Tally interface.
*
* @param    ctHandle     - @b{(input)} The handle for the group of
*                                      completion callbacks -correlators.
* @param    nextState    - @b{(input)} value for next message
* @param    nextCid      - @b{(input}) value for next message
*
* @returns  void
*
* @notes    This function is intended to be used by the CCtlr when it
*           has finish issuing asynchronous requests. 
*
*       
* @end
*********************************************************************/
void cnfgrTallyAddComplete(CNFGR_IN CNFGR_CT_HANDLE_t ctHandle, 
                           CNFGR_IN L7_int32 nextState,
                           CNFGR_IN L7_COMPONENT_IDS_t nextCid);


/*********************************************************************
* @purpose  This function receives and processes completion callbacks.
*           This function is a Tally interface.
*
* @param    ctHandle   - @b{(input)} the handle index into the handle
*                                   list.                         
*
* @returns  void
*
*       
* @end
*********************************************************************/
void cnfgrTallyCallbackReceive( CNFGR_IN L7_CNFGR_CB_DATA_t *pCbData);

/*********************************************************************
* @purpose  This function releases a handle to the free handle pool.
*           This function is a Tally interface.  
*
* @param    ctHandle   - @b{(input)} the handle index into the handle
*                                   list.                         
*
* @returns  void
*
* @notes    This function is intended to be used by the a module authorized
*           to clean up the handle. 
*
* @notes    If there are correlators attached to this handle, they 
*           SHALL be released as well.
*
*       
* @end
*********************************************************************/
void cnfgrTallyClose(CNFGR_IN CNFGR_CT_HANDLE_t ctHandle);

/*********************************************************************
* @purpose  This function deletes an entry from the handle list. This 
*           function is a Tally interface.
*
* @param    correllator - @b{(input)} The correlator to be removed.
*
* @returns  L7_SUCCESS - Tally deleted the correlator from the         
*                        handle list.
* @returns  L7_ERROR   - Tally failed to delete the correlator
*                        from the handle list. This COULD be a fatal error.
*
*
* @notes    The CCtlr COULD issue this function call at any time if and
*           only if Tally is initialized and ready.
*       
* @end
*********************************************************************/
L7_RC_t cnfgrTallyDelete(CNFGR_IN L7_CNFGR_CORRELATOR_t  correlator);

/*********************************************************************
* @purpose  This function terminates tally module. The use of this  
*           function is optional. This function is a Tally interface.                          
*
* @param    None
*
* @returns  None.          
*
* @notes    None                                              
*
*       
* @end
*********************************************************************/
void cnfgrTallyFini(void);

/*********************************************************************
* @purpose  This function initializes tally module. The use of this  
*           function is mandatory. This function is a Tally interface.                          
*
* @param    None
*
* @returns  L7_SUCCESS - Tally initialized successfully and it is         
*                        ready to receive and tally callbacks.
* @returns  L7_ERROR   - Tally had problems and did not initialized.
*                        This is a fatal error.
*
*
* @notes    None                                              
*
*       
* @end
*********************************************************************/
L7_RC_t cnfgrTallyInitialize(void);

/*********************************************************************
* @purpose  This function creates a handle that will host one or more 
*           correlators (completion callbacks). This is a Tally 
*           interface.
*
* @param    cbHandle   - @b{(input)} handle to callers callback function.
*           correlator - @b{(input)} value provided by component.
* @returns  ctHandle   -  a non-zero value that represent a
*                         Tally handle. If value is zero
*                         no handle is available.
*
* @notes    This function can only be used after Tally has been
*           initialized successfully. 
*
*       
* @end
*********************************************************************/
CNFGR_CT_HANDLE_t cnfgrTallyOpen(CNFGR_IN L7_CNFGR_CORRELATOR_t correlator,
                                 CNFGR_IN L7_CNFGR_CB_HANDLE_t cbHandle
                                );

/*********************************************************************
* @purpose  This function handles the watchdog timer expired message. 
*           The use of this function is mandatory. This function is
*           Tally interface.
*
* @param    ctHandle  - @b{(input)} contains handle index (cthandle)
*                                   being timed.                    
* @param    signature - @b{(input)} timer signature for identification.
*                                                     
*
* @returns  void
*
* @notes    This function runs in the configurator thread and
*           handles the watchdog timer.
*
*       
* @end
*********************************************************************/
void cnfgrTallyWdTimerExpiredReceive(
                                     CNFGR_IN CNFGR_CT_HANDLE_t ctHandle, 
                                     CNFGR_IN L7_int32 signature
                                    );


#endif /* INCLUDE_CNFGR_TALLY_H */
