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

#ifdef _L7_OS_LINUX_
/* Use a longer time-out, as Linux platforms have relatively slow performance */
#define DOT1S_STATE_SET_TIMEOUT (40000) /*in milliseconds*/
#else
#define DOT1S_STATE_SET_TIMEOUT (20000) /*in milliseconds*/
#endif

L7_uint32 dot1sIhNimEventPendingGet (void);
L7_RC_t dot1sIhNotifySystem(L7_uint32 intIfNum, L7_uint32 intIfEvent);
L7_RC_t dot1sIntfChangeCallBack(L7_uint32 intIfNum, L7_uint32 intIfEvent,NIM_CORRELATOR_t correlator);
L7_uint32 dot1sIhProcessIntfChange(L7_uint32 intIfNum, NIM_EVENT_COMPLETE_INFO_t status);
void dot1sIhAcquire(L7_uint32 intIfNum);
void dot1sIhRelease(L7_uint32 intIfNum);
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
L7_BOOL dot1sMapIntfConfigEntryGet(L7_uint32 intIfNum, DOT1S_PORT_COMMON_CFG_t **pCfg);
L7_BOOL dot1sMapIntfIsConfigurable(L7_uint32 intIfNum, DOT1S_PORT_COMMON_CFG_t **pCfg);
L7_RC_t dot1sIntfCfgToPortCopy(DOT1S_PORT_COMMON_CFG_t *pPortCfg);
L7_BOOL dot1sIsPortForwarding(L7_uint32 intIfNum, L7_uint32 instIndex);
L7_BOOL dot1sIsPortDiscarding(L7_uint32 intIfNum, L7_uint32 instIndex);
void dot1sIhStateSet(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 state);
void dot1sStateSetTimerExpired(L7_uint32 intIfNum, L7_uint32 state);
L7_RC_t dot1sStateSetCallback(L7_uint32 intIfNum, 
							  L7_uint32 instance, 
							  L7_uint32 state, 
							  dot1s_stateCB_t stateStatus);
L7_uint32 dot1sStateSetBookkeeping(L7_uint32 intIfNum, 
								   dot1s_stateCB_t stateStatus);
void dot1sStateSetNimAction(L7_uint32 intIfNum, L7_uint32 state);
void dot1sStateSetProtocolAction(L7_uint32 intIfNum, L7_uint32 instance, L7_uint32 state);
void dot1sStateSetTimerStart();
#endif /*INCLUDE_DOT1S_IH_H*/
