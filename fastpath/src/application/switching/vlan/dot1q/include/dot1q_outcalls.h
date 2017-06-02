/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename dot1q_outcalls.h
*
* @purpose  Function definitions about outcalls
*
* @component dot1Q
*
* @comments none
*
* @create 08/15/2005
*
* @author akulkarni
* @end
*
**********************************************************************/

#ifndef _DOT1Q_OUTCALLS_H_
#define _DOT1Q_OUTCALLS_H_

L7_RC_t dot1qSysInitOutcalls();
L7_RC_t dot1qVlanDeleteOutcalls(L7_uint32 vlanID, L7_uint32 requestor);
L7_RC_t dot1qVlanTransientUpdate(L7_uint32 vlanID,
                                 dot1q_vlan_dataEntry_t *pCurrentDataEntry,
                                 dot1q_vlan_dataEntry_t *pDataEntry);
L7_RC_t vlanRegisterForChange(L7_RC_t (*notify)(dot1qNotifyData_t *vlanData, 
                                                L7_uint32 intIfNum,
                                                L7_uint32 event),
                             L7_COMPONENT_IDS_t registrarID, L7_uint32 eventMask);
L7_RC_t vlanDeregisterForChange( L7_COMPONENT_IDS_t registrarID);
void vlanNotifyRegisteredUsers(dot1qNotifyData_t *vlanData,
                               L7_uint32 intIfNum,
                               L7_uint32 event);
L7_RC_t dot1qVlanIntfValidate(L7_uint32 intIfNum);
void dot1qPortDefaultPriorityUpdateNotification(L7_uint32 intIfNum, 
                                                L7_uint32 portDefaultPriority,
                                                L7_uint32 portDefaultTrafficClass);
void dot1qPortNumTrafficClassesUpdateNotification(L7_uint32 intIfNum, 
                                                  L7_uint32 numTrafficClasses);
void dot1qPortTrafficClassUpdateNotification(L7_uint32 intIfNum, 
                                             L7_uint32 priority,
                                             L7_uint32 trafficClass);
L7_BOOL dot1qDot1pMappingIsActive(L7_uint32 intIfNum);


#endif  /* #ifndef _DOT1Q_OUTCALLS_H_  */
