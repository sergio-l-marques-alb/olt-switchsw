/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_ih.h
*
* @purpose   Multiple Spanning tree Interface Handler
*
* @component dot1s
*
* @comments 
*
* @create    9/19/2002
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1S_IH_H
#define INCLUDE_DOT1S_IH_H

/***************************************************************************
 **************************FUNCTION PROTOTYPES******************************
 ***************************************************************************
 */

#define DOT1S_STATE_SET_CB_SIZE ((L7_MAX_PORT_COUNT+L7_MAX_NUM_LAG_INTF)*(L7_MAX_MULTIPLE_STP_INSTANCES +1)*4)

#define DOT1S_STATE_SET_TIMEOUT (90000) /*in milliseconds*/

L7_uint32 dot1sIhNimEventPendingGet (void);
L7_RC_t dot1sIhNotifySystem(L7_uint32 intIfNum, L7_uint32 intIfEvent);
L7_RC_t dot1sIntfChangeCallBack(L7_uint32 intIfNum, L7_uint32 intIfEvent,NIM_CORRELATOR_t correlator);
void dot1sIntfStartupCallBack(NIM_STARTUP_PHASE_t startup_phase);
L7_RC_t dot1sIntfActivateStartup();
L7_RC_t dot1sIntfActivate(L7_uint32 intIfNum);
L7_uint32 dot1sIhProcessIntfChange(L7_uint32 intIfNum, NIM_EVENT_COMPLETE_INFO_t status);
L7_RC_t dot1sProcessIntfStartupCallBack(NIM_STARTUP_PHASE_t startup_phase);
void dot1sIhAcquire(DOT1S_PORT_COMMON_t *p);
void dot1sIhRelease(DOT1S_PORT_COMMON_t *p);
void dot1sIhDoNotify(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 intIfEvent);
void dot1sIhDoNotifyAllInstances(L7_uint32 intIfNum, L7_uint32 intIfEvent);
L7_RC_t dot1sIhDoNotifyInstanceCreate(L7_uint32 instNumber);
L7_RC_t dot1sIhDoNotifyInstanceDelete(L7_uint32 instNumber);
L7_RC_t dot1sIhIntfSpeedDataRateGet(L7_uint32 intIfNum, L7_uint32 *dataRate);
L7_BOOL dot1sIhIsIntfSpeedFullDuplex(L7_uint32 intIfNum);
L7_RC_t dot1sIhIntfSpeedGet(L7_uint32 intIfNum, L7_uint32 *speed);
L7_RC_t dot1sIhIntfLinkStateGet(L7_uint32 intIfNum, L7_uint32 *state);
L7_RC_t dot1sIhIntfAddrGet(L7_uint32 intIfNum, L7_uint32 addrType, L7_uchar8 *macAddr);
L7_RC_t dot1sIhIntfUspGet(L7_uint32 intIfNum, DOT1S_USP_t *usp);
L7_RC_t dot1sIhIntfAutoNegStatusGet(L7_uint32 intIfNum, L7_uint32 *autoNegoStatus);
L7_BOOL dot1sIhIsFilterMember(L7_uint32 intIfNum);
L7_RC_t dot1sIhHighestIntfNumGet(L7_uint32 *highestIntfNumber);
void dot1sIhPortStateAllSet(L7_uint32 intIfNum, L7_uint32 state);
L7_RC_t dot1sIntfCreate(L7_uint32 intIfNum);
L7_RC_t dot1sIntfDelete(L7_uint32 intIfNum);
L7_RC_t dot1sIntfDetach(L7_uint32 intIfNum);
L7_BOOL dot1sMapIntfIsConfigurable(L7_uint32 intIfNum, DOT1S_PORT_COMMON_CFG_t **pCfg);
L7_RC_t dot1sIntfCfgToPortCopy(DOT1S_PORT_COMMON_CFG_t *pPortCfg);
L7_BOOL dot1sIsPortForwarding(L7_uint32 intIfNum, L7_uint32 instIndex);
L7_BOOL dot1sIsPortDiscarding(L7_uint32 intIfNum, L7_uint32 instIndex);
L7_RC_t dot1sDiagDisablePort(L7_uint32 errIntIfNum);
void dot1sIhStateSet(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 state);
void dot1sStateSetTimerExpired(L7_uint32 intIfNum, L7_uint32 state);
void dot1sStateSetTimerExpiredProcess(L7_uint32 intIfNum);
L7_RC_t dot1sStateSetCallback(L7_uint32 intIfNum, 
							  L7_uint32 instance, 
							  L7_uint32 state, 
							  dot1s_stateCB_t stateStatus);
L7_RC_t dot1sStateSetCallbackProcess(L7_uint32 intIfNum, 
							  L7_uint32 instance, 
							  dot1s_stateCB_t *stateStatus);
L7_RC_t dot1sStateSetAction(L7_uint32 intIfNum, 
							  L7_uint32 instance, 
							  L7_uint32 state);
L7_uint32 dot1sStateSetBookkeeping(L7_uint32 intIfNum, 
								   dot1s_stateCB_t *stateStatus);
void dot1sStateSetNimAction(L7_uint32 intIfNum, L7_uint32 state);
void dot1sStateSetProtocolAction(L7_uint32 intIfNum, L7_uint32 instance, 
                                 L7_uint32 state, L7_BOOL reconInProgress,
                                 L7_BOOL forceNotify);
void dot1sStateSetTimerStart();

L7_RC_t dot1sPortEnabledAction(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPortDisabledAction(DOT1S_PORT_COMMON_t *p);
L7_BOOL dot1sIsStateCBFull();
L7_BOOL dot1sIsStateCBEmpty();
L7_uint32 dot1sStateCBDeQueue();
L7_RC_t dot1sApplyIntfConfigData(L7_uint32 intIfNum);
L7_RC_t dot1sUnApplyIntfConfigData(L7_uint32 intIfNum);

#endif /*INCLUDE_DOT1S_IH_H*/
