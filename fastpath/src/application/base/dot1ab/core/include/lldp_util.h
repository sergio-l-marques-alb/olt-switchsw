/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename lldp_util.h
*
* @purpose 802.1AB utility function prototypes
*
* @component 802.1AB
*
* @comments none
*
* @create 02/01/2005
*
* @author dfowler
* @end
*
**********************************************************************/

/*********************************************************************
 *
 ********************************************************************/
#ifndef LLDP_UTIL_H
#define LLDP_UTIL_H

#include "nimapi.h"
#include "sysnet_api.h"
#include "lldp_tlv.h"

/* handle interface events */
L7_RC_t lldpIntfChangeCallback(L7_uint32 intIfNum,
                               L7_uint32 event,
                               NIM_CORRELATOR_t correlator);
void    lldpStartupCallback(NIM_STARTUP_PHASE_t startupPhase);
L7_RC_t lldpNimCreateStartup(void);
L7_RC_t lldpNimActivateStartup(void);
L7_RC_t lldpIntfChangeProcess(L7_uint32 intIfNum,
                              L7_uint32 event,
                              NIM_CORRELATOR_t correlator);

/* handle timer events */
void    lldpTimerProcess();
void    lldpTimerCallback();

/* handle sysnet events */
L7_RC_t lldpPduReceiveCallback(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);
L7_RC_t lldpPduReceiveProcess(L7_uint32 intIfNum, L7_netBufHandle pduHandle);

/* handle configuration mapping */
L7_BOOL lldpMapIntfIndexGet(L7_uint32 intIfNum, L7_uint32 *index);
L7_BOOL lldpMapIntfCfgEntryGet(L7_uint32 intIfNum, lldpIntfCfgData_t **pCfg);
lldpRemDataEntry_t *lldpIsRemEntryValid(L7_uint32 intIfNum, L7_uint32 index, L7_uint32 timestamp);

/* apply configuration per interface */
L7_RC_t lldpIntfConfigDataApply(L7_uint32 index);
L7_RC_t lldpIntfTxModeApply(L7_uint32 index);
L7_RC_t lldpIntfRxModeApply(L7_uint32 index);

L7_RC_t lldpIntfModeApplyPostMsg(L7_uint32 intIfNum,
                                 L7_BOOL   rxApply,
                                 L7_BOOL   txApply);
L7_RC_t lldpRemoteDBClearPostMsg(void);


/* debug */
L7_RC_t lldpCfgDump(void);
L7_RC_t lldpMedTLVValidate(lldpRemDataEntry_t *remEntry,
               L7_uint32 index,
               lldpPDUHandle_t     *pduHdl,
                           L7_uint32            length);
L7_RC_t lldpOrgDefTLVGet(lldpRemDataEntry_t *remEntry,
             L7_uchar8 *oui,
             L7_uint32 subtype,
             L7_uchar8 *info,
             L7_uint32 infoLength);
L7_RC_t lldpOrgDefMedXTLVGet(lldpRemDataEntry_t *remEntry,
               L7_uchar8 *oui,
               L7_uint32 subtype,
               L7_uint32 appType,
               L7_uchar8 *info,
               L7_uint32 infoLength);
L7_RC_t lldpMedProcessVoiceDevicePdu(lldpRemDataEntry_t *remEntry,
                          L7_BOOL voiceDevice,
                          L7_uchar8 deviceType);
L7_RC_t lldpMedProcessPoeDevicePdu(lldpRemDataEntry_t *remEntry,
                          L7_BOOL poeDevice,
                          L7_uchar8 deviceType);
L7_RC_t lldpMedGetRemEntryVoiceInfo(lldpRemDataEntry_t *remEntry,
                                    L7_BOOL *voiceDevice,
                                    L7_uchar8 *deviceType);
L7_RC_t lldpMedGetRemEntryPoeInfo(lldpRemDataEntry_t *remEntry,
                                    L7_BOOL *voiceDevice,
                                    L7_uchar8 *deviceType);
void lldpMedRemDeviceRemoveCheck(lldpRemDataEntry_t *remEntry);

#endif /* LLDP_UTIL_H */
