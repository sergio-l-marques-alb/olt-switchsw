/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename dot1s_control.c
*
* @purpose   Vlan Control file
*
* @component dot1q
*
* @comments 
*
* @create    06/22/2005
*
* @author    akulkarni
*
* @end
*             
**********************************************************************/
#ifndef __DOT1Q_CONTROL_H__
#define __DOT1Q_CONTROL_H__

#include "l7_common.h"
#include "osapi.h" 
#include "osapi_support.h"
#include "sysapi.h"
#include "nimapi.h"
#include "simapi.h"
#include "dtlapi.h"
#include "trace_api.h"
#include "sysnet_api.h"
#include "dot1q_api.h"
#include "fdb_api.h"
#include "log.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "filter_api.h"
#include "datatypes.h"
#include "trapapi.h"
#include "dot1q.h"

typedef enum dot1q_events_e
{
    dot1q_first_Event = 0,   /*keep this event first */
	dot1q_vlan_create_static,
    dot1q_vlan_delete_static,
    dot1q_vlan_create_dynamic,
    dot1q_vlan_delete_dynamic,
    dot1q_vlan_tagged_set_static,
    dot1q_vlan_tagged_set_static_mask,
    dot1q_vlan_tagged_set_delta_mask,
    dot1q_vlan_member_set,
    dot1q_vlan_member_set_static_mask,
    dot1q_vlan_member_set_delta_mask,
#if DOT1Q_FUTURE_FUNC_GROUP_FILTER
    dot1q_vlan_group_filter_static,
#endif
    dot1q_vlan_tagged_set_dynamic,
#if DOT1Q_FUTURE_FUNC_GROUP_FILTER
    dot1q_vlan_group_filter_dynamic,
#endif
    dot1q_vlan_dynamic_to_static_set,
    dot1q_qports_vid_set,
    dot1q_qports_accept_frame_type_set,
    dot1q_qports_default_user_prio_set,
    dot1q_qports_default_user_prio_global_set,
    dot1q_qports_ingress_filter_set,
    dot1q_qports_traffic_classes_enabled_set,
    dot1q_qports_traffic_class_set,
    dot1q_qports_num_traffic_classes_set,
    dot1q_qports_num_traffic_classes_global_set, 
    dot1q_qports_traffic_class_global_set,
    dot1q_clear,
    dot1q_cnfgr,
    dot1q_intfChange,
    dot1q_queueSynchronize,
    dot1q_qports_vlan_membership_delta_mask_set,
    dot1q_qports_vlan_tagging_delta_mask_set,
    dot1q_switchport_mode_set,
    dot1q_switchport_access_vlan_set,
    dot1q_vlan_create_static_mask,
    dot1q_vlan_delete_static_mask,
	dot1q_last_event   /* Keep this event last */

} DOT1Q_EVENTS_t ;


#define DOT1Q_MAX_EVENTS dot1q_last_event

typedef struct dot1q_msg_prio_s 
{
   L7_uint32         intIfNum;
   L7_uint32         priority;
   L7_uint32         trafficClass;
}dot1q_msg_prio_t;

typedef struct dot1q_msg_intf_vlan_mask_s
{
  L7_uint32 intIfNum;
  L7_VLAN_MASK_t vlanMask;

}dot1q_msg_intf_vlan_mask_t;

typedef struct DOT1Q_MSG_s 
{
   L7_uint32            vlanId;
   L7_uint32            mode; /* data for the event (untagged/tagged or fixed/aut/forbidden)*/
   DOT1Q_SWPORT_MODE_t  swport_mode; /* Access | Trunk | General|None  mode through which the cmd is issued */
   DOT1Q_EVENTS_t    event;
   union 
   {
     L7_uint32         intIfNum;
     L7_uchar8         name[L7_MAX_VLAN_NAME];
     NIM_INTF_MASK_t   intfMask;
     L7_CNFGR_CMD_DATA_t CmdData;
     NIM_EVENT_COMPLETE_INFO_t status;
     dot1q_msg_prio_t  prio;  /* NOTE: When using prio struct, use its intIfNum field*/ 
                              /*       instead of the one in the 'data' union        */
	 dot1q_msg_intf_vlan_mask_t intfVlanMask;  /* NOTE: When using this struct, use its intIfNum field*/ 
											   /*       instead of the one in the 'data' union        */

   } data;
   vlanRequestor_t    requestor;
   /* vlan data*/
}DOT1Q_MSG_t;

extern void * dot1qSem;

void dot1qTask(void);
L7_RC_t dot1qIssueCmd(DOT1Q_MSG_t *msg);
L7_RC_t dot1qDispatch(DOT1Q_MSG_t *msg);
L7_int32 dot1qQueueGetNumMsgs( void ); // PTin added

L7_RC_t dot1qVlanCreateProcess(L7_uint32 fdbId, L7_uint32 vlanId, L7_uint32 requestor);
L7_RC_t dot1qVlanDeleteProcess(L7_uint32 vlanId,L7_uint32 requestor);
L7_RC_t dot1qVlanMemberSetModify(L7_uint32 vlanId,L7_uint32 intIfNum, L7_uint32 mode, L7_uint32 requestor, DOT1Q_SWPORT_MODE_t swport_mode);
L7_RC_t dot1qVlanMSMaskModify(L7_uint32 vlanId, NIM_INTF_MASK_t *mask, L7_uint32 mode, DOT1Q_SWPORT_MODE_t swport_mode);
#if DOT1Q_FUTURE_FUNC_GROUP_FILTER
L7_RC_t dot1qVlanGroupFilterModify(L7_uint32 vlanId,L7_uint32 intIfNum, L7_uint32 mode, L7_uint32 requestor);
#endif
L7_RC_t dot1qIntfVlanListPopulate(L7_uint32 intIfNum, L7_VLAN_MASK_t *memberVlans, 
                                  L7_VLAN_MASK_t *taggedVlans);
L7_RC_t dot1qVlanTSMaskModify(L7_uint32 vlanId,NIM_INTF_MASK_t *mask, L7_uint32 tagMode );



L7_RC_t dot1qVlanTaggedMemberSetModify(L7_uint32 vlanId,L7_uint32 intIfNum, L7_uint32 mode);
L7_RC_t dot1qDynamicToStaticVlanConvertProcess(L7_uint32 vlanId);
L7_RC_t dot1qVlanCfgClearProcess(void);
L7_RC_t dot1qVlanTaggedSetMaskModify(L7_uint32 vlanId, NIM_INTF_MASK_t intfMask);
L7_RC_t dot1qVlanMemberSetMaskModify(L7_uint32 vlanId, NIM_INTF_MASK_t intfMask);
L7_RC_t dot1qVlanMemberSetMaskForbidden(L7_uint32 vlanId, NIM_INTF_MASK_t intfMask );
/*  Vlan BridgePort paramters manipulation routines */

/* dot1q Queue synchronise routines*/
void dot1qQueueSyncSemCreate (void);
void dot1qQueueSyncSemGet (void);
void dot1qQueueSyncSemFree (void);
void dot1qQueueSyncSemDelete (void);

void dot1qQportsVlanTagMaskProcess(L7_uint32 intIfNum, L7_VLAN_MASK_t *vlanMask, L7_uint32 tagMode);
void dot1qQportsVlanMembershipMaskProcess(L7_uint32 intIfNum, 
                                          L7_VLAN_MASK_t *vlanMask, 
                                          L7_uint32 tagMode, 
                                          DOT1Q_SWPORT_MODE_t swport_mode,
                                          L7_uint32 requestor);
void dot1qVlanCreateMaskProcess(L7_VLAN_MASK_t *vlanMask);
void dot1qVlanDeleteMaskProcess(L7_VLAN_MASK_t *vlanMask);

L7_RC_t dot1qSwitchPortModeProcess(NIM_INTF_MASK_t *mask, DOT1Q_SWPORT_MODE_t mode, L7_BOOL setupDefaults);
L7_RC_t dot1qSwitchPortAccessModeSet(L7_uint32 intIfNum, L7_uint32 vlanId, 
                                     L7_uint32 requestor);
L7_BOOL dot1qCanRequestorModifyIntf(L7_uint32 intIfNum, vlanRequestor_t requestor);

#endif 



