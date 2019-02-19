/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_ih.h
*
* @purpose   dot1x Interface Handler
*
* @component dot1x
*
* @comments  none
*
* @create    01/31/2003
*
* @author    mfiorito
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1X_IH_H
#define INCLUDE_DOT1X_IH_H

extern L7_RC_t dot1xIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 intfEvent,NIM_CORRELATOR_t correlator);
extern L7_RC_t dot1xIhProcessIntfChange(L7_uint32 intIfNum, L7_uint32 intfEvent, NIM_CORRELATOR_t correlator);
extern L7_RC_t dot1xIhIntfValidate(L7_uint32 intIfNum);
extern L7_RC_t dot1xIhPortStatusSet(L7_uint32 intIfNum, L7_DOT1X_PORT_STATUS_t portStatus);
/*extern L7_RC_t dot1xIhPhyPortStatusSet(L7_uint32 intIfNum, L7_DOT1X_PORT_STATUS_t portStatus);*/
extern L7_RC_t dot1xIhPhyPortStatusSet(L7_uint32 intIfNum, L7_DOT1X_PORT_STATUS_t portStatus,L7_BOOL bNotifyNim);
extern L7_BOOL dot1xIntfIsConfigurable(L7_uint32 intIfNum, dot1xPortCfg_t **pCfg);
extern L7_BOOL dot1xIntfConfigEntryGet(L7_uint32 intIfNum, dot1xPortCfg_t **pCfg);
extern L7_RC_t dot1xIntfCreate(L7_uint32 intIfNum);
extern L7_RC_t dot1xIntfDetach(L7_uint32 intIfNum);
extern L7_RC_t dot1xIntfDelete(L7_uint32 intIfNum);
extern L7_RC_t dot1xIhPhyPortViolationCallbackSet(L7_uint32 intIfNum, L7_BOOL flag);

#endif /*INCLUDE_DOT1X_IH_H*/
