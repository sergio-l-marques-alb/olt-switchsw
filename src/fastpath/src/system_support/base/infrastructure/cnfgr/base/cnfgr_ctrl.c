/*********************************************************************
 *                                                                 
 * *****  **   ***** **      Code classified LVL7 Confidential 
 *
 **********************************************************************
 *
 * Name: cnfgr_ctrl.c
 *
 * Purpose: Configurator component controller functions.
 *
 * Component: Configurator (cnfgr)
 *
 * Comments:  The following is the function catalog for Tally:
 *
 *            1. Configurator Interface (prototype in cnfgr_api.h)
 *
 *               - cnfgrApiCommand
 *               - cnfgrApiDeregister
 *               - cnfgrApiRegister
 *
 *            2. CCTLR Internal Use Only (prototype in this file)
 *
 *               - cnfgrCCtlrActionProcess 
 *               - cnfgrCCtlrCmdRqstBufferInit 
 *               - cnfgrCCtlrCompletionResponseIssue
 *               - cnfgrCCtlrEventGenerate 
 *               - cnfgrCCtlrEventProcess
 *               - cnfgrCCtlrEventReceive
 *               - cnfgrCCtlrInternalCommandIsValid
 *
 *            3. CCTLR Interface (prototype in cnfgr_ctl.h)
 * 
 *               - cnfgrCCtlrTallyComplete 
 *               - cnfgrCCtlrFini 
 *               - cnfgrCCtlrInitialize 
 *               - cnfgrCCtlrProcessCommand
 *
 *
 * Created by: avasquez 03/17/2003 
 *
 *********************************************************************/
#include "cnfgr_include.h"
#include "osapi_trace.h"

/* PTin added: logger */
#include "logger.h"

/*
 *********************************************************************
 *             Generic Defines
 *********************************************************************
 */

/* Configurator generic index numbers (decimal) 
 *
 */
#define CNFGR_NDX0    0
#define CNFGR_NDX1    1
#define CNFGR_NDX2    2
#define CNFGR_NDX3    3
#define CNFGR_NDX4    4
#define CNFGR_NDX5    5
#define CNFGR_NDX6    6
#define CNFGR_NDX7    7
#define CNFGR_NDX8    8
#define CNFGR_NDX9    9
#define CNFGR_NDX10  10
#define CNFGR_NDX11  11
#define CNFGR_NDX12  12
#define CNFGR_NDX13  13
#define CNFGR_NDX14  14
#define CNFGR_NDX15  15
#define CNFGR_NDX16  16

#define CNFGR_NDX17  17
#define CNFGR_NDX18  18
#define CNFGR_NDX19  19
#define CNFGR_NDX20  20

/*
 *********************************************************************
 *             Static (local) Variables
 *********************************************************************
 */

/*
 * These state variables are the current and next states of ALL components.
 * They are not affected by state changes in any single component.  The next
 * state variable is only meaningful if the tally handle is not NULL.
 */
static L7_CNFGR_STATE_t  currentState_g = L7_CNFGR_STATE_NULL;
static L7_CNFGR_STATE_t  nextState_g    = L7_CNFGR_STATE_NULL;
static CNFGR_CT_HANDLE_t tallyHandle_g  = L7_NULL;

/* L7_TRUE when restart is warm. Sent with p3 init notification. */
static L7_BOOL cnfgrWarmRestart = L7_FALSE;



/* Callback table -  This table contains registered callback functions
 *
 */
#define CNFGR_CTRL_CALLBACK_MAX_f  cnfgrSidCtrlCallbackMaxGet()       
static struct
{
  void             *lock;
  L7_uint32         size;
  L7_uint32         maxSize;
  L7_VOIDFUNCPTR_t *pArray;

} callbackTable = {0,0,0,0};



/* Transition Table - This table is used to index into the action  
 *                    table based on the event and current state.
 */

#define CNFGR_MAX_EVENTS  CNFGR_EVNT_LAST-CNFGR_EVNT_FIRST
#define CNFGR_MAX_STATE   L7_CNFGR_STATE_LAST-L7_CNFGR_STATE_NULL

/* If the unit manager is not present, i.e. no stacking, then there's
 * no need to cycle the components through WMU unit, but directly to
 * phase 3.
 */

#ifdef L7_STACKING_PACKAGE
#define CNFGR_NXT0  CNFGR_NDX4     /* IP2  -> IWMU */
#define CNFGR_NXT1  CNFGR_NDX3     /* IWMU -> IP3  */
#else
#define CNFGR_NXT0  CNFGR_NDX3     /* IP2 -> IP3   */
#define CNFGR_NXT1  CNFGR_NDX0     /* can't happen */
#endif

static L7_uint32 transitionTable[CNFGR_MAX_EVENTS+1][CNFGR_MAX_STATE] = 
{

  /* event     NULL      IDLE       r.i.P1      r.i.P2      r.i.P3     r.i.WMU      r.e         r.u.U1      r.u.U2      s      */
  /* nil*/ {CNFGR_NDX0,CNFGR_NDX0,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 },
  /* I1 */ {CNFGR_NDX0,CNFGR_NDX1,CNFGR_NDX11,CNFGR_NDX11,CNFGR_NDX11,CNFGR_NDX11,CNFGR_NDX11,CNFGR_NDX11,CNFGR_NDX11,CNFGR_NDX11},
  /* I2 */ {CNFGR_NDX0,CNFGR_NDX0,CNFGR_NDX2 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 },    
  /* I3 */ {CNFGR_NDX0,CNFGR_NDX0,CNFGR_NDX0 ,CNFGR_NDX4 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 },    
  /* I4 */ {CNFGR_NDX0,CNFGR_NDX0,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX3 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 },    
  /* I5 */ {CNFGR_NDX0,CNFGR_NDX0,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX4 ,CNFGR_NDX0 },    
  /* E  */ {CNFGR_NDX0,CNFGR_NDX0,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX5 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 },    
  /* U1 */ {CNFGR_NDX0,CNFGR_NDX0,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX6 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 },    
  /* U2 */ {CNFGR_NDX0,CNFGR_NDX0,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX7 ,CNFGR_NDX0 ,CNFGR_NDX0 },     
  /* T  */ {CNFGR_NDX0,CNFGR_NDX0,CNFGR_NDX8 ,CNFGR_NDX8 ,CNFGR_NDX8,CNFGR_NDX8 ,CNFGR_NDX8,CNFGR_NDX13,CNFGR_NDX14,CNFGR_NDX15},   
  /* S  */ {CNFGR_NDX0,CNFGR_NDX0,CNFGR_NDX9 ,CNFGR_NDX9 ,CNFGR_NDX9 ,CNFGR_NDX9 ,CNFGR_NDX9 ,CNFGR_NDX9 ,CNFGR_NDX9 ,CNFGR_NDX0 },
  /* R  */ {CNFGR_NDX0,CNFGR_NDX0,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX0 ,CNFGR_NDX10},
  /*tmr!*/ {CNFGR_NDX0,CNFGR_NDX0,CNFGR_NDX0, CNFGR_NDX0, CNFGR_NDX0, CNFGR_NDX0, CNFGR_NDX0, CNFGR_NDX0, CNFGR_NDX0, CNFGR_NDX0 }    
}; 


/* Action Data - These definitions and structures specify the action
 *               to be taken during an event/state transition.
 * 
 */

/* Action entry */
typedef struct 
{
  L7_CNFGR_STATE_t  nextState;
  L7_uint32         cmdRqstIndex;
  L7_BOOL           saveState;
  L7_BOOL           restoreState; 
  L7_BOOL           timer;
  CNFGR_CTLR_EVNT_t msgIndex;

} CNFGR_CTLR_ACTION_t;

/* Action table - This table is used to index into the command and the message tables. The
 *                action table also provide other actions such as timer, save, restore states.
 */
#define CNFGR_CTLR_ACTION_MAX  14

static CNFGR_CTLR_ACTION_t actionTable[] =
{
  /* ndx : action    {nextState,cmdRqstIndex,saveState,restoreState,timer,msgIndex}  */  
  /*  0  : -X-       */ {0,0,0,0,0,0},                 /* NULL */
  /*  1  : r.i.P1    */ {L7_CNFGR_STATE_P1,
    CNFGR_NDX1,
    L7_FALSE,
    L7_FALSE,
    L7_TRUE,
    CNFGR_NDX0            /* None */
  },
  /*  2  : r.i.P2    */ {L7_CNFGR_STATE_P2,
    CNFGR_NDX2,
    L7_FALSE,
    L7_FALSE,
    L7_TRUE,
    CNFGR_NDX0           /* None */
  },
  /*  3  : r.i.P3    */ {L7_CNFGR_STATE_P3,
    CNFGR_NDX3,
    L7_FALSE,
    L7_FALSE,
    L7_TRUE,
    CNFGR_NDX0           /* None */       
  },
  /*  4  : r.i.WMU   */ {L7_CNFGR_STATE_WMU,
    CNFGR_NDX4,
    L7_FALSE,
    L7_FALSE,
    L7_TRUE,
    CNFGR_NDX0           /* None */
  },
  /*  5  : r.e.      */ {L7_CNFGR_STATE_E,
    CNFGR_NDX5,
    L7_FALSE,
    L7_FALSE,
    L7_TRUE,
    CNFGR_NDX0           /* None */                       
  },
  /*  6  : r.u.U1    */ {L7_CNFGR_STATE_U1,
    CNFGR_NDX6,
    L7_FALSE,
    L7_FALSE,
    L7_TRUE,
    CNFGR_NDX0,          /* None */                        
  },
  /*  7  : r.u.U2    */ {L7_CNFGR_STATE_U2,
    CNFGR_NDX7,
    L7_FALSE,
    L7_FALSE,
    L7_TRUE,
    CNFGR_NDX0          /* None */                        
  },
  /*  8  : Terminate */ {L7_CNFGR_STATE_T,
    CNFGR_NDX8,
    L7_FALSE,
    L7_FALSE,
    L7_TRUE,
    CNFGR_NDX0,         /* None */                         
  },
  /*  9  : S(x.y.Zw) */ {L7_CNFGR_STATE_S,
    CNFGR_NDX9,
    L7_TRUE,
    L7_FALSE,
    L7_TRUE,
    CNFGR_NDX0          /* None */                        
  },
  /* 10  : (x.t.Zw)  */ {L7_CNFGR_STATE_NULL,
    CNFGR_NDX10,
    L7_FALSE,
    L7_TRUE,
    L7_TRUE,
    CNFGR_NDX0          /* None */                        
  },

  /* 11  : g[T]      */ {L7_CNFGR_STATE_NULL,
    CNFGR_NDX0,
    L7_FALSE,
    L7_FALSE,
    L7_FALSE,
    CNFGR_NDX0        /* TBD: CNFGR_CTLR_EVNT_TERMINATE */                         
  },
  /* 12  : r.i.U1    */ {L7_CNFGR_STATE_U1,
    CNFGR_NDX6,
    L7_FALSE,
    L7_FALSE,
    L7_TRUE,
    /*       g[T]      */  CNFGR_NDX0        /* TBD: CNFGR_CTLR_EVNT_TERMINATE */                         
  },
  /* 13  : r.i.U2    */ {L7_CNFGR_STATE_U2,
    CNFGR_NDX7,
    L7_FALSE,
    L7_FALSE,
    L7_TRUE,
    /*       g[T]      */  CNFGR_NDX0        /* TBD: CNFGR_CTLR_EVNT_TERMINATE */                         
  },

  /* 14  : r.i.WMU   */ {L7_CNFGR_STATE_WMU,
    CNFGR_NDX4,
    L7_FALSE,
    L7_FALSE,
    L7_TRUE,
    /*       g[T]      */  CNFGR_NDX0        /* TBD: CNFGR_CTLR_EVNT_TERMINATE */                         
  },
  /* 15  : (x.y.Zw)  */ {L7_CNFGR_STATE_NULL,
    CNFGR_NDX0,
    L7_FALSE,
    L7_TRUE,
    L7_FALSE,
    /*       g[T]      */  CNFGR_NDX0        /* TBD: CNFGR_CTLR_EVNT_TERMINATE */                         
  }
};

/* Command Data - This Table is used to issue command/request pairs.
 *                The command data does not change over the ON cycle.
 * 
 */
#define CNFGR_CMDRQST_MAX  11

static L7_CNFGR_CMD_DATA_t cmdRqstTable[] =
{
  /* ndx: id    {correlator,cbhandle,command,type,{{rqstData}}}  */  
  /*  0 : XX */ {0,0,0,0,{{0,0}}},                 /* NULL */
  /*  1 : I1 */ {0,
    0,
    L7_CNFGR_CMD_INITIALIZE,
    L7_CNFGR_RQST,
    {{L7_CNFGR_RQST_I_PHASE1_START,0}}
  },
  /*  2 : I2 */ {0,
    0,
    L7_CNFGR_CMD_INITIALIZE,
    L7_CNFGR_RQST,
    {{L7_CNFGR_RQST_I_PHASE2_START,0}}
  },
  /*  3 : I3 */ {0,
    0,
    L7_CNFGR_CMD_INITIALIZE,
    L7_CNFGR_RQST,
    {{L7_CNFGR_RQST_I_PHASE3_START,0}}
  },
  /*  4 : I4 */ {0,                                  
    0,
    L7_CNFGR_CMD_INITIALIZE,
    L7_CNFGR_RQST,
    {{L7_CNFGR_RQST_I_WAIT_MGMT_UNIT,0}}
  },
  /*  5 : E1 */ {0,                                  
    0,
    L7_CNFGR_CMD_EXECUTE,
    L7_CNFGR_RQST,
    {{L7_CNFGR_RQST_E_START,0}}
  },
  /*  6 : U1 */ {0,                                  
    0,
    L7_CNFGR_CMD_UNCONFIGURE,
    L7_CNFGR_RQST,
    {{L7_CNFGR_RQST_U_PHASE1_START,0}}
  },
  /*  7 : U2 */ {0,                                  
    0,
    L7_CNFGR_CMD_UNCONFIGURE,
    L7_CNFGR_RQST,
    {{L7_CNFGR_RQST_U_PHASE2_START,0}}
  },
  /*  8 : T1 */ {0,                                  
    0,
    L7_CNFGR_CMD_TERMINATE,
    L7_CNFGR_RQST,
    {{L7_CNFGR_RQST_T_START,0}}
  },
  /*  9 : S1 */ {0,                                  
    0,
    L7_CNFGR_CMD_SUSPEND,
    L7_CNFGR_RQST,
    {{L7_CNFGR_RQST_S_START,0}}
  },
  /* 10 : R1 */ {0,                                  
    0,
    L7_CNFGR_CMD_RESUME,
    L7_CNFGR_RQST,
    {{L7_CNFGR_RQST_R_START,0}}
  },
  /* 11 : XY */ { 0,   0,       0,     0,     {{0,0}}}    
};


/* Command/Event Mapping table -
 *
 */
static CNFGR_CTLR_EVNT_t  ctlrExt2IntEvntMappingTable[L7_CNFGR_EVNT_LAST-L7_CNFGR_EVNT_FIRST+1] =
{
  /* Ext:    Int                       */
  /* 0  : */ CNFGR_EVNT_FIRST,            

  /* Component Initialization events    */
  /* 1  : */ CNFGR_EVNT_I_START,
  /* 2  : */ CNFGR_EVNT_I_MGMT_UNIT,

  /* Component Unconfigure events       */
  /* 3  : */ CNFGR_EVNT_U_START,
  /* 4  : */ CNFGR_EVNT_U_PHASE1_CMPLT, 

  /* Component Termination event        */
  /* 5  : */ CNFGR_EVNT_T_START,

  /* Component Suspend event            */
  /* 6  : */ CNFGR_EVNT_S_START,

  /* Component Resume event             */
  /* 7  : */ CNFGR_EVNT_R_START,

  /* Last event -- range checking       */
  /* 8  : */ CNFGR_EVNT_LAST
};

/* Map internal events to commands
 * 
 * NOTE: this is a tightly coupled table to the CNFGR_CTLR_EVNT_t. Any changes
 *       to the event order, this table MUST be changed as well.
 */
static L7_CNFGR_CMD_t intEvent2CmdTable[CNFGR_EVNT_LAST-CNFGR_EVNT_FIRST+1] =
{
  /* evnt:   cnd                       */
  /* 0  : */ L7_CNFGR_CMD_FIRST,            

  /* Component Initialization */
  /* 1  : */ L7_CNFGR_CMD_INITIALIZE,
  /* 2  : */ L7_CNFGR_CMD_INITIALIZE,
  /* 3  : */ L7_CNFGR_CMD_INITIALIZE,
  /* 4  : */ L7_CNFGR_CMD_INITIALIZE,
  /* 5  : */ L7_CNFGR_CMD_INITIALIZE,

  /* Component Execution     */
  /* 6  : */ L7_CNFGR_CMD_EXECUTE,

  /* Component Unconfigure events       */
  /* 7  : */ L7_CNFGR_CMD_UNCONFIGURE,
  /* 8  : */ L7_CNFGR_CMD_UNCONFIGURE, 

  /* Component Termination event        */
  /* 9  : */ L7_CNFGR_CMD_TERMINATE,

  /* Component Suspend event            */
  /* 10 : */ L7_CNFGR_CMD_SUSPEND,

  /* Component Resume event             */
  /* 11 : */ L7_CNFGR_CMD_RESUME,

  /* Last event -- range checking       */
  /* 12 : */ L7_CNFGR_CMD_LAST

};

/* Map next states to next internal events --
 *
 * When tally is complete, the next state becomes current state, and if there is
 * an internal event to be generated, then the next state indexes the event to
 * be generated. If there is no event to be generated, the table indexes to
 * CNFGR_EVNT_FIRST.  Typically those states were triggered by an external
 * event.
 * 
 * NOTE: this is a tightly coupled table to the CNFGR_CTLR_EVNT_t. Any changes
 *       to the event or state order , this table MUST be changed as well.
 */
static CNFGR_CTLR_EVNT_t nextState2NextInternalEvent[] =
{
  /* next state:     next internal event   */
  /* NULL- 0  : */ CNFGR_EVNT_FIRST,            

  /* Component Initialization */
  /* IDLE- 1  : */ CNFGR_EVNT_FIRST,         /* during crInitialize */
  /* P1-   2  : */ CNFGR_EVNT_I_PHASE1_CMPLT,
  /* P2-   3  : */ CNFGR_EVNT_I_PHASE2_CMPLT,
  /* P3-   4  : */ CNFGR_EVNT_E_START,
  /* WMU-  5  : */ CNFGR_EVNT_FIRST,         /* external generated event */    

  /* Component Execution     */
  /* E-    6  : */ CNFGR_EVNT_FIRST,         /* external generated event */

  /* Component Unconfigure events       */
  /* U1-   7  : */ CNFGR_EVNT_U_PHASE1_CMPLT,
  /* U2-   8  : */ CNFGR_EVNT_I_UNCONFIGURE_CMPLT,
  /* WMU-  5  : */ CNFGR_EVNT_FIRST,         /* external generated event */

  /* Component Termination event        */
  /* T-    9 : */ CNFGR_EVNT_FIRST,         /* external generated event */

  /* Component Suspend event            */
  /* S-    10 : */ CNFGR_EVNT_FIRST,         /* external generated event */

  /* Component Resume event             */
  /* R-    11 : */ CNFGR_EVNT_FIRST,         /* external generated event */

};


/*
 *********************************************************************
 *             Configurator Interface Function Calls
 *********************************************************************
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
L7_RC_t cnfgrApiCommand( CNFGR_IN  L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_int32          ndx;
  CNFGR_CTLR_EVNT_t event;
  CNFGR_MSG_DATA_t  msgData,
		    *pMsgData = &msgData;
  L7_RC_t           cnfgrRC  = L7_ERROR;

  if ( cnfgrReadyAndInitialized != L7_TRUE )
    return (L7_ERROR );

  /* Do initial check on pCmdData ---
   *
   */
  if ( pCmdData != L7_NULLPTR ) {

    /* map the event -
     *
     * NOTE: Final validation will be done in the cnfgr's msg handler thread.
     */
    if (pCmdData->u.evntData.event > L7_CNFGR_EVNT_FIRST &&
	pCmdData->u.evntData.event < L7_CNFGR_EVNT_LAST)

      ndx = pCmdData->u.evntData.event % L7_CNFGR_EVNT_FIRST;
    else
      /* Something is wrong.. Force an invalid event. Let
       * msg handler thread figure it out.
       */
      ndx = L7_CNFGR_EVNT_LAST % L7_CNFGR_EVNT_FIRST;  

    event = ctlrExt2IntEvntMappingTable[ndx];

     /* prepare the message */
    pMsgData->msgType                  = CNFGR_MSG_COMMAND;
    pMsgData->u.cmdData.correlator     = pCmdData->correlator;
    pMsgData->u.cmdData.cbHandle       = pCmdData->cbHandle;
    pMsgData->u.cmdData.command        = pCmdData->command;
    pMsgData->u.cmdData.type           = pCmdData->type;
    pMsgData->u.cmdData.evntData.event = event;
    pMsgData->u.cmdData.evntData.data  = pCmdData->u.evntData.data;

    if (event == CNFGR_EVNT_I_MGMT_UNIT)
      OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_P3_START, (L7_uchar8 *)L7_NULLPTR, 0);
    else if (event == CNFGR_EVNT_E_START)
      OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_EXECUTE_START, (L7_uchar8 *)L7_NULLPTR, 0);

    cnfgrTraceEvent(event, L7_TRUE);
    cnfgrProfileEventStart(event);    

    /* send it ! */
    cnfgrRC = cnfgrMsgSend(pMsgData);

  } /* endif valid pCmdData value */

  /* Return to Caller */
  return (cnfgrRC);
}


/*********************************************************************
 * @purpose  This function is invoked by timer expiry for sending
 *           delayed configurator event.
 *
 * @param    pMsgData - Pointer to the message buffer.
 *
 * @returns  none
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
void cnfgrPendingMsgSend (CNFGR_IN CNFGR_MSG_DATA_t *pMsgData)
{
  L7_RC_t rc;

  rc = cnfgrMsgSend (pMsgData);

  osapiFree (L7_CNFGR_COMPONENT_ID, (void *) pMsgData);
}


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
    )
{
  /* set up variables and structures */
  L7_int32          ndx;
  CNFGR_CTLR_EVNT_t event;
  CNFGR_MSG_DATA_t *pMsgData = 0; 
  L7_RC_t           cnfgrRC  = L7_ERROR;
  osapiTimerDescr_t *pTimerHolder;

  if ( cnfgrReadyAndInitialized != L7_TRUE )
    return (L7_ERROR );

  /* Do initial check on pCmdData ---
   *
   */
  pMsgData = osapiMalloc (L7_CNFGR_COMPONENT_ID, sizeof (CNFGR_MSG_DATA_t));
  if ( pCmdData != L7_NULLPTR ) {

    /* map the event -
     *
     * NOTE: Final validation will be done in the cnfgr's msg handler thread.
     */
    if (pCmdData->u.evntData.event > L7_CNFGR_EVNT_FIRST &&
	pCmdData->u.evntData.event < L7_CNFGR_EVNT_LAST)

      ndx = pCmdData->u.evntData.event % L7_CNFGR_EVNT_FIRST;
    else
      /* Something is wrong.. Force an invalid event. Let
       * msg handler thread figure it out.
       */
      ndx = L7_CNFGR_EVNT_LAST % L7_CNFGR_EVNT_FIRST;  

    event = ctlrExt2IntEvntMappingTable[ndx];

    /* prepare the message */
    pMsgData->msgType                  = CNFGR_MSG_COMMAND;
    pMsgData->u.cmdData.correlator     = pCmdData->correlator;
    pMsgData->u.cmdData.cbHandle       = pCmdData->cbHandle;
    pMsgData->u.cmdData.command        = pCmdData->command;
    pMsgData->u.cmdData.type           = pCmdData->type;
    pMsgData->u.cmdData.evntData.event = event;
    pMsgData->u.cmdData.evntData.data  = pCmdData->u.evntData.data;

    /* schedule it ! */
    osapiTimerAdd( (void *) cnfgrPendingMsgSend, 
	(L7_uint32)pMsgData, 
	L7_NULL, 
	milliseconds,
	&pTimerHolder );
    cnfgrRC = L7_SUCCESS;

  } /* endif valid pCmdData value */

  /* Return to Caller */
  return (cnfgrRC);
}

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
void cnfgrApiDeregister(CNFGR_IN L7_CNFGR_CB_HANDLE_t cbHandle)
{  
  if ( cnfgrReadyAndInitialized != L7_TRUE )
    return;

  osapiSemaTake(callbackTable.lock, -1);   /* -1: wait forever */

  if (cbHandle != 0 && cbHandle < callbackTable.maxSize+1) 
    callbackTable.pArray[(int)cbHandle] = (L7_VOIDFUNCPTR_t)L7_NULLPTR;

  osapiSemaGive(callbackTable.lock);

  /* Return to caller */
  return;
}

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
    )
{
  int        i;
  L7_RC_t    cnfgrRC   = L7_SUCCESS;
  *pCbHandle = L7_CNFGR_NO_HANDLE;

  if ( cnfgrReadyAndInitialized != L7_TRUE )
    return (L7_ERROR );

  osapiSemaTake(callbackTable.lock, -1);

  if (pCbFcnt == (L7_VOIDFUNCPTR_t)L7_NULLPTR || callbackTable.size == callbackTable.maxSize) 
    cnfgrRC = L7_ERROR;
  else 
  {
    /* Find if the callback function exist, if so, then return same handle
     * else find a null space.
     *
     * NOTE: at this point there is space available.
     */
    for (i = 1; callbackTable.pArray[i] == pCbFcnt; i++) {} 

    if (i == callbackTable.maxSize+1){
      for (i = 1; callbackTable.pArray[i] == (L7_VOIDFUNCPTR_t)L7_NULLPTR; i++) {}

      callbackTable.size++;
    } /* endif function exist */

    callbackTable.pArray[i] = pCbFcnt;
    *pCbHandle = (L7_CNFGR_CB_HANDLE_t)i;
  }

  osapiSemaGive(callbackTable.lock);

  /* Return to caller */
  return (cnfgrRC);
}

/*********************************************************************
 * @purpose  This function returns the current state of all components.
 *           The use of this function is optional.
 *
 * @param    currentState - @b{(output)} Storage for current state.
 *
 * @returns  L7_SUCCESS
 *
 * @notes    This function is synchronous, therefore it will return to
 *           the caller once this function has completed its work.
 *
 * @end
 *********************************************************************/
L7_RC_t cnfgrApiSystemStateGet(CNFGR_OUT L7_CNFGR_STATE_t *currentState)
{
  *currentState = currentState_g;
  return( L7_SUCCESS );
}

/*
 *********************************************************************
 *                      CCTRL intenal functions 
 *********************************************************************
 */


/*
   Internal function prototypes
 */
L7_RC_t cnfgrCCtlrActionProcess(
    CNFGR_IN    CNFGR_CT_HANDLE_t     ctHandle,
    CNFGR_IN    L7_uint32             actionNdx,
    CNFGR_INOUT CNFGR_CR_COMPONENT_t *pComponent
    );
void cnfgrCCtlrCmdRqstBufferInit(L7_CNFGR_CMD_DATA_t *cmdRqst, L7_uint32 cmdRqstIndex);
void cnfgrCCtlrCompletionResponseIssue(
    CNFGR_IN L7_CNFGR_CORRELATOR_t correlator, 
    CNFGR_IN L7_CNFGR_CB_HANDLE_t  cbHandle, 
    CNFGR_IN L7_CNFGR_ASYNC_RESPONSE_t aRsp
    );
void cnfgrCCtlrEventGenerate( CNFGR_IN L7_COMPONENT_IDS_t cid, 
    CNFGR_IN CNFGR_CTLR_EVNT_t event
    );
L7_uint32 cnfgrCCtlrEventProcess(
    CNFGR_IN CNFGR_CTLR_EVNT_t     event,
    CNFGR_IN CNFGR_CR_COMPONENT_t *pComponent 
    );
void cnfgrCCtlrEventReceive(
    CNFGR_IN CNFGR_EVNT_DATA_t     eventData,
    CNFGR_IN L7_CNFGR_CORRELATOR_t correlator,
    CNFGR_IN L7_CNFGR_CB_HANDLE_t  cbHandle
    );
L7_BOOL cnfgrCCtlrInternalCommandIsValid(
    CNFGR_IN  L7_CNFGR_CMD_t command, 
    CNFGR_IN  L7_CNFGR_CMD_TYPE_t type, 
    CNFGR_IN  CNFGR_CTLR_EVNT_t event, 
    CNFGR_OUT L7_CNFGR_ERR_RC_t *pReason);

/*
   Internal functions
 */ 

/*********************************************************************
 * @purpose  This function proccesses an action. This function is only
 *           used by CCtlr.
 *
 * @param    ctHandle  - @b{(input)}  A tally handle for completion 
 *                                    callback. 
 * @param    actionNdx - @b{(input)}  Value that represent the   
 *                                    action to be taken by CCtrl. 
 * @param    pComponent- @b{(input)}  Component(s) involved.  
 * 
 * @returns  L7_SUCCESS - Action has been processed susscefully.        
 * @returns  L7_ERROR   - Configurator failed to register the callback
 *                        function. pCBHandle is L7_CNFGR_NO_HANDLE
 *
 * @notes    None.                                                     
 *       
 * @end
 *********************************************************************/
L7_RC_t cnfgrCCtlrActionProcess(
    CNFGR_IN   CNFGR_CT_HANDLE_t    ctHandle,
    CNFGR_IN   L7_uint32            actionNdx,
    CNFGR_CR_COMPONENT_t *CNFGR_IN  pComponent
    )
{
  L7_CNFGR_CMD_DATA_t   *pCmdData, cmdData;
  L7_RC_t                cnfgrRC  = L7_SUCCESS;
  CNFGR_CTLR_ACTION_t   *pAction  = &actionTable[actionNdx];
  L7_CNFGR_CORRELATOR_t  correlator,
			 *pCorrelator;

  LOG_TRACE(LOG_CTX_STARTUP,"Processing action %u for cid=%u",actionNdx,pComponent->cid);

  pCmdData = &cmdData;

  /* Check if not action need */
  if (actionNdx != CNFGR_NDX0) { 
    if (pAction->cmdRqstIndex != CNFGR_NDX0) { 
      /* At this point there is a request to be issued to the
       * component (pComponent). Do it ! 
       *
       * Get a buffer to place the command/request.
       */   
      cnfgrCCtlrCmdRqstBufferInit(pCmdData, pAction->cmdRqstIndex);
	/* At this point there is a buffer available and partially
	 * filled up.  Finish and send it!
	 * 
	 * Attributes in: cbHandle,command,type,request and data.
	 *
	 *
	 * Get a correlator.
	 */
	pCorrelator = &correlator;

  if (cnfgrWarmRestart)
  {
    pCmdData->u.rqstData.data |= L7_CNFGR_RESTART_WARM;
  }
	cnfgrRC = cnfgrTallyAdd(ctHandle, pComponent->cid, pCorrelator);
	if (cnfgrRC == L7_SUCCESS) {
	  /* Issue the command. The response to the command 
	   * will be received by cnfgrApiCallback().
	   *
	   * NOTE: The CCtlr ensures that only components which are
	   *       present shall be processed.
	   *
	   * Fill in component attributes for query
	   */
      cnfgrTraceEventPerComp(pCmdData->u.rqstData.request, L7_TRUE, pComponent->cid, correlator);
      cnfgrProfileEventCompStart(pCmdData->u.rqstData.request, pComponent->cid, correlator);
	  pCmdData->correlator = correlator;
	  pComponent->pCommandFunction(pCmdData);

	  pComponent->cmd  = pCmdData->command;
	  pComponent->rqst = pCmdData->u.rqstData.request;

	  /* Ensure that state is saved if necessary. When
	   * the event completes, then the states will be
	   * transitioned.
	   */
	  if (pAction->saveState == L7_TRUE || 
	      pAction->restoreState == L7_TRUE
	     ) {
	    /* Either save or restore the state is required.
	     * Do it!
	     *
	     * NOTE: save and restore are mutually exclusive.
	     */ 
	    if (pAction->saveState == L7_TRUE) {
	      pComponent->saveState = pComponent->currentState;
	      pComponent->nextState = pAction->nextState;
	    } else {
	      pComponent->nextState = pComponent->saveState;
	    }    /* endif save state */
	  } else { 
	    pComponent->nextState = pAction->nextState;
	  } /* endif save or restore */
	} else {
	  /* At this point a fatal error has occurred. Log message
	   * and return to caller for further processing.
	   */
	  cnfgrRC = L7_ERROR;
	} /* endif tallyAdd */

    } else { 

      /* At this point, there is not command/request, but there may be
       * a message to be issued. 
       * to be issued.  Go ahead!
       */  
      cnfgrCCtlrEventGenerate(pComponent->cid, pAction->msgIndex);

    } /* endif cmd/request */
  } /* endif action needed */

  /* Return value to caller */
  return (cnfgrRC);
}

/*********************************************************************
 * @purpose  This function gets a buffer which contains the command/request
 *           pair to be issued to a component. This function is only used by
 *           CCTlr.
 *
 * @param    cmdRqstIndex - @b{(input)} index to cmdRqst. This will be
 *                                      used to fill the cmd data.
 * 
 * @returns  cmdRqst    - points to command/request data. If zero,
 *                        no buffers available.
 *
 * @notes    These functions (get and put) was implemented with the
 *           intent of providing the buffer to the component until
 *           completion of the command/request pair. At this point
 *           the buffer is returned upon completion of the command
 *           call.                                                     
 *       
 * @end
 *********************************************************************/
void cnfgrCCtlrCmdRqstBufferInit(L7_CNFGR_CMD_DATA_t *cmdRqst, L7_uint32 cmdRqstIndex)
{

  memset (cmdRqst, 0, sizeof (L7_CNFGR_CMD_DATA_t));

  /* copy the command into the buffer */
  cmdRqst->cbHandle            =  L7_CNFGR_NO_HANDLE;
  cmdRqst->command             =  cmdRqstTable[cmdRqstIndex].command;
  cmdRqst->correlator          =  (L7_CNFGR_CORRELATOR_t)L7_NULL;
  cmdRqst->type                =  L7_CNFGR_RQST;
  cmdRqst->u.rqstData.request  = cmdRqstTable[cmdRqstIndex].u.rqstData.request;
  cmdRqst->u.rqstData.data     = cmdRqstTable[cmdRqstIndex].u.rqstData.data;
}

/*********************************************************************
 * @purpose  This function issues completion callback to the component.
 *           This function is only used by CCtlr.
 *
 * @param    correlator- @b{(input)}  A value provided by the component.
 * @param    cbHandle  - @b{(input)}  A value that links the component's
 *                                    callback function.           
 * @param    aRsp      - @b{(input)}  final response for the component.
 * 
 * @returns  None.                                                      
 *
 * @notes    This function will be called even if the user of the 
 *           configurator does not register callback function. Thus
 *           this function must handle this case.
 *       
 * @end
 *********************************************************************/
void cnfgrCCtlrCompletionResponseIssue(
    CNFGR_IN L7_CNFGR_CORRELATOR_t correlator, 
    CNFGR_IN L7_CNFGR_CB_HANDLE_t  cbHandle, 
    CNFGR_IN L7_CNFGR_ASYNC_RESPONSE_t aRsp
    )
{
  /* Set up data structures and Values */
  L7_VOIDFUNCPTR_t pCbFcnt;
  L7_uint32 maxSize;

  /* Lock the callback table */
  osapiSemaTake(callbackTable.lock, L7_WAIT_FOREVER);

  pCbFcnt = callbackTable.pArray[cbHandle];
  maxSize = callbackTable.maxSize;

  /* Done with the table, unlock the callback table */
  osapiSemaGive(callbackTable.lock);

  /* Validate that:                           */
  /* 1.- cbHandle is on range                 */
  if (cbHandle != L7_CNFGR_NO_HANDLE && 
      (cbHandle > 0 && cbHandle <= maxSize)) {

    /* 2.- The callback function exist      */
    if (pCbFcnt != (L7_VOIDFUNCPTR_t)L7_NULLPTR) {

      /* make the call back !  */
      pCbFcnt(correlator, aRsp);

    } /* endif valid callback function */

  } /* endif Valid cbHandle */


  /* Return to caller */
  return;
}





/*********************************************************************
 * @purpose  This function generates an internal event to the controller.
 *           This function is only used by CCtlr.
 *
 * @param    cid         - @b{(input)}  destination component               
 *                                                             
 *
 * @param    command     - @b{(input)}  command to be issued                
 *                                                             
 * @param    event       - @b{(input)}  event to be sent
 *
 * @returns  None                       
 *
 * @notes    Assumes that always the message to the controller was issued
 *           successfully.                                                     
 *       
 * @end
 *********************************************************************/
void cnfgrCCtlrEventGenerate( CNFGR_IN L7_COMPONENT_IDS_t cid, 
    CNFGR_IN CNFGR_CTLR_EVNT_t event
    )
{
  CNFGR_MSG_DATA_t  msgData,
  *pMsgData = &msgData;
  /* Check for NOOP event ---
   *
   */
  if ( event > CNFGR_EVNT_FIRST && event < CNFGR_EVNT_LAST ) {

    /* prepare the message */
    pMsgData->msgType                  = CNFGR_MSG_COMMAND;
    pMsgData->u.cmdData.correlator     = L7_NULL;
    pMsgData->u.cmdData.cbHandle       = L7_NULL;
    pMsgData->u.cmdData.command        = intEvent2CmdTable[event];
    pMsgData->u.cmdData.type           = L7_CNFGR_EVNT;
    pMsgData->u.cmdData.evntData.event = event;
    pMsgData->u.cmdData.evntData.data  = cid;

    /* send it ! */
    (void)cnfgrMsgSend(pMsgData);

    if (event == CNFGR_EVNT_I_MGMT_UNIT)
      OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_P3_START, (L7_uchar8 *)L7_NULLPTR, 0);
    else if (event == CNFGR_EVNT_E_START)
      OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_EXECUTE_START, (L7_uchar8 *)L7_NULLPTR, 0);

    cnfgrTraceEvent(event, L7_TRUE);
    cnfgrProfileEventStart(event);    

  } /* check for NOOP */
  /* return to caller */
  return;
}

/*********************************************************************
 * @purpose  This function processes an event. This function is only 
 *           used by CCtlr.
 *
 * @param    event     - @b{(input)}  event received.               
 *                                                             
 * @param    pComponent- @b{(input)}  Component involved in the event.
 *
 * @returns  actionNdx  - action per event input.                       
 *
 * @notes    Event received are always valid.                                                     
 *       
 * @end
 *********************************************************************/
L7_uint32 cnfgrCCtlrEventProcess(
    CNFGR_IN CNFGR_CTLR_EVNT_t     event,
    CNFGR_IN CNFGR_CR_COMPONENT_t *pComponent 
    )
{
  /* Return value to caller */
  return (transitionTable[event][pComponent->currentState % L7_CNFGR_STATE_NULL]);
}

/*********************************************************************
 * @purpose  This function receives and process a configurator event. 
 *           This function is only used by CCtlr.
 *
 * @param    eventData  - @b{(input)}  Contains event information. 
 *                                     generator. 
 * @param    correlator - @b{(input)}  value provided by the component.
 * @param    cbHandle   - @b{(input)}  Contains callback information. 
 * 
 * @returns  None.      - Action has been processed susscefully.        
 *
 * @notes    None.                                                     
 *       
 * @end
 *********************************************************************/
void cnfgrCCtlrEventReceive(
                           CNFGR_IN CNFGR_EVNT_DATA_t     eventData,
                           CNFGR_IN L7_CNFGR_CORRELATOR_t correlator,
                           CNFGR_IN L7_CNFGR_CB_HANDLE_t  cbHandle
                           )
{
  L7_uint32              actionNdx;
  CNFGR_CR_COMPONENT_t  *pComponent;
  CNFGR_CT_HANDLE_t      ctHandle;
  CNFGR_CR_HANDLE_t      crHandle;
  L7_BOOL                exit;
  L7_int32               i;
  L7_RC_t                ctrlRC;
  L7_CNFGR_STATE_t       nextState = L7_CNFGR_STATE_NULL;

  L7_COMPONENT_IDS_t     cid         = (L7_COMPONENT_IDS_t)eventData.data;

  LOG_TRACE(LOG_CTX_STARTUP,"Received event %u for cid %u",eventData.event, cid);

  if ( cid != L7_LAST_COMPONENT_ID )
  {

    /* Single component request.
   *
   * NOTE: At this point the cid is valid (ensured by the caller)
   *
   * Check if this component is enable or if this component has
   * entry point for control. If is not, then
   * action is NOOP.
   */
    ctrlRC = cnfgrCrComponentTake( cid, &crHandle, &pComponent );
    if ( (ctrlRC == L7_SUCCESS) && (pComponent != L7_NULLPTR) &&
         (pComponent->mode == L7_CNFGR_COMPONENT_ENABLE) && 
         (pComponent->pCommandFunction != L7_NULLPTR) )
    {

      /* Get a tally handle -- Open a tally handle*/
      if ( (ctHandle = cnfgrTallyOpen(correlator, cbHandle)) == CNFGR_NDX0 )
      {

        /* At this point there is a error, let
         * system policies determine action. This may be fatal error. 
         */
      } /* endif open tally handle */

      LOG_TRACE(LOG_CTX_STARTUP,"Going to process event %u for cid %u",
                eventData.event, cid);

      /* Process the event to obtain an action.
       *
       * NOTE: No need to check for validity of the event since it
       *       was filtered by other processes. Thus at this point
       *       the event is valid  
       */
      actionNdx = cnfgrCCtlrEventProcess( eventData.event, pComponent );

      if ( actionNdx != CNFGR_NDX0 )
      {

        /* Process the action */
        (void)cnfgrCCtlrActionProcess( ctHandle, actionNdx, pComponent );

        /* At this point action has been processed. Tell tally all done
         * updating the handle.
         *
         * NOTE: if error (L7_ERROR), complete tally anyways. 
         */
        cnfgrTallyAddComplete( ctHandle, pComponent->nextState, cid );

      }
      else
      {

        /* At this point there are no components in the handle ---
         * Close it and signal the message handler that the event
         * is complete
         */ 
        cnfgrTallyClose(ctHandle);

        /* notify the event handler that this event processing has 
         * been completed
         */
        cnfgrMsgEventComplete();

      } /* endif NO ACTION */

    } /* endif component enable */

    /* We are done with this component. Returned it back to the repository! */
    (void)cnfgrCrComponentGive( crHandle );

  }
  else
  {
    LOG_TRACE(LOG_CTX_STARTUP,"All componentes processed!");

    /* Set up to traverse the Component Repository -
     *
     * NOTE: there should be at least one component in CR. This component is
     *       the L7_LAST_COMPONENT. In such case the operation is a NOOP.
     */
    exit = L7_FALSE;
    i = 0;
    ctrlRC = cnfgrCrComponentFirstTake( &crHandle, &pComponent );
    if ( ctrlRC == L7_SUCCESS )
    {

      /* Get a tally handle -- Open a tally handle*/
      if ( (ctHandle = cnfgrTallyOpen(correlator, cbHandle)) == CNFGR_NDX0 )
      {

        /* At this point there is a error, let
         * system policies determine action. 
         */
      } /* endif open tally handle */

      do
      {

        /* Check if this component is enable or if this component has
         * entry point for control. If is not, then
         * action is NOOP.
         */
        ctrlRC = L7_SUCCESS;
        if ( pComponent->mode == L7_CNFGR_COMPONENT_ENABLE && 
             pComponent->pCommandFunction != L7_NULLPTR )
        {

          /* Process the event to obtain action to take.
           *
           * NOTE: No need to check for validity of the event since it
           *       was filtered by other processes. Thus at this point
           *       the event is valid  
           */
          OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_COMP_EVT_ENTRY,
                            (L7_uchar8 *)&pComponent->cid, sizeof(int));
          actionNdx = cnfgrCCtlrEventProcess( eventData.event, pComponent );

          if ( actionNdx != CNFGR_NDX0 )
          {
            LOG_TRACE(LOG_CTX_STARTUP,"Going to process action %u for cid %u",
                      actionNdx, pComponent->cid);

            /* Process the action 
             *
             * NOTE: Stop on error. Other policies could be implemented
             *       by calling an error handler routine.
             */
            OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_COMP_ACT_ENTRY,
                              (L7_uchar8 *)&pComponent->cid, sizeof(int));
            ctrlRC = cnfgrCCtlrActionProcess( ctHandle, actionNdx, pComponent );
            OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_COMP_ACT_END,
                              (L7_uchar8 *)&pComponent->cid, sizeof(int));

            /* set the nextState for ALL cids... Do it once! */ 
            if ( nextState == L7_CNFGR_STATE_NULL )
            {
              nextState = pComponent->nextState;
            }

            i++;

          } /* endif NO ACTION */

        } /* endif component enabled */

        if ( ctrlRC == L7_SUCCESS )
        {

          /* At this point successful action has been taken, including NOOP. 
           * Release the component to the Component Repository.
           * Take the next one and process.
           */
          ctrlRC = cnfgrCrComponentGive(crHandle);
          if ( ctrlRC == L7_SUCCESS)
          {

            if ( pComponent->cid == L7_LAST_COMPONENT_ID )
              exit = L7_TRUE;
            else
            {
              ctrlRC = cnfgrCrComponentNextTake( &crHandle, &pComponent );
              if ( ctrlRC != L7_SUCCESS )
              {
                exit = L7_TRUE;
              } /* endif get next component from CR */
            } /* endif Last component */
          }
          else
          {
            exit = L7_TRUE;
          } /* return component to CR */
        }
        else
        {

          (void)cnfgrCrComponentGive( crHandle );
          exit = L7_TRUE;
        } /* endif process action */

      } while ( exit == L7_FALSE);

      /* At this point action has been processed. Tell tally all done
       * updating the handle. If there are no components in the handle
       * will be closed.
       *
       * NOTE: if error (ctrlRC == L7_ERROR), complete tally anyway. 
       */
      if (i > 0)
      {

        cnfgrTallyAddComplete( ctHandle, nextState, cid );
        /*
         * All components are to be moved to the next state.  Save the next
         * state and the tally which must complete successfully in order to
         * move to the next state.
         */
        nextState_g   = nextState;
        if (nextState == L7_CNFGR_STATE_P3)
        {
          cnfgrHwUpdatePendingMasksInit();
        }
        tallyHandle_g = ctHandle;
      }
      else
      {
        /* At this point there are no components in the handle ---
         * Close it and signal the message handler that the event
         * is complete
         */ 
        cnfgrTallyClose(ctHandle);

        /* notify the event handler that this event processing has 
         * been completed
         */
        cnfgrMsgEventComplete();

      } /* endif at least one component processed */

    } /* endif get the first component in CR */

  } /* endif single or all component */

  return;

}


/*********************************************************************
 * @purpose  This function validates command/events pairs. This function
 *           is a CCtlr internal use only.
 *
 * @param    command   - @b{(input)}  command to be processed.       
 * @param    type      - @b{(input)}  type of command.               
 * @param    event     - @b{(input)}  event to be processed.         
 * @param    pReason   - @b{(output)} completion callback value. 
 *
 * @returns  L7_TRUE  - command data is  valid.
 * @returns  L7_FALSE - command data is invalid.
 *                      pReason = L7_CNFGR_ERR_RC_INVALID_PAIR 
 *                                L7_CNFGR_ERR_RC_INVALID_CMD
 *                                L7_CNFGR_ERR_RC_INVALID_EVNT
 *                                L7_CNFGR_ERR_RC_INVALID_CMD_TYPE
 *
 * @notes    None.                                                     
 *       
 * @end
 *********************************************************************/
L7_BOOL cnfgrCCtlrInternalCommandIsValid(
    CNFGR_IN  L7_CNFGR_CMD_t command, 
    CNFGR_IN  L7_CNFGR_CMD_TYPE_t type, 
    CNFGR_IN  CNFGR_CTLR_EVNT_t event, 
    CNFGR_OUT L7_CNFGR_ERR_RC_t *pReason)
{
  /* set up variables and structures */
  L7_BOOL ctlRC = L7_FALSE;

  *pReason = L7_CNFGR_ERR_RC_INVALID_PAIR;

  /* validate command type */
  if (type == L7_CNFGR_EVNT) 
  {
    if (event > CNFGR_EVNT_FIRST && 
	event < CNFGR_EVNT_LAST) 
    {
      /* validate command/event pair */
      switch (command) {
	case L7_CNFGR_CMD_INITIALIZE:
	  switch (event) {
	    case CNFGR_EVNT_I_START:
	    case CNFGR_EVNT_I_PHASE1_CMPLT:
	    case CNFGR_EVNT_I_PHASE2_CMPLT:
	    case CNFGR_EVNT_I_MGMT_UNIT:
	    case CNFGR_EVNT_I_UNCONFIGURE_CMPLT:
	      ctlRC    = L7_TRUE;
	      break;
	    default:
	      /* invalid command/event pair */
	      break;
	  } /* endswitch initialize events */
	  break;
	case L7_CNFGR_CMD_EXECUTE:
	  if (event == CNFGR_EVNT_E_START) {
	    ctlRC    = L7_TRUE;
	  } /* endif execute event */
	  break;
	case L7_CNFGR_CMD_UNCONFIGURE:
	  switch (event) {
	    case CNFGR_EVNT_U_START:
	    case CNFGR_EVNT_U_PHASE1_CMPLT:
	      ctlRC    = L7_TRUE;
	      break;
	    default:
	      /* invalid command/event pair */
	      break;
	  } /* endswitch unconfigure events */ 
	  break;
	case L7_CNFGR_CMD_TERMINATE:
	  if (event == CNFGR_EVNT_T_START) {
	    ctlRC    = L7_TRUE;
	  } /* endif execute event */
	case L7_CNFGR_CMD_SUSPEND:
	  if (event == CNFGR_EVNT_S_START) {
	    ctlRC    = L7_TRUE;
	  } /* endif execute event */
	  break;
	case L7_CNFGR_CMD_RESUME:
	  if (event == CNFGR_EVNT_R_START) {
	    ctlRC    = L7_TRUE;
	  } /* endif execute event */
	  break;
	default:
	  *pReason = L7_CNFGR_ERR_RC_INVALID_CMD;
      } /* endswitch command/event pair */

    } else {
      *pReason = L7_CNFGR_ERR_RC_INVALID_EVNT;
    } /* endif validate event */

  } else {
    *pReason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;
  } /* endif validate command type */

  /* return value to caller */
  return (ctlRC);
}


/*
 *********************************************************************
 *                      CCTRL Interface functions 
 *********************************************************************
 */

/*********************************************************************
 * @purpose  This function is used by Tally to indicate that it has    
 *           completed the tally operation. This is a CCtlr interface.
 *
 * @param    ctHandle  - @b{(input)}  Handle of the completed tally. 
 * @param    nextState - @b{(input)}  Value to generate next message.
 * @param    pRspList  - @b{(input)}  list of responses received by tally.
 *
 * @returns  None.              
 *
 * @notes    None.                                                     
 *       
 * @end
 *********************************************************************/
void cnfgrCCtlrTallyComplete( CNFGR_IN CNFGR_CT_HANDLE_t   ctHandle,
    CNFGR_IN L7_CNFGR_STATE_t    nextState,
    CNFGR_IN L7_COMPONENT_IDS_t  nextCid,
    CNFGR_IN CNFGR_RSP_LIST_t   *pRspList
    )

{
  /* set up structures and variables */
  L7_int32              i;
  CNFGR_CR_COMPONENT_t *pComponent;
  CNFGR_CR_HANDLE_t     crHandle;
  CNFGR_CTLR_EVNT_t     nextEvent;
  L7_CNFGR_CB_HANDLE_t  cbHandle = pRspList->cbHandle;

  /* Issue completion response, if callback function registered */
  if (cbHandle != L7_CNFGR_NO_HANDLE)
    cnfgrCCtlrCompletionResponseIssue(pRspList->correlator, cbHandle, pRspList->aRsp);

  /* At this point some, none, or all components completed the cmd/rqst succesfully.
   * Now nextState is current for those that completed succesfully...Do it!
   */
  for (i = 0; i < pRspList->size; i++) {

    if (cnfgrCrComponentTake( pRspList->pRlEntry[i].cid, 
	  &crHandle, 
	  &pComponent ) == L7_SUCCESS) {
      /* set up component's attributes for query */
      (void)memcpy((void *)&pComponent->aRsp, 
		   (void *)&pRspList->pRlEntry[i].aRsp, 
		   sizeof( L7_CNFGR_ASYNC_RESPONSE_t )
		  );

      /* process error if any */
      if (pRspList->pRlEntry[i].aRsp.rc != L7_SUCCESS) {
	/* call error handling routine */

      } else {

	/* get component and update relevant fields */
	pComponent->currentState = nextState;

      } /* endif component failed cmd/rqst */

      /* all done here.  Release component */
      (void)cnfgrCrComponentGive( crHandle );

    } /* endif get component */

  } /* endfor process all components in response list */

  /* check general response and proceed to next step --
   *
   * pull next event to be generated internally, if any 
   *
   */ 
  if ( pRspList->aRsp.rc == L7_SUCCESS) 
  {

    nextEvent = nextState2NextInternalEvent[nextState % L7_CNFGR_STATE_NULL];

    /*
     * If the tallyHandle_g matches the globally stored value, then success
     * here indicates that all components have successfully taken the stored
     * nextState_g.  Update currentState_g.  nextState_g will be updated when
     * the new tally is opened for moving all components to the next state.
     */
    if ( tallyHandle_g == ctHandle )
    {
      currentState_g = nextState_g;
    }
  } 
  else 
  {
    /* call error handling routine */
    nextEvent = CNFGR_EVNT_T_START;

  } /* endif check for error */

  /*
   * If the tally which has just completed has the same handle as is stored in
   * tallyHandle_g, then set tallyHandle_g to NULL to indicate that there is
   * no outstanding tally.
   */
  if ( tallyHandle_g == ctHandle )
  {
    tallyHandle_g  = L7_NULL;
  }

  if (currentState_g == L7_CNFGR_STATE_P3)
    OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_P3_END, (L7_uchar8 *)L7_NULLPTR, 0);
  else if (currentState_g == L7_CNFGR_STATE_E)
    OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_EXECUTE_END, (L7_uchar8 *)L7_NULLPTR, 0);

  cnfgrTraceEvent(CNFGR_EVNT_FIRST, L7_FALSE);

  /* Stop the profiling for the current event */
  cnfgrProfileEventStop();    

  /* issue message to issue next cmd/request, if any */
  cnfgrCCtlrEventGenerate(nextCid, nextEvent);

  /* tell Tally we are done with this handle */
  cnfgrTallyClose(ctHandle);

  /* notify the event handler that this event processing has 
   * been completed
   */
  cnfgrMsgEventComplete();

  /* Return to caller */
  return;
}

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
void cnfgrCCtlrFini(void)
{

  /* Free the callback table resources
   * 
   */
  callbackTable.size    = 0; 
  callbackTable.maxSize = 0;

  if (callbackTable.lock != L7_NULLPTR) {

    (void)osapiSemaDelete( callbackTable.lock );
    callbackTable.lock = L7_NULLPTR;

  } /* endif callback table lock exists */

  if (callbackTable.pArray != (L7_VOIDFUNCPTR_t *)L7_NULL) {

    (void)osapiFree( L7_CNFGR_COMPONENT_ID, (void *)callbackTable.pArray );
    callbackTable.pArray = (L7_VOIDFUNCPTR_t *)L7_NULL;

  } /* endif callback table array exists */

  /* set CCTrl not ready not initialized */
  /* Return to caller */
  return;
}

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
L7_RC_t cnfgrCCtlrInitialize(void)
{
  int i;

  /* Initialize the callback table
   * 
   */
  callbackTable.size    = 0; 
  callbackTable.maxSize = CNFGR_CTRL_CALLBACK_MAX_f;

  currentState_g = L7_CNFGR_STATE_NULL;
  nextState_g    = L7_CNFGR_STATE_NULL;
  tallyHandle_g  = L7_NULL;

  if ((callbackTable.lock = osapiSemaMCreate(OSAPI_SEM_Q_FIFO)) == L7_NULLPTR)
    return (L7_ERROR);

  callbackTable.pArray =
    (L7_VOIDFUNCPTR_t *)osapiMalloc( L7_CNFGR_COMPONENT_ID, (L7_uint32)((callbackTable.maxSize+1)*sizeof(L7_VOIDFUNCPTR_t)));

  if (callbackTable.pArray != (L7_VOIDFUNCPTR_t *)L7_NULL ) {

    /* Initialize entries */
    for (i = 0; i < callbackTable.maxSize+1; i++)
      callbackTable.pArray[i] = (L7_VOIDFUNCPTR_t)L7_NULLPTR;

  } else return (L7_ERROR); /* endif NULL callback table array */


  /* set CCtlr initialized and ready in the Configurator control block */

  /* Return value to caller */
  return (L7_SUCCESS);
}


/*********************************************************************
 * @purpose  This function process command/events pairs. This function
 *           is a CCtlr interface.
 *
 * @param    pCmdData  - @b{(input)}  Contains the command/event pair
 *
 * @returns  None.              
 *
 * @notes    None.                                                     
 *       
 * @end
 *********************************************************************/
void cnfgrCCtlrProcessCommand(CNFGR_IN CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t            command;
  CNFGR_CTLR_EVNT_t         event;
  L7_CNFGR_CB_HANDLE_t      cbHandle;
  L7_CNFGR_CORRELATOR_t     correlator;
  L7_CNFGR_CMD_TYPE_t       type;
  L7_CNFGR_ASYNC_RESPONSE_t aRsp;
  L7_CNFGR_ERR_RC_t         reason;
  L7_COMPONENT_IDS_t        cid;

  /* validate command --
   *
   * ensure that the pCmdData is not null
   */
  if ( pCmdData != L7_NULLPTR ) {

    command    = pCmdData->command;
    event      = pCmdData->evntData.event;
    cid        = (L7_COMPONENT_IDS_t)pCmdData->evntData.data;
    correlator = pCmdData->correlator;
    cbHandle   = pCmdData->cbHandle;
    type       = pCmdData->type;

    if (cnfgrCCtlrInternalCommandIsValid( command, type, event, &reason) == L7_TRUE &&
	(cid >  L7_FIRST_COMPONENT_ID && cid <= L7_LAST_COMPONENT_ID) ) {

      /* valid command/event pair (ignore rc and reason) and valid component. 
       * Go and pProcess the event!
       * 
       * NOTE: completion response, if any, will be done by event receive function.
       */
      OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_COMP_ENTRY,
                       (L7_uchar8 *)&cid, sizeof(cid));
      cnfgrCCtlrEventReceive( pCmdData->evntData, correlator, cbHandle );
      OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_COMP_END,
                       (L7_uchar8 *)&cid, sizeof(cid));

    } else {
      /* Issue completion response, if callback function registered */
      if (cbHandle != L7_CNFGR_NO_HANDLE) {

	aRsp.rc       = L7_ERROR;
	aRsp.u.reason = reason;
	cnfgrCCtlrCompletionResponseIssue( correlator, cbHandle, aRsp );

      } /* endif callback function registered */

      /* notify the event handler that this event processing has 
       * been completed
       */
      cnfgrMsgEventComplete();

    } /* endif validate command */

  } else 

    /* notify the event handler that this event processing has 
     * been completed
     */
    cnfgrMsgEventComplete();

  /* endif command exist */

  /* return to caller */
  return;
}

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
void cnfgrCCtlrWarmRestartSet(L7_BOOL warmRestart)
{
  cnfgrWarmRestart = warmRestart;
}



