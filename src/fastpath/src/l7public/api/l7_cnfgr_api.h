/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_cnfgr_api.h
*
* @purpose    Cnfgr component API functions Calls
*
* @component  cnfgr
*
* @comments   none
*
* @create     03/03/2003
*
* @author     avasquez
* @end
*
**********************************************************************/
/*********************************************************************
 *                   FASTPATH Release x.x.x
 ********************************************************************/


#ifndef L7_CNFGR_API_H
#define L7_CNFGR_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "l7_common.h"
#include "async_event_api.h"

#define L7_CNFGR_VER_1      0x1

#define CNFGR_IN   const
#define CNFGR_OUT
#define CNFGR_INOUT


/*
=================================================
=================================================
=================================================

   Basic Data Types

=================================================
=================================================
=================================================
*/

/* Enumerated values for configurator commands */
typedef enum L7_CNFGR_CMD_e
{
    L7_CNFGR_CMD_FIRST      = 0,    /* Range Checking */

    L7_CNFGR_CMD_INITIALIZE,
    L7_CNFGR_CMD_EXECUTE,
    L7_CNFGR_CMD_UNCONFIGURE,
    L7_CNFGR_CMD_TERMINATE,
    L7_CNFGR_CMD_SUSPEND,
    L7_CNFGR_CMD_RESUME,

    L7_CNFGR_CMD_LAST              /* Range Checking */

} L7_CNFGR_CMD_t;

/* Enumerated values for configurator command types */
typedef enum L7_CNFGR_CMD_TYPE_e
{
    L7_CNFGR_RQST = 1,
    L7_CNFGR_EVNT

} L7_CNFGR_CMD_TYPE_t;


/* Enumerated values for request to the component */
typedef enum L7_CNFGR_RQST_e
{
    /* First request  ---- range checking */
    L7_CNFGR_RQST_FIRST = 30,

    /* Component Initialization Requests. */
    L7_CNFGR_RQST_I_PHASE1_START,
    L7_CNFGR_RQST_I_PHASE2_START,
    L7_CNFGR_RQST_I_PHASE3_START,
    L7_CNFGR_RQST_I_WAIT_MGMT_UNIT,

    /* Component Execute Request          */
    L7_CNFGR_RQST_E_START,

    /* Component Unconfigure Request      */
    L7_CNFGR_RQST_U_PHASE1_START,
    L7_CNFGR_RQST_U_PHASE2_START,

    /* Component Termination Request      */
    L7_CNFGR_RQST_T_START,

    /* Component Suspend Request          */
    L7_CNFGR_RQST_S_START,

    /* Component Resume Request           */
    L7_CNFGR_RQST_R_START,

    /* Last Request --- Range checking    */
    L7_CNFGR_RQST_LAST

} L7_CNFGR_RQST_t;

/* Enumerated values for external event notifications to the configurator */
typedef enum L7_CNFGR_EVNT_e
{
    /* First Event -- range checking      */
    L7_CNFGR_EVNT_FIRST = 50,

    /* Component Initialization events    */
    L7_CNFGR_EVNT_I_START,
    L7_CNFGR_EVNT_I_MGMT_UNIT,

    /* Component Unconfigure events       */
    L7_CNFGR_EVNT_U_START,
    L7_CNFGR_EVNT_U_PHASE1_CMPLT,

    /* Component Termination event        */
    L7_CNFGR_EVNT_T_START,

    /* Component Suspend event            */
    L7_CNFGR_EVNT_S_START,

    /* Component Resume event             */
    L7_CNFGR_EVNT_R_START,

    /* Last Event --Range checking        */
    L7_CNFGR_EVNT_LAST

} L7_CNFGR_EVNT_t;


/* Enumerated values for error reason code      */
typedef enum L7_CNFGR_ERR_RC_e
{
    L7_CNFGR_ERR_RC_FIRST = 80,    /* Range checking */

    L7_CNFGR_ERR_RC_FATAL,
    L7_CNFGR_ERR_RC_LACK_OF_RESOURCES,
    L7_CNFGR_ERR_RC_NOT_AVAILABLE,
    L7_CNFGR_ERR_RC_NOT_FOUND,
    L7_CNFGR_ERR_RC_BUSY,
    L7_CNFGR_ERR_RC_IGNORED,
    L7_CNFGR_ERR_RC_INTERNAL,
    L7_CNFGR_ERR_RC_INVALID_ID,
    L7_CNFGR_ERR_RC_INVALID_DATA,
    L7_CNFGR_ERR_RC_INVALID_RSQT,
    L7_CNFGR_ERR_RC_INVALID_EVNT,
    L7_CNFGR_ERR_RC_INVALID_HANDLE,
    L7_CNFGR_ERR_RC_INVALID_CMD,
    L7_CNFGR_ERR_RC_INVALID_CMD_TYPE,
    L7_CNFGR_ERR_RC_INVALID_PAIR,
    L7_CNFGR_ERR_RC_OUT_OF_SEQUENCE,

    L7_CNFGR_ERR_RC_LAST           /* Range checking */

} L7_CNFGR_ERR_RC_t;

/* Enumerated values for configurator reponse              */
/* Valid if callback return code is L7_CNFGR_CB_RC_SUCCESS */
typedef enum CNFGR_RESPONSE_e
{
    L7_CNFGR_CMD_COMPLETE = 111,
    L7_CNFGR_INIT_COMPLETE,
    L7_CNFGR_INVALID_RESPONSE

} L7_CNFGR_RESPONSE_t;

/* Configuration Data definition */
typedef L7_uint32 L7_CNFGR_DATA_t;


/* Callback Handle and correlator definitions */
typedef L7_uint32 L7_CNFGR_CORRELATOR_t,
                  L7_CNFGR_CB_HANDLE_t;

#define L7_CNFGR_NO_HANDLE  (L7_CNFGR_CB_HANDLE_t)L7_NULL

/* Normalized component states */
typedef enum L7_CNGFR_STATE_e
{
    /* Range checking           */
    L7_CNFGR_STATE_NULL = 150,

    L7_CNFGR_STATE_IDLE,

    /* Initializing             */
    L7_CNFGR_STATE_P1,
    L7_CNFGR_STATE_P2,
    L7_CNFGR_STATE_P3,
    L7_CNFGR_STATE_WMU,

    /* Executing                */
    L7_CNFGR_STATE_E,

    /* Unconfiguring            */
    L7_CNFGR_STATE_U1,
    L7_CNFGR_STATE_U2,

    /* Terminated               */
    L7_CNFGR_STATE_T,

    /* Suspended                */
    L7_CNFGR_STATE_S,

    /* Resuming                 */
    L7_CNFGR_STATE_R,

    /* Range checking           */
    L7_CNFGR_STATE_LAST

} L7_CNFGR_STATE_t;

/* Access definitions */

typedef enum L7_CNFGR_QUERY_RQST_e
{
    L7_CNFGR_QRY_RQST_FIRST  = 180,

    L7_CNFGR_QRY_RQST_STATE,
    L7_CNFGR_QRY_RQST_PRESENT,
    L7_CNFGR_QRY_RQST_ALL_PRESENT,
    L7_CNFGR_QRY_RQST_FEATURE,
    L7_CNFGR_QRY_RQST_MODE,
    L7_CNFGR_QRY_RQST_STATUS,
    L7_CNFGR_QRY_RQST_CMDRQST,
    L7_CNFGR_QRY_RQST_MNEMONIC,
    L7_CNFGR_QRY_RQST_NAME,
    L7_CNFGR_QRY_RQST_HW_APPLY_PHASES,

    L7_CNFGR_QRY_RQST_LAST

} L7_CNFGR_QUERY_RQST_t;

typedef enum L7_CNFGR_QUERY_TYPE_e
{
  L7_CNFGR_QRY_TYPE_SINGLE  = 195,
  L7_CNFGR_QRY_TYPE_LIST
} L7_CNFGR_QUERY_TYPE_t;

typedef enum L7_CNFGR_COMPONENT_MODE_e
{
    L7_CNFGR_COMPONENT_ENABLE  = 201,
    L7_CNFGR_COMPONENT_DISABLE

} L7_CNFGR_COMPONENT_MODE_t;

typedef enum L7_CNFGR_COMPONENT_STATUS_e
{
    L7_CNFGR_COMPONENT_ACTIVE  = 251,
    L7_CNFGR_COMPONENT_INACTIVE

} L7_CNFGR_COMPONENT_STATUS_t;

/* Definition of the hardware reconciliation phases */
typedef enum L7_CNFGR_HW_APPLY_e
{
	L7_CNFGR_HW_APPLY_CONFIG = 1,
	L7_CNFGR_HW_APPLY_L3 = 2,
	L7_CNFGR_HW_APPLY_IPMCAST = 4,

  /* Number of reconciliation phases */
  L7_CNFGR_HW_APPLY_NUM_PHASES = 3

} L7_CNFGR_HW_APPLY_t;

/* Initialization request flags sent to components. 
 * pCmdData->u.rqstData.data */
typedef enum L7_CNFGR_REQUEST_FLAGS_e
{
  /* When this bit is on a p3 init indication, the restart is warm. When the 
   * bit is not set, the restart is cold. */
  L7_CNFGR_RESTART_WARM = 1,

} L7_CNFGR_RESTART_TYPE_t;

/*
=================================================
=================================================
=================================================

   CONFIGURATOR INITIALIZATION DATA

=================================================
=================================================
=================================================
*/
typedef L7_uint32 L7_CNFGR_INIT_DATA_t;



/*
=================================================
=================================================
=================================================

   COMMAND FROM/TO THE CONFIGURATOR

=================================================
=================================================
=================================================
*/

/* Request data structure */
typedef struct {

    L7_CNFGR_RQST_t request;
    L7_CNFGR_DATA_t data;

} L7_CNFGR_RQST_DATA_t;


/* Event data structure */
typedef struct {

    L7_CNFGR_EVNT_t event;
    L7_CNFGR_DATA_t data;

} L7_CNFGR_EVNT_DATA_t;

/* Command from (output)/to(input) the configurator structure  */
typedef struct {

    L7_CNFGR_CORRELATOR_t  correlator;
    L7_CNFGR_CB_HANDLE_t   cbHandle;
    L7_CNFGR_CMD_t         command;
    L7_CNFGR_CMD_TYPE_t    type;
    union{

        L7_CNFGR_RQST_DATA_t rqstData;       /* output */
        L7_CNFGR_EVNT_DATA_t evntData;       /* input  */

    }u;

} L7_CNFGR_CMD_DATA_t;


/*
=================================================
=================================================
=================================================

   COMPLETION CALLBACKS

=================================================
=================================================
=================================================
*/

/* Asynchronous Response Structure */
typedef struct {

    L7_RC_t                     rc;
    union {
        L7_CNFGR_ERR_RC_t       reason;
        L7_CNFGR_RESPONSE_t     response;
    }u;

} L7_CNFGR_ASYNC_RESPONSE_t;

/* Callback data structure        */
typedef struct {

    L7_CNFGR_CORRELATOR_t       correlator;
    L7_CNFGR_ASYNC_RESPONSE_t   asyncResponse;

} L7_CNFGR_CB_DATA_t;


/*
=================================================
=================================================
=================================================

   CONFIGURATOR QUERY DATA

=================================================
=================================================
=================================================
*/
typedef struct {

    L7_CNFGR_QUERY_TYPE_t type;                 /* input */
    union {
      L7_COMPONENT_IDS_t    cid;                /* input */
      L7_uint32             *cid_list;          /* output */
    } args;
    L7_CNFGR_QUERY_RQST_t request;              /* input */
    L7_CNFGR_ERR_RC_t     reason;               /* ouput */
    union {
        struct {
            L7_CNFGR_STATE_t current;
            L7_CNFGR_STATE_t next;
            L7_CNFGR_STATE_t saved;
        } state;
        L7_BOOL                     present;
        L7_CNFGR_COMPONENT_MODE_t   mode;
        L7_CNFGR_COMPONENT_STATUS_t status;
        struct {
            L7_CNFGR_CMD_t            cmd;
            L7_CNFGR_RQST_t           rqst;
            L7_CNFGR_ASYNC_RESPONSE_t aRsp;
        } cmdRqst;
        L7_char8                    *name;
        L7_uint32                    hwApplyPhases;
    } data;                                     /* output */

} L7_CNFGR_QUERY_DATA_t;

/*
=================================================
=================================================
=================================================

   CONFIGURATOR INTERFACE

=================================================
=================================================
=================================================
*/

/*
 ********************************************************************
 *   INITIALIZATION AND CLEANUP FUNCTIONS
 ********************************************************************
*/

/*********************************************************************
* @purpose  Initialize the configurator
*
* @param    pInitData     @b{(input)} Pointer to the configuratorn
*                                     initialization Data.
*
* @returns  L7_SUCCESS    The configurator initialized and is ready to
*                         accept commands.
* @returns  L7_ERROR      The configurator failed to initialized.
*
* @notes    NOTE: This function only initializes the configurator. The
*           Components are not initialized at the completion of this
*           function.
*
* @end
*********************************************************************/
L7_RC_t cnfgrApiInit (CNFGR_IN  L7_CNFGR_INIT_DATA_t *pInitData );


/*********************************************************************
* @purpose  Terminate the configurator and all components controlled by it.
*
* @param    NONE
*
* @returns  L7_SUCCESS    Configurator has susscefully terminated all software
*                         components, including itself.
* @returns  L7_ERROR      Configurator could not terminate one or more
*                         components or itself. Fatal System Error.
*
* @notes    If return value is L7_ERROR the caller MUST assume that there
*           are resources allocated and components running. The configurator
*           is in a state that the only function call can receive is
*           cnfgrApiFini. No component in the system SHOULD be trusted.
*
* @end
*********************************************************************/
L7_RC_t cnfgrApiFini(void);

/*
 ********************************************************************
 *   REGISTRATION AND DEREGISTRATION FUNCTIONS
 ********************************************************************
*/

/*********************************************************************
* @purpose  This function is used by the caller to register callback
*           function. The use of this function is optional.
*
* @param    pCbFcnt   - @b{(input)}  The function pointer for the
*                                    configurator to callback.
* @param    pCbHandle - @b{(output)} Opaque value known to the
*                                    configurator. The caller must
*                                    provide this parameter if wants
*                                    configurator to callback.
*
* @returns  L7_SUCCESS - Configurator succesfully register the callback
*                        function. pCBHandle is valid.
* @returns  L7_ERROR   - Configurator failed to register the callback
*                        function. pCBHandle is L7_CNFGR_NO_HANDLE
*
* @notes    The configurator registers the pCbFcnt callback function,
*           returning a handle via pCbHandle location. If the pCBFcnt
*           is L7_CNFGR_NO_HANDLE, the configurator returns L7_ERROR.
*
* @notes    This function is synchronous, therefore it will return to
*           the caller once this function has completed its work.
*
* @end
*********************************************************************/
L7_RC_t cnfgrApiRegister(
                          CNFGR_IN  L7_VOIDFUNCPTR_t      pCbFcnt,
                          CNFGR_OUT L7_CNFGR_CB_HANDLE_t *pCbHandle
                        );

/*********************************************************************
* @purpose  This function deregisters a callback function. The use of
*           this function is optional.
*
* @param    cbHandle - @b{(input)} Indicates which callback function
*                                  SHALL be no longer available.
*
* @returns  void
*
* @notes    The configurator will try to stop any pending callbacks. The
*           callback function might be called after the deregistration
*           has been invoked, but the configurator will guarantee that
*           the callback function is not called after the deregister
*           function has returned.
*
* @notes    This function is synchronous, therefore it will return to
*           the caller once this function has completed its work.
*
* @end
*********************************************************************/
void  cnfgrApiDeregister(CNFGR_IN L7_CNFGR_CB_HANDLE_t cbHandle);


/*
 ********************************************************************
 *   COMPLETION CALLBACK FUNCTION
 ********************************************************************
*/

/*********************************************************************
* @purpose  This function handles request to components asycnchronous
*           responses. The use of this function is mandatory.
*
* @param    pCbData    - @b{(input)} the completion response from the
*                                   component to the configurator.
*
* @returns  void
*
* @notes    This function is intended to be used by the component to
*           respond to a request issued by the configurator.
*           Registration to the component is not necessary.
*
*
* @end
*********************************************************************/
void cnfgrApiCallback( CNFGR_IN L7_CNFGR_CB_DATA_t *pCbData );

/*********************************************************************
* @purpose  API that components call to indicate they have finished their
*           hardware updates following a system restart.
*
* @param    cid    @b{(input)}  caller's component ID
* @param    phase  @b{(input)}  hardware reconciliation phase
*
* @returns  void
*
* @notes    This function is intended to be used by the component to
*           respond to a request issued by the configurator.
*           Registration to the component is not necessary.
*
*
* @end
*********************************************************************/
L7_RC_t cnfgrApiComponentHwUpdateDone(L7_COMPONENT_IDS_t cid,                          
                                      L7_CNFGR_HW_APPLY_t phase);


/*
 ********************************************************************
 *   CONFIGURATOR CONTROL FUNCTIONS
 ********************************************************************
*/
/*********************************************************************
* @purpose  This function is used by the component to issue an event
*           to the configurator.The use of this function is mandatory.
*
* @param    pCmdData - @b{(input)} pointer to the data containing event
*                                  information.
*
* @returns  L7_SUCCESS - The configurator schedules the command for
*                        processing. Operation is in progress.
* @returns  L7_ERROR   - The configurator could not schedule the
*                        command.
*
* @notes    This function completes asynchronously. If the component
*           has registered a completion callback, the following are
*           the possible return code.
*           L7_CNFGR_CB_RC_SUCCESS - There were no errors. Response
*                                    is available:
*           L7_CNFGR_CB_RC_ERROR   - There were errors. Reason code
*                                    is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_CB_ERR_RC_FATAL
*           L7_CNFGR_CB_ERR_RC_LACK_OF_RESOURCES
*           L7_CNFGR_CB_ERR_RC_INVALID_EVNT
*           L7_CNFGR_CB_ERR_RC_INVALID_CMD
*           L7_CNFGR_CB_ERR_RC_INVALID_HANDLE
*           L7_CNFGR_CB_ERR_RC_OUT_OF_SEQUENCE
*
* @end
*********************************************************************/
L7_RC_t cnfgrApiCommand( CNFGR_IN  L7_CNFGR_CMD_DATA_t *pCmdData );

/*********************************************************************
* @purpose  This function is used by the component to issue a delayed event
*           to the configurator.
*
* @param    pCmdData     - @b{(input)} pointer to the data containing event
*                                  information.
* @param    milliseconds - @b{(input)} number of milliseconds to delay before
                                   issuing the command
*
* @returns  L7_SUCCESS - The configurator schedules the command for
*                        processing. Operation is in progress.
* @returns  L7_ERROR   - The configurator could not schedule the
*                        command.
*
* @notes    This function completes asynchronously. If the component
*           has registered a completion callback, the following are
*           the possible return code.
*           L7_CNFGR_CB_RC_SUCCESS - There were no errors. Response
*                                    is available:
*           L7_CNFGR_CB_RC_ERROR   - There were errors. Reason code
*                                    is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_CB_ERR_RC_FATAL
*           L7_CNFGR_CB_ERR_RC_LACK_OF_RESOURCES
*           L7_CNFGR_CB_ERR_RC_INVALID_EVNT
*           L7_CNFGR_CB_ERR_RC_INVALID_CMD
*           L7_CNFGR_CB_ERR_RC_INVALID_HANDLE
*           L7_CNFGR_CB_ERR_RC_OUT_OF_SEQUENCE
*
* @end
*********************************************************************/
L7_RC_t cnfgrApiScheduleCommand(
                                 CNFGR_IN  L7_CNFGR_CMD_DATA_t *pCmdData,
                                 CNFGR_IN  L7_uint32           milliseconds
                               );


/*
 ********************************************************************
 *   CONFIGURATOR ACCESS FUNCTIONS
 ********************************************************************
*/
/*********************************************************************
* @purpose  This function returns the current state of all components.
*           The use of this function is optional.
*
* @param    currentState - @b{(output)} Storage for current state.
*
* @returns  void
*
* @notes    This function is synchronous, therefore it will return to
*           the caller once this function has completed its work.
*
* @end
*********************************************************************/
L7_RC_t cnfgrApiSystemStateGet(CNFGR_OUT L7_CNFGR_STATE_t *currentState);

/*********************************************************************
* @purpose  This function provides information about a component.
*           The use of this function is optional.
*
* @param    pQueryData   - @b{(output)}pointer to data where the
*                                      component will place
*                                      the requested information.
*
* @returns  L7_SUCCESS   - function completed succesfully. pQueryData
*                          contains valid information
* @returns  L7_ERROR     - function failed. pQueryData contain reason
*                        - for failure.
*
* @notes    This function excecute synchronously.
*
* @end
*********************************************************************/
L7_RC_t cnfgrApiQuery( CNFGR_OUT L7_CNFGR_QUERY_DATA_t *pQueryData );

/*********************************************************************
* @purpose  Get component name for the specified component ID.
*
* @param    cid   - Component ID.
* @param    comp_name   - Component name.
*
* @returns  L7_SUCCESS   - function completed succesfully.
* @returns  L7_FAILURE   - Component not found.
*
* @notes    This function excecute synchronously.
*
* @end
*********************************************************************/
L7_RC_t cnfgrApiComponentNameGet( L7_COMPONENT_IDS_t cid, L7_char8 *comp_name);

/*********************************************************************
* @purpose  Get component mnemonic for the specified component ID.
*
* @param    cid   - Component ID.
* @param    comp_mnemonic   - Component mnemonic.
*
* @returns  L7_SUCCESS   - function completed succesfully.
* @returns  L7_FAILURE   - Component not found.
*
* @notes    This function excecute synchronously.
*
* @end
*********************************************************************/
L7_RC_t cnfgrApiComponentMnemonicGet( L7_COMPONENT_IDS_t cid, L7_char8 *comp_mnemonic);

/*********************************************************************
* @purpose  Set the reason for starting the management functionality
*           (move to P3)
*           
*
* @param    systemStartupReason {(input)} System startup reason
*
* @returns  void
*
* @notes    This function excecutes synchronously.
*
* @end
*********************************************************************/
void cnfgrApiSystemStartupReasonSet(L7_LAST_STARTUP_REASON_t systemStartupReason);


/*********************************************************************
* @purpose  Get the Cnfgr Hw Tally timeout value for the specified phase
*
*
* @param    phase    {(input)}   Hw Tally phase
* @param    timeout  {(output)}  Timeout val (secs)
*
* @returns  L7_RC_t
*
* @notes    
*
*
* @end
*********************************************************************/
L7_RC_t cnfgrHwTallyTimeoutGet(L7_CNFGR_HW_APPLY_t phase, L7_uint32 *timeout);

#ifdef __cplusplus
}
#endif

#endif /* L7_CNFGR_API_H */
