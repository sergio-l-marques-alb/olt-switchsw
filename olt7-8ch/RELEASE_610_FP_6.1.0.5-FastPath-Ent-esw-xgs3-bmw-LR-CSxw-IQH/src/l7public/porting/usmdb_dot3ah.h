/****************************************************************************
*
* (C) Copyright Broadcom Corporation 2008-2009
*
*****************************************************************************
*
* @filename usmdb_dot3ah.h
*
* @purpose Provide interface to USMDB API's for dot1ad
*
* @component unitmgr
*
* @comments tba
*
* @create 05/14/2008
*
* @author Sengottuvelan Srirangan
* @end
*
****************************************************************************/

#ifndef USMDB_DOT3AH_H
#define USMDB_DOT3AH_H

#include "l7_common.h"

#include "usmdb_common.h"
#include "l7_dot3ah_api.h"

L7_uint8 *usmDbDot3ahModeToStr(L7_uint32 num);

L7_RC_t usmDbDot3ahRemEntityModeGet(L7_uint32 unit,L7_uint32 intIfNum, L7_uint32 *modeType);

L7_RC_t usmDbDot3ahOrgSpecInfoTlvSupportSet(L7_uint32 interface, L7_BOOL enable);

L7_RC_t usmDbDot3ahOrgSpecEventPduSupportGet( L7_uint32 intIfNum, L7_BOOL *enable);
L7_RC_t usmDbDot3ahTransmitPdu( L7_uint32 intIfNum, L7_uint8 pdu);
L7_RC_t usmDbDot3ahRemEntryGetNext(L7_uint32 intfNum, L7_dot3ah_node_discvry_t *nodeEntry);

L7_BOOL usmDbDot3ahIntfIsValid(L7_uint32 intIfNum);

L7_BOOL usmDbDot3ahIntfIsEnabled(L7_uint32 intIfNum);

L7_RC_t usmDbDot3ahInterfaceEnableGet(L7_uint32 unit,L7_uint32 intIfNum, L7_BOOL *enable);

L7_RC_t usmDbDot3ahRemLbIntfIsEnabled(L7_uint32 intIfNum, L7_BOOL *stat);

L7_RC_t usmDbDot3ahRemEntityVsiget(L7_uint32 intIfNum, L7_uint8 *remoteVsi);
L7_RC_t usmDbDot3ahLocalEntityStateget(L7_uint32 intIfNum, L7_BOOL *state);

L7_RC_t usmDbDot3ahRemEntityStateget(L7_uint32 intIfNum, L7_BOOL *state);

L7_RC_t usmDbDot3ahLinkStatusGet(L7_uint32 intIfNum, L7_uint8 *linkStatus);

L7_uint8 *usmDbDot3ahLinkStatusToStr(L7_uint8 mode);
L7_RC_t usmDbDot3ahLocalOamConfigGet(L7_uint32 intIfNum, L7_uint8 *oamConfig);

L7_RC_t usmDbdot3ahLocalOamMaxPduSizeGet(L7_uint32 intIfNum, L7_ushort16 *pduSize);


L7_RC_t usmDbDot3ahLinkModeGet(L7_uint32 intIfNum, L7_uint32 *mode);

L7_RC_t usmDbDot3ahLinkModeSet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 mode);

L7_RC_t usmDbDot3ahPduTimerIntervalGet (L7_uint32 intfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahlinkTimerIntervalGet(L7_uint32 intfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahRemLbExpiryTimerIntervalGet(L7_uint32 intfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahLocEntityMuxParGet(L7_uint32 intIfNum,  L7_uint8 *local_state);

L7_RC_t usmDbDot3ahRemEntityMuxParGet(L7_uint32 intIfNum, L7_uint8 *rem_state);

L7_RC_t usmDbDot3ahLocLbStateGet(L7_uint32 intIfNum, L7_uint8 *localLbState);

L7_RC_t usmDbDot3ahLocalOuiGet(L7_uint32 intIfNum, L7_uint8 *oui);

L7_RC_t usmDbDot3ahLocalMacAddrGet(L7_uint32 intIfNum, L7_uint8 *macAddr);

L7_RC_t usmDbDot3ahLocalVsiGet(L7_uint32 intIfNum, L7_uint8 *vsi);

L7_RC_t usmDbDot3ahRemOuiGet(L7_uint32 intIfNum, L7_uint8 *oui);

L7_RC_t usmDbDot3ahRemMacGet(L7_uint32 intIfNum, L7_uint8 *remMac);

L7_RC_t usmDbDot3ahRemLbStateGet(L7_uint32 intIfNum, L7_uint8 *remLbState);

L7_uint8 * usmdbDot3ah_oampdu_to_str(L7_uint32 num);

L7_RC_t usmDbDot3ahPortStatsPdusTxGet (L7_uint32 intfNum, L7_uint32 num, L7_uint32 *stat);

L7_RC_t usmDbDot3ahPortStatsPdusRxGet (L7_uint32 intfNum, L7_uint32 num, L7_uint32 *stat);

L7_RC_t usmDbDot3ahNumOfDiscoveredNodes(L7_uint32 intfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahNumofDiscoveredNodesRemoed(L7_uint32 intfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahNumofdiscoveryFail(L7_uint32 intfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahNumofDiscNodesAdded(L7_uint32 intfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahPortStatsIllegalRxGet(L7_uint32 intIfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahPortStatsIllegalTxGet( L7_uint32 intIfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahPortStatsFrameErrorTxGet(L7_uint32 intIfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahPortStatsEventErrorTxGet(L7_uint32 intIfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahPortStatsBadSrcMacRxGet(L7_uint32 intIfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahPortStatsbadDstMacRxGet(L7_uint32 intIfNum,  L7_uint32 *stat);

L7_RC_t usmDbDot3ahPortStatsMsgInterceptRxGet(L7_uint32 intIfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahPortStatsOAMPDUsRxGet(L7_uint32 intIfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahPortStatsOAMPDUsTxGet( L7_uint32 intIfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahPortStatsOAMPDUsFilterGet(L7_uint32 intIfNum, L7_uint32 *stat);
L7_RC_t usmDbDdot3ahLocRevNumGet(L7_uint32 intIfNum, L7_uint32 *stat);
L7_RC_t usmDbDdot3ahRemRevNumGet(L7_uint32 intIfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahIntfModeGet(L7_uint32 intIfNum, L7_uint32 *mode);
L7_RC_t usmDbDot3ahIntfModeSet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint8 mode);
L7_RC_t usmDbDot3ahDisable(L7_uint32 unit, L7_uint32 intIfNum);
L7_RC_t usmDbDot3ahEnable(L7_uint32 unit, L7_uint32 intIfNum);
L7_RC_t usmDbDot3ahLinkLostTimeSet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 linkLostTime);
L7_RC_t usmDbDot3ahMaxPduRateSet( L7_uint32 unit, L7_uint32 interface, L7_uint32 pduRate);
L7_RC_t usmDbDot3ahMaxPduRateGet( L7_uint32 unit, L7_uint32 interface, L7_uint32 *pduRate);
L7_RC_t usmDbDot3ahMinPduRateGet( L7_uint32 unit, L7_uint32 interface, L7_uint32 *pduRate);
L7_RC_t usmDbDot3ahRemoteLoopbackSupportSet(L7_uint32 interface, L7_BOOL enable);
L7_RC_t usmDbDot3ahRemLbExpiryTimerIntervalSet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 timeOut);
L7_RC_t usmDbDot3ahRemoteLoopbackStart(L7_uint32 intIfNum, DOT3AH_REMOTE_LOOPBACK_ERR_CODE_t *errCode);
L7_RC_t usmDbDot3ahRemoteLoopbackStop(L7_uint32 intIfNum);
L7_RC_t usmDbDot3ahLinkMonitorSupportGet( L7_uint32 intIfNum, L7_BOOL *enable);
L7_RC_t usmDbDot3ahLinkMonitorSupportSet( L7_uint32 intIfNum, L7_BOOL enable);
L7_RC_t usmDbDot3ahLinkMonitorOnorOff(L7_uint32 intIfNum, L7_BOOL enable, L7_uint32 unit);
L7_RC_t usmDbDot3ahLinkMonitorOnorOffGet(L7_uint32 unit, L7_uint32 intIfNum, L7_BOOL *enable);
L7_RC_t usmDbDot3ahLinkMonitorFrameParamSet(L7_uint32 intIfNum, L7_uint32 frameHigh,
                                           L7_uint32 frameLow, L7_uint32 frameWindow);
L7_RC_t usmDbDot3ahLinkMonitorFramePeriodParamSet(L7_uint32 intIfNum,
                                                 L7_uint32 framePeriodHigh,
                                                 L7_uint32 framePeriodLow, 
                                                 L7_uint32 framePeriodWindow);
L7_RC_t usmDbDot3ahLinkMonitorFrameSumSecParamSet(L7_uint32 intIfNum,
                                                 L7_uint32 frameSumSecHigh,
                                                 L7_uint32 frameSumSecLow,
                                                 L7_uint32 frameSumSecWindow);
L7_RC_t usmDbDot3ahLinkMonitorFrameParamGet(L7_uint32 intIfNum, L7_uint32 *high,
                                           L7_uint32 *low, L7_uint32 *window);
L7_RC_t usmDbDot3ahLinkMonitorFramePeriodParamGet(L7_uint32 intIfNum, L7_uint32 *high,
                                           L7_uint32 *low, L7_uint32 *window);

L7_RC_t usmDbDot3ahLinkMonitorFrameSumSecParamGet (L7_uint32 intIfNum, L7_uint32 *high,
                                                  L7_uint32 *low, L7_uint32 *window);

L7_RC_t usmDbDot3ahProtocolInfoPerIntfGet(L7_uint32 unit, L7_uint32 intf, L7_dot3ahIntfCfg_t *dot3ahIntfCfg);
L7_RC_t usmDbDot3ahInterfaceModeTypeGet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 *modeType);
L7_RC_t usmDbDot3ahRemoteLoopbackSupportGet(L7_uint32 interface, L7_BOOL *enable, L7_uint8 *oamState);

L7_RC_t  usmDbLanPortTest(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 modeType);
L7_RC_t usmDbLanPortTestEvent(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 eventNum);
L7_RC_t usmDbPortTestEnDisSet(L7_uint32 intIfNum, L7_BOOL enDis);
L7_RC_t usmDbDot3ahIntfCapsGet(L7_uint32 intIfNum,
                               L7_BOOL   *remLb,
                               L7_BOOL   *linkEvent,L7_BOOL   *orgInfoTlv,
                               L7_BOOL   *orgSpecEvent,
                               L7_BOOL   *orgSpecPdus);


L7_RC_t usmDbDot3ahIntfCapsSet(L7_uint32 intIfNum,
                               L7_BOOL   remLb,
                               L7_BOOL   linkEvent,
                               L7_BOOL   orgInfoTlv,
                               L7_BOOL   orgSpecEvent,
                               L7_BOOL   orgSpecPdus);

L7_RC_t usmDbDot3ahEnToDisSet(L7_uint32 unit, L7_uint32 intIfNum, L7_BOOL EnDis);
L7_RC_t usmDbLanOamIntfIsEnabled(L7_uint32 intIfNum, L7_BOOL *enable);
L7_RC_t  usmDbLanPortTestGet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 *modeType);
L7_uint8  *usmDbLanPortTestTypeToStr(L7_uint32 modeType);
L7_RC_t usmDbPortTestIntfStart(L7_uint32 intIfNum);
L7_uint8 *usmDbDot3ahDiscoveryStateToStr(L7_uint8 currStat);

L7_RC_t usmDbDot3ahDiscoveryStateGet(L7_uint32 intIfNum, L7_uint8 *currStat);

L7_RC_t usmDbDot3ahValidIntfFirstGet(L7_uint32 *intIfNum);

L7_RC_t usmDbDot3ahValidIntfNextGet(L7_uint32 prevIfNum, L7_uint32 *intIfNum);

L7_RC_t usmDbDot3ahStatsLastFaultTime(L7_uint32 intIfNum);

L7_RC_t usmDbDot3ahStatsLastFaultTimeGet(L7_uint32 intIfNum,  usmDbTimeSpec_t *val);

L7_RC_t usmDbDot3ahStatsDyingGaspTimeGet(L7_uint32 intIfNum,  usmDbTimeSpec_t *val);
L7_RC_t usmDbDot3ahStatsDyingGaspTime(L7_uint32 intIfNum);
L7_RC_t usmDbDot3ahStatsTotalLinkFaultTimeGet(L7_uint32 intfNum, L7_uint32 *stat);
L7_RC_t usmDbDot3ahStatsTotalDyingGaspTimeGet(L7_uint32 intfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahMinPduRateSet( L7_uint32 unit, L7_uint32 interface, L7_uint32 pduRate);
L7_RC_t usmDbDot3ahRemOamConfigGet(L7_uint32 intIfNum, L7_uint8 *oamConfig);
void usmDbDot3ahPacketDebugTraceFlagGet(L7_BOOL *transmitFlag,L7_BOOL *receiveFlag);
L7_RC_t usmDbDot3ahPacketDebugTraceFlagSet(L7_BOOL transmitFlag,L7_BOOL receiveFlag);
L7_RC_t usmDbDot3ahLocCriticalEventFlagsGet(L7_uint32 infNum, L7_uchar8 *flags);
L7_RC_t usmDbDot3ahLocErrorFrameEventTsGet(L7_uint32 infNum, L7_uint32 *timeStamp);
L7_RC_t usmDbDot3ahLocErrorFramePeriodEventTsGet(L7_uint32 infNum, L7_uint32 *timeStamp);
L7_RC_t usmDbDot3ahLocErrorFrameSumEventTsGet(L7_uint32 infNum, L7_uint32 *timeStamp);
L7_RC_t usmDbDot3ahRemErrorFrameEventTsGet(L7_uint32 infNum, L7_uint32 *timeStamp);
L7_RC_t usmDbDot3ahRemErrorFramePeriodEventTsGet(L7_uint32 infNum, L7_uint32 *timeStamp);
L7_RC_t usmDbDot3ahRemErrorFrameSumEventTsGet(L7_uint32 infNum, L7_uint32 *timeStamp);
L7_RC_t usmDbDot3ahLocErrorFrameEventDetailGet(L7_uint32 infNum, L7_ushort16 *window,
                                       L7_uint32 *err_frames,
                                       L7_ulong64 *err_frame_total,
                                       L7_uint32 *event_total);
L7_RC_t usmDbDot3ahLocErrorFramePeriodEventDetailGet(L7_uint32 infNum, L7_uint32 *window,
                                                     L7_uint32 *err_frames,
                                                     L7_ulong64 *err_frame_total,
                                                     L7_uint32 *event_total);
L7_RC_t usmDbDot3ahLocErrorFrameSumSecEventDetailGet(L7_uint32 infNum, L7_ushort16 *window,
                                             L7_ushort16 *err_frames,
                                             L7_uint32 *err_frame_total,
                                             L7_uint32 *event_total);
L7_RC_t usmDbDot3ahRemErrorFrameEventDetailGet(L7_uint32 infNum, L7_ushort16 *window,
                                       L7_uint32 *err_frames,
                                       L7_ulong64 *err_frame_total,
                                       L7_uint32 *event_total);
L7_RC_t usmDbDot3ahRemErrorFramePeriodEventDetailGet(L7_uint32 infNum, L7_uint32 *window,
                                       L7_uint32 *err_frames,
                                       L7_ulong64 *err_frame_total,
                                       L7_uint32 *event_total);
L7_RC_t usmDbDot3ahRemErrorFrameSumSecEventDetailGet(L7_uint32 infNum, L7_ushort16 *window,
                                             L7_ushort16 *err_frames,
                                             L7_uint32 *err_frame_total,
                                             L7_uint32 *event_total);
L7_RC_t usmDbDot3ahRemErrorFramePeriodEventDetailGet(L7_uint32 infNum, L7_uint32 *window,
                                       L7_uint32 *err_frames,
                                       L7_ulong64 *err_frame_total,
                                       L7_uint32 *event_total);
L7_uint32 usmDbDot3ahRemLbTestResultGet(L7_uint32 infNum);
L7_RC_t usmDbDot3ahClearStatisticsDetail(L7_uint32 infNum);

L7_RC_t usmDbDot3ahPortStatsDataRxGet (L7_uint32 intfNum, L7_uint32 *stat);

L7_RC_t usmDbDot3ahPortStatsDataTxGet (L7_uint32 intfNum, L7_uint32 *stat);

#endif
