/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename  dot1x_api.h
*
* @purpose   Externs for dot1x
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
**********************************************************************/

#ifndef INCLUDE_DOT1X_API_H
#define INCLUDE_DOT1X_API_H

#include "l7_common.h"
#include "dot1x_exports.h"
#include "dot1x_exports.h"

/*********************************************************************
* @purpose  Get administrative mode setting for dot1x
*
* @param    *adminMode @b{(output)} dot1x admin mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xAdminModeGet(L7_uint32 *adminMode);

/*********************************************************************
* @purpose  Set administrative mode setting for dot1x
*
* @param    adminMode @b{(input)} dot1x admin mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xAdminModeSet(L7_uint32 adminMode);

/*********************************************************************
* @purpose  Get initialize value for a port
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    *initialize @b{(output)} initialize value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This value indicates whether a port is being initialized
*           due to a management request
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortInitializeGet(L7_uint32 intIfNum, L7_BOOL *initialize);

/*********************************************************************
* @purpose  Set initialize value for a port
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    initialize @b{(input)} initialize value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This value is set to L7_TRUE by management in order to force
*           initialization of a port.  It is re-set to L7_FALSE after
*           initialization has completed.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortInitializeSet(L7_uint32 intIfNum, L7_BOOL initialize);

/*********************************************************************
* @purpose  Get reauthentication value for a port
*
* @param    intIfNum        @b{(input)} internal interface number
* @param    *reauthenticate @b{(output)} reauthentication value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This value indicates whether a port is being reauthenticated
*           due to a management request
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortReauthenticateGet(L7_uint32 intIfNum, L7_BOOL *reauthenticate);

/*********************************************************************
* @purpose  Set reauthentication value for a port
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    reauthenticate @b{(input)} reauthentication value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This value is set to L7_TRUE by management in order to force
*           reauthentication of a port.  It is re-set to L7_FALSE after
*           reauthentication has completed.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortReauthenticateSet(L7_uint32 intIfNum, L7_BOOL reauthenticate);

/*********************************************************************
* @purpose  Get admin value of controlled directions
*
* @param    intIfNum                   @b{(input)} internal interface number
* @param    *adminControlledDirections @b{(output)} controlled directions value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Since uni-directional control is currently not supported,
*           this will always return L7_DOT1X_PORT_DIRECTION_BOTH.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAdminControlledDirectionsGet(L7_uint32 intIfNum,
                                       L7_DOT1X_PORT_DIRECTION_t *adminControlledDirections);

/*********************************************************************
* @purpose  Set admin value of controlled directions
*
* @param    intIfNum                  @b{(input)} internal interface number
* @param    adminControlledDirections @b{(input)} controlled directions value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Uni-directional control is currently not supported.
*           The dot1xAuthAdminControlledDirections MIB object is supported as
*           read-only.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAdminControlledDirectionsSet(L7_uint32 intIfNum,
                                       L7_DOT1X_PORT_DIRECTION_t adminControlledDirections);

/*********************************************************************
* @purpose  Get port control mode
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    *portControl @b{(output)} port control mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortControlModeGet(L7_uint32 intIfNum, L7_DOT1X_PORT_CONTROL_t *portControl);

/*********************************************************************
* @purpose  Set port control mode
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    portControl @b{(input)} port control mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortControlModeSet(L7_uint32 intIfNum, L7_DOT1X_PORT_CONTROL_t portControl);

/*********************************************************************
* @purpose  Get quiet period value
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    *quietPeriod @b{(output)} quiet period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The quietTime is the initialization value for quietWhile,
*           which is a timer used by the Authenticator state machine
*           to define periods of time in which it will not attempt to
*           acquire a Supplicant.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortQuietPeriodGet(L7_uint32 intIfNum, L7_uint32 *quietPeriod);

/*********************************************************************
* @purpose  Set quiet period value
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    quietPeriod @b{(input)} quiet period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The quietPeriod is the initialization value for quietWhile,
*           which is a timer used by the Authenticator state machine
*           to define periods of time in which it will not attempt to
*           acquire a Supplicant.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortQuietPeriodSet(L7_uint32 intIfNum, L7_uint32 quietPeriod);

/*********************************************************************
* @purpose  Get tx period value
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    *txPeriod @b{(output)} tx period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The txPeriod is the initialization value for txWhen,
*           which is a timer used by the Authenticator state machine
*           to determine when to send an EAPOL EAP Request/Identity
*           frame to the Supplicant.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortTxPeriodGet(L7_uint32 intIfNum, L7_uint32 *txPeriod);

/*********************************************************************
* @purpose  Set tx period value
*
* @param    intIfNum @b{(input)} internal interface number
* @param    txPeriod @b{(input)} tx period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The txPeriod is the initialization value for txWhen,
*           which is a timer used by the Authenticator state machine
*           to determine when to send an EAPOL EAP Request/Identity
*           frame to the Supplicant.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortTxPeriodSet(L7_uint32 intIfNum, L7_uint32 txPeriod);


/*********************************************************************
* @purpose  Get Supplicant timeout value
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    *suppTimeout @b{(output)} Supplicant timeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The suppTimeout is the initialization value for aWhile,
*           which is a timer used by the Authenticator state machine
*           to time out the Supplicant.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortSuppTimeoutGet(L7_uint32 intIfNum, L7_uint32 *suppTimeout);

/*********************************************************************
* @purpose  Set Supplicant timeout value
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    suppTimeout @b{(input)} Supplicant timeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The suppTimeout is the initialization value for aWhile,
*           which is a timer used by the Authenticator state machine
*           to time out the Supplicant.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortSuppTimeoutSet(L7_uint32 intIfNum, L7_uint32 suppTimeout);

/*********************************************************************
* @purpose  Get Authentiation Server timeout value
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    *serverTimeout @b{(output)} Authentication Server timeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The serverTimeout is the initialization value for aWhile,
*           which is a timer used by the Authenticator state machine
*           to time out the Authentiation Server.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortServerTimeoutGet(L7_uint32 intIfNum, L7_uint32 *serverTimeout);

/*********************************************************************
* @purpose  Set Authentiation Server timeout value
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    serverTimeout @b{(input)} Authentication Server timeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The serverTimeout is the initialization value for aWhile,
*           which is a timer used by the Authenticator state machine
*           to time out the Authentiation Server.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortServerTimeoutSet(L7_uint32 intIfNum, L7_uint32 serverTimeout);

/*********************************************************************
* @purpose  Get Maximum Request value
*
* @param    intIfNum @b{(input)} internal interface number
* @param    *maxReq  @b{(output)} maximum request value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The maxReq value is the number of times the Authenticator
*           state machine will retransmit an EAPOL EAP Request/Identity
*           before timing out the Supplicant.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortMaxReqGet(L7_uint32 intIfNum, L7_uint32 *maxReq);

/*********************************************************************
* @purpose  Set Maximum Request value
*
* @param    intIfNum @b{(input)} internal interface number
* @param    maxReq   @b{(input)} maximum request value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The maxReq value is the number of times the Authenticator
*           state machine will retransmit an EAPOL EAP Request/Identity
*           before timing out the Supplicant.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortMaxReqSet(L7_uint32 intIfNum, L7_uint32 maxReq);

/*********************************************************************
* @purpose  Get the Reauthentication period
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    *reAuthPeriod  @b{(output)} reauthentication period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The reAuthPeriod is the initialization value for reAuthWhen,
*           which is a timer used by the Authenticator state machine to
*           determine when reauthentication of the Supplicant takes place.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortReAuthPeriodGet(L7_uint32 intIfNum, L7_uint32 *reAuthPeriod);

/*********************************************************************
* @purpose  Set the Reauthentication period
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    reAuthPeriod  @b{(input)} reauthentication period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The reAuthPeriod is the initialization value for reAuthWhen,
*           which is a timer used by the Authenticator state machine to
*           determine when reauthentication of the Supplicant takes place.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortReAuthPeriodSet(L7_uint32 intIfNum, L7_uint32 reAuthPeriod);

/*********************************************************************
* @purpose  Get the Reauthentication mode
*
* @param    intIfNum        @b{(input)} internal interface number
* @param    *reAuthEnabled  @b{(output)} reauthentication mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The reAuthEnabled mode determines whether reauthentication
*           of the Supplicant takes place.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortReAuthEnabledGet(L7_uint32 intIfNum, L7_BOOL *reAuthEnabled);

/*********************************************************************
* @purpose  Set the Reauthentication mode
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    reAuthEnabled  @b{(input)} reauthentication mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The reAuthEnabled mode determines whether reauthentication
*           of the Supplicant takes place.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortReAuthEnabledSet(L7_uint32 intIfNum, L7_BOOL reAuthEnabled);

/*********************************************************************
* @purpose  Get the Key Transmission mode
*
* @param    intIfNum               @b{(input)} internal interface number
* @param    *keyTranmissionEnabled @b{(output)} key transmission mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The keyTranmissionEnabled mode determines whether key
*           transmission to the Supplicant takes place.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortKeyTransmissionEnabledGet(L7_uint32 intIfNum,
                                                  L7_BOOL *keyTranmissionEnabled);

/*********************************************************************
* @purpose  Set the Key Transmission mode
*
* @param    intIfNum              @b{(input)} internal interface number
* @param    keyTranmissionEnabled @b{(input)} key transmission mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The keyTranmissionEnabled mode determines whether key
*           transmission to the Supplicant takes place.  Note that
*           key transmission is not supported.  The dot1xAuthKeyTxEnabled
*           MIB object is supported as read-only.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortKeyTransmissionEnabledSet(L7_uint32 intIfNum,
                                                  L7_BOOL keyTranmissionEnabled);

/*********************************************************************
* @purpose  Get the dot1x protocol version of a port
*
* @param    intIfNum         @b{(input)} internal interface number
* @param    *protocolVersion @b{(output)} dot1x protocol version of this port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortProtocolVersionGet(L7_uint32 intIfNum,
                                L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_t *protocolVersion);

/*********************************************************************
* @purpose  Get the port's dot1x capabilities (Supplicant and/or Authenticator)
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    *capabilities @b{(output)} dot1x capabilities bitmask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortPaeCapabilitiesGet(L7_uint32 intIfNum,
                                           L7_uchar8 *capabilities);
/*********************************************************************
* @purpose  Set the port's dot1x capabilities (Supplicant and/or Authenticator)
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    capabilities  @b{(input)} dot1x capabilities bitmask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t dot1xPortPaeCapabilitiesSet(L7_uint32 intIfNum,
                                           L7_uchar8 capabilities);


/*********************************************************************
* @purpose  Get the current state of the port's Authenticator PAE state machine
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    *authPaeState @b{(output)} Authenticator PAE state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthPaeStateGet(L7_uint32 intIfNum,
                                        L7_DOT1X_APM_STATES_t *authPaeState);

/*********************************************************************
* @purpose  Get the current state of the port's Backend Authentication state machine
*
* @param    intIfNum          @b{(input)} internal interface number
* @param    *backendAuthState @b{(output)} Backend Authentication state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortBackendAuthStateGet(L7_uint32 intIfNum,
                                            L7_DOT1X_BAM_STATES_t *backendAuthState);

/*********************************************************************
* @purpose  Get operational value of controlled directions
*
* @param    intIfNum                    @b{(input)} internal interface number
* @param    *operControlledDirections   @b{(output)} controlled directions value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Since uni-directional control is currently not supported,
*           this will always return L7_DOT1X_PORT_DIRECTION_BOTH.
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortOperControlledDirectionsGet(L7_uint32 intIfNum,
                                      L7_DOT1X_PORT_DIRECTION_t *operControlledDirections);

/*********************************************************************
* @purpose  Get operational value of controlled directions
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    *portStatus   @b{(output)} port authentication status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthControlledPortStatusGet(L7_uint32 intIfNum,
                                                    L7_DOT1X_PORT_STATUS_t *portStatus);

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
extern L7_RC_t dot1xPortEapolFramesRx(L7_uint32 intIfNum, L7_uint32 *eapolFramesRx);

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
extern L7_RC_t dot1xPortEapolFramesTx(L7_uint32 intIfNum, L7_uint32 *eapolFramesTx);

/*********************************************************************
* @purpose  Get number of EAPOL Start frames received
*
* @param    intIfNum            @b{(input)} internal interface number
* @param    *eapolStartFramesRx @b{(output)} EAPOL start frames received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortEapolStartFramesRx(L7_uint32 intIfNum, L7_uint32 *eapolStartFramesRx);

/*********************************************************************
* @purpose  Get number of EAPOL Logoff frames received
*
* @param    intIfNum             @b{(input)} internal interface number
* @param    *eapolLogoffFramesRx @b{(output)} EAPOL logoff frames received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortEapolLogoffFramesRx(L7_uint32 intIfNum, L7_uint32 *eapolLogoffFramesRx);

/*********************************************************************
* @purpose  Get number of EAP Response/Identity frames received
*
* @param    intIfNum           @b{(input)} internal interface number
* @param    *eapRespIdFramesRx @b{(output)} EAP Resp/Id frames received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortEapRespIdFramesRx(L7_uint32 intIfNum, L7_uint32 *eapRespIdFramesRx);

/*********************************************************************
* @purpose  Get number of EAP Response frames received
*
* @param    intIfNum             @b{(input)} internal interface number
* @param    *eapResponseFramesRx @b{(output)} EAP Response frames received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortEapResponseFramesRx(L7_uint32 intIfNum, L7_uint32 *eapResponseFramesRx);

/*********************************************************************
* @purpose  Get number of EAP Request/Identity frames transmitted
*
* @param    intIfNum          @b{(input)} internal interface number
* @param    *eapReqIdFramesTx @b{(output)} EAP Req/Id frames transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortEapReqIdFramesTx(L7_uint32 intIfNum, L7_uint32 *eapReqIdFramesTx);

/*********************************************************************
* @purpose  Get number of EAP Request frames transmitted
*
* @param    intIfNum        @b{(input)} internal interface number
* @param    *eapReqFramesTx @b{(output)} EAP Request frames transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortEapReqFramesTx(L7_uint32 intIfNum, L7_uint32 *eapReqFramesTx);

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
extern L7_RC_t dot1xPortInvalidEapolFramesRx(L7_uint32 intIfNum, L7_uint32 *invalidEapolFramesRx);

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
extern L7_RC_t dot1xPortEapLengthErrorFramesRx(L7_uint32 intIfNum, L7_uint32 *eapLengthErrorFramesRx);

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
extern L7_RC_t dot1xPortLastEapolFrameVersion(L7_uint32 intIfNum, L7_uint32 *lastEapolFrameVersion);

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
extern L7_RC_t dot1xPortLastEapolFrameSourceGet(L7_uint32 intIfNum, L7_enetMacAddr_t *lastEapolFrameSource);

/*********************************************************************
* @purpose  Get port operational mode
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    *portMode    @b{(output)} port operational mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortOperControlModeGet(L7_uint32 intIfNum, L7_DOT1X_PORT_CONTROL_t *portMode);

/*********************************************************************
* @purpose  Clear dot1x stats for specified port
*
* @param    intIfNum     @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortStatsClear(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get number of times the FSM enters the CONNECTING state from any other state
*
* @param    intIfNum              @b{(input)} internal interface number
* @param    *authEntersConnecting @b{(output)} number of times CONNECTING state entered
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthEntersConnectingGet(L7_uint32 intIfNum, L7_uint32 *authEntersConnecting);

/*********************************************************************
* @purpose  Get number of times the FSM transitions from  CONNECTING to DISCONNECTED
*           as a result of receiving an EAPOL-Logoff message
*
* @param    intIfNum                       @b{(input)} internal interface number
* @param    *authEapLogoffsWhileConnecting @b{(output)} number of logoffs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthEapLogoffsWhileConnectingGet(L7_uint32 intIfNum, L7_uint32 *authEapLogoffsWhileConnecting);

/*********************************************************************
* @purpose  Get the number of times the FSM transitions from CONNECTING to AUTHENTICATING,
*           as a result of an EAP-Response/Identity message being received from the Supplicant
*
* @param    intIfNum                  @b{(input)} internal interface number
* @param    *authEntersAuthenticating @b{(output)} number of EAP-Response/Identity messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthEntersAuthenticatingGet(L7_uint32 intIfNum, L7_uint32 *authEntersAuthenticating);

/*********************************************************************
* @purpose  Get the number of times the FSM transitions from AUTHENTICATING to AUTHENTICATED,
*           as a result of the Backend Authentication state machine indicating successful
*           authentication of the Supplicant
*
* @param    intIfNum                            @b{(input)} internal interface number
* @param    *authAuthSuccessWhileAuthenticating @b{(output)} number of successes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthAuthSuccessWhileAuthenticatingGet(L7_uint32 intIfNum, L7_uint32 *authAuthSuccessWhileAuthenticating);

/*********************************************************************
* @purpose  Get the number of times the FSM transitions from AUTHENTICATING to ABORTING,
*           as a result of the Backend Authentication state machine indicating authentication
*           timeout
*
* @param    intIfNum                             @b{(input)} internal interface number
* @param    *authAuthTimeoutsWhileAuthenticating @b{(output)} number of timeouts
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthAuthTimeoutsWhileAuthenticatingGet(L7_uint32 intIfNum, L7_uint32 *authAuthTimeoutsWhileAuthenticating);

/*********************************************************************
* @purpose  Get the number of times the FSM transitions from AUTHENTICATING to HELD,
*           as a result of the Backend Authentication state machine indicating authentication
*           failure
*
* @param    intIfNum                         @b{(input)} internal interface number
* @param    *authAuthFailWhileAuthenticating @b{(output)} number of failures
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthAuthFailWhileAuthenticatingGet(L7_uint32 intIfNum, L7_uint32 *authAuthFailWhileAuthenticating);

/*********************************************************************
* @purpose  Get the number of times the FSM transitions from AUTHENTICATING to ABORTING,
*           as a result of a reauthentication request
*
* @param    intIfNum                            @b{(input)} internal interface number
* @param    *authAuthReauthsWhileAuthenticating @b{(output)} number of reauthentication requests
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthAuthReauthsWhileAuthenticatingGet(L7_uint32 intIfNum, L7_uint32 *authAuthReauthsWhileAuthenticating);

/*********************************************************************
* @purpose  Get the number of times the FSM transitions from AUTHENTICATING to ABORTING,
*           as a result of an EAPOL-Start message being received from the Supplicant
*
* @param    intIfNum                              @b{(input)} internal interface number
* @param    *authAuthEapStartsWhileAuthenticating @b{(output)} number of EAP-Start messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthAuthEapStartsWhileAuthenticatingGet(L7_uint32 intIfNum, L7_uint32 *authAuthEapStartsWhileAuthenticating);

/*********************************************************************
* @purpose  Get the number of times the FSM transitions from AUTHENTICATING to ABORTING,
*           as a result of an EAPOL-Logoff message being received from the Supplicant
*
* @param    intIfNum                              @b{(input)} internal interface number
* @param    *authAuthEapLogoffWhileAuthenticating @b{(output)} number of EAP-Logoff messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthAuthEapLogoffWhileAuthenticatingGet(L7_uint32 intIfNum, L7_uint32 *authAuthEapLogoffWhileAuthenticating);

/*********************************************************************
* @purpose  Get the number of times the FSM transitions from AUTHENTICATED to CONNECTING,
*           as a result of a reauthentication request
*
* @param    intIfNum                           @b{(input)} internal interface number
* @param    *authAuthReauthsWhileAuthenticated @b{(output)} number of reauthentication requests
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthAuthReauthsWhileAuthenticatedGet(L7_uint32 intIfNum, L7_uint32 *authAuthReauthsWhileAuthenticated);

/*********************************************************************
* @purpose  Get the number of times the FSM transitions from AUTHENTICATED to CONNECTING,
*           as a result of an EAPOL-Start message being received from the Supplicant
*
* @param    intIfNum                             @b{(input)} internal interface number
* @param    *authAuthEapStartsWhileAuthenticated @b{(output)} number of EAPOL-Start messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthAuthEapStartsWhileAuthenticatedGet(L7_uint32 intIfNum, L7_uint32 *authAuthEapStartsWhileAuthenticated);

/*********************************************************************
* @purpose  Get the number of times the FSM transitions from AUTHENTICATED to DISCONNECTED,
*           as a result of an EAPOL-Logoff message being received from the Supplicant
*
* @param    intIfNum                             @b{(input)} internal interface number
* @param    *authAuthEapLogoffWhileAuthenticated @b{(output)} number of EAPOL-Logoff messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthAuthEapLogoffWhileAuthenticatedGet(L7_uint32 intIfNum, L7_uint32 *authAuthEapLogoffWhileAuthenticated);

/*********************************************************************
* @purpose  Get the number of times the FSM sends an initial Access-Request packet to the
*           Authentication server
*
* @param    intIfNum              @b{(input)} internal interface number
* @param    *authBackendResponses @b{(output)} number of Access-Request packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthBackendResponsesGet(L7_uint32 intIfNum, L7_uint32 *authBackendResponses);

/*********************************************************************
* @purpose  Get the number of times the FSM receives an initial Access-Challenge packet from
*           the Authentication server
*
* @param    intIfNum                     @b{(input)} internal interface number
* @param    *authBackendAccessChallenges @b{(output)} number of Access-Challenge packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthBackendAccessChallengesGet(L7_uint32 intIfNum, L7_uint32 *authBackendAccessChallenges);

/*********************************************************************
* @purpose  Get the number of times the FSM sends an EAP-Request packet
*
* @param    intIfNum                              @b{(input)} internal interface number
* @param    *authBackendOtherRequestsToSupplicant @b{(output)} number of EAP-Request packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthBackendOtherRequestsToSupplicantGet(L7_uint32 intIfNum, L7_uint32 *authBackendOtherRequestsToSupplicant);

/*********************************************************************
* @purpose  Get the number of times the FSM receives a response from the Supplicant to an
*           initial EAP-Request, and the response is something other than EAP-NAK
*
* @param    intIfNum                                  @b{(input)} internal interface number
* @param    *authBackendNonNakResponsesFromSupplicant @b{(output)} number of non-Nak responses
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthBackendNonNakResponsesFromSupplicantGet(L7_uint32 intIfNum, L7_uint32 *authBackendNonNakResponsesFromSupplicant);

/*********************************************************************
* @purpose  Get the number of times the FSM receives an EAP-Success message from the
*           Authentication Server
*
* @param    intIfNum                  @b{(input)} internal interface number
* @param    *authBackendAuthSuccesses @b{(output)} number of EAP-Success messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthBackendAuthSuccessesGet(L7_uint32 intIfNum, L7_uint32 *authBackendAuthSuccesses);

/*********************************************************************
* @purpose  Get the number of times the FSM receives an EAP-Failure message from the
*           Authentication Server
*
* @param    intIfNum              @b{(input)} internal interface number
* @param    *authBackendAuthFails @b{(output)} number of EAP-Success messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t dot1xPortAuthBackendAuthFailsGet(L7_uint32 intIfNum, L7_uint32 *authBackendAuthFails);


/*********************************************************************
* @purpose  Determine if the interface is valid to participate in dot1x
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_BOOL dot1xIsValidIntf(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine if the interface type is valid to participate in dot1x
*
* @param    sysIntfType              @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL dot1xIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Return Internal Interface Number of next valid interface for
*           dot1x.
*
* @param    intIfNum  @b{(input)}   Internal Interface Number
* @param    pNextintIfNum @b{(output)}  pointer to Next Internal Interface Number,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none 
*
* @end
*********************************************************************/
extern L7_RC_t dot1xNextValidIntf(L7_uint32 intIfNum, L7_uint32 *pNextIntIfNum);

/*********************************************************************
* @purpose  Return Internal Interface Number of the first valid interface for
*           dot1x.
*
* @param    pFirstIntIfNum @b{(output)}  pointer to first internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none 
*
* @end
*********************************************************************/
extern L7_RC_t dot1xFirstValidIntfNumber(L7_uint32 *pFirstIntIfNum);

/*********************************************************************
* @purpose  Set administrative mode setting for dot1x Vlan Assignment
*
* @param    mode @b{(input)} radius vlan assignment mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1xVlanAssignmentModeSet (L7_uint32 mode);

/*********************************************************************
* @purpose  Get administrative mode setting for dot1x Vlan Assignment
*
* @param    mode @b{(input)} radius vlan assignment mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1xVlanAssignmentModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Set administrative mode setting for dot1x dynamic Vlan creation
*
* @param    mode @b{(input)} radius dynamic vlan creation mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xDynamicVlanCreationModeSet (L7_uint32 mode);

/*********************************************************************
* @purpose  Get administrative mode setting for dot1x dynamic Vlan creation
*
* @param    mode @b{(input)} radius dynamic vlan creation mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xDynamicVlanCreationModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Get the Vlan Assigned to a port by dot1x.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    *vlanId       @b{(output)} vlan Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
L7_RC_t dot1xPortVlanAssignedGet(L7_uint32 intIfNum, L7_uint32 *vlanId);

/*********************************************************************
* @purpose  Get the reason vlan was assigned to a port by dot1x.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    *reason       @b{(output)} reason vlan was assigned to the port by dot1x
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
L7_RC_t dot1xPortVlanAssignedReasonGet(L7_uint32 intIfNum, L7_uint32 *reason);

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
L7_RC_t dot1xPortUsernameGet(L7_uint32 intIfNum,L7_uchar8 *username);

/*********************************************************************
* @purpose  Set the Guest Vlan Id for the port.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    *guestVlanId       @b{(output)} guest vlan Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xAdvancedGuestPortsCfgSet(L7_uint32 intIfNum,L7_uint32 guestVlanId);

/*********************************************************************
* @purpose  Set the Guest Vlan Id for the port.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    *guestVlanId  @b{(output)} guest vlan Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xAdvancedGuestPortCfgGet(L7_uint32 intIfNum,L7_uint32 *guestVlanId);

/*********************************************************************
* @purpose  Set the Guest Vlan Period for the port.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    guestVlanPeriod  @b{(output)} guest vlan Period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xAdvancedGuestVlanPeriodSet(L7_uint32 intIfNum,L7_uint32 guestVlanPeriod);

/*********************************************************************
* @purpose  Get the Guest Vlan Period for the port.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    *guestVlanPeriod  @b{(output)} guest vlan Period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xAdvancedGuestVlanPeriodGet(L7_uint32 intIfNum,L7_uint32 *guestVlanPeriod);

/*********************************************************************
* @purpose  Set max users value
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    maxUsers @b{(input)} max users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The maxUsers is the maximum number of hosts that can be 
*           authenticated on a port using mac based authentication
*       
* @end
*********************************************************************/
L7_RC_t dot1xPortMaxUsersSet(L7_uint32 intIfNum, L7_uint32 maxUsers);

/*********************************************************************
* @purpose  Get max users value
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    *maxUsers @b{(output)} max users per port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The maxUsers is the maximum number of hosts that can be 
*           authenticated on a port using mac based authentication
*       
* @end
*********************************************************************/
L7_RC_t dot1xPortMaxUsersGet(L7_uint32 intIfNum, L7_uint32 *maxUsers);

/*********************************************************************
* @purpose  Set initialize value for a logical port
*
* @param    lIntIfNum   @b{(input)} Logical internal interface number
* @param    initialize @b{(input)} initialize value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This value is set to L7_TRUE by management in order to force
*           initialization of a port.  It is re-set to L7_FALSE after
*           initialization has completed.
*       
* @end
*********************************************************************/
L7_RC_t dot1xLogicalPortInitializeSet(L7_uint32 lIntIfNum, L7_BOOL initialize);

/*********************************************************************
* @purpose  Set reauthentication value for a logical port
*
* @param    lIntIfNum       @b{(input)} logical interface number
* @param    reauthenticate @b{(input)} reauthentication value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This value is set to L7_TRUE by management in order to force
*           reauthentication of a port.  It is re-set to L7_FALSE after
*           reauthentication has completed.
*       
* @end
*********************************************************************/
L7_RC_t dot1xLogicalPortReauthenticateSet(L7_uint32 lIntIfNum, L7_BOOL reauthenticate);

/*********************************************************************
* @purpose  Returns Session time
*
* @param    lIntIfNum @b((input)) the specified interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dot1xPortSessionTimeGet( L7_uint32 lIntIfNum, L7_uint32 *pval);

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
L7_RC_t dot1xLogicalPortFirstGet(L7_uint32 intIfNum,L7_uint32 *lIntIfNum);

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
L7_RC_t dot1xLogicalPortNextGet(L7_uint32 lIntIfNum,L7_uint32 *nextIntf);

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
L7_RC_t dot1xLogicalPortSupplicantMacAddrGet(L7_uint32 lIntIfNum,
                                             L7_uchar8 *macAddr);

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
L7_RC_t dot1xLogicalPortPaeStateGet(L7_uint32 lIntIfNum,
                                    L7_uint32 *authPaeState);

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
L7_RC_t dot1xLogicalPortBackendAuthStateGet(L7_uint32 lIntIfNum,
                                            L7_uint32 *state);

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
L7_RC_t dot1xLogicalPortVlanAssignmentGet(L7_uint32 lIntIfNum,
                                          L7_uint32 *vlanId,
                                          L7_uint32 *mode);



/*********************************************************************
* @purpose  Returns the User Name for the  logical interface
*
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
L7_RC_t dot1xLogicalPortUserNameGet(L7_uint32 lIntIfNum,
                                    L7_uchar8 *userName);

/*********************************************************************
* @purpose  Returns the filter id for the  logical interface
*
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
L7_RC_t dot1xLogicalPortFilterIdGet(L7_uint32 lIntIfNum,
                                    L7_uchar8 *filter);

/*********************************************************************
* @purpose  Set Unathenticated vlan 
*
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
L7_RC_t dot1xPortUnauthenticatedVlanSet(L7_uint32 intIfNum, L7_uint32 vlanId);

/*********************************************************************
* @purpose  Get UnatuthenticatedVlan value
*
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
L7_RC_t dot1xPortUnauthenticatedVlanGet(L7_uint32 intIfNum, L7_uint32 *vlanId);

/*********************************************************************
* @purpose  Get the port's session timeout value
*
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
L7_RC_t dot1xPortSessionTimeoutGet(L7_uint32 intIfNum, L7_uint32 *sessionTimeout);

/*********************************************************************
* @purpose  Returns the session timeout value for the  logical interface
*
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
L7_RC_t dot1xLogicalPortSessionTimeoutGet(L7_uint32 lIntIfNum,
                                    L7_uint32 *session_timeout);

/*********************************************************************
* @purpose  Get the ports termination action value
*
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
L7_RC_t dot1xPortTerminationActionGet(L7_uint32 intIfNum, L7_uint32 *terminationAction);

/*********************************************************************
* @purpose  Returns the termination Action for the  logical interface
*
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
L7_RC_t dot1xLogicalPortTerminationActionGet(L7_uint32 lIntIfNum,
                                    L7_uint32 *terminationAction);


/*********************************************************************
* @purpose  Returns the logical port for the corresponding supplicant Mac Address
*
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
L7_RC_t dot1xClientMacAddressGet(L7_enetMacAddr_t *macAddr,L7_uint32 *lIntIfNum);

/*********************************************************************
* @purpose  Returns the logical port for the next supplicant Mac Address
*           in the next mac address database
*
* @param    mac_addr    @b{(input)} supplicant mac address to be searched
* @param    lIntIfNum    @b((output)) the logical interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @comments for SNMP
*
* @end
*********************************************************************/
L7_RC_t dot1xClientMacAddressNextGet(L7_enetMacAddr_t *macAddr,L7_uint32 *lIntIfNum);

/*********************************************************************
* @purpose  Returns the physical port corresponding to the logical interface
*
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
L7_RC_t dot1xPhysicalPortGet(L7_uint32 lIntIfNum,L7_uint32 *physPort);
/*********************************************************************
*
* @purpose  Callback from DTL informing about an unauthorized address
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1xUnauthAddrCallBack( L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId );

/*********************************************************************
*
* @purpose  Enable/Disable Voice Vlan Device authorization.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
* @param    L7_BOOL          flag       @b{(input)} TRUE - Enable, FALSE - DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dot1xPortVoiceVlanAuthMode( L7_uint32 intIfNum, 
                                    L7_enetMacAddr_t macAddr, 
                                    L7_ushort16 vlanId,
                                    L7_BOOL  flag);


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
L7_RC_t dot1xSupplicantPortMaxStartGet(L7_uint32 intIfNum,
                                    L7_uint32 *maxStart);

/*********************************************************************
* @purpose  Set the Port's Max start value
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    maxStart   @b((input)) Max start value
*
* @returns  L7_SUCCESS
*
* @notes    The max start value represents the maximum number of
*           successive EAPOL start messages that will be sent before the
*           Supplicant assumes that there is no Authenticator is present
*
* @end
*********************************************************************/
L7_RC_t dot1xSupplicantPortMaxStartSet(L7_uint32 intIfNum,
                                    L7_uint32 maxStart);


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
L7_RC_t dot1xPortSupplicantPortStartPeriodGet(L7_uint32 intIfNum,
                                           L7_uint32 *startPeriod);

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
L7_RC_t dot1xPortSupplicantPortStartPeriodSet(L7_uint32 intIfNum,
                                           L7_uint32 startPeriod);

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
*           determine when to send start message on previous authentication failure.
*
* @end
*********************************************************************/
L7_RC_t dot1xSupplicantPortHeldPeriodGet(L7_uint32 intIfNum,
                                             L7_uint32 *heldPeriod);


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
L7_RC_t dot1xSupplicantPortHeldPeriodSet(L7_uint32 intIfNum,
                                          L7_uint32 heldPeriod);

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
L7_RC_t dot1xSupplicantPortAuthPeriodGet(L7_uint32 intIfNum,
                                     L7_uint32 *authPeriod);

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
L7_RC_t dot1xSupplicantPortAuthPeriodSet(L7_uint32 intIfNum,
                                               L7_uint32 authPeriod);

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
L7_RC_t dot1xSupplicantPortControlModeGet(L7_uint32 intIfNum, 
                                     L7_uint32 *portControl);

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
L7_RC_t dot1xSupplicantPortControlModeSet(L7_uint32 intIfNum, 
                                           L7_uint32 portControl);


/*********************************************************************
* @purpose  Get the Supplicant PAE state 
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    *spmState   @b((output)) Supplican't SPM state mode.
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1xSupplicantPortPaeStateGet(L7_uint32 intIfNum,
                                           L7_DOT1X_SPM_STATES_t *spmState);

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
L7_RC_t dot1xSupplicantPortSpmStatusGet(L7_uint32 intIfNum,
                                           L7_uchar8 *spmStatus);


/*********************************************************************
* @purpose  Get the current state of the port's Supplicant Backend
*           state machine
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    *suppSbmState @b{(output)} Backend Machine state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSupplicantPortSbmStateGet(L7_uint32 intIfNum,
                                 L7_DOT1X_SBM_STATES_t *suppSbmState);

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
L7_RC_t dot1xSupplicantPortUserNameSet(L7_uint32 intIfNum,
                                         L7_uchar8 * userName,
                                         L7_uint32 userLen);

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
L7_RC_t dot1xSupplicantPortUserNameGet(L7_uint32 intIfNum,
                                     L7_uchar8 *userName);


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
L7_RC_t dot1xSuppicantPortEapolFramesRx(L7_uint32 intIfNum,
                                         L7_uint32 *eapolFramesRx);

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
L7_RC_t dot1xSuppicantPortEapolFramesTx(L7_uint32 intIfNum,
                                         L7_uint32 *eapolFramesTx);

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
L7_RC_t dot1xSuppicantPortEapolStartFramesTx(L7_uint32 intIfNum,
                                         L7_uint32 *eapolStartFramesTx);

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
L7_RC_t dot1xSuppicantPortEapolLogoffFramesTx(L7_uint32 intIfNum,
                                         L7_uint32 *eapolLogoffFramesTx);

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
L7_RC_t dot1xSuppicantPortEapolRespIdFramesTx(L7_uint32 intIfNum,
                                         L7_uint32 *eapolRespIdFramesTx);

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
L7_RC_t dot1xSuppicantPortEapolResponseFramesTx(L7_uint32 intIfNum,
                                         L7_uint32 *eapolResponseFramesTx);

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
L7_RC_t dot1xSuppicantPortEapolReqIdFramesRx(L7_uint32 intIfNum,
                                         L7_uint32 *eapolReqIdFramesRx);
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
L7_RC_t dot1xSuppicantPortEapolRequestFramesRx(L7_uint32 intIfNum,
                                         L7_uint32 *eapolReqFramesRx);
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
L7_RC_t dot1xSupplicantPortInvalidEapolFramesRx(L7_uint32 intIfNum,
                                  L7_uint32 *invalidEapolFramesRx);

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
L7_RC_t dot1xSupplicantPortEapLengthErrorFramesRx(L7_uint32 intIfNum,
                                     L7_uint32 *eapLengthErrorFramesRx);

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
L7_RC_t dot1xSupplicantPortLastEapolFrameVersion(L7_uint32 intIfNum,
                                      L7_uint32 *lastEapolFrameVersion);

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
L7_RC_t dot1xSupplicantPortLastEapolFrameSourceGet(L7_uint32 intIfNum,
                                 L7_enetMacAddr_t *lastEapolFrameSource);

/*********************************************************************
* @purpose  Get the port autherization status.
*
* @param    intIfNum               @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t dot1xPortIsAuthorized(L7_uint32 intIfNum);

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
L7_RC_t dot1xPortMABEnabledGet(L7_uint32 intIfNum, L7_uint32 *mabEnabled);

/*********************************************************************
* @purpose  Get the operational MAB value on the port.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    mabEnabled   @b{(output)} value detrmining if MAB 
*                                     has been operationally 
*                                     enabled on the port 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
L7_RC_t dot1xPortOperMABEnabledGet(L7_uint32 intIfNum, L7_uint32 *mabEnabled);

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
*
* @comments
*       
* @end
*********************************************************************/
L7_RC_t dot1xPortMABEnabledSet(L7_uint32 intIfNum, L7_uint32 mabEnabled);

/*********************************************************************
* @purpose  Determine if a client is authenticated on an interface
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    macAddr       @b{(input)} client's MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL dot1xPortClientAuthenticationGet(L7_uint32 intIfNum, L7_uchar8 *macAddr);
/*********************************************************************
* @purpose  Set Monitor mode setting for dot1x
*
* @param    monitorMode @b{(input)} dot1x Monitor mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1xMonitorModeSet(L7_uint32 monitorMode);

/*********************************************************************
* @purpose  Get Monitor mode setting for dot1x
*
* @param    *monitorMode @b{(output)} dot1x monitor mode
*
* @returns  L7_SUCCESS
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1xMonitorModeGet(L7_uint32 *monitorMode);


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
L7_RC_t dot1xMonitorModeNumClientsGet(L7_uint32 *pCount); 

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
L7_RC_t dot1xNonMonitorModeNumClientsGet(L7_uint32 *pCount);



/*********************************************************************
 * @purpose  Get the timestamp from the dot1x Auth History table
 *          
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
L7_RC_t dot1xAuthHistoryLogTimestampGet(L7_uint32 intIfNum,
                                        L7_uint32 entryIndex,
                                        L7_uint32 *pTimeStamp); 

/*********************************************************************
 * @purpose  Get the reasonCode from the dot1x Auth History table
 *          
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
L7_RC_t dot1xAuthHistoryLogReasonCodeGet(L7_uint32 intIfNum,
                                        L7_uint32 entryIndex,
                                        L7_uint32 *pReasonCode); 

/*********************************************************************
 * @purpose  Get the VlanId from the dot1x Auth History table
 *          
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
L7_RC_t dot1xAuthHistoryLogVlanIdGet(L7_uint32   intIfNum,
                                     L7_uint32   entryIndex,
                                     L7_ushort16 *pVlanId); 

/*********************************************************************
 * @purpose  Get the accessStatus from the dot1x Auth History table
 *          
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
L7_RC_t dot1xAuthHistoryLogAccessStatusGet(L7_uint32 intIfNum,
                                        L7_uint32 entryIndex,
                                        L7_uint32 *pStatus); 

/*********************************************************************
 * @purpose  Get the dot1x authentication Status from the dot1x Auth 
 *           History table
 *          
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
L7_RC_t dot1xAuthHistoryLogAuthStatusGet(L7_uint32 intIfNum,
                                        L7_uint32 entryIndex,
                                        L7_uint32 *pStatus); 

/*********************************************************************
 * @purpose  Get the supplicant Mac Address from the dot1x Auth History table
 *          
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
L7_RC_t dot1xAuthHistoryLogSupplicantMacAddressGet(L7_uint32 intIfNum,
                                                   L7_uint32 entryIndex,
                                                   L7_enetMacAddr_t *pMacAddress);

/*********************************************************************
 * @purpose  Get the FilterID for the Dot1x Radius Accept Packet
 *           located in the dot1x Auth History table
 *          
 * @param    intIfNum   @b{(input)} Physical Interface Number
 * @param    entryIndex @b{(input)} EntryIndex
 * @param    pFilterId  @b{(output)} Reference to the filter Id
 * @param    pFilterLen @b{(output)} Reference to filter Length
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
L7_RC_t dot1xAuthHistoryLogFilterIdGet(L7_uint32 intIfNum,
                                       L7_uint32 entryIndex,
                                       L7_uchar8 *pFilterIdName,
                                       L7_uint32 *pFilterLen); 

/*********************************************************************
 * @purpose  Get the Vlan Assigned from the dot1x Auth History table
 *          
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
L7_RC_t dot1xAuthHistoryLogVlanIdAssignedGet(L7_uint32   intIfNum,
                                             L7_uint32   entryIndex,
                                             L7_ushort16 *pVlanId);

/*********************************************************************
 * @purpose  Get the Vlan Assigned Type from the dot1x Auth History table
 *          
 * @param    intIfNum   @b{(input)} Physical Interface Number
 * @param    entryIndex @b{(input)} EntryIndex
 * @param    pVlanAssignedType @b{(output)} reference to the VLAN 
                                            assigned Type.
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
L7_RC_t dot1xAuthHistoryLogVlanAssignedTypeGet(L7_uint32 intIfNum,
                                               L7_uint32 entryIndex,
                                               L7_uint8  *pVlanAssignedType);

/*********************************************************************
 * @purpose  Get the MAB enable status from the dot1x Auth History table
 *          
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
L7_RC_t dot1xAuthHistoryLogMABStatusGet(L7_uint32 intIfNum,
                                        L7_uint32 entryIndex,
                                        L7_uint8  *pMABStatus);


/*********************************************************************
 * @purpose  Check if the dot1x auth history log entry exists in History
 *           table
 *          
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
L7_RC_t dot1xAuthHistoryLogEntryIsValid(L7_uint32 intIfNum,
                                        L7_uint32 entryIndex);

/******************************************************************************
 * * @purpose  Gets the next History Log entry indexs
 * *
 * * @param    intIfNum   @b{(output)} Physical Interface Number
 * * @param    entryIndex @b{(output)} EntryIndex
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE  If there are no entries
 * *
 * * @notes  
 * *
 * * @end
 * ******************************************************************************/
L7_RC_t dot1xAuthHistoryLogIndexNextGet(L7_uint32 *intIfNum,
                                        L7_uint32 *entryIndex);

/******************************************************************************
* @purpose  Gets the next History Log interface Index
*
* @param    intIfNum   @b{(input/output)} Physical Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  If there are no entries
*
* @notes  
*
* @end
******************************************************************************/
L7_RC_t dot1xAuthHistoryLogIfIndexNextGet(L7_uint32 *intIfNum);

/*********************************************************************
 * @purpose  Purge all dot1x auth history log entries for the given 
 *           interface
 *          
 * @param    intIfNum   @b{(input)} Physical Interface Number
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
 L7_RC_t dot1xAuthHistoryLogInterfacePurge(L7_uint32 intIfIndex);

/*********************************************************************
 * @purpose  Purge all dot1x auth history log entries 
 *          
 * @param    unitIndex @b((input)) the unit for this operation
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
 L7_RC_t dot1xAuthHistoryLogPurgeAll();


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
L7_RC_t dot1xAuthHistoryLogReasonStringGet(L7_uint32  intIfNum,
                                           L7_uint32  entryIndex,
                                           L7_uint32  reasonCode,
                                           L7_char8  *strReason,
                                           L7_uint32 *strLen);


/******************************************************************************
 * @purpose  Gets the next History Log entry indexs in reverse order
 *
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
L7_RC_t dot1xAuthHistoryLogReverseIndexNextGet(L7_uint32 *intIfNum,
                                               L7_uint32 *entryIndex);


#endif /* INCLUDE_DOT1X_API_H */
