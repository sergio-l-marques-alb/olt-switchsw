/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1s_cnfgr.h
*
* @purpose   Multiple Spanning tree configurator header
*
* @component dot1s
*
* @comments 
*
* @create    8/5/2003
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1S_CNFGR_H
#define INCLUDE_DOT1S_CNFGR_H

#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "dot1s_cfg.h"
#include "dot1s_txrx.h"
#include "comm_mask.h"

typedef enum
{
  DOT1S_RECON_NA = 0, /* reconciliation state not applicable*/
  DOT1S_NSF_SO_BEGIN,/*switchover begin*/
  DOT1S_NSF_SO_IF_ACT, /* interfaces activated*/
  DOT1S_NSF_EXT_RECON, /* External reconciliation complete*/
  DOT1S_NSF_STATE_SYNC,/* intf state agrees with ckpted*/
  DOT1S_READY 
}dot1s_nsf_recon_states_t;

typedef struct
{
  L7_uint32 phase3Begin;
  L7_uint32 startupCBDone;
  L7_uint32 activateDone;
  L7_uint32 extRecon;
  L7_uint32 reconComplete;
}dot1s_startup_times_t;

typedef enum
{
  dot1s_recon_event_pdu_rx = 1,
  dot1s_recon_event_state_change_begin,
  dot1s_recon_event_state_change_finish,
  dot1s_recon_event_flush,
  dot1s_recon_event_update

} dot1s_recon_intf_events_t;

typedef struct dot1s_nsfFuncTable_s
{
  L7_BOOL (*dot1sIsIntfReconciled)(L7_uint32 intIfNum);
  void (*dot1sUpdateIntfReconciled)(L7_uint32 intIfNum);
  L7_RC_t (*dot1sNsfReconEventFunc)(L7_uint32 intIfNum, L7_uint32 instIndex,
                               dot1s_recon_intf_events_t reconEvent);
  L7_RC_t (*dot1sCallCheckpointService)(L7_INTF_MASK_t *intfMask, L7_uint32 instIndex);
  void (*dot1sCheckGlobalReconciled)(L7_BOOL coldRestart);
  L7_RC_t (*dot1sIntfReconcile)(DOT1S_PORT_COMMON_t *p);
  void (*dot1sNsfActivateStartupBegin)();
  void (*dot1sReconcileTimerExpiredProcess)();
  L7_RC_t (*dot1sCkptStateGet)(L7_uint32 intIfNum, L7_uint32 instIndex, 
                             L7_uint32 *state)

} dot1s_nsfFuncTable_t;

typedef struct dot1s_helperFuncTable_s
{
  L7_RC_t (*dot1sNotifyHelperPduStart)(DOT1S_BYTE_MSTP_ENCAPS_t *pdu, 
                                  L7_uint32 intIfNum, L7_uint32 length);
  L7_RC_t (*dot1sNotifyHelperPduModify)(L7_uint32 intIfNum,
                                   L7_uint32 interval);
  L7_RC_t (*dot1sNotifyHelperPduStop)(L7_uint32 intIfNum);

} dot1s_HelperFuncTable_t;

extern dot1s_nsf_recon_states_t dot1sAppState;
extern dot1s_HelperFuncTable_t dot1sHelperFuncTable;
extern dot1s_nsfFuncTable_t dot1sNsfFuncTable;
extern dot1s_startup_times_t dot1sStartupTime;

L7_RC_t dot1sCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t dot1sCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t dot1sCnfgrInitPhase3Process( L7_BOOL isWarmRestart,
                                     L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );
void    dot1sCnfgrFiniPhase1Process();
void    dot1sCnfgrFiniPhase2Process();
void    dot1sCnfgrFiniPhase3Process();
L7_RC_t dot1sCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t dot1sCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason );
void    dot1sCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t dot1sCfgDump();
L7_RC_t dot1sCfgInstanceDefaultPopulate(DOT1S_INSTANCE_INFO_CFG_t *instance, 
                                        L7_uint32 instIndex, 
                                        L7_uint32 instId);
L7_RC_t dot1sCfgInstanceMacAddrPopulate(DOT1S_INSTANCE_INFO_CFG_t *instance, 
                                        L7_uint32 instIndex);
void dot1sCnfgrInstApply();
DOT1S_PORT_COMMON_CFG_t *dot1sIntfCfgEntryGet(L7_uint32 intIfNum);
L7_BOOL dot1sIsRestartTypeWarm();
L7_BOOL dot1sCnfgrIsHelperPresent();
L7_BOOL dot1sCnfgrAppIsReady();


#endif /*INCLUDE_DOT1S_CNFGR_H*/
