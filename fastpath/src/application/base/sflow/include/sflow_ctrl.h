/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename sflow_ctrl.h
*
* @purpose   SFlow 
*
* @component sflow
*
* @comments 
*
* @create 20-Nov-2007
*
* @author drajendra
* @end
*
**********************************************************************/
#ifndef SFLOW_CTRL_H
#define SFLOW_CTRL_H
#include "sysnet_api.h"

void    sFlowTask(void);
/* Call back functions */
L7_RC_t sFlowIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,
                                NIM_CORRELATOR_t correlator);
void sFlowNimStartupNotifyCallback(NIM_STARTUP_PHASE_t startup_phase);
L7_RC_t sFlowPduReceive(L7_netBufHandle netBufHandle,
                        sysnet_pdu_info_t *pduInfo);

/* Sample Processing/Generation functions */
void    sFlowSampleProcess(sflowPDU_Msg_t *pduMsg);
void    sFlowCounterProcess(SFLOW_poller_t *pPoller);
L7_RC_t sFlowReceiverSampleAdd(SFLOW_receiver_t *pReceiver,
                               SFLOW_sample_data_t *pSample);
L7_RC_t sFlowReceiverDatagramSend(SFLOW_receiver_t *pReceiver);

/* Operational configuration apply APIs */
L7_RC_t sFlowSamplerOperModeSet(SFLOW_sampler_t *pSampler, L7_uint32 mode);
L7_RC_t sFlowPollerOperModeSet(SFLOW_poller_t *pPoller, L7_uint32 mode);

/* Configuration Apply APIs */
L7_RC_t sFlowPollerIntervalApply(L7_uint32 dsIndex, L7_uint32 instance,
                                 L7_uint32 pollInterval);
L7_RC_t sFlowSamplerRateApply(L7_uint32 dsIndex, L7_uint32 instance,
                              L7_uint32 samplingRate);
L7_RC_t sFlowSamplerReceiverApply(L7_uint32 dsIndex, L7_uint32 instance,
                                  L7_uint32 rcvrIndex);
L7_RC_t sFlowPollerReceiverApply(L7_uint32 dsIndex, L7_uint32 instance,
                                 L7_uint32 rcvrIndex);
L7_RC_t sFlowGlobalCfgApply(void);

/* Timer APIs */
void    sFlowPollerTimerExpiry(void *param);
L7_RC_t sFlowPollerTimerStop(SFLOW_poller_t *pPoller);
L7_RC_t sFlowPollerTimerStart(SFLOW_poller_t *pPoller, L7_uint32 interval);
void    sflowTimerExpiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* ptrData);

/* Misc */
L7_BOOL sFlowIsReceiverReady(SFLOW_receiver_t *pReceiver);
void    sFlowSamplerPollerInstancesCleanup(void);
#endif /* SFLOW_CTRL_H */
