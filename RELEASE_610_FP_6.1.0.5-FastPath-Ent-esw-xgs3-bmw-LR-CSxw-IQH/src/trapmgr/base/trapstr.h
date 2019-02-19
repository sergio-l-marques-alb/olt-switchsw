/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename trapstr.c
*
* @purpose Trap Manager String File
*
* @component trapmgr
*
* @comments none
*
* @create 08/31/2000
*
* @author bmutz
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#ifndef TRAPSTR_H
#define TRAPSTR_H

extern const L7_uchar8 *const coldstart_str;
extern const L7_uchar8 *const warmstart_str;
extern const L7_uchar8 *const mylinkdown_str;
extern const L7_uchar8 *const authfail_str;
extern const L7_uchar8 *const linkup_str;
extern const L7_uchar8 *const linkdown_str;
extern const L7_uchar8 *const multiuser_str;
extern const L7_uchar8 *const linkstatus_str;
extern const L7_uchar8 *const securityint_str;
extern const L7_uchar8 *const newroot_str;
extern const L7_uchar8 *const loop_inconsistent_start_str;
extern const L7_uchar8 *const loop_inconsistent_end_str;
extern const L7_uchar8 *const topochange_str;
extern const L7_uchar8 *const rmonevent_str;
extern const L7_uchar8 *const bcaststormexc_str;
extern const L7_uchar8 *const framesdiscardexc_str;
extern const L7_uchar8 *const bcaststorm_start;
extern const L7_uchar8 *const bcaststorm_end;
extern const L7_uchar8 *const general_system;
extern const L7_uchar8 *const vlan_request_fail;
extern const L7_uchar8 *const vlan_delete_last_str;
extern const L7_uchar8 *const vlan_default_cfg_fail_str;
extern const L7_uchar8 *const vlan_restore_fail_str;
extern const L7_uchar8 *const fan_status_str;
extern const L7_uchar8 *const power_supply_status_str;

extern const L7_uchar8 *const dot1q_str;

extern const L7_uchar8 *const rmon_rising_alarm_str;
extern const L7_uchar8 *const rmon_falling_alarm_str;

extern const L7_uchar8 *const failed_login_str;
extern const L7_uchar8 *const user_login_lockout_str;
extern const L7_uchar8 *const mac_lock_violation_str;
extern const L7_uchar8 *const poe_port_change_str;
extern const L7_uchar8 *const lldp_rem_change_str;

extern const L7_uchar8 *const dot1x_unauth_host_str;
extern const L7_uchar8 *const acl_mac_rule_deny_str;
extern const L7_uchar8 *const acl_ip_rule_deny_str;
extern const L7_uchar8 *const configChanged_str;
extern const L7_uchar8 *const tftpEnd_str;
extern const L7_uchar8 *const tftpAbort_str;
extern const L7_uchar8 *const tftpStart_str;
extern const L7_uchar8 *const linkFailure_str;
extern const L7_uchar8 *const vlanDynPortAdded_str;
extern const L7_uchar8 *const vlanDynPortRemoved_str;
extern const L7_uchar8 *const stackMasterFailed_str;
extern const L7_uchar8 *const stackNewMasterElected_str;
extern const L7_uchar8 *const stackMemberUnitFailed_str;
extern const L7_uchar8 *const stackMemberUnitAdded_str;
extern const L7_uchar8 *const stackMemberUnitRemoved_str;
extern const L7_uchar8 *const stackSplitMasterReport_str;
extern const L7_uchar8 *const stackSplitNewMasterReport_str;
extern const L7_uchar8 *const stackRejoined_str;
extern const L7_uchar8 *const stackLinkFailed_str;
extern const L7_uchar8 *const dot1dStpPortStateForwarding_str;
extern const L7_uchar8 *const dot1dStpPortStateNotForwarding_str;
extern const L7_uchar8 *const trunkPortAdded_str;
extern const L7_uchar8 *const trunkPortRemoved_str;
extern const L7_uchar8 *const lockPort_str;
extern const L7_uchar8 *const vlanDynVlanAdded_str;
extern const L7_uchar8 *const vlanDynVlanRemoved_str;
extern const L7_uchar8 *const envMonFanStateChange_str;
extern const L7_uchar8 *const envMonPowerSupplyStateChange_str;
extern const L7_uchar8 *const envMonTemperatureRisingAlarm_str;
extern const L7_uchar8 *const copyFinished_str;
extern const L7_uchar8 *const copyFailed_str;
extern const L7_uchar8 *const dot1xPortStatusAuthorized_str;
extern const L7_uchar8 *const dot1xPortStatusUnauthorized_str;
extern const L7_uchar8 *const stpElectedAsRoot_str;
extern const L7_uchar8 *const stpNewRootElected_str;
extern const L7_uchar8 *const invalidUserLoginAttempted_str;
extern const L7_uchar8 *const managementACLViolation_str;

extern const L7_uchar8 *const sfpInserted_str;
extern const L7_uchar8 *const sfpRemoved_str;
extern const L7_uchar8 *const xfpInserted_str;
extern const L7_uchar8 *const xfpRemoved_str;
extern const L7_uchar8 *const lldp_med_topo_change_str;
extern const L7_uchar8 *const dai_intf_error_disabled_str;
extern const L7_uchar8 *const ds_intf_error_disabled_str;

extern const L7_uchar8 *const temperature_change_alarm_str;
extern const L7_uchar8 *const fan_state_change_alarm_str;
extern const L7_uchar8 *const powsupply_state_change_alarm_str;

extern const L7_uchar8 *const entConfigChanged_str;
#ifdef L7_DOT1AG_PACKAGE
extern const L7_uchar8 *const dot1ag_cfm_defect_notify_str;
#endif /* L7_DOT1AG_PACKAGE */

#ifdef FEAT_METRO_CPE_V1_0
extern L7_char8 no_startup_config_str[];
#endif
#endif /* TRAPSTR_H */
