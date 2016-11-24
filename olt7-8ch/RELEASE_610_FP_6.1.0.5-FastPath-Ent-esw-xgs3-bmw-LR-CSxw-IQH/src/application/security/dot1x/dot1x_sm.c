/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_sm.c
*
* @purpose   dot1x State Machines
*
* @component dot1x
*
* @comments  none
*
* @create    04/08/2003
*
* @author    mfiorito
*
* @end
*             
**********************************************************************/

#include "dot1x_include.h"
#include "dot1ad_l2tp_api.h"

static DOT1X_APM_STATES_t apmStateTable[dot1xApmEvents][APM_STATES] =
{
/*Ev/St APM_INITIALIZE    APM_DISCONNECTED  APM_CONNECTING      APM_AUTHENTICATING APM_AUTHENTICATED  APM_ABORTING      APM_HELD          APM_FORCE_AUTH    APM_FORCE_UNAUTH */
/*E1*/ {APM_INITIALIZE,   APM_INITIALIZE,   APM_INITIALIZE,     APM_INITIALIZE,    APM_INITIALIZE,    APM_INITIALIZE,   APM_INITIALIZE,   APM_INITIALIZE,   APM_INITIALIZE   },
/*E2*/ {APM_INITIALIZE,   APM_INITIALIZE,   APM_INITIALIZE,     APM_INITIALIZE,    APM_INITIALIZE,    APM_INITIALIZE,   APM_INITIALIZE,   APM_INITIALIZE,   APM_INITIALIZE   },
/*E3*/ {APM_INITIALIZE,   APM_INITIALIZE,   APM_INITIALIZE,     APM_INITIALIZE,    APM_INITIALIZE,    APM_INITIALIZE,   APM_INITIALIZE,   APM_INITIALIZE,   APM_INITIALIZE   },
/*E4*/ {APM_STATES,       APM_STATES,       APM_DISCONNECTED,   APM_ABORTING,      APM_DISCONNECTED,  APM_DISCONNECTED, APM_STATES,       APM_STATES,       APM_STATES       },
/*E5*/ {APM_STATES,       APM_STATES,       APM_DISCONNECTED,   APM_STATES,        APM_STATES,        APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E6*/ {APM_STATES,       APM_STATES,       APM_STATES,         APM_STATES,        APM_STATES,        APM_STATES,       APM_CONNECTING,   APM_STATES,       APM_STATES       },
/*E7*/ {APM_STATES,       APM_STATES,       APM_CONNECTING,     APM_STATES,        APM_STATES,        APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E8*/ {APM_STATES,       APM_STATES,       APM_CONNECTING,     APM_STATES,        APM_STATES,        APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E9*/ {APM_STATES,       APM_STATES,       APM_CONNECTING,     APM_STATES,        APM_STATES,        APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E10*/{APM_STATES,       APM_STATES,       APM_AUTHENTICATING, APM_STATES,        APM_STATES,        APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E11*/{APM_STATES,       APM_STATES,       APM_STATES,         APM_STATES,        APM_STATES,        APM_CONNECTING,   APM_STATES,       APM_STATES,       APM_STATES       },
/*E12*/{APM_STATES,       APM_STATES,       APM_STATES,         APM_STATES,        APM_STATES,        APM_DISCONNECTED, APM_STATES,       APM_STATES,       APM_STATES       },
/*E13*/{APM_STATES,       APM_STATES,       APM_CONNECTING,     APM_ABORTING,      APM_CONNECTING,    APM_STATES,       APM_STATES,       APM_FORCE_AUTH,   APM_FORCE_UNAUTH },
/*E14*/{APM_STATES,       APM_STATES,       APM_STATES,         APM_ABORTING,      APM_CONNECTING,    APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E15*/{APM_STATES,       APM_STATES,       APM_STATES,         APM_AUTHENTICATED, APM_STATES,        APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E16*/{APM_STATES,       APM_STATES,       APM_STATES,         APM_HELD,          APM_STATES,        APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E17*/{APM_STATES,       APM_STATES,       APM_STATES,         APM_ABORTING,      APM_STATES,        APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E18*/{APM_FORCE_AUTH,   APM_FORCE_AUTH,   APM_FORCE_AUTH,     APM_FORCE_AUTH,    APM_FORCE_AUTH,    APM_FORCE_AUTH,   APM_FORCE_AUTH,   APM_FORCE_AUTH,   APM_FORCE_AUTH   },
/*E19*/{APM_FORCE_UNAUTH, APM_FORCE_UNAUTH, APM_FORCE_UNAUTH,   APM_FORCE_UNAUTH,  APM_FORCE_UNAUTH,  APM_FORCE_UNAUTH, APM_FORCE_UNAUTH, APM_FORCE_UNAUTH, APM_FORCE_UNAUTH },
/*E37*/{APM_STATES,       APM_STATES,       APM_STATES,         APM_STATES,        APM_HELD,          APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E40*/{APM_STATES,       APM_STATES,       APM_AUTHENTICATED,  APM_STATES,        APM_STATES,        APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E41*/{APM_STATES,       APM_STATES,       APM_STATES,         APM_AUTHENTICATED, APM_STATES,        APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E42*/{APM_STATES,       APM_STATES,       APM_STATES,         APM_STATES,        APM_DISCONNECTED,  APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E43*/{APM_STATES,       APM_STATES,       APM_STATES,         APM_STATES,        APM_DISCONNECTED,  APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E44*/{APM_STATES,       APM_STATES,       APM_AUTHENTICATING, APM_STATES,        APM_STATES,        APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       },
/*E45*/{APM_STATES,       APM_STATES,       APM_STATES,         APM_AUTHENTICATED, APM_STATES,        APM_STATES,       APM_STATES,       APM_STATES,       APM_STATES       }
};

static DOT1X_KTX_STATES_t ktxStateTable[dot1xKtxEvents][KTX_STATES] =
{
/*Ev/St KTX_NO_KEY_TRANSMIT   KTX_KEY_TRANSMIT */
/*E1*/ {KTX_NO_KEY_TRANSMIT,  KTX_NO_KEY_TRANSMIT },
/*E20*/{KTX_NO_KEY_TRANSMIT,  KTX_NO_KEY_TRANSMIT },
/*E21*/{KTX_KEY_TRANSMIT,     KTX_STATES          },
/*E22*/{KTX_STATES,           KTX_KEY_TRANSMIT    },
/*E23*/{KTX_STATES,           KTX_NO_KEY_TRANSMIT },
/*E24*/{KTX_STATES,           KTX_NO_KEY_TRANSMIT }
};

static DOT1X_RTM_STATES_t rtmStateTable[dot1xRtmEvents][RTM_STATES] =
{
/*Ev/St RTM_INITIALIZE      RTM_REAUTHENTICATE */
/*E1*/ {RTM_INITIALIZE,     RTM_INITIALIZE },
/*E20*/{RTM_INITIALIZE,     RTM_INITIALIZE },
/*E24*/{RTM_INITIALIZE,     RTM_INITIALIZE },
/*E25*/{RTM_INITIALIZE,     RTM_INITIALIZE },
/*E26*/{RTM_REAUTHENTICATE, RTM_STATES     }
};

static DOT1X_BAM_STATES_t bamStateTable[dot1xBamEvents][BAM_STATES] =
{
/*Ev/St BAM_REQUEST     BAM_RESPONSE    BAM_SUCCESS     BAM_FAIL        BAM_TIMEOUT     BAM_IDLE        BAM_INITIALIZE  */
/*E1*/ {BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE },
/*E20*/{BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE },
/*E27*/{BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE },
/*E28*/{BAM_REQUEST,    BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES     },
/*E29*/{BAM_RESPONSE,   BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES     },
/*E30*/{BAM_TIMEOUT,    BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES     },
/*E31*/{BAM_STATES,     BAM_SUCCESS,    BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES     },
/*E32*/{BAM_STATES,     BAM_FAIL,       BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES     },
/*E33*/{BAM_STATES,     BAM_TIMEOUT,    BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES     },
/*E34*/{BAM_STATES,     BAM_REQUEST,    BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES     },
/*E35*/{BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_RESPONSE,   BAM_STATES     },
/*E37*/{BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE, BAM_INITIALIZE },
/*E40*/{BAM_SUCCESS,    BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_SUCCESS,     BAM_STATES    },
/*E41*/{BAM_STATES,     BAM_SUCCESS,    BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_SUCCESS,     BAM_STATES    },
/*E45*/{BAM_STATES,     BAM_SUCCESS,    BAM_STATES,     BAM_STATES,     BAM_STATES,     BAM_SUCCESS,     BAM_STATES    }
};

static DOT1X_KRX_STATES_t krxStateTable[dot1xKrxEvents][KRX_STATES] =
{
/*Ev/St KRX_NO_KEY_RECEIVE  KRX_KEY_RECEIVE */
/*E1*/ {KRX_NO_KEY_RECEIVE, KRX_NO_KEY_RECEIVE },
/*E3*/ {KRX_NO_KEY_RECEIVE, KRX_NO_KEY_RECEIVE },
/*E36*/{KRX_KEY_RECEIVE,    KRX_KEY_RECEIVE    }
};

/* Supplicant PAE state machine (SPMS) */
static DOT1X_SPM_STATES_t spmStateTable[dot1xSpmEvents][DOT1X_SPM_STATES] =
{
 {DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED},
 {DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED},
 {DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED,DOT1X_SPM_DISCONNECTED},
 {DOT1X_SPM_STATES,DOT1X_SPM_LOGOFF,DOT1X_SPM_LOGOFF,DOT1X_SPM_LOGOFF,DOT1X_SPM_LOGOFF,DOT1X_SPM_LOGOFF,DOT1X_SPM_LOGOFF,DOT1X_SPM_LOGOFF,DOT1X_SPM_LOGOFF},
 {DOT1X_SPM_DISCONNECTED,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES},
 {DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_CONNECTING,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES},
 {DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_CONNECTING,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES},
 {DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_AUTHENTICATED,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES},
 {DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_RESTART,DOT1X_SPM_STATES,DOT1X_SPM_RESTART,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES},
 {DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_CONNECTING,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES},
 {DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_HELD,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES},
 {DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_AUTHENTICATED,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES},
 {DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_RESTART,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES},
 {DOT1X_SPM_FORCE_AUTH,DOT1X_SPM_FORCE_AUTH,DOT1X_SPM_FORCE_AUTH,DOT1X_SPM_FORCE_AUTH,DOT1X_SPM_FORCE_AUTH,DOT1X_SPM_FORCE_AUTH,DOT1X_SPM_FORCE_AUTH,DOT1X_SPM_STATES,DOT1X_SPM_FORCE_AUTH},
 {DOT1X_SPM_FORCE_UNAUTH,DOT1X_SPM_FORCE_UNAUTH,DOT1X_SPM_FORCE_UNAUTH,DOT1X_SPM_FORCE_UNAUTH,DOT1X_SPM_FORCE_UNAUTH,DOT1X_SPM_FORCE_UNAUTH,DOT1X_SPM_FORCE_UNAUTH,DOT1X_SPM_FORCE_UNAUTH,DOT1X_SPM_STATES},
 {DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_AUTHENTICATING,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES},
 {DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_AUTHENTICATING,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES,DOT1X_SPM_STATES}

};

/* Supplicant Backend state machine (SBMS) */
static DOT1X_SPM_STATES_t sbmStateTable[dot1xSbmEvents][DOT1X_SBM_STATES] =
{
 {DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE},
 {DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE},
 {DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE,DOT1X_SBM_INITIALIZE},
 {DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_REQUEST,DOT1X_SBM_STATES,DOT1X_SBM_STATES},
 {DOT1X_SBM_RESPONSE,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES},
 {DOT1X_SBM_RESPONSE,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES},
 {DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_TIMEOUT},
 {DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_FAIL,DOT1X_SBM_STATES,DOT1X_SBM_FAIL},
 {DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_SUCCESS,DOT1X_SBM_STATES,DOT1X_SBM_SUCCESS},
 {DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_STATES,DOT1X_SBM_REQUEST}
};


/*********************************************************************
* @purpose  This is the classifier which dispatches the received dot1x event to
*           a particular state machine 
*          
* @param   dot1xEvent  @b{(input)) event
* @param   intIfNum    @b{(input)) internal interface number
* @param   bufHandle   @b{(input)) network buffer handle
* @param   msg         @b{(input)) message containing event data
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xStateMachineClassifier(dot1xEvents_t dot1xEvent, L7_uint32 intIfNum,
                                    L7_netBufHandle bufHandle, void *msg)
{
  L7_RC_t rc;

  if (dot1xEvent < dot1xApmEvents)
  {
    rc = dot1xApmMachine(dot1xEvent, intIfNum, bufHandle);
  }
  else if (dot1xEvent < dot1xKtxEvents && dot1xEvent > dot1xApmEvents)
  {
    rc = dot1xKtxMachine(dot1xEvent, intIfNum, msg);
  }
  else if (dot1xEvent < dot1xRtmEvents && dot1xEvent > dot1xKtxEvents)
  {
    rc = dot1xRtmMachine(dot1xEvent, intIfNum, msg);
  }
  else if (dot1xEvent < dot1xBamEvents && dot1xEvent > dot1xRtmEvents)
  {
    rc = dot1xBamMachine(dot1xEvent, intIfNum, bufHandle);
  }
  else if (dot1xEvent < dot1xKrxEvents && dot1xEvent > dot1xBamEvents)
  {
    rc = dot1xKrxMachine(dot1xEvent, intIfNum, bufHandle);
  }
  else if (dot1xEvent < dot1xSpmEvents && dot1xEvent > dot1xKrxEvents)
  {
    rc = dot1xSpmMachine (dot1xEvent, intIfNum, bufHandle);
  }
  else if (dot1xEvent < dot1xSbmEvents && dot1xEvent > dot1xSpmEvents)
  {
    rc = dot1xSbmMachine (dot1xEvent, intIfNum, bufHandle);
  }
  else
    rc = L7_FAILURE;

  return rc;
}

/***************************************************************************/
/*******************************APM State Machine Routines******************/
/***************************************************************************/

/*********************************************************************
* @purpose  This routine calculates the next state for the APM state machine
*           and executes the action for that next state
*
* @param   dot1xEvent  @b{(input)) event
* @param   lIntIfNum    @b{(input)) Logical internal interface number
* @param   msg         @b{(input)) message containing event data
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xApmMachine(L7_uint32 dot1xEvent, L7_uint32 lIntIfNum, L7_netBufHandle bufHandle)
{
  DOT1X_APM_STATES_t nextState;
  L7_uint32 normalizedEvent;
  L7_BOOL freeBuffer = L7_TRUE;
  L7_RC_t rc = L7_FAILURE;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32 physPort;

  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  physPort = logicalPortInfo->physPort;

  normalizedEvent = dot1xEvent;

  nextState = apmStateTable[normalizedEvent][logicalPortInfo->apmState];

  DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,dot1xPhysPortGet(lIntIfNum),
                    "APM Machine for port - %d moving from %d to %d for event %d \n",lIntIfNum,
                    logicalPortInfo->apmState,nextState,normalizedEvent);
  switch (nextState)
  {
  case APM_INITIALIZE:
    rc = dot1xApmInitializeAction(logicalPortInfo);
    /* UCT to Disconnected State if port is operable */
    if (dot1xPortInfo[physPort].portEnabled == L7_TRUE)
    {
      rc = dot1xApmDisconnectedAction(logicalPortInfo);
      rc = dot1xApmGenerateEvents(logicalPortInfo, L7_NULLPTR);
      /* UCT to Connecting State */
      rc = dot1xApmConnectingAction(logicalPortInfo);
      rc = dot1xApmGenerateEvents(logicalPortInfo, L7_NULLPTR);
    }
    else
    {
      /* If port is not operable, set it to unauthorized */
      if (dot1xIhPortStatusSet(lIntIfNum, L7_DOT1X_PORT_STATUS_UNAUTHORIZED) != L7_SUCCESS)
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "dot1xApmMachine: could not set state to unauthorized, intf %u."
            " DTL call failed setting authorization state of the port or client (logical port)."
               , lIntIfNum);
      /*dot1xM acAddrInfoRemove(&(logicalPortInfo->suppMacAddr));*/
    }
    break;
  case APM_DISCONNECTED:
    rc = dot1xApmDisconnectedAction(logicalPortInfo);
    rc = dot1xApmGenerateEvents(logicalPortInfo, L7_NULLPTR);
    /* UCT to Connecting State */
    rc = dot1xApmConnectingAction(logicalPortInfo);
    rc = dot1xApmGenerateEvents(logicalPortInfo, L7_NULLPTR);
    break;
  case APM_CONNECTING:
    rc = dot1xApmConnectingAction(logicalPortInfo);
    rc = dot1xApmGenerateEvents(logicalPortInfo, L7_NULLPTR);
    break;
  case APM_AUTHENTICATING:
    rc = dot1xApmAuthenticatingAction(logicalPortInfo);
    rc = dot1xApmGenerateEvents(logicalPortInfo, (void *)bufHandle);
    freeBuffer = L7_FALSE;
    break;
  case APM_AUTHENTICATED:
    rc = dot1xApmAuthenticatedAction(logicalPortInfo);
    /* No need to generate events here */
    break;
  case APM_ABORTING:
    rc = dot1xApmAbortingAction(logicalPortInfo);
    rc = dot1xApmGenerateEvents(logicalPortInfo, L7_NULLPTR);
    break;
  case APM_HELD:
    rc = dot1xApmHeldAction(logicalPortInfo);
    /* No need to generate events here */
    break;
  case APM_FORCE_AUTH:
    rc = dot1xApmForceAuthAction(logicalPortInfo);
    /* No need to generate events here */
    break;
  case APM_FORCE_UNAUTH:
    rc = dot1xApmForceUnauthAction(logicalPortInfo);
    rc = dot1xApmGenerateEvents(logicalPortInfo, L7_NULLPTR);
    break;
  default:
      break;
  }

  if (freeBuffer == L7_TRUE && bufHandle != L7_NULL)
    SYSAPI_NET_MBUF_FREE(bufHandle);
  
  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the APM state INITIALIZE
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xApmInitializeAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  logicalPortInfo->currentIdL = 0;

  logicalPortInfo->apmState = APM_INITIALIZE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the APM state DISCONNECTED
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xApmDisconnectedAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_uint32 physPort,lIntIfNum;
  physPort = logicalPortInfo->physPort;
  lIntIfNum = logicalPortInfo->logicalPortNumber;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,physPort,"%s:Logical port-%d\n",
                      __FUNCTION__,lIntIfNum);

  if ( (logicalPortInfo->apmState == APM_CONNECTING) &&
       (logicalPortInfo->eapLogoff == L7_TRUE) )
  {
    dot1xPortStats[physPort].authEapLogoffsWhileConnecting++;
  }

  if ( (logicalPortInfo->apmState == APM_AUTHENTICATED) &&
       (logicalPortInfo->eapLogoff == L7_TRUE) )
  {
    dot1xPortStats[physPort].authAuthEapLogoffWhileAuthenticated++;
    if (dot1xRadiusAccountingStop(lIntIfNum, RADIUS_ACCT_TERM_CAUSE_USER_REQUEST) != L7_SUCCESS)
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
          "dot1xApmDisconnectedAction: failed sending terminate cause, intf %u."
          " Failed sending accounting stop to RADIUS server", lIntIfNum);
  }

  if (logicalPortInfo->reAuthenticating == L7_TRUE)
  {
      if (dot1xRadiusAccountingStop(lIntIfNum, RADIUS_ACCT_TERM_CAUSE_REAUTHENTICATION_FAILURE) != L7_SUCCESS)
          LOG_MSG("dot1xApmHeldAction reAuthenticating: failed sending terminate cause, intf %u\n", lIntIfNum);
      logicalPortInfo->reAuthenticating = L7_FALSE;
      if (logicalPortInfo->reauth_auth_apply == L7_TRUE) 
          logicalPortInfo->reauth_auth_apply = L7_FALSE;
            
  }

  if (logicalPortInfo->logicalPortStatus != L7_DOT1X_PORT_STATUS_UNAUTHORIZED)
  {
    if (dot1xIhPortStatusSet(lIntIfNum, L7_DOT1X_PORT_STATUS_UNAUTHORIZED) != L7_SUCCESS)
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
          "dot1xApmDisconnectedAction: could not set state to unauthorized, intf %u."
          " DTL call failed setting authorization state of the port or client (logical port)."
              , lIntIfNum);
  }

  logicalPortInfo->eapLogoff = L7_FALSE;
  logicalPortInfo->reAuthCount = 0;

  if (logicalPortInfo->apmState != APM_INITIALIZE)
  {
    dot1xTxCannedFail(lIntIfNum,DOT1X_LOGICAL_PORT);
  }


  if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO)
  {
    dot1xPortInfo[physPort].currentId = logicalPortInfo->currentIdL + 1;
  }
  logicalPortInfo->currentIdL = dot1xPortInfo[physPort].currentId;


  if (logicalPortInfo->inUse == L7_TRUE &&
      logicalPortInfo->apmState != APM_INITIALIZE)
  {

    /* Remove this from the L2 Table */
    if(logicalPortInfo->apmState == APM_CONNECTING && dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
    {
       dtlDot1xIntfClientUnblock(physPort,logicalPortInfo->suppMacAddr,logicalPortInfo->vlanId);
    }

   /*remove supplicant mac address from Mac address Database*/
    dot1xMacAddrInfoRemove(&(logicalPortInfo->suppMacAddr));
    if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO)
    {
        dot1xPortInfo[physPort].portMacAddrAdded  = L7_FALSE;
    }

    memset(logicalPortInfo->suppMacAddr.addr,0, L7_ENET_MAC_ADDR_LEN);
    logicalPortInfo->vlanId = 0;
   

    if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO)
    {
      dot1xPortInfo[physPort].currentId = logicalPortInfo->currentIdL;
      dot1xPortInfo[physPort].incCurrentId = L7_FALSE;
    }

    /*if (dot1xPortInfo[physPort].numUsers == dot1xPortInfo[physPort].maxUsers)
    {
      dot1xPortInfo[physPort].txWhenPort = dot1xPortInfo[physPort].txPeriod;
      dot1xTxReqId(physPort,DOT1X_PHYSICAL_PORT);
    }*/

    dot1xPortInfo[physPort].numUsers--;

    if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
    {
      logicalPortInfo->inUse = L7_FALSE;

      /* reset client timeout*/
      logicalPortInfo->clientTimeout = 0;
      if (logicalPortInfo->isMABClient == L7_TRUE)
            logicalPortInfo->isMABClient = L7_FALSE;
          
      if(dot1xLogicalPortInfoDeAlloc(logicalPortInfo) == L7_SUCCESS)
          return L7_SUCCESS;
      if ((dot1xPortInfo[physPort].portControlMode != L7_DOT1X_PORT_AUTO_MAC_BASED) && 
          (logicalPortInfo->inUse == L7_FALSE) )
      {
             logicalPortInfo->inUse = L7_TRUE;
      }
    }
  }
  logicalPortInfo->apmState = APM_DISCONNECTED;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the APM state CONNECTING
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xApmConnectingAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  dot1xPortCfg_t *pCfg;
  L7_uint32 physPort,lIntIfNum;

  physPort = logicalPortInfo->physPort;
  lIntIfNum = logicalPortInfo->logicalPortNumber;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,physPort,"%s:Logical port-%d\n",
                      __FUNCTION__,lIntIfNum);

  if (dot1xIntfIsConfigurable(physPort, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /* Count number of times we entered this state from any other state */
  if (logicalPortInfo->apmState != APM_CONNECTING)
    dot1xPortStats[physPort].authEntersConnecting++;

  if (logicalPortInfo->apmState == APM_AUTHENTICATED)
  {
    if (logicalPortInfo->reAuthenticate == L7_TRUE)
      dot1xPortStats[physPort].authAuthReauthsWhileAuthenticated++;
    else if (logicalPortInfo->eapStart == L7_TRUE)
      dot1xPortStats[physPort].authAuthEapStartsWhileAuthenticated++;

    if (logicalPortInfo->eapStart == L7_TRUE)
      logicalPortInfo->suppRestarting = L7_TRUE;
    else
    {
          logicalPortInfo->suppRestarting = L7_FALSE;
          if (logicalPortInfo->reauth_auth_apply == L7_TRUE) 
              logicalPortInfo->reauth_auth_apply = L7_FALSE;
          

    }

    if (logicalPortInfo->reAuthenticate == L7_TRUE)
      logicalPortInfo->reAuthenticating = L7_TRUE;
    else
    {
          logicalPortInfo->reAuthenticating = L7_FALSE;
          if (logicalPortInfo->reauth_auth_apply == L7_TRUE) 
              logicalPortInfo->reauth_auth_apply = L7_FALSE;
          

      }
  }

  logicalPortInfo->eapStart = L7_FALSE;
  logicalPortInfo->reAuthenticate = L7_FALSE;
  logicalPortInfo->txWhenLP = dot1xPortInfo[physPort].txPeriod;
  logicalPortInfo->rxRespId = L7_FALSE;

  logicalPortInfo->reAuthCount++; 
  if (logicalPortInfo->guestVlanTimer !=0)
  {
        if(logicalPortInfo->reAuthCount > dot1xPortInfo[physPort].reAuthMax)
        {
               logicalPortInfo->reAuthCount = 0;
               logicalPortInfo->currentIdL++;
        } 
  }


  if (logicalPortInfo->reAuthCount <= dot1xPortInfo[physPort].reAuthMax)
  {
    if (logicalPortInfo->logicalPortStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)
    {
        dot1xTxReqId(lIntIfNum, DOT1X_LOGICAL_PORT);
    }
    else
    {
      if (logicalPortInfo->inUse == L7_TRUE)
      {
        if (logicalPortInfo->apmState == APM_HELD ||
            logicalPortInfo->apmState == APM_ABORTING)
        {
          if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO)
          {
            dot1xPortInfo[physPort].currentId = logicalPortInfo->currentIdL;
            dot1xPortInfo[physPort].incCurrentId = L7_FALSE;
          }

         /* if (dot1xPortInfo[physPort].numUsers == dot1xPortInfo[physPort].maxUsers)
          {
            dot1xPortInfo[physPort].txWhenPort = dot1xPortInfo[physPort].txPeriod;
            dot1xTxReqId(physPort, DOT1X_PHYSICAL_PORT);
          }*/

          /*remove supplicant mac address from Mac address Database*/
          dot1xMacAddrInfoRemove(&(logicalPortInfo->suppMacAddr));

          /* delete the l2 entry in the table */
          dtlFdbMacAddrDelete(logicalPortInfo->suppMacAddr.addr, physPort, logicalPortInfo->vlanId,
                              L7_FDB_ADDR_FLAG_LEARNED, L7_NULL);

          if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO)
          {
              dot1xPortInfo[physPort].portMacAddrAdded  = L7_FALSE;
          }

          if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
          {
              logicalPortInfo->inUse = L7_FALSE;
          }
          memset(logicalPortInfo->suppMacAddr.addr,0, sizeof(L7_enetMacAddr_t));
          logicalPortInfo->vlanId = 0;
          if (logicalPortInfo->isMABClient == L7_TRUE)
            logicalPortInfo->isMABClient = L7_FALSE;
          /*dot1xLogicalPortInfo[intIfNum].physPort = 0;*/
          dot1xPortInfo[physPort].numUsers--;

          
         if(dot1xLogicalPortInfoDeAlloc(logicalPortInfo) == L7_SUCCESS)
              return L7_SUCCESS;

         if ((dot1xPortInfo[physPort].portControlMode != L7_DOT1X_PORT_AUTO_MAC_BASED) && (logicalPortInfo->inUse == L7_FALSE) )
         {
             logicalPortInfo->inUse = L7_TRUE;
         }

        }
        else
        {
          DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,physPort,"%s:Sending TxReqId Logical port-%d\n",
                              __FUNCTION__,lIntIfNum);
          dot1xTxReqId(lIntIfNum, DOT1X_LOGICAL_PORT);
        }
      }
    }
  }

  if(logicalPortInfo->apmState != APM_CONNECTING &&
     dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO)
  {
    if (dot1xIntfIsConfigurable(physPort, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    (void) dot1xCtlPortGuestVlanTimerStart(physPort,pCfg->guestVlanId,DOT1X_PHYSICAL_PORT);
  }

     
  


  logicalPortInfo->apmState = APM_CONNECTING;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the APM state AUTHENTICATING
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xApmAuthenticatingAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_uint32 physPort = logicalPortInfo->physPort;

  dot1xPortStats[physPort].authEntersAuthenticating++;

  logicalPortInfo->authSuccess = L7_FALSE;
  logicalPortInfo->authFail = L7_FALSE;
  logicalPortInfo->authTimeout = L7_FALSE;
  logicalPortInfo->authStart = L7_TRUE;

  logicalPortInfo->txWhenLP = 0; /* re-set EAPOL transmit timer */

  /*if(dot1xPortInfo[physPort].portControlMode != L7_DOT1X_PORT_AUTO_MAC_BASED)
    dot1xPortInfo[physPort].txWhenPort = 0; *//* re-set EAPOL transmit timer */

  logicalPortInfo->apmState = APM_AUTHENTICATING;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the APM state AUTHENTICATED
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xApmAuthenticatedAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_uint32  physPort = logicalPortInfo->physPort;

  dot1xPortStats[physPort].authAuthSuccessWhileAuthenticating++;

  /*Stop Guest VLAN Timer incase if it is already running*/
  if (logicalPortInfo->guestVlanTimer != 0)    
  {
    logicalPortInfo->guestVlanTimer = 0;
  }

  if (logicalPortInfo->reauth_auth_apply == L7_TRUE) 
  {
      /* set port to unauthorized to clear previous configuration */
      if (dot1xIhPortStatusSet(logicalPortInfo->logicalPortNumber, L7_DOT1X_PORT_STATUS_UNAUTHORIZED) != L7_SUCCESS)
    {
          LOG_MSG("dot1xApmAuthenticatedAction: could not set state to unauthorized for applying changes during reauthentication, intf %u\n", 
                   logicalPortInfo->logicalPortNumber);
    }
         
   }
   
  
  if (dot1xIhPortStatusSet(logicalPortInfo->logicalPortNumber, L7_DOT1X_PORT_STATUS_AUTHORIZED) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
        "dot1xApmAuthenticatedAction: could not set state to authorized, intf %u."
        " DTL call failed setting authorization state of the port or client (logical port).", 
            logicalPortInfo->logicalPortNumber);
  }

  if ((logicalPortInfo->reAuthenticating == L7_TRUE)|| 
      (logicalPortInfo->suppRestarting == L7_TRUE))   
  {
      if (logicalPortInfo->reauth_auth_apply == L7_TRUE)
    {
          logicalPortInfo->reauth_auth_apply = L7_FALSE;
    }

      if (logicalPortInfo->reAuthenticating == L7_TRUE) 
    {
          logicalPortInfo->reAuthenticating = L7_FALSE;
    }

      if(logicalPortInfo->suppRestarting == L7_TRUE)
    {
          logicalPortInfo->suppRestarting = L7_FALSE;
    }
  }


  logicalPortInfo->reAuthCount = 0;
  logicalPortInfo->currentIdL++;

  logicalPortInfo->txWhenLP = 0; /* re-set EAPOL transmit timer */
  /* set client timeout period if client is authenticated on a mac based port */
  if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)    
  {
    /* If radius has assigned a session timeout or if regular reauthentication
     * is enabled, then that will take care of client cleanup.  However,
     * reauthentication does not apply to voice, unauthorized or guest VLANs.
     */
    if ((logicalPortInfo->sessionTimeout == 0) &&
        ((dot1xPortInfo[physPort].reAuthEnabled == L7_FALSE) ||
         (logicalPortInfo->voiceVlanId != 0) ||
         (logicalPortInfo->unauthVlan  != 0) ||
         (logicalPortInfo->guestVlanId != 0)))
    {
      logicalPortInfo->clientTimeout = DOT1X_CLIENT_TIMEOUT;
    }
  }
  logicalPortInfo->apmState = APM_AUTHENTICATED;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the APM state ABORTING
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xApmAbortingAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_uint32  physPort = logicalPortInfo->physPort;

  if (logicalPortInfo->authTimeout == L7_TRUE)
    dot1xPortStats[physPort].authAuthTimeoutsWhileAuthenticating++;
  else if (logicalPortInfo->reAuthenticate == L7_TRUE)
    dot1xPortStats[physPort].authAuthReauthsWhileAuthenticating++;
  else if (logicalPortInfo->eapStart == L7_TRUE)
    dot1xPortStats[physPort].authAuthEapStartsWhileAuthenticating++;
  else if (logicalPortInfo->eapLogoff == L7_TRUE)
    dot1xPortStats[physPort].authAuthEapLogoffWhileAuthenticating++;

  logicalPortInfo->authAbort = L7_TRUE;
  logicalPortInfo->currentIdL++;

  logicalPortInfo->apmState = APM_ABORTING;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the APM state HELD
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xApmHeldAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_uint32  physPort = logicalPortInfo->physPort;
  L7_uint32  lIntIfNum = logicalPortInfo->logicalPortNumber;

  dot1xPortStats[physPort].authAuthFailWhileAuthenticating++;

  if (logicalPortInfo->logicalPortStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)
  {
    if (logicalPortInfo->suppRestarting == L7_TRUE)
    {
      if (dot1xRadiusAccountingStop(lIntIfNum,RADIUS_ACCT_TERM_CAUSE_SUPPLICANT_RESTART) != L7_SUCCESS)
        LOG_MSG("dot1xApmHeldAction suppRestarting: failed sending terminate cause, intf %u\n",lIntIfNum);
      logicalPortInfo->suppRestarting = L7_FALSE;
    }
    else
    {
      if (logicalPortInfo->reAuthenticating == L7_TRUE)
      {
        if (dot1xRadiusAccountingStop(lIntIfNum, RADIUS_ACCT_TERM_CAUSE_REAUTHENTICATION_FAILURE) != L7_SUCCESS)
          LOG_MSG("dot1xApmHeldAction reAuthenticating: failed sending terminate cause, intf %u\n", lIntIfNum);
        logicalPortInfo->reAuthenticating = L7_FALSE;
        if (logicalPortInfo->reauth_auth_apply == L7_TRUE) 
        {
             logicalPortInfo->reauth_auth_apply = L7_FALSE;
         }
       }
    }

    if (dot1xIhPortStatusSet(lIntIfNum, L7_DOT1X_PORT_STATUS_UNAUTHORIZED) != L7_SUCCESS)
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
          "dot1xApmHeldAction: could not set state to unauthorized, intf %u."
          " DTL call failed setting authorization state of the port or client (logical port)."
          , lIntIfNum);
  }

  logicalPortInfo->quietWhile = dot1xPortInfo[physPort].quietPeriod;
  logicalPortInfo->eapLogoff = L7_FALSE;
  logicalPortInfo->currentIdL++;

   /* reset client timeout*/
  if ((dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)&&
       (logicalPortInfo->clientTimeout != 0)) 
  {
      logicalPortInfo->clientTimeout = 0;
  }
  logicalPortInfo->apmState = APM_HELD;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the APM state FORCE_AUTHORIZED
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xApmForceAuthAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_uint32  lIntIfNum = logicalPortInfo->logicalPortNumber;
  L7_uint32  physPort = logicalPortInfo->physPort; 

  if ((dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED) &&
      (logicalPortInfo->inUse != L7_TRUE))
  {
      logicalPortInfo->inUse = L7_TRUE;
  }
  dot1xPortInfo[physPort].portControlMode = L7_DOT1X_PORT_FORCE_AUTHORIZED;

  if (dot1xIhPortStatusSet(lIntIfNum, L7_DOT1X_PORT_STATUS_AUTHORIZED) != L7_SUCCESS)
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
        "dot1xApmForceAuthAction: could not set state to authorized, intf %u."
        " DTL call failed setting authorization state of the port or client (logical port)."
            , lIntIfNum);

  
 
  logicalPortInfo->eapStart = L7_FALSE;
  dot1xTxCannedSuccess(lIntIfNum,DOT1X_LOGICAL_PORT);
  logicalPortInfo->currentIdL++;

  logicalPortInfo->apmState = APM_FORCE_AUTH;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the APM state FORCE_UNAUTHORIZED
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xApmForceUnauthAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_uint32  lIntIfNum = logicalPortInfo->logicalPortNumber;
  L7_uint32  physPort = logicalPortInfo->physPort; 

  if (logicalPortInfo->logicalPortStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)
  {
    if (dot1xIhPortStatusSet(lIntIfNum, L7_DOT1X_PORT_STATUS_UNAUTHORIZED) != L7_SUCCESS)
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
          "dot1xApmForceUnauthAction: could not set state to unauthorized, intf %u."
          " DTL call failed setting authorization state of the port or client (logical port)."
              , lIntIfNum);

    if (dot1xRadiusAccountingStop(lIntIfNum, RADIUS_ACCT_TERM_CAUSE_ADMIN_RESET) != L7_SUCCESS)
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
          "dot1xApmForceUnauthAction: failed sending terminate cause, intf %u."
          " Failed sending accounting stop to RADIUS server", lIntIfNum);
  }

  /*dot1xLogicalPortInfo[lIntIfNum].portMode = L7_DOT1X_PORT_FORCE_UNAUTHORIZED; */
   dot1xPortInfo[physPort].portControlMode = L7_DOT1X_PORT_FORCE_UNAUTHORIZED;
  logicalPortInfo->eapStart = L7_FALSE;
  dot1xTxCannedFail(lIntIfNum,DOT1X_LOGICAL_PORT);
  logicalPortInfo->currentIdL++;

  logicalPortInfo->apmState = APM_FORCE_UNAUTH;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose 	Events that the APM needs to generate and propagate
*          
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
* @param   *msg     @b{(input)) message containing event, bufHandle
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xApmGenerateEvents(dot1xLogicalPortInfo_t *logicalPortInfo, void *msg)
{
  L7_uint32      physPort = logicalPortInfo->physPort;
  L7_uint32  lIntIfNum = logicalPortInfo->logicalPortNumber;

  if (logicalPortInfo->apmState == APM_DISCONNECTED ||
      logicalPortInfo->apmState == APM_FORCE_UNAUTH)
  {

    (void)dot1xStateMachineClassifier(ktxPortStatusEqualUnauthorized, lIntIfNum, 
                                     L7_NULL, L7_NULLPTR);
    (void)dot1xStateMachineClassifier(rtmPortStatusEqualUnauthorized, lIntIfNum, 
                                     L7_NULL, L7_NULLPTR);
  }

  if (logicalPortInfo->apmState == APM_CONNECTING &&
      logicalPortInfo->reAuthCount > dot1xPortInfo[physPort].reAuthMax)
      (void) dot1xStateMachineClassifier(apmReAuthCountGreaterThanReAuthMax, lIntIfNum, 
                                       L7_NULL, L7_NULLPTR);

  if (logicalPortInfo->apmState == APM_AUTHENTICATING)
      (void) dot1xStateMachineClassifier(bamAuthStart, lIntIfNum,(L7_netBufHandle)msg, L7_NULLPTR);

  if (logicalPortInfo->apmState == APM_ABORTING)
      (void) dot1xStateMachineClassifier(bamAuthAbort, lIntIfNum, L7_NULL, L7_NULLPTR);

  return L7_SUCCESS;
}

/***************************************************************************/
/*******************************KTX State Machine Routines******************/
/***************************************************************************/

/*********************************************************************
* @purpose  This routine claculates the next state for the KTX state machine
*           and executes the action for that next state
*
* @param   dot1xEvent  @b{(input)) event
* @param   lIntIfNum @b{(input)) Logical interface number
* @param   msg         @b{(input)) message containing event data
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xKtxMachine(L7_uint32 dot1xEvent, L7_uint32 lIntIfNum, void *msg)
{
  DOT1X_KTX_STATES_t nextState;
  L7_uint32 normalizedEvent;
  L7_RC_t rc = L7_FAILURE;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  normalizedEvent = dot1xEvent - dot1xApmEvents - 1;

  nextState = ktxStateTable[normalizedEvent][logicalPortInfo->ktxState];

  switch (nextState)
  {
  case KTX_NO_KEY_TRANSMIT:
    rc = dot1xKtxNoKeyTransmitAction(logicalPortInfo);
    /* No need to generate events here */
    break;
  case KTX_KEY_TRANSMIT:
    rc = dot1xKtxKeyTransmitAction(logicalPortInfo);
    /* No need to generate events here */
    break;
  default:
      break;
  }

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the KTX state NO_KEY_TRANSMIT
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xKtxNoKeyTransmitAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  /* No actions required when in this state */

  logicalPortInfo->ktxState = KTX_NO_KEY_TRANSMIT;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the KTX state KEY_TRANSMIT
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xKtxKeyTransmitAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_RC_t rc;
  L7_uint32  physPort = logicalPortInfo->physPort;

  rc = dot1xTxKey(logicalPortInfo->logicalPortNumber);

  dot1xPortInfo[physPort].keyAvailable = L7_FALSE;
  logicalPortInfo->ktxState = KTX_KEY_TRANSMIT;

  return rc;
}

/***************************************************************************/
/*******************************RTM State Machine Routines******************/
/***************************************************************************/

/*********************************************************************
* @purpose  This routine claculates the next state for the RTM state machine
*           and executes the action for that next state
*
* @param   dot1xEvent  @b{(input)) event
* @param   lIntIfNum @b{(input)) Logical interface number
* @param   msg         @b{(input)) message containing event data
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xRtmMachine(L7_uint32 dot1xEvent, L7_uint32 lIntIfNum, void *msg)
{
  DOT1X_RTM_STATES_t nextState;
  L7_uint32 normalizedEvent;
  L7_RC_t rc = L7_FAILURE;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  normalizedEvent = dot1xEvent - dot1xKtxEvents - 1;

  nextState = rtmStateTable[normalizedEvent][logicalPortInfo->rtmState];

  DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,dot1xPhysPortGet(logicalPortInfo->logicalPortNumber),
                    "RTM Machine for port - %d moving from %d to %d for event %d \n",
                    logicalPortInfo->logicalPortNumber,
                    logicalPortInfo->rtmState,nextState,normalizedEvent);

  switch (nextState)
  {
  case RTM_INITIALIZE:
    rc = dot1xRtmInitializeAction(logicalPortInfo);
    /* No need to generate events here */
    break;
  case RTM_REAUTHENTICATE:
    rc = dot1xRtmReauthenticateAction(logicalPortInfo);
    rc = dot1xRtmGenerateEvents(logicalPortInfo, msg);
    /* UCT to Initialize State */
    rc = dot1xRtmInitializeAction(logicalPortInfo);
    break;
  default:
      break;
  }

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the RTM state INITIALIZE
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xRtmInitializeAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_uint32  physPort = logicalPortInfo->physPort;

  logicalPortInfo->reAuthWhen = dot1xPortInfo[physPort].reAuthPeriod;

  logicalPortInfo->rtmState = RTM_INITIALIZE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the RTM state REAUTHENTICATE
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xRtmReauthenticateAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  logicalPortInfo->reAuthenticate = L7_TRUE;

  logicalPortInfo->rtmState = RTM_REAUTHENTICATE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose 	Events that the APM needs to generate and propagate
*          
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
* @param   *msg     @b{(input)) message containing event, bufHandle
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xRtmGenerateEvents(dot1xLogicalPortInfo_t *logicalPortInfo, void *msg)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32  physPort = logicalPortInfo->physPort;

  if (logicalPortInfo->rtmState == RTM_REAUTHENTICATE)
  {
    if (logicalPortInfo->apmState == APM_CONNECTING && 
        logicalPortInfo->reAuthCount <= dot1xPortInfo[physPort].reAuthMax)
        rc = dot1xStateMachineClassifier(apmReauthAndReAuthCountLessThanEqualReAuthMax, 
                                         logicalPortInfo->logicalPortNumber, L7_NULL, L7_NULLPTR);
    else
    {

        rc = dot1xStateMachineClassifier(apmReAuthenticate, logicalPortInfo->logicalPortNumber, 
                                         L7_NULL, L7_NULLPTR);
          if (logicalPortInfo->isMABClient == L7_TRUE)
          {
            dot1xCtlLogicalPortMABOperational(logicalPortInfo->logicalPortNumber);
          }

     }
  }

  return rc;
}



/***************************************************************************/
/*******************************BAM State Machine Routines******************/
/***************************************************************************/

/*********************************************************************
* @purpose  This routine calculates the next state for the BAM state machine
*           and executes the action for that next state
*
* @param   dot1xEvent  @b{(input)) event
* @param   lIntIfNum @b{(input)) Logical internal interface number
* @param   msg         @b{(input)) message containing event data
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xBamMachine(L7_uint32 dot1xEvent, L7_uint32 lIntIfNum, L7_netBufHandle bufHandle)
{
  DOT1X_BAM_STATES_t nextState;
  L7_uint32 normalizedEvent;
  L7_RC_t rc = L7_FAILURE;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  normalizedEvent = dot1xEvent - dot1xRtmEvents - 1;

  nextState = bamStateTable[normalizedEvent][logicalPortInfo->bamState];

  DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,logicalPortInfo->physPort,
                    "BAM Machine for port - %d moving from %d to %d for event %d \n",
                    logicalPortInfo->logicalPortNumber,
                    logicalPortInfo->bamState,nextState,normalizedEvent);

  switch (nextState)
  {
  case BAM_REQUEST:
    rc = dot1xBamRequestAction(logicalPortInfo, bufHandle);
    /* No need to generate events here */
    break;
  case BAM_RESPONSE:
    rc = dot1xBamResponseAction(logicalPortInfo, bufHandle);
    /* No need to generate events here */
    break;
  case BAM_SUCCESS:
    rc = dot1xBamSuccessAction(logicalPortInfo);
    rc = dot1xBamGenerateEvents(logicalPortInfo, L7_NULLPTR);
    /* UCT to Idle State */
    rc = dot1xBamIdleAction(logicalPortInfo);
    /* No need to generate events here */
    break;
  case BAM_FAIL:
    rc = dot1xBamFailAction(logicalPortInfo);
    rc = dot1xBamGenerateEvents(logicalPortInfo, L7_NULLPTR);
    /* UCT to Idle State */
    rc = dot1xBamIdleAction(logicalPortInfo);
    /* No need to generate events here */
    break;
  case BAM_TIMEOUT:
    rc = dot1xBamTimeoutAction(logicalPortInfo);
    rc = dot1xBamGenerateEvents(logicalPortInfo, L7_NULLPTR);
    /* UCT to Idle State */
    rc = dot1xBamIdleAction(logicalPortInfo);
    /* No need to generate events here */
    break;
  case BAM_IDLE:
    rc = dot1xBamIdleAction(logicalPortInfo);
    /* No need to generate events here */
    break;
  case BAM_INITIALIZE:
    rc = dot1xBamInitializeAction(logicalPortInfo);
    rc = dot1xBamGenerateEvents(logicalPortInfo, L7_NULLPTR);
    /* UCT to Idle State */
    rc = dot1xBamIdleAction(logicalPortInfo);
    /* No need to generate events here */
    break;
  default:
      break;
  }

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the BAM state REQUEST
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xBamRequestAction(dot1xLogicalPortInfo_t *logicalPortInfo, L7_netBufHandle bufHandle)
{
  L7_uchar8 *dataStart, *suppDataStart;
  L7_uint32 length;
  L7_netBufHandle reXmitBufHandle;
  L7_uint32  physPort = logicalPortInfo->physPort;


  if (logicalPortInfo->bamState == BAM_RESPONSE)
    dot1xPortStats[physPort].authBackendAccessChallenges++;

  dot1xPortStats[physPort].authBackendOtherRequestsToSupplicant++;

  logicalPortInfo->currentIdL = logicalPortInfo->idFromServer;

  if (logicalPortInfo->isMABClient == L7_FALSE)
  {
  if (logicalPortInfo->bamState != BAM_REQUEST)
  {
    /* Send request to Supplicant */
    dot1xTxReq(logicalPortInfo, bufHandle);
  }
  else if (logicalPortInfo->suppBufHandle != L7_NULL)
  {
    /* Need to retransmit */
    reXmitBufHandle = sysapiNetMbufGet();
    if (reXmitBufHandle != L7_NULL)
    {
      SYSAPI_NET_MBUF_GET_DATASTART(logicalPortInfo->suppBufHandle, suppDataStart);
      SYSAPI_NET_MBUF_GET_DATALENGTH(logicalPortInfo->suppBufHandle, length);
      SYSAPI_NET_MBUF_GET_DATASTART(reXmitBufHandle, dataStart);
      memcpy(dataStart, suppDataStart, length);
      SYSAPI_NET_MBUF_SET_DATALENGTH(reXmitBufHandle, length);

      /* Free suppBufHandle as another one will be allocated in dot1xTxReq */
      SYSAPI_NET_MBUF_FREE(logicalPortInfo->suppBufHandle);
      logicalPortInfo->suppBufHandle = L7_NULL;

      /* Re-send request to Supplicant */
      dot1xTxReq(logicalPortInfo, reXmitBufHandle);
    }
    else
    {
        L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
          "dot1xBamRequestAction: Out of System buffers."
          " 802.1X cannot process/transmit message due to lack of internal buffers");
      }
    }
  }
  


  logicalPortInfo->aWhile = dot1xPortInfo[physPort].suppTimeout;
  logicalPortInfo->reqCount++;

  logicalPortInfo->bamState = BAM_REQUEST;

  if (logicalPortInfo->isMABClient == L7_TRUE)
  {
       dot1xCtlLogicalPortMABGenResp(logicalPortInfo->logicalPortNumber);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the BAM state RESPONSE
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xBamResponseAction(dot1xLogicalPortInfo_t *logicalPortInfo, L7_netBufHandle bufHandle)
{
  dot1xPortCfg_t * pCfg;
  L7_USER_MGR_AUTH_METHOD_t authMethod;
  L7_uint32  physPort = logicalPortInfo->physPort;
  L7_uint32  lIntIfNum = logicalPortInfo->logicalPortNumber;

  if (dot1xIntfIsConfigurable(physPort, &pCfg) != L7_TRUE)
    return L7_FAILURE;
 
  dot1xPortStats[physPort].authBackendResponses++;

  logicalPortInfo->authTimeout = L7_FALSE;
  logicalPortInfo->rxResp = L7_FALSE;
  logicalPortInfo->aWhile = dot1xPortInfo[physPort].serverTimeout;
  logicalPortInfo->reqCount = 0;

  logicalPortInfo->bamState = BAM_RESPONSE;

  if (logicalPortInfo->suppBufHandle != L7_NULL)
  {
    SYSAPI_NET_MBUF_FREE(logicalPortInfo->suppBufHandle);
    logicalPortInfo->suppBufHandle = L7_NULL;
  }

  if (dot1xSmAuthMethodGet(logicalPortInfo, bufHandle, &authMethod) == L7_SUCCESS)
  {
    switch (authMethod) 
    {
    case L7_AUTH_METHOD_UNDEFINED:
    case L7_AUTH_METHOD_REJECT:
      SYSAPI_NET_MBUF_FREE(bufHandle);
      /* reset port in any dot1x assigned vlans*/
      dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);
      (void) dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
       break;

    case L7_AUTH_METHOD_LOCAL:
      /* attempt local authentication */
      (void)dot1xLocalAuthResponseProcess(logicalPortInfo, bufHandle);
      break;

    case L7_AUTH_METHOD_RADIUS:
        /* attempt authentication with RADIUS */  
      (void)dot1xRadiusSuppResponseProcess(lIntIfNum, bufHandle);
        break;
    case L7_AUTH_METHOD_NONE:
        /* authorize */
        SYSAPI_NET_MBUF_FREE(bufHandle);
        (void) dot1xStateMachineClassifier(bamASuccess, lIntIfNum, L7_NULL, L7_NULLPTR);
        break;

    default:
      SYSAPI_NET_MBUF_FREE(bufHandle);
        /* reset port in any dot1x assigned vlans*/
      dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);
      (void) dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
      break;
    }
  }
  else
  {
    /* If we couldn't get an auth method, log an error and fail the authentication */
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
        "dot1xBamResponseAction: failed getting auth method, intf %u."
        " Could not determine the authentication method to be used . "
        "Probably because of  a mis-configuration.", lIntIfNum);
    /* reset port in any dot1x assigned vlans*/
    dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);
    (void) dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the BAM state SUCCESS
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xBamSuccessAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_uint32  physPort = logicalPortInfo->physPort;

  dot1xPortStats[physPort].authBackendAuthSuccesses++;

  logicalPortInfo->currentIdL = logicalPortInfo->idFromServer;
  dot1xTxCannedSuccess(logicalPortInfo->logicalPortNumber, DOT1X_LOGICAL_PORT);
  logicalPortInfo->authSuccess = L7_TRUE;

 
  logicalPortInfo->bamState = BAM_SUCCESS;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the BAM state FAIL
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xBamFailAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_uint32  physPort = logicalPortInfo->physPort;

  dot1xPortStats[physPort].authBackendAuthFails++;

  logicalPortInfo->currentIdL = logicalPortInfo->idFromServer;
  dot1xTxCannedFail(logicalPortInfo->logicalPortNumber,DOT1X_LOGICAL_PORT);
  logicalPortInfo->authFail = L7_TRUE;

  logicalPortInfo->bamState = BAM_FAIL;
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the BAM state TIMEOUT
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xBamTimeoutAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  if (logicalPortInfo->logicalPortStatus == L7_DOT1X_PORT_STATUS_UNAUTHORIZED)
    dot1xTxCannedFail(logicalPortInfo->logicalPortNumber,DOT1X_LOGICAL_PORT);
    
  logicalPortInfo->authTimeout = L7_TRUE;

  logicalPortInfo->bamState = BAM_TIMEOUT;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the BAM state IDLE
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xBamIdleAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  logicalPortInfo->authStart = L7_FALSE;
  logicalPortInfo->reqCount = 0;

  logicalPortInfo->bamState = BAM_IDLE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the BAM state INITIALZE
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t dot1xBamInitializeAction(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  dot1xAbortAuth(logicalPortInfo);
  logicalPortInfo->authAbort = L7_FALSE;

  logicalPortInfo->bamState = BAM_INITIALIZE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose 	Events that the BAM needs to generate and propagate
*          
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
* @param   *msg     @b{(input)) message containing event, bufHandle
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t dot1xBamGenerateEvents(dot1xLogicalPortInfo_t *logicalPortInfo, void *msg)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 lIntIfNum = logicalPortInfo->logicalPortNumber;

  if (logicalPortInfo->bamState == BAM_SUCCESS)
    rc = dot1xStateMachineClassifier(apmAuthSuccess, lIntIfNum, L7_NULL, L7_NULLPTR);

  if (logicalPortInfo->bamState == BAM_FAIL) 
    rc = dot1xStateMachineClassifier(apmAuthFail, lIntIfNum, L7_NULL, L7_NULLPTR);


  if (logicalPortInfo->bamState == BAM_TIMEOUT)
    rc = dot1xStateMachineClassifier(apmAuthTimeout, lIntIfNum, L7_NULL, L7_NULLPTR);

  if (logicalPortInfo->bamState == BAM_INITIALIZE)
  {
    if (logicalPortInfo->eapLogoff == L7_TRUE)
      rc = dot1xStateMachineClassifier(apmEapLogoffAndNotAuthAbort, lIntIfNum, 
          L7_NULL, L7_NULLPTR);
    else
      rc = dot1xStateMachineClassifier(apmNotEapLogoffAndNotAuthAbort, lIntIfNum, 
          L7_NULL, L7_NULLPTR);
  }

  return rc;
}

/***************************************************************************/
/*******************************KRX State Machine Routines******************/
/***************************************************************************/

/*********************************************************************
* @purpose  This routine claculates the next state for the KRX state machine
*           and executes the action for that next state
*
* @param   dot1xEvent  @b{(input)) event
* @param   lIntIfNum @b{(input)) Logical internal interface number
* @param   msg         @b{(input)) message containing event data
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t dot1xKrxMachine(L7_uint32 dot1xEvent, L7_uint32 lIntIfNum, L7_netBufHandle bufHandle)
{
  DOT1X_KRX_STATES_t nextState;
  L7_uint32 normalizedEvent;
  L7_RC_t rc = L7_FAILURE;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  normalizedEvent = dot1xEvent - dot1xBamEvents - 1;

  nextState = krxStateTable[normalizedEvent][logicalPortInfo->krxState];

  switch (nextState)
  {
  case KRX_NO_KEY_RECEIVE:
    rc = dot1xKrxNoKeyReceiveAction(logicalPortInfo);
    /* No need to generate events here */
    break;
  case KRX_KEY_RECEIVE:
    rc = dot1xKrxKeyReceiveAction(logicalPortInfo, bufHandle);
    /* No need to generate events here */
    break;
  default:
      break;
  }

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the KRX state NO_KEY_RECEIVE
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t dot1xKrxNoKeyReceiveAction(dot1xLogicalPortInfo_t *logicalPortInf)
{
  /* No actions required when in this state */

  logicalPortInf->krxState = KRX_NO_KEY_RECEIVE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the KRX state KEY_RECEIVE
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns L7_SUCCESS  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t dot1xKrxKeyReceiveAction(dot1xLogicalPortInfo_t *logicalPortInf, L7_netBufHandle bufHandle)
{
  L7_RC_t rc;

  rc = dot1xProcessKey(logicalPortInf->logicalPortNumber, bufHandle);

  logicalPortInf->rxKey = L7_FALSE;
  logicalPortInf->krxState = KRX_KEY_RECEIVE;

  return rc;
}

/*********************************************************************
* @purpose  Abort current authorization session for specified interface
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns void
*
* @comments 
*
* @end
*********************************************************************/
void dot1xAbortAuth(dot1xLogicalPortInfo_t *logicalPortInf)
{
  if (logicalPortInf->suppBufHandle != L7_NULL)
  {
    SYSAPI_NET_MBUF_FREE(logicalPortInf->suppBufHandle);
    logicalPortInf->suppBufHandle = L7_NULL;
  }

  memset(logicalPortInf->dot1xUserName,0, DOT1X_USER_NAME_LEN);
  logicalPortInf->dot1xUserIndex = 0;
  memset(logicalPortInf->dot1xChallenge,0, DOT1X_CHALLENGE_LEN);
  memset(logicalPortInf->serverState,0, DOT1X_SERVER_STATE_LEN);
  logicalPortInf->serverStateLen = 0;
  memset(logicalPortInf->serverClass,0, DOT1X_SERVER_CLASS_LEN);
  logicalPortInf->serverClassLen = 0;
  logicalPortInf->sessionTimeout = 0;
  logicalPortInf->terminationAction = RADIUS_TERMINATION_ACTION_DEFAULT;
  logicalPortInf->authMethod = L7_AUTH_METHOD_UNDEFINED;

  return;
}

/*********************************************************************
* @purpose Get the authentication method for the user of this port
*
* @param   logicalPortInfo  @b{(input))  Logical Port Info node
* @param   bufHandle  @b{(input)} the handle to the dot1x PDU
* @param   authMethod @b{(output)} user's authentication method
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t dot1xSmAuthMethodGet(dot1xLogicalPortInfo_t *logicalPortInf, 
                             L7_netBufHandle bufHandle, 
                             L7_USER_MGR_AUTH_METHOD_t *authMethod)
{
  L7_uchar8 *data;
  L7_enetHeader_t *enetHdr;
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  L7_eapRrPacket_t *eapRrPkt=L7_NULLPTR;
  L7_char8 aplName[L7_MAX_APL_NAME_SIZE +1];
  L7_uint32 i, method;
  L7_BOOL authenticationResolved = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;

  if (bufHandle != L7_NULL)
  {
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  enetHdr = (L7_enetHeader_t *)data;   
  eapolPkt = (L7_eapolPacket_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE);
  eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)eapolPkt + sizeof(L7_eapolPacket_t));
  eapRrPkt = (L7_eapRrPacket_t *)((L7_uchar8 *)eapPkt + sizeof(L7_eapPacket_t));
  }

  /* User name was stored in port info structure when EAP-Response/Identity frame was received. */
  if ((logicalPortInf->isMABClient == L7_TRUE)||
      ((logicalPortInf->isMABClient == L7_FALSE )&&(eapRrPkt!=L7_NULLPTR)&& (eapRrPkt->type == EAP_RRIDENTITY)))
  {
    memset(aplName, 0, sizeof(aplName));

#if 0 /* This is no longer required since APL's are not associated with users */
    /* userName in port structure was filled in by dot1xEapPktProcess() */
    if (userMgrAPLUserGet((L7_char8 *)logicalPortInf->dot1xUserName,
                          L7_DOT1X_COMPONENT_ID, aplName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
#endif

    for (i=0; i<L7_MAX_AUTH_METHODS; i++) /* check all methods; break when we find first valid */
    {
      if (userMgrAPLAuthMethodGet(ACCESS_LINE_DOT1X,0,aplName, i, &method) == L7_FAILURE) 
      {
         return L7_FAILURE;
      }
      switch (method) 
      {
      case L7_AUTH_METHOD_UNDEFINED:
        /* skip and go to next method */
        break;

      case L7_AUTH_METHOD_LOCAL:
        /* attempt local authentication */
        *authMethod = L7_AUTH_METHOD_LOCAL;
        authenticationResolved = L7_TRUE;
        break;

      case L7_AUTH_METHOD_RADIUS:
        /* attempt authentication with RADIUS */  
        *authMethod = L7_AUTH_METHOD_RADIUS;
        authenticationResolved = L7_TRUE;
        break;

      case L7_AUTH_METHOD_REJECT:
        /* this user is never authenticated */
        *authMethod = L7_AUTH_METHOD_REJECT;
        authenticationResolved = L7_TRUE;
          break;

        case L7_AUTH_METHOD_NONE:
          /* this user is always authenticated */
          *authMethod = L7_AUTH_METHOD_NONE;
          authenticationResolved = L7_TRUE;
          break;

      default:
        /* try the next method */
        break;
      }

      if (L7_TRUE == authenticationResolved)
      {
        logicalPortInf->authMethod = *authMethod;
        rc = L7_SUCCESS;
        break;
      }
    }
  }
  else
  {
    if (logicalPortInf->authMethod != L7_AUTH_METHOD_UNDEFINED)
    {
      *authMethod = logicalPortInf->authMethod;
      rc = L7_SUCCESS;
    }
    else
      rc = L7_FAILURE;
  }

  return rc;
}

/* Supplicant state machine (SPMS)*/

/* Mimic the Port down event for all other ports till the Supplicant
   port becomes authenticated*/
L7_RC_t dot1xDisableAuthenticatorPorts(intIfNum)
{
  
  L7_uint32 authIntf=0;
  L7_uint32 lIntIfNum=0;
  dot1xPortCfg_t *pCfg;
  dot1xLogicalPortInfo_t *logicalPortInfo;

   for (authIntf = 1; authIntf < L7_DOT1X_INTF_MAX_COUNT; authIntf++)
   {
     if (authIntf != intIfNum)
     {
        if (dot1xIntfIsConfigurable(authIntf, &pCfg) == L7_TRUE)
        {
          lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
          while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(authIntf,&lIntIfNum))!= L7_NULLPTR)
          {
            if(logicalPortInfo->inUse != L7_TRUE)
            continue;
           (void) dot1xStateMachineClassifier(apmNotPortEnabled, lIntIfNum, L7_NULL, L7_NULLPTR);
           (void) dot1xStateMachineClassifier(krxNotPortEnabled, lIntIfNum, L7_NULL, L7_NULLPTR);
           if (dot1xRadiusAccountingStop(lIntIfNum, RADIUS_ACCT_TERM_CAUSE_LOST_CARRIER) != L7_SUCCESS)
             LOG_MSG("dot1xIhProcessIntfChange L7_DOWN: failed sending terminate cause, intf %u\n", authIntf);
          }
        /* check if the port is vlan assigned */
        dot1xPortVlanAssignmentDisable(authIntf);
        if(pCfg->guestVlanId !=0)
        {
          dot1xCtlPortGuestVlanRemove(authIntf,pCfg->guestVlanId);
        }
       /* reset operational info on the port*/
        (void)dot1xPortInfoInitialize(authIntf,L7_TRUE);
        dot1xPortInfo[authIntf].portEnabled = L7_FALSE;
        (void)dot1xCtlApplyPortConfigData(authIntf);
      }
     }
   }

  return L7_SUCCESS;
}
/* Mimic the L7_UP event for the authenticator */
L7_RC_t dot1xEnableAuthenticatorPorts(intIfNum)
{
  L7_uint32 authIntf=0;
  L7_uint32 adminState;
  L7_uint32 linkState;
  dot1xPortCfg_t *pCfg;

  for (authIntf = 1; authIntf < L7_DOT1X_INTF_MAX_COUNT; authIntf++)
  {
    if (authIntf == intIfNum)
    continue;

    if (dot1xIntfIsConfigurable(authIntf, &pCfg) != L7_TRUE)
    continue;

    if ( (nimGetIntfAdminState(authIntf, &adminState) == L7_SUCCESS) &&
         (adminState == L7_ENABLE) &&
         (nimGetIntfLinkState(authIntf, &linkState) == L7_SUCCESS) &&
         (linkState == L7_UP) )
    {
        dot1xPortInfo[authIntf].portEnabled = L7_TRUE;
       (void)dot1xPortInfoInitialize(authIntf,L7_TRUE);
       (void)dot1xCtlApplyPortConfigData(authIntf);
    }

  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the SPM state DISCONNECTED
*
* @param   intIfNum @b{(input)) internal interface number
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSpmDisconnectedAction(L7_uint32 intIfNum)
{

  dot1xSupplicantPortInfo[intIfNum].sPortMode = L7_DOT1X_PORT_AUTO;
  dot1xSupplicantPortInfo[intIfNum].startCount = 0;
  dot1xSupplicantPortInfo[intIfNum].logoffSent = L7_FALSE;
  dot1xSupplicantPortInfo[intIfNum].supplicantPortStatus =
                                         L7_DOT1X_PORT_STATUS_UNAUTHORIZED;
  dot1xSupplicantPortInfo[intIfNum].suppAbort = L7_TRUE;
  dot1xSupplicantPortInfo[intIfNum].spmState = DOT1X_SPM_DISCONNECTED;

  dot1xIhPhyPortStatusSet(intIfNum,L7_DOT1X_PORT_STATUS_UNAUTHORIZED,
                                  L7_TRUE);

   /* Abort the BAM state machine */
   (void) dot1xStateMachineClassifier(sbmSuppAbort, intIfNum, L7_NULL, L7_NULLPTR);

   #if defined L7_DOT1AD_PACKAGE
   if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,
                              L7_FEAT_DOT1X_SUPPLICANT_CONTROL_ON_AUTH_PORTS))
   {
     
       DOT1AD_INTFERFACE_TYPE_t  intfType;

      dot1adInterfaceTypeGet(intIfNum, &intfType);
      if(intfType == DOT1AD_INTFERFACE_TYPE_NNI)
      {
        if (dot1xSupplicantPortInfo[intIfNum].portEnabled == L7_TRUE)
        {
          dot1xDisableAuthenticatorPorts (intIfNum);
        }
        if (dot1xSupplicantPortInfo[intIfNum].portEnabled == L7_FALSE)
        {
         dot1xSupplicantPortInfo[intIfNum].supplicantPortStatus =
                                         L7_DOT1X_PORT_STATUS_AUTHORIZED;

          dot1xIhPhyPortStatusSet(intIfNum,L7_DOT1X_PORT_STATUS_AUTHORIZED,
                                  L7_TRUE);
          dot1xEnableAuthenticatorPorts (intIfNum);
        }

      }
   }
#endif

  return L7_SUCCESS;
  
}

/*********************************************************************
* @purpose  Actions to be performed in the DISCONNECTED State
*
* @param   intIfNum @b{(input)) internal interface number
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSpmConnectingAction(L7_uint32 intIfNum)
{
  dot1xPortCfg_t *pCfg;

  if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  dot1xSupplicantPortInfo[intIfNum].startCount ++ ;
  if (dot1xSupplicantPortInfo[intIfNum].startCount > pCfg->supplicantPortCfg.maxStart)
  {
    (void)dot1xStateMachineClassifier(spmStartWhenZeroStartCountntEqualMaxStart, intIfNum,
                                     L7_NULL, L7_NULLPTR); 
    return L7_SUCCESS;
  }
  dot1xSupplicantPortInfo[intIfNum].startWhen = pCfg->supplicantPortCfg.startPeriod;
  dot1xSupplicantPortInfo[intIfNum].eapolEap = L7_FALSE;
  dot1xSupplicantPortInfo[intIfNum].spmState = DOT1X_SPM_CONNECTING;

  /* Send Start Message to the Authenticator */
  dot1xTxStart(intIfNum);

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Actions to be performed in the SPM state FORCE AUTHORIZED 
*
* @param   intIfNum @b{(input)) internal interface number
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSpmForceAuthAction(L7_uint32 intIfNum)
{
  dot1xSupplicantPortInfo[intIfNum].sPortMode = L7_DOT1X_PORT_FORCE_AUTHORIZED;
  dot1xSupplicantPortInfo[intIfNum].supplicantPortStatus =
                                         L7_DOT1X_PORT_STATUS_AUTHORIZED;
  dot1xSupplicantPortInfo[intIfNum].spmState = DOT1X_SPM_FORCE_AUTH;

  dot1xIhPhyPortStatusSet(intIfNum,L7_DOT1X_PORT_STATUS_AUTHORIZED,
                                  L7_TRUE);
    #if defined L7_DOT1AD_PACKAGE
    if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,
                              L7_FEAT_DOT1X_SUPPLICANT_CONTROL_ON_AUTH_PORTS))
  {
    DOT1AD_INTFERFACE_TYPE_t  intfType;
    dot1adInterfaceTypeGet(intIfNum, &intfType);
    if(intfType == DOT1AD_INTFERFACE_TYPE_NNI)
    {  
      dot1xEnableAuthenticatorPorts(intIfNum);
    }
  }
#endif

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Actions to be performed in the SPM state FORCE UnAUTHORIZED
*
* @param   intIfNum @b{(input)) internal interface number
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t dot1xSpmForceUnAuthAction(L7_uint32 intIfNum)
{
  dot1xSupplicantPortInfo[intIfNum].sPortMode = L7_DOT1X_PORT_FORCE_UNAUTHORIZED;
  dot1xSupplicantPortInfo[intIfNum].supplicantPortStatus =
                                         L7_DOT1X_PORT_STATUS_UNAUTHORIZED;
  dot1xSupplicantPortInfo[intIfNum].spmState = DOT1X_SPM_FORCE_UNAUTH;

  dot1xIhPhyPortStatusSet(intIfNum,L7_DOT1X_PORT_STATUS_UNAUTHORIZED,
                                  L7_TRUE);
  #if defined L7_DOT1AD_PACKAGE
  if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,
                              L7_FEAT_DOT1X_SUPPLICANT_CONTROL_ON_AUTH_PORTS))
  {
    
    DOT1AD_INTFERFACE_TYPE_t  intfType;
    dot1adInterfaceTypeGet(intIfNum, &intfType);
    if(intfType == DOT1AD_INTFERFACE_TYPE_NNI)
    {
      if (dot1xSupplicantPortInfo[intIfNum].portEnabled == L7_TRUE)
      {
        dot1xDisableAuthenticatorPorts (intIfNum);
      }
    }
  }
#endif

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Actions to be performed in the SPM state HELD 
*
* @param   intIfNum  @b{(input)) internal interface number
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t dot1xSpmHeldAction(L7_uint32 intIfNum)
{
  dot1xPortCfg_t *pCfg;

  if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  dot1xSupplicantPortInfo[intIfNum].heldWhile =  pCfg->supplicantPortCfg.heldPeriod;
  dot1xSupplicantPortInfo[intIfNum].supplicantPortStatus =
                                         L7_DOT1X_PORT_STATUS_UNAUTHORIZED;
  dot1xSupplicantPortInfo[intIfNum].spmState = DOT1X_SPM_HELD;

  return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Actions to be performed in the SPM state AUTHENTICATING 
*
* @param   intIfNum  @b{(input)) internal interface number
* @param   bufHandle @b{(input)) Buffer Handler to the EAP Packet
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t dot1xSpmAuthenticatingAction(L7_uint32 intIfNum,
                                      L7_netBufHandle bufHandle)
{
  dot1xSupplicantPortInfo[intIfNum].startCount = 0;
  dot1xSupplicantPortInfo[intIfNum].suppSuccess = L7_FALSE;
  dot1xSupplicantPortInfo[intIfNum].suppFail = L7_FALSE;
  dot1xSupplicantPortInfo[intIfNum].suppTimeout = L7_FALSE;
  dot1xSupplicantPortInfo[intIfNum].suppStart = L7_TRUE;
  dot1xSupplicantPortInfo[intIfNum].spmState = DOT1X_SPM_AUTHENTICATING;

 /* Inform the Backend state machine about the Authentication process*/
 if (dot1xSupplicantPortInfo[intIfNum].eapFail == L7_TRUE)
 {
    (void) dot1xStateMachineClassifier(sbmEapFail, intIfNum, bufHandle,
                                                  L7_NULLPTR);
 } /*eapFail Action */
 else if (dot1xSupplicantPortInfo[intIfNum].eapSuccess == L7_TRUE)
 {
    (void) dot1xStateMachineClassifier(sbmEapSuccess, intIfNum, bufHandle,
                                                  L7_NULLPTR);
 } /* eapSuccess Action */
 else
 {
   (void) dot1xStateMachineClassifier(sbmSuppStart, intIfNum, bufHandle,
                                                  L7_NULLPTR);
 } /* eapolEap Action */

  return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Actions to be performed in the SPM state RESTART 
*
* @param   intIfNum  @b{(input)) internal interface number
* @param   bufHandle @b{(input)) Buffer Handler to the EAP Packet  
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t dot1xSpmRestartAction(L7_uint32 intIfNum,L7_netBufHandle bufHandle)
{
  dot1xSupplicantPortInfo[intIfNum].eapRestart = L7_TRUE;
  dot1xSupplicantPortInfo[intIfNum].spmState = DOT1X_SPM_RESTART;

  /* Higher layers should set the eapRestart to false value
     to move the next state (Authenticating). But as we don't have any
     user interaction here, we move to the Authenticating state here itself */
  DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,intIfNum,
                    "SPM Machine for port - %d moving from %d to %d  \n",intIfNum,
                    dot1xSupplicantPortInfo[intIfNum].spmState,DOT1X_SPM_AUTHENTICATING);

  dot1xSupplicantPortInfo[intIfNum].eapRestart = L7_FALSE; 
  dot1xSpmAuthenticatingAction(intIfNum, bufHandle);

  return L7_SUCCESS;

}


/*********************************************************************
* @purpose  This routine claculates the next state for the SPM
*           state machine and executes the action for that next state
*
* @param   dot1xEvent  @b{(input)) event
* @param   intIfNum   @b{(input))  Internal interface number
* @param   msg         @b{(input)) message containing event data
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSpmMachine(L7_uint32 dot1xEvent, L7_uint32 intIfNum, 
                               L7_netBufHandle bufHandle)
{
  DOT1X_SPM_STATES_t nextState;
  L7_uint32 normalizedEvent;
  L7_RC_t rc = L7_FAILURE;


  normalizedEvent = dot1xEvent - dot1xKrxEvents - 1;

  nextState = spmStateTable[normalizedEvent][dot1xSupplicantPortInfo[intIfNum].spmState];
 
  DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,intIfNum,
                    "SPM Machine for port - %d moving from %d to %d for event %d \n",intIfNum,
                    dot1xSupplicantPortInfo[intIfNum].spmState,nextState,normalizedEvent);

  switch (nextState)
  {
    case DOT1X_SPM_LOGOFF:
      rc = L7_SUCCESS;
    break;

    case DOT1X_SPM_DISCONNECTED:
      rc = dot1xSpmDisconnectedAction(intIfNum);
      if (dot1xSupplicantPortInfo[intIfNum].portEnabled == L7_TRUE)
      {
        /* UCT to connecting State */
        rc = dot1xSpmConnectingAction(intIfNum);
      }
      else
      {
#if 0
        /* If port is not operable, set it to unauthorized */
        if (dot1xIhPhyPortStatusSet(intIfNum, L7_DOT1X_PORT_STATUS_UNAUTHORIZED,L7_TRUE)
                                                         != L7_SUCCESS)
          LOG_MSG("dot1xSpmMachine: could not set state to unauthorized, intf %u\n", intIfNum);
#endif
      }

    break;

    case DOT1X_SPM_CONNECTING:
      rc = dot1xSpmConnectingAction(intIfNum);
    break;

    case DOT1X_SPM_AUTHENTICATING:
      rc = dot1xSpmAuthenticatingAction(intIfNum,bufHandle);
    break;

   case DOT1X_SPM_HELD:
     rc = dot1xSpmHeldAction(intIfNum);
   break;

   case DOT1X_SPM_AUTHENTICATED:
   dot1xSupplicantPortInfo[intIfNum].supplicantPortStatus =
                                         L7_DOT1X_PORT_STATUS_AUTHORIZED;
    dot1xSupplicantPortInfo[intIfNum].spmState = DOT1X_SPM_AUTHENTICATED;
    dot1xIhPhyPortStatusSet(intIfNum,L7_DOT1X_PORT_STATUS_AUTHORIZED,
                                  L7_TRUE);
    #if defined L7_DOT1AD_PACKAGE
    if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,
                              L7_FEAT_DOT1X_SUPPLICANT_CONTROL_ON_AUTH_PORTS))
    {
      
      DOT1AD_INTFERFACE_TYPE_t  intfType;
      dot1adInterfaceTypeGet(intIfNum, &intfType);
      if(intfType == DOT1AD_INTFERFACE_TYPE_NNI)
      {
        dot1xEnableAuthenticatorPorts(intIfNum);
      }
   }
#endif

    rc = L7_SUCCESS;
   break;

   case DOT1X_SPM_RESTART:
    rc = dot1xSpmRestartAction(intIfNum,bufHandle);
   break;

   case DOT1X_SPM_FORCE_AUTH:
    dot1xSpmForceAuthAction(intIfNum);
    rc = L7_SUCCESS;
   break;
 
   case DOT1X_SPM_FORCE_UNAUTH:
    dot1xSpmForceUnAuthAction(intIfNum);
    rc = L7_SUCCESS;
   break;
  
   default:
      break;
  }

  return rc;
}

/* Supplicant state machine (SBMS)*/

/*********************************************************************
* @purpose  Actions to be performed in the SBM state INITIALIZE 
*
* @param   intIfNum @b{(input)) internal interface number
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSbmInitializeAction(L7_uint32 intIfNum)
{
  dot1xSupplicantPortInfo[intIfNum].suppAbort = L7_FALSE;
  dot1xSupplicantPortInfo[intIfNum].sbmState = DOT1X_SBM_INITIALIZE;
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Actions to be performed in the SBM state IDLE
*
* @param   intIfNum @b{(input)) internal interface number
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSbmIdleAction(L7_uint32 intIfNum)
{
  dot1xSupplicantPortInfo[intIfNum].suppStart = L7_FALSE;
  dot1xSupplicantPortInfo[intIfNum].sbmState = DOT1X_SBM_IDLE;
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Actions to be performed in the SBM state REQUEST
*
* @param   intIfNum  @b{(input)) internal interface number
* @param   bufHandle @b{(input)) received bufHandle from
*                                Authenticator  
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSbmRequestAction(L7_uint32 intIfNum,
                                   L7_netBufHandle bufHandle)
{
  dot1xSupplicantPortInfo[intIfNum].authWhile = 0;
  dot1xSupplicantPortInfo[intIfNum].sbmState = DOT1X_SBM_REQUEST;

  /* eapRequest is not manipulated here as there are no interactive
     users for this supplicant */

  /* getSuppRsp() function needs to be implemented */

  /* eapResp and eapNoResp are also not implemented as there no
     interactive user for this supplicant */

  /* if eapFail is True Transit to the FAIL State */ 
  if (dot1xSupplicantPortInfo[intIfNum].eapFail == L7_TRUE)
  {
    (void) dot1xStateMachineClassifier(sbmEapFail, intIfNum,bufHandle,
                                               L7_NULLPTR);
  }
  /* if eapFail is True Transit to the FAIL State */ 
  else if (dot1xSupplicantPortInfo[intIfNum].eapSuccess == L7_TRUE)
  {
    (void) dot1xStateMachineClassifier(sbmEapSuccess, intIfNum,bufHandle,
                                               L7_NULLPTR);
  }
  else
  {
   /* if eapSuccess is TRUE move to the SUCCESS state */

   /* Transit to the RESPONSE State machine */
   (void) dot1xStateMachineClassifier(sbmEapResp, intIfNum,bufHandle,
                                                          L7_NULLPTR);
  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Actions to be performed in the SBM state RECEIVE
*
* @param   intIfNum  @b{(input)) internal interface number
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSbmReceiveAction(L7_uint32 intIfNum)
{
  dot1xPortCfg_t *pCfg;

  if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;
   
  dot1xSupplicantPortInfo[intIfNum].authWhile =  pCfg->supplicantPortCfg.startPeriod;
  dot1xSupplicantPortInfo[intIfNum].eapolEap  =  L7_FALSE; 
  dot1xSupplicantPortInfo[intIfNum].sbmState = DOT1X_SBM_RECEIVE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Actions to be performed in the SBM state RESPONSE
*
* @param   intIfNum  @b{(input)) internal interface number
* @param   bufHandle @b{(input)) received bufHandle from
*                                Authenticator
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSbmResponseAction(L7_uint32 intIfNum,
                                   L7_netBufHandle bufHandle)
{
  /* Send the response to the Authenticator */
  dot1xTxSuppRsp(intIfNum); 
  dot1xSupplicantPortInfo[intIfNum].sbmState = DOT1X_SBM_RESPONSE;
 
  /* UCT to the RECEIVE State */
  DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,intIfNum,
                    "SBM Machine for port - %d moving from %d to %d \n",intIfNum,
                    dot1xSupplicantPortInfo[intIfNum].sbmState,DOT1X_SBM_RECEIVE);
 
  return dot1xSbmReceiveAction(intIfNum) ;

}

/*********************************************************************
* @purpose  Actions to be performed in the SBM state TIMEOUT
*
* @param   intIfNum  @b{(input)) internal interface number
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSbmTimeoutAction(L7_uint32 intIfNum)
{
  L7_RC_t rc;

  dot1xSupplicantPortInfo[intIfNum].suppTimeout = L7_TRUE;
  dot1xSupplicantPortInfo[intIfNum].sbmState = DOT1X_SBM_TIMEOUT;

  /* UCT to the RECEIVE State */
  DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,intIfNum,
                    "SBM Machine for port - %d moving from %d to %d \n",intIfNum,
                    dot1xSupplicantPortInfo[intIfNum].sbmState,DOT1X_SBM_IDLE);
  rc=dot1xSbmIdleAction(intIfNum);

 /* Inform the SPM state about the TIMEOUT */
 (void) dot1xStateMachineClassifier(spmSuppTimeOut, intIfNum, L7_NULL, L7_NULLPTR);

 return rc;

}

/*********************************************************************
* @purpose  Actions to be performed in the SBM state SUCCESS 
*
* @param   intIfNum  @b{(input)) internal interface number
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSbmSuccessAction(L7_uint32 intIfNum)
{
  L7_RC_t rc;

  dot1xSupplicantPortInfo[intIfNum].suppSuccess = L7_TRUE;
  dot1xSupplicantPortInfo[intIfNum].sbmState = DOT1X_SBM_SUCCESS;

  /* Reset the eap flags */
  dot1xSupplicantPortInfo[intIfNum].eapFail    = L7_FALSE;
  dot1xSupplicantPortInfo[intIfNum].eapSuccess = L7_FALSE;
  dot1xSupplicantPortInfo[intIfNum].eapolEap   = L7_FALSE;


  /* UCT to the IDLE State */
  DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,intIfNum,
                    "SBM Machine for port - %d moving from %d to %d \n",intIfNum,
                    dot1xSupplicantPortInfo[intIfNum].sbmState,DOT1X_SBM_IDLE);
  rc=dot1xSbmIdleAction(intIfNum);

 /* Inform the SPM state about the SUCCESS */
 (void) dot1xStateMachineClassifier(spmSuppSuccess, intIfNum, L7_NULL, L7_NULLPTR);

 return rc;

}

/*********************************************************************
* @purpose  Actions to be performed in the SBM state FAILURE
*
* @param   intIfNum  @b{(input)) internal interface number
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSbmFailAction(L7_uint32 intIfNum)
{
  L7_RC_t rc;

  dot1xSupplicantPortInfo[intIfNum].suppFail = L7_TRUE;
  dot1xSupplicantPortInfo[intIfNum].sbmState = DOT1X_SBM_FAIL;

  /* Reset the eap flags */
  dot1xSupplicantPortInfo[intIfNum].eapFail    = L7_FALSE;
  dot1xSupplicantPortInfo[intIfNum].eapSuccess = L7_FALSE;
  dot1xSupplicantPortInfo[intIfNum].eapolEap   = L7_FALSE;

  /* UCT to the IDLE State */
  DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,intIfNum,
                    "SBM Machine for port - %d moving from %d to %d \n",intIfNum,
                    dot1xSupplicantPortInfo[intIfNum].sbmState,DOT1X_SBM_IDLE);
  rc=dot1xSbmIdleAction(intIfNum);

 /* Inform the SPM state about the FAIL */
 (void) dot1xStateMachineClassifier(spmSuppFail, intIfNum, L7_NULL, L7_NULLPTR);

 return rc;

}





/*********************************************************************
* @purpose  This routine claculates the next state for the SBM
*           state machine and executes the action for that next state
*
* @param   dot1xEvent  @b{(input)) event
* @param   intIfNum   @b{(input))  Internal interface number
* @param   msg         @b{(input)) message containing event data
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSbmMachine(L7_uint32 dot1xEvent, L7_uint32 intIfNum,
                               L7_netBufHandle bufHandle)
{
  DOT1X_SBM_STATES_t nextState;
  L7_uint32 normalizedEvent;
  L7_RC_t rc = L7_FAILURE;


  normalizedEvent = dot1xEvent - dot1xSpmEvents - 1;

  nextState = sbmStateTable[normalizedEvent][dot1xSupplicantPortInfo[intIfNum].sbmState];

  DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,intIfNum,
                    "SBM Machine for port - %d moving from %d to %d for event %d \n",intIfNum,
                    dot1xSupplicantPortInfo[intIfNum].sbmState,nextState,normalizedEvent);
  switch (nextState)
  {
    case DOT1X_SBM_INITIALIZE:
      rc = dot1xSbmInitializeAction(intIfNum);
      /* UCT Action */
      rc = dot1xSbmIdleAction(intIfNum);
    break;
    case DOT1X_SBM_IDLE:
     rc = dot1xSbmIdleAction(intIfNum);
    break;
    case DOT1X_SBM_REQUEST:
     rc = dot1xSbmRequestAction(intIfNum,bufHandle);
    break;
    case DOT1X_SBM_RESPONSE:
     rc = dot1xSbmResponseAction(intIfNum,bufHandle);
    break;
    case DOT1X_SBM_RECEIVE:
     rc = dot1xSbmReceiveAction(intIfNum);
    break;
    case DOT1X_SBM_TIMEOUT:
     rc = dot1xSbmTimeoutAction(intIfNum);
    break;
    case DOT1X_SBM_SUCCESS:
     rc = dot1xSbmSuccessAction(intIfNum);
    break;
    case DOT1X_SBM_FAIL:
     rc = dot1xSbmFailAction(intIfNum);
    break;

    default:
      rc = L7_SUCCESS;
    break;

  }

 return rc;

}
