/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 * @filename usmdb_dot1x.c
 *
 * @purpose USMDB API's for IEEE 802.1x
 *
 * @component dot1s
 *
 * @component unitmgr
 *
 * @create 3/14/2003
 *
 * @author jflanagan
 *
 * @end
 **********************************************************************/   
#include <string.h>
#include "l7_common.h"
#include "usmdb_dot1x_api.h"
#include "dot1x_api.h"

#include "usmdb_util_api.h"


extern void dot1xDebugPacketTraceFlagGet(L7_BOOL *transmitFlag,L7_BOOL *receiveFlag);
extern L7_RC_t dot1xDebugPacketTraceFlagSet(L7_BOOL transmitFlag,L7_BOOL receiveFlag);

/*********************************************************************
 *
 * @purpose  Set the Dot1x Administrative Mode 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    val @b((input)) the boolean value of the dot1x mode  
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAdminModeSet(L7_uint32 unitIndex, 
    L7_uint32 val)
{
  return(dot1xAdminModeSet(val));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x administrative mode 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    pval @b((output)) ptr to the current value of the administrative mode  
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAdminModeGet(L7_uint32 unitIndex, 
    L7_uint32 *pVal)
{
  return dot1xAdminModeGet(pVal);
}

/*********************************************************************
 *
 * @purpose  Set the dot1x initialize value on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    val @b((input)) the boolean value to set the intialize mode  
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortInitializeSet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_BOOL val )
{
  return(dot1xPortInitializeSet(intIfNum, val));
}

/*********************************************************************
 *
 * @purpose  Set the dot1x reauthenticate value on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    val @b((input)) the boolean value to set the reauthenticate mode  
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortReauthenticateSet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_BOOL val )
{
  return(dot1xPortReauthenticateSet(intIfNum, val));
}

/*********************************************************************
 *
 * @purpose  Set the dot1x admin control direction on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    val @b((input)) the uint value for the admin control direction
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortAdminControlledDirectionsSet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 val )
{
  return(dot1xPortAdminControlledDirectionsSet(intIfNum, val));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x admin control direction on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((output)) ptr to the uint value for the admin control direction
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortAdminControlledDirectionsGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_DOT1X_PORT_DIRECTION_t *pVal )
{
  return(dot1xPortAdminControlledDirectionsGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x oper control direction on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((output)) ptr to the uint value for the oper control direction
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortOperControlledDirectionsGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_DOT1X_PORT_DIRECTION_t *pVal )
{
  return(dot1xPortOperControlledDirectionsGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Set the dot1x control mode on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    val @b((input)) the uint value for the control mode
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortControlModeSet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 val )
{
  return(dot1xPortControlModeSet(intIfNum, val));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x control mode on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pval @b((output)) ptr to the uint value for the control mode
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortControlModeGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_DOT1X_PORT_CONTROL_t *pVal )
{
  return(dot1xPortControlModeGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x operational control mode on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pval @b((output)) ptr to the uint value for the oper control mode
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortOperControlModeGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_DOT1X_PORT_CONTROL_t *pVal )
{
  return(dot1xPortOperControlModeGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x quiet period on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the quiet period
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortQuietPeriodGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 * pVal )
{
  return(dot1xPortQuietPeriodGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Set the dot1x quiet period on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    val @b((input)) the uint value for the quiet period
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortQuietPeriodSet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 val )
{
  return(dot1xPortQuietPeriodSet(intIfNum, val));
}

/*********************************************************************
 *
 * @purpose  Set the dot1x transmit period on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    val @b((input)) the uint value for the transmit period
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortTxPeriodSet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 val )
{
  return(dot1xPortTxPeriodSet(intIfNum, val));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x transmit period on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the transmit period
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortTxPeriodGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortTxPeriodGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Set the dot1x supplicant timeout on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    val @b((input)) the uint value for the supplicant timeout
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortSuppTimeoutSet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 val )
{
  return(dot1xPortSuppTimeoutSet(intIfNum, val));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x supplicant timeout on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((output)) ptr to the uint value for the supplicant timeout
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortSuppTimeoutGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortSuppTimeoutGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Set the dot1x server timeout on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    val @b((input)) the uint value for the server timeout
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortServerTimeoutSet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 val )
{
  return(dot1xPortServerTimeoutSet(intIfNum, val));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x server timeout on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((output)) ptr to the uint value for the server timeout
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortServerTimeoutGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortServerTimeoutGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Set the dot1x max requests on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    val @b((input)) the uint value for the max requests
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortMaxReqSet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 val )
{
  return(dot1xPortMaxReqSet(intIfNum, val));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x max requests on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the max requests
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortMaxReqGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortMaxReqGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Set the dot1x reauthentication period on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    val @b((input)) the uint value for the reauthentication period
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortReAuthPeriodSet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 val )
{
  return(dot1xPortReAuthPeriodSet(intIfNum, val));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x reauthentication period on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the reauthentication period
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortReAuthPeriodGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortReAuthPeriodGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Set the dot1x reauth enabled flag on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    val @b((input)) the bool value for the reauth enabled flag
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortReAuthEnabledSet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 val )
{
  return(dot1xPortReAuthEnabledSet(intIfNum, val));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x reauth enabled flag on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the bool value for the reauth enabled flag
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortReAuthEnabledGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_BOOL * pVal )
{
  return(dot1xPortReAuthEnabledGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Set the dot1x key transmission enabled flag on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    val @b((input)) the bool value for the key transmission enabled flag
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortKeyTransmissionEnabledSet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 val )
{
  return(dot1xPortKeyTransmissionEnabledSet(intIfNum, val));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x key transmission enabled flag on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the bool value for the key transmission enabled flag
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortKeyTransmissionEnabledGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_BOOL *pVal )
{
  return(dot1xPortKeyTransmissionEnabledGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x protocol version on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the protocol version
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortProtocolVersionGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_t *pVal )
{
  return(dot1xPortProtocolVersionGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x PAE capabilities on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uchar value for the pae capabilities bitmask
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortPaeCapabilitiesGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uchar8 *pVal )
{
  return(dot1xPortPaeCapabilitiesGet(intIfNum, pVal));
}

/*********************************************************************
* @purpose  Set the port's dot1x capabilities (Supplicant or Authenticator)
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    paeCapabilities  @b{(input)} dot1x capabilities bitmask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xPortPaeCapabilitiesSet(L7_uint32 intIfNum,
                                    L7_uchar8 paeCapabilities)
{
  return dot1xPortPaeCapabilitiesSet(intIfNum, paeCapabilities);
}


/*********************************************************************
 *
 * @purpose  Get the dot1x auth PAE state of the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the auth pae state
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthPaeStateGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_DOT1X_APM_STATES_t *pVal )
{
  return(dot1xPortAuthPaeStateGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x backend auth state of the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the backend auth state
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortBackendAuthStateGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_DOT1X_BAM_STATES_t *pVal )
{
  return(dot1xPortBackendAuthStateGet(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x port Eapol Frames Received Stat 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortEapolFramesRxGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortEapolFramesRx(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x port Eapol Frames Transmitted Stat 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortEapolFramesTxGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortEapolFramesTx(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x port Eapol Start Frames Received Stat 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortEapolStartFramesRxGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortEapolStartFramesRx(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x port Eapol Logoff Frames Received Stat 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortEapolLogoffFramesRxGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortEapolLogoffFramesRx(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x port Eap Response Id Frames Received Stat 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortEapRespIdFramesRxGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortEapRespIdFramesRx(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x port Eap Response Frames Received Stat 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortEapResponseFramesRxGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortEapResponseFramesRx(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x port Eap Request Id Frames Transmitted Stat 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortEapReqIdFramesTxGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortEapReqIdFramesTx(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x port Eap Request Frames Transmitted Stat 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortEapReqFramesTxGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortEapReqFramesTx(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x port Invalid Eapol Frames Received Stat 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortInvalidEapolFramesRxGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortInvalidEapolFramesRx(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x port Eap Length error Frames Received Stat 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortEapLengthErrorFramesRxGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortEapLengthErrorFramesRx(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x port Last Eapol Frame Version Stat 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortLastEapolFrameVersionGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *pVal )
{
  return(dot1xPortLastEapolFrameVersion(intIfNum, pVal));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x port Last Eapol Frame Source Stat 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortLastEapolFrameSourceGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uchar8 *pVal )
{
  return(dot1xPortLastEapolFrameSourceGet(intIfNum, (L7_enetMacAddr_t *)pVal));
}

/*********************************************************************
 *
 * @purpose  Clear the dot1x stats on the specified port 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortStatsClear(L7_uint32 unitIndex, 
    L7_uint32 intIfNum )
{
  return(dot1xPortStatsClear(intIfNum));
}

/*********************************************************************
 *
 * @purpose  Get the authorization status of the specified port
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pStatus   @b((output)) ptr to the status value  
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthControlledPortStatusGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_DOT1X_PORT_STATUS_t *pStatus )
{
  return(dot1xPortAuthControlledPortStatusGet(intIfNum, pStatus));
}

/*********************************************************************
 *
 * @purpose  Determine if the port is valid for dot1x
 *          
 * @param unit         the unit index (for a stacking environment)
 * @param iface        the interface to check
 *
 * @return   L7_SUCCESS the interface is valid
 * @return   L7_FAILURE the interface is not a valid type for dot1s.
 *
 * @note An interface is considered valid for dot1s if it is one of
 *       the following types:    USM_PHYSICAL_INTF
 *       In case of failure, the failure message is written to the 
 *       cli context.  The caller needs only to return.    
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xInterfaceValidate(L7_uint32 unit, 
    L7_uint32 iface )
{
  L7_uint32 inclIntfTypes = USM_PHYSICAL_INTF;
  L7_uint32 rc = L7_SUCCESS;
  L7_uint32 intIfType;

  /* Get the interface type always returns L7_SUCCESS*/
  (void)usmDbIntIfNumTypeMaskGet(unit, iface, &intIfType); 

  /* Validate the interface type */      
  if ( (intIfType & inclIntfTypes) == 0 ) 
  {
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
 * @purpose  Get number of times the FSM enters the CONNECTING state from any other state
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat

 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthEntersConnectingGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthEntersConnectingGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get number of times the FSM transitions from  CONNECTING to DISCONNECTED
 *           as a result of receiving an EAPOL-Logoff message
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthEapLogoffsWhileConnectingGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthEapLogoffsWhileConnectingGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM transitions from CONNECTING to AUTHENTICATING,
 *           as a result of an EAP-Response/Identity message being received from the Supplicant
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthEntersAuthenticatingGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthEntersAuthenticatingGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM transitions from AUTHENTICATING to AUTHENTICATED,
 *           as a result of the Backend Authentication state machine indicating successful
 *           authentication of the Supplicant
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthAuthSuccessWhileAuthenticatingGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthAuthSuccessWhileAuthenticatingGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM transitions from AUTHENTICATING to ABORTING,
 *           as a result of the Backend Authentication state machine indicating authentication
 *           timeout
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthAuthTimeoutsWhileAuthenticatingGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthAuthTimeoutsWhileAuthenticatingGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM transitions from AUTHENTICATING to HELD,
 *           as a result of the Backend Authentication state machine indicating authentication
 *           failure
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthAuthFailWhileAuthenticatingGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthAuthFailWhileAuthenticatingGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM transitions from AUTHENTICATING to ABORTING,
 *           as a result of a reauthentication request
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthAuthReauthsWhileAuthenticatingGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthAuthReauthsWhileAuthenticatingGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM transitions from AUTHENTICATING to ABORTING,
 *           as a result of an EAPOL-Start message being received from the Supplicant
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthAuthEapStartsWhileAuthenticatingGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthAuthEapStartsWhileAuthenticatingGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM transitions from AUTHENTICATING to ABORTING,
 *           as a result of an EAPOL-Logoff message being received from the Supplicant
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthAuthEapLogoffWhileAuthenticatingGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthAuthEapLogoffWhileAuthenticatingGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM transitions from AUTHENTICATED to CONNECTING,
 *           as a result of a reauthentication request
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthAuthReauthsWhileAuthenticatedGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthAuthReauthsWhileAuthenticatedGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM transitions from AUTHENTICATED to CONNECTING,
 *           as a result of an EAPOL-Start message being received from the Supplicant
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthAuthEapStartsWhileAuthenticatedGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthAuthEapStartsWhileAuthenticatedGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM transitions from AUTHENTICATED to DISCONNECTED,
 *           as a result of an EAPOL-Logoff message being received from the Supplicant
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthAuthEapLogoffWhileAuthenticatedGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthAuthEapLogoffWhileAuthenticatedGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM sends an initial Access-Request packet to the
 *           Authentication server
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthBackendResponsesGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthBackendResponsesGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM receives an initial Access-Challenge packet from
 *           the Authentication server
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthBackendAccessChallengesGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthBackendAccessChallengesGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM sends an EAP-Request packet
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthBackendOtherRequestsToSupplicantGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthBackendOtherRequestsToSupplicantGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM receives a response from the Supplicant to an
 *           initial EAP-Request, and the response is something other than EAP-NAK
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthBackendNonNakResponsesFromSupplicantGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthBackendNonNakResponsesFromSupplicantGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM receives an EAP-Success message from the
 *           Authentication Server
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthBackendAuthSuccessesGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthBackendAuthSuccessesGet(intIfNum, pVal));
}

/*********************************************************************
 * @purpose  Get the number of times the FSM receives an EAP-Failure message from the
 *           Authentication Server
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface  
 * @param    pVal @b((input)) ptr to the uint value for the stat
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *       
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPortAuthBackendAuthFailsGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *pVal)
{
  return(dot1xPortAuthBackendAuthFailsGet(intIfNum, pVal));
}


/*********************************************************************
 *
 * @purpose  Set the Dot1x Vlan Assignment Mode 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    val @b((input)) the boolean value of the dot1x mode  
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xVlanAssignmentModeSet(L7_uint32 unitIndex, 
    L7_uint32 val)
{
  return(dot1xVlanAssignmentModeSet(val));
}

/*********************************************************************
 *
 * @purpose  Get the Dot1x Vlan Assignment Mode 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    mode @b((input)) the boolean value of the mode
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xVlanAssignmentModeGet(L7_uint32 unitIndex, 
    L7_uint32 *mode)
{
  return dot1xVlanAssignmentModeGet(mode);
}

/*********************************************************************
 *
 * @purpose  Set the Dot1x Dynamic Vlan Creation Mode
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    val @b((input)) the boolean value of the dot1x dynamic vlan creation mode
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xDynamicVlanCreationModeSet(L7_uint32 unitIndex,
    L7_uint32 val)
{
  return(dot1xDynamicVlanCreationModeSet(val));
}
/*********************************************************************
 *
 * @purpose  Get the Dot1x Dynamic Vlan Creation Mode
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    mode @b((input)) the boolean value of the mode
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xDynamicVlanCreationModeGet(L7_uint32 unitIndex,
    L7_uint32 *mode)
{
  return dot1xDynamicVlanCreationModeGet(mode);
}

/*********************************************************************
 *
 * @purpose  Get the Vlan Assigned to a port by radius server.
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum      @b{(input)} internal interface number
 * @param    *vlanId       @b{(output)} vlan Id
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xPortVlanAssignedGet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 *vlanId )
{
  return(dot1xPortVlanAssignedGet(intIfNum, vlanId));
}

/*********************************************************************
* @purpose  Get the reason vlan was assigned to a port by dot1x.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum      @b{(input)} internal interface number
* @param    *reason       @b{(output)} reason vlan was assigned to the port by dot1x
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @notes 
*       
* @end
*********************************************************************/
L7_RC_t usmDbDot1xPortVlanAssignedReasonGet(L7_uint32 unitIndex,
                                            L7_uint32 intIfNum, L7_uint32 *reason)
{
    return(dot1xPortVlanAssignedReasonGet(intIfNum,reason));
}


/*********************************************************************
 *
 * @purpose  Set the Guest Vlan <vlan-id>.
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum      @b{(input)} internal interface number
 * @param    *vlanId       @b{(input)} guest vlan Id
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *          L7_ERROR      if MAB has been enabled on the port
 *
 * @comments
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t  usmDbDot1xAdvancedGuestPortsCfgSet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 vlanId )
{
  return dot1xAdvancedGuestPortsCfgSet(intIfNum,vlanId); 
}

/*********************************************************************
 *
 * @purpose  Get the Guest Vlan <vlan-id>.
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum      @b{(input)} internal interface number
 * @param    *vlanId       @b{(output)} guest vlan Id
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t  usmDbDot1xAdvancedGuestPortsCfgGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *vlanId )
{
  return (dot1xAdvancedGuestPortCfgGet(intIfNum,vlanId));
}


/*********************************************************************
 *
 * @purpose  Set the dot1x guest period on the specified port
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface
 * @param    period @b((input)) the uint value for the guest vlan period
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAdvancedPortGuestVlanPeriodSet(L7_uint32 unitIndex, 
    L7_uint32 intIfNum,
    L7_uint32 period)
{
  return (dot1xAdvancedGuestVlanPeriodSet(intIfNum,period));
}

/*********************************************************************
 *
 * @purpose  Get the dot1x guest vlan period on the specified port
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface
 * @param    period @b((input)) ptr to the uint value for the guestvlan period
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAdvancedPortGuestVlanPeriodGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *period )
{
  return (dot1xAdvancedGuestVlanPeriodGet(intIfNum,period));
}

/*********************************************************************
 * @purpose  Get the current status of displaying dot1x packet debug info
 *            
 * @param    none
 *                      
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes    
 *
 * @end
 *********************************************************************/
void usmDbDot1xPacketDebugTraceFlagGet(L7_BOOL *transmitFlag,L7_BOOL *receiveFlag)
{
  dot1xDebugPacketTraceFlagGet(transmitFlag,receiveFlag);
}

/*********************************************************************
 * @purpose  Turns on/off the displaying of dot1x packet debug info
 *            
 * @param    flag         new value of the Packet Debug flag
 *                      
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xPacketDebugTraceFlagSet(L7_BOOL transmitFlag,L7_BOOL receiveFlag)
{
  return dot1xDebugPacketTraceFlagSet(transmitFlag,receiveFlag);
}

/*********************************************************************
* @purpose  Get the port authentication method
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum @b((input)) the specified interface
* @param    pVal @b((input)) ptr to the value for the max users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xPortMaxUsersGet(L7_uint32 unitIndex,
                                   L7_uint32 intIfNum,
                                   L7_uint32 *pVal)
{
  return(dot1xPortMaxUsersGet(intIfNum, pVal));
}


/*********************************************************************
* @purpose  Set the max users for the port
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum @b((input)) the specified interface
* @param    val @b((input)) the boolean value to set the max users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xPortMaxUsersSet(L7_uint32 unitIndex,
                                   L7_uint32 intIfNum,
                                   L7_uint32 val)
{
  return (dot1xPortMaxUsersSet(intIfNum, val));
}

/*********************************************************************
* @purpose  Returns the first logical port for the physcial interface
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    lIntIfNum    @b((output)) the logical interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xLogicalPortFirstGet(L7_uint32 intIfNum,L7_uint32 *lIntIfNum)
{
  return dot1xLogicalPortFirstGet(intIfNum,lIntIfNum);
}

/*********************************************************************
* @purpose  Returns the first logical port for the physcial interface
*
* @param    lIntIfNum    @b((input)) the logical interface
* @param    nextIntf    @b{(ouput)}  the next interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xLogicalPortNextGet(L7_uint32 lIntIfNum,
                                     L7_uint32 *nextIntf)
{
  return dot1xLogicalPortNextGet(lIntIfNum,nextIntf);
}

/*********************************************************************
* @purpose  Returns the Supplicant Mac address for the logical interface
*
* @param    lIntIfNum    @b((input)) the specified interface
* @param    macAddr      @b((output)) Mac Address of the supplicant
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xLogicalPortSupplicantMacAddrGet(L7_uint32 lIntIfNum,
                                                  L7_uchar8 *macAddr)
{
  return dot1xLogicalPortSupplicantMacAddrGet(lIntIfNum,macAddr);
}

/*********************************************************************
* @purpose  Returns the PAE state for the logical interface
*
* @param    lIntIfNum    @b((input)) the specified interface
* @param    authPaeState @b((output)) PAE state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xLogicalPortPaeStateGet(L7_uint32 lIntIfNum,
                                         L7_uint32 *authPaeState)
{
  return dot1xLogicalPortPaeStateGet(lIntIfNum,authPaeState);
}

/*********************************************************************
* @purpose  Returns the Backend state for the logical interface
*
* @param    lIntIfNum    @b((input)) the specified interface
* @param    state        @b((output)) backend state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xLogicalPortBackendAuthStateGet(L7_uint32 lIntIfNum,
                                                 L7_uint32 *state)
{
  return dot1xLogicalPortBackendAuthStateGet(lIntIfNum,state);
}

/*********************************************************************
* @purpose  Returns the Vlan assigned for the  logical interface
*
* @param    lIntIfNum    @b((input)) the specified interface
* @param    vlan        @b((output)) vlan assigned to the logical interface
* @param    mode        @b((output)) mode of assignment Radius/unauthenticated/Default
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xLogicalPortVlanAssignmentGet(L7_uint32 lIntIfNum,
                                               L7_uint32 *vlanId,
                                               L7_uint32  *mode)
{
  return dot1xLogicalPortVlanAssignmentGet(lIntIfNum,vlanId,mode);
}
/*********************************************************************
* @purpose  Returns the User Name for the  logical interface
*
* @param    unitIndex    @b((input)) the unit for this operation
* @param    lIntIfNum    @b((input)) the specified interface
* @param    userName     @b((output)) user name for the logical interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmdbDot1xLogicalPortUserNameGet(L7_uint32 unitIndex,
                                         L7_uint32 lIntIfNum,
                                         L7_uchar8 *userName)
{
  return dot1xLogicalPortUserNameGet(lIntIfNum,userName);
}


/*********************************************************************
* @purpose  Returns Session time
*
* @param    unitIndex   @b((input)) the unit for this operation
* @param    lIntIfNum   @b((input)) the specified interface
* @param    value       @b((output)) session time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmdbDot1xPortSessionTimeGet(L7_uint32 unitIndex,
                                     L7_uint32 lIntIfNum,
                                     L7_uint32 *value)
{
  return dot1xPortSessionTimeGet(lIntIfNum,value);
}


/*********************************************************************
* @purpose  Returns the filter id for the  logical interface
*
* @param    unitIndex   @b((input)) the unit for this operation
* @param    lIntIfNum  @b((input)) the specified interface
* @param    filter     @b((output)) filter id for the logical interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmdbDot1xLogicalPortFilterIdGet(L7_uint32 unitIndex,
                                         L7_uint32 lIntIfNum,
                                         L7_uchar8 *filter)
{
   return dot1xLogicalPortFilterIdGet(lIntIfNum,filter);
}

/*********************************************************************
* @purpose  Set Unathenticated vlan 
*
* @param    unitIndex   @b((input)) the unit for this operation
* @param    intIfNum    @b{(input)} internal interface number
* @param    vlanId      @b{(input)} Unathenticated vlan
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t usmDbDot1xPortUnauthenticatedVlanSet(L7_uint32 unitIndex,
                                              L7_uint32 intIfNum, 
                                              L7_uint32 vlanId)
{
  return dot1xPortUnauthenticatedVlanSet(intIfNum,vlanId);
}

/*********************************************************************
* @purpose  Get UnatuthenticatedVlan value
*
* @param    unitIndex   @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *vlanId @b{(output)} Unatuthenticated Vlan
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t usmDbDot1xPortUnauthenticatedVlanGet(L7_uint32 unitIndex,
                                              L7_uint32 intIfNum,
                                              L7_uint32 *vlanId)
{
  return dot1xPortUnauthenticatedVlanGet(intIfNum,vlanId);
}

/*********************************************************************
* @purpose  Get the port's session timeout value
*
* @param    unitIndex   @b((input)) the unit for this operation
* @param    intIfNum      @b{(input)} internal interface number
* @param    *sessionTimeout @b{(output)} session timeout value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
L7_RC_t usmDbDot1xPortSessionTimeoutGet(L7_uint32 unitIndex,
                                        L7_uint32 intIfNum, 
                                        L7_uint32 *sessionTimeout)
{
    return (dot1xPortSessionTimeoutGet(intIfNum,sessionTimeout));
}

/*********************************************************************
* @purpose  Returns the session timeout value for the  logical interface
*
* @param    unitIndex   @b((input)) the unit for this operation
* @param    lIntIfNum  @b((input)) the specified interface
* @param    sessiontimeout  @b((output)) session timeout for the logical interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xLogicalPortSessionTimeoutGet(L7_uint32 unitIndex,
                                               L7_uint32 lIntIfNum,
                                               L7_uint32 *session_timeout)
{
    return(dot1xLogicalPortSessionTimeoutGet(lIntIfNum,session_timeout));
}

/*********************************************************************
* @purpose  Get the ports termination action value
*
* @param    unitIndex   @b((input)) the unit for this operation
* @param    intIfNum      @b{(input)} internal interface number
* @param    *terminationAction @b{(output)} termination action value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
L7_RC_t usmDbDot1xPortTerminationActionGet(L7_uint32 unitIndex,
                                           L7_uint32 intIfNum, 
                                           L7_uint32 *terminationAction)
{
    return(dot1xPortTerminationActionGet(intIfNum,terminationAction));
}

/*********************************************************************
* @purpose  Returns the termination Action for the  logical interface
*
* @param    unitIndex   @b((input)) the unit for this operation
* @param    lIntIfNum  @b((input)) the specified interface
* @param    terminationAction  @b((output)) termination Action for the logical interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xLogicalPortTerminationActionGet(L7_uint32 unitIndex,
                                                  L7_uint32 lIntIfNum,
                                                  L7_uint32 *terminationAction)
{
    return(dot1xLogicalPortTerminationActionGet(lIntIfNum,terminationAction));
}

/*********************************************************************
* @purpose  Returns the logical port for the corresponding supplicant Mac Address
*
* @param    unitIndex   @b((input)) the unit for this operation
* @param    mac_addr    @b{(input)} supplicant mac address to be searched
* @param    lIntIfNum    @b((output)) the logical interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xClientMacAddressGet(L7_uint32 unitIndex,
                                      L7_uchar8 *macAddr,
                                      L7_uint32 *lIntIfNum)
{
    L7_enetMacAddr_t clientMac;

    memset(&clientMac,0,sizeof(L7_enetMacAddr_t));

    memcpy(clientMac.addr,macAddr,L7_ENET_MAC_ADDR_LEN);

    return (dot1xClientMacAddressGet(&clientMac,lIntIfNum));
       
}

/*********************************************************************
* @purpose  Returns the logical port for the next supplicant Mac Address
*           in the next mac address database
*
* @param    unitIndex   @b((input)) the unit for this operation
* @param    mac_addr    @b{(input)} supplicant mac address to be searched
* @param    lIntIfNum    @b((output)) the logical interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xClientMacAddressNextGet(L7_uint32 unitIndex,
                                     L7_uchar8 *macAddr,
                                     L7_uint32 *lIntIfNum)
{
  L7_RC_t rc;
  L7_enetMacAddr_t clientMac;

  memset(&clientMac,0,sizeof(L7_enetMacAddr_t));

  memcpy(clientMac.addr,macAddr,L7_ENET_MAC_ADDR_LEN);
  rc= (dot1xClientMacAddressNextGet(&clientMac,lIntIfNum));
  if (rc==L7_SUCCESS) 
  {
      memcpy(macAddr,clientMac.addr,L7_ENET_MAC_ADDR_LEN);
  }
  return rc;
}

/*********************************************************************
* @purpose  Returns the physical port corresponding to the logical interface
*
* @param    unitIndex   @b((input)) the unit for this operation
* @param    lIntIfNum    @b((input)) the logical interface
* @param    physport    @b{(ouput)}  the physical interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xPhysicalPortGet(L7_uint32 unitIndex,
                                  L7_uint32 lIntIfNum,
                                  L7_uint32 *physPort)
{
    return (dot1xPhysicalPortGet(lIntIfNum,physPort));
}

/*********************************************************************
* @purpose  Get the username that authenticated on a port.
*
* @param    intIfNum @b((input)) the specified interface
* @param    username @b((output))    Username
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xPortUsernameGet(L7_uint32 intIfNum,
                                  L7_uchar8 *username)
{
  return dot1xPortUsernameGet(intIfNum,username);
}


/* Supplicant APIs */
/*********************************************************************
* @purpose  Get the Port's Max start value
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    maxStart   @b((output)) Max start value
*
* @returns  L7_SUCCESS
*
* @notes    The max start value represents the maximum number of
*           successive EAPOL start messages that will be sent before
*           the Supplicant assumes that there is no Authenticator is
*           present.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortMaxStartGet(L7_uint32 intIfNum,
                                    L7_uint32 *maxStart)
{
  return dot1xSupplicantPortMaxStartGet(intIfNum, maxStart);
}

/*********************************************************************
* @purpose  Set the Port.s Max start value
*
*@param    intIfNum     @b{(input)} internal interface number
* @param    maxStart   @b((input)) Max start value
*
* @returns  L7_SUCCESS
*
* @notes    The max start value represents the maximum number of successive
*           EAPOL start messages that will be sent before the Supplicant
*           assumes that there is no Authenticator is present.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortMaxStartSet(L7_uint32 intIfNum,
                                     L7_uint32 maxStart)
{
  return dot1xSupplicantPortMaxStartSet(intIfNum, maxStart);
}

/*********************************************************************
* @purpose  Get the start period
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    *startPeriod  @b{(output)} start period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The startPeriod is the initialization value for startWhen,
*           which is a timer used by the Supplicant PAE state machine to
*           determine when to send next start message to the Authenticator.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortStartPeriodGet(L7_uint32 intIfNum,
                                           L7_uint32 *startPeriod)
{
  return dot1xPortSupplicantPortStartPeriodGet(intIfNum, startPeriod);
}

/*********************************************************************
* @purpose  Set the start period
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    startPeriod  @b{(input)} start period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The startPeriod is the initialization value for startWhen,
*           which is a timer used by the Supplicant PAE state machine to
*           determine when to send next start message to the Authenticator.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortStartPeriodSet(L7_uint32 intIfNum,
                                           L7_uint32 startPeriod)
{
  return dot1xPortSupplicantPortStartPeriodSet (intIfNum, startPeriod);
}

/*********************************************************************
* @purpose  Get the Held period
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    *heldPeriod  @b{(output)} Held period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The heldPeriod is the initialization value for heldWhile,
*           which is a timer used by the Supplicant PAE state machine to
*           determine when to send start message on previous authentication
*           failure.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortHeldPeriodGet(L7_uint32 intIfNum,
                                             L7_uint32 *heldPeriod)
{
  return dot1xSupplicantPortHeldPeriodGet (intIfNum, heldPeriod);
}

/*********************************************************************
* @purpose  Set the Held period
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    heldPeriod  @b{(input)} Held period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The heldPeriod is the initialization value for heldWhile,
*           which is a timer used by the Supplicant PAE state machine to
*           determine when to send start message on previous authentication
*           failure.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortHeldPeriodSet(L7_uint32 intIfNum,
                                          L7_uint32 heldPeriod)
{
  return dot1xSupplicantPortHeldPeriodSet (intIfNum, heldPeriod);
}


/*********************************************************************
* @purpose  Get the Auth period
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    authPeriod  @b{(output)} Auth period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The authPeriod is the initialization value for authWhile,
*           which is a timer used by the Supplicant Backend state machine to
*          determine when to abort the authentication process on no response from
*          Authenticator.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortAuthPeriodGet(L7_uint32 intIfNum,
                                     L7_uint32 *authPeriod)
{

  return dot1xSupplicantPortAuthPeriodGet(intIfNum, authPeriod);
}

/*********************************************************************
* @purpose  Set the Auth period
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    authPeriod  @b{(input)} Auth period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The authPeriod is the initialization value for authWhile,
*           which is a timer used by the Supplicant Backend state machine to
*           determine when to abort the authentication process on no response
*           from Authenticator.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortAuthPeriodSet(L7_uint32 intIfNum,
                                               L7_uint32 authPeriod)
{
  return dot1xSupplicantPortAuthPeriodSet (intIfNum, authPeriod);
}


/*********************************************************************
* @purpose  Gets the Supplicant port's control Mode
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    *portControl   @b((output))Supplicant Port Control mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortControlModeGet(L7_uint32 intIfNum,
                                     L7_uint32 *portControl)
{
  return dot1xSupplicantPortControlModeGet(intIfNum,portControl);
}

/*********************************************************************
* @purpose  Sets the Supplicant port's control Mode
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    portControl @b((input)) Supplican't control mode.
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortControlModeSet(L7_uint32 intIfNum,
                                           L7_uint32 portControl)
{
  return dot1xSupplicantPortControlModeSet (intIfNum, portControl);
}


/*********************************************************************
 *
 * @purpose  Get the dot1x Supplicant PAE state of the specified port
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface
 * @param    pVal @b((input)) ptr to the uint value for the Supplicant
 *           pae state
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortPaeStateGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_DOT1X_SPM_STATES_t *pVal )
{
  return(dot1xSupplicantPortPaeStateGet(intIfNum, pVal));
}


/*********************************************************************
* @purpose  Get the Supplicant Port's Authorization state
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    *spmStatus  @b((output)) Supplican't SPM state mode.
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortStatusGet(L7_uint32 intIfNum,
                                           L7_uchar8 *spmStatus)
{
  return dot1xSupplicantPortSpmStatusGet(intIfNum, spmStatus);
}

/*********************************************************************
 *
 * @purpose  Get the dot1x Supplicant Backend machine state of the
 *           specified port.
 *
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum @b((input)) the specified interface
 * @param    pVal @b((input)) ptr to the uint value for the Supplicant
 *           Backend state machine state
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortSbmStateGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_DOT1X_SBM_STATES_t *pVal )
{
  return(dot1xSupplicantPortSbmStateGet(intIfNum, pVal));
}

/*********************************************************************
* @purpose  Set the user name
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    userName  @b{(input)} user Name period
* @param    userLen  @b{(input)} user Length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortUserNameSet(L7_uint32 intIfNum,
                                         L7_uchar8 * userName,
                                         L7_uint32 userLen)
{
  return dot1xSupplicantPortUserNameSet (intIfNum,
                                         userName,
                                         userLen);
}


/*********************************************************************
* @purpose  Get the Supplicant user name
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    userName       @b{(output)} Auth period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortUserNameGet(L7_uint32 intIfNum,
                                     L7_uchar8 *userName)
{
  return dot1xSupplicantPortUserNameGet (intIfNum, userName);
}


/* Supplicant port Stats retreival APIs */

/*********************************************************************
* @purpose  Get number of EAPOL frames received
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    *eapolFramesRx @b{(output)} EAPOL frames received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortEapolFramesRxGet(L7_uint32 intIfNum,
                                         L7_uint32 *eapolFramesRx)
{
  return dot1xSuppicantPortEapolFramesRx(intIfNum, eapolFramesRx);
}

/*********************************************************************
* @purpose  Get number of EAPOL frames transmitted
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    *eapolFramesTx @b{(output)} EAPOL frames transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortEapolFramesTxGet(L7_uint32 intIfNum,
                                         L7_uint32 *eapolFramesTx)
{
 return dot1xSuppicantPortEapolFramesTx(intIfNum, eapolFramesTx);
}
/*********************************************************************
* @purpose  Get number of EAPOL Start frames transmitted
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    *eapolStartFramesTx @b{(output)} EAPOL frames transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortEapolStartFramesTxGet(L7_uint32 intIfNum,
                                         L7_uint32 *eapolStartFramesTx)
{
  return dot1xSuppicantPortEapolStartFramesTx(intIfNum,eapolStartFramesTx);
}

/*********************************************************************
* @purpose  Get number of EAPOL LOGOFF frames transmitted
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    *eapolLogoffFramesTx @b{(output)} EAPOL frames transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortEapolLogoffFramesTxGet(L7_uint32 intIfNum,
                                         L7_uint32 *eapolLogoffFramesTx)
{
  return dot1xSuppicantPortEapolLogoffFramesTx(intIfNum,eapolLogoffFramesTx);
}

/*********************************************************************
* @purpose  Get number of EAPOL Response/Id frames transmitted
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    *eapolRespIdFramesTx @b{(output)} EAPOL frames transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortEapolRespIdFramesTxGet(L7_uint32 intIfNum,
                                         L7_uint32 *eapolRespIdFramesTx)
{
  return dot1xSuppicantPortEapolRespIdFramesTx(intIfNum,eapolRespIdFramesTx);
}

/*********************************************************************
* @purpose  Get number of EAPOL Response frames transmitted
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    *eapolResponseFramesTx @b{(output)} EAPOL frames transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortEapolResponseFramesTxGet(L7_uint32 intIfNum,
                                         L7_uint32 *eapolResponseFramesTx)
{
  return dot1xSuppicantPortEapolResponseFramesTx(intIfNum, eapolResponseFramesTx); 
}
/*********************************************************************
* @purpose  Get number of EAPOL Request/Id frames transmitted
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    *eapolReqIdFramesTx @b{(output)} EAPOL frames received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortEapolReqIdFramesRxGet(L7_uint32 intIfNum,
                                         L7_uint32 *eapolReqIdFramesRx)
{
  return dot1xSuppicantPortEapolReqIdFramesRx (intIfNum,eapolReqIdFramesRx);
}
/*********************************************************************
* @purpose  Get number of EAPOL Request frames transmitted
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    *eapolRequestFramesRx @b{(output)} EAPOL frames transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortEapolRequestFramesRxGet(L7_uint32 intIfNum,
                                         L7_uint32 *eapolReqFramesRx)
{
  return dot1xSuppicantPortEapolRequestFramesRx(intIfNum, eapolReqFramesRx);
}
/*********************************************************************
* @purpose  Get number of invalid EAPOL frames received
*
* @param    intIfNum              @b{(input)} internal interface number
* @param    *invalidEapolFramesRx @b{(output)} Invalid EAPOL frames received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortInvalidEapolFramesRxGet(L7_uint32 intIfNum,
                                  L7_uint32 *invalidEapolFramesRx)
{
  return dot1xSupplicantPortInvalidEapolFramesRx(intIfNum, invalidEapolFramesRx);
}

/*********************************************************************
* @purpose  Get number of EAP length error frames received
*
* @param    intIfNum                @b{(input)} internal interface number
* @param    *eapLengthErrorFramesRx @b{(output)} EAP length error frames received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortEapLengthErrorFramesRxGet(L7_uint32 intIfNum,
                                     L7_uint32 *eapLengthErrorFramesRx)
{
  return dot1xSupplicantPortEapLengthErrorFramesRx (intIfNum,
                                                  eapLengthErrorFramesRx);
}

/*********************************************************************
* @purpose  Get protocol version number carried in the most recently
*           received EAPOL frame
*
* @param    intIfNum               @b{(input)} internal interface number
* @param    *lastEapolFrameVersion @b{(output)} last EAPOL version received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortLastEapolFrameVersionGet(L7_uint32 intIfNum,
                                      L7_uint32 *lastEapolFrameVersion)
{
 return dot1xSupplicantPortLastEapolFrameVersion (intIfNum,
                                    lastEapolFrameVersion);
}

/*********************************************************************
* @purpose  Get the source MAC address carried in the most recently
*           recently received EAPOL frame
*
* @param    intIfNum               @b{(input)} internal interface number
* @param    *lastEapolFrameVersion @b{(output)} last source MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xSupplicantPortLastEapolFrameSourceGet(L7_uint32 intIfNum,
                                 L7_enetMacAddr_t *lastEapolFrameSource)
{
  return dot1xSupplicantPortLastEapolFrameSourceGet(intIfNum,
                                               lastEapolFrameSource);
}

/*********************************************************************
* @purpose  Get the configured MAB value on the port.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    mabEnabled   @b{(output)} value detrmining if MAB 
*                                      has been enabled on the port 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
L7_RC_t usmDbDot1xPortMABEnabledGet(L7_uint32 unitIndex,L7_uint32 intIfNum, L7_uint32 *mabEnabled)
{
  return (dot1xPortMABEnabledGet(intIfNum,mabEnabled));
}

/*********************************************************************
* @purpose  Get the operational MAB value on the port.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    mabEnabled   @b{(output)} value detrmining if MAB 
*                                      has been operationally
*                                      enabled on the port 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
L7_RC_t usmDbDot1xPortOperMABEnabledGet(L7_uint32 unitIndex,L7_uint32 intIfNum, L7_uint32 *mabEnabled)
{
  return (dot1xPortOperMABEnabledGet(intIfNum,mabEnabled));
}

/*********************************************************************
* @purpose  Set the MAB value on the port.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    mabEnabled   @b{(output)} boolean value detrmining if MAB 
*                                      has been configured on the port 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @results  L7_REQUEST_DENIED     if port control mode of the port is 
*                                 not mac-based    
* @results  L7_ERROR              if guest vlan has been configured on 
*                                 the same port.    
*
* @comments
*       
* @end
*********************************************************************/
L7_RC_t usmDbDot1xPortMABEnabledSet(L7_uint32 unitIndex,L7_uint32 intIfNum, L7_uint32 mabEnabled)
{
  return dot1xPortMABEnabledSet(intIfNum,mabEnabled);
}

/*********************************************************************
 *
 * @purpose  Set the Dot1x Monitor Mode 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    val @b((input)) the boolean value of the Monitor mode  
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE  
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xMonitorModeSet(L7_uint32 unitIndex, 
                                 L7_BOOL val)
{
  return dot1xMonitorModeSet(val);
}

/*********************************************************************
 * @purpose  Get the dot1x Monitor mode 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    pval @b((output)) ptr to the current value of the Monitor mode  
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xMonitorModeGet(L7_uint32 unitIndex, 
                                 L7_BOOL *pVal)
{
  return dot1xMonitorModeGet(pVal);
}

/*********************************************************************
* @purpose  Get the next valid Auth History Interface Index
*
* @param    unitIndex  @b((input)) the unit for this operation
*           intIfNum   @b((input/output)) interface Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogIfIndexNextGet(L7_uint32 unitIndex,
                                               L7_uint32 *intIfNum) 
{
  return dot1xAuthHistoryLogIfIndexNextGet(intIfNum);
}

/*********************************************************************
* @purpose  Get the next valid Auth History Entry Index
*
* @param    unitIndex  @b((input)) the unit for this operation
*           intIfNum   @b((input/output)) interface Index
*           entryIndex @b((input/output)) reference to entry Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogEntryIndexNextGet(L7_uint32 unitIndex,
                                                  L7_uint32 *intIfNum, 
                                                  L7_uint32 *entryIndex)
{
  return dot1xAuthHistoryLogIndexNextGet(intIfNum, entryIndex);
}

/******************************************************************************
 * @purpose  Gets the next History Log entry indexs in reverse order
 *
 * @param    unitIndex  @b((input)) the unit for this operation
 * @param    intIfNum   @b{(input/output)} Physical Interface Number
 * @param    entryIndex @b{(input/output)} EntryIndex
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE  If there are no entries
 *
 * @notes
 *
 * @end
 *******************************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogReverseIndexNextGet(L7_uint32 unitIndex,
                                                    L7_uint32 *intIfNum, 
                                                    L7_uint32 *entryIndex)
{
  return dot1xAuthHistoryLogReverseIndexNextGet(intIfNum, entryIndex);
}

/***************************************************************************
* @purpose  Purge dot1x authentication history log table for the given interface
*
* @param    unitIndex  @b((input)) the unit for this operation
* @param    intIfNum   @b((input)) interface Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*****************************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogInterfacePurge(L7_uint32 unitIndex,  
                                               L7_uint32 intIfNum)
{
  return dot1xAuthHistoryLogInterfacePurge(intIfNum);
}

/***************************************************************************
* @purpose  Purge entire dot1x authentication history log table
*
* @param    unitIndex  @b((input)) the unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*****************************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogPurgeAll(L7_uint32 unitIndex)
{
  return dot1xAuthHistoryLogPurgeAll();
}

/*********************************************************************
 * @purpose  Get the number of dot1x Monitor mode clients
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    pCount @b((output)) ptr to the number of monitor mode clients  
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xMonitorModeNumClientsGet(L7_uint32 unitIndex, 
                                           L7_uint32 *pCount)
{
  return dot1xMonitorModeNumClientsGet(pCount);
}

/*********************************************************************
 * @purpose  Get the number of dot1x Non Monitor mode clients
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    pCount @b((output)) ptr to the number of non monitor mode
 *                               clients  
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xNonMonitorModeNumClientsGet(L7_uint32 unitIndex, 
                                              L7_uint32 *pCount)
{
  return dot1xNonMonitorModeNumClientsGet(pCount);
}

/*********************************************************************
 * @purpose  Get the timestamp from the dot1x Auth History table
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum   @b{(input)} Physical Interface Number
 * @param    entryIndex @b{(input)} EntryIndex
 * @param    pTimeStamp @b{(output)} reference to the Reason Code
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogTimeStampGet(L7_uint32 unitIndex,
                                             L7_uint32 intIfNum,
                                             L7_uint32 entryIndex,
                                             L7_uint32 *pTimeStamp)
{
  return dot1xAuthHistoryLogTimestampGet(intIfNum, entryIndex, pTimeStamp);
} 

/*********************************************************************
 * @purpose  Get the reasonCode from the dot1x Auth History table
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum   @b{(input)} Physical Interface Number
 * @param    entryIndex @b{(input)} EntryIndex
 * @param    pReasonCode @b{(output)} Reference to the Reason Code
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogReasonCodeGet(L7_uint32 unitIndex,
                                              L7_uint32 intIfNum,
                                              L7_uint32 entryIndex,
                                              L7_uint32 *pReasonCode) 
{
  return dot1xAuthHistoryLogReasonCodeGet(intIfNum, entryIndex, pReasonCode);
} 

/*********************************************************************
 * @purpose  Get the VlanId from the dot1x Auth History table
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum   @b{(input)} Physical Interface Number
 * @param    entryIndex @b{(input)} EntryIndex
 * @param    pVlanId    @b{(output)} reference to the VLANID
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogVlanIdGet(L7_uint32 unitIndex,
                                          L7_uint32 intIfNum,
                                          L7_uint32 entryIndex,
                                          L7_ushort16 *pVlanId) 
{
  return dot1xAuthHistoryLogVlanIdGet(intIfNum, entryIndex, pVlanId);
} 


/*********************************************************************
 * @purpose  Get the accessStatus from the dot1x Auth History table
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum   @b{(input)} Physical Interface Number
 * @param    entryIndex @b{(input)} EntryIndex
 * @param    pStatus    @b{(output)} Reference to the Access Status
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogAccessStatusGet(L7_uint32 unitIndex,
                                                L7_uint32 intIfNum,
                                                L7_uint32 entryIndex,
                                                L7_uint32 *pStatus) 
{
  return dot1xAuthHistoryLogAccessStatusGet(intIfNum, entryIndex, pStatus);
} 

/*********************************************************************
 * @purpose  Get the dot1x authentication Status from the dot1x Auth 
 *           History table
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum   @b{(input)} Physical Interface Number
 * @param    entryIndex @b{(input)} EntryIndex
 * @param    pStatus    @b{(output)} Reference to the Auth Status
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogAuthStatusGet(L7_uint32 unitIndex,
                                              L7_uint32 intIfNum,
                                              L7_uint32 entryIndex,
                                              L7_uint32 *pStatus) 
{
  return dot1xAuthHistoryLogAuthStatusGet(intIfNum, entryIndex, pStatus);
} 

/*********************************************************************
 * @purpose  Get the supplicant Mac Address from the dot1x Auth History table
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum   @b{(input)} Physical Interface Number
 * @param    entryIndex @b{(input)} EntryIndex
 * @param    pMacAddress @b{(output)} Reference to the Mac Address
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogSupplicantMacAddressGet(L7_uint32 unitIndex,
                                                        L7_uint32 intIfNum,
                                                        L7_uint32 entryIndex,
                                                        L7_enetMacAddr_t *pMacAddress)
{
  return dot1xAuthHistoryLogSupplicantMacAddressGet(intIfNum, entryIndex, pMacAddress);
} 

/*********************************************************************
 * @purpose  Get the FilterID for the Dot1x Radius Accept Packet
 *           located in the dot1x Auth History table
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum   @b{(input)} Physical Interface Number
 * @param    entryIndex @b{(input)} EntryIndex
 * @param    pFilterId  @b{(output)} Reference to the filter Id
 * @param    pFilterLen @b{(input/output)} Reference to filter Length
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogFilterIdGet(L7_uint32 unitIndex,
                                            L7_uint32 intIfNum,
                                            L7_uint32 entryIndex,
                                            L7_uchar8 *pFilterIdName,
                                            L7_uint32 *pFilterLen) 
{
  return dot1xAuthHistoryLogFilterIdGet(intIfNum, 
                                        entryIndex, 
                                        pFilterIdName,
                                        pFilterLen);
} 


/*********************************************************************
 * @purpose  Get the Vlan Assigned from the dot1x Auth History table
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum   @b{(input)} Physical Interface Number
 * @param    entryIndex @b{(input)} EntryIndex
 * @param    pVlanId    @b{(output)} reference to the VLANID
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogVlanIdAssignedGet(L7_uint32 unitIndex,
                                                  L7_uint32 intIfNum,
                                                  L7_uint32 entryIndex,
                                                  L7_ushort16 *pVlanId)
{
  return dot1xAuthHistoryLogVlanIdAssignedGet(intIfNum,
                                              entryIndex,
                                              pVlanId);
}

/*********************************************************************
 * @purpose  Get the Vlan Assigned Type from the dot1x Auth History table
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum   @b{(input)} Physical Interface Number
 * @param    entryIndex @b{(input)} EntryIndex
 * @param    pVlanAssignedType @b{(output)} reference to the VLAN 
                                              assigned type.
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogVlanAssignedTypeGet(L7_uint32 unitIndex,
                                                    L7_uint32 intIfNum,
                                                    L7_uint32 entryIndex,
                                                    L7_uint8  *pVlanAssignedType)
{
   return dot1xAuthHistoryLogVlanAssignedTypeGet(intIfNum,
                                                 entryIndex,
                                                 pVlanAssignedType);
}


/*********************************************************************
 * @purpose  Get the MAB enable status from the dot1x Auth History table
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum   @b{(input)} Physical Interface Number
 * @param    entryIndex @b{(input)} EntryIndex
 * @param    pMABStatus @b{(output)} MAB enable status
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogMABStatusGet(L7_uint32 unitIndex,
                                             L7_uint32 intIfNum,
                                             L7_uint32 entryIndex,
                                             L7_uint8  *pMABStatus)
{
  return dot1xAuthHistoryLogMABStatusGet(intIfNum,
                                         entryIndex,
                                         pMABStatus);
}
/*********************************************************************
 * @purpose  Check if the dot1x auth history log entry exists in History
 *           table
 *          
 * @param    unitIndex @b((input)) the unit for this operation
 * @param    intIfNum   @b{(input)} Physical Interface Number
 * @param    entryIndex @b{(input)} EntryIndex
 *
 * @returns L7_SUCCESS  
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogEntryIsValid(L7_uint32 unitIndex,
                                             L7_uint32 intIfNum,
                                             L7_uint32 entryIndex)
{
  return dot1xAuthHistoryLogEntryIsValid(intIfNum, entryIndex);
}


 /*********************************************************************
 * @purpose  Return Reason in String format for the given entry
 *          
 * @param    intIfNum   (input)  - Interface
 *           entryIndex (input)  - Entry Index
 *           reasonCode (input)  - Reason Code
 *           strReason  (output) - Reason in String format
 *           strLen     (input/output) - Length of Reason String
 *
 * @returns L7_SUCCESS/L7_FAILURE
 *
 * @comments
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbDot1xAuthHistoryLogReasonStringGet(L7_uint32 intIfNum, 
                                                L7_uint32 entryIndex, 
                                                L7_uint32 reasonCode, 
                                                L7_char8  *strReason,
                                                L7_uint32 *strLen)
{
  return dot1xAuthHistoryLogReasonStringGet(intIfNum, 
                                            entryIndex,
                                            reasonCode,
                                            strReason,
                                            strLen);
}


