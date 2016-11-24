/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cnfgr_sid.h
*
* @purpose    Configuraror component initialization header file that
*             contains the structurat initialization data for the
*             configurator.
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
#ifndef INCLUDE_CNFGR_SID_H
#define INCLUDE_CNFGR_SID_H

#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "osapi.h"

typedef struct CNFGR_COMPONENT_LIST_ENTRY
{
    L7_COMPONENT_IDS_t        cid;
    L7_CNFGR_COMPONENT_MODE_t mode;
    L7_VOIDFUNCPTR_t          pComponentCmdFunc;
    L7_RC_t                   (*enable_routine)(L7_uint32 mode);
} CNFGR_COMPONENT_LIST_ENTRY_t;


/* Component ID to component name mapping.
*/
typedef struct CNFGR_COMPONENT_NAME_LIST_ENTRY 
{
  L7_COMPONENT_IDS_t        cid;
#ifndef L7_PRODUCT_SMARTPATH
  L7_char8                  mnemonic[L7_COMPONENT_MNEMONIC_MAX_LEN];
  L7_char8                  name[L7_COMPONENT_NAME_MAX_LEN];
#else
  L7_char8                  *mnemonic;
  L7_char8                  *name;
#endif
} CNFGR_COMPONENT_NAME_LIST_ENTRY_t;

/*
***********************************************************************
*                          Function Prototypes
***********************************************************************
*/

/* ===================================================================
 *  Configurator General--
 * ===================================================================
*/

/*********************************************************************
* @purpose  Make sure all component mnemonic and name strings are terminated
*
* @param    void
*
* @returns  void
*
* @notes    This guards against any names defined in excess of the maximum
*           allowed length.
*
* @end
*********************************************************************/
void cnfgrSidComponentNameCleanup(void);

/*********************************************************************
* @purpose  Get options for semaphores
*
* @param    void    
*
* @returns  CNFGR_SEMAPHORE_OPTIONS 
*
* @notes    none 
*       
* @end
*********************************************************************/
#define CNFGR_SEMAPHORE_OPTIONS_f  cnfgrSidSemaphoreOptionsGet()
L7_int32 cnfgrSidSemaphoreOptionsGet();

/* ===================================================================
 *  Component Control (CCTRL)--
 * ===================================================================
*/

/*********************************************************************
* @purpose  Get maximum number of callback supported by the configurator.
*
* @param    void    
*
* @returns  CNFGR_CTRL_CALLBACK_MAX 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cnfgrSidCtrlCallbackMaxGet( void );


/* =================================================================== 
 * Message Handler --
 * ===================================================================
*/

/*********************************************************************
* @purpose  Get maximum number of Log Messages to issued.
*
* @param    void    
*
* @returns  CNFGR_MSG_LOG_MAX_COUNT 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cnfgrSidMsgLogMaxCountGet( void );

/* Message Queues */
/*********************************************************************
* @purpose  Get maximum number of noop to be processed.
*
* @param    void    
*
* @returns  CNFGR_MSG_NOOP_COUNT 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cnfgrSidMsgNoopCountGet( void );


/* Message Q1 - Receive messages when !EIP.
 *
 */
/*********************************************************************
* @purpose  Get size of Q1.
*
* @param    void    
*
* @returns  CNFGR_MSG_Q1_COUNT 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cnfgrSidMsgQ1CountGet( void );

/* Message Q2 - Receive messages when EIP.
 *
 */
/*********************************************************************
* @purpose  Get size of Q2.
*
* @param    void    
*
* @returns  CNFGR_MSG_Q2_COUNT 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cnfgrSidMsgQ2CountGet( void );

/* Message Handler Thread -
 *
*/
/*********************************************************************
* @purpose  Get thread priority value.
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_PRIO 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cnfgrSidMsgHandlerThreadPrioGet( void );

/*********************************************************************
* @purpose  Get thread priority value.
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cnfgrSidMsgHandlerThreadStackSizeGet( void );

/*********************************************************************
* @purpose  Get thread time slice value.
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_SLICE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cnfgrSidMsgHandlerThreadSliceGet( void );


/* Tally handle -  
 *
*/
/*********************************************************************
* @purpose  Get maximun number of handles.
*
* @param    void    
*
* @returns  CNFGR_TALLY_HANDLE_MAX 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cnfgrSidTallyHandleMaxGet( void );

/*********************************************************************
* @purpose  Get maximun number of response list entries.
*
* @param    void    
*
* @returns  CNFGR_TALLY_RSP_LIST_MAX 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cnfgrSidTallyRsplMaxGet( void );

/* Correlator pool -
 *
*/
/*********************************************************************
* @purpose  Get maximun number of correlators.
*
* @param    void    
*
* @returns  CNFGR_TALLY_CORRELATOR_MAX 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cnfgrSidTallyCorrelatorMaxGet( void );

/* Tally Watchdog Timer -  
 *
*/
/*********************************************************************
* @purpose  Get watchdog time value.
*
* @param    void    
*
* @returns  CNFGR_TALLY_WATCHDOG_TIME 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cnfgrSidTallyWatchdogTimeGet( void );


/*********************************************************************
* @purpose  Get watchdog timer retry coout value.
*
* @param    void    
*
* @returns  CNFGR_TALLY_WATCHDOG_RETRY 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cnfgrSidTallyWatchdogRetryGet( void );


/* =================================================================== 
 * Component Repository --
 * ===================================================================
*/

/*********************************************************************
* @purpose  Get component table maximum size value.
*
* @param    void    
*
* @returns  CNFGR_CR_COMPONENT_TABLE_END 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cnfgrSidCrComponentTableEndGet(void );


/*********************************************************************
* @purpose  Get component list address.
*
* @param    void    
*
* @returns  CNFGR_COMPONENT_LIST_ENTRY_t * 
*
* @notes    none 
*       
* @end
*********************************************************************/
CNFGR_COMPONENT_LIST_ENTRY_t * cnfgrSidCrComponentListAddrGet(void );

/*********************************************************************
* @purpose  Determine if specified component should be started even
*           when unit can't become manager.
*
* @param    component_id
*
* @returns  L7_TRUE - Component should be started.
*           L7_FALSE - Component should not be started.
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrSidNonMgrComponentCheck(L7_COMPONENT_IDS_t component_id);

/*********************************************************************
* @purpose  Determine component name for the specified component ID.
*
* @param    component_id
*
* @returns  component name - Name of the component, if component is found.
*           L7_NULLPTR - Component is not found.
*
* @notes    none
*
* @end
*********************************************************************/
L7_char8 *cnfgrSidComponentNameGet(L7_COMPONENT_IDS_t component_id);

/*********************************************************************
* @purpose  Determine component mnemonic for the specified component ID.
*
* @param    component_id
*
* @returns  component name - Name of the component, if component is found.
*           L7_NULLPTR - Component is not found.
*
* @notes    none
*
* @end
*********************************************************************/
L7_char8 *cnfgrSidComponentMnemonicGet(L7_COMPONENT_IDS_t component_id);

#endif /* INCLUDE_CNFGR_SID_H */
