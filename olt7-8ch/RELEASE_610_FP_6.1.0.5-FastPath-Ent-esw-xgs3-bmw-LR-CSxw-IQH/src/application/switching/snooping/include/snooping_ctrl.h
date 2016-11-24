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

struct snoop_cb_s;

void snoopTask(void);

// PTin added
L7_RC_t snoopVlanAdminModeApply(L7_uint32 adminMode, L7_uint32 intf, L7_uint16 vlan_id, L7_uint8 prio);

L7_RC_t snoopAdminModeApply(L7_uint32 adminMode, L7_uint32 intf, L7_uint16 vlan_id, L7_uint8 prio, struct snoop_cb_s *pSnoopCB);
L7_RC_t snoopIntfVlanModeApply(L7_uint32 intIfNum, L7_uint32 vlanId,
                               L7_uint32 mode, snoop_cb_t *pSnoopCB);
// PTin added
L7_RC_t snoopIntfVlanStaticEntriesAdd(L7_uint32 intIfNum, L7_uint32 vlanId, snoop_cb_t *pSnoopCB);

L7_RC_t snoopApplyIntfConfigData(L7_uint32 intIfNum, struct snoop_cb_s *pSnoopCB);
L7_RC_t snoopIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, 
                                NIM_CORRELATOR_t correlator);
L7_RC_t snoopDot1sIntfChangeCallback(L7_uint32 mstID, L7_uint32 intIfNum, 
                                     L7_uint32 event);
L7_RC_t snoopVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, 
                                L7_uint32 event);

L7_int32 snoopQueueGetNumMsgs( void );

L7_RC_t snoopNetworkAddrChanged(L7_COMPONENT_IDS_t registrar_ID, L7_uint32 addrType,
                                L7_uchar8 addrFamily);
void snoopTimerExpiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* ptrData);
#endif /* SNOOPING_CTRL_H */
