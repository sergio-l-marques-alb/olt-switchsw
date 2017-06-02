/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dot3ah_api.h
*
* @purpose Contains dot1ad offerings
*
* @component DOT3AH
*
* @comments
*
* @create
*
* @author Sengottuvelan Srirangan
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_L7_DOT3AH_API_H
#define INCLUDE_L7_DOT3AH_API_H
#include "osapi.h"


/* Global declarations */
extern L7_dot3ahIntfCfg_t *dot3ahIntfCfg;

typedef enum
{
  DOT3AH_REMOTE_LOOPBACK_ERR_OAM_DISABLED,
  DOT3AH_REMOTE_LOOPBACK_ERR_OAM_PASSIVE,
  DOT3AH_REMOTE_LOOPBACK_ERR_REM_LB_UNSUPPORTED,
  DOT3AH_REMOTE_LOOPBACK_ERR_DISC_STATE_NOT_ANY,
  DOT3AH_REMOTE_LOOPBACK_ERR_ALREADY_INITIATED,
  DOT3AH_REMOTE_LOOPBACK_ERR_REM_LB_FAILED,
  DOT3AH_REMOTE_LOOPBACK_ERR_INVALID
}DOT3AH_REMOTE_LOOPBACK_ERR_CODE_t;


/*********************************************************************
* @purpose  Determine if the interface is valid for 802.3AH
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
void dot3ahLinkFaultTimestamp(void);
L7_RC_t dot3ahTransmitPdu(L7_uint32 intIfNum, L7_uint8 pdu);
L7_RC_t portTestIntfEnDisSet(L7_uint32 intIfNum, L7_BOOL enDis);
L7_RC_t dot3ahPduTimerIntervalGet (L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPduTimerIntervalSet (L7_uint32 intfNum, L7_uint32 efmPduTimer);
L7_RC_t dot3ahlinkTimerIntervalGet(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahlinkTimerIntervalSet(L7_uint32 intfNum, L7_uint32 efmLinkTimer);
L7_RC_t dot3ahRemLbExpiryTimerIntervalSet(L7_uint32 intfNum, L7_uint32 remLbExpirytime);
L7_RC_t dot3ahRemLbExpiryTimerIntervalGet(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahMaxPduRateSet(L7_uint32 intfNum, L7_uint32 PduRate);
L7_RC_t dot3ahMinPduRateSet(L7_uint32 intfNum, L7_uint32 PduRate);
L7_RC_t dot3ahMinPduRateGet(L7_uint32 intfNum, L7_uint32 *PduRate);
L7_RC_t dot3ahMaxPduRateGet(L7_uint32 intfNum, L7_uint32 *PduRate);
L7_RC_t dot3ahPortStatsPdusTxGet (L7_uint32 intfNum, L7_uint32 num, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsPdusRxGet (L7_uint32 intfNum, L7_uint32 num, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsOrgSpecPdusTxGet (L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsLBsTxGet (L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsEVENTPDUsTxGet (L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsINFOPDUsTxGet (L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsOrgSpecPdusRxGet (L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsLBsRxGet (L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsEVENTPDUsRxGet (L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsINFOPDUsRxGet (L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsIllegalRxGet(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsOAMPDUsRxGet(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsIllegalTxGet(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsFrameErrorTxGet(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsEventErrorTxGet(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsBadSrcMacRxGet(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsbadDstMacRxGet(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsMsgInterceptRxGet(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsOAMPDUsFilterGet(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahPortStatsOAMPDUsTxGet(L7_uint32 intfNum, L7_uint32 *stat);
L7_BOOL dot3ahIsLinkActiveNode(L7_uint32 intIfNum);
L7_RC_t dot3ahNumOfDiscoveredNodes(L7_uint32 intNum, L7_uint32 *stat);
L7_RC_t dot3ahNumofDiscoveredNodesRemoed(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahNumofDiscNodesAdded(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahNumofdiscoveryFail(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahRemEntryGetNext(L7_uint32 intfNum, L7_dot3ah_node_discvry_t **nodeEntry);
L7_RC_t dot3ahLinkModeGet(L7_uint32 intIfNum, L7_uint32 *mode);
L7_RC_t dot3ahLocalOuiGet(L7_uint32 intIfNum, L7_uint8 *oui);
L7_RC_t dot3ahLocalVsiGet(L7_uint32 intIfNum, L7_uint8 *vsi);
L7_RC_t dot3ahLocalMacAddrGet(L7_uint32 intIfNum, L7_uint8 *macAddr);
L7_RC_t dot3ahRemOuiGet(L7_uint32 intIfNum, L7_uint8 *oui);
L7_RC_t dot3ahRemMacGet(L7_uint32 intIfNum, L7_uint8 *mac);
L7_RC_t dot3ahLocalOamConfigGet(L7_uint32 intIfNum, L7_uint8 *oamConfig);
L7_RC_t dot3ahRemOamConfigGet(L7_uint32 intIfNum, L7_uint8 *oamConfig);
L7_RC_t dot3ahLocalOamMaxPduSizeGet(L7_uint32 intIfNum, L7_ushort16 *pduSize);
L7_RC_t dot3ahLinkStatusGet(L7_uint32 intIfNum, L7_uint8 *linkStatus);
L7_RC_t dot3ahRemEntityModeGet(L7_uint32 intIfNum, L7_uint32 *modeType);
L7_RC_t dot3ahLinkModeSet(L7_uint32 intIfNum, L7_uint32 mode);
L7_RC_t dot3ahLocalEntityStateget(L7_uint32 intIfNum, L7_BOOL *state);
L7_RC_t dot3ahRemEntityStateget(L7_uint32 intIfNum, L7_BOOL *state);
L7_RC_t dot3ahLocEntityMuxParGet(L7_uint32 intIfNum, L7_uint8 *state);
L7_RC_t dot3ahRemEntityMuxParGet(L7_uint32 intIfNum, L7_uint8 *state);
L7_RC_t dot3ahLocLbStateGet(L7_uint32 intIfNum, L7_uint8 *state);
L7_RC_t dot3ahRemLbStateGet(L7_uint32 intIfNum, L7_uint8 *state);
L7_RC_t dot3ahRemEntityVsiget(L7_uint32 intIfNum, L7_uint8 *remoteVsi);
L7_BOOL dot3ahIntfIsValid(L7_uint32 intIfNum);
L7_RC_t dot3ahValidIntfFirstGet(L7_uint32 *intIfNum);
L7_RC_t dot3ahValidIntfNextGet(L7_uint32 prevIfNum, L7_uint32 *intIfNum);
L7_BOOL
dot3ahIntfIsEnabled(L7_uint32 intIfNum, L7_BOOL *stat);
L7_RC_t
dot3ahIntfIsEnabledget(L7_uint32 intIfNum, L7_BOOL *enable);
L7_BOOL
lanOamIntfIsEnabled(L7_uint32 intIfNum, L7_BOOL *stat);
L7_RC_t portTestIntfStart(L7_uint32 intIfNum);
L7_RC_t
dot3ahRemLbIntfIsEnabled(L7_uint32 intIfNum, L7_BOOL *stat);
L7_RC_t dot3ahOrgSpecInfoTlvSupportSet (L7_uint32 intIfNum, L7_BOOL enable);
L7_RC_t dot3ahOrgSpecEventPduSupportGet(L7_uint32 intIfNum, L7_BOOL *enable);
L7_RC_t
dot3ahLinkMonitorSupportSet (L7_uint32 intIfNum, L7_BOOL enable);
L7_RC_t
dot3ahRemoteLoopbackSupportSet (L7_uint32 intIfNum, L7_BOOL enable);
L7_RC_t
dot3ahRemoteLoopbackSupportGet (L7_uint32 intIfNum, L7_BOOL *enable, L7_uint8 *state);
L7_RC_t
dot3ahRemoteLoopbackStart (L7_uint32 intIfNum, DOT3AH_REMOTE_LOOPBACK_ERR_CODE_t *errCode);
L7_RC_t
dot3ahRemoteLoopbackStop (L7_uint32 intIfNum);

L7_uint32 dot3ahRemLbTestResultGet(L7_uint32 intIfNum);

L7_RC_t dot3ahLocRevNumGet(L7_uint32 intfNum, L7_uint32 *stat);

L7_RC_t dot3ahRemRevNumGet(L7_uint32 intfNum, L7_uint32 *stat);

L7_RC_t
dot3ahLinkMonitorSupportGet (L7_uint32 intIfNum, L7_BOOL *enable);
L7_RC_t
dot3ahLinkMonitorFrameParamSet (L7_uint32 intIfNum, L7_uint32 high,
                             L7_uint32 low, L7_uint32 window);
L7_RC_t
dot3ahLinkMonitorFramePeriodParamSet (L7_uint32 intIfNum, L7_uint32 high,
                             L7_uint32 low, L7_uint32 window);

L7_RC_t
dot3ahLinkMonitorFrameSumSecParamSet (L7_uint32 intIfNum, L7_ushort16 high,
                             L7_ushort16 low, L7_ushort16 window);

L7_RC_t
dot3ahLinkMonitorFrameParamGet (L7_uint32 intIfNum, L7_uint32 *high,
                             L7_uint32 *low, L7_uint32 *window);
L7_RC_t
dot3ahLinkMonitorFramePeriodParamGet (L7_uint32 intIfNum, L7_uint32 *high,
                             L7_uint32 *low, L7_uint32 *window);

L7_RC_t
dot3ahLinkMonitorFrameSumSecParamGet (L7_uint32 intIfNum, L7_uint32 *high,
                             L7_uint32 *low, L7_uint32 *window);

L7_RC_t
dot3ahLinkMonitorOnorOff (L7_uint32 intIfNum,L7_BOOL enable, L7_uint32 unit);

L7_RC_t
dot3ahLinkMonitorOnorOffGet (L7_uint32 intIfNum,L7_BOOL *enable);

L7_RC_t
dot3ahEnableModeApply(L7_uint32 unit, L7_uint32 intIfNum, L7_BOOL enableMode);

L7_RC_t
lanOamIfLoopbackModeSet(L7_uint32 intIfNum, L7_uint32 loopbackMode);
L7_RC_t
dot3ahIfLoopbackModeGet(L7_uint32 intIfNum, L7_uint32 *loopbackmode);
L7_RC_t dot3ahProtocolInfoPerIntfGet(L7_uint32 intIfNum, L7_dot3ahIntfCfg_t *dot3ahIntfCfg);
L7_RC_t dot3ahPostEvent(L7_uint32 intIfNum, L7_uint32 eventNum, L7_int32 pdu);
L7_RC_t dot3ahIntfCapsGet(L7_uint32 intIfNum,
                               L7_BOOL   *remLb,
                               L7_BOOL   *linkEvent,L7_BOOL   *orgInfoTlv,
                               L7_BOOL   *orgSpecEvent,
                               L7_BOOL   *orgSpecPdus);

L7_RC_t dot3ahIntfCapsSet(L7_uint32 intIfNum,
                               L7_BOOL   remLb,
                               L7_BOOL   linkEvent,
                               L7_BOOL orgInfoTlv,
                               L7_BOOL   orgSpecEvent,
                               L7_BOOL   orgSpecPdus);

L7_RC_t dot3ahDiscoveryStateGet(L7_uint32 intIfNum, L7_uint8 *currStat);
L7_RC_t dot3ahLastFaultTimeUpdateGet(L7_uint32 intIfNum, L7_timespec *val);
L7_RC_t dot3ahLastFaultTimeUpdate(L7_uint32 intIfNum);
L7_RC_t dot3ahStatsTotalLinkFaultTimeGet (L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t dot3ahDyingGaspTimeUpdateGet(L7_uint32 intIfNum, L7_timespec *val);
L7_RC_t dot3ahDyingGaspTimeUpdate(L7_uint32 intIfNum);
L7_RC_t dot3ahStatsTotalDyingGaspTimeGet (L7_uint32 intfNum, L7_uint32 *stat);


L7_uint8 *dot3ahDiscoveryStateToStr (L7_uint8 discovery_state);
L7_uint8 *dot3ahModeToStr (L7_uint8 mode);
L7_uint8 *dot3ahLinkStatusToStr (L7_uint8 mode);
L7_uint8 *dot3ah_oampdu_to_str (L7_uint8 oampdu);
L7_RC_t lanOamIfLoopbackModeGet(L7_uint32 intIfNum, L7_uint32 *loopbackMode);
L7_uint8 *lanOamModeTypeToStr (L7_uint8 mode);
L7_RC_t dot3ahLocCriticalEventFlagsGet(L7_uint32 infNum, L7_uchar8 *flags);
L7_RC_t dot3ahLocErrorFrameEventTsGet(L7_uint32 infNum, L7_uint32 *timeStamp);
L7_RC_t dot3ahLocErrorFramePeriodEventTsGet(L7_uint32 infNum, L7_uint32 *timeStamp);
L7_RC_t dot3ahLocErrorFrameSumEventTsGet(L7_uint32 infNum, L7_uint32 *timeStamp);
L7_RC_t dot3ahRemErrorFrameEventTsGet(L7_uint32 infNum, L7_uint32 *timeStamp);
L7_RC_t dot3ahRemErrorFramePeriodEventTsGet(L7_uint32 infNum, L7_uint32 *timeStamp);
L7_RC_t dot3ahRemErrorFrameSumEventTsGet(L7_uint32 infNum, L7_uint32 *timeStamp);
L7_RC_t dot3ahLocErrorFrameEventDetailGet(L7_uint32 infNum, L7_ushort16 *window,
                                       L7_uint32 *err_frames,
                                       L7_ulong64 *err_frame_total,
                                       L7_uint32 *event_total);

L7_RC_t dot3ahLocErrorFramePeriodEventDetailGet(L7_uint32 infNum, L7_uint32 *window,
                                       L7_uint32 *err_frames,
                                       L7_ulong64 *err_frame_total,
                                       L7_uint32 *event_total);
L7_RC_t dot3ahLocErrorFrameSumSecEventDetailGet(L7_uint32 infNum, L7_ushort16 *window,
                                             L7_ushort16 *err_frames,
                                             L7_uint32 *err_frame_total,
                                             L7_uint32 *event_total);

L7_RC_t dot3ahRemErrorFrameEventDetailGet(L7_uint32 infNum, L7_ushort16 *window,
                                       L7_uint32 *err_frames,
                                       L7_ulong64 *err_frame_total,
                                       L7_uint32 *event_total);

L7_RC_t dot3ahRemErrorFramePeriodEventDetailGet(L7_uint32 infNum, L7_uint32 *window,
                                       L7_uint32 *err_frames,
                                       L7_ulong64 *err_frame_total,
                                       L7_uint32 *event_total);
L7_RC_t dot3ahRemErrorFrameSumSecEventDetailGet(L7_uint32 infNum, L7_ushort16 *window,
                                             L7_ushort16 *err_frames,
                                             L7_uint32 *err_frame_total,
                                             L7_uint32 *event_total);
L7_RC_t dot3ahClearStatisticsDetail(L7_uint32 infNum);
L7_RC_t dot3ahPortStatsDataRxGet (L7_uint32 intfNum,L7_uint32 *stat);
L7_RC_t dot3ahPortStatsDataTxGet (L7_uint32 intfNum,L7_uint32 *stat);

#endif /* INCLUDE_L7_DOT3AH_API_H */
