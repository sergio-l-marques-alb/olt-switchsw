/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping_ctrl.h
*
* @purpose    
*
* @component  Snooping
*
* @comments   none
*
* @create     07-Dec-2006
*
* @author     drajendra
*
* @end
*
**********************************************************************/
#ifndef SNOOPING_CTRL_H
#define SNOOPING_CTRL_H

#ifdef PTIN_ENABLE_ERPS
#include "ptin_prot_erps.h"
#endif

struct snoop_cb_s;

void snoopTask(void);
L7_RC_t snoopAdminModeApply(L7_uint32 adminMode, L7_uint16 vlan_id /* PTin added: IGMP snooping */, struct snoop_cb_s *pSnoopCB);
L7_RC_t snoopIntfVlanModeApply(L7_uint32 intIfNum, L7_uint32 vlanId,
                               L7_uint32 mode, snoop_cb_t *pSnoopCB);
L7_RC_t snoopApplyIntfConfigData(L7_uint32 intIfNum, struct snoop_cb_s *pSnoopCB);
L7_RC_t snoopIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, 
                                NIM_CORRELATOR_t correlator);
void    snoopIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase);
L7_RC_t snoopNimCreateStartup(void);
L7_RC_t snoopNimActivateStartup(void);
L7_RC_t snoopDot1sIntfChangeCallback(L7_uint32 mstID, L7_uint32 intIfNum, 
                                     L7_uint32 event);
L7_RC_t snoopVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, 
                                L7_uint32 event);
L7_RC_t snoopNetworkAddrChanged(L7_COMPONENT_IDS_t registrar_ID, L7_uint32 addrType,
                                L7_uchar8 addrFamily);
L7_RC_t snoopL3McastAddNotifyProcess(L7_inet_addr_t *mcastGroupAddr,
                                     L7_inet_addr_t *mcastSrcAddr,
                                     L7_int32        srcVlan,
                                     L7_VLAN_MASK_t *vlanMask);
void snoopTimerExpiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* ptrData);
L7_RC_t snoopMcastEventCallback(L7_uint32 event, void *pData,
                                ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);
L7_RC_t snoopRoutingEventCallback(L7_uint32 intIfNum, L7_uint32 event,
                                  void *pData,
                                  ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);
#endif /* SNOOPING_CTRL_H */
