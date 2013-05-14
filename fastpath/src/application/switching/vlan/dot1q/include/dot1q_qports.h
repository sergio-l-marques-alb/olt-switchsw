/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename dot1q_qports.c
*
* @purpose port routines for dot1q
*
* @component dot1q
*
* @comments none
*
* @create 11/1/2005
*
* @author akulkarni
*
* @end
*             
**********************************************************************/
#ifndef __DOT1Q_QPORTS_H__
#define __DOT1Q_QPORTS_H__

L7_RC_t dot1dPortDefaultUserPriorityProcess(L7_uint32 intIfNum, L7_uint32 priority);
L7_RC_t dot1dPortDefaultUserPriorityGlobalProcess(L7_uint32 priority);
L7_RC_t dot1dPortNumTrafficClassesGlobalProcess(L7_uint32 numTrafficClasses);
L7_RC_t dot1dPortNumTrafficClassesProcess(L7_uint32 intIfNum, L7_uint32 numTrafficClasses);
L7_RC_t dot1dPortTrafficClassProcess(L7_uint32 intIfNum, L7_uint32 priority, L7_uint32 trafficClass);
L7_RC_t dot1dPortTrafficClassGlobalProcess(L7_uint32 priority, L7_uint32 trafficClass);
L7_RC_t dot1dTrafficClassesEnabledProcess(L7_BOOL status);
L7_RC_t dot1qQportsIngressFilterProcess(L7_uint32 intf, L7_uint32 status);
L7_RC_t dot1qQportsVIDProcess(L7_uint32 intf, L7_uint32 vid, L7_uint32 vid_type,L7_uint32 requestor);
L7_RC_t dot1qQportsAcceptFrameTypeProcess(L7_uint32 intf, L7_uint32 type);
L7_RC_t dot1qQportsVlanParamSet(L7_uint32 intIfNum,L7_uint32 vid,L7_uchar8 acceptFrameType, L7_uchar8 ingressFiltering);
DOT1Q_SWPORT_MODE_t dot1qOperSwitchPortModeGet(L7_uint32 intIfNum);
L7_uint32 dot1qOperSwitchPortAccessVlanGet(L7_uint32 intIfNum);
L7_BOOL dot1qOperSwitchPortVlanConfigAllowed(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 mode);

#endif /* __DOT1Q_QPORTS_H__ */
